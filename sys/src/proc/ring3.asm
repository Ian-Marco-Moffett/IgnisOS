bits 64

global enter_ring3

enter_ring3:
  cli
  mov ax, 0x40 | 3  ;; DS (low bits for RPL).
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov rax, rsp
  push 0x40 | 3
  push rax
  pushf

  mov rax, [rsp]
  or rax, 1 << 9    ;; Interrupts enable bit.
  mov [rsp], rax

  push 0x38 | 3     ;; CS (low bits for RPL).
  push rdi          ;; RIP (first argument).
  iretq
