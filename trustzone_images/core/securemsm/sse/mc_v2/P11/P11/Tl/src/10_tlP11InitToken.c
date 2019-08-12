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

/** Process a InitToken command
 *
 * This command returns the wrapped version of the token.
 *
 * @param tci shared buffer containing the command and our working area
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdInitToken(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint8_t i = 0;
  size_t randomLen = 0;

  do {
    TL_LOG_I("processCmdInitToken+");

    if (g_config.arg.allowLocalToken != 1) {
      TL_LOG_E("Local-only tokens not allowed!");
      ret = CKR_DEVICE_ERROR;
      break;
    }

    // initialize the token pointer
    g_token_0_ptr = (tlp11_token_t*) tci_message->rspInitToken.params.so_token.value;
    memset(g_token_0_ptr,0,sizeof(tlp11_token_t));
    // copy over the label from input
    memscpy(g_token_0_ptr->info.label,TOKEN_LABEL_LEN,tci_message->cmdInitToken.params.label,sizeof(g_token_0_ptr->info.label));

    // sanitise the label, replace '0' with <space>
    for(i=0; i<sizeof(g_token_0_ptr->info.label); i++) {
      if (g_token_0_ptr->info.label[i] == 0)
    	  g_token_0_ptr->info.label[i]=' ';
    }

    // fill in the other default values
    fillTokenInfo(&g_token_0_ptr->info);
    g_token_0_ptr->authSpan = g_config.arg.maxAuthSpan;
    g_token_0_ptr->usageNum = g_config.arg.maxUsageNum;

    if ((g_token_0_ptr->info.ulMaxPinLen < g_token_0_ptr->info.ulMinPinLen) ||
        (g_token_0_ptr->info.ulMaxPinLen > TOKEN_PIN_LEN) ||
        (g_token_0_ptr->info.ulMinPinLen < TOKEN_MIN_PIN_LEN)){
      TL_LOG_E("Invalid pin length!");
      ret = CKR_GENERAL_ERROR;
      break;
    }
    if (g_config.arg.flags & CKF_PROTECTED_AUTHENTICATION_PATH) {
      // this version of the command CANNOT be called on token supporting
      // the secure keypad
      TL_LOG_E("This token does supports secure keypad!");
      ret = CKR_FUNCTION_FAILED;
      break;
    }
    // use pin as passed
    g_token_0_ptr->pinLen = tci_message->cmdInitToken.params.pinLen;
    if ((g_token_0_ptr->pinLen < g_token_0_ptr->info.ulMinPinLen) ||
        (g_token_0_ptr->pinLen > g_token_0_ptr->info.ulMaxPinLen)) {
      ret = CKR_PIN_INCORRECT;
      break;
    }
    memscpy(&g_token_0_ptr->pin,TOKEN_PIN_LEN,tci_message->cmdInitToken.params.pin,g_token_0_ptr->pinLen);

    g_token_0_ptr->info.flags |= CKF_USER_PIN_INITIALIZED;
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

    // tokens created with InitToken are local only
    g_token_0_ptr->tokenIsLocal = 1;

    // set supported functions from TL
    g_token_0_ptr->enabledFunctions = g_config.arg.enabledFunctions;
    g_token_0_ptr->securityExtensions = g_config.arg.securityExtensions;

    // wrap the token
    result = wrapToken(g_token_0_ptr,&tci_message->rspInitToken.params.so_token);
    if (!IS_OK(result)) {
      TL_LOG_E("wrapToken: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }

  } while(0);

  onAuthFailed(ret);

	TL_LOG_I("processCmdInitToken- 0x%08X",ret);
  return ret;
}
