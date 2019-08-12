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

/** Process a InitPIN command
 *
 * @param tci shared buffer containing the command and our working area
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdInitPIN(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t index;

  do {
    TL_LOG_I("processCmdInitPIN+");

    g_token_0_ptr = (tlp11_token_t*) tci_message->cmdInitPIN.params.so_token.value;
    // unwrap token and session group
    ret = processTokenGroupSession(
        &tci_message->cmdInitPIN.params.so_token,
        &tci_message->cmdInitPIN.params.so_group,
        tci_message->cmdInitPIN.params.sessionId,
        g_token_0_ptr,
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
        g_token_0_ptr,
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
        (g_token_0_ptr->info.flags & CKF_PROTECTED_AUTHENTICATION_PATH)){
      // this version of the command CANNOT be called on token supporting
      // the secure keypad
      TL_LOG_E("This token supports secure keypad!");
      ret = CKR_FUNCTION_FAILED;
      break;
    } else {
      // use pin as passed
      if ((tci_message->cmdInitPIN.params.pinLen < g_token_0_ptr->info.ulMinPinLen) ||
          (tci_message->cmdInitPIN.params.pinLen > g_token_0_ptr->info.ulMaxPinLen)) {
        ret = CKR_PIN_LEN_RANGE;
        break;
      }
      g_token_0_ptr->pinLen = tci_message->cmdInitPIN.params.pinLen;
      memscpy(g_token_0_ptr->pin,TOKEN_PIN_LEN,tci_message->cmdInitPIN.params.pin,g_token_0_ptr->pinLen);
    }
    g_token_0_ptr->info.flags |= CKF_USER_PIN_INITIALIZED;
    g_token_0_ptr->info.flags &= ~CKF_USER_PIN_LOCKED;

    // wrap the token
    result = wrapToken(g_token_0_ptr,&tci_message->rspInitPIN.params.so_token);
    if (!IS_OK(result)) {
      TL_LOG_E("wrapStore: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
  } while(0);

  do {
    if (g_session_group_0.changed) {
      // wrap the session group
      result = wrapSessionGroup(&g_session_group_0,&tci_message->rspInitPIN.params.so_group);
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  }while(0);

  onAuthFailed(ret);

	TL_LOG_I("processCmdInitPIN- 0x%08X",ret);
  return ret;
}
