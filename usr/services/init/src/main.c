#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <sys/fb.h>
#include <services.cfg.h>

void main(void) {
  _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_CLEARSCREEN, 0x808080);
  /*
  for (size_t i = 0; i < SIZEOF_ARRAY(services); ++i) {
    // syscall(0x1, services[i]);
  }
  */

  while (1);
}
