#ifndef UIM_REMOTE_H
#define UIM_REMOTE_H

/*===========================================================================

                         U I M _ R E M O T E . H

DESCRIPTION

 This header file defines data types and functions necessary to access a SIM
 attached to a remote device such as an applications processor or another modem
 via the QMI interface.

Copyright (c) 2014-2016 by QUALCOMM Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_remote.h#1 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/10/16   ks      Adding report call bcak for remote/local cards power-up
01/25/16   ssr     Delay local card power up in Modem SSR in BTSAP mode
09/08/15    ll     UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
02/03/14   sam     Usage of newer style diag macros
12/12/13   ks      BTSAP in client mode, intial implementation
===========================================================================*/

/*=============================================================================

                   I N C L U D E S

=============================================================================*/

#include "uimi.h"
#include "uimgen.h"
#include "uimglobals.h"
#include "qmi_uim_remote.h"
#include "uimdrv_msg.h"
#include "uim_p.h"


#define UIM_REMOTE_APDU_HDR_SIZE                    5
#define UIM_REMOTE_SW1_SW2_LENGTH                   2
#define UIM_REMOTE_MAX_RAPDU_LENGTH                 256

/* Masks in BTSAP handle for various indications */
/* Indicates if UIM received connection request from QMI REMOTE client */
#define UIM_BTSAP_ACTIVE_ENABLE_MASK   0x01
#define UIM_BTSAP_ACTIVE_DISABLE_MASK  0xFE

/* Indicates if connection with remote card is established */
#define UIM_BTSAP_CARD_CONNCECTTION_ENABLE_MASK   0x02
#define UIM_BTSAP_CARD_CONNCECTTION_DISABLE_MASK  0xFD

/* Indicates if recovery needs to be triggered */
#define UIM_BTSAP_TRIGGER_RECOVERY_MASK  0x04
#define UIM_BTSAP_DISABLE_RECOVERY_MASK  0xFB


/* Indicates if connection unavailable is requested with remote card */
#define UIM_BTSAP_CARD_CONN_UNAVAIL_ENABLE_MASK  0x08
#define UIM_BTSAP_CARD_CONN_UNAVAIL_DISABLE_MASK  0xF7

/* BTSAP Transaction timer default value - 7000ms */
#define UIM_BTSAP_TRANSACTION_TIME_DURATION 7000

/* Macro to check if UIM is in BTSAP client mode */
#define IS_BTSAP_HANDLE_ACTIVE(uim_ptr) (uim_ptr->remote.btsap.handle & UIM_BTSAP_ACTIVE_ENABLE_MASK)

/* UIM btsap Status word data type */
typedef struct {
  uint8                      sw1;             /* SW1 */
  uint8                      sw2;             /* SW2 */
  boolean                    def_sw_proc;     /* Default SW2 Handler Flag */
  uim_apdu_status_type       cmd_status;      /* Command response status  */
} uim_remote_sw_data_type;

extern void uim_remote_card_connect
(
  uim_instance_global_type *uim_ptr
);

extern void uim_remote_card_power_up
(
  uim_instance_global_type *uim_ptr
);

extern void uim_remote_card_power_down
(
  uim_instance_global_type *uim_ptr
);

extern void uim_remote_card_reset
(
  uim_instance_global_type *uim_ptr
);

extern void uim_remote_send_command
(
  uim_req_buf_type *uim_req_ptr , uim_instance_global_type *uim_ptr 
);

extern void uim_remote_cmd_rsp_timer_expiry_cb
(
  timer_cb_data_type param
);

extern void uim_btsap_send_power_down_local_sim
(
  uim_instance_global_type *uim_ptr
);

extern void uim_btsap_power_down_local_sim_cb
( 
  uim_rpt_type *report_ptr
);

extern void uim_btsap_send_power_down_remote_sim
(
  uim_instance_global_type *uim_ptr
);

extern void uim_btsap_power_down_remote_sim_cb
(
  uim_rpt_type *report_ptr
);

extern void uim_btsap_report_cb
(
  uim_rpt_type *report_ptr
);


extern void uim_remote_send_atr
(
  uim_instance_global_type *uim_ptr,
  const qmi_uim_remote_card_atr_data_type *atr_data_ptr
);

extern void uim_btsap_card_inserted_req
(
  uim_instance_global_type *uim_ptr
);

extern void uim_btsap_card_removed_req
(
  uim_instance_global_type *uim_ptr
);

extern void uim_btsap_free_cmd_buffer
(
  uim_rpt_type *report_ptr
);

extern void uim_remote_apdu_response_cb
(
  const qmi_uim_remote_rsp_apdu_data_type *remote_rsp_apdu_ptr
);

extern void uim_btsap_event_callback
(
  const qmi_uim_remote_event_data_type *evt_ptr
);

extern uint16 uim_btsap_get_work_waiting_time
(
  uim_instance_global_type *uim_ptr
);

extern void uim_remote_get_command_status
(
   uim_instance_global_type *uim_ptr
);

extern boolean uim_remote_get_entire_response
(
  uim_instance_global_type *uim_ptr
);
extern void uim_btsap_init
(
  uim_instance_global_type *uim_ptr
);

extern void uim_btswap_save_current_mode_to_efs 
(
  uim_instance_global_type *uim_ptr ,  
  uim_slot_connection_enum_type uim_mode
);

#endif /* UIM_REMOTE_H */
