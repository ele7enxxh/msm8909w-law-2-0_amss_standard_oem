/*============================================================================
@file aon_gmproxy.cpp

  @brief
    This file contains the implementation for the GM Proxy abstraction for GM API
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_gmproxy.cpp#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
09/30/15    sj    Added hysterisis for unsolicited status indications 
08/13/15    sj    Added default configuration of GM from NV 
04/06/15    sj    LB 2.0 integration 
03/11/15    sj    Fixed swapped values for semiMajor & semiMinor hor Uncertainty
03/02/15    sj    Use replaceGf() API from GM instead of Del/Add seq 
02/25/15    sj    Fixed status indications for client specfic cases 
02/19/15    sj    Removed CPI rate configuration from AON  
02/10/15    sj    Configured max geofences needed by AON from GM
01/30/15    sj    Fixed DOP & Sv Used Info transalation for DBT 
01/19/15    sj    Canned responsiveness to UHR for maxLatency = 1 sec 
09/17/14    sj    Initial creation of file.


=============================================================================*/
/*****************************************************************************
* Include Files
* *************************************************************************/
#include "math.h"
#include "comdef.h"
#include "gnss_calendar_util.h"
#include "aon_internal.h"
#include "aon_gmproxy.h"
#include "aon_transac.h"
#include "loc_api_internal.h"
#include "aon_internal.h"
#include "gm_api.h"
#include "tm_common.h"
#include "gps_common.h"

AonGmProxy *AonGmProxy::m_pInstance = 0;
sm_gm_client_type gmClient; // Identifier for the GMProxy client for GM

/* 
   @brief Overloaded constructor- Creates an singleton instance of AonGmProxy object
   @param[in] p_nvConfig Object containing the NV default values that are needed to register with GM
   */
AonGmProxy::AonGmProxy(const aonGmNvDefaultConfigType *p_nvConfig):m_nvConfig(*p_nvConfig)
{
  uint8 errorCode;
  sm_gm_set_client_config_type gfClientConfig = {0};
  AON_MSG_MED("AonGmProxy  CTOR invoked", 0,0,0);
  
  m_aonDbHdl = AonDatabase::Instance();
 
  gmClient.client_id = GEOFENCE_CLIENT_AON;

  gmClient.q_client_thread_id = THREAD_ID_LOC_MIDDLEWARE;
  gmClient.q_base_msg_id = LM_MIDDLEWARE_MSG_ID_AON_MIN;
  gmClient.client_cap.client_capabilites = GEOFENCE_CLIENT_GEOFENCE_SERVICE_REQ;

  gfClientConfig.q_mask = (SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK|
                              SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK |
                              SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK |
                              SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK |
                              SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK |
                              SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_GPS_MASK |
                              SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_CPI_MASK |
                              SM_GM_CLIENT_CONFIG_GNSS_UNAVAIL_AGGR_EXIT_MASK);
  gfClientConfig.q_max_geofences = AON_DB_MAX_GF_TRANSACTIONS * 2;
  gfClientConfig.q_gnss_pos_qos_sess_timeout = p_nvConfig->q_gnssPositionQosSessionTimeout;
  gfClientConfig.q_cpi_request_rate = p_nvConfig->q_cpiReqRate;
  gfClientConfig.q_chal_gps_env_bo_min =  p_nvConfig->q_backoffMin;
  gfClientConfig.q_chal_gps_env_bo_max =  p_nvConfig->q_backoffMax;
  gfClientConfig.q_chal_gps_env_ms_dist_gps =  p_nvConfig->q_motionSensingDistGps;
  gfClientConfig.q_chal_gps_env_ms_dist_cpi = p_nvConfig->q_motionSensingDistCpi;	 
  gfClientConfig.gnss_unavail_aggresive_exit = FALSE;
  
  if(gm_client_register(&gmClient,&gmHandle,&gfClientConfig,&errorCode) == FAILED)
  {
	AON_MSG_ERROR("Not able to add AON GmProxy as Geofence client to GM, errorCode %d", 
	  errorCode,0,0 );
  }

  mUnableToTrackReported = FALSE;  
}

/* 
   @brief Overloaded constructor- Creates an singleton instance of AonGmProxy object
   @param[in] None
   */
AonGmProxy * AonGmProxy::Instance()
{
  AON_MSG_MED("AonGmProxy  Instance invoked", 0,0,0);
  if (NULL != m_pInstance)   
  {
	return m_pInstance;
  } 
  else
  {
    AON_MSG_ERROR("AonGmProxy - Null instance", 0, 0, 0 );
    return NULL;
  }
}

