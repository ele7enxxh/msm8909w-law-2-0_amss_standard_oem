/***********************************************************************
 * fs_hotplug_task.c
 *
 * Hotplug Task module.
 * Copyright (C) 2009-2010,2012 Qualcomm Technologies, Inc.
 *
 * Runs the Hotplug task.
 * If FEATURE_FS_HOTPLUG_DISABLE_TASK is defined, then no hotplug_task
 * is created, and COLDPLUG will be simulated by doing one and only
 * hotplug_poll()
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_task.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-02-29   rp    Make Tasks free-float (CPU affinity free)
2010-10-06   dks   Discover device when Hotplug polling is disabled.
2010-05-10   rp    In coldplug mode, fix compiler warning for missing prototype
2010-03-25   rp    In coldplug mode, create the /hdev directory.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/


#include "fs_hotplug_config_i.h"
#include "fs_hotplug.h"
#include "fs_hotplug_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_osal.h"


fs_os_tcb_type hotplug_tcb;


#ifndef FEATURE_FS_HOTPLUG_DISABLE_TASK

#include "task.h"
#include "fs_hotplug_usbhost.h"

/* Hotplug has its own task to asynchronously poll devices */
static void hotplug_task (dword arg);

#define HOTPLUG_STACK_SIZE 2048 /* (needs a measured reduction) */
static unsigned char hotplug_stack [HOTPLUG_STACK_SIZE];


/* This function sets up the hotplug task if FEATURE_FS_HOTPLUG is
 * enabled. Otherwise it forces a single poll if coldplug is enabled. */
void
hotplug_task_init (void)
{
  /* Start the hotplug management task. */
  fs_os_def_task_ext (&hotplug_tcb,
                    hotplug_stack,
                    HOTPLUG_STACK_SIZE,
                    FS_HOTPLUG_PRI,
                    hotplug_task,
                    0,
                    "FS Hotplug Task",
                    0,
                    FS_DOG_INVALID_RPT);

  hotplug_legacy_names_init ();
  hotplug_mount_init ();
  hotplug_poll_init ();
  hotplug_idle_init ();
}



static void
hotplug_task (dword arg)
{
  fs_os_sigs_type sigs;

  (void) arg;                   /* unused */

  /* Register with USB for notifications */
  hotplug_usbhost_register();

  while (1)
  {
    sigs = fs_os_wait (HOTPLUG_POLL_SIG
                     | HOTPLUG_USBHOST_OPEN_SIG
                     | HOTPLUG_USBHOST_CLOSE_SIG
                     | HOTPLUG_IDLE_DEVICE_SIG);

    /* Did we lose a connected device?   Handle this FIRST */
    if (sigs & HOTPLUG_USBHOST_CLOSE_SIG)
    {
      (void) fs_os_clr_sigs (&hotplug_tcb, HOTPLUG_USBHOST_CLOSE_SIG);
      hotplug_usbhost_close ();
      hotplug_force_poll_now ();
    }

    /* Has hotplug_usbhost_callback signalled that a device is ready? */
    if (sigs & HOTPLUG_USBHOST_OPEN_SIG)
    {
      (void) fs_os_clr_sigs (&hotplug_tcb, HOTPLUG_USBHOST_OPEN_SIG);
      hotplug_usbhost_open ();
      hotplug_force_poll_now ();
    }

    /* Has our timer expired?  Then it's time to visit our devices */
    if (sigs & HOTPLUG_POLL_SIG)
    {
      hotplug_poll_signal_received ();
    }

    /* Has our timer expired?
     * Then it's the time to visit for timeout event check. */
    if (sigs & HOTPLUG_IDLE_DEVICE_SIG)
    {
      hotplug_idle_device_timer_signal_received ();
    }

    (void) fs_os_clr_sigs (&hotplug_tcb, sigs);
  }
}

void
hotplug_task_discover_device (const char *hdev_name)
{
  (void) hdev_name;
}

#else /* !(FEATURE_FS_HOTPLUG_DISABLE_TASK) */

/* Hotplug is enabled, but Hotplug-Task is disabled, so a single poll at
 * startup is needed to simulate COLDPLUG. */
void
hotplug_task_init (void)
{
  hotplug_legacy_names_init ();
  hotplug_mount_init ();
  hotplug_poll ();
}

void
hotplug_task_discover_device (const char *hdev_name)
{
  struct hotplug_physical_device *phy_hdev;
  phy_hdev = hotplug_find_physical_device (hdev_name);
  if (phy_hdev && phy_hdev->parti_list == NULL)
  {
    hotplug_discover_device (phy_hdev);
  }
}

#endif /* !(FEATURE_FS_HOTPLUG_DISABLE_TASK) */

#endif /* FEATURE_FS_HOTPLUG */

