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

/** Process a CloseSession command
 *
 * @param tci shared buffer containing the command and our working area
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdCloseSession(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t sessionCount = 0;
  uint32_t index;
  g_token_0_ptr = (tlp11_token_t*)tci_message->cmdCloseSession.params.so_token.value;

  do {
    TL_LOG_I("processCmdCloseSession+");

    // unwrap token and session group
    ret = processTokenGroupSession(
        &tci_message->cmdCloseSession.params.so_token,
        &tci_message->cmdCloseSession.params.so_group,
        tci_message->cmdCloseSession.params.sessionId,
        g_token_0_ptr,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }
    // if a cryptographic session is ongoing, cancel it
    if (g_session_group_0.sessions[index].context.cryptoSession != CR_SID_INVALID) {
      result = tlApiCrAbort(g_session_group_0.sessions[index].context.cryptoSession);
      if (!IS_OK(result)) {
        TL_LOG_E("Failed to abort the session: 0x%08X",result);
      }
    }

    memset(&g_session_group_0.sessions[index],0,sizeof(tlp11_session_t));

    ret = getSessionCount(&g_session_group_0,&sessionCount,NULL,NULL);
    if (CKR_OK != ret) {
      TL_LOG_E("Error parsing sessions");
      break;
    }

    TL_LOG_I("Remaining sessions: %d",sessionCount);

    if (sessionCount != 0) {
      // we need to pass out the session_group only if there are open sessions

      // wrap the session group
      result = wrapSessionGroup(&g_session_group_0,&tci_message->rspCloseSession.params.so_group);
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    } else {
      tci_message->rspCloseSession.params.so_group.len = 0;
    }

  } while(0);

	TL_LOG_I("processCmdCloseSession- 0x%08X",ret);
  return ret;
}
