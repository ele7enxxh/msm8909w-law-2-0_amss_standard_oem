/***********************************************************************
 * fs_ramfs_pm.c
 *
 * Page Manager for RamFS.
 * Copyright (C) 2010-2013 QUALCOMM Technologies, Inc.
 *
 * Page Manager for RamFS.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_ramfs_pm.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-08-19   rp    Support persistence of RAM-FS.
2013-07-22   rp    Add new EFS API to store data in SuperBlock.
2013-06-19   dks   Added NAND Flash scrub support.
2013-01-11   dks   Move writing to Golden partition outside of efs_sync.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-12-05   nr    Fix compiler warnings.
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2011-10-11   nr    Added borrow_ram support to borrow ram from lower layers.
2011-03-08   rp    Add support to lock  EFS.
2010-11-18   rp    Support TAR image Golden-Copy for RMTS.
2010-06-25   wek   Add Golden Copy op to RamFS PM.
2010-05-21   rp    Fix compiler warnings seen on target.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-05-07   rp    Create

===========================================================================*/

#include "fs_device.h"
#include "fs_util.h"
#include "fs_errno.h"

#if defined (FEATURE_EFS_EFS2_ON_RAMFS) && \
    defined (FEATURE_EFS_QEFS2)

#include "fs_ramfs_pm.h"
#include "assert.h"
#include "fs_device_efs_ramfs.h"

/* There are two magic numbers in the superblock itself.  These are to
 * reduce the likelyhood of deciding alternative data is a superblock.
 * In little endian, this is viewed in ASCII as "EFSSuper". */
#define FS_RAMFS_SUPER_MAGIC1    (0x53534645)
#define FS_RAMFS_SUPER_MAGIC2    (0x72657075)

#define FS_RAMFS_SUPER_VERSION   (1)
#define FS_RAMFS_SUPER_HEADER    (0x53000000)

static struct fs_ramfs_pm_data ramfs_pm_data_inst;

/* prototypes: */
static void fs_ramfs_pm_init_ops (fs_pm_t pm_data);

static int
fs_ramfs_is_valid_superblock (struct fs_ramfs_pm_data *pm)
{
  int result = 0;

  if (pm->superblock->header != FS_RAMFS_SUPER_HEADER)
  {
    goto End;
  }

  if (pm->superblock->version != FS_RAMFS_SUPER_VERSION)
  {
    goto End;
  }

  if ((pm->superblock->magic1 != FS_RAMFS_SUPER_MAGIC1) ||
      (pm->superblock->magic2 != FS_RAMFS_SUPER_MAGIC2))
  {
    goto End;
  }

  result = 1;

End:
  return result;
}

static void
fs_ramfs_create_superblock (struct fs_ramfs_pm_data *pm)
{
  memset (pm->superblock, 0xFF, FS_RAMFS_PAGE_SIZE_IN_BYTES);
  pm->superblock->header = FS_RAMFS_SUPER_HEADER;
  pm->superblock->version = FS_RAMFS_SUPER_VERSION;
  pm->superblock->magic1 = FS_RAMFS_SUPER_MAGIC1;
  pm->superblock->magic2 = FS_RAMFS_SUPER_MAGIC2;
  pm->superblock->age = 0;
}

fs_pm_t
fs_ramfs_pm_init (efs_device_t dev)
{
  struct fs_ramfs_pm_data *pm;
  uint8 *ram_addr;
  int is_valid_superblock;

  ASSERT (sizeof (struct fs_ramfs_superblock_data_type) ==
          FS_RAMFS_PAGE_SIZE_IN_BYTES);

  memset (&ramfs_pm_data_inst, 0, sizeof (ramfs_pm_data_inst));
  pm = &ramfs_pm_data_inst;

  ram_addr = dev->priv.ramfs_dev.ramfs_buffer;

  pm->ramfs_dev = dev;
  pm->superblock = (struct fs_ramfs_superblock_data_type *)ram_addr;
  pm->ramfs_buff = (ram_addr + sizeof (struct fs_ramfs_superblock_data_type));
  pm->ramfs_size = dev->priv.ramfs_dev.ramfs_size;
  pm->page_size = fs_device_page_size (dev);
  pm->total_clusters = (pm->ramfs_size / pm->page_size);

  fs_ramfs_pm_init_ops (&pm->parent);

  is_valid_superblock = fs_ramfs_is_valid_superblock (pm);
  if (!is_valid_superblock)
  {
    memset (ram_addr, 0xA5, pm->ramfs_size);
    fs_ramfs_create_superblock (pm);
  }

  ++pm->superblock->age;

  return &pm->parent;
}

