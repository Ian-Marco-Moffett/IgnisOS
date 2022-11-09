bits 64

global __vec_0x81
global __vec_0x83
global __vec_0x84

extern trap_entry

;; Timer IPI.
__vec_0x81:
  cli
  push 0x81
  push rbp
  jmp trap_entry


;; Yield.
__vec_0x83:
  cli
  push 0x83
  push rbp
  jmp trap_entry
