bits 64

global __irq0_isr

extern context_switch

__irq0_isr:
  cli
  jmp context_switch
