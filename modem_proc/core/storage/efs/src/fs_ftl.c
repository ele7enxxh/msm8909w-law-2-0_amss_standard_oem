/******************************************************************************
 * fs_ftl.c
 *
 * This file implements the NAND Flash Translation Layer to present the NAND
 *   flash as logical sectors.
 *
 * Copyright (C) 2006-2010,2012-2013 QUALCOMM Technologies, Inc.
 * All Rights Reserved
 *
 *****************************************************************************/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_ftl.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2013-10-09   nr      Replace banned memory functions with safer versions.
2012-05-18   wek     Remove jzap header file.
2012-03-20   wek     Cleanup customer header file inclusion.
2012-03-08   wek     Rename macros and remove header files to compile for boot.
2010-12-22   wek     Remove FTL from memory allocation from the heap.
2010-10-30   vr      Remove redundant fs_flash.h.
2010-10-08   rp      Move the FTL stub functions to the .c file
2010-10-06   rp      Initialize the FTL Sync timer before using it.
2010-07-30   wek     Dynamically allocate all buffers dependent on page size.
2010-04-28   vr      Relocate fs_task from EFS1 to EFS2
2010-03-15   vr      Fix Misaligned FTL Buffer passed to Flash.
2010-02-08   rp      CMI compliance, remove #ifdef from API files.
2009-04-01   yg      Featurization cleanup related changes.
2009-01-26   ebb     Moved from flash.h => fs_flash.h
2008-11-12   yg      Remove unused flash header file includes.
2008-11-03   rp      Introduce device abstraction.
2008-05-28   yg      Some code cleanup.
2008-04-28   rp      Fix klocwork warnings.
2008-04-28   np      Added support for multiple ftl partitions.
2008-04-14   rp      Use flash_open() to get the FTL partition.
2008-04-07   yg      Fix klocwork errors.
2008-03-06   yg      Moved flash driver related header file from FTL's headers.
2007-11-16   sh      Removed TIMETEST2 include
2006-10-09   yg      Added device layer and counters support.
2006-10-02   yg      Add Partition support to FTL.
2006-09-20   dlb     Lint fixes.
2006-08-28   yg      Added 2k page device and Cache layer support.
2006-06-12   yg      Lint fixes.
2006-06-07   yg      Removed block_count correction
2006-06-07   yg      Removed MMC stub functions
2006-06-02   yg      Rename var, release func clean, bad power down review fix
2006-06-02   sh      Message on ftl_init()
2006-05-28   sh      Cosmetic changes
2006-05-24   yg      Changed to use unsigned integers.
2006-05-21   yg      Cleanup
2006-04-10   yg      Initial file for FTL layer
=============================================================================*/

#include "comdef.h"
#include "fs_config_i.h"

#ifdef FEATURE_EFS_FTL

#include "assert.h"
#include <string.h>
#include "fs_ftl.h"
#include "fs_ftl_i.h"
#include "crc.h"
#include "fs_err.h"
#include "fs_babylon.h"
#include "assert.h"

#include "fs_device.h"
#include "fs_device_ftl_flash.h"
#include "fs_util.h"


/* Currently used FTL PARTITIONS. */
static int active_ftl_devices = 0;

/* An extra page buffer to avoid doing memset everytime.
 * Common for all the device instances. */
FS_ALIGN byte erased_buffer[MAX_DATA_PAGE_SIZE];

/* All the partition related information is stored below. */
static ftl_data partitions[MAX_FTL_PARTITIONS];

/* FTL page size in bytes */
static uint32 ftl_flash_page_size;

/* Number of pages (logical blocks) in one flash block */
static uint32 ftl_pages_per_block;

/* In case we want to erase the whole flash, just set this true from T32 */
uint32 ftl_fresh_start = 0;

/* This is the number of logical sub pages in a device page
 * for a 2k page device we find 4, 512 byte logical pages.
 */
static uint32 ftl_logical_pages_per_dev_page;

/*
 *  These variables denote what is the mask for observing each entity
 *  given the sector number. Shift is used in the conversions. In case
 *  a device which has odd number which makes these masks not possible
 *  will have to use division method which will be slower than this.
 *
 */
static uint32 logical_page_sector_mask;
static uint32 physical_page_sector_mask;

FS_ALIGN byte dev_page_buffer_aligned [MAX_FTL_PARTITIONS][MAX_DATA_PAGE_SIZE];

/*=============================================================================

          F T L   I m p l e m e n t a t i o n    s t a r t

=============================================================================*/


