/***********************************************************************
 * fs_blockdev.c
 *
 * Block device abstraction layer.
 * Copyright (C) 2009-2010, 2012, 2014 QUALCOMM Technologies, Inc.
 *
 * This file abstracts all block devices using a common interface. If
 * a new block device is to be added a driver must be written to implement
 * these common functions. The driver should then be added to the array
 * blockdev_driver_list.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2012-03-14   rp    Seperate EFS and Hotplug counters.
2010-09-30   wek   Remove device iovec interface.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-12-23   vr    Return NULL if device is invalid in blockdev_get_name
2009-12-10   rp    Do sector translation in write protect function.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_blockdev.h"
#include "fs_blockdev_i.h"
#include "fs_blockdev_driver.h"
#include "fs_hotplug_counters.h"

#include <string.h>
#include "assert.h"



/* Used to verify if the supplied block device is valid */
#define IS_INVALID_BDEV(dev)  (dev == NULL || dev->canary != CANARY)

/* This Blk dev ID will not match any internal blk dev ID.
 * None of the clients should really be using this Device ID, since
 * the offset could be changing and the clients should never be aware
 * of the offset. */
#define INVALID_BLK_DEV_ID    0xFFFFU;

/* Pointer to the head of all managed block devices */
static struct block_device *blockdev_head;

void
blockdev_init (void)
{
  int i;
  uint16 dev_id = 0;
  uint32 num_drivers;


  blockdev_head = NULL;

  /* Step through list and initialize all drivers. Each driver will
   * add their device nodes to the linked list passed in. At the end
   * of enumeration this should contain all possible block devices
   * in the system. We walk through the list backwards so that the linked list
   * gets created in same order as the drivers are declared */
  num_drivers = blockdev_driver_get_total_devices ();
  for (i = num_drivers - 2; i >= 0; i--)
  {
    blockdev_driver *driver = blockdev_driver_list[i];

    /* Let driver initialize and add to device list */
    driver->init (&blockdev_head, &dev_id);
  }
}


struct block_device *
blockdev_find_by_path (char *path)
{
  struct block_device *dev;

  for (dev = blockdev_head; dev != NULL; dev = dev->next)
  {
    if (IS_INVALID_BDEV (dev))
      return NULL;

    if (strcmp (dev->name, path) == 0)
      return dev;
  }

  /* Couldn't find the device */
  return NULL;
}

/* This function assumes all devices of the same type are contiguous
 * in the linked list pointed to by blockdev_head. Any hidden block
 * devices will be skipped */
struct block_device *
blockdev_find_by_type (block_device_type type, struct block_device *last)
{
  struct block_device *cur;

  /* If last = NULL start from the beginning */
  if (last == NULL)
    cur = blockdev_head;
  else
    cur = last->next;

  /* Iterate through device list */
  while (cur != NULL)
  {
    if (IS_INVALID_BDEV (cur))
      return NULL;

    /* Does type match and not hidden? */
    if ((cur->type == type || type == BLOCKDEV_TYPE_ALL) &&
        !cur->hidden)
      return cur;

    /* Skip to next device */
    cur = cur->next;
  }

  /* At the end of the list */
  return NULL;

}


int
blockdev_hide_device (struct block_device *dev)
{
  int was_hidden;

  if (IS_INVALID_BDEV (dev))
    return -1;

  was_hidden = dev->hidden;
  dev->hidden = 1;

  return was_hidden;
}

int
blockdev_unhide_device (struct block_device *dev)
{
  int was_hidden;

  if (IS_INVALID_BDEV (dev))
      return -1;

  was_hidden = dev->hidden;
  dev->hidden = 0;
  return was_hidden;
}



const char*
blockdev_get_name (struct block_device *dev)
{
  if (IS_INVALID_BDEV (dev))
    return NULL;

  return dev->name;
}

block_device_type
blockdev_get_type (struct block_device *dev)
{
  if (IS_INVALID_BDEV (dev))
    return BLOCKDEV_TYPE_INVALID;

  return dev->type;
}

/* Call the underlying driver function for anything device related */
int
blockdev_open (struct block_device *bdev, void **handle, uint32 data)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->open (bdev, handle, data);
}

int
blockdev_close (struct block_device *bdev, void *handle)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->close (bdev, handle);
}

int
blockdev_read (struct block_device *bdev, void *handle, uint32 lba,
               unsigned char *buf, uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  hotplug_counters_update (HOTPLUG_BDEV_OP_READ, block_cnt);
  return bdev->ops->read (bdev, handle, lba, buf, block_cnt);
}

int
blockdev_write (struct block_device *bdev, void *handle, uint32 lba,
                unsigned char *buf, uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  hotplug_counters_update (HOTPLUG_BDEV_OP_WRITE, block_cnt);
  return bdev->ops->write (bdev, handle, lba, buf, block_cnt);
}

int
blockdev_reliable_write (struct block_device *bdev, void *handle, uint32 lba,
                         uint32 *buf, uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  hotplug_counters_update (HOTPLUG_BDEV_OP_RELIABLE_WRITE, block_cnt);
  return bdev->ops->reliable_write (bdev, handle, lba, buf, block_cnt);
}

int
blockdev_uwrite (struct block_device *bdev, void *handle, uint32 lba,
                 unsigned char *buf, uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  hotplug_counters_update (HOTPLUG_BDEV_OP_WRITE, block_cnt);
  return bdev->ops->uwrite (bdev, handle, lba, buf, block_cnt);
}

int
blockdev_erase (struct block_device *bdev, void *handle, uint32 lba,
                uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  hotplug_counters_update (HOTPLUG_BDEV_OP_ERASE, block_cnt);
  return bdev->ops->erase (bdev, handle, lba, block_cnt);
}

int
blockdev_reset (struct block_device *bdev, void *handle)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->reset (bdev, handle);
}

int
blockdev_is_present (struct block_device *bdev, void *handle)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->is_present (bdev, handle);
}

int
blockdev_get_device_info (struct block_device *bdev, void *handle,
                          struct block_device_info *dev_info)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->get_device_info (bdev, handle, dev_info);
}

int
blockdev_device_format (struct block_device *bdev, void *handle)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->device_format (bdev, handle);
}

int
blockdev_get_device_size (struct block_device *bdev, void *handle,
                          uint32 *blocks, uint16 *bytes_per_block)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->get_device_size (bdev, handle, blocks, bytes_per_block);
}

int
blockdev_set_device_idle (struct block_device *bdev, void *handle)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->set_device_idle (bdev, handle);
}


uint16
blockdev_get_device_id (struct block_device *bdev)
{
  if (IS_INVALID_BDEV (bdev))
    return INVALID_BLK_DEV_ID;

  return bdev->device_id;
}

uint32
blockdev_get_entire_device_id (struct block_device *bdev, void *handle)
{
  if (IS_INVALID_BDEV (bdev))
    return 0;

  return bdev->ops->get_entire_device_id (bdev, handle);
}

int
blockdev_is_embedded_device (struct block_device *bdev, void *handle)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->is_embedded_device (bdev, handle);
}

int
blockdev_set_sps_end_point (struct block_device *bdev,void *handle,void *param)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->set_sps_end_point (bdev, handle, param);
}

int
blockdev_get_sps_end_point (struct block_device *bdev,void *handle,void *param)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->get_sps_end_point (bdev, handle, param);
}

int blockdev_set_power_on_write_protection (struct block_device *bdev,
                  void *handle, uint32 lba, uint32 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->set_power_on_write_protection (bdev, handle, lba,
                                                   block_cnt);
}



#endif /* FEATURE_FS_HOTPLUG */

