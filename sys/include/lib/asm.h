#ifndef ASM_H_
#define ASM_H_


#define ASMV(_asm) __asm__ __volatile__(_asm)
#define _packed __attribute__((packed))
#define _noreturn __attribute__((noreturn))
#define _naked __attribute__((naked))
#define _isr __attribute__((interrupt))

#define CLI_SLEEP \
  for (unsigned long long i = 0; i < 900000; ++i) {                 \
    ASMV("cli");                                                      \
  }


#endif
