#include <drivers/hdd/ata.h>
#include <arch/x86/io.h>
#include <lib/log.h>
#include <lib/panic.h>

#define MASTER_DRIVE 0xA0
#define DRIVE_SELECT 0x1F6
#define STATUS_COMMAND 0x1F7      // Read (status), write (command).
#define CTRL 0x3F6

#define STATUS_BSY (1 << 7)
#define STATUS_RDY (1 << 6)
#define STATUS_DRQ (1 << 3)
#define STATUS_DF  (1 << 5)
#define STATUS_ERR (1 << 0)

static inline void wait(void) {
  io_wait();
  io_wait();
  io_wait();
  io_wait();
  io_wait();
  io_wait();
  io_wait();
}


static inline void reset(void) {
  outportb(CTRL, inportb(CTRL) | (1 << 2));
  wait();
  outportb(CTRL, inportb(CTRL) & ~(1 << 2));
}


static uint8_t drive_checked = 0;


static void check_drive(void) {
  outportb(DRIVE_SELECT, MASTER_DRIVE);
  outportb(0x1F2, 0);
  outportb(0x1F3, 0);
  outportb(0x1F4, 0);
  outportb(0x1F5, 0);
  outportb(STATUS_COMMAND, 0xEC);      // IDENTIFY.
  wait();

  /*
   *  Check if the drive exists.
   *
   */
  if (inportb(STATUS_COMMAND) == 0) {
    printk(PRINTK_PANIC "kpanic: Could not locate target ATA drive.\n");
    kpanic();
  }

  if (inportb(0x1F4) != 0 || inportb(0x1F5) != 0) { 
    printk(PRINTK_PANIC "kpanic: Could not locate target ATA drive.\n");
    kpanic();
  }

  drive_checked = 1;
  reset();
}





static void wait_busy(void) {
  while (inportb(STATUS_COMMAND) & STATUS_BSY);
}

static void wait_drq(void) {
  while (!(inportb(STATUS_COMMAND) & STATUS_RDY)); 
}

void ata_read(uint16_t* out_buf, uint32_t lba, uint16_t sector_count) {
  wait_busy();
  wait_drq();
  outportb(DRIVE_SELECT, 0xE0);
  outportb(0x1F2, sector_count);
  outportb(0x1F3, lba & 0xFF);
  outportb(0x1F4, (lba >> 8) & 0xFF);
  outportb(0x1F5, (lba >> 16) & 0xFF);
  outportb(STATUS_COMMAND, 0x20);      // READ.
  
  for (uint16_t i = 0; i < sector_count; ++i) {
    wait_busy();
    wait_drq();
    
    for (uint16_t j = 0; j < 256; ++j) {
      out_buf[j] = inportw(0x1F0);
    }

    out_buf += 256;
  }
}

void ata_write(uint16_t* in_buf, uint32_t lba, uint16_t sector_count) {
  wait_busy();
  outportb(DRIVE_SELECT, 0xE0);
  outportb(0x1F2, sector_count);
  outportb(0x1F3, lba & 0xFF);
  outportb(0x1F4, (lba >> 8) & 0xFF);
  outportb(0x1F5, (lba >> 16) & 0xFF);
  outportb(STATUS_COMMAND, 0x30);       // WRITE.
  
  for (uint16_t i = 0; i < sector_count; ++i) {
    wait_busy();
    wait_drq();

    for (uint16_t j = 0; j < 256; ++j) {
      outportl(0x1F0, in_buf[j]);
    }
  }
}


void ata_init(void) { 
  /*
   *  Check for the floating bus
   *  condition (i.e no drives on bus).
   *
   */
  if (inportb(STATUS_COMMAND) == 0xFF) {
    printk(PRINTK_PANIC "kpanic: No ATA drives on bus.\n");
    kpanic();
  }

  printk("%x\n", inportb(STATUS_COMMAND));
  printk("[ATA]: Possible drives have been detected on bus.\n");
}
