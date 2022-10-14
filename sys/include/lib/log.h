#ifndef LOG_H_
#define LOG_H_

#include <stdarg.h>

#define PRINTK_PANIC "\\1"


void printk(const char* fmt, ...);


#endif
