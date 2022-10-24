#include <uapi/uapi.h>
#include <lib/types.h>
#include <lib/assert.h>
#include <lib/string.h>
#include <mm/heap.h>

// For builtins.
#include <drivers/video/framebuffer.h>


static driver_node_t* driverlist_base = NULL;
static driver_node_t* driverlist_head = NULL;

errno_t uapi_register_driver(const char* id, void(*ioctl)(unsigned long cmd, size_t args[20])) {
  if (driverlist_base == NULL) {
    driverlist_base = kmalloc(sizeof(driver_node_t));
    ASSERT(driverlist_base != NULL, "Could not setup UAPI.\n");
    driverlist_head = driverlist_base;
    driverlist_base->next = NULL;
  }

  /*
   *  Check if the driver id is not
   *  already taken.
   *
   */

  driver_node_t* cur = driverlist_base;
  while (cur != NULL) {
    if (cur->id != NULL) {
      if (kstrcmp(cur->id, id) == 0) {
        return -EXIT_FAILURE;
      }
    }

    cur = cur->next;
  }

  driverlist_head->next = kmalloc(sizeof(driver_node_t));

  if (driverlist_head->next == NULL) {
    return -ENOMEM;
  }

  driverlist_head = driverlist_head->next;
  driverlist_head->id = id;
  driverlist_head->ioctl = ioctl;
  driverlist_head->next = NULL;
  return EXIT_SUCCESS;
}


void uapi_init_builtins(void) {
  uapi_register_driver("fb", framebuffer_ioctl);
}


driver_node_t* uapi_locate_driver(const char* id) {
  driver_node_t* cur = driverlist_base;

  while (cur != NULL) {
    if (cur->id != NULL) {
      if (kstrcmp(cur->id, id) == 0) {
        return cur;
      }
    }

    cur = cur->next;
  }

  return NULL;
}
