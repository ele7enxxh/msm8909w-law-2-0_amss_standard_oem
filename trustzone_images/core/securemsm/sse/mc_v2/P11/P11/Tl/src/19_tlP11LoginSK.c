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
#include "tlLog.h"
#include "tlP11Glob.h"

/** Process a Login command
 * The secured token the user is authenticating against is passed in, together with the pin.
 * If the pin is empty, the secure keypad is used.
 * The (potentially) authenticated token is returned.
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */

tciReturnCode_t processCmdLoginSKInit(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_SK_CONTINUE;
  tlApiResult_t result = TLAPI_OK;
  uint32_t index;

  do {
    TL_LOG_I("processCmdLoginSKInit+");

    ret = processTokenGroupSession(
        &tci_message->cmdLoginSKInit.params.so_token,
        &tci_message->cmdLoginSKInit.params.so_group,
        tci_message->cmdLoginSKInit.params.sessionId,
        &g_token_0,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }

    if (CKU_USER == g_session_group_0.authInfo.authEntity) {
      ret = CKR_USER_ALREADY_LOGGED_IN;
      break;
    }
    if (g_session_group_0.authInfo.authEntity != CKU_NONE) {
      ret = CKR_USER_ANOTHER_ALREADY_LOGGED_IN;
      break;
    }

    if (g_token_0.pinLen == 0) {
      ret = CKR_USER_PIN_NOT_INITIALIZED;
      break;
    }
    if ((g_token_0.info.flags & CKF_USER_PIN_LOCKED) &&
        (unlockToken(&g_token_0) & CKF_USER_PIN_LOCKED) ){
      ret = CKR_PIN_LOCKED;
      break;
    }
    // ok to proceed, let's get the PIN
    if ((g_config.arg.flags & CKF_PROTECTED_AUTHENTICATION_PATH) &&
        (g_token_0.info.flags & CKF_PROTECTED_AUTHENTICATION_PATH)) {
      pinLen = TOKEN_PIN_LEN;
      memset(pin,0,sizeof(pin));
      pinLen = 0;
      pinMaxLen = g_token_0.pinLen; // trying to login on this pin, no reason to accept anything longer
      pinFailed = 0;
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
    tci_message->rspLoginSKInit.params.event = tlp11_keypd_event_started;
    ret = CKR_SK_CONTINUE;

  } while(0);

	TL_LOG_I("processCmdLoginSKInit- ret:0x%08X",ret);
  return ret;
}

tciReturnCode_t processCmdLoginSKContinue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_SK_CONTINUE;
  tlApiResult_t result = TLAPI_OK;

  do {
    TL_LOG_I("processCmdLoginSKContinue+");
    // login does not prompt for verification
    // get the PIN
    ret = getPIN(
        g_token_0.pinLen,
        g_token_0.pinLen,
        pin,
        &pinLen,
        &(tci_message->rspLoginSKContinue.params.event),
        &pinFailed
        );
    if (ret != CKR_SK_CONTINUE) {
      break;
    }

    if (tci_message->rspLoginSKContinue.params.event == tlp11_keypd_event_password_done) {
      // verify the entered PIN
      if (pinFailed == 1) {
        TL_LOG_E("PIN too long");
        ret = CKR_PIN_LEN_RANGE;
        break;
      }
      if (pinLen != g_token_0.pinLen) {
        TL_LOG_E("Wrong length");
        ret = CKR_PIN_INCORRECT;
        break;
      }
      if (0 != timesafe_memcmp(pin,g_token_0.pin,g_token_0.pinLen)) {
        TL_LOG_E("PIN doesn't match");
        ret = CKR_PIN_INCORRECT;
        break;
      }
      g_session_group_0.authInfo.authEntity = CKU_USER;
      g_session_group_0.authInfo.usageNumLeft = g_token_0.usageNum;

      memset(pin,0,sizeof(pin));
      pinLen = 0;
      pinMaxLen = 0;
      pinFailed = 0;
      g_session_group_0.authInfo.lastAuthTime = 0;
      if ((g_tsSource != TS_SOURCE_ILLEGAL) &&
          (!IS_OK(tlApiGetTimeStamp(&g_session_group_0.authInfo.lastAuthTime,g_tsSource)))) {
        g_session_group_0.authInfo.lastAuthTime = 0;
      }
      ret = CKR_OK;
    }

  } while(0);

  // now, tidy up token
  do {
    if (ret == CKR_OK) {
      g_token_0.loginAttempts = 0;
      g_token_0.info.flags &= ~CKF_USER_PIN_COUNT_LOW;
      g_token_0.info.flags &= ~CKF_USER_PIN_FINAL_TRY;
      g_token_0.info.flags &= ~CKF_USER_PIN_LOCKED;
    } else if (ret == CKR_PIN_INCORRECT) {
      // login failed, we need to update the lock flags
      g_token_0.loginAttempts++;
      g_token_0.info.flags |= CKF_USER_PIN_COUNT_LOW;
      if (g_token_0.loginAttempts == g_config.arg.maxLocalLoginAttempts-1) {
        g_token_0.info.flags |= CKF_USER_PIN_FINAL_TRY;
      } else if (g_token_0.loginAttempts == g_config.arg.maxLocalLoginAttempts) {
        lockToken(&g_token_0,CKU_USER);
      }
    } else {
      // for all other error values we don't actually have to update anything
      break;
    }
    result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspLoginSKContinue.params.so_group));
    if (!IS_OK(result)) {
      TL_LOG_E("wrapSessionGroup: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
    result = wrapToken(&g_token_0,&(tci_message->rspLoginSKContinue.params.so_token));
    if (!IS_OK(result)) {
      TL_LOG_E("wrapToken: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
  } while(0);

  onAuthFailed(ret);
  TL_LOG_I("processCmdLoginSKContinue- ret:0x%08X",ret);
  return ret;
}
