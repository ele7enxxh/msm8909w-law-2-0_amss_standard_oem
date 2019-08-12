/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            Q M I    U I M    R E M O T E    S U B S Y S T E M


GENERAL DESCRIPTION

  This source file contains the supporting functions to access the QMI UIM
  remote service

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_uim_remote.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/16   ar      Invalidate the slot on receiving conn_unavail req
08/01/16   ks      Return error to client when card service is not found
05/17/16   bcho    Deregister qmi uim remote service on task stop
03/28/16   vr      Initialize the client_register slot value to invalid
06/05/14   tkl     Add Vcc and response_timeout for power up request
05/06/14   tkl     Fix KW errors
03/06/14   tkl     Add power down mode for power down request
01/28/14   df      Use q_cnt() instead directly accessing cnt
01/10/14   df      Use v2.0 diag macros
12/11/13   vs      Addition of UIMQMI task and QMI UIM REMOTE

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#ifdef FEATURE_UIM_REMOTE_UIM
#include "rex.h"
#include "modem_mem.h"
#include "queue.h"
#include "uim_msg.h"

#include "uimqmi_task.h"
#include "qmi_uim_remote.h"

#include "qmi_csi.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "user_identity_module_remote_v01.h"
#include "ipc_policy.h"

#include <stringl/stringl.h>

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* Max requests that can be accomodated by queue */
#define QMI_UIM_REMOTE_CMD_QUEUE_MAX_SIZE  10

qmi_uim_remote_state_type * qmi_uim_remote_state_ptr = NULL;

/*===========================================================================

                         REQUEST HANDLER FUNCTIONS

===========================================================================*/
/* Request handler fucntion prototype */
typedef qmi_csi_error (* const qmi_uim_remote_req_handler_type)
(void                * conn_handle,
 qmi_req_handle        req_handle,
 void                * req_c_struct,
 int                   req_c_struct_len);

static qmi_csi_error qmi_uim_remote_reset_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
);

static qmi_csi_error qmi_uim_remote_event_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
);

static qmi_csi_error qmi_uim_remote_apdu_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
);

/* Table to handle the dispatch of request handler functions. */
static qmi_uim_remote_req_handler_type qmi_uim_remote_req_handler_table[] =
{
  qmi_uim_remote_reset_req_handler,            /* Request handler for message ID 0x20 */
  qmi_uim_remote_event_req_handler,            /* Request handler for message ID 0x21 */
  qmi_uim_remote_apdu_req_handler              /* Request handler for message ID 0x22 */
};


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_GLOBALS_INIT

   DESCRIPTION:
     This function initializes the globals associated with QMI UIM REMOTE

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void qmi_uim_remote_globals_init (
  void
)
{
  if (qmi_uim_remote_state_ptr != NULL)
  {
    return;
  }

  qmi_uim_remote_state_ptr =
    modem_mem_calloc(1, sizeof(qmi_uim_remote_state_type), MODEM_MEM_CLIENT_UIM);

  if (qmi_uim_remote_state_ptr == NULL)
  {
    UIM_MSG_ERR_0("QMI UIM Remote init failed");
    return;
  }

  (void) q_init(&qmi_uim_remote_state_ptr->cmd_q);
} /* qmi_uim_remote_globals_init */


/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_MAP_CSI_TO_CSI_CB_ERR

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
static qmi_csi_cb_error qmi_uim_remote_map_csi_to_csi_cb_err (
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
      return QMI_CSI_CB_UNSUPPORTED_ERR;
  }
} /* qmi_uim_remote_map_csi_to_csi_cb_err */


