#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#ifdef __x86_64__

void register_trap(uint8_t vector, void(*isr)(void));
void register_user_int(uint8_t vector, void(*isr)(void));

#endif    // __x86_64__
#endif    // INTERRUPT_H_
