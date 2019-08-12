#ifndef QMI_SAP_XPORT_H
#define QMI_SAP_XPORT_H
/******************************************************************************
  @file    qmi_sap_xport.h
  @brief   The QMI Service Access Proxy (SAP) Xport Header file.

  DESCRIPTION
  QMI SAP xport routines for different transport implementations
  for initializing and handling requests, responses, and indications.
  
  ---------------------------------------------------------------------------
  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include "qmi_sap.h"
#include "qmi_sap_target_ext.h"

/*=============================================================================
  CALLBACK FUNCTION qmi_sap_open_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QSAP infrastructure to open a new
  transport 

@param[in]   xport_data          Opaque parameter to the xport (e.g. port ID)
@param[in]   xport               Pointer to infrastructure's transport struct.
                                 Can be treated as opaque, but prototyped for
                                 ease of debugging.
@param[in]   max_rx_len          Maximum length of messages that can be
                                 received. Used by xport to allocate a buffer
                                 if the underlying transport cannot pass the
                                 message through a callback.  

@retval      Opaque handle to the transport. NULL on failure.

*/
/*=========================================================================*/
typedef void *(*qmi_sap_open_fn_type)
(
 void *xport_data,
 void *xport,
 uint32_t max_rx_len
);

/*=============================================================================
  CALLBACK FUNCTION qmi_sap_reg_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QSAP infrastructure to register a
  new server 

@param[in]   handle              Opaque handle returned by the open call
@param[in]   service_id          Service ID of the server
@param[in]   version             Version of the service
                                 received. Used by xport to allocate a buffer
                                 if the underlyin transport cannot pass the
                                 message through a callback.  

@retval      QMI_SAP_NO_ERR      Success

*/
/*=========================================================================*/
typedef qmi_sap_error (*qmi_sap_reg_fn_type)
(
 void *handle,
 uint32_t service_id,
 uint32_t version
);

/*=============================================================================
  CALLBACK FUNCTION qmi_sap_send_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QSAP infrastructure to send data to
  a client.

@param[in]   handle              Opaque handle returned by the open call
@param[in]   addr                Opaque address sent to the infrastructure
                                 through the connect or recv calls.
@param[in]   msg                 Pointer to message to be sent
@param[in]   msg_len             Length of the message
@param[in]   client_data         Pointer to client-specific storage, if defined

@retval      QMI_SAP_NO_ERR      Success

*/
/*=========================================================================*/
typedef qmi_sap_error (*qmi_sap_send_fn_type)
(
 void *handle,
 void *addr,
 uint8_t *msg,
 uint32_t msg_len,
 void **client_data
);

/*=============================================================================
  CALLBACK FUNCTION qmi_sap_close_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QSAP infrastructure to close the
  transport usually when the service is unregistered. It is crucial that the
  xport synchronize the deallocation of memory and its callback functions
  before calling qmi_sap_xport_closed() to free up the rest of the data
  associated with the service.

@param[in]   handle              Opaque handle returned by the open call

*/
/*=========================================================================*/
typedef void (*qmi_sap_close_fn_type)
(
 void *handle
);

/*=============================================================================
  CALLBACK FUNCTION qmi_sap_addr_len_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QSAP infrastructure to retrieve the
  length of the (source) address of the xport.

@retval         Length of address

*/
/*=========================================================================*/
typedef uint32_t (*qmi_sap_addr_len_fn_type)
(
 void
);

/* xport ops table type */
typedef struct qmi_sap_xport_ops_s
{
  qmi_sap_open_fn_type open;
  qmi_sap_reg_fn_type  reg;
  qmi_sap_reg_fn_type  unreg;
  qmi_sap_send_fn_type send;
  qmi_sap_close_fn_type close;
  qmi_sap_addr_len_fn_type addr_len;
} qmi_sap_xport_ops_type;

/*=============================================================================
                        Xport to infrastructure up-calls
=============================================================================*/
/*=============================================================================
  FUNCTION qmi_sap_xport_start
=============================================================================*/
/*!
@brief
  This function is used to register a transport with the infrastructure

@param[in]   ops                Pointer to transport operations table
@param[in]   xport_data         Opaque data associated with the transport,
                                such as port ID or other parameters.
*/
/*=========================================================================*/
void qmi_sap_xport_start
(
 qmi_sap_xport_ops_type *ops,
 void *xport_data
 );

/*=============================================================================
  FUNCTION qmi_sap_xport_connect
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  a new client has connected. In a connectionless environment, this step is
  unnecessary.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      QMI_SAP_NO_ERR     Success
*/
/*=========================================================================*/
qmi_sap_error qmi_sap_xport_connect
(
 void *xport,
 void *addr
 );

/*=============================================================================
  FUNCTION qmi_sap_xport_recv
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure to
  process the incoming message, one at a time.

  In most cases, it is triggered by a callback to the handle_event function.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.
@param[in]   tx_id              Transaction ID
@param[in]   msg_id             Message ID
@param[in]   buf                Pointer to the message buffer 
@param[in]   len                Length of the message

@retval      QMI_SAP_NO_ERR     Success
*/
/*=========================================================================*/
qmi_sap_error qmi_sap_xport_recv
(
 void *xport,
 void *addr,
 uint8_t *buf,
 uint32_t len
 );

/*=============================================================================
  FUNCTION qmi_sap_xport_disconnect
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  a client has disconnected. The data associated with the client will be freed

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      QMI_SAP_NO_ERR     Success
*/
/*=========================================================================*/
qmi_sap_error qmi_sap_xport_disconnect
(
 void *xport,
 void *addr
 );

/*=============================================================================
  FUNCTION qmi_sap_xport_closed
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  the transport has been fully closed, no more callbacks can occur and the
  xport-specific data has been deallocated. The infrastructure will then free
  data associated with the clients and service that unregistered.
  
@param[in]   xport              Pointer to infrastructure's xport struct
*/
/*=========================================================================*/
void qmi_sap_xport_closed
(
 void *xport
 );

#endif /* QMI_SAP_XPORT_H */
