#ifndef MQCSI_LOG_H
#define MQCSI_LOG_H
/*============================================================================
@file mqcsi_log.h

@brief
  This module is a wrapper to the QCSI module. This module provides a common
  logging framework for all QCSI services. It acts as a Layer between 
  QCSI-Framework and the Service. The service if interested in logging of 
  the QMI-log packets transmitted and received can use the functionality of 
  this module. 

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/mqcsi_log.h#1 $ 
  $DateTime: 2016/12/13 08:00:19 $ 
  $Author: mplcsds1 $ 

when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
2012-09-28  ag   Added register_with_options api 
2011-06-23  spn  Initial revision.

=============================================================================*/

/*****************************************************************************
 * Include Files
 * *************************************************************************/

#include <customer.h>
#include <comdef.h>

#include <qmi_csi_target_ext.h>
#include <qmi_csi.h>


/*****************************************************************************
 * Preprocessor Definitions and Constants
 * *************************************************************************/


/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  This callback function is called by the MCS-QCSI infrastructure when  it 
  receives a request from each client(user of the service).

@param[in]   client_handle_p    Handle used by the MCS-QCSI infrastructure to 
                                identify different clients. 
@param[in]   service_cookie_p   Service specific data. Service cookie is 
                                registered with the MCS-QCSI infrastructure  
                                during service registration(mqcsi_register).                     
@param[out]  service_conn_handle_pp  Services return this handle as a token to 
                                     represent this client connection
                                     to the service.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB.....       Look into the enumeration qmi_csi_error for
                                 the error values.
*//* ------------------------------------------------------------------------*/
typedef qmi_csi_cb_error (*mqcsi_connect_cb)
(
  void                      *client_handle_p,
  void                      *service_cookie_p,
  void                     **service_conn_handle_pp
);

/* -----------------------------------------------------------------------*//**
@brief
  This callback function is called by the QCSI infrastructure when each
  client(user of service) deregisters with the  QCSI infrastructure.

@param[in]  service_conn_handle_p  Service handle as given by the service in
                                   mqcsi_connect_cb for the client 
                                   disconnecting.
@param[in]   service_cookie_p      Service specific data. Service cookie is 
                                   registered with the MCS-QCSI infrastructure  
                                   during service registration(mqcsi_register).                     
*//* ------------------------------------------------------------------------*/
typedef void (*mqcsi_disconnect_cb)
(
  void                      *service_conn_handle_p,
  void                      *service_cookie_p
);

/* -----------------------------------------------------------------------*//**
@brief
  This callback is invoked when the MCS-QCSI infrastructure receives an 
  incoming message. The MCS-QCSI infrastructure relays the data and gives it 
  to the services. 

@param[in]  service_conn_handle_p  Service handle as given by the service in
                                   mqcsi_connect_cb.
@param[in]  req_handle             Handle provided by the QCSI infrastructure
                                   to specify this particular transaction and
                                   message.
@param[in]  msg_id                 Message Id pertaining to this particular 
                                   message. 
@param[in]  decoded_struct_p       C struct with the decoded data.
@param[in]  decoded_struct_len     Length of the c struct.                                   
@param[in]  service_cookie_p       Service specific data.Service cookie is 
                                   registered with the infrastructure during 
                                   service registration(mqcsi_register).                     


@retval    QMI_CSI_CB_NO_ERR       Success
@retval    QMI_CSI_CB.....         Look into the enumeration qmi_csi_error for
                                   the error values.
*//* ------------------------------------------------------------------------*/
typedef qmi_csi_cb_error (*mqcsi_process_req_cb)
(
  void                      *service_conn_handle_p,
  qmi_req_handle             req_handle,
  uint32                     msg_id,
  void                      *decoded_struct_p,
  uint32                     decoded_struct_len,
  void                      *service_cookie_p
);


/*****************************************************************************
 * Function Declarations and Documentation
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to perform one time initializations. All local data 
  structures used by the module will be initialized in this layer. 

@retval  TRUE  - Successful initialization of the module
@retval  FALSE - Module initialization was not successful
*//* ------------------------------------------------------------------------*/
boolean mqcsi_powerup_init
( 
  void 
);

/* -----------------------------------------------------------------------*//**
@brief
  A service that is interested in using the mcsQCSI Wrapper, can register 
  itself using this function.

@param[in]  service_major_rev      Major Revision of the Service
@param[in]  service_minor_rev      Minor Revision of the Service
@param[in]  service_obj_p          Object Containing meta-information to 
                                   encode/decode the structures. 
@param[in]  service_conn_cb        Callback to notify service of a new client
                                   connect
@param[in]  service_disconn_cb     Callback to notify service of a client
                                   disconnect
@param[in]  service_proc_req_cb    Callback to notify service of a message 
                                   sent by the client to be processed. 
@param[in]  service_cookie_p       Service specific user-data provided by the 
                                   Service.
@param[in]  os_params_p            OS specific Parameters for signal handling.
@param[out] pservice_handle_p      Handle that the MCS-QCSI infrastructure 
                                   provides to the service to represent the  
                                   service.

@retval    QMI_CSI_CB_NO_ERR       Success
@retval    QMI_CSI_CB.....         Look into the enumeration qmi_csi_error for
                                   the error values.
*//* ------------------------------------------------------------------------*/
qmi_csi_error mqcsi_register 
(
  uint32                                    service_id,
  uint32                                    service_major_rev,
  uint32                                    service_minor_rev,
  qmi_idl_service_object_type               service_obj_p,
  mqcsi_connect_cb                          service_conn_cb,
  mqcsi_disconnect_cb                       service_disconn_cb,
  mqcsi_process_req_cb                       service_proc_req_cb,
  void                                     *service_cookie_p,
  qmi_csi_os_params                        *os_params_p,
  void                                    **service_handle_pp
);

