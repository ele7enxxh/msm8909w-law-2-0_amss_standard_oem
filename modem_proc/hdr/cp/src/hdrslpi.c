/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            H D R   S I G N A L I N G   L A Y E R   P R O T O C O L

                     I N T E R N A L   F U N C T I O N S

GENERAL DESCRIPTION
  This file contains data definitions and functions for SLP's internal use.

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrslpi.c_v   1.39   18 Apr 2003 11:28:34   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrslpi.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
05/24/13   ukl     Added additional critical sections to guard SLP data structures.
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
04/04/13   cnx     Deregister roll callback when connection is closed.
01/24/13   wsh     Added crit_sect to guard SLP data structures
09/11/12   cnx     Handle TX and RX messages during active handoff
04/17/12   cnx     Added HDRIND_SAP_RETURNING_TO_CLOSE.
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
11/08/11   cnx     Fixed compiler warnings.
09/24/11   cnx     Changed subtype checking for Optimized Handoff.
07/27/11   cnx     Merged Optimized Handoff changes.
01/07/11   kss     Fixed sync issue by providing sync pkt hstr for validation.
12/17/10   cnx     Fixed compiler warnings.
12/01/10   cnx     Fixed dsm item allocation issue when SLP reset msg rcvd on CC.
06/04/09   wsh     Removed unneeded rx.h and tx.h from HDR files
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/23/09   wsh     Fixed crash in logging
12/23/08   pba     Support for Quick Idle Protocol
12/10/08   smd     Replaced assert.h with amssassert.h
11/20/08   kss     Removed/replaced intlocks.
08/07/08   wsh     Fixed OTA msg parsed w/ wrong subtype during negotiation
05/13/08   pba     Fixed channel number that is displayed in the logging of
                   signalling messages.
04/09/08   sju     Fixed logging by using new hdrscp_get_logging_subtype()
11/08/07   sju     Added support to allow AHO before the first access attempt
                   when there is no DO session
09/11/07   cws     Added support for MCTAP
03/05/07   sju     Added MMPA in hdrslp_get_app_type()
01/08/07   yll     Added support for CSNA.
12/06/06   sju     Make SLP reassembly buffers standard compliant.
11/27/06   sju     Added support for handling CCMAC_INTERRUPTED indication. 
08/03/06   yll     Use different app_subtype for EMPA.
07/18/06   dna     Major RASM changes to allow reliable QoS call setup.
07/11/06   dna     Fix logging of msgs > 255 bytes.
06/16/06   dna     SLP was incorrectly initializing hdrslp_seqno_received to 7
                   marking seqnos 0,1,2 as already received when they were not.
05/10/06   yll/ksu EMPA: octet stream, octet sequencing
08/28/05   mpa     Added HDRIND_CCMAC_INTERRUPTED indication handling (no op)
07/11/05   dna     Added support for HDRHAI_ENH_TEST_APP_IS890A
07/07/05   dna     Log the protocol subtype in the app_type field of the
                   message log packets, until we implement the new log packet.
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
11/29/04   dna     Added HDRIND_IDLE_CONNECTION_ABORTED to abort connection 
                   setup before it starts.
11/29/04   grl     Added connected state stat logging support.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
09/09/04   mpa     Added hdrslp_is_apps_pkt().
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDRPROT SSID
04/17/03   mpa     Fixed handling of Reset message.
09/10/02   mpa     Merged changes from 08/01, 08/12 & 08/21 back in.
08/28/02   om      Fixed SLP reset during fragmentation.
08/21/02   mpa     Added HDRIND_LMAC_ACCESS_ENTRY_HANDOFF to abort msgs on TX
                   queue if transmission stopped before it even started
08/12/02   mpa     Fixed is_hdris890 initialization
08/01/02   mpa     Added is_hdris890 flag in signalling messages
07/23/02   om      Fixed handling of Reset message.
06/12/02   om      Fixed SLP reset during fragmentation of an ack'd msg.
06/11/02   om      Fixed calling of cb's w/ CANCELLED after clearing the 
                   rexmit and fragment buffer.
01/14/01   dna     Abort messages on the TX queue if the ACMAC transmission 
                   is stopped before it was started.
11/05/01   om      Fixed timestamping of logged messages
09/26/01   om      Fixed calling cb's from hdrslp_reset
09/25/01   om      Removed FEATURE_HDR_HAI_X4
09/05/01   dna     Check log mask and don't log messages if not enabled.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
                   Added some trace events.
09/04/01   dna     Added HDRIND_RUP_CONNECTION_INITIATED based on DDR-46
                   to reset SLP before reliable RTCACK message comes.
08/10/01   om      Changed hdrslp_mp_init_msg to return the msgs's next ptr
08/01/01   dna     Added logging of ack sequence number valid bit of SLP 
                   header as part of the signaling message log packet
07/10/01   dna     Reset SLP when a session is opened, and clear the tx queue
                   (and reset SLP) when a session is closed.
