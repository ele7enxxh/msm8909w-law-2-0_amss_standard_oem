/***********************************************************************
 * fs_hotplug_events.c
 *
 * Hotplug Events module.
 * Copyright (C) 2009-2010, 2012 Qualcomm Technologies, Inc.
 *
 * Implements Hotplug Events nofication functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_events.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-18   wek   Reduce hotplug size for bootloaders.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/


#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#ifndef FEATURE_FS_HOTPLUG_DISABLE_TASK

#include "fs_hotplug_i.h"

#include <string.h>

/* This structure represents one instance of a registered Hotplug
 * Notification client. */
struct hotplug_notify_client
{
  hotplug_dev_type                device_type;
  hotplug_notify_event            event_mask;
  hotplug_notify_event_cb         user_cb;
  hotplug_notify_event_cb_new     user_cb_new;
  void                            *user_data;
};

/* The whole set of registered clients is in this array */
static struct hotplug_notify_client
  hotplug_notify_client_tbl[FS_HOTPLUG_NOTIFY_MAX_CLIENTS];


void
hotplug_events_init (void)
{
  memset (hotplug_notify_client_tbl, 0, sizeof (hotplug_notify_client_tbl));
}


static unsigned int
hotplug_events_get_free_slot (void)
{
  unsigned int i;

  /* find an empty slot in the client table */
  for (i = 0; i < FS_HOTPLUG_NOTIFY_MAX_CLIENTS; ++i)
  {
    if ( (hotplug_notify_client_tbl[i].user_cb == NULL) &&
         (hotplug_notify_client_tbl[i].user_cb_new == NULL) )
    {
      break;
    }
  }

  return i;
}



hotplug_notify_handle
hotplug_notify_register (hotplug_dev_type device_type,
                         hotplug_notify_event event_mask,
                         hotplug_notify_event_cb user_cb,
                         void *user_data)
{
  hotplug_notify_handle ret_val = HOTPLUG_NOTIFY_INVALID_HANDLE;
  unsigned int i;

  /* validate the input parameters */
  if (user_cb == NULL)
  {
    return ret_val;
  }

  HOTPLUG_LOCK ();

  i = hotplug_events_get_free_slot ();
  if (i < FS_HOTPLUG_NOTIFY_MAX_CLIENTS)
  {
    /* allocate this empty slot to this client. */
    hotplug_notify_client_tbl[i].device_type = device_type;
    hotplug_notify_client_tbl[i].event_mask = event_mask;
    hotplug_notify_client_tbl[i].user_cb = user_cb;
    hotplug_notify_client_tbl[i].user_cb_new = NULL;
    hotplug_notify_client_tbl[i].user_data = user_data;

    /* this slot index (base 1) is the registration-handle for this client. */
    ret_val = (hotplug_notify_handle)(i + 1);
  }

  HOTPLUG_UNLOCK ();
  return ret_val;
}

hotplug_notify_handle
hotplug_notify_register_new (hotplug_dev_type device_type,
                             hotplug_notify_event event_mask,
                             hotplug_notify_event_cb_new user_cb_new,
                             void *user_data)
{
  hotplug_notify_handle ret_val = HOTPLUG_NOTIFY_INVALID_HANDLE;
  unsigned int i;

  /* validate the input parameters */
  if (user_cb_new == NULL)
  {
    return ret_val;
  }

  HOTPLUG_LOCK ();

  i = hotplug_events_get_free_slot ();
  if (i < FS_HOTPLUG_NOTIFY_MAX_CLIENTS)
  {
    /* allocate this empty slot to this client. */
    hotplug_notify_client_tbl[i].device_type = device_type;
    hotplug_notify_client_tbl[i].event_mask = event_mask;
    hotplug_notify_client_tbl[i].user_cb = NULL;
    hotplug_notify_client_tbl[i].user_cb_new = user_cb_new;
    hotplug_notify_client_tbl[i].user_data = user_data;

    /* this slot index (base 1) is the registration-handle for this client. */
    ret_val = (hotplug_notify_handle)(i + 1);
  }

  HOTPLUG_UNLOCK ();
  return ret_val;
}

