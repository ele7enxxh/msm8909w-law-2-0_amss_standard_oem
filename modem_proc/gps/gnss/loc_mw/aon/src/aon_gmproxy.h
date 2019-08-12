#ifndef AON_GMPROXY_H
#define AON_GMPROXY_H

/*============================================================================
  @file aon_gmproxy.h

  @brief
    This module abstracts the GM API interface to AlwaysOn service.
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_gmproxy.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
08/13/15    sj    Added default configuration of GM from NV 
04/06/15    sj    LB 2.0 integration 
03/02/15    sj    Use replaceGf() API from GM instead of Del/Add seq 
01/19/15    sj    Canned responsiveness to UHR for maxLatency = 1 sec 
09/17/14    sj    Initial creation of file.


=============================================================================*/
/*****************************************************************************
 * Include Files
 * *************************************************************************/
#include "comdef.h"
#include "gm_api.h"
#include "aon_db.h"
#include "aries_os_api.h"

#define AON_GM_CONFIG_MAX_CPI_REQ_RATE (0xFFFF) //65535 seconds

#define AON_GEOFENCE_CONFIG_INVALID_ACCURACY (0) // 0 meters

class AonGeofenceConfig
{
public: 
  uint32 mGfRadius;
  sm_gm_geofence_responsiveness mResponsiveness;
  uint32 mGfResponsivenessInSec;
  sint31 mGfLatInWGS_84;
  sint31 mGfLonInWGS_84;
  boolean mGfContextUsageFlag;
  sm_gm_geofence_event_type mGfBreachEventType;
  boolean mGfForceResponsiveness;
  uint32 mGfAccuracy;
  boolean b_force_use_cached_fix;
  AonGeofenceConfig():
			mGfRadius(0),
			mResponsiveness(GEOFENCE_RESPONSIVENESS_CUSTOM),
			mGfResponsivenessInSec(0),
			mGfContextUsageFlag(TRUE),
			mGfBreachEventType((sm_gm_geofence_event_type)0),
			mGfForceResponsiveness(FALSE),
			mGfAccuracy(AON_GEOFENCE_CONFIG_INVALID_ACCURACY)
			{ }

	~AonGeofenceConfig() {}
};

#define AON_GM_PROXY_DEFAULT_SESSION_TIMEOUT (30)

class AonGmClientConfig
{
  
  public:
	uint32 mCpiReqRate; //in seconds
	boolean mGnssUnavailAggresiveExit;
	uint32 mSessionTimeout; // in seconds
	uint32 mGmBackOffMax; // in seconds
	uint32 mGmBackOffMin; // in seconds
	AonGmClientConfig():
	                 mCpiReqRate(AON_GM_CONFIG_MAX_CPI_REQ_RATE),
			         mGnssUnavailAggresiveExit(FALSE),
					 mSessionTimeout(0),
					 mGmBackOffMax(CGPS_MAX_UINT32),
					 mGmBackOffMin(CGPS_MAX_UINT32)
					 {}
					    
	            
    ~AonGmClientConfig(){ } 
};




