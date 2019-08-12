/***********************************************************************
 * fs_otp_pcache.h
 *
 * EFS OTP (One-Time-Programmable) Page-Cache module.
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * This file reads OTP pages from the flash partition and caches them
 * for the EFS OTP module to read and write to.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_otp_pcache.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-08-17   rp    Create

===========================================================================*/

#ifndef __FS_OTP_PCACHE_H__
#define __FS_OTP_PCACHE_H__

#include "fs_otp_config.h"

#ifdef FEATURE_EFS_OTP

#include "fs_device.h"


struct fs_otp_pcache_status
{
  int       is_in_use;
  int       is_for_write;
  uint32    page_nbr;
  uint32    age;
};

struct fs_otp_pcache
{
  uint8                                 *pages[FS_OTP_MAX_PCACHE_PAGES];
  struct fs_otp_pcache_status           status[FS_OTP_MAX_PCACHE_PAGES];
  efs_device_t                          dev;
  uint32                                dev_page_size;
  uint32                                dev_total_pages;
};


void fs_otp_pcache_init (struct fs_otp_pcache *pcache,
                         efs_device_t dev, uint32 dev_total_pages);
void fs_otp_pcache_read_bytes (struct fs_otp_pcache *pcache,
                uint32 offset, uint32 bytes_count, uint8 *buffer);
void fs_otp_pcache_write_bytes (struct fs_otp_pcache *pcache,
                uint32 offset, uint32 bytes_count, uint8 *buffer);
void fs_otp_pcache_write_changed_pages (struct fs_otp_pcache *pache);


#endif /* FEATURE_EFS_OTP */

#endif /* not __FS_OTP_PCACHE_H__ */
