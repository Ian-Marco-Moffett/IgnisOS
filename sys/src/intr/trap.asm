global trap_entry
global trap_sched_exit

extern trap

trap_entry: 
  ;; TRAPNO HERE.
  ;; K_RSP HERE.
  push gs
  push fs
  push r15
  push r14
  push r13
  push r12
  push r11
  push r10
  push r9
  push r8
  push rax
  push rcx
  push rdx
  push rbx
  push rsi
  push rdi

  mov rdi, rsp
  call trap
  
  cmp rax, 0
  jz .trap_end
  
  ;; Must be a task switch
  ;; since we didn't jump to
  ;; .trap_end
  mov rsp, rax

.trap_end:
  pop rdi
  pop rsi
  pop rbx
  pop rdx
  pop rcx
  pop rax
  pop r8
  pop r9
  pop r10
  pop r11
  pop r12
  pop r13
  pop r14
  pop r15
  pop fs
  pop gs
  pop rbp
  add rsp, 8
  iretq

trap_sched_exit:
  mov rsp, rdi
  jmp trap_entry.trap_end
