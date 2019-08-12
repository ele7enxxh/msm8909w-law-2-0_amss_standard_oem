/***********************************************************************
 * fs_device_otp_flash.h
 *
 * EFS OTP (One-Time-Programmable) Device layer.
 * Copyright (C) 2011 Qualcomm Technologies, Inc.
 *
 * This file abstracts the OTP device to the EFS module.
 * The fs_use_otp variable can be set to debug the EFS OTP module using
 * the FOTA partition.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_device_otp_flash.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-08-17   rp    Create

===========================================================================*/

#ifndef __FS_DEVICE_OTP_FLASH_H__
#define __FS_DEVICE_OTP_FLASH_H__

#include "fs_config_i.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_OTP

  extern void fs_device_otp_flash_init (void);
  extern efs_device_t fs_device_otp_flash_open (uint32 which_device);
  extern int fs_device_otp_flash_close (efs_device_t base_dev);
  extern void fs_device_otp_flash_init_ops (efs_device_t base_dev);

#else /* FEATURE_EFS_OTP */

  static inline void
  fs_device_otp_flash_init (void)
  {
  }

  static inline efs_device_t
  fs_device_otp_flash_open (uint32 which_device)
  {
    (void) which_device;
    return NULL;
  }

  static inline int
  fs_device_otp_flash_close (efs_device_t base_dev)
  {
    (void) base_dev;
    return -1;
  }

  static inline void
  fs_device_otp_init_flash_ops (efs_device_t base_dev)
  {
    (void) base_dev;
  }

#endif /* FEATURE_EFS_OTP */

#endif /* not __FS_DEVICE_OTP_FLASH_H__ */
