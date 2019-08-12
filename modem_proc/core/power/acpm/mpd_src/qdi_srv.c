/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/**
 * @file qdi_srv.c - implementation of the QDI driver in the guest OS
 *
 *  Created on: Jun 11, 2012
 *      Author: yrusakov
 */

#include <limits.h>
#include "stdlib.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi_driver.h"
#include "qurt_qdi.h"
#include "adsppm_qdi.h"
#include "mmpm.h"
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "adsppmcb.h"
#include "asyncmgr.h"

#define ADSPPM_SRV_ULOG_BUFFER_SIZE 8972

// Macro to align an address to the nearest 8-byte boundary
#define ADSPPM_ALIGN8(val) (((val) + 7) & ~0x00000007)


QDI_Adsppm_Ctx_t gAdsppm;
int debugMPDAdsppm = 0;


void print_request_data(int client_handle, MmpmRscExtParamType* pUserReq, MmpmRscExtParamType* pReqData)
{
    int i, j;
    if((NULL != pUserReq)&&(NULL != gAdsppm.hLog))
    {
        ULOG_RT_PRINTF_4(gAdsppm.hLog, "%s: client=%d,userapitype=%d,numofReq=%d", __FUNCTION__, client_handle, pUserReq->apiType, pUserReq->numOfReq);
        ULOG_RT_PRINTF_3(gAdsppm.hLog, "%s: copyapitype=%d,numofReq=%d", __FUNCTION__, pReqData->apiType, pReqData->numOfReq);
        if(NULL != pUserReq->pReqArray)
        {    
                for(i = 0; i < pUserReq->numOfReq; i++)
                {
                    ULOG_RT_PRINTF_2(gAdsppm.hLog, "%s: userrscId=%d", __FUNCTION__, pUserReq->pReqArray[i].rscId);
                    ULOG_RT_PRINTF_2(gAdsppm.hLog, "%s: copyrscId=%d", __FUNCTION__, pReqData->pReqArray[i].rscId);
                    switch (pUserReq->pReqArray[i].rscId)
                    {
                    case MMPM_RSC_ID_MIPS_EXT:
                        if(NULL != pUserReq->pReqArray[i].rscParam.pMipsExt)
                        {
                            ULOG_RT_PRINTF_5(gAdsppm.hLog, "%s: usermipsT=%d,mipsP=%d, CL=%d,RO=%d", __FUNCTION__,  
                                   pUserReq->pReqArray[i].rscParam.pMipsExt->mipsTotal,
                                   pUserReq->pReqArray[i].rscParam.pMipsExt->mipsPerThread,
                                   pUserReq->pReqArray[i].rscParam.pMipsExt->codeLocation,
                                   pUserReq->pReqArray[i].rscParam.pMipsExt->reqOperation
                                   );
                            ULOG_RT_PRINTF_5(gAdsppm.hLog, "%s: copymipsT=%d,mipsP=%d, CL=%d,RO=%d", __FUNCTION__,  
                                   pReqData->pReqArray[i].rscParam.pMipsExt->mipsTotal,
                                   pReqData->pReqArray[i].rscParam.pMipsExt->mipsPerThread,
                                   pReqData->pReqArray[i].rscParam.pMipsExt->codeLocation,
                                   pReqData->pReqArray[i].rscParam.pMipsExt->reqOperation
                                   );
                        }
                        break;
                    case MMPM_RSC_ID_CORE_CLK:
                        if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk &&
                                NULL != pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray)
                        {
                            ULOG_RT_PRINTF_2(gAdsppm.hLog, "%s: usernumClk=%d", __FUNCTION__, pUserReq->pReqArray[i].rscParam.pCoreClk->numOfClk);
                            ULOG_RT_PRINTF_2(gAdsppm.hLog, "%s: copynumClk=%d", __FUNCTION__, pReqData->pReqArray[i].rscParam.pCoreClk->numOfClk);
                            for(j=0; j<pUserReq->pReqArray[i].rscParam.pCoreClk->numOfClk; j++)
                                   {
                                      ULOG_RT_PRINTF_4(gAdsppm.hLog, "%s: userclkId=%d,clkfreq=%d, freqmatch=%d", __FUNCTION__, 
                                         pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray[j].clkId,
                                         pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray[j].clkFreqHz,
                                         pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray[j].freqMatch
                                         );
                                      ULOG_RT_PRINTF_4(gAdsppm.hLog, "%s: copyclkId=%d,clkfreq=%d, freqmatch=%d", __FUNCTION__, 
                                         pReqData->pReqArray[i].rscParam.pCoreClk->pClkArray[j].clkId,
                                         pReqData->pReqArray[i].rscParam.pCoreClk->pClkArray[j].clkFreqHz,
                                         pReqData->pReqArray[i].rscParam.pCoreClk->pClkArray[j].freqMatch
                                         );
                                   }
                        }
                        break;
                    case MMPM_RSC_ID_GENERIC_BW:
                        if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq &&
                                NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray)
                        {
                            ULOG_RT_PRINTF_2(gAdsppm.hLog, "%s: userreqnumBw=%d", __FUNCTION__,  pUserReq->pReqArray[i].rscParam.pGenBwReq->numOfBw);
                            ULOG_RT_PRINTF_2(gAdsppm.hLog, "%s: copyreqnumBw=%d", __FUNCTION__,  pReqData->pReqArray[i].rscParam.pGenBwReq->numOfBw);
                            for(j=0; j< pUserReq->pReqArray[i].rscParam.pGenBwReq->numOfBw; j++)
                                {
                                     ULOG_RT_PRINTF_8(gAdsppm.hLog, "%s: userreqmaster=%d, slave=%d, bwps=%d, usageP=%d, usageT=%d,Ab=%d,Ib=%d", __FUNCTION__,  
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].busRoute.masterPort,
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].busRoute.slavePort,
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwValue.bwBytePerSec,
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usagePercentage,
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usageType,
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwAbIb.Ab,
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwAbIb.Ib
                                        );
                                      ULOG_RT_PRINTF_8(gAdsppm.hLog, "%s: userreqmaster=%d, slave=%d, bwps=%d, usageP=%d, usageT=%d,Ab=%d,Ib=%d", __FUNCTION__,  
                                        pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].busRoute.masterPort,
                                        pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].busRoute.slavePort,
                                        pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwValue.bwBytePerSec,
                                        pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usagePercentage,
                                        pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usageType,
                                        pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwAbIb.Ab,
                                        pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray[j].bwValue.busBwAbIb.Ib
                                        );
                                }
                        }
                        break;
					case MMPM_RSC_ID_MEM_POWER:
						if(NULL != pUserReq->pReqArray[i].rscParam.pMemPowerState)
                        {
                           ULOG_RT_PRINTF_3(gAdsppm.hLog, "%s: usermemT=%d, pwState=%d", __FUNCTION__,  pUserReq->pReqArray[i].rscParam.pMemPowerState->memory,
                                 pUserReq->pReqArray[i].rscParam.pMemPowerState->powerState);
                           ULOG_RT_PRINTF_3(gAdsppm.hLog, "%s: copymemT=%d, pwState=%d", __FUNCTION__,  pReqData->pReqArray[i].rscParam.pMemPowerState->memory,
                                 pReqData->pReqArray[i].rscParam.pMemPowerState->powerState);
                        }
						break;
                    default:
                        break;
                    }
                }
        }
     }
}




