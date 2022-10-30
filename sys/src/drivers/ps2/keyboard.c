#include <drivers/ps2/keyboard.h>
#include <arch/x86/io.h>
#include <arch/x86/apic/lapic.h>

/*
 *  Bits 15-0: Scancode
 *  Bits 23-16: Character
 *  Bit 24: Pressed (1: true, 0: false).
 *
 *  TODO: Buffer keystrokes.
 */
static uint32_t last_keystroke = 0;


ssize_t ps2_keyboard_ioctl(unsigned long cmd, size_t args[20]) {
  switch (cmd) {
    case PS2_KEYBOARD_FETCH_KEY:
      /*
       *  args[0] must be a pointer to a buffer that will
       *  hold the keystroke.
       */
      
      // TODO  fix this.
      return 0;
  }

  return 0;
}


__attribute__((interrupt)) void __irq1_isr(void* stackframe) {
  const char* const SC_ASCII = "\x00\x1B" "1234567890-=" "\x08"
      "\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
      "zxcvbnm,./" "\x00\x00\x00" " ";

  // Read in scancode.
  uint16_t scancode = inportb(0x60);
  char character = SC_ASCII[scancode];
  uint8_t pressed = 0;

  if (!(scancode & 0x80)) {
      if ((SC_ASCII[scancode] >= 'a' && SC_ASCII[scancode] <= 'z') || 
              (SC_ASCII[scancode] >= 'A' && SC_ASCII[scancode] <= 'Z') || 
              (SC_ASCII[scancode] >= '0' && SC_ASCII[scancode]) <= '9' || 
              (SC_ASCII[scancode] >= '!' && SC_ASCII[scancode] <= '.') || 
              (SC_ASCII[scancode] >= '[' && SC_ASCII[scancode] <= '`')) {
            pressed = 1;
 
      }
  }

  last_keystroke = (pressed << 24 | character << 16 | scancode);
  lapic_send_eoi();
}


uint32_t ps2_keyboard_fetch_keystroke(void) {
  uint32_t ret = last_keystroke;
  last_keystroke = 0;
  return ret;
}