/* 
   @brief Overloaded constructor- Creates an singleton instance of AonGmProxy object
   @param[in] p_nvConfig Object containing the NV default values that are needed to register with GM
   */
AonGmProxy * AonGmProxy::Instance(const aonGmNvDefaultConfigType *p_nvConfig)
{
  AON_MSG_MED("AonGmProxy  Instance with config invoked", 0,0,0);
  if (!m_pInstance)   
  {
	m_pInstance = new AonGmProxy(p_nvConfig);

  } 

  return m_pInstance; 

}
/* 
    @brief Transalates geofence config info into Background engine format 
    @param[in] aonGmConfig AON geofence config object 
    @param[out] geofence Background engine genfence sepcification object
    @return TRUE or FALSE
    */
void AonGmProxy::xlateGeofenceConfig(AonGeofenceConfig &aonGfConfig, sm_gm_geofence_type &geofence)
{
  // Defaults not configured by AON clients
  geofence.start_time = 0;
  geofence.stop_time = CGPS_MAX_UINT32;
  geofence.confidence = GEOFENCE_CONFIDENCE_MEDIUM;
  geofence.target_area_provided = TRUE;
  geofence.target_area.target_area_type = GEOFENCE_TARGET_AREA_CIRCULAR_TYPE;
  geofence.num_of_area_ids = 0;
  geofence.area_id_list = NULL;
  geofence.max_num_reports = CGPS_MAX_UINT32;
  geofence.client_blob = NULL;
  geofence.client_blob_size = 0;
  geofence.responsiveness =  GEOFENCE_RESPONSIVENESS_CUSTOM;
  geofence.location_estimate = TRUE;
  geofence.hysteresis_required = FALSE;
  geofence.dynamic_responsiveness = FALSE;
  geofence.event_type = aonGfConfig.mGfBreachEventType;  
  geofence.context_usage = 
	aonGfConfig.mGfContextUsageFlag;	
  geofence.q_custom_responsiveness_secs = 
	aonGfConfig.mGfResponsivenessInSec;
  geofence.responsiveness =  aonGfConfig.mResponsiveness;
  geofence.target_area.u.circ_target_area.latitude = 
	aonGfConfig.mGfLatInWGS_84;
  geofence.target_area.u.circ_target_area.longitude = 
	aonGfConfig.mGfLonInWGS_84;
  geofence.target_area.u.circ_target_area.radius = aonGfConfig.mGfRadius;
  if(aonGfConfig.mGfAccuracy != AON_GEOFENCE_CONFIG_INVALID_ACCURACY)
  {
	geofence.gnss_max_pos_unc_acceptable = aonGfConfig.mGfAccuracy;
	geofence.gnss_max_pos_unc_acceptable_valid = TRUE;
  }
  geofence.forced_responsiveness = aonGfConfig.mGfForceResponsiveness;
  geofence.b_force_use_cached_fix = aonGfConfig.b_force_use_cached_fix;
}
/* 
   @brief Adds a geofence using Background Engine APIs
   @param[in] aonGfConfig Equivalent geofence config for the AON transaction
   @param[in] txId  transaction Id
   @return TRUE or FALSE
   */
boolean AonGmProxy::addGeofence(AonGeofenceConfig &aonGfConfig, uint32 txId)
{
  sm_gm_add_request_type gmAddRequest ={0};
  uint8 u_error_code = GEOFENCE_ERROR_NONE;

  // Geofence params configured by the AON clients
  gmAddRequest.q_transaction_id = txId;
  gmAddRequest.client_handle = gmHandle;
  xlateGeofenceConfig(aonGfConfig, gmAddRequest.geofence);  

  if(gm_add_geofence(&gmAddRequest, &u_error_code) == FAILED)
  {
	AON_MSG_ERROR("Not able to add Geofence, error_code %d", u_error_code, 0,0);
	return FALSE; 
  }
  else
  {          
	AON_MSG_MED("Succesfully sent request to add Geofence, GM TID:: %d",
	  gmAddRequest.q_transaction_id,0,0);
	return TRUE; 
  }

}
 /* 
    @brief Background engine events handler 
    @param[in] q_MsgId Message Id passed in
    @param[in] p_data Payload
    @return TRUE or FALSE
    */
