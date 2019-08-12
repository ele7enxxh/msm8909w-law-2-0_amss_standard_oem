#ifndef SLIM_SAMLITE_PROVIDER_H
#define SLIM_SAMLITE_PROVIDER_H
/**
  @file
  @brief SAMLite-based provider factory interface.

  Implementation of SAMLite-based sensor data requests.

  This module sends the sensor data requests received from SLIM clients to
  SAMLite.

  @ingroup slim_SAMLProvider
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/samlite/inc/slim_samlite_provider.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_internal.h"
#include "slim_provider_data.h"

#ifdef __cplusplus
extern "C" {
#endif

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
@brief Initializes SAMLite provider module.

Function initializes SAMLite provider module.

@return Pointer to the SAMLite provider implementation.
@ingroup slim_SAMLProvider
*/
slim_ServiceProviderInterfaceType *slim_SAMLiteInitialize
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_SAMLITE_PROVIDER_H */
