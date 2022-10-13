#ifndef IDT_H_
#define IDT_H_

#include <lib/types.h>

#ifdef __x86_64__

#define TRAP_GATE_FLAGS 0x8F
#define INT_GATE_FLAGS 0x8E
#define IDT_INT_GATE_USER 0xEE

typedef struct {
  uint16_t isr_low16;
  uint16_t cs;
  uint8_t ist : 2;
  uint8_t zero : 1;
  uint8_t zero1 : 3;
  uint8_t type : 4;
  uint8_t zero2 : 1;
  uint8_t dpl : 2;
  uint8_t p : 1;
  uint16_t isr_mid16;
  uint32_t isr_high32;
  uint32_t reserved;
} intr_gate_descriptor_t;

void load_idt(void);
void set_desc(uint8_t vector, void* isr, uint16_t type);

#endif        // __x86_64__
#endif        // IDT_H_
