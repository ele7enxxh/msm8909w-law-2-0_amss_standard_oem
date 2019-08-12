/***********************************************************************
 * fs_buffer.h
 *
 * Buffering operations for Embedded File System.
 * Copyright (C) 2001-2013 QUALCOMM Technologies, Inc.
 *
 * This file describes the generic interface to buffering operations in
 * EFS.  Each "cluster" (read/write unit of filesystem data) is managed
 * here.
 *
 * At this level, data is treated and stored in a number of fixed sized
 * clusters.  Clusters are referred to by cluster number.
 *
 * Although below this interface activities such as garbage collection and
 * protection may be happening, this is hidden through this interface.
 *
 * This interface also presents a non-overlapping transaction interface.
 * This mechanism supports protecting the integrity of the tree of
 * meta-data stored above this level.
 */


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_buffer.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-07-22    rp      Add new EFS API to store data in SuperBlock.
2013-06-19    dks     Added NAND Flash scrub support.
2013-05-07    wek     Add EFS2 Read Only support.
2013-01-11    dks     Move writing to Golden partition outside of efs_sync.
2012-12-18    dks     Update golden copy strategy for new rmts layout.
2012-12-18    dks     Make number of EFS buffers configurable.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2011-10-24    nr      Minor Cleanup and Fixes.
2011-10-11    nr      Added borrow_ram support to borrow ram from lower layers.
2011-08-01    nr      Change EFS Buffer use and replacement strategy.
2011-03-08    rp      Add support to lock EFS.
2010-11-18    rp      Support TAR image Golden-Copy for RMTS.
2010-10-20    wek     Make buffer layer independent of page size macro.
2010-09-01    wek     Remove compile time calculations based on page size.
2010-07-30    wek     Dynamically allocate all buffers dependent on page size.
2010-05-13    wek     Make sync_no_wait non blocking.
2010-05-06    wek     Implement golden copy for remote storage page manager.
2010-04-12    wek     Added 8K page support.
2010-01-19    vr      Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-04-01    yg      Moved buffer layer's internal config from public header
2008-12-12    rp      Introducing i/o-vec support.
2008-11-03    rp      Introduce device abstraction.
2008-10-06    np      Add b_shred for shredding obsolete user data.
2008-09-25    yg      Revert the freemap size macro redefinition.
2008-09-16    yog     Remove trailing commas from the enum declaration.
2008-09-04    yg      Make sure freemap size is at least a page size
2008-01-02    yg      Updated write wire param to enum type.
2007-12-28    yg      Make tag names as enums instead of numeric consts.
2007-11-02    yg      Fix dup macro because of GP auto Merge.
2007-10-18    rp      Featurized QEFS2.
2007-09-07    rp      Moved IS_QUOTA_RESERVED_GROUP to fs_sys_types.h
2007-08-10    rp      Align fsb_nand_data buffers on a 32-byte boundary.
2007-07-30    sh      Added fs_borrow_nand_data().
2007-07-25    umr     Added support for 4k Page size.
2007-07-10    yg      Added some comments for the tags.
2007-06-29    sch     Pass in a data tag to write_wire
2007-06-22    sch     Pass hint to write_wire to read cluster from flash
2006-11-02    dlb     Delete recovery support.
2006-10-26    dlb     Put freemap changes back.
2006-10-11    dlb     Back out freemap changes for now.
2006-10-10    dlb     Partial transaction flush support.
2006-10-02    dlb     Add transaction callback for testing.
2006-09-18    dlb     Cache freemap as independent data structure.
2006-09-08    dlb     Moved buffer size configuration into fs_config.h
2006-03-02    sh      Removed FS_FIELD_LIMIT_DIFF
2006-01-27    sh      Renamed some Q&R defines to be more explanatory
2006-01-03    nrs     More Lint cleanup
2005-12-22    dlb     Add hardalloc query.
2005-10-26     sh     Lint cleanup.
2005-10-25    nrs     Changed RESERVATION_AVAIL to RESERVATION_ALLOC
2005-10-17    nrs     Cleaned up Q&R, Added macros, defines, and comments
2005-10-11    nrs     Remove FS_ALLOC_SOFT, FS_ALLOC_HARD and quota_exceeded
2005-10-06    nrs     Adjusted group lookup macro to make gid 0 being special
2005-09-21    nrs     Add function to check for exceeded quota
2005-09-13    nrs     Implement reservation management for efs_chown ()
2005-08-25    nrs     Implent buffer accounting for quotas and reservations
2005-07-22    nrs     Added set and get for quotas and reservations
2005-04-26    dlb     Add 2K page support.
2005-03-23    dlb     RVCT fix.
2005-01-04    dlb     Update copyright line.
2004-12-30    dlb     Provide hooks for new factory image code.
2004-11-05    dlb     Additional checking of buffers in unit test.
2004-10-15    dlb     Fix copyright line.
2004-10-07    dlb     Whitespace cleanup.
2003-10-23    dlb     Internal clusters use wires.
2003-09-09    dlb     Factory image works with bitmap and tree.
2003-08-28    dlb     Bitmap allocate/deallocate with move from free.
2003-08-20    dlb     Add tree allocate/deallocate.
2003-06-17    jkl     Clean up code.
2003-05-19     cr     Changed FS_BUFFER_NUMBER_BUFFERS from 12 to 24.
2003-04-21     gr     Merged the contents of fs_buffer_nand.h and
                      fs_buffer_nandi.h into this file.
