/***********************************************************************
 * fs_blockdev_sd_driver.c
 *
 * Blockdev driver for the SD device.
 * Copyright (C) 2009-2012 Qualcomm Technologies, Inc.
 *
 * Blockdev driver for the SD device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_sd_driver.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-11-09   nr    Add dummy slot for rmts on simulator.
2011-04-21   wek   Use the partition type to open the mnand hotplug device.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_SD

#include "fs_blockdev_sd_driver.h"
#include "assert.h"

/* Bump this for any code changes in the file */
#define SD_DRIVER_VERSION            0x0001


/* Driver functions */
static void blockdev_sd_driver_init (struct block_device **, uint16 *);
static void blockdev_sd_driver_cleanup (void);


/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_sd_driver =
{
  "BDEV_SD_DRIVER",               /* char *name */
  BLOCKDEV_TYPE_MMC,              /* block_device_type */
  SD_DRIVER_VERSION,              /* uint16 version */
  blockdev_sd_driver_init,     /* init fn */
  blockdev_sd_driver_cleanup   /* cleanup fn */
};


/* List of sd block devices to manage */
static struct block_device sdcc_devices[] =
{
  {"/hdev/sdc1", NULL, NULL, BLOCKDEV_TYPE_MMC, 0, 0xFF, NULL, CANARY},
  {"/hdev/sdc2", NULL, NULL, BLOCKDEV_TYPE_MMC, 0, 0xFF, NULL, CANARY},
  {"/hdev/sdc3", NULL, NULL, BLOCKDEV_TYPE_MMC, 0, 0xFF, NULL, CANARY},
  {"/hdev/sdc4", NULL, NULL, BLOCKDEV_TYPE_MMC, 0, 0xFF, NULL, CANARY}
  /* Phony mnand device for off target testing. */
#ifdef FS_UNIT_TEST
 ,{"/hdev/mnand", NULL, NULL, BLOCKDEV_TYPE_MMC, 0, 0xFF, NULL, CANARY}
 ,{"/hdev/rmts",  NULL, NULL, BLOCKDEV_TYPE_MMC, 0, 0xFF, NULL, CANARY}
#endif /* FS_UNIT_TEST */
};

#define SDCC_NUM_DEVICES   (sizeof (sdcc_devices) / sizeof (sdcc_devices[0]))


/*--------------------
 * SD Driver functions
 *--------------------*/

/* This should populate the blockdev linked list */
static void
blockdev_sd_driver_init (struct block_device **head, uint16 *dev_id)
{
  int i;
  blockdev_ops *sdcc_ops;

  blockdev_sd_device_data_init ();
  sdcc_ops = blockdev_sd_get_ops ();
  ASSERT (sdcc_ops != NULL);

  /* Link the list of devices together */
  for (i=(SDCC_NUM_DEVICES-1); i >= 0; i--)
  {
    struct block_device *bdev = &sdcc_devices[i];

    bdev->device_id = (*dev_id)++;
    bdev->ops = sdcc_ops;
    bdev->device_data = blockdev_sd_get_device_data (i);

    /* Add device to beginning of the linked list */
    bdev->next = *head;
    *head = bdev;
  }
}



static void
blockdev_sd_driver_cleanup (void)
{
  /* Do nothing as of now */
}


#endif /* #ifdef BLOCKDEV_SD */