/* -----------------------------------------------------------------------*//**
@brief
  A service that is interested in using the mcsQCSI Wrapper, can register 
  itself using this function.

@param[in]  service_major_rev      Major Revision of the Service
@param[in]  service_minor_rev      Minor Revision of the Service
@param[in]  p_ServiceObj           Object Containing meta-information to 
                                   encode/decode the structures. 
@param[in]  service_conn_cb        Callback to notify service of a new client
                                   connect
@param[in]  service_disconn_cb     Callback to notify service of a client
                                   disconnect
@param[in]  service_proc_req_cb    Callback to notify service of a message 
                                   sent by the client to be processed. 
@param[in]  service_cookie_p       Service specific user-data provided by the 
                                   Service.
@param[in]  service_options        Set any options to the server like do not
                                   expose to certain clients.
@param[in]  os_params_p            OS specific Parameters for signal handling.
@param[out] service_handle_pp      Handle that the MCS-QCSI infrastructure 
                                   provides to the service to represent the  
                                   service.

@retval    QMI_CSI_NO_ERR        Success
@retval    QMI_CSI_.....         Look into the enumeration qmi_csi_error for
                                   the error values.
*//* ------------------------------------------------------------------------*/
qmi_csi_error mqcsi_register_with_options 
(
  uint32                                    service_id,
  uint32                                    service_major_rev,
  uint32                                    service_minor_rev,
  qmi_idl_service_object_type               z_ServiceObj,
  mqcsi_connect_cb                          service_conn_cb,
  mqcsi_disconnect_cb                       service_disconn_cb,
  mqcsi_process_req_cb                      service_proc_req_cb,
  void                                     *service_cookie_p,
  qmi_csi_options                          *service_options,
  qmi_csi_os_params                        *os_params_p,
  void                                    **service_handle_pp
);

/* -----------------------------------------------------------------------*//**
@brief
  Function for a service to unregister from the MCS-QCSI Infrastructure
  
@param[in] service_handle_p      Handle that the MCS-QCSI infrastructure 
                                 provides to the service to represent the  
                                 service.


@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB.....       Look into the enumeration qmi_csi_error for
                                 the error values.
*//* ------------------------------------------------------------------------*/
qmi_csi_error mqcsi_unregister 
( 
  void                                    *service_handle_p
);


/* -----------------------------------------------------------------------*//**
@brief
  Function for the service to send an indication to the client. 

@param[in]   client_handle_p     Handle used by the MCS-QCSI infrastructure to 
                                 identify different clients. 
@param[in]  msg_id               Message Id pertaining to this particular 
                                 message. 
@param[in]  decoded_struct_p     C struct with the decoded data.
@param[in]  decoded_struct_len   Length of the c struct.                                   

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB.....       Look into the enumeration qmi_csi_error for
                                 the error values.
*//* ------------------------------------------------------------------------*/
qmi_csi_error mqcsi_send_ind
(
  void                                   *client_handle_p,
  uint32                                  msg_id,
  void                                   *decoded_struct_p,
  uint32                                  decoded_struct_len
);

/* -----------------------------------------------------------------------*//**
@brief
  Function for the service to send a response to the client. 

@param[in]  client_handle_p      Handle used by the MCS-QCSI infrastructure to 
                                 identify different clients. 
@param[in]  req_handle           Handle provided by the QCSI infrastructure
                                 to specify this particular transaction and
                                 message, given to the service in 
                                 mqcsi_process_req_cb.
@param[in]  msg_id               Message Id pertaining to this particular 
                                 message. 
@param[in]  decoded_struct_p     C struct with the decoded data.
@param[in]  decoded_struct_len   Length of the c struct.                                   

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB.....       Look into the enumeration qmi_csi_error for
                                 the error values.
*//* ------------------------------------------------------------------------*/
qmi_csi_error mqcsi_send_rsp
(
  void                                   *client_handle_p,
  qmi_req_handle                          req_handle,
  uint32                                  msg_id,
  void                                   *decoded_struct_p,
  uint32                                  decoded_struct_len
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the module that intends to broadcast an 
  indication to multiple clients of the QMI-LOC, Service. 

@param[in] service_handle_p      Handle that the MCS-QCSI infrastructure 
                                 provides to the service to represent the  
                                 service.
@param[in]  os_params_p          OS specific Parameters for signal handling.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB.....       Look into the enumeration qmi_csi_error for
                                 the error values.
*//* ------------------------------------------------------------------------*/
qmi_csi_error mqcsi_handle_event
(
  void                                    *service_handle_p,
  qmi_csi_os_params                       *os_params_p
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the module that intends to broadcast an 
  indication to multiple clients of the QMI-LOC, Service. 

@param[in] service_handle_p      options that will be passed to 
                                 qmi_csi_register_with_options()

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB.....       Look into the enumeration qmi_csi_error for
                                 the error values.
*//* ------------------------------------------------------------------------*/
qmi_csi_error mqcsi_register_log_options
(
   qmi_csi_options                        *options
);

#endif /* #ifndef MQCSI_LOG_H */

