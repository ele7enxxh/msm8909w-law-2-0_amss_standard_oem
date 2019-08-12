/*!
   @file
   rf_qmi_rfrpe_svc.c

   @brief
   This file contains the function and service implementations for
   RF Tuner service
*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/qmi_svc/rfrpe/src/rf_qmi_rfrpe_svc.c#1 $

when         who   what, where, why
--------   ---   ------------------------------------------------------------------- 
11/20/14   aca   QMI service export to RFM_TP
09/19/14   ndb   KW warnings fix
04/09/13   vb    Enabled query commands/TLVs supported by service
03/15/13   vb    Reverted the changes for query commands as it was breaking OFT
03/15/13   pm/vb  Added support to query commands/TLVs supported by service
03/13/13   aro    Added RFRPE deinit to perform memory cleanup
02/04/13   aca   DIME update
10/16/12   gh     Fix race condition between task creation and QMI core registration
09/19/12   gh     Fixed populating req_handle and setting OEM string valid flag
                  Commented out F3 in event_loop
09/06/12   gh     Initial release

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
/* QMI related headers */
#include "qmi_core_server.h"
#include "qmi_sap.h"
#include "radio_frequency_radiated_performance_enhancement_v01.h"
#include "amssassert.h"

/* RFA headers */
#include "comdef.h"
#include "rf_qmi_rfrpe_svc.h"
#include "msg.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"
#include "rfc_card.h"

/* MSGR/Task Headers */
#include "rf_cmd_interface.h"
#include "rfm_rf_tuner_umid.h"
#include "msgr_types.h"

/*===========================================================================
                            FORWARD DECLARATIONS
===========================================================================*/
static qmi_dispatch_service_error_type rf_qmi_rfrpe_svc_req_set_scenario
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 );

static qmi_dispatch_service_error_type rf_qmi_rfrpe_svc_req_get_scenario
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 );

static qmi_dispatch_service_error_type rf_qmi_rfrpe_svc_req_get_revision
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 );

static qmi_csi_cb_error rf_qmi_rfrpe_svc_connect
(
  qmi_client_handle client_handle,
  void              *service_cookie,
  void              **connection_handle
);

static void rf_qmi_rfrpe_svc_disconnect
(
  void              *connection_handle,
  void              *service_cookie
 ) ;

static qmi_csi_cb_error rf_qmi_rfrpe_svc_process_req
(
  void              *connection_handle,
  qmi_req_handle    req_handle,
  unsigned int      msg_id,
  void              *req_c_struct,
  unsigned int      req_c_struct_len,
  void              *service_cookie
);


/*===========================================================================
                           DEFINITIONS
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Signal to wait while in event wait loop. This signal is defined here and can
  be updated. This wait sig is also updated to QMI core server as part of the
  core registration/initialization
*/
#define RF_QMI_RFRPE_SVC_WAIT_SIG (0x00001000)

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum number of services that RFRPE services that can be accomodated.
  At this point we limit the number of services to be only 1 based on current
  design.

  @warning
  Increasing this number is NOT STRAIGHTFORWARD. Will need to to find out
  RFRPE-daemon arbitration requirements if it is to come.
*/
#define QMI_NUM_RFRPE_SERVICES     (1)

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This struct keeps track of the clients that connects through connect callback.
*/
typedef  struct {
  /* First element of structure should be connection object */
  qmi_core_conn_obj_type        conn_obj;

  /* Indications for test status*/
  qmi_indication_type           test_status_ind;

  /* Rest of the client data goes here */

}rf_qmi_rfrpe_svc_client_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This struct contains params for a client handle
*/
typedef struct 
{
  /* First element of structure should be connection object */
  qmi_core_server_object_type  core_object;
  rf_qmi_rfrpe_svc_client_type client_data;
  boolean                      is_client_connected;
} rf_qmi_rfrpe_svc_client_handle_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This static var keeps track of scenario updates from QMI is to be ignored
*/
static boolean rf_qmi_rfrpe_svc_bypass_scenario_update = FALSE;


/*===========================================================================
                           GLOBAL VARS
===========================================================================*/
/* Dispatch Table for QMI server */
qmi_msg_handler_type rf_qmi_rfrpe_svc_cmd_dispatcher[] = 
{
  {QMI_RFRPE_SET_RFM_SCENARIO_REQ_V01,
   (qmi_dispatch_fn_type)rf_qmi_rfrpe_svc_req_set_scenario},
  {QMI_RFRPE_GET_RFM_SCENARIO_REQ_V01,
   (qmi_dispatch_fn_type)rf_qmi_rfrpe_svc_req_get_scenario},
  {QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_REQ_V01,
   (qmi_dispatch_fn_type)rf_qmi_rfrpe_svc_req_get_revision},
};

/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic event loop for the service

  @detail
  This routine will be used by QMI core as svc wait loop

  @param[in]  param   Client handle

  @retval    None