/******************************************************************************
FUNCTION      get_unique_free_block_search_pos

DESCRIPTION   Use CRC 16 algorithm on the existing map and get a unique
              (pseudo random) location to search for the free block. This
              will make sure we don't start from a known physical block
              always and wearing them out the first.

RETURN VALUE
              Block number to start search for a free block.
******************************************************************************/
static uint32
get_unique_free_block_search_pos (ftl_data *pt)
{
  uint32 pseudo_random_pos, data_bit_len;

  /* Number of bits to compute */
  data_bit_len = ((pt->max_logical_page_cnt /
                   (ftl_logical_pages_per_dev_page *
                    ftl_pages_per_block))
                  * sizeof (*pt->lbn_to_phy_blk_map) * BITS_PER_BYTE);

  /* Correct it, if it overflows to more than CRC routine can handle */
  if (data_bit_len > 0xFFF0)
    data_bit_len = 0xFFF0;

  pseudo_random_pos = crc_16_calc ((byte*)pt->lbn_to_phy_blk_map,
                                   data_bit_len);

  return pseudo_random_pos % pt->ftl_block_count;
}

/******************************************************************************
FUNCTION      erase_block_and_update_status

DESCRIPTION   This function erases the given block.
              Also attempts to mark the block bad on failure to erase.

RETURN VALUE  Flash status
******************************************************************************/
static int
erase_block_and_update_status (ftl_data *pt, uint32 block_num)
{
  int result;

  ASSERT (pt != NULL);
  ASSERT (block_num < MAX_BLOCK_COUNT);
  result = pt->ftl_flash_dev->erase_block (pt->ftl_flash_dev, block_num);
  if (result != FS_DEVICE_OK)
  {
    /* No use in checking error return for this function, we can't do much */
    (void) pt->ftl_flash_dev->mark_block_bad (pt->ftl_flash_dev, block_num);
    pt->phy_blk_status[block_num] = BLK_STATE_BAD;
  }
  else
  {
    pt->phy_blk_status[block_num] = BLK_STATE_FREE;
    pt->free_count++;
  }

  return result;
}

/**********************************************************************
FUNCTION      ftl_raw_close

DESCRIPTION   Clear all information related to a specific partition.

PARAMETERS
              dev : partition device pointer.

RETURN VALUE
*****************************************************************************/
void
ftl_raw_close (ftl_raw_device_t dev)
{
  int i;

  ASSERT (dev != NULL);

  for (i = 0; i < MAX_FTL_PARTITIONS; i++) {
    if (&partitions[i] == (ftl_data*) dev) {
      fs_device_close (partitions[i].ftl_flash_dev);
      partitions[i].ftl_flash_dev = NULL;
      ASSERT (active_ftl_devices > 0);
      active_ftl_devices--;
      return;
    }
  }
  ASSERT (0);
  return;
}

