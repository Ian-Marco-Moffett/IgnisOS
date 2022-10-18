#ifndef VMM_H_
#define VMM_H_

#include <lib/types.h>
#include <lib/limine.h>

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000
#define PTE_PRESENT (1ull << 0)
#define PTE_WRITABLE (1ull << 1)
#define PTE_USER (1ULL << 2)
#define PTE_NX (1ULL << 63)
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)

typedef uint64_t PAGEMAP;

extern volatile struct limine_hhdm_request hhdm_request;


void vmm_map_page(PAGEMAP* pml4, uintptr_t phys, uintptr_t virt, size_t flags);
void vmm_unmap_page(PAGEMAP* pml4, uintptr_t virt);
uintptr_t vmm_get_phys(PAGEMAP* pml4, uintptr_t virt);
PAGEMAP* vmm_make_pml4(void);
void vmm_init(void);


#define VMM_HIGHER_HALF (hhdm_request.response->offset)
#define PAGE_SIZE 0x1000
#define ALIGN_DOWN(address, align)  ((address) & ~((align)-1))
#define ALIGN_UP(address, align)    (((address) + (align)-1) & ~((align)-1))
#define PAGE_ALIGN(address) (ALIGN_DOWN(address, PAGE_SIZE))

#endif
