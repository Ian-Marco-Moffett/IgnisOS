.global syscall
syscall:
  movq %rdi, %rax
  movq %rsi, %rbx
  pushq %rcx
  movq %rdx, %rcx
  popq %rdx
  movq %r8, %rsi
  movq %r9, %rdi
  movq 8(%rsp),%r8
  movq 16(%rsp),%r9
  movq 24(%rsp),%r10
  int $0x80
  ret
