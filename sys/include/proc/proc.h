#ifndef PROC_H_
#define PROC_H_

#include <lib/types.h>


typedef uint16_t pid_t;


typedef struct Process {
  pid_t pid;
  uint64_t rsp;
  uint64_t stack_base;
  uint64_t cr3;
  struct Process* next;
} process_t;


void proc_init(void);


#endif
