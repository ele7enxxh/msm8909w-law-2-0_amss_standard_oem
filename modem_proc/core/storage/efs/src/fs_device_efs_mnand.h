/***********************************************************************
 * fs_device_efs_mnand.h
 *
 * Device layer abstration for EFS on mNand device.
 * Copyright (C) 2008 QUALCOMM Technologies, Inc.
 *
 * Device layer abstration for EFS on mNand device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_efs_mnand.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_DEVICE_EFS_MNAND_H__
#define __FS_DEVICE_EFS_MNAND_H__

#include "fs_config.h"
#include "fs_device.h"
#include "fs_sys_types.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

extern void fs_device_efs_mnand_init (void);
extern efs_device_t fs_device_efs_mnand_open (uint32 which_device);
extern int fs_device_efs_mnand_close (efs_device_t base_dev);
extern void fs_device_efs_mnand_init_ops (efs_device_t base_dev);

#else /* FEATURE_EFS_EFS2_ON_MNAND */

static inline void
fs_device_efs_mnand_init (void)
{
}

static inline efs_device_t
fs_device_efs_mnand_open (uint32 which_device)
{
  (void) which_device;
  return NULL;
}

static inline int
fs_device_efs_mnand_close (efs_device_t base_dev)
{
  (void) base_dev;
  return -1;
}

static inline void
fs_device_efs_mnand_init_ops (efs_device_t base_dev)
{
  (void) base_dev;
}


#endif /* FEATURE_EFS_EFS2_ON_MNAND */

#endif /* not __FS_DEVICE_EFS_MNAND_H__ */
