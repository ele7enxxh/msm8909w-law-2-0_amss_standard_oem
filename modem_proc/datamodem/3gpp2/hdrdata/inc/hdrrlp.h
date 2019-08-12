#ifndef _HDRRLP_H
#define _HDRRLP_H

/*===========================================================================

             H D R   R L P   E X T E R N A L   H E A D E R  F I L E

DESCRIPTION
   This file contains external variable and function declarations for the
   High Data Rate (HDR) Radio Link Protocol.

   This should be included by files which interface with RLP, like the
   PCP and SLP.

Copyright (c) 2000-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/07/11   msh        Replaced FEATURE_HDR_RL_FLOW_CONTROL by FEATURE_DATA_RL_FLOW_CONTROL  
11/18/11   vpk     HDR SU API cleanup
08/16/11   vpk     Move hdr log packet definitions to hdrdata
02/08/11   ls      Global variable cleanup
09/04/09   ss      CMI SU level modifications.
04/09/09   ls      In hdrrlp_cta_clear_enhanced_idle_bit(), add parameter
                   enable_inactivity_timer to check before going to dormant
03/04/09   sa      AU level CMI modifications.
10/05/06   spn     Changed the function definition of
                   hdrrlp_populate_tx_data_q_level(), as it was calculating
                   from wrong data structures, which caused NULL ptr
                   dereferencing.
07/11/06   mpa     Added hdrrlp_get_pending_tx_data_queue_size() definition.
06/06/06   ksu     EMPA: RLP Rx / Sig code review comments
01/11/06   sy      Changed hdrrlp_process_pkt_payload() to hdrrlp_process_rlp_pkt()
11/15/05   ksu     EMPA: packet stream, octet sequencing (forward link also)
                   to process the RLP packet including the seq_num.
09/28/05   ksu/sy  HDR: use one-shot abort timer instead of 20ms ticks
09/28/05   ksu/sy  Call hdr[m]rlpi_rx_process_sig_msg() directly from hdr[m]rlp_msg_cb().
09/01/05   ksu/sy  RLP reset should clear flush timer.
10/11/05   ksu/sy  Eliminate hdrmrlpi_rx_process_nak()
10/10/05   ksu/sy  Combine payload processing of hdrrlp_put_pkt_cb() and hdrmrlp_put_pkt_cb()
09/30/05   ksu/sy  Eliminate hdrmrlp_get_new_or_flush_data() and hdrmrlp_get_retx_data()
08/08/05   sk/sy   Merged semi-dynamic QOS changes from Sandbox.
01/31/05   sy      Moved HDRRLP_NAK_ABORT_TIMER_VALUE from hdrrlp.c.
09/09/04   mpa     Preliminary QOS support.
10/13/02   vas     Removed hdrrlp_resume()
09/18/03   vas     Removed functions for register/deregister RLP callbacks
                   with PCP.
07/10/03   vas     Ifdef-ed the RLP stream Ids since they do not make sense
                   for MM architecture.
02/23/03   ht      Based on asw/COMMON/vcs/hdrrlp.h_v   1.13
                   Add changes to support HDR 3GData call.
05/09/02   hcg     Added macro that determines RLP reset state.
01/15/02   hcg     Updated initial value for NAK age.
12/01/01   vas     Added constant for Invalid RLP ID definition
09/22/01   rao     Fixed if defined, endif mismatch problem
09/20/01   rao     Added Partial RLP Frame logging support.
09/19/01   vas     Added hdrrlp_clear_stats()
09/16/01   vas     hdrrlp_cleanup() takes rscb_index instead of rlp_id
09/14/01   hcg     Updated hdrrlp_reg_srvc() prototype for boolean return
                   value.
08/28/01   vas     Added hdrrlp_reg_srvc()
08/17/01   vas     Added Support for registering hdrrlp on any stream.
                   rlp_put_pkt() & get_pkt() functions not externalized as
                   these are now registered with PCP thru callbacks. Removed
                   deprecated functions ind_cb(),flow_enable(),flow_disable()
04/02/01   hcg     Removed FEATURE_DS_HDRRLP and changed to just FEATURE_HDR.
02/29/00   hcg     Created module.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#if defined(FEATURE_HDR)
#include "dsat_v.h"
#include "dsm.h"
#include "hdrcom_api.h"
#include "task.h"
#ifdef FEATURE_DS
#include "dsrlpi.h"
#endif /* FEATURE_DS //DA */