static void
fs_ramfs_page_write (fs_pm_t pm_data, fs_cluster_id cluster,
                    void *buffer, uint16 tag)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;
  uint32 offset;
  uint8 *addr;

  (void) tag;

  ASSERT (pm != NULL);
  ASSERT (cluster < pm->total_clusters);
  ASSERT (cluster != 0);

  /* Make sure we are inside a transaction. */
  ASSERT (pm->inside_txn == 1);

  offset = cluster * pm->page_size;
  addr = pm->ramfs_buff + offset;
  fs_memscpy (addr, pm->ramfs_size - offset, buffer, pm->page_size);
}

static void
fs_ramfs_page_read (fs_pm_t pm_data, fs_cluster_id cluster, void *buffer)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;
  uint32 offset;
  uint8 *addr;

  ASSERT (pm != NULL);
  ASSERT (cluster < pm->total_clusters);
  ASSERT (cluster != 0);

  offset = cluster * pm->page_size;
  addr = pm->ramfs_buff + offset;
  fs_memscpy (buffer, pm->page_size, addr, pm->page_size);
}

static void
fs_ramfs_store_info (fs_pm_t pm_data, unsigned offset, uint32 data)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;

  ASSERT (offset < FS_UPPER_DATA_COUNT);

  ASSERT (pm != NULL);
  pm->superblock->upper_data[offset] = data;
}

static uint32
fs_ramfs_get_info (fs_pm_t pm_data, unsigned offset)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;

  ASSERT (offset < FS_UPPER_DATA_COUNT);

  ASSERT (pm != NULL);
  return pm->superblock->upper_data[offset];
}

static void
fs_ramfs_start_transaction (fs_pm_t pm_data)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;
  ASSERT (pm != NULL);

  ASSERT (pm->inside_txn == 0);
  pm->inside_txn = 1;
  /* We are volatile, nothing to do */
}

static void
fs_ramfs_end_transaction (fs_pm_t pm_data)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;
  ASSERT (pm != NULL);

  ASSERT (pm->inside_txn == 1);
  pm->inside_txn = 0;
  /* We are volatile, nothing to do */
}

static uint32
fs_ramfs_space_limit (fs_pm_t pm_data)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;

  ASSERT (pm != NULL);
  return pm->total_clusters;
}

static void
fs_ramfs_register_free_check_cb (fs_pm_t pm_data,
    int (*is_used_cb) (void *, fs_cluster_id cluster), void *data)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;

  (void) is_used_cb; (void) data;
  ASSERT (pm != NULL);
}

static void
fs_ramfs_register_alloc_count_cb (fs_pm_t pm_data,
      uint32 (*alloc_count_cb) (void *), void *data)
{
  struct fs_ramfs_pm_data *pm = (struct fs_ramfs_pm_data *)pm_data;

  (void) alloc_count_cb; (void) data;
  ASSERT (pm != NULL);
}

static void
fs_ramfs_read_iovec (fs_pm_t pm_data, fs_iovec_t iovec, uint32 iovec_cnt)
{
  uint32 i;

  for (i = 0; i < iovec_cnt; ++i)
  {
    fs_ramfs_page_read (pm_data, iovec[i].cluster_id, iovec[i].buff_base);
  }
}

static void
fs_ramfs_write_iovec (fs_pm_t pm_data, fs_iovec_t iovec, uint32 iovec_cnt)
{
  uint32 i;

  for (i = 0; i < iovec_cnt; ++i)
  {
    fs_ramfs_page_write (pm_data, iovec[i].cluster_id,iovec[i].buff_base,0);
  }
}

