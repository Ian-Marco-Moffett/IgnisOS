#ifndef LOG_H_
#define LOG_H_

#include <stdarg.h>

#define PRINTK_PANIC "\\1"
#define PRINTK_NOTE "\\2"


void printk(const char* fmt, ...);


#endif
