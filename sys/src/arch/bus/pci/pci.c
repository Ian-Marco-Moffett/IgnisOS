#include <arch/bus/pci/pci.h>
#include <arch/x86/io.h>

#define CONFIG_ADDR 0xCF8
#define CONFIG_DATA 0xCFC


uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint16_t tmp = 0;

  uint32_t addr = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  outportl(CONFIG_ADDR, addr);
  tmp = (uint16_t)((inportl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
  return tmp;
}


static uint32_t get_bar0(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t lo = pci_config_read_word(bus, slot, func, 0x10);
  uint16_t hi = pci_config_read_word(bus, slot, func, 0x12);
  return ((uint32_t)hi << 16 | lo);
}


static uint32_t get_bar1(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t lo = pci_config_read_word(bus, slot, func, 0x14);
  uint16_t hi = pci_config_read_word(bus, slot, func, 0x16);
  return ((uint32_t)hi << 16 | lo);
}



static uint32_t get_bar2(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t lo = pci_config_read_word(bus, slot, func, 0x18);
  uint16_t hi = pci_config_read_word(bus, slot, func, 0x1A);
  return ((uint32_t)hi << 16 | lo);
}



static uint32_t get_bar3(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t lo = pci_config_read_word(bus, slot, func, 0x1C);
  uint16_t hi = pci_config_read_word(bus, slot, func, 0x1E);
  return ((uint32_t)hi << 16 | lo);
}


static uint32_t get_bar4(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t lo = pci_config_read_word(bus, slot, func, 0x20);
  uint16_t hi = pci_config_read_word(bus, slot, func, 0x22);
  return ((uint32_t)hi << 16 | lo);
}


static uint32_t get_bar5(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t lo = pci_config_read_word(bus, slot, func, 0x24);
  uint16_t hi = pci_config_read_word(bus, slot, func, 0x26);
  return ((uint32_t)hi << 16 | lo);
}


static void set_dev_bars(pcidev_t* dev, uint8_t bus, uint8_t slot, uint8_t func) {
  dev->bar0 = get_bar0(bus, slot, func);
  dev->bar1 = get_bar1(bus, slot, func);
  dev->bar2 = get_bar2(bus, slot, func);
  dev->bar3 = get_bar3(bus, slot, func);
  dev->bar4 = get_bar4(bus, slot, func);
  dev->bar5 = get_bar5(bus, slot, func);
}


pcidev_t pci_locate_device(uint16_t baseclass, uint16_t subclass) {
  pcidev_t ret;
  ret.valid = 0;

  for (uint8_t bus = 0; bus < 5; ++bus) {
    for (uint8_t slot = 0; slot < 32; ++slot) {
      for (uint8_t func = 0; func < 8; ++func) {
        uint16_t class = pci_config_read_word(bus, slot, func, 0xA) >> 8;
        uint16_t _subclass = pci_config_read_word(bus, slot, func, 0xA) & 0xFF;
        if (class == baseclass && subclass == _subclass) {
          ret.valid = 1;
          ret.bus = bus;
          ret.slot = slot;
          ret.func = func;
          set_dev_bars(&ret, bus, slot, func);
          return ret;
        }
      }
    }
  }

  return ret;
}
