#ifndef ASSERT_H_
#define ASSERT_H_

#include <lib/log.h>
#include <lib/panic.h>

#define ASSERT(condition, failmsg)                                                \
  if (!(condition)) {                                                             \
    printk(PRINTK_PANIC "-- ASSERTION \"" #condition "\" FAILED --\n" failmsg);  \
    kpanic();                                                                     \
  }


#define ASSERTF(condition, fmt, ...)                                                       \
  if (!(condition)) {                                                                      \
    printk(PRINTK_PANIC "-- ASSERTION \"" #condition "\" FAILED --\n" fmt, __VA_ARGS__);   \
    kpanic();                                                                              \
  }

#endif
