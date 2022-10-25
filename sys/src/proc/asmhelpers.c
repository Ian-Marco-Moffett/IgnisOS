#include <proc/proc.h>
#include <lib/string.h>
#include <lib/log.h>

extern process_t* running_process;
extern process_t* process_queue_base;

void proc_switch(regs_t* regs, iret_sf_t* stackframe) {
  kmemcpy((uint8_t*)&running_process->regs, (uint8_t*)regs, sizeof(running_process->regs));
  kmemcpy((uint8_t*)&running_process->iret_stackframe, (uint8_t*)stackframe, sizeof(running_process->iret_stackframe));
  running_process = running_process->next == NULL ? process_queue_base : running_process->next;
  kmemcpy((uint8_t*)regs, (uint8_t*)&running_process->regs, sizeof(running_process->regs));
  kmemcpy((uint8_t*)stackframe, (uint8_t*)&running_process->iret_stackframe, sizeof(running_process->iret_stackframe));
}
