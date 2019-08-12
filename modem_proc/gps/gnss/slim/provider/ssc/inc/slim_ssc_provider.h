#ifndef SLIM_SSC_PROVIDER_H
#define SLIM_SSC_PROVIDER_H
/**
  @file
  @brief Implementation of SSC-based sensor data requests.

 This module implements the sensor data request handling when sensor provider
 is SSC.


  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/inc/slim_ssc_provider.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_provider_data.h"
#include "slim_ssc_provider_data.h"

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
@brief Initializes SSC provider module.

Function initializes SSC provider module.

@return Pointer to the SSC provider implementation.
*/
slim_ServiceProviderInterfaceType *slim_SscInitialize
(
  void
);

/**
@brief Converts sample time base to milliseconds.

Function converts sample time base to milliseconds.

@param[in] q_TimeInClockTicks Slow clock value in ticks.
@return Clock value in milliseconds.
*/
uint64 slim_SscTimeBaseToMilliseconds
(
  uint32 q_TimeInClockTicks
);

/**
@brief Converts sample timestamp to milliseconds.

Function converts sample timestamp to milliseconds.

@param[in] q_TimeInClockTicks Slow clock value in ticks.
@return Clock value in milliseconds.
*/
uint64 slim_SscTimestampToMilliseconds
(
  uint32 q_TimeInClockTicks
);

/**
@brief Notify function for time request response

SLIM providers should use this function to notify of time request results.

@param[in] l_TransactionId           Transaction id for SLIM core.
@param[in] q_RemoteTimeTicks         Remote time in clock ticks.
@param[in] q_RemoteTimeRolloverCount Rollover count of remote time.
*/
void slim_SscNotifyTimeResponse
(
  int32 l_TransactionId,
  uint32 q_RemoteTimeTicks,
  uint32 q_RemoteTimeRolloverCount
);

/**
@brief Checks if SSC data should be throttled.

Function checks if SSC data should be throttled.

@param[in] p_Handle        SLIM client handle.
@param[in] e_Service       SLIM service type.
@param[in] t_DataTimestamp Current timestamp in milliseconds.

@retval TRUE if SSC data should be throttled.
@retval FALSE if data should be forwarded to client.
*/
boolean slim_SscThrottleData
(
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  uint64 t_DataTimestamp
);

//! @}

#endif /* #ifndef SLIM_SSC_PROVIDER_H */