02/07/01   dna     Handle HDRIND_ALMP_REACQUIRING, HDRIND_ALMP_EXIT_HDR_MODE,
                   and HDRIND_ALMP_RETURNING_TO_IDLE indications to properly
                   clean up when we lose the TC or lose acquisition.
01/26/01   dna     Log broadcast messages separate from other CC messages.
02/22/00   om      Created Module


===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "hdrdsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "memory.h"
#include "qw.h"

#include "hdrtx.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdrerrno.h"
#include "hdrhai.h"
#include "log.h"
#include "hdrlogi.h"
#include "hdrdec.h"
#include "hdrlmac.h"
#include "hdrcmac.h"
#include "hdrsrch.h"
#include "hdrslp.h"
#include "hdrslpi.h"
#include "hdrslprx.h"
#include "hdrslptx.h"
#ifdef FEATURE_HDR_IS890
#include "hdrstream.h"
#endif
#include "amssassert.h"
#include "hdrtrace.h"
#include "hdrutil.h"
#include "hdrrmaclog.h"
#include "hdrscp.h"
#include "hdrrmac4.h"
#include "hdrpcp.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define HDRSLP_MAX_MSG_SIZE       2

#ifdef FEATURE_HDR_IS890
#define HDRSLP_MAX_STREAMS        4
#endif

#define HDRSLP_CONN_STATE_STATS_VER 0
  /* Version number of the connected state stats packet. */

/*** Reset msg sequence number ***/
uint8                   hdrslp_reset_seqno;
boolean                 hdrslp_in_reset;
boolean                 hdrslp_access_attempted_since_entering_idle;

/*** SLP statistics ***/
hdrslp_stat_struct_type  hdrslp_stats;

/*** Connected state related SLP log statistics ***/
hdrslp_log_conn_state_stats_struct_subtype hdrslp_conn_state_stats;

uint8               hdrslp_verbose_debug; 
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_PROCESS_IND

DESCRIPTION
  This function processes any indications given to SLP.

DEPENDENCIES
  None
 
PARAMETERS
  indication_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  indication_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr 
)
{

  SYS_ARG_NOT_USED(ind_data_ptr);/* remove this line when ind_data_ptr 
                                  * is used */

  HDRTRACE_IND( HDRHAI_SIGNALING_LINK_PROTOCOL, ind_name, 0 );

  switch ( ind_name )
  {
    case HDRIND_SMP_BOOT_COMPLETED:
      /* Priority dependency - if UATI response and ACACK are in same capsule,
       * the ACMAC must be higher priority to send TRANSMISSION_SUCCESSFUL
       * before session layer sends boot completed, or we call the message 
       * callback with E_FAILED instead of E_SUCCESS.
       */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "HDRIND_SMP_BOOT_COMPLETED ind rcvd");
      hdrslp_reset();
      break;

    case HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  
                   "HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL ind rcvd");
      hdrslp_cb_process();
      break;

    case HDRIND_ACMAC_TRANSMISSION_FAILED:
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_SAPTX_TRANSMISSION_FAILED_AC: 
      if (ind_name == HDRIND_SAPTX_TRANSMISSION_FAILED_AC)
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                     "HDRIND_SAPTX_TRANSMISSION_FAILED_AC ind rcvd");
      }
      else 
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                     "HDRIND_ACMAC_TRANSMISSION_FAILED ind rcvd");
      }
      hdrslp_cb_wait_queue_clear( E_FAILURE );
      hdrslp_cb_failure( E_FAILURE );
      hdrslp_cb_process();

      /* Flag was set to TRUE when ACMAC requested AC pkt from slp queue
       * Set it back to FALSE after transmission failed
       */
      hdrslp_access_attempted_since_entering_idle = FALSE;  
      break;

    case HDRIND_ACMAC_TRANSMISSION_ABORTED:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  
                   "HDRIND_ACMAC_TRANSMISSION_ABORTED ind rcvd");
      hdrslp_cb_wait_queue_clear( E_CANCELED );
      hdrslp_cb_failure( E_CANCELED );
      hdrslp_cb_process();

      /* Flag was set to TRUE when ACMAC requested AC pkt from slp queue
       * Set it back to FALSE after transmission aborted
       */
      hdrslp_access_attempted_since_entering_idle = FALSE;  
      break;

    case HDRIND_IDLE_CONNECTION_ABORTED:
    case HDRIND_LMAC_ACCESS_ENTRY_HANDOFF:
    case HDRIND_ACMAC_TRANSMISSION_STOPPED:
#ifdef FEATURE_LTE_TO_HDR_OH            
    case HDRIND_SAP_TRANSMISSION_STOPPED:
#endif /* FEATURE_LTE_TO_HDR_OH */
      if (ind_name == HDRIND_LMAC_ACCESS_ENTRY_HANDOFF)
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                     "HDRIND_LMAC_ACCESS_ENTRY_HANDOFF ind rcvd");
      }
      else if (ind_name == HDRIND_IDLE_CONNECTION_ABORTED)
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                     "HDRIND_IDLE_CONNECTION_ABORTED ind rcvd");
      }