boolean AonGmProxy::gmEventHandler(uint32 q_MsgId, uint8 *p_data)
{
  AON_MSG_MED("gmEventHandler, Msg Id:: %d",q_MsgId, 0, 0);
  boolean retVal = FALSE;

  switch(q_MsgId)
  {
  case GEOFENCE_ADD_NOTIFY:
	/*GM notification for Add request*/
	retVal = addGfRspHandler((sm_gm_client_notification_type*)p_data );
	break;

  case GEOFENCE_BREACH_NOTIFY:
	/*GM notification for Geofence Breach*/
	retVal = breachGfRspHandler((sm_gm_client_breach_notify_type*)p_data);
	break;

  case GEOFENCE_UNSOL_EVENT_NOTIFY:
	/* GM notification for unsolicited events */
	retVal = unsolEventHandler((sm_gm_client_unsol_event_notify_type*)p_data);
	break;

  case GEOFENCE_DELETE_NOTIFY:
	/*GM notification for delete request*/
	retVal = delGfRspHandler((sm_gm_client_notification_type*)p_data );
	break;

  case GEOFENCE_SET_ENGINE_CONFIG_NOTIFY:
    /*GM notification for setEngineConfig request*/
    if(((sm_gm_client_notification_type*)p_data)->notification.return_code
 	   == GEOFENCE_ERROR_NONE)
 	{
 	  AON_MSG_MED("setEngineConfig was successfull",0, 0, 0);
 	}
    else
 	{
 	  AON_MSG_ERROR("setEngineConfig failed",0, 0, 0);
 	}
    break;

  case GEOFENCE_PROXIMITY_NOTIFY:
    AON_MSG_MED("PROXIMITY_NOTIFY",0, 0, 0);
	break;

  case GEOFENCE_REPLACE_NOTIFY: 
	/* GM notification for replace geofence */
	retVal = replaceGfRspHandler((sm_gm_client_notification_type*)p_data );
	break;

  default:
	AON_MSG_MED("gmEventHandler unknown event type : %d",q_MsgId, 0, 0);
	break;
  }//end switch

  return retVal;
}
/* 
    @brief Add handler fn invoked by background engine 
    @param[in] notifyData Add notification data
    @return TRUE or FALSE
    */
boolean AonGmProxy::addGfRspHandler(sm_gm_client_notification_type* notifyData)
{
  AON_MSG_MED("addGfRspHandler",0,0,0);
  AonTransac *aonTransac = m_aonDbHdl->getAonTransacByReqId(notifyData->q_transaction_id);
  
  if( NULL == aonTransac)
  {
	AON_MSG_ERROR("AonGmProxy:: addGfRspHandler did not find AON transaction id, deleting gf id %d",
	  notifyData->q_transaction_id,notifyData->notification.geofence_id,0);
	/* delete the geofence which got added, it could have been added as a result of stop/replace race condition */
	delGeofence(notifyData->notification.geofence_id, notifyData->q_transaction_id);
	return FALSE;
  }

  if(notifyData->notification.return_code == GEOFENCE_ERROR_NONE)
  {
	aonTransac->addGfRspHandler(notifyData->q_transaction_id, 
	  notifyData->notification.geofence_id, TRUE);
  }
  else 
  {
	AON_MSG_ERROR("addGfRspHandler returned error : %d",
	  notifyData->notification.return_code, 0, 0);
	aonTransac->addGfRspHandler(notifyData->q_transaction_id, 
	  notifyData->notification.geofence_id, FALSE);
  }

  return TRUE;
}
/* 
    @brief Unsolicited handler fn invoked by background engine 
    @param[in] notifyData Unsolicited events notification data
    @return TRUE or FALSE
    */
boolean AonGmProxy::unsolEventHandler(sm_gm_client_unsol_event_notify_type* notifyData)
{
   aonSessionStatusType sessionStatus = AON_GNL_FAILURE;
   if (NULL == notifyData)
   {
     AON_MSG_ERROR("unsolEventHandler NULL ptr",
                   0,0,0);
     return FALSE;
   }
   
   AON_MSG_MED("unsolEventHandler tx ID = %d, event = %d", 
	notifyData->q_transaction_id, notifyData->unsol_event,0);

   switch(notifyData->unsol_event)
   {
     case GEOFENCE_UNSOL_EVENT_GPS_LOST:
	 {
	   sessionStatus = AON_GNSS_UNAVAIL;
	   break;
	 }
	 case GEOFENCE_UNSOL_EVENT_GPS_AVAIL:
	 {
	   sessionStatus = AON_GNSS_AVAIL;
	   break;
	 }
	 case GEOFENCE_UNSOL_EVENT_ABLE_TO_TRACK:
	 {
	   sessionStatus = AON_ABLE_TO_TRACK;
		 mUnableToTrackReported = FALSE;
	   break;
	 }
	 case GEOFENCE_UNSOL_EVENT_UNABLE_TO_TRACK:
	 {
	   sessionStatus = AON_UNABLE_TO_TRACK;
		 mUnableToTrackReported = TRUE;
	   break;
	 }
	 default:
	 {
	   AON_MSG_HIGH("unsolEventHandler. Did not handle this event", 
	                 0, 0, 0);
	   break;
	 }
   }
   
   if(sessionStatus != AON_GNL_FAILURE)
	   m_aonDbHdl->broadcastStatusInfo(sessionStatus);

   return TRUE;  

}
/* 
    @brief Breach event handler fn invoked by background engine 
    @param[in] notifyData Breach events notification data
    @return TRUE or FALSE
    */
