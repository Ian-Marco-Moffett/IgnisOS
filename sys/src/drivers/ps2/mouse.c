#include <drivers/ps2/mouse.h>
#include <arch/x86/io.h>
#include <arch/x64/interrupts.h>
#include <intr/irq.h>

/*
static uint8_t irq_allowed = 0;


void ps2_mouse_ioctl(unsigned long cmd, size_t args[20]) {
  switch (cmd) {
    case PS2_MOUSE_IOCTL_ENABLE:
      if (irq_allowed)
        return;

      register_irq(12, 0x2C, __irq12_isr);
      irq_allowed = 1;

  }
}

static void wait(uint8_t type) {
  uint32_t timeout = 100000;

  if (!(type)) {
    while (timeout--) {
      if (inportb(0x64) & (1 << 0)) {
        return;
      }
    }
  } else {
    while(timeout--) {
      if (!(inportb(0x64) & (1 << 1))) {
        return;
      }
    }
  }
}


static void write(uint8_t byte) {
  wait(1);
  outportb(0x64, 0xD4);
  
  wait(1);
  outportb(0x60, byte);
}


static uint8_t read(void) {
  wait(0);
  return inportb(0x60);
}


void ps2_mouse_init(void) {
  // Enable the mouse.
  wait(1);
  outportb(0x64, 0xAB);

  // Enable interrupts.
  wait(1);
  outportb(0x64, 0x20);

  wait(0);
  uint8_t status = inportb(0x60) | (1 << 1);

  wait(1);
  outportb(0x64, 0x60);

  wait(1);
  outportb(0x60, status);
  
  // Use default settings.
  write(0xF6);
  read();         // ACK.
  
  // Tell the mouse to 
  // stream events.
  write(0xF4);
  read();       // ACK.

}
*/
