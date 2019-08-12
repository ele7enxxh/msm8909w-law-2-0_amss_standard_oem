/**********************************************************************
 * fs_buffer.c
 *
 * Buffer layer implementation.
 * Copyright (C) 2003-2013 QUALCOMM Technologies, Inc.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_buffer.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-07-22    rp      Add new EFS API to store data in SuperBlock.
2013-06-19    dks     Added NAND Flash scrub support.
2013-05-07    wek     Add EFS2 Read Only support.
2013-01-11    dks     Move writing to Golden partition outside of efs_sync.
2012-12-18    dks     Update golden copy strategy for new rmts layout.
2012-12-18    dks     Make number of EFS buffers configurable.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2011-12-01    wek     Avoid calling shred when it is not implemented.
2011-10-11    nr      Add borrow ram vnode/pm operation.
2011-08-01    nr      Change EFS Buffer use and replacement strategy.
2011-04-04    wek     Fixed error message in err fatals.
2011-03-08    rp      Add support to lock EFS.
2010-11-18    rp      Support TAR image Golden-Copy for RMTS.
2010-10-20    wek     Make buffer layer independent of page size macro.
2010-10-12    vr      Change fs_iovec parameter from num_sector to size.
2010-09-01    wek     Remove compile time calculations based on page size.
2010-05-13    wek     Make sync_no_wait non blocking.
2010-05-06    wek     Implement golden copy for remote storage page manager.
2010-01-19    vr      Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-09-24    wek     Moved caml/compatibility.h to the top of includes.
2009-04-22    ebb     Added memory protection.
2008-12-12    rp      Introducing i/o-vec interface.
2008-11-03    rp      Introduce device abstraction.
2008-10-21    rp      In find_buffer(), avoid stale buffers, fill them with ff.
2008-10-06    np      Added b_shred to support efs_shred.
2008-01-02    yg      Updated write wire param to enum type.
2007-12-28    yg      Updated tag names to new enum tag names.
2007-10-18    rp      Featurized QEFS2.
2007-09-07    rp      Use FS_IS_QUOTA_RESERVED_GROUP from fs_sys_types.h
2007-08-10    rp      Align fsb_nand_data buffers on a 32-byte boundary.
2007-06-29    sch     Associated appropriate tags with different types of data
2007-06-22    sch     Pass a hint to find_buffer to read the cluster
2007-03-06    dlb     Create alloc_count function for page manager.
2006-12-13    dlb     Move end transaction out of freemap init loop.
2006-12-05    dlb     Max transaction size configurable.
2006-11-27    dlb     Fix freemap init flushing on large devices.
2006-11-27    dlb     Check freemap size with 'if' instead of 'assert'.
2006-11-15    dlb     Fix freemap base problem on restart.
2006-11-14    dlb     Remove stray printf.
2006-11-09    dlb     Add pm query of freemap.
2006-11-02    dlb     Delete recovery support.
2006-10-26    dlb     Put freemap changes back.
2006-10-11    dlb     Back out freemap changes for now.
2006-10-10    dlb     Partial transaction flush support.
2006-10-02    dlb     Deallocate uses freemap, not chain.
2006-09-25    sh      Remove "inline" keyword breaking ADS120
2006-09-18    dlb     Cache freemap as independent data structure.
2006-05-02    sh      Q&R fields must persist across reboot.
2006-03-23    dlb     Always indicate freemap is allocated.
2006-01-27    sh      Renamed some Q&R defines to be more explanatory
2006-01-03    nrs     More Lint Cleanup
2005-12-22    dlb     Fix charge and refund re general pool.
2005-10-26     sh     Lint cleanup.
2005-10-25    nrs     Create charge and refund functions Q&R understandable
2005-10-14    nrs     Clean up to make more readable and logical
2005-10-11    nrs     Fixed bug in allocation for when file system is full
2005-10-11    nrs     Remove FS_ALLOC_HARD and FS_ALLOC_SOFT
2005-09-28    nrs     Make GID 0 a special case
2005-09-19    nrs     Implement quota limits
2005-09-13    nrs     Implement reservation management for efs_chown ()
2005-09-12    nrs     Implement reservation accounting
2005-08-25    nrs     Implent buffer accounting for quotas and reservations
2005-07-21    nrs     Added set and get for quotas and reservations
2005-05-26    sd      Compilation fixes for L4.
2005-04-26    dlb     Add 2K page support.
2005-01-04    dlb     Update copyright line.
2004-12-30    dlb     Provide hooks for new factory image code.
2004-12-01    dlb     Eliminate spurious start transaction in rollback.
2004-11-14    dlb     Fix test environment change.
2004-11-05    dlb     Additional checking of buffers in unit test.
2004-10-15    dlb     Fix copyright line.
2003-12-04    dlb     Freemap setup works for large flashes.
2003-10-23    dlb     Internal clusters use wires.
2003-10-08    dlb     Tree root is no longer wiped.
2003-09-09    dlb     Factory image works with bitmap and tree.
2003-08-28    dlb     Bitmap allocate/deallocate with move from free.
2003-08-20    dlb     Add tree allocate/deallocate.
2003-06-17    jkl     Clean up code.
2003-04-21     gr     Renamed and updated comments.
2003-04-04    bgc     Added fatal error if free_wire does a write outside a
                      transaction.
2003-01-24    bgc     Added call to fs_gc_dealloc() in b_dealloc() to mark
                      pages as garbage and clusters as deallocated.
2002-08-09    drh     Created by dlb.  Added history header

===========================================================================*/

#ifdef FS_UNIT_TEST
#include <caml/compatibility.h>
#endif

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QEFS2

#include "fs_buffer.h"

#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_errno.h"
#include "fs_iovec.h"
#include "fs_protected_memory.h"
#include "assert.h"

#include <string.h>

#ifdef FS_UNIT_TEST

struct MD5Context {
  uint32 buf[4];
  uint32 bits[2];
  uint8  in[64];
};

void MD5Init (struct MD5Context *context);
void MD5Update (struct MD5Context *context,
    uint8 *buf, unsigned len);
void MD5Final (uint8 *digest, struct MD5Context *ctx);

void (*fsb_transaction_callback) (void) = NULL;
#endif


/*  FS IOVEC buffers passed to PM to do the buffer writes in one shot. */
static struct fs_iovec iovec_buffer[FS_MAX_IOVEC_ITEMS_COUNT];


/* Methods we implement. */
static unsigned b_cluster_size (fsb_t imp);
static void b_start_transaction (fsb_t imp);
static void b_end_transaction (fsb_t imp);
static void b_rollback_transaction (fsb_t imp);
static int b_is_too_large (fsb_t imp);
static void b_partial_transaction_flush (fsb_t imp);
static int b_allocate (fsb_t imp, uint32 gid, fs_cluster_id* id);
static void * b_read_wire (fsb_t imp, fs_cluster_id id);
static void * b_write_wire (fsb_t imp, fs_cluster_id id,
                            fs_read_hint read_hint,
                            fs_buffer_data_tag_type tag);
static void b_free_wire (fsb_t imp, fs_cluster_id id);
static void b_deallocate (fsb_t imp, fs_cluster_id id, uint32 gid);
static void b_deallocate_recovered (fsb_t imp, fs_cluster_id id);
static void b_deallocate_tree (fsb_t imp, fs_cluster_id id, unsigned depth,
                               unsigned count, uint32 gid);
static uint32 b_get_field (fsb_t imp, int field_id);
static void b_set_field (fsb_t imp, int field_id, uint32 value);
static unsigned b_get_write_count (fsb_t imp);
static fs_cluster_id b_get_space_limit (fsb_t imp);
static fs_cluster_id b_get_num_alloc (fsb_t imp);
static int b_set_reservation (fsb_t imp, uint32 gid, uint32 bytes);
static int b_set_quota (fsb_t imp, uint32 gid, uint32 bytes);
static int b_get_group_info (fsb_t imp, uint32 gid,
    struct fs_group_info *ginfo);
static int b_chown (fsb_t imp, int old_uid, int new_uid, int old_gid,
    int new_gid, uint32 blocks);
static int b_shred (fsb_t imp);
static int b_scrub (fsb_t imp);
static int b_sync_start (fsb_t imp);
static int b_sync_wait (fsb_t imp);
static int b_sync_end (fsb_t imp);
static int b_sync_no_wait (fsb_t imp, fs_sync_token_t *sync_token);
static int b_sync_get_status (fsb_t imp, fs_sync_token_t sync_token,
                              enum fs_sync_result *sync_result);
static int b_make_golden_copy (fsb_t imp);
static int b_fs_init_complete (fsb_t imp);
static int b_lock_efs (fsb_t imp);
static int b_borrow_ram (fsb_t imp, void **ram_addr, uint32 *ram_size);
static int b_store_data_in_sb (fsb_t imp, const void *data, uint32 data_size);

static int freemap_init_read_only (fsb_nand_t buf);
static void freemap_init (fsb_nand_t buf);
static void freemap_dealloc (fsb_nand_t buf, fs_cluster_id id);
static fs_cluster_id alloc_from_free_chain (fsb_nand_t buf);

static void wired_init (fsb_wired_t wired);
static void wired_flush (fsb_nand_t buf, fsb_wired_t wired);
static void wired_ready_cluster (fsb_nand_t buf, fsb_wired_t wired,
    fs_cluster_id cluster, int for_write);

static void read_all_fields (fsb_nand_t buf);
static unsigned lowest_set_bit (uint32 value);