/******************************************************************************
FUNCTION      ftl_raw_open

DESCRIPTION   Initialize the FTL layer

PARAMETER
              driveno  : ftl drive number. Drive number 0 usaually notes ftl
              partition path "0:FTL".

RETURN VALUE
     Valid partition handle : Success
     NULL : Failure
******************************************************************************/
ftl_raw_device_t
ftl_raw_open (int16 driveno)
{
  uint32 corrected_spare, pg_to_read, first_page_written, last_page_written;
  uint32 stored_spare, block_num;
  ftl_data *pt;
  uint32 which_device;

  EFS_BAB_DEBUG_PRINTF ("ftl_init()");

  /* Open the device from its partition name. */
  pt = &partitions[driveno];
  ASSERT (pt != NULL);
  ASSERT (pt->ftl_flash_dev == NULL);

  which_device = (int16) driveno;
  pt->ftl_flash_dev = fs_device_open (FS_DEVICE_TYPE_FTL_NAND, which_device);
  if (pt->ftl_flash_dev == NULL)
    return FTL_NO_DEVICE;

  /* Now initialize this raw_ftl dev */
  pt->max_logical_page_cnt = 0;

  /* Since we have small number of states, compiler should not allocate more
   * than a byte for the enum. If it does we just waste a lot of memory space
   * for no better use, so correct something to instruct compiler to allocate
   * just 1 byte for this enum */
  if (sizeof (pt->phy_blk_status) > MAX_BLOCK_COUNT) {
    FS_ERR_FATAL("Compiler allocated size for enum bigger than a byte",0,0,0);
  }


  /* Get basic flash device layout parameters */
  pt->ftl_block_count = pt->ftl_flash_dev->block_count (pt->ftl_flash_dev);

  if (!active_ftl_devices) {
    ftl_flash_page_size = pt->ftl_flash_dev->page_size (pt->ftl_flash_dev);
    ftl_pages_per_block = pt->ftl_flash_dev->block_size (pt->ftl_flash_dev);

    /* Fill our erased buffer */
    memset (erased_buffer, ERASED_BUFFER_VAL, sizeof (erased_buffer));
  }
  else {
    ASSERT (ftl_flash_page_size ==
                pt->ftl_flash_dev->page_size (pt->ftl_flash_dev));
    ASSERT (ftl_pages_per_block ==
                pt->ftl_flash_dev->block_size (pt->ftl_flash_dev));
  }

  ASSERT (active_ftl_devices < MAX_FTL_PARTITIONS);
  active_ftl_devices++;

  /* The maximum block (sector) count is reduced by our reserved space
     to allow spare pages for bad blocks. */
  pt->max_logical_page_cnt = (pt->ftl_block_count
                         * ftl_pages_per_block
                         * (ftl_flash_page_size / DEFAULT_MS_SECTOR_SIZE)
                         * (100 - PERCENT_BAD_BLOCK_BACKUP) / 100);

  /* Limit the number to something which is rounded off to nearest 256 */
  pt->max_logical_page_cnt &= 0xFFFFFF00;

  ftl_logical_pages_per_dev_page = ftl_flash_page_size/DEFAULT_MS_SECTOR_SIZE;

  physical_page_sector_mask =
        (ftl_pages_per_block * ftl_logical_pages_per_dev_page) - 1;

  logical_page_sector_mask = ftl_logical_pages_per_dev_page - 1;

  if ((physical_page_sector_mask
       & (ftl_pages_per_block * ftl_logical_pages_per_dev_page))
      || (ftl_logical_pages_per_dev_page & logical_page_sector_mask))
  {
    FS_ERR_FATAL("Odd device configuration for this software to handle",0,0,0);
  }

  /* If the device is bigger than we can handle just quit */
  if ((pt->ftl_block_count > MAX_BLOCK_COUNT)
      || (ftl_flash_page_size > MAX_DATA_PAGE_SIZE))
  {
    /*  Here the arrays block map and page buffers are defined to be
     *  the smallest possible units for a given flash size, to save
     *  memory. If support for a bigger flash size is needed, then
     *  these sizes should be increased to accomodate these increased sizes. */
    FS_ERR_FATAL ("Mis configured size constants, Increase the size...",0,0,0);
  }

  /* Check if we have enough Spare data bytes for storing FTL Meta data */
  if (pt->ftl_flash_dev->get_spare_corrected_bytes (pt->ftl_flash_dev) < 2)
  {
    FS_ERR_FATAL("Flash Device doesn't have enough Spare bytes for FTL",0,0,0);
  }

  /* Initialize the block map with invalid values */
  memset (pt->lbn_to_phy_blk_map, INVALID_GEN_PHY_BLK,
          sizeof (pt->lbn_to_phy_blk_map));

  /* A count of free (erased and useable) blocks is maintained
   * for debug purposes. */
  pt->free_count = 0;

  corrected_spare = 0;

  /* Walk through each physical block to assemble our map */
  for (block_num = 0; block_num < pt->ftl_block_count; ++block_num)
  {
    /* If this block is bad, skip it and never touch it again */
    if (pt->ftl_flash_dev->bad_block_check (pt->ftl_flash_dev, block_num)
        != FS_DEVICE_OK)
    {
      pt->phy_blk_status[block_num] = BLK_STATE_BAD; /* Avoid */
      continue;
    }

    /* If this was set TRUE for testing, erase each good block of the flash */
    if (ftl_fresh_start) {
      erase_block_and_update_status (pt, block_num);
      continue;
    }

    /* ------------------------------------------------------------
     * Check the first and last page of this block to see if it has
     * been written.
     * ------------------------------------------------------------ */

    /* Check if the first page of this block is written */
    pg_to_read = BLOCK_TO_PAGE_NUM (block_num);
    first_page_written =
        pt->ftl_flash_dev->is_erased (pt->ftl_flash_dev, pg_to_read) == FALSE;

    /* Now read the last page of this block without ECC */
    pg_to_read += (ftl_pages_per_block - 1);

    last_page_written =
       (pt->ftl_flash_dev->read_spare_udata (pt->ftl_flash_dev, pg_to_read,
                                  &corrected_spare, 2) == FS_DEVICE_OK);

    /* If nothing is written in first and last page, then this block is free */
    if (!first_page_written && !last_page_written)
    {
      pt->phy_blk_status[block_num] = BLK_STATE_FREE;
      pt->free_count++;
      continue;
    }

    /* If any one of the first or last page is not written, or ECC decode
     * fails, then erase the block and continue, since power might have
     * failed either while writing or during erase */
    if ( (first_page_written && !last_page_written)   ||
         (!first_page_written && last_page_written)   ||
         ((corrected_spare & LBN_MASK) >= pt->ftl_block_count))
    {
      erase_block_and_update_status (pt, block_num);
      continue;
    }

    /* ------------------------------------------------------------
     * This block has been fully written with valid data.  Find out if
     * it's the newest version of this logical block and update our
     * maps accordingly.
     * ------------------------------------------------------------ */

    /* Check if this LBN has been seen in any other block */
    stored_spare = pt->lbn_to_phy_blk_map[corrected_spare & LBN_MASK];

    if (stored_spare == 0xFFFF) {
      /* Nothing is assigned, so just map this LBN to this physical block */
      pt->lbn_to_phy_blk_map[corrected_spare & LBN_MASK] =
        block_num | (corrected_spare & GENERATION_MASK);
      pt->phy_blk_status[block_num] = BLK_STATE_IN_USE;
    }
    else
    {
      /* We found duplicate blocks claiming to be the same LBN.
       * Decide which is newest and discard the old copy (erase it). */

      /* Combine the two generations into a four-bit value for testing: */
      #define GENERATION_PAIR(a, b) (((a) << 2) | b)

      switch (GENERATION_PAIR (GET_GENERATION (stored_spare),
                               GET_GENERATION (corrected_spare)))
      {
        /* Left (previously-seen) generation is newer: */
        case GENERATION_PAIR(1, 0):
        case GENERATION_PAIR(2, 1):
        case GENERATION_PAIR(3, 2):
        case GENERATION_PAIR(0, 3):
          /* Erase and forget the current block, which is outdated */
          erase_block_and_update_status (pt, block_num);
          break;

          /* Right (current block) generation is newer: */
        case GENERATION_PAIR(0, 1):
        case GENERATION_PAIR(1, 2):
        case GENERATION_PAIR(2, 3):
        case GENERATION_PAIR(3, 0):
          /* Erase the previously-seen block that is outdated */
          erase_block_and_update_status (pt, stored_spare & LBN_MASK);

          /* Note this one as the current block */
          pt->lbn_to_phy_blk_map[corrected_spare & LBN_MASK] =
            block_num | (corrected_spare & GENERATION_MASK);
          break;

          /* Ambiguous cases: identical generations: */
        case GENERATION_PAIR(0, 0):
        case GENERATION_PAIR(1, 1):
        case GENERATION_PAIR(2, 2):
        case GENERATION_PAIR(3, 3):
          /* Ambiguous cases: too far (2) apart: */
        case GENERATION_PAIR(0, 2):
        case GENERATION_PAIR(1, 3):
        case GENERATION_PAIR(2, 0):
        case GENERATION_PAIR(3, 1):
        default:
          /* The two generation values are ambiguous, and it is not
           * possible to determine which one is newer. This should
           * never happen.  For now we can ERR_FATAL, but if it ever
           * actually happens, we should probably just delete an
           * arbitrary block (or even both blocks) to recover. */
          FS_ERR_FATAL ("Generation fields are corrupted...", 0, 0, 0);
      }
    }
  }

  /* Compute the location where we will begin looking for a free
   * block.  While this doesn't have to be random, it should be
   * well-distributed so that we don't unevenly wear flash by favoring
   * some physical blocks over others.  Once we pick a starting block,
   * we will rotate through the blocks in a simple linear walk.  */
  pt->free_blk_pos = get_unique_free_block_search_pos (pt);

  return (ftl_raw_device_t)pt;
}

