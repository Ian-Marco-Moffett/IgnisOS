#ifndef LOG_H_
#define LOG_H_

#include <stdarg.h>

#define PRINTK_PANIC "\\1"
#define PRINTK_NOTE "\\2"
#define PRINTK_CLEAR "\\3"


void printk(const char* fmt, ...);
void log_disable_screenlog(void);
void log_enable_screenlog(void);


#endif
