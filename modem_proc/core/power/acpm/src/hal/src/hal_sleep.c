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
//#include "stdio.h"
#include "stdlib.h"
#include "hal_sleep.h"
#include "adsppm_utils.h"
#include "sleep_lpr.h"

const char SLEEP_LATENCY_NODE_NAME[] = "/core/cpu/latency/usec";
const char ADSPPM_SLEEP_CLIENT_NAME[] = "/core/power/adsppm";
const char CORE_CPU_BUSY_NODE_NAME[] = "/core/cpu/busy";

typedef struct{
    npa_client_handle handle;
    char clientName[MAX_ADSPPM_CLIENT_NAME];
}clientHandleType;

typedef struct 
{
    boolean           slpInitSuccess;
    DALSYSSyncHandle  slpdrvCtxLock;
    clientHandleType  slpRequestHandle;
    clientHandleType  cpuBusyHandle;
}gSlpInfoType;

static gSlpInfoType gSlpInfo;
static void SlpHandleInitCb( void         *context,
        unsigned int  event_type,
        void         *data,
        unsigned int  data_size );
static void CpuBusyHandleInitCb( void  *context,
        unsigned int  event_type,
        void         *data,
        unsigned int  data_size );


AdsppmStatusType Slp_Init( void (*sleep_enter_func)(uint32), void (*sleep_exit_func)(void))//enter and exit function currently unused
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    gSlpInfo.slpInitSuccess = FALSE;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&gSlpInfo.slpdrvCtxLock, NULL ))
    {
        sts = Adsppm_Status_Failed;
    }
    else
    {
        strlcpy(gSlpInfo.slpRequestHandle.clientName, "adsppmsleep", MAX_ADSPPM_CLIENT_NAME);
        npa_resource_available_cb(SLEEP_LATENCY_NODE_NAME, SlpHandleInitCb, NULL);

        strlcpy(gSlpInfo.cpuBusyHandle.clientName, "adsppm_core_cpu_busy", MAX_ADSPPM_CLIENT_NAME);
        npa_resource_available_cb(CORE_CPU_BUSY_NODE_NAME, CpuBusyHandleInitCb, NULL);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);

    return sts;
}

static void SlpHandleInitCb( void         *context,
        unsigned int  event_type,
        void         *data,
        unsigned int  data_size )
{
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    gSlpInfo.slpRequestHandle.handle = npa_create_sync_client( SLEEP_LATENCY_NODE_NAME,
            gSlpInfo.slpRequestHandle.clientName,
            NPA_CLIENT_REQUIRED);
    if (NULL != gSlpInfo.slpRequestHandle.handle)
    {
        gSlpInfo.slpInitSuccess = TRUE;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(gSlpInfo.slpInitSuccess);

}

static void CpuBusyHandleInitCb( void  *context,
        unsigned int  event_type,
        void         *data,
        unsigned int  data_size )
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    gSlpInfo.cpuBusyHandle.handle = npa_create_sync_client( CORE_CPU_BUSY_NODE_NAME,
                                                            gSlpInfo.cpuBusyHandle.clientName,
                                                            NPA_CLIENT_REQUIRED);
    if (NULL == gSlpInfo.slpRequestHandle.handle)
    {
        sts = Adsppm_Status_Failed;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
}

AdsppmStatusType Slp_RequestLatency(uint32 latency)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if (FALSE == gSlpInfo.slpInitSuccess)
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        adsppm_lock(gSlpInfo.slpdrvCtxLock);
        if ( HAL_SLEEP_LATENCY_DONT_CARE == latency)
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Complete NPA request for %s\n",SLEEP_LATENCY_NODE_NAME);
            npa_complete_request(gSlpInfo.slpRequestHandle.handle);

	    //relinquish \core\cpu\busy vote
	    if(NULL != gSlpInfo.cpuBusyHandle.handle)
	    {
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Complete NPA request for %s\n",CORE_CPU_BUSY_NODE_NAME);
		npa_complete_request(gSlpInfo.cpuBusyHandle.handle);
	    }
        }
        else
        {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "NPA request for %s, latency: %d\n",SLEEP_LATENCY_NODE_NAME, latency);
            npa_issue_required_request(gSlpInfo.slpRequestHandle.handle, latency);

	    //issue core\cpu\busy vote "1'
	    if(NULL != gSlpInfo.cpuBusyHandle.handle)
	    {
	    
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "NPA request for %s, state: %d\n",CORE_CPU_BUSY_NODE_NAME, 1);
                npa_issue_required_request(gSlpInfo.cpuBusyHandle.handle, 1);
	    }
	    
        }
        adsppm_unlock(gSlpInfo.slpdrvCtxLock);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}


AdsppmStatusType Slp_Release()
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if (FALSE == gSlpInfo.slpInitSuccess)
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        adsppm_lock(gSlpInfo.slpdrvCtxLock);
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Complete NPA request for %s\n",SLEEP_LATENCY_NODE_NAME);
        npa_complete_request(gSlpInfo.slpRequestHandle.handle);

	if(NULL != gSlpInfo.cpuBusyHandle.handle)
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Complete NPA request for %s\n",CORE_CPU_BUSY_NODE_NAME);
            npa_complete_request(gSlpInfo.cpuBusyHandle.handle);
	}
        adsppm_unlock(gSlpInfo.slpdrvCtxLock);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);

    return sts;
}
