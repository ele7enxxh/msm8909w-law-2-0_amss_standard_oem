/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * pwrmgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "asic.h"
#include "mempwrmgr.h"
#include "DALSys.h"
#include "hal_clkrgm.h"
#include "requestmgr.h"
#include "adsppm_utils.h"

/*
 * Data type definitions
 */


static memPwrMgrContextType gMemPwrMgrCtx;//!< Global context for the ocmem power manager

Adsppm_Status MEMPWR_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    uint32 j;
    ADSPPM_LOG_FUNC_ENTER;
    
    gMemPwrMgrCtx.activeStateIndex = 0;

    if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &gMemPwrMgrCtx.ctxLock, NULL))
    {
        result = Adsppm_Status_Failed;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to create mutex\n");
    }

    for(j= 0; j < Adsppm_Mem_Max -Adsppm_Mem_Lpass_LPM; j++)
    {
        gMemPwrMgrCtx.memPwrState[j][gMemPwrMgrCtx.activeStateIndex] = Adsppm_Mem_Power_Off;
        gMemPwrMgrCtx.memPwrState[j][gMemPwrMgrCtx.activeStateIndex^1] = Adsppm_Mem_Power_Off;
    }

    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

Adsppm_Status MEMPWR_ProcessRequest(coreUtils_Q_Type *pMemPwrReqQ)
{
    Adsppm_Status result = Adsppm_Status_Success;
    RMCtxType *pLRMCtxItem = NULL;

    ADSPPM_LOG_FUNC_ENTER;
    if(NULL == pMemPwrReqQ)
    {

        result = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Bad parameter\n");
    }
    else
    {
        if(NULL != gMemPwrMgrCtx.ctxLock)
        {
            int index = 0;

            adsppm_lock(gMemPwrMgrCtx.ctxLock); //lock Pwr Manager's data
            ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_MemPower); // Lock Request manager's data

            //clean up current state and update sts

            for(index = 0; index < Adsppm_Mem_Max -Adsppm_Mem_Lpass_LPM; index++)
            {
                gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex] = Adsppm_Mem_Power_Off;
            }
            // Loop through all clients link list
            pLRMCtxItem = (RMCtxType *)coreUtils_Q_Check(pMemPwrReqQ);
            for(index = 0; index < pMemPwrReqQ->nCnt; index++)
            {              
                if(NULL != pLRMCtxItem)
                {
                    if(RM_Valid == pLRMCtxItem->validFlag)
                    {
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_DEBUG, "Client %d: request valid\n", pLRMCtxItem->clientId);
                        if(NULL == ((AdsppmMemPowerReqParamType *)(pLRMCtxItem->pRequestData)))
                        {
                            result = Adsppm_Status_BadParm;
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pRequestData=NULL\n");
                            break;
                        }
                        else
                        {
                            AdsppmMemPowerReqParamType * pReqData = (AdsppmMemPowerReqParamType *)pLRMCtxItem->pRequestData;
                            if((Adsppm_Mem_None < pReqData->memory) && (Adsppm_Mem_Max > pReqData->memory))
                            {
                                switch(ACMPwr_GetMemPowerType(pReqData->memory))
                                {
                                case AsicRsc_Power_Mem:
                                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT, "Mem %d, state %d\n", pReqData->memory, pReqData->powerState);
                                    if(pReqData->memory >= Adsppm_Mem_Lpass_LPM)
                                    {
                                        gMemPwrMgrCtx.memPwrState[pReqData->memory-Adsppm_Mem_Lpass_LPM][gMemPwrMgrCtx.activeStateIndex] =
                                            MAX( gMemPwrMgrCtx.memPwrState[pReqData->memory-Adsppm_Mem_Lpass_LPM][gMemPwrMgrCtx.activeStateIndex],
                                                    pReqData->powerState);
                                    }
                                    else
                                    {
                                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Mem %d. Shouldn't be here\n", pReqData->memory);
                                    }
                                    break;
                            default:

                                break;
                            }
                        }
                        else
                        {
                            result = Adsppm_Status_BadParm;
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Invalid Mem type %d\n", pReqData->memory);
                        }
                    }
                    }
                    pLRMCtxItem = (RMCtxType *)coreUtils_Q_Next(pMemPwrReqQ, &(pLRMCtxItem->RMCtxLink));
                }
            }
            ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_MemPower); // unlock Request manager's data
            if(Adsppm_Status_Success == result)
            {
                //process internal memories
                for(index = 0; index < Adsppm_Mem_Max -Adsppm_Mem_Lpass_LPM; index++)
                {
                    if (gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex] != gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex^1])
                    {
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "Requesting power state % for memory %d\n",
                                gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex], index + Adsppm_Mem_Lpass_LPM);
						if( Adsppm_Mem_Power_Off== gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex])
						{
							result = ClkRgm_MemoryControl(index + Adsppm_Mem_Lpass_LPM, Hal_ClkRgmMemCollapse);
						}
						else
						{
							result = ClkRgm_MemoryControl(index + Adsppm_Mem_Lpass_LPM, Hal_ClkRgmMemRetain);
						}
                        if(Adsppm_Status_Success != result)
                        {
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Error requesting power state for memory %d\n", index + Adsppm_Mem_Lpass_LPM);
                            break;
                        }
                    }
                }

                //Flip active/previous request
                gMemPwrMgrCtx.activeStateIndex ^= 1;
            }
            adsppm_unlock(gMemPwrMgrCtx.ctxLock);//Unlock request data as we'll work only on local PwrManager's data further
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized\n");
        }
    }
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}
