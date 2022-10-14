#include <lib/log.h>
#include <lib/release.h>
#include <drivers/video/framebuffer.h>
#include <intr/init.h>
#include <mm/pmm.h>

#ifdef __x86_64__
#include <arch/x64/idt.h>
#endif

void _start(void) {
#ifdef __x86_64__
  load_idt();
#endif
 
  framebuffer_init();
  intr_init();
  pmm_init();
  printk(RELEASE_TITLE);
  while (1);
}
