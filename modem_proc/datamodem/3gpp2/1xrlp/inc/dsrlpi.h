#ifndef _DSRLPI_H
#define _DSRLPI_H
/*===========================================================================

                     R A D I O   L I N K   P R O T O C O L
                  I N T E R N A L    H E A D E R     F I L E

DESCRIPTION
  This header file contains the internal interface definitions used in the
  framework for the Radio Link Protocol (RLP).  The framework is designed to
  support all flavors of RLP (RLP-1, RLP-2, RLP-3, RLP-HDR). This file should
  only be included by the RLP modules and not by external files.

  Naming convention:
  All functions in this file are external to dsrlpi.c and start as dsrlpi_...
  All variables declared here are external to dsrlpi.c and start as dsrlpi_..

  Copyright (c) 1999 - 2011 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/dsrlpi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
02/02/11   ack     Added DS_RLP_INIT_CMD
04/16/10   ls      Fix the featurization issues
02/10/10   spn      RX Task Optimization
09/04/09   ss      CMI SU level modifications.
07/24/08   spn     Update the phys_layer_enabled parameter when it is
                   GAUPed without deactivating RLP
07/22/08   spn     Update the NAK Delay Timer change because of a GAUP
07/18/08   as      Fix to properly assign new qn indices
06/25/08   as      MMPA: Fix to send Quick Repeat NAKs in a timely manner
03/30/07   ac      EMPA: Implemented RLP inactivity timer feature. Let every
				   RLP instance maintain its own inactivity timer.
03/05/07   spn     Fixed lint errors
02/01/07   ac      EMPA: Change for Logging
01/09/07   ac      EMPA: added the flush_giveup_timer the stop the flush attempt
12/21/06   spn     Integration of the SPD module into dsrlp.
07/25/06   ksu     EMPA: 1x RLP code review comments
06/06/06   ksu     EMPA: RLP Rx / Sig code review comments
05/30/06   ksu     EMPA: RLP init code review comments
04/17/06   squ     Added flag ack_to_sned to the struct dsrlpi_state_info_type.
01/20/06   ksu     EMPA: link up hdrmrlpcif to hdrmrlpc
01/03/06   ksu     EMPA: hook up forward link with new phys link architecture
12/23/05   ksu     EMPA RSP: two fwd rx queues
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to
                   dsrlp_reg_srvc() and RX callback function
12/12/05   ksu/sy  Introduced a critical section between RX and TX task to
                   prevent RX task from resetting the RLP while the
                   the TX task build the packets and waiting for a
                   response from PCP.
11/28/05   ksu     CR 81596: Rev 0 data call fails all the time with dsrlpsq
                   crash during bi-directional data transfer
11/22/05   ksu     EMPA packet stream: segment sequencing (forward link only)
11/21/05   ksu     EMPA packet stream: out of order delivery to route protocol forward
11/03/05   ksu     EMPA: packet stream, octet sequencing (reverse link only)
10/27/05   ksu     EMPA: octet stream, octet sequencing
10/25/05   sy/as   Removed dsrlpi_get_free_rscb_index() as it is not used
                   anywhere.
09/28/05   ksu/sy  HDR: use one-shot abort timer instead of 20ms ticks
09/28/05   ksu/sy  Call hdr[m]rlpi_rx_process_sig_msg() directly from hdr[m]rlp_msg_cb().
08/11/05   tc      Added handle_tx_new_data_count into dsrlpi_state_info_type.
08/08/05   sk/sy   Merged semi-dynamic QOS changes from Sandbox.
07/25/05   tc      Added local_tx_item_len into dsrlpi_state_info_type.
07/15/05   tc      Support multiple format B connection layer packet
                   in one RTCMAC subframe.
05/09/05   tc      Added local_rlp_packet_ptr into dsrlpi_state_info_type
04/20/05   tc      Added local_rtxq local retransmission queue
11/29/04   gr      Merged PZID hysteresis changes into mainline
11/22/04   vas     Changes to support GAUP. Added variables to the config
                   info to keep track of if config parameters have changed
09/10/04   vas     QOS related changes
09/08/04   vr      Added notify_ppp_data_transfer_func_ptr in the RLP cfg info
06/08/04   atp     Added channel assignment info and EXT_SEQ to state info.
05/07/04   atp     First cut at supporting EVDV (IS707-B).
07/10/03   vas     HDRRLP assigned SR ID values that dont clash with 1x values
06/18/03   vas     Increased MAX_SESSIONS for RLP to accomodate for 3 RLPs in
                   MM architecture
05/21/03   vas     Increased the number of elements in RSCB array for MM data
03/18/02   vas     Added NULL state to RLP state enum type. Needed for HDRRLP
03/01/02   hcg     Increased MAX_SESSIONS to support multiple simultaneous
                   RLPs.
02/15/02   hcg     Added fail-safe reset state timer to RLP control block to
                   protect against potential reset deadlock situation for
                   1xEV.
02/14/02   hcg     Added boolean and Rex timer to RLP control block to support
                   flush timer for 1xEV.
10/17/01   vas     Added stats logging timer to dsrlpi_sess_ctl_blk_type.
09/14/01   rama    SR_ID fix. Differentiate between SR_ID and RLP instance.
                   Set a mapping between SR_ID and RLP Instance.
08/13/01   snn     Modified the extern for dsrlpi_type_mask_table.
08/02/01   ak      Add a flag to indicate when rlp is being reset.
07/02/01   ak      Changed MAX_CHAINS to 3 * max_fwd_rlp frames.
06/04/01   igt     Added two pointer fields reseq_last_insert_chain_ptr and
                   reseq_last_insert_item_ptr in the dsrlpi_state_info_type to
                   facilitate fast inserts into resequencing q in edge cases.
05/24/01   hcg     Made changes to support HDR (1xEV).  Made size of the
                   transmit queue part of version specific constants.
05/21/01   sy      Added a new frame type DSRLPI_RESET and a new varibale in
                   reset_rlp in dsrlpi_state_info_type.
05/11/01   ak      MAX CHAINS now set to 75 items.
04/16/01   ak      Added pointer to state struct for quick access into txq.
02/21/01   ak      Support for checking against old frames and also for
                   potentially mis-RDA'd frames which cause big NAK gaps.
01/15/01   igt     #def DSRLPI_MAX_RTT_SETUP and added sync_delay in
                   dsrlpi_state_info_type.
01/04/01   igt     Added two pointer fields reseq_last_chain_ptr and
                   reseq_last_item_ptr in the dsrlpi_state_info_type to
                   facilitate fast inserts into resequencing q in edge cases.
12/06/00   ak      Changes to force FILL frames every 80 frames so that
                   other side gets our V(N) appropriately.
11/15/00   igt     remove txq_id field from dsrlpi_state_info_type struct
06/05/00   ak      New category DSRLPI_FILL, so that RLP 3 can know if data
                   was received in last 20 msec (for dormancy detection).
05/15/00   na/ak   Support for delayed frame detection.
05/10/00   ak      Added fields to support sendig fill frames and better
                   mux erasure counts.  Also added tx_fill to fcn ptr table.
04/28/00   igt     moved and renamed DSRLPI_MAX_NAK_ROUNDS
04/19/00   ak      Added new frame type enums, and added rlp_rx_mux_erasure
                   in the function ptr table.
04/13/00   ak      Made frame type enum explicit on enum values, to help in
                   debugging.  Also added externs for bytes used to record
                   first 3 bytes of each frame.
04/12/00   na      Changed Max. NAK rounds to 3.
03/21/00   igt     moved and renamed dsrlpi_stats_info_type
02/15/00   igt     Extended the dsrlpi_ver_const_type to support BLOBs
01/14/00   ak      Removed typebit_pos array extern.
01/07/00   ak      made get_best_fr_rate public.
01/06/00   na      Increased dsrlpi_type_mask_table to accomodate genric
                   service type.
07/28/99   na/hcg  Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#ifdef FEATURE_DS
#include "dsrlp_v.h"
#include "dsm.h"
#include "rex.h"

#include "dsrsp.h"
#include "hdrmrlpschedi.h"
#include "ds707_qosdefs.h"

#ifdef FEATURE_HDR
#include "queue.h"
#include "hdrcom_api.h"
#include "hdrcp_api.h"
#endif /* FEATURE_HDR */

