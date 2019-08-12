#ifndef SECBOOT_UTIL_H
#define SECBOOT_UTIL_H
/*****************************************************************************
*
* @file  secboot_util.h (Secboot wrapper API)
*
* @brief API to support extra/wrapper secboot logic
*       
* Copyright (c) 2010-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secboot/api/secboot_img_util.h#1 $

when         who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/13   hw      Init version

============================================================================*/
#include "secboot.h"
#include "secboot_hw.h"

typedef enum
{
  E_AUTH_IMG_SUCCESS        = 0x0,
  E_AUTH_IMG_FAILURE        = 0x1,

  /* reserved for secboot error type */

  E_AUTH_IMG_INVALID_PRAM   = 0x100,
  E_AUTH_IMG_INIT_FAILED,
  E_AUTH_IMG_DEINIT_FAILED,
  E_AUTH_ENG_CERT_NOT_ALLOWED,
  E_AUTH_IMG_IMAGE_NOT_SUPPORTED,
  E_AUTH_IMG_HW_FAILURE,
} e_auth_img_error_type;

typedef struct 
{
  uint32                         code_segment;
  uint64                         image_sw_id;
  uint32                         pbl_reenable_debug;
  secboot_ftbl_type*             secboot_ftbl_ptr;
  secboot_hw_ftbl_type*          secboot_hw_ftbl_ptr;
  secboot_crypto_hash_ftbl_type* crypto_hash_ftbl_ptr;
} secboot_auth_image_context;

/**
 * @brief This function authenticates the signed image for secure and non-secure device, but ignores 
 *        UNTRUSTED ROOT failure for non-secure device. It passes with unsigned image in non-secure 
 *        device, if FEATURE_ALLOW_UNSIGNED_IMAGE is defined. It support engg cert functionality.
 *
 * @param[in]        context                context that stores function pointers and related info.
 *                   code_segment           indicate which processor the code is running on
 *                   image_info             image information
 *
 * @param[in, out] verified_info            verified information
 *
 * @return E_AUTH_IMG_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 */
e_auth_img_error_type secboot_authenticate_image_context(
  secboot_auth_image_context*  context,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info
);

/**
 * @brief This function authenticates the signed image for secure and non-secure device, but ignores UNTRUSTED ROOT
 *        failure for non-secure device. It passes for unsigned image in non-secure device, 
 *        if FEATURE_ALLOW_UNSIGNED_IMAGE is defined                 
 *
 * @param[in] code_segment             indicate which processor the code is running on
 *            image_info                 image information
 *            secboot_ftbl_ptr          secboot function table pointer
 *            secboot_hw_ftbl_ptr    secboot hw function table pointer
 *            crypto_hash_ftbl_ptr    secboot hashing function table pointer
 *
 * @param[in, out] code_segment             indicate which processor the code is running on
 *
 * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 */
secboot_error_type secboot_authenticate_image(
  uint32 code_segment,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info,
  secboot_ftbl_type* secboot_ftbl_ptr,
  secboot_hw_ftbl_type* secboot_hw_ftbl_ptr,
  secboot_crypto_hash_ftbl_type* crypto_hash_ftbl_ptr
);

#endif //SECBOOT_UTIL_H
