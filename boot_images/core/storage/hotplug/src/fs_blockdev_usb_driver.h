/***********************************************************************
 * fs_blockdev_usb_driver.h
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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_usb_driver.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-06-16   rp    Featurized Hotplug USB code.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_USB_DRIVER_H__
#define __FS_BLOCKDEV_USB_DRIVER_H__

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_USB

#include "fs_blockdev_i.h"
#include "fs_blockdev_driver.h"
#include "usbhost_api.h"

/* These are the USB Instance and Handle default assignments.  They
   are magic values so that we can spot cases where calls are made with
   an uninitialized handle.   The specific value is unimportant.  */
#define USB_NO_INSTANCE 0xDEAD
#define USB_NO_HANDLE   0xDEAF

typedef enum
{
  USBSTATUS_CONNECTED,          /* New device connected */
  USBSTATUS_OPEN_REQUESTED,     /* We are trying to open it */
  USBSTATUS_OPEN,               /* The open was approved! */
  USBSTATUS_CLOSED,             /* Disconnect notify */
  USBSTATUS_UNCONNECTED         /* Nothing connected */
} blockdev_usbhost_state;

/* USB specific device data */
typedef struct
{
  usbhost_dev_instance_type  usb_instance;
  usbhost_dev_handle_type    usb_handle;
  blockdev_usbhost_state     usb_state;
} usb_device_data;


extern void blockdev_usb_device_data_init (void);
extern blockdev_ops* blockdev_usb_get_ops (void);
extern void* blockdev_usb_get_device_data (uint32 idx);


/* Export handle to usb device driver */
extern blockdev_driver blockdev_usb_driver;

/* Unique ID that USBHOST uses to recognize clients (like Hotplug) */
extern usbhost_client_id_type client_id;

#endif /* FEATURE_FS_HOTPLUG_USB */

#endif /* not __FS_BLOCKDEV_USB_DRIVER_H__ */
