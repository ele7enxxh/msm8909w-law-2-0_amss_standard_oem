/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "tlSecureTouchApi.h"
#include "SecureTouchError.h"
#include "TouchApi.h"
#include "glob.h"
#include <string.h>

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
  uint32_t result = SSE_OK;
  //size_t soLen = SECURETOUCH_RESPONSE_SO_SIZE;

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
    // TODO: wrap the response
    response->teardown.response.clear = g_response.clear;
    // all ok, close driver
    result = touchClose(g_sid);
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
