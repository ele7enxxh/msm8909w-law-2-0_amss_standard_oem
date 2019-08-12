/***********************************************************************
 * fs_signed_img_param.h
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_tar/src/fs_signed_img_param.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-01-29   nr    Migrate to new signed image authentication apis.
2011-06-18   wek   Create. Add secure TAR signing/verification.

===========================================================================*/

#ifndef __FS_SIGNED_IMG_PARAM_H__
#define __FS_SIGNED_IMG_PARAM_H__

#include "fs_config.h"
#include "comdef.h"
#include "secboot_util.h"

/* Returns the hash and hash length for the root certificate.
 *
 * The root certificate hash is defined by the macro FS_SIGNED_IMG_ROOT_HASH.
 * There are several ways to modify the root certificate hash used to
 * verify the file system image. The different methods are:
 *   - Define the macro FS_SIGNED_IMG_ROOT_HASH globally. The value of this
 *     macro is an array as defined in C, e.i. {0x1f, 0x29, 0x3B, 0x1A} for
 *     a 4 byte hash. The root certificate hash is typically 32 bytes long.
 *   - Modify the value of the macro in fs_signed_img_param.c
 *   - Modify the implementation of the function fs_sign_get_root_hash ().
 *
 * @param hash_len[out]  Lenght of the root certificate hash.
 *
 * @return   Pointer to the root certificate hash.
 */
uint8* fs_sign_get_root_hash (uint32 *hash_len);

/* Gets the MSM Hardware ID used to sign the file system image.
 *
 * The MSM Hardware ID is defined by the macro FS_SIGNED_IMG_MSM_HW_ID, it
 * is 8 bytes long. Similarly to the root certificate hash there are three
 * ways to change this value:
 *   - Define a new value for the macro FS_SIGNED_IMG_MSM_HW_ID.
 *   - Modify the value of the macro in fs_signed_img_param.c
 *   - Modify the implementation of the function fs_sign_get_msm_id ().
 *
 * @param msm_id[out]   MSM Hardware ID configured to authenticate the file
 *                      system image.
 */
void fs_sign_get_msm_id (uint64 *msm_id);

/* Gets the Software ID used to sign the file system image.
 *
 * The software ID is defined by the macro FS_SIGNED_IMG_SOFTWARE_ID, it
 * is 8 bytes long. Similarly to the root certificate hash there are three
 * ways to change this value:
 *   - Define a new value for the macro FS_SIGNED_IMG_SOFTWARE_ID.
 *   - Modify the value of the macro in fs_signed_img_param.c
 *   - Modify the implementation of the function
 *     fs_sign_get_software_version_id ().
 *
 * @param sw_version[out]   Software version ID configured to authenticate
 *                          the file system image.
 */
void fs_sign_get_software_version_id (uint64 *sw_version);

#endif /* not __FS_SIGNED_IMG_PARAM_H__ */
