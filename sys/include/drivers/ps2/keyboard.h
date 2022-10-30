#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <lib/types.h>


#define PS2_KEYBOARD_FETCH_KEY 0


ssize_t ps2_keyboard_ioctl(unsigned long cmd, size_t args[20]);
uint32_t ps2_keyboard_fetch_keystroke(void);


#endif
