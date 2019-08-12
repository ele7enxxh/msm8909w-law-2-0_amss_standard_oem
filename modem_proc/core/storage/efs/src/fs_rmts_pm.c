/***********************************************************************
 * fs_rmts_pm.c
 *
 * Remote Storage Page Manager.
 * Copyright (C) 2009-2014, 2016 QUALCOMM Technologies, Inc.
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
 * clusters from buffer layer. The Write-Cache is used to cache the most
 * frequently written pages to avoid huge data region syncs.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_pm.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-06-08   sb    Fix KW warnings.
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-11-01   wek   Allow syncs after creation of golden copy.
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-09-25   dks   Fix compiler warnings.
2013-07-22   rp    Add new EFS API to store data in SuperBlock.
2013-07-17   wek   Move MD5 stubs into util header file.
2013-06-19   dks   Added NAND Flash scrub support.
2013-03-25   dks   Fix for missed write-cache updates from sync start to end.
2013-03-24   dks   Add RAM variable to indicate full sync vs not.
2013-03-21   dks   Compute RMTS PM region sizes at init for use in sync.
2013-04-02   dks   Make FSG write available in non customer-fusion.
2013-03-27   dks   Featurize writing to FSG for golden copy creation.
2013-03-08   dks   Add apps sync control, remove timeout in read/write-iovec.
2013-02-28   dks   Add additional debugging information on write-cache.
2013-02-22   dks   Remove support for unsigned image in FSG.
2013-02-12   dks   Fix limit check on cluster table index on rollover.
2013-02-08   nr    Fix compiler warnings.
2013-02-08   dks   Fix variable name used in memcpy.
2013-02-07   dks   Enable write-cache usage to cache frequently changing data.
2013-02-07   wek   Remove Klocwork warning.
2013-02-05   wek   Add read back verify for an efs sync call.
2013-02-04   rp    Add RAM debug-variable to RMTS-PM for debug purpose.
2013-01-29   nr    Migrate to new signed image authentication apis.
2013-01-17   dks   Fix image source offset calculation for rmts image header.
2013-01-16   dks   Move writing to Golden partition outside of efs_sync.
2013-01-15   nr    Fix bug to use correct shared ram address for golden copy.
2012-12-05   wek   Fix compile warning when getting the next partition.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-12-18   dks   Write boot image headers while syncing for FS1 and FS2.
2012-12-05   wek   Fix compile warning when getting the next partition.
2012-11-09   nr    Update rmts to use new api's.
2012-11-08   wek   Fix compile warning when decrypting active efs copy.
2012-10-19   dks   Make superblock age run to uint32_max.
2012-09-20   dks   Update rmts layout to support write-cache.
2012-08-09   wek   Move the superblock metadata to the end of the superblock.
2012-08-09   dks   Rename fs_cookie.h to fs_cookie_i.h.
2012-06-12   dks   Cleanup IMEM cookies and cookie APIs.
2012-05-31   rp    Optimize EFS Untar timings.
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2012-05-23   dks   Cleanup code to remove Halcyon behavior.
2012-05-07   dks   Support storing/reading cookies to/from emmc partitions.
2012-04-30   rp    Add safe benchmark command to measure efs_sync timings.
2012-04-02   wek   Cleanup return value for authenticate/decrypt function.
2012-03-28   nr    Add colors and clean debug MSGs and ERRORs.
2012-03-05   rp    Handle remotefs_open failure correctly.
2012-02-24   rp    Move remotefs_open calls outside of fs_global lock.
2012-02-13   rp    Add security-api's clock enable disable support.
2012-01-19   dks   Fix remotefs client init sequence for QMI based transfer.
2012-01-07   wek   Move remotefs write iovec outside efs global lock.
2011-11-29   nr    Fix same age superblock in FS1 and FS2 when FSG is present.
2011-10-24   nr    Minor Cleanup and Fixes.
2011-10-13   rp    Added support to log EFS-Sync timings.
2011-10-11   nr    Add GZIP support for signed golden copy.
2011-10-10   dks   Remove fs_hotplug.h inclusion.
2011-10-10   dks   Remove Hotplug dependency from rmts files.
2011-10-04   dks   Make EFS VU independent of Hotplug and Hfat VUs.
2011-08-26   rp    Fix compiler warning when FS_TAR feature is disabled.
2011-06-18   wek   Add secure TAR signing/verification.
2011-06-03   wek   Print return value when authentication fails.
2011-05-27   rp    Handle write_iovec failure when remote server is down.
2011-05-09   dks   Do not set backup cookie for 9k simulator in PM.
2011-03-25   dks   Support Golden-Copy in RAM for fusion.
2011-03-25   rp    Support RemoteFS layer for QMI changes.
2011-03-08   rp    Add support to lock EFS.
2011-02-28   wek   Add debugging info on why ERR_FATAL was hit.
2011-01-13   vr    Initial support for golden copy in RAM.
2010-12-03   rp    Featurize FS-TAR module.
2010-12-02   rp    Fix compiler warnings seen in Halcyon WM target builds.
2010-12-01   rp    Allow TAR in FSG only when FSG is ready by OSBL.
2010-11-29   rp    Use light-weight SEC-APIs.
2010-11-24   rp    Use shared RAM address only for reading/writing.
2010-11-19   rp    Added support to read FSG from modem if possible.
2010-11-18   rp    Support TAR image Golden-Copy for RMTS.
2010-11-17   rp    Use enums to refer to FS1 and FS2 eMMC partitions.
2010-11-17   rp    Avoid reset to try FSG on first ever boot.
2010-11-15   rp    Replace Boot cookies with EFS cookies.
2010-10-12   vr    Change fs_iovec parameter from num_sector to size.
2010-08-20   vr    Added logic to load efs partitions in EFS for rmts targets.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-05-06   wek   Implement golden copy for remote storage page manager.
2010-03-31   wek   Added authentication of EFS copies.
2010-03-15   rp    Optimize the efs sync timer.
2010-03-04   wek   Removed unneeded unit test code to sync automatically.
2010-03-03   rp    Fix crash in efs_sync_no_wait when called the second time.
2010-01-19   vr    Added efs_sync_no_wait and efs_sync_get_status APIs.
2010-01-07   wek   Calling new encryption function signatures.
2009-11-18   wek   Make the runtime flag below compile time.
2009-11-17   wek   Added runtime flag to use encryption/decryption.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_rmts_config.h"

#if defined (FEATURE_EFS_EFS2_ON_RMTS) && \
    defined (FEATURE_EFS_QEFS2)

  /*************************************************************************

  FS PARTITION LAYOUT
                    |-----------------------|
                    |                       |         Sector 0
                    |    EFL/MBN Header     |
                    |-----------------------|         Sector 1
             -------|                       |
             |      |      EFS Super        |
    ---------|------|                       |
    |        ------>|-----------------------|
    |               |                       |         Sector 2 to x
    |               |      Write-Cache      |         if Write-Cache enabled
    |               |                       |
    --------------->|-----------------------|
                    |                       |         Sector x+1 to last sector
                    |                       |         if Write-Cache enabled
                    |                       |                    OR
                    |      Data Region      |         Sector 2 to last sector
                    |                       |         if Write-Cache disabled
                    |                       |
                    |                       |
                    |-----------------------|


Write-Cache Region
                    |-----------------------|---------
                    |   Write Cache Header  |-----   |
                    |                       |    |   |
                    |  -------------------  |<----   |
                    |Write-Cache Cluster Tbl|        |
                    |                       |        |
                    |  -------------------  |<--------
                    |                       |
                    |      Write-Cache      |
                    |                       |
                    |-----------------------|


  Partition Layout when TAR is loaded
                    |-----------------------|
                    |                       |
                    |    EFL/MBN Header     |       Sector 0
                    |-----------------------|
                    |                       |
                    |        Signed/        |
                    |  Optionally Zipped/   |
                    |       TAR file        |       Sector 1 to x
                    |-----------------------|

  ***************************************************************************/

#include "fs_rmts_pm.h"
#include "fs_err.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_osal.h"
#include "fs_task_i.h"
#include "fs_cookie_i.h"
#include "fs_util.h"
#include "fs_timings.h"
#include "fs_rmts_guid.h"
#include "fs_rmts_image_header.h"
#include "fs_rmts_cookie.h"
#include "secapi_util.h"
#include "assert.h"

#ifdef FEATURE_FS_TAR
  #include "fs_tar_i.h"
#endif
#include "fs_zip.h"

/* Uncomment the line below to enable "Read Back Verify" to happen outside
 * FS task context. On some targets there is a limitation in RMTS and
 * reads are only supported from FS Task. */
//#define EFS_READ_BACK_ALLOW_ANY_TASK
#ifndef EFS_READ_BACK_ALLOW_ANY_TASK
extern fs_os_tcb_type *efs_tcb;
#endif

#define FS_READ_BACK_VERIFY_HASH_LEN   16
static struct fs_iovec fs_verify_iovec; /* Holds the last write iovec. */

/* The variable below will control if the file system will do read back
 * verify of data written during an efs_sync(). If this variable should be
 * changed to non zero ONLY for debug purposes. It will increase the
 * sync time and potentially go beyond the time limit for an efs_sync. */
volatile uint8 fs_enable_read_back_verify_sync = 0;

/* Default sector offset and sizes of various regions */
#define FS_RMTS_PM_IMG_HEADER_START_SECTOR    0
#define FS_RMTS_PM_IMG_HEADER_NUM_SECTORS     1

#define FS_RMTS_PM_SUPERBLOCK_START_SECTOR  \
   (FS_RMTS_PM_IMG_HEADER_START_SECTOR + FS_RMTS_PM_IMG_HEADER_NUM_SECTORS)
#define FS_RMTS_PM_SUPERBLOCK_NUM_SECTORS     1

#define FS_RMTS_PM_WRITE_CACHE_START_SECTOR \
   (FS_RMTS_PM_SUPERBLOCK_START_SECTOR + FS_RMTS_PM_SUPERBLOCK_NUM_SECTORS)

#define FS_RMTS_PM_TAR_START_SECTOR         \
   (FS_RMTS_PM_IMG_HEADER_START_SECTOR + FS_RMTS_PM_IMG_HEADER_NUM_SECTORS)

#define FS_RMTS_PM_NUM_RESERVED_SECTORS     \
   (FS_RMTS_PM_IMG_HEADER_NUM_SECTORS + FS_RMTS_PM_SUPERBLOCK_NUM_SECTORS)

/* Mininum size in bytes of the write-cache region we want to support. Having
   a minimum makes the computation of various parts of the write-cache simple
   and ensures that the write-cache is being configured for a useful size.
   The mininum size will be bytes needed to hold at least one cluster and
   one sector that is needed to store the write-cache header and cluster tbl */
#define FS_RMTS_PM_MIN_WRITE_CACHE_SIZE (FS_MAX_PAGE_SIZE+FS_RMTS_SECTOR_SIZE)

/* Each entry in the Write Cache cluster table is as a uint32 value. */
#define FS_RMTS_CLUSTER_TBL_ENTRY_SIZE (sizeof (uint32))

#define FS_RMTS_PM_WRITE_CACHE_HDR_VER 1

/* Can help in debugging issues involving data corruption. */
#define FS_RMTS_PM_DEFAULT_WRITE_CACHE_PATTERN 0xAA
#define FS_RMTS_PM_DEFAULT_DATA_PATTERN        0x55


#define FS_RMTS_VALIDATE_CLUSTER(c) \
        ASSERT ( (c != 0) && (c <= pm->avail_clusters) )

/** Instance of the PM. */
static struct fs_rmts_pm rmts_pm_inst;

/* Declare space to hold a superblock. */
static uint8 shared_superblk_buf [FS_RMTS_SUPERBLOCK_SIZE];
static struct fs_rmts_superblock *shared_superblk;

/*--------------------   Prototypes   ---------------------------------------*/
static void fs_rmts_pm_init_ops (fs_pm_t pm_data);
static void fs_rmts_fresh_start (struct fs_rmts_pm *pm);
static void fs_rmts_pm_init_write_cache_region (struct fs_rmts_pm *pm,
                                                struct fs_rmts_superblock *sb);
static void fs_rmts_pm_init_data_region (struct fs_rmts_pm *pm,
                                         struct fs_rmts_superblock *sb);

static void fs_rmts_load_active_efs_copy (struct fs_rmts_pm *pm);
static void fs_rmts_pm_hookup_ram_addr (struct fs_rmts_pm_efs_copy *efs_copy,
                                        struct fs_rmts_superblock *src_sb);

static int fs_rmts_pm_read_superblock (struct fs_rmts_pm *pm,
                                    struct fs_rmts_pm_efs_copy *dest_efs_copy,
                                    enum fs_rmts_partition_id which_partition);

static void fs_rmts_pm_try_golden_copy (struct fs_rmts_pm *pm);
static void fs_rmts_boot_out_of_fsg (struct fs_rmts_pm *pm);
static int fs_rmts_pm_bootup_action_notify (struct fs_rmts_pm *pm);

static enum fs_rmts_partition_id
   get_next_partition (enum fs_rmts_partition_id current_partition);


#ifdef FEATURE_FS_TAR
  static void fs_rmts_pm_copy_tar_from_modem_to_shared (struct fs_rmts_pm *pm,
                          uint32 src_offset, uint32 dest_offset, uint32 size);
  static void fs_rmts_pm_untar_from_shared_ram (struct fs_rmts_pm *pm,
                                                uint32 start_offset);

#ifdef FEATURE_FS_ZIP_TAR
  static void fs_rmts_copy_gzip_from_modem_to_shared (struct fs_rmts_pm *pm,
                        uint32 src_offset, uint32 dest_offset, uint32 size);
  static void fs_rmts_pm_unzip_from_shared_ram (struct fs_rmts_pm *pm,
                                                uint32 start_offset);
#endif /* end feature FEATURE_FS_ZIP_TAR */
  /* While untaring TAR-file this much memory will be malloced. */
  uint32 fs_rmts_pm_untar_malloc_size = FS_RMTS_UNTAR_MALLOC_SIZE;
#endif /* FEATURE_FS_TAR */

#ifdef FS_UNIT_TEST
  /* Used by unit-tests to figure out where different regions start to
     modify partition data from the test. */
  uint32 rmts_ut_superblock_offset, rmts_ut_tar_offset;
  uint32 rmts_ut_write_cache_region_offset, rmts_ut_data_region_offset;

  /* Set this to 1 from unit-test before invoking efs_initialize to force a
     size for write-cache on fresh start . This sets the cache size to
     value of rmts_ut_write_cache_reg_size. */
  int rmts_ut_defines_cache_size = 0;
  uint32 rmts_ut_write_cache_reg_size;
#endif