#ifdef FEATURE_LTE_TO_HDR_OH   
      else if (ind_name == HDRIND_SAP_TRANSMISSION_STOPPED)
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "HDRIND_SAP_TRANSMISSION_STOPPED ind rcvd" );
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
      else
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH,  
                     "HDRIND_ACMAC_TRANSMISSION_STOPPED ind rcvd");

        /* Flag was set to TRUE when ACMAC requested AC pkt from slp queue
         * Set it back to FALSE after transmission failed
         */
        hdrslp_access_attempted_since_entering_idle = FALSE;  
      }
      /* If the transmision was stopped before it even started, there may
       * be messages on the TX queue that need to be aborted, too.
       */
      hdrslp_tx_queue_clear( E_ABORTED );
      hdrslp_cb_wait_queue_clear( E_ABORTED );
      hdrslp_cb_failure( E_ABORTED );
      hdrslp_cb_process();
      break;

    case HDRIND_RUP_CONNECTION_INITIATED:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  
                   "HDRIND_RUP_CONNECTION_INITIATED ind rcvd");
      hdrslp_reset();
      hdrslp_reset_seqno = HDRSLP_INITIALIZE_MSG_SEQ;
      break;
      
    case HDRIND_ALMP_RETURNING_TO_IDLE:
#ifdef FEATURE_LTE_TO_HDR_OH            
    case HDRIND_SAP_RETURNING_TO_CLOSE:
#endif /* FEATURE_LTE_TO_HDR_OH */
    case HDRIND_ALMP_REACQUIRING:
    case HDRIND_ALMP_EXIT_HDR_MODE:
    case HDRIND_SMP_SESSION_CLOSED:
      /* This indication handling is not a requirement from the HAI,      */
      /* but upper layers assume that their message transmission ends if  */
      /* a) they send a message on the RTC and the RTC is lost or         */
      /* b) they send a message on the AC and acquisition is lost or      */
      /* c) they send a message on the AC and the session is closed.      */
      /* They can include processing in their outcome callback to re-send */
      /* the message if they so desire.                                   */
       
      HDR_MSG_PROT (MSG_LEGACY_MED,  "Connection closed or acquisition lost");
      {
        int i = 0;
        hdrslp_msg_struct_type* itr;
        for ( itr = hdrslp_tx_queue_first_item_ptr;
              itr != NULL;
              itr = (hdrslp_msg_struct_type*) (itr->next) ) i++;
        if ( i != hdrslp_tx_queue_item_count )
        {
          HDR_MSG_PROT_2 (MSG_LEGACY_ERROR,  "SLP TXQ: Count mismatch %d/%d",
                         i, hdrslp_tx_queue_item_count);
        }
      }
      if ((ind_name == HDRIND_ALMP_RETURNING_TO_IDLE) ||
          (ind_name == HDRIND_ALMP_REACQUIRING) ||
          (ind_name == HDRIND_SAP_RETURNING_TO_CLOSE) )
      {
        hdrslp_tx_queue_clear( E_CANCELED );
      }
      else
      {
        /* For session close or entering power save, consider
         * these as failures so RASM won't keep retrying them.
         */
        hdrslp_tx_queue_clear( E_FAILURE );
      }

      /* Flag was set to TRUE when ACMAC requested AC pkt from slp queue
       * Set it back to FALSE after receiving session closed ind.
       */
      hdrslp_access_attempted_since_entering_idle = FALSE;  

      hdrslp_reset();
      hdrpcp_reset_roll_cb();
      break;
      
    case HDRIND_CCMAC_INTERRUPTED:
      /* Flush brdcst reassembly buffer*/
      hdrslp_flush_reassembly_buf( 1 ); /* buf index: 0: unicast, 1: broadcast*/
      break;

    default:
      ERR( "Rcvd unknown indication %d", ind_name, 0, 0 );
  }

} /* hdrslp_process_ind() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_PROCESS_MSG

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to SLP.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_msg
(
  dsm_item_type* msg_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8  msg_buffer[HDRSLP_MAX_MSG_SIZE];
  uint8  msg_id;
  uint8  msg_len;
  uint8  msg_seqno;
  uint8  chan_bit = 0; /* 1 = Control Channel, 0 = Forward Traffic Channel */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG)); 

  /* MessageID is the first uint8 */
  /* b_unpackb (msg_ptr->data_ptr, 2); */
  if ( dsm_extract( msg_ptr, 0, &msg_id, 1 ) == 0 )
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &msg_ptr );
    return;
  }

  /* Currently only the in use instance receives any messages */
  if (protocol_instance != HDRHAI_IN_USE)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    return;
  }

  HDRTRACE_MSG( HDRHAI_SIGNALING_LINK_PROTOCOL, msg_id, 0 );

  msg_len = dsm_length_packet( msg_ptr );

  /* Get channel information */
  chan_bit = HDRDSM_GET_CHAN_BIT( msg_ptr );

  if ( msg_len > HDRSLP_MAX_MSG_SIZE )
  {
    ERR( "Rcvd msg too large (%d)", msg_len, 0, 0 );
    dsm_free_packet( &msg_ptr );
  }
  else
  {
    msg_len = dsm_pullup( &msg_ptr, msg_buffer, msg_len );

    switch ( msg_id )
    {
      case HDRSLP_RESET_MSG: 
        msg_seqno = msg_buffer[1];

        /* Continue to process reset msg only when it is rcvd on FTC. */
        if ( !chan_bit )
        {
          /* Compare reset sequence number in modulo 8 */
          if ( hdrutil_msg_seq_num_is_greater(msg_seqno, hdrslp_reset_seqno, 8) )
          {
            HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "RX SLP Reset (%d)", msg_seqno);
            
            /* Prevent the TX interrupt from pulling new signaling data */
            hdrslp_in_reset = TRUE;
    
            /* Reset all internal data */
            hdrslp_reset();
            hdrslp_reset_seqno = msg_seqno;
    
            /* Send ResetAck message */
            hdrslp_send_reset_ack( msg_seqno );
    
            hdrslp_in_reset = FALSE;          
          }  
          else
          {
            /* Got sequence number smaller than expected - it's a stale msg */
            HDR_MSG_PROT_2 (MSG_LEGACY_MED,  "RX stale Reset (rcv:%d - exp: %d)",
                           msg_seqno, hdrslp_reset_seqno);
            HDRTRACE_RXMSG_IGNORED_SEQ( HDRHAI_SIGNALING_LINK_PROTOCOL,
                                        HDRSLP_RESET_MSG, 0 );
          }
        }
        else
        {
          HDR_MSG_PROT ( MSG_LEGACY_ERROR, "Rcvd Reset msg on CC" );
        }
        break;

      default:
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Rcvd unknown msg id %d", 
                       msg_buffer[0]);
    }   
  }

} /* hdrslp_process_msg() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_RESET

DESCRIPTION
  This function performs an SLP reset

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  Rexmit, resequencing buffers get flushed, all sequence variables
  are reinitialized.
===========================================================================*/

