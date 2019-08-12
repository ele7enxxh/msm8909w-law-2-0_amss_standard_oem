#ifndef SLIM_PROVIDER_H
#define SLIM_PROVIDER_H
/**
@file
@brief Service provider registry operations.

  This module configures and keeps track of different sensor service
  providers.

@ingroup slim_CoreProviderRegistry
*/
/*============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/common/core/src/slim_provider.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_provider_data.h"
#include "slim_utils.h"
#include "slim_transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreProviderRegistry
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*! Struct for sensor provider data */
typedef struct slim_ServiceProviderStructType
{
  /** Actual provider implementation */
  slim_ServiceProviderInterfaceType *pz_Impl;
  /** Latest time-sync request transmitted time */
  uint64                             t_LocalTxTime;
  /** Time sync transaction identifier */
  int32                              l_TimeTxId;
  /** Time-sync filter */
  slim_TimeSyncFilterStateStructType z_Filter;
} slim_ServiceProviderStructType;

/*! Struct for service configuration data */
typedef struct slim_ServiceConfigurationStructType
{
  /*! Mask of the available providers for this service */
  uint32 q_AvailableProviderMask;

} slim_ServiceConfigurationStructType;

/*! Struct for SLIM provider map */
typedef struct slim_ProviderRegistryStructType
{
  /*! Preferred provider. Default is SSC. */
  slim_ProviderSettingEnumType e_ProviderSetting;
  /*! Service providers. Array index corresponds to provider type. */
  slim_ServiceProviderStructType z_Providers[SLIM_PROVIDER_COUNT];
  /*! Service-provider map.
      Array index corresponds to value in enum #slimServiceEnumT. */
  slim_ServiceConfigurationStructType z_ServiceProviderMap[SLIM_SERVICE_COUNT];
  /*! Provider mutex */
  slim_MutexStructType z_ProviderMutex;
} slim_ProviderRegistryStructType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
@brief Initializes provider registry.

Function initializes provider registry.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
*/
void slim_ProviderInitialize
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry
);

/**
@brief Adds new provider to the registry.

Function adds new provider to the provider registry. This function should be
called once per each provider when SLIM initializes.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Provider          Provider type.
@param[in] pz_ProviderImpl     Pointer to the new provider implementation.
*/
void slim_ProviderAdd
(
  slim_ProviderRegistryStructType   *pz_ProviderRegistry,
  slim_ProviderEnumType              e_Provider,
  slim_ServiceProviderInterfaceType *pz_ProviderImpl
);

/**
@brief Configures providers for the given sensor service.

Function sets the active service providers for the given service.
This function should be called when there are changes in the available providers
for the given service.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Service           Service to configure.
@param[in] q_NewProviderMask   Providers to configure for the service.
*/
void slim_ProviderConfigure
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slimServiceEnumT                 e_Service,
  uint32                           q_NewProviderMask
);

/**
@brief Returns mask for currently available providers for the given service.

Function returns mask for currently available providers for the given service.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Service           SLIM service

@return Mask of currently available providers.
*/
uint32 slim_ProviderGetCurrentProvidersForService
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slimServiceEnumT                 e_Service
);

/**
@brief Returns updated mask for available providers for the given service.

Function returns updated mask for available providers for the given service.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Service           SLIM service

@return Updated mask for available providers.
*/
uint32 slim_ProviderGetUpdatedProvidersForService
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slimServiceEnumT                 e_Service
);

/**
@brief Returns mask for currently available services.

Function returns mask for currently available SLIM services.

@param[in] pz_ProviderRegistry    Pointer to the provider registry.
@param[in] v_AllowRemoteProviders TRUE if client allows usage of remote
                                  SLIM providers.

@return Mask of currently available services.
*/
slimAvailableServiceMaskT slim_ProviderGetCurrentServices
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  boolean                          v_AllowRemoteProviders
);

