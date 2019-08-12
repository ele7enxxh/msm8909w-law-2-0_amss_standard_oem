#ifndef __SAML_API_H__
#define __SAML_API_H__

/*============================================================================
  @file saml_api.h

                       SAMLite API

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

 ============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/samlite/inc/saml_api.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/14   th      Initial
05/13/14   mk      Added SAMLite APIs

=============================================================================*/


/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "comdef.h"   /* Definition for basic types and macros */
#include "customer.h"
#include "aries_os_api.h"

#include "saml_client_types.h"
#include "gnss_common.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * External Function Declarations -- SAMLite Client APIs
 * -------------------------------------------------------------------------*/

/**
@brief Registers SAMLite client (i.e., SLIM) with the SAMLite service.

Before SLIM can use SAMLite service, this function should be always called.
It should be called with a predefined client handle that is needed in all
communications with the SAMLite service. Client should also provide a
pointer to a callback function that is called whenever SAMLite data is
available.

When client connection is opened, SAMLite notifies which SAMLite services are available
using client callback function (@see eSAML_MESSAGE_ID_SERVICE_STATUS_IND).
Client can use @see saml_ServiceAvailable function to check the availability
of a specific service when it has received the service status indication.

@param  p_Handle: Pointer to a handle used to identify this client
@param  fn_Callback : A pointer to the client’s callback function to process
received data.
@return eSAML_SUCCESS if handle is opened successfully.
Otherwise SAMLite error code which means that handle is invalid and SAMLite
services cannot be used with this handle.
*/
saml_ErrorEnumT saml_Open
(
  const saml_ClientHandleT p_Handle,
  saml_NotifyCallbackFunctionT fn_Callback
);

/**
@brief This function de-registers a client, and releases the client handle.
The client handle shall not be used after calling saml_Close().

Before calling saml_Close() a client should disable sensor services it is using.
Registered callback functions will not be called after this function has returned.
However, the callbacks may be called while the client is in the process of closing.
Thus, clients should take care to insure their registered callback function will
execute properly until after this function has returned.

@param  p_Handle: The opaque handle used to identify this client.
@return eSAML_SUCCESS if request was sent successfully. Otherwise SAMLite error code.
*/
saml_ErrorEnumT saml_Close
(
  const saml_ClientHandleT p_Handle
);

/**
@brief Enabling or disabling of motion data streaming.

Motion data streaming can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data
of the response. If service is started successfully, indications are
reported periodically via registered client callback function.

Final result to motion data streaming request is provided to client
via registered callback function. @see saml_NotifyCallbackFunctionT
 - Service: eSAML_SERVICE_MOTION_DATA
 - Type:    eSAML_MESSAGE_TYPE_RESPONSE
 - Error:   eSAML_SUCCESS if request was successfull, otherwise SAMLite error code.
 - ID/Payload: eSAML_MESSAGE_ID_MOTION_DATA_ENABLE_RESP/no payload

Motion data indications are provided to client via registered callback function.
@see saml_NotifyCallbackFunctionT
 - Service: eSAML_SERVICE_MOTION_DATA
 - Type:    eSAML_MESSAGE_TYPE_INDICATION
 - Error:   SAMLite error code if error was detected in indication.
            eSAML_SUCCESS otherwise.
 - ID/Payload:
    - eSAML_MESSAGE_ID_MOTION_DATA_IND/saml_MotionDataStructT

@param  p_Handle: The opaque handle used to identify this client.
@param  pz_Request : Data for motion data configuration.
@param  l_TxnId : A transaction ID for the service request. This id is
provided back to client when a response to this request is sent.
@return eSAML_SUCCESS if request was sent successfully. Otherwise SAMLite error code.
*/
saml_ErrorEnumT saml_EnableMotionData
(
  const saml_ClientHandleT p_Handle,
  const saml_EnableMotionDataRequestStructT *pz_Request,
  int32 l_TxnId
);

