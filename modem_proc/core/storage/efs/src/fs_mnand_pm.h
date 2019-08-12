/***********************************************************************
 * fs_mnand_pm.h
 *
 * Page Manager for mNand.
 * Copyright (C) 2008,2010,2012-2013 QUALCOMM Technologies, Inc.
 *
 * The Page Manager exposes the mNand device as a set of clusters to the
 * buffer layer. The buffer layer then can use the PM ops to read/write
 * the clusters at any time.
 *
 * The PM maintains the transaction state, so that the buffer-layers
 * writes either all of them make it to the mNand device or none at
 * all, thus power-safe.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_pm.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2012-05-16   nr    Make MNAND cluster/page size configurable at runtime.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-07-30   wek   Dynamically allocate all buffers dependent on page size.
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_MNAND_PM_H__
#define __FS_MNAND_PM_H__

#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_MNAND)&& \
    defined (FEATURE_EFS_QEFS2)

#include "fs_mnand_config.h"
#include "fs_sys_types.h"
#include "fs_pm.h"
#include "fs_mnand_super.h"
#include "fs_mnand_ptbl.h"
#include "fs_mnand_log.h"


/** The PM module's main structre. */
struct fs_mnand_pm
{
  struct fs_pm_data                 parent; /* Ops expose to buffer layer. */

  efs_device_t                      dev;
  struct fs_mnand_super             super;
  struct fs_mnand_ptbl              ptbl;
  struct fs_mnand_log               log;

  int                               fresh_start;

  uint32                            min_page;
  uint32                            max_page;
  uint32                            avail_pages;
  uint32                            page_size;
  uint32                            sectors_per_page;

  fs_page_id                        solid_alloc;
  fs_page_id                        liquid_alloc;

  uint8                             inside_xact;
  int                               write_super_pg;

  uint8                            *combined_buffer;
  uint32                            combined_buffer_size;
};
typedef struct fs_mnand_pm* fs_mnand_pm_t;


/*************************************************************************//**
 * Initialize the PM module. Also initializes the mNand sub-modules.
 * When this function returns, the mNand device is good and the buffer layer
 * use the mNand device now.
 *
 * @param dev
 *    The dev pointer to the mnand device.
 *
 * @return
 *    An instannce of the mNand page manager.
*****************************************************************************/
fs_pm_t fs_mnand_pm_init (efs_device_t dev);

/*************************************************************************//**
 * Replays a single log-entry. This function is called by the log-module
 * fs_mnand_log_replay_logs(), while replaying the logs during power-up.
 *
 * @param pm_data
 *    PM's data pointer, passed as void * to fs_mnand_log_replay_logs().
 *
 * @param log_code
 *    The log-code from fs_mnand_log.h.
 *
 * @param args
 *    The arguments for this log entry, depending upon the log-code upto
 * 4 argumens will be passed in this array.
 *
*****************************************************************************/
void fs_mnand_pm_replay_log_entry (void *pm_data, uint8 log_code,
                         uint32 args_cnt, uint32 args[4]);

/*************************************************************************//**
 * Writes all 0xFF to the super-block, equivalent to erasing the entire
 * EFS2 partition. Upon next boot, a fresh EFS2 file system will be started.
 *
*****************************************************************************/
void fs_mnand_pm_erase_efs (void);


/*************************************************************************//**
 * Creates mNand regions. On a fresh_start, the super module will call this
 * function to create the mNand regions.
 *
 * @param dev
 *    The dev pointer to the mnand device.
 *
 * @param super
 *    The super blocks region members are intialized to the new region values.
 *
*****************************************************************************/
extern void fs_mnand_region_create (efs_device_t dev, fs_mnand_super_t super);


/*************************************************************************//**
 * Fills the given range of sectors to 0xFF. This function efficiently fills
 * up the region by borrowing the biggest available buffers from the Buffer
 * module and tries to fill the region with as least as possible device write
 * calls.
 *
 * @param dev
 *    The dev pointer to the mnand device.
 *
 * @param start_sect
 *    The starting_sector(inclusive) from where to start the filling.
 *
 * @param end_sect
 *    Till where to fill. Fills upto (end_sect-1), so end_sect is
 * non-inclusive.
 *
*****************************************************************************/
extern void fs_mnand_region_fill (efs_device_t dev,
                                  uint32 start_sect, uint32 end_sect,
                                  enum fs_flashop_category category);

#ifdef FS_UNIT_TEST
fs_sector_id fs_mnand_pm_liquid_alloc_for_sim (fs_mnand_pm_t pm);
fs_sector_id fs_mnand_pm_solid_alloc_for_sim (fs_mnand_pm_t pm);
uint32 fs_mnand_pm_block_nbr_for_sim (uint32 page);
#endif /* FS_UNIT_TEST */


#endif /* FEATURE_EFS_EFS2_ON_MNAND */

#endif /* not __FS_MNAND_PM_H__ */

