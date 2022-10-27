.global syscall
syscall:
  mov %rdi, (a_rdi)
  mov %rsi, (a_rsi)
  mov %rdx, (a_rdx)
  mov %rcx, (a_rcx)
  mov %r8, (a_r8)
  mov %r9, (a_r9)

  mov (a_rdi), %rax
  mov (a_rsi), %rbx
  mov (a_rdx), %rcx
  mov (a_rcx), %rdi
  mov (a_r8), %rsi
  mov (a_r9), %r8
  int $0x80
  retq

.section .data
a_rdi: .quad 0
a_rsi: .quad 0
a_rdx: .quad 0
a_rcx: .quad 0
a_r8: .quad 0
a_r9: .quad 0
