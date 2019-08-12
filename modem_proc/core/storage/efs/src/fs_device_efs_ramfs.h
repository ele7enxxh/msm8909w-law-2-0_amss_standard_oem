/***********************************************************************
 * fs_device_efs_ramfs.h
 *
 * Device layer abstration for EFS on RAM device.
 * Copyright (C) 2010 QUALCOMM Technologies, Inc.
 *
 * Device layer abstration for EFS on RAM device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_efs_ramfs.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-05-07   rp    Create

===========================================================================*/

#ifndef __FS_DEVICE_EFS_RAMFS_H__
#define __FS_DEVICE_EFS_RAMFS_H__

#include "fs_ramfs_config.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_RAMFS

#include "fs_sys_types.h"

extern void fs_device_efs_ramfs_init (void);
extern efs_device_t fs_device_efs_ramfs_open (uint32 which_device);
extern int fs_device_efs_ramfs_close (efs_device_t base_dev);
extern void fs_device_efs_ramfs_init_ops (efs_device_t base_dev);

#else /* !(FEATURE_EFS_EFS2_ON_RAMFS) */

static inline void
fs_device_efs_ramfs_init (void)
{
}

static inline efs_device_t
fs_device_efs_ramfs_open (uint32 which_device)
{
  (void) which_device;
  return NULL;
}

static inline int
fs_device_efs_ramfs_close (efs_device_t base_dev)
{
  (void) base_dev;
  return -1;
}

static inline void
fs_device_efs_ramfs_init_ops (efs_device_t base_dev)
{
  (void) base_dev;
}

#endif /* FEATURE_EFS_EFS2_ON_RAMFS */


#endif /* not __FS_DEVICE_EFS_RAMFS_H__ */
