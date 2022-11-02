#include <proc/proc.h>
#include <proc/tss.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/mmap.h>
#include <lib/log.h>
#include <lib/assert.h>
#include <lib/string.h>
#include <intr/syscall.h>
#include <drivers/video/framebuffer.h>

#define PROC_U_STACK_START 0x1000

extern PAGEMAP root_pagemap;
static size_t next_pid = 1;
process_t* process_queue_head = NULL;
process_t* process_queue_base = NULL;
process_t* running_process = NULL;


static inline void fork_pml4(void* frame) {
  kmemcpy((void*)((uint64_t)frame + VMM_HIGHER_HALF), (void*)root_pagemap, 0x1000);
}


__attribute__((naked)) void enter_ring3(uint64_t rip, uint64_t rsp);


static void update_kernel_stack(uint64_t kstack_base) {
  tss->rsp0Low = KSTACK_LOW(KSTACK_START_OFFSET(kstack_base));
  tss->rsp0High = KSTACK_HIGH(KSTACK_START_OFFSET(kstack_base));
}


static void make_process(void) {
  process_t* new_proc = kmalloc(sizeof(process_t));  
  new_proc->pid = next_pid++;

  new_proc->ctx.cr3 = (uint64_t)vmm_make_pml4();
  
  /*
   *  Userstack is at a fixed virtual
   *  address which is mapped to the 
   *  process's address space.
   *
   *  ustack_phys_base: Physical address ustack_base 
   *                    is mapped to.
   *
   */
  new_proc->ctx.ustack_base = PROC_U_STACK_START;
  new_proc->ctx.ustack_phys_base = pmm_alloc_frame();
  vmm_map_page((PAGEMAP*)new_proc->ctx.cr3, new_proc->ctx.ustack_phys_base, new_proc->ctx.ustack_base, PTE_PRESENT | PTE_WRITABLE | PTE_USER);


  /*
   *  Kernel stack is kmalloc'd.
   *
   *  So all we need to do is kmalloc
   *  a page-sized buffer.
   *
   */

  new_proc->ctx.kstack_base = (uint64_t)kmalloc(0x1000);

  /*
   *  Now stuff for the queue.
   *
   *  If we have no queue base,
   *  then that means we are initializing 
   *  proccess management.
   *
   *  Otherwise, just append new_proc
   *  to the queue and things should
   *  be good :)
   *
   *  Some notes:
   *  Upon a task switch, the scheduler
   *  has the responsibility to setup
   *  values like:
   *
   *  tf->k_rsp
   *  (where tf is a trapframe).
   *
   *  And switch to the next process's 
   *  address space (loading CR3) etc (more explained
   *  in the scheduler).
   *
   *
   */

  if (process_queue_base == NULL) {
    process_queue_base = new_proc;
    process_queue_head = new_proc;
    running_process = new_proc;
  } else {
    process_queue_head->next = new_proc;
    process_queue_head = process_queue_head->next;
  }
}


void task_sched(struct trapframe* tf) {
  /*
   *  Alright, so we have gotten IRQ 0
   *  and the trap handler has called task_sched().
   *
   *  task_sched() responsibility:
   *
   *  Copy over the old trapframe to the process
   *  state and switch tasks, check if things need to be done
   *  with the process, change address space and TSS's rsp0.
   *
   *
   */

  kmemcpy((uint8_t*)&running_process->tf, (uint8_t*)tf, sizeof(running_process->tf));

  /*
   *  Switch to the next process
   *  and update tf->k_rsp.
   *
   */

  if (running_process->next != NULL)
    running_process = running_process->next;
  else
    running_process = process_queue_base;

  /*
   *  Copy over this task's trapframe state
   *  to the main trapframe ptr.
   *
   *  We also need to update the TSS's
   *  rsp0 values so we can switch to a 
   *  new kernel stack during an interrupt.
   *
   *
   */

  kmemcpy((uint8_t*)tf, (uint8_t*)&running_process->tf, sizeof(running_process->tf));
  update_kernel_stack(running_process->ctx.kstack_base);
  ASMV("mov %0, %%cr3" :: "a" (running_process->ctx.cr3));

  if (running_process->ctx.framebuffer_mirror != NULL) {
    framebuffer_mirror(running_process->ctx.framebuffer_mirror);
  }
}


void launch_exec(const char* path, pperm_t pmask) {
  if (pmask != 0 && !(running_process->pmask & PPERM_PERM))
    return;

  make_process();
  uint64_t rip = (uint64_t)elf_load(path, &process_queue_head->img);

  update_kernel_stack(process_queue_head->ctx.kstack_base);
  ASMV("mov %0, %%cr3" :: "a" (process_queue_head->ctx.cr3));

  /*
   *  Allocate memory for the process's 
   *  framebuffer mirror if it has access
   *  to the framebuffer.
   *
   */
  if (pmask & PPERM_FRAMEBUFFER) {
    create_process_heap(0x7000, (framebuffer->pitch*framebuffer->height)/PAGE_SIZE, &process_queue_head->ctx.heap_ctx);
    process_queue_head->ctx.framebuffer_mirror = __malloc(framebuffer->pitch*framebuffer->height, &process_queue_head->ctx.heap_ctx);
    
    if (process_queue_head->ctx.framebuffer_mirror != NULL)
      kmemzero(process_queue_head->ctx.framebuffer_mirror, framebuffer->pitch*framebuffer->height);
  } else {
    create_process_heap(0x7000, 3, &process_queue_head->ctx.heap_ctx);
  }

  uint64_t rsp = PROC_U_STACK_START + (0x1000/2);
  running_process = process_queue_head;
  enter_ring3(rip, rsp);
}


void proc_init(void) {
  /*
   *  Make a new process and queue.
   */

  make_process();
  process_queue_base->pmask |= PPERM_INITRD | PPERM_PERM;

  /*
   *  Load initd.sys and setup
   *  RIP and stuff.
   *
   */

  uint64_t rip = (uint64_t)elf_load("initd.sys", &process_queue_base->img);
  ASSERT(rip != 0, "Could not load initd.sys!\n");

  /*
   *  We shouldn't forget
   *  to change the kernel rsp0
   *  in the TSS so we can come 
   *  back to kernelspace via
   *  an interrupt.
   *
   *  Then finally we can switch to
   *  userland.
   *
   */

  update_kernel_stack(process_queue_base->ctx.kstack_base);
 
  /*
   *  Change CR3, setup a syscall shmem
   *  port for this process and change RSP.
   */
  ASMV("mov %0, %%cr3" :: "a" (process_queue_base->ctx.cr3));
  create_process_heap(0xB8000, 1, &process_queue_base->ctx.heap_ctx);

  uint64_t rsp = PROC_U_STACK_START + (0x1000/2);
  
  printk("[INFO]: InitD started..\n");
  enter_ring3(rip, rsp);
  __builtin_unreachable();
}
