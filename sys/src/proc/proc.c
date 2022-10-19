#include <proc/proc.h>
#include <mm/heap.h>
#include <lib/log.h>
#include <lib/assert.h>
#include <lib/elf.h>

#define STACK_SIZE_PER_PROCCESS 0x500

static process_t* process_queue_head = NULL;
process_t* process_queue_base = NULL;
process_t* running_process;


static void start_init(void) {
  program_image_t unused;
  void(*initd)(void) = elf_load("initd.sys", (void*)process_queue_base->cr3, &unused);
}


void proc_init(void) {
  process_queue_base = kmalloc(sizeof(process_t));
  process_queue_base->pid = 1;
  process_queue_base->stack_base = (uint64_t)kmalloc(STACK_SIZE_PER_PROCCESS);
  ASSERT(process_queue_base->stack_base != 0, "Base process's stack base is null.\n");
  process_queue_base->rsp = process_queue_base->stack_base + (STACK_SIZE_PER_PROCCESS/2);
  process_queue_base->cr3 = (uint64_t)vmm_make_pml4();
  ASSERT(process_queue_base->cr3 != 0, "Base process's cr3 is null.\n");
  process_queue_base->next = NULL; 

  process_queue_head = process_queue_base;
  running_process = process_queue_base;
  printk("[INFO]: Base process initialized.\n");
  start_init();
}
