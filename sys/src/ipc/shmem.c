#include <ipc/shmem.h>
#include <mm/mmap.h>
#include <proc/proc.h>

void shmem_make_port(SHMEM_PORT_TYPE port_type, uintptr_t vaddr) {
  extern process_t* running_process;

  if (port_type > SPT_GENERAL)
    return;

  mmap((void*)vaddr, 1, PROT_READ | PROT_WRITE | PROT_USER);
  running_process->ports[port_type] = (uint64_t*)vaddr;
}
