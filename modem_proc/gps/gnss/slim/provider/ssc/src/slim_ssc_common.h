#ifndef SLIM_SSC_COMMON_H
#define SLIM_SSC_COMMON_H
/**
  @file
  @brief Util functions needed in SSC connection handling.

  This module provides functions that are needed in for establishing
  QMI-connection to the SSC.

  @ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_common.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_ssc_msg_types.h"
#include "slim_ssc_provider.h"
#include "slim_ssc_provider_data.h"
#include "slim_utils.h"

//! @addtogroup slim_SSCProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/** Enum for SSC service error codes */
typedef enum
{
  SSC_SERVICE_SUCCESS                    = 0,  /**< Success */
  SSC_SERVICE_NULL_PTR_FAILURE           = -1, /**< NULL pointer failure */
  SSC_SERVICE_QMI_SERVICE_OBJECT_FAILURE = -2, /**< QMI service object was not found */
  SSC_SERVICE_QMI_SERVICE_INSTANCE_FAILURE = -3, /**< Unable to get the QMI client service instance */
  SSC_SERVICE_QMI_CLIENT_INIT_FAILURE    = -4, /**< QMI client initialization failed */
  SSC_SERVICE_ALLOCATION_FAILURE         = -5, /**< Failure in QMI request/response allocation */
  SSC_SERVICE_QMI_SEND_FAILURE           = -6, /**< QMI message sending failed */
  SSC_SERVICE_BAD_ARGUMENT_FAILURE       = -7, /**< Invalid request data */
  SSC_SERVICE_QMI_RELEASE_FAILURE        = -8, /**< Release of QMI connection failed */
  SSC_SERVICE_QMI_DECODE_FAILURE         = -9, /**< QMI indication decoding failed */
  SSC_SERVICE_QMI_CONNECTION_NOT_OPEN    = -10, /**< Operation attempted for a closed connection */
  //! @cond
  SSC_SERVICE_ERROR_MIN = -2147483647 /* To force a 32 bit signed enum.
                                         Do not change or use */
  //! @endcond
} slim_SscServiceErrorEnumType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Provides the SNS service type for the given SLIM service.

Function provides the SNS service type for the given SLIM service.

@param[in] e_Service SLIM service
@return QMI_SNS service type for the given service.
*/
slim_SscSnsServiceEnumType slim_SscGetSnsServiceForSlimService
(
  slimServiceEnumT e_Service
);

/**
@brief Provides the QMI service object for the given service.

Function provides the QMI service object for the given service.

@param[in] e_SnsService QMI_SNS service type
@return QMI_SNS service object for the given service. NULL, if not found.
*/
qmi_idl_service_object_type slim_SscGetServiceObject
(
  slim_SscSnsServiceEnumType e_SnsService
);

/**
@brief Checks if the SSC service is available

Function checks if the given sensor service is available with SSC.
If this function fails, SLIM should not use SSC as the sensor data provider for
the given service.

@param[in] e_SnsService SSC QMI_SNS Sensor service type
@return SSC_SERVICE_SUCCESS if the SSC server interface is available for the
                            given sensor service. Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscCheckServiceStatus
(
  slim_SscSnsServiceEnumType e_SnsService
);

/**
@brief Initializes SSC connection for the given service object.

Function establishes a QMI-client connection to SSC and stores the connection
data to slim_SscConnectionDataStructType given as parameter. Given pointer to 
indication callback function is registered to the QMI-interface. Function is 
called whenever the QMI-interface sends a service indication. Given client
handle is used as the callback function data.

@param[in]  p_Handle              SLIM client handle.
@param[out] pz_SscConnection      Pointer to the data struct for storing the SSC
                                  connection details.
@param[in]  p_ServiceObject       SSC service object.
@param[in]  q_InstanceId          SSC service instance id.
@param[in]  fn_IndicationCallback Pointer to the indication callback function.

@retval SSC_SERVICE_SUCCESS if the connection is established successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscGetService
(
  slimClientHandleT p_Handle,
  slim_SscConnectionDataStructType *pz_SscConnection,
  const qmi_idl_service_object_type p_ServiceObject,
  qmi_service_instance q_InstanceId,
  qmi_client_ind_cb fn_IndicationCallback
);

/**
@brief Releases SSC connection

Function releases SSC QMI-client connection. Given client handle is used as
the release callback function data.

@param[in] p_Handle           SLIM client handle.
@param[in] pz_SscData         Pointer to the SSC connection data structure.
@param[in] fn_ReleaseCallback Pointer to the release callback function.

@retval SSC_SERVICE_SUCCESS if the release request is sent successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscReleaseService
(
  slimClientHandleT p_Handle,
  slim_SscConnectionDataStructType *pz_SscData,
  qmi_client_release_cb fn_ReleaseCallback
);

/**
@brief Allocates QMI requests/responses.

Function allocates memory for a QMI request/response. Allocated memory needs
to be freed with a call to os_MemFree.

@param[out] pp_Data Address of the pointer to the data struct.
@param[in]  q_Size  Size of the data struct.

@retval SSC_SERVICE_SUCCESS if memory is allocated successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscAllocQmiStruct
(
  void **pp_Data,
  uint32 q_Size
);

/**
@brief Frees QMI requests/responses.

Function frees memory for a QMI request/response and resets the data pointer.

@param[in,out] pp_Data Address of the pointer to the data struct.
*/
void slim_SscFreeQmiStruct
(
  void **pp_Data
);

