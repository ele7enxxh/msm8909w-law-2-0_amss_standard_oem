/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * asic_func_8916.c - Contains 8916 specific functions for the Config Manager
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */


#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "asic_internal.h"
#include "asic_8916.h"
#include "core.h"
#include "hal_clkrgm.h"
#include "hal_busdrv.h"



#define DSP_BWUSAGE_FACTOR_8916  256
#define DMA_BWUSAGE_FACTOR_8916  256
#define EXTCPU_BWUSAGE_FACTOR_8916 256
#define AVERAGE_BYTES_PER_PACKET 6
#define MAX_BYTES_PER_PACKET 16

#define INSTRUCTION_WIDTH_BITS_8916  32
#define BUS_WIDTH_BITS_8916  64
#define AHB_BUS_WIDTH_BITS_8916  32

Adsppm_Status aggregateMips_8916(AdsppmMipsAggregateDataType *pMipsAggregateData, AdsppmMipsRequestType *pMipsReqData, AdsppmBusBWDataUsageType* pMipsBW)
{
	Adsppm_Status result = Adsppm_Status_Success;
	uint64 bwAb = 0, bwIb = 0;
	uint32 footprint = 0;
    if ((pMipsReqData->mipsTotal > 0) || (pMipsReqData->mipsPerThread > 0))
	{
		if (Adsppm_ClientType_Periodic == pMipsReqData->clientType)
		{
			pMipsAggregateData->mipsAggregate.numOfPeriodicClients++;
		}

		if ((pMipsBW != NULL) && (pMipsBW->bwBytePerSec > 0) && (pMipsBW->usagePercentage > 0))
		{
			//consider requested BW here
			bwIb = pMipsBW->bwBytePerSec;
			bwAb = bwIb * pMipsBW->usagePercentage;
			bwAb /= 100;
		}
		else
		{
			//use default BW assumption
			//assuming for each packet we need to fetch 1.5 words on average
			//use default assumption only for clients requesting MIPS with BW and 0 for all other clients
			//(meaning  that if client doesn't provide BW vote for its MIPS vote then the BW vote is already covered by other client)
			if(pMipsReqData->reqOperation == MipsRequestOperation_BWandMIPS)
			{
				bwAb = pMipsReqData->mipsTotal * 1000000 * AVERAGE_BYTES_PER_PACKET;
				//TODO: move to table based approach for default BW  for Ib to reflect latency requirement
				bwIb = pMipsReqData->mipsTotal * 1000000 * MAX_BYTES_PER_PACKET;
			}
		}

		//Aggregate MPPS
		pMipsAggregateData->mipsAggregate.mipsTotal +=
				pMipsReqData->mipsTotal;
		pMipsAggregateData->mipsAggregate.mipsPerThread = MAX(
				pMipsAggregateData->mipsAggregate.mipsPerThread,
				pMipsReqData->mipsPerThread);
		pMipsAggregateData->mipsAggregate.numOfClients++;

		//Aggregate footprint factor
		footprint = bwAb/pMipsReqData->mipsTotal;
		pMipsAggregateData->mipsAggregate.footprintFactor += footprint/1000; // Bytes per packet * 1000

		//Aggregate DDR BW
		pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ib = MAX(
				bwIb,
				pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ib);
		pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ab += bwAb;

	}
	return result;
}

