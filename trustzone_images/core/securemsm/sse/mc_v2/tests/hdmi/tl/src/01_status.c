/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "sse_error.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlTestHdmiApi.h"
#include "TlApiTestHdmi.h"

/** GetData
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdStatus(
  tlTestHdmiCommandPayload_t* command,
  tlTestHdmiResponsePayload_t* response
)
{
  ENTERRV;
  tlApiResult_t result = TLAPI_OK;
  
  do {
    result = tlApiHdmiGetStatus(
      &response->status.hdmi_enable,
      &response->status.hdmi_sense,
      &response->status.hdcp_auth);
    if (!IS_OK(result)) {
      LOG_E("Error getting data from driver",result);
      rv = E_SSE_UNKNOWN;
      break;
    }
  } while (0);
  EXITRV;
}
