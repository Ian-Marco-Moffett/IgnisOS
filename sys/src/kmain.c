#include <lib/log.h>
#include <lib/release.h>
#include <lib/assert.h>
#include <drivers/video/framebuffer.h>
#include <intr/init.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/heap.h>
#include <fs/vfs.h>
#include <fs/tmpfs.h>

#define KHEAP_START 0x1000

#ifdef __x86_64__
#include <arch/x64/idt.h>
#endif

static void init_memory_managers(void) {
  pmm_init();
  vmm_init();
  create_heap(KHEAP_START, 2);
}

static void fs_init(void) {
  // This shouldn't happen but if it does I'll know.
  ASSERT(vfs_init() == EXIT_SUCCESS, "Could not initialize VFS - Not enough memory\n");
  tmpfs_init();
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
  
  
  fs_init();
  printk("[INFO]: File systems initialized.\n");

  fcreate("/tmp/eof_file");
  FILE* fp = fopen("/tmp/eof_file");
  ASSERT(fp != NULL, "Could not open /tmp/eof_file");

  char buf;
  fread(fp, &buf, 1);
  printk("Data: %x\n", buf);

  fclose(fp);

  __asm__ __volatile__("cli; hlt");
  
  while (1);
}
