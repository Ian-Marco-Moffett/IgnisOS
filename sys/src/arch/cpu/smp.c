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
static struct limine_smp_response* smp_resp;
static size_t bsp_lapic_id = 0;
static PAGEMAP start_pml4 = 0;


size_t smp_get_core_count(void) {
  return core_count;
}


size_t smp_get_bsp_lapic_id(void) {
  return bsp_lapic_id;
}


void smp_goto(struct core* core, void* to) {
  ASSERT(core->index < core_count, "Core ID >= core count!!!\n");
  cores[core->index]->extra_argument = (uint64_t)core;
  cores[core->index]->goto_address = to;
}

void smp_init(void) {
  start_pml4 = (PAGEMAP)vmm_make_pml4();
  smp_resp = smp_req.response;
  bsp_lapic_id = smp_resp->bsp_lapic_id;
  cores = smp_resp->cpus;
  core_count = smp_resp->cpu_count;
  printk(PRINTK_CLEAR "[INFO]: CPU has %d %s.\n", core_count, core_count > 1 ? "cores" : "core");

  lapic_id_list = kmalloc(sizeof(size_t)*core_count);

  for (size_t i = 0; i < smp_resp->cpu_count; ++i) {
    struct limine_smp_info* cpu = smp_resp->cpus[i];
    lapic_id_list[i] = cpu->lapic_id;
  }
}
