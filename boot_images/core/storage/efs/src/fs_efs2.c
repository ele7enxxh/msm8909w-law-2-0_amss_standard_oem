/***********************************************************************
 * fs_efs2.c
 *
 * Filesystem mountpoints for EFS2
 * Copyright (C) 2002-2014, QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_efs2.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $


when          who     what, where, why
--------      ---     ------------------------------------------------------
2014-04-26    vm      Use correct FS_MSG macros to save RAM.
2013-10-09    nr      Replace banned memory functions with safer versions.
2013-07-22    rp      Add new EFS API to store data in SuperBlock.
2013-06-19    dks     Added NAND Flash scrub support.
2013-05-07    wek     Add EFS2 Read Only support.
2013-01-18    wek     Allow multiple devices to be defined in one build.
2013-01-11    dks     Move writing to Golden partition outside of efs_sync.
2012-12-14    wek     Implement modified efs_utime for item files.
2012-12-18    dks     Update golden copy strategy for new rmts layout.
2012-11-16    wek     Optimize chown to write only when uid and gid change.
2012-11-05    wek     Introduce new item file, created with efs open.
2012-10-22    wek     Convert item files to regular files.
2012-10-18    wek     Make efs_raw_put/get wrappers around efs_put/get.
2012-09-28    wek     Fix err_fatal by calling efs_chown on item file.
2012-09-28    wek     Implement write and truncate for item files.
2012-09-27    wek     Check for NULL device name when getting dev attributes.
2012-09-19    wek     Use parent dir uid and gid for item files and symlinks.
2012-09-14    wek     Allow efs_read to work on item files.
2012-09-12    wek     Fix error in setting the directory size for item files.
2012-08-30    wek     Allow zero size item files.
2012-08-20    wek     On chmod check if mode bits changed.
2012-08-14    wek     Implement chmod for item files.
2012-05-25    nr      Allow RAMFS and RMTS to have configurable page size.
2012-05-16    nr      Make MNAND cluster/page size configurable at runtime.
2012-03-30    rp      Optimize Item-file write while overwrite with same data.
2012-03-22    wek     Fix stat returning incorrect values for links and items
2012-03-20    wek     Cleanup customer header file inclusion.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2012-03-08    wek     Rename macros and remove header files to compile for boot
2011-11-02    wek     Use library supplied strl cpy and cat instead of fs_strl*
2011-10-24    vm      Klocwork error fixes
2011-10-11    nr      Added borrow_ram support to borrow ram from lower layers.
2011-06-09    wek     Change assert to err_fatal for invalid cluster size.
2011-04-04    wek     Calculate Max Transaction on init time.
2011-03-08    rp      Add support to lock EFS.
2010-11-18    rp      Support TAR image Golden-Copy for RMTS.
2010-11-16    vr      Remove dependency on flash for non-flash based targets.
2010-11-12    wek     Make efs2 vnode layer independent of page size.
2010-11-08    wek     Remove impractical and unused paranoia module.
2010-11-03    wek     Make DB page size independent.
2010-10-29    wek     Fix errors when renaming item files.
2010-10-20    wek     Make buffer layer independent of page size macro.
2010-09-01    wek     Remove compile time calculations based on page size.
2010-07-30    wek     Dynamically allocate all buffers dependent on page size.
2010-05-13    wek     Make sync_no_wait non blocking.
2010-05-10    dks     Added change to support EFS on SPI-NOR devices.
2010-05-07    rp      Added Ram filesystem.
2010-05-06    wek     Implement golden copy for remote storage page manager.
2010-04-19    yog     Added support for files having max filesize upto 4GB.
2010-04-12    wek     Added 8K page support.
2010-01-19    vr      Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-12-18    vr      Added set_volume_name and get_volume_name APIs.
2009-11-17    vr      Return EXDEV error for all renames across mountpoints.
2009-11-08    yg      Fix on target compile error.
2009-11-04    dks     Added get_device_attr, get_fs_data, image_prepare ops.
2009-08-11    wek     Pass void pointer instead of char pointer on start.
2009-06-30    yg      Correct the Tag info for info cluster.
2009-05-18    rp      Fix compiler warning for 8200 which has gcc with more -Ws
2009-04-03    wek     Added utime ops and updated getstat to set atime.
2009-03-30    wek     Added gid and uid to symlink and item getstat ops.
2009-01-09    ebb     Renamed inode function calls.
2008-12-12    rp      Introducing Managed Nand.
2008-12-02    yog     Assign value to st_rdev param for all filetypes.
2008-11-03    rp      Introduce device abstraction.
2008-10-27    yg      Fixed issue with count in parent for rename item and sym
2008-10-17    yog     Included msg.h for usgae of macro MSG_HIGH.
2008-10-06    np      Added shred functionality to delete obsolete user data.
2008-05-13    ebb     Restructured EFS_PARTITION_NAME definition.
2008-01-24    sch     Added getsize functions to vnode ops
2007-12-28    rp      Enhancements to the statvfs struct.
2007-12-28    yg      Updated with new hint and tag names.
2007-12-27    yg      Removed memory protection related code
2007-12-05    rp      Moved FS_PARANOIA_CHECKS feature to fs_paranoia.c
2007-11-16    sh      Removed timetest2 reference
2007-10-26    rp      Corrected featurization of FS_PARANOIA_CHECKS
2007-10-18    rp      Featurized QEFS2.
2007-09-20    sh      Test for removed feature FEATURE_EFS_RAMDISK
2007-08-31    sh      Add explicit NULL check in rename (for Klokwork)
2007-08-19    rp      FS_GLOBAL_LOCK() cleanup.
2007-08-20    umr     Converted level_size, span_a, span_b to uint64.
2007-08-18    rp      struct fs_vnode cleanup.
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2007-07-30    sh      Rename confusing local fs_buf variable.
2007-07-30    sh      Allow sharing of the data buffers with factory startup.
2007-07-01    sch     Changed the tag type from INODE to DATA in uio_start_io
2007-06-29    sch     Passed in data tag to write_wire
2007-06-22    sch     Pass hint to write_wire to read cluster from flash
2007-05-25    umr     Added support for EFS memory protection feature.
2007-05-15    sch     Implemented symlink renaming
2007-05-11    umr     Added stack check on simulator build
2007-04-24    sch     Implemented directory renaming
2007-04-05    sh      Remove FLASH_NAND_XTRN_PARTI_TBL references
2007-03-02    dlb     Another compilation fix.
2007-02-27    yg      Fixed Compile error
2007-02-22    dlb     Support unified flash API.
2007-01-12    dlb     Use safer string functions.
2006-12-21    sh      Implement FIFO and Socket file nodes.
2006-12-14    sh      Lint fixes II.
2006-12-13    sh      Lint fixes.
2006-12-05    dlb     Max transaction size configurable.
2006-11-15    dlb     Zero structures in unit test environment.
2006-11-11    dlb     Fix item file mode retrieval.
2006-11-11    sh      Block chmod on item files.
2006-11-10    sh      Add chmod handling.
2006-11-06    dlb     Delete recovery support.
2006-10-10    dlb     Partial transaction flush support.
2006-10-02    dlb     More efficient file deletion.
2006-09-29    dlb     Support info block upgrades.
2006-09-06    sh      Lint fixes in rename_item()
2006-08-31    dlb     Fix read_wire leak during startup.
2006-07-19    dlb     Move transaction limits down to each FS.
2006-07-06    dlb     Remove 'size' field from vnode.
2006-06-27    dlb     Pass stat through readdir.
2006-06-19    nrs     Allow renames on different size items
2006-05-31    dlb     Fix problem with multiple iterators.
2006-05-07    sh      Fixed hardalloc vs -1 compiler warning
2006-05-05    dlb     Include cleanup.
2006-04-13    dlb     Add very long filename support.
2006-03-24    sh      Moved MAX mount count into common header.
2006-03-14    dlb     UIO fix for non-512 byte pages.
2006-03-02    sh      Removed FS_FIELD_LIMIT_DIFF
2006-03-02    sh      Wrapped symlink creation with TRY
2006-01-26    sh      Added TIMETEST2 support
2006-01-11    nrs     Fixed Copyright header
2005-12-22    dlb     Fill in extra query fields in statvfs.
2005-12-09    dlb     Leaky try in raw_put.
2005-12-06    sh      Added FS_PARANOIA_CHECKS to detect memory corruption.
2005-11-28    nrs     Merge 7500 changes
2005-10-26    sh      Lint cleanup.
2005-10-18    nrs     Cleaned up code, place GID and UID in correct initializer
2005-10-13    dlb     Check symlink target against proper length limit.
2005-10-11    nrs     Remove FS_ALLOC_SOFT and FS_ALLOC_HARD
2005-10-08    nrs     Allocate inodes from the free pool before reservation
2005-10-06    nrs     Prevent reservations and quotas assigned invalid groups
2005-09-21    nrs     Add checks for quota limit exceeded
2005-09-13    nrs     Implemented reservation management for efs_chown ()
2005-08-30    dlb     Fix mode type on mknod.
2005-08-22    sh      Added device-special files.
2005-08-25    nrs     Implent buffer accounting for quotas and reservations
2005-08-05    sh      Fixed accidentally non-negative ENOSPC return value.
2005-08-05    sh      Item get must now have a matched size.
2005-07-26    nrs     Implemented set and get for reservations and quotas
2005-07-19    nrs     Implemented UID and GID inheritance on file creation
2005-07-15    nrs     Added efs_chown
2005-07-13    nrs     Added owner id and group id to stat implementation
2005-07-13    dlb     Mountpoint cleanup.
2005-07-12    dlb     Move transactions out of mount.
2005-07-05    dlb     Fix vnode rollback problem on write.
2005-07-01    dlb     Export a few additional fields from driver level.
2005-06-16    nrs     Fix rename to same name
2006-06-15    nrs     Sanity check added for rename
2005-06-09    sh      Added test for nlink overflow.
2005-05-26    sd      Compilation fixes for L4.
2005-04-26    dlb     Add 2K page support.
2005-04-12    dlb     Correct spelling of old driver header.
2005-04-05    dlb     Support old driver architecture for NOR.
2005-03-29    dlb     Clear vp in rmdir, unlink.
2005-03-28    dlb     Move unref of mkdir vp up one level.
2005-03-23    dlb     Add support for root as ramdisk.
2005-03-19    nrs     Prevention of allocation beyond soft limit
2005-03-17    dlb     Correct truncate of sparse file.
2005-02-17    dlb     Move transactions down into mkdir.
2005-02-14    dlb     Use all inodes available in each cluster.
2005-02-03    dng     Added code to m_start function to create FOTA
                      partition.  If FOTA feature is turned on and
                      no partition table existed.
2005-01-27    dlb     Allow standalone builds.
2005-01-17    dlb     Cleanup from code review.
2005-01-04    dlb     Initialize fs_buf before use by factory image code.
2004-12-30    dlb     Utilize new factory image code.
2004-11-05    dlb     Fix dirty buffers in uio writes.
2004-11-02    dlb     Move all transactions into here.
2004-10-26    dlb     Fix uio for write at full to use rollback.
2004-10-25    nrs     Fix return value of v_efs_write
2004-10-22    nrs     Added TRY block to v_efs_write
2004-10-21    nrs     Added efs_write_helper() and changed v_efs_write to
                      to use transactions.
2004-10-21    drh     Throw fatal error here when partition is not found.
2004-10-18    drh     Conditionally compile out creation of partition table
                      when it is created and programmed externally.
2004-10-13    dlb     Fixed flush outside of xact on unlink.
2004-10-11    nrs     Changed rename to call efs_unlink_helper if file exists.
2004-10-11    nrs     Added transaction start and stop to efs_unlink.
2004-10-07    dlb     Fix unreferenced vnode on file creation.
2004-10-07    dlb     Whitespace cleanup.
2004-07-19    dlb     Restructure vnode interface.
2004-04-26    drh     Remove conditionals for UNIFORM_FLASH_DRIVER
2004-04-23    drh     Back out one erroneous name change from "fs" to "flash"
2004-04-10    drh     Correct some wrong comments
2004-04-08    dlb     Rename FS_NAND... to FLASH_NAND...
2004-04-06    drh     Add support for creating a new partition if on open,
                      one does not already exist.
2004-02-03    drh     Add support for uniform flash device driver and
                      partition table open
2004-01-14    adm     Modified factory start get data to map cluster
                      information in bits.
2003-12-04    dlb     Handle vnode exhaustion better.
2003-10-17     gr     Deleted unnecessary code.
2003-09-09    dlb     Factory image works with bitmap and tree.
2003-08-20    dlb     Use tree deallocate.
2003-08-20    dlb     Use direct inode addresses.
2003-08-20    dlb     Remove delayed truncate code.
2003-07-25    bc      Added a inode flush in v_efs_truncate for delayed
                      truncation to prevent the file inode update from
                      being in a different transaction from the truncation
                      inode update.
2003-06-30    gr      Moved deletion from database to the end of v_efs_unlink
                      to get rid of a potential space leak and an
                      inconsistency.
                      Deleted a call to FS_ERR as this condition can occur
                      legitimately during operation.
2003-06-26    dlb     Change globals for easier unit test.  Fixed problem
                      with factory dump not dumping last cluster.
2003-06-12    adm     Add support for factory start. Clean up code.
2003-06-12    cr      v_efs_truncate() might be called from efs_open(), but
                      the inode copy to delayed truncate data pool was only
                      happening in v_efs_unlink().
2003-06-12    cr      Add rex_timed_wait to yield processor so that delayed
                      truncation task doesn't cause any lower priority task
                      to dog timeout.
2003-06-05    cr      Fix large file deletion across reset.
2003-05-28    jkl     Added call to efs_reset.
2003-05-23    cr      Various bug fixes and completion of delayed truncate
                      restart replay. Bump EFS_INFO_VERSION number resulting
                      in fatal error for version mismatch if fresh start is
                      not done.
2003-04-22    cr      Add delayed truncation restart to prevent device storage
                      space leak on untimely powerdown. Add FS_TRACE(). Remove
                      call to rex_wait() in truncate_freer() between
                      unlock/lock. Add threshold below which files are
                      immediately truncated rather than delaying truncate.
                      Fix tab/space.
2003-04-21    gr      Deleted inclusion of fs_buffer_nand.h, which no longer
                      exists.
2003-04-15    cr      Added delayed truncation implementation to fix CR#28410.
2003-04-04    bgc     Added initializer for transaction depth to m_start.
2003-02-28    jkl     Clean up code.
2003-02-24    bgc     Fixed initialization of v_efs_truncate.
2003-02-13    jkl     Merged NOR and NAND support.
2002-12-16     gr     Made changes needed to support both NAND and NOR flash.
2002-10-08    dlb     Renamed to efs2.
2002-08-08    drh     Created by dlb.  Added history header.

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QEFS2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>

#include "assert.h"
#include "fs_sys_types.h"

#include "fs_err.h"
#include "fs_efs2.h"
#include "fs_errno.h"
#include "fs_vnode.h"
#include "fs_db.h"
#include "fs_pm.h"
#include "fs_public.h"
#include "fs_util.h"
#include "fs_protected_memory.h"
#include "fs_device.h"
#include "fs_pm_types.h"
#include "fs_mnand_pm.h"
#include "fs_rmts_pm.h"
#include "fs_ramfs_pm.h"
#include "fs_malloc.h"
#include "fs_priv_funcs.h"
#include "stringl/stringl.h"

#ifdef FEATURE_EFS_NAND_FACTORY_START
#include "fs_factory.h"
#endif

/* For debugging, assure that there is never an operation requested outside
 * of the bounds of a transaction. */
#ifdef FS_UNIT_TEST
# define XACT_CHECK(mp) \
    do { \
      if ((mp)->transaction_depth <= 0) { \
        FS_ERR_FATAL ("Performing operation outside of transaction", \
            0, 0, 0); \
      } \
    } while (0)
#else
# define XACT_CHECK(mp) \
   do { } while (0)
#endif

/**********************************************************************
 * Inode allocation strategy.
 *
 * EFS pre-determined inodes (such as root) are allocated dynamically on
 * initialization, and placed in the appropriate fields in the info
 * cluster. */

/* For marking invalid inodes.  This needs to fit in the fs_inode_t type. */
#define FS_INODE_INVALID        0xFFFFFFFFu

/***********************************************************************
 * EFS Info block.
 *
 * The info block describes core information about EFS.
 */

#define EFS_INFO_MAGIC          0xa7b93ea0u

/* The info cluster is versioned similar to the superblock.  Version
 * changes here can also be forward compatible.  Code in m_start determines
 * if an upgrade is needed. */

#define EFS_INFO_VERSION_LEGACY  5  /* Prior to the possibility of in-place
                                       upgrade of the info block. */

#define EFS_INFO_VERSION_DELETE  6  /* Add support for fast-delete. */

#define EFS_INFO_VERSION EFS_INFO_VERSION_DELETE

/* Offsets in info structure for each upgrade. */
#define EFS_INFO_OFFSET_PRE_DELETE \
  (offsetof (struct efs_info_data, partial_delete))

typedef struct efs_info_data *efs_info_t;

#ifdef FS_UNIT_TEST
void fs_integrity_check (void);
#endif

static unsigned tree_deallocate (struct fs_mount_efs *mp, fs_cluster_id id,
    unsigned level, unsigned desired_level, unsigned gid,
    unsigned is_recovery);

static int m_start_read_only (struct fs_mount *mpp, const void *args);
static int m_start (struct fs_mount *mpp, const void *args);
static int m_stop (struct fs_mount *mpp);
static int m_creat_root (struct fs_mount *mpp);
static int m_get_root (struct fs_mount *mpp, struct fs_vnode **rp);
static struct fs_vnode *m_create_inode (struct fs_mount *mpp, fs_mode_t mode,
    uint32 gid, uint32 uid);
static int m_write_inode (struct fs_mount *mpp, struct fs_vnode *vp);
static int m_read_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_inode_t inum);
static int m_read_named_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_mode_t mode, const char *key, int key_len);
static void m_start_transaction (struct fs_mount *mpp);
static void m_end_transaction (struct fs_mount *mpp);
static void m_rollback_transaction (struct fs_mount *mpp);

static struct fs_mount_ops efs_ops;

static int v_efs_lookup (struct nameidata *ndp);
static int v_efs_create (struct nameidata *ndp, fs_mode_t mode);
static int v_efs_link (struct nameidata *ndp);
static int v_efs_unlink (struct nameidata *ndp);
static int efs_unlink_helper (struct nameidata *ndp);
static fs_ssize_t v_efs_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode);
static fs_ssize_t efs_write_helper (struct fs_vnode *vp, fs_off_t pos,
    const void *buf, fs_size_t count);
