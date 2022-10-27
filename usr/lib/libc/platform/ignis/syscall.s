.global syscall
syscall:
  movq %rdi, (0x7000)
  movq %rsi, (0x7000+8)
  movq %rdx, (0x7000+16)
  movq %rcx, (0x7000+24)
  movq %r8, (0x7000+32)
  movq $1, (0x7000+40)
.busyloop:
  cmpq $0, (0x7000)
  jne .busyloop
  ret
