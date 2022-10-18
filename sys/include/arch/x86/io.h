#ifndef IO_H_
#define IO_H_

#ifdef __x86_64__

#include <lib/types.h>

unsigned char inportb(unsigned short port);
void outportb(unsigned short port, unsigned char data);
void outportw(unsigned short port, unsigned short data);
unsigned short inportw(unsigned short port);
void outportl(unsigned short port, uint32_t data);
uint32_t inportl(unsigned short port);
void io_wait();

#endif
#endif
