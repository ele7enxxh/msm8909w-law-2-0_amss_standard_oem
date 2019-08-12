/***********************************************************************
 * fs_rmts_pm.h
 *
 * Remote Storage Page Manager.
 * Copyright (C) 2009-2013 QUALCOMM Technologies, Inc.
 *
 * The remote storage page manager maintains the EFS in RAM and periodically
 * flushes the entire EFS onto the remote storage.
 *
 * Upon boot, the boot module reads the EFS partitions from the SD-card
 * to the RAM, this page manager performs all EFS operations in the RAM
 * and when needed will RPC the entire EFS to the RPC server at the apps-side
 * to get the EFS image written onto the SD-card.
 *
 * The remote storage page manager divides the sectors into 3 regions:
 * Superblock, Write-Cache and Data Region. Superblock contains meta data
 * regarding the filesystem layout. The Data Regions stores the actual data
 * clusters for buffer layer. The Write-Cache is used to cache the most
 * frequently written pages to avoid huge data region syncs.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_rmts_pm.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-07-22   rp    Add new EFS API to store data in SuperBlock.
2013-03-25   dks   Fix for missed write-cache updates from sync start to end.
2013-03-24   dks   Add RAM variable to indicate full sync vs not.
2013-03-21   dks   Compute region sizes at init for use in sync.
2013-04-02   dks   Make FSG write available in non customer-fusion.
2013-03-27   dks   Featurize writing to FSG for golden copy creation.
2013-01-08   dks   Fix comments on fs1_meta_data and fs2_meta_data.
2013-01-07   dks   Enable write-cache usage to cache frequently changing data.
2013-02-04   rp    Add RAM debug-variable to RMTS-PM for debug purpose.
2013-01-16   nr    Fix issues with aegis merge of parent and child branch.
2013-01-16   dks   Move writing to Golden partition outside of efs_sync.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-12-18   dks   Write boot image headers while syncing for FS1 and FS2.
2012-11-09   nr    Update rmts to use new api's.
2012-09-20   dks   Update rmts layout to support write-cache.
2012-05-31   rp    Optimize EFS untar timings.
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2012-03-05   rp    Handle remotefs_open failure correctly.
2012-02-24   rp    Move remotefs_open calls outside of fs_global lock.
2012-01-07   wek   Move remotefs write iovec outside efs global lock.
2011-10-13   rp    Added support to log EFS-Sync timings.
2011-09-06   nr    Add GZIP support for signed golden copy.
2011-06-18   wek   Add secure TAR signing/verification.
2011-03-25   dks   Support Golden-Copy in RAM for fusion.
2010-11-18   rp    Support TAR image Golden-Copy for RMTS.
2010-11-17   rp    Use enums to refer to FS1 and FS2 eMMC partitions.
2010-10-12   vr    Change fs_iovec parameter from num_sector to size.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-03-03   rp    Fix crash in efs_sync_no_wait when called the second time.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_RMTS_PM_H__
#define __FS_RMTS_PM_H__

#include "fs_rmts_config.h"

#if defined (FEATURE_EFS_EFS2_ON_RMTS) && \
    defined (FEATURE_EFS_QEFS2)

#include "fs_pm.h"
#include "fs_rmts_super.h"
#include "fs_sync.h"
#include "comdef.h"
#include "fs_rmts.h"
#include "fs_rmts_bootup_action.h"

/** Pointers to various EFS partition regions in RAM */
struct fs_rmts_pm_efs_copy
{
  uint8 *ram_addr;
  struct fs_rmts_superblock *superblock;
  uint8 *write_cache_region;
  uint8 *data_region;
};

enum fs_rmts_pm_fsg_file_type
{
  FS_RMTS_PM_FSG_FILE_TYPE_INVALID,
  FS_RMTS_PM_FSG_FILE_TYPE_TAR_FILE,
  FS_RMTS_PM_FSG_FILE_TYPE_ZIP_TAR
};

/** Keep track of time spent in each stage of an EFS-Sync. */
struct fs_rmts_pm_sync_timing
{
  uint32                     enc_time;
  uint32                     write_iovec_time;
  enum fs_rmts_device_errno  rmts_result;
};

/* The PM stores a header with the following structure at the beginning of the
   write-cache region to interpret its layout. */
PACKED struct fs_rmts_write_cache_header
{
  uint32 version;
  /* Byte offset of the Cluster table from header start. Cluster table stores
     the mapping of what cluster data a particular cache cluster contains. */
  uint32 write_cache_ctable_byte_offset;
  /* Number of entries in the Cluster table. Same as the number of clusters
     that can be cached in the write-cache. */
  uint32 write_cache_ctable_num_entries;
  /* Byte offset from hdr start where the actual cached clusters start. */
  uint32 write_cache_byte_offset;
}PACKED_POST;

#define FS_RMTS_PM_WRITE_CACHE_HDR_SIZE \
               (sizeof (struct fs_rmts_write_cache_header))


PACKED struct fs_rmts_pm_debug_info
{
  unsigned int fs1_sb_decrypt_failed : 1;
  unsigned int fs1_wc_decrypt_failed : 1;
  unsigned int fs1_dr_decrypt_failed : 1;

  unsigned int fs2_sb_decrypt_failed : 1;
  unsigned int fs2_wc_decrypt_failed : 1;
  unsigned int fs2_dr_decrypt_failed : 1;

  unsigned int fsg_sb_decrypt_failed : 1;
  unsigned int fsg_wc_decrypt_failed : 1;
  unsigned int fsg_dr_decrypt_failed : 1;

  unsigned int booted_out_of_fsg : 1;/**< Indicates if we boot out of FSG. */

  uint32 fs1_decrypt_age_at_boot;
  uint32 fs2_decrypt_age_at_boot;
  uint32 fsg_decrypt_age_at_boot;
}PACKED_POST;

