#ifndef PCI_H_
#define PCI_H_

#include <lib/types.h>

// Base class stuff.
#define PCI_MASS_STORAGE_CONTROLLER 0x01

// Subclass stuff.
#define PCI_SATA_CONTROLLER 0x06


typedef struct {
  uint8_t bus;
  uint8_t slot;
  uint8_t func;
  uint8_t valid;
  uint32_t bar0;
  uint32_t bar1;
  uint32_t bar2;
  uint32_t bar3;
  uint32_t bar4;
  uint32_t bar5;
} pcidev_t;


uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
pcidev_t pci_locate_device(uint16_t baseclass, uint16_t subclass);

#endif
