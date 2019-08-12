/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "sselog.h"
#include "drStd.h"
#include "DrApi/DrApiMm.h"
#include "DrApi/DrApiThread.h"
#include "DrApi/DrApiIpcMsg.h"
#include "drApiMarshal.h"
#include "TlApi/TlApiError.h"
#include "MtkApiTzbspApiWrapper.h"

tlApiResult_t processFuncGetStatus(drMarshalingParam_t *pParam) {
  int tzbspRet = 0;
  tlApiResult_t rv = TLAPI_OK;
  ENTER;
  do {
    tzbspRet = tzbspApi_hdmiStatusRead(
      &pParam->payload.params.getStatus.hdmi_enable,
      &pParam->payload.params.getStatus.hdmi_sense,
      &pParam->payload.params.getStatus.hdcp_auth
      );
    if (tzbspRet == 0) {
      LOG_I("hdmi_enable=%u, hdmi_sense=%u, hdcp_auth=%u",
        pParam->payload.params.getStatus.hdmi_enable,
        pParam->payload.params.getStatus.hdmi_sense,
        pParam->payload.params.getStatus.hdcp_auth);
      rv = TLAPI_OK;
    } else {
      LOG_E("Error getting HDMI status: 0x%08X", tzbspRet);
      rv = E_TLAPI_UNKNOWN;
    }
  } while(0);
  EXITRV;
}
