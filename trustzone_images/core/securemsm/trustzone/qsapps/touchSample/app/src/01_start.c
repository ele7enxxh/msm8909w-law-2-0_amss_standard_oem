/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include <stdint.h>
#include "touchSampleApi.h"
#include "TouchApi.h"

/** Start
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdStart(
  touchSampleCommandPayload_t* command,
  touchSampleResponsePayload_t* response
)
{
  ENTERRV;
  uint32_t result = SSE_OK;
  uint32_t sid = DR_SID_INVALID;
  
  do {
    // map the buffer in the driver
    result = touchOpen(&sid);
    if (!IS_OK(result)) {
      LOG_E("Error opening session with driver",result);
      rv = E_SSE_UNKNOWN;
      break;
    }
    LOG_I("Session: %d", sid);
    response->start.sid = sid;
  } while (0);
  EXITRV;
}
