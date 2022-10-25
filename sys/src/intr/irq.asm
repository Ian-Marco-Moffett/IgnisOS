bits 64

global __irq0_isr

extern context_switch
extern lapic_send_eoi

__irq0_isr:
  cli
  call lapic_send_eoi
  jmp context_switch
