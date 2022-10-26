#include <stdio.h>
#include <sys/fb.h>
#include <sys/ioctl.h>
#include <window.h>

__attribute__((naked)) void main(void) {
  _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_CLEARSCREEN, 0x076678);
  libgfx_open_window(50, 50, 150, 150, 0xFFFFFF, 0x928374);

  _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_PUTSTR, 55, 80, "Welcome\nto\nIgnisOS!\nMore stuff\ncoming soon!", 0xFFFFFF, 0x000000);

  while (1);
}