/**
@brief Enabling or disabling of pedometer reporting.

Pedometer reporting can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data
of the response. If service is started successfully, indications are
reported periodically via registered client callback function.

Final result to pedometer reporting request is provided to client
via registered callback function. @see saml_NotifyCallbackFunctionT
 - Service: eSAML_SERVICE_PEDOMETER
 - Type:    eSAML_MESSAGE_TYPE_RESPONSE
 - Error:   eSAML_SUCCESS if request was successfull, otherwise SAMLite error code.
 - ID/Payload: eSAML_MESSAGE_ID_PEDOMETER_ENABLE_RESP/no payload

Pedometer indications are provided to client via registered callback function.
@see saml_NotifyCallbackFunctionT
 - Service: eSAML_SERVICE_PEDOMETER
 - Type:    eSAML_MESSAGE_TYPE_INDICATION
 - Error:   SAMLite error code if error was detected in indication.
            eSAML_SUCCESS otherwise.
 - ID/Payload:
    - eSAML_MESSAGE_ID_PEDOMETER_IND/saml_PedometerDataStructT

@param  p_Handle: The opaque handle used to identify this client.
@param  pz_Request : Data for pedometer configuration.
@param  l_TxnId : A transaction ID for the service request. This id is
provided back to client when a response to this request is sent.
@return eSAML_SUCCESS if request was sent successfully. Otherwise SAMLite error code.
*/
saml_ErrorEnumT saml_EnablePedometer
(
  const saml_ClientHandleT p_Handle,
  const saml_EnablePedometerRequestStructT *pz_Request,
  int32 l_TxnId
);

/**
@brief Enabling or disabling of pedestrian alignment data streaming.

Pedestrian alignment data streaming can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data of
the response. If service is started successfully, indications are reported
via registered client callback function.

Final result to sensor data streaming request is provided to client
via registered callback function. @see saml_NotifyCallbackFunctionT
 - Service: eSAML_SERVICE_PED_ALIGNMENT
 - Type:    eSAML_MESSAGE_TYPE_RESPONSE
 - Error:   eSAML_SUCCESS if request was successfull, otherwise SAMLite error code.
 - ID/Payload:
   -eSAML_MESSAGE_ID_PED_ALIGNMENT_ENABLE_RESP/saml_PedestrianAlignmentEnableResponseStructT
     if the request was successful.

Pedestrian alignment data indications are provided to client via registered
callback function.
@see saml_NotifyCallbackFunctionT
 - Service: eSAML_SERVICE_PED_ALIGNMENT
 - Type:    eSAML_MESSAGE_TYPE_INDICATION
 - Error:   eSAML_SUCCESS if request was successfull, otherwise SAMLite error code.
 - ID/Payload:
    - eSAML_MESSAGE_ID_PED_ALIGNMENT_IND/saml_PedestrianAlignmentDataStructT

@param  p_Handle: The opaque handle used to identify this client.
@param  pz_Request : Data for the pedestrian alignment streaming configuration.
@param  l_TxnId : A transaction ID for the sensor service request. This id
is provided to client when a response to this request is sent.
@return eSAML_SUCCESS if request was sent successfully. Otherwise SAMLite error code.
*/
saml_ErrorEnumT saml_EnablePedestrianAlignment
(
  const saml_ClientHandleT p_Handle,
  const saml_EnablePedestrianAlignmentRequestStructT *pz_Request,
  int32 l_TxnId
);

/*----------------------------------------------------------------------------
 * External Function Declarations -- SAMLite Task APIs for Owner Task (SDP)
 * -------------------------------------------------------------------------*/

/**
@brief SAMLite module initialization.

This function initializes SAMLite module. It should be called when SAMLite owning
task is initialized. No SAMLite services are available before this function
is called.
*/
void saml_TaskInit
(
  void
);

/**
@brief SAMLite module deinitializion.

This function stops SAMLite module. It should be called when SAMLite receives
the TASK_STOP IPC message.  All SAMLite services are in a non-deterministic
state after this function is called.
*/
void saml_TaskStop
(
  void
);

/**
@brief Handler for SAMLite IPC messages.

This function should be called when SAMLite owning task receives
SAMLite IPC message.

@param  pz_IpcMsgIn: Received IPC message.
*/
void saml_TaskIpcHandler
(
  const os_IpcMsgType *const pz_IpcMsgIn
);

/**
@brief This function handles timer-expiry IPCs for SDP.

@param q_TimerId: SDP timer Id
@param q_TimerParam: parameter attached with the timer
@return 0 if there is no error
*/
uint32 saml_TaskTimerExpiryHandler
(
  uint32 q_TimerId,
  uint32 q_TimerParam
);

/**
@brief  This function sends the PDR control (either Build-Config/
         QMI-LOC/PDR/NV) to SAMLite module.

@param b_PdrDisableFlag: PDR control config, TRUE if feature is 
                         disabled, FALSE otherwise
@param e_PdrCtrlSrc: Source of the config, build-config, QMI or NV
@return TRUE if send was success, FALSE otherwise
*/
boolean saml_InjectPdrControl
(
  boolean b_PdrDisableFlag,
  gnss_PdrCtrlSrcEnumType e_PdrCtrlSrc
);

#endif /* __SAML_API_H__ */
