#ifndef INITRD_H_
#define INITRD_H_

#include <lib/types.h>


/*
 *  @param path: Path of file we want to open.
 *  @param size_out: Pointer to variable that will hold the size
 *                   of the initrd file.
 *
 */

const char* initrd_open(const char* path, size_t* size_out);

#endif
