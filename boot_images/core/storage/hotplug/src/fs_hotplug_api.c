/***********************************************************************
 * fs_hotplug_api.c
 *
 * Hotplug APIs.
 * Copyright (C) 2009-2014 QUALCOMM Technologies, Inc.
 *
 * Hotplug APIs are implemented in this file.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_api.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2013-05-24   wek   Make GUID parameter as constant.
2012-01-17   wek   Remove unused utils header file.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-04-29   wek   Check iterator return value on open by type APIs.
2011-03-31   wek   Implement GPT/MBR partition table parser.
2010-10-25   wek   Fix hotplug_format to format an soft partition.
2010-10-21   dks   Fix hotplug APIs to work correctly for legacy hdevs.
2010-10-06   rp    Maintain legacy hotplug API behaviour.
2010-10-06   dks   Discover device when Hotplug polling is disabled.
2010-09-30   wek   Remove device iovec interface.
2010-08-27   rp    Make is_present() API to return 0 when there is no media.
2010-02-06   yog   Validate the input param for NULL check.
2010-02-24   rp    Added hotplug_open_device_by_partition_type API.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-12-10   rp    Do sector translation in write protect function.
2009-11-16   rp    Create

===========================================================================*/


#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_hotplug_i.h"
#include "assert.h"
#include "fs_errno.h"
#include "fs_err.h"
#include "fs_hfat_api.h"
#include "stringl/stringl.h"
#include <string.h>
#include <ctype.h>

/* Check for integer overflows */
#define INT_OVERFLOW(x,y) (x + y < x)


int
hotplug_is_valid_physical_hdev (struct hotplug_physical_device *phy_hdev)
{
  if (phy_hdev == NULL)
    return -1;

  switch (phy_hdev->dev_state)
  {
    case HPDEV_UNINITIALIZED:
    case HPDEV_UNUSABLE:
      return -1;

    case HPDEV_UNDISCOVERED:
    case HPDEV_LOCKED:
    case HPDEV_FORMATTING:
    case HPDEV_DISCOVERED:
    case HPDEV_MOUNTED:
    case HPDEV_UNMOUNTED:
      break;

    default:
      FS_ERR_FATAL ("invalid hpdev state : %d",phy_hdev->dev_state,0,0);
      break;
  }

  return 0;
}

int
hotplug_is_valid_hdev (struct hotplug_device *hdev)
{
  if (hdev == NULL)
    return -1;

  if (hdev->is_in_use != 1)
    return -1;

  if (hdev->phy_hdev == NULL)
    return -1;

  return 0;
}

int
hotplug_is_valid_hdev_for_read_write (struct hotplug_device *hdev)
{
  if (hotplug_is_valid_hdev (hdev) != 0)
    return -1;

  if (hotplug_is_valid_physical_hdev (hdev->phy_hdev) != 0)
    return -1;

  if (hotplug_is_valid_parti (hdev->parti) != 0)
    return -1;

  if (hdev->is_invalid)
    return -1;

  if (hdev->bdev_handle == NULL)
    return -1;

  return 0;
}


int
hotplug_is_valid_parti (struct hotplug_partition *parti)
{
  if (parti == NULL)
    return -1;

  if (parti->phy_hdev == NULL)
    return -1;

  return 0;
}

int
hotplug_validate_physical_hdev (struct hotplug_physical_device *phy_hdev)
{
  ASSERT (phy_hdev != NULL);
  return 0;
}

static unsigned int
char_to_num (char ch)
{
  if ((ch >= '0') && (ch <= '9')) return (ch - '0');
  if ((ch >= 'a') && (ch <= 'z')) return (ch - 'a' + 10);
  if ((ch >= 'A') && (ch <= 'Z')) return (ch - 'A' + 10);
  return 0;
}


static unsigned int
is_num (char ch)
{
  if (ch >= '0' && ch <= '9')
    return 1;
  return 0;
}

static uint32
canonical_to_phys_partition (char *path)
{
  int len = strlen (path);
  int i = 0;

  /* Loop through looking for hardware partition
   *                v
   * ie: //hdev/sdc1h12p2e3 */
  while (len >= 2)
  {
    if ( (path[i] == 'h' || path[i] == 'H') &&
         is_num(path[i + 1]) )
    {
      return char_to_num (path[i + 1]);
    }

    i++;
    len--;
  }

  /* Return 0 as default if none is found */
  return 0;
}


