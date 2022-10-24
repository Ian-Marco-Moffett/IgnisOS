#include <mm/mmap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <lib/asm.h>


static size_t get_flags_from_prot(mmap_prot_t prot) {
  size_t pte_flags = 0;

  if (!(prot & PROT_EXEC))
    pte_flags |= PTE_NX;

  if (prot & PROT_WRITE)
    pte_flags |= PTE_WRITABLE;

  if (prot & PROT_READ)
    pte_flags |= PTE_PRESENT;

  if (prot & PROT_USER)
    pte_flags |= PTE_USER;

  return pte_flags;
}


errno_t mmap_at(void* cr3, void* virt, size_t n_pages, mmap_prot_t prot) {
  size_t pte_flags = get_flags_from_prot(prot);

  for (uint32_t i = 0; i < n_pages; ++i) {
    uint64_t phys = pmm_alloc_frame();

    if (phys == 0) {
      return -ENOMEM;
    }

    vmm_map_page(cr3, phys, PAGE_ALIGN((uintptr_t)virt), pte_flags);
    virt += PAGE_SIZE;
  }
  
  return 0;
}


errno_t mmap(void* virt, size_t n_pages, mmap_prot_t prot) {
  size_t pte_flags = get_flags_from_prot(prot);
  PAGEMAP* top_level = get_pml4();

  for (uint32_t i = 0; i < n_pages; ++i) {
    uint64_t phys = pmm_alloc_frame();

    if (phys == 0) {
      return -ENOMEM;
    }

    vmm_map_page(top_level, phys, PAGE_ALIGN((uintptr_t)virt), pte_flags);
    virt += PAGE_SIZE;
  }
  return 0;
}


void munmap(void* virt) {
  PAGEMAP* top_level = (PAGEMAP*)get_pml4();
  uintptr_t phys = vmm_get_phys(top_level, PAGE_ALIGN((uintptr_t)virt));
  vmm_unmap_page(top_level, PAGE_ALIGN((uintptr_t)virt));
  pmm_free_frame(phys);
}


void __munmap_any(void* virt) {
  PAGEMAP* top_level = (PAGEMAP*)get_pml4();
  vmm_unmap_page(top_level, PAGE_ALIGN((uintptr_t)virt));
}
