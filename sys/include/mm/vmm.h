#ifndef VMM_H_
#define VMM_H_

#include <lib/types.h>

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000
#define PTE_PRESENT (1ull << 0)
#define PTE_WRITABLE (1ull << 1)
#define PTE_USER (1ULL << 2)
#define PTE_NX (1ULL << 63)

extern volatile struct limine_hhdm_request hhdm_request;

#define VMM_HIGHER_HALF (hhdm_request.response->offset)
#define PAGE_SIZE 0x1000
#define ALIGN_DOWN(address, align)  ((address) & ~((align)-1))
#define ALIGN_UP(address, align)    (((address) + (align)-1) & ~((align)-1))

#endif
