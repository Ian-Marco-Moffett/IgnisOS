#include <sys/ioctl.h>
#include <sys/fb.h>

#define VRAM_MIRROR ((uint32_t*)0x7000)

static uint32_t pitch = 0;
static uint32_t* vram_mirror = VRAM_MIRROR;


static uint32_t get_index(uint32_t x, uint32_t y) {
  if (pitch == 0)
      pitch = _IOCTL(FRAMEBUFFER_IOCTL_ID, FRAMEBUFFER_GET_PITCH, 0);

  return x + y * (pitch/4);
}


void kesswm_putpix(uint32_t x, uint32_t y, uint32_t color) {
  vram_mirror[get_index(x, y)] = color;
}
