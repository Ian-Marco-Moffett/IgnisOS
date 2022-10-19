#include <intr/init.h>
#include <arch/x64/interrupts.h>
#include <intr/irq.h>
#include <intr/exceptions.h>

void intr_init(void) {
  init_exceptions();
  register_irq(0, 0x20, __irq0_isr);
}
