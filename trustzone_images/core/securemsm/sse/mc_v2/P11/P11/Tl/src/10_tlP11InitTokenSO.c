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
#include "tlLog.h"
#include "tlP11Glob.h"
#include "asn1.h"

/** Process a InitToken command
 *
 * This command returns the wrapped version of the token.
 *
 * @param tci shared buffer containing the command and our working area
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdInitTokenSO(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  size_t soLen = 0;
  uint8_t i = 0;
  size_t randomLen = 0;

  do {
    TL_LOG_I("processCmdInitTokenSO+");

    if (tci_message->cmdInitTokenSO.params.wrappedDataLen == 0) {
      TL_LOG_E("Null key!");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }

    // initialize the token
    g_token_0_ptr = (tlp11_token_t*) tci_message->rspInitTokenSO.params.so_token.value;
    memset(g_token_0_ptr,0,sizeof(tlp11_token_t));
    // copy over the label from input
    memscpy(g_token_0_ptr->info.label,TOKEN_LABEL_LEN,tci_message->cmdInitTokenSO.params.label,sizeof(g_token_0_ptr->info.label));

    // sanitise the label, replace '0' with <space>
    for(i=0; i<sizeof(g_token_0_ptr->info.label); i++) {
      if (g_token_0_ptr->info.label[i] == 0)
        g_token_0_ptr->info.label[i]=' ';
    }

    // fill in the other default values
    fillTokenInfo(&g_token_0_ptr->info);
    g_token_0_ptr->authSpan = g_config.arg.maxAuthSpan;
    g_token_0_ptr->usageNum = g_config.arg.maxUsageNum;

    tlp11_so_container_t *so_container_0_ptr = (tlp11_so_container_t *)tci_message->cmdInitTokenSO.params.wrappedData;
    soLen = sizeof(tlp11_so_container_t);
    {
      result = CRYPTO_OP(tlApiUnwrapObjectExt(
            (mcSoHeader_t *)tci_message->cmdInitTokenSO.params.wrappedData,
            sizeof(tci_message->cmdInitTokenSO.params.wrappedData),
            so_container_0_ptr,
            &soLen,
            TLAPI_UNWRAP_PERMIT_CONTEXT_MASK));
    }
    if (IS_STUBBED(result)) {
      memscpy(
          so_container_0_ptr,
          SO_CONTAINER_MAX_LEN,
          tci_message->cmdInitTokenSO.params.wrappedData+sizeof(mcSoHeader_t),
          min(soLen,tci_message->cmdInitTokenSO.params.wrappedDataLen-sizeof(mcSoHeader_t)));
      soLen = tci_message->cmdInitTokenSO.params.wrappedDataLen - sizeof(mcSoHeader_t) ;
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("Failed to unwrap data: 0x%08X",result);
      ret = CKR_GENERAL_ERROR;
      break;
    }
    // now check what key we got
    if (so_container_0_ptr->object.cls == CKO_SECRET_KEY) {
      TL_LOG_I("AES key");
      // can this key be used to initialize a token?
      if ((so_container_0_ptr->key.flags & CKF_SO_INIT) == 0) {
        TL_LOG_E("Key does not allow being used for token initialization: 0x%08X",
            so_container_0_ptr->key.flags);
        ret = CKR_ARGUMENTS_BAD;
        break;
      }
      // check that the final length is supported
      if ((so_container_0_ptr->secret.len != 16) &&
          (so_container_0_ptr->secret.len != 32)) {
        TL_LOG_E("Unsupported key length: %d!",so_container_0_ptr->secret.len);
        ret = CKR_PIN_LEN_RANGE;
        break;
      }
      g_token_0_ptr->associatedKey.header.type = CKK_AES;
      g_token_0_ptr->associatedKey.symmetric.len = so_container_0_ptr->secret.len;
      memscpy(g_token_0_ptr->associatedKey.symmetric.value,AES256_KEY_LEN,so_container_0_ptr->secret.value,so_container_0_ptr->secret.len);
    } else if (so_container_0_ptr->object.cls == CKO_PUBLIC_KEY) {
      if ((so_container_0_ptr->key.flags & CKF_SO_INIT) == 0) {
        TL_LOG_E("Key does not allow being used for token initialization");
        ret = CKR_ARGUMENTS_BAD;
        break;
      }
      // proceed to decode
      g_token_0_ptr->associatedKey.asymmetric.modulus_len=MODULUS_MAX_LEN;
      g_token_0_ptr->associatedKey.asymmetric.exponent_len=PUBLIC_EXPONENT_LEN;
      result = DER_decode_RSAPublicKey(
          so_container_0_ptr->pub.derData,
          so_container_0_ptr->pub.derDataLen,
          g_token_0_ptr->associatedKey.asymmetric.exponent,
          &g_token_0_ptr->associatedKey.asymmetric.exponent_len,
          g_token_0_ptr->associatedKey.asymmetric.modulus,
          &g_token_0_ptr->associatedKey.asymmetric.modulus_len);

      if (!IS_OK(result)) {
        TL_LOG_E("Failed to decode data!");
        ret = CKR_PIN_INVALID;
        break;
      }
      if (g_token_0_ptr->associatedKey.asymmetric.modulus_len != 256) {
        TL_LOG_E("Unsupported key length: %d!",g_token_0_ptr->associatedKey.asymmetric.modulus_len);
        ret = CKR_PIN_INVALID;
        break;
      }
      g_token_0_ptr->associatedKey.header.type = CKK_RSA;
    } else {
      TL_LOG_E("Unsupported key type: 0x%08X",so_container_0_ptr->object.cls);
      ret = CKR_ARGUMENTS_BAD;
      break;
    }
    // key is good for us
    g_token_0_ptr->associatedKey.header.valid = 1;

    g_token_0_ptr->info.flags |= CKF_TOKEN_INITIALIZED;

    // generate the key used to bind token objects
    randomLen = sizeof(g_token_0_ptr->tokenStamp);
    result = tlApiRandomGenerateData(TLAPI_ALG_PSEUDO_RANDOM,(uint8_t*)(&g_token_0_ptr->tokenStamp),&randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
    if (randomLen != sizeof(g_token_0_ptr->tokenStamp)) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",randomLen,sizeof(g_token_0_ptr->tokenStamp));
      ret = CKR_FUNCTION_FAILED;
      break;
    }
    TL_LOG_I("Token stamp: 0x%08X",g_token_0_ptr->tokenStamp);
    g_token_0_ptr->tokenStamp |= TOKEN_STAMP_MASK;

    // tokens created with InitTokenSO are remote
    g_token_0_ptr->tokenIsLocal = 0;

    // set supported functions from TL
    g_token_0_ptr->enabledFunctions = g_config.arg.enabledFunctions;
    g_token_0_ptr->securityExtensions = g_config.arg.securityExtensions;

    // wrap the token
    result = wrapToken(g_token_0_ptr,&tci_message->rspInitTokenSO.params.so_token);
    if (!IS_OK(result)) {
      TL_LOG_E("wrapToken: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }

  } while(0);

  onAuthFailed(ret);

	TL_LOG_I("processCmdInitTokenSO- 0x%08X",ret);
  return ret;
}
