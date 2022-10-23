bits 64

global context_switch

extern proc_set_rsp
extern proc_get_rsp
extern proc_get_cr3
extern proc_set_rip
extern proc_get_rip
extern proc_next

context_switch:
  mov [_saved_rax], rax
  mov [_saved_rbx], rbx
  mov [_saved_rdx], rdx
  mov [_saved_rcx], rcx
  mov [_saved_rbp], rbp
  mov [_saved_rdi], rdi
  mov [_saved_rsi], rsi
  mov rdi, [rsp]
  call proc_set_rip

  ;; We currently have RSP as
  ;; as the kernel RSP from 
  ;; the task state segment.
  ;;
  ;; We must switch to the user
  ;; RSP before creating
  ;; the register stackframe.
  call proc_get_rsp
  mov rsp, rax

  ;; Create register stackframe.
  push qword [_saved_rax]
  push qword [_saved_rbx]
  push qword [_saved_rdx]
  push qword [_saved_rcx]
  push qword [_saved_rbp]
  push qword [_saved_rdi]
  push qword [_saved_rsi]
  
  ;; Update user RSP.
  mov rdi, rsp
  call proc_set_rsp

  ;; Get next process.
  call proc_next

  ;; Get this processs's RSP
  ;; and restore it's register state
  ;; from the register stackframe.
  ;; 
  ;; BEFORE DOING THIS WE MUST
  ;; CHANGE ADDRESS SPACES.
  call proc_get_cr3
  mov cr3, rax

  call proc_get_rsp
  mov rsp, rax
  pop rsi
  pop rdi
  pop rbp
  pop rcx
  pop rdx
  pop rbx
  pop rax

  ;; TODO: Allow kernel-space tasks.
  ;; we are currently assuming the task
  ;; is operating in ring 3.
  mov [_saved_rax], rax

  mov rax, rsp
  push 0x40 | 3
  push rax
  pushf

  mov rax, [rsp]
  or rax, 1 << 9
  mov[rsp], rax

  push 0x38 | 3
  call proc_get_rip
  push rax

  mov rax, [_saved_rax]
  iretq

section .data
_saved_rax: dq 0
_saved_rbx: dq 0
_saved_rdx: dq 0
_saved_rcx: dq 0
_saved_rbp: dq 0
_saved_rdi: dq 0
_saved_rsi: dq 0
