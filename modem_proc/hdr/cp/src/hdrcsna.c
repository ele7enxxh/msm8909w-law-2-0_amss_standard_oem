/*===========================================================================

                  H D R   C I R C U I T   S E R V I C E S   

              N O T I F I C A T I O N   A P P L I C A T I O N



DESCRIPTION
  This file implements the Circuit Services Notification Protocol.

EXTERNALIZED FUNCTIONS (Global)
  hdrcsna_send_3g1x_msg - Send a 3G1X Message
  hdrcsna_msg_cb - Callback function that queues incoming messages for this 
                  protocol
  hdrcsna_init - Initializes the HDRCSNA protocol

EXTERNALIZED FUNCTIONS (Regional)
  hdrcsna_process_msg - Process message received for the protocol
  hdrcsna_process_cmd - Process command received for the protocol
  hdrcsna_process_timer - Process timer expiration event for the protocol

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.


Copyright (c)2006 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrcsna.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/14   cnx     Support NV refresh without reset.
06/13/13   cnx     Added sanity check on CSNA 3G1X SRV msg length.
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
12/13/10   cnx     Added sanity check on the PDU length of 3G1XServices message.
09/30/09   sju     Fixed KW warnings.
09/15/09   kss     Use new PACK() macro for Q6 compatibility.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
12/10/08   smd     Replaced assert.h with amssassert.h
08/03/07   wsh     Fixed compile error.
07/31/07   wsh     Fixed error so that 3G1XParameters are incorrectly saved 
                   when signature not matching RouteUpdateTriggerCode.
06/11/07   yll     Removed hdrrxi.h from the include files.
04/06/07   yll     Start quick repeat timer after message transmission
                   outcome callback.
03/05/07   yll     Removed the 3G1XServices duplicate detection.
01/17/07   yll     Changes for HOMER interface.
01/08/07   yll     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "cmxll.h"
#include "hdrhai.h"
#include "task.h"
#include "msg.h"
#include "err.h"
#include "hdrhmp.h"
#include "hdrmci.h"
#include "hdrbit.h"
#include "hdrstream.h"
#include "amssassert.h"
#include "hdrtrace.h"
#include "hdrerrno.h"
#include "hdrdebug.h"
#include "hdrutil.h"
#include "hdrscmdb.h"
#include "hdrovhd.h"
#include "hdrrup.h"
#include "hdrcsna.h"
#include "hdrscmcsna.h"

#ifdef FEATURE_HDR_CSNA
/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRCSNA_TASK_PTR hdrmc_tcb_ptr

/* Message priority. */
#define HDRCSNA_MSG_PRI 40

/*---------------------------------------------------------------------------
                            PROTOCOL MESSAGE HEADER                      
---------------------------------------------------------------------------*/

/* Message ID values */
#define HDRCSNA_3G1X_SERVICES_ACK_MSG_ID      0x00
#define HDRCSNA_3G1X_PARMS_MSG_ID             0x01
#define HDRCSNA_3G1X_SERVICES_MSG_ID          0x02
#define HDRCSNA_3G1X_PARMS_RESET_MSG_ID       0x03
#define HDRCSNA_3G1X_PARMS_RESET_ACK_MSG_ID   0x04
#define HDRCSNA_3G1X_PARMS_ACK_MSG_ID         0x05

/*---------------------------------------------------------------------------
  3G1X Messages and their unpack/pack routines
  Unpack and pack routines take a pointer to a DSM item and a 
  reference to a variable of the appropriate type
---------------------------------------------------------------------------*/
#define HDRCSNA_UNPACK_MSG_ID( item_ptr, msg_id) \
          (HDRBIT_UNPACK8((item_ptr), (msg_id), 0, 8))
#define HDRCSNA_UNPACK_TRANS_ID( item_ptr, trans_id) \
          (HDRBIT_UNPACK8((item_ptr), (trans_id), 8, 8))

#define HDRCSNA_PACK_MESSAGE_ID( item_ptr, msg_id ) \
               HDRBIT_PACK8((item_ptr), (msg_id), 0, 8 )
#define HDRCSNA_PACK_TRANS_ID( item_ptr, trans_id) \
          (HDRBIT_PACK8((item_ptr), (trans_id), 8, 8))

/********************   3G1X Services Message     ************************
 *
 *       Field         Length (bits)      Offset
 *     MessageID            8               0
 *     AckRequired          1               8
 *     MessageSequence      6               9
 *     3G1XLogicalChannel   1               15
 *     ProtocolRevisioin    8               16
 *     PDULength            16              24
 *     PDU                  PDULength x 8   40
 */
/* 3G1X Services Message external format. It is used for calculating
 * offset and size for each message fields
 */
typedef PACK(struct)
{
  uint8     msg_id[8];
  uint8     ack_req[1];
  uint8     msg_seq[6];
  uint8     log_ch_3g1x[1];
  uint8     p_rev[8];
  uint8     pdu_len[16];
  uint8     pdu[1];     /* Just the first bit for calculating offsets */
} hdrcsna_3g1x_services_ext_type;

/* The max length of 3G1X Services Message */
#define HDRCSNA_3G1X_MAX_PDU_LEN  HOMER_MAX_PDU_LENGTH

/* The max value of a MessageSequence Field */
#define HDRCSNA_MAX_3G1X_SRV_MSG_SEQ  0x3F
#define HDRCSNA_INVALID_3G1X_SRV_MSG_SEQ  (HDRCSNA_MAX_3G1X_SRV_MSG_SEQ + 1)

/* 3G1X Services Message internal format */
typedef struct
{
  boolean   ack_req;
  uint8     msg_seq;
  boolean   log_ch_3g1x;
  uint8     p_rev;
  uint16    pdu_len;
  uint8     pdu[HDRCSNA_3G1X_MAX_PDU_LEN];

} hdrcsna_3g1x_services_type;

/* Unpacking Routines */
#define HDRCSNA_UNPACK_ACK_REQUIRED( item_ptr, val_ptr) \
          (HDRBIT_UNPACK8((item_ptr), (val_ptr), \
             FPOS(hdrcsna_3g1x_services_ext_type, ack_req), \
             FSIZ(hdrcsna_3g1x_services_ext_type, ack_req)))
#define HDRCSNA_UNPACK_MESSAGE_SEQUENCE( item_ptr, val_ptr) \
          (HDRBIT_UNPACK8((item_ptr), (val_ptr), \
             FPOS(hdrcsna_3g1x_services_ext_type, msg_seq), \
             FSIZ(hdrcsna_3g1x_services_ext_type, msg_seq)))
#define HDRCSNA_UNPACK_3G1X_LOGICAL_CHANNEL( item_ptr, val_ptr) \
          (HDRBIT_UNPACK8((item_ptr), (val_ptr), \
             FPOS(hdrcsna_3g1x_services_ext_type, log_ch_3g1x), \
             FSIZ(hdrcsna_3g1x_services_ext_type, log_ch_3g1x)))
#define HDRCSNA_UNPACK_PROTOCOL_REVISION( item_ptr, val_ptr) \
          (HDRBIT_UNPACK8((item_ptr), (val_ptr), \
             FPOS(hdrcsna_3g1x_services_ext_type, p_rev), \
             FSIZ(hdrcsna_3g1x_services_ext_type, p_rev)))
#define HDRCSNA_UNPACK_PDU_LENGTH( item_ptr, val_ptr) \
          (HDRBIT_UNPACK16((item_ptr), (val_ptr), \
             FPOS(hdrcsna_3g1x_services_ext_type, pdu_len), \
             FSIZ(hdrcsna_3g1x_services_ext_type, pdu_len)))
#define HDRCSNA_UNPACK_PDU(item_ptr, val_ptr, pdu_len) \
           if ((pdu_len) != \
                dsm_extract((item_ptr), \
                            (FPOS(hdrcsna_3g1x_services_ext_type, pdu)/8), \
                            (val_ptr), (pdu_len))) \
           { \
             HDR_MSG_PROT_1(MSG_LEGACY_ERROR, \
                            "CSNA: Problem unpack 3G1X Services Message pdu_len=%d", \
                            (pdu_len)); \
           }

/* Packing Routines */
#define HDRCSNA_PACK_ACK_REQUIRED( item_ptr, val) \
          (HDRBIT_PACK8((item_ptr), (val), \
             FPOS(hdrcsna_3g1x_services_ext_type, ack_req), \
             FSIZ(hdrcsna_3g1x_services_ext_type, ack_req)))
#define HDRCSNA_PACK_MESSAGE_SEQUENCE( item_ptr, val) \
          (HDRBIT_PACK8((item_ptr), (val), \
             FPOS(hdrcsna_3g1x_services_ext_type, msg_seq), \
             FSIZ(hdrcsna_3g1x_services_ext_type, msg_seq)))
#define HDRCSNA_PACK_3G1X_LOGICAL_CHANNEL( item_ptr, val) \
          (HDRBIT_PACK8((item_ptr), (val), \
             FPOS(hdrcsna_3g1x_services_ext_type, log_ch_3g1x), \
             FSIZ(hdrcsna_3g1x_services_ext_type, log_ch_3g1x)))
#define HDRCSNA_PACK_PROTOCOL_REVISION( item_ptr, val) \
          (HDRBIT_PACK8((item_ptr), (val), \
             FPOS(hdrcsna_3g1x_services_ext_type, p_rev), \
             FSIZ(hdrcsna_3g1x_services_ext_type, p_rev)))
