#ifndef HDRSLPTX_H
#define HDRSLPTX_H
/*===========================================================================

            H D R   S I G N A L I N G   L A Y E R   P R O T O C O L
          T R A N S M I T T I N G   C H A I N   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Signaling Layer Protocol (SLP)
  transmitting chain.

  Note: The magic numbers 4 and 16 :-) for the retransmission buffer
        are intentionally _not_ #define's. The buffer size and the
        lookup array are coupled, and values other than 4 and 16 will
        require additional code changes. They are _not_ to be changed
        on their own.

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/hdrslptx.h_v   1.12   01 May 2002 00:40:48   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrslptx.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/13   ukl     Added critical section init for slptx crit sections.
09/24/11   cnx     Fixed compiler error.
05/07/09   kss     Lint fix.
03/17/09   sju     Changed tx queue size to 20 to support multicarrier
04/17/08   sju     Added support to bundle AC messages 
12/06/06   sju     When RMAC3 is used, flush slp cb for be msg every 4 slots 
                   instead of 16 slots. 
12/06/05   etv     Added function to Flush all pending SLP Acks.
06/22/05   pba     No fragmentation of DOS message on access channel.
11/29/04   dna     Avoid getting stuck in connection setup state.
                   hdrslp_update_tx_timers needs to take a uint16, otherwise
                   an input with lowest 8 bits of zero won't increment timers!
05/01/02   om      Removed obsolete variable
01/14/01   dna     Changed prototype for hdrslp_tx_queue_clear()
09/25/01   om      Removed FEATURE_HDR_HAI_X4
08/14/01   om      Changed type of hdrslp_cb_enqueue()
08/10/01   om      Changed type of hdrslp_mp_init_msg()
05/01/01   dna     Minor change to compile IS-856 related changes
02/22/00   om      Created Module

===========================================================================*/

#include "hdr_variation.h"
#ifndef COMDEF_H 
  #error Please include <comdef.h> first 
#endif
#ifndef DSM_H
  #error Please include <dsm.h> first
#endif
#ifndef HDRSLP_H
  #error Please include <hdrslp.h> first
#endif

#include "hdrslpi.h"
#include "hdrerrno.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*** SLP incoming transmit queue (from upper layers)              ***/
#define HDRSLP_TX_QUEUE_SIZE  20

/*** SLP msg buffer pool size =                                   ***/
/***    (tx queue + rexmit queue) *                               ***/
/***    (callback waiting queue + callback processing queue)      ***/
#define HDRSLP_MSG_POOL_SIZE  2 * (HDRSLP_TX_QUEUE_SIZE + 4)

/*** SLP msg buffer queues ***/
typedef enum
{
  SLP_FREE_Q = 0,       /* Unused items                                   */
  SLP_XMIT_Q,           /* Items in the transmit queue                    */
  SLP_REXMIT_Q,         /* Items in the retransmit buffer                 */
  SLP_FRG_Q,            /* Item in the fragmentation buffer               */
  SLP_CB_WAIT_Q,        /* Items waiting for a timeout on their callback  */
  SLP_CB_PROCESS_Q,     /* Items ready to get their callback processed    */
  SLP_NIRVANA_Q         /* Items transitioning between queues             */
} hdrslp_msg_queue_enum_type;

/*** SLP msg buffer items ***/
typedef struct
{
  hdrslp_sdu_struct_type       sdu;               /* size = 6 bytes */
  uint8                        priority;
  boolean                      reliable;
  uint16                       rexmit_time;
  uint16                       rexmit_count;
  hdrslp_outcome_cb_type       outcome_func_ptr;
  void*                        outcome_tag_ptr;
  uint16                       cb_time;
  hdrerrno_enum_type           tx_status;
  hdrslp_msg_queue_enum_type   queue;
  void*                        next;
  uint8                        frg_seqno;
} hdrslp_msg_struct_type;

/*** SLP msg pool ***/
extern hdrslp_msg_struct_type      hdrslp_msg_pool[HDRSLP_MSG_POOL_SIZE];

/*** SLP transmit queue ***/
extern hdrslp_msg_struct_type*     hdrslp_tx_queue_item[HDRSLP_TX_QUEUE_SIZE];
extern hdrslp_msg_struct_type*     hdrslp_tx_queue_first_item_ptr;
extern uint16                      hdrslp_tx_queue_item_count;

