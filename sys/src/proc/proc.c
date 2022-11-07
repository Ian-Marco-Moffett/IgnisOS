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
#include <arch/x64/interrupts.h>
#include <intr/irq.h>
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


static void make_process(struct core* core) {
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

  if (core->queue_base == NULL) {
    core->queue_base = new;
    core->queue_head = new;
    core->running = new;
    core->sleeping = 0;
  }
  
  ++core->n_running_tasks;
  core->sleeping = 0;
  mutex_release(&lock);
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
  VMM_LOAD_CR3(core->queue_head->ctx.cr3);

  load_idt();
  intr_init();
  lapic_init();

  printk("[Processor%d]: IDT loaded, interrupts initialized.\n", info->lapic_id);

  /*
   *  Setup the global descriptor table
   *  for the current processor.
   *
   */

  size_t gdt_size = sizeof(struct GDTDesc) * 11;
  core->gdt = kmalloc(gdt_size);
  core->gdtr = kmalloc(sizeof(struct GDTR));
  kmemcpy((uint8_t*)core->gdt, (uint8_t*)base_gdt, gdt_size);
  core->gdtr->limit = gdt_size;
  core->gdtr->ptr = (uintptr_t)core->gdt;

  /*
   *  Now setup the task state segment
   *  for the current processor.
   */
  core->tss = kmalloc(sizeof(struct TSSEntry));
  kmemzero(core->tss, sizeof(struct TSSEntry));
  
  uint64_t tss_base = (uint64_t)core->tss;
  uint64_t k_rsp = KSTACK_START_OFFSET(core->queue_head->ctx.kstack_base);

  /*
   *  Ring0 stack pointers for
   *  switching stacks on a userland to kernelspace
   *  switch.
   *
   *
   */

  core->tss->rsp0Low = KSTACK_LOW(k_rsp);
  core->tss->rsp0High = KSTACK_HIGH(k_rsp);
  
  /*
   *  Setup the TSS descriptor 
   *  in the global descriptor table.
   *
   */
  struct TSSDescriptor* gdt_tss = (struct TSSDescriptor*)&core->gdt[9];
  gdt_tss->seglimit = sizeof(struct TSSEntry);
  gdt_tss->g = 0;
  gdt_tss->baseAddrLow = tss_base & 0xFFFF;
  gdt_tss->baseAddrMiddle = (tss_base >> 16) & 0xFF;
  gdt_tss->baseAddrOtherMiddle = (tss_base >> 24) & 0xFF;
  gdt_tss->baseAddrUpper = tss_base >> 32;
  gdt_tss->avl = 0;                                           // 0 for TSS.
  gdt_tss->dpl = 0;
  gdt_tss->p = 1;
  gdt_tss->reserved = 0;
  gdt_tss->type = 0x9;
  gdt_tss->zero = 0;

  LGDT(*core->gdtr);
  load_tss();

  /*
   *  Setup the stack pointer
   *  and get the instruction pointer
   *  from loading the binary.
   *
   */

  uint64_t rsp = core->queue_head->ctx.ustack_base + (0x1000/2);
  uint64_t rip = (uint64_t)elf_load(core->queue_base->path, &core->queue_head->img);

  ASSERTF(rip != 0, "Processor with LAPIC ID %d could not\nload base process in it's queue.\n", info->lapic_id);

  // Some info logging.
  printk("[Processor%d]: GDT loaded.\n", info->lapic_id);
  printk("[Processor%d]: TSS loaded.\n", info->lapic_id);
  enter_ring3(rip, rsp);

  ASMV("cli; hlt");
}


static struct core* current_core = NULL;
uint64_t __task_sched(uint64_t k_rsp) {

  /*
   *  Save the trapframe into the running process's copy
   *  of it so we can restore the state later.
   *
   */
  
  current_core->queue_base->k_rsp = 0;

  /*
   *  Alright, now it is time
   *  to switch to the next process.
   */

  if (current_core->running->next != NULL)
    current_core->running = current_core->running->next;
  else
    current_core->running = current_core->queue_base;

  /*
   *  Now we must restore to this
   *  process's state.
   *
   */

  current_core->tss->rsp0Low = KSTACK_LOW(current_core->running->ctx.kstack_base);
  current_core->tss->rsp0High = KSTACK_HIGH(current_core->running->ctx.kstack_base);
  VMM_LOAD_CR3(current_core->running->ctx.cr3);
  return current_core->running->k_rsp;
}


__attribute__((naked)) void __system_halt(void* stackframe) {
  ASMV("cli; hlt");
}


void timer_isr(void) {
  const size_t bsp_lapic_id = smp_get_bsp_lapic_id();

  for (size_t i = 0; i < core_count; ++i) {
    if (!(cores[i].sleeping) && cores[i].lapic_id != bsp_lapic_id) {
      /*
       *  Alright, so the boostrap processor (BSP)
       *  has gotten IRQ 0 (Interrupt Vector 0x20).
       *
       *  The BSP's job is to send an interprocessor interrupt (IPI)
       *  to all active cores that fires interrupt vector 0x81.
       *
       *  This vector sends the target processor
       *  to __task_sched()
       *
       */
      
      current_core = &cores[i];
      lapic_send_ipi(cores[i].lapic_id, 0x81);
    }
  }
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
  struct core* core = core_sched();
  make_process(core);

  /*
   *  Name the process as we will
   *  need the path for setting it up 
   *  and loading it's binary.
   *
   */

  core->queue_base->path = "initd.sys";
  
  /*
   *  Put some logging information 
   *  for debugging.
   */

  printk("[INFO]: Waking up processor with LAPIC ID %d..\n", core->lapic_id);
  printk("[INFO]: Dispatching Application Processor with LAPIC ID %d to start InitD..\n", core->lapic_id);

  /*
   *  Disable video logging and
   *  dispatch the processor.
   */

  log_disable_screenlog();
  smp_goto(core, __processor_startup_routine);
 
  for (uint32_t i = 0; i < 20; ++i) {
    CLI_SLEEP;
  }

  ASMV("sti");
  
  __processor_idle();
  __builtin_unreachable();
}
