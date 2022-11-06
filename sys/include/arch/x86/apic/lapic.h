#ifndef LAPIC_H_
#define LAPIC_H_

#include <lib/types.h>

#ifdef __x86_64__
void lapic_init(void);
void lapic_send_eoi(void);
uint32_t lapic_read_id(void);

// Sends IPI to all processors except self.
void lapic_send_ipi_all(uint8_t vector);

// Sends IPI to specific processor.
void lapic_send_ipi(uint8_t apic_id, uint8_t vector);
#endif // __x86_64__

#endif // LAPIC_H_