/*** SLP reliable transport sequence number ***/
extern uint8                       hdrslp_tx_seqno;

/*** SLP rexmit buffer and managing data (up to 4 rexmits) ***/
extern hdrslp_msg_struct_type*     hdrslp_rexmit_buffer[4];
extern uint8                       hdrslp_rxb_level;
extern uint8                       hdrslp_rxb_free_mask;
extern uint8                       hdrslp_rxb_timer_mask;
extern uint8                       hdrslp_rxb_lookup[16];

/*** SLP fragmentation buffer */
extern hdrslp_msg_struct_type*     hdrslp_fragment_buffer;
extern uint16                      hdrslp_fragment_header;
extern uint16                      hdrslp_fragment_pointer;
extern uint16                      hdrslp_fragment_tx_count;
extern uint8                       hdrslp_fragment_seqno;

/*** SLP RX Ack buffer and timers (up to 4 Acks to be sent) ***/
#define HDRSLP_RX_ACK_BUFFER_USED_MASK    0x80
#define HDRSLP_RX_ACK_BUFFER_TIMER_MASK   0x38
#define HDRSLP_RX_ACK_BUFFER_SEQNO_MASK   0x07
#define HDRSLP_GET_RX_ACK_TIMER( index ) \
        ((hdrslp_rx_ack_buffer[i] & HDRSLP_RX_ACK_BUFFER_TIMER_MASK) >> 3)
#define HDRSLP_DECR_RX_ACK_TIMER( index, num_ticks ) \
        hdrslp_rx_ack_buffer[i] -= (num_ticks << 3)

extern uint8                       hdrslp_rx_ack_buffer[4];
extern uint8                       hdrslp_rx_ack_timer_mask;
extern uint8                       hdrslp_rx_ack_buffer_write_index;
extern uint8                       hdrslp_rx_ack_buffer_read_index;
extern uint8                       hdrslp_rx_ack_current_seqno;
extern hdrslp_msg_struct_type      hdrslp_rx_ack_only_msg;

/*** SLP Callback Function list ***/
extern hdrslp_msg_struct_type*     hdrslp_cb_wait_q_ptr;
extern hdrslp_msg_struct_type*     hdrslp_cb_process_q_ptr;


/* <EJECT> */ 
/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
-----------     Transmission and Acknowledgement Functions       -----------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_UPDATE_TIMERS

DESCRIPTION
  This function updates the frame-count based timers for transmission
  with the number of 26.66... ms interrupts since the last build signal.

DEPENDENCIES
  Simon's Mac to keep track of the number of 26.66... interrupts between
  build signals.
  
PARAMETERS
  num_tx_ints - Number of TX interrupts since we were called the last time

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_update_tx_timers
(
  uint16 num_tx_ints 
);

/*===========================================================================

FUNCTION HDRSLP_BE_MSG_TX_COMPLETE

DESCRIPTION
  This function moves all best effort message with cb_timer == 1 to 
  SLP_CB_PROCESS_Q. 

DEPENDENCIES
  None
   
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrslp_be_msg_tx_complete(void);


/*===========================================================================

FUNCTION HDRSLP_FLUSH_PENDING_ACKS

DESCRIPTION
  This function is used flush all the pending SLP acks for the reliable 
  messages that AN sent before closing the connection.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_flush_pending_acks(void);


/*===========================================================================

FUNCTION HDRSLP_BUILD_TX_DATA

DESCRIPTION
  This function creates a signaling layer packet given the maximum
  size of the next transmitted frame.

DEPENDENCIES
  None
  
PARAMETERS
  tx_sdu_ptr - Pointer to the msg and meta data needed by PCP
  max_bytes  - maximum frame size the signaling layer packet has to fit in

RETURN VALUE
  Length of signaling layer packet in bytes
  
SIDE EFFECTS
  None
===========================================================================*/

uint8 hdrslp_build_tx_data
(
  boolean                  first_callfor_slpdata,
  hdrslp_sdu_struct_type*  tx_sdu_ptr, 
  uint8                    max_bytes 
);

