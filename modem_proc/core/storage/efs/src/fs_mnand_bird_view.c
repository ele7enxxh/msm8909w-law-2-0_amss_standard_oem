/***********************************************************************
 * fs_mnand_bird_view.c
 *
 * BirdView module for the mnand.
 * Copyright (C) 2008,2010,2012-2013 QUALCOMM Technologies, Inc.
 *
 * The mnand has flat Ptbl layout(only level-0), so on a large mnand EFS
 * partition, on a fresh_start, we will have huge amount of Ptbl sectors
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_bird_view.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-10   wek   Move enum to source file.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-11-08   wek   Remove impractical and unused paranoia module.
2008-12-12   rp    Create

===========================================================================*/

#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_MNAND) &&    \
    defined (FEATURE_EFS_QEFS2)

#include "fs_mnand_bird_view.h"
#include "fs_mnand_config.h"
#include "fs_mnand_pm.h"
#include "fs_err.h"
#include "assert.h"
#include "fs_protected_memory.h"

#include <string.h>


/** Get the Ptbl status bits at the given bit-position (bp). */
#define FS_MNAND_BIRD_VIEW_GET_STATUS_BITS(val, bp)   \
    ( ((val) & (3<<(8-(bp)-2))) >> (8-(bp)-2) )

/** Set Ptbl status bits at the given bit-position (bp) with new value(nval) */
#define FS_MNAND_BIRD_VIEW_SET_STATUS_BITS(val, bp, nval)  \
  (uint8)( ((val) & (~(uint8)(3<<(8-(bp)-2)))) | ((nval)<<(8-(bp)-2)) )


/** Increment BirdView-Queue variable handling the wrapping. */
#define FS_MNAND_BIRD_VIEW_Q_INCR(x)              \
  do                                              \
  {                                               \
    if ((++x) >= FS_MNAND_MAX_BIRD_VIEW_PAGES)    \
      ((x) = 0);                                  \
  } while (0)


/** The 2-bit Ptbl status bits values. */
enum fs_mnand_ptbl_page_status
{
  FS_MNAND_PTBL_PAGE_STATUS_WRITTEN = 0,
  FS_MNAND_PTBL_PAGE_STATUS_NOT_WRITTEN = 3
};


/** Cached Bird-View sectors. */
static FS_ALIGN uint8
   mnand_bird_view_pages[FS_MNAND_MAX_BIRD_VIEW_PAGES][FS_MNAND_SECTOR_SIZE];



void
fs_mnand_bird_view_init (fs_mnand_bird_view_t pbv, efs_device_t dev,
                         fs_mnand_super_t super, int fresh_start)
{
  int16 i;

  pbv->dev = dev;
  pbv->super = super;
  pbv->recent_idx = -1;
  memset (mnand_bird_view_pages, 0xFF, sizeof(mnand_bird_view_pages));

  for (i=0; i<FS_MNAND_MAX_BIRD_VIEW_PAGES; ++i)
  {
    pbv->bv_cache[i].page_id = FS_INVALID_SECTOR_ID;
    pbv->bv_cache[i].which_tbl = FS_MNAND_JOURNAL_KEY_TYPE_UNKNOWN;
     pbv->bv_cache[i].page = &mnand_bird_view_pages[i][0];
    ASSERT (FS_OKAY_FOR_DM (pbv->bv_cache[i].page));
  }

  /* On a fresh-start, we have to set all the Bird-View sectors to 0xFF so
   * that the bird-view pages will indicate that no Ptbl pages have been
   * written yet. Note that 0xFF will conveniently match to the enum
   * FS_MNAND_PTBL_PAGE_STATUS_NOT_WRITTEN. */
  if (fresh_start)
  {
    fs_mnand_region_fill (dev, super->data->bv_rptbl.start,
                          super->data->bv_fptbl.end, FS_FOP_BIRDVIEW);
  }
}

/** Search in the cache for the given bird-view page and table. */
static int16
fs_mnand_bird_view_search_page (fs_mnand_bird_view_t pbv, fs_sector_id page,
                                enum fs_journal_key_type which_tbl)
{
  int16 i;

  for (i=0; i<FS_MNAND_MAX_BIRD_VIEW_PAGES; ++i)
  {
    if ( (pbv->bv_cache[i].page_id == page) &&
         (pbv->bv_cache[i].which_tbl == which_tbl) )
    {
      return i;
    }
  }