static void
fs_rmts_pm_init_debug_fields (struct fs_rmts_pm *pm)
{
  pm->debug_info.fs1_decrypt_age_at_boot = 0xDEADCAFE;
  pm->debug_info.fs2_decrypt_age_at_boot = 0xDEADCAFE;
  pm->debug_info.fsg_decrypt_age_at_boot = 0xDEADCAFE;
}

static void
fs_rmts_pm_compute_sync_iovec_sizes (struct fs_rmts_pm *pm)
{
  struct fs_rmts_superblock *sb = pm->modem_copy.superblock;
  pm->write_cache_sync_size_bytes =
                                ((sb->write_cache_region.start_sector -
                                  FS_RMTS_PM_IMG_HEADER_START_SECTOR) *
                                  sb->sector_size) +
                                  pm->write_cache_region_size_bytes;
  pm->full_sync_size_bytes =
                            ((sb->data_region.start_sector -
                              FS_RMTS_PM_IMG_HEADER_START_SECTOR) *
                              sb->sector_size) + pm->data_region_size_bytes;
}

fs_pm_t
fs_rmts_pm_init (efs_device_t dev)
{
  struct fs_rmts_pm *pm;
  uint32 offset;
  uint8 *ram_addr, *img_hdr_ram;
  uint32 img_hdr_ram_size, image_size;
  struct fs_rmts_superblock *active_sb;
  enum fs_rmts_device_errno result;
  int rmts_error;

  /* Make sure super-block fits in one sector. */
  ASSERT (FS_RMTS_SUPERBLOCK_SIZE == FS_RMTS_SECTOR_SIZE);

  memset (&rmts_pm_inst, 0, sizeof (rmts_pm_inst));
  pm = &rmts_pm_inst;
  pm->dev = dev;

  fs_rmts_pm_init_debug_fields (pm);
  fs_sync_init (&pm->sync_data);
  fs_sync_set_sync_timer_interval (&pm->sync_data, FS_SYNC_TIMER);

  /* Setup the PM's RAM addreses to read in superblocks. */
  pm->ram_size = pm->dev->priv.rmts_dev.ram_size;
  offset = FS_RMTS_PM_SUPERBLOCK_START_SECTOR * FS_RMTS_SECTOR_SIZE;

  ram_addr = pm->dev->priv.rmts_dev.modem_ram;
  pm->modem_copy.ram_addr = ram_addr;
  pm->modem_copy.superblock = (struct fs_rmts_superblock*)(ram_addr + offset);

  ram_addr = pm->dev->priv.rmts_dev.shared_ram;
  pm->shared_copy.ram_addr = ram_addr;
  pm->shared_copy.superblock = (struct fs_rmts_superblock*)(ram_addr + offset);

  ram_addr = pm->dev->priv.rmts_dev.golden_ram;
  pm->golden_copy.ram_addr = ram_addr;
  if (pm->golden_copy.ram_addr != NULL)
  {
    pm->golden_copy.superblock =
                               (struct fs_rmts_superblock*)(ram_addr + offset);
  }

  pm->modem_type = pm->dev->rmts_get_rmts_modem_type (pm->dev);

#ifdef FEATURE_EFS_RMTS_WRITE_TO_FSG_FOR_GOLDEN_COPY
  pm->write_to_fsg_for_golden_copy = 1;
#endif

  result = pm->dev->rmts_client_init (pm->dev, FS_OP_COMPLETE_SIG,
                                      FS_RMTS_IOVEC_TIMEOUT);

  if (result != FS_RMTS_ERRNO_SUCCESS)
  {
    rmts_error = pm->dev->rmts_get_last_errno (pm->dev);
    FS_ERR_FATAL ("[%d, %d] rmts_client_init failed", result, rmts_error, 0);
  }

  pm->booted_out_of_efs_partition = FS_RMTS_PARTITION_ID_MAX;

  /* If someone with a debugger is requesting a fresh start, do so. */
  if (fs_fresh_start)
  {
    fs_rmts_fresh_start (pm);
  }
  else
  {
    fs_rmts_load_active_efs_copy (pm);
  }

  result = fs_rmts_pm_bootup_action_notify (pm);
  if (result != 0)
  {
    pm->is_oem_forced_fresh_start = 1;
    fs_rmts_fresh_start (pm);

    /* We did a forced-fresh-start of EFS on OEM's request. But let's not reset
     * and lose age values of the EFS-images. Let's continue with the age of
     * the latest EFS-image. Since the next sync in always on FS1, it is safe
     * to use FS2's age as the latest age. */
    if (pm->fs2_bootup_action_image_info.was_sb_decrypt_successful)
    {
      pm->modem_copy.superblock->age = pm->fs2_bootup_action_image_info.age;
    }
    else
    {
      /* FS2's SuperBlock did not decrypt, so lets use FS1's previoius-age
       * as the latest-age, it will incremented by 1 on the next sync. */
      if (pm->fs1_bootup_action_image_info.was_sb_decrypt_successful)
      {
        pm->modem_copy.superblock->age =
                                   pm->fs1_bootup_action_image_info.age - 1;
      }
    }
  }

  /* Now the modem-copy will have the active EFS. */
  active_sb = pm->modem_copy.superblock;
  pm->max_sectors = active_sb->sector_cnt;
  pm->page_size = fs_device_page_size (pm->dev);

  fs_rmts_verify_build_with_superblock (dev, active_sb);
  fs_rmts_pm_init_ops (&pm->parent);

  fs_rmts_pm_init_write_cache_region (pm, active_sb);
  fs_rmts_pm_init_data_region (pm, active_sb);
  fs_rmts_pm_compute_sync_iovec_sizes (pm);

  img_hdr_ram = (pm->shared_copy.ram_addr +
                (FS_RMTS_PM_IMG_HEADER_START_SECTOR * active_sb->sector_size));
  img_hdr_ram_size = FS_RMTS_PM_IMG_HEADER_NUM_SECTORS *active_sb->sector_size;
  image_size = pm->ram_size - img_hdr_ram_size;
  pm->image_header = fs_rmts_image_header_create (img_hdr_ram,
                                                  img_hdr_ram_size,
                                                  image_size);

#ifdef FS_UNIT_TEST
  /* Fill up the Unit-test offsets and sizes for tests to verify/manipulate */
  rmts_ut_superblock_offset = FS_RMTS_PM_SUPERBLOCK_START_SECTOR *
                              active_sb->sector_size;

  rmts_ut_write_cache_region_offset =
         active_sb->write_cache_region.start_sector * active_sb->sector_size;
  rmts_ut_write_cache_reg_size =
         active_sb->write_cache_region.num_sectors * active_sb->sector_size;

  rmts_ut_data_region_offset = active_sb->data_region.start_sector *
                               active_sb->sector_size;

  rmts_ut_tar_offset = FS_RMTS_PM_TAR_START_SECTOR * active_sb->sector_size;
#endif

  return &pm->parent;
}

/* Hook up RAM address for all regions of the requested efs_copy based on
   supplied superblock region fields. */
static void
fs_rmts_pm_hookup_ram_addr (struct fs_rmts_pm_efs_copy *efs_copy,
                             struct fs_rmts_superblock *sb)
{
  uint32 offset;
  uint8 *ram_addr;

  ram_addr = efs_copy->ram_addr;
  offset = sb->write_cache_region.start_sector * sb->sector_size;
  efs_copy->write_cache_region = ram_addr + offset;

  offset = sb->data_region.start_sector * sb->sector_size;
  efs_copy->data_region = ram_addr + offset;
}

static void
fs_rmts_pm_reset_write_cache_data (struct fs_rmts_pm *pm)
{
  uint32 i;
  struct fs_rmts_write_cache_header *hdr = pm->write_cache_hdr;

  ASSERT (pm->is_write_cache_present == 1);

  for (i = 0; i < hdr->write_cache_ctable_num_entries; i++)
  {
    pm->write_cache_cluster_table[i] = FS_INVALID_PAGE_ID;
  }

  /* Fill with non-zero pattern */
  memset (pm->write_cache, FS_RMTS_PM_DEFAULT_WRITE_CACHE_PATTERN,
          pm->write_cache_size_bytes);
}

static void
fs_rmts_pm_handle_write_cache_overflow (struct fs_rmts_pm *pm)
{
  uint32 i;
  struct fs_rmts_write_cache_header *hdr = pm->write_cache_hdr;

  /* Sanity check. */
  for (i = 0; i < hdr->write_cache_ctable_num_entries; i++)
  {
    FS_RMTS_VALIDATE_CLUSTER (pm->write_cache_cluster_table[i]);
  }

  fs_rmts_pm_reset_write_cache_data (pm);

  pm->did_write_cache_overflow = 1;
}

/* Called on Fresh start. Allocates required sectors for Wirte-Cache region and
   divides the region into sectors containing the Header and Cluster table and
   the actual cached clusters. */
static void
fs_rmts_pm_create_write_cache_region (struct fs_rmts_pm *pm,
                                      struct fs_rmts_superblock *sb)
{
  uint32 device_size, write_cache_reg_size_bytes, write_cache_reg_size_sectors;
  uint8 *write_cache_reg_start_addr, *write_cache;
  uint32 write_cache_offset_bytes, write_cache_num_entries;
  uint32 write_cache_size_bytes;
  struct fs_rmts_write_cache_header *hdr;
  uint32 *write_cache_cluster_table, i;

  write_cache_reg_size_bytes = 0;

  /* Enable Write-Cache for bigger EFS */
  device_size = sb->sector_cnt * sb->sector_size;
  if (device_size > FS_RMTS_MIN_RAM_SIZE_BYTES_TO_ENABLE_CACHE)
    write_cache_reg_size_bytes = FS_RMTS_PM_WRITE_CACHE_SIZE_BYTES;

  /* If set then use the configured value  */
#ifdef EFS_RMTS_WRITE_CACHE_SIZE_BYTES
  write_cache_reg_size_bytes = EFS_RMTS_WRITE_CACHE_SIZE_BYTES;
#endif

  /* If specified, disable write-cache */
#ifdef EFS_RMTS_DISABLE_WRITE_CACHE
  write_cache_reg_size_bytes = 0;
#endif

#ifdef FS_UNIT_TEST
  /* If unit-test is specifying the size, use it. */
  if (rmts_ut_defines_cache_size)
    write_cache_reg_size_bytes = rmts_ut_write_cache_reg_size;
#endif

  sb->write_cache_region.start_sector = FS_RMTS_PM_WRITE_CACHE_START_SECTOR;
  sb->write_cache_region.num_sectors = 0;

  /* If no cache region configured, nothing more to do. */
  if (write_cache_reg_size_bytes == 0)
    return;

  if (write_cache_reg_size_bytes < FS_RMTS_PM_MIN_WRITE_CACHE_SIZE)
  {
    FS_ERR_FATAL ("%d,%d,%d, Invalid write cache size config",
             write_cache_reg_size_bytes, FS_RMTS_PM_MIN_WRITE_CACHE_SIZE, 0);
  }

  /* Calculate write-cache region size at sector boundary. */
  write_cache_reg_size_sectors = write_cache_reg_size_bytes / sb->sector_size;
  write_cache_reg_size_bytes = write_cache_reg_size_sectors * sb->sector_size;

  /* Sanity check : Make sure write-cache is not more than 1/2 of EFS. */
  sb->write_cache_region.num_sectors = write_cache_reg_size_sectors;
  if (sb->write_cache_region.num_sectors >= (sb->sector_cnt / 2))
  {
    FS_ERR_FATAL ("%d,%d,%d, Invalid write cache size config",sb->sector_cnt,
                   sb->write_cache_region.num_sectors, 0);
  }

  /* Carve out space for Header, Cluster table and Write-cache clusters. */
  write_cache_num_entries =
     (write_cache_reg_size_bytes - FS_RMTS_PM_WRITE_CACHE_HDR_SIZE) /
     (FS_RMTS_CLUSTER_TBL_ENTRY_SIZE + sb->cluster_size);

  ASSERT (write_cache_num_entries > 1);

  write_cache_offset_bytes = FS_RMTS_PM_WRITE_CACHE_HDR_SIZE +
                    (write_cache_num_entries * FS_RMTS_CLUSTER_TBL_ENTRY_SIZE);

  write_cache_reg_start_addr = pm->modem_copy.ram_addr +
                      (sb->write_cache_region.start_sector * sb->sector_size);
  memset (write_cache_reg_start_addr, 0x0, write_cache_reg_size_bytes);

  /* Initialize the header with offset and size of the cache */
  hdr = (struct fs_rmts_write_cache_header *) write_cache_reg_start_addr;
  hdr->version = FS_RMTS_PM_WRITE_CACHE_HDR_VER;
  hdr->write_cache_ctable_byte_offset = FS_RMTS_PM_WRITE_CACHE_HDR_SIZE;
  hdr->write_cache_ctable_num_entries = write_cache_num_entries;
  hdr->write_cache_byte_offset = write_cache_offset_bytes;

  write_cache_cluster_table  = (uint32 *) (write_cache_reg_start_addr +
                                          hdr->write_cache_ctable_byte_offset);

  for (i = 0; i < hdr->write_cache_ctable_num_entries; i++)
    write_cache_cluster_table[i] = FS_INVALID_PAGE_ID;

  write_cache_size_bytes = write_cache_reg_size_bytes -
                           hdr->write_cache_byte_offset;
  write_cache = ((uint8 *)hdr) + hdr->write_cache_byte_offset;
  /* Fill with non-zero pattern */
  memset (write_cache, FS_RMTS_PM_DEFAULT_WRITE_CACHE_PATTERN,
          write_cache_size_bytes);
}

/* Set the sectors remaining for data region as available to buffer layer */
static void
fs_rmts_pm_create_data_region (struct fs_rmts_pm *pm,
                               struct fs_rmts_superblock *sb)
{
  uint32 avail_data_sectors, data_region_size;
  uint8 *data_reg_start_addr;

  (void) pm;

  /* Remove reserved sectors and write_cache region sectors from available
     sectors to calculate data region sectors. */
  avail_data_sectors = sb->sector_cnt - FS_RMTS_PM_NUM_RESERVED_SECTORS;
  avail_data_sectors -= sb->write_cache_region.num_sectors;

  sb->data_region.start_sector = sb->write_cache_region.start_sector +
                                 sb->write_cache_region.num_sectors;
  sb->data_region.num_sectors = avail_data_sectors;
  data_region_size = sb->data_region.num_sectors * sb->sector_size;
  data_reg_start_addr = pm->modem_copy.ram_addr +
                        (sb->data_region.start_sector * sb->sector_size);
  /* Fill with non-zero pattern */
  memset (data_reg_start_addr, FS_RMTS_PM_DEFAULT_DATA_PATTERN,
          data_region_size);
}


