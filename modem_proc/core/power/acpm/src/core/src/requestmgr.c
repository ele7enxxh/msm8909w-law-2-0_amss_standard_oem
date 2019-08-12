/*
 *@file requestmgr.c - adsppm request manager implementation

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.

==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/power/acpm/src/core/src/requestmgr.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------------
07/20/11   alisonc      Initial version
===================================================================================*/

/**----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
//#include "AEEstd.h"
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "core.h"
#include "core_internal.h"
#include "coreUtils.h"
#include "requestmgr.h"
#include "clkmgr.h"
#include "mipsmgr.h"
#include "busmgr.h"
#include "pwrmgr.h"
#include "latencymgr.h"
#include "thermalmgr.h"
#include "stdlib.h"
#include "mempwrmgr.h"


/**
 * @fn findRMCtx - find the RMCtx in linklist
 */
int findRMCtx(RMCtxType *pLRMCtxItem,  uint32 *clientId)
{
    int match = 0;
    if(pLRMCtxItem->clientId == *clientId)
    {
        match = 1;
    }
    return match;
}



static void RM_FreeParam(AdsppmHandleType *pHandle);

/**
 * @fn getRequestDataSize - return size of request data structure per resource
 */
__inline uint32 getRequestDataSize(AdsppmRscIdType resourceId)
{
    uint32 result =0;
    switch (resourceId)
    {
    case Adsppm_Rsc_Id_Power:
        result = sizeof(uint32);
        break;
    case Adsppm_Rsc_Id_Core_Clk:
        result = sizeof(AdsppmClkRequestType);
        break;
    case Adsppm_Rsc_Id_BW:
        result =sizeof(AdsppmBwReqType);
        break;
    case Adsppm_Rsc_Id_Mips:
        result = sizeof(AdsppmMipsRequestType);
        break;
    case Adsppm_Rsc_Id_Sleep_Latency:
        result = sizeof(uint32);
        break;
    case Adsppm_Rsc_Id_Thermal:
        result = sizeof(uint32);
        break;
    case Adsppm_Rsc_Id_MemPower:
        result = sizeof(AdsppmMemPowerReqParamType);
        break;
    case Adsppm_Rsc_Id_Core_Clk_Domain:
        result = sizeof(AdsppmClkDomainReqType);
        break;
    default :
        break;
    }

    return result;
}

/**
 * @fn getRequestData_ExtSize - return size of extended request data structure per resource
 */
__inline uint32 getRequestData_ExtSize(AdsppmRscIdType resourceId)
{
    uint32 result =0;
    switch (resourceId)
    {
    case Adsppm_Rsc_Id_Power:
        break;
    case Adsppm_Rsc_Id_Core_Clk:
        break;
    case Adsppm_Rsc_Id_BW:
        result =sizeof(AdsppmBusBWRequestValueType);
        break;
    case Adsppm_Rsc_Id_Mips:
    	result = sizeof(AdsppmBusBWDataUsageType);
        break;
    case Adsppm_Rsc_Id_Sleep_Latency:
        break;
    case Adsppm_Rsc_Id_Thermal:
        break;
    case Adsppm_Rsc_Id_MemPower:
        break;
    case Adsppm_Rsc_Id_Core_Clk_Domain:
        break;
    default :
        break;
    }

    return result;
}

/**============================================================================

@fn RM_Init 
@Brief - init mutex for RM

@param input AdsppmCoreIdType coreId - core Id in LPASS
@param input uint32 *callback - call back function for this client.

@return uint32 clientId 

============================================================================*/