  return -1;
}

/** Get the best slot in the BirdView cache to add a new page. */
static int16
fs_mnand_bird_view_get_best_free_slot (fs_mnand_bird_view_t pbv)
{
  int16 i, stop, use = -1;

  /* Oldest pages are from the right of the the recent_idx onwards. */
  stop = pbv->recent_idx;
  FS_MNAND_BIRD_VIEW_Q_INCR (pbv->recent_idx);

  /* Starting from the right of the recent_idx, lets go through the cache
   * array and find an unused slot. */
  i=pbv->recent_idx;
  while (i != stop)
  {
    if (pbv->bv_cache[i].page_id == FS_INVALID_SECTOR_ID)
      return i; /* We found an empty spot, we can use this. */

    if (use == -1)
      use = i; /* Oldest page. */

    FS_MNAND_BIRD_VIEW_Q_INCR (i);
  }

  if (use == -1)
    FS_ERR_FATAL ("No free slot in bird view...",0,0,0);

  /* Now use should have the oldest non-dirty page. */
  return use;
}

/** For a given Ptbl page, compute which bird-view sector[offset] will hold
 * the status bits. */
static void
fs_mnand_bird_view_get_page_and_offset (fs_sector_id ptbl_page,
                                        uint32 *bv_sect,
                                        int *byte_offset,
                                        int *bit_posn)
{
  /* Compute which ptbl sector */
  *bv_sect = ptbl_page / FS_MNAND_BIRD_VIEW_ENTRIES_PER_SECT;

  /* Compute offset value */
  *bit_posn = ptbl_page & (FS_MNAND_BIRD_VIEW_ENTRIES_PER_SECT -1);
  *bit_posn *= FS_MNAND_BIRD_VIEW_ENTRIES_SIZE_IN_BITS;

  /* Compute which byte in the sector will have this ptbl page. */
  *byte_offset = *bit_posn / 8;
  ASSERT (*byte_offset < FS_MNAND_SECTOR_SIZE);

  /* Compute which 2 bits will have this ptbl page status. */
  *bit_posn -= (*byte_offset * 8);
  ASSERT (*bit_posn <= (8-FS_MNAND_BIRD_VIEW_ENTRIES_SIZE_IN_BITS));
}


/** Convert the given logical bird-view sector to an physical sector based on
 * either Reverse/Forward page table. */
static fs_sector_id
fs_mnand_bird_view_get_phy_sect (fs_mnand_bird_view_t pbv, fs_page_id bv_page,
                                 enum fs_journal_key_type which_tbl)
{
  fs_sector_id bv_sect;

  ASSERT (bv_page != FS_INVALID_SECTOR_ID);

  if (which_tbl == FS_MNAND_JOURNAL_KEY_TYPE_RTBL)
  {
    bv_sect = pbv->super->data->bv_rptbl.start;
  }
  else if (which_tbl == FS_MNAND_JOURNAL_KEY_TYPE_FTBL)
  {
    bv_sect = pbv->super->data->bv_fptbl.start;
  }
  else
  {
    FS_ERR_FATAL ("error in which_tbl %d",which_tbl,0,0);
    return FS_INVALID_SECTOR_ID; /* to keep lint happy. */
  }

  bv_sect += bv_page;
  return bv_sect;
}

/** Read a BirdView sector at a given logical BirdView sector. */
static void
fs_mnand_bird_view_read_page (fs_mnand_bird_view_t pbv, fs_sector_id bv_page,
                             enum fs_journal_key_type which_tbl,
                             uint8 *buff)
{
  fs_sector_id phy_sect;
  phy_sect = fs_mnand_bird_view_get_phy_sect (pbv, bv_page, which_tbl);
  fs_device_read_sectors (pbv->dev, phy_sect, buff, 1, FS_FOP_BIRDVIEW);
}

/** Write a BirdView sector at a given logical BirdView sector. */
static void
fs_mnand_bird_view_write_page (fs_mnand_bird_view_t pbv, int16 idx)
{
  fs_sector_id phy_sect;

  phy_sect = fs_mnand_bird_view_get_phy_sect (pbv, pbv->bv_cache[idx].page_id,
                pbv->bv_cache[idx].which_tbl);
  fs_device_reliable_write_sectors (pbv->dev, phy_sect,
                                  pbv->bv_cache[idx].page, 1, FS_FOP_BIRDVIEW);
}