static void
fs_rmts_fresh_start (struct fs_rmts_pm *pm)
{
  pm->fresh_start = 1;

  memset (pm->modem_copy.ram_addr, 0, pm->ram_size);

  /* Create a new superblock with all fields of the superblock initalized. */
  fs_rmts_super_create_superblock (pm->modem_copy.superblock, pm->dev);

  /* Allocate space for write-cache and data regions. */
  fs_rmts_pm_create_write_cache_region (pm, pm->modem_copy.superblock);
  fs_rmts_pm_create_data_region (pm, pm->modem_copy.superblock);

  /* Hook up PM's RAM addresses for the regions created in both efs copies. */
  fs_rmts_pm_hookup_ram_addr (&pm->modem_copy, pm->modem_copy.superblock);
  fs_rmts_pm_hookup_ram_addr (&pm->shared_copy, pm->modem_copy.superblock);

  /* Set active partition to FS2, so the next efs_sync() will write to FS1. */
  pm->active_efs_partition = FS_RMTS_PARTITION_ID_FS2;
}


/* Encrypt and authenticate memory region */
static void
authenticate_encrypt_memory (void *dest, void *src, uint32 len, void *hash)
{
  IxErrnoType result;
  /* On the upper layers we make sure the destination buffer has len bytes
   * to store the encrytped data */
  uint32 out_len = len;
  /* For this module we always use the same hash size */
  uint32 hash_len = FS_RMTS_META_DATA_SIZE;

  result = secapi_util_encrypt_authenticate (src, len, dest, &out_len,
                                             hash, hash_len);
  if (result != E_SUCCESS)
    FS_ERR_FATAL ("encrypt error: %d", result, 0, 0);

  if (out_len != len)
    FS_ERR_FATAL ("encrypt error, %d(in) != %d(out)", len, out_len, 0);

}


/* Authenticate and decrypt memory region. Returns 0 on success, -1 on error */
static int
authenticate_decrypt_memory (void *dest, void *src, uint32 len, void *hash)
{
  IxErrnoType result;
  /* On the upper layers we make sure the destination buffer has len bytes
   * to store the encrytped data */
  uint32 out_len = len;
  /* For this module we always use the same hash size */
  uint32 hash_len = FS_RMTS_META_DATA_SIZE;

  result = secapi_util_decrypt_authenticate (src, len, dest, &out_len,
                                             hash, hash_len);

  if (out_len != len)
    FS_ERR_FATAL ("decrypt error, %d(in) != %d(out)", len, out_len, 0);

  if (result != E_SUCCESS)
  {
    FS_MSG_MED_1 ("[%d] secapi authenticate failure", result);
    return -1;
  }

  return 0;
}

/* Encrypts the modem copy into the shared copy. It will error fatal if it
   fails to encrypt. */
static void
fs_rmts_encrypt_efs_copy (struct fs_rmts_pm *pm)
{
  void *src, *dest;
  uint32 len;
  void *meta_data;
  struct fs_rmts_superblock *modem_sb = pm->modem_copy.superblock;

  /* Clear out shared superblock. */
  memset (pm->shared_copy.superblock, 0x0, FS_RMTS_SUPERBLOCK_SIZE);

  /* Encrypt only if data is dirty and not previously encrypted. */
  if (pm->is_curr_sync_full_sync)
  {
    src = pm->modem_copy.data_region;
    dest = pm->shared_copy.data_region;
    len = pm->data_region_size_bytes;
    modem_sb->data_region.encrypted_data_size_bytes = len;
    meta_data = modem_sb->data_region.encryption_meta_data;
    authenticate_encrypt_memory (dest, src, len, meta_data);
    switch (modem_sb->which_partition)
    {
      case FS_RMTS_PARTITION_ID_FS1:
        fs_memscpy (pm->fs1_meta_data, sizeof (pm->fs1_meta_data), meta_data,
                    sizeof (pm->fs1_meta_data));
        break;
      case FS_RMTS_PARTITION_ID_FS2:
        fs_memscpy (pm->fs2_meta_data, sizeof (pm->fs2_meta_data), meta_data,
                    sizeof (pm->fs2_meta_data));
        break;
      case FS_RMTS_PARTITION_ID_FSG:
        break; /* If encrypting for FSG creation, no need to backup, there
                  shouldnt be any more syncs. */
      default:
        FS_ERR_FATAL ("%d, Invalid partition being encrypted [parti]",
                      modem_sb->which_partition, 0, 0);
        break;
    }
  }
  else
  {
    switch (modem_sb->which_partition)
    {
      case FS_RMTS_PARTITION_ID_FS1:
        fs_memscpy (modem_sb->data_region.encryption_meta_data,
                    sizeof (modem_sb->data_region.encryption_meta_data),
                    pm->fs1_meta_data,
                    sizeof (modem_sb->data_region.encryption_meta_data));
        break;
      case FS_RMTS_PARTITION_ID_FS2:
        fs_memscpy (modem_sb->data_region.encryption_meta_data,
                    sizeof (modem_sb->data_region.encryption_meta_data),
                    pm->fs2_meta_data,
                    sizeof (modem_sb->data_region.encryption_meta_data));
        break;
      case FS_RMTS_PARTITION_ID_FSG:
      default:
        FS_ERR_FATAL ("%d, Invalid partition being encrypted [parti]",
                      modem_sb->which_partition, 0, 0);
        break;
    }
  }

  /* Encrypt Write-Cache region. */
  if (pm->is_write_cache_present)
  {
    src = pm->modem_copy.write_cache_region;
    dest = pm->shared_copy.write_cache_region;
    len = pm->write_cache_region_size_bytes;
    modem_sb->write_cache_region.encrypted_data_size_bytes = len;
    meta_data = modem_sb->write_cache_region.encryption_meta_data;
    authenticate_encrypt_memory (dest, src, len, meta_data);
  }

  /* Encrypt superblock. */
  len = FS_RMTS_SUPERBLOCK_SIZE - FS_RMTS_META_DATA_SIZE;
  meta_data = pm->shared_copy.superblock->super_meta_data;
  authenticate_encrypt_memory (pm->shared_copy.superblock,
                               pm->modem_copy.superblock, len, meta_data);
}


static void
fs_rmts_pm_read_iovec (struct fs_rmts_pm *pm,
                       struct fs_iovec *iovec, uint32 iovec_cnt,
                       enum fs_rmts_partition_id which_partition,
                       uint8 *dest_addr)
{
  enum fs_rmts_device_errno result;
  int rmts_error = -1;

  result = pm->dev->rmts_read_iovec (pm->dev, iovec, iovec_cnt,
                                     which_partition);
  switch (pm->modem_type)
  {
    case FS_RMTS_MODEM_TYPE_STANDALONE:
    {
      if (result == FS_RMTS_ERRNO_SUCCESS)
      {
        if (iovec->buff_base != dest_addr)
        {
          fs_memscpy (dest_addr, iovec->size, iovec->buff_base, iovec->size);
        }

        rmts_error = 0;
      }

      break;
    }

    case FS_RMTS_MODEM_TYPE_FUSION:
    {
      if (result == FS_RMTS_ERRNO_NOT_SUPPORTED)
      {
        rmts_error = 0;
      }

      break;
    }

    default:
    {
      FS_ERR_FATAL ("[%d] Invalid rmts modem type ", pm->modem_type, 0, 0);
      break;
    }
  }

  if (rmts_error != 0)
  {
    rmts_error = pm->dev->rmts_get_last_errno (pm->dev);
    FS_ERR_FATAL ("[%d, %d] rmts_read_iovec failed", result, rmts_error, 0);
  }
}


/* Reads the requested sectors of the partition into the shared RAM address and
   copies over to the destination efs_copy if different from shared RAM. */
static int
fs_rmts_pm_read_sectors (struct fs_rmts_pm *pm,
                         enum fs_rmts_partition_id efs_partition,
                         uint32 start_sector, uint32 num_sectors,
                         uint32 sector_size, uint8 *dest_addr)
{
  struct fs_iovec iovec;
  uint32 ram_size;
  uint8 *shared_ram_addr;

  shared_ram_addr = pm->shared_copy.ram_addr + (start_sector * sector_size);
  ram_size = num_sectors * sector_size;

  iovec.sector_id = start_sector;
  iovec.size = ram_size;
  iovec.buff_base = shared_ram_addr;

  fs_rmts_pm_read_iovec (pm, &iovec, 1, efs_partition, dest_addr);

  return 0;
}


/* Reads the superblock region for the partition into the superblock region of
   requested efs copy. */
static int
fs_rmts_pm_read_superblock (struct fs_rmts_pm *pm,
                            struct fs_rmts_pm_efs_copy *dest_efs_copy,
                            enum fs_rmts_partition_id which_partition)
{
  uint8 *dest_addr;
  uint32 start_sector, num_sectors;
  int result;
  dest_addr = (uint8 *)dest_efs_copy->superblock;
  start_sector = FS_RMTS_PM_SUPERBLOCK_START_SECTOR;
  num_sectors = FS_RMTS_PM_SUPERBLOCK_NUM_SECTORS;

  result = fs_rmts_pm_read_sectors (pm, which_partition, start_sector,
                         num_sectors, FS_RMTS_SECTOR_SIZE, dest_addr);
  return result;
}

/* Reads the Write-Cache and Data regions for the partition into the respective
   regions of requested destination efs copy. */
static int
fs_rmts_pm_read_regions (struct fs_rmts_pm *pm, struct fs_rmts_superblock *sb,
                         struct fs_rmts_pm_efs_copy *dest_efs_copy,
                         enum fs_rmts_partition_id which_partition)
{
  uint8 *dest_addr;
  uint32 start_sector, end_sector, num_sectors;
  int result;

  dest_addr = dest_efs_copy->write_cache_region;
  start_sector = sb->write_cache_region.start_sector;
  end_sector = sb->data_region.start_sector + sb->data_region.num_sectors;
  num_sectors = end_sector - start_sector;

  result = fs_rmts_pm_read_sectors (pm, which_partition, start_sector,
                             num_sectors, sb->sector_size, dest_addr);
  return result;
}

/* Check for image layout validity by verifying offsets do not overlap or do
   not overflow the available memory. Returns 0 on success and -1 on error. */
static int
fs_rmts_pm_is_image_layout_valid (struct  fs_rmts_superblock *sb,
                                  struct fs_rmts_pm *pm)
{
  if (pm->ram_size < (sb->sector_size * sb->sector_cnt))
    return -1;

  /* Ensure Wirte-cache region doesnt overflow into data region. */
  if ((sb->write_cache_region.start_sector +
       sb->write_cache_region.num_sectors) > sb->data_region.start_sector)
    return -1;

  /* Ensure data region doesn't overflow available memory sectors. */
  if ((sb->data_region.start_sector + sb->data_region.num_sectors) >
      (pm->ram_size / sb->sector_size))
    return -1;

  return 0;
}

/* Checks if the data was expected to be written to/read from the specified
   partiton */
static void
fs_rmts_pm_validate_superblock_for_partition (struct fs_rmts_superblock *sb,
                                  enum fs_rmts_partition_id which_partition)
{
  switch (which_partition)
  {
    case FS_RMTS_PARTITION_ID_FS1:
    case FS_RMTS_PARTITION_ID_FS2:
      if (sb->which_partition != which_partition || sb->i_am_golden == 1)
      {
        FS_ERR_FATAL ("%d,%d,%d,Partition data not meant for this partition",
                      sb->which_partition, which_partition, sb->i_am_golden);
      }
      break;

    case FS_RMTS_PARTITION_ID_FSG:
      if (sb->which_partition != which_partition || sb->i_am_golden != 1)
      {
        FS_ERR_FATAL ("%d,%d,%d,Non Golden-copy data in FSG",
                      sb->which_partition, which_partition, sb->i_am_golden);
      }
      break;

    default:
      FS_ERR_FATAL ("%d,Unexpected partition", sb->age, which_partition, 0);
      break;
  }
}

/* Decrypt superblock and check for image compatibility. src_sb is encrypted
   superblock, dest_sb is destination buff for decrypted validated superbock.
   Returns 0 on successful decrypt validate and -1 on failure. */
static int
fs_rmts_decrypt_validate_superblock (struct fs_rmts_superblock *src_sb,
                                     struct fs_rmts_superblock *dest_sb,
                                     struct fs_rmts_pm *pm,
                                     enum fs_rmts_partition_id which_partition)
{
  void *hash;
  int result;
  uint32 len;

  len = FS_RMTS_SUPERBLOCK_SIZE - FS_RMTS_META_DATA_SIZE;
  hash = src_sb->super_meta_data;

  result = authenticate_decrypt_memory (dest_sb, src_sb, len, hash);
  if (result == 0)
  {
    result = fs_rmts_validate_superblock (dest_sb);

    if (result == 0)
    {
      fs_rmts_pm_validate_superblock_for_partition (dest_sb, which_partition);
      result = fs_rmts_pm_is_image_layout_valid (dest_sb, pm);
    }
  }

  return result;
}


static int
fs_rmts_pm_decrypt_write_cache_region (struct fs_rmts_superblock *sb,
                                       struct fs_rmts_pm_efs_copy *src_copy,
                                       struct fs_rmts_pm_efs_copy *dest_copy)
{
  uint8 *dest_addr, *src_addr;
  uint32 region_size;

  dest_addr = dest_copy->write_cache_region;
  src_addr = src_copy->write_cache_region;
  region_size = sb->write_cache_region.num_sectors * sb->sector_size;

  if(region_size > 0)
  {
    return authenticate_decrypt_memory (dest_addr, src_addr, region_size,
                             sb->write_cache_region.encryption_meta_data);
  }

  return 0;
}


static int
fs_rmts_pm_decrypt_data_region (struct fs_rmts_superblock *sb,
                                struct fs_rmts_pm_efs_copy *src_copy,
                                struct fs_rmts_pm_efs_copy *dest_copy)
{
  uint8 *dest_addr, *src_addr;
  uint32 region_size;

  dest_addr = dest_copy->data_region;
  src_addr = src_copy->data_region;
  region_size = sb->data_region.num_sectors * sb->sector_size;

  return authenticate_decrypt_memory (dest_addr, src_addr, region_size,
                                 sb->data_region.encryption_meta_data);
}

/* Based on the active efs, sets up the RAM addresses based on the superblock
   fields to read the regions and decrypt them into Modem RAM location.
   read-hint specifies if data has already been read, like in the case of
   decrypting FSG. Since FSG data can be a TAR file, we would have already
   read the whole partition data & doesn't require reading before
   decryption. */