// 32-bit addition with overflow check.  The boolean flag pointed to by
// overflowFlag is set to true if overflow is detected; otherwise it is left
// unmodified.
static uint32 add_overflow_check(uint32 a, uint32 b, boolean* overflowFlag)
{
    uint32 result = a + b;
    if(((UINT_MAX - a) < b) && (overflowFlag != NULL))
        *overflowFlag = TRUE;
    return result;
}

// 32-bit multiplication with overflow check.  The boolean flag pointed to by
// overflowFlag is set to true if overflow is detected; otherwise it is left
// unmodified.
static uint32 mult_overflow_check(uint32 a, uint32 b, boolean* overflowFlag)
{
    uint32 result = a * b;
    if(((UINT_MAX / a) < b) && (overflowFlag != NULL))
        *overflowFlag = TRUE;
    return result;
}

// Alignment of an integer to the next multiple of 8, with overflow check.  The
// boolean flag pointed to by overflowFlag is set to true if overflow is
// detected; otherwise it is left unmodified.
static uint32 align8_overflow_check(uint32 a, boolean* overflowFlag)
{
    uint32 result = ADSPPM_ALIGN8(a);
    if(((UINT_MAX - 7) < a) && (overflowFlag != NULL))
        *overflowFlag = TRUE;
    return result;
}