void hdrslp_reset( void )
{
  int i;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  REX_DISABLE_PREMPTION();

  hdrslp_enter_crit_sect();

  /* Reset all receive related data */
  if ( hdrslp_reassembly_buffer[0].buffer_ptr ) 
  {
    dsm_free_packet( &(hdrslp_reassembly_buffer[0].buffer_ptr) );
  }
  if ( hdrslp_reassembly_buffer[1].buffer_ptr ) 
  {
    dsm_free_packet( &(hdrslp_reassembly_buffer[1].buffer_ptr) );
  }
  hdrslp_reassembly_buffer[0].length = 0;
  hdrslp_reassembly_buffer[1].length = 0;
  hdrslp_reassembly_buffer[0].frg_seqno = 255;
  hdrslp_reassembly_buffer[1].frg_seqno = 255;
  hdrslp_reassembly_buffer[0].last_frg= FALSE;
  hdrslp_reassembly_buffer[1].last_frg= FALSE;
  hdrslp_seqno_expected = 0;

  /* From 2000 to 2006, SLP was incorrectly initializing this value */
  /* to 7 instead of 0, which marks seqnos 0, 1 and 2 as received   */
  /* when they were not.                                            */
  hdrslp_seqno_received = 0;
  
  /* Reset all transmit related data except the TX queue */
  for ( i=0; i<4; i++ )
  {
    if ( hdrslp_rexmit_buffer[i] )
    {
      hdrslp_rexmit_buffer[i] = NULL;
    }
  }

  /* Check if any message is pending fragmentation. (For Ack's      */
  /* received during fragmentation the fragment buffer is pointing  */
  /* to an item in the callback queues until the next transmission. */
  /* Those are cleared out below when we handle the callbacks.)     */
  if ( hdrslp_fragment_buffer )
  {
    hdrslp_fragment_buffer = NULL;
  }

  hdrslp_tx_seqno = 0;
  hdrslp_rxb_level = 0;
  hdrslp_rxb_free_mask = 0x0f;
  hdrslp_rxb_timer_mask = 0x00;
  hdrslp_fragment_header = 0;
  hdrslp_fragment_pointer = 0;
  hdrslp_fragment_tx_count = 0;
  hdrslp_fragment_seqno = 0;
  hdrslp_rx_ack_buffer[0] = 0x00;
  hdrslp_rx_ack_buffer[1] = 0x00;
  hdrslp_rx_ack_buffer[2] = 0x00;
  hdrslp_rx_ack_buffer[3] = 0x00;
  hdrslp_rx_ack_timer_mask = 0x00;
  hdrslp_rx_ack_buffer_write_index = 0;
  hdrslp_rx_ack_buffer_read_index = 0;
  hdrslp_rx_ack_current_seqno = 7;
  hdrslp_cb_wait_q_ptr = NULL;
  hdrslp_cb_process_q_ptr = NULL;

  hdrslp_leave_crit_sect();

  REX_ENABLE_PREMPTION();

  /* Call all pending callbacks w/ a CANCELED result */
  for ( i=0; i<HDRSLP_MSG_POOL_SIZE; i++ )
  {
    if ( (hdrslp_msg_pool[i].queue == SLP_REXMIT_Q)
         ||
         (hdrslp_msg_pool[i].queue == SLP_FRG_Q)
         ||
         (hdrslp_msg_pool[i].queue == SLP_CB_WAIT_Q)
         ||
         (hdrslp_msg_pool[i].queue == SLP_CB_PROCESS_Q) )
    {
      (void) hdrslp_mp_free_msg( &hdrslp_msg_pool[i], E_CANCELED );
    }
  }

  HDR_MSG_PROT (MSG_LEGACY_HIGH,  "SLP Reset performed");

} /* hdrslp_reset() */