#define HDRCSNA_PACK_PDU_LENGTH( item_ptr, val) \
          (HDRBIT_PACK16((item_ptr), (val), \
             FPOS(hdrcsna_3g1x_services_ext_type, pdu_len), \
             FSIZ(hdrcsna_3g1x_services_ext_type, pdu_len)))
#define HDRCSNA_PACK_PDU(item_ptr, val_ptr, pdu_len) \
          if ((pdu_len) != \
              dsm_pushdown_tail((item_ptr), (val_ptr), \
                                (uint16)(pdu_len), \
                                DSM_DS_SMALL_ITEM_POOL)) \
          { \
            HDR_MSG_PROT_1(MSG_LEGACY_ERROR, \
                           "CSNA: Problem packing 3G1X Services Message pdu_len=%d", \
                           (pdu_len)); \
          }

/******************* 3G1X Services Ack Message ***********************/
#define HDRCSNA_UNPACK_ACK_SEQUENCE( msg_ptr, ack_seq) \
          (HDRBIT_UNPACK8((msg_ptr), (ack_seq), 8, 6))

#define HDRCSNA_PACK_ACK_SEQUENCE( msg_ptr, ack_seq) \
          (HDRBIT_PACK8((msg_ptr), (ack_seq), 8, 6))

/******************** 3G1X Parameters Message ************************/
/* External format */
typedef PACK(struct)
{
  uint8     message_id[8];
  uint8     trans_id[8];
  uint8     param_signature[12];
  uint8     sid_included[1];
  uint8     sid[15];
  uint8     nid_included[1];
  uint8     nid[16];
  uint8     reg_zone_included[1];
  uint8     reg_zone[12];
  uint8     total_zones_included[1];
  uint8     total_zones[3];
  uint8     zone_timer_included[1];
  uint8     zone_timer[3];
  uint8     packet_zone_id_included[1];
  uint8     packet_zone_id[8];
  uint8     pzid_hyst_parameters_included[1];
  uint8     pz_hyst_enabled[1];
  uint8     pz_hyst_info_incl[1];
  uint8     pz_hyst_list_len[1];
  uint8     pz_hyst_act_timer[8];
  uint8     pz_hyst_timer_mul[3];
  uint8     pz_hyst_timer_exp[5];
  uint8     p_rev_included[1];
  uint8     p_rev[8];
  uint8     neg_slot_cycle_index_sup_included[1];
  uint8     neg_slot_cycle_index_sup[1];
  uint8     encrypt_mode_included[1];
  uint8     encrypt_mode[2];
  uint8     enc_supported_included[1];
  uint8     enc_supported[1];
  uint8     sig_encrypt_sup_included[1];
  uint8     sig_encrypt_sup[8];
  uint8     msg_integrity_sup_included[1];
  uint8     msg_integrity_sup[1];
  uint8     sig_integrity_sup_incl_included[1];
  uint8     sig_integrity_sup_incl[1];
  uint8     sig_integrity_sup_included[1];
  uint8     sig_integrity_sup[8];
  uint8     auth_included[1];
  uint8     auth[2];
  uint8     max_num_alt_so_included[1];
  uint8     max_num_alt_so[3];
  uint8     use_sync_id_included[1];
  uint8     use_sync_id[1];
  uint8     ms_init_pos_loc_sup_ind_included[1];
  uint8     ms_init_pos_loc_sup_ind[1];
  uint8     mob_qos_included[1];
  uint8     mob_qos[1];
  uint8     band_class_info_req_included[1];
  uint8     band_class_info_req[1];
  uint8     alt_band_class_included[1];
  uint8     alt_band_class[5];
  uint8     max_add_serv_instance_included[1];
  uint8     max_add_serv_instance[3];
  uint8     home_reg_included[1];
  uint8     home_reg[1];
  uint8     for_sid_reg_included[1];
  uint8     for_sid_reg[1];
  uint8     for_nid_reg_included[1];
  uint8     for_nid_reg[1];
  uint8     power_up_reg_included[1];
  uint8     power_up_reg[1];
  uint8     power_down_reg_included[1];
  uint8     power_down_reg[1];
  uint8     parameter_reg_included[1];
  uint8     parameter_reg[1];
  uint8     reg_prd_included[1];
  uint8     reg_prd[7];
  uint8     reg_dist_included[1];
  uint8     reg_dist[11];
  uint8     pref_msid_type_included[1];
  uint8     pref_msid_type[2];
  uint8     ext_pref_msid_type_included[1];
  uint8     ext_pref_msid_type[2];
  uint8     meid_reqd_included[1];
  uint8     meid_reqd[1];
  uint8     mcc_included[1];
  uint8     mcc[10];
  uint8     imsi_11_12_included[1];
  uint8     imsi_11_12[7];
  uint8     imsi_t_supported_included[1];
  uint8     imsi_t_supported[1];
  uint8     reconnect_msg_ind_included[1];
  uint8     reconnect_msg_ind[1];
  uint8     rer_mode_supported_included[1];
  uint8     rer_mode_supported[1];
  uint8     tkz_mode_supported_included[1];
  uint8     tkz_mode_supported[1];
  uint8     tkz_id_included[1];
  uint8     tkz_id[11];
  uint8     pilot_report_included[1];
  uint8     pilot_report[1];
  uint8     sdb_supported_included[1];
  uint8     sdb_supported[1];
  uint8     auto_fcso_allowed_included[1];
  uint8     auto_fcso_allowed[1];
  uint8     sdb_in_rcnm_ind_included[1];
  uint8     sdb_in_rcnm_ind[1];
  uint8     fpc_fch_included[1];
  uint8     fpc_fch_init_setpt_rc3[8];
  uint8     fpc_fch_init_setpt_rc4[8];
  uint8     fpc_fch_init_setpt_rc5[8];
  uint8     t_add_included[1];
  uint8     t_add[6];
  uint8     pilot_inc_included[1];
  uint8     pilot_inc[4];
} hdrcsna_3g1x_parm_ext_type;

/* 3G1X Parameters Message internal format */

typedef homer_3g1x_param_s_type  hdrcsna_3g1x_parm_intf_type;

typedef struct
{
  uint8                         trans_id;
  hdrcsna_3g1x_parm_intf_type   ho_parm;

} hdrcsna_3g1x_parm_type;

/*---------------------------------------------------------------------------
                       Timer, events and other defines                      
---------------------------------------------------------------------------*/

/* 3G1X Services message Quick Repeat timeout. 
 *
 * 40m 40m     320m       40m 40m      320m      40m 40m      320m
 * |__|__|________________|__|__|________________|__|__|________________|
 * 1  2  3                4  5  6                7  8  9              Timeout
 *
 * AT will queue three identical copies of 3G1X Services separated
 * by 40 msec. If AT doesn't receive the 3G1X Services Ack messsage
 * witin 400 msec starting from when the 1st copy of 3G1X services
 * message is queued, it will queue three identical message copies
 * again separated by 40 msec, for a total of 3 times.
 */
#define HDRCSNA_N_3G1X                            3
#define HDRCSNA_N_3G1X_QUICK_REPEAT               3
#define HDRCSNA_N_3G1X_TOTAL_COUNT    \
        ( HDRCSNA_N_3G1X * HDRCSNA_N_3G1X_QUICK_REPEAT )

#define HDRCSNA_T_3G1X                            400
#define HDRCSNA_T_3G1X_QUICK_REPEAT               40
#define HDRCSNA_T_3G1X_LONG_INTERVAL  \
        ( HDRCSNA_T_3G1X -            \
          (HDRCSNA_N_3G1X_QUICK_REPEAT - 1) * HDRCSNA_T_3G1X_QUICK_REPEAT )

typedef enum
{
  HDRCSNA_3G1X_SRV_MSG_ACK_TIMER

} hdrcsna_timer_name_enum_type;


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------
 Control block of the CSNA 
--------------------------------------------------------------*/
LOCAL struct
{

  /* The MessageSequence of last 3G1X Services Messages sent by
   * AT with AckRequired = 1.
   */
  uint8           last_sent_3g1x_srv_msg_seq;

  /* 3G1XParameters message. This information will be used by 1x
   * when setting up its traffic channel.
   */
  boolean                     param_3g1x_valid;
  hdrcsna_3g1x_parm_type      parm_3g1x;

  /* Buffer to hold the incoming and outgoing 3G1X Services Message.
   */
  hdrcsna_3g1x_services_type  rcvd_3g1x_srv_msg;

  hdrcsna_3g1x_services_type  sent_3g1x_srv_msg;

  struct
  {
    /* If we are waiting for Ack message for our 3G1XServices
     * message */
    boolean                   waiting_for_ack;

    /* Timer for receiving the 3G1X Services Ack message */
    rex_timer_type            ack_timer;

    /* 3G1X Services Message quick repeat counter */
    uint8                     sent_msg_count;

  } quick_repeat;

} hdrcsna;


/* <EJECT> */
/*=============================================================================

                       LOCAL FUNCTION DECLARATIONS

=============================================================================*/
void hdrcsna_timer_cb
(
  uint32 timer_id
);

void hdrcsna_tx_outcome_cb
(
  void                 *outcome_data_ptr,
  hdrerrno_enum_type    status
);

/* <EJECT> */
/*===========================================================================

                      LOCAL FUNCTION  DEFINITIONS

===========================================================================*/


