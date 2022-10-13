#include <arch/x64/idt.h>
#include <lib/asm.h>
#include <lib/types.h>

#ifdef __x86_64__
typedef struct {
  uint16_t limit;
  uint64_t base;
} _packed idtr_t;

intr_gate_descriptor_t idt[256];
static idtr_t idtr = {
  .limit = sizeof(intr_gate_descriptor_t) * 256 - 1,
  .base = (uint64_t)&idt
};

void set_desc(uint8_t vector, void* isr, uint16_t type) {
  intr_gate_descriptor_t* desc = &idt[vector];
  uint64_t addr = (uint64_t)isr;
  desc->isr_low16 = addr & 0xFFFF;
  desc->isr_mid16 = (addr >> 16) & 0xFFFF;
  desc->isr_high32 = (addr >> 32);
  desc->cs = 0x28;
  desc->ist = 0;
  desc->zero = 0;
  desc->zero1 = 0;
  desc->zero2 = 0;
  desc->reserved = 0;
  desc->type = type;
  desc->p = 1;
  desc->dpl = 3;
}

void load_idt(void) {
  ASMV("lidt %0" :: "m" (idtr) : "memory");
}
#endif