Adsppm_Status getBWFromMips_8916(AdsppmMipsAggregateDataType *pMipsAggregateData)
{
    Adsppm_Status result = Adsppm_Status_Success;
    if(NULL != pMipsAggregateData)
    {
        AdsppmBusBWRequestValueType *pBw = &pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW;
        uint32 maxMips = (pMipsAggregateData->mipsAggregate.mipsTotal * pMipsAggregateData->mipsAggregate.cppFactor)/1000; //Compensate for CPP unit

        uint64 bwValAb = 0, bwValIb = 0;
        //Translate MIPS into IPS
        maxMips *= 1000000;
        //Translate IPS into BW in bytes per second
        bwValIb = maxMips;// TODO: figure out if adjustment by AVERAGE_BYTES_PER_PACKET is necessary

        if((AdsppmBusPort_Ddr_Slave == pMipsAggregateData->mipsToBWAggregateDataDdr.mipsBWSlavePort) &&
                (AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Min_Adsp_BW_Vote)))
        {
            //Adjust BW based on feature config
            bwValIb = ACM_AdjustParamValue(AsicFeatureId_Min_Adsp_BW_Vote, bwValIb);
        }

        if((bwValIb > 0) && (AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_TimelineOptimisationBw))&&
                IsPeriodicUseCase())
        {
            //Only periodic clients are active
            //Select baseline or aggregated value if it is higher
            bwValIb = ACM_AdjustParamValue(AsicFeatureId_TimelineOptimisationBw, bwValIb);
        }

        // TODO: Figure out how to account for concurrencies in BW aggregation
        // Use pre-aggregated BW and compensate for latency/concurrencies here
        pBw->bwValue.busBwAbIb.Ab = pBw->bwValue.busBwAbIb.Ab + bwValAb;
        pBw->bwValue.busBwAbIb.Ib = MAX(pBw->bwValue.busBwAbIb.Ib, bwValIb);
    }
    else
    {
		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"pMipsAggregateData=NULL\n");

        result = Adsppm_Status_NullPointer;
    }

    return result;
}

Adsppm_Status getCppFromMips_8916(AdsppmMipsAggregateDataType *pMipsAggregateData)
{
	Adsppm_Status result = Adsppm_Status_Success;
	if(NULL != pMipsAggregateData)
	{
		uint32 footprint = pMipsAggregateData->mipsAggregate.footprintFactor;
		// Normalize footprint based on cache size
		footprint /= ACM_GetCacheSize();
		ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO_EXT, "Input: MPPS=%d, Footprint=%d, Aggregated Ab=%d, Ib=%d\n",
				pMipsAggregateData->mipsAggregate.mipsTotal, footprint,
				(uint32)pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ab,
				(uint32)pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ib);

		//Get CPP based on total MPPS
		pMipsAggregateData->mipsAggregate.cppFactor = ACM_GetCppforMpps(pMipsAggregateData->mipsAggregate.mipsTotal);

		//Adjust CPP based on footprint
		pMipsAggregateData->mipsAggregate.cppFactor *= ACM_GetCppFudgeFactor(footprint);
		//Compensate for CPP fudge factor being x*1000
		pMipsAggregateData->mipsAggregate.cppFactor /= 1000;

		//Calculate DDR BW for MIPS to ensure correct latency for CPP factor

		result = getBWFromMips_8916(pMipsAggregateData);
		ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "Output: Adjusted DDR BW Ab=%d, Ib=%d, CPP=%d\n",
				(uint32)pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ab,
				(uint32)pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ib,
				pMipsAggregateData->mipsAggregate.cppFactor);

	}
	else
	{
		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"pMipsAggregateData = NULL\n");
		result = Adsppm_Status_NullPointer;
	}
	return result;
}

uint32 getClockFromMips_8916(AdsppmMIPSAggregateType *pMipsAggregateData)
{
	uint32 result = 0;
    if(NULL != pMipsAggregateData)
    {
        uint32 maxMips = MAX((pMipsAggregateData->mipsTotal * pMipsAggregateData->cppFactor)/1000, //Compensate for CPP unit
        		ACM_GetHwThreadNumber() * pMipsAggregateData->mipsPerThread) ;
        //int i = 0;
        //uint32 factor = 256;
        //Check if only periodic clients are active
        if((AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_TimelineOptimisationMips)) && IsPeriodicUseCase())
        {
            //Only periodic clients are active
            //Select baseline or aggregated value if it is higher
            maxMips = ACM_AdjustParamValue(AsicFeatureId_TimelineOptimisationMips, maxMips);
        }

      //Adjust MIPS based on feature config
      maxMips = ACM_AdjustParamValue(AsicFeatureId_Adsp_Clock_Scaling, maxMips);
      //Translate MIPS into Hz
      result = maxMips * 1000000;
    }
    else
    {
		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"pMipsAggregateData = NULL\n");
    }
    return result;
}

