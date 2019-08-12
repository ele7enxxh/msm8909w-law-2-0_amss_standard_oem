/***********************************************************************
 * fs_rmts_config.h
 *
 * Remote Storage Configuration file.
 * Copyright (C) 2009-2013 QUALCOMM Technologies, Inc.
 *
 * This file will configure the Remote Storage page manager.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_config.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-07-22   rp    Add new EFS API to store data in SuperBlock.
2012-11-09   nr    Update rmts to use new api's.
2012-09-20   dks   Update rmts layout to support write-cache.
2012-05-31   rp    Optimize EFS untar timings.
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2012-04-09   dks   Support storing/reading cookies to/from emmc partitions.
2011-10-13   rp    Added support to log EFS-Sync timings.
2011-10-10   dks   Remove Hotplug dependency from rmts files.
2011-10-04   dks   Make EFS VU independent of Hotplug and Hfat VUs.
2011-06-08   wek   Use every page size in off target testing.
2010-11-16   wek   Remove dependency with page size macro.
2010-11-15   rp    Replace Boot cookies with EFS cookies.
2010-08-20   vr    Added logic to load efs partitions in EFS for rmts targets.
2010-02-24   rp    Added hotplug_open_device_by_partition_type API.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_RMTS_CONFIG_H__
#define __FS_RMTS_CONFIG_H__


#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

/*-----------------------------------------------------------------------------
  Configure the default page size.
  The default value is 512 bytes. The reasons are: 1. this is a well tested
  value in the file system 2. It will better divide the storage space, e.g.
  Storing a 200 byte file we will waste 312 bytes, as opposed to 1948 bytes
  if page size size is 2k.
-----------------------------------------------------------------------------*/
#ifndef FS_RMTS_PAGE_SIZE_IN_BYTES
  #define FS_RMTS_PAGE_SIZE_IN_BYTES         (512)
#endif


/*-----------------------------------------------------------------------------
  Configure the sector size.
-----------------------------------------------------------------------------*/
#ifdef FEATURE_EFS_RMTS_SECTOR_SIZE_IN_BYTES
  #define FS_RMTS_SECTOR_SIZE        (FEATURE_EFS_RMTS_SECTOR_SIZE_IN_BYTES)
#else
  #define FS_RMTS_SECTOR_SIZE        (512)
#endif


/*-----------------------------------------------------------------------------
  Verify the EFS partition size.
-----------------------------------------------------------------------------*/
#if !defined(SCL_MODEM_EFS_RAM_SIZE)
  #error "SCL_MODEM_EFS_RAM_SIZE is not defined"
#endif

/*-----------------------------------------------------------------------------
  RMTS client id string to be used to get the the RMTS client handle.
-----------------------------------------------------------------------------*/
#define FS_RMTS_CLIENT_ID_STRING "EFS"

/*-----------------------------------------------------------------------------
  Configure how many recent past EFS-sync timings to log.
-----------------------------------------------------------------------------*/
#ifndef FS_RMTS_SYNC_TIME_QUEUE_SIZE
  #define FS_RMTS_SYNC_TIME_QUEUE_SIZE    4
#endif

/*-----------------------------------------------------------------------------
  Configure how many recent past rmts device errors to log.
-----------------------------------------------------------------------------*/
#ifndef FS_RMTS_DEVICE_ERROR_QUEUE_SIZE
  #define FS_RMTS_DEVICE_ERROR_QUEUE_SIZE    4
#endif

/*-----------------------------------------------------------------------------
  Configure the EFS untar malloc size, By default 63KB which is also the max
  we can hash using the current security apis.
  The security-api's take uint16 bytes as argument so the max we could hash
  is 2^16 = 64KB. Since we are chaining, we will use the last 1KB for the
  chaining hash.
-----------------------------------------------------------------------------*/
#ifdef FS_RMTS_EFS_UNTAR_MALLOC_SIZE_IN_BYTES
  #if (FS_RMTS_EFS_UNTAR_MALLOC_SIZE_IN_BYTES > (63 * 1024))
    #error "FS_RMTS_EFS_UNTAR_MALLOC_SIZE_IN_BYTES exceeds 63KB"
  #endif

  #define FS_RMTS_UNTAR_MALLOC_SIZE   (FS_RMTS_EFS_UNTAR_MALLOC_SIZE_IN_BYTES)
#else
  #define FS_RMTS_UNTAR_MALLOC_SIZE   (63 * 1024)
#endif

#if (FS_RMTS_UNTAR_MALLOC_SIZE % FS_RMTS_SECTOR_SIZE) != 0
  #error "FS_RMTS_UNTAR_MALLOC_SIZE is not multiple of sector size"
#endif


/*-----------------------------------------------------------------------------
  Security APIs are used to generate authentication hash for different regions
  of the Remote Storage partition. The size recommended for it was 64 and it
  should not be changed.
-----------------------------------------------------------------------------*/
#define FS_RMTS_META_DATA_SIZE      64


/*-----------------------------------------------------------------------------
  The minimum FS ram size in bytes that will trigger using a write cache by
  default to avoid encrypting and writing large size data to remote processor.
  If RAM size is greater than this value, write-cache is enabled unless
  explicitly disabled by defining EFS_RMTS_DISABLE_WRITE_CACHE.
-----------------------------------------------------------------------------*/
#define FS_RMTS_MIN_RAM_SIZE_BYTES_TO_ENABLE_CACHE (1 * 1024 * 1024)

/*-----------------------------------------------------------------------------
  By default the write cache is enabled if FS RAM size is greater than
  FS_RMTS_MIN_RAM_SIZE_BYTES_TO_ENABLE_CACHE. It needs to be exclusively turned
  off if required by defining EFS_DISABLE_WRITE_CACHE.

  It can also be enabled for smaller FS RAM sizes by explicitly defining
  EFS_RMTS_WRITE_CACHE_SIZE_BYTES to required size in which case a cache of
  that size will be used. When the cache size is defined from builds file, the
  flag to disable write cache cannot be defined.

  The configuration will not take effect if build is loaded on a phone with
  existing image. For configuration to take effect, image has to be erased.
-----------------------------------------------------------------------------*/
#ifdef EFS_RMTS_WRITE_CACHE_SIZE_BYTES
  #ifdef EFS_RMTS_DISABLE_WRITE_CACHE
    #error "Cannot override Write Cache size and disable it at the same time"
  #endif
#endif
/*-----------------------------------------------------------------------------
  The default write cache size is 32KB and can be overridden by defining
  EFS_RMTS_WRITE_CACHE_SIZE_BYTES to the cache size required. The configuration
  will not take effect if build is loaded on a phone with existing image in
  which case the Image configuration is used. For configuration to take effect,
  image has to be erased, there by losing data.
-----------------------------------------------------------------------------*/
#define FS_RMTS_PM_WRITE_CACHE_SIZE_BYTES (32 * 1024)

/*-----------------------------------------------------------------------------
  Amount of space in Bytes reserved in the SuperBlock for OEMs to use.
-----------------------------------------------------------------------------*/
#define FS_RMTS_SUPERBLOCK_OEM_DATA_SIZE_IN_BYTES     32


#else /* (!FEATURE_EFS_EFS2_ON_RMTS) */

  /* Keep lint happy. */
  extern int  __dont_complain_about_empty_file;

#endif /* FEATURE_EFS_EFS2_ON_RMTS */


#endif /* not __FS_RMTS_CONFIG_H__ */
