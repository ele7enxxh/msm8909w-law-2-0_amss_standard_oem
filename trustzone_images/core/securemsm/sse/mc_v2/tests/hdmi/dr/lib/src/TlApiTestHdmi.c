/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "tlStd.h"
#include "TlApi/TlApiCommon.h"
#include "drApiMarshal.h"
#include "TlApi/TlDriverApi.h"

_TLAPI_EXTERN_C tlApiResult_t tlApiHdmiGetStatus(
  uint32_t* hdmi_enable, 
  uint32_t* hdmi_sense,
  uint32_t* hdcp_auth)
{
  tlApiResult_t rv = TLAPI_OK;
  if (!hdmi_enable && !hdmi_sense && !hdcp_auth)
    return E_TLAPI_NULL_POINTER;
  drMarshalingParam_t marParam = {
    .functionId = FID_DRV_HDMI_GETSTATUS,
  };
  
  rv = tlApi_callDriver(DRV_HDMI_ID, &marParam);
  if (rv == TLAPI_OK) {
    if (hdmi_enable)
      *hdmi_enable = marParam.payload.params.getStatus.hdmi_enable;
    if (hdmi_sense)
      *hdmi_sense = marParam.payload.params.getStatus.hdmi_sense;
    if (hdcp_auth)
      *hdcp_auth = marParam.payload.params.getStatus.hdcp_auth;
    rv = marParam.payload.retVal;
  }
  return rv;
}
