/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "touchSampleApi.h"
#include "TlApiTs.h"

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
  tlApiResult_t result = TLAPI_OK;
  uint32_t sid = DR_SID_INVALID;
  
  do {
    // map the buffer in the driver
    result = tlApiTsOpen(&sid);
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