/* EJECT */
/*===========================================================================
FUNCTION HDRCSNA_SEND_3G1X_ACK_MSG

DESCRIPTION
  This function sends a 3G1X Ack message, including: 3G1XServicesAckMessage,
  3G1XParametersAckMessage, 3G1XParametersResetAckMessage.

DEPENDENCIES
  None

PARAMETERS
  msg_id  - MessageId of the Ack Message.
  payload - Payload of the Ack Message.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void hdrcsna_send_3g1x_ack_msg
( 
  uint8   msg_id,
  uint8   payload
)
{
  dsm_item_type *item_ptr;              /* DSM buffer for message to send */
  hdrhai_stream_enum_type stream_id;             /* stream to send ACK on */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Ensure that CSNA is negotiated and bound */
  if (hdrstream_get_stream( HDRHAI_3G1X_CSN_APP, &stream_id ) ==
                            FALSE )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "CSNA: Couldn't get stream. Not q-ing msgs");
    return;
  }

  /* Get new buffer */
  item_ptr = hdrutil_new_msg_buffer();

  /* Pack fields */
  HDRCSNA_PACK_MESSAGE_ID( item_ptr, msg_id );

  if ( msg_id == HDRCSNA_3G1X_SERVICES_ACK_MSG_ID )
  {
    /* Pack the AckSequence for 3G1XServicesAckMessage */
    HDRCSNA_PACK_ACK_SEQUENCE( item_ptr, payload );
  }
  else
  {
    /* Pack the TransactionID for 3G1XParametersAckMessage,
     * and 3G1XParametersResetAckMessage */
    HDRCSNA_PACK_TRANS_ID( item_ptr, payload );
  }

  hdrhmp_app_send_msg( HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL,
                       stream_id,
                       HDRHAI_AC_OR_RTC,
                       FALSE,
                       HDRCSNA_MSG_PRI,
                       HDRHAI_IN_USE, /* Instance of protocol sending msg */
                       item_ptr,
                       NULL,          /* No callback is required */
                       NULL );

} /* hdrcsna_send_3g1x_ack_msg() */



/* EJECT */
/*===========================================================================
FUNCTION HDRCSNA_SEND_3G1X_SERVICES_MSG

DESCRIPTION
  This function sends a 3G1X Services message on a DO Channel

DEPENDENCIES
  None

PARAMETERS
  msg_ptr   pointer to the 3G1X Services Message to be sent out.
RETURN VALUE
  E_SUCCESS
  E_FAILURE

SIDE EFFECTS
  None
===========================================================================*/
static hdrerrno_enum_type hdrcsna_send_3g1x_services_msg
(
  hdrcsna_3g1x_services_type   *msg_ptr
)
{
  dsm_item_type *item_ptr;              /* DSM buffer for message to send */
  hdrhai_stream_enum_type stream_id;             /* stream to send ACK on */
  hdrerrno_enum_type  err = E_SUCCESS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, "CSNA: hdrcsna_send_3g1x_services_msg()");

  ASSERT( msg_ptr != NULL );

  /* Ensure that CSNA is negotiated and bound */
  if (hdrstream_get_stream( HDRHAI_3G1X_CSN_APP, &stream_id ) ==
                            FALSE )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "CSNA: Couldn't get stream. can't send 3G1X Srv msgs");
    err = E_NOT_ALLOWED;
    return err;
  }

  /* Get new buffer */
  item_ptr = hdrutil_new_msg_buffer();

  /* Pack fields */
  HDRCSNA_PACK_MESSAGE_ID( item_ptr, HDRCSNA_3G1X_SERVICES_MSG_ID );

  /* Ack is always require for 3G1X Services message */
  HDRCSNA_PACK_ACK_REQUIRED( item_ptr, msg_ptr->ack_req );
  HDRCSNA_PACK_MESSAGE_SEQUENCE( item_ptr, msg_ptr->msg_seq );
  HDRCSNA_PACK_3G1X_LOGICAL_CHANNEL( item_ptr, msg_ptr->log_ch_3g1x );
  HDRCSNA_PACK_PROTOCOL_REVISION( item_ptr, msg_ptr->p_rev );
  HDRCSNA_PACK_PDU_LENGTH( item_ptr, msg_ptr->pdu_len );

  if ( msg_ptr->pdu_len > 0 )
  {
    if (msg_ptr->pdu_len != dsm_pushdown_tail(&item_ptr, msg_ptr->pdu, 
                                              (uint16)msg_ptr->pdu_len, 
                                              DSM_DS_SMALL_ITEM_POOL))
    { 
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "CSNA: Problem pack 3G1X Services Message pdu_len=%d",
                     msg_ptr->pdu_len);
      err = E_FAILURE;
    }
    else
    {
      HDR_MSG_PROT_4(MSG_LEGACY_MED, 
                     "Send 3G1X, seq=%d ch=%d p_rev=%d len=%d",
                     msg_ptr->msg_seq, msg_ptr->log_ch_3g1x,
                     msg_ptr->p_rev, msg_ptr->pdu_len );

      /* We send the message on existing RTC. That is, if the 
       * connection is closed before the message can be sent 
       * out, then the message will be dropped and HMP will not
       * attempt to open a new connection to send that message out. */
      hdrhmp_app_send_msg_on_existing_rtc( 
                           HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL,
                           stream_id,
                           FALSE,         /* Not reliable */
                           HDRCSNA_MSG_PRI,
                           HDRHAI_IN_USE, /* Instance of protocol sending msg */
                           item_ptr,
                           hdrcsna_tx_outcome_cb,
                           (void *)msg_ptr->msg_seq ); /* outcome_tag_ptr */
    }
  }

  return err;

} /* hdrcsna_send_3g1x_services_msg */


/* EJECT */
/*===========================================================================

FUNCTION HDRCSNA_SEND_RPT_TO_HOMER

DESCRIPTION
  This function sends a report to the Handoff Manager.

DEPENDENCIES
  None.

PARAMETERS

  cmd_ptr - pointer to command to send to Handoff Manager

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrcsna_send_rpt_to_homer
(
  cm_mc_rpt_type *cmd_ptr
  /* Pointer to command to send to CM */
)
{
  HDR_MSG_PROT_1(MSG_LEGACY_MED, "CSNA: Send homer rpt %d",cmd_ptr->hdr.cmd);

  cmd_ptr->hdr.task_ptr = NULL;
  cm_mc_rpt(cmd_ptr);

} /* hdrcsna_send_rpt_to_homer() */

/* EJECT */
/*===========================================================================

FUNCTION HDRCSNA_SEND_3G1X_SERVICES_TO_HOMER

DESCRIPTION
  This function sends the 3G1X Services message to Handoff Manager

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr   - A pointer to the 3G1X Services Message to be sent to 
              Handoff Manager.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
static void hdrcsna_send_3g1x_services_to_homer
(
  hdrcsna_3g1x_services_type  *msg_ptr
)
{
  cm_mc_rpt_type  *cmd_ptr;    /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, "Send 3G1XServicesMsg to HOMER");

  /* Queue the message on CM's report queue */
  cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

  cmd_ptr->hdr_tunnel_msg.hdr.cmd = CM_HDR_TUNNEL_MSG_F;

  cmd_ptr->hdr_tunnel_msg.hdr_msg.hdr_payload.dest_protocol = 
    SYS_SYS_ID_TYPE_IS95;
  cmd_ptr->hdr_tunnel_msg.hdr_msg.hdr_payload.len = msg_ptr->pdu_len;
  cmd_ptr->hdr_tunnel_msg.hdr_msg.hdr_payload.chan = msg_ptr->log_ch_3g1x;

  cmd_ptr->hdr_tunnel_msg.hdr_msg.hdr_payload.p_rev = msg_ptr->p_rev;
  memcpy((void *)cmd_ptr->hdr_tunnel_msg.hdr_msg.hdr_payload.payload,
         (void *)msg_ptr->pdu,
         msg_ptr->pdu_len);

  /* Include the 3G1XParameters to make sure they are latest */
  memcpy((void *)&cmd_ptr->hdr_tunnel_msg.hdr_msg.hdr_3g1x_param,
         &hdrcsna.parm_3g1x.ho_parm,
         sizeof(hdrcsna.parm_3g1x.ho_parm));

  /* Also include the RAND value */
  cmd_ptr->hdr_tunnel_msg.hdr_msg.hdr_3g1x_param.rand =
    hdrscmcsna_get_rand();

  hdrcsna_send_rpt_to_homer( cmd_ptr );

} /* hdrcsna_send_3g1x_services_to_homer */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRCSNA_SEND_STATUS_TO_HOMER

DESCRIPTION
  This function sends status regarding 3G1XServices messages to the Handoff
  Manager

DEPENDENCIES
  None.

PARAMETERS
  err_code - Error code describing the status of 3G1XServicesMessage to CM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrcsna_send_status_to_homer
