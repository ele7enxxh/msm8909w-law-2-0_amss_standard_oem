/***********************************************************************
 * fs_device_efs_mnand.c
 *
 * Device layer abstration for EFS on mNand device.
 * Copyright (C) 2008-2013 QUALCOMM Technologies, Inc.
 *
 * Device layer abstration for EFS on mNand device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_efs_mnand.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-01-18   wek   Allow multiple devices to be defined in one build.
2012-10-04   wek   Remove all err fatals from unimplemented dev functions.
2012-09-28   wek   Remove ERR_FATALs hit with diag dev info handler.
2012-05-16   nr    Make MNAND cluster/page size configurable at runtime.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2011-04-21   wek   Use the partition type to open the mnand hotplug device.
2010-11-08   wek   Remove impractical and unused paranoia module.
2010-10-15   wek   Add page size ops for all efs devices.
2010-10-07   yog   Avoid hotplug_lock_dev call before "/" gets mounted.
2010-10-01   wek   Rename mnand macros from clusters to page.
2010-09-30   wek   Remove device iovec interface.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2009-11-04   dks   Changed fs_device_efs_mnand_device_type
2009-05-15   ebb   Updated blockdev_open call with dummy argument.
2009-05-08   ebb   Moved to block_device interface.
2008-12-16   yg    Use get_size function to get the size params of device.
2008-12-15   rp    Remove reference to obsolete flash APIs.
2008-12-12   rp    Create

===========================================================================*/
#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

#include "fs_device_efs_mnand.h"
#include "fs_mnand_config.h"
#include "assert.h"
#include "fs_err.h"
#include "fs_sys_types.h"
#include <string.h>
#include "fs_util.h"
#include "fs_hotplug.h"
#include "fs_hotplug_priv_api.h"

#ifdef FS_UNIT_TEST
  #include "sdcc_frame.h"
#endif

#define FS_DEVICE_EFS_MNAND_MAX_DEVICES   (1)

/* Max MNAND instances to encapsulate the MNAND-device. */
static struct efs_device fs_base_mnand_device[FS_DEVICE_EFS_MNAND_MAX_DEVICES];
static uint32 mnand_device_blk_cnt;


void
fs_device_efs_mnand_init (void)
{
  memset (fs_base_mnand_device, 0, sizeof(fs_base_mnand_device));
}

efs_device_t
fs_device_efs_mnand_open (uint32 which_device)
{
  efs_device_t base_dev;
  struct hotplug_device *hdev;
  int result;
  uint16 sector_size;
  uint32 page_size = FS_MNAND_PAGE_SIZE;

  if (which_device >= FS_DEVICE_EFS_MNAND_MAX_DEVICES)
    FS_ERR_FATAL ("Max device limit exceeded...",0,0,0);

  hdev = hotplug_open_device_by_partition_type (HOTPLUG_TYPE_MMC,
            HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY, HOTPLUG_PARTITION_ID_MNAND);
  if (hdev == NULL)
  {
    return NULL;
  }

  /**
   * Since we try to lock device before mounting it as root, part of the code
   * inside this call tries to remove legacy names hotplug_legacy_names_remove
   * this would result in crash because root "/" mount-point is still not
   * created.
   * This change by *NOT* locking the device before mount is temporary. */
#if 0
  result = hotplug_lock_dev (hdev);
  if (result != 0)
  {
    return NULL;
  }
#endif
  result = hotplug_dev_get_size (hdev, &mnand_device_blk_cnt,
                                 &sector_size);
  if (result != 0)
  {
    (void) hotplug_close_device (hdev);
    return NULL;
  }

  base_dev = &fs_base_mnand_device[which_device];
  base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_MNAND;
  base_dev->priv.mnand_dev.hdev = hdev;
  fs_device_efs_mnand_init_ops (base_dev);

#ifdef FS_UNIT_TEST
{
  efs_device_t sim_dev;
  sim_dev = lookup_partition (EFS_PARTITION_NAME);
  ASSERT (sim_dev != NULL);
  ASSERT (sim_dev->efs_device_type == FS_DEVICE_TYPE_EFS_MNAND);
  sim_dev->priv.mnand_dev.hdev = hdev;
  page_size = sim_dev->priv.mnand_dev.dev_page_size;
}
#endif
  base_dev->priv.mnand_dev.dev_page_size = page_size;
  base_dev->priv.mnand_dev.sectors_per_page =
    page_size / FS_MNAND_SECTOR_SIZE;
  return base_dev;
}