static fs_ssize_t v_efs_item_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode);
static fs_ssize_t v_efs_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count);
static fs_ssize_t v_efs_item_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count);
static int v_efs_truncate (struct fs_vnode *vp, fs_size_t pos);
static int v_efs_item_truncate (struct fs_vnode *vp, fs_size_t pos);
static int v_efs_mkdir (struct nameidata *ndp, fs_mode_t mode);
static int v_efs_rmdir (struct nameidata *ndp);
static void *v_efs_opendir (struct fs_vnode *vp);
static int v_efs_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent);
static int v_efs_closedir (struct fs_vnode *vp, void *dir);
static int v_efs_getstat (struct fs_vnode *vp, struct fs_stat *buf);
static int v_efs_rename (struct nameidata *ndp, struct nameidata *ndp2);
static int v_efs_symlink (struct nameidata *ndp, const char *dest);
static int v_efs_readlink (struct fs_vnode *vp, char *buf, fs_size_t bufsiz);
static int v_efs_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf);
static int v_efs_symlink_getstat (struct fs_vnode *vp, struct fs_stat *buf);
static int v_efs_item_getstat (struct fs_vnode *vp, struct fs_stat *buf);
static int v_efs_devspecial_getstat (struct fs_vnode *vp, struct fs_stat *buf);
static int v_efs_put_item (struct nameidata *ndp,
    void *data, int length, int mode);
static int v_efs_get_item (struct fs_vnode *dvp, void *data, int length);
static int v_efs_mknod (struct nameidata *ndp, fs_mode_t mode,
    fs_devspecial_t dev);

static int v_efs_chmod (struct nameidata *ndp, fs_mode_t mode);
static int v_efs_item_chmod (struct nameidata *ndp, fs_mode_t mode);
static int v_efs_chown (struct fs_vnode *vp, int uid_val, int gid_val);
static int v_efs_item_chown (struct fs_vnode *vp, int uid_val, int gid_val);
static int v_efs_set_reservation (struct fs_vnode *vp, uint32 gid,
    fs_size_t size);
static int v_efs_set_quota (struct fs_vnode *vp, uint32 gid, fs_size_t size);
static int v_efs_get_group_info (struct fs_vnode *vp, uint32 gid,
    struct fs_group_info *ginfo);
static int v_efs_shred (struct fs_vnode *vp);
static int v_efs_scrub (struct fs_vnode *vp);
static int v_efs_utime (struct fs_vnode *vp, const struct fs_utimbuf *times);
static int v_efs_item_utime (struct fs_vnode *vp,
                             const struct fs_utimbuf *times);
static int v_efs_item_utime_item (struct fs_vnode *vp,
                                  const struct fs_utimbuf *times);
static int v_efs_sync_start (struct fs_vnode *vp);
static int v_efs_sync_wait (struct fs_vnode *vp);
static int v_efs_sync_end (struct fs_vnode *vp);
static int v_efs_sync_no_wait (struct fs_vnode *vp,
                               fs_sync_token_t *sync_token);
static int v_efs_sync_get_status (struct fs_vnode *vp,
                fs_sync_token_t sync_token, enum fs_sync_result *sync_result);
static int v_efs_get_device_attr (struct fs_vnode *vp,
    struct fs_device_attr *dev_attr);
static int v_efs_get_fs_data (struct fs_vnode *vp,
    struct fs_factimage_read_info *read_info_ptr, byte *page_buffer);
static int v_efs_image_prepare (struct fs_vnode *vp);
static int v_efs_make_golden_copy (struct fs_vnode *vp);
static int v_efs_fs_init_complete (struct fs_vnode *vp);
static int v_efs_lock_efs (struct fs_vnode *vp);
static int v_efs_borrow_ram (struct fs_vnode *vp, void **ram_addr,
                             uint32 *ram_size);
static int v_efs_store_data_in_sb (struct fs_vnode *vp, const void *data,
                                   uint32 data_size);


static struct vfs_ops efs_dir_vops;
static struct vfs_ops efs_file_vops;
static struct vfs_ops efs_symlink_vops;
static struct vfs_ops efs_item_vops;
static struct vfs_ops efs_devspecial_vops;

#include "fs_buffer.h"

#ifdef FS_STANDALONE
  #include "ut_utils.h"
#endif  /* FS_STANDALONE */

/* Device structure pointer to public form of device structure */
static efs_device_t fs_device = NULL;

static struct fs_mount_efs efs_mounts[FS_MAX_EFS_MOUNTS];
/* Query information covertly handed off to the fs_diag interface, until
 * the real query function is implemented.  0xFFFFFFFF indicates this entry
 * is not real. */
static uint32 efs2_total_blocks = 0xFFFFFFFF;
static uint32 efs2_bus_width = 0xFFFFFFFF;

/* Buffer data structure */
static struct fsb_nand_data static_fs_buf;
fsb_t fs_buf;

/* Page Manager Collection structure */
fs_pm_t fs_pm;

static FS_ALIGN uint32 freemap_bitmap[FS_BUFFER_FREE_BITMAP_UINT32_COUNT];

#if defined (FEATURE_EFS_EFS2_ON_NAND) ||     \
    defined (FEATURE_EFS_EFS2_ON_NOR)  ||     \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
  static struct fs_pm_flash_data static_fs_pm;
#endif


/* Static name buffer for EFS2. */
static struct nameidata efs2_name_buffer;

void
fs_efs2_init_aligned_buf (uint32 cluster_size)
{
  int i;

  /* In the heap we have reserved a max of FS_MAX_PAGE_SIZE byte buffers. Make
   * sure we are not requesting more space than what is assigned. */
  if (cluster_size > FS_MAX_PAGE_SIZE)
    FS_ERR_FATAL ("%d>%d Cluster larger than max supported size",
                  cluster_size, FS_MAX_PAGE_SIZE, 0);

  /*---------------------------------------------------------------------------
   * Because we have to align the buffers that we pass to the DMA, the buffers
   * cannot be present inside the structures any more as ARM-compiler can only
   * align variable-instances (not definitions). So we have made the buffers
   * as pointers inside the structure-defintion and declared and aligned the
   * actual buffer arrays as global variables. In this function, we hook-up
   * the pointers with their corresponding aligned arrays.
   * ------------------------------------------------------------------------*/
  ASSERT ((cluster_size % FS_DM_ALIGN_BYTES) == 0);
  for (i = 0; i < FS_BUFFER_NUMBER_BUFFERS; i++) {
    static_fs_buf.buffers[i] = fs_align_malloc (cluster_size);
    ASSERT (FS_OKAY_FOR_DM (static_fs_buf.buffers[i]));
  }

  static_fs_buf.freemap.bitmap = freemap_bitmap;
  ASSERT (FS_OKAY_FOR_DM (static_fs_buf.freemap.bitmap));

#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)  ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
  static_fs_pm.realloc_buffer = fs_align_malloc (cluster_size);
  ASSERT (FS_OKAY_FOR_DM (static_fs_pm.realloc_buffer));

  static_fs_pm.log.buffer = fs_align_malloc (cluster_size);
  ASSERT (FS_OKAY_FOR_DM (static_fs_pm.log.buffer));
#endif
}

/***********************************************************************/
/* Direct inodes.  Inode values used in EFS2 refer to an absolute cluster
 * address of that inode.  Enough bits in the lower portion of the address
 * indicate the offset within a cluster of the inode.  The rest of the bits
 * indicate a cluster offset.  This does slightly reduce the number of
 * clusters available for the filesystem (limiting the ultimate size of the
 * device used). */


static void
efs2_init_vops (void)
{
  /* Initalize directory vops. */
  fs_vnode_init_invalid_vops (&efs_dir_vops);
  efs_dir_vops.lookup           = v_efs_lookup;
  efs_dir_vops.create           = v_efs_create;
  efs_dir_vops.link             = v_efs_link;
  efs_dir_vops.unlink           = v_efs_unlink;
  efs_dir_vops.mkdir            = v_efs_mkdir;
  efs_dir_vops.rmdir            = v_efs_rmdir;
  efs_dir_vops.opendir          = v_efs_opendir;
  efs_dir_vops.readdir          = v_efs_readdir;
  efs_dir_vops.closedir         = v_efs_closedir;
  efs_dir_vops.getstat          = v_efs_getstat;
  efs_dir_vops.rename           = v_efs_rename;
  efs_dir_vops.symlink          = v_efs_symlink;
  efs_dir_vops.getstatvfs       = v_efs_getstatvfs;
  efs_dir_vops.put_item         = v_efs_put_item;
  efs_dir_vops.mknod            = v_efs_mknod;
  efs_dir_vops.chmod            = v_efs_chmod;
  efs_dir_vops.chown            = v_efs_chown;
  efs_dir_vops.set_reservation  = v_efs_set_reservation;
  efs_dir_vops.set_quota        = v_efs_set_quota;
  efs_dir_vops.get_group_info   = v_efs_get_group_info;
  efs_dir_vops.shred            = v_efs_shred;
  efs_dir_vops.scrub            = v_efs_scrub;
  efs_dir_vops.utime            = v_efs_utime;
  efs_dir_vops.utime_item       = v_efs_utime;
  efs_dir_vops.sync_start       = v_efs_sync_start;
  efs_dir_vops.sync_wait        = v_efs_sync_wait;
  efs_dir_vops.sync_end         = v_efs_sync_end;
  efs_dir_vops.sync_no_wait     = v_efs_sync_no_wait;
  efs_dir_vops.sync_get_status  = v_efs_sync_get_status;
  efs_dir_vops.get_device_attr  = v_efs_get_device_attr;
  efs_dir_vops.get_fs_data      = v_efs_get_fs_data;
  efs_dir_vops.image_prepare    = v_efs_image_prepare;
  efs_dir_vops.make_golden_copy = v_efs_make_golden_copy;
  efs_dir_vops.fs_init_complete = v_efs_fs_init_complete;
  efs_dir_vops.lock_efs         = v_efs_lock_efs;
  efs_dir_vops.borrow_ram       = v_efs_borrow_ram;
  efs_dir_vops.store_data_in_sb = v_efs_store_data_in_sb;

  /* Initalize file vops. */
  fs_vnode_init_invalid_vops (&efs_file_vops);
  efs_file_vops.write            = v_efs_write;
  efs_file_vops.read             = v_efs_read;
  efs_file_vops.truncate         = v_efs_truncate;
  efs_file_vops.getstat          = v_efs_getstat;
  efs_file_vops.getstatvfs       = v_efs_getstatvfs;
  efs_file_vops.chmod            = v_efs_chmod;
  efs_file_vops.chown            = v_efs_chown;
  efs_file_vops.set_reservation  = v_efs_set_reservation;
  efs_file_vops.set_quota        = v_efs_set_quota;
  efs_file_vops.get_group_info   = v_efs_get_group_info;
  efs_file_vops.shred            = v_efs_shred;
  efs_file_vops.scrub            = v_efs_scrub;
  efs_file_vops.utime            = v_efs_utime;
  efs_file_vops.utime_item       = v_efs_utime;
  efs_file_vops.sync_start       = v_efs_sync_start;
  efs_file_vops.sync_wait        = v_efs_sync_wait;
  efs_file_vops.sync_end         = v_efs_sync_end;
  efs_file_vops.sync_no_wait     = v_efs_sync_no_wait;
  efs_file_vops.sync_get_status  = v_efs_sync_get_status;
  efs_file_vops.get_device_attr  = v_efs_get_device_attr;
  efs_file_vops.get_fs_data      = v_efs_get_fs_data;
  efs_file_vops.image_prepare    = v_efs_image_prepare;
  efs_file_vops.make_golden_copy = v_efs_make_golden_copy;
  efs_file_vops.borrow_ram       = v_efs_borrow_ram;
  efs_dir_vops.store_data_in_sb  = v_efs_store_data_in_sb;

  /* Initalize symlink vops. */
  fs_vnode_init_invalid_vops (&efs_symlink_vops);
  efs_symlink_vops.getstat          = v_efs_symlink_getstat;
  efs_symlink_vops.readlink         = v_efs_readlink;
  efs_symlink_vops.getstatvfs       = v_efs_getstatvfs;
  efs_symlink_vops.shred            = v_efs_shred;
  efs_symlink_vops.scrub            = v_efs_scrub;
  efs_symlink_vops.sync_start       = v_efs_sync_start;
  efs_symlink_vops.sync_wait        = v_efs_sync_wait;
  efs_symlink_vops.sync_end         = v_efs_sync_end;
  efs_symlink_vops.sync_no_wait     = v_efs_sync_no_wait;
  efs_symlink_vops.sync_get_status  = v_efs_sync_get_status;
  efs_symlink_vops.get_device_attr  = v_efs_get_device_attr;
  efs_symlink_vops.get_fs_data      = v_efs_get_fs_data;
  efs_symlink_vops.image_prepare    = v_efs_image_prepare;
  efs_symlink_vops.make_golden_copy = v_efs_make_golden_copy;
  efs_symlink_vops.borrow_ram       = v_efs_borrow_ram;
  efs_dir_vops.store_data_in_sb     = v_efs_store_data_in_sb;

  /* Initalize item vops. */
  fs_vnode_init_invalid_vops (&efs_item_vops);
  efs_item_vops.write            = v_efs_item_write;
  efs_item_vops.read             = v_efs_item_read;
  efs_item_vops.truncate         = v_efs_item_truncate;
  efs_item_vops.getstat          = v_efs_item_getstat;
  efs_item_vops.getstatvfs       = v_efs_getstatvfs;
  efs_item_vops.get_item         = v_efs_get_item;
  efs_item_vops.chmod            = v_efs_item_chmod;
  efs_item_vops.chown            = v_efs_item_chown;
  efs_item_vops.set_reservation  = v_efs_set_reservation;
  efs_item_vops.set_quota        = v_efs_set_quota;
  efs_item_vops.get_group_info   = v_efs_get_group_info;
  efs_item_vops.shred            = v_efs_shred;
  efs_item_vops.scrub            = v_efs_scrub;
  efs_item_vops.utime            = v_efs_item_utime;
  efs_item_vops.utime_item       = v_efs_item_utime_item;
  efs_item_vops.sync_start       = v_efs_sync_start;
  efs_item_vops.sync_wait        = v_efs_sync_wait;
  efs_item_vops.sync_end         = v_efs_sync_end;
  efs_item_vops.sync_no_wait     = v_efs_sync_no_wait;
  efs_item_vops.sync_get_status  = v_efs_sync_get_status;
  efs_item_vops.get_device_attr  = v_efs_get_device_attr;
  efs_item_vops.get_fs_data      = v_efs_get_fs_data;
  efs_item_vops.image_prepare    = v_efs_image_prepare;
  efs_item_vops.make_golden_copy = v_efs_make_golden_copy;
  efs_item_vops.borrow_ram       = v_efs_borrow_ram;
  efs_dir_vops.store_data_in_sb  = v_efs_store_data_in_sb;

  /* Initalize devspecial vops. */
  fs_vnode_init_invalid_vops (&efs_devspecial_vops);
  efs_devspecial_vops.getstat          = v_efs_devspecial_getstat;
  efs_devspecial_vops.getstatvfs       = v_efs_getstatvfs;
  efs_devspecial_vops.chmod            = v_efs_chmod;
  efs_devspecial_vops.chown            = v_efs_chown;
  efs_devspecial_vops.get_group_info   = v_efs_get_group_info;
  efs_devspecial_vops.shred            = v_efs_shred;
  efs_devspecial_vops.scrub            = v_efs_scrub;
  efs_devspecial_vops.utime            = v_efs_utime;
  efs_devspecial_vops.utime_item       = v_efs_utime;
  efs_devspecial_vops.sync_start       = v_efs_sync_start;
  efs_devspecial_vops.sync_wait        = v_efs_sync_wait;
  efs_devspecial_vops.sync_end         = v_efs_sync_end;
  efs_devspecial_vops.sync_no_wait     = v_efs_sync_no_wait;
  efs_devspecial_vops.sync_get_status  = v_efs_sync_get_status;
  efs_devspecial_vops.get_device_attr  = v_efs_get_device_attr;
  efs_devspecial_vops.get_fs_data      = v_efs_get_fs_data;
  efs_devspecial_vops.image_prepare    = v_efs_image_prepare;
  efs_devspecial_vops.make_golden_copy = v_efs_make_golden_copy;
  efs_devspecial_vops.borrow_ram       = v_efs_borrow_ram;
  efs_dir_vops.store_data_in_sb        = v_efs_store_data_in_sb;
}

static void
efs2_init_read_only_vops (void)
{
  /* Initalize directory vops. */
  fs_vnode_init_invalid_vops (&efs_dir_vops);
  efs_dir_vops.lookup  = v_efs_lookup;
  efs_dir_vops.getstat = v_efs_getstat;

  fs_vnode_init_invalid_vops (&efs_file_vops);
  efs_file_vops.read    = v_efs_read;
  efs_file_vops.getstat = v_efs_getstat;

  fs_vnode_init_invalid_vops (&efs_symlink_vops);
  efs_symlink_vops.getstat          = v_efs_symlink_getstat;

  fs_vnode_init_invalid_vops (&efs_item_vops);
  efs_item_vops.read = v_efs_item_read;
  efs_item_vops.getstat  = v_efs_item_getstat;
  efs_item_vops.get_item = v_efs_get_item;

  fs_vnode_init_invalid_vops (&efs_devspecial_vops);
  efs_devspecial_vops.getstat = v_efs_devspecial_getstat;
}

static struct fs_vnode *
m_invalid_create_inode (struct fs_mount *mpp, fs_mode_t mode,
                        uint32 gid, uint32 uid)
{
  (void) mpp; (void) mode; (void) gid; (void) uid;

  return NULL;
}

static int
m_invalid_write_inode (struct fs_mount *mpp, struct fs_vnode *vp)
{
  (void) mpp; (void) vp;

  return -EPERM;
}


