/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_busdrv.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */


#include "hal_busdrv.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "icbarb.h"
#include "npa.h"
#include "adsppm.h"
#include "asic.h"
#include "npa_transaction.h"
#include "core_internal.h"
#include "stdlib.h"
#include "stdio.h"
#include "core.h"
#include "icbid.h"
#include "adsppm_utils.h"


//structs
typedef struct{
   AdsppmBusRouteType busdrvRoute;
   npa_client_handle handle;
   char clientName[MAX_ADSPPM_CLIENT_NAME];
}clientHandleType;

typedef struct
{
       DALSYSSyncHandle       busdrbCtxLock;
}busdrvCtxType;

typedef struct
{
   boolean gBusInitSuccess;
   uint32 gNumMasterSlave;
   AsicBusExtRouteType *gExtRouteArray;
   AdsppmBusBWDataIbAbType *pExtBwValues;
   npa_transaction_handle gTransactionHandle;
   clientHandleType *gBwRequestHandle;
   clientHandleType gMipsBwRequestHandle;
   uint32 gNumMipsRequest;
   AdsppmBusRouteType *pMipsRouteArray;
   AdsppmBusBWDataIbAbType *pMipsBwValues;
}busdrvInfoType;

static busdrvInfoType busdrvInfo;
static busdrvCtxType busdrvCtx;
static void BwHandleInitCb ( void         *context,
                     unsigned int  event_type,
                     void         *data,
                     unsigned int  data_size );
const char *resources[] = {(ICBARB_NODE_NAME)};


AdsppmStatusType Bus_Init(void)
{
   
   AdsppmStatusType sts = Adsppm_Status_Success;
   ADSPPM_LOG_FUNC_ENTER;
   ADSPPM_LOG_FUNC_PROFILESTART;
   busdrvInfo.gBusInitSuccess = FALSE;
   busdrvInfo.gNumMipsRequest =  ACMBus_GetNumberOfMipsBwRoutes();
   busdrvInfo.pMipsRouteArray = ACMBus_GetMipsBwRoutes();
   //getting number of Master/Slave routes and allocating memory for ExtRouteArray and ClientHandle arrarys
   busdrvInfo.gNumMasterSlave = ACMBus_GetNumberOfExtRoutes();
   if(0 != busdrvInfo.gNumMasterSlave)
   {
      busdrvInfo.gExtRouteArray = (AsicBusExtRouteType *) malloc(sizeof(AsicBusExtRouteType)*(busdrvInfo.gNumMasterSlave));
      busdrvInfo.gBwRequestHandle= (clientHandleType *) malloc(sizeof(clientHandleType)*(busdrvInfo.gNumMasterSlave)); //extra handle for MIPS
      busdrvInfo.pExtBwValues = (AdsppmBusBWDataIbAbType *) malloc(sizeof(AdsppmBusBWDataIbAbType)*(busdrvInfo.gNumMasterSlave));
      busdrvInfo.pMipsBwValues = (AdsppmBusBWDataIbAbType *) malloc(sizeof(AdsppmBusBWDataIbAbType)*(busdrvInfo.gNumMipsRequest));
      if ((NULL == busdrvInfo.gExtRouteArray)||
      (NULL == busdrvInfo.gBwRequestHandle) ||
      (NULL == busdrvInfo.pExtBwValues) ||
      (NULL == busdrvInfo.pMipsBwValues))
      {
         sts = Adsppm_Status_NoMemory;
         if(NULL != busdrvInfo.gExtRouteArray)
         {
            free(busdrvInfo.gExtRouteArray);
         }
         if (NULL != busdrvInfo.gBwRequestHandle)
         {
            free( busdrvInfo.gBwRequestHandle);
         }
         if(NULL != busdrvInfo.pExtBwValues)
         {
             free(busdrvInfo.pExtBwValues);
         }
         if(NULL != busdrvInfo.pMipsBwValues)
         {
             free(busdrvInfo.pMipsBwValues);
         }
      }
      else
      {
         if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&busdrvCtx.busdrbCtxLock, NULL))
         {
             sts = Adsppm_Status_Failed;
         }
         else
         {
            //getting array of master/slave routes
            ACMBus_GetExtRoutes(busdrvInfo.gExtRouteArray);
            npa_resource_available_cb(ICBARB_NODE_NAME, BwHandleInitCb, NULL);
         }
      }
   }
   else
   {
         sts = Adsppm_Status_Failed;
   }
   ADSPPM_LOG_FUNC_PROFILEEND;
   ADSPPM_LOG_FUNC_EXIT(sts);
   return sts;
}

