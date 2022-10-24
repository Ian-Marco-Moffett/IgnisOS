#include <mm/heap.h>
#include <mm/mmap.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <lib/assert.h>

#define USED 0
#define FREE 1

#define DATA_START(mem_block) (((char*) mem_block) + sizeof(heapblk_t))
#define MAGIC 0xCA7F00D


typedef struct _HeapBlk {
  uint32_t magic;
  uint8_t state : 1;
  size_t size;
  struct _HeapBlk* next;
} heapblk_t;


static heapblk_t* heap_head = NULL;
static heapblk_t* heap_tail = NULL;
static size_t heap_size = 0;
static size_t bytes_allocated = 0;


static heapblk_t* first_fit(size_t size) {
  heapblk_t* block = heap_tail;

  while (block != NULL) {
    if (block->state == FREE && block->size >= size) {
      return block;
    }

    block = block->next;
  }

  return NULL;
}


void create_heap(uintptr_t vaddr_start, size_t n_pages) {  
  mmap((void*)vaddr_start, n_pages, PROT_READ | PROT_WRITE); 
  heap_size = n_pages*PAGE_SIZE;
  heap_head = (heapblk_t*)vaddr_start;
  heap_head->size = 0;
  heap_head->next = NULL;
  heap_head->state = USED;
  heap_head->magic = MAGIC;
  heap_tail = heap_head;
}


void* kmalloc(size_t size) { 
  if (bytes_allocated + size > heap_size || size == 0) {
    return NULL;
  }
  
  // Ensure there is no heap corruption.
  ASSERT(heap_head->magic == MAGIC, "Kernel heap corruption detected, panicked.\n");
  ASSERT(heap_tail->magic == MAGIC, "Kernel heap corruption detected, panicked.\n");

  if (bytes_allocated >= heap_size) {
    return NULL;
  }

  heapblk_t* region = first_fit(size);

  if (region == NULL) {
    char* next = DATA_START(heap_head + heap_head->size);
    heap_head->next = (heapblk_t*)next;
    region = heap_head->next;
    region->next = NULL;
    region->state = USED;
    region->magic = MAGIC;
    region->size = size;
    heap_head = region;
    heap_tail = heap_head;
  }

  bytes_allocated += size;
  return DATA_START(region);
}


void kfree(void* ptr) {
  heapblk_t* region = ptr - sizeof(heapblk_t);

  if (region->magic != MAGIC) {
    return;
  }

  heapblk_t* cur = region;

  while (cur != NULL && ptr != DATA_START(cur)) {
    cur->state = FREE;
    cur = cur->next;
  }

  heap_tail = region;
}

void* krealloc(void* oldptr, size_t newsize) {
  char* new_ptr = kmalloc(newsize);

  for (size_t i = 0; i < newsize; ++i) {
    new_ptr[i] = ((char*)oldptr)[i];
  }

  kfree(oldptr);
  return new_ptr;
}