/**
@brief Returns currently configured provider for sensor service.

Function returns the provider type that is currently configured
for providing the given sensor service.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Service           Sensor service.
@param[in] q_AllowedProviders  Mask of allowed providers by client.
@param[in] v_AllowRemoteProviders TRUE if client allows usage of remote
                                  SLIM providers.

@return SLIM provider type.
@retval SLIM_PROVIDER_NONE if service is not supported.
*/
slim_ProviderEnumType slim_ProviderForService
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slimServiceEnumT                 e_Service,
  slimServiceProviderMaskT         q_AllowedProviders,
  boolean                          v_AllowRemoteProviders
);

/**
@brief Returns default provider for the given providers.

Function returns the default provider type for the given providers.

@param[in] pz_ProviderRegistry    Pointer to the provider registry.
@param[in] q_AvailableProviders   SLIM provider mask
@param[in] v_AllowRemoteProviders TRUE if client allows usage of remote
                                  SLIM providers.
@return SLIM provider type.
@retval SLIM_PROVIDER_NONE if service is not supported.
*/
slim_ProviderEnumType slim_DefaultProviderForService
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  uint32                           q_AvailableProviders,
  boolean                          v_AllowRemoteProviders
);

/**
@brief Opens provider connections for a specific client

The function initializes all provider connections for a specific client.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] p_Handle            Pointer to the client handle
*/
void slim_ProviderOpenClientConnection
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slimClientHandleT                p_Handle
);

/**
@brief Closes provider connections for a specific client

The function makes provider cleanup when client closes a connection if needed.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] p_Handle            Pointer to the client handle
*/
void slim_ProviderCloseClientConnection
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slimClientHandleT                p_Handle
);

/**
@brief Forwards message to given provider.

The function forwards message to given provider.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Provider          Provider type.
@param[in] q_MessageId         Message id.
@param[in] q_MessageSize       Size of the message.
@param[in] ca_Message          Pointer to the message.
*/
void slim_ProviderHandleMessage
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ProviderEnumType            e_Provider,
  uint32                           q_MessageId,
  uint32                           q_MessageSize,
  const void                      *ca_Message
);

/**
@brief Checks if the given provider has time in common with SLIM.

The function checks if the given provider has time in common with SLIM.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Provider          Service provider to query.

@return TRUE if provider time is common with SLIM.
*/
boolean slim_ProviderTimeIsCommonWithSlim
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ProviderEnumType            e_Provider
);

/**
@brief Requests time update for the given sensor service.

The function requests time update for the provider responsible for
the given sensor service.

This method opens a time update transaction which shall be closed with a
corresponding #slim_ProviderHandleTimeUpdate invocation.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Provider          Service provider to update time for.
@param[in] e_Service           Service to update time for.

@sa slim_ProviderHandleTimeUpdate
*/
void slim_ProviderUpdateTime
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ProviderEnumType            e_Provider,
  slimServiceEnumT                 e_Service
);

/**
@brief Handles time update result for the given provider.

The function handles time update result for the given provider.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Provider          Provider which updates time.
@param[in] cz_TimeResp         Time response data.

@sa slim_ProviderUpdateTime
*/
void slim_ProviderHandleTimeUpdate
(
  slim_ProviderRegistryStructType          *pz_ProviderRegistry,
  slim_ProviderEnumType                     e_Provider,
  const slimGetProviderTimeResponseStructT *cz_TimeResp
);

/**
@brief Converts remote time to local time

This function is used to convert the provider timestamps to SLIM core timestamps
applying monotonous offset calculated from the sample frequency.

@param[in]  pz_ProviderRegistry   Pointer to the provider registry.
@param[in]  p_Handle              Pointer to the client handle.
@param[in]  e_Provider            Sensor provider who sends the time data.
@param[in]  e_Service             Sensor service to do the conversion for.
@param[in]  t_RemoteTimeFirst_ms  Remote time of first sample in milliseconds.
@param[in]  q_RemoteTimeLastOffset_ms  Offset of last and first sample in remote
                                       time (milliseconds).
@param[out] pt_LocalTimeOut_ms    Local time (of first sample) in milliseconds
                                  is stored to this pointer if function is
                                  successful.
@param[out] pq_FlagsOut           Time flags are updated to this pointer value
                                  if function is successful.\n
                                  Possible Flags:
                                  - #SLIM_FLAGS_MASK_TIME_JUMP
                                  - #SLIM_FLAGS_MASK_TIME_UNC_HIGH

@retval TRUE if conversion succeeds.
*/
boolean slim_ProviderConvertTimeMt
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slimClientHandleT                p_Handle,
  slim_ProviderEnumType            e_Provider,
  slimServiceEnumT                 e_Service,
  uint64                           t_RemoteTimeFirst_ms,
  uint32                           q_RemoteTimeLastOffset_ms,
  uint64                          *pt_LocalTimeOut_ms,
  uint32                          *pq_FlagsOut
);

