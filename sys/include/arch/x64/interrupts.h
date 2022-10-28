#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <lib/types.h>

#ifdef __x86_64__
void register_trap(uint8_t vector, void(*isr)(void* stackframe));
void register_user_int(uint8_t vector, void(*isr)(void* stackframe));
void register_irq(uint8_t irq, uint8_t vector, void(*isr)(void* stackframe));

#endif    // __x86_64__
#endif    // INTERRUPT_H_
