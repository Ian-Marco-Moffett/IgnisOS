#include <lib/log.h>
#include <lib/release.h>
#include <drivers/video/framebuffer.h>
#include <intr/init.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#ifdef __x86_64__
#include <arch/x64/idt.h>
#endif

static void init_memory_managers(void) {
  pmm_init();
  vmm_init();
}

void _start(void) {
#ifdef __x86_64__
  load_idt();
#endif
 
  framebuffer_init();
  printk(RELEASE_TITLE);

  intr_init();
  printk("[INFO]: Loaded interrupt service routines.\n");

  init_memory_managers();
  printk("[INFO]: Memory managers initialized.\n");
  while (1);
}