#include "dstask_v.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/****************************************************************************/
/*************************************  HDR SPECIFIC   ***********************/
/****************************************************************************/
/*---------------------------------------------------------------------------
  Type for storing reservation related information
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 count;        /* Num of reservervations associated with this RLP */
  uint8 label[DS707_MAX_N_RESV_PER_DIR]; /* Reservations associated with the RLP */
} dsrlpi_resv_list_type;

/*
 *  Structure storing handles for each combo specified by combo_id.
 *  Each Route has an array of 3 elements of this type.
 *  carrier_handles_q --> is a queue, each element of which corresponds to
 *                  handles for each carrier. (hdrrlptsh_per_carrier_type)
 */

typedef struct
{
  uint8   combo_id;
  q_type  carrier_handles_q;
}dsrlpi_per_combo_type;

/*************************************************************************/
/*************************************  COMMON   *************************/
/****************************************************************************/

/*---------------------------------------------------------------------------
  RLP version dependendent const definitions. This struct contains the static
  information for each revision of RLP.  It includes a function pointer
  table, which is used to call into the RLP version-specific functions for
  frame processing.
---------------------------------------------------------------------------*/


typedef struct
{
  struct dsrlpi_fcn_ptr_table *fcn_ptr_table; /* Ptr table to RLP functions*/
  uint16 max_conseq_erasures;                 /* Max # of conseq erasures  */
  uint16 txq_size;                            /* Max size of tx queue      */

  byte   max_nak_rounds_fwd;                  /* Max. NAK rounds for BS    */
  byte   max_nak_rounds_rev;                  /* Max. NAK rounds for MS    */

  byte   desired_nak_rounds_fwd;              /* Def. NAK rounds for BS    */
  byte   desired_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS];
                                              /* Def. NAKs per round for BS*/
  byte   desired_nak_rounds_rev;              /* Default NAK rounds for MS */
  byte   desired_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];
                                              /* Def. NAKs per round for MS*/
  boolean seg_allowed;                        /* frames can be segmented?  */
} dsrlpi_ver_const_type;



/*---------------------------------------------------------------------------
  RLP configuration definition. This structure contains all RLP
  configuration information and is updated at the beginning of an RLP
  session.
---------------------------------------------------------------------------*/
typedef struct
{
  dsrlpi_ver_const_type*  ver_ptr; /* Ptr to version control block         */
  byte sr_id;                                 /* Service ID for this flow  */
  dsrlp_ver_enum_type ver;                    /* Current RLP version       */
  dsm_watermark_type *tx_wm_ptr;              /* Output watermark for txmit*/
  void (*notify_ppp_data_transfer_func_ptr)(byte rlp_instance);
                                              /* Notify PZID hyst engine?  */
    /*-------------------------------------------------------------------------
    The rx queue list needs to point to an array of queues of length
    (at least) DSRSP_NUM_ROUTES.  This array is stored in the common
    configuration for both RLP instances, but each RLP will only
    access the index that matches its route.
  -------------------------------------------------------------------------*/
  /* queues for rxed frames    */
  dsm_watermark_type *rx_wm_ptr[DSRSP_NUM_ROUTES];

  void  (*rx_func_ptr)(uint32);                /* rx callback               */
  uint32   rx_data;                            /* arg to rx_func_ptr()      */

  uint32    seq_modulus;                       /* 2^num-bits in seq. no.    */
  uint8     seq_length;          /* Size of Sequence space in units of bits */
  uint32    seq_modulus_short;                 /* 2^num-bits in seq. no.    */
  uint8     seq_length_short;      /* Size of Sequence space short for mmpa */
  uint32    qn_seq_modulus;  /* 2^num-bits in QN seq. no. - Redundant
                                information but useful since it will
                                calculated only once per config negotiation */
  uint8     qn_seq_length;            /* Size of QN Sequence space for mmpa */
  boolean   segment_sequencing;                 /* Data Unit is RLP packets */


  /*****************************1X specific variables*************************/
   //..................................

  /*****************************HDR specific variables************************/
  rex_crit_sect_type crit_sect;
  boolean   nak_enabled;                    /* Are Naks enabled for this RLP */
  uint16    nak_abort_timer_val;/* Value of the abort timer (in units of msec).
                                   Not needed for Reverse RLP (transmitter)  */
  uint16    flush_timer_val;    /* Value of the flush timer (in units of msec).
                                   Not needed for Forward RLP (Receiver) */
  uint16    nak_delay_timer_val; /* Value of nak delay timer (msec). */
  boolean   phys_link_nak_enabled_rev; /* Is RLP to retransmit octets when
                                   PhysicalLayer.ReverseTrafficePacketMissed
                                   ind is rcvd. Only for Rever RLP (transmitter) */
  uint8     higher_layer_protocol; /* Always set to 0x00 = Octet based HDLC framing */

  boolean has_active_rlp_cfg_changed;/* MRLP Params of an active RLP have changed */
  boolean changed_nak_enabled; /* Are Naks enabled for this RLP */
  uint16  changed_nak_abort_timer_val;/* Value of the abort timer (in units of msec).
                                         Not needed for Reverse RLP (transmitter) */
  uint16  changed_flush_timer_val; /* Value of the flush timer (in units of msec).
                                      Not needed for Forward RLP (Receiver) */
  uint16    changed_nak_delay_timer_val; /* Value of nak delay timer (msec). */
  boolean changed_phys_link_nak_enabled_rev;
                                     /* Is RLP to retransmit octets when
                                        PhysicalLayer.ReverseTrafficePacketMissed
                                        ind is rcvd. Only for Rever RLP (transmitter) */
  boolean changed_phys_link_nak_enabled_rev_val; /* Is RLP to retransmit octets when
                                   PhysicalLayer.ReverseTrafficePacketMissed
                                   ind is rcvd. Only for Rever RLP (transmitter) */

  dsrlpi_resv_list_type rsv;

  dword transmit_abort_timer_rev_val;
                                    /* The value of the timer, after multiplying
                                       with 5, this is the actual time
                                       and not the attribute value */
  boolean  dropped_stale_packet;
  boolean  dropped_marq_packet;     /* This variable keeps track of the MARQ info
                                       happened at the lower layers and pass it
                                       up to the application. */

  uint8     rlp_id_len_in_pkt;  /* Length, in units of bits of the rlp_id */
  dsrlp_enhanced_ver_enum_type enhanced;  /* EMPA or MMPA */

  boolean changed_segment_sequencing;
  boolean packet_stream;        /* RLP provides framing */
  boolean changed_packet_stream;
  boolean out_of_order_allowed;         /* RLP may pass RP packets out of order */
  boolean changed_out_of_order_allowed;

#ifdef FEATURE_HDR_QOS
  hdrcp_mrlpc_flow_prot_id_type  flow_protocol;
  hdrcp_mrlpc_flow_prot_id_type  changed_flow_protocol;
  hdrcp_mrlpc_route_prot_id_type  route_protocol;
  hdrcp_mrlpc_route_prot_id_type  changed_route_protocol;
#endif /* FEATURE_HDR_QOS*/


  boolean octet_sequenced;                    /* Octet sequenced operation?*/
  uint8     rlp_id_in_pkt;      /* Data pkts tagged with rlpflow identifier */

#ifdef FEATURE_HDR
  hdrcom_hai_stream_enum_type   stream_id; /* Stream associated with RLP */
  uint8 activity_bitmap; /* RLP activity bitmap */
  uint32  inactivity_timer_val;
  boolean use_default_inactivity_timer_val;
  boolean enable_inactivity_timer;
  rex_timer_type inactivity_timer;
#endif /* FEATURE_HDR */

#ifdef FEATURE_HDR_QOS
  hdrmrlpsched_wm_node_type *sched_head_node_ptr; /* Head of the scheduler node*/
  uint8 num_sched_nodes;                          /* Number of scheduler nodes */
#endif /* FEATURE_HDR_QOS*/


  /*Unused Variables*/
  uint32  dsrlp_rsq_max_chain_cnt;            /* Max  to remove from rsq   */

} dsrlpi_cfg_info_type;



