#ifndef SLIM_LOG_H
#define SLIM_LOG_H
/**
@file
@brief SLIM diagnostic logging API

@ingroup slim_CoreDiag
*/
/*============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/common/core/src/slim_log.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_provider_data.h"

//! @addtogroup slim_CoreDiag
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/**
 * @brief Growing list of SLIM requests for logging.
 *
 * @sa slim_LogClientRequestEventIdEnumType
 */
enum
{
  eSLIM_LOG_REQ_OPEN_V01  = 0,                 /**< Open client connection. */
  eSLIM_LOG_REQ_CLOSE_V01,                     /**< Close client connection. */
  eSLIM_LOG_REQ_ENABLE_DEVICE_SENSOR_DATA_V01, /**< Enable device sensor data */
  eSLIM_LOG_REQ_ENABLE_MOTION_DATA_V01,        /**< Enable motion data. */
  eSLIM_LOG_REQ_ENABLE_PEDOMETER_V01,          /**< Enable pedometer. */
  eSLIM_LOG_REQ_ENABLE_QMD_V01,                /**< Enable QMD. */
  eSLIM_LOG_REQ_ENABLE_SMD_V01,                /**< Enable SMD. */
  eSLIM_LOG_REQ_ENABLE_DB_V01,                 /**< Enable Distance Bound. */
  eSLIM_LOG_REQ_SET_DB_V01,                    /**< Set Distance Bound. */
  eSLIM_LOG_REQ_GET_DB_V01,                    /**< Get Distance Bound. */
  eSLIM_LOG_REQ_ENABLE_VEHICLE_SENSOR_DATA_V01,/**< Enable vehicle sensor data. */
  eSLIM_LOG_REQ_ENABLE_PED_ALIGNMENT_V01,      /**< Enable pedestrian alignment. */
  eSLIM_LOG_REQ_ENABLE_MAG_FIELD_DATA_V01,     /**< Enable magnetic field data. */
  eSLIM_LOG_REQ_GET_PROVIDER_TIME_V01,         /**< Obsolete. */
  eSLIM_LOG_REQ_GET_PROVIDER_TIME_V02          /**< Get provider time. */
};

/** @brief Enum for SLIM client request event id. */
typedef enum
{
  eSLIM_LOG_REQ_OPEN  = eSLIM_LOG_REQ_OPEN_V01,
  /**< Open client connection. */
  eSLIM_LOG_REQ_CLOSE = eSLIM_LOG_REQ_CLOSE_V01,
  /**< Close client connection. */
  eSLIM_LOG_REQ_ENABLE_DEVICE_SENSOR_DATA = eSLIM_LOG_REQ_ENABLE_DEVICE_SENSOR_DATA_V01,
  /**< Enable device sensor data */
  eSLIM_LOG_REQ_ENABLE_MOTION_DATA = eSLIM_LOG_REQ_ENABLE_MOTION_DATA_V01,
  /**< Enable motion data. */
  eSLIM_LOG_REQ_ENABLE_PEDOMETER = eSLIM_LOG_REQ_ENABLE_PEDOMETER_V01,
  /**< Enable pedometer. */
  eSLIM_LOG_REQ_ENABLE_QMD = eSLIM_LOG_REQ_ENABLE_QMD_V01,
  /**< Enable QMD. */
  eSLIM_LOG_REQ_ENABLE_SMD = eSLIM_LOG_REQ_ENABLE_SMD_V01,
  /**< Enable SMD. */
  eSLIM_LOG_REQ_ENABLE_DB = eSLIM_LOG_REQ_ENABLE_DB_V01,
  /**< Enable Distance Bound. */
  eSLIM_LOG_REQ_SET_DB = eSLIM_LOG_REQ_SET_DB_V01,
  /**< Set Distance Bound. */
  eSLIM_LOG_REQ_GET_DB = eSLIM_LOG_REQ_GET_DB_V01,
  /**< Get Distance Bound. */
  eSLIM_LOG_REQ_ENABLE_VEHICLE_SENSOR_DATA = eSLIM_LOG_REQ_ENABLE_VEHICLE_SENSOR_DATA_V01,
  /**< Enable vehicle sensor data. */
  eSLIM_LOG_REQ_ENABLE_PED_ALIGNMENT = eSLIM_LOG_REQ_ENABLE_PED_ALIGNMENT_V01,
  /**< Enable pedestrian alignment. */
  eSLIM_LOG_REQ_ENABLE_MAG_FIELD_DATA = eSLIM_LOG_REQ_ENABLE_MAG_FIELD_DATA_V01,
  /**< Enable magnetic field data. */
  eSLIM_LOG_REQ_GET_PROVIDER_TIME = eSLIM_LOG_REQ_GET_PROVIDER_TIME_V02,
  /**< Get provider time. */

  //! @cond
  SLIM_LOG_REQ_MAX = 2147483647 /* To force a 32 bit signed enum.
                                   Do not change or use */
  //! @endcond
} slim_LogClientRequestEventIdEnumType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Logs time sync data

