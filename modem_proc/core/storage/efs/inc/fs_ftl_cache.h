#ifndef __FTL_CACHE_H__
#define __FTL_CACHE_H__
/******************************************************************************
 * fs_ftl_cache.h
 *
 * This file declares externally available interface for the FTL cache layer.
 *
 * Copyright (C) 2006-2010 QUALCOMM Technologies, Inc.  All Rights Reserved
 *
 *****************************************************************************/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/inc/fs_ftl_cache.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2010-11-30   vr      Remove redundant fs_flash.h
2009-01-26   ebb     Moved from flash.h => fs_flash.h
2008-05-27   yg      Changed ftl_cache_init function proto.
2008-04-28   np      Added support for multiple FTL partitions.
2008-03-06   yg      Removed flash driver header file.
2006-10-10   yg      Changed periodic sync function proto.
2006-09-14   sh      Removed unused Benchmark hook.
2006-09-01   yg      Fixed compilation issue for non FTL enabled builds.
2006-08-31   dlb     Fix RCS header.
2006-07-21   yg      Initial file for FTL cache layer
=============================================================================*/

/*
 *    NOTE:
 *
 *    SECTOR : Is a 512 byte page unit.
 *    Page   : Is the smallest writable unit of the flash device (can be 2k).
 *
 */

#include "fs_config_i.h"

#ifdef FEATURE_EFS_FTL

#include "comdef.h"

/* A cached device handle type is defined to hide the details of cached
 * partition device internal details.
 */
typedef void* ftl_cached_device_t;

#define FTL_CACHED_NO_DEVICE  ((ftl_cached_device_t)NULL)

/******************************************************************************
FUNCTION      ftl_cache_init

DESCRIPTION   Initialize and open the FTL Cache layer for the drive number.

PARAMETERS

RETURN VALUE
******************************************************************************/
int
ftl_cache_init (int16 driveno);

/******************************************************************************
FUNCTION      ftl_cache_open

DESCRIPTION   Open an FTL Cache layer device.

PARAMETERS
              driveno  : ftl drive number. Drive number 0 usaually notes ftl
              partition path "0:FTL".

RETURN VALUE  device handle.
******************************************************************************/
ftl_cached_device_t
ftl_cache_open (int16 driveno);

/******************************************************************************
FUNCTION      ftl_cache_read

DESCRIPTION   Read specified number of sectors from mentioned sector onwards.
              Get the contents from either cache or directly from flash
              depending on the availability.

PARAMETERS
          dev       : The FTL cache device partition handle.
          sector    : Starting Sector number to read from.
          buffer    : buffer where read data has to be placed.
          count     : Number of sectors to read starting from mentioned sector.

RETURN VALUE
              0 : Success
     Error code : Failure
******************************************************************************/
int ftl_cache_read (ftl_cached_device_t dev, uint32 sector, byte* buffer,
                    uint16 count);

/******************************************************************************
FUNCTION      ftl_cache_write

DESCRIPTION   Cached Write multiple logical sectors starting from
              the specified one.

PARAMETERS
          pt     : The FTL cache device partition handle.
          sector  : Starting Sector number to write to.
          buffer  : buffer where new data to be written is available.
          count   : Number of sectors to write starting from mentioned sector.

RETURN VALUE  Success / Failure
******************************************************************************/
int ftl_cache_write (ftl_cached_device_t dev, uint32 sector, byte* buffer,
                     uint16 count);

/******************************************************************************
FUNCTION      ftl_cache_periodic_sync

DESCRIPTION   Do periodic sync. This call will sync one oldest cached block
              and return if there are more blocks to sync.

PARAMETERS
              None
RETURN VALUE
              0 : If no more blocks are cached.
              1 : If there are more blocks in cache need to be flushed.
******************************************************************************/
int ftl_cache_periodic_sync (void);

/******************************************************************************
FUNCTION      ftl_cache_force_sync_all

DESCRIPTION
            Sync all the cache contents to flash. This would be done before
            power down.

PARAMETERS
           NONE

RETURN VALUE
          0 : Success
******************************************************************************/
int ftl_cache_force_sync_all (void);

/******************************************************************************
FUNCTION      ftl_cache_get_size

DESCRIPTION   Gets the geometry of the FTL device.

PARAMETERS
          dev        : The FTL cache device partition handle.
          pblk_cnt  : Pointer to location to store the maximum available number
                        of Sectors in this device (# of pages in flash term).
          ppg_size  : Pointer to location to store the page size.

RETURN VALUE  0
******************************************************************************/
int ftl_cache_get_size (ftl_cached_device_t dev, uint32* pblk_cnt,
                        uint16* ppg_size);

/**********************************************************************
FUNCTION      fs_ftl_cache_close

DESCRIPTION   Clear all information related a specific partition.

PARAMETERS
              dev : The FTL cache device partition handle.
RETURN VALUE
******************************************************************************/
void
fs_ftl_cache_close (ftl_cached_device_t dev);

#endif /* FEATURE_EFS_FTL */

#endif /* __FTL_CACHE_H__ */

