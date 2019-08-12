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
#include "pwrmgr.h"
#include "DALSys.h"
#include "hal_clkrgm.h"
#include "requestmgr.h"
#include "adsppm_utils.h"
/*
 * Data type definitions
 */

/**
 * Definition of the Power Manager Context.
 */
typedef struct{
    DALSYSSyncHandle       ctxLock;//
    Hal_ClkRgmEnableDisableType pwrDomainState[AsicPowerDomain_EnumMax][2];
    uint32 activeStateIndex;
} PwrMgrContextType;

static PwrMgrContextType gPwrMgrCtx;//!< Global context for the power manager

Adsppm_Status PWR_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    AsicPowerDomainType i = AsicPowerDomain_AON;
    ADSPPM_LOG_FUNC_ENTER;
    for(i = AsicPowerDomain_AON; i < AsicPowerDomain_EnumMax; i++)
    {
        gPwrMgrCtx.pwrDomainState[i][0] = Hal_ClkRgmDisable;
        gPwrMgrCtx.pwrDomainState[i][1] = Hal_ClkRgmDisable;
    }
    gPwrMgrCtx.activeStateIndex = 0;

    if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &gPwrMgrCtx.ctxLock, NULL))
    {
        result = Adsppm_Status_Failed;
    }
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

Adsppm_Status PWR_ProcessRequest(coreUtils_Q_Type *pPwrReqQ)
{
    Adsppm_Status result = Adsppm_Status_Success;
    RMCtxType *pLRMCtxItem = NULL;
    AdsppmHandleType *pLHandleItem = NULL;
    Hal_ClkRgmEnableDisableType lpassCoreStateRequested = 0;
    ADSPPM_LOG_FUNC_ENTER;
    if(NULL == pPwrReqQ)
    {
        result = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Bad parameter\n");
    }
    else
    {
        if(NULL == gPwrMgrCtx.ctxLock)
        {
            result = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized\n");
        }
        else
        {
            int index = 0;
            AsicPowerDomainType pwrDomain = AsicPowerDomain_AON;
            AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
            adsppm_lock(gPwrMgrCtx.ctxLock); //lock Pwr Manager's data
            ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_Power); // Lock Request manager's data
            // Clear current state
            for(pwrDomain = AsicPowerDomain_AON; pwrDomain < AsicPowerDomain_EnumMax; pwrDomain++)
            {
                gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex] = Hal_ClkRgmDisable;
            }
            // Loop through all clients
            pLRMCtxItem = (RMCtxType *)coreUtils_Q_Check(pPwrReqQ);
            for(index = 0; index < pPwrReqQ->nCnt; index++)
            {
                if(NULL != pLRMCtxItem)
                {
                        //find coreid through clientid
                    pLHandleItem = coreUtils_Q_LinearSearch(&pAdsppmCtx->AdsppmHandleQ, (coreUtils_Q_CompareFuncType)findClientHandle, (void *)&(pLRMCtxItem->clientId));
                    //If request is valid that means that power has to be On
                    if(NULL != pLHandleItem)
                    {
                        //get the power domain based on core ID for this client
                        pwrDomain = ACMPwr_GetPowerDomain(pLHandleItem->coreId);
                        if(RM_Valid == pLRMCtxItem->validFlag)
                        {
                            gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex] = Hal_ClkRgmEnable;
                        }
                    }
                    else
                    {
                        result = Adsppm_Status_BadParm;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "can't find correct handle for clientid=%d\n", pLRMCtxItem->clientId);

                    }
                    pLRMCtxItem = (RMCtxType *)coreUtils_Q_Next(pPwrReqQ, &(pLRMCtxItem->RMCtxLink));
                }

            }
            ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_Power); // unlock Request manager's data

            for(pwrDomain = AsicPowerDomain_AON; pwrDomain < AsicPowerDomain_EnumMax; pwrDomain++)
            {
                //Apply overrides if feature is disabled.
                switch (pwrDomain)
                {
                case AsicPowerDomain_Adsp:
                    if((AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_Adsp_PC)))
                    {
                        gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex] = Hal_ClkRgmEnable;
                    }
                    break;
                case AsicPowerDomain_LpassCore:
                    //turns on/off LPASS core shared clocks
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,"LPASS core requested state %d, turn on/off shared clocks \n",gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex]);
                    result = clkrgm_SharedClkControl(gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex]);
                    // Saving the clients requested state for lpass core
                    lpassCoreStateRequested = gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex];
                    if((AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_LpassCore_PC)))
                    {
                        gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex] = Hal_ClkRgmEnable;
                    }
                    break;
                default:
                    break;
                }

                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_DEBUG, "Power domain %d state %d: ", pwrDomain, gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex]);

                if(gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex] !=
                        gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex ^ 1])
                {
                    // Only Call Clk regime wrapper to enable/disable power domains if the state changes
                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,"Power domain %d requested state %d\n",pwrDomain, gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex]);
                    
                    if( AsicPowerDomain_LpassCore == pwrDomain)
                    {
                        result = clkrgm_SharedClkControl(gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex]);
                    }
                    if (Adsppm_Status_Success == result)
                    {
                        result = ClkRgm_RequestPower(pwrDomain, gPwrMgrCtx.pwrDomainState[pwrDomain][gPwrMgrCtx.activeStateIndex]);
                    }
                    else
                    {
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,"turn on shared clock failed before request lpass power\n");
                        break;
                    }
                    if ((Adsppm_Status_Success == result) && (AsicPowerDomain_LpassCore == pwrDomain))
                    {
                        if((lpassCoreStateRequested == 0) && ((AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_LpassCore_PC))))
                        {
                            // Turn off shared clocks when lpass core PC feature is disabled and no clients requested
                            result = clkrgm_SharedClkControl(Hal_ClkRgmDisable); 
                        }
                    }
                }

                if (Adsppm_Status_Success != result)
                {
                    break;
                }
            }          
            //Flip active/previous request
            gPwrMgrCtx.activeStateIndex ^= 1;
            adsppm_unlock(gPwrMgrCtx.ctxLock);//Unlock request data as we'll work only on local PwrManager's data further
        }
    }    
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}
