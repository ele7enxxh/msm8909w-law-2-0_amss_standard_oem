/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * main.c
 *
 *  Created on: Feb 10, 2012
 *      Author: yrusakov
 */

/** edit history
 *   04/16/2012 ------------ Alisonc    init
 */
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "hal.h"
#include "core.h"
#include "asic.h"
#include "ahbmgr.h"
#include "core_internal.h"
#include "hal_clkrgm.h"
#include "busmgr.h"

static AHBMCtxType AHBMCtx;


/**
 *@fn AHBM_Init - init AHB management related data structure and mutex
 *@return Adsppm_Status_Failed
 *@return Adsppm_Status_Success
 */

Adsppm_Status AHBM_Init(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&AHBMCtx.AHBMgrCtxLock, NULL ))
    {
        sts = Adsppm_Status_Failed;
    }
    if((Adsppm_Status_Success == sts)&& (NULL != AHBMCtx.AHBMgrCtxLock))
    {
        memset(&AHBMCtx.curAHBBwData, 0, sizeof(AdsppmBusBWDataIbAbType));
        AHBMCtx.preAHBClock = 0;
        AHBMCtx.activeAHBClock = 0;
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}


/**
 * @fn ahbm_AggregateAhbBw(int numOfBw, busMgrInterBwReqType* pBusBwReq) - performs aggregation of
 * AHB BW for active and sleep (only non-suppressible BW) states. The aggregated BW is then translated into
 * AHB clock frequency for both active and sleep states correspondingly.
 *
 * @param numOfBw - number of internal BW entries
 * @param pBusBwReq - pointer to the internal BW array
 * @return - execution status
 */
Adsppm_Status ahbm_AggregateAhbBw(int numOfBw, busMgrInterBwReqType* pBusBwReq)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 index = 0;
    AHBMCtx.curAHBBwData.Ib = 0;
    AHBMCtx.curAHBBwData.Ab = 0;
    AHBMCtx.nonSuppressibleAhbBW.Ib = 0;
    AHBMCtx.nonSuppressibleAhbBW.Ab = 0;
    for (index = 0; index < numOfBw; index++)
    {
        sts = ACM_BusBWAggregate(&(AHBMCtx.curAHBBwData),
                &(pBusBwReq[index].pBWreq->bwValue));
        if (Adsppm_Status_Success != sts)
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "Aggregation failed: %d\n", sts);
            break;
        }
        if (AdsppmBusPort_Adsp_Master
                != pBusBwReq[index].pBWreq->busRoute.masterPort)
        { //Aggregate all Non-suppressible BW requests
          //All BW requirements originating from ADSP are considered
          //suppressible and should be excluded
            sts = ACM_BusBWAggregate(&(AHBMCtx.nonSuppressibleAhbBW),
                    &(pBusBwReq[index].pBWreq->bwValue));
            if (Adsppm_Status_Success != sts)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "Aggregation failed: %d\n", sts);
                break;
            }
        }
    }
    if(Adsppm_Status_Success == sts)
    {
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT, "Aggregated BW: Ab=%d, Ib=%d\n", (uint32)AHBMCtx.curAHBBwData.Ab, (uint32)AHBMCtx.curAHBBwData.Ib);
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT, "Aggregated non-suppressible BW: Ab=%d, Ib=%d\n",
                (uint32)AHBMCtx.nonSuppressibleAhbBW.Ab, (uint32)AHBMCtx.nonSuppressibleAhbBW.Ib);

        sts = ACM_GetClockFromBW(&AHBMCtx.nonSuppressibleAHBClock, &AHBMCtx.nonSuppressibleAhbBW);

        if(Adsppm_Status_Success == sts)
        {
            sts = ACM_GetClockFromBW(&AHBMCtx.aggregatedAHBClock, &AHBMCtx.curAHBBwData);
        }
    }
    return sts;
}

Adsppm_Status ahbm_SetAHBClock(uint32 ahbClock)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "AHB clock: %d\n", ahbClock);
    if (AHBMCtx.preAHBClock != ahbClock)
    {
        sts = ClkRgm_SetClock(AdsppmClk_Ahb_Root, ahbClock);
        if (Adsppm_Status_Success == sts)
        {
            AHBMCtx.preAHBClock = ahbClock;
        }
    }
    return sts;
}

inline Adsppm_Status AHBM_SetSleepAHBClock(void)
{
    return   ahbm_SetAHBClock(AHBMCtx.nonSuppressibleAHBClock);
}

inline Adsppm_Status AHBM_SetActiveAHBClock(void)
{
    return ahbm_SetAHBClock(AHBMCtx.activeAHBClock);
}

Adsppm_Status AHBM_AdjustAndSetAHBClock(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    adsppm_lock(AHBMCtx.AHBMgrCtxLock);

    //Adjust AHB clock if ADSP is active
    //The timeline optimization by keeping AHB clock not lower than predefined frequency (19.2MHz) is applied
    // also to non-periodic clients in order to compensate for inadequate BW voting from audio clients.
    // TODO: revert this change once proper solution for AHb voting accounting for latency hiding is implemented
    if((AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_TimelineOptimisationAhb)) /*&&
            IsPeriodicUseCase()*/)
    {
        //Select baseline or aggregated value if it is higher
        AHBMCtx.activeAHBClock = ACM_AdjustParamValue(AsicFeatureId_TimelineOptimisationAhb, AHBMCtx.aggregatedAHBClock);
    }
    else
    {
        AHBMCtx.activeAHBClock = AHBMCtx.aggregatedAHBClock;
    }

    sts = AHBM_SetActiveAHBClock();
    adsppm_unlock(AHBMCtx.AHBMgrCtxLock);
    return sts;
}

/**
 *@fn AHBM_ProcessRequest - Aggregate AHB bandwidth and convert to AHB clock
 *@param input int numOfBw
 *@param input AdsppmBusBWRequestValueType *pBusBwReq
 *@return Adsppm_Status
 */

Adsppm_Status AHBM_ProcessRequest(int numOfBw, busMgrInterBwReqType *pBusBwReq)
{
    Adsppm_Status sts = Adsppm_Status_Success;

    ADSPPM_LOG_FUNC_ENTER;
    if((NULL == pBusBwReq) || (0 == numOfBw))
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pBusBwReq=NULL\n");
    }
    else
    {
        if(NULL == AHBMCtx.AHBMgrCtxLock)
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized\n");
            sts = Adsppm_Status_NotInitialized;
        }
        else
        {
            adsppm_lock(AHBMCtx.AHBMgrCtxLock);
            sts = ahbm_AggregateAhbBw(numOfBw, pBusBwReq);
            adsppm_unlock(AHBMCtx.AHBMgrCtxLock);

            if(Adsppm_Status_Success == sts)
            {
                sts = AHBM_AdjustAndSetAHBClock();
            }

        }
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}