static int charge_group (fsb_t imp, uint32 gid, uint32 blocks);
static void refund_group (fsb_t imp, uint32 gid, uint32 blocks);
static int quota_exceeded  (fsb_t imp, uint32 gid, uint32 blocks);
static unsigned freemap_flush (fsb_nand_t buf, unsigned limit,
                               fs_iovec_t iovec, uint32 iovec_cnt);
static void freemap_revert (fsb_nand_t buf);
static int buf_is_used (void *, fs_cluster_id cluster);
static uint32 buf_alloc_count (void *);


/* Read only stubs for invalid buffer ops. */
static void
b_start_transaction_invalid (fsb_t imp)
{
  (void) imp;
}

static void
b_end_transaction_invalid (fsb_t imp)
{
  (void) imp;
}

static void
b_rollback_transaction_invalid (fsb_t imp)
{
  (void) imp;
}

static int
b_is_too_large_invalid (fsb_t imp)
{
  (void) imp;
  return 0;
}

static void
b_partial_transaction_flush_invalid (fsb_t imp)
{
  (void) imp;
}

static int
b_allocate_invalid (fsb_t imp, uint32 gid, fs_cluster_id* id)
{
  (void) imp; (void) gid; (void) id;
  return -EPERM;
}

static void *
b_write_wire_invalid (fsb_t imp, fs_cluster_id id,
                            fs_read_hint read_hint,
                            fs_buffer_data_tag_type tag)
{
  (void) imp; (void) id; (void) read_hint; (void) tag;
  FS_ERR_FATAL ("Writes not supported!", 0, 0, 0);
  return NULL;
}

static void
b_deallocate_invalid (fsb_t imp, fs_cluster_id id, uint32 gid)
{
  (void) imp; (void) id; (void) gid;
}

static void
b_deallocate_recovered_invalid (fsb_t imp, fs_cluster_id id)
{
  (void) imp; (void) id;
}

static void
b_deallocate_tree_invalid (fsb_t imp, fs_cluster_id id, unsigned depth,
                               unsigned count, uint32 gid)
{
  (void) imp; (void) id; (void) depth; (void) count; (void) gid;
}

static void
b_set_field_invalid (fsb_t imp, int field_id, uint32 value)
{
  (void) imp; (void) field_id; (void) value;
}

static unsigned
b_get_write_count_invalid (fsb_t imp)
{
  (void) imp;
  return 0;
}

static int
b_set_reservation_invalid (fsb_t imp, uint32 gid, uint32 bytes)
{
  (void) imp, (void) gid; (void) bytes;
  return -EPERM;
}

static int
b_set_quota_invalid (fsb_t imp, uint32 gid, uint32 bytes)
{
  (void) imp; (void) gid; (void) bytes;
  return -EPERM;
}

static int
b_get_group_info_invalid (fsb_t imp, uint32 gid,
                             struct fs_group_info *ginfo)
{
  (void) imp; (void) gid; (void) ginfo;
  return -EPERM;
}

static int
b_chown_invalid (fsb_t imp, int old_uid, int new_uid, int old_gid,
    int new_gid, uint32 blocks)
{
  (void) imp; (void) old_uid; (void) new_uid; (void) old_gid;
  (void) new_gid; (void) blocks;
  return -EPERM;
}

static int
b_shred_invalid (fsb_t imp)
{
  (void) imp;
  return -EPERM;
}

static int
b_scrub_invalid (fsb_t imp)
{
  (void) imp;
  return -EPERM;
}


static int
b_sync_start_invalid (fsb_t imp)
{
  (void) imp;
  return -EPERM;
}

static int
b_sync_wait_invalid (fsb_t imp)
{
  (void) imp;
  return -EPERM;
}

static int
b_sync_end_invalid (fsb_t imp)
{
  (void) imp;
  return -EPERM;
}

static int
b_sync_no_wait_invalid (fsb_t imp, fs_sync_token_t *sync_token)
{
  (void) imp, (void) *sync_token;
  return -EPERM;
}

static int
b_sync_get_status_invalid (fsb_t imp, fs_sync_token_t sync_token,
                              enum fs_sync_result *sync_result)
{
  (void) imp; (void) sync_token; (void) sync_result;
  return -EPERM;
}

static int
b_make_golden_copy_invalid (fsb_t imp)
{
  (void) imp;
  return -EPERM;
}

static int
b_fs_init_complete_invalid (fsb_t imp)
{
  (void) imp;
  return -EPERM;
}

static int
b_lock_efs_invalid (fsb_t imp)
{
  (void) imp;
  return -EPERM;
}

static int
b_borrow_ram_invalid (fsb_t imp, void **ram_addr, uint32 *ram_size)
{
  (void) imp; (void) ram_addr; (void) ram_size;
  return -EPERM;
}

static int
b_store_data_in_sb_invalid (fsb_t imp, const void *data, uint32 data_size)
{
  (void) imp; (void) data; (void) data_size;
  return -EPERM;
}

static int
buf_is_used_invalid (void *data, fs_cluster_id cluster)
{
  (void) data; (void) cluster;
  return 1;
}

static uint32
buf_alloc_count_invalid (void *data)
{
  (void) data;
  return 0;
}

static void
fsb_init_helper (fsb_nand_t buf, fs_pm_t gc_data, uint32 cluster_size)
{
  int i;
  /* Compute the buffer mask for the number of buffers declared. To avoid a
   * modulus on each cache lookup, we use a mask and some logic to determine
   * the cache value. This value must be defined as  2^(n+1)-1, where
   * 2^n <= FS_BUFFER_NUMBER_BUFFERS < 2^(n+1). For example, for a number of
   * buffers 8-15, you would use 15, for 16-31 use 31, for 32-63, use 63 etc.
   */
  buf->fs_buffer_number_mask = 1;
  i = FS_BUFFER_NUMBER_BUFFERS;
  while (i > 0)
  {
    i = i >> 1;
    buf->fs_buffer_number_mask = buf->fs_buffer_number_mask << 1;
  }
  buf->fs_buffer_number_mask--;

  /* Validate the pointers, Make sure the pointers are hooked-up
     to an aligned address as required by DMA. */
  for (i = 0; i < FS_BUFFER_NUMBER_BUFFERS; i++)
  {
    ASSERT (FS_OKAY_FOR_DM (buf->buffers[i]));
  }
  ASSERT (FS_OKAY_FOR_DM (buf->freemap.bitmap));

  buf->current_time = 1;

  buf->gc = gc_data;
  buf->cluster_size = cluster_size;

  for (i = 0; i < FS_BUFFER_NUMBER_BUFFERS; i++)
  {
    buf->info[i].state = FS_BSTATE_FREE;
    buf->info[i].cluster = FS_INVALID_CLUSTER_ID;
    buf->info[i].access_time = 0;
  }

  wired_init (&buf->chain);
  buf->transaction_nest = 0;

  read_all_fields (buf);

  /* For each Q&R group's fields... */
  for (i = 1; i <= FS_QR_GROUP_COUNT; i++)
  {
    if (buf->imp.get_field (&buf->imp, FS_GROUP_RESERVATION_SIZE(i))
        == FS_INVALID_CLUSTER_ID)
    {
      /* Reset all Q&R fields for this group */
      buf->imp.set_field (&buf->imp, FS_GROUP_QUOTA_SIZE(i), FS_MAX_RES_QUOTA);
      buf->imp.set_field (&buf->imp, FS_GROUP_RESERVATION_SIZE(i), 0);
      buf->imp.set_field (&buf->imp, FS_GROUP_SPACE_USED(i), 0);
    }
  }

  /* Compute the hard space limit, if needed. Otherwise read the value from
   * the superblock. */
  buf->space_limit = buf->imp.get_field (&buf->imp, FS_FIELD_SPACE_LIMIT);
  if (buf->space_limit == FS_INVALID_CLUSTER_ID)
  {
    buf->space_limit = buf->gc->ops.space_limit (buf->gc);
    buf->imp.set_field (&buf->imp, FS_FIELD_SPACE_LIMIT, buf->space_limit);
  }

  for (i = 0; i < FSB_MAX_DEPTH; i++)
  {
    wired_init (&buf->tree[i]);
  }
}


