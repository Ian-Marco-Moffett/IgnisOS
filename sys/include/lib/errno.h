#ifndef ERRNO_H_
#define ERRNO_H_

#include <lib/types.h>


enum {
  EXIT_SUCCESS = 0,
  EXIT_FAILURE = 1,
  ENOMEM = 2,
  EBUSY = 3,
  ENOENT = 4,
  ENOSPC = 5,
};

typedef int16_t errno_t;

#endif
