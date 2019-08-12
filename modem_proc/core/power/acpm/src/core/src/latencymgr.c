/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * latencymgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "core.h"
#include "core_internal.h"
#include "requestmgr.h"
#include "latencymgr.h"
#include "hal_sleep.h"


/*
 * Data type definitions
 */

/**
 * Definition of the Power Manager Context.
 */
typedef struct{
    DALSYSSyncHandle   ctxLock; //!< Lock for the sleep context
    uint32 latencyUs[2]; //!< current/previous latency
    uint32 activeStateIndex; //!< active state index
} SleepMgrContextType;

static SleepMgrContextType gSleepMgrCtx;//!< Global context for the power manager



Adsppm_Status SLEEP_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;

    ADSPPM_LOG_FUNC_ENTER;
    gSleepMgrCtx.latencyUs[0] = HAL_SLEEP_LATENCY_DONT_CARE;
    gSleepMgrCtx.latencyUs[1] = HAL_SLEEP_LATENCY_DONT_CARE;
    gSleepMgrCtx.activeStateIndex = 0;


    if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &gSleepMgrCtx.ctxLock, NULL))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to create lock");
        result = Adsppm_Status_Failed;
    }
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

Adsppm_Status SLEEP_ProcessRequest(coreUtils_Q_Type *pSlpReqQ)
{
   Adsppm_Status result = Adsppm_Status_Success;
   RMCtxType *pLRMCtxItem = NULL;
   ADSPPM_LOG_FUNC_ENTER;

   if(NULL == pSlpReqQ)
   {
      result = Adsppm_Status_BadParm;
      ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Bad parameter\n");
   }
   else
   {
      if(NULL == gSleepMgrCtx.ctxLock)
      {
         result = Adsppm_Status_NotInitialized;
         ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized\n");
      }
      else
      {
         int index = 0;
         adsppm_lock(gSleepMgrCtx.ctxLock); //lock Sleep Manager's data
         ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_Sleep_Latency); // Lock Request manager's data
         // Clear current state
         gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex] = HAL_SLEEP_LATENCY_DONT_CARE;

         // Loop through all clients
          pLRMCtxItem = (RMCtxType *)coreUtils_Q_Check(pSlpReqQ);
         for(index = 0; index < pSlpReqQ->nCnt; index++)
         {
            if(NULL != pLRMCtxItem)
            {
               if((RM_Valid == pLRMCtxItem->validFlag) && (NULL != pLRMCtxItem->pRequestData))
               {
                  //If request is valid that means that power has to be On
                  gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex] = MIN(gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex],
                                                               *((uint32*)pLRMCtxItem->pRequestData));
               }
            }
            pLRMCtxItem = (RMCtxType *)coreUtils_Q_Next(pSlpReqQ, &(pLRMCtxItem->RMCtxLink));
         }

         ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_Sleep_Latency); // unlock Request manager's data

         ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Aggregated latency value: %d\n",gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex]);
         
         if(gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex] !=
               gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex^1])
         {
            // Only Call Sleep wrapper if the state changes
            result = Slp_RequestLatency(gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex]);
         }

         //Flip active/previous request
         gSleepMgrCtx.activeStateIndex ^= 1;
         adsppm_unlock(gSleepMgrCtx.ctxLock);//Unlock request data as we'll work only on local SleepManager's data further

      }
   }
   ADSPPM_LOG_FUNC_EXIT(result);
   return result;
}