static void BwHandleInitCb ( void         *context,
      unsigned int  event_type,
      void         *data,
      unsigned int  data_size )
{

   ICBArb_CreateClientVectorType vector;
   ICBArb_CreateClientVectorType mipsVector;
   ICBArb_MasterSlaveType route;
   uint32 i,j;
   uint32 result = TRUE;
   ADSPPM_LOG_FUNC_ENTER;
   ADSPPM_LOG_FUNC_PROFILESTART;
   //gets transaction handle
   busdrvInfo.gTransactionHandle = npa_define_transaction("Adsppm_Bw",
           NPA_TRANSACTION_LAZY_LOCKING,
           1,
           resources);

   if (NULL != busdrvInfo.gTransactionHandle)
   {
       ICBArb_MasterSlaveType *mipsMasterSlavePairs =  (ICBArb_MasterSlaveType *) malloc(sizeof(ICBArb_MasterSlaveType)*(busdrvInfo.gNumMipsRequest));

       if(NULL != mipsMasterSlavePairs)
       {
           for (i = 0; i < busdrvInfo.gNumMasterSlave ; i++)
           {
               route.eMaster = busdrvInfo.gExtRouteArray[i].masterPort.icbarbMaster;
               route.eSlave = busdrvInfo.gExtRouteArray[i].slavePort.icbarbSlave;

               busdrvInfo.gBwRequestHandle[i].busdrvRoute.masterPort
               = busdrvInfo.gExtRouteArray[i].masterPort.adsppmMaster;
               busdrvInfo.gBwRequestHandle[i].busdrvRoute.slavePort
               = busdrvInfo.gExtRouteArray[i].slavePort.adsppmSlave;

               snprintf(busdrvInfo.gBwRequestHandle[i].clientName,
                       sizeof(busdrvInfo.gBwRequestHandle[i].clientName),
                       "AdsppmM%dS%d",
                       busdrvInfo.gBwRequestHandle[i].busdrvRoute.masterPort,
                       busdrvInfo.gBwRequestHandle[i].busdrvRoute.slavePort);

               if(AdsppmBusPort_Adsp_Master == busdrvInfo.gBwRequestHandle[i].busdrvRoute.masterPort)
               {
                    busdrvInfo.gBwRequestHandle[i].handle = npa_create_sync_client_ex( ICBARB_NODE_NAME,
                        busdrvInfo.gBwRequestHandle[i].clientName,
                        NPA_CLIENT_SUPPRESSIBLE_VECTOR,
                        ICBARB_CREATE_CLIENT_VECTOR(&vector,&route,1,NULL));
               }
               else
               {
                    busdrvInfo.gBwRequestHandle[i].handle = npa_create_sync_client_ex( ICBARB_NODE_NAME,
                        busdrvInfo.gBwRequestHandle[i].clientName,
                        NPA_CLIENT_VECTOR,
                        ICBARB_CREATE_CLIENT_VECTOR(&vector,&route,1,NULL));
               }

               if (NULL == busdrvInfo.gBwRequestHandle[i].handle)
               {
                   ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "icbarb client create failed:M=%d,S=%d\n",busdrvInfo.gBwRequestHandle[i].busdrvRoute.masterPort, busdrvInfo.gBwRequestHandle[i].busdrvRoute.slavePort );
                   busdrvInfo.gBusInitSuccess = FALSE;
                   result = FALSE;
                   break;
               }

               for (j = 0; j<busdrvInfo.gNumMipsRequest; j++)
               {
                   if((busdrvInfo.gExtRouteArray[i].masterPort.adsppmMaster == busdrvInfo.pMipsRouteArray[j].masterPort) &&
                           (busdrvInfo.gExtRouteArray[i].slavePort.adsppmSlave == busdrvInfo.pMipsRouteArray[j].slavePort))
                   {
                       mipsMasterSlavePairs[j].eMaster = route.eMaster;
                       mipsMasterSlavePairs[j].eSlave = route.eSlave;
                   }
               }
           }
           if (TRUE == result)
           {
        	   snprintf(busdrvInfo.gMipsBwRequestHandle.clientName, MAX_ADSPPM_CLIENT_NAME, "ADSPPM MIPS BW");
        	   busdrvInfo.gMipsBwRequestHandle.handle = npa_create_sync_client_ex( ICBARB_NODE_NAME,
        			   busdrvInfo.gMipsBwRequestHandle.clientName,
        			   NPA_CLIENT_SUPPRESSIBLE_VECTOR,
        			   ICBARB_CREATE_CLIENT_VECTOR(&mipsVector,mipsMasterSlavePairs,busdrvInfo.gNumMipsRequest,NULL));
        	   if (NULL != busdrvInfo.gMipsBwRequestHandle.handle)
        	   {
        		   busdrvInfo.gBusInitSuccess = TRUE;
        	   }
        	   else
        	   {
        		   busdrvInfo.gBusInitSuccess = FALSE;
        		   ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed creation of MIPS icbarb client \n");
        	   }
           }
           free(mipsMasterSlavePairs);
       }
       else
       {
           busdrvInfo.gBusInitSuccess = FALSE;
           ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to allocate memory\n");
       }
   }
   ADSPPM_LOG_FUNC_PROFILEEND;
   ADSPPM_LOG_FUNC_EXIT(busdrvInfo.gBusInitSuccess);
}

