#include <tty/console.h>
#include <stdint.h>
#include <stdarg.h>

#define MAX_SYSCALLS 1

const uint16_t g_SYSCALL_COUNT = MAX_SYSCALLS;

struct SyscallRegs {
  int64_t rax;
  int64_t rbx;
  int64_t rcx;
  int64_t rdx;
  int64_t rsi;
  int64_t rdi;
  int64_t r8;
  int64_t r9;
  int64_t r10;
} __attribute__((packed)) syscall_regs;


/*
 *  SYSCALL 0x0.
 *  RBX: String.
 *
 *
 */

void sys_conout(void) {
  const char* str = (const char*)syscall_regs.rbx;

  // I don't want it to be that easy to fake
  // a kernel panic (seems like a silly thing to make easy).
  if (str[0] == '\\' && str[1] == '1') {
    return;
  }

  va_list ap;
  console_write((const char*)syscall_regs.rbx, ap);
}


void(*syscall_table[MAX_SYSCALLS])(void) = {
  sys_conout,       // 0x0.
};

