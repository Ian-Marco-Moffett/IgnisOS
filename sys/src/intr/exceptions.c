#include <intr/exceptions.h>
#include <arch/x64/interrupts.h>
#include <lib/log.h>
#include <lib/types.h>

#define INTR_END    \
  ASMV("cli; hlt"); \
  __builtin_unreachable();


_isr void __vec0(void* stackframe) {
  printk(PRINTK_PANIC "Divison by zero.\n");
  INTR_END;
}


_isr void __vec1(void* stackframe) {
  printk(PRINTK_PANIC "Debug exception.\n");
  INTR_END;
}


_isr void __vec3(void* stackframe) {
  printk(PRINTK_PANIC "Breakpoint.\n");
  INTR_END;
}


_isr void __vec4(void* stackframe) {
  printk(PRINTK_PANIC "Overflow.\n");
  INTR_END;
}


_isr void __vec5(void* stackframe) {
  printk(PRINTK_PANIC "BOUND range exceeded.\n");
  INTR_END;
}


_isr void __vec6(void* stackframe) {
  printk(PRINTK_PANIC "Undefined opcode.\n");
  INTR_END;
}

_isr void __vec8(void* stackframe) {
  printk(PRINTK_PANIC "Double fault.\n");
  ASMV("cli; hlt");
  __builtin_unreachable();
}


_isr void __vec10(void* stackframe) {
  printk(PRINTK_PANIC "Invalid TSS.\n");
  INTR_END;
}


_isr void __vec11(void* stackframe) {
  printk(PRINTK_PANIC "Segment not present.\n");
  INTR_END;
}


_isr void __vec12(void* stackframe) {
  printk(PRINTK_PANIC "Stack segment fault.\n");
  INTR_END;
}


_isr void __vec13(void* stackframe) {
  printk(PRINTK_PANIC "Privilege violation.\n");
  INTR_END;
}


_isr void __vec14(void* stackframe) {
  printk(PRINTK_PANIC "Memory access violation.\n");
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