boolean AonGmProxy::breachGfRspHandler(sm_gm_client_breach_notify_type* notifyData)
{
  AON_MSG_MED("breachGfRspHandler. GFID: %d BreachType: %d",
	notifyData->geofence_id , notifyData->breach_type,0);

  aonGMBreachType breachType = AON_GEOFENCE_EVENT_MAX;

  /* Ack the breach */
  sm_gm_breach_ack_notify_type gmAck = {0};
  gmAck.accepted = TRUE;
  gmAck.geofence_id = notifyData->geofence_id;
  (void)gm_breach_ack(&gmAck, NULL);

  switch(notifyData->breach_type)
  {
  case GEOFENCE_EVENT_ENTERING_AREA:
	breachType = AON_GEOFENCE_EVENT_ENTERING_AREA;
	break;
  case GEOFENCE_EVENT_LEAVING_AREA:
	breachType = AON_GEOFENCE_EVENT_LEAVING_AREA;
	break;
  case GEOFENCE_EVENT_INSIDE_AREA :
	breachType = AON_GEOFENCE_EVENT_INSIDE_AREA;
	break;
  case GEOFENCE_EVENT_OUTSIDE_AREA :
	breachType = AON_GEOFENCE_EVENT_OUTSIDE_AREA;
	break;
  default:
	AON_MSG_ERROR("breachGfRspHandler. Unexpected breach event",0,0,0);

	break;
  }	 

  locEngPositionReportMsgT_v01 location;

  //Translate the position information to AON Pos Struct
  if(notifyData->fix_included)
  {
	if (TRUE == xlatePositionRpt(&notifyData->pos_fix_rpt,location))
	{
	  location.sessionStatus = eLOC_ENG_POS_SESS_STATUS_SUCCESS_V01;
      
	  /* overwrite  the last known lat, lon */
	  if(TRUE == location.latitude_valid)
	  {
		m_lastBreachLatInDeg = location.latitude;
	  }
	  if(TRUE == location.longitude_valid)
	  {
		m_lastBreachLonInDeg = location.longitude;
	  }
	  
	  AonTransac *aonTransac = m_aonDbHdl->getAonTransacByGfId(notifyData->geofence_id);
	  
	  if( NULL == aonTransac)
	  {
		AON_MSG_ERROR(" breachRspHandler did not find AON transaction",0,0,0);
	  }
	  else
	  {
		return aonTransac->breachRspHandler(notifyData->geofence_id, breachType, location);
	  }
	}
	else
	{
	  AON_MSG_ERROR("breachGfRspHandler. breach position could not be translated",
		0,0,0);
	}
  } 
  else
  {
	AON_MSG_ERROR("breachGfRspHandler. breach position not included",0,0,0);
  }
  
  return FALSE;
}
 /* 
   @brief Deletes a geofence using Background Engine APIs
   @param[in] gfId Geofence Id
   @param[in] txId  transaction Id
   @return TRUE or FALSE
   */
boolean AonGmProxy::delGeofence( uint32 gfId, uint32 txId)
{
  sm_gm_delete_request_type gmDelRequest={0};
  gmDelRequest.client_handle = gmHandle;
  gmDelRequest.q_transaction_id = txId;
  gmDelRequest.geofence_id = gfId;
  uint8 u_error_code;

  if ( gm_delete_geofence(&gmDelRequest, &u_error_code) == 
	FAILED )
  {
	AON_MSG_ERROR( "Failure of geofence delete: ID %d, error=%u", 
	  gfId, u_error_code, 0);
	return FALSE; 

  }

  AON_MSG_MED("Succesfully sent request for delete GfId %d TxID %d",  gfId, txId, 0);
  return TRUE;

}
/* 
    @brief Delete request response event handler fn invoked by background engine 
    @param[in] notifyData Delete request response notification data
    @return TRUE or FALSE
    */
