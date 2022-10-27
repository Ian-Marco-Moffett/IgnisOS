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
 * args[1]: Driver ID string.
 * args[2]: Command.
 * args[3] to args[10]: IOCTL args (8 args max).
 *
 *
 */

static void sys_ioctl(uint64_t* args) {
  if (args[1] == 0)
    return;

  driver_node_t* driver = uapi_locate_driver((const char*)args[1]);

  if (driver == NULL) {
    return;
  }

  size_t ioctl_args[20] = {args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10]};
  driver->ioctl(args[2], ioctl_args);
}


void(*syscall_table[MAX_SYSCALLS])(uint64_t* args) = {
  sys_conout,       // 0x1.
  sys_ioctl,        // 0x2.
};
