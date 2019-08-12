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
#include "LayoutUtils.h"

/** Setup
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdSetup(
   tlSecureTouchCommandPayload_t *command,
   tlSecureTouchResponsePayload_t *response
)
{
  tlApiResult_t result = TLAPI_OK;
  size_t soLen = sizeof(command->setup.request.clear);

  ENTERRV;
  do {
    if (g_sid != DR_SID_INVALID) {
      LOG_E("Already processing a page request");
      rv = E_ST_BUSY;
      break;
    }
    // we need to keep a copy to know who we will have to respond to
    g_request = command->setup.request;
    DUMPHEX("Request",g_request.so,sizeof(g_request.so));
    result = tlApiUnwrapObjectExt(g_request.so,
                                  sizeof(g_request.so),
                                  &g_request.clear,
                                  &soLen,
                                  TLAPI_UNWRAP_DEFAULT | TLAPI_UNWRAP_PERMIT_DELEGATED);
    if (!IS_OK(result)) {
      LOG_E("Error unwrapping command request: 0x%08X", result);
      rv = E_SSE_INVALID_INPUT;
      break;
    }
    if (soLen != sizeof(g_request.clear)) {
      LOG_E("Request unwrapped to an unknown length object, rejecting")
      rv = E_SSE_INVALID_INPUT;
      break;
    }
    // verify request
    if (g_request.clear.nInputFields > MAX_INPUT_FIELDS) {
      LOG_E("Too many requested input fields: %d vs %d",
            g_request.clear.nInputFields,
            MAX_INPUT_FIELDS);
      rv = E_SSE_INVALID_INPUT;
      break;
    }
    if (g_request.clear.nInputFields == 0) {
      LOG_E("No input fields requested");
      rv = E_SSE_INVALID_INPUT;
      break;
    }
    // request is good, verify the layout
    rv = verifyLayout(&command->setup.page, &g_inputStatus, g_config.arg.w, g_config.arg.h);
    if (rv != SSE_OK) {
      LOG_E("Failed to vefify layout");
      rv = E_ST_INVALID_LAYOUT;
      break;
    }
    if (g_inputStatus.num_input != g_request.clear.nInputFields) {
      LOG_E("Mismatch in input fields: %d vs %d",
            g_inputStatus.num_input,
            g_request.clear.nInputFields);
      rv = E_SSE_INVALID_INPUT;
      break;
    }
    // open the connection with the driver
    result = tlApiTsOpen(&g_sid);
    if (!IS_OK(result) || (g_sid == DR_SID_INVALID)) {
      LOG_E("Error opening session with driver: 0x%08X, sid=0x%08X",result,g_sid);
      rv = E_ST_DRIVER_BUSY;
      break;
    }
    // keep a copy
    g_page = command->setup.page;
    // request is good, setup the response structure
    memset(&g_response, 0, sizeof(g_response));
    g_response.clear.nonce = g_request.clear.nonce;
    LOG_I("Setting nonce: 0x%08X", g_response.clear.nonce);
    memset(&g_inputStatus, 0, sizeof(g_inputStatus));
    g_inputStatus.focus = 0; // focus by default on the first input field
    g_inputStatus.status = SECURETOUCH_COMPLETION_STATUS_INPROGRESS;
    g_inputStatus.num_input = g_request.clear.nInputFields;
    response->setup.page = g_page; // we copy it back to let caller know about input indexing
  } while (0);
  EXITRV;
}