/*---------------------------------------------------------------------------
  Enum of the RLP states. Note that this state is an enum of all the states
  for all the versions of RLP.

  I.   RLP 3 can only take one of the following states:
        DSRLPI_SYNC_STATE
        DSRLPI_SYNC_ACK_STATE
        DSRLPI_ACK_STATE
        DSRLPI_RT_MEAS_STATE
        DSRLPI_ESTABLISHED_STATE
        DSRLPI_RESET_STATE

  II.  HDR RLP can only take one of the following states:
        DSRLPI_ESTABLISHED_STATE - Ready for transferring  data.
        DSRLPI_RESET_STATE       - Reset is in progress.
        DSRLPI_NULL_STATE        - State when traffic channel is down. No RLP.

  III. HDR MultiFlow RLP can take one of the following states:
        DSRLPI_ESTABLISHED_STATE - RLP is Active Open, TC is Up and ready to
                                   transfer data
        DSRLPI_RESET_STATE       - RLP is Active Open, TC is Up, reset in
                                   progress
        DSRLPI_NULL_STATE        - RLP is Active Open, but TC is down
        DSRLPI_CLOSE_STATE       - RLP is Active but Closed. TC state is
                                   irrelevant
        DSRLPI_INACTIVE_STATE    - RLP is Inactive. TC state is irrelevant
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLPI_SYNC_STATE = 0,           /* Tx SYNCs                             */
  DSRLPI_SYNC_ACK_STATE = 1,       /* Tx SYNC/ACKs                         */
  DSRLPI_ACK_STATE = 2,            /* Tx ACKs - calc. RTT                  */
  DSRLPI_RT_MEAS_STATE = 3,        /* Tx idle/data - calc. RTT             */
  DSRLPI_ESTABLISHED_STATE = 4,    /* Steady state operation               */
  DSRLPI_RESET_STATE = 5,          /* Reset                                */
  DSRLPI_NULL_STATE = 6,           /* Null state. No (HDR) RLP             */
  DSRLPI_CLOSE_STATE = 7,          /* RLP is Closed. Cannot transmit       */
  DSRLPI_INACTIVE_STATE = 8        /* RLP is deactivated(Only for HDR MRLP */
                                   /* This state is not needed if the RLP is not initialized when it is not activated! */
} dsrlpi_state_enum_type;

typedef struct
{
  uint16  seek_offset;        /* local seek offset for tx queue      */
  uint32  next_seq_num;       /* local next sequence number to use   */
  boolean txed_flush_pkt;     /* Tells if we built a flush packet
                                 and gave to PCP or not */

  /* Following two variables are required, to update the status of
    bytes transferred, and are used to count only the new data packet
    handles are are created in a subframe for a particular combo */
  uint16  new_handle_status_rcvd_count; /* handle data count for tx new data   */
  uint16  new_handles_built_count;  /* local count for tx new rlp packet   */

} dsrlpi_local_state_type;

/*---------------------------------------------------------------------------
  Struct used for storing rlp header info
---------------------------------------------------------------------------*/
typedef struct
{
  uint8     link_id;          /* Physical layer link on which pkt was rcvd */
  uint8     scheduler_group_id; /* SchedGrpId of carrier on which rcvd     */
  uint8     link_flow_id;
  uint8     route;
  uint8     qn_seq_included;  /* QNSeqIncluded field : MMPA only  */
  uint32    qn_seq_num;       /* QN Sequence Number : MMPA only   */
  uint32    seq_num_short;
  uint32    seq_num;
  uint8     first_data_unit;
  uint8     last_data_unit;
  uint32    end_seq_num;
  uint8     qn_pkts_merged;
} dsrlpi_header_type;

 /*--------------------------------------------------------------------------
 This structure holds the pointer to the fwd pkt received and the RLP
 header information that has been extracted from the packet.
 --------------------------------------------------------------------------*/
typedef struct
{
  q_link_type    link;       /* Queue link used to queue/dequeue the entry */
  dsm_item_type       *fwd_pkt_ptr;
  dsrlpi_header_type   rlp_header;
} dsrlpi_pkt_info_type;

typedef enum
{
  DSRLPI_RX_PROC_EVENT_PKT_RECV = 0,
  DSRLPI_RX_PROC_EVENT_RESET = 1,
  DSRLPI_RX_PROC_EVENT_ABORT = 2,
  DSRLPI_RX_PROC_EVENT_FLUSH = 3

} dsrlpi_rx_processing_event_type;

