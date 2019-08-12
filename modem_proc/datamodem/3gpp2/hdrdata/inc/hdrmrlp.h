#ifndef HDRMRLP_H
#define HDRMRLP_H

/*===========================================================================

          M U L T I - F L O W   R A D I O   L I N K   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Multiflow Radio Link Protocol.

Copyright (c) 2003-2011 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
02/08/11   ls      Global variable cleanup
09/04/09   ss      CMI SU level modifications.
10/11/08   cs      Compiler Warning fix.
07/18/08   as      Fix to properly assign new qn indices
06/06/08   spn     Added functionality to count number of data bytes that
                   have been sent or received by a particular RLP
07/21/07   kvd     7500 re-arch changes.
02/17/06   sy      Added support to handle the reset all rlp's messages
                   received from AN.
10/27/05   ksu     EMPA: octet stream, octet sequencing
10/19/05   sy      Support for version2.0 reservation signalling messages.
09/28/05   ksu/sy  HDR: use one-shot abort timer instead of 20ms ticks
09/28/05   ksu/sy  Call hdr[m]rlpi_rx_process_sig_msg() directly from hdr[m]rlp_msg_cb()
07/20/05   sy      Support for Rev A with FEATURE_HDR_QOS.
07/11/05   sy      Added support for DO Rev A RTCMAC code changes.
06/24/05   sk      Added support for HDR_RST_MRLP_STATS diag command
03/05/05   sy      Added support for air interface signalling messages for
                   Reservation and deleted the RLP signalling messages.
09/16/04   vas     Changed function prototype
11/06/03   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "errno.h"

#ifdef FEATURE_HDR_QOS

#include "hdrcom_api.h"
#include "dsm.h"
#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "queue.h"
#include "errno.h"
#include "ds707_qosdefs.h"
#include "hdrmrlp_api.h"

#ifndef FEATURE_HDR
#error "FEATURE_HDR_QOS defined without FEATURE_HDR being defined"
#endif /* FEATURE_HDR */

/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/

/*---------------------------------------------------------------------------

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  MESSAGE ID FOR THE RLP SIGNALING MESSAGES
---------------------------------------------------------------------------*/
#define HDRMRLP_SIG_RESET_TX_INDICATION     0x00
#define HDRMRLP_SIG_RESET_RX_INDICATION     0x01
#define HDRMRLP_SIG_NAK                     0x02
#define HDRMRLP_SIG_RESET_TX_INDICATION_ACK 0x0D
#define HDRMRLP_SIG_RESET_TX_COMPLETE       0x0E
#define HDRMRLP_SIG_RESET_RX_COMPLETE       0x0F
#define HDRMRLP_SIG_MULTILINK_NAK           0x25
#define HDRMRLP_SIG_FLUSH                   0x26

#define HDRLMRLP_SIG_RESERVATION_ON_REQUEST  0x16
#define HDRLMRLP_SIG_RESERVATION_OFF_REQUEST 0x17
#define HDRLMRLP_SIG_RESERVATION_ACCEPT      0x18
#define HDRLMRLP_SIG_RESERVATION_REJECT      0x19
#define HDRLMRLP_SIG_REV_RESERVATION_ON      0x1A
#define HDRLMRLP_SIG_REV_RESERVATION_OFF     0x1B
#define HDRLMRLP_SIG_FWD_RESERVATION_OFF     0x22
#define HDRLMRLP_SIG_FWD_RESERVATION_ON      0x23
#define HDRLMRLP_SIG_FWD_RESERVATION_ACK     0x24

/*---------------------------------------------------------------------------
  PRIORITY OF THE RLP SIGNALING MESSAGES
---------------------------------------------------------------------------*/
#define HDRMRLP_SIG_PRI_RESET_TX_INDICATION     50
#define HDRMRLP_SIG_PRI_RESET_RX_INDICATION     50
#define HDRMRLP_SIG_PRI_NAK                     50
#define HDRMRLP_SIG_PRI_RESET_TX_INDICATION_ACK 50
#define HDRMRLP_SIG_PRI_RESET_TX_COMPLETE       50
#define HDRMRLP_SIG_PRI_RESET_RX_COMPLETE       50
#define HDRMRLP_SIG_PRI_FLUSH                   50

#define HDRMRLP_SIG_PRI_RESV_ACCEPT             40
/* FwdReservationAck uses HDRMRLP_SIG_PRI_RESV_ACCEPT */
#define HDRMRLP_SIG_PRI_RESV_REJECT             40
#define HDRMRLP_SIG_PRI_RESV_ON                 40
#define HDRMRLP_SIG_PRI_RESV_OFF                40
#define HDRMRLP_SIG_PRI_RESV_ONOFF_REQUEST      40
/*---------------------------------------------------------------------------
  For creating RLP signaling messages, offset to leave for header
  information.  The value is in bytes.
---------------------------------------------------------------------------*/
#define HDRMRLP_SIGHDR_OFFSET 12 //Why is this offsetted by 12?? 12 bytes is a lot!

#define HDRMRLP_STREAM_HEADER_SIZE 2