/* Initialize the mountpoint interface. */
void
fs_efs2_read_only_init (void)
{
  int i;

  /* Initialize mount point ops. */
  efs_ops.start = m_start_read_only;
  efs_ops.stop = m_stop;
  efs_ops.get_root = m_get_root;
  efs_ops.create_inode = m_invalid_create_inode;
  efs_ops.write_inode = m_invalid_write_inode;
  efs_ops.read_inode = m_read_inode;
  efs_ops.read_named_inode = m_read_named_inode;
  efs_ops.cleanup = NULL;

  for (i = 0; i < FS_MAX_EFS_MOUNTS; i++) {
    efs_mounts[i].db = &efs_mounts[i].database;
    efs_mounts[i].parent.ops = &efs_ops;

    fs_mount_register ("EFS2_read_only", &efs_mounts[i].parent);
  }

#ifdef FS_UNIT_TEST
  memset (&static_fs_buf, 0, sizeof (static_fs_buf));
  memset (&static_fs_pm, 0, sizeof (static_fs_pm));
#endif
}

/* Initialize the mountpoint interface. */
void
fs_efs2_init (void)
{
  int i;

  /* Initialize mount point ops. */
  efs_ops.start = m_start;
  efs_ops.stop = m_stop;
  efs_ops.get_root = m_get_root;
  efs_ops.create_inode = m_create_inode;
  efs_ops.write_inode = m_write_inode;
  efs_ops.read_inode = m_read_inode;
  efs_ops.read_named_inode = m_read_named_inode;
  efs_ops.cleanup = NULL;

  for (i = 0; i < FS_MAX_EFS_MOUNTS; i++) {
    efs_mounts[i].db = &efs_mounts[i].database;
    efs_mounts[i].parent.ops = &efs_ops;

    fs_mount_register ("EFS2", &efs_mounts[i].parent);
  }

#ifdef FS_UNIT_TEST
  memset (&static_fs_buf, 0, sizeof (static_fs_buf));
  memset (&static_fs_pm, 0, sizeof (static_fs_pm));
#endif

}

static efs_device_t
fs_open_root_device (enum fs_device_type *device_type)
{
  efs_device_t dev = NULL;
  enum fs_device_type dev_type;

#if defined (FEATURE_EFS_EFS2_ON_RAMFS)
  if (dev == NULL)
  {
    dev_type = FS_DEVICE_TYPE_EFS_RAMFS;
    dev = fs_device_open (dev_type, 0);
  }
#endif

#if defined (FEATURE_EFS_EFS2_ON_NAND) || \
    defined (FEATURE_EFS_EFS2_ON_NOR)  || \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
  /* Try to open the flash using NAND, it should work regardless of the
   * flash device type we gave it since they all use the same API. */
  if (dev == NULL)
  {
    dev_type = FS_DEVICE_TYPE_EFS_NAND;
    dev = fs_device_open (dev_type, 0);
  }
#endif

#if defined (FEATURE_EFS_EFS2_ON_MNAND)
  if (dev == NULL)
  {
    dev_type = FS_DEVICE_TYPE_EFS_MNAND;
    dev = fs_device_open (dev_type, 0);
  }
#endif

#if defined (FEATURE_EFS_EFS2_ON_RMTS)
  if (dev == NULL)
  {
    dev_type = FS_DEVICE_TYPE_EFS_RMTS;
    dev = fs_device_open (dev_type, 0);
  }
#endif

  /* If open returned success, update the retun value. */
  if (dev == NULL)
    *device_type = FS_DEVICE_TYPE_UNKNOWN;
  else
    *device_type = dev->efs_device_type;

  return dev;
}

static int
efs_mount_start_common (struct fs_mount_efs *mp)
{
  unsigned tmp;
  uint32 entries_per_cluster;

  mp->transaction_depth = 0;
  mp->buf = fs_buf;
  mp->cluster_size = mp->buf->cluster_size (mp->buf);
  mp->max_transaction = mp->cluster_size * FS_MAX_TRANSACTION_DATA_PAGES;

  /* Compute the shift to use with this cluster size. */
  tmp = mp->cluster_size;
  mp->cluster_shift = fs_get_shifts (tmp);

  tmp = mp->cluster_size / sizeof (fs_cluster_id);
  mp->indirect_cluster_mask = tmp - 1;
  mp->indirect_cluster_shift = fs_get_shifts (tmp);

  /* Compute the inode values. */
  tmp = mp->cluster_size / (sizeof (struct fs_inode));
  mp->inode_shift = fs_get_shifts (tmp);
  mp->inode_low_mask = ~((~0u) << mp->inode_shift);

  /* compute the direct and indirect cluster count of inode. */
  entries_per_cluster = mp->cluster_size / sizeof (fs_cluster_id);
  mp->indirect_counts[0] = FS_DIRECT_CLUSTER_COUNT;
  mp->indirect_counts[1] = entries_per_cluster;
  mp->indirect_counts[2] = entries_per_cluster * entries_per_cluster;
  /* For tripple indirect the cluster count is entries_per_cluster^3.
   * Indirect_count is 32 bits, therefore triple indirect count will
   * overflow when cluster size is ((2^32)^(1/3))*sizeof(fs_page_id) =
   * 6502. How our cluster size is a power of 2, for 8k (and above)
   * use UINT_MAX for triple indirect clusters; this sets the upper
   * limit of file size by indirect clusters to 2050GBytes, bigger
   * than what we can actually support.  */
  if (mp->cluster_size < 8192)
    mp->indirect_counts[3] = mp->indirect_counts[2] * entries_per_cluster;
  else
    mp->indirect_counts[3] = UINT_MAX;

  /* Startup the database. */
  fs_db_init (mp->db, mp->buf);

  return 0;
}

static int
m_start_read_only (struct fs_mount *mpp, const void *args)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  fs_cluster_id info_cluster;
  efs_info_t info = NULL;
  int result;
  enum fs_device_type device_type;
  uint32 cluster_size = 0;

  (void) args;

  efs2_init_read_only_vops ();

#if !defined (FEATURE_EFS_EFS2_ON_NAND)    && \
    !defined (FEATURE_EFS_EFS2_ON_NOR)     && \
    !defined (FEATURE_EFS_EFS2_ON_SPI_NOR) && \
    !defined (FEATURE_EFS_EFS2_ON_MNAND)   && \
    !defined (FEATURE_EFS_EFS2_ON_RMTS)    && \
    !defined (FEATURE_EFS_EFS2_ON_RAMFS)
  #error "Unknown device type. Please configure desired device type."
#endif

  fs_device = fs_open_root_device (&device_type);
  if (fs_device == NULL)
    return -1;

  cluster_size = fs_device_page_size (fs_device);
  fs_efs2_init_aligned_buf (cluster_size);

  switch (device_type)
  {
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)  ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
    case FS_DEVICE_TYPE_EFS_NOR:
    case FS_DEVICE_TYPE_EFS_SPI_NOR:
      fs_pm = fs_pm_read_only_init (&static_fs_pm, fs_device);
      break;
#endif

    default:
      fs_pm = NULL;
      break;
  }

  if (fs_pm == NULL)
    return -1;

  fs_buf = &static_fs_buf.imp;
  result = fsb_buffer_init_read_only (&static_fs_buf, fs_pm, cluster_size);
  fs_buf = &static_fs_buf.imp;
  if (result != 0)
    return result;

  result = efs_mount_start_common (mp);
  if (result != 0)
    return result;

  result = -1;
  /* Lookup the info block and determine if this is a fresh start. */
  info_cluster = mp->buf->get_field (mp->buf, FS_FIELD_FS_INFO);
  if (info_cluster != FS_INVALID_CLUSTER_ID)
  {
    info = mp->buf->read_wire (mp->buf, info_cluster);

    /* Check if info cluster is OK or needs an upgrad. If upgrade needed
     * return error. Upgrades not supported on read only mode */
    if (info->magic == EFS_INFO_MAGIC && info->version == EFS_INFO_VERSION)
    {
      info = mp->buf->read_wire (mp->buf, info_cluster);
      mp->info = *info;
      mp->buf->free_wire (mp->buf, info_cluster);
      mp->info_cluster = info_cluster;

      mp->root_dir = fs_vnode_lookup (&mp->parent, mp->dev, info->root);
      if (mp->root_dir != NULL)
      {
        result = 0;
      }
    }
  }

  return result;
}

/***********************************************************************
FUNCTION      m_start

DESCRIPTION   This function helps initialize the mount point by
              initializing the file system and the DB tree

RETURN VALUE  0 if mount was sucessful
**********************************************************************/
static int
m_start (struct fs_mount *mpp, const void *args)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  efs_info_t info = NULL;
  fs_cluster_id info_cluster;
  fs_cluster_id tmp_clust;
  int result;
  enum fs_device_type device_type;
  uint32 cluster_size = 0;

  (void) args;

  efs2_init_vops ();

#if !defined (FEATURE_EFS_EFS2_ON_NAND)    && \
    !defined (FEATURE_EFS_EFS2_ON_NOR)     && \
    !defined (FEATURE_EFS_EFS2_ON_SPI_NOR) && \
    !defined (FEATURE_EFS_EFS2_ON_MNAND)   && \
    !defined (FEATURE_EFS_EFS2_ON_RMTS)    && \
    !defined (FEATURE_EFS_EFS2_ON_RAMFS)
  #error "Unknown device type. Please configure desired device type."
#endif

  fs_device = fs_open_root_device (&device_type);
  if (fs_device == NULL)
    return -1;

  cluster_size = fs_device_page_size (fs_device);
  fs_efs2_init_aligned_buf (cluster_size);

  switch (device_type)
  {
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)  ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
    case FS_DEVICE_TYPE_EFS_NOR:
    case FS_DEVICE_TYPE_EFS_SPI_NOR:
      fs_pm = &static_fs_pm.parent;
      fs_pm_init (&static_fs_pm, fs_device);
      break;
#endif

#ifdef FEATURE_EFS_EFS2_ON_MNAND
    case FS_DEVICE_TYPE_EFS_MNAND:
      fs_pm = fs_mnand_pm_init (fs_device);
      break;
#endif

#ifdef FEATURE_EFS_EFS2_ON_RMTS
    case FS_DEVICE_TYPE_EFS_RMTS:
      fs_pm = fs_rmts_pm_init (fs_device);
      break;
#endif

#ifdef FEATURE_EFS_EFS2_ON_RAMFS
    case FS_DEVICE_TYPE_EFS_RAMFS:
      fs_pm = fs_ramfs_pm_init (fs_device);
      break;
#endif

    default:
      fs_pm = NULL;
      break;
  }

  if (fs_pm == NULL)
    return -1;

  fs_buf = &static_fs_buf.imp;
  fsb_buffer_initialize (&static_fs_buf, fs_pm, cluster_size);
  fs_buf = &static_fs_buf.imp;


  result = efs_mount_start_common (mp);
  if (result != 0)
    return result;

  /* Lookup the info block and determine if this is a fresh start. */
  info_cluster = mp->buf->get_field (mp->buf, FS_FIELD_FS_INFO);

  /* Wire this down. */
  if (info_cluster != FS_INVALID_CLUSTER_ID) {
    unsigned pad_offset;

    info = mp->buf->read_wire (mp->buf, info_cluster);

    /* Check for an upgrade from an earlier version.  Code supports
     * upgrades that simply extend the structure, by padding with zeros. */
    if (info->magic == EFS_INFO_MAGIC) {
      switch (info->version) {
        case EFS_INFO_VERSION:
          /* Current version, nothing to do. */
          pad_offset = mp->cluster_size;
          break;

        case EFS_INFO_VERSION_LEGACY:
          /* Bump from version 5. */
          pad_offset = EFS_INFO_OFFSET_PRE_DELETE;
          break;

        default:
          /* Invalid version.  Flag as "full" upgrade. */
          pad_offset = 0;
      }
    } else {
      /* Magic number is wrong, do full upgrade. */
      pad_offset = 0;
    }

    /* Check for a full upgrade needed. */
    if (pad_offset == 0) {
      efs_reset();
      FS_ERR_FATAL ("Version number change. Erase Flash", 0, 0, 0);
      /* We should reset the buffer allocation information, and then
       * startup as if nothing was needed. */
    }

    /* If an upgrade is needed, perform it in place.  This spends a
     * transaction, but will only happen once per upgrade. */
    if (pad_offset < mp->cluster_size) {
      uint8 *buf;
      m_start_transaction (mpp);
      buf = mp->buf->write_wire (mp->buf, info_cluster, FS_BUF_READ_CLUSTER,
                                 FS_DATA_TAG_INFO);
      memset (buf + pad_offset, 0, mp->cluster_size - pad_offset);
      mp->buf->free_wire (mp->buf, info_cluster);
      m_end_transaction (mpp);
    } else {
      /* Otherwise, just free the read wired cluster. */
      mp->buf->free_wire (mp->buf, info_cluster);
    }
  }

  /* Initialize the FS if necessary.  The info pointer is not valid at this
   * point, but will be non-null if there was one. */
  if (info == NULL) {
    m_start_transaction (mpp);
    result = mp->buf->allocate (mp->buf, 0, &info_cluster);
    if (result < 0)
    {
      FS_ERR_FATAL ("No space for filesystem.", 0, 0, 0);
    }
    info = mp->buf->write_wire (mp->buf, info_cluster,
                                FS_BUF_DO_NOT_READ_CLUSTER, FS_DATA_TAG_INFO);
    mp->buf->set_field (mp->buf, FS_FIELD_FS_INFO, info_cluster);

    memset (info, 0, sizeof (struct efs_info_data));

    info->magic = EFS_INFO_MAGIC;
    info->version = EFS_INFO_VERSION;
    /* info->inode_levels = FS_MOUNT_INODE_TREE_DEPTH; */
    info->inode_top = FS_INVALID_CLUSTER_ID;

    /* Allocate the new cluster for the nodes. */
    result = mp->buf->allocate (mp->buf, 0, &tmp_clust);
    if (result < 0)
      FS_ERR_FATAL ("Unable to allocate any inodes", 0, 0, 0);
                        /* Creating root will write to it. */
    info->root = tmp_clust << mp->inode_shift;
    info->inode_next = info->root + 1;  /* Assumes more than 1 inode. */
    info->inode_free = FS_INODE_INVALID;


    mp->buf->free_wire (mp->buf, info_cluster);

    mp->info = *info;

    mp->info_cluster = info_cluster;

    m_creat_root (mpp);

    m_end_transaction (mpp);
  } else {
    info = mp->buf->read_wire (mp->buf, info_cluster);
    mp->info = *info;
    mp->buf->free_wire (mp->buf, info_cluster);
    mp->info_cluster = info_cluster;

    mp->root_dir = fs_vnode_lookup (&mp->parent, mp->dev, info->root);
    if (mp->root_dir == NULL) {
      FS_ERR_FATAL ("Unable to find root directory", 0, 0, 0);
    }
  }

  /* Was there a truncate in progress? */
  if (info->partial_delete != 0) {
    /* Warning.  It is very important that the startup up until this point
     * performs no allocation.  The partial truncate recovery will look at
     * some of the clusters that might have been deleted (indirect blocks).
     * We depend on the lower-layer having not freed these blocks. */
    FS_MSG_HIGH_0 ("Recover from partially completed truncate");
    m_start_transaction (&mp->parent);
    m_end_transaction (&mp->parent);
  }

  return 0;
}

/***********************************************************************
FUNCTION      m_creat_root

DESCRIPTION   This function is called by m_start if the file system is
              not initialized. In this function a root inode is allocated
              and an inode for that is also created and inserted into
              the database. Then a vnode is allocted for this root inode
              and returned.

              This has too much knowledge of what goes on over in the vnode
              code.  What this really tells us is that all of that code
              that is over in vnode really belongs over here.

RETURN VALUE  0 if creation was sucessful
**********************************************************************/
static int
m_creat_root (struct fs_mount *mpp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  struct fs_vnode *root;
  struct nameidata *ndp = &efs2_name_buffer;

  root = m_create_inode (mpp, S_IFDIR | 0777, FS_GROUP_ZERO, FS_CURR_UID);
  if (root == NULL)
    return -1;

  root->inum = mp->info.root;
  mp->info_dirty = 1;

  mp->buf->start_transaction (mp->buf);
  ndp->dvp = root;
  ndp->vp = root;
  ndp->next = ".";
  ndp->length = 1;
  (void) root->vops->link (ndp); /* XXX Do we care if this fails? */

  ndp->next = "..";
  ndp->length = 2;
  (void) root->vops->link (ndp); /* or this */

  mp->root_dir = root;

  /* Flush it out. */
  fs_vnode_flush (root);
  mp->buf->end_transaction (mp->buf);

  return 0;
}


/***********************************************************************
FUNCTION      m_stop

DESCRIPTION   This function handles normal shut down.

RETURN VALUE  0 is operation was sucessful
**********************************************************************/
static int
m_stop (struct fs_mount *mp)
{
  (void) mp;

  return 0;
}

static int
m_get_root (struct fs_mount *mpp, struct fs_vnode **rp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;

  *rp = mp->root_dir;
  fs_vnode_ref (*rp);

  return 0;
}

/**********************************************************************
 * Inode I/O
 *
 * A single entry is used for Inode I/O, since reading and writing share
 * most of their code.  Reading will never write wire, whereas writing
 * always will wire, up to as many pages as there are levels.
 *
 * XXX: This code is hardcoded for 8 inodes per cluster, and 128
 * cluster_id's per cluster. */

#define INODE_WRITE     1
#define INODE_READ      0

static int
inode_io (struct fs_mount_efs *mp, fs_inode_t inum,
    struct fs_inode *inode, int op)
{
  fs_cluster_id clust;
  int offset;
  struct fs_inode *data;

  clust = inum >> mp->inode_shift;
  offset = inum & mp->inode_low_mask;

  if (op == INODE_WRITE) {
    data = mp->buf->write_wire (mp->buf, clust, FS_BUF_READ_CLUSTER,
                                FS_DATA_TAG_INODE);
    data[offset] = *inode;
  } else {
    data = mp->buf->read_wire (mp->buf, clust);
    *inode = data[offset];
  }
  mp->buf->free_wire (mp->buf, clust);

  return 0;
}

/**********************************************************************
 * Inode allocation and deallocation.
 *
 * Two entities are used to keep track of which inodes are free.
 * info->inode_next is the smallest inode number that has never been used.
 * info->inode_free is the head inode on an inode free list.  This list is
 * chained through the data[0] field of each inode.
 *
 * Note that the allocation always performs a write of the info block and
 * possibly another cluster.  If inode allocation performance becomes an
 * issue, this could be optimized.
 *
 * If quotas and reservations are used, the inode allocation is charged to
 * the group of the calling vnode. If the file system is being initialized,
 * all inodes created at that time will be charged to group 0
 */

/* Allocate a new inode number.  Causes writes of 1.  Returns
 * 0 if success, or -ERRNO. */
