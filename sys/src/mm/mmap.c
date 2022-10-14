#include <mm/mmap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <lib/asm.h>


static PAGEMAP get_pagemap(void) {
  PAGEMAP ret;
  ASMV("mov %%cr3, %0" : "=r" (ret) :: "memory");
  return ret;
}


errno_t mmap(void* virt, mmap_prot_t prot) {
  PAGEMAP* top_level = (PAGEMAP*)get_pagemap();
  size_t pte_flags = 0;

  if (!(prot & PROT_EXEC))
    pte_flags |= PTE_NX;

  if (prot & PTE_WRITABLE)
    pte_flags |= PTE_WRITABLE;

  if (prot & PROT_READ)
    pte_flags |= PTE_PRESENT;

  uint64_t phys = pmm_alloc_frame();

  if (phys == 0) {
    return -ENOMEM;
  }

  vmm_map_page(top_level, pmm_alloc_frame(), PAGE_ALIGN((uintptr_t)virt), pte_flags);
  return 0;
}


void munmap(void* virt) {
  PAGEMAP* top_level = (PAGEMAP*)get_pagemap();
  uintptr_t phys = vmm_get_phys(top_level, PAGE_ALIGN((uintptr_t)virt));
  vmm_unmap_page(top_level, PAGE_ALIGN((uintptr_t)virt));
  pmm_free_frame(phys);
}


void __munmap_any(void* virt) {
  PAGEMAP* top_level = (PAGEMAP*)get_pagemap();
  vmm_unmap_page(top_level, PAGE_ALIGN((uintptr_t)virt));
}