(
  homer_tunnel_msg_status_e_type err_code
  /* CSNA tunneling message error code */
)
{
  cm_mc_rpt_type *cmd_ptr;
  /* Pointer to CM command buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                 "Send tunnel message status %d to homer", err_code);

  cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

  cmd_ptr->tunnel_msg_status.hdr.cmd = CM_TUNNEL_MSG_STATUS_F;
  cmd_ptr->tunnel_msg_status.status = err_code;

  hdrcsna_send_rpt_to_homer( cmd_ptr );

} /* hdrcsna_send_status_to_homer() */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_SEND_3G1X_SRV_MSG_CMD

DESCRIPTION
  This function processes the send 3G1XServices message command 
  from CM/HOMER.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - 3G1XServices message to be sent.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static void hdrcsna_process_send_3g1x_srv_msg_cmd
(
  hdrcsna_send_3g1x_msg_params_type * msg_ptr
)
{
  homer_tunnel_msg_status_e_type err_code = HOMER_TUNNEL_MSG_STATUS_NONE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( msg_ptr->payload_len > HDRCSNA_3G1X_MAX_PDU_LEN )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                    "3G1X Srv msg payload length (%d) is too large", 
                    msg_ptr->payload_len );
    hdrcsna_send_status_to_homer( HOMER_TUNNEL_MSG_STATUS_FAIL_INVALID_MSG );
    return;
  }

  /* We can only send one 3G1X Services message at a time */
  if ( hdrcsna.quick_repeat.sent_msg_count != 0 )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "Can't send a 3G1X Srv msg while another is pending");
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                   "CSNA: pending 3G1X Srv msg seq=%d, repeat cnt=%d",
                   hdrcsna.sent_3g1x_srv_msg.msg_seq, 
                   hdrcsna.quick_repeat.sent_msg_count);
    err_code = HOMER_TUNNEL_MSG_STATUS_FAIL_INVALID_STATE;
  }
  else
  {
    /* Save the 3G1X Services message into hdrcsna structure. */
  
    /* Always set the AckRequired to be TRUE so AN will respond back */
    hdrcsna.sent_3g1x_srv_msg.ack_req = TRUE;
  
    /* Increment the message sequence module 64 */ 
    if ( hdrcsna.sent_3g1x_srv_msg.ack_req )
    {
      if (hdrcsna.last_sent_3g1x_srv_msg_seq == 
          HDRCSNA_INVALID_3G1X_SRV_MSG_SEQ)
      {
        hdrcsna.last_sent_3g1x_srv_msg_seq = 0;
      }
      else
      {
        hdrcsna.last_sent_3g1x_srv_msg_seq = 
          (hdrcsna.last_sent_3g1x_srv_msg_seq + 1) % 
          (HDRCSNA_MAX_3G1X_SRV_MSG_SEQ + 1);
      }

      hdrcsna.sent_3g1x_srv_msg.msg_seq = 
        hdrcsna.last_sent_3g1x_srv_msg_seq;
    }
    else
    {
      /* If AckRequired = 0, always set MessageSequence to 0 */
      hdrcsna.sent_3g1x_srv_msg.msg_seq = 0;
    }
  
    hdrcsna.sent_3g1x_srv_msg.log_ch_3g1x = msg_ptr->chan;
    hdrcsna.sent_3g1x_srv_msg.p_rev = msg_ptr->p_rev;
    hdrcsna.sent_3g1x_srv_msg.pdu_len = msg_ptr->payload_len;
  
    /* Copy the PDU also */
    memcpy( hdrcsna.sent_3g1x_srv_msg.pdu, 
            msg_ptr->payload, msg_ptr->payload_len );
  
    /* Send 3G1X Services message */
    if ( hdrcsna_send_3g1x_services_msg( &hdrcsna.sent_3g1x_srv_msg )
         == E_SUCCESS )
    {
      if ( hdrcsna.sent_3g1x_srv_msg.ack_req )
      {
        /* This is the first attempt of sending this 3G1X 
         * Services message */
        hdrcsna.quick_repeat.sent_msg_count++;
        hdrcsna.quick_repeat.waiting_for_ack = TRUE;
      }
    }
    else
    {
      err_code = HOMER_TUNNEL_MSG_STATUS_FAIL_NO_SRV;
    }
  }

  /* We report SUCCESS to HOMER when:
   * 1. If AckRequired=1, 3G1XServicesAck has been received, or
   * 2. If AckRequired=0, this message has been successfully sent out.
   *    This should be done in SLP callback function
   */
  if ( err_code != HOMER_TUNNEL_MSG_STATUS_NONE )
  {
    hdrcsna_send_status_to_homer( err_code );
  }

} /* hdrcsna_process_send_3g1x_srv_msg_cmd */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_TX_MSG_STATUS

DESCRIPTION
  This function processes the 3G1XServices message TX outcome status.
  If this 3G1XServices message requires an ack, it starts the Ack timer
  for quick repeating.
  If the transmission fails, it reports the status back to HOMER.

DEPENDENCIES
  None

PARAMETERS
  tx_msg_seq - Message sequence number for 3G1XServices message being Tx'ed
  status     - Transmission status

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static void hdrcsna_process_tx_msg_status
(
  uint8   tx_msg_seq,
            /* 3G1XServices message sequence */
  hdrerrno_enum_type  status
            /* TX outcome status */
)
{
  homer_tunnel_msg_status_e_type err_code = HOMER_TUNNEL_MSG_STATUS_NONE;
  rex_timer_cnt_type    timeout;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( tx_msg_seq == hdrcsna.sent_3g1x_srv_msg.msg_seq )
  {
    if ( status == E_SUCCESS )
    {
      /* Message is successfully sent out but could got lost
       * during transmission. Since the is not a reliable message,
       * we need to quick repeat to increase the chance of 
       * successful delivery. */
      if ( hdrcsna.quick_repeat.waiting_for_ack )
      {
        if (( hdrcsna.quick_repeat.sent_msg_count % 
              HDRCSNA_N_3G1X_QUICK_REPEAT ) == 0 )
        {
          /* We have sent out a batch of 3G1X Services message in
           * quick repeat but did't received the Ack yet, wait 
           * for 320 msec and send another batch. */
          timeout = HDRCSNA_T_3G1X_LONG_INTERVAL;
        }
        else
        {
          /* Still in the quick repeat operation. wait for 40 msec */
          timeout = HDRCSNA_T_3G1X_QUICK_REPEAT;
        }

        /* Start the timer */
        rex_set_timer(&hdrcsna.quick_repeat.ack_timer, timeout);

        HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                       "CSNA: repeat count %d, start Ack timer %d(ms)",
                       hdrcsna.quick_repeat.sent_msg_count, timeout);
      }
      else if ( !hdrcsna.sent_3g1x_srv_msg.ack_req )
      {
        /* If 3G1XServicesAck is not required, we can report success to
         * HOMER after the message is delivered */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Send 3G1XSrv msg is successful");
        err_code = HOMER_TUNNEL_MSG_STATUS_SUCCESS;
      }
      else
      {
        /* 3G1XServicesAck has been received and success status
         * sent to HOMER already. This happens when we queue 
         * multiple messages onto SLP TX queue, and receives 
         * the Ack before the sending the next message. */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                     "Ack received, Ignore MSG TX Outcome");
      }
    }
    else
    {
      /* In all other status, SLP failed to send the message on existing
       * RTC due to connection closed or any other reasons, we give up
       * without doing quick repeat */
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Send 3G1XSrv msg failed, errno=%d", status );

      err_code = HOMER_TUNNEL_MSG_STATUS_FAIL_NO_SRV;
    }
  }
  else
  {
    /* If this outcome callback is for a previous 3G1XServices message,
     * just ignore */
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                   "Recv a cback for previous 3G1xSrv msg(%d) cur(%d)",
                   tx_msg_seq, hdrcsna.sent_3g1x_srv_msg.msg_seq);

  }

  if ( err_code != HOMER_TUNNEL_MSG_STATUS_NONE )
  {
    /* We already know the status of sending out this 3G1XServices
     * message, either success with receiving an ACK or failure. 
     * Clear the timer and flag, and let HOMER know the status. */

    (void)rex_clr_timer( &hdrcsna.quick_repeat.ack_timer );
    hdrcsna.quick_repeat.sent_msg_count = 0;
    hdrcsna.quick_repeat.waiting_for_ack = FALSE;

    hdrcsna_send_status_to_homer( err_code );
  }

} /* hdrcsna_process_tx_msg_status */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_3G1X_SERVICES_MSG

DESCRIPTION
  This function processes 3G1X Services message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  hdrerrno_enum_type - Any message parsing errors
  
SIDE EFFECTS
  None

===========================================================================*/
static hdrerrno_enum_type hdrcsna_process_3g1x_services_msg
(
  dsm_item_type * item_ptr
)
{
  hdrerrno_enum_type          err = E_SUCCESS;
  hdrcsna_3g1x_services_type  *msg_ptr;
  boolean                     send_to_homer = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If 3G1X Parameters Message has not been received or has just
   * been reset but without update, drop the 3G1X Services message */
  if ( !hdrcsna.param_3g1x_valid )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "3G1X Param hasn't received, ignore 3G1X Srv msg");
    return E_FAILURE;
  }

  /* We only store one copy of the last received 3G1XServicesMessage */
  msg_ptr = &hdrcsna.rcvd_3g1x_srv_msg;

  /* Unpack the message */
  err |= HDRCSNA_UNPACK_ACK_REQUIRED( item_ptr, &msg_ptr->ack_req );
  err |= HDRCSNA_UNPACK_MESSAGE_SEQUENCE( item_ptr, &msg_ptr->msg_seq );
  err |= HDRCSNA_UNPACK_3G1X_LOGICAL_CHANNEL( item_ptr, 
                                              &msg_ptr->log_ch_3g1x );
  err |= HDRCSNA_UNPACK_PROTOCOL_REVISION( item_ptr, &msg_ptr->p_rev );
  err |= HDRCSNA_UNPACK_PDU_LENGTH( item_ptr, &msg_ptr->pdu_len );
  if ( msg_ptr->pdu_len > HDRCSNA_3G1X_MAX_PDU_LEN )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "CSNA: PDU in 3G1X Services Msg is too large" );
    err |= E_FAILURE;
  }

  if ( err != E_SUCCESS )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "CSNA: 3G1X Services Msg unpack error %d", err);
    send_to_homer = FALSE;
  }
  else
  {
    if ( msg_ptr->ack_req )
    {
      /* If Acknowledgement is required, send 3G1X Services Ack message */
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                     "CSNA: Send 3G1XServicesAck seq=%d", msg_ptr->msg_seq);

      hdrcsna_send_3g1x_ack_msg( HDRCSNA_3G1X_SERVICES_ACK_MSG_ID, 
                                 msg_ptr->msg_seq );
  
    }
  
    if ( send_to_homer )
    {
      /* Unpack the PDU field */
      if ( msg_ptr->pdu_len != 
           dsm_extract(item_ptr,
                       (FPOS(hdrcsna_3g1x_services_ext_type, pdu) / 8),
                          /* offset in terms of bytes */
                       msg_ptr->pdu, msg_ptr->pdu_len))
      { 
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                       "CSNA: err unpack 3G1X Services Message pdu_len=%d", 
                       msg_ptr->pdu_len);
  
        err |= E_DATA_INVALID;
        send_to_homer = FALSE;
      }
    }
  }

  if ( send_to_homer )
  {
    /* Send the 3G1X Services Message to Handoff Manager */
    hdrcsna_send_3g1x_services_to_homer( msg_ptr );
  }

  return err;

} /* hdrcsna_process_3g1x_services_msg */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_3G1X_SERVICES_ACK_MSG