static int
fs_rmts_decrypt_active_efs_copy (enum fs_rmts_partition_id which_partition,
                                 int read_hint, struct fs_rmts_pm *pm,
                                 struct fs_rmts_superblock *superblock,
                                 uint8 *wc_decrypt_failed,
                                 uint8 *dr_decrypt_failed)
{
   struct fs_rmts_pm_efs_copy *read_dest_copy;
   struct fs_rmts_pm_efs_copy *decrypt_dest_copy = NULL;
   int result;

   if (pm == NULL || superblock == NULL)
   {
     return -1;
   }

   *wc_decrypt_failed = *dr_decrypt_failed = 1;

   /* Hook up RAM addresses using data in superblock */
   fs_rmts_pm_hookup_ram_addr (&pm->modem_copy, superblock);
   fs_rmts_pm_hookup_ram_addr (&pm->shared_copy, superblock);

   /* Set up source and destination efs copies for reading and decryption
      based on the active partition and based on whether it is standalone or
      fusion.*/
   switch (which_partition)
   {
     case FS_RMTS_PARTITION_ID_FS1:
        /* FS1 is read into Modem copy and decrypted in place */
        read_dest_copy = &pm->modem_copy;
        decrypt_dest_copy = &pm->modem_copy;
        break;

     case FS_RMTS_PARTITION_ID_FS2:
       /* FS2 is read into Shared RAM and decrypted into Modem RAM in
          Standalone. On Fusion, all the 3 partitions' data are read by boot
          and cannot be read in AMSS. So decrypt FS2 data in-place into
          shared RAM since a failed attempt to authenticate-decrypt into the
          Modem RAM will leave the destination data changed and in this case,
          will end up corrupting FS1 data stored in Modem RAM. */
        read_dest_copy = &pm->shared_copy;
        switch (pm->modem_type)
        {
          case FS_RMTS_MODEM_TYPE_STANDALONE:
          {
            decrypt_dest_copy = &pm->modem_copy;
            break;
          }

          case FS_RMTS_MODEM_TYPE_FUSION:
          {
            decrypt_dest_copy = &pm->shared_copy;
            break;
          }

          default:
          {
            FS_ERR_FATAL ("[%d] Invalid rmts modem type ", pm->modem_type,0,0);
            break;
          }
        }
        break;

     default:
       FS_ERR_FATAL ("%d, Parti should not be read/decrypted", which_partition,
                     0, 0);
       return -1;
       break;
   }

   if (read_hint == 1)
     (void) fs_rmts_pm_read_regions (pm, superblock, read_dest_copy,
                                     which_partition);

   result = fs_rmts_pm_decrypt_write_cache_region (superblock, read_dest_copy,
                                                   decrypt_dest_copy);
   if (result != 0)
     return result;

   *wc_decrypt_failed = 0;

   result = fs_rmts_pm_decrypt_data_region (superblock, read_dest_copy,
                                            decrypt_dest_copy);
   if (result != 0)
     return result;

   *dr_decrypt_failed = 0;

  /* If FS2 is active, copy the superblock to modem copy. If fusion, copy
     superblock and the rest of the regions from shared copy to modem. */
  if (which_partition == FS_RMTS_PARTITION_ID_FS2)
  {
    if (pm->modem_type == FS_RMTS_MODEM_TYPE_FUSION)
      fs_memscpy (pm->modem_copy.ram_addr, pm->ram_size,
                  pm->shared_copy.ram_addr, pm->ram_size);

    fs_memscpy (pm->modem_copy.superblock, FS_RMTS_SUPERBLOCK_SIZE,
                shared_superblk, FS_RMTS_SUPERBLOCK_SIZE);
  }

  /* Set the partition with the sane copy as active. */
  pm->active_efs_partition = which_partition;

  return 0;
}

/* Initialization routine that reads superblock to figure out latest copy
   and loads the latest efs copy into Modem RAM. */
static void
fs_rmts_load_active_efs_copy (struct fs_rmts_pm *pm)
{
  struct fs_rmts_superblock *modem_superblk, *superblock;
  int active_efs; /* 0 -> Modem and 1 -> Shared */
  int fs1_sb_res, fs2_sb_res, age_in_seq, num_valid_superblocks;
  int validation_res = -1;
  enum fs_rmts_partition_id active_partition;
  uint32 fs1_age = 0; /* keep lint from complaining */
  uint8 wc_decrypt_failed, dr_decrypt_failed;


  /* Try and read the FS1 and FS2 superblocks. */
  fs1_sb_res = fs_rmts_pm_read_superblock (pm, &pm->modem_copy,
                                           FS_RMTS_PARTITION_ID_FS1);
  fs2_sb_res = fs_rmts_pm_read_superblock (pm, &pm->shared_copy,
                                           FS_RMTS_PARTITION_ID_FS2);
  if (fs1_sb_res != fs2_sb_res)
  {
    /* Either we should read both FS1 and FS2 superblocks or none. */
    FS_ERR_FATAL ("Read supeblock failed (%d!=%d)", fs1_sb_res, fs2_sb_res, 0);
  }

  fs1_sb_res = fs_rmts_decrypt_validate_superblock (pm->modem_copy.superblock,
                      pm->modem_copy.superblock, pm, FS_RMTS_PARTITION_ID_FS1);

  fs2_sb_res = fs_rmts_decrypt_validate_superblock(pm->shared_copy.superblock,
                         (struct fs_rmts_superblock *)shared_superblk_buf, pm,
                          FS_RMTS_PARTITION_ID_FS2);

  pm->debug_info.fs1_sb_decrypt_failed = (fs1_sb_res != 0);
  pm->debug_info.fs2_sb_decrypt_failed = (fs2_sb_res != 0);

  if ((fs1_sb_res != 0) && (fs2_sb_res != 0))
  {
    fs_rmts_pm_try_golden_copy (pm);
    return;
  }

  modem_superblk = NULL;
  if (fs1_sb_res == 0)
  {
    modem_superblk  = pm->modem_copy.superblock;
    pm->debug_info.fs1_decrypt_age_at_boot = modem_superblk->age;
    fs_memscpy (pm->fs1_meta_data, sizeof (pm->fs1_meta_data),
                modem_superblk->data_region.encryption_meta_data,
                sizeof (pm->fs1_meta_data));

    pm->fs1_bootup_action_image_info.was_sb_decrypt_successful = 1;
    pm->fs1_bootup_action_image_info.age = modem_superblk->age;
    fs_memscpy (pm->fs1_bootup_action_image_info.oem_data,
                sizeof (pm->fs1_bootup_action_image_info.oem_data),
                modem_superblk->oem_data,
                sizeof (pm->fs1_bootup_action_image_info.oem_data));

  }

  shared_superblk = NULL;
  if (fs2_sb_res == 0)
  {
    shared_superblk = (struct fs_rmts_superblock *) shared_superblk_buf;
    pm->debug_info.fs2_decrypt_age_at_boot = shared_superblk->age;
    fs_memscpy (pm->fs2_meta_data, sizeof (pm->fs2_meta_data),
                shared_superblk->data_region.encryption_meta_data,
                sizeof (pm->fs2_meta_data));

    pm->fs2_bootup_action_image_info.was_sb_decrypt_successful = 1;
    pm->fs2_bootup_action_image_info.age = shared_superblk->age;
    fs_memscpy (pm->fs2_bootup_action_image_info.oem_data,
                sizeof (pm->fs2_bootup_action_image_info.oem_data),
                shared_superblk->oem_data,
                sizeof (pm->fs2_bootup_action_image_info.oem_data));
  }

  /* At least one superblock is sane */
  num_valid_superblocks = 1;

  age_in_seq = fs_rmts_super_validate_age (modem_superblk, shared_superblk);

  if (modem_superblk != NULL && shared_superblk != NULL)
  {
    /* Both copies' superblocks are good so get the latest. */
    num_valid_superblocks++;
    active_efs = fs_rmts_get_latest_superblock (modem_superblk,
                                                      shared_superblk);
    /* Backup FS1's age for later since successful decryption of FS2 will
       replace modem superblock and we might have to print the modem age for
       debugging.*/
    fs1_age = modem_superblk->age;
  }
  else
  {
    /* Only one of the copies' superblock is good. */
    active_efs = (modem_superblk != NULL) ? 0 : 1;
  }

  while (num_valid_superblocks-- > 0)
  {
    superblock = (active_efs == 0) ? modem_superblk : shared_superblk;
    active_partition = (active_efs == 0) ? FS_RMTS_PARTITION_ID_FS1 :
                                           FS_RMTS_PARTITION_ID_FS2;
    validation_res = fs_rmts_decrypt_active_efs_copy (active_partition, 1, pm,
                       superblock, &wc_decrypt_failed, &dr_decrypt_failed);

    if (active_partition == FS_RMTS_PARTITION_ID_FS1)
    {
      pm->debug_info.fs1_wc_decrypt_failed = (wc_decrypt_failed == 1);
      pm->debug_info.fs1_dr_decrypt_failed = (dr_decrypt_failed == 1);
    }
    else
    {
      pm->debug_info.fs2_wc_decrypt_failed = (wc_decrypt_failed == 1);
      pm->debug_info.fs2_dr_decrypt_failed = (dr_decrypt_failed == 1);
    }

    if (validation_res == 0)
    {
      /* We found the latest EFS-copy. Make sure superblock's ages are in
         sequence. FS1 and FS2 superblock's age will always grow in sequence
         except one corner case explained below:
           1. On phone's reset, we detect FS1 and FS2's superblocks are good,
              but both their data-regions are corrupt, then we do a fresh
              start of EFS.
           2. Now on first EFS-Sync, we write FS1 with superblock-age as 0.
              But FS2's superblock is still intact with a non-sequential-age
              from previous run.
           3. If the phone resets now before another sync, then on boot up we
              will be comparing FS1-superblock with age-0 with an older
              non-sequential FS2-superblock's age.
         If age is not in sequence, ensure FS2 did not decrypt correctly.

         The check of shared_superblk != NULL is redundant since the
         combination of validation_res == 0 && active_partition = FS2 &&
         shared_superblk == NULL will never happen. It is added to keep
         static analysis tools happy. */
      if (age_in_seq != 1 && active_partition == FS_RMTS_PARTITION_ID_FS2 &&
          shared_superblk != NULL)
      {
        /* FS2 decrypted fine but ages are not in seq. */
        FS_ERR_FATAL ("%d,%d,Super ages not sequential & FS2 decrypts fine",
                      fs1_age, shared_superblk->age, 0);
      }

      ASSERT (active_partition == FS_RMTS_PARTITION_ID_FS1 ||
              active_partition == FS_RMTS_PARTITION_ID_FS2);

      pm->booted_out_of_efs_partition = active_partition;

      break;  /* We found the latest sane copy, break. */
    }

    active_efs = !active_efs;
  }

  if (validation_res != 0)
  {
    fs_rmts_pm_try_golden_copy (pm); /* No sane copies found. try golden. */
  }
}

static enum fs_rmts_device_errno
fs_rmts_pm_write_iovec (struct fs_rmts_pm *pm, struct fs_iovec *iovec,
                  uint32 iovec_count,enum fs_rmts_partition_id which_partition)
{
  enum fs_rmts_device_errno result;
  int rmts_error;

  FS_MSG_MED_3 ("%d,%d,%d,EFS: RMTS Write_iovec start[parti,bytes,sync_count]",
               which_partition, iovec[0].size,
               pm->modem_copy.superblock->data_region.sync_counter);

  result = pm->dev->rmts_write_iovec (pm->dev, iovec, iovec_count,
                                      which_partition);

  switch (result)
  {
    /* Success so we dont do anything. */
    case FS_RMTS_ERRNO_SUCCESS:
    {
      break;
    }

    case FS_RMTS_ERRNO_TIMEOUT:
    {
      rmts_error = pm->dev->rmts_get_last_errno (pm->dev);
      FS_MSG_ERROR_3 ("[%d,%d,%d] EFS:RMTS Write_iovec timeout ", rmts_error,
                    which_partition, FS_RMTS_IOVEC_TIMEOUT);
      break;
    }

    case FS_RMTS_ERRNO_API_FAILURE:
    {
      rmts_error = pm->dev->rmts_get_last_errno (pm->dev);
      FS_MSG_ERROR_2 ("[%d,%d] EFS:RMTS Write_iovec Failed!", rmts_error,
                    which_partition);
      break;
    }

    case FS_RMTS_ERRNO_INVALID_PARAMS:
    {
      rmts_error = pm->dev->rmts_get_last_errno (pm->dev);
      FS_ERR_FATAL ("%d,%d,EFS:RMTS Write_iovec Invalid params!", rmts_error,
                    which_partition, 0);
      break;
    }

    case FS_RMTS_ERRNO_WRITE_PROTECTED:
    {
      rmts_error = pm->dev->rmts_get_last_errno (pm->dev);
      if (which_partition == FS_RMTS_PARTITION_ID_FSG)
      {
        FS_MSG_ERROR_2 ("%d, %d, EFS: Golden parti write protected",
                        rmts_error, which_partition);
        break;
      }

      FS_ERR_FATAL ("%d,%d,EFS:RMTS Write_iovec:Partition write protected!",
                     rmts_error, which_partition, 0);
      break;
    }

    default:
    {
      FS_ERR_FATAL ("%d,%d,%d,EFS: RMTS Write_iovec unknown error!",
                    result, which_partition, iovec->size);
      break;
    }
  }

  return result;
}

/* Writes the Modem data into the partition specified in 'which_partition'
   with the headers updated for booting out of the partition specified in
   'partition_to_boot'. */
static enum fs_rmts_device_errno
fs_rmts_pm_write_efs_partition (struct fs_rmts_pm *pm,
                                enum fs_rmts_partition_id which_partition,
                                enum fs_rmts_partition_id partition_to_boot)
{
  struct fs_iovec iovec;
  uint8 *src_addr, *image_dest_ptr;
  enum fs_rmts_device_errno result;
  struct fs_rmts_superblock *sb = pm->modem_copy.superblock;
  uint64 curr_phone_time_in_ms;

  memset (&iovec, 0, sizeof(iovec));

  /* Prepare Boot image header for partition being written to. */
  switch (partition_to_boot)
  {
    case FS_RMTS_PARTITION_ID_FS1:
      image_dest_ptr = (uint8 *) pm->modem_copy.superblock;
      break;

    case FS_RMTS_PARTITION_ID_FS2:
      image_dest_ptr = (uint8 *) pm->shared_copy.superblock;
      break;

    case FS_RMTS_PARTITION_ID_FSG:
      image_dest_ptr = (uint8 *) pm->golden_copy.superblock;
      break;

    case FS_RMTS_PARTITION_ID_FSC:
    default:
      image_dest_ptr = NULL;
      FS_ERR_FATAL ("%d, Img Hdr for invalid EFS partition", partition_to_boot,
                    0, 0);
      break;

  }

