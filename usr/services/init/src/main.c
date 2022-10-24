#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/syscall.h>
#include <services.cfg.h>

void main(void) {
  puts(_CLEARSCREEN "-- IgnisInitD v0.0.1 --\n");
  puts("More is coming to the init daemon soon..\n");
  
  for (size_t i = 0; i < SIZEOF_ARRAY(services); ++i) {
    syscall(0x1, services[i]);
  }

  while (1);
}
