#ifndef SLIM_CLIENT_MSG_TYPES_H
#define SLIM_CLIENT_MSG_TYPES_H
/**
@file
@brief Header with data types for message forwarding.

  Header containing the message types that SLIM client can use to forward
  SLIM data.

@ingroup slim_MPSS
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/mpss/inc/slim_client_msg_types.h#1 $ */

#include <slim_client_types.h>

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_MPSS
//! @{
//! @name Client IPC Messages
//! @{

/* SLIM -> client IPC messages on modem.

   To keep IPC sizes constant, we map all (e_MsgType, e_MsgId) pairs to
   individual IPCs.
 */

/*! Struct for SLIM response IPC header */
typedef struct
{
  slimServiceEnumT service;
  /**< SLIM service ID of the source that sent this response */

  slimErrorEnumT   msgError;
  /**< SLIM error code */

  uint32_t         msgId;
  /**< SLIM message ID */

  uint8_t          txnId;
  /**< Client transaction ID set in request */
} slimRespHeaderStructT;

/*! Struct for SLIM indication IPC header */
typedef struct
{
  slimServiceEnumT service;
  /**< SLIM service ID of the source that sent this indication */

  slimErrorEnumT   msgError;
  /**< SLIM error code */

  uint32_t         msgId;
  /**< SLIM message ID */
} slimIndHeaderStructT;

/**
@brief Sensor data message

@sa slimSensorDataStructT
*/
typedef struct
{
  uint64_t              modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT  indHeader;
  /**< Indication header */

  slimSensorDataStructT sensorData;
  /**< SLIM sensor data payload */

  uint8_t               callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t              callbackData;
  /**< Callback data provided to SLIM with client open */
} slimInjectSensorDataIndMsgT;

/**
@brief Pedometer data message

@sa slimPedometerDataStructT
*/
typedef struct
{
  uint64_t                 modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT     indHeader;
  /**< Indication header */

  slimPedometerDataStructT pedometerData;
  /**< SLIM pedometer data payload */

  uint8_t                  callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t                 callbackData;
  /**< Callback data provided to SLIM with client open */
} slimInjectPedometerDataIndMsgT;

/**
@brief Motion data message

@sa slimMotionDataStructT
*/
typedef struct
{
  uint64_t              modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT  indHeader;
  /**< Indication header */

  slimMotionDataStructT motionData;
  /**< SLIM motion data payload */

  uint8_t               callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t              callbackData;
  /**< Callback data provided to SLIM with client open */
} slimInjectMotionDataIndMsgT;

/**
@brief Vehicle sensor data message

@sa slimVehicleSensorDataStructT
*/
typedef struct
{
  uint64_t                     modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT         indHeader;
  /**< Indication header */

  slimVehicleSensorDataStructT vehicleSensorData;
  /**< SLIM vehicle sensor data payload */

  uint8_t                      callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t                     callbackData;
  /**< Callback data provided to SLIM with client open */
} slimInjectVehicleSensorDataIndMsgT;

/**
@brief Vehicle odometry data message

@sa slimVehicleOdometryDataStructT
*/
typedef struct
{
  uint64_t                       modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT           indHeader;
  /**< Indication header */

  slimVehicleOdometryDataStructT vehicleOdometryData;
  /**< SLIM vehicle odometry data payload */

  uint8_t                        callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t                       callbackData;
  /**< Callback data provided to SLIM with client open */
} slimInjectVehicleOdometryDataIndMsgT;

/**
@brief Service status message

@sa slimServiceStatusEventStructT
*/
typedef struct
{
  uint64_t                      modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT          indHeader;
  /**< Indication header */

  slimServiceStatusEventStructT serviceStatusEvent;
  /**< SLIM service status event payload */

  uint8_t                       callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t                      callbackData;
  /**< Callback data provided to SLIM with client open */
} slimInjectServiceStatusEventIndMsgT;

/* Following Error response and indication are used when
   message received from SLIM has MsgId NONE. */
/**
@brief Error response message

This response is used when SLIM message identifier is eSLIM_MESSAGE_ID_NONE.
*/
typedef struct
{
  uint64_t              modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimRespHeaderStructT respHeader;
  /**< Response header */

  /* No payload */

  uint8_t               callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t              callbackData;
  /**< Callback data provided to SLIM with client open */
} slimErrorRespMsgT;

/**
@brief Error indication message

This response is used when SLIM message identifier is eSLIM_MESSAGE_ID_NONE.
*/
typedef struct
{
  uint64_t             modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT indHeader;
  /**< Indication header */

  /* No payload */

  uint8_t              callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t             callbackData;
  /**< Callback data provided to SLIM with client open */
} slimErrorIndMsgT;

/**
@brief Pedestrian alignment data message

@sa slimPedestrianAlignmentDataStructT
*/
typedef struct
{
  uint64_t                 modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT     indHeader;
  /**< Indication header */

  slimPedestrianAlignmentDataStructT pedestrianAlignmentData;
  /**< SLIM pedestrian alignment data payload */

  uint8_t                  callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t                 callbackData;
  /**< Callback data provided to SLIM with client open */
} slimInjectPedestrianAlignmentDataIndMsgT;

/**
@brief Magnetic field data message

@sa slimMagneticFieldDataStructT
*/
typedef struct
{
  uint64_t                 modemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slimIndHeaderStructT     indHeader;
  /**< Indication header */

  slimMagneticFieldDataStructT magneticFieldData;
  /**< SLIM magnetic field data payload */

  uint8_t                  callbackData_valid;
  /**< Callback data validity.
       TRUE if callbackData is valid, FALSE otherwise. */
  uint64_t                 callbackData;
  /**< Callback data provided to SLIM with client open */
} slimInjectMagneticFieldDataIndMsgT;

//! @}
//! @}

#ifdef __cplusplus
}
#endif

#endif
