bits 64

global __vec_0x81

extern trap_entry

__vec_0x81:
  cli
  push 0x81
  push rbp
  jmp trap_entry
