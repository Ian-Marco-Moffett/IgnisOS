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
#include <proc/proc.h>
#include <firmware/acpi/acpi.h>

#define KHEAP_START 0x1000

#ifdef __x86_64__
#include <arch/x64/idt.h>
#include <arch/x86/apic/ioapic.h>
#include <arch/x86/apic/lapic.h>
#include <drivers/serial.h>
#include <drivers/pit.h>
#endif

static void init_memory_managers(void) {
  pmm_init();
  vmm_init();
  create_heap(KHEAP_START, 1);
}

static void fs_init(void) {
  // This shouldn't happen but if it does I'll know.
  ASSERT(vfs_init() == EXIT_SUCCESS, "Could not initialize VFS - Not enough memory\n");
  tmpfs_init();
}

static void apic_init(void) {
  ioapic_init();
  printk("[INFO]: I/O APIC initialized.\n");
  lapic_init();
  printk("[INFO]: Local APIC initialized for BSP.\n");
}

void _start(void) {
#ifdef __x86_64__
  load_idt();
  serial_init();
#endif
 
  framebuffer_init();
  printk(RELEASE_TITLE);

  intr_init();
  printk("[INFO]: Loaded interrupt service routines.\n");

  init_memory_managers();
  printk("[INFO]: Memory managers initialized.\n"); 
  
  
  fs_init();
  printk("[INFO]: File systems initialized.\n"); 

  acpi_init();
  printk("[INFO]: ACPI tables parsed.\n");

  apic_init();

#ifdef __x86_64__
  init_pit();
  printk("[INFO]: PIT initialized.\n");
#endif

  proc_init();

  __asm__ __volatile__("cli; hlt");
  
  while (1);
}
