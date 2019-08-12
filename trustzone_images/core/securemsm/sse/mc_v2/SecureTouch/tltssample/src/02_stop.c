/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "touchSampleApi.h"
#include "TlApiTs.h"

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
  tlApiResult_t result = TLAPI_OK;
  
  do {
    result = tlApiTsClose(command->stop.sid);
    if (!IS_OK(result)) {
      LOG_E("Error closing session %d with driver",command->stop.sid,result);
      rv = E_SSE_UNKNOWN;
      break;
    }
  } while (0);
  EXITRV;
}
