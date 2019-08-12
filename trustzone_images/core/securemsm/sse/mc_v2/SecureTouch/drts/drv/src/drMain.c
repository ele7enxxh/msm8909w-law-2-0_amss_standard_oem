/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#define MAIN_C

#include "sselog.h"
#include "drCommon.h"
#include "DrApi/DrApiMm.h"
#include "DrApi/DrApiThread.h"
#include "DrApi/DrApiIpcMsg.h"
#include "drApiMarshal.h"
#include "TlApiTsError.h"
#include "drUtils.h"
#include "drSmgmt.h"
#include "drGlob.h"


DECLARE_DRIVER_MAIN_STACK(2048);

#define DECLARE_DR_FUNC(name) \
  extern tlApiResult_t name(drMarshalingParam_t *pParam)
  
#define DRIVER_VERSION       1

DECLARE_DR_FUNC(processFuncOpen);
DECLARE_DR_FUNC(processFuncClose);
DECLARE_DR_FUNC(processFuncGetData);

struct dr_function {
  uint32_t funcId;
  tlApiResult_t (*func)(drMarshalingParam_t *pParam);
};

static const struct dr_function functions[]={
  {FID_DRV_TS_OPEN,processFuncOpen},
  {FID_DRV_TS_CLOSE,processFuncClose},
  {FID_DRV_TS_GETDATA,processFuncGetData},
};

/* Needed to keep track of the DCI for logging */
static addr_t   dci;
static uint32_t dciLen;

extern void drIpchInit( const addr_t dciBuffer, const uint32_t dciBufferLen );
extern _NORETURN void drExchLoop( const addr_t dciBuffer, const uint32_t  dciBufferLen );
extern _NORETURN void drIsrhLoop( void );

/* Main routine for the example driver.
   Initializes the Api data structures and starts the required threads.
*/
_DRAPI_ENTRY void drMain(
    const addr_t    dciBuffer,
    const uint32_t  dciBufferLen)
{
  dci = dciBuffer;
  dciLen = dciBufferLen;
  g_fingers_ptr = NULL;
  
  DR_LOG_INIT(dci,dciLen); // init the first time
  
  driver_version = DRIVER_VERSION;
  
  drSmgmtInit();
  
	/* Start IPC handler */
  drIpchInit(dciBuffer, dciBufferLen);

  /* Start exception handler */
  drExchLoop(dciBuffer, dciBufferLen);
}

void ProcessDriverFunction(const threadid_t ipcClient, message_t *ipcMsg, uint32_t *ipcData)
{
  tlApiResult_t rv = TLAPI_OK;
  drMarshalingParam_t *pParam; // pointer to IPC marshaling structure
  uint32_t functionId; // ID of the called function
  unsigned int i = 0;
  ENTER;
  do {
    DR_LOG_INIT(dci,dciLen);
    
    // Map requesting client so we can access the data in the request
    pParam = (drMarshalingParam_t*)drApiMapClientAndParams(
                ipcClient,
                *ipcData);
  
    // Check parameters
    if (NULL == pParam) {
      rv = E_TLAPI_DRV_UNKNOWN;
      break;
    }
    functionId=pParam->functionId;
    
    // Process command message
    rv = E_TLAPI_DRV_UNKNOWN;
    for (i=0; i<LENGTH(functions); i++) {
      if ((functions[i].funcId == functionId) && (functions[i].func != NULL)) {
        rv = (*(functions[i].func))(pParam);
        break;
      }
    }   
    // prepare response message
    *ipcMsg = MSG_RS;
    pParam->payload.retVal = rv;
    *ipcData = 0; // message was processed
  } while (0);
  EXIT;
}