/*---------------------------------------------------------------------------
  Legacy hotplug_dev_open and hotplug_dev_close functions.
---------------------------------------------------------------------------*/
int
hotplug_dev_open (struct hotplug_device *hdev)
{
  int result = -1;

  if ( (hdev == NULL) || (hdev->phy_hdev == NULL) )
     return -1;

  if (hdev == hdev->phy_hdev->legacy_hdev)
    result = 0;

  return result;
}

int
hotplug_dev_close (struct hotplug_device *hdev)
{
  int result = -1;

  if ( (hdev == NULL) || (hdev->phy_hdev == NULL) )
     return -1;

  if (hdev == hdev->phy_hdev->legacy_hdev)
    result = 0;

  return result;
}


/*---------------------------------------------------------------------------
  hotplug_open_device
---------------------------------------------------------------------------*/
struct hotplug_device*
hotplug_open_device (const char *hdev_name)
{
  struct hotplug_partition *parti;
  struct hotplug_device *hdev = NULL;
  void *bdev_handle = NULL;
  uint16 bytes_per_block;
  uint32 phys_part_num;
  uint32 block_cnt;
  int result;

  /* Check for the hdev_name validity. */
  if (hdev_name == NULL)
    return NULL;

  /* Strip leading slash(es) */
  while (hdev_name[0] == '/' && hdev_name[1] == '/')
    hdev_name++;

  if (hdev_name[0] == '\0')          /* Name is required */
    return NULL;

  HOTPLUG_LOCK ();

  hotplug_task_discover_device (hdev_name);

  parti = hotplug_find_partition (hdev_name);
  if (parti == NULL)
    goto Error;

  if (hotplug_is_valid_parti (parti) != 0)
    goto Error;

  if (parti->is_locked || parti->is_formatting)
    goto Error;

  if (hotplug_is_valid_physical_hdev (parti->phy_hdev) != 0)
    goto Error;

  hdev = hotplug_hdev_alloc ();
  if (hdev == NULL)
    goto Error;

  /* Open the device and obtain handle. */
  phys_part_num = canonical_to_phys_partition (parti->canonical_name);
  result = blockdev_open (parti->phy_hdev->bdev, &bdev_handle,
                          phys_part_num);
  if (result != 0)
    goto Error;

  result = blockdev_get_device_size (parti->phy_hdev->bdev,
               bdev_handle, &block_cnt, &bytes_per_block);
  if (result != 0)
    goto Error;

  hotplug_reset_device_idle_counter (parti->phy_hdev);

  hdev->size_in_sectors = parti->size_in_sectors;
  hdev->one_sector_size = bytes_per_block;

  hotplug_add_hdev_to_phy_hdev (parti->phy_hdev, hdev);

  hdev->bdev_handle = bdev_handle;
  hdev->phy_hdev = parti->phy_hdev;
  hdev->parti = parti;
  hotplug_partition_ref (parti);

  goto Success;

Error:
  if (bdev_handle)
    blockdev_close (parti->phy_hdev->bdev, bdev_handle);
  if (hdev != NULL)
    hotplug_hdev_free (hdev);

Success:
  HOTPLUG_UNLOCK ();
  return hdev;
}


/*---------------------------------------------------------------------------
  hotplug_close_device
---------------------------------------------------------------------------*/
int
hotplug_close_device (struct hotplug_device *hdev)
{
  int result = 0;

  HOTPLUG_LOCK ();

  if (hotplug_is_valid_hdev (hdev) != 0)
  {
    HOTPLUG_UNLOCK ();
    return -1;
  }

  hotplug_remove_hdev_from_phy_hdev (hdev->phy_hdev, hdev);

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  result = blockdev_close (hdev->phy_hdev->bdev, hdev->bdev_handle);
  hdev->bdev_handle = NULL;

  hotplug_partition_unref (hdev->parti);

  hotplug_hdev_free (hdev);

  HOTPLUG_UNLOCK ();

  return result;
}


/*---------------------------------------------------------------------------
  hotplug_dev_read
---------------------------------------------------------------------------*/
int
hotplug_dev_read (struct hotplug_device *hdev, uint32 lba,
                  unsigned char *buf, uint16 block_cnt)
{
  if (hotplug_is_valid_hdev_for_read_write (hdev) != 0)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  if (INT_OVERFLOW (lba, block_cnt) ||
      lba + block_cnt > hdev->size_in_sectors)
  {
    return -FS_ERANGE;
  }

  return blockdev_read (hdev->phy_hdev->bdev, hdev->bdev_handle,
                        lba + hdev->parti->start_sector, buf, block_cnt);
}


