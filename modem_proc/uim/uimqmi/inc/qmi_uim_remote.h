#ifndef QMI_UIM_REMOTE_H
#define QMI_UIM_REMOTE_H

/*===========================================================================

                         Q M I _ U I M _ R E M O T E . H

DESCRIPTION

 This header file defines data types and functions necessary to access a SIM
 attached to a remote device such as an applications processor or another modem
 via the QMI interface.

                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/inc/qmi_uim_remote.h#1 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/16/16    bcho   Deregister qmi uim remote service on task stop
11/25/15    sp     Updated copyright
05/07/14    tkl    Add Vcc and response_timeout for power up request
03/16/14    tkl    Add power down mode for power down request
12/11/13    vs     Addition of UIMQMI task and QMI UIM REMOTE

===========================================================================*/

/*=============================================================================

                   I N C L U D E S

=============================================================================*/
#include "comdef.h"
#include "qmi_csi.h"
#include "qmi_sap.h"
#include "queue.h"

/*=============================================================================

                   D E F I N E S

=============================================================================*/
#define QMI_UIM_REMOTE_MAX_ATR_LEN               32
#define QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT       5
#define QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE      5

/*=============================================================================

                   E N U M E R A T E D   D A T A

=============================================================================*/

/*===========================================================================
   ENUM:      QMI_UIM_REMOTE_RESULT_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_REMOTE_SUCCESS    = 0x0,
  QMI_UIM_REMOTE_ERROR
} qmi_uim_remote_result_type;

/*===========================================================================
   ENUM:      QMI_UIM_REMOTE_SLOT_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_REMOTE_SLOT_1              = 0x0,
  QMI_UIM_REMOTE_SLOT_2,
  QMI_UIM_REMOTE_SLOT_3,
  QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE = 0x7FFFFFFF
} qmi_uim_remote_slot_type;

/*===========================================================================
   ENUM:      QMI_UIM_REMOTE_EVT_ENUM_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_REMOTE_EVT_NOT_RECEIVED            = 0x0,
  QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE,
  QMI_UIM_REMOTE_EVT_CONNECTION_AVAILABLE,
  QMI_UIM_REMOTE_EVT_CARD_INSERTED,
  QMI_UIM_REMOTE_EVT_CARD_REMOVED,
  QMI_UIM_REMOTE_EVT_CARD_ERROR,
  QMI_UIM_REMOTE_EVT_CARD_RESET,
  QMI_UIM_REMOTE_EVT_CARD_WAKEUP,
  QMI_UIM_REMOTE_EVT_NOT_APPLICABLE          = 0x7FFFFFFF
} qmi_uim_remote_evt_enum_type;

/*===========================================================================
   ENUM:      QMI_UIM_REMOTE_CARD_ERROR_ENUM_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_REMOTE_CARD_ERROR_UNKNOWN_ERROR              = 0x0,
  QMI_UIM_REMOTE_CARD_ERROR_NO_LINK_ESTABLISHED        = 0x1,
  QMI_UIM_REMOTE_CARD_ERROR_COMMAND_TIMEOUT            = 0x2,
  QMI_UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN          = 0x3,
  QMI_UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN_TELECOM  = 0x04,
  QMI_UIM_REMOTE_CARD_ERROR_NOT_APPLICABLE             = 0x7FFFFFFF
} qmi_uim_remote_card_error_enum_type;

/*===========================================================================
   ENUM:      QMI_UIM_REMOTE_CMD_ENUM_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_REMOTE_CMD_REGISTER         = 0x0,
  QMI_UIM_REMOTE_CMD_RELEASE,
  QMI_UIM_REMOTE_CMD_CARD_CONNECT,
  QMI_UIM_REMOTE_CMD_CARD_DISCONNECT,
  QMI_UIM_REMOTE_CMD_SEND_APDU,
  QMI_UIM_REMOTE_CMD_CARD_POWER_DOWN,
  QMI_UIM_REMOTE_CMD_CARD_POWER_UP,
  QMI_UIM_REMOTE_CMD_CARD_RESET,
  QMI_UIM_REMOTE_CMD_NOT_APPLICABLE   = 0x7FFFFFFF
} qmi_uim_remote_cmd_enum_type;

/*===========================================================================
   ENUM:      QMI_UIM_REMOTE_POWER_DOWN_MODE_ENUM_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE   = 0x0,
  QMI_UIM_REMOTE_POWER_DOWN_CARD,
  QMI_UIM_REMOTE_POWER_DOWN_NOT_APPLICABLE      = 0x7FFFFFFF
} qmi_uim_remote_power_down_mode_enum_type;

/*===========================================================================
   ENUM:      QMI_UIM_REMOTE_VOLTAGE_CLASS_ENUM_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_REMOTE_VOLTAGE_CLASS_C_LOW           = 0x0,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_C,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_C_HIGH,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_B_LOW,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_B,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_B_HIGH,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_NOT_APPLICABLE   = 0x7FFFFFFF
} qmi_uim_remote_voltage_class_enum_type;

/*=============================================================================

      S T R U C T    A N D    C A L L B A C K   D A T A    T Y P E S

=============================================================================*/

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_CARD_ATR_DATA_TYPE
=============================================================================*/
typedef struct
{
  uint8              data_len;
  uint8 *            data_ptr;
}qmi_uim_remote_card_atr_data_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_CARD_INSERTED_EVT_TYPE
=============================================================================*/
typedef struct
{
  qmi_uim_remote_card_atr_data_type    atr;
}qmi_uim_remote_card_inserted_evt_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_CARD_RESET_EVT_TYPE
=============================================================================*/
typedef struct
{
  qmi_uim_remote_card_atr_data_type    atr;
}qmi_uim_remote_card_reset_evt_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_CARD_ERROR_EVT_TYPE
=============================================================================*/
typedef struct
{
  qmi_uim_remote_card_error_enum_type    err_cause;
}qmi_uim_remote_card_error_evt_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_EVENT_DATA_TYPE
=============================================================================*/
typedef struct
{
  qmi_uim_remote_evt_enum_type    evt;
  qmi_uim_remote_slot_type        slot;
  boolean                         wakeup_support;
  union {
    qmi_uim_remote_card_inserted_evt_type    card_inserted;
    qmi_uim_remote_card_reset_evt_type       card_reset;
    qmi_uim_remote_card_error_evt_type       card_error;
  } evt_data;
} qmi_uim_remote_event_data_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_RSP_APDU_DATA_TYPE
=============================================================================*/
typedef struct
{
  qmi_uim_remote_result_type    result;
  qmi_uim_remote_slot_type      slot;
  void *                        client_ref_ptr;
  uint16                        rsp_apdu_len;
  uint8 *                       rsp_apdu_ptr;
} qmi_uim_remote_rsp_apdu_data_type;

