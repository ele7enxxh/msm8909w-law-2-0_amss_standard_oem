/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include "tci.h"
#include "tlSecureTouchApiParams.h"

#define LOG_SIZE 8192

/**
 * @cond
 * Command ID's for communication Trustlet Connector -> Trustlet.
 */

#define TLSECURETOUCH_CMD_SETUP          1
#define TLSECURETOUCH_CMD_PROCESS        2
#define TLSECURETOUCH_CMD_TEARDOWN       3

/*
 * @endcond
 */

typedef union {
  cmdSetup_t              setup;
  cmdProcess_t            process;
  cmdTeardown_t           teardown;
} tlSecureTouchCommandPayload_t;

typedef union {
  rspSetup_t              setup;
  rspProcess_t            process;
  rspTeardown_t           teardown;
} tlSecureTouchResponsePayload_t;

/**
 * TCI message data.
 */
typedef struct {
  union {
    tciCommandHeader_t          command;
    tciResponseHeader_t         response;
  } header;
  union {
    tlSecureTouchCommandPayload_t  command;
    tlSecureTouchResponsePayload_t response;
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
#define TL_ST_UUID { { 4, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2} }
