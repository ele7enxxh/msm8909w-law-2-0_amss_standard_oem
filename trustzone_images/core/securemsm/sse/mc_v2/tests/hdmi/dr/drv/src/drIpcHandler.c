/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
/**
 * @file   drIpcHandler.c
 * @brief  Implements IPC handler of the driver.
 *
 */

#include "drStd.h"
#include "DrApi/DrApiMm.h"
#include "DrApi/DrApiThread.h"
#include "DrApi/DrApiIpcMsg.h"

#include "drCommon.h"
#include "drApiMarshal.h"
#include "sselog.h"

/* Thread specific definitions */
#define threadid_is_null(threadid)  (NILTHREAD == threadid)  /* returns true if thread id is NILTHREAD */

extern void ProcessDriverFunction(const threadid_t ipcClient, message_t *ipcMsg, uint32_t *ipcData);

DECLARE_STACK(drIpchStack, 2048);

/* Global variables */
threadid_t gLastIpcClient = NILTHREAD;

/**
 * IPC handler loop. this is the function where IPC messages are handled
 */
 _NORETURN void drIpchLoop( void )
{
  /* Set IPC parameters for initial MSG_RD to IPCH */
  threadid_t     ipcClient = NILTHREAD;
  message_t      ipcMsg    = MSG_RD;
  uint32_t       ipcData   = 0;
  uint32_t r;


  /**
   * Check if there is a pending client. If there is, this is an
   * indication that IPC handler thread crashed before completing
   * the request. Respond with failure.
   */
  if (!threadid_is_null(gLastIpcClient))
  {
    ipcClient = gLastIpcClient;
    ipcMsg    = MSG_RS;
    ipcData   = E_TLAPI_DRV_UNKNOWN;
  }

  for (;;)
  {
    /*
     * When called first time sends ready message to IPC server and
     * then waits for IPC requests
     */
    if (E_OK != (r = drApiIpcCallToIPCH(&ipcClient, &ipcMsg, &ipcData)))
    {
      LOG_E("drApiIpcCallToIPCH failed: 0x%08X",r);
      continue;
    }

    /* Update last IPC client */
    gLastIpcClient = ipcClient;

    /* Dispatch request */
    switch (ipcMsg)
    {
      case MSG_CLOSE_TRUSTLET:
        ipcMsg = MSG_CLOSE_TRUSTLET_ACK;
        ipcData = 0;

        break;
      case MSG_CLOSE_DRIVER:
        /* Acknowledge */
        ipcMsg = MSG_CLOSE_DRIVER_ACK;
        ipcData = 0;
        break;
      case MSG_GET_DRIVER_VERSION:
        ipcMsg = (message_t) TLAPI_OK;
        ipcData = DRIVER_VERSION  ;
        break;
      case MSG_RQ:
        ProcessDriverFunction(ipcClient,&ipcMsg,&ipcData);
        break;
      default:
        /* Unknown message has been received*/
        ipcMsg = (message_t) E_TLAPI_DRV_UNKNOWN;
        ipcData = 0;
        break;
    }
  }
}

//------------------------------------------------------------------------------
_THREAD_ENTRY void drIpch( void )
{
  drIpchLoop();
}


//------------------------------------------------------------------------------
void drIpchInit(
  const addr_t    dciBuffer,
  const uint32_t  dciBufferLen
){
  uint32_t r;
  /* ensure thread stack is clean */
  clearStack(drIpchStack);

  /**
   * Start IPC handler thread. Exception handler thread becomes local
   * exception handler of IPC handler thread
   */
  if (E_OK != (r = drApiStartThread(
                  DRIVER_THREAD_NO_IPCH,
                  FUNC_PTR(drIpch),
                  getStackTop(drIpchStack),
                  IPCH_PRIORITY,
                  DRIVER_THREAD_NO_EXCH)))

  {
    LOG_E("drApiStartThread failed: 0x%08X",r);
  }
}