  curr_phone_time_in_ms = fs_unix_time_ms ();

  fs_rmts_image_header_update_for_partition (pm->image_header,
                                             partition_to_boot,
                                             image_dest_ptr,
                                             sb->age,
                                             curr_phone_time_in_ms,
                                             pm->is_write_cache_present,
                                             sb->data_region.sync_counter,
                                             pm->is_curr_sync_full_sync);

  /* Writing as one iovec is most effecient. Since superblock and write-cache
     region need to be written always, create the iovec with these by
     default. */
  iovec.sector_id = FS_RMTS_PM_IMG_HEADER_START_SECTOR;
  iovec.size = pm->write_cache_sync_size_bytes;
  /* If data region also needs to be written, increase the size. When syncing
     for Golden copy the full partition data needs to be written irrespective
     of the sync counter value. */
  if (pm->is_curr_sync_full_sync)
  {
    iovec.size = pm->full_sync_size_bytes;
  }

  src_addr = pm->shared_copy.ram_addr +
             (FS_RMTS_PM_IMG_HEADER_START_SECTOR * sb->sector_size);
  iovec.buff_base = src_addr;

  if (fs_enable_read_back_verify_sync != 0)
  {
    fs_verify_iovec.sector_id = iovec.sector_id;
    fs_verify_iovec.size = iovec.size;
    fs_verify_iovec.buff_base = iovec.buff_base;
  }

  result = fs_rmts_pm_write_iovec (pm, &iovec, 1, which_partition);

  return result;
}


static void
fs_rmts_pm_write_cookie_partition (struct fs_rmts_pm *pm, uint8 *buffer,
                                   uint32 buffer_size)
{
  enum fs_rmts_device_errno result;
  struct fs_iovec iovec;
  int rmts_error;

  ASSERT (pm != NULL);

  FS_MSG_HIGH_0 ("Cookie partition write started\n");

  /* copy the cookie ram data into shared ram address */
  fs_memscpy (pm->shared_copy.ram_addr, pm->ram_size, buffer, buffer_size);

  /* Prepare iovec for the writes */
  iovec.sector_id = 0; /* 0 is the start sector */
  iovec.buff_base = (uint8 *)pm->shared_copy.ram_addr;
  iovec.size = buffer_size;

  result = pm->dev->rmts_write_iovec (pm->dev, &iovec, 1,
                                      FS_RMTS_PARTITION_ID_FSC);
  if (result != FS_RMTS_ERRNO_SUCCESS)
  {
    rmts_error = pm->dev->rmts_get_last_errno (pm->dev);
    FS_ERR_FATAL ("[%d,%d,%d]EFS: Write cookie failed", rmts_error,
                  iovec.size, (uint32) iovec.buff_base);
  }

  FS_MSG_HIGH_0 ("Cookie partition write_iovec complete");
}


static void
fs_rmts_pm_try_golden_copy (struct fs_rmts_pm *pm)
{
  int result, valid;
  int is_tar_present = 0;
  uint8 *tar_addr = NULL;
  uint32 tar_size = 0;
  uint32 tar_offset = 0;
  uint8 *buf = NULL;
  uint32 buf_size, src_offset, dest_offset;
  struct fs_rmts_superblock *modem_superblock;
  uint8 wc_decrypt_failed, dr_decrypt_failed;

  pm->fsg_file_type = FS_RMTS_PM_FSG_FILE_TYPE_INVALID;

  switch (pm->modem_type)
  {
    case FS_RMTS_MODEM_TYPE_STANDALONE:
    {
      /* Read the whole partition data */
      uint32 num_sectors = pm->ram_size / FS_RMTS_SECTOR_SIZE;
      result = fs_rmts_pm_read_sectors (pm, FS_RMTS_PARTITION_ID_FSG, 0,
               num_sectors, FS_RMTS_SECTOR_SIZE, pm->modem_copy.ram_addr);
      if (result != 0)
        FS_ERR_FATAL ("%d, FSG Read failed", result, 0, 0);
      break;
    }

    case FS_RMTS_MODEM_TYPE_FUSION:
    {
      /* Golden copy is already in RAM, we dont have to read from server */
      /* We overwrite modem ram with golden ram*/
      fs_memscpy (pm->modem_copy.ram_addr, pm->ram_size,
                  pm->golden_copy.ram_addr, pm->ram_size);
      break;
    }

    default:
    {
      FS_ERR_FATAL ("[%d] Invalid rmts modem type ", pm->modem_type, 0, 0);
      break;
    }
  }

  /* FSG data is now in modem copy. */
  /* TAR image starts after boot image header. */
  src_offset = FS_RMTS_PM_TAR_START_SECTOR * FS_RMTS_SECTOR_SIZE;
  dest_offset = src_offset;
  buf = pm->modem_copy.ram_addr + src_offset;
  buf_size = pm->ram_size - src_offset;

#ifdef FEATURE_FS_TAR
  /* Verify the signature of the TAR/ZIP-TAR image in Modem Ram and get
     offset at which the actual TAR starts. */
  valid = fs_tar_verify_signature (buf, buf_size, &tar_offset, &tar_size,
                                   shared_superblk_buf,
                                   sizeof(shared_superblk_buf));
  if (valid == 1)
  {
    tar_addr = buf + tar_offset;
    is_tar_present = fs_tar_is_this_a_tar_file (tar_addr, tar_size);
    if (is_tar_present)
    {
      pm->fsg_file_type = FS_RMTS_PM_FSG_FILE_TYPE_TAR_FILE;
    }
    else
    {
      #ifdef FEATURE_FS_ZIP_TAR
        is_tar_present =  fs_zip_is_this_a_gzip_file (tar_addr, tar_size);
      #endif /* FEATURE_FS_ZIP_TAR */
      if (is_tar_present)
      {
        pm->fsg_file_type = FS_RMTS_PM_FSG_FILE_TYPE_ZIP_TAR;
      }
      else
      {
        FS_ERR_FATAL ("Signed FSG has unsupported format", 0, 0, 0);
      }
    }

    /* Offset the tar location from start of modem RAM for Tar processing. */
    src_offset += tar_offset;
  }
  else
  {
    is_tar_present = 0;
  }
#else /* FEATURE_FS_TAR */
  (void) tar_offset;
  (void) tar_size;
  (void) valid;
#endif /* FEATURE_FS_TAR */

  if (is_tar_present)
  {
    switch(pm->fsg_file_type)
    {
      case FS_RMTS_PM_FSG_FILE_TYPE_TAR_FILE:
      {
        #ifdef FEATURE_FS_TAR
        fs_rmts_pm_copy_tar_from_modem_to_shared (pm, src_offset, dest_offset,
                                                  tar_size);
        #endif /* FEATURE_FS_TAR */
        break;
      }

      case FS_RMTS_PM_FSG_FILE_TYPE_ZIP_TAR:
      {
        #ifdef FEATURE_FS_ZIP_TAR
        fs_rmts_copy_gzip_from_modem_to_shared (pm, src_offset, dest_offset,
                                                tar_size);
        #endif /* FEATURE_FS_ZIP_TAR */
        break;
      }

      default:
      {
        FS_ERR_FATAL ("Golden copy exists but format is unknown", 0, 0, 0);
        break;
      }
    }

    fs_rmts_fresh_start (pm);
    return;
  }

  /* It is not a tar. So it is a regular EFS image. Decrypt the superblock
     inplace. */
  modem_superblock = pm->modem_copy.superblock;
  result = fs_rmts_decrypt_validate_superblock (modem_superblock,
                 modem_superblock, pm, FS_RMTS_PARTITION_ID_FSG);
  if (result != 0)
  {
    /* The golden partition doesnt contain a FS copy. Do fresh start. */
    pm->debug_info.fsg_sb_decrypt_failed = 1;
    fs_rmts_fresh_start (pm);
    return;
  }

  pm->debug_info.fsg_decrypt_age_at_boot = modem_superblock->age;

  /* Decrypt regions in modem copy */
  result = fs_rmts_decrypt_active_efs_copy (FS_RMTS_PARTITION_ID_FS1, 0, pm,
              modem_superblock, &wc_decrypt_failed, &dr_decrypt_failed);

  pm->debug_info.fsg_wc_decrypt_failed = (wc_decrypt_failed == 1);
  pm->debug_info.fsg_dr_decrypt_failed = (dr_decrypt_failed == 1);

  if (result != 0)
  {
    /* We are using the golden copy, the superblock authenticated correctly but
       Write cache or data region did not. This can happen when power loss
       occurs while creating the golden copy. */
    FS_ERR_FATAL ("%d, Golden Copy corrupted!", result, 0, 0);
  }

  pm->fsg_bootup_action_image_info.was_sb_decrypt_successful = 1;
  pm->fsg_bootup_action_image_info.age = modem_superblock->age;
  fs_memscpy (pm->fsg_bootup_action_image_info.oem_data,
              sizeof (pm->fsg_bootup_action_image_info.oem_data),
              modem_superblock->oem_data,
              sizeof (pm->fsg_bootup_action_image_info.oem_data));
  pm->booted_out_of_efs_partition = FS_RMTS_PARTITION_ID_FSG;

  fs_rmts_boot_out_of_fsg (pm);
}

static void
fs_rmts_boot_out_of_fsg (struct fs_rmts_pm *pm)
{
  pm->debug_info.booted_out_of_fsg = 1;

  /* Set Active partition and age like it is a Fresh start to sync to FS1. */
  pm->active_efs_partition = FS_RMTS_PARTITION_ID_FS2;
  fs_rmts_super_init_superblock_sync_fields (pm->modem_copy.superblock);
}

/* Initialize data and RAM addresses to maintain write-cache region. */
static void
fs_rmts_pm_init_write_cache_region (struct fs_rmts_pm *pm,
                                    struct fs_rmts_superblock *sb)
{
  uint8 *cache_reg_start_addr;
  struct fs_rmts_write_cache_header *hdr;
  uint32 i;

  pm->is_write_cache_present = 0;
  pm->did_write_cache_overflow = 0;

  if (sb->write_cache_region.num_sectors == 0)
  {
    return;   /* No cache region configured, nothing to do. */
  }

  /* Hook up addresses of Header, Cluster table and Write-cache in RAM. */
  cache_reg_start_addr = pm->modem_copy.ram_addr +
                     (sb->write_cache_region.start_sector * sb->sector_size);
  hdr = (struct fs_rmts_write_cache_header *) (cache_reg_start_addr);

  /* Do basic sanity on Header fields. */
  if (hdr->version != FS_RMTS_PM_WRITE_CACHE_HDR_VER)
  {
    /* If header version is upgraded in the future, upgrade logic goes here. */
    FS_ERR_FATAL ("%d,%d, Unknown Write-Cache Header Version", hdr->version,
                  FS_RMTS_PM_WRITE_CACHE_HDR_VER, 0);
  }

  /* Compute RAM address and sizes */
  pm->write_cache_hdr = hdr;
  pm->write_cache_cluster_table = (uint32 *) (cache_reg_start_addr +
                                   hdr->write_cache_ctable_byte_offset);
  pm->write_cache = cache_reg_start_addr + hdr->write_cache_byte_offset;

  pm->write_cache_region_size_bytes = sb->write_cache_region.num_sectors *
                                      sb->sector_size;
  pm->write_cache_size_bytes = pm->write_cache_region_size_bytes -
                               hdr->write_cache_byte_offset;

  /* Calculate the mask for computing a hashed index for the clsuter number to
     save lookup time. */
  pm->write_cache_mask = 1;
  i = hdr->write_cache_ctable_num_entries;
  while (i > 0)
  {
    i = i >> 1;
    pm->write_cache_mask = pm->write_cache_mask << 1;
  }
  pm->write_cache_mask--;

  pm->is_write_cache_present = 1;
}

/* Initialize data and RAM addresses to maintain data region. */
static void
fs_rmts_pm_init_data_region (struct fs_rmts_pm *pm,
                             struct fs_rmts_superblock *sb)
{
  uint32 data_region_size, i, cluster_num;
  uint8 *src_addr, *dest_addr;

  data_region_size = sb->data_region.num_sectors * sb->sector_size;
  pm->data_region_size_bytes = data_region_size;
  pm->data_region =  pm->modem_copy.data_region;

  /* Set up available clusters for buffer layer. */
  pm->avail_clusters = data_region_size / sb->cluster_size;

  /* If write-cache has more recent data update the data region. */
  if (sb->data_region.sync_counter == 0)
  {
    ASSERT (pm->is_write_cache_present == 1);

    for (i = 0; i < pm->write_cache_hdr->write_cache_ctable_num_entries; i++)
    {
      cluster_num = pm->write_cache_cluster_table[i];
      if (cluster_num != FS_INVALID_PAGE_ID)
      {
        uint32 dest_size;

        src_addr = pm->write_cache + (i * pm->page_size);
        dest_addr = pm->data_region + (cluster_num * pm->page_size);
        dest_size = pm->data_region_size_bytes - (cluster_num * pm->page_size);
        fs_memscpy (dest_addr, dest_size, src_addr, pm->page_size);
      }
    }
  }

  /* Set to MAX after reset, to overwrite any corruptions to non-super regions
     of the partitions */
  sb->data_region.sync_counter = FS_RMTS_NUM_EFS_COPIES;
}

/* Update the cache with the new cluster data if space is available. Else
   cache has overflowed and take action on it. */
static void
fs_rmts_write_cache_add_cluster (struct fs_rmts_pm *pm, fs_cluster_id cluster,
                                 void *buffer)
{
  uint32 i, index, num_buffers;
  uint8 *dest_addr;

  if (pm->is_write_cache_present == 0)
  {
    return;
  }

  if (pm->did_write_cache_overflow)
  {
    return; /* Cache has overflowed and will not be used until after a sync. */
  }

  num_buffers = pm->write_cache_hdr->write_cache_ctable_num_entries;
  index = cluster & pm->write_cache_mask;
  if (index >= num_buffers)
    index -= num_buffers;

  for (i = 0; i < num_buffers; i++)
  {
    if (pm->write_cache_cluster_table[index] == cluster)
    {
      break;
    }
    else if (pm->write_cache_cluster_table[index] == FS_INVALID_PAGE_ID)
    {
      pm->write_cache_cluster_table[index] = cluster;
      break;
    }

    index++;
    if (index == num_buffers)
      index = 0;
  }

  if (i < num_buffers)
  {
    uint32 dest_size;

    ASSERT (pm->write_cache_cluster_table[index] == cluster);
    dest_addr = pm->write_cache + (index * pm->page_size);
    dest_size = pm->write_cache_size_bytes - (index * pm->page_size);
    fs_memscpy (dest_addr, dest_size, buffer, pm->page_size);
  }
  else   /* Cache is full. Make cache invalid. */
  {
    fs_rmts_pm_handle_write_cache_overflow (pm);
  }
}


