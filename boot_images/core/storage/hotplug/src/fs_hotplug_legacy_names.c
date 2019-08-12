/***********************************************************************
 * fs_hotplug_legacy_names.c
 *
 * Support for the previously assumed hotplug device names.
 * Copyright (C) 2009-2012 Qualcomm Technologies, Inc.
 *
 * Before hotplug supported partitions, the following hotplug device paths
 * were assumed by the hotplug clients
 *
 * "/mmc1"    ->    First FAT partiton in the external slot 0.
 * "/mmc2"    ->    First FAT partiton in the external slot 1.
 * "/mmc3"    ->    First FAT partiton in the external slot 2.
 * "/mmc4"    ->    First FAT partiton in the external slot 3.
 * "/mmc1_p"  ->    First FAT partiton in the external slot 0.
 * "/ftl1"    ->    First FAT partiton in the internal FTL-1 partition.
 * "/ftl2"    ->    First FAT partiton in the internal FTL-2 partition.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_legacy_names.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-18   wek   Reduce hotplug size for bootloaders.
2011-11-03   dks   Make Hotplug independent of EFS.
2011-04-05   dks   Fix hotplug notify event sequence.
2010-08-26   rp    Added legacy name for the USB devices.
2010-04-15   yog   Added legacy name entry for /hdev/sdc3 and /hdev/sdc4.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_hotplug_i.h"

#ifndef FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS

#include "fs_err.h"
#include "fs_public.h"
#include "fs_errno.h"

#include <string.h>

struct hotplug_legacy_names
{
  const char canonical_name[15];
  const char legacy_name[15];
  int is_embedded_device;
};

struct hotplug_legacy_names legacy_names[] =
{
  {"/hdev/sdc1",     "/mmc1",    0},
  {"/hdev/sdc2",     "/mmc2",    0},
  {"/hdev/sdc3",     "/mmc3",    0},
  {"/hdev/sdc4",     "/mmc4",    0},
  {"/hdev/sdc1_p",   "/mmc1_p",  0},
  {"/hdev/ftl1",     "/ftl1",    1},
  {"/hdev/ftl2",     "/ftl2",    1},
  {"/hdev/usb1",     "/usb1",    0},
  {"/hdev/usb2",     "/usb2",    0},
  {"/hdev/usb3",     "/usb3",    0},
  {"/hdev/usb4",     "/usb4",    0},
  {"/hdev/usb5",     "/usb5",    0},
  {"/hdev/usb6",     "/usb6",    0},
  {"/hdev/usb7",     "/usb7",    0},
  {"/hdev/usb8",     "/usb8",    0}
};

#define TOTAL_LEGACY_NAMES (sizeof (legacy_names) / sizeof (legacy_names[0]))


void
hotplug_legacy_names_init (void)
{
  uint32 i;

  for (i = 0; i < TOTAL_LEGACY_NAMES; ++i)
  {
    hotplug_mount_nodev (legacy_names[i].legacy_name);
  }
}

void
hotplug_legacy_names_create (struct hotplug_physical_device *phy_hdev)
{
  uint32 i;
  struct hotplug_partition *parti;
  int found_fat_partition = 0;

  for (i = 0; i < TOTAL_LEGACY_NAMES; ++i)
  {
    if (legacy_names[i].is_embedded_device != phy_hdev->is_embedded_device)
      continue; /* Not out guy, so check with the next */

    if (strcmp (legacy_names[i].canonical_name,
                phy_hdev->canonical_name ) == 0)
      break;
  }

  if (i == TOTAL_LEGACY_NAMES)
    return;

  /* locate the first FAT partition. */
  parti = phy_hdev->parti_list;
  while (parti != NULL)
  {
    if (hotplug_is_it_a_fat_partition (parti))
    {
      found_fat_partition = 1;
      break;
    }

    parti = parti->next_parti;
  }

  if (parti == NULL)
    parti = phy_hdev->parti_list; /* No FAT found, so use entire device.*/

  if (found_fat_partition)
  {
    /* We found a FAT partition, so hookup the legacy-name with EFS2 */
    hotplug_umount_nodev (legacy_names[i].legacy_name);
    /* Force/Create the symlink. */
    (void) efs_unlink (legacy_names[i].legacy_name);
    (void) efs_rmdir (legacy_names[i].legacy_name);
    if (efs_symlink (parti->canonical_name,
                     legacy_names[i].legacy_name) != 0)
    {
      FS_ERR_FATAL ("error creating legacy names",0,0,0);
    }
  }
  /* Associate the legacy-name as friendly name to this partition. */
  parti->friendly_name = legacy_names[i].legacy_name;

}

void
hotplug_legacy_names_remove (struct hotplug_physical_device *phy_hdev)
{
  uint32 i;
  int result;
  struct fs_stat stat_buf;
  struct hotplug_partition *parti;

  for (i = 0; i < TOTAL_LEGACY_NAMES; ++i)
  {
    if (strcmp (legacy_names[i].canonical_name, phy_hdev->canonical_name) == 0)
    {
      /* Remove the friendly name associations with the partition. */
      parti = phy_hdev->parti_list;
      while (parti != NULL)
      {
        if ( (parti->friendly_name) &&
             (strcmp (parti->friendly_name,
                      legacy_names[i].legacy_name) == 0) )
          {
            parti->friendly_name = NULL;
          }

        parti = parti->next_parti;
      }

      result = efs_stat (legacy_names[i].legacy_name, &stat_buf);
      if ( (result != 0) && (efs_errno == ENODEV) )
        break;

      /* Put it back as nodev. */
      hotplug_mount_nodev (legacy_names[i].legacy_name);
      break;
    }
  }
}


const char*
hotplug_legacy_names_get_path (const char *canonical_name)
{
  unsigned int i;
  const char *legacy_name = NULL;

  for (i = 0; i < TOTAL_LEGACY_NAMES; ++i)
  {
    if (strcmp (legacy_names[i].canonical_name, canonical_name ) == 0)
    {
      legacy_name = (const char *)legacy_names[i].legacy_name;
      break;
    }
  }

  return legacy_name;
}

#endif /* !FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS */

#endif /* FEATURE_FS_HOTPLUG */

