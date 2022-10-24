#ifndef UAPI_H_
#define UAPI_H_

#include <lib/errno.h>
#include <lib/types.h>


typedef struct _DriverNode {
  const char* id;
  void(*ioctl)(unsigned long cmd, size_t args[20]);
  struct _DriverNode* next;
} driver_node_t;


errno_t uapi_register_driver(const char* id, void(*ioctl)(unsigned long cmd, size_t args[20]));
void uapi_init_builtins(void);
driver_node_t* uapi_locate_driver(const char* id);

#endif
