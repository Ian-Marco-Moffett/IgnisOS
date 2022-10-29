#ifndef __LIBGFX_WINDOW_H
#define __LIBGFX_WINDOW_H

#define WINDOW_TRANSPARENT 1
#define WINDOW_BAR_HEIGHT  20
#define WINDOW_TITLEBAR_BG 0x5555FF

#include <stdint.h>


void libgfx_open_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t bg, uint32_t fg);


#endif
