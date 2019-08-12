/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlSecureTouchApi.h"
#include "SecureTouchError.h"
#include "TlApiTs.h"
#include "tlGlob.h"

/** Teardown
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdTeardown(
  tlSecureTouchCommandPayload_t* command,
  tlSecureTouchResponsePayload_t* response
)
{
  ENTERRV;
  tlApiResult_t result = TLAPI_OK;
  size_t soLen = SECURETOUCH_RESPONSE_SO_SIZE;
  tlApiSpTrustletId_t spTrustletId = {0};

  do {
    // check that there is an active session
    if ((g_sid == DR_SID_INVALID)) {
      LOG_E("No active session!");
      rv = E_ST_NO_SESSION_ACTIVE;
      break;
    }
    if ((command->teardown.cmd != TL_SECURETOUCH_TEARDOWN_CMD_COMPLETE) &&
        (command->teardown.cmd != TL_SECURETOUCH_TEARDOWN_CMD_ABORT)) {
      LOG_E("Unknown teardown command: 0x%08X", command->teardown.cmd);
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }
    if ((command->teardown.cmd == TL_SECURETOUCH_TEARDOWN_CMD_COMPLETE) &&
        (g_inputStatus.status == SECURETOUCH_COMPLETION_STATUS_INPROGRESS)) {
      LOG_E("Not yet completed by the user, cannot complete");
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }
    // prepare response SO
    if (command->teardown.cmd == TL_SECURETOUCH_TEARDOWN_CMD_ABORT) {
      LOG_I("Aborting...");
      memset(g_response.clear.length, 0, sizeof(g_response.clear.length));
      memset(g_response.clear.input_fields, 0, sizeof(g_response.clear.input_fields));
      g_response.clear.status = SECURETOUCH_COMPLETION_STATUS_ABORTED;
    } else {
      LOG_I("Completing...");
      g_response.clear.status = g_inputStatus.status;
    }
    // wrap the object
    LOG_D("bytes in input 0: %u: %s", g_response.clear.length[0], (char*)g_response.clear.input_fields[0]);
    LOG_D("bytes in input 1: %u: %s", g_response.clear.length[1], (char*)g_response.clear.input_fields[1]);
    spTrustletId.spid = g_request.clear.srcSpid;
    spTrustletId.uuid = g_request.clear.srcUuid;
    LOG_D("Wrapping for %08X / %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
      spTrustletId.spid,
      spTrustletId.uuid.value[0],
      spTrustletId.uuid.value[1],
      spTrustletId.uuid.value[2],
      spTrustletId.uuid.value[3],
      spTrustletId.uuid.value[4],
      spTrustletId.uuid.value[5],
      spTrustletId.uuid.value[6],
      spTrustletId.uuid.value[7],
      spTrustletId.uuid.value[8],
      spTrustletId.uuid.value[9],
      spTrustletId.uuid.value[10],
      spTrustletId.uuid.value[11],
      spTrustletId.uuid.value[12],
      spTrustletId.uuid.value[13],
      spTrustletId.uuid.value[14],
      spTrustletId.uuid.value[15]);
    result = tlApiWrapObjectExt(
       &g_response.clear,
       PLAINLEN(SecureTouchResponse_t),
       ENCRYPTEDLEN(SecureTouchResponse_t),
       response->teardown.response.so,
       &soLen,
       MC_SO_CONTEXT_TLT,
       MC_SO_LIFETIME_POWERCYCLE,
       &spTrustletId,
       TLAPI_WRAP_DEFAULT
       );
    if (!IS_OK(result)) {
      LOG_E("Error wrapping the response: 0x%08X", result);
      rv = E_SSE_PANIC;
      break;
    }
    // all ok, close driver
    result = tlApiTsClose(g_sid);
    if (!IS_OK(result)) {
      LOG_E("Error closing session %d with driver: 0x%08X",g_sid,result);
      rv = E_SSE_PANIC;
      break;
    }
    // got to this point, we can clear our status
    memset(&g_page,0,sizeof(g_page));
    g_sid = DR_SID_INVALID;
    memset(&g_command, 0, sizeof(g_command));
    memset(&g_response, 0, sizeof(g_response));
    memset(&g_request, 0, sizeof(g_request));
    memset(&g_inputStatus, 0, sizeof(g_inputStatus));
  } while (0);
  EXITRV;
}
