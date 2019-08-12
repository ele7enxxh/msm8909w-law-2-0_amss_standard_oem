/***********************************************************************
 * fs_blockdev_usb.c
 *
 * Blockdev USB device driver.
 * Copyright (C) 2009-2010,2012,2014 QUALCOMM Technologies, Inc.
 *
 * Contains implementation for the blockdev_ops functions specific to
 * the USB driver.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_usb.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2012-03-08   wek   Rename macros and remove header files to compile for boot.
2010-10-06   rp    Fix bug in get_device_info().
2010-09-30   wek   Remove device iovec interface.
2010-08-26   rp    Implement get_device_info().
2010-06-16   rp    Featurized Hotplug USB code.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create
===========================================================================*/


#include "fs_hotplug_config_i.h"

/* Hotplug supports USB Host Mass Storage.. ie, when the MSM controls
 * an external USB device like a thumbdrive, hard drive, or card
 * reader.  */
#ifdef FEATURE_FS_HOTPLUG_USB

#include "fs_blockdev_usb_driver.h"
#include "fs_blockdev_i.h"
#include "fs_err.h"

#include "usbhost_api.h"

/* Unique ID that USBHOST uses to recognize clients (like Hotplug) */
usbhost_client_id_type client_id = USBHOST_INVALID_CLIENT_ID;

/* Device IO functions */
static int blockdev_usb_read (struct block_device*, void*, uint32,
                              unsigned char*, uint16);
static int blockdev_usb_write (struct block_device*, void*, uint32,
                               unsigned char*, uint16);
static int blockdev_usb_get_size (struct block_device*, void*, uint32 *blocks,
                                  uint16 *bytes_per_block);
static int blockdev_usb_is_present (struct block_device*, void*);

/* Stubs */
static int blockdev_usb_success_stub (struct block_device*, void*);
static int blockdev_usb_open_stub (struct block_device*, void**, uint32);
static int blockdev_usb_reliable_write_fail (struct block_device*, void*,
                                             uint32, uint32*, uint32);
static int blockdev_usb_erase_fail (struct block_device*, void*,uint32,uint16);
static int blockdev_usb_get_device_info (struct block_device*, void*,
                                         struct block_device_info*);
static int blockdev_usb_device_idle_fail (struct block_device*, void*);
static uint32 blockdev_usb_get_entire_device_stub (struct block_device*,void*);
static int blockdev_usb_is_embedded_device (struct block_device*, void*);
static int blockdev_usb_set_sps_end_point (struct block_device*, void*, void*);
static int blockdev_usb_get_sps_end_point (struct block_device*, void*, void*);
static int blockdev_usb_set_power_on_write_protection (struct block_device *,
                                                      void*, uint32, uint32);

/* USB operations */
static blockdev_ops usb_ops =
{
  blockdev_usb_open_stub,            /* open */
  blockdev_usb_success_stub,         /* close */
  blockdev_usb_read,                 /* read */
  blockdev_usb_write,                /* write */
  blockdev_usb_write,                /* user write */
  blockdev_usb_reliable_write_fail,  /* reliable write */
  blockdev_usb_erase_fail,           /* erase */
  blockdev_usb_success_stub,         /* reset */
  blockdev_usb_success_stub,         /* device format */
  blockdev_usb_is_present,           /* device present */
  blockdev_usb_get_size,             /* device size */
  blockdev_usb_get_device_info,      /* get_device_info */
  blockdev_usb_get_entire_device_stub, /* get_entire_device */
  blockdev_usb_device_idle_fail,      /* set_device_idle */
  blockdev_usb_is_embedded_device,    /* is_embedded_device */
  blockdev_usb_set_sps_end_point,
  blockdev_usb_get_sps_end_point,
  blockdev_usb_set_power_on_write_protection
};

/* Initialize all usb specific data */
static usb_device_data usb_dev_data[] =
{
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
};



void
blockdev_usb_device_data_init (void)
{
  uint32 total_devices, i;

  total_devices = sizeof (usb_dev_data) / sizeof (usb_dev_data[0]);
  for (i = 0; i < total_devices; ++i)
  {
    usb_dev_data[i].usb_instance = USB_NO_INSTANCE;
    usb_dev_data[i].usb_handle = USB_NO_HANDLE;
    usb_dev_data[i].usb_state = USBSTATUS_UNCONNECTED;
  }
}

blockdev_ops*
blockdev_usb_get_ops (void)
{
  return &usb_ops;
}

void*
blockdev_usb_get_device_data (uint32 idx)
{
  uint32 total_devices;

  total_devices = sizeof (usb_dev_data) / sizeof (usb_dev_data[0]);
  if (idx >= total_devices)
    FS_ERR_FATAL ("mismatch in device data...", 0, 0, 0);

  return &usb_dev_data[idx];
}


/*---------------------
 * USB Device Operations
 *---------------------*/

static int
blockdev_usb_read (struct block_device *bdev, void *handle, uint32 lba,
                   unsigned char *buf, uint16 n_to_read)
{
  usbhost_IoctlTransfer ms_data;
  usb_device_data *dev = (usb_device_data *)bdev->device_data;

  (void) handle;

  ms_data.numberBlocks = n_to_read;
  ms_data.startBlock = lba;
  ms_data.buffer = buf;
  ms_data.length = n_to_read * 512;

  if ((dev->usb_handle != USB_NO_HANDLE)
      && (USBHOST_SUCCESS == usbhost_ioctl (client_id, dev->usb_handle,
                                            USBHOST_IOCTL_STOR_BLOCK_READ,
                                            (void *) &ms_data)))
    return 0;                   /* Good! */
  else
    return -1;                  /* Read failed */
}

