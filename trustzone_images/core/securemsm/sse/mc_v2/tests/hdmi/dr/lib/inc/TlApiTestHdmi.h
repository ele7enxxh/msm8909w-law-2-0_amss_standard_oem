/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#include "TlApi/TlApiCommon.h"
#include "TlApi/TlApiError.h"

_TLAPI_EXTERN_C tlApiResult_t tlApiHdmiGetStatus(
  uint32_t* hdmi_enable, 
  uint32_t* hdmi_sense,
  uint32_t* hdcp_auth);


