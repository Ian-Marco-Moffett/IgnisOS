#include <drivers/video/framebuffer.h>
#include <lib/limine.h>
#include <lib/string.h>
#include <uapi/uapi.h>

static volatile struct limine_framebuffer_request framebuf_req = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};


static struct Font {
  uint32_t width;
  uint32_t height;
  uint16_t* data;
} font = {
  .width = FONT_WIDTH,
  .height = FONT_HEIGHT,
  .data = (uint16_t*)DEFAULT_FONT_DATA
};

struct limine_framebuffer* framebuffer = NULL;

static uint32_t _framebuffer_get_index(uint32_t x, uint32_t y) {
  return x + y * (framebuffer->pitch/4);
}

static uint32_t getpix(uint32_t x, uint32_t y) {
  if (x > framebuffer->width-1 || y > framebuffer->height-1)
    return 0;

  return ((uint32_t*)framebuffer->address)[_framebuffer_get_index(x, y)];
}

ssize_t framebuffer_ioctl(unsigned long cmd, size_t args[20]) {
  switch (cmd) {
    case FRAMEBUFFER_IOCTL_CLEAR:
      framebuffer_clear(args[0]);
      break;
    case FRAMEBUFFER_IOCTL_PUTPIX:
      if (args[0] > framebuffer_get_width()-5 || args[1] > framebuffer_get_height()-5)
        break;
      framebuffer_putpix(args[0], args[1], args[2]);
      break;
    case FRAMEBUFFER_IOCTL_PUTSTR:
      {
        uint32_t orig_x = args[0];
        uint32_t y = args[1];
        uint32_t bg = args[3];
        uint32_t fg = args[4];
        uint32_t x = orig_x;
        const char* str = (const char*)args[2];

        for (size_t i = 0; i < kstrlen(str); ++i) {
          if (str[i] == '\n') {
            y += FONT_HEIGHT+4;
            x = orig_x;
            continue;
          }
          framebuffer_putch(x, y, str[i], bg, fg);
          x += FONT_WIDTH;
        }
      }
      break;
    case FRAMEBUFFER_GET_PIXEL:
      return getpix(args[0], args[1]);
    case FRAMEBUFFER_GET_PITCH:
      return framebuffer->pitch;
  }

  return 0;
}


void framebuffer_putpix(uint32_t x, uint32_t y, uint32_t color) {
  if (x > framebuffer->width-1 || y > framebuffer->height-1)
    return;

  ((uint32_t*)framebuffer->address)[_framebuffer_get_index(x, y)] = color;
}


void framebuffer_mirror(uint32_t* buf) {
  for (size_t i = 0; i < (framebuffer->pitch*framebuffer->height)/2; ++i) {
    ((uint32_t*)framebuffer->address)[i] = buf[i]; 
  }
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


void framebuffer_clear(uint32_t color) {
  for (uint32_t y = 0; y < framebuffer->height; ++y) {
    for (uint32_t x = 0; x < framebuffer->pitch; ++x) {
      framebuffer_putpix(x, y, color);
    }
  }
}


uint32_t framebuffer_get_height(void) {
  return framebuffer->height;
}

uint32_t framebuffer_get_width(void) {
  return framebuffer->width;
}


void framebuffer_init(void) { 
  framebuffer = framebuf_req.response->framebuffers[0];
}