/*---------------------------------------------------------------------------
  hotplug_dev_write
---------------------------------------------------------------------------*/
int
hotplug_dev_write (struct hotplug_device *hdev, uint32 lba,
                   unsigned char *buf, uint16 block_cnt)
{
  int rv;

  if (hotplug_is_valid_hdev_for_read_write (hdev) != 0)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  if (INT_OVERFLOW (lba, block_cnt) ||
      lba + block_cnt > hdev->size_in_sectors)
  {
    return -FS_ERANGE;
  }

  rv = blockdev_write (hdev->phy_hdev->bdev, hdev->bdev_handle,
                       lba + hdev->parti->start_sector, buf, block_cnt);
  return rv;
}


/*---------------------------------------------------------------------------
  hotplug_dev_reliable_write
---------------------------------------------------------------------------*/
int
hotplug_dev_reliable_write (struct hotplug_device *hdev, uint32 lba,
                            uint32 *buf, uint16 block_cnt)
{
  if (hotplug_is_valid_hdev_for_read_write (hdev) != 0)
    return -1;

  /* check for the buffer validity. */
  if (buf == NULL)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  if (INT_OVERFLOW (lba, block_cnt) ||
    lba + block_cnt > hdev->size_in_sectors)
  {
    return -FS_ERANGE;
  }

  return blockdev_reliable_write (hdev->phy_hdev->bdev,
    hdev->bdev_handle, lba + hdev->parti->start_sector, buf, block_cnt);
}

/*---------------------------------------------------------------------------
  hotplug_dev_write_udata
---------------------------------------------------------------------------*/
int
hotplug_dev_write_udata (struct hotplug_device *hdev, uint32 lba,
                         unsigned char *buf, uint16 block_cnt)
{
  if (hotplug_is_valid_hdev_for_read_write (hdev) != 0)
    return -1;

  /* check for the buffer validity. */
  if (buf == NULL)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  if (INT_OVERFLOW (lba, block_cnt) ||
      lba + block_cnt > hdev->size_in_sectors)
  {
    return -FS_ERANGE;
  }

  return blockdev_uwrite (hdev->phy_hdev->bdev, hdev->bdev_handle,
                          lba + hdev->parti->start_sector,
                          buf, block_cnt);
}

/*---------------------------------------------------------------------------
  hotplug_dev_erase
---------------------------------------------------------------------------*/
int
hotplug_dev_erase (struct hotplug_device *hdev, uint32 lba,
                   uint16 block_cnt)
{
  if (hotplug_is_valid_hdev (hdev) != 0)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  if (INT_OVERFLOW (lba, block_cnt) ||
      lba + block_cnt > hdev->size_in_sectors)
  {
    return -FS_ERANGE;
  }

  return blockdev_erase (hdev->phy_hdev->bdev, hdev->bdev_handle,
                         lba + hdev->parti->start_sector, block_cnt);
}

/*---------------------------------------------------------------------------
  hotplug_dev_reset
---------------------------------------------------------------------------*/
int
hotplug_dev_reset (struct hotplug_device *hdev)
{
  if (hotplug_is_valid_hdev_for_read_write (hdev) != 0)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  return blockdev_reset (hdev->phy_hdev->bdev, hdev->bdev_handle);
}

/*---------------------------------------------------------------------------
  hotplug_dev_is_present
---------------------------------------------------------------------------*/
int
hotplug_dev_is_present (struct hotplug_device *hdev)
{
  if (hotplug_is_valid_hdev (hdev) != 0)
    return 0;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  return blockdev_is_present (hdev->phy_hdev->bdev, hdev->bdev_handle);
}

/*---------------------------------------------------------------------------
  hotplug_dev_get_device_info
---------------------------------------------------------------------------*/
int
hotplug_dev_get_device_info (struct hotplug_device *hdev,
                             struct hotplug_device_info *dev_info)
{
  int rv;
  struct block_device_info bdev_info;

  if (hotplug_is_valid_hdev (hdev) != 0)
    return -1;

  if (hdev->bdev_handle == NULL)
    return -1;

  /* check for the hotplug device dev_info validity. */
  if (dev_info == NULL)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  /* Populate the device specific info. */
  rv = blockdev_get_device_info (hdev->phy_hdev->bdev,
                                 hdev->bdev_handle, &bdev_info);
  if (rv)
    return rv;

