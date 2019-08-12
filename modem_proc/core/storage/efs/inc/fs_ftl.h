#ifndef __FTL_H__
#define __FTL_H__
/******************************************************************************
 * fs_ftl.h
 *
 * This file declares the interface for the NAND Flash Translation Layer to
 * present the NAND flash as logical sectors.
 *
 * Copyright (C) 2006-2008, 2010, 2012 QUALCOMM Technologies, Inc.
 * All Rights Reserved
 *
 *****************************************************************************/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/inc/fs_ftl.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2012-03-20   wek     Cleanup customer header file inclusion.
2010-12-22   wek     Remove FTL from memory allocation from the heap.
2010-10-08   rp      Move the FTL stub functions to the .c file
2010-10-06   rp      Initialize the FTL Sync timer before using it.
2010-07-30   wek     Dynamically allocate all buffers dependent on page size.
2010-04-28   vr      Relocate fs_task from EFS1 to EFS2
2010-04-12   wek     Added 8K page support.
2010-03-15   vr      Fix Misaligned FTL Buffer passed to Flash.
2008-11-03   rp      Introduce device abstraction.
2008-04-28   np      Added support for multiple FTL partitions.
2008-04-04   rp      Expose FTL_PARTITION_NAME.
2008-03-06   yg      Removed flash driver header file.
2007-07-25   umr     Added support for 4k Page size.
2006-09-01   yg      Fixed compilation issue for non FTL enabled builds.
2006-08-28   yg      Changed function name for 2K page dev & cache support.
2006-05-28   sh      Cosmetic changes
2006-05-21   yg      Cleanup
2006-04-10   yg      Initial file for FTL layer
=============================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_FTL

#include "comdef.h"

/* Set these values to what the maximum size the software has to support */
#define MAX_BLOCK_COUNT          4096
#define MAX_DATA_PAGE_SIZE       8192
#define SPARE_AREA_SIZE            16

/* Return codes for FTL module */
#define FTL_SUCCESS                     0
#define FTL_FAILURE                   (-1)

/* FTL partition handle definition.*/
typedef void* ftl_raw_device_t;

/* Define an invalid TL partition handle. */
#define FTL_NO_DEVICE          ((ftl_raw_device_t)NULL)

/* This structure defines the interface that need to be used while flushing
 * cached pages, where the pages might not be contiguous, so FTL can really
 * look through the list to check if any page is available in the cache.
 * The list should be sorted by sector_num since a moving pointer will be
 * used for checking, so no real search is performed.
 */
typedef struct
{
  uint32    sector_num;
  byte*     data_ptr;
}
cached_buffer_list_type;

/******************************************************************************
FUNCTION      ftl_raw_open

DESCRIPTION   Initialize the FTL layer

PARAMETERS
              driveno  : ftl drive number. Drive number 0 usaually notes ftl
              partition path "0:FTL".

RETURN VALUE
     Valid partition handle : Success
     NULL : Failure
******************************************************************************/
ftl_raw_device_t
ftl_raw_open (int16 driveno);

/**********************************************************************
FUNCTION      ftl_raw_close

DESCRIPTION   Clear all information related a specific partition.

PARAMETERS
              dev : FTL partition handle.
RETURN VALUE
******************************************************************************/
void
ftl_raw_close (ftl_raw_device_t dev);

/******************************************************************************
FUNCTION      ftl_raw_read

DESCRIPTION   Read specified number of sectors from mentioned sector onwards.

PARAMETERS
          dev       : FTL partition device handle.
          sector    : Starting Sector number to read from (page number in flash
                        terminology).
          buffer    : buffer where read data has to be placed.
          count     : Number of sectors to read starting from mentioned sector.

RETURN VALUE
              0 : Success
     Error code : Failure
******************************************************************************/
int ftl_raw_read (ftl_raw_device_t dev, uint32 sector, byte* buffer,
                  uint16 count);

/******************************************************************************
FUNCTION      ftl_raw_write

DESCRIPTION   Write multiple logical sectors starting from the specified one.

PARAMETERS
          dev       : FTL partition device handle.
          sector    : Starting Sector number to write to (page number in flash
                        terminology).
          buffer    : buffer where new data to be written is passed.
          count     : Number of sectors to wrote starting from mentioned sector

RETURN VALUE  Success / Failure
******************************************************************************/
int ftl_raw_write (ftl_raw_device_t dev, uint32 sector,
                   cached_buffer_list_type* buffer, uint16 count);

/******************************************************************************
FUNCTION      ftl_get_raw_dev_params

DESCRIPTION   Get physical flash device parameters.

RETURN VALUE  0
******************************************************************************/
int ftl_get_raw_dev_params (ftl_raw_device_t dev, uint32 * pblock_count,
                            uint32 * ppg_per_block, uint16 * pg_size,
                            uint32 *psector_cnt);

/******************************************************************************
FUNCTION      fs_ftl_init

DESCRIPTION   Initialize FTL buffers

RETURN VALUE
******************************************************************************/
void fs_ftl_init (void);


/******************************************************************************
FUNCTION      fs_ftl_sync_timer_init

DESCRIPTION   Initialize and start the FTL Sync timer.

RETURN VALUE
******************************************************************************/
void fs_ftl_sync_timer_init (void);


/******************************************************************************
FUNCTION      fs_ftl_start_sync_timer

DESCRIPTION   Start the timer which begins ftl sync

RETURN VALUE
******************************************************************************/
void fs_ftl_start_sync_timer (void);

/******************************************************************************
FUNCTION      fs_ftl_sync_timer_signal_received

DESCRIPTION   Handle the signal from the ftl sync timer

RETURN VALUE
******************************************************************************/
void fs_ftl_sync_timer_signal_received (void);

#else /* FEATURE_EFS_FTL */

void fs_ftl_init (void);
void fs_ftl_sync_timer_signal_received (void);

#endif /* FEATURE_EFS_FTL */

#endif /* __FTL_H__ */


