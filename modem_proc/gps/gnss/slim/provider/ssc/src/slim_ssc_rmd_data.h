#ifndef SLIM_SSC_RMD_DATA_H
#define SLIM_SSC_RMD_DATA_H
/**
@file
@brief Module implementing the RMD data request to sensor core.
 
  SLIM can use this module to start or stop the RMD data streaming from SSC.

@ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_rmd_data.h#1 $ */

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
@brief Initiates RMD data request to SSC.

Function initiates RMD data request to SSC. If request is successful,
SSC RMD data streaming is either started or stopped.

@param[in] pz_SscData     Pointer to the client RMD SSC data.
@param[in] pz_Txn         Pointer to RMD data request transaction data.
@param[in] l_ServiceTxnId Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscRmdDataEnableReporting
(
  slim_SscSamConnectionDataStructType *pz_SscData,
  const slim_EnableQmdDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates RMD data request to SSC.

Function initiates RMD data request to SSC. If request is successful,
SSC RMD data streaming is either started or stopped. This function is intended
for use of other SSC service modules that utilize RMD data in the service they
are implementing (such as motion data).

@param[in] pz_QmdConnection      Pointer to the QMD connection data
@param[in] fn_ResponseCallback   Pointer to the response callback function.
@param[in] fn_IndicationCallback Pointer to the indication callback function.
@param[in] pz_Txn                Pointer to RMD data request transaction data.
@param[in] l_ServiceTxnId        Service transaction id.

@retval SSC_SERVICE_SUCCESS if request is done successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscRmdDataEnableReportingForCallback
(
  slim_SscSamConnectionDataStructType *pz_QmdConnection,
  qmi_client_recv_msg_async_cb fn_ResponseCallback,
  qmi_client_ind_cb fn_IndicationCallback,
  const slim_EnableQmdDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

//! @}

#endif /* #ifndef SLIM_SSC_RMD_DATA_H */
