#ifndef SLIM_PE_CONVERSION_H
#define SLIM_PE_CONVERSION_H
/**
  @file
 
  Implementation of data conversion from PE structures to SLIM structures.
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/pe/src/slim_pe_conversion.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "mgp_provider_api.h"
#include "slim_client_types.h"

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
@brief Converts SLIM magnetic-field data request to PE structure.

Function converts SLIM magnetic-field data request to PE structure.

@param[in]  pz_SlimRequest  Pointer to the SLIM data.
@param[out] pz_PeRequestOut Pointer to the PE data.
*/
void slim_PEConvertMagneticFieldDataRequest
(
  const slimEnableMagneticFieldDataRequestStructT *pz_SlimRequest,
  mgp_EnableEarthMagFieldDataRequestStructT *pz_PeRequestOut
);

/**
@brief Converts PE magnetic-field data indication to SLIM structure.

Function converts PE magnetic-field data indication to SLIM structure.

@param[in]  pz_PeData      Pointer to the PE data.
@param[out] pz_SlimDataOut Pointer to the SLIM data.
*/
void slim_PEConvertMagneticFieldDataIndication
(
  const mgp_EarthMagFieldDataStructT *pz_PeData,
  slimMagneticFieldDataStructT *pz_SlimDataOut
);

/**
@brief Converts PE service status indication to SLIM available service
mask.

Function converts PE service status indication to SLIM available service
mask.

@param[in]  pz_PeData         Pointer to the PE data.
@param[out] pq_ServiceMaskOut Pointer to the SLIM service mask.
*/
void slim_PEConvertServiceStatusIndication
(
  const mgp_ServiceStatusEventStructT *pz_PeData,
  slimAvailableServiceMaskT *pq_ServiceMaskOut
);

//! @}

#endif /* #ifndef SLIM_PE_CONVERSION_H */
