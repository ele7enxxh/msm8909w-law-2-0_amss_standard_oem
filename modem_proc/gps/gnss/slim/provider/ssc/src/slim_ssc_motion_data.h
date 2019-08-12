#ifndef SLIM_SSC_MOTION_DATA_H
#define SLIM_SSC_MOTION_DATA_H
/**
  @file
  @brief Module implementing the motion data request to sensor core.
 
  SLIM can use this module to start or stop the motion data streaming from SSC.

  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_motion_data.h#1 $ */

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
@brief Initiates motion data request to SSC.

Function initiates motion data request to SSC. If request is successfull,
SSC motion data streaming is either started or stopped.

@param  pz_SscData: Pointer to the client SSC motion data.
@param  pz_Txn: Pointer to motion data request transaction data.
@param  l_ServiceTxnId : Service transaction id.
@return SSC_SERVICE_SUCCESS if request is done successfully. Otherwise SSC error
code.
*/
slim_SscServiceErrorEnumType slim_SscMotionDataEnableReporting
(
  slim_SscMotionDataConnectionDataStructType *pz_SscData,
  const slim_EnableMotionDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Handles QMD indication.

Function handles QMD indication. Data is converted to SLIM data and forwarded
to client.

@param pz_SscData: Pointer to the client motion data.
@param pz_Msg: Indication message.
*/
void slim_SscMotionDataHandleQmdIndication
(
  slim_SscMotionDataConnectionDataStructType *pz_SscData,
  const slim_SscQmdIndMsgT *pz_Msg
);

//! @}

#endif /* #ifndef SLIM_SSC_MOTION_DATA_H */
