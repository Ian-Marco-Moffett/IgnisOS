#ifndef MOUSE_H_
#define MOUSE_H_

#include <lib/types.h>

#define PS2_MOUSE_IOCTL_ENABLE 0  // Allows mouse IRQs through.


void ps2_mouse_init(void);
void ps2_mouse_ioctl(unsigned long cmd, size_t args[20]);


#endif
