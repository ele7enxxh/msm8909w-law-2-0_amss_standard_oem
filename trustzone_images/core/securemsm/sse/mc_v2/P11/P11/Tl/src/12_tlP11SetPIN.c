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

/** Process a SetPIN command
 *
 * @param tci shared buffer containing the command and our working area
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdSetPIN(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t index;
  uint64_t now=0;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdSetPIN.params.so_token.value;

  do {
    TL_LOG_I("processCmdSetPIN+");

    // unwrap token and session group
    ret = processTokenGroupSession(
        &tci_message->cmdSetPIN.params.so_token,
        &tci_message->cmdSetPIN.params.so_group,
        tci_message->cmdSetPIN.params.sessionId,
        g_token_0_ptr,
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
          (now - g_session_group_0.authInfo.lastAuthTime > g_token_0_ptr->authSpan) &&
          (g_token_0_ptr->authSpan > 0)) {
        g_session_group_0.authInfo.authEntity = CKU_NONE;
        g_session_group_0.changed = 1;
      }
    }
    if ((g_session_group_0.authInfo.authEntity != CKU_NONE) &&
        (g_session_group_0.authInfo.usageNumLeft == 0) &&
        (g_token_0_ptr->usageNum > 0)) {
      g_session_group_0.authInfo.authEntity = CKU_NONE;
      g_session_group_0.changed = 1;
    }
    if ((g_session_group_0.sessions[index].access & SESSION_ACCESS_RW) == 0) // session is RO
    {
      TL_LOG_E("No RW access!");
      ret = CKR_SESSION_READ_ONLY;
      break;
    }

    if ((g_token_0_ptr->info.flags & CKF_USER_PIN_LOCKED) &&
        (unlockToken(g_token_0_ptr) & CKF_USER_PIN_LOCKED) ){
      ret = CKR_PIN_LOCKED;
      break;
    }

    // if we got to this point, we're good to go
    if ((g_config.arg.flags & CKF_PROTECTED_AUTHENTICATION_PATH) &&
        (g_token_0_ptr->info.flags & CKF_PROTECTED_AUTHENTICATION_PATH)){
      // this version of the command CANNOT be called on token supporting
      // the secure keypad
      TL_LOG_E("This token does supports secure keypad!");
      ret = CKR_FUNCTION_FAILED;
      break;
    }
    // use pin as passed
    if (tci_message->cmdSetPIN.params.oldPinLen != g_token_0_ptr->pinLen) {
      ret = CKR_PIN_INCORRECT;
      break;
    }
    if ((tci_message->cmdSetPIN.params.newPinLen < g_token_0_ptr->info.ulMinPinLen) ||
        (tci_message->cmdSetPIN.params.newPinLen > g_token_0_ptr->info.ulMaxPinLen)) {
      ret = CKR_PIN_LEN_RANGE;
      break;
    }

    if (0 != timesafe_memcmp(tci_message->cmdSetPIN.params.oldPin,g_token_0_ptr->pin,g_token_0_ptr->pinLen)) {
      ret = CKR_PIN_INCORRECT;
     break;
    }
    memscpy(g_token_0_ptr->pin,TOKEN_PIN_LEN,tci_message->cmdSetPIN.params.newPin,tci_message->cmdSetPIN.params.newPinLen);
    g_token_0_ptr->pinLen = tci_message->cmdSetPIN.params.newPinLen;
    g_token_0_ptr->changed = 1;

  } while(0);

  // now, tidy up token
  do {
    if (ret == CKR_OK) {
      // SetPIN was successful, clear flags
      g_token_0_ptr->loginAttempts = 0;
      g_token_0_ptr->info.flags &= ~CKF_USER_PIN_COUNT_LOW;
      g_token_0_ptr->info.flags &= ~CKF_USER_PIN_FINAL_TRY;
      // here the token has already been unlocked (if it was locked), or the
      // command wouldn't have been accepted
      g_token_0_ptr->changed = 1;
    } else if (ret == CKR_PIN_INCORRECT) {
      // failed, we need to update the lock flags
      g_token_0_ptr->loginAttempts++; // it counts as a failed login attempt
      g_token_0_ptr->info.flags |= CKF_USER_PIN_COUNT_LOW;
      if (g_token_0_ptr->loginAttempts == g_config.arg.maxLocalLoginAttempts-1) {
        g_token_0_ptr->info.flags |= CKF_USER_PIN_FINAL_TRY;
      } else if (g_token_0_ptr->loginAttempts == g_config.arg.maxLocalLoginAttempts) {
        lockToken(g_token_0_ptr,CKU_USER);
      }
      g_token_0_ptr->changed = 1;
    }
    if (g_token_0_ptr->changed) {
      result = wrapToken(g_token_0_ptr,&(tci_message->rspSetPIN.params.so_token));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapToken: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
    if (g_session_group_0.changed) {
      // wrap the session group
      result = wrapSessionGroup(&g_session_group_0,&tci_message->rspSetPIN.params.so_group);
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);
  onAuthFailed(ret);

	TL_LOG_I("processCmdSetPIN- 0x%08X",ret);
  return ret;
}
