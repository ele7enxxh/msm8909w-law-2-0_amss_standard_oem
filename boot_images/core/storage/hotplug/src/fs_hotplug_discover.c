/***********************************************************************
 * fs_hotplug_discover.c
 *
 * Hotplug discovery module.
 * Copyright (C) 2009-2014 QUALCOMM Technologies, Inc.
 *
 * Discover hard/soft partitions inside an hotplug device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_discover.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-09-30   wek   Remove the feature that disables MBR checks.
2012-03-18   wek   Reduce hotplug size for bootloaders.
2012-01-17   wek   Remove unused utils header file.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-10-31   wek   Partition parser speed optimizations.
2011-10-25   nr    Fixed Klocwork errors.
2011-04-29   wek   Move max physical partitions to config file.
2011-03-31   wek   Implement GPT/MBR partition table parser.
2011-12-01   vr    Restore partition paths to Fix compilation issue.
2010-12-23   vr    Remove hardcoded EFS partition paths.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-12-15   wek   Added partition ID for ADSP.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include <stdio.h>

#include "fs_hotplug_parser.h"
#include "fs_hotplug_i.h"
#include "fs_err.h"
#include "fs_util.h"
#include "assert.h"
#include "stringl/stringl.h"

/* Indicates the first partition number, e.g. sdc0h0pX, X is the number */
#define HOTPLUG_FIRST_PARTITION_NUMBER  1
/* Indicates the first extended partition number, e.g. sdc0h0p3eX. */
#define HOTPLUG_FIRST_EXTENDED_NUMBER  1

/* Adds the entire device as a partition in the physical device link list.
 *
 * returns Valid partition pointer on success, NULL on error.
 *         Error can occur when:
 *         - There are no more partitions to allocate.
 *         - One of the block device operations failed for the physical device.
 */
static struct hotplug_partition *
hotplug_add_entire_device (struct hotplug_physical_device *phy_hdev)
{
  struct hotplug_partition *parti = NULL;
  uint32 entire_device_id;
  uint32 size_in_sectors = 0;
  uint16 bytes_per_block = 0;
  const char* name;
  int result;
  int close_result = -1;
  void *bdev_handle = NULL;
  struct block_device *bdev = phy_hdev->bdev;


  /* get the ID for the entire device */
  entire_device_id = blockdev_get_entire_device_id (bdev, NULL);
  result = blockdev_open (bdev, &bdev_handle, entire_device_id);

  if (result == 0)
  {
    result = blockdev_get_device_size (bdev, bdev_handle,
                          &size_in_sectors, &bytes_per_block);
    close_result = blockdev_close (bdev, bdev_handle);
  }

  if (result == 0 && close_result == 0)
    parti = hotplug_partition_alloc ();

  /* If no error, then we do have a valid device, populate the structure */
  if (parti != NULL)
  {
    uint32 copied_size;
    uint32 name_size = sizeof (parti->canonical_name);

    name = phy_hdev->canonical_name;
    copied_size = strlcpy (parti->canonical_name, name, name_size);
    if (copied_size > name_size)
    {
      FS_ERR_FATAL ("Name size too long %lu > %lu%d", copied_size,
                     name_size, 0);
    }

    parti->friendly_name = NULL;
    parti->partition_type = HOTPLUG_ENTIRE_DEVICE;
    parti->hard_partition_number = entire_device_id;

    parti->bootable = 0;
    memset (&parti->boot_record_partition_type, 0,
            sizeof (parti->boot_record_partition_type));
    parti->start_sector = 0;
    parti->size_in_sectors = size_in_sectors;

    parti->phy_hdev = phy_hdev;
    /* add it to the link list for this physical hdev */
    hotplug_add_partition (phy_hdev, parti);
  }

  return parti;
}

/* Adds a hard partition to a physical device.
 *
 * param parent_parti Partition of the parent, that is partitition structure
 *                    for the entire device.
 * param hard_partition_num Hard partition number.
 *
 * returns Pointer to the added partition, NULL on error. Error means could
 *         not allocate a partition structure.
 */
