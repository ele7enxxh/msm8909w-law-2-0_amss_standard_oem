#ifndef AON_BATCHTRANSAC_H
#define AON_BATCHTRANSAC_H
/*============================================================================
  @file aon_batchtransac.h

  @brief
    This file contains API definitions to maange the Batching
    use case of AlwaysOn Service
 
               Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_batchtransac.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
04/06/15    sj    Initial creation of file.
=============================================================================*/
#include "comdef.h"
#include "aon_api.h"
#include "aon_transac.h"
#include "aries_os_api.h"

class AonBatchTransac :public AonTransac{

public:
    AonBatchTransac(
		uint32 reqId, uint32 minInterval,uint32 minDistance, 
		uint32 batchingConfigMask, uint32 fixTimeout, 
		uint32 accuracy, aonBatchingMode mode, 
		AonDatabase* aonDbHdl, AonGmProxy *aonGmProxy,
		aonLocationCallback locCb, aonStatusCallback statusCb);
                     
    ~AonBatchTransac(){}

    /* 
    @brief Starts the session for the AON transaction
    @param[in] void
    @return AON_SUCCEEDED or AON_FAILED
    */
    aonOperationStatusType start();
    /* 
    @brief Stops the session for the AON transaction
    @param[in] void
    @return AON_SUCCEEDED or AON_FAILED
    */
    aonOperationStatusType stop();
 
    /* 
    @brief Retrieve the equivalent geofence config for the AON transaction
    @param[out] gfConfig GF Config object that needs to be populated
    @return TRUE or FALSE
    */
    boolean getEqGeofenceConfig(AonGeofenceConfig &gfConfig);
     /* 
    @brief Retrieve the equivalent background engine client config for AON
    @param[in] gmConfig Background engine client level config object
    @return TRUE or FALSE
    */
    boolean getEqGmClientConfig(AonGmClientConfig &gmConfig);
    /* 
    @brief Breach response handler fn invoked by gmproxy 
    @param[in] gfId Geofence Id
    @param[in] breachType Type of breatch
    @param[in] location  Lcoation object instance adssocited witht his breach
    @return TRUE or FALSE
    */
    boolean breachRspHandler(uint32 gfId, aonGMBreachType breachType, 
    	                          locEngPositionReportMsgT_v01 &location);
    /* 
    @brief Timer handler
    @param[in] void
    @return void
    */
    void timerHandler();
    /* 
    @brief Add handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the add request to background engine
    @return TRUE or FALSE
    */
    boolean addGfRspHandler(uint32 txId, uint32 gfId, boolean result);
    /* 
    @brief Replace geofence handler fn invoked by gmproxy 
    @param[in] txId transaction Id
    @param[in] gfId Geofence Id
    @param[in] result Result of the delete request to background engine
    @return TRUE or FALSE
    */
    boolean replaceGfRspHandler(uint32 txId, uint32 gfId, boolean result);

private:
    uint32 mMinInterval; // in ms
    uint32 mMinDistance; // in meters
      uint32 mBatchingConfigMask;
    uint32 mFixTimeout; // in ms
    uint32 mAccuracy; // in meters
    aonBatchingMode mMode;
    double mLastGfLat;
    double mLastGfLon;
    uint64 mLastUtc;
    os_TimerMsgType* mTimer;
    boolean mTimerExpired;
};

#endif /* AON_BATCHTRANSAC_H */

