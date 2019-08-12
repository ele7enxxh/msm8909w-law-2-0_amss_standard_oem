#ifndef AON_API_H
#define AON_API_H

/*============================================================================
  @file aon_api.h

  @brief
    This file contains API definitions by which AlwaysOn location services can 
    be accessed by clients (e.g LOC MW).
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/inc/aon_api.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
09/30/15    sj    Added hysterisis for unsolicited status indications 
08/13/15    sj    Added default configuration of GM from NV
04/06/15    sj    LB 2.0 integration 
03/11/15    sj    Fixed an overflow issue in the reqId 
02/25/15    sj    Fixed status indications for client specfic cases
01/26/15    sj    Added correct transalation for positionType field 
01/19/15    sj    Added default maxLatency defn 
01/14/15    sj    Removed bread crumb references 
07/31/14    sj    Initial creation of file.


=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 * Include Files
 * *************************************************************************/
#include "comdef.h"
#include "locEng_sm_common_msg_v01.h"
/*****************************************************************************
 * Preprocessor Definitions and Constants
 * *************************************************************************/

//Default latency of 1 sec for DBT if not specified by client
#define AON_DBT_DEFAULT_LATENCY (1)

// default latency for distance only batching mode
#define AON_BATCHING_MODE_DISTANCE_ONLY_MIN_INTERVAL_MS (10000) // 5 seconds

/* Batching configuration masks */
// Configuration for AON to batch all types of breaches provided by background engine
#define AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES       ((uint32)1<<0)
// Configuration for AON to disable Context based back off in background engine
#define AON_BATCHING_CONFIG_MASK_DISABLE_CONTEXT_BASED_BO ((uint32)1<<1)
//Maximum CPI rate that can be configured by AON
#define AON_BATCHING_CONFIG_MASK_CPI_REQ_RATE_MAX         ((uint32)1<<2)


  
/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
 @brief 
  Operation Status type: Enumeration with information that the client needs 
  to determine if the request to the service was acepted successfully.
*//* ------------------------------------------------------------------------*/
typedef enum
{
  AON_SUCCEEDED, /*< Request was accepted */
  AON_FAILED, /*< Request was rejected */
} aonOperationStatusType;

/* -----------------------------------------------------------------------*//**
 @brief 
  Session Status type: Enumeration with information that the client needs 
  to determine if the request to the service was executed successfully.
*//* ------------------------------------------------------------------------*/
typedef enum
{
  AON_UNABLE_TO_TRACK,/*< Service currently cannot track the location of the
                          client*/
  AON_ABLE_TO_TRACK, /*< Service currently is able track the location of the
                        client*/
  AON_START_SUCCEEDED, /**< AON request was started successfully */

  AON_START_FAILED, /**< AON request could not be started */

  AON_GNL_FAILURE, /*< AON Transaction has encountered a failure from which it
				      cannot recover */
  AON_GNSS_UNAVAIL, /* Service cannot obtain GNSS locations */
  
  AON_GNSS_AVAIL /* Service can obtain GNSS locations now */
} aonSessionStatusType;

/* -----------------------------------------------------------------------*//**
 @brief 
  Position type: Enumeration with information that the client needs 
  to determine if the position obtained is the one at the origin of the session
  or while tracking
*//* ------------------------------------------------------------------------*/
typedef enum
{
  AON_POSITION_TYPE_ORIGIN, /*< Position at the origin */
  AON_POSITION_TYPE_TRACKING /*<  Position while tracking */
} aonPositionType;

/* -----------------------------------------------------------------------*//**
 @brief 
  Batching mode: enumerates the various batching modes
*//* ------------------------------------------------------------------------*/
typedef enum
{
  AON_BATCHING_MODE_TIME_ONLY, /*< batching strictly based on time (LB1.0) */
  AON_BATCHING_MODE_AUTO, /*< automatic user motion based batching (LB1.5) */
  AON_BATCHING_MODE_DISTANCE_ONLY, /*< Only distance based batching */
  AON_BATCHING_MODE_TIME_AND_DISTANCE /*< time and distance based batching (LB2.0) */
} aonBatchingMode;

