/***********************************************************************
 * fs_blockdev_ftl.c
 *
 * Flash Translation Layer support for Hotplug
 * Copyright (C) 2009-2010,2012,2014 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_ftl.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2012-03-20   wek   Cleanup customer header file inclusion.
2010-09-30   wek   Remove device iovec interface.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_FTL

#include "fs_blockdev_i.h"
#include "fs_blockdev_ftl_driver.h"
#include "fs_ftl_cache.h"
#include "assert.h"
#include "fs_public.h"
#include "fs_ftl.h"
#include "fs_ftl_cache.h"
#include <string.h>



typedef struct
{
  int16                   driveno;
  ftl_cached_device_t     handle;
} ftl_device_data;


/* Private storage for ftl devices */
static ftl_device_data ftl_dev_data[] =
{
  {0, FTL_CACHED_NO_DEVICE},  /* driveno, FTL handle */
  {1, FTL_CACHED_NO_DEVICE}
};


static int
blockdev_ftl_get_size (struct block_device *bdev, void *handle,
                 uint32* pblk_cnt, uint16* ppg_size)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;

  (void) handle;

  if (FTL_CACHED_NO_DEVICE == fdev->handle)
    return -1;

  return ftl_cache_get_size (fdev->handle, pblk_cnt, ppg_size);
}


static int
blockdev_ftl_is_present (struct block_device *bdev, void *handle)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;
  (void) handle;
  return (FTL_CACHED_NO_DEVICE != fdev->handle);
}

/*
 * For Unit test, it's important to be able to 'reboot' the phone
 * and rescan the flash.  This function ensures that when blockdev
 * restarts, the FTL subsystem will also reinspect the flash
 * contents.
 */
static int
blockdev_ftl_reset (struct block_device *bdev, void *handle)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;

  (void) handle;

  /* Initialization is done here since reset is called on bootup before
   * any other blockdev functions is called. If initialization is not
   * done here, but post poned till our first poll (which is 10 secs
   * later), then if some module like USB mass storage wants to lock
   * the device for exclusive access before FTL is initialized it would
   * not work. Also we do not want to initialize FTL in lock function
   * since the task in which lock is done will be unknown task.
   * So to keep the things consistent and simple we do initialize
   * FTL in here which happens on bootup. */

  /* Make sure that the device is not open, if its open, close it. */
  ASSERT (fdev->handle == FTL_CACHED_NO_DEVICE);

  if (ftl_cache_init (fdev->driveno) != 0)
    return -1;

  fdev->handle = ftl_cache_open (fdev->driveno);
  if (fdev->handle == NULL)
    return -1;

  return 0;
}

static int
blockdev_ftl_read (struct block_device *bdev, void *handle, uint32 lba,
             unsigned char *buf, uint16 n_to_read)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;

  (void) handle;

  if (FTL_CACHED_NO_DEVICE == fdev->handle)
    return -1;

  return ftl_cache_read (fdev->handle, lba, buf, n_to_read);
}

static int
blockdev_ftl_write (struct block_device *bdev, void *handle, uint32 lba,
              unsigned char *buf, uint16 n_to_write)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;

  (void) handle;

  if (FTL_CACHED_NO_DEVICE == fdev->handle)
    return -1;

  return ftl_cache_write (fdev->handle, lba, buf, n_to_write);
}

static int
blockdev_ftl_close (struct block_device *bdev, void *handle)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;

  (void) handle;

  if (FTL_CACHED_NO_DEVICE == fdev->handle)
    return -1;

  ftl_cache_force_sync_all ();

  return 0;
}

/* Non-implemented stub functions */
static int
blockdev_ftl_success_stub (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_ftl_fail_stub (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return -1;
}

static int
blockdev_ftl_open_stub (struct block_device *bdev, void **handle, uint32 data)
{
  uint32 partition = data;

  (void) handle; (void) bdev;

  if (partition != 0)
    return -1;

  *handle = (void *)1;

  return 0;
}

static int
blockdev_ftl_reliable_write (struct block_device *bdev, void *handle,
                             uint32 lba, uint32 *buf, uint32 num_sectors)
{
  (void) bdev; (void) handle; (void) lba; (void) buf; (void) num_sectors;
  return -1;
}
static int
blockdev_ftl_erase_fail (struct block_device *bdev, void *handle,
                  uint32 lba, uint16 n_to_erase)
{
  (void) bdev; (void) handle; (void) lba; (void) n_to_erase;
  return -1;
}
static int
blockdev_ftl_get_device_info (struct block_device *bdev, void *handle,
                        struct block_device_info *info)
{
  int result;
  uint32 total_blocks;
  uint16 block_size;

  memset (info, 0, sizeof (struct block_device_info));

  result = blockdev_ftl_get_size (bdev, handle, &total_blocks, &block_size);
  if (result == 0)
  {
    info->card_size_in_sectors = total_blocks;
    info->bytes_per_sector = block_size;
  }

  return result;
}

static uint32
blockdev_ftl_get_entire_device_stub (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_ftl_is_embedded_device (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 1; /* FTL is always an embedded device. */
}


static int
blockdev_ftl_set_sps_end_point (struct block_device *bdev, void *handle,
                                void *param)
{
  (void) bdev; (void) handle; (void) param;
  return -1;
}

static int
blockdev_ftl_get_sps_end_point (struct block_device *bdev, void *handle,
                                void *param)
{
  (void) bdev; (void) handle; (void) param;
  return -1;
}

static int
blockdev_ftl_set_power_on_write_protection (struct block_device *bdev,
                  void *handle, uint32 start_block, uint32 num_blocks)
{
  (void) bdev; (void) handle; (void) start_block; (void) num_blocks;
  return -1;
}


static blockdev_ops ftl_ops =
{
  blockdev_ftl_open_stub,          /* open */
  blockdev_ftl_close,              /* close */
  blockdev_ftl_read,               /* read */
  blockdev_ftl_write,              /* write */
  blockdev_ftl_write,              /* uwrite */
  blockdev_ftl_reliable_write,     /* reliable_write */
  blockdev_ftl_erase_fail,         /* erase */
  blockdev_ftl_reset,              /* reset */
  blockdev_ftl_success_stub,       /* device_format */
  blockdev_ftl_is_present,         /* is_present */
  blockdev_ftl_get_size,           /* get_device_size */
  blockdev_ftl_get_device_info,    /* get_device_info */
  blockdev_ftl_get_entire_device_stub, /* get_entire_device_id */
  blockdev_ftl_fail_stub,          /* set_device_idle */
  blockdev_ftl_is_embedded_device, /* is_embedded_device */
  blockdev_ftl_set_sps_end_point,
  blockdev_ftl_get_sps_end_point,
  blockdev_ftl_set_power_on_write_protection
};


void
blockdev_blockdev_ftl_device_data_init (void)
{
  uint32 total_devices, i;

  total_devices = sizeof (ftl_dev_data) / sizeof (ftl_dev_data[0]);
  for (i=0; i<total_devices; ++i)
  {
    ftl_dev_data[i].handle = NULL;
  }
}

blockdev_ops*
blockdev_blockdev_ftl_get_ops (void)
{
  return &ftl_ops;
}

void*
blockdev_blockdev_ftl_get_device_data (uint32 idx)
{
  uint32 total_devices;

  total_devices = sizeof (ftl_dev_data) / sizeof (ftl_dev_data[0]);
  ASSERT (idx < total_devices);

  return &ftl_dev_data[idx];
}


#endif /* BLOCKDEV_FTL */
