#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/heap.h>
#include <lib/limine.h>
#include <lib/asm.h>
#include <lib/log.h>

volatile struct limine_hhdm_request hhdm_request = {
  .id = LIMINE_HHDM_REQUEST,
  .revision = 0
};


PAGEMAP root_pagemap = 0;

static inline void __tlb_flush_single(uintptr_t virt) {
  ASMV("invlpg (%0)" :: "r" (virt) : "memory");
}

PAGEMAP* vmm_make_pml4(void) {
  PAGEMAP* new_pagemap = (PAGEMAP*)pmm_alloc_frame(); 

  for (size_t i = 0; i < 512; ++i) {
    new_pagemap[i] = ((PAGEMAP*)root_pagemap)[i];
  }

  return new_pagemap;
}


static PAGEMAP* _get_next_level(PAGEMAP* top_level, uint64_t index) {

  if ((top_level[index] & PTE_PRESENT)) {
    return ((uintptr_t*)PTE_GET_ADDR(top_level[index]));
  }

  /*
   *  If we got here, the entry is not present
   *  so we must allocate our own entry.
   */
  uintptr_t next_level = pmm_alloc_frame();

  if (next_level == 0)
    return NULL;

  /*
   *  Set the next level PRESENT READ-WRITE flags.
   *
   */
  top_level[index] = (uintptr_t)next_level | PTE_PRESENT | PTE_WRITABLE;
  return (PAGEMAP*)next_level;
}


void vmm_map_page(PAGEMAP* pml4, uintptr_t phys, uintptr_t virt, size_t flags) {
  // Fetch indexes.
  size_t index4 = (virt & ((size_t)0x1FF << 39)) >> 39;
  size_t index3 = (virt & ((size_t)0x1FF << 30)) >> 30;
  size_t index2 = (virt & ((size_t)0x1FF << 21)) >> 21;
  size_t index1 = (virt & ((size_t)0x1FF << 12)) >> 12;

  uintptr_t* pml3 = _get_next_level(pml4, index4);  

  if (pml3 == NULL)
    return;

  uintptr_t* pml2 = _get_next_level(pml3, index3);

  if (pml2 == NULL)
    return;

  uintptr_t* page_table = _get_next_level(pml2, index2);

  if (page_table == NULL)
    return;
  
  page_table[index1] = phys | flags;
  __tlb_flush_single(virt);
}

PAGEMAP* get_pml4(void) {
  PAGEMAP ret;
  ASMV("mov %%cr3, %0" : "=r" (ret) :: "memory");
  return (PAGEMAP*)ret;
}


void vmm_unmap_page(PAGEMAP* pml4, uintptr_t virt) {
  // Fetch indexes.
  size_t index4 = (virt & ((size_t)0x1FF << 39)) >> 39;
  size_t index3 = (virt & ((size_t)0x1FF << 30)) >> 30;
  size_t index2 = (virt & ((size_t)0x1FF << 21)) >> 21;
  size_t index1 = (virt & ((size_t)0x1FF << 12)) >> 12;

  uintptr_t* pml3 = _get_next_level(pml4, index4);
  
  if (pml3 == NULL)
    return;

  uintptr_t* pml2 = _get_next_level(pml3, index3);

  if (pml2 == NULL)
    return;

  uintptr_t* page_table = _get_next_level(pml2, index2);

  if (page_table == NULL)
    return;

  page_table[index1] = 0;
  __tlb_flush_single(virt);
}

uintptr_t vmm_get_phys(PAGEMAP* pml4, uintptr_t virt) {
  virt = PAGE_ALIGN(virt);

  // Fetch indexes.
  size_t index4 = (virt & ((size_t)0x1FF << 39)) >> 39;
  size_t index3 = (virt & ((size_t)0x1FF << 30)) >> 30;
  size_t index2 = (virt & ((size_t)0x1FF << 21)) >> 21;
  size_t index1 = (virt & ((size_t)0x1FF << 12)) >> 12;

  uintptr_t* pml3 = _get_next_level(pml4, index4);
  
  if (pml3 == NULL)
    return 0;

  uintptr_t* pml2 = _get_next_level(pml3, index3);

  if (pml2 == NULL)
    return 0;

  uintptr_t* page_table = _get_next_level(pml2, index2);

  if (page_table == NULL)
    return 0;
  
  return (PTE_GET_ADDR(page_table[index1]));
}


void vmm_update_flags(PAGEMAP* pml4, uintptr_t virt, size_t flags) {
  // Fetch indexes.
  size_t index4 = (virt & ((size_t)0x1FF << 39)) >> 39;
  size_t index3 = (virt & ((size_t)0x1FF << 30)) >> 30;
  size_t index2 = (virt & ((size_t)0x1FF << 21)) >> 21;
  size_t index1 = (virt & ((size_t)0x1FF << 12)) >> 12;

  uintptr_t* pml3 = _get_next_level(pml4, index4);  

  if (pml3 == NULL)
    return;

  uintptr_t* pml2 = _get_next_level(pml3, index3);

  if (pml2 == NULL)
    return;

  uintptr_t* page_table = _get_next_level(pml2, index2);

  if (page_table == NULL)
    return;
  
  page_table[index1] &= ~(0b111 | PTE_NX);
  page_table[index1] |= flags;
  __tlb_flush_single(virt);
}


void vmm_init(void) {
  ASMV("mov %%cr3, %0" : "=r" (root_pagemap) :: "memory");
}
