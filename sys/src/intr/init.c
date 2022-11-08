#include <intr/init.h>
#include <arch/x64/interrupts.h>
#include <intr/irq.h>
#include <intr/exceptions.h>
#include <arch/cpu/smp.h>
#include <proc/proc.h>

#define DESTMODE_LOGICAL (1 << 11)
#define DESTFIELD_START  (56)

void syscall_entry(void* stackframe);
void __vec_0x81(void* stackframe);


void intr_init(void) {
  init_exceptions();

  /*
   *  Route this interrupt to the BSP
   *  by ensuring the Destination Field
   *  is set to the LAPIC ID of the BSP
   *  and IOREDTBL[11]=0
   *
   */

  const uint32_t BSP_LAPIC_ID = smp_get_bsp_lapic_id();
  uint64_t timer_redentry_data = ((uint64_t)BSP_LAPIC_ID << DESTFIELD_START);

  register_irq(0, 0x20, __irq0_isr, timer_redentry_data);
  register_int(0x81, __vec_0x81);
  register_int(0x82, __system_halt);
  register_user_int(0x80, syscall_entry);
}
