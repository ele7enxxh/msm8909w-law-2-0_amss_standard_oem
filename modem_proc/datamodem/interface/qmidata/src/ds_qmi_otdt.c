/*===========================================================================

                    DS_QMI_OTDT.C

DESCRIPTION
  Implementation of DS QMI OTDT.
  This service allows to control On-target data test application.

EXTERNALIZED FUNCTIONS
  ds_qmi_otdt_send_ind()
    Sends indication of the test result back to the client.

  
Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_otdt.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/17/12   prp     Removed on_target_data_test_v01.h  3698-6829
09/02/12    aa     Created module
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

//#include "on_target_data_test_v01.h"
#include "qmi_svc_defs.h"
#include "ds_qmi_svc.h"
#include "qmi_core_server.h"
#include "ps_system_heap.h"
#include "ds_Utils_DebugMsg.h"
#include "qmi_sap.h"
#include "ds_ontarget_test_svc.h"
#include "stringl/stringl.h"


/*===========================================================================

                                 DEFINITIONS

===========================================================================*/
#define DS_QMI_OTDT_SVC_WAIT_SIG (0x00040000)
#define NUM_INDICATIONS                   (1)

/* This struct keeps track of the clients that connects through connect callback */
typedef  struct {
  /* First element of structure should be connection object */
  qmi_core_conn_obj_type        conn_obj;

  /* Indications for test status*/
  qmi_indication_type           test_status_ind;

  /* Rest of the client data goes here */

}ds_qmi_ontarget_ds_test_client_type;

typedef struct 
{
  /* First element of structure should be connection object */
  qmi_core_server_object_type            core_object;
  ds_qmi_ontarget_ds_test_client_type    client_data;
  boolean                                is_client_connected;
} ds_qmi_otdt_type;

/*===========================================================================

                            FORWARD DECLARATIONS

===========================================================================*/
static qmi_dispatch_service_error_type ds_qmi_otdt_start_test
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 );

static qmi_dispatch_service_error_type ds_qmi_otdt_stop_test
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 );

/*===========================================================================

                                 GLOBALS

===========================================================================*/

/* Dispatch Table for time server */
qmi_msg_handler_type ds_qmi_otdt_dispatcher[] = {
  {QMI_OTDT_START_TEST_REQ_V01,(qmi_dispatch_fn_type)ds_qmi_otdt_start_test},
  {QMI_OTDT_STOP_TEST_REQ_V01,(qmi_dispatch_fn_type)ds_qmi_otdt_stop_test},
};

/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  ds_qmi_otdt_event_loop
===========================================================================*/
/*!
@brief
  Generic event loop for the service

@param[in]  param                       Ingored

@retval    None
*/
/*=========================================================================*/
static void ds_qmi_otdt_event_loop(dword param)
{
  rex_sigs_type      wait_on,sig_received;
  qmi_csi_os_params  *os_params;
  ds_qmi_otdt_type* me = 
    (ds_qmi_otdt_type*)param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ds_qmi_otdt_event_loop(): ");
  os_params = qmi_core_server_get_os_params(&me->core_object);
  wait_on = DS_QMI_OTDT_SVC_WAIT_SIG;

  while (1) {
    sig_received = rex_wait(wait_on);
    rex_clr_sigs(os_params->tcb,sig_received);
    LOG_MSG_INFO1_0("ds_qmi_otdt_event_loop(): "
                    "dispatching event");
    qmi_core_server_handle_event(me);
  }
}

