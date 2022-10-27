#include <tty/console.h>
#include <proc/proc.h>
#include <lib/types.h>
#include <uapi/uapi.h>
#include <lib/log.h>
#include <intr/syscall.h>

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

  if (str == NULL)
    return;

  // I don't want it to be that easy to fake
  // a kernel panic (seems like a silly thing to make easy).
  if (str[0] == '\\' && str[1] == '1') {
    return;
  }

  va_list ap;
  console_write(str, ap);
}


/*
 * RBX: Driver ID string.
 * RCX: Command.
 * RDX to R13: Args.
 *
 *
 */

static void sys_ioctl(struct trapframe* tf) { 
  driver_node_t* driver = uapi_locate_driver((const char*)tf->rbx);

  if (driver == NULL) {
    return;
  } 
  
  size_t ioctl_args[20] = {
    tf->rdx, tf->rsi, tf->rdi,
    tf->r8, tf->r9, tf->r10,
    tf->r11, tf->r12, tf->r13
  };

  driver->ioctl(tf->rcx, ioctl_args);

}

/*
 *  Loads a process from the initrd.
 *  RBX: Path.
 *
 */

static void sys_launch(struct trapframe* tf) {
  launch_exec((const char*)tf->rbx);
}


void(*syscall_table[MAX_SYSCALLS])(struct trapframe* tf) = {
  sys_conout,       // 0x0.
  sys_ioctl,        // 0x1.
  sys_launch,       // 0x2.
};
