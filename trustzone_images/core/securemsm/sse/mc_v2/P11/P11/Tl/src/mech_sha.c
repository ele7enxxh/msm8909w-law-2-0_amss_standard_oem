/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlP11Utils.h"
#include "tlP11Validation.h"
#include "tlLog.h"
#include "tlP11Mechanisms.h"
#include "securityExt.h"
#include "tlP11Glob.h"

/** Process a DeriveKey command
 */
/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_DERIVE_FUNC(sha_derive)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCrSession_t sessionHandle;
  uint8_t *src = NULL;
  size_t srcLen = 0;
  uint8_t *dst = NULL;
  size_t dstLen = 0;
  uint8_t buffer[SHA256_HASH_LEN];
  size_t bufferLen = SHA256_HASH_LEN;
  tlApiMdAlg_t alg;
  TL_LOG_I("sha_derive+");
  do {
    if (!baseKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (mech == CKM_SHA1_KEY_DERIVATION) {
      alg = TLAPI_ALG_SHA1;
      bufferLen = SHA1_HASH_LEN;
    } else if (mech == CKM_SHA256_KEY_DERIVATION) {
      alg = TLAPI_ALG_SHA256;
      bufferLen = SHA256_HASH_LEN;
    } else {
      TL_LOG_E("Unsupported mechanism: 0x%08X",mech);
      rv = CKR_MECHANISM_INVALID;
      break;
    }
    if (baseKey->secret_key.always_sensitive == 0) {
      newKey->secret_key.always_sensitive = 0;
    } else {
      newKey->secret_key.always_sensitive = newKey->secret_key.sensitive;
    }
    if (baseKey->secret_key.never_extractable == 0) {
      newKey->secret_key.never_extractable = 0;
    } else {
      if (newKey->secret_key.extractable == 1)
        newKey->secret_key.never_extractable = 0;
      else
        newKey->secret_key.never_extractable = 1;
    }

    if (weakerKey(g_token_0_ptr)) {
      // if the destination key is greater than 128 bit, deny the use of SHA-1, as per
      // NIST 800-57-Part1-revised2
      if ((newKey->secret_key_simple.len * 8 > 128) && (alg == TLAPI_ALG_SHA1)) {
        TL_LOG_E("NIST 800-57-Part1-revised2 SHA1 forbidden for keys longer than 128 bits");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }

    // get the source
    srcLen = baseKey->secret_key_simple.len;
    src = baseKey->secret_key_simple.value;

    // generate the digest and put it in the temporary buffer
    result = tlApiMessageDigestInit(&sessionHandle,alg);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiMessageDigestInit: %d",result);
      break;
    }
    result = tlApiMessageDigestDoFinal(
        sessionHandle,
        src,
        srcLen,
        buffer,
        &bufferLen);
    if (IS_STUBBED(result)) {
      memscpy(buffer,sizeof(buffer),src,min(srcLen,bufferLen));
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiMessageDigestDoFinal: %d",result);
      break;
    }

    // check length for the key we're going to generate
    if (newKey->secret_key_simple.len > bufferLen) {
      rv = CKR_KEY_SIZE_RANGE;
      break;
    }
    dstLen=newKey->secret_key_simple.len;
    dst=newKey->secret_key_simple.value;

    memscpy(dst,dstLen,buffer,dstLen);
    result = computeChecksum(
      newKey->secret_key.check_value,
      dst,
      dstLen,
      newKey->header.object_type);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to compute checksum");
      rv = TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("sha_derive-: %08X",rv);
  return rv;
}

DECLARE_DIGEST_INIT_FUNC(sha_digest_init)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiMdAlg_t alg;
  TL_LOG_I("sha_digest_init+");
  do {
    if (mech == CKM_SHA_1) {
      alg = TLAPI_ALG_SHA1;
    } else if (mech == CKM_SHA256) {
      alg = TLAPI_ALG_SHA256;
    } else {
      TL_LOG_E("Unsupported mechanism: 0x%08X",mech);
      rv = CKR_MECHANISM_INVALID;
      break;
    }
    result = tlApiMessageDigestInit(
          &session->context.cryptoSession,
          alg);
    if (IS_STUBBED(result)) {
      session->context.cryptoSession=0x12345678;
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiMessageDigestInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    TL_LOG_I("Initialized cryptoSession: 0x%08X",session->context.cryptoSession);
  } while(0);
  TL_LOG_I("sha_digest_init-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_DIGEST_UPDATE_FUNC(sha_digest_update)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("sha_digest_update+");
  TL_LOG_I("Using cryptoSession: 0x%08X",session->context.cryptoSession);
  TL_LOG_I("In:  0x%08X, %d bytes",dataIn,dataInLen);

  do {
    result = tlApiMessageDigestUpdate(
          session->context.cryptoSession,
          dataIn,
          dataInLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiMessageDigestUpdate: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("sha_digest_update-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_DIGEST_FINAL_FUNC(sha_digest_final)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("sha_digest_final+");
  TL_LOG_I("Using cryptoSession: 0x%08X",session->context.cryptoSession);
  TL_LOG_I("In:  0x%08X, %d bytes",dataIn,dataInLen);
  TL_LOG_I("Out: 0x%08X, %d bytes",dataOut,*dataOutLen);
  do {
    result = tlApiMessageDigestDoFinal(
          session->context.cryptoSession,
          dataIn,
          dataInLen,
          dataOut,
          dataOutLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiMessageDigestDoFinal: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    if (session->context.mechanism == CKM_SHA_1) {
      *dataOutLen = SHA1_HASH_LEN;
    } else {
      *dataOutLen = SHA256_HASH_LEN;
    }
    DUMP_HEX("SHA Out",dataOut,*dataOutLen);
  } while(0);
  TL_LOG_I("sha_digest_final-: %08X",rv);
  return rv;
}
