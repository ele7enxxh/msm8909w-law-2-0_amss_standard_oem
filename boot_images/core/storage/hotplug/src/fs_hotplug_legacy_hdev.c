/***********************************************************************
 * fs_hotplug_legacy_hdev.c
 *
 * Short description.
 * Copyright (C) 2010, 2012-2014 QUALCOMM Technologies, Inc.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_legacy_hdev.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2013-05-10   wek   Fix compile time errors when hotplug is turned off.
2012-03-18   wek   Reduce hotplug size for bootloaders.
2010-10-21   dks   Fix hotplug APIs to work correctly for legacy hdevs.
2010-09-02   rp    Fix possible NULL reference in hotplug_find_dev_by_path.
2010-02-19   rp    Create

===========================================================================*/


#include "fs_hotplug_config.h"

#ifdef FEATURE_FS_HOTPLUG

#ifdef FEATURE_FS_HOTPLUG_LEGACY_HDEV_SUPPORT

#include "fs_hotplug_i.h"
#include "fs_err.h"
#include "assert.h"

#include <string.h>

void
hotplug_legacy_hdev_create (struct hotplug_physical_device *phy_hdev)
{
  phy_hdev->legacy_hdev = hotplug_hdev_alloc ();
  if (phy_hdev->legacy_hdev == NULL)
  {
    FS_ERR_FATAL ("ran out of hdev's",0,0,0);
  }

  phy_hdev->legacy_hdev->phy_hdev = phy_hdev;
}

int
hotplug_legacy_hdev_open (struct hotplug_physical_device *phy_hdev)
{
  void *bdev_handle = NULL;
  uint32 block_cnt;
  uint16 bytes_per_block;
  int result = -1;
  struct hotplug_partition *parti;

  ASSERT (phy_hdev->legacy_hdev != NULL);

  parti = phy_hdev->parti_list;
  if (parti == NULL)
    goto Error;

  result = blockdev_open (phy_hdev->bdev, &bdev_handle, 0);
  if (result != 0)
    goto Error;

  result = blockdev_get_device_size (phy_hdev->bdev, bdev_handle,
                                     &block_cnt, &bytes_per_block);
  if (result != 0)
    goto Error;

  phy_hdev->legacy_hdev->size_in_sectors = parti->size_in_sectors;
  phy_hdev->legacy_hdev->one_sector_size = bytes_per_block;

  phy_hdev->legacy_hdev->bdev_handle = bdev_handle;
  phy_hdev->legacy_hdev->parti = parti;
  hotplug_partition_ref (parti);

  goto Success;

Error:
  if (bdev_handle)
    blockdev_close (parti->phy_hdev->bdev, bdev_handle);

Success:
  return result;
}


void
hotplug_legacy_hdev_close (struct hotplug_physical_device *phy_hdev)
{
  ASSERT (phy_hdev->legacy_hdev != NULL);

  if (phy_hdev->legacy_hdev->parti == NULL)
    return;

  (void) blockdev_close (phy_hdev->bdev, phy_hdev->legacy_hdev->bdev_handle);
  phy_hdev->legacy_hdev->bdev_handle = NULL;

  hotplug_partition_unref (phy_hdev->legacy_hdev->parti);
  phy_hdev->legacy_hdev->parti = NULL;

  phy_hdev->legacy_hdev->size_in_sectors = 0;
  phy_hdev->legacy_hdev->one_sector_size = 0;
}



void
fs_sfat_lock_card (void)
{
}

void
fs_sfat_unlock_card (void)
{
}

struct hotplug_device*
hotplug_hdev (int16 drive_id)
{
  (void) drive_id;
  return NULL;
}