  /* Copy device info into hotplug info struct */
  dev_info->manufacturer_id = bdev_info.manufacturer_id;
  dev_info->oem_id = bdev_info.oem_id;
  dev_info->prod_rev = bdev_info.prod_rev;
  dev_info->prod_serial_num = bdev_info.prod_serial_num;
  strlcpy ((char *)dev_info->product_name, bdev_info.product_name,
              sizeof (dev_info->product_name));
  strlcpy ((char *)dev_info->manufactured_date, bdev_info.manufactured_date,
              sizeof (dev_info->manufactured_date));

  return 0;
}

/*---------------------------------------------------------------------------
  hotplug_dev_get_size
---------------------------------------------------------------------------*/
int
hotplug_dev_get_size (struct hotplug_device *hdev, uint32 *blocks,
                      uint16 *bytes_per_block)
{
  if (hotplug_is_valid_hdev (hdev) != 0)
    return -1;

  /* check for the input param blocks and bytes_per_block validity. */
  if ( (blocks == NULL) || (bytes_per_block == NULL) )
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  *blocks = hdev->size_in_sectors;
  *bytes_per_block = hdev->one_sector_size;

  return 0;
}

/*---------------------------------------------------------------------------
  hotplug_set_device_idle
---------------------------------------------------------------------------*/
int
hotplug_set_device_idle (struct hotplug_physical_device *phy_hdev)
{
  hotplug_validate_physical_hdev (phy_hdev);
  return blockdev_set_device_idle (phy_hdev->bdev, phy_hdev->bdev_handle);
}

/*---------------------------------------------------------------------------
  hotplug_dev_get_path
---------------------------------------------------------------------------*/
const char*
hotplug_dev_get_path (struct hotplug_device *hdev)
{
  if ( (hdev == NULL) || (hdev->phy_hdev == NULL) )
    return NULL;

  if (hdev == hdev->phy_hdev->legacy_hdev)
    return hotplug_legacy_hdev_get_path (hdev);

  if (hotplug_is_valid_hdev (hdev) != 0)
    return NULL;

  if (hotplug_is_valid_physical_hdev (hdev->phy_hdev) != 0)
    return NULL;

  return hdev->parti->canonical_name;
}

/*---------------------------------------------------------------------------
  hotplug_dev_get_state
---------------------------------------------------------------------------*/
int
hotplug_dev_get_state (struct hotplug_device *hdev,
                       struct hdev_state *state)
{
  if (state == NULL)
    return -1;

  if (hotplug_is_valid_hdev (hdev) != 0)
    return -1;

  memset (state, 0, sizeof (struct hdev_state));

  if (hdev == hdev->phy_hdev->legacy_hdev)
  {
    return hotplug_legacy_hdev_get_state (hdev, state);
  }

  if (hdev->parti)
  {
    state->is_present = 1;
    state->is_mounted = hdev->parti->is_mounted;
    state->is_formatting = hdev->parti->is_formatting;
    state->is_locked = hdev->parti->is_locked;
  }

  return 0;                     /* success */
}

/*---------------------------------------------------------------------------
  hotplug_dev_is_mounted
---------------------------------------------------------------------------*/
int
hotplug_dev_is_mounted (struct hotplug_device *hdev)
{
  int is_mounted = 0;

  if (hotplug_is_valid_hdev (hdev) != 0)
    return 0;

  if (hdev == hdev->phy_hdev->legacy_hdev)
  {
    return hotplug_legacy_hdev_is_mounted (hdev);
  }

  if (hdev->parti != NULL)
  {
    is_mounted = hdev->parti->is_mounted;
  }

  return is_mounted;
}


/*---------------------------------------------------------------------------
  hotplug_set_sps_end_point
---------------------------------------------------------------------------*/
int
hotplug_set_sps_end_point (struct hotplug_device *hdev, void *param)
{
  if (hotplug_is_valid_hdev_for_read_write (hdev) != 0)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  return blockdev_set_sps_end_point (hdev->phy_hdev->bdev, hdev->bdev_handle,
                                     param);
}

/*---------------------------------------------------------------------------
  hotplug_get_sps_end_point
---------------------------------------------------------------------------*/
int
hotplug_get_sps_end_point (struct hotplug_device *hdev, void *param)
{
  if (hotplug_is_valid_hdev_for_read_write (hdev) != 0)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  return blockdev_get_sps_end_point (hdev->phy_hdev->bdev, hdev->bdev_handle,
                                     param);
}