Adsppm_Status RM_Init(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
    AdsppmRscIdType   index;
    ADSPPM_LOG_FUNC_ENTER;

	coreUtils_Q_Init(&pAdsppmCtx->AdsppmHandleQ);
    for(index = Adsppm_Rsc_Id_Power; index < Adsppm_Rsc_Id_Max; index++)
    {
        coreUtils_Q_Init(&pAdsppmCtx->AdsppmReqQArray[index-1]);
        if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&pAdsppmCtx->RMCtxLock[index-1], NULL ))
        {
            sts = Adsppm_Status_Failed;
            break;
        }
        
    }
    ADSPPM_LOG_FUNC_EXIT(sts);

    return sts;
}
/**============================================================================

@fn RM_CreateClient 
@Brief - register a new client, save all informatin to client handle and return clientID.

@param input AdsppmCoreIdType coreId - core Id in LPASS
@param input name - client name
@param input uint32 *callback - call back function for this client.

@return uint32 clientId 

============================================================================*/
uint32 RM_CreateClient(AdsppmCoreIdType coreId, char* name, void *callback, AdsppmClientTypeType clientType)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
    AsicCoreDescType* pCoreDesc = ACM_GetCoreDescriptor(coreId);
    uint32 clientId = 0;
	AdsppmRscIdType resourceId = Adsppm_Rsc_Id_None;
	AdsppmHandleType *pLHandleItem = NULL;
	RMCtxType *pLRMCtxItem = NULL;
    ADSPPM_LOG_FUNC_ENTER;

    if ((Adsppm_Core_Id_None == coreId) ||
            (Adsppm_Core_Id_Max <= coreId) || (NULL == name) || (NULL == pCoreDesc))
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,"Invalid coreId =%d requested by client %s", coreId, name);
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "coreId=%d ",coreId);
        if (NULL == pAdsppmCtx->adsppmCtxLock)
        {
            sts = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
        else
        {
            AdsppmHandleType *pHandle = NULL;
			adsppm_lock(pAdsppmCtx->adsppmCtxLock);
            if (MAX_NUM_OF_ADSPPM_CLIENTS - 1 == pAdsppmCtx->numOfRegisteredClients)
            {
                /* no more room for clients */
                sts = Adsppm_Status_Failed;
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Registered clients exceed max=%d, clients=%d ", MAX_NUM_OF_ADSPPM_CLIENTS, pAdsppmCtx->numOfRegisteredClients );
				adsppm_unlock(pAdsppmCtx->adsppmCtxLock);

            }
            else
            {
				//allocate client handle and put into clienthandle queue
				pLHandleItem = malloc(sizeof(AdsppmHandleType));
				if(NULL == pLHandleItem)
                {
					sts = Adsppm_Status_Failed;
					ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Malloc failed for pLHandleItem ");

                }
				else
				{
						coreUtils_Q_Link(pLHandleItem, &(pLHandleItem->AdsppmHandleLink));
						coreUtils_Q_Put(&(pAdsppmCtx->AdsppmHandleQ), &(pLHandleItem->AdsppmHandleLink));	
						pHandle = pLHandleItem;
						pAdsppmCtx->clientIdNum++;
						clientId = pAdsppmCtx->clientIdNum;
                        pHandle->clientId = clientId;
                        pHandle->coreId = coreId;
                        pHandle->ADSPPM_Callback = callback;
                        strlcpy (pHandle->clientName, name, MAX_ADSPPM_CLIENT_NAME);
                        pHandle->pSetParamConfig = NULL;
                        pHandle->clientType = clientType;
                        ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "Client %s (type %d) registered with ID %d", pHandle->clientName, pHandle->clientType, clientId);
                        pAdsppmCtx->numOfRegisteredClients++;
                }
				if(Adsppm_Status_Success == sts)
				{
				//allocate RMCtx for each resource and put into resource RMCtx Queue
					for(resourceId = Adsppm_Rsc_Id_Power; resourceId < Adsppm_Rsc_Id_Max; resourceId++)
					{
						pLRMCtxItem = malloc(sizeof(RMCtxType));
						if (NULL == pLRMCtxItem)
						{
							sts = Adsppm_Status_Failed;
							ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Malloc failed for pLRMCtxItem ");
							break;
						}
						else
						{
							memset(pLRMCtxItem, 0, sizeof(RMCtxType));
							pLRMCtxItem->pRequestData = malloc(getRequestDataSize(resourceId));
							if( NULL == pLRMCtxItem->pRequestData)
							{
								sts = Adsppm_Status_Failed;
								ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Malloc failed for pLRMCtxItem->pRequestData ");
								free(pLRMCtxItem);
								break;
							}
							else
							{
								if (NULL == pAdsppmCtx->RMCtxLock[resourceId-1])
								{
									sts = Adsppm_Status_NotInitialized;
									ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pAdsppmCtx->RMCtxLock[resourceId-1]=NULL" );
								}
								else
								{
									ADSPPM_LOCK_RESOURCE(resourceId);
								memset(pLRMCtxItem->pRequestData, 0, getRequestDataSize(resourceId));
								if(getRequestData_ExtSize(resourceId))
								{
									pLRMCtxItem->pRequestData_Ext = malloc(getRequestData_ExtSize(resourceId));
									if(NULL == pLRMCtxItem->pRequestData_Ext)
									{
										 sts = Adsppm_Status_Failed;
										 ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Malloc failed for pLRMCtxItem->pRequestData ");
										 free(pLRMCtxItem->pRequestData);
										 free(pLRMCtxItem);
										 break;
									}
									else
									{
										memset(pLRMCtxItem->pRequestData_Ext, 0, getRequestData_ExtSize(resourceId));
									}
								}
								pLRMCtxItem->clientId = clientId;
								pLRMCtxItem->rmRscId = resourceId;
								coreUtils_Q_Link(pLRMCtxItem, &pLRMCtxItem->RMCtxLink);
								coreUtils_Q_Put(&(pAdsppmCtx->AdsppmReqQArray[resourceId-1]), &pLRMCtxItem->RMCtxLink);	
									ADSPPM_UNLOCK_RESOURCE(resourceId);
								}
							}
						}
					}
				}
                adsppm_unlock(pAdsppmCtx->adsppmCtxLock);
                if(Adsppm_Status_Success != sts)
                {
                    RM_DeleteClient(clientId);
                    clientId = 0;
                }
            }
        } /* lock null failed */
    } /* parameter check failed */
    ADSPPM_LOG_FUNC_EXIT(clientId);
    return clientId;
}

