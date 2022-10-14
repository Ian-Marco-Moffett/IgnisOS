#include <mm/pmm.h>
#include <mm/vmm.h>
#include <lib/limine.h>
#include <lib/types.h>
#include <lib/string.h>

#define MAX(A, B) ({ \
    __auto_type MAX_a = A; \
    __auto_type MAX_b = B; \
    MAX_a > MAX_b ? MAX_a : MAX_b; \
})

volatile struct limine_memmap_request mmap_req = {
  .id = LIMINE_MEMMAP_REQUEST,
  .revision = 0
};


static uint8_t* bitmap = NULL;
static size_t usable_pages = 0;
static size_t bitmap_size = 0;
static uintptr_t highest_addr = 0;


static inline void _bitmap_unset_bit(size_t bit) {
  bitmap[bit / 8] &= ~(1 << (bit % 8));
}


static inline void _bitmap_set_bit(size_t bit) {
  bitmap[bit / 8] &= ~(1 << (bit % 8));
}


static size_t _bitmap_test(size_t bit) {
  return bitmap[bit/8] & (1 << (bit % 8));
}


uintptr_t pmm_alloc_frame(void) {
  for (size_t bit = 0; bit < bitmap_size*8; ++bit) {
    if (!(_bitmap_test(bit))) {
      _bitmap_set_bit(bit);
      kmemzero(((void*)(PAGE_SIZE*bit)), PAGE_SIZE);
      return PAGE_SIZE*bit;
    }
  }

  return 0;
}


void pmm_free_frame(uintptr_t frame) {
  _bitmap_unset_bit(frame/PAGE_SIZE);
}


void pmm_init(void) {
  struct limine_memmap_response* mmap = mmap_req.response;

  for (size_t i = 0; i < mmap->entry_count; ++i) {
    struct limine_memmap_entry* entry = mmap->entries[i];

    if (entry->type != LIMINE_MEMMAP_USABLE) {
      continue;
    }

    usable_pages = entry->length/PAGE_SIZE;
    highest_addr = MAX(highest_addr, entry->base + entry->length);
  }
  
  bitmap_size = ALIGN_UP((highest_addr/PAGE_SIZE)/8, PAGE_SIZE);

  // Find a chunk of memory to hold
  // our bitmap.
  for (size_t i = 0; i < mmap->entry_count; ++i) {
    struct limine_memmap_entry* entry = mmap->entries[i];

    if (entry->type != LIMINE_MEMMAP_USABLE) {
      continue;
    }

    if (entry->length >= bitmap_size) {
      bitmap = (uint8_t*)(entry->base + VMM_HIGHER_HALF);
      kmemset(bitmap, 0xFF, bitmap_size);    // Set all entries to unused.
      entry->length -= bitmap_size;
      entry->base += bitmap_size;
      break;
    }
  }


  for (size_t i = 0; i < mmap->entry_count; ++i) {
    struct limine_memmap_entry* entry = mmap->entries[i];

    if (entry->type != LIMINE_MEMMAP_USABLE) {
      continue;
    }

    for (uint64_t j = 0; j < entry->length; j += PAGE_SIZE) {
      _bitmap_unset_bit((entry->base + j)/PAGE_SIZE);
    }
  }
}