#if defined(FEATURE_HDRRLP_LOGGING)
#include "hdrrlplogi.h"
#else
#include "errno.h"
#endif
#include "hdrrlptxq.h"
#include "hdrrlp_api.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_HDRRLP_LOGGING
/*---------------------------------------------------------------------------
   This structure is being defined to carry some MMPA specific info from the
   place where we process packet status (from PCP) to the hdrrlplog module.
   As of now only the carrier id and Tx_reason need to be propagated to the
   logging module.
---------------------------------------------------------------------------*/
typedef struct{
  uint8   carrier_id;       /* Carrier Id*/
  uint8   tx_reason;        /* Transmission reason*/
}hdrrlp_tx_partial_log_packet_info_type;

/*---------------------------------------------------------------------------
   This structure is being defined to be used in the interface for logging
   the TX RLP packets. This will be used in all App types (from DPA to MMPA
   and anything that might come up later. Info specific to each type can be
   included inside the union thus making is consistent and extensible
---------------------------------------------------------------------------*/
typedef struct{
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr;
  dsm_item_type               *rev_pkt_ptr;
  union{
    hdrrlp_tx_partial_log_packet_info_type mmpa_tx_partial_log_packet_info;
  }app_specific_info;
}hdrrlp_tx_packet_info_type;
#endif /* FEATURE_HDRRLP_LOGGING */


#define HDRRLP_RX_TASK_PTR &hdrrx_tcb
#define HDRRLP_TX_TASK_PTR &hdrtx_tcb

/*---------------------------------------------------------------------------
  HDR RLP NAK Abort Timer

  T(RLPAbort)
    The NAK abort timer is used to age entries in the NAK list until the
    "hole" is aborted, meaning that all in sequence octets will be passed
    to the higher layer.  Aging and aborting of NAK list entries are handled
    in the context of the RX task; the receive side processing.  As such,
    it cannot "piggyback" the frame build interrupt, and thus must have its
    own Rex timer to perform the aging.
---------------------------------------------------------------------------*/
#define HDRRLP_NAK_ABORT_TIMER_VALUE 500                /* value in ms     */

#define HDRRLP_INVALID_ID 0xFF

/* Theese fields of the RLP packet header are fixed length for EMPA */
#define HDRRLP_EMPA_LINK_FLOW_ID_LEN    5
#define HDRRLP_EMPA_ROUTE_LEN           1
#define HDRRLP_EMPA_FDU_LEN             1
#define HDRRLP_EMPA_LDU_LEN             1

/* Theese fields of the RLP packet header are fixed length for MMPA */
#define HDRRLP_MMPA_LINK_FLOW_ID_LEN    5
#define HDRRLP_MMPA_ROUTE_LEN           1
#define HDRRLP_MMPA_QNSEQ_INCLUDED_LEN  1
#define HDRRLP_MMPA_FDU_LEN             1
#define HDRRLP_MMPA_LDU_LEN             1

/*---------------------------------------------------------------------------
  The hdrrlp stream header is 2 bits
---------------------------------------------------------------------------*/
#define HDRRLP_STREAM_HEADER_SIZE 2

#define HDRRLPI_SIG_RST       0x00
#define HDRRLPI_SIG_RSTACK    0x01
#define HDRRLPI_SIG_NAK       0x02

/*---------------------------------------------------------------------------
  Modulus to use for all sequence number calculations.  The value is equal
  to 2^s, where s = 22 bits for HDR.  The value is upshifted to support
  internal sequence number representation of the RLP framework.
---------------------------------------------------------------------------*/
#define  HDRRLPSEQ_32BIT_SEQ_MODULUS        (0x00400000)

/*-------------------------------------------------------------------------
  Maximum number of PacketInfo entries in the free pool
---------------------------------------------------------------------------*/
#define HDRRLP_MAX_PKT_INFO_ENTRIES     DSRLP_MAX_PKTS_IN_SLOT

#define HDRRLP_HEADER_SIZE    3
/*---------------------------------------------------------------------------
  Bits in a byte.  Used in figuring out first set of padding in a control
  frame.
---------------------------------------------------------------------------*/
#define   HDRMRLP_BITS_IN_OCTET        (sizeof(byte) * 8)