/*--------------------------------------------------------------------------
  This structure holds the RX processing state information to be logged
  for different events that can change the RLP state variables,
  namely V(n) and V(r). This structure is filled up before and after the
  event is handled and is logged immediately thereafter.
--------------------------------------------------------------------------*/
typedef struct
{
  dsrlpi_rx_processing_event_type   event_type;
  uint32  vn_before;            /* V(N) before the event handling         */
  uint32  vn_after;             /* V(N) after the event handling          */
  uint32  vr_before;            /* V(R) before the event handling         */
  uint32  vr_after;             /* V(R) after the event handling          */

  /* Following fields are meaningful only for DSRLPI_RX_PROC_EVENT_PKT_RECV event */
  uint8   link_id;              /* link on which the pkt was received     */
  uint8   scheduler_group_id;   /* schdgrp_id associated with the link    */
  uint8   qn_seq_included;      /* QN seq # included in the RLP packet    */
  uint16  qn_seq_num;           /* If included, the seq num               */
  uint32  sar_seq_short_num;    /* SAR sequence short                     */
  uint32  sar_seq_num;          /* SAR seq interpreted from short         */
  uint32  num_data_units;       /* data units in the RLP pkt              */

} dsrlpi_rx_processing_state_info;

/*---------------------------------------------------------------------------
  RLP state information definition. This structure contains all of the RLP
  state information for the currently active RLP session.
---------------------------------------------------------------------------*/
typedef struct
{
  dsrlpi_state_enum_type state;   /* RLP State                             */
  boolean ppp_data_transfer_after_dormancy;   /* Was PPP data transferred
                                              after waking up from dormancy*/

  /***************************** RECIEVE VARIABLES ************************/
  /*                           Common variables                           */
  uint32  expected_seq_num;       /* V(R), Next sequence number expected   */
  uint32  needed_seq_num;         /* V(N), Next sequence number needed     */

  q_type  buffered_rx_q;          /* Queue of rx pkts buffered before
                                     being sorted and processed by RLP     */

  dsrlpi_rx_processing_state_info   rx_proc_state; /* Store the state info
                                     variables to be logged for RX processing */
  /*******************************HDR Specific Transmit RLP variables *******/
  boolean reset_rx_ind_to_send;    /* MRLP needs to send ResetRxInd */
  boolean reset_tx_ind_ack_to_send;/* MRLP needs to send ResetTxIndAck */

  /*******************************1X Specific Recieve RLP variables *********/
  boolean ack_to_send;            /* pending ack (recv sync_ack) to send   */
  uint32  peer_vn;                /* Other side's V(N)                     */
  word    contig_erase;           /* Contiguous erasures                   */
  word    rlp_delay;              /* RLP_DELAY, Round trip time in frames  */
  word    sync_delay;             /* keep track how long it takes to sync  */
  boolean hole_active;            /* TRUE when RX stream has a hole        */


  /*************************** TRANSMIT VARIABLES *************************/
  /*                           Common variables                           */

  uint32  next_seq_num;           /* V(S), Next sequence number to use     */
  boolean naks_to_send;           /* Does RLP have NAKs to send            */
  boolean quick_repeat_naks_to_send; /* Does RLP have Quick Repeat NAKs to send */

  q_type  nak_q;                  /* NAK list for this version             */
  q_type  delayed_nak_q;          /* DELAYED NAK list                      */
  q_type  multilink_nak_q;        /* MultiLink NAK list for RevB           */
  q_type  quick_repeat_nak_q;     /* DelayedNak items pending Quick Repeat */
  q_type  reseq_q;                /* Resequencing q for this version       */
  q_type  tx_q;                   /* The transmit q for this version       */
  dsm_item_type*  tx_q_recent;    /* pts to last accessed itme from txq    */

  uint32  rtxq;                   /* retransmit queue pointer              */


  /*******************************HDR Specific Transmit RLP variables *******/


  boolean reset_tx_ind_to_send;   /* MRLP Needs to send ResetTxInd?   */
  boolean reset_tx_complete_to_send;   /* MRLP Needs to send ResetTxCompl? */
  boolean reset_rx_complete_to_send;   /* MRLP Needs to send ResetRxCompl? */

  boolean rst_to_send;            /* pending RST signaling msg to send     */
  boolean rst_ack_to_send;        /* pending RST ACK signaling msg to send */
  boolean mob_initiated_reset;    /* mobile initiated the reset procedures */

  uint16  seek_offset;              /* seek offset for tx queue            */
  boolean reset_active;           /* is a reset occuring right now?        */
  q_type  tx_item_q;                /* items dequeued for tx               */

  rex_crit_sect_type data_path_crit_sect;

#ifdef FEATURE_HDR
  boolean flush_timer_expired;    /* build flush pkt?                      */
  boolean flush_giveup_timer_expired;    /* give up sending flush pkt      */
  boolean abort_timer_expired;    /* has the abort timer expired?          */
  boolean delayed_nak_timer_expired; /* has the delayed nak timer expired? */
#ifdef FEATURE_HDR_QOS
  hdrmrlpsched_wm_node_type *sched_rr_node_ptr; /* Pointer to the round robin scheduler node */
#endif /* FEATURE_HDR_QOS */

  dsrlpi_local_state_type local_state[HDRCP_PCP_MAX_COMBOS];  /* Local State
                                  each combo, this is reset after
                                  every subframe */
#endif /* FEATURE_HDR */
  q_type hdr_rtxq;

  /********************HDR REV-B Specific Transmit RLP Variables************/
  uint8 last_carrier; /* The previous carrier for which DATA was built */
  uint8 last_combo;   /* The previous combo for which DATA was built   */
  uint8 marq_index;   /* The Current MARQ index into the MARQ array,   *
                       * where the handles are to be added             */
  uint8 marq_id;      /* 8bit number that keeps rolling, so to make    *
                       * the handle_set_id unique                      */
  uint8 combo_index;  /* Index into the current combo instance that is *
                       * being treated                                 */
  uint8 handles_built;/* Total count of handles built in this subframe,*
                       * This is a Debug Variable, that is used for    *
                       * sanity                                        */
  q_type hdrrlpmarq_info[DSRLP_MARQ_NUM_SUBFRAMES]; /* Each element of *
                       * this array stores the information regarding   *
                       * the RLP packets transmitted in that           *
                       * particular subframe.                          */
#ifdef FEATURE_HDR
  dsrlpi_per_combo_type handles_per_combo[HDRCP_PCP_MAX_COMBOS];
#endif
                      /* Set handles per combo that are given to PCP   */

  /************************** 1X Specific Transmit RLP variables************/
  // TODO: Check the usability of this variable for HDR
  dsm_item_type* tx_item_ptr;     /* Ptr holding item last dequeued for tx */

  uint32  del_fr_id;              /* delayed frame arrays id               */
  uint8   chan_assignment;        /* Channels assigned at this point       */
  uint32  ext_seq;                /* EXT_SEQ in case of encryption         */
  uint16  fwd_ddw;                /* DDW for Rx                            */
  uint16  fwd_rexmit_time;        /* Delay time for subsequent naks        */
  q_type  fwd_pre_q;              /* PreQ for Rx                           */
  q_type  fwd_input_q;            /* Input queue for Rx where Mux enqueues */
  word    old_frames;             /* number old frames rx'ed consecutively */
  boolean big_nak_gap;            /* prev new/idle resulted in big nak gap */
  boolean reset_rlp;              /* Reset flag for this RLP               */

  byte    saved_frames_index;     /* index into saved frames array         */
  byte    saved_frames_cnt;       /* num of saved data frames in buf       */

  boolean idle_timer_active;      /* if the idle timer is active           */
  byte    idle_timer;             /* used when xmitting idle/blank frames  */
  byte    idle_fr_xmit_cntr;      /* #idle frames sent due to idle timer   */
                                  /* expiration                            */
  byte    rnd_trip_fr_cntr;       /* used to determine if idle frame should*/
                                  /* be sent as a data frame               */
  word    last_fill_nak_cnt;      /* number fr's since a nak or gap sent   */
  uint32  mux_erasure_cnt;        /* mux erasures rx'ed in past 100 fch frs*/

  byte    nak_rounds_fwd;
  byte    naks_per_round_fwd[DSRLP_MAX_NAK_ROUNDS];
  byte    nak_rounds_rev;
  byte    naks_per_round_rev[DSRLP_MAX_NAK_ROUNDS];

  boolean reseq_q_more_data;      /* more data chains for rx watermark     */

  dsm_item_type *reseq_last_chain_ptr;/* last dsm  chain on q              */
  dsm_item_type *reseq_last_item_ptr; /* last DSM item of last chain on q  */
  dsm_item_type *reseq_last_insert_chain_ptr;/* point to most recent insert*/
  dsm_item_type *reseq_last_insert_item_ptr; /* last DSM item of chain     */

} dsrlpi_state_info_type;

