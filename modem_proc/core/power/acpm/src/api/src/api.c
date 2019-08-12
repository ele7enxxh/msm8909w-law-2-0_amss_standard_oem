


/*==============================================================================

FILE:      api.c

DESCRIPTION: MMPM API wrapper

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A



* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.

* Qualcomm Technologies, Inc. Confidential and Proprietary.

==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/power/acpm/src/api/src/api.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------------
04/03/12   alisonc      Initial version

===================================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
//#include "AEEstd.h"
#include "mmpm.h"
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "stdlib.h"
#include "core.h"
#include "asyncmgr.h"
#include "coreUtils.h"
#include "DDIPlatformInfo.h"

#ifdef ADSPPM_TEST
#include "adsppm_test_main.h"
#endif

/** this global flag set to 1 to disable MMPM_Request_Ext at runtime, only return MMPM_STATUS_SUCCESS*/
static volatile uint32 gDISABLEADSPPMREQUESTFLAG = 0;

static struct{
	uint32 gNumMipsRequest;
	AdsppmBusRouteType * pMipsRouteArray;
} gAdsppmApiCtx;

extern uint32 gADSPPMINITSTATE;

static const AdsppmClkIdType mmpmToAdsppmClk[MMPM_CLK_ID_LPASS_ENUM_MAX]=
{
        AdsppmClk_None, 
        AdsppmClk_HwRsp_Core, 
        AdsppmClk_Midi_Core,
        AdsppmClk_AvSync_Xo, 
        AdsppmClk_AvSync_Bt, 
        AdsppmClk_AvSync_Fm,
        AdsppmClk_Slimbus_Core, 
        AdsppmClk_Avtimer_core, 
        AdsppmClk_Atime_core,
        AdsppmClk_Adsp_Core, 
        AdsppmClk_Ahb_Root
};

static const AdsppmBusPortIdType mmpmToAdsppmBusPort[MMPM_BW_PORT_ID_MAX]=
{ 
    AdsppmBusPort_None,                   //!< AdsppmBusMaster_None
    AdsppmBusPort_Adsp_Master,                //!< AdsppmBusMaster_Adsp
    AdsppmBusPort_Dml_Master,                 //!< AdsppmBusMaster_Dml
    AdsppmBusPort_Aif_Master,                 //!< AdsppmBusMaster_Aif
    AdsppmBusPort_Slimbus_Master,             //!< AdsppmBusMaster_Slimbus
    AdsppmBusPort_Midi_Master,                //!< AdsppmBusMaster_Midi
    AdsppmBusPort_HwRsmp_Master,              //!< AdsppmBusMaster_HwRsmp
    AdsppmBusPort_Ext_Ahb_Master,             //!< SNOC Master port from AHB-X
    AdsppmBusPort_Spdif_Master,               //!< SPDIF master port
    AdsppmBusPort_Hdmirx_Master,              //!< HDMIRX slave port
    AdsppmBusPort_Sif_Master,                 //!< SIF slave port    
    AdsppmBusPort_Dml_Slave,                  //!< AdsppmBusSlave_Dml
    AdsppmBusPort_Aif_Slave,                  //!< AdsppmBusSlave_Aif
    AdsppmBusPort_Slimbus_Slave,              //!< AdsppmBusSlave_Slimbus
    AdsppmBusPort_Midi_Slave,                 //!< AdsppmBusSlave_Midi
    AdsppmBusPort_HwRsmp_Slave,               //!< AdsppmBusSlave_HwRsmp
    AdsppmBusPort_AvSync_Slave,               //!< AdsppmBusSlave_AvSync
    AdsppmBusPort_Lpm_Slave,                  //!< AdsppmBusSlave_Lpm
    AdsppmBusPort_Sram_Slave,                 //!< AdsppmBusSlave_Sram
    AdsppmBusPort_Ext_Ahb_Slave,              //!< SNOC Slave port from AHB-E
    AdsppmBusPort_Ddr_Slave,                  //!< AdsppmBusSlave_Ddr
    AdsppmBusPort_Ocmem_Slave,                //!< AdsppmBusSlave_Ocmem
    AdsppmBusPort_PerifNoc_Slave,             //!< AdsppmBusSlave_PerifNoc
	AdsppmBusPort_DCodec_Slave,				  //!< AdsppmBusPort_DCodec
    AdsppmBusPort_Spdif_Slave,                //!< SPDIF slave port
    AdsppmBusPort_Hdmirx_Slave,               //!< HDMIRX slave port
    AdsppmBusPort_Sif_Slave,                  //!< SIF slave port
    AdsppmBusPort_Bstc_Slave,                 //!< BSTC slave port
    AdsppmBusPort_Core,                       //!< Core current client is registered for. Should be specified in the srcPortId for Core-toMemory transfers*/
};



__inline uint32 isMipsRelatedBw(AdsppmBusPortIdType masterPort,AdsppmBusPortIdType slavePort)
{
	uint32 result = FALSE;
	int i = 0;
	for(i = 0; i < gAdsppmApiCtx.gNumMipsRequest; i++)
	{
		if ((masterPort == gAdsppmApiCtx.pMipsRouteArray[i].masterPort) &&
				(slavePort == gAdsppmApiCtx.pMipsRouteArray[i].slavePort))
		{
			result = TRUE;
			break;
		}
	}
	return result;
}


__inline MMPM_STATUS apiProcessSingleRequest(AdsppmRscIdType adsppmRscId, MmpmApiType apiType, uint32* pAsyncRscMap)
{
	MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
	*pAsyncRscMap |= (1<<adsppmRscId);
	if( MMPM_API_TYPE_ASYNC != apiType)
	{
		sts = (MMPM_STATUS)RM_RequestResource(adsppmRscId);
	}
	return sts;
}

__inline MMPM_STATUS apiReleaseSingleResource(uint32 clientId, AdsppmRscIdType adsppmRscId, uint32 extData, MmpmApiType apiType, uint32* asyncRscMap)
{
	MMPM_STATUS sts = (MMPM_STATUS)RM_ReleaseRequest(clientId, adsppmRscId, extData);
	switch (sts)
	{
	case MMPM_STATUS_SUCCESS:
		sts = apiProcessSingleRequest(adsppmRscId, apiType, asyncRscMap);
		break;
	case MMPM_STATUS_RESOURCENOTREQUESTED:
		sts = MMPM_STATUS_SUCCESS;
		break;
	default:
		break;
	}
	return sts;
}

