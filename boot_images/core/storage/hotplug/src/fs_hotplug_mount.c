/***********************************************************************
 * fs_hotplug_mount.c
 *
 * Hotplug Mount module.
 * Copyright (C) 2009-2012,2014 QUALCOMM Technologies, Inc.
 *
 * Hotplug Mount helper functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_mount.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-06-12   wek   Bringup of hotplug for XBL.
2012-05-02   dks   Make fs_pm_init free of flash writes and erases.
2012-07-30   nr    Fix GPT based FAT mount during boot.
2011-11-03   dks   Make Hotplug independent of EFS.
2011-05-01   wek   Comply with legacy behavior for MBR parsing.
2011-03-31   wek   Implement GPT/MBR partition table parser.
2010-10-06   rp    Create legacy names after mounting all partitions.
2010-03-05   vr    Add support for QEFS2 in BOOT
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_hotplug.h"
#include "fs_hotplug_i.h"
#include <string.h>

#ifndef FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS

#include "fs_public.h"
#include "fs_errno.h"
#include "fs_err.h"
/* This is directory in EFS2, inside which hotplug will create mountpoints. */
#define HOTPLUG_MOUNT_DIR_NAME                "/hdev/"

/* prototypes. */
static int hotplug_force_directory (const char *dirname);


void
hotplug_mount_init (void)
{
#ifndef FEATURE_EFS_ROMFS
  {
    struct fs_stat buf;
    int result;

    result = efs_stat (HOTPLUG_MOUNT_DIR_NAME, &buf);
    if (result == 0 && buf.st_size > 2)
    {
      (void) efs_deltree (HOTPLUG_MOUNT_DIR_NAME);
    }
  }
#endif

  if (hotplug_force_directory (HOTPLUG_MOUNT_DIR_NAME) != 0 &&
      efs_errno != EEXIST)
  {
    FS_ERR_FATAL ("hotplug mount dir create failed.",0,0,0);
  }
}


/* To ensure that no process creates files in EFS2 under the hotplug
 * mountpoints, we mount each of them to the 'nodev' filesystem until
 * a device is available.   This should remain the default state until
 * something useable is mounted.
 *
 * This function must succeed, or something is very wrong. */
void
hotplug_mount_nodev (const char *mname)
{
  if (hotplug_force_directory (mname) != 0)
  {
    FS_ERR_FATAL ("Could not prepare hotplug directory %s", mname, 0, 0);
  }

  if (efs_mount ("", mname, "nodev", 0, NULL) != 0)
  {
    FS_ERR_FATAL ("Could not mount %s as NODEV", mname, 0, 0);
  }
}

/*
 * Unmount the NODEV fs that was holding this inactive directory
 * This is necessary so that we can remount a different filesystem
 * there when ready.
 */
void
hotplug_umount_nodev (const char *mname)
{
  if (efs_umount (mname))  /* Should not fail */
  {
    FS_ERR_FATAL ("Failed to unmount NODEV from %s!\n\r", mname,0,0);
  }
}


static int
hotplug_fat_mount (struct hotplug_partition *parti)
{
  const char *hdev_name;

  hdev_name = parti->canonical_name;

  /* Create a directory under root to mount the device */
  (void) hotplug_force_directory (hdev_name);

  /* Now mount the FAT filesystem there */
  if (efs_mount ("", hdev_name, "extfs", 0, hdev_name) != 0)
  {
    (void ) efs_rmdir (hdev_name);
    return -1;
  }

  return 0;
}

static int
hotplug_fat_umount (struct hotplug_partition *parti)
{
  if (efs_umount (parti->canonical_name))  /* Should not fail */
    return -1;

  /* Remove directory that device was mounted under */
  if (efs_rmdir (parti->canonical_name) != 0)
  {
    FS_ERR_FATAL ("Hotplug error: rmdir",0,0,0);
  }

  return 0;
}

/* Ensure that a directory of this name exists or is created */
static int
hotplug_force_directory (const char *dirname)
{
  /* Kill any regular file that stands in our way. */
  (void) efs_unlink (dirname);

  /* Try to make the new directory.
   * An existing directory of the same name is okay.
   * We will just mount over it, hiding the contents. */
  if ((efs_mkdir (dirname, 0755) != 0)
      && (efs_errno != EEXIST))
  {
    /* Failed to make the directory.  Admit defeat. */
    return -1;
  }
  else
  {
    return 0;                   /* Directory good */
  }
}

int
hotplug_poll_and_mount_first_fat (hotplug_dev_type dev_type,
                                  enum hotplug_iter_device_location dev_loc)
{
  struct hotplug_iter *hiter;
  struct hotplug_iter_record *iter_rec;
  int result = -1;

  if (dev_type <= HOTPLUG_TYPE_INVALID)
  {
    return -1;
  }

  /* Init EFS, if not already done. */
  result = efs_boot_init_romfs ();
  if (result != 0)
  {
    FS_ERR_FATAL ("%d, efs_boot_init_romfs failed.", result, 0, 0);
  }