/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_CONNECT_CB

  DESCRIPTION
    Connect callback function registered with the QCSI to be invoked when
    client sends the first request message to be service.

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
static qmi_csi_cb_error qmi_uim_remote_connect_cb (
  qmi_client_handle   cli_handle,
  void               *service_cookie,
  void              **connection_handle
)
{
  uint8  index = 0;

  /* there is no need to store service specific context information */
  (void) service_cookie;

  UIM_MSG_HIGH_0("qmi_uim_remote_connect_cb");

  if(connection_handle == NULL)
  {
    UIM_MSG_ERR_0("Invalid connection handle");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  if (qmi_uim_remote_state_ptr == NULL)
  {
    UIM_MSG_LOW_0("Connect received when qmi uim remote not init");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  for (index=0; index < QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT; index++)
  {
    if (qmi_uim_remote_state_ptr->card_client_registry[index].in_use == FALSE)
    {
      qmi_uim_remote_state_ptr->card_client_registry[index].in_use         = TRUE;
      qmi_uim_remote_state_ptr->card_client_registry[index].client_handle  = cli_handle;
      /* Initialize slot to invalid value will updated in conn available event */
      qmi_uim_remote_state_ptr->card_client_registry[index].slot           = 
                                                    QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;
      *connection_handle = &(qmi_uim_remote_state_ptr->card_client_registry[index]);
      break;
    }
  }

  if (index >= QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT)
  {
    UIM_MSG_LOW_0("qmi_uim_remote_connect_cb: No table entry availbale for new client");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  return QMI_CSI_CB_NO_ERR;
} /* qmi_uim_remote_connect_cb */


/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_DISCONNECT_CB

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
static void qmi_uim_remote_disconnect_cb (
  void    * conn_handle,
  void    * service_cookie
)
{
  uint8                                      card_client_index      = 0;
  uint8                                      card_service_index     = 0;
  qmi_uim_remote_card_client_info_type     * card_client_handle_ptr = NULL;
  qmi_uim_remote_slot_type                   slot                   = QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;
  qmi_uim_remote_event_data_type             evt;

  (void)service_cookie;

  if (conn_handle == NULL)
  {
    return;
  }

  card_client_handle_ptr = (qmi_uim_remote_card_client_info_type *)conn_handle;

  UIM_MSG_HIGH_0("qmi_uim_remote_disconnect_cb");

  /* Find the client that has disconnected */
  for (card_client_index = 0;
       card_client_index < QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT;
       card_client_index++)
  {
    if ((qmi_uim_remote_state_ptr->card_client_registry[card_client_index].in_use) &&
        (qmi_uim_remote_state_ptr->card_client_registry[card_client_index].client_handle ==
           card_client_handle_ptr->client_handle))
    {
      slot = qmi_uim_remote_state_ptr->card_client_registry[card_client_index].slot;
      break;
    }
  }

  if (card_client_index == QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT)
  {
    UIM_MSG_LOW_0("Disconnect received but no card client active");
    return;
  }

  /* Check if a card service has previously connected to that card client */
  for (card_service_index = 0;
       card_service_index < QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE;
       card_service_index++)
  {
    if (qmi_uim_remote_state_ptr->card_service_registry[card_service_index].in_use &&
        qmi_uim_remote_state_ptr->card_service_registry[card_service_index].slot ==
        slot)
    {
      break;
    }
  }

  /* If a card service had previously connected to that client and the last event
     was not a connection unavailable event then send the card service a
     connection unavailable event */
  if (card_service_index < QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE &&
      qmi_uim_remote_state_ptr->card_client_registry[card_client_index].last_evt_data.evt >
        QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE)
  {
    memset(&evt, 0x0, sizeof(qmi_uim_remote_event_data_type));
    evt.slot = slot;
    evt.evt  = QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE;
    (qmi_uim_remote_state_ptr->card_service_registry[card_service_index].event_cb_ptr)(&evt);
  }

  /* Clear the card client entry including any pending APDU info */
  memset(&(qmi_uim_remote_state_ptr->card_client_registry[card_client_index]),
         0x0,
         sizeof(qmi_uim_remote_card_client_info_type));
  /* Reset slot to invalid value */
  qmi_uim_remote_state_ptr->card_client_registry[card_client_index].slot =
                                                QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;
} /* qmi_uim_remote_disconnect_cb */


/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_HANDLE_REQ_CB

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
static qmi_csi_cb_error qmi_uim_remote_handle_req_cb (
  void              * connection_handle,
  qmi_req_handle      req_handle,
  unsigned int        msg_id,
  void              * req_struct,
  unsigned int        req_struct_len,
  void              * service_cookie
)
{
  qmi_csi_error   status    = QMI_CSI_NO_ERR;
  uint8           msg_index = 0;

  (void) service_cookie;

  if(connection_handle == NULL)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  UIM_MSG_HIGH_1("qmi_uim_remote_handle_req_cb with msg_id 0x%x", msg_id);

  /* Verify that the msg_id received is within supported range and find
     appropriate request handler to process the request */
  if(msg_id >= QMI_UIM_REMOTE_RESET_REQ_V01)
  {
    msg_index = (uint8) msg_id - QMI_UIM_REMOTE_RESET_REQ_V01;
    if (msg_index < (sizeof(qmi_uim_remote_req_handler_table) / sizeof(qmi_uim_remote_req_handler_type)))
    {
      if(qmi_uim_remote_req_handler_table[msg_index])
      {
        status = qmi_uim_remote_req_handler_table[msg_index](connection_handle,
                                                             req_handle,
                                                             req_struct,
                                                             req_struct_len);
      }
    }
  }
  return qmi_uim_remote_map_csi_to_csi_cb_err(status);
} /* qmi_uim_remote_handle_req_cb */


/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_SERVICE_INIT

  DESCRIPTION
    Called to start the QMI UIM Remote service when the QMI modem task is up

  PARAMETERS
    TCB and SIG

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_remote_service_init (
  rex_tcb_type    * tcb_ptr,
  rex_sigs_type     sig
)
{
  qmi_csi_error                 err          =  QMI_CSI_NO_ERR;
  qmi_sap_error                 sap_err      =  QMI_SAP_NO_ERR;
  qmi_idl_service_object_type   service_obj  =  uim_remote_get_service_object_v01();
  qmi_csi_os_params             os_params;
  qmi_csi_options               options;

  UIM_MSG_HIGH_0("qmi_uim_remote_service_init");

  memset(&os_params, 0, sizeof(qmi_csi_os_params));

  os_params.tcb = tcb_ptr;
  os_params.sig = sig;

  if (service_obj == NULL || qmi_uim_remote_state_ptr == NULL)
  {
    UIM_MSG_ERR_1("Cannot initialize QMI UIM Remote service obj 0x%x",
                  service_obj);
    return;
  }

  QMI_CSI_OPTIONS_INIT(options);
  QMI_CSI_OPTIONS_SET_SCOPE(options, IPC_POLICY_SCOPE_OPEN);

  err = qmi_csi_register_with_options (service_obj,
                                       qmi_uim_remote_connect_cb,
                                       qmi_uim_remote_disconnect_cb,
                                       qmi_uim_remote_handle_req_cb,
                                       NULL,
                                       &os_params,
                                       &options,
                                       &(qmi_uim_remote_state_ptr->service_handle));

  if (err != QMI_CSI_NO_ERR || qmi_uim_remote_state_ptr->service_handle == NULL)
  {
    UIM_MSG_ERR_1("QMI CSI register failed, error=%d", err);
    return;
  }

  /* SAP registration necessary for native QMI clients to access the service
     and to test using QMI Test Pro */
  sap_err = qmi_sap_register(service_obj, NULL, &(qmi_uim_remote_state_ptr->sap_handle));
  if (sap_err != QMI_SAP_NO_ERR)
  {
    UIM_MSG_ERR_1("QMI SAP register failed, error=%d", sap_err);
  }
} /* qmi_uim_remote_service_init */


/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_HANDLE_QCSI_SIG

  DESCRIPTION
    Function is called when a QCSI signal is received by the QMI UIM REMOTE
    service. It will result in the calling of one of the QMI UIM REMOTE
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
void qmi_uim_remote_handle_qcsi_sig (
   rex_tcb_type *tcb, rex_sigs_type sig
)
{
  qmi_csi_error err;
  qmi_csi_os_params os_params;

  if (qmi_uim_remote_state_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cannot handle qcsi sig as qmi uim remote is not init");
    return;
  }

  memset(&os_params, 0, sizeof(qmi_csi_os_params));
  os_params.tcb = tcb;
  os_params.sig = sig;

  err = qmi_csi_handle_event(qmi_uim_remote_state_ptr->service_handle,
                             &os_params);

  if (err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("qmi_uim_remote_handle_qcsi_sig: failed with error=%d", err);
    return;
  }
} /* qmi_uim_remote_handle_qcsi_sig */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_DECODE_EVT

   DESCRIPTION:
     This function decodes the QCSI event enumeration to the QMI UIM Remote
     event enumeration

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_uim_remote_evt_enum_type qmi_uim_remote_decode_evt (
  uim_remote_event_type_enum_v01  event_enum
)
{
  switch (event_enum)
  {
    case UIM_REMOTE_CONNECTION_UNAVAILABLE_V01:
      return QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE;

    case UIM_REMOTE_CONNECTION_AVAILABLE_V01:
      return QMI_UIM_REMOTE_EVT_CONNECTION_AVAILABLE;

    case UIM_REMOTE_CARD_INSERTED_V01:
      return QMI_UIM_REMOTE_EVT_CARD_INSERTED;

    case UIM_REMOTE_CARD_REMOVED_V01:
      return QMI_UIM_REMOTE_EVT_CARD_REMOVED;

    case UIM_REMOTE_CARD_ERROR_V01:
      return QMI_UIM_REMOTE_EVT_CARD_ERROR;

    case UIM_REMOTE_CARD_RESET_V01:
      return QMI_UIM_REMOTE_EVT_CARD_RESET;

    case UIM_REMOTE_CARD_WAKEUP_V01:
      return QMI_UIM_REMOTE_EVT_CARD_WAKEUP;

    default:
      return QMI_UIM_REMOTE_EVT_NOT_APPLICABLE;
  }
} /* qmi_uim_remote_decode_evt */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_DECODE_SLOT

   DESCRIPTION:
     This function decodes the QCSI slot enumeration to the QMI UIM Remote
     slot enumeration

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_uim_remote_slot_type qmi_uim_remote_decode_slot (
  uim_remote_slot_type_enum_v01  slot
)
{
  switch (slot)
  {
    case UIM_REMOTE_SLOT_1_V01:
      return QMI_UIM_REMOTE_SLOT_1;

    case UIM_REMOTE_SLOT_2_V01:
      return QMI_UIM_REMOTE_SLOT_2;

    case UIM_REMOTE_SLOT_3_V01:
      return QMI_UIM_REMOTE_SLOT_3;

    default:
      return QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;
  }
} /* qmi_uim_remote_decode_slot */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_DECODE_CARD_ERROR

   DESCRIPTION:
     This function decodes the QCSI card error enumeration to the QMI UIM Remote
     card error enumeration

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_uim_remote_card_error_enum_type qmi_uim_remote_decode_card_error (
  uim_remote_card_error_type_enum_v01  card_error_enum
)
{
  switch (card_error_enum)
  {
    case UIM_REMOTE_CARD_ERROR_NO_LINK_ESTABLISHED_V01:
      return QMI_UIM_REMOTE_CARD_ERROR_NO_LINK_ESTABLISHED;

    case UIM_REMOTE_CARD_ERROR_COMMAND_TIMEOUT_V01:
      return QMI_UIM_REMOTE_CARD_ERROR_COMMAND_TIMEOUT;

    case UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN_V01:
      return QMI_UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN;

    case UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN_TELECOM_V01:
      return QMI_UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN_TELECOM;

    default:
      return QMI_UIM_REMOTE_CARD_ERROR_UNKNOWN_ERROR;
  }
} /* qmi_uim_remote_decode_card_error */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_DECODE_RESULT

   DESCRIPTION:
     This function decodes the QCSI APDU status enumeration to the QMI UIM Remote
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
static qmi_uim_remote_result_type qmi_uim_remote_decode_result (
  qmi_result_type_v01  result
)
{
  switch (result)
  {
    case QMI_RESULT_SUCCESS_V01:
      return QMI_UIM_REMOTE_SUCCESS;

    case QMI_RESULT_FAILURE_V01:
      return QMI_UIM_REMOTE_ERROR;

    default:
      return QMI_UIM_REMOTE_ERROR;
  }
} /* qmi_uim_remote_decode_result */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_ENCODE_SLOT

   DESCRIPTION:
     This function encodes the QCSI slot enumeration from the QMI UIM Remote
     slot enumeration

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static uim_remote_slot_type_enum_v01 qmi_uim_remote_encode_slot (
  qmi_uim_remote_slot_type  slot
)
{
  switch (slot)
  {
    case QMI_UIM_REMOTE_SLOT_1:
      return UIM_REMOTE_SLOT_1_V01;

    case QMI_UIM_REMOTE_SLOT_2:
      return UIM_REMOTE_SLOT_2_V01;

    case QMI_UIM_REMOTE_SLOT_3:
      return UIM_REMOTE_SLOT_3_V01;

    default:
      return UIM_REMOTE_SLOT_NOT_APPLICABLE_V01;
  }
} /* qmi_uim_remote_encode_slot */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_ENCODE_PDOWN_MODE

   DESCRIPTION:
     This function encodes the QCSI power down mode enumeration from the
     QMI UIM Remote power down mode enumeration

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean

   SIDE EFFECTS:
     None
===========================================================================*/
static boolean qmi_uim_remote_encode_pdown_mode (
  qmi_uim_remote_power_down_mode_enum_type   qmi_uimrmt_mode,
  uim_remote_power_down_mode_enum_v01      * uim_remote_mode
)
{
  switch (qmi_uimrmt_mode)
  {
    case QMI_UIM_REMOTE_POWER_DOWN_CARD:
      *uim_remote_mode = UIM_REMOTE_POWER_DOWN_CARD_V01;
      return TRUE;

    case QMI_UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE:
      *uim_remote_mode = UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE_V01;
      return TRUE;

    default:
      return FALSE;
  }
} /* qmi_uim_remote_encode_pdown_mode */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_ENCODE_PUP_VCC

   DESCRIPTION:
     This function encodes the QCSI power up vcc from the
     QMI UIM Remote power up vcc

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean

   SIDE EFFECTS:
     None
===========================================================================*/
static boolean qmi_uim_remote_encode_pup_vcc (
  qmi_uim_remote_voltage_class_enum_type   qmi_uimrmt_vcc,
  uim_remote_voltage_class_enum_v01      * uim_remote_vcc
)
{
  switch (qmi_uimrmt_vcc)
  {
    case QMI_UIM_REMOTE_VOLTAGE_CLASS_C_LOW:
      *uim_remote_vcc = UIM_REMOTE_VOLTAGE_CLASS_C_LOW_V01;
      return TRUE;

    case QMI_UIM_REMOTE_VOLTAGE_CLASS_C:
      *uim_remote_vcc = UIM_REMOTE_VOLTAGE_CLASS_C_V01;
      return TRUE;

    case QMI_UIM_REMOTE_VOLTAGE_CLASS_C_HIGH:
      *uim_remote_vcc = UIM_REMOTE_VOLTAGE_CLASS_C_HIGH_V01;
      return TRUE;

    case QMI_UIM_REMOTE_VOLTAGE_CLASS_B_LOW:
      *uim_remote_vcc = UIM_REMOTE_VOLTAGE_CLASS_B_LOW_V01;
      return TRUE;

    case QMI_UIM_REMOTE_VOLTAGE_CLASS_B:
      *uim_remote_vcc = UIM_REMOTE_VOLTAGE_CLASS_B_V01;
      return TRUE;

    case QMI_UIM_REMOTE_VOLTAGE_CLASS_B_HIGH:
      *uim_remote_vcc = UIM_REMOTE_VOLTAGE_CLASS_B_HIGH_V01;
      return TRUE;

    default:
      return FALSE;
  }
} /* qmi_uim_remote_encode_pup_vcc */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_RESET_REQ_HANDLER

   DESCRIPTION:
     This function processes the reset request from the card client

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_reset_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
)
{
  uim_remote_reset_resp_msg_v01     resp;

  qmi_uim_remote_disconnect_cb(conn_handle, NULL);

  resp.resp.error  = QMI_ERR_NONE_V01;
  resp.resp.result = QMI_RESULT_SUCCESS_V01;

  return qmi_csi_send_resp(req_handle,
                           QMI_UIM_REMOTE_RESET_RESP_V01,
                           &resp,
                           sizeof(resp));
} /* qmi_uim_remote_reset_req_handler */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_EVENT_REQ_HANDLER

   DESCRIPTION:
     This function processes card and connection events sent from the card
     client

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_event_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
)
{
  uim_remote_event_req_msg_v01           * req_msg_ptr         = NULL;
  uint8                                    card_service_index  = 0;
  uint8                                    card_client_index   = 0;
  uim_remote_event_resp_msg_v01            resp;
  qmi_uim_remote_event_data_type           evt;
  qmi_uim_remote_card_client_info_type   * card_client_handle_ptr = NULL;

  ASSERT(qmi_uim_remote_state_ptr != NULL);

  if (req_handle == NULL || req_c_struct_len == 0x0)
  {
    UIM_MSG_LOW_0("qmi_uim_remote_event_req_handler: Bad input");
    return QMI_CSI_INTERNAL_ERR;
  }

  req_msg_ptr            = (uim_remote_event_req_msg_v01 *)req_c_struct;
  card_client_handle_ptr = (qmi_uim_remote_card_client_info_type *)conn_handle;

  UIM_MSG_HIGH_2("qmi_uim_remote_event_req_handler:event 0x%x, slot 0x%x",
                 req_msg_ptr->event_info.event, req_msg_ptr->event_info.slot);

  resp.resp.error  = QMI_ERR_NONE_V01;
  resp.resp.result = QMI_RESULT_SUCCESS_V01;

  /* Decode the event received */
  evt.evt   = qmi_uim_remote_decode_evt(req_msg_ptr->event_info.event);
  evt.slot  = qmi_uim_remote_decode_slot(req_msg_ptr->event_info.slot);

  if (evt.evt == QMI_UIM_REMOTE_EVT_CARD_INSERTED && req_msg_ptr->atr_valid)
  {
    evt.evt_data.card_inserted.atr.data_len = (uint8) req_msg_ptr->atr_len;
    evt.evt_data.card_inserted.atr.data_ptr = req_msg_ptr->atr;
  }
  else if (evt.evt == QMI_UIM_REMOTE_EVT_CARD_RESET && req_msg_ptr->atr_valid)
  {
    evt.evt_data.card_reset.atr.data_len = (uint8) req_msg_ptr->atr_len;
    evt.evt_data.card_reset.atr.data_ptr = req_msg_ptr->atr;
  }
  else if (evt.evt == QMI_UIM_REMOTE_EVT_CARD_ERROR)
  {
    if (req_msg_ptr->error_cause_valid)
    {
      evt.evt_data.card_error.err_cause =
        qmi_uim_remote_decode_card_error(req_msg_ptr->error_cause);
    }
  }
  else if (evt.evt == QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE ||
           evt.evt == QMI_UIM_REMOTE_EVT_CONNECTION_AVAILABLE   ||
           evt.evt == QMI_UIM_REMOTE_EVT_CARD_REMOVED           ||
           evt.evt == QMI_UIM_REMOTE_EVT_CARD_WAKEUP)
  {
    memset(&(evt.evt_data), 0x0, sizeof(evt.evt_data));
  }
  else
  {
    UIM_MSG_ERR_0("Received malformed or unknown event request");
    resp.resp.error  = QMI_ERR_MALFORMED_MSG_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  if (req_msg_ptr->wakeup_support_valid)
  {
    evt.wakeup_support  = req_msg_ptr->wakeup_support;
  }

  for (card_client_index = 0;
       card_client_index < QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT;
       card_client_index++)
  {
    if (evt.slot < QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE                                &&
        qmi_uim_remote_state_ptr->card_client_registry[card_client_index].in_use     &&
        qmi_uim_remote_state_ptr->card_client_registry[card_client_index].client_handle ==
          card_client_handle_ptr->client_handle)
    {
       /* If it is a connection available event then copy the slot for the particular
          client */
      if (evt.evt == QMI_UIM_REMOTE_EVT_CONNECTION_AVAILABLE)
      {
        qmi_uim_remote_state_ptr->card_client_registry[card_client_index].slot =
          evt.slot;
      }
      /* If a connection unavailable or card error event is received and there is a
         pending APDU command clear it */
      else if ((evt.evt == QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE   ||
                evt.evt == QMI_UIM_REMOTE_EVT_CARD_ERROR)                        &&
               qmi_uim_remote_state_ptr->card_client_registry[card_client_index].slot ==
                 evt.slot)
      {
        memset(&(qmi_uim_remote_state_ptr->card_client_registry[card_client_index].curr_apdu),
               0x0, sizeof(qmi_uim_remote_curr_apdu_info_type));
      }

      /* Save the last received event data in case we need to call an event callback
         of a late card service registrant */
      if (qmi_uim_remote_state_ptr->card_client_registry[card_client_index].slot ==
            evt.slot)
      {
        memscpy(&(qmi_uim_remote_state_ptr->card_client_registry[card_client_index].last_evt_data),
                sizeof(qmi_uim_remote_event_data_type),
                &evt,
                sizeof(qmi_uim_remote_event_data_type));

        /* Setting slot value to INVALID as connection_unavailable is done.
           The slot value will be filled in connection available request */
        if (evt.evt == QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE)
        {
          qmi_uim_remote_state_ptr->card_client_registry[card_client_index].slot = QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;
        }
        break;
      }
    }
  }

  if (card_client_index == QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT)
  {
    UIM_MSG_LOW_0("qmi_uim_remote_event_req_handler: Cannot find client for slot");
    resp.resp.error  = QMI_ERR_INTERNAL_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  /* If a card service has registered for a remote card check if it is for
     the slot for which the card event is received and dispatch the event
     to the card service */
  for (card_service_index = 0;
       card_service_index < QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE;
       card_service_index++)
  {
    if (qmi_uim_remote_state_ptr->card_service_registry[card_service_index].in_use &&
        qmi_uim_remote_state_ptr->card_service_registry[card_service_index].slot ==
        evt.slot)
    {
      break;
    }
  }

  if (card_service_index == QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE)
  {
    UIM_MSG_LOW_0("qmi_uim_remote_event_req_handler: Cannot find service");
    resp.resp.error  = QMI_ERR_INTERNAL_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  (qmi_uim_remote_state_ptr->card_service_registry[card_service_index].event_cb_ptr)(&evt);

send_resp:
  return qmi_csi_send_resp(req_handle,
                           QMI_UIM_REMOTE_EVENT_RESP_V01,
                           &resp,
                           sizeof(resp));
} /* qmi_uim_remote_event_req_handler */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_APDU_REQ_HANDLER

   DESCRIPTION:
     This function processes the APDU transaction response sent by the card
     client

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_apdu_req_handler (
  void                * conn_handle,
  qmi_req_handle        req_handle,
  void                * req_c_struct,
  int                   req_c_struct_len
)
{
  uim_remote_apdu_req_msg_v01            * req_msg_ptr         = NULL;
  uint8                                    card_client_index   = 0;
  uim_remote_apdu_resp_msg_v01             resp;
  qmi_uim_remote_rsp_apdu_data_type      * rsp_ptr             = NULL;
  qmi_uim_remote_card_client_info_type   * card_client_ptr     = NULL;
  qmi_uim_remote_card_client_info_type   * client_handle_ptr   = NULL;

  client_handle_ptr = (qmi_uim_remote_card_client_info_type *)conn_handle;

  ASSERT(qmi_uim_remote_state_ptr != NULL);

  UIM_MSG_HIGH_0("qmi_uim_remote_apdu_req_handler");

  if (req_handle == NULL || req_c_struct_len == 0x0 || client_handle_ptr == NULL)
  {
    UIM_MSG_LOW_0("qmi_uim_remote_apdu_req_handler: Bad input");
    return QMI_CSI_INTERNAL_ERR;
  }

  req_msg_ptr = (uim_remote_apdu_req_msg_v01 *)req_c_struct;

  resp.resp.error  = QMI_ERR_NONE_V01;
  resp.resp.result = QMI_RESULT_SUCCESS_V01;

  /* Find the client from which the response APDU was received based on the
     slot in the APDU message */
  for (card_client_index = 0;
       card_client_index < QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT;
       card_client_index++)
  {
    if (qmi_uim_remote_state_ptr->card_client_registry[card_client_index].in_use &&
        qmi_uim_remote_state_ptr->card_client_registry[card_client_index].slot ==
          qmi_uim_remote_decode_slot(req_msg_ptr->slot) &&
        qmi_uim_remote_state_ptr->card_client_registry[card_client_index].client_handle ==
          client_handle_ptr->client_handle)
    {
      break;
    }
  }

  if (card_client_index == QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT)
  {
    UIM_MSG_LOW_0("qmi_uim_remote_apdu_req_handler: Cannot find client");
    resp.resp.error  = QMI_ERR_INTERNAL_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  /* Check to ensure that the APDU ID in the response APDU matches with the
     APDU ID of the command APDU that was sent */
  card_client_ptr =
    &(qmi_uim_remote_state_ptr->card_client_registry[card_client_index]);

  if (card_client_ptr->curr_apdu.id != req_msg_ptr->apdu_id)
  {
    UIM_MSG_ERR_2("Pending APDU id 0x%x does not match recevied APDU id 0x%x",
                  card_client_ptr->curr_apdu.id, req_msg_ptr->apdu_id);
    resp.resp.error  = QMI_ERR_MALFORMED_MSG_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  rsp_ptr = &(card_client_ptr->curr_apdu.rsp_apdu_data);

  rsp_ptr->result = qmi_uim_remote_decode_result(req_msg_ptr->apdu_status);
  rsp_ptr->slot   = qmi_uim_remote_decode_slot(req_msg_ptr->slot);

  /* If card client returns an error for the APDU transaction notify
     the card service about the error */
  if (rsp_ptr->result == QMI_UIM_REMOTE_ERROR)
  {
    goto callback_and_send_resp;
  }

  /* If the card client returns success for the APDU transaction
     but APDU response message is malformed return a error to
     the card client */
  if (req_msg_ptr->response_apdu_info_valid == FALSE ||
      (req_msg_ptr->response_apdu_info_valid &&
       req_msg_ptr->response_apdu_info.total_response_apdu_size < 2) ||
      req_msg_ptr->response_apdu_segment_valid == FALSE ||
      (req_msg_ptr->response_apdu_info_valid &&
       req_msg_ptr->response_apdu_segment_len == 0))
  {
    UIM_MSG_ERR_3("For APDU id 0x%x malformed msg apdu info valid 0x%x, len 0x%x",
                  req_msg_ptr->apdu_id,
                  req_msg_ptr->response_apdu_info_valid,
                  req_msg_ptr->response_apdu_info.total_response_apdu_size);
    UIM_MSG_ERR_2("Apdu segment info valid 0x%x, segment len 0x%x",
                  req_msg_ptr->response_apdu_segment_valid,
                  req_msg_ptr->response_apdu_segment_len);
    resp.resp.error  = QMI_ERR_MALFORMED_MSG_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  /* Construct the response APDU as follows */
  /* If this is the first response APDU segment received determine the
     total APDU length and allocate required buffer size to build the
     response */
  if (rsp_ptr->rsp_apdu_ptr == NULL)
  {
    rsp_ptr->rsp_apdu_len =
      req_msg_ptr->response_apdu_info.total_response_apdu_size;
    rsp_ptr->rsp_apdu_ptr =
      modem_mem_calloc(1,
                       req_msg_ptr->response_apdu_info.total_response_apdu_size,
                       MODEM_MEM_CLIENT_UIM);
  }

  if (rsp_ptr->rsp_apdu_ptr == NULL)
  {
    UIM_MSG_ERR_0("Response APDU buffer NULL, heap exhaustion or corruption");
    resp.resp.error  = QMI_ERR_INTERNAL_V01;
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_resp;
  }

  /* Copy over the APDU segment into the allocated buffer as long as the
     segment offset + segment length does not exceed the total response
     APDU length */
  if ((req_msg_ptr->response_apdu_info.response_apdu_segment_offset +
       req_msg_ptr->response_apdu_segment_len) <=
         req_msg_ptr->response_apdu_info.total_response_apdu_size)
  {
    memscpy(rsp_ptr->rsp_apdu_ptr
            + req_msg_ptr->response_apdu_info.response_apdu_segment_offset,
            rsp_ptr->rsp_apdu_len,
            req_msg_ptr->response_apdu_segment,
            req_msg_ptr->response_apdu_segment_len);
  }

  /* There is an assumption that if there is more than one APDU segment
     the card client sends the segments in order and the transport
     also delivers the segments in order.
     If (segment offset + segment length) received is smaller than the
     total response APDU length then we copy over the segment received
     and wait for the next segment by just returning the QMI resopnse
     back to the card client.
     If (segment offset + segment length) received is equal to total
     APDU length then we have received all the segments and we can
     call the card service callback to deliver the response APDU */
  if ((req_msg_ptr->response_apdu_info.response_apdu_segment_offset +
       req_msg_ptr->response_apdu_segment_len) <
         req_msg_ptr->response_apdu_info.total_response_apdu_size)
  {
    goto send_resp;
  }

callback_and_send_resp:
  if (card_client_ptr->curr_apdu.rsp_apdu_cb_ptr)
  {
    (card_client_ptr->curr_apdu.rsp_apdu_cb_ptr)(rsp_ptr);
  }

  /* As the callback has been called and response APDU
     delievered the response APDU buffer is freed */
  if (rsp_ptr->rsp_apdu_ptr)
  {
    modem_mem_free(rsp_ptr->rsp_apdu_ptr, MODEM_MEM_CLIENT_UIM);
    memset(rsp_ptr, 0x0, sizeof(qmi_uim_remote_rsp_apdu_data_type));
    rsp_ptr = NULL;
    card_client_ptr->curr_apdu.rsp_apdu_cb_ptr = NULL;
    card_client_ptr = NULL;
  }

send_resp:
  return qmi_csi_send_resp(req_handle,
                           QMI_UIM_REMOTE_APDU_RESP_V01,
                           &resp,
                           sizeof(resp));
} /* qmi_uim_remote_apdu_req_handler */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CMD_MEM_FREE

   DESCRIPTION:
     This function is used to free the memory allocated to process a command
     queued by a card service

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_remote_cmd_mem_free (
  qmi_uim_remote_cmd_enum_type    cmd_enum,
  qmi_uim_remote_cmd_type       * cmd_ptr
)
{
  if (cmd_ptr == NULL)
  {
    return;
  }

  switch (cmd_enum)
  {
    case QMI_UIM_REMOTE_CMD_REGISTER:
    case QMI_UIM_REMOTE_CMD_RELEASE:
    case QMI_UIM_REMOTE_CMD_CARD_CONNECT:
    case QMI_UIM_REMOTE_CMD_CARD_DISCONNECT:
    case QMI_UIM_REMOTE_CMD_CARD_POWER_DOWN:
    case QMI_UIM_REMOTE_CMD_CARD_POWER_UP:
    case QMI_UIM_REMOTE_CMD_CARD_RESET:
      modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_UIM);
      break;

    case QMI_UIM_REMOTE_CMD_SEND_APDU:
      if (cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_ptr)
      {
        modem_mem_free(cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_ptr,
                       MODEM_MEM_CLIENT_UIM);
      }
      modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_UIM);
      break;

    default:
      break;
  }
} /* qmi_uim_remote_cmd_mem_free */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_POPULATE_CARD_CONNECT_IND

   DESCRIPTION:
     This function populates the card connect QSCI indication to be
     sent to remote card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_csi_cb_error

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_populate_card_connect_ind (
  const  qmi_uim_remote_cmd_type   * cmd_ptr,
  unsigned int                     * msg_id_ptr,
  unsigned int                     * struct_len_ptr,
  void                            ** ind_c_struct_ptr
)
{
  ASSERT(cmd_ptr          != NULL);
  ASSERT(struct_len_ptr   != NULL);
  ASSERT(msg_id_ptr       != NULL);
  ASSERT(ind_c_struct_ptr != NULL);

  *msg_id_ptr        = QMI_UIM_REMOTE_CONNECT_IND_V01;
  *struct_len_ptr    = sizeof(uim_remote_connect_ind_msg_v01);

  *ind_c_struct_ptr  =
    modem_mem_calloc(1, *struct_len_ptr, MODEM_MEM_CLIENT_UIM);

  if (*ind_c_struct_ptr == NULL)
  {
    UIM_MSG_ERR_0("Populate indication: heap exhaustion");
    return QMI_CSI_INTERNAL_ERR;
  }

  ((uim_remote_connect_ind_msg_v01 *)(*ind_c_struct_ptr))->slot =
    qmi_uim_remote_encode_slot(cmd_ptr->slot);

  return QMI_CSI_NO_ERR;
} /* qmi_uim_remote_populate_card_connect_ind */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_POPULATE_CARD_DISCONNECT_IND

   DESCRIPTION:
     This function populates the card disconnect QSCI indication to be
     sent to remote card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_csi_cb_error

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_populate_card_disconnect_ind (
  const  qmi_uim_remote_cmd_type   * cmd_ptr,
  unsigned int                     * msg_id_ptr,
  unsigned int                     * struct_len_ptr,
  void                            ** ind_c_struct_ptr
)
{
  ASSERT(cmd_ptr          != NULL);
  ASSERT(struct_len_ptr   != NULL);
  ASSERT(msg_id_ptr       != NULL);
  ASSERT(ind_c_struct_ptr != NULL);

  *msg_id_ptr       = QMI_UIM_REMOTE_DISCONNECT_IND_V01;
  *struct_len_ptr   = sizeof(uim_remote_disconnect_ind_msg_v01);

  *ind_c_struct_ptr =
    modem_mem_calloc(1, *struct_len_ptr, MODEM_MEM_CLIENT_UIM);

  if (*ind_c_struct_ptr == NULL)
  {
    UIM_MSG_ERR_0("Populate indication: heap exhaustion");
    return QMI_CSI_INTERNAL_ERR;
  }

  ((uim_remote_disconnect_ind_msg_v01 *)(*ind_c_struct_ptr))->slot =
    qmi_uim_remote_encode_slot(cmd_ptr->slot);

  return QMI_CSI_NO_ERR;
} /* qmi_uim_remote_populate_card_disconnect_ind */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_POPULATE_CARD_POWER_DOWN_IND

   DESCRIPTION:
     This function populates the card power down QSCI indication to be
     sent to remote card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_csi_cb_error

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_populate_card_power_down_ind (
  const  qmi_uim_remote_cmd_type   * cmd_ptr,
  unsigned int                     * msg_id_ptr,
  unsigned int                     * struct_len_ptr,
  void                            ** ind_c_struct_ptr
)
{
  ASSERT(cmd_ptr          != NULL);
  ASSERT(struct_len_ptr   != NULL);
  ASSERT(msg_id_ptr       != NULL);
  ASSERT(ind_c_struct_ptr != NULL);

  *msg_id_ptr       = QMI_UIM_REMOTE_CARD_POWER_DOWN_IND_V01;
  *struct_len_ptr   = sizeof(uim_remote_card_power_down_ind_msg_v01);

  *ind_c_struct_ptr =
    modem_mem_calloc(1, *struct_len_ptr, MODEM_MEM_CLIENT_UIM);

  if (*ind_c_struct_ptr == NULL)
  {
    UIM_MSG_ERR_0("Populate indication: heap exhaustion");
    return QMI_CSI_INTERNAL_ERR;
  }

  ((uim_remote_card_power_down_ind_msg_v01 *)(*ind_c_struct_ptr))->slot =
    qmi_uim_remote_encode_slot(cmd_ptr->slot);

  if (cmd_ptr->cmd_data.power_down_req.pdown_mode >= QMI_UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE &&
      cmd_ptr->cmd_data.power_down_req.pdown_mode <= QMI_UIM_REMOTE_POWER_DOWN_CARD)
  {
    ((uim_remote_card_power_down_ind_msg_v01 *)(*ind_c_struct_ptr))->mode_valid =
        qmi_uim_remote_encode_pdown_mode(cmd_ptr->cmd_data.power_down_req.pdown_mode,
                                         &(((uim_remote_card_power_down_ind_msg_v01 *)(*ind_c_struct_ptr))->mode));
  }

  return QMI_CSI_NO_ERR;
} /* qmi_uim_remote_populate_card_power_down_ind */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_POPULATE_CARD_POWER_UP_IND

   DESCRIPTION:
     This function populates the card power up QSCI indication to be
     sent to remote card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_csi_cb_error

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_populate_card_power_up_ind (
  const  qmi_uim_remote_cmd_type   * cmd_ptr,
  unsigned int                     * msg_id_ptr,
  unsigned int                     * struct_len_ptr,
  void                            ** ind_c_struct_ptr
)
{
  ASSERT(cmd_ptr          != NULL);
  ASSERT(struct_len_ptr   != NULL);
  ASSERT(msg_id_ptr       != NULL);
  ASSERT(ind_c_struct_ptr != NULL);

  *msg_id_ptr       = QMI_UIM_REMOTE_CARD_POWER_UP_IND_V01;
  *struct_len_ptr   = sizeof(uim_remote_card_power_up_ind_msg_v01);

  *ind_c_struct_ptr =
    modem_mem_calloc(1, *struct_len_ptr, MODEM_MEM_CLIENT_UIM);

  if (*ind_c_struct_ptr == NULL)
  {
    UIM_MSG_ERR_0("Populate indication: heap exhaustion");
    return QMI_CSI_INTERNAL_ERR;
  }

  ((uim_remote_card_power_up_ind_msg_v01 *)(*ind_c_struct_ptr))->slot =
    qmi_uim_remote_encode_slot(cmd_ptr->slot);

  if (cmd_ptr->cmd_data.power_up_req.timeout_isvalid &&
      cmd_ptr->cmd_data.power_up_req.timeout > 0)
  {
    ((uim_remote_card_power_up_ind_msg_v01 *)(*ind_c_struct_ptr))->response_timeout_valid = TRUE;
    ((uim_remote_card_power_up_ind_msg_v01 *)(*ind_c_struct_ptr))->response_timeout =
         cmd_ptr->cmd_data.power_up_req.timeout;
  }

  if (cmd_ptr->cmd_data.power_up_req.vcc_isvalid &&
      cmd_ptr->cmd_data.power_up_req.vcc >= QMI_UIM_REMOTE_VOLTAGE_CLASS_C_LOW &&
      cmd_ptr->cmd_data.power_up_req.vcc <= QMI_UIM_REMOTE_VOLTAGE_CLASS_B_HIGH)
  {
    ((uim_remote_card_power_up_ind_msg_v01 *)(*ind_c_struct_ptr))->voltage_class_valid =
      qmi_uim_remote_encode_pup_vcc(cmd_ptr->cmd_data.power_up_req.vcc,
                                    &(((uim_remote_card_power_up_ind_msg_v01 *)
                                       (*ind_c_struct_ptr))->voltage_class));
  }

  return QMI_CSI_NO_ERR;
} /* qmi_uim_remote_populate_card_power_up_ind */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_POPULATE_CARD_RESET_IND

   DESCRIPTION:
     This function populates the card reset QSCI indication to be
     sent to remote card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_csi_cb_error

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_populate_card_reset_ind (
  const  qmi_uim_remote_cmd_type   * cmd_ptr,
  unsigned int                     * msg_id_ptr,
  unsigned int                     * struct_len_ptr,
  void                            ** ind_c_struct_ptr
)
{
  ASSERT(cmd_ptr          != NULL);
  ASSERT(struct_len_ptr   != NULL);
  ASSERT(msg_id_ptr       != NULL);
  ASSERT(ind_c_struct_ptr != NULL);

  *msg_id_ptr       = QMI_UIM_REMOTE_CARD_RESET_IND_V01;
  *struct_len_ptr   = sizeof(uim_remote_card_reset_ind_msg_v01);

  *ind_c_struct_ptr =
    modem_mem_calloc(1, *struct_len_ptr, MODEM_MEM_CLIENT_UIM);

  if (*ind_c_struct_ptr == NULL)
  {
    UIM_MSG_ERR_0("Populate indication: heap exhaustion");
    return QMI_CSI_INTERNAL_ERR;
  }

  ((uim_remote_card_reset_ind_msg_v01 *)(*ind_c_struct_ptr))->slot =
    qmi_uim_remote_encode_slot(cmd_ptr->slot);

  return QMI_CSI_NO_ERR;
} /* qmi_uim_remote_populate_card_reset_ind */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_POPULATE_SEND_APDU_IND

   DESCRIPTION:
     This function populates the send APDU QSCI indication to be
     sent to remote card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_csi_cb_error

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_csi_error qmi_uim_remote_populate_send_apdu_ind (
  const  qmi_uim_remote_cmd_type   * cmd_ptr,
  uint8                              card_client_index,
  unsigned int                     * msg_id_ptr,
  unsigned int                     * struct_len_ptr,
  void                            ** ind_c_struct_ptr
)
{
  ASSERT(cmd_ptr                  != NULL);
  ASSERT(struct_len_ptr           != NULL);
  ASSERT(msg_id_ptr               != NULL);
  ASSERT(ind_c_struct_ptr         != NULL);
  ASSERT(qmi_uim_remote_state_ptr != NULL);

  *msg_id_ptr       = QMI_UIM_REMOTE_APDU_IND_V01;
  *struct_len_ptr   = sizeof(uim_remote_apdu_ind_msg_v01);

  if (cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_len >
        QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN_V01)
  {
    UIM_MSG_ERR_1("Populate APDU ind: Cmd APDU len 0x%x invalid",
                  cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_len);
    return QMI_CSI_INTERNAL_ERR;
  }

  *ind_c_struct_ptr =
    modem_mem_calloc(1, *struct_len_ptr, MODEM_MEM_CLIENT_UIM);

  if (*ind_c_struct_ptr == NULL)
  {
    UIM_MSG_ERR_0("Populate indication: heap exhaustion");
    return QMI_CSI_INTERNAL_ERR;
  }

  ((uim_remote_apdu_ind_msg_v01 *)(*ind_c_struct_ptr))->slot             =
    qmi_uim_remote_encode_slot(cmd_ptr->slot);

  /* The APDU ID is a counter to keep track of the APDU sent and associate the
     the right response with the request. A pre-increment operator so that the
     APDU ID is updated before it is assigned to the request message. */
  ((uim_remote_apdu_ind_msg_v01 *)(*ind_c_struct_ptr))->apdu_id          =
    ++(qmi_uim_remote_state_ptr->card_client_registry[card_client_index].curr_apdu.id);

  ((uim_remote_apdu_ind_msg_v01 *)(*ind_c_struct_ptr))->command_apdu_len =
    cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_len;

  memscpy(((uim_remote_apdu_ind_msg_v01 *)(*ind_c_struct_ptr))->command_apdu,
          QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN_V01,
          cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_ptr,
          cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_len);

  qmi_uim_remote_state_ptr->card_client_registry[card_client_index].curr_apdu.rsp_apdu_data.client_ref_ptr =
     cmd_ptr->cmd_data.send_apdu_req.client_ref_ptr;

  qmi_uim_remote_state_ptr->card_client_registry[card_client_index].curr_apdu.rsp_apdu_cb_ptr =
     cmd_ptr->cmd_data.send_apdu_req.rsp_apdu_cb_ptr;

  return QMI_CSI_NO_ERR;
} /* qmi_uim_remote_populate_send_apdu_ind */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_PROCESS_CARD_OP_REQ

   DESCRIPTION:
     This function processes requests from the card service bound to the
     remote card which take slot as input and expect a certain operation
     such as power down, power up, connect, disconnect, reset to be
     performed on the remote card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_remote_process_card_op_req (
  qmi_uim_remote_cmd_type     * cmd_ptr
)
{
  uint8                card_client_index = 0x0;
  qmi_csi_error        ind_status        = QMI_CSI_NO_ERR;
  unsigned int         ind_c_struct_len  = 0x0;
  unsigned int         msg_id            = 0x0;
  void               * ind_c_struct_ptr  = NULL;
  qmi_client_handle    client_handle;

  ASSERT(cmd_ptr != NULL);
  ASSERT(qmi_uim_remote_state_ptr != NULL);

  for (card_client_index = 0;
       card_client_index < QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT;
       card_client_index++)
  {
    if (qmi_uim_remote_state_ptr->card_client_registry[card_client_index].slot ==
          cmd_ptr->slot     &&
        qmi_uim_remote_state_ptr->card_client_registry[card_client_index].last_evt_data.evt >
          QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE)
    {
      client_handle =
        qmi_uim_remote_state_ptr->card_client_registry[card_client_index].client_handle;
      break;
    }
  }

  if (card_client_index == QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT)
  {
    UIM_MSG_ERR_0("Unable to find card client to send command");

    if (cmd_ptr->cmd_enum == QMI_UIM_REMOTE_CMD_SEND_APDU)
    {
      qmi_uim_remote_rsp_apdu_data_type  rsp_data;

      memset(&rsp_data, 0x0, sizeof(qmi_uim_remote_rsp_apdu_data_type));
      rsp_data.result         = QMI_UIM_REMOTE_ERROR;
      rsp_data.slot           = cmd_ptr->slot;
      rsp_data.client_ref_ptr = cmd_ptr->cmd_data.send_apdu_req.client_ref_ptr;
      cmd_ptr->cmd_data.send_apdu_req.rsp_apdu_cb_ptr(&rsp_data);
    }
    return;
  }

  switch (cmd_ptr->cmd_enum)
  {
    case QMI_UIM_REMOTE_CMD_CARD_CONNECT:
      ind_status =
        qmi_uim_remote_populate_card_connect_ind(cmd_ptr, &msg_id,
          &ind_c_struct_len, &ind_c_struct_ptr);
      break;

    case QMI_UIM_REMOTE_CMD_CARD_DISCONNECT:
      ind_status =
        qmi_uim_remote_populate_card_disconnect_ind(cmd_ptr, &msg_id,
          &ind_c_struct_len, &ind_c_struct_ptr);
      break;

    case QMI_UIM_REMOTE_CMD_CARD_POWER_DOWN:
      ind_status =
        qmi_uim_remote_populate_card_power_down_ind(cmd_ptr, &msg_id,
          &ind_c_struct_len, &ind_c_struct_ptr);
      break;

    case QMI_UIM_REMOTE_CMD_CARD_POWER_UP:
      ind_status =
        qmi_uim_remote_populate_card_power_up_ind(cmd_ptr, &msg_id,
          &ind_c_struct_len, &ind_c_struct_ptr);
      break;

    case QMI_UIM_REMOTE_CMD_CARD_RESET:
      ind_status =
        qmi_uim_remote_populate_card_reset_ind(cmd_ptr, &msg_id,
          &ind_c_struct_len, &ind_c_struct_ptr);
      break;

    case QMI_UIM_REMOTE_CMD_SEND_APDU:
      ind_status =
        qmi_uim_remote_populate_send_apdu_ind(cmd_ptr, card_client_index,
          &msg_id, &ind_c_struct_len, &ind_c_struct_ptr);
      break;

    default:
      break;
  }

  if (ind_status == QMI_CSI_NO_ERR)
  {
    ind_status = qmi_csi_send_ind(client_handle,
                                  msg_id,
                                  ind_c_struct_ptr,
                                  ind_c_struct_len);
    UIM_MSG_HIGH_2("Card operation req 0x%x send indication status : 0x%x",
                   cmd_ptr->cmd_enum, ind_status);
  }

  if (ind_status != QMI_CSI_NO_ERR &&
      cmd_ptr->cmd_enum == QMI_UIM_REMOTE_CMD_SEND_APDU)
  {
    qmi_uim_remote_rsp_apdu_data_type  rsp_data;

    memset(&rsp_data, 0x0, sizeof(qmi_uim_remote_rsp_apdu_data_type));
    rsp_data.result         = QMI_UIM_REMOTE_ERROR;
    rsp_data.slot           = cmd_ptr->slot;
    rsp_data.client_ref_ptr = cmd_ptr->cmd_data.send_apdu_req.client_ref_ptr;
    cmd_ptr->cmd_data.send_apdu_req.rsp_apdu_cb_ptr(&rsp_data);
  }

  if (ind_c_struct_ptr)
  {
    modem_mem_free(ind_c_struct_ptr, MODEM_MEM_CLIENT_UIM);
    ind_c_struct_ptr = NULL;
  }
} /* qmi_uim_remote_process_card_op_req */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_PROCESS_RELEASE_REQ

   DESCRIPTION:
     This function processes the release from a card service

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_remote_process_release_req (
  qmi_uim_remote_cmd_type     * cmd_ptr
)
{
  uint8  index = 0;

  ASSERT(cmd_ptr != NULL);
  ASSERT(qmi_uim_remote_state_ptr != NULL);

  /* Remove the card service to the service registry */
  for (index = 0; index < QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE; index++)
  {
    if (qmi_uim_remote_state_ptr->card_service_registry[index].slot ==
          cmd_ptr->slot)
    {
      qmi_uim_remote_state_ptr->card_service_registry[index].in_use = FALSE;
      qmi_uim_remote_state_ptr->card_service_registry[index].event_cb_ptr =
        NULL;
      qmi_uim_remote_state_ptr->card_service_registry[index].slot =
        QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;
      break;
    }
  }
} /* qmi_uim_remote_process_release_req */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_PROCESS_REGISTER_REQ

   DESCRIPTION:
     This function processes the registration request from a card service

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_remote_process_register_req (
  qmi_uim_remote_cmd_type     * cmd_ptr
)
{
  uint8  index = 0;

  ASSERT(cmd_ptr != NULL);
  ASSERT(qmi_uim_remote_state_ptr != NULL);
  ASSERT(cmd_ptr->cmd_data.reg_req.event_cb_ptr != NULL);

  /* Ensure that a card service is not already registered for the slot
     for which there is a registration request */
  for (index = 0; index < QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE; index++)
  {
    if (qmi_uim_remote_state_ptr->card_service_registry[index].in_use &&
        qmi_uim_remote_state_ptr->card_service_registry[index].slot ==
          cmd_ptr->slot)
    {
      UIM_MSG_ERR_1("There is already a card service registered for slot 0x%x",
                    cmd_ptr->slot);
      return;
    }
  }

  /* Add the card service to the service registry */
  for (index = 0; index < QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE; index++)
  {
    if (qmi_uim_remote_state_ptr->card_service_registry[index].in_use == FALSE)
    {
      qmi_uim_remote_state_ptr->card_service_registry[index].in_use = TRUE;
      qmi_uim_remote_state_ptr->card_service_registry[index].event_cb_ptr =
        cmd_ptr->cmd_data.reg_req.event_cb_ptr;
      qmi_uim_remote_state_ptr->card_service_registry[index].slot = cmd_ptr->slot;
      break;
    }
  }

  if (index == QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE)
  {
    UIM_MSG_LOW_0("Card service registry full. Cannot process request");
    return;
  }

  /* If an event was previously received for the slot dispatch it to the
     service */
  for (index = 0; index < QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT; index++)
  {
    if (qmi_uim_remote_state_ptr->card_client_registry[index].in_use    &&
        qmi_uim_remote_state_ptr->card_client_registry[index].slot ==
          cmd_ptr->slot                                                 &&
        qmi_uim_remote_state_ptr->card_client_registry[index].last_evt_data.evt >
          QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE)
    {
      (cmd_ptr->cmd_data.reg_req.event_cb_ptr)(
        &(qmi_uim_remote_state_ptr->card_client_registry[index].last_evt_data));
      break;
    }
  }
} /* qmi_uim_remote_process_register_req */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_PROCESS_CMD

   DESCRIPTION:
     This function processes the QMI UIM REMOTE command request

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void qmi_uim_remote_process_cmd (
  void
)
{
  qmi_uim_remote_cmd_type     * cmd_ptr    = NULL;

  ASSERT(qmi_uim_remote_state_ptr != NULL);

  if (q_cnt(&qmi_uim_remote_state_ptr->cmd_q) > 0)
  {
    cmd_ptr = (qmi_uim_remote_cmd_type *)
                q_get(&(qmi_uim_remote_state_ptr->cmd_q));
  }

  if (cmd_ptr == NULL)
  {
    return;
  }

  switch(cmd_ptr->cmd_enum)
  {
    case QMI_UIM_REMOTE_CMD_REGISTER:
      qmi_uim_remote_process_register_req(cmd_ptr);
      break;

    case QMI_UIM_REMOTE_CMD_RELEASE:
      qmi_uim_remote_process_release_req(cmd_ptr);
      break;

    case QMI_UIM_REMOTE_CMD_CARD_CONNECT:
    case QMI_UIM_REMOTE_CMD_CARD_DISCONNECT:
    case QMI_UIM_REMOTE_CMD_CARD_POWER_DOWN:
    case QMI_UIM_REMOTE_CMD_CARD_POWER_UP:
    case QMI_UIM_REMOTE_CMD_CARD_RESET:
    case QMI_UIM_REMOTE_CMD_SEND_APDU:
      qmi_uim_remote_process_card_op_req(cmd_ptr);
      break;

    default:
      break;
  }

  qmi_uim_remote_cmd_mem_free(cmd_ptr->cmd_enum, cmd_ptr);
  cmd_ptr = NULL;
} /* qmi_uim_remote_process_cmd */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CMD_Q_SIG_HANDLER

  DESCRIPTION:
    QMI UIM REMOTE CMD Q signal handler

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
boolean qmi_uim_remote_cmd_q_sig_handler (
  void
)
{
  ASSERT(qmi_uim_remote_state_ptr != NULL);

  qmi_uim_remote_process_cmd();

  /* This signal is set asyncronously however the framework handles
     concurrency issues so no need to worry about it here */
  if (q_cnt(&qmi_uim_remote_state_ptr->cmd_q) == 0)
  {
    return TRUE;
  }

  return FALSE;
} /* qmi_uim_remote_cmd_q_sig_handler */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_PUT_CMD_IN_Q

   DESCRIPTION:
     This function allows putting of command into the QMI UIM REMOTE CMD queue.
     This is used by the card service (i.e. UIMDRV) to send a request to the
     card client that is off-modem.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
===========================================================================*/
static qmi_uim_remote_result_type qmi_uim_remote_put_cmd_in_q (
  qmi_uim_remote_cmd_type  * qmi_uim_remote_cmd_ptr
)
{
  if (qmi_uim_remote_cmd_ptr == NULL)
  {
    UIM_MSG_LOW_0("Command not accessible for queueing");
    return QMI_UIM_REMOTE_ERROR;
  }

  /* Do not queue command if UIMQMI task is not operational */
  if(uimqmi_task_is_task_operational() == FALSE)
  {
    UIM_MSG_LOW_0("UIMQMI task it not operational so cannot q cmd");
    return QMI_UIM_REMOTE_ERROR;
  }

  if (qmi_uim_remote_state_ptr == NULL)
  {
    UIM_MSG_LOW_0("QMI UIM remote service is not init so cannot q cmd");
    return QMI_UIM_REMOTE_ERROR;
  }

  if (q_cnt(&qmi_uim_remote_state_ptr->cmd_q) >= QMI_UIM_REMOTE_CMD_QUEUE_MAX_SIZE)
  {
    UIM_MSG_LOW_0("Max cmds in qmi uim remote cmd_q");
    return QMI_UIM_REMOTE_ERROR;
  }

  (void)q_link(qmi_uim_remote_cmd_ptr, &qmi_uim_remote_cmd_ptr->link);

  /* Put the message on the queue */
  q_put(&qmi_uim_remote_state_ptr->cmd_q, &qmi_uim_remote_cmd_ptr->link);

  /* Set the command queue signal */
  (void) rex_set_sigs(UIM_UIMQMI_TASK_TCB,
                      UIMQMI_TASK_QMI_UIM_REMOTE_UIM_CMD_Q_SIG);
  UIM_MSG_HIGH_1("Successfully queued qmi uim remote cmd 0x%x",
                 qmi_uim_remote_cmd_ptr->cmd_enum);
  return QMI_UIM_REMOTE_SUCCESS;
} /* qmi_uim_remote_put_cmd_in_q */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_REGISTER

   DESCRIPTION:
     This function registers the calling task with the UIM remote service.
     The function returns immediately and the service sends events with status
     of the connection or slot

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_register (
  qmi_uim_remote_evt_callback_type    * event_cb_ptr,
  qmi_uim_remote_slot_type              slot
)
{
  qmi_uim_remote_cmd_type     * cmd_ptr    = NULL;
  qmi_uim_remote_result_type    cmd_status = QMI_UIM_REMOTE_ERROR;

  if (event_cb_ptr == NULL         ||
      slot < QMI_UIM_REMOTE_SLOT_1 ||
      slot >= QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE)
  {
    UIM_MSG_LOW_0("Remote register input validation failed");
    return cmd_status;
  }

  cmd_ptr =  modem_mem_calloc(1, sizeof(qmi_uim_remote_cmd_type),
                              MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr == NULL)
  {
    UIM_MSG_LOW_0("Remote register malloc failed");
    return cmd_status;
  }

  cmd_ptr->cmd_enum                      = QMI_UIM_REMOTE_CMD_REGISTER;
  cmd_ptr->slot                          = slot;
  cmd_ptr->cmd_data.reg_req.event_cb_ptr = event_cb_ptr;

  cmd_status = qmi_uim_remote_put_cmd_in_q (cmd_ptr);
  if (cmd_status == QMI_UIM_REMOTE_ERROR)
  {
    qmi_uim_remote_cmd_mem_free(QMI_UIM_REMOTE_CMD_REGISTER, cmd_ptr);
  }
  return cmd_status;
} /* qmi_uim_remote_register */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_SEND_APDU

   DESCRIPTION:
     This function allows the calling task to send an APDU via the QMI UIM
     REMOTE service to a card with which it has connected

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_send_apdu (
  qmi_uim_remote_slot_type               slot,
  void *                                 client_ref_ptr,
  uint16                                 cmd_apdu_len,
  const uint8 *                          cmd_apdu_ptr,
  qmi_uim_remote_apdu_callback_type    * rsp_apdu_cb_ptr
)
{
  qmi_uim_remote_cmd_type     * cmd_ptr    = NULL;
  qmi_uim_remote_result_type    cmd_status = QMI_UIM_REMOTE_ERROR;

  if (rsp_apdu_cb_ptr == NULL                                 ||
      slot < QMI_UIM_REMOTE_SLOT_1                            ||
      slot >= QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE              ||
      cmd_apdu_len == 0x0                                     ||
      cmd_apdu_len > QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN_V01  ||
      cmd_apdu_ptr == NULL                                    ||
      rsp_apdu_cb_ptr == NULL)
  {
    UIM_MSG_LOW_2("Remote send apdu input bad, slot 0x%x, apdu len 0x%x",
                  slot, cmd_apdu_len);
    return cmd_status;
  }

  cmd_ptr =  modem_mem_calloc(1, sizeof(qmi_uim_remote_cmd_type),
                              MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr == NULL)
  {
    UIM_MSG_LOW_0("Remote send apdu cmd malloc failed");
    return cmd_status;
  }

  cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_ptr =
    modem_mem_calloc(1, cmd_apdu_len, MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr == NULL)
  {
    UIM_MSG_LOW_0("Remote send apdu cmd data malloc failed");
    return cmd_status;
  }

  cmd_ptr->cmd_enum                              = QMI_UIM_REMOTE_CMD_SEND_APDU;
  cmd_ptr->slot                                  = slot;
  cmd_ptr->cmd_data.send_apdu_req.client_ref_ptr = client_ref_ptr;
  cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_len   = cmd_apdu_len;
  memscpy(cmd_ptr->cmd_data.send_apdu_req.cmd_apdu_ptr, cmd_apdu_len,
          cmd_apdu_ptr, cmd_apdu_len);
  cmd_ptr->cmd_data.send_apdu_req.rsp_apdu_cb_ptr = rsp_apdu_cb_ptr;

  cmd_status = qmi_uim_remote_put_cmd_in_q (cmd_ptr);
  if (cmd_status == QMI_UIM_REMOTE_ERROR)
  {
    qmi_uim_remote_cmd_mem_free(QMI_UIM_REMOTE_CMD_SEND_APDU, cmd_ptr);
  }
  return cmd_status;
} /* qmi_uim_remote_send_apdu */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_SEND_POWER_DOWN_CMD

   DESCRIPTION:
     This function allows the calling task to send a power down cmd via
     the QMI UIM REMOTE service to a card with which it has connected

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
static qmi_uim_remote_result_type qmi_uim_remote_send_power_down_cmd (
  qmi_uim_remote_slot_type                  slot,
  qmi_uim_remote_power_down_mode_enum_type  pdown_mode
)
{
  qmi_uim_remote_cmd_type     * cmd_ptr    = NULL;
  qmi_uim_remote_result_type    cmd_status = QMI_UIM_REMOTE_ERROR;

  UIM_MSG_LOW_0("QMI UIM Remote API call for send power down cmd");

  if (slot < QMI_UIM_REMOTE_SLOT_1                  ||
      slot >=  QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE   ||
      pdown_mode < QMI_UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE ||
      pdown_mode >= QMI_UIM_REMOTE_POWER_DOWN_NOT_APPLICABLE)
  {
    UIM_MSG_LOW_2("Remote pdown cmd for slot 0x%x with power down mode 0x%x failed input validation",
                   slot, pdown_mode);
    return cmd_status;
  }

  cmd_ptr =  modem_mem_calloc(1, sizeof(qmi_uim_remote_cmd_type),
                              MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr == NULL)
  {
    UIM_MSG_LOW_0("Remote pdown cmd malloc failed");
    return cmd_status;
  }

  cmd_ptr->cmd_enum                           = QMI_UIM_REMOTE_CMD_CARD_POWER_DOWN;
  cmd_ptr->slot                               = slot;
  cmd_ptr->cmd_data.power_down_req.pdown_mode = pdown_mode;

  cmd_status = qmi_uim_remote_put_cmd_in_q (cmd_ptr);
  if (cmd_status == QMI_UIM_REMOTE_ERROR)
  {
    qmi_uim_remote_cmd_mem_free(QMI_UIM_REMOTE_CMD_CARD_POWER_DOWN, cmd_ptr);
  }
  return cmd_status;
} /* qmi_uim_remote_send_power_down_cmd */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_SEND_POWER_UP_CMD

   DESCRIPTION:
     This function allows the calling task to send a power up cmd via
     the QMI UIM REMOTE service to a card with which it has connected

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
static qmi_uim_remote_result_type qmi_uim_remote_send_power_up_cmd (
  qmi_uim_remote_slot_type                  slot,
  boolean                                   timeout_isvalid,
  uint32                                    timeout,
  boolean                                   vcc_isvalid,
  qmi_uim_remote_voltage_class_enum_type    vcc
)
{
  qmi_uim_remote_cmd_type     * cmd_ptr    = NULL;
  qmi_uim_remote_result_type    cmd_status = QMI_UIM_REMOTE_ERROR;

  UIM_MSG_LOW_0("QMI UIM Remote API call for send power up cmd");

  if (slot < QMI_UIM_REMOTE_SLOT_1                  ||
      slot >=  QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE   ||
      (timeout_isvalid && timeout == 0)             ||
      (vcc_isvalid &&
       (vcc < QMI_UIM_REMOTE_VOLTAGE_CLASS_C_LOW    ||
        vcc >= QMI_UIM_REMOTE_VOLTAGE_CLASS_NOT_APPLICABLE)))
  {
    UIM_MSG_LOW_3("Remote pup cmd for slot 0x%x with timeout 0x%x, vcc 0x%x failed input validation",
            slot, timeout, vcc);
    return cmd_status;
  }

  cmd_ptr =  modem_mem_calloc(1, sizeof(qmi_uim_remote_cmd_type),
                              MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr == NULL)
  {
    UIM_MSG_LOW_0("Remote cmd malloc failed");
    return cmd_status;
  }

  cmd_ptr->cmd_enum                           = QMI_UIM_REMOTE_CMD_CARD_POWER_UP;
  cmd_ptr->slot                               = slot;
  if (timeout_isvalid)
  {
    cmd_ptr->cmd_data.power_up_req.timeout_isvalid = TRUE;
    cmd_ptr->cmd_data.power_up_req.timeout = timeout;
  }
  if (vcc_isvalid)
  {
    cmd_ptr->cmd_data.power_up_req.vcc_isvalid = TRUE;
    cmd_ptr->cmd_data.power_up_req.vcc = vcc;
  }

  cmd_status = qmi_uim_remote_put_cmd_in_q (cmd_ptr);
  if (cmd_status == QMI_UIM_REMOTE_ERROR)
  {
    qmi_uim_remote_cmd_mem_free(QMI_UIM_REMOTE_CMD_CARD_POWER_UP, cmd_ptr);
  }
  return cmd_status;
} /* qmi_uim_remote_send_power_up_cmd */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_SLOT_CMD

   DESCRIPTION:
     This is a common function used by all the APIs that only pass in slot
     parameter

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
static qmi_uim_remote_result_type qmi_uim_remote_slot_cmd (
  qmi_uim_remote_slot_type            slot,
  qmi_uim_remote_cmd_enum_type        cmd_enum
)
{
  qmi_uim_remote_cmd_type     * cmd_ptr    = NULL;
  qmi_uim_remote_result_type    cmd_status = QMI_UIM_REMOTE_ERROR;

  UIM_MSG_LOW_1("QMI UIM Remote API call for cmd 0x%x", cmd_enum);

  if (cmd_enum < QMI_UIM_REMOTE_CMD_REGISTER        ||
      cmd_enum >= QMI_UIM_REMOTE_CMD_NOT_APPLICABLE ||
      slot < QMI_UIM_REMOTE_SLOT_1                  ||
      slot >=  QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE)
  {
    UIM_MSG_LOW_2("Remote cmd 0x%x for slot 0x%x failed input validation",
                  cmd_enum, slot);
    return cmd_status;
  }

  cmd_ptr =  modem_mem_calloc(1, sizeof(qmi_uim_remote_cmd_type),
                              MODEM_MEM_CLIENT_UIM);
  if (cmd_ptr == NULL)
  {
    UIM_MSG_LOW_1("Remote cmd 0x%x malloc failed", cmd_enum);
    return cmd_status;
  }

  cmd_ptr->cmd_enum                  = cmd_enum;
  cmd_ptr->slot                      = slot;

  cmd_status = qmi_uim_remote_put_cmd_in_q (cmd_ptr);
  if (cmd_status == QMI_UIM_REMOTE_ERROR)
  {
    qmi_uim_remote_cmd_mem_free(cmd_enum, cmd_ptr);
  }
  return cmd_status;
} /* qmi_uim_remote_slot_cmd */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_RELEASE

   DESCRIPTION:
     This function un-registers the calling task from the QMI UIM REMOTE
     service.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_release (
  qmi_uim_remote_slot_type            slot
)
{
  return qmi_uim_remote_slot_cmd(slot, QMI_UIM_REMOTE_CMD_RELEASE);
} /* qmi_uim_remote_release */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_CONNECT

   DESCRIPTION:
     This function connects the calling task with a card on a particular slot
     via the QMI UIM REMOTE service. As part of connect the card may be
     powered up and the calling task may receive a remote card event

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_connect (
  qmi_uim_remote_slot_type            slot
)
{
  return qmi_uim_remote_slot_cmd(slot, QMI_UIM_REMOTE_CMD_CARD_CONNECT);
} /* qmi_uim_remote_card_connect */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_DISCONNECT

   DESCRIPTION:
     This function disconnects the calling task from a card with which it was
     previously connected via the QMI UIM REMOTE service.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_disconnect (
  qmi_uim_remote_slot_type            slot
)
{
  return qmi_uim_remote_slot_cmd(slot, QMI_UIM_REMOTE_CMD_CARD_DISCONNECT);
} /* qmi_uim_remote_card_disconnect */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_POWER_DOWN

   DESCRIPTION:
     This function allows the calling task to power down (i.e. drive VCC low)
     a card with which it has previously connected via the QMI UIM REMOTE
     service.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_power_down (
  qmi_uim_remote_slot_type                  slot,
  qmi_uim_remote_power_down_mode_enum_type  pdown_mode
)
{
  return qmi_uim_remote_send_power_down_cmd(slot,
                                            pdown_mode);
} /* qmi_uim_remote_card_power_down */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_POWER_UP

   DESCRIPTION:
     This function allows the calling task to power up (i.e. drive VCC high)
     a card with which it has previously connected via the QMI UIM REMOTE
     service.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_power_up (
  qmi_uim_remote_slot_type                slot,
  boolean                                 timeout_isvalid,
  uint32                                  timeout,
  boolean                                 vcc_isvalid,
  qmi_uim_remote_voltage_class_enum_type  vcc
)
{
  return qmi_uim_remote_send_power_up_cmd( slot,
                                           timeout_isvalid,
                                           timeout,
                                           vcc_isvalid,
                                           vcc);
} /* qmi_uim_remote_card_power_up */


/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_RESET

   DESCRIPTION:
     This function allows the calling task to reset (i.e. toggle RST line)
     a card with which it has previously connected via the QMI UIM REMOTE
     service.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     qmi_uim_remote_result_type

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_reset (
  qmi_uim_remote_slot_type            slot
)
{
  return qmi_uim_remote_slot_cmd(slot, QMI_UIM_REMOTE_CMD_CARD_RESET);
} /* qmi_uim_remote_card_reset */


/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_DEREG

  DESCRIPTION
    Called to deregister the QMI UIM Remote as QMI service and as SAP client on
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
void qmi_uim_remote_dereg(
  void
)
{
  if (qmi_uim_remote_state_ptr == NULL)
  {
    return;
  }

  (void)qmi_csi_unregister (qmi_uim_remote_state_ptr->service_handle);

  (void)qmi_sap_deregister (qmi_uim_remote_state_ptr->sap_handle);
}/* qmi_uim_remote_dereg */

#endif /* FEATURE_UIM_REMOTE_UIM */