int
fsb_buffer_init_read_only (fsb_nand_t buf, fs_pm_t gc_data,
                           uint32 cluster_size)
{
  int result;

   /* Check for: 1. Freemap multiple of page size.
   *             2. Cluster size is larger than the the minimum supported.
   *             3. Cluster size is power of 2.
   *             4. There are enough iovecs to hold all the buffers.
   * If one of this conditions is not met, return error. */
  if ((FS_BUFFER_FREEMAP_SIZE % cluster_size) != 0 ||
      cluster_size < FS_MIN_PAGE_SIZE              ||
      (cluster_size & (cluster_size - 1)) != 0     ||
      FS_MAX_IOVEC_ITEMS_COUNT < FS_BUFFER_NUMBER_BUFFERS)
  {
    return -1;
  }

  buf->imp.cluster_size = b_cluster_size;
  buf->imp.start_transaction = b_start_transaction_invalid;
  buf->imp.end_transaction = b_end_transaction_invalid;
  buf->imp.rollback_transaction = b_rollback_transaction_invalid;
  buf->imp.is_too_large = b_is_too_large_invalid;
  buf->imp.partial_transaction_flush = b_partial_transaction_flush_invalid;
  buf->imp.allocate = b_allocate_invalid;
  buf->imp.read_wire = b_read_wire;
  buf->imp.write_wire = b_write_wire_invalid;
  buf->imp.free_wire = b_free_wire;
  buf->imp.deallocate = b_deallocate_invalid;
  buf->imp.deallocate_tree = b_deallocate_tree_invalid;
  buf->imp.deallocate_recovered = b_deallocate_recovered_invalid;
  buf->imp.get_field = b_get_field;
  buf->imp.set_field = b_set_field_invalid;
  buf->imp.get_write_count = b_get_write_count_invalid;
  buf->imp.get_space_limit = b_get_space_limit;
  buf->imp.get_num_alloc = b_get_num_alloc;
  buf->imp.set_reservation = b_set_reservation_invalid;
  buf->imp.set_quota = b_set_quota_invalid;
  buf->imp.get_group_info = b_get_group_info_invalid;
  buf->imp.chown = b_chown_invalid;
  buf->imp.shred = b_shred_invalid;
  buf->imp.scrub = b_scrub_invalid;
  buf->imp.sync_start = b_sync_start_invalid;
  buf->imp.sync_wait  = b_sync_wait_invalid;
  buf->imp.sync_end   = b_sync_end_invalid;
  buf->imp.sync_no_wait = b_sync_no_wait_invalid;
  buf->imp.sync_get_status = b_sync_get_status_invalid;
  buf->imp.make_golden_copy = b_make_golden_copy_invalid;
  buf->imp.fs_init_complete = b_fs_init_complete_invalid;
  buf->imp.lock_efs = b_lock_efs_invalid;
  buf->imp.borrow_ram = b_borrow_ram_invalid;
  buf->imp.store_data_in_sb = b_store_data_in_sb_invalid;

  fsb_init_helper (buf, gc_data, cluster_size);

  result = freemap_init_read_only (buf);
  if (result != 0)
    return -1;

  buf->gc->ops.register_free_check_cb (buf->gc, buf_is_used_invalid,
                                       (void *) buf);
  buf->gc->ops.register_alloc_count_cb (buf->gc, buf_alloc_count_invalid,
                                        (void *) buf);
  return 0;
}

void
fsb_buffer_initialize (fsb_nand_t buf, fs_pm_t gc_data, uint32 cluster_size)
{
  if ((FS_BUFFER_FREEMAP_SIZE % cluster_size) != 0)
  {
    FS_ERR_FATAL ("[%d %d]FS_BUFFER_FREEMAP_SIZE not multiple of cluster size",
                  FS_BUFFER_FREEMAP_SIZE, cluster_size, 0);
  }

  if (cluster_size < FS_MIN_PAGE_SIZE)
  {
    FS_ERR_FATAL ("%d<%d cluster size less than min supported size",
                  cluster_size, FS_MIN_PAGE_SIZE, 0);
  }

  if ((cluster_size & (cluster_size - 1)) != 0)
  {
    FS_ERR_FATAL ("%d cluster size not a power of 2", cluster_size, 0, 0);
  }

  /* Make sure that there are as IOVEC items as buffers. */
  if (FS_MAX_IOVEC_ITEMS_COUNT < FS_BUFFER_NUMBER_BUFFERS)
  {
    FS_ERR_FATAL ("FS_MAX_IOVEC_ITEMS_COUNT is not big enough",0,0,0);
  }

  /* Make sure that these 2 solid-data markers match exact in value. */
  ASSERT (FS_IOVEC_DATA_TYPE_PERMENANT_DATA == FS_DATA_TAG_DATA);

  buf->imp.cluster_size = b_cluster_size;
  buf->imp.start_transaction = b_start_transaction;
  buf->imp.end_transaction = b_end_transaction;
  buf->imp.rollback_transaction = b_rollback_transaction;
  buf->imp.is_too_large = b_is_too_large;
  buf->imp.partial_transaction_flush = b_partial_transaction_flush;
  buf->imp.allocate = b_allocate;
  buf->imp.read_wire = b_read_wire;
  buf->imp.write_wire = b_write_wire;
  buf->imp.free_wire = b_free_wire;
  buf->imp.deallocate = b_deallocate;
  buf->imp.deallocate_tree = b_deallocate_tree;
  buf->imp.deallocate_recovered = b_deallocate_recovered;
  buf->imp.get_field = b_get_field;
  buf->imp.set_field = b_set_field;
  buf->imp.get_write_count = b_get_write_count;
  buf->imp.get_space_limit = b_get_space_limit;
  buf->imp.get_num_alloc = b_get_num_alloc;
  buf->imp.set_reservation = b_set_reservation;
  buf->imp.set_quota = b_set_quota;
  buf->imp.get_group_info = b_get_group_info;
  buf->imp.chown = b_chown;
  buf->imp.shred = b_shred;
  buf->imp.scrub = b_scrub;
  buf->imp.sync_start = b_sync_start;
  buf->imp.sync_wait  = b_sync_wait;
  buf->imp.sync_end   = b_sync_end;
  buf->imp.sync_no_wait = b_sync_no_wait;
  buf->imp.sync_get_status = b_sync_get_status;
  buf->imp.make_golden_copy = b_make_golden_copy;
  buf->imp.fs_init_complete = b_fs_init_complete;
  buf->imp.lock_efs = b_lock_efs;
  buf->imp.borrow_ram = b_borrow_ram;
  buf->imp.store_data_in_sb = b_store_data_in_sb;

  fsb_init_helper (buf, gc_data, cluster_size);

  freemap_init (buf);

  buf->gc->ops.register_free_check_cb (buf->gc, buf_is_used, (void *) buf);
  buf->gc->ops.register_alloc_count_cb (buf->gc, buf_alloc_count,
      (void *) buf);
}

static void
read_all_fields (fsb_nand_t buf)
{
  unsigned i;

  for (i = 0; i < FS_UPPER_DATA_COUNT; i++) {
    buf->fields[i] = buf->gc->ops.get_info (buf->gc, i);
  }
  buf->field_dirty_mask = 0;

  /* Special case to initialize the one we use.  This doesn't need to be
   * marked as dirty, since we will just do this check again next time. */
  if (buf->fields[FS_FIELD_NUM_ALLOC] == FS_INVALID_CLUSTER_ID)
    buf->fields[FS_FIELD_NUM_ALLOC] = 0;
}

void
fsb_nand_shutdown (fsb_nand_t buf)
{
  (void) buf;
  FS_ERR_FATAL ("Not yet implemented", 0, 0, 0);
}

static unsigned
b_cluster_size (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  return buf->cluster_size;
}

static void
b_start_transaction (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  buf->transaction_nest++;
  if (buf->transaction_nest == 1) {
    buf->write_count = 0;
  }
}

int fs_buffer_has_work (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  /* If there is a freechain at all, then return one unit of work. */
  return buf->imp.get_field (&buf->imp, FS_FIELD_FREE_CHAIN)
    != FS_INVALID_CLUSTER_ID;
}


static void
buff_write_single (fsb_nand_t buf, fs_iovec_t iovec, uint32 iovec_cnt)
{
  uint32 i;

  ASSERT (buf->gc->ops.page_write != NULL);

  for (i=0; i<iovec_cnt; ++i)
  {
    buf->gc->ops.page_write (buf->gc, iovec[i].cluster_id,
                             iovec[i].buff_base, iovec[i].data_type);
  }
}

static void
buff_write_multiple (fsb_nand_t buf, fs_iovec_t iovec, uint32 iovec_cnt)
{
  ASSERT (buf->gc->ops.write_iovec != NULL);
  buf->gc->ops.write_iovec (buf->gc, iovec, iovec_cnt);
}

static void
buff_write_iovec (fsb_nand_t buf, fs_iovec_t iovec, uint32 iovec_cnt)
{
  if (buf->gc->ops.write_iovec != NULL)
  {
    buff_write_multiple (buf, iovec, iovec_cnt);
  }
  else
  {
    buff_write_single (buf, iovec, iovec_cnt);
  }
}

/* Perform the work of finishing a transaction.  If limit is non-zero, then
 * only this many pages of the freemap will be flushed. */
static void
end_transaction_work (fsb_nand_t buf, unsigned limit)
{
  unsigned counter = 0;
  unsigned i;
  unsigned index;
  uint32 iovec_cnt;
  uint32 freemap_write_cnt;

  /* Flush out all of the wired pages.  This should unwire everything. */
  wired_flush (buf, &buf->chain);
  for (i = 0; i < FSB_MAX_DEPTH; i++) {
    wired_flush (buf, &buf->tree[i]);
  }

  /* Flush out any dirty buffers.  Also check that everything that was
   * wired has been freed. */
  for (i = 0, iovec_cnt = 0; i < FS_BUFFER_NUMBER_BUFFERS; i++)
  {
    if (buf->info[i].state == FS_BSTATE_WRITE_DIRTY)
    {
      ASSERT (iovec_cnt < FS_MAX_IOVEC_ITEMS_COUNT);
      iovec_buffer[iovec_cnt].buff_base = buf->buffers[i];
      iovec_buffer[iovec_cnt].cluster_id = buf->info[i].cluster;
      iovec_buffer[iovec_cnt].data_type = buf->info[i].tag;
      ++iovec_cnt;
      buf->info[i].state = FS_BSTATE_FREE;
      counter++;
      buf->write_count--;
    }
    else if (buf->info[i].state != FS_BSTATE_FREE)
    {
      FS_ERR_FATAL ("End transaction with pending wired clusters", 0, 0, 0);
    }
  }

  /* Write out the fields. */
  while (buf->field_dirty_mask != 0) {
    index = lowest_set_bit (buf->field_dirty_mask);
    buf->gc->ops.store_info (buf->gc, index, buf->fields[index]);
    buf->field_dirty_mask &= ~(1u << index);
  }

  /* The unlimited case always writes out the entire freemap. */
  if (limit == 0)
  {
    freemap_write_cnt = freemap_flush (buf, 0, iovec_buffer, iovec_cnt);
    buf->write_count -= freemap_write_cnt;
    iovec_cnt += freemap_write_cnt;
  }
  else
  {
    /* If there is any work left, write out some of the freemap. */
    if (limit > counter)
    {
      freemap_write_cnt = freemap_flush (buf, (limit - counter),
                                         iovec_buffer, iovec_cnt);
      buf->write_count -= freemap_write_cnt;
      iovec_cnt += freemap_write_cnt;
    }
  }

  buff_write_iovec (buf, iovec_buffer, iovec_cnt);
}