/*======================================================================*/
/* MMPM_Register_Ext */
/*============================================================================*/
uint32 MMPM_Register_Ext(MmpmRegParamType  *pRegParam)
{
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;

    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
	if(!gDISABLEADSPPMREQUESTFLAG)
	{
    /*enforce initialization of NULL pointers */
    if (ADSPPM_IsInitialized())
    {
        /* check parameters */
        if (NULL == pRegParam)
        {
            sts = MMPM_STATUS_NULLPOINTER;
            
        }
        else
        {
            if ((MMPM_CORE_ID_LPASS_START >= pRegParam->coreId) ||
                    (MMPM_CORE_ID_LPASS_END <= pRegParam->coreId) ||
                    (NULL == pRegParam->pClientName) ||
                    (MAX_ADSPPM_CLIENT_NAME <= strlen(pRegParam->pClientName)))
            {
                sts = MMPM_STATUS_BADPARM;
				ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "bad parameters or nullclientname or clientname is too long, coreId = %d", pRegParam->coreId);
                                        
            }
            else
            {
                AdsppmClientTypeType clientType = ((pRegParam->pwrCtrlFlag & PWR_CTRL_PERIODIC_CLIENT) == PWR_CTRL_PERIODIC_CLIENT)?
                        Adsppm_ClientType_Periodic:Adsppm_ClientType_Normal;

                //keep MMPM core ID and ADSPPM core id enum definition identical
                clientId = RM_CreateClient((AdsppmCoreIdType)(pRegParam->coreId - MMPM_CORE_ID_LPASS_START), pRegParam->pClientName, (void*)pRegParam->MMPM_Callback, clientType);
            }
        }/* pRegParam null? */
    }
    else
    {
        /*MMPM_Init is not successful*/
        sts = MMPM_STATUS_NOTINITIALIZED;
		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "MMPM not initialized"); 
    }/* initialized? */
	}
	else
	{
		return 1;
	}

    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return clientId;
}


/*======================================================================*/
/* MMPM_Deregister_Ext */
/*============================================================================*/
MMPM_STATUS     MMPM_Deregister_Ext(uint32          clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_NOTINITIALIZED;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
	if(!gDISABLEADSPPMREQUESTFLAG)
	{
		if (ADSPPM_IsInitialized())
		{
			if(clientId)
			{
				sts = (MMPM_STATUS)RM_DeleteClient(clientId);
				if(Adsppm_Status_Success == sts)
				{
					if(!gDISABLEADSPPMREQUESTFLAG)
					{
					AdsppmRscIdType rsc;
					for (rsc = Adsppm_Rsc_Id_Power; rsc < Adsppm_Rsc_Id_Max; rsc++)
					{
						sts = (MMPM_STATUS)RM_RequestResource(rsc);
						if(Adsppm_Status_Success != sts)
						{
							break;
						}
					}
					}
				}
			}
			else
			{
				sts = MMPM_STATUS_BADPARM;
				ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "bad clientid in deregister = %d", clientId);

			}
		}
	}
	else
	{ 
		sts = (MMPM_STATUS)Adsppm_Status_Success;
	}
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

