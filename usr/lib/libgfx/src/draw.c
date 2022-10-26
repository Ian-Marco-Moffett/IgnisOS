#include <draw.h>
#include <sys/ioctl.h>
#include <sys/fb.h>

void draw_square(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
  for (uint32_t cy = y; cy < y+height; ++cy) {
    for (uint32_t cx = x; cx < x+width; ++cx) {
      _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_PUTPIX, cx, cy, color);
    }
  }
}