/*---------------------------------------------------------------------------
  Size of the RLP header in bytes. this includes the Stream Header, the
  RLP ID and the Sequence number
---------------------------------------------------------------------------*/
#define HDRMRLP_HEADER_SIZE( cfg_ptr )                                     \
          ((HDRMRLP_STREAM_HEADER_SIZE +                                   \
            cfg_ptr->seq_length +                                          \
            cfg_ptr->rlp_id_len_in_pkt) / (HDRMRLP_BITS_IN_OCTET))
#define HDRERLP_HEADER_SIZE( cfg_ptr )                                     \
          ((HDRMRLP_STREAM_HEADER_SIZE +                                   \
            cfg_ptr->seq_length + HDRRLP_EMPA_LINK_FLOW_ID_LEN +           \
            HDRRLP_EMPA_ROUTE_LEN + HDRRLP_EMPA_FDU_LEN +                  \
            HDRRLP_EMPA_LDU_LEN) / (HDRMRLP_BITS_IN_OCTET))
#define HDRMMRLP_HEADER_SIZE( cfg_ptr )                                    \
          ((HDRMRLP_STREAM_HEADER_SIZE +                                   \
            cfg_ptr->seq_length + HDRRLP_MMPA_LINK_FLOW_ID_LEN +           \
            HDRRLP_MMPA_ROUTE_LEN + HDRRLP_MMPA_FDU_LEN +                  \
            HDRRLP_MMPA_LDU_LEN + HDRRLP_MMPA_QNSEQ_INCLUDED_LEN)          \
            / (HDRMRLP_BITS_IN_OCTET))


/*---------------------------------------------------------------------------
  Macro to determine the reset state of RLP.  RLP is either in the RESET
  state, or it is about to be reset in the HDRTX task, by either an AN
  initiated RLP reset or an AT initiated reset.  In either case, this
  decision is made in the HDRRX task context, however, the actual RLP
  reset procedures are performed at the next build frame interrupt in the
  HDRTX task, where the RLP reset signaling response messages are generated
  and transmitted.  Once RLP is in this condition of either RESET or about
  to be reset in HDRTX, it should dump all subsequent received data queued
  from the HDRRX receive buffers, since the RLP is in a reset state.
---------------------------------------------------------------------------*/
#define HDRRLP_IN_RESET( rrcb_ptr )                                         \
                   (                                                        \
                     (rrcb_ptr->state.state != DSRLPI_ESTABLISHED_STATE) || \
                     (rrcb_ptr->state.rst_to_send == TRUE)               || \
                     (rrcb_ptr->state.rst_ack_to_send == TRUE)              \
                   )


/*---------------------------------------------------------------------------
  Structure to enable or disable extreme verbose debug f3 messages
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 enable_rx_debug;    /* Enable extensive Rx Debugging */
  uint8 enable_tx_debug;    /* Enable extensive Tx Debugging */

}hdrrlp_debug_ctl;

#if defined(FEATURE_HDRRLP_LOGGING)
#define LOG_MAX_HDRRLP_RX_COUNT 30
#define LOG_MAX_HDRRLP_TX_COUNT 30
#define LOG_MAX_HDRRLP_RX_HEADER_SIZE 6   /* in bytes */
#define LOG_MAX_HDRRLP_TX_HEADER_SIZE 6   /* in bytes */
#define HDRRLP_SRID_DEFAULT 1
#define HDRRLP_STREAM_ID_SHIFT_BITS 0x06
#define HDRRLP_SR_ID_MASK           0x03
#define HDRRLP_STREAM_ID_MASK       0xC0

#define HDRRLP_LOG_RLP_RX_FRM_BUF_THRESHOLD \
  (sizeof(DS_LOG_RLP_RX_FRAMES_C_type) - \
    (FPOS(log_hdrrlp_pkt_type, hdrrlp_payload) + LOG_MAX_HDRRLP_RX_HEADER_SIZE ))

#define HDRRLP_LOG_RLP_TX_FRM_BUF_THRESHOLD \
  (sizeof(DS_LOG_RLP_TX_FRAMES_C_type) - \
    (FPOS(log_hdrrlp_pkt_type, hdrrlp_payload) + LOG_MAX_HDRRLP_TX_HEADER_SIZE ))