MMPM_STATUS apiProcessBwRequest(uint32 clientId,  MmpmGenBwReqType *pGenBwReq,	MmpmApiType apiType, uint32* pAsyncRscMap)
{
	MMPM_STATUS sts= MMPM_STATUS_SUCCESS;
	AdsppmBwReqType adsppmBwReqData;
	AdsppmBusBWDataUsageType adsppmDspBwReqData;
	uint32 bwNum, BwNumNonDsp;

	ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: BW request ",
			clientId);
	//CHECK LIMITATION OF BW array = 8;
	if (MAX_NUM_OF_ADSPPM_BW_ARRAY
			< pGenBwReq->numOfBw)
	{
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
				"Number of BW entries (%d) is too big",
				pGenBwReq->numOfBw);
		sts = MMPM_STATUS_BADPARM;
	}
	else
	{
		memset(&adsppmBwReqData, 0, sizeof(AdsppmBwReqType));
		//CHECK PORT id and translate port ID to adsppm
		for (bwNum = 0, BwNumNonDsp = 0; bwNum < pGenBwReq->numOfBw; bwNum++)
		{
			if ((MMPM_BW_PORT_ID_NONE
					== pGenBwReq->pBandWidthArray[bwNum].busRoute.masterPort)
					|| (MMPM_BW_PORT_ID_EXT_AHB_MASTER
							< pGenBwReq->pBandWidthArray[bwNum].busRoute.masterPort)
					|| (MMPM_BW_PORT_ID_DML_SLAVE
							> pGenBwReq->pBandWidthArray[bwNum].busRoute.slavePort)
					|| (MMPM_BW_PORT_ID_SPDIF_SLAVE
							< pGenBwReq->pBandWidthArray[bwNum].busRoute.slavePort)
					|| (MMPM_BW_USAGE_LPASS_NONE
							== pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.usageType)
					|| (MMPM_BW_USAGE_LPASS_ENUM_MAX
							<= pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.usageType))
			{
				sts = MMPM_STATUS_BADPARM;
				ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_ERROR,
						"BW route or usageType is invalid: master %d, slave %d usageType = %d ",
						pGenBwReq->pBandWidthArray[bwNum].busRoute.masterPort,
						pGenBwReq->pBandWidthArray[bwNum].busRoute.slavePort,
						pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.usageType);
				break;
			}
			else
			{
				// Separate DSP vs non-DSP BW based on the MIPS routes
				if (isMipsRelatedBw(
						mmpmToAdsppmBusPort[pGenBwReq->pBandWidthArray[bwNum].busRoute.masterPort],
						mmpmToAdsppmBusPort[pGenBwReq->pBandWidthArray[bwNum].busRoute.slavePort]))
				{
					//DSP BW to be passed as ext parameter to MIPS manager

					adsppmDspBwReqData.usageType =
							(AdsppmBwUsageLpassType) pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.usageType;
					adsppmDspBwReqData.bwBytePerSec =
							pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.bwBytePerSec;
					adsppmDspBwReqData.usagePercentage =
							pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.usagePercentage;

					ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
							"    DSP BW: usage=%d, BW = %d at %d%% ",
							adsppmDspBwReqData.usageType,
							(uint32 )adsppmDspBwReqData.bwBytePerSec,
							adsppmDspBwReqData.usagePercentage);
					sts = (MMPM_STATUS)RM_AddRequest(clientId,
							Adsppm_Rsc_Id_Mips, &adsppmDspBwReqData, 1);
					if (MMPM_STATUS_SUCCESS == sts)
					{
						sts = apiProcessSingleRequest(Adsppm_Rsc_Id_Mips,
										apiType, pAsyncRscMap);
					}
				}
				else
				{
					// non-DSP BW to be passed to Bus Manager
					adsppmBwReqData.pBwArray[BwNumNonDsp].busRoute.masterPort =
							mmpmToAdsppmBusPort[pGenBwReq->pBandWidthArray[bwNum].busRoute.masterPort];
					adsppmBwReqData.pBwArray[BwNumNonDsp].busRoute.slavePort =
							mmpmToAdsppmBusPort[pGenBwReq->pBandWidthArray[bwNum].busRoute.slavePort];
					adsppmBwReqData.pBwArray[BwNumNonDsp].bwValue.busBwValue.usageType =
							(AdsppmBwUsageLpassType) pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.usageType;
					adsppmBwReqData.pBwArray[BwNumNonDsp].bwValue.busBwValue.bwBytePerSec =
							pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.bwBytePerSec;
					adsppmBwReqData.pBwArray[BwNumNonDsp].bwValue.busBwValue.usagePercentage =
							pGenBwReq->pBandWidthArray[bwNum].bwValue.busBwValue.usagePercentage;
					adsppmBwReqData.pBwArray[BwNumNonDsp].bwOperation =	AdsppmBusBWOperation_BW;
					ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
							"  Master=%d, Slave= %d, Operation %d :",
							adsppmBwReqData.pBwArray[BwNumNonDsp].busRoute.masterPort,
							adsppmBwReqData.pBwArray[BwNumNonDsp].busRoute.slavePort,
							adsppmBwReqData.pBwArray[BwNumNonDsp].bwOperation);
					ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
							"    usage=%d, BW = %d at %d%% ",
							adsppmBwReqData.pBwArray[BwNumNonDsp].bwValue.busBwValue.usageType,
							(uint32 )adsppmBwReqData.pBwArray[BwNumNonDsp].bwValue.busBwValue.bwBytePerSec,
							adsppmBwReqData.pBwArray[BwNumNonDsp].bwValue.busBwValue.usagePercentage);
					ADSPPM_QDSS_EVENT_5(ADSPPM_API_BW_REQ, clientId,
							adsppmBwReqData.pBwArray[bwNum].busRoute.masterPort,
							adsppmBwReqData.pBwArray[BwNumNonDsp].busRoute.slavePort,
							(uint32 )adsppmBwReqData.pBwArray[BwNumNonDsp].bwValue.busBwValue.bwBytePerSec,
							adsppmBwReqData.pBwArray[BwNumNonDsp].bwValue.busBwValue.usagePercentage);
					BwNumNonDsp++;
				}
			}
		}
		adsppmBwReqData.numOfBw = BwNumNonDsp;

		if ((MMPM_STATUS_SUCCESS == sts) && (pGenBwReq->numOfBw == BwNumNonDsp))
		{
			// Remove DSP vote requirement
			sts = apiReleaseSingleResource(clientId, Adsppm_Rsc_Id_Mips, 1, apiType, pAsyncRscMap);
		}

		if ((MMPM_STATUS_SUCCESS == sts) && (0 < BwNumNonDsp))
		{
			sts = (MMPM_STATUS)RM_AddRequest(clientId,
					Adsppm_Rsc_Id_BW, &adsppmBwReqData, 0);
		}
	}
	return sts;
}

