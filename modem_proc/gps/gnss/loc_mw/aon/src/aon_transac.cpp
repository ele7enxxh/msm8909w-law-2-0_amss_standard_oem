/*============================================================================
  @file aon_transac.cpp

  @brief
    This file contains the implementation for the GM Proxy abstraction for GM API
 
               Copyright (c) 2014 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential.
			   Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_transac.cpp#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  -----------------------------------------------------------
10/21/15    sudj  Fix Crash caused by bad AON configuration 
04/06/15    sj    LB 2.0 integration 
02/25/15    sj    Fixed status indications for client specfic cases  
09/17/14    sj    Initial creation of file.


=============================================================================*/
/*****************************************************************************
 * Include Files
 * *************************************************************************/
#include "aon_transac.h"
#include "aon_gmproxy.h"

/* 
   @brief Starts the session for the AON transaction
   @param[in] void
   @return AON_SUCCEEDED or AON_FAILED
   */
aonOperationStatusType AonTransac::start()
{
  AonGeofenceConfig aonGfConfig;
  if(getEqGeofenceConfig(aonGfConfig))
  {
	//Initial location saved from last breach . 
	aonGfConfig.mGfLatInWGS_84 = AON_CONVERT_INTO_WGS84(mAonGmProxy->m_lastBreachLatInDeg);
	aonGfConfig.mGfLonInWGS_84 = AON_CONVERT_INTO_WGS84(mAonGmProxy->m_lastBreachLonInDeg);
	aonGfConfig.b_force_use_cached_fix = FALSE;
	if(mAonGmProxy->addGeofence(aonGfConfig, getReqId()))
	{
	  return AON_SUCCEEDED;
	}
  }
 
  AON_MSG_ERROR("AonTransac::start failed", 0, 0, 0);
  return AON_FAILED;
}
/* 
    @brief Replaces the geofence for the AON transaction
    @param[in] void
    @return TRUE or FALSE
    */
boolean AonTransac::replaceGF(
  uint32 gfId, double latInDeg, double lonInDeg)
{
  AON_MSG_MED("replaceGF invoked", 0, 0, 0);

  AonGeofenceConfig aonGfConfig;

  if(getEqGeofenceConfig(aonGfConfig))
  {
	aonGfConfig.mGfLatInWGS_84 = (sint31)( ( latInDeg * (1<<25) )/180 );
	aonGfConfig.mGfLonInWGS_84 = (sint31)( ( lonInDeg * (1<<25) )/180 );
	
	if(mAonGmProxy->replaceGeofence(aonGfConfig, getReqId(), gfId))
	{
	  clearGfId();
	  return TRUE;
	}
  }
  AON_MSG_ERROR("AonTransac::replace failed", 0, 0, 0);
  return FALSE;
}
/* 
    @brief Stops the session for the AON transaction
    @param[in] void
    @return AON_SUCCEEDED or AON_FAILED
    */
aonOperationStatusType AonTransac::stop()
{
  if( validGfId())
  {
	mAonGmProxy->delGeofence(getGfId(), getReqId());
	clearGfId();
  } 
  else
  {
	AON_MSG_ERROR("stop: GF not valid",0,0,0);
  }

  mFirstFixPending = FALSE;
  
  //stop always succeeds
  return AON_SUCCEEDED;
}
/* 
    @brief Add handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the add request to background engine
    @return TRUE or FALSE
    */
boolean AonTransac::addGfRspHandler(uint32 txId, uint32 gfId, boolean result)
{
  AON_MSG_MED("addGfRspHandler invoked. txId %d, gfId %d, result %d", txId, gfId, result);
  if( TRUE == result)
  {
	setGfId(gfId);
  }
  
  //Check state and send start success indication
  mAonStatusCb(txId, ((TRUE==result)? AON_START_SUCCEEDED: AON_START_FAILED));
  
  if(TRUE==result)
  {
    this->mFirstFixPending = TRUE;
  

  /*Check if a Unable to track event has already been reported for previous AON request
    * If so we need to resend the associated indication for the new request as GM will not
    * send additional Unable to track events for new Geofences added
    */
    if(mAonGmProxy->getUnableToTrackReported())
    {
      AON_MSG_MED("addGfRspHandler. UnableToTrack already reported", 0, 0, 0);
      mAonStatusCb(txId, AON_UNABLE_TO_TRACK);
    }
  }
  return TRUE;
}
/* 
    @brief Status call back handler invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] status Status of the background engine
    @return TRUE or FALSE
    */
void AonTransac::statusCallbackHandler(uint32 txId, aonSessionStatusType status)
{
  AON_MSG_MED("statusCallbackHandler invoked. status %d", status,0,0);
  
  mAonStatusCb(txId,status);
}
/* 
    @brief Delete handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the delete request to background engine
    @return TRUE or FALSE
    */
boolean AonTransac::delGfRspHandler(uint32 txId, uint32 gfId, boolean result)
{
  AON_MSG_MED("delGfRspHandler invoked. txId %d, gfId %d, result %d", txId, gfId, result);
  return TRUE;
}
/* 
    @brief Replace geofence handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the delete request to background engine
    @return TRUE or FALSE
    */
boolean AonTransac::replaceGfRspHandler(uint32 txId, uint32 gfId, boolean result)
{
  AON_MSG_MED("replaceGfRspHandler invoked. txId %d, gfId %d, result %d", txId, gfId, result);
  if( mGfId  != gfId)
  {
	AON_MSG_ERROR("replaceGfRspHandler new Gf ID %d different from old Gf ID %d, result %d", gfId, mGfId, result);
  }
  if( TRUE == result)
  {
	setGfId(gfId);
  }
  return TRUE;
}
/* 
    @brief Timer handler
    @param[in] void
    @return void
    */
void AonTransac::timerHandler()
{
  /* does nothing in the base class*/
  return;
}
