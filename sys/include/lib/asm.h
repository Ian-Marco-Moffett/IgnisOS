#ifndef ASM_H_
#define ASM_H_


#define ASMV(_asm) __asm__ __volatile__(_asm)
#define _packed __attribute__((packed))
#define _noreturn __attribute__((noreturn))
#define _naked __attribute__((naked))
#define _isr _noreturn

#define CLI_SLEEP \
  for (unsigned long long i = 0; i < 1000000; ++i) {             \
    ASMV("cli");                                                      \
  }


#endif
