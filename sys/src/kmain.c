#include <lib/types.h>
#ifdef __x86_64__
#include <arch/x64/idt.h>
#endif

void _start(void) {
#ifdef __x86_64__
  load_idt();
#endif
  while (1);
}
