#ifndef LAPIC_H_
#define LAPIC_H_

#include <lib/types.h>

#ifdef __x86_64__
void lapic_init(void);
#endif // __x86_64__

#endif // LAPIC_H_