static struct hotplug_partition *
hotplug_add_hard_partition (struct hotplug_partition *parent_parti,
                            void *bdev_handle,
                            uint8 hard_partition_num)
{
  struct hotplug_partition *parti;
  uint32 size_in_sectors;
  uint16 bytes_per_block;
  int result;
  struct block_device *bdev = parent_parti->phy_hdev->bdev;

  result = blockdev_get_device_size (bdev, bdev_handle,
                                     &size_in_sectors, &bytes_per_block);
  if (result != 0)
    return NULL;

  if (size_in_sectors == 0 || bytes_per_block == 0)
    return NULL;

  parti = hotplug_partition_alloc ();
  /* If no more partitions structructures are available, return failure. */
  if (parti == NULL)
    return NULL;

  result = snprintf (parti->canonical_name, FS_HOTPLUG_NAME_MAX, "%sh%d",
                     parent_parti->canonical_name, hard_partition_num);
  /* Check if we copied everything successfully. */
  if (result >= FS_HOTPLUG_NAME_MAX || result < 0)
  {
    FS_ERR ("Too long string %d h%d %lu", result, hard_partition_num,
            strlen (parent_parti->canonical_name));
    hotplug_partition_unref (parti);
    return NULL;
  }

  parti->friendly_name = NULL;
  parti->partition_type = HOTPLUG_HARD_PARTITION;
  parti->hard_partition_number = hard_partition_num;

  parti->bootable = 0;
  memset (&parti->boot_record_partition_type, 0,
          sizeof (parti->boot_record_partition_type));

  parti->start_sector = 0;
  parti->size_in_sectors = size_in_sectors;

  parti->phy_hdev = parent_parti->phy_hdev;

  hotplug_add_partition (parti->phy_hdev, parti);

  return parti;
}

/* Adds a Soft GPT partition to the link list. */
static struct hotplug_partition *
hotplug_add_gpt_partition (const struct hotplug_gpt_entry *gpt_entry,
                           uint32 partition_num,
                           struct hotplug_partition *parent_parti)
{
  struct hotplug_partition *parti;
  int result;
  parti = hotplug_partition_alloc ();
  /* If no more partitions structructures are available, return failure. */
  if (parti == NULL)
    return NULL;

  parti->partition_type = HOTPLUG_SOFT_GPT_PARTITION;
  parti->hard_partition_number = parent_parti->hard_partition_number;

  /* Bit 2 in GPT partition entry attribute is bootable. */
  parti->bootable = (gpt_entry->attributes >> 2) & 0x01;
  fs_memscpy (&parti->boot_record_partition_type.guid,
              sizeof (parti->boot_record_partition_type.guid),
              &gpt_entry->partition_type_guid,
              sizeof (struct hotplug_guid));
  parti->start_sector = (uint32) gpt_entry->starting_lba;
  parti->size_in_sectors = 1 + ((uint32) (gpt_entry->ending_lba -
                                          gpt_entry->starting_lba));
  /* No friendly names for GPT partitions. */
  parti->friendly_name = NULL;
  result = snprintf (parti->canonical_name, FS_HOTPLUG_NAME_MAX, "%sp%lu",
                     parent_parti->canonical_name, partition_num);
  /* Check if we copied everything successfully. */
  if (result >= FS_HOTPLUG_NAME_MAX || result < 0)
  {
    FS_ERR ("Too long string %d p%lu %lu", result, partition_num,
            strlen (parent_parti->canonical_name));
    hotplug_partition_unref (parti);
    return NULL;
  }

  parti->phy_hdev = parent_parti->phy_hdev;
  hotplug_add_partition (parti->phy_hdev, parti);
  return parti;
}

static int
hotplug_try_gpt (struct hotplug_partition *hard_parti,
                 struct hotplug_sector_cache *data_cache)
{
  HOTPLUG_GPT_ITER *gpt_iter;
  struct hotplug_gpt_entry *gpt_entry;
  struct hotplug_partition *parti;
  uint32 parti_num = HOTPLUG_FIRST_PARTITION_NUMBER;
  uint32 parti_count = 0;

  /* Parse GPT partitions. */
  gpt_iter = hotplug_gpt_parse_open (data_cache);
  /* If no GPT was found return error. */
  if (gpt_iter == NULL)
    return -1;