*/
static void rf_qmi_rfrpe_svc_event_loop(dword param)
{
  rex_sigs_type      wait_on,sig_received;
  qmi_csi_os_params  *os_params;
  rf_qmi_rfrpe_svc_client_handle_type  *me = (rf_qmi_rfrpe_svc_client_handle_type*)param;
  qmi_core_server_object_type *server = &(me->core_object);
	
  /* If Server is valid then register the new server */
  if( qmi_core_server_check_valid_object((void*)server) == QMI_CORE_SERVER_NO_ERR )
  {
    (void)qmi_core_server_register((void*)server,
                                   rfrpe_get_service_object_v01(),
                                   rf_qmi_rfrpe_svc_connect,
                                   rf_qmi_rfrpe_svc_disconnect,
                                   rf_qmi_rfrpe_svc_process_req);
  }
  else
  {
    RF_MSG(RF_ERROR, "rf_qmi_rfrpe_svc_event_loop: server invalid, no registration done!");
  }


  RF_MSG(RF_HIGH, "rf_qmi_rfrpe_svc_event_loop: event loop started");

  os_params = qmi_core_server_get_os_params(server);
  wait_on =  RF_QMI_RFRPE_SVC_WAIT_SIG;

  while (1) 
  {
    sig_received = rex_wait(wait_on);
    rex_clr_sigs(os_params->tcb, sig_received);

    /* RF_MSG(RF_HIGH, "rf_qmi_rfrpe_svc_event_loop: QMI request received. Dispatching event."); */

    qmi_core_server_handle_event(me);
  }
} /* rf_qmi_rfrpe_svc_event_loop */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal function that is part of the QMI dispatch functions
  Handles QMI request when a set_scenario cmd is received

  @details
  Internal function that is part of the QMI dispatch functions
  Handles QMI request when a set_scenario cmd is received
  Note that the only error check here is the number of scenarios.


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
static qmi_dispatch_service_error_type rf_qmi_rfrpe_svc_req_set_scenario
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 )
{
  rfrpe_set_scenario_req_v01* request = (rfrpe_set_scenario_req_v01*)req_c_struct;
  rfrpe_set_scenario_resp_v01 resp_msg;

  qmi_dispatch_service_error_type err_code = QMI_DISPATCH_SERVICE_NO_ERR;

  RF_MSG(RF_HIGH, "rf_qmi_rfrpe_svc_req_set_scenario: Processing QMI set_scenario req.");

  /* Check if bypass scenario updates is required */
  if(rf_qmi_rfrpe_svc_bypass_scenario_update)
  {
    RF_MSG(RF_MED, "rf_qmi_rfrpe_svc_req_set_scenario: Bypass this QMI scenario update.");

    /* Initialize response msg to all 0 */
    memset(&resp_msg, 0, sizeof(rfrpe_set_scenario_resp_v01));

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    
    return (qmi_dispatch_service_error_type)qmi_core_server_send_resp(req_handle,
                                                                      QMI_RFRPE_SET_RFM_SCENARIO_RESP_V01,
                                                                      &resp_msg,
                                                                      sizeof(rfrpe_set_scenario_resp_v01));
  } /* end if for bypass scenario update */

  /* Check if the scenario_len is valid */
  if((request->scenarios_len == 0) || 
     (request->scenarios_len > RFRPE_CONCURRENT_SCENARIOS_MAX_V01))
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_svc_req_set_scenario: Invalid scenario length %d!",
             request->scenarios_len);

    /* Initialize response msg to all 0 */
    memset(&resp_msg, 0, sizeof(rfrpe_set_scenario_resp_v01));

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;

    return (qmi_dispatch_service_error_type)qmi_core_server_send_resp(req_handle,
                                                                      QMI_RFRPE_SET_RFM_SCENARIO_RESP_V01,
                                                                      &resp_msg,
                                                                      sizeof(rfrpe_set_scenario_resp_v01));
  }

  /* If there are no errors then dispatch to rf command along with req_handle */
  rf_qmi_rfrpe_send_rf_task_set_scenario_cmd((void*)req_handle,
                                             request->scenarios_len,
                                             request->scenarios,
                                             RFRPE_CONCURRENT_SCENARIOS_MAX_V01);

  return err_code;
} /* rf_qmi_rfrpe_svc_req_set_scenario */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal function that is part of the QMI dispatch functions
  Handles QMI request when a get_scenario cmd is received

  @details
  Note that there are no req_msg associated to get_scenario

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
static qmi_dispatch_service_error_type rf_qmi_rfrpe_svc_req_get_scenario
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 )
{
  qmi_dispatch_service_error_type err_code = QMI_DISPATCH_SERVICE_NO_ERR;

  RF_MSG(RF_HIGH, "rf_qmi_rfrpe_svc_req_get_scenario: Processing QMI get_scenario req.");

  /* There are no error checks needed here. Send rf command along with req_handle */
  rf_qmi_rfrpe_send_rf_task_get_scenario_cmd((void*)req_handle);

  return err_code;
} /* rf_qmi_rfrpe_svc_req_get_scenario */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal function that is part of the QMI dispatch functions
  Handles QMI request when a get_revision cmd is received

  @details
  Note that there are no req_msg associated to get_revision

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
static qmi_dispatch_service_error_type rf_qmi_rfrpe_svc_req_get_revision
(
 void               *server_data,
 void               *conn_obj,
 qmi_req_handle     req_handle,
 uint32_t           msg_id,
 void               *req_c_struct,
 uint32_t           req_c_struct_len
 )
{
  qmi_dispatch_service_error_type err_code = QMI_DISPATCH_SERVICE_NO_ERR;

  RF_MSG(RF_HIGH, "rf_qmi_rfrpe_svc_req_get_revision: Processing QMI get_revision req.");

  /* There are no error checks needed here. Send rf command along with req_handle */
  rf_qmi_rfrpe_send_rf_task_get_revision_cmd((void*)req_handle);

  return err_code;
} /* rf_qmi_rfrpe_svc_req_get_revision */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal callback function for QCSI interface for connecting to server
*/
static qmi_csi_cb_error rf_qmi_rfrpe_svc_connect
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
)
{
  qmi_core_server_error_type rc = QMI_CORE_SERVER_NO_ERR;
  rf_qmi_rfrpe_svc_client_handle_type* me = (rf_qmi_rfrpe_svc_client_handle_type*)service_cookie;

  /* Check if service_cookie is NULL */
  if(me == NULL)
  {
    RF_MSG(RF_ERROR, "rf_qmi_rfrpe_svc_connect: service_cookie is NULL!");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Reject multiple connections for a single client */
  if (me->is_client_connected == TRUE)
  {
    RF_MSG(RF_ERROR, "rf_qmi_rfrpe_svc_connect: Client already connected!");
    return QMI_CSI_CB_CONN_REFUSED;
  }
  
  rc = qmi_core_server_register_client(&me->client_data.conn_obj,
                                       &me->core_object,
                                       client_handle,
                                       QMI_NUM_RFRPE_SERVICES,
                                       &me->client_data);

  /* Check if connection went through */
  if(rc != QMI_CORE_SERVER_NO_ERR)
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_svc_connect: qmi_core_server_register_client() returned %d",
             rc);
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* If all went well then set up connection params */
  me->is_client_connected = TRUE;
  *connection_handle = &me->client_data.conn_obj;
  return QMI_CSI_CB_NO_ERR;

} /* rf_qmi_rfrpe_svc_connect */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal callback function for QCSI interface for disconnecting from the
  server