AdsppmStatusType Bus_IssueBWRequest_MIPS(AdsppmBusBWRequestValueType *pRequest)
{
    ICBArb_RequestType bwRequest[busdrvInfo.gNumMipsRequest];
    uint32 i, j;
    AdsppmStatusType sts = Adsppm_Status_Success;

    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if (FALSE == busdrvInfo.gBusInitSuccess)
    {
    	ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized\n");
    	sts = Adsppm_Status_NotInitialized;
    }
    else
    {
    	adsppm_lock(busdrvCtx.busdrbCtxLock);
    	for(i = 0; i < busdrvInfo.gNumMipsRequest; i++)
    	{
    		for(j = 0; j < busdrvInfo.gNumMipsRequest; j++)
    		{
    			if((pRequest[i].busRoute.masterPort == busdrvInfo.pMipsRouteArray[j].masterPort)
    					&& (pRequest[i].busRoute.slavePort == busdrvInfo.pMipsRouteArray[j].slavePort))
    			{
    				bwRequest[j].arbType = ICBARB_REQUEST_TYPE_3;
    				bwRequest[j].arbData.type3.uIb = pRequest[i].bwValue.busBwAbIb.Ib;
    				bwRequest[j].arbData.type3.uAb = pRequest[i].bwValue.busBwAbIb.Ab;
    				ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO, "master: %d, slave: %d, Ib: %d, Ab: %d\n",pRequest[i].busRoute.masterPort, pRequest[i].busRoute.slavePort, (uint32)bwRequest[j].arbData.type3.uIb, (uint32)bwRequest[j].arbData.type3.uAb);
    				ADSPPM_QDSS_EVENT_4(ADSPPM_BW_REQ, pRequest[i].busRoute.masterPort, pRequest[i].busRoute.slavePort, (uint32)bwRequest[j].arbData.type3.uIb, (uint32)bwRequest[j].arbData.type3.uAb);
    			}
    			busdrvInfo.pMipsBwValues[i].Ab = pRequest[i].bwValue.busBwAbIb.Ab;
    			busdrvInfo.pMipsBwValues[i].Ib = pRequest[i].bwValue.busBwAbIb.Ib;
    		}
    	}
    	npa_set_request_attribute( busdrvInfo.gMipsBwRequestHandle.handle, NPA_REQUEST_FIRE_AND_FORGET);
    	icbarb_issue_request(busdrvInfo.gMipsBwRequestHandle.handle, bwRequest, busdrvInfo.gNumMipsRequest);
    	adsppm_unlock(busdrvCtx.busdrbCtxLock);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}