/*---------------------------------------------------------------------------
  QN instance assigned parameters
---------------------------------------------------------------------------*/
typedef struct
{
  uint64 qn_id;         /* QN ID assigned to this QN instance              */
  uint8  schdgrp_id;    /* Scheduler group id of the QN instance           */
  uint8  num_carriers;  /* Number of carriers in this QN instance          */

} dsrlpi_qn_info_type;

/*---------------------------------------------------------------------------
  QN instance stats info
---------------------------------------------------------------------------*/
typedef struct
{
  uint64  qn_create_time;     /* Time when this QN instance was activated  */
  uint64  qn_delete_time;     /* Time of deactivation                      */
  uint64  rx_new_data_bytes;  /* New bytes received by this QN instance    */
  uint64  rx_new_data_frames; /* New frames received by this QN instance   */
  uint32  quick_nak_ind_cnt;  /* Quick nak indications generated           */

} dsrlpi_qn_stats_info_type;

/*---------------------------------------------------------------------------
  QN instance state information
---------------------------------------------------------------------------*/
typedef struct
{
  uint64  qn_id;                      /* Unique QN ID                      */
  uint8   schdgrp_id;                 /* Scheduler group id for this QN    */
  uint8   num_carriers;               /* Number of carriers associated
                                         with this QN instance             */
  boolean activated;                  /* Is this qn instance activated     */
  boolean marked_for_deletion;        /* QN needs to be deactivated        */
  uint32  expected_seq_num;           /* Next expected QN seq number       */
  uint32  largest_sar_seq;            /* Largest SAR seq number rcvd by
                                         this QN instance                  */
  uint32  prev_sar_seq;               /* Previous sar seq number rcvd by
                                         this QN instance                  */
  uint64  last_pkt_timestamp_in_ms;   /* Time when the last pkt was rcvd   */
  boolean pending_quick_nak;          /* Was a hole detected in QNseq      */
  boolean include_leading_edge;       /* If hole was detected in last round,
                                         should leading edge be included in
                                         multilink nak                     */
  boolean first_pkt_on_qn;            /* Is this the first packet on this
                                         QN instance                       */
  boolean largest_seq_num_reported;   /* Has the largest seq num rcvd on
                                         this QN instance been reported in
                                         a previous multilink nak? Used to
                                         fill in OtherQN field in ml-nak   */
  dsrlpi_qn_stats_info_type stats;    /* QN instance statistics info       */
} dsrlp_qn_ctl_type;

/*---------------------------------------------------------------------------
  RLP state and stats for one route
---------------------------------------------------------------------------*/
typedef struct dsrlpi_rlp_ctl_blk_type_tag
{
  byte route;                     /* DSRLP_ROUTE_*                         */
  const dsrlpi_cfg_info_type* cfg_ptr;  /* reference config info           */

  dsrlpi_state_info_type state;   /* state information                     */
  dsrlp_stats_info_type  stats;   /* statistical information               */

  dsrlp_qn_ctl_type  qn_instance_table[DSRLP_MAX_QN_INSTANCES_PER_RLP];

#ifdef FEATURE_HDR
  rex_timer_type     flush_timer; /* flush timer to re-send last octet     */
  rex_timer_type     flush_giveup_timer; /* timer to stop the flush attempt */
  rex_timer_type     reset_timer; /* fail-safe RLP reset state timer       */
  rex_timer_type     abort_timer; /* abort timer to delete nak list entry  */
  rex_timer_type     delayed_nak_timer; /* delayed nak timer               */
  boolean            abort_timer_def; /* is the abort timer defined yet?   */
  boolean            delayed_nak_timer_def; /* is the delayed nak timer defined yet?   */
#endif /* FEATURE_HDR */
} dsrlpi_rlp_ctl_blk_type;


/*---------------------------------------------------------------------------
  Flow session control block definition. This structure contains all
  session information and is comprised of configuration, RLP state, and
  statistical information.

  1x/DPA: this represents an RLP instance

  MFPA/: this represents an RLP Flow

  EMPA: this represents a Link Flow; including two RLPs
---------------------------------------------------------------------------*/
typedef struct
{
  dsrlpi_cfg_info_type    cfg;     /* configuration information            */
  boolean is_hdrmrlp_active;       /* State for HDR MRLP. RLP Active or not*/
  dsrlpi_rlp_ctl_blk_type rlp_list[DSRSP_NUM_ROUTES]; /* RLP instances     */
} dsrlpi_sess_ctl_blk_type;

extern  dsrlpi_sess_ctl_blk_type    dsrlpi_rscb_array[ DSRLP_MAX_RSCB_ELEMENTS];

#define DSRLPI_INVALID_RSCB_INDEX DSRLP_MAX_RSCB_ELEMENTS