static int
blockdev_usb_write (struct block_device *bdev, void *handle, uint32 lba,
                    unsigned char *buf, uint16 n_to_write)
{
  usbhost_IoctlTransfer ms_data;
  usb_device_data *dev = (usb_device_data *)bdev->device_data;

  (void) handle;

  ms_data.numberBlocks = n_to_write;
  ms_data.startBlock = lba;
  ms_data.buffer = buf;
  ms_data.length = n_to_write * 512;

  if ((dev->usb_handle != USB_NO_HANDLE)
      && (USBHOST_SUCCESS == usbhost_ioctl (client_id, dev->usb_handle,
                                            USBHOST_IOCTL_STOR_BLOCK_WRITE,
                                            (void *) &ms_data)))
    return 0;                   /* Good! */
  else
    return -1;                  /* Write failed */
}

static int
blockdev_usb_get_size (struct block_device *bdev, void *handle, uint32 *blocks,
                       uint16 *bytes_per_block)
{
  uint32 count;
  usbhost_status_type status;
  usb_device_data *dev = (usb_device_data *)bdev->device_data;

  (void) handle;

  if (dev->usb_handle == USB_NO_HANDLE)
    return -1;

  status = usbhost_ioctl (client_id, dev->usb_handle,
                          USBHOST_IOCTL_STOR_BLOCK_SIZE, (void *) &count);
  if (status == USBHOST_ERROR)
    return -1;
  *bytes_per_block = count;

  status = usbhost_ioctl (client_id, dev->usb_handle,
                          USBHOST_IOCTL_STOR_NUMBER_OF_BLOCKS,
                          (void *) &count);
  if (status == USBHOST_ERROR)
    return -1;
  *blocks = count;

  return 0;
}

/* This checks if a particular USB device is ready for use.
 * In the case of card readers, this includes having useable media inserted.
 * (A connected device without media is considered not-present.)
 *
 * NOTE that this must be callable even when the device is mounted and active!
 * This means re-entrancy is required in the USB stack.
 */
static int
blockdev_usb_is_present (struct block_device *bdev, void *handle)
{
  usbhost_status_type status;
  int ready;
  usb_device_data *dev = (usb_device_data *)bdev->device_data;

  (void) handle;

  if (dev->usb_state != USBSTATUS_OPEN)
    return 0;

  /* This will check if the media has changed or become unavailable */
  status = usbhost_ioctl (client_id, dev->usb_handle,
                          USBHOST_IOCTL_STOR_CHECK_MEDIA, NULL);

  ready = 0;                    /* Assume the worst */
  switch (status)
  {
    case USBHOST_SUCCESS:
      ready = 1;                /* Success! */
      break;
    case USBHOST_ERROR:
      break;
    case USBHOST_ERROR_MEDIA_CHANGED:
      break;
    case USBHOST_ERROR_MEDIA_NOT_PRESENT:
      break;
    default:
      break;
  }

  /* Also make sure this is a 512-byte block device, so we don't
   * accidentally talk to a CD-ROM device (like U3 thumbdrives) and
   * crash the USB stack. */
  if (ready)
  {
    uint32 count;
    status = usbhost_ioctl (client_id, dev->usb_handle,
                            USBHOST_IOCTL_STOR_BLOCK_SIZE, (void *) &count);
    if (status != USBHOST_SUCCESS) {
      ready = 0;
    } else if (count != 512) {
      ready = 0;
    }
  }

  return ready;
}

/***************
 *  Noop Stubs
 **************/

static int
blockdev_usb_success_stub (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_usb_open_stub (struct block_device *bdev, void **handle, uint32 data)
{
  uint32 partition = data;

  (void) bdev; (void) handle;

  if (partition != 0)
    return -1;

  *handle = (void *)1;

  return 0;
}

static int
blockdev_usb_reliable_write_fail (struct block_device *bdev, void *handle,
                                  uint32 lba, uint32 *buf, uint32 num_sectors)
{
  (void) bdev; (void) handle; (void) lba; (void) buf; (void) num_sectors;
  return -1;
}
static int
blockdev_usb_erase_fail (struct block_device *bdev, void *handle, uint32 lba,
                         uint16 erase_len)
{
  (void) bdev; (void) handle; (void) lba; (void) erase_len;
  return -1;
}

static int
blockdev_usb_get_device_info (struct block_device *bdev, void *handle,
                              struct block_device_info *dev_info)
{
  int result;
  uint32 blocks;
  uint16 bytes_per_block;

  result = blockdev_usb_get_size (bdev, handle, &blocks, &bytes_per_block);
  if (result == 0)
  {
    memset (dev_info, 0, sizeof (struct block_device_info));
    dev_info->card_size_in_sectors = blocks;
    dev_info->bytes_per_sector = (uint32) bytes_per_block;
  }

  return result;
}

static uint32 blockdev_usb_get_entire_device_stub (struct block_device *bdev,
                                                   void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_usb_device_idle_fail (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return -1;
}

static int
blockdev_usb_is_embedded_device (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return -1;
}

static int
blockdev_usb_set_sps_end_point (struct block_device *bdev, void *handle,
                                void *param)
{
  (void) bdev; (void) handle; (void) param;
  return -1;
}

static int
blockdev_usb_get_sps_end_point (struct block_device *bdev, void *handle,
                                void *param)
{
  (void) bdev; (void) handle; (void) param;
  return -1;
}

static int
blockdev_usb_set_power_on_write_protection (struct block_device *bdev,
                  void *handle, uint32 start_block, uint32 num_blocks)
{
  (void) bdev; (void) handle; (void) start_block; (void) num_blocks;
  return -1;
}

#else /* FEATURE_FS_HOTPLUG_USB */

extern int __dont_complain_about_empty_file;

#endif /* FEATURE_FS_HOTPLUG_USB */