boolean AonGmProxy::delGfRspHandler(sm_gm_client_notification_type* notifyData)
{
  AON_MSG_MED("delGfRspHandler tx id %d, gf id %d", notifyData->q_transaction_id, 
               notifyData->notification.geofence_id,0);
  
  if(notifyData->notification.return_code == GEOFENCE_DELETE_ON_CLIENT_REQUEST)
  {
	AonTransac *aonTransac = m_aonDbHdl->getAonTransacByGfId(notifyData->notification.geofence_id);
  
	if( NULL == aonTransac)
	{
	  AON_MSG_ERROR("AonGmProxy:: delGfRspHandler AON GF ID  %d should've been invalidated", 
		notifyData->notification.geofence_id,0,0);
	  return FALSE;
	}

   aonTransac->delGfRspHandler(notifyData->q_transaction_id, 
	  notifyData->notification.geofence_id, TRUE);
  } 
  else 
  {
	AON_MSG_ERROR("delGfRspHandler returned unexpected value : %d, tx Id : %d",
	  notifyData->notification.return_code, notifyData->q_transaction_id, 0);
  }
  return TRUE;
}
/* 
    @brief Transalates position report from brach event provided by background engine 
    @param[in] p_gnss_fix_rpt Position information from breach event 
    @param[out] location Location object
    @return TRUE or FALSE
    */
