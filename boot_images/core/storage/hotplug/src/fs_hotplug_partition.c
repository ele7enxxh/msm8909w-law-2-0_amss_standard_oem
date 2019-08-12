/***********************************************************************
 * fs_hotplug_partition.c
 *
 * Hotplug partition helper functions.
 * Copyright (C) 2009-2011 Qualcomm Technologies, Inc.
 *
 * Hotplug partition helper functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_partition.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-04-05   dks   Fix hotplug notify event sequence.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_hotplug_i.h"
#include "assert.h"
#include "fs_err.h"

#include <string.h>


/* Array of partitions. */
struct hotplug_partition hotplug_partitions[FS_HOTPLUG_MAX_PARTITIONS];
int hotplug_total_partitions;


void
hotplug_init_partitions (void)
{
  memset (hotplug_partitions, 0, sizeof (hotplug_partitions));
  hotplug_total_partitions = 0;
}

struct hotplug_partition*
hotplug_partition_alloc (void)
{
  struct hotplug_partition *parti = NULL;
  int i;

  for (i = 0; i < FS_HOTPLUG_MAX_PARTITIONS; ++i)
  {
    if (hotplug_partitions[i].ref_cnt == 0)
    {
      parti = &hotplug_partitions[i];
      memset (parti, 0, sizeof (struct hotplug_partition));
      parti->ref_cnt = 1;
      ++hotplug_total_partitions;
      break;
    }
  }

  return parti;
}

void
hotplug_partition_ref (struct hotplug_partition *parti)
{
  ASSERT (parti != NULL);
  ASSERT (parti->ref_cnt >= 1);

  ++parti->ref_cnt;
}

void
hotplug_partition_unref (struct hotplug_partition *parti)
{
  ASSERT (parti != NULL);
  ASSERT (parti->ref_cnt >= 1);

  --parti->ref_cnt;
  if (parti->ref_cnt > 0)
    return;

  if (hotplug_total_partitions <= 0)
    FS_ERR_FATAL ("hotplug_total_partitions underflow",0,0,0);

  --hotplug_total_partitions;

  memset (parti, 0, sizeof (struct hotplug_partition));
}


void
hotplug_free_all_partitions (struct hotplug_physical_device *phy_hdev)
{
  struct hotplug_partition *parti, *temp_parti;

  ASSERT (phy_hdev != NULL);

  parti = phy_hdev->parti_list;
  phy_hdev->parti_list = NULL;

  /* Blow away all the partitions, they are reference counted. */
  while (parti != NULL)
  {
    temp_parti = parti;
    parti = parti->next_parti;
    hotplug_partition_unref (temp_parti);
  }
}


void
hotplug_mount_all_partitions (struct hotplug_physical_device *phy_hdev,
                              const char *hdev_name)
{
  struct hotplug_partition *parti;
  int len;

  ASSERT (phy_hdev != NULL);
  ASSERT (hdev_name != NULL);

  hotplug_legacy_names_create (phy_hdev);

  len = strlen (hdev_name);

  for ( parti = phy_hdev->parti_list;
        parti != NULL;
        parti = parti->next_parti)
  {
    if (strncmp (hdev_name, parti->canonical_name, len) == 0)
    {
      (void )hotplug_partition_mount_helper (parti);
    }
  }
}


void
hotplug_unmount_all_partitions (struct hotplug_physical_device *phy_hdev,
                                const char *hdev_name)
{
  struct hotplug_partition *parti;
  int len;

  ASSERT (phy_hdev != NULL);
  ASSERT (hdev_name != NULL);

  hotplug_legacy_names_remove (phy_hdev);

  len = strlen (hdev_name);

  for ( parti = phy_hdev->parti_list;
        parti != NULL;
        parti = parti->next_parti)
  {
    if (strncmp (hdev_name, parti->canonical_name, len) == 0)
    {
      (void) hotplug_partition_unmount_helper (parti);
    }
  }

}

void
hotplug_mark_all_partitions_as_formatting (
    struct hotplug_physical_device *phy_hdev, const char *hdev_name)
{
  struct hotplug_partition *parti;
  int len;

  ASSERT (phy_hdev != NULL);
  ASSERT (hdev_name != NULL);

  len = strlen (hdev_name);

  for ( parti = phy_hdev->parti_list;
        parti != NULL;
        parti = parti->next_parti)
  {
    if (strncmp (hdev_name, parti->canonical_name, len) == 0)
    {
      ASSERT (parti->is_formatting == 0);
      parti->is_formatting = 1;
    }
  }
}



void
hotplug_unmark_all_partitions_as_formatting (
    struct hotplug_physical_device *phy_hdev, const char *hdev_name)
{
  struct hotplug_partition *parti;
  int len;

  ASSERT (phy_hdev != NULL);
  ASSERT (hdev_name != NULL);

  len = strlen (hdev_name);

  for ( parti = phy_hdev->parti_list;
        parti != NULL;
        parti = parti->next_parti)
  {
    if (strncmp (hdev_name, parti->canonical_name, len) == 0)
    {
      ASSERT (parti->is_formatting == 1);
      parti->is_formatting = 0;
    }
  }
}

int
hotplug_is_partition_ok_to_lock (struct hotplug_physical_device *phy_hdev,
                                 const char *hdev_name)
{
  struct hotplug_partition *parti;
  int len;

  ASSERT (phy_hdev != NULL);
  ASSERT (hdev_name != NULL);

  len = strlen (hdev_name);

  for (parti = phy_hdev->parti_list;
       parti != NULL;
       parti = parti->next_parti)
  {
    if (strncmp (hdev_name, parti->canonical_name, len) == 0)
    {
      if (parti->is_locked)
        return -1;
    }
  }

  return 0; /* This partiton can be locked. */
}

void
hotplug_lock_all_partitions (struct hotplug_physical_device *phy_hdev,
                             const char *hdev_name)
{
  struct hotplug_partition *parti;
  int len;

  ASSERT (phy_hdev != NULL);
  ASSERT (hdev_name != NULL);

  len = strlen (hdev_name);

  for ( parti = phy_hdev->parti_list;
        parti != NULL;
        parti = parti->next_parti)
  {
    if (strncmp (hdev_name, parti->canonical_name, len) == 0)
    {
      ASSERT (parti->is_locked == 0);
      parti->is_locked = 1;
    }
  }
}


void
hotplug_unlock_all_partitions (struct hotplug_physical_device *phy_hdev,
                               const char *hdev_name)
{
  struct hotplug_partition *parti;
  int len;

  ASSERT (phy_hdev != NULL);
  ASSERT (hdev_name != NULL);

  len = strlen (hdev_name);

  for ( parti = phy_hdev->parti_list;
        parti != NULL;
        parti = parti->next_parti)
  {
    if (strncmp (hdev_name, parti->canonical_name, len) == 0)
    {
      ASSERT (parti->is_locked == 1);
      parti->is_locked = 0;
    }
  }
}

#endif /* FEATURE_FS_HOTPLUG */
