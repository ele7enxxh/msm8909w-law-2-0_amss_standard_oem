/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   C R Y P T O   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock crypto functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_crypto.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/20/16   bcho    Remove compiler warnings
01/20/16   stv     Fix to validate the RSA signature in the blob request
01/06/16   stv     Remote simlock support
12/31/15   stv     Send events to client from engine msg handling 
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Support for RSU in simlock
05/12/14   vv      Fix compiler errors
04/15/15   vv      Added the support for FEATURE_SEC_SFS
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_crypto.h"
#include "secapi.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define SIMLOCK_CRYPTO_RSA_ENCRYPT_DATA_LENGTH 256
#define SIMLOCK_CRYPTO_SHA256_DIGEST_LEN        32

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_GET_RANDOM_DATA

DESCRIPTION
  This function generates the random data of requested length

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_get_random_data
(
  uint8         * data_ptr,
  uint16          data_len
)
{
#ifdef FEATURE_SEC_SFS
  secerrno_enum_type  result = E_SUCCESS;

  if((data_ptr == NULL) ||
     (data_len == 0))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  result = secapi_get_random(SECAPI_SECURE_RANDOM, data_ptr, data_len);
  if(result != E_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
#else
  (void)data_ptr;
  (void)data_len;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS */
} /* simlock_crypto_get_random_data */


/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_GENERATE_SALT

DESCRIPTION
  This function generates the random salt

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_generate_salt
(
  simlock_salt_type   salt
)
{
  return simlock_crypto_get_random_data(salt, SIMLOCK_SALT_LEN);
} /* simlock_crypto_generate_salt */


/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_RUN_PBKDF2_ALGORITHM

DESCRIPTION
  This function generates the hash of the control key (HCK) using the PBKDF2
  algorithm. It takes password (control key), salt and iterations as the
  input and outputs the hashed control key (key_data_ptr). The PBKDF2 uses
  the HMAC_SHA256 as the pseudo random function
 
  The imeplementation is based on the RFC2898,
  http://www.rfc-editor.org/rfc/rfc2898.txt

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_run_pbkdf2_algorithm(
  const uint8            * password_data_ptr,
  uint16                   password_data_len,
  simlock_salt_type        salt_data,
  uint16                   salt_data_len,
  uint32                   iterations,
  simlock_hck_type         key_data,
  uint32                   key_data_len
)
{
#ifdef FEATURE_SEC_SFS
  uint8                     * salt_and_index_ptr = NULL;
  uint8                     * hmac_digest1_ptr   = NULL;
  uint8                     * hmac_digest2_ptr   = NULL;
  uint8                     * hmac_out_ptr       = NULL;
  uint32                      dk_block_num       = 0;
  uint32                      dk_block_cnt       = 0;
  uint32                      iteration_cnt      = 0;
  uint8                       digest_index       = 0;
  secerrno_enum_type          result             = E_SUCCESS;
  secapi_handle_type          hsh_handle         = NULL;
  uint8                       password_buf[SIMLOCK_CK_MAX];
  secapi_hsh_param_data_type  hsh_param;

  if((password_data_ptr == NULL)    ||
     (salt_data_len != SIMLOCK_SALT_LEN) ||
     (key_data_len != SIMLOCK_HCK_LEN)   ||
     (password_data_len > SIMLOCK_CK_MAX))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(iterations <= 0)
  {
    SIMLOCK_MSG_ERR_1("invalid iteration count, 0x%x",iterations);
    return SIMLOCK_INCORRECT_PARAMS;
  }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  memset(&hsh_param, 0x00, sizeof(secapi_hsh_param_data_type));
  memset(password_buf, 0x00, SIMLOCK_CK_MAX);

  SIMLOCK_MSG_MED_0("simlock_crypto_run_pbkdf2_algorithm");

  SIMLOCK_MSG_MED_1("iteration_cnt: 0x%x",iterations);
  SIMLOCK_MSG_MED_1("password_data_len: 0x%x",password_data_len);
  SIMLOCK_MSG_MED_1("salt_data_len: 0x%x",salt_data_len);
  SIMLOCK_MSG_MED_1("key_data_len: 0x%x",key_data_len);

  result = secapi_new(&hsh_handle, SECAPI_SHA256);
  if(result != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to get handle, result: 0x%x", result);
    return SIMLOCK_GENERIC_ERROR;
  }

  hsh_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to set param, result: 0x%x", result);
    (void)secapi_delete(&hsh_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  hsh_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    (void)secapi_delete(&hsh_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(salt_and_index_ptr,
                               salt_data_len + 4);

  SIMLOCK_CHECK_AND_MEM_MALLOC(hmac_digest1_ptr,
                               SIMLOCK_HMAC_SHA256_LEN);

  SIMLOCK_CHECK_AND_MEM_MALLOC(hmac_digest2_ptr,
                               SIMLOCK_HMAC_SHA256_LEN);

  SIMLOCK_CHECK_AND_MEM_MALLOC(hmac_out_ptr,
                               SIMLOCK_HMAC_SHA256_LEN);
  if((salt_and_index_ptr == NULL) ||
     (hmac_digest1_ptr == NULL)   ||
     (hmac_digest2_ptr == NULL)   ||
     (hmac_out_ptr == NULL))
  {
    (void)secapi_delete(&hsh_handle);
    SIMLOCK_MEM_FREE(salt_and_index_ptr);
    SIMLOCK_MEM_FREE(hmac_digest1_ptr);
    SIMLOCK_MEM_FREE(hmac_digest2_ptr);
    SIMLOCK_MEM_FREE(hmac_out_ptr);

    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_memscpy(password_buf,
                  SIMLOCK_CK_MAX,
                  password_data_ptr,
                  password_data_len);

  simlock_memscpy(salt_and_index_ptr,
                  salt_data_len + 4,
                  salt_data,
                  salt_data_len);

  SIMLOCK_MSG_MED_4("pwd[0-3]:0x%x, 0x%x, 0x%x, 0x%x",
                      password_buf[0],password_buf[1],password_buf[2],password_buf[3]);
  SIMLOCK_MSG_MED_4("pwd[4-7]:0x%x, 0x%x, 0x%x, 0x%x",
                      password_buf[4],password_buf[5],password_buf[6],password_buf[7]);

  /* calculate the number of blocks of length
     SIMLOCK_HMAC_SHA256_LEN in the derived key */
  dk_block_num = (key_data_len / SIMLOCK_HMAC_SHA256_LEN);
  if(dk_block_num == 0)
  {
    dk_block_num = 1;
  }

  SIMLOCK_MSG_MED_1("dk_block_num: 0x%x", dk_block_num);

  /* for each block apply the pseudo random function (PRF) to
     the password and salt concatenated with the block count for
     requested iterations. Then exclusive-or sum the output of
     PRF for each iteration. Concatenate the output of each block
     and extract first key_data_len blocks for the derived key */
  for(dk_block_cnt = 1; dk_block_cnt <= dk_block_num;
      dk_block_cnt++)
  {
    salt_and_index_ptr[salt_data_len + 0] = (uint8)((dk_block_cnt >> 24) & 0xFF);
    salt_and_index_ptr[salt_data_len + 1] = (uint8)((dk_block_cnt >> 16) & 0xFF);
    salt_and_index_ptr[salt_data_len + 2] = (uint8)((dk_block_cnt >> 8) & 0xFF);
    salt_and_index_ptr[salt_data_len + 3] = (uint8)(dk_block_cnt & 0xFF);

    result = secapi_hsh_create_hmac(hsh_handle,
                                    SECAPI_SHA256,
                                    salt_and_index_ptr,
                                    salt_data_len + 4,
                                    password_buf,
                                    password_data_len,
                                    hmac_digest1_ptr);
    if(result != E_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("failed to create hmac, result: 0x%x", result);
      SIMLOCK_MEM_FREE(salt_and_index_ptr);
      SIMLOCK_MEM_FREE(hmac_digest1_ptr);
      SIMLOCK_MEM_FREE(hmac_digest2_ptr);
      SIMLOCK_MEM_FREE(hmac_out_ptr);
      (void)secapi_delete(&hsh_handle);
      return SIMLOCK_GENERIC_ERROR;
    }

    simlock_memscpy(hmac_out_ptr,
                    SIMLOCK_HMAC_SHA256_LEN,
                    hmac_digest1_ptr, SIMLOCK_HMAC_SHA256_LEN);
    for(iteration_cnt = 1; iteration_cnt < iterations; iteration_cnt++)
    {
      result = secapi_hsh_create_hmac(hsh_handle,
                                      SECAPI_SHA256,
                                      hmac_digest1_ptr,
                                      SIMLOCK_HMAC_SHA256_LEN,
                                      password_buf,
                                      password_data_len,
                                      hmac_digest2_ptr);
      if(result != E_SUCCESS)
      {
        SIMLOCK_MSG_ERR_2("failed to create hmac, result: 0x%x,"
                          "iteration: 0x%x", result, iteration_cnt);
        SIMLOCK_MEM_FREE(salt_and_index_ptr);
        SIMLOCK_MEM_FREE(hmac_digest1_ptr);
        SIMLOCK_MEM_FREE(hmac_digest2_ptr);
        SIMLOCK_MEM_FREE(hmac_out_ptr);
        (void)secapi_delete(&hsh_handle);
        return SIMLOCK_GENERIC_ERROR;
      }

      simlock_memscpy(hmac_digest1_ptr,
                      SIMLOCK_HMAC_SHA256_LEN,
                      hmac_digest2_ptr, SIMLOCK_HMAC_SHA256_LEN);
      for(digest_index = 0; digest_index < SIMLOCK_HMAC_SHA256_LEN; digest_index++)
      {
        hmac_out_ptr[digest_index] ^= hmac_digest1_ptr[digest_index];
      }
    }

    if(key_data_len < SIMLOCK_HMAC_SHA256_LEN)
    {
      simlock_memscpy(key_data, key_data_len,
                      hmac_out_ptr, key_data_len);
      key_data += key_data_len;
    }
    else
    {
      simlock_memscpy(key_data, key_data_len,
                      hmac_out_ptr, SIMLOCK_HMAC_SHA256_LEN);
      key_data += SIMLOCK_HMAC_SHA256_LEN;
    }
  }

  (void)secapi_delete(&hsh_handle);

  SIMLOCK_MEM_FREE(salt_and_index_ptr);
  SIMLOCK_MEM_FREE(hmac_digest1_ptr);
  SIMLOCK_MEM_FREE(hmac_digest2_ptr);
  SIMLOCK_MEM_FREE(hmac_out_ptr);

  return SIMLOCK_SUCCESS;
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
#else
  (void)password_data_ptr;
  (void)password_data_len;
  (void)salt_data;
  (void)salt_data_len;
  (void)iterations;
  (void)key_data;
  (void)key_data_len;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS */
} /* simlock_crypto_run_pbkdf2_algorithm */


/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_RSA_ENCRYPT

DESCRIPTION
  This function performs rsa encrypted on the input buffer in_data_ptr
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_crypto_rsa_encrypt(
  secapi_handle_type   rsa_handle,
  uint8               *in_data_ptr,
  uint16               in_len,
  simlock_data_type   *out_data_ptr)
{
#if defined (FEATURE_SEC_SFS) && !defined (FEATURE_UIM_TEST_FRAMEWORK)
  uint16               temp_len = SIMLOCK_CRYPTO_RSA_ENCRYPT_DATA_LENGTH;
  secerrno_enum_type   result   = E_SUCCESS;

  if(rsa_handle == NULL ||
     in_data_ptr == NULL ||
     in_len == 0 ||
     out_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(out_data_ptr->data_ptr, temp_len);
  if(out_data_ptr->data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  result = secapi_pkx_encrypt(rsa_handle,
                              SECAPI_RSA,
                              SECAPI_PKX_RSA_PADDING_SIG_PKCS1_V15,
                              in_data_ptr,
                              in_len,
                              out_data_ptr->data_ptr,
                              &temp_len);

  if(E_SUCCESS != result ||
     temp_len != SIMLOCK_CRYPTO_RSA_ENCRYPT_DATA_LENGTH)
  {
    SIMLOCK_MEM_FREE(out_data_ptr->data_ptr);
    SIMLOCK_MSG_MED_1("RSA Encryption failed 0x%x", result);
    return SIMLOCK_GENERIC_ERROR;
  }

  out_data_ptr->data_len = temp_len;
  return SIMLOCK_SUCCESS;
#else
  (void)rsa_handle;
  (void)in_data_ptr;
  (void)in_len;
  (void)out_data_ptr;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS && !FEATURE_UIM_TEST_FRAMEWORK*/
} /* simlock_crypto_rsa_encrypt */


/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_RSA_VALIDATE_SIGNATURE

DESCRIPTION
  This function validates the signature and message hash provided to it

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_crypto_rsa_validate_signature(
  secapi_handle_type   rsa_handle,
  uint8               *in_data_ptr,
  uint16               in_len,
  simlock_data_type   *msg_hash_info_ptr)
{
#if defined (FEATURE_SEC_SFS) && !defined (FEATURE_UIM_TEST_FRAMEWORK)
  uint16              temp_len = SIMLOCK_CRYPTO_SHA256_DIGEST_LEN;
  secerrno_enum_type  result   = E_SUCCESS;

  if(rsa_handle == NULL ||
     in_data_ptr == NULL ||
     in_len != SIMLOCK_RSA_LEN ||
     msg_hash_info_ptr == NULL ||
     msg_hash_info_ptr->data_len != SIMLOCK_CRYPTO_SHA256_DIGEST_LEN ||
     msg_hash_info_ptr->data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  temp_len = msg_hash_info_ptr->data_len;

  result = secapi_pkx_authenticate(rsa_handle,
                                   SECAPI_RSA,
                                   SECAPI_PKX_RSA_PADDING_SIG_PSS,
                                   in_data_ptr,
                                   in_len,
                                   msg_hash_info_ptr->data_ptr,
                                   &temp_len);

  if(E_SUCCESS != result)
  {
    SIMLOCK_MEM_FREE(msg_hash_info_ptr->data_ptr);
    SIMLOCK_MSG_MED_1("RSA msg generation failed 0x%x", result);
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
#else
  (void)rsa_handle;
  (void)in_data_ptr;
  (void)in_len;
  (void)msg_hash_info_ptr;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS && !FEATURE_UIM_TEST_FRAMEWORK */
} /* simlock_crypto_rsa_validate_signature */


/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_RSA

DESCRIPTION
  This function performs rsa operation specified in the input parameter rsa_operation

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_rsa(
  simlock_crypto_rsa_operation_type    rsa_operation,
  uint8                             *  in_data_ptr,
  uint16                               in_len,
  uint8                             *  mod_ptr,
  uint16                               mod_len,
  uint8                             *  exp_ptr,
  uint16                               exp_len,
  simlock_data_type                 *  out_data_ptr
)
{
#if defined (FEATURE_SEC_SFS) && !defined (FEATURE_UIM_TEST_FRAMEWORK)
  secerrno_enum_type            result        = E_SUCCESS;
  secapi_handle_type            rsa_handle    = NULL;
  secapi_pkx_param_data_type    rsa_param;
  simlock_result_enum_type      simlock_status = SIMLOCK_GENERIC_ERROR;

  if((in_data_ptr == NULL) ||
     (mod_ptr == NULL) ||
     (mod_len == 0) ||
     (exp_ptr == NULL) ||
     (exp_len == 0) ||
     (out_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&rsa_param, 0x00, sizeof(secapi_pkx_param_data_type));

  result = secapi_new(&rsa_handle, SECAPI_RSA);
  if(result != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to get handle, result: 0x%x", result);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* set the execution mode */
  rsa_param.common.exec_mode = SECAPI_SYNC_EXEC;
  result = secapi_pkx_set_param(rsa_handle, SECAPI_RSA,
                                SECAPI_EXEC_MODE,
                                &rsa_param);
  if(result != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to set param, result: 0x%x", result);
    (void)secapi_delete(&rsa_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* set the execution platform */
  rsa_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  result = secapi_pkx_set_param(rsa_handle, SECAPI_RSA,
                                SECAPI_EXEC_PLATFORM,
                                &rsa_param);
  if(result != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to set param, result: 0x%x", result);
    (void)secapi_delete(&rsa_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* set the modulus */
  rsa_param.rsa.key.modulus_ptr  = mod_ptr;
  rsa_param.rsa.key.modulus_size = mod_len;
  result = secapi_pkx_set_param(rsa_handle, SECAPI_RSA,
                                SECAPI_PKX_RSA_MODULUS,
                                &rsa_param);
  if(result != E_SUCCESS)
  {
    (void)secapi_delete(&rsa_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* set the public key (exponent) */
  rsa_param.rsa.key.public_key_ptr = exp_ptr;
  rsa_param.rsa.key.key_size       = exp_len;
  result = secapi_pkx_set_param(rsa_handle, SECAPI_RSA,
                                SECAPI_PKX_RSA_PUBLIC_KEY,
                                &rsa_param);

  if(result != E_SUCCESS)
  {
    (void)secapi_delete(&rsa_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  switch(rsa_operation)
  {
    case SIMLOCK_CRYPTO_RSA_ENCRYPT:
      simlock_status = simlock_crypto_rsa_encrypt(rsa_handle,
                                                  in_data_ptr,
                                                  in_len,
                                                  out_data_ptr);
      break;

    case SIMLOCK_CRYPTO_RSA_SIGNATURE_VALIDATE:
      /* No output is returned in out_data_ptr
         Signature is sent in in_dataptr & Hash is sent in out_data_ptr
         This function would validate signature and hash sent to it */
      simlock_status = simlock_crypto_rsa_validate_signature(rsa_handle,
                                                             in_data_ptr,
                                                             in_len,
                                                             out_data_ptr);
      break;

    default:
      simlock_status = SIMLOCK_INCORRECT_PARAMS;
      break;
  }

  (void)secapi_delete(&rsa_handle);

  return simlock_status;
#else
  (void)rsa_operation;
  (void)in_data_ptr;
  (void)in_len;
  (void)mod_ptr;
  (void)mod_len;
  (void)exp_ptr;
  (void)exp_len;
  (void)out_data_ptr;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS && !FEATURE_UIM_TEST_FRAMEWORK */
} /* simlock_crypto_encrypt_key_rsa */


/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_GENERATE_HMAC_SHA256

DESCRIPTION
  This function generates the mac for the message to be authenticated.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_generate_hmac_sha256(
  uint8                        * msg_ptr,
  uint16                         msg_len,
  uint8                        * key_ptr,
  uint16                         key_len,
  simlock_hmac_signature_type    hmac_data
)
{
#if defined (FEATURE_SEC_SFS)
  secerrno_enum_type          result        = E_SUCCESS;
  secapi_handle_type          hsh_handle    = NULL;
  secapi_hsh_param_data_type  hsh_param;

  if((msg_len == 0) ||
     (msg_ptr == NULL) ||
     (key_len == 0) ||
     (key_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&hsh_param, 0x00, sizeof(secapi_hsh_param_data_type));

  result = secapi_new(&hsh_handle, SECAPI_SHA256);
  if(result != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to get handle, result: 0x%x", result);
    return SIMLOCK_GENERIC_ERROR;
  }

  hsh_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to set param, result: 0x%x", result);
    (void)secapi_delete(&hsh_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  hsh_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    (void)secapi_delete(&hsh_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  result = secapi_hsh_create_hmac(hsh_handle,
                                  SECAPI_SHA256,
                                  msg_ptr,
                                  msg_len,
                                  key_ptr,
                                  key_len,
                                  hmac_data);
  (void)secapi_delete(&hsh_handle);

  if(result != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to create hmac, result: 0x%x", result);
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
#else
  (void)msg_ptr;
  (void)msg_len;
  (void)key_ptr;
  (void)key_len;
  (void)hmac_data;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS && !FEATURE_UIM_TEST_FRAMEWORK */
} /* simlock_crypto_generate_hmac_sha256 */


/*===========================================================================
FUNCTION SIMLOCK_CRYPTO_GENERATE_DIGEST_SHA256

DESCRIPTION
  This function generates the digest for the message to be authenticated.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_crypto_generate_digest_sha256(
  uint8                        * msg_ptr,
  uint16                         msg_len,
  simlock_data_type            * digest_info_ptr
)
{
#if defined (FEATURE_SEC_SFS) && !defined(FEATURE_UIM_TEST_FRAMEWORK)
  secerrno_enum_type          result        = E_SUCCESS;
  secapi_handle_type          hsh_handle    = NULL;
  secapi_hsh_param_data_type  hsh_param;

  if((msg_len == 0) ||
     (msg_ptr == NULL) ||
     (digest_info_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_HIGH_0("simlock_crypto_generate_digest_sha256");

  memset(&hsh_param, 0x00, sizeof(secapi_hsh_param_data_type));

  SIMLOCK_CHECK_AND_MEM_MALLOC(digest_info_ptr->data_ptr, SIMLOCK_CRYPTO_SHA256_DIGEST_LEN);
  if(digest_info_ptr->data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  result = secapi_new(&hsh_handle, SECAPI_SHA256);
  if(result != E_SUCCESS)
  {
    SIMLOCK_FREE(digest_info_ptr->data_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  hsh_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    SIMLOCK_FREE(digest_info_ptr->data_ptr);
    (void)secapi_delete(&hsh_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  hsh_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  result = secapi_hsh_set_param(hsh_handle, SECAPI_SHA256,
                                SECAPI_HSH_INPUT_MODE,
                                &hsh_param);
  if(result != E_SUCCESS)
  {
    SIMLOCK_FREE(digest_info_ptr->data_ptr);
    (void)secapi_delete(&hsh_handle);
    return SIMLOCK_GENERIC_ERROR;
  }

  result = secapi_hsh_create_digest(hsh_handle,
                                    SECAPI_SHA256,
                                    msg_ptr,
                                    msg_len,
                                    digest_info_ptr->data_ptr);

  (void)secapi_delete(&hsh_handle);

  if(result != E_SUCCESS)
  {
    SIMLOCK_FREE(digest_info_ptr->data_ptr);
    SIMLOCK_MSG_ERR_1("failed to create hmac, result: 0x%x", result);
    return SIMLOCK_GENERIC_ERROR;
  }

  digest_info_ptr->data_len = SIMLOCK_CRYPTO_SHA256_DIGEST_LEN;

  return SIMLOCK_SUCCESS;
#else
  (void)msg_ptr;
  (void)msg_len;
  (void)digest_info_ptr;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS && !FEATURE_UIM_TEST_FRAMEWORK */
} /* simlock_crypto_generate_digest_sha256 */

#endif /* FEATURE_SIMLOCK */