/******************************************************************************
FUNCTION      ftl_raw_read

DESCRIPTION   Read specified number of sectors from mentioned sector onwards.

    Now with 2k page support we have too many variables in the picture.

    dev          : partition handle
    Logical page : 512 byte page (or simply sector)
    Logical unit : 2k page, its the actual page where logical page is present.
                   for 512 page device Logical page == Logical unit.

RETURN VALUE
              0 : Success
     Error code : Failure
******************************************************************************/
int
ftl_raw_read (ftl_raw_device_t dev, uint32 sector, byte* buffer, uint16 count)
{
  uint32 phy_blk, phy_src_page, device_page, last_page_to_read;
  ftl_data *pt = (ftl_data *)dev;
  byte* page_buff_ptr;

  ASSERT (pt != NULL);
  page_buff_ptr = erased_buffer;
  ASSERT (FS_OKAY_FOR_DM (page_buff_ptr));

  if (sector >= pt->max_logical_page_cnt) {
    return FTL_FAILURE;         /* Out of range */
  }

  ftl_device_read_counter (sector, count, FTL_OP_RAW);

  last_page_to_read = sector + count;

  device_page = INVALID_PAGE;

  /* Loop for as many pages we need to read */
  for ( ; sector < last_page_to_read; ++sector)
  {
    if (device_page != LOGICAL_TO_DEVICE_PAGE (sector))
    {
      device_page = LOGICAL_TO_DEVICE_PAGE (sector);

      phy_blk = pt->lbn_to_phy_blk_map[BLOCK_NUM_OF_PAGE (device_page)];

      /* Check if we have the block within range if valid */
      if ( (phy_blk != INVALID_GEN_PHY_BLK) &&
           ((phy_blk & LBN_MASK) >= MAX_BLOCK_COUNT) )
      {
        FS_ERR_FATAL ("phy_blk[%d] & LBN_MASK) < MAX_BLOCK_COUNT",phy_blk,0,0);
      }

      phy_src_page = (BLOCK_TO_PAGE_NUM (phy_blk & LBN_MASK)
                      + PAGE_OFFSET_IN_BLOCK (device_page));

      page_buff_ptr = pt->dev_page_buffer;

      if ((phy_blk != INVALID_GEN_PHY_BLK)
          && (pt->phy_blk_status[phy_blk & LBN_MASK] == BLK_STATE_BAD))
      {
        /* This should never happen, a bad block is assigned to
         * a usable sector */
        FS_ERR_FATAL ("Bad block is assigned to a useable block: %X %d",
                   phy_blk, pt->free_count, 0);
      }

      if ((phy_blk == INVALID_GEN_PHY_BLK)
          || (pt->phy_blk_status[phy_blk & LBN_MASK] == BLK_STATE_FREE)
          || (pt->ftl_flash_dev->read_page (pt->ftl_flash_dev, phy_src_page,
                                 page_buff_ptr)
              != FS_DEVICE_OK))
      {
        page_buff_ptr = erased_buffer;
      }

      page_buff_ptr += (DEFAULT_MS_SECTOR_SIZE * LOGICAL_PAGE_OFFSET (sector));

    }

    /*   This double memory copy can be avoided later by adding a bit more
     *   complexity
     */
    fs_memscpy (buffer, DEFAULT_MS_SECTOR_SIZE, page_buff_ptr,
                DEFAULT_MS_SECTOR_SIZE);

    /* Point to next page */
    buffer += DEFAULT_MS_SECTOR_SIZE;
    page_buff_ptr += DEFAULT_MS_SECTOR_SIZE;
  }

  return FTL_SUCCESS;
}

