/*============================================================================
  @file aon_api.cpp

  @brief
    This file contains the implementation for Alwayson C API.
 
               Copyright (c) 2014 - 2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential.
			   Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc. 

============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_api.cpp#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
09/30/15    sj    Added hysterisis for unsolicited status indications 
08/13/15    sj    Added default configuration of GM from NV 
04/06/15    sj    LB 2.0 integration 
03/11/15    sj    Fixed an overflow issue in the reqId 
01/14/15    sj    Removed BreadCrumbing references 
07/31/14    sj    Initial creation of file.


=============================================================================*/
/*****************************************************************************
 * Include Files
 * *************************************************************************/
#include "aon_api.h"
#include "aon_batchtransac.h"
#include "aon_dbttransac.h"
#include "aon_db.h"
#include "aon_gmproxy.h"
#include "loc_api_internal.h"

static AonDatabase* g_aonDatabase = NULL;
static AonGmProxy* g_aonGmProxy = NULL;
static aonLocationCallback g_locCb = NULL;
static aonStatusCallback g_statusCb = NULL;

/* -----------------------------------------------------------------------*//**
@brief
  This function is used by clients to register to AlwaysOn location Services.
   Any client who is interested in AON services such as Distance Based Tracking etc. 
   should register using this API to AON

@retval  aonOperationStatusType: SUCCESS or FAIL
@param[in] p_LocCb: Pointer to location callback
@param[in] p_StatusCb: Pointer to status callback
@param[in] p_nvConfig: Pointer to the default NV Config
*//* ------------------------------------------------------------------------*/
aonOperationStatusType aonClientRegister(
  aonLocationCallback locCb,
  aonStatusCallback statusCb,
  const aonGmNvDefaultConfigType *p_nvConfig)
{
  AON_MSG_MED("aonClientRegister invoked", 0,0,0);
  if(NULL == locCb || NULL == statusCb)
  {
	return AON_FAILED;
  }

  if(!g_aonDatabase)
	g_aonDatabase = AonDatabase::Instance();

  if(!g_aonGmProxy)
	g_aonGmProxy = AonGmProxy::Instance(p_nvConfig);

  g_locCb = locCb;
  g_statusCb = statusCb;
  return AON_SUCCEEDED;
}
/* -----------------------------------------------------------------------*//**
@brief
  This function is used by clients to create a AON request for a Batching Session

@retval  aonOperationStatusType: SUCCESS or FAIL
@param[in] reqId A unique request Id for this batching request
@param[in] minInterval  Minimum time between location updates
@param[in] minDistance  minimum distance that should be traversed before a 
position should be batched.
@param[in] aonBatchingConfigMask Operation modem configuration that needs to be used
@param[in] batchingGfRadius   Geofence radius to be used for batching request
@param[in] fixTimeout Configures the fix session timeout duration
@param[in] accuracy Specifies the horizontal accuracy level required 
@param[in] aonBatchingMode Batching mode to be employed.
*//* ------------------------------------------------------------------------*/
aonOperationStatusType aonCreateBatchingRequest(
 uint32 reqId, uint32 minInterval, uint32 minDistance, uint32 aonBatchingConfigMask,
 uint32 fixTimeout, uint32 accuracy, aonBatchingMode mode)
{
  int32 err = 0;
  
  AON_MSG_MED("aonCreateBatchingRequest invoked reqId = %d, minInterval = %d, "
	"minDistance = %d", reqId, minInterval, minDistance);
  AON_MSG_MED("aonCreateBatchingRequest invoked config mask = 0x%x, mode = %d, timeout = %d",
	aonBatchingConfigMask, mode, fixTimeout);

  AonBatchTransac *batchTransac = new AonBatchTransac(
	   reqId, minInterval, minDistance, aonBatchingConfigMask,
	   fixTimeout, accuracy, mode, g_aonDatabase, g_aonGmProxy, 
	   g_locCb, g_statusCb);
   
  if(NULL == batchTransac) 
  {
	err = -1;
  }
  else if(FALSE == g_aonDatabase->addAonTransac(batchTransac))
  {
	err = -2; 
	delete(batchTransac);
  }
  else if( AON_SUCCEEDED != batchTransac->start())
  {
	err = -3;
	(void)g_aonDatabase->delAonTransac(batchTransac); //batchTrasac will get deleted inside.
  }

  if( 0 != err)
  {
	AON_MSG_ERROR("aonCreateBatchingRequest failed err = %d, reqID = %d", 
	  err, reqId,0);
	return AON_FAILED;
  }

  return AON_SUCCEEDED;
}


