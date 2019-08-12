/***********************************************************************
 * fs_mnand_super.h
 *
 * mNand Super module.
 * Copyright (C) 2008, 2012 QUALCOMM Technologies, Inc.
 *
 * This file contains all the mNand super module logic.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_super.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_MNAND_SUPER_H__
#define __FS_MNAND_SUPER_H__

#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

#include "fs_mnand_config.h"
#include "comdef.h"
#include "fs_sys_types.h"


/** Account for the unused space the super-page.  */
#define FS_MNAND_SUPER_BLOCK_PAD_SIZE   (FS_MNAND_SUPER_BLOCK_SIZE - (5*4) - \
                                        (4*4) - (8*7) - (4*4) - (32*4) -4)

struct fs_mnand_region
{
  uint32    start;
  uint32    end;
};

/** This is the layout of the superblock. */
struct fs_mnand_superblock_data
{
  uint32                    page_header;
  uint32                    version;
  uint32                    age;
  uint32                    magic1;
  uint32                    magic2;

  uint32                    page_size;
  uint32                    sect_per_clust;
  uint32                    block_size;
  uint32                    total_sectors;

  struct fs_mnand_region    rsrvd;
  struct fs_mnand_region    log;
  struct fs_mnand_region    bv_rptbl;
  struct fs_mnand_region    bv_fptbl;
  struct fs_mnand_region    rptbl;
  struct fs_mnand_region    fptbl;
  struct fs_mnand_region    data;

  fs_sector_id              log_head;
  fs_sector_id              solid_alloc;
  fs_sector_id              liquid_alloc;

  fs_sector_id              gc;

  uint32                    upper_data[FS_UPPER_DATA_COUNT];

  uint8                     pad[FS_MNAND_SUPER_BLOCK_PAD_SIZE];

  uint32                    crc32;
};
typedef struct fs_mnand_superblock_data *fs_mnand_superblock_data_t;

/** This is main structure for the super module. */
struct fs_mnand_super
{
  struct fs_mnand_superblock_data    *data;
  efs_device_t                       dev;
  fs_sector_id                       super_sector;
};
typedef struct fs_mnand_super* fs_mnand_super_t;


/*************************************************************************//**
 * Initialize the Super module. Searches for a superblock, creates one if
 * none found.
 *
 * @param super
 *    The Superblock structure.
 *
 * @param dev
 *    The dev pointer to the mnand device.
 *
 * @param fresh_start
 *    This parameter will be:
 *        Set to 1, if there were no superblocks and one has been created now.
 *        Set to 0, if we found a good superblock.
*****************************************************************************/
void fs_mnand_super_init (fs_mnand_super_t super, efs_device_t dev,
                          int *fresh_start);


/*************************************************************************//**
 * Flushes the superblock structure.
 *
 * @param super
 *    The Superblock structure.
 *
*****************************************************************************/
void fs_mnand_super_write_super (fs_mnand_super_t super);

/*************************************************************************//**
 * Erase the super-page, fills the super-page to 0xFF.
 *
 * @param super
 *    The Superblock structure.
 *
*****************************************************************************/
 void fs_mnand_super_erase_super_page (fs_mnand_super_t super);


#endif /* FEATURE_EFS_EFS2_ON_MNAND */


#endif /* not __FS_MNAND_SUPER_H__ */