/*===========================================================================
   FUNCTION PROTOTYPE:      QMI_UIM_REMOTE_EVT_CALLBACK_TYPE
=============================================================================*/
typedef void qmi_uim_remote_evt_callback_type
(
  const qmi_uim_remote_event_data_type    * evt_ptr
);

/*===========================================================================
   FUNCTION PROTOTYPE:      QMI_UIM_REMOTE_APDU_CALLBACK_TYPE
=============================================================================*/
typedef void qmi_uim_remote_apdu_callback_type
(
  const qmi_uim_remote_rsp_apdu_data_type    * rsp_apdu_ptr
);

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_CMD_TYPE
=============================================================================*/
typedef struct {
  q_link_type                     link;
  qmi_uim_remote_slot_type        slot;
  qmi_uim_remote_cmd_enum_type    cmd_enum;
  union
  {
    struct
    {
      qmi_uim_remote_evt_callback_type       * event_cb_ptr;
    } reg_req;
    struct
    {
      void                                   * client_ref_ptr;
      uint16                                   cmd_apdu_len;
      uint8                                  * cmd_apdu_ptr;
      qmi_uim_remote_apdu_callback_type      * rsp_apdu_cb_ptr;
    } send_apdu_req;
    struct
    {
      qmi_uim_remote_power_down_mode_enum_type pdown_mode;
    } power_down_req;
    struct
    {
      boolean                                  timeout_isvalid;
      uint32                                   timeout;
      boolean                                  vcc_isvalid;
      qmi_uim_remote_voltage_class_enum_type   vcc;
    } power_up_req;
  }cmd_data;
}qmi_uim_remote_cmd_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_LAST_APDU_INFO_TYPE
=============================================================================*/
typedef struct
{
  uint8                                id;
  qmi_uim_remote_apdu_callback_type  * rsp_apdu_cb_ptr;
  qmi_uim_remote_rsp_apdu_data_type    rsp_apdu_data;
} qmi_uim_remote_curr_apdu_info_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_CARD_CLIENT_INFO_TYPE
=============================================================================*/
typedef struct
{
  boolean                             in_use;
  qmi_client_handle                   client_handle;
  qmi_uim_remote_slot_type            slot;
  qmi_uim_remote_event_data_type      last_evt_data;
  qmi_uim_remote_curr_apdu_info_type  curr_apdu;
} qmi_uim_remote_card_client_info_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_CARD_SERVICE_INFO_TYPE
=============================================================================*/
typedef struct
{
  boolean                               in_use;
  qmi_uim_remote_evt_callback_type    * event_cb_ptr;
  qmi_uim_remote_slot_type              slot;
} qmi_uim_remote_card_service_info_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_REMOTE_STATE_TYPE
=============================================================================*/
typedef struct {
  qmi_csi_service_handle                   service_handle;
  qmi_sap_client_handle                    sap_handle;
  q_type                                   cmd_q;
  qmi_uim_remote_card_client_info_type     card_client_registry[QMI_UIM_REMOTE_MAX_NUM_CARD_CLIENT];
  qmi_uim_remote_card_service_info_type    card_service_registry[QMI_UIM_REMOTE_MAX_NUM_CARD_SERVICE];
}qmi_uim_remote_state_type;

