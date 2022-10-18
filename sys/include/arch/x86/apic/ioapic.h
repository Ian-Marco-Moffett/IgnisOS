#ifndef IOAPIC_H_
#define IOAPIC_H_

#include <lib/types.h>

#ifdef __x86_64__
void ioapic_init(void);

// Sets an entry in the IO APIC redirection table.
void ioapic_set_entry(uint8_t index, uint64_t data);
#endif   // __x86_64__

#endif   // IOAPIC_H_
