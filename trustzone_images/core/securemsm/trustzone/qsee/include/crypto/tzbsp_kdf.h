#ifndef TZBSP_KDF_H
#define TZBSP_KDF_H

/**
@file tzbsp_kdf.h
@brief Provide kdf api wrapper
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/crypto/tzbsp_kdf.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/23/13   anishm      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/** 
 * Derives a Key using known label, context strings and using either 
 * the Hardware Key [Primary or Secondary] or a user provided Key.
 *
 * This is an implementation of the key derivation algorithm based on
 * AES_CMAC from FIPS Special Publication 800-38B.
 *
 * @param[in]  input_key        Pointer to the input Key.
 * 
 * @param[in]  input_key_len    Length of the 'input_key' in bytes. Has to represent
 *                              size of input_key
 *
 * @param[in]  label            Pointer to a string constant.
 *
 * @param[in]  label_len        Length of the string constant 'label'.
 *
 * @param[in]  context          Pointer to a string constant.
 *
 * @param[in]  context_len      Length of the string constant 'context'.
 *
 * @param[out]  output_key      Pointer to the generated Key.
 *                              The Memory for this should be provided by the caller.
 * 
 * @param[in]   output_key_len  Length of the Key to be generated in bytes.
 *                              This is size of the buffer that the 'output_key'
 *                              points to.
 *      
 * @return 
 *          E_SUCCESS       If success
 *          -E_NOT_ALLOWED  If invalid data 
 *          -E_FAILURE      If failure
 */

int tzbsp_kdf(const void  *input_key,
              uint32       input_key_len,
              const void  *label,
              uint32       label_len,
              const void  *context,
              uint32       context_len,
              void        *output_key,
              uint32       output_key_len);

#endif
