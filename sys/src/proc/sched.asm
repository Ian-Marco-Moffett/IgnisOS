bits 64

global context_switch

extern proc_set_rsp
extern proc_get_rsp
extern proc_get_cr3
extern proc_next

context_switch:
  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push rbp
  mov rdi, rsp
  call proc_set_rsp

  call proc_next

  call proc_get_cr3
  mov cr3, rax

  call proc_get_rsp
  mov rsp, rax

  pop rbp
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax
  iretq

section .data
_rax: dq 0