static int
inode_alloc (struct fs_mount_efs *mp, fs_inode_t *inum /*OUT*/, uint32 gid)
{
  int result;
  fs_cluster_id tmp_clust;

  /* Do everything first, in case there is a failure, and then update the
   * info block. */

  if (mp->info.inode_free != FS_INODE_INVALID) {
    /* There is an entity on the free list. */
    *inum = mp->info.inode_free;
    result = inode_io (mp, *inum, &mp->tmp_inode, INODE_READ);
    mp->info.inode_free = (fs_inode_t) mp->tmp_inode.data[0];
    if (result < 0)
      return result;
  } else if (mp->info.inode_next != FS_INVALID_INODE) {
    /* We can bump the chain. */
    *inum = mp->info.inode_next;

    if ((mp->info.inode_next >> mp->inode_shift) !=
        ((mp->info.inode_next + 1) >> mp->inode_shift))
    {
      mp->info.inode_next = FS_INVALID_INODE;
    } else {
      mp->info.inode_next++;
    }
  } else {
    /* Otherwise, grab the next available. First verify that there is enough
     * space left before attempting an inode allocation. This check is
     * conservative. Some allocations that could succeed will be denied */

    /* Try allocating an inode from the general pool first. If no space is
     * available, then allocate an inode from the reservation
     */
    result = mp->buf->allocate (mp->buf, 0, &tmp_clust);
    if (result < 0)
    {
      result = mp->buf->allocate (mp->buf, gid, &tmp_clust);
      if (result < 0)
        FS_RAISE(-ENOSPC, "Unable to allocate inode", 0, 0, 0);
    }

    *inum = tmp_clust << mp->inode_shift;
    mp->info.inode_next = *inum + 1;
  }

  mp->info_dirty = 1;

  return 0;
}

/* Free an inode.  Causes 2 clusters to be written, one for the info block,
 * and one for the inode itself (to add to free chain).  Returns 0 for
 * success or -ERRNO. */
static int
inode_dealloc (struct fs_mount_efs *mp, fs_inode_t inum)
{
  efs_info_t info;
  int result;

  /* Chain this inode onto the free list. */
  result = inode_io (mp, inum, &mp->tmp_inode, INODE_READ);
  if (result < 0)
    return result;
  mp->tmp_inode.data[0] = (fs_cluster_id) mp->info.inode_free;
  result = inode_io (mp, inum, &mp->tmp_inode, INODE_WRITE);
  if (result < 0)
    return result;

  /* Update the info block. */
  mp->info.inode_free = inum;
  info = mp->buf->write_wire (mp->buf, mp->info_cluster, FS_BUF_READ_CLUSTER,
                              FS_DATA_TAG_INFO);
  info->inode_free = inum;
  mp->buf->free_wire (mp->buf, mp->info_cluster);

  return 0;
}

/* Lookup a vnode for a given inode.  If it is not loaded, allocate a new
 * vnode for it, and read the inode from flash.  The returned vnode has an
 * incremented reference count. */
static int m_read_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_inode_t inum)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  int result;

  result = inode_io (mp, inum, &vp->p.inode, INODE_READ);
  if (result != 0)
    FS_ERR_FATAL ("Inconsistency, inode is not in flash", 0, 0, 0);

  if (S_ISDIR (vp->p.inode.mode))
    vp->vops = &efs_dir_vops;
  else if (S_ISSPECIAL (vp->p.inode.mode))
    vp->vops = &efs_devspecial_vops;
  else
    vp->vops = &efs_file_vops;

  vp->mode = vp->p.inode.mode;

  return 0;
}

/* Lookup a vnode for a named inode.  If not loaded, allocate a new vnode
 * for it, and setup the vnode. */
static int m_read_named_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_mode_t mode, const char *key, int key_len)
{
  (void) mpp;

  fs_memscpy (vp->p.named.key, sizeof (vp->p.named.key), key, key_len);
  vp->p.named.key_length = key_len;

  vp->mode = mode;
  vp->inum = FS_INVALID_INODE;

  ASSERT (!S_ISSPECIAL(mode));

  if (S_ISITM (mode)) {
    vp->vops = &efs_item_vops;
  } else if (S_ISLNK (mode)) {
    vp->vops = &efs_symlink_vops;
  } else {
    return -EINVAL;
  }

  return 0;
}

/**********************************************************************
 * Database entry operations.
 */

/* The data entry is the letter 'i' followed by the data of the inode
 * number.  Later we can add other types (such as nv keys). */
static uint8 static_name_entry[1 + sizeof (fs_inode_t)];

/* Data entries for symlinks are the letter 's' followed by the text of the
 * symlink. */
static uint8 static_symlink_entry [1 + FS_MAX_SYMLINK_TARGET];

/* Data entries for items are the key for the item followed by the database
 * entry. */
#define ITEM_HEADER_SIZE (1 + 2)

/* Metadata for item file 2. */
PACKED struct fs_item_file2_metadata
{
  uint8     type; /* Type, should be capital 'N'. */
  fs_mode_t mode;
  uint16    gid;
  fs_time_t ctime;
} PACKED_POST;

/* Maximum size of all entries of type 'item' */
#define ITEM_HEADER_SIZE_MAX  MAX (ITEM_HEADER_SIZE, \
                                   sizeof (struct fs_item_file2_metadata))

/* Data entries for database items. */
static uint8 static_item_entry[ITEM_HEADER_SIZE_MAX + FS_ITEM_MAX];

/**********************************************************************
 * Mountpoint specific operations.
 */

static struct fs_vnode *
m_create_inode (struct fs_mount *mpp, fs_mode_t mode, uint32 gid, uint32 uid)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  struct fs_vnode *vp = NULL;
  int result;
  fs_inode_t inum = 0;

  mp->buf->start_transaction (mp->buf);

  result = inode_alloc (mp, &inum, gid);
  if (result == 0)
  {
    vp = fs_vnode_alloc ();

    if (vp != NULL)
    {
      vp->mp = mpp;
      vp->dev = mp->dev;

      vp->inum = inum;

      vp->dirty = 1;

      if (S_ISDIR (mode))
        vp->vops = &efs_dir_vops;
      else if (S_ISREG (mode))
        vp->vops = &efs_file_vops;
      else if (S_ISSPECIAL(mode))
        vp->vops = &efs_devspecial_vops;
      else
        FS_ERR_FATAL ("Unknown file type", 0, 0, 0);

      fs_inode_construct (&vp->p.inode, mode, gid, uid);

      vp->mode = vp->p.inode.mode;
    }
  }

  mp->buf->end_transaction (mp->buf);

  return vp;
}

static int
m_write_inode (struct fs_mount *mpp, struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  int result;

  result = inode_io (mp, vp->inum, &vp->p.inode, INODE_WRITE);

  return result;
}

static void
m_start_transaction (struct fs_mount *mpp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  mp->transaction_depth++;
  mp->buf->start_transaction (mp->buf);
}

/* Finishes the current transaction.  Does all work necessary, including
 * performing a given delayed transaction. */
static void
m_end_transaction (struct fs_mount *mpp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  efs_info_t info;

  mp->transaction_depth--;

  if (mp->transaction_depth == 0 && mp->info_dirty) {
    /* Flush out the changes to the info block. */
    info = mp->buf->write_wire (mp->buf, mp->info_cluster, FS_BUF_READ_CLUSTER,
                                FS_DATA_TAG_INFO);
    *info = mp->info;
    mp->buf->free_wire (mp->buf, mp->info_cluster);

    mp->info_dirty = 0;
  }

  /* Perform the work of truncation, breaking it into multiple
   * transactions, with proper checkpointing in the info block so that we
   * can restart at any time. */
  while (mp->transaction_depth == 0 &&
      mp->info.partial_delete > 0)
  {
    unsigned i;
    unsigned work = 0;
    unsigned did_split = 0;

    /* The 'partial_delete' field indicates what level we are to delete
     * next.  Level 1 indicates leaf nodes, level 2 the next level up.
     * Perform the freeing work for the appropriate level. */
    for (i = 0; i < FS_DIRECTION_LEVELS - 1; i++) {
      // printf ("Cleanup: level %d, (%x)\n", i,
      //     mp->info.partial_delete_data[i]);
      if (mp->info.partial_delete_data[i] != FS_INVALID_CLUSTER_ID)
        work += tree_deallocate (mp, mp->info.partial_delete_data[i],
            i + 1, mp->info.partial_delete - 1,
            mp->info.partial_delete_gid,
            mp->info.partial_delete_mid);
    }

    /* If this doesn't fit into a transaction, split the transaction. */
    while (work > 0 && mp->buf->is_too_large (mp->buf)) {
      if (!did_split) {
        /* Indicate that the level is partially finished.  The first
         * partial transaction write will update all of the counts, so next
         * time around at this level should not change counts. */
        mp->info.partial_delete_mid = 1;

        info = mp->buf->write_wire (mp->buf, mp->info_cluster,
                                    FS_BUF_READ_CLUSTER, FS_DATA_TAG_INFO);
        *info = mp->info;
        mp->buf->free_wire (mp->buf, mp->info_cluster);

        did_split = 1;
      }

      /* Flush out all non-freemap data and some part of the freemap. */
      mp->buf->partial_transaction_flush (mp->buf);
    }

    /* If this level did any work, and there are more levels to cover,
     * start a new transaction, updating the progress fields in the level
     * indicator. */
    mp->info.partial_delete++;
    mp->info.partial_delete_mid = 0;

    if (mp->info.partial_delete > FS_DIRECTION_LEVELS) {
      mp->info.partial_delete = 0;
    }

    /* Flush the info cluster. */
    info = mp->buf->write_wire (mp->buf, mp->info_cluster, FS_BUF_READ_CLUSTER,
                                FS_DATA_TAG_INFO);
    *info = mp->info;
    mp->buf->free_wire (mp->buf, mp->info_cluster);

    /* If the previous level did work, and there are more, create a
     * transaction boundary. */
    if (work > 0 && mp->info.partial_delete != 0) {
      mp->buf->end_transaction (mp->buf);
      mp->buf->start_transaction (mp->buf);
    }

    // FS_MSG_HIGH ("Truncation work: %d", work, 0, 0);
  }

  mp->buf->end_transaction (mp->buf);
}

static void
m_rollback_transaction (struct fs_mount *mpp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) mpp;
  efs_info_t info;

  mp->transaction_depth = 0;
  mp->info_dirty = 0;
  mp->buf->rollback_transaction (mp->buf);

  /* Re-read the info block. */
  info = mp->buf->read_wire (mp->buf, mp->info_cluster);
  mp->info = *info;
  mp->buf->free_wire (mp->buf, mp->info_cluster);
}

/**********************************************************************
 * Vnode operations.
 */

static int
v_efs_lookup (struct nameidata *ndp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) ndp->dvp->mp;
  const uint8 *value = NULL;
  unsigned value_length;
  int i;
  fs_inode_t rinode;
  int result;

  result = fs_efs2_name_lookup (mp->db,
      ndp->next, ndp->length, ndp->dvp->inum);

  if (result == 0) {
    fs_efs2_name_last_value (&value, &value_length);
  }

  if (result == 0 && value[0] == FS_VAL_FILE) {
    rinode = 0;

    /* Yes, this starts at size and counts down to 1.  That's where the
     * data is. */
    for (i = (int) sizeof (fs_inode_t); i > 0; i--) {
      rinode <<= 8;
      rinode |= (uint8) value[i];
    }

    ndp->vp = fs_vnode_lookup (&mp->parent, mp->dev, rinode);
    if (ndp->vp == NULL)
      return -ENOMEM;

    return 0;
  } else if (result == 0
      && (value[0] == FS_VAL_SYMLINK ||
          value[0] == FS_VAL_ITEM || value[0] == FS_VAL_ITEM2))
  {
    fs_mode_t mode;
    const uint8 *key;
    unsigned key_length;

    if (value[0] == FS_VAL_SYMLINK)
      mode = S_IFLNK | 0777;
    else {
      /* Real mode bits are stored in the data item. */
      mode = value[1] | (value[2] << 8);
      mode &= ~S_IFMT;
      mode |= S_IFITM;
    }
    fs_efs2_name_last_key (&key, &key_length);
    /* The vnode lookup will copy the key into a new vnode if necessary. */
    ndp->vp = fs_vnode_named_lookup (&mp->parent, mp->dev, mode,
                                     (const char *) key, key_length);
    if (ndp->vp != NULL) {
      if (value[0] == FS_VAL_ITEM2) {
        struct fs_item_file2_metadata *meta;
        meta = (struct fs_item_file2_metadata *)value;
        ndp->vp->p.named.p.owners.uid = FS_CURR_UID;
        ndp->vp->p.named.p.owners.gid = meta->gid;
      } else {
        /* The named vnode entry don't have a UID and GID, named entries
         * (item  files and symlinks) inherit the UID and GID from the
         * parent. */
        ndp->vp->p.named.p.owners.uid = ndp->dvp->p.inode.uid;
        ndp->vp->p.named.p.owners.gid = ndp->dvp->p.inode.gid;
      }

      return 0;
    } else {
      return -ENOENT;
    }

  } else {
    return -ENOENT;
  }
}

static int
v_efs_create (struct nameidata *ndp, fs_mode_t mode)
{
  struct fs_vnode *dvp = ndp->dvp;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) dvp->mp;
  int result;
  int entry_added = 0;

  m_start_transaction (dvp->mp);
  FS_SIMPLE_TRY (result);

  /* Step 1: Create the entry in the database. */
  if (result == 0) {
    uint32 header_size = sizeof (struct fs_item_file2_metadata);
    struct fs_item_file2_metadata *meta;

    /* Store the permission bits in the low part of the mode. */
    mode &= ~S_IFMT;
    static_item_entry[1] = mode & 0xFF;
    static_item_entry[2] = ((mode & 0xFF00u) >> 8);

    meta = (struct fs_item_file2_metadata *)static_item_entry;
    (void) meta->mode;
    meta->type = FS_VAL_ITEM2;
    meta->gid = ndp->dvp->p.inode.gid;
    meta->ctime = fs_time();

    result = fs_efs2_name_add (mp->db, ndp->next, ndp->length, dvp->inum,
                               static_item_entry, header_size,
                               dvp->p.inode.gid, FALSE, &entry_added);
  }

  /* Step 2: Get the key (d<inode><name>) back. This is required to
   * correctly handle long file names. */
  if (result == 0)
    result = fs_efs2_name_lookup (mp->db, ndp->next, ndp->length, dvp->inum);

  /* Step3: Populate the vnode values. */
  if (result == 0) {
    struct fs_vnode *vp;
    vp = fs_vnode_alloc ();
    if (vp == NULL) {
      result = -ENOMEM;
    } else {
      const uint8 *key;
      unsigned key_length;

      vp->mp = dvp->mp;
      vp->dev = mp->dev;
      vp->inum = FS_INVALID_INODE;
      vp->dirty = 0;
      vp->mode = S_IFITM | (mode & 07777);

      fs_efs2_name_last_key (&key, &key_length);
      fs_memscpy (vp->p.named.key, sizeof (vp->p.named.key), key, key_length);
      vp->p.named.key_length = key_length;
      vp->p.named.p.owners.gid = ndp->dvp->p.inode.gid;
      vp->p.named.p.owners.uid = FS_CURR_UID;
      vp->vops = &efs_item_vops;

      ndp->vp = vp;
    }
  }

  /* Step 4: Update the parent inode. */
  if (result == 0 && entry_added == 1) {
    /* When we add, adjust the parent directory. */
    dvp->p.inode.size++;
    dvp->dirty = 1;
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (dvp);
    m_end_transaction (dvp->mp);
  } else {
    m_rollback_transaction (dvp->mp);
  }

  return result;
}

static int
v_efs_link (struct nameidata *ndp)
{
  struct fs_vnode *dvp = ndp->dvp;
  struct fs_vnode *vp = ndp->vp;
  int i;
  fs_inode_t entry = ndp->vp->inum;
  int result;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) dvp->mp;

  XACT_CHECK (mp);

  /* Fail if the new link count WOULD overflow our 8 or 16-bit counter, but
   * don't actually increment it until after the other checks. */
  vp->p.inode.nlink++;
  if (vp->p.inode.nlink-- < 1)
    return -ENOSPC;

  static_name_entry[0] = FS_VAL_FILE;
  for (i = 0; i < (int) sizeof (fs_inode_t); i++) {
    static_name_entry[i+1] = entry & 0xFF;
    entry >>= 8;
  }

  result = fs_efs2_name_add (mp->db, ndp->next, ndp->length, dvp->inum,
                             static_name_entry, sizeof (static_name_entry),
                             vp->p.inode.gid, FALSE, NULL);

  if (result == 0) {
    /* Indicate we have a single link for this inode. */
    vp->p.inode.nlink++;
    vp->dirty = 1;

    /* Increment the parent's size to indicate the number of entries in this
     * directory. */
    dvp->p.inode.size++;
    dvp->dirty = 1;
  }

  return result;
}

static int
v_efs_unlink (struct nameidata *ndp)
{
  fs_mount_t mp = ndp->dvp->mp;
  int result = 0;

  m_start_transaction (mp);
  result = efs_unlink_helper(ndp);
  fs_vnode_flush (ndp->dvp);
  m_end_transaction (mp);

  return result;
}

static int
efs_unlink_helper (struct nameidata *ndp)
{
  struct fs_vnode *dvp = ndp->dvp;
  struct fs_vnode *vp = ndp->vp;
  struct fs_mount_efs *dmp = (struct fs_mount_efs *) dvp->mp;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
  int result;

  XACT_CHECK (mp);

  if (!S_ISLNK (vp->mode) && !S_ISITM (vp->mode)) {
    ASSERT (vp->p.inode.nlink > 0);
    vp->p.inode.nlink--;
    vp->dirty = 1;

    if (vp->p.inode.nlink == 0) {
      if (S_ISREG (vp->mode))
        v_efs_truncate (vp, 0);

      /* Free inode. */
      vp->dirty = 0;

      result = inode_dealloc (mp, vp->inum);
    } else {
      fs_vnode_flush (vp);
      result = 0;
    }
  } else {
    result = 0;
  }

  fs_vnode_unref (vp);
  ndp->vp = NULL;

  result = fs_efs2_name_remove (dmp->db,
      ndp->next, ndp->length, dvp->inum);
  if (result != 0) {
    FS_ERR_FATAL ("Unable to delete name", 0, 0, 0);
  }

  /* Clean up the parent. */
  dvp->p.inode.size--;
  dvp->dirty = 1;

  return result;
}

/**********************************************************************
 * Read and write. */

