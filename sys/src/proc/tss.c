#include <proc/tss.h>
#include <mm/mmap.h>
#include <mm/heap.h>
#include <lib/string.h>

struct TSSEntry* tss;


void write_tss(void) {
  extern struct TSSDescriptor* gdt_tss;

  tss = kmalloc(sizeof(struct TSSEntry));
  kmemzero(tss, sizeof(struct TSSEntry));

  uint64_t tss_base = (uint64_t)tss;

  tss->rsp0Low = 0;
  tss->rsp0High = 0;

  gdt_tss->seglimit = sizeof(struct TSSEntry);
  gdt_tss->g = 0;

  gdt_tss->baseAddrLow = tss_base & 0xFFFF;
  gdt_tss->baseAddrMiddle = (tss_base >> 16) & 0xFF;
  gdt_tss->baseAddrOtherMiddle = (tss_base >> 24) & 0xFF;
  gdt_tss->baseAddrUpper = tss_base >> 32;
  gdt_tss->avl = 0;                                           // 0 for TSS.
  gdt_tss->dpl = 0;
  gdt_tss->p = 1;
  gdt_tss->reserved = 0;
  gdt_tss->type = 0x9;
  gdt_tss->zero = 0;
}
