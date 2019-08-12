#ifndef SLIM_SSC_PEDOMETER_H
#define SLIM_SSC_PEDOMETER_H
/**
  @file
  @brief Module implementing the pedometer request to sensor core.
 
  SLIM can use this module to start or stop the pedometer
  streaming from SSC.

  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_pedometer.h#1 $ */

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
@brief Initiates pedometer request to SSC.

Function initiates pedometer request to SSC. If the request is successfull,
SSC pedometer streaming is either started or stopped.

@param[in] pz_SscData     Pointer to the SSC data.
@param[in] pz_Txn         Pointer to the pedometer request transaction data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscPedometerEnableReporting
(
  slim_SscPedometerConnectionDataStructType *pz_SscData,
  const slim_EnablePedometerTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Handles enable response data for pedometer request

This function handles enable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_PedometerData Pointer to the client pedometer data
@param[in] pz_RespMsg       Pointer to the response message data
*/
void slim_SscPedometerHandleEnableResponse
(
  slim_SscPedometerConnectionDataStructType *pz_PedometerData,
  const slim_SscEnablePedRespMsgT *pz_RespMsg
);

/**
@brief Handles disable response data for pedometer request

This function handles disable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_PedometerData Pointer to the client pedometer data
@param[in] pz_RespMsg       Pointer to the response message data
*/
void slim_SscPedometerHandleDisableResponse
(
  slim_SscPedometerConnectionDataStructType *pz_PedometerData,
  const slim_SscDisablePedRespMsgT *pz_RespMsg
);

/**
@brief Handles reset response data for pedometer request

This function handles reset response. Response is converted and forwarded to
SLIM client.

@param[in] pz_PedometerData Pointer to the client pedometer data
@param[in] pz_RespMsg       Pointer to the response message data
*/
void slim_SscPedometerHandleResetResponse
(
  slim_SscPedometerConnectionDataStructType *pz_PedometerData,
  const slim_SscResetPedRespMsgT *pz_RespMsg
);

/**
@brief Handles pedometer indication data

This function handles pedometer indication data. Data is converted and forwarded
to SLIM client.

@param[in] pz_PedometerData Pointer to the client pedometer data
@param[in] pz_IndMsg        Pointer to the indication message
*/
void slim_SscPedometerHandleIndication
(
  slim_SscPedometerConnectionDataStructType *pz_PedometerData,
  const slim_SscPedIndMsgT *pz_IndMsg
);

//! @}

#endif /* #ifndef SLIM_SSC_PEDOMETER_H */
