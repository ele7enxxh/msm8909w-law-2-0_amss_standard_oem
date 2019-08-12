/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
/**
 * @file   drExcHandler.c
 * @brief  Implements exception handler of the driver.
 *
 * Responsible for handling exceptions cause by IPC and DCI handler threads.
 * Restarts the thread that causes specific exception. runs with higher priority
 * than IPC and DCI handler threads
 */

#include "sselog.h"
#include "drCommon.h"
#include "DrApi/DrApiMm.h"
#include "DrApi/DrApiThread.h"
#include "DrApi/DrApiIpcMsg.h"

#include "drSmgmt.h"
#include "drUtils.h"
#include "drTsController.h"

/* IPC handler stack */
EXTERNAL_STACK(drIpchStack)

/* IPC handler entry function */
extern _NORETURN void drIpchLoop( void );


/**
 * Cleanup function
 */
static void doCleanup( void )
{
  /* Close all sessions */
  drSmgmtCloseAllSessions();
}


/**
 * Exception handler loop.
 */
_NORETURN void drExchLoop(
    const addr_t    dciBuffer,
    const uint32_t  dciBufferLen
){
  threadno_t      faultedThread;
  threadid_t      ipcPartner;
  uint32_t        mr0, mr1, mr2;
  addr_t          ip = NULL;
  addr_t          sp = NULL;

  ENTER;

  for (;;) {
    // wait for exception
    if ( E_OK != drApiIpcWaitForMessage(
             &ipcPartner,
             &mr0,
             &mr1,
             &mr2) ) {
      /* Unable to receive IPC message */
      LOG_E("drApiIpcWaitForMessage failed");
      continue;
    }

    /* mr0 holds threadid value of thread that caused the exception */
    faultedThread = GET_THREADNO(mr0);

    /* Process exception */
    switch(mr1) {
      //--------------------------------------
      case TRAP_SEGMENTATION:
        /* Update ip and sp accordingly and restart the thread */
        switch(faultedThread) {
          //--------------------------------------
          case DRIVER_THREAD_NO_IPCH:
            ip = FUNC_PTR(drIpchLoop);
            sp = getStackTop(drIpchStack);
            break;
          //--------------------------------------
          default:
            LOG_E("Unknown thread. This should never happen");
            break;
        }

        if ((ip != NULL) && (sp != NULL)) {
          /* Set sp and ip accordingly and resume execution if DCIH thread */
          if (E_OK != drUtilsRestartThread(
                          faultedThread,
                          ip,
                          sp)) {
            LOG_E("restarting thread failed");
          }
        }

        break;
      //--------------------------------------
      case TRAP_ALIGNMENT:
      case TRAP_UNDEF_INSTR:
        /**
         * This should never happen. If it does, do the cleanup and exit gracefully
         */
        doCleanup();

        /* Stop IPC handler thread */
        if (E_OK != drApiStopThread(DRIVER_THREAD_NO_IPCH)) {
          LOG_E("Unable to stop IPC handler thread");
        }

        /* Stop main thread */
        if (E_OK != drApiStopThread(NILTHREAD)) {
          LOG_E("Unable to stop main thread");
        }

        /* Will not come to this point */
        break;
      //--------------------------------------
      default:
        /**
         * TODO: Update this sestion accordingly
         *
         * Unknown exception occured.Do cleanup in case
         */
        doCleanup();
        break;
    }
  }
}

