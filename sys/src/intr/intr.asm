bits 64

global __vec_0x81

extern trap_entry

__vec_0x81:
  mov [saved_rsp], rsp
  push 0x81
  push rbp
  push qword [saved_rsp]
  jmp trap_entry

section .data
saved_rsp: dq 0