2002-08-09    drh     Created by dlb.  Added history header

===========================================================================*/

#ifndef __FS_BUFFER_H__
#define __FS_BUFFER_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QEFS2

#include "fs_device.h"
#include "fs_pm.h"
#include "fs_sys_types.h"

/* These flags decide whether to read the cluster from the flash or just
 * to allocate an empty buffer for it */
typedef enum fs_buffer_read_cluster {
  FS_BUF_READ_CLUSTER,
  FS_BUF_DO_NOT_READ_CLUSTER
} fs_read_hint;

/* Type of data indication while writing into buffer. This is some information
 * which is also passed into PM layer so that if it is possible to do some kind
 * of optimization in PM. PM need not know too much details about what kind of
 * data, but its just a number which might help PM to do some data management
 * to improve throughput. This isn't really needed while reading the page, only
 * hint while writing. */
typedef enum fs_buffer_data_tag {
  FS_DATA_TAG_DATA    = 0,  /* File data cluster       */
  FS_DATA_TAG_FREEMAP = 1,  /* Free map cluster        */
  FS_DATA_TAG_INFO    = 2,  /* FS Info cluster         */
  FS_DATA_TAG_INODE   = 3,  /* Inode cluster           */
  FS_DATA_TAG_DB      = 4   /* Database cluster        */
} fs_buffer_data_tag_type;

/* We need to differentiate between allocations that should be allowed to
 * occur past the soft limit on allocations and those that should not.
 * The allocation function takes a second parameter that indicates the
 * type of the allocation, which should be one of the following two values.
 */

struct fsb_implementation;
typedef struct fsb_implementation *fsb_t;
struct fsb_implementation {
  /* Get the size of clusters for this implementation. */
  unsigned (*cluster_size) (fsb_t imp);

  /* Call these two functions around a transaction.  These transactions are
   * intended to be short. */
  void (*start_transaction) (fsb_t imp);
  void (*end_transaction) (fsb_t imp);
  void (*rollback_transaction) (fsb_t imp);

  /* Certain high-level operations can't be written entirely in a single
   * transaction.  The high-level code does need to be able to query if
   * this is the case, and be able to perform the updates across several
   * transactions.
   *
   * This will be generally used something like:
   *   int set_in_process = 0;
   *   while (buf->is_too_large (buf)) {
   *     if (!set_in_process) {
   *       info = buf->write_write (...);
   *       info->in_process = ...;
   *       buf->free_wire ();
   *       set_in_process = 1;
   *     }
   *     buf->partial_transaction_flush (buf);
   *   }
   *   if (set_in_process) {
   *     info = buf->write_wire (...);
   *     info->in_process = 0;
   *     buf->free_wire (...);
   *   }
   *   buf->end_transaction (buf);
   * */

  /* Returns true if there is too much work to do in a single transaction.
   * It _might_ be possible to do the end_transaction anyway, but if code
   * is prepared to split a transaction it should do so. */
  int (*is_too_large) (fsb_t imp);

  /* Perform part of a transaction.  At this point, we assume that all
   * clusters modified through write_wire() can be written out, but changes
   * to the freemap will be too large.  This call writes out all
   * user-modified clusters, and at least part of the free map. */
  void (*partial_transaction_flush) (fsb_t imp);

  /* Allocate a new cluster of data.  The allocation itself is only valid
   * within the context of a transaction, and the cluster must be written
   * to before the transaction has finished. */
  int (*allocate) (fsb_t imp, uint32 gid, fs_cluster_id * id);

