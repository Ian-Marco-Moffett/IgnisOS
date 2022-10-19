#include <firmware/acpi/acpi.h>
#include <firmware/acpi/def.h>
#include <lib/limine.h>
#include <lib/string.h>
#include <lib/assert.h>

#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2

static const acpi_rsdt_t* rsdt = NULL;
static const acpi_madt_t* madt = NULL;
static uint32_t rsdt_entry_count = 0;
static void* ioapic_base = NULL;
static void* lapic_base = NULL;

static volatile struct limine_rsdp_request rsdp_req = {
  .id = LIMINE_RSDP_REQUEST,
  .revision = 0
};


static void find_madt(void) {
  for (uint32_t i = 0; i < rsdt_entry_count; ++i) {
    acpi_header_t* current = (acpi_header_t*)(uint64_t)rsdt->tables[i];
    if (kstrncmp(current->signature, "APIC", 4) == 0) {
      madt = (acpi_madt_t*)current;
      return;
    }
  }
}


static void parse_madt(void) {
  lapic_base = (void*)(uint64_t)(madt->lapic_addr);
  uint8_t* cur = (uint8_t*)(madt + 1);
  uint8_t* end = (uint8_t*)madt + madt->header.length;

  while (cur < end) {
    apic_header_t* apic_header = (apic_header_t*)cur;
    if (apic_header->type == APIC_TYPE_IO_APIC) {
      io_apic_t* ioapic = (io_apic_t*)cur;
      ioapic_base = (void*)(uint64_t)ioapic->io_apic_addr;
    }

    cur += apic_header->length;
  }
}


static uint8_t do_checksum(const acpi_header_t* header) {
  uint8_t sum = 0;

  for (uint32_t i = 0; i < header->length; ++i) {
    sum += ((char*)header)[i];
  }

  return sum % 0x100 == 0;
} 


void* acpi_get_ioapic_base(void) {
  return ioapic_base;
}

void* acpi_get_lapic_base(void) {
  return lapic_base;
}


uint16_t acpi_remap_irq(uint8_t irq) {
  uint8_t* cur = (uint8_t*)(madt + 1);
  uint8_t* end = (uint8_t*)madt + madt->header.length;

  while (cur < end) {
    apic_header_t* apic_header = (apic_header_t*)cur;
    if (apic_header->type == APIC_TYPE_INTERRUPT_OVERRIDE) {
      apic_interrupt_override_t* intr_override = (apic_interrupt_override_t*)cur;
      if (intr_override->source == irq) {
        return intr_override->interrupt;
      }
    }

    cur += apic_header->length;
  }
  
  return irq;
}


void acpi_init(void) {
  acpi_rsdp_t* rsdp = rsdp_req.response->address;
  rsdt = (acpi_rsdt_t*)(uint64_t)rsdp->rsdtaddr;
  ASSERT(do_checksum(&rsdt->header), "ACPI RSDT checkum is invalid!\n");
  rsdt_entry_count = (rsdt->header.length - sizeof(rsdt->header)) / 4;

  // Fetch and parse MADT.
  find_madt();
  parse_madt();
}

