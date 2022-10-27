bits 64

global __irq0_isr

extern context_switch
extern lapic_send_eoi
extern trap_entry

__irq0_isr:
  cli
  push rbp
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
  pop rbp
  
  mov qword [saved_rsp], rsp
  push 0x20
  push rbp
  push qword [saved_rsp]
  jmp trap_entry

section .data
saved_rsp: dq 0
