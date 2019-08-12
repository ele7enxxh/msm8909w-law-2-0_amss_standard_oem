/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include "tci.h"
#include "touchSampleApiParams.h"

#define LOG_SIZE 8192

/**
 * @cond
 * Command ID's for communication HLOS Client App -> TZ App.
 */

#define TOUCHSAMPLE_CMD_START          1
#define TOUCHSAMPLE_CMD_STOP           2
#define TOUCHSAMPLE_CMD_GETDATA        3

#ifdef TL_BUILD
#define IS_OK(x) (x == TLAPI_OK)
#else
#define IS_OK(x) (x == SSE_OK)
#endif

/*
 * @endcond
 */

typedef union {
  cmdStart_t              start;
  cmdStop_t               stop;
  cmdGetData_t            getData;
} touchSampleCommandPayload_t;

typedef union {
  rspStart_t              start;
  rspStop_t               stop;
  rspGetData_t            getData;
} touchSampleResponsePayload_t;

/**
 * TCI message data.
 */
typedef struct {
  union {
    tciCommandHeader_t          command;
    tciResponseHeader_t         response;
  } header;
  union {
    touchSampleCommandPayload_t  command;
    touchSampleResponsePayload_t response;
  } payload;
} tciMessage_t;

/**
 * Overall TCI structure.
 */
typedef struct {
    tciMessage_t    message;                    /**< TCI message */
    uint8_t         log[LOG_SIZE];       /**< size = len + log_len*/
} tci_t;

/**
 * Trustlet UUID.
 */
#define TL_TSSAMPLE_UUID { { 4, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} }
