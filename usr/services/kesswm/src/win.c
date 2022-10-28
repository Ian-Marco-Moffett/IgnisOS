#include <win.h>
#include <libgfx/window.h>
#include <sys/fb.h>
#include <sys/ioctl.h>


window_t kesswm_open_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  window_t window;
  window.x = x;
  window.y = y;
  libgfx_open_window(x, y, width, height, DEFAULT_WINDOW_BG, DEFAULT_WINDOW_FG);
  return window;
}


void kesswm_putstr(window_t window, const char* str) {
  _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_PUTSTR, window.x + 5, window.y + 20, str, DEFAULT_WINDOW_BG, 0x000000);
}
