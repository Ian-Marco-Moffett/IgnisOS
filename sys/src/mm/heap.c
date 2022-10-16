#include <mm/heap.h>
#include <mm/mmap.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <lib/log.h>

#define USED 0
#define FREE 1

#define DATA_START(block_base) ((&block_base->next) + 1)



/*
 *  Ensure each whole heap block is page sized with 
 *  __attribute__((aligned(PAGE_SIZE)))
 *
 */

typedef struct _HeapBlk {
  uint8_t state : 1;
  size_t bytes_allocated;
  struct _HeapBlk* next;
} __attribute__((aligned(PAGE_SIZE))) heapblk_t;


static heapblk_t* heap_start = NULL;
static heapblk_t* freelist = NULL; 
static size_t heap_size_pages = 0;
static size_t bytes_allocated = 0;


void create_heap(uintptr_t vaddr_start, size_t n_pages) {  
  // Map the heap.
  mmap((void*)vaddr_start, n_pages, PROT_READ | PROT_WRITE); 
  heap_size_pages = n_pages;
  heap_start = (void*)vaddr_start;
  
  heap_start->state = FREE;

  // Prepare setting up all next fields.
  heapblk_t* cur = heap_start;
  heapblk_t* tmp = heap_start;
  freelist = heap_start;
  
  // Set all next fields.
  for (size_t i = 0; i < n_pages; ++i) {
    cur->bytes_allocated = 0;
    cur->next = ++tmp;
    cur = cur->next;
  }
  
  cur->next = NULL;
  cur->bytes_allocated = 0;
}


void* kmalloc(size_t size) { 
  // Invalid size is bad >:(
  if (size == 0)
    return NULL;
  
  // Allocated too much at once or the freelist is NULL?
  if (size >= 0x1000 || freelist == NULL)
    return NULL;

  // Too many bytes have been allocated?
  if (bytes_allocated/0x1000 >= heap_size_pages)
    return NULL;

  // Do we need the next block?
  if (freelist->bytes_allocated >= 0x1000-1) {
    while (freelist != NULL) {
      if (freelist->state == FREE) {
        break;
      }

      freelist = freelist->next;
    }

    if (freelist == NULL)
      return NULL;
  }

  char* ret = (char*)DATA_START(freelist) + freelist->bytes_allocated;
  freelist->bytes_allocated += size;
  return ret;
}
