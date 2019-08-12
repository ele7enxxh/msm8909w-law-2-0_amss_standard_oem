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

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_SIGN_VERIFY_INIT_FUNC(hmac_sign_verify_init)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiSigAlg_t alg = TLAPI_ALG_HMAC_SHA_256;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  tlApiSigMode_t mode;
  TL_LOG_I("hmac_sign_verify_init+");
  do {
    // check the input
    if (!session || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (!IS_TYPE(key,tlp11_type_secret_key)) {
      TL_LOG_E(" Wrong key type");
      rv = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }
    if (IS_TYPE(key,tlp11_type_otp_key)) {
      TL_LOG_E(" OTP keys not allowed for normal signatures");
      rv = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }
    // check the key
    if (operation == CKF_SIGN) {
      mode = TLAPI_MODE_SIGN;
      if (key->secret_key.sign == 0) {
        TL_LOG_E(" Key doesn't support signing");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
    } else if (operation == CKF_VERIFY) {
      if (key->secret_key.verify == 0) {
        TL_LOG_E(" Key doesn't support signature verification");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
      // in case we're doing a "GENERAL" HMAC, a verification implies a signature
      // since MC does not account for signatures shorter then the required length
      if ((mech == CKM_SHA_1_HMAC_GENERAL) || (mech == CKM_SHA256_HMAC_GENERAL)) {
        mode = TLAPI_MODE_SIGN;
      } else {
        mode = TLAPI_MODE_VERIFY;
      }
    } else {
      TL_LOG_E(" Sign function called for operation other than sign/verify: 0x%08X",operation);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    // copy over the key
    session->context.key.symmetric.len = key->secret_key_simple.len;
    memscpy(session->context.key.symmetric.value, AES256_KEY_LEN, key->secret_key_simple.value,key->secret_key_simple.len);
    symKey.key = session->context.key.symmetric.value;
    symKey.len = session->context.key.symmetric.len;
    mcKey.symKey = &symKey;

    switch (mech) {
    case CKM_SHA_1_HMAC:
    case CKM_SHA_1_HMAC_GENERAL:
      alg = TLAPI_ALG_HMAC_SHA1;
      break;
    case CKM_SHA256_HMAC:
    case CKM_SHA256_HMAC_GENERAL:
      alg = TLAPI_ALG_HMAC_SHA_256;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;

    if ((mech == CKM_SHA_1_HMAC_GENERAL) || (mech == CKM_SHA256_HMAC_GENERAL)) {
      if ((params == NULL) || (params->header.valid == 0)) {
        TL_LOG_E(" NULL param input!");
        rv = CKR_GENERAL_ERROR;
        break;
      }
      // check the validity of the parameters
      if ((mech == CKM_SHA_1_HMAC_GENERAL) && (params->MAC_GENERAL_PARAMS.length > SHA1_HASH_LEN)) {
        TL_LOG_E(" Mechanism parameter invalid (SHA1)");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if ((mech == CKM_SHA256_HMAC_GENERAL) && (params->MAC_GENERAL_PARAMS.length > SHA256_HASH_LEN)) {
        TL_LOG_E(" Mechanism parameter invalid (SHA256)");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      session->context.params.mac_general = params->MAC_GENERAL_PARAMS;
    }

    result = CRYPTO_OP(tlApiSignatureInit(
          &session->context.cryptoSession,
          &mcKey,
          mode,
          alg));
    if (IS_STUBBED(result)) {
      session->context.cryptoSession=0x12345678;
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiSignatureInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("hmac_sign_verify_init-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_SIGN_VERIFY_FINAL_FUNC(hmac_sign_verify_final)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("hmac_sign_verify_final+");
  do {
    if (session->context.cryptoSession != CR_SID_INVALID) {
      if (session->context.operation == CKF_SIGN) {
        // we store it in a temporary buffer, since the GENERAL mechanisms
        // will only take a subset of the signature
        uint8_t buffer[SHA256_HASH_LEN] = {0};
        size_t bufferLen = SHA256_HASH_LEN;
        result = CRYPTO_OP(tlApiSignatureSign(
            session->context.cryptoSession,
            dataIn,
            dataInLen,
            buffer,
            &bufferLen));
        if (IS_STUBBED(result)) {
          result = TLAPI_OK;
        }
        if (!IS_OK(result)) {
          TL_LOG_E("tlApiSignatureSign: 0x%08X",result);
          rv=TL2CK(result);
          break;
        }
        // now set the output buffer
        if (dataOut == NULL) {
          TL_LOG_E("NULL output buffer");
          rv = CKR_ARGUMENTS_BAD;
          break;
        }
        if (session->context.mechanism == CKM_SHA_1_HMAC) {
          if (*dataOutLen < SHA1_HASH_LEN) {
            TL_LOG_E("Buffer too small");
            rv = CKR_BUFFER_TOO_SMALL;
            break;
          }
//          if (bufferLen != SHA1_HASH_LEN) {
//            TL_LOG_E("SHA1 unexpected buffer length returned! %d", bufferLen);
//            rv = CKR_GENERAL_ERROR;
//            break;
//          }
          *dataOutLen = SHA1_HASH_LEN;
          memscpy(dataOut,*dataOutLen,buffer,SHA1_HASH_LEN);
        } else if (session->context.mechanism == CKM_SHA256_HMAC) {
          if (*dataOutLen < SHA256_HASH_LEN) {
            TL_LOG_E("Buffer too small");
            rv = CKR_BUFFER_TOO_SMALL;
            break;
          }
//          if (bufferLen != SHA256_HASH_LEN) {
//            TL_LOG_E("SHA1 unexpected buffer returned!");
//            rv = CKR_GENERAL_ERROR;
//            break;
//          }
          *dataOutLen = SHA256_HASH_LEN;
          memscpy(dataOut,*dataOutLen,buffer,SHA256_HASH_LEN);
        } else {
          if (*dataOutLen < session->context.params.mac_general.length) {
            TL_LOG_E("Buffer too small");
            rv = CKR_BUFFER_TOO_SMALL;
            break;
          }
          if (bufferLen < session->context.params.mac_general.length) {
            TL_LOG_E("SHA1/256 unexpected buffer returned!");
            rv = CKR_GENERAL_ERROR;
            break;
          }
          *dataOutLen = session->context.params.mac_general.length;
          memscpy(dataOut,*dataOutLen,buffer,session->context.params.mac_general.length);
        }
      } else {
        // verify
        if ((session->context.mechanism == CKM_SHA_1_HMAC) ||
            (session->context.mechanism == CKM_SHA256_HMAC)) {
          bool validity = 0;
          result = CRYPTO_OP(tlApiSignatureVerify(
              session->context.cryptoSession,
              dataIn,
              dataInLen,
              dataOut,
              *dataOutLen,
              &validity));
          if (IS_STUBBED(result)) {
            result = TLAPI_OK;
          }
          if (!IS_OK(result)) {
            TL_LOG_E("tlApiSignatureVerify: 0x%08X",result);
            rv=TL2CK(result);
            break;
          }
          if (!validity) {
            TL_LOG_I("Signature verified as invalid");
            rv = CKR_SIGNATURE_INVALID;
            break;
          }
        } else {
          // it is actually a sign operation, and we verify the outcome on our own
          uint8_t buffer[SHA256_HASH_LEN] = {0};
          size_t bufferLen = SHA256_HASH_LEN;
          result = CRYPTO_OP(tlApiSignatureSign(
              session->context.cryptoSession,
              dataIn,
              dataInLen,
              buffer,
              &bufferLen));
          if (IS_STUBBED(result)) {
            result = TLAPI_OK;
          }
          if (!IS_OK(result)) {
            TL_LOG_E("tlApiSignatureSign: 0x%08X",result);
            rv=TL2CK(result);
            break;
          }
          if (*dataOutLen < session->context.params.mac_general.length) {
            TL_LOG_E("Signature too small");
            rv = CKR_SIGNATURE_LEN_RANGE;
            break;
          }
          if (bufferLen < session->context.params.mac_general.length) {
            TL_LOG_E("SHA1/256 unexpected buffer returned!");
            rv = CKR_GENERAL_ERROR;
            break;
          }
          if (0 == memcmp(dataOut,buffer,session->context.params.mac_general.length)) {
            rv = CKR_OK;
          } else {
            TL_LOG_I("Signature verified as invalid");
            rv = CKR_SIGNATURE_INVALID;
          }
        }
      }
    } else {
      TL_LOG_E("Invalid MC CryptoSession!");
      rv = CKR_OPERATION_NOT_INITIALIZED;
      break;
    }
  } while(0);
  TL_LOG_I("hmac_sign_verify_final-: %08X",rv);
  return rv;
}
