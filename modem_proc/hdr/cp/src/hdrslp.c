/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            H D R   S I G N A L I N G   L A Y E R   P R O T O C O L

GENERAL DESCRIPTION
  The Signaling Layer Protocol (SLP) is used as transport layer protocol 
  for HDR signaling messages. It provides reliable and best-effort
  transport as well as fragmentation.
  When queuing a message for transmission, the user can supply a callback
  function to be notified about the outcome of the transmission.

EXTERNALIZED FUNCTIONS

  hdrslp_init()       - One time initialization of all SLP data structures
  hdrslp_msg_cb()     - Callback registered with HMP to deliver messages
  hdrslp_ind_cb()     - Callback registered for delivery of indications
  hdrslp_send_msg()   - Used to enqueue msgs for transmission
  hdrslp_abort_msg()  - Used to abort rexmits of reliable msgs
  hdrslp_get_common_ac_pkt() - Used by lower layers to get AC spec. sign.
  hdrslp_get_pkt()    - Used by lower layers to get sign. data
  hdrslp_put_pkt()    - Used by lower layers to deliver recvd. sign. data

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrslp_init() has to be called before any messages are enqueued for
  transmission.

Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrslp.c_v   1.26   18 Apr 2003 11:27:42   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrslp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/16/16   vko     Reduce excessive F3
06/03/15   wsh     Added NULL check before ptr dereferencing 
08/09/13   cnx     Fix race condition in hdrslp_abort_msg().
08/06/13   cnx     Fix race condition in hdrslp_abort_msg().
07/15/13   vke     klockwork fix for null ptr check for 
                   hdrslp_route_update_gen_func
06/13/13   ukl     Added critical section init for slptx crit sections.
04/19/13   cnx     Fix a bug in abort SLP callback.
01/24/13   wsh     Added crit_sect to guard SLP data structures
09/11/12   cnx     Handle TX and RX messages during active handoff
03/20/12   arm     Resolve TCB errors for Dime. 
03/19/12   cnx     Fixed compiler errors.
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
10/28/11   pba     Optimized handoff phase 2 support
09/24/11   cnx     Changed subtype checking for Optimized Handoff.
07/27/11   cnx     Merged Optimized Handoff changes.
06/04/09   wsh     Removed unneeded rx.h and tx.h from HDR files
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
12/11/08   sju     Fixed memory leak in DSM buffer when generating large RUP.
12/10/08   smd     Replaced assert.h with amssassert.h
11/22/08   kss     Removed/replaced intlocks.
11/21/08   pba     Added events for SLPQH feature.
09/25/08   pba     Fixed incorrect use of prot_id in hdrslp_send_msg().
09/08/08   sju     Fixed issue where update_mc_rup_msg() was called unnecessarily.   
04/17/08   sju     Added support to bundle AC messages 
04/03/08   wsh     Merged fix for waiting UATIAssign onto 2H07
11/08/07   sju     Added support to allow AHO before the first access attempt
                   when there is no DO session
10/08/07   sju     Added comments 
09/07/07   sju     Added support for revB RU msg
03/05/07   sju     Added support to handle the same message ID with different 
                   subtype. 
12/06/06   sju     When RMAC3 is used, flush slp cb for be msg every 4 slots 
                   instead of 16 slots. 
11/14/06   wsh     Disallow message on access probe when connection attempt is in
                   progress. This should not affect bundling with ConReq.
11/09/06   mpa     Use the monitored link instead of ref link for slp logging.
06/07/06   dna     Include any pending message fragment in the RMAC3 size 
                   estimate.  Otherwise the fragment may never be sent.
12/11/05   hal     Updated hdrslp_get_tx_queue_size() to estimate pending tx
                   data with greater accuracy
07/11/05   dna     Add time_since_last_req to hdrslp_get_tx_queue_size
07/11/05   pba     Restrict route update message size in case it is too large
                   and does not allow sending other messages.
07/09/05   hal     Added hdrslp_get_tx_queue_size().
06/22/05   pba     No fragmentation of DOS message on access channel.
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
11/29/04   dna     Avoid getting stuck in connection setup state.
                   hdrslp_update_tx_timers needs to take a uint16, otherwise
                   an input with lowest 8 bits of zero won't increment timers!
11/29/04   grl     Added connected state stat logging support.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
06/18/03   vas     Added handling of NULL message for AC in hdrslp_send_msg
                   NULL msg pointer indicates RUP msg
04/07/03   mpa     hdrslp_msg_cb should queue msg for tx if not in tx task
03/04/03   dna/mpa Fixed TX RTC F3 messages
07/23/02   om      Fixed handling of Reset message.
04/22/02   om      Changed MSG macro in send_msg()
04/11/02   dna     Log fragmented messages correctly.
04/04/02   om      Added parameter checking to send_msg()
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/14/01   om      Added debug output of prot. & msg ID
08/01/01   dna     Added logging of ack sequence number valid bit of SLP 
                   header as part of the signaling message log packet
06/21/01   dna     hdrslp_get_common_ac_pkt must still update timers even if 
                   there is nothing to send, so callbacks get called.
01/16/01   dna     HSRSRCH_REF_LINK_ID -> HDRSRCH_REF_LINK_ID
02/22/00    om     Created Module


