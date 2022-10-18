#ifndef ACPI_H_
#define ACPI_H_


void acpi_init(void);
void* acpi_get_ioapic_base(void);
void* acpi_get_lapic_base(void);

#endif
