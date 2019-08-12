#ifndef TZBSP_HASH_H
#define TZBSP_HASH_H

/**
@file tzbsp_hash.h
@brief Provide hash API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
08/20/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TZBSP_SHA1_HASH_SZ    20
#define TZBSP_SHA256_HASH_SZ  32

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef void tzbsp_hash_ctx;

/** Support for SHA1 and SHA256 for hash */
typedef enum
{
  TZBSP_HASH_NULL         = 1, ///< Do not perform any hashing
  TZBSP_HASH_SHA1         = 2,
  TZBSP_HASH_SHA256       = 3
} TZBSP_HASH_ALGO_ET;

typedef enum
{
  TZBSP_HASH_PARAM_MODE              = 0x00,
  TZBSP_HASH_PARAM_HMAC_KEY          = 0x01,
  TZBSP_HASH_PARAM_SEQ               = 0x02
} TZBSP_HASH_PARAM_ET;

typedef enum 
{ 
  TZBSP_HASH_MODE_HASH    = 0, // Plain SHA
  TZBSP_HASH_MODE_HMAC    = 1  // HMAC SHA 
} TZBSP_HASH_MODE_ET;

typedef enum 
{ 
  TZBSP_HASH_FIRST        = 0, 
  TZBSP_HASH_LAST         = 1  
} TZBSP_HASH_SEQ_ET; 

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief  This function will create a message digest hash using the
 *         algorithm specified.
 *
 * @param hash[in]         The hash algorithm
 * @param msg[in]          The message to hash
 * @param msg_len[in]      The length of the message
 * @param digest[in,out]   The digest to store
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
int tzbsp_hash(TZBSP_HASH_ALGO_ET alg,
               const uint8 *msg,
               uint32 msg_len,
               uint8 *digest,
               uint32 digest_len);

/**
 * @brief Intialize a hash context for update and final functions.
 *
 * @param alg[in] The algorithm standard to use
 * @param hash_ctx[out] The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
int tzbsp_hash_init(TZBSP_HASH_ALGO_ET alg,
                    tzbsp_hash_ctx **hash_ctx);


/**
 * @brief Resets the state of context to initial state
 *
 * @param[in] hash_ctx Hash context to be reset
 *
 * @return 0 on success, negative on failure
 *
 */
int tzbsp_hash_reset(tzbsp_hash_ctx  *hash_ctx);

/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         tzbsp_hash_init().
 *
 * @param hash_ctx[in]    The hash context
 * @param[in]             Pointer to the msg to hash
 * @param[in]             Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 *
 * @see tzbsp_hash_init
 */
int tzbsp_hash_update(const tzbsp_hash_ctx  *hash_ctx,
                      const uint8           *msg,
                      uint32                msg_len);

/**
 * @brief  Compute the digest hash value
 *
 * @param hash_ctx[in]     The hash context
 * @param digest[in]       Pointer to output message digest hash
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 *
 * @see tzbsp_hash_init
 */
int tzbsp_hash_final(const tzbsp_hash_ctx  *hash_ctx,
                     uint8                 *digest,
                     uint32                digest_len);

/**
 * @brief Release all resources with a given hash context.
 *
 * @param hash_ctx[in] Hash context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int tzbsp_hash_free_ctx(tzbsp_hash_ctx *hash_ctx);

/**
 * @brief Modify the parameters for a given hash/hmac operation.
 *
 * @param[in] hash_ctx    Hash context
 * @param[in] param_id    The parameter to modify
 * @param[in] param       The parameter value to set.
 * @param[in] param_len   The length of the param (in bytes).
 *
 * @return 0 on success,
 * negative on failure,
 * -E_NOT_SUPPORTED if an alogirthm or parameter is not currently supported.
 *
 */
int tzbsp_hash_set_param(tzbsp_hash_ctx *hash_ctx,
                         TZBSP_HASH_PARAM_ET param_id,
                         const void *param,
                         uint32 param_len);


#endif
