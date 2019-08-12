/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            Q M I    U I M    H T T P


GENERAL DESCRIPTION

  This source file contains the supporting functions to access the QMI UIM
  HTTP service

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_uim_http.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/16   ar      Fix to support bigger profile (> 65535) download
05/27/16   ks      MACRO for memory freeing
05/17/16   bcho    Deregister qmi uim http service on task stop
05/04/16   ks      APPS to register with QMI HTTP service using reset request
04/14/16   ks      KW fixes
04/13/16   ks      Initial implementation of QMI UIM HTTP service

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "rex.h"
#include "modem_mem.h"
#include "queue.h"
#include "uim_msg.h"

#include "qmi_csi.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "ipc_policy.h"

#include <stringl/stringl.h>

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#include "qmi_uim_http_util.h"
#include "lpalib.h"
#include "qmi_uim_http.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* Max requests that can be accomodated by queue */
#define QMI_UIM_HTTP_CMD_QUEUE_MAX_SIZE  10

#define QMI_UIM_HTTP_CREATE_TOKEN_ID(service_id, transaction_id)  ((service_id) | (transaction_id << 4))

#define QMI_UIM_HTTP_GET_SERVICE_ID_FROM_TOKEN_ID(token_id)       (token_id & 0x0F)
#define QMI_UIM_HTTP_GET_TRANSACTION_ID_FROM_TOKEN_ID(token_id)   (token_id >> 4)

/* Global to store all the QMI UIM HTTP service related variables */
static qmi_uim_http_globals_type * qmi_uim_http_global_ptr = NULL;

/*===========================================================================

                         REQUEST HANDLER FUNCTIONS

===========================================================================*/
typedef qmi_csi_error (* const qmi_uim_http_req_handler_type) (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len);

static qmi_csi_error qmi_uim_http_reset_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
);

static qmi_csi_error qmi_uim_http_transaction_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
);

/* Table to handle the dispatch of request handler functions. */
static qmi_uim_http_req_handler_type qmi_uim_http_req_handler_table[] =
{
  qmi_uim_http_reset_req_handler,            /* Request handler for message ID 0x20 */
  qmi_uim_http_transaction_req_handler       /* Request handler for message ID 0x21 */
};

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_GLOBALS_INIT

   DESCRIPTION:
     This function initializes the globals associated with QMI UIM HTTP service

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_http_globals_init (
  void
)
{
  if (qmi_uim_http_global_ptr != NULL)
  {
    return;
  }

  qmi_uim_http_global_ptr =
    modem_mem_calloc(1, sizeof(qmi_uim_http_globals_type), MODEM_MEM_CLIENT_UIM);

  if (qmi_uim_http_global_ptr == NULL)
  {
    return;
  }

  (void) q_init(&qmi_uim_http_global_ptr->cmd_q);
  rex_init_crit_sect(&qmi_uim_http_global_ptr->crit_sect);
} /* qmi_uim_http_globals_init */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_RESP_CB_AND_MEM_FREE

   DESCRIPTION:
     This function calls the rsp call back if any and frees the response memory if allocated already

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/

static void qmi_uim_http_resp_cb_and_mem_free(
  void
 )
{
  uint8                                     service_index  = 0;
  uint8                                     index          = 0;
  qmi_uim_http_transaction_resp_data_type  *rsp_ptr        = NULL;
  qmi_uim_http_client_info_type            *client_ptr     = NULL;  

  if (qmi_uim_http_global_ptr == NULL)
  {
    return;
  }

  client_ptr =  &(qmi_uim_http_global_ptr->client_registry);

  for (service_index = 0;
       service_index < QMI_UIM_HTTP_MAX_NUM_SERVICE;
       service_index++)
  {

    rsp_ptr = &(client_ptr->curr_transaction[service_index].rsp_data);

    if (client_ptr->curr_transaction[service_index].rsp_cb_ptr)
    {
      rsp_ptr->result  = QMI_UIM_HTTP_UNKNOWN_ERROR;
      (client_ptr->curr_transaction[service_index].rsp_cb_ptr)(rsp_ptr);
    }

    for(index = 0;
        index < rsp_ptr->num_cust_headers && index < UIM_HTTP_CUST_HEADER_MAX_COUNT_V01;
        index ++)	
    {
      QMI_UIM_HTTP_FREE(rsp_ptr->headers[index].name_ptr);
      QMI_UIM_HTTP_FREE(rsp_ptr->headers[index].value_ptr);
    }

    QMI_UIM_HTTP_FREE(rsp_ptr->payload_ptr);
    memset(rsp_ptr, 0x0, sizeof(qmi_uim_http_transaction_resp_data_type));

    client_ptr->curr_transaction[service_index].rsp_cb_ptr     = NULL;
    client_ptr->curr_transaction[service_index].received_bytes  = 0;
    client_ptr->curr_transaction[service_index].service_in_use = FALSE;
  }
} /* qmi_uim_http_resp_cb_and_mem_free */


/*===========================================================================
  FUNCTION QMI_UIM_HTTP_MAP_CSI_TO_CSI_CB_ERR

  DESCRIPTION
    Function converts a CSI error to a CSI callback error type

  PARAMETERS
   qmi_csi_error

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_uim_http_map_csi_to_csi_cb_err (
  qmi_csi_error  csi_error
)
{
  switch (csi_error)
  {
    case QMI_CSI_NO_ERR:
      return QMI_CSI_CB_NO_ERR;

    case QMI_CSI_CONN_REFUSED:
      return QMI_CSI_CB_CONN_REFUSED;

    case QMI_CSI_INTERNAL_ERR:
      return QMI_CSI_CB_INTERNAL_ERR;

    case QMI_CSI_NO_MEM:
      return QMI_CSI_CB_NO_MEM;

    case QMI_CSI_CONN_BUSY:
    case QMI_CSI_INVALID_HANDLE:
    case QMI_CSI_INVALID_ARGS:
    case QMI_CSI_ENCODE_ERR:
    case QMI_CSI_DECODE_ERR:
    default:
      break;
  }
  return QMI_CSI_CB_UNSUPPORTED_ERR;
} /* qmi_uim_http_map_csi_to_csi_cb_err */


