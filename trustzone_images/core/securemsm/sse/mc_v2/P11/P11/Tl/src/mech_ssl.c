/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "stringl.h"
#include "tlStd.h"
#include "tlLog.h"
#include "tlP11Utils.h"
#include "tlP11Validation.h"
#include "tlP11Mechanisms.h"
#include "ssl.h"

/*
  CK_RV name
      (CK_MECHANISM_TYPE mech, \
      tlp11_mechanism_param_t *params, \
      tlp11_container_object_t *baseKey, \
      tlp11_container_object_t *newKey, \
      const tlp11_token_t *token, \
      tlp11_session_group_t *group, \
      const uint32_t index)
 */
DECLARE_DERIVE_FUNC(derive_master_key)
{
  CK_RV rv = CKR_OK;

  tlApiResult_t result = TLAPI_OK;
  tlApiCrSession_t sessionHandle;

  TL_LOG_I("derive_master_key+");
  do {
    if (!baseKey || !newKey) {
      TL_LOG_E("NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // sanity check param
    if((params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulClientRandomLen
       > sizeof(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.clientRandom))
     ||(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulServerRandomLen
       > sizeof(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.serverRandom)) ) {
      TL_LOG_E("SSL3 params length error! ");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // sanity check Pre Master Secret
    if (!IS_TYPE(baseKey,tlp11_type_generic_key)) {
      TL_LOG_E("base Key needs to generic key : Template Inconsistent");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    if (!IS_TYPE(newKey,tlp11_type_generic_key)) {
      TL_LOG_E("Can only generate generic key : Template Inconsistent");
      rv = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }

    // Mechanism supports only SSL3.0
    if ((baseKey->secret_key_simple.value[0] != 3 )
      ||(baseKey->secret_key_simple.value[1] != 0 )) {
      TL_LOG_E(" SSL3 version %d.%d not supported",
      baseKey->secret_key_simple.value[0] ,
      baseKey->secret_key_simple.value[1] );
      TL_LOG_E("SSL3 Key version incorrect! ");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // Check if the key was generated through the right mechanism
    if (baseKey->key.key_gen_mechanism != CKM_SSL3_PRE_MASTER_KEY_GEN) {
      TL_LOG_E("SSL3 Key incorrectly generated ! ");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    /* now compute the master key from Pre_Master_Secret,
     * Client_Hello_Random and Server_Hello_Random
     *
     * master_secret = MD5[ Pre_Master_Secret
     *                         + SHA ( 'A' + Pre_Master_Secret
     *                                     + Client_Hello_Random
     *                                     + Server_Hello_Random )]
     *              + MD5[ Pre_Master_Secret
     *                         + SHA ('BB' + Pre_Master_Secret
     *                                     + Client_Hello_Random
     *                                     + Server_Hello_Random )]
     *              + MD5[ Pre_Master_Secret
     *                         + SHA ('CCC'+ Pre_Master_Secret
     *                                     + Client_Hello_Random
     *                                     + Server_Hello_Random )]
     */

    uint8_t *ms_const[3] = { "A", "BB","CCC"};
    uint8_t master_secret[SSL3_MASTER_SECRET_LEN];
    memset(master_secret, 0, SSL3_MASTER_SECRET_LEN);
    do
    { int master_secret_offset = 0;
      for (int i = 0; i < 3; ++i ) {
        uint8_t md5_input[SSL3_PREKEY_LEN
                            + SHA1_HASH_LEN];
        uint8_t sha_input[3 + SSL3_PREKEY_LEN
                            + sizeof(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.clientRandom)    //SSL3_C_HELLO_RAND_MAX_LEN
                            + sizeof(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.serverRandom) ]; // SSL3_S_HELLO_RAND_MAX_LEN

        int md5_input_length = SSL3_PREKEY_LEN
                            + SHA1_HASH_LEN;
        int sha_input_length = (i+1)
                            + SSL3_PREKEY_LEN
                            + sizeof(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.clientRandom)
                            + sizeof(params-> SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.serverRandom);
        int sha_offset = 0;
        size_t sha_output_length = SHA1_HASH_LEN;
        size_t md5_output_length = MD5_HASH_LEN;

        // clear bothe MD5 and SHA input buffer
        memset(sha_input, 0, sha_input_length);
        memset(md5_input, 0, md5_input_length);

        // copy Pre Master Secret to MD5 input buffer
        memscpy( md5_input,
                 md5_input_length,
                 baseKey->secret_key_simple.value,
                 baseKey->secret_key_simple.len);

        // copy master secret equation constant to SHA input buffer
        memscpy( sha_input,
                 sha_input_length,
                 ms_const[i],
                 i+1);
        sha_offset +=(i+1);

        // copy Pre Master Key to SHA input buffer
        memscpy(sha_input+sha_offset,
                sha_input_length-sha_offset,
                baseKey->secret_key_simple.value,
                baseKey->secret_key_simple.len);
        sha_offset +=(baseKey->secret_key_simple.len);

        // Copy Client Hello Random to SHA input buffer
        memscpy( sha_input+sha_offset,
                 sha_input_length-sha_offset,
                 params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.clientRandom,
                 params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulClientRandomLen);
        sha_offset +=params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulClientRandomLen;

        // Copy Server Hello Random to SHA input buffer
        memscpy( sha_input+sha_offset,
                 sha_input_length-sha_offset,
                 params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.serverRandom,
                 params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulServerRandomLen);

        // Now compute SHA1 Hash on it
        result = tlApiMessageDigestInit(&sessionHandle,TLAPI_ALG_SHA1);
        if (!IS_OK(result)) {
          TL_LOG_E("tlApiMessageDigestInit: SHA1 : 0x%08X",result);
          rv = CKR_FUNCTION_FAILED;
          break;
        }
        result = tlApiMessageDigestDoFinal( sessionHandle,
                                            sha_input,
                                            sha_input_length,
                                            (md5_input + SSL3_PREKEY_LEN),
                                            &sha_output_length);
        if (!IS_OK(result)) {
          TL_LOG_E("tlApiMessageDigestDoFinal: SHA1 : 0x%08X",result);
          rv = CKR_FUNCTION_FAILED;
          break;
        }
        if (sha_output_length != SHA1_HASH_LEN) {
          TL_LOG_E("tlApiMessageDigestDoFinal: returned wrong length for SHA1 %d vs %d",
                    sha_output_length,
                    SHA1_HASH_LEN);
          rv = CKR_FUNCTION_FAILED;
          break;
        }

        // Compute MD5 on the resultant buffer
        result = tlApiMessageDigestInit(&sessionHandle,TLAPI_ALG_MD5);
        if (!IS_OK(result)) {
          TL_LOG_E("tlApiMessageDigestInit: MD5 : 0x%08X",result);
          rv = CKR_FUNCTION_FAILED;
          break;
        }
        result = tlApiMessageDigestDoFinal( sessionHandle,
                                            md5_input,
                                            md5_input_length,
                                            (master_secret + master_secret_offset),
                                            &md5_output_length);
        if (!IS_OK(result)) {
          TL_LOG_E("tlApiMessageDigestDoFinal: MD5 : 0x%08X",result);
          rv = CKR_FUNCTION_FAILED;
          break;
        }
        if (md5_output_length != MD5_HASH_LEN) {
          TL_LOG_E("tlApiMessageDigestDoFinal: returned wrong length for MD5 %d vs %d",
                               md5_output_length,
                               MD5_HASH_LEN);
          rv = CKR_FUNCTION_FAILED;
          break;
        }

        // adjust the offset to get the next part of master secret
        master_secret_offset += MD5_HASH_LEN;
        DUMP_HEX("master_secret step",master_secret,sizeof(master_secret));
      }
      // Check all is good
      if (rv != CKR_OK) {
        break;
      }

      // Prepare to return master secret
      if (baseKey->secret_key.always_sensitive == CK_FALSE) {
        newKey->secret_key.always_sensitive = CK_FALSE;
      } else {
        newKey->secret_key.always_sensitive = newKey->secret_key.sensitive;
      }

      if (baseKey->secret_key.never_extractable == CK_FALSE) {
        newKey->secret_key.never_extractable = CK_FALSE;
      } else {
        if (newKey->secret_key.extractable == CK_TRUE)
          newKey->secret_key.never_extractable = CK_FALSE;
        else
          newKey->secret_key.never_extractable = CK_TRUE;
      }
      newKey->secret_key_simple.len = SSL3_MASTER_SECRET_LEN;
      memscpy(newKey->secret_key_simple.value,
              newKey->secret_key_simple.len,
              master_secret,
              newKey->secret_key_simple.len);
      params->SSL3_MASTER_KEY_DERIVE_PARAMS.version.major = baseKey->secret_key_simple.value[0];
      params->SSL3_MASTER_KEY_DERIVE_PARAMS.version.minor = baseKey->secret_key_simple.value[1];

    } while (0);
  } while(0);
  TL_LOG_I("derive_master_key-: %08X",rv);
  return rv;
}