__inline AdsppmHandleType* RM_GetClientHandle(uint32 clientId)
{
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
    AdsppmHandleType* pClientHandle = coreUtils_Q_LinearSearch(&pAdsppmCtx->AdsppmHandleQ,
            (coreUtils_Q_CompareFuncType)findClientHandle, (void *)&clientId);
    return pClientHandle;
}


/**============================================================================

@fn RM_GetClientInfo
@Brief -  based on clientid, retrieve information about coreID and callback function.

@param input uint32 clientId - client ID.
@param output AdsppmCoreIdType *pCoreId - core ID that client registered.
@param output uint32 *callback - client call back function pointer.

@return Adsppm_Status_BadParm
@return Adsppm_Status_Success

============================================================================*/


Adsppm_Status RM_GetClientInfo(uint32 clientId, AdsppmCoreIdType *pCoreId, uint32 *callback, AdsppmClientTypeType* clientType)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    if(0 == clientId)
    {
        sts = Adsppm_Status_BadParm;
    }
    else
    {
        sts = Adsppm_Status_UnSupported;
        //*pCoreId = gAdsppmCtx.pAdsppmHandle[clientId-1].coreId;
        // callback = gAdsppmCtx.pAdsppmHandle[clientId-1].ADSPPM_Callback;
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

/**============================================================================

@fn RM_GetResourceInfo
@Brief -  based on clientid, retrieve information about coreID and callback function.

@param input uint32 clientId - client ID.
@param output AdsppmCoreIdType *pCoreId - core ID that client registered.
@param output uint32 *callback - client call back function pointer.

@return Adsppm_Status_BadParm
@return Adsppm_Status_Success

============================================================================*/


Adsppm_Status RM_GetResourceInfo(AdsppmRscIdType rsc_Id, void *pInfo, AdsppmInfoType infoType)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AsicFeatureDescType *pDesc = NULL;
    ADSPPM_LOG_FUNC_ENTER;
    if(NULL == pInfo)
    {
        sts = Adsppm_Status_NullPointer;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "NULL pointer for pInfo ");
    }
    else
    {
        switch(rsc_Id)
        {
        case Adsppm_Rsc_Id_Core_Clk:
		    if( Adsppm_Info_Type_Current_Value == infoType)
			{
				sts = CLK_GetInfo((AdsppmInfoClkFreqType *)pInfo);
			}
			else if(Adsppm_Info_Type_Max_Value == infoType)
			{
				pDesc = ACM_GetFeatureDescriptor(AsicFeatureId_CoreClk_Scaling);
				((AdsppmInfoClkFreqType *)pInfo)->clkFreqHz = pDesc->max;
			}
			else
			{
				sts = Adsppm_Status_UnSupported;
			}
            break;
        case Adsppm_Rsc_Id_Mips:
		    if(Adsppm_Info_Type_Max_Value == infoType)
			{
				pDesc = ACM_GetFeatureDescriptor(AsicFeatureId_Adsp_Clock_Scaling);
				*(uint32 *)pInfo = pDesc->max;
			}
			else
			{
				sts = Adsppm_Status_UnSupported;
			}
            break;
        case Adsppm_Rsc_Id_BW:
			if(Adsppm_Info_Type_Max_Value == infoType)
			{
				pDesc = ACM_GetFeatureDescriptor(AsicFeatureId_Ahb_Scaling);
				*(uint64 *)pInfo = pDesc->max;
			}
			else
			{
				sts = Adsppm_Status_UnSupported;
			}
            break;
        default:
        sts = Adsppm_Status_UnSupported;
        break; 
        }
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}




