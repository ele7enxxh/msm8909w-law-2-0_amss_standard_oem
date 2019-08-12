#ifndef __MQCSI_CONN_MGR_TYPES_H__
#define __MQCSI_CONN_MGR_TYPES_H__
/*!
  @file
  mqcsi_conn_mgr_types.h

  @brief
  APIs to manage for CXM's connection/transport interfaces

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mqcsi/inc/mqcsi_conn_mgr_types.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
07/03/14   rj      CFCM's BUS BW changes
10/01/12   ckk     Support delayed QMI response for metrics
06/06/12   gs      Added sap_handle to mqcsi_service_context_s
04/12/12   ckk     Rename & organize code 
04/08/12   sr      Initial revision

==========================================================================*/

/*==========================================================================

                             INCLUDE FILES

============================================================================*/
#include "qmi_sap.h"

#include <qmi_csi.h>

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
/* Maximum number of clients currently supported by all services under CXM */
#define MQCSI_MAX_CLIENTS_ALLOWED   4

/* Maximum size of buffer to hold the name of CXM's service */
#define MQCSI_SERVICE_NAME_MAX_SIZE 20

/* Maximum number of outstanding delayed/deferred QMI request for the service */
#define MQCSI_MAX_DEFERRED_REQS_ALLOWED   10

/*=============================================================================

                   INTERNAL DEFINITIONS AND TYPES

=============================================================================*/
/* Unique IDs for all services supported by CXM's QCSI layer
     this gets stored in the service_cookie */
typedef enum 
{
  /* ID for CXM's COEX service */
  MQCSI_COEX_SERVICE_ID = 0,

  /* ID for CXM's STX service */
  MQCSI_STX_SERVICE_ID = 1,

  /* ID for CXM's DSI service */
  MQCSI_DSI_SERVICE_ID = 2,

  /* ID for CFCM's BUS BW service */
  MQCSI_CFCM_SERVICE_ID = 3,

  /* Current maximum allowed/supported services by CXM */
  MQCSI_MAX_SERVICE_ID,

}mqcsi_service_ids_e;

/*=============================================================================

                             TYPEDEFS

=============================================================================*/
/* Indication Map Type */
typedef uint32 indication_map_type;

/* QMI CSI Callback function type used to notify the service provider
   when a REQ is received from a client.*/
typedef qmi_csi_cb_error (*mqcsi_conn_mgr_request_cb_t) (
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
);

/*=============================================================================

  FUNCTION:  mqcsi_conn_mgr_get_ind_offset_type

=============================================================================*/
/*!
    @brief
    This cb maintains the internal mapping between the indication method
    id and offset of the message in the message map.

    @return
    int32 offset
*/
/*===========================================================================*/
typedef int32 (*mqcsi_conn_mgr_get_ind_offset_type) (
  const uint32            ind_msg_id  /*!< Indication whose internal offset is needed */
);

/* CxM Connection Manager's Service's Client structure */
typedef struct {
  /* Stores the Client's QMI handle */
  qmi_client_handle   handle;

  /* Whether Service's Client is active/connected or not */
  boolean             connected;

  /* This variable holds the data about the indications. 
  Each bit is enabled or disabled depending on whether the 
  corresponding mapped indication should be enabled or not */
  indication_map_type ind_map;

} mqcsi_conn_mgr_client_s;

/* CxM Connection Manager's Service Cookie structure */
typedef struct {
  /* One of the unique service ID defined in mqcsi_qcsi_service_ids_e */
  mqcsi_service_ids_e id;

  /* Name of the service */
  char              name[MQCSI_SERVICE_NAME_MAX_SIZE];

} mqcsi_conn_mgr_service_cookie_s;

/* CxM Connection Manager's outstanding deferred/delayed REQ structure */
typedef struct {
  /* Validity check */
  boolean        is_valid;

  /* Connection Details [Client Info] */
  void          *connection_handle;

  /* Request Handle */
  qmi_req_handle req_handle;

  /* Message Id */
  unsigned int   msg_id;

} mqcsi_conn_mgr_deferred_req_s;

/* CxM Connection Manager's Service's context info structure */
typedef struct {
  /* Stores the Service's QMI CSI handle */
  qmi_csi_service_handle        csi_handle;

  /* Stores the Service's QMI SAP handle */
  qmi_sap_client_handle         sap_handle;

  /* Stores the QMI IDL object */
  qmi_idl_service_object_type   idl_object;

  /* Stores the Service ID */
  uint32                        service_id;

  /* Service Cookie shared with the QCSI layer to pass around */
  mqcsi_conn_mgr_service_cookie_s cookie;

  /* Pointer to the handle REQ message callback for this Service */
  mqcsi_conn_mgr_request_cb_t     req_cb;

  /* Pointer to the handle IND offset callback for this Service */
  mqcsi_conn_mgr_get_ind_offset_type ind_offset_cb;

  /* List of Clients being served by the Service */
  mqcsi_conn_mgr_client_s         client[MQCSI_MAX_CLIENTS_ALLOWED];

  /* OS parameters for this Service */
  qmi_csi_os_params             os_params;

  /* Has the service registered ? */
  boolean                       registered;

  /* Maintain list of uotsanding deferred/delayed REQs */
  mqcsi_conn_mgr_deferred_req_s   deferred_reqs[MQCSI_MAX_DEFERRED_REQS_ALLOWED];

  /* Policy options for this service */
  qmi_csi_options               options;

} mqcsi_service_context_s;

#endif /* __MQCSI_CONN_MGR_TYPES_H__ */
