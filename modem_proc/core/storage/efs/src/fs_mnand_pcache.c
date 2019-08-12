/***********************************************************************
 * fs_mnand_pcache.c
 *
 * Ptable page cache.
 * Copyright (C) 2008,2010,2012 QUALCOMM Technologies, Inc.
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_pcache.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-11-08   wek   Remove impractical and unused paranoia module.
2008-12-12   rp    Create

===========================================================================*/

#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_MNAND) &&    \
    defined (FEATURE_EFS_QEFS2)

#include "fs_mnand_pcache.h"
#include "fs_mnand_config.h"
#include "fs_mnand_bird_view.h"
#include "fs_err.h"
#include "assert.h"
#include "fs_protected_memory.h"

#include <string.h>


#define FS_MNAND_PCACHE_Q_INCR(x)                 \
  do                                              \
  {                                               \
    if ((++x) >= FS_MNAND_MAX_PCACHE_PAGES)       \
      ((x) = 0);                                  \
  } while (0)


/* Buffers to hold the PTbl pages. */
static FS_ALIGN uint32 mnand_pcache_pages[FS_MNAND_MAX_PCACHE_PAGES]
                                         [FS_MNAND_SECTOR_SIZE/sizeof(uint32)];


void
fs_mnand_pcache_init (fs_mnand_pcache_t pcache, efs_device_t dev,
                      fs_mnand_super_t super, int fresh_start)
{
  int16 i;

  pcache->dev = dev;
  pcache->super = super;
  pcache->recent_idx = -1;
  memset (mnand_pcache_pages, 0xFF, sizeof(mnand_pcache_pages));

  for (i=0; i<FS_MNAND_MAX_PCACHE_PAGES; ++i)
  {
    pcache->pcache[i].page_id = FS_INVALID_SECTOR_ID;
    pcache->pcache[i].which_tbl = FS_MNAND_JOURNAL_KEY_TYPE_UNKNOWN;
    pcache->pcache[i].is_dirty = 0;
    pcache->pcache[i].page = &mnand_pcache_pages[i][0];
    ASSERT (FS_OKAY_FOR_DM (pcache->pcache[i].page));
  }

  fs_mnand_bird_view_init (&pcache->bv, dev, super, fresh_start);
}

/** Compute the physical sector for a given logical ptbl page and table. */
static fs_sector_id
fs_mnand_pcache_get_phy_sect (fs_mnand_pcache_t pcache, fs_page_id ptbl_pg,
                              enum fs_journal_key_type which_tbl)
{
  fs_sector_id ptbl_sect;

  ASSERT (ptbl_pg != FS_INVALID_SECTOR_ID);

  if (which_tbl == FS_MNAND_JOURNAL_KEY_TYPE_RTBL)
  {
    ptbl_sect = pcache->super->data->rptbl.start;
  }
  else if (which_tbl == FS_MNAND_JOURNAL_KEY_TYPE_FTBL)
  {
    ptbl_sect = pcache->super->data->fptbl.start;
  }
  else
  {
    FS_ERR_FATAL ("error in which_tbl %d",which_tbl,0,0);
    return FS_INVALID_SECTOR_ID;
  }

  ptbl_sect += ptbl_pg;
  return ptbl_sect;
}

/** Read a PTBL page, consult BirdView before reading and read the PTBL
 *  page only if the PTBL page is valid on the device. */
static void
fs_mnand_pcache_read_page (fs_mnand_pcache_t pcache, fs_sector_id page,
                           enum fs_journal_key_type which_tbl, uint32 *buff)
{
  int is_valid_page;
  fs_sector_id phy_sect;

  /* First consult the BirdView to see if this PTbl page is good in device. */
  is_valid_page = fs_mnand_bird_view_is_valid_page (&pcache->bv,
                            page, which_tbl);
  if (is_valid_page)
  {
    phy_sect = fs_mnand_pcache_get_phy_sect (pcache, page, which_tbl);
    fs_device_read_sectors (pcache->dev, phy_sect, buff, 1, FS_FOP_PTABLE);
  }
  else
  {
    /* This PTbl page is never written on the device. */
    memset (buff, 0xFF, FS_MNAND_SECTOR_SIZE);
  }
}

 /** Write a PTBL page, also update BirdView to mark this PTBL page as valid.*/