/**============================================================================

@fn RM_DeleteClient
@Brief -  mark client as invalid and free all client’s resources.

@param input uint32 clientId - client ID.

@return Adsppm_Status_BadParm
@return Adsppm_Status_Success

============================================================================*/


Adsppm_Status RM_DeleteClient(uint32 clientId)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
    AdsppmRscIdType   index;
	RMCtxType *pLRMCtxItem = NULL;
	AdsppmHandleType *pLHandleItem = NULL;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Delete clientId=%d ", clientId);
    if (0 == clientId)
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Invalid ClientId %d", clientId);
    } 
    else
    {
        adsppm_lock(pAdsppmCtx->adsppmCtxLock);
		pLHandleItem = coreUtils_Q_LinearSearch(&pAdsppmCtx->AdsppmHandleQ, (coreUtils_Q_CompareFuncType)findClientHandle, (void *)&clientId);
		if(NULL != pLHandleItem)
		{
            /* release resource */
            for(index = Adsppm_Rsc_Id_Power; index < Adsppm_Rsc_Id_Max; index++)
            {
		if (NULL == pAdsppmCtx->RMCtxLock[index-1])
		{
			sts = Adsppm_Status_NotInitialized;
			ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pAdsppmCtx->RMCtxLock[index-1]=NULL" );
		}
		else
		{
			ADSPPM_LOCK_RESOURCE(index);
                pLRMCtxItem = coreUtils_Q_LinearSearch(&(pAdsppmCtx->AdsppmReqQArray[index-1]), (coreUtils_Q_CompareFuncType)findRMCtx, (void *)&clientId);
    			if(NULL != pLRMCtxItem)
				{
					pLRMCtxItem->validFlag = RM_InValid;
					if(NULL != pLRMCtxItem->pRequestData)
					{
							free(pLRMCtxItem->pRequestData);
							pLRMCtxItem->pRequestData = NULL;
					}
					pLRMCtxItem->validFlag_Ext = RM_InValid;
					if(NULL != pLRMCtxItem->pRequestData_Ext)
					{
							free(pLRMCtxItem->pRequestData_Ext);
							pLRMCtxItem->pRequestData_Ext = NULL;
					}
					coreUtils_Q_Delete(&(pAdsppmCtx->AdsppmReqQArray[index-1]), &pLRMCtxItem->RMCtxLink);
					free(pLRMCtxItem);
				}
			ADSPPM_UNLOCK_RESOURCE(index);
		}
            }
		 /* free set paramter memory*/
			RM_FreeParam(pLHandleItem);
        /* set client handle to invalid */
			coreUtils_Q_Delete(&pAdsppmCtx->AdsppmHandleQ, &pLHandleItem->AdsppmHandleLink);
            free(pLHandleItem);
		}
        else
		{
		    sts = Adsppm_Status_Failed;
			ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "can't find handle for clientid=%d", clientId);
		}		
        /* Update the counter for registered clients */
        pAdsppmCtx->numOfRegisteredClients--;
        adsppm_unlock(pAdsppmCtx->adsppmCtxLock);
	}
    /* client id point out of range? */
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

