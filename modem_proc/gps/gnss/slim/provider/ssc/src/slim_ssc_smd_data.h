#ifndef SLIM_SSC_SMD_DATA_H
#define SLIM_SSC_SMD_DATA_H
/**
@file
@brief Module implementing the SMD data request to sensor core.
 
  SLIM can use this module to start or stop the SMD data streaming from SSC.

@ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_smd_data.h#1 $ */

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
@brief Initiates SMD data request to SSC.

Function initiates SMD data request to SSC. If request is successfull,
SSC SMD data streaming is either started or stopped.

@param  pz_SscData: Pointer to the client SMD SSC data.
@param  pz_Txn: Pointer to SMD data request transaction data.
@param  l_ServiceTxnId : Service transaction id.
@return SSC_SERVICE_SUCCESS if request is done successfully. Otherwise SSC error
code.
*/
slim_SscServiceErrorEnumType slim_SscSmdDataEnableReporting
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_EnableSmdDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Handles enable response data for SMD request

This function handles enable response. Response is converted and forwarded to
SLIM client.

@param  pz_SmdData: Pointer to the client SMD data
@param  pz_RespMsg: Pointer to the response message data
*/
void slim_SscSmdDataHandleEnableResponse
(
  slim_SscSamConnectionDataStructType *pz_SmdData,
  const slim_SscEnableSmdRespMsgT *pz_RespMsg
);

/**
@brief Handles disable response data for SMD request

This function handles disable response. Response is converted and forwarded to
SLIM client.

@param  pz_SmdData: Pointer to the client SMD data
@param  pz_RespMsg: Pointer to the response message data
*/
void slim_SscSmdDataHandleDisableResponse
(
  slim_SscSamConnectionDataStructType *pz_SmdData,
  const slim_SscDisableSmdRespMsgT *pz_RespMsg
);

/**
@brief Handles SMD indication data

This function handles SMD indication data. Data is converted and forwarded
to SLIM client.

@param  pz_IndMsg: Pointer to the response message data
*/
void slim_SscSmdDataHandleIndication
(
  const slim_SscSmdIndMsgT *pz_IndMsg
);

//! @}

#endif /* #ifndef SLIM_SSC_SMD_DATA_H */