/*===========================================================================

FUNCTION HDRSLP_CREATE_HEADER

DESCRIPTION
  This function takes a new msg from the TX queue and prepends
  the common header parts in it's own dsm item.

DEPENDENCIES
  None
  
PARAMETERS
  tx_msg_ptr - Pointer to the new SLP msg from TX queue

RETURN VALUE
  Length of header in bytes
  
SIDE EFFECTS
  The dsm item chain of the passed queue item gets the common part
  of the SLP header prepended.
===========================================================================*/

uint8 hdrslp_create_header
(
  hdrslp_msg_struct_type* tx_msg_ptr
);

/*===========================================================================

FUNCTION HDRSLP_FRAGMENT_SDU

DESCRIPTION
  This function takes an SDU and does fragmentation if necessary.
  the hdrslp_xmit_fragment_buffer is used to keep a fragmented SDU
  until all fragments are transmitted.

DEPENDENCIES
  None
  
PARAMETERS
  frg_msg_ptr - Pointer to the fragmented SDU
  tx_msg_ptr  - Pointer to the SLP msg buffer containing the original SDU
  max_bytes   - maximum frame size the signaling layer packet has to fit in

RETURN VALUE
  Length of signaling layer packet in bytes
  
SIDE EFFECTS
  None
===========================================================================*/

uint8 hdrslp_fragment_sdu
(
  hdrslp_sdu_struct_type*  frg_msg_ptr,
  hdrslp_msg_struct_type*  tx_msg_ptr,
  uint8                    max_bytes
);

/*===========================================================================

FUNCTION HDRSLP_VERIFY_DEST_CHANNEL

DESCRIPTION
  This function checks that the next pending sign. msg is for the
  current destination channel. Non-matching messages are removed from
  the TX queue until the queue is empty or a message for the current
  destination channel is found.

DEPENDENCIES
  None
  
PARAMETERS
  current_channel - Current destination channel

RETURN VALUE
  None
  
SIDE EFFECTS
  SLP's TX queue is altered (see above).
===========================================================================*/

void hdrslp_verify_dest_channel
( 
  hdrhai_channel_enum_type current_channel 
);

/*===========================================================================

FUNCTION HDRSLP_PROCESS_TX_ACK

DESCRIPTION
  This function takes a received sequence number and updates the
  retransmission buffer accordingly.
  NOTE: This function is run in the context of the Rx Task and hence
  needs to lock interrupts while accessing the retransmission buffer.

DEPENDENCIES
  None
  
PARAMETERS
  seqno - Received sequence number

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_tx_ack
(
  uint8 seqno
);

/*===========================================================================

FUNCTION HDRSLP_FIND_MSG

DESCRIPTION
  This function finds a reliable message in the retransmission
  buffer given its callback funcptr and tag.
  It is used by hdrslp_abort_msg().

DEPENDENCIES
  None
  
PARAMETERS
  slp_outcome_cb  - Function to be called after message has been
                    transmitted successfully or transmission was aborted
  outcome_tag_ptr - User data for callback function

RETURN VALUE
  Pointer to SLP message buffer containing this msg (NULL if not found)
  
SIDE EFFECTS
  None
===========================================================================*/

hdrslp_msg_struct_type* hdrslp_find_msg
(
  hdrslp_outcome_cb_type     slp_outcome_cb,
  void*                      outcome_tag_ptr 
);

/* <EJECT> */ 
/*--------------------------------------------------------------------------
-------------      Transmission Queue Mgmt Functions       -----------------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_TX_QUEUE_INSERT_ITEM

DESCRIPTION
  This function enqueues a tx item from the pool according to
  the given priority and returns the address so the caller can
  fill in the data fields.

DEPENDENCIES
  None
  
PARAMETERS
  priority - signaling message priority ( 0 -> 255, 0 is highest )

RETURN VALUE
  Address of the queued SLP msg buffer or NULL if queue is full
  
SIDE EFFECTS
  None
===========================================================================*/

hdrslp_msg_struct_type* hdrslp_tx_queue_insert_item
(
  uint8 priority 
);

/*===========================================================================

FUNCTION HDRSLP_TX_QUEUE_FREE_FIRST_ITEM

DESCRIPTION
  This function dequeues the first msg buffer from the transmit queue
  and should be called _before_ the msg buffer is enqueued into another
  queue.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  The regional pointer to the first item in the tx q is updated.
===========================================================================*/

