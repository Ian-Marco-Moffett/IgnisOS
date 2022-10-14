// #include <lib/types.h>
#include <lib/log.h>
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
  printk("Hello, World!\n");
  while (1);
}
