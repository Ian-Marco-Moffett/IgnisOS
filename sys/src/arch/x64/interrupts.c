#include <arch/x64/idt.h>
#include <arch/x86/apic/ioapic.h>
#include <firmware/acpi/acpi.h>

#ifdef __x86_64__

void register_trap(uint8_t vector, void(*isr)(void* stackframe)) {
  set_desc(vector, isr, TRAP_GATE_FLAGS);
}


void register_user_int(uint8_t vector, void(*isr)(void* stackframe)) {
  set_desc(vector, isr, IDT_INT_GATE_USER);
}


void register_irq(uint8_t irq, uint8_t vector, void(*isr)(void* stackframe), uint64_t extra_redentry_data) {
  set_desc(vector, isr, INT_GATE_FLAGS);
  ioapic_set_entry(acpi_remap_irq(irq), vector | extra_redentry_data);
}

void register_int(uint8_t vector, void(*isr)(void* stackframe)) {
  set_desc(vector, isr, INT_GATE_FLAGS);
}

#endif    // __x86_64__
