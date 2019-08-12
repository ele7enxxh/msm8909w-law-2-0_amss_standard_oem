#ifndef LOC_FOR_SLIM_H
#define LOC_FOR_SLIM_H
/*============================================================================
  @file loc_for_slim.h
 
  Functions for SLIM module to communicate with Loc MW task
 
               Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/inc/loc_for_slim.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "location_service_v02.h"
#include "locEng_sm_common_msg_v01.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Initiates a time sync request via QMI_LOC.
  
SLIM can send time synchronization request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_time_sync_needed_ind
(
  const qmiLocEventTimeSyncReqIndMsgT_v02 *const ptr_ind
);

/**
@brief Initiates a sensor data request via QMI_LOC.
  
SLIM can send sensor data streaming request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_sensor_streaming_readiness_ind
(
  const qmiLocEventSensorStreamingReadyStatusIndMsgT_v02 *const ptr_ind
);

/**
@brief Initiates a motion data request via QMI_LOC.
  
SLIM can send motion data streaming request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_motion_data_control_ind
(
  const qmiLocEventMotionDataControlIndMsgT_v02 *const ptr_ind
);

/**
@brief Initiates a pedometer request via QMI_LOC.
  
SLIM can send pedometer streaming request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_pedometer_control_ind
(
  const qmiLocEventPedometerControlIndMsgT_v02 *const ptr_ind
);

/**
@brief Initiates a vehicle data request via QMI_LOC.
  
SLIM can send vehicle data streaming request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_vehicle_sensor_injection_ind
(
  const qmiLocEventVehicleDataReadyIndMsgT_v02 *const ptr_ind
);

/**
@brief Sends time sync request status indication.
  
SLIM can send the status of the time synchronization request handling via QMI_LOC
using this function.
  
@param  client_handle: LocAPI client handle
@param  transaction_id: Same transaction id passed in earlier by the request
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_time_sync_data_ind_proxy
(
  const int32 client_handle,
  const uint32 transaction_id, 
  const qmiLocInjectTimeSyncDataIndMsgT_v02 * const p_ind
);

/**
@brief Sends sensor data request status indication.
  
SLIM can send the status of the sensor data request handling via QMI_LOC
using this function.
  
@param  client_handle: LocAPI client handle
@param  transaction_id: Same transaction id passed in earlier by the request
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_sensor_data_ind_proxy
(
  const int32 client_handle,
  const uint32 transaction_id,
  const qmiLocInjectSensorDataIndMsgT_v02 * const p_ind
);

/**
@brief Sends motion data request status indication.
  
SLIM can send the status of the motion data request handling via QMI_LOC
using this function.
  
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_motion_data_ind_proxy
(
  const locEngInjectMotionDataIndMsgT_v01 * const p_ind
);

/**
@brief Sends pedometer request status indication.
  
SLIM can send the status of the pedometer request handling via QMI_LOC
using this function.
  
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_pedometer_ind_proxy
(
  const locEngPedometerReportIndMsgT_v01 * const p_ind
);

/**
@brief Sends vehicle data request status indication.
  
SLIM can send the status of the vehicle data request handling via QMI_LOC
using this function.
  
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_vehicle_sensor_data_resp_ind_proxy
(
  const locEngInjectVehicleSensorDataRespMsgT_v01 * const p_ind
);
#endif /* #ifndef LOC_FOR_SLIM_H */