DESCRIPTION
  This function processes 3G1X Services Ack message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  hdrerrno_enum_type - Any message parsing errors
  
SIDE EFFECTS
  None

===========================================================================*/
static hdrerrno_enum_type hdrcsna_process_3g1x_services_ack_msg
(
  dsm_item_type * item_ptr
)
{
  
  hdrerrno_enum_type          err = E_SUCCESS;
  uint8                       ack_seq;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Unpack the message */
  err |= HDRCSNA_UNPACK_ACK_SEQUENCE( item_ptr, &ack_seq );

  if ( err != E_SUCCESS )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "CSNA: 3G1X Services Ack Msg unpack error %d", err);
  }
  else
  {
    if ( hdrcsna.sent_3g1x_srv_msg.msg_seq == ack_seq )
    {
      /* We received an Ack for the outgoing 3G1X Services message. */
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                     "Received Ack (seq=%d) for 3G1X Services msg", 
                     ack_seq );

      /* Cancel the 3G1X Services timer */
      (void)rex_clr_timer(&hdrcsna.quick_repeat.ack_timer);
      hdrcsna.quick_repeat.sent_msg_count = 0;
      hdrcsna.quick_repeat.waiting_for_ack = FALSE;

      /* Send the status to HOMER */
      hdrcsna_send_status_to_homer(HOMER_TUNNEL_MSG_STATUS_SUCCESS);
    }
    else
    {
      /* Ack sequence doesn't match our 3G1X Services msg sequence */
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "Ack seq(%d) doesn't match AT's msg seq (%d)",
                     ack_seq, hdrcsna.sent_3g1x_srv_msg.msg_seq);
    }
  }

  return err;

} /* hdrcsna_process_3g1x_services_ack_msg */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_UNPACK_3G1X_PARMS_MSG

DESCRIPTION
  This function unpacks 3G1X Parameters message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the packed message
  msg_ptr  - Pointer to the 3G1X Parameter Message internal structure
             for unpacked message. 

RETURN VALUE
  hdrerrno_enum_type - Any message parsing errors
  
SIDE EFFECTS
  None

