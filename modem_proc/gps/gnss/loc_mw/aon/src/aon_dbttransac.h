#ifndef AON_DBTTRANSAC_H
#define AON_DBTTRANSAC_H
/*============================================================================
  @file aon_dbtransac.h

  @brief
    This file contains API definitions to maange the Distance Based Tracking
    use case of AlwaysOn Service
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_dbttransac.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  -----------------------------------------------------------
04/06/15    sj    LB 2.0 integration
03/18/15    sj    Fixed an yield issue with DBT through radius adjustment 
03/11/15    sj    Fixed an overflow issue in the reqId 
01/14/15    sj    Added copyright info & removed bread crumb references
07/31/14    sj    Initial creation of file.
=============================================================================*/
#include "aon_api.h"
#include "aon_transac.h"

//DBT specific macro definitions
#define AON_GF_RADIUS_ADJUSTMENT_THRESHOLD_MAX (500) // 500 meters
/* Due to the integer arithmetic being exclusively used in GM for radius calculations a few meters
*  maybe lost in round off errors. Becuase of stringent yield requirements for DBT this adjustment
*  is done before feeding the radius into GM
*/ 
#define AON_GM_RADIUS_ROUNDOFF_PREADJUSTMENT (2) // 2 meters
/* For DBT an ideal user exeprience is obtained when the distance reported to the client is maintained
*  between the following two percentage bounds of the minimum distance requested
*/
#define AON_DBT_DISTANCE_THREHOLD_UPPERBOUND_PERCENTAGE (115)
#define AON_DBT_DISTANCE_THREHOLD_LOWERBOUND_PERCENTAGE (85) 

class AonDbtTransac :public AonTransac{

public:
    AonDbtTransac(uint32 reqId, uint32 minDistance, 
                  aonDbtDistanceType distType,
                  boolean needOriginLocation,
                  uint32 maxLatency, aonDbtUsageType usageType,
                  AonDatabase* aonDbHdl, AonGmProxy *aonGmProxy,
		          aonLocationCallback locCb,aonStatusCallback statusCb);
                     

    ~AonDbtTransac(){}
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
private:
    uint32 mMinDistance;
    aonDbtDistanceType mDistType;
    boolean mNeedOriginLocation;
    uint32 mMaxLatency;
    aonDbtUsageType mUsageType;
    float mLastFixHorSpeed;
    boolean mLastFixHorSpeedIsValid;
    void calculateGfRadius(uint32 &radius);
};

#endif /*AON_DBTTRANSAC_H*/

