/***********************************************************************
 * fs_blockdev.c
 *
 * Block device abstraction layer.
 * Copyright (C) 2008-2010,2013 Qualcomm Technologies, Inc.
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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_driver.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-24   rp    Add UFS support to Hotplug.
2010-06-16   rp    Featurized Hotplug USB code.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_blockdev_driver.h"
#include "fs_blockdev_sd_driver.h"
#include "fs_blockdev_sd_cprm_driver.h"
#include "fs_blockdev_usb_driver.h"
#include "fs_blockdev_ftl_driver.h"
#include "fs_blockdev_devnull_driver.h"
#include "fs_blockdev_ufs_driver.h"


#define NUM_DRIVERS ( (int )(sizeof (blockdev_driver_list) / \
                      sizeof (blockdev_driver_list[0])) )


/* Driver list -
 * This is a list of all drivers currently supported. Each of these
 * will be called to be initialized at startup. */
blockdev_driver *blockdev_driver_list[] =
{
#ifdef FEATURE_FS_HOTPLUG_SD
  &blockdev_sd_driver,
#endif

#ifdef FEATURE_FS_HOTPLUG_UFS
  &blockdev_ufs_driver,
#endif

#ifdef FEATURE_FS_HOTPLUG_SD_CPRM
  &blockdev_sd_cprm_driver,
#endif

#ifdef FEATURE_FS_HOTPLUG_FTL
  &blockdev_ftl_driver,
#endif

#ifdef FEATURE_FS_HOTPLUG_USB
  &blockdev_usb_driver,
#endif

  &blockdev_devnull_driver,

  NULL
};

uint32 blockdev_driver_get_total_devices (void)
{
  return NUM_DRIVERS;
}


#endif /* FEATURE_FS_HOTPLUG */
