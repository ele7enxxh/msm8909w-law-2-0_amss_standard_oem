#ifndef SLIM_SSC_SENSOR_DATA_H
#define SLIM_SSC_SENSOR_DATA_H
/**
@file
@brief Module implementing the sensor data request to sensor core.
 
  SLIM can use this module to start or stop the sensor data
  streaming from SSC.

@ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_sensor_data.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_ssc_common.h"

//! @addtogroup slim_SSCProvider
//! @{

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
@brief Initiates sensor data request to SSC.

Function initiates sensor data request to SSC. If request is successfull,
SSC sensor data streaming is either started or stopped.

@param[in] pz_SscData     Pointer to the client SSC sensor data.
@param[in] pz_Txn         Pointer to sensor data request transaction data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscSensorDataEnableSensorReporting
(
  slim_SscSmgrConnectionDataStructType *pz_SscData,
  const slim_EnableSensorDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Handles response data for SMGR request

This function handles SMGR responses. Response is converted and forwarded to
SLIM client.

@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscSensorDataHandleResponse
(
  const slim_SscSmgrRespMsgT *pz_RespMsg
);

/**
@brief Handles SMGR indication data

This function handles SMGR indications. Data is converted and forwarded to
SLIM client.

@param[in] pz_IndMsg Pointer to the indication message data
*/
void slim_SscSensorDataHandleIndication
(
  const slim_SscSmgrIndMsgT *pz_IndMsg
);

/**
@brief Checks if SSC data should be throttled.

Function checks if SSC data should be throttled.

@param[in] pz_SscData      Pointer to the client device sensor data.
@param[in] e_Service       SLIM service type.
@param[in] t_DataTimestamp Timestamp of new data indication.

@retval TRUE  if SSC data should be throttled.
@retval FALSE if data should be forwarded to client.
*/
boolean slim_SscSensorDataThrottle
(
   slim_SscSmgrConnectionDataStructType* pz_SscData,
   slimServiceEnumT e_Service,
   uint64 t_DataTimestamp
);

//! @}

#endif /* #ifndef SLIM_SSC_SENSOR_DATA_H */
