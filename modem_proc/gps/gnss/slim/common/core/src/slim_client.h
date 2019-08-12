#ifndef SLIM_CLIENT_H
#define SLIM_CLIENT_H
/**
@file
@brief SLIM client connection handling API.

  This module provides routines for handling SLIM client connections.

  SLIM client handling routines are implemented in this module:
  - opening client connection
  - closing client connection
  - updating client service registration status
  - providing access to client data

@ingroup slim_CoreClientRegistry
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/common/core/src/slim_client.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_api.h"
#include "slim_provider_data.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreClientRegistry
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*! Struct for SLIM client service registration data */
typedef struct slim_ClientServiceDataStructType
{
  boolean b_Enabled;                         /*!< TRUE if service is enabled. */
  slim_ProviderEnumType e_UsedProvider;      /*!< Currently used provider. */
  slimServiceProviderMaskT q_ProviderStatus; /*!< Latest sent provider state for service. */
} slim_ClientServiceDataStructType;

/*! Struct for SLIM client data */
typedef struct slim_ClientStructType
{
  slimClientHandleT p_Handle;               /*!< Client handle */
  boolean b_Reserved;                       /*!< TRUE if this slot is reserved. */
  boolean b_Open;                           /*!< TRUE if client connection is opened */
  slimNotifyCallbackFunctionT fn_Callback;  /*!< Pointer to client callback function */
  slimOpenFlagsT q_ClientFlags;             /*!< Flags given by client. */
  uint64 t_CallbackData;                    /*!< Callback function data */
  slim_ClientServiceDataStructType z_ServiceData[SLIM_SERVICE_COUNT];
  /*!< Service registration data */
} slim_ClientStructType;

/*! Struct for SLIM client registry */
typedef struct slim_ClientRegistryStructType
{
  slim_ClientStructType z_Clients[SLIM_CLIENT_MAX_COUNT]; /*!< Client array */
  slim_MutexStructType z_ClientMutex;                     /*!< Client mutex */
} slim_ClientRegistryStructType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Registers SLIM client with the SLIM service.

This function validates the client arguments for the SLIM connection and
adds client to the SLIM client registry if a free slot is found. Client handle
is generated and provided back to client.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to an opaque handle used to identify this client.
@param  fn_Callback : A pointer to the client's callback function to process
received data.
@param  q_ClientFlags : Client given flags.
@param  t_CallbackData : This data is set by the client, and will be passed
unmodified as a parameter to the callback function.
@return eSLIM_SUCCESS if client was added successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_ClientOpen
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimNotifyCallbackFunctionT fn_Callback,
  slimOpenFlagsT q_ClientFlags,
  uint64 t_CallbackData
);

/**
@brief Check if client handle is registered

This function performs a client handle registration check. Client is registered
after successful @ref slim_ClientOpen call and until @ref slim_ClientRemove one.

@param[in] pz_ClientRegistry Pointer to the client registry.
@param[in] p_Handle          Pointer to the client handle.
@return TRUE if client is registered; FALSE otherwise.
*/
boolean slim_ClientIsPresent
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle
);

/**
@brief This function de-registers a client, and releases the client handle.

This function removes SLIM client from the client registry. The client handle
is invalid after a call to this function

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to the client handle.
@return eSLIM_SUCCESS if client was removed successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_ClientClose
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle
);

/**
@brief This function updates client service registration status

This function returns eSLIM_SUCCESS if the client registration status is
updated successfully.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to the client handle.
@param  e_Service: Sensor service.
@param  e_Provider: Sensor service provider.
@param  b_Register: TRUE if service is enabled, FALSE if service is disabled.
@return eSLIM_SUCCESS status was updated. Otherwise SLIM error code.
*/
slimErrorEnumT slim_ClientRegisterForService
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  slim_ProviderEnumType e_Provider,
  boolean b_Register
);

/**
@brief Provides client service registration status

This function provides client service registration status for the given
client handle and sensor service.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to the client handle.
@param  e_Service: Sensor service.
@return TRUE if service is enabled. FALSE if service is disabled.
*/
boolean slim_ClientGetServiceStatus
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service
);

/**
@brief Checks if client has any active services.

This function checks if client has registered for any sensor services.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to the client handle.
@return TRUE if active services are found. FALSE otherwise.
*/
boolean slim_ClientHasActiveServices
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle
);

