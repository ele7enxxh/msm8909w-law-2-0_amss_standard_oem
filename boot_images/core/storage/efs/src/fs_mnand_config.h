/***********************************************************************
 * fs_mnand_config.h
 *
 * Config file for mnand.
 * Copyright (C) 2008, 2010-2012 Qualcomm Technologies, Inc.
 *
 * Configuration file for mnand.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_mnand_config.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-05-16   nr    Make MNAND cluster/page size configurable at runtime.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2011-10-10   dks   Make EFS VU independent of Hotplug and Hfat VUs.
2011-06-08   wek   Use every page size in off target testing.
2010-11-15   wek   Make mnand indpendent of efs page size macro.
2010-10-01   wek   Rename mnand macros from clusters to page.
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_MNAND_CONFIG_H__
#define __FS_MNAND_CONFIG_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

#ifndef FEATURE_FS_HOTPLUG
  #error "MNAND needs HOTPLUG to be enabled\n\n"
#endif

/** How big is a sector and cluster in Bytes */
#define FS_MNAND_SECTOR_SIZE              (FS_SDCC_SECTOR_SIZE_IN_BYTES)

/** Size in bytes of the fundamental page size used by MNAND page manager.
 * Throughput in eMMC improves by writing large portions of data at once.
 * A default page size of 2048 was chosen as a compromise to improve write
 * throughput while keeping the page size small to better divide the data,
 * to avoid wasting space. */
#ifndef FS_MNAND_PAGE_SIZE
  #define FS_MNAND_PAGE_SIZE           2048
#endif

/** How big is a logical block */
#ifndef FS_MNAND_PAGES_PER_BLOCK
  #define FS_MNAND_PAGES_PER_BLOCK     (64)
#endif

/** How many Page-Table entries fit in one sector */
#define FS_MNAND_PTBL_ENTRIES_PER_SECT    (FS_MNAND_SECTOR_SIZE/4)

/** How many Bird-View entries fit in one sector, 1 BV entry is 2 bits. */
#define FS_MNAND_BIRD_VIEW_ENTRIES_SIZE_IN_BITS  (2)
#define FS_MNAND_BIRD_VIEW_ENTRIES_PER_SECT      ( (FS_MNAND_SECTOR_SIZE*8) \
                                / FS_MNAND_BIRD_VIEW_ENTRIES_SIZE_IN_BITS )

/** Reserved region [ also contains superblock. ] */
#ifndef FS_MNAND_RESERVED_TOTAL_SECTORS
  #define FS_MNAND_RESERVED_TOTAL_SECTORS         (16)
#endif
#define FS_MNAND_SUPER_SECTOR_ID                  (0)
#ifndef FS_MNAND_SUPER_BLOCK_SIZE
  #define FS_MNAND_SUPER_BLOCK_SIZE               (FS_MNAND_SECTOR_SIZE*1)
#endif
#define FS_MNAND_SUPER_BLOCK_SECTORS_PER_PAGE     (FS_MNAND_SUPER_BLOCK_SIZE  \
                                                        /FS_MNAND_SECTOR_SIZE)


/** Configure log region, */
#ifndef FS_MNAND_LOG_TOTAL_SECTORS
  #define FS_MNAND_LOG_TOTAL_SECTORS      (128)
#endif
#ifndef FS_MNAND_LOG_PAGE_SIZE
  #define FS_MNAND_LOG_PAGE_SIZE          (FS_MNAND_SECTOR_SIZE*1)
#endif
#define FS_MNAND_LOG_SECTORS_PER_PAGE     (FS_MNAND_LOG_PAGE_SIZE   \
                                              /FS_MNAND_SECTOR_SIZE)


/** How much space to reserve as PM overhead in data region */
#ifndef FS_MNAND_OVERHEAD_PERCENT
  #define FS_MNAND_OVERHEAD_PERCENT       (3)
#endif

/* Configure Log-Queue. How many max written-logs in the Log-Q */
#define FS_MNAND_LOG_WRITTEN_LOGS         (20)
#define FS_MNAND_LOG_ALLOCATED_LOGS       (10)
#define FS_MNAND_LOG_QUEUE_SIZE           (FS_MNAND_LOG_WRITTEN_LOGS + \
                                           FS_MNAND_LOG_ALLOCATED_LOGS + 10)

#endif /* FEATURE_EFS_EFS2_ON_MNAND */

#endif /* not __FS_MNAND_CONFIG_H__ */
