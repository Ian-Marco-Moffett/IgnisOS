#ifndef LAPIC_H_
#define LAPIC_H_

#include <lib/types.h>

#ifdef __x86_64__
void lapic_init(void);
void lapic_send_eoi(void);
uint32_t lapic_read_id(void);
#endif // __x86_64__

#endif // LAPIC_H_
