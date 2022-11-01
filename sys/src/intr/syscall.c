#include <tty/console.h>
#include <proc/proc.h>
#include <lib/types.h>
#include <uapi/uapi.h>
#include <lib/log.h>
#include <lib/panic.h>
#include <intr/syscall.h>
#include <mm/vmm.h>
#include <fs/initrd.h>
#ifdef __IGNIS_LIVE_ISO
#include <drivers/hdd/ata.h>
#endif

const uint16_t g_SYSCALL_COUNT = MAX_SYSCALLS;


/*
 *  Syscall arguments:
 *
 *  RAX
 *  RBX
 *  RCX
 *  RDX
 *  RSI
 *  RDI
 *  R8
 *  R9
 *  R10
 *  R11
 *  R12
 *  R13
 * 
 *
 */


/*
 *  SYSCALL 0x0.
 *  RBX: STR.
 *
 *
 */

static void sys_conout(struct trapframe* tf) {
  const char* str = (const char*)tf->rbx;
  
  /*
   *  Deny if it is out of program range.
   *
   */

  if ((uint64_t)str < running_process->img.prog_start || ((uint64_t)str > running_process->img.prog_end)) {
    return;
  }

  va_list ap;
  console_write(str, ap);
}


/*
 * RBX: Driver ID.
 * RCX: Command.
 * RDX to R13: Args.
 *
 *
 */

static void sys_ioctl(struct trapframe* tf) {
  size_t ioctl_args[20] = {
    tf->rdx, tf->rsi, tf->rdi,
    tf->r8, tf->r9, tf->r10,
    tf->r11, tf->r12, tf->r13
  };

  switch (tf->rbx) {
    case DRIVER_FRAMEBUFFER:
      tf->rax = framebuffer_ioctl(tf->rcx, ioctl_args);
      break;
  } 
}

/*
 *  Loads a process from the initrd.
 *  RBX: Path.
 *
 */

static void sys_launch(struct trapframe* tf) {
  launch_exec((const char*)tf->rbx);
}

/*
 *  TODO: Get rid of this later.
 *  NOTE: Keep at last syscall entry.
 *
 *
 */

#ifdef __IGNIS_LIVE_ISO
static void sys_disk_install(struct trapframe* tf) {
  size_t size;
  const char* system = initrd_open("Ignis.iso", &size);

  if (system == NULL || size == 0) {
    printk(PRINTK_PANIC "kpanic: %s failure.\n", __func__);
    kpanic();
  }
  
  ata_write((uint16_t*)system, 0, size);
}
#endif


void(*syscall_table[MAX_SYSCALLS])(struct trapframe* tf) = {
  sys_conout,       // 0x0.
  sys_ioctl,        // 0x1.
  sys_launch,       // 0x2.
#ifdef __IGNIS_LIVE_ISO
  sys_disk_install, // 0x3.
#endif
};
