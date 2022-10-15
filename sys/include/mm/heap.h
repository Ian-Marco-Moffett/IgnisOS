#ifndef KHEAP_H_
#define KHEAP_H_

#include <lib/types.h>
#include <mm/vmm.h>

#define MAX_BYTES_PER_ALLOC PAGE_SIZE

void create_heap(uintptr_t vaddr_start, size_t n_pages);
void* kmalloc(size_t size);

#endif