/*-------------------------------------------------------------------------
  Maximum number of Scheduler Group Ids reported by PCP
---------------------------------------------------------------------------*/
#define HDRMRLP_MAX_SCHEDULER_GROUP_IDS  64

#define HDRMRLP_INVALID_QN_INDEX  0xFF

#define HDRMRLP_INVALID_QN_ID     (0xFFFFFFFFFFFFFFFFULL)

/*---------------------------------------------------------------------------
  Macro to determine the reset state of RLP.
---------------------------------------------------------------------------*/
#define HDRMRLP_IN_RESET( rrcb_ptr )                                        \
          ((rrcb_ptr->state.state != DSRLPI_ESTABLISHED_STATE) ||           \
           (rrcb_ptr->state.reset_rx_ind_to_send == TRUE)      ||           \
           (rrcb_ptr->state.reset_tx_ind_to_send == TRUE)      ||           \
           (rrcb_ptr->state.reset_tx_complete_to_send == TRUE))

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Typedef that identifies an RLP.
 ---------------------------------------------------------------------------*/
typedef struct
{
  uint8 flow;              /* RLP FLow used in sig msgs to identify RLP   */
  dsrlp_ver_enum_type ver; /* Version of M-RLP (Fwd or Rev)               */
} hdrmrlp_rlp_identity_type;

/*---------------------------------------------------------------------------
  Information relating to a particular Transaction ID. Note all fields
  are not relevant to a particular transaction. This is used in the following
  cases:
  1. When the AT sends a RLP On/Off Request or Accept/Reject message  or if it receives a RLP On/Off
  or Accept/Reject, the message indicates the Transaction ID it relates to.
  Based on the Transacation ID, the AT/AN infer what RLPs the message affects
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type       link; /* Used to Link this to a queue */
  uint8 trans_id;       /* Transaction ID                                  */
  uint8 msg_id;         /* Message ID corresponding to this transaction    *
                         */
} hdrmrlp_transaction_type;


/*---------------------------------------------------------------------------
  This structure contains state information that is common across the
  various RLP flows. This includes information related to processing of and
  sending of group RLP messages like RLPOn, etc.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   tx_transaction_id; /* Last sent TransID for a RLPOn/Off message  */

  /*--------------------------------------------------------------------------
  The following variables indicate if there are any response messages to ResvOn
  pending to be sent. A list of the pending Transaction Ids for Accepts in
  response RResvOn is maintained. Since each Resv could potentially get a ResvOn
  message, a list of up to the max number of Reverse Link Resvs is maintained.
  Since an ResvOff for each Resv may also potentially be pending, we need twice
  the number of buffers as number of Reverse Link Resvs
  --------------------------------------------------------------------------*/
  q_type accept_pending_tx_q;
  q_type accept_pending_tx_free_q;

  hdrmrlp_transaction_type accept_pending_buffer[(2*DS707_N_RESV_MAX_REV)];

 /*--------------------------------------------------------------------------
     Array for maintaining a list of Reservation for which response to
     message is pending. Since the ip flows could send potentially individual
     Reservation Request on/off message instead of group message we need to
     account for that. Also since a Reservation could send a ReservationOnRequest
     followed by ReservationOffRequest or vice-versa without waiting for a response,
     we should account for 2 transacations pending per ip flow
  --------------------------------------------------------------------------*/
  q_type resv_on_off_rsp_pending_q;
  q_type resv_on_off_rsp_pending_free_q;

  boolean resv_on_request_to_send;  /* ResvOnRequest msgs need to be sent*/
  boolean resv_off_request_to_send; /* ResvOffRequest msgs need to be send */
  boolean reset_tx_ind_ack_all_fwd_rlp_to_send;
  boolean reset_rx_complete_all_rlp_to_send;

  byte active_route; /* To Remember the Active route, This value is
                 updated from the RSP Module */

} hdrmrlp_app_ctl_blk_type;

/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      GET_HDRMRLP_QN_INFO_TABLE_CRIT_SECT_PTR

DESCRIPTION   Accessor function for hdrmrlp_qn_info_table_crit_sect

DEPENDENCIES  None

RETURN VALUE  Address of hdrmrlp_qn_info_table_crit_sect

SIDE EFFECTS  None
===========================================================================*/
rex_crit_sect_type* get_hdrmrlp_qn_info_table_crit_sect_ptr
(
  void
);

/*===========================================================================
FUNCTION      GET_HDRMRLP_QN_INFO_TABLE_PTR

DESCRIPTION   Accessor function for hdrmrlp_qn_info_table

DEPENDENCIES  None

RETURN VALUE  Address of hdrmrlp_qn_info_table

SIDE EFFECTS  None
===========================================================================*/
dsrlpi_qn_info_type* get_hdrmrlp_qn_info_table_ptr
(
  void
);

/*===========================================================================
FUNCTION      GET_HDRMRLP_QN_GROUP_INDEX_TABLE_PTR

DESCRIPTION   Accessor function for hdrmrlp_qn_group_index_table

DEPENDENCIES  None

RETURN VALUE  Address of hdrmrlp_qn_group_index_table

SIDE EFFECTS  None
===========================================================================*/
uint8* get_hdrmrlp_qn_group_index_table_ptr
(
  void
);

