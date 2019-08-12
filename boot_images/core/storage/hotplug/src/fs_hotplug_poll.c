/***********************************************************************
 * fs_hotplug_poll.c
 *
 * Hotplug Polling module.
 * Copyright (C) 2009-2011,2013,2014 QUALCOMM Technologies, Inc.
 *
 * Poll all hotplug devices once every at every configured time-interval, and
 * discover devices when the media is inserted, and monitor inserted media.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_poll.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2013-11-20   rp    Detect embedded MMC-devices without opening them.
2013-10-10   rp    Hotplug to poll only devices of interest.
2011-09-08   dks   Remove sdcc_api.h inclusion.
2011-04-05   dks   Fix hotplug notify event sequence.
2011-03-15   wek   Get entire device id from bdev instead of hard coding it.
2010-09-29   dks   Moved hotplug configurable variables to fs_hotplug_config*.h
2010-07-14   rp    Remove the assert to check hotplug_poll_interval for 0.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-12-09   yog   Passing correct args to blockdev_close()
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_hotplug_i.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_err.h"
#include "fs_osal.h"
#include "assert.h"


/* Timer to trigger periodic poll events */
static fs_os_timer_type hotplug_poll_timer;

/* The interval time for the periodic hotplug polls */
static fs_os_timer_cnt_type hotplug_poll_interval;

/* Prototypes. */
static void hotplug_poll_do_initialization (struct hotplug_physical_device *);
static void hotplug_poll_do_discovery (struct hotplug_physical_device *,
                        enum hotplug_iter_device_location dev_location);
static void hotplug_poll_do_mounting (struct hotplug_physical_device *);
static void hotplug_poll_do_monitoring (struct hotplug_physical_device *);



void
hotplug_poll_init (void)
{
  /* Create our timer for the poll cycle */
  fs_os_def_timer (&hotplug_poll_timer, &hotplug_tcb, HOTPLUG_POLL_SIG);

  /* Set our default poll interval and clear the timer */
  hotplug_set_polling_interval (FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS);
}

/* Mini-version of the hotplug_poll(), this function will only go till the
 * device discovery state, and after this state will not mount anything with
 * EFS. It is safe to call this function even before we mount the root "/"
 * file-system. */
void
hotplug_poll_before_mounting_root_helper (hotplug_dev_type dev_type,
                     enum hotplug_iter_device_location dev_location)
{
  struct hotplug_physical_device *phy_hdev;
  enum hotplug_physical_dev_state prior_state;
  int i;

  HOTPLUG_LOCK ();

  /* Walk through each device in the physical device list and poll. */
  i = 0;
  while (i < hotplug_total_physical_devices)
  {
    phy_hdev = &hotplug_physical_devices[i];
    prior_state = phy_hdev->dev_state;

    if (dev_type != HOTPLUG_TYPE_ALL)
    {
      if (phy_hdev->dev_type != dev_type)
      {
        ++i;
        continue;
      }
    }

    switch (phy_hdev->dev_state)
    {
      case HPDEV_UNINITIALIZED:
        hotplug_poll_do_initialization (phy_hdev);
        break;

      case HPDEV_UNUSABLE:
        ASSERT (phy_hdev->hdev_list == NULL);
        break;

      case HPDEV_UNDISCOVERED:
        hotplug_poll_do_discovery (phy_hdev, dev_location);
        break;

      case HPDEV_DISCOVERED:
        /* Don't do anything as root-FS is not yet there. */
        break;

      case HPDEV_MOUNTED:
      case HPDEV_UNMOUNTED:
      case HPDEV_LOCKED:
      case HPDEV_FORMATTING:
      default:
        FS_ERR_FATAL ("Invalid state while polling...%d%d%d",0,0,0);
        break;
    }

    /* If this device was unchanged, advance to the next device.
     * Otherwise, this device will be polled again immediately
     * until the state settles */
    if (prior_state == phy_hdev->dev_state)
    {
      ++i;
    }
  }

  HOTPLUG_UNLOCK ();

}

void
hotplug_poll_before_mounting_root (void)
{
  hotplug_poll_before_mounting_root_helper (HOTPLUG_TYPE_ALL,
                                            HOTPLUG_ITER_ALL_DEVICES);
}


