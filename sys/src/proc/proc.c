#include <proc/proc.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/mmap.h>
#include <lib/log.h>
#include <lib/assert.h>
#include <lib/elf.h>
#include <lib/string.h>
#include <intr/syscall.h>

#define PROC_STACK_START 0x1000

extern PAGEMAP root_pagemap;
static size_t next_pid = 1;
process_t* process_queue_head = NULL;
process_t* process_queue_base = NULL;
process_t* running_process = NULL;


static inline void fork_pml4(void* frame) {
  kmemcpy((void*)((uint64_t)frame + VMM_HIGHER_HALF), (void*)root_pagemap, 0x1000);
}



static process_t* make_process(void) {
  process_t* new_proc = kmalloc(sizeof(process_t));
  new_proc->pid = next_pid++;

  new_proc->stack_base = PROC_STACK_START;

  new_proc->cr3 = (uint64_t)vmm_make_pml4();
  new_proc->next = NULL; 
  ASSERT(new_proc->cr3 != 0, "Process's cr3 is null.\n");
  
  ASMV("mov %0, %%cr3" :: "r" (new_proc->cr3));
  mmap((void*)PROC_STACK_START, 1, PROT_READ | PROT_WRITE | PROT_USER);
  new_proc->regs.rsp = new_proc->stack_base + (PAGE_SIZE/2);

  return new_proc;
}


void proc_set_rsp(uint64_t rsp) {
  running_process->regs.rsp = rsp;
}


_naked static void spawn_from_rip(void* rip, uint8_t push_task) { 
  process_t* new_proc = make_process(); 
  shmem_make_port(SPT_SYSCALL, 0x7000);
  ASMV("mov %0, %%rsp" :: "r" (new_proc->regs.rsp) : "memory");

  if (push_task) {
    process_queue_head->next = new_proc;
    process_queue_head = new_proc;
    new_proc->regs.rflags = 0x202;
    new_proc->regs.rip = (uint64_t)rip;
  }
  
  /*
   *  Make a syscall port
   *  so this process
   *  can make requests
   *  to the kernel.
   */
  enter_ring3((uint64_t)rip);
  __builtin_unreachable();
}

uint8_t proc_initrd_load(const char* path) { 
  program_image_t unused;
  ASMV("mov %0, %%cr3" :: "a" (root_pagemap));
  void(*rip)(void) = elf_load(path, &unused);
  spawn_from_rip(rip, 1);
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
  spawn_from_rip(initd, 0); 
  while (1);
}


void proc_switch(regs_t* regs) {
  kmemcpy((uint8_t*)&running_process->regs, (uint8_t*)regs, sizeof(running_process->regs));

  // TODO: Allow SPT_GENERAL usage.
  if (running_process->ports[SPT_SYSCALL].vaddr != NULL) {
    uint64_t* port_shmem = running_process->ports[SPT_SYSCALL].vaddr;
    if (*port_shmem != 0 && *port_shmem < MAX_SYSCALLS+1 && port_shmem[5] == 1) {
      syscall_table[*port_shmem - 1](port_shmem);
      *port_shmem = 0;
    }

  }

  if (running_process->next)
    running_process = running_process->next;
  else
    running_process = process_queue_base;

  kmemcpy((uint8_t*)regs, (uint8_t*)&running_process->regs, sizeof(running_process->regs));
}


void proc_init(void) { 
  process_t* init = make_process();
  process_queue_base = init;
  process_queue_head = init;
  running_process = init;
  uint64_t rsp = init->stack_base + (PAGE_SIZE/2);
  start_init(rsp);
}
