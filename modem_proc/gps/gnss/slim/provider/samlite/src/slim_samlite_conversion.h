#ifndef SLIM_SAMLITE_CONVERSION_H
#define SLIM_SAMLITE_CONVERSION_H
/**
  @file
 
  Implementation of data conversion from SAMLite structures to SLIM structures.
 
  @ingroup slim_SAMLProvider
*/
/*============================================================================
               Copyright (c) 2014 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
               Copyright (c) 2015 Qualcomm Technologies, Inc.  
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/samlite/src/slim_samlite_conversion.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "saml_client_types.h"
#include "slim_client_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_SAMLProvider
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
@brief Converts SLIM motion data request to SAMLite structure.

Function converts SLIM motion data request to SAMLite structure.

@param[in]  pz_SlimRequest    Pointer to the SLIM data.
@param[out] pz_SamlRequestOut Pointer to the SAMLite data.
*/
void slim_SAMLiteConvertMotionDataRequest
(
  const slimEnableMotionDataRequestStructT *pz_SlimRequest,
  saml_EnableMotionDataRequestStructT *pz_SamlRequestOut
);

/**
@brief Converts SLIM pedometer request to SAMLite structure.

Function converts SLIM pedometer request to SAMLite structure.

@param[in]  pz_SlimRequest    Pointer to the SLIM data.
@param[out] pz_SamlRequestOut Pointer to the SAMLite data.
*/
void slim_SAMLiteConvertPedometerRequest
(
  const slimEnablePedometerRequestStructT *pz_SlimRequest,
  saml_EnablePedometerRequestStructT *pz_SamlRequestOut
);

/**
@brief Converts SLIM ped alignment request to SAMLite structure.

Function converts SLIM ped alignment request to SAMLite structure.

@param[in]  pz_SlimRequest        Pointer to the SLIM data.
@param[out] pz_SamlRequestOut     Pointer to the SAMLite data.
@param[in]  q_SupportedAlgorithms Currently supported ped alignment algorithms
*/
void slim_SAMLiteConvertPedAlignmentRequest
(
  const slimEnablePedestrianAlignmentRequestStructT *pz_SlimRequest,
  saml_EnablePedestrianAlignmentRequestStructT *pz_SamlRequestOut,
  slimPedAlignmentInputTypeMaskT q_SupportedAlgorithms
);

/**
@brief Converts SAMLite pedestrian alignment response to SLIM structure.

Function converts SAMLite pedestrian alignment response to SLIM structure.

@param[in]  pz_SamlResponse    Pointer to the SAMLite data.
@param[out] pz_SlimResponseOut Pointer to the SLIM data.
*/
void slim_SAMLiteConvertPedAlignmentResponse
(
  const saml_PedestrianAlignmentEnableResponseStructT *pz_SamlResponse,
  slimPedestrianAlignmentEnableResponseStructT *pz_SlimResponseOut
);

/**
@brief Converts SAMLite motion data indication to SLIM structure.

Function converts SAMLite motion data indication to SLIM structure.

@param[in]  pz_SamlData    Pointer to the SAMLite data.
@param[out] pz_SlimDataOut Pointer to the SLIM data.
*/
void slim_SAMLiteConvertMotionDataIndication
(
  const saml_MotionDataStructT *pz_SamlData,
  slimMotionDataStructT *pz_SlimDataOut
);

/**
@brief Converts SAMLite pedometer indication to SLIM structure.

Function converts SAMLite pedometer indication to SLIM structure.

@param[in]  pz_SamlData            Pointer to the SAMLite data.
@param[out] pz_SlimDataOut         Pointer to the SLIM data.
@param[in]  t_PedometerResetTimeMs Last pedometer reset time.
*/
void slim_SAMLiteConvertPedometerIndication
(
  const saml_PedometerDataStructT *pz_SamlData,
  slimPedometerDataStructT *pz_SlimDataOut,
  uint64 t_PedometerResetTimeMs
);

/**
@brief Converts SAMLite ped alignment input type to SLIM input type.

Function converts SAMLite ped alignment input type to SLIM input type.

@param[in]  e_SamlType     SAMLite data.
@param[out] pe_SlimTypeOut Pointer to the SLIM data.
*/
void slim_SAMLiteConvertPedAlignmentInputType
(
  saml_PedAlignmentInputTypeEnumT e_SamlType,
  slimPedAlignmentInputTypeEnumT *pe_SlimTypeOut
);

/**
@brief Converts SAMLite ped alignment indication to SLIM structure.

Function converts SAMLite ped alignment indication to SLIM structure.

@param[in]  pz_SamlData    Pointer to the SAMLite data.
@param[out] pz_SlimDataOut Pointer to the SLIM data.
*/
void slim_SAMLiteConvertPedAlignmentIndication
(
  const saml_PedestrianAlignmentDataStructT *pz_SamlData,
  slimPedestrianAlignmentDataStructT *pz_SlimDataOut
);

/**
@brief Converts SAMLite service status indication to SLIM available service
mask.

Function converts SAMLite service status indication to SLIM available service
mask.

@param[in]  pz_SamlData       Pointer to the SAMLite data.
@param[out] pq_ServiceMaskOut Pointer to the SLIM service mask.
*/
void slim_SAMLiteConvertServiceStatusIndication
(
  const saml_ServiceStatusEventStructT *pz_SamlData,
  slimAvailableServiceMaskT *pq_ServiceMaskOut
);

/**
@brief Converts SAMLite ped alignment input type mask to SLIM mask.

Function converts SAMLite ped alignment input type mask to SLIM mask.

@param[in]  q_SamlMask     SAMLite mask
@param[out] pq_SlimMaskOut Pointer to the SLIM mask.
*/
void slim_SAMLiteConvertPedAlignmentInputTypeMask
(
  saml_PedAlignmentInputTypeMaskT q_SamlMask,
  slimPedAlignmentInputTypeMaskT *pq_SlimMaskOut
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_SAMLITE_CONVERSION_H */
