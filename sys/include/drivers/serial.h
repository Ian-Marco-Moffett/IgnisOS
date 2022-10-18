#ifndef SERIAL_H_
#define SERIAL_H_

#ifdef __x86_64__


void serial_init(void);
void serial_write(const char* str);

#endif  // __x86_64__
#endif  // SERIAL_H_