AdsppmStatusType Bus_ReleaseBWRequest_MIPS(void)
{
	AdsppmStatusType sts = Adsppm_Status_Success;
	int i = 0;
	ADSPPM_LOG_FUNC_ENTER;
	ADSPPM_LOG_FUNC_PROFILESTART;

	if (busdrvInfo.gBusInitSuccess == FALSE)
	{
		sts = Adsppm_Status_NotInitialized;
	}
	else
	{
		adsppm_lock(busdrvCtx.busdrbCtxLock);

		icbarb_complete_request(busdrvInfo.gMipsBwRequestHandle.handle);
		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "Complete bus request\n");
		for (i=0; i < busdrvInfo.gNumMipsRequest; i++)
		{
			busdrvInfo.pMipsBwValues[i].Ab = 0;
			busdrvInfo.pMipsBwValues[i].Ib = 0;
		}
		adsppm_unlock(busdrvCtx.busdrbCtxLock);

	}

	ADSPPM_LOG_FUNC_PROFILEEND;
	ADSPPM_LOG_FUNC_EXIT(sts);

	return sts;
}

AdsppmStatusType Bus_IssueBWRequest( int num, AdsppmBusBWRequestValueType *pRequest)
{ 
    uint8 i;
    uint8 j;
    ICBArb_RequestType bwRequest[1];
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if (FALSE == busdrvInfo.gBusInitSuccess )
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        if(0 < num)
        {
            adsppm_lock(busdrvCtx.busdrbCtxLock);
            npa_begin_transaction(busdrvInfo.gTransactionHandle);
            bwRequest[0].arbType = ICBARB_REQUEST_TYPE_3;

            for (j = 0; j< num; j++)
            {
                for (i = 0; i< busdrvInfo.gNumMasterSlave; i++)
                {
                    if((pRequest[j].busRoute.masterPort == busdrvInfo.gBwRequestHandle[i].busdrvRoute.masterPort)&&
                            (pRequest[j].busRoute.slavePort == busdrvInfo.gBwRequestHandle[i].busdrvRoute.slavePort))
                    {
                        //check ib and ab and issue request (or complete request)
                        if (NULL == busdrvInfo.gBwRequestHandle[i].handle)
                        {
                            sts = Adsppm_Status_Failed;
                            break;
                        }
                        else
                        {
                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "Master: %d, Slave: %d\n",
                                    pRequest[j].busRoute.masterPort, pRequest[j].busRoute.slavePort);
                            ADSPPM_QDSS_EVENT_4(ADSPPM_BW_REQ, pRequest[j].busRoute.masterPort, pRequest[j].busRoute.slavePort, (uint32)pRequest[j].bwValue.busBwAbIb.Ib, (uint32)pRequest[j].bwValue.busBwAbIb.Ab);
                            if ((0 == pRequest[j].bwValue.busBwAbIb.Ab)&&(0 == pRequest[j].bwValue.busBwAbIb.Ib))// 0 == variable
                            {
                                icbarb_complete_request(busdrvInfo.gBwRequestHandle[i].handle);
                                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "Complete bus request,ab,ib is 0\n");
                            }
                            else
                            {
                                bwRequest[0].arbData.type3.uIb = pRequest[j].bwValue.busBwAbIb.Ib;
                                bwRequest[0].arbData.type3.uAb = pRequest[j].bwValue.busBwAbIb.Ab;
                                icbarb_issue_request(busdrvInfo.gBwRequestHandle[i].handle, bwRequest, 1);
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO, "Ib: %d, Ab: %d\n",
                                        (uint32)bwRequest[0].arbData.type3.uIb, (uint32)bwRequest[0].arbData.type3.uAb);                               
                            }
                            busdrvInfo.pExtBwValues[i].Ab = pRequest[j].bwValue.busBwAbIb.Ab;
                            busdrvInfo.pExtBwValues[i].Ib = pRequest[j].bwValue.busBwAbIb.Ib;
                        }
                    }
                }
            }
            npa_end_transaction(busdrvInfo.gTransactionHandle);
            adsppm_unlock(busdrvCtx.busdrbCtxLock);
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    ADSPPM_LOG_FUNC_EXIT(sts);
    return sts;
}