/* The uio_data structure keeps track of cluster locations for reads and
 * writes. */
struct uio_data {
  struct fs_vnode *vp;

  uint32                pos;            /* Position in file to operate. */
  uint32                count;          /* Bytes remaining to write. */
  uint8                *buf;            /* Current byte to write. */
  uint32                bytes_moved;    /* How much is done so far. */

  uint32                cluster_index;  /* Cluster within file. */
  uint32                cluster_offset; /* Offset within cluster. */
  uint32                this_count;     /* Bytes to write within cluster. */

  int                   level;          /* What level is this cluter at. */

  fs_cluster_id         data[FS_DIRECTION_LEVELS];
                                        /* Cluster for data at level. */
  uint8                *(dbuf)[FS_DIRECTION_LEVELS];
                                        /* Wired data, or NULL. */
};

static void
uio_setup (struct uio_data *uio, struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count)
{
  int i;

  uio->vp = vp;
  uio->pos = pos;
  uio->count = count;
  uio->buf = (uint8 *) buf;
  uio->bytes_moved = 0;

  for (i = 0; i < FS_DIRECTION_LEVELS; i++)
    uio->dbuf[i] = 0;
}
#ifdef EFS2_CHECK_UIO
void check_uio(struct uio_data *uio)
{
  int i,j;
  for (i=0;i<FS_DIRECTION_LEVELS;i++)
  {
    for (j=0;j<FS_DIRECTION_LEVELS;j++)
    {
      if (i != j &&
          uio->data[i] != FS_INVALID_CLUSTER_ID &&
          uio->data[i] != 0x0 &&
          uio->data[i] == uio->data[j] &&
          uio->dbuf[i] != 0x0 &&
          uio->dbuf[j] != 0x0)
      {
        FS_ERR("duplicate uio->data entries %d %d 0x%x",i,j,uio->data[i]);
        return;
      }
    }
  }
}
#else
  #define check_uio(x)
#endif /* EFS2_CHECK_UIO */

/* Returns 0 on success, -1 on failure.
 */
static int
uio_start_io (struct uio_data *uio, int is_for_write)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) uio->vp->mp;
  fs_size_t     index;
  int level;
  int data_offset;
  uint32 offsets[FS_DIRECTION_LEVELS];
  int i;
  uint32 *ptr;
  int result;
  uint32 *indirect_counts = mp->indirect_counts;
  uint32 indirect_mask, indirect_shift;

  /* Offset into struct fs_inode.data[] for each level. */
  static const int indirect_offsets[FS_DIRECTION_LEVELS] = {
    0, FS_DIRECT_CLUSTER_COUNT, FS_DIRECT_CLUSTER_COUNT + 1,
    FS_DIRECT_CLUSTER_COUNT + 2};

  uio->cluster_index = uio->pos >> mp->cluster_shift;
  uio->cluster_offset = uio->pos - (uio->cluster_index << mp->cluster_shift);
  uio->this_count = mp->cluster_size - uio->cluster_offset;
  if (uio->this_count > uio->count)
    uio->this_count = uio->count;

  /* Compute the level */
  index = uio->cluster_index;

  for (level = 0; level < FS_DIRECTION_LEVELS; level++) {
    if (index < indirect_counts[level])
      break;

    index -= indirect_counts[level];
  }
  ASSERT (level < FS_DIRECTION_LEVELS);

  /* Compute the index at each level. */
  indirect_mask = mp->indirect_cluster_mask;
  indirect_shift = mp->indirect_cluster_shift;
  for (i = level; i >= 0; i--) {
    offsets[i] = index & indirect_mask;
    index >>= indirect_shift;
  }

  /* Compute which element of the inode data index to use.  I think this
   * could just add offsets[level], but I'm not sure. */
  data_offset = indirect_offsets[level];
  if (level == 0)
    data_offset += uio->cluster_index;

  /* Loop through the levels, wiring down any of the pages as needed. */
  for (i = 0; i <= level; i++) {
    if (i == 0)
      ptr = &uio->vp->p.inode.data[data_offset];
    else
      ptr = &((uint32 *) uio->dbuf[i-1])[offsets[i]];

    if (uio->dbuf[i] == NULL || uio->data[i] != *ptr) {
      if (uio->dbuf[i] != NULL) {
        mp->buf->free_wire (mp->buf, uio->data[i]);
        uio->dbuf[i] = NULL;
      }
      uio->data[i] = *ptr;
      check_uio(uio);
      if (*ptr == FS_INVALID_CLUSTER_ID) {
        if (is_for_write) {
          result = mp->buf->allocate (mp->buf,
            uio->vp->p.inode.gid, &uio->data[i]);
          check_uio(uio);
          if (result < 0)
          {
            uio->this_count = 0;
            FS_RAISE(result, "Space exhausted",0,0,0);
          }

          if (i > 0) {
            uio->dbuf[i-1] = mp->buf->write_wire (mp->buf, uio->data[i-1],
                                                  FS_BUF_READ_CLUSTER,
                                                  FS_DATA_TAG_INODE);
            check_uio(uio);
            ptr = &((uint32 *) uio->dbuf[i-1])[offsets[i]];
          }

          *ptr = uio->data[i];

          if (i == level)
            uio->dbuf[i] =
                (uint8*) mp->buf->write_wire (mp->buf, uio->data[i],
                                              FS_BUF_DO_NOT_READ_CLUSTER,
                                              FS_DATA_TAG_DATA);
          else
            uio->dbuf[i] =
                (uint8*) mp->buf->write_wire (mp->buf, uio->data[i],
                                              FS_BUF_READ_CLUSTER,
                                              FS_DATA_TAG_INODE);

          check_uio(uio);
          if (i == level)
            memset (uio->dbuf[i], 0, mp->cluster_size);
          else
            /* XXX: This depends on the byte value of FS_INVALID_CLUSTER_ID. */
            memset (uio->dbuf[i], 0xFF, mp->cluster_size);

          uio->vp->p.inode.blocks++;
        } else {
          /* This is redundant. */
          /* uio->dbuf[i] = NULL; */

          /* Any remaining levels should be unwired in case we've moved on
           * to a cluster that isn't available. */
          for (i++; i <= level; i++) {
            if (uio->dbuf[i] != NULL) {
              mp->buf->free_wire (mp->buf, uio->data[i]);
              uio->dbuf[i] = NULL;
            }
          }
          break;
        }
      } else {
        /* Wire buffer.  The top level is wired for write, but indirect
         * clusters aren't (they will be in the code above). */
        if (i == level && is_for_write)
          uio->dbuf[i] = mp->buf->write_wire (mp->buf, uio->data[i],
                                              FS_BUF_READ_CLUSTER,
                                              FS_DATA_TAG_DATA);
        else
          uio->dbuf[i] = mp->buf->read_wire (mp->buf, uio->data[i]);
        check_uio(uio);
      }
    }
  }

  uio->level = level;

  return 0;
}

static void
uio_end_io (struct uio_data *uio, int is_for_write)
{
  /* Bump all of the counters. */
  uio->pos += uio->this_count;
  uio->buf += uio->this_count;
  uio->count -= uio->this_count;
  uio->bytes_moved += uio->this_count;

  /* Update the inodes size field if necessary. */
  if (is_for_write && uio->vp->p.inode.size < uio->pos) {
    uio->vp->p.inode.size = uio->pos;
  }
}

static void
uio_finish (struct uio_data *uio, int is_for_write)
{
  int i;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) uio->vp->mp;
  (void) is_for_write;

  check_uio(uio);
  /* Unwire anything that is still wired down. */
  for (i = 0; i < FS_DIRECTION_LEVELS; i++) {
    if (uio->dbuf[i] != NULL) {
      mp->buf->free_wire (mp->buf, uio->data[i]);
      uio->dbuf[i] = NULL;
    }
  }

}

/* Because of the global lock, we can use a global uio structure.  Should
 * be moved into the descriptors later if we want concurrency. */
static struct uio_data global_uio;

/* Actually write data to a file.  A simple outline is followed here. */
static fs_ssize_t
v_efs_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode)
{
  int status = 0;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  /* Due to limits on transactions, we limit writes to a certain amount.
   * The call to write will actually write less than we said we would, but
   * this is allowed.  However, this may be a gotcha, since most Posix
   * systems write as much as requested with filesystems. */
  if (count > mp->max_transaction)
    count = mp->max_transaction;

  /* Force values onto the stack, since the longjmp can clobber it. */
  (void) &pos;
  (void) &count;

  /* In append mode, all writes always seek to the end of the file.
   * In normal writes, use the given pos. */
  if (mode & O_APPEND)
    pos = vp->p.inode.size;

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY(status);
  if (status == 0)
  {
    status = efs_write_helper(vp, pos, buf, count);
    fs_vnode_flush (vp);
  }
  FS_SIMPLE_TRY_CLEANUP();
  if (status < 0) {
    m_rollback_transaction(vp->mp);
    /* Invalidate it, but don't remove the reference. */
    fs_vnode_ref (vp);
    fs_vnode_invalidate (vp);
  } else
    m_end_transaction (vp->mp);
  return status;
}

/* Actually write data to file */
static fs_ssize_t
efs_write_helper (struct fs_vnode *vp, fs_off_t pos,
    const void *buf, fs_size_t count)
{
  struct uio_data *uio = &global_uio;
  int uio_status = 0;
  uint32 cluster_size;

  XACT_CHECK ((struct fs_mount_efs *) vp->mp);

  cluster_size = ((struct fs_mount_efs *)vp->mp)->cluster_size;

  /* Throw away the const, we just have to trust ourselves. */
  uio_setup (uio, vp, pos, (void *) buf, count);
  while (uio->count > 0) {
    uio_status = uio_start_io (uio, 1);
    if (uio_status == 0)
      /* Update the data in the actual cluster. */
      fs_memscpy (uio->dbuf[uio->level] + uio->cluster_offset,
                  (cluster_size - uio->cluster_offset), uio->buf,
                  uio->this_count);
    uio_end_io (uio, 1);
    if (uio_status != 0)
      break;
  }

  if (uio_status < 0)
    return -ENOSPC;

  uio_finish (uio, 1);

  vp->dirty = 1;

  return uio->bytes_moved;
}

/* Read data from the file. */
static fs_ssize_t
v_efs_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count)
{
  struct uio_data *uio = &global_uio;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  /* If the read is past the end of the file, just return 0 without doing
   * any work. */
  if (pos > vp->p.inode.size) {
    return 0;
  }

  /* Limit the count by our transaction size (since the FS is locked). */
  if (count > mp->max_transaction)
    count = mp->max_transaction;

  /* Limit the count to the size of the file. */
  if (vp->p.inode.size - pos < count) {
    count = vp->p.inode.size - pos;
  }

  uio_setup (uio, vp, pos, buf, count);

  while (uio->count > 0) {
    (void) uio_start_io (uio, 0);

    if (uio->dbuf[uio->level] != NULL)
      fs_memscpy (uio->buf, uio->count,
                 uio->dbuf[uio->level] + uio->cluster_offset, uio->this_count);
    else
      memset (uio->buf, 0, uio->this_count);

    uio_end_io (uio, 0);
  }

  uio_finish (uio, 0);

  return uio->bytes_moved;
}

/* Read in a item into the static buffer, filling in the pointers to the
 * buffer if successful. */
static int
read_item (struct fs_vnode *vp, const uint8 **value, unsigned *value_length)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
  int result;

  result = fs_efs2_name_key_lookup (mp->db,
      (const uint8 *) vp->p.named.key, vp->p.named.key_length);
  if (result == 0) {
    fs_efs2_name_last_value (value, value_length);
  }

  return result;
}


/* Converts a item file to a regular file. The vnode will be updated
 * accordingly.
 *
 * The parameters 'val' and 'value_length' is the vnode (vp) value
 * and the length of the value buffer. The vnode value for a item
 * file is 'n'<mode bits><file_contents>. This is used to copy the
 * file data and mode bits over to the regular file.
 *
 * Returns 0 on success or -errno on error. */
static int
convert_item_to_regular (struct fs_vnode *vp, const uint8 *val,
                         unsigned value_length)
{
  int result;
  uint32 header_size;
  fs_ssize_t write_size;
  uint8 *static_value = static_item_entry;
  struct fs_vnode *regular_vp = NULL;

  /* This function should be only called when vnodes that are item files.
   * If it is called for not an item file there is something wrong with the
   * caller. */
  ASSERT (S_ISITM(vp->mode));

  if (val[0] == FS_VAL_ITEM2)
    header_size = sizeof (struct fs_item_file2_metadata);
  else
    header_size = ITEM_HEADER_SIZE;

  if (header_size > value_length)
    return -EINVAL;

  /* Copy the 'value' (e.g. mode + item file contents) into the private
   * buffer, only if it was not already copied by the caller. */
  if (static_value != val)
    fs_memscpy (static_value, sizeof (static_item_entry), val, value_length);

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0) {
    /* Create the inode for the new entry using the item file metadata. */
    fs_mode_t mode;
    uint32 uid, gid;

    mode = static_value[1] | (static_value[2] << 8);
    uid = vp->p.named.p.owners.uid;
    gid = vp->p.named.p.owners.gid;

    regular_vp = m_create_inode (vp->mp, S_IFREG | (mode & 07777), gid, uid);
    if (regular_vp == NULL) {
      result = -ENOSPC;
    } else if (val[0] == FS_VAL_ITEM2) {
      /* If this is the item file 2, update the change time. */
      fs_time_t ctime;
      struct fs_item_file2_metadata *meta;

      meta = (struct fs_item_file2_metadata *)static_value;
      ctime = meta->ctime;

      regular_vp->p.inode.ctime = ctime;
      regular_vp->p.inode.mtime = ctime;
      regular_vp->p.inode.atime = ctime;
    }
  }

  if (result == 0) {
    /* Copy the item file contents into the regular file. */
    fs_ssize_t count;
    count = value_length - header_size;
    write_size = efs_write_helper (regular_vp, 0, static_value + header_size,
                                   count);
    /* If efs_write_helper does not write the whole data it would of
     * done an FS_RAISE, put this check for consistency. */
    if (write_size != count)
    {
      FS_RAISE (-ENOSPC, "Unable to write item file contents", 0, 0, 0);
    }
  }

  if (result == 0) {
    /* Replace the key/value pair of the item file for the key/value pair
     * of the regular file. */
    uint32 i;
    fs_inode_t inum = regular_vp->inum;
    struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

    /* Create the inode key value, e.i. i<inode number> */
    static_name_entry[0] = FS_VAL_FILE;
    for (i = 0; i < sizeof (fs_inode_t); i++) {
      static_name_entry[i+1] = inum & 0xFF;
      inum >>= 8;
    }
    /* Overwrite the old item file entry with the new inode information. */
    result = fs_efs2_name_key_add (mp->db, vp->p.named.key,
                                   vp->p.named.key_length, static_name_entry,
                                   sizeof (static_name_entry),
                                   regular_vp->p.inode.gid, TRUE, NULL);
  }

  if (result == 0) {
    fs_vnode_supplant (vp, regular_vp);
    vp->p.inode.nlink++;
  } else if (regular_vp != NULL) {
    fs_vnode_invalidate (regular_vp);
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (vp);
    m_end_transaction (vp->mp);
  } else {
    m_rollback_transaction (vp->mp);
  }

  return result;
}

static fs_ssize_t
v_efs_item_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode)
{
  int result;
  uint32 len, header_size;
  const uint8 *value;
  unsigned value_length;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  result = read_item (vp, &value, &value_length);
  if (result != 0)
    return result;

  if (value[0] == FS_VAL_ITEM2)
    header_size = sizeof (struct fs_item_file2_metadata);
  else
    header_size = ITEM_HEADER_SIZE;

  /* If in append mode, all writes go to the end. */
  if (mode & O_APPEND)
    pos = value_length - header_size;

  /* Check if the write is going to go beyond the item file size */
  if (pos >= FS_ITEM_MAX || count > FS_ITEM_MAX - pos) {
    result = convert_item_to_regular (vp, value, value_length);
    if (result == 0)
      result = vp->vops->write (vp, pos, buf, count, mode);
    return result;
  }

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0) {
    /* Copy the whole item over. */
    fs_memscpy (static_item_entry, sizeof (static_item_entry), value,
                value_length);

    pos += header_size;
    /* If extending the file contents, zero out the gap. */
    if (pos > value_length)
      memset (static_item_entry + value_length, 0, pos - value_length);

    /* Overwrite or append the input buffer */
    fs_memscpy (static_item_entry + pos, (sizeof (static_item_entry) - pos),
                buf, count);

    /* Calculate the resulting item size. */
    len = pos + count;
    if (len < value_length)
      len = value_length;

    result = fs_efs2_name_key_add (mp->db,
                                   vp->p.named.key, vp->p.named.key_length,
                                   static_item_entry, len,
                                   vp->p.named.p.owners.gid, TRUE, NULL);
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    result = count;
    fs_vnode_flush (vp);
    m_end_transaction (vp->mp);
  } else {
    m_rollback_transaction (vp->mp);
  }

  return result;
}

static fs_ssize_t
v_efs_item_read (struct fs_vnode *vp, fs_size_t pos,
                 void *buf, fs_size_t count)
{
  int result;
  uint32 len, item_len, header_size;
  const uint8 *value;
  unsigned value_length;

  result = read_item (vp, &value, &value_length);
  if (result != 0)
    return result;

  if (value[0] == FS_VAL_ITEM2)
    header_size = sizeof (struct fs_item_file2_metadata);
  else
    header_size = ITEM_HEADER_SIZE;

  item_len = value_length - header_size;

  /* If the read is past the end of the item, just return 0. */
  if (pos >= item_len)
    return 0;

  value += (header_size + pos);
  len = item_len - pos;
  if (len > count)
    len = count;

  fs_memscpy (buf, count, value, len);

  return len;
}

/* Perform the deletion at a specific level of the tree.  Level is the
 * number of levels of indirection encountered before the leaf node is
 * reached.  '0' indicates a direct cluster, '1' would be single indirect,
 * and so on.
 *
 * The 'desired_level' value is the level to deallocate.  This number
 * counts from the leaf of the tree, '0' begin the real leaves, '1' being
 * the indirect blocks that point to those leaves, and so on.
 *
 * Returns the number of clusters that have been freed. */