===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "amssassert.h"
#include "task.h"
#include "qw.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdrerrno.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrtx.h"
#include "hdrtxi.h"
#include "hdrrup.h"
#include "hdrlogi.h"
#include "hdrsrch.h"
#include "hdrtrace.h"

#include "hdrslp.h"
#include "hdrslpi.h"
#include "hdrslprx.h"
#include "hdrslptx.h"
#include "hdrstream.h"
#include "hdrtrace.h"

#include "hdrscp.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* A temporary hack to get access to set connection_req_tx_started */
extern void hdridles_creq_given_to_acmac( void );

/********** Route update function pointer **********/
hdrslp_route_update_gen_type hdrslp_route_update_gen_func = NULL;

rex_crit_sect_type hdrslp_crit_sect;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_INIT

DESCRIPTION
  This function does all one-time initialization.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_init()
{
  hdrslp_mp_init_pool();
  hdrslp_reset();
  hdrslp_reset_stats();
  hdrslptx_init_crit_sect();

  /* Reset all the connected state related logging statistics. */
  hdrslp_log_reset_conn_state_stats( HDRSLP_ALL_STATS );

  hdrslp_reset_seqno = HDRSLP_INITIALIZE_MSG_SEQ;
  hdrslp_in_reset = FALSE;
  hdrslp_access_attempted_since_entering_idle = FALSE;

  HDR_MSG_PROT (MSG_LEGACY_HIGH,  "SLP initialized.");
}

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRSLP_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the SLP verbose debug setting.
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrslp_set_verbose_debug 
( 
  uint8 verbose_debug_setting 
)
{
  hdrslp_verbose_debug = verbose_debug_setting;
} /* hdrslp_set_verbose_debug() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to SLP.

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

void hdrslp_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr 
)
{
  rex_tcb_type *hdrtx_tcb_ptr; /* temp ptr */
  /* This protocol runs in the RX and TX tasks. To make sure we reset   */
  /* SLP before any signaling data is sent, we process the indications  */
  /* in the TX task context, which has the highest priority.            */
  hdrtx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
  if ( rex_self() == hdrtx_tcb_ptr )
  {
    /* Indication is from another protocol in the TX task, so consider  */
    /* it an internal indication.                                       */
    hdrslp_process_ind( ind_name, ind_data_ptr );
  }
  else
  {
    hdrtx_queue_ind( HDRHAI_SIGNALING_LINK_PROTOCOL, 
                     ind_name, 
                     ind_data_ptr );
  }

} /* hdrslp_ind_cb() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever HMP receives a
  message for SLP.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_msg_cb
(
  dsm_item_type* msg_ptr
)
{
  rex_tcb_type *hdrtx_tcb_ptr; /* temp ptr */

  hdrtx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
  if ( rex_self() == hdrtx_tcb_ptr )
  {
    /* Indication is from another protocol in the TX task, so consider     */
    /* it an internal message                                              */
    hdrslp_process_msg( msg_ptr, HDRHAI_IN_USE );
  }
  else
  {
    hdrtx_queue_msg( HDRHAI_SIGNALING_LINK_PROTOCOL,
                     HDRHAI_IN_USE,
                     msg_ptr );
  }
} /* hdrslp_msg_cb() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_PROCESS_SIG

DESCRIPTION
  This function is the signal handler for SLP. The only signal for
  SLP is the callback processing signal HDRTX_SLP_CB_SIG.

DEPENDENCIES
  None
 
PARAMETERS
  signal - REX signal
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_sig
(
  rex_sigs_type signal
)
{
  if ( signal == HDRTX_SLP_CB_SIG )
  {
    if (hdrslp_verbose_debug)
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Process slp cb sig"  );
    }
    hdrslp_cb_process();
  }
  else
  {
    ERR( "SLP rcvd unknown sig %d", signal, 0, 0 );
  }
} /* hdrslp_process_sig */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRSLP_TX_QUEUE_IS_EMPTY

DESCRIPTION
   This function returns TRUE if the slp tx queue is empty, otherwise
   it returns FALSE.

DEPENDENCIES
  None.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  - If the slp tx queue is empty
  FALSE - otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrslp_tx_queue_is_empty (hdrhai_protocol_name_enum_type caller)
{
   SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

   return ( hdrslp_tx_queue_first_item_ptr == NULL );
}

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRSLP_GET_TX_QUEUE_SIZE

DESCRIPTION
   This function returns the total size of the messages in the SLP tx queue,
   including an estimate of the size the SLP headers will be.

DEPENDENCIES
  None.

PARAMETERS
  msg_count_ptr - Output parameter; Number of messages in the queue
  time_since_prev_req - PN roll interrupts since last call

RETURN VALUE
  Number of bytes queued in SLP for transmit, including SLP headers.

SIDE EFFECTS
  None.
