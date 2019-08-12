/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include "tci.h"
#include "tlTestHdmiApiParams.h"

#define LOG_SIZE 8192

/**
 * @cond
 * Command ID's for communication Trustlet Connector -> Trustlet.
 */

#define TLTESTHDMI_CMD_STATUS         1

/**
 * Termination codes
 */
#define EXIT_ERROR                      ((uint32_t)(-1))

#define IS_OK(x) (x == TLAPI_OK)

/*
 * @endcond
 */

typedef union {
  cmdGetStatus_t             status;
} tlTestHdmiCommandPayload_t;

typedef union {
  rspGetStatus_t             status;
} tlTestHdmiResponsePayload_t;

/**
 * TCI message data.
 */
typedef struct {
  union {
    tciCommandHeader_t          command;
    tciResponseHeader_t         response;
  } header;
  union {
    tlTestHdmiCommandPayload_t  command;
    tlTestHdmiResponsePayload_t response;
  } payload;
} tciMessage_t;

/**
 * Overall TCI structure.
 */
typedef struct {
    tciMessage_t    message;                    /**< TCI message */
    uint8_t         log[LOG_SIZE];       /**< size = len + log_len*/
}tci_t;


/**
 * Trustlet UUID.
 */
#define TL_TESTHDMI_UUID { { 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} }