===========================================================================*/
static hdrerrno_enum_type hdrcsna_unpack_3g1x_parms_msg
(
  dsm_item_type               *item_ptr,
  hdrcsna_3g1x_parm_type      *msg_ptr
)
{
  hdrerrno_enum_type          err = E_SUCCESS;
  uint16                      offset;
  boolean                     field_included;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* MessageID */
  err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.msg_id,
                        FPOS(hdrcsna_3g1x_parm_ext_type, message_id),
                        FSIZ(hdrcsna_3g1x_parm_ext_type, message_id));

  /* TransactionID */
  err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->trans_id,
                        FPOS(hdrcsna_3g1x_parm_ext_type, trans_id),
                        FSIZ(hdrcsna_3g1x_parm_ext_type, trans_id));

  /* 3G1XParametersSignature */
  err |= HDRBIT_UNPACK16(item_ptr, &msg_ptr->ho_parm.signature,
                        FPOS(hdrcsna_3g1x_parm_ext_type, param_signature),
                        FSIZ(hdrcsna_3g1x_parm_ext_type, param_signature));

  /* SID */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included,
                        FPOS(hdrcsna_3g1x_parm_ext_type, sid_included),
                        FSIZ(hdrcsna_3g1x_parm_ext_type, sid_included));
  offset = FPOS(hdrcsna_3g1x_parm_ext_type, sid_included) +
           FSIZ(hdrcsna_3g1x_parm_ext_type, sid_included);

  /* If SIDIncluded, overwrite the previous value. Otherwise, do nothing */
  if ( field_included )
  {
    msg_ptr->ho_parm.sid_included = field_included;
    err |= HDRBIT_UNPACK16(item_ptr, &msg_ptr->ho_parm.sid, offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, sid));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sid);
  }

  /* NID */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, nid_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, nid_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.nid_included = field_included;
    err |= HDRBIT_UNPACK16(item_ptr, &msg_ptr->ho_parm.nid, offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, nid));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, nid);
  }

  /* REG_ZONE */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, reg_zone_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, reg_zone_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.reg_zone_included = field_included;
    err |= HDRBIT_UNPACK16(item_ptr, &msg_ptr->ho_parm.reg_zone, offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, reg_zone));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, reg_zone);
  }

  /* TOTAL_ZONES */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             total_zones_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, total_zones_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.total_zones_included = field_included;
    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.total_zones, offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, total_zones));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, total_zones);
  }

  /* ZONE_TIMER */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included,
                        offset,
               FSIZ(hdrcsna_3g1x_parm_ext_type, zone_timer_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, zone_timer_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.zone_timer_included = field_included;
    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.zone_timer,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, zone_timer));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, zone_timer);
  }

  /* PACKET_ZONE_ID */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
               FSIZ(hdrcsna_3g1x_parm_ext_type, packet_zone_id_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, packet_zone_id_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.pzid_included = field_included;
    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.pzid,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, packet_zone_id));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, packet_zone_id);
  }

  /* PZIDHystParameters */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             pzid_hyst_parameters_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pzid_hyst_parameters_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.pzid_hyst_param_included = field_included;
    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.pz_hyst_enabled,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, pz_hyst_enabled));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pz_hyst_enabled);

    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.pz_hyst_info_incl,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                pz_hyst_info_incl));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pz_hyst_info_incl);

    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.pz_hyst_list_len,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                pz_hyst_list_len));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pz_hyst_list_len);

    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.pz_hyst_act_timer,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                pz_hyst_act_timer));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pz_hyst_act_timer);

    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.pz_hyst_timer_mul,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                pz_hyst_timer_mul));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pz_hyst_timer_mul);

    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.pz_hyst_timer_exp,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                pz_hyst_timer_exp));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pz_hyst_timer_exp);
  }

  /* P_REV */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, p_rev_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, p_rev_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.p_rev_included = field_included;
    err |= HDRBIT_UNPACK8( item_ptr, &msg_ptr->ho_parm.p_rev, offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, p_rev));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, p_rev);
  }

  /* NEG_SLOT_CYCLE_INDEX_SUP */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             neg_slot_cycle_index_sup_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, 
                 neg_slot_cycle_index_sup_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.neg_slot_cycle_index_sup_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, 
                          &msg_ptr->ho_parm.neg_slot_cycle_index_sup,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, 
                               neg_slot_cycle_index_sup));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, neg_slot_cycle_index_sup);
  }

  /* ENCRYPT_MODE */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             encrypt_mode_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, encrypt_mode_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.encrypt_mode_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.encrypt_mode, offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, encrypt_mode));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, encrypt_mode);
  }

  /* ENC_SUPPORTED */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             enc_supported_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, enc_supported_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.enc_supported_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.enc_supported,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, enc_supported));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, enc_supported);
  }

  /* SIG_ENCRYPT_SUP */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             sig_encrypt_sup_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sig_encrypt_sup_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.sig_encrypt_sup_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.sig_encrypt_sup,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, sig_encrypt_sup));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sig_encrypt_sup);
  }

  /* MSG_INTEGRITY_SUP */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             msg_integrity_sup_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, msg_integrity_sup_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.msg_integrity_sup_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.msg_integrity_sup,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, msg_integrity_sup));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, msg_integrity_sup);
  }

  /* SIG_INTEGRITY_SUP_INCL */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             sig_integrity_sup_incl_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sig_integrity_sup_incl_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.sig_integrity_sup_incl_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.sig_integrity_sup_incl,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, 
                               sig_integrity_sup_incl));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sig_integrity_sup_incl);
  }

  /* SIG_INTEGRITY_SUP_INCLUDED */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             sig_integrity_sup_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sig_integrity_sup_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.sig_integrity_sup_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.sig_integrity_sup,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, sig_integrity_sup));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sig_integrity_sup);
  }

  /* AUTH */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset, 
                        FSIZ(hdrcsna_3g1x_parm_ext_type, auth_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, auth_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.auth_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.auth,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, auth));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, auth);
  }

  /* MAX_NUM_ALT_SO */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             max_num_alt_so_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, max_num_alt_so_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.max_num_alt_so_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.max_num_alt_so,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, max_num_alt_so));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, max_num_alt_so);
  }

  /* USE_SYNC_ID */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             use_sync_id_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, use_sync_id_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.use_sync_id_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.use_sync_id,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, use_sync_id));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, use_sync_id);
  }

  /* MS_INIT_POS_LOC_SUP_IND */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             ms_init_pos_loc_sup_ind_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, ms_init_pos_loc_sup_ind_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.ms_init_pos_loc_sup_ind_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.ms_init_pos_loc_sup_ind,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, 
                               ms_init_pos_loc_sup_ind));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, ms_init_pos_loc_sup_ind);
  }

  /* MOB_QOS */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included,
                        offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, mob_qos_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, mob_qos_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.mob_qos_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.mob_qos,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, mob_qos));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, mob_qos);
  }

  /* BAND_CLASS_INFO_REQ */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             band_class_info_req_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, band_class_info_req_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.band_class_info_req_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.band_class_info_req,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       band_class_info_req));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, band_class_info_req);
  }

  /* ALT_BAND_CLASS */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             alt_band_class_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, alt_band_class_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.alt_band_class_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.alt_band_class,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, alt_band_class));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, alt_band_class);
  }

  /* MAX_ADD_SERV_INSTANCE */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             max_add_serv_instance_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, max_add_serv_instance_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.max_add_serv_instance_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.max_add_serv_instance,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, 
                               max_add_serv_instance));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, max_add_serv_instance);
  }

  /* HOME_REG */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset, 
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             home_reg_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, home_reg_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.home_reg_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.home_reg, offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, home_reg));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, home_reg);
  }

  /* FOR_SIG_REG */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             for_sid_reg_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, for_sid_reg_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.for_sid_reg_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.for_sid_reg, offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, for_sid_reg));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, for_sid_reg);
  }

  /* FOR_NID_REG */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             for_nid_reg_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, for_nid_reg_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.for_nid_reg_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.for_nid_reg,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, for_nid_reg));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, for_nid_reg);
  }

  /* POEWR_UP_REG */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             power_up_reg_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, power_up_reg_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.power_up_reg_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.power_up_reg,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, power_up_reg));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, power_up_reg);
  }

  /* POEWR_DOWN_REG */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             power_down_reg_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, power_down_reg_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.power_down_reg_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.power_down_reg,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, power_down_reg));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, power_down_reg);
  }

  /* PARAMETER_REG */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             parameter_reg_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, parameter_reg_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.parameter_reg_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.parameter_reg,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, parameter_reg));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, parameter_reg);
  }

  /* REG_PRD */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             reg_prd_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, reg_prd_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.reg_prd_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.reg_prd,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, reg_prd));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, reg_prd);
  }

  /* REG_DIST */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             reg_dist_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, reg_dist_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.reg_dist_included = field_included;

    /* 11 bits */
    err |= HDRBIT_UNPACK16(item_ptr, &msg_ptr->ho_parm.reg_dist,
                           offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, reg_dist));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, reg_dist);
  }

  /* PREF_MSID_TYPE */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             pref_msid_type_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pref_msid_type_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.pref_msid_type_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.pref_msid_type,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, pref_msid_type));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pref_msid_type);
  }

  /* EXT_PREF_MSID_TYPE */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             ext_pref_msid_type_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, ext_pref_msid_type_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.ext_pref_msid_type_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.ext_pref_msid_type,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       ext_pref_msid_type));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, ext_pref_msid_type);
  }

  /* MEID_REQD */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             meid_reqd_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, meid_reqd_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.meid_reqd_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.meid_reqd,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, meid_reqd));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, meid_reqd);
  }

  /* MCC */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             mcc_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, mcc_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.mcc_included = field_included;
    err |= HDRBIT_UNPACK16(item_ptr, &msg_ptr->ho_parm.mcc, offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, mcc));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, mcc);
  }

  /* IMSI_11_12 */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             imsi_11_12_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, imsi_11_12_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.imsi_11_12_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.imsi_11_12,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, imsi_11_12));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, imsi_11_12);
  }

  /* IMSI_T_SUPPORTED */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             imsi_t_supported_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, imsi_t_supported_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.imsi_t_supported_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.imsi_t_supported,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, imsi_t_supported));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, imsi_t_supported);
  }

  /* RECONNECT_MSG_IND */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             reconnect_msg_ind_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, reconnect_msg_ind_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.reconnect_msg_ind_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.reconnect_msg_ind,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       reconnect_msg_ind));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, reconnect_msg_ind);
  }

  /* RER_MODE_SUPPORTED */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             rer_mode_supported_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, rer_mode_supported_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.rer_mode_supported_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.rer_mode_supported,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       rer_mode_supported));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, rer_mode_supported);
  }

  /* TKZ_MODE_SUPPORTED */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             tkz_mode_supported_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, tkz_mode_supported_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.tkz_mode_supported_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.tkz_mode_supported,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       tkz_mode_supported));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, tkz_mode_supported);
  }

  /* TKZ_ID */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             tkz_id_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, tkz_id_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.tkz_id_included = field_included;

    /* 11 bits */
    err |= HDRBIT_UNPACK16(item_ptr, &msg_ptr->ho_parm.tkz_id, offset,
                           FSIZ(hdrcsna_3g1x_parm_ext_type, tkz_id));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, tkz_id);
  }

  /* PILOT_REPORT */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             pilot_report_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pilot_report_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.pilot_report_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.pilot_report,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, pilot_report));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pilot_report);
  }

  /* SDB_SUPPORTED */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             sdb_supported_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sdb_supported_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.sdb_supported_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.sdb_supported,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, sdb_supported));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sdb_supported);
  }

  /* AUTO_FCSO_ALLOWED */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             auto_fcso_allowed_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, auto_fcso_allowed_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.auto_fcso_allowed_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.auto_fcso_allowed,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       auto_fcso_allowed));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, auto_fcso_allowed);
  }

  /* SDB_IN_RCNM_IND */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             sdb_in_rcnm_ind_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sdb_in_rcnm_ind_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.sdb_in_rcnm_ind_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.sdb_in_rcnm_ind,
                          offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, sdb_in_rcnm_ind));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, sdb_in_rcnm_ind);
  }

  /* FPC_FCH */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             fpc_fch_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, fpc_fch_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.fpc_fch_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.fpc_fch_init_setpt_rc3,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       fpc_fch_init_setpt_rc3));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, fpc_fch_init_setpt_rc3);

    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.fpc_fch_init_setpt_rc4,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       fpc_fch_init_setpt_rc4));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, fpc_fch_init_setpt_rc4);

    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.fpc_fch_init_setpt_rc5,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       fpc_fch_init_setpt_rc5));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, fpc_fch_init_setpt_rc5);
  }

  /* T_ADD */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             t_add_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, t_add_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.t_add_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.t_add, offset,
                          FSIZ(hdrcsna_3g1x_parm_ext_type, t_add));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, t_add);
  }

  /* PILOT_INC */
  err |= HDRBIT_UNPACK8(item_ptr, &field_included, offset,
                        FSIZ(hdrcsna_3g1x_parm_ext_type, 
                             pilot_inc_included));
  offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pilot_inc_included);

  if ( field_included )
  {
    msg_ptr->ho_parm.pilot_inc_included = field_included;
    err |= HDRBIT_UNPACK8(item_ptr, &msg_ptr->ho_parm.pilot_inc,
                          offset, FSIZ(hdrcsna_3g1x_parm_ext_type, 
                                       pilot_inc));
    offset += FSIZ(hdrcsna_3g1x_parm_ext_type, pilot_inc);
  }

  return err;

} /* hdrcsna_unpack_3g1x_parms_msg */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_3G1X_PARMS_MSG

DESCRIPTION
  This function processes 3G1X Parameters message:
  - Unpacks the 3G1X Parameters Message
  - Sends a 3G1X Parameters Ack Message
  - If 3G1XParametersSignature is 0 or the same as RUTC from OMP, then
    AT issues 3G1XParametersReceived indication
  - Otherwise, AT sends RouteUpdateMessage.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  hdrerrno_enum_type: Unpack error
  
SIDE EFFECTS
  None

===========================================================================*/
static hdrerrno_enum_type hdrcsna_process_3g1x_parms_msg
(
  dsm_item_type * item_ptr
)
{
  hdrerrno_enum_type          err = E_SUCCESS;
  static hdrcsna_3g1x_parm_type      parm_3g1x;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* save a backup copy of 3G1X parameters in case rollback is needed */
  memcpy(&parm_3g1x, &hdrcsna.parm_3g1x, sizeof(parm_3g1x));

  /* Unpack the 3G1X Parameters Message */
  err |= hdrcsna_unpack_3g1x_parms_msg(item_ptr, &parm_3g1x);

  if ( err != E_SUCCESS )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "CSNA: Error(%d) unpack 3G1XParmMsg", err); 
  }
  else
  {
    /* Send a 3G1XParametersAck message */
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                   "CSNA: Send 3G1XParamsAckMsg, trans_id=%d", 
                   parm_3g1x.trans_id);

    hdrcsna_send_3g1x_ack_msg(HDRCSNA_3G1X_PARMS_ACK_MSG_ID, 
                              parm_3g1x.trans_id);

    /* If 3G1XParametersSignature is 0 or the same as RUTC of the OHP,
     * AT will issue 3G1XParametersReceived indication. Otherwise, AT
     * sends out the a RouteUpdateMessage. */

    if ( parm_3g1x.ho_parm.signature != 0 &&
         parm_3g1x.ho_parm.signature != hdrovhd_get_curr_rutc())
    {
      /* Send RouteUpdateMessage */
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                     "CSNA: Sig=%x, send RouteUpdateMessage",
                     parm_3g1x.ho_parm.signature);

      hdrrup_send_route_update(HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL);
    }
    else
    {
      /* 3G1x Param valid */
      hdrcsna.param_3g1x_valid = TRUE;

      memcpy(&hdrcsna.parm_3g1x, &parm_3g1x, sizeof(parm_3g1x));

      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                     "CSNA: Sig=%x, send 3G1XParamReceived ind",
                     hdrcsna.parm_3g1x.ho_parm.signature);

      /* Give the indication */
      hdrind_give_ind(HDRIND_CSNA_3G1X_PARM_RECEIVED, NULL);
    }
  }

  return err;

} /* hdrcsna_process_3g1x_parms_msg */




