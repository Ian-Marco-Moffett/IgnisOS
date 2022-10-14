#include <lib/log.h>
#include <tty/console.h>

void printk(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  console_write(fmt, ap);
  va_end(ap);
}
