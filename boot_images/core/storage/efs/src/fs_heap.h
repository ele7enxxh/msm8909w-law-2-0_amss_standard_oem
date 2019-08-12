/***********************************************************************
 * fs_heap.h
 *
 * Definitions for the heap size.
 * Copyright (C) 2010-2013 Qualcomm Technologies, Inc.
 *
 * We have a buffer or heap to use for all of EFS buffers. fs_malloc
 * we allocate memory from a buffer sequentially. The size of this
 * buffer depends on features and maximum characteristics of the
 * supported device. In this header file we calculate the size of the
 * buffer depending on build configuration.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_heap.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-07   wek   Add a default heap size when QEFS2 is not defined.
2012-08-22   rp    Remove FDI.
2011-06-16   wek   Remove buffers for read pointer in SPI NOR.
2010-12-22   wek   Remove FTL from memory allocation from the heap.
2010-11-08   wek   Remove impractical and unused paranoia module.
2010-10-01   wek   Rename mnand macros from clusters to page.
2010-09-01   wek   Remove compile time calculations based on page size.
2010-07-30   wek   Create. Dynamically allocate all buffers.

===========================================================================*/

#ifndef __FS_HEAP_H__
#define __FS_HEAP_H__

#include "fs_config_i.h"
#include "fs_mnand_config.h"
#include "fs_pm_ptable.h"

/* If we are using EFS we need to reserve space for all its buffers. */
#ifdef FEATURE_EFS_QEFS2

/* Buffers used for EFS, layers above PMs, they are allocated in fs_efs2.c.
 * FS_BUFFER_NUMBER_BUFFERS for EFS buffers
 * FS_MAX_EFS_MOUNTS each efs mount has a page size buffer for DB workspace. */
#define FS_EFS_BUFFERS     (FS_MAX_PAGE_SIZE * \
                            (FS_BUFFER_NUMBER_BUFFERS + FS_MAX_EFS_MOUNTS))

#if (defined FEATURE_EFS_EFS2_ON_NAND     || \
     defined FEATURE_EFS_EFS2_ON_NOR      || \
     defined FEATURE_EFS_EFS2_ON_SPI_NOR)
  /* By enabling EFS on a flash device we will need the following buffers
   * of page size:
   * 2 from fs_efs2.c, pm realloc buffer and pm log buffer.
   * 2 + FS_MAX_CACHED_PTABLE_PAGES form fs_pm.c, ptable realoc buffer and
   *    ptable cache
   * 1 from fs_pm_log.c one time temporary use. */
  #define FS_BUFFERS_FOR_NAND_NOR   (FS_MAX_PAGE_SIZE * \
                                     (FS_MAX_CACHED_PTABLE_PAGES + 5))
#else
  #define FS_BUFFERS_FOR_NAND_NOR   0
#endif


#if (defined FEATURE_EFS_EFS2_ON_NOR       || \
     defined FEATURE_EFS_EFS2_ON_SPI_NOR)
  /* All extra NOR buffers used in fs_pm_ptable_nor.c */
  /* Reverse page table cache in NOR is unaligned and it is not a multiple
   * of page size or alingh bytes. Add align bytes to make sure we don't
   * run out of space when we align all other buffers. We only need to round
   * up to FS_DM_ALIGN_BYTES multiple, that gets complicated. */
  #define FS_NOR_PTABLE_RCACHE (FS_DM_ALIGN_BYTES +                       \
             (FS_PTABLE_MAX_RCACHE * sizeof (struct fs_ptable_rcache_entry)))
#else
  #define FS_NOR_PTABLE_RCACHE          0
#endif

#ifdef FEATURE_EFS_NAND_FACTORY_START
  /* Temporary use during efs_get_fs_data to get factory data in
     fs_factory.c */
  #define FS_FACTORY_PAGE_BUFFERS       (FS_MAX_PAGE_SIZE * 1)
#else
  #define FS_FACTORY_PAGE_BUFFERS       0
#endif

#ifdef FEATURE_EFS_EFS2_ON_MNAND
  /* fs_mnand_pm.c has a buffer to combine all the data to be written. */
  #define FS_MNAND_COMBINED_BUFFER     (FS_MAX_IOVEC_ITEMS_COUNT * \
                                        FS_MNAND_PAGE_SIZE)
#else
  #define FS_MNAND_COMBINED_BUFFER      0
#endif


#if (defined FEATURE_EFS_EFS2_ON_NAND     || \
     defined FEATURE_EFS_EFS2_ON_MNAND)
/* The journal code only runs for NAND and eMMC page manger.
 * Journal has 8 arrays, each of different sizes calculated below using sizeof.
 * One uint8 array size has to be a multiple of 8, therefore we add 7 bytes
 * to guarantee space for it. Journal is unaligned, therefore we add
 * FS_DM_ALIGN_BYTES to guarantee space for other alignments.       */
  #define FS_JOURNAL_BUFFERS (FS_DM_ALIGN_BYTES + FS_MAX_JOURNAL_SIZE *   \
                         (3 * sizeof (fs_j_index) + 2 * sizeof (uint32) + \
                          2 * sizeof (uint8)) +                           \
                         ((FS_MAX_JOURNAL_SIZE + 7) / 8) * sizeof (uint8))
#else
#define FS_JOURNAL_BUFFERS 0
#endif

#define FS_MALLOC_BUFFER_SIZE (FS_EFS_BUFFERS + FS_BUFFERS_FOR_NAND_NOR + \
                     FS_NOR_PTABLE_RCACHE + FS_FACTORY_PAGE_BUFFERS + \
                     FS_MNAND_COMBINED_BUFFER + FS_JOURNAL_BUFFERS)
#else
/* EFS2 is disabled, there are no allocations because of page size
 * dependency. Allocate the smalles buffer possible. */
#define FS_MALLOC_BUFFER_SIZE 1

#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_HEAP_H__ */
