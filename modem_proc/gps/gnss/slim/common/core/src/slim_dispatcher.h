#ifndef SLIM_DISPATCHER_H
#define SLIM_DISPATCHER_H
/**
@file
@brief Module for dispatching SLIM data for clients.

  This module provides functions for dispatching the sensor data to clients.
  Sensor providers should use these functions when sample data is received
  from sensor data sources.

@ingroup slim_CoreDispatcher
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/common/core/src/slim_dispatcher.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_client.h"
#include "slim_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreDispatcher
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
@brief Dispatches data messages.

This function should be used to dispatch indication messages to SLIM clients.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] pz_ClientRegistry   Pointer to the client registry.
@param[in] p_Handle            Client handle.
@param[in] e_Provider          Service data provider
@param[in] e_Service           SLIM service.
@param[in] e_MessageId         SLIM message id.
@param[in] w_DataSize          Size of the payload.
@param[in] p_Data              Pointer to the payload.
*/
void slim_DispatchData
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slim_ProviderEnumType e_Provider,
  slimServiceEnumT e_Service,
  slimMessageIdEnumT e_MessageId,
  uint16 w_DataSize,
  const void *p_Data
);

/**
@brief Function for dispatching error indication.

This function should be used to dispatch error indication messages
to SLIM clients.

@param[in] pz_ClientRegistry Pointer to the client registry.
@param[in] p_Handle          Client handle.
@param[in] e_Service         SLIM service.
@param[in] e_MsgId           SLIM message id.
@param[in] e_MsgError        SLIM error code.
*/
void slim_DispatchErrorIndication
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  slimMessageIdEnumT e_MsgId,
  slimErrorEnumT e_MsgError
);

/**
@brief Function for dispatching response.

This function should be used to dispatch response messages to SLIM clients.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] pz_ClientRegistry   Pointer to the client registry.
@param[in] p_Handle            Client handle.
@param[in] u_TxnId             Client transaction id.
@param[in] e_TxnType           Transaction type.
@param[in] e_Service           SLIM service.
@param[in] e_Provider          Provider sending the response.
@param[in] e_MsgError          Transaction result code.
@param[in] e_MessageId         Message id.
@param[in] w_MessageSize       Message size.
@param[in] p_Message           Pointer to message data (aligned).
*/
void slim_DispatchResponse
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  uint8 u_TxnId,
  slim_TxnEnumType e_TxnType,
  slimServiceEnumT e_Service,
  slim_ProviderEnumType e_Provider,
  slimErrorEnumT e_MsgError,
  slimMessageIdEnumT e_MessageId,
  uint16 w_MessageSize,
  const void *p_Message
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_DISPATCHER_H */
