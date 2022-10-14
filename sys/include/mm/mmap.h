#ifndef MMAP_H_
#define MMAP_H_

#include <lib/errno.h>


typedef enum {
  PROT_EXEC   =   (1 << 0),
  PROT_READ   =   (1 << 1),
  PROT_WRITE  =   (1 << 2),
} mmap_prot_t;

/*
 *  Maps a chunk of virtual memory.
 *
 *  @param virt: Virtual address to be allocated.
 *  @param prot: Protection flags.
 *
 *
 */

errno_t mmap(void* virt, mmap_prot_t prot);

/*
 *  Unmaps a chunk of memory.
 *
 *  @param virt: Virtual address to be unmapped.
 *
 *
 */

void munmap(void* virt);


/*
 *  Unmaps a chunk of virtual memory 
 *  even if it wasn't mapped
 *  by mmap().
 *
 *  NOTE: DANGEROUS, BE CAREFUL!
 *
 *  @param virt: Virtual address to be unmapped.
 *
 *
 */

void __munmap_any(void* virt);

#endif
