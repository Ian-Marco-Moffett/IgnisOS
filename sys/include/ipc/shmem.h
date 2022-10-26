#ifndef SHMEM_H_
#define SHMEM_H_

#include <lib/types.h>


/*
 *  Shared memory IPC stuff.
 *
 */


/*
 *  Two ports are available for use.
 *
 *  A syscall port used for requesting
 *  services from the kernel. And a 
 *  general purpose one.
 *
 */


typedef enum {
  SPT_SYSCALL,
  SPT_GENERAL
} SHMEM_PORT_TYPE;


void shmem_make_port(SHMEM_PORT_TYPE port_type, uintptr_t vaddr);


#endif
