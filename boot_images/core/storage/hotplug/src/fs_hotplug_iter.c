/***********************************************************************
 * fs_hotplug_iter.c
 *
 * Hotrplug iterator module.
 * Copyright (C) 2009-2012 Qualcomm Technologies, Inc.
 *
 * Implements the hotplug iterator APIs.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_iter.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-01-17   wek   Remove unused utils header file.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-05-01   wek   Comply with legacy behavior for MBR parsing.
2011-03-31   wek   Implement GPT/MBR partition table parser.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_hotplug_i.h"
#include "fs_hotplug_parser.h"
#include "assert.h"
#include "stringl/stringl.h"
#include <string.h>


static struct hotplug_iter hotplug_iter_inst[FS_HOTPLUG_MAX_ITERATORS];

void
hotplug_iter_init (void)
{
  memset (hotplug_iter_inst, 0, sizeof (hotplug_iter_inst));
}

static struct hotplug_iter*
hotplug_iter_allocate (void)
{
  struct hotplug_iter *hiter = NULL;
  int i;

  for (i = 0; i < FS_HOTPLUG_MAX_ITERATORS; ++i)
  {
    if (hotplug_iter_inst[i].is_in_use == 0)
    {
      hotplug_iter_inst[i].is_in_use = 1;
      hiter = &hotplug_iter_inst[i];
      break;
    }
  }

  return hiter;
}

static void
hotplug_iter_free (struct hotplug_iter *hiter)
{
  memset (hiter, 0, sizeof (struct hotplug_iter));
}

static void
hotplug_iter_goto_next_device (struct hotplug_iter *hiter)
{
  int i;
  struct hotplug_physical_device *phy_hdev, *prev_phy_hdev, *next_phy_hdev;

  prev_phy_hdev = hiter->phy_hdev;
  next_phy_hdev = NULL;

  /* Loop and go to next physical device skipping all unusable devices. */
  for (i = 0; i < hotplug_total_physical_devices; ++i)
  {
    phy_hdev = &hotplug_physical_devices[i];

    /* skip hidden and unusable devices. */
    if ( (phy_hdev->is_hidden) ||
         (phy_hdev->dev_state == HPDEV_UNUSABLE) )
      continue;

    if ( (hiter->dev_type != HOTPLUG_TYPE_ALL) &&
         (phy_hdev->dev_type != hiter->dev_type) )
      continue;

    if ( (hiter->dev_location == HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY) &&
        (!phy_hdev->is_embedded_device) )
        continue;

    if ( (hiter->dev_location == HOTPLUG_ITER_EXTERNAL_DEVICES_ONLY) &&
        (phy_hdev->is_embedded_device) )
        continue;

    if (prev_phy_hdev == NULL)
    {
      next_phy_hdev = phy_hdev;
      break;
    }

    if (prev_phy_hdev == phy_hdev)
    {
      prev_phy_hdev = NULL;
      continue;
    }

  }

  hiter->phy_hdev = next_phy_hdev;
}

static int
hotplug_iter_goto_next_partition (struct hotplug_iter *hiter)
{
  if (hiter->parti != NULL)
  {
    hiter->parti = hiter->parti->next_parti;
    if (hiter->parti != NULL)
      return 0;
  }

  hotplug_iter_goto_next_device (hiter);
  while (hiter->phy_hdev != NULL)
  {
    hiter->parti = hiter->phy_hdev->parti_list;
    if (hiter->parti != NULL)
      return 0;

    hotplug_iter_goto_next_device (hiter);
  }

  return -1;
}

static struct hotplug_iter_record*
hotplug_iter_get_next_device (struct hotplug_iter *hiter)
{
  hotplug_iter_goto_next_device (hiter);
  if (hiter->phy_hdev == NULL)
    return NULL;

  (void) strlcpy (hiter->iter_rec.hdev_name,hiter->phy_hdev->canonical_name,
                  sizeof (hiter->iter_rec.hdev_name));
  hiter->iter_rec.device_type = hiter->phy_hdev->dev_type;
  hiter->iter_rec.is_mounted = (hiter->phy_hdev->dev_state == HPDEV_MOUNTED);
  hiter->iter_rec.is_formatting =
                          (hiter->phy_hdev->dev_state == HPDEV_FORMATTING);
  hiter->iter_rec.is_locked = (hiter->phy_hdev->dev_state == HPDEV_LOCKED);
  hiter->iter_rec.is_embedded_device = hiter->phy_hdev->is_embedded_device;

  return &hiter->iter_rec;
}

