#include <proc/proc.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/mmap.h>
#include <lib/log.h>
#include <lib/assert.h>
#include <lib/elf.h>
#include <lib/asm.h>
#include <lib/string.h>

#define PROC_STACK_START 0x1000

extern PAGEMAP root_pagemap;
static process_t* process_queue_head = NULL;
static size_t next_pid = 2;
process_t* process_queue_base = NULL;
process_t* running_process;


static inline void fork_pml4(void* frame) {
  kmemcpy((void*)((uint64_t)frame + VMM_HIGHER_HALF), (void*)root_pagemap, 0x1000);
}


_naked static void spawn_from_rip(void* rip) {
  static process_t* new_proc;
  new_proc = kmalloc(sizeof(process_t));

  if (new_proc == NULL)
    return;

  new_proc->pid = next_pid++;
  new_proc->stack_base = PROC_STACK_START;
  uint64_t rsp = new_proc->stack_base + (PAGE_SIZE/2);
  
  // Fork a new PML4.
  new_proc->cr3 = pmm_alloc_frame();
  fork_pml4((void*)new_proc->cr3);

  ASMV("mov %0, %%cr3" :: "a" (new_proc->cr3));

  if (new_proc->cr3 == 0)
    return;

  mmap((void*)new_proc->stack_base, 1, PROT_READ | PROT_WRITE | PROT_USER);
  new_proc->next = NULL;

  process_queue_head->next = new_proc;
  process_queue_head = new_proc;
  
  ASMV("mov %0, %%rsp" :: "a" (rsp));
  enter_ring3((uint64_t)rip);
  __builtin_unreachable();
}

uint8_t proc_initrd_load(const char* path) { 
  program_image_t unused;
  ASMV("mov %0, %%cr3" :: "a" (root_pagemap));
  void(*rip)(void) = elf_load(path, &unused);
  spawn_from_rip(rip);
  return 0;
}


static void start_init(uint64_t rsp) {
  /*
   *  Load the initd binary, switch stacks
   *  and enter ring 3.
   *
   */
  program_image_t unused;
  void(*initd)(void) = elf_load("initd.sys", &unused);
  
  ASMV("mov %0, %%rsp" :: "r" (rsp) : "memory");
  enter_ring3((uint64_t)initd);

  while (1);
}


void proc_init(void) {
  process_queue_base = kmalloc(sizeof(process_t));
  process_queue_base->pid = 1;

  process_queue_base->stack_base = PROC_STACK_START;

  process_queue_base->cr3 = (uint64_t)vmm_make_pml4();
  ASSERT(process_queue_base->cr3 != 0, "Base process's cr3 is null.\n");
  
  ASMV("mov %0, %%cr3" :: "r" (process_queue_base->cr3));
  
  mmap((void*)PROC_STACK_START, 1, PROT_READ | PROT_WRITE | PROT_USER);

  uint64_t rsp = process_queue_base->stack_base+(PAGE_SIZE/2);

  process_queue_base->next = NULL; 
  process_queue_head = process_queue_base;

  running_process = process_queue_base;
  printk("[INFO]: Base process initialized.\n");
  start_init(rsp);
}
