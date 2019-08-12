/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include <stdint.h>
#include "touchSampleApi.h"
#include "TouchApi.h"

/** Stop
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdStop(
  touchSampleCommandPayload_t* command,
  touchSampleResponsePayload_t* response
)
{
  ENTERRV;
  uint32_t result = SSE_OK;
  
  do {
    result = touchClose(command->stop.sid);
    if (!IS_OK(result)) {
      LOG_E("Error closing session %d with driver",command->stop.sid,result);
      rv = E_SSE_UNKNOWN;
      break;
    }
  } while (0);
  EXITRV;
}
