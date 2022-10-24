#include <string.h>


size_t strlen(const char* str) {
  size_t len = 0;

  while (str[len++]);
  return len-1;
}


void memzero(uint8_t* dst, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    dst[i] = 0;
  }
}
