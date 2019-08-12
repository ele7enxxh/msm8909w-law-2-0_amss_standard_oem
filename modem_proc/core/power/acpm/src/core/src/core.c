/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * core.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "core.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "core_internal.h"
#include "clkmgr.h"
#include "mipsmgr.h"
#include "pwrmgr.h"
#include "latencymgr.h"
#include "thermalmgr.h"
#include "requestmgr.h"
#include "busmgr.h"
#include "ahbmgr.h"
#include "mempwrmgr.h"


static AdsppmCtxType gAdsppmCoreCtx;

extern uint32 gADSPPMINITSTATE;
/**
 * @fn GetGlobalContext - retrieves pointer to the global ADSPPM context
 * @return pointer to the global ADSPPM context
 */
inline AdsppmCtxType* GetGlobalContext(void)
{
    return &gAdsppmCoreCtx;
}

/**
 * @fn findClientHandle - find the client context or handle in 
 *     linklist
 */
int findClientHandle(AdsppmHandleType* pLHandleItem,  uint32 *clientId)
{
    int match = 0;
    if(pLHandleItem->clientId == *clientId)
    {
         match = 1;
    }
    return match;  
}

inline uint32 IsPeriodicUseCase(void)
{
    return gAdsppmCoreCtx.periodicUseCase;
}

inline void SetPeriodicUseCase(uint32 periodic)
{
    gAdsppmCoreCtx.periodicUseCase = periodic;
}

/**
 * Prepare LPASS resources for power collapse.
 * Currently it will just ramp down AHB clock.
 * TODO: Handle the clock gates as well
 * @return completion status
 */
Adsppm_Status Core_EnterSleep(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    result = AHBM_SetSleepAHBClock();
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

/**
 * Restore LPASS resources after power collapse.
 * Currently it will just ramp up AHB clock.
 * TODO: Handle the clock gates as well
 * @return completion status
 */
Adsppm_Status Core_ExitSleep(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    result = AHBM_SetActiveAHBClock();
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

/**
 * Initialize all components within ADSPPM Core
 * @return completion status @see Adsppm_Status
 */
Adsppm_Status Core_Init(void)
{

    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;

    memset(&gAdsppmCoreCtx, 0, sizeof(AdsppmCtxType));

    //set periodic usecase by default to ensure quick powerdown
    gAdsppmCoreCtx.periodicUseCase = 1;

    if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&gAdsppmCoreCtx.adsppmCtxLock, NULL )) 
    {
        result = Adsppm_Status_Failed;
    }
    if(Adsppm_Status_Success == result)
    {
        gADSPPMINITSTATE |= Adsppm_State_CoreCtxLockInit;
        result = RM_Init();
        if(Adsppm_Status_Success == result)
        {
            gADSPPMINITSTATE |= Adsppm_State_CoreRMInit;
            result = MIPS_Init();
            if(Adsppm_Status_Success == result)
            {
                gADSPPMINITSTATE |= Adsppm_State_CoreMIPSInit;
                result = BUS_Init();
                if(Adsppm_Status_Success == result)
                {
                    gADSPPMINITSTATE |= Adsppm_State_CoreBUSInit;
                    result = AHBM_Init();
                    if(Adsppm_Status_Success == result)
                    {
                        gADSPPMINITSTATE |= Adsppm_State_CoreAHBMInit;
                        result = PWR_Init();
                        if(Adsppm_Status_Success == result)
                        {
                            gADSPPMINITSTATE |= Adsppm_State_CorePWRInit;
                            result = CLK_Init();
                            if(Adsppm_Status_Success == result)
                            {
                                gADSPPMINITSTATE |= Adsppm_State_CoreCLKInit;
                                result = SLEEP_Init();
                                if(Adsppm_Status_Success == result)
                                {
                                    gADSPPMINITSTATE |= Adsppm_State_CoreSLEEPInit;
                                    result = THERMAL_Init();
									if(Adsppm_Status_Success == result)
									{
                                        gADSPPMINITSTATE |= Adsppm_State_CoreTHERMALInit;
										result = MEMPWR_Init();
									}
                                    if(Adsppm_Status_Success == result)
                                    {
                                        gADSPPMINITSTATE |= Adsppm_State_CoreMEMPWRInit;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if((Adsppm_Status_Success == result) &&
            (AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_InitialState)))
    {
        AdsppmRscIdType rsc;
        for (rsc = Adsppm_Rsc_Id_Power; rsc < Adsppm_Rsc_Id_Max; rsc++)
        {
            result = RM_RequestResource(rsc);
            if(Adsppm_Status_Success != result)
            {
                gADSPPMINITSTATE |= rsc << 28;   /*put failed resource ID in high 4 bits*/
                break;
            }
        }
    }
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}
