#ifndef __FTL_I_H__
#define __FTL_I_H__
/******************************************************************************
 * fs_ftl_i.h
 *
 * This file declares the internal data used by the FTL layer.
 *
 * Copyright (C) 2006, 2008-2010 QUALCOMM Technologies, Inc.
 * All Rights Reserved
 *
 *****************************************************************************/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_ftl_i.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2010-03-15   vr      Fix Misaligned FTL Buffer passed to Flash.
2009-04-01   yg      Featurization cleanup related changes.
2008-11-03   rp      Introduce device abstraction.
2008-05-27   yg      Moved the erased buffer out of device struct.
2008-04-28   np      Add support for multiple FTL partitions.
2006-09-01   yg      Fixed compilation issue for non FTL enabled builds.
2006-08-28   yg      Added 2k page dev and cache support.
2006-06-05   yg      Changed erased buffer value
2006-05-28   sh      Cosmetic changes
2006-04-25   yg      Code Cleanup
2006-04-10   yg      Initial file for FTL layer
=============================================================================*/

#include "fs_ftl.h"

#ifdef FEATURE_EFS_FTL

#include "fs_device.h"


/******************************************************************************
*
*   Spare Area assignment  (16 bytes per 512 byte page)
*
*   00 00000000 11 1111
*   01 23456789 01 2345
*   EE EEEEEEEE BB UUUU    8 bit 512 byte page device
*   BB EEEEEEEE EE UUUU   16 bit 512 byte page device
*
*   Where:
*      E : Reed-Solomon ECC code bytes (Hardware NAND controller dependent).
*      B : Bad block marker (1 byte for 8 bit, 2 bytes for 16 bit device).
*      U : Unused spare bytes used as meta data for FTL (See below).
*
* -----------------------------------------------------------------------------
*
*    Spare user data assignment (4 bytes)
*
*   33222222    22 221111    11 111100    00000000
*   10987654    32 109876    54 321098    76543210
*   PPPPPPPP    PP EEEEEE    GG LLLLLL    LLLLLLLL
*
*   P : Hamming code parity bits     (10 =>  xx )
*   E : Empty, not used for now.     ( 6 =>  xx )
*   G : Generation bits.             ( 2 =>  4  )
*   L : Logical Block Number bits.   (14 => 16384 )
*
******************************************************************************/
#define LBN_MASK                  0x3FFF
#define GENERATION_MASK           0xC000
#define GENERATION_SHIFT          14
#define ECC_CODE_SHIFT            22
#define GET_GENERATION(spare) (((spare) & GENERATION_MASK) >> GENERATION_SHIFT)


/*   Mass storage devices always need page size to be 512 bytes
 *   2k doesn't work
 */
#define DEFAULT_MS_SECTOR_SIZE      512

/*
 *   To get logical page offset in a device page. Only for 2k page device.
 */
#define LOGICAL_PAGE_OFFSET(x)    ((x) & (ftl_logical_pages_per_dev_page - 1))

/*
 *   To get device page number (still in logical context) from the
 *   logical page number.
 */
#define LOGICAL_TO_DEVICE_PAGE(page) ((page) / ftl_logical_pages_per_dev_page)

#define BLOCK_TO_PAGE_NUM(blk)       ((blk)  * ftl_pages_per_block)
#define BLOCK_NUM_OF_PAGE(page)      ((page) / ftl_pages_per_block)
#define PAGE_OFFSET_IN_BLOCK(page)   ((page) % ftl_pages_per_block)
#define LAST_PAGE_OF_BLOCK(blk)      (BLOCK_TO_PAGE_NUM((blk) + 1) - 1)

/* Since the current design is storing 2 bit generation and 14 bit map together
 * in the map table, and value 0xFFFF is considered as invalid value, we can
 * only have 16383 max blocks supported here without modifications. If support
 * for more blocks than this is needed, it is easy to modify the code to
 * seperate 2 bit Generation number from the LBN in the map. In that case
 * Generation can be stored in another array of byte. So max block count
 * supported cannot exceed 0x3FFF ie 16383 for now.   */

#if MAX_BLOCK_COUNT > 16383
#error FTL is designed to support maximum 16383 blocks, refer to comments
#endif

#define  BLK_STATE_FREE        0
#define  BLK_STATE_IN_USE      1
#define  BLK_STATE_SYS_INFO    2
#define  BLK_STATE_BAD         3

/* Percent number of blocks reserved in case blocks go bad */
#define PERCENT_BAD_BLOCK_BACKUP    4

/* Invalid number in the map table for Generation and physical blk
 * Need to change this accordingly with change in the map table params */
#define INVALID_GEN_PHY_BLK         0xFFFF
#define INVALID_PAGE                0xFFFFFFFF

#define PAGE_IS_VALID(pg)          ((pg) != INVALID_PAGE)
#define PAGE_IS_NOT_VALID(pg)      ((pg) == INVALID_PAGE)

/* Contents of the erased buffer. If this value is 0 then just writing
 * 1 bit before power down will catch the error early instead of late */
#define ERASED_BUFFER_VAL           0x0

/* Source data status, do we need to write this page
 * according to the source data..? */
#define SOURCE_DATA_AVAILABLE           0
#define SOURCE_DATA_NOT_AVAILABLE       0xFFFFFFFF

/* Just to make sure, we don't put just numbers confusing the reader. */
#define BITS_PER_BYTE      8

/* Maximum path length for FTL partition names */
#define MAX_FTL_PATH_LEN 10

/* Each partition need to store the following information. */
typedef struct {

  /* fs_device_t handle. */
  efs_device_t ftl_flash_dev;

  /* Partition name. */
  char ftl_partition_name[MAX_FTL_PATH_LEN];

  /* Buffer to hold the data to write */
  byte *dev_page_buffer;

  /*
   *  This is the map to translate logical block to physical block,
   *  will also have 2 bit generation along with. So its a tradeoff
   *  between code complexity and memory usage to keep Generation
   *  in a seperate array.
   *
   *  Storage format is:
   *  GG PPPPPP PPPPPPPP
   *  G(2)   : 2 bit generation
   *  P(14)  : 14 bit Physical block number
   *  Offset : LBN Logical block number
  */
  uint16 lbn_to_phy_blk_map[MAX_BLOCK_COUNT];

  /*
   *  This is the state of the physical blocks to pick a free block.
   *  At any given point of time this gives the status of the physical
   *  block.
   */
  uint8 phy_blk_status[MAX_BLOCK_COUNT];

  /* Just keep track of how many free blocks are available */
  uint32 free_count;

  /* A running location pointer to pick next free block */
  uint32 free_blk_pos;

  /* This is the maximum number of "logical sectors", which
   * are actually physical flash PAGES for 512-byte flash. */
  uint32 max_logical_page_cnt;

  /* Number of blocks in the FTL area of flash */
  uint32 ftl_block_count;

} ftl_data;
#endif /* FEATURE_EFS_FTL */

#endif /* __FTL_I_H__ */

