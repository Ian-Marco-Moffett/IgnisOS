#include <lib/log.h>
#include <lib/string.h>
#include <tty/console.h>

#ifdef __x86_64__
#include <drivers/serial.h>
#endif


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

void printk(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  console_write(fmt, ap);

#ifdef __x86_64__
  serial_log(fmt, ap);
#endif

  va_end(ap);
}
