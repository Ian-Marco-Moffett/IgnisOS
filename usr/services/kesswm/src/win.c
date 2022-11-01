#include <win.h>
#include <draw.h>

/*
void kesswm_putstr(window_t* window, const char* str, uint32_t x, uint32_t y, uint8_t use_cursor_off) {
  for (const char* ptr = str; *ptr; ++ptr) {
    if (use_cursor_off) {
      // libgfx_putch(*ptr, (window->x + 3)+window->cursor_x_off, (window->y + 10)+window->cursor_y_off, 0x000000);
      window->cursor_x_off += 8;
    } else {
      // libgfx_putch(*ptr, x, y, 0x000000);
      x += 8;
    }
  }
}
*/

window_t kesswm_open_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char* title) {
  window_t window;
  window.x = x;
  window.y = y;
  window.cursor_x_off = 0;
  window.cursor_y_off = 0;

  for (uint32_t cx = x; cx < x+width; ++cx) {
    for (uint32_t cy = y; cy < y+height; ++cy) {
      kesswm_putpix(cx, cy, DEFAULT_WINDOW_BG);
    }
  }

  // kesswm_putstr(&window, title, window.x, window.y + 10, 0);
  return window;
}

