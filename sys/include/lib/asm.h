#ifndef ASM_H_
#define ASM_H_


#define ASMV(_asm) __asm__ __volatile__(_asm)
#define _packed __attribute__((packed))
#define _noreturn __attribute__((noreturn))
#define _isr _noreturn


#endif