/**
@brief Sends QMI-request to SSC QCSI.

Function sends the SNS-request to the SSC. The memory allocated for the request
is freed in both failed and successful calls to this function. The given
callback function is called whenever the response for the request is available.
The response should be freed in the callback function.

@param[in]     pz_SscConnection    Pointer to the SSC connection data.
@param[in]     q_Id                Message id to be sent.
@param[in]     fn_ResponseCallback Pointer to the response callback function.
@param[in,out] pp_Request          Address of the pointer to the request data.
@param[in]     q_RequestSize       Request data size.
@param[in]     q_ResponseSize      Response size.
@param[in]     p_CbData            Data for the callback function.

@retval SSC_SERVICE_SUCCESS if the request is sent successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscMakeRequest
(
  const slim_SscConnectionDataStructType *pz_SscConnection,
  uint32 q_Id,
  qmi_client_recv_msg_async_cb fn_ResponseCallback,
  void **pp_Request,
  uint32 q_RequestSize,
  uint32 q_ResponseSize,
  void *p_CbData
);

/**
@brief Makes request for enabling QMD reporting

This function can be used to enable QMD reporting for a specific QMD-algorithm
service (AMD/RMD).

@param[in] pz_SscConnection    Pointer to the SSC connection data.
@param[in] pz_Txn              Request configuration
@param[in] q_Id                Message id to be sent.
@param[in] fn_ResponseCallback Pointer to the response callback function.
@param[in] p_CbData            Data for the callback function.

@retval SSC_SERVICE_SUCCESS if the request is sent successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscEnableQmdReporting
(
  const slim_SscConnectionDataStructType *pz_SscConnection,
  const slim_EnableQmdDataTxnStructType *pz_Txn,
  uint32 q_Id,
  qmi_client_recv_msg_async_cb fn_ResponseCallback,
  void *p_CbData
);

/**
@brief Sends QMD enable response data to SLIM owning task IPC queue.

Function sends QMD enable response data to SLIM owning task IPC queue.

@param[in] e_QmdConnection QMD connection type.
@param[in] p_Buf           Pointer to message.
@param[in] q_Len           Message length.
@param[in] p_RespCbData    Callback data (transaction id).
@param[in] e_TranspErr     QMI error.
*/
void slim_SscQmdEnableResponseProxy
(
  slim_SscQmdConnectionType e_QmdConnection,
  void *p_Buf,
  unsigned int q_Len,
  void *p_RespCbData,
  qmi_client_error_type e_TranspErr
);

/**
@brief Handles response data for enabling QMD reporting request

This function can be used to handle QMI response data for enabling QMD (AMD/RMD)
algorithm service. Algorithm instance id is stored to provider state.

@param[out] pz_QmdConnection Pointer to the QMD connection data
@param[in]  pz_RespMsg       Pointer to the response message data
*/
void slim_SscHandleQmdEnableResponse
(
  slim_SscSamConnectionDataStructType *pz_QmdConnection,
  const slim_SscEnableQmdRespMsgT *pz_RespMsg
);

/**
@brief Makes request for disabling QMD reporting

This function can be used to disable QMD reporting for a specific QMD-algorithm
service (AMD/RMD).

@param[in] pz_SscConnection    Pointer to the SSC connection data.
@param[in] u_InstanceId        Algorithm instance id.
@param[in] q_Id                Message id to be sent.
@param[in] fn_ResponseCallback Pointer to the response callback function.
@param[in] p_CbData           Data for the callback function.

@retval SSC_SERVICE_SUCCESS if the request is sent successfully.
                            Otherwise SSC error code.
*/
slim_SscServiceErrorEnumType slim_SscDisableQmdReporting
(
  const slim_SscConnectionDataStructType *pz_SscConnection,
  uint8 u_InstanceId,
  uint32 q_Id,
  qmi_client_recv_msg_async_cb fn_ResponseCallback,
  void *p_CbData
);

