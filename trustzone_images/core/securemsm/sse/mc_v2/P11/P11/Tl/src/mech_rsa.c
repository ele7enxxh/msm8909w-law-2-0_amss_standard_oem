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

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_CIPHER_INIT_FUNC(rsa_cipher_init) {
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_RSA_PKCS1;
  tlApiCipherMode_t mode;
  TL_LOG_I("rsa_cipher_init+");
  do {
    // check the input
    if (!session || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    // check key and mode
    if (IS_TYPE(key,tlp11_type_rsa_private_key) && (operation == CKF_DECRYPT)) {
      if (key->private_key.decrypt == 0) {
        TL_LOG_E(" Key doesn't support decryption");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
      session->context.key.asymmetric.exponent_len = key->rsa_private_key.exponent_len;
      memscpy(session->context.key.asymmetric.exponent,
          PRIVATE_EXPONENT_MAX_LEN,
          key->rsa_private_key.exponent,
          key->rsa_private_key.exponent_len);
      session->context.key.asymmetric.modulus_len = key->rsa_private_key.modulus_len;
      memscpy(session->context.key.asymmetric.modulus,
          MODULUS_MAX_LEN,
          key->rsa_private_key.modulus,
          key->rsa_private_key.modulus_len);
      session->context.key.asymmetric.public_exponent_len = key->rsa_private_key.public_exponent_len;
      memscpy(session->context.key.asymmetric.public_exponent,
          PUBLIC_EXPONENT_LEN,
          key->rsa_private_key.public_exponent,
          key->rsa_private_key.public_exponent_len);
      mode = TLAPI_MODE_DECRYPT;
    } else if (IS_TYPE(key,tlp11_type_rsa_public_key) && (operation == CKF_ENCRYPT)) {
      if (key->public_key.encrypt == 0) {
        TL_LOG_E(" Key doesn't support encryption");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
      session->context.key.asymmetric.exponent_len = 0;
      session->context.key.asymmetric.public_exponent_len = key->rsa_public_key.public_exponent_len;
      memscpy(session->context.key.asymmetric.public_exponent,
          PUBLIC_EXPONENT_LEN,
          key->rsa_public_key.public_exponent,
          key->rsa_public_key.public_exponent_len);
      session->context.key.asymmetric.modulus_len = key->rsa_public_key.modulus_len;
      memscpy(session->context.key.asymmetric.modulus,
          MODULUS_MAX_LEN,
          key->rsa_public_key.modulus,
          key->rsa_public_key.modulus_len);
      mode = TLAPI_MODE_ENCRYPT;
    } else {
      TL_LOG_E(" Key-operation unsupported");
      rv = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }
    switch (mech) {
    case CKM_RSA_PKCS:
      alg = TLAPI_ALG_RSA_PKCS1;
      //alg = TLAPI_ALG_RSA_ISO14888;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;
    memset(&session->context.key.asymmetric.rsaKey,0,sizeof(session->context.key.asymmetric.rsaKey));
    session->context.key.asymmetric.rsaKey.privateExponent.len = session->context.key.asymmetric.exponent_len;
    if (session->context.key.asymmetric.rsaKey.privateExponent.len)
      session->context.key.asymmetric.rsaKey.privateExponent.value = session->context.key.asymmetric.exponent;
    else
      session->context.key.asymmetric.rsaKey.privateExponent.value = NULL;
    session->context.key.asymmetric.rsaKey.exponent.len = session->context.key.asymmetric.public_exponent_len;
    session->context.key.asymmetric.rsaKey.exponent.value = session->context.key.asymmetric.public_exponent;
    session->context.key.asymmetric.rsaKey.modulus.len = session->context.key.asymmetric.modulus_len;
    session->context.key.asymmetric.rsaKey.modulus.value = session->context.key.asymmetric.modulus;
    session->context.key.asymmetric.mcKey.rsaKey=&(session->context.key.asymmetric.rsaKey);
    TL_LOG_I("Initializing RSA cipher with alg 0x%08X, mode 0x%08X",alg,mode);
    TL_LOG_I("Modulus len: %d bytes @ 0x%08X",session->context.key.asymmetric.mcKey.rsaKey->modulus.len,session->context.key.asymmetric.mcKey.rsaKey->modulus.value);
    TL_LOG_I("Exponent len: %d bytes @ 0x%08X",session->context.key.asymmetric.mcKey.rsaKey->privateExponent.len,session->context.key.asymmetric.mcKey.rsaKey->privateExponent.value);
    TL_LOG_I("Public Exponent len: %d bytes @ 0x%08X",session->context.key.asymmetric.mcKey.rsaKey->exponent.len,session->context.key.asymmetric.mcKey.rsaKey->exponent.value);
    DUMP_HEX("Exponent",session->context.key.asymmetric.mcKey.rsaKey->privateExponent.value,session->context.key.asymmetric.mcKey.rsaKey->privateExponent.len);
    DUMP_HEX("Public Exponent",session->context.key.asymmetric.mcKey.rsaKey->exponent.value,session->context.key.asymmetric.mcKey.rsaKey->exponent.len);
    DUMP_HEX("Modulus",session->context.key.asymmetric.mcKey.rsaKey->modulus.value,session->context.key.asymmetric.mcKey.rsaKey->modulus.len);
    TL_LOG_I("Session @ 0x%08X",session);
    TL_LOG_I("Context @ 0x%08X",&session->context);
    TL_LOG_I("Key @ 0x%08X",&session->context.key);
    TL_LOG_I("Asymmetric @ 0x%08X",&session->context.key.asymmetric);
    TL_LOG_I("mcKey @ 0x%08X",&session->context.key.asymmetric.mcKey);
    TL_LOG_I("rsaKey @ 0x%08X",&session->context.key.asymmetric.mcKey.rsaKey);
    result = tlApiCipherInit(
          &session->context.cryptoSession,
          alg,
          mode,
          &session->context.key.asymmetric.mcKey);
    if (IS_STUBBED(result)) {
      session->context.cryptoSession=0x12345678;
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    TL_LOG_I("Modulus len: %d bytes @ 0x%08X",session->context.key.asymmetric.mcKey.rsaKey->modulus.len,session->context.key.asymmetric.mcKey.rsaKey->modulus.value);
    TL_LOG_I("Exponent len: %d bytes @ 0x%08X",session->context.key.asymmetric.mcKey.rsaKey->exponent.len,session->context.key.asymmetric.mcKey.rsaKey->exponent.value);
  } while(0);
  TL_LOG_I("rsa_cipher_init-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_SIGN_VERIFY_INIT_FUNC(rsa_sign_verify_init)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiSigAlg_t alg = TLAPI_SIG_RSA_SHA_PKCS1;
  tlApiSigMode_t mode;
  TL_LOG_I("rsa_sign_verify_init+");
  do {
    // check the input
    if (!session || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if ((mech == CKM_SHA1_RSA_PKCS_PSS) ||
        (mech == CKM_SHA256_RSA_PKCS_PSS) ||
        (mech == CKM_RSA_PKCS_PSS)) {
      // we need a valid parameter structure
      if (!params || (params->header.valid == 0)) {
        TL_LOG_E(" NULL parameter input!");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    }
    // check the key
    if (operation == CKF_SIGN) {
      if (!IS_TYPE(key,tlp11_type_rsa_private_key)) {
        TL_LOG_E(" Wrong key type");
        rv = CKR_KEY_TYPE_INCONSISTENT;
        break;
      }
      if (key->private_key.sign == 0) {
        TL_LOG_E(" Key doesn't support signing");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
      // copy over the key
      session->context.key.asymmetric.exponent_len = key->rsa_private_key.exponent_len;
      memscpy(session->context.key.asymmetric.exponent,
          PRIVATE_EXPONENT_MAX_LEN,
          key->rsa_private_key.exponent,
          key->rsa_private_key.exponent_len);
      session->context.key.asymmetric.modulus_len = key->rsa_private_key.modulus_len;
      memscpy(session->context.key.asymmetric.modulus,
          MODULUS_MAX_LEN,
          key->rsa_private_key.modulus,
          key->rsa_private_key.modulus_len);
      session->context.key.asymmetric.public_exponent_len = key->rsa_private_key.public_exponent_len;
      memscpy(session->context.key.asymmetric.public_exponent,
          PUBLIC_EXPONENT_LEN,
          key->rsa_private_key.public_exponent,
          key->rsa_private_key.public_exponent_len);
      mode = TLAPI_MODE_SIGN;
    } else if (operation == CKF_VERIFY) {
      if (!IS_TYPE(key,tlp11_type_rsa_public_key)) {
        TL_LOG_E(" Wrong key type");
        rv = CKR_KEY_TYPE_INCONSISTENT;
        break;
      }
      if (key->public_key.verify == 0) {
        TL_LOG_E(" Key doesn't support signature verification");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
      // copy over the key
      session->context.key.asymmetric.exponent_len = 0;
      session->context.key.asymmetric.public_exponent_len = key->rsa_public_key.public_exponent_len;
      memscpy(session->context.key.asymmetric.public_exponent,
          PUBLIC_EXPONENT_LEN,
          key->rsa_public_key.public_exponent,
          key->rsa_public_key.public_exponent_len);
      session->context.key.asymmetric.modulus_len = key->rsa_public_key.modulus_len;
      memscpy(session->context.key.asymmetric.modulus,
          MODULUS_MAX_LEN,
          key->rsa_public_key.modulus,
          key->rsa_public_key.modulus_len);
      mode = TLAPI_MODE_VERIFY;
    } else {
      TL_LOG_E(" Sign function called for operation other than sign/verify: 0x%08X",operation);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    memset(&session->context.key.asymmetric.rsaKey,0,sizeof(session->context.key.asymmetric.rsaKey));
    session->context.key.asymmetric.rsaKey.privateExponent.len = session->context.key.asymmetric.exponent_len;
    if (session->context.key.asymmetric.rsaKey.privateExponent.len)
      session->context.key.asymmetric.rsaKey.privateExponent.value = session->context.key.asymmetric.exponent;
    else
      session->context.key.asymmetric.rsaKey.privateExponent.value = NULL;
    session->context.key.asymmetric.rsaKey.exponent.len = session->context.key.asymmetric.public_exponent_len;
    session->context.key.asymmetric.rsaKey.exponent.value = session->context.key.asymmetric.public_exponent;
    session->context.key.asymmetric.rsaKey.modulus.len = session->context.key.asymmetric.modulus_len;
    session->context.key.asymmetric.rsaKey.modulus.value = session->context.key.asymmetric.modulus;
    session->context.key.asymmetric.mcKey.rsaKey=&(session->context.key.asymmetric.rsaKey);
    switch (mech) {
    case CKM_SHA1_RSA_PKCS:
      alg = TLAPI_SIG_RSA_SHA_PKCS1;
      break;
    case CKM_SHA1_RSA_PKCS_PSS:
      alg = TLAPI_SIG_RSA_SHA1_PSS;
      if (params->RSA_PKCS_PSS_PARAMS.hashAlg != CKM_SHA_1) {
        TL_LOG_E(" hashAlg must match mechanism");
        rv = CKR_MECHANISM_PARAM_INVALID;
      }
      break;
    case CKM_SHA256_RSA_PKCS_PSS:
      alg = TLAPI_SIG_RSA_SHA256_PSS;
      if (params->RSA_PKCS_PSS_PARAMS.hashAlg != CKM_SHA256) {
        TL_LOG_E(" hashAlg must match mechanism");
        rv = CKR_MECHANISM_PARAM_INVALID;
      }
      break;
    case CKM_RSA_PKCS_PSS:
      // we have to read the digest algorith from the parameters
      if (params->RSA_PKCS_PSS_PARAMS.hashAlg == CKM_SHA_1) {
        if (params->RSA_PKCS_PSS_PARAMS.mgf != CKG_MGF1_SHA1) {
          TL_LOG_E(" hashAlg and mgf must point to the same hash algorith");
          rv = CKR_MECHANISM_PARAM_INVALID;
          break;
        }
        if ((operation == CKF_SIGN) &&
            (params->RSA_PKCS_PSS_PARAMS.sLen > SHA1_HASH_LEN)) {
          TL_LOG_E(" We support only salts up to the length of the hash");
          rv = CKR_MECHANISM_PARAM_INVALID;
          break;
        }
        alg = TLAPI_SIG_RSA_SHA1_PSS;
      } else if (params->RSA_PKCS_PSS_PARAMS.hashAlg == CKM_SHA256) {
        if (params->RSA_PKCS_PSS_PARAMS.mgf != CKG_MGF1_SHA256) {
          TL_LOG_E(" hashAlg and mgf must point to the same hash algorith");
          rv = CKR_MECHANISM_PARAM_INVALID;
          break;
        }
        if ((operation == CKF_SIGN) &&
            (params->RSA_PKCS_PSS_PARAMS.sLen > SHA256_HASH_LEN)) {
          TL_LOG_E(" We support only salts up to the length of the hash");
          rv = CKR_MECHANISM_PARAM_INVALID;
          break;
        }
        alg = TLAPI_SIG_RSA_SHA256_PSS;
      } else {
        TL_LOG_E(" Unrecognised hashAlg: 0x%08X",params->RSA_PKCS_PSS_PARAMS.hashAlg);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;
    if (alg == TLAPI_SIG_RSA_SHA_PKCS1) {
      result = CRYPTO_OP(tlApiSignatureInit(
            &session->context.cryptoSession,
            &session->context.key.asymmetric.mcKey,
            mode,
            alg));
    } else {
      uint8_t salt[SHA256_HASH_LEN] = {0};
      if (params->RSA_PKCS_PSS_PARAMS.sLen > SHA256_HASH_LEN) {
        TL_LOG_E("Unsupported salt length: %d",params->RSA_PKCS_PSS_PARAMS.sLen);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if ((params->RSA_PKCS_PSS_PARAMS.sLen > 0) && (mode == TLAPI_MODE_SIGN)){
        size_t randomLen = params->RSA_PKCS_PSS_PARAMS.sLen;
        // we have to generate a salt
        //result = tlApiRandomGenerateData(TLAPI_ALG_SECURE_RANDOM,salt,params->RSA_PKCS_PSS_PARAMS.sLen); STUB
        result = tlApiRandomGenerateData(TLAPI_ALG_PSEUDO_RANDOM,salt,&randomLen);
        if (!IS_OK(result)) {
          TL_LOG_E(" Failed to generate salt: 0x%08X",result);
          rv = TL2CK(result);
          break;
        }
        if (randomLen != params->RSA_PKCS_PSS_PARAMS.sLen) {
          TL_LOG_E("Failed to generate enough random data: %d vs %d",
              randomLen,params->RSA_PKCS_PSS_PARAMS.sLen);
          rv = CKR_FUNCTION_FAILED;
          break;
        }
      }
      result = CRYPTO_OP(tlApiSignatureInitWithData(
            &session->context.cryptoSession,
            &session->context.key.asymmetric.mcKey,
            mode,
            alg,
            (params->RSA_PKCS_PSS_PARAMS.sLen > 0)?(salt):NULL,
            params->RSA_PKCS_PSS_PARAMS.sLen));

    }
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
  TL_LOG_I("rsa_sign_verify_init-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_SIGN_VERIFY_FINAL_FUNC(rsa_sign_verify_final)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("rsa_sign_verify_final+");
  do {
    // just perform the operation
    if (session->context.cryptoSession != CR_SID_INVALID) {
      TL_LOG_I("In:  %d bytes @ 0x%08X",dataInLen,dataIn);
      TL_LOG_I("Out: %d bytes @ 0x%08X",*dataOutLen,dataOut);
      DUMP_HEX("In",dataIn,dataInLen);
      if (session->context.operation == CKF_SIGN) {
        result = CRYPTO_OP(tlApiSignatureSign(
            session->context.cryptoSession,
            dataIn,
            dataInLen,
            dataOut,
            dataOutLen));
        if (IS_STUBBED(result)) {
          result = TLAPI_OK;
        }
        if (!IS_OK(result)) {
          TL_LOG_E("tlApiSignatureSign/tlApiSignatureVerify: 0x%08X",result);
          rv=TL2CK(result);
          break;
        }
        session->context.cryptoSession = CR_SID_INVALID;
      } else {
        bool validity = 0;
        DUMP_HEX("dataIn",dataIn,dataInLen);
        DUMP_HEX("dataOut",dataOut,*dataOutLen);
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
          TL_LOG_E("tlApiSignatureSign/tlApiSignatureVerify: 0x%08X",result);
          rv=TL2CK(result);
          break;
        }
        session->context.cryptoSession = CR_SID_INVALID;
        if (!validity) {
          TL_LOG_I("Signature verified as invalid");
          rv = CKR_SIGNATURE_INVALID;
          break;
        }
      }
      TL_LOG_I("In:  %d bytes @ 0x%08X",dataInLen,dataIn);
      TL_LOG_I("Out: %d bytes @ 0x%08X",*dataOutLen,dataOut);
      DUMP_HEX("Out",dataOut,*dataOutLen);
    } else {
      TL_LOG_E("Invalid MC CryptoSession!");
      rv = CKR_OPERATION_NOT_INITIALIZED;
      break;
    }
  } while(0);
  TL_LOG_I("rsa_sign_verify_final-: %08X",rv);
  return rv;
}

/** Process a GenerateKeyPair command
 */
/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_GENERATE_KEY_PAIR_FUNC(rsa_generate)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiKeyPair_t keyPair = {0}; /*lint !e708*/
  tlApiRsaKey_t rsaKey = {0};
  TL_LOG_I("rsa_generate+");
  do {
    if (!pub || !prv || (pub->rsa_public_key.public_exponent == NULL)) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // set lengths appropriately for generation
    pub->rsa_public_key.modulus_len = pub->rsa_public_key.modulus_bits / 8;
    pub->rsa_public_key.public_exponent_len = PUBLIC_EXPONENT_LEN;
    prv->rsa_private_key.modulus_len = pub->rsa_public_key.modulus_bits / 8;
    prv->rsa_private_key.exponent_len = prv->rsa_private_key.modulus_len;
    prv->rsa_private_key.public_exponent_len = pub->rsa_public_key.public_exponent_len;

    // new MC v1.2 APIs require public exponent to be set, so we set it to 17
    pub->rsa_public_key.public_exponent_len = 2;
    pub->rsa_public_key.public_exponent[0] = 1;
    pub->rsa_public_key.public_exponent[1] = 1;

    TL_LOG_I("(PUB) Modulus len: %d bytes",pub->rsa_public_key.modulus_len);
    TL_LOG_I("(PUB) Exponent len: %d bytes",pub->rsa_public_key.public_exponent_len);
    TL_LOG_I("(PRV) Modulus len: %d bytes",prv->rsa_private_key.modulus_len);
    TL_LOG_I("(PRV) Exponent len: %d bytes",prv->rsa_private_key.exponent_len);
    TL_LOG_I("(PRV) Public Exponent len: %d bytes",prv->rsa_private_key.public_exponent_len);

    rv = validateRSAPrivateKey(&prv->rsa_private_key);
    if (rv != CKR_OK) {
      TL_LOG_E(" Failed to validate private key");
      break;
    }
    rv = validateRSAPublicKey(&pub->rsa_public_key);
    if (rv != CKR_OK) {
      TL_LOG_E(" Failed to validate public key");
      break;
    }

    // assign keys
    rsaKey.exponent.value=pub->rsa_public_key.public_exponent;
    rsaKey.exponent.len=pub->rsa_public_key.public_exponent_len;
    rsaKey.modulus.value=pub->rsa_public_key.modulus;
    rsaKey.modulus.len=pub->rsa_public_key.modulus_len;
    rsaKey.privateExponent.value=prv->rsa_private_key.exponent;
    rsaKey.privateExponent.len=prv->rsa_private_key.exponent_len;
    keyPair.rsaKeyPair = &rsaKey;
    TL_LOG_I("(PUB) Modulus len: %d bytes @ 0x%08X",keyPair.rsaKeyPair->modulus.len, keyPair.rsaKeyPair->modulus.value);
    TL_LOG_I("(PUB) Exponent len: %d bytes @ 0x%08X",keyPair.rsaKeyPair->exponent.len, keyPair.rsaKeyPair->exponent.value);
    TL_LOG_I("(PRV) Exponent len: %d bytes @ 0x%08X",keyPair.rsaKeyPair->privateExponent.len, keyPair.rsaKeyPair->privateExponent.value);
    TL_LOG_I("Now generating");
    // now call into MC to generate it
    result = tlApiGenerateKeyPair(&keyPair,TLAPI_RSA, pub->rsa_public_key.modulus_len);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to generate RSA key pair: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }
    if (IS_STUBBED(result)) {
      // just use the labels now...
      TL_LOG_I(" Stubbing RSA key pair generation");
      pub->rsa_public_key.public_exponent[3]=0x3;
      pub->rsa_public_key.public_exponent_len=1;
      memscpy(pub->rsa_public_key.modulus,MODULUS_MAX_LEN,pub->storage.label,pub->storage.label_len);
      memscpy(prv->rsa_private_key.modulus,MODULUS_MAX_LEN,prv->storage.label,prv->storage.label_len);
      memscpy(prv->rsa_private_key.exponent,PRIVATE_EXPONENT_MAX_LEN,prv->storage.label,prv->storage.label_len);
      result = TLAPI_OK;
    }
    TL_LOG_I("Generation done");
    TL_LOG_I("(PUB) Modulus len: %d bytes",keyPair.rsaKeyPair->modulus.len);
    TL_LOG_I("(PUB) Exponent len: %d bytes",keyPair.rsaKeyPair->exponent.len);
    TL_LOG_I("(PRV) Exponent len: %d bytes",keyPair.rsaKeyPair->privateExponent.len);
    // move back the lengths
    pub->rsa_public_key.public_exponent_len=rsaKey.exponent.len;
    pub->rsa_public_key.modulus_len=rsaKey.modulus.len;
    prv->rsa_private_key.exponent_len=rsaKey.privateExponent.len;
    prv->rsa_private_key.modulus_len=rsaKey.modulus.len;
    // done for good, move the content around
    memscpy(prv->rsa_private_key.public_exponent,PUBLIC_EXPONENT_LEN,pub->rsa_public_key.public_exponent,PUBLIC_EXPONENT_LEN);
    prv->rsa_private_key.public_exponent_len=pub->rsa_public_key.public_exponent_len;
    memscpy(prv->rsa_private_key.modulus,MODULUS_MAX_LEN,pub->rsa_public_key.modulus,pub->rsa_public_key.modulus_len);
    DUMP_HEX("Public exponent",pub->rsa_public_key.public_exponent,pub->rsa_public_key.public_exponent_len);
    DUMP_HEX("Private exponent",prv->rsa_private_key.exponent,prv->rsa_private_key.exponent_len);
    DUMP_HEX("Modulus (PRV)",prv->rsa_private_key.modulus,prv->rsa_private_key.modulus_len);
    DUMP_HEX("Modulus (PUB)",pub->rsa_public_key.modulus,pub->rsa_public_key.modulus_len);
  } while(0);
  TL_LOG_I("rsa_generate-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_WRAP_FUNC(rsa_wrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_RSA_PKCS1;
  tlApiCrSession_t sessionHandle;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiRsaKey_t rsaKey = {0};
  TL_LOG_I("rsa_wrap+");
  do {
    if (!wrappingKey || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (!IS_TYPE(wrappingKey,tlp11_type_rsa_public_key)) {
      TL_LOG_E(" Not a public key!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    switch (mech) {
    case CKM_RSA_PKCS:
      alg = TLAPI_ALG_RSA_PKCS1;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;

    /**
     * Get pointers to the destination. Here there are 2 big different cases.
     * We might be dealing with a private key or a symmetric key. Start with the
     * symmetric.
     */

    // prepare to encrypt
    rsaKey.exponent.value = wrappingKey->rsa_public_key.public_exponent;
    rsaKey.exponent.len = wrappingKey->rsa_public_key.public_exponent_len;
    rsaKey.modulus.value = wrappingKey->rsa_public_key.modulus;
    rsaKey.modulus.len = wrappingKey->rsa_public_key.modulus_len;
    mcKey.rsaKey = &rsaKey;

    result = CRYPTO_OP(tlApiCipherInit(&sessionHandle,alg,TLAPI_MODE_ENCRYPT,&mcKey));

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
  TL_LOG_I("rsa_wrap-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_UNWRAP_FUNC(rsa_unwrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg = TLAPI_ALG_RSA_PKCS1;
  tlApiCrSession_t sessionHandle;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiRsaKey_t rsaKey = {0};
  TL_LOG_I("rsa_unwrap+");
  do {
    if (!unwrappingKey || !newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (!IS_TYPE(unwrappingKey,tlp11_type_rsa_private_key)) {
      TL_LOG_E(" Not a private key!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    switch (mech) {
    case CKM_RSA_PKCS:
      alg = TLAPI_ALG_RSA_PKCS1;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;

    /**
     * Get pointers to the destination. Here there are 2 big different cases.
     * We might be dealing with a private key or a symmetric key. Start with the
     * symmetric.
     */

    // prepare to decrypt
    rsaKey.exponent.value = unwrappingKey->rsa_private_key.public_exponent;
    rsaKey.exponent.len = unwrappingKey->rsa_private_key.public_exponent_len;
    rsaKey.privateExponent.value = unwrappingKey->rsa_private_key.exponent;
    rsaKey.privateExponent.len = unwrappingKey->rsa_private_key.exponent_len;
    rsaKey.modulus.value = unwrappingKey->rsa_private_key.modulus;
    rsaKey.modulus.len = unwrappingKey->rsa_private_key.modulus_len;
    mcKey.rsaKey = &rsaKey;

    result = CRYPTO_OP(tlApiCipherInit(&sessionHandle,alg,TLAPI_MODE_DECRYPT,&mcKey));

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
  TL_LOG_I("rsa_unwrap-: %08X",rv);
  return rv;
}