/*===========================================================================

FUNCTION HDRMRLP_MSG_CB

DESCRIPTION
    This callback function invoked by HMP whenever a control message is
    received for RLP.  The function queues up a received RLP signaling
    message to the App receive signaling message queue. Since the messages
    for all RLPs are queued up on the same queue in the sequence in which
    they are received, they will be processed in the same order that they
    are received in.

    Note that this gets called from the HDRRX task context and the proessing
    of the messages also happens in the HDRRX task context.

DEPENDENCIES
  The receive signaling message queue must have been initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlp_msg_cb
(
  dsm_item_type *msg_ptr,               /* ptr to received RLP message     */
  hdrcom_hai_app_subtype_enum_type app_subtype /* Application Subtype          */
);

/*===========================================================================

FUNCTION HDRMRLP_PUT_PKT_CB

DESCRIPTION
  This function is called by the PCP layer, to process a received RLP data
  packet.  The function is called on a per packet basis.  As data is
  processed, the function updates RLP state variables as needed.
  Out-of-sequence data may also cause this function to generate NAK list
  entries used to build up a NAK signaling message on subsequent TX
  processing.

  In addition, the function will process any RLP signaling messages available
  on the received message queue.  This ensures that RLP signaling messages
  will have been processed, prior to processing incoming data.  At the end of
  processing a full HDR frame, PCP will call this function with a NULL
  parameter, to ensure that all signaling messages for RLP have been
  processed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RLP state variables V(R) and/or V(N) may get updated.  Entries may be
  inserted into the NAK list.  Entries may be added or removed from the
  resequencing buffer.  Signaling message processing may cause data to
  be queued for re-transmission and/or cause and RLP RESET.

===========================================================================*/
void hdrmrlp_put_pkt_cb
(
  dsm_item_type *fwd_pkt_ptr,         /* ptr to received RLP data packet   */
  void * fwd_pkt_pacb_ptr             /* App Control Block                 */
);

/*===========================================================================

FUNCTION HDRMRLP_END_OF_SLOT_CB

DESCRIPTION
  Callback registered with PCP to indicate end of slot notification.
  All the packets received in the previous slot can now be processed in
  sorted order.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void hdrmrlp_end_of_slot_cb( void* unused );

/*===========================================================================

FUNCTION HDRMRLP_INIT

DESCRIPTION
  This function starts the statistics timers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlp_init
(
  uint8 rlp_id,
  dsrlp_ver_enum_type ver
);

/*===========================================================================

FUNCTION HDRMRLP_CLEANUP

DESCRIPTION
  Does the HDR MRLP specific cleanup of the timers.
  note that it should only be called from dsrlp_cleanup,
  since the rest of the cleanup is done in dsrlp_cleanup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlp_cleanup
(
  uint8 rlp_id,
  dsrlp_ver_enum_type ver
);


/*===========================================================================
FUNCTION      HDRMRLP_POWERUP_INIT

DESCRIPTION   This function reads the NV items corresponding to MRLP logging.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void hdrmrlp_powerup_init
(
  void
);

/*===========================================================================

FUNCTION HDRMRLP_STOP_LOG_TIMERS

DESCRIPTION
  This function stops the rex timers associated with MRLP logging.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmrlp_stop_log_timers
(
  void
);

/*===========================================================================

FUNCTION HDRMRLP_UPDATE_ACTIVE_ROUTE

DESCRIPTION
  This function is called to update the active route.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlp_update_active_route
(
  byte route
);

/*===========================================================================

FUNCTION HDRMRLP_GET_STATS_FOR_MRLP

DESCRIPTION
  This function returns the number of bytes transmitted and/or received
  the network. It sums up this byte count across all fwd and reverse RLPs

DEPENDENCIES

RETURN VALUE
  Returns the byte count in the supplied uint32 pointers

SIDE EFFECTS
  None
===========================================================================*/
void hdrmrlp_get_stats_for_mrlp(

  /* Number of bytes received from network ..OUTPUT parameter.. */
  uint32 * rx_from_network_byte_cnt,

  /* Number of bytes sent to network ..OUTPUT parameter.. */
  uint32 * tx_to_network_byte_cnt
);

/*===========================================================================

FUNCTION HDRMRLP_SET_RLP_INACTIVITY_TIMER

DESCRIPTION
  This function sets the RLP inactivity timer

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void hdrmrlp_set_rlp_inactivity_timer
(
  uint8   rscb_index,
  uint32 inactivity_timer_val
);
/*===========================================================================

FUNCTION HDRMRLP_GET_RLP_INACTIVITY_TIMER

DESCRIPTION
  This function gets the RLP inactivity timer.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void hdrmrlp_get_rlp_inactivity_timer
(
  uint8   rscb_index,
  uint32  *inactivity_timer_val_ptr
);
#endif /* FEATURE_HDR_QOS */

#endif /* HDRMRLP_H */
