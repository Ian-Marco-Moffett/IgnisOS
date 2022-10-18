#ifdef __x86_64__

#include <drivers/serial.h>
#include <arch/x86/io.h>
#include <lib/types.h>
#include <lib/string.h>

#define COM1 0x3F8

static uint8_t faulty = 0;

void serial_init(void) {
  outportb(COM1 + 1, 0x00);    // Disable interrupts
  outportb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outportb(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  outportb(COM1 + 1, 0x00);    //                  (hi byte)
  outportb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
  outportb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outportb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
  outportb(COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
  outportb(COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
  
  // Ensure serial is not faulty.
  if (inportb(COM1 + 0) != 0xAE) {
    faulty = 1;
  } else {
    // Set normal operation mode.
    outportb(COM1 + 4, 0x0F);
  }
}

static uint8_t is_transmit_empty(void) {
  return inportb(COM1 + 5) & 0x20;
}


static void put_char(char c) {
  while (is_transmit_empty() == 0);
  outportb(COM1, c);
}

void serial_write(const char* str) {
  if (faulty)
    return; 

  for (const char* s = str; *s; ++s) {
    put_char(*s);
  }
}

#endif