#endif /*  (FEATURE_HDRRLP_LOGGING) */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#if defined(FEATURE_HDRRLP_LOGGING)
/*===========================================================================
FUNCTION      GET_HDRRLP_FR_RX_LOG_PTR

DESCRIPTION   Accessor function for hdrrlp_fr_rx_log

DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_fr_rx_log

SIDE EFFECTS  None
===========================================================================*/
log_hdrrlp_frames_rx_type* get_hdrrlp_fr_rx_log_ptr
(
  void
);

/*===========================================================================
FUNCTION      GET_HDRRLP_FR_TX_LOG_PTR

DESCRIPTION   Accessor function for hdrrlp_fr_tx_log

DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_fr_tx_log

SIDE EFFECTS  None
===========================================================================*/
log_hdrrlp_frames_tx_type* get_hdrrlp_fr_tx_log_ptr
(
  void
);
#endif /*  (FEATURE_HDRRLP_LOGGING) */

#ifdef FEATURE_DATA_RL_FLOW_CONTROL
/*===========================================================================
FUNCTION      GET_HDRRLP_RLFC_DELAY_TIMER_PTR

DESCRIPTION   Accessor function for hdrrlp_rlfc_delay_timer

DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_rlfc_delay_timer

SIDE EFFECTS  None
===========================================================================*/
rex_timer_type* get_hdrrlp_rlfc_delay_timer_ptr
(
  void
);
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */

/*===========================================================================
FUNCTION      GET_HDRRLP_CONSTANTS_PTR

DESCRIPTION   Accessor function for hdrrlp_constants

DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_constants

SIDE EFFECTS  None
===========================================================================*/
dsrlpi_ver_const_type* get_hdrrlp_constants_ptr
(
  void
);

/*===========================================================================
FUNCTION      GET_HDRRLP_DEBUG_PTR

DESCRIPTION   Accessor function for hdrrlp_debug

DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_debug

SIDE EFFECTS  None
===========================================================================*/
hdrrlp_debug_ctl* get_hdrrlp_debug_ptr
(
  void
);

/*===========================================================================
FUNCTION      GET_HDRRLP_DEFAULT_CTA_VAL_PTR

DESCRIPTION   Accessor function for hdrrlp_default_cta_val

DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_default_cta_val

SIDE EFFECTS  None
===========================================================================*/
dsat_num_item_type* get_hdrrlp_default_cta_val_ptr
(
  void
);

/*===========================================================================
FUNCTION      GET_HDRRLP_ENHANCED_DORMANT_BITMAP_PTR

DESCRIPTION   Accessor function for hdrrlp_enhanced_dormant_bitmap

DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_enhanced_dormant_bitmap

SIDE EFFECTS  None
===========================================================================*/
qword* get_hdrrlp_enhanced_dormant_bitmap_ptr
(
  void
);

/*===========================================================================
FUNCTION HDRRLP_GET_HEADER_SIZE

DESCRIPTION
  Computes RLP header size based on RLP version

PARAMETERS
  cfg_ptr:   RLP Config pointer

DEPENDENCIES
  None.

RETURN VALUE
  The computed header size

SIDE EFFECTS
  None.
===========================================================================*/
int hdrrlp_get_header_size
(
  const dsrlpi_cfg_info_type* cfg_ptr
);

/*===========================================================================
FUNCTION HDRRLP_LOG_STATISTICS

DESCRIPTION
  Logs the current values of the HDR RLP statistics to the off-target
  diagnostic tool. If we can't allocate space for the log packet on the DIAG
  ring buffer, we'll abort and try again next time.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlp_log_statistics
(
  uint8 rscb_index
);

/*===========================================================================
FUNCTION HDRRLP_RESET_LOCAL_STATE

DESCRIPTION
  Resets the local state variables for a given combo_index

PARAMETERS
  state_ptr:  State pointer for a particular rlp
  combo_index: Index to the combo that needs to be cleared up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrlp_reset_new_data_local_state
(
 dsrlpi_state_info_type  *state_ptr,
 uint8 combo_index
);

/*===========================================================================

FUNCTION HDRRLP_MSG_CB

DESCRIPTION
    This callback function invoked by SLP whenever a control message is
    received for RLP.  This function queues up a received RLP signaling
    message to the RLP receive signaling message queue.

DEPENDENCIES
  The receive signaling message queue must have been initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_msg_cb
(
  dsm_item_type *msg_ptr,               /* ptr to received RLP message     */
  uint8 sr_id                  /* RLP instance for which this msg is meant */
);

