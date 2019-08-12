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
#include "tls.h"

// Use TL_TEST_WITH_SECRET_1 #define to test the PRF function and use tlcP11PRF app in the HLOS side
//#define TL_TEST_WITH_SECRET_1

/*
 * CKM_TLS_PRF mechanism used to produce a securely generated pseudo-random
 * output of arbitrary length
 *
 **/
 DECLARE_DERIVE_FUNC(tls_pseudorandom_function)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCrSession_t sessionHandle;

  TL_LOG_I("tls_pseudorandom_function+");
  do {
    if (!baseKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if(baseKey->secret_key_simple.len != SSL3_MASTER_SECRET_LEN) {
      TL_LOG_E(" Wrong Key length!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if( (params->TLS_PRF_PARAMS.ulOutputLen < PRF_OUT_MIN_LEN )
      ||(params->TLS_PRF_PARAMS.ulOutputLen > PRF_OUT_MAX_LEN ) ){
     TL_LOG_E(" OutPut Length Error! ");
     rv = CKR_GENERAL_ERROR;
     break;
    }

    // Key = Secret
    tlApiKey_t mcKey = {0};
    tlApiSymKey_t symKey = {0};

    #ifdef TL_TEST_WITH_SECRET_1

    CK_BYTE Secret_1[16] = {
      0x9b, 0xbe, 0x43, 0x6b, 0xa9, 0x40, 0xf0, 0x17,
      0xb1, 0x76, 0x52, 0x84, 0x9a, 0x71, 0xdb, 0x35
    };
    uint32_t Secret_1_length  = sizeof(Secret_1);
    symKey.key = Secret_1;
    symKey.len = Secret_1_length;
    mcKey.symKey = &symKey;

    DUMP_HEX("Secret", Secret_1,Secret_1_length);
    DUMP_HEX("Seed"  , params->TLS_PRF_PARAMS.seed, params->TLS_PRF_PARAMS.ulSeedLen);
    DUMP_HEX("Label" , params->TLS_PRF_PARAMS.label,params->TLS_PRF_PARAMS.ulLabelLen);

    #else

    symKey.key = baseKey->secret_key_simple.value;
    symKey.len = baseKey->secret_key_simple.len;
    mcKey.symKey = &symKey;

    #endif

    /*
     * TLS's PRF is created by applying P_hash to the secret as:
     * PRF(secret, label, seed)  = P_<hash>(secret, label + seed)
     *                           = P_<hash>(secret, hmac_seed)
     *
     * P_<hash>(secret, hmac_seed) = HMAC_hash(secret, A(1) + hmac_seed)
     *                             + HMAC_hash(secret, A(2) + hmac_seed)
     *                             + HMAC_hash(secret, A(3) + hmac_seed)
     *                             + ...
     *
     * A() is defined as:
     *        A(0) = hmac_seed
     *        A(i) = HMAC_hash(secret, A(i-1))
     * P_hash can be iterated as many times as necessary to produce the
     * required quantity of data. For example, if P_SHA256 is being used to create 80 bytes of data,
     * it will have to be iterated three times (through A(3)), creating 96 bytes of output data;
     * the last 16 bytes of the final iteration will then be discarded, leaving 80 bytes of output data.
     *
     */

    unsigned int iteration_count = params->TLS_PRF_PARAMS.ulOutputLen / TLS_PRF_SHA256_LEN;
    unsigned int iteration_difference_count = 0;
    if (params->TLS_PRF_PARAMS. ulOutputLen % TLS_PRF_SHA256_LEN) {
      iteration_difference_count = params->TLS_PRF_PARAMS.ulOutputLen
                                 - iteration_count * TLS_PRF_SHA256_LEN;
      iteration_count++;
    }

    uint8_t A         [ TLS_PRF_SHA256_LEN ];
    uint8_t hmac_out  [ TLS_PRF_SHA256_LEN ];
    uint8_t hmac_seed [ sizeof(params->TLS_PRF_PARAMS.label)
                       +sizeof(params->TLS_PRF_PARAMS.seed)];
    uint8_t h_term_in [ TLS_PRF_SHA256_LEN
                        +sizeof(params->TLS_PRF_PARAMS.label)
                        +sizeof(params->TLS_PRF_PARAMS.seed)];

    size_t hLen      = TLS_PRF_SHA256_LEN;
    uint32_t offset  = 0;

    memset( hmac_seed, 0, sizeof(params->TLS_PRF_PARAMS.label) + sizeof(params->TLS_PRF_PARAMS.seed));
    memset( A,         0, TLS_PRF_SHA256_LEN);

    // hmac_seed = label + seed
    memscpy(hmac_seed,
            sizeof(hmac_seed),
            params->TLS_PRF_PARAMS.label,
            params->TLS_PRF_PARAMS.ulLabelLen);

    memscpy(hmac_seed + params->TLS_PRF_PARAMS.ulLabelLen,
            sizeof(hmac_seed) - params->TLS_PRF_PARAMS.ulLabelLen,
            params->TLS_PRF_PARAMS.seed,
            params->TLS_PRF_PARAMS.ulSeedLen);

    // A(0) = hmac_seed
    uint8_t * A_ptr   = hmac_seed;
    uint32_t  A_len   = params->TLS_PRF_PARAMS.ulLabelLen + params->TLS_PRF_PARAMS.ulSeedLen;

    for (int i = 0; i < iteration_count; ++i ) {
      // Init TL signature with the key
      result = tlApiSignatureInit(&sessionHandle,
                                  &mcKey,
                                  TLAPI_MODE_SIGN,
                                  TLAPI_ALG_HMAC_SHA_256);
      if (!IS_OK(result)) {
        TL_LOG_E("Failed to initialize signature session: %08X",result);
        rv = TL2CK(result);
        break;
      }

      // A(i+1) = HMAC( secret, A(i))
      // Compute  HMAC( secret, A(i))
      // DUMP_HEX("Input A(i)", A_ptr, A_len);
      result   = tlApiSignatureSign(sessionHandle,
                                    A_ptr,
                                    A_len,
                                    A,
                                    &hLen);
      if (!IS_OK(result)) {
        TL_LOG_E("Failed to finalize signature session: %08X",result);
        rv = TL2CK(result);
        break;
      }
      if (hLen != TLS_PRF_SHA256_LEN) {
        TL_LOG_E("Unexpected returned length: %08X",hLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }

      A_ptr = A;
      A_len = hLen;

      // Init TL signature with the key
      result = tlApiSignatureInit(&sessionHandle,
                                  &mcKey,
                                  TLAPI_MODE_SIGN,
                                  TLAPI_ALG_HMAC_SHA_256);
      if (!IS_OK(result)) {
        TL_LOG_E("Failed to initialize signature session: %08X",result);
        rv = TL2CK(result);
        break;
      }

      // Build hmac_seed = A(i+1) + hmac_seed
      memscpy(h_term_in,
              sizeof(h_term_in),
              A,
              hLen);
      memscpy(h_term_in + hLen,
              sizeof(h_term_in) - hLen,
              hmac_seed,
              params->TLS_PRF_PARAMS.ulLabelLen + params->TLS_PRF_PARAMS.ulSeedLen);

      // Compute PRF Term using HMAC
      // Term = HMAC_SHA256(secret, A(i+1) + hmac_seed)
      // DUMP_HEX("H Term Input", h_term_in, hLen + params->TLS_PRF_PARAMS.ulLabelLen + params->TLS_PRF_PARAMS.ulSeedLen);
      result = tlApiSignatureSign( sessionHandle,
                                   h_term_in,
                                   hLen + params->TLS_PRF_PARAMS.ulLabelLen + params->TLS_PRF_PARAMS.ulSeedLen,
                                   hmac_out,
                                   &hLen);
      if (!IS_OK(result)) {
        TL_LOG_E("Failed to finalize signature session: %08X",result);
        rv = TL2CK(result);
        break;
      }
      if (hLen != TLS_PRF_SHA256_LEN) {
        TL_LOG_E("Unexpected returned length: %08X",hLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }

      // Construct output PRE part by part
      if (  (i ==  iteration_count-1)
         && (iteration_difference_count > 0)
         && (iteration_difference_count < TLS_PRF_SHA256_LEN)) {
          memscpy(params->TLS_PRF_PARAMS.output + offset,
                  PRF_OUT_MAX_LEN - offset,
                  hmac_out,
                  iteration_difference_count);
      }else {
          memscpy(params->TLS_PRF_PARAMS.output + offset,
                  PRF_OUT_MAX_LEN - offset,
                  hmac_out,
                  hLen);
      }
      offset += hLen;
      DUMP_HEX("HMAC term", hmac_out,hLen);
    }
    // Check all is good
    if (rv != CKR_OK) {
      break;
    }
    params->TLS_PRF_PARAMS.header.changed = CK_TRUE;
    DUMP_HEX("PRF", params->TLS_PRF_PARAMS.output, params->TLS_PRF_PARAMS.ulOutputLen );
  } while(0);
  TL_LOG_I("tls_pseudorandom_function-: %08X",rv);
  return rv;
}

/*
 * CKM_TLS_PRE_MASTER_KEY_GEN
 * CKM_SSL3_PRE_MASTER_KEY_GEN
 * Is a mechanism which generates a 48-byte generic secret key.
 *
 **/
DECLARE_GENERATE_KEY_FUNC(tls_pre_master_key)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  size_t randomLen = 0;
  TL_LOG_I("tls_pre_master_key+");
  do {
    if (!key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // Mechanism supports only TLS Version 1.2, which uses the version { 3, 3 }
    // Or SSL3 , which uses the version { 3, 0 }
    if (CKM_SSL3_PRE_MASTER_KEY_GEN == key->key.key_gen_mechanism) {
      if( !((params->SSL3_PREMASTER_SECRET.version.major == 3 ) && (params->SSL3_PREMASTER_SECRET.version.minor == 0)) ) {
        TL_LOG_E(" SSL version %d.%d not supported",
        params->SSL3_PREMASTER_SECRET.version.major,
        params->SSL3_PREMASTER_SECRET.version.minor);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    } else if (CKM_TLS_PRE_MASTER_KEY_GEN == key->key.key_gen_mechanism) {
      if( !((params->SSL3_PREMASTER_SECRET.version.major == 3 ) && (params->SSL3_PREMASTER_SECRET.version.minor == 3 )) ) {
        TL_LOG_E(" TLS version %d.%d not supported",
        params->SSL3_PREMASTER_SECRET.version.major,
        params->SSL3_PREMASTER_SECRET.version.minor);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    } else {
      TL_LOG_E(" Wrong Key generation mechanism in Key object %d",key->key.key_gen_mechanism);
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (!IS_TYPE(key,tlp11_type_generic_key)) {
      TL_LOG_E(" Can only generate generic key : Template Inconsistent");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    randomLen = SSL3_PRE_KEY_GEN_LEN;
    result = tlApiRandomGenerateData( TLAPI_ALG_PSEUDO_RANDOM,
                                      key->secret_key_simple.value + SSL3_PRE_KEY_OFFSET,
                                      &randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }

    if (randomLen != SSL3_PRE_KEY_GEN_LEN) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
                randomLen,SSL3_PRE_KEY_GEN_LEN);
      rv = CKR_FUNCTION_FAILED;
      break;
    }

    key->secret_key_simple.value[0] = params->SSL3_PREMASTER_SECRET.version.major;
    key->secret_key_simple.value[1] = params->SSL3_PREMASTER_SECRET.version.minor;
    TL_LOG_I("---TLS_PREMASTER_SECRET---");
    DUMP_HEX("PreKey",key->secret_key_simple.value,key->secret_key_simple.len);
  } while(0);
  TL_LOG_I("tls_pre_master_key-: %08X",rv);
  return rv;
}

/*
 * CKM_TLS_MASTER_KEY_DERIVE
 * This a mechanism is used to derive a 48-byte generic secret key from
 * another 48-byte generic secret key
 *
 **/
DECLARE_DERIVE_FUNC(tls_derive_master_secret)
{
  CK_RV rv = CKR_OK;
  TL_LOG_I("tls_derive_master_secret+");
  do {
    if (!baseKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // sanity check param
    if((params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulClientRandomLen
       > sizeof(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.clientRandom))
     ||(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulServerRandomLen
       > sizeof(params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.serverRandom )) ) {
      TL_LOG_E(" SSL3 params length error! ");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // sanity check Pre Master Secret
    if (!IS_TYPE(baseKey,tlp11_type_generic_key)) {
      TL_LOG_E(" base Key needs to generic key : Template Inconsistent");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    // Check if the key was generated through the right mechanism
    if (baseKey->key.key_gen_mechanism != CKM_TLS_PRE_MASTER_KEY_GEN) {
      TL_LOG_E(" SSL3 Key incorrectly generated ! ");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (!IS_TYPE(newKey,tlp11_type_generic_key)) {
      TL_LOG_E(" Can only generate generic key : Template Inconsistent");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    /* now compute the master key from Pre_Master_Secret, Client_Hello_Random
     * and Server_Hello_Random
     *
     *  master_secret = PRF( pre_master_secret,
     *                       "master secret",
     *                       ClientHello.random + ServerHello.random)
     *
     */
    tlp11_mechanism_param_t prf_params = {0};

    // seed data  = ClientHello.random + ServerHello.random
    memscpy(prf_params.TLS_PRF_PARAMS.seed,
            SEED_MAX_LEN,
            params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.serverRandom,
            params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulServerRandomLen);

    memscpy(prf_params.TLS_PRF_PARAMS.seed + params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulServerRandomLen,
            SEED_MAX_LEN - params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulServerRandomLen,
            params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.clientRandom,
            params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulClientRandomLen);

    prf_params.TLS_PRF_PARAMS.ulSeedLen = params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulServerRandomLen
                                        + params->SSL3_MASTER_KEY_DERIVE_PARAMS.RandomInfo.ulClientRandomLen;
    DUMP_HEX("TLS_PRF_PARAMS.seed",
            prf_params.TLS_PRF_PARAMS.seed,
            prf_params.TLS_PRF_PARAMS.ulSeedLen);

    // label data = "master secret"
    const char *Label = "master secret";
    prf_params.TLS_PRF_PARAMS.ulLabelLen = strlen(Label);
    memscpy (prf_params.TLS_PRF_PARAMS.label,
             LABEL_LEN,
             Label,
             prf_params.TLS_PRF_PARAMS.ulLabelLen);
    DUMP_HEX("TLS_PRF_PARAMS.label",
            prf_params.TLS_PRF_PARAMS.label,
            prf_params.TLS_PRF_PARAMS.ulLabelLen);


    // expected random output bytes
    prf_params.TLS_PRF_PARAMS.ulOutputLen = SSL3_MASTER_SECRET_LEN;

    // baseKey holds the pre master secret
    DUMP_HEX("Secret",
            baseKey->secret_key_simple.value,
            baseKey->secret_key_simple.len);

    // Call PRF function
    rv = tls_pseudorandom_function( mech,
                                    &prf_params,
                                    baseKey,
                                    newKey,
                                    token,
                                    group,
                                    index);
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

    // prf_params.TLS_PRF_PARAMS.output contains master secret
    newKey->secret_key_simple.len = prf_params.TLS_PRF_PARAMS.ulOutputLen;
    memscpy(newKey->secret_key_simple.value,
            SECRET_KEY_MAX_LEN,
            prf_params.TLS_PRF_PARAMS.output,
            newKey->secret_key_simple.len);

    TL_LOG_I("---TLS_MASTER_SECRET---");
    DUMP_HEX("master secret",
            newKey->secret_key_simple.value,
            newKey->secret_key_simple.len);

    params->SSL3_MASTER_KEY_DERIVE_PARAMS.version.major = baseKey->secret_key_simple.value[0];
    params->SSL3_MASTER_KEY_DERIVE_PARAMS.version.minor = baseKey->secret_key_simple.value[1];
    params->SSL3_MASTER_KEY_DERIVE_PARAMS.header.changed = CK_TRUE;
    TL_LOG_I("---TLS_Version_major--- %d",params->SSL3_MASTER_KEY_DERIVE_PARAMS.version.major);
    TL_LOG_I("---TLS_Version_minor--- %d",params->SSL3_MASTER_KEY_DERIVE_PARAMS.version.minor);

  } while (0);
  TL_LOG_I("tls_derive_master_secret-: %08X",rv);
  return rv;
}

/*
 * CKM_TLS_KEY_AND_MAC_DERIVE
 * This mechanism is used to derive cryptographic keying materials used by the Cipher Suite
 * Input  : "master secret" and rondom data
 * OutPut : 4 keys ( Client & Server MAC wtie Keys and ENC write Keys) and 2 IV
 *
 **/
DECLARE_DERIVE_FUNC(tls_derive_key_and_mac)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("tls_derive_key_and_mac+");
  do {
    if (!baseKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (!params) {
      TL_LOG_E(" NULL params!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }

    // sanity check param
    if((params->SSL3_KEY_MAT_PARAMS.RandomInfo.ulServerRandomLen
       > sizeof(params->SSL3_KEY_MAT_PARAMS.RandomInfo.serverRandom))
     ||(params->SSL3_KEY_MAT_PARAMS.RandomInfo.ulClientRandomLen
       > sizeof(params->SSL3_KEY_MAT_PARAMS.RandomInfo.clientRandom)) ) {
      TL_LOG_E(" SSL3 params length error! ");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if((params->SSL3_KEY_MAT_PARAMS.ulIVSizeInBits
       > sizeof(params->SSL3_KEY_MAT_OUT.IVClient))
     ||(params->SSL3_KEY_MAT_PARAMS.ulIVSizeInBits
       > sizeof(params->SSL3_KEY_MAT_OUT.IVServer)) ) {
      TL_LOG_E(" SSL3 params IV output length error! ");
      rv = CKR_DATA_LEN_RANGE;
      break;
    }

    if((params->SSL3_KEY_MAT_PARAMS.ulMacSizeInBits
       > sizeof(((tlp11_container_object_t *)0)->secret_key_simple.value) )
     ||(params->SSL3_KEY_MAT_PARAMS.ulKeySizeInBits
       > sizeof(((tlp11_container_object_t *)0)->secret_key_simple.value) ) ) {
      TL_LOG_E(" SSL3 params Key output length error! ");
      rv = CKR_DATA_LEN_RANGE;
      break;
    }

    // sanity check Pre Master Secret
    if (!IS_TYPE(baseKey,tlp11_type_generic_key)) {
      TL_LOG_E(" base Key needs to generic key : Template Inconsistent");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    if (!IS_TYPE(newKey,tlp11_type_generic_key)) {
      TL_LOG_E(" Can only generate generic key : Template Inconsistent");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    /* Enc keys and MAC keys are generated using the master secret
     * as an entropy source.
     *
     *  key_block = PRF( SecurityParameters.master_secret,
     *                  "key expansion",
     *                  SecurityParameters.server_random +
     *                  SecurityParameters.client_random);
     *
     * To generate the key material, compute Key_block until enough
     * output has been generated. Then, the key_block is partitioned
     * as follows:
     *
     *    client_write_MAC_key [SecurityParameters.mac_key_length]
     *    server_write_MAC_key [SecurityParameters.mac_key_length]
     *    client_write_key     [SecurityParameters.enc_key_length]
     *    server_write_key     [SecurityParameters.enc_key_length]
     *    client_write_IV      [SecurityParameters.fixed_iv_length]
     *    server_write_IV      [SecurityParameters.fixed_iv_length]
     *
     */

    tlp11_mechanism_param_t prf_params = {0};

    // seed data  = server_random + client_random
    memscpy(prf_params.TLS_PRF_PARAMS.seed,
            SEED_MAX_LEN,
            params->SSL3_KEY_MAT_PARAMS.RandomInfo.serverRandom,
            params->SSL3_KEY_MAT_PARAMS.RandomInfo.ulServerRandomLen);

    memscpy(prf_params.TLS_PRF_PARAMS.seed + params->SSL3_KEY_MAT_PARAMS.RandomInfo.ulServerRandomLen,
            SEED_MAX_LEN - params->SSL3_KEY_MAT_PARAMS.RandomInfo.ulServerRandomLen,
            params->SSL3_KEY_MAT_PARAMS.RandomInfo.clientRandom,
            params->SSL3_KEY_MAT_PARAMS.RandomInfo.ulClientRandomLen);

    prf_params.TLS_PRF_PARAMS.ulSeedLen = params->SSL3_KEY_MAT_PARAMS.RandomInfo.ulServerRandomLen
                                        + params->SSL3_KEY_MAT_PARAMS.RandomInfo.ulClientRandomLen;
    DUMP_HEX("TLS_PRF_PARAMS.ulSeedLen",
            prf_params.TLS_PRF_PARAMS.seed,
            prf_params.TLS_PRF_PARAMS.ulSeedLen);

    // label data = "key expansion"
    const char * Label = "key expansion";
    prf_params.TLS_PRF_PARAMS.ulLabelLen = strlen(Label);
    memscpy (prf_params.TLS_PRF_PARAMS.label,
             LABEL_LEN,
             Label,
             prf_params.TLS_PRF_PARAMS.ulLabelLen);
    DUMP_HEX("TLS_PRF_PARAMS.ulLabelLen",
            prf_params.TLS_PRF_PARAMS.label,
            prf_params.TLS_PRF_PARAMS.ulLabelLen);

    /* expected random output bytes = client_write_MAC_key_length
     *                              + server_write_MAC_key_length
     *                              + client_write_key_length
     *                              + server_write_key_length
     *                              + client_write_IV_length
     *                              + server_write_IV_length
     *
     */
    prf_params.TLS_PRF_PARAMS.ulOutputLen = 2*params->SSL3_KEY_MAT_PARAMS.ulMacSizeInBits
                                          + 2*params->SSL3_KEY_MAT_PARAMS.ulKeySizeInBits
                                          + 2*params->SSL3_KEY_MAT_PARAMS.ulIVSizeInBits;

    /* Section 6.3 RFC5246 for TLS1.2
     * (http://datatracker.ietf.org/doc/rfc5246/?include_text=1)
     *
     * Implementation note: The currently defined cipher suite which
     * requires the most material is AES_256_CBC_SHA256.  It requires 2 x 32
     * byte keys and 2 x 32 byte MAC keys, for a total 128 bytes of key material.
     *
     */

    // baseKey holds the master secret
    DUMP_HEX("Secret",
            baseKey->secret_key_simple.value,
            baseKey->secret_key_simple.len);

    // Call PRF function
    rv = tls_pseudorandom_function( mech,
                                    &prf_params,
                                    baseKey,
                                    newKey,
                                    token,
                                    group,
                                    index);
    // Check all is good
    if (rv != CKR_OK) {
      break;
    }

    // construct the output param
    CK_BYTE *out =  prf_params.TLS_PRF_PARAMS.output;
    CK_ULONG mac_key_size = params->SSL3_KEY_MAT_PARAMS.ulMacSizeInBits;
    CK_ULONG enc_key_size = params->SSL3_KEY_MAT_PARAMS.ulKeySizeInBits;
    CK_ULONG iv_size      = params->SSL3_KEY_MAT_PARAMS.ulIVSizeInBits;

    /*
     * --------- out ---------------+
     *    client_write_MAC_key
     * -----------------------------+
     *    server_write_MAC_key
     * -----------------------------+
     *    client_write_key
     * -----------------------------+
     *    server_write_key
     * -----------------------------+
     *    client_write_IV
     * -----------------------------+
     *    server_write_IV
     * -----------------------------+
     */

    tlp11_container_object_t tempobj = {0};

    tempobj.header.version               = 1;
    tempobj.header.object_type           = tlp11_type_generic_key;  /**< final object type */
    tempobj.header.changed               = 0;                       /**< volatile, for internal use only */
    tempobj.header.magic                 = 0;
    tempobj.object.cls                   = CKO_SECRET_KEY;  /**< CKA_CLASS */
    tempobj.storage.token                = CK_FALSE;   /**< CKA_TOKEN */
    tempobj.storage.prvt                 = CK_FALSE;   /**< CKA_PRIVATE */
    tempobj.storage.modifiable           = CK_FALSE;   /**< CKA_MODIFIABLE */
    tempobj.key.key_type                 = CKK_GENERIC_SECRET; /**< CKA_KEY_TYPE */
    tempobj.key.derive                   = CK_TRUE;    /**< CKA_DERIVE */
    tempobj.key.local                    = CK_TRUE;    /**< CKA_LOCAL */
    tempobj.key.key_gen_mechanism        = CKM_TLS_KEY_AND_MAC_DERIVE;     /**< CKA_KEY_GEN_MECHANISM */
    tempobj.secret_key.verify            = CK_FALSE;   /**< CKA_VERIFY */
    tempobj.secret_key.wrap              = CK_FALSE;   /**< CKA_WRAP */
    tempobj.secret_key.sensitive         = CK_FALSE;   /**< CKA_SENSITIVE */
    tempobj.secret_key.decrypt           = CK_FALSE;   /**< CKA_DECRYPT */
    tempobj.secret_key.sign              = CK_FALSE;   /**< CKA_SIGN */
    tempobj.secret_key.unwrap            = CK_FALSE;   /**< CKA_UNWRAP */
    tempobj.secret_key.extractable       = CK_TRUE;    /**< CKA_EXTRACTABLE */
    tempobj.secret_key.always_sensitive  = CK_FALSE;   /**< CKA_ALWAYS_SENSITIVE */
    tempobj.secret_key.never_extractable = CK_FALSE;   /**< CKA_NEVER_EXTRACTABLE */
    tempobj.secret_key.wrap_with_trusted = CK_FALSE;   /**< CKA_WRAP_WITH_TRUSTED */
    tempobj.secret_key.trusted           = CK_FALSE;   /**< CKA_TRUSTED */

    const char *tlsLabel = "TLS_KEY";
    memset(tempobj.storage.label, 0, sizeof(tempobj.storage.label));
    memscpy(tempobj.storage.label, LABEL_LEN, tlsLabel, min(strlen(tlsLabel), sizeof(tempobj.storage.label)));
    tempobj.storage.label_len = min(strlen(tlsLabel), sizeof(tempobj.storage.label));

    memset(tempobj.key.id, 0, sizeof(tempobj.key.id));


    // client_write_MAC_key SO object
    memscpy(&tempobj.secret_key_simple.value, SECRET_KEY_MAX_LEN, out, mac_key_size);
    out += mac_key_size;

    tempobj.secret_key_simple.len = mac_key_size;
    tlsLabel = "client_write_MAC_key";
    memscpy(tempobj.key.id, ID_MAX_LEN, tlsLabel, min(strlen(tlsLabel), sizeof(tempobj.key.id)));
    tempobj.key.id_len = min(strlen(tlsLabel), sizeof(tempobj.key.id));

    result = wrapObject( &tempobj, &params->SSL3_KEY_MAT_OUT.so_clientMacSecret );
    if (!IS_OK(result)) {
      TL_LOG_E("client_write_MAC_key wrapObject ERROR : 0x%08X",result);
      rv = TL2CK(result);
      break;
    }

    // server_write_MAC_key SO object
    memscpy(&tempobj.secret_key_simple.value, SECRET_KEY_MAX_LEN, out, mac_key_size);
    out += mac_key_size;

    tempobj.secret_key_simple.len = mac_key_size;
    tlsLabel = "server_write_MAC_key";
    memscpy(tempobj.key.id, ID_MAX_LEN, tlsLabel, min(strlen(tlsLabel), sizeof(tempobj.key.id)));
    tempobj.key.id_len = min(strlen(tlsLabel), sizeof(tempobj.key.id));

    result = wrapObject( &tempobj, &params->SSL3_KEY_MAT_OUT.so_serverMacSecret );
    if (!IS_OK(result)) {
      TL_LOG_E("server_write_MAC_key wrapObject ERROR : 0x%08X",result);
      rv = TL2CK(result);
      break;
    }

    // client_write_key SO object
    memscpy(&tempobj.secret_key_simple.value, SECRET_KEY_MAX_LEN, out, enc_key_size);
    out += enc_key_size;

    tempobj.secret_key_simple.len = enc_key_size;
    tlsLabel = "client_write_key";
    memscpy(tempobj.key.id, ID_MAX_LEN, tlsLabel, min(strlen(tlsLabel), sizeof(tempobj.key.id)));
    tempobj.key.id_len = min(strlen(tlsLabel), sizeof(tempobj.key.id));

    result = wrapObject( &tempobj, &params->SSL3_KEY_MAT_OUT.so_clientKey );
    if (!IS_OK(result)) {
      TL_LOG_E("client_write_key wrapObject ERROR : 0x%08X",result);
      rv = TL2CK(result);
      break;
    }

    // server_write_key SO object
    memscpy(&tempobj.secret_key_simple.value, SECRET_KEY_MAX_LEN, out, enc_key_size);
    out += enc_key_size;

    tempobj.secret_key_simple.len = enc_key_size;
    tlsLabel = "server_write_key";
    memscpy(tempobj.key.id, ID_MAX_LEN, tlsLabel, min(strlen(tlsLabel), sizeof(tempobj.key.id)));
    tempobj.key.id_len = min(strlen(tlsLabel), sizeof(tempobj.key.id));

    result = wrapObject( &tempobj,&params->SSL3_KEY_MAT_OUT.so_serverKey );
    if (!IS_OK(result)) {
      TL_LOG_E("server_write_key wrapObject ERROR : 0x%08X",result);
      rv = TL2CK(result);
      break;
    }

    // IVClient
    memscpy(params->SSL3_KEY_MAT_OUT.IVClient,
            MD5_HASH_LEN,
            out,
            iv_size);
    out += iv_size;

    //IVServer
    memscpy(params->SSL3_KEY_MAT_OUT.IVServer,
            MD5_HASH_LEN,
            out,
            iv_size);
    params->SSL3_KEY_MAT_OUT.header.changed = CK_TRUE;

  } while (0);
  TL_LOG_I("tls_derive_key_and_mac-: %08X",rv);
  return rv;
}