/******************************************************************************
FUNCTION      get_free_block

DESCRIPTION   Search for a free block in the free map and return block number.
              Mark this block as "IN_USE" and decrement the free_count.
              NOTE: this is physical Block number.

PARAMETERS
              pt   :  partition data.

RETURN VALUE
              0 : Failure
     Free Blk# : Success
******************************************************************************/
uint32
get_free_block (ftl_data *pt)
{
  uint32 block_num;

  /* At the most we have search ftl_block_count times */
  for (block_num = 0; block_num < pt->ftl_block_count; ++block_num)
  {
    if (pt->phy_blk_status[pt->free_blk_pos] == BLK_STATE_FREE)
    {
      pt->phy_blk_status[pt->free_blk_pos] = BLK_STATE_IN_USE;
      --pt->free_count;
      return pt->free_blk_pos;
    }
    pt->free_blk_pos++;
    if (pt->free_blk_pos >= pt->ftl_block_count)
      pt->free_blk_pos = 0;
  }

  return INVALID_GEN_PHY_BLK;
}

/******************************************************************************
FUNCTION      get_spare_data_to_write

DESCRIPTION   Calculate the spare data to write with new generation num.

PARAMETERS
              pt   :  partition data.
              sec  :  sector to write data

RETURN VALUE  Spare data to write into the last page spare area.
******************************************************************************/
uint32
get_spare_data_to_write (ftl_data *pt, uint32 sector)
{
  uint32 spare_val;

  uint32 temp_lbn = BLOCK_NUM_OF_PAGE (sector);

  /* Have new generation ready in the spare data */
  spare_val = GET_GENERATION (pt->lbn_to_phy_blk_map[temp_lbn]);

  /* Increment the Generation value and mask off. After max it wraps around */
  spare_val = ((spare_val + 1) << GENERATION_SHIFT) & GENERATION_MASK;

  /* Get the LBN combined with Generation to write into the spare area */
  spare_val |= temp_lbn;

  return spare_val;
}

/******************************************************************************
FUNCTION      convert_absolute_to_relative_sectors

DESCRIPTION   Convert all absolute sector numbers to relative sector numbers
                within physical block.

RETURN VALUE  Success / Failure
******************************************************************************/

