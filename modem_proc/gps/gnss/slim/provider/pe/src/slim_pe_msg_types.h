#ifndef SLIM_PE_MSG_TYPES_H
#define SLIM_PE_MSG_TYPES_H
/**
@file

  Header containing the message types that PE provider can route through
  SLIM owning task.

@ingroup slim_PEProvider
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/pe/src/slim_pe_msg_types.h#1 $ */

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "mgp_provider_api.h"

//! @addtogroup slim_PEProvider
//! @{

/*----------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/
/*! IPC message types for PE provider */
typedef enum
{
  eSLIM_PE_MSG_ID_IND_SERVICE_STATUS = 0,
  eSLIM_PE_MSG_ID_RESPONSE_MAGNETIC_FIELD_DATA,
  eSLIM_PE_MSG_ID_IND_MAGNETIC_FIELD_DATA,

  //! @cond
  eSLIM_PE_MSG_ID_MAX                  = 2147483647 /* Force 32bit */
  //! @endcond
} slim_PEMsgIdEnumType;

/*! Struct for PE response IPC header */
typedef struct
{
  mgp_ErrorEnumT e_MsgError;
  /**< SAMLite error code */

  int32 l_TxnId;
  /**< Client transaction ID set in request */
} slim_PERespHeaderStructT;

/*! Struct for PE indication IPC header */
typedef struct
{
  mgp_ErrorEnumT e_MsgError;
  /**< SAMLite error code */
} slim_PEIndHeaderStructT;

/*! Struct for PE response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_PERespHeaderStructT z_RespHeader;
  /**< Response header */

  /* No payload */
} slim_PERespMsgT;

/*! Struct for magnetic field data indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slim_PEIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_MagneticFieldData_valid;
  /**< TRUE if PE magnetic field data payload is valid */

  mgp_EarthMagFieldDataStructT z_MagneticFieldData;
  /**< PE magnetic field data payload */

} slim_PEInjectMagneticFieldDataIndMsgT;

/*! Struct for PE service status event indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slim_PEIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_ServiceStatusEvent_valid;
  /**< TRUE if PE service status event payload is valid */

  mgp_ServiceStatusEventStructT z_ServiceStatusEvent;
  /**< PE service status event payload */

} slim_PEInjectServiceStatusEventIndMsgT;

/*! Union for PE provider messages */
typedef union
{
  slim_PERespMsgT z_GenericRespMsg;
  /**< PE generic response message */

  slim_PEInjectMagneticFieldDataIndMsgT z_MagneticFieldDataMsg;
  /**< PE magnetic field data message */

  slim_PEInjectServiceStatusEventIndMsgT z_ServiceStatusEventMsg;
  /**< PE service status event message */

} slim_PEMsgUnionT;

//! @}

#endif /* SLIM_PE_MSG_TYPES_H */
