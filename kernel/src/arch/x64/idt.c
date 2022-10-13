#include <arch/x64/idt.h>
#include <lib/asm.h>
#include <lib/types.h>

#if defined(__x86_64__)
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

typedef struct {
  uint16_t limit;
  uint64_t base;
} _packed idtr_t;

static intr_gate_descriptor_t idt[256];
static idtr_t idtr = {
  .limit = sizeof(intr_gate_descriptor_t) * 256 - 1,
  .base = (uint64_t)&idt
};

void load_idt(void) {
  ASMV("lidt %0" :: "m" (idtr) : "memory");
}
#endif
