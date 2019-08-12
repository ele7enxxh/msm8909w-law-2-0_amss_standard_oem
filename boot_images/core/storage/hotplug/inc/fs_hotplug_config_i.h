/***********************************************************************
 * fs_hotplug_config_i.h
 *
 * Hotplug configuration.
 * Copyright (C) 2010-2014 QUALCOMM Technologies, Inc.
 *
 * This file will configure the Hotplug module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/inc/fs_hotplug_config_i.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-01   vm    Add support for dual OS boot.
2013-05-24   rp    Add UFS support to Hotplug.
2012-03-18   wek   Reduce hotplug size for bootloaders.
2012-03-02   wek   Fix configuration define names.
2011-04-29   wek   Move max physical partitions to config file.
2011-03-31   wek   Implement GPT/MBR partition table parser.
2010-09-29   dks   Moved hotplug configurable variables to fs_hotplug_config*.h
2010-03-15   vr    Move Hotplug selection logic to custefs
2010-02-08   rp    Create

===========================================================================*/

#ifndef __FS_HOTPLUG_CONFIG_I_H__
#define __FS_HOTPLUG_CONFIG_I_H__

#include "fs_hotplug.h"
#include "fs_config_i.h"

#ifndef FS_HOTPLUG_NOTIFY_MAX_CLIENTS
  #define FS_HOTPLUG_NOTIFY_MAX_CLIENTS  (10)
#endif

#ifndef FS_HOTPLUG_MAX_ITERATORS
  #define FS_HOTPLUG_MAX_ITERATORS     (5)
#endif

#ifndef FS_HOTPLUG_MAX_HDEVS
  #define FS_HOTPLUG_MAX_HDEVS         (128)
#endif

#ifndef FS_HOTPLUG_MAX_SOFT_PARTITIONS
  #define FS_HOTPLUG_MAX_SOFT_PARTITIONS  (36)
#endif

#ifndef FS_HOTPLUG_MAX_PHY_PARTITIONS
  #define FS_HOTPLUG_MAX_PHY_PARTITIONS   (8)
#endif

#ifndef FS_HOTPLUG_MAX_PARTITIONS
  #define FS_HOTPLUG_MAX_PARTITIONS \
          (FS_HOTPLUG_MAX_SOFT_PARTITIONS+FS_HOTPLUG_MAX_PHY_PARTITIONS+2)
#endif

/*************************************************************************//**
 * This is the starting interval (in minutes) to use for setting the
 * idle device event generation interval. We keep track of the reads/writes
 * on per slot basis and if there is no activity going on that slot for the
 * two consecutive polls then we would generate the event for timeout.
 *
 * The target can set this value in cust, which allows a different
 * default to be used without changing this file.  This default value
 * will be used if none is previously defined.
 *
 * Default value of the timeout event interval has been set as 30 mins.
 *
 *
*****************************************************************************/
#ifndef FS_HOTPLUG_TIMEOUT_EVT_INTERVAL_MINUTES
  #define FS_HOTPLUG_TIMEOUT_EVT_INTERVAL_MINUTES  (30)  /* 30 mins */
#endif


/* What is the fundamental sector size in bytes when parsing partitions.
 * This is used to allocate memory to read and parse sectors. */

#ifndef HOTPLUG_LOGICAL_BLOCK_SIZE_BYTES

  #ifdef FEATURE_FS_HOTPLUG_UFS
    #define HOTPLUG_LOGICAL_BLOCK_SIZE_BYTES  (4096)
  #else
    #define HOTPLUG_LOGICAL_BLOCK_SIZE_BYTES  (512)
  #endif

#endif /* HOTPLUG_LOGICAL_BLOCK_SIZE_BYTES */



#endif /* not __FS_HOTPLUG_CONFIG_I_H__ */