/*===========================================================================
FUNCTION  ds_qmi_otdt_start_test
===========================================================================*/
/*!
@brief
  Handle for QMI_OTDT_START_TEST_REQ message

@param[in]  server_data           Pointer to server object. 
@param[in]  conn_obj              Pointer to connection object.
@param[in]  req_handle            Handle provided by the infrastructure
                                  to specify this particular transaction and
                                  message.  
@param[in]  msg_id                Message ID for this particular message.
@param[in]  req_c_struct          C struct with the decoded data.
@param[in]  req_c_struct_len      Length of c struct. 


@retval    Returns a QMI_DISPATCH_SERVICE_NO_ERR on Success.
@retval    QMI_DISPATCH_...    Error, see error codes defined
                               in qmi_dispatch_service_error_type 
*/
/*=========================================================================*/
static qmi_dispatch_service_error_type ds_qmi_otdt_start_test
(
  void               *server_data,
  void               *conn_obj,
  qmi_req_handle     req_handle,
  uint32_t           msg_id,
  void               *req_c_struct,
  uint32_t           req_c_struct_len
)
{
  otdt_start_test_resp_msg_v01           resp_msg;
  otdt_start_test_req_msg_v01*           request = 
    (otdt_start_test_req_msg_v01*)req_c_struct;
  ds_ontarget_test_cmd_data_type*        cmd_data_ptr;
  ds_qmi_otdt_type*  me = 
    (ds_qmi_otdt_type*)server_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ds_qmi_otdt_start_test(): ");
  
  memset(&resp_msg, 0, sizeof(otdt_start_test_resp_msg_v01));
  
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;

  // Prepare command data
   PS_SYSTEM_HEAP_MEM_ALLOC(cmd_data_ptr, sizeof(ds_ontarget_test_cmd_data_type),
                            ds_ontarget_test_cmd_data_type*);
  if (NULL == cmd_data_ptr) {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
 
  memset(cmd_data_ptr, 0, sizeof(ds_ontarget_test_cmd_data_type));
  strlcpy(cmd_data_ptr->test_group_name,
          request->test_group,
          TEST_GROUP_NAME_MAX_LEN);
  
  cmd_data_ptr->test_number = request->test_number;
  if (TRUE == request->task_priority_valid) {
    if (OTDT_PRIORITY_ENUM_MAX_V01 <= request->task_priority) {
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
      goto send_result;
    }
    
    cmd_data_ptr->priority = request->task_priority;
  }
  else {
    cmd_data_ptr->priority = OTDT_PRIORITY_DEFAULT_V01;
  }

  cmd_data_ptr->qmi_handle = me;

  if (0 != ds_ontarget_test_send_cmd(DS_ONTARGET_TEST_START_CMD, cmd_data_ptr)) {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  
send_result:
  return (qmi_dispatch_service_error_type) qmi_core_server_send_resp(req_handle,
                                   QMI_OTDT_START_TEST_RESP_V01,
                                   &resp_msg,
                                   sizeof(otdt_start_test_resp_msg_v01));
}

/*===========================================================================
FUNCTION  ds_qmi_otdt_start_test
===========================================================================*/
/*!
@brief
Handle for QMI_OTDT_START_STOP_REQ message

@param[in]  server_data           Pointer to server object. 
@param[in]  conn_obj              Pointer to connection object.
@param[in]  req_handle            Handle provided by the infrastructure
                                  to specify this particular transaction and
                                  message.  
@param[in]  msg_id                Message ID for this particular message.
@param[in]  req_c_struct          C struct with the decoded data.
@param[in]  req_c_struct_len      Length of c struct. 


@retval    Returns a QMI_DISPATCH_SERVICE_NO_ERR on Success.
@retval    QMI_DISPATCH_...    Error, see error codes defined
in qmi_dispatch_service_error_type 
*/
/*=========================================================================*/
static qmi_dispatch_service_error_type ds_qmi_otdt_stop_test
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 )
{
  otdt_stop_test_resp_msg_v01           resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ds_qmi_otdt_stop_test(): ");

  memset(&resp_msg, 0, sizeof(otdt_stop_test_resp_msg_v01));

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;

  ds_ontarget_test_stop_test_excecution();

  return (qmi_dispatch_service_error_type) qmi_core_server_send_resp(req_handle,
                                   QMI_OTDT_STOP_TEST_RESP_V01,
                                   &resp_msg,
                                   sizeof(otdt_stop_test_resp_msg_v01));

}

/*===========================================================================
              Callback Implementation of QCSI interface.
===========================================================================*/ 
static qmi_csi_cb_error ds_qmi_otdt_connect
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
)
{
  qmi_core_server_error_type                               rc;
  ds_qmi_otdt_type* me = 
    (ds_qmi_otdt_type*)service_cookie;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_1("ds_qmi_otdt_connect(): "
                  "client 0x%x", client_handle);
  
  if (NULL == me) {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  // This service allow only one connection simultaneously
  if (TRUE == me->is_client_connected) {
    return QMI_CSI_CB_CONN_REFUSED;
  }
  

  rc = qmi_core_server_register_client(&me->client_data.conn_obj,
                                       &me->core_object,
                                       client_handle,
                                       NUM_INDICATIONS,
                                       &me->client_data);
  if (QMI_CORE_SERVER_NO_ERR != rc) {
    LOG_MSG_INFO1_1("ds_qmi_otdt_connect(): "
                    "qmi_core_server_register_client() returned %d", rc);
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  me->is_client_connected = TRUE;
  *connection_handle = &me->client_data.conn_obj;
  return QMI_CSI_CB_NO_ERR;
}

static void ds_qmi_otdt_disconnect
(
  void                              *connection_handle,
  void                              *service_cookie
) 
{
  ds_qmi_otdt_type* me = 
    (ds_qmi_otdt_type*)service_cookie;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_1("ds_qmi_otdt_disconnect(): "
                  "client 0x%x", connection_handle);
  
  me->is_client_connected = FALSE;
}

static qmi_csi_cb_error ds_qmi_otdt_process_req
(
  void                              *connection_handle,
  qmi_req_handle                    req_handle,
  unsigned int                      msg_id,
  void                              *req_c_struct,
  unsigned int                      req_c_struct_len,
  void                              *service_cookie
)
{
  
  ds_qmi_otdt_type* me = 
                    (ds_qmi_otdt_type*)service_cookie;
  qmi_core_server_error_type rc = QMI_CORE_SERVER_NO_ERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_2("ds_qmi_otdt_process_req(): "
                  "client 0x%x msg %u", connection_handle, msg_id);

  rc = qmi_core_server_dispatch_msg((qmi_core_conn_obj_type *)connection_handle,
                                    me,
                                    req_handle,
                                    msg_id,
                                    req_c_struct,
                                    req_c_struct_len);
  if (QMI_CORE_SERVER_NO_ERR != rc) {
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
qmi_core_server_error_type ds_qmi_otdt_init
(
  void
)
{
  qmi_core_server_error_type                   rc = QMI_CORE_SERVER_NO_ERR;
  uint32_t                                     priority = 14;// TODO: Decide the proper priority for service
  unsigned long                                sig = DS_QMI_OTDT_SVC_WAIT_SIG;
  static ds_qmi_otdt_type* me = NULL;
  qmi_sap_error                                sap_rc = QMI_SAP_NO_ERR;
  qmi_indication_error_type                    ind_rc = QMI_INDICATION_NO_ERR;
  qmi_sap_client_handle                        sap_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ds_qmi_otdt_init(): ");

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(me, sizeof(ds_qmi_otdt_type),
                                  ds_qmi_otdt_type*);
  if (NULL == me) {
     return QMI_CORE_SERVER_MEMORY_ERR;
  }

  rc = qmi_core_server_new(&me->core_object,
                           "OTDT",
                           1, /* instance id */
                           1, /* new task for service */
                           (void*)ds_qmi_otdt_event_loop,
                           (void*)&priority,
                           NULL,
                           (void*)&sig,
                           NULL,
                           ds_qmi_otdt_dispatcher,
                           sizeof(ds_qmi_otdt_dispatcher)/sizeof(qmi_msg_handler_type));
 
  if (QMI_CORE_SERVER_NO_ERR != rc) {
    LOG_MSG_ERROR_1("ds_qmi_otdt_init(): "
                    "qmi_core_server_new() returned %d", rc);
    PS_SYSTEM_HEAP_MEM_FREE(me);
    ASSERT(0);
    return rc;
  }

  /* register QMI_OTDT_TEST_STATUS_IND */
  ind_rc = qmi_indication_initialize(&me->client_data.test_status_ind,
                                     QMI_UNICAST_IND,
                                     QMI_OTDT_TEST_STATUS_IND_V01,
                                     sizeof(qmi_indication_type),
                                     NULL);
  
  if (QMI_INDICATION_NO_ERR != ind_rc) {
    LOG_MSG_ERROR_1("ds_qmi_otdt_init(): "
                    "qmi_indication_initialize() returned %d", rc);
    rc = QMI_CORE_SERVER_INTERNAL_ERR;
    qmi_core_server_delete(&me->core_object);
    PS_SYSTEM_HEAP_MEM_FREE(me);
    ASSERT(0);
    return rc;
  }

  rc = qmi_core_server_register(me,
                                otdt_get_service_object_v01(),
                                ds_qmi_otdt_connect,
                                ds_qmi_otdt_disconnect,
                                ds_qmi_otdt_process_req);
  
  if (QMI_CORE_SERVER_NO_ERR != rc) {
    LOG_MSG_ERROR_1("ds_qmi_otdt_init(): "
                    "qmi_core_server_register() returned %d", rc);
    qmi_core_server_delete(&me->core_object);
    PS_SYSTEM_HEAP_MEM_FREE(me);
    ASSERT(0);
    return rc;
  }

  sap_rc = qmi_sap_register(otdt_get_service_object_v01(),
                            NULL,
                            &sap_handle);
  if (QMI_SAP_NO_ERR != sap_rc) {
    LOG_MSG_ERROR_1("ds_qmi_otdt_init(): "
                    "qmi_sap_register() returned %d", sap_rc);
    rc = QMI_CORE_SERVER_INTERNAL_ERR;
    qmi_core_server_unregister(me);
    qmi_core_server_delete(&me->core_object);
    PS_SYSTEM_HEAP_MEM_FREE(me);
    ASSERT(0);
    return rc;
  }

  rc = qmi_core_server_start_server(me);
  if (QMI_CORE_SERVER_NO_ERR != rc) {
    LOG_MSG_ERROR_1("ds_qmi_otdt_init(): "
                    "qmi_core_server_start_server() returned %d", rc);
    qmi_sap_deregister(sap_handle);
    qmi_core_server_unregister(me);
    qmi_core_server_delete(&me->core_object);
    PS_SYSTEM_HEAP_MEM_FREE(me);
    ASSERT(0);
    return rc;
  }

  return QMI_CORE_SERVER_NO_ERR;
}


qmi_core_server_error_type ds_qmi_otdt_send_ind
(
  void* handle,
  otdt_test_status_ind_msg_v01* response
)
{
  ds_qmi_otdt_type* me = 
    (ds_qmi_otdt_type*)handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == me) {
    LOG_MSG_ERROR_0("ds_qmi_otdt_send_ind(): "
                    "NULL handle passed!");
    return QMI_CORE_SERVER_INVALID_OBJECT;
  }

  return qmi_core_server_send_ind(&me->core_object,
    QMI_OTDT_TEST_STATUS_IND_V01,
    response,
    sizeof(otdt_test_status_ind_msg_v01));
}



