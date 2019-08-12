/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_sleep.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */
#include "npa.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm.h"
#include "asic.h"
#include "core.h"
#include "stdlib.h"
#include "hal_cpu_aggregator.h"
#include "adsppm_utils.h"
#include "coremca.h"

const char AGGREGATOR_NODE_NAME[] = "/core/mca";
const char ADSPPM_CA_CLIENT_NAME[] = "/core/power/adsppm";

#define MCA_Q6_CONVERT_FACTOR 1000
#define MCA_BW_CONVERT_FACTOR 1048576

#define NUM_MCA_REQUESTS 5

typedef struct
{
	// Example 'KVP' for MPPS request
	unsigned mpps_key; //MPPS_REQ_KEY
	unsigned mpps_length; //data length
	unsigned mpps_data; //actual data
	//Example 'KVP' for CPP request
	unsigned cpp_key; // CPP_REQ_KEY
	unsigned cpp_length; //data length
	unsigned cpp_data; //actual data
	//Example 'KVP' for Q6 Clock Peak Value
	unsigned q6_key; //Q6_CLK_PEAK_KEY
	unsigned q6_length; //data length
	unsigned q6_data; //actual data
	//Example 'KVP' for Ib value
	unsigned Ib_key; //Q6_CLK_PEAK_KEY
	unsigned Ib_length; //data length
	unsigned Ib_data; //actual data
	//Example 'KVP' for Ab vlue
	unsigned Ab_key; //Q6_CLK_PEAK_KEY
	unsigned Ab_length; //data length
	unsigned Ab_data; //actual data
}CpuAggregatorRequestVectorType;

typedef struct{
    npa_client_handle handle;
}clientHandleType;

typedef struct 
{
    boolean           caInitSuccess;
    DALSYSSyncHandle  cadrvCtxLock;
    clientHandleType  caRequestHandle;
    CpuAggregatorRequestVectorType caRequestData;
    AdsppmStatusType (*p_FnSetCpuPerformanceLevel)(AdsppmMipsAggregateDataType* pCpuData);
}gCAInfoType;

static gCAInfoType gCAInfo;
static void CAHandleInitCb( void         *context,
        unsigned int  event_type,
        void         *data,
        unsigned int  data_size );

AdsppmStatusType caRequestCpuPerformanceLevel_internal(AdsppmMipsAggregateDataType* pCpuData);
AdsppmStatusType caRequestCpuPerformanceLevel_NPA(AdsppmMipsAggregateDataType* pCpuData);

AdsppmStatusType CA_Init(void)//enter and exit function currently unused
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    gCAInfo.caInitSuccess = FALSE;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&gCAInfo.cadrvCtxLock, NULL ))
    {
        sts = Adsppm_Status_Failed;
    }
    else
    {
    	switch(ACM_GetCpuAggregatorType())
    	{
    	case AsicCpuAggregatorType_Npa:
    		//NPA version of the aggregator
    		gCAInfo.p_FnSetCpuPerformanceLevel = caRequestCpuPerformanceLevel_NPA;
    		npa_resource_available_cb(AGGREGATOR_NODE_NAME, CAHandleInitCb, NULL);
    		break;
    	case AsicCpuAggregatorType_Internal:
    	default:
    		//Aggregation and translation into clock/BW requests is handled internally
    		gCAInfo.p_FnSetCpuPerformanceLevel = caRequestCpuPerformanceLevel_internal;
    		gCAInfo.caInitSuccess = TRUE;
    		break;
    	}
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);

    return sts;
}

