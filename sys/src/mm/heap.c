#include <mm/heap.h>
#include <mm/mmap.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <lib/assert.h>

#define USED 0
#define FREE 1

#define DATA_START(block_base) ((&block_base->next) + 1)
#define MAGIC 0xCA7F00D



/*
 *  Ensure each whole heap block is page sized with 
 *  __attribute__((aligned(PAGE_SIZE)))
 *
 */

typedef struct _HeapBlk {
  uint32_t magic;
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
    cur->magic = MAGIC;
    cur->next = ++tmp;
    cur->state = FREE;
    cur = cur->next;
  }
  
  cur->next = NULL;
  cur->state = FREE;
  cur->bytes_allocated = 0;
  cur->magic = MAGIC;
}


void* kmalloc(size_t size) { 
  ASSERT(freelist->magic == MAGIC, "Heap corruption detected.\n");

  // Invalid size is bad >:(
  if (size == 0)
    return NULL;
  
  // Is the freelist is NULL?
  if (freelist == NULL)
    return NULL;

  uint8_t n_blocks_allocd = size/0x1000;

  // Too many bytes have been allocated?
  if (bytes_allocated >= (heap_size_pages*PAGE_SIZE)-512)
    return NULL;

  // Do we need the next block?
  if (freelist->bytes_allocated >= PAGE_SIZE-512 || freelist->bytes_allocated + size > PAGE_SIZE-512) {
    while (n_blocks_allocd--) {
      freelist->state = USED;
      freelist = freelist->next;

      if (freelist == NULL) {
        return NULL;
      }
    }

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
  bytes_allocated += size;
  return ret;
}


void kfree(void* ptr) {
  /*
   *  Freeing the memory involves
   *  taking the pointer and getting header base.
   *
   *  If the magic is not correct, the heap must be
   *  corrupted and we must throw a kernel panic.
   *
   *  Once we have the pointer we will set header->state to FREE.
   *  Once we set header->state to FREE we must set freelist to header.
   *
   *
   */

    heapblk_t* header = (heapblk_t*)PAGE_ALIGN((uintptr_t)ptr);

    if (header == NULL)
      return;

    ASSERT(header->magic == MAGIC, "Heap corruption detected.\n");
    header->state = FREE;
    freelist = header;
    freelist->bytes_allocated = 0;
}

void* krealloc(void* oldptr, size_t newsize) {
  char* new_ptr = kmalloc(newsize);

  heapblk_t* header = (heapblk_t*)PAGE_ALIGN((uintptr_t)oldptr);

  for (size_t i = 0; i < newsize; ++i) {
    new_ptr[i] = ((char*)oldptr)[i];
  }

  kfree(oldptr);
  return new_ptr;
}
