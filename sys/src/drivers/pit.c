#include <drivers/pit.h>
#include <lib/types.h>
#include <arch/x86/io.h>

void init_pit(void) {
  int divisor = 1193180/PIT_FREQ;
  outportb(0x43, 0x36);
  outportb(0x40, divisor & 0xFF);
  outportb(0x40, divisor >> 8);
}
