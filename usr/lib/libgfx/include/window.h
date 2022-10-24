#ifndef __LIBGFX_WINDOW_H
#define __LIBGFX_WINDOW_H

#include <stdint.h>


typedef struct {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
  uint32_t color;
} window_t;


window_t libgfx_open_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t bg, uint32_t fg);


#endif
