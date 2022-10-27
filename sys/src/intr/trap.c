#include <proc/proc.h>
#include <intr/syscall.h>

/*
 *  If this returns a non-zero value
 *  then a task switch has happened
 *  and RSP needs to be switched 
 *  to the return value of this function.
 *
 */

uint64_t trap(struct trapframe* tf) { 
  switch (tf->trapno) {
    case 0x20:
      task_sched(tf);
      return (uint64_t)&tf->rdi;
    case 0x80:
      if (tf->rax >= MAX_SYSCALLS)
        return 0;

      syscall_table[tf->rax](tf);
      break;
  }

  return 0;
}
