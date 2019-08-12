/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * busmgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "busmgr.h"
#include "asic.h"
#include "core_internal.h"
#include "hal_clkrgm.h"
#include "ahbmgr.h"
#include "hal_busdrv.h"
#include "stdlib.h"


static BUSMgrCtxType gBUSMgrCtx;

/** internal function to get bus port arrary index for bw Aggregate*/
static Adsppm_Status BusMgrGetIndex(AdsppmBusRouteType *pRoute, uint32 *pExtRouteIndex);
/** internal function to get bus bw type based on bus route */
static BUSMgrBwTypeType BusMgrGetBwType(AsicBusPortDescriptorType *pMasterPort, AsicBusPortDescriptorType *pSlavePort);

/**
 *@fn BUS_Init  - bus management init, init mutex and  global varibles.
 */
Adsppm_Status BUS_Init(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;

    memset(&gBUSMgrCtx, 0, sizeof(BUSMgrCtxType));

    if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&gBUSMgrCtx.BUSMgrCtxLock, NULL ))
    {
        sts = Adsppm_Status_Failed;
    }
    else
    {
        //get support route pair number from asic manager
        gBUSMgrCtx.extRouteNum = ACMBus_GetNumberOfExtRoutes();
        gBUSMgrCtx.pExtRoutes = malloc(gBUSMgrCtx.extRouteNum*sizeof(AsicBusExtRouteType));
        gBUSMgrCtx.pBUSMgrExtBWAggregate[0] = malloc(gBUSMgrCtx.extRouteNum*sizeof(AdsppmBusBWRequestValueType));
        gBUSMgrCtx.pBUSMgrExtBWAggregate[1] = malloc(gBUSMgrCtx.extRouteNum*sizeof(AdsppmBusBWRequestValueType));

        //any of malloc failed
        if((NULL == gBUSMgrCtx.pExtRoutes) || (NULL ==gBUSMgrCtx.pBUSMgrExtBWAggregate[0]) || (NULL ==gBUSMgrCtx.pBUSMgrExtBWAggregate[1]))
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to allocate aggregate buffer\n");

            if( NULL != gBUSMgrCtx.pExtRoutes)
            {
                free(gBUSMgrCtx.pExtRoutes);
            }
            if(NULL != gBUSMgrCtx.pBUSMgrExtBWAggregate[0])
            {
                free(gBUSMgrCtx.pBUSMgrExtBWAggregate[0]);
            }
            if(NULL != gBUSMgrCtx.pBUSMgrExtBWAggregate[1])
            {
                free( gBUSMgrCtx.pBUSMgrExtBWAggregate[1]);
            }
            sts = Adsppm_Status_NoMemory;
        }
        else
        {
            //get support route pair from asic manager and save
            ACMBus_GetExtRoutes(gBUSMgrCtx.pExtRoutes);
        }
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

/**
 *@fn BUS_ProcessRequest  - bus management main function, separate bw to external and internal bw and do external bw aggregate
 *@param input: RMCtxType *gpRMCtx
 *@return Adsppm_Status
 */

