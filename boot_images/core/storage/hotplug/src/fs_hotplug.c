/***********************************************************************
 * fs_hotplug.c
 *
 * Hotplug module.
 * Copyright (C) 2009-2014 QUALCOMM Technologies, Inc.
 *
 * Auto mount/umount hot-insertable devices in EFS2.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-06-12   wek   Bringup of hotplug for XBL.
2013-10-10   rp    Hotplug to poll only devices of interest.
2013-05-24   rp    Add UFS support to Hotplug.
2012-11-09   nr    Add dummy slot for rmts on simulator.
2012-08-14   wek   Allow calling hotplug_init multiple times and execute once.
2012-03-18   wek   Reduce hotplug size for bootloaders.
2012-03-14   rp    Seperate EFS and Hotplug counters.
2012-01-17   wek   Remove unused header files to compile for boot.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-03-15   wek   Add check for canonical/blockdev name length.
2010-10-11   vr    Klocwork Bug fixes
2010-10-06   rp    Don't make blockdev calls without initialzing first.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_osal.h"
#include "assert.h"
#include "string.h"
#include "stringl/stringl.h"

#include "fs_err.h"
#include "fs_public.h"
#include "fs_hotplug_i.h"
#include "fs_hotplug_counters.h"
#include "fs_blockdev.h"

#include "fs_hfat_api.h"


/* Critical section behind the HOTPLUG_LOCK. */
fs_os_crit_sect_type hotplug_crit_sect;


/* Hotplug devices */
int hotplug_total_devices;
struct hotplug_device hotplug_devices[FS_HOTPLUG_MAX_HDEVS];

#ifdef FEATURE_FS_HOTPLUG_SD
  #define HOTPLUG_NUM_PHYSICAL_SD 4
#else
  #define HOTPLUG_NUM_PHYSICAL_SD 0
#endif

#ifdef FEATURE_FS_HOTPLUG_UFS
  #ifdef FS_UNIT_TEST
    #define HOTPLUG_NUM_PHYSICAL_UFS 2
  #else
    #define HOTPLUG_NUM_PHYSICAL_UFS 1
  #endif
#else
  #define HOTPLUG_NUM_PHYSICAL_UFS 0
#endif

#ifdef FEATURE_FS_HOTPLUG_SD_CPRM
  #define HOTPLUG_NUM_PHYSICAL_SD_CPRM 1
#else
  #define HOTPLUG_NUM_PHYSICAL_SD_CPRM 0
#endif

#ifdef FEATURE_FS_HOTPLUG_FTL
  #define HOTPLUG_NUM_PHYSICAL_FTL 2
#else
  #define HOTPLUG_NUM_PHYSICAL_FTL 0
#endif

#ifdef FEATURE_FS_HOTPLUG_USB
  #define HOTPLUG_NUM_PHYSICAL_USB 8
#else
  #define HOTPLUG_NUM_PHYSICAL_USB 0
#endif

#ifdef FS_UNIT_TEST
  #define HOTPLUG_NUM_PHYSICAL_MNAND  1
  #define HOTPLUG_NUM_PHYSICAL_RMTS   1
#else
  #define HOTPLUG_NUM_PHYSICAL_MNAND  0
  #define HOTPLUG_NUM_PHYSICAL_RMTS   0
#endif

#define HOTPLUG_NUM_PHYSICAL_DEVNULL  1

/* Features control what are the devices we support. The total number of
 * physical devices will depend on what features are turned on. */
#define FS_HOTPLUG_MAX_PHYSICAL_DEVICES  \
  (HOTPLUG_NUM_PHYSICAL_SD + HOTPLUG_NUM_PHYSICAL_SD_CPRM + \
   HOTPLUG_NUM_PHYSICAL_FTL + HOTPLUG_NUM_PHYSICAL_USB + \
   HOTPLUG_NUM_PHYSICAL_MNAND + HOTPLUG_NUM_PHYSICAL_RMTS + \
   HOTPLUG_NUM_PHYSICAL_UFS + HOTPLUG_NUM_PHYSICAL_DEVNULL)

int hotplug_total_physical_devices;
struct hotplug_physical_device
            hotplug_physical_devices[FS_HOTPLUG_MAX_PHYSICAL_DEVICES];


/* Prototypes */
static void hotplug_init_device_lists (void);
static void hotplug_init_devices (void);
static void hotplug_init_physical_devices (void);
static hotplug_dev_type blockdev_to_hotplug_type (block_device_type type);

int hotplug_init_complete = 0;
int hotplug_init_no_poll_complete = 0;

void
hotplug_init (void)
{
  /* Debug-Check to ensure only one type of hotplug-init is used. */
  ASSERT (hotplug_init_no_poll_complete == 0);

  if (hotplug_init_complete)
  {
    return;
  }
  hotplug_init_complete = 1;


  HOTPLUG_LOCK_INIT ();
  hotplug_counters_init ();
  blockdev_init ();
  hotplug_init_device_lists ();
  hotplug_iter_init ();
  hotplug_events_init ();
  hotplug_poll_before_mounting_root ();
}