/*============================================================================*/
/* MMPM_Request_Ext */
/*============================================================================*/
MMPM_STATUS     MMPM_Request_Ext(uint32 clientId, 
        MmpmRscExtParamType *pRscExtParam)
{
    MMPM_STATUS sts = MMPM_STATUS_NOTINITIALIZED;
    AdsppmRscIdType adsppmRscId = Adsppm_Rsc_Id_None;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
	if(!gDISABLEADSPPMREQUESTFLAG)
	{
    if (ADSPPM_IsInitialized())
    {
        if ((NULL == pRscExtParam) || (0 == clientId))
        {
            sts = MMPM_STATUS_BADPARM;
			ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "null pointer of pRscExtParam or clientid is 0, clientid=%d ", clientId);
           
        }
        else
        {
            if ((NULL == pRscExtParam->pReqArray) ||
                    (NULL == pRscExtParam->pStsArray))
            {
                sts = MMPM_STATUS_NULLPOINTER;
            }
            else
            {
			  if(Adsppm_Rsc_Id_Max > pRscExtParam->numOfReq)
			  {
                uint32 index = 0;
                uint32 asyncRscMap = 0;

                //init return sts array
                for (index=0; index< pRscExtParam->numOfReq; index++)
                {
                    pRscExtParam->pStsArray[index] = MMPM_STATUS_SUCCESS;
                }

                sts = MMPM_STATUS_SUCCESS;

                for (index=0; index< pRscExtParam->numOfReq; index++)
                {
                    switch (pRscExtParam->pReqArray[index].rscId)
                    {
                    case MMPM_RSC_ID_POWER:
                    {
                        uint32 powerOn = 1;
                        adsppmRscId = Adsppm_Rsc_Id_Power;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: Power request ", clientId);
                        pRscExtParam->pStsArray[index] = (MMPM_STATUS)RM_AddRequest(clientId, Adsppm_Rsc_Id_Power, &powerOn, 0);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_API_PWR_REQ, clientId);
                        
                    }
                    break;
                    case MMPM_RSC_ID_REG_PROG:
                    {
                        //covert to bw request
                        AdsppmBusBWRequestValueType adsppmBwReqData;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d:Register programming request ", clientId);
                        adsppmRscId = Adsppm_Rsc_Id_BW;
                        memset(&adsppmBwReqData, 0, sizeof(AdsppmBusBWRequestValueType));
                        adsppmBwReqData.busRoute.masterPort = AdsppmBusPort_Adsp_Master;
                        adsppmBwReqData.busRoute.slavePort = RM_GetCoreSlavePort(clientId);
						if((AdsppmBusPort_Dml_Slave > adsppmBwReqData.busRoute.slavePort) ||( AdsppmBusPort_EnumMax <= adsppmBwReqData.busRoute.slavePort))
						{
							sts = MMPM_STATUS_BADPARM;
							ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Reg Prog:bad param slaveport = %d",
                               adsppmBwReqData.busRoute.slavePort);
						}
						else
						{
							adsppmBwReqData.bwValue.busBwValue.bwBytePerSec = ADSPPM_REG_PROC_CLOCK_BW;
							adsppmBwReqData.bwValue.busBwValue.usagePercentage = ADSPPM_REG_PROC_CLOCK_BW_PERCENTAGE;
							adsppmBwReqData.bwValue.busBwValue.usageType = Adsppm_BwUsage_DSP;
							adsppmBwReqData.bwOperation = AdsppmBusBWOperation_RegAccess;
							ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "Reg Prog:Master=%d, Slave = %d, BW = %d ",
									adsppmBwReqData.busRoute.masterPort,
									adsppmBwReqData.busRoute.slavePort,
									(uint32)adsppmBwReqData.bwValue.busBwValue.bwBytePerSec);
							pRscExtParam->pStsArray[index] = (MMPM_STATUS)RM_AddRequest(clientId, Adsppm_Rsc_Id_BW, &adsppmBwReqData, 1);
							ADSPPM_QDSS_EVENT_3(ADSPPM_API_REG_REQ, clientId, adsppmBwReqData.busRoute.masterPort, adsppmBwReqData.busRoute.slavePort);
						}
                    }
                    break;
                    case MMPM_RSC_ID_CORE_CLK:
                    {
                        AdsppmClkRequestType adsppmClkReqData;
                        uint32 clkNum = 0;
                        adsppmRscId = Adsppm_Rsc_Id_Core_Clk;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d:Clock request:", clientId);
                        //CHECK NUM OF CLOCKS array, LIMITED TO 4
                        if(MAX_NUM_OF_ADSPPM_CLOCK_ARRAY < pRscExtParam->pReqArray[index].rscParam.pCoreClk->numOfClk)
                        {
                            pRscExtParam->pStsArray[index] = MMPM_STATUS_BADPARM;
							ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "numbofclk exceed max numofclk=%d ", pRscExtParam->pReqArray[index].rscParam.pCoreClk->numOfClk);
                        }
                        else
                        {
                            memset(&adsppmClkReqData, 0, sizeof(AdsppmClkRequestType));
                            adsppmClkReqData.numOfClk = pRscExtParam->pReqArray[index].rscParam.pCoreClk->numOfClk;
                            //translate clock Id to ADSPPM
                            //And copy all the request values
                            for(clkNum = 0; clkNum < pRscExtParam->pReqArray[index].rscParam.pCoreClk->numOfClk; clkNum++)
                            {
                                if((MMPM_CLK_ID_LPASS_NONE == pRscExtParam->pReqArray[index].rscParam.pCoreClk->pClkArray[clkNum].clkId.clkIdLpass)
                                        ||(MMPM_CLK_ID_LPASS_ENUM_MAX <= pRscExtParam->pReqArray[index].rscParam.pCoreClk->pClkArray[clkNum].clkId.clkIdLpass)
										|| (MMPM_FREQ_MAX <=  pRscExtParam->pReqArray[index].rscParam.pCoreClk->pClkArray[clkNum].freqMatch))
                                {
                                    pRscExtParam->pStsArray[index] = MMPM_STATUS_BADPARM;
									ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "bad lpassclkid =%d or freqmatch =%d ", pRscExtParam->pReqArray[index].rscParam.pCoreClk->pClkArray[clkNum].clkId.clkIdLpass, pRscExtParam->pReqArray[index].rscParam.pCoreClk->pClkArray[clkNum].freqMatch);
                                    break;
                                }
                                else
                                {
                                    adsppmClkReqData.pClkArray[clkNum].clkId = mmpmToAdsppmClk[pRscExtParam->pReqArray[index].rscParam.pCoreClk->pClkArray[clkNum].clkId.clkIdLpass];
                                    adsppmClkReqData.pClkArray[clkNum].clkFreqHz = pRscExtParam->pReqArray[index].rscParam.pCoreClk->pClkArray[clkNum].clkFreqHz;
                                    adsppmClkReqData.pClkArray[clkNum].freqMatch = pRscExtParam->pReqArray[index].rscParam.pCoreClk->pClkArray[clkNum].freqMatch;

                                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "  clkId=%d, freqHz = %d, match = %d ",
                                            adsppmClkReqData.pClkArray[clkNum].clkId,
                                            adsppmClkReqData.pClkArray[clkNum].clkFreqHz,
                                            adsppmClkReqData.pClkArray[clkNum].freqMatch);
                                }
                            }
                            if( MMPM_STATUS_SUCCESS == pRscExtParam->pStsArray[index])
                            {
                                pRscExtParam->pStsArray[index] = (MMPM_STATUS)RM_AddRequest(clientId, Adsppm_Rsc_Id_Core_Clk, &adsppmClkReqData, 0);
                            }
                        }
                    }
                    break;
                    case MMPM_RSC_ID_SLEEP_LATENCY:
                    {
                        uint32 sleepMicroSec =  pRscExtParam->pReqArray[index].rscParam.sleepMicroSec;
                        adsppmRscId = Adsppm_Rsc_Id_Sleep_Latency;
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "Client %d:Latency request: latency =%d ", clientId, sleepMicroSec);
                        pRscExtParam->pStsArray[index] = (MMPM_STATUS)RM_AddRequest(clientId, Adsppm_Rsc_Id_Sleep_Latency, &sleepMicroSec, 0);
                        ADSPPM_QDSS_EVENT_2(ADSPPM_API_LATENCY_REQ, clientId, sleepMicroSec);
                    }
                    break;
                    case MMPM_RSC_ID_MIPS_EXT:
                    {
                        AdsppmMipsRequestType adsppmMipsReqData;
						adsppmRscId = Adsppm_Rsc_Id_Mips;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d requesting MIPS:", clientId);
						if((MMPM_BW_PORT_ID_MAX <= pRscExtParam->pReqArray[index].rscParam.pMipsExt->codeLocation)
						|| ( MMPM_MIPS_REQUEST_NONE == pRscExtParam->pReqArray[index].rscParam.pMipsExt->reqOperation) 
						|| ( MMPM_MIPS_REQUEST_ENUM_MAX <= pRscExtParam->pReqArray[index].rscParam.pMipsExt->reqOperation))
						{
							 pRscExtParam->pStsArray[index] = MMPM_STATUS_BADPARM;
							 ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "bad parameters in codelocation or reqoperation:codelocation=%d, reqoperation=%d ", pRscExtParam->pReqArray[index].rscParam.pMipsExt->codeLocation, pRscExtParam->pReqArray[index].rscParam.pMipsExt->reqOperation );
						}
						else
						{
							AdsppmHandleType *pClientHandle = RM_GetClientHandle(clientId);
							memset(&adsppmMipsReqData,0, sizeof(AdsppmMipsRequestType));
							//make sure MMPM MIPS request data structure is the same as ADSPPM mips request data structure
							memscpy(&adsppmMipsReqData, sizeof(AdsppmMipsRequestType), pRscExtParam->pReqArray[index].rscParam.pMipsExt, sizeof(MmpmMipsReqType));
							ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO, "  MIPS request: total=%d, MIPS Floor = %d, location = %d, operation =%d ",
															adsppmMipsReqData.mipsTotal,
															adsppmMipsReqData.mipsPerThread,
															adsppmMipsReqData.codeLocation,
															adsppmMipsReqData.reqOperation);
							if (pClientHandle != NULL)
							{
								// Set client type in the request
								adsppmMipsReqData.clientType = pClientHandle->clientType;
							}

							pRscExtParam->pStsArray[index] = (MMPM_STATUS)RM_AddRequest(clientId, Adsppm_Rsc_Id_Mips, &adsppmMipsReqData, 0);
							ADSPPM_QDSS_EVENT_5(ADSPPM_API_MIPS_REQ, clientId, adsppmMipsReqData.mipsTotal, adsppmMipsReqData.mipsPerThread, adsppmMipsReqData.codeLocation, adsppmMipsReqData.reqOperation);
						}
						
                    }
                    break;
                    case MMPM_RSC_ID_GENERIC_BW:

						adsppmRscId = Adsppm_Rsc_Id_BW;
                    	pRscExtParam->pStsArray[index] = apiProcessBwRequest(clientId,
                    			pRscExtParam->pReqArray[index].rscParam.pGenBwReq, pRscExtParam->apiType, &asyncRscMap);

                    	break;
                    case MMPM_RSC_ID_THERMAL:
                    {
                        //CHECK AND TRANSLATE THERMAL LEVEL TO ADSPPM
                        AdsppmThermalType adsppmThermalLevel =  (AdsppmThermalType)pRscExtParam->pReqArray[index].rscParam.thermalMitigation;
                        adsppmRscId = Adsppm_Rsc_Id_Thermal;

                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "Client %d:Thermal request: level = %d ", clientId, adsppmThermalLevel);
                        pRscExtParam->pStsArray[index] = (MMPM_STATUS)RM_AddRequest(clientId, Adsppm_Rsc_Id_Thermal, &adsppmThermalLevel, 0);
                    }
                    break;

                    case MMPM_RSC_ID_MEM_POWER:
                    {
                        AdsppmMemPowerReqParamType adsppmMemPwrReqData;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: Memory power request ", clientId);
                        adsppmRscId = Adsppm_Rsc_Id_MemPower;
						if(( MMPM_MEM_NONE == pRscExtParam->pReqArray[index].rscParam.pMemPowerState->memory)
						|| (MMPM_MEM_MAX <=  pRscExtParam->pReqArray[index].rscParam.pMemPowerState->memory)
						|| ( MMPM_MEM_POWER_NONE == pRscExtParam->pReqArray[index].rscParam.pMemPowerState->powerState)
						|| ( MMPM_MEM_POWER_MAX <= pRscExtParam->pReqArray[index].rscParam.pMemPowerState->powerState))
						{
							pRscExtParam->pStsArray[index] = MMPM_STATUS_BADPARM;
							ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "bad memoryid = %d or bad powerstate = %d ", pRscExtParam->pReqArray[index].rscParam.pMemPowerState->memory, 
												pRscExtParam->pReqArray[index].rscParam.pMemPowerState->powerState);
						}
						else
						{
							memset(&adsppmMemPwrReqData, 0, sizeof(AdsppmMemPowerReqParamType));
							//make sure mmpm ocmem power request parameter is the same structure with adsppm
							memscpy(&adsppmMemPwrReqData, sizeof(AdsppmMemPowerReqParamType), pRscExtParam->pReqArray[index].rscParam.pMemPowerState, sizeof(AdsppmMemPowerReqParamType));
							pRscExtParam->pStsArray[index] = (MMPM_STATUS)RM_AddRequest(clientId, Adsppm_Rsc_Id_MemPower, &adsppmMemPwrReqData, 0);
							ADSPPM_QDSS_EVENT_3(ADSPPM_API_MEM_PWR_REQ, clientId, adsppmMemPwrReqData.memory, adsppmMemPwrReqData.powerState);
						}
                    }
                    break;
					case MMPM_RSC_ID_CORE_CLK_DOMAIN:
					{
                        AdsppmClkDomainReqType adsppmClkDomainReqData;
                        uint32 clkNum = 0;
                        adsppmRscId = Adsppm_Rsc_Id_Core_Clk_Domain;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d:Clock Domain request:", clientId);
                        //CHECK NUM OF CLOCKS array, LIMITED TO 4
                        if(MAX_NUM_OF_ADSPPM_CLOCK_ARRAY < pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->numOfClk)
                        {
                            pRscExtParam->pStsArray[index] = MMPM_STATUS_BADPARM;
							ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "numbofclk in clkdomain exceed max numofclk=%d ", pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->numOfClk);
                        }
                        else
                        {
                            memset(&adsppmClkDomainReqData, 0, sizeof(AdsppmClkDomainReqType));
                            adsppmClkDomainReqData.numOfClk = pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->numOfClk;
                            //translate clock Id to ADSPPM
                            //And copy all the request values
                            for(clkNum = 0; clkNum < pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->numOfClk; clkNum++)
                            {
                                if((MMPM_CLK_ID_LPASS_NONE == pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->pClkDomainArray[clkNum].clkId.clkIdLpass)
                                        ||(MMPM_CLK_ID_LPASS_ENUM_MAX <= pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->pClkDomainArray[clkNum].clkId.clkIdLpass)
										||(MMPM_CLK_DOMAIN_SRC_ID_LPASS_NONE ==  pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->pClkDomainArray[clkNum].clkDomainSrc.clkDomainSrcIdLpass)
										|| (MMPM_CLK_DOMAIN_SRC_ID_LPASS_MAX <=  pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->pClkDomainArray[clkNum].clkDomainSrc.clkDomainSrcIdLpass))
                                {
                                    pRscExtParam->pStsArray[index] = MMPM_STATUS_BADPARM;
									ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "bad lpassclkid =%d or clkdomainsource =%d ", pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->pClkDomainArray[clkNum].clkId.clkIdLpass, pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->pClkDomainArray[clkNum].clkDomainSrc.clkDomainSrcIdLpass);
                                    break;
                                }
                                else
                                {
                                    adsppmClkDomainReqData.pClkDomainArray[clkNum].clkId = mmpmToAdsppmClk[pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->pClkDomainArray[clkNum].clkId.clkIdLpass];                      
									adsppmClkDomainReqData.pClkDomainArray[clkNum].clkDomainSrc.clkDomainSrcIdLpass = (AdsppmClkDomainSrcIdLpassType)pRscExtParam->pReqArray[index].rscParam.pCoreClkDomain->pClkDomainArray[clkNum].clkDomainSrc.clkDomainSrcIdLpass;

                                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "  clkId=%d,  clkfreq = %d clockdomainsrc= %d ",
                                           adsppmClkDomainReqData.pClkDomainArray[clkNum].clkId, adsppmClkDomainReqData.pClkDomainArray[clkNum].clkFreqHz, 
										   adsppmClkDomainReqData.pClkDomainArray[clkNum].clkDomainSrc.clkDomainSrcIdLpass
                                         );
								 
                                }
                            }
                            if( MMPM_STATUS_SUCCESS == pRscExtParam->pStsArray[index])
                            {
                                pRscExtParam->pStsArray[index] = (MMPM_STATUS)RM_AddRequest(clientId, Adsppm_Rsc_Id_Core_Clk_Domain, &adsppmClkDomainReqData, 0);
                            }
                        }
                    }
					break;
                    default:
                    	pRscExtParam->pStsArray[index] = MMPM_STATUS_UNSUPPORTED;
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Client %d: Unsupported resource %d requested ", clientId, pRscExtParam->pReqArray[index].rscId);
                        break;
                    }
                    if(MMPM_STATUS_SUCCESS == pRscExtParam->pStsArray[index])
                    {
                    	pRscExtParam->pStsArray[index] = apiProcessSingleRequest(adsppmRscId, pRscExtParam->apiType, &asyncRscMap);
                    }
                }

                if( MMPM_API_TYPE_ASYNC == pRscExtParam->apiType)
                {
                    sts = (MMPM_STATUS)AM_AsyncRequest(clientId, asyncRscMap,  pRscExtParam->reqTag);
                }

                //update return sts
                for (index=0; index< pRscExtParam->numOfReq; index++)
                {
                    if (Adsppm_Status_Success != pRscExtParam->pStsArray[index])
                    {
                        sts = MMPM_STATUS_FAILED;
						ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_ERROR, "Request for rscId %d failed (%d) for client %d",
								pRscExtParam->pReqArray[index].rscId, pRscExtParam->pStsArray[index], clientId);
                    }
                }
			  }
			  else
			  {
				sts = MMPM_STATUS_BADPARM;
				ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Invalid number of requests in bundle %d ", pRscExtParam->numOfReq);
			  }
            }   //pRscExtParam->pReqArray; pRscExtParam->pStsArray NULL pointer
        }   //pRscExtParam NULL pointer
    }
	}
	else
	{
		sts = MMPM_STATUS_SUCCESS;
	}
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

