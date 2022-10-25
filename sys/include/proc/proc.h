#ifndef PROC_H_
#define PROC_H_

#include <lib/types.h>


typedef uint16_t pid_t;


typedef struct {
  uint64_t rax;
  uint64_t rbx;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbp;
} regs_t;


typedef struct {
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} iret_sf_t;


typedef struct Process {
  pid_t pid;
  uint64_t stack_base;
  uint64_t cr3;
  iret_sf_t iret_stackframe;
  regs_t regs;
  struct Process* next;
} process_t;


void proc_init(void);
uint8_t proc_initrd_load(const char* path);
void enter_ring3(uint64_t rip);

#endif
