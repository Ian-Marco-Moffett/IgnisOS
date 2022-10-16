#ifndef VFS_H_
#define VFS_H_

#include <lib/types.h>
#include <lib/errno.h>

#define FS_FILE       (1 << 0)
#define FS_DIRECTORY  (1 << 1)
#define FS_MOUNTPOINT (1 << 2)
#define EOF 0x05

typedef uint8_t fsattr_t;


/*
 *  File descriptor.
 *
 *  rlock: Resource lock.
 *  off: File offset (i.e how far we are into the file).
 *
 */

typedef struct {
  uint8_t rlock;
  size_t off;
} filedesc_t;


/*
 *  fname: File name.
 *  fidx: Index into some structure that holds a collection of file data.
 *  fid:  Folder identifier, things may break if you mess with it - set by the vfs.
 *  len: Length of file.
 *  flags: Flags identifying the type of file.
 *  fd: File descriptor.
 *  next: Next node.
 *  children: Folders/files that belong to this node.
 *  parent: Parent folder.
 *  n_children: Child count.
 *  read/write: Filesystem callbacks.
 *  fcreate: Creates a file (file system callback).
 *
 *
 */


typedef struct _FSNode {
  char fname[195];
  size_t fidx;
  size_t fid;
  size_t len;
  fsattr_t flags;
  filedesc_t fd;
  struct _FSNode* next;
  struct _FSNode* children;
  struct _FSNode* parent;
  size_t n_children;
  errno_t(*read)(struct _FSNode* _this, char* buf, size_t len);
  errno_t(*write)(struct _FSNode* _this, const char* buf, size_t len);
  errno_t(*fcreate)(struct _FSNode* _this, const char* name);
} fsnode_t;

typedef fsnode_t FILE;

errno_t vfs_init(void);
void mount_filesystem(fsnode_t* fs_base);

FILE* fopen(const char* path);
void fclose(FILE* fp);
errno_t fread(FILE* fp, char* buf, size_t len);
errno_t fcreate(const char* path);

#endif
