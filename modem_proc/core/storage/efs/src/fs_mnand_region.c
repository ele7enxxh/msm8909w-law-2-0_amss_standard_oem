/***********************************************************************
 * fs_mnand_region.c
 *
 * Helper file for the mNand PM moudle.
 * Copyright (C) 2008, 2010, 2012 QUALCOMM Technologies, Inc.
 *
 * This is an helper file for the mNnand PM module. This file
 * contains all the region related operations.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_region.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-05-16   nr    Make MNAND cluster/page size configurable at runtime.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-11-15   wek   Make mnand indpendent of efs page size macro.
2010-10-01   wek   Rename mnand macros from clusters to page.
2008-12-12   rp    Create

===========================================================================*/

#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_MNAND) &&    \
    defined (FEATURE_EFS_QEFS2)

#include "fs_mnand_config.h"
#include "fs_mnand_super.h"
#include "fs_buffer.h"
#include "assert.h"


void
fs_mnand_region_create (efs_device_t dev, fs_mnand_super_t super)
{
  uint32 total_sectors;
  uint32 data_clusters, avail_clusters;
  uint32 ptbl_sect, ptbl_clust;
  uint32 bv_sect, bv_clust;
  uint32 tmp;
  const uint32 SECT_PER_CLUST = dev->priv.mnand_dev.sectors_per_page;
  const uint32 PTBL_ENTR_PER_SECT = FS_MNAND_PTBL_ENTRIES_PER_SECT;
  const uint32 BV_ENTR_PER_SECT = FS_MNAND_BIRD_VIEW_ENTRIES_PER_SECT;

  /*---------------------------------------------------------------------------
    Compute total sectors and clusters.
  ---------------------------------------------------------------------------*/
  total_sectors = dev->block_count (dev);

  /* Round off the total-sectors to multiple of clusters. */
  tmp = total_sectors % SECT_PER_CLUST;
  total_sectors -= tmp;
  data_clusters = total_sectors / SECT_PER_CLUST;
  tmp = data_clusters % SECT_PER_CLUST;
  data_clusters -= tmp;
  ASSERT ((data_clusters % SECT_PER_CLUST) == 0);

  /*---------------------------------------------------------------------------
    Configure Reserved region.
  ---------------------------------------------------------------------------*/
  super->data->rsrvd.start = 0;
  super->data->rsrvd.end = FS_MNAND_RESERVED_TOTAL_SECTORS;

  /*---------------------------------------------------------------------------
    Configure LOG region.
  ---------------------------------------------------------------------------*/
  tmp = super->data->rsrvd.end;
  super->data->log.start = tmp + (tmp % SECT_PER_CLUST);
  super->data->log.end = super->data->log.start +
                              FS_MNAND_LOG_TOTAL_SECTORS;

  /* Deduct Reserved and LOG region clusters from the clusters count. */
  tmp = (super->data->log.end + SECT_PER_CLUST -1) / SECT_PER_CLUST;
  ASSERT (tmp < data_clusters);
  data_clusters -= tmp;

  /*---------------------------------------------------------------------------
    Rough estimate for PTBL region.
  ---------------------------------------------------------------------------*/
  /* First attempt to see roughly how many PTBL clusters are needed */
  ptbl_sect = (data_clusters + (PTBL_ENTR_PER_SECT-1)) / PTBL_ENTR_PER_SECT;
  ptbl_clust = (ptbl_sect +(SECT_PER_CLUST-1)) / SECT_PER_CLUST;
  ASSERT (ptbl_clust < data_clusters);
  avail_clusters = data_clusters - (2 * ptbl_clust);

  /*---------------------------------------------------------------------------
    Configure bird-view region.
  ---------------------------------------------------------------------------*/
  bv_sect = (ptbl_sect + (BV_ENTR_PER_SECT-1)) / BV_ENTR_PER_SECT;
  bv_clust = ((bv_sect*2) + (SECT_PER_CLUST-1)) / SECT_PER_CLUST;
  ASSERT (bv_clust < avail_clusters);
  avail_clusters -= bv_clust;
  data_clusters -= bv_clust;

  tmp = super->data->log.end;
  super->data->bv_rptbl.start =  tmp + (tmp % SECT_PER_CLUST);
  super->data->bv_rptbl.end = super->data->bv_rptbl.start + bv_sect;
  super->data->bv_fptbl.start = super->data->bv_rptbl.end;
  super->data->bv_fptbl.end = super->data->bv_fptbl.start + bv_sect;

  /*---------------------------------------------------------------------------
    Now compute the PTBL clusters accurately.
  ---------------------------------------------------------------------------*/
  ptbl_sect = (avail_clusters + PTBL_ENTR_PER_SECT-1) / PTBL_ENTR_PER_SECT;
  ptbl_clust = (ptbl_sect + SECT_PER_CLUST-1) / SECT_PER_CLUST;
  data_clusters -= (2 * ptbl_clust);

  /* configure the PTBL region   */
  tmp = super->data->bv_fptbl.end;
  super->data->rptbl.start = tmp + (tmp % SECT_PER_CLUST);
  super->data->rptbl.end = super->data->rptbl.start + ptbl_sect;
  super->data->fptbl.start = super->data->rptbl.end;
  super->data->fptbl.end = super->data->fptbl.start + ptbl_sect;

  /*---------------------------------------------------------------------------
    Configure the DATA region.
  ---------------------------------------------------------------------------*/
  tmp = total_sectors - (data_clusters * SECT_PER_CLUST);
  super->data->data.start = tmp + (tmp % SECT_PER_CLUST);
  super->data->data.end = total_sectors;

  super->data->page_size = fs_device_page_size (dev);
  super->data->sect_per_clust = SECT_PER_CLUST;
  super->data->block_size = FS_MNAND_PAGES_PER_BLOCK;
  super->data->total_sectors = total_sectors;
}


void
fs_mnand_region_fill (efs_device_t dev, uint32 start_sect, uint32 end_sect,
                      enum fs_flashop_category category)
{
  uint32 bufsize_bytes;
  uint8 *buf;
  uint32 write_size, rem_sect;

  /* Borrow the buffers, we can use these buffers to do the initialization. */
  buf = fs_borrow_data_buffers (&bufsize_bytes);
  ASSERT (buf != NULL);

  /* Compute how many sectors can we write in one shot. */
  write_size = bufsize_bytes / FS_MNAND_SECTOR_SIZE;
  ASSERT (write_size >= 1);

  /* Fill the entire buffer with 0xFF. */
  memset (buf, 0xFF, bufsize_bytes);

  while (start_sect < end_sect)
  {
    rem_sect = end_sect - start_sect;
    if (write_size > rem_sect)
      write_size = rem_sect;

    fs_device_write_sectors (dev, start_sect, buf, write_size, category);
    start_sect += write_size;
  }
}

#endif /* FEATURE_EFS_EFS2_ON_MNAND */

