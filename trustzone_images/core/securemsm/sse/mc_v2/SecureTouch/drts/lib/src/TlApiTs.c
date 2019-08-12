/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "tlStd.h"
#include "TlApi/TlApiCommon.h"
#include "drApiMarshal.h"
#include "TlApi/TlDriverApi.h"

_TLAPI_EXTERN_C tlApiResult_t tlApiTsOpen(uint32_t *sid)
{
  tlApiResult_t rv = TLAPI_OK;
  if (!sid)
    return E_TLAPI_NULL_POINTER;
  drMarshalingParam_t marParam = {
    .functionId = FID_DRV_TS_OPEN,
  };
  rv = tlApi_callDriver(DRV_TS_ID, &marParam);
  if (rv == TLAPI_OK)
  {
      *sid = marParam.sid;
  }

  return rv;
}

_TLAPI_EXTERN_C tlApiResult_t tlApiTsClose(
  uint32_t sid)
{
  tlApiResult_t rv = TLAPI_OK;
  drMarshalingParam_t marParam = {
    .functionId = FID_DRV_TS_CLOSE,
    .sid = sid
  };
  rv = tlApi_callDriver(DRV_TS_ID, &marParam);
  if (rv == TLAPI_OK) {
    rv = marParam.payload.retVal;
  }
  return rv;
}

_TLAPI_EXTERN_C tlApiResult_t tlApiTsGetData(
  const uint32_t sid,
  struct tsFingerData *fingers)
{
  tlApiResult_t rv = TLAPI_OK;
  if (!fingers)
    return E_TLAPI_NULL_POINTER;
  drMarshalingParam_t marParam = {
    .functionId = FID_DRV_TS_GETDATA,
    .payload.params.getData.fingers = fingers,
    .sid = sid
  };
  
  rv = tlApi_callDriver(DRV_TS_ID, &marParam);
  if (rv == TLAPI_OK) {
    rv = marParam.payload.retVal;
  }
  return rv;
}
