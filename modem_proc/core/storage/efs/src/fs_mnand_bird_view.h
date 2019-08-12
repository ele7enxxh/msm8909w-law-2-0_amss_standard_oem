/***********************************************************************
 * fs_mnand_bird_view.h
 *
 * BirdView module for the mnand.
 * Copyright (C) 2008, 2012-2013 QUALCOMM Technologies, Inc.
 *
 * The mnand has flat Ptbl layout(only level-0), so on a large mnand EFS
 * partition, on a fresh_start, we will have huge amount of Ptbl pages
 * to initialize, so to avoid such huge writes at fs_fresh_start, we have
 * this BirdView of the Ptbl pages.
 * In the BirdView, each Ptbl page will consume 2-bits, and in these 2 status
 * bits, we will track the status of a Ptbl page (like non-written/written etc)
 *
 * Also in future, we can make the page-manager to do allocations by checking
 * these Ptbl status bits to figure out how many pages are available in a given
 * block.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_bird_view.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-10   wek   Move enum to source file.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_MNAND_BIRD_VIEW_H__
#define __FS_MNAND_BIRD_VIEW_H__

#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

#include "fs_mnand_config.h"
#include "fs_sys_types.h"
#include "fs_mnand_super.h"


/** How many Bird-View pages to cache? */
#define FS_MNAND_MAX_BIRD_VIEW_PAGES     (4)


/** Which Table, Reverse/Forward */
enum fs_journal_key_type
{
  FS_MNAND_JOURNAL_KEY_TYPE_RTBL    = 0,
  FS_MNAND_JOURNAL_KEY_TYPE_FTBL    = 1,
  FS_MNAND_JOURNAL_KEY_TYPE_UNKNOWN = 2
};


/** This is how one BirdView page will look in the cache. */
struct fs_mnand_bird_view_page
{
  fs_sector_id                  page_id;    /**< logical sector id */
  enum fs_journal_key_type      which_tbl;  /**< Rev/Fwd table.    */
  uint8                         *page;      /**< Page data.        */
};


/** The BirdView module's main structre. */
struct fs_mnand_bird_view
{
  efs_device_t                      dev;
  fs_mnand_super_t                  super;

  /** Cache of BirdView pages. */
  struct fs_mnand_bird_view_page    bv_cache[FS_MNAND_MAX_BIRD_VIEW_PAGES];
  int16                             recent_idx; /*< Recently added page idx */
};
typedef struct fs_mnand_bird_view* fs_mnand_bird_view_t;


/*************************************************************************//**
 * Initialize the BirdView module.
 *
 * @param pbv
 *    The BirdView structure.
 *
 * @param dev
 *    The dev pointer to the mnand device.
 *
 * @param super
 *    Superblock structure.
 *
 * @param fresh_start
 *    On a fresh_start, this function will write to all the BirdView pages
 * to initialze all the Ptbl status bits to PAGE_STATUS_NOT_WRITTEN.
 *
*****************************************************************************/
void fs_mnand_bird_view_init (fs_mnand_bird_view_t pbv, efs_device_t dev,
                              fs_mnand_super_t super, int fresh_start);


/*************************************************************************//**
 * Given a Reverse/Forward Ptbl page, this function will return 1 if the Ptbl
 * page has been written atleast once, and 0 if the Ptbl page has never been
 * written.
 *
 *The PCache module will use this function to figure out if a PTbl
 * page has ever been written (hence initailized) or not.
 *
 * @param pbv
 *    The BirdView structure.
 *
 * @param ptbl_pg
 *    Which Ptbl page?
 *
 * @param which_tbl
 *    Reverse or Forware page table.
 *
 * @return
 *    1, if the Ptbl-page has even been written (hence initialized).
 *    0, if the Ptbl-page has never been written (hence non-initialized).
*****************************************************************************/
int fs_mnand_bird_view_is_valid_page (fs_mnand_bird_view_t pbv,
                                      fs_page_id ptbl_pg,
                                      enum fs_journal_key_type which_tbl);


/*************************************************************************//**
 * This function will mark a given Ptbl page as written, It sets the 2 bits
 * of the given Ptbl page to FS_MNAND_PTBL_PAGE_STATUS_WRITTEN.
 *
 * The PCache module will call this function after writing an PTbl page.
 *
 * Note that if the given PTbl page is already marked as written, then this
 * function doesn't write it again.
 *
 * @param pbv
 *    The BirdView structure.
 *
 * @param ptbl_pg
 *    Which Ptbl page?
 *
 * @param which_tbl
 *    Reverse or Forware page table.
 *
*****************************************************************************/
void fs_mnand_bird_view_update (fs_mnand_bird_view_t pbv, fs_page_id ptbl_pg,
                                enum fs_journal_key_type which_tbl);


#endif /* FEATURE_EFS_EFS2_ON_MNAND */


#endif /* not __FS_MNAND_BIRD_VIEW_H__ */
