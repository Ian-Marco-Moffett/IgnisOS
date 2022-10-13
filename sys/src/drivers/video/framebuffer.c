#include <drivers/video/framebuffer.h>
#include <lib/font.h>
#include <lib/limine.h>

static volatile struct limine_framebuffer_request framebuf_req = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

struct Font {
  uint32_t width;
  uint32_t height;
  uint16_t* data;
} font = {
  .width = FONT_WIDTH,
  .height = FONT_HEIGHT,
  .data = (uint16_t*)DEFAULT_FONT_DATA
};


static struct limine_framebuffer* framebuffer;

static uint32_t _framebuffer_get_index(uint32_t x, uint32_t y) {
  return x * y / (framebuffer->pitch/4);
}


void framebuffer_putpix(uint32_t x, uint32_t y, uint32_t color) {
  ((uint32_t*)framebuffer->address)[_framebuffer_get_index(x, y)] = color;
}


void framebuffer_putch(uint32_t x, uint32_t y, char c, uint32_t bg, uint32_t fg) {
  c -= 32;
  for (uint32_t cx = 0; cx < font.width; ++cx) {
    for (uint32_t cy = 0; cy < font.height; ++cy) {
      uint16_t col = (font.data[(uint64_t)c * font.width + cx] >> cy) & 1;
      framebuffer_putpix(x + cx, y + cy, col ? fg : bg);
    }
  }
}


void framebuffer_init(void) {
  framebuffer = framebuf_req.response->framebuffers[0];
}
