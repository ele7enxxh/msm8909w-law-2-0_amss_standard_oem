/***********************************************************************
 * fs_hotplug_counters.h
 *
 * This file holds the counters to track Hotplug operations.
 * Copyright (C) 2012 Qualcomm Technologies, Inc.
 *
 * This file exposes the counters to track the read/write/erase operations
 * on the block-devices.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/inc/fs_hotplug_counters.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-14   rp   Create

===========================================================================*/

#ifndef __FS_HOTPLUG_COUNTERS_H__
#define __FS_HOTPLUG_COUNTERS_H__

#include "fs_hotplug_config_i.h"

struct hotplug_counters
{
  uint32 hotplug_read_sector_counter;
  uint32 hotplug_write_sector_counter;
  uint32 hotplug_reliable_write_sector_counter;
  uint32 hotplug_erase_counter;
};

extern struct hotplug_counters hotplug_counter;

/* Type of Blockdev operations. */
enum hotplug_bdev_op
{
  HOTPLUG_BDEV_OP_READ,
  HOTPLUG_BDEV_OP_WRITE,
  HOTPLUG_BDEV_OP_RELIABLE_WRITE,
  HOTPLUG_BDEV_OP_ERASE,
};


void hotplug_counters_init (void);
void hotplug_counters_update (enum hotplug_bdev_op bdev_op, uint32 count);


#endif /* not __FS_HOTPLUG_COUNTERS_H__ */