// Calculate the size of the request while taking into account padding
// needed to maintain the memory address alignment for the various structures
// in the request.
static uint32 calculate_padded_request_size(MmpmRscExtParamType *pUserReq,
    boolean* overflowFlag)
{
    uint32 size = 0, i;

    if(NULL != pUserReq)
    {
        // Top level structure
        size = add_overflow_check(size,
            align8_overflow_check(sizeof(MmpmRscExtParamType), overflowFlag),
            overflowFlag);

        if(NULL != pUserReq->pReqArray)
        {
            // Request array
            size = add_overflow_check(size,
                align8_overflow_check(mult_overflow_check(pUserReq->numOfReq,
                sizeof(MmpmRscParamType), overflowFlag), overflowFlag),
                overflowFlag);
            // Loop through all requests
            for(i = 0; i < pUserReq->numOfReq; i++)
            {
                switch(pUserReq->pReqArray[i].rscId)
                {
                case MMPM_RSC_ID_MIPS_EXT:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmMipsReqType),
                        overflowFlag), overflowFlag);
                    break;

                case MMPM_RSC_ID_CORE_CLK:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmClkReqType),
                        overflowFlag), overflowFlag);
                    if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk)
                    {
                        if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray)
                        {
                            size = add_overflow_check(size,
                                align8_overflow_check(mult_overflow_check(
                                pUserReq->pReqArray[i].rscParam.pCoreClk->numOfClk,
                                sizeof(MmpmClkValType), overflowFlag),
                                overflowFlag), overflowFlag);
                        }
                    }
                    break;

                case MMPM_RSC_ID_GENERIC_BW:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmGenBwReqType),
                        overflowFlag), overflowFlag);
                    if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq)
                    {
                        if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray)
                        {
                            size = add_overflow_check(size,
                                align8_overflow_check(mult_overflow_check(
                                pUserReq->pReqArray[i].rscParam.pGenBwReq->numOfBw,
                                sizeof(MmpmGenBwValType), overflowFlag),
                                overflowFlag), overflowFlag);
                        }
                    }
                    break;

                case MMPM_RSC_ID_MEM_POWER:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmMemPowerReqParamType),
                        overflowFlag), overflowFlag);
                    break;

                default:
                    break;
                }
            }
        }
    }
    return size;
}


