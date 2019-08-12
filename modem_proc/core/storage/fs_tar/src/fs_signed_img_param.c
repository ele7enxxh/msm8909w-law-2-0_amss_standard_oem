/***********************************************************************
 * fs_signed_img_param.c
 *
 * Parameters needed to verify signed image.
 * Copyright (C) 2011, 2013 QUALCOMM Technologies, Inc.
 *
 * When the file system image is signed, there are some parameters
 * needed to verify the authenticity of the image. This file implements
 * the methods for retrieving these parameters.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_tar/src/fs_signed_img_param.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-01-29   nr    Migrate to new signed image authentication apis.
2011-06-18   wek   Create. Add secure TAR signing/verification.

===========================================================================*/

#include "fs_signed_img_param.h"

#ifndef FS_SIGNED_IMG_ROOT_HASH
  #define FS_SIGNED_IMG_ROOT_HASH \
   {0xd5, 0x85, 0x22, 0xcd, 0x60, 0x2a, 0x50, 0x1c, \
    0x3a, 0x79, 0x8d, 0x20, 0x15, 0x33, 0xae, 0x31, \
    0xf9, 0x24, 0x27, 0x2b, 0x0b, 0x76, 0xf1, 0x59, \
    0xfe, 0x81, 0x75, 0x3b, 0x5f, 0xcf, 0xce, 0x60}
#endif

#ifndef FS_SIGNED_IMG_MSM_HW_ID
  #define FS_SIGNED_IMG_MSM_HW_ID    0x0000000000000000
#endif

#ifndef FS_SIGNED_IMG_SOFTWARE_ID
  #define FS_SIGNED_IMG_SOFTWARE_ID  0x0000000000000000
#endif


static uint8 fs_root_hash[] = FS_SIGNED_IMG_ROOT_HASH;

uint8* fs_sign_get_root_hash (uint32 *hash_len)
{
  *hash_len = sizeof (fs_root_hash);
  return &fs_root_hash[0];
}

void fs_sign_get_msm_id (uint64 *msm_id)
{
  *msm_id = FS_SIGNED_IMG_MSM_HW_ID;
}

void fs_sign_get_software_version_id (uint64 *sw_version)
{
  *sw_version = FS_SIGNED_IMG_SOFTWARE_ID;
}
