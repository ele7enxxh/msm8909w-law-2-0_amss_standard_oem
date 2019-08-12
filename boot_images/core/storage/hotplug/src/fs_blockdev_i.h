/***********************************************************************
 * fs_blockdev_i.h
 *
 * Internal header containing private definitions for the block device
 * module.
 * Copyright (C) 2009-2010,2012,2014 QUALCOMM Technologies, Inc.
 *
 * This header contains all the private structure definitions for the
 * block device module. Nothing defined here should be accessible to
 * external clients. Also all logic concerning blockdev flags
 * ie: BLOCKDEV_xxx should be in this file.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_i.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2012-03-20   wek   Cleanup customer header file inclusion.
2010-09-30   wek   Remove device iovec interface.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_I_H__
#define __FS_BLOCKDEV_I_H__

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "comdef.h"
#include "fs_blockdev.h"

/* All the proper flags are set accoring to hoplug external features */
#include "fs_hotplug_i.h"


#define CANARY 0x0D00CAFE

/* Operations for each block device */
typedef struct
{
  int    (*open)(struct block_device*, void**, uint32);
  int    (*close)(struct block_device*, void*);
  int    (*read)(struct block_device*, void*, uint32,
                 unsigned char*, uint16);
  int    (*write)(struct block_device *, void*, uint32,
                  unsigned char*, uint16);
  int    (*uwrite)(struct block_device*, void*, uint32,
                   unsigned char *, uint16);
  int    (*reliable_write)(struct block_device*,void*,uint32,uint32*,uint32);
  int    (*erase)(struct block_device*, void*, uint32, uint16);
  int    (*reset)(struct block_device*, void*);
  int    (*device_format)(struct block_device*, void*);
  int    (*is_present)(struct block_device*, void*);
  int    (*get_device_size)(struct block_device*, void*, uint32*, uint16*);
  int    (*get_device_info)(struct block_device*,void*,
                            struct block_device_info*);
  uint32 (*get_entire_device_id)(struct block_device*, void*);
  int    (*set_device_idle)(struct block_device *, void*);
  int    (*is_embedded_device)(struct block_device*, void*);
  int    (*set_sps_end_point)(struct block_device*, void*, void*);
  int    (*get_sps_end_point)(struct block_device*, void*, void*);
  int    (*set_power_on_write_protection)(struct block_device*, void*, uint32,
                                          uint32);
} blockdev_ops;

/* Structure containing all device specific data */
struct block_device
{
  const char            *name;          /* Name of this device              */
  blockdev_ops          *ops;           /* Device operations                */
  struct block_device   *next;          /* Pointer to the next device       */
  block_device_type     type;           /* Type of this block device        */
  int8                  hidden;         /* Skip this device when iterating  */
  uint16                device_id;      /* Unique id to represent dev       */
  void                  *device_data;   /* Internal device specific data    */
  uint32                canary;         /* Verify struct integrity          */
};

#endif /* FEATURE_FS_HOTPLUG */

#endif /* not __FS_BLOCKDEV_I_H__ */