/**============================================================================

@fn RM_AddRequest
@Brief - fill in the client’s request data for the specified resource and mark the request as valid.

@param input uint32 clientId - client ID.
@param input AdsppmRscIdType resourceId - resource ID that client request.
@param input void* pAdsppmReqData - pointer for request data, it'll cast to different data structure based on resource ID.
@param input uint32 extData - if non-zero extended data will be updated instead of request data.

@return Adsppm_Status_BadParm
@return Adsppm_Status_NotInitialized
@return Adsppm_Status_Success

============================================================================*/
Adsppm_Status RM_AddRequest(uint32 clientId, AdsppmRscIdType resourceId, void* pAdsppmReqData, uint32 extData)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
	RMCtxType *pLRMCtxItem = NULL;
	AdsppmHandleType *pLHandleItem = NULL;
    ADSPPM_LOG_FUNC_ENTER;
    if ((0 == clientId)
            ||(Adsppm_Rsc_Id_None == resourceId) || (Adsppm_Rsc_Id_Max <= resourceId) || (NULL == pAdsppmReqData))
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Invalid clientId or resourceId,clientId=%d, resourceId=%d ", clientId, resourceId );

    }
    else
    {
        if (NULL == pAdsppmCtx->adsppmCtxLock)
        {
            sts = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
        else
        {
            adsppm_lock(pAdsppmCtx->adsppmCtxLock);
        if (NULL == pAdsppmCtx->RMCtxLock[resourceId-1])
        {
            sts = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pAdsppmCtx->RMCtxLock[resourceId-1]=NULL" );
        }
        else
        {
            ADSPPM_LOCK_RESOURCE(resourceId);
			pLHandleItem = coreUtils_Q_LinearSearch(&pAdsppmCtx->AdsppmHandleQ, (coreUtils_Q_CompareFuncType)findClientHandle, (void *)&clientId);
            if (NULL == pLHandleItem)
            {
               sts = Adsppm_Status_BadParm;
               ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "invalid request: clientId=%d, resourceId=%d", clientId, resourceId);
            }
            else
            {
                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "Adding request: clientId=%d (%s), resourceId=%d", clientId, pLHandleItem->clientName, resourceId);

    			pLRMCtxItem = coreUtils_Q_LinearSearch(&(pAdsppmCtx->AdsppmReqQArray[resourceId-1]), (coreUtils_Q_CompareFuncType)findRMCtx, (void *)&clientId);
                //the first time new resource request from client ID, need allocate item for that
                if (NULL == pLRMCtxItem)  
                {
					sts = Adsppm_Status_BadParm;
					ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "can't find RMCtx: clientId=%d, resourceId=%d", clientId, resourceId);
                }
                if((NULL != pLRMCtxItem)&&(NULL != pLRMCtxItem->pRequestData) 
    				&& (NULL != pLHandleItem))
                {
                    if((pLRMCtxItem->clientId == clientId)
                            && (pLRMCtxItem->rmRscId == resourceId))
                    {
                        if(0 == extData)
                        {
                                memscpy(pLRMCtxItem->pRequestData, getRequestDataSize(resourceId), pAdsppmReqData, getRequestDataSize(resourceId));
                            pLRMCtxItem->validFlag = RM_Valid;
                            ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "Set request valid for clientId =%d (%s), resourceId=%d ", clientId, pLHandleItem->clientName ,resourceId );
                        }
                        else
                        {
						    if(NULL != pLRMCtxItem->pRequestData_Ext)
							{
		         		memscpy(pLRMCtxItem->pRequestData_Ext, getRequestData_ExtSize(resourceId), pAdsppmReqData, getRequestData_ExtSize(resourceId));
								pLRMCtxItem->validFlag_Ext = RM_Valid;
								ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "Set ext request valid for clientId =%d (%s), resourceId=%d ", clientId, pLHandleItem->clientName, resourceId );
							}
                        }
                    }
                    else
                    {
                        sts = Adsppm_Status_BadParm;
                    }
                }
            }
            ADSPPM_UNLOCK_RESOURCE(resourceId);
        }
            adsppm_unlock(pAdsppmCtx->adsppmCtxLock);
        }
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}


/**============================================================================

@fn RM_ReleaseRequest
@Brief - mark request for the client/resource pair as invalid.

@param input uint32 clientId - client ID.
@param input AdsppmRscIdType resourceId - resource ID that client request.
@param input uint32 extData - if non-zero extended data will be updated instead of request data.

@return Adsppm_Status_BadParm
@return Adsppm_Status_Success

============================================================================*/


