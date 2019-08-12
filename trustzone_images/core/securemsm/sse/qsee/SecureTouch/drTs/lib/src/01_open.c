/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "sselog.h"
#include "SecureTouchError.h"
#include "drTsController.h"
#include "TouchApi.h"

// this is not really useful here
uint32_t g_drTs_sid = DR_SID_INVALID;

uint32_t touchOpen(uint32_t *sid) {
  uint32_t rv = SSE_OK;
  ENTER;
  do {
    if (g_drTs_sid != DR_SID_INVALID) {
      LOG_E("Secure touch already in use");
      rv = E_ST_BUSY;
      break;
    }
    g_drTs_sid = 0; // poor man random...
    *sid = g_drTs_sid;
    if (0 != drTsOpen(0,0)) {
      LOG_E("Failed to open Touch interface");
      rv = E_ST_DRIVER_ERROR; // the TZBSP I2C interface doesn't return much info on failures...
      break;
    }
  } while(0);
  EXITRV;
}
