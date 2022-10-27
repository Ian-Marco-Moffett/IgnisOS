#ifndef __IOCTL_H
#define __IOCTL_H

#include <sys/syscall.h>


#define _IOCTL(device_id, command, ...) syscall(0x2, device_id, command, __VA_ARGS__);


#endif
