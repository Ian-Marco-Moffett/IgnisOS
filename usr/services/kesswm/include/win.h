#ifndef KESSWM_WIN_H_
#define KESSWM_WIN_H_

#include <stdint.h>

#define DEFAULT_WINDOW_BG 0xEBDBB2
#define DEFAULT_WINDOW_FG 0x458588


typedef struct {
  uint16_t x;
  uint16_t y;
  uint32_t cursor_x_off;
  uint32_t cursor_y_off;
} window_t;


window_t kesswm_open_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char* title);
void kesswm_putstr(window_t* window, const char* str, uint32_t x, uint32_t y, uint8_t use_cursor_off);

#endif