  while ((gpt_entry = hotplug_gpt_parse_read (gpt_iter)) != NULL)
  {
    parti = hotplug_add_gpt_partition (gpt_entry, parti_num, hard_parti);
    if (parti == NULL)
      break;
    parti_num++; parti_count++;
    if (parti_count >= FS_HOTPLUG_MAX_SOFT_PARTITIONS)
      break;
  }
  hotplug_gpt_parse_close (gpt_iter);

  return 0;
}

static struct hotplug_partition *
hotplug_add_mbr_partition (struct hotplug_mbr_partition_data *mbr_entry,
                           uint32 partition_num, char ptype,
                           struct hotplug_partition *parent_parti)
{
  struct hotplug_partition *parti;
  int result;
  parti = hotplug_partition_alloc ();
  /* If no more partitions structructures are available, return failure. */
  if (parti == NULL)
    return NULL;

  parti->partition_type = HOTPLUG_SOFT_MBR_PARTITION;
  parti->hard_partition_number = parent_parti->hard_partition_number;

  parti->bootable = mbr_entry->bootable;
  parti->boot_record_partition_type.mbr = mbr_entry->partition_type;
  parti->start_sector = mbr_entry->start_lba;
  parti->size_in_sectors = mbr_entry->size_in_blocks;
  parti->friendly_name = NULL;
  result = snprintf (parti->canonical_name, FS_HOTPLUG_NAME_MAX, "%s%c%lu",
                     parent_parti->canonical_name, ptype, partition_num);
  /* Check if we copied everything successfully. */
  if (result >= FS_HOTPLUG_NAME_MAX || result < 0)
  {
    FS_ERR ("Too long string %d p%lu %lu", result, partition_num,
            strlen (parent_parti->canonical_name));
    hotplug_partition_unref (parti);
    return NULL;
  }

  parti->phy_hdev = parent_parti->phy_hdev;
  hotplug_add_partition (parti->phy_hdev, parti);
  return parti;
}

/* Adds a Soft MBR Primary partition to the link list. */
static struct hotplug_partition *
hotplug_add_mbr_pri_partition (struct hotplug_mbr_partition_data *mbr_entry,
                               uint32 parti_num,
                               struct hotplug_partition *parent_parti)
{
  return hotplug_add_mbr_partition (mbr_entry, parti_num, 'p', parent_parti);
}

/* Adds a Soft MBR Extended partition to the link list. */
static struct hotplug_partition *
hotplug_add_mbr_ext_partition (struct hotplug_mbr_partition_data *mbr_entry,
                               uint32 ext_num,
                               struct hotplug_partition *parent_parti)
{
  return hotplug_add_mbr_partition (mbr_entry, ext_num, 'e', parent_parti);
}

/* Adds a partition to indicate there is a FAT boot record for the entire
 * device. */
static struct hotplug_partition *
hotplug_add_fat_partition (uint32 partition_num,
                           struct hotplug_partition *parent_parti)
{
  struct hotplug_partition *parti;
  int result;
  parti = hotplug_partition_alloc ();
  /* If no more partitions structructures are available, return failure. */
  if (parti == NULL)
    return NULL;

  parti->partition_type = HOTPLUG_SOFT_FAT_PARTITION;
  parti->hard_partition_number = parent_parti->hard_partition_number;

  parti->bootable = 0;
  memset (&parti->boot_record_partition_type, 0,
          sizeof (parti->boot_record_partition_type));
  parti->start_sector = parent_parti->start_sector;
  parti->size_in_sectors = parent_parti->size_in_sectors;
  parti->friendly_name = NULL;
  result = snprintf (parti->canonical_name, FS_HOTPLUG_NAME_MAX, "%sp%lu",
                     parent_parti->canonical_name, partition_num);
  /* Check if we copied everything successfully. */
  if (result >= FS_HOTPLUG_NAME_MAX || result < 0)
  {
    FS_ERR ("Too long string %d p%lu %lu", result, partition_num,
            strlen (parent_parti->canonical_name));
    hotplug_partition_unref (parti);
    return NULL;
  }

  parti->phy_hdev = parent_parti->phy_hdev;
  hotplug_add_partition (parti->phy_hdev, parti);
  return parti;
}

