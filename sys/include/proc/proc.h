#ifndef PROC_H_
#define PROC_H_

#include <lib/types.h>
#include <lib/asm.h>
#include <ipc/shmem.h>


typedef uint16_t pid_t;


typedef struct {
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbp;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t rsp;
  uint64_t rip;
  uint64_t rflags;
} regs_t;


typedef struct {
  uint64_t* vaddr;
} pport_t;

typedef struct Process {
  pid_t pid;
  uint64_t stack_base;
  uint64_t cr3;
  pport_t ports[2];
  regs_t regs;
  struct Process* next;
} process_t;


void proc_init(void);
uint8_t proc_initrd_load(const char* path);
void enter_ring3(uint64_t rip);
_noreturn void sched_main(void* rip, uint64_t rflags);

#endif
