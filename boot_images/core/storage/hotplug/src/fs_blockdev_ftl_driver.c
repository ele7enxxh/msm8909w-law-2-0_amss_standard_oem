/***********************************************************************
 * fs_blockdev_ftl_driver.c
 *
 * Blockdev driver to the FTL block device.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * Blockdev driver to the FTL block device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_ftl_driver.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/
#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_FTL

#include "fs_blockdev_ftl_driver.h"
#include "assert.h"

#define FTL_DRIVER_VERSION         0x0001


/* Only one dev.null device neccesary ;) */
static struct block_device devices[] =
{
  {"/hdev/ftl1", NULL, NULL, BLOCKDEV_TYPE_FTL, 0, 0xFF, NULL, CANARY},
  {"/hdev/ftl2", NULL, NULL, BLOCKDEV_TYPE_FTL, 0, 0xFF, NULL, CANARY}
};

#define NUM_DEVICES (sizeof (devices) / sizeof (devices[0]))



/* This should populate the linked list pointer passed in*/
static void
ftl_init (struct block_device **head, uint16 *dev_id)
{
  int i;
  blockdev_ops *ftl_ops;

  blockdev_blockdev_ftl_device_data_init ();
  ftl_ops = blockdev_blockdev_ftl_get_ops ();
  ASSERT (ftl_ops != NULL);

  /* Link the list of devices together */
  for (i = NUM_DEVICES - 1; i >= 0 ; i--)
  {
    struct block_device *bdev = &devices[i];

    bdev->device_id = (*dev_id)++;
    bdev->ops = ftl_ops;
    bdev->device_data = blockdev_blockdev_ftl_get_device_data (i);

    /* Add device to beginning of the linked list */
    bdev->next = *head;
    *head = bdev;
  }
}

static void
blockdev_ftl_cleanup (void)
{
  /* Do nothing as of now */
}


/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_ftl_driver =
{
  "BDEV_FTL_DRIVER",  /* char *name */
  BLOCKDEV_TYPE_FTL,  /* block_device_type */
  FTL_DRIVER_VERSION, /* uint16 version */
  ftl_init,           /* init fn */
  blockdev_ftl_cleanup      /* cleanup fn */
};

#endif /* BLOCKDEV_FTL */
