bits 64

global context_switch
extern proc_switch

context_switch:
  mov [regs.rax], rax
  mov [regs.rbx], rbx
  mov [regs.rcx], rcx
  mov [regs.rdx], rdx
  mov [regs.rdi], rdi
  mov [regs.rsi], rsi
  mov [regs.rbp], rbp
  mov [regs.r8], r8
  mov [regs.r9], r9
  mov [regs.r10], r10
  mov [regs.r11], r11
  mov [regs.r12], r12
  mov [regs.r13], r13
  mov [regs.r14], r14
  mov [regs.r15], r15

  mov rax, [rsp]
  mov [regs.rip], rax

  mov rax, [rsp+(8*2)]
  mov [regs.rflags], rax

  mov rax, [rsp+(8*3)]
  mov [regs.rsp], rax

  mov rdi, regs
  call proc_switch

  push 0x40 | 3         ;; Low bits for RPL.
  push qword [regs.rsp]
  push qword [regs.rflags]
  push 0x38 | 3         ;; CS (low bits for RPL).
  push qword [regs.rip]
  mov rax, [regs.rax]
  mov rbx, [regs.rbx]
  mov rcx, [regs.rcx]
  mov rdx, [regs.rdx]
  mov rdi, [regs.rdi]
  mov rsi, [regs.rsi]
  mov rbp, [regs.rbp]
  mov r8, [regs.r8]
  mov r9, [regs.r9]
  mov r10, [regs.r10]
  mov r11, [regs.r11]
  mov r12, [regs.r12]
  mov r13, [regs.r13]
  mov r14, [regs.r14]
  mov r15, [regs.r15]
  iretq

section .data
regs:
  .rax: dq 0
  .rbx: dq 0
  .rcx: dq 0
  .rdx: dq 0
  .rdi: dq 0
  .rsi: dq 0
  .rbp: dq 0
  .r8: dq 0
  .r9: dq 0
  .r10: dq 0
  .r11: dq 0
  .r12: dq 0
  .r13: dq 0
  .r14: dq 0
  .r15: dq 0
  .rsp: dq 0
  .rip: dq 0
  .rflags: dq 0