/*---------------------------------------------------------------------------
  hotplug_set_power_on_write_protection
---------------------------------------------------------------------------*/
int hotplug_set_power_on_write_protection (struct hotplug_device *hdev,
                                           uint32 lba, uint32 block_cnt)
{
  if (hotplug_is_valid_hdev_for_read_write (hdev) != 0)
    return -1;

  hotplug_reset_device_idle_counter (hdev->phy_hdev);

  if (block_cnt == 0)
    block_cnt = hdev->size_in_sectors;

  if (INT_OVERFLOW (lba, block_cnt) ||
      lba + block_cnt > hdev->size_in_sectors)
  {
    return -FS_ERANGE;
  }

  return blockdev_set_power_on_write_protection (hdev->phy_hdev->bdev,
                hdev->bdev_handle, lba + hdev->parti->start_sector, block_cnt);
}


/*---------------------------------------------------------------------------
  hotplug_format_dev
---------------------------------------------------------------------------*/
int
hotplug_format_dev (struct hotplug_device *hdev)
{
  int result = -1;

  HOTPLUG_LOCK ();

  if (hotplug_is_valid_hdev (hdev) != 0)
    goto Cleanup;

  if (hotplug_is_valid_physical_hdev (hdev->phy_hdev) != 0)
    goto Cleanup;

  if (hotplug_is_valid_parti (hdev->parti) != 0)
    goto Cleanup;

  if (hdev->parti->is_formatting)
    goto Cleanup;

  /* We don't have HFAT support to just format a partition, HFAT formats the
   * whole device by writing MBR at sector-0, so until we fix this lets not
   * allow formatting of internal-cards and while formatting the external-
   * cards, we shall format the whole device always.
   */
  if ( (hdev->parti->phy_hdev->is_embedded_device) &&
       (hdev->parti->phy_hdev->dev_type == HOTPLUG_TYPE_MMC) )
    goto Cleanup;

  result = hotplug_format_hdev (hdev);

Cleanup:
  HOTPLUG_UNLOCK ();
  return result;
}


/*---------------------------------------------------------------------------
  hotplug_format
---------------------------------------------------------------------------*/
int
hotplug_format (const char *hdev_name)
{
  struct hotplug_device *hdev;
  int result;

  if ( (hdev_name == NULL) || (strlen (hdev_name) == 0))
    return -1;

  hdev = hotplug_open_device (hdev_name);
  if (hdev == NULL)
    return -1;

  result = hotplug_format_dev (hdev);

  hotplug_close_device (hdev);

  return result;
}

/*---------------------------------------------------------------------------
  hotplug_lock_dev
---------------------------------------------------------------------------*/
int
hotplug_lock_dev (struct hotplug_device *hdev)
{
  int result = -1;

  HOTPLUG_LOCK ();

  if ( (hdev == NULL) || (hdev->phy_hdev == NULL) )
    goto Cleanup;

  if ( (hdev == hdev->phy_hdev->legacy_hdev) &&
       (hdev->phy_hdev->legacy_hdev->parti == NULL) )
  {
    result = hotplug_legacy_hdev_lock (hdev);
    goto Cleanup;
  }

  if (hotplug_is_valid_hdev (hdev) != 0)
    goto Cleanup;

  if (hotplug_is_valid_physical_hdev (hdev->phy_hdev) != 0)
    goto Cleanup;

  if (hotplug_is_valid_parti (hdev->parti) != 0)
    goto Cleanup;

  if (hdev->parti->is_locked || hdev->parti->is_formatting)
    goto Cleanup;

  result = hotplug_lock_hdev (hdev);

Cleanup:
  HOTPLUG_UNLOCK ();
  return result;
}

/*---------------------------------------------------------------------------
  hotplug_unlock_dev
----------------------------------------------------------------------------*/
int
hotplug_unlock_dev (struct hotplug_device *hdev)
{
  int result = -1;

  HOTPLUG_LOCK ();

  if ( (hdev == NULL) || (hdev->phy_hdev == NULL) )
    goto Cleanup;

  if ( (hdev == hdev->phy_hdev->legacy_hdev) &&
       (hdev->phy_hdev->legacy_hdev->parti == NULL) )
  {
    result = hotplug_legacy_hdev_unlock (hdev);
    goto Cleanup;
  }

  if (hotplug_is_valid_hdev (hdev) != 0)
    goto Cleanup;

  if (hotplug_is_valid_physical_hdev (hdev->phy_hdev) != 0)
    goto Cleanup;

  if (hotplug_is_valid_parti (hdev->parti) != 0)
    goto Cleanup;

  if (!hdev->parti->is_locked)
    goto Cleanup;

  if (hdev->parti->is_formatting)
    goto Cleanup;

  result = hotplug_unlock_hdev (hdev);

Cleanup:
  HOTPLUG_UNLOCK ();
  return result;
}