/** Adds a BirdView page in the cache on the best available slot. */
static int16
fs_mnand_bird_view_add_page (fs_mnand_bird_view_t pbv, fs_sector_id bv_page,
                             enum fs_journal_key_type which_tbl)
{
  int16 idx;

  idx = fs_mnand_bird_view_get_best_free_slot (pbv);

  /* Add the new page at this slot. */
  pbv->bv_cache[idx].page_id = bv_page;
  pbv->bv_cache[idx].which_tbl = which_tbl;
  fs_mnand_bird_view_read_page (pbv, bv_page, which_tbl,
                                pbv->bv_cache[idx].page);
  return idx;
}

int
fs_mnand_bird_view_is_valid_page (fs_mnand_bird_view_t pbv,
                                  fs_page_id ptbl_pg,
                                  enum fs_journal_key_type which_tbl)
{
  int is_valid_page = 1;
  fs_sector_id bv_sect;
  int byte_offset, bit_posn;
  uint8 status;
  enum fs_mnand_ptbl_page_status page_status;
  int16 idx;

  ASSERT (ptbl_pg != FS_INVALID_PAGE_ID);

  fs_mnand_bird_view_get_page_and_offset (ptbl_pg, &bv_sect, &byte_offset,
                                          &bit_posn);

  idx = fs_mnand_bird_view_search_page (pbv, bv_sect, which_tbl);
  if (idx == -1)
  {
    /* page not found in cache, so add it. */
    ++fs_flash_bird_view_miss_counter;
    idx = fs_mnand_bird_view_add_page (pbv, bv_sect, which_tbl);
  }
  else
  {
    /* good, we found the page in the cache. */
    ++fs_flash_bird_view_hit_counter;
  }

  ASSERT (idx != -1);
  status = pbv->bv_cache[idx].page[byte_offset];
  status = FS_MNAND_BIRD_VIEW_GET_STATUS_BITS (status, bit_posn);

  page_status = (enum fs_mnand_ptbl_page_status) status;
  ASSERT ( (page_status == FS_MNAND_PTBL_PAGE_STATUS_WRITTEN) ||
           (page_status == FS_MNAND_PTBL_PAGE_STATUS_NOT_WRITTEN) );

  if (page_status == FS_MNAND_PTBL_PAGE_STATUS_NOT_WRITTEN)
    is_valid_page = 0;

  return is_valid_page;
}

void
fs_mnand_bird_view_update (fs_mnand_bird_view_t pbv, fs_page_id ptbl_pg,
                           enum fs_journal_key_type which_tbl)
{
  fs_sector_id bv_sect;
  int byte_offset, bit_posn;
  uint8 status, val;
  enum fs_mnand_ptbl_page_status new_status;
  int16 idx;

  ASSERT (ptbl_pg != FS_INVALID_PAGE_ID);

  fs_mnand_bird_view_get_page_and_offset (ptbl_pg, &bv_sect, &byte_offset,
                                          &bit_posn);

  /* Check if we have this page in our cache. */
  idx = fs_mnand_bird_view_search_page (pbv, bv_sect, which_tbl);
  if (idx == -1)
  {
    /* page not found in cache, so add it. */
    ++fs_flash_bird_view_miss_counter;
    idx = fs_mnand_bird_view_add_page (pbv, bv_sect, which_tbl);
  }

  status = pbv->bv_cache[idx].page[byte_offset];
  status = FS_MNAND_BIRD_VIEW_GET_STATUS_BITS (status, bit_posn);
  if (status == FS_MNAND_PTBL_PAGE_STATUS_WRITTEN)
  {
    /* This page is already marked as Written, so nothing further to do. */
    return;
  }

  new_status = FS_MNAND_PTBL_PAGE_STATUS_WRITTEN;
  status = pbv->bv_cache[idx].page[byte_offset];
  val = FS_MNAND_BIRD_VIEW_SET_STATUS_BITS (status, bit_posn, new_status);
  pbv->bv_cache[idx].page[byte_offset] = val;

  fs_mnand_bird_view_write_page (pbv, idx);
 }


#endif /* FEATURE_EFS_EFS2_ON_MNAND */
