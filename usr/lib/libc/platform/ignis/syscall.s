.global syscall
syscall:
  pushq %rbp
  movq %rdi, (a_rdi)
  movq %rsi, (a_rsi)
  movq %rdx, (a_rdx)
  movq %rcx, (a_rcx)
  movq %r8, (a_r8)
  movq %r9, (a_r9)

  movq (a_rdi), %rax
  movq (a_rsi), %rbx
  movq (a_rdx), %rcx
  movq (a_rcx), %rdx
  movq (a_r8), %rsi
  movq (a_r9), %rdi
  movq 16(%rsp), %r8
  movq 24(%rsp), %r9
  movq 32(%rsp), %r10
  movq 40(%rsp), %r11
  movq 48(%rsp), %r12
  movq 56(%rsp), %r13
  int $0x80
  popq %rbp
  retq

.section .data
a_rdi: .quad 0
a_rsi: .quad 0
a_rdx: .quad 0
a_rcx: .quad 0
a_r8: .quad 0
a_r9: .quad 0