Adsppm_Status RM_ReleaseRequest(uint32 clientId,  AdsppmRscIdType resourceId, uint32 extData)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
	RMCtxType *pLRMCtxItem = NULL;
    AdsppmHandleType *pLHandleItem = NULL;
    ADSPPM_LOG_FUNC_ENTER;

    if ((0 == clientId)
            ||(0 == resourceId) || (Adsppm_Rsc_Id_Max-1 < resourceId))
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "Invalid clientId or resourceId: clientId=%d, resourceId=%d ", clientId, resourceId );
    }
    else
    {
	if (NULL == pAdsppmCtx->adsppmCtxLock)
        {
            sts = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
        else
        {
            adsppm_lock(pAdsppmCtx->adsppmCtxLock);
	 
            ADSPPM_LOCK_RESOURCE(resourceId);
			pLRMCtxItem = coreUtils_Q_LinearSearch(&(pAdsppmCtx->AdsppmReqQArray[resourceId-1]), (coreUtils_Q_CompareFuncType)findRMCtx, (void *)&clientId);
			pLHandleItem = coreUtils_Q_LinearSearch(&pAdsppmCtx->AdsppmHandleQ, (coreUtils_Q_CompareFuncType)findClientHandle, (void *)&clientId);

            if((NULL != pLRMCtxItem)&&(NULL != pLHandleItem))
			{
				ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO, "Releasing request: clientId=%d (%s) , resourceId=%d ", clientId, pLHandleItem->clientName, resourceId );
        
				if(pLRMCtxItem->rmRscId == resourceId)
                {
					if(0 == extData)
					{
						if(pLRMCtxItem->validFlag != RM_InValid)
						{
							pLRMCtxItem->validFlag = RM_InValid;
							ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "Set request to invalid: clientId=%d (%s), resourceId=%d ", clientId, pLHandleItem->clientName, resourceId );
						}
						else
						{
							sts = Adsppm_Status_ResourceNotRequested;
						}
					}
					else
					{
						if(pLRMCtxItem->validFlag_Ext != RM_InValid)
						{
							pLRMCtxItem->validFlag_Ext = RM_InValid;
							ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT, "Set ext request to invalid: clientId=%d (%s), resourceId=%d ", clientId, pLHandleItem->clientName, resourceId );
						}
						else
						{
							sts = Adsppm_Status_ResourceNotRequested;
						}
					}
                }
                else
                {
                    sts = Adsppm_Status_BadParm;
					ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "release request with invalid resourceid=%d", resourceId);
                }
            }
			else
			{
				sts = Adsppm_Status_BadParm;
				ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "release request with invalid clientId=%d", clientId);
			}
         ADSPPM_UNLOCK_RESOURCE(resourceId);
            adsppm_unlock(pAdsppmCtx->adsppmCtxLock);
        } 
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

/**============================================================================

@fn RM_RequestResource
@Brief - send request to resource managers, based on resource ID.

@param input AdsppmRscIdType resourceId - resource ID that client request.

@return Adsppm_Status_BadParm
@return Adsppm_Status_Success

============================================================================*/

