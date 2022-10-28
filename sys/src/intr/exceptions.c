#include <intr/exceptions.h>
#include <arch/x64/interrupts.h>
#include <lib/log.h>
#include <lib/types.h>
#include <proc/proc.h>
#include <lib/panic.h>

#define INTR_END    \
  ASMV("cli; hlt"); \
  __builtin_unreachable();



_isr void __vec0(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Divison by zero (PID %d)\n", running_process->pid);
  INTR_END;
}


_isr void __vec1(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Debug exception (PID %d).\n", running_process->pid);
  INTR_END;
}


_isr void __vec3(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Breakpoint (PID %d).\n", running_process->pid);
  INTR_END;
}


_isr void __vec4(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Overflow (PID %d).\n", running_process->pid);
  INTR_END;
}


_isr void __vec5(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: BOUND range exceeded (PID %d).\n", running_process->pid);
  INTR_END;
}


_isr void __vec6(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Undefined opcode (PID %d).\n", running_process->pid);
  INTR_END;
}

_isr void __vec8(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Double fault (PID %d)\n", running_process->pid);
  kpanic();
  ASMV("cli; hlt");
  __builtin_unreachable();
}


_isr void __vec10(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Invalid TSS (PID %d)\n", running_process->pid);
  INTR_END;
}


_isr void __vec11(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Segment not present (PID %d).\n", running_process->pid);
  INTR_END;
}


_isr void __vec12(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Stack segment fault (PID %d).\n", running_process->pid);
  INTR_END;
}


_isr void __vec13(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Privilege violation (PID %d).\n", running_process->pid);
  INTR_END;
}


_isr void __vec14(void* stackframe) {
  __asm__ __volatile__("cli");
  uint64_t cr2;
  ASMV("mov %%cr2, %0" : "=r" (cr2));
  printk(PRINTK_PANIC "kpanic: Memory access violation at %x(PID %d)\n", cr2, running_process->pid);
  printk(PRINTK_PANIC "error code: %x\n", (*(uint32_t*)stackframe));
  INTR_END;
}


void init_exceptions(void) {
  register_trap(0, __vec0);
  register_trap(1, __vec1);
  register_trap(3, __vec3);
  register_trap(4, __vec5);
  register_trap(5, __vec5);
  register_trap(6, __vec6);
  register_trap(8, __vec8);
  register_trap(10, __vec10);
  register_trap(11, __vec11);
  register_trap(12, __vec12);
  register_trap(13, __vec13);
  register_trap(14, __vec14);
}