static unsigned
tree_deallocate (struct fs_mount_efs *mp, fs_cluster_id id, unsigned level,
    unsigned desired_level, unsigned gid, unsigned is_recovery)
{
  unsigned count = 0;
  fs_cluster_id *iblock;
  unsigned i;
  uint32 ids_per_cluster = mp->cluster_size / sizeof (fs_cluster_id);

  if (level == desired_level) {
    // printf ("Delete level %d, desired %d\n", level, desired_level);
    if (is_recovery)
      mp->buf->deallocate_recovered (mp->buf, id);
    else
      mp->buf->deallocate (mp->buf, id, gid);
    count++;
  } else if (level > desired_level) {
    /* Otherwise, recurse. */
    // printf ("Recurse level %d, desired %d\n", level, desired_level);
    iblock = (fs_cluster_id *) mp->buf->read_wire (mp->buf, id);
    for (i = 0; i < ids_per_cluster; i++) {
      if (iblock[i] != FS_INVALID_CLUSTER_ID) {
        count += tree_deallocate (mp, iblock[i], level-1, desired_level, gid,
            is_recovery);
      }
    }
    mp->buf->free_wire (mp->buf, id);
    // printf ("Unrecurse level %d, desired %d\n", level, desired_level);
  } /* Otherwise, we are past it, so do no work. */

  return count;
}

static int partial_truncate (struct fs_vnode *vp);
static int
v_efs_truncate (struct fs_vnode *vp, fs_size_t pos)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
  unsigned i;
  fs_cluster_id clust;

  ASSERT (!S_ISSPECIAL (vp->mode));

  m_start_transaction (vp->mp);

  /* Optimize for the truncate to zero case, since that is the most common
   * use (overwrite a file, for example).
   */
  if (pos == 0) {

    /* Free the direct blocks. */
    for (i = 0; i < FS_DIRECT_CLUSTER_COUNT; i++) {
      clust = vp->p.inode.data[i];
      if (clust != FS_INVALID_CLUSTER_ID) {
        mp->buf->deallocate (mp->buf, clust, vp->p.inode.gid);
        vp->p.inode.data[i] = FS_INVALID_CLUSTER_ID;
      }
    }

    /* Record the state of the indirect blocks. */
    fs_memscpy (mp->info.partial_delete_data,
        sizeof (mp->info.partial_delete_data),
        vp->p.inode.data + FS_DIRECT_CLUSTER_COUNT,
        sizeof (fs_cluster_id) * (FS_DIRECTION_LEVELS - 1));

    /* Indicate to end transaction that there is some truncation work to
     * do. */
    mp->info.partial_delete = 1;
    mp->info.partial_delete_mid = 0;
    mp->info.partial_delete_gid = vp->p.inode.gid;
    mp->info_dirty = 1;

    /* Delete the inode's pointers to the indirect lists. */
    for (i = 1; i < FS_DIRECTION_LEVELS; i++) {
      vp->p.inode.data[i + FS_DIRECT_CLUSTER_COUNT - 1] =FS_INVALID_CLUSTER_ID;
    }

    vp->p.inode.blocks = 0;
    vp->p.inode.size = 0;
    vp->dirty = 1;
  }
  else
  {
    fs_size_t old_size = vp->p.inode.size;

    vp->p.inode.size = pos;
    vp->dirty = 1;

    if (pos < old_size)
    {
      partial_truncate (vp);
    }
  }

  fs_vnode_flush (vp);
  m_end_transaction (vp->mp);

  return 0;
}

/* TODO: Zero partial block in truncate.
 * TODO: Handle bad powerdowns mid-truncate.
 */

static int count_tree_nodes (struct fs_vnode *vp, fs_cluster_id root,
                             int depth);
static void truncate_split_tree (struct fs_vnode *vp,
    fs_cluster_id  node,
    uint64   offset,
    uint64   width,
    int depth);

/* Truncate a file at a position other than beginning.  This function
 * removes any blocks that are past the end of the file, as well as zeroing
 * out the data in the part of the block that is at the end of the file. */
static int
partial_truncate (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
  uint32 size = vp->p.inode.size;
  int dindex;
  uint64 span_a, span_b;
  /* level_size needs to be sufficiently large to hold the multiplication
   * factor of level_size *= (Cluster entries per page). This gets large
   * enough when the page size is 4K or grater.
   */
  uint64 level_size = mp->cluster_size;
  int level;
  uint32 ids_per_cluster = mp->cluster_size / sizeof (fs_cluster_id);

  span_a = 0;
  level = 0;

  /* Loop through each of the direct and indirect clusters. */
  for (dindex = 0;
      dindex < FS_DIRECT_CLUSTER_COUNT + FS_DIRECTION_LEVELS - 1;
      dindex++)
  {
    /* Indirect cluster [dindex] refers to byte offsets
     * [span_a, span_b) in the file. */
    span_b = span_a + level_size;

    if (size > span_a && size < span_b) {
      if (dindex >= FS_DIRECT_CLUSTER_COUNT &&
          vp->p.inode.data[dindex] != FS_INVALID_CLUSTER_ID)
      {
        truncate_split_tree (vp, vp->p.inode.data[dindex],
            size - span_a, level_size, level);
      }
    } else if (size <= span_a) {
      if (vp->p.inode.data[dindex] != FS_INVALID_CLUSTER_ID) {
        if (level == 0) {
          mp->buf->deallocate (mp->buf, vp->p.inode.data[dindex],
              vp->p.inode.gid);
          vp->p.inode.data[dindex] = FS_INVALID_CLUSTER_ID;
          vp->p.inode.blocks--;
          vp->dirty = 1;
        } else {
          int nodes = count_tree_nodes (vp, vp->p.inode.data[dindex], level);
          mp->buf->deallocate_tree (mp->buf, vp->p.inode.data[dindex], level,
              nodes, vp->p.inode.gid);
          vp->p.inode.data[dindex] = FS_INVALID_CLUSTER_ID;
          vp->p.inode.blocks -= nodes;
          vp->dirty = 1;
        }
      }
    }

    /* Adjust for the next span. */
    span_a = span_b;
    if (dindex >= FS_DIRECT_CLUSTER_COUNT - 1) {
      level_size *= ids_per_cluster;
      level++;
    }
  }

  return 0;
}

/* Count the number of clusters contained in the allocation tree pointed to
 * by root.  The count is appropriate for giving to buf->deallocate_tree. */
static int
count_tree_nodes (struct fs_vnode *vp, fs_cluster_id root, int depth)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
  int count = 1;
  uint32 i;
  fs_cluster_id *data;
  uint32 ids_per_cluster = mp->cluster_size / sizeof (fs_cluster_id);

  data = mp->buf->read_wire (mp->buf, root);

  if (depth == 1) {
    for (i = 0; i < ids_per_cluster; i++) {
      if (data[i] != FS_INVALID_CLUSTER_ID)
        count++;
    }
  } else {
    for (i = 0; i < ids_per_cluster; i++) {
      if (data[i] != FS_INVALID_CLUSTER_ID)
        count += count_tree_nodes (vp, data[i], depth-1);
    }
  }

  mp->buf->free_wire (mp->buf, root);

  return count;
}

/* Perform a truncate on a tree that is where the truncation point is
 * partway through the tree.  Walks down the tree and free's the right hand
 * side of each tree. */
static void truncate_split_tree (struct fs_vnode *vp,
    fs_cluster_id  node,
    uint64   offset,
    uint64   width,
    int depth)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
  uint32 ids_per_cluster = mp->cluster_size / sizeof (fs_cluster_id);
  uint64  lower_width = width / ids_per_cluster;
  fs_cluster_id *data;
  uint32 i;
  uint64 left, right;

  data = mp->buf->write_wire (mp->buf, node,
                              FS_BUF_READ_CLUSTER, FS_DATA_TAG_DB);

  if (depth == 1) {
    left = 0;
    for (i = 0; i < ids_per_cluster; i++) {
      right = left + lower_width;

      if (left >= offset) {
        if (data[i] != FS_INVALID_CLUSTER_ID) {
          mp->buf->deallocate (mp->buf, data[i], vp->p.inode.gid);
          data[i] = FS_INVALID_CLUSTER_ID;
          vp->p.inode.blocks--;
          vp->dirty = 1;
        }

        if (mp->buf->get_write_count (mp->buf) >= 19) {
          mp->buf->free_wire (mp->buf, node);
          fs_vnode_flush (vp);
          mp->buf->end_transaction (mp->buf);
          mp->buf->start_transaction (mp->buf);
          data = mp->buf->write_wire (mp->buf, node, FS_BUF_READ_CLUSTER,
                                      FS_DATA_TAG_DB);
        }
      }
      /* TODO: Check for partial page, and zero out the rest of the
       * cluster. */

      left = right;
    }
  } else {
    left = 0;
    for (i = 0; i < ids_per_cluster; i++) {
      right = left + lower_width;

      if (data[i] == FS_INVALID_CLUSTER_ID)
        continue;
      if (left >= offset) {
        int nodes = count_tree_nodes (vp, data[i], depth - 1);
        mp->buf->deallocate_tree (mp->buf, data[i], depth - 1, nodes,
            vp->p.inode.gid);
        data[i] = FS_INVALID_CLUSTER_ID;
        vp->p.inode.blocks -= nodes;
        vp->dirty = 1;
      } else if (right > offset) {
        truncate_split_tree (vp, data[i], offset - left, lower_width,
            depth - 1);
      }

      if (mp->buf->get_write_count (mp->buf) >= 19) {
        mp->buf->free_wire (mp->buf, node);
        fs_vnode_flush (vp);
        mp->buf->end_transaction (mp->buf);
        mp->buf->start_transaction (mp->buf);
        data = mp->buf->write_wire (mp->buf, node, FS_BUF_READ_CLUSTER,
                                    FS_DATA_TAG_DB);
      }

      left = right;
    }
  }

  mp->buf->free_wire (mp->buf, node);
}

static int
v_efs_item_truncate (struct fs_vnode *vp, fs_size_t pos)
{
  int result;
  uint32 len, item_len, header_size;
  const uint8 *value;
  unsigned value_length;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  result = read_item (vp, &value, &value_length);
  if (result != 0)
    return result;

  if (value[0] == FS_VAL_ITEM2)
    header_size = sizeof (struct fs_item_file2_metadata);
  else
    header_size = ITEM_HEADER_SIZE;

  item_len = value_length - header_size;

  /* Check if there is enough space for the data in the buffer. */
  if (pos > FS_ITEM_MAX)
  {
    result = convert_item_to_regular (vp, value, value_length);
    if (result == 0)
      result = vp->vops->truncate (vp, pos);
    return result;
  }

  /* If the new position is the same as the old then no change is required */
  if (pos == item_len)
    return 0;

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0) {
    if (pos > item_len) {
      /* Copy the whole item over. */
      fs_memscpy (static_item_entry, sizeof (static_item_entry), value,
                  value_length);
      /* Truncate is expanding the item file, zero out the end. */
      memset (static_item_entry + value_length, 0, pos - item_len);
    } else {
      /* The item file size is smaller than before, copy item data. */
      fs_memscpy (static_item_entry, sizeof (static_item_entry), value,
                  header_size + pos);
    }

    /* Calculate the new entry length */
    len = pos + header_size;

    result = fs_efs2_name_key_add (mp->db,
                                   vp->p.named.key, vp->p.named.key_length,
                                   static_item_entry, len,
                                   vp->p.named.p.owners.gid, TRUE, NULL);
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (vp);
    m_end_transaction (vp->mp);
  } else {
    m_rollback_transaction (vp->mp);
  }

  return result;
}

static int mkdir_helper (struct nameidata *ndp, fs_mode_t mode);

static int
v_efs_mkdir (struct nameidata *ndp, fs_mode_t mode)
{
  fs_mount_t mp = ndp->dvp->mp;
  int result = 0;

  m_start_transaction (mp);
  FS_SIMPLE_TRY (result);

  if (result == 0) {
    result = mkdir_helper (ndp, mode);
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    m_end_transaction (mp);
  } else {
    if (ndp->vp != NULL) {
      fs_vnode_invalidate (ndp->vp);
      ndp->vp = NULL;
    }

    /* The parent still references the parent, so we need to add a
     * reference, since the invalidate reduces it by one. */
    fs_vnode_ref (ndp->dvp);
    fs_vnode_invalidate (ndp->dvp);
    m_rollback_transaction (mp);
  }

  return result;
}

static int
mkdir_helper (struct nameidata *ndp, fs_mode_t mode)
{
  int result;
  struct fs_vnode *vp;
  struct nameidata *ndp2 = &efs2_name_buffer;

  vp = m_create_inode (ndp->dvp->mp, S_IFDIR | (mode & 07777),
      ndp->dvp->p.inode.gid, FS_CURR_UID);
  if (vp == NULL) {
    return -ENOSPC;
  }

  /* TODO: Now, assumes that the caller rolls back transaction.  Would be
   * cleaner to have that here. */
  /* Create links for directory entry, ".", and "..". */
  ndp->vp = vp;
  result = v_efs_link (ndp);
  if (result == 0) {
    ndp2->dvp = vp;
    ndp2->vp = vp;
    ndp2->next = ".";
    ndp2->length = 1;
    result = v_efs_link (ndp2);
  }
  if (result == 0) {
    ndp2->vp = ndp->dvp;
    ndp2->next = "..";
    ndp2->length = 2;
    result = v_efs_link (ndp2);
  }

  fs_vnode_flush (ndp->dvp);
  fs_vnode_flush (vp);

  return result;
}

/* Remove the specified directory.  The upper layer has checked that the
 * name is valid. */
static int
v_efs_rmdir (struct nameidata *ndp)
{
  int result;
  struct nameidata *ndp2 = &efs2_name_buffer;

  /* Only allow directories to be removed. */
  if (!S_ISDIR (ndp->vp->mode))
    return -ENOTDIR;

  /* Only allow empty directories to be removed.  Since '.' and '..' are
   * entries, 2 is the size of an empty directory. */
  if (ndp->vp->p.inode.size != 2)
    return -ENOTEMPTY;

  /* XXX: If the directory is open, return an error. */
  if (fs_vnode_getref (ndp->vp) > 1)
    return -ETXTBSY;

  /* Each call to unlink below unrefs the vnode.  Add extra refs so that
   * the vnodes stick around long enough.  There is one unlink of dvp, and
   * two of vp.  By adding one to each, we will end up with a single
   * reference to dvp. */
  fs_vnode_ref (ndp->dvp);
  fs_vnode_ref (ndp->vp);

  /* Remove the two links inside the directory, and then remove the
   * directory itself.  If these unlinks don't work, something is wrong
   * with the FS itself, just go on. */
  m_start_transaction (ndp->dvp->mp);
  ndp2->dvp = ndp->vp;
  ndp2->vp = ndp->dvp;
  ndp2->next = "..";
  ndp2->length = 2;
  (void) ndp->vp->vops->unlink (ndp2);
  ndp2->vp = ndp->vp;
  ndp2->next = ".";
  ndp2->length = 1;
  (void) ndp->vp->vops->unlink (ndp2);

  result = ndp->dvp->vops->unlink (ndp);
  fs_vnode_flush (ndp->dvp);
  m_end_transaction (ndp->dvp->mp);

  /* We removed the last reference. */
  ndp->vp = NULL;

  return result;
}

static void *
v_efs_opendir (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return fs_efs2_iter_begin (mp->db, vp->inum);
}

static int
v_efs_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent)
{
  return fs_efs2_iter_next (dir, vp->inum, dirent);
}

static int
v_efs_closedir (struct fs_vnode *vp, void *dir)
{
  (void) vp;

  return fs_efs2_iter_close (dir);
}

static int
v_efs_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  /* Unless a symlink or an "item", just copy the fields over. */
  buf->st_dev = vp->dev;
  buf->st_ino = vp->inum;
  buf->st_mode = vp->p.inode.mode;
  buf->st_nlink = vp->p.inode.nlink;
  buf->st_size = vp->p.inode.size;
  buf->st_uid = vp->p.inode.uid;
  buf->st_gid = vp->p.inode.gid;

  buf->st_blksize = ((struct fs_mount_efs *) vp->mp)->cluster_size;

  buf->st_blocks = vp->p.inode.blocks;
  buf->st_atime = vp->p.inode.atime;
  buf->st_mtime = vp->p.inode.mtime;
  buf->st_ctime = vp->p.inode.ctime;

  /* This param make sense for the dev special files. */
  buf->st_rdev = 0;

  return 0;
}

static int
rename_item (struct nameidata *ndp, struct nameidata *ndp2)
{
  const uint8 *value = NULL;    /* stupid lint */
  unsigned value_length = 0;    /*             */
  struct fs_mount_efs *mp = (struct fs_mount_efs *) ndp->vp->mp;
  struct fs_vnode *vp = ndp->vp;
  struct fs_vnode *dvp = ndp2->dvp;
  int result = 0;

  /* Read the item out so it can be renamed */
  result = fs_efs2_name_key_lookup (mp->db,
      (const uint8 *)vp->p.named.key, vp->p.named.key_length);

  if (result == 0) {
    fs_efs2_name_last_value (&value, &value_length);

    /* The item has been read, now put it back with the new name */
    fs_memscpy (static_item_entry, sizeof (static_item_entry),
                (void *)value, value_length);

    result = fs_efs2_name_add (mp->db,
                               ndp2->next, ndp2->length, dvp->inum,
                               static_item_entry,
                               value_length,
                               ndp2->dvp->p.inode.gid, TRUE, NULL);

    /* Increment the number of entries in the parent's INode. This is needed
     * if the entry is moving across two different parents (ie two different
     * directories). In case of rename in the same directory this gets
     * decremented where the source name is removed. */
    if (result == 0)
    {
      ndp2->dvp->p.inode.size++;
      ndp2->dvp->dirty = 1;
    }
  }
  return result;
}

static int
rename_file (struct nameidata *ndp2)
{
  return ndp2->dvp->vops->link (ndp2);
}

static int
rename_dir (struct nameidata *ndp, struct fs_vnode *dvp)
{
  int result;
  struct fs_vnode *vp = NULL;
  struct nameidata *ndp2 = &efs2_name_buffer;

  /* Link the new directory */
  result = ndp->dvp->vops->link (ndp);

  /* If new and old directories have different parents */
  if (ndp->dvp != dvp)
  {
    /* Remove the '..' entry from the new directory and create
       a new one that has updated parent */
    ndp2->dvp = ndp->vp;
    ndp2->vp = dvp;
    ndp2->next = "..";
    ndp2->length = 2;

    /* Unlink is going to decrement the ref count. So, increment it here */
    fs_vnode_ref (ndp2->vp);

    result = efs_unlink_helper (ndp2);
    if (result == 0)
    {
      vp = m_create_inode (ndp->dvp->mp, S_IFDIR,
          ndp->dvp->p.inode.gid, FS_CURR_UID);
      if (vp == NULL) {
        result = -ENOSPC;
      }
    }

    if (result == 0 && vp != NULL)
    {
      ndp2->vp = ndp->dvp;
      ndp2->dvp = ndp->vp;
      ndp2->next = "..";
      ndp2->length = 2;
      result = v_efs_link (ndp2);
      fs_vnode_flush (ndp->dvp);
      fs_vnode_flush (vp);
      fs_vnode_unref (vp);
    }
  }

  return result;
}

