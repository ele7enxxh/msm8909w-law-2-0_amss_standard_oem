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
tciReturnCode_t processCmdInitTokenSKInit(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_SK_CONTINUE;
  tlApiResult_t result = TLAPI_OK;
  uint8_t i = 0;

  do {
    TL_LOG_I("processCmdInitTokenSKInit+");

    if (g_config.arg.allowLocalToken != 1) {
      TL_LOG_E("Local-only tokens not allowed!");
      ret = CKR_DEVICE_ERROR;
      break;
    }

    // initialize the token
    memset(&g_token_0,0,sizeof(tlp11_token_t));
    // copy over the label from input
    memscpy(g_token_0.info.label,TOKEN_LABEL_LEN,tci_message->cmdInitTokenSKInit.params.label,sizeof(g_token_0.info.label));

    // sanitise the label, replace '0' with <space>
    for(i=0; i<sizeof(g_token_0.info.label); i++) {
      if (g_token_0.info.label[i] == 0)
        g_token_0.info.label[i]=' ';
    }

    // fill in the other default values
    fillTokenInfo(&g_token_0.info);
    g_token_0.authSpan = g_config.arg.maxAuthSpan;
    g_token_0.usageNum = g_config.arg.maxUsageNum;

    if ((g_token_0.info.ulMaxPinLen < g_token_0.info.ulMinPinLen) ||
        (g_token_0.info.ulMaxPinLen > TOKEN_PIN_LEN) ||
        (g_token_0.info.ulMinPinLen < TOKEN_MIN_PIN_LEN)){
      TL_LOG_E("Invalid pin length!");
      ret = CKR_GENERAL_ERROR;
      break;
    }
    if (g_config.arg.flags & CKF_PROTECTED_AUTHENTICATION_PATH) {
      memset(pin,0,sizeof(pin));
      memset(pinVerification,0,sizeof(pinVerification));
      pinLen = 0;
      pinVerLen = 0;
      pinMaxLen = g_token_0.info.ulMaxPinLen;
      pinFailed = 0;
      pinEntered = 0;
      // grab the keypad
      result = tlApiGrabKeypad();
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiGrabKeypad: %d",result);
        ret = TL2CK(result);
        break;
      }
      // we're good to return
    } else {
      TL_LOG_E("This method can be called only on SK enabled tokens!");
      ret = CKR_FUNCTION_FAILED;
      break;
    }
    tci_message->rspInitTokenSKInit.params.event = tlp11_keypd_event_started;
    ret = CKR_SK_CONTINUE;

  } while(0);

  onAuthFailed(ret);

	TL_LOG_I("processCmdInitTokenSKInit- 0x%08X",ret);
  return ret;
}

tciReturnCode_t processCmdInitTokenSKContinue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_SK_CONTINUE;
  tlApiResult_t result = TLAPI_OK;
  size_t randomLen = 0;

  do {
    TL_LOG_I("processCmdInitTokenSKContinue+");

    if (pinEntered == 0) {
      // get the PIN
      ret = getPIN(
          g_token_0.info.ulMinPinLen,
          pinMaxLen,
          pin,
          &pinLen,
          &(tci_message->rspInitTokenSKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspInitTokenSKContinue.params.event == tlp11_keypd_event_password_done) {
        pinEntered=1;
      }
    } else {
      // get the confirmation of the PIN already entered
      ret = getPIN(
          pinLen,
          pinLen,
          pinVerification,
          &pinVerLen,
          &(tci_message->rspInitTokenSKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspInitTokenSKContinue.params.event == tlp11_keypd_event_password_done) {
        // verify the entered PIN
        tci_message->rspInitTokenSKContinue.params.event = tlp11_keypd_event_confirmation_done;
         ret = verifySavePIN(
          &pinFailed,
          pin,
          &pinLen,
          pinVerification,
          &pinVerLen,
          &g_token_0
          );
      }
    }
  } while(0);

  // now, tidy up token
  do {
    if (ret == CKR_OK) {
      g_token_0.info.flags |= CKF_USER_PIN_INITIALIZED;
      g_token_0.info.flags |= CKF_TOKEN_INITIALIZED;

      // generate the key used to bind token objects
      randomLen = sizeof(g_token_0.tokenStamp);
      result = tlApiRandomGenerateData(TLAPI_ALG_PSEUDO_RANDOM,(uint8_t*)(&g_token_0.tokenStamp),&randomLen);
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
      if (randomLen != sizeof(g_token_0.tokenStamp)) {
        TL_LOG_E("Failed to generate enough random data: %d vs %d",randomLen,sizeof(g_token_0.tokenStamp));
        ret = CKR_FUNCTION_FAILED;
        break;
      }
      TL_LOG_I("Token stamp: 0x%08X",g_token_0.tokenStamp);
      g_token_0.tokenStamp |= TOKEN_STAMP_MASK;

      // tokens created with InitToken are local only
      g_token_0.tokenIsLocal = 1;

      // set supported functions from TL
      g_token_0.enabledFunctions = g_config.arg.enabledFunctions;
      g_token_0.securityExtensions = g_config.arg.securityExtensions;

      // wrap the token
      result = wrapToken(&g_token_0,&tci_message->rspInitTokenSKContinue.params.so_token);
      if (!IS_OK(result)) {
        TL_LOG_E("wrapStore: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    } else {
      // for all other error values we don't actually have to update anything
      break;
    }

  } while(0);

  onAuthFailed(ret);
  TL_LOG_I("processCmdInitTokenSKContinue- ret:0x%08X",ret);
  return ret;
}

tciReturnCode_t processCmdSKCancel(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;

  TL_LOG_I("processCmdSKCancel+");
  // safe measures
  pinFailed = 1;
  pinMaxLen = 0;
  pinLen = 0;
  memset(pin,0,TOKEN_PIN_LEN);
  result = tlApiReleaseKeypad();
  if (!IS_OK(result)){
    TL_LOG_E("Failed to release keypad, nothing we can do...");
  }
  TL_LOG_I("processCmdSKCancel- ret:0x%08X",ret);
  return ret;
} //lint !e715