static int
hotplug_try_mbr (struct hotplug_partition *hard_parti,
                 struct hotplug_sector_cache *data_cache)
{
  HOTPLUG_MBR_ITER *mbr_iter;
  struct hotplug_mbr_partition_data *mbr_entry;
  struct hotplug_partition *parti;
  uint32 parti_num = HOTPLUG_FIRST_PARTITION_NUMBER;
  uint32 parti_count = 0;

  mbr_iter = hotplug_mbr_parse_open (data_cache);

  /* If no MBR was found, return error. */
  if (mbr_iter == NULL)
    return -1;

  while ((mbr_entry = hotplug_mbr_read_primary (mbr_iter)) != NULL)
  {
    parti = hotplug_add_mbr_pri_partition (mbr_entry, parti_num,
                                           hard_parti);
    if (parti == NULL)
      break;

    parti_num++; parti_count++;

    /* If this is an extended partition, parse all the extended entries. */
    if (hotplug_mbr_check_extended (mbr_iter) == 0)
    {
      int ext_num = HOTPLUG_FIRST_EXTENDED_NUMBER;
      struct hotplug_partition *ext;

      while ((mbr_entry = hotplug_mbr_read_extended (mbr_iter)) != NULL)
      {
        if (parti_count >= FS_HOTPLUG_MAX_SOFT_PARTITIONS)
          break;
        ext = hotplug_add_mbr_ext_partition (mbr_entry, ext_num, parti);
        if (ext == NULL)
          break;
        ext_num++; parti_count++;
      }
    }

    if (parti_count >= FS_HOTPLUG_MAX_SOFT_PARTITIONS)
      break;
  }
  hotplug_mbr_parse_close (mbr_iter);

  return 0;
}

static int
hotplug_try_fat (struct hotplug_partition *hard_parti,
                 struct hotplug_sector_cache *data_cache)
{
  struct hotplug_partition *parti;
  uint32 parti_num = HOTPLUG_FIRST_PARTITION_NUMBER;

  /* If there is a Fat boot record, add it. */
  if (hotplug_fat_parse_check (data_cache) == 0)
  {
    parti = hotplug_add_fat_partition (parti_num, hard_parti);
    if (parti != NULL)
      return 0;
  }

  return -1;
}

int
hotplug_discover_device (struct hotplug_physical_device *phy_hdev)
{
  int is_embedded_device;
  uint32 hard_pnum;
  static struct hotplug_partition *entire_parti;

  is_embedded_device = blockdev_is_embedded_device (phy_hdev->bdev,
                                                    phy_hdev->bdev_handle);
  if (is_embedded_device == 1)
    phy_hdev->is_embedded_device = 1;
  else
    phy_hdev->is_embedded_device = 0;

  entire_parti = hotplug_add_entire_device (phy_hdev);
  if (entire_parti == NULL)
    return -1;

  /* Try to open each hard partition. */
  for (hard_pnum = 0; hard_pnum < FS_HOTPLUG_MAX_PHY_PARTITIONS ; hard_pnum++)
  {
    struct hotplug_partition *hard_parti;
    int result;
    void *bdev_handle = NULL;
    struct hotplug_sector_cache *cache;

    result = blockdev_open (phy_hdev->bdev, &bdev_handle, hard_pnum);
    /* If a hard partition fails to open, then it does not exist, skip it. */
    if (result != 0)
      continue;

    /* Found hard partition, add it */
    hard_parti = hotplug_add_hard_partition (entire_parti, bdev_handle,
                                             hard_pnum);

    cache = NULL;
    if (hard_parti != NULL)
      cache = hotplug_pparser_cache_init (phy_hdev->bdev, bdev_handle);

    if (cache != NULL)
    {
      /* If the hard partition exist check if it is partitioned. */
      result = hotplug_try_gpt (hard_parti, cache);

      if (result != 0)
        result = hotplug_try_mbr (hard_parti, cache);

      if (result != 0)
        result = hotplug_try_fat (hard_parti, cache);
    }

    /* We are done parsing all soft partitions, go to next hard partition */
    result = blockdev_close (phy_hdev->bdev, bdev_handle);
  }

  return 0;
}

#endif /* FEATURE_FS_HOTPLUG */
