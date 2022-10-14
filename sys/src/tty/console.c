#include <tty/console.h>
#include <drivers/video/framebuffer.h>
#include <lib/string.h>
#include <lib/types.h>

#define MAKE_FG_BG(fg, bg) ((uint64_t)fg << 32 | bg)
#define EXTRACT_FG(color) ((uint64_t)color >> 32)
#define EXTRACT_BG(color) (color & 0xFFFFFFFF)

static uint32_t x = 10, y = 50;


static uint32_t _reset_pos(void) {
  x = 0;
  y = 0;
}


static void _putstr(const char* str, uint64_t color) {
  for (size_t i = 0; i < kstrlen(str); ++i) {
    framebuffer_putch(x, y, str[i], EXTRACT_BG(color), EXTRACT_FG(color));
    x += FONT_WIDTH;
  }
}


void console_write(const char* fmt, va_list ap) {
  uint64_t color = MAKE_FG_BG(0x808080, 0x000000);

  if (*fmt == '\\' && *(fmt + 1) == '1') {
    framebuffer_clear(0x1434A4);
    _reset_pos();
    color = MAKE_FG_BG(0xFFFFFF, 0x1434A4);
    fmt += 2;
  }

  for (const char* ptr = fmt; *ptr; ++ptr) {
    if (*ptr == '%') {
      switch (*ptr) {
        case 'd':
          _putstr(dec2str(va_arg(ap, uint64_t)), color);
          break;
        case 'x':
          _putstr(hex2str(va_arg(ap, uint64_t)), color);
          break;
        case 'c':
          _putstr((char[2]){va_arg(ap, int), 0}, color);
          break;
      }
    } else {
      _putstr((char[2]){*ptr, 0}, color);
    }
  }
}