static void
convert_absolute_to_relative_sectors (cached_buffer_list_type* data_buffer_ary)
{
  uint32 conversion_mask;

  conversion_mask = ((ftl_logical_pages_per_dev_page * ftl_pages_per_block)
                      - 1);
  /*   Place only the offset numbers in the location, instead of sector
   *   number. This will help just make one calculation for mapping
   *   and rest of the code needs only the offsets
   */
  while (data_buffer_ary->data_ptr != 0)
  {
    data_buffer_ary->sector_num
        = conversion_mask & data_buffer_ary->sector_num;

    data_buffer_ary++;
  }
}

int
find_and_copy_new_data (uint32 dest_page,
                        cached_buffer_list_type * src_data_buffer,
                        byte * dest_page_buffer)
{
  uint32 logical_page_start, logical_page_end;
  uint32 matched_page_cnt = 0;

  logical_page_start = dest_page * ftl_logical_pages_per_dev_page;

  /*   Remove the block component out of the sector values, since we
   *   don't have block information in the buffers. It has only the
   *   physical page and logical part in the page as sector number.
   */
  logical_page_start = logical_page_start & physical_page_sector_mask;
  logical_page_end = logical_page_start + ftl_logical_pages_per_dev_page;

  while (src_data_buffer)
  {
    if ((src_data_buffer->data_ptr == 0)
        || (src_data_buffer->sector_num == INVALID_PAGE)
        || (src_data_buffer->sector_num >= logical_page_end))
    {
      break;
    }

    if ((src_data_buffer->sector_num >= logical_page_start)
        && (src_data_buffer->sector_num < logical_page_end))
    {
      ++matched_page_cnt;

      if (dest_page_buffer != 0)
      {
        byte* dst_ptr = dest_page_buffer;

        dst_ptr += ((src_data_buffer->sector_num & logical_page_sector_mask)
                    * DEFAULT_MS_SECTOR_SIZE);

        fs_memscpy (dst_ptr, DEFAULT_MS_SECTOR_SIZE, src_data_buffer->data_ptr,
                    DEFAULT_MS_SECTOR_SIZE);
      }

      if (matched_page_cnt == ftl_logical_pages_per_dev_page)
      {
        break;
      }
    }
    src_data_buffer++;
  }

  return matched_page_cnt;
}

int
write_device_page (ftl_data *pt,
                   uint32 dest_page,
                   uint32 old_src_page,
                   cached_buffer_list_type * src_data_buffer,
                   uint32 spare_val)
{

  uint32 matched_page_cnt, read_source_page_needed;
  uint8 * data_ptr;
  uint32 spare_data = spare_val;
  int result = FS_DEVICE_OK;

  data_ptr = pt->dev_page_buffer;
  ASSERT (FS_OKAY_FOR_DM (data_ptr));

  /* Just search how many new pages we got, Don't copy yet. */
  matched_page_cnt = find_and_copy_new_data (dest_page, src_data_buffer, 0);

  read_source_page_needed = 0;

  /*   If we don't have src data available, but this page is either first or
   *   last page then we have to write the page data, so need to read the
   *   old src page.
   */
  if ((matched_page_cnt == 0) &&
      ((PAGE_OFFSET_IN_BLOCK (dest_page) == 0) ||
       (PAGE_OFFSET_IN_BLOCK (dest_page) == (ftl_pages_per_block - 1))))
  {
    read_source_page_needed = 1;
  }
  else if ((matched_page_cnt != 0)
           && (matched_page_cnt != ftl_logical_pages_per_dev_page))
  {
    read_source_page_needed = 1;
  }

  if (read_source_page_needed)
  {
    if (PAGE_IS_NOT_VALID (old_src_page) ||
        (pt->ftl_flash_dev->read_page (pt->ftl_flash_dev,
                                       old_src_page, data_ptr)
         != FS_DEVICE_OK))
    {
      memset (data_ptr, ERASED_BUFFER_VAL, ftl_flash_page_size);
    }
  }

  if (matched_page_cnt)
  {
    /* This will overwrite the buffer with new data if available */
    find_and_copy_new_data (dest_page, src_data_buffer, data_ptr);
  }

  /* Now we have source data ready if we need to write, or copy page */

  if (PAGE_OFFSET_IN_BLOCK (dest_page) == (ftl_pages_per_block - 1))
  {
    result = pt->ftl_flash_dev->write_page_and_udata (pt->ftl_flash_dev,
                                    dest_page, data_ptr, &spare_data, 2);
  }
  else if ((PAGE_OFFSET_IN_BLOCK (dest_page) == 0) || (matched_page_cnt != 0))
  {
    result = pt->ftl_flash_dev->write_page (pt->ftl_flash_dev, dest_page,
                                            data_ptr);
  }
  else if (PAGE_IS_VALID (old_src_page))
  {
    result = pt->ftl_flash_dev->copy_page (pt->ftl_flash_dev, old_src_page,
                                           dest_page);
  }

  return result;

/* DONE */
}

