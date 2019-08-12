/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "sselog.h"
#include "drCommon.h"
#include "DrApi/DrApiIpcMsg.h"
#include "drApiMarshal.h"
#include "drSmgmt.h"
#include "TlApiTsError.h"
#include "drTsController.h"

extern threadid_t gLastIpcClient;
extern void drTsSessionClose(void * args);

tlApiResult_t processFuncOpen(drMarshalingParam_t *pParam) {
  uint32_t sid;
  drSessionReg_t *session = NULL;
  tlApiResult_t rv = TLAPI_OK;
  ENTER;
  do {
    sid= drSmgmtOpenSession(gLastIpcClient, drTsSessionClose);
    pParam->sid = sid;
    LOG_I("Open Session: %d. Client: %d",sid, gLastIpcClient);
    if (0 != drTsOpen(0,0)) {
      LOG_E("Failed to open Touch interface");
      rv = E_TLAPI_UNKNOWN;
      break;
    }
    session = drSmgmtGetSessionData(sid);
    if (session == NULL) {
      LOG_E("Could not get session data for sid=0x%08X",sid);
      // this is never supposed to happen, close touch and return
      drTsClose();
      rv = E_TLAPI_UNKNOWN;
      break;
    }
    session->tsInUse=1;
  } while(0);
  EXITRV;
}