/*============================================================================*/
/* MMPM_Release_Ext */
/*============================================================================*/
MMPM_STATUS     MMPM_Release_Ext(uint32 clientId, 
        MmpmRscExtParamType *pRscExtParam)
{
    MMPM_STATUS sts = MMPM_STATUS_NOTINITIALIZED;
    AdsppmRscIdType adsppmRscId = Adsppm_Rsc_Id_None;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
	if(!gDISABLEADSPPMREQUESTFLAG)
	{
    if (ADSPPM_IsInitialized())
    {
        if ((NULL == pRscExtParam) || (0 == clientId))
        {
            sts = MMPM_STATUS_BADPARM;
			ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "NULL pointer for pRscExtParam or clientID=0, clientid= %d ", clientId);                                              
          
        }
        else
        {
            if ((NULL == pRscExtParam->pReqArray) ||
                    (NULL == pRscExtParam->pStsArray))
            {
                sts = MMPM_STATUS_NULLPOINTER;
            }
            else
            {
			  if(Adsppm_Rsc_Id_Max > pRscExtParam->numOfReq)
			  {
                uint32 index = 0;
                uint32 asyncRscMap = 0;
              
                //init return sts array
                for (index=0; index< pRscExtParam->numOfReq; index++)
                {
                    pRscExtParam->pStsArray[index] = MMPM_STATUS_SUCCESS;
                }

                sts = MMPM_STATUS_SUCCESS;

                for (index=0; index< pRscExtParam->numOfReq; index++)
                {
                    uint32 extData = 0;
                    
                    switch (pRscExtParam->pReqArray[index].rscId)
                    {
                    case MMPM_RSC_ID_POWER:
                        adsppmRscId = Adsppm_Rsc_Id_Power;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: power release", clientId);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_API_PWR_REL, clientId);
                        break;
                    case MMPM_RSC_ID_REG_PROG:
                        adsppmRscId = Adsppm_Rsc_Id_BW;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: regprog release", clientId);
                         ADSPPM_QDSS_EVENT_1(ADSPPM_API_REG_REL, clientId);
                        extData = 1;
                        break;
                    case MMPM_RSC_ID_CORE_CLK:
                        adsppmRscId = Adsppm_Rsc_Id_Core_Clk;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: coreclk release", clientId);
                        break;
                    case MMPM_RSC_ID_SLEEP_LATENCY:
                        adsppmRscId = Adsppm_Rsc_Id_Sleep_Latency;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: sleeplatency release", clientId);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_API_LATENCY_REL, clientId);
                        break;
                    case MMPM_RSC_ID_MIPS_EXT:
                        adsppmRscId = Adsppm_Rsc_Id_Mips;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: mipsext release", clientId);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_API_MIPS_REL, clientId);
                        break;
                    case MMPM_RSC_ID_GENERIC_BW:
                        adsppmRscId = Adsppm_Rsc_Id_BW;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: genBW release", clientId);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_API_BW_REL, clientId);
                        // Remove DSP vote requirement as well
                        sts = apiReleaseSingleResource(clientId, Adsppm_Rsc_Id_Mips, 1, pRscExtParam->apiType, &asyncRscMap);
                        break;
                    case MMPM_RSC_ID_THERMAL:
                        adsppmRscId = Adsppm_Rsc_Id_Thermal;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: thermal release", clientId);
                        break;
                    case MMPM_RSC_ID_MEM_POWER:
                        adsppmRscId = Adsppm_Rsc_Id_MemPower;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: mempower release", clientId);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_API_MEM_PWR_REL, clientId);
                        break;
					case MMPM_RSC_ID_CORE_CLK_DOMAIN:
						adsppmRscId = Adsppm_Rsc_Id_Core_Clk_Domain;
						ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Client %d: clkdomain release", clientId);
                        break;
                    default:
                        sts = MMPM_STATUS_UNSUPPORTED;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Unsupported rscId = %d",
                                                    pRscExtParam->pReqArray[index].rscId);
                        break;
                    }
					ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT, "Releasing rscId = %d for clientId %d ",
                            adsppmRscId,
                            clientId);
					pRscExtParam->pStsArray[index] = apiReleaseSingleResource(clientId,	adsppmRscId, extData, pRscExtParam->apiType, &asyncRscMap);
                }

                if( MMPM_API_TYPE_ASYNC == pRscExtParam->apiType)
                {
                    sts = (MMPM_STATUS)AM_AsyncRequest(clientId, asyncRscMap,  pRscExtParam->reqTag);
                }

                //update return sts
                for (index=0; index< pRscExtParam->numOfReq; index++)
                {
                    if (MMPM_STATUS_SUCCESS != pRscExtParam->pStsArray[index])
                    {
                        sts = MMPM_STATUS_FAILED;
						ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_ERROR, "Release for rscId %d failed (%d) for client %d",
								pRscExtParam->pReqArray[index].rscId, pRscExtParam->pStsArray[index], clientId);
                    }
                }
			  }
			  else
			  {
				sts = MMPM_STATUS_BADPARM;
				ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Invalid number of requests in bundle %d ", pRscExtParam->numOfReq);
			  }
            } // pRscExtParam->pReqArray, pRscExtParam->pStsArray NULL pointer
        }  // pRscExtParam NULL pointer
    }
	}
	else
	{
		sts = MMPM_STATUS_SUCCESS;
	}
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

