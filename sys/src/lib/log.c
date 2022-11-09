#include <lib/log.h>
#include <lib/string.h>
#include <tty/console.h>
#include <arch/cpu/smp.h>
#include <arch/x86/apic/lapic.h>
#include <drivers/serial.h>
#include <sync/mutex.h>


static uint8_t do_screenlog = 1;


static void serial_log(const char* fmt, va_list ap) {
  if (*fmt == '\\') { 
    fmt += 2;
  }

  for (const char* ptr = fmt; *ptr; ++ptr) {
    if (*ptr == '%') {
      ++ptr;

      switch (*ptr) {
        case 'd':
          serial_write(dec2str(va_arg(ap, uint64_t)));
          break;
        case 'x':
          serial_write(hex2str(va_arg(ap, uint64_t)));
          break;
        case 's':
          serial_write(va_arg(ap, char*));
          break;
        case 'c':
          serial_write((char[2]){va_arg(ap, int), 0});
          break;
      }
    } else {
      serial_write((char[2]){*ptr, 0});
    }
  }
}


void log_disable_screenlog(void) {
  do_screenlog = 0;
}


void log_enable_screenlog(void) {
  do_screenlog = 1;
}

void printk(const char* fmt, ...) {
  static uint8_t lock = 0;
  mutex_acquire(&lock);
  va_list ap;

  // Write to the screen if do_screenlog is 1.
  if (do_screenlog || lapic_read_id() == smp_get_bsp_lapic_id()) {
    va_start(ap, fmt);
    console_write(fmt, ap);
    va_end(ap);
  }

  va_start(ap, fmt);
  serial_log(fmt, ap);
  va_end(ap);
  mutex_release(&lock);
}
