#ifndef SLIM_QMISLIM_PROVIDER_MSG_TYPES_H
#define SLIM_QMISLIM_PROVIDER_MSG_TYPES_H
/**
  @file
  @brief QMI-SLIM Provider IPC Message Types
  @ingroup slim_QSProvider
*/
/*
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
 */

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/qmislim/src/qmislim_provider_msg_types_i.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qmislim_common_api.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_QSProvider
//! @{
//! @name QMI-SLIM Provider Internal IPC types
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

//! @brief Raw enumeration of QMI-SLIM Provider messages.
enum
{
  //! @cond
  eQS_PROVIDER_MSG_RECONNECT_V0,
  eQS_PROVIDER_MSG_DISCONNECT_V0,
  eQS_PROVIDER_MSG_DATA_INDICATION_V0,   /* obsolete */
  eQS_PROVIDER_MSG_SERVICE_RESPONSE_V0,  /* obsolete */
  eQS_PROVIDER_MSG_TIME_RESPONSE_V0,     /* obsolete */
  eQS_PROVIDER_MSG_SERVICE_CHANGE_V0,    /* obsolete */
  eQS_PROVIDER_MSG_REGISTER_RESPONSE_V0, /* obsolete */
  eQS_PROVIDER_MSG_DISCONNECTED_V0,
  eQS_PROVIDER_MSG_DATA_INDICATION_V1,
  eQS_PROVIDER_MSG_SERVICE_RESPONSE_V1,
  eQS_PROVIDER_MSG_TIME_RESPONSE_V1,
  eQS_PROVIDER_MSG_SERVICE_CHANGE_V1,
  eQS_PROVIDER_MSG_REGISTER_RESPONSE_V1,
  //! @endcond
};

/**
@brief Internal provider messages

Internal messages are used for forwarding incoming QCCI events from QCCI
task/thread context into SLIM task/thread context.
*/
enum
{
  eQS_PROVIDER_MSG_RECONNECT = eQS_PROVIDER_MSG_RECONNECT_V0,
  /**< Asynchronous reconnect message */
  eQS_PROVIDER_MSG_DISCONNECT = eQS_PROVIDER_MSG_DISCONNECT_V0,
  /**< Asynchronous disconnect message */
  eQS_PROVIDER_MSG_DATA_INDICATION = eQS_PROVIDER_MSG_DATA_INDICATION_V1,
  /**< Asynchronous service data indication message */
  eQS_PROVIDER_MSG_SERVICE_RESPONSE = eQS_PROVIDER_MSG_SERVICE_RESPONSE_V1,
  /**< Asynchronous service request response message */
  eQS_PROVIDER_MSG_TIME_RESPONSE = eQS_PROVIDER_MSG_TIME_RESPONSE_V1,
  /**< Asynchronous time sync response message */
  eQS_PROVIDER_MSG_SERVICE_CHANGE = eQS_PROVIDER_MSG_SERVICE_CHANGE_V1,
  /**< Asynchronous service change message */
  eQS_PROVIDER_MSG_REGISTER_RESPONSE = eQS_PROVIDER_MSG_REGISTER_RESPONSE_V1,
  /**< Asynchronous client registration message */
  eQS_PROVIDER_MSG_DISCONNECTED = eQS_PROVIDER_MSG_DISCONNECTED_V0,
  /**< Asynchronous disconnect operation competition */
};
/**
@brief Internal message for service data routing.

Message is used for forwarding service data messages from QCCI into SLIM
thread/task context.

@sa eQS_PROVIDER_MSG_DATA_INDICATION
*/
typedef struct
{
  slimMessageHeaderStructT z_Header;
  /**< Generic header */
  slim_QSSlimMessageDataT z_Data;
  /**< Service data */
} slim_QSProvider_ServiceDataMsgT;

/**
@brief Internal message for service response routing.

Message is used for forwarding service response messages from QCCI into SLIM
thread/task context.

@sa eQS_PROVIDER_MSG_SERVICE_RESPONSE
*/
typedef struct
{
  slimMessageHeaderStructT z_Header;
  /**< Generic header */
  int32  l_TxId;
  /**< Transaction number */
  slimClientHandleT p_ClientHandle;
  /**< Client handler for response dispatch */
  boolean u_Enable;
  /**< Operation type: TRUE - enable service; FALSE - disable */
  boolean u_RateRequest;
  /**< When TRUE the request is rate request */
  uint16 w_ReportRateHz;
  /**< For rate requests: reporting rate in Hz preceding the request */
  uint16 w_SampleCount;
  /**< For rate requests: sample count in batch preceding the request */
} slim_QSProvider_ServiceResponseMsgT;

/**
@brief Internal message for time response routing.

Message is used for forwarding time response messages from QCCI into SLIM
thread/task context.

@sa eQS_PROVIDER_MSG_TIME_RESPONSE
*/
typedef struct
{
  int32 l_TxId;
  /**< Transaction number */
  slimErrorEnumT e_Error;
  /**< Operation status */
  uint64 t_RemoteRxTimeMs;
  /**< Remote message time in milliseconds */
  uint64 t_RemoteTxTimeMs;
  /**< Remote message time in milliseconds */
} slim_QSProvider_TimeResponseMsgT;

/**
@brief Internal message for client registration response routing.

Message is used for forwarding time response messages from QCCI into SLIM
thread/task context.

@sa eQS_PROVIDER_MSG_REGISTER_RESPONSE
*/
typedef struct
{
  slimErrorEnumT e_Error;
  /**< Error code. */
} slim_QSProvider_RegisterResponseMsgT;

/**
@brief Internal message for service change routing.

Message is used for forwarding time response messages from QCCI into SLIM
thread/task context.

@sa eQS_PROVIDER_MSG_SERVICE_CHANGE
*/
typedef struct
{
  slimServiceEnumT e_Service;
  /**< Service */
  slimServiceProviderMaskT q_ServiceProviderMask;
  /**< Service mask change */
} slim_QSProvider_ServiceChangeMsgT;

//! @}
//! @}

#ifdef __cplusplus
}
#endif

#endif /* SLIM_QMISLIM_PROVIDER_MSG_TYPES_H */
