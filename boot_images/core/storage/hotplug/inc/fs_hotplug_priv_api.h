 /***********************************************************************
 * fs_hotplug_priv_api.h
 *
 * Hotplug Private APIs.
 * Copyright (C) 2009-2013 QUALCOMM Technologies, Inc.
 *
 * The hotplug APIs that are private and only intended the EFS to use.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/inc/fs_hotplug_priv_api.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-10   rp    Hotplug to poll only devices of interest.
2012-08-14   wek   Allow calling hotplug_init multiple times and execute once.
2011-10-11   wek   Add hotplug public header to fix compile time warnings.
2011-04-21   wek   Use the partition type to open the mnand hotplug device.
2011-12-01   vr    Restore partition paths to Fix compilation issue.
2010-12-23   vr    Remove hardcoded EFS partition paths.
2010-11-19   rp    Added support to read FSG from modem if possible.
2010-10-25   wek   Fix hotplug_format to format an soft partition.
2010-09-30   wek   Remove device iovec interface.
2010-02-24   rp    Added hotplug_open_device_by_partition_type API.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_HOTPLUG_PRIV_API_H__
#define __FS_HOTPLUG_PRIV_API_H__

#include "comdef.h"
#include "fs_hotplug.h"

/* Scheduled for removal.*/
/* Maximum size of a friendly name in map */
#define HOTPLUG_MAX_FRIENDLY_NAME_LEN                 24
#define HOTPLUG_PARTITION_NAME_BOOT_QCSBL             "/boot/qcsbl"
#define HOTPLUG_PARTITION_NAME_BOOT_OEMSBL            "/boot/oemsbl"
#define HOTPLUG_PARTITION_NAME_BOOT_APPSSBL           "/boot/appsbl"
#define HOTPLUG_PARTITION_NAME_BOOT_APPS              "/boot/apps"
#define HOTPLUG_PARTITION_NAME_BOOT_MODEM             "/boot/modem"
#define HOTPLUG_PARTITION_NAME_BOOT_BOOT_FOTA         "/boot/fota"
#define HOTPLUG_PARTITION_NAME_BOOT_QCSBLHD_CFG_DATA  "/boot/qcsbl_cfg"
#define HOTPLUG_PARTITION_NAME_BOOT_OSBL              "/boot/osbl"
#define HOTPLUG_PARTITION_NAME_ADSP                   "/boot/adsp"

/*-----------------------------------------------------------------------------
  Configure the Remote storage files for off-target simulator build.
  In simulator, we create physical files on disk with these names, so create
  files in local test directory.
-----------------------------------------------------------------------------*/
#ifdef FS_UNIT_TEST
  #define HOTPLUG_PARTITION_NAME_BOOT_BOOT_MODEM_FS1    "./modem_fs1"
  #define HOTPLUG_PARTITION_NAME_BOOT_BOOT_MODEM_FS2    "./modem_fs2"
  #define HOTPLUG_PARTITION_NAME_BOOT_BOOT_MODEM_FSG    "./modem_fsg"
#else
  #define HOTPLUG_PARTITION_NAME_BOOT_BOOT_MODEM_FS1    "/boot/modem_fs1"
  #define HOTPLUG_PARTITION_NAME_BOOT_BOOT_MODEM_FS2    "/boot/modem_fs2"
  #define HOTPLUG_PARTITION_NAME_BOOT_BOOT_MODEM_FSG    "/boot/modem_fsg"
#endif


/*************************************************************************//**
 * Create the Hotplug-task that will do the periodic polling.
 * If the FEATURE_FS_HOTPLUG_DISABLE_TASK feature is enabled, then this
 * function will simulate a cold-plug behavior.
*****************************************************************************/
void hotplug_task_init (void);

/*************************************************************************//**
 * Write reliably the given 'block_cnt' number of sectors starting from 'lba'
 * to the opened device referred by 'hdev' from the buffer starting at 'buf'
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, obtained from the
 *    hotplug_open_device function. This represents the persistent
 *    device handle.
 * @param lba
 *    Start reading from this block
 * @param buf
 *    Buffer that holds the read data
 * @param block_count
 *    Number of blocks to read
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
*****************************************************************************/
int hotplug_dev_reliable_write (struct hotplug_device *hdev, uint32 lba,
                                uint32 *buf, uint16 block_cnt);

/*************************************************************************//**
 * Get what is the start sector of the current hdev on the physical device
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, obtained from the
 *    hotplug_open_device function. This represents the persistent
 *    device handle.
 *
 * @return Start sector.
 *
 ****************************************************************************/
uint32 hotplug_get_physical_start_sector (struct hotplug_device *hdev);


#endif /* !__FS_HOTPLUG_PRIV_API_H__ */
