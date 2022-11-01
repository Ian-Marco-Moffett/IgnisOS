#include <mm/heap.h>
#include <mm/mmap.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <lib/assert.h>

#define USED 0
#define FREE 1

#define DATA_START(mem_block) (((char*) mem_block) + sizeof(heapblk_t))
#define MAGIC 0xCA7F00D



static struct heap_context kctx = {
  .bytes_allocated = 0,
  .heap_size = 0,
  .head = NULL,
  .tail = NULL
};


static heapblk_t* first_fit(size_t size, struct heap_context ctx) {
  heapblk_t* block = ctx.tail;

  while (block != NULL) {
    if (block->state == FREE && block->size >= size) {
      return block;
    }

    block = block->next;
  }

  return NULL;
}


void* __malloc(size_t size, struct heap_context* ctx) {
  if (ctx->bytes_allocated + size > ctx->heap_size || size == 0) {
    return NULL;
  }

  if (ctx->head->magic != MAGIC || ctx->tail->magic != MAGIC) {
    // TODO: On heap corruption, kill the process.
    return NULL;
  }

  heapblk_t* region = first_fit(size, *ctx);
  if (region == NULL) {
    char* next = DATA_START(ctx->head + ctx->head->size);
    ctx->head->next = (heapblk_t*)next;
    region = ctx->head->next;
    region->next = NULL;
    region->state = USED;
    region->magic = MAGIC;
    region->size = size;
    ctx->head = region;
    ctx->tail = ctx->head;
  }

  ctx->bytes_allocated += size;
  return DATA_START(region);
}


void __free(void* ptr, struct heap_context* ctx) {
  heapblk_t* region = ptr - sizeof(heapblk_t);

  if (region->magic != MAGIC)
    return;

  heapblk_t* cur = region;

  while (cur != NULL && ptr != DATA_START(cur)) {
    cur->state = FREE;
    cur = cur->next;
  }

  ctx->tail = region;
}


void create_heap(uintptr_t vaddr_start, size_t n_pages) {  
  mmap((void*)vaddr_start, n_pages, PROT_READ | PROT_WRITE); 
  kctx.heap_size = n_pages*PAGE_SIZE;
  kctx.head = (heapblk_t*)vaddr_start;
  kctx.head->size = 0;
  kctx.head->next = NULL;
  kctx.head->state = USED;
  kctx.head->magic = MAGIC;
  kctx.tail = kctx.head;
}


void create_process_heap(uintptr_t vaddr_start, size_t n_pages, struct heap_context* p_heapctx) {
  mmap((void*)vaddr_start, n_pages, PROT_READ | PROT_WRITE | PROT_USER);
  p_heapctx->heap_size = n_pages*PAGE_SIZE;
  p_heapctx->head = (heapblk_t*)vaddr_start;
  p_heapctx->head->size = 0;
  p_heapctx->head->next = NULL;
  p_heapctx->head->state = USED;
  p_heapctx->head->magic = MAGIC;
  p_heapctx->tail = kctx.head;
}


void* kmalloc(size_t size) {
  return __malloc(size, &kctx);
}

void kfree(void* ptr) {
  __free(ptr, &kctx);
}

void* krealloc(void* oldptr, size_t newsize) {
  char* new_ptr = kmalloc(newsize);

  for (size_t i = 0; i < newsize; ++i) {
    new_ptr[i] = ((char*)oldptr)[i];
  }

  kfree(oldptr);
  return new_ptr;
}