/* Main structure of the remote storage page manager */
struct fs_rmts_pm
{
  struct fs_pm_data     parent;   /**< Ops to expose to Buffer-Layer. */

  efs_device_t          dev;
  int32                 fresh_start; /**< is this a fresh start for the PM? */
  int32   is_oem_forced_fresh_start; /**< is this fresh start forced by OEM?*/


  uint32                ram_size; /**< How big is the device RAM. */
  uint32                max_sectors; /**< Max sectors used by page manager. */
  uint32                page_size;   /**< size of page or cluster */
  uint32                avail_clusters; /**< How many clusters available to
                                             the buffer layer */

  uint32                inside_txn; /**< are we inside a transaction? */
  uint32                txns_since_sync; /**< How many txns since last sync? */

  struct fs_rmts_pm_efs_copy modem_copy;  /**< Modem copy in RAM */
  struct fs_rmts_pm_efs_copy shared_copy; /**< Shared-mem copy in RAM */
  struct fs_rmts_pm_efs_copy golden_copy; /**< Golden-mem copy in RAM */

  enum fs_rmts_modem_type   modem_type; /**< The type of rmts modem */
  int  write_to_fsg_for_golden_copy;  /**< Directly write to FSG from Modem
                                          for golden copy creation. */
  enum fs_rmts_partition_id active_efs_partition; /**< in eMMC card */

  struct fs_sync_data sync_data; /**< Sync module state data. */

  uint32  prev_sb_age; /**< Previous superblock age. Used when the write iovec
                            call failed. */
  uint32  prev_txns_since_sync; /**< Previous transaction count. Used  when
                                     remote iovec write call failed. */
  uint8   prev_which_partition; /**< Previous value of which_partition field in
                                     the superblock. */

  enum fs_rmts_pm_fsg_file_type fsg_file_type; /**< The type of file in FSG */
  uint32          untar_size; /**< 0 indicates no untar is pending,
                  otherwise the size of fs image to untar from shared ram. */
  uint8           *untar_buffer; /**< Malloced buffer to hold data to untar. */
  uint32          untar_buffer_size; /**< Malloced buffer size for untar hash*/
  uint8           *temp_hash_ptr; /**<  Intermediate untar chain hash. */
  uint8           *final_hash_ptr; /**< Final untar chain hash when the
                                        TAR-file was in modem-ram. */

  /* Variables below are protected only by Sync lock, not by global lock.*/
  fs_os_tcb_type *waiting_task; /**< Task waiting for sync to finish. */
  int            sync_result;   /**< Result of remotefs api calls. */

  /** Recent past sync timings */
  struct fs_rmts_pm_sync_timing   sync_timings[FS_RMTS_SYNC_TIME_QUEUE_SIZE];
  uint32 sync_timing_index; /**< index to the above sync_timings array */
  struct fs_rmts_pm_sync_timing   curr_sync_timing;/**< current sync timings.*/

  uint8 is_curr_sync_full_sync; /**< Write-cache and data region both synced */
  uint32 write_cache_sync_size_bytes;
  uint32 full_sync_size_bytes;

  /* Write cache region related information */
  uint8 is_write_cache_present;
  uint8 did_write_cache_overflow;
  uint8 did_write_cache_overflow_at_sync_start;
  uint32 write_cache_mask;
  struct fs_rmts_write_cache_header *write_cache_hdr;
  uint32 *write_cache_cluster_table;
  uint8 *write_cache;
  uint32 write_cache_region_size_bytes;
  uint32 write_cache_size_bytes;

  /* Data region related information */
  uint8* data_region;
  uint32 data_region_size_bytes;

  struct fs_rmts_image_header *image_header; /**< Address of image header */

  struct fs_rmts_pm_debug_info  debug_info; /*< Fields used for debugging. */

  /** Save the meta data generated from encryption of data region for each
      partition. Since the data region always changes between each sync, when
      we do a sync that only writes to the write-cache region, we need to use
      the right hash for the data present in that partition. This will not be
      the same as the hash used in the last encryption of the data region. */
  uint8 fs1_meta_data [FS_RMTS_META_DATA_SIZE];
  uint8 fs2_meta_data [FS_RMTS_META_DATA_SIZE];

  enum fs_rmts_partition_id booted_out_of_efs_partition;

  struct fs_rmts_bootup_action_image_info fs1_bootup_action_image_info;
  struct fs_rmts_bootup_action_image_info fs2_bootup_action_image_info;
  struct fs_rmts_bootup_action_image_info fsg_bootup_action_image_info;

};

/*************************************************************************//**
 * Initialize the remote-storage page manager.
 *
 * @param dev
 *    Pointer to the device.
 *
 * @return
 *    Pointer to the pm ops.
*****************************************************************************/
fs_pm_t fs_rmts_pm_init (efs_device_t dev);

#endif /* FEATURE_EFS_EFS2_ON_RMTS */


#endif /* not __FS_RMTS_PM_H__ */
