#ifndef KESSWM_WIN_H_
#define KESSWM_WIN_H_

#include <stdint.h>

#define DEFAULT_WINDOW_BG 0xFFFFFF
#define DEFAULT_WINDOW_FG 0x928374


typedef struct {
  uint16_t x;
  uint16_t y;
} window_t;


window_t kesswm_open_window(uint32_t x, uint32_t y);
void kesswm_putstr(window_t window, const char* str);


#endif
