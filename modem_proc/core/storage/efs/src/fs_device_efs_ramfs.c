/***********************************************************************
 * fs_device_efs_ramfs.c
 *
 * Device layer abstration for EFS on RAM device.
 * Copyright (C) 2010, 2012-2013 QUALCOMM Technologies, Inc.
 *
 * Device layer abstration for EFS on RAM device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_efs_ramfs.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-08-19   rp    Support persistence of RAM-FS.
2013-01-18   wek   Allow multiple devices to be defined in one build.
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2010-12-06   rp    Removed memset on the RAMFS Arrray.
2010-05-07   rp    Create

===========================================================================*/


#include "fs_ramfs_config.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_RAMFS

#include "fs_device_efs_ramfs.h"
#include "assert.h"
#include "fs_err.h"
#include "fs_sys_types.h"
#include <string.h>

#ifdef FS_UNIT_TEST
  #include "utdevice.h"
#endif

#define FS_DEVICE_EFS_RAMFS_NAME          "RamFS"
#define FS_DEVICE_EFS_RAMFS_MAKER_ID      0xcafe
#define FS_DEVICE_EFS_RAMFS_DEVICE_ID     0x10

static struct efs_device fs_ramfs_device;
uint8 fs_ramfs_data[FS_RAMFS_SIZE];

void
fs_device_efs_ramfs_init (void)
{
  memset (&fs_ramfs_device, 0, sizeof(fs_ramfs_device));
}

efs_device_t
fs_device_efs_ramfs_open (uint32 which_device)
{
  efs_device_t base_dev;
  uint8 *ramfs_buffer = NULL;
  uint32 ramfs_size = 0;
  uint32 page_size = FS_RAMFS_PAGE_SIZE_IN_BYTES;

  (void) which_device;

  base_dev = &fs_ramfs_device;
  base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_RAMFS;
  ramfs_buffer = (uint8 *)fs_ramfs_data;
  ramfs_size = sizeof (fs_ramfs_data);

  /* For simulator builds, use the device provided by framework. */
#ifdef FS_UNIT_TEST
  {
    efs_device_t framework_dev;

    framework_dev = lookup_partition (EFS_PARTITION_NAME);
    ASSERT (framework_dev != NULL);
    if (framework_dev->efs_device_type != FS_DEVICE_TYPE_EFS_RAMFS)
      return NULL;
    ramfs_buffer = framework_dev->priv.ramfs_dev.ramfs_buffer;
    ramfs_size = framework_dev->priv.ramfs_dev.ramfs_size;
    page_size = framework_dev->priv.ramfs_dev.dev_page_size;
  }
#endif

  /* Don't memset on the RAM-FS array, many targets show interest in
   * backing up this array in CMM scripts and hence make this RAM-FS
   * file-system as a non-volatile EFS. */

  base_dev->priv.ramfs_dev.ramfs_buffer = ramfs_buffer;
  base_dev->priv.ramfs_dev.ramfs_size = ramfs_size;
  base_dev->priv.ramfs_dev.dev_page_size = page_size;

  fs_device_efs_ramfs_init_ops (base_dev);

  return base_dev;
}

int
fs_device_efs_ramfs_close (efs_device_t base_dev)
{
  base_dev->priv.ramfs_dev.ramfs_buffer = NULL;
  base_dev->priv.ramfs_dev.ramfs_size = 0;
  return 0;
}

static char*
fs_device_ramfs_device_name (efs_device_t base_dev)
{
  (void) base_dev;
  return FS_DEVICE_EFS_RAMFS_NAME;
}

static int
fs_device_ramfs_device_maker_id (efs_device_t base_dev)
{
  (void) base_dev;
  return FS_DEVICE_EFS_RAMFS_MAKER_ID;
}

static int
fs_device_ramfs_device_id (efs_device_t base_dev)
{
  (void) base_dev;
  return FS_DEVICE_EFS_RAMFS_DEVICE_ID;
}

static uint32
fs_device_ramfs_block_count (efs_device_t base_dev)
{
  return (base_dev->priv.ramfs_dev.ramfs_size /
          base_dev->priv.ramfs_dev.dev_page_size);
}

static uint32
fs_device_ramfs_block_size (efs_device_t base_dev)
{
  (void) base_dev;
  return 1; /* One page per block. */
}

static uint32
fs_device_ramfs_page_size (efs_device_t base_dev)
{
  return base_dev->priv.ramfs_dev.dev_page_size;
}

static uint32
fs_device_ramfs_total_page_size (efs_device_t base_dev)
{
  /* There is no difference between page size and total page size in ramfs */
  return base_dev->priv.ramfs_dev.dev_page_size;
}


void
fs_device_efs_ramfs_init_ops (efs_device_t base_dev)
{
  fs_device_init_invalid_ops (base_dev);
  base_dev->device_name = fs_device_ramfs_device_name;
  base_dev->device_maker_id = fs_device_ramfs_device_maker_id;
  base_dev->device_id = fs_device_ramfs_device_id;
  base_dev->block_count = fs_device_ramfs_block_count;
  base_dev->block_size = fs_device_ramfs_block_size;
  base_dev->page_size = fs_device_ramfs_page_size;
  base_dev->total_page_size = fs_device_ramfs_total_page_size;
}

#endif /* FEATURE_EFS_EFS2_ON_RAM */