void
hotplug_init_no_poll (void)
{
  /* Debug-Check to ensure only one type of hotplug-init is used. */
  ASSERT (hotplug_init_complete == 0);

  if (hotplug_init_no_poll_complete)
  {
    return;
  }
  hotplug_init_no_poll_complete = 1;

  HOTPLUG_LOCK_INIT ();
  hotplug_counters_init ();
  blockdev_init ();
  hotplug_init_device_lists ();
  hotplug_iter_init ();
  hotplug_events_init ();
}

int
hotplug_poll_device (hotplug_dev_type dev_type,
                     enum hotplug_iter_device_location dev_location)
{
  if (dev_type <= HOTPLUG_TYPE_INVALID)
  {
    return -1;
  }

  hotplug_poll_before_mounting_root_helper (dev_type, dev_location);

  return 0;
}

static void
hotplug_init_device_lists (void)
{
  hotplug_init_devices ();
  hotplug_init_partitions ();
  hotplug_init_physical_devices ();
}

static void
hotplug_init_devices (void)
{
  hotplug_total_devices = 0;
  memset (hotplug_devices, 0, sizeof (hotplug_devices));
}

static void
hotplug_init_physical_devices (void)
{
  struct block_device *bdev;
  struct hotplug_physical_device *phy_hdev;
  block_device_type bdev_type;
  int i;
  const char *blockdev_name;

  hotplug_total_physical_devices = 0;
  memset (hotplug_physical_devices, 0, sizeof (hotplug_physical_devices));
  for ( i = 0; i < FS_HOTPLUG_MAX_PHYSICAL_DEVICES; ++i)
  {
    phy_hdev = &hotplug_physical_devices[i];
    phy_hdev->dev_state = HPDEV_UNUSABLE;
    hotplug_legacy_hdev_create (phy_hdev);
  }

  /* Go through all the blockdevices and construct physical device list. */
  bdev = blockdev_find_by_type (BLOCKDEV_TYPE_ALL, NULL);
  while (bdev != NULL)
  {
    uint32 result;
    phy_hdev = &hotplug_physical_devices[hotplug_total_physical_devices++];

    phy_hdev->dev_state = HPDEV_UNINITIALIZED;

    phy_hdev->bdev = bdev;
    bdev_type = blockdev_get_type (phy_hdev->bdev);
    phy_hdev->dev_type = blockdev_to_hotplug_type (bdev_type);

    blockdev_name = blockdev_get_name (phy_hdev->bdev);
    /* if valid name doesnt exist, Error out */
    if (blockdev_name == NULL)
    {
      FS_ERR_FATAL ("No valid name for blockdev type: %d", bdev_type, 0,0);
    }

    result = strlcpy (phy_hdev->canonical_name, blockdev_name,
                         sizeof (phy_hdev->canonical_name));
    if (result > sizeof (phy_hdev->canonical_name))
    {
      FS_ERR_FATAL ("Blockdev name too long %d > %d", result,
                    sizeof (phy_hdev->canonical_name),0);
    }

    /* We have not yet opened the block-device, so right now we can't
     * figure out if it is internal or external device. Lets not trust
     * this device and so lets assume that this is an external device.  */
    phy_hdev->is_embedded_device = 0;

    bdev = blockdev_find_by_type (BLOCKDEV_TYPE_ALL, bdev);
  }

}


/* Resolve internal block_device_type to advertised hotplug_dev_type */
static hotplug_dev_type
blockdev_to_hotplug_type (block_device_type type)
{
  hotplug_dev_type btype = HOTPLUG_TYPE_INVALID;

  switch (type)
  {
    case BLOCKDEV_TYPE_INVALID:
      btype = HOTPLUG_TYPE_INVALID;
      break;
    case BLOCKDEV_TYPE_MMC:
      btype = HOTPLUG_TYPE_MMC;
      break;
    case BLOCKDEV_TYPE_REMOTE:
      btype = HOTPLUG_TYPE_REMOTE;
      break;
    case BLOCKDEV_TYPE_USB_MS:
      btype = HOTPLUG_TYPE_USB_MS;
      break;
    case BLOCKDEV_TYPE_DEVNULL:
      btype = HOTPLUG_TYPE_DEVNULL;
      break;
    case BLOCKDEV_TYPE_FTL:
      btype = HOTPLUG_TYPE_FTL;
      break;
    case BLOCKDEV_TYPE_SD_CPRM:
      btype = HOTPLUG_TYPE_SD_CPRM;
      break;
    case BLOCKDEV_TYPE_UFS:
      btype = HOTPLUG_TYPE_UFS;
      break;
    case BLOCKDEV_TYPE_ALL:
      btype = HOTPLUG_TYPE_ALL;
      break;

    default:
      FS_ERR_FATAL ("Unexpected blockdev type: %d", type, 0,0);
      break;
  }
  return btype;
}

/* Will go through all the hdev's that have the supplied hdev_name, and
 * mark them as invalid. Also will skip the supplied skip_hdev. */
static void
hotplug_invalidate_hdev_names (struct hotplug_physical_device *phy_hdev,
                               const char *hdev_name,
                               struct hotplug_device *skip_hdev)
{
  struct hotplug_device *hdev;
  int len;

