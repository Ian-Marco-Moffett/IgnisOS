#include <proc/proc.h>

extern process_t* running_process;
extern process_t* process_queue_base;


void proc_set_rsp(uint64_t rsp_val) {
  running_process->rsp = rsp_val;
}

uint64_t proc_get_rsp(void) {
  return running_process->rsp;
}


uint64_t proc_get_cr3(void) {
  return running_process->cr3;
}

uint64_t proc_get_rip(void) {
  return running_process->rip;
}


void proc_set_rip(uint64_t rip) {
  running_process->rip = rip;
}


void proc_next(void) {
  running_process = running_process->next == NULL ? process_queue_base : running_process->next;
}
