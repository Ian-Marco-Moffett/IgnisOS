#ifndef ACPI_H_
#define ACPI_H_

#include <lib/types.h>


void acpi_init(void);
void* acpi_get_ioapic_base(void);
void* acpi_get_lapic_base(void);
uint16_t acpi_remap_irq(uint8_t irq);

#endif