/*---------------------------------------------------------------------------
  hotplug_checkfat
---------------------------------------------------------------------------*/
int
hotplug_checkfat (const char *hdev_name, int mode, int *errors,
                  void (*callback) (void *), void *param)
{
  struct hotplug_partition *parti;
  struct hotplug_device *hdev;
  int result = -1;

  /* Check for the validity of hdev_name. */
  if (hdev_name == NULL)
    return -1;

  /* Strip leading slash(es) */
  while (hdev_name[0] == '/' && hdev_name[1] == '/')
    hdev_name++;

  if (hdev_name[0] == '\0')          /* Name is required */
    return -1;

  HOTPLUG_LOCK ();

  parti = hotplug_find_partition (hdev_name);
  if (hotplug_is_valid_parti (parti) != 0)
    goto Cleanup;

  if (hotplug_is_valid_physical_hdev (parti->phy_hdev) != 0)
    goto Cleanup;

  if (parti->phy_hdev->dev_state != HPDEV_MOUNTED)
    goto Cleanup;

  if (!hotplug_is_it_a_fat_partition (parti))
    goto Cleanup;

  if (!parti->is_mounted)
    goto Cleanup;

  if (parti->is_locked || parti->is_formatting)
    goto Cleanup;

  hdev = hotplug_open_device (parti->canonical_name);
  if (hdev == NULL)
    goto Cleanup;

  parti->is_locked = 1;

  /* Run checkfat */
  HOTPLUG_UNLOCK ();
  result = fs_hfat_checkfat (hdev, mode, errors, callback, param);
  HOTPLUG_LOCK ();

  hotplug_close_device (hdev);

  parti->is_locked = 0;

Cleanup:
  HOTPLUG_UNLOCK ();
  return result;
}


struct hotplug_device*
hotplug_open_device_by_partition_type (hotplug_dev_type dev_type,
       enum hotplug_iter_device_location dev_loc, uint32 partition_type)
{
  struct hotplug_device *hdev = NULL;
  struct hotplug_iter *hiter;
  struct hotplug_iter_record *hiter_rec;

  HOTPLUG_LOCK ();

  hiter = hotplug_iter_open (dev_type, HOTPLUG_ITER_SOFT_PARTITIONS, dev_loc);
  if (hiter == NULL)
    goto mbr_iter_done;

  hiter_rec = hotplug_iter_get_next (hiter);
  while (hiter_rec != NULL)
  {
    if (hiter->parti->partition_type == HOTPLUG_SOFT_MBR_PARTITION &&
        hiter_rec->boot_record_partition_type == partition_type)
    {
      hdev = hotplug_open_device (hiter_rec->hdev_name);
      break;
    }
    hiter_rec = hotplug_iter_get_next (hiter);
  }

  hotplug_iter_close (hiter);

mbr_iter_done:
  HOTPLUG_UNLOCK ();

  return hdev;
}

struct hotplug_device* hotplug_open_device_by_gpt_partition_type (
    hotplug_dev_type dev_type,
    enum hotplug_iter_device_location dev_loc,
    const struct hotplug_guid *guid_type)
{
  struct hotplug_device *hdev = NULL;
  struct hotplug_iter *hiter;
  struct hotplug_iter_record *hiter_rec;

  HOTPLUG_LOCK ();

  hiter = hotplug_iter_open (dev_type, HOTPLUG_ITER_SOFT_PARTITIONS, dev_loc);
  if (hiter == NULL)
    goto gpt_iter_done;

  hiter_rec = hotplug_iter_get_next (hiter);
  while (hiter_rec != NULL)
  {
    if (hiter->parti->partition_type == HOTPLUG_SOFT_GPT_PARTITION)
    {
      int result;
      result = memcmp (&hiter->parti->boot_record_partition_type.guid,
                       guid_type, sizeof (struct hotplug_guid));
      if (result == 0)
      {
        hdev = hotplug_open_device (hiter_rec->hdev_name);
        break;
      }
    }
    hiter_rec = hotplug_iter_get_next (hiter);
  }

  hotplug_iter_close (hiter);

gpt_iter_done:
  HOTPLUG_UNLOCK ();

  return hdev;
}

uint32
hotplug_get_physical_start_sector (struct hotplug_device *hdev)
{
  return hdev->parti->start_sector;
}

#endif /* FEATURE_FS_HOTPLUG */