/*===========================================================================

FUNCTION HDRRLP_STARTUP

DESCRIPTION
  This function is to be called only once per power-up of the phone. The
  function initalizes the timer for logging RLP statistics.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_startup
(
  void
);

/*===========================================================================

FUNCTION HDRRLP_RESET

DESCRIPTION
  Does the HDR RLP specific reset. Note that it should only be called from
  dsrlpi_reset, since the rest of the reset is done in dsrlpi_reset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrrlp_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION HDRRLP_CLEANUP

DESCRIPTION
  This function is to be called when a data call is closed. The
  function stops the timer for logging RLP statistics.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrrlp_cleanup
(
  uint8 sr_id
);

/*===========================================================================

FUNCTION HDRRLP_INIT

DESCRIPTION
  This function registers RLP callbacks with PCP and starts the statistics
  timers associated with the particular RLP

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_init
(
  uint8 rlp_id
);

/*===========================================================================

FUNCTION HDRRLP_GET_PENDING_TX_DATA_QUEUE_SIZE

DESCRIPTION
  This function is called to determine the number of bytes pending transmission
  by this RLP.

  The size returned is the total number of bytes in the retransmission queue,
  plus the new data pending transmission. The new data pending transmission
  includes data in the tx watermark as well as data that has been  dequeued
  from the watermark, but not yet transmitted.

  This function is also used as a callback by hdrpac (transmit)

DEPENDENCIES
  None.

RETURN VALUE
  Number of bytes pending transmission for this RLP.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint32 hdrrlp_get_pending_tx_data_queue_size
(
  uint32 *num_pkts,            /* Number of separate RLP packets           */
  void *pkt_rrcb_ptr           /* RLP-specific ctl block                   */
);


/*===========================================================================

FUNCTION HDRRLP_GET_RETX_DATA

DESCRIPTION
  This function is called by the PCP layer in order to get Re-transmit data.
  The size of the RLP packet is dependent on the  maximum number of bytes
  that is passed in as a parameter, and determined by the reverse traffic
  channel MAC.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet to send, including headers.  A zero-length value
  indicates that the PCP layer should discontinue to query RLP for more data.

SIDE EFFECTS
  The *rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
extern uint16 hdrrlp_get_retx_data
(
  uint8 combo_id,
  uint8 carrier_id,
  uint16 max_bytes,            /* max number of bytes allowed to send      */
  dsm_item_type **rev_pkt_ptr, /* ptr to data packet built for TX          */
  void **tx_data_handle,        /* Handle for the data                      */
  void *pkt_rrcb_ptr           /* RLP-specific ctl block                   */
);

/*===========================================================================

FUNCTION HDRRLP_GET_NEW_OR_FLUSH_DATA

DESCRIPTION
  This function is called by the PCP layer. It builds any  new data to
  be sent.  If there is no data to send, and the flush timer has expired, a
  packet containing the last octet of the last packet that was sent will be
  built. The size of the RLP packet is dependent on the  maximum number of
  bytes that is passed in as a parameter, and determined by the reverse
  traffic channel MAC.

  Note that the function only checks for new data. It does not check if there
  is any retransmit data pending to be sent (except for the flush packets if
  there is no new data.)

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet to send, including headers.  A zero-length value
  indicates that the PCP layer should discontinue to query RLP for more data.

SIDE EFFECTS
  The *rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
extern uint16 hdrrlp_get_new_or_flush_data
(
  uint8 combo_id,
  uint8 carrier_id,
  uint16 max_bytes,            /* max number of bytes allowed to send      */
  dsm_item_type **rev_pkt_ptr, /* ptr to data packet built for TX          */
  void **tx_data_handle,        /* Handle for the data                      */
  void *pkt_rrcb_ptr           /* RLP-specific ctl block                   */
);