/* Poll all the hotplug devices, and take action if there are any changes.*/
void
hotplug_poll (void)
{
  struct hotplug_physical_device *phy_hdev;
  enum hotplug_physical_dev_state prior_state;
  int i;

  HOTPLUG_LOCK ();

  /* Walk through each device in the physical device list and poll. */
  i = 0;
  while (i < hotplug_total_physical_devices)
  {
    phy_hdev = &hotplug_physical_devices[i];
    prior_state = phy_hdev->dev_state;

    switch (phy_hdev->dev_state)
    {
      case HPDEV_UNUSABLE:
        break;

      case HPDEV_UNDISCOVERED:
        hotplug_poll_do_discovery (phy_hdev, HOTPLUG_ITER_ALL_DEVICES);
        break;

      case HPDEV_DISCOVERED:
        hotplug_poll_do_mounting (phy_hdev);
        break;

      case HPDEV_MOUNTED:
      case HPDEV_UNMOUNTED:
        hotplug_poll_do_monitoring (phy_hdev);
        break;

      case HPDEV_FORMATTING:
      case HPDEV_LOCKED:
        break;

      case HPDEV_UNINITIALIZED: /* Not possible... */
      default:
        FS_ERR_FATAL ("Invalid state while polling...",0,0,0);
        break;
    }

    /* If this device was unchanged, advance to the next device.
     * Otherwise, this device will be polled again immediately
     * until the state settles */
    if (prior_state == phy_hdev->dev_state)
    {
      ++i;
    }
  }


  HOTPLUG_UNLOCK ();
}

static void
hotplug_poll_do_initialization (struct hotplug_physical_device *phy_hdev)
{
  ASSERT (phy_hdev->dev_state == HPDEV_UNINITIALIZED);

  if (blockdev_reset (phy_hdev->bdev, phy_hdev->bdev_handle) == 0)
  {
    hotplug_change_physical_dev_state (phy_hdev, HPDEV_UNDISCOVERED);
  }
  else
  {
    hotplug_change_physical_dev_state (phy_hdev, HPDEV_UNUSABLE);
  }
}


static void
hotplug_poll_do_discovery (struct hotplug_physical_device *phy_hdev,
                           enum hotplug_iter_device_location dev_location)
{
  int result, is_embedded_device;
  uint32 entire_device_id;

  hotplug_validate_physical_hdev (phy_hdev);

  ASSERT (phy_hdev->bdev_handle == NULL);
  ASSERT (phy_hdev->parti_list == NULL);

  phy_hdev->hdev_list = NULL;
  phy_hdev->bdev_handle = NULL;

  if (dev_location != HOTPLUG_ITER_ALL_DEVICES)
  {
    /* Check if we can know the type of device without opening it. */
    is_embedded_device = blockdev_is_embedded_device (phy_hdev->bdev,
                                                      phy_hdev->bdev_handle);
    if (is_embedded_device == 1)
    {
      if (dev_location != HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY)
      {
        goto Error;
      }
    }
    else if (is_embedded_device == 0)
    {
      if (dev_location != HOTPLUG_ITER_EXTERNAL_DEVICES_ONLY)
      {
        goto Error;
      }
    }
  }

  entire_device_id = blockdev_get_entire_device_id (phy_hdev->bdev, NULL);
  result = blockdev_open (phy_hdev->bdev, &phy_hdev->bdev_handle,
                          entire_device_id);
  if (result != 0)
    goto Error; /* remain in DEV_UNDISCOVERED state */

  result = blockdev_is_present (phy_hdev->bdev, phy_hdev->bdev_handle);
  if (result != 1)
    goto Error; /* remain in DEV_UNDISCOVERED state */

  if (dev_location != HOTPLUG_ITER_ALL_DEVICES)
  {
    is_embedded_device = blockdev_is_embedded_device (phy_hdev->bdev,
                                                      phy_hdev->bdev_handle);
    if (is_embedded_device == 1)
    {
      if (dev_location != HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY)
      {
        goto Error;
      }
    }
    else if (is_embedded_device == 0)
    {
      if (dev_location != HOTPLUG_ITER_EXTERNAL_DEVICES_ONLY)
      {
        goto Error;
      }
    }
    else
    {
      /* We don't know if this device is Internal/External, So ignore it. */
      goto Error;
    }

  }

  result = hotplug_discover_device (phy_hdev);
  if (result != 0)
    goto Error; /* remain in DEV_UNDISCOVERED state */

  result = hotplug_legacy_hdev_open (phy_hdev);
  if (result != 0)
    goto Error; /* remain in DEV_UNDISCOVERED state */

  hotplug_notify_events (phy_hdev->canonical_name, phy_hdev,
                         HOTPLUG_NOTIFY_EVENT_CARD_INSERTED);

  hotplug_change_physical_dev_state (phy_hdev, HPDEV_DISCOVERED);

  goto Success;

Error:
  if (phy_hdev->bdev_handle != NULL)
  {
    (void )blockdev_close (phy_hdev->bdev, phy_hdev->bdev_handle);
    phy_hdev->bdev_handle = NULL;
    /* remain in DEV_UNDISCOVERED state */
  }

Success:
  return;
}


