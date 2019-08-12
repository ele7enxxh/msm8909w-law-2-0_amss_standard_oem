/***********************************************************************
 * fs_blockdev_devnull.c
 *
 * Implement a small no-op device for blockdev
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * It is sometimes useful to have a 'sink' device for benchmarking.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_devnull_driver.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_blockdev_devnull_driver.h"
#include "assert.h"


#define DEVNULL_DRIVER_VERSION        0x0001

/* Only one dev.null device neccesary ;) */
static struct block_device devices[] =
{
  {"/hdev/dev.null", NULL, NULL, BLOCKDEV_TYPE_DEVNULL, 0, 0xFF, NULL, CANARY}
};

#define NUM_DEVICES (sizeof (devices) / sizeof (devices[0]))

static void blockdev_devnull_init (struct block_device **head, uint16 *dev_id);
static void blockdev_devnull_cleanup (void);


/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_devnull_driver =
{
  "BDEV_DEVNULL_DRIVER",     /* char *name */
  BLOCKDEV_TYPE_DEVNULL,     /* block_device_type */
  DEVNULL_DRIVER_VERSION,    /* uint16 version */
  blockdev_devnull_init,              /* init fn */
  blockdev_devnull_cleanup            /* cleanup fn */
};


/* This should populate the linked list pointer passed in*/
static void
blockdev_devnull_init (struct block_device **head, uint16 *dev_id)
{
  int i;
  blockdev_ops *devnull_ops;

  devnull_ops = blockdev_devnull_get_ops ();
  ASSERT (devnull_ops != NULL);

  /* Link the list of devices together */
  for (i = NUM_DEVICES - 1; i >= 0; i--)
  {
    struct block_device *dev = &devices[i];

    dev->device_id = (*dev_id)++;
    dev->ops = devnull_ops;

    /* Add device to beginning of the linked list */
    dev->next = *head;
    *head = dev;
  }
}

static void
blockdev_devnull_cleanup (void)
{
  /* Do nothing as of now */
}



#endif /* FEATURE_FS_HOTPLUG */