  len = strlen (hdev_name);

  for ( hdev = phy_hdev->hdev_list;
        hdev != NULL;
        hdev = hdev->next_hdev )
  {
    if (skip_hdev == hdev)
      continue;

    if (strncmp (hdev_name, hdev->parti->canonical_name, len) == 0)
      hdev->is_invalid = 1;
  }
}


static int
hotplug_format_helper (const char *hdev_name,
                       struct hotplug_physical_device *phy_hdev,
                       struct hotplug_device *hdev)
{
  int result;

  if (hotplug_is_partition_ok_to_lock (phy_hdev, hdev_name) != 0)
    return -1;

  hotplug_unmount_all_partitions (phy_hdev, hdev_name);
  hotplug_mark_all_partitions_as_formatting (phy_hdev, hdev_name);
  hotplug_invalidate_hdev_names (phy_hdev, hdev_name, hdev);
  hotplug_notify_events (hdev_name,phy_hdev,HOTPLUG_NOTIFY_EVENT_FORMAT_START);

  HOTPLUG_UNLOCK ();
  result = fs_hfat_format (hdev);
  HOTPLUG_LOCK ();

  hotplug_unmark_all_partitions_as_formatting (phy_hdev, hdev_name);
  hotplug_notify_events (hdev_name, phy_hdev,
                         HOTPLUG_NOTIFY_EVENT_FORMAT_COMPLETE);

  return result;
}


int
hotplug_format_hdev (struct hotplug_device *hdev)
{
  struct hotplug_physical_device *phy_hdev;
  int result;

  phy_hdev = hdev->phy_hdev;

  hotplug_change_physical_dev_state (phy_hdev, HPDEV_UNMOUNTED);
  hotplug_change_physical_dev_state (phy_hdev, HPDEV_FORMATTING);

  result = hotplug_format_helper (phy_hdev->canonical_name, phy_hdev, hdev);

  hotplug_legacy_hdev_close (phy_hdev);
  hotplug_free_all_partitions (phy_hdev);

  (void) blockdev_close (phy_hdev->bdev, phy_hdev->bdev_handle);
  phy_hdev->bdev_handle = NULL;

  hotplug_change_physical_dev_state (phy_hdev, HPDEV_UNDISCOVERED);
  hotplug_force_poll_now ();

  return result;
}


int
hotplug_lock_hdev (struct hotplug_device *hdev)
{
  struct hotplug_physical_device *phy_hdev;
  char *hdev_name;

  phy_hdev = hdev->phy_hdev;
  hdev_name = hdev->parti->canonical_name;

  if (hotplug_is_partition_ok_to_lock (phy_hdev, hdev_name) != 0)
    return -1;

  hotplug_unmount_all_partitions (phy_hdev, hdev_name);
  hotplug_lock_all_partitions (phy_hdev, hdev_name);
  hotplug_invalidate_hdev_names (phy_hdev, hdev_name, hdev);

  /* Check if we are locking the entire device. */
  if (strcmp (hdev->parti->canonical_name,
              hdev->phy_hdev->canonical_name) == 0)
  {
    hotplug_change_physical_dev_state (phy_hdev, HPDEV_LOCKED);
    hotplug_notify_events (phy_hdev->canonical_name, phy_hdev,
                           HOTPLUG_NOTIFY_EVENT_CARD_LOCKED);
  }

  return 0;
}


int
hotplug_unlock_entire_device (struct hotplug_device *hdev)
{
  struct hotplug_physical_device *phy_hdev;

  phy_hdev = hdev->phy_hdev;

  hotplug_legacy_hdev_close (phy_hdev);
  hotplug_free_all_partitions (phy_hdev);

  (void) blockdev_close (phy_hdev->bdev, phy_hdev->bdev_handle);
  phy_hdev->bdev_handle = NULL;

  hotplug_change_physical_dev_state (phy_hdev, HPDEV_UNDISCOVERED);
  hotplug_force_poll_now ();

  hotplug_notify_events (phy_hdev->canonical_name, phy_hdev,
                         HOTPLUG_NOTIFY_EVENT_CARD_UNLOCKED);

  return 0;
}

static int
hotplug_unlock_partition (struct hotplug_partition *parti)
{
  hotplug_mount_all_partitions (parti->phy_hdev, parti->canonical_name);
  return 0;
}

int
hotplug_unlock_hdev (struct hotplug_device *hdev)
{
  struct hotplug_physical_device *phy_hdev;
  char *hdev_name;

  phy_hdev = hdev->phy_hdev;
  hdev_name = hdev->parti->canonical_name;

  hotplug_unlock_all_partitions (phy_hdev, hdev_name);

  /* Check if we are locking the entire device. */
  if (strcmp (hdev->parti->canonical_name,
              hdev->phy_hdev->canonical_name) == 0)
  {
    return hotplug_unlock_entire_device (hdev);
  }
  else
  {
    return hotplug_unlock_partition (hdev->parti);
  }
}


#endif /* FEATURE_FS_HOTPLUG  */
