#include <lib/panic.h>
#include <lib/asm.h>

void kpanic(void) {
  ASMV("cli; hlt");
}