AdsppmStatusType Bus_ReleaseBWRequest( int num, AdsppmBusBWRequestValueType *pRequest)
{ 
   uint8 i;
   uint8 j;
   ICBArb_RequestType bwRequest;
   AdsppmStatusType sts = Adsppm_Status_Success;

   ADSPPM_LOG_FUNC_ENTER;
   ADSPPM_LOG_FUNC_PROFILESTART;
   if (busdrvInfo.gBusInitSuccess == FALSE)
   {
      sts = Adsppm_Status_NotInitialized;
   }
   else
   {

      adsppm_lock(busdrvCtx.busdrbCtxLock);
      npa_begin_transaction(busdrvInfo.gTransactionHandle);
      bwRequest.arbType = ICBARB_REQUEST_TYPE_3;

      for (j = 0; j< num; j++)
      {
         for (i = 0; i< busdrvInfo.gNumMasterSlave; i++)
         {
            if((pRequest[j].busRoute.masterPort == busdrvInfo.gBwRequestHandle[i].busdrvRoute.masterPort)&&
            (pRequest[j].busRoute.slavePort == busdrvInfo.gBwRequestHandle[i].busdrvRoute.slavePort))
            {
               ADSPPM_QDSS_EVENT_2(ADSPPM_BW_REL, pRequest[j].busRoute.masterPort, pRequest[j].busRoute.slavePort);
               //check ib and ab and issue request (or complete request)
               if (NULL == busdrvInfo.gBwRequestHandle[i].handle)
               {
                  sts = Adsppm_Status_Failed;
                  break;
               }
               icbarb_complete_request(busdrvInfo.gBwRequestHandle[i].handle);

               busdrvInfo.pExtBwValues[i].Ab = 0;
               busdrvInfo.pExtBwValues[i].Ib = 0;
               break;
            }
         }
      }
      npa_end_transaction(busdrvInfo.gTransactionHandle);
      adsppm_unlock(busdrvCtx.busdrbCtxLock);
   }
   ADSPPM_LOG_FUNC_PROFILEEND;
   ADSPPM_LOG_FUNC_EXIT(sts);
   return sts;
}

void freeResource(void){
    ADSPPM_LOG_FUNC_ENTER;
      free (busdrvInfo.gExtRouteArray);
      free (busdrvInfo.gBwRequestHandle);
    ADSPPM_LOG_FUNC_EXIT(0);
}//should not be called


/**
 * Get last requested BW value for a specific route
 * @param pRoute - external BW route
 * @return BW value
 */
AdsppmBusBWDataIbAbType Bus_GetRequestedBw(AdsppmBusRouteType *pRoute)
{
    AdsppmBusBWDataIbAbType bw = {0,0};
    int i = 0;
    if ((busdrvInfo.gBusInitSuccess == TRUE) && (NULL != pRoute))
    {
        for (i = 0; i < busdrvInfo.gNumMasterSlave; i++)
        {
            if((busdrvInfo.gExtRouteArray[i].masterPort.adsppmMaster == pRoute->masterPort) &&
                    (busdrvInfo.gExtRouteArray[i].slavePort.adsppmSlave == pRoute->slavePort))
            {
                bw.Ab = busdrvInfo.pExtBwValues[i].Ab;
                bw.Ib = busdrvInfo.pExtBwValues[i].Ib;
                break;
            }
        }

        for (i = 0; i < busdrvInfo.gNumMipsRequest; i++)
        {
            if((busdrvInfo.pMipsRouteArray[i].masterPort == pRoute->masterPort) &&
                    (busdrvInfo.pMipsRouteArray[i].slavePort == pRoute->slavePort))
            {
                bw.Ab += busdrvInfo.pMipsBwValues[i].Ab;
                bw.Ib = MAX(bw.Ib, busdrvInfo.pMipsBwValues[i].Ib);
                break;
            }
        }
    }
    return bw;
}
