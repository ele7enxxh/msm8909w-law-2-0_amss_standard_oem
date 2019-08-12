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
#include "asn1.h"

DECLARE_CIPHER_INIT_FUNC(aes_cipher_init) {
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_AES_256_CBC_PKCS7;
  tlApiCipherMode_t mode;
  TL_LOG_I("aes_cipher_init+");
  do {
    // check the input
    if (!session || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    // check the key
    if (!IS_TYPE(key,tlp11_type_aes_key)) {
      TL_LOG_E(" NULL input!");
      rv = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }
    if (operation == CKF_ENCRYPT) {
      if (key->secret_key.encrypt == 0) {
        TL_LOG_E(" Key doesn't support encryption");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
      mode = TLAPI_MODE_ENCRYPT;
    } else if (operation == CKF_DECRYPT) {
      if (key->secret_key.decrypt == 0) {
        TL_LOG_E(" Key doesn't support decryption");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
      mode = TLAPI_MODE_DECRYPT;
    } else {
      TL_LOG_E(" Cipher function called for operation other than cipher: 0x%08X",operation);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    switch (mech) {
    case CKM_AES_ECB:
      if (key->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_ECB_NOPAD;
      else
        alg = TLAPI_ALG_AES_256_ECB_NOPAD;
      break;
    case CKM_AES_CBC:
      if (key->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_CBC_NOPAD;
      else
        alg = TLAPI_ALG_AES_256_CBC_NOPAD;
      break;
    case CKM_AES_CBC_PAD:
      if (key->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_CBC_PKCS7;
      else
        alg = TLAPI_ALG_AES_256_CBC_PKCS7;
      break;
    case CKM_AES_CTR:
      if (key->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_CTR_NOPAD;
      else
        alg = TLAPI_ALG_AES_256_CTR_NOPAD;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;
    // prepare to encrypt
    // copy over the key
    session->context.key.symmetric.len = key->secret_key_simple.len;
    memscpy(session->context.key.symmetric.value, AES256_KEY_LEN, key->secret_key_simple.value,key->secret_key_simple.len);
    session->context.key.symmetric.symKey.key = session->context.key.symmetric.value;
    session->context.key.symmetric.symKey.len = session->context.key.symmetric.len;
    session->context.key.symmetric.mcKey.symKey = &session->context.key.symmetric.symKey;
    if ((mech == CKM_AES_CBC) || (mech == CKM_AES_CBC_PAD)) {
      if (!params || (params->header.valid == 0)) {
        TL_LOG_E(" NULL input!");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if (params->IV_PARAMS.iv_len != 16) {
        TL_LOG_E(" Bad IV length: %d",params->IV_PARAMS.iv_len);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      // we need IV
      DUMP_HEX("Key",session->context.key.symmetric.mcKey.symKey->key,session->context.key.symmetric.mcKey.symKey->len);
      DUMP_HEX("IV",params->IV_PARAMS.iv,params->IV_PARAMS.iv_len);
      result = CRYPTO_OP(tlApiCipherInitWithData(
          &session->context.cryptoSession,
          alg,
          mode,
          &session->context.key.symmetric.mcKey,
          params->IV_PARAMS.iv,
          params->IV_PARAMS.iv_len));
    } else if (mech == CKM_AES_CTR) {
      if (!params || (params->header.valid == 0)) {
        TL_LOG_E(" NULL input!");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if ( params->AES_CTR_PARAMS.ulCounterBits > 128) {
        TL_LOG_E(" Bad Counter block length: %d",params->AES_CTR_PARAMS.ulCounterBits);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if (params->AES_CTR_PARAMS.pCb == NULL_PTR) {
        TL_LOG_E("CB Null pointer Error");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      } else {
        DUMP_HEX("Key", session->context.key.symmetric.mcKey.symKey->key,session->context.key.symmetric.mcKey.symKey->len);
        DUMP_HEX("CTR", params->AES_CTR_PARAMS.pCb,sizeof(((CK_AES_CTR_PARAMS *)(0))->cb));
        result = CRYPTO_OP(tlApiCipherInitWithData(
            &session->context.cryptoSession,
            alg,
            mode,
            &session->context.key.symmetric.mcKey,
            params->IV_PARAMS.iv,
            params->IV_PARAMS.iv_len));
      }
    } else {
      // no IV
      result = CRYPTO_OP(tlApiCipherInit(
          &session->context.cryptoSession,
          alg,
          mode,
          &session->context.key.symmetric.mcKey));
    }
    if (IS_STUBBED(result)) {
      session->context.cryptoSession=0x12345678;
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    // save operation
  } while(0);
  TL_LOG_I("aes_cipher_init-: %08X",rv);
  return rv;
}

/** Process a GenerateKey command
 */
/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_GENERATE_KEY_FUNC(aes_generate)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  size_t randomLen = 0;
  TL_LOG_I("aes_generate+");
  do {
    if (!key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (!IS_TYPE(key,tlp11_type_aes_key)) {
      TL_LOG_E(" Can only generate AES keys");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    // done for good, fill the content
    // STUB result = tlApiRandomGenerateData(TLAPI_ALG_SECURE_RANDOM,(uint8_t*)(key->secret_key.value),key->secret_key.len);
    randomLen = key->secret_key_simple.len;
    result = tlApiRandomGenerateData(TLAPI_ALG_PSEUDO_RANDOM,(uint8_t*)(key->secret_key_simple.value),&randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }
    if (randomLen != key->secret_key_simple.len) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,key->secret_key_simple.len);
      rv = CKR_FUNCTION_FAILED;
      break;
    }
    // fill the CKA_CHECH_VALUE field
    result = computeChecksum(
        key->secret_key.check_value,
        key->secret_key_simple.value,
        key->secret_key_simple.len,
        tlp11_type_aes_key);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to compute checksum");
      rv = TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("aes_generate-: %08X",rv);
  return rv;
}

/*lint -e{818}*/
DECLARE_WRAP_FUNC(aes_wrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_AES_256_CBC_PKCS7;
  tlApiCrSession_t sessionHandle;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  TL_LOG_I("aes_wrap+");
  do {
    if (!wrappingKey || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    switch (mech) {
    case CKM_AES_ECB:
      if (wrappingKey->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_ECB_NOPAD;
      else
        alg = TLAPI_ALG_AES_256_ECB_NOPAD;
      break;
    case CKM_AES_CBC:
      if (wrappingKey->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_CBC_NOPAD;
      else
        alg = TLAPI_ALG_AES_256_CBC_NOPAD;
      break;
    case CKM_AES_CBC_PAD:
      if (wrappingKey->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_CBC_PKCS7;
      else
        alg = TLAPI_ALG_AES_256_CBC_PKCS7;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;

    if (IS_TYPE(key,tlp11_type_rsa_private_key)) {
      // cannot be wrapped by mechanisms not supporting padding!
      if (mech != CKM_AES_CBC_PAD) {
        TL_LOG_E("Private key cannot be wrapped with this mechanism");
        rv = CKR_KEY_NOT_WRAPPABLE;
        break;
      }
    }

    /**
     * Get pointers to the destination. Here there are 2 big different cases.
     * We might be dealing with a private key or a symmetric key. Start with the
     * symmetric.
     */

    // prepare to encrypt
    symKey.key = wrappingKey->secret_key_simple.value;
    symKey.len = wrappingKey->secret_key_simple.len;
    mcKey.symKey = &symKey;
    //DUMP_HEX("Key",symKey.key,symKey.len);
    if ((mech == CKM_AES_CBC) || (mech == CKM_AES_CBC_PAD)) {
      // we need IV
      if (!params || (params->header.valid == 0)) {
        TL_LOG_E(" NULL input!");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if (params->IV_PARAMS.iv_len != 16) {
        TL_LOG_E(" Bad IV length: %d",params->IV_PARAMS.iv_len);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      //DUMP_HEX("IV",params->IV_PARAMS.iv,params->IV_PARAMS.iv_len);
      //TL_LOG_I("alg = 0x%08X",alg);
      result = CRYPTO_OP(tlApiCipherInitWithData(
          &sessionHandle,
          alg,
          TLAPI_MODE_ENCRYPT,
          &mcKey,
          params->IV_PARAMS.iv,
          params->IV_PARAMS.iv_len));
    } else {
      // no IV
      result = CRYPTO_OP(tlApiCipherInit(&sessionHandle,alg,TLAPI_MODE_ENCRYPT,&mcKey));
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    rv = completeWrapping(
        sessionHandle,
        key,
        wrappedData,
        wrappedLen);
  } while(0);
  TL_LOG_I("aes_wrap-: %08X",rv);
  return rv;
}

DECLARE_UNWRAP_FUNC(aes_unwrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_AES_256_CBC_PKCS7;
  tlApiCrSession_t sessionHandle;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  TL_LOG_I("aes_unwrap+");
  do {
    if (!unwrappingKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    switch (mech) {
    case CKM_AES_ECB:
      if (unwrappingKey->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_ECB_NOPAD;
      else
        alg = TLAPI_ALG_AES_256_ECB_NOPAD;
      break;
    case CKM_AES_CBC:
      if (unwrappingKey->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_CBC_NOPAD;
      else
        alg = TLAPI_ALG_AES_256_CBC_NOPAD;
      break;
    case CKM_AES_CBC_PAD:
      if (unwrappingKey->secret_key_simple.len == 16)
        alg = TLAPI_ALG_AES_128_CBC_PKCS7;
      else
        alg = TLAPI_ALG_AES_256_CBC_PKCS7;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;

    if (IS_TYPE(newKey,tlp11_type_rsa_private_key)) {
      if (mech != CKM_AES_CBC_PAD) {
        TL_LOG_E("Private key cannot be unwrapped with this mechanism");
        rv = CKR_KEY_NOT_WRAPPABLE;
        break;
      }
    }

    /**
     * Get pointers to the destination. Here there are 2 big different cases.
     * We might be dealing with a private key or a symmetric key. Start with the
     * symmetric.
     */

    // prepare to decrypt
    symKey.key = unwrappingKey->secret_key_simple.value;
    symKey.len = unwrappingKey->secret_key_simple.len;
    mcKey.symKey = &symKey;
    //DUMP_HEX("Key",symKey.key,symKey.len);
    if ((mech == CKM_AES_CBC) || (mech == CKM_AES_CBC_PAD)) {
      // we need IV
      if (!params || (params->header.valid == 0)) {
        TL_LOG_E(" NULL input!");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if (params->IV_PARAMS.iv_len != 16) {
        TL_LOG_E(" Bad IV length: %d",params->IV_PARAMS.iv_len);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      //DUMP_HEX("IV",params->IV_PARAMS.iv,params->IV_PARAMS.iv_len);
      //TL_LOG_I("alg = 0x%08X",alg);
      result = CRYPTO_OP(tlApiCipherInitWithData(
          &sessionHandle,
          alg,
          TLAPI_MODE_DECRYPT,
          &mcKey,
          params->IV_PARAMS.iv,
          params->IV_PARAMS.iv_len));
    } else {
      // no IV
      result = CRYPTO_OP(tlApiCipherInit(&sessionHandle,alg,TLAPI_MODE_DECRYPT,&mcKey));
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    rv = completeUnwrapping(
        sessionHandle,
        newKey,
        wrappedData,
        wrappedLen);
  } while(0);
  TL_LOG_I("aes_unwrap-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_DERIVE_FUNC(aes_encrypt_data)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCrSession_t sessionHandle = CR_SID_INVALID;
  tlApiCipherAlg_t alg;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  uint8_t *src = NULL;
  uint8_t *dst = NULL;
  uint32_t dstLen = 0;
  uint8_t block[AES_BLOCK_SIZE] = {0}; // this is our output block
  size_t blockLen = 0;
  uint32_t nUsefulBlocks = 0;
  uint32_t nTotalBlocks = 0;
  uint32_t maxKeyLen = 0;
  unsigned int i = 0; // block counter during encryption, for input
  unsigned int j = 0; // offset encryption, for output
  TL_LOG_I("aes_encrypt_data+");
  do {
    if (!baseKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (!IS_TYPE(baseKey,tlp11_type_aes_key)) {
      TL_LOG_E(" Unsupported key type!");
      rv = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }
    // checks on parameters
    if (mech == CKM_AES_ECB_ENCRYPT_DATA) {
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
      maxKeyLen = params->KEY_DERIVATION_STRING_DATA.len;
      if (maxKeyLen < newKey->secret_key_simple.len) {
        TL_LOG_E(" Not enough data!");
        rv = CKR_DATA_LEN_RANGE;
        break;
      }
      // input must be multiple of AES block size
      if ((maxKeyLen & 0xF) != 0) {
        TL_LOG_E(" Len not multiple of block size!");
        rv = CKR_DATA_LEN_RANGE;
        break;
      }
    } else {
      if ((params == NULL) ||
          (params->AES_CBC_ENCRYPT_DATA_PARAMS.header.valid == 0) ||
          (params->AES_CBC_ENCRYPT_DATA_PARAMS.data == NULL) ||
          (params->AES_CBC_ENCRYPT_DATA_PARAMS.len == 0)) {
        TL_LOG_E(" Wrong parameter!");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if (CKR_OK != validateBufferLocation(params->AES_CBC_ENCRYPT_DATA_PARAMS.data)) {
        TL_LOG_E("AES_CBC_ENCRYPT_DATA_PARAMS.data ouside valid range!");
        rv = CKR_ARGUMENTS_BAD;
        break;
      }
      maxKeyLen = params->AES_CBC_ENCRYPT_DATA_PARAMS.len;
      if (maxKeyLen < newKey->secret_key_simple.len) {
        TL_LOG_E(" Not enough data!");
        rv = CKR_DATA_LEN_RANGE;
        break;
      }
      // input must be multiple of AES block size
      if ((maxKeyLen & 0xF) != 0) {
        TL_LOG_E(" Len not multiple of block size!");
        rv = CKR_DATA_LEN_RANGE;
        break;
      }
    }
    dstLen=newKey->secret_key_simple.len;
    dst=newKey->secret_key_simple.value;
    // set pointers to the key
    symKey.key = baseKey->secret_key_simple.value;
    symKey.len = baseKey->secret_key_simple.len;
    mcKey.symKey = &symKey;
    // how many blocks are actually needed to fill the key value?
    // nUsefulBlocks = (dstLen + AES_BLOCK_SIZE) / AES_BLOCK_SIZE;
    if (dstLen & 0xF) {
      nUsefulBlocks = (dstLen + AES_BLOCK_SIZE) >> AES_BLOCK_SIZE_SHIFT;
    } else {
      nUsefulBlocks = (dstLen) >> AES_BLOCK_SIZE_SHIFT;
    }
    //nTotalBlocks = maxKeyLen / AES_BLOCK_SIZE;
    nTotalBlocks = maxKeyLen >> AES_BLOCK_SIZE_SHIFT;
    // based on mechanism, initialize the operation
    if (mech == CKM_AES_ECB_ENCRYPT_DATA) {
      if (baseKey->secret_key_simple.len == 16) {
        alg = TLAPI_ALG_AES_128_ECB_NOPAD;
      } else {
        alg = TLAPI_ALG_AES_256_ECB_NOPAD;
      }
      src=params->KEY_DERIVATION_STRING_DATA.data;
      result = CRYPTO_OP(tlApiCipherInit(
            &sessionHandle,
            alg,
            TLAPI_MODE_ENCRYPT,
            &mcKey));
    } else {
      if (baseKey->secret_key_simple.len == 16) {
        alg = TLAPI_ALG_AES_128_CBC_NOPAD;
      } else {
        alg = TLAPI_ALG_AES_256_CBC_NOPAD;
      }
      src=params->AES_CBC_ENCRYPT_DATA_PARAMS.data;
      result = CRYPTO_OP(tlApiCipherInitWithData(
            &sessionHandle,
            alg,
            TLAPI_MODE_ENCRYPT,
            &mcKey,
            params->AES_CBC_ENCRYPT_DATA_PARAMS.iv,
            sizeof(params->AES_CBC_ENCRYPT_DATA_PARAMS.iv)
            ));
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    for (i=0; i<=(nTotalBlocks-nUsefulBlocks); i++) {
      blockLen = AES_BLOCK_SIZE;
#ifndef AES_HAS_UPDATE
      // temporary hack, due to the lack of Update function in AES implementation
      result = CRYPTO_OP(tlApiCipherDoFinal(
          sessionHandle,
          src + i*AES_BLOCK_SIZE,
          AES_BLOCK_SIZE,
          block,
          &blockLen));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      if (blockLen != AES_BLOCK_SIZE) {
        TL_LOG_E("tlApiCipherUpdate returned a bad blocksize! %d",blockLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }
      // and reinitialize it
      if (mech == CKM_AES_ECB_ENCRYPT_DATA) {
        result = CRYPTO_OP(tlApiCipherInit(
            &sessionHandle,
            alg,
            TLAPI_MODE_ENCRYPT,
            &mcKey));
      } else {
        result = CRYPTO_OP(tlApiCipherInitWithData(
            &sessionHandle,
            alg,
            TLAPI_MODE_ENCRYPT,
            &mcKey,
            block,
            blockLen
            ));
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherInit: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
#else
      result = CRYPTO_OP(tlApiCipherUpdate(
          sessionHandle,
          src + i*AES_BLOCK_SIZE,
          AES_BLOCK_SIZE,
          block,
          &blockLen));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      if (blockLen != AES_BLOCK_SIZE) {
        TL_LOG_E("tlApiCipherUpdate returned a bad blocksize! %d",blockLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }
#endif
    }
    // the last encrypted block is a particular case, since we might need a part of it
    blockLen = (dstLen & 0xF); //this is the extent we need it
    if (blockLen == 0) {
      // we need it all
      memscpy(dst,dstLen,block,AES_BLOCK_SIZE);
      j += AES_BLOCK_SIZE;
    } else {
      memscpy(dst,dstLen,block+AES_BLOCK_SIZE-blockLen,blockLen);
      j += blockLen;
    }
    // all the following now will go straight into the destination
    for (i++; (i<nTotalBlocks) && (j+AES128_KEY_LEN<=dstLen); i++) {
      blockLen = AES_BLOCK_SIZE;
#ifndef AES_HAS_UPDATE
      // temporary hack, due to the lack of Update function in AES implementation
      result = CRYPTO_OP(tlApiCipherDoFinal(
          sessionHandle,
          src + i*AES_BLOCK_SIZE,
          AES_BLOCK_SIZE,
          block,
          &blockLen));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      if (blockLen != AES_BLOCK_SIZE) {
        TL_LOG_E("tlApiCipherUpdate returned a bad blocksize! %d",blockLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }
      // and reinitialize it
      if (mech == CKM_AES_ECB_ENCRYPT_DATA) {
        result = CRYPTO_OP(tlApiCipherInit(
            &sessionHandle,
            alg,
            TLAPI_MODE_ENCRYPT,
            &mcKey));
      } else {
        result = CRYPTO_OP(tlApiCipherInitWithData(
            &sessionHandle,
            alg,
            TLAPI_MODE_ENCRYPT,
            &mcKey,
            block,
            blockLen
            ));
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherInit: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
#else
      result = STUB(tlApiCipherUpdate(
          sessionHandle,
          src + i*AES_BLOCK_SIZE,
          AES_BLOCK_SIZE,
          block,
          &blockLen));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      if (blockLen != AES_BLOCK_SIZE) {
        TL_LOG_E("tlApiCipherUpdate returned a bad blocksize! %d",blockLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }
#endif
      memscpy(dst+j,dstLen-j,block,AES_BLOCK_SIZE);
      j += AES_BLOCK_SIZE;
    }
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
  CRYPTO_OP(tlApiCrAbort(sessionHandle));
  TL_LOG_I("aes_ecb_encrypt_data-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_WRAP_FUNC(wrapkey_aes_cbc_pad_wrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg;
  tlApiSigAlg_t algSig;
  tlApiCrSession_t sessionHandle = CR_SID_INVALID;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  uint8_t *src = NULL;
  uint32_t src_len = 0;
  uint8_t keyBuf[1300] = {0};
  size_t keyBufLen = sizeof(keyBuf);
  uint8_t kmac[AES256_KEY_LEN] = {0};
  uint32_t kmac_len = AES256_KEY_LEN;
  size_t mac_len = SHA256_HASH_LEN;
  size_t eKmac_len = AES256_KEY_LEN;
  size_t randomLen = 0;
  TL_LOG_I("wrapkey_aes_cbc_pad_wrap+");
  do {
    if (!wrappingKey || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    // this works only with a 256 bit wrapping key
    if (wrappingKey->secret_key_simple.len != 32) {
      TL_LOG_E(" Wrapping key must be 256 bit");
      rv = CKR_WRAPPING_KEY_SIZE_RANGE;
      break;
    }
    alg = TLAPI_ALG_AES_256_CBC_PKCS7;
    // proceed as usual in the key encryption, and save a pointer to the key itself
    // since we'll need it for the HMAC. We also need to compute the attribute flag
    // prepare to encrypt
    symKey.key = wrappingKey->secret_key_simple.value;
    symKey.len = wrappingKey->secret_key_simple.len;
    //DUMPHEX("Enc key",wrappingKey->secret_key.value,wrappingKey->secret_key.len);
    mcKey.symKey = &symKey;
    if (!params || (params->header.valid == 0)) {
      TL_LOG_E(" NULL input!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    // generate IV
    randomLen = sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv);
    result = tlApiRandomGenerateData(
        TLAPI_ALG_PSEUDO_RANDOM,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.iv,
        &randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }
    if (randomLen != sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv)) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv));
      rv = CKR_FUNCTION_FAILED;
      break;
    }
    //DUMP_HEX("Key",mcKey.symKey->key,mcKey.symKey->len);
    //DUMP_HEX("IV",params->WRAPKEY_AES_CBC_PAD_PARAMS.iv,sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv));
    result = CRYPTO_OP(tlApiCipherInitWithData(
        &sessionHandle,
        alg,
        TLAPI_MODE_ENCRYPT,
        &mcKey,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.iv,
        sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv)));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    params->WRAPKEY_AES_CBC_PAD_PARAMS.flags = 0;
    if (key->key.derive)
      params->WRAPKEY_AES_CBC_PAD_PARAMS.flags = CKF_DERIVE;
    if (IS_TYPE(key,tlp11_type_secret_key)) {
      // we can write straight to the key
      if (IS_TYPE(key,tlp11_type_otp_key)) {
        src_len = key->otp.len;
        src = key->otp.value;
      } else {
        src_len = key->secret_key_simple.len;
        src = key->secret_key_simple.value;
      }
      memscpy(keyBuf,sizeof(keyBuf),src,src_len);
      keyBufLen=src_len;
      if (key->secret_key.encrypt)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_ENCRYPT;
      if (key->secret_key.decrypt)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_DECRYPT;
      if (key->secret_key.wrap)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_WRAP;
      if (key->secret_key.unwrap)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_UNWRAP;
      if (key->secret_key.sign)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_SIGN;
      if (key->secret_key.verify)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_VERIFY;
    } else if (IS_TYPE(key,tlp11_type_rsa_private_key)) {
      // encode the content
      rv = cryptoValidateRSAPrivateKey(&key->rsa_private_key);
      if (rv != CKR_OK) {
        TL_LOG_E("Key failed validation!");
        break;
      }
      //DUMPHEX("Modulus",key->rsa_private_key.modulus,key->rsa_private_key.modulus_len);
      //DUMPHEX("Pub Exp",key->rsa_private_key.public_exponent,key->rsa_private_key.public_exponent_len);
      //DUMPHEX("Exponent",key->rsa_private_key.exponent,key->rsa_private_key.exponent_len);
      result = DER_encode_PrivateKeyInfo(&key->rsa_private_key,keyBuf,&keyBufLen);
      if (!IS_OK(result)) {
        TL_LOG_E("DER_encode_PrivateKeyInfo: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
      src_len = keyBufLen;
      src = keyBuf;
      if (key->private_key.decrypt)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_DECRYPT;
      if (key->private_key.unwrap)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_UNWRAP;
      if (key->private_key.sign)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_SIGN;
      if (key->private_key.sign_recovery)
        params->WRAPKEY_AES_CBC_PAD_PARAMS.flags |= CKF_SIGN_RECOVER;
    } else {
      TL_LOG_E("Only private and secret keys can be wrapped!");
      rv = CKR_KEY_NOT_WRAPPABLE;
      break;
    }
    //DUMP_HEX("Enc input",src,src_len);
    result = CRYPTO_OP(tlApiCipherDoFinal(
        sessionHandle,
        src,
        src_len,
        wrappedData,
        wrappedLen));
    if (IS_STUBBED(result)) {
      *wrappedLen = min(src_len,*wrappedLen);
      memscpy(wrappedData,WRAPPED_DATA_MAX_LEN,src,*wrappedLen);
      result = TLAPI_OK;
    }
    if (result == E_TLAPI_BUFFER_TOO_SMALL) {
      TL_LOG_E("Buffer too small, return error");
      *wrappedLen=WRAPPED_DATA_MAX_LEN;
      rv = CKR_BUFFER_TOO_SMALL;
      break;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }
    //DUMP_HEX("Enc output",wrappedData,*wrappedLen);
    // now proceed to fill up the content of the structure

    // generate the key for HMAC
    randomLen = kmac_len;
    result = tlApiRandomGenerateData(TLAPI_ALG_PSEUDO_RANDOM,kmac,&randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }
    if (randomLen != kmac_len) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,kmac_len);
      rv = CKR_FUNCTION_FAILED;
      break;
    }
    // HMAC-SHA256 [key + flags + wrapping_key]
    if (keyBufLen > ((sizeof(keyBuf) - wrappingKey->secret_key_simple.len) - sizeof(CK_ULONG))) {
      TL_LOG_E("No room left in buffer to append the flags!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    memscpy(keyBuf+keyBufLen,sizeof(keyBuf)-keyBufLen,&params->WRAPKEY_AES_CBC_PAD_PARAMS.flags,sizeof(CK_ULONG));
    keyBufLen += sizeof(CK_ULONG);
    memscpy(keyBuf+keyBufLen,sizeof(keyBuf)-keyBufLen,wrappingKey->secret_key_simple.value,wrappingKey->secret_key_simple.len);
    keyBufLen += wrappingKey->secret_key_simple.len;
    algSig = TLAPI_ALG_HMAC_SHA_256;
    symKey.key = kmac;
    symKey.len = kmac_len;
    mcKey.symKey = &symKey;
    result = CRYPTO_OP(tlApiSignatureInit(
          &sessionHandle,
          &mcKey,
          TLAPI_MODE_SIGN,
          algSig));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiSignatureInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    mac_len = sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.mac);
    result = CRYPTO_OP(tlApiSignatureSign(
        sessionHandle,
        keyBuf,
        keyBufLen,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.mac,
        &mac_len));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiSignatureSign: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    if (mac_len != sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.mac)) {
      TL_LOG_E(" Bad returned size for HMAC! %d vs %d",
          mac_len,
          sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.mac));
      rv=TL2CK(result);
      break;
    }

    // last piece: encrypt the kmac with the wrapping key
    symKey.key = wrappingKey->secret_key_simple.value;
    symKey.len = wrappingKey->secret_key_simple.len;
    mcKey.symKey = &symKey;
    result = CRYPTO_OP(tlApiCipherInitWithData(
        &sessionHandle,
        alg,
        TLAPI_MODE_ENCRYPT,
        &mcKey,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.iv,
        sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv)));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    eKmac_len = sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac);
    TL_LOG_I("eKmac_len: %d", eKmac_len);
    result = CRYPTO_OP(tlApiCipherDoFinal(
        sessionHandle,
        kmac,
        kmac_len,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac,
        &eKmac_len));
    if (IS_STUBBED(result)) {
      *wrappedLen = min(src_len,*wrappedLen);
      memscpy(wrappedData,WRAPPED_DATA_MAX_LEN,src,*wrappedLen);
      result = TLAPI_OK;
    }
    if (result == E_TLAPI_BUFFER_TOO_SMALL) {
      TL_LOG_E("Buffer too small, return error");
      *wrappedLen=WRAPPED_DATA_MAX_LEN;
      rv = CKR_BUFFER_TOO_SMALL;
      break;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }
    if (eKmac_len != sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac)) {
      TL_LOG_E(" Bad returned size for eKmac! %d vs %d",
          eKmac_len,
          sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac));
      rv=TL2CK(result);
      break;
    }
    //done!
    params->header.changed = 1;
  } while(0);
  CRYPTO_OP(tlApiCrAbort(sessionHandle));
  TL_LOG_I("wrapkey_aes_cbc_pad_wrap-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_UNWRAP_FUNC(wrapkey_aes_cbc_pad_unwrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg;
  tlApiSigAlg_t algSig;
  tlApiCrSession_t sessionHandle = CR_SID_INVALID;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  uint8_t *dst = NULL;
  size_t dst_len = 0;
  uint8_t keyBuf[1300] = {0};
  size_t keyBufLen = sizeof(keyBuf);
  uint8_t kmac[AES256_KEY_LEN+AES_BLOCK_SIZE] = {0};
  size_t kmac_len = sizeof(kmac);
  bool validity = 0;
  TL_LOG_I("wrapkey_aes_cbc_pad_unwrap+");
  do {
    if (!unwrappingKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    // this works only with a 256 bit unwrapping key
    if (unwrappingKey->secret_key_simple.len != 32) {
      TL_LOG_E(" Wrapping key must be 256 bit");
      rv = CKR_UNWRAPPING_KEY_SIZE_RANGE;
      break;
    }
    alg = TLAPI_ALG_AES_256_CBC_PKCS7;
    // proceed as usual in the key decryption, and save a pointer to the key itself
    // since we'll need it for the HMAC. We also need to compute the attribute flag
    // prepare to decrypt
    symKey.key = unwrappingKey->secret_key_simple.value;
    symKey.len = unwrappingKey->secret_key_simple.len;
    mcKey.symKey = &symKey;
    if (!params || (params->header.valid == 0)) {
      TL_LOG_E(" NULL input!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    DUMP_HEX("Key",mcKey.symKey->key,mcKey.symKey->len);
    DUMP_HEX("IV",params->WRAPKEY_AES_CBC_PAD_PARAMS.iv,sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv));
    result = CRYPTO_OP(tlApiCipherInitWithData(
        &sessionHandle,
        alg,
        TLAPI_MODE_DECRYPT,
        &mcKey,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.iv,
        sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv)));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    newKey->key.derive = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_DERIVE) != 0);
    if (IS_TYPE(newKey,tlp11_type_secret_key)) {
      newKey->secret_key.encrypt = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_ENCRYPT) != 0);
      newKey->secret_key.decrypt = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_DECRYPT) != 0);
      newKey->secret_key.wrap = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_WRAP) != 0);
      newKey->secret_key.unwrap = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_UNWRAP) != 0);
      newKey->secret_key.sign = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_SIGN) != 0);
      newKey->secret_key.verify = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_VERIFY) != 0);
    } else if (IS_TYPE(newKey,tlp11_type_rsa_private_key)) {
      newKey->private_key.decrypt = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_DECRYPT) != 0);
      newKey->private_key.unwrap = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_UNWRAP) != 0);
      newKey->private_key.sign = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_SIGN) != 0);
      newKey->private_key.sign_recovery = ((params->WRAPKEY_AES_CBC_PAD_PARAMS.flags & CKF_SIGN_RECOVER) != 0);
    } else {
      // this should never happen!
      TL_LOG_E("Unsupported target key!");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    dst_len = keyBufLen;
    dst = keyBuf;
    // now decrypt
    DUMP_HEX("Input",wrappedData,wrappedLen);
    result = CRYPTO_OP(tlApiCipherDoFinal(
        sessionHandle,
        wrappedData,
        wrappedLen,
        dst,
        &dst_len));
    if (IS_STUBBED(result)) {
      dst_len = min(dst_len,wrappedLen);
      memscpy(dst,dst_len,wrappedData,dst_len);
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    DUMP_HEX("Output",dst,dst_len);
    keyBufLen = dst_len;
    if (IS_TYPE(newKey,tlp11_type_secret_key)) {
      if (IS_TYPE(newKey,tlp11_type_otp_key)) {
        if (keyBufLen > sizeof(newKey->otp.value)) {
          TL_LOG_E("Wrong size, key was NOT an OTP key!");
          rv = CKR_TEMPLATE_INCONSISTENT;
          break;
        }
        newKey->otp.len = keyBufLen;
        memscpy(newKey->otp.value,OTP_KEY_MAX_LEN,keyBuf,keyBufLen);
      } else {
        if (keyBufLen > sizeof(newKey->secret_key_simple.value)) {
          TL_LOG_E("Wrong size, key was NOT a secret key!");
          rv = CKR_TEMPLATE_INCONSISTENT;
          break;
        }
        newKey->secret_key_simple.len = keyBufLen;
        memscpy(newKey->secret_key_simple.value,SECRET_KEY_MAX_LEN,keyBuf,keyBufLen);
      }
      // we need to evaluate checksum
      result = computeChecksum(
        newKey->secret_key.check_value,
        keyBuf,
        keyBufLen,
        newKey->header.object_type);
      if (!IS_OK(result)) {
        TL_LOG_E(" Failed to compute checksum");
        rv = TL2CK(result);
        break;
      }
    } else if (IS_TYPE(newKey,tlp11_type_rsa_private_key)) {
      // we need to decode the ASN.1 buffer into the key
      newKey->rsa_private_key.modulus_len=MODULUS_MAX_LEN;
      newKey->rsa_private_key.public_exponent_len=PUBLIC_EXPONENT_LEN;
      newKey->rsa_private_key.exponent_len=PRIVATE_EXPONENT_MAX_LEN;
      newKey->rsa_private_key.prime1_len=PRIME1_MAX_LEN;
      newKey->rsa_private_key.prime2_len=PRIME2_MAX_LEN;
      newKey->rsa_private_key.exponent1_len=EXPONENT1_MAX_LEN;
      newKey->rsa_private_key.exponent2_len=EXPONENT2_MAX_LEN;
      newKey->rsa_private_key.coefficient_len=COEFFICIENT_MAX_LEN;
      result = DER_decode_PrivateKeyInfo(
          dst,
          dst_len,
          &newKey->rsa_private_key
          );
      if (!IS_OK(result)) {
        TL_LOG_E("DER_decode_PrivateKeyInfo: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
      //DUMPHEX("Modulus",newKey->rsa_private_key.modulus,newKey->rsa_private_key.modulus_len);
      //DUMPHEX("Pub Exp",newKey->rsa_private_key.public_exponent,newKey->rsa_private_key.public_exponent_len);
      //DUMPHEX("Exponent",newKey->rsa_private_key.exponent,newKey->rsa_private_key.exponent_len);
      rv = cryptoValidateRSAPrivateKey(&newKey->rsa_private_key);
      if (rv != CKR_OK) {
        TL_LOG_E("New key failed validation!");
        break;
      }
    }
    // now decrypt kmac
    //DUMPHEX("Key",mcKey.symKey->key,mcKey.symKey->len);
    //DUMPHEX("IV",params->WRAPKEY_AES_CBC_PAD_PARAMS.iv,sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv));
    result = CRYPTO_OP(tlApiCipherInitWithData(
        &sessionHandle,
        alg,
        TLAPI_MODE_DECRYPT,
        &mcKey,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.iv,
        sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.iv)));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    // now decrypt
    kmac_len = sizeof(kmac);
    //DUMPHEX("eKmac",params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac,sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac));
    result = CRYPTO_OP(tlApiCipherDoFinal(
        sessionHandle,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac,
        sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac),
        kmac,
        &kmac_len));
    //DUMPHEX("kmac",kmac,kmac_len);
    if (IS_STUBBED(result)) {
      kmac_len = min(kmac_len,sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac));
      memscpy(kmac,sizeof(kmac),params->WRAPKEY_AES_CBC_PAD_PARAMS.eKmac,kmac_len);
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (kmac_len != AES256_KEY_LEN) {
      TL_LOG_E("Bad kmac length from decryption!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    // verify the signature
    // HMAC-SHA256 [key + flags + wrapping_key]
    if (keyBufLen > ((sizeof(keyBuf) - unwrappingKey->secret_key_simple.len) - sizeof(CK_ULONG))) {
      TL_LOG_E("No room left in buffer to append the flags!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    memscpy(keyBuf+keyBufLen,sizeof(keyBuf)-keyBufLen,&params->WRAPKEY_AES_CBC_PAD_PARAMS.flags,sizeof(CK_ULONG));
    keyBufLen += sizeof(CK_ULONG);
    memscpy(keyBuf+keyBufLen,sizeof(keyBuf)-keyBufLen,unwrappingKey->secret_key_simple.value,unwrappingKey->secret_key_simple.len);
    keyBufLen += unwrappingKey->secret_key_simple.len;
    algSig = TLAPI_ALG_HMAC_SHA_256;
    symKey.key = kmac;
    symKey.len = kmac_len;
    mcKey.symKey = &symKey;
    TL_LOG_I("KeyBufLen = %d",keyBufLen);
    result = CRYPTO_OP(tlApiSignatureInit(
          &sessionHandle,
          &mcKey,
          TLAPI_MODE_VERIFY,
          algSig));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiSignatureInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    // at last the wrapping key and get the result
    result = CRYPTO_OP(tlApiSignatureVerify(
        sessionHandle,
        keyBuf,
        keyBufLen,
        params->WRAPKEY_AES_CBC_PAD_PARAMS.mac,
        sizeof(params->WRAPKEY_AES_CBC_PAD_PARAMS.mac),
        &validity));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiSignatureVerify: 0x%08X",result);
      rv=CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (!validity) {
      TL_LOG_E("HMAC verification failed!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    //done!
  } while(0);
  CRYPTO_OP(tlApiCrAbort(sessionHandle));
  TL_LOG_I("wrapkey_aes_cbc_pad_unwrap-: %08X",rv);
  return rv;
}