static void
b_end_transaction (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  int free_limit;

#ifdef FS_UNIT_TEST
  if (fsb_transaction_callback != NULL)
    fsb_transaction_callback ();
#endif

  /* If this ends a transaction, writeout all of the data.  Since some of
   * the operations called here check for transaction consistency, do not
   * decrement the transaction_nest value until after we have fully cleaned
   * up. */
  if (buf->transaction_nest == 1) {

    buf->gc->ops.start_transaction (buf->gc);

    /* If there is a free chain, move some of the nodes from there to the
     * freemap. */
    free_limit = 0;
    while (buf->write_count < FS_MAX_TRANSACTION_DATA_PAGES &&
        free_limit < 10)
    {
      fs_cluster_id clust = alloc_from_free_chain (buf);
      if (clust == FS_INVALID_CLUSTER_ID)
        break;
      freemap_dealloc (buf, clust);
      free_limit++;
    }

    /* Do the main work to end a transaction, without any kind of limit. */
    end_transaction_work (buf, 0);

    buf->gc->ops.end_transaction (buf->gc);
  }
  buf->transaction_nest--;
}

static void
b_rollback_transaction (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  int i;

  buf->transaction_nest = 0;

  /* Obliterate all wired pages. */
  for (i = 0; i < FS_BUFFER_NUMBER_BUFFERS; i++) {
    buf->info[i].state = FS_BSTATE_FREE;
    buf->info[i].cluster = FS_INVALID_CLUSTER_ID;
  }

  read_all_fields (buf);

  wired_init (&buf->chain);
  for (i = 0; i < FSB_MAX_DEPTH; i++)
    wired_init (&buf->tree[i]);

  /* Re-read any dirty freemap pages. */
  freemap_revert (buf);
}

static int
b_is_too_large (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  /* It is an error to call this with nested transactions. */
  ASSERT (buf->transaction_nest == 1);

  return (buf->write_count > FS_MAX_TRANSACTION_DATA_PAGES);
}

static void
b_partial_transaction_flush (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  ASSERT (buf->transaction_nest == 1);

#ifdef FS_UNIT_TEST
  if (fsb_transaction_callback != NULL)
    fsb_transaction_callback ();
#endif

  buf->gc->ops.start_transaction (buf->gc);
  end_transaction_work (buf, 22);
  buf->gc->ops.end_transaction (buf->gc);
}



static inline void
update_buffer_access_time (fsb_nand_t buf, int index)
{
  buf->info[index].access_time = buf->current_time;
  buf->current_time++;
}

static inline uint32
get_buffer_access_age (fsb_nand_t buf, int index)
{
  uint32 age = (buf->current_time) - (buf->info[index].access_time);
  return age;
}

/* Find an appropriate buffer for the named cluster.  Attempts to read in
 * the contents of the data, if that is possible.  Returns the buffer index
 * in the array.  The buffers are treated somewhat as a TLA cache.
 * Information about the id is used to create a first index, and we start
 * searching from there.  The search must be full, since read buffers can
 * get reused later. */
static int
find_buffer (fsb_nand_t buf, fs_cluster_id id, int is_alloc,
             fs_read_hint read_hint)
{
  int i;
  int best_guess_index = -1;        /* initialize to invalid index */
  uint32 best_guess_access_age = 0; /* initialize to Lowest possible */
  uint32 index_age = 0;
  int index;

  ASSERT (buf != NULL);
  index = id & buf->fs_buffer_number_mask;
  if (index >= FS_BUFFER_NUMBER_BUFFERS)
    index -= FS_BUFFER_NUMBER_BUFFERS;

  /* Search for the given buffer. */
  for (i = 0; i < FS_BUFFER_NUMBER_BUFFERS; i++)
  {
    if (buf->info[index].cluster == id)
      break;
    index++;
    if (index == FS_BUFFER_NUMBER_BUFFERS)
      index = 0;
  }

  if (i == FS_BUFFER_NUMBER_BUFFERS)
  {
    /* It wasn't found, find a free space starting at the current index.
     * The index at this point has wrapped around once, and is at the same
     * point it was at the start. */
    for (i = 0; i < FS_BUFFER_NUMBER_BUFFERS; i++)
    {
      if (buf->info[index].state == FS_BSTATE_FREE)
      {
        index_age = get_buffer_access_age (buf, index);
        if(best_guess_access_age < index_age)
        {
          best_guess_index = index;
          best_guess_access_age = index_age;
        }
      }
      if((i >= (FS_BUFFER_LRU_WINDOW - 1)) && (best_guess_index != -1))
      {
        /* we found the most suitable replacement index, so use that */
        index = best_guess_index;
        break;
      }
      index++;
      if (index == FS_BUFFER_NUMBER_BUFFERS)
        index = 0;
    }

   /* if we found no free buffers, the best_guess_index will not get updated,
       so we have run out of space */
    if (best_guess_index == -1)
    {
      FS_ERR_FATAL ("Out of buffers in filesystem", 0, 0, 0);
    }
  }

  /* If this is buffer hasn't been visited yet, read in the data. */
  if (is_alloc && buf->info[index].cluster != id)
  {
    buf->info[index].cluster = id;

    /* This hint is passed from the upper layers. If the cluster is
     * just allocated, there is no use reading the corresponding page into
     * the buffer */
    if (read_hint == FS_BUF_READ_CLUSTER)
      buf->gc->ops.page_read (buf->gc, id, buf->buffers[index]);
    else
      memset (buf->buffers[index], 0xFF, buf->cluster_size);
  }

  /* Must either find or allocate a buffer. */
  ASSERT (buf->info[index].cluster == id);

  /* update the access time of the buffer in use */
  update_buffer_access_time (buf, index);

  return index;
}

static void *
b_read_wire (fsb_t imp, fs_cluster_id id)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  int bnum;

  /* Simple range check, but should catch most invalid things, such as
   * trying to wire the FS_INVALID_CLUSTER_ID. */
  if (id > buf->space_limit)
    FS_ERR_FATAL ("Attempt to read past allocated space", 0, 0, 0);

  bnum = find_buffer (buf, id, 1, FS_BUF_READ_CLUSTER);

  /* If the buffer was write wired, this is a usage error. */
  if (buf->info[bnum].state == FS_BSTATE_WRITE_WIRE) {
    FS_ERR_FATAL ("Attempt to convert write wire to read wire", 0, 0, 0);
  }

  /* If the buffer is dirty, then use the special state
   * FS_BSTATE_READ_WIRE_DIRTY to indicate that it still needs to become
   * written. */
  if (buf->info[bnum].state == FS_BSTATE_WRITE_DIRTY) {
    buf->info[bnum].state = FS_BSTATE_READ_WIRE_DIRTY;
  } else {
    /* Mark the correct state, and return a pointer to the buffer. */
    buf->info[bnum].state = FS_BSTATE_READ_WIRE;
  }

#ifdef FS_UNIT_TEST
  /* Compute a hash of the buffer to see if anything has changed. */
  {
    struct MD5Context ctx;

    MD5Init (&ctx);
    MD5Update (&ctx, buf->buffers[bnum], buf->cluster_size);
    MD5Final (buf->info[bnum].hash, &ctx);
  }
#endif

  return buf->buffers[bnum];
}

static void *
b_write_wire (fsb_t imp, fs_cluster_id id,
              fs_read_hint read_hint, fs_buffer_data_tag_type tag)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  int bnum;

  /* Simple range check, but should catch most invalid things, such as
   * trying to wire the FS_INVALID_CLUSTER_ID. */
  if (id > buf->space_limit)
    FS_ERR_FATAL ("Attempt to write past allocated space", 0, 0, 0);

  bnum = find_buffer (buf, id, 1, read_hint);

  buf->info[bnum].tag = tag;

  /* XXX: Check for invalid transitions here, such as redundant write
   * wires. */

  /* Mark the correct state, and return a pointer to the buffer. */
  buf->info[bnum].state = FS_BSTATE_WRITE_WIRE;

  return buf->buffers[bnum];
}

static void
b_free_wire (fsb_t imp, fs_cluster_id id)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  int bnum;
  enum fs_buffer_state_t state;

  bnum = find_buffer (buf, id, 0, FS_BUF_READ_CLUSTER);

  if (bnum < 0) {
    FS_ERR_FATAL ("Freeing unwired cluster", 0, 0, 0);
  }

  state = buf->info[bnum].state;

  /* If it is a write wire, mark the data as dirty. */
  if (state == FS_BSTATE_WRITE_WIRE ||
      state == FS_BSTATE_READ_WIRE_DIRTY)
  {
    if(buf->transaction_nest < 1)
      FS_ERR_FATAL ("free_wire write outside a transaction.", 0, 0, 0);

    if (state != FS_BSTATE_READ_WIRE_DIRTY)
      buf->write_count++;
    buf->info[bnum].state = FS_BSTATE_WRITE_DIRTY;
  } else if (state != FS_BSTATE_WRITE_DIRTY) {
    buf->info[bnum].state = FS_BSTATE_FREE;
#ifdef FS_UNIT_TEST
    /* Since this buffer wasn't marked as dirty, make sure that it didn't
     * get changed. */
    {
      struct MD5Context ctx;
      uint8 hash2[16];

      MD5Init (&ctx);
      MD5Update (&ctx, buf->buffers[bnum], buf->cluster_size);
      MD5Final (hash2, &ctx);
      if (memcmp (buf->info[bnum].hash, hash2, 16) != 0)
        FS_ERR_FATAL ("Buffer changed, but not marked dirty", 0, 0, 0);
    }
#endif
  }
}


