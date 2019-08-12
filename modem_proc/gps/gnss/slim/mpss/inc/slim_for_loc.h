#ifndef SLIM_FOR_LOC_H
#define SLIM_FOR_LOC_H
/**
  @file
  @brief Interface for LocMW-task and SLIM module

  This interface is provided by SLIM for the LocMW-task. LocMW can inject
  QMI_LOC-requests to SLIM using the interface functions.

  @ingroup slim_QMILOCProvider slim_MPSS
*/

/*============================================================================
               Copyright (c) 2013 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/mpss/inc/slim_for_loc.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "location_service_v02.h"
#include "locEng_sm_common_msg_v01.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_QMILOCProvider
//! @{
//! @name SLIM interface to LOC_MW Module
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/** QMI_LOC error codes */
typedef enum
{
  SLIM_QMI_LOC_SUCCESS                     = 0,         /* No error - success */
  SLIM_QMI_LOC_ERROR_DEFAULT               = 1,
  SLIM_QMI_LOC_ERROR_INDICATION_NOT_SENT   = 2,
  SLIM_QMI_LOC_ERROR_NULL_POINTER          = 3,
  SLIM_QMI_LOC_ERROR_INVALID_REQUEST       = 4,
  SLIM_QMI_LOC_ERROR_UNSUPPORTED           = 5,
  //! @cond
  SLIM_QMI_LOC_ERROR_MAX                   = 2147483647 /* used to force enum to 32-bit */
  //! @endcond
} slim_QmiLocServiceErrorEnumType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Handler for QMI_LOC sensor control config message.

This function should be called when LocMW task receives sensor control config
request. SLIM makes the decision which sensor provider (QMI_LOC/SSC) to use
based on this message. By default SLIM uses SSC if it is available.

@param  pz_Request: Pointer to the request data.
@return 0 if function succeeds.
*/
uint32 slim_LocInjectSensorControlConfigRequest
(
  const qmiLocSetSensorControlConfigReqMsgT_v02 *pz_Request
);

/**
@brief Handler for QMI_LOC time synchronization injection message.

This function should be called when LocMW task receives time
synchronization request from an external sensor data provider. SLIM forwards the received
time synchronization data to registered clients.

@param  h_ClientHandle: Client handle.
@param  q_TransactionId: Transaction id.
@param  pz_Request: Pointer to the request data.
@return 0 if function succeeds.
*/
uint32 slim_LocInjectTimeSyncDataRequest
(
  int h_ClientHandle,
  uint32 q_TransactionId,
  const qmiLocInjectTimeSyncDataReqMsgT_v02 *pz_Request
);

/**
@brief Handler for QMI_LOC sensor data injection message.

This function should be called when LocMW task receives sensor
data request from an external sensor data provider. SLIM forwards the received
sensor data to registered clients.

@param  h_ClientHandle: Client handle.
@param  q_TransactionId: Transaction id.
@param  pz_Request: Pointer to the request data.
@return 0 if function succeeds.
*/
uint32 slim_LocInjectSensorDataRequest
(
  int h_ClientHandle,
  uint32 q_TransactionId,
  const qmiLocInjectSensorDataReqMsgT_v02 *pz_Request
);

/**
@brief Handler for QMI_LOC motion data injection message.

This function should be called when LocMW task receives motion
data request from an external sensor data provider. SLIM forwards the received
motion data to registered clients.

@param  pz_Request: Pointer to the request data.
@return locEng status code
*/
locEngStatusEnumT_v01 slim_LocInjectMotionDataRequest
(
  const locEngInjectMotionDataReqMsgT_v01 *pz_Request
);

/**
@brief Handler for QMI_LOC pedometer injection message.

This function should be called when LocMW task receives pedometer
request from an external sensor data provider. SLIM forwards the received
pedometer data to registered clients.

@param  pz_Request: Pointer to the request data.
@return locEng status code
*/
locEngStatusEnumT_v01 slim_LocInjectPedometerRequest
(
  const locEngPedometerReportReqMsgT_v01 *pz_Request
);

/**
@brief Handler for QMI_LOC vehicle data injection message.

This function should be called when LocMW task receives vehicle data
request from an external sensor data provider. SLIM forwards the received
vehicle data to registered clients.

@param  pz_Request: Pointer to the request data.
@return locEng status code
*/
locEngStatusEnumT_v01 slim_LocInjectVehicleDataRequest
(
  const locEngInjectVehicleSensorDataReqMsgT_v01 *pz_Request
);

//! @}
//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_FOR_LOC_H */
