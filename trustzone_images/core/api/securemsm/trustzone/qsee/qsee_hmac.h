#ifndef QSEE_HMAC_H
#define QSEE_HMAC_H

/**
@file qsee_hmac.h
@brief Provide hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_hmac.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/09/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef void qsee_hmac_ctx;

/** Supported HMAC algorithms   */
typedef enum
{
   QSEE_HMAC_SHA1          = 1,
   QSEE_HMAC_SHA256        = 2,
   QSEE_HMAC_INVALID = 0x7FFFFFFF,
}  QSEE_HMAC_ALGO_ET;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Create a hash MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to HMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] msg_digest     - Pointer to message digest (memory provided by caller)
 *
 * @return 0 on success, negative on failure
*/
int qsee_hmac(QSEE_HMAC_ALGO_ET alg, const uint8 *msg, uint32 msg_len,
              const uint8 *key, uint16 key_len, uint8 *msg_digest);

/**
 * @brief Initialize a hash MAC per FIPS pub 198 using the 
 *        specified hash algorithm.
 *
 * @param[out] ctx         - Pointer to hold context 
 * @param[in]  alg         - algorithm for Hash MAC
 *
 * @return 0 on success, negative on failure
*/
int qsee_hmac_init(QSEE_HMAC_ALGO_ET alg, qsee_hmac_ctx **ctx);

/**
 * @brief Updates hash MAC per FIPS pub 198 using the specified 
 *        hash algorithm.
 *
 * @param[in] ctx             - Context pointer
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 *
 * @return 0 on success, negative on failure
*/
int qsee_hmac_update(qsee_hmac_ctx *ctx, uint8 *msg, uint32 msg_len);

/**
 * @brief Final operation for hash MAC per FIPS pub 198 using 
 *        the specified hash algorithm.
 *
 * @param[out] msg_digest        - Pointer to message digest 
 *                                 (memory provided by caller)
 * @param[in] msg_digest_length  - Length of message in bytes
 *
 * @return 0 on success, negative on failure
*/
int qsee_hmac_final(qsee_hmac_ctx *ctx, uint8 *msg_digest, uint32 msg_digest_length);

/**
 * @brief Release all resources with a given hmac context.
 *
 * @param hmac_ctx[in] hmac context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hmac_free_ctx(qsee_hmac_ctx *hmac_ctx);

#endif /*QSEE_HMAC_H*/