Adsppm_Status getClockFromBW_8916(uint32 *pClock, AdsppmBusBWDataIbAbType *pAHBBwData)
{
    Adsppm_Status result = Adsppm_Status_Success;
    if((NULL != pClock) && (NULL != pAHBBwData))
    {
        uint64 maxBW = MAX(pAHBBwData->Ab, pAHBBwData->Ib);
        //Adjust BW based on feature config
        maxBW = ACM_AdjustParamValue(AsicFeatureId_Ahb_Scaling, maxBW);
        //translate into bits per second
        maxBW *= 8;
        //translate into clock in Hz by dividing by bus width
        *pClock = maxBW/AHB_BUS_WIDTH_BITS_8916;
    }
    else
    {
		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"pAHBBwData=NULL\n");

        result = Adsppm_Status_NullPointer;
    }
    return result;
}

/**
 * @fn aggregateBusBW - aggregate bw. So far the function is common for all chips
 * @param input: AdsppmBusBWDataType *pBwValue. BW is specified in BW, Usage %
 * @param output: AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue. BW output in Ab, Ib form
 * @return Adsppm_Status
 */

Adsppm_Status aggregateBusBW_8916(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue)
{
       Adsppm_Status sts = Adsppm_Status_Success;
       uint32 BwUsageFactor = 0;
       uint64 bwValue =  0;
	   
       AdsppmBusBWDataIbAbType bwValueAbIb;
       if((NULL == pAggregateBwIbAbValue)
          || (NULL == pBwValue))
       {
	     ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"pAggregateBwIbAbValue or pBwValue is NULL\n");
         sts = Adsppm_Status_BadParm;
       }
       else
       {
		 bwValue = pBwValue->busBwValue.bwBytePerSec;
         switch(pBwValue->busBwValue.usageType)
         {
         case Adsppm_BwUsage_DSP:
            BwUsageFactor = DSP_BWUSAGE_FACTOR_8916;
            break;
         case Adsppm_BwUsage_DMA:
            BwUsageFactor = DMA_BWUSAGE_FACTOR_8916;
            break;
         case Adsppm_BwUsage_EXT_CPU:
             BwUsageFactor = EXTCPU_BWUSAGE_FACTOR_8916;
            break;
         default:
			 {
				ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Invalid usageType=%d\n",pBwValue->busBwValue.usageType );
				sts = Adsppm_Status_BadParm;
			 }
            break;
         }
         //apply bus usage factor to bw
         bwValue = bwValue * BwUsageFactor;
         bwValue /= PERCENTS_IN_A_WHOLE;
         //translate bw to ab and ib
         bwValueAbIb.Ab = bwValue * pBwValue->busBwValue.usagePercentage;
         bwValueAbIb.Ab /= 100;
         bwValueAbIb.Ib = bwValue;
         //aggregate bw
         pAggregateBwIbAbValue->Ab += bwValueAbIb.Ab;
         pAggregateBwIbAbValue->Ib = MAX(pAggregateBwIbAbValue->Ib, bwValueAbIb.Ib);
       }
       return sts;
}

Adsppm_Status setDspPerformanceLevel_8916(AdsppmMipsAggregateDataType* pMipsAggregateData)
{
	Adsppm_Status sts = Adsppm_Status_Success;
	uint32 q6Clock = 0;
	//Calculate Q6 Clock
	q6Clock = getClockFromMips_8916(&(pMipsAggregateData->mipsAggregate));
	ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
			"MIPS total=%d, MIPS min=%d, Clock=%d\n",
			pMipsAggregateData->mipsAggregate.mipsTotal,
			pMipsAggregateData->mipsAggregate.mipsPerThread,
			q6Clock);

    //Issue BW request for MIPS
    if(Adsppm_Status_Success == sts)
    {
        AdsppmBusBWRequestValueType busBw[BUS_NUMBER_MIPS_ROUTES];
        busBw[0].bwValue =  pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.bwValue;
        busBw[0].busRoute.masterPort =  pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.busRoute.masterPort;
        busBw[0].busRoute.slavePort =  pMipsAggregateData->mipsToBWAggregateDataDdr.mipsToBW.busRoute.slavePort;
        sts = Bus_IssueBWRequest_MIPS(&busBw[0]);
    }

    // Set Q6 Clock
    if (0 != q6Clock)
    {
        sts = ClkRgm_SetClock(AdsppmClk_Adsp_Core, q6Clock);
    }
    //Adjust AHB clock  for periodic usecases if required
    if (Adsppm_Status_Success == sts)
    {
    	//TODO: Figure out if we need to control modem config bus
        sts = AHBM_AdjustAndSetAHBClock();
    }
    return sts;
}


