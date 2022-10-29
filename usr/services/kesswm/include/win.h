#ifndef KESSWM_WIN_H_
#define KESSWM_WIN_H_

#include <stdint.h>

#define DEFAULT_WINDOW_BG  0xFFFFFF
#define DEFAULT_WINDOW_FG  0x000000
#define DEFAULT_WINDOW_PAD 10

#define WINDOW_NORMAL  0
#define WINDOW_CENTERX (1 << 0)
#define WINDOW_CENTERY (1 << 1)
#define WINDOW_CENTER  (1 << 2)

typedef struct {
  uint32_t flags;
  char* title;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} window_t;


window_t kesswm_open_window(uint32_t flags, char* title, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void kesswm_putstr(window_t window, const char* str);


#endif