Function sends time sync data for logging.

@param[in] e_Provider          Sensor service provider type.
@param[in] pd_KfStatePriori    Filter state prior to the RTT time update.
@param[in] pd_KfStateCovPriori Kf state  covariance matrix prior to the RTT
                               time update.
@param[in] pz_Filter           Provider time synchronization filter.
@param[in] e_UpdateResult      RTT time update result.
@param[in] pz_TimeResp         Time response data.
@param[in] t_LocalRxTimeMsec   Local receive time.
*/
void slim_LogTimeSync
(
  slim_ProviderEnumType e_Provider,
  const DBL pd_KfStatePriori[2],
  const DBL pd_KfStateCovPriori[3],
  const slim_TimeSyncFilterStateStructType *pz_Filter,
  slim_TimeSyncUpdateStatusEnumType e_UpdateResult,
  const slimGetProviderTimeResponseStructT *pz_TimeResp,
  uint64 t_LocalRxTimeMsec
);

/**
@brief Logs client request.

Function sends client request data for logging.

@param[in] e_RequestId     Id of the client request.
@param[in] e_Provider      Selected SLIM service provider.
@param[in] l_TransactionId SLIM transaction id.
@param[in] e_Error         SLIM error code.
@param[in] w_RequestSize   Size for the client request.
@param[in] p_Request       Pointer to the client request.
*/
void slim_LogEventClientRequest
(
  slim_LogClientRequestEventIdEnumType e_RequestId,
  slim_ProviderEnumType e_Provider,
  int32 l_TransactionId,
  slimErrorEnumT e_Error,
  uint16 w_RequestSize,
  const void *p_Request
);

/**
@brief Logs response sent to client.

Function logs response sent to client.

@param[in] e_MessageId   SLIM message id.
@param[in] l_TxnId       SLIM transaction id.
@param[in] p_Handle      SLIM client handle.
@param[in] u_ClientTxnId SLIM client transaction id.
@param[in] e_Service     SLIM service.
@param[in] e_Provider    SLIM service provider sending the response.
@param[in] e_MsgError    SLIM error code.
*/
void slim_LogEventClientResponse
(
  slimMessageIdEnumT e_MessageId,
  int32 l_TxnId,
  slimClientHandleT p_Handle,
  uint8 u_ClientTxnId,
  slimServiceEnumT e_Service,
  slim_ProviderEnumType e_Provider,
  slimErrorEnumT e_MsgError
);

/**
@brief Logs indication sent to client.

Function logs indication sent to client.

@param[in] e_MessageId   SLIM message id.
@param[in] p_Handle      SLIM client handle.
@param[in] e_Service     SLIM service.
@param[in] e_Provider    SLIM service provider sending the indication.
@param[in] e_MsgError    SLIM error code.
@param[in] w_PayloadSize Size of the payload.
@param[in] p_Payload     Payload data. One of:
                         - #slimSensorDataStructT
                         - #slimMotionDataStructT
                         - #slimPedometerDataStructT
                         - #slimQmdDataStructT
                         - #slimSmdDataStructT
                         - #slimDistanceBoundDataStructT
                         - #slimVehicleSensorDataStructT
                         - #slimVehicleOdometryDataStructT
                         - #slimPedestrianAlignmentDataStructT
                         - #slimMagneticFieldDataStructT
*/
void slim_LogEventClientIndication
(
  slimMessageIdEnumT e_MessageId,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  slim_ProviderEnumType e_Provider,
  slimErrorEnumT e_MsgError,
  uint16 w_PayloadSize,
  const void *p_Payload
);

/**
@brief Enable or disable logs

@param[in] u_EnableFlag When set to TRUE logging is enabled.
*/
void slim_LogSetEnabled
(
  boolean u_EnableFlag
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_LOG_H */