/*===========================================================================
  FUNCTION QMI_UIM_HTTP_CONNECT_CB

  DESCRIPTION
    Connect callback function registered with the QCSI.

  PARAMETERS
    cli_handle
    service_cookie
    connection_handle

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_uim_http_connect_cb (
  qmi_client_handle   client_handle,
  void               *service_cookie,
  void              **connection_handle
)
{
  /* there is no need to store service specific context information */
  (void) service_cookie;

  UIM_MSG_HIGH_0("qmi_uim_http_connect_cb");

  if(connection_handle == NULL)
  {
    UIM_MSG_ERR_0("Invalid connection handle");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  if (qmi_uim_http_global_ptr == NULL)
  {
    UIM_MSG_LOW_0("Connect received when qmi uim http not initialized");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  if (qmi_uim_http_global_ptr->client_registry.in_use == FALSE)
  {
    qmi_uim_http_global_ptr->client_registry.in_use         = TRUE;
    qmi_uim_http_global_ptr->client_registry.client_handle  = client_handle;
    *connection_handle = &(qmi_uim_http_global_ptr->client_registry);
    return QMI_CSI_CB_NO_ERR;
  }

  UIM_MSG_LOW_0("qmi_uim_http_connect_cb: No entry available for new client");
  return QMI_CSI_CB_INTERNAL_ERR;
} /* qmi_uim_http_connect_cb */
 

/*===========================================================================
  FUNCTION QMI_UIM_HTTP_DISCONNECT_CB

  DESCRIPTION
    Callback function registered with the QCSI to be invoked when a client
    requests to remove connection with service.

  PARAMETERS
    connection_handle
    service_cookie

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_http_disconnect_cb (
  void    * conn_handle,
  void    * service_cookie
)
{
  qmi_uim_http_client_info_type     * client_handle_ptr = NULL;

  (void)service_cookie;

  if (conn_handle == NULL)
  {
    return;
  }

  client_handle_ptr = (qmi_uim_http_client_info_type *)conn_handle;

  UIM_MSG_HIGH_0("qmi_uim_http_disconnect_cb");

  if(client_handle_ptr == &(qmi_uim_http_global_ptr->client_registry))
  {
    qmi_uim_http_resp_cb_and_mem_free();
    /* Clear the client entry */
    memset(&(qmi_uim_http_global_ptr->client_registry),
           0x0,
           sizeof(qmi_uim_http_client_info_type));
  }
} /* qmi_uim_http_disconnect_cb */


/*===========================================================================
  FUNCTION QMI_UIM_HTTP_HANDLE_REQ_CB

  DESCRIPTION
    Request callback function registered with the QCSI to be invoked when
    client sends request message to service.

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    msg_id : message id
    req_c_struct
    req_c_struct_len
    service_cookie

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_uim_http_handle_req_cb (
  void              * connection_handle,
  qmi_req_handle      req_handle,
  unsigned int        msg_id,
  void              * req_struct,
  unsigned int        req_struct_len,
  void              * service_cookie
)
{
  qmi_csi_error   status    = QMI_CSI_INVALID_ARGS;
  uint8           msg_index = 0;

  (void) service_cookie;

  if(connection_handle == NULL)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  UIM_MSG_HIGH_1("qmi_uim_http_handle_req_cb with msg_id 0x%x", msg_id);

  /* Verify that the msg_id received is within supported range and find
     appropriate request handler to process the request */
  if(msg_id >= QMI_UIM_HTTP_RESET_REQ_V01)
  {
    msg_index = (uint8) msg_id - QMI_UIM_HTTP_RESET_REQ_V01;
    if (msg_index < (sizeof(qmi_uim_http_req_handler_table) / sizeof(qmi_uim_http_req_handler_type)))
    {
      if(qmi_uim_http_req_handler_table[msg_index])
      {
        status = qmi_uim_http_req_handler_table[msg_index](connection_handle,
                                                          req_handle,
                                                          req_struct,
                                                          req_struct_len);
      }
    }
  }
  return qmi_uim_http_map_csi_to_csi_cb_err(status);
} /* qmi_uim_http_handle_req_cb */


/*===========================================================================
  FUNCTION QMI_UIM_HTTP_SERVICE_INIT

  DESCRIPTION
    Called to start the QMI UIM HTTP service when the HTTP modem task is up

  PARAMETERS
    TCB and SIG

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_http_service_init (
  rex_tcb_type    * tcb_ptr,
  rex_sigs_type     sig
)
{
  qmi_csi_error                 err          =  QMI_CSI_NO_ERR;
  qmi_sap_error                 sap_err      =  QMI_SAP_NO_ERR;
  qmi_idl_service_object_type   service_obj  =  uim_http_get_service_object_v01();
  qmi_csi_os_params             os_params;
  qmi_csi_options               options;

  UIM_MSG_HIGH_0("qmi_uim_http_service_init");
  
  qmi_uim_http_globals_init();

  if (service_obj == NULL || qmi_uim_http_global_ptr == NULL)
  {
    UIM_MSG_ERR_1("Cannot initialize QMI UIM HTTP service obj 0x%x",
                  service_obj);
    return;
  }
  
  memset(&os_params, 0, sizeof(qmi_csi_os_params));
  os_params.tcb = tcb_ptr;
  os_params.sig = sig;

  QMI_CSI_OPTIONS_INIT(options);
  QMI_CSI_OPTIONS_SET_SCOPE(options, IPC_POLICY_SCOPE_OPEN);

  err = qmi_csi_register_with_options (service_obj,
                                       qmi_uim_http_connect_cb,
                                       qmi_uim_http_disconnect_cb,
                                       qmi_uim_http_handle_req_cb,
                                       NULL,
                                       &os_params,
                                       &options,
                                       &(qmi_uim_http_global_ptr->service_handle));

  if (err != QMI_CSI_NO_ERR || qmi_uim_http_global_ptr->service_handle == NULL)
  {
    UIM_MSG_ERR_1("QMI CSI register failed, error=0x%x", err);
    return;
  }

  /* SAP registration necessary for native QMI clients to access the service
     and to test using QMI Test Pro */
  sap_err = qmi_sap_register(service_obj, NULL, &(qmi_uim_http_global_ptr->sap_handle));
  if (sap_err != QMI_SAP_NO_ERR)
  {
    UIM_MSG_ERR_1("QMI SAP register failed, error=0x%x", sap_err);
  }
} /* qmi_uim_http_service_init */


