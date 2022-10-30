#ifndef __LIBGFX_DRAW_H
#define __LIBGFX_DRAW_H

#include <stdint.h>

#define DRAW_FLAGS_TRANSPARENT (1 << 0)

#define WINDOW_ALPHA 0.9

void libgfx_draw_square(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void libgfx_set_bg(uint32_t _bg);

#endif
