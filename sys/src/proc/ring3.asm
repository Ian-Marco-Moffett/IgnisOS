bits 64

global enter_ring3

enter_ring3:
  cli
  
  mov ax, 0x40 | 3
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov rax, rsi    ;; RSP.
  push 0x40 | 3
  push rax
  pushf

  mov rax, [rsp]
  or rax, 1 << 9
  mov [rsp], rax

  push 0x38 | 3
  push rdi        ;; RIP.
  iretq

section .data
sysret_enabled: db 0