static void CAHandleInitCb( void         *context,
        unsigned int  event_type,
        void         *data,
        unsigned int  data_size )
{
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    gCAInfo.caRequestHandle.handle = npa_create_sync_client( AGGREGATOR_NODE_NAME,
    		ADSPPM_CA_CLIENT_NAME,
    		NPA_CLIENT_VECTOR);
    if (NULL != gCAInfo.caRequestHandle.handle)
    {
    	gCAInfo.caRequestData.cpp_key = CPP_REQ_KEY;
    	gCAInfo.caRequestData.cpp_length = sizeof(gCAInfo.caRequestData.cpp_data);

    	gCAInfo.caRequestData.mpps_key = MPPS_REQ_KEY;
    	gCAInfo.caRequestData.mpps_length = sizeof(gCAInfo.caRequestData.mpps_data);

    	gCAInfo.caRequestData.q6_key = Q6_CLK_REQ_KEY;
    	gCAInfo.caRequestData.q6_length = sizeof(gCAInfo.caRequestData.q6_data);
		
		gCAInfo.caRequestData.Ib_key = IB_REQ_KEY;
    	gCAInfo.caRequestData.Ib_length = sizeof(gCAInfo.caRequestData.Ib_data);
		
		gCAInfo.caRequestData.Ab_key = AB_REQ_KEY;
    	gCAInfo.caRequestData.Ab_length = sizeof(gCAInfo.caRequestData.Ab_data);

        gCAInfo.caInitSuccess = TRUE;
		
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(gCAInfo.caInitSuccess);

}


AdsppmStatusType caRequestCpuPerformanceLevel_NPA(AdsppmMipsAggregateDataType* pCpuData)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if (NULL != pCpuData)
    {
        if (NULL != gCAInfo.caRequestHandle.handle)
        {
            adsppm_lock(gCAInfo.cadrvCtxLock);

            gCAInfo.caRequestData.mpps_data = pCpuData->mipsAggregate.mipsTotal;
            gCAInfo.caRequestData.q6_data = ACM_GetHwThreadNumber() * pCpuData->mipsAggregate.mipsPerThread * MCA_Q6_CONVERT_FACTOR ; // Changing unit as per MCA
            gCAInfo.caRequestData.cpp_data = pCpuData->mipsAggregate.cppFactor; //Using CPP*1000 here
            gCAInfo.caRequestData.Ib_data = DIVIDE_ROUND_UP(pCpuData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ib, MCA_BW_CONVERT_FACTOR);
            gCAInfo.caRequestData.Ab_data = DIVIDE_ROUND_UP(pCpuData->mipsToBWAggregateDataDdr.mipsToBW.bwValue.busBwAbIb.Ab, MCA_BW_CONVERT_FACTOR);

            ADSPPM_LOG_PRINTF_6(ADSPPM_LOG_LEVEL_INFO, "NPA request for %s, MPPS: %d, Floor: %d, CPP: %d, Ib: %d, Ab: %d\n",AGGREGATOR_NODE_NAME,
                gCAInfo.caRequestData.mpps_data, gCAInfo.caRequestData.q6_data, gCAInfo.caRequestData.cpp_data,
                gCAInfo.caRequestData.Ib_data, gCAInfo.caRequestData.Ab_data);
                
            npa_issue_vector_request( gCAInfo.caRequestHandle.handle, NUM_MCA_REQUESTS,  (npa_resource_state*)&gCAInfo.caRequestData);
            adsppm_unlock(gCAInfo.cadrvCtxLock);
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "No client handle for %s node. Skipping request",AGGREGATOR_NODE_NAME);
        }
    }
    else
    {
    	sts = Adsppm_Status_BadParm;
    	ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
    			"pCpuData=NULL\n");
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}


AdsppmStatusType caRequestCpuPerformanceLevel_internal(AdsppmMipsAggregateDataType* pCpuData)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if (NULL != pCpuData)
    {
    	adsppm_lock(gCAInfo.cadrvCtxLock);

    	//Use ASIC specific function to set CPU performance level
        sts = ACM_SetDspPerformanceLevel(pCpuData);

    	adsppm_unlock(gCAInfo.cadrvCtxLock);
    }
    else
    {
    	sts = Adsppm_Status_BadParm;
    	ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
    			"pCpuData=NULL\n");
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

AdsppmStatusType CA_RequestCpuPerformanceLevel(AdsppmMipsAggregateDataType* pCpuData)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if (FALSE == gCAInfo.caInitSuccess)
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
    	//Call helper function to set DSP performance level
    	sts = gCAInfo.p_FnSetCpuPerformanceLevel(pCpuData);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}
