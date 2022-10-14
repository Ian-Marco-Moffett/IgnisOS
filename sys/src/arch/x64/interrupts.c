#include <arch/x64/idt.h>

#ifdef __x86_64__

void register_trap(uint8_t vector, void(*isr)(void* stackframe)) {
  set_desc(vector, isr, TRAP_GATE_FLAGS);
}


void register_user_int(uint8_t vector, void(*isr)(void* stackframe)) {
  set_desc(vector, isr, IDT_INT_GATE_USER);
}

#endif    // __x86_64__