/** Write the page for the supplied cluster. */
static void
fs_rmts_pm_page_write (fs_pm_t pm_data, fs_cluster_id cluster,
                       void *buffer, uint16 tag)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;
  uint8 *dest_addr;
  uint32 dest_size;

  (void) tag;

  /* The supplied cluster MUST be in the avail_pages range. */
  ASSERT (pm != NULL);
  FS_RMTS_VALIDATE_CLUSTER (cluster);

  /* Update data cluster. */
  dest_addr = pm->data_region + (cluster * pm->page_size);
  dest_size = pm->data_region_size_bytes - (cluster * pm->page_size);
  fs_memscpy (dest_addr, dest_size, buffer, pm->page_size);

  /* Update cache as well */
  fs_rmts_write_cache_add_cluster (pm, cluster, buffer);
}


/** Read the page for the supplied cluster. */
static void
fs_rmts_pm_page_read (fs_pm_t pm_data, fs_cluster_id cluster, void *buffer)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;
  uint8 *src_addr;

  ASSERT (pm != NULL);
  FS_RMTS_VALIDATE_CLUSTER (cluster);

  src_addr = pm->data_region + (cluster * pm->page_size);
  fs_memscpy (buffer, pm->page_size, src_addr, pm->page_size);
}

/** Store the upper-data's info in the superblock. */
static void
fs_rmts_pm_store_info (fs_pm_t pm_data, unsigned offset, uint32 data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);
  ASSERT (offset < FS_UPPER_DATA_COUNT);

  pm->modem_copy.superblock->upper_data[offset] = data;
}

static enum fs_rmts_partition_id
get_next_partition (enum fs_rmts_partition_id current_partition)
{
  enum fs_rmts_partition_id next_partition;

  /* Initalize the variable so compiler does not complain. Compiler ignores
   * that FS_ERR_FATAL never returns. */
  next_partition = FS_RMTS_PARTITION_ID_FS1;

  /* sync data to inactive partition */
  switch (current_partition)
  {
    case FS_RMTS_PARTITION_ID_FS1:
      next_partition = FS_RMTS_PARTITION_ID_FS2;
      break;

    case FS_RMTS_PARTITION_ID_FS2:
      next_partition = FS_RMTS_PARTITION_ID_FS1;
      break;

    case FS_RMTS_PARTITION_ID_FSG:
      next_partition = FS_RMTS_PARTITION_ID_FSG;
      break;

    default:
      FS_ERR_FATAL ("[%d] Unxpected partition", current_partition, 0, 0);
      break;
  }
  return next_partition;
}

/** Get the upper-data's info from the superblock. */
static uint32
fs_rmts_pm_get_info (fs_pm_t pm_data, unsigned offset)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);
  ASSERT (offset < FS_UPPER_DATA_COUNT);

  return pm->modem_copy.superblock->upper_data[offset];
}

/** Add the supplied sync_time_entry to the sync_timings array. */
static void
fs_rmts_pm_add_sync_time_entry (struct fs_rmts_pm *pm,
                                struct fs_rmts_pm_sync_timing *sync_time_entry)
{
  ASSERT (pm != NULL);
  ASSERT (sync_time_entry != NULL);

  ++pm->sync_timing_index;
  if (pm->sync_timing_index >= FS_RMTS_SYNC_TIME_QUEUE_SIZE)
  {
    pm->sync_timing_index = 0;
  }

  pm->sync_timings[pm->sync_timing_index].enc_time = sync_time_entry->enc_time;
  pm->sync_timings[pm->sync_timing_index].write_iovec_time =
    sync_time_entry->write_iovec_time;
  pm->sync_timings[pm->sync_timing_index].rmts_result =
    sync_time_entry->rmts_result;

}

/* Returns what is the status of the next sync.
 *
 * returns   1    sync not needed (no dirty data)
 *           0    Ready to sync (remotefs_server is up)
 */
static int
fs_rmts_sync_start_check (struct fs_rmts_pm *pm)
{
  if (pm->txns_since_sync == 0)
  {
    FS_MSG_HIGH_0 (",,EFS: Nothing to sync");
    return 1;
  }

  /* Check if last sync was to create the golden copy. If it was return
   * error. No more writes are allowed after golden copy is written to
   * fs1 or fs2. */
  if (pm->modem_copy.superblock->i_am_golden == 1)
  {
    FS_MSG_HIGH_0 (",,EFS: Trying to sync after golden copy. ");
    return -EPERM;
  }

  return 0;
}

static int
fs_rmts_pm_sync_start (fs_pm_t pm_data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;
  int result;
  uint64 start_time, end_time;

  ASSERT (pm != NULL);

  result = fs_rmts_sync_start_check (pm);
  if (result != 0)
    return result;

  /* Reset previous sync results. */
  pm->sync_result = -ENODEV;

  fs_sync_start (&pm->sync_data);

  pm->prev_txns_since_sync = pm->txns_since_sync;
  pm->txns_since_sync = 0;

  pm->prev_which_partition = pm->modem_copy.superblock->which_partition;
  pm->modem_copy.superblock->which_partition =
        get_next_partition (pm->modem_copy.superblock->which_partition);

  /* Update data sync counter based on overflow. */
  if ((pm->is_write_cache_present == 0) || pm->did_write_cache_overflow)
  {
    pm->modem_copy.superblock->data_region.sync_counter =
                                                     FS_RMTS_NUM_EFS_COPIES;
  }

  pm->is_curr_sync_full_sync = 0;
  if (pm->modem_copy.superblock->data_region.sync_counter > 0)
    pm->is_curr_sync_full_sync = 1;

  /* Open the write-cache for edits. */
  pm->did_write_cache_overflow_at_sync_start = pm->did_write_cache_overflow;
  pm->did_write_cache_overflow = 0;

  pm->prev_sb_age = pm->modem_copy.superblock->age;
  fs_rmts_super_increment_age (pm->modem_copy.superblock);

  /* Add the currently held sync timings to the queue. */
  fs_rmts_pm_add_sync_time_entry (pm, &pm->curr_sync_timing);
  memset (&pm->curr_sync_timing, 0, sizeof (pm->curr_sync_timing));

  /* Encrypt the data before writing it to the device. */
  start_time = fs_time_ms_u64 ();
  fs_rmts_encrypt_efs_copy (pm);
  end_time = fs_time_ms_u64 ();
  pm->curr_sync_timing.enc_time = (uint32)(end_time - start_time);

  FS_MSG_MED_2 ("%d,%d, EFS: Encrypt complete for sync [time(ms),age]",
             pm->curr_sync_timing.enc_time, pm->modem_copy.superblock->age);

  return 0;
}

static int
fs_rmts_pm_sync_wait (fs_pm_t pm_data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;
  uint64 start_time, end_time;
  enum fs_rmts_partition_id active_efs_partition;
  enum fs_rmts_device_errno write_result;
  int result = 0;

  ASSERT (pm != NULL);

  active_efs_partition = get_next_partition (pm->active_efs_partition);

  start_time = fs_time_ms_u64 ();
  write_result = fs_rmts_pm_write_efs_partition (pm, active_efs_partition,
                                                 active_efs_partition);

  end_time = fs_time_ms_u64 ();
  pm->curr_sync_timing.write_iovec_time = (uint32)(end_time - start_time);
  pm->curr_sync_timing.rmts_result = write_result;

  if (write_result == FS_RMTS_ERRNO_SUCCESS)
  {
    FS_MSG_MED_2 ("[%d,%d], EFS: write_iovec for sync [parti,time (ms)]",
                active_efs_partition, pm->curr_sync_timing.write_iovec_time);
    pm->sync_result = 0;

    if (fs_enable_read_back_verify_sync != 0)
    {
    #ifndef EFS_READ_BACK_ALLOW_ANY_TASK
    if (fs_os_self () != efs_tcb)
    {
      fs_os_tcb_type *self = fs_os_self ();
      FS_MSG_ERROR_2 ("RMTS limitation: reads only from FS task [%d, %d]",
                    (uint32)self, (uint32)fs_enable_read_back_verify_sync);
    }
    else
    {
    #endif /* EFS_READ_BACK_ALLOW_ANY_TASK */
      uint8 *shared_buf = fs_verify_iovec.buff_base;
      uint32 size = fs_verify_iovec.size;
      MD5_CTX hash_context;
      uint8 fs_original_hash[FS_READ_BACK_VERIFY_HASH_LEN];
      uint8 fs_read_back_hash[FS_READ_BACK_VERIFY_HASH_LEN];
      int result;

      memset (fs_original_hash, 0, FS_READ_BACK_VERIFY_HASH_LEN);
      memset (fs_read_back_hash, 0, FS_READ_BACK_VERIFY_HASH_LEN);

      start_time = fs_time_ms_u64 ();

      /* Compute the hash of the written data. */
      MD5Init (&hash_context);
      MD5Update (&hash_context, shared_buf, size);
      MD5Final (fs_original_hash, &hash_context);

      /* Clean the buffer. */
      memset (shared_buf, 0, size);

      /* Read the data back. */
      fs_rmts_pm_read_iovec (pm, &fs_verify_iovec, 1, active_efs_partition,
                             shared_buf);
      /* Compute the hash and compare it with the previous hash. */
      MD5Init (&hash_context);
      MD5Update (&hash_context, shared_buf, size);
      MD5Final (fs_read_back_hash, &hash_context);

      result = memcmp (fs_read_back_hash, fs_original_hash,
                       FS_READ_BACK_VERIFY_HASH_LEN);
      if (result != 0)
      {
        FS_ERR_FATAL ("%d Read Back Sync Verify Failed!", size, 0, 0);
      }
      end_time = fs_time_ms_u64 ();

      FS_MSG_MED_1 ("%d, EFS: Sync verify md5 passed (ms)",
                  (uint32)(end_time - start_time));
    #ifndef EFS_READ_BACK_ALLOW_ANY_TASK
    }
    #endif /* EFS_READ_BACK_ALLOW_ANY_TASK */
    }
  }
  else
  {
    /* Write iovec failed */
    FS_MSG_ERROR_3 (
            "%d,%d,%d,EFS:write for sync failed [parti,result,time(ms)]",
             active_efs_partition, write_result,
             pm->curr_sync_timing.write_iovec_time);
    pm->waiting_task = NULL;
    pm->sync_result = -ENODEV;
    result = -ENODEV;

    if (write_result == FS_RMTS_ERRNO_TIMEOUT)
    {
      pm->sync_result = -ETIMEDOUT;
      result = -ETIMEDOUT;
    }
  }

  return result;
}


static inline void
fs_rmts_pm_sync_revert (struct fs_rmts_pm *pm)
{
  /* Failure, revert back to values before sync_start */
  pm->modem_copy.superblock->age = pm->prev_sb_age;
  pm->modem_copy.superblock->which_partition = pm->prev_which_partition;
  if (pm->did_write_cache_overflow_at_sync_start)
  {
    /* The write-cache had overflown when sync started, and sync failed, so
       disable write-cache and reset contents. */
    fs_rmts_pm_reset_write_cache_data (pm);
    pm->did_write_cache_overflow = 1;
  }

  /* If a sync has not been requested, schedule another sync */
  if (pm->txns_since_sync == 0)
  {
    fs_sync_start_timer (&pm->sync_data);
  }

  pm->txns_since_sync += pm->prev_txns_since_sync;
}


static int
fs_rmts_pm_sync_end (fs_pm_t pm_data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;
  struct fs_timings_type *timings;

  ASSERT (pm != NULL);
  if (pm->sync_result != 0)
  {
    /* Sync failed, rollback the relevant values. */
    fs_rmts_pm_sync_revert (pm);
    return pm->sync_result;
  }

  if (pm->modem_copy.superblock->data_region.sync_counter > 0)
  {
    (pm->modem_copy.superblock->data_region.sync_counter)--;
  }

  /* Sync succeeded. */
  pm->active_efs_partition = get_next_partition (pm->active_efs_partition);

  /* If no transactions are pending, stop the timer
   * No need to re-sync the data */
  if (pm->txns_since_sync == 0)
  {
    fs_sync_stop_timer (&pm->sync_data);
  }

  fs_sync_complete (&pm->sync_data);

  timings = fs_timings_get_instance ();

  timings->efs_sync_timings.encryption_time  = pm->curr_sync_timing.enc_time;
  timings->efs_sync_timings.write_iovec_time =
                                        pm->curr_sync_timing.write_iovec_time;

  return 0;
}


static int
fs_rmts_pm_sync_no_wait (fs_pm_t pm_data, fs_sync_token_t *sync_token)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;
  int result;

  ASSERT (pm != NULL);

  *sync_token = fs_sync_get_completed_token (&pm->sync_data);

  result = fs_rmts_sync_start_check (pm);
  if (result != 0)
    return result;

  *sync_token = fs_sync_request_sync (&pm->sync_data);

  return result;
}


static int
fs_rmts_pm_sync_get_status (fs_pm_t pm_data, fs_sync_token_t sync_token,
                            enum fs_sync_result *sync_result)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  *sync_result = fs_sync_get_status (&pm->sync_data, sync_token);

  return 0;
}

enum fs_rmts_partition_id
fs_rmts_pm_golden_copy_sync (struct fs_rmts_pm *pm)
{
  enum fs_rmts_device_errno result;
  enum fs_rmts_partition_id write_partition;
  uint8 original_partition;

  ASSERT (pm != NULL);
  /* Make sure there are no pending syncs. */
  ASSERT (pm->txns_since_sync == 0);

  original_partition = pm->modem_copy.superblock->which_partition;
  /* Update superblock fields to indicate sync for FSG. */
  pm->modem_copy.superblock->i_am_golden = 1;
  pm->modem_copy.superblock->which_partition = FS_RMTS_PARTITION_ID_FSG;
  pm->modem_copy.superblock->debug_data_bit_map &=
                                ~(FS_RMTS_SUPER_GOLDEN_NOT_CREATED_MASK);
  pm->is_curr_sync_full_sync = 1;
  /* Since FSG write will write the full partition data, we need to encrypt
     the filesystem again as the data region would have been updated. */
  fs_rmts_encrypt_efs_copy (pm);

  write_partition = (pm->write_to_fsg_for_golden_copy == 1) ?
                     FS_RMTS_PARTITION_ID_FSG : pm->active_efs_partition;

  result = fs_rmts_pm_write_efs_partition (pm, write_partition,
                                           FS_RMTS_PARTITION_ID_FSG);
  if (result != FS_RMTS_ERRNO_SUCCESS)
  {
    FS_ERR_FATAL ("%d,%d, EFS: make golden write failed [parti, result]",
                   write_partition, result, 0);
  }

  /* If the write was directly to FSG, then revert the values. */
  if (write_partition == FS_RMTS_PARTITION_ID_FSG)
  {
    pm->modem_copy.superblock->i_am_golden = 0;
    pm->modem_copy.superblock->which_partition = original_partition;
  }

  return write_partition;
}

