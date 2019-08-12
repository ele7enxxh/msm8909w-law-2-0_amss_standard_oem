/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "tlLog.h"
#include "tlP11Utils.h"
#include "tlP11Validation.h"
#include "tlP11Mechanisms.h"
#include "tlP11Glob.h"
#include "securityExt.h"
#include "pbkd.h"

#define T_LEN (SECRET_KEY_MAX_LEN+SHA256_HASH_LEN)

static CK_RV pbkdf2(
    uint8_t *password,
    uint32_t passwordLen,
    const uint8_t *salt,
    uint32_t saltLen,
    uint32_t iterationCount, // "c" in the spec
    tlApiSigAlg_t alg,
    uint8_t *key,
    const size_t key_len)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCrSession_t sessionHandle;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};

  do {
    if (!password || !key || !key_len) {
      TL_LOG_E(" NULL input!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (!salt && (saltLen > 0)) {
      TL_LOG_E(" Inconsistent saltLen!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if ((alg != TLAPI_ALG_HMAC_SHA1) && (alg != TLAPI_ALG_HMAC_SHA_256)) {
      TL_LOG_E(" Unsupported HMAC/PRF");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (key_len > SECRET_KEY_MAX_LEN) {
      TL_LOG_E(" Unsupported key length: %d",key_len);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (iterationCount == 0) {
      TL_LOG_E(" iteration count = 0");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    {
      symKey.key = (uint8_t*)password;
      symKey.len = passwordLen;
      mcKey.symKey=&symKey;
      uint8_t T[T_LEN] = {0};
      uint8_t U[T_LEN] = {0};
      uint32_t iMax = 0; // "l" in the spec
      size_t hLen;
      size_t hLenTmp;

      if (alg == TLAPI_ALG_HMAC_SHA1) {
        hLen = SHA1_HASH_LEN;
      }
      else {
        hLen = SHA256_HASH_LEN;
      }
      // the following is to avoid inclusion of aeabi_sdiv
      iMax = 0;
      while((int32_t)(key_len + (hLen >> 1)) - (int32_t)(hLen * iMax) > 0) {
        iMax++;
      }
      iMax--;
      //iMax = (key_len + (hLen / 2)) / hLen;

      TL_LOG_I("hLen=%d", hLen);
      TL_LOG_I("iMax=%d", iMax);

      // evaluate Ti(1)=U1
      for (uint32_t i=0; i<iMax; i++) {
        uint8_t iBE[4] = {0};
        uint8_t buffer[SALT_MAX_LEN+4]={0}; // to accommodate the salt + iBE
        size_t bufferLen = 0;

        // initialize digest session
        result = CRYPTO_OP(tlApiSignatureInit(&sessionHandle,&mcKey,TLAPI_MODE_SIGN,alg));
        if (!IS_OK(result)) {
          TL_LOG_E("Failed to initialize signature session: %08X",result);
          rv = TL2CK(result);
          break;
        }

        if (salt && (saltLen > 0)) {
          memscpy(buffer,sizeof(buffer),salt,saltLen);
          bufferLen = saltLen;
        }

        // a four-octet encoding of the integer i, most significant octet first.
        iBE[0] = (uint8_t) ((i+1) >> 24);
        iBE[1] = (uint8_t) ((i+1) >> 16);
        iBE[2] = (uint8_t) ((i+1) >> 8);
        iBE[3] = (uint8_t) ((i+1));
        memscpy(buffer+bufferLen,sizeof(buffer)-bufferLen,iBE,sizeof(iBE));
        bufferLen += sizeof(iBE);

        hLenTmp = hLen;
        result = CRYPTO_OP(tlApiSignatureSign(sessionHandle, buffer, bufferLen, T+i*hLen, &hLenTmp));
        if (!IS_OK(result)) {
          TL_LOG_E("Failed to finalize signature session: %08X",result);
          rv = TL2CK(result);
          break;
        }
        if (hLenTmp != hLen) {
          TL_LOG_E("Unexpected returned length: %08X",hLenTmp);
          rv = CKR_GENERAL_ERROR;
          break;
        }
        rv = CKR_OK;
      }

      if (rv != CKR_OK) break;

      // special case count = 1
      if(iterationCount == 1) {
        // we return the derived key
        for (uint32_t i = 0; i < key_len; i++) {
          key[i] = T[i];
        }

        break;
      }

      memscpy(U, sizeof(U), T, hLen);

      // we have T1, now iterates till Tc
      // for the number of iterations
      for(uint32_t j=1; (j<iterationCount) && (rv == CKR_OK); j++) {
//        dumpHex("Partial T", T, hLen);

        // per each block
        for(uint32_t i=0; i<iMax; i++) {

          // initialize signature session
          result = CRYPTO_OP(tlApiSignatureInit(&sessionHandle,&mcKey,TLAPI_MODE_SIGN,alg));
          if (!IS_OK(result)) {
            TL_LOG_E("Failed to initialize signature session: %08X",result);
            rv = TL2CK(result);
            break;
          }

          // digest
          hLenTmp = hLen;
          result = CRYPTO_OP(tlApiSignatureSign(sessionHandle, U+i*hLen, hLen, U+i*hLen, &hLenTmp));
          if (!IS_OK(result)) {
            TL_LOG_E("Failed to finalize signature session: %08X",result);
            rv = TL2CK(result);
            break;
          }
          if (hLenTmp != hLen) {
            TL_LOG_E("Unexpected returned length: %08X",hLenTmp);
            rv = CKR_GENERAL_ERROR;
            break;
          }

          // now XOR Ti and Ui
          for(uint32_t k=0; k<hLen; k+=4) {
            *(uint32_t*)(T+i*hLen+k) ^= *(uint32_t*)(U+i*hLen+k);
          }

        }
      }
      for (uint32_t i = 0; i < key_len; i++) {
        key[i] = T[i];
      }
    }


  } while(0);
  TL_LOG_I("pbkdf2-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_GENERATE_KEY_FUNC(pkcs5_pbkd2)
{
  CK_RV rv = CKR_OK;
  tlApiSigAlg_t alg;
  tlApiResult_t result = TLAPI_OK;
  uint8_t *dst;
  size_t dst_len;
  TL_LOG_I("pkcs5_pbkd2+");
  do {
    if (!key || !params) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    TL_LOG_I("pkcs5_pbkd2+ passwordLen=%d, saltLen=%d, iterationCount=%d",
             params->PKCS5_PBKD2_PARAMS.password_len,
             params->PKCS5_PBKD2_PARAMS.salt_len,
             params->PKCS5_PBKD2_PARAMS.iterations);
    TL_LOG_I("pkcs5_pbkd2+ password = %s, salt =%s",
                  params->PKCS5_PBKD2_PARAMS.password,
                  params->PKCS5_PBKD2_PARAMS.salt);

    if (IS_TYPE(key,tlp11_type_otp_key)) {
      dst = key->otp.value;
      dst_len = key->otp.len;
    } else if (IS_TYPE(key,tlp11_type_secret_key)) {
      dst = key->secret_key_simple.value;
      dst_len = key->secret_key_simple.len;
    } else {
      TL_LOG_E(" Can only generate secret keys");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    // check that the buffers in the params look legitimate
    if ((params->PKCS5_PBKD2_PARAMS.password_len > sizeof(params->PKCS5_PBKD2_PARAMS.password)) ||
        (params->PKCS5_PBKD2_PARAMS.salt_len > sizeof(params->PKCS5_PBKD2_PARAMS.salt))) {
      TL_LOG_E(" Parameters inconsistent");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (params->PKCS5_PBKD2_PARAMS.prf == CKP_PKCS5_PBKD2_HMAC_SHA1) {
      alg = TLAPI_ALG_HMAC_SHA1;
    } else if (params->PKCS5_PBKD2_PARAMS.prf == CKP_PKCS5_PBKD2_HMAC_SHA256) {
      alg = TLAPI_ALG_HMAC_SHA_256;
    } else {
      TL_LOG_E(" Unknown PRF");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (weakerKey(g_token_0_ptr)) {
      // if the destination key is greater than 128 bit, deny the use of SHA-1, as per
      // NIST 800-57-Part1-revised2
      if ((dst_len * 8 > 128) && (alg == TLAPI_ALG_HMAC_SHA1)) {
        TL_LOG_E("NIST 800-57-Part1-revised2 SHA1 forbidden for keys longer than 128 bits");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }
    // pointer to the key
    rv = pbkdf2(
        (uint8_t*)(params->PKCS5_PBKD2_PARAMS.password),
        params->PKCS5_PBKD2_PARAMS.password_len,
        params->PKCS5_PBKD2_PARAMS.salt,
        params->PKCS5_PBKD2_PARAMS.salt_len,
        params->PKCS5_PBKD2_PARAMS.iterations,
        alg,
        dst,
        dst_len
        );
    if (rv != CKR_OK) {
      TL_LOG_E("PBKDF2 failed: 0x%08X",rv);
      break;
    }
    // fill the CKA_CHECH_VALUE field
    result = computeChecksum(
        key->secret_key.check_value,
        dst,
        dst_len,
        key->header.object_type);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to compute checksum");
      rv = TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("pkcs5_pbkd2-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_GENERATE_KEY_FUNC(pkcs5_pbkd2_generic_secret)
{
  CK_RV rv = CKR_OK;
  tlApiSigAlg_t alg;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0; // RO
  uint8_t *dst;
  size_t dst_len;
  tlp11_container_object_t *freeKey = NULL;
  TL_LOG_I("pkcs5_pbkd2_generic_secret+");
  do {
    if (!key || !params || !token || !group) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (IS_TYPE(key,tlp11_type_otp_key)) {
      dst = key->otp.value;
      dst_len = key->otp.len;
    } else if (IS_TYPE(key,tlp11_type_secret_key)) {
      dst = key->secret_key_simple.value;
      dst_len = key->secret_key_simple.len;
    } else {
      TL_LOG_E(" Can only generate secret keys");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    // here we deal with the original generic key used as password
    freeKey = (tlp11_container_object_t*)params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.so_key.value;
    result = unwrapObject(&params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.so_key,freeKey);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapObject: %d",result);
      rv = TL2CK(result);
      break;
    }
    if (!IS_TYPE(freeKey,tlp11_type_generic_key)) {
      TL_LOG_E("Passed SO must be a GENERIC_SECRET key: 0x%08X",freeKey->header.object_type);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (freeKey->storage.token) {
      if (freeKey->header.stamp != token->tokenStamp) {
        TL_LOG_E("Object claims to be a token object, but doesn't match the passed token");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    } else { // session object
      rv = CKR_MECHANISM_PARAM_INVALID;
      for(unsigned int i=0; i<token->info.ulMaxSessionCount; i++) {
        if (freeKey->header.stamp == group->sessions[i].sessionStamp) {
          rv = CKR_OK;
          break;
        }
      }
      if (rv != CKR_OK) {
        TL_LOG_E("Object claims to be a session object, but doesn't match any session");
        break;
      }
    }
    // check access, before inspecting the object
    if (freeKey->storage.prvt) {
      access |= SESSION_ACCESS_PRIVATE;
    }
    if (freeKey->storage.token) {
      access |= SESSION_ACCESS_TOKEN;
    }
    rv = checkAccess(token,group,index,access);
    if (rv != CKR_OK) {
      break;
    }
    if (params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.prf == CKP_PKCS5_PBKD2_HMAC_SHA1) {
      alg = TLAPI_ALG_HMAC_SHA1;
    } else if (params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.prf == CKP_PKCS5_PBKD2_HMAC_SHA256) {
      alg = TLAPI_ALG_HMAC_SHA_256;
    } else {
      TL_LOG_E(" Unknown PRF");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (minIterationsPbkdf2SK(g_token_0_ptr)) {
      if (params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.iterations < max(g_config.arg.minIterationsPBKDF2_SK,MIN_PBKDF2_ITERATIONS)) {
        TL_LOG_E(" Not enough iterations specified: %d vs %d",
            params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.iterations,
            max(g_config.arg.minIterationsPBKDF2_SK,MIN_PBKDF2_ITERATIONS));
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    } else {
      if (params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.iterations < MIN_PBKDF2_ITERATIONS) {
        TL_LOG_E(" Not enough iterations specified: %d vs %d",
            params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.iterations,
            MIN_PBKDF2_ITERATIONS);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    }
    if (weakerKey(g_token_0_ptr)) {
      // if the destination key is greater than 128 bit, deny the use of SHA-1, as per
      // NIST 800-57-Part1-revised2
      if ((dst_len * 8 > 128) && (alg == TLAPI_ALG_HMAC_SHA1)) {
        TL_LOG_E("NIST 800-57-Part1-revised2 SHA1 forbidden for keys longer than 128 bits");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }
    if (params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.salt_len < SALT_MIN_LEN) {
      TL_LOG_E(" Salt too short: %d",params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.salt_len);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    // pointer to the key
    rv = pbkdf2(
        freeKey->secret_key_simple.value,
        freeKey->secret_key_simple.len,
        params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.salt,
        params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.salt_len,
        params->PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS.iterations,
        alg,
        dst,
        dst_len
        );
    if (rv != CKR_OK) {
      TL_LOG_E("PBKDF2 failed: 0x%08X",rv);
      break;
    }
    // fill the CKA_CHECH_VALUE field
    result = computeChecksum(
        key->secret_key.check_value,
        dst,
        dst_len,
        key->header.object_type);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to compute checksum");
      rv = TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("pkcs5_pbkd2_generic_secret-: %08X",rv);
  return rv;
}
