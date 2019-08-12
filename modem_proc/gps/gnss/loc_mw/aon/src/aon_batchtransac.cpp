/*============================================================================
  @file aon_batchtransac.cpp

  @brief
    This file contains the implementation for an AON batching transaction
 
               Copyright (c) 2014 - 2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential.
			   Copyright (c) 2015-2016 Qualcomm Technologies, Inc. All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_batchtransac.cpp#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
03/28/16    mk    Cached position Reset during LB Session Start
08/13/15    sj    Added default configuration of GM from NV 
04/06/15    sj    LB 2.0 integration
10/23/14    ns    Initial creation of file.


=============================================================================*/
/*****************************************************************************
 * Include Files
 * *************************************************************************/
#include "aon_internal.h"
#include "aon_batchtransac.h"
#include "aon_gmproxy.h"
#include "aon_db.h"
#include "aries_os_api.h"
#include "loc_api_internal.h"

#define BATCHING_MODE_TIME_BASED_MAX_PUNC_ACCEPTABLE (500)  // 500 m

#define BATCHING_MAX_TIME_AND_DIST_TIMER (10) // 10 seconds

#define BATCHING_CPI_RATE_TIME_ONLY (300)  //5mins

AonBatchTransac:: AonBatchTransac(
  uint32 reqId, uint32 minInterval, uint32 minDistance, 
  uint32 batchingConfigMask, uint32 fixTimeout, 
  uint32 accuracy, aonBatchingMode mode,
  AonDatabase* aonDbHdl, AonGmProxy *aonGmProxy,
  aonLocationCallback locCb, aonStatusCallback statusCb) : 
  AonTransac(reqId, aonDbHdl, aonGmProxy, locCb, statusCb), 
	mMinInterval(minInterval), mMinDistance(minDistance),
	mBatchingConfigMask(batchingConfigMask), mFixTimeout(fixTimeout),
	mAccuracy(accuracy), mMode(mode), mTimer(NULL), mTimerExpired(FALSE)                
{ }
/* 
    @brief Retrieve the equivalent background engine client config for AON
    @param[in] gmConfig Background engine client level config object
    @return TRUE or FALSE
    */
boolean AonBatchTransac::getEqGmClientConfig(AonGmClientConfig &gmConfig)
{
  const aonGmNvDefaultConfigType z_nvConfig = mAonGmProxy->getGmNvDefaultConfig();
  
  gmConfig.mGmBackOffMax = z_nvConfig.q_backoffMax; 
  
  gmConfig.mGmBackOffMin = z_nvConfig.q_backoffMin;
  
  gmConfig.mGnssUnavailAggresiveExit = FALSE;

  gmConfig.mSessionTimeout = (mFixTimeout/1000);

  gmConfig.mCpiReqRate =  z_nvConfig.q_cpiReqRate;

  if(AON_BATCHING_MODE_TIME_ONLY == mMode)
  {
	uint32 intervalInSecs = (mMinInterval/1000);	
	gmConfig.mCpiReqRate =  BATCHING_CPI_RATE_TIME_ONLY;
	gmConfig.mGmBackOffMax = intervalInSecs;
    gmConfig.mGmBackOffMin = intervalInSecs;
  }
  /* If CPI is disabled by the NV then set the CPI Req rate to max value  */
  if(mBatchingConfigMask & AON_BATCHING_CONFIG_MASK_CPI_REQ_RATE_MAX)
  {
	gmConfig.mCpiReqRate = AON_GM_CONFIG_MAX_CPI_REQ_RATE;
  }

  
  AON_MSG_MED("AonBatchTransac : getEqGmClientConfig gmConfig bo min = %d, bo max = %d, session timeout = %d", 
	gmConfig.mGmBackOffMin, gmConfig.mGmBackOffMax, gmConfig.mSessionTimeout);
  
  AON_MSG_MED("AonBatchTransac : getEqGmClientConfig gmConfig cpi req rate = %d", gmConfig.mCpiReqRate, 0, 0);

  AON_MSG_MED(" Default motion sensing dist GPS: %d, CPI %d", 
	z_nvConfig.q_motionSensingDistGps , 
    z_nvConfig.q_motionSensingDistCpi, 
	0);
  return TRUE;
}
/* 
    @brief Retrieve the equivalent geofence config for the AON transaction
    @param[out] gfConfig GF Config object that needs to be populated
    @return TRUE or FALSE
    */
