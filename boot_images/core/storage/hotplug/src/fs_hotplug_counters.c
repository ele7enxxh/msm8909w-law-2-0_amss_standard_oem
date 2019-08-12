/***********************************************************************
 * fs_hotplug_counters.c
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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_counters.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-14   rp    Create

===========================================================================*/

#include "fs_hotplug_counters.h"
#include "fs_err.h"

#include <string.h>

struct hotplug_counters hotplug_counter = { 0, 0, 0, 0 };


void
hotplug_counters_init (void)
{
  memset (&hotplug_counter, 0, sizeof (hotplug_counter));
}

void
hotplug_counters_update (enum hotplug_bdev_op bdev_op, uint32 count)
{
  switch (bdev_op)
  {
    case HOTPLUG_BDEV_OP_READ:
      hotplug_counter.hotplug_read_sector_counter += count;
      break;

    case HOTPLUG_BDEV_OP_WRITE:
      hotplug_counter.hotplug_write_sector_counter += count;
      break;

    case HOTPLUG_BDEV_OP_RELIABLE_WRITE:
      hotplug_counter.hotplug_reliable_write_sector_counter += count;
      break;

    case HOTPLUG_BDEV_OP_ERASE:
      hotplug_counter.hotplug_erase_counter += count;
      break;

    default:
      break;
  }
}