int
hotplug_dev_get_driveno (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

void*
hotplug_dev_get_handle (struct hotplug_device *hdev)
{
  (void) hdev;
  return NULL;
}

struct hotplug_device*
hotplug_find_dev_by_type (hotplug_dev_type dev_type,
                          struct hotplug_device *last)
{
  struct hotplug_physical_device *phy_hdev;
  struct hotplug_device *hdev = NULL;
  int i;

  if (last == NULL)
  {
    i = 0;
  }
  else
  {
    phy_hdev = last->phy_hdev;
      /* Loop and go to next physical device skipping all unusable devices. */
    for (i = 0; i < hotplug_total_physical_devices; ++i)
    {
      if (phy_hdev == &hotplug_physical_devices[i])
        break;
    }
    ++i;
  }

  /* Loop and go to next physical device skipping all unusable devices. */
  for (; i < hotplug_total_physical_devices; ++i)
  {
    phy_hdev = &hotplug_physical_devices[i];

    /* skip hidden and unusable devices. */
    if ( (phy_hdev->is_hidden) ||
         (phy_hdev->dev_state == HPDEV_UNUSABLE) )
      continue;

    if ( (dev_type != HOTPLUG_TYPE_ALL) &&
         (phy_hdev->dev_type != dev_type) )
      continue;

    hdev = phy_hdev->legacy_hdev;
    break;
  }

  return hdev;
}

struct hotplug_device*
hotplug_find_dev_by_path (const char *path)
{
  struct hotplug_device *hdev = NULL;
  struct hotplug_physical_device *phy_hdev;
  const char *legacy_name;
  int i;

  if (path == NULL)
    return NULL;

  /* Loop and go to next physical device skipping all unusable devices. */
  for (i=0; i < hotplug_total_physical_devices; ++i)
  {
    phy_hdev = &hotplug_physical_devices[i];

    /* skip hidden and unusable devices. */
    if ( (phy_hdev->is_hidden) ||
         (phy_hdev->dev_state == HPDEV_UNUSABLE) )
      continue;

    legacy_name = hotplug_legacy_names_get_path (phy_hdev->canonical_name);
    if (legacy_name  && (strcmp (legacy_name, path) == 0))
    {
      hdev = phy_hdev->legacy_hdev;
      break;
    }
  }

  return hdev;
}

const char*
hotplug_legacy_hdev_get_path (struct hotplug_device *hdev)
{
  return hotplug_legacy_names_get_path (hdev->phy_hdev->canonical_name);
}

int
hotplug_legacy_hdev_lock (struct hotplug_device *hdev)
{
  int result = -1;

  switch (hdev->phy_hdev->dev_state)
  {
    case HPDEV_UNUSABLE:
    case HPDEV_UNDISCOVERED:
    {
      hotplug_change_physical_dev_state (hdev->phy_hdev, HPDEV_LOCKED);
      hotplug_notify_events (hdev->phy_hdev->canonical_name, hdev->phy_hdev,
                             HOTPLUG_NOTIFY_EVENT_CARD_LOCKED);
      result = 0;
    }
    break;

    default:
      break;
  }

  return result;
}

int
hotplug_legacy_hdev_unlock (struct hotplug_device *hdev)
{
  int result = -1;

  if (hdev->phy_hdev->dev_state == HPDEV_LOCKED)
  {
    result = hotplug_unlock_entire_device (hdev);
  }

  return result;
}

int
hotplug_legacy_hdev_is_mounted (struct hotplug_device *hdev)
{
  struct hotplug_partition *parti;
  int is_mounted = 0;

  parti = hdev->parti;
  while (parti != NULL)
  {
    is_mounted = parti->is_mounted;
    if (is_mounted)
      break;

    parti = parti->next_parti;
  }

  return is_mounted;
}

int
hotplug_legacy_hdev_get_state (struct hotplug_device *hdev,
                               struct hdev_state *state)
{
  if (hdev->parti)
  {
    state->is_present = 1;
    state->is_mounted = hotplug_legacy_hdev_is_mounted (hdev);
    state->is_formatting = hdev->parti->is_formatting;
    state->is_locked = hdev->parti->is_locked;
  }

  return 0;
}

#endif /* FEATURE_FS_HOTPLUG_LEGACY_HDEV_SUPPORT */

#else

extern int __dont_complain_about_empty_file;

#endif /* FEATURE_FS_HOTPLUG */
