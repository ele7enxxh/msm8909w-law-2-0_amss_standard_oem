/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#define E_DRAPI_DRV_TS_ 0x40001000

typedef enum {
  E_TLAPI_TS_BUSY                   = (E_DRAPI_DRV_TS_) | 0x00000001, /**< Driver already in use */
  E_TLAPI_TS_GENERAL_ERROR          = (E_DRAPI_DRV_TS_) | 0x0000000A, /**< Area not protected */
} tlApiTsResult_t;

#define TLAPI_T(X)  ((tlApiResult_t)(X))