/*---------------------------------------------------------------------------
  Mapping betwen the service provider's sr_id and index in dsrlpi_rscb_array.
  Internally, the code supports 2 RLP sessions with array indices 1 and 2.
  However, sr_id is a 3 bit field in MUX PDU3 and the sr_id for a given call
  assigned by the service provider can be anywhere from 1 to 6 (7 is reserved).

  For HDR, the sr_id is not assigned by the basestation, but rather they are
  internally usd values. Since we do not want collision with the CDMA sr id
  values, values out of cdma range (7 and 8) are used for HDR.
---------------------------------------------------------------------------*/

#define DSRLPI_MAX_CDMA_SR_ID_VALUE 7
#define DSRLP_HDR_SN_RLP_SR_ID_VALUE   (DSRLPI_MAX_CDMA_SR_ID_VALUE)
#define DSRLPI_HDR_AN_AUTH_SR_ID_VALUE (DSRLPI_MAX_CDMA_SR_ID_VALUE + 1)
#define DSRLPI_MAX_SP_SR_ID_VALUE      (DSRLPI_MAX_CDMA_SR_ID_VALUE + 2)


/*---------------------------------------------------------------------------
  Maximum number of RLP sessions allowable at one time.
---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_QOS
#define DSRLPI_MAX_SESSIONS 9
#else
#define DSRLPI_MAX_SESSIONS 3
#endif

/*---------------------------------------------------------------------------
  This constant is used to decide whether RLP should get a SMALL ITEM or a
  LARGE ITEM. The size of the requested item is right shifted (divided by 4)
  by this number. If the result is larger than DSM_SMALL_ITEM_SIZ, a small
  item is allocated otherwise a large item is allocated.
---------------------------------------------------------------------------*/
#define DSRLPI_ITEM_SIZ_DIVISOR 2


/****************************************************************************/
/*************************************  1X SPECIFIC ************************/
/****************************************************************************/

/*---------------------------------------------------------------------------
  Type for NAK list entry. The NAK list is used to keep track of missing
  RLP data frames, and the associated retransmission/abort timers.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type    link;       /* Queue link used to queue/dequeue the entry */
  uint32         first_seq;  /* 1st missing Sequence number                */
  uint32         last_seq;   /* last missing Sequence number               */
  byte           age;        /* Delta/Age of the NAK list entry, in frames */
  byte           curr_rnd;   /* Which round are we on for this NAK         */
  byte           naks_left;  /* How many NAKs left in this round           */

} dsrlpi_nak_list_entry_type;

/*---------------------------------------------------------------------------
  Type for information extracted from the received frame. Used for frame
  type based processing. Information includes seq number, frame category, etc
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 seq_num;                                /* Frame seq num          */
  uint32 fr_cat;                                 /* Frame category bitmask */
  dsrlp_mux_pdu_enum_type mux_pdu_type;          /* mux pdu of this frame  */
} dsrlpi_curr_fr_info_type;


/*---------------------------------------------------------------------------
  RLP function pointer table.
  Each function pointer represents specific processing per RLP version.
---------------------------------------------------------------------------*/
struct dsrlpi_fcn_ptr_table
{
  dsrlpi_fr_det_enum_type (*rx_frame_det)(dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_fwd_frame_type   *fwd_fr_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr);

  dsrlpi_fr_det_enum_type (*rx_new_data) (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsm_item_type    **fr_item_pkt_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr);

  dsrlpi_fr_det_enum_type (*rx_rexmit)   (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_fwd_frame_type *fwd_fr_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr);

  dsrlpi_fr_det_enum_type (*rx_init)     (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr);

  dsrlpi_fr_det_enum_type (*rx_idle)     (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr);

  dsrlpi_fr_det_enum_type (*rx_rlp_erasure)
                                         (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_fwd_frame_type *fwd_fr_ptr);

  dsrlpi_fr_det_enum_type (*rx_mux_erasure)
                                         (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_fwd_frame_type *fwd_fr_ptr);

  boolean (*tx_new_data)                 (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);

  boolean (*tx_rexmit)                   (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);

  boolean (*tx_nak)                      (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);

  boolean (*tx_init)                     (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);

  boolean (*tx_idle)                     (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);
  boolean (*tx_fill)                     (dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);
};

/*---------------------------------------------------------------------------
  Bit mask positions for different frame categories. This is used in
  augmenting sub-categories of the major frame processing type in the
  dsrlpi_fr_det_enum_type. Frame determination extracts this extra piece of
  information.
---------------------------------------------------------------------------*/
#define CURR_FRAME_SYNC_MASK               0x01  /* SYNC frame indication  */
#define CURR_FRAME_ACK_MASK                0x02  /* ACK frame indication   */
#define CURR_FRAME_LST_BIT_MASK            0x04  /* LST bit indication set */
#define CURR_FRAME_RESET_MASK              0x08  /* RST bit indication set */
#define CURR_FRAME_SEQ_MASK                0x10  /* 8-bit SEQ OTA present  */
#define CURR_FRAME_SEQ_HI_MASK             0x20  /* 4-bit SEQ_HI present   */
#define CURR_FRAME_S_SEQ_MASK              0x40  /* 12-bit S_SEQ present   */
#define CURR_FRAME_IN_DELQ_MASK            0x80  /* from delay fr arrays   */
/*---------------------------------------------------------------------------
  Mask to detect whether a frame is a sync, sync/ack or ack.
---------------------------------------------------------------------------*/
#define CURR_FRAME_SYNC_ACK_DETECT_MASK    0x03

/*---------------------------------------------------------------------------
  This is the max number of dsm item chains to be removed with a single call
  to the dsrlprsq_remove() from the resequencing Q. The reason is every dsm
  item chain causes a separate dsm_enqueue() operation.

  Make sure to remove more chains than can be received in one frame time.
---------------------------------------------------------------------------*/
#define DSRLPI_MAX_CHAINS_TO_REMOVE  (RLP_MAX_FWD_PDU * 3)


/*---------------------------------------------------------------------------
  #defines used in RLP 3 mostly.  The Round trip frame counter is used to
  make sure some idles are sent on the DCCH, so that the base station can
  calculate it's RLP Delay.  The idle timer makes sure some frames does go
  out the pipe...but only up to the frame transmission counter limit.
---------------------------------------------------------------------------*/
#define  DSRLPI_RND_TRIP_FR_CNTR_DEF     4    /* rnd trip frame counter def*/
                                              /* value.  must be >= 4      */
#define  DSRLPI_IDLE_TIMER_DEF           5    /* idle timer default value  */
#define  DSRLPI_IDLE_FR_XMIT_CNTR_DEF    3    /* idle frame xmission cntr  */
                                              /* default value.            */
/*---------------------------------------------------------------------------
  Count of how many FCH frames can be sent before forcing a FILL frame. This
  is so the peer's V(N) updates correctly.  Sending a NAK is like sending
  a FILL,as that also updates the other side to our V(N).

  Define the number as 80;  Since seq. number ambiguity starts at 255, this
  allows 3 tries to get the V(N) info to the other side.
---------------------------------------------------------------------------*/
#define DSRLPI_LAST_FILL_NAK_GAP   80

