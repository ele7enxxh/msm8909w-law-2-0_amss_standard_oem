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

/** Process a Logout command
 * The secured token the user is authenticating against is passed in, together with the pin.
 * If the pin is empty, the secure keypad is used.
 * The (potentially) authenticated token is returned.
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdLogout(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t index;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdLogout.params.so_token.value;

  do {
    TL_LOG_I("processCmdLogout+");

    ret = processTokenGroupSession(
        &tci_message->cmdLogout.params.so_token,
        &tci_message->cmdLogout.params.so_group,
        tci_message->cmdLogout.params.sessionId,
        g_token_0_ptr,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }

    if (g_session_group_0.authInfo.authEntity == CKU_NONE) {
      ret = CKR_USER_NOT_LOGGED_IN;
      break;
    }
    g_session_group_0.authInfo.authEntity = CKU_NONE;
    result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspLogout.params.so_group));
    if (!IS_OK(result)) {
      TL_LOG_E("wrapSessionGroup: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
  } while(0);

	TL_LOG_I("processCmdLogout- ret:0x%08X",ret);
  return ret;
}
