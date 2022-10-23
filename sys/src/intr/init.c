#include <intr/init.h>
#include <arch/x64/interrupts.h>
#include <intr/irq.h>
#include <intr/exceptions.h>


void dispatch_syscall(void* stackframe);

void intr_init(void) {
  init_exceptions();
  register_irq(0, 0x20, __irq0_isr);
  register_user_int(0x80, dispatch_syscall);
}
