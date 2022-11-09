#include <intr/exceptions.h>
#include <arch/x64/interrupts.h>
#include <arch/x86/apic/lapic.h>
#include <lib/log.h>
#include <lib/types.h>
#include <proc/proc.h>
#include <lib/panic.h>
#include <mm/vmm.h>

#define INTR_END              \
  lapic_send_ipi_all(0x81);   \
  ASMV("cli; hlt");           \
  __builtin_unreachable();

typedef struct {
  uint64_t error_code;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} stackframe_t;


static void dump_state(void* stackframe) {
  struct core* core = proc_get_current_core();
  printk("Exception caused by process with PID of %d on processor with LAPIC ID of %d\n", core->running->pid, core->lapic_id);
  stackframe_t* frame = stackframe;
  printk(PRINTK_PANIC "-- Begin dump of exception stackframe --\n");
  printk(PRINTK_PANIC "ERROR_CODE=%x RIP=%x\n", frame->error_code, frame->rip);
  printk(PRINTK_PANIC "RFLAGS=%x RSP=%x\n", frame->rflags, frame->rsp);
  printk(PRINTK_PANIC "       SS=%x\n", frame->ss);
}



_isr void __vec0(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Divison by zero\n");
  INTR_END;
}


_isr void __vec1(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Debug exception\n");
  INTR_END;
}


_isr void __vec3(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Breakpoint\n");
  INTR_END;
}


_isr void __vec4(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Overflow\n");
  INTR_END;
}


_isr void __vec5(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: BOUND range exceeded\n");
  INTR_END;
}


_isr void __vec6(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Undefined opcode\n");
  INTR_END;
}

_isr void __vec8(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Double fault\n");
  kpanic();
  ASMV("cli; hlt");
  __builtin_unreachable();
}


_isr void __vec10(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Invalid TSS\n");
  INTR_END;
}


_isr void __vec11(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Segment not present\n");
  INTR_END;
}


_isr void __vec12(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Stack segment fault\n");
  INTR_END;
}


_isr void __vec13(void* stackframe) {
  __asm__ __volatile__("cli");
  printk(PRINTK_PANIC "kpanic: Privilege violation\n");
  dump_state(stackframe);
  INTR_END;
}


_isr void __vec14(void* stackframe) {
  __asm__ __volatile__("cli");
  lapic_send_ipi_others(0x82);
  uint64_t cr2;
  ASMV("mov %%cr2, %0" : "=r" (cr2));

  printk(PRINTK_PANIC "kpanic: Memory access violation at %x\n", cr2);
  dump_state(stackframe);
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