/*============================================================================*/
/* MMPM_GetInfo */
/*============================================================================*/
MMPM_STATUS     MMPM_GetInfo(MmpmInfoDataType   *pInfoData )
{
    MMPM_STATUS sts = MMPM_STATUS_NOTINITIALIZED;
    AdsppmCoreIdType coreId;
    uint32 clientId = 0;
    uint32     callBackFunc;
	AdsppmInfoClkFreqType clkInfoFreq;
	AdsppmClientTypeType clientType;
    uint32      mipsValue;
    uint64      bwValue;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
	if(!gDISABLEADSPPMREQUESTFLAG)
	{
    //TODO: getinfo is different with MMPM, need to discuss
    //put unsupported
    if (ADSPPM_IsInitialized())
    {
        switch(pInfoData->infoId)
        {
        case MMPM_INFO_ID_CLK_FREQ:
		case MMPM_INFO_ID_CORE_CLK_MAX:
		 if((MMPM_CLK_ID_LPASS_NONE == pInfoData->info.pInfoClkFreqType->clkId)
            ||(MMPM_CLK_ID_LPASS_ENUM_MAX <= pInfoData->info.pInfoClkFreqType->clkId))
             {
                sts = MMPM_STATUS_BADPARM;
             }
			 else
			 {
			    clkInfoFreq.clkId = mmpmToAdsppmClk[pInfoData->info.pInfoClkFreqType->clkId];
				clkInfoFreq.forceMeasure = pInfoData->info.pInfoClkFreqType->forceMeasure;
				clkInfoFreq.clkFreqHz = 0;
				if( MMPM_INFO_ID_CLK_FREQ == pInfoData->infoId)
				{
					sts = (MMPM_STATUS)RM_GetResourceInfo(Adsppm_Rsc_Id_Core_Clk, (void *)&clkInfoFreq, Adsppm_Info_Type_Current_Value);
				}
				else
				{
					sts = (MMPM_STATUS)RM_GetResourceInfo(Adsppm_Rsc_Id_Core_Clk, (void *)&clkInfoFreq, Adsppm_Info_Type_Max_Value);
				}
				if(MMPM_STATUS_SUCCESS == sts)
				{
					pInfoData->info.pInfoClkFreqType->clkFreqHz = clkInfoFreq.clkFreqHz;
				}
			 }
             break;
		
        case MMPM_INFO_ID_MIPS_MAX:
                sts = (MMPM_STATUS)RM_GetResourceInfo(Adsppm_Rsc_Id_Mips, (void *)&mipsValue, Adsppm_Info_Type_Max_Value);
			    if(MMPM_STATUS_SUCCESS == sts)
				{
					pInfoData->info.mipsValue= mipsValue;
				}
            break;
        case MMPM_INFO_ID_BW_MAX:
                sts = (MMPM_STATUS)RM_GetResourceInfo(Adsppm_Rsc_Id_BW, (void *)&bwValue, Adsppm_Info_Type_Max_Value);
				if(MMPM_STATUS_SUCCESS == sts)
				{
					pInfoData->info.bwValue= bwValue;
				}
            break;
        default:
             sts =(MMPM_STATUS)RM_GetClientInfo(clientId, &coreId, &callBackFunc, &clientType);
             break;
        }
    }
	}
	else
	{
		sts = MMPM_STATUS_SUCCESS;
	}
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}