void hdrslp_tx_queue_free_first_item( void );

/*===========================================================================

FUNCTION HDRSLP_TX_QUEUE_ADVANCE_BE_ITEM

DESCRIPTION
  This function finds the next best-effort signaling message in the 
  transmit queue and advances it to the first place.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_tx_queue_advance_be_item( void );

/*===========================================================================

FUNCTION HDRSLP_TX_QUEUE_CLEAR

DESCRIPTION
  This function clears out any items in the transmit queue.

DEPENDENCIES
  None
  
PARAMETERS
  errno - HDR error number to use in callbacks

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_tx_queue_clear
(
  hdrerrno_enum_type err_num 
);


/* <EJECT> */ 
/*--------------------------------------------------------------------------
---------------      Callback Queues Mgmt Functions       ------------------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_CB_ENQUEUE

DESCRIPTION
  This function enqueues a transmitted SLP msg buffer into the 
  the callback waiting queue for timeouts > 0 and into the
  callback processing queue for timeout 0.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr   - Pointer to SLP msg buffer
  time      - time until callback needs to be processed
  tx_status - The error code to give when the message callback is made
RETURN VALUE
  Pointer to the next message in the callback queue
  
SIDE EFFECTS
  None
===========================================================================*/

hdrslp_msg_struct_type* hdrslp_cb_enqueue
(
  hdrslp_msg_struct_type* msg_ptr, 
  uint16 time,
  hdrerrno_enum_type tx_status
);

/*===========================================================================

FUNCTION HDRSLP_CB_WAIT_QUEUE_CLEAR

DESCRIPTION
  This function clears out any items in the cb_wait queue.

DEPENDENCIES
  None
  
PARAMETERS
  errno - HDR error number to use in callbacks

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_cb_wait_queue_clear
(
  hdrerrno_enum_type err_num 
);

/*===========================================================================

FUNCTION HDRSLP_CB_PROCESS

DESCRIPTION
  This function loops through the list of pending callbacks and
  calls each of them.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  The callback processing queue is empty after function returned.
===========================================================================*/

void hdrslp_cb_process( void );

/*===========================================================================

FUNCTION HDRSLP_CB_FAILURE

DESCRIPTION
  This function loops through the list of pending callbacks and
  sets their tx status to the given error code.

DEPENDENCIES
  None
  
PARAMETERS
  errno - HDR error number to use in callbacks

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_cb_failure
(
  hdrerrno_enum_type err_num
);

/* <EJECT> */ 
/*--------------------------------------------------------------------------
--------------      SLP Message Pool Mgmt Functions       ------------------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_MP_INIT_POOL

DESCRIPTION
  This function initializes all SLP message buffers.

DEPENDENCIES
  This function has to be called before any SLP queue is used.
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_mp_init_pool( void );

/*===========================================================================

FUNCTION HDRSLP_MP_INIT_MSG

DESCRIPTION
  This function initializes a single item from the SLP msg pool.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr - Address of the item to be initialized

RETURN VALUE
  Pointer to next message in the list.
  
SIDE EFFECTS
  Any data contained in the DSM item chain is freed
===========================================================================*/

hdrslp_msg_struct_type* hdrslp_mp_init_msg( hdrslp_msg_struct_type* msg_ptr );

/*===========================================================================

FUNCTION HDRSLP_MP_GET_MSG

DESCRIPTION
  This function fetches a free SLP msg buffer

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  Pointer to an unused SLP msg buffer or NULL if none are available
  
SIDE EFFECTS
  None
===========================================================================*/

hdrslp_msg_struct_type* hdrslp_mp_get_msg( void );

