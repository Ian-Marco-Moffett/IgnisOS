#include <fs/vfs.h>
#include <mm/heap.h>
#include <lib/string.h>
#include <lib/log.h>

// FS root (i.e '/')
static fsnode_t* root_fs = NULL;
static fsnode_t* head = NULL;
static size_t next_fid = 1;

static fsnode_t* locate_root_folder(const char* folder_name) {
  fsnode_t* cur = root_fs;

  while (cur != NULL) {
    if (kstrcmp(cur->fname, folder_name) == 0) {
      return cur;
    }

    cur = cur->next;
  }

  printk("%s - %s\n", head->fname, folder_name);
  return NULL;
}


static fsnode_t* locate_file(fsnode_t* folder_node, const char* fname) {
  fsnode_t* cur = folder_node->children;

  while (cur != NULL) {
    if (kstrcmp(cur->fname, fname) == 0) {
      return cur;
    }

    cur = cur->next;
  }

  return NULL;
}

// @param create should be 1 if it should create the file
// if it detects it doesn't exist.
static errno_t parse_path(const char* path, FILE** folder_out, FILE** file_out, uint8_t create) {
  if (*path++ != '/')
    return EXIT_FAILURE;

  fsnode_t* folder;

  char fname[195];
  size_t fname_idx = 0;

  size_t i;
  for (i = 0; i < kstrlen(path); ++i) {
    if (fname_idx >= 194) {
      return EXIT_FAILURE;
    }

    if (path[i] == '/' && path[i + 1] != '\0') {
      fname[fname_idx] = '\0';
      ++i;
      break;
    } else if (path[i + 1] == '\0') {
      // Just return the folder.
      fname[fname_idx] = '\0';
      folder = locate_root_folder(fname);
      *file_out == NULL;
      *folder_out = folder;
      return EXIT_SUCCESS;
    }

    fname[fname_idx++] = path[i];
  }
  
  folder = locate_root_folder(fname);

  if (folder == NULL) {
    return EXIT_FAILURE;
  }
  
  kmemzero(fname, sizeof(fname));
  fname_idx = 0;
  for (; i < kstrlen(path); ++i) {
    // No subdirectories yet.
    if (path[i] == '/') {
      return EXIT_FAILURE;
    }

    if (fname_idx >= 194) {
      return EXIT_FAILURE;
    }

    fname[fname_idx++] = path[i];
  }

  fname[fname_idx] = '\0';

  fsnode_t* file = locate_file(folder, fname);

  errno_t ret_status = EXIT_SUCCESS;
  if (file == NULL && !(create))
    return EXIT_FAILURE;
  else if (create)
    ret_status = folder->fcreate(folder, fname);

  *file_out = file;
  *folder_out = folder;
  return ret_status;
}


errno_t vfs_init(void) {
  if (root_fs != NULL)
    return -EBUSY;

  root_fs = kmalloc(sizeof(fsnode_t));

  if (root_fs == NULL) {
    return -ENOMEM;
  }
 
  root_fs->fname[0] = '/';
  root_fs->fname[1] = '\0';
  root_fs->fidx = 0;
  root_fs->fid = 0;
  root_fs->len = 0;
  root_fs->flags = FS_DIRECTORY | FS_MOUNTPOINT;
  
  root_fs->fd.rlock = 0;
  root_fs->fd.off = 0;

  root_fs->next = NULL;
  root_fs->children = NULL;
  root_fs->n_children = 0;
  root_fs->read = NULL;
  root_fs->write = NULL;
  root_fs->fcreate = NULL;
  head = root_fs;
  return EXIT_SUCCESS;
}


void mount_filesystem(fsnode_t* fs_base) {
  fs_base->fid = next_fid++;
  head->next = fs_base;
  head = head->next;
  head->next = NULL;
}

FILE* fopen(const char* path) {  
  FILE* folder;
  FILE* file;
  if (parse_path(path, &folder, &file, 0) != EXIT_SUCCESS)
    return NULL;
  
  if (file == NULL)
    return NULL;

  if (file->fd.rlock >= 1)
    // Resource is locked.
    return NULL;

  return file;
}

void fclose(FILE* fp) {
  if (fp == NULL)
    return;

  fp->fd.rlock = 0;
  fp->fd.off = 0;
}

errno_t fread(FILE* fp, char* buf, size_t len) {
  return fp->read(fp, buf, len);
}

errno_t fcreate(const char* path) {
  FILE* unused;
  return parse_path(path, &unused, &unused, 1);
}
