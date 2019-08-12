#ifndef AON_TRANSAC_H
#define AON_TRANSAC_H
/*============================================================================
  @file aon_transac.h

  @brief
    This file contains API definitions to maange the generic AON transaction
    use case
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_transac.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
04/06/15    sj    LB 2.0 integration
01/14/15    sj    Added copyright info 
07/31/14    sj    Initial creation of file. 
=============================================================================*/
#include "comdef.h"
#include "aon_api.h"
#include "aon_internal.h"
#include "aon_gmproxy.h"

#define AON_CONVERT_INTO_WGS84(input) ((sint31)( (input * (1<<25) )/180 ))

class AonGeofenceConfig;
class AonGmClientConfig;
class AonDatabase;

class AonTransac {

public:
    AonTransac(uint32 reqId, AonDatabase* aonDbHdl, AonGmProxy *aonGmProxy,
	  aonLocationCallback locCb, aonStatusCallback statusCb): mReqId(reqId), 
	  mGfId_valid(FALSE), mGfId(0), mAonDbHdl(aonDbHdl), mAonGmProxy(aonGmProxy),
	  mAonLocCb(locCb), mAonStatusCb(statusCb),
	  mFirstFixPending(FALSE)
	  {}

    virtual ~AonTransac(){}
    /* 
    @brief Starts the session for the AON transaction
    @param[in] void
    @return AON_SUCCEEDED or AON_FAILED
    */
    virtual aonOperationStatusType start();
    /* 
    @brief Stops the session for the AON transaction
    @param[in] void
    @return AON_SUCCEEDED or AON_FAILED
    */
    virtual aonOperationStatusType stop();
    /* 
    @brief Replaces the geofence for the AON transaction
    @param[in] void
    @return TRUE or FALSE
    */
    boolean replaceGF(uint32 gfId, double latInDeg, double lonInDeg);
    /* 
    @brief Retrieve the equivalent geofence config for the AON transaction
    @param[out] gfConfig GF Config object that needs to be populated
    @return TRUE or FALSE
    */
    virtual boolean getEqGeofenceConfig(AonGeofenceConfig &gfConfig)=0;
    /* 
    @brief Retrieve the equivalent background engine client config for AON
    @param[in] gmConfig Background engine client level config object
    @return TRUE or FALSE
    */
    virtual boolean getEqGmClientConfig(AonGmClientConfig &gmConfig)=0;
    /* 
    @brief Breach response handler fn invoked by gmproxy 
    @param[in] gfId Geofence Id
    @param[in] breachType Type of breatch
    @param[in] location  Lcoation object instance adssocited witht his breach
    @return TRUE or FALSE
    */
    virtual boolean breachRspHandler(uint32 gfId, aonGMBreachType breachType, 
      locEngPositionReportMsgT_v01 &location)=0;
    /* 
    @brief Add handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the add request to background engine
    @return TRUE or FALSE
    */
    virtual boolean addGfRspHandler(uint32 txId, uint32 gfId, boolean result);
    /* 
    @brief Delete handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the delete request to background engine
    @return TRUE or FALSE
    */
    boolean delGfRspHandler(uint32 txId, uint32 gfId, boolean result);
    /* 
    @brief Replace geofence handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the delete request to background engine
    @return TRUE or FALSE
    */
    virtual boolean replaceGfRspHandler(uint32 txId, uint32 gfId, boolean result);
    /* 
    @brief Status call back handler invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] status Status of the background engine
    @return TRUE or FALSE
    */
    void statusCallbackHandler(uint32 txId, aonSessionStatusType status);
    /* 
    @brief Timer handler
    @param[in] void
    @return void
    */
    virtual void timerHandler();

    uint32 getReqId() {return mReqId; }
    void setGfId(uint32 gId) {mGfId_valid = TRUE; mGfId = gId; }
    void clearGfId() { mGfId_valid = FALSE; }
    uint32 getGfId() {return mGfId; }
    boolean validGfId() { return (TRUE == mGfId_valid); }
    	
protected:
    uint32 mReqId;
    boolean mGfId_valid;
    uint32 mGfId;	
    AonDatabase *mAonDbHdl;
    AonGmProxy *mAonGmProxy;
    aonLocationCallback mAonLocCb;
    aonStatusCallback mAonStatusCb;
    boolean mFirstFixPending; // used to send the origin location
};

#endif /*AON_TRANSAC_H*/

