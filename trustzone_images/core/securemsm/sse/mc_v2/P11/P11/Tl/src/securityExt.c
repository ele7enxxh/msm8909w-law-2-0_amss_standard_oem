/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "tlStd.h"
#include "tlP11Token.h"
#include "tlP11Glob.h"
#include "securityExt.h"
#include "tlLog.h"

#ifdef ENABLE_WEAK_DERIVE
/*lint -esym(714,unsecure_mechanisms) -esym(528,unsecure_mechanisms)*/
static CK_MECHANISM_TYPE unsecure_mechanisms[] = {
    CKM_RSA_X_509,
    CKM_CONCATENATE_BASE_AND_KEY,
    CKM_CONCATENATE_BASE_AND_DATA,
    CKM_CONCATENATE_DATA_AND_BASE,
    CKM_XOR_BASE_AND_DATA,
    CKM_EXTRACT_KEY_FROM_KEY,
};
#endif

static uint32_t securityFlag(const tlp11_token_t *token, const uint32_t flag) {
  if (token) {
    return (((g_config.arg.securityExtensions & flag) |
            (token->securityExtensions & flag)) != 0);
  } else {
    return ((g_config.arg.securityExtensions & flag) != 0);
  }
}

uint32_t sticky(const tlp11_token_t *token) {
  return securityFlag(token,TL_SE_STICKY_ATTRIBUTES);
}

uint32_t conflictingAttributes(const tlp11_token_t *token) {
  return securityFlag(token,TL_SE_CONFLICTS_CHECK);
}

uint32_t secureTemplates(const tlp11_token_t *token) {
  return securityFlag(token,TL_SE_SECURE_TEMPLATES);
}

uint32_t minIterationsPbkdf2SK(const tlp11_token_t *token) {
  return securityFlag(token,TL_SE_ITER_PBKDF2_SK);
}

uint32_t weakerKey(const tlp11_token_t *token) {
  return securityFlag(token,TL_SE_WEAKER_KEY);
}

static uint32_t getPasswordEntropy(uint32_t len) {
  // we adopt here NIST Special Pubblication 800-63, Appendix A, pg 49
  if (len == 0)
    return 0;
  if (len == 1)
    return 4;
  if (len <= 8)
    return 4 + (len-1)*2;
  if (len <= 20)
    return 4 + 7*2 + (len - 8) + ((len - 7) >> 1); // we want (len-8) * 1.5, but cannot afford FP
  return 4 + 14 + 18 + (len - 20);
}

uint32_t getKeyStrength(const tlp11_container_object_t *object)
{
  uint32_t lenEq = 0;
  TL_LOG_I("getKeyStrength+");
  do {
    if (!object) {
      TL_LOG_E("NULL input!");
      break;
    }
    if (IS_TYPE(object,tlp11_type_rsa_private_key)) {
      if (object->rsa_private_key.modulus_len == 128) {
        lenEq = 80;
      } else if (object->rsa_private_key.modulus_len == 256) {
        lenEq = 112;
      } else {
        TL_LOG_E("Unsupported RSA private key length!");
        break;
      }
    } else if (IS_TYPE(object,tlp11_type_rsa_public_key)) {
      if (object->rsa_public_key.modulus_len == 128) {
        lenEq = 80;
      } else if (object->rsa_public_key.modulus_len == 256) {
        lenEq = 112;
      } else {
        TL_LOG_E("Unsupported RSA public key length, we treat it as 1024");
        lenEq = 80;
      }
    } else if (IS_TYPE(object,tlp11_type_aes_key)) {
      lenEq = object->secret_key_simple.len * 8;
    } else if (IS_TYPE(object,tlp11_type_otp_key)) {
      lenEq = object->otp.len * 8;
    } else if (IS_TYPE(object,tlp11_type_generic_key)) {
      if (object->key.key_gen_mechanism == CKM_GENERIC_SECRET_KEY_GEN_KEYPAD) {
        // it's a password, hence poor security
        lenEq=getPasswordEntropy(object->secret_key_simple.len);
      } else {
        lenEq = object->secret_key_simple.len * 8;
      }
    } else if (IS_TYPE(object,tlp11_type_des3_key)) {
      lenEq = 112;
    } else if (IS_TYPE(object,tlp11_type_des_key)) {
      lenEq = 56;
    }
  } while(0);
  TL_LOG_I("getKeyStrength-: %d",lenEq);
  return lenEq;
}

uint32_t mechanismIsSecure(const CK_MECHANISM_TYPE mech)
{
#ifdef ENABLE_WEAK_DERIVE
  if (securityFlag(NULL,TL_SE_SECURE_MECH)) {
    for (unsigned int i=0; i<LENGTH(unsecure_mechanisms); i++) {
      if (mech == unsecure_mechanisms[i])
        return 0;
    }
  }
#endif //ENABLE_WEAK_RERIVE
  return 1;
}

uint32_t secureWrapOnly(const tlp11_token_t *token) {
  return securityFlag(token,TL_SE_SECURE_WRAP_ONLY);
}

uint32_t trustedWrapOnly(const tlp11_token_t *token) {
  return securityFlag(token,TL_SE_TRUSTED_WRAP_ONLY);
}