=============================================================================*/
uint32 hdrslp_get_tx_queue_size
( 
  uint32 *msg_count_ptr,
  uint16 time_since_prev_req
)
{
  int i; /* index */
  hdrslp_msg_struct_type* tx_queue_item_ptr; /* Item in SLP TX queue */
  uint32 total_msg_bytes = 0; /* Accumulates number of bytes queued for xmit */
  uint32 msg_count = 0; /* How many messages are on the queue */

  uint16 msg_size = 0;
  rex_tcb_type *hdrtx_tcb_ptr; /* temp ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrslp_verbose_debug)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"=TX=  get_tx_queue_size time_since %d", 
                 time_since_prev_req);
  }

  hdrslp_enter_crit_sect();

  tx_queue_item_ptr = hdrslp_tx_queue_first_item_ptr;

  /* Update the timers for SLP retransmit buffers and SLP ACKs */
  if ( time_since_prev_req ) 
  {
    hdrslp_update_tx_timers( time_since_prev_req );
  }
  else
  {
    /* RMAC3 gets packet from PCP every 4 slots instead of 16 slots and always call
     * hdrslp_get_tx_queue_size().
     * Because time_since_prev_req is in units of frame(16 slots), we need 
     * to update slp cb list timer when receiving request from RMAC3.  
     */
    hdrslp_be_msg_tx_complete();
  }

  hdrtx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
  (void) rex_set_sigs( hdrtx_tcb_ptr, HDRTX_SLP_CB_SIG );

  /* Add up the sizes of new messages queued for transmit */
  while( tx_queue_item_ptr != NULL )
  {
    msg_size = dsm_length_packet( tx_queue_item_ptr->sdu.msg_ptr );
    HDR_MSG_PROT_1( MSG_LEGACY_LOW, "SLPTXQ: New msg size %d", msg_size );
    total_msg_bytes += msg_size;

    /* Check SLP message type */
    if( tx_queue_item_ptr->reliable )
    {
      /* Reliable SLP messages have a 2 byte header */
      total_msg_bytes += 2; 
    }
    else
    {
      /* Best effort SLP messages have a 1 byte header */
      total_msg_bytes += 1;
    }
    
    msg_count++;
    HDR_MSG_PROT_3( MSG_LEGACY_MED, "SLPTXQ: Adding xmit(%d,%d) of size %d to queue size", 
                    tx_queue_item_ptr->sdu.prot_id,
                    tx_queue_item_ptr->sdu.prot_msg,
                    msg_size);

    tx_queue_item_ptr = tx_queue_item_ptr->next;
  }

  /* Add in the sizes of any messages ready for retransmit */
  for ( i = 0; i < 4; i++ )
  {
    if ((hdrslp_rxb_timer_mask & (0x01 << i)) &&
        (hdrslp_rexmit_buffer[i] != NULL ) &&
        (hdrslp_rexmit_buffer[i]->queue == SLP_REXMIT_Q))
    {
      msg_size = dsm_length_packet( hdrslp_rexmit_buffer[i]->sdu.msg_ptr );
      HDR_MSG_PROT_1( MSG_LEGACY_LOW, "SLPTXQ: Rexmit msg size %d", msg_size );

      /* rexmit buffer already has SLP header, so it is included in msg_size */

      total_msg_bytes += msg_size;
      HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                      "SLPTXQ: Adding rexmit[%d] of msg_size %d to queue size", i, msg_size);	
      msg_count++;
    }
  }

  if ( hdrslp_fragment_buffer )
  {
    /* Include size of message fragment left over from last subframe */
    msg_size = dsm_length_packet( hdrslp_fragment_buffer->sdu.msg_ptr );
    msg_size -= hdrslp_fragment_tx_count;

    /* fragment buffer already has SLP header, so it is included in msg_size */

    HDR_MSG_PROT_1( MSG_LEGACY_MED, "SLPTXQ: Fragment msg size %d", msg_size );

    total_msg_bytes += msg_size;
    msg_count++;
  }

  if (msg_count == 0)
  {
    /* If we have nothing else to send, see if we have ACK only packets */
    for ( i = 0; i < 4; i++ )
    {
      if (hdrslp_rx_ack_timer_mask & (0x01 << i))
      {
        total_msg_bytes += 3; /* ACK only header is 3 bytes */
        msg_count++;
        HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                        "SLPTXQ: Adding ack only[%d] for %d to queue size",
                        i, hdrslp_rx_ack_buffer[i] & 0x0007);
      }
    }
  }

  *msg_count_ptr = msg_count;

  hdrslp_leave_crit_sect();

  return total_msg_bytes;
} /* hdrslp_get_tx_queue_size() */

/* <EJECT> */ 
/*--------------------------------------------------------------------------
------------------         Transmit Functions        -----------------------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_SEND_MESSAGE

DESCRIPTION
  This function enqueues a signaling message onto SLP's TX queue
  by its priority.

  NOTE: This function runs in the context of the protocol sending
        the message. 

DEPENDENCIES
  None

PARAMETERS
  channel            - Channel SDU has to be transmitted on
  reliable           - Set to true if reliable delivery transport is required
  priority           - Priority of the message
  message_body       - dsm item chain containing the signaling message
  slp_outcome_cbPtr  - Function to be called after message has been
                       transmitted successfully aor transmission was aborted
  outcome_tag_ptr    - User data for callback function

RETURN VALUE
  Enqueueing status (E_SUCCESS or E_PROT_Q_FULL)

SIDE EFFECTS

===========================================================================*/

