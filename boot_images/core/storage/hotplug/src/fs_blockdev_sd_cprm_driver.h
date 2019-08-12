/***********************************************************************
 * fs_blockdev_sd_cprm_driver.h
 *
 * Blockdev driver for the SD CPRM device.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * Blockdev driver for the SD CPRM device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_sd_cprm_driver.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_SD_CPRM_DRIVER_H__
#define __FS_BLOCKDEV_SD_CPRM_DRIVER_H__


#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_SD_CPRM

#include "fs_blockdev_i.h"
#include "fs_blockdev_driver.h"

extern void blockdev_sd_cprm_device_data_init (void);
extern blockdev_ops* blockdev_sd_cprm_get_ops (void);
extern void* blockdev_sd_cprm_get_device_data (uint32 idx);

/* This is the only external structure we export */
extern blockdev_driver blockdev_sd_cprm_driver;

#else
  extern int __dont_complain_about_empty_file;
#endif /* FEATURE_FS_HOTPLUG_SD_CPRM */

#endif /* not __FS_BLOCKDEV_SD_CPRM_DRIVER_H__ */
