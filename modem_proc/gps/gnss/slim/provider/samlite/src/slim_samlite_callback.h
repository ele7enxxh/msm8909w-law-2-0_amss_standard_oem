#ifndef SLIM_SAMLITE_CALLBACK_H
#define SLIM_SAMLITE_CALLBACK_H
/**
  @file
 
  Implementation of callback function of the SAMLite module. Callback pushes
  the received SAMLite messages to the IPC queue of the SLIM owning task.

  @ingroup slim_SAMLProvider
*/
 
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/samlite/src/slim_samlite_callback.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "saml_client_types.h"

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
@brief SAMLite callback function.

SAMLite callback function implementation.

@param[in] pz_MessageHeader Pointer to the message header.
@param[in] p_Message        Pointer to the message data.

@sa saml_NotifyCallbackFunctionT

@ingroup slim_SAMLProvider
*/
void slim_SAMLiteNotifyCallbackFunction
(
  const saml_MessageHeaderStructT *pz_MessageHeader,
  void *p_Message
);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_SAMLITE_CALLBACK_H */
