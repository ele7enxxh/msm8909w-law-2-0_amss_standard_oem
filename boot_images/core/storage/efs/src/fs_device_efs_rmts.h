/***********************************************************************
 * fs_device_efs_rmts.h
 *
 * EFS Remote Storage Device module.
 * Copyright (C) 2009,2010 Qualcomm Technologies, Inc.
 *
 * Device layer abstration for EFS on Remote MMC device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_device_efs_rmts.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-12-23   vr    Use handle in updated remotefs_get_shared_ram_address().
2010-10-12   vr    Change fs_iovec parameter from num_sector to size.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_DEVICE_EFS_RMTS_H__
#define __FS_DEVICE_EFS_RMTS_H__

#include "fs_rmts_config.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

extern void fs_device_efs_rmts_init (void);
extern efs_device_t fs_device_efs_rmts_open (uint32 which_device);
extern int fs_device_efs_rmts_close (efs_device_t base_dev);
void fs_device_efs_rmts_init_dev (efs_device_t base_dev, uint32 ram_size);


#else /* FEATURE_EFS_EFS2_ON_RMTS */

static inline void
fs_device_efs_rmts_init (void)
{
}

static inline efs_device_t
fs_device_efs_rmts_open (uint32 which_device)
{
  (void) which_device;
  return NULL;
}

static inline int
fs_device_efs_rmts_close (efs_device_t base_dev)
{
  (void) base_dev;
  return -1;
}

static inline void
fs_device_efs_rmts_init_dev (efs_device_t base_dev)
{
  (void) base_dev;
}

#endif /* FEATURE_EFS_EFS2_ON_RMTS */

#endif /* not __FS_DEVICE_EFS_RMTS_H__ */
