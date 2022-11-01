#ifndef SYSCALL_H_
#define SYSCALL_H_

#define MAX_SYSCALLS 4

#include <lib/types.h>
#include <proc/proc.h>

extern void(*syscall_table[MAX_SYSCALLS])(struct trapframe* tf);


#endif