/**********************************************************************
 * The fields are handled by the GC layer. */

static uint32
b_get_field (fsb_t imp, int field_id)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  return (buf->fields[field_id]);
}

static void
b_set_field (fsb_t imp, int field_id, uint32 value)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  buf->fields[field_id] = value;
  buf->field_dirty_mask |= 1 << field_id;
}

static unsigned
b_get_write_count (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  if (buf->transaction_nest == 0)
    return 0;
  else
    return buf->write_count;
}

static fs_cluster_id
b_get_space_limit (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  return buf->space_limit;
}

static fs_cluster_id
b_get_num_alloc (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  return buf->fields[FS_FIELD_NUM_ALLOC];
}


/**********************************************************************
 * Free bitmap management.
 *
 * Clusters that have been returned to the garbage collector are maintained
 * in the freemap.  This freemap is a bitmap of all clusters.  A set bit
 * indicates this cluster is "free" according to the garbage collector. */

/* Return the index of the lowest number bit that is set. */
static unsigned
lowest_set_bit (uint32 value)
{
  unsigned bit = 0;

  if ((value & 0xFFFF) == 0) {
    bit += 16;
    value >>= 16;
  }
  if ((value & 0xFF) == 0) {
    bit += 8;
    value >>= 8;
  }
  if ((value & 0xF) == 0) {
    bit += 4;
    value >>= 4;
  }
  if ((value & 0x3) == 0) {
    bit += 2;
    value >>= 2;
  }
  if ((value & 1) == 0) {
    bit += 1;
  }
  return bit;
}

#define FREEMAP_FREE 1
#define FREEMAP_ALLOCATED 0
/* Mark a single cluster in the freemap to a certain state.  'state' should
 * be FREEMAP_FREE for 'free', and FREEMAP_ALLOCATED for allocated.  Also
 * marks the appropriate dirty bit.
 *
 * Note that since all of the divisors are powers of two, the compiler
 * should optimize this into shifts and masks. */
static /* inline */ void
freemap_mark (fsb_nand_t buf, fs_cluster_id cluster, unsigned state)
{
  unsigned bitmap_offset = cluster / 32;
  unsigned dirty_offset = (cluster / (buf->cluster_size * 8)) / 32;
  uint32 bitmap_bit = 1U << (cluster % 32);
  uint32 dirty_bit = 1U << ((cluster / (buf->cluster_size * 8)) % 32);

  ASSERT (bitmap_offset < FS_BUFFER_FREE_BITMAP_UINT32_COUNT);
  ASSERT (dirty_offset < FS_BUFFER_FREE_DIRTYMAP_UINT32_COUNT);

  if (state == FREEMAP_FREE) {
    buf->freemap.bitmap[bitmap_offset] |= bitmap_bit;
  } else {
    buf->freemap.bitmap[bitmap_offset] &= ~bitmap_bit;
  }
  if ((buf->freemap.dirty[dirty_offset] & dirty_bit) == 0) {
    buf->freemap.dirty[dirty_offset] |= dirty_bit;
    buf->write_count++;
  }
}

/* Flush all clusters in the freemap that have been marked as dirty.  If
 * 'limit' is non-zero, only write at most this many pages.  Returns the
 * number of pages written. */
static unsigned
freemap_flush (fsb_nand_t buf, unsigned limit,
               fs_iovec_t iovec, uint32 iovec_cnt)
{
  unsigned offset, index;
  unsigned cluster_offset;
  unsigned counter = 0;
  uint8 *freemap_buff;

  offset = 0;
  while (offset < FS_BUFFER_FREE_DIRTYMAP_UINT32_COUNT) {
    if (buf->freemap.dirty[offset] == 0)
      offset++;
    else {
      index = lowest_set_bit (buf->freemap.dirty[offset]);
      cluster_offset = offset * 32 + index;

      /* Add this to the iovec array. */
      ASSERT (iovec_cnt < FS_MAX_IOVEC_ITEMS_COUNT);
      iovec[iovec_cnt].cluster_id = buf->freemap.base + cluster_offset;
      iovec[iovec_cnt].data_type = FS_DATA_TAG_FREEMAP;
      freemap_buff = ((uint8 *)buf->freemap.bitmap) +
                                  (cluster_offset * buf->cluster_size);
      iovec[iovec_cnt].buff_base = freemap_buff;
      ++iovec_cnt;

      buf->freemap.dirty[offset] &= ~(1U << index);

      /* Check to see if we've done too much work. */
      counter++;
      if (limit > 0 && counter >= limit) {
        break;
      }
    }
  }

  return counter;
}

/* Re-read any freemap clusters that have been marked as dirty.  Used to
 * revert changes. */
static void
freemap_revert (fsb_nand_t buf)
{
  unsigned offset, index;
  unsigned cluster_offset;

  offset = 0;
  while (offset < FS_BUFFER_FREE_DIRTYMAP_UINT32_COUNT) {
    if (buf->freemap.dirty[offset] == 0)
      offset++;
    else {
      index = lowest_set_bit (buf->freemap.dirty[offset]);
      cluster_offset = offset * 32 + index;
      buf->gc->ops.page_read (buf->gc, buf->freemap.base + cluster_offset,
        (uint8 *) buf->freemap.bitmap + (cluster_offset * buf->cluster_size));
      buf->freemap.dirty[offset] &= ~(1U << index);
    }
  }
}

static void
freemap_clear_init (fsb_nand_t buf)
{
  /* Start by clearing the freemap, nothing is free. */
  memset (buf->freemap.bitmap, 0,
      FS_BUFFER_FREE_BITMAP_UINT32_COUNT * sizeof (uint32));

  /* Also clear out the dirty map. */
  memset (buf->freemap.dirty, 0,
      FS_BUFFER_FREE_DIRTYMAP_UINT32_COUNT * sizeof (uint32));
}

static void
freemap_read_init (fsb_nand_t buf, fs_cluster_id base, unsigned cluster_count)
{
  fs_cluster_id cluster;

  buf->freemap.base = base;

  /* Read in the existing freemap. */
  for (cluster = 0; cluster < cluster_count; cluster++) {
    buf->gc->ops.page_read (buf->gc, cluster + base,
                            &buf->freemap.bitmap[cluster *
                              (buf->cluster_size / sizeof (uint32))]);
  }
}

static int
freemap_init_read_only (fsb_nand_t buf)
{
  fs_cluster_id base;
  unsigned cluster_count;

  base = buf->imp.get_field (&buf->imp, FS_FIELD_FREEMAP_BASE);
  cluster_count = (buf->space_limit + buf->cluster_size * 8 - 1) /
    (buf->cluster_size * 8);

  /* Make sure the configured size is big enough.  It is important that this
   * is not an assertion, since those can be turned off. */
  if (cluster_count > FS_BUFFER_FREEMAP_SIZE / buf->cluster_size)
  {
    return -1;
  }

  if (base == FS_INVALID_CLUSTER_ID)
  {
    return -1;
  }

  freemap_clear_init (buf);
  freemap_read_init (buf, base, cluster_count);
  buf->freemap.last_index = 0;
  return 0;
}

static void
freemap_init (fsb_nand_t buf)
{
  fs_cluster_id base;
  unsigned cluster_count;

  base = buf->imp.get_field (&buf->imp, FS_FIELD_FREEMAP_BASE);
  cluster_count = (buf->space_limit + buf->cluster_size * 8 - 1) /
    (buf->cluster_size * 8);

  /* Make sure the configured size is big enough.  It is important that this
   * is not an assertion, since those can be turned off. */
  if (cluster_count > FS_BUFFER_FREEMAP_SIZE / buf->cluster_size) {
    FS_ERR_FATAL ("FS_BUFFER_FREEMAP_SIZE not configured large enough",
        0, 0, 0);
  }

  freemap_clear_init (buf);

  if (base == FS_INVALID_CLUSTER_ID) {
    unsigned count, offset, bit;
    fs_cluster_id cluster;
    base = 1;
    buf->freemap.base = base;

    /* Mark all clusters between one past the end of the freemap and the
     * end of the usable space as available. */
    for (cluster = base + cluster_count;
        cluster < buf->space_limit;
        cluster++)
    {
      freemap_mark (buf, cluster, FREEMAP_FREE);
    }

    /* Flush out the freemap. */
    buf->gc->ops.start_transaction (buf->gc);
    count = 0;
    for (offset = 0; offset < FS_BUFFER_FREE_DIRTYMAP_UINT32_COUNT;
        offset++)
    {
      for (bit = 0; bit < 32; bit++) {
        if ((buf->freemap.dirty[offset] & (1 << bit)) != 0) {
          /* Flush transaction if it grows too large. */
          if (count > FS_MAX_TRANSACTION_DATA_PAGES) {
            buf->gc->ops.end_transaction (buf->gc);
            buf->gc->ops.start_transaction (buf->gc);
            count = 0;
          }

          cluster = (offset * 32) + bit;
          buf->gc->ops.page_write (buf->gc, cluster + base,
              &buf->freemap.bitmap[cluster *
                  (buf->cluster_size / sizeof (uint32))], FS_DATA_TAG_FREEMAP);
          count++;
          buf->freemap.dirty[offset] &= ~(1U << bit);
        }
      }
    }

    buf->fields[FS_FIELD_FREEMAP_BASE] = base;
    buf->gc->ops.store_info (buf->gc, FS_FIELD_FREEMAP_BASE, base);

    buf->fields[FS_FIELD_NUM_ALLOC] = cluster_count + 1;
    buf->gc->ops.store_info (buf->gc, FS_FIELD_NUM_ALLOC,
        cluster_count + 1);

    buf->fields[FS_FIELD_GENERAL_POOL] = cluster_count + 1;
    buf->gc->ops.store_info (buf->gc, FS_FIELD_GENERAL_POOL,
        cluster_count + 1);

    buf->gc->ops.end_transaction (buf->gc);
  } else {
    freemap_read_init (buf, base, cluster_count);
  }

  buf->freemap.last_index = 0;
}