hdrerrno_enum_type hdrslp_send_msg
(
  hdrhai_channel_enum_type  channel,
  boolean                   reliable,
  uint8                     priority,
  dsm_item_type*            msg_ptr,
  hdrslp_outcome_cb_type    slp_outcome_cb_ptr,
  void*                     outcome_tag_ptr 
)
{
  hdrerrno_enum_type       err_num;
  hdrslp_msg_struct_type*  tx_msg_ptr;
  uint16                   new_packet_length;  /* new packet size for MC RUP*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check the parameters */
  if ( (msg_ptr == NULL) 
       &&
       (channel != HDRHAI_AC) )
  {
    ERR( "SLP: Got NULL msg ptr in send_msg()", 0, 0, 0 );
    return E_INVALID_ARG;
  }

  /* This runs from another protocol's context and hence from  */
  /* a lower priority than the TX task. The insertion needs    */
  /* to be protected.                                          */
  REX_DISABLE_PREMPTION();

  hdrslp_enter_crit_sect();
  
  /* Enqueue an item according to the priority and get the  */
  /* data pointer back to fill in the msg, channel, etc.    */
  tx_msg_ptr = hdrslp_tx_queue_insert_item( priority );
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "SPLF: inserting in 0x%x",tx_msg_ptr);
  if ( tx_msg_ptr )
  {
    /* Update the fields */
    tx_msg_ptr->sdu.msg_ptr      = msg_ptr;
    tx_msg_ptr->sdu.length       = dsm_length_packet( msg_ptr );
    tx_msg_ptr->sdu.channel      = channel;
    if( msg_ptr != NULL )
    {
      tx_msg_ptr->sdu.prot_id      = *(msg_ptr->data_ptr);
      if ( msg_ptr->used == 1 )
      {
        if ( msg_ptr->pkt_ptr ) 
          tx_msg_ptr->sdu.prot_msg = *(msg_ptr->pkt_ptr->data_ptr);
      }
      else
      {
        tx_msg_ptr->sdu.prot_msg   = *((msg_ptr->data_ptr) + 1);
      }
    }
    else
    {
      /* Else this is a Route Update message.
       Fill in the appropriate protocol & message id */
      tx_msg_ptr->sdu.prot_id = HDRHAI_N_RUP_TYPE;
      tx_msg_ptr->sdu.prot_msg = 0; /* 0 = HDRRUP_RUP_MSG_ID */
    }
    tx_msg_ptr->outcome_func_ptr = slp_outcome_cb_ptr;
    tx_msg_ptr->outcome_tag_ptr  = outcome_tag_ptr;
    tx_msg_ptr->priority         = priority;
    tx_msg_ptr->reliable         = reliable;
    tx_msg_ptr->queue            = SLP_XMIT_Q;
	HDR_MSG_PROT_2 (MSG_LEGACY_MED,  "SPLF: inserting in 0x%x with tag ptr 0x%x",
		                   tx_msg_ptr, tx_msg_ptr->outcome_tag_ptr);

#ifndef HDRSLP_NO_LOGGING
    /* Default the fragmentation related debug fields so that we only have */
    /* to change them if we actually fragment the message.                 */
    tx_msg_ptr->sdu.frg = FALSE;
    tx_msg_ptr->sdu.fst = TRUE;
    tx_msg_ptr->sdu.lst = TRUE;
#endif  /* HDRSLP_NO_LOGGING */

    err_num = E_SUCCESS;
  }
  else
  {
    err_num = E_PROT_Q_FULL;
	HDR_MSG_PROT (MSG_LEGACY_MED,  "SPLF: Tx queue full");
  }    

  if ( ( err_num == E_SUCCESS ) &&
       ( ( tx_msg_ptr->sdu.prot_id & 0x7F ) == HDRHAI_ROUTE_UPDATE_PROTOCOL ) &&
       ( hdrscp_get_current_subtype( (hdrhai_protocol_name_enum_type)(tx_msg_ptr->sdu.prot_id & 0x7F) )
           == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
         || 
         hdrscp_get_current_subtype( (hdrhai_protocol_name_enum_type)(tx_msg_ptr->sdu.prot_id & 0x7F) )
           == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */
       ) &&
       ( tx_msg_ptr->sdu.prot_msg == 0x00 ) &&
       ( tx_msg_ptr->sdu.channel == HDRHAI_RTC )
     )
  {
    /* It is MC RUP message on RTC, ask RUP to update revB fields of RUM.
     * RUP will query RMAC for SLP carrier information. This information will not 
     * be changed as long as SLP queue is not empty.
     */ 
    HDR_MSG_PROT(MSG_LEGACY_MED,"Updating RU msg content");
    hdrrup_update_mc_rup_msg(tx_msg_ptr->sdu.msg_ptr);

    /* Length of sdu may be shorter if ReferencePilotChannelInclude == 0*/
    new_packet_length = dsm_length_packet(tx_msg_ptr->sdu.msg_ptr);
    if (new_packet_length != tx_msg_ptr->sdu.length)
    {
      HDR_MSG_PROT_2(MSG_LEGACY_MED,"Updating RU msg length from %d to %d", 
                                     tx_msg_ptr->sdu.length,
                                     new_packet_length);
      tx_msg_ptr->sdu.length  = new_packet_length;
    }
  }

  hdrslp_leave_crit_sect();

  REX_ENABLE_PREMPTION();

  HDR_MSG_PROT_3 (MSG_LEGACY_MED,  "SPLF: Msg enq'd (c %d, s %d, 0x%x)",
                 channel, err_num, tx_msg_ptr );

  return err_num;

} /* hdrslp_send_msg() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_ABORT_MESSAGE

DESCRIPTION
  This function is used to prevent a reliable delivery message to be
  retransmitted if the upper layers know that the message was already
  received, but the layer 2 acknowledgement got lost.

  NOTE: This function runs in the context of the protocol sending
        the message. 

DEPENDENCIES
  None

PARAMETERS
  slp_outcome_cb  - Function to be called after message has been
                    transmitted successfully aor transmission was aborted
  outcome_tag_ptr - User data for callback function

RETURN VALUE
  Enqueueing status (E_SUCCESS, E_PROT_MSG_Q_FULL)

SIDE EFFECTS

===========================================================================*/

