#ifndef KESSWM_WIN_H_
#define KESSWM_WIN_H_

#include <stdint.h>

#define DEFAULT_WINDOW_BG 0xC3C3C3
#define DEFAULT_WINDOW_FG 0x000082


typedef struct {
  uint16_t x;
  uint16_t y;
} window_t;


window_t kesswm_open_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void kesswm_putstr(window_t window, const char* str);


#endif
