/***********************************************************************
 * fs_blockdev_devnull.c
 *
 * Implement a small no-op device for blockdev
 * Copyright (C) 2009-2010,2014 QUALCOMM Technologies, Inc.
 *
 * It is sometimes useful to have a 'sink' device for benchmarking.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_devnull.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2010-09-30   wek   Remove device iovec interface.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"
#include "fs_blockdev_devnull_driver.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_blockdev.h"
#include "fs_blockdev_i.h"




#define DEVNULL_DEVICE_SIZE_IN_BLKS (1 * 1024 * 1024 * 1024 / 512) /* 1 GB */

static int
blockdev_devnull_get_size (struct block_device *bdev, void *handle,
                     uint32 *blocks, uint16 *bytes_per_block)
{
  (void) bdev; (void) handle;

  /* Good question.. how big is the NULL device?  */
  *bytes_per_block = 512;
  *blocks = DEVNULL_DEVICE_SIZE_IN_BLKS;
  return 0;
}

/* This device is never "present" so that we don't try to mount it. */
static int
blockdev_devnull_is_present (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 0; /* FALSE */
}

static int
blockdev_devnull_read (struct block_device *bdev, void *handle,
                 uint32 lba, unsigned char *buf, uint16 n_to_read)
{
  (void) bdev; (void) handle; (void) lba; (void) buf; (void) n_to_read;
  return 0; /* Sure, done */
}


static int
blockdev_devnull_write (struct block_device *bdev, void *handle,
                  uint32 lba, unsigned char *buf, uint16 n_to_write)
{
  (void) bdev; (void) handle; (void) lba; (void) buf; (void) n_to_write;
  return 0; /* No problem */
}

static int
blockdev_devnull_reliable_write (struct block_device *bdev, void *handle,
                           uint32 lba, uint32 *buf, uint32 num_sectors)
{
  (void) bdev; (void) handle; (void) lba; (void) buf; (void) num_sectors;
  return -1;
}
static int
blockdev_devnull_erase (struct block_device *bdev, void *handle, uint32 lba,
                  uint16 n_to_erase)
{
  (void) bdev; (void) handle; (void) lba; (void) n_to_erase;
  return 0;
}
static int
blockdev_devnull_get_device_info (struct block_device *bdev, void *handle,
                            struct block_device_info *info)
{
  (void) bdev; (void) handle; (void) info;
  return -1;
}

static int
blockdev_devnull_success_stub (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_devnull_open_stub (struct block_device *bdev, void **handle,
                            uint32 data)
{
  uint32 partition = data;

  (void) bdev; (void) handle;

  if (partition != 0)
    return -1;

  *handle = (void *)1;

  return 0;
}

static int
blockdev_devnull_fail_stub (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return -1;
}

static uint32
blockdev_devnull_get_entire_device_stub (struct block_device *bdev,
                                         void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_devnull_is_embedded_device (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return -1;
}

static int
blockdev_devnull_set_sps_end_point (struct block_device *bdev, void *handle,
                              void *param)
{
  (void) bdev; (void) handle; (void) param;
  return -1;
}

static int
blockdev_devnull_get_sps_end_point (struct block_device *bdev, void *handle,
                              void *param)
{
  (void) bdev; (void) handle; (void) param;
  return -1;
}

static int
blockdev_devnull_set_power_on_write_protection (struct block_device *bdev,
                      void *handle, uint32 start_block, uint32 num_blocks)
{
  (void) bdev; (void) handle; (void) start_block; (void) num_blocks;
  return -1;
}



static blockdev_ops devnull_ops =
{
  blockdev_devnull_open_stub,          /* open */
  blockdev_devnull_success_stub,       /* close */
  blockdev_devnull_read,               /* read */
  blockdev_devnull_write,              /* write */
  blockdev_devnull_write,              /* uwrite */
  blockdev_devnull_reliable_write,     /* reliable_write */
  blockdev_devnull_erase,              /* erase */
  blockdev_devnull_success_stub,       /* reset */
  blockdev_devnull_success_stub,       /* device_format */
  blockdev_devnull_is_present,         /* is_present */
  blockdev_devnull_get_size,           /* get_device_size */
  blockdev_devnull_get_device_info,    /* get_device_info */
  blockdev_devnull_get_entire_device_stub, /* get_entire_device_id */
  blockdev_devnull_fail_stub,           /* set_device_idle */
  blockdev_devnull_is_embedded_device,  /* is_embedded_device */
  blockdev_devnull_set_sps_end_point,
  blockdev_devnull_get_sps_end_point,
  blockdev_devnull_set_power_on_write_protection
};

blockdev_ops*
blockdev_devnull_get_ops (void)
{
  return &devnull_ops;
}


#endif /* FEATURE_FS_HOTPLUG */