hdrerrno_enum_type hdrslp_abort_msg
( 
  hdrslp_outcome_cb_type     slp_outcome_cb,
  void*                      outcome_tag_ptr 
)
{
  hdrslp_msg_struct_type* abort_msg_ptr = NULL;
  uint8 seqno;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrslp_enter_crit_sect();
  abort_msg_ptr = hdrslp_find_msg( slp_outcome_cb, outcome_tag_ptr );
  if ( abort_msg_ptr && abort_msg_ptr->sdu.msg_ptr )
  {
    seqno = HDRSLP_SDU_SEQNO( abort_msg_ptr->sdu.msg_ptr->data_ptr );
    hdrslp_leave_crit_sect();

    hdrslp_process_tx_ack( seqno );    
    return E_SUCCESS;
  }
  else if ( abort_msg_ptr && 
            abort_msg_ptr->sdu.msg_ptr == NULL )
  {
    /* This should not happen. Everytime message is released, rexmit buffer
     * is also reset. */
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "Message is released, but SLP rexmit buffer is not released" );
    hdrslp_leave_crit_sect();
    return E_NO_DATA;
  }
  else
  {
    hdrslp_leave_crit_sect();
    return E_NO_DATA;
  }

} /* hdrslp_abort_msg() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_GET_COMMON_AC_PKT

DESCRIPTION
  This function causes SLP to create an SDU of up to max_bytes length
  from registered common access channel data (data to be transmitted
  with every access channel probe).

DEPENDENCIES
  None

PARAMETERS
  max_bytes            - Maximum number of bytes current TX frame can 
                         accommodate  
  time_since_prev_req  - Number of TX build interrupts since last call
  msg_ptr              - Will be set to pointer to dsm_item chain 
                         containing the common AC SDU
  channel_ptr          - Will be set to AC channel

RETURN VALUE
  Number of bytes in SDU

SIDE EFFECTS

===========================================================================*/

uint16 hdrslp_get_common_ac_pkt
(
  uint16                 max_bytes,
  uint16                 time_since_prev_req,
  dsm_item_type**        msg_ptr,
  hdrhai_channel_enum_type  *channel_ptr
)
{
  uint16 length = 0;
  uint16 routeupdate_len = 0;
  hdrslp_msg_struct_type tx_msg;
  hdrslp_msg_struct_type* curr_msg_ptr = NULL;
  hdrslp_msg_struct_type* next_msg_ptr = NULL;
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrhai_tunnel_mode_type curr_tunnel_mode;
#endif /* FEATURE_LTE_TO_HDR_OH */
  rex_tcb_type *hdrtx_tcb_ptr; /* temp ptr */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrslp_verbose_debug)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "=TX=  get_common_ac time_since %d",
               time_since_prev_req);
  }

  /* If this is for a new frame update the timers and call all callbacks */
  /* Here we set a signal to ourselves which's handler function          */
  /* is hdrslp_cb_process(). This way we process the callbacks after     */
  /* building the frame.                                                 */
  if ( time_since_prev_req ) 
  {
    hdrslp_update_tx_timers( time_since_prev_req );
  }
  hdrtx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
  (void) rex_set_sigs( hdrtx_tcb_ptr, HDRTX_SLP_CB_SIG );
  /* If this is the last call from PCP our TX queue should be empty */
  if ( hdrslp_tx_queue_first_item_ptr == NULL ) 
  {
    return 0;
  }
  #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
  else if ( hdrslp_is_waiting_for_uati_assignment() )
  {
    if ((hdrslp_tx_queue_first_item_ptr->sdu.prot_id == 
         HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) &&
        (hdrslp_tx_queue_first_item_ptr->sdu.prot_msg == 0) /* uati_req */ )
    {
      /* Only allow UATIRequest messages while waiting for UATIAssignment */
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "SLPQH: Sending RouteUpdate for UATIReq after IdleHO");
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "SLPQH: Waiting for UATIAssignment. Not sending RouteUpdate");

      return 0;
    }
  }
  else 
  #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */
  if(hdrslp_is_waiting_for_bundling())
  {
    /* Waiting for more AC messages to complete bundling request */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "BUNDLE: Waiting for bundling. Not sending RouteUpdate");
    return 0;
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  curr_tunnel_mode = hdrsap_get_current_tunnel_mode_value ( 
      HDRHAI_SIGNALING_LINK_PROTOCOL );