  /* Wire down a cluster of data in memory for read access.  The wiring may
   * be converted to a write wire with [write_wire].  In either case, it
   * must be freed before ending the transaction. */
  void * (*read_wire) (fsb_t imp, fs_cluster_id id);

  /* Wire down a cluster of data in memory for write access.  If a read
   * wire is being changed, it is possible (and even likely) that the
   * address returned will be different than the address returned by
   * read_wire.  The wire must be freed before the transaction can be
   * ended. */
  void * (*write_wire) (fsb_t imp, fs_cluster_id id,
                        fs_read_hint read_hint, fs_buffer_data_tag_type tag);

  /* Free a wire.  Write_wired data will be written, and any buffer space
   * reclaimed. */
  void (*free_wire) (fsb_t imp, fs_cluster_id id);

  /* Deallocate the given chunk.  After the transaction finishes,
   * references to the chunk will be invalid. */
  void (*deallocate) (fsb_t imp, fs_cluster_id id, uint32 gid);

  /* Deallocate a given tree.  [depth] is the number of indirection levels
   * in the tree.  For example, a single indirect block would have a depth
   * of 1.  The count is a count of the number of referenced clusters,
   * including the tree nodes.  If you call deallocate_tree multiple times
   * within a transaction, it is only necessary that all of the counts for
   * the calls add to the total number of free clusters.  (Typically, when
   * freeing a file, you will only know the total, not each individual
   * tree). */
  void (*deallocate_tree) (fsb_t imp, fs_cluster_id id,
      unsigned depth, unsigned count, uint32 gid);

  /* Mark the given cluster as deallocated.  This is only done during
   * recovery (after failure during a partial transaction_flush. */
  void (*deallocate_recovered) (fsb_t imp, fs_cluster_id id);

  /* Retrieve the given "field". */
  uint32 (*get_field) (fsb_t imp, int field_id);

  /* Set the value for the given "field". */
  void (*set_field) (fsb_t imp, int field_id, uint32 value);

  /* Return the number of writes that are pending in this transaction.
   * This includes pages that are write wired, as well as writes that have
   * happened due to allocation and deallocation. */
  unsigned (*get_write_count) (fsb_t imp);

  /* Return the space limit in number of clusters. */
  fs_cluster_id (*get_space_limit) (fsb_t imp);

  /* Return the number of clusters currently allocated. */
  fs_cluster_id (*get_num_alloc) (fsb_t imp);

  /* Set the size of the reservation */
  int (*set_reservation) (fsb_t imp, uint32 gid, uint32 bytes);

  /* Set the size of the quota */
  int (*set_quota) (fsb_t imp, uint32 gid, uint32 bytes);

  /* Get the quota and reservation information for a group id */
  int (*get_group_info) (fsb_t imp, uint32 gid, struct fs_group_info *ginfo);

  /* Manage the reservation allocation when chown'ing a file */
  int (*chown) (fsb_t imp, int old_uid, int new_uid, int old_gid, int new_gid,
      uint32 blocks);

  /* Shred obsolete user data from the file systems. */
  int (*shred) (fsb_t imp);

  /* Move all written pages to new erased blocks. */
  int (*scrub) (fsb_t imp);

  /* Start sync procedure to underlying filesystem */
  int (*sync_start) (fsb_t imp);

  /* Block caller and wait for sync to complete */
  int (*sync_wait) (fsb_t imp);

  /* Cleanup procedures after sync wait. */
  int (*sync_end) (fsb_t imp);

  /* Request a non blocking sync */
  int (*sync_no_wait) (fsb_t imp, fs_sync_token_t *sync_token);

  /* Return sync completion status using previously generated token */
  int (*sync_get_status) (fsb_t imp, fs_sync_token_t sync_token,
                          enum fs_sync_result *sync_result);

  /* Make a golden copy (backup) of the current file system. */
  int (*make_golden_copy) (fsb_t imp);

  /* Called when efs_initialize is complete */
  int (*fs_init_complete) (fsb_t imp);

  /* Lock filesystem for block all further access */
  int (*lock_efs) (fsb_t imp);

  /* Borrow a chunk of ram from lower layers. Data is clobbbered */
  int (*borrow_ram) (fsb_t imp, void **ram_addr, uint32 *ram_size);

  /* Called when efs_initialize is complete */
  int (*store_data_in_sb) (fsb_t imp, const void *data, uint32 data_size);
};

/* The buffer manager also has the ability to store a small number of 32
 * bit words that are updated in the context of transactions, yet do not
 * carry the entire burden of updating an entire page in flash for each
 * one.  The maximum number of these "Fields" is defined in fs_super.h.
 * These fields are used by various different layers of the FS.  Each
 * defined field should be described here, along with a comment as to where
 * it is used.
 *
 * Fields are all set to FS_INVALID_CLUSTER_ID (or all F's) on a freshly
 * initialized filesystem. */