/*---------------------------------------------------------------------------
  Maximum time (in frames) to synch up RLP. Set real high for MTOM
  Korea builds.
---------------------------------------------------------------------------*/
#define DSRLPI_MAX_RTT_SETUP       900

/*---------------------------------------------------------------------------
  Type and table that contains primary and secondary channel full rate mask,
  TYPE, and Payload fields values.
---------------------------------------------------------------------------*/
typedef struct
{
  byte type_mask;         /* Field that contains the TYPE field mask       */
  byte format_a;          /* TYPE field Format A value                     */
  byte format_bc_new;     /* TYPE field Format B/C new data frames         */
  byte format_bc_rexmit;  /* TYPE field Format B/C rexmit data frames      */
  byte format_d;          /* TYPE field Format D value                     */
} dsrlpi_type_mask_table_type;

/*---------------------------------------------------------------------------
  Table is indexed by MuxPDU type and service class (primary or secondary).
---------------------------------------------------------------------------*/
extern const dsrlpi_type_mask_table_type dsrlpi_type_mask_table[4][3];

#define DSRLPI_BIG_NAK_GAP   (3 * (8 + 1))

/*---------------------------------------------------------------------------
  Bytes used in printing out RLP-level erasures, to help in debugging.
---------------------------------------------------------------------------*/
extern byte dsrlpi_ctl_byte1,dsrlpi_ctl_byte2,dsrlpi_ctl_byte3;
extern byte dsrlpi_ctl_used;

/****************************************************************************/
/*************************************  UNUSED   *************************/
/****************************************************************************/
#if 0
/*---------------------------------------------------------------------------
  This is the modulus used when representing the sequence numbers as
  32-bit numbers.
---------------------------------------------------------------------------*/

#define DSRLPI_32BIT_SEQ_MODULUS     0x10000000   /* 2 to the 28th power   */

/*---------------------------------------------------------------------------
  Bitmask used to say when a 32-bit number is generated, whether there
  was a 4-bit SEQ_HI, 8-bit SEQ, or 12-bit S_SEQ included in the info.
  For example, in an unsegmented data frame, only the 8-bit SEQ is put into
  the header.  Therefore, for the functions called, it is not known if
  the 4-bit SEQ_HI is really 0000, or if it is 0000 because frame
  determination did not know what the value wa.
---------------------------------------------------------------------------*/
#define DSRLPI_SEQ_HI_MASK   0x01
#define DSRLPI_SEQ_MASK      0x02
#define DSRLPI_S_SEQ_MASK    0x04
#endif

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/
/****************************************************************************/
/******************************1X SPECIFIC ****************************/
/****************************************************************************/

/*===========================================================================

MACRO          DSRLPI_HAS_S_SEQ_MASK

DESCRIPTION    Tells if the input byte has the s_seq mask bit set.

PARAMETERS     a byte.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the bit was set, false otherwise

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_HAS_S_SEQ_MASK(x) \
       ((x & CURR_FRAME_S_SEQ_MASK) ? TRUE : FALSE)


/*===========================================================================

MACRO          DSRLPI_HAS_LAST_SEG_MASK

DESCRIPTION    Sees if the last seg bit is set.

PARAMETERS     a 32-bit uint.

DEPENDENCIES   None.

RETURN VALUE   TRUE if it is set, FALSE otherwise.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_HAS_LAST_SEG_MASK(x) \
       (((x & CURR_FRAME_LST_BIT_MASK) || !( x & CURR_FRAME_S_SEQ_MASK)) \
        ? TRUE : FALSE)

/*===========================================================================

MACRO          DSRLPI_HAS_SEQ_HI_MASK

DESCRIPTION    Tells if the input byte has the seq_hi mask bit set.

PARAMETERS     a byte.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the bit was set, false otherwise

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_HAS_SEQ_HI_MASK(x) \
       ((x & CURR_FRAME_SEQ_HI_MASK) ? TRUE : FALSE)

/****************************************************************************/
/******************************UNUSED MACROS ****************************/
/****************************************************************************/

#if 0
/*===========================================================================

MACRO          DSRLPI_SET_SEQ_HI_MASK

DESCRIPTION    Sets the SEQ_HI in the input var.

PARAMETERS     a byte.  But this is an expression, and assumes that the call
               looks like this:

               x = DSRLPI_SET_SEQ_HI_MASK(x);

DEPENDENCIES   None.

RETURN VALUE   The changed byte.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_SET_SEQ_HI_MASK(x) \
       (x | CURR_FRAME_SEQ_HI_MASK)
#endif


#if 0
/*===========================================================================

MACRO          DSRLPI_SET_SEQ_MASK

DESCRIPTION    Sets the SEQ bit in the input var.

PARAMETERS     a byte.  But this is an expression, and assumes that the call
               looks like this:

               x = DSRLPI_SET_SEQ_MASK(x);

DEPENDENCIES   None.

RETURN VALUE   The changed byte.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_SET_SEQ_MASK(x) \
       (x | CURR_FRAME_SEQ_MASK)
#endif

#if 0
/*===========================================================================

MACRO          DSRLPI_HAS_SEQ_MASK

DESCRIPTION    Tells if the input byte has the seq mask bit set.

PARAMETERS     a byte.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the bit was set, false otherwise

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_HAS_SEQ_MASK(x) \
       ((x & CURR_FRAME_SEQ_MASK) ? TRUE : FALSE)
#endif

#if 0
/*===========================================================================

MACRO          DSRLPI_SET_S_SEQ_MASK

DESCRIPTION    Sets the S_SEQ bit in the input var.

PARAMETERS     a byte.  But this is an expression, and assumes that the call
               looks like this:

               x = DSRLPI_SET_S_SEQ_MASK(x);

DEPENDENCIES   None.

RETURN VALUE   The changed byte.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_SET_S_SEQ_MASK(x) \
       (x | CURR_FRAME_S_SEQ_MASK)
#endif

#if 0
/*===========================================================================

MACRO          DSRLPI_SET_LAST_SEG_MASK

DESCRIPTION    Sets the last segment bit indicator in the fr_cat mask.

PARAMETERS     a 32-bit uint.

DEPENDENCIES   None.

RETURN VALUE   NONE

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_SET_LAST_SEG_MASK(x) \
       (x | CURR_FRAME_LST_BIT_MASK)
#endif


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/****************************************************************************/
/******************************COMMON FUNCTIONS ****************************/
/****************************************************************************/

/*===========================================================================

FUNCTION DSRLPI_INIT_CONSTANTS

DESCRIPTION
  This function is called from dsrlp_init. All the version specific constants
  including function pointers are initialized here.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsrlpi_init_constants
(
  dsrlpi_sess_ctl_blk_type  *rscb_ptr,       /* rlp-specific control block */
  dsrlp_ver_enum_type        ver             /* RLP version                */
);
/*===========================================================================

FUNCTION       DSRLPI_RESET

DESCRIPTION    Resets RLP. This involves clearing various values and
               resetting the RLP state machine.

               Note, stats.resets counter is incremented outside this
               function, because it is only incremented for RLP resets in
               the middle of a call (error conditions).

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpi_reset
(
  dsrlpi_rlp_ctl_blk_type        *rrcb_ptr   /* rlp-specific control block */
);