static void
hotplug_poll_do_mounting (struct hotplug_physical_device *phy_hdev)
{
  hotplug_validate_physical_hdev (phy_hdev);

  ASSERT (phy_hdev->dev_state == HPDEV_DISCOVERED);
  hotplug_change_physical_dev_state (phy_hdev, HPDEV_MOUNTED);
  hotplug_mount_all_partitions (phy_hdev, phy_hdev->canonical_name);
}



static void
hotplug_invalidate_all_hdevs (struct hotplug_physical_device *phy_hdev)
{
  struct hotplug_device *hdev;

  ASSERT (phy_hdev != NULL);

  /* Free up all the hdevs. */
  for ( hdev = phy_hdev->hdev_list;
        hdev != NULL;
        hdev = hdev->next_hdev )
  {
    hdev->is_invalid = 1;
  }
  phy_hdev->hdev_list = NULL;
}

static void
hotplug_poll_do_monitoring (struct hotplug_physical_device *phy_hdev)
{
  int present;
  const char *hdev_name;

  hotplug_validate_physical_hdev (phy_hdev);

  present = blockdev_is_present (phy_hdev->bdev, phy_hdev->bdev_handle);
  if (present == 1)
    return; /* media is still there. */

  /* Okay, media has vanished now. */
  hdev_name = (const char *)phy_hdev->canonical_name;

  hotplug_legacy_hdev_close (phy_hdev);
  hotplug_change_physical_dev_state (phy_hdev, HPDEV_UNMOUNTED);
  hotplug_unmount_all_partitions (phy_hdev, phy_hdev->canonical_name);
  hotplug_invalidate_all_hdevs (phy_hdev);
  hotplug_free_all_partitions (phy_hdev);

  (void) blockdev_close (phy_hdev->bdev, phy_hdev->bdev_handle);
  phy_hdev->bdev_handle = NULL;

  hotplug_change_physical_dev_state (phy_hdev, HPDEV_UNDISCOVERED);

  hotplug_notify_events (hdev_name,phy_hdev,HOTPLUG_NOTIFY_EVENT_CARD_REMOVED);
}


/*
 * Cause the Hotplug poll timer to expire and trigger an immediate
 * poll.
 *
 * Note that this is an internal hotplug function.  External apps can
 * accomplish the same by using hotplug_set_polling_interval().
 */
void
hotplug_force_poll_now (void)
{
 /* If COLDPLUG is requested, then no task will poll and the polling
  * must be done right now, and in the caller's context. */
#if defined FEATURE_FS_HOTPLUG_DISABLE_TASK || defined FS_UNIT_TEST
  hotplug_poll ();
#else
  /* Otherwise, pretend the timer just expired.  Hotplug Task will put
   * the timer back if polling is still enabled */
  fs_os_clr_timer (&hotplug_poll_timer);
  (void) fs_os_set_sigs (&hotplug_tcb, HOTPLUG_POLL_SIG);
#endif
}


static void
hotplug_poll_start_timer (void)
{
  if (hotplug_poll_interval == 0)
    return;

  (void) fs_os_clr_timer (&hotplug_poll_timer);
  (void) fs_os_set_timer (&hotplug_poll_timer, hotplug_poll_interval);
}

static void
hotplug_poll_stop_timer (void)
{
  (void) fs_os_clr_timer (&hotplug_poll_timer);
  (void) fs_os_clr_sigs (&hotplug_tcb, HOTPLUG_POLL_SIG);
}

/* Allow an external task to enable/disable hotplug polling and set
 * the frequency (in milliseconds).  Polling is disabled with a value
 * of zero milliseconds.
 *
 * Returns the former polling interval value in milliseconds.
 */
unsigned int
hotplug_set_polling_interval (unsigned int milliseconds)
{
  unsigned int prev_value = (unsigned int) hotplug_poll_interval;

  if (milliseconds == hotplug_poll_interval)
    return prev_value;

  hotplug_poll_interval = (fs_os_timer_cnt_type )milliseconds;

  if (milliseconds == 0)
  {
    hotplug_poll_stop_timer ();
    return prev_value;
  }

  /* Each  update to the polling interval we signal a hotplug poll. This
   * poll will update the hotplug devices and reset the timer. If we don't
   * signal a poll and reset the timer then an application can call this
   * API periodically resetting the timer each time but never signaling
   * the poll. */
  hotplug_force_poll_now ();

  return prev_value;
}


void
hotplug_poll_signal_received (void)
{
  (void) fs_os_clr_sigs (&hotplug_tcb, HOTPLUG_POLL_SIG);

  hotplug_poll ();

  hotplug_poll_start_timer ();
}

#endif /* FEATURE_FS_HOTPLUG */
