#ifndef PROC_H_
#define PROC_H_

#include <lib/types.h>
#include <lib/asm.h>
#include <ipc/shmem.h>

#define KSTACK_SIZE 0x1000
#define KSTACK_START_OFFSET(stack_base) (stack_base+(KSTACK_SIZE/2))
#define KSTACK_LOW(stack_base) (stack_base & 0xFFFFFFFF)
#define KSTACK_HIGH(stack_base) (stack_base >> 32)


typedef volatile uint16_t pid_t;

struct trapframe {
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbp;
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
  uint64_t trapno;
};


struct context {
  uint64_t cr3;
  uint64_t ustack_base;
  uint64_t ustack_phys_base;
  uint64_t kstack_base;
};

typedef struct Process {
  pid_t pid;
  struct trapframe tf;
  struct context ctx;
  uint64_t* ports[2];
  struct Process* next;
} process_t;


void proc_init(void);
void enter_ring3(uint64_t rip);
void task_sched(struct trapframe* tf);
void launch_exec(const char* path);



extern process_t* process_queue_head;
extern process_t* process_queue_base;
extern process_t* running_process;


#endif