/*=============================================================================

                   F U N C T I O N S

=============================================================================*/

/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_REGISTER

   DESCRIPTION:
     This function registers the calling task with the UIM remote service.
     It also registers the callback for events. The function returns
     immediately and the service sends events with status of the connection
     or slot

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_register (
  qmi_uim_remote_evt_callback_type    * event_cb_ptr,
  qmi_uim_remote_slot_type            slot
);

/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_RELEASE

   DESCRIPTION:
     This function releases the calling task from the UIM remote service

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_release (
  qmi_uim_remote_slot_type    slot
);

/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_CONNECT

   DESCRIPTION:
     This function initiates the establishment of a connection with the remote
     SIM and powers it up

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_connect (
  qmi_uim_remote_slot_type    slot
);

/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_DISCONNCET

   DESCRIPTION:
     This function releases the calling task from the UIM remote service

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_disconnect (
  qmi_uim_remote_slot_type    slot
);

/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_SEND_APDU

   DESCRIPTION:
     This function transmits an APDU from the calling task to the remote SIM
     card. For each APDU, the calling task can indicate a response callback,
     used to receive the response from the SIM card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_send_apdu (
  qmi_uim_remote_slot_type               slot,
  void *                                 client_ref_ptr,
  uint16                                 cmd_apdu_len,
  const uint8 *                          cmd_apdu_ptr,
  qmi_uim_remote_apdu_callback_type    * rsp_apdu_cb_ptr
);

/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_POWER_DOWN

   DESCRIPTION:
     This function powers down the remote SIM card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_power_down (
  qmi_uim_remote_slot_type                  slot,
  qmi_uim_remote_power_down_mode_enum_type  pdown_mode
);

/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_POWER_UP

   DESCRIPTION:
     This function powers up the remote SIM card.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_power_up (
  qmi_uim_remote_slot_type                  slot,
  boolean                                   timeout_isvalid,
  uint32                                    timeout,
  boolean                                   vcc_isvalid,
  qmi_uim_remote_voltage_class_enum_type    vcc
);

/*===========================================================================
   FUNCTION:      QMI_UIM_REMOTE_CARD_RESET

   DESCRIPTION:
     This function can be used by the calling task to perform a warm reset
     of the remote SIM card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_remote_result_type qmi_uim_remote_card_reset (
  qmi_uim_remote_slot_type    slot
);

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
);

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
void qmi_uim_remote_service_init(
  rex_tcb_type    * tcb_ptr,
  rex_sigs_type     sig
);

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
);

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
);

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
 ); 
#endif /* QMI_UIM_REMOTE_H */