/**
@brief Sends QMD disable response data to SLIM owning task IPC queue.

Function sends QMD disable response data to SLIM owning task IPC queue.

@param[in] e_QmdConnection QMD connection type.
@param[in] p_Buf           Pointer to message.
@param[in] q_Len           Message length.
@param[in] p_RespCbData    Callback data (transaction id).
@param[in] e_TranspErr     QMI error.
*/
void slim_SscQmdDisableResponseProxy
(
  slim_SscQmdConnectionType e_QmdConnection,
  void *p_Buf,
  unsigned int q_Len,
  void *p_RespCbData,
  qmi_client_error_type e_TranspErr
);

/**
@brief Handles response data for disabling QMD reporting request

This function can be used to handle QMI response for disabling QMD (AMD/RMD)
algorithm service.

@param[out] pz_QmdConnection Pointer to the QMD connection data
@param[in]  pz_RespMsg       Pointer to the response message data
*/
void slim_SscHandleQmdDisableResponse
(
  slim_SscSamConnectionDataStructType *pz_QmdConnection,
  const slim_SscDisableQmdRespMsgT *pz_RespMsg
);

/**
@brief Sends QMD indication data to SLIM owning task IPC queue.

Function sends QMD indication data to SLIM owning task IPC queue.

@param[in] e_QmdConnection QMD connection type.
@param[in] e_Error         SLIM error.
@param[in] p_UserHandle    QMI user handle.
@param[in] q_MsgId         Received message id.
@param[in] p_Buf           Pointer to message.
@param[in] q_Len           Message length.
@param[in] p_IndCbData     Callback data (client handle).
*/
void slim_SscQmdIndicationProxy
(
  slim_SscQmdConnectionType e_QmdConnection,
  slimErrorEnumT e_Error,
  qmi_client_type p_UserHandle,
  unsigned int q_MsgId,
  void *p_Buf,
  unsigned int q_Len,
  void *p_IndCbData
);

/**
@brief Converts and sends QMD indication data.

Function converts and sends QMD indication data.

@param[in] pz_Msg QMD indication data.
*/
void slim_SscHandleQmdIndication
(
  const slim_SscQmdIndMsgT *pz_Msg
);

/**
@brief Converts SSC QMD indication data to SLIM data.

Function converts QMD indication data to SLIM data.

@param[in]  pz_SscInd   Pointer to SSC QMD indication data.
@param[out] pz_SlimData Pointer to SLIM QMD indication data.
*/
void slim_SscConvertQmdIndication
(
  const sns_sam_qmd_report_ind_msg_v01 *pz_SscInd,
  slimQmdDataStructT *pz_SlimData
);

/**
@brief Dispatch function for request responses.

SSC modules should use this function to dispatch request responses to SLIM
core.

@param[in] l_ServiceTxnId SLIM service transaction id.
@param[in] e_Error        Response error code.
@param[in] w_DataSize     Size of the message.
@param[in] p_Data         Pointer to the message data.
*/
void slim_SscDispatchResponse
(
  int32 l_ServiceTxnId,
  slimErrorEnumT e_Error,
  uint16 w_DataSize,
  const void *p_Data
);

/**
@brief Decodes QMI-indication.

Function decodes received QMI-indication using the client QMI connection data.

@param[in]  p_UserHandle     QMI client handle
@param[in]  q_MsgId          Indication message id.
@param[in]  p_Source         Pointer to the received data.
@param[in]  q_SourceLen      Size of the received data.
@param[out] p_Destination    Pointer to the target data structure.
@param[in]  q_DestinationLen Size of the target data structure.

@retval eSLIM_SUCCESS if the data is decoded successfully.
                      Otherwise SLIM error code.
*/
slimErrorEnumT slim_SscDecodeIndication
(
  qmi_client_type p_UserHandle,
  uint32 q_MsgId,
  const void *p_Source,
  uint32 q_SourceLen,
  void *p_Destination,
  uint32 q_DestinationLen
);

//! @}

#endif /* #ifndef SLIM_SSC_COMMON_H */