/*===========================================================================

FUNCTION HDRSLP_FLUSH_REASSEMBLY_BUF

DESCRIPTION
  This function flush reassembly buffer 

DEPENDENCIES
  None
  
PARAMETERS
  Index - Buffer index. [0] for unicast fragments and [1] for brdcst

RETURN VALUE
  None
  
SIDE EFFECTS
  reassembly buffer gets flushed
===========================================================================*/

void hdrslp_flush_reassembly_buf
(
  uint8 index
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
    hdrslp_enter_crit_sect();

  if ( hdrslp_reassembly_buffer[index].buffer_ptr != NULL ) 
  {
    dsm_free_packet( &(hdrslp_reassembly_buffer[index].buffer_ptr) );
    hdrslp_reassembly_buffer[index].length = 0;
    hdrslp_reassembly_buffer[index].frg_seqno = 255;
    hdrslp_reassembly_buffer[index].last_frg= FALSE;

    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "Buf %d: Dropped incomplete fragment", index );

    /* Update stats */
    hdrslp_stats.rx_frg_missing_cnt++;
  }

  hdrslp_leave_crit_sect();
	
} /* hdrslp_flush_reassembly_buf() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_SEND_RESET_ACK

DESCRIPTION
  This function sends a ResetAck message with the given sequence number.

DEPENDENCIES
  None
  
PARAMETERS
  seqno - Reset sequence number for this transaction

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_send_reset_ack
(
  uint8 seqno
)
{
  dsm_item_type* reset_ack_msg_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  reset_ack_msg_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
  if ( reset_ack_msg_ptr == NULL )
  {
    ERR_FATAL( "SLP: Out of dsm items", 0, 0, 0 );
  }

  /* Build the ResetAck message - HMP header, msg ID, and seq. number */
  reset_ack_msg_ptr->data_ptr[0] = HDRHAI_N_APP0_TYPE;
  reset_ack_msg_ptr->data_ptr[1] = HDRSLP_RESET_ACK_MSG;
  reset_ack_msg_ptr->data_ptr[2] = seqno;
  reset_ack_msg_ptr->used = 3;

  /* Send the message with highest priority */
  if ( hdrslp_send_msg( HDRHAI_RTC,
                        FALSE,
                        0,
                        reset_ack_msg_ptr,
                        NULL,
                        NULL )
       != E_SUCCESS )
  {
    ERR( "SLP: Failed to send ResetAck(%d)", seqno, 0, 0 );
  }
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_RESET_STATS

DESCRIPTION
  This function resets all of SLP's stat counters to zero

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_reset_stats( void )
{
  memset( &hdrslp_stats, 0, sizeof( hdrslp_stat_struct_type ) );
  hdrslp_stats.rx_min_msg_size = 0xFFFFFFFFUL;
  hdrslp_stats.tx_min_msg_size = 0xFFFFFFFFUL;

  HDR_MSG_PROT (MSG_LEGACY_HIGH,  "SLP Statistics reset");

} /* hdrslp_reset_stats() */

/* <EJECT> */   
/*===========================================================================

FUNCTION HDRSLP_IS_HDRIS890

DESCRIPTION
  This helper function determines if a message is an HDR IS-890 message.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id - 
  
RETURN VALUE
  FALSE - The msg type couldn't be determined or it is not an IS890 message.
  TRUE  - The msg is an IS890 message.
  
SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean hdrslp_is_hdris890
( 
  hdrhai_stream_enum_type stream_id
)
{
  hdrhai_app_subtype_enum_type stream_config_app[HDRSLP_MAX_STREAMS]; 
    /* Stream config look up table */

  boolean is_hdris890 = FALSE; /* If the stream is a test app */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrstream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_IS890) ||
      hdrstream_stream_is_assigned(HDRHAI_ENH_TEST_APP_IS890A) ||
      hdrstream_stream_is_assigned(HDRHAI_MC_TEST_APP_IS890B))

  {
    /* Some stream is for test app, but we don't know which one yet */
    is_hdris890 = TRUE;
  }
  
  if (is_hdris890)
  {
    /* Now see if the actual stream being asked about is for test app */
    is_hdris890 = FALSE; /* May as well re-use the same flag... */

    if (hdrstream_get_stream_config (&stream_config_app[0]))
    {
      if ((stream_config_app[stream_id] == HDRHAI_DEF_PACKET_APP_IS890) ||
          (stream_config_app[stream_id] == HDRHAI_ENH_TEST_APP_IS890A) ||
          (stream_config_app[stream_id] == HDRHAI_MC_TEST_APP_IS890B))
      {
        /* Now we are sure the stream asked about is for test app */
        is_hdris890 = TRUE;
      }
    }
    else
    {
      /* No config available, which means the stream is not test app */
      ERR("Stream Configuration for App Subtypes not negotiated",0,0,0); 
    }
  }
  
  return is_hdris890;

} /* hdrslp_is_hdris890() */

