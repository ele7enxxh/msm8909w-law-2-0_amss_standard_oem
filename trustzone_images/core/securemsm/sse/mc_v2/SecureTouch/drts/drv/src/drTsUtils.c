/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include <drStd.h>

#include <sselog.h>
#include "TlApiTsDefs.h"
#include "drTsController.h"
#include "drGlob.h"
#include "drSmgmt.h"

/** Notify the upper layer of a touch event.
 * 
 */
uint32_t drNotifyEvent(const uint32_t x, const uint32_t y, 
                       const uint32_t code, const uint8_t finger)
{
  uint32_t rv = 0;
  ENTER;
  do {
    if (finger >= MAX_FINGER_NUM) {
      LOG_E("Only %u fingers supported. Received event for finger %u", 
        MAX_FINGER_NUM, finger);
      rv = 0xFFFFFFFD;
      break;
    }
    if (g_fingers_ptr == NULL) {
      LOG_W("TL is not requesting data, data is silently discarded");
      break;
    }
    g_fingers_ptr->finger[finger].event |= code;
    g_fingers_ptr->finger[finger].x = x;
    g_fingers_ptr->finger[finger].y = y;
    g_fingers_ptr->finger[finger].dirty = 1;
    LOG_I("[%u]: %u, (%u, %u)",finger, g_fingers_ptr->finger[finger].event, 
      g_fingers_ptr->finger[finger].x, g_fingers_ptr->finger[finger].y);
  } while (0);
  EXITV(rv);
}

void drTsSessionClose(void * args)
{
  drSessionReg_t *reg = NULL;
  ENTER;
  do {
    if (!args) {
      LOG_E("Expecting a session!");
      break;
    }
    // we're closing a session
    reg = (drSessionReg_t *)args;
    if (reg->tsInUse == 0) {
      // TS not being used by this session, nothing to do
      break;
    }
    // first we empty the queue of currently registered events
    if (0 != drTsProcessEvents()) {
      LOG_E("Failed to read event from Touch controller");
      // we have to ignore the error and proceed
    }
    // then we close our connection
    if (0 != drTsClose()) {
      LOG_E("Failed to close Touch interface");
      // nothing we can do about it
    }

  } while (0);
  EXIT;
}