boolean AonBatchTransac::getEqGeofenceConfig(AonGeofenceConfig &gfConfig)
{
  if(mBatchingConfigMask & AON_BATCHING_CONFIG_MASK_DISABLE_CONTEXT_BASED_BO)
  {
	gfConfig.mGfContextUsageFlag = FALSE; 
  }
  else
  {
	gfConfig.mGfContextUsageFlag = TRUE;
  }
    
  gfConfig.mGfBreachEventType = (sm_gm_geofence_event_type)( 
	  GEOFENCE_EVENT_ENTERING_AREA |
	  GEOFENCE_EVENT_LEAVING_AREA  |
	  GEOFENCE_EVENT_INSIDE_AREA   |
	  GEOFENCE_EVENT_OUTSIDE_AREA  );

  gfConfig.mGfResponsivenessInSec = (mMinInterval/1000);
  gfConfig.mResponsiveness = GEOFENCE_RESPONSIVENESS_CUSTOM;
  gfConfig.mGfRadius = mMinDistance;
  gfConfig.mGfForceResponsiveness = FALSE;
  gfConfig.b_force_use_cached_fix = TRUE;

  if(mMode == AON_BATCHING_MODE_TIME_ONLY)
  {
	/* For time only mode : radius is set to accuracy scaled by 99% and
	   max Punc acceptable is set to 500 m*/
	gfConfig.mGfRadius = AON_SCALE_HEPE_63_TO_99_CONF(mAccuracy);
	//accuracy only valid for legacy time based operation
	gfConfig.mGfAccuracy = BATCHING_MODE_TIME_BASED_MAX_PUNC_ACCEPTABLE;
	gfConfig.mGfForceResponsiveness = TRUE; 
  }
  else if(mMode == AON_BATCHING_MODE_TIME_AND_DISTANCE)
  {
	if(mTimerExpired)
	{
	  /* If minInterval time already expired the set the responsiveness so that breach is sent soon after distance
	  criteria is met */
	  if(gfConfig.mGfResponsivenessInSec > BATCHING_MAX_TIME_AND_DIST_TIMER)
	  {
		gfConfig.mGfResponsivenessInSec = BATCHING_MAX_TIME_AND_DIST_TIMER;
	  }
	}
  }
  
  AON_MSG_HIGH("AonBatchTransac : getEqGeofenceConfig  GF radius = %d, responsiveness sec = %d, context enabled = %d", 
	gfConfig.mGfRadius, gfConfig.mGfResponsivenessInSec, gfConfig.mGfContextUsageFlag);
  
  AON_MSG_HIGH("AonBatchTransac : getEqGeofenceConfig GF mode = %d, accuracy = %d, force responsiveness = %d", 
	mMode, gfConfig.mGfAccuracy, gfConfig.mGfForceResponsiveness );
  return TRUE;
}
/* 
    @brief Breach response handler fn invoked by gmproxy 
    @param[in] gfId Geofence Id
    @param[in] breachType Type of breatch
    @param[in] location  Lcoation object instance adssocited witht his breach
    @return TRUE or FALSE
    */
boolean AonBatchTransac::breachRspHandler(uint32 gfId, aonGMBreachType breachType, locEngPositionReportMsgT_v01 &location)
{
  AON_MSG_MED("AonBatchTransac breachRspHandler invoked, mode = %d", mMode, 0,0);

  /* Ignore breaches right after the replace */
 
  /* Replace breaches come before replace response handler, uncomment after that behavior is fixed 
  if(TRUE == mReplaceFixPending)
  {
    AON_MSG_HIGH("Ignoring first breach received after replace",0,0,0);
    mReplaceFixPending = FALSE;
    return TRUE;
  }
  */

  if(mMode == AON_BATCHING_MODE_TIME_AND_DISTANCE)
  {
    /* stop the timer, shall be restarted when the geofence is added/replaced */
    os_TimerStop(mTimer);
  }

/* for adaptive and time only geofences, batch the fix and replace the geofence with each breach */
  if((mMode == AON_BATCHING_MODE_AUTO) ||    /* LB 1.5 Behavior */
     (mMode == AON_BATCHING_MODE_TIME_ONLY)) /* LB 1.0 Behavior */
  {
    //send the position for batching, let batching take care of timing issues for batch all
    //or no batch all.
    mAonLocCb(mReqId, &location, AON_POSITION_TYPE_TRACKING);
      
    /* always replace geofence even if the fix is not batched to make sure the next fix
       is scheduled as per the criteria */
    replaceGF(gfId, location.latitude, location.longitude);
  }   
/* for time and distance, store the origin of the Geofence (to be used if the min Interval timer expires)
     and replace only on OUTSIDE OR EXIT breach */
  else if(mMode == AON_BATCHING_MODE_TIME_AND_DISTANCE || mMode == AON_BATCHING_MODE_DISTANCE_ONLY)
  {
    mLastGfLat = location.latitude;
    mLastGfLon = location.longitude;
    if((breachType == AON_GEOFENCE_EVENT_OUTSIDE_AREA) ||
       (breachType == AON_GEOFENCE_EVENT_LEAVING_AREA))
    {
      //send the position for batching
      mAonLocCb(mReqId, &location, AON_POSITION_TYPE_TRACKING);
      
      /* always replace geofence on the exit breach, otherwise
         there can be cases where the geofence is not around the user and no fix is generated */
      replaceGF(gfId, location.latitude, location.longitude);
    }
    else if((mBatchingConfigMask & AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES))
    {
      /* Send the position for batching even if inside breach if the batch all 
       * flag is set. No need to replace GF as the user is still inside */
      mAonLocCb(mReqId, &location, AON_POSITION_TYPE_TRACKING);
    }
  }

  return TRUE;
}
/* 
    @brief Timer handler
    @param[in] void
    @return void
    */
