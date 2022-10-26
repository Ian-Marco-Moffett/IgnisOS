#include <stdlib.h>

#define COF0 13
#define COF1 17
#define COF2 5


static uint16_t _seed = 14;


void srand(uint16_t seed) {
  _seed = seed;
}


uint32_t rand(void) {
  _seed ^= _seed << COF0;
  _seed ^= _seed >> COF1;
  _seed ^= _seed << COF2;
  return _seed;
}
