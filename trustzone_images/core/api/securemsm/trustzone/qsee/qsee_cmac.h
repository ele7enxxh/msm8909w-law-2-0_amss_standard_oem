#ifndef QSEE_CMAC_H
#define QSEE_CMAC_H

/**
@file qsee_cmac.h
@brief Provide hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_cmac.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/29/13   ejt      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define QSEE_CMAC_DIGEST_SIZE      16

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** Supported CMAC algorithms   */
typedef enum
{
   QSEE_CMAC_ALGO_AES_128          = 1,
   QSEE_CMAC_ALGO_AES_256          = 2,
   QSEE_CMAC_INVALID               = 0x7FFFFFFF,
}  QSEE_CMAC_ALGO_ET;  

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Create a cipher MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to CMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] cmac_digest    - Pointer to cmac digest (memory provided by caller)
 * @param[in] cmac_len        - Length of CMAC in bytes
 *
 * @return 0 on success, negative on failure
*/
int qsee_cmac(QSEE_CMAC_ALGO_ET alg, const uint8 *msg, uint32 msg_len,
              const uint8 *key, uint32 key_len, uint8 *cmac_digest,
              uint32 cmac_len);


#endif /*QSEE_CMAC_H*/

