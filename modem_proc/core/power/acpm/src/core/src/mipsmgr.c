/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * mipsmgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */


 /**
 *@fn MIPS_Init - init MIPS management data structure
 */

#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "requestmgr.h"
#include "asic.h"
#include "mipsmgr.h"
#include "ahbmgr.h"
#include "core_internal.h"
#include "hal_cpu_aggregator.h"
#include "hal_busdrv.h"


static AdsppmMipsAggregateDataType preMipsAggregateData, MipsAggregateData;
static MIPSCtxType  MIPSCtx;

/**
*@fn MIPS_Init - init MIPS management related data structure and mutex 
*@return Adsppm_Status_Failed
*@return Adsppm_Status_Success
*/

Adsppm_Status MIPS_Init(void)
{
     Adsppm_Status sts = Adsppm_Status_Success;
     ADSPPM_LOG_FUNC_ENTER;

     if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&MIPSCtx.MipsmgrCtxLock, NULL )) 
     {
        sts = Adsppm_Status_Failed;
     }
     if((Adsppm_Status_Success == sts)&& (NULL != MIPSCtx.MipsmgrCtxLock))
     {
        adsppm_lock(MIPSCtx.MipsmgrCtxLock);
        memset(&preMipsAggregateData, 0, sizeof(AdsppmMipsAggregateDataType));
        memset(&MipsAggregateData, 0, sizeof(AdsppmMipsAggregateDataType));
        MipsAggregateData.mipsToBWAggregateDataDdr.mipsBWSlavePort = AdsppmBusPort_Ddr_Slave;
        MipsAggregateData.mipsToBWAggregateDataDdr.mipsToBW.busRoute.slavePort = AdsppmBusPort_Ddr_Slave;
        MipsAggregateData.mipsToBWAggregateDataDdr.mipsToBW.busRoute.masterPort = AdsppmBusPort_Adsp_Master;
        preMipsAggregateData.mipsToBWAggregateDataDdr.mipsBWSlavePort = AdsppmBusPort_Ddr_Slave;
        preMipsAggregateData.mipsToBWAggregateDataDdr.mipsToBW.busRoute.slavePort = AdsppmBusPort_Ddr_Slave;
        preMipsAggregateData.mipsToBWAggregateDataDdr.mipsToBW.busRoute.masterPort = AdsppmBusPort_Adsp_Master;
        adsppm_unlock(MIPSCtx.MipsmgrCtxLock);
     }
     ADSPPM_LOG_FUNC_EXIT(sts);

     return sts;
}

Adsppm_Status mipsAggregateClientMIPS(coreUtils_Q_Type* pMipsReqQ)
{
    uint32 index = 0;
    Adsppm_Status sts = Adsppm_Status_Success;
    ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_Mips);
    RMCtxType *pLRMCtxItem = (RMCtxType*) coreUtils_Q_Check(pMipsReqQ);
    AdsppmBusBWDataUsageType* pDspBwReqData = NULL;

    for (index = 0; index < pMipsReqQ->nCnt; index++)
    {
        if (NULL != pLRMCtxItem)
        {
            if (RM_Valid == pLRMCtxItem->validFlag)
            {
                if (NULL == pLRMCtxItem->pRequestData)
                {
                    sts = Adsppm_Status_BadParm;
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                            "pRequestData=NULL\n");
                    break;
                }
                else
                {
                	//Get associated BW
                	if ((RM_Valid == pLRMCtxItem->validFlag_Ext) && (NULL != pLRMCtxItem->pRequestData_Ext))
                	{
                		pDspBwReqData = (AdsppmBusBWDataUsageType*)pLRMCtxItem->pRequestData_Ext;
                	}
                	else
                	{
                		pDspBwReqData = NULL;
                	}

                	//Aggregate Mips
                	ACM_AggregateMips(&MipsAggregateData, (AdsppmMipsRequestType*) (pLRMCtxItem->pRequestData), pDspBwReqData);
                }
            }
            pLRMCtxItem = (RMCtxType *) coreUtils_Q_Next(pMipsReqQ,
                    &(pLRMCtxItem->RMCtxLink));
        }
    }

    //Calculate CPP factor based on aggregation results
    sts = ACM_GetCppFromMips(&MipsAggregateData);

    if(Adsppm_Status_Success == sts)
    {
        //Specify whether only periodic clients are running
        //This has to be done before calling ACM and AHBM functions
        SetPeriodicUseCase((MipsAggregateData.mipsAggregate.numOfPeriodicClients == MipsAggregateData.mipsAggregate.numOfClients)?1:0);
    }
    ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_Mips);
    return sts;
}


/**
*@fn MIPS_ProcessRequest - process clients MIPS request 
*@param input RMCtxType *gpRMCtx - MIPS request data from clients 
*@return Adsppm_Status
*/

Adsppm_Status MIPS_ProcessRequest(coreUtils_Q_Type *pMipsReqQ)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;

    if(NULL == pMipsReqQ)
    {
        sts = Adsppm_Status_BadParm;
    }
    else
    {
        if(NULL == MIPSCtx.MipsmgrCtxLock)
        {
            sts = Adsppm_Status_NotInitialized;
        }
        else
        {
            adsppm_lock(MIPSCtx.MipsmgrCtxLock);
            memset(&MipsAggregateData, 0, sizeof(AdsppmMipsAggregateDataType));
            MipsAggregateData.mipsToBWAggregateDataDdr.mipsBWSlavePort = AdsppmBusPort_Ddr_Slave;
            //aggregate clients requests and decide if the usecase is periodic
            sts = mipsAggregateClientMIPS(pMipsReqQ);
            if(Adsppm_Status_Success == sts)
            {
            	if((Adsppm_Status_Success == sts) &&
            		 ((MipsAggregateData.mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ab !=
            			preMipsAggregateData.mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ab) ||
					 (MipsAggregateData.mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ib !=
            					preMipsAggregateData.mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ib) ||	
					 (MipsAggregateData.mipsAggregate.mipsTotal != preMipsAggregateData.mipsAggregate.mipsTotal) ||
            		 (MipsAggregateData.mipsAggregate.cppFactor != preMipsAggregateData.mipsAggregate.cppFactor) ||
            		 (MipsAggregateData.mipsAggregate.mipsPerThread != preMipsAggregateData.mipsAggregate.mipsPerThread)))
            	{
            		//Call into the CPU aggregator to set DSP performance level
            		//TODO: Add check for previous request
            		sts =  CA_RequestCpuPerformanceLevel(&MipsAggregateData);
            	}
            }
            if(Adsppm_Status_Success == sts)
            {
                memscpy(&preMipsAggregateData, sizeof(AdsppmMipsAggregateDataType), &MipsAggregateData, sizeof(AdsppmMipsAggregateDataType));
            }
            adsppm_unlock(MIPSCtx.MipsmgrCtxLock);
        }
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}



