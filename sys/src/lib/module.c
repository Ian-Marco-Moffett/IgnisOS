#include <lib/module.h>
#include <lib/string.h>
#include <lib/limine.h>

static volatile struct limine_module_request mod_req = {
  .id = LIMINE_MODULE_REQUEST,
  .revision = 0
};


char* get_module(const char* path, uint64_t* size) {
  for (uint64_t i = 0; i < mod_req.response->module_count; ++i) {
    if (kstrcmp(mod_req.response->modules[i]->path, path) == 0) {
      *size = mod_req.response->modules[i]->size;
      return mod_req.response->modules[i]->address;
    }
  }

  return NULL;
}