void AonBatchTransac :: timerHandler()
{
  AON_MSG_HIGH("AonBatchTransac :: timerHandler, timer expiry!", 0,0,0);
  os_TimerStop(mTimer);
    /* breach wasn't received within the minInterval time, for minDist+minTime case
      change the custom responsiveness to the specified minInterval */
  if( mMode == AON_BATCHING_MODE_TIME_AND_DISTANCE)
  {
	 mTimerExpired = TRUE;
	(void)replaceGF(mGfId, mLastGfLat, mLastGfLon); 
  }
  return;
}
/* 
    @brief Starts the session for the AON transaction
    @param[in] void
    @return AON_SUCCEEDED or AON_FAILED
    */
aonOperationStatusType AonBatchTransac :: start()
{
  aonOperationStatusType status = AON_FAILED;
  // Reset the cached Position during start of a batch session
  mAonGmProxy->m_lastBreachLatInDeg = 0;
  mAonGmProxy->m_lastBreachLonInDeg = 0;
  AON_MSG_HIGH("AonBatchTransac ::start : Cached position Reset during LB Session Start!", 0,0,0);
  status = AonTransac::start();
  if( AON_SUCCEEDED == status && 
	mMode == AON_BATCHING_MODE_TIME_AND_DISTANCE)
  {
	mLastGfLat = mAonGmProxy->m_lastBreachLatInDeg;
	mLastGfLon = mAonGmProxy->m_lastBreachLonInDeg;
	mTimer = os_TimerCreate((mReqId + LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MIN) , THREAD_ID_LOC_MIDDLEWARE);
	if( NULL == mTimer)
	{
	  status = AON_FAILED;
	}
  }

  return status;
}
 /* 
    @brief Stops the session for the AON transaction
    @param[in] void
    @return AON_SUCCEEDED or AON_FAILED
    */
aonOperationStatusType AonBatchTransac :: stop()
{
  if (NULL != mTimer)
  {
	os_TimerStop(mTimer);
	(void)os_TimerDestroy(mTimer);
  }
  return (AonTransac::stop());
}
/* 
    @brief Add handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the add request to background engine
    @return TRUE or FALSE
    */
boolean AonBatchTransac :: addGfRspHandler(uint32 txId, uint32 gfId, boolean result)
{
  if(TRUE == result && mMode == AON_BATCHING_MODE_TIME_AND_DISTANCE)
  {
	uint32 responsivenessInSec = (mMinInterval/1000);
	if( responsivenessInSec > BATCHING_MAX_TIME_AND_DIST_TIMER)
	{
	  AON_MSG_MED("AonBatchTransac :: addGfRspHandler starting timer with interval = %d secs", mMinInterval,0,0);
	  (void)os_TimerStart(mTimer, mMinInterval, 0);
	}
  }
  return (AonTransac::addGfRspHandler(txId, gfId, result));
}
/* 
    @brief Replace geofence handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the delete request to background engine
    @return TRUE or FALSE
    */
boolean AonBatchTransac :: replaceGfRspHandler(uint32 txId, uint32 gfId, boolean result)
{
  if(TRUE == result && mMode == AON_BATCHING_MODE_TIME_AND_DISTANCE)
  {
	uint32 responsivnessInSec = (mMinInterval/1000);
	/* Start the timer only for a replace invoked after breach and not for replace invoked
	   after timer expiry */
	   
	if(TRUE == mTimerExpired)
	{
	  mTimerExpired = FALSE;
	}
	/* start a timer only if responsiveness is greater than max threshold*/
	else if(responsivnessInSec > BATCHING_MAX_TIME_AND_DIST_TIMER)
	{
	   AON_MSG_MED("AonBatchTransac :: addGfRspHandler starting timer with interval = %d secs", mMinInterval,0,0);
	  (void)os_TimerStart(mTimer, mMinInterval, 0);
	}
  }
  return (AonTransac::replaceGfRspHandler(txId, gfId, result));
}


 

