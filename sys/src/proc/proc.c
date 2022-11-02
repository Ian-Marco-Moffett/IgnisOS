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
#include <arch/cpu/smp.h>

#define PROC_U_STACK_START 0x1000

extern PAGEMAP root_pagemap;
static size_t next_pid = 1;
static struct core* cores = NULL;

static inline void fork_pml4(void* frame) {
  kmemcpy((void*)((uint64_t)frame + VMM_HIGHER_HALF), (void*)root_pagemap, 0x1000);
}


__attribute__((naked)) void enter_ring3(uint64_t rip, uint64_t rsp);


void task_sched(struct trapframe* tf) {
}

void proc_init(void) {
  /*
   *  Allocate memory for core list.
   *
   */
  size_t core_count = smp_get_core_count();
  cores = kmalloc(sizeof(struct core) * core_count);
  ASSERT(cores != NULL, "Could not allocate memory for corelist.\n");

  for (size_t i = 0; i < core_count; ++i) {
    cores[i].index = i;
    cores[i].lapic_id = lapic_id_list[i];
    cores[i].queue_head = NULL;
    cores[i].queue_base = NULL;
    cores[i].running = NULL;
    cores[i].dice = 0;
  }

  ASMV("cli; hlt");
  __builtin_unreachable();
}
