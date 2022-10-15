#include <mm/heap.h>
#include <mm/mmap.h>
#include <mm/pmm.h>
#include <sync/mutex.h>


static uint8_t is_init = 0;


typedef union header {
  struct {
    size_t bytes_allocated;
    union header* next;
  } meta;

  char align[16];
} header_t;


static header_t* head, *tail;


static header_t* get_free_block(size_t size) {
  header_t* cur = head;

  while (cur) {
    if (cur->meta.bytes_allocated + size < MAX_BYTES_PER_ALLOC-sizeof(cur->meta)) {
      cur->meta.bytes_allocated += size;
      return cur;
    }

    cur = cur->meta.next;
  }

  return NULL;
}


void create_heap(uintptr_t vaddr_start, size_t n_pages) { 
  
  // Map the heap.
  mmap((void*)vaddr_start, n_pages, PROT_READ | PROT_WRITE);
  is_init = 1;
  head = (header_t*)vaddr_start;
  tail = head;
}


void* kmalloc(size_t size) {
  if (!(size) || !(is_init)) {
    return NULL;
  } else if (size >= MAX_BYTES_PER_ALLOC-sizeof(head->meta)) {
    return NULL;
  }
  
  header_t* header = get_free_block(size);
  
  if (header != NULL) {
    char* ret = (char*)(header + 1);
    return ret + header->meta.bytes_allocated;
  }
  
  head->meta.next = (header_t*)pmm_alloc_frame();

  if (!(head->meta.next))
    return NULL;
  
  header_t* ret = head;
  head = head->meta.next;
  head->meta.next = NULL;
  return ret + 1;
}
