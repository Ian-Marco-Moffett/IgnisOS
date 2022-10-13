.globl intr_init

intr_init:
  adr x0, vectors
  msr vbar_el1, x0
  ret

.macro ventry label
.align 7
  b \label
.endm

vectors:
  ventry	sync_invalid_el0       // Synchronous 64-bit EL0
  ventry	irq_invalid_el0       // IRQ 64-bit EL0
  ventry	fiq_invalid_el0       // FIQ 64-bit EL0
  ventry	error_invalid_el0     // Error 64-bit EL0



sync_invalid_el0:
  hlt 0

irq_invalid_el0:
  hlt 0

fiq_invalid_el0:
  hlt 0 

error_invalid_el0:
  hlt 0
