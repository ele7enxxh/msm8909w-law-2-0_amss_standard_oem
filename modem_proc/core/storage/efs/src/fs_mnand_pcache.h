/***********************************************************************
 * fs_mnand_pcache.h
 *
 * Ptable page cache.
 * Copyright (C) 2008, 2012 QUALCOMM Technologies, Inc.
 *
 * The PTable pages will be used very frequently by PM and PTbl, so the
 * latest PTbl pages are cached in this file.
 *
 * The PCache module will consult with the BirdView module to check for
 * the status of an PTbl page.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_pcache.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_MNAND_PCACHE_H__
#define __FS_MNAND_PCACHE_H__

#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

#include "fs_mnand_config.h"
#include "fs_sys_types.h"
#include "fs_mnand_super.h"
#include "fs_mnand_bird_view.h"


/** How many PTbl pages to cache? */
#define FS_MNAND_MAX_PCACHE_PAGES     (10)


/** This is how one PTbl page will look in the cache. */
struct fs_mnand_pcache_page
{
  fs_sector_id                  page_id;    /**< Logical sector     */
  enum fs_journal_key_type      which_tbl;  /**< Rev/Fwd table.     */
  int                           is_dirty;   /**< opened for write?  */
  uint32                        *page;      /**< Page data          */
};

struct fs_mnand_pcache
{
  efs_device_t                          dev;
  fs_mnand_super_t                      super;

  struct fs_mnand_bird_view             bv;

  /** The PCache queue.  */
  struct fs_mnand_pcache_page           pcache[FS_MNAND_MAX_PCACHE_PAGES];
  int16                                 recent_idx;
};
typedef struct fs_mnand_pcache* fs_mnand_pcache_t;


/*************************************************************************//**
 * Initialize the PCache module.
 *
 * @param pcache
 *    The PCache structure.
 *
 * @param dev
 *    The dev pointer to the mnand device.
 *
 * @param super
 *    Superblock structure.
 *
 * @param fresh_start
 *    Is this a fresh start? On a fresh-start, the BirdView will initialize the
 *    PTbl region.
 *
*****************************************************************************/
void fs_mnand_pcache_init (fs_mnand_pcache_t pcache, efs_device_t dev,
            fs_mnand_super_t super, int fresh_start);

/*************************************************************************//**
 * Get a PTable page for reading/writing. If the PTable page was not found in
 * the PCache, then it is added.
 *
 * @param pcache
 *    The PCache structure.
 *
 * @param ptbl_pg
 *    Which ptbl_pg to read.
 *
 * @param which_tbl
 *    Reverse/Forward page table.
 *
 * @param is_to_write
 *    Are we opening this PTbl page for writing, then the PTbl is marked
 * as dirty and will not be give up the spot for another PTbl page until
 * this page is written.
 *
*****************************************************************************/
uint32* fs_mnand_pcache_lookup (fs_mnand_pcache_t pcache, fs_page_id ptbl_pg,
            enum fs_journal_key_type which_tbl, int is_to_write);

/*************************************************************************//**
 * Write the given PTbl pageto the device.
 *
 * Note we should have looked up this PTbl page before using the
 * fs_mnand_pcache_lookup().
 *
 * @param pcache
 *    The PCache structure.
 *
 * @param ptbl_pg
 *    Which ptbl_pg to write.
 *
 * @param which_tbl
 *    Reverse/Forward page table.
 *
*****************************************************************************/
void fs_mnand_pcache_write (fs_mnand_pcache_t pcache, fs_sector_id ptbl_pg,
                            enum fs_journal_key_type which_tbl);

#endif /* FEATURE_EFS_EFS2_ON_MNAND */

#endif /* not __FS_MNAND_PCACHE_H__ */