/*======================================================================*/
/* MMPM_SetParameter */
/*============================================================================*/
MMPM_STATUS     MMPM_SetParameter(uint32 clientId, MmpmParameterConfigType *pParamConfigData)
{
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
	if(!gDISABLEADSPPMREQUESTFLAG)
	{
    if (ADSPPM_IsInitialized())
    {
        if ((NULL == pParamConfigData) || (0 == clientId))
        {
            sts = MMPM_STATUS_BADPARM;
			ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "NULL pointer for pParamConfigData or clientid is 0, clientId= %d ", clientId);       
        }
        else
        {
            AdsppmParameterConfigType paramConfig;
			MmpmMemoryMapType *pMemoryMap;
            switch(pParamConfigData->paramId)
            {
            case MMPM_PARAM_ID_RESOURCE_LIMIT:
                paramConfig.paramId = Adsppm_Param_Id_Resource_Limit;
                break;

            case MMPM_PARAM_ID_MEMORY_MAP:
                    paramConfig.paramId = Adsppm_Param_Id_Memory_Map;
                    paramConfig.pParamConfig = pParamConfigData->pParamConfig;
                    pMemoryMap = (MmpmMemoryMapType*) pParamConfigData->pParamConfig;
                    if(NULL != pMemoryMap)
                    {
					    sts = (MMPM_STATUS)ACM_SetVirtMemAddressRange((AdsppmMemIdType)pMemoryMap->memory, pMemoryMap->startAddress, pMemoryMap->size);
                        ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO, "Setting memory map for clientId %u, memory %u: addr = 0x%X, size = %u, sts = %d",
                                clientId, pMemoryMap->memory, (uint32)pMemoryMap->startAddress, pMemoryMap->size, sts);
                    }
                    else
                    {
                        sts = MMPM_STATUS_BADPARM;
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Parameter pointer is NULL");
                    }
                    break;
            default:
                sts = MMPM_STATUS_UNSUPPORTED;
                break;
            }
        }
    }
	}
	else
	{
		sts = MMPM_STATUS_SUCCESS;
	}
	
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}



