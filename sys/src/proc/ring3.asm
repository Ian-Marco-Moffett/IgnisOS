bits 64

global enter_ring3

enter_ring3:
  cli

  cmp byte [sysret_enabled], 1
  je .skip_sysret_enable
  mov byte [sysret_enabled], 1

  mov rcx, 0xC0000082
  wrmsr
  mov rcx, 0xC0000080
  rdmsr
  or eax, 1
  wrmsr
  mov rcx, 0xC0000081
  rdmsr
  mov edx, 0x00180008
  
.skip_sysret_enable:
  mov ax, 0x40 | 3  ;; DS (low bits for RPL).
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  
  mov rcx, rdi
  mov r11, 0x202
  o64 sysret

section .data
sysret_enabled: db 0
