#include <sys/syscall.h>


void puts(const char* str) {
  syscall(0x0, str);
}