/* -----------------------------------------------------------------------*//**
 @brief 
  NV Default Config: For configuring the default values for registering with GM
*//* ------------------------------------------------------------------------*/
typedef struct
{
  uint32 q_gnssPositionQosSessionTimeout;
  uint32 q_cpiReqRate;
  uint32 q_backoffMin;
  uint32 q_backoffMax;
  uint32 q_motionSensingDistGps;
  uint32 q_motionSensingDistCpi;
} aonGmNvDefaultConfigType;


/* -----------------------------------------------------------------------*//**
@brief
  This function pointer is used to register a call back for location updates

@retval  void
@param[in] reqId unique Id generated by the client to identify its request
@param[out] p_clientCookie ptr which was passed in by the client at registration
@param[out] p_location Ptr to Location information
@param[out] positionType Tracking or Origin location
*//* ------------------------------------------------------------------------*/

typedef void (* aonLocationCallback)(uint32 reqId, 
  locEngPositionReportMsgT_v01 * p_location,
  aonPositionType positionType);

/* -----------------------------------------------------------------------*//**
@brief
  This function pointer is used to register a call back for status updates for 
  individual requests

@retval  void
@param[out] reqId unique Id generated by the client to identify its request
@param[out] sessionStatus Status information for the request
*//* ------------------------------------------------------------------------*/

typedef void (* aonStatusCallback)(uint32 reqId, 
  aonSessionStatusType sessionStatus);

/* -----------------------------------------------------------------------*//**
@brief
  This function is used by clients to register to AlwaysOn location Services.
   Any client who is interested in AON services such as Distance Based Tracking etc. 
   should register using this API to AON

@retval  aonOperationStatusType: SUCCESS or FAIL
@param[in] locCb: Pointer to location callback
@param[in] statusCb: Pointer to status callback
@param[in] p_nvConfig: Pointer to the default NV Config
*//* ------------------------------------------------------------------------*/
aonOperationStatusType aonClientRegister(
  aonLocationCallback locCb,
  aonStatusCallback statusCb,
  const aonGmNvDefaultConfigType *p_nvConfig);
typedef enum {
  
 AON_DBT_DISTANCE_TYPE_STRAIGHT_LINE= 1, /**<  Straight line distance between 
       location updates   */
  
}aonDbtDistanceType;

typedef enum {
  AON_DBT_USAGE_NAVIGATION = 1, /**<  Navigation usage type  */
  
}aonDbtUsageType;

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
  uint32 maxLatency, aonDbtUsageType usageType);

/* -----------------------------------------------------------------------*//**
@brief
  This function is used to delete an AON request 

@retval  aonOperationStatusType: SUCCESS or FAIL
@param[in] reqId A unique request Id for this location request

*//* ------------------------------------------------------------------------*/
aonOperationStatusType aonDeleteRequest( uint32 reqId);

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
aonOperationStatusType aonCreateBatchingRequest(uint32 reqId, uint32 minInterval, 
  uint32 minDistance, uint32 aonBatchingConfigMask, uint32 fixTimeout, uint32 accuracy,
  aonBatchingMode mode);



/* -----------------------------------------------------------------------*//**
@brief
  This function is used to route a GM message to AON library
  if available

@retval  aonOperationStatusType: SUCCESS or FAIL
@param[in] q_MsgId GM message Id
@param[in] pData   Message payload 
*//* ------------------------------------------------------------------------*/
aonOperationStatusType aonGmEventHandler(uint32 q_MsgId, uint8 *p_data);

/* 
@brief handle the AON timer IPC messages
@param[in] id corresponding to the timer
@return None 
*/
void aonTimerHandler( uint32 timerId);

#ifdef __cplusplus
}
#endif


#endif /*AON_API_H*/