Adsppm_Status BUS_ProcessRequest(coreUtils_Q_Type *pBWReqQ)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 index = 0, extRouteIndex = 0;
    uint32 numOfBW = 0, bwNum = 0;
    AdsppmBwReqType *pBwReqData = NULL;
    RMCtxType *pLRMCtxItem = NULL;
    AsicBusPortDescriptorType *pAsicMasterBusPortDescriptor, *pAsicSlaveBusPortDescriptor;
    BUSMgrBwTypeType  busMgrBwType = BUSMgr_Bw_None;
    AdsppmBusBWRequestValueType updateExtBusBwValue[gBUSMgrCtx.extRouteNum];
    uint32                      updateAHBClkSts[AdsppmClk_EnumMax-1];
    uint32                 numOfExtBw = 0;
    uint32                 numOfInterBw = 0;
    AsicClkDescriptorType* pClk_M = NULL, *pClk_S = NULL;

    ADSPPM_LOG_FUNC_ENTER;

    if(NULL == pBWReqQ)
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Pointer is NULL\n");
    }
    else
    {
        if(NULL == gBUSMgrCtx.BUSMgrCtxLock)
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized\n");
            sts = Adsppm_Status_NotInitialized;
        }
        else
        {
            //lock for bus manager global data update
            adsppm_lock(gBUSMgrCtx.BUSMgrCtxLock);
            //init local variables
            memset(updateExtBusBwValue, 0, sizeof(AdsppmBusBWRequestValueType)*gBUSMgrCtx.extRouteNum);
            memset(updateAHBClkSts, 0, AdsppmClk_EnumMax);
            memset(gBUSMgrCtx.externalBwReq, 0, sizeof(busMgrExtBwReqType)*(MAX_NUM_OF_ADSPPM_CLIENTS-1)*MAX_NUM_OF_ADSPPM_BW_ARRAY);
            memset(gBUSMgrCtx.internalBwReq, 0, sizeof(busMgrInterBwReqType)*(MAX_NUM_OF_ADSPPM_CLIENTS-1)*MAX_NUM_OF_ADSPPM_BW_ARRAY);

            ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_BW);

            //go through each client's bw request
            pLRMCtxItem = (RMCtxType *)coreUtils_Q_Check(pBWReqQ);
            for(index = 0; index <pBWReqQ->nCnt; index++)
            {
                //check the client is valid
                if(NULL != pLRMCtxItem)
                {
                if(RM_Valid == pLRMCtxItem->validFlag)
                {
                    if(NULL == ((AdsppmBwReqType *)(pLRMCtxItem->pRequestData)))
                    {
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "pRequestData = NULL, clientId=%d\n", index+1);
                        sts = Adsppm_Status_BadParm;
                        break;
                    }
                    else
                    {
                        pBwReqData = (AdsppmBwReqType *)(pLRMCtxItem->pRequestData);
                        //go through each bw request for this client
                        for(bwNum = 0; bwNum < pBwReqData->numOfBw; bwNum++)
                        {
                            //get port description from asic config
                            pAsicMasterBusPortDescriptor = ACMBus_GetPortDescriptor(pBwReqData->pBwArray[bwNum].busRoute.masterPort);
                            pAsicSlaveBusPortDescriptor = ACMBus_GetPortDescriptor(pBwReqData->pBwArray[bwNum].busRoute.slavePort);
                            //get bw type: internal only, external only, internal&external
                            if(( NULL == pAsicMasterBusPortDescriptor) || (NULL == pAsicSlaveBusPortDescriptor))
                            {
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Bus route descriptor is NULL, master=%d slave=%d \n", pBwReqData->pBwArray[bwNum].busRoute.masterPort, pBwReqData->pBwArray[bwNum].busRoute.slavePort);
                                sts = Adsppm_Status_BadParm;
                                break;
                            }
                            else
                            {
                                busMgrBwType = BusMgrGetBwType(pAsicMasterBusPortDescriptor, pAsicSlaveBusPortDescriptor);
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "master=%d, slave=%d, bwType=%d\n",pAsicMasterBusPortDescriptor->portConnection, pAsicSlaveBusPortDescriptor->portConnection, busMgrBwType);

                                //internal&external bw process
                                if((Adsppm_Status_Success == sts)
                                        && (BUSMgr_Bw_Internal_External == busMgrBwType))
                                {
                                    //check bw array num
                                    if(((MAX_NUM_OF_ADSPPM_CLIENTS-1)*MAX_NUM_OF_ADSPPM_BW_ARRAY <= numOfExtBw)
										|| ( (MAX_NUM_OF_ADSPPM_CLIENTS-1)*MAX_NUM_OF_ADSPPM_BW_ARRAY <= numOfInterBw))
                                    {
                                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "too many BW entries\n");
                                        sts = Adsppm_Status_Failed;
                                        break;
                                    }
                                    else
                                    {
                                        //setup bw data array
                                        gBUSMgrCtx.externalBwReq[numOfExtBw].pBWreq = &(pBwReqData->pBwArray[bwNum]);
                                        gBUSMgrCtx.internalBwReq[numOfInterBw].pBWreq = &(pBwReqData->pBwArray[bwNum]);
                                        //external master port
                                        if(AsicBusPort_Ext_M == pAsicMasterBusPortDescriptor->portConnection)
                                        {
                                            gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.masterPort = pBwReqData->pBwArray[bwNum].busRoute.masterPort;
                                            gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.slavePort = pAsicSlaveBusPortDescriptor->accessPort;
                                            gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_M = pAsicMasterBusPortDescriptor->busClk;
                                            gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_S = pAsicSlaveBusPortDescriptor->busClk;
                                            gBUSMgrCtx.internalBwReq[numOfInterBw].csrClk = pAsicSlaveBusPortDescriptor->busClk;
                                            ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO_EXT, "interexter,exterM=%d,exterS=%d,busclkM=%d,busclkS=%d, csrClk=%d\n",gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.masterPort, gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.slavePort, gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_M, gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_S, gBUSMgrCtx.internalBwReq[numOfInterBw].csrClk);
                                        }
                                        else if(AsicBusPort_Ext_S == pAsicSlaveBusPortDescriptor->portConnection)
                                        {
                                            gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.masterPort = pAsicMasterBusPortDescriptor->accessPort;
                                            gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.slavePort = pBwReqData->pBwArray[bwNum].busRoute.slavePort;
                                            gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_M = pAsicMasterBusPortDescriptor->busClk;
                                            gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_S = pAsicSlaveBusPortDescriptor->busClk;
                                            gBUSMgrCtx.internalBwReq[numOfInterBw].csrClk = pAsicSlaveBusPortDescriptor->busClk;
                                            ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO_EXT, "interexter,exterM=%d,exterS=%d,busclkM=%d,busclkS=%d, csrClk=%d\n",gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.masterPort, gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.slavePort, gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_M, gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_S, gBUSMgrCtx.internalBwReq[numOfInterBw].csrClk);
                                        }
                                        numOfExtBw++;
                                        numOfInterBw++;
                                    }
                                }
                                //external only bw process
                                else if((Adsppm_Status_Success == sts)
                                        && (BUSMgr_Bw_External_Only == busMgrBwType))
                                {
                                    gBUSMgrCtx.externalBwReq[numOfExtBw].pBWreq = &(pBwReqData->pBwArray[bwNum]);
                                    gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.masterPort = pBwReqData->pBwArray[bwNum].busRoute.masterPort;
                                    gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.slavePort = pBwReqData->pBwArray[bwNum].busRoute.slavePort;
                                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT, "exter only,exterM=%d,exterS=%d \n",gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.masterPort, gBUSMgrCtx.externalBwReq[numOfExtBw].extRoute.slavePort );
                                    numOfExtBw++;
                                }
                                //internal only bw process
                                else if((Adsppm_Status_Success == sts)
                                        && (BUSMgr_Bw_Internal_Only == busMgrBwType))
                                {
                                    gBUSMgrCtx.internalBwReq[numOfInterBw].pBWreq = &(pBwReqData->pBwArray[bwNum]);
                                    gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_M = pAsicMasterBusPortDescriptor->busClk;
                                    gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_S = pAsicSlaveBusPortDescriptor->busClk;
                                    gBUSMgrCtx.internalBwReq[numOfInterBw].csrClk = pAsicSlaveBusPortDescriptor->busClk;
                                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "inter only,master busclk=%d, slave busclk=%d, csrclk=%d\n",gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_M, gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_S, gBUSMgrCtx.internalBwReq[numOfInterBw].csrClk);
                                    numOfInterBw++;
                                }
                            }//check master slave port
                        }//for bwnum
                    }//check bad param
                } //req data is valid

                //check the extended data is valid (reg programming)
                if(RM_Valid == pLRMCtxItem->validFlag_Ext)
                {
                    if(NULL == ((AdsppmBwReqType *)(pLRMCtxItem->pRequestData_Ext)))
                    {
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "pRequestData_Ext = NULL, clientId=%d\n", pLRMCtxItem->clientId);
                        sts = Adsppm_Status_BadParm;
                        break;
                    }
                    else
                    {
                        AdsppmBusBWRequestValueType* pRegBwReqData = (AdsppmBusBWRequestValueType *)(pLRMCtxItem->pRequestData_Ext);
                        //get port description from asic config
                        pAsicMasterBusPortDescriptor = ACMBus_GetPortDescriptor(pRegBwReqData->busRoute.masterPort);
                        pAsicSlaveBusPortDescriptor = ACMBus_GetPortDescriptor(pRegBwReqData->busRoute.slavePort);
                        //get bw type: internal only, external only, internal&external
                        if(( NULL == pAsicMasterBusPortDescriptor) || (NULL == pAsicSlaveBusPortDescriptor))
                        {
                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Bus route descriptor is NULL, master=%d slave=%d \n", pRegBwReqData->busRoute.masterPort, pRegBwReqData->busRoute.slavePort);
                            sts = Adsppm_Status_BadParm;
                            break;
                        }
                        else
                        {
                            busMgrBwType = BusMgrGetBwType(pAsicMasterBusPortDescriptor, pAsicSlaveBusPortDescriptor);
                            ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "Reg BW master=%d, slave=%d, bwType=%d\n", pRegBwReqData->busRoute.masterPort, pRegBwReqData->busRoute.slavePort, busMgrBwType);

                            gBUSMgrCtx.internalBwReq[numOfInterBw].pBWreq = pRegBwReqData;
                            gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_M = pAsicMasterBusPortDescriptor->busClk;
                            gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_S = pAsicSlaveBusPortDescriptor->busClk;
                            gBUSMgrCtx.internalBwReq[numOfInterBw].csrClk = pAsicSlaveBusPortDescriptor->busClk;
                            ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO_EXT, "regprogrammingInter,interM=%d,interS=%d, busclk_M=%d, busClk_S=%d, csrClk=%d \n",pAsicMasterBusPortDescriptor->portConnection, pAsicSlaveBusPortDescriptor->portConnection, gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_M, gBUSMgrCtx.internalBwReq[numOfInterBw].busClk_S, gBUSMgrCtx.internalBwReq[numOfInterBw].csrClk );
                            numOfInterBw++;
                        }//check master slave port

                    }//check bad param
                } //req data is valid
                pLRMCtxItem = (RMCtxType *)coreUtils_Q_Next(pBWReqQ, &(pLRMCtxItem->RMCtxLink));
                } //check RMCTX
            } // for clients num
           
			
			ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT, "forallclients, interbwNum=%d, exterbwNum=%d\n",numOfInterBw, numOfExtBw);
            //process internal AHB bandwidth
			
            if((Adsppm_Status_Success == sts) && ( 0 < numOfInterBw))
            {
                sts = AHBM_ProcessRequest(numOfInterBw, gBUSMgrCtx.internalBwReq);
            }

            for(index = AdsppmClk_Adsp_Hclk; index < AdsppmClk_HwRsp_Core; index++)
            {
                gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][index] = 0;
            }
            //Aggregate AHB clock
            for(index= 0; index< numOfInterBw; index++)
            {
                AdsppmClkIdType clkId_S = gBUSMgrCtx.internalBwReq[index].busClk_S;
                AdsppmClkIdType clkId_M = gBUSMgrCtx.internalBwReq[index].busClk_M;
                //get port description from asic config
                pAsicMasterBusPortDescriptor = ACMBus_GetPortDescriptor(gBUSMgrCtx.internalBwReq[index].pBWreq->busRoute.masterPort);
                pAsicSlaveBusPortDescriptor = ACMBus_GetPortDescriptor(gBUSMgrCtx.internalBwReq[index].pBWreq->busRoute.slavePort);

                if(( NULL == pAsicMasterBusPortDescriptor) || (NULL == pAsicSlaveBusPortDescriptor))
                {
                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Bus route descriptor is NULL, master=%d slave=%d \n",
                            gBUSMgrCtx.internalBwReq[index].pBWreq->busRoute.masterPort, gBUSMgrCtx.internalBwReq[index].pBWreq->busRoute.slavePort);
                    sts = Adsppm_Status_BadParm;
                    break;
                }

                if(AdsppmBusBWOperation_RegAccess == gBUSMgrCtx.internalBwReq[index].pBWreq->bwOperation)
                {
                    // Use CSR clock for reg access
                    clkId_S = gBUSMgrCtx.internalBwReq[index].csrClk;
                    pClk_S = ACMClk_GetClockDescriptor(clkId_S);
                    pClk_M = ACMClk_GetClockDescriptor(clkId_M);
                    if(NULL != pClk_S)
                    {
                        switch(pClk_S->clkCntlType)
                        {
                        case AsicClk_CntlSW:
                            //increase AHB clock voting number for slave port
                            gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][clkId_S] = 1;
                            break;
                        case AsicClk_CntlSW_DCG:
                            if(AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
                            {
                                //increase AHB clock voting number only if DCG is not enabled for slave clock
                                gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][clkId_S] = 1;
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    if(NULL != pClk_M)
                    {
                        switch(pClk_M->clkCntlType)
                        {
                        case AsicClk_CntlSW:
                            //increase AHB clock voting number for master port
                            gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][clkId_M] = 1;
                            break;
                        case AsicClk_CntlSW_DCG:
                            if(AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
                            {
                                //increase AHB clock voting number for master port only if DCG is not enabled
                                gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][clkId_M] = 1;
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
                else
                {
                    //increase AHB clock voting number for slave port
                    gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][clkId_S] = 1;
                    //increase AHB clock voting number for master port
                    gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][clkId_M] = 1;
                }
                //Account for connection for slave port
                if(0 < gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][clkId_S])
                {
                    if(pAsicSlaveBusPortDescriptor->portConnection & AsicBusPort_AhbE_S)
                    {
                        //Also force AHB_E ON
                        gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][AdsppmClk_AhbE_Hclk] = 1;
                    }
                    if(pAsicSlaveBusPortDescriptor->portConnection & AsicBusPort_AhbI_S)
                    {
                        //Also force AHB_I ON
                        gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][AdsppmClk_AhbI_Hclk] = 1;
                    }
                    if(pAsicSlaveBusPortDescriptor->portConnection & AsicBusPort_AhbX_S)
                    {
                        //Also force AHB_I ON
                        gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][AdsppmClk_AhbX_Hclk] = 1;
                    }
                }
                //Account for connection for master port
                if(0 < gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][clkId_M])
                {
                    if(pAsicMasterBusPortDescriptor->portConnection & AsicBusPort_AhbE_M)
                    {
                        //Also force AHB_E ON
                        gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][AdsppmClk_AhbE_Hclk] = 1;
                    }
                    if(pAsicMasterBusPortDescriptor->portConnection & AsicBusPort_AhbI_M)
                    {
                        //Also force AHB_I ON
                        gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][AdsppmClk_AhbI_Hclk] = 1;
                    }
                    if(pAsicMasterBusPortDescriptor->portConnection & AsicBusPort_AhbX_M)
                    {
                        //Also force AHB_I ON
                        gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][AdsppmClk_AhbX_Hclk] = 1;
                    }
                }
            }
            //setup AHB clocks
            //Exclude all core and AHB root clocks from processing
            for(index = AdsppmClk_Adsp_Hclk; index < AdsppmClk_HwRsp_Core; index++)
            {
                pClk_S = ACMClk_GetClockDescriptor(index);
                if(AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_Ahb_Sw_CG))
                {
                    // Force all AHB clocks to be enabled
                    gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][index] = 1;
                }
                if(NULL != pClk_S && gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex^1][index] != gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][index])
                {
                    switch(pClk_S->clkCntlType)
                    {
                    case AsicClk_CntlSW_DCG:
                    case AsicClk_CntlAlwaysON_DCG:
                    case AsicClk_CntlSW:
                        if(0 < gBUSMgrCtx.AHBClkSts[gBUSMgrCtx.preCurrentClkIndex][index])
                        {
                            sts = ClkRgm_EnableClock((AdsppmClkIdType)index, Hal_ClkRgmEnable);
                        }
                        else
                        {
                            sts = ClkRgm_EnableClock((AdsppmClkIdType)index, Hal_ClkRgmDisable);
                        }
                        break;
                    default:
                        //Ignore AON and AOFF clocks
                        break;
                    }
                    if(Adsppm_Status_Success != sts)
                    {
                        break;
                    }
                }
            }
            //switch current and previous AHB clock sts
            gBUSMgrCtx.preCurrentClkIndex ^= 1;

            memset(&(gBUSMgrCtx.pBUSMgrExtBWAggregate[gBUSMgrCtx.preCurrentBwIndex][0]), 0, gBUSMgrCtx.extRouteNum*sizeof(AdsppmBusBWRequestValueType));
            //Aggregate external BW per pair
            for(index=0; index < numOfExtBw; index++)
            {
                sts = BusMgrGetIndex(&(gBUSMgrCtx.externalBwReq[index].extRoute), &extRouteIndex);
                if(Adsppm_Status_Success == sts)
                {
                    sts = ACM_BusBWAggregate(&(gBUSMgrCtx.pBUSMgrExtBWAggregate[gBUSMgrCtx.preCurrentBwIndex][extRouteIndex].bwValue.busBwAbIb), &(gBUSMgrCtx.externalBwReq[index].pBWreq->bwValue));
                    gBUSMgrCtx.pBUSMgrExtBWAggregate[gBUSMgrCtx.preCurrentBwIndex][extRouteIndex].busRoute = gBUSMgrCtx.externalBwReq[index].extRoute;
                }
            }
            //check bandwidth difference and call hal bus driver
            if(Adsppm_Status_Success == sts)
            {
                for(index = 0; index < gBUSMgrCtx.extRouteNum; index++)
                {
                    if((gBUSMgrCtx.pBUSMgrExtBWAggregate[gBUSMgrCtx.preCurrentBwIndex^1][index].bwValue.busBwAbIb.Ab != gBUSMgrCtx.pBUSMgrExtBWAggregate[gBUSMgrCtx.preCurrentBwIndex][index].bwValue.busBwAbIb.Ab)
                            || (gBUSMgrCtx.pBUSMgrExtBWAggregate[gBUSMgrCtx.preCurrentBwIndex^1][index].bwValue.busBwAbIb.Ib != gBUSMgrCtx.pBUSMgrExtBWAggregate[gBUSMgrCtx.preCurrentBwIndex][index].bwValue.busBwAbIb.Ib))
                    {
                        memscpy(&(updateExtBusBwValue[numOfBW]), sizeof(AdsppmBusBWRequestValueType), &(gBUSMgrCtx.pBUSMgrExtBWAggregate[gBUSMgrCtx.preCurrentBwIndex][index]), sizeof(AdsppmBusBWRequestValueType));
                        ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "ext BW %d updated: Ib=%d, Ab=%d\n",
                                                        numOfBW,(uint32)updateExtBusBwValue[numOfBW].bwValue.busBwAbIb.Ib,
                                                        (uint32)updateExtBusBwValue[numOfBW].bwValue.busBwAbIb.Ab);
                        numOfBW++;
                    }
                }
                //issue external bw request to hal
                gBUSMgrCtx.preCurrentBwIndex ^= 1;
            }

            ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_BW);
            adsppm_unlock(gBUSMgrCtx.BUSMgrCtxLock);

            if((Adsppm_Status_Success == sts)&&(0< numOfBW))
            {
                sts = Bus_IssueBWRequest(numOfBW,  &updateExtBusBwValue[0]);
            }
        }
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}


