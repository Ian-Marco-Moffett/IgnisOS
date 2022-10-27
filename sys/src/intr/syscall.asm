bits 64

global syscall_entry

extern trap_entry

syscall_entry:
  cli
  mov qword [saved_rsp], rsp
  push 0x80
  push rbp
  push qword [saved_rsp]
  jmp trap_entry

section .data
saved_rsp: dq 0