Adsppm_Status RM_RequestResource(AdsppmRscIdType resourceId)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();

    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "Request resourceId=%d ", resourceId);
    if ((Adsppm_Rsc_Id_None == resourceId) || (Adsppm_Rsc_Id_Max < resourceId))
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Invalid resourceId=%d ", resourceId);
    } 
    else
    {
        //TODO: ADSPPM_LOG_4(MMPM_LOG_LEVEL_DEBUG, MMPM2_LOG_REQUEST_PARAM, clientId, pAdsppmCtx->pClients[clientId-1]->coreId, pAdsppmCtx->pClients[clientId-1]->instanceId, pAdsppmRscParam->rscId);
        switch (resourceId)
        {
        case Adsppm_Rsc_Id_Core_Clk:
        {
            sts = CLK_ProcessRequest(&(pAdsppmCtx->AdsppmReqQArray[Adsppm_Rsc_Id_Core_Clk-1]));
            break;
        }
        case Adsppm_Rsc_Id_Power:
        {
            sts = PWR_ProcessRequest(&(pAdsppmCtx->AdsppmReqQArray[Adsppm_Rsc_Id_Power-1]));
            break;
        }

        case Adsppm_Rsc_Id_Sleep_Latency:
        {
            sts = SLEEP_ProcessRequest(&(pAdsppmCtx->AdsppmReqQArray[Adsppm_Rsc_Id_Sleep_Latency-1]));
            break;
        }
        case Adsppm_Rsc_Id_Mips:
        {
            sts = MIPS_ProcessRequest(&(pAdsppmCtx->AdsppmReqQArray[Adsppm_Rsc_Id_Mips-1]));
            break;
        }
        case Adsppm_Rsc_Id_BW:
        {
            sts = BUS_ProcessRequest(&(pAdsppmCtx->AdsppmReqQArray[Adsppm_Rsc_Id_BW-1]));
            break;
        }
        case Adsppm_Rsc_Id_Thermal:
        {
            sts = THERMAL_ProcessRequest(&(pAdsppmCtx->AdsppmReqQArray[Adsppm_Rsc_Id_Thermal-1]));
            break;
        }
        case Adsppm_Rsc_Id_MemPower:
            sts = MEMPWR_ProcessRequest(&(pAdsppmCtx->AdsppmReqQArray[Adsppm_Rsc_Id_MemPower-1]));
            break;
	    case Adsppm_Rsc_Id_Core_Clk_Domain:
		    sts = CLKDomain_ProcessRequest(&(pAdsppmCtx->AdsppmReqQArray[Adsppm_Rsc_Id_Core_Clk_Domain-1]));
            break;
        default:
        {
            sts = Adsppm_Status_UnSupported;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Unsupported resourceId=%d ", resourceId);
            break;
        }
        } /* switch resource Id */
    }/* resoureId out of range? */
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

/**
 * RM_SetParameter - set parameters for client, allocate memory for parameter and put parameter pointer into client handle context
 */

Adsppm_Status RM_SetParameter(uint32 clientId, AdsppmParameterConfigType *pParamConfigData)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCtxType* pAdsppmCtx = GetGlobalContext();
	AdsppmHandleType *pLHandleItem = NULL;


    ADSPPM_LOG_FUNC_ENTER;
    if(NULL == pParamConfigData)
    {
        sts = Adsppm_Status_BadParm;
    }
    else
    {
		pLHandleItem = coreUtils_Q_LinearSearch(&pAdsppmCtx->AdsppmHandleQ, (coreUtils_Q_CompareFuncType)findClientHandle, (void *)&clientId);
		if(NULL != pLHandleItem)
    {
        switch(pParamConfigData->paramId)
        {
        break;
        case Adsppm_Param_Id_Resource_Limit:
        default:
            {
            sts = Adsppm_Status_UnSupported;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "Parameter %d not supported",
                    pParamConfigData->paramId);
            }
            break;
        }
		}
		else
		{
			sts = Adsppm_Status_BadParm;
			ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "clientId = %d is invalid", clientId);
        }
    }
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

/** 
 * RM_FreeParam - free parameter's memory, it'll be called in delete client time
 */

static void RM_FreeParam(AdsppmHandleType *pHandle)
{

    ADSPPM_LOG_FUNC_ENTER;


    if( NULL != pHandle->pSetParamConfig)
    {
        free(pHandle->pSetParamConfig);
        pHandle->pSetParamConfig = NULL;
    }
    ADSPPM_LOG_FUNC_EXIT(0);
}

__inline AdsppmBusPortIdType RM_GetCoreSlavePort(uint32 clientId)
{
    AdsppmBusPortIdType port = AdsppmBusPort_None;
    AdsppmCtxType *pAdsppmCtx = GetGlobalContext();
    AsicCoreDescType* pCoreDesc = NULL;
	AdsppmHandleType *pLHandleItem = NULL;
	pLHandleItem = coreUtils_Q_LinearSearch(&pAdsppmCtx->AdsppmHandleQ, (coreUtils_Q_CompareFuncType)findClientHandle, (void *)&clientId);
	if(NULL != pLHandleItem)
	{
		pCoreDesc = ACM_GetCoreDescriptor(pLHandleItem->coreId);
    if (NULL != pCoreDesc)
    {
        port = pCoreDesc->slaveBusPort;
    }
	}
	else
	{
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "clientId = %d is invalid", clientId);
	}
    return port;
}