*/
static void rf_qmi_rfrpe_svc_disconnect
(
  void                              *connection_handle,
  void                              *service_cookie
) 
{
  rf_qmi_rfrpe_svc_client_handle_type* me = 
    (rf_qmi_rfrpe_svc_client_handle_type*)service_cookie;

  RF_MSG_1(RF_HIGH, 
           "rf_qmi_rfrpe_svc_disconnect: client 0x%x",
           connection_handle);
  
  me->is_client_connected = FALSE;

} /* rf_qmi_rfrpe_svc_disconnect */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal callback function for QCSI interface for handling/dispatching
  client requests
*/
static qmi_csi_cb_error rf_qmi_rfrpe_svc_process_req
(
  void            *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void            *req_c_struct,
  unsigned int    req_c_struct_len,
  void            *service_cookie
)
{ 
  rf_qmi_rfrpe_svc_client_handle_type* me = 
    (rf_qmi_rfrpe_svc_client_handle_type*)service_cookie;
  qmi_core_server_error_type rc = QMI_CORE_SERVER_NO_ERR;

  RF_MSG_2(RF_HIGH,
           "rf_qmi_rfrpe_svc_process_req: client 0x%x msg %u",
           connection_handle, msg_id);

  rc = qmi_core_server_dispatch_msg((qmi_core_conn_obj_type *)connection_handle,
                                    me,
                                    req_handle,
                                    msg_id,
                                    req_c_struct,
                                    req_c_struct_len);

  /* Check if dispatch went through */
  if (QMI_CORE_SERVER_NO_ERR != rc)
  {
    RF_MSG(RF_ERROR, "rf_qmi_rfrpe_svc_process_req: dispatch_msg failed!");
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  
  return QMI_CSI_CB_NO_ERR;
} /* rf_qmi_rfrpe_svc_process_req */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal utility function to convert framework error code to QMI error codes

  @param framework_ec
  Error code type that is used between RFRPE SVC and tuner commonn framework

  @param result
  Error code type to convert to based on framework_ec. This error code is used
  between RFRPE SVC and QMI core server. Values are either success or failure

  @param error
  Error code type to convert to based on framework_ec. This error code is used
  between RFRPE SVC and QMI core server. This error code is exhaustive with
  members such as no_error, internal_failure, info_unavailable...
*/
static void rf_qmi_rfrpe_svc_convert_resp_error_code
(
 rf_qmi_rfrpe_resp_err_code_type framework_ec,
 qmi_result_type_v01* result,
 qmi_error_type_v01* error
)
{
  switch(framework_ec)
  {
  case RF_QMI_RFRPE_RESP_ERR_CODE_SUCCESS:
    *result = QMI_RESULT_SUCCESS_V01;
    *error = QMI_ERR_NONE_V01;
    break;

  case RF_QMI_RFRPE_RESP_ERR_CODE_INTERNAL_ERROR:
    *result = QMI_RESULT_FAILURE_V01;
    *error = QMI_ERR_INTERNAL_V01;
    break;

  case RF_QMI_RFRPE_RESP_ERR_CODE_BAD_PARAMETERS:
    *result = QMI_RESULT_FAILURE_V01;
    *error = QMI_ERR_INVALID_ARG_V01;
    break;

  case RF_QMI_RFRPE_RESP_ERR_CODE_INVALID_SCENARIO_NUMBER:
    *result = QMI_RESULT_FAILURE_V01;
    *error = QMI_ERR_INVALID_ARG_V01;
    break;

  case RF_QMI_RFRPE_RESP_ERR_CODE_CURRENT_SCENARIO_NOT_AVAILABLE:
    *result = QMI_RESULT_FAILURE_V01;
    *error = QMI_ERR_INFO_UNAVAILABLE_V01;
    break;

  case RF_QMI_RFRPE_RESP_ERR_CODE_REVISION_NOT_AVAILABLE:
    *result = QMI_RESULT_FAILURE_V01;
    *error = QMI_ERR_INFO_UNAVAILABLE_V01;
    break;

  case RF_QMI_RFRPE_RESP_ERR_CODE_OEM_STRING_NOT_AVAILABLE:
    *result = QMI_RESULT_FAILURE_V01;
    *error = QMI_ERR_INFO_UNAVAILABLE_V01;
    break;

  default:
    *result = QMI_RESULT_FAILURE_V01;
    *error = QMI_ERR_UNKNOWN_V01;
    ;
  } /* end switch for framework errors */

} /* rf_qmi_rfrpe_svc_convert_resp_error_code */



/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  External function for rf_qmi_rfrpe_svc initialization

  @detail
  This API is to be called once to register the RFRPE SVC components to 
  the QMI core and start the RFRPE QCSI SVC. This API must be called
  after RFC initialization is done.

  @Note
  The return type is defined as uint8 instead of qmi_core_server_error_type
  such that it can be generalized for inclusion to rfm/mc/core without having
  to include the QMI headers
*/
static rf_qmi_rfrpe_svc_client_handle_type*  rfrpe_client_handle = NULL;
uint8 rf_qmi_rfrpe_svc_init()
{
  static boolean                               rf_qmi_rfrpe_svc_started = FALSE;
  qmi_core_server_error_type                   rc = QMI_CORE_SERVER_NO_ERR;
  uint32_t                                     priority = 14;// TODO: Decide the proper priority for service
  unsigned long                                sig = RF_QMI_RFRPE_SVC_WAIT_SIG;
  qmi_sap_error                                sap_rc = QMI_SAP_NO_ERR;
  qmi_sap_client_handle                        sap_handle;

  /* Check if server has started */
  if(rf_qmi_rfrpe_svc_started)
  {
    RF_MSG(RF_HIGH, "rf_qmi_rfrpe_svc_init: Server has already started.");
    return QMI_CORE_SERVER_NO_ERR;
  }

  /* Start server initialization */
  RF_MSG(RF_HIGH, "rf_qmi_rfrpe_svc_init: svc_init starting.");

  rfrpe_client_handle = (rf_qmi_rfrpe_svc_client_handle_type*)
    modem_mem_alloc(sizeof(rf_qmi_rfrpe_svc_client_handle_type), MODEM_MEM_CLIENT_RFA);

  if(rfrpe_client_handle == NULL)
  {
    RF_MSG(RF_ERROR, "rf_qmi_rfrpe_svc_init: mem alloc failed!");
    return (uint8)QMI_CORE_SERVER_MEMORY_ERR;
  }

  /* Instantiate new server */
  rc = qmi_core_server_new(&rfrpe_client_handle->core_object,
                           "RFRPE",
                           1, /* instance id */
                           1, /* new task for service */
                           (void*)rf_qmi_rfrpe_svc_event_loop,
                           (void*)&priority,
                           NULL,
                           (void*)&sig,
                           NULL,
                           rf_qmi_rfrpe_svc_cmd_dispatcher,
                           sizeof(rf_qmi_rfrpe_svc_cmd_dispatcher)/sizeof(qmi_msg_handler_type));
 
  /* Check if there are problems instantiating the server */
  if(rc != QMI_CORE_SERVER_NO_ERR)
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_svc_init: qmi_core_server_new() returned %d",
             rc);

    modem_mem_free(rfrpe_client_handle, MODEM_MEM_CLIENT_RFA);
    ASSERT(0);
    return (uint8)rc;
  }

  /* Notes: Current RFRPE design doees not incorporate any usages of
            indicators. If there should be in the future then add
            indication initialization here via
            qmi_indication_initialize()
  */
  
  /* Check if there are registration errors */
  if(rc != QMI_CORE_SERVER_NO_ERR)
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_svc_init: qmi_core_server_register() returned %d", rc);
    qmi_core_server_delete(&rfrpe_client_handle->core_object);
    modem_mem_free(rfrpe_client_handle, MODEM_MEM_CLIENT_RFA);
    ASSERT(0);
    return (uint8)rc;
  }

  /* Register SAP */
  sap_rc = qmi_sap_register(rfrpe_get_service_object_v01(),
                            NULL,
                            &sap_handle);

  /* Check if there are SAP registration errors */
  if(sap_rc != QMI_SAP_NO_ERR)
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_svc_init: qmi_sap_register() returned %d", sap_rc);
    rc = QMI_CORE_SERVER_INTERNAL_ERR;
    qmi_core_server_unregister(rfrpe_client_handle);
    qmi_core_server_delete(&rfrpe_client_handle->core_object);
    modem_mem_free(rfrpe_client_handle, MODEM_MEM_CLIENT_RFA);
    ASSERT(0);
    return (uint8)rc;
  }

  /* Start server */
  rc = qmi_core_server_start_server(rfrpe_client_handle);

  /* Check if there are errors starting server */
  if(rc != QMI_CORE_SERVER_NO_ERR)
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_svc_init: qmi_core_server_start_server() returned %d", rc);
    qmi_sap_deregister(sap_handle);
    qmi_core_server_unregister((void*)&(rfrpe_client_handle->core_object));
    qmi_core_server_delete(&rfrpe_client_handle->core_object);
    modem_mem_free(rfrpe_client_handle, MODEM_MEM_CLIENT_RFA);
    ASSERT(0);
    return (uint8)rc;
  }

  /* Set indication that RFRPE server has started */
  rf_qmi_rfrpe_svc_started = TRUE;

  return (uint8)QMI_CORE_SERVER_NO_ERR;
} /* rf_qmi_rfrpe_svc_init */

