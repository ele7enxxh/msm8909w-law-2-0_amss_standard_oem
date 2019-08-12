/***********************************************************************
 * fs_hotplug_usbhost.c
 *
 * Glue logic between hotplug and usbhost drivers.
 *
 * Copyright (C) 2009-2010,2012,2014 QUALCOMM Technologies, Inc.
 *
 * This glue logic contains the USB callbacks and helper functions
 * necessary for USB MS mode. This code requires the BLOCKDEV_USB
 * module be defined.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_usbhost.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-28   vm    Use correct MSG macro to save RAM.
2012-03-08   wek   Rename macros and remove header files to compile for boot.
2010-06-16   rp    Featurized Hotplug USB code.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Migrate file from robusta to peaberry.
2009-08-11   yg    Fixed Possible NULL pointer dereference.
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2009-05-08   ebb   Create

===========================================================================*/

#include "fs_hotplug_config_i.h"


#ifdef FEATURE_FS_HOTPLUG_USB

#include "fs_hotplug_i.h"
#include "fs_osal.h"
#include "fs_err.h"
#include "usbhost_api.h"
#include "fs_blockdev.h"

/* Needed to peek into internal USB state information
 * and reference unique usb client_id */
#include "fs_blockdev_usb_driver.h"

/* Hotplug supports USB Host Mass Storage.. ie, when the USB Host software
 * controls an external USB device like a thumbdrive, hard drive, or card
 * reader. */

/* Returns the first USB block device which has specified 'state' value. */
static struct block_device *
find_by_state (blockdev_usbhost_state state,
               struct block_device *bdev)
{
  while (1){
    /* Look for the next Mass storage device if available */
    bdev = blockdev_find_by_type (BLOCKDEV_TYPE_USB_MS, bdev);
    if (bdev == NULL)
      return NULL;

    /* If we found one of the interested type then, check if the USB Dev
     * is in the state that we are interested in */
    {
      usb_device_data *udev = (usb_device_data *)bdev->device_data;
      if (udev == NULL)
        return NULL;

      if (udev->usb_state == state)
        return bdev;
    }
  }
}


/* Returns the first USB device in the hotplug_device_list which has
 * the specific instance. */
static struct block_device *
find_by_instance (usbhost_dev_instance_type inst,
                  struct block_device *bdev)
{
  while (1){
    /* Look for the next Mass storage device if available */
    bdev = blockdev_find_by_type (BLOCKDEV_TYPE_USB_MS, bdev);
    if (bdev == NULL)
      return NULL;

    /* If the device is found then check if this device is the
     * same instance that we are looking for */
    {
      usb_device_data *udev = (usb_device_data *)bdev->device_data;
      if (udev == NULL)
        return NULL;

      if (udev->usb_instance == inst)
        return bdev;
    }
  }
}


/* This is the callback function hotplug passes to usbhost_register.
 * THIS EXECUTES IN USB CONTEXT!  Don't spend too long here.
 * This can be called multiple times in a row (for multiple devices)
 * When it is called, we are given a valid dev_instance, but not "handle".
 */
void
hotplug_usbhost_callback (usbhost_pdev_type pdev_typ,
                          usbhost_dev_status_type status,
                          usbhost_dev_handle_type dev_instance, void *param)
{
  (void) pdev_typ;
  (void) param;

  if (status == USBHOST_DEV_CONNECTED)
  {
    struct block_device *bdev;

    /*
     * USB has detected the presence of a mass storage device.  Hurah!
     * Try to find the next available structure in the hotplug table
     * (the first USB device that is in UNCONNECTED state).
     */
    FS_MSG_HIGH_0 ("USBHOST_DEV_CONNECTED");
    bdev = find_by_state (USBSTATUS_UNCONNECTED, NULL);

    if (bdev)
    {
      usb_device_data *udev = (usb_device_data *)bdev->device_data;

      if (find_by_instance (dev_instance, NULL) != NULL)
        FS_MSG_HIGH_0 ("Adding a SECOND duplicate instance!"); /* Bad */

      udev->usb_state = USBSTATUS_CONNECTED;
      udev->usb_instance = dev_instance;
      fs_os_set_sigs (&hotplug_tcb, HOTPLUG_USBHOST_OPEN_SIG);
    }
    else
    {
      FS_MSG_ERROR_0 ("HOTPLUG: New device, with no available slot!");
      /* Drop this one */
    }
  }
  else if (status == USBHOST_DEV_DISCONNECTED)
  {
    struct block_device *bdev;
    /*
     * USB has detected a disconnect on a device.
     * Find this device instance, mark it closed, and wake up Hotplug task.
     */
    FS_MSG_HIGH_0 ("USBHOST_DEV_DISCONNECTED");
    bdev = find_by_instance (dev_instance, NULL);

    if (!bdev)
    {
      FS_MSG_HIGH_0 ("Got a DISCONNECT Notify with no matching instance?");
    }
    else
    {
      usb_device_data *udev;
      udev = (usb_device_data *)bdev->device_data;
      udev->usb_state = USBSTATUS_CLOSED;
      fs_os_set_sigs (&hotplug_tcb, HOTPLUG_USBHOST_CLOSE_SIG);
    }
  }
  else
  {
    FS_MSG_ERROR_0 ("HOTPLUG: Unexpected device status from USB Host MS!");
  }
}

/*
 * Register with USB host stack.  Tell it what signal to use for its own
 * semaphore, which will execute in our task context.
 * Our Hotplug task reserves a signal for this.
 * We get our client_id from this call.
 */