/*======================================================================*/
/* MMPM_Init */
/*============================================================================*/
uint32 MMPM_Init(char * cmd_line)
{
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
	if(!gDISABLEADSPPMREQUESTFLAG)
	{
    /* Do initialization only once. This cannot be protected using critical section 
    since this is done before initializing the lock for critical section.*/
        if (!ADSPPM_IsInitialized())
        {
            sts = (MMPM_STATUS)ADSPPM_Init();
        }
	}
	else
	{
		sts = MMPM_STATUS_SUCCESS;
	}
    return (uint32)sts;
}

/** for corebsp image init
 */
void MMPM_Init_Ext(void)
{
    MMPM_Init(NULL);
}


/*======================================================================*/
/* MMPM_Deinit */
/*============================================================================*/
/* Deinitialize core MMPM modules.  */
void MMPM_Deinit(void)
{
  
    return; 
}


/**
 * Async API completion callback notification
 */
uint32 API_doClientCallback(AsyncMgrPipeDataType* receiveData)
{
    uint32 result =0;
    AdsppmCallbackParamType callbackParam;
    MmpmCompletionCallbackDataType callbackData;
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
    AdsppmHandleType *pLHandleItem = NULL;

    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;


    memset(&callbackParam, 0, sizeof(AdsppmCallbackParamType));

    callbackData.reqTag = receiveData->reqTag;
    callbackData.result = receiveData->result;

    callbackParam.eventId = Adsppm_Callback_Event_Id_Async_Complete;
    callbackParam.clientId = receiveData->clientId;
    callbackParam.callbackDataSize = sizeof(callbackData);
    callbackParam.callbackData = &(callbackData);
    pLHandleItem = coreUtils_Q_LinearSearch(&pAdsppmCtx->AdsppmHandleQ, (coreUtils_Q_CompareFuncType)findClientHandle, (void *)&(callbackParam.clientId));
	if(NULL != pLHandleItem)
    {
        if(0 != pLHandleItem->ADSPPM_Callback)
        {
        //callback clents call back function
        result = pLHandleItem->ADSPPM_Callback(&callbackParam);
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}

/**
 * Initialize all components within ADSPPM API
 * @return completion status @see Adsppm_Status
 */
Adsppm_Status API_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;

    gAdsppmApiCtx.gNumMipsRequest =  ACMBus_GetNumberOfMipsBwRoutes();
    gAdsppmApiCtx.pMipsRouteArray = ACMBus_GetMipsBwRoutes();

    result = AM_AsyncInit();
    if (Adsppm_Status_Success == result)
    {
        gADSPPMINITSTATE |= Adsppm_State_CoreAMAsyncInit;
    }

    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}
