#ifndef SLIM_CONFIGURE_H
#define SLIM_CONFIGURE_H
/**
@file
@brief SLIM service configuration API.

  Module for SLIM core to configure SLIM services.

@ingroup slim_CoreConfigure
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/common/core/src/slim_configure.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_client.h"
#include "slim_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreConfigure
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/*!
@brief  Core callback function type

This function is used by SLIM core to dispatch both indication and response
messages.

@param[in] p_Handle    Client handle.
@param[in] e_Provider  Sensor data provider.
@param[in] e_Service   SLIM service.
@param[in] e_Error     SLIM status code.
@param[in] e_MessageId SLIM message id.
@param[in] w_DataSize  Size of the payload.
@param[in] p_Data      Pointer to the payload.

@sa slim_CoreDispatchData
*/
typedef void slim_ConfigureDispatchDataFunctionType
(
  slimClientHandleT p_Handle,
  slim_ProviderEnumType e_Provider,
  slimServiceEnumT e_Service,
  slimErrorEnumT e_Error,
  slimMessageIdEnumT e_MessageId,
  uint16 w_DataSize,
  const void *p_Data
);

/*! Service configuration argument data */
typedef struct
{
  slim_ProviderRegistryStructType *pz_Providers;
  /**< @brief SLIM Core Provider registry */
  slim_ClientRegistryStructType *pz_Clients;
  /**< @brief SLIM Core Client registry */
  slimServiceEnumT e_Service;
  /**< @brief Service type */
  slim_ConfigureDispatchDataFunctionType *fn_Dispatch;
  /**< @brief Core callback function for dispatching data */
} slim_ConfigureArgumentStructType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Updates given SLIM service configuration.

Function updates given SLIM service configuration.

@param[in] pz_ConfData Service configuration data.
*/
void slim_ConfigureUpdateService
(
  const slim_ConfigureArgumentStructType *pz_ConfData
);

/**
@brief Notifies client of changes in SLIM service status.

Function notifies client of any changes in SLIM service status.

@param[in] pz_ConfData            Service configuration data.
@param[in] p_Handle               Client handle.
@param[in] q_AvailableProviders   Mask containing the currently available
                                  providers.
@param[in] q_CurrentServices      Mask containing the currently available
                                  services.
@param[in] b_AllowRemoteProviders TRUE if client allows usage of remote SLIM
                                  providers.
@param[in] b_ServiceDisabled      TRUE if service was disabled by SLIM.
*/
void slim_ConfigureSendServiceStatus
(
  const slim_ConfigureArgumentStructType *pz_ConfData,
  slimClientHandleT p_Handle,
  uint32 q_AvailableProviders,
  slimAvailableServiceMaskT q_CurrentServices,
  boolean b_AllowRemoteProviders,
  boolean b_ServiceDisabled
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_CONFIGURE_H */