/**
@brief Notifies provider of service status changes.

This function is called when enable/disable response for service reporting
activation is received.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Provider          Provider type.
@param[in] p_Handle            Pointer to the client handle.
@param[in] e_Service           Service that was enabled/disabled.
@param[in] v_Enable            TRUE if service was enabled. FALSE otherwise.
*/
void slim_ProviderServiceStatusChanged
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ProviderEnumType            e_Provider,
  slimClientHandleT                p_Handle,
  slimServiceEnumT                 e_Service,
  boolean                          v_Enable
);

/**
@brief Forwards request to provider implementation.

The function requests currently configured sensor provider to enable/disable
requested service reporting or perform another interface operation.

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_Provider          Service provider.
@param[in] e_Service           Service type or #eSLIM_SERVICE_NONE if request
                               is not service specific.
@param[in] e_TxnType           Transaction type.
@param[in] ca_Request          Request-specific transaction data.
@param[in] q_RequestSize       Size of the transaction data.
@param[in] l_ServiceTxnId      Reserved service transaction id.

@return eSLIM_SUCCESS if request is successful.
*/
slimErrorEnumT slim_ProviderMakeRequest
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ProviderEnumType            e_Provider,
  slimServiceEnumT                 e_Service,
  slim_TxnEnumType                 e_TxnType,
  const void                      *ca_Request,
  uint32                           q_RequestSize,
  int32                            l_ServiceTxnId
);

/**
 * @brief Forwards request to cancel transaction to provider implementation.
 *
 * @param[in] pz_ProviderRegistry Pointer to the provider registry.
 * @param[in] e_Provider          Service provider.
 * @param[in] l_ServiceTxnId      Reserved service transaction id.
 *
 * @retval eSLIM_SUCCESS  Transaction has been cancelled.
 * @retval eSLIM_ERROR... Transaction can't be cancelled.
 */
slimErrorEnumT slim_ProviderCancelTransactionRequest
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ProviderEnumType            e_Provider,
  int32                            l_ServiceTxnId
);

/**
@brief Sets the current provider setting (SSC/Native).

The function sets the current provider setting (SSC/Native).

@param[in] pz_ProviderRegistry Pointer to the provider registry.
@param[in] e_ProviderSetting   New provider setting.

@return TRUE if the value was actually changed.
*/
boolean slim_ProviderSetProviderSetting
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ProviderSettingEnumType     e_ProviderSetting
);

/**
@brief Returns mask of available providers for client.

Function returns mask of type slimServiceProviderMaskT of available providers
for client.

@param[in] pz_ProviderRegistry    Pointer to the provider registry.
@param[in] q_AvailableProviders   Available SLIM service providers.
@param[in] v_AllowRemoteProviders TRUE if client allows usage of remote
                                  SLIM providers.

@return Mask of type #slimServiceProviderMaskT of available providers to be sent
        to SLIM client.
*/
slimServiceProviderMaskT slim_ProviderGetClientProviderMask
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  uint32                           q_AvailableProviders,
  boolean                          v_AllowRemoteProviders
);

/**
 * @brief Mark or clear time sync transaction identifier
 *
 * Each provider can have not more than one time sync at a time. This function
 * associates time sync transaction with provider or resets the association.
 *
 * @param[in] pz_ProviderRegistry Pointer to the provider registry.
 * @param[in] e_Provider          Provider.
 * @param[in] l_ServiceTxId       Time sync transaction identifier or
 *                                #SLIM_INVALID_TXN_ID
 */
void slim_ProviderSetTimeTxId
(
  slim_ProviderRegistryStructType *pz_ProviderRegistry,
  slim_ProviderEnumType            e_Provider,
  int32                            l_ServiceTxId
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_PROVIDER_H */