/*===========================================================================

FUNCTION HDRSLP_GET_APP_TYPE

DESCRIPTION
  This helper function determines which application subtype was negotiated
  for a particular stream.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id - Stream ID 
  
RETURN VALUE
  app_type - Application Type for stream_id (returns DEFAULT if IS890)
    
SIDE EFFECTS
  None
===========================================================================*/
LOCAL hdrlog_hdr_app_enum_type hdrslp_get_app_type 
(
  hdrhai_stream_enum_type stream_id
)
{

  hdrlog_hdr_app_enum_type app_type = HDRLOG_APP_DEFAULT;
    /* Application Subtype */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  switch (hdrstream_get_apptype_for_neg_stream(stream_id))
  {
    case HDRHAI_DEF_PACKET_APP_AN:
    case HDRHAI_DEF_PACKET_APP_SN: 
      app_type = HDRLOG_APP_DEFAULT;
      break;

#ifdef FEATURE_HDR_QOS
    case HDRHAI_MULTIFLOW_PACKET_APP_AN:
    case HDRHAI_MULTIFLOW_PACKET_APP_SN:
      app_type = HDRLOG_APP_MULTIFLOW;
    break;

    case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN:
    case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
      app_type = HDRLOG_APP_ENH_MULTIFLOW;
      break;

  case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN:
  case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
    app_type = HDRLOG_APP_MULTILINK_MULTIFLOW;
    break;
#endif /* FEATURE_HDR_QOS */

    case HDRHAI_3G1X_CSN_APP:
      app_type = HDRLOG_APP_CSNA;
      break;

    default:
      app_type = HDRLOG_APP_DEFAULT;
      break;
  }
 
  return app_type;

} /* hdrslp_get_app_type() */

/* <EJECT> */   
/*===========================================================================

FUNCTION HDRSLP_DETERMINE_SUBTYPE

DESCRIPTION
  This helper function determines the protocol subtype of the message being 
  logged.  This is accomplished by extracting the ProtocolType from the 
  message and asking the upper layers what the current subtype is for the
  given ProtocolType.  If the ProtocolType is a stream, then the subtype is
  really the application subtype which is determined by seeing what 
  application is bound to the stream.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr     - DSM item holding the message to be logged
  sdu_offset  - (AKA SLP header size) Offset of first byte of SDU (protocol ID)
  stream_id   - Returns the Stream ID from the Protocol ID

RETURN VALUE
  protocol_subtype/application_subtype of message to be logged.

SIDE EFFECTS
  None
===========================================================================*/
static uint16 hdrslp_determine_subtype
( 
  dsm_item_type            *msg_ptr,
  uint8                    sdu_offset,
  boolean                  *is_is980_ptr,
  hdrhai_channel_enum_type channel
)
{
  uint8 protocol_id;  
    /* Protocol id of received msg  */
  boolean is_apps_pkt = TRUE;
    /* Flag indicating if the msg is an application message */
  hdrhai_protocol_instance_enum_type prot_inst;

  hdrhai_stream_enum_type  stream_id = HDRHAI_STREAM0;
  uint16                   protocol_subtype;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the protocol ID from the msg_ptr */
  if ( dsm_extract( msg_ptr, sdu_offset, (void*)&protocol_id, 1) == 0 )
  {
    ERR("Could not extract the protocol id",0,0,0);
    return 0;
  }

  /* Bit 8 = InConfiguration Instance, Bit 0-7 = protocol_id */
  prot_inst = ((protocol_id & 0x80) == 0) ? HDRHAI_IN_USE : HDRHAI_IN_CONFIG;
  protocol_id &= 0x7F;

  /* Get the stream ID (if applicable) from the protocol ID */
  switch ( (hdrhai_protocol_type_enum_type)protocol_id )
  {
    case HDRHAI_N_APP1_TYPE:
      stream_id = HDRHAI_STREAM1;
      break;
    case HDRHAI_N_APP2_TYPE:
      stream_id = HDRHAI_STREAM2;
      break;
    case HDRHAI_N_APP3_TYPE:
      stream_id = HDRHAI_STREAM3;
      break;
    default:
      is_apps_pkt = FALSE;
      break;
  }
  
  if (is_apps_pkt)
  {
    *is_is980_ptr = hdrslp_is_hdris890(stream_id);
    protocol_subtype = (uint8) hdrslp_get_app_type(stream_id);
  }
  else
  {
    *is_is980_ptr = FALSE;
    protocol_subtype = hdrscp_get_logging_subtype(
      (hdrhai_protocol_name_enum_type) protocol_id,
      prot_inst, channel );
  }

  return protocol_subtype;

} /* hdrslp_determine_subtype() */

