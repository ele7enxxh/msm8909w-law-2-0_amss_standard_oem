/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

#include "tlSecureTouchCommands.h"
#include "SecureTouchLayout.h"
#include "InputStatus.h"

/******************************************************************************/
/* (1) Setup */
/******************************************************************************/
/** Setup command */
typedef struct {
  tlSecureTouchRequest_t request;
  LayoutPage_t page;
} cmdSetup_t;
/** Setup response */
typedef struct {
  LayoutPage_t page; /**< modified with indexed input fields */
} rspSetup_t;

/******************************************************************************/
/* (2) Process */
/******************************************************************************/
/** Process command */
typedef struct {
  uint32_t reserved;
} cmdProcess_t;
/** Process response */
typedef struct {
  SecureTouch_Event_t event;
  uint32_t focusIndex;
  uint32_t num_chars[MAX_INPUT_FIELDS];
} rspProcess_t;

/******************************************************************************/
/* (3) Teardown */
/******************************************************************************/
/** Teardown command */
typedef struct {
  tlSecureTouch_Teardown_Cmd_t cmd;
} cmdTeardown_t;
/** Teardown response */
typedef struct {
  tlSecureTouchResponse_t response;
} rspTeardown_t;