static int
rename_symlink (struct nameidata *ndp_old, struct nameidata *ndp_new)
{
  const uint8 *value = NULL;
  unsigned int value_length = 0;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) ndp_old->vp->mp;
  int result;

  /* Look up the existing symlink and create one under the new directory */
  result = fs_efs2_name_key_lookup (mp->db,
                                  (const uint8 *) ndp_old->vp->p.named.key,
                                  ndp_old->vp->p.named.key_length);

  if (result == 0)
  {
    fs_efs2_name_last_value (&value, &value_length);

    fs_memscpy (static_symlink_entry, sizeof (static_symlink_entry),
                (void *) value, value_length);

    result = fs_efs2_name_add (mp->db,
                               ndp_new->next, ndp_new->length,
                               ndp_new->dvp->inum,
                               static_symlink_entry,
                               value_length,
                               ndp_new->dvp->p.inode.gid, TRUE, NULL);

    /* Increment the number of entries in the parent's INode. This is needed
     * if the entry is moving across two different parents (ie two different
     * directories). In case of rename in the same directory this gets
     * decremented where the source name is removed. */
    if (result == 0)
    {
      ndp_new->dvp->p.inode.size++;
      ndp_new->dvp->dirty = 1;
    }
  }
  return result;
}

static int
is_cycle_present (fs_inode_t old_inode, fs_inode_t new_inode,
                   struct fs_mount_efs *mp)
{
  int result = 0;
  int i;
  const uint8 *value = NULL;
  unsigned value_length;
  fs_inode_t rinode;
  fs_inode_t curr_inode;

  curr_inode = new_inode;
  while (1)
  {
    /* Check whether we hit a cycle */
    if (curr_inode == old_inode)
    {
      result = 1;
      break;
    }

    result = fs_efs2_name_lookup (mp->db,
        "..", 2, curr_inode);

    if (result == 0) {
      fs_efs2_name_last_value (&value, &value_length);
    }

    rinode = 0;

    ASSERT (value != NULL);
    for (i = (int) sizeof (fs_inode_t); i > 0; i--) {
      rinode <<= 8;
      rinode |= (uint8) value[i];
    }

    /* We are at the root of the file system (not necessarily at '/') */
    if (rinode == curr_inode)
    {
      result = 0;
      break;
    }

    curr_inode = rinode;
  }

  return result;
}

static int
v_efs_rename (struct nameidata *ndp, struct nameidata *ndp2)
{
  fs_mount_t mp;
  int result = 0;
  int inside_xact = 0;

  ASSERT (ndp->dvp != NULL);
  mp = ndp->dvp->mp;

  /* Check to see if renaming to same name, if it does return
   * without doing anything. */
  if (ndp->vp == ndp2->vp)
    return 0;

  /* Force variable to be on stack, since longjmp isn't guaranteed to
   * preserve register variables. */
  (void) &inside_xact;

  /* The caller already tested this, but Klokwork wants us to test again.
   * Ensure that the "old" name has a vp.  (ndp2 might be new, NULL) */
  if ((result == 0) && (ndp->vp == NULL))
    result = -EINVAL;

  /* Rename is only allowed for regular files, directories, item files
     and symlinks */
  if ((result == 0) &&
      ((ndp->vp->mode & S_IFMT) != S_IFREG) &&
      ((ndp->vp->mode & S_IFMT) != S_IFDIR) &&
      ((ndp->vp->mode & S_IFMT) != S_IFITM) &&
      ((ndp->vp->mode & S_IFMT) != S_IFLNK))
    result = -EINVAL;

  /* If we are trying to rename directories, following checks need
     to take place */
  if ((result == 0) && (ndp->vp->mode & S_IFMT) == S_IFDIR)
  {
    /* New path is not a directory but old path is */
    if (result == 0 && ndp2->vp != NULL
        && (ndp2->vp->mode & S_IFMT) != S_IFDIR)
      result = -ENOTDIR;

    /* Check whether an attempt to make a directory a subdirectory of itself */
    if (result == 0 &&
        is_cycle_present (ndp->vp->inum, ndp2->dvp->inum,
                         (struct fs_mount_efs *) ndp2->dvp->mp))
    {
      result = -EINVAL;
    }

    /* New path should be a non-empty directory
       i.e contains entries other than '.' and '..' */
    if (result == 0  && ndp2->vp != NULL && ndp2->vp->p.inode.size != 2)
      result = -ENOTEMPTY;
  }
  else /* Old path is not a directory */
  {
    /* New path is an existing directory */
    if (result == 0 && ndp2->vp != NULL &&
       (ndp2->vp->mode & S_IFMT) == S_IFDIR)
      result = -EISDIR;
  }

  if (result == 0) {
    m_start_transaction (mp);
    inside_xact = 1;
    FS_SIMPLE_TRY (result);
  }

  if (result == 0 && ndp2->vp != NULL) {
    /* Handle removing destination. */
    result = efs_unlink_helper (ndp2);
  }

  if (result == 0) {
    /* Link the new name. */
    ndp2->vp = ndp->vp;
    fs_vnode_ref (ndp2->vp);

    switch (ndp2->vp->mode & S_IFMT)
    {
      case S_IFITM:
        result = rename_item(ndp, ndp2);
        break;

      case S_IFREG:
        result = rename_file(ndp2);
        break;

      case S_IFDIR:
        result = rename_dir(ndp2, ndp->dvp);
        break;

      case S_IFLNK:
        result = rename_symlink(ndp, ndp2);
        break;

      default:
        result = -EINVAL;
    }
  }

  if (result == 0) {
    /* Unlink the old name. */
    result = efs_unlink_helper (ndp);
    ndp->vp = NULL;
  }

  if (ndp->dvp != NULL) {
    fs_vnode_flush (ndp->dvp);
  }
  if (ndp->vp != NULL) {
    fs_vnode_flush (ndp->vp);
  }
  if (ndp2->dvp != NULL) {
    fs_vnode_flush (ndp2->dvp);
  }
  if (ndp2->vp != NULL) {
    fs_vnode_flush (ndp2->vp);
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result != 0) {
    if (inside_xact)
      m_rollback_transaction (mp);
  } else {
    m_end_transaction (mp);
  }

  return result;
}

static int
v_efs_symlink (struct nameidata *ndp, const char *dest)
{
  int result;
  struct fs_vnode *dvp = ndp->dvp;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) dvp->mp;
  int vlen;

  vlen = strlen (dest);

  if (vlen > FS_MAX_SYMLINK_TARGET)
    return -ENAMETOOLONG;

  static_symlink_entry[0] = FS_VAL_SYMLINK;
  fs_memscpy (static_symlink_entry + 1, sizeof (static_symlink_entry) - 1,
              dest, vlen);

  m_start_transaction (dvp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0) {
    result = fs_efs2_name_add (mp->db,
        ndp->next, ndp->length, dvp->inum,
        static_symlink_entry, vlen + 1, ndp->dvp->p.inode.gid, FALSE, NULL);
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    /* Indicate an added entry in this directory. */
    dvp->p.inode.size++;
    dvp->dirty = 1;

    fs_vnode_flush (dvp);
    m_end_transaction (dvp->mp);
  } else {
    m_rollback_transaction (dvp->mp);
  }

  return result;
}

static int
v_efs_put_item (struct nameidata *ndp,
    void *data, int length, int mode)
{
  int result;
  struct fs_vnode *dvp = ndp->dvp;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) dvp->mp;
  int entry_added = 0;
  uint32 header_size = 0;
  const uint8 *val = NULL;

  (void) &mode;

  if (length > FS_ITEM_MAX)
    return -FS_ERANGE;

  /* If this item exists and its content is exactly the same, then lets
   * return success without overwriting the data. */
  if (ndp->vp != NULL)
  {
    const uint8 *value = NULL;
    unsigned value_length = 0;
    result = read_item (ndp->vp, &value, &value_length);
    ASSERT (result == 0);

    if (value[0] == FS_VAL_ITEM2)
      header_size = sizeof (struct fs_item_file2_metadata);
    else
      header_size = ITEM_HEADER_SIZE;

    ASSERT (value_length >= header_size);

    val = value;
    value_length -= header_size;
    value += header_size;

    if ((int)value_length == length)
    {
      /* Check length first to avoid passing NULL to memcmp. */
      if (length == 0 || memcmp (value, data, length) == 0)
      {
        int new_mode, stored_mode;

        new_mode = mode & ~S_IFMT;
        stored_mode = ndp->vp->mode & ~S_IFMT;
        if (new_mode == stored_mode)
        {
          /* This item-file exists with the same exact data. So lets avoid
           * this overwrite and return success. */
          return 0;
        }
      }
    }
  }

  m_start_transaction (dvp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0) {
    if (header_size != 0 &&
        header_size == sizeof (struct fs_item_file2_metadata)) {
      /* The entry is item_file2 type. */
      fs_memscpy (static_item_entry, sizeof (static_item_entry),
                  val, header_size);
    } else {
      static_item_entry[0] = FS_VAL_ITEM;
      header_size = ITEM_HEADER_SIZE;
    }

    /* Store the permission bits in the low part of the mode. */
    mode &= ~S_IFMT;
    static_item_entry[1] = mode & 0xFF;
    static_item_entry[2] = ((mode & 0xFF00u) >> 8);

    /* Only copy the data if length not zero.
     * If length is zero, *data pointer may be NULL, do the check to avoid
     * passing NULL to the lower layer. */
    if (length != 0)
      fs_memscpy (static_item_entry + header_size,
                  (sizeof (static_item_entry) - header_size), data, length);

    result = fs_efs2_name_add (mp->db,
        ndp->next, ndp->length, dvp->inum,
        static_item_entry, length + header_size,
        ndp->dvp->p.inode.gid, TRUE, &entry_added);
  }

  if (result == 0 && entry_added == 1) {
    /* When we add, adjust the parent directory. */
    dvp->p.inode.size++;
    dvp->dirty = 1;
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (dvp);
    m_end_transaction (dvp->mp);
  } else {
    m_rollback_transaction (dvp->mp);
  }

  return result;
}

static int
v_efs_mknod (struct nameidata *ndp, fs_mode_t mode, fs_devspecial_t dev)
{
  fs_mount_t mp = ndp->dvp->mp;
  int result;

  (void) &mode;

  m_start_transaction (mp);
  FS_SIMPLE_TRY (result);

  /* One of S_IFCHR or S_IFBLK, etc must be selected for mknod */
  if (!S_ISSPECIAL(mode))
    result = -EINVAL;

  /* Create the node */
  if (result == 0) {
    mode &= (S_IFCHR | S_IFBLK | S_IFIFO | S_IFSOCK | 07777);
    ndp->vp = m_create_inode (ndp->dvp->mp, mode, ndp->dvp->p.inode.gid,
        FS_CURR_UID);

    if (ndp->vp == NULL)
      result = -ENOSPC;
  }

  if (result == 0) {
    /* Store the major/minor number in the inode */
    ndp->vp->p.inode.data[0] = dev;

    result = v_efs_link (ndp);
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result != 0) {
    m_rollback_transaction (mp);
    if (ndp->vp != NULL) {
      fs_vnode_invalidate (ndp->vp);
      ndp->vp = NULL;
    }
    /* The root directory would only be modified by v_efs_link.  So, that
     * would have been the last thing that could have failed. */
  } else {
    fs_vnode_flush (ndp->vp);
    fs_vnode_flush (ndp->dvp);
    m_end_transaction (mp);
  }

  return result;
}

static int
v_efs_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz)
{
  int result;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
  const uint8 *value;
  unsigned value_length;

  result = fs_efs2_name_key_lookup (mp->db,
      (const uint8 *) vp->p.named.key, vp->p.named.key_length);
  if (result != 0)
    return result;

  fs_efs2_name_last_value (&value, &value_length);
  if ((value_length - 1) > bufsiz)
    return -ENAMETOOLONG;

  fs_memscpy (buf, bufsiz, value + 1, value_length - 1);

  return value_length - 1;
}

static int
v_efs_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
  fsb_t mp_fs_buf;
  uint32 num_alloc_blocks;

  mp_fs_buf = mp->db->buf;
  num_alloc_blocks = mp_fs_buf->get_num_alloc (mp_fs_buf);
  buf->f_bsize  = mp->cluster_size;
  buf->f_blocks = mp_fs_buf->get_space_limit (mp_fs_buf);
  buf->f_bfree  = buf->f_blocks - num_alloc_blocks;
  buf->f_bavail = buf->f_bfree; /* There is no soft limit */
  buf->f_balloc = mp_fs_buf->get_field (mp_fs_buf, FS_FIELD_GENERAL_POOL);
  /* f_free and f_avail are just made up. These limits don't really exist.
   */
  buf->f_ffree  = 0x01000000;
  buf->f_favail = 0x01000000;
  /* No easy way to compute some of the fields, so just return bogus numbers
   * for now.
   */
  buf->f_files   = 0;
  buf->f_fsid    = 0;
  buf->f_flag    = 0;
  buf->f_namemax = FS_NAME_MAX;

  buf->f_maxwrite = mp->max_transaction;

#ifdef FS_UNIT_TEST
  /* Only compute the hard allocation in the off-target testing, since it
   * is expensive. */
  buf->f_hardalloc = fs_buf_get_hardalloc (mp_fs_buf);
#else
  buf->f_hardalloc = FS_INVALID_HARDALLOC;
#endif

  buf->f_pathmax = FS_PATH_MAX;
  buf->f_is_case_sensitive = 1; /* Path is case-sensitive. */
  buf->f_is_case_preserving = 1; /* Path is case-preserved. */
  buf->f_max_file_size = FS_MAX_FILE_SIZE;
  buf->f_max_file_size_unit = 1;
  buf->f_max_open_files = FS_MAX_DESCRIPTORS;
  buf->f_name_rule = FS_FILENAME_RULE_8BIT_RELAXED;
  buf->f_name_encoding = FS_FILENAME_ENCODING_8BIT;

  return 0;
}

static int
v_efs_symlink_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  int result;
  const uint8 *value;
  unsigned value_length = 0;

  result = read_item (vp, &value, &value_length);
  if (result == 0) {
    value_length -= 1;
  }

  buf->st_dev = vp->dev;
  buf->st_ino = vp->inum;
  buf->st_mode  = vp->mode;
  buf->st_mtime = 0;
  buf->st_ctime = 0;
  buf->st_nlink = 0;
  buf->st_size = value_length;
  buf->st_blksize = ((struct fs_mount_efs *) vp->mp)->cluster_size;
  buf->st_blocks = 0;
  buf->st_atime = 0;
  buf->st_uid = vp->p.named.p.owners.uid;
  buf->st_gid = vp->p.named.p.owners.gid;

  /* This param make sense for the dev special files. */
  buf->st_rdev = 0;

  return 0;
}

static int
v_efs_item_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  int result;
  const uint8 *value;
  unsigned value_length;
  uint32 header_size, size, block_size;
  fs_time_t ctime = 0;
  fs_mode_t mode;

  result = read_item (vp, &value, &value_length);
  if (result != 0)
    return result;

  mode = vp->mode;
  if (value[0] == FS_VAL_ITEM2) {
    struct fs_item_file2_metadata *meta;
    header_size = sizeof (struct fs_item_file2_metadata);
    meta = (struct fs_item_file2_metadata *)value;
    ctime = meta->ctime;
    /* To keep backwards compatability make it look like a regular file. */
    mode &= ~S_IFMT;
    mode |= S_IFREG;

    /* Use valid nlink and to make it look like a regular file. */
    buf->st_nlink = 1;
  } else {
    header_size = ITEM_HEADER_SIZE;

    /* Use item files default values. */
    buf->st_nlink = 0;
  }

  block_size = ((struct fs_mount_efs *) vp->mp)->cluster_size;
  buf->st_blksize = block_size;

  size = buf->st_size = value_length - header_size;
  if (value[0] == FS_VAL_ITEM2 && size != 0)
    buf->st_blocks = (size + (block_size - 1)) / block_size;
  else
    buf->st_blocks = 0;

  buf->st_dev = vp->dev;
  buf->st_ino = vp->inum;
  buf->st_mode  = mode;

  buf->st_atime = ctime;
  buf->st_mtime = ctime;
  buf->st_ctime = ctime;

  /* This param make sense for the dev special files. */
  buf->st_rdev = 0;

  buf->st_uid = vp->p.named.p.owners.uid;
  buf->st_gid = vp->p.named.p.owners.gid;

  return 0;
}

static int
v_efs_devspecial_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  buf->st_dev = vp->dev;
  buf->st_ino = vp->inum;
  buf->st_mode  = vp->mode;
  buf->st_atime = vp->p.inode.atime;
  buf->st_mtime = vp->p.inode.mtime;
  buf->st_ctime = vp->p.inode.ctime;
  buf->st_nlink = vp->p.inode.nlink;
  buf->st_blksize = ((struct fs_mount_efs *) vp->mp)->cluster_size;
  buf->st_blocks = 0;
  buf->st_size = 0;

  /* Device Major/Minor number */
  buf->st_rdev = vp->p.inode.data[0];

  return 0;
}

static int
v_efs_get_item (struct fs_vnode *vp, void *data, int length)
{
  int result;
  unsigned len;
  const uint8 *value;
  unsigned value_length;
  uint32 header_size;

  result = read_item (vp, &value, &value_length);
  if (result != 0)
    return result;

  if (value[0] == FS_VAL_ITEM2)
    header_size = sizeof (struct fs_item_file2_metadata);
  else
    header_size = ITEM_HEADER_SIZE;

  len = value_length - header_size;

  /* We now require that the caller specify the correct
   * length of the item, and no more. */
  if (len != (unsigned) length)
    return -FS_ERANGE;

  /* Only copy the data if length not zero.
   * If length is zero, *data pointer may be NULL, do the check to avoid
   * passing NULL to the lower layer. */
  if (len != 0)
    fs_memscpy (data, length, value + header_size, len);

  return len;
}

