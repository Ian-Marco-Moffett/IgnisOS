#include <fs/tmpfs.h>
#include <fs/vfs.h>
#include <lib/assert.h>
#include <lib/string.h>
#include <lib/log.h>
#include <mm/heap.h>
#include <mm/mmap.h>
#include <mm/pmm.h>

#define DIRNAME "tmp"
#define TMPFS_POOL_VADDR pmm_alloc_frame()
#define TMPFS_BLOCK_SIZE 50
#define TMPFS_POOL_N_PAGES 1

static fsnode_t* fs_base = NULL;
static uint8_t* mpool = NULL;
static size_t mpool_idx = 0;
static size_t fidx = 0;

typedef struct {
  size_t data_size;
  char mag[3];
  size_t volume_blockcount;
} tmpfs_superblock_t;


typedef struct {
  const char* filename;
  char* data_start;
} tmpfs_file_t;


static errno_t read(struct _FSNode* _this, char* buf, size_t len) {
  tmpfs_file_t* file = (tmpfs_file_t*)(mpool + _this->fidx);

  if (file == NULL) {
    return -ENOENT;
  } 

  for (size_t i = 0; i < len; ++i) {
    buf[i] = file->data_start[i];
  }

  return EXIT_SUCCESS;
}

static errno_t write(struct _FSNode* _this, const char* buf, size_t len) {  
  /*
   *  Reallocate file data buffer 
   *  and append new data.
   *
   */

  tmpfs_file_t* file = (tmpfs_file_t*)(mpool+_this->fidx);

  file->data_start = krealloc(file->data_start, ((_this->len+len)+1)*(sizeof(char)));

  size_t bufidx = 0;
  for (size_t i = _this->fd.off; i < _this->fd.off+len; ++i, ++bufidx) {
    file->data_start[i] = buf[bufidx];
  } 

  _this->fd.off += len;
  _this->len += len;
  file->data_start[_this->len-1] = EOF;
  return EXIT_SUCCESS;
}

static void init_node(struct _FSNode* parent, fsnode_t* node, const char* name, size_t len) {
  kmemcpy(node->fname, name, kstrlen(name));
  node->fidx = mpool_idx;
  node->fid = 0;
  node->len = len;
  node->flags = FS_FILE;
  node->fd.rlock = 0;
  node->fd.off = 0;
  node->next = NULL;
  node->children = NULL;
  node->n_children = 0;
  node->read = read;
  node->write = write;
  node->fcreate = NULL;
  node->parent = parent;
}

static errno_t _fcreate(struct _FSNode* _this, const char* name) {
  tmpfs_file_t* file_slot = (tmpfs_file_t*)(mpool + mpool_idx);
  file_slot->data_start = kmalloc(1);
  *file_slot->data_start = EOF;

  // Add file as child node.
  fsnode_t* child_base = _this->children; 
  for (uint32_t i = 0; i < _this->n_children && child_base != NULL; ++i) {
    child_base = child_base->next;
  }

  if (_this->n_children > 0) {
    child_base->next = kmalloc(sizeof(fsnode_t));
    init_node(_this, child_base->next, name, 1);
  } else {
    _this->children = kmalloc(sizeof(fsnode_t));
    init_node(_this, _this->children, name, 1);
  }

  return EXIT_SUCCESS;
}


static void init_mpool(void) {
  tmpfs_superblock_t* superblock = (tmpfs_superblock_t*)mpool;
  superblock->data_size = (TMPFS_POOL_N_PAGES*4096)-sizeof(tmpfs_superblock_t);
  superblock->volume_blockcount = superblock->data_size/TMPFS_BLOCK_SIZE;

  superblock->mag[0] = 'T';
  superblock->mag[1] = 'M';
  superblock->mag[2] = 'P';

  mpool_idx = sizeof(tmpfs_superblock_t);
}


void tmpfs_init(void) {
  fs_base = kmalloc(sizeof(fsnode_t));
  ASSERT(fs_base != NULL, "Not enough memory for initializing tmpfs!\n");

  kmemcpy(fs_base->fname, DIRNAME, kstrlen(DIRNAME));
  fs_base->fidx = 0;
  fs_base->len = 0;
  fs_base->flags = FS_DIRECTORY;

  fs_base->fd.rlock = 0;
  fs_base->fd.off = 0;

  fs_base->next = NULL;

  fs_base->children = kmalloc(sizeof(fsnode_t));
  ASSERT(fs_base->children != NULL, "Not enough memory for initializing tmpfs!\n");

  fs_base->children->next = NULL;
  fs_base->n_children = 0;
  fs_base->read = read;
  fs_base->write = write;
  fs_base->fcreate = _fcreate;

  printk("[INFO]: Mounting tmpfs to /tmp..\n");
  mount_filesystem(fs_base);

  printk("[INFO]: Mapping memory pool for tmpfs..\n");
  
  errno_t errno = mmap((void*)TMPFS_POOL_VADDR, TMPFS_POOL_N_PAGES, PROT_READ | PROT_WRITE);
  mpool = (uint8_t*)TMPFS_POOL_VADDR;

  ASSERT(errno != -ENOMEM, "Could not map memory pool for tmpfs!\n");
  
  printk("[INFO]: Initializing tmpfs memory pool..\n");
  init_mpool();
  printk("[INFO]: tmpfs initialized.\n");
}