/*===========================================================================

FUNCTION HDRSLP_MP_FREE_MSG

DESCRIPTION
  This function takes an SLP msg buffer off its queue, frees any existing
  data in the dsm item chain, and initializes the buffer.  If defined,
  the message callback is called to report the status of the message
  transmission.  The tx_status is set as follows:
  
  For each error code, there is a list of events that cause messages to
  be freed, and a code for which queues the messages are freed from
  X - SLP_XMIT_Q
  R - SLP_REXMIT_Q
  F - SLP_FRG_Q
  W - SLP_CB_WAIT_Q
  P - SLP_CB_PROCESS_Q
  
                                                                         
  E_IN_PROGRESS - 
    Each message buffer is initialized to this value until changed 
    by some later event.  By the time a message is being freed, no 
    message should still have E_IN_PROGRESS as its status.
    
  E_SUCCESS - 
    a) NULL AC message (for sending RUP) [P]
    b) best effort message is sent un-fragmented [P]
    c) last fragment of best effort message is sent [P]
    d) ACK is received for a reliable message [P]
    e) If an ACK is received to a reliable message on its last 
       retransmission, (it is already on the SLP_CB_WAIT_Q with E_FAILURE) [P]

  E_ABORTED - 
    The message was aborted due to Access Handoff or Access Entry Handoff
    [XFP]
  
  E_AGAIN - 
    a) Currently any message freed through normal callback processing whose
       status is anything other than E_SUCCESS or E_FAILURE will be changed 
       to E_AGAIN.  This needs to change.
    b) dsm_dup_packet() fails.  this error can occur when fragmenting a message
       or when queueing a reliable message for re-transmit [any]
  
  E_CANCELED - 
    a) SMP entered the OPEN state (initial UATI assigned) [RFWP]
    b) A valid TCA was received when idle [RFWP]
    c) ALMP made any of the following state transitions [XRFWP]
       connected -> idle, init or inactive  
       idle -> init or inactive
       init -> init or inactive
    d) a session was closed [XRFWP]
    e) An SLPReset message was received [RFWP]
    f) ACMAC was deactivated during an access attempt (e.g. a TCA was received)
       [P]
       
  E_FAILURE - 
    a) tx_msg_ptr->sdu.msg_ptr is NULL for a reliable message [P]
    b) SLP fails to pushdown the header into the DSM item [P]
    c) tx_msg_ptr->sdu.msg_ptr is NULL for a best effort message being sent on 
       the RTC [P]
    d) message is null when hdrslp_fragment_sdu() is called [any]
    e) max access probes [P]
    f) reliable message is on its last retry (changed later if ACK is received)
       [P]
    
  E_NOT_ALLOWED - 
    a) the message is for the wrong channel [P] 
         
DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr   - buffer to be freed
  tx_status - the error code to give when the message callback

RETURN VALUE
  Pointer to next msg buffer in this msg buffer's queue.
  
SIDE EFFECTS
  None
===========================================================================*/

hdrslp_msg_struct_type* hdrslp_mp_free_msg
( 
  hdrslp_msg_struct_type* msg_ptr,
  hdrerrno_enum_type tx_status
);

/*===========================================================================

FUNCTION HDRSLP_TX_SET_WAITING_FOR_BUNDLING_FLAG

DESCRIPTION
  This is wrapped by hdrslp_set_waiting_for_bundling_flag, see that  
  function header for details.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern void hdrslp_tx_set_waiting_for_bundling_flag
( 
  boolean waiting_for_bundling
);

/*===========================================================================

FUNCTION HDRSLP_TX_IS_WAITING_FOR_BUNDLING

DESCRIPTION
  This is wrapped by hdrslp_is_waiting_for_bundling, see that  
  function header for details.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern boolean hdrslp_tx_is_waiting_for_bundling(void);

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
/*===========================================================================

FUNCTION HDRSLP_TX_STOP_WAITING_FOR_UATI_ASSIGNMENT

DESCRIPTION
  This is wrapped by hdrslp_stop_waiting_for_uati_assignment, see that  
  function header for details.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern void hdrslp_tx_stop_waiting_for_uati_assignment(void);

/*===========================================================================

FUNCTION HDRSLP_TX_IS_WAITING_FOR_UATI_ASSIGNMENT

DESCRIPTION
  This is wrapped by hdrslp_tx_is_waiting_for_uati_assignment, see that  
  function header for details.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern boolean hdrslp_tx_is_waiting_for_uati_assignment(void);

#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

/*===========================================================================

FUNCTION HDRSLP_TX_INIT_CRIT_SECT

DESCRIPTION
  This function initialized the critical sections used in hdrslp_tx
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void hdrslptx_init_crit_sect(void);


#endif /* HDRSLPTX_H */