/*===========================================================================

FUNCTION HDRRLP_TX_STATUS_CB

DESCRIPTION
  This function is called by the PCP layer to inform RLP about the status of
  transmission of a packet.

  Currently since this code is expected to run over Rev 0 RTCMAC, this function
  is only called with status = TRUE, implying data was transmitted. The code
  ASSERTs this.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void hdrrlp_tx_status_cb
(
  uint8 combo,
  hdrcp_pcp_carrier_tx_outcome_type tx_data_handle[],
  uint8 num_sets,
  void *pkt_rrcb_ptr
);



/*===========================================================================

FUNCTION HDRRLP_TX_FAILED_CB

DESCRIPTION
  This function is called by the PCP layer to inform RLP about physical layer
  NAK happened due to MARQ on a given RLP.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrrlp_tx_failed_cb
(
  void *tx_data_handle,
  void *pkt_rrcb_ptr           /* RLP-specific ctl block                   */
);


#if defined(FEATURE_HDRRLP_LOGGING)

/*===========================================================================

FUNCTION HDRRLP_RX_SEND_FRAMES_LOG

DESCRIPTION
  Terminate and send the accumulated HDR RLP partial rx frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
void hdrrlp_rx_send_frames_log
(
  void
);

/*===========================================================================

FUNCTION HDRRLP_RX_FRAME_LOG_INIT

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
log_hdrrlp_pkt_type * hdrrlp_rx_frame_log_init
(
  void
);

/*===========================================================================

FUNCTION HDRRLP_TX_SEND_FRAMES_LOG

DESCRIPTION
  Terminate and send the accumulated RLP partial  tx frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RLP partial tx frames log may be sent.

===========================================================================*/
void hdrrlp_tx_send_frames_log
(
  void
);

/*===========================================================================

FUNCTION HDRRLP_TX_FRAME_LOG_INIT

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
log_hdrrlp_pkt_type * hdrrlp_tx_frame_log_init
(
  void
);

/*===========================================================================

FUNCTION HDRRLP_LOG_RX_RLP_FRAMES()

DESCRIPTION
  This function LOGS THE PARTIAL RX RLP FRAMES
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
extern void hdrrlp_log_rx_rlp_frames
(
  dsm_item_type *rlp_pkt_ptr
);

/*===========================================================================

FUNCTION HDRRLP_LOG_TX_RLP_FRAMES()

DESCRIPTION
  This function LOGS THE PARTIAL RX RLP FRAMES
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
void hdrrlp_log_tx_rlp_frames
(
  dsm_item_type *rlp_pkt_ptr
);

#endif  /* FEATURE_HDRRLP_LOGGING  */

/*===========================================================================

FUNCTION HDRRLP_POPULATE_TX_DATA_Q_LEVEL

DESCRIPTION
  This function is called to determine the number of bytes pending transmission
  by this RLP.

  This function populates the field in the structure
  ps_flow_ioctl_tx_queue_level_type

  The size populated is the total number of bytes in the retransmission queue,
  plus the new data pending transmission. The new data pending transmission
  includes data in the tx watermark as well as data that has been  dequeued
  from the watermark, but not yet transmitted.

DEPENDENCIES
  None.

RETURN VALUE
  The function argument holds the return value

SIDE EFFECTS
  None.
===========================================================================*/
int8 hdrrlp_populate_tx_data_q_level
(
 /* The following 3 fields correspond to struct ps_flow_ioctl_tx_queue_level_type */
 uint32  *total_pending_cnt,
 uint8 rlp_id,
 dsrlp_ver_enum_type ver
);

/*===========================================================================

FUNCTION HDRRLP_PROCESS_PKT_PAYLOAD

DESCRIPTION
  This function processes the payload of a received RLP data
  packet.  The function is called on a per packet basis.  As data is
  processed, the function updates RLP state variables as needed.
  Out-of-sequence data may also cause this function to generate NAK list
  entries used to build up a NAK signaling message on subsequent TX
  processing.

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
extern void hdrrlp_process_rlp_pkt
(
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr,       /* RLP-specific ctl block   */
  dsm_item_type *fwd_pkt_ptr,      /* ptr to received RLP data packet      */
  dsrlpi_header_type          *rlp_header_ptr /* RLP Header info          */
);