/* These first two fields are used by the buffer management code itself to
 * keep track of free clusters. */
#define FS_FIELD_FREEMAP_BASE   0
#define FS_FIELD_FREE_CHAIN     1

/* This field stores the root of the database.  Since the database needs to
 * change this when the tree changes depth, it uses a field to store this
 * information. */
#define FS_FIELD_DB_ROOT        2

/* This field stores the cluster number of the info-block for the
 * filesystem.  This is described in fs_mount.[ch]. */
#define FS_FIELD_FS_INFO        3

/* This field is used to keep track of the number of allocated clusters. */
#define FS_FIELD_NUM_ALLOC      4

/* This field is now unused and uninitialized.
 * (It was formerly the difference between the soft and hard limits.) */
#define FS_FIELD_UNUSED         5

/* This field tracks allocations to the general pool */
#define FS_FIELD_GENERAL_POOL   6

/* This field holds the space limit for the given build */
#define FS_FIELD_SPACE_LIMIT    7

/* These macros define the index of the group information */
/* To calculate the index for a given field, we need to used the base
 * address, then add 4 * (gid -1) since each group contains 4 elements and
 * the gid is offset by the reservation group by 1, and then finally add
 * the offset for the specific element in the group we are looking for.
 */

#define FS_GROUP_QUOTA_SIZE(gid) (FS_GROUP_INFO_START + (FS_FIELDS_PER_GROUP *\
    (gid - 1)))
#define FS_GROUP_RESERVATION_SIZE(gid) (FS_GROUP_INFO_START + \
    (FS_FIELDS_PER_GROUP * (gid - 1)) + 1)
#define FS_GROUP_SPACE_USED(gid) (FS_GROUP_INFO_START + (FS_FIELDS_PER_GROUP *\
    (gid - 1)) + 2)

struct fsb_nand_data;
typedef struct fsb_nand_data *fsb_nand_t;

void fsb_buffer_initialize (fsb_nand_t imp, fs_pm_t gc, uint32 page_size);
int fsb_buffer_init_read_only (fsb_nand_t imp, fs_pm_t gc, uint32 page_size);
void fsb_nand_shutdown (fsb_nand_t imp);

/**********************************************************************
 * Private data.
 *********************************************************************/

/* Structure to keep track of the state of each buffer. */
enum fs_buffer_state_t {
  FS_BSTATE_FREE,
  FS_BSTATE_READ_WIRE,
  FS_BSTATE_READ_WIRE_DIRTY,
  FS_BSTATE_WRITE_WIRE,
  FS_BSTATE_WRITE_DIRTY
};

/* Kept for each wired page used internally. */
struct fsb_wired_data {
  fs_cluster_id   cluster;
  void            *data;
  int             write_wired;
};
typedef struct fsb_wired_data *fsb_wired_t;

#define FS_BUFFER_FREE_BITMAP_UINT32_COUNT \
  ((FS_BUFFER_FREEMAP_SIZE / sizeof (uint32)))
/* Number of bits needed to have a dirty map of freemap.
 * The total number of cluster needed to hold the freemap is
 * freemap size / page size, the dirty map is one bit for each cluster
 * of the freemap. The maximum number of clusters for needed for freemap
 * will be when page size is the smallest possible value. For example,
 * if the smalles page size is 512 and freemap 64K, then we have 128 freemap
 * clusters. The dirty map will be 128 bits or 4 uint32. For 8K we will need
 * one uint32 for the dirtymap.
 * We will allocate an array of this size to hold the dirty map. The maximum
 * size is small therefore we will allocate it statically, although it
 * depends on the page size. */
#define FS_BUFFER_FREE_DIRTYMAP_UINT32_COUNT \
  (((FS_BUFFER_FREEMAP_SIZE / FS_MIN_PAGE_SIZE) + 31) / 32)

/* Local memory cache for the free bitmap. */
struct fsb_freemap_data {
  unsigned      cluster_count;

  /* First cluster holding the freemap.  Only this variable holds the
   * offset.  Other variables, such as 'last_index', are 0-based, adding
   * them to 'base' to get the real cluster. */
  fs_cluster_id base;

  /* Tracking the last index in the bitmap we've looked at. */
  unsigned      last_index;

