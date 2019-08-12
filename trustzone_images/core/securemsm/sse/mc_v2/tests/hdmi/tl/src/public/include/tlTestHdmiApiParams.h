/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

/******************************************************************************/
/* (1) GetStatus */
/******************************************************************************/
/** GetData command */
typedef struct {
  uint32_t reserved; /**< For future use */
} cmdGetStatus_t;
/** GetData response */
typedef struct {
  uint32_t hdmi_enable;
  uint32_t hdmi_sense;
  uint32_t hdcp_auth;
} rspGetStatus_t;

