/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "sselog.h"
#include "drCommon.h"
#include "DrApi/DrApiMm.h"
#include "DrApi/DrApiThread.h"
#include "DrApi/DrApiIpcMsg.h"
#include "drApiMarshal.h"
#include "TlApiTsError.h"
#include "drSmgmt.h"
#include "drTsController.h"
#include "drGlob.h"

tlApiResult_t processFuncGetData(drMarshalingParam_t *pParam) {
  drSessionReg_ptr drSession;
  uint8_t i = 0;
  tlApiResult_t rv = TLAPI_OK;
  ENTER;
  do {
    drSession = drSmgmtGetSessionData(pParam->sid);
    if (drSession == NULL) {
      LOG_E("No session associated with this sid: %u", pParam->sid);
      rv = E_TLAPI_UNKNOWN;
      break;
    }

    if (SESSION_STATE_ACTIVE == drSession->state) {
      if (drSession->tsInUse == 1) {
        // map in the passed fingers structure
        g_fingers_ptr = drApiAddrTranslateAndCheck(pParam->payload.params.getData.fingers);
        if (g_fingers_ptr == NULL) {
          LOG_E("Failed to map TL buffer");
          rv = E_TLAPI_UNKNOWN;
          break;
        }
        if (0 != drTsProcessEvents()) {
          LOG_E("Failed to read event from Touch controller");
          rv = E_TLAPI_UNKNOWN;
          break;
        }
        for (i = 0; i < MAX_FINGER_NUM; i++) {
          if (g_fingers_ptr->finger[i].event) {
            LOG_I("[%u]: %u, (%u, %u)",i, 
              g_fingers_ptr->finger[i].event, 
              g_fingers_ptr->finger[i].x,
              g_fingers_ptr->finger[i].y);
          }
        }
        g_fingers_ptr = NULL;
      } else {
        LOG_E("Touch Controller not opened in this session");
        rv = E_TLAPI_UNKNOWN;
      }
    } else {
      LOG_E("Session not open");
      rv = E_TLAPI_UNKNOWN;
    }
  } while(0);
  EXITRV;
}