uint8 rf_qmi_rfrpe_svc_deinit()
{
  if ( rfrpe_client_handle != NULL )
  {
    modem_mem_free(rfrpe_client_handle, MODEM_MEM_CLIENT_RFA);
  }
  return 0;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  External function for RFRPE server to send set_scenario cmd to rf_task

  @param req_handle
  void pointer that holds the req_handler info of the requestor.

  @param scenario_len
  Number of scenarios to program

  @param scenarios
  Array of scenario values to be programmed

  @para max_scenario_len
  Max number of scenarios allowed to be programmed
*/
void rf_qmi_rfrpe_send_rf_task_set_scenario_cmd
(
 void*   req_handle,
 uint32  scenario_len,
 uint32* scenarios,
 uint32  max_scenario_len
)
{
  rf_cmd_type* rf_cmd_buf = NULL;
  rf_qmi_rfrpe_svc_set_scenario_req_s* rf_cmd_buf_payload = NULL;
  uint8 counter = 0;
  rfrpe_set_scenario_resp_v01 resp_msg;

  /* Check if scenario_len is valid  */
  if((scenario_len == 0) || (scenario_len > max_scenario_len))
  {
    /* Initialize response msg to all 0 */
    memset(&resp_msg, 0, sizeof(rfrpe_set_scenario_resp_v01));

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;

    RF_MSG_2(RF_ERROR,
             "rf_qmi_rfrpe_send_rf_task_set_scenario_cmd: scenario_len %d > max %d!",
             scenario_len,
             max_scenario_len);

    /* Send back response with error */
    qmi_core_server_send_resp(req_handle,
                              QMI_RFRPE_SET_RFM_SCENARIO_RESP_V01,
                              &resp_msg,
                              sizeof(rfrpe_set_scenario_resp_v01));

    return;
  }

  /* Get a cmd_buf for rf_task */
  rf_cmd_buf = rf_cmd_get_buf( RF_TASK_MAIN);

  /* Check if cmd buf had been returned properly */
  if(rf_cmd_buf == NULL)
  {
    RF_MSG(RF_ERROR, "rf_qmi_rfrpe_send_rf_task_set_scenario_cmd: cmd_get_buf failed!");
    return;
  }


  /* If error check went fine then populate cmd buf headers */
  rf_cmd_buf->msgr_hdr.id = RFA_RF_TUNER_SET_SCENARIO_CMD;
  rf_cmd_buf->rex_hdr.cmd_hdr.tech_id = MSGR_TECH_RFA;
  rf_cmd_buf->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_TUNER;
  rf_cmd_buf->rex_hdr.cmd_hdr.cmd_id = RFM_RF_TUNER_SET_SCENARIO_CMD;

  /* Populate the payload
     Note that max allowable payload size for a rf_cmd_buf is only 72 bytes
     as defined by RF_MAX_CMD_PAYLOAD_SIZE.
     The way the IDL had designed using uint32 will overflow so will need to down
     convert to using uint8 types. See rf_qmi_rfrpe_svc_set_scenario_req_s
  */
  rf_cmd_buf_payload = (rf_qmi_rfrpe_svc_set_scenario_req_s*)rf_cmd_buf->payload;
  rf_cmd_buf_payload->qmi_req_handle = req_handle;
  rf_cmd_buf_payload->scenario_len = (uint8)scenario_len;
  for(counter = 0; counter < scenario_len; counter++)
  {
    rf_cmd_buf_payload->scenarios[counter] = (uint8)scenarios[counter];
  }

  /* Put cmd buf to RF cmd Q */
  rf_cmd_put( RF_TASK_MAIN, rf_cmd_buf);

} /* rf_qmi_rfrpe_send_rf_task_set_scenario_cmd */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  External function for RFRPE server to send get_scenario cmd to rf_task

  @param req_handle
  void pointer that holds the req_handler info of the requestor.
*/
void rf_qmi_rfrpe_send_rf_task_get_scenario_cmd(void* req_handle)
{
  rf_cmd_type* rf_cmd_buf = NULL;
  rf_qmi_rfrpe_svc_get_scenario_req_s* rf_cmd_buf_payload = NULL;

  /* Get a cmd_buf for rf_task */
  rf_cmd_buf = rf_cmd_get_buf( RF_TASK_MAIN);

  /* Check if cmd buf had been returned properly */
  if(rf_cmd_buf == NULL)
  {
    RF_MSG(RF_ERROR, "rf_qmi_rfrpe_send_rf_task_get_scenario_cmd: cmd_get_buf failed!");
    return;
  }

  /* If cmd_buf is valid then populate cmd buf headers */
  rf_cmd_buf->msgr_hdr.id = RFA_RF_TUNER_GET_SCENARIO_CMD;
  rf_cmd_buf->rex_hdr.cmd_hdr.tech_id = MSGR_TECH_RFA;
  rf_cmd_buf->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_TUNER;
  rf_cmd_buf->rex_hdr.cmd_hdr.cmd_id = RFM_RF_TUNER_GET_SCENARIO_CMD;

  /* Populate the payload */
  rf_cmd_buf_payload = (rf_qmi_rfrpe_svc_get_scenario_req_s*)rf_cmd_buf->payload;
  rf_cmd_buf_payload->qmi_req_handle = req_handle;

  /* Put cmd buf to RF cmd Q */
  rf_cmd_put( RF_TASK_MAIN, rf_cmd_buf);

} /* rf_qmi_rfrpe_send_rf_task_get_scenario_cmd */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  External function for RFRPE server to send get_revision cmd to rf_task

  @param req_handle
  void pointer that holds the req_handler info of the requestor.
*/
void rf_qmi_rfrpe_send_rf_task_get_revision_cmd(void* req_handle)
{
  rf_cmd_type* rf_cmd_buf = NULL;
  rf_qmi_rfrpe_svc_get_revision_req_s* rf_cmd_buf_payload = NULL;

  /* Get a cmd_buf for rf_task */
  rf_cmd_buf = rf_cmd_get_buf( RF_TASK_MAIN);

  /* Check if cmd buf had been returned properly */
  if(rf_cmd_buf == NULL)
  {
    RF_MSG(RF_ERROR, "rf_qmi_rfrpe_send_rf_task_get_scenario_cmd: cmd_get_buf failed!");
    return;
  }

  /* If cmd_buf is valid then populate cmd buf headers */
  rf_cmd_buf->msgr_hdr.id = RFA_RF_TUNER_GET_REVISION_CMD;
  rf_cmd_buf->rex_hdr.cmd_hdr.tech_id = MSGR_TECH_RFA;
  rf_cmd_buf->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_TUNER;
  rf_cmd_buf->rex_hdr.cmd_hdr.cmd_id = RFM_RF_TUNER_GET_REVISION_CMD;

  /* Populate the payload */
  rf_cmd_buf_payload = (rf_qmi_rfrpe_svc_get_revision_req_s*)rf_cmd_buf->payload;
  rf_cmd_buf_payload->qmi_req_handle = req_handle;

  /* Put cmd buf to RF cmd Q */
  rf_cmd_put( RF_TASK_MAIN, rf_cmd_buf);

} /* rf_qmi_rfrpe_send_rf_task_get_revision_cmd */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  External function for to send response associated to set_scenario operation
  to the RFRPE server

  @param req_handle
  void pointer that holds the req_handler info of the requestor.

  @param err_code
  Error code of rf_qmi_rfrpe_resp_err_code_type that indicates errors occured
  within the tuner common framework

  @return TRUE if operations went fine. FALSE otherwise
*/
boolean rf_qmi_rfrpe_set_scenarios_resp
(
 void *qmi_handle,
 rf_qmi_rfrpe_resp_err_code_type err_code
)
{
  qmi_dispatch_service_error_type dispatch_error = QMI_DISPATCH_SERVICE_NO_ERR;
  rfrpe_set_scenario_resp_v01 resp_msg;
  qmi_req_handle req_handle = (qmi_req_handle)qmi_handle;

  /* Check if qmi_handle is valid. 0 is not a valid req_handle */
  if(qmi_handle == NULL)
  {
    RF_MSG(RF_ERROR,
           "rf_qmi_rfrpe_set_scenarios_resp: qmi_handle is NULL! Response not sent!");
    return FALSE;
  }

  /* Check if a response is needed */
  if(*((uint16*)qmi_handle) == RF_QMI_RFRPE_RSP_NOT_NEEDED)
    return TRUE;
    
  /* Clear the response message */
  memset(&resp_msg, 0, sizeof(rfrpe_set_scenario_resp_v01));

  /* Convert and populate QMI error codes */
  rf_qmi_rfrpe_svc_convert_resp_error_code(err_code,
                                           &resp_msg.resp.result,
                                           &resp_msg.resp.error);
  
  /* Send set_scenario response */
  dispatch_error = (qmi_dispatch_service_error_type)qmi_core_server_send_resp(req_handle,
                                                                              QMI_RFRPE_SET_RFM_SCENARIO_RESP_V01,
                                                                              &resp_msg,
                                                                              sizeof(rfrpe_set_scenario_resp_v01));

  /* Check if response dispatch went through */
  if(dispatch_error != QMI_DISPATCH_SERVICE_NO_ERR)
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_set_scenarios_resp: Send set_scenario response failed! Error code %d",
             dispatch_error);
    return FALSE;
  }

  RF_MSG(RF_MED, "rf_qmi_rfrpe_set_scenarios_resp: set_scenario response sent.");
  return TRUE;
} /* rf_qmi_rfrpe_set_scenarios_resp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  External function for to send response associated to get_scenario operation
  to the RFRPE server

  @param req_handle
  void pointer that holds the req_handler info of the requestor.

  @param num_of_scenarios
  Number of currently active scenarios on tuner manager

  @param scenario_vals
  List of currently active scenarios on tuner manager

  @param err_code
  Error code of rf_qmi_rfrpe_resp_err_code_type that indicates errors occured
  within the tuner common framework

  @return TRUE if operations went fine. FALSE otherwise
*/
boolean rf_qmi_rfrpe_get_scenarios_resp
(
 void *qmi_handle,
 uint32 num_of_scenarios,
 uint32 *scenario_vals,
 rf_qmi_rfrpe_resp_err_code_type err_code
)
{
  qmi_dispatch_service_error_type dispatch_error = QMI_DISPATCH_SERVICE_NO_ERR;
  rfrpe_get_rfm_scenarios_resp_v01 resp_msg;
  qmi_req_handle req_handle = (qmi_req_handle)qmi_handle;
  uint8 counter = 0;

  /* Check if qmi_handle is valid. 0 is not a valid req_handle */
  if(qmi_handle == NULL)
  {
    RF_MSG(RF_ERROR,
           "rf_qmi_rfrpe_get_scenarios_resp: qmi_handle is NULL! Response not sent!");
    return FALSE;
  }

  /* Clear the response message */
  memset(&resp_msg, 0, sizeof(rfrpe_get_rfm_scenarios_resp_v01));

  /* Convert and populate QMI error codes */
  rf_qmi_rfrpe_svc_convert_resp_error_code(err_code,
                                           &resp_msg.resp.result,
                                           &resp_msg.resp.error);

  /* Check if num_of_scenarios is valid */
  if((num_of_scenarios == 0) || (num_of_scenarios > RFRPE_CONCURRENT_SCENARIOS_MAX_V01))
  {
    resp_msg.active_scenarios_valid = FALSE;
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_get_scenarios_resp: Invalid num_of_scenarios %d!",
             num_of_scenarios);
  }
  /* NULL check on scenario_vals */
  else if(scenario_vals == NULL)
  {
    resp_msg.active_scenarios_valid = FALSE;
    RF_MSG(RF_ERROR,
           "rf_qmi_rfrpe_get_scenarios_resp: scenarios array is NULL!");
  }
  /* If num_of_scenarios is valid then copy over the scenarios */
  else
  {
    resp_msg.active_scenarios_valid = TRUE;
    resp_msg.active_scenarios_len = num_of_scenarios;
    /* Copy the values */
    for(counter = 0; counter < num_of_scenarios; counter++)
    {
      resp_msg.active_scenarios[counter] = scenario_vals[counter];
    }
  }

  /* Send set_scenario response */
  dispatch_error = (qmi_dispatch_service_error_type)qmi_core_server_send_resp(req_handle,
                                                                              QMI_RFRPE_GET_RFM_SCENARIO_RESP_V01,
                                                                              &resp_msg,
                                                                              sizeof(rfrpe_get_rfm_scenarios_resp_v01));

  /* Check if response dispatch went through */
  if(dispatch_error != QMI_DISPATCH_SERVICE_NO_ERR)
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_get_scenarios_resp: Send set_scenario response failed! Error code %d",
             dispatch_error);
    return FALSE;
  }

  RF_MSG(RF_MED, "rf_qmi_rfrpe_get_scenarios_resp: get_scenario response sent.");
  return TRUE;
} /* rf_qmi_rfrpe_get_scenarios_resp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  External function for to send response associated to get_revision operation
  to the RFRPE server

  @param revision_num
  32-bit revision number

  @param oem_name_len
  Length of the OEM name/string

  @param oem_name
  OEM name/string

  @param err_code
  Error code of rf_qmi_rfrpe_resp_err_code_type that indicates errors occured
  within the tuner common framework

  @return TRUE if operations went fine. FALSE otherwise
*/
boolean rf_qmi_rfrpe_get_provisioned_table_info_resp
(
 void *qmi_handle,
 uint32 revision_num,
 uint32 oem_name_len,
 uint16 *oem_name,
 rf_qmi_rfrpe_resp_err_code_type err_code
)
{
  qmi_dispatch_service_error_type dispatch_error = QMI_DISPATCH_SERVICE_NO_ERR;
  rfrpe_get_provisioned_table_revision_resp_v01 resp_msg;
  qmi_req_handle req_handle = (qmi_req_handle)qmi_handle;
  uint32 counter = 0;

  /* Check if qmi_handle is valid. 0 is not a valid req_handle */
  if(qmi_handle == NULL)
  {
    RF_MSG(RF_ERROR,
           "rf_qmi_rfrpe_get_provisioned_table_info_resp: qmi_handle is NULL! Response not sent!");
    return FALSE;
  }
  
  /* Clear the response message */
  memset(&resp_msg, 0, sizeof(rfrpe_get_provisioned_table_revision_resp_v01));

  /* Convert and populate QMI error codes */
  rf_qmi_rfrpe_svc_convert_resp_error_code(err_code,
                                           &resp_msg.resp.result,
                                           &resp_msg.resp.error);

  /* Populate revision */
  resp_msg.provisioned_table_revision_valid = TRUE;
  resp_msg.provisioned_table_revision = revision_num;

  /* Error check and populate OEM string */
  if(oem_name == NULL)
  {
    resp_msg.provisioned_table_OEM_valid = FALSE;
    RF_MSG(RF_ERROR,
           "rf_qmi_rfrpe_get_provisioned_table_info_resp: NULL OEM string!");
  }
  else if(oem_name_len > RFRPE_OEM_STR_LENGTH_V01)
  {
    resp_msg.provisioned_table_OEM_valid = FALSE;
    RF_MSG_2(RF_ERROR,
             "rf_qmi_rfrpe_get_provisioned_table_info_resp: String length %d larger than max %d!",
             oem_name_len, RFRPE_OEM_STR_LENGTH_V01);
  }
  else
  {
    resp_msg.provisioned_table_OEM_valid = TRUE;
    resp_msg.provisioned_table_OEM_len = oem_name_len;
    
    /* copy over the OEM string */
    for(counter = 0; counter < oem_name_len; counter++)
    {
      resp_msg.provisioned_table_OEM[counter] = oem_name[counter];
    }
  } /* end else for populating OEM string */

  /* Send set_scenario response */
  dispatch_error = (qmi_dispatch_service_error_type)qmi_core_server_send_resp(req_handle,
                                                                              QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_RESP_V01,
                                                                              &resp_msg,
                                                                              sizeof(rfrpe_get_provisioned_table_revision_resp_v01));

  /* Check if response dispatch went through */
  if(dispatch_error != QMI_DISPATCH_SERVICE_NO_ERR)
  {
    RF_MSG_1(RF_ERROR,
             "rf_qmi_rfrpe_get_provisioned_table_info_resp: Send set_scenario response failed! Error code %d",
             dispatch_error);
    return FALSE;
  }

  RF_MSG(RF_MED, "rf_qmi_rfrpe_get_provisioned_table_info_resp: get_revision response sent.");
  return TRUE;
} /* rf_qmi_rfrpe_get_provisioned_table_info_resp */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  External function to disable/bypass scenario updates coming from QMI side

  @param disable_scenario_update
  Set to TRUE if the disable/byoass scenario updates from QMI is desired.
  Else set FALSE.
*/
void rf_qmi_rfrpe_svc_disable_scenario_update(boolean disable_scenario_update)
{
  rf_qmi_rfrpe_svc_bypass_scenario_update = disable_scenario_update;

  if(disable_scenario_update)
  {
    RF_MSG(RF_HIGH,
           "rf_qmi_rfrpe_svc_disable_scenario_update: QMI scenario updates will be bypassed");
  }
  else
  {
    RF_MSG(RF_HIGH,
           "rf_qmi_rfrpe_svc_disable_scenario_update: QMI scenario updates will be applied");
  }

} /* rf_qmi_rfrpe_svc_disable_scenario_update */

