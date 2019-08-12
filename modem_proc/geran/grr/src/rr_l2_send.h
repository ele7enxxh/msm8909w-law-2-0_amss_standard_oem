#ifndef RR_L2_SEND_H
#define RR_L2_SEND_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_l2_send.h

GENERAL DESCRIPTION
   This module contains functions that send messages from rr to layer 2.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_l2_send.h_v   1.1   16 May 2002 10:53:10   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_l2_send.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/29/02   Rao     Added suspend reason in dl_suspend_req for InterRAT HO
05/15/02   JAC     Added rr_send_mdl_release_req

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "rr_l2.h"
#include "rr_l2_g.h"
#include "geran_multi_sim.h"

/*===========================================================================

                              CONSTANTS

===========================================================================*/
#define RR_STATUS_LEN 3

#define RR_FIRST_SAPI0_MSG_ID (MAX_NAS_MESSAGE_BUFFER_SIZE+2) /* beyond NAS */
#define RR_LAST_SAPI0_MSG_ID   0xfe

#ifdef FEATURE_GSM_DTM
#define RR_MSG_ID_GTTP         0xff
#endif // FEATURE_GSM_DTM

/* add definitions here for RR signalling messages where an ack is required */
#define RR_MSG_ID_CIPHER_MODE_COMPLETE (RR_FIRST_SAPI0_MSG_ID + 0)

/*===========================================================================

                               MACROS

===========================================================================*/
#define IS_RR_SIGNALLING_MSG(id) (((id) == 0) || \
                                  (((id) >= RR_FIRST_SAPI0_MSG_ID) && (id) <= RR_LAST_SAPI0_MSG_ID))
#define IS_RR_SIGNALLING_ACK(sap,id) (((sap) == SAPI0) && IS_RR_SIGNALLING_MSG(id))

#ifdef FEATURE_GSM_DTM
#define IS_GTTP_SIGNALLING_ACK(sap,id) (((sap) == SAPI0) && (id == RR_MSG_ID_GTTP))
#endif // FEATURE_GSM_DTM

typedef enum
{
  RR_L2_RECONNECT_REQUIRED,
  RR_L2_RESUME_REQUIRED,
  RR_L2_RESUME_REQUIRED_L1_ALREADY_CONNECTED,
} rr_l2_resumption_method_t;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*
Sends a L3 message using a DL_DATA_REQ to L2
*/
void rr_l2_send_l3_msg_ota(
  byte *l3_msg,
  uint8 l3_msg_len,
  const gas_id_t gas_id
);

/*
Sends DL_RESUME_REQ to L2 with a L3 message payload. Also takes the parameter
'l1_already_connected' which tells L2 whether to wait for a CONNECT_IND from L1 or not
(this is used in DTM)
*/
void rr_l2_resume_l2_and_send_l3_msg_ota(
  byte *l3_msg,
  uint8 l3_msg_len,
  boolean l1_already_connected,
  const gas_id_t gas_id
);

/*
Sends DL_RECONNECT_REQ to L2 with a L3 message payload
*/
void rr_l2_reconnect_l2_and_send_l3_msg_ota(
  byte *l3_msg,
  uint8 l3_msg_len,
  const gas_id_t gas_id
);

/*
Alternative generic function, which will send DL_RECONNECT_REQ or DL_RESUME_REQ, and
set l1_already_connected, according to the 'method' enum parameter passed in
*/
void rr_l2_restart_l2_and_send_l3_msg_ota(
  rr_l2_resumption_method_t method,
  byte *l3_msg,
  uint8 l3_msg_len,
  const gas_id_t gas_id
);

byte rr_send_dl_data_req(
  byte *layer3_message,
  byte length,
  SAPI_T SAPI,
  l2_channel_type_T l2_channel_type,
  const gas_id_t gas_id
);

void rr_send_status(
  byte RR_cause,
  const gas_id_t gas_id
);

void rr_send_dl_reconnect_req(
  byte *layer3_message,
  byte message_length,
  const gas_id_t gas_id
);

void rr_send_dl_resume_req(
  rr_l1_handover_type_T resume_reason,
  byte *layer3_message,
  byte message_length,
  const gas_id_t gas_id
);

void rr_send_dl_random_access_req(
  byte access_reference,
  l2_channel_type_T l2_channel_type,
  const gas_id_t gas_id
);

void rr_send_dl_random_access_req_for_egprs(
  word egprs_access_reference,
  l2_channel_type_T l2_channel_type,
  const gas_id_t gas_id
);

void rr_send_dl_suspend_req(
  rr_l1_handover_type_T suspend_reason,
  const gas_id_t gas_id
);

void rr_send_dl_release_req(
  release_mode_T release_mode,
  byte SAPI,
  byte l2_channel_type,
  const gas_id_t gas_id
);

void rr_send_mdl_release_req(
  byte SAPI,
  byte l2_channel_type,
  const gas_id_t gas_id
);


void rr_send_dl_establish_req(
  SAPI_T SAPI,
  l2_channel_type_T l2_channel_type,
  byte *layer3_message,
  byte message_length,
  establish_mode_T establish_mode,
  const gas_id_t gas_id
);

void rr_send_dl_unit_data_req(
  byte *layer3_message,
  byte message_length,
  byte SAPI,
  byte l2_channel_type,
  boolean short_header,
  const gas_id_t gas_id
);

boolean rr_send_signalling_msg_with_ack(
  byte *layer3_message,
  byte length,
  byte msg_id,
  const gas_id_t gas_id
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
void rr_send_dl_data_mux_ind(
  l2_mux_cb_fn_t ccch_callback,
  l2_mux_cb_fn_t cbch_callback,
  const gas_id_t gas_id
);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#endif
