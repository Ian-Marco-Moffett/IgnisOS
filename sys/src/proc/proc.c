#include <proc/proc.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/mmap.h>
#include <lib/log.h>
#include <lib/assert.h>
#include <lib/elf.h>

#define PROC_STACK_START 0x1000

static process_t* process_queue_head = NULL;
process_t* process_queue_base = NULL;
process_t* running_process;


static void start_init(void) {
  /*
   *  Load the initd binary, switch stacks
   *  and enter ring 3.
   *
   */
  program_image_t unused;
  void(*initd)(void) = elf_load("initd.sys", &unused);

  __asm__ __volatile__("mov %0, %%rsp" :: "r" (process_queue_base->rsp) : "memory");
  enter_ring3((uint64_t)initd);

  while (1);
}


void proc_init(void) {
  process_queue_base = kmalloc(sizeof(process_t));
  process_queue_base->pid = 1;

  process_queue_base->stack_base = PROC_STACK_START;
  ASSERT(process_queue_base->stack_base != 0, "Base process's stack base is null.\n");

  process_queue_base->cr3 = (uint64_t)vmm_make_pml4();
  ASSERT(process_queue_base->cr3 != 0, "Base process's cr3 is null.\n");

  __asm__ __volatile__("mov %0, %%cr3" :: "r" (process_queue_base->cr3));
  
  mmap((void*)PROC_STACK_START, 1, PROT_READ | PROT_WRITE | PROT_USER);

  process_queue_base->rsp = process_queue_base->stack_base + (PAGE_SIZE/2); 

  process_queue_base->next = NULL; 
  process_queue_head = process_queue_base;

  running_process = process_queue_base;
  printk("[INFO]: Base process initialized.\n");

  start_init();
}
