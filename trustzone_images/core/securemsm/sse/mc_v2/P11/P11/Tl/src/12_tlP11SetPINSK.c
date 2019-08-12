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

static bool_t verPinDone;
static bool_t verFailed;

/** Process a SetPIN command using the secure keypad
 *
 * @param tci shared buffer containing the command and our working area
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdSetPINSKInit(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t index;
  uint64_t now=0;

  do {
    TL_LOG_I("processCmdSetPINSKInit+");

    // unwrap token and session group
    ret = processTokenGroupSession(
        &tci_message->cmdSetPINSKInit.params.so_token,
        &tci_message->cmdSetPINSKInit.params.so_group,
        tci_message->cmdSetPINSKInit.params.sessionId,
        &g_token_0,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }

    // this operation requires R/W access to the token
    // SO is NOT allowed to perform it
    if (g_session_group_0.authInfo.authEntity == CKU_SO) {
      TL_LOG_E("SO logged in.");
      ret = CKR_USER_NOT_LOGGED_IN;
      break;
    }
    // we cannot use checkAccess here, since the condition is sort of mixed
    // this command is allowed either if CKU_NONE or CKU_USER are logged in
    // BUT it is a command on the token.
    if (g_tsSource != TS_SOURCE_ILLEGAL) {
      if (!IS_OK(tlApiGetTimeStamp(&g_session_group_0.authInfo.lastAuthTime,g_tsSource))) {
        now = UINT64_MAX;
      }
      if ((g_session_group_0.authInfo.authEntity != CKU_NONE) &&
          (now - g_session_group_0.authInfo.lastAuthTime > g_token_0.authSpan) &&
          (g_token_0.authSpan > 0)) {
        g_session_group_0.authInfo.authEntity = CKU_NONE;
        g_session_group_0.changed = 1;
      }
    }
    if ((g_session_group_0.authInfo.authEntity != CKU_NONE) &&
        (g_session_group_0.authInfo.usageNumLeft == 0) &&
        (g_token_0.usageNum > 0)) {
      g_session_group_0.authInfo.authEntity = CKU_NONE;
      g_session_group_0.changed = 1;
    }
    if ((g_session_group_0.sessions[index].access & SESSION_ACCESS_RW) == 0) // session is RO
    {
      TL_LOG_E("No RW access!");
      ret = CKR_SESSION_READ_ONLY;
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
      memset(pin,0,sizeof(pin));
      memset(pinVerification,0,sizeof(pinVerification));
      pinLen = 0;
      pinVerLen = 0;
      pinMaxLen = g_token_0.info.ulMaxPinLen;
      pinFailed = 0;
      pinEntered = 0;

      verPinDone = 0;
      verFailed = 0;
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
    tci_message->rspSetPINSKInit.params.event = tlp11_keypd_event_started;
    ret = CKR_SK_CONTINUE;
  } while(0);

  do {
    if (g_session_group_0.changed) {
      // wrap the session group
      result = wrapSessionGroup(&g_session_group_0,&tci_message->rspSetPINSKInit.params.so_group);
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        if (ret == CKR_SK_CONTINUE) {
          // we have to release the keypad before exiting with error
          tlApiReleaseKeypad();
        }
        ret = TL2CK(result);
        break;
      }
    }
  }while(0);
  onAuthFailed(ret);

	TL_LOG_I("processCmdSetPINSKInit- 0x%08X",ret);
  return ret;
}

tciReturnCode_t processCmdSetPINSKContinue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_SK_CONTINUE;
  tlApiResult_t result = TLAPI_OK;

  do {
    TL_LOG_I("processCmdSetPINSKContinue+");

    if (verPinDone == 0) {
      // get the PIN
      ret = getPIN(
          g_token_0.info.ulMinPinLen,
          pinMaxLen,
          pin,
          &pinLen,
          &(tci_message->rspSetPINSKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspSetPINSKContinue.params.event == tlp11_keypd_event_password_done) {
        verPinDone=1;
        // verify that pin against what is in the token
        if (pinFailed == 1) {
          TL_LOG_E("PIN too long");
          verFailed = 1;
        } else if (pinLen != g_token_0.pinLen) {
          TL_LOG_E("Wrong length");
          verFailed = 1;
        } else if (0 != timesafe_memcmp(pin,g_token_0.pin,g_token_0.pinLen)) {
          TL_LOG_E("PIN doesn't match");
          verFailed = 1;
        } else {
          TL_LOG_I("Old PIN is good.");
          verFailed = 0;
          // the PIN we got matches the PIN in the token, we can proceed and
          // get the new PIN
        }
        // get ready to receive the new pin
        memset(pin,0,sizeof(pin));
        pinLen = 0;
        pinMaxLen = g_token_0.info.ulMaxPinLen;
        pinFailed = 0;
      }
    } else if (pinEntered == 0) {
      // entering the new PIN
      ret = getPIN(
          g_token_0.info.ulMinPinLen,
          pinMaxLen,
          pin,
          &pinLen,
          &(tci_message->rspSetPINSKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspSetPINSKContinue.params.event == tlp11_keypd_event_password_done) {
        pinEntered=1;
      }
    } else {
      // confirming the new PIN
      // get the confirmation of the PIN already entered
      ret = getPIN(
          pinLen,
          pinLen,
          pinVerification,
          &pinVerLen,
          &(tci_message->rspSetPINSKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspSetPINSKContinue.params.event == tlp11_keypd_event_password_done) {
        // verify the entered PIN
        tci_message->rspSetPINSKContinue.params.event = tlp11_keypd_event_confirmation_done;
         ret = verifySavePIN(
          &pinFailed,
          pin,
          &pinLen,
          pinVerification,
          &pinVerLen,
          &g_token_0
          );
        g_token_0.changed = 1;
        ret = CKR_OK;
      }
    }
  } while(0);

  // now, tidy up token
  do {
    if (verFailed) {
      // failed, we need to update the lock flags
      g_token_0.loginAttempts++; // it counts as a failed login attempt
      g_token_0.info.flags |= CKF_USER_PIN_COUNT_LOW;
      if (g_token_0.loginAttempts == g_config.arg.maxLocalLoginAttempts-1) {
        g_token_0.info.flags |= CKF_USER_PIN_FINAL_TRY;
      } else if (g_token_0.loginAttempts == g_config.arg.maxLocalLoginAttempts) {
        lockToken(&g_token_0,CKU_USER);
      }
      g_token_0.changed = 1;
      ret = CKR_PIN_INCORRECT;
    } else if ((!verFailed) && (verPinDone)) {
      // The old PIN was successful, clear flags
      g_token_0.loginAttempts = 0;
      g_token_0.info.flags &= ~CKF_USER_PIN_COUNT_LOW;
      g_token_0.info.flags &= ~CKF_USER_PIN_FINAL_TRY;
      // here the token has already been unlocked (if it was locked), or the
      // command wouldn't have been accepted
      g_token_0.changed = 1;
    }
    if (g_token_0.changed) {
      result = wrapToken(&g_token_0,&(tci_message->rspSetPINSKContinue.params.so_token));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapToken: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

  onAuthFailed(ret);
  TL_LOG_I("processCmdSetPINSKContinue- ret:0x%08X",ret);
  return ret;
}
