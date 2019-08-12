#ifndef TZBSP_HMAC_H
#define TZBSP_HMAC_H

/**
@file tzbsp_hmac.h
@brief Provide hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/crypto/tzbsp_hmac.h#1 $
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
typedef void tzbsp_hmac_ctx;

/** Supported HMAC algorithms   */
typedef enum
{
   TZBSP_HMAC_SHA1          = 1,
   TZBSP_HMAC_SHA256        = 2
}  TZBSP_HMAC_ALGO_ET;

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
int tzbsp_hmac(TZBSP_HMAC_ALGO_ET alg, const uint8 *msg, uint32 msg_len,
               const uint8 *key, uint16 key_len, uint8 *msg_digest);

/**
 * @brief Initialization routine for hmac operation
 *
 * @param[in] alg             - Hmac algorithm, HMAC-SHA1 or HMAC-SHA256
 * @param[in] hmac_ctx        - Pointer to pointer of hmac context
 *
 * @return 0 on success, negative on failure
*/
int tzbsp_hmac_init(TZBSP_HMAC_ALGO_ET alg,
                    tzbsp_hmac_ctx **hmac_ctx);


/**
 * @brief Update hmac, it is callable multiple times.
 *
 * @param[in] hmac_ctx        - Pointer to hmac context
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 *
 * @return 0 on success, negative on failure
*/
int tzbsp_hmac_update(const tzbsp_hmac_ctx *hmac_ctx,
                      uint8 *msg, uint32 msg_len);


/**
 * @brief Finalize hmac,
 *
 * @param[in]  hmac_ctx        - Pointer to hmac context
 * @param[out] digest          - Pointer to message digest (memory provided by caller)
 * @param[in/out] digest_len   - Length of message digest in bytes
 *
 * @return 0 on success, negative on failure
*/

int tzbsp_hmac_final(const tzbsp_hmac_ctx *hmac_ctx,
                     uint8 *digest, uint32 digest_len);

/**
 * @brief Release all resources with a given hmac context.
 *
 * @param[in] hmac_ctx HMAC context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int tzbsp_hmac_free_ctx(tzbsp_hmac_ctx *hmac_ctx);

#endif
