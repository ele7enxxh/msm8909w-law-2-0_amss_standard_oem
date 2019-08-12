/***********************************************************************
 * fs_blockdev_sd_driver.h
 *
 * Blockdev driver for the SD device.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * Blockdev driver for the SD device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_sd_driver.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_SD_DRIVER_H__
#define __FS_BLOCKDEV_SD_DRIVER_H__

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_SD

#include "fs_blockdev_i.h"
#include "fs_blockdev_driver.h"
#include "sdcc_api.h"

/* SD/MMC device specific data */
typedef struct
{
  int16              driveno;
  unsigned char      is_open;
  int                was_present;
  struct sdcc_device *handle;
} sd_device_data;

extern void blockdev_sd_device_data_init (void);
extern blockdev_ops* blockdev_sd_get_ops (void);
extern void* blockdev_sd_get_device_data (uint32 idx);
extern int blockdev_sd_get_driveno (struct block_device *hdev);

/* Handle to SD device driver */
extern blockdev_driver blockdev_sd_driver;

#endif /* FEATURE_FS_HOTPLUG_SD */

#endif /* not __FS_BLOCKDEV_SD_DRIVER_H__ */
