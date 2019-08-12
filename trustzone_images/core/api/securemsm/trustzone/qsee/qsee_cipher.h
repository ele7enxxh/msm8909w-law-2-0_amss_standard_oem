#ifndef QSEE_CIPHER_H
#define QSEE_CIPHER_H

/**
@file qsee_cipher.h
@brief Provide cipher API wrappers
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_cipher.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/8/10    cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QSEE_AES128_IV_SIZE         16
#define QSEE_AES128_KEY_SIZE        16
#define QSEE_AES256_IV_SIZE         16
#define QSEE_AES256_KEY_SIZE        32
#define QSEE_TRIPLE_DES_KEY_SIZE    24
#define QSEE_TRIPLE_DES_IV_SIZE     8

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef void qsee_cipher_ctx;

/** Cipher supported algorithms */
typedef enum
{
   QSEE_CIPHER_ALGO_AES_128    = 0,
   QSEE_CIPHER_ALGO_AES_256,
   QSEE_CIPHER_ALGO_TRIPLE_DES,
   QSEE_CIPHER_ALGO_INVALID     =0x7FFFFFFF,
} QSEE_CIPHER_ALGO_ET;

typedef enum
{
  QSEE_CIPHER_PARAM_KEY        = 0,
  QSEE_CIPHER_PARAM_IV,
  QSEE_CIPHER_PARAM_MODE,
  QSEE_CIPHER_PARAM_PAD,
  QSEE_CIPHER_PARAM_NONCE,
  QSEE_CIPHER_PARAM_XTS_KEY,
  QSEE_CIPHER_PARAM_XTS_DU_SIZE,
  QSEE_CIPHER_PARAM_CCM_PAYLOAD_LEN,
  QSEE_CIPHER_PARAM_CCM_MAC_LEN,
  QSEE_CIPHER_PARAM_CCM_HDR_LEN,
  QSEE_CIPHER_PARAM_INVALID = 0x7FFFFFFF,
} QSEE_CIPHER_PARAM_ET;

/** Supported modes of operation */
typedef enum
{
   QSEE_CIPHER_MODE_ECB        = 0,
   QSEE_CIPHER_MODE_CBC,
   QSEE_CIPHER_MODE_CTR,
   QSEE_CIPHER_MODE_XTS,
   QSEE_CIPHER_MODE_CCM,
   QSEE_CIPHER_MODE_CTS,
   QSEE_CIPHER_MODE_INVALID = 0x7FFFFFFF,
} QSEE_CIPHER_MODE_ET;

typedef enum
{
  QSEE_CIPHER_PAD_ISO10126,
  QSEE_CIPHER_PAD_PKCS7,
  QSEE_CIPHER_PAD_NO_PAD,
  QSEE_CIPHER_PAD_INVALID = 0x7FFFFFFF,
} QSEE_CIPHER_PAD_ET;

#define AES_BLOCK_SZ (0x10)

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

 /**
 * @brief Intialize a cipher context for encrypt/decrypt operation
 *
 * @param[in] alg  The algorithm standard to use
 * @param[out] cipher_ctx The cipher ctx
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_cipher_init(QSEE_CIPHER_ALGO_ET alg,
                     qsee_cipher_ctx  **cipher_ctx);

/**
 * @brief Release all resources with a given cipher context.
 *
 * @param[in] cipher_ctx Cipher context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_cipher_free_ctx(qsee_cipher_ctx *cipher_ctx);


/**
 * @brief Resets cipher context, will not reset key
 *
 * @param[in] cipher_ctx The cipher ctx
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_cipher_reset(qsee_cipher_ctx *cipher_ctx);

/**
 * @brief Modify the parameters for a given cipher operation.
 *
 * @param[in] cipher_ctx  Cipher context
 * @param[in] param_id    The parameter to modify
 * @param[in] param       The parameter value to set.
 * @param[in] param_len   The length of the param (in bytes).
 *
 * @return 0 on success,
 * negative on failure,
 * -E_NOT_SUPPORTED if an alogirthm or parameter is not currently supported.
 *
 */
int qsee_cipher_set_param(qsee_cipher_ctx *cipher_ctx,
                          QSEE_CIPHER_PARAM_ET param_id,
                          const void *param,
                          uint32 param_len);
/**
 * @brief Retrieve the parameters for a given cipher context.
 *
 * @param[in] cipher_ctx  Cipher context
 * @param[in] param_id    The parameter to retrieve
 * @param[in] param       The memory location to store the parameter.
 * @param[in] param_len   The length of the param (in bytes).
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_cipher_get_param(const qsee_cipher_ctx *cipher_ctx,
                          QSEE_CIPHER_PARAM_ET param_id,
                          void *param,
                          uint32 *param_len);

/**
 * @brief This function encrypts the passed plaintext message using
 *        the specified algorithm. The memory allocated for the
 *        ciphertext must be large enough to hold the plaintext
 *        equivalent. If the output buffer is not large enough to
 *        hold the encrypted results, an error is returned.
 *
 * @param[in] cipher_ctx         The cipher context to create
 * @param[in] pt                 The input plaintext buffer
 * @param[in] pt_len             The input plaintext buffer length (in bytes)
 * @param[in,out] ct             The output ciphertext buffer
 * @param[in,out] ct_len         The output ciphertext buffer length. This
 *                               is modified to the actual ct bytes written.
 *
 * @return E_SUCCESS if successful
 *         E_INVALID_ARG if not multiple of block length
 *         E_FAILURE otherwise
 */
int qsee_cipher_encrypt(const qsee_cipher_ctx *cipher_ctx,
                        const uint8 *pt,
                        uint32 pt_len,
                        uint8 *ct,
                        uint32 *ct_len);

/**
 * @brief This function decrypts the passed ciphertext message using
 *        the specified algorithm. The memory allocated for the
 *        plaintext must be large enough to hold the ciphertext
 *        equivalent. If the output buffer is not large enough to
 *        hold the decrypted results, an error is returned.
 *
 * @param[in] cipher_ctx         The cipher context to create
 * @param[in] ct                 The input ciphertext buffer
 * @param[in] ct_len             The input ciphertext buffer length (in bytes)
 * @param[in,out] pt             The output plaintext buffer
 * @param[in,out] pt_len         The output plaintext buffer length. This
 *                               is modified to the actual pt bytes written.
 *
 * @return E_SUCCESS if successful
 *         E_INVALID_ARG if not multiple of block length
 *         E_FAILURE otherwise
 */
int qsee_cipher_decrypt(const qsee_cipher_ctx *cipher_ctx,
                        const uint8 *ct,
                        uint32 ct_len,
                        uint8* pt,
                        uint32 *pt_len);

#endif /*QSEE_CIPHER_H*/

