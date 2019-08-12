/***********************************************************************
 * fs_otp.h
 *
 * EFS OTP (One-Time-Programmable) module.
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * OTP is One-Time-Programmable partition in Nand/Nor flash memory.
 * Nand/Nor flash parts have a partition called OTP (One-Time-Programmable)
 * partition. This partition is usually small (like 5 to 128 flash pages)
 * and these pages can be written exactly once and can never be erased.
 *
 * This file mounts a OTP filesystem and maintains this file system
 * on the OTP partition on the flash device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_otp.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-10-18   rp    Fix compiler warning due to missing inline definition.
2011-08-17   rp    Create

===========================================================================*/

#ifndef __FS_OTP_H__
#define __FS_OTP_H__

#include "fs_otp_config.h"
#include "comdef.h"

#ifdef FEATURE_EFS_OTP

  void fs_otp_init (void);

#else

  static inline void
  fs_otp_init (void)
  {
  }

#endif /* FEATURE_EFS_OTP */


#endif /* not __FS_OTP_H__ */
