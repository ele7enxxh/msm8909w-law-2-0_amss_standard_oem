/***********************************************************************
 * fs_blockdev_ufs_driver.c
 *
 * Blockdev driver for the UFS device.
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 * Blockdev driver for the UFS device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_ufs_driver.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-24   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_UFS

#include "fs_blockdev_ufs_driver.h"
#include "assert.h"

/* Bump this for any code changes in the file */
#define UFS_DRIVER_VERSION            0x0001


/* Driver functions */
static void blockdev_ufs_driver_init (struct block_device **, uint16 *);
static void blockdev_ufs_driver_cleanup (void);


/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_ufs_driver =
{
  "BDEV_UFS_DRIVER",            /* char *name */
  BLOCKDEV_TYPE_MMC,            /* block_device_type */
  UFS_DRIVER_VERSION,           /* uint16 version */
  blockdev_ufs_driver_init,     /* init fn */
  blockdev_ufs_driver_cleanup   /* cleanup fn */
};


/* List of ufs block devices to manage */
static struct block_device ufs_devices[] =
{
  {"/hdev/ufs1", NULL, NULL, BLOCKDEV_TYPE_UFS, 0, 0xFF, NULL, CANARY},
#ifdef FS_UNIT_TEST
  {"/hdev/ufs2", NULL, NULL, BLOCKDEV_TYPE_UFS, 0, 0xFF, NULL, CANARY}
#endif
};

#define UFS_NUM_DEVICES   (sizeof (ufs_devices) / sizeof (ufs_devices[0]))


/*--------------------
 * UFS Driver functions
 *--------------------*/

/* This should populate the blockdev linked list */
static void
blockdev_ufs_driver_init (struct block_device **head, uint16 *dev_id)
{
  int i;
  blockdev_ops *ufs_ops;

  blockdev_ufs_device_data_init ();
  ufs_ops = blockdev_ufs_get_ops ();
  ASSERT (ufs_ops != NULL);

  /* Link the list of devices together */
  for (i=(UFS_NUM_DEVICES-1); i >= 0; i--)
  {
    struct block_device *bdev = &ufs_devices[i];

    bdev->device_id = (*dev_id)++;
    bdev->ops = ufs_ops;
    bdev->device_data = blockdev_ufs_get_device_data (i);

    /* Add device to beginning of the linked list */
    bdev->next = *head;
    *head = bdev;
  }
}



static void
blockdev_ufs_driver_cleanup (void)
{
  /* Do nothing as of now */
}

#endif /* #ifdef BLOCKDEV_UFS */