/******************************************************************************
FUNCTION      single_block_write

DESCRIPTION   Attempt to Write one block worth of data.

RETURN VALUE  Success / Failure
******************************************************************************/
static int
single_block_write (ftl_data *pt, uint32 sector,
                    cached_buffer_list_type* data_buffer_ary,
                    uint32 free_blk_to_write)
{
  uint32 old_phy_src_page;
  uint32 dst_page, dst_end_page;
  uint32 old_phy_blk, spare_data;
  uint32 device_page_num;
  int result;

  convert_absolute_to_relative_sectors (data_buffer_ary);

  device_page_num = LOGICAL_TO_DEVICE_PAGE (sector);

  spare_data = get_spare_data_to_write (pt, device_page_num);

  /* Do we have old block assigned to this LBN?  */
  old_phy_blk =
            pt->lbn_to_phy_blk_map[BLOCK_NUM_OF_PAGE (device_page_num)];
  old_phy_blk = old_phy_blk & LBN_MASK;

  /* Have the source page number for old block ready, if one is available */
  if ((old_phy_blk < pt->ftl_block_count) &&              /* Within limit.?? */
      ((old_phy_blk & LBN_MASK) < MAX_BLOCK_COUNT) &&
      (pt->phy_blk_status[old_phy_blk] == BLK_STATE_IN_USE)) /* Available.?? */
    old_phy_src_page = BLOCK_TO_PAGE_NUM (old_phy_blk);
  else
    old_phy_src_page = INVALID_PAGE;

  /* Our range of physical destination pages to loop through.
   * The physical destination's: First page to last page.
   */
  dst_page = BLOCK_TO_PAGE_NUM (free_blk_to_write);
  dst_end_page = LAST_PAGE_OF_BLOCK (free_blk_to_write);

  /*
   *   Write this entire block page by page. Sequence to write the data
   */
  while (dst_page <= dst_end_page)
  {
    /* Check if our current writing page, should write new data that we got */
    result =  write_device_page (pt, dst_page, old_phy_src_page,
                                data_buffer_ary, spare_data);

    if ((result != FS_DEVICE_OK) && (result != FS_DEVICE_CP_READ_FAIL))
      {
        return FTL_FAILURE;
      }

    /* Pick next page */
    dst_page++;
    if (PAGE_IS_VALID (old_phy_src_page))
      ++old_phy_src_page;

  }  /* Handling of one block is done now */

  /* Delete the old block if we had it active */
  if ((old_phy_blk < pt->ftl_block_count) &&
      ((old_phy_blk & LBN_MASK) < MAX_BLOCK_COUNT) &&
      (pt->phy_blk_status[old_phy_blk] == BLK_STATE_IN_USE))
  {
    erase_block_and_update_status (pt, old_phy_blk);
  }

  /* Update our main map, use the spare data which already has the value */
  pt->phy_blk_status[free_blk_to_write] = BLK_STATE_IN_USE;
  ASSERT ((spare_data & LBN_MASK) < MAX_BLOCK_COUNT);
  pt->lbn_to_phy_blk_map[spare_data & LBN_MASK] = free_blk_to_write |
                                              (spare_data & GENERATION_MASK);
  return FTL_SUCCESS;
}

/******************************************************************************
FUNCTION      ftl_raw_write

DESCRIPTION   Write multiple logical sectors starting from the specified one.

RETURN VALUE  Success / Failure
******************************************************************************/
int
ftl_raw_write (ftl_raw_device_t dev, uint32 sector,
               cached_buffer_list_type* buffer, uint16 count)
{
  uint32 free_blk;
  uint32 logical_device_page;
  ftl_data *pt = (ftl_data *) dev;

  ASSERT (pt != NULL);

  if ((sector >= pt->max_logical_page_cnt) ||
      ((sector + count) > pt->max_logical_page_cnt)) {
    return FTL_FAILURE;
  }

  ftl_device_write_counter (sector, count, FTL_OP_RAW);

  /* We don't have a free block available handy now, later its possible */
  free_blk = INVALID_GEN_PHY_BLK;

  while (count > 0)
  {
    /* Get a free block if we don't have one handy already */
    if (free_blk == INVALID_GEN_PHY_BLK)
    {
      free_blk = get_free_block (pt);
      ASSERT (free_blk < MAX_BLOCK_COUNT);
      if (free_blk == INVALID_GEN_PHY_BLK)
      {
        FS_ERR_FATAL ("Error, NO more free blocks available to write",0,0,0);
      }
    }

    /* Write 1 block with this new data in the free block that we got */
    if (single_block_write (pt, sector, buffer, free_blk) == FTL_SUCCESS)
    {
      uint32 pages_written, pg_of_next_blk;

      logical_device_page = LOGICAL_TO_DEVICE_PAGE (sector);

      /* Calculate how many pages of required data we have in this 1 block */
      pg_of_next_blk =
        BLOCK_TO_PAGE_NUM ((BLOCK_NUM_OF_PAGE (logical_device_page) + 1));
      pages_written = ((pg_of_next_blk - logical_device_page)
                        * ftl_logical_pages_per_dev_page);

      pages_written -= LOGICAL_PAGE_OFFSET (sector);

      pages_written = (pages_written > count) ? count : pages_written;

      /* Update our params */
      count -= pages_written;
      sector += pages_written;

      /* Get new block to write */
      free_blk = INVALID_GEN_PHY_BLK;
    } else {
        if (erase_block_and_update_status (pt, free_blk) != FS_DEVICE_OK) {
          /* Retry writing the data by allocating new block */
          free_blk = INVALID_GEN_PHY_BLK;
        }
    } /* Single block write */
  }  /* While */

  return FTL_SUCCESS;
}

