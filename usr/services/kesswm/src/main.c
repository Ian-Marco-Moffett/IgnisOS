#include <win.h>
#include <stdio.h>
#include <desktop.h>

void main(void) {
  while (1) {
    kesswm_draw_desktop();
    window_t welcome_window = kesswm_open_window(50, 50, 300, 300, "Welcome :-)");
    kesswm_putstr(welcome_window, "Welcome to\nIgnisOS!");
  }
}