/* This function changes the some of the 'mode' bits of a given file
   or directory.  Note that this is only valid for entries that HAVE
   an inode.  Anything else would need a new function.  */
static int
v_efs_chmod (struct nameidata *ndp, fs_mode_t mode)
{
  struct fs_vnode *vp = ndp->vp;
  int result = 0;
  fs_mode_t old_mode;

  /* Only the permission/suid/sticky bits can be set */
  if (mode & ~07777)
    return -EINVAL;

  /* Get the existing mode bits */
  old_mode = vp->p.inode.mode;

  /* After the if above we know that mode == (mode & 07777) */
  if ((old_mode & 07777) == mode)
    return 0;

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0)
  {
    vp->p.inode.mode = (old_mode & ~07777) | mode;
    vp->dirty = 1;
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (vp);
    m_end_transaction (vp->mp);
  } else {
    m_rollback_transaction (vp->mp);
    /* Invalidate it, but don't remove the reference. */
    fs_vnode_ref (vp);
    fs_vnode_invalidate (vp);
  }

  return result;
}

static int
v_efs_item_chmod (struct nameidata *ndp, fs_mode_t mode)
{
  int result;
  struct fs_vnode *dvp = ndp->dvp;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) dvp->mp;
  uint8 mode_low, mode_high;
  const uint8 *value = NULL;
  unsigned value_length = 0;

  /* Only the permission/suid/sticky bits can be set */
  if (mode & ~07777)
    return -EINVAL;

  result = read_item (ndp->vp, &value, &value_length);
  if (result != 0)
    return result;

  mode &= ~S_IFMT;
  mode_low = mode & 0xFF;
  mode_high = ((mode & 0xFF00u) >> 8);

  /* If the mode bits are not being modified, return success. */
  if (value[1] == mode_low && value[2] == mode_high)
    return 0;

  m_start_transaction (dvp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0) {
    /* Copy the whole item file contents. */
    fs_memscpy (static_item_entry, sizeof (static_item_entry), value,
                value_length);

    /* Overwite the permission bits. */
    static_item_entry[1] = mode_low;
    static_item_entry[2] = mode_high;

    /* Write the data to disk. */
    result = fs_efs2_name_add (mp->db,
        ndp->next, ndp->length, dvp->inum,
        static_item_entry, value_length,
        ndp->dvp->p.inode.gid, TRUE, NULL);
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (dvp);
    m_end_transaction (dvp->mp);
  } else {
    m_rollback_transaction (dvp->mp);
  }

  return result;
}

/***********************************************************************
FUNCTION      v_efs_chown

DESCRIPTION   This function changes the group and user id of the given
              file or directory

RETURN VALUE  0 if setting the quota was sucessful. A negative value
              corresponding to the error number if there was a failure.
**********************************************************************/
static int
v_efs_chown (struct fs_vnode *vp,
             int uid_val,
             int gid_val)
{
  int result;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  /* Check if the uid and gid changed. If there are no changes
   * return immediately. */
  if ((uid_val == -1 || uid_val == vp->p.inode.uid) &&
      (gid_val == -1 || gid_val == vp->p.inode.gid))
    return 0;

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0)
    if (S_ISREG(vp->p.inode.mode))
      result = mp->buf->chown(mp->buf, vp->p.inode.uid, uid_val,
          vp->p.inode.gid, gid_val, vp->p.inode.blocks);
  if (result == 0)
  {
    if (uid_val >= 0)
      vp->p.inode.uid = uid_val;
    if (gid_val >= 0)
      vp->p.inode.gid = gid_val;

    vp->dirty = 1;
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (vp);
    m_end_transaction (vp->mp);
  } else {
    m_rollback_transaction (vp->mp);
    /* Invalidate it, but don't remove the reference. */
    fs_vnode_ref (vp);
    fs_vnode_invalidate (vp);
  }

  return result;
} /*end v_efs_chown*/

static int
v_efs_item_chown (struct fs_vnode *vp, int uid_val, int gid_val)
{
  int result;
  const uint8 *value;
  unsigned value_length;

  /* Check if the uid and gid changed. If there are no changes
   * return immediately. */
  if ((uid_val == -1 || uid_val == vp->p.named.p.owners.uid) &&
      (gid_val == -1 || gid_val == vp->p.named.p.owners.gid))
    return 0;

  result = read_item (vp, &value, &value_length);
  if (result != 0)
    return result;

  result = convert_item_to_regular (vp, value, value_length);
  if (result == 0)
    result = vp->vops->chown (vp, uid_val, gid_val);

  return result;
}

/**************************************************************************
FUNCTION      v_efs_utime

DESCRIPTION   Sets atime, mtime and updates ctime for a given vnode. Note
              that this is only valid for entries that HAVE an inode.

RETURN VALUE  0 if sucessful. A negative value corresponding to the error
              number if there was a failure.
*************************************************************************/
static int
v_efs_utime (struct fs_vnode *vp, const struct fs_utimbuf *times)
{
  int result = 0;

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0)
  {
    uint32 curr_time = fs_time();
    if (times != NULL)
    {
      vp->p.inode.atime = times->actime;
      vp->p.inode.mtime = times->modtime;
    }
    else
    {
      vp->p.inode.atime = curr_time;
      vp->p.inode.mtime = curr_time;
    }
    vp->p.inode.ctime = curr_time;
    vp->dirty = 1;
  }

  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (vp);
    m_end_transaction (vp->mp);
  } else {
    m_rollback_transaction (vp->mp);
    /* Invalidate it, but don't remove the reference. */
    fs_vnode_ref (vp);
    fs_vnode_invalidate (vp);
  }

  return result;
}


static int
v_efs_item_utime (struct fs_vnode *vp, const struct fs_utimbuf *times)
{
  int result;
  const uint8 *value;
  unsigned value_length;

  result = read_item (vp, &value, &value_length);
  if (result != 0)
    return result;

  result = convert_item_to_regular (vp, value, value_length);
  if (result == 0)
    result = vp->vops->utime (vp, times);

  return result;
}

static int
v_efs_item_utime_item (struct fs_vnode *vp, const struct fs_utimbuf *times)
{
  int result;
  const uint8 *value;
  unsigned value_length;
  fs_time_t old_time, new_time;
  struct fs_item_file2_metadata *meta;

  result = read_item (vp, &value, &value_length);
  if (result != 0)
    return result;

  /* If it is not item file 2, return error. Item file 1 does not have time
   * field. */
  if (value[0] != FS_VAL_ITEM2)
    return -EPERM;

  if (times == NULL)
    new_time = fs_time();
  else
    new_time = times->modtime;

  meta = (struct fs_item_file2_metadata *)value;
  old_time = meta->ctime;
  /* If there is no change with the time stored, return success. */
  if (new_time == old_time)
    return 0;

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY (result);

  if (result == 0)
  {
    struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;
    /* Copy the whole item file contents. */
    fs_memscpy (static_item_entry, sizeof (static_item_entry), value,
                value_length);

    meta = (struct fs_item_file2_metadata *)static_item_entry;
    meta->ctime = new_time;

    /* Write the data to disk. */
    result = fs_efs2_name_key_add (mp->db,
                                   vp->p.named.key, vp->p.named.key_length,
                                   static_item_entry, value_length,
                                   vp->p.named.p.owners.gid, TRUE, NULL);
  }


  FS_SIMPLE_TRY_CLEANUP ();

  if (result == 0) {
    fs_vnode_flush (vp);
    m_end_transaction (vp->mp);
  } else {
    m_rollback_transaction (vp->mp);
  }

  return result;
}

/***********************************************************************
FUNCTION      v_efs_set_reservation

DESCRIPTION   This function sets a reservation for a given group id

RETURN VALUE  0 if setting the quota was sucessful. A negative value
              corresponding to the error number if there was a failure.
**********************************************************************/
static int
v_efs_set_reservation (struct fs_vnode *vp, uint32 gid, uint32 size)
{
  int result = 0;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  m_start_transaction (vp->mp);
  FS_SIMPLE_TRY(result);
  if (result == 0)
  {
    if (gid != 0)
      result = mp->buf->set_reservation (mp->buf, gid, size);
    else
      result = -EPERM;
  }
  FS_SIMPLE_TRY_CLEANUP();
  if (result < 0)
  {
    m_rollback_transaction(vp->mp);
    /* Invalidate it, but don't remove the reference. */
    fs_vnode_ref (vp);
    fs_vnode_invalidate (vp);
  }
  else
    m_end_transaction (vp->mp);
  return result;

} /* end v_efs_set_reservation */

/***********************************************************************
FUNCTION      v_efs_set_quota

DESCRIPTION   This function sets a quota limit on a given group id

RETURN VALUE  0 if setting the quota was sucessful. A negative value
              corresponding to the error number if there was a failure.
**********************************************************************/
static int
v_efs_set_quota (struct fs_vnode *vp, uint32 gid, uint32 size)
{
  int result = 0;
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  m_start_transaction (vp->mp);

  FS_SIMPLE_TRY(result);
  if (result == 0)
    result = mp->buf->set_quota (mp->buf, gid, size);

  FS_SIMPLE_TRY_CLEANUP();
  if (result < 0) {
    m_rollback_transaction(vp->mp);
    /* Invalidate it, but don't remove the reference. */
    fs_vnode_ref (vp);
    fs_vnode_invalidate (vp);
  }
  else
    m_end_transaction (vp->mp);
  return result;

} /*end v_efs_set_quota */

/***********************************************************************
FUNCTION      v_efs_get_group_info

DESCRIPTION   This function gets the information for a given group id

RETURN VALUE  0 if getting info was sucessful. A negative value corresponding
              to the error number if there was a failure.
**********************************************************************/
static int
v_efs_get_group_info (struct fs_vnode *vp,
    uint32 gid, struct fs_group_info * ginfo)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->get_group_info (mp->buf, gid, ginfo);

} /* end fs_get_group_info */

/***********************************************************************
FUNCTION      v_efs_shred

DESCRIPTION   This function shreds obsolete data from the file system.

RETURN VALUE  0 if the shred is complete, 1 if the shred is incomplete,
              -1 if the shred is not possible.
**********************************************************************/
static int
v_efs_shred (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->shred (mp->buf);

} /* end v_efs_shred */



/***********************************************************************
FUNCTION      v_efs_scrub

DESCRIPTION   This function moves all existing data into new erased bloks.

RETURN VALUE  0 if the scrub is complete, 1 if the scrub is incomplete,
              -1 if the scrub is not possible.
**********************************************************************/

static int
v_efs_scrub (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->scrub (mp->buf);

} /* end v_efs_scrub */



static int
v_efs_sync_start (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->sync_start (mp->buf);
}


static int
v_efs_sync_wait (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->sync_wait (mp->buf);
}

static int
v_efs_sync_end (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->sync_end (mp->buf);
}

static int
v_efs_sync_no_wait (struct fs_vnode *vp, fs_sync_token_t *sync_token)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->sync_no_wait (mp->buf, sync_token);
}

static int
v_efs_sync_get_status (struct fs_vnode *vp, fs_sync_token_t sync_token,
                       enum fs_sync_result *sync_result)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->sync_get_status (mp->buf, sync_token, sync_result);
}

/**********************************************************************
 FUNCTION      v_efs_get_device_attr

 DESCRIPTION   This function gets the attributes of the flash device

 RETURN VALUE  0 if getting device info was sucessful, -1 otherwise
**********************************************************************/
static int
v_efs_get_device_attr (struct fs_vnode *vp, struct fs_device_attr *dev_attr)
{
  enum fs_device_type device_type;
  char *dname;

  (void) vp;

  if (fs_device == NULL)
  {
    return -1;
  }

  dev_attr->device_maker_id = fs_device->device_maker_id(fs_device);
  dev_attr->device_id = fs_device->device_id(fs_device);
  dev_attr->block_count = fs_device->block_count(fs_device);
  dev_attr->block_size = fs_device->block_size(fs_device);
  dev_attr->page_size = fs_device->page_size(fs_device);
  dev_attr->total_page_size = fs_device->total_page_size(fs_device);
  dev_attr->total_blocks = efs2_total_blocks;
  dev_attr->bus_width = efs2_bus_width;

  device_type = fs_device->efs_device_type;
  switch (device_type)
  {
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
      dev_attr->device_type = FS_DEV_TYPE_EFS_NAND;
      break;

    case FS_DEVICE_TYPE_EFS_NOR:
      dev_attr->device_type = FS_DEV_TYPE_EFS_NOR;
      break;

    case FS_DEVICE_TYPE_EFS_MNAND:
      dev_attr->device_type = FS_DEV_TYPE_EFS_MNAND;
      break;

    case FS_DEVICE_TYPE_EFS_RAMFS:
      dev_attr->device_type = FS_DEV_TYPE_EFS_RAMFS;
      break;

    case FS_DEVICE_TYPE_EFS_RMTS:
      dev_attr->device_type = FS_DEV_TYPE_EFS_RMTS;
      break;

    case FS_DEVICE_TYPE_EFS_SPI_NOR:
      dev_attr->device_type = FS_DEV_TYPE_EFS_SPI_NOR;
      break;

    case FS_DEVICE_TYPE_UNKNOWN:
      dev_attr->device_type = FS_DEV_TYPE_UNKNOWN;
      break;

    default:
      FS_ERR_FATAL ("Unknown dev type",0,0,0);
      break;
  }

  memset (dev_attr->device_name, 0, sizeof (dev_attr->device_name));
  dname = fs_device->device_name(fs_device);
  if (dname != NULL)
    strlcpy (dev_attr->device_name, dname, sizeof (dev_attr->device_name));

  return 0;
}

/***********************************************************************
 * FUNCTION      v_efs_get_fs_data
 *
 * DESCRIPTION   Returns the factory image one page at a time. Each
 *               successive call retuns the next page of the factory
 *               data.
 *
 * RETURN VALUE  0 if getting file system data was sucessful, errno otherwise.
 **********************************************************************/
static int
v_efs_get_fs_data (struct fs_vnode *vp,
    struct fs_factimage_read_info *read_info_ptr, byte *page_buffer)
{
  (void) vp; (void) read_info_ptr; (void) page_buffer;
#ifdef FEATURE_EFS_NAND_FACTORY_START
  return fs_fact_get_chunk (read_info_ptr, page_buffer,
      (struct fsb_nand_data *) fs_buf);
#else
  return -EINVAL;
#endif
}

/**********************************************************************
 * FUNCTION      v_efs_image_prepare
 *
 * DESCRIPTION   Performs a single unit of work in preparation for a factory
 *               image. The only work needed to be done at this time is to move
 *               data from the free-tree to the free list.
 *
 * RETURN VALUE  Returns a count indicating any additional work.
 *               0 indicates no work and >0 indicates more work to be done.
 **********************************************************************/
static int
v_efs_image_prepare (struct fs_vnode *vp)
{
  (void) vp;

  /* XXX: For now, just use the root filesystem pointer we have. */

  if (fs_buffer_has_work (fs_buf)) {
    /* XXX: Assumes that we aren't inside of a transaction. */
    fs_buf->start_transaction (fs_buf);
    fs_buf->end_transaction (fs_buf);
    return 1;
  } else
    return 0;
}


/**********************************************************************
 * FUNCTION      v_efs_make_golden_copy
 *
 * DESCRIPTION   Creates a golden copy of the file system. If the file
 *               system gets corrupted some how we fall back to this
 *               copy.
 *
 * RETURN VALUE  On success this function should reset the phone, never
 *               return. On error retun -1.
 **********************************************************************/
static int
v_efs_make_golden_copy (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->make_golden_copy (mp->buf);
}

/**********************************************************************
 * FUNCTION      v_efs_fs_init_complete
 *
 * DESCRIPTION   Signals that efs is now initialized and is ready for a
 *               tar image to be processed if any.
 *
 * RETURN VALUE  On success this function should reset the phone, never
 *               return. On error retun -1.
 **********************************************************************/
static int
v_efs_fs_init_complete (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->fs_init_complete (mp->buf);
}

/**********************************************************************
 * FUNCTION      v_efs_lock_efs
 *
 * DESCRIPTION   Waits for any on-going transaction to complete before
 *               letting the calling task to lock the filesystem.
 *
 * RETURN VALUE  Returns 0 on success and error status on failure.
 **********************************************************************/
static int
v_efs_lock_efs (struct fs_vnode *vp)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->lock_efs (mp->buf);
}


/**********************************************************************
 * FUNCTION      v_efs_borrow_ram
 *
 * DESCRIPTION   Borrows RAM from the lower layers. Data is clobbered.
 *
 * RETURN VALUE  Returns 0 on success and error status on failure.
 **********************************************************************/
static int
v_efs_borrow_ram (struct fs_vnode *vp, void **ram_addr, uint32 *ram_size)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->borrow_ram (mp->buf, ram_addr, ram_size);
}

static int
v_efs_store_data_in_sb (struct fs_vnode *vp, const void *data,
                        uint32 data_size)
{
  struct fs_mount_efs *mp = (struct fs_mount_efs *) vp->mp;

  return mp->buf->store_data_in_sb (mp->buf, data, data_size);
}

/**********************************************************************
 * This cheater function allows us to share an otherwise-static fs
 * buffer structure with the factory startup code.
 * This structure is large, and there is only one statically allocated.
 *
 * The Factory Startup code really doesn't need to keep another one
 * allocated (since it is used only during CEFS startup), so we lend
 * it the one we have in EFS2, while not actually exporting it.
 *
 * An alternative would be to make it prefixed, global instead of
 * static, and perhaps more at risk from external corruption.
 *
 * Note that NO CODE other than factory startup is permitted to borrow
 * the nand data.  This is a special and unique arrangement, done only
 * to conserve RAM footprint.
 **********************************************************************/
struct fsb_nand_data *
fs_borrow_nand_data (void)
{
  return &static_fs_buf;
}

/* Compute the number of shifts needed to get value, value is rounded down
 * to the neares power of two.
 * Another way of seeing it is, returns FLOOR (log2 (value)) */
uint32
fs_get_shifts (uint32 value)
{
  uint32 m = 0;
  while ((value >>= 1) != 0)
    m++;
  return m;
}

#ifdef FS_UNIT_TEST_NOT
/* Debugging utility: Show information about an inode. */
static void
dump_inode (struct fs_mount_efs *mp, fs_inode_type *inode)
{
  (void) mp;
  printf ("Dump inode, size = %d, blocks = %d\n",
      inode->size, inode->blocks);
}
#endif

#endif /* FEATURE_EFS_QEFS2 */

