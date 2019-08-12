#ifndef SLIM_PE_PROVIDER_H
#define SLIM_PE_PROVIDER_H
/**
  @file
  @brief PE-based service provider factory interface.

  This module sends the sensor data requests received from SLIM clients to
  PE.

  @ingroup slim_PEProvider
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/pe/inc/slim_pe_provider.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_internal.h"
#include "slim_provider_data.h"

//! @addtogroup slim_PEProvider
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
@brief Initializes PE provider module.

Function initializes PE provider module.

@return Pointer to the PE provider implementation.
*/
slim_ServiceProviderInterfaceType *slim_PEInitialize
(
  void
);

//! @}

#endif /* #ifndef SLIM_PE_PROVIDER_H */
