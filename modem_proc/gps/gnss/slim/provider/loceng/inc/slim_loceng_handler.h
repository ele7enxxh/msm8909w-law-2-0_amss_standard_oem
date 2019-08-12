#ifndef SLIM_LOCENG_HANDLER_H
#define SLIM_LOCENG_HANDLER_H
/**
  @file
  @brief Interface for LocEng message handling in SLIM.

  This module converts locEng structures to SLIM-structures and forwards
  those to client.

  @ingroup slim_LocEng
  @sa slim_QMILOCProvider
  @sa slim_QMIPDSProvider
*/
/*============================================================================
               Copyright (c) 2013,2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/loceng/inc/slim_loceng_handler.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "locEng_sm_common_msg_v01.h"
#include "slim_client_types.h"
#include "slim_provider_data.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_LocEng
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/*!
@brief Function type for forwarding converted data

@param[in] e_Service   SLIM Service.
@param[in] e_MessageId SLIM message id.
@param[in] w_DataSize  Payload size.
@param[in] p_Data      Payload data.

@sa slim_LocEngInjectMotionData
@sa slim_LocEngInjectPedometerData
@sa slim_LocEngInjectVehicleData
*/
typedef void slim_LocEngDispatchDataFunctionT
(
  slimServiceEnumT e_Service,
  slimMessageIdEnumT e_MessageId,
  uint16 w_DataSize,
  const void *p_Data
);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Function for injecting motion data.

Function converts locEng motion data to SLIM motion data and forwards it
to SLIM client.

@param  pz_LocEngMotionData: Pointer to the LocEng motion data.
@param  fn_Dispatch: Function pointer for forwarding converted data
*/
void slim_LocEngInjectMotionData
(
  const locEngMotionDataStructT_v01 *pz_LocEngMotionData,
  slim_LocEngDispatchDataFunctionT *fn_Dispatch
);

/**
@brief Function for injecting pedometer data.

Function converts locEng pedometer data to SLIM pedometer data and forwards it
to SLIM client.

@param  pz_LocEngPedometerData: Pointer to the LocEng pedometer data.
@param  fn_Dispatch: Function pointer for forwarding converted data
*/
void slim_LocEngInjectPedometerData
(
  const locEngPedometerReportReqMsgT_v01 *pz_LocEngPedometerData,
  slim_LocEngDispatchDataFunctionT *fn_Dispatch
);

/**
@brief Function for injecting vehicle sensor data.

Function converts locEng vehicle data to SLIM vehicle data and forwards it
to SLIM client.

@param  pz_LocEngVehicleData: Pointer to the LocEng vehicle sensor data.
@param  fn_Dispatch: Function pointer for forwarding converted data
*/
void slim_LocEngInjectVehicleData
(
  const locEngInjectVehicleSensorDataReqMsgT_v01 *pz_LocEngVehicleData,
  slim_LocEngDispatchDataFunctionT *fn_Dispatch
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_LOCENG_HANDLER_H */
