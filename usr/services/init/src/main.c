#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <sys/fb.h>
#include <services.cfg.h>

void main(void) {
  puts(_CLEARSCREEN "Hello from Ring 3!\n");
  /*
  puts(_CLEARSCREEN "-- InitD v0.0.1 --\n");
  for (size_t i = 0; i < SIZEOF_ARRAY(services); ++i) {
    syscall(0x2, services[i]);
  }
  */

  while (1);
}
