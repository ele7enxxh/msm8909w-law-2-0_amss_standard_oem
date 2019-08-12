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

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_CIPHER_UPDATE_FUNC(common_cipher_update) {
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("common_cipher_update+");
  do {
    // just perform the operation
    TL_LOG_I("In:  %d bytes @ 0x%08X",dataInLen,dataIn);
    TL_LOG_I("Out: %d bytes @ 0x%08X",*dataOutLen,dataOut);
    if (session->context.cryptoSession != CR_SID_INVALID) {
      result = CRYPTO_OP(tlApiCipherUpdate(
          session->context.cryptoSession,
          dataIn,
          dataInLen,
          dataOut,
          dataOutLen));
      if (IS_STUBBED(result)) {
        *dataOutLen=min(*dataOutLen,dataInLen);
        memscpy(dataOut,*dataOutLen,dataIn,*dataOutLen);
        result = TLAPI_OK;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
    } else {
      TL_LOG_E("Invalid MC CryptoSession!");
      rv = CKR_OPERATION_NOT_INITIALIZED;
      break;
    }
    TL_LOG_I("In:  %d bytes @ 0x%08X",dataInLen,dataIn);
    TL_LOG_I("Out: %d bytes @ 0x%08X",*dataOutLen,dataOut);
  } while(0);
  TL_LOG_I("common_cipher_update-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_CIPHER_FINAL_FUNC(common_cipher_final) {
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("common_cipher_final+");
  do {
    // just perform the operation
    if (session->context.cryptoSession != CR_SID_INVALID) {
      //TL_LOG_I("Session @ 0x%08X",session);
      //TL_LOG_I("Context @ 0x%08X",&session->context);
      //TL_LOG_I("Key @ 0x%08X",&session->context.key);
      //TL_LOG_I("Asymmetric @ 0x%08X",&session->context.key.asymmetric);
      //TL_LOG_I("mcKey @ 0x%08X",&session->context.key.asymmetric.mcKey);
      //TL_LOG_I("rsaKey @ 0x%08X",&session->context.key.asymmetric.mcKey.rsaKey);
      //TL_LOG_I("Modulus len: %d bytes @ 0x%08X",session->context.key.asymmetric.mcKey.rsaKey->modulusLen,session->context.key.asymmetric.mcKey.rsaKey->modulus);
      //TL_LOG_I("Exponent len: %d bytes @ 0x%08X",session->context.key.asymmetric.mcKey.rsaKey->exponentLen,session->context.key.asymmetric.mcKey.rsaKey->exponent);
      //dumpHex("Exponent",session->context.key.asymmetric.mcKey.rsaKey->exponent,session->context.key.asymmetric.mcKey.rsaKey->exponentLen);
      //dumpHex("Modulus",session->context.key.asymmetric.mcKey.rsaKey->modulus,session->context.key.asymmetric.mcKey.rsaKey->modulusLen);
      TL_LOG_I("In:  %d bytes @ 0x%08X",dataInLen,dataIn);
      TL_LOG_I("Out: %d bytes @ 0x%08X",*dataOutLen,dataOut);
      DUMP_HEX("In",dataIn,dataInLen);
      result = CRYPTO_OP(tlApiCipherDoFinal(
          session->context.cryptoSession,
          dataIn,
          dataInLen,
          dataOut,
          dataOutLen));
      if (IS_STUBBED(result)) {
        *dataOutLen=min(*dataOutLen,dataInLen);
        memscpy(dataOut,*dataOutLen,dataIn,*dataOutLen);
        result = TLAPI_OK;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      TL_LOG_I("In:  %d bytes @ 0x%08X",dataInLen,dataIn);
      TL_LOG_I("Out: %d bytes @ 0x%08X",*dataOutLen,dataOut);
      DUMP_HEX("Out",dataOut,*dataOutLen);
      // call completed ok, session is closed
      session->context.cryptoSession = CR_SID_INVALID;
    } else {
      TL_LOG_E("Invalid MC CryptoSession!");
      rv = CKR_OPERATION_NOT_INITIALIZED;
      break;
    }
  } while(0);
  TL_LOG_I("common_cipher_final-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_SIGN_VERIFY_UPDATE_FUNC(common_sign_verify_update)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("common_sign_verify_update+");
  do {
    // just perform the operation
    if (session->context.cryptoSession != CR_SID_INVALID) {
      TL_LOG_I("In:  %d bytes @ 0x%08X",dataInLen,dataIn);
      DUMP_HEX("In",dataIn,dataInLen);
      result = CRYPTO_OP(tlApiSignatureUpdate(
          session->context.cryptoSession,
          dataIn,
          dataInLen));
      if (IS_STUBBED(result)) {
        result = TLAPI_OK;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiSignatureUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
    } else {
      TL_LOG_E("Invalid MC CryptoSession!");
      rv = CKR_OPERATION_NOT_INITIALIZED;
      break;
    }
  } while(0);
  TL_LOG_I("common_sign_verify_update-: %08X",rv);
  return rv;
}

#ifdef ENABLE_WEAK_DERIVE
/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_DERIVE_FUNC(derive_data_base)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint8_t *src = NULL;
  uint32_t srcLen = 0;
  uint8_t *dst = NULL;
  uint32_t dstLen = 0;
  uint32_t maxKeyLen = 0;
  uint32_t nMaxCopy = 0;
  TL_LOG_I("derive_data_base+");
  do {
    if (!baseKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if ((params == NULL) ||
        (params->KEY_DERIVATION_STRING_DATA.header.valid == 0) ||
        (params->KEY_DERIVATION_STRING_DATA.data == NULL) ||
        (params->KEY_DERIVATION_STRING_DATA.len == 0)) {
      TL_LOG_E(" Wrong parameter!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (CKR_OK != validateBufferLocation(params->KEY_DERIVATION_STRING_DATA.data)) {
      TL_LOG_E("KEY_DERIVATION_STRING_DATA.data ouside valid range!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (mech == CKM_XOR_BASE_AND_DATA) {
      maxKeyLen = min(params->KEY_DERIVATION_STRING_DATA.len,baseKey->secret_key_simple.len);
    } else {
      maxKeyLen = params->KEY_DERIVATION_STRING_DATA.len + baseKey->secret_key_simple.len;
    }
    if (maxKeyLen < newKey->secret_key_simple.len) {
      TL_LOG_E(" Not enough data. Want %d, have %d",newKey->secret_key_simple.len,maxKeyLen);
      rv = CKR_DATA_LEN_RANGE;
      break;
    }
    // get the source
    srcLen = baseKey->secret_key_simple.len;
    src = baseKey->secret_key_simple.value;
    dstLen=newKey->secret_key_simple.len;
    dst=newKey->secret_key_simple.value;
    if (mech == CKM_CONCATENATE_BASE_AND_DATA) {
      nMaxCopy = min(srcLen,dstLen);
      memscpy(dst,dstLen,src,nMaxCopy);
      if (dstLen > nMaxCopy) {
        // go on copying also a part of the data
        memscpy(dst+nMaxCopy,dstLen-nMaxCopy,params->KEY_DERIVATION_STRING_DATA.data,dstLen-nMaxCopy);
      }
    } else if (mech == CKM_CONCATENATE_DATA_AND_BASE) {
      nMaxCopy = min(params->KEY_DERIVATION_STRING_DATA.len,dstLen);
      memscpy(dst,dstLen,params->KEY_DERIVATION_STRING_DATA.data,nMaxCopy);
      if (dstLen > nMaxCopy) {
        // go on copying also a part of the data
        memscpy(dst+nMaxCopy,dstLen-nMaxCopy,src,dstLen-nMaxCopy);
      }
    } else if (mech == CKM_XOR_BASE_AND_DATA) {
      for (unsigned int i=0; i<newKey->secret_key_simple.len; i++) {
        dst[i] = src[i] ^ params->KEY_DERIVATION_STRING_DATA.data[i];
      }
    } else {
      // should never happen
      TL_LOG_E("Unsupported mechanism!");
      rv = CKR_MECHANISM_INVALID;
      break;
    }
    result = computeChecksum(
      newKey->secret_key.check_value,
      dst,
      newKey->secret_key_simple.len,
      newKey->header.object_type);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to compute checksum");
      rv = TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("derive_data_base-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_DERIVE_FUNC(derive_base_key)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint8_t *src = NULL;
  uint32_t srcLen = 0;
  uint8_t *dst = NULL;
  uint32_t dstLen = 0;
  uint32_t maxKeyLen = 0;
  uint32_t nMaxCopy = 0;
  uint32_t access = 0; // RO
  tlp11_container_object_t *freeKey = NULL;
  TL_LOG_I("derive_base_key+");
  do {
    if (!baseKey || !newKey || !token || !group ) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if ((params == NULL) ||
        (params->OBJECT_HANDLE.header.valid == 0)) {
      TL_LOG_E(" Wrong parameter!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    freeKey =(tlp11_container_object_t*)params->OBJECT_HANDLE.so_key.value;
    result = unwrapObject(&params->OBJECT_HANDLE.so_key,freeKey);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapObject: %d",result);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (!IS_TYPE(freeKey,tlp11_type_secret_key)) {
      TL_LOG_E("Passed SO must be a SECRET key: 0x%08X",freeKey->header.object_type);
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
    maxKeyLen = baseKey->secret_key_simple.len + freeKey->secret_key_simple.len;

    if (maxKeyLen < newKey->secret_key_simple.len) {
      TL_LOG_E(" Not enough data!");
      rv = CKR_DATA_LEN_RANGE;
      break;
    }

    // we have to re-evaluate sensitivity and extractability
    if (baseKey->secret_key.sensitive || freeKey->secret_key.sensitive) {
      newKey->secret_key.sensitive = 1;
    }
    if ((baseKey->secret_key.extractable == 0) || (freeKey->secret_key.extractable == 0)) {
      newKey->secret_key.extractable = 0;
    }
    if (baseKey->secret_key.always_sensitive && freeKey->secret_key.always_sensitive) {
      newKey->secret_key.always_sensitive = 1;
    }
    if (baseKey->secret_key.never_extractable && freeKey->secret_key.never_extractable) {
      newKey->secret_key.never_extractable = 1;
    }

    // get the source
    srcLen = baseKey->secret_key_simple.len;
    src = baseKey->secret_key_simple.value;
    dstLen=newKey->secret_key_simple.len;
    dst=newKey->secret_key_simple.value;

    nMaxCopy = min(srcLen,dstLen);
    memscpy(dst,dstLen,src,nMaxCopy);
    if (dstLen > nMaxCopy) {
      // go on copying also a part of the second key
      memscpy(dst+nMaxCopy,dstLen-nMaxCopy,freeKey->secret_key_simple.value,dstLen-nMaxCopy);
    }

    result = computeChecksum(
      newKey->secret_key.check_value,
      dst,
      newKey->secret_key_simple.len,
      newKey->header.object_type);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to compute checksum");
      rv = TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("derive_base_key-: %08X",rv);
  return rv;
}

// this function is implemented to be compliant with NSS, even if
// that wouldn't be my interpretation of the standard!!!
// I would consider b0 to be the least significant bit of the key, not the
// most significant of byte 0.
/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_DERIVE_FUNC(derive_extract)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint8_t *srcByte = 0;
  uint8_t *nxtByte = 0;
  uint8_t *dstByte = 0;
  uint32_t bitShift = 0;
  uint32_t maxKeyLen = 0;
  TL_LOG_I("derive_extract+");
  do {
    if (!baseKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if ((params == NULL) ||
        (params->OBJECT_HANDLE.header.valid == 0)) {
      TL_LOG_E(" Wrong parameter!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }

    maxKeyLen = baseKey->secret_key_simple.len;

    if (maxKeyLen < newKey->secret_key_simple.len) {
      TL_LOG_E(" Not enough data!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }

    if (params->EXTRACT_PARAMS.bitIndex >= newKey->secret_key_simple.len * 8) {
      TL_LOG_E(" Offset exceed key length!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }

    // we have to re-evaluate sensitivity and extractability
    if (baseKey->secret_key.sensitive) {
      newKey->secret_key.sensitive = 1;
    }
    if (baseKey->secret_key.extractable == 0) {
      newKey->secret_key.extractable = 0;
    }
    if (baseKey->secret_key.always_sensitive) {
      newKey->secret_key.always_sensitive = 1;
    }
    if (baseKey->secret_key.never_extractable) {
      newKey->secret_key.never_extractable = 1;
    }

    // position pointers
    srcByte = baseKey->secret_key_simple.value + (params->EXTRACT_PARAMS.bitIndex >> 3);
    bitShift = params->EXTRACT_PARAMS.bitIndex & 0x7;
    dstByte = newKey->secret_key_simple.value;

    for (unsigned int i=0; i<newKey->secret_key_simple.len; i++) {
      // write current byte
      nxtByte = srcByte +1;
      if (nxtByte >= baseKey->secret_key_simple.value + baseKey->secret_key_simple.len) {
        nxtByte = baseKey->secret_key_simple.value;
      }
      if (bitShift)
        *dstByte = (uint8_t)(*srcByte << bitShift) | (*nxtByte >> (8 - bitShift));
      else
        *dstByte = *srcByte;
      srcByte++;
      dstByte++;
      // do we need to wrap around?
      if (srcByte >= baseKey->secret_key_simple.value + baseKey->secret_key_simple.len) {
        srcByte = baseKey->secret_key_simple.value;
      }
    }

    result = computeChecksum(
      newKey->secret_key.check_value,
      newKey->secret_key_simple.value,
      newKey->secret_key_simple.len,
      newKey->header.object_type);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to compute checksum");
      rv = TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("derive_extract-: %08X",rv);
  return rv;
}
#endif