void
hotplug_usbhost_register (void)
{
  usbhost_task_table_type usb_sigs;

  usb_sigs.sem_wait_sig = HOTPLUG_USBHOST_SEM_WAIT_SIG;
  usb_sigs.wait_sig = 0;        /* Deprecated */
  usb_sigs.dog_rpt_sig = 0;     /* No need for dog reporting */
  usb_sigs.dog_rpt_fn_ptr = NULL;
  client_id =
    usbhost_register (USBHOST_PDEV_TYPE_MASS_STORAGE, "Hotplug", usb_sigs,
                      hotplug_usbhost_callback, NULL);
}


/*
 * Called from USB's context when a device has been opened (by our request)
 * We had an instance from the connection, and
 * we now receive a valid handle to use in future calls.
 */
static usbhost_open_ret_type
hotplug_usbhost_open_notification (usbhost_dev_instance_type inst,
                                   usbhost_dev_handle_type handle,
                                   usbhost_open_status stat,
                                   char *client_desc, void *param)
{
  usbhost_open_ret_type result;
  (void) client_desc;
  (void) param;

  /* Assume failure.  Request a retry?  There is no "ERROR" code possible. */
  result = USBHOST_OPEN_RET_RESCHEDULE;

  if (stat == USBHOST_OPEN_AVAILABLE)
  {
    /*
     * Find the first entry in our hotplug table that is of this USB
     * instance and associate that dev_instance to this handle.
     */
    struct block_device *bdev;
    FS_MSG_HIGH_0 ("USBHOST_OPEN_AVAILABLE");
    bdev = find_by_instance (inst, NULL);

    if (!bdev)
    {
      FS_MSG_HIGH_0 ("Mysterious USB device opened.  No matching instance");
    }
    else
    {
      usb_device_data *udev = (usb_device_data *)bdev->device_data;

      if (udev->usb_state != USBSTATUS_OPEN_REQUESTED)
      {
        FS_MSG_HIGH_0 ("We did not request open on this instance!");
      }
      else
      {
        udev->usb_handle = handle; /* Save the handle! */
        udev->usb_state = USBSTATUS_OPEN;
        result = USBHOST_OPEN_RET_UNSCHEDULE; /* Accept this connection */
      }
    }
  }
  else if (stat == USBHOST_OPEN_NOT_AVAILABLE)
  {
    /* Our open request was denied.  Request a retry. */
    FS_MSG_HIGH_0 ("USBHOST_OPEN_NOT_AVAILABLE");
  }
  else
  {
    FS_MSG_HIGH_1 ("Got an open_notify with a bizzare status: %d", stat);
  }
  return result;
}

/*
 * Called from USB's context when a device is requested for closure.
 * This means some other task would like to use a device that we have
 * open, and we are being asked to release it.
 *
 * Since Hotplug "owns" any device listed in the table, we ignore
 * this request.  It would indicate a real problem.
 */
static void
hotplug_usbhost_close_notification (usbhost_dev_instance_type handle,
                                    char *client_desc,
                                    usbhost_client_id_type from_client_id,
                                    void *param)
{
  (void) handle;
  (void) client_desc;
  (void) from_client_id;
  (void) param;

  FS_MSG_HIGH_0 ("Got a usbhost_close_notification, and ignored it.");
}

/* This is called by hotplug_task() after a USB device is connected
 * and the callback gives us the signal to open any new devices */
void
hotplug_usbhost_open (void)
{
  int serviced = 0;

  /* Some of the USB devices are available for communication.
     (state = USBSTATUS_CONNECTED)
     We should find ALL such devices and open them. */
  FS_MSG_HIGH_0 ("hotplug_usbhost_open");
  while (1)
  {
    struct block_device *bdev = NULL;

    bdev = find_by_state (USBSTATUS_CONNECTED, bdev);
    if (!bdev)
      break;

    FS_MSG_HIGH_1 ("hotplug_usbhost_open(%s)", bdev->name);

    {
      usb_device_data *udev;
      udev = (usb_device_data *)bdev->device_data;
      /* Ask that the device be opened now. */
      udev->usb_state = USBSTATUS_OPEN_REQUESTED;
      usbhost_req_open (client_id,
                        udev->usb_instance,
                        hotplug_usbhost_open_notification,
                        hotplug_usbhost_close_notification);
      serviced++;
    }
  }

  if (serviced == 0)
    FS_MSG_HIGH_0 ("hotplug_usbhost_open without any CONNECTED devices!");
}

/* This is called by hotplug_task() after a USB device is disconnected.
 * One or more devices is now in USBSTATUS_CLOSED. */
void
hotplug_usbhost_close (void)
{
  struct block_device *bdev = NULL;
  int serviced = 0;

  /* Some of the devices are no longer available */
  while (1)
  {
    usb_device_data *udev;
    bdev = find_by_state (USBSTATUS_CLOSED, bdev);
    if (!bdev)
      break;

    /* This device will be unmounted the next time hotplug task
     * polls and finds it unuseable without a handle. */
    udev = (usb_device_data *)bdev->device_data;
    udev->usb_instance = USB_NO_INSTANCE;
    udev->usb_handle = USB_NO_HANDLE;
    udev->usb_state = USBSTATUS_UNCONNECTED;

    FS_MSG_HIGH_1 ("hotplug_usbhost_close(%s)", bdev->name);
    serviced++;
  }

  if (serviced == 0)
    FS_MSG_HIGH_0 ("hotplug_usbhost_close without any CLOSED devices!");
}

#endif /* FEATURE_FS_HOTPLUG_USB */
