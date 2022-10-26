#include <tty/console.h>
#include <proc/proc.h>
#include <lib/types.h>
#include <uapi/uapi.h>
#include <lib/log.h>
#include <intr/syscall.h>

const uint16_t g_SYSCALL_COUNT = MAX_SYSCALLS;


/*
 *  SYSCALL 0x0.
 *  args[1]: String.
 *
 *
 */

static void sys_conout(uint64_t* args) {
  const char* str = (const char*)args[1];

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
static void sys_initrd_load(void) {
  // regs.rax = proc_initrd_load((const char*)regs.rbx);
}
*/


/*
 *  RSI: Driver ID.
 *  RDX: Command.
 *  
 *
 */

/*
static void sys_ioctl(regs_t regs) {
  driver_node_t* driver = uapi_locate_driver((const char*)regs.rbx);

  if (driver == NULL) {
    regs.rax = -1;
    return;
  }

  size_t args[20] = {regs.rdx, regs.rsi, regs.rdi, regs.r8, regs.r9, regs.r10};
  driver->ioctl(regs.rcx, args);
}
*/


void(*syscall_table[MAX_SYSCALLS])(uint64_t* args) = {
  sys_conout,       // 0x1.
};
