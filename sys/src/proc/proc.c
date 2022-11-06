#include <proc/proc.h>
#include <proc/tss.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/mmap.h>
#include <lib/log.h>
#include <lib/assert.h>
#include <lib/string.h>
#include <lib/rand.h>
#include <intr/syscall.h>
#include <drivers/video/framebuffer.h>
#include <arch/cpu/smp.h>
#include <arch/x86/apic/lapic.h>
#include <arch/x86/gdt.h>
#include <arch/x64/idt.h>
#include <intr/init.h>
#include <sync/mutex.h>
#include <lib/limine.h>

#define PROC_U_STACK_START 0x1000

extern PAGEMAP root_pagemap;
static size_t next_pid = 1;
static struct core* cores = NULL;
static size_t core_count = 0;

static inline void fork_pml4(void* frame) {
  kmemcpy((void*)((uint64_t)frame + VMM_HIGHER_HALF), (void*)root_pagemap, 0x1000);
}

__attribute__((naked)) void enter_ring3(uint64_t rip, uint64_t rsp);

/*
 *  First in orer to choose a core
 *  we must find a core with the least
 *  running tasks, if all of them have 
 *  an equal amount of running tasks then
 *  we will keep rolling a dice until one of 
 *  the cores get the biggest number.
 * 
 *
 */


static void core_roll_dice(struct core* core) {
  core->roll = rand();
}

static void roll_all_dice(void) {
  for (size_t i = 1; i < core_count; ++i) {
    core_roll_dice(&cores[i]);
  }
}

static struct core* core_sched(void) {
  static mutex_t lock = 0;
  mutex_acquire(&lock);
  
  /*
   *  We will check which has the least running
   *  tasks but also prepare each core with a 
   *  dice roll.
   *
   *  where likely is a likely core to 
   *  get the task, this will change a lot.
   *
   *  Roll time or dice rolling time
   *  is when it is time to roll dice.
   */

  uint8_t is_roll_time = 0;
  struct core* likely = NULL;
  for (uint32_t i = 1; i < core_count; ++i) {
    if (likely == NULL)
      likely = &cores[i];

    if (cores[i].n_running_tasks == likely->n_running_tasks) {
      // Welp, it's dice rolling time as we found two cores
      // with an equal amount of tasks running.
      is_roll_time = 1;
    }
  
    if (cores[i].n_running_tasks < likely->n_running_tasks)
      likely = &cores[i];
  }

  /*
   * If there was no roll time (i.e found core with least tasks)
   * then we should return the core with the least tasks, otherwise
   * we will check which roll is the biggest, if all equal somehow 
   * we will keep rolling.
   */
  if (!(is_roll_time)) {
    mutex_release(&lock);
    return likely;
  }

  /*
   *  We will roll a few times.
   *
   */

  for (uint32_t i = 0; i < 3; ++i) {
    roll_all_dice();
  }

  struct core* highest_roller = NULL;
  for (size_t i = 1; i < core_count; ++i) {
    if (highest_roller == NULL)
      highest_roller = &cores[i];

    if (cores[i].roll > highest_roller->roll)
      highest_roller = &cores[i];
  }

  mutex_release(&lock);
  return highest_roller;
}


process_t* get_running_process(void) {
  static mutex_t lock = 0;
  mutex_acquire(&lock);

  uint32_t lapic_id = lapic_read_id();
  for (size_t i = 0; i < core_count; ++i) {
    if (cores[i].lapic_id == lapic_id) {
      mutex_release(&lock);
      return cores[i].running;
    }
  }
  
  mutex_release(&lock);
  return NULL;
}


/*
 *  Returns core that the process is on.
 *
 */

static struct core* make_process(void) {
  static mutex_t lock = 0;
  mutex_acquire(&lock);

  /*
   *  Allocate memory for a new process
   *  and initialize the new process.
   * 
   */
  process_t* new = kmalloc(sizeof(process_t));
  new->pid = next_pid++;
  new->pmask = 0;
  new->next = NULL;
  new->ctx.cr3 = (uint64_t)vmm_make_pml4();
  new->ctx.ustack_base = PROC_U_STACK_START;
  new->ctx.ustack_phys_base = pmm_alloc_frame();
  vmm_map_page((PAGEMAP*)new->ctx.cr3, new->ctx.ustack_phys_base, new->ctx.ustack_base, PTE_PRESENT | PTE_WRITABLE | PTE_USER);

  new->ctx.kstack_base = (uint64_t)kmalloc(0x1000);

  struct core* core = core_sched();
  if (core->queue_base == NULL) {
    core->queue_base = new;
    core->queue_head = new;
    core->running = new;
    core->sleeping = 0;
  }
  
  ++core->n_running_tasks;
  mutex_release(&lock);
  return core;
}


static void init_cores(void) {
  /*
   *  Initialize all core
   *  descriptors.
   */

  for (size_t i = 0; i < core_count; ++i) {
    cores[i].index = i;
    cores[i].lapic_id = lapic_id_list[i];
    cores[i].queue_head = NULL;
    cores[i].queue_base = NULL;
    cores[i].running = NULL;
    cores[i].n_running_tasks = 0;
    cores[i].roll = 0;
    cores[i].sleeping = i == 0 ? 0 : 1;
  }
}
  

void task_sched(struct trapframe* tf) {
  for (size_t i = 0; i < core_count; ++i) {
    if (cores[i].running != NULL) {
      cores[i].running = cores[i].running->next != NULL ? cores[i].running->next : cores[i].queue_base;
    }
  }
}

/*
 *  Sleepy time for a processor.
 *
 *  This is a processors favorite place
 *  to sleep in Ignis :)
 *
 */

static void __processor_idle(void) {
  while (1) {
    ASMV("hlt");
  }
}


static void __processor_startup_routine(struct limine_smp_info* info) {
  struct core* core = (struct core*)info->extra_argument;

  load_idt();
  intr_init();

  load_gdt();
  write_tss();
  load_tss();

  VMM_LOAD_CR3(core->queue_head->ctx.cr3);
  uint64_t rsp = core->queue_head->ctx.ustack_base + (0x1000/2);

  ASMV("cli; hlt");
}


void proc_init(void) {  
  smp_init();

  /*
   *  Allocate memory for core list.
   *
   */
  core_count = smp_get_core_count();
  cores = kmalloc(sizeof(struct core) * core_count);
  ASSERT(cores != NULL, "Could not allocate memory for corelist.\n"); 
  
  // Just make a seed from core_count XOR rand().
  srand(core_count ^ rand());
  init_cores();

  /*
   *  Make new process and load 
   *  CR3.
   */
  struct core* core = make_process();
  printk("[INFO]: Dispatching processor with LAPIC ID %d to start InitD..\n", core->lapic_id);
  smp_goto(core, __processor_startup_routine);

  __processor_idle();
  __builtin_unreachable();
}
