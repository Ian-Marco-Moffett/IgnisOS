#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <lib/font.h>
#include <lib/types.h>

#define FRAMEBUFFER_IOCTL_CLEAR 0
#define FRAMEBUFFER_IOCTL_PUTPIX 1
#define FRAMEBUFFER_IOCTL_PUTSTR 2


void framebuffer_putpix(uint32_t x, uint32_t y, uint32_t color);
void framebuffer_putch(uint32_t x, uint32_t y, char c, uint32_t bg, uint32_t fg);
void framebuffer_clear(uint32_t color);
void framebuffer_init(void);
uint32_t framebuffer_get_height(void);
uint32_t framebuffer_get_width(void);
ssize_t framebuffer_ioctl(unsigned long cmd, size_t args[20]);

#endif