/******************************************************************************
FUNCTION      ftl_raw_format

DESCRIPTION   Low level formatting

RETURN VALUE  0
******************************************************************************/
int
ftl_raw_format (ftl_data *pt)
{
  uint32 block_num;

  if ((pt == NULL) || (pt->ftl_flash_dev == NULL))
  {
    return -1;
  }

  for (block_num = 0; block_num < pt->ftl_block_count; ++block_num)
  {
    if (pt->ftl_flash_dev->bad_block_check (pt->ftl_flash_dev, block_num)
        != FS_DEVICE_BAD_BLOCK)
    {
      (void) pt->ftl_flash_dev->erase_block (pt->ftl_flash_dev, block_num);
      pt->phy_blk_status[block_num] = BLK_STATE_FREE;
      pt->free_count++;
    }
    else
    {
      pt->phy_blk_status[block_num] = BLK_STATE_BAD;
    }

  }

  memset (pt->lbn_to_phy_blk_map, INVALID_GEN_PHY_BLK,
          sizeof(pt->lbn_to_phy_blk_map));

  return 0;
}

/******************************************************************************
FUNCTION      ftl_get_raw_dev_params

DESCRIPTION   Get physical flash device parameters.

RETURN VALUE  0
******************************************************************************/
int
ftl_get_raw_dev_params ( ftl_raw_device_t ph, uint32 * pblock_count,
                        uint32 * ppg_per_block,
                        uint16 * pg_size, uint32 *psector_cnt)
{
  ftl_data *pt = (ftl_data *)ph;

  if (active_ftl_devices == 0)
  {
    return FTL_FAILURE;
  }

  if (ppg_per_block)
  {
    *ppg_per_block = ftl_pages_per_block;
  }

  if (pblock_count)
  {
    *pblock_count = pt->ftl_block_count;
  }

  if (pg_size)
  {
    *pg_size = ftl_flash_page_size;
  }

  if (psector_cnt)
  {
    *psector_cnt = pt->max_logical_page_cnt;
  }

  return FTL_SUCCESS;
}

void
fs_ftl_init (void)
{
  int i;
  memset (partitions, 0, sizeof (partitions));
  active_ftl_devices = 0;

  for (i = 0; i < MAX_FTL_PARTITIONS; i++)
  {
    partitions[i].dev_page_buffer = dev_page_buffer_aligned[i];
    ASSERT (FS_OKAY_FOR_DM (partitions[i].dev_page_buffer));
  }

  fs_ftl_sync_timer_init ();
}

#ifdef FS_UNIT_TEST
/******************************************************************************
FUNCTION      print_map_phy_blk_status

DESCRIPTION   Writes the current map and free count to file,
                used for reference in simulator

RETURN VALUE  0
******************************************************************************/
void
print_map_phy_blk_status (ftl_data *pt, FILE * fp)
{
  uint32 i;

  fprintf (fp, "\nOffset  Phy Block  Gen    Free_list\n"
                "------------------------------------\n");

  fprintf (fp, "Free count: %ld\n", pt->free_count);

  for (i = 0; i < pt->ftl_block_count; ++i)
  {
    fprintf (fp, " %4ld :  %4d    %3d      %4d\n", i,
             pt->lbn_to_phy_blk_map[i] & LBN_MASK,
             GET_GENERATION (pt->lbn_to_phy_blk_map[i]),
             pt->phy_blk_status[i]);
  }

}
#endif /* FS_UNIT_TEST */

#else /* FEATURE_EFS_FTL */

/* Stubs for FTL since FEATURE_EFS_FTL is not defined */
void
fs_ftl_init (void)
{
}

void
fs_ftl_sync_timer_signal_received (void)
{
}

#endif /* FEATURE_EFS_FTL */