static void
wired_init (fsb_wired_t wired)
{
  wired->cluster = FS_INVALID_CLUSTER_ID;
  wired->data = NULL;
  wired->write_wired = 0;
}

static void
wired_flush (fsb_nand_t buf, fsb_wired_t wired)
{
  if (wired->cluster != FS_INVALID_CLUSTER_ID) {
    buf->imp.free_wire (&buf->imp, wired->cluster);
    wired->cluster = FS_INVALID_CLUSTER_ID;
    wired->write_wired = 0;
    wired->data = NULL;
  }
}

/* Bring the appropriate wired page in. */
static void
wired_ready_cluster (fsb_nand_t buf, fsb_wired_t wired,
    fs_cluster_id cluster, int for_write)
{
  if (wired->cluster == cluster) {
    if (!wired->write_wired && for_write) {
      wired->data = buf->imp.write_wire (&buf->imp, cluster,
                                         FS_BUF_READ_CLUSTER,
                                         FS_DATA_TAG_FREEMAP);
      wired->write_wired = 1;
    }
  } else {
    if (wired->cluster != FS_INVALID_CLUSTER_ID) {
      buf->imp.free_wire (&buf->imp, wired->cluster);
    }

    if (for_write) {
      wired->data = buf->imp.write_wire (&buf->imp, cluster,
                                         FS_BUF_READ_CLUSTER,
                                         FS_DATA_TAG_FREEMAP);
      wired->write_wired = 1;
    } else {
      wired->data = buf->imp.read_wire (&buf->imp, cluster);
      wired->write_wired = 0;
    }
  }

  wired->cluster = cluster;
}

/* Allocate a single cluster from the freemap. */
static fs_cluster_id
freemap_alloc (fsb_nand_t buf)
{
  unsigned limit = (buf->space_limit + 31) / 32;
  unsigned index = buf->freemap.last_index;
  unsigned count = 0;
  unsigned bit;
  fs_cluster_id cluster;

  /* Scan the free bitmap, looking for a bit that is set. */
  while (buf->freemap.bitmap[index] == 0) {
    index++;
    if (index > limit) {
      index = 0;
      count++;
      if (count > 2) {
        /* We've scanned the bitmap twice and found nothing set in it.
         * Space is exhausted. */
        return FS_INVALID_CLUSTER_ID;
      }
    }
  }

  buf->freemap.last_index = index;

  bit = lowest_set_bit (buf->freemap.bitmap[index]);
  cluster = index * 32 + bit;

  freemap_mark (buf, cluster, FREEMAP_ALLOCATED);

  ASSERT (cluster <= buf->space_limit);
  return cluster;
}

static void
freemap_dealloc (fsb_nand_t buf, fs_cluster_id id)
{
  /* Make sure nothing tries to deallocate something that is already free.
   */
  ASSERT (fs_buf_is_allocated (buf, id));

  freemap_mark (buf, id, FREEMAP_FREE);
}

int
fs_buf_is_allocated (fsb_nand_t buf, fs_cluster_id id)
{
  ASSERT (id <= buf->space_limit);
  return  (buf->freemap.bitmap[id / 32] & (1 << (id % 32))) == 0;
}

static int
buf_is_used (void *data, fs_cluster_id id)
{
  return fs_buf_is_allocated ((fsb_nand_t) data, id);
}

/* Count up the current allocation.  This is the amount of space allocated
 * in the general pool, plus the specific allocations for each group. */
static uint32
buf_alloc_count (void *data)
{
  fsb_nand_t buf = (fsb_nand_t) data;
  uint32 bill_count = 0;
  unsigned i;

  for (i = 1; i <= FS_QR_GROUP_COUNT; i++) {
    bill_count += buf->imp.get_field (&buf->imp, FS_GROUP_SPACE_USED (i));
  }

  bill_count += buf->imp.get_field (&buf->imp, FS_FIELD_GENERAL_POOL);

  return bill_count;
}

/**********************************************************************
 * Free cluster management.
 *
 * Although the lower layer does maintain information about which clusters
 * have been allocated, this information is not readily available.
 * Instead, we keep track of our own free cluster information.  We have two
 * pieces of information in the FS superblock (stored in cluster 1).  One
 * is the next allocation location.  This is the next cluster to allocate
 * when the free list is completely empty.
 *
 * The second is the head of the free cluster chain.  Each chain is a
 * cluster (itself free) containing a list of other cluster chains.  Its
 * contents are defined by the cluster_chain structure.
 *
 * The head page of the free chain is kept cached in our data structure
 * until the end of a transaction.
 */

/* Remove the lowest number entry from the given heap data.  Does not check
 * if the heap is empty, so beware. */
static uint32 heap_take (struct cluster_chain *heap);

/* Peak at the lowest number entry in the given heap.  Also doesn't check
 * if the heap is empty. */
static uint32 heap_peek (struct cluster_chain *heap);

/* Insert an entry into the heap.  Does not check for overflow, so beware.
 */
static void heap_insert (struct cluster_chain *heap, uint32 item);

static fs_cluster_id tree_node_alloc (fsb_nand_t buf, fs_cluster_id node,
    int level, int depth);

/* Read in the head of the free chain if necessary. */
static void
ready_free_chain (fsb_nand_t buf, int for_write)
{
  fs_cluster_id chain;

  chain = buf->imp.get_field (&buf->imp, FS_FIELD_FREE_CHAIN);

  if (chain == FS_INVALID_CLUSTER_ID) {
    buf->chain.cluster = FS_INVALID_CLUSTER_ID;
  } else {
    wired_ready_cluster (buf, &buf->chain, chain, for_write);
  }
}

static void
add_to_free_chain (fsb_nand_t buf, uint32 entry)
{
  fs_cluster_id old_node, new_node;
  struct cluster_chain *cchain;
  int do_heap_insert = 0;

  ready_free_chain (buf, 0);
  cchain = (struct cluster_chain *) buf->chain.data;

  /* If this chain node is full (or non-existant), find a block to use for
   * the chain block. */
  if (buf->chain.cluster == FS_INVALID_CLUSTER_ID
      || cchain->data[0] >= 126)
  {
    old_node = buf->chain.cluster;

    if (FSB_TREE_DEPTH (entry) == 0) {
      /* This is a leaf node, so it can just become the new free chain
       * element. */
      new_node = entry;
    } else {
      new_node = tree_node_alloc (buf,
          FSB_TREE_CLUSTER (entry), 0,
          FSB_TREE_DEPTH (entry));
      if (new_node != FSB_TREE_CLUSTER (entry)) {
        /* This node must be added to the new heap. */
        do_heap_insert = 1;
      }
    }

    buf->imp.set_field (&buf->imp, FS_FIELD_FREE_CHAIN, new_node);

    ready_free_chain (buf, 1);
    cchain = (struct cluster_chain *) buf->chain.data;

    cchain->next = old_node;
    cchain->data[0] = 0;

    if (do_heap_insert)
      heap_insert (cchain, entry);
  } else {
    ready_free_chain (buf, 1);
    cchain = (struct cluster_chain *) buf->chain.data;

    heap_insert (cchain, entry);

    /* XXX: Wrong, but code using this expects it right now. */
    /* buf->write_count++; */
  }

}

/* Allocate a single node from the given tree, at the specified level.  If
 * the return result is the same as 'node', then this node should now be
 * marked as in use. */
static fs_cluster_id
tree_node_alloc (fsb_nand_t buf, fs_cluster_id node,
    int level, int depth)
{
  uint32 tmp;
  fs_cluster_id child;
  uint32 *nodes;

  if (level >= depth)
    return node;

  /* Are we viewing the correct node? */
  if (buf->tree[level].cluster != node) {
    buf->tree_offset[level] = 0;
  }
  wired_ready_cluster (buf, &buf->tree[level], node, 0);
  nodes = (uint32 *) buf->tree[level].data;

  /* Find the next entry in this node that has a value. */
  tmp = buf->tree_offset[level];
  while (tmp < buf->cluster_size / 4
         && nodes[tmp] == FS_INVALID_CLUSTER_ID)
    tmp++;
  buf->tree_offset[level] = tmp;

  /* If there are no pointers down, just return this node. */
  if (tmp >= buf->cluster_size / 4) {
    return node;
  }

  /* Otherwise, get the next node down. */
  child = tree_node_alloc (buf, nodes[tmp],
      level + 1, depth);

  /* If the child is empty, mark it as free at this level. */
  if (child == nodes[tmp]) {
    wired_ready_cluster (buf, &buf->tree[level], node, 1);
    nodes = (uint32 *) buf->tree[level].data;
    nodes[tmp] = FS_INVALID_CLUSTER_ID;
    buf->tree_offset[level]++;
  }

  return child;
}