static void
fs_mnand_pcache_write_page (fs_mnand_pcache_t pcache, int16 idx)
{
  fs_sector_id phy_sect;

  /* Make sure we have valid page and is also dirty. */
  ASSERT (pcache->pcache[idx].page_id != FS_INVALID_PAGE_ID);
  ASSERT (pcache->pcache[idx].is_dirty != 0);

  /* Compute the physical sector for this page. */
  phy_sect = fs_mnand_pcache_get_phy_sect (pcache,
               pcache->pcache[idx].page_id,
               pcache->pcache[idx].which_tbl);

  fs_device_reliable_write_sectors (pcache->dev, phy_sect,
                                   pcache->pcache[idx].page, 1, FS_FOP_PTABLE);

  /* Now update the BirdView's 2-bits to mark this PTbl page as written. */
  fs_mnand_bird_view_update (&pcache->bv,
        pcache->pcache[idx].page_id, pcache->pcache[idx].which_tbl);

  /* This slot is now available for any other PTbl page. */
  pcache->pcache[idx].is_dirty = 0;
}

/** Search in the PCache for a give Ptbl page and table.  */
static int16
fs_mnand_pcache_search_page (fs_mnand_pcache_t pcache, fs_sector_id page,
                            enum fs_journal_key_type which_tbl)
{
  int16 i;

  for (i=0; i<FS_MNAND_MAX_PCACHE_PAGES; ++i)
  {
    if ( (pcache->pcache[i].page_id == page) &&
         (pcache->pcache[i].which_tbl == which_tbl) )
    {
      return i;
    }
  }

  return -1;
}

/** This function decides which is the best slot to a new PTbl page
  * on the PCache queue. */
static int16
fs_mnand_pcache_get_best_free_slot (fs_mnand_pcache_t pcache)
{
  int16 i, stop, use = -1;

  /* Oldest pages are from the right of the the recent_idx onwards. */
  stop = pcache->recent_idx;
  FS_MNAND_PCACHE_Q_INCR (pcache->recent_idx);

  /* Starting from the right of the recent_idx, lets go through the PCache
   * array and find an unused slot. */
   i = pcache->recent_idx;
  while (i != stop)
  {
    if (pcache->pcache[i].page_id == FS_INVALID_SECTOR_ID)
      return i; /* We found an empty spot, we can use this. */

    if ( (use == -1) && (!pcache->pcache[i].is_dirty) )
      use = i; /* Oldest non-dirty page. */

    FS_MNAND_PCACHE_Q_INCR (i);
  }

  if (use == -1)
    FS_ERR_FATAL ("No free slot in pcache...",0,0,0);

  return use;
}

static int16
fs_mnand_pcache_add_page (fs_mnand_pcache_t pcache, fs_sector_id page,
                          enum fs_journal_key_type which_tbl)
{
  int16 idx = -1;

  /* Select a best slot in the queue to add this PTbl page. */
  idx = fs_mnand_pcache_get_best_free_slot (pcache);

  /* Add the new page at this slot. */
  pcache->pcache[idx].page_id = page;
  pcache->pcache[idx].which_tbl = which_tbl;
  pcache->pcache[idx].is_dirty = 0;
  fs_mnand_pcache_read_page (pcache, page, which_tbl,
                             pcache->pcache[idx].page);
  return idx;
}


uint32*
fs_mnand_pcache_lookup (fs_mnand_pcache_t pcache, fs_page_id ptbl_pg,
                        enum fs_journal_key_type which_tbl, int is_to_write)
{
  int idx;

  idx = fs_mnand_pcache_search_page (pcache, ptbl_pg, which_tbl);
  if (idx == -1)
  {
    /* page not found in pcache, so add it. */
    ++fs_flash_pcache_miss_counter;
    idx = fs_mnand_pcache_add_page (pcache, ptbl_pg, which_tbl);
  }
  else
  {
    /* good, we found the page in the pcache. */
    ++fs_flash_pcache_hit_counter;
  }

  ASSERT (idx != -1);
  pcache->pcache[idx].is_dirty += is_to_write;
  return pcache->pcache[idx].page;
}

void
fs_mnand_pcache_write (fs_mnand_pcache_t pcache, fs_sector_id ptbl_pg,
                       enum fs_journal_key_type which_tbl)
{
  int16 idx;

  ASSERT (ptbl_pg != FS_INVALID_SECTOR_ID);

  /* This page must be present in our pcache. */
  idx = fs_mnand_pcache_search_page (pcache, ptbl_pg, which_tbl);
  ASSERT (idx != -1);

  fs_mnand_pcache_write_page (pcache, idx);
}

#endif /* FEATURE_EFS_EFS2_ON_MNAND */
