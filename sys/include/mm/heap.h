#ifndef KHEAP_H_
#define KHEAP_H_

#include <lib/types.h>
#include <mm/vmm.h>

#define MAX_BYTES_PER_ALLOC PAGE_SIZE


typedef struct _HeapBlk {
  uint32_t magic;
  uint8_t state : 1;
  size_t size;
  struct _HeapBlk* next;
} heapblk_t;


struct heap_context {
  size_t bytes_allocated;
  size_t heap_size;
  heapblk_t* head;
  heapblk_t* tail;
};


void create_heap(uintptr_t vaddr_start, size_t n_pages);
void create_process_heap(uintptr_t vaddr_start, size_t n_pages, struct heap_context* p_heapctx);
void* kmalloc(size_t size);
void* krealloc(void* oldptr, size_t newsize);
void kfree(void* ptr);
void __free(void* ptr, struct heap_context* ctx);
void* __malloc(size_t size, struct heap_context* ctx);

#endif
