/***********************************************************************
 * fs_blockdev_sd_cprm_driver.c
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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_sd_cprm_driver.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_SD_CPRM

#include "fs_blockdev_sd_cprm_driver.h"
#include "assert.h"

/* Bump this version for any software changes */
#define SD_CPRM_DRIVER_VERSION          0x0001

/* Function prototypes */
static void blockdev_sd_cprm_init (struct block_device **, uint16 *);
static void blockdev_sd_cprm_cleanup (void);




/* List of sd block devices to manage */
static struct block_device devices[] =
{
  /* Protected area of SD card 0, using MKB 0 (See CPRM_DEVNO above) */
  {"/hdev/sdc1_p", NULL, NULL, BLOCKDEV_TYPE_SD_CPRM, 0, 0xFF, NULL, CANARY},
};


/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_sd_cprm_driver =
{
  "BDEV_SD_CPRM_DRIVER",  /* char *name */
  BLOCKDEV_TYPE_MMC,      /* block_device type */
  SD_CPRM_DRIVER_VERSION, /* uint16 version */
  blockdev_sd_cprm_init,           /* init fn */
  blockdev_sd_cprm_cleanup,        /* cleanup fn */
};


/*-------------------------
 * SD CPRM Driver functions
 *-------------------------*/

/* This should populate the blockdev linked list */
static void
blockdev_sd_cprm_init (struct block_device **head, uint16 *dev_id)
{
  int i;
  blockdev_ops *sd_cprm_ops;

  blockdev_sd_cprm_device_data_init ();
  sd_cprm_ops = blockdev_sd_cprm_get_ops ();
  ASSERT (sd_cprm_ops != NULL);

  /* Link the list of devices together */
  for (i = (sizeof (devices) / sizeof (devices[0])) - 1; i >= 0; i--)
  {
    struct block_device *bdev = &devices[i];

    bdev->device_id = (*dev_id)++;
    bdev->ops = sd_cprm_ops;
    bdev->device_data = blockdev_sd_cprm_get_device_data (i);

    /* Add device to beginning of the linked list */
    bdev->next = *head;
    *head = bdev;
  }
}

static void
blockdev_sd_cprm_cleanup (void)
{
  /* Do nothing as of now */
}

#endif /* BLOCKDEV_SD_CPRM */