/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_3G1X_PARMS_RESET_MSG

DESCRIPTION
  This function processes 3G1X Parameters Reset message:
  - Sends a 3G1X Parameters Reset Ack Message
  - Clear all stored value of 3G1X circuit services fields

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  hdrerrno_enum_type:
  
SIDE EFFECTS
  None

===========================================================================*/
static hdrerrno_enum_type hdrcsna_process_3g1x_parms_reset_msg
(
  dsm_item_type * item_ptr
)
{
  hdrerrno_enum_type          err = E_SUCCESS;
  uint8                       trans_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Unpacks the TransactionID field */
  err |= HDRCSNA_UNPACK_TRANS_ID(item_ptr, &trans_id);

  if ( err != E_SUCCESS )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "CSNA: Error(%d) unpack 3G1XParmResetMsg", err); 
  }
  else
  {
    /* 3G1x Param valid */
    hdrcsna.param_3g1x_valid = FALSE;

    /* Send indication and event */
    hdrind_give_ind(HDRIND_CSNA_3G1X_PARM_RESET_RECEIVED, NULL);

    /* Send a 3G1XParametersAck message */
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                   "CSNA:Send 3G1XParamsResetAckMsg,trans_id=%d", trans_id);

    hdrcsna_send_3g1x_ack_msg(HDRCSNA_3G1X_PARMS_RESET_ACK_MSG_ID, trans_id);

    /* Clear up the contents of 3G1XParameter cache */
    memset(&hdrcsna.parm_3g1x, 0, sizeof(hdrcsna.parm_3g1x));

    HDR_MSG_PROT(MSG_LEGACY_HIGH, "CSNA: Clear the 3G1XParameters");

  }

  return err;

} /* hdrcsna_process_3g1x_parms_reset_msg */


/* <EJECT> */
/*===========================================================================

                     EXTERNAL  FUNCTION  DEFINITIONS

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRCSNA_POWERUP_INIT

DESCRIPTION
  This function initializes the CSNA protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrcsna_powerup_init ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "hdrcsna_powerup_init()");

  /* Initialize hdrscmcsna variables */
  hdrscmcsna_powerup_init();

  /* Initialize hdrcsna variables */
  memset(&hdrcsna, 0, sizeof(hdrcsna));

  hdrcsna.last_sent_3g1x_srv_msg_seq = HDRCSNA_INVALID_3G1X_SRV_MSG_SEQ;
  hdrcsna.quick_repeat.waiting_for_ack = FALSE;
  hdrcsna.quick_repeat.sent_msg_count = 0;

  hdrcsna.param_3g1x_valid = FALSE;

  if ( hdrmc_is_first_init() )
  {
    /* Define 3G1X Services Message Ack timer */
    rex_def_timer_ex( &hdrcsna.quick_repeat.ack_timer, 
                      hdrcsna_timer_cb,
                      HDRCSNA_3G1X_SRV_MSG_ACK_TIMER );
  }

} /* hdrcsna_powerup_init */



/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_MSG

DESCRIPTION
  This function processes messages for CSNA.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcsna_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1(MSG_LEGACY_MED, "hdrcsna_process_msg sender %d",
                 protocol_instance);

  /* MessageID is the first uint8 */
  if (HDRCSNA_UNPACK_MSG_ID(item_ptr, &msg_id) != E_SUCCESS)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "CSNA: Could not extract message_id");
  }
  else
  {
    HDRTRACE_MSG( HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, msg_id, 0 );

    switch ( msg_id )
    {
      case HDRCSNA_3G1X_SERVICES_MSG_ID:
        HDR_MSG_PROT (MSG_LEGACY_MED, "3G1X Services Msg received");
        hdrcsna_process_3g1x_services_msg(item_ptr);
        break;
  
      case HDRCSNA_3G1X_SERVICES_ACK_MSG_ID:
        HDR_MSG_PROT (MSG_LEGACY_MED, "3G1X Services Ack Msg received");
        hdrcsna_process_3g1x_services_ack_msg(item_ptr);
        break;
    
      case HDRCSNA_3G1X_PARMS_MSG_ID:
        HDR_MSG_PROT (MSG_LEGACY_MED, "3G1X Parameters Msg received");
        hdrcsna_process_3g1x_parms_msg(item_ptr);
        break;
    
      case HDRCSNA_3G1X_PARMS_RESET_MSG_ID:
        HDR_MSG_PROT (MSG_LEGACY_MED, "3G1X Parameters Reset Msg received");
        hdrcsna_process_3g1x_parms_reset_msg(item_ptr);
        break;
    
      default:
        ERR ("CSNA: Unknown message %d received", msg_id, 0, 0);
        break;
    }
  }

  dsm_free_packet( &item_ptr );

} /* hdrcsna_process_msg */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_CMD

DESCRIPTION
  This function processes commands given to CSNA.

DEPENDENCIES
  None

PARAMETERS
  csna_cmd_ptr - Pointer to the command for CSNA to process  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrcsna_process_cmd
( 
  hdrcsna_cmd_type * cmd_ptr
)
{

  HDRTRACE_CMD( HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, cmd_ptr->name,
                cmd_ptr->sender, 0 );

  /*-------------------------------------------------------------------------
  Determine which command has been received.
  -------------------------------------------------------------------------*/
  switch ( cmd_ptr->name )
  {
    case HDRCSNA_SEND_3G1X_SERVICES_MSG_CMD:

      HDR_MSG_PROT(MSG_LEGACY_MED, "CSNA: rcvd Send 3G1XSrvMsg command");
      hdrcsna_process_send_3g1x_srv_msg_cmd( &cmd_ptr->params.msg_3g1x );
      break;

    case HDRCSNA_UPDATE_CSNA_CONFIG_CMD:

      HDR_MSG_PROT(MSG_LEGACY_MED, "CSNA: rcvd Update CSNA config command");
      hdrscmcsna_get_at_supported_attrib_from_homer(
        &cmd_ptr->params.config_attrib);
      break;

    default:
      /*---------------------------------------------------------------------
      Unrecognized command.
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "CSNA: Received Unrecognized cmd:%d", cmd_ptr->name);
      break;
  }

} /* hdrcsna_process_cmd() */



/* EJECT */
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for CSNA.
  
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcsna_process_timer
(
  uint32 timer_id
)
{
  homer_tunnel_msg_status_e_type status = HOMER_TUNNEL_MSG_STATUS_NONE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Determine which CSNA timer has expired.
  -------------------------------------------------------------------------*/
  switch ( (hdrcsna_timer_name_enum_type) timer_id )
  {
    case HDRCSNA_3G1X_SRV_MSG_ACK_TIMER:

      if ( hdrcsna.quick_repeat.sent_msg_count >= HDRCSNA_N_3G1X_TOTAL_COUNT)
      {
        /* We have exhausted every quick repeat of 3G1X Services message
         * but didn't receive the Ack. Report the status to Homer. Don't
         * close the session since the Ack may get lost and AN may just 
         * send the UHDM subsequently */
        HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                     "CSNA: Fail to receive 3G1XSrvAck message");

        /* Send Tunnel message failure status to Homer */
        status = HOMER_TUNNEL_MSG_STATUS_FAIL_NO_ACK;
      }
      else
      {
        /* Try to Send the 3G1X Services message again. If it fails
         * report the failure to HOMER */
        if ( hdrcsna_send_3g1x_services_msg( &hdrcsna.sent_3g1x_srv_msg )
             == E_SUCCESS )
        {
          hdrcsna.quick_repeat.sent_msg_count++;
        }
        else
        {
          HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                       "CSNA: Fail to send 3G1XServices message");

          status = HOMER_TUNNEL_MSG_STATUS_FAIL_NO_SRV;
        }
      }

      if ( status != HOMER_TUNNEL_MSG_STATUS_NONE )
      {
        hdrcsna.quick_repeat.sent_msg_count = 0;
        hdrcsna.quick_repeat.waiting_for_ack = FALSE;
        hdrcsna_send_status_to_homer(status);
      }
      break;
    
    default:
      /*-----------------------------------------------------------------------
        Unrecognized Timer:
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "CSNA: Invalid Timer ID (%d)",timer_id);

      break;
  }

} /* hdrcsna_process_timer() */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_IND

DESCRIPTION
  This function processes indications given to CSNA.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for CSNA to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcsna_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  hdrcsna_tx_msg_status_ind_type   *msg_status_ptr;
  uint8                            tx_msg_seq;
  hdrerrno_enum_type               tx_msg_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_IND( HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, ind_name, 0);

  switch ( ind_name )
  {
    case HDRIND_SCM_CSNA_AN_GAUPED:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,
                    "CSNA: Received and accepted AN-GAUP!");

      /* Update CM with the changed CSNA attributes */
      hdrscmcsna_process_attrib_changed();

      break;

    case HDRIND_SCM_CSNA_AT_CONFIG_COMPLETE:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,
                    "CSNA: AT config for CSNA protocol is complete!");

      break;

    case HDRIND_CSNA_PROT_NEG_FAILED:
      HDR_MSG_PROT (MSG_LEGACY_ERROR,
                    "CSNA: Protocol Negotiation Failed!");

      /* Inform CM that CSNA Protocol Negotiation failed */
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,
                    "CSNA: SCM_SESSION_CHANGED Indication");

      /* Update CM with the changed CSNA attributes */
      hdrscmcsna_process_attrib_changed();
      break;

    case HDRIND_CSNA_TX_MSG_STATUS_RECEIVED:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,
                    "CSNA: HDRIND_CSNA_TX_MSG_STATUS_RECEIVED ind");
      msg_status_ptr = 
        (hdrcsna_tx_msg_status_ind_type *)(ind_data_ptr->csna_status_ptr);
      tx_msg_seq = msg_status_ptr->tx_msg_seq;
      tx_msg_status = msg_status_ptr->status;

      hdrcsna_process_tx_msg_status(tx_msg_seq, tx_msg_status );

      break;
     
    default:
      ERR ("Unrecognized Indication %d", ind_name, 0, 0);
      break;
  }

} /* hdrcsna_process_ind */