void
fs_ramfs_pm_erase_efs (void)
{
  struct fs_ramfs_pm_data *pm;

  pm = (struct fs_ramfs_pm_data *)&ramfs_pm_data_inst;

  memset (pm->ramfs_buff, 0xCD, pm->ramfs_size);
  fs_ramfs_create_superblock (pm);
}

static int
fs_ramfs_pm_sync_start (fs_pm_t pm_data)
{
  (void) pm_data;
  return 1; /* Nothing to sync. */
}

static int
fs_ramfs_pm_sync_wait (fs_pm_t pm_data)
{
  (void) pm_data;
  return 0; /* Nothing to sync. */
}

static int
fs_ramfs_pm_sync_end (fs_pm_t pm_data)
{
  (void) pm_data;
  return 0; /* Nothing to sync. */
}

static int
fs_ramfs_pm_sync_no_wait (fs_pm_t pm_data, fs_sync_token_t *sync_token)
{
  (void) pm_data;
  *sync_token = 0; /* Give the always success sync token. */
  return 1; /* Nothing to sync. */
}

static int
fs_ramfs_pm_sync_get_status (fs_pm_t pm_data, fs_sync_token_t sync_token,
                               enum fs_sync_result *sync_result)
{
  (void) pm_data;
  (void) sync_token;
  *sync_result = FS_SYNC_COMPLETE;
  return 0; /* Nothing to sync. */
}

static int
fs_ramfs_pm_make_golden_copy (fs_pm_t pm_data)
{
  (void) pm_data;
  return -EPERM;
}

static int
fs_ramfs_pm_fs_init_complete (fs_pm_t pm_data)
{
  (void) pm_data;
  return -EPERM;
}

static int
fs_ramfs_pm_lock_efs (fs_pm_t pm_data)
{
  (void) pm_data;
  return -EPERM;
}

static int
fs_ramfs_pm_borrow_ram (fs_pm_t pm_data, void **ram_addr, uint32 *ram_size)
{
  (void) pm_data;
  *ram_addr = NULL;
  *ram_size = 0;
  return -ENOMEM;
}

static int
fs_ramfs_pm_store_data_in_sb (fs_pm_t pm_data, const void *data,
                              uint32 data_size)
{
  (void) pm_data; (void) data; (void) data_size;
  return -EPERM;
}

static void
fs_ramfs_pm_init_ops (fs_pm_t pm_data)
{
  ASSERT (pm_data != NULL);
  pm_data->ops.page_write = fs_ramfs_page_write;
  pm_data->ops.page_read = fs_ramfs_page_read;
  pm_data->ops.store_info = fs_ramfs_store_info;
  pm_data->ops.get_info = fs_ramfs_get_info;
  pm_data->ops.start_transaction = fs_ramfs_start_transaction;
  pm_data->ops.end_transaction = fs_ramfs_end_transaction;
  pm_data->ops.space_limit = fs_ramfs_space_limit;
  pm_data->ops.register_free_check_cb = fs_ramfs_register_free_check_cb;
  pm_data->ops.register_alloc_count_cb = fs_ramfs_register_alloc_count_cb;
  pm_data->ops.shred = NULL;
  pm_data->ops.scrub = NULL;
  pm_data->ops.read_iovec = fs_ramfs_read_iovec;
  pm_data->ops.write_iovec = fs_ramfs_write_iovec;
  pm_data->ops.sync_start = fs_ramfs_pm_sync_start;
  pm_data->ops.sync_wait  = fs_ramfs_pm_sync_wait;
  pm_data->ops.sync_end   = fs_ramfs_pm_sync_end;
  pm_data->ops.sync_no_wait = fs_ramfs_pm_sync_no_wait;
  pm_data->ops.sync_get_status = fs_ramfs_pm_sync_get_status;
  pm_data->ops.make_golden_copy = fs_ramfs_pm_make_golden_copy;
  pm_data->ops.fs_init_complete = fs_ramfs_pm_fs_init_complete;
  pm_data->ops.lock_efs = fs_ramfs_pm_lock_efs;
  pm_data->ops.borrow_ram = fs_ramfs_pm_borrow_ram;
  pm_data->ops.store_data_in_sb = fs_ramfs_pm_store_data_in_sb;
}


#endif /* FEATURE_EFS_EFS2_ON_RAMFS */

