/***********************************************************************
 * fs_blockdev_usb_driver.c
 *
 * Blockdev driver for the USB device.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * Blockdev driver for the USB device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_usb_driver.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-06-16   rp    Featurized Hotplug USB code.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-23   yg    Fix pointer increment issue.
2009-11-16   rp    Create

===========================================================================*/


#include "fs_hotplug_config_i.h"

/* Hotplug supports USB Host Mass Storage.. ie, when the MSM controls
 * an external USB device like a thumbdrive, hard drive, or card
 * reader.  */
#ifdef FEATURE_FS_HOTPLUG_USB

#include "fs_blockdev_usb_driver.h"

/* Bump this for any code changes */
#define USB_DRIVER_VERSION        0x0001

/* Devices this driver manages */
static struct block_device devices[] =
{
  {"/hdev/usb1", NULL, NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, NULL, CANARY},
  {"/hdev/usb2", NULL, NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, NULL, CANARY},
  {"/hdev/usb3", NULL, NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, NULL, CANARY},
  {"/hdev/usb4", NULL, NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, NULL, CANARY},
  {"/hdev/usb5", NULL, NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, NULL, CANARY},
  {"/hdev/usb6", NULL, NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, NULL, CANARY},
  {"/hdev/usb7", NULL, NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, NULL, CANARY},
  {"/hdev/usb8", NULL, NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, NULL, CANARY},
};

#define NUM_DEVICES (sizeof (devices) / sizeof (devices[0]))


static void blockdev_usb_init (struct block_device **head, uint16 *dev_id);
static void blockdev_usb_cleanup (void);


/* Driver object to be exposed to the upper layer */
blockdev_driver blockdev_usb_driver =
{
  "BDEV_USB_DRIVER",    /* char *name */
  BLOCKDEV_TYPE_USB_MS, /* block_device type */
  USB_DRIVER_VERSION,   /* uint16 version */
  blockdev_usb_init,          /* init fn */
  blockdev_usb_cleanup        /* cleanup fn */
};



/*--------------------
 * USB Driver functions
 *--------------------*/

/* This should populate the blockdev linked list */
static void
blockdev_usb_init (struct block_device **head, uint16 *dev_id)
{
  int i;
  blockdev_ops *usb_ops;

  blockdev_usb_device_data_init ();
  usb_ops = blockdev_usb_get_ops ();
  ASSERT (usb_ops != NULL);

  /* Link the list of devices together */
  for (i = NUM_DEVICES - 1; i >= 0; i--)
  {
    struct block_device *bdev = &devices[i];

    bdev->device_id = (*dev_id)++;
    bdev->ops = usb_ops;
    bdev->device_data = blockdev_usb_get_device_data (i);

    /* Add device to beginning of the linked list */
    bdev->next = *head;
    *head = bdev;
  }
}

static void
blockdev_usb_cleanup (void)
{
  /* Do nothing as of now */
}

#endif /* FEATURE_FS_HOTPLUG_USB */
