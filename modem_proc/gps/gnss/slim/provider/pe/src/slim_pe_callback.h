#ifndef SLIM_PE_CALLBACK_H
#define SLIM_PE_CALLBACK_H
/*!
  @file
  @brief Implementation of callback function of the PE module
 
  Implementation of callback function of the PE module. Callback pushes
  the received PE messages to the IPC queue of the SLIM owning task.
  @ingroup slim_PEProvider
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/pe/src/slim_pe_callback.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "mgp_provider_api.h"

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
@brief PE callback function.

PE callback function implementation.

@param[in] pz_MessageHeader Pointer to the message header.
@param[in] p_Message        Pointer to the message data.
*/
void slim_PENotifyCallbackFunction
(
  const mgp_MessageHeaderStructT* pz_MessageHeader,
  void *p_Message
);

//! @}

#endif /* #ifndef SLIM_PE_CALLBACK_H */