/* <EJECT> */   
/*===========================================================================

FUNCTION HDRSLP_LOG_MSG

DESCRIPTION
  This function logs a signaling message, either receive or transmit.

DEPENDENCIES
  None
  
PARAMETERS
  link_id - Link ID of the link message received/transmitted on
  sdu_length - Including HMP header but NOT SLP header
  sdu_offset - (AKA SLP header size) Offset of first byte of SDU (protocol ID)
  is_broadcast - If the message is a received broadcast message
  channel - channel message was received/transmitted on
  reliable - If the transmission is reliable or best effort
  fragmented - If this SDU is fragmented
  seq_no - For reliable delivery
  ack_seq_no - For reliable delivery
  ack_seq_valid - Whether the ack seq no given is a valid seq no
  msg_ptr - DSM item whose data INCLUDES the SLP header

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_log_msg
(
  hdrsrch_link_id_type link_id,
  uint16         sdu_length,
  uint8          sdu_offset,                                             
  boolean        is_broadcast,
  hdrhai_channel_enum_type channel,
  boolean        scc_msg,
  boolean        reliable,
  boolean        fragmented,
  uint8          seq_no,
  uint8          ack_seq_no,
  boolean        ack_seq_no_valid,
  dsm_item_type *msg_ptr
)
{
#ifndef TARGET_OS_SOLARIS
  /* Log messages buffers for all channels have the same structure. */
  log_hdr_msg_type            *log_ptr;
  sys_link_type               link;           /* Buffer to store link info  */
  hdrdec_half_slot_time_type  timestamp;         /* Timestamp dest address  */
  log_code_type               code = LOG_HDR_BASE_C;    /* Log code to use  */
  boolean                     is_is890 = TRUE; 
    /* Is this an IS-890 msg to log? */
  sys_channel_type slp_carrier; 
    /* Channel record of the carrier designated to carry the SLP packets */
  int8 dummy;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr == NULL)
  {
    /* msg_ptr should at least include the SLP header */
    ERR("About to transmit msg without a header!",0,0,0);
    return;
  }

  switch (channel)
  {
    case HDRHAI_AC:
      code = LOG_HDR_SIG_AC_MSG_C;
      break;

    case HDRHAI_CC:
      if (is_broadcast)
      {
        code = LOG_HDR_SIG_CC_BCAST_MSG_C;
      }
      else
      {
        code = LOG_HDR_SIG_CC_DIR_MSG_C;
      }
      break;

    case HDRHAI_RTC:
      code = LOG_HDR_SIG_RTC_MSG_C;
      break;

    case HDRHAI_FTC:
      code = LOG_HDR_SIG_FTC_MSG_C;
      break;

    default:
      ERR_FATAL(" TX logging message for unknown channel.",0,0,0);
      break;
  }

  if( !log_status( code ) )
  {
    /* Log mask is not set to log this packet */
    return;
  }

  ASSERT (code != LOG_HDR_BASE_C);

  /* When allocating a log packet, the length passed in must   */
  /* include space for the log header that is added by the     */
  /* log routines.                                             */
  if ( (log_ptr = (log_hdr_msg_type *) log_alloc_ex( code, 
      sdu_length + HDRLOG_MSG_BUF_FIXED_SIZE + sizeof(log_hdr_type))) == NULL)
  {
    /* Logging is not enabled for this type of packet. */
    return;
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Don't get link info in tunnel mode since HDR lower layer is inactive. */
  if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_SIGNALING_LINK_PROTOCOL) )
  {
    memset(&link, 0, sizeof(sys_link_type));
  }
  else 
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdrlmac_get_link_info (link_id, &link);
  }

  if (hdrcmac_get_sync_caps_timestamp (HDRDSM_GET_TIMESTAMP(msg_ptr),
                                       &timestamp) != E_SUCCESS)
  {
    /* The timestamp might not be valid yet if we just woke up */
    timestamp = 0;
  }

  if ( ( channel == HDRHAI_RTC ) &&
       ( hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL ) 
           == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
         ||
         hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
           == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */
       )
     )
  {
#ifdef FEATURE_HDR_REVB_SUPPORTED
    hdrrmac4_get_slp_carrier_and_tx_pilot_pwr_info(&slp_carrier, 
                                                   &dummy);
#else
    dummy = 0;
    slp_carrier.band = 0;
    slp_carrier.chan_num = 0;
#endif /* FEATURE_HDR_REVB_SUPPORTED */
    log_ptr->band = (uint8) slp_carrier.band;
    log_ptr->chan_num = slp_carrier.chan_num;
  }
  else
  {
    log_ptr->band = (uint8) link.channel.band;
    log_ptr->chan_num = link.channel.chan_num;
  }
  log_ptr->pilot_pn = link.pilot_pn;
  log_ptr->hstr = timestamp;
  log_ptr->flags.reliable = reliable;
  log_ptr->flags.fragmented = fragmented;
  log_ptr->flags.ack_seq_no_valid = ack_seq_no_valid;
  log_ptr->flags.seq_no_valid = reliable; /* Always same as reliable */
  
  /* Extract the protocol subtype/application subtype */
  if (sdu_length != 0)
  {
    log_ptr->flags.app_type =
      hdrslp_determine_subtype( msg_ptr, sdu_offset, &is_is890, channel);
    log_ptr->flags.is_hdris890 = is_is890;
  }
  else
  {
    log_ptr->flags.is_hdris890 = FALSE;
    log_ptr->flags.app_type    = HDRLOG_APP_DEFAULT;
  }

  /* Determine the app type for this message*/
  log_ptr->seq_no = seq_no;
  log_ptr->ack_seq_no = ack_seq_no;
  log_ptr->scc_flag = scc_msg;

  /* Don't log the SLP header */
  if ( dsm_extract( msg_ptr, sdu_offset, 
       (void *) log_ptr->msg, sdu_length ) != sdu_length)
  {
    ERR("Could not extract message for logging",0,0,0);
  }


  log_commit( (log_type *) log_ptr ); 
