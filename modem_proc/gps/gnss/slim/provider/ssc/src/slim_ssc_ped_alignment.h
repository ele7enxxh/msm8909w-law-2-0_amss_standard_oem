#ifndef SLIM_SSC_PED_ALIGNMENT_H
#define SLIM_SSC_PED_ALIGNMENT_H
/**
@file
@brief Module implementing the pedestrian alignment data request to sensor core.
 
  SLIM can use this module to start or stop the pedestrian alignment data
  streaming from SSC.

@dependencies
 - FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT -- Enable pedestrial alignment support.

@ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_ped_alignment.h#1 $ */

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
@brief Initiates pedestrian alignment request to SSC.

Function initiates pedestrian alignment request to SSC. If request is
successful, SSC pedestrian alignment data streaming is either started
or stopped.

@param[in] pz_SscData     Pointer to the client SMD SSC data.
@param[in] pz_Txn         Pointer to pedestrian alignment request transaction data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscPedestrianAlignmentEnableReporting
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_EnablePedestrianAlignmentTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

#ifdef FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT
/**
@brief Handles enable response data for ped alignment request

This function handles enable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_PedAlignmentData Pointer to the client ped alignment data
@param[in] pz_RespMsg          Pointer to the response message data
*/
void slim_SscPedAlignmentHandleEnableResponse
(
  slim_SscSamConnectionDataStructType *pz_PedAlignmentData,
  const slim_SscEnablePedAlignmentRespMsgT *pz_RespMsg
);

/**
@brief Handles disable response data for ped alignment request

This function handles disable response. Response is converted and forwarded to
SLIM client.

@param[in] pz_PedAlignmentData Pointer to the client ped alignment data
@param[in] pz_RespMsg          Pointer to the response message data
*/
void slim_SscPedAlignmentHandleDisableResponse
(
  slim_SscSamConnectionDataStructType *pz_PedAlignmentData,
  const slim_SscDisablePedAlignmentRespMsgT *pz_RespMsg
);

/**
@brief Handles ped alignment indication data

This function handles ped alignment indication data. Data is converted and
forwarded to SLIM client.

@param[in] pz_IndMsg Pointer to the indication message
*/
void slim_SscPedAlignmentHandleIndication
(
  const slim_SscPedAlignmentIndMsgT *pz_IndMsg
);
#endif /* FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT */

//! @}

#endif /* #ifndef SLIM_SSC_PED_ALIGNMENT_H */