/* <EJECT> */
/*===========================================================================
FUNCTION HDRCSNA_TIMER_CB

DESCRIPTION
  This function is the callback for all CSNA state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrcsna_timer_cb
(
  uint32 timer_id
)
{
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "CSNA: hdrcsna_timer_cb");

  hdrmc_queue_timer(HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, timer_id);

} /* hdrcsna_timer_cb() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRCSNA_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrcsna_msg_cb 
( 
  dsm_item_type *msg_ptr
)
{
  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrcsna_msg_cb");

  hdrmc_queue_msg( HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, 
                   HDRHAI_IN_USE, 
                   msg_ptr ); 

} /* hdrcsna_msg_cb() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRCSNA_TX_OUTCOME_CB

DESCRIPTION
  Callback function for transmission outcome of the 3G1XServices message.

DEPENDENCIES
  None

PARAMETERS
  outcome_data_ptr - Message sequence of 3G1XService message being Tx'ed
  status           - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrcsna_tx_outcome_cb
(
  void                 *outcome_data_ptr,
  hdrerrno_enum_type    status
)
{
  hdrind_ind_data_union_type ind_data;

  /* Allocate out of the heap because it is possible for this indication to
  be queued from a different task context. Otherwise, the payload may be
  corrupted which could cause a null pointer de-reference. */
  static hdrcsna_tx_msg_status_ind_type  hdrcsna_tx_status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrcsna_tx_status.tx_msg_seq = (uint8)((uint32)outcome_data_ptr);
  hdrcsna_tx_status.status     = status;

  ind_data.csna_status_ptr = (void *)&hdrcsna_tx_status;

  hdrind_give_ind(HDRIND_CSNA_TX_MSG_STATUS_RECEIVED, (void *)&ind_data);

} /* hdrcsna_tx_outcome_cb() */


/* EJECT */
/*===========================================================================
FUNCTION HDRCSNA_TUNNEL_MSG

DESCRIPTION
  This function commands CSNA protocol to tunnel message on DO channel

DEPENDENCIES
  None

PARAMETERS
  sender        - Protocol name who send this command
  dest_protocol - Protocol name who should receive this command
  chan          - 3G1X Logical Channel
  p_rev         - 1X Protocol Revision
  payload_len   - PDU length
  payload       - PDU data
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrcsna_tunnel_msg
( 
  hdrhai_protocol_name_enum_type  sender,
  sys_sys_id_type_e_type          dest_protocol,
  uint8                           chan,
  uint8                           p_rev,
  uint16                          payload_len,
  uint8                           *payload
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrcsna_cmd_type                hdrcsna_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdrcsna_tunnel_msg");

  if ( dest_protocol != SYS_SYS_ID_TYPE_IS856 )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "TUNNEL_MSG_F, wrong dest_prot %d", dest_protocol );
    hdrcsna_send_status_to_homer(HOMER_TUNNEL_MSG_STATUS_FAIL_INVALID_MSG);
    return;
  }

  if ( payload_len > HDRCSNA_3G1X_MAX_PDU_LEN )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                    "TUNNEL_MSG_F, payload length (%d) too large", payload_len );
    hdrcsna_send_status_to_homer(HOMER_TUNNEL_MSG_STATUS_FAIL_INVALID_MSG);
    return;
  }

  /* Fill in all members of the hdrcsna_cmd_type for this command */
  hdrcsna_cmd.name = HDRCSNA_SEND_3G1X_SERVICES_MSG_CMD;
  hdrcsna_cmd.sender = sender;

  hdrcsna_cmd.params.msg_3g1x.chan = chan;
  hdrcsna_cmd.params.msg_3g1x.p_rev = p_rev;
  hdrcsna_cmd.params.msg_3g1x.payload_len = payload_len;
  hdrcsna_cmd.params.msg_3g1x.payload = payload;


  if ( rex_self() == HDRCSNA_TASK_PTR )
  {
    hdrcsna_process_cmd( &hdrcsna_cmd );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "Currently no other task ever asks CSNA to send messages!");

    hdrmc_queue_cmd( HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, &hdrcsna_cmd,
                     sizeof( hdrcsna_cmd_type ) );
  }

} /* hdrcsna_tunnel_msg */


/* EJECT */
/*===========================================================================
FUNCTION HDRCSNA_UPDATE_MSG_CONFIG

DESCRIPTION
  This function commands CSNA protocol to update the CSNA attribute 
  configurations.

DEPENDENCIES
  None

PARAMETERS
  sender        - Protocol name who send this command
    
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrcsna_update_msg_config
( 
  hdrhai_protocol_name_enum_type  sender,
  hdrmc_csna_msg_config_type      *csna_config_ptr
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrcsna_cmd_type hdrcsna_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_LOW, "hdrcsna_update_msg_config");

  /* Fill in all members of the hdrcsna_cmd_type for this command */
  hdrcsna_cmd.name = HDRCSNA_UPDATE_CSNA_CONFIG_CMD;
  hdrcsna_cmd.sender = sender;
  memcpy( &hdrcsna_cmd.params.config_attrib, 
          csna_config_ptr, sizeof(hdrmc_csna_msg_config_type));

  if ( rex_self() == HDRCSNA_TASK_PTR )
  {
    hdrcsna_process_cmd( &hdrcsna_cmd );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "Currently no other task ever asks CSNA to send messages!");

    hdrmc_queue_cmd( HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, &hdrcsna_cmd,
                     sizeof( hdrcsna_cmd_type ) );
  }

} /* hdrcsna_update_msg_config */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRCSNA_CONFIGURE

DESCRIPTION
  This function conditionally commands HDRCSNA to configure 

DEPENDENCIES
  hdrcsna_configure shall only be called after the AT Initiated stream is 
  complete, or there is already a valid stream configuration.
  HDRMC task context

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the configure command is accepted
  FALSE - the configure command is rejected

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcsna_configure
(
  hdrhai_protocol_name_enum_type sender 
)
{
  if (rex_self() == HDRCSNA_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    return hdrscmcsna_process_config();
  }
  else
  {
    /* Currently no other task should ever issue this command,
     * so we should never get here.
     */
    ERR_FATAL("Configure command not expected from other task",0,0,0);
    return FALSE;
  }
} /* hdrcsna_configure */

#endif /* FEATURE_HDR_CSNA */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRCSNA.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcsna_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void * ind_data_ptr
)
{
  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrcsna_ind_cb");

  #ifdef FEATURE_HDR_CSNA
  if (rex_self() == HDRCSNA_TASK_PTR)
  {
    hdrcsna_process_ind(ind_name, (hdrind_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, 
                     ind_name, ind_data_ptr );
  }
  #endif /* FEATURE_HDR_CSNA */
  
} /* hdrcsna_ind_cb */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_GET_1X_SRCH_PARAMS

DESCRIPTION
  This function retrieves the 1X Search parameters from 3G1XParameters 
  message.

DEPENDENCIES
  The 1x Search parameters are retrieved only when 3G1XParameters 
  message is valid

PARAMETERS
  params - pointer to the 1x Search parameter variable whose values to
           be filled in.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrcsna_get_1x_srch_params
(
  hdrcsna_1x_search_params_type   *params
)
{
  /* If 3G1XParameters message is not valid, set the flags to be FALSE */
  params->t_add_included     = FALSE;
  params->pilot_inc_included = FALSE;
  params->pilot_inc = 0;
  params->t_add = 0;

  #ifdef FEATURE_HDR_CSNA
  if ( hdrcsna.param_3g1x_valid )
  {
    if ( hdrcsna.parm_3g1x.ho_parm.t_add_included )
    {
      params->t_add_included = TRUE;
      params->t_add = hdrcsna.parm_3g1x.ho_parm.t_add;
    }

    if ( hdrcsna.parm_3g1x.ho_parm.pilot_inc_included )
    {
      params->pilot_inc_included = TRUE;
      params->pilot_inc = hdrcsna.parm_3g1x.ho_parm.pilot_inc;
    }
  }
  else
  #endif /* FEATURE_HDR_CSNA */
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, "Can't get 1xsrch parm: 3G1xparm not valid");
  }

} /* hdrcsna_get_1x_srch_params */
