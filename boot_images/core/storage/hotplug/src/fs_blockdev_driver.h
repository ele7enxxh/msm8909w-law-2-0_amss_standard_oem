/***********************************************************************
 * fs_blockdev_driver.h
 *
 * Block device abstraction layer.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * This file abstracts all block devices using a common interface. If
 * a new block device is to be added a driver must be written to implement
 * these common functions. The driver should then be added to the array
 * blockdev_driver_list.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_driver.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_DRIVER_H__
#define __FS_BLOCKDEV_DRIVER_H__

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_blockdev.h"
#include "fs_blockdev_i.h"

/* This contains driver specific information and functions */
typedef struct
{
  const char            *name;
  block_device_type     type;
  uint16                version;
  void                  (*init)(struct block_device **, uint16 *dev_id);
  void                  (*cleanup)(void);
} blockdev_driver;


extern blockdev_driver *blockdev_driver_list[];
uint32 blockdev_driver_get_total_devices (void);


#endif /* FEATURE_FS_HOTPLUG */

#endif /* not __FS_BLOCKDEV_DRIVER_H__ */