#if defined(FEATURE_HDRRLP_LOGGING)
/*===========================================================================
FUNCTION      HDRRLP_TX_RLP_FRAMES_LOGGING

DESCRIPTION   Logs the tx frame based on RLP version

PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  rev_pkt:    data packet built for TX

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
extern void hdrrlp_tx_rlp_frames_logging
(
  hdrrlp_tx_packet_info_type* tx_packet_info_ptr
);
#endif /* FEATURE_HDRRLP_LOGGING */

/*===========================================================================
FUNCTION      HDRRLP_CTA_SET_ENHANCED_IDLE_BIT

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_cta_set_enhanced_idle_bit
(
  uint8 rlp_id,
  dsrlp_ver_enum_type ver

);

/*===========================================================================
FUNCTION      HDRRLP_CTA_CLEAR_ENHANCED_IDLE_BIT

DESCRIPTION   This function clears enhanced bitmap

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_cta_clear_enhanced_idle_bit
(
  uint8 rlp_id,
  dsrlp_ver_enum_type ver,
  boolean enable_inactivity_timer
);

/*===========================================================================
FUNCTION      HDRRLP_DATA_ACTIVITY_UPDATE

DESCRIPTION   This function is called from the rlp layer, every TX/RX interrupt If the
              input parameter is FALSE, then no data frames have been received in the
			  past TX/RX inerrupt.  Otherwise at least one data frame was received or
			  trasmitted.  If data has been data activity, then the idle timer should
			  be reset, so that it doesn't expire.

              This function should be called from the RX/TX task.

DEPENDENCIES

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_data_activity_update
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr
);

/*===========================================================================
FUNCTION      HDRRLP_PHY_LINK_DOWN_INACTIVITY_TIMER_HDLR

DESCRIPTION   This function is called from DS to handle Physical Link Down event for RLP
              Inactivity timer.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_phy_link_down_inactivity_timer_hdlr(void);

/*===========================================================================
FUNCTION      HDRRLP_PHY_LINK_UP_INACTIVITY_TIMER_HDLR

DESCRIPTION   This function is called from DS to handle Physical Link Up event for RLP
              Inactivity timer.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_phy_link_up_inactivity_timer_hdlr(void);

/*===========================================================================
FUNCTION      HDRRLP_RESV_BOUND_TO_RLP_INACTIVITY_TIMER_HDLR

DESCRIPTION   This function is called from DS, to handle RLP inactivity timer ,
              when reservation bound to RLP
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_resv_bound_to_rlp_inactivity_timer_hdlr
(
  uint8  rscb_index,
  uint32 ds_flow_inactivity_val
);

/*===========================================================================
FUNCTION      HDRRLP_RESV_UNBOUND_FROM_RLP_INACTIVITY_TIMER_HDLR

DESCRIPTION   This function is called from DS, to handle RLP inactivity timer ,
              when reservation bound to RLP
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_resv_unbound_from_rlp_inactivity_timer_hdlr
(
  uint8  rscb_index,
  uint32 ds_flow_max_inactivity_timer_val
);
/*===========================================================================
FUNCTION      HDRRLP_COPY_DEFAULT_CTA_VAL

DESCRIPTION   This function is called from DS to propagate the default CTA
              timer value to RLP
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_copy_default_cta_val
(
  dsat_num_item_type cta_val
);

/*===========================================================================
FUNCTION HDRRLP_GET_RLP_ID_FOR_DEF_APP_SUBTYPE

DESCRIPTION
  This function returns the RLP ID of the RLP that is configured for a
  particular Default Packet Application Subtype (AN or SN)

  It cannot be used for multiflow packet application since the a multiflow
  packet application can have multiple RLPs mapped to it.

DEPENDENCIES
  None. Note that unlike other mapping functions, this function is
  intentionally not dependant on the HDR session being opened. This is
  because the RLP ID for the SN and AN stream are fixed and will not
  change irrespective of if the session is opened or closed.

  Note that this enables the RLP to be initialized even before the
  HDR session is opened.

PARAMETERS
   app_subtype - Application Subtype who RLPID is to be determined//

RETURN VALUE
  rlp_id for the RLP that is mapped to the inputted app subtype.
  The rlp_id is set to HDRRLP_INVALID_ID if no stream is mapped for the
  app_subtype.

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdrrlp_get_rlp_id_for_def_app_subtype
(
  hdrcom_hai_app_subtype_enum_type app_subtype
);

#endif  /* FEATURE_HDR */
#endif  /* _HDRRLP_H */