boolean  AonGmProxy::xlatePositionRpt(sm_GnssFixRptStructType *p_gnss_fix_rpt,
  locEngPositionReportMsgT_v01 &location)
{
  uint32 i = 0, count = 0;

  memset(&location, 0, sizeof(locEngPositionReportMsgT_v01));

  if (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsValid)
  {
	// Convert the latitude from radians into degrees.
	// radians = degrees * (PI/180)
	// degrees = (radians * 180)/PI

	/* Latitude */
	location.latitude = ( p_gnss_fix_rpt->z_NavPos.d_PosLla[0] * 180 )/C_PI;
	location.latitude_valid = TRUE;

	/* Longitude */
	// Convert the longitude from radians into degrees.
	location.longitude =  ( p_gnss_fix_rpt->z_NavPos.d_PosLla[1] * 180 )/C_PI;
	location.longitude_valid = TRUE;

	/*  Altitude With Respect to Ellipsoid */
	location.altitudeWrtEllipsoid = (float)p_gnss_fix_rpt->z_NavPos.d_PosLla[2];
	location.altitudeWrtEllipsoid_valid = TRUE;

	/* horz Unc circular */
	location.horUncCircular = sqrt(
	  (p_gnss_fix_rpt->z_NavPos.f_ErrorEllipse[1] * p_gnss_fix_rpt->z_NavPos.f_ErrorEllipse[1])
	  +(p_gnss_fix_rpt->z_NavPos.f_ErrorEllipse[2] * p_gnss_fix_rpt->z_NavPos.f_ErrorEllipse[2]));
	location.horUncCircular_valid = TRUE;

	/*  Horizontal circular confidence */
	location.horCircularConfidence = 63; // Hard coded as of now
	location.horCircularConfidence_valid = TRUE;

  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
	location.horUncEllipseSemiMajor = p_gnss_fix_rpt->z_NavPos.f_ErrorEllipse[1];
	location.horUncEllipseSemiMajor_valid = TRUE;
  
	/*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
	location.horUncEllipseSemiMinor = p_gnss_fix_rpt->z_NavPos.f_ErrorEllipse[2];
	location.horUncEllipseSemiMinor_valid = TRUE;
	/*  Elliptical Horizontal Uncertainty Azimuth */
	location.horUncEllipseOrientAzimuth = p_gnss_fix_rpt->z_NavPos.f_ErrorEllipse[0];
	location.horUncEllipseOrientAzimuth_valid = TRUE;

	/*  Horizontal Elliptical Confidence */
	location.horEllipticalConfidence = 39;
	location.horEllipticalConfidence_valid = TRUE;

	/*  Vertical Uncertainty */
	location.vertUnc = p_gnss_fix_rpt->z_NavPos.f_PuncVertMeters;
	location.vertUnc_valid = TRUE;

	/*  Vertical Confidence */
	location.vertConfidence = 68; //TBD #define for 68?
	location.vertConfidence_valid = TRUE;

	/* Compute horizontal velocity from components and scale
	* 
	*/
	if( p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsVelValid)
	{
	  /* speed = sqrt ((velcity due east)^2 + (velcity due east)^2*/
	  location.horSpeed = sqrt( 
		(p_gnss_fix_rpt->z_NavPos.f_VelEnu[0]*p_gnss_fix_rpt->z_NavPos.f_VelEnu[0])  
		+ (p_gnss_fix_rpt->z_NavPos.f_VelEnu[1]*p_gnss_fix_rpt->z_NavPos.f_VelEnu[1]) );

	  location.horSpeed_valid = TRUE;

	  /* not calculating the horSpeeUnc as final output needs 3d speed Unc and it
	  is not clear how speed Unc's should be calculated from east and north Unc's
	  location.horSpeedUnc = sqrt((f_VuncEastNorthMps[0]*f_VuncEastNorthMps[0])+
	  (f_VuncEastNorthMps[0]*f_VuncEastNorthMps[0]));
	  location.horSpeedUnc_valid = TRUE;
	  */


	  /*  Vertical Speed */
	  location.vertSpeed = p_gnss_fix_rpt->z_NavPos.f_VelEnu[2];
	  location.vertSpeed_valid = TRUE;

	  /*  Vertical Speed Uncertainty */
	  location.vertSpeedUnc = p_gnss_fix_rpt->z_NavPos.f_VuncVertMps;
	  location.vertSpeedUnc_valid = TRUE;

	  /* 3D Speed Unc */
	  location.speedUnc3d = p_gnss_fix_rpt->z_NavPos.f_Vunc3dMps;
	  location.speedUnc3d_valid = TRUE;
	}

	/*  Heading */
	if(p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsHeadingValid)
	{
	  location.heading = (p_gnss_fix_rpt->z_NavPos.f_HeadingRad *180 )/C_PI;
	  location.heading_valid = TRUE;

	  /*  Heading Uncertainty */
	  location.headingUnc = (p_gnss_fix_rpt->z_NavPos.f_HeadingUncRad * 180 )/C_PI;
	  location.headingUnc_valid = TRUE;
	}

	/*  Magnetic Deviation */
	if(TRUE == p_gnss_fix_rpt->z_NavPos.u_MagDevGood)
	{
	  location.magneticDeviation = p_gnss_fix_rpt->z_NavPos.f_MagDeviation;
	  location.magneticDeviation_valid = TRUE;
	}

  /*  Technology Used */
  
	location.technologyMask = 0;
	if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsCellId)
	{
	  location.technologyMask |= LOC_ENG_POS_TECH_MASK_CELLID_V01;
	}

	if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsWifi)
	{
	  location.technologyMask |= LOC_ENG_POS_TECH_MASK_WIFI_V01;
	}

	if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsSensors)
	{
	  location.technologyMask |= LOC_ENG_POS_TECH_MASK_SENSORS_V01;
	}

	if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsSatellite)
	{
	  location.technologyMask |= LOC_ENG_POS_TECH_MASK_SATELLITE_V01;
	}

	if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsRefLoc)
	{
	  location.technologyMask |= LOC_ENG_POS_TECH_MASK_REFERENCE_LOCATION_V01;
	}

	if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsCoarsePos)
	{
	  location.technologyMask |= LOC_ENG_POS_TECH_MASK_INJECTED_COARSE_POSITION_V01;
	}

	/*Set the technology valid mask to FALSE if we could not get the technology used*/
	if (location.technologyMask != 0)
	{
	  location.technologyMask_valid = TRUE;
	}

	/*  GPS and UTC Time */
	if (p_gnss_fix_rpt->z_NavPos.w_GpsWeek == C_GPS_WEEK_UNKNOWN)
	{
	  AON_MSG_ERROR("GPS timestamp is not valid !!" ,0,0,0);	  
	  location.gpsTime_valid = FALSE;	  
	  location.timestampUtc_valid = FALSE;
	}
	else
	{

	  /* Week number of the fix */
	  location.gpsTime.gpsWeek = p_gnss_fix_rpt->z_NavPos.w_GpsWeek;
	  location.gpsTime.gpsTimeOfWeekMs = p_gnss_fix_rpt->z_NavPos.q_GpsTimeMs;
	  location.gpsTime_valid = TRUE;

	  AON_MSG_MED("AONGMProxy:GPS Week = %u, GPS Ms = %u", 
		location.gpsTime.gpsWeek, location.gpsTime.gpsTimeOfWeekMs,0);

	  /* populate time unc if the source is not only CPI, */
	  if ( location.technologyMask != LOC_ENG_POS_TECH_MASK_INJECTED_COARSE_POSITION_V01)
	  {
		location.timeUnc = p_gnss_fix_rpt->z_NavPos.f_ClockBiasUncMs;
		location.timeUnc_valid = TRUE;
	  }

	  /* calculate UTC time */

	  /* calculate the  UTC time in pdsm format */
	  utc_time z_utc_time = tm_util_construct_utc_time(
		p_gnss_fix_rpt->z_NavPos.w_GpsWeek,
		p_gnss_fix_rpt->z_NavPos.q_GpsTimeMs,
		p_gnss_fix_rpt->z_NavPos.q_UtcTimeMs);

	  pdsm_utc_time_s_type z_pdsm_utc_time = {0};
	  z_pdsm_utc_time.hour = z_utc_time.q_hours;
	  z_pdsm_utc_time.minute = z_utc_time.q_mins;
	  z_pdsm_utc_time.sec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));

	  z_pdsm_utc_time.date = (uint32) ((z_utc_time.q_utc_days * 1000000) + 
		(z_utc_time.q_month * 10000) + z_utc_time.q_year);


	  uint64 t_timestamp_utc_ms, t_timestamp_utc_s;

	  /* Convert the UTC time we have into UNIX timestamp ( Since Jan1 1970 ) */
	  gnss_GetUnixEpochFromUTC( &z_pdsm_utc_time,
		&t_timestamp_utc_s) ;

	  /* Scale it to milliseconds, as the TimeStamp now is in seconds. */
	  t_timestamp_utc_ms = t_timestamp_utc_s * 1000;
	  t_timestamp_utc_ms += (( z_pdsm_utc_time.sec % 100 ) * 10 ) ;

	  location.timestampUtc = t_timestamp_utc_ms;
	  location.timestampUtc_valid = TRUE;
	}

    if (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsGpsUsed ||
        p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsGloUsed ||
        p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsBdsUsed ) 
    {
      /* Position dilution of precision. */
      location.DOP.PDOP = p_gnss_fix_rpt->z_NavPos.f_PDOP;

      /* Horizontal dilution of precision. */
      location.DOP.HDOP = p_gnss_fix_rpt->z_NavPos.f_HDOP;

      /* Vertical dilution of precision. */
      location.DOP.VDOP = p_gnss_fix_rpt->z_NavPos.f_VDOP;
      location.DOP_valid = TRUE;
    }
   
    int i=0, q_SvListIdx = 0;
    /** convert the sv used info to gnssSvUsedList */
    /* Populate the GPS satellite info */
    for (i = 0; (i < PDSM_N_ACTIVE_GPS_CHAN) && (q_SvListIdx < LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01); i++)
    {
      /* Check whether GPS SVs were used in calculating position */
      if(p_gnss_fix_rpt->z_NavSvInfo.z_GpsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
      {
        location.gnssSvUsedList[q_SvListIdx++] =
        p_gnss_fix_rpt->z_NavSvInfo.z_GpsSvInfo[i].w_SvId;
      }
    }

    /* Populate the GLO satellite info */
    for (i = 0; (i < PDSM_N_ACTIVE_GLO_CHAN) && (q_SvListIdx < LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01); i++)
    {
      /* Check whether GLO SVs were used in calculating position */
      if(p_gnss_fix_rpt->z_NavSvInfo.z_GloSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
      {
        location.gnssSvUsedList[q_SvListIdx++] =
          p_gnss_fix_rpt->z_NavSvInfo.z_GloSvInfo[i].w_SvId;
      }
    }

    /* Populate the BDS satellite info */
    for (i = 0; (i < PDSM_N_ACTIVE_BDS_CHAN) && (q_SvListIdx < LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01); i++)
    {
      /* Check whether BDS SVs were used in calculating position */
      if(p_gnss_fix_rpt->z_NavSvInfo.z_BdsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
      {
        location.gnssSvUsedList[q_SvListIdx++] =
          p_gnss_fix_rpt->z_NavSvInfo.z_BdsSvInfo[i].w_SvId;
      }
    }

    /** set the valid flag only if the list is non-empty */
    if(q_SvListIdx > 0 )
    {
      location.gnssSvUsedList_len = q_SvListIdx ;
      location.gnssSvUsedList_valid = TRUE;
    }
    
	return TRUE;
  } // end of if (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsValid)

  return FALSE;

}
/* 
    @brief Sets the configuration for Background engine 
    @param[in] aonGmConfig \requried config
    @return TRUE or FALSE
    */
boolean AonGmProxy :: setGfEngineParam(AonGmClientConfig &aonGmConfig)
{
  uint8 err =0;
  sm_gm_set_geofence_client_config_request_type gfClientConfig = {0};
  
  gfClientConfig.q_transaction_id = 0;
  gfClientConfig.config_request.client_handle = gmHandle;
   
  gfClientConfig.config_request.q_mask = (SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK | 
                                          SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK |
               	                          SM_GM_CLIENT_CONFIG_GNSS_UNAVAIL_AGGR_EXIT_MASK| 
               	                          SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK);

  gfClientConfig.config_request.q_chal_gps_env_bo_min = aonGmConfig.mGmBackOffMin;
  gfClientConfig.config_request.q_chal_gps_env_bo_max = aonGmConfig.mGmBackOffMax;
  
  gfClientConfig.config_request.gnss_unavail_aggresive_exit = aonGmConfig.mGnssUnavailAggresiveExit;

  gfClientConfig.config_request.q_gnss_pos_qos_sess_timeout = aonGmConfig.mSessionTimeout;

  AON_MSG_MED(" Configuring GM session timeout  to %d sec, bo min to %d, bo max to %d", 
	gfClientConfig.config_request.q_gnss_pos_qos_sess_timeout, 
    gfClientConfig.config_request.q_chal_gps_env_bo_min, 
	gfClientConfig.config_request.q_chal_gps_env_bo_max);

  gfClientConfig.config_request.q_mask |= SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK ;
	gfClientConfig.config_request.q_cpi_request_rate = aonGmConfig.mCpiReqRate;
	AON_MSG_MED(" Configuring CPI req rate  to %d sec", gfClientConfig.config_request.q_cpi_request_rate ,0,0);
  	
   return (FAILED != gm_set_geofence_client_config(&gfClientConfig, &err));
 }
 /* 
   @brief Replaces a geofence using Background Engine APIs
   @param[in] aonGfConfig Equivalent geofence config for the AON transaction
   @param[in] txId  transaction Id   
   @param[in] gfId Geofence Id 
   @return TRUE or FALSE
   */
boolean AonGmProxy::replaceGeofence(AonGeofenceConfig &aonGfConfig, uint32 txId, uint32 gfId)
{
  sm_gm_replace_request_type gmRepRequest ={0};
  uint8 u_error_code;

  // Geofence params configured by the AON clients
  gmRepRequest.q_transaction_id = txId;
  gmRepRequest.client_handle = gmHandle;
  gmRepRequest.geofence_id = gfId;
  xlateGeofenceConfig(aonGfConfig, gmRepRequest.geofence);  

  if(gm_replace_geofence(&gmRepRequest, &u_error_code) == FAILED)
  {
	AON_MSG_ERROR("Not able to replace Geofence, error_code %d", u_error_code, 0,0);
	return FALSE; 
  }
  else
  {          
	AON_MSG_MED("Succesfully sent request to  Geofence, GM TID:: %d, GF Id :: %d",
	  gmRepRequest.q_transaction_id, gmRepRequest.geofence_id,0);
	return TRUE; 
  }

}
 /* 
    @brief Replace geofence handler fn invoked by Background engine
    @param[in] notifyData replace request response notification data
    @return TRUE or FALSE
    */
boolean AonGmProxy::replaceGfRspHandler(sm_gm_client_notification_type* notifyData)
{
  AON_MSG_MED("replaceGfRspHandler",0,0,0);
  AonTransac *aonTransac = m_aonDbHdl->getAonTransacByReqId(notifyData->q_transaction_id);
  
  if( NULL == aonTransac)
  {
	AON_MSG_ERROR("AonGmProxy:: replaceGfRspHandler did not find AON transaction id, deleting gf id %d",
	  notifyData->q_transaction_id,notifyData->notification.geofence_id,0);
	/* delete the geofence which got added, it could have been added as a result of stop/replace race condition */
	delGeofence(notifyData->notification.geofence_id, notifyData->q_transaction_id);
	return FALSE;
  }

  if(notifyData->notification.return_code == GEOFENCE_ERROR_NONE)
  {
	aonTransac->replaceGfRspHandler(notifyData->q_transaction_id, 
	  notifyData->notification.geofence_id, TRUE);
  }
  else 
  {
	AON_MSG_ERROR("replaceGfRspHandler return error : %d",
	  notifyData->notification.return_code, 0, 0);
	aonTransac->replaceGfRspHandler(notifyData->q_transaction_id, 
	  notifyData->notification.geofence_id, FALSE);
  }

  return TRUE;
}

 /* 
    @brief Retrieves a saved copy of the NV config used
    @param[in] None
    @return The saved config object
    */
const aonGmNvDefaultConfigType AonGmProxy::getGmNvDefaultConfig()
{
  return m_nvConfig;
}
