#ifndef SLIM_SSC_DISTANCE_BOUND_H
#define SLIM_SSC_DISTANCE_BOUND_H
/**
  @file slim_ssc_distance_bound.h
  @brief Module implementing the distance bound request to sensor core.
 
  SLIM can use this module to start or stop the distance bound service
  streaming from SSC.

  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_distance_bound.h#1 $ */

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
@brief Initiates enable distance bound request to SSC.

Function initiates enable distance bound request to SSC.

@param[in] pz_SscData     Pointer to the SSC data.
@param[in] pz_Txn         Pointer to the distance bound data request transaction
                          data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscDistanceBoundMakeEnableRequest
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_EnableDistanceBoundTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates set distance bound request to SSC.

Function initiates set distance bound request to SSC.

@param[in] pz_SscData     Pointer to the SSC data.
@param[in] pz_Txn         Pointer to the distance bound data request transaction
                          data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscDistanceBoundMakeSetRequest
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_SetDistanceBoundTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates get distance bound request to SSC.

Function initiates get distance bound request to SSC.

@param[in] pz_SscData     Pointer to the SSC data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscDistanceBoundMakeGetRequest
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  int32 l_ServiceTxnId
);

/**
@brief Handles enable response data for distance bound request

This function handles enable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_DbData  Pointer to the client distance bound data
@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscDistanceBoundHandleEnableResponse
(
  slim_SscSamConnectionDataStructType *pz_DbData,
  const slim_SscEnableDbRespMsgT *pz_RespMsg
);

/**
@brief Handles disable response data for distance bound request

This function handles disable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_DbData  Pointer to the client distance bound data
@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscDistanceBoundHandleDisableResponse
(
  slim_SscSamConnectionDataStructType *pz_DbData,
  const slim_SscDisableDbRespMsgT *pz_RespMsg
);

/**
@brief Handles set response data for distance bound request

This function handles set response. Response is converted and forwarded to
SLIM client.

@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscDistanceBoundHandleSetResponse
(
  const slim_SscSetDbRespMsgT *pz_RespMsg
);

/**
@brief Handles get response data for distance bound request

This function handles get response. Response is converted and forwarded to
SLIM client.

@param[in] pz_RespMsg Pointer to the response message data
*/
void slim_SscDistanceBoundHandleGetResponse
(
  const slim_SscGetDbRespMsgT *pz_RespMsg
);

/**
@brief Handles distance bound data indication

This function handles distance bound data indication. Data is converted and
forwarded to SLIM client.

@param[in] pz_IndMsg Pointer to the indication message
*/
void slim_SscDistanceBoundHandleIndication
(
  const slim_SscDbIndMsgT *pz_IndMsg
);

//! @}

#endif /* #ifndef SLIM_SSC_DISTANCE_BOUND_H */