#endif /* FEATURE_LTE_TO_HDR_OH */

  if (( hdrslp_route_update_gen_func != NULL ) 
#ifdef FEATURE_LTE_TO_HDR_OH
      ||
      ( curr_tunnel_mode == HDRHAI_TUNNEL_MODE_ACTIVE_HO )
#endif /* FEATURE_LTE_TO_HDR_OH */
      )
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if (curr_tunnel_mode == HDRHAI_TUNNEL_MODE_ACTIVE_HO)
    {
      tx_msg.sdu.msg_ptr = hdrsap_get_rup_msg();
      ASSERT ( tx_msg.sdu.msg_ptr != NULL );
      /* RUP message will be freed when Ack is received. Make sure we don't
       * have dangling pointer. */
      hdrsap_clear_rup_msg();
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    {
      tx_msg.sdu.msg_ptr = (*hdrslp_route_update_gen_func)
                                        ( HDRRUP_BUILD_NEW_RUP_MESSAGE, 
                                          HDRRUP_RUP_SIZE_NO_RESTRICTION );
    }

    /* Size of default route update message */
    routeupdate_len = dsm_length_packet( tx_msg.sdu.msg_ptr ); 

    /* Local pointer to first message on SLP TX Q */
    curr_msg_ptr = hdrslp_tx_queue_first_item_ptr;

    if ( (curr_msg_ptr != NULL )
#ifdef FEATURE_LTE_TO_HDR_OH
    &&
    ( curr_tunnel_mode != HDRHAI_TUNNEL_MODE_ACTIVE_HO )
#endif /* FEATURE_LTE_TO_HDR_OH */
    )
    {
      if ( (routeupdate_len + curr_msg_ptr->sdu.length + 4) > max_bytes )
      /* SLP header size together for both messages is 4 bytes */
      {
        /* In case when default route update message is large, to avoid not
         * being able to send other signaling messages request a route update
         * of reduced size */

        /* This check also ensures that if DOS message is first in the
         * SLP TX Q we will be able fit it completely in the probe */

        /* Free DSM item allocated previously for RUM */
        dsm_free_packet( &(tx_msg.sdu.msg_ptr) );

        tx_msg.sdu.msg_ptr = (*hdrslp_route_update_gen_func)
                                 ( HDRRUP_REBUILD_SAME_RUP_MESSAGE, 
                                   HDRRUP_RUP_REDUCED_SIZE );
      }
      else if ( (curr_msg_ptr->sdu.prot_id  == HDRHAI_IDLE_STATE_PROTOCOL) &&
                (curr_msg_ptr->sdu.prot_msg == HDRIDLE_CONN_REQ_ID) )
      {
        /* Local pointer to second message on SLP TX Q */
        next_msg_ptr = (hdrslp_msg_struct_type*)curr_msg_ptr->next;

        if ( (next_msg_ptr != NULL) && 
             (next_msg_ptr->sdu.prot_id == HDRHAI_DATA_OVER_SIGNALING_PROTOCOL) &&
             (next_msg_ptr->sdu.prot_msg == HDRDOS_DATA_OVER_SIGNALING_MSG_ID) )
        {
          /* If connection request and DOS message are first and second on the
           * SLP TX Q. We need to send it together without fragmentation in 
           * single access probe, so request smaller route update */
          if ( (routeupdate_len + curr_msg_ptr->sdu.length
                                + next_msg_ptr->sdu.length + 6) > max_bytes )
          /* SLP header size together for these messages is 6 bytes */
          {
            /* Free DSM item allocated previously for RUM */
            dsm_free_packet( &(tx_msg.sdu.msg_ptr) );

            tx_msg.sdu.msg_ptr = (*hdrslp_route_update_gen_func)
                                   ( HDRRUP_REBUILD_SAME_RUP_MESSAGE, 
                                     HDRRUP_RUP_REDUCED_SIZE );
          }
        }
      } /* End check for DOS and connection request on SLP TX Q */
      else
      {
        /* Route update message size is good for current probe */
      }
    }

    tx_msg.sdu.length  = dsm_length_packet( tx_msg.sdu.msg_ptr );
    tx_msg.sdu.channel = HDRHAI_AC;
    tx_msg.reliable    = FALSE;
  
    /* Add header to route update */
    length = tx_msg.sdu.length + hdrslp_create_header( &tx_msg );
  
    if ( length > max_bytes )
    {
      ERR( "Route Update too large for AC probe (%d/%d)",
        length, max_bytes, 0 );

      dsm_free_packet( &(tx_msg.sdu.msg_ptr) );
      return 0;
    }
  }
  else
  {
          ERR_FATAL( "hdrslp_route_update_gen_func not initialzed",
                     0 ,0 , 0 );
  }

#ifndef HDRSLP_NO_LOGGING
  /* The common AC packet is never fragmented, so we can log tx_msg */
  hdrslp_log_msg( HDRSRCH_MON_LINK_ID,
                  tx_msg.sdu.length, 
                  HDRSLP_SDU_HDR_SIZE( tx_msg.sdu.msg_ptr->data_ptr ),
                  FALSE, /* Not broadcast */
                  tx_msg.sdu.channel, 
                  FALSE,
                  tx_msg.sdu.rel, 
                  FALSE, 
                  tx_msg.sdu.sno, 
                  tx_msg.sdu.ano, 
                  tx_msg.sdu.ano_valid, 
                  tx_msg.sdu.msg_ptr );