static Adsppm_Status BusMgrGetIndex(AdsppmBusRouteType *pRoute, uint32 *pExtRouteIndex)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 index = 0;
    *pExtRouteIndex = gBUSMgrCtx.extRouteNum + 1;

    for (index=0; index < gBUSMgrCtx.extRouteNum; index++)
    {
        if(( gBUSMgrCtx.pExtRoutes[index].masterPort.adsppmMaster == pRoute->masterPort )
                && ( gBUSMgrCtx.pExtRoutes[index].slavePort.adsppmSlave == pRoute->slavePort ))
        {
            *pExtRouteIndex = index;
        }
    }
    if( gBUSMgrCtx.extRouteNum +1 == *pExtRouteIndex )
    {
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Invalid external route,M=%d,S=%d\n", pRoute->masterPort,pRoute->slavePort);
        sts = Adsppm_Status_BadParm;
    }
    return sts;
}

static BUSMgrBwTypeType BusMgrGetBwType(AsicBusPortDescriptorType *pMasterPort, AsicBusPortDescriptorType *pSlavePort)
{
    //TODO get port information from asic configuration
    BUSMgrBwTypeType bwType = BUSMgr_Bw_None;

    if((0 != (pMasterPort->portConnection & AsicBusPort_Ext_Any ))
            && (0 != (pSlavePort->portConnection & AsicBusPort_Ext_Any )))
    {
        bwType = BUSMgr_Bw_External_Only;
    }
    else  if((0 != (pMasterPort->portConnection & AsicBusPort_Ahb_Any ))
            && (0 != (pSlavePort->portConnection & AsicBusPort_Ahb_Any )))
    {
        bwType = BUSMgr_Bw_Internal_Only;
    }
    else if((0 != (pMasterPort->portConnection & AsicBusPort_Ext_Any))
            ||( 0 != (pMasterPort->portConnection & AsicBusPort_Ahb_Any))
            || (0 != (pSlavePort->portConnection & AsicBusPort_Ext_Any))
            || (0 != (pSlavePort->portConnection & AsicBusPort_Ahb_Any)))
    {
        bwType = BUSMgr_Bw_Internal_External;
    }
    else
    {
        bwType = BUSMgr_Bw_None;
    }
    return bwType;
}

