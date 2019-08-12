/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
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
#include "asn1.h"

/* Returns 0 for even parity and 1 for odd parity for a supplied uint8_t */
#define PARITY(x) ((((x)>>7 & 0x01) + \
                    ((x)>>6 & 0x01) + \
                    ((x)>>5 & 0x01) + \
                    ((x)>>4 & 0x01) + \
                    ((x)>>3 & 0x01) + \
                    ((x)>>2 & 0x01) + \
                    ((x)>>1 & 0x01) + \
                    ((x)>>0 & 0x01)) % 2)

static uint8_t oddParityLookupTable[256];
static bool    oddParityLookupTableInit = FALSE;

#define DES_NUMBER_OF_WEAK_KEYS 64

// Weak key information taken from
//  http://en.wikipedia.org/wiki/Weak_key#Weak_keys_in_DES
//  http://csrc.nist.gov/publications/nistpubs/800-67/SP800-67.pdf
static const uint8_t desWeakKeys[DES_NUMBER_OF_WEAK_KEYS][DES_KEY_LEN] = {
  // Weak keys
  {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
  {0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE},
  {0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E},
  {0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F},
  // Semi-weak keys
  {0x01, 0xfe, 0x01, 0xfe, 0x01, 0xfe, 0x01, 0xfe},
  {0xfe, 0x01, 0xfe, 0x01, 0xfe, 0x01, 0xfe, 0x01},
  {0x1f, 0xe0, 0x1f, 0xe0, 0x0e, 0xf1, 0x0e, 0xf1},
  {0xe0, 0x1f, 0xe0, 0x1f, 0xf1, 0x0e, 0xf1, 0x0e},
  {0x01, 0xe0, 0x01, 0xe0, 0x01, 0xf1, 0x01, 0xf1},
  {0xe0, 0x01, 0xe0, 0x01, 0xf1, 0x01, 0xf1, 0x01},
  {0x1f, 0xfe, 0x1f, 0xfe, 0x0e, 0xfe, 0x0e, 0xfe},
  {0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x0e, 0xfe, 0x0e},
  {0x01, 0x1f, 0x01, 0x1f, 0x01, 0x0e, 0x01, 0x0e},
  {0x1f, 0x01, 0x1f, 0x01, 0x0e, 0x01, 0x0e, 0x01},
  {0xe0, 0xfe, 0xe0, 0xfe, 0xf1, 0xfe, 0xf1, 0xfe},
  {0xfe, 0xe0, 0xfe, 0xe0, 0xfe, 0xf1, 0xfe, 0xf1},
  // Possibly weak keys
  {0x01, 0x01, 0x1F, 0x1F, 0x01, 0x01, 0x0E, 0x0E},
  {0x1F, 0x1F, 0x01, 0x01, 0x0E, 0x0E, 0x01, 0x01},
  {0xE0, 0xE0, 0x1F, 0x1F, 0xF1, 0xF1, 0x0E, 0x0E},
  {0x01, 0x01, 0xE0, 0xE0, 0x01, 0x01, 0xF1, 0xF1},
  {0x1F, 0x1F, 0xE0, 0xE0, 0x0E, 0x0E, 0xF1, 0xF1},
  {0xE0, 0xE0, 0xFE, 0xFE, 0xF1, 0xF1, 0xFE, 0xFE},
  {0x01, 0x01, 0xFE, 0xFE, 0x01, 0x01, 0xFE, 0xFE},
  {0x1F, 0x1F, 0xFE, 0xFE, 0x0E, 0x0E, 0xFE, 0xFE},
  {0xE0, 0xFE, 0x01, 0x1F, 0xF1, 0xFE, 0x01, 0x0E},
  {0x01, 0x1F, 0x1F, 0x01, 0x01, 0x0E, 0x0E, 0x01},
  {0x1F, 0xE0, 0x01, 0xFE, 0x0E, 0xF1, 0x01, 0xFE},
  {0xE0, 0xFE, 0x1F, 0x01, 0xF1, 0xFE, 0x0E, 0x01},
  {0x01, 0x1F, 0xE0, 0xFE, 0x01, 0x0E, 0xF1, 0xFE},
  {0x1F, 0xE0, 0xE0, 0x1F, 0x0E, 0xF1, 0xF1, 0x0E},
  {0xE0, 0xFE, 0xFE, 0xE0, 0xF1, 0xFE, 0xFE, 0xF1},
  {0x01, 0x1F, 0xFE, 0xE0, 0x01, 0x0E, 0xFE, 0xF1},
  {0x1F, 0xE0, 0xFE, 0x01, 0x0E, 0xF1, 0xFE, 0x01},
  {0xFE, 0x01, 0x01, 0xFE, 0xFE, 0x01, 0x01, 0xFE},
  {0x01, 0xE0, 0x1F, 0xFE, 0x01, 0xF1, 0x0E, 0xFE},
  {0x1F, 0xFE, 0x01, 0xE0, 0x0E, 0xFE, 0x01, 0xF1},
  {0xFE, 0x01, 0x1F, 0xE0, 0xFE, 0x01, 0x0E, 0xF1},
  {0x01, 0xE0, 0x1F, 0xFE, 0x01, 0xF1, 0xF1, 0x0E},
  {0x1F, 0xFE, 0xE0, 0x01, 0x0E, 0xFE, 0xF0, 0x01},
  {0xFE, 0x1F, 0x01, 0xE0, 0xFE, 0x0E, 0x01, 0xF1},
  {0x01, 0xE0, 0xE0, 0x01, 0x01, 0xF1, 0xF1, 0x01},
  {0x1F, 0xFE, 0xFE, 0x1F, 0x0E, 0xFE, 0xFE, 0x0E},
  {0xFE, 0x1F, 0xE0, 0x01, 0xFE, 0x0E, 0xF1, 0x01},
  {0x01, 0xE0, 0xFE, 0x1F, 0x01, 0xF1, 0xFE, 0x0E},
  {0xE0, 0x01, 0x01, 0xE0, 0xF1, 0x01, 0x01, 0xF1},
  {0xFE, 0x1F, 0x1F, 0xFE, 0xFE, 0x0E, 0x0E, 0xFE},
  {0x01, 0xFE, 0x1F, 0xE0, 0x01, 0xFE, 0x0E, 0xF1},
  {0xE0, 0x01, 0x1F, 0xFE, 0xF1, 0x01, 0x0E, 0xFE},
  {0xFE, 0xE0, 0x01, 0x1F, 0xFE, 0xF1, 0x01, 0x0E},
  {0x01, 0xFE, 0xE0, 0x1F, 0x01, 0xFE, 0xF1, 0x0E},
  {0xE0, 0x01, 0xFE, 0x1F, 0xF1, 0x01, 0xFE, 0x0E},
  {0xFE, 0xE0, 0x1F, 0x01, 0xFE, 0xF1, 0x0E, 0x01},
  {0x01, 0xFE, 0xFE, 0x01, 0x01, 0xFE, 0xFE, 0x01},
  {0xE0, 0x1F, 0x01, 0xFE, 0xF1, 0x0E, 0x01, 0xFE},
  {0xFE, 0xE0, 0xE0, 0xFE, 0xFE, 0xF1, 0xF1, 0xFE},
  {0x1F, 0x01, 0x01, 0x1F, 0x0E, 0x01, 0x01, 0x0E},
  {0xE0, 0x1F, 0x1F, 0xE0, 0xF1, 0x0E, 0x0E, 0xF1},
  {0xFE, 0xFE, 0x01, 0x01, 0xFE, 0xFE, 0x01, 0x01},
  {0x1F, 0x01, 0xE0, 0xFE, 0x0E, 0x01, 0xF1, 0xFE},
  {0xE0, 0x1F, 0xFE, 0x01, 0xF1, 0x0E, 0xFE, 0x01},
  {0xFE, 0xFE, 0x1F, 0x1F, 0xFE, 0xFE, 0x0E, 0x0E},
  {0x1F, 0x01, 0xFE, 0xE0, 0x0E, 0x01, 0xFE, 0xF1},
  {0xE0, 0xE0, 0x01, 0x01, 0xF1, 0xF1, 0x01, 0x01},
  {0xFE, 0xFE, 0xE0, 0xE0, 0xFE, 0xFE, 0xF1, 0xF1}
};

static void initOddParityLookupTable(void)
{
  int i;

  for(i = 0;i < 256;i++)
  {
    if(!PARITY(i)) {
      if(i & 1) {
        oddParityLookupTable[i] = (uint8_t)(i & 0xFE);
      } else {
        oddParityLookupTable[i] = (uint8_t)(i | 0x01);
      }
    } else {
      oddParityLookupTable[i] = (uint8_t)(i);
    }
  }
  oddParityLookupTableInit = true;
}

static void desSetOddParity(uint8_t *desKey)
{
  int i;

  if(false == oddParityLookupTableInit) {
    initOddParityLookupTable();
  }

  for(i = 0;i < DES_KEY_LEN;i++) {
    *(desKey + i) = oddParityLookupTable[*(desKey + i)];
  }
}

static CK_RV desCheckOddParity(const uint8_t *desKey)
{
  CK_RV rv = CKR_OK;
  int   i;

  if(false == oddParityLookupTableInit) {
    initOddParityLookupTable();
  }

  for(i = 0;i < DES_KEY_LEN;i++) {
    if(*(desKey + i) != oddParityLookupTable[*(desKey + i)]) {
      rv = CKR_GENERAL_ERROR;
    }
  }
  return rv;
}

static CK_RV desCheckForWeakKey(const uint8_t *desKey)
{
  CK_RV rv = CKR_OK;
  int   i;

  for(i = 0;i < DES_NUMBER_OF_WEAK_KEYS;i++) {
    if(0 == timesafe_memcmp(desWeakKeys[i], desKey, DES_KEY_LEN)) {
      // Generated key matches one of the known weak keys
      rv = CKR_GENERAL_ERROR;
      break;
    }
  }
  return rv;
}

DECLARE_CIPHER_INIT_FUNC(des_cipher_init) {
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_3DES_CBC_PKCS5;
  tlApiCipherMode_t mode;
  uint32_t desKeyLen = 0;
  uint32_t desKeyOffset = 0;
  TL_LOG_I("des_cipher_init+");
  do {

    // check the input
    if (!session || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    // check the key
    if (!IS_TYPE(key,tlp11_type_des_key) && !IS_TYPE(key,tlp11_type_des3_key)) {
      TL_LOG_E(" Key type inconsistent!");
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
    case CKM_DES_CBC:
      alg = TLAPI_ALG_DES_CBC_NOPAD;
      break;
    case CKM_DES_CBC_PAD:
      alg = TLAPI_ALG_DES_CBC_PKCS5;
      break;
    case CKM_DES3_CBC:
      alg = TLAPI_ALG_3DES_CBC_NOPAD;
      break;
    case CKM_DES3_CBC_PAD:
      alg = TLAPI_ALG_3DES_CBC_PKCS5;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK)
      break;

    // Set the key length
    desKeyLen = IS_TYPE(key,tlp11_type_des_key) ? DES_KEY_LEN : DES3_KEY_LEN;

    do
    {
      // Check the parity on the key
      rv = desCheckOddParity(key->secret_key_simple.value + desKeyOffset);
      if(CKR_OK != rv) {
        TL_LOG_E(" Parity error on DES key");
        break;
      }

      // Increment the key offset to the next key
      desKeyOffset += DES_KEY_LEN;

    } while(desKeyOffset < desKeyLen);
    if (CKR_OK != rv) break;

    // MobiCore TL API implementation of 3DES uses 2 keys not 3
    // DES3-E( {K1,K2,K1}, P ) = E( K1, D( K2, E( K1, P ) ) )
    // DES3-D( {K1,K2,K1}, C ) = D( K1, E( K2, D( K1, P ) ) )
    // To comply with PKCS 11 standard we will make K1 = K3
    // and correct the Key length in the session info to 2
    //desKeyLen = IS_TYPE(key,tlp11_type_des3_key) ? (DES_KEY_LEN * 2) : desKeyLen;

    // Prepare to encrypt, copy over the key
    session->context.key.symmetric.len = desKeyLen;
    memscpy(session->context.key.symmetric.value, AES256_KEY_LEN, key->secret_key_simple.value,desKeyLen);
    session->context.key.symmetric.symKey.key = session->context.key.symmetric.value;
    session->context.key.symmetric.symKey.len = session->context.key.symmetric.len;
    session->context.key.symmetric.mcKey.symKey = &session->context.key.symmetric.symKey;
    if (!params || (params->header.valid == 0)) {
      TL_LOG_E(" NULL input!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (params->IV_PARAMS.iv_len != 8) {
      TL_LOG_E(" Bad IV length: %d",params->IV_PARAMS.iv_len);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    result = CRYPTO_OP(tlApiCipherInitWithData(
        &session->context.cryptoSession,
        alg,
        mode,
        &session->context.key.symmetric.mcKey,
        params->IV_PARAMS.iv,
        params->IV_PARAMS.iv_len));
    if (IS_STUBBED(result)) {
      session->context.cryptoSession=0x12345678;
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("des_cipher_init-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_WRAP_FUNC(des_wrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_3DES_CBC_PKCS5;
  tlApiCrSession_t sessionHandle;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  uint32_t desKeyLen = 0;
  uint32_t desKeyOffset = 0;
  TL_LOG_I("des_wrap+");
  do {
    if (!wrappingKey || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // Check the mechanism and set the algorithm
    switch (mech) {
    case CKM_DES_CBC:
      alg = TLAPI_ALG_DES_CBC_NOPAD;
      break;
    case CKM_DES_CBC_PAD:
      alg = TLAPI_ALG_DES_CBC_PKCS5;
      break;
    case CKM_DES3_CBC:
      alg = TLAPI_ALG_3DES_CBC_NOPAD;
      break;
    case CKM_DES3_CBC_PAD:
      alg = TLAPI_ALG_3DES_CBC_PKCS5;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (rv != CKR_OK)
      break;

    if (IS_TYPE(key,tlp11_type_rsa_private_key)) {
      // cannot be wrapped by mechanisms not supporting padding!
      if ((mech != CKM_DES_CBC_PAD) && (mech != CKM_DES3_CBC_PAD)) {
        TL_LOG_E("Private key cannot be wrapped with this mechanism");
        rv = CKR_KEY_NOT_WRAPPABLE;
        break;
      }
    }

    desKeyLen = IS_TYPE(wrappingKey,tlp11_type_des_key) ? DES_KEY_LEN : DES3_KEY_LEN;
    do
    {
      // Check the parity on the key
      rv = desCheckOddParity(wrappingKey->secret_key_simple.value + desKeyOffset);
      if(CKR_OK != rv) {
        TL_LOG_E(" Parity error on DES key");
        break;
      }

      // Increment the key offset to the next key
      desKeyOffset += DES_KEY_LEN;

    } while(desKeyOffset < desKeyLen);
    if (CKR_OK != rv) break;

    /**
     * Get pointers to the destination. Here there are 2 big different cases.
     * We might be dealing with a private key or a symmetric key. Start with the
     * symmetric.
     */

    // MobiCore TL API implementation of 3DES uses 2 keys not 3
    // To comply with PKCS 11 standard we will make K1 = K3
    // and correct the Key length in the session info to 2
    desKeyLen = IS_TYPE(key,tlp11_type_des3_key) ? (DES_KEY_LEN * 2) : desKeyLen;

    // prepare to encrypt
    symKey.key = wrappingKey->secret_key_simple.value;
    symKey.len = desKeyLen;
    mcKey.symKey = &symKey;
    if (!params || (params->header.valid == 0)) {
      TL_LOG_E(" NULL input!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (params->IV_PARAMS.iv_len != 8) {
      TL_LOG_E(" Bad IV length: %d",params->IV_PARAMS.iv_len);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    result = CRYPTO_OP(tlApiCipherInitWithData(
        &sessionHandle,
        alg,
        TLAPI_MODE_ENCRYPT,
        &mcKey,
        params->IV_PARAMS.iv,
        params->IV_PARAMS.iv_len));
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
  TL_LOG_I("des_wrap-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_UNWRAP_FUNC(des_unwrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_3DES_CBC_PKCS5;
  tlApiCrSession_t sessionHandle;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  uint32_t desKeyLen = 0;
  uint32_t desKeyOffset = 0;
  TL_LOG_I("des_unwrap+");
  do {
    if (!unwrappingKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // Check the mechanism and set the algorithm
    switch (mech) {
    case CKM_DES_CBC:
      alg = TLAPI_ALG_DES_CBC_NOPAD;
      break;
    case CKM_DES_CBC_PAD:
      alg = TLAPI_ALG_DES_CBC_PKCS5;
      break;
    case CKM_DES3_CBC:
      alg = TLAPI_ALG_3DES_CBC_NOPAD;
      break;
    case CKM_DES3_CBC_PAD:
      alg = TLAPI_ALG_3DES_CBC_PKCS5;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (rv != CKR_OK)
      break;

    if (IS_TYPE(newKey,tlp11_type_rsa_private_key)) {
      if ((mech != CKM_DES_CBC_PAD) && (mech != CKM_DES3_CBC_PAD)) {
        TL_LOG_E("Private key cannot be unwrapped with this mechanism");
        rv = CKR_KEY_NOT_WRAPPABLE;
        break;
      }
    }

    desKeyLen = IS_TYPE(unwrappingKey,tlp11_type_des_key) ? DES_KEY_LEN : DES3_KEY_LEN;
    do
    {
      // Check the parity on the key
      rv = desCheckOddParity(unwrappingKey->secret_key_simple.value + desKeyOffset);
      if(CKR_OK != rv) {
        TL_LOG_E(" Parity error on DES key");
        break;
      }

      // Increment the key offset to the next key
      desKeyOffset += DES_KEY_LEN;

    } while(desKeyOffset < desKeyLen);
    if (CKR_OK != rv) break;

    /**
     * Get pointers to the destination. Here there are 2 big different cases.
     * We might be dealing with a private key or a symmetric key. Start with the
     * symmetric.
     */

    // MobiCore TL API implementation of 3DES uses 2 keys not 3
    // To comply with PKCS 11 standard we will make K1 = K3
    // and correct the Key length in the session info to 2
    desKeyLen = IS_TYPE(unwrappingKey,tlp11_type_des3_key) ? (DES_KEY_LEN * 2) : desKeyLen;

    // prepare to decrypt
    symKey.key = unwrappingKey->secret_key_simple.value;
    symKey.len = desKeyLen;
    mcKey.symKey = &symKey;
    if (!params || (params->header.valid == 0)) {
      TL_LOG_E(" NULL input!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (params->IV_PARAMS.iv_len != DES_KEY_LEN) {
      TL_LOG_E(" Bad IV length: %d",params->IV_PARAMS.iv_len);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    result = CRYPTO_OP(tlApiCipherInitWithData(
        &sessionHandle,
        alg,
        TLAPI_MODE_DECRYPT,
        &mcKey,
        params->IV_PARAMS.iv,
        params->IV_PARAMS.iv_len));
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
  TL_LOG_I("des_unwrap-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_DERIVE_FUNC(des_encrypt_data)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCrSession_t sessionHandle = CR_SID_INVALID;
  tlApiCipherAlg_t alg = TLAPI_ALG_DES_CBC_PKCS5;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  uint8_t *src = NULL;
  uint8_t *dst = NULL;
  uint32_t dstLen = 0;
  uint8_t block[DES_BLOCK_SIZE] = {0}; // this is our output block
  size_t blockLen = 0;
  uint32_t nUsefulBlocks = 0;
  uint32_t nTotalBlocks = 0;
  size_t maxKeyLen = 0;
  uint32_t newSecretKeyLen = 0;
  size_t desKeyLen = 0;
  uint32_t desKeyOffset = 0;
  unsigned int i = 0; // block counter during encryption, for input
  unsigned int j = 0; // offset encryption, for output
  TL_LOG_I("des_encrypt_data+");
  do {
    if (!baseKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (!IS_TYPE(baseKey,tlp11_type_des_key) && !IS_TYPE(baseKey,tlp11_type_des3_key)) {
      TL_LOG_E(" Unsupported key type!");
      rv = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }

    desKeyLen = IS_TYPE(baseKey,tlp11_type_des_key) ? DES_KEY_LEN : DES3_KEY_LEN;
    do
    {
      // Check the parity on the key
      rv = desCheckOddParity(baseKey->secret_key_simple.value + desKeyOffset);
      if(CKR_OK != rv) {
        TL_LOG_E(" Parity error on DES key");
        break;
      }

      // Increment the key offset to the next key
      desKeyOffset += DES_KEY_LEN;

    } while(desKeyOffset < desKeyLen);
    if (CKR_OK != rv) break;

    // Check the mechanism and set the algorithm
    switch (mech) {
    case CKM_DES_CBC_ENCRYPT_DATA:
      alg = TLAPI_ALG_DES_CBC_NOPAD;
      break;
    case CKM_DES3_CBC_ENCRYPT_DATA:
      alg = TLAPI_ALG_DES_CBC_PKCS5;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (rv != CKR_OK)
      break;

    // Check to new key
    if (IS_TYPE(newKey,tlp11_type_des_key))
      newSecretKeyLen = DES_KEY_LEN;
    else if (IS_TYPE(newKey,tlp11_type_des3_key))
      newSecretKeyLen = DES3_KEY_LEN;
    else
      newSecretKeyLen = newKey->secret_key_simple.len;

    // Checks on parameters
    if ((params == NULL) ||
        (params->DES_CBC_ENCRYPT_DATA_PARAMS.header.valid == 0) ||
        (params->DES_CBC_ENCRYPT_DATA_PARAMS.data == NULL) ||
        (params->DES_CBC_ENCRYPT_DATA_PARAMS.len == 0)) {
      TL_LOG_E(" Wrong parameter!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (CKR_OK != validateBufferLocation(params->DES_CBC_ENCRYPT_DATA_PARAMS.data)) {
      TL_LOG_E("DES_CBC_ENCRYPT_DATA_PARAMS.data ouside valid range!");
      rv = CKR_ARGUMENTS_BAD;
      break;
    }
    maxKeyLen = params->DES_CBC_ENCRYPT_DATA_PARAMS.len;
    if (maxKeyLen < newSecretKeyLen) {
      TL_LOG_E(" Not enough data!");
      rv = CKR_DATA_LEN_RANGE;
      break;
    }
    // input must be multiple of DES block size
    if ((maxKeyLen & 0x7) != 0) {
      TL_LOG_E(" Len not multiple of block size!");
      rv = CKR_DATA_LEN_RANGE;
      break;
    }
    dstLen=newSecretKeyLen;
    dst=newKey->secret_key_simple.value;

    // MobiCore TL API implementation of 3DES uses 2 keys not 3
    // To comply with PKCS 11 standard we will make K1 = K3
    // and correct the Key length in the session info to 2
    desKeyLen = IS_TYPE(baseKey,tlp11_type_des3_key) ? (DES_KEY_LEN * 2) : desKeyLen;

    // set pointers to the key
    symKey.key = baseKey->secret_key_simple.value;
    symKey.len = desKeyLen;
    mcKey.symKey = &symKey;
    // how many blocks are actually needed to fill the key value?
    // nUsefulBlocks = (dstLen + DES_BLOCK_SIZE) / DES_BLOCK_SIZE;
    nUsefulBlocks = (dstLen + DES_BLOCK_SIZE) >> DES_BLOCK_SIZE_SHIFT;
    //nTotalBlocks = maxKeyLen / DES_BLOCK_SIZE;
    nTotalBlocks = maxKeyLen >> DES_BLOCK_SIZE_SHIFT;
    // initialize the operation
    src=params->DES_CBC_ENCRYPT_DATA_PARAMS.data;
    result = CRYPTO_OP(tlApiCipherInitWithData(
          &sessionHandle,
          alg,
          TLAPI_MODE_ENCRYPT,
          &mcKey,
          params->DES_CBC_ENCRYPT_DATA_PARAMS.iv,
          sizeof(params->DES_CBC_ENCRYPT_DATA_PARAMS.iv)
          ));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    for (i=0; i<=(nTotalBlocks-nUsefulBlocks); i++) {
      blockLen = DES_BLOCK_SIZE;
#ifndef DES_HAS_UPDATE
      // temporary hack, due to the lack of Update function in DES implementation
      result = CRYPTO_OP(tlApiCipherDoFinal(
          sessionHandle,
          src + i*DES_BLOCK_SIZE,
          DES_BLOCK_SIZE,
          block,
          &blockLen));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      if (blockLen != DES_BLOCK_SIZE) {
        TL_LOG_E("tlApiCipherUpdate returned a bad blocksize! %d",blockLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }
      // and reinitialize it
      result = CRYPTO_OP(tlApiCipherInitWithData(
          &sessionHandle,
          alg,
          TLAPI_MODE_ENCRYPT,
          &mcKey,
          block,
          blockLen
          ));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherInit: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
#else
      result = CRYPTO_OP(tlApiCipherUpdate(
          sessionHandle,
          src + i*DES_BLOCK_SIZE,
          DES_BLOCK_SIZE,
          block,
          &blockLen));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      if (blockLen != DES_BLOCK_SIZE) {
        TL_LOG_E("tlApiCipherUpdate returned a bad blocksize! %d",blockLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }
#endif
    }
    // the last encrypted block is a particular case, since we might need a part of it
    blockLen = (dstLen & 0x7); //this is the extent we need it
    if (blockLen == 0) {
      // we need it all
      memscpy(dst,dstLen,block,DES_BLOCK_SIZE);
      j += DES_BLOCK_SIZE;
    } else {
      memscpy(dst,dstLen,block+DES_BLOCK_SIZE-blockLen,blockLen);
      j += blockLen;
    }
    // all the following now will go straight into the destination
    for (i++; (i<nTotalBlocks) && (j+DES_KEY_LEN<=dstLen); i++) {
      blockLen = DES_BLOCK_SIZE;
#ifndef DES_HAS_UPDATE
      // temporary hack, due to the lack of Update function in DES implementation
      result = CRYPTO_OP(tlApiCipherDoFinal(
          sessionHandle,
          src + i*DES_BLOCK_SIZE,
          DES_BLOCK_SIZE,
          block,
          &blockLen));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      if (blockLen != DES_BLOCK_SIZE) {
        TL_LOG_E("tlApiCipherUpdate returned a bad blocksize! %d",blockLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }
      // and reinitialize it
      result = CRYPTO_OP(tlApiCipherInitWithData(
          &sessionHandle,
          alg,
          TLAPI_MODE_ENCRYPT,
          &mcKey,
          block,
          blockLen
          ));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherInit: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
#else
      result = CRYPTO_OP(tlApiCipherUpdate(
          sessionHandle,
          src + i*DES_BLOCK_SIZE,
          DES_BLOCK_SIZE,
          block,
          &blockLen));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherUpdate: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      if (blockLen != DES_BLOCK_SIZE) {
        TL_LOG_E("tlApiCipherUpdate returned a bad blocksize! %d",blockLen);
        rv = CKR_GENERAL_ERROR;
        break;
      }
#endif
      memscpy(dst+j,dstLen-j,block,DES_BLOCK_SIZE);
      j += DES_BLOCK_SIZE;
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
  TL_LOG_I("des_encrypt_data-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_GENERATE_KEY_FUNC(des_generate)
{
  CK_RV         rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t      desKeyLen = 0;
  uint32_t      desKeyOffset = 0;
  TL_LOG_I("des_generate+");
  do {
    if (!key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (!IS_TYPE(key,tlp11_type_des_key) && !IS_TYPE(key,tlp11_type_des3_key)) {
      TL_LOG_E(" Can only generate DES keys");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    // Set the des key length
    if(IS_TYPE(key,tlp11_type_des_key)) {
      desKeyLen = DES_BLOCK_SIZE;
    } else {
      desKeyLen = DES_BLOCK_SIZE * 3;
    }

    // Generate either a single key or a block of 3 keys
    do {

      // Generate a single key
      do {

        //result = tlApiRandomGenerateData(TLAPI_ALG_SECURE_RANDOM,(uint8_t*)(key->secret_key.value),key->secret_key.len);
        size_t randomLen = DES_BLOCK_SIZE;
        result = tlApiRandomGenerateData(
            TLAPI_ALG_PSEUDO_RANDOM,
            key->secret_key_simple.value + desKeyOffset,
            &randomLen);

        if (!IS_OK(result)) {
          TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
          rv = TL2CK(result);
          break;
        }
        if (randomLen != DES_BLOCK_SIZE) {
          TL_LOG_E("Failed to generate enough random data: %d vs %d",
              randomLen,DES_BLOCK_SIZE);
          rv = CKR_FUNCTION_FAILED;
          break;
        }

        // Set odd parity on the key
        desSetOddParity(key->secret_key_simple.value + desKeyOffset);

        // Check for a weak key before continuing
      } while(desCheckForWeakKey(key->secret_key_simple.value + desKeyOffset) != CKR_OK);

      // Check all is good
      if (rv != CKR_OK) {
        break;
      }

      // Increment offset in to the generated key value
      desKeyOffset += DES_BLOCK_SIZE;

      // MobiCore TL API implementation of 3DES uses 2 keys not 3
      // DES3-E( {K1,K2,K1}, P ) = E( K1, D( K2, E( K1, P ) ) )
      // DES3-D( {K1,K2,K1}, C ) = D( K1, E( K2, D( K1, P ) ) )
      // To comply with PKCS 11 standard we will make K1 = K3
      if (desKeyOffset >= (DES_BLOCK_SIZE * 2)){
        memscpy(key->secret_key_simple.value + desKeyOffset,SECRET_KEY_MAX_LEN - desKeyOffset,key->secret_key_simple.value,DES_BLOCK_SIZE);
        break;
      }

    } while(desKeyOffset < desKeyLen);
    key->secret_key_simple.len = desKeyLen;

  } while(0);

  TL_LOG_I("des_generate-: %08X",rv);
  return rv;
}
