#ifndef SYSCALL_H_
#define SYSCALL_H_

#define MAX_SYSCALLS 2

#include <lib/types.h>

extern void(*syscall_table[MAX_SYSCALLS])(uint64_t* args);


#endif
