#include <win.h>
#include <libgfx/window.h>
#include <sys/fb.h>
#include <sys/ioctl.h>


window_t kesswm_open_window(uint32_t flags, char* title, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  window_t window;
  window.flags = flags;
  window.title = title;
  window.x = x;
  window.y = y;
  window.width = width;
  window.height = height;

  // Apply flags
  if(flags & WINDOW_CENTER) flags |= (WINDOW_CENTERX | WINDOW_CENTERY);
  if(flags & WINDOW_CENTERX) window.x = (1980 / 2) - (width / 2);
  if(flags & WINDOW_CENTERY) window.y = (1080 / 2) - (height / 2);

  libgfx_open_window(window.x, window.y, window.width, window.height, DEFAULT_WINDOW_BG, DEFAULT_WINDOW_FG);
  _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_PUTSTR, window.x + 2, window.y + 2, window.title, WINDOW_TITLEBAR_BG, DEFAULT_WINDOW_BG, window.width - (DEFAULT_WINDOW_PAD * 2));
  return window;
}


void kesswm_putstr(window_t window, const char* str) {
  _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_PUTSTR, window.x + DEFAULT_WINDOW_PAD, window.y + WINDOW_BAR_HEIGHT + DEFAULT_WINDOW_PAD, str, DEFAULT_WINDOW_BG, DEFAULT_WINDOW_FG, window.width - (DEFAULT_WINDOW_PAD * 2));
}
