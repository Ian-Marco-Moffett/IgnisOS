bits 64

global context_switch

extern proc_set_rsp
extern proc_get_rsp
extern proc_get_cr3
extern proc_set_rip
extern proc_get_rip
extern proc_next
extern proc_switch

context_switch:
  mov [regs.rax], rax
  mov [regs.rbx], rbx
  mov [regs.rdx], rdx
  mov [regs.rcx], rcx
  mov [regs.rdi], rdi
  mov [regs.rsi], rsi
  mov [regs.rbp], rbp 
 
  mov rdi, iret_stackframe
  mov rsi, rsp
  mov rcx, 5
  rep movsq

  mov rax, [regs.rax]
  mov rbx, [regs.rbx]
  mov rdx, [regs.rdx]
  mov rcx, [regs.rcx]
  mov rdi, [regs.rdi]
  mov rsi, [regs.rsi]
  mov rbp, [regs.rbp]

  mov rsp, iret_stackframe
  iretq

section .data
regs:
  .rax dq 0
  .rbx dq 0
  .rdx dq 0
  .rcx dq 0
  .rdi dq 0
  .rsi dq 0
  .rbp dq 0

section .bss
iret_stackframe: resq 6