  /* Poll the device, if not already done.  */
  result = hotplug_poll_device (dev_type, dev_loc);
  if (result != 0)
  {
    FS_ERR_FATAL ("%d, hotplug_poll_device failed.", result, 0, 0);
  }

  result = -1;

  hiter = hotplug_iter_open (dev_type, HOTPLUG_ITER_FAT_PARTITIONS, dev_loc);
  if (hiter != NULL)
  {
    iter_rec = hotplug_iter_get_next (hiter);

    while (iter_rec != NULL)
    {
      result = efs_mount ("", "/mmc1", "extfs", 0,
                          (void*)iter_rec->hdev_name);
      if (result == 0)
      {
        break;
      }
      iter_rec = hotplug_iter_get_next (hiter);
    }

    hotplug_iter_close (hiter);
  }

  return result;
}


#else /* FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS */

void
hotplug_mount_init (void)
{
  return;
}

void
hotplug_mount_nodev (const char *mname)
{
  (void) mname;
  return;
}

void
hotplug_umount_nodev (const char *mname)
{
  (void) mname;
  return;
}

static int
hotplug_fat_mount (struct hotplug_partition *parti)
{
  (void) parti;
  return -1;
}

static int
hotplug_fat_umount (struct hotplug_partition *parti)
{
  (void) parti;
  return -1;
}

int
hotplug_poll_and_mount_first_fat (hotplug_dev_type dev_type,
                                  enum hotplug_iter_device_location dev_loc)
{
  (void) dev_type;
  (void) dev_loc;
  /* Mounting not supported with EFS is disabled. */
  return -1;
}


#endif /* !FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS */


static const struct hotplug_guid GUID_DATA_PARTITION =
{0xEBD0A0A2, 0xB9E5, 0x4433, {0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7}};

static int
hotplug_should_mount_this_partition (struct hotplug_partition *parti)
{
  (void) parti;
  return 1;
}

int
hotplug_is_gpt_data_partition (struct hotplug_partition *parti)
{
  if (parti->partition_type == HOTPLUG_SOFT_GPT_PARTITION)
  {
    int result;
    result = memcmp (&parti->boot_record_partition_type.guid,
                     &GUID_DATA_PARTITION, sizeof (struct hotplug_guid));
    if (result == 0)
      return 1;
  }

  return 0;
}

int
hotplug_is_mbr_fat_partition (struct hotplug_partition *parti)
{
  if (parti->partition_type == HOTPLUG_SOFT_MBR_PARTITION)
  {
    switch (parti->boot_record_partition_type.mbr)
    {
      /* All possible fat partition types */
    case 0x01:
    case 0x04:
    case 0x06:
    case 0x0B:
    case 0x0C:
    case 0x0E:
      return 1;

    default:
      break;
    }
  }

  return 0;
}


int
hotplug_is_it_a_fat_partition (struct hotplug_partition *parti)
{
  switch (parti->partition_type)
  {
  case HOTPLUG_SOFT_GPT_PARTITION:
    return hotplug_is_gpt_data_partition (parti);

  case HOTPLUG_SOFT_MBR_PARTITION:
    return hotplug_is_mbr_fat_partition (parti);

  case HOTPLUG_SOFT_FAT_PARTITION:
    return 1;

  case HOTPLUG_ENTIRE_DEVICE:
  case HOTPLUG_HARD_PARTITION:
  default:
    break;
  }

  return 0;
}

int
hotplug_partition_mount_helper (struct hotplug_partition *parti)
{
  if (parti->is_mounted)
    return -1;

  if (hotplug_should_mount_this_partition (parti) != 1)
    return -1;

  /* Mount if there is either a gpt data partition or a FAT partition in
   * MBR or a FAT is found instead of MBR. */

  if (hotplug_is_gpt_data_partition (parti) == 0 &&
      hotplug_is_mbr_fat_partition (parti) == 0 &&
      parti->partition_type != HOTPLUG_SOFT_FAT_PARTITION)
    return -1;

  if (hotplug_fat_mount (parti) == 0)
  {
    parti->is_mounted = 1;
    hotplug_notify_events (parti->canonical_name, parti->phy_hdev,
                           HOTPLUG_NOTIFY_EVENT_MOUNTED);
    return 0;
  }

  hotplug_notify_events (parti->canonical_name, parti->phy_hdev,
                         HOTPLUG_NOTIFY_EVENT_MOUNT_FAILED);


  return -1;
}

int
hotplug_partition_unmount_helper (struct hotplug_partition *parti)
{
  if (!parti->is_mounted)
    return -1;

  if (hotplug_is_it_a_fat_partition (parti) != 1)
    return -1;

  if (hotplug_fat_umount (parti) == 0)
  {
    parti->is_mounted = 0;
    hotplug_notify_events (parti->canonical_name, parti->phy_hdev,
                           HOTPLUG_NOTIFY_EVENT_UNMOUNTED);
    return 0;
  }


  return -1;
}


#endif /* FEATURE_FS_HOTPLUG */