static struct hotplug_partition*
hotplug_iter_get_next_partition (struct hotplug_iter *hiter)
{
  hotplug_iter_goto_next_partition (hiter);
  while (hiter->parti != NULL)
  {
    switch (hiter->iter_type)
    {
      case HOTPLUG_ITER_HARD_PARTITIONS:
        if (hiter->parti->partition_type == HOTPLUG_HARD_PARTITION)
        {
          return hiter->parti;
        }
        break;

      case HOTPLUG_ITER_SOFT_PARTITIONS:
      case HOTPLUG_ITER_FAT_PARTITIONS:
        if (hiter->parti->partition_type == HOTPLUG_SOFT_MBR_PARTITION ||
            hiter->parti->partition_type == HOTPLUG_SOFT_GPT_PARTITION ||
            hiter->parti->partition_type == HOTPLUG_SOFT_FAT_PARTITION)
        {
          return hiter->parti;
        }
        break;

      default:
        ASSERT (0);
        break;
    }

    hotplug_iter_goto_next_partition (hiter);
  }

  return NULL;
}


static struct hotplug_partition*
hotplug_iter_get_next_fat_partition (struct hotplug_iter *hiter)
{
  hotplug_iter_get_next_partition (hiter);
  while (hiter->parti != NULL)
  {
    if (hotplug_is_it_a_fat_partition (hiter->parti))
      return hiter->parti;

    hotplug_iter_get_next_partition (hiter);
  }

  return NULL;
}



struct hotplug_iter*
hotplug_iter_open (hotplug_dev_type dev_type, enum hotplug_iter_type iter_type,
                   enum hotplug_iter_device_location dev_location)
{
  struct hotplug_iter *hiter = NULL;

  HOTPLUG_LOCK ();

  hiter = hotplug_iter_allocate ();
  if (hiter != NULL)
  {
    hiter->dev_type = dev_type;
    hiter->iter_type = iter_type;
    hiter->dev_location = dev_location;
    hiter->phy_hdev = NULL;
    hiter->parti = NULL;
  }

  HOTPLUG_UNLOCK ();

  return hiter;
}


struct hotplug_iter_record*
hotplug_iter_get_next (struct hotplug_iter *hiter)
{
  struct hotplug_partition *parti = NULL;
  struct hotplug_iter_record *iter_rec = NULL;

  if (hiter == NULL)
    return NULL;

  HOTPLUG_LOCK ();

  memset (&hiter->iter_rec, 0, sizeof (hiter->iter_rec));

  switch (hiter->iter_type)
  {
    case HOTPLUG_ITER_DEVICES:
      iter_rec = hotplug_iter_get_next_device (hiter);
      break;

    case HOTPLUG_ITER_HARD_PARTITIONS:
    case HOTPLUG_ITER_SOFT_PARTITIONS:
      parti = hotplug_iter_get_next_partition (hiter);
      break;

    case HOTPLUG_ITER_FAT_PARTITIONS:
      parti = hotplug_iter_get_next_fat_partition (hiter);
      break;

    default:
      break;
  }


  if (parti != NULL)
  {
    uint32 mbr_boot_type;
    (void) strlcpy (hiter->iter_rec.hdev_name, parti->canonical_name,
                    sizeof (hiter->iter_rec.hdev_name));
    hiter->iter_rec.device_type = parti->phy_hdev->dev_type;
    hiter->iter_rec.is_fat_partition = hotplug_is_it_a_fat_partition (parti);
    if (parti->partition_type == HOTPLUG_SOFT_MBR_PARTITION)
      mbr_boot_type = parti->boot_record_partition_type.mbr;
    else
      mbr_boot_type = 0;

    /* The iterator record used to return 0xFFFF when the type was extended. */
    if (mbr_boot_type == HOTPLUG_MBR_DOS_EXTENDED_PARTITION ||
        mbr_boot_type == HOTPLUG_MBR_WIN_EXTENDED_PARTITION)
      mbr_boot_type = 0xFFFF;

    hiter->iter_rec.boot_record_partition_type = mbr_boot_type;
    hiter->iter_rec.bootable = parti->bootable;

    if (parti->partition_type == HOTPLUG_ENTIRE_DEVICE ||
        parti->partition_type == HOTPLUG_HARD_PARTITION)
      hiter->iter_rec.is_hard_partition = 1;
    else
      hiter->iter_rec.is_hard_partition = 0;

    hiter->iter_rec.is_mounted = parti->is_mounted;
    hiter->iter_rec.is_formatting = parti->is_formatting;
    hiter->iter_rec.is_locked = parti->is_locked;
    hiter->iter_rec.is_embedded_device = parti->phy_hdev->is_embedded_device;
    iter_rec = &hiter->iter_rec;
  }

  HOTPLUG_UNLOCK ();

  return iter_rec;
}


int
hotplug_iter_close (struct hotplug_iter *hiter)
{
  if (hiter == NULL)
    return -1;

  HOTPLUG_LOCK ();

  hotplug_iter_free (hiter);

  HOTPLUG_UNLOCK ();

  return 0;
}

#endif /* FEATURE_FS_HOTPLUG */