static int copy_request_data(
    int client_handle,
    MmpmRscExtParamType *pUserReq,
    MmpmRscExtParamType *pReqData)
{
    int i, result = 0;
    unsigned char *pData = (unsigned char *)pReqData;

    if(NULL != pUserReq)
    {
        // Top level structure
        result = qurt_qdi_copy_from_user(
            client_handle,
            (void *)pReqData,
            (void *)pUserReq,
            sizeof(MmpmRscExtParamType));
        pData += ADSPPM_ALIGN8(sizeof(MmpmRscExtParamType));

        if((result >= 0) && (NULL != pUserReq->pReqArray))
        {
            // Set pointer to the flat structure
            pReqData->pReqArray = (MmpmRscParamType *)pData;
            result = qurt_qdi_copy_from_user(
                client_handle,
                (void *)pReqData->pReqArray,
                (void *)pUserReq->pReqArray,
                pUserReq->numOfReq * sizeof(MmpmRscParamType));
            // Request structure array
            pData += ADSPPM_ALIGN8(pUserReq->numOfReq * sizeof(MmpmRscParamType));

            // Loop through all requests
            if(result >= 0)
            {
                for(i = 0; i < pUserReq->numOfReq; i++)
                {
                    switch(pUserReq->pReqArray[i].rscId)
                    {
                    case MMPM_RSC_ID_MIPS_EXT:
                        if(NULL != pUserReq->pReqArray[i].rscParam.pMipsExt)
                        {
                            pReqData->pReqArray[i].rscParam.pMipsExt = (MmpmMipsReqType *)pData;
                            result = qurt_qdi_copy_from_user(
                                client_handle,
                                (void *)(pReqData->pReqArray[i].rscParam.pMipsExt),
                                (void *)(pUserReq->pReqArray[i].rscParam.pMipsExt),
                                sizeof(MmpmMipsReqType));
                            pData += ADSPPM_ALIGN8(sizeof(MmpmMipsReqType));
                        }
                        break;
                    case MMPM_RSC_ID_CORE_CLK:
                        pReqData->pReqArray[i].rscParam.pCoreClk = (MmpmClkReqType *)pData;
                        if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk)
                        {
                            if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray)
                            {
                                result = qurt_qdi_copy_from_user(
                                    client_handle,
                                    (void *)(pReqData->pReqArray[i].rscParam.pCoreClk),
                                    (void *)(pUserReq->pReqArray[i].rscParam.pCoreClk),
                                    sizeof(MmpmClkReqType));
                                pData += ADSPPM_ALIGN8(sizeof(MmpmClkReqType));
                                pReqData->pReqArray[i].rscParam.pCoreClk->pClkArray = (MmpmClkValType *)pData;
                                if(result >= 0)
                                {
                                    result = qurt_qdi_copy_from_user(
                                        client_handle,
                                        (void *)(pReqData->pReqArray[i].rscParam.pCoreClk->pClkArray),
                                        (void *)(pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray),
                                        pUserReq->pReqArray[i].rscParam.pCoreClk->numOfClk * sizeof(MmpmClkValType));
                                    pData += ADSPPM_ALIGN8(
                                        pUserReq->pReqArray[i].rscParam.pCoreClk->numOfClk *
                                        sizeof(MmpmClkValType));
                                }
                            }
                        }
                        break;

                    case MMPM_RSC_ID_GENERIC_BW:
                        pReqData->pReqArray[i].rscParam.pGenBwReq = (MmpmGenBwReqType *)pData;
                        if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq)
                        {
                            if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray)
                            {
                                result = qurt_qdi_copy_from_user(
                                    client_handle,
                                    (void *)(pReqData->pReqArray[i].rscParam.pGenBwReq),
                                    (void *)(pUserReq->pReqArray[i].rscParam.pGenBwReq),
                                    sizeof(MmpmGenBwReqType));
                                pData += ADSPPM_ALIGN8(sizeof(MmpmGenBwReqType));
                                pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray = (MmpmGenBwValType *)pData;
                                if(result >= 0)
                                {
                                    result = qurt_qdi_copy_from_user(
                                        client_handle,
                                        (void *)(pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray),
                                        (void *)(pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray),
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->numOfBw * sizeof(MmpmGenBwValType));
                                    pData += ADSPPM_ALIGN8(
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->numOfBw *
                                        sizeof(MmpmGenBwValType));
                                }
                            }
                        }
                        break;

                    case MMPM_RSC_ID_MEM_POWER:
                        if(NULL != pUserReq->pReqArray[i].rscParam.pMemPowerState)
                        {
                            pReqData->pReqArray[i].rscParam.pMemPowerState = (MmpmMemPowerReqParamType *)pData;
                            result = qurt_qdi_copy_from_user(
                                client_handle,
                                (void *)(pReqData->pReqArray[i].rscParam.pMemPowerState),
                                (void *)(pUserReq->pReqArray[i].rscParam.pMemPowerState),
                                sizeof(MmpmMemPowerReqParamType));
                            pData += ADSPPM_ALIGN8(sizeof(MmpmMemPowerReqParamType));
                        }
                        break;

                    default:
                        break;
                    }

                    if(result < 0)
                    {
                        break;
                    }
                }
            }
        }
        if(debugMPDAdsppm)
        {
            print_request_data(client_handle, pUserReq, pReqData );
        }
    }
    return result;
}


