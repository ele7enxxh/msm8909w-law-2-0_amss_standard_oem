/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlP11Utils.h"
#include "tlLog.h"
#include "tlP11Glob.h"

/** Process a InitPIN command
 *
 * @param tci shared buffer containing the command and our working area
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdInitPINSKInit(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t index;

  do {
    TL_LOG_I("processCmdInitPINSKInit+");

    // unwrap token and session group
    ret = processTokenGroupSession(
        &tci_message->cmdInitPINSKInit.params.so_token,
        &tci_message->cmdInitPINSKInit.params.so_group,
        tci_message->cmdInitPINSKInit.params.sessionId,
        &g_token_0,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }

    // this operation requires R/W access to the token AND only SO
    // is allowed to perform it
    if (g_session_group_0.authInfo.authEntity != CKU_SO) {
      TL_LOG_E("SO not logged in.");
      ret = CKR_USER_NOT_LOGGED_IN;
      break;
    }
    ret = checkAccess(
        &g_token_0,
        &g_session_group_0,
        index,
        SESSION_ACCESS_RW | SESSION_ACCESS_TOKEN
        );
    if (ret != CKR_OK) {
      TL_LOG_E("Error checking for access permissions");
      break;
    }

    // if we got to this point, we're good to go
    if ((g_config.arg.flags & CKF_PROTECTED_AUTHENTICATION_PATH) &&
        (g_token_0.info.flags & CKF_PROTECTED_AUTHENTICATION_PATH)){
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
      tci_message->rspInitPINSKInit.params.event = tlp11_keypd_event_started;
      ret = CKR_SK_CONTINUE;
      // we're good to return
    } else {
      TL_LOG_E("This method can be called only on SK enabled tokens!");
      ret = CKR_FUNCTION_FAILED;
      break;
    }
  } while(0);

  do {
    if (g_session_group_0.changed) {
      // wrap the session group
      result = wrapSessionGroup(&g_session_group_0,&tci_message->rspInitPINSKInit.params.so_group);
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        if (ret == CKR_SK_CONTINUE) {
          // we've grabbed the keypad, release it before exiting with error
          tlApiReleaseKeypad();
        }
        ret = TL2CK(result);
        break;
      }
    }
  }while(0);

  onAuthFailed(ret);

	TL_LOG_I("processCmdInitPINSKInit- 0x%08X",ret);
  return ret;
}

tciReturnCode_t processCmdInitPINSKContinue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_SK_CONTINUE;
  tlApiResult_t result = TLAPI_OK;

  do {
    TL_LOG_I("processCmdInitPINSKContinue+");

    if (pinEntered == 0) {
      // get the PIN
      ret = getPIN(
          g_token_0.info.ulMinPinLen,
          pinMaxLen,
          pin,
          &pinLen,
          &(tci_message->rspInitPINSKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspInitPINSKContinue.params.event == tlp11_keypd_event_password_done) {
        pinEntered=1;
      }
    } else {
      // get the confirmation of the PIN already entered
      ret = getPIN(
          pinLen,
          pinLen,
          pinVerification,
          &pinVerLen,
          &(tci_message->rspInitPINSKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspInitPINSKContinue.params.event == tlp11_keypd_event_password_done) {
        // verify the entered PIN
        tci_message->rspInitPINSKContinue.params.event = tlp11_keypd_event_confirmation_done;

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
      g_token_0.info.flags &= ~CKF_USER_PIN_LOCKED;
      // wrap the token
      result = wrapToken(&g_token_0,&tci_message->rspInitPINSKContinue.params.so_token);
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

  TL_LOG_I("processCmdInitPINSKContinue- 0x%08X",ret);
  return ret;
}
