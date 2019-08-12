/***********************************************************************
 * fs_otp_config.h
 *
 * EFS OTP (One-Time-Programmable) module configuration file.
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * This file configures EFS OTP module related things.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_otp_config.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-08-17   rp    Create

===========================================================================*/

#ifndef __FS_OTP_CONFIG_H__
#define __FS_OTP_CONFIG_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_OTP

#ifdef FEATURE_EFS_OTP_PARTITION_NAME
  #define FS_OTP_PARTITION_NAME               FEATURE_EFS_OTP_PARTITION_NAME
#else
  #define FS_OTP_PARTITION_NAME               "0:OTP"
#endif

#ifdef FEATURE_EFS_OTP_MAX_PAGE_SIZE
  #define FS_OTP_MAX_PAGE_SIZE                FEATURE_EFS_OTP_MAX_PAGE_SIZE
#else
  #define FS_OTP_MAX_PAGE_SIZE                FS_MAX_PAGE_SIZE
#endif

#ifdef FEATURE_EFS_OTP_MAX_OTP_PAGES
  #define FS_OTP_MAX_OTP_PAGES                FEATURE_EFS_OTP_MAX_OTP_PAGES
#else
  #define FS_OTP_MAX_OTP_PAGES                0XFFFFFFFF
#endif

#ifdef FEATURE_EFS_OTP_PATH_MAX
  #define FS_OTP_PATH_MAX                     FEATURE_EFS_OTP_PATH_MAX
#else
  #define FS_OTP_PATH_MAX                     128
#endif

#ifdef FEATURE_EFS_OTP_MAX_PCACHE_PAGES
  #define FS_OTP_MAX_PCACHE_PAGES             FEATURE_EFS_OTP_MAX_PCACHE_PAGES
#else
  #define FS_OTP_MAX_PCACHE_PAGES             2
#endif

#define FS_OTP_MAX_ITERS                      1

#ifdef FEATURE_EFS_OTP_MAX_TRANS_BYTES
  #define FS_OTP_MAX_TRANS_BYTES             FEATURE_EFS_OTP_MAX_TRANS_BYTES
#else
  #define FS_OTP_MAX_TRANS_BYTES             (5 * 1024)
#endif

#define FS_OTP_INVALID_PAGE                  0xFFFFFFFF


#endif /* FEATURE_EFS_OTP */

#endif /* not __FS_OTP_CONFIG_H__ */