static MmpmRscExtParamType *get_request_data_from_user(
    int client_handle,
    MmpmRscExtParamType *pUserReq)
{
    MmpmRscExtParamType *pData = NULL;
    boolean overflowFlag = FALSE;
    uint32 size = calculate_padded_request_size(pUserReq, &overflowFlag);
    if(overflowFlag)
    {
        ULOG_RT_PRINTF_0(gAdsppm.hLog,
            "detected integer overflow while handling request");
    }
    if((size > sizeof(MmpmRscExtParamType)) && !overflowFlag)
    {
        pData = malloc(size);
        if(NULL != pData)
        {
            if(0 > copy_request_data(client_handle, pUserReq, pData))
            {
                free(pData);
                pData = NULL;
            }
        }
    }
    return pData;
}

int copy_request_status_to_user(int client_handle,MmpmRscExtParamType* pReqData, MmpmRscExtParamType* pUserReq)
{
    int ret = -1;
    if (NULL != pReqData  &&  NULL != pUserReq && NULL != pReqData->pStsArray && NULL != pUserReq->pStsArray)
    {
        ret = qurt_qdi_copy_to_user(client_handle, (void *)pUserReq->pStsArray, (void *)pReqData->pStsArray, pReqData->numOfReq * sizeof(MMPM_STATUS));
    }

    return ret;
}


static int calculate_setparam_size(MmpmParameterConfigType *pParamConfigData)
{
    int size = 0;
    if(NULL != pParamConfigData)
    {
        // Top level structure
        size += ADSPPM_ALIGN8(sizeof(MmpmParameterConfigType));

        if(NULL != pParamConfigData->pParamConfig)
        {
            switch(pParamConfigData->paramId)
            {
            case MMPM_PARAM_ID_RESOURCE_LIMIT:
                break;
            case MMPM_PARAM_ID_MEMORY_MAP:
				size += sizeof(MmpmMemoryMapType);
				break;
            default:
                break;
            }
        }
    }
    return size;
}


static int copy_setparam_data(
    int client_handle,
    MmpmParameterConfigType *pUserParamData,
    MmpmParameterConfigType *pParamData)
{
    int result = 0;
    unsigned char *pData = (unsigned char *)pParamData;
    if(NULL != pUserParamData)
    {
        // Top level structure
        result = qurt_qdi_copy_from_user(
            client_handle,
            pData,
            (void *)pUserParamData,
            sizeof(MmpmParameterConfigType));
        pData += ADSPPM_ALIGN8(sizeof(MmpmParameterConfigType));

        if((result >= 0) && (NULL != pUserParamData->pParamConfig))
        {
            switch(pUserParamData->paramId)
            {
            case MMPM_PARAM_ID_RESOURCE_LIMIT:
                break;
			case MMPM_PARAM_ID_MEMORY_MAP:
			    pParamData->pParamConfig = (MmpmMemoryMapType *)pData;
			    result = qurt_qdi_copy_from_user(client_handle, pData, (void *)pUserParamData->pParamConfig, sizeof(MmpmMemoryMapType));
			    pData += sizeof(MmpmMemoryMapType);
				break;
            default:
                break;
            }
        }
    }
    return result;
}


MmpmParameterConfigType* get_setparam_data_from_user(int client_handle,MmpmParameterConfigType* pUserParam)
{
    MmpmParameterConfigType* pData = NULL;
    int size = calculate_setparam_size(pUserParam);

    if(size > sizeof(MmpmParameterConfigType))
    {
        pData = malloc(size);

        if(NULL != pData)
        {
            if(0 > copy_setparam_data(client_handle, pUserParam, pData))
            {
                free(pData);
                pData = NULL;
            }
        }
    }
    return pData;
}

