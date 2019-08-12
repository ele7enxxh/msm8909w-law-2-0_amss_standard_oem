#ifndef __MQCSI_CONN_MGR_H__
#define __MQCSI_CONN_MGR_H__
/*!
  @file
  mqcsi_conn_mgr.h

  @brief
  APIs to manage for MQCSI's connection/transport interfaces

*/

/*===========================================================================

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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mqcsi/inc/mqcsi_conn_mgr.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
10/01/12   ckk     Support delayed QMI response for metrics
07/05/12   ejv     Add support for mqcsi layer.
04/17/12   ckk     Move to new coex IDL
04/12/12   ckk     Rename & organize code 
04/08/12   sr      Initial revision

==========================================================================*/

/*==========================================================================

                             INCLUDE FILES

============================================================================*/
#include <IxErrno.h>

#include "mqcsi_conn_mgr_types.h"

#include <qmi_csi.h>

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_service_open

=============================================================================*/
/*!
    @brief
    Open/Register/Initialize a service over the underlying transport  mechanism
    Called by corresponding server provider when it gets started. It
    registers the connect, disconnect and request callbacks with the qmi.

    @return
    qcsi service handle
*/
/*===========================================================================*/
qmi_csi_service_handle* mqcsi_conn_mgr_service_open (
  mqcsi_service_ids_e           id,     /*!< Unique service provider ID */
  char                       *name,   /*!< Service Name */
  rex_tcb_type               *tcb,    /*!< REX task TCB pointer for which to set the sig */
  rex_sigs_type               sig,    /*!< REX sig to set for this service */
  qmi_idl_service_object_type object, /*!< QCSI (IDL) Service object */
  mqcsi_conn_mgr_request_cb_t   req_cb,  /*!< Callback to handle REQ messages */
  mqcsi_conn_mgr_get_ind_offset_type ind_offset_cb, /*!< Callback to get ind offset */
  qmi_csi_options            *service_options, /*!< To set service options */
  int32                       major_rev, /*!< Major Rev */
  int32                       minor_rev  /*!< Minor Rev */
);

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_service_close

=============================================================================*/
/*!
    @brief
    Close/Deregister/Deinitialize the Service over the underlying transport mechanism
    Called by the Co-existence manager task when TASK_STOP signal is received.
    This method unregisters the service from the QMI Interface.

    @return
    qmi_csi_error
*/
/*===========================================================================*/
qmi_csi_error mqcsi_conn_mgr_service_close (
  mqcsi_service_ids_e service_id /*!< Unique service provider ID */
);

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_handle_event

=============================================================================*/
/*!
    @brief
    Wrapper for qmi_csi_handle_event

    @return
    qmi_csi_error
*/
/*===========================================================================*/
qmi_csi_error mqcsi_conn_mgr_handle_event(
  mqcsi_service_ids_e service_id /*!< Unique service provider ID */
); 

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_send_resp

=============================================================================*/
/*!
    @brief
    Wrapper for qmi_csi_send_resp

    @return
    qmi_csi_error
*/
/*===========================================================================*/
qmi_csi_error mqcsi_conn_mgr_send_resp (
  void          *connection_handle, /*!< Connection Details [Client Info] */
  qmi_req_handle req_handle,        /*!< Request Handle */
  unsigned int   msg_id,            /*!< Message Id */
  void          *resp_msg,          /*!< RESP message Data */
  unsigned int   resp_msg_len       /*!< RESP message size */
);

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_send_resp_from_cb

=============================================================================*/
/*!
    @brief
    Wrapper for qmi_csi_send_resp, but additionally maps qmi_csi_error
    to qmi_csi_cb_error type

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error mqcsi_conn_mgr_send_resp_from_cb (
  void          *connection_handle, /*!< Connection Details [Client Info] */
  qmi_req_handle req_handle,        /*!< Request Handle */
  unsigned int   msg_id,            /*!< Message Id */
  void          *resp_msg,          /*!< RESP message Data */
  unsigned int   resp_msg_len       /*!< RESP message size */
);

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_update_client_ind_map

=============================================================================*/
/*!
    @brief
    Helper funciton to update a service's client's IND enable/disable map
 
    @details
    This method is used to set or reset the indication data in the internal
    indication map. Indications are the data that are sent from the server
    to the clients. Clients can enable or disable their subcription to an
    indication at any point. These enable and disable indications are sent
    through packets. Hence, once the Service reads the packet, if its an
    enable/disable indication, it sends the data to the interface.

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type mqcsi_conn_mgr_update_client_ind_map (
  mqcsi_service_ids_e  service_id,        /*!< Unique service provider ID */
  void              *connection_handle, /*!< Connection Details [Client Info] */
  const unsigned int msg_id,            /*!< Indication message ID */
  const boolean      to_enable          /*!< True/False to Enable/Disable */
);

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_send_ind

=============================================================================*/
/*!
    @brief
    To send IND type messages to all clients of a service
 
    @details
    This method is used to send the indication to the registered clients.
    This method goes through all the clients and checks if they are active
    and have enabled the given indication. if true, it sends the indication
    to that client.
    
    @return
    qmi_csi_error
*/
/*===========================================================================*/
qmi_csi_error mqcsi_conn_mgr_send_ind(
  mqcsi_service_ids_e service_id, /*!< Unique service provider ID */
  unsigned int      msg_id,     /*!< Message Id */
  void             *ind_msg,    /*!< Indication Message to be sent */
  uint32            ind_msg_len /*!< Size of the indication message */
);

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_push_deferred_req

=============================================================================*/
/*!
    @brief
    Helper API to push delayed/deferred QMI request onto the service's stack

    @return
    qmi_csi_error
*/
/*===========================================================================*/
qmi_csi_error mqcsi_conn_mgr_push_deferred_req (
  mqcsi_service_ids_e service_id,        /*!< Unique service provider ID */
  void             *connection_handle, /*!< Connection Details [Client Info] */
  qmi_req_handle    req_handle,        /*!< Request Handle */
  unsigned int      msg_id,            /*!< Message Id */
  uint32           *deferred_req_id    /*!< Deferred Request ID*/
);

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_pop_deferred_req

=============================================================================*/
/*!
    @brief
    Helper API to pop delayed/deferred QMI request onto the service's stack

    @return
    qmi_csi_error
*/
/*===========================================================================*/
qmi_csi_error mqcsi_conn_mgr_pop_deferred_req (
  mqcsi_service_ids_e service_id,        /*!< Unique service provider ID */
  uint32            deferred_req_id,   /*!< Deferred Request ID*/
  unsigned int      msg_id,            /*!< Message Id */
  void            **connection_handle, /*!< Connection Details [Client Info] */
  qmi_req_handle   *req_handle         /*!< Request Handle */
);

#endif /* __MQCSI_CONN_MGR_H__ */
