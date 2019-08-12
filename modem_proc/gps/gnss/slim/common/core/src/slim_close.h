#ifndef SLIM_CLOSE_H
#define SLIM_CLOSE_H
/**
@file
@brief This module disables all sensor services.

  Module handles disabling of sensor services when client closes a connection.

@ingroup slim_CoreClientRegistry
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/common/core/src/slim_close.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_client.h"

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

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Function for disabling SLIM service.

Function sends disable request for all SLIM services client has enabled.

@param  pz_ClientRegistry: Pointer to the SLIM client registry.
@param  p_Handle: Pointer to the client handle.
*/
void slim_CloseDisableAllServices
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle
);

/**
@brief Function for disabling given SLIM service for the given client.

Function sends disable request for the given SLIM service.

@param  pz_ClientRegistry: Pointer to the SLIM client registry.
@param  p_Handle: Pointer to the client handle.
@param  e_Service: Service
*/
void slim_CloseDisableService
(
  slim_ClientRegistryStructType *pz_ClientRegistry,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_CLOSE_H */