void QDI_Adsppm_do_callback(QDI_Adsppm_cbinfo_t *ptr, MmpmCallbackParamType* pValue)
{
    QDI_Adsppm_CbqElem_t *cbq;

    MmpmCompletionCallbackDataType* pReturnCallbackValue = (MmpmCompletionCallbackDataType *)pValue->callbackData;

    cbq = malloc(sizeof(QDI_Adsppm_CbqElem_t));

    if (cbq) {
        cbq->cbinfo = ptr;
        cbq->callbackParam.eventId = pValue->eventId;
        cbq->callbackParam.clientId = pValue->clientId;
        cbq->callbackParam.callbackDataSize = pValue->callbackDataSize;
        cbq->callbackParam.callbackData = &cbq->callbackValue;
        cbq->callbackValue.reqTag = pReturnCallbackValue->reqTag;
        cbq->callbackValue.result = pReturnCallbackValue->result;
        qurt_rmutex_lock(&(ptr->object->mtx));
        cbq->next = ptr->object->cbqueue;
        ptr->object->cbqueue = cbq;
        qurt_rmutex_unlock(&(ptr->object->mtx));
        qurt_qdi_signal_set(ptr->object->hSigLocal);
    }
}

int QDI_Adsppm_get_callback(int client_handle,
        QDI_Adsppm_Ctx_t *pCbDrv,
        void *buf)
{
    Adsppm_cbinfo_Client_t info;
    QDI_Adsppm_CbqElem_t *cbq = NULL, *ptr = NULL, *prev = NULL;
    int result = 0;

    for (;;) {
        qurt_rmutex_lock(&pCbDrv->mtx);
        ptr = pCbDrv->cbqueue;
        while(ptr)
        {
            //remember non-NULL item
            prev = cbq;
            cbq = ptr;
            //get to the end of the list
            ptr = ptr->next;
        }
        if (cbq)
        {
            if(prev)
            {
                prev->next = NULL;
            }
            else
            {
                //Removing the only element from the queue
                pCbDrv->cbqueue = NULL;
            }
            qurt_rmutex_unlock(&pCbDrv->mtx);
            break;
        }
        else
        {
            qurt_rmutex_unlock(&pCbDrv->mtx);
        qurt_qdi_signal_wait(pCbDrv->hSigLocal);
        qurt_qdi_signal_clear(pCbDrv->hSigLocal);
        }
    }


    info.pfn = cbq->cbinfo->pfn;
    // Pointer to the data should be substituted on the client's side
    info.callbackParam.callbackDataSize = cbq->callbackParam.callbackDataSize;
    info.callbackParam.clientId = cbq->callbackParam.clientId;
    info.callbackParam.eventId = cbq->callbackParam.eventId;
    info.callbackValue.reqTag = cbq->callbackValue.reqTag;
    info.callbackValue.result = cbq->callbackValue.result;
    result = qurt_qdi_copy_to_user(client_handle, buf, &info, sizeof(info));
    if(NULL != cbq)
    {
        free(cbq);
    }
    return result; 
}


void QDI_Adsppm_Release (qurt_qdi_obj_t *qdiobj)
{
    Adsppmcb_Deinit_Srv();
    free(qdiobj);
}