int
hotplug_notify_unregister (hotplug_notify_handle reg_hdl)
{
  int ret_val = -1;

  /* sanity check on the registration handle */
  if ((reg_hdl != HOTPLUG_NOTIFY_INVALID_HANDLE) &&
      (reg_hdl <= FS_HOTPLUG_NOTIFY_MAX_CLIENTS))
  {
    /* For us the registration handle is 0-based array index. */
    reg_hdl--;

    HOTPLUG_LOCK ();

    /* Make sure we are not attempting to free an already-empty slot */
    if ( (hotplug_notify_client_tbl[reg_hdl].user_cb != NULL) ||
         (hotplug_notify_client_tbl[reg_hdl].user_cb_new != NULL) )
    {
      /* mark this slot as free by zeroing out all fields. */
      memset (&hotplug_notify_client_tbl[reg_hdl], 0,
              sizeof (hotplug_notify_client_tbl[0]));

      ret_val = 0;              /* Success */
    }

    HOTPLUG_UNLOCK ();
  }

  return ret_val;
}



void
hotplug_notify_events (const char *hdev_name,
                       struct hotplug_physical_device *phy_hdev,
                       hotplug_notify_event event_id)
{
  unsigned int i;
  struct hotplug_notify_event_callback_info event_info;

  memset (&event_info, 0, sizeof (event_info));

  HOTPLUG_LOCK ();

  event_info.hdev_name = hdev_name;
  event_info.dev_type = phy_hdev->dev_type;

  /* loop through each slot and check if we have to do a callback. */
  for (i = 0; i < FS_HOTPLUG_NOTIFY_MAX_CLIENTS; ++i)
  {
    hotplug_notify_event interesting_events;

    /* skip empty slots */
    if ( (hotplug_notify_client_tbl[i].user_cb == 0) &&
         (hotplug_notify_client_tbl[i].user_cb_new == 0) )
      continue;

    /* Mask off any unrequested events, leaving only the ones of interest */
    interesting_events = event_id & hotplug_notify_client_tbl[i].event_mask;

    /* check if these events are registered for. */
    if (interesting_events == 0)
      continue;

    /* check if this device is registered for. */
    if ( (hotplug_notify_client_tbl[i].device_type != HOTPLUG_TYPE_ALL) &&
         (hotplug_notify_client_tbl[i].device_type != phy_hdev->dev_type) )
    {
      continue;
    }

    event_info.event_mask = interesting_events;

    /* if we make it till here, then this client has registered for this
       device-type and events, so invoke the user's callback */

    if (hotplug_notify_client_tbl[i].user_cb)
    {
      (hotplug_notify_client_tbl[i].user_cb) (phy_hdev->legacy_hdev,
          interesting_events, i+1, hotplug_notify_client_tbl[i].user_data);
    }
    else if (hotplug_notify_client_tbl[i].user_cb_new != NULL)
    {
      (hotplug_notify_client_tbl[i].user_cb_new) (&event_info, i+1,
          hotplug_notify_client_tbl[i].user_data);
    }
  }

  HOTPLUG_UNLOCK ();
}

#else /* FEATURE_FS_HOTPLUG_DISABLE_TASK is defined*/
#include "fs_err.h"

hotplug_notify_handle
hotplug_notify_register (hotplug_dev_type device_type,
                         hotplug_notify_event event_mask,
                         hotplug_notify_event_cb user_cb,
                         void *user_data)
{
  (void) device_type;
  (void) event_mask;
  (void) user_cb;
  (void) user_data;
  FS_ERR_FATAL ("Unsupported function.", 0, 0, 0);
  return HOTPLUG_NOTIFY_INVALID_HANDLE;
}

hotplug_notify_handle
hotplug_notify_register_new (hotplug_dev_type device_type,
                             hotplug_notify_event event_mask,
                             hotplug_notify_event_cb_new user_cb_new,
                             void *user_data)
{
  (void) device_type;
  (void) event_mask;
  (void) user_cb_new;
  (void) user_data;
  FS_ERR_FATAL ("Unsupported function.", 0, 0, 0);
  return HOTPLUG_NOTIFY_INVALID_HANDLE;
}

int
hotplug_notify_unregister (hotplug_notify_handle reg_hdl)
{
  (void) reg_hdl;
  FS_ERR_FATAL ("Unsupported function.", 0, 0, 0);
  return -1;
}

#endif /* FEATURE_FS_HOTPLUG_DISABLE_TASK */

#endif /* FEATURE_FS_HOTPLUG */