  /* Pointer to the entire free bitmap.  Bits in this array correspond with
   * clusters, starting at number '0', packed little endian within each
   * uint32.  A '1' bit indicates that the specific cluster is free. */
  uint32        *bitmap;

  /* A bitmap, little endian, marking which clusters in the freemap have
   * been dirtied. For page sizes greater than FS_MIN_PAGE_SIZE there will
   * be unused entries. */
  uint32        dirty[FS_BUFFER_FREE_DIRTYMAP_UINT32_COUNT];
};

/* For each buffer, keep track of its state, and which cluster it is
 * associated with. */
struct fs_buffer_info {
  enum fs_buffer_state_t        state;
  fs_buffer_data_tag_type       tag;
  fs_cluster_id                 cluster;
  /* Time at which the buffer was last accessed. */
  uint32                        access_time;

#ifdef FS_UNIT_TEST
  /* Unit test consistency hash. */
  uint8                         hash[16];
#endif
};

/* Cluster of data containing a list of other clusters that are free.
 * After the count drops to zero, this chain itself will be allocated, and
 * we move on to the next entry in the chain.  The data field is heap
 * sorted (data[0] is the count of data, so that data[1] is the first in
 * the heap, this eliminates many of the -1 normally found in heap
 * management. */
struct cluster_chain {
  uint32        next;
  uint32        data[128-1];
};

/* Each of the clusters listed in the cluster_chain is actually the root of
 * a tree.  The top few bits of the data field indicate the depth of the
 * tree.  We use a depth of zero to indicate just a single cluster, which
 * keeps us compatible with the single cluster deallocations. */
#define FSB_MAX_DEPTH           4
#define FSB_TREE_DEPTH(x)       ((x) >> 29)
#define FSB_TREE_CLUSTER(x)     ((x) & 0x1FFFFFFF)
#define FSB_TREE_MAKE(cl,dep)   ((cl) | ((dep) << 29))

struct fsb_nand_data {
  /* Our parent data.  This _must_ be the first element of the structure. */
  struct fsb_implementation imp;

  /* Time based on buffer accesses. Incremented each time a buffer is searched
     for in find_buffer */
  uint32 current_time;

  /* The garbage collection data. */
  fs_pm_t gc;

  /* Nesting of transactions.  Since the lowest level can only be called
   * once, we just count them otherwise. */
  int transaction_nest;

  /* Size of a single cluster in bytes */
  uint32 cluster_size;

  /* The state information for each buffer. */
  struct fs_buffer_info info[FS_BUFFER_NUMBER_BUFFERS];

  /* The buffer pointer, pointing to actual buffers at aligned address. */
  uint8 *buffers[FS_BUFFER_NUMBER_BUFFERS];

  uint32 fs_buffer_number_mask; /* For quick indexing into the cache */

  struct fsb_wired_data chain;

  /* Free cluster tree "cache". */
  struct fsb_wired_data tree[FSB_MAX_DEPTH];
  int tree_offset[FSB_MAX_DEPTH];

  /* How many writes are in this transaction. */
  uint32 write_count;

  /* Pre-computed hard space limit.  Allocates past this will fail. */
  fs_cluster_id space_limit;

  /* Freemap cluster buffer. */
  struct fsb_freemap_data freemap;

  /* Local cache of the values of the fields. */
  uint32 fields[FS_UPPER_DATA_COUNT];
  /* Dirty bits for the fields.  Needs to change if there are more than 32
   * fields. */
  uint32 field_dirty_mask;
};

/* Factory: Return if there is any work in the free tree that needs to be
 * moved. */
int fs_buffer_has_work (fsb_t imp);

/* Factory: Used to determine if a given cluster has been allocated. */
int fs_buf_is_allocated (fsb_nand_t buf, fs_cluster_id id);

/* Factory: Used to borrow access to the EFS buffers as scratch space. */
struct fsb_nand_data *fs_borrow_nand_data (void);

/* To borrow EFS buffers to do one time intialization on a fresh_start. */
uint8* fs_borrow_data_buffers (uint32 *bufsize_bytes);

#ifdef FS_UNIT_TEST
int check_freemap (fsb_t imp);
uint32 check_space_used (fsb_nand_t buf);
uint32 fs_buf_get_hardalloc (fsb_t imp);

/* If non-null, this callback will be called before each low-level
 * transaction is started.  This allows transaction safety to be tested
 * above the page manager layer.  The callback takes no argument, and
 * doesn't return a value, but can use longjmp to escape from the
 * fielsystem. */
void (*fsb_transaction_callback) (void);
#endif /*FS_UNIT_TEST*/

#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_BUFFER_H__ */
