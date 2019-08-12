/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "sselog.h"
#include "drCommon.h"
#include "DrApi/DrApiMm.h"
#include "DrApi/DrApiThread.h"
#include "DrApi/DrApiIpcMsg.h"
#include "drSmgmt.h"
#include "drApiMarshal.h"
#include "TlApiTsError.h"
#include "drTsController.h"

extern threadid_t gLastIpcClient;

tlApiResult_t processFuncClose(drMarshalingParam_t *pParam) {
  drSessionReg_t *session = NULL;
  tlApiResult_t rv = TLAPI_OK;
  ENTER;
  do {
    LOG_I("Close Session: %d, client: %d",pParam->sid, gLastIpcClient);
    session = drSmgmtGetSessionData(pParam->sid);
    if (session == NULL) {
      LOG_E("Could not get session data for sid=0x%08X",pParam->sid);
      // this is never supposed to happen
      rv = E_TLAPI_UNKNOWN;
      break;
    }
    if (session->threadid != gLastIpcClient) {
      LOG_E("Session sid=0x%08X does not belong to client 0x%08X",pParam->sid, gLastIpcClient);
      rv = E_TLAPI_UNKNOWN;
      break;
    }
    // this will take care of closing the I2C connection
    drSmgmtCloseSession(pParam->sid);    
  } while(0);
  EXITRV;
}
