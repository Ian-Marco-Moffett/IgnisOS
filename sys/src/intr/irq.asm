bits 64

global __irq0_isr

extern context_switch
extern lapic_send_eoi

__irq0_isr:
  cli
  push rax
  push rdi
  push rsi
  push rdx
  push rcx
  push rbx
  push r8
  call lapic_send_eoi
  pop r8
  pop rbx
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  pop rax
  jmp context_switch
