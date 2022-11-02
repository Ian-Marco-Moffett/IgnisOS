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
#include <sync/mutex.h>

#define PROC_U_STACK_START 0x1000

extern PAGEMAP root_pagemap;
static size_t next_pid = 1;
static struct core* cores = NULL;
static size_t core_count;

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


void task_sched(struct trapframe* tf) {
}

void proc_init(void) {
  /*
   *  Allocate memory for core list.
   *
   */
  core_count = smp_get_core_count();
  cores = kmalloc(sizeof(struct core) * core_count);
  ASSERT(cores != NULL, "Could not allocate memory for corelist.\n");
  
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
  }
  
  // Just make a seed from core_count XOR rand().
  srand(core_count ^ rand());

  printk("CORE CHOSEN => %d\n", core_sched()->lapic_id);
  printk("CORE CHOSEN => %d\n", core_sched()->lapic_id);
  printk("CORE CHOSEN => %d\n", core_sched()->lapic_id);
  printk("CORE CHOSEN => %d\n", core_sched()->lapic_id);
  printk("CORE CHOSEN => %d\n", core_sched()->lapic_id);
  printk("CORE CHOSEN => %d\n", core_sched()->lapic_id);

  ASMV("cli; hlt");
  __builtin_unreachable();
}
