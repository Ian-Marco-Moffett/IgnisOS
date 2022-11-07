#include <proc/proc.h>
#include <intr/syscall.h>
#include <sync/mutex.h>

static mutex_t lock = 0;


/*
 *  If this returns a non-zero value
 *  then a task switch has happened
 *  and RSP needs to be switched 
 *  to the return value of this function.
 *
 */

uint64_t trap(struct trapframe* tf) { 
  uint64_t ret = 0;
  
  mutex_acquire(&lock);
  switch (tf->trapno) {
    case 0x20:
      timer_isr();
      break;
    case 0x80:
      if (tf->rax >= MAX_SYSCALLS)
        return 0;

      syscall_table[tf->rax](tf);
      break;
    case 0x81:
      ret = __task_sched((uint64_t)tf);
  }
  
  mutex_release(&lock);
  return ret;
}
