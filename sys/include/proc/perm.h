#ifndef PERMS_H_
#define PERMS_H_

#include <lib/types.h>


// Initrd access.
// TODO: Remove initrd access later.
#define PPERM_INITRD (1 << 0)

typedef uint32_t pperm_t;

#endif
