/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#include "TlApi/TlApiCommon.h"
#include "TlApi/TlApiError.h"
#include "TlApiTsDefs.h"

_TLAPI_EXTERN_C tlApiResult_t tlApiTsOpen(uint32_t *sid);

_TLAPI_EXTERN_C tlApiResult_t tlApiTsClose(const uint32_t sid);

_TLAPI_EXTERN_C tlApiResult_t tlApiTsGetData(const uint32_t sid, struct tsFingerData *fingers);