#endif
  
  /* Fill in results */
  *msg_ptr = tx_msg.sdu.msg_ptr;
  *channel_ptr = tx_msg.sdu.channel;
  
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "TX AC RUP msg (%d bytes)", length);
  
  return length;

} /* hdrslp_get_common_ac_pkt */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_GET_PKT

DESCRIPTION
  This function causes SLP to create an SDU of up to max_bytes length
  from pending TX data.

DEPENDENCIES
  None

PARAMETERS
  max_bytes            - Maximum number of bytes current TX frame can 
                         accommodate
  time_since_prev_req  - Number of TX build interrupts since last call
  msg_ptr              - Will be set to pointer to dsm_item chain 
                         containing the SDU
  channel_ptr          - Will be set to channel SDU is destined for

RETURN VALUE
  Number of bytes in SDU

SIDE EFFECTS

===========================================================================*/

uint16 hdrslp_get_pkt
(
  boolean                first_callfor_slpdata,
  uint16                 max_bytes,
  uint16                 time_since_prev_req,
  dsm_item_type**        msg_ptr,
  hdrhai_channel_enum_type* channel_ptr
)
{
  uint16 length;
  hdrslp_sdu_struct_type tx_sdu;
#ifndef HDRSLP_NO_LOGGING
  hdrslp_sdu_struct_type *log_sdu_ptr;
#endif /* HDRSLP_NO_LOGGING */

  hdrhai_stream_enum_type stream_id;
  rex_tcb_type *hdrtx_tcb_ptr; /* temp ptr */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If this is for a new frame update the timers and call all callbacks */
  /* Here we set a signal to ourselves which's handler function          */
  /* is hdrslp_cb_process(). This way we process the callbacks after     */
  /* building the frame. */

  if (hdrslp_verbose_debug)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "=TX=  get_pkt time_since %d", 
                  time_since_prev_req);
  }

  if ( time_since_prev_req ) 
  {
    hdrslp_update_tx_timers( time_since_prev_req );
  }
  hdrtx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
  (void) rex_set_sigs( hdrtx_tcb_ptr, HDRTX_SLP_CB_SIG );

  /* Prevent us from sending new msgs while in reset */
  if ( hdrslp_in_reset ) return 0;

  /* Tell SLP which channel this is for */
  tx_sdu.channel = *channel_ptr;

  length = hdrslp_build_tx_data( first_callfor_slpdata, 
                                 &tx_sdu, 
                                 (uint8) max_bytes );
  if (hdrslp_verbose_debug)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "=TX= pkt length %d bytes", length);
  }

  if ( length )
  {
    /* Fill in results */
    *msg_ptr = tx_sdu.msg_ptr;
    ASSERT( *channel_ptr == tx_sdu.channel );

    /* Update stats */
    hdrslp_stats.tx_msg_cnt++;
    hdrslp_stats.tx_byte_cnt += length;
    hdrslp_stats.tx_min_msg_size = MIN( hdrslp_stats.tx_min_msg_size, length );
    hdrslp_stats.tx_max_msg_size = MAX( hdrslp_stats.tx_max_msg_size, length );

    if ( tx_sdu.channel == HDRHAI_RTC )
    {
      hdrslp_stats.tx_traffic_msg_cnt++;
      if (tx_sdu.prot_id != 0)
      {
        if (!tx_sdu.frg || tx_sdu.fst)
        {
          HDR_MSG_PROT_3 (MSG_LEGACY_HIGH,  
                         "=TX= TX RTC msg (P 0x%x, ID 0x%x, %d bytes)",
                         tx_sdu.prot_id & 0x7F,
                         tx_sdu.prot_msg,
                         length );

          stream_id = hdrstream_get_stream_for_protocol(tx_sdu.prot_id & 0x7F);
          HDRTRACE_SLP_RTC_MSG_TX( tx_sdu.prot_id,
                                   tx_sdu.prot_msg, 
                                   hdrstream_get_apptype_for_neg_stream(stream_id));
        }
        else
        {
          HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "=TX= TX RTC msg fragment, %d bytes", 
                         length);
        }
      }
      else
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH,  "=TX= TX RTC Header Only ACK");
        HDRTRACE_SLP_RTC_MSG_TX( HDRHAI_SIGNALING_LINK_PROTOCOL, 
                                 HDRSLP_HEADER_ONLY_MSG,
                                 0 );
      }
    }
    else
    {
      /* This flag is used by ovhd to determine if AHO is allowed when 
       * DO session is not open
         */
      hdrslp_access_attempted_since_entering_idle = TRUE;

      hdrslp_stats.tx_access_msg_cnt++;
      /* set idle connection_setup substate, 
       * at this point, data has been given to acmac.
       */
      if ((tx_sdu.prot_id == HDRHAI_IDLE_STATE_PROTOCOL) &&
          (tx_sdu.prot_msg == 0x01 /* connection_req */))
      {
        hdridles_creq_given_to_acmac();
      }      
      HDR_MSG_PROT_3 (MSG_LEGACY_HIGH,  "=TX= TX AC msg (P 0x%x, ID 0x%x, %d bytes)",
                     tx_sdu.prot_id, 
                     tx_sdu.prot_msg,
                     length );

      stream_id = hdrstream_get_stream_for_protocol(tx_sdu.prot_id & 0x7F);
      HDRTRACE_SLP_AC_MSG_TX( tx_sdu.prot_id, 
                              tx_sdu.prot_msg, 
                              hdrstream_get_apptype_for_neg_stream(stream_id));
    }