/*===========================================================================
  FUNCTION QMI_UIM_HTTP_HANDLE_QCSI_SIG

  DESCRIPTION
    Function is called when a QCSI signal is received by the QMI UIM HTTP
    service. It will result in the calling of one of the QMI UIM HTTP
    request handlers.

  PARAMETERS
    tcb
    sig

  RETURN VALUE
    int

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_http_handle_qcsi_sig (
   rex_tcb_type *tcb, rex_sigs_type sig
)
{
  qmi_csi_error err;
  qmi_csi_os_params os_params;

  if (qmi_uim_http_global_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cannot handle qcsi sig as qmi uim http is not init");
    return;
  }
  
  memset(&os_params, 0, sizeof(qmi_csi_os_params));
  os_params.tcb = tcb;
  os_params.sig = sig;

  err = qmi_csi_handle_event(qmi_uim_http_global_ptr->service_handle,
                             &os_params);

  if (err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("qmi_uim_http_handle_qcsi_sig: failed with error=0x%x", err);
  }
} /* qmi_uim_http_handle_qcsi_sig */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_DECODE_RESULT

   DESCRIPTION:
     This function decodes the http response status enumeration to the QMI UIM HTTP
     result enumeration

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_uim_http_transaction_result_type qmi_uim_http_decode_result (
  uim_http_result_type_enum_v01  result
)
{
  switch (result)
  {
    case UIM_HTTP_TRANSACTION_SUCCESSFUL_V01:
      return QMI_UIM_HTTP_TRANSACTION_SUCCESS;

    case UIM_HTTP_SERVER_ERROR_V01:
      return QMI_UIM_HTTP_SERVER_ERROR;

    case UIM_HTTP_TLS_ERROR_V01:
      return QMI_UIM_HTTP_TLS_ERROR;

    case UIM_HTTP_NETWORK_ERROR_V01:
      return QMI_UIM_HTTP_NETWORK_ERROR;

    case UIM_HTTP_UNKNOWN_ERROR_V01:
    default:
      return QMI_UIM_HTTP_UNKNOWN_ERROR;
  }
} /* qmi_uim_http_decode_result */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_RESET_REQ_HANDLER

   DESCRIPTION:
     This function processes the reset request from the client

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_http_reset_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
)
{
  uim_http_reset_resp_msg_v01       resp;
  qmi_uim_http_client_info_type   * client_info_ptr = NULL;
  qmi_client_handle                 client_handle;

  if (conn_handle == NULL)
  {
    return QMI_CSI_INTERNAL_ERR;
  }

  /* Store the client handle */
  client_handle   = qmi_uim_http_global_ptr->client_registry.client_handle;
  /* Client info needs to be cleared */
  client_info_ptr = (qmi_uim_http_client_info_type *)conn_handle;

  UIM_MSG_HIGH_0("qmi_uim_http_reset_req_handler");

  if(client_info_ptr == &(qmi_uim_http_global_ptr->client_registry))
  {
    qmi_uim_http_resp_cb_and_mem_free();
    /* Clear the client information */
    memset(&(qmi_uim_http_global_ptr->client_registry),
           0x0,
           sizeof(qmi_uim_http_client_info_type));
  }

  /* Restore the client handle. MODEM can send the indications using this handle */
  qmi_uim_http_global_ptr->client_registry.in_use 		= TRUE;
  qmi_uim_http_global_ptr->client_registry.client_handle	= client_handle;

   /* Send response to the client */
  resp.resp.error  = QMI_ERR_NONE_V01;
  resp.resp.result = QMI_RESULT_SUCCESS_V01;

  return qmi_csi_send_resp(req_handle,
                           QMI_UIM_HTTP_RESET_RESP_V01,
                           &resp,
                           sizeof(resp));
} /* qmi_uim_http_reset_req_handler */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_TRANSACTION_REQ_HANDLER

   DESCRIPTION:
     This function processes the HTTP response sent by the server
     
   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_http_transaction_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
)
{
  uim_http_transaction_req_msg_v01         *req_msg_ptr    = NULL;
  uint8                                     index          = 0;
  uint8                                     service_index  = 0;
  uint8                                     transaction_id  = 0;
  qmi_uim_http_transaction_resp_data_type  *rsp_ptr        = NULL;
  qmi_uim_http_client_info_type            *client_ptr     = NULL;
  qmi_uim_http_service_registry_id_type     service_id     = QMI_UIM_HTTP_SERVICE_ID_NOT_APPLICABLE;
  uim_http_transaction_resp_msg_v01         resp;
  
  (void) conn_handle;

  UIM_MSG_HIGH_0("qmi_uim_http_transaction_req_handler");

  if (req_handle == NULL || req_c_struct_len == 0x0 || qmi_uim_http_global_ptr == NULL)
  {
    return QMI_CSI_INTERNAL_ERR;
  }

  req_msg_ptr = (uim_http_transaction_req_msg_v01 *)req_c_struct;

  /* Initialize the resp result with success */
  resp.resp.error  = QMI_ERR_NONE_V01;
  resp.resp.result = QMI_RESULT_SUCCESS_V01;

  /* Extract the service_id and transaction_id from the TOKEN ID  */
  service_id       = QMI_UIM_HTTP_GET_SERVICE_ID_FROM_TOKEN_ID(req_msg_ptr->token_id);
  transaction_id   = QMI_UIM_HTTP_GET_TRANSACTION_ID_FROM_TOKEN_ID(req_msg_ptr->token_id);

  /* Find the service to which the http response belongs to based on the
     service_id in the http request message */
  for (service_index = 0;
       service_index < QMI_UIM_HTTP_MAX_NUM_SERVICE;
       service_index++)
  {
    if (qmi_uim_http_global_ptr->client_registry.curr_transaction[service_index].service_in_use &&
        qmi_uim_http_global_ptr->client_registry.curr_transaction[service_index].service_id == service_id)
    {
      break;
    }
  }

  if (service_index == QMI_UIM_HTTP_MAX_NUM_SERVICE)
  {
    UIM_MSG_LOW_0("qmi_uim_http_http_req_handler: Cannot find client");
    resp.resp.error  = QMI_ERR_INTERNAL_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  client_ptr =
    &(qmi_uim_http_global_ptr->client_registry);

  rsp_ptr = &(client_ptr->curr_transaction[service_index].rsp_data);

  if (client_ptr->curr_transaction[service_index].transaction_id != transaction_id)
  {
    UIM_MSG_ERR_2("Pending http transaction id 0x%x does not match recevied id 0x%x",
                   client_ptr->curr_transaction[service_index].transaction_id, transaction_id);
    resp.resp.error  = QMI_ERR_NO_EFFECT_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  rsp_ptr->result = qmi_uim_http_decode_result(req_msg_ptr->result);

  /* If client returns an error for the http request notify
     the service about the error */
  if (rsp_ptr->result != QMI_UIM_HTTP_TRANSACTION_SUCCESS)
  {
    goto callback;
  }

  /* If the client returns success for the http request
     but response message is malformed return a error to
     the client. Both segment info valid and payload valid should have same value */
  if (req_msg_ptr->segment_info_valid != req_msg_ptr->payload_body_valid)
  {
     UIM_MSG_ERR_3("For TOKEN id 0x%x, req msg is malformed with seg info valid 0x%x, payload valid 0x%x",
                  req_msg_ptr->token_id,
                  req_msg_ptr->segment_info_valid,
                  req_msg_ptr->payload_body_valid);
    resp.resp.error  = QMI_ERR_MALFORMED_MSG_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto callback;
  }
  
  /* If the client returns success for the http request
     but custom headers is malformed return a error to
     the client */
  if ((req_msg_ptr->headers_valid &&
       req_msg_ptr->headers_len > UIM_HTTP_CUST_HEADER_MAX_COUNT_V01))
  {
    UIM_MSG_ERR_3("For TOKEN id 0x%x, malformed cust hdr valid 0x%x, len 0x%x",
                  req_msg_ptr->token_id,
                  req_msg_ptr->headers_valid,
                  req_msg_ptr->headers_len);
    resp.resp.error  = QMI_ERR_MALFORMED_MSG_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto callback;
  }

  if(req_msg_ptr->headers_valid)
  {
    rsp_ptr->num_cust_headers = req_msg_ptr->headers_len;
    for(index = 0; index < rsp_ptr->num_cust_headers; index ++)	
    {
      rsp_ptr->headers[index].name_ptr = modem_mem_calloc(1,
                                         UIM_HTTP_HEADER_NAME_MAX_LEN_V01 + 1,
                                         MODEM_MEM_CLIENT_UIM);

      rsp_ptr->headers[index].value_ptr = modem_mem_calloc(1,
                                          UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 + 1,
                                          MODEM_MEM_CLIENT_UIM);

      if((rsp_ptr->headers[index].name_ptr == NULL) || (rsp_ptr->headers[index].value_ptr == NULL))
      {
        UIM_MSG_ERR_0("Response buffer NULL, heap exhaustion or corruption");
        resp.resp.error  = QMI_ERR_INTERNAL_V01;
        resp.resp.result = QMI_RESULT_FAILURE_V01;
        goto callback;                
      }

      (void)strlcpy((char*)rsp_ptr->headers[index].name_ptr, req_msg_ptr->headers[index].name, UIM_HTTP_HEADER_NAME_MAX_LEN_V01);
      (void)strlcpy((char*)rsp_ptr->headers[index].value_ptr, req_msg_ptr->headers[index].value, UIM_HTTP_HEADER_VALUE_MAX_LEN_V01); 
    }
  }

  if(req_msg_ptr->segment_info_valid)
  {
    /* Construct the response as follows.
       If this is the first response segment received determine the
       total response length and allocate required buffer size to build the
       response */
    if (rsp_ptr->payload_ptr == NULL)
    {
      rsp_ptr->payload_len = req_msg_ptr->segment_info.total_size;
      rsp_ptr->payload_ptr = modem_mem_calloc(1,
                             req_msg_ptr->segment_info.total_size,
                             MODEM_MEM_CLIENT_UIM);
    }

    if (rsp_ptr->payload_ptr == NULL)
    {
      UIM_MSG_ERR_0("Response buffer NULL, heap exhaustion or corruption");
      /* Since the receive bytes check in callback label fails, error status 
         is sent in modem client(eg - LPA) cb */
      goto callback;
    }

    /* Copy over the response segment into the allocated buffer as long as the
       segment offset + segment length does not exceed the total response
       length */
    if ((req_msg_ptr->segment_info.total_size >= req_msg_ptr->payload_body_len) &&
        (req_msg_ptr->segment_info.segment_offset <= 
         req_msg_ptr->segment_info.total_size - req_msg_ptr->payload_body_len))
    {
      memscpy(rsp_ptr->payload_ptr + req_msg_ptr->segment_info.segment_offset,
              rsp_ptr->payload_len - req_msg_ptr->segment_info.segment_offset,
              req_msg_ptr->payload_body,
              req_msg_ptr->payload_body_len);
    }
    else
    {
      /* Since the receive bytes check in callback label fails, error status 
         is sent in modem client(eg - LPA) cb */
      goto callback;
    }

    client_ptr->curr_transaction[service_index].received_bytes += req_msg_ptr->payload_body_len;

    /* There is an assumption that if there is more than one resp segment
       the client sends the segments in order and the transport
       also delivers the segments in order.
       If (segment offset + payload length) received is smaller than the
       total response size then we copy over the payload body received
       and wait for the next payload by just returning the QMI resopnse
       back to the client.
       If (segment offset + payload length) received is equal to total
       resp size then we have received the entire payload and we can
       call the http callback to deliver the response */
    if ((req_msg_ptr->segment_info.total_size >= req_msg_ptr->payload_body_len) &&
        (req_msg_ptr->segment_info.segment_offset < 
         req_msg_ptr->segment_info.total_size - req_msg_ptr->payload_body_len))
    {
      goto send_resp;
    }
  }

callback:
  if(client_ptr->curr_transaction[service_index].received_bytes != rsp_ptr->payload_len)
  {
    if(resp.resp.error ==  QMI_ERR_NONE_V01 || resp.resp.result == QMI_RESULT_SUCCESS_V01)
    {
      resp.resp.error  = QMI_ERR_INTERNAL_V01;
      resp.resp.result = QMI_RESULT_FAILURE_V01;
    }
    /* UNKNOWN error status sent in modem client (LPA) call back */    
    rsp_ptr->result  = QMI_UIM_HTTP_UNKNOWN_ERROR;
  }
  if (client_ptr->curr_transaction[service_index].rsp_cb_ptr)
  {
    (client_ptr->curr_transaction[service_index].rsp_cb_ptr)(rsp_ptr);
  }

  /* Free memory */
  for(index = 0; index < rsp_ptr->num_cust_headers; index ++)	
  {
    QMI_UIM_HTTP_FREE(rsp_ptr->headers[index].name_ptr);
    QMI_UIM_HTTP_FREE(rsp_ptr->headers[index].value_ptr);
  }
  /* As the callback has been called and response to http request
     is received, the response buffer is freed */
  QMI_UIM_HTTP_FREE(rsp_ptr->payload_ptr);
  memset(rsp_ptr, 0x0, sizeof(qmi_uim_http_transaction_resp_data_type));
  
  client_ptr->curr_transaction[service_index].rsp_cb_ptr     = NULL;
  client_ptr->curr_transaction[service_index].received_bytes  = 0;
  client_ptr->curr_transaction[service_index].service_in_use = FALSE;
  client_ptr = NULL;

send_resp:
  return qmi_csi_send_resp(req_handle,
                           QMI_UIM_HTTP_TRANSACTION_RESP_V01,
                           &resp,
                           sizeof(resp));
} /* qmi_uim_http_transaction_req_handler */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_CMD_MEM_FREE

   DESCRIPTION:
     This function is used to free the memory allocated to process a command
     queued by modem client using qmi uim http service

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_http_cmd_mem_free (
  qmi_uim_http_cmd_type       * cmd_ptr
)
{
  uint8 index = 0;
  
  if (cmd_ptr == NULL)
  {
    return;
  }

  QMI_UIM_HTTP_FREE(cmd_ptr->cmd_data.payload_ptr);
  QMI_UIM_HTTP_FREE(cmd_ptr->cmd_data.url_ptr);

  if(cmd_ptr->cmd_data.header_ptr)
  {
    for(index = 0;
        index < cmd_ptr->cmd_data.header_ptr->num_headers  &&  index < UIM_HTTP_CUST_HEADER_MAX_COUNT_V01;
        index ++)   
    {
      QMI_UIM_HTTP_FREE(cmd_ptr->cmd_data.header_ptr->headers[index].name_ptr);
      QMI_UIM_HTTP_FREE(cmd_ptr->cmd_data.header_ptr->headers[index].value_ptr);
    }
    QMI_UIM_HTTP_FREE(cmd_ptr->cmd_data.header_ptr);
  }
  QMI_UIM_HTTP_FREE(cmd_ptr);
} /* qmi_uim_http_cmd_mem_free */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_PROCESS_TRANSACTION_REQUEST

   DESCRIPTION:
     This function sends http payload in chunks through multiple QSCI indications

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_http_process_transaction_request (
  const  qmi_uim_http_cmd_type   * cmd_ptr,
  uint8                            service_index
)
{
  uint8                                     index             = 0;
  uint8                                     chunk_index       = 0;
  uint8                                     num_chunks        = 0;
  qmi_csi_error                             ind_status        = QMI_CSI_NO_ERR;
  uim_http_transaction_ind_msg_v01        * ind_c_struct_ptr  = NULL;
  qmi_uim_http_transaction_resp_data_type   rsp_data;
  qmi_uim_http_curr_transaction_info_type * curr_transaction_ptr;

  memset(&rsp_data, 0x0, sizeof(qmi_uim_http_transaction_resp_data_type));

  if ((cmd_ptr == NULL) || (qmi_uim_http_global_ptr == NULL) || (cmd_ptr->cmd_data.cb_ptr == NULL))
  {
    return;
  }

  if(cmd_ptr->cmd_data.payload_len == 0)
  {
    num_chunks = 1;
  }
  else
  {
    num_chunks = ((cmd_ptr->cmd_data.payload_len - 1) / UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01) + 1;
  }

  curr_transaction_ptr = &qmi_uim_http_global_ptr->client_registry.curr_transaction[service_index];

  ind_c_struct_ptr  = (uim_http_transaction_ind_msg_v01 *) modem_mem_calloc(1,
                                     sizeof(uim_http_transaction_ind_msg_v01),
                                     MODEM_MEM_CLIENT_UIM);

  if (ind_c_struct_ptr == NULL)
  {
    rsp_data.result  = QMI_UIM_HTTP_UNKNOWN_ERROR;
    rsp_data.client_ref_ptr = cmd_ptr->cmd_data.client_ref_ptr;
    cmd_ptr->cmd_data.cb_ptr(&rsp_data);
    return; 
  }

  curr_transaction_ptr->transaction_id++;
  curr_transaction_ptr->service_in_use          = TRUE;
  curr_transaction_ptr->service_id              = cmd_ptr->cmd_data.service_id;
  curr_transaction_ptr->rsp_cb_ptr              = cmd_ptr->cmd_data.cb_ptr;
  curr_transaction_ptr->rsp_data.client_ref_ptr = cmd_ptr->cmd_data.client_ref_ptr;

  for (chunk_index = 0; chunk_index < num_chunks; chunk_index++)
  {
    memset(ind_c_struct_ptr, 0x0, sizeof(uim_http_transaction_ind_msg_v01));

    /* For all chunks, token ID(combo of transaction ID + service ID) will be same */
    ind_c_struct_ptr->token_id  =
      QMI_UIM_HTTP_CREATE_TOKEN_ID(curr_transaction_ptr->service_id, curr_transaction_ptr->transaction_id);

    ind_c_struct_ptr->segment_info_valid      = TRUE;
    ind_c_struct_ptr->segment_info.total_size = cmd_ptr->cmd_data.payload_len;
    
    ind_c_struct_ptr->segment_info.segment_offset = 
      (chunk_index * UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01);

    if(cmd_ptr->cmd_data.payload_len > 0)
    {
      ind_c_struct_ptr->payload_body_valid      = TRUE;

      if ((cmd_ptr->cmd_data.payload_len - 
             ind_c_struct_ptr->segment_info.segment_offset) 
               > UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01 )
      {
        ind_c_struct_ptr->payload_body_len = UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01;
      }
      else
      {
        ind_c_struct_ptr->payload_body_len = 
          (cmd_ptr->cmd_data.payload_len - ind_c_struct_ptr->segment_info.segment_offset);
      }

      memscpy(ind_c_struct_ptr->payload_body,
              ind_c_struct_ptr->payload_body_len,
              cmd_ptr->cmd_data.payload_ptr 
                + ind_c_struct_ptr->segment_info.segment_offset,
              UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01);
    }
  
    /* Req Header and URL can be passed in first chunk itself 
       and not required in the subsequent chunks */
    if(chunk_index == 0)
    {
      ind_c_struct_ptr->url_valid = TRUE;
      (void)strlcpy((char*)(ind_c_struct_ptr->url), 
                           cmd_ptr->cmd_data.url_ptr,
                           UIM_HTTP_URL_MAX_LEN_V01);

      ind_c_struct_ptr->headers_valid = TRUE;
      (void)strlcpy((char*)(ind_c_struct_ptr->headers.content_type),
                    cmd_ptr->cmd_data.header_ptr->content_type,
                    UIM_HTTP_HEADER_VALUE_MAX_LEN_V01);

      ind_c_struct_ptr->headers.custom_header_len = cmd_ptr->cmd_data.header_ptr->num_headers;

      for(index = 0; 
          index < cmd_ptr->cmd_data.header_ptr->num_headers && index < UIM_HTTP_CUST_HEADER_MAX_COUNT_V01;
          index++)
      {
        (void)strlcpy((char*)(ind_c_struct_ptr->headers.custom_header[index].name), 
                      cmd_ptr->cmd_data.header_ptr->headers[index].name_ptr, 
                      UIM_HTTP_HEADER_NAME_MAX_LEN_V01);

        (void)strlcpy((char*)(ind_c_struct_ptr->headers.custom_header[index].value), 
                      cmd_ptr->cmd_data.header_ptr->headers[index].value_ptr,
                      UIM_HTTP_HEADER_VALUE_MAX_LEN_V01);
      }
    }
    
    ind_status = qmi_csi_send_ind(qmi_uim_http_global_ptr->client_registry.client_handle,
                                  QMI_UIM_HTTP_TRANSACTION_IND_V01,
                                  (void *)ind_c_struct_ptr,
                                  sizeof(uim_http_transaction_ind_msg_v01));
    UIM_MSG_HIGH_3("Sent chunk 0x%x of total chunks 0x%x with Send indication status : 0x%x",
                                                      chunk_index + 1, num_chunks, ind_status);

    if (ind_status != QMI_CSI_NO_ERR)
    {
      rsp_data.result         = QMI_UIM_HTTP_UNKNOWN_ERROR;
      rsp_data.client_ref_ptr = cmd_ptr->cmd_data.client_ref_ptr;
      cmd_ptr->cmd_data.cb_ptr(&rsp_data);
      break;
    }
  }
  QMI_UIM_HTTP_FREE(ind_c_struct_ptr);
} /* qmi_uim_http_process_transaction_request */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_PROCESS_CMD

   DESCRIPTION:
     This function processes the QMI UIM HTTP command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_http_process_cmd (
  void
)
{
  uint8                                     service_index   = 0x0;
  qmi_uim_http_cmd_type                   * cmd_ptr         = NULL;
  qmi_uim_http_transaction_resp_data_type   rsp_data;

  memset(&rsp_data, 0x0, sizeof(qmi_uim_http_transaction_resp_data_type));

  if(qmi_uim_http_global_ptr == NULL)
  {
    return;
  }

  cmd_ptr = (qmi_uim_http_cmd_type *)
              q_get(&(qmi_uim_http_global_ptr->cmd_q));

  if ((cmd_ptr == NULL) || (cmd_ptr->cmd_data.cb_ptr == NULL))
  {
    return;
  }

  for (service_index = 0;
       service_index < QMI_UIM_HTTP_MAX_NUM_SERVICE;
       service_index++)
  {
    if (qmi_uim_http_global_ptr->service_registry[service_index].service_id ==
          cmd_ptr->cmd_data.service_id)
    {
      break;
    }
  }
 
  if (service_index == QMI_UIM_HTTP_MAX_NUM_SERVICE)
  { 
    /* Case where the service_id in the cmd has exceeded the max services supported */
    UIM_MSG_ERR_0("Unable to find card service registry to send command");
    rsp_data.result         = QMI_UIM_HTTP_UNKNOWN_ERROR;
    rsp_data.client_ref_ptr = cmd_ptr->cmd_data.client_ref_ptr;
    cmd_ptr->cmd_data.cb_ptr(&rsp_data);
  }
  else
  { 
    qmi_uim_http_process_transaction_request(cmd_ptr, service_index);
  }
  qmi_uim_http_cmd_mem_free(cmd_ptr);
} /* qmi_uim_http_process_cmd */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_CMD_Q_SIG_HANDLER

  DESCRIPTION:
    QMI UIM HTTP CMD Q signal handler

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     Returns TRUE if command queue is empty
     FALSE if there are more commands to process

   SIDE EFFECTS:
     None
===========================================================================*/
boolean qmi_uim_http_cmd_q_sig_handler (
  void
)
{
  if(qmi_uim_http_global_ptr == NULL)
  {
    return FALSE;
  }
  qmi_uim_http_process_cmd();

  /* This signal is set asyncronously however the framework handles
     concurrency issues so no need to worry about it here */
  if (q_cnt(&qmi_uim_http_global_ptr->cmd_q) == 0)
  {
    return TRUE;
  }
  return FALSE;
} /* qmi_uim_http_cmd_q_sig_handler */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_PUT_CMD_IN_Q

   DESCRIPTION:
     This function allows putting of command into the QMI UIM HTTP CMD queue.
     This is used by the modem clients to send a request to the
     server that is off-modem.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_http_result_type

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_uim_http_result_type qmi_uim_http_put_cmd_in_q (
  qmi_uim_http_cmd_type  * qmi_uim_http_cmd_ptr
)
{
  if ((qmi_uim_http_cmd_ptr == NULL) || (qmi_uim_http_global_ptr == NULL))
  {
    return QMI_UIM_HTTP_ERROR;
  }

  if (q_cnt(&qmi_uim_http_global_ptr->cmd_q) >= QMI_UIM_HTTP_CMD_QUEUE_MAX_SIZE)
  {
    UIM_MSG_LOW_0("Max cmds in qmi uim http cmd_q");
    return QMI_UIM_HTTP_ERROR;
  }

  (void)q_link(qmi_uim_http_cmd_ptr, &qmi_uim_http_cmd_ptr->link);

  /* Put the message on the queue */
  q_put(&qmi_uim_http_global_ptr->cmd_q, &qmi_uim_http_cmd_ptr->link);

  /* Set the command queue signal */
  (void)lpa_set_qmi_uim_http_cmd_q_sig();

  UIM_MSG_HIGH_0("Successfully queued qmi uim http cmd");
  return QMI_UIM_HTTP_SUCCESS;
} /* qmi_uim_http_put_cmd_in_q */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_SERVICE_REGISTER

   DESCRIPTION:
     This function registers the calling task with the UIM HTTP service.
     The function returns immediately and the service sends events with status
     of the connection

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_http_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_http_result_type qmi_uim_http_service_register (
  qmi_uim_http_service_registry_id_type    *service_id
)
{
  uint8  index = 0;
  qmi_uim_http_result_type    cmd_status = QMI_UIM_HTTP_ERROR;

  if ((service_id == NULL) || (qmi_uim_http_global_ptr == NULL))
  {
    UIM_MSG_LOW_0("QMI UIM HTTP register input validation failed");
    return cmd_status;
  }

  rex_enter_crit_sect(&qmi_uim_http_global_ptr->crit_sect);
  
  for (index = 0; index < QMI_UIM_HTTP_MAX_NUM_SERVICE; index++)
  {
    if (qmi_uim_http_global_ptr->service_registry[index].in_use == FALSE)
    {
      break;
    }
  }
  
  if (index == QMI_UIM_HTTP_MAX_NUM_SERVICE)
  {
    rex_leave_crit_sect(&qmi_uim_http_global_ptr->crit_sect);
    UIM_MSG_LOW_0("qmi_uim_http_http_req_handler: Cannot find client");
    return cmd_status;
  }
  
  qmi_uim_http_global_ptr->service_registry[index].in_use = TRUE;
  qmi_uim_http_global_ptr->service_registry[index].service_id = 
             (qmi_uim_http_service_registry_id_type) (index + 1);
  
  rex_leave_crit_sect(&qmi_uim_http_global_ptr->crit_sect);
  
  *service_id = qmi_uim_http_global_ptr->service_registry[index].service_id;

  return QMI_UIM_HTTP_SUCCESS;
} /* qmi_uim_http_service_register */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_VALIDATE_TRANSACTION_PARAMETERS

   DESCRIPTION:
     This function validates the input transaction parameters

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_http_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
static qmi_uim_http_result_type qmi_uim_http_validate_transaction_parameters (
  const qmi_uim_http_transaction_req_data_type  * transaction_ptr)
{
  qmi_uim_http_result_type result =  QMI_UIM_HTTP_SUCCESS;

  if (transaction_ptr               == NULL         ||
      transaction_ptr->service_id   ==
        QMI_UIM_HTTP_SERVICE_ID_NOT_APPLICABLE      ||
      transaction_ptr->service_id   >= 
        QMI_UIM_HTTP_MAX_SERVICE_ID                 ||
      transaction_ptr->cb_ptr       == NULL         ||
      transaction_ptr->url_ptr      == NULL         ||
      strlen(transaction_ptr->url_ptr)  ==  0       ||
      transaction_ptr->header_ptr   == NULL         ||
      transaction_ptr->header_ptr->num_headers > 
        UIM_HTTP_CUST_HEADER_MAX_COUNT_V01          ||
      transaction_ptr->payload_ptr  == NULL)
  {
    UIM_MSG_LOW_0("HTTP send http request - bad input parameters");
    return QMI_UIM_HTTP_ERROR;
  }

  return result;
} /* qmi_uim_http_validate_transaction_parameters */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_SEND_TRANSACTION_REQUEST

   DESCRIPTION:
     This function allows the calling task to send an http request via the QMI UIM
     HTTP service

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_http_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_http_result_type qmi_uim_http_send_transaction_request (
  const qmi_uim_http_transaction_req_data_type  * qmi_uim_http_transaction_ptr
)
{
  uint8                       index      = 0;
  qmi_uim_http_cmd_type     * cmd_ptr    = NULL;
  qmi_uim_http_result_type    cmd_status = QMI_UIM_HTTP_ERROR;

  if(QMI_UIM_HTTP_SUCCESS != 
       qmi_uim_http_validate_transaction_parameters(qmi_uim_http_transaction_ptr))
  {
    return QMI_UIM_HTTP_ERROR;
  }

  cmd_ptr =  modem_mem_calloc(1, sizeof(qmi_uim_http_cmd_type),
                              MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr == NULL)
  {
    return QMI_UIM_HTTP_ERROR;
  }

  cmd_ptr->cmd_data.payload_ptr =
    modem_mem_calloc(1, qmi_uim_http_transaction_ptr->payload_len, MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr->cmd_data.payload_ptr == NULL)
  {
    qmi_uim_http_cmd_mem_free(cmd_ptr);
    return QMI_UIM_HTTP_ERROR;
  }
  
  cmd_ptr->cmd_data.header_ptr =
    modem_mem_calloc(1, sizeof(qmi_uim_http_transaction_header_info_type), MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr->cmd_data.header_ptr == NULL)
  {
    qmi_uim_http_cmd_mem_free(cmd_ptr);
    return QMI_UIM_HTTP_ERROR;
  }

  cmd_ptr->cmd_data.service_id         = qmi_uim_http_transaction_ptr->service_id;
  cmd_ptr->cmd_data.client_ref_ptr     = qmi_uim_http_transaction_ptr->client_ref_ptr;
  cmd_ptr->cmd_data.payload_len        = qmi_uim_http_transaction_ptr->payload_len;
  
  memscpy(cmd_ptr->cmd_data.payload_ptr, cmd_ptr->cmd_data.payload_len,
          qmi_uim_http_transaction_ptr->payload_ptr, qmi_uim_http_transaction_ptr->payload_len);

  (void)strlcpy((char*)cmd_ptr->cmd_data.header_ptr->content_type, 
                 qmi_uim_http_transaction_ptr->header_ptr->content_type,
                 UIM_HTTP_HEADER_VALUE_MAX_LEN_V01);

  cmd_ptr->cmd_data.header_ptr->num_headers = 
                qmi_uim_http_transaction_ptr->header_ptr->num_headers;

  for(index = 0; index < cmd_ptr->cmd_data.header_ptr->num_headers ; index++)
  {
    cmd_ptr->cmd_data.header_ptr->headers[index].name_ptr =
      modem_mem_calloc(1,
                       UIM_HTTP_HEADER_NAME_MAX_LEN_V01 + 1,
                       MODEM_MEM_CLIENT_UIM);

    cmd_ptr->cmd_data.header_ptr->headers[index].value_ptr =
      modem_mem_calloc(1,
                       UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 + 1,
                       MODEM_MEM_CLIENT_UIM);

    if(cmd_ptr->cmd_data.header_ptr->headers[index].name_ptr == NULL ||
       cmd_ptr->cmd_data.header_ptr->headers[index].value_ptr == NULL )
    {
      qmi_uim_http_cmd_mem_free(cmd_ptr);
      return QMI_UIM_HTTP_ERROR;
    }
    (void)strlcpy(cmd_ptr->cmd_data.header_ptr->headers[index].name_ptr, 
                  qmi_uim_http_transaction_ptr->header_ptr->headers[index].name_ptr,
                  UIM_HTTP_HEADER_NAME_MAX_LEN_V01);

    (void)strlcpy(cmd_ptr->cmd_data.header_ptr->headers[index].value_ptr,
                  qmi_uim_http_transaction_ptr->header_ptr->headers[index].value_ptr, 
                  UIM_HTTP_HEADER_VALUE_MAX_LEN_V01);
  }
		  
  cmd_ptr->cmd_data.url_ptr = modem_mem_calloc(1, UIM_HTTP_URL_MAX_LEN_V01 + 1, MODEM_MEM_CLIENT_UIM);
  if(cmd_ptr->cmd_data.url_ptr == NULL)
  {
    qmi_uim_http_cmd_mem_free(cmd_ptr);
    return QMI_UIM_HTTP_ERROR;
  }

 (void)strlcpy(cmd_ptr->cmd_data.url_ptr, 
               qmi_uim_http_transaction_ptr->url_ptr,
               UIM_HTTP_URL_MAX_LEN_V01);
		  
  cmd_ptr->cmd_data.cb_ptr = qmi_uim_http_transaction_ptr->cb_ptr;
  cmd_status = qmi_uim_http_put_cmd_in_q (cmd_ptr);
  if (cmd_status != QMI_UIM_HTTP_SUCCESS)
  {
    qmi_uim_http_cmd_mem_free(cmd_ptr);
  }
  return cmd_status;
} /* qmi_uim_http_send_transaction_request */


/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_SERVICE_RELEASE

   DESCRIPTION:
     This function un-registers the calling task from the QMI UIM HTTP
     service.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_http_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_http_result_type qmi_uim_http_service_release (
  qmi_uim_http_service_registry_id_type    service_id
)
{
  uint8                                     index         = 0;
  uint8                                     header_index  = 0;
  qmi_uim_http_result_type                  cmd_status    = QMI_UIM_HTTP_ERROR;
  qmi_uim_http_transaction_resp_data_type  *rsp_ptr       = NULL;
  qmi_uim_http_client_info_type            *client_ptr    = NULL;  

  if(qmi_uim_http_global_ptr == NULL)
  {
    return cmd_status;
  }

  for (index = 0; index < QMI_UIM_HTTP_MAX_NUM_SERVICE; index++)
  {
    if (qmi_uim_http_global_ptr->service_registry[index].service_id ==
          service_id)
    {
      break;
    }
  }
  
  if(index == QMI_UIM_HTTP_MAX_NUM_SERVICE)
  {
    return cmd_status;
  }

  client_ptr =  &(qmi_uim_http_global_ptr->client_registry);
  rsp_ptr    =  &(client_ptr->curr_transaction[index].rsp_data);
  
  if (client_ptr->curr_transaction[index].rsp_cb_ptr)
  {
    rsp_ptr->result  = QMI_UIM_HTTP_UNKNOWN_ERROR;
    (client_ptr->curr_transaction[index].rsp_cb_ptr)(rsp_ptr);
  }

  for(header_index = 0;
      header_index < rsp_ptr->num_cust_headers && header_index < UIM_HTTP_CUST_HEADER_MAX_COUNT_V01;
      header_index ++) 
  {
    QMI_UIM_HTTP_FREE(rsp_ptr->headers[header_index].name_ptr);
    QMI_UIM_HTTP_FREE(rsp_ptr->headers[header_index].value_ptr);
  }

  QMI_UIM_HTTP_FREE(rsp_ptr->payload_ptr);
  memset(rsp_ptr, 0x0, sizeof(qmi_uim_http_transaction_resp_data_type));

  client_ptr->curr_transaction[index].rsp_cb_ptr	 = NULL;
  client_ptr->curr_transaction[index].received_bytes  = 0;
  client_ptr->curr_transaction[index].service_in_use = FALSE;
  client_ptr = NULL;

  qmi_uim_http_global_ptr->service_registry[index].in_use = FALSE;
  qmi_uim_http_global_ptr->service_registry[index].service_id =
    QMI_UIM_HTTP_SERVICE_ID_NOT_APPLICABLE;
  return QMI_UIM_HTTP_SUCCESS;
 } /* qmi_uim_http_service_release */


/*=========================================================================== 
  FUNCTION QMI_UIM_HTTP_DEREG 
  
  DESCRIPTION 
    Called to deregister the QMI UIM http as QMI service and as SAP client on 
    reception of task stop signal 
  
  PARAMETERS 
    None 
 
  RETURN VALUE 
    None 
  
  DEPENDENCIES 
    None 
  
  SIDE EFFECTS 
    None 
===========================================================================*/ 
void qmi_uim_http_dereg( 
  void 
)
{
  if (qmi_uim_http_global_ptr == NULL)
  {
    return;
  }

  (void)qmi_csi_unregister (qmi_uim_http_global_ptr->service_handle);

  (void)qmi_sap_deregister (qmi_uim_http_global_ptr->sap_handle);
}/* qmi_uim_http_dereg */