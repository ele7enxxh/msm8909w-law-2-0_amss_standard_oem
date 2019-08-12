/***********************************************************************
 * fs_hotplug_dev.c
 *
 * Hotplug physical device helper functions.
 * Copyright (C) 2009-2012 Qualcomm Technologies, Inc.
 *
 * Hotplug physical device helper functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_dev.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-20   wek   Cleanup customer header file inclusion.
2012-01-17   wek   Remove unused header files to compile for boot.
2011-11-10   rp    Return null when hotplug_find_physical_device fails.
2011-03-31   wek   Implement GPT/MBR partition table parser.
2010-10-06   dks   Discover device when Hotplug polling is disabled.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/


#include "fs_hotplug_config_i.h"


#ifdef FEATURE_FS_HOTPLUG

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "fs_sys_types.h"
#include "fs_errno.h"
#include "fs_osal.h"
#include "fs_hotplug.h"
#include "fs_hotplug_i.h"
#include "fs_blockdev.h"
#include "fs_err.h"
#include "assert.h"



/* Allocate a new device from the pool of hotplug devices */
struct hotplug_device *
hotplug_hdev_alloc (void)
{
  struct hotplug_device *hdev = NULL;
  int i;

  for (i = 0; i < FS_HOTPLUG_MAX_HDEVS; ++i)
  {
    if (hotplug_devices[i].is_in_use == 0)
    {
      hdev = &hotplug_devices[i];
      break;
    }
  }

  if (hdev != NULL)
  {
    memset (hdev, 0, sizeof (struct hotplug_device));
    hdev->is_in_use = 1;
    ++hotplug_total_devices;
  }

  return hdev;
}

/* Free chain of hotplug devices */
void
hotplug_hdev_free (struct hotplug_device *hdev)
{
  if (hdev == NULL)
    FS_ERR_FATAL ("Attempt to free NULL device!", 0,0,0);

  if (hotplug_total_devices <= 0)
    FS_ERR_FATAL ("hotplug_total_devices underflow..", 0,0,0);

  --hotplug_total_devices;

  /* Free device */
  memset (hdev, 0, sizeof (struct hotplug_device));
}

void
hotplug_add_hdev_to_phy_hdev (struct hotplug_physical_device *phy_hdev,
                              struct hotplug_device *new_hdev)
{
  struct hotplug_device *hdev;

  if (phy_hdev->hdev_list == NULL)
  {
    /* Insertion at head. */
    phy_hdev->hdev_list = new_hdev;
    return;
  }

  /* Insertion at tail. */
  hdev = phy_hdev->hdev_list;
  while (hdev->next_hdev != NULL)
    hdev = hdev->next_hdev;

  hdev->next_hdev = new_hdev;
  new_hdev->next_hdev = NULL;
}

void
hotplug_remove_hdev_from_phy_hdev (struct hotplug_physical_device *phy_hdev,
                                   struct hotplug_device *freeing_hdev)
{
  struct hotplug_device *hdev;

  /* Check if removal at head? */
  if (phy_hdev->hdev_list == freeing_hdev)
  {
    if (freeing_hdev != NULL)
      phy_hdev->hdev_list = freeing_hdev->next_hdev;
    else
      phy_hdev->hdev_list = NULL;

    return;
  }

  hdev = phy_hdev->hdev_list;
  while (hdev != NULL)
  {
    if (hdev->next_hdev == freeing_hdev)
    {
      hdev->next_hdev = freeing_hdev->next_hdev;
      break;
    }

    hdev = hdev->next_hdev;
  }
}


void
hotplug_add_partition (struct hotplug_physical_device *phy_hdev,
                       struct hotplug_partition *new_parti)
{
  struct hotplug_partition *parti;

  if (phy_hdev->parti_list == NULL)
  {
    /* Insertion at the head */
    phy_hdev->parti_list = new_parti;
    return;
  }

  /* Insertion at tail. */
  parti = phy_hdev->parti_list;
  while (parti->next_parti != NULL)
    parti = parti->next_parti;

  parti->next_parti = new_parti;
  new_parti->next_parti = NULL;
}


static struct hotplug_partition*
hotplug_find_partition_in_physical_device (
  struct hotplug_physical_device *phy_hdev,
  const char *hdev_name)
{
  struct hotplug_partition *parti = NULL;

  for (parti = phy_hdev->parti_list;
       parti != NULL;
       parti = parti->next_parti )
  {
    if (strcmp (parti->canonical_name, hdev_name) == 0)
      break;

    if ((parti->friendly_name) &&
        (strcmp (parti->friendly_name, hdev_name) == 0))
      break;
  }

  return parti;
}

struct hotplug_partition*
hotplug_find_partition (const char *hdev_name)
{
  struct hotplug_physical_device *phy_hdev;
  struct hotplug_partition *parti = NULL;
  int i;

  for (i = 0; i < hotplug_total_physical_devices; ++i)
  {
    phy_hdev = &hotplug_physical_devices[i];

    /* skip hidden and unusable devices. */
    if ( (phy_hdev->is_hidden) ||
         (phy_hdev->dev_state == HPDEV_UNUSABLE) )
      continue;

    parti = hotplug_find_partition_in_physical_device (phy_hdev, hdev_name);
    if (parti != NULL)
      break;
  }

  return parti;
}

struct hotplug_physical_device*
hotplug_find_physical_device (const char *name)
{
  struct hotplug_physical_device *phy_hdev = NULL;
  int i;

  if (name == NULL)
    return NULL;

  for (i = 0; i < hotplug_total_physical_devices; ++i)
  {
    phy_hdev = &hotplug_physical_devices[i];

    /* skip hidden and unusable devices. */
    if ( (phy_hdev->is_hidden) ||
         (phy_hdev->dev_state == HPDEV_UNUSABLE) )
      continue;

    if (strcmp (phy_hdev->canonical_name, name) == 0)
      break;
  }

  if (i == hotplug_total_physical_devices)
  {
    phy_hdev = NULL;
  }

  return phy_hdev;
}


#endif /* FEATURE_FS_HOTPLUG */
