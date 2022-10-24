#include <stdio.h>
#include <sys/syscall.h>

void main(void) {
  puts(_CLEARSCREEN "-- IgnisInitD v0.0.1 --\n");
  puts("More is coming to the init daemon soon..\n");
  syscall(0x1, "kesswm.sys");
  while (1);
}