static int
fs_rmts_pm_make_golden_copy (fs_pm_t pm_data)
{
  int cookie_handle;
  struct fs_backup_cookie_payload backup_cookie;
  int result;
  uint32 buff_size;
  enum fs_rmts_partition_id write_partition;
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);

  write_partition = fs_rmts_pm_golden_copy_sync (pm);
  if (write_partition == FS_RMTS_PARTITION_ID_FSG)
    return 0; /* Direct write to FSG; nothing more to do */

  /* Re-using shared_superblk_buf for storing backup cookie data. */
  buff_size = FS_RMTS_SECTOR_SIZE;
  memset (shared_superblk_buf, 0x0, buff_size);
  cookie_handle = fs_cookie_get_handle_for_ram (shared_superblk_buf,buff_size);
  if (cookie_handle < 0)
  {
    FS_ERR_FATAL ("%d, %d, %d, Cookie get handle failed", cookie_handle,
                  (uint32)shared_superblk_buf, buff_size);
  }

  backup_cookie.modem_type = pm->modem_type;
  backup_cookie.src_partition = write_partition;
  backup_cookie.dest_partition = FS_RMTS_PARTITION_ID_FSG;
  backup_cookie.second_src_partition = get_next_partition (write_partition);
  backup_cookie.corrupt_src_partitions = 1;

  result = fs_cookie_add (cookie_handle, EFS_COOKIE_ID_BACKUP,
                         (uint8 *)&backup_cookie, sizeof (backup_cookie));
  if (result != 0)
  {
    FS_ERR_FATAL ("%d, %d, %d, Cookie add failed", result,
                   EFS_COOKIE_ID_BACKUP, sizeof (backup_cookie));
  }

  /* Release cookie handle */
  result = fs_cookie_release_handle (cookie_handle);
  ASSERT (result == 0);

  fs_rmts_pm_write_cookie_partition (pm, shared_superblk_buf, buff_size);

  return 0;
}

static void
fs_rmts_pm_start_transaction (fs_pm_t pm_data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);
  ASSERT (pm->inside_txn == 0); /* No nested transactions please */

  pm->inside_txn = 1; /* Okay, let the transaction begin. */
}

static void
fs_rmts_pm_end_transaction (fs_pm_t pm_data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);
  ASSERT (pm->inside_txn == 1); /* make sure we are inside an transaction. */
  pm->inside_txn = 0;

  /* If not started already, start the sync timer. */
  if (pm->txns_since_sync == 0)
  {
    fs_sync_start_timer (&pm->sync_data);
  }

  ++pm->txns_since_sync;
}

/** return how many clusters are available to the Buffer layer for writing. */
static uint32
fs_rmts_pm_space_limit (fs_pm_t pm_data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);
  return pm->avail_clusters;
}

/** These Buffer layer callbacks are not used. */
static void
fs_rmts_pm_register_free_check_cb (fs_pm_t pm_data, int (*is_used_cb)
          (void *, fs_cluster_id cluster), void *data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);
  (void)is_used_cb;
  (void)data;
}

/** These Buffer layer callbacks are not used. */
static void
fs_rmts_pm_register_alloc_count_cb (fs_pm_t pm_data,
          uint32 (*alloc_count_cb) (void *), void *data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);
  (void)alloc_count_cb;
  (void)data;
}


static int
fs_rmts_pm_fs_init_complete (fs_pm_t pm_data)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;
  uint32 tar_zip_offset;

  ASSERT (pm != NULL);

  /* Shared RAM has Boot Image Header created already. The tar/zip file is
     copied after the header. */
  tar_zip_offset = FS_RMTS_PM_TAR_START_SECTOR * FS_RMTS_SECTOR_SIZE;

  switch(pm->fsg_file_type)
  {
    case FS_RMTS_PM_FSG_FILE_TYPE_INVALID:
      break;

    case FS_RMTS_PM_FSG_FILE_TYPE_TAR_FILE:
#ifdef FEATURE_FS_TAR
      ASSERT (pm->untar_size > 0);
      fs_rmts_pm_untar_from_shared_ram (pm, tar_zip_offset);
#endif /* FEATURE_FS_TAR */
      break;

    case FS_RMTS_PM_FSG_FILE_TYPE_ZIP_TAR:
#ifdef FEATURE_FS_ZIP_TAR
      ASSERT (pm->untar_size > 0);
      fs_rmts_pm_unzip_from_shared_ram (pm, tar_zip_offset);
#endif /* FEATURE_FS_ZIP_TAR */
      break;

    default:
      FS_ERR_FATAL ("[%d - %d] invalid fsg file type",
              pm->fsg_file_type, pm->untar_size, 0);
      break;
  }

  return 0;
}

static int
fs_rmts_pm_lock_efs (fs_pm_t pm_data)
{
  (void) pm_data;
  return -EPERM;
}

static int
fs_rmts_pm_fs_borrow_ram (fs_pm_t pm_data, void **ram_addr, uint32 *ram_size)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);

  *ram_addr = pm->shared_copy.ram_addr;
  *ram_size = pm->ram_size;
  return 0;
}

#ifdef FEATURE_FS_TAR


static void
secapi_clk_enable_helper (void)
{
#ifdef SECAPI_HAVE_SLEEP_API
  IxErrnoType result;

  result = secapi_clk_enable ();
  if (result != E_SUCCESS)
  {
    FS_MSG_HIGH_1 ("%d, secapi_clk_enable failed.", result);
  }
#endif
}

static int
fs_rmts_pm_store_data_in_sb (fs_pm_t pm_data, const void *data,
                             uint32 data_size)
{
  struct fs_rmts_pm *pm = (struct fs_rmts_pm *)pm_data;

  ASSERT (pm != NULL);
  ASSERT (data != NULL);
  ASSERT (data_size != 0);

  if (data_size > sizeof (pm->modem_copy.superblock->oem_data))
  {
    return -ENOSPC;
  }

  memset (pm->modem_copy.superblock->oem_data, 0,
          sizeof (pm->modem_copy.superblock->oem_data));

  fs_memscpy (pm->modem_copy.superblock->oem_data,
              sizeof (pm->modem_copy.superblock->oem_data), data, data_size);

  ++pm->txns_since_sync;

  return 0;
}

static void
secapi_clk_disable_helper (void)
{
#ifdef SECAPI_HAVE_SLEEP_API
  IxErrnoType result;

  result = secapi_clk_disable ();
  if (result != E_SUCCESS)
  {
    FS_MSG_HIGH_1 ("%d, secapi_clk_disable failed.", result);
  }
#endif
}

static void
create_hmac_helper (uint8 *msg_ptr, uint16 msg_len, uint8 *msg_digest_ptr)
{
  IxErrnoType result;

  result = secapi_util_create_hmac_with_hw_key (msg_ptr, msg_len,
                                                msg_digest_ptr);
  if (result != E_SUCCESS)
  {
    FS_ERR_FATAL ("%d, hmac creation failed", result, 0, 0);
  }
}

static void
fs_rmts_pm_allocate_memory_for_untar (struct fs_rmts_pm *pm)
{
  uint8 *data_ptr, *hash1_ptr, *hash2_ptr, *hash3_ptr;
  size_t malloc_size;

  ASSERT ((fs_rmts_pm_untar_malloc_size % FS_RMTS_SECTOR_SIZE) == 0);

  /* Allocate memory to hold untar-buffer + 3 hashes :
   * 1st hash : chaining hash
   * 2nd hash : intermediate temp hash
   * 3rd hash : final hash computed when TAR file was in modem-ram
   */
  malloc_size = (fs_rmts_pm_untar_malloc_size +  (3 * FS_RMTS_META_DATA_SIZE));
  data_ptr = (uint8 *)malloc (malloc_size);
  if (data_ptr == NULL)
  {
    FS_ERR_FATAL ("%d, untar malloc failed", malloc_size, 0, 0);
  }

  hash1_ptr = data_ptr + fs_rmts_pm_untar_malloc_size;
  hash2_ptr = hash1_ptr + FS_RMTS_META_DATA_SIZE;
  hash3_ptr = hash2_ptr + FS_RMTS_META_DATA_SIZE;

  /* Chain the data-buffer + a hash */
  pm->untar_buffer = data_ptr;
  pm->untar_buffer_size = fs_rmts_pm_untar_malloc_size+FS_RMTS_META_DATA_SIZE;

  pm->temp_hash_ptr  = hash2_ptr;
  pm->final_hash_ptr = hash3_ptr;
}

static void
fs_rmts_pm_free_memory_for_untar (struct fs_rmts_pm *pm)
{
  ASSERT (pm->untar_buffer != NULL);
  free (pm->untar_buffer);
  pm->untar_buffer = NULL;
  pm->temp_hash_ptr = NULL;
  pm->final_hash_ptr = NULL;
}

/* Copy the TAR-file from the Modem-ram onto the Shared-ram and while doing
 * this copy prepare a chain-hash so that we can detect tampering if any done
 * to the TAR-file for the short duration when it lives on the Shared-ram.
 *
 * param pm  Page manager structure for remote storage.
 * param offset from what offset to start copying from modem ram.
 * param total_size number of bytes to copy from modem ram to shared ram.
 */
static void
fs_rmts_pm_copy_tar_from_modem_to_shared (struct fs_rmts_pm *pm,
                                         uint32 src_offset, uint32 dest_offset,
                                         uint32 total_size)
{
  uint8 *modem_ram, *shared_ram;
  uint8 *buffer_ptr, *hash_ptr;
  uint32 buffer_size, hash_size;
  uint16 buffer_plus_hash_size;
  uint32 curr_size, copy_size, remaining_size;

  pm->untar_size = total_size;

  /* Sanity check the offset and size before copying. */
  ASSERT (src_offset + total_size <= pm->ram_size);
  ASSERT (dest_offset + total_size <= pm->ram_size);

  modem_ram = pm->modem_copy.ram_addr + src_offset;
  shared_ram = pm->shared_copy.ram_addr + dest_offset;

  fs_rmts_pm_allocate_memory_for_untar (pm);

  buffer_ptr = pm->untar_buffer;
  buffer_size = pm->untar_buffer_size - FS_RMTS_META_DATA_SIZE;
  hash_ptr = buffer_ptr + buffer_size;
  hash_size = FS_RMTS_META_DATA_SIZE;
  ASSERT ((buffer_size % FS_RMTS_SECTOR_SIZE) == 0);

  /* For the first time, we have to start with zero-hash */
  memset (pm->untar_buffer,   0, pm->untar_buffer_size);
  memset (pm->temp_hash_ptr,  0, hash_size);
  memset (pm->final_hash_ptr, 0, hash_size);

  secapi_clk_enable_helper ();

  curr_size = 0;
  while (curr_size < total_size)
  {
    remaining_size = total_size - curr_size;
    copy_size = (remaining_size < buffer_size) ? remaining_size : buffer_size;

    hash_ptr = buffer_ptr + copy_size;
    buffer_plus_hash_size = (uint16)(copy_size + hash_size);

    /* Prepare to chain the data-buffer and previous-hash */
    fs_memscpy (buffer_ptr, copy_size, modem_ram, copy_size);
    fs_memscpy (hash_ptr, hash_size, pm->temp_hash_ptr, hash_size);

    /* chain data-buffer with previous-hash and compute new chained-hash. */
    create_hmac_helper (buffer_ptr, buffer_plus_hash_size, pm->temp_hash_ptr);

    fs_memscpy (shared_ram, copy_size, modem_ram, copy_size);
    modem_ram  += copy_size;
    shared_ram += copy_size;
    curr_size  += copy_size;
  }

  secapi_clk_disable_helper ();

  /* Store the final hash to be used for verification later. */
  fs_memscpy (pm->final_hash_ptr, hash_size, pm->temp_hash_ptr, hash_size);
}


static void
fs_rmts_pm_untar_from_shared_ram (struct fs_rmts_pm *pm, uint32 start_offset)
{
  uint8 *shared_ram;
  uint8 *buffer_ptr, *hash_ptr;
  uint32 buffer_size, hash_size;
  uint16 buffer_plus_hash_size;
  uint32 curr_size, copy_size, remaining_size;
  struct fs_tar_info *tar_info;
  int process_tar, did_hash_match, result;

  shared_ram = pm->shared_copy.ram_addr + start_offset;

  buffer_ptr = pm->untar_buffer;
  buffer_size = pm->untar_buffer_size - FS_RMTS_META_DATA_SIZE;
  hash_ptr = buffer_ptr + buffer_size;
  hash_size = FS_RMTS_META_DATA_SIZE;
  ASSERT ((buffer_size % FS_RMTS_SECTOR_SIZE) == 0);

  /* For the first time, we have to start with zero-hash */
  memset (pm->untar_buffer,   0, pm->untar_buffer_size);
  memset (pm->temp_hash_ptr,  0, hash_size);

  process_tar = 1;
  tar_info = fs_tar_open ();
  if (tar_info == NULL)
  {
    FS_ERR_FATAL ("fs_tar_open() failed!", 0, 0, 0);
  }

  secapi_clk_enable_helper ();

  curr_size = 0;
  while (curr_size < pm->untar_size)
  {
    remaining_size = pm->untar_size - curr_size;
    copy_size = (remaining_size < buffer_size) ? remaining_size : buffer_size;

    hash_ptr = buffer_ptr + copy_size;
    buffer_plus_hash_size = (uint16)(copy_size + hash_size);

    /* Prepare to chain the data-buffer and previous-hash */
    fs_memscpy (buffer_ptr, copy_size, shared_ram, copy_size);
    fs_memscpy (hash_ptr, hash_size, pm->temp_hash_ptr, hash_size);

    /* chain data-buffer with previous-hash and compute new chained-hash. */
    create_hmac_helper (buffer_ptr, buffer_plus_hash_size, pm->temp_hash_ptr);

    if (process_tar)
    {
      process_tar = fs_tar_process_data (tar_info, buffer_ptr, copy_size);
    }

    shared_ram += copy_size;
    curr_size += copy_size;
  }

  secapi_clk_disable_helper ();

  /* Compare current Hash with the one computed on Modem-ram. */
  did_hash_match = memcmp (pm->final_hash_ptr, pm->temp_hash_ptr, hash_size);
  if (did_hash_match != 0)
  {
    FS_ERR_FATAL ("TAR Data has been tampered (%d)", did_hash_match, 0, 0);
  }

  result = fs_tar_close (tar_info);
  if (result != 0)
  {
    FS_ERR_FATAL ("fs_tar_close() failed! (%d)", result, 0, 0);
  }

  fs_rmts_pm_free_memory_for_untar (pm);
}


