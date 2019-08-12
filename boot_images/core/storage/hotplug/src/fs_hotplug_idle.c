/***********************************************************************
 * fs_hotplug_idle.c
 *
 * Hotplug Idle device module.
 * Copyright (C) 2009-2010, 2012 Qualcomm Technologies, Inc.
 *
 * Hotplug devices that have not been used in 2 consecutive idle-polls will
 * be put in idle-mode (low-power) by this module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_idle.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-18   wek   Reduce hotplug size for bootloaders.
2010-09-29   dks   Moved hotplug configurable variables to fs_hotplug_config*.h
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#ifndef FEATURE_FS_HOTPLUG_DISABLE_TASK

#include "fs_hotplug_i.h"
#include "fs_osal.h"
#include "assert.h"


/* Timer to trigger periodic idle device check event.*/
static fs_os_timer_type hotplug_idle_device_timer;

/* The interval time for the idle device event generation in millisecond. */
static uint32 hotplug_idle_device_evt_duration;


void
hotplug_idle_init (void)
{
  /* Create our timer for timeout event generation. */
  fs_os_def_timer (&hotplug_idle_device_timer, &hotplug_tcb,
                   HOTPLUG_IDLE_DEVICE_SIG);

  /* Set our default idle device event generation interval. */
  hotplug_set_idle_device_event_duration
                   (FS_HOTPLUG_TIMEOUT_EVT_INTERVAL_MINUTES);
}


static void
hotplug_increment_device_idle_counter(struct hotplug_physical_device *phy_hdev)
{
  hotplug_validate_physical_hdev (phy_hdev);
  ++phy_hdev->idle_counter;
}

static uint32
hotplug_get_device_idle_counter (struct hotplug_physical_device *phy_hdev)
{
  hotplug_validate_physical_hdev (phy_hdev);
  return phy_hdev->idle_counter;
}

void
hotplug_reset_device_idle_counter (struct hotplug_physical_device *phy_hdev)
{
  hotplug_validate_physical_hdev (phy_hdev);
  phy_hdev->idle_counter = 0;
}

/*
 * Device goes into idle state only when there is no activity for that
 * device. We would set device_idle_state as TRUE in the first call of this
 * function and on subsequent call if the value of device_idle_state remains
 * TRUE then we would call the corresponding hotplug device dispatch
 * function to go to sleep mode.
 *
 * We don't require LOCK protection here, because hotplug task is running in
 * lower priority just above sleep. Hotplug task will be preempted if any other
 * task need to write. By checking the device_idle_state here, if write is
 * preempted we still get correct behavior of waiting for some more time
 * before we generate the timeout dispatch to driver. On the other hand,
 * we should not take global hotplug lock here, since read/write don't take
 * hotplug global locks. Even having per device lock will make read/writes
 * slower. When its in contention, raising the priority of hotplug to generate
 * the timeout dispatch to driver to power down and later doing another
 * initialization, which consumes a lot of time.
 *
 * Device goes into idle state only if there is no activity on that
 * particular slot between 2 consecutive polls. For e.g. if the
 * hotplug_idle_device_evt_duration is being set as 30 mins then the device
 * goes to sleep stage in between 30 mins (minimum) or 60 mins (maximum),
 * only if there is no activity going on device.
 */

void
hotplug_idle_device_dispatch (void)
{
  int i;
  struct hotplug_physical_device *phy_hdev;

  for (i = 0; i < hotplug_total_physical_devices; ++i)
  {
    phy_hdev = &hotplug_physical_devices[i];

    /* Ignore Unusable devices. */
    if (phy_hdev->dev_state ==  HPDEV_UNUSABLE)
      continue;

    hotplug_increment_device_idle_counter (phy_hdev);

    if (hotplug_get_device_idle_counter (phy_hdev) == 2)
    {
      /* For past 2 consecutive polls, the device was not used. */
      (void) hotplug_set_device_idle (phy_hdev);

      /* notify device_idle event. */
      hotplug_notify_events (phy_hdev->canonical_name, phy_hdev,
                             HOTPLUG_NOTIFY_EVENT_DEVICE_IDLE);
    }
  }

}


static void
hotplug_idle_start_timer (void)
{
  ASSERT (hotplug_idle_device_evt_duration != 0);
  /* Set the timer for first time for idle device event generation. */
  (void) fs_os_clr_timer (&hotplug_idle_device_timer);
  (void) fs_os_set_timer (&hotplug_idle_device_timer,
                   hotplug_idle_device_evt_duration);
}

static void
hotplug_idle_stop_timer (void)
{
  (void) fs_os_clr_timer (&hotplug_idle_device_timer);
}



/* Set the frequency at which Hotplug task would check for any idle
 * device present on all the slots. */
void
hotplug_set_idle_device_event_duration (uint8 minutes)
{
  /* Check the value of the input variable minutes to make sure that
   * value of minutes would not get overflow the limit of uint8. */
  if (minutes >= 255)
    return;

  hotplug_idle_device_evt_duration = minutes * 60 * 1000;

  if (hotplug_idle_device_evt_duration == 0)
  {
    hotplug_idle_stop_timer ();
  }
  else
  {
    hotplug_idle_start_timer ();
  }
}

void
hotplug_idle_device_timer_signal_received (void)
{
  (void) fs_os_clr_sigs (&hotplug_tcb, HOTPLUG_IDLE_DEVICE_SIG);

  hotplug_idle_device_dispatch ();

  hotplug_idle_start_timer ();
}

#endif /* FEATURE_FS_HOTPLUG_DISABLE_TASK */

#endif /* FEATURE_FS_HOTPLUG */
