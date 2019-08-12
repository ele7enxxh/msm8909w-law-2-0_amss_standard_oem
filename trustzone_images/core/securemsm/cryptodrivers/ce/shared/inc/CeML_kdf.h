#ifndef _CeML_kdf
#define _CeML_kdf

/*===========================================================================

                    Crypto Engine Module API

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

 $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/cryptodrivers/ce/shared/inc/CeML_kdf.h#1 $
 $DateTime: 2016/06/17 14:31:11 $
 $Author: pwbldsvc $ 

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
===========================================================================*/

#include "comdef.h"
#include "stddef.h"
#include "CeML.h"

/**
  Derives a Key using known label, context strings and using either 
  the Hardware Key [Primary or Secondary] or a user provided Key.

  This is an implementation of the key derivation algorithm based on
  AES_CMAC from FIPS Special Publication 800-38B.

  @param[in]  input_key        Pointer to the input Key.
                               This should be NULL if Primary or Secondary
                               HW Key are to be used for Ciphering.
			       
  @param[in]  input_key_len    Length of the 'input_key' in bytes.
                               The value of this is not acted on if the
                               'input_key' is NULL.

  @param[in]  label            Pointer to a string constant.

  @param[in]  label_len        Length of the string constant 'label'.

  @param[in]  context          Pointer to a string constant.

  @param[in]  context_len      Length of the string constant 'context'.

  @param[out] output_key       Pointer to the generated Key.
                               The Memory for this should be provided by the caller.
  @param[in]  output_key_len   Length of the Key to be generated
                               in bytes. This is size of the
                               buffer that the 'output_key'
                               points to.
       
  @return
  CEML_ERROR_SUCCESS - Successful completion.
  CEML_ERROR_FAILURE - Any other failure.

  @dependencies
  The memory for the 'output_key' should be allocated by the caller
  according to the 'output_key_len'.
*/
CeMLErrorType CeML_kdf
(
  const void  *input_key,
  uint32       input_key_len,
  const void  *label,
  uint32       label_len,
  const void  *context,
  uint32       context_len,
  void        *output_key,
  uint32       output_key_len
);

#endif