/* Attempt to take a cluster from the free chain.  Returns
 * FS_INVALID_CLUSTER_ID if the free_chain is empty. */
static fs_cluster_id
alloc_from_free_chain (fsb_nand_t buf)
{
  fs_cluster_id clust, tmp;
  struct cluster_chain *cchain;

  ready_free_chain (buf, 0);
  cchain = (struct cluster_chain *) buf->chain.data;

  if (buf->chain.cluster == FS_INVALID_CLUSTER_ID)
    return FS_INVALID_CLUSTER_ID;

  if (cchain->data[0] == 0) {
    /* The node at the head of the chain is empty, so use it as the
     * allocated cluster. */
    buf->imp.set_field (&buf->imp, FS_FIELD_FREE_CHAIN, cchain->next);

    clust = buf->chain.cluster;

    /* We don't really need to flush this out, since it doesn't have
     * meaningful data in it.  However, since we're going to return it
     * anyway, and the user will soon write to it, it doesn't hurt anything
     * to leave the cluster marked as dirty. */
    wired_flush (buf, &buf->chain);
  } else {
    /* There is a free chain node, and it points to something. */
    ready_free_chain (buf, 1);
    cchain = (struct cluster_chain *) buf->chain.data;

    clust = heap_peek (cchain);
    if (FSB_TREE_DEPTH (clust) > 0) {
      tmp = tree_node_alloc (buf,
          FSB_TREE_CLUSTER (clust), 0,
          FSB_TREE_DEPTH (clust));
      if (tmp == FSB_TREE_CLUSTER (clust)) {
        /* This cluster is now going to be used for user data, so release
         * our wire upon it. */
        ASSERT (buf->tree[0].cluster == tmp);
        wired_flush (buf, &buf->tree[0]);

        (void) heap_take (cchain);
      }
      clust = tmp;
    } else {
      (void) heap_take (cchain);
    }
  }

  return clust;
}

static int
b_allocate (fsb_t imp, uint32 gid, fs_cluster_id *id)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  fs_cluster_id clust;
  int result;

  ASSERT (buf->transaction_nest > 0);

  /* Verify the quota */
  if(quota_exceeded (&buf->imp, gid, 1))
    return -EDQUOT;

  /* Charge the group for now. If there is no space, later it can be
   * refunded */
  result = charge_group (&buf->imp, gid, 1);
  if (result < 0)
    return -ENOSPC;

  /* The simple case, there is nothing on the free chain, so advance the
   * allocation pointer. */
  clust = alloc_from_free_chain (buf);
  if (clust == FS_INVALID_CLUSTER_ID)
    clust = freemap_alloc (buf);
  if (clust == FS_INVALID_CLUSTER_ID)
  {
    /* No space was left for anything, so we have to refund what was
     * charged earlier. This is only done for the general pool and
     * reservations that have exceeded their reserved space. */
    refund_group (&buf->imp, gid, 1);
    return -ENOSPC;
  }

  *id = clust;
  return 0;
}

static void
b_deallocate (fsb_t imp, fs_cluster_id id, uint32 gid)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  ASSERT (buf->transaction_nest > 0);

  refund_group (&buf->imp, gid, 1);
  freemap_mark (buf, id, FREEMAP_FREE);

}

static void
b_deallocate_recovered (fsb_t imp, fs_cluster_id id)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  ASSERT (buf->transaction_nest > 0);

  freemap_mark (buf, id, FREEMAP_FREE);
}

static void
b_deallocate_tree (fsb_t imp, fs_cluster_id id, unsigned  depth,
                   unsigned count, uint32 gid)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  ASSERT (buf->transaction_nest > 0);
  ASSERT (depth < FSB_MAX_DEPTH);

  refund_group (&buf->imp, gid, count);
  add_to_free_chain (buf, FSB_TREE_MAKE (id, depth));
}

static uint32
heap_peek (struct cluster_chain *heap)
{
  return heap->data[1];
}

static uint32
heap_take (struct cluster_chain *heap)
{
  uint32 item;
  unsigned node;
  unsigned next;
  uint32 tmp;

  /* The smallest item is always in front. */
  item = heap->data[1];

  if (heap->data[0] > 1) {
    heap->data[1] = heap->data[heap->data[0]];
    heap->data[0]--;

    node = 1;

    while (1) {
      next = node << 1;
      if (next > heap->data[0])
        break;

      if (next < heap->data[0]
          && heap->data[next + 1] < heap->data[next])
      {
        next++;
      }

      if (heap->data[node] > heap->data[next]) {
        /* Swap the two entries. */
        tmp = heap->data[node];
        heap->data[node] = heap->data[next];
        heap->data[next] = tmp;
        node = next;
      } else {
        break;
      }
    }
  } else {
    heap->data[0]--;
  }

  return item;
}

static void
heap_insert (struct cluster_chain *heap, uint32 item)
{
  unsigned node;
  unsigned next;
  uint32 tmp;

  /* Append the item to the heap. */
  heap->data[0]++;
  node = heap->data[0];
  heap->data[node] = item;

  /* Bubble the item up in the heap, maintaining the heap property. */
  while (node > 1) {
    next = node >> 1;
    if (heap->data[node] < heap->data[next]) {
      /* Swap the two entries. */
      tmp = heap->data[node];
      heap->data[node] = heap->data[next];
      heap->data[next] = tmp;
    } else {
      /* If no swapping needed, we are done, so leave the loop. */
      break;
    }

    node = next;
  }
}

static int
b_set_reservation (fsb_t imp, uint32 gid, uint32 bytes)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  uint32 new_res_blocks = 0;
  uint32 old_res_blocks = 0;
  uint32 space_used = 0;
  uint32 alloc_returned = 0;


  if (!FS_IS_QUOTA_RESERVED_GROUP (gid))
    return -EPERM;

  /* Get the old reservation size in the case where the reservation is
   * reduced below the current size */
  old_res_blocks = buf->imp.get_field (&buf->imp,
      FS_GROUP_RESERVATION_SIZE(gid));

  /* Recalculate the size from bytes as input to pages for storage. */
  new_res_blocks = (bytes + buf->cluster_size - 1)/ buf->cluster_size;

  /* Check to ensure the new allocation will not exceed the space limit. To
   * allow resizing of the same reservation, first subtract the reservation
   * size from the current blocks allocated. */
  if ((buf->imp.get_field(&buf->imp, FS_FIELD_NUM_ALLOC) -
        old_res_blocks + new_res_blocks) > buf->space_limit)
    return -ENOSPC;

  /* Set the new reservation size */
  buf->imp.set_field (&buf->imp, FS_GROUP_RESERVATION_SIZE (gid),
      new_res_blocks);

  space_used = buf->imp.get_field(&buf->imp, FS_GROUP_SPACE_USED (gid));

  /* Since reservation re-sizing is allowed, there are several cases that
   * need to be checked. The two major cases are when the new reservation
   * is either more or less than the old reservation. In each case we need
   * to see if there are any blocks that need to be charged or refunded to
   * the general pool */
  if (new_res_blocks == old_res_blocks)
  {
    /* Trivial case, nothing changes in the file system */
  }
  else if (new_res_blocks > old_res_blocks)
  {
    /* The reservation being set is greater than the old reservation */
    if (new_res_blocks <= space_used)
    {
      /* No new blocks are being taken from the global pool */
      new_res_blocks = 0;
    }
    else
    {
      /* Calculate the number of blocks needed to be set aside */
      if (old_res_blocks > space_used)
        new_res_blocks = new_res_blocks - old_res_blocks;
      else
        new_res_blocks = new_res_blocks - space_used;
    }
    /* Set aside the new reserved blocks */
    buf->imp.set_field (&buf->imp, FS_FIELD_NUM_ALLOC,
      buf->imp.get_field (&buf->imp, FS_FIELD_NUM_ALLOC) + new_res_blocks);
  }
  else /* new_res_blocks < old_res_blocks */
  {
    /* The reservation being set is smaller than the current usage */
    if (new_res_blocks >= space_used)
    {
      /* Since the new reservation is smaller than the old one, return the
       * difference between the old reservation and the new reservation */
      alloc_returned = old_res_blocks - new_res_blocks;
    }
    else
    {
      if (old_res_blocks <= space_used)
      {
        /* All the blocks for the old reservation are used up, so nothing
         * to return to the global pool. */
        alloc_returned = 0;
      }
      else
      {
        /* There are some blocks that were initially set aside, but are no
         * longer needed for the reservation, so it is time to return them
         * to the global pool */
        alloc_returned = old_res_blocks - space_used;
      }
    }

    /* Return the allocations */
    if (alloc_returned > 0)
      buf->imp.set_field (&buf->imp, FS_FIELD_NUM_ALLOC,
        buf->imp.get_field (&buf->imp, FS_FIELD_NUM_ALLOC) - alloc_returned);
  }

  return 0;
} /* end b_set_reservation */

static int
b_set_quota (fsb_t imp, uint32 gid, uint32 bytes)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  uint32 quota_limit = 0;

  if (!FS_IS_QUOTA_RESERVED_GROUP (gid))
    return -EPERM;

  /* Recalculate the size from bytes as input to pages for storage. */
  quota_limit = (bytes + buf->cluster_size -1) / buf->cluster_size;

  /* Set the Quota limit. It does not matter what the other fields are. */
  buf->imp.set_field (&buf->imp, FS_GROUP_QUOTA_SIZE (gid), quota_limit);

  return 0;
} /* end b_set_quota */