/* -----------------------------------------------------------------------*//**
@brief
  This function is used by clients to create a AON request for a Distance
  Based Tracking Session

@retval  aonOperationStatusType: SUCCESS or FAIL
@param[in] reqId A unique request Id for this location request
@param[in] minDistance minimum threshold distance after which location updates are needed
@param[in] distType - Whether straight line distance or any other type of tracking required
@param[in] needOriginLocation - If location at the origin is required
@param[in] maxLatency maximum latency threshold within which location updates are needed
@param[in] usageType Whether high performance Navigation type tracking is required
*//* ------------------------------------------------------------------------*/
aonOperationStatusType aonCreateDbtRequest(uint32 reqId, 
  uint32 minDistance, aonDbtDistanceType distType,
  boolean needOriginLocation,
  uint32 maxLatency, aonDbtUsageType usageType)
{
   int32 err = 0;

   AON_MSG_MED("aonCreateDbtRequest invoked reqId = %d, minDistance = %d, "
	"maxLatency = %d", reqId, minDistance, maxLatency);
	
   AonDbtTransac *dbtTransac = new AonDbtTransac(reqId, minDistance, distType,
                                                 needOriginLocation,
                                                 maxLatency, usageType, g_aonDatabase, g_aonGmProxy, 
	                                             g_locCb, g_statusCb);
  if(NULL == dbtTransac) 
  {
	err = -1;
  }
  else if(FALSE == g_aonDatabase->addAonTransac(dbtTransac))
  {
	err = -2; 
	delete(dbtTransac);
  }
  else if( AON_SUCCEEDED != dbtTransac->start())
  {
	err = -3;
	(void)g_aonDatabase->delAonTransac(dbtTransac); //dbtTransac will get deleted inside.
  }

  if( 0 != err)
  {
	AON_MSG_ERROR("aonCreateDbtRequest failed err = %d, reqID = %d", 
	  err, reqId,0);
	return AON_FAILED;
  }

  return AON_SUCCEEDED;

}
/* -----------------------------------------------------------------------*//**
@brief
  This function is used to delete an AON request 

@retval  aonOperationStatusType: SUCCESS or FAIL
@param[in] reqId A unique request Id for this location request

*//* ------------------------------------------------------------------------*/
aonOperationStatusType aonDeleteRequest(
 uint32 reqId)
{
  AON_MSG_MED("aonDeleteRequest invoked reqId = %d", reqId,0,0);
  AonTransac *aonTransac = g_aonDatabase->getAonTransacByReqId(reqId);
  aonOperationStatusType retVal = AON_FAILED;
  if( NULL != aonTransac)
  {
	
	/* stop if current transaction -- always succeeds */
	aonTransac->stop();
	
	/* Remove the Transaction entry from the TransactionInfo*
	and delete the transation */
	if(TRUE == g_aonDatabase->delAonTransac(aonTransac))
	{
	  retVal = AON_SUCCEEDED;
	}	
  }
  return retVal;
}
/* -----------------------------------------------------------------------*//**
@brief
  This function is used to route a GM message to AON library
  if available

@retval  aonOperationStatusType: SUCCESS or FAIL
@param[in] q_MsgId GM message Id
@param[in] pData   Message payload 
*//* ------------------------------------------------------------------------*/
aonOperationStatusType aonGmEventHandler(uint32 q_MsgId, uint8 *p_data)
{
  AON_MSG_MED("AON- aonGmEventHandler invoked.MsgId %d ", q_MsgId , 0, 0);
  if(g_aonGmProxy->gmEventHandler(q_MsgId, p_data))
	return AON_SUCCEEDED;
  else
	return AON_FAILED;
}

/* 
@brief handle the AON timer IPC messages
@param[in] id corresponding to the timer
@return None 
*/
void aonTimerHandler( uint32 timerId)
{

  if (LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_HYSTERISIS == timerId)
  {
      AON_MSG_MED("aonTimerHandler invoked for AonDB session status hyst Id %d",timerId,0,0);
      g_aonDatabase->timerHandler();
  }
  else
  {
     uint32 reqId = timerId - LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MIN;
     AON_MSG_MED("aonTimerHandler invoked reqId = %d", reqId,0,0);
     AonTransac *aonTransac = g_aonDatabase->getAonTransacByReqId(reqId);
     if( NULL != aonTransac)
     {
   	   aonTransac->timerHandler();
     }
     else
     {
   	   AON_MSG_ERROR("aonTimerHandler transaction not found for reqId = %d", reqId,0,0);
     }
  }
}


