#ifndef PMM_H_
#define PMM_H_

#include <lib/types.h>


void pmm_init(void);
uintptr_t pmm_alloc_frame(void);
uintptr_t pmm_alloc_heap_base(size_t n_frames);
void pmm_free_frame(uintptr_t frame);


#endif