/**
@brief Provides client connection status

This function provides client connection status for the given
client handle.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to the client handle.
@return TRUE if client connection is open. FALSE is client connection is closed.
*/
boolean slim_ClientGetConnectionStatus
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle
);

/**
@brief Removes client from the registry.

Function removes all stored client data from the registry.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to the client handle.
@return eSLIM_SUCCESS if client was removed successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_ClientRemove
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle
);

/**
@brief Provides type of the current provider for the given service.

Function provides type of the current provider for the given service.
Function returns SLIM_PROVIDER_NONE, if the service is disabled.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to the client handle.
@param  e_Service: Service to get the provider for.
@return Provider type. SLIM_PROVIDER_NONE, if the service is disabled.
*/
slim_ProviderEnumType slim_ClientGetCurrentProvider
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service
);

/**
@brief Sends notification to client

Function sends notification to SLIM client.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Pointer to the client handle.
@param  e_Service: SLIM service.
@param  e_Type: SLIM message type.
@param  e_MsgError: SLIM error code.
@param  e_MsgId: SLIM message id.
@param  w_Size: Payload size.
@param  u_TxnId: Client transaction id.
@param  p_Message: Pointer to the payload.
*/
void slim_ClientNotify
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  slimMessageTypeEnumT e_Type,
  slimErrorEnumT e_MsgError,
  slimMessageIdEnumT e_MsgId,
  uint16 w_Size,
  uint8 u_TxnId,
  const void *p_Message
);

/**
@brief Lists open clients.

Function saves handles of open clients to handle array given as the function
parameter. Count of open clients is stored to p_ClientCount.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  pp_HandleArray: Pointer to the handle array.
@param  q_MaxClientCount: Size of the handle array.
@return Count of open clients.
*/
uint32 slim_ClientListOpenClients
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT *pp_HandleArray,
  uint32 q_MaxClientCount
);

/**
@brief Returns TRUE if the given client requires SLIM time conversion.

Function returns TRUE if the given client requires SLIM time conversion from
provider time to local processor time.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Client handle
@return TRUE if time conversion is required. FALSE otherwise.
*/
boolean slim_ClientRequireTimeConversion
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle
);

/**
@brief Returns TRUE if the given client allows the use of remote SLIM providers.

Function returns TRUE if the given client allows the use of remote SLIM
providers.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Client handle
@return TRUE if the given client allows the use of remote SLIM providers.
FALSE otherwise.
*/
boolean slim_ClientAllowRemoteSlimProviders
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle
);

/**
@brief Stores the latest provider status to client data.

Function stores the latest provider status for a given service to client data.
Provider status can be checked when sending updates for providers service status
so that unnecessary updates can be avoided.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Client handle
@param  e_Service: SLIM service
@param  q_ProviderStatus: New provider status
*/
void slim_ClientSetProviderStatus
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  slimServiceProviderMaskT q_ProviderStatus
);

/**
@brief Returns the latest provider status sent to client.

Function returns the latest provider status for a given service sent to client.

@param  pz_ClientRegistry: Pointer to the client registry.
@param  p_Handle: Client handle
@param  e_Service: SLIM service
@return Client provider status.
*/
slimServiceProviderMaskT slim_ClientGetProviderStatus
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service
);

/**
@brief Constructs notification message and performs callback notification

Function translates input arguments into notification message and invokes
callback function.

@param[in] fn_Callback    Client callback function to invoke.
@param[in] t_CallbackData Connection-specific callback argument.
@param[in] e_Service      SLIM service.
@param[in] e_Type         SLIM message type.
@param[in] e_MsgError     SLIM error code.
@param[in] e_MsgId        SLIM message id.
@param[in] w_Size         Payload size.
@param[in] u_TxnId        Client transaction id.
@param[in] p_Message      Pointer to the payload.
*/
void slim_ClientNotifyCallback
(
  slimNotifyCallbackFunctionT fn_Callback,
  uint64 t_CallbackData,
  slimServiceEnumT e_Service,
  slimMessageTypeEnumT e_Type,
  slimErrorEnumT e_MsgError,
  slimMessageIdEnumT e_MsgId,
  uint16 w_Size,
  uint8 u_TxnId,
  const void *p_Message
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_CLIENT_H */
