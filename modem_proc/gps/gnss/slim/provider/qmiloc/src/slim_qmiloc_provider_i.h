#ifndef SLIM_QMILOC_PROVIDER_I_H
#define SLIM_QMILOC_PROVIDER_I_H
/**
  @file
  @brief Internal interface for QMI-LOC Provider
  @ingroup slim_QMILOCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/qmiloc/src/slim_qmiloc_provider_i.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_provider_data.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_QMILOCProvider
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
@brief Function for dispatching data received from QMI_LOC-pipe.

This function should be called when data received from QMI_LOC is ready to
be dispatched to SLIM clients..

@param[in] e_Service   SLIM service.
@param[in] e_MessageId SLIM message id.
@param[in] w_DataSize  Size of the payload.
@param[in] p_Data      Pointer to the payload.
*/
void slim_QmiLocDispatchData
(
  slimServiceEnumT e_Service,
  slimMessageIdEnumT e_MessageId,
  uint16 w_DataSize,
  const void *p_Data
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_QMILOC_PROVIDER_I_H */