int
fs_device_efs_mnand_close (efs_device_t base_dev)
{
  int result;

  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);

  result = hotplug_close_device (base_dev->priv.mnand_dev.hdev);
  ASSERT (result == 0);
  base_dev->priv.mnand_dev.hdev = NULL;
  return 0;
}

static int
fs_device_efs_mnand_read_sectors (efs_device_t base_dev, fs_sector_id sect,
                                  void *data, uint32 num_sectors)
{
  int result;

  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);
  result = hotplug_dev_read (base_dev->priv.mnand_dev.hdev, sect,
                          data, (uint16)num_sectors);
  if (result != 0)
    FS_ERR_FATAL ("hotplug_dev_read() failed...",0,0,0);

  return result;
}

static int
fs_device_efs_mnand_write_sectors (efs_device_t base_dev, fs_sector_id sect,
                                   void *data, uint32 num_sectors)
{
  int result;

  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);
  result = hotplug_dev_write (base_dev->priv.mnand_dev.hdev, sect,
                           data, (uint16)num_sectors);
  if (result != 0)
    FS_ERR_FATAL ("hotplug_dev_write() failed...",0,0,0);

  return result;
}


static int
fs_device_efs_mnand_reliable_write_sectors (efs_device_t base_dev,
                            fs_sector_id sect, void *data, uint32 num_sectors)
{
  int result;

  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);
  result = hotplug_dev_reliable_write (base_dev->priv.mnand_dev.hdev, sect,
                                       data, num_sectors);
  if (result != 0)
    FS_ERR_FATAL ("hotplug_dev_reliable_write() failed...",0,0,0);

  return result;
}


static int
fs_device_efs_mnand_read_page (efs_device_t base_dev, fs_page_id page,
                               void *data)
{
  int result;
  uint32 sectors_per_page = base_dev->priv.mnand_dev.sectors_per_page;

  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);
  result = hotplug_dev_read (base_dev->priv.mnand_dev.hdev, page,
                             data, sectors_per_page);
  if (result != 0)
    FS_ERR_FATAL ("hotplug_dev_read() failed...",0,0,0);

  return result;
}

static int
fs_device_efs_mnand_write_page (efs_device_t base_dev, fs_page_id page,
                                void *data)
{
  int result;
  uint32 sectors_per_page = base_dev->priv.mnand_dev.sectors_per_page;

  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);
  result = hotplug_dev_write (base_dev->priv.mnand_dev.hdev, page,
                              data, sectors_per_page);
  if (result != 0)
    FS_ERR_FATAL ("hotplug_dev_write() failed...",0,0,0);

  return result;
}

static uint32
fs_device_efs_mnand_block_count (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);
  return mnand_device_blk_cnt;
}

static uint32
fs_device_efs_mnand_block_size (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);
  return FS_MNAND_PAGES_PER_BLOCK;
}

static uint32
fs_device_efs_mnand_page_size (efs_device_t base_dev)
{
  ASSERT (base_dev->priv.mnand_dev.hdev != NULL);
  return base_dev->priv.mnand_dev.dev_page_size;
}


static uint32
fs_device_efs_mnand_device_type (efs_device_t base_dev)
{
  return base_dev->efs_device_type;
}

void
fs_device_efs_mnand_init_ops (efs_device_t base_dev)
{
  fs_device_init_invalid_ops (base_dev);

  base_dev->block_count = fs_device_efs_mnand_block_count;
  base_dev->block_size = fs_device_efs_mnand_block_size;
  base_dev->page_size = fs_device_efs_mnand_page_size;
  base_dev->device_type = fs_device_efs_mnand_device_type;
  base_dev->write_page = fs_device_efs_mnand_write_page;
  base_dev->read_page = fs_device_efs_mnand_read_page;
  base_dev->read_sectors = fs_device_efs_mnand_read_sectors;
  base_dev->write_sectors = fs_device_efs_mnand_write_sectors;
  base_dev->reliable_write_sectors =fs_device_efs_mnand_reliable_write_sectors;
}

#endif /* FEATURE_EFS_EFS2_ON_MNAND */
