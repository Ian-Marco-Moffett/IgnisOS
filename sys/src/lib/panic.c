#include <lib/panic.h>
#include <lib/asm.h>
#include <lib/log.h>


void kpanic(void) {
  ASMV("cli; hlt");
}
