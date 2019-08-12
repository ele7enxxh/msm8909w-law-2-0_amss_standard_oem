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

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secboot/api/secboot_util.h#1 $

when         who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/13   hw      Init version

============================================================================*/
#include "secboot.h"
#include "secboot_hw.h"

/**
 * @brief This function authenticates the signed image for secure and non-secure device, but ignores UNTRUSTED ROOT
 *            failure for non-secure device. It returns SUCCESS for unsigned image in non-secure device.                  
 *
 * @param[in] code_segment             indicate which processor the code is running on
 *                   image_info                 image information
 *                   secboot_ftbl_ptr          secboot function table pointer
 *                   secboot_hw_ftbl_ptr    secboot hw function table pointer
 *                   crypto_hash_ftbl_ptr    secboot hashing function table pointer
 *
 * @param[in, out] verified_info       verified information
 *
 * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 */
secboot_error_type secboot_authenticate_image(uint32 code_segment,
                                     secboot_image_info_type* image_info,
                                     secboot_verified_info_type* verified_info,
                                     secboot_ftbl_type* secboot_ftbl_ptr,
                                     secboot_hw_ftbl_type* secboot_hw_ftbl_ptr,
                                     const secboot_crypto_hash_ftbl_type* crypto_hash_ftbl_ptr);

secboot_error_type secboot_search_OU_field_data(uint64* ou_field_value_ptr, 
                                                const uint8* ou_field_string_ptr, 
                                                uint64 ou_field_string_size, 
                                                const uint8* data_ptr, 
                                                uint64 data_len);
									 
#endif //SECBOOT_UTIL_H
