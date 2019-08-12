/***********************************************************************
 * fs_ramfs_pm.h
 *
 * Page Manager for RamFS.
 * Copyright (C) 2010, 2012-2013 QUALCOMM Technologies, Inc.
 *
 * Page Manager for RamFS.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_ramfs_pm.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-08-19   rp    Support persistence of RAM-FS.
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-05-07   rp    Create

===========================================================================*/

#ifndef __FS_RAMFS_PM_H__
#define __FS_RAMFS_PM_H__

#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_RAMFS) && \
    defined (FEATURE_EFS_QEFS2)

#include "fs_pm.h"
#include "fs_ramfs_config.h"
#include "fs_sys_types.h"


/** Account for the unused space in the superblock. */
#define FS_RAMFS_SUPERBLOCK_PAD_SIZE (FS_RAMFS_PAGE_SIZE_IN_BYTES  - \
                                     (5 * sizeof (uint32)) - \
                                     (FS_UPPER_DATA_COUNT * sizeof (uint32)))

PACKED struct fs_ramfs_superblock_data_type
{
  uint32 header;
  uint32 version;
  uint32 magic1;
  uint32 magic2;
  uint32 age;

  uint32 upper_data[FS_UPPER_DATA_COUNT];

  uint8  pad[FS_RAMFS_SUPERBLOCK_PAD_SIZE];

}PACKED_POST;



struct fs_ramfs_pm_data
{
  struct fs_pm_data     parent; /**< Ops expose to buffer layer. */

  efs_device_t          ramfs_dev; /**< RAM device. */

  uint8                 *ramfs_buff; /**< RAM buffer. */
  uint32                ramfs_size;  /**< RAM buffer size. */
  uint32                page_size;   /**< size of page or cluster */

  struct fs_ramfs_superblock_data_type *superblock;

  uint32                total_clusters; /**< Total number of clusters. */
  int                   inside_txn; /**< Inside a transaction ? */
};


fs_pm_t fs_ramfs_pm_init (efs_device_t dev);
void fs_ramfs_pm_erase_efs (void);


#endif /* FEATURE_EFS_EFS2_ON_RAMFS */

#endif /* not __FS_RAMFS_PM_H__ */
