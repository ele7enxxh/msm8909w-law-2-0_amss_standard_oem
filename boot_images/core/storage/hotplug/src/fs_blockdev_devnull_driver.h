/***********************************************************************
 * fs_blockdev_devnull_driver.h
 *
 * Blockdev driver to the devnull block device.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * Blockdev driver to the devnull block device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_devnull_driver.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_DEVNULL_DRIVER_H__
#define __FS_BLOCKDEV_DEVNULL_DRIVER_H__

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_blockdev_i.h"
#include "fs_blockdev_driver.h"


extern blockdev_ops* blockdev_devnull_get_ops (void);

/* Handle to devnull device driver */
extern blockdev_driver blockdev_devnull_driver;

#else
extern int __dont_complain_about_empty_file;
#endif /* FEATURE_FS_HOTPLUG */

#endif /* not __FS_BLOCKDEV_DEVNULL_DRIVER_H__ */