class AonGmProxy {

public:
	/* 
   @brief Overloaded constructor- Creates an singleton instance of AonGmProxy object
   @param[in] None
   */
    static AonGmProxy * Instance();
	/* 
   @brief Overloaded constructor- Creates an singleton instance of AonGmProxy object
   @param[in] p_nvConfig Object containing the NV default values that are needed to register with GM
   */
    static AonGmProxy * Instance(const aonGmNvDefaultConfigType *p_nvConfig);
    /* 
   @brief Adds a geofence using Background Engine APIs
   @param[in] aonGfConfig Equivalent geofence config for the AON transaction
   @param[in] txId  transaction Id
   @return TRUE or FALSE
   */
	boolean addGeofence(AonGeofenceConfig &aonGfConfig, uint32 txId);
     /* 
    @brief Background engine events handler 
    @param[in] q_MsgId Message Id passed in
    @param[in] p_data Payload
    @return TRUE or FALSE
    */
	boolean gmEventHandler(uint32 q_MsgId, uint8 *p_data);
    /* 
   @brief Deletes a geofence using Background Engine APIs
   @param[in] gfId Geofence Id
   @param[in] txId  transaction Id
   @return TRUE or FALSE
   */
	boolean delGeofence( uint32 gfId, uint32 txId);
    /* 
    @brief Sets the configuration for Background engine 
    @param[in] aonGmConfig \requried config
    @return TRUE or FALSE
    */
    boolean setGfEngineParam(AonGmClientConfig &aonGmConfig);
     /* 
   @brief Replaces a geofence using Background Engine APIs
   @param[in] aonGfConfig Equivalent geofence config for the AON transaction
   @param[in] txId  transaction Id   
   @param[in] gfId Geofence Id 
   @return TRUE or FALSE
   */
	boolean replaceGeofence(AonGeofenceConfig &aonGfConfig, uint32 txId, uint32 gfId);
    //Setter/getters for unable to track reproted flag
	boolean getUnableToTrackReported() {return mUnableToTrackReported; }
	void setUnableToTrackReported(boolean setting) {mUnableToTrackReported = setting;}
    const aonGmNvDefaultConfigType getGmNvDefaultConfig();
	double m_lastBreachLatInDeg; 
    double m_lastBreachLonInDeg;

private:
	static AonGmProxy * m_pInstance;
    const aonGmNvDefaultConfigType m_nvConfig;
	AonGmProxy (); // Private so that it can  not be called
    AonGmProxy(const aonGmNvDefaultConfigType *p_nvConfig);
	~AonGmProxy();
    sm_gm_client_handle gmHandle; /* Handle to the GM core*/
	AonDatabase *m_aonDbHdl;
    /* 
    @brief Add handler fn invoked by background engine 
    @param[in] notifyData Add notification data
    @return TRUE or FALSE
    */
	boolean addGfRspHandler(sm_gm_client_notification_type* notifyData);
    /* 
    @brief Unsolicited handler fn invoked by background engine 
    @param[in] notifyData Unsolicited events notification data
    @return TRUE or FALSE
    */
	boolean unsolEventHandler(sm_gm_client_unsol_event_notify_type* notifyData);
    /* 
    @brief Breach event handler fn invoked by background engine 
    @param[in] notifyData Breach events notification data
    @return TRUE or FALSE
    */
	boolean breachGfRspHandler(sm_gm_client_breach_notify_type* notifyData);
    /* 
    @brief Delete request response event handler fn invoked by background engine 
    @param[in] notifyData Delete request response notification data
    @return TRUE or FALSE
    */
	boolean delGfRspHandler(sm_gm_client_notification_type* notifyData);
    /* 
    @brief Transalates position report from brach event provided by background engine 
    @param[in] p_gnss_fix_rpt Position information from breach event 
    @param[out] location Location object
    @return TRUE or FALSE
    */
	boolean xlatePositionRpt(sm_GnssFixRptStructType *p_gnss_fix_rpt,
                           locEngPositionReportMsgT_v01 &location);
     /* 
    @brief Transalates geofence config info into Background engine format 
    @param[in] aonGmConfig AON geofence config object 
    @param[out] geofence Background engine genfence sepcification object
    @return TRUE or FALSE
    */
	void xlateGeofenceConfig(AonGeofenceConfig &aonGmConfig, sm_gm_geofence_type &geofence);
     /* 
    @brief Replace geofence handler fn invoked by Background engine
    @param[in] notifyData replace request response notification data
    @return TRUE or FALSE
    */
	boolean replaceGfRspHandler(sm_gm_client_notification_type* notifyData);
    //flag for tracking unable to track state
    boolean mUnableToTrackReported;
};


#endif /*AON_GMPROXY_H*/
