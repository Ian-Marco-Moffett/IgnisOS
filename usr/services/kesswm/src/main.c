#include <win.h>
#include <stdio.h>
#include <desktop.h>

void main(void) {
  kesswm_draw_desktop();
  window_t welcome_window = kesswm_open_window(WINDOW_CENTER, "Welcome!", 0, 0, 512, 288);
  kesswm_putstr(welcome_window, "Welcome to IgnisOS!\nIgnisOS v0.0.3 - Copyright (c) 2022 Ian Marco Moffett\n");
}
