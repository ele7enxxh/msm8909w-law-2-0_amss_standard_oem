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

/** Process
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdProcess(
  tlSecureTouchCommandPayload_t* command,
  tlSecureTouchResponsePayload_t* response
)
{
  tlApiResult_t result = TLAPI_OK;
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t code = 0;
  struct tsFingerData fingers = {0};
  uint32_t touches = 0;
  ENTERRV;

  do {
    // check that there is an active session
    if ((g_sid == DR_SID_INVALID)) {
      LOG_E("No active session!");
      rv = E_ST_NO_SESSION_ACTIVE;
      break;
    }
    result = tlApiTsGetData(g_sid, &fingers);
    if (!IS_OK(result)) {
      LOG_E("Error getting data from driver", result);
      rv = E_ST_DRIVER_ERROR;
      break;
    }
    // we are interested only in single touch events
    for (uint32_t i = 0; i < MAX_FINGER_NUM; i++) {
      if (fingers.finger[i].event != TLAPI_TOUCH_EVENT_NONE) {
        touches++;
        x = fingers.finger[i].x;
        y = fingers.finger[i].y;
        code = fingers.finger[i].event;
      }
    }
    if (touches != 1) {
      LOG_W("Unsupported number of touches detected, ignored: %u", touches);
      break;
    }
    // by default, we mask everything, also touches that don't
    // hit on any element, as data touches
    response->process.event = SECURETOUCH_EVENT_DATA;
    if ((code & TLAPI_TOUCH_EVENT_UP) == 0) {
      break;
    }
    rv = updatePage(x,y,&g_page,&g_inputStatus,&g_response.clear, &response->process.event);
    
  } while (0);
  if (rv == SSE_OK) {
    response->process.focusIndex = g_inputStatus.focus;
    LOG_D("Focus on input element %u", response->process.focusIndex);
    for (uint32_t i = 0; i < g_inputStatus.num_input; i++) {
      response->process.num_chars[i] = g_inputStatus.num_chars[i];
      LOG_D("num_chars[%u] = %u", i, g_inputStatus.num_chars[i]);
    }
  }
  EXITRV;
}
