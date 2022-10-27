#include <proc/proc.h>
#include <intr/syscall.h>

void trap(struct trapframe* tf) { 
  switch (tf->trapno) {
    case 0x20:
      task_sched(tf);
      break;
    case 0x80:
      if (tf->rax >= MAX_SYSCALLS)
        return;

      syscall_table[tf->rax](tf);
      break;
  }
}
