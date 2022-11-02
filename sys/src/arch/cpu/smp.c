#include <arch/cpu/smp.h>
#include <arch/x86/gdt.h>
#include <arch/x64/idt.h>
#include <intr/init.h>
#include <proc/tss.h>
#include <lib/limine.h>
#include <lib/log.h>
#include <lib/asm.h>
#include <sync/mutex.h>
#include <mm/vmm.h>
#include <mm/heap.h>
#include <lib/assert.h>

static struct limine_smp_request smp_req = {
  .id = LIMINE_SMP_REQUEST,
  .revision = 0
};

size_t* lapic_id_list = NULL;
static size_t core_count = 0;
static struct limine_smp_info** cores;


static void cpu_init(struct limine_smp_info* info) { 
  VMM_LOAD_CR3(info->extra_argument);

  load_idt();
  intr_init();

  load_gdt();
  write_tss();
  load_tss();

  while (1) {
    ASMV("hlt");
  }
}

size_t smp_get_core_count(void) {
  return core_count;
}


void smp_goto(struct core* core, void* to) {
  ASSERT(core->index < core_count, "Core ID >= core count!!!\n");
  cores[core->index]->goto_address = to;
}

void smp_init(void) {
  struct limine_smp_response* smp_resp = smp_req.response;
  cores = smp_resp->cpus;
  core_count = smp_resp->cpu_count;
  printk(PRINTK_CLEAR "[INFO]: CPU has %d %s.\n", core_count, core_count > 1 ? "cores" : "core");
  printk("[INFO]: Starting up APs..\n");

  lapic_id_list = kmalloc(sizeof(size_t)*core_count);

  for (size_t i = 0; i < smp_resp->cpu_count; ++i) {
    struct limine_smp_info* cpu = smp_resp->cpus[i];
    lapic_id_list[i] = cpu->lapic_id;

    if (cpu->lapic_id != smp_resp->bsp_lapic_id) {
      cpu->extra_argument = (uint64_t)vmm_make_pml4();
      cpu->goto_address = cpu_init;
      printk("[INFO]: Core with LAPIC ID %d started.\n", cpu->lapic_id);
    } else {
      printk("[INFO]: BSP skipped (Core with LAPIC ID %d).\n", cpu->lapic_id);
    }
  }
}
