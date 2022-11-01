#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <sys/fb.h>
#include <services.cfg.h>

void main(void) {
#ifdef __IGNIS_LIVE_ISO
  puts(_CLEARSCREEN "-- InitD v0.0.1 --\n");
  puts("Installing Ignis..\n");
  syscall(0x3);
  puts("Done installing, you may restart!\n");
  while (1);
#endif

  /*
  for (size_t i = 0; i < SIZEOF_ARRAY(services); ++i) {
    syscall(0x2, services[i]);
  }
  */

  while (1);
}