int QDI_Adsppm_Invocation (int client_handle,
        qurt_qdi_obj_t *obj,
        int method,
        qurt_qdi_arg_t a1,
        qurt_qdi_arg_t a2,
        qurt_qdi_arg_t a3,
        qurt_qdi_arg_t a4,
        qurt_qdi_arg_t a5,
        qurt_qdi_arg_t a6,
        qurt_qdi_arg_t a7,
        qurt_qdi_arg_t a8,
        qurt_qdi_arg_t a9)
{
    QDI_Adsppm_cbinfo_t CbInfo;
    int result = 0;
    MmpmStatusType ret = MMPM_STATUS_SUCCESS;
    MmpmRegParamType *pRegParam = NULL;
    uint32 clientId = 0;
    MmpmRscExtParamType *pRscExtParam = NULL;
    MmpmInfoDataType   *pInfoData = NULL;
	MmpmParameterConfigType   *pSetParamData =NULL;


    gAdsppm.qdiobj = *obj;

    switch (method)
    {
    case QDI_OPEN:

        gAdsppm.qdiobj.invoke = QDI_Adsppm_Invocation;
        gAdsppm.qdiobj.refcnt = QDI_REFCNT_INIT;
        gAdsppm.qdiobj.release = QDI_Adsppm_Release;

        gAdsppm.cbqueue = NULL;
        qurt_rmutex_init(&gAdsppm.mtx);
        if (qurt_qdi_signal_group_create(client_handle,
                &gAdsppm.hGroupLocal,
                &gAdsppm.hGroupRemote) < 0)
        {
            result = -1;
        }
        if (qurt_qdi_signal_create(gAdsppm.hGroupLocal,
                &gAdsppm.hSigLocal,
                &gAdsppm.hSigRemote) < 0)
        {
            qurt_qdi_release_handle(client_handle, gAdsppm.hGroupRemote);
            qurt_qdi_release_handle(QDI_HANDLE_LOCAL_CLIENT, gAdsppm.hGroupLocal);
            result = -1;
        }
        result = qurt_qdi_new_handle_from_obj_t(client_handle, &gAdsppm.qdiobj);
        if (result < 0)
        {
            qurt_qdi_release_handle(client_handle, gAdsppm.hSigRemote);
            qurt_qdi_release_handle(client_handle, gAdsppm.hGroupRemote);
            qurt_qdi_release_handle(QDI_HANDLE_LOCAL_CLIENT, gAdsppm.hSigLocal);
            qurt_qdi_release_handle(QDI_HANDLE_LOCAL_CLIENT, gAdsppm.hGroupLocal);
        }
        break;
    case ADSPPM_REGISTER:
        pRegParam =  malloc(sizeof(MmpmRegParamType));
        if(NULL != pRegParam)
        {
            result = qurt_qdi_copy_from_user(client_handle, (void *)pRegParam, (void *)a1.ptr, sizeof(MmpmRegParamType));
            if(0 > result)
            {
                result = -1;
            }
            else
            {
                if( CALLBACK_NONE != pRegParam->callBackFlag)
                {
                        CbInfo.object = &gAdsppm;
                        CbInfo.pfn = pRegParam->MMPM_Callback; //call back function
                        pRegParam->MMPM_Callback = Adsppmcb_notify_callbacks_Srv;
                }
                clientId =  MMPM_Register_Ext(pRegParam);
                result = qurt_qdi_copy_to_user(client_handle, (void *)a2.ptr, (void *)&clientId, sizeof(clientId));
                if(clientId != 0)
                {
                    CbInfo.clientId = clientId;
                    Adsppmcb_register_callback_Srv(&CbInfo);
                }
                else
                {
                    result = -1;
                }
            }
            free(pRegParam);
        }
        else
        {
            result = -1;
        }

        break;
    case ADSPPM_REQUEST:
        pRscExtParam = get_request_data_from_user(client_handle, (MmpmRscExtParamType*) a2.ptr);
        if(NULL != pRscExtParam)
        {
            ret = MMPM_Request_Ext(a1.num, pRscExtParam);
            result = qurt_qdi_copy_to_user(client_handle, (void *)a3.ptr, (void *)&ret, sizeof(ret));
            if (result >=0)
            {
                //copy status array to user
                result = copy_request_status_to_user(client_handle, pRscExtParam, (MmpmRscExtParamType*) a2.ptr);
            }
            if( MMPM_STATUS_SUCCESS != ret)
            {
                result = -1;
            }
            free(pRscExtParam);
        }
        else
        {
            result = -1;
        }

        break;
    case ADSPPM_RELEASE:
        pRscExtParam = malloc(sizeof(MmpmRscExtParamType));
        if(NULL != pRscExtParam)
        {
            result = qurt_qdi_copy_from_user(client_handle, (void *)pRscExtParam, (void *)a2.ptr, sizeof(MmpmRscExtParamType));
            if(result < 0)
            {
                result = -1;
            }
            else
            {
                ret = MMPM_Release_Ext(a1.num, pRscExtParam);
                result = qurt_qdi_copy_to_user(client_handle, (void *)a3.ptr, (void *)&ret, sizeof(ret));
                if( MMPM_STATUS_SUCCESS != ret)
                {
                    result = -1;
                }
            }
            free(pRscExtParam);
        }
        else
        {
            result = -1;
        }

        break;
    case ADSPPM_DEREGISTER:
        Adsppmcb_deRegister_callback_Srv(a1.num);
        ret = MMPM_Deregister_Ext(a1.num);
        result = qurt_qdi_copy_to_user(client_handle, (void *)a2.ptr, (void *)&ret, sizeof(ret));
        if( MMPM_STATUS_SUCCESS != ret)
        {
            result = -1;
        }

        break;
    case ADSPPM_GET_INFO:
        pInfoData = malloc(sizeof(MmpmInfoDataType));
        if(NULL != pInfoData)
        {
            result = qurt_qdi_copy_from_user(client_handle, (void *)pInfoData, (void *)a1.ptr, sizeof(MmpmInfoDataType));
            if(result < 0)
            {
                result = -1;
            }
            else
            {
                ret = MMPM_GetInfo(pInfoData);
                result = qurt_qdi_copy_to_user(client_handle, (void *)a2.ptr, (void *)&ret, sizeof(ret));
                if(MMPM_STATUS_SUCCESS != ret)
                {
                    result = -1;
                }
                else
                {
                    result = qurt_qdi_copy_to_user(client_handle, (void *)a1.ptr, (void *)pInfoData, sizeof(MmpmInfoDataType));
                }
            }
            free(pInfoData);
        }
        else
        {
            result = -1;
        }

        break;

	  case ADSPPM_SET_PARAM:
        pSetParamData = get_setparam_data_from_user(client_handle, (MmpmParameterConfigType*) a2.ptr);
        if(NULL != pSetParamData)
        {
            ret = MMPM_SetParameter(a1.num, pSetParamData);
            result = qurt_qdi_copy_to_user(client_handle, (void *)a3.ptr, (void *)&ret, sizeof(ret));
            if( MMPM_STATUS_SUCCESS != ret)
            {
                result = -1;
            }
            free(pSetParamData);
        }
        else
        {
            result = -1;
        }

        break;
    case ADSPPM_GET_CB:
        result = QDI_Adsppm_get_callback(client_handle, &gAdsppm, a1.ptr);
        break;
    default:
        result = qurt_qdi_method_default(client_handle, obj, method,
                a1, a2, a3, a4, a5, a6, a7, a8, a9);
        break;
    }
    return result;
}

const QDI_Adsppm_Opener_t adsppm_opener = {
        {
                QDI_Adsppm_Invocation,
                QDI_REFCNT_PERM,
                0
        } // 0 as object is PERM, never released
};

/**
 * @fn QDI_adsppm_init - registers driver with QDI framework
 * This function is called by RCINIT after ADSPPM is initialized.
 */
void QDI_adsppm_init(void)
{
     int result = 0;
    gAdsppm.log_buffer_size = ADSPPM_SRV_ULOG_BUFFER_SIZE;
    result = ULogFront_RealTimeInit(&gAdsppm.hLog,
            "ADSPPM SRV Log",
            gAdsppm.log_buffer_size,
            ULOG_MEMORY_LOCAL,
            ULOG_LOCK_OS );

    Adsppmcb_Init_Server();
    qurt_qdi_devname_register(ADSPPM_QDI_DRV_NAME, (void *)&adsppm_opener);
}





