#ifndef PROC_H_
#define PROC_H_

#include <lib/types.h>
#include <lib/asm.h>
#include <lib/elf.h>
#include <mm/heap.h>
#include <proc/perm.h>
#include <proc/tss.h>
#include <arch/x86/gdt.h>

#define KSTACK_SIZE 0x1000
#define KSTACK_START_OFFSET(stack_base) (stack_base+(KSTACK_SIZE/2))
#define KSTACK_LOW(stack_base) (stack_base & 0xFFFFFFFF)
#define KSTACK_HIGH(stack_base) (stack_base >> 32)


typedef volatile uint16_t pid_t;


struct trapframe {
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbx;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rax;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  
  uint64_t fs;
  uint64_t gs;

  // Other information.
  uint64_t k_rsp;
  uint64_t rbp;
  uint64_t trapno;
};


struct context {
  uint64_t cr3;
  uint64_t ustack_base;
  uint64_t ustack_phys_base;
  uint64_t kstack_base;
  void* framebuffer_mirror;
  struct heap_context heap_ctx;
};

typedef struct Process {
  pid_t pid;
  const char* path;
  pperm_t pmask;          // Permissions mask.
  struct trapframe tf;
  struct context ctx;
  program_image_t img;
  struct Process* next;
} process_t;

struct core {
  uint32_t index;
  uint32_t lapic_id;
  size_t n_running_tasks;
  process_t* queue_head;
  process_t* queue_base;
  process_t* running;
  struct GDTR* gdtr;
  struct GDTDesc* gdt;
  struct TSSEntry* tss;
  uint8_t sleeping : 1;
  // For scheduling.
  uint32_t roll;
};


void proc_init(void);
void task_sched(struct trapframe* tf);
void launch_exec(const char* path, pperm_t pmask);

/*
 *  Returns the currently running process
 *  on the core that executes this function.
 *
 */

process_t* get_running_process(void);

/*
 *  Returns instruction pointer.
 *
 */

uint64_t fetch_rip(void);
void __sched_init(void);

#endif
