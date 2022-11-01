#ifndef INITRD_H_
#define INITRD_H_

#include <lib/types.h>


/*
 *  @param path: Path of file we want to open.
 *
 */

const char* initrd_open(const char* path, size_t* size_out);

#endif