/****************************************************************************/
/******************************1X SPECIFIC FUNCTIONS ***********************/
/****************************************************************************/

/*===========================================================================

FUNCTION       DSRLPI_GET_BEST_FR_RATE

DESCRIPTION    This function returns the smallest available rate that a frame
               of the given length can be transmitted at. RLP conveys this
               rate to the Mux layer.

DEPENDENCIES   Assumes that non-NULL pointers are passed in.
               This will only work for MuxPDU type1 and MuxPDU type2.

RETURN VALUE   Returns the rate at which this frame should be sent at.

SIDE EFFECTS   NONE
===========================================================================*/
dsrlp_rate_enum_type dsrlpi_get_best_fr_rate
(
   dsrlp_rev_frame_type    *rev_fr_ptr,  /*  contains rev frame info.      */
   word                     fr_len       /* Num of bytes in frame          */
);

/*===========================================================================

FUNCTION       DSRLPI_RX_INIT_FRAME

DESCRIPTION    This function handles incoming SYNC, SYNC/ACK, and ACK frames
               and increments/resets the RLP initialization state machine
               accordingly.

DEPENDENCIES   Assumes that the incoming frame is either a SYNC, SYNC/ACK,
               or ACK.

RETURN VALUE   Always returns success DSRLPI_DONE.

SIDE EFFECTS   Depending on the state and incoming frame, the round trip
               delay timer may be incremented.
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_rx_init_frame
(
  dsrlpi_rlp_ctl_blk_type   *rrcb_ptr,       /* rlp-specific control block */
  dsrlpi_curr_fr_info_type  *frame_info_ptr  /* current frame information  */
);

/*===========================================================================

FUNCTION       DSRLPI_RX_NEW_DATA

DESCRIPTION    This function receives new data (segmented or unsegmented)
               and processes it.  If there is skew in the incoming seq
               number, then some Nak generation may occur.

               This code is same for all versions of RLP.

DEPENDENCIES   CRC checks done by frame determination.
               Input pointers are non-NULL.
               The dsm_item embedded within fwd_fr_ptr contains only the RLP
                frame's data payload, and does not contain any header info
                or CRC info.

RETURN VALUE   Always returns DONE.

SIDE EFFECTS   V(R) and/or V(N) may get updated.  Entries may be inserted
               into the NAK list.  Entries may be added or removed from
               the resequencing buffer.
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_rx_new_data
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block       */
  dsm_item_type            **fr_item_pkt_ptr,
  dsrlpi_curr_fr_info_type *curr_info_ptr    /* frame info ptr             */
);


/*===========================================================================

FUNCTION       DSRLPI_PROCESS_REXMIT_FRAME

DESCRIPTION    This function will process re-tansmitted frames. It will
               first remove them from the nak list and then insert into the
               resequencing queue. After that any frames that can be passed
               to higher layers will be moved from the resequencing q into
               the rx watermark structure.

DEPENDENCIES   Non-null pointers passed into this function

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_process_rexmit_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  dsrlp_fwd_frame_type         *fwd_fr_ptr, /* ptr to received RLP frame   */
  dsrlpi_curr_fr_info_type     *fr_info_ptr /* current frame info          */
);

/*===========================================================================

FUNCTION        DSRLPI_CRC_GEN

DESCRIPTION    This function will generate the CRC for the passed byte stream
               and store the CRC at the end of the passed stream.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlpi_crc_gen
(
  byte *buf,          /* pointer to Frame buffer to put CRC at the end     */
  word len            /* length of the Frame buffer, including CRC bytes   */
);

/*===========================================================================

FUNCTION       DSRLPI_GET_MIN_FRAME_AND_LEN

DESCRIPTION    This function returns the minimum frame size supported by
               a reverse frame and also returns the length (in bytes) of
               that frame.  Values are returned via passed-in ptrs.

               The user can specify a minimum size (i.e, the returned
               frame size must be at least a certain frame rate).

               If the proper frame is not found, then the input parameters
               are unchanged.

DEPENDENCIES   Assumes that the frame rate bitmasks are defined such
               that the smallest frame rate is 0x01, and each successively
               larger frame rate is that value shifted one bit to the left.

               Assumes that non-NULL pointers are passed in.

               Assumes that the input min_fr_rate is equivalent to one
               of the frame rate #defines.

RETURN VALUE   TRUE if the frame info could be generated.  FALSE if the
               frame info could not be generated (for example, the
               requested minimum size was not supported by the rev frame).

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_get_min_frame_and_len
(
   dsrlp_rev_frame_type    *rev_fr_ptr,  /*  contains rev frame info.      */
                                         /* fr_rate_ptr updated in function*/
   uint16                   min_fr_rate, /*  min frame rate needed, using  */
                                         /*  bitmask                       */
   byte                    *fr_len       /*  returned frame len (bytes)    */
);
/*===========================================================================

FUNCTION       DSRLPI_TX_NEW_DATA_FRAME

DESCRIPTION    This function constructs an new data frame from payload
               available in the transmit watermark.

               Only non segmented new frames are constructed.

               This code is specific to RLP 1, RLP2 and RLP3.

DEPENDENCIES   Since idle frames can only go on fundicated channels,
               the assumption is that only one idle frame can be sent
               per 20msec frame-time.

               rrcb_ptr->state.idle_timer is only decremented in this
               function.

RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               Returning FALSE in this case implies a Blank frame should
               be supplied to the MUX layer.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_tx_new_data_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  dsrlp_rev_frame_type         *rev_fr_ptr  /* Ptr to frame to be built    */
);

/*===========================================================================

FUNCTION       DSRLPI_REXMIT_DATA_FRAME

DESCRIPTION    This function constructs an new data frame from payload
               available in the transmit watermark.

               Only non segmented new frames are constructed.

               This code is specific to RLP 1, RLP2 and RLP3.

DEPENDENCIES   Since idle frames can only go on fundicated channels,
               the assumption is that only one idle frame can be sent
               per 20msec frame-time.

               rrcb_ptr->state.idle_timer is only decremented in this
               function.

               This function assumes RLP is in DSRLPI_ESTABLISHED_STATE
               or DSRLPI_RT_MEAS_STATE. This function should not be called
               if RLP cannot transmit new data frames.

RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               Returning FALSE in this case implies a Blank frame should
               be supplied to the MUX layer.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_rexmit_data_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  dsrlp_rev_frame_type         *rev_fr_ptr  /* Ptr to frame to be built    */
);

/*===========================================================================

FUNCTION      DSRLP_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to RLP.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_process_cmd
(
  ds_cmd_type  *ds_cmd_ptr
);

#endif  /* FEATURE_DS */

#endif  /* _DSRLPI_H */