#endif /* TARGET_OS_SOLARIS */
} /* hdrslp_log_msg() */

/* <EJECT> */   
/*=========================================================================

FUNCTION HDRSLP_LOG_CONN_STATE_STATS

DESCRIPTION  
  This function logs the connected state stats. It gets a 
  log record from the diag log buffer, fills it with the latest 
  connected state info and commits the log record to
  the diag for delivery.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/
void hdrslp_log_conn_state_stats( void )
{
  LOG_HDR_CONN_STATE_TX_STATS_C_type *log_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  log_ptr = (LOG_HDR_CONN_STATE_TX_STATS_C_type *)
            log_alloc( (log_code_type) LOG_HDR_CONN_STATE_TX_STATS_C,
                       sizeof( LOG_HDR_CONN_STATE_TX_STATS_C_type ) );
    /* Actually log the packet if the log mask is set properly. */
 
  if ( log_ptr )
  {
    REX_DISABLE_PREMPTION();

#ifdef FEATURE_MEMCPY_REMOVAL
    log_ptr->conn_state_tx_stats  = hdrslp_conn_state_stats;
#else
    memcpy( (void *)( &log_ptr->conn_state_tx_stats ),
            (void *)( &hdrslp_conn_state_stats ),
            sizeof( hdrslp_log_conn_state_stats_struct_subtype ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      /* Copy log info into the log record */

    REX_ENABLE_PREMPTION();

    log_commit( (log_type *) log_ptr );
      /* Commit the log packet to diag. */
  }
} /* hdrslp_log_conn_state_stats() */

/* <EJECT> */   
/*=========================================================================

FUNCTION HDRSLP_LOG_INC_CONN_STATE_STATS_COUNTER

DESCRIPTION
  This function increments the appropriate connected
  state counter by the given amount.

DEPENDENCIES
  None
  
PARAMETERS
  counter - The counter to increment.
  inc_count - How much to increment the counter by.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/
void hdrslp_log_inc_conn_state_stats_counter
( 
  hdrslp_log_conn_state_stats_count_type counter,
  uint16                                 inc_count
)
{
  REX_DISABLE_PREMPTION();

  hdrslp_conn_state_stats.since_reset_counts[counter] += inc_count;
    /* Increment the specified counter kept since the last AT reset. */

  hdrslp_conn_state_stats.since_call_counts[counter] += inc_count;
    /* Increment the specified counter kept since the beginning of
       the last call. */

  REX_ENABLE_PREMPTION();
} /* hdrslp_log_inc_conn_state_stats_counter() */

/* <EJECT> */   
/*=========================================================================

FUNCTION HDRSLP_LOG_RESET_CONN_STATE_STATS

DESCRIPTION 
  This function resets the SLP connected state stats as
  specified by the stats reset parameter.

DEPENDENCIES
  None
  
PARAMETERS
  stats_reset - Which statistics to clear... all or just the ones
                related to the last call.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/
void hdrslp_log_reset_conn_state_stats
( 
  hdrslp_reset_stats_type stats_reset
)
{
  REX_DISABLE_PREMPTION();

  switch ( stats_reset )
  {
    case HDRSLP_ALL_STATS:

      memset( ( void* ) &hdrslp_conn_state_stats, 0, 
              sizeof( hdrslp_log_conn_state_stats_struct_subtype ) );
        /* Reset all of the statistics tracked for this log packet. */

      hdrslp_conn_state_stats.version = HDRSLP_CONN_STATE_STATS_VER;
        /* Set the version number of the packet for future expandability. */

      break;

    case HDRSLP_LAST_CALL_STATS:

      memset( ( void* ) hdrslp_conn_state_stats.since_call_counts, 0, 
              HDRSLP_NUM_CONN_STATE_STATS_COUNTS * sizeof( uint32 ) );
        /* Reset only the statistics tracked since the start of the
           last call. */

      break;

    default:
      ERR("Unknown statistics to reset: %d", stats_reset, 0, 0 );
      break;

  }

  REX_ENABLE_PREMPTION();
} /* hdrslp_log_reset_conn_state_stats() */