static int
b_get_group_info (fsb_t imp, uint32 gid, struct fs_group_info *ginfo)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  if (FS_IS_QUOTA_RESERVED_GROUP (gid))
  {
    ginfo->quota_size = buf->imp.get_field(&buf->imp,
        FS_GROUP_QUOTA_SIZE(gid)) * buf->cluster_size;
    ginfo->reservation_size = buf->imp.get_field (&buf->imp,
        FS_GROUP_RESERVATION_SIZE(gid)) * buf->cluster_size;
    ginfo->space_used = buf->imp.get_field (&buf->imp,
        FS_GROUP_SPACE_USED(gid)) * buf->cluster_size;
  }
  else
    return -EPERM;
  return 0;
} /* end b_get_group_info */

static int
b_chown (fsb_t imp, int old_uid, int new_uid, int old_gid, int new_gid,
         uint32 blocks)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  int result = 0;

  (void) new_uid;
  (void) old_uid;

  /* Set all non-Q&R gid's to 0 */
  if (!FS_IS_QUOTA_RESERVED_GROUP (new_gid))
    new_gid = 0;

  if (!FS_IS_QUOTA_RESERVED_GROUP (old_gid))
   old_gid = 0;

  /* Check to see if we chown to the same group and shortcut the process */
  if (old_gid == new_gid)
    return 0;

  /* Check to make the new group quota will not be exceeded */
  if (quota_exceeded (&buf->imp, new_gid, blocks))
    return -EDQUOT;

  /* Refund the old group first to guarantee enough space is available to
   * chown the file. If the charge group fails, re-charge the old group */
  refund_group (&buf->imp, old_gid, blocks);
  result = charge_group (&buf->imp, new_gid, blocks);
  if (result < 0)
  {
    (void) charge_group (&buf->imp, old_gid, blocks);
    return -ENOSPC;
  }

  return 0;
} /* end b_chown */

static int
b_shred (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  if (buf->gc->ops.shred != NULL)
    return (buf->gc->ops.shred (buf->gc));

  return -EPERM;

} /* end b_shred */


static int
b_scrub (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  if (buf->gc->ops.scrub != NULL)
    return (buf->gc->ops.scrub (buf->gc));

  return -EPERM;
} /* end b_shred */


static int
b_sync_start (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  return (buf->gc->ops.sync_start (buf->gc));
}

static int
b_sync_wait (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  return (buf->gc->ops.sync_wait (buf->gc));
}

static int
b_sync_end (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  return (buf->gc->ops.sync_end (buf->gc));
}

static int
b_sync_no_wait (fsb_t imp, fs_sync_token_t *sync_token)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  return (buf->gc->ops.sync_no_wait (buf->gc, sync_token));
}


static int
b_sync_get_status (fsb_t imp, fs_sync_token_t sync_token,
                   enum fs_sync_result *sync_result)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  return (buf->gc->ops.sync_get_status (buf->gc, sync_token,
                                        sync_result));
}

static int
b_make_golden_copy (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  return (buf->gc->ops.make_golden_copy (buf->gc));
}

static int
b_fs_init_complete (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  return (buf->gc->ops.fs_init_complete (buf->gc));
}

static int
b_lock_efs (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  return (buf->gc->ops.lock_efs (buf->gc));
}

static int
b_borrow_ram (fsb_t imp, void **ram_addr, uint32 *ram_size)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  return (buf->gc->ops.borrow_ram (buf->gc, ram_addr, ram_size));
}

static int
b_store_data_in_sb (fsb_t imp, const void *data, uint32 data_size)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  return (buf->gc->ops.store_data_in_sb (buf->gc, data, data_size));
}


static int
charge_group (fsb_t imp, uint32 gid, uint32 blocks)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  uint32 general_blocks;
  uint32 billed_blocks;
  uint32 reservation_blocks, reservation_used;
  unsigned bill_to_field;

  /* Determine who is going to be charged for the allocation and if
   * allocations in the general pool are required. */
  if (!FS_IS_QUOTA_RESERVED_GROUP (gid))
  {
    general_blocks = blocks;
    billed_blocks = 0;
    bill_to_field = FS_FIELD_GENERAL_POOL;
  }
  else
  {
    billed_blocks = blocks;

    reservation_blocks = buf->imp.get_field(&buf->imp,
        FS_GROUP_RESERVATION_SIZE (gid));

    reservation_used = buf->imp.get_field (&buf->imp,
        FS_GROUP_SPACE_USED (gid));

    bill_to_field = FS_GROUP_SPACE_USED (gid);

    /* Find out how many blocks come from the general pool. */
    if (reservation_used + blocks <= reservation_blocks)
    {
      /* None of the blocks come from the general pool */
      general_blocks = 0;
    }
    else if (reservation_used < reservation_blocks)
    {
      /* Some of the blocks go to the general pool */
      general_blocks = blocks - (reservation_blocks - reservation_used);
    }
    else
    {
      /* All the blocks will be come from the general pool */
      general_blocks = blocks;
    }
  }

  /* Check to make sure we won't run out of free space */
  if ((buf->imp.get_field(&buf->imp, FS_FIELD_NUM_ALLOC) + general_blocks) <
    buf->space_limit)
  {
    /* Charge the appropriate allocations */
    if (billed_blocks > 0) {
      buf->imp.set_field (&buf->imp, bill_to_field,
        buf->imp.get_field (&buf->imp, bill_to_field) + billed_blocks);
    }
    if (general_blocks > 0) {
      buf->imp.set_field (&buf->imp, FS_FIELD_NUM_ALLOC,
        buf->imp.get_field (&buf->imp, FS_FIELD_NUM_ALLOC) + general_blocks);
      buf->imp.set_field (&buf->imp, FS_FIELD_GENERAL_POOL,
        (buf->imp.get_field (&buf->imp, FS_FIELD_GENERAL_POOL)
         + general_blocks));
    }
    return 0;
  }
  return -1;
}

static void
refund_group (fsb_t imp, uint32 gid, uint32 blocks)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  uint32 general_blocks = blocks;
  uint32 reservation_blocks, reservation_used;
  uint32 refund_blocks;
  unsigned refund_to_field;

  /* Determine who is going to be charged for the allocation and if
   * allocations in the general pool are required. */
  if (!FS_IS_QUOTA_RESERVED_GROUP (gid))
  {
    refund_to_field = FS_FIELD_GENERAL_POOL;
    refund_blocks = 0;
    general_blocks = blocks;
  }
  else
  {
    reservation_used = buf->imp.get_field (&buf->imp,
        FS_GROUP_SPACE_USED (gid));
    reservation_blocks = buf->imp.get_field(&buf->imp,
        FS_GROUP_RESERVATION_SIZE (gid));

    refund_to_field = FS_GROUP_SPACE_USED (gid);
    refund_blocks = blocks;

    ASSERT (reservation_used >= blocks);
    if (reservation_used < reservation_blocks)
    {
      /* How many blocks go to the general pool */
      general_blocks = 0;
    }
    else if ((reservation_used - blocks) < reservation_blocks)
    {
      /* The reservation limit was crossed, so determine how many blocks
       * over the limit need to be returned to the global allocation count.
       */
      general_blocks = reservation_used - reservation_blocks;
    }
    else
    {
      /* All the blocks are returned to the global pool */
      general_blocks = blocks;
    }
  }

  /* Refund the appropriate allocations */
  if (refund_blocks > 0) {
    buf->imp.set_field (&buf->imp, refund_to_field,
      buf->imp.get_field (&buf->imp, refund_to_field) - refund_blocks);
  }

  if (general_blocks > 0) {
    buf->imp.set_field (&buf->imp, FS_FIELD_NUM_ALLOC,
      buf->imp.get_field (&buf->imp, FS_FIELD_NUM_ALLOC) - general_blocks);
    buf->imp.set_field (&buf->imp, FS_FIELD_GENERAL_POOL,
      buf->imp.get_field (&buf->imp, FS_FIELD_GENERAL_POOL) - general_blocks);
  }
}

static int
quota_exceeded (fsb_t imp, uint32 gid, uint32 blocks)
{
  fsb_nand_t buf = (fsb_nand_t) imp;

  if (FS_IS_QUOTA_RESERVED_GROUP(gid))
  {

    if (buf->imp.get_field (&buf->imp, FS_GROUP_SPACE_USED (gid)) + blocks >=
        buf->imp.get_field (&buf->imp, FS_GROUP_QUOTA_SIZE (gid)))
      return -1;
  }

  return 0;
}

#ifdef FS_UNIT_TEST
uint32
fs_buf_get_hardalloc (fsb_t imp)
{
  fsb_nand_t buf = (fsb_nand_t) imp;
  unsigned i;
  uint32 count = 0;

  for (i = 0; i < buf->space_limit; i++) {
    if (fs_buf_is_allocated (buf, i))
      count++;
  }

  return count;
}

int
check_freemap(fsb_t imp)
{
#if 0
  /* This test isn't correct. */

  fsb_nand_t buf = (fsb_nand_t) imp;
  uint32 i = 0;
  uint32 count = 0;

  while (i < buf->space_limit)
  {
    if (fs_buf_is_allocated (buf, i))
      count++;
    i++;
  }

  ASSERT (count == check_space_used(buf));
#else
  (void) imp;
#endif

  return 0;
}

uint32
check_space_used(fsb_nand_t buf)
{
  int i;
  uint32 space_used;

  space_used = buf->imp.get_field (&buf->imp, FS_FIELD_GENERAL_POOL);
  for (i = 1; i < FS_QR_GROUP_COUNT; i++)
    space_used += buf->imp.get_field (&buf->imp, FS_GROUP_SPACE_USED (i));

  return space_used;
}
#endif

#endif /* FEATURE_EFS_QEFS2 */