#ifndef HDRSLP_NO_LOGGING
    if (tx_sdu.frg && tx_sdu.fst)
    {
      /* This is the first part of a fragmented message */
      /* tx_sdu.msg_ptr is only the first fragment, but */
      /* we want to log the entire message here.        */
      log_sdu_ptr = &hdrslp_fragment_buffer->sdu;
    }
    else if(tx_sdu.frg)
    {
      /* Don't log continuing fragments */
      log_sdu_ptr = NULL;
    }
    else
    {
      /* The message is not fragmented */
      log_sdu_ptr = &tx_sdu;
    }

    if(log_sdu_ptr != NULL)
    {
      hdrslp_log_msg( HDRSRCH_MON_LINK_ID,
                      log_sdu_ptr->length, 
                      HDRSLP_SDU_HDR_SIZE( log_sdu_ptr->msg_ptr->data_ptr ),
                      FALSE, /* Not broadcast */
                      log_sdu_ptr->channel, 
                      FALSE,
                      log_sdu_ptr->rel, 
                      log_sdu_ptr->frg, 
                      log_sdu_ptr->sno, 
                      log_sdu_ptr->ano, 
                      log_sdu_ptr->ano_valid, 
                      log_sdu_ptr->msg_ptr );
    }
#endif 
  }

  return length;

} /* hdrslp_get_pkt() */

/* <EJECT> */ 
/*--------------------------------------------------------------------------
-----------------          Receive Functions          ----------------------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_PUT_PKT

DESCRIPTION
  This function is used by PCP to deliver received SDUs to SLP
  for further processing.

DEPENDENCIES
  None

PARAMETERS
  received_pkt_ptr - dsm item chain containing the received SDU

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void hdrslp_put_pkt
(
  dsm_item_type* received_pkt_ptr 
)
{
  /* Call SLP's processing function - received msgs don't get     */
  /* queued since PCP and SLP run in the context of the rx task.  */

  hdrslp_process_rcvd_signaling( received_pkt_ptr );
}

/*===========================================================================

FUNCTION HDRSLP_SET_WAITING_FOR_BUNDLING_FLAG

DESCRIPTION
  This function is used to set hdrslp_waiting_for_bundling flag.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

void hdrslp_set_waiting_for_bundling_flag
( 
  boolean waiting_for_bundling
)
{
  hdrslp_tx_set_waiting_for_bundling_flag(waiting_for_bundling);
} /* hdrslp_set_waiting_for_bundling_flag() */

/*===========================================================================

FUNCTION HDRSLP_IS_WAITING_FOR_BUNDLING

DESCRIPTION
  This function is called to check if we are still waiting for bundling.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

boolean hdrslp_is_waiting_for_bundling(void)
{
  return (hdrslp_tx_is_waiting_for_bundling());
} /* hdrslp_is_waiting_for_bundling() */

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
/*===========================================================================

FUNCTION HDRSLP_STOP_WAITING_FOR_UATI_ASSIGNMENT

DESCRIPTION
  This function is called when AMP no longer expects a UATIAssignment message.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

void hdrslp_stop_waiting_for_uati_assignment(void)
{
  hdrslp_tx_stop_waiting_for_uati_assignment();
} /* hdrslp_stop_waiting_for_uati_assignment() */

/*===========================================================================

FUNCTION HDRSLP_IS_WAITING_FOR_UATI_ASSIGNMENT

DESCRIPTION
  This function is called when AMP no longer expects a UATIAssignment message.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

boolean hdrslp_is_waiting_for_uati_assignment(void)
{
  return (hdrslp_tx_is_waiting_for_uati_assignment());
} /* hdrslp_is_waiting_for_uati_assignment() */

#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSLP_FIRST_ACCESS_ATTEMPTED

DESCRIPTION
  This function checks if the AT has started an access attempt since entering 
  idle state when there is no DO session. 
  
DEPENDENCIES
  To call this function, user must also check DO session is not open

PARAMETERS
  None

RETURN VALUE
  TRUE - If AT has started access attempt since entering idle state 

SIDE EFFECTS
===========================================================================*/

boolean hdrslp_first_access_attempted(void)
{
  return hdrslp_access_attempted_since_entering_idle;
} /* hdrslp_first_access_attempted() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSLP_ENTER_CRIT_SECT

DESCRIPTION
  This enters SLP critcal section to protect SLP data struct
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void hdrslp_enter_crit_sect( void )
{
  static boolean first = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( first )
  {
    rex_init_crit_sect(&hdrslp_crit_sect);
    first = FALSE;
  }

  if ( hdrslp_verbose_debug )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "SLP enter crit task:%x", 
                    rex_self() );
  }

  rex_enter_crit_sect(&hdrslp_crit_sect);
}

/*===========================================================================

FUNCTION HDRSLP_LEAVE_CRIT_SECT

DESCRIPTION
  This leaves SLP critcal section
  
DEPENDENCIES
  Must be called after enter_crit_sect and must be matched in pairs.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void hdrslp_leave_crit_sect( void )
{
  rex_leave_crit_sect(&hdrslp_crit_sect);

  if ( hdrslp_verbose_debug )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "SLP leave crit task:%x",
                    rex_self() );  
  }
}

