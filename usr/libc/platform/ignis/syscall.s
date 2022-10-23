.global syscall
syscall:
  movq %rdi, %rax
  movq %rsi, %rbx
  movq %rdx, %rcx
  movq %rcx, %rdx
  movq %r8, %rsi
  movq %r9, %rdi
  movq 8(%rsp),%r8
  int $0x80
  ret