#ifdef FEATURE_FS_ZIP_TAR

/* Copy the Zip-file from Modem-ram to Shared-ram.
 *
 * While unzipping a zip-file, we have the following 2 ways in which the
 * Zipped-TAR file can be tampered.
 *  1. The Zip-file itself when it is held in Shared-ram can be tampered.
 *  2. While unzipping, we will be using the shared-ram as Heap for the
 *     ZLIB library, now this Heap region can be tampered.
 *
 *  Because of the above 2 reasons, we will be computing the chained-hash
 *  not of the zip file itself, but of the TAR-file inside the zip file,
 *  and later when we unzip the TAR-file to write to EFS, we will recompute
 *  the TAR-file's chain hash and confirm that there were no tamperings in the
 *  shared-ram.
 *
 * param pm         Page manager structure for remote storage.
 * param offset     From what offset to start copying from modem ram.
 * param total_size Number of bytes to copy from modem ram to shared ram.
 */
static void
fs_rmts_copy_gzip_from_modem_to_shared (struct fs_rmts_pm *pm,
                                        uint32 src_offset, uint32 dest_offset,
                                        uint32 total_size)
{
  uint8 *modem_ram, *shared_ram;
  uint8 *buffer_ptr, *hash_ptr, *temp_buffer_ptr;
  uint32 buffer_size, hash_size;
  uint16 buffer_plus_hash_size;
  uint32 avail_bytes;

  /* Variables for zlib */
  void *zip_handle;
  enum fs_zip_mode mode = FS_ZIP_MODE_DECOMPRESSION;
  enum fs_zip_return_status status = FS_ZIP_STATUS_UNKNOWN_ERROR;
  void *zlib_heap_start;
  uint32 zlib_heap_size;
  uint32 unzip_output_expected, unzip_output_length;
  int is_unzip_a_tar = 0;


  /* Sanity check the offset and size before copying. */
  FS_ASSERT (src_offset + total_size <= pm->ram_size);
  FS_ASSERT (dest_offset + total_size <= pm->ram_size);

  modem_ram = pm->modem_copy.ram_addr + src_offset;
  shared_ram = pm->shared_copy.ram_addr + dest_offset;

  fs_rmts_pm_allocate_memory_for_untar (pm);

  buffer_ptr = pm->untar_buffer;
  buffer_size = pm->untar_buffer_size - FS_RMTS_META_DATA_SIZE;
  hash_ptr = buffer_ptr + buffer_size;
  hash_size = FS_RMTS_META_DATA_SIZE;
  ASSERT ((buffer_size % FS_RMTS_SECTOR_SIZE) == 0);

  /* For the first time, we have to start with zero-hash */
  memset (pm->untar_buffer,   0, pm->untar_buffer_size);
  memset (pm->temp_hash_ptr,  0, hash_size);
  memset (pm->final_hash_ptr, 0, hash_size);

  /* Allocate a heap to zlib in the remaning modem ram. */
  zlib_heap_start = modem_ram + total_size;
  zlib_heap_size  = pm->ram_size - (src_offset + total_size);

  zip_handle = fs_zip_open (mode, zlib_heap_start, zlib_heap_size);
  if(zip_handle == NULL)
  {
    FS_ERR_FATAL ("fs_zip_open() failed!", 0, 0, 0);
  }

  /* Give the whole zip file buffer to ZLIB. */
  status = fs_zip_set_input (zip_handle, modem_ram, total_size);
  if (status != FS_ZIP_STATUS_SUCCESS)
  {
    FS_ERR_FATAL ("%d, Zlib set input failed.", status, 0, 0);
  }

  secapi_clk_enable_helper ();

  temp_buffer_ptr = buffer_ptr;
  unzip_output_expected = buffer_size;
  avail_bytes = 0;

  while (status == FS_ZIP_STATUS_SUCCESS)
  {
    unzip_output_length = 0;
    status = fs_zip_decompress (zip_handle, temp_buffer_ptr,
                                unzip_output_expected, &unzip_output_length);
    switch (status)
    {
      case FS_ZIP_STATUS_SUCCESS:
      case FS_ZIP_STATUS_FINISHED:
      {
        avail_bytes += unzip_output_length;

        if (status == FS_ZIP_STATUS_SUCCESS)
        {
          if(unzip_output_length < unzip_output_expected)
          {
            /* There is still room in the buffer, so lets decompress again. */
            unzip_output_expected -= unzip_output_length;
            temp_buffer_ptr += unzip_output_length;
            continue;
          }
        }

        if (avail_bytes == 0)
          break;

        ASSERT ((avail_bytes % FS_RMTS_SECTOR_SIZE) == 0);

        /* One time check to see if the Zipped file is a TAR-file. */
        if(!is_unzip_a_tar)
        {
          is_unzip_a_tar = fs_tar_is_this_a_tar_file (buffer_ptr, avail_bytes);
          if(!is_unzip_a_tar)
          {
            FS_ERR_FATAL ("%d, Decompressed file is not a valid tar file",
                           avail_bytes, 0, 0);
          }
        }

        /* avail_bytes number of bytes are available in the buffer, Fix the
         * hash_ptr right beneath the buffer. */
        hash_ptr = buffer_ptr + avail_bytes;
        buffer_plus_hash_size = (uint16)(avail_bytes + hash_size);

        /* Prepare to chain the data-buffer and previous-hash. */
        fs_memscpy (hash_ptr, hash_size, pm->temp_hash_ptr, hash_size);

        create_hmac_helper (buffer_ptr, buffer_plus_hash_size,
                            pm->temp_hash_ptr);

        /* Ready to handle the next unzip buffer. */
        temp_buffer_ptr = buffer_ptr;
        unzip_output_expected = buffer_size;
        avail_bytes = 0;
      }
      break;

      default:
      {
        FS_ERR_FATAL ("%d, Unexpected fs_zip state.", status, 0, 0);
      }
      break;

    }
  }

  if (status != FS_ZIP_STATUS_FINISHED)
  {
    FS_ERR_FATAL ("[%d]unzip failed code", status, 0, 0);
  }

  secapi_clk_disable_helper ();

  /* Close unconditionally */
  fs_zip_close (zip_handle);

  /* Store the final hash to be used for verification later. */
  fs_memscpy (pm->final_hash_ptr, hash_size, pm->temp_hash_ptr, hash_size);

  /* Copy the Zip-file from modem-ram to shared-ram. */
  fs_memscpy (shared_ram, total_size, modem_ram, total_size);
  pm->untar_size = total_size;

}


static void
fs_rmts_pm_unzip_from_shared_ram (struct fs_rmts_pm *pm, uint32 start_offset)
{
  uint8 *shared_ram;
  uint8 *buffer_ptr, *hash_ptr, *temp_buffer_ptr;
  uint32 buffer_size, hash_size;
  uint16 buffer_plus_hash_size;
  uint32 avail_bytes;

  /* Variables for zlib */
  void *zip_handle;
  enum fs_zip_mode mode = FS_ZIP_MODE_DECOMPRESSION;
  enum fs_zip_return_status status = FS_ZIP_STATUS_UNKNOWN_ERROR;
  void *zlib_heap_start;
  uint32 zlib_heap_size;
  uint32 unzip_output_expected, unzip_output_length;
  int process_tar = 1, did_hash_match, result;
  struct fs_tar_info *tar_info;

  shared_ram = pm->shared_copy.ram_addr + start_offset;

  buffer_ptr = pm->untar_buffer;
  buffer_size = pm->untar_buffer_size - FS_RMTS_META_DATA_SIZE;
  hash_ptr = buffer_ptr + buffer_size;
  hash_size = FS_RMTS_META_DATA_SIZE;
  ASSERT ((buffer_size % FS_RMTS_SECTOR_SIZE) == 0);

  /* For the first time, we have to start with zero-hash */
  memset (pm->untar_buffer,   0, pm->untar_buffer_size);
  memset (pm->temp_hash_ptr,  0, hash_size);

  /* Allocate a heap to zlib */
  zlib_heap_start = shared_ram + pm->untar_size;
  zlib_heap_size  = pm->ram_size - pm->untar_size;

  zip_handle = fs_zip_open (mode, zlib_heap_start, zlib_heap_size);
  if(zip_handle == NULL)
  {
    FS_ERR_FATAL ("fs_zip_open() failed!", 0, 0, 0);
  }

  /* Give the whole zip file buffer to ZLIB. */
  status = fs_zip_set_input (zip_handle, shared_ram, pm->untar_size);
  if (status != FS_ZIP_STATUS_SUCCESS)
  {
    FS_ERR_FATAL ("%d, Zlib set input failed.", status, 0, 0);
  }

  tar_info = fs_tar_open ();
  if (tar_info == NULL)
  {
    FS_ERR_FATAL ("fs_tar_open failed!", 0, 0, 0);
  }

  secapi_clk_enable_helper ();

  temp_buffer_ptr = buffer_ptr;
  unzip_output_expected = buffer_size;
  avail_bytes = 0;

  while (status == FS_ZIP_STATUS_SUCCESS)
  {
    unzip_output_length = 0;
    status = fs_zip_decompress (zip_handle, temp_buffer_ptr,
                                unzip_output_expected, &unzip_output_length);
    switch (status)
    {
      case FS_ZIP_STATUS_SUCCESS:
      case FS_ZIP_STATUS_FINISHED:
      {
        avail_bytes += unzip_output_length;

        if (status == FS_ZIP_STATUS_SUCCESS)
        {
          if(unzip_output_length < unzip_output_expected)
          {
            /* There is still room in the buffer, so lets decompress again. */
            unzip_output_expected -= unzip_output_length;
            temp_buffer_ptr += unzip_output_length;
            continue;
          }
        }

        if (avail_bytes == 0)
          break;

        ASSERT ((avail_bytes % FS_RMTS_SECTOR_SIZE) == 0);

        /* avail_bytes number of bytes are available in the buffer, Fix the
         * hash_ptr right beneath the buffer. */
        hash_ptr = buffer_ptr + avail_bytes;
        buffer_plus_hash_size = (uint16)(avail_bytes + hash_size);

        /* Prepare to chain the data-buffer and previous-hash. */
        fs_memscpy (hash_ptr, hash_size, pm->temp_hash_ptr, hash_size);

        create_hmac_helper (buffer_ptr, buffer_plus_hash_size,
                            pm->temp_hash_ptr);

        if(process_tar)
        {
          process_tar = fs_tar_process_data (tar_info, buffer_ptr,avail_bytes);
        }

        /* Ready to handle the next unzip buffer. */
        temp_buffer_ptr = buffer_ptr;
        unzip_output_expected = buffer_size;
        avail_bytes = 0;
      }
      break;

      default:
      {
        FS_ERR_FATAL ("%d, Unexpected fs_zip state.", status, 0, 0);
      }
      break;

    }
  }

  if (status != FS_ZIP_STATUS_FINISHED)
  {
    FS_ERR_FATAL ("[%d]unzip failed code", status, 0, 0);
  }

  secapi_clk_disable_helper ();

  /* Compare current Hash with the one computed on Modem-ram. */
  did_hash_match = memcmp (pm->final_hash_ptr, pm->temp_hash_ptr, hash_size);
  if (did_hash_match != 0)
  {
    FS_ERR_FATAL ("ZIP/TAR Data has been tampered (%d)", did_hash_match, 0, 0);
  }

  fs_zip_close (zip_handle);

  result = fs_tar_close (tar_info);
  if (result != 0)
  {
    FS_ERR_FATAL ("fs_tar_close() failed! (%d)", result, 0, 0);
  }

  fs_rmts_pm_free_memory_for_untar (pm);

}

#endif /* FEATURE_FS_ZIP_TAR */

#endif /* FEATURE_FS_TAR */

static int
fs_rmts_pm_bootup_action_notify (struct fs_rmts_pm *pm)
{
  int result;
  enum fs_rmts_bootup_action_image_id image_id;

  image_id = FS_RMTS_BOOTUP_ACTION_IMAGE_ID_NONE;

  switch (pm->booted_out_of_efs_partition)
  {
    case FS_RMTS_PARTITION_ID_FS1:
      image_id = FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FS1;
      break;

    case FS_RMTS_PARTITION_ID_FS2:
      image_id = FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FS2;
      break;

    case FS_RMTS_PARTITION_ID_FSG:
      image_id = FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FSG;
      break;

    default:
      image_id = FS_RMTS_BOOTUP_ACTION_IMAGE_ID_NONE;
      break;
  }

  result = fs_rmts_bootup_action_notify (image_id,
              &pm->fs1_bootup_action_image_info,
              &pm->fs2_bootup_action_image_info,
              &pm->fsg_bootup_action_image_info);

  return result;
}

static void
fs_rmts_pm_init_ops (fs_pm_t pm_data)
{
  ASSERT (pm_data != NULL);
  pm_data->ops.page_write = fs_rmts_pm_page_write;
  pm_data->ops.page_read = fs_rmts_pm_page_read;
  pm_data->ops.store_info = fs_rmts_pm_store_info;
  pm_data->ops.get_info = fs_rmts_pm_get_info;
  pm_data->ops.start_transaction = fs_rmts_pm_start_transaction;
  pm_data->ops.end_transaction = fs_rmts_pm_end_transaction;
  pm_data->ops.space_limit = fs_rmts_pm_space_limit;
  pm_data->ops.register_free_check_cb = fs_rmts_pm_register_free_check_cb;
  pm_data->ops.register_alloc_count_cb = fs_rmts_pm_register_alloc_count_cb;
  pm_data->ops.shred = NULL;
  pm_data->ops.scrub = NULL;
  pm_data->ops.read_iovec = NULL;
  pm_data->ops.write_iovec = NULL;
  pm_data->ops.sync_start = fs_rmts_pm_sync_start;
  pm_data->ops.sync_wait  = fs_rmts_pm_sync_wait;
  pm_data->ops.sync_end   = fs_rmts_pm_sync_end;
  pm_data->ops.sync_no_wait    = fs_rmts_pm_sync_no_wait;
  pm_data->ops.sync_get_status = fs_rmts_pm_sync_get_status;
  pm_data->ops.make_golden_copy = fs_rmts_pm_make_golden_copy;
  pm_data->ops.fs_init_complete = fs_rmts_pm_fs_init_complete;
  pm_data->ops.lock_efs = fs_rmts_pm_lock_efs;
  pm_data->ops.borrow_ram = fs_rmts_pm_fs_borrow_ram;
  pm_data->ops.store_data_in_sb = fs_rmts_pm_store_data_in_sb;
}

#endif /* FEATURE_EFS_EFS2_ON_RMTS */
