#include <window.h>
#include <sys/ioctl.h>
#include <sys/fb.h>

#define WINDOW_BAR_HEIGHT 17


static void draw_square(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
  for (uint32_t cy = y; cy < y+height; ++cy) {
    for (uint32_t cx = x; cx < x+width; ++cx) {
      _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_PUTPIX, cx, cy, color);
    }
  }
}


static void draw_window_bar(uint32_t win_x, uint32_t win_y, uint32_t width, uint32_t bar_color) {
  draw_square(win_x, win_y, width, WINDOW_BAR_HEIGHT, bar_color);
}


void libgfx_open_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t bg, uint32_t fg) {
  draw_square(x, y, width, height, bg);
  draw_window_bar(x, y, width, fg);
}
