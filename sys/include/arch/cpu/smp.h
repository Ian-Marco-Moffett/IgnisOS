#ifndef CPU_SMP_H_
#define CPU_SMP_H_

#include <lib/types.h>
#include <proc/proc.h>


void smp_init(void);
void smp_goto(struct core* core, void* to);
size_t smp_get_core_count(void);
size_t smp_get_bsp_lapic_id(void);

extern size_t* lapic_id_list;

#endif
