/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            H D R   S I G N A L I N G   L A Y E R   P R O T O C O L

GENERAL DESCRIPTION
  TX functionality of HDR SLP, management of transmit queue and
  the SLP message pool.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrslptx.c_v   1.34   05 Mar 2003 14:18:02   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrslptx.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/18/15   wsh     Fixed race cond. between sending msg and clearing SLP Q
02/25/15   sjj     Fixed KlockWork issues in the hdrslp_tx_queue_clear()
02/13/15   vko     Fixed race condition between hdrslp_mp_init_msg() and hdrslp_mp_get_msg().
10/11/13   vko     Fixed not sending Alt link close request due to race cond in hdrsaptx
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
12/04/13   sat     Removed unused parameters in F3s.
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
08/09/13   cnx     Fix race condition in hdrslp_abort_msg().
06/13/13   ukl     Added gaurding critical section for hdrslp_cb_process_q
03/29/13   cnx     Added critical section to guard DSM item free.
01/24/13   wsh     Added crit_sect to guard SLP data structures
09/26/12   wsh     Fixed assert due to xmit RTC msg on AC channel
03/16/12   arm     Resolve TCB errors for Dime. 
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
10/10/11   cnx     Removed 'SJU' in F3.
08/15/11   wsh     Freeing SLPTX queue head when freeing DoS msg due to size
07/27/11   cnx     Merged Optimized Handoff changes.
04/09/11   wsh     Replaced ASSERT with if to avoid crash when DSM out of mem
06/21/10   cnx     Fixed compiler warnings.
10/27/09   pba     Klockwork fixes.
08/05/09   pba     Cancel sending DOS msg if it can not fit in the 1st probe. 
03/17/09   sju     Added F3 message.
12/17/08   pba     Updated F3 messages.
12/10/08   smd     Replaced assert.h with amssassert.h
11/22/08   kss     Removed/replaced intlocks.
11/21/08   pba     Added events for SLPQH feature.
04/17/08   sju     Added support to bundle AC messages 
04/03/08   wsh     Merged fix for waiting UATIAssign onto 2H07
07/30/07   hal     Use hdrutil_pushdown() to circumvent DSM issue
06/20/07   hal     Reverted DSM change to prevent crashing due to direct DSM data
                   pointer manipulation in legacy code
05/04/07   sju     Check for correctness of destination channel on advanced 
                   best-effort message
03/22/07   hal     Updated hdrslp_fragment_sdu() to pushdown one extra byte when
                   creating the header instead of poking around in the DSM item
12/06/06   sju     When RMAC3 is used, flush slp cb for be msg every 4 slots 
                   instead of 16 slots. 
11/15/06   yll     Removed DSM tracer field to save memory.
09/05/06   sju     Fixed instead of enqueuing NULL msg to callback processing 
                   queue, enqueuing it to callback waiting queue.
07/27/06   mpa     Check for NULL pointer in hdrslp_find_msg().
06/07/06   dna     Copy channel/ID into subsequent fragments (for logging).
04/07/06   hal     Changed dsm_pushdown() to use hdrutil_pushdown_head()
12/06/05   etv     Added function to Flush all pending SLP Acks.
11/28/05   yll     Fixed a fragmentation problem for messages over 255 bytes.
06/22/05   pba     No fragmentation of DOS message on access channel.
04/28/05   dna     Fixed transmit seqno to not exceed the window.
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
11/29/04   dna     Avoid getting stuck in connection setup state.
                   hdrslp_update_tx_timers needs to take a uint16, otherwise
                   an input with lowest 8 bits of zero won't increment timers!
11/29/04   grl     Added connected state stat logging support.
11/24/03   dna     Prevent sending an SLP ACK on the AC.
10/28/03   mpa     SLP TX queue should respect FIFO order for messages of 
                   same priority.
10/08/03   mpa     Fixed SLP TX queue insertion algorithm.
08/27/03   dna     Clean up and organize the errno values returned in message
                   callbacks. See description of hdrslp_mp_free_msg().
03/05/03   dna     Allow E_FAILURE to be reported for failed reliable RTC msgs
09/16/02   dna     Fixed infinite RUP registration problem
08/28/02   om      Fixed SLP reset during fragmentation.
07/25/02   om      Added resyncing of Ack buffer and timer mask.
06/19/02   om      Added ASSERT for manipulating DSM items directly.
06/11/02   om      Fixed clearing of slp tx queue to allow callbacks to re-
                   enqueue new msgs, fixed moving msgs from wait to prcs q.
05/12/02   om      Fixed verification of channel (cb-enqueuing)
05/09/02   om      Fixed advancing of best-effort msgs in tx queue
05/02/02   om      Fixed message level
05/01/02   om      Fixed stale msgs in rexmit buffer (NULL frg error)
04/24/02   om      Added TASKLOCK's to slp_process_ack()
04/22/02   om      Fixed freeing of NULL sdu in frg_msg()
04/11/02   dna     Log fragmented messages correctly.
03/05/02   om      Fixed piggyback ACKs for best-effort messages
01/14/01   dna     Call the callbacks with the appropriate reason instead of
                   always calling them with E_CANCELLED when the TX queue is 
                   cleared.
11/19/01   dna     Log for Ack only packet needs correct header info, too.
11/01/01   dna     Log for Ack only packet is zero length SDU.
09/25/01   om      Removed FEATURE_HDR_HAI_X4
08/14/01   om      Fixed moving of msgs from CB wait to process queue
08/10/01   om      Fixed clearing of tx msg queue
08/01/01   dna     Added logging of ack sequence number valid bit of SLP 
                   header as part of the signaling message log packet
07/10/01   dna     Set tx_queue_item_count to zero in hdrslp_tx_queue_clear()
05/01/01   dna     Minor changes to compile IS-856 related changes
01/30/01   dna     Account for best effort message in CB_WAIT_Q when checking
                   for ack to reliable messages on their last re-try.
02/22/00   om      Created Module


===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "amssassert.h"

#include "comdef.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "memory.h"
#include "qw.h"
#include "task.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#include "hdrerrno.h"
#include "hdrhai.h"
#include "hdrdsm.h"
#include "hdrsrch.h"
#include "hdrtx.h"
#include "hdrutil.h"

#include "hdrlogi.h"
#include "hdrslp.h"
#include "hdrslpi.h"
#include "hdrslptx.h"
#include "hdrtrace.h"

#define LA_TRACE( val ) \
  outpw( 0x2800000, 0x0000 ); \
  outpw( 0x2800000, (uint16) val )

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*** SLP message buffer pool ***/
hdrslp_msg_struct_type   hdrslp_msg_pool[HDRSLP_MSG_POOL_SIZE];
uint16                   hdrslp_msg_pool_level = 0;

/*** SLP transmit queue ***/
hdrslp_msg_struct_type*  hdrslp_tx_queue_first_item_ptr = NULL;
uint16                   hdrslp_tx_queue_item_count = 0;

/*** SLP reliable transport sequence number ***/
uint8                    hdrslp_tx_seqno = 0;

/*** SLP rexmit buffer and managing data ***/
hdrslp_msg_struct_type*  hdrslp_rexmit_buffer[4];
uint8                    hdrslp_rxb_level = 0;
uint8                    hdrslp_rxb_free_mask = 0x0f;
uint8                    hdrslp_rxb_timer_mask = 0x00;
uint8                    hdrslp_rxb_lookup[16] = 
                           { 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };

/*** SLP fragmentation buffer */
hdrslp_msg_struct_type*  hdrslp_fragment_buffer;
uint16                   hdrslp_fragment_header = 0;
uint16                   hdrslp_fragment_pointer = 0;
uint16                   hdrslp_fragment_tx_count = 0;
uint8                    hdrslp_fragment_seqno = 0;

/*** SLP RX Ack buffer and timers ***/
uint8                    hdrslp_rx_ack_buffer[4] = { 0x00, 0x00, 0x00, 0x00 };
uint8                    hdrslp_rx_ack_timer_mask = 0x00;
uint8                    hdrslp_rx_ack_buffer_write_index = 0;
uint8                    hdrslp_rx_ack_buffer_read_index = 0;
uint8                    hdrslp_rx_ack_current_seqno = 7;
hdrslp_msg_struct_type   hdrslp_rx_ack_only_msg;

/*** SLP Callback Function list ***/
hdrslp_msg_struct_type*  hdrslp_cb_wait_q_ptr = NULL;
hdrslp_msg_struct_type*  hdrslp_cb_process_q_ptr = NULL;

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
/* This flag is used to hold off giving messages to ACMAC for transmission
   between the time we send a UATIRequest and when we receivce a
   UATIAssignment, for infrastructure than can not accommodate other
   messages during this window of time. */
boolean hdrslp_waiting_for_uati_assignment;
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

/* This flag is used to hold off giving messages to ACMAC for transmission
   when we are still waiting for messages to be bundled in the same access
   probe */
boolean hdrslp_waiting_for_bundling = FALSE;

rex_crit_sect_type hdrslptx_crit_sect_dsmfree;
rex_crit_sect_type hdrslptx_crit_sect_cbpq;
rex_crit_sect_type hdrslptx_crit_sect_mpfree;

void hdrslptx_enter_crit_sect( rex_crit_sect_type * );
void hdrslptx_leave_crit_sect( rex_crit_sect_type * );


/* <EJECT> */ 
/*--------------------------------------------------------------------------
-----------     Transmission and Acknowledgement Functions       -----------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_UPDATE_TIMERS

DESCRIPTION
  This function updates the frame-count based timers for transmission
  with the number of 26.66... ms interrupts since the last build signal.

DEPENDENCIES
  RTC Mac to keep track of the number of 26.66... interrupts between
  build signls.
  
PARAMETERS
  num_tx_ints - Number of TX interrupts since we were signaled the last time

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_update_tx_timers( uint16 num_tx_ints )
{
  uint32 i;
  hdrslp_msg_struct_type* itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (hdrslp_verbose_debug)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "=TX=  udpate timer %d", num_tx_ints );
  }

  hdrslp_enter_crit_sect();

  for ( i=0; i<4; i++ ) 
  {
    if ( ! (hdrslp_rxb_free_mask & (0x01 << i)) )
    {
      if (hdrslp_verbose_debug)
      {
        HDR_MSG_PROT_3( MSG_LEGACY_HIGH,  "=TX=  udpate rexmit timer %d,  %d, mask %d", 
                        hdrslp_rexmit_buffer[i]->rexmit_time,
                        num_tx_ints,
                        hdrslp_rxb_timer_mask);
      }
      /* Update rexmit timers */
      if ( hdrslp_rexmit_buffer[i]->rexmit_time > num_tx_ints )
      {
        hdrslp_rexmit_buffer[i]->rexmit_time -= num_tx_ints;
      }
      else
      {
        hdrslp_rxb_timer_mask |= (0x01 << i);
        
        /* Also record how often we expire a timer that is already    */
        /* expired, i.e., we didn't get to send a rexmission in time. */
      }
    }

    /* Update rx ack timers */
    if ( hdrslp_rx_ack_buffer[i] & HDRSLP_RX_ACK_BUFFER_USED_MASK )
    {
      if ( HDRSLP_GET_RX_ACK_TIMER( i ) > num_tx_ints )
      {
        HDRSLP_DECR_RX_ACK_TIMER( i, num_tx_ints );
      }
      else
      {
        hdrslp_rx_ack_timer_mask |= (0x01 << i);
        
        HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "SLP: Ack Timer %d expired for %d", 
                        i, hdrslp_rx_ack_buffer[i] & 0x0007);
        /* Also record how often we expire a timer that is already    */
        /* expired, i.e., we didn't get to send a rexmission in time. */
      }
    }
  }

  /* Update cb list timers */
  if ( hdrslp_cb_wait_q_ptr )
  {
    itr = hdrslp_cb_wait_q_ptr; 
    while ( itr != NULL )
    {
      if ( itr == itr->next ) 
      {
        itr->next = NULL;
        HDR_MSG_PROT( MSG_LEGACY_HIGH,  "CB q linked to itself !" );
      }
      if ( itr->next && (itr == ((hdrslp_msg_struct_type*) itr->next)->next) )
      {
        ((hdrslp_msg_struct_type*) itr->next)->next = NULL;
        HDR_MSG_PROT( MSG_LEGACY_HIGH,  "CB q linked to itself @ 2nd item !" );
      }

      if ( itr->cb_time > num_tx_ints )
      {
        if (hdrslp_verbose_debug)
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH,  "=TX=  udpate slp cb time" );
        }
        /* update time */
        itr->cb_time -= num_tx_ints;
        itr = itr->next;
      }
      else
      {
        /* Check if the fragmentation buffer is pointing to this msg */
        if ( hdrslp_fragment_buffer == itr )
        {
          /* Reset the fragment buffer */
          hdrslp_fragment_buffer  = NULL;
          hdrslp_fragment_header  = 0;
          hdrslp_fragment_pointer = 0;
        }

        /* move msg item to process queue */
        /* The tx_status is already set from       */
        /* when the item was put on the wait queue */
        if (hdrslp_verbose_debug)
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH, "=TX=  move to process queue" );
        }
        itr = hdrslp_cb_enqueue( itr, 0, itr->tx_status );
      }
    }
  }

  hdrslp_leave_crit_sect();
} /* hdrslp_update_tx_timers() */

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

void hdrslp_be_msg_tx_complete(void)
{
  hdrslp_msg_struct_type* itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrslp_cb_wait_q_ptr == NULL)
  {
    return;
  }

  /* All be msgs in cb_wait_q with cb_time == 1 have been transmitted */
  itr = hdrslp_cb_wait_q_ptr; 

  while ( itr != NULL )
  {
    if (itr->reliable || itr->cb_time > 1)
    {
      /* If it's a reliable message in the cb_wait_q, do nothing
       * hdrslp_update_tx_timers() will update its cb timer
       */

      /* If cb_time > 1, it means that we want msg to wait for specific time 
       * before calling cb. Don't flush cb now, update_tx_timers() will flush cb.
       * currently, there is no be msg with cb_timer > 1
       */
      itr = itr->next;
    }
    else
    {
      /* move msg item to process queue 
       * The tx_status is already set when the item was put on the wait queue 
       */
      itr = hdrslp_cb_enqueue( itr, 0, itr->tx_status );
    }
  }

} /* hdrslp_be_msg_tx_complete() */

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

void hdrslp_flush_pending_acks(void)
{
  uint32 i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_DISABLE_PREMPTION();

  hdrslp_enter_crit_sect();

  for ( i=0; i<4; i++ ) 
  {
    if ( hdrslp_rx_ack_buffer[i] & HDRSLP_RX_ACK_BUFFER_USED_MASK )
    {
      hdrslp_rx_ack_timer_mask |= (0x01 << i);

      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "SLP: Ack Timer %d flushed for %d", 
                      i, hdrslp_rx_ack_buffer[i] & 0x0007);
      /* Also record how often we expire a timer that is already    */
      /* expired, i.e., we didn't get to send a rexmission in time. */
    }
  }

  hdrslp_leave_crit_sect();

  REX_ENABLE_PREMPTION();

} /* hdrslp_flush_pending_acks */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_BUILD_TX_DATA

DESCRIPTION
  This function creates a signaling layer packet given the maximum
  size of the next transmitted frame.

DEPENDENCIES
  None
  
PARAMETERS
  tx_sdu - Pointer to the meta data needed by PCP
  max_bytes - maximum frame size the signaling layer packet has to fit in

RETURN VALUE
  Length of signaling layer packet in bytes
  
SIDE EFFECTS
  None
===========================================================================*/

uint8 hdrslp_build_tx_data
(
  boolean first_callfor_slpdata,
  hdrslp_sdu_struct_type* tx_sdu_ptr, 
  uint8 max_bytes
)
{
  uint8                              num_tx_bytes;
  uint32                             i;
  hdrslp_msg_struct_type*            current_msg_ptr;
  hdrslp_msg_struct_type*            cb_wait_msg_ptr;  /* msg on cb_wait_q */
  uint8                         oldest_seqno = 0; /* Smallest rexmit seqno */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++  S L P - F  +++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

  /* Check for any pending fragments */
  if ( hdrslp_fragment_buffer )
  {
    if ( hdrslp_fragment_buffer->queue == SLP_FREE_Q )
    {
      /* The last retransmission was fragmented, and we didn't get */
      /* to send out all fragments - discard the contents.         */
      HDR_MSG_PROT( MSG_LEGACY_MED,  "SLPF: Discarding remaining fragments" );
      hdrslp_fragment_buffer   = NULL;
      hdrslp_fragment_header   = 0;
      hdrslp_fragment_pointer  = 0;      
    }
    else
    {
      /* Check if we even fit the minimum (2 byte header + payload) in */
      if ( max_bytes <= 2 ) return 0;
      
      /* We need to send remaining fragments before anything else */
      return hdrslp_fragment_sdu( tx_sdu_ptr, 
                                  hdrslp_fragment_buffer,
                                  max_bytes );
    }
  }

  hdrslp_enter_crit_sect();

  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++  S L P - D  +++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
  
  /* Sanity check that this is not a free msg item */
  if ( hdrslp_rxb_timer_mask )
  {
    for ( i=0; i<4; i++ )
    {
      if ( hdrslp_rexmit_buffer[i] != NULL )
      {
        if ( hdrslp_rexmit_buffer[i]->queue != SLP_REXMIT_Q )
        {
          HDR_MSG_PROT_1( MSG_LEGACY_MED, "SLPF: Emptying rexmit[%d]", i );

          /* Clear out rexmit buffer entry */
          hdrslp_rexmit_buffer[i] = NULL;
          hdrslp_rxb_free_mask |= (0x01 << i);
          if ( hdrslp_rxb_level ) hdrslp_rxb_level--;

          /* Clear timer bit */
          hdrslp_rxb_timer_mask &= ~(uint8)(0x01 << i);
        }
      }
    }
  }
  
  /* Check for any pending retransmissions */
  if ( hdrslp_rxb_timer_mask )
  {
    /* Check if we even fit the minimum (5 byte header + payload) in */
    if ( max_bytes <= 5 ) 
    {
      hdrslp_leave_crit_sect();
      return 0;
    }

    ASSERT ( hdrslp_rxb_timer_mask < 16 );

    /* Figure index of rexmit buffer */
    i = hdrslp_rxb_lookup[hdrslp_rxb_timer_mask];

    /* Check which msg has expired and give it to PCP */
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "SLPF: TX rexmit buffer %d (0x%x)", 
                    i, hdrslp_rexmit_buffer[i] );

    if ( hdrslp_rexmit_buffer[i]->sdu.channel ==
         tx_sdu_ptr->channel )
    {
      /* if rexmit msg is for the right channel, send it */
      num_tx_bytes = hdrslp_fragment_sdu( tx_sdu_ptr,
                                          hdrslp_rexmit_buffer[i],
                                          max_bytes );

      /* If something went wrong we need to reset the rexmit buffer entry */
      if ( num_tx_bytes == 0 )
      {
        hdrslp_rexmit_buffer[i] = NULL;
        hdrslp_rxb_free_mask |= (0x01 << i);
        if ( hdrslp_rxb_level ) hdrslp_rxb_level--;
      }
    }
    else
    {
      /* if rexmit msg is now on the wrong channel, treat it as
         if it's been retried MAX times */
      num_tx_bytes = 0;
      hdrslp_rexmit_buffer[i]->rexmit_count = HDRSLP_REXMIT_COUNT;

      HDR_MSG_PROT_3(MSG_LEGACY_MED, "chan %d!=%d, drop rexmit buf %d",
                     hdrslp_rexmit_buffer[i]->sdu.channel,
                     tx_sdu_ptr->channel,
                     i);
    }

    /* Check if this is the last retransmission for this SDU */
    if ( ++(hdrslp_rexmit_buffer[i]->rexmit_count) >= HDRSLP_REXMIT_COUNT )
    {
      /* SDU won't be kept in rexmit buffer anymore, but _don't_     */
      /* free the msg chain yet - fragmented ones need to be kept    */
      /* around and will be freed when their callback is processed,  */
      /* so just add msg to the cb waiting list.                     */

      (void) hdrslp_cb_enqueue( hdrslp_rexmit_buffer[i], 
                                HDRSLP_REXMIT_TIME, E_FAILURE );

      /* Mark this item in the rexmit buffer as unused  and       */
      /* decrease the usage count.                                */
#ifdef HDR_ON_REXNT
      printf( "slp: rxb - clearing item #%d after %d rexmits, level = %d.\n",
              i,
              hdrslp_rexmit_buffer[i]->rexmit_count,
              hdrslp_rxb_level - 1 );
#endif
      hdrslp_rexmit_buffer[i] = NULL;
      hdrslp_rxb_free_mask |= (0x01 << i);
      hdrslp_rxb_level--;
    }
    else
    {
      /* SDU needs to stay in rexmit buffer since we didn't send */
      /* max attempts yet - reset the timer to initial value.    */
      hdrslp_rexmit_buffer[i]->rexmit_time = HDRSLP_REXMIT_TIME;
    }

    /* Clear timer bit */
    hdrslp_rxb_timer_mask &= ~(uint8)(0x01 << i);

    /* Update stats */
    hdrslp_stats.tx_rexmit_msg_cnt++;

    hdrslp_leave_crit_sect();

    return num_tx_bytes;
  }

  /* Check for correctness of destination channel */
  hdrslp_verify_dest_channel( tx_sdu_ptr->channel );

  /* If we have more than one Acks pending, Hold off sending the signaling 
     message and fall thru to send header-only ACKs */
  if ((hdrutil_bit_count16(hdrslp_rx_ack_timer_mask) > 1) &&
      (tx_sdu_ptr->channel == HDRHAI_RTC))
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "SLP has more than one reliable mesg to ACK");
  }
  /* Check for any new signaling messages */
  else if ( hdrslp_tx_queue_first_item_ptr )
  {
    if ( hdrslp_tx_queue_first_item_ptr->reliable )
    {
      /* Check if we even fit the minimum (5 byte header + payload) in */
      if ( max_bytes <= 5 ) 
      {
        hdrslp_leave_crit_sect();
        return 0;
      }

      /* Check if we can accept any new reliable messages */
      if ( hdrslp_rxb_level < HDRSLP_REXMIT_BUFFER_MAX_LEVEL )
      {
        /* Initialize oldest_seqno to the last seqno transmitted. */
        oldest_seqno = (hdrslp_tx_seqno + 7) % 8; /* hdrslp_tx_seqno - 1 */
        for ( i = 0; i < 4; i++ ) 
        {
          if ( ! (hdrslp_rxb_free_mask & (0x01 << i)) )
          {
            if (hdrutil_msg_seq_num_is_greater( oldest_seqno, 
                  hdrslp_rexmit_buffer[i]->sdu.sno, 3 ))
            {
              /* If this message's seqno is less, it is older */
              oldest_seqno = hdrslp_rexmit_buffer[i]->sdu.sno;
            }
          }
        }

        /* If a message has been retransmitted for the last time but is still 
         * waiting to be acknowledged, the rexmit buffer is already freed
         * but the message is still sitting on the cb_wait queue.
         */
        cb_wait_msg_ptr = hdrslp_cb_wait_q_ptr; 
        while ( cb_wait_msg_ptr != NULL )
        {
          if ( cb_wait_msg_ptr->reliable && 
               (cb_wait_msg_ptr->tx_status != E_SUCCESS))
          {
            if (hdrutil_msg_seq_num_is_greater( oldest_seqno, 
                  cb_wait_msg_ptr->sdu.sno, 3 ))
            {
              /* If this message's seqno is less, it is older */
              oldest_seqno = cb_wait_msg_ptr->sdu.sno;
            }
          }
          cb_wait_msg_ptr = cb_wait_msg_ptr->next;
        }
      } /* if (hdrslp_rxb_level < HDRSLP_REXMIT_BUFFER_MAX_LEVEL) */

      /* At this point oldest_seqno is the smallest (modulo 2) seqno of the 
       * rexmit buffers.  All other rexmit buffers have seqnos that are greater
       * than this.  We can only transmit a new reilable message if 
       * hdrslp_tx_seqno is also still considered "greater than" this.
       */

      if ((hdrslp_rxb_level < HDRSLP_REXMIT_BUFFER_MAX_LEVEL) &&
          (hdrutil_msg_seq_num_is_greater( hdrslp_tx_seqno, oldest_seqno, 3)))
      {
        /* Keep local pointer to msg and take it off TX queue */
        current_msg_ptr = hdrslp_tx_queue_first_item_ptr;
        hdrslp_tx_queue_free_first_item();

        /* This is a new SDU - create the header and prepend it in  */
        /* the dsm item chain.                                      */
        if ( hdrslp_create_header( current_msg_ptr ) == 0 )
        {
          ERR( "SLP: No header for rel. msg (0x%x)",
               current_msg_ptr, 0, 0 );
          (void) hdrslp_cb_enqueue( current_msg_ptr, 0, E_FAILURE);

          hdrslp_leave_crit_sect();
          return 0;
        }
        
        /* Find first free item in rexmit buffer */
        i = hdrslp_rxb_lookup[hdrslp_rxb_free_mask];

        /* Move the queue item into the rexmit buffer and dup */
        /* the corresponding dsm item chain.                  */
        DSM_TOUCH_ITEM( current_msg_ptr->sdu.msg_ptr); 
        hdrslp_rexmit_buffer[i] = current_msg_ptr;
        hdrslp_rexmit_buffer[i]->queue = SLP_REXMIT_Q;

        /* Set the rexmit timer and count to initial values. */
        hdrslp_rexmit_buffer[i]->rexmit_time  = HDRSLP_REXMIT_TIME;
        hdrslp_rexmit_buffer[i]->rexmit_count = 0;

#ifdef HDR_ON_REXNT
        printf( "slp: rxb - populating item #%d.\n", i );
#endif        
        /* Make sure this item's timer is not set. This is really  */
        /* a sanity check.                                        */
        hdrslp_rxb_timer_mask &= ~(uint8)(0x01 << i);
        
        /* Mark this buffer item as used (0->non-free) */
        hdrslp_rxb_free_mask &= ~(uint8)(0x01 << i);

        /* Update rexmit buffer level */
        hdrslp_rxb_level++;
        
        /* Do any fragmentation if necessary */
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "SLPF: Tx rel. msg 0x%x",
                        current_msg_ptr );
        num_tx_bytes = hdrslp_fragment_sdu( tx_sdu_ptr,
                                            current_msg_ptr,
                                            max_bytes );
        
        /* Update stats */
        hdrslp_stats.tx_rel_msg_cnt++;

        hdrslp_leave_crit_sect();
        return num_tx_bytes;
      }
      else
      {
        /* We have too many outstanding reliable messages - don't */
        /* accept any new ones, but count this event.             */
        hdrslp_stats.tx_seq_space_full_cnt++;

        if (hdrslp_rxb_level < HDRSLP_REXMIT_BUFFER_MAX_LEVEL)
        {
          /* We still had a buffer but the seqno window was used up */
          HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                          "SLPF: stuck waiting for ack to seqno %d",
                          oldest_seqno );
        }
        
        /* Make sure any best-effort messages are still processed */
        hdrslp_tx_queue_advance_be_item();

        /* Check for correctness of destination channel on advanced best-effort
         * message
         */
        hdrslp_verify_dest_channel( tx_sdu_ptr->channel );
      }
    }
    
    /* The transmission buffer can only contain best-effort or reliable  */
    /* messages. This second check is to catch any best-effort messages  */
    /* which were advanced because we are out of sequence space and can  */
    /* only send non-reliably.                                           */
    if ( ! hdrslp_tx_queue_first_item_ptr->reliable )
    {
      /* Check if we even fit the minimum (4 byte header + payload) in */
      if ( max_bytes <= 4 ) 
      {
        hdrslp_leave_crit_sect();
        return 0;
      }

      /* Keep local pointer to msg */
      current_msg_ptr = hdrslp_tx_queue_first_item_ptr;


      #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
      /* If we have sent a UATIRequest and haven't received a UATIAssignment,
         don't send any more messages accept possibly another UATIRequest if
         we did idle handoff.  Luckily the UATIRequest is highest priority so
         if it a second one was queued will be at the head of the queue. */
      if ((current_msg_ptr->sdu.prot_id == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) 
         && (current_msg_ptr->sdu.prot_msg == 0) /* uati_req */ )
      {
        /* More UATIRequests are OK */
        if (hdrslp_waiting_for_uati_assignment)
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                        "SLPQH: Sending UATIRequest after IdleHO");
        }
        else
        {
          hdrslp_waiting_for_uati_assignment = TRUE;
          HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                        "SLPQH: Sending UATIRequest. Holding other msgs until UATIAssignment.");
        }

        HDRTRACE_SLP_SLPQH_NUM_PENDING_MSGS( 
            (hdrslp_tx_queue_item_count - 1 ) );
      }
      else if (hdrslp_waiting_for_uati_assignment)
      {
        /* Anything else should stay on the queue. */
        HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
                        "SLPQH: Holding (P 0x%x, ID 0x%x) and %d others.",
                        current_msg_ptr->sdu.prot_id,
                        current_msg_ptr->sdu.prot_msg,
                        hdrslp_tx_queue_item_count - 1 );
        HDRTRACE_SLP_SLPQH_NUM_PENDING_MSGS( 
            (hdrslp_tx_queue_item_count - 1 ) );

        hdrslp_leave_crit_sect();
        return 0;
      }
      else
      #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */
      if(hdrslp_waiting_for_bundling)
      {
        /* Waiting for more AC messages to complete bundling request */
        HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
                        "BUNDLE: Holding (P 0x%x, ID 0x%x) and %d others.",
                        current_msg_ptr->sdu.prot_id,
                        current_msg_ptr->sdu.prot_msg,
                        hdrslp_tx_queue_item_count - 1 );
        hdrslp_leave_crit_sect();
        return 0;
      }

      /* Remove from TX Q if we decide to send in current probe */
      hdrslp_tx_queue_free_first_item();

      /* If this is not the first call to SLP to get data, check that DOS */ 
      /* message would fit without fragmentation on AC. The MAX size of   */
      /* DOS message we queue to SLP is MAX_PROBESIZE - (COMMON_AC_PKT +  */
      /* CONN_REQ) so we know it will fit on first call                   */
      if ( !first_callfor_slpdata )
      {
        if (((current_msg_ptr->sdu.prot_id == HDRHAI_N_APP1_TYPE) ||
             (current_msg_ptr->sdu.prot_id == HDRHAI_N_APP2_TYPE) ||
             (current_msg_ptr->sdu.prot_id == HDRHAI_N_APP3_TYPE))
           && (current_msg_ptr->sdu.prot_msg == HDRDOS_DATA_OVER_SIGNALING_MSG_ID))
        {
          /* SLP header size is 2 bytes */
          if ((dsm_length_packet(current_msg_ptr->sdu.msg_ptr) + 2) > max_bytes)
          {
            /* Do not send the message if it can not fit in the current probe */
            HDR_MSG_PROT( MSG_LEGACY_HIGH, "Canceling DOS message" );

            (void) hdrslp_cb_enqueue( current_msg_ptr, 0, E_DATA_TOO_LARGE );
            hdrslp_leave_crit_sect();
            return 0;
          }
        }
      }

      /* This is a new SDU - create the header and prepend it in  */
      /* the dsm item chain.                                      */
      if ( hdrslp_create_header( current_msg_ptr ) == 0 )
      {
        if (tx_sdu_ptr->channel == HDRHAI_AC)
        {
          /* A null message is used to force an access probe for */
          /* sending only the "common_ac_pkt" (i.e. RUP message) */
          /* The null message is lowest priority so we are       */
          /* guaranteed there is no more signaling, so we can    */
          /* safely return "zero bytes"                          */
          (void) hdrslp_cb_enqueue( current_msg_ptr, 1, E_SUCCESS);
        }
        else
        {
          /* For RTC messages is not allowed to send an empty message */
          ERR( "SLP: No header for b-e msg (0x%x)",
               current_msg_ptr, 0, 0 );
          (void) hdrslp_cb_enqueue( current_msg_ptr, 0, E_FAILURE);
        }
        hdrslp_leave_crit_sect();
        return 0;
      }
      
      /* Do any fragmentation if necessary */
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "SLPF: Tx b-e msg 0x%x",
                      current_msg_ptr );
      num_tx_bytes = hdrslp_fragment_sdu( tx_sdu_ptr, 
                                          current_msg_ptr, 
                                          max_bytes );
      
      /* Update stats */
      hdrslp_stats.tx_bef_msg_cnt++;
      hdrslp_leave_crit_sect();
      return num_tx_bytes;
    }
  }

  /* No new signaling msgs in tx queue - check for pending Acks only */
  if ( hdrslp_rx_ack_timer_mask )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "SLP: Sending Ack-only pkt" );

    /* Check if we even fit the minimum (3 byte header) in */
    if ( max_bytes < 3 ) 
    {
      hdrslp_leave_crit_sect();
      return 0;
    }

    hdrslp_rx_ack_only_msg.sdu.msg_ptr = NULL;

    num_tx_bytes = hdrslp_create_header( &hdrslp_rx_ack_only_msg );
    if ( num_tx_bytes == 0 )
    {
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH,  "SLP: AckBuf=0x%x[%d], 0x%x",
                (uint32) ((hdrslp_rx_ack_buffer[3] << 24)
                          ||
                          (hdrslp_rx_ack_buffer[2] << 16)
                          ||
                          (hdrslp_rx_ack_buffer[1] << 8)
                          ||
                          hdrslp_rx_ack_buffer[0]),
                hdrslp_rx_ack_buffer_read_index,
                hdrslp_rx_ack_timer_mask );
      hdrslp_leave_crit_sect();
      return 0;
    }


    /* Send the ack-only header as a complete, new message on the RTC */
    /* If the AN sent a reliable message on the CC while the AT was   */
    /* idle, or if for any other reason the AT thinks it needs to ack */
    /* a reliable message while idle, we must "drop" the ack, because */
    /* the SLP sequence numbers aren't initialized and the ACK would  */
    /* be late, anyway, since we don't maintain the T_SLPAck timer    */
    /* properly when idle and would also need to do an access probe.  */
    if (tx_sdu_ptr->channel == HDRHAI_RTC)
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( (uint8*) tx_sdu_ptr, sizeof(hdrslp_sdu_struct_type), 
               (uint8*) &hdrslp_rx_ack_only_msg, 
               sizeof( hdrslp_sdu_struct_type ) );
#else
      memcpy( (uint8*) tx_sdu_ptr, 
          (uint8*) &hdrslp_rx_ack_only_msg, 
          sizeof( hdrslp_sdu_struct_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      /* We must re-initialize this field since the memcpy overwrote it */
      tx_sdu_ptr->channel = HDRHAI_RTC;
      tx_sdu_ptr->length  = 0;
      hdrslp_leave_crit_sect();
      return num_tx_bytes;
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH,  "Drop SLP ACK to CC msg when idle");
      hdrslp_leave_crit_sect();
      return 0;
    }
  }

  /* No msgs or acks to be sent */
  hdrslp_leave_crit_sect();
  return 0;

} /* hdrslp_build_tx_data() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_CREATE_HEADER

DESCRIPTION
  This function takes a new msg from the TX queue and prepends
  the common header parts in it's own dsm item.

DEPENDENCIES
  None
  
PARAMETERS
  tx_item - Pointer to the new SLP msg from TX queue

RETURN VALUE
  Length of header in bytes
  
SIDE EFFECTS
  The dsm item chain of the passed queue item gets the common part
  of the SLP header prepended.
===========================================================================*/

uint8 hdrslp_create_header( hdrslp_msg_struct_type* tx_msg_ptr )
{
  uint8          num_header_bytes;
  uint8          header[5];
  uint8          i;
  void*          tmp_ptr; /* Temporary pointer */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /***********************************************************************/
  /* The SLP header for X6 is slpit into a fragmentation and a delivery  */
  /* part. The maximum length header is as follows:                      */
  /*                                                                     */
  /*     0               8               16                              */
  /*     |. . . . . . . .|. . . . . . . .|. . . . . . . .|               */
  /*      H H X X X X F B E S S S S S S I AvAnAnAnSvSnSnSn               */
  /*                                                                     */
  /*     with:  H  - Stream header (2 bits)                              */
  /*            X  - Reserved                                            */
  /*            F  - Fragmentation bit                                   */
  /*            B  - Begin (first fragment) bit                          */
  /*            E  - End (last fragment) bit                             */
  /*            S  - Fragment sequence number (6 bits)                   */
  /*            I  - SLP-D header included bit (padding if B = 0)        */
  /*            Av - Ack number valid bit                                */
  /*            An - Ack number (3 bits)                                 */
  /*            Sv - Sequence number valid bit                           */
  /*            Sn - Sequence number (3 bits)                            */
  /*                                                                     */
  /* Common cases:                                                       */
  /* -------------                                                       */
  /*                                                                     */
  /* Unfragmented best-effort message:                                   */
  /*                                                                     */
  /*     0               8               16                              */
  /*     |. . . . . . . .|. . . . . . . .|. . . . . . . .|               */
  /*      H H X X X X 0 0 -m-e-s-s-a-g-e--p-a-y-l-o-a-d--                */
  /*                                                                     */
  /* Unfragmented best-effort message w/ Ack:                            */
  /*                                                                     */
  /*     0               8               16                              */
  /*     |. . . . . . . .|. . . . . . . .|. . . . . . . .|               */
  /*      H H X X X X 0 1 1 AnAnAn0 0 0 0 -m-e-s-s-a-g-e-                */
  /*                                                                     */
  /* Unfragmented reliable message:                                      */
  /*                                                                     */
  /*     0               8               16                              */
  /*     |. . . . . . . .|. . . . . . . .|. . . . . . . .|               */
  /*      H H X X X X 0 1 0 0 0 0 1 SnSnSn -m-e-s-s-a-g-e-               */
  /*                                                                     */
  /* Unfragmented reliable message w/ Ack:                               */
  /*                                                                     */
  /*     0               8               16                              */
  /*     |. . . . . . . .|. . . . . . . .|. . . . . . . .|               */
  /*      H H X X X X 0 1 1 AnAnAn1 SnSnSn -m-e-s-s-a-g-e-               */
  /*                                                                     */
  /***********************************************************************/

  /* Build the common header first and update the fragment bit later */
  memset( header, 0x00, 5 );

  /* Check if we build an ack-only header */
  if ( ! tx_msg_ptr->sdu.msg_ptr )
  {
    num_header_bytes = 2;
    header[0] = 0x01;
    header[1] = 0x80;

#ifndef HDRSLP_NO_LOGGING
    tx_msg_ptr->sdu.rel = FALSE;
    tx_msg_ptr->sdu.frg = FALSE;
    tx_msg_ptr->sdu.fst = TRUE;
    tx_msg_ptr->frg_seqno = hdrslp_fragment_seqno;
    tx_msg_ptr->sdu.sno = 0xff;
#endif
  }
  else
  {
    if ( tx_msg_ptr->reliable )
    {
      num_header_bytes = 2;
      header[0] = 0x01;
      header[1] = 0x08;
      header[1] |= (hdrslp_tx_seqno & 0x07);

#ifndef HDRSLP_NO_LOGGING
      tx_msg_ptr->sdu.rel = TRUE;
      tx_msg_ptr->sdu.sno = hdrslp_tx_seqno;
#endif

      hdrslp_tx_seqno = (hdrslp_tx_seqno + 1) % 8;
    }
    else
    {
      num_header_bytes = 1;
      header[0] = 0x00;

#ifndef HDRSLP_NO_LOGGING
      tx_msg_ptr->sdu.rel = FALSE;
      tx_msg_ptr->sdu.sno = 0xff;
#endif
    }
  }

  /* Fill in next Ack seqno if we got a new one,   */
  /* otherwise just omit it                        */
  if ( hdrslp_rx_ack_buffer[hdrslp_rx_ack_buffer_read_index] 
       & 
       HDRSLP_RX_ACK_BUFFER_USED_MASK )
  {
    /* Use void pointer as a bridge to avoid the following compiler warning: 
       "cast increases required alignment of target type". */
    tmp_ptr = (void *)(hdrslp_rx_ack_buffer);
    if (hdrslp_verbose_debug)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_MED, "SLP: Tx Ack for # 0x%x (AckBuf = 0x%x)",
                    hdrslp_rx_ack_buffer[hdrslp_rx_ack_buffer_read_index]
                    & HDRSLP_RX_ACK_BUFFER_SEQNO_MASK,
                    *((uint32*)tmp_ptr) );
    }
    /* Update current rx ack seqno */
    hdrslp_rx_ack_current_seqno = 
      hdrslp_rx_ack_buffer[hdrslp_rx_ack_buffer_read_index] 
        & HDRSLP_RX_ACK_BUFFER_SEQNO_MASK;

    /* Clear item in rx ack buffer */
    hdrslp_rx_ack_buffer[hdrslp_rx_ack_buffer_read_index] &= 0x00;

    /* Clear timer in rx ack timer mask */
    hdrslp_rx_ack_timer_mask &= ~(uint8)(0x01 << hdrslp_rx_ack_buffer_read_index);
    hdrslp_rx_ack_timer_mask &= 0x0f;

    /* Advance read index by one */
    hdrslp_rx_ack_buffer_read_index = 
      (hdrslp_rx_ack_buffer_read_index + 1) % 4;

    /* Update stats*/
    hdrslp_stats.tx_msg_ack_cnt++;

    /* Mark the Ack # field as valid and update the header length  */
    /* of best-effort messages (don't include full header).        */
    if ( tx_msg_ptr->sdu.msg_ptr
         &&
         (! tx_msg_ptr->reliable) )
    {
      /* Set full-header-included bit */
      header[0] |= 0x01;

      /* Increase the header length by one to accommodate full header */
      num_header_bytes++;
    }
    header[1] |= 0x80;

    /* Fill in the Ack # */
    header[1] |= (hdrslp_rx_ack_current_seqno << 4) & 0x70;
#ifndef HDRSLP_NO_LOGGING
    tx_msg_ptr->sdu.ano = hdrslp_rx_ack_current_seqno;
    tx_msg_ptr->sdu.ano_valid = TRUE;
#endif
  }
  else
  {
    /* Sanity check to avoid producing empty headers */
    if ( ! tx_msg_ptr->sdu.msg_ptr )
    {
      /* Make sure the timer mask is in sync w/ the ack buffer */
      for ( i=hdrslp_rx_ack_buffer_read_index;
            i<(hdrslp_rx_ack_buffer_read_index + 4);
            i++ )
      {
        if ( !(hdrslp_rx_ack_buffer[i%4] & HDRSLP_RX_ACK_BUFFER_USED_MASK) )
        {
          hdrslp_rx_ack_timer_mask &= ~(uint8)(0x01 << (i % 4));
        }
      }

      /* Find the oldest entry and resync the read index */
      for ( i=hdrslp_rx_ack_buffer_write_index + 1;
            i<=(hdrslp_rx_ack_buffer_write_index + 4);
            i++ )
      {
        if ( hdrslp_rx_ack_buffer[i%4] & HDRSLP_RX_ACK_BUFFER_USED_MASK )
        {
          break;
        }
      }
      hdrslp_rx_ack_buffer_read_index = i % 4;

      return 0;
    }

#ifndef HDRSLP_NO_LOGGING
    tx_msg_ptr->sdu.ano = 0;
    tx_msg_ptr->sdu.ano_valid = FALSE;
#endif
  }

  /* Prepend header to sig msg */
  if( hdrutil_pushdown_head( &(tx_msg_ptr->sdu.msg_ptr),
                                   (void *) header,
                                   num_header_bytes )
      !=
      num_header_bytes )
  {
    ERR( "SLP: Header pushdown failed", 0, 0, 0 );
    num_header_bytes = 0;
  }

  return num_header_bytes;

} /* hdrslp_create_header() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_FRAGMENT_SDU

DESCRIPTION
  This function takes an SDU and does fragmentation if necessary.
  the hdrslp_xmit_fragment_buffer is used to keep a fragmented SDU
  until all fragments are transmitted.

DEPENDENCIES
  None
  
PARAMETERS
  frg_sdu_ptr - Pointer to the fragmented SDU
  org_msg_ptr - Pointer to the original message buffer
  max_bytes   - maximum frame size the signaling layer packet has to fit in

RETURN VALUE
  Length of signaling layer packet in bytes
  
SIDE EFFECTS
  None
===========================================================================*/

uint8 hdrslp_fragment_sdu
( 
  hdrslp_sdu_struct_type* frg_sdu_ptr, 
  hdrslp_msg_struct_type* org_msg_ptr,
  uint8 max_bytes 
)
{
  uint8                    length;
  uint8                    h_len;
  hdrslp_sdu_struct_type*  org_sdu_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++  S L P - F  +++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

  /* Deal with the SDU struct only */
  org_sdu_ptr = &(org_msg_ptr->sdu);

  if ( org_sdu_ptr->msg_ptr == NULL )
  {
    ERR( "SLPF: Frg'ing NULL msg 0x%x (%d)", 
         org_msg_ptr, org_msg_ptr->queue, 0 );

    (void) hdrslp_mp_free_msg( org_msg_ptr, E_FAILURE );

    return 0;
  }

  /* Avoid the same logic over and over */
  h_len = HDRSLP_SDU_HDR_SIZE( org_sdu_ptr->msg_ptr->data_ptr );

  /* Check if a remaining fragment is still too big, or if   */
  /* a new message is too big.                               */
  if ( ! hdrslp_fragment_header )
  {
    /* Check if a new message has to be fragmented - dup the   */
    /* the max possible size into the tx sdu.                  */
    if ( (h_len + org_sdu_ptr->length) > max_bytes )
    {
      /* Store the sig msg in the frg buffer */
      hdrslp_fragment_buffer = org_msg_ptr;
      hdrslp_fragment_buffer->queue = SLP_FRG_Q;

      length = dsm_dup_packet( &(frg_sdu_ptr->msg_ptr), 
                               org_sdu_ptr->msg_ptr, 
                               h_len,
                               max_bytes - h_len - 1 );

      if ( ( length != max_bytes - h_len - 1 ) ||
           ( frg_sdu_ptr->msg_ptr == NULL ) )
      {
        ERR( "SLPF: 1st frg dup 0x%x->0x%x (%d)", 
             org_sdu_ptr->msg_ptr, 
             frg_sdu_ptr->msg_ptr,
             length );

        /* Free up the message and the fragment buffer */

        (void) hdrslp_mp_free_msg( org_msg_ptr, E_AGAIN );
        
        return 0;
      }
      HDRDSM_TRACE_DUP( frg_sdu_ptr->msg_ptr, HDRDSM_TX_SLP_FRG );

      /* Update the frg buffer pointer up to were we duped the */
      /* original message, update tx count.                    */
      hdrslp_fragment_pointer = length + h_len;
      hdrslp_fragment_tx_count = length;

      /* Copy the header into it's own item since we need to modify it;
         pushdown one extra garbage byte because we push a fragment
         byte inbetween immediately afterwards */
      (void) hdrutil_pushdown( &(frg_sdu_ptr->msg_ptr),
                        org_sdu_ptr->msg_ptr->data_ptr,
                        h_len );

      DSM_TOUCH_ITEM( frg_sdu_ptr->msg_ptr );

      /* Push fragment byte inbetween (presrve SLP-D incl. bit) ... */
      *(frg_sdu_ptr->msg_ptr->data_ptr + 2) =
        *(frg_sdu_ptr->msg_ptr->data_ptr + 1);
      *(frg_sdu_ptr->msg_ptr->data_ptr + 1) = 0x00;
      *(frg_sdu_ptr->msg_ptr->data_ptr + 1) |= 
        *(frg_sdu_ptr->msg_ptr->data_ptr) & 0x01;

      /* ... set the FRAGMENT and BEGIN bit ... */
      *(frg_sdu_ptr->msg_ptr->data_ptr) |= 0x03;

      /* ... and fill in the seqno's */
      *(frg_sdu_ptr->msg_ptr->data_ptr + 1) |= 
        (hdrslp_fragment_seqno << 1) & 0xfe;
      hdrslp_fragment_seqno = (hdrslp_fragment_seqno + 1) % 64;

      /* Update the header length to account for the inserted byte */
      frg_sdu_ptr->msg_ptr->used++;
      h_len++;
      ASSERT( frg_sdu_ptr->msg_ptr->used <= frg_sdu_ptr->msg_ptr->size );

      /* length contains the payload length, add the frg header */
      length += h_len;

      /* Keep meta inforation in the fragmented message */
      frg_sdu_ptr->channel = org_sdu_ptr->channel;
      frg_sdu_ptr->prot_id = org_sdu_ptr->prot_id;
      frg_sdu_ptr->prot_msg = org_sdu_ptr->prot_msg;

#ifndef HDRSLP_NO_LOGGING
      org_sdu_ptr->frg = TRUE;
      org_sdu_ptr->fst = TRUE;
      org_sdu_ptr->lst = FALSE;
      frg_sdu_ptr->frg = TRUE;
      frg_sdu_ptr->fst = TRUE;
      frg_sdu_ptr->lst = FALSE;
#endif  /* HDRSLP_NO_LOGGING */

      /* Update the frg header for the remaining fragments */
      *((uint8*) &hdrslp_fragment_header) = 0x02;
      *(((uint8*) &hdrslp_fragment_header) + 1) = 0x00;

      /* Update stats */
      hdrslp_stats.tx_fragment_msg_cnt++;

      HDR_MSG_PROT_3( MSG_LEGACY_MED, "TX 1st frg, seq=%d, %d/%d bytes\n",
                      hdrslp_fragment_seqno - 1,
                      hdrslp_fragment_tx_count,
                      org_sdu_ptr->length );
    }
    else
    {
      /* Just send the msg in complete */
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( (uint8*) frg_sdu_ptr, sizeof(hdrslp_sdu_struct_type),
               (uint8*) org_sdu_ptr, 
               sizeof( hdrslp_sdu_struct_type ) );
#else
      memcpy( (uint8*) frg_sdu_ptr, 
              (uint8*) org_sdu_ptr, 
              sizeof( hdrslp_sdu_struct_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      if ( org_msg_ptr->reliable )
      {
        length = dsm_dup_packet( &(frg_sdu_ptr->msg_ptr), 
                                 org_sdu_ptr->msg_ptr, 
                                 0,
                                 org_sdu_ptr->length + h_len );

        if ( ( length != org_sdu_ptr->length + h_len ) ||
             ( frg_sdu_ptr->msg_ptr == NULL ) )
        {
          ERR( "SLPF: B-E dup 0x%x->0x%x (%d)",
               org_sdu_ptr->msg_ptr,
               frg_sdu_ptr->msg_ptr,
               length );

          (void) hdrslp_mp_free_msg( org_msg_ptr, E_AGAIN );
          return 0;
        }
        DSM_TOUCH_ITEM( frg_sdu_ptr->msg_ptr );

        HDR_MSG_PROT_1( MSG_LEGACY_LOW, "TX rel. msg (%d bytes)", length );
      }
      else
      {
        length = org_sdu_ptr->length + h_len;
        
        /* The orig. dsm msg ptr is now obsolete since we handed  */
        /* the msg buffer off and are not gonna send this again;  */
        /* set msg ptr to NULL and the tx status to successful.   */
        org_msg_ptr->sdu.msg_ptr = NULL;

        if (hdrslp_verbose_debug)
        {
          HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "=TX= TX b-e msg (%d bytes), Q=%d", 
                        length,
                        org_msg_ptr->queue );
        }
        (void) hdrslp_cb_enqueue( org_msg_ptr, 1, E_SUCCESS);

        HDRDSM_SET_TRACER( frg_sdu_ptr->msg_ptr, HDRDSM_TX_SLP );
      }
    }
  }
  else
  {
    /* Pick remaining fragment from frg buffer ... */
    length = dsm_dup_packet( 
               &(frg_sdu_ptr->msg_ptr), 
               org_sdu_ptr->msg_ptr, 
               hdrslp_fragment_pointer, 
               max_bytes - (uint16) sizeof( hdrslp_fragment_header ) );

    if (( frg_sdu_ptr->msg_ptr == NULL ) || ( length == 0 ))
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "dsm_dup_packet returns length %d", length);
      return 0;
    }

    HDRDSM_TRACE_DUP( frg_sdu_ptr->msg_ptr, HDRDSM_TX_SLP_FRG );

    /* Update offset into the msgs's item chain and the tx count */
    hdrslp_fragment_pointer += length;
    hdrslp_fragment_tx_count += length;

    /* Update the fragment header w/ the sequence number */
    *(((uint8*) &hdrslp_fragment_header) + 1) = 
      (hdrslp_fragment_seqno << 1) & 0x7e;
    hdrslp_fragment_seqno = (hdrslp_fragment_seqno + 1) % 64;

    /* ... and prepend frg header to the new fragment */
    (void) hdrutil_pushdown_head( &(frg_sdu_ptr->msg_ptr), 
                           (void*) &hdrslp_fragment_header,
                           (uint16) sizeof( hdrslp_fragment_header ) );
    length += (uint8) sizeof( hdrslp_fragment_header );

    HDRDSM_SET_TRACER( frg_sdu_ptr->msg_ptr, HDRDSM_TX_SLP_FRG );

    /* Keep meta inforation in the fragmented message */
    frg_sdu_ptr->channel = org_sdu_ptr->channel;
    frg_sdu_ptr->prot_id = org_sdu_ptr->prot_id;
    frg_sdu_ptr->prot_msg = org_sdu_ptr->prot_msg;

#ifndef HDRSLP_NO_LOGGING
    frg_sdu_ptr->frg = TRUE;
    frg_sdu_ptr->fst = FALSE;
    frg_sdu_ptr->lst = FALSE;
#endif  /* HDRSLP_NO_LOGGING */
    
    HDR_MSG_PROT_3( MSG_LEGACY_MED, "TX cont frg, seq=%d, %d/%d bytes\n",
             hdrslp_fragment_seqno - 1,
             hdrslp_fragment_tx_count,
             org_sdu_ptr->length );
    
    /* Check if we sent all fragments */
    ASSERT ( hdrslp_fragment_tx_count <= org_sdu_ptr->length );

    if ( hdrslp_fragment_tx_count == org_sdu_ptr->length )
    {
      /* Set the END bit in the header */
      *(frg_sdu_ptr->msg_ptr->data_ptr + 1) |= 0x80;

      /* Check if this was a best-effort and schedule the callback */
      if ( ! org_msg_ptr->reliable )
      {
        if (hdrslp_verbose_debug)
        {
          HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Free 0x%x",
                        org_msg_ptr->sdu.msg_ptr );
        }
        dsm_free_packet( &(org_msg_ptr->sdu.msg_ptr) );

        (void) hdrslp_cb_enqueue( org_msg_ptr, 1, E_SUCCESS );
      }
      else
      {
        /* Reassign this msg buffer to the rexmit queue  */
        /* as long as it is still on the fragment queue. */
        if ( org_msg_ptr->queue == SLP_FRG_Q )
        {
          org_msg_ptr->queue = SLP_REXMIT_Q;
        }
      }

#ifndef HDRSLP_NO_LOGGING
      frg_sdu_ptr->lst = TRUE;
#endif  /* HDRSLP_NO_LOGGING */

      /* We sent all fragments, reset the frg buffer   */
      hdrslp_fragment_buffer   = NULL;
      hdrslp_fragment_header   = 0;
      hdrslp_fragment_pointer  = 0;
    }
  }

  return length;

} /* hdrslp_fragment_sdu() */

/* <EJECT> */ 
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
)
{
  hdrslp_msg_struct_type*  invalid_channel_msg_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while ( hdrslp_tx_queue_first_item_ptr 
          &&
          hdrslp_tx_queue_first_item_ptr->sdu.channel != current_channel )
  {
    /* Let sender know this is not allowed and remove msg from TX queue */
    invalid_channel_msg_ptr = hdrslp_tx_queue_first_item_ptr;
    hdrslp_tx_queue_free_first_item();

    (void) hdrslp_cb_enqueue( invalid_channel_msg_ptr, 0, E_NOT_ALLOWED );
  }
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_PROCESS_TX_ACK

DESCRIPTION
  This function takes a received Ack sequence number and updates the
  retransmission buffer accordingly.
  NOTE: This function is run in the context of the Rx Task and hence
  needs to lock interrupts while accessing the retransmission buffer.

DEPENDENCIES
  None
  
PARAMETERS
  ack_seqno - Received Ack sequence number

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_tx_ack( uint8 ack_seqno )
{
  hdrslp_msg_struct_type* itr;
  int                     i;
  rex_tcb_type *hdrtx_tcb_ptr; /* temp ptr */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_DISABLE_PREMPTION();

  hdrslp_enter_crit_sect();

  for ( i=0; i<4; i++ )
  {
    if ( (! (hdrslp_rxb_free_mask & (0x01 << i)))
         &&
         hdrslp_rexmit_buffer[i]
         &&
         (hdrslp_rexmit_buffer[i]->queue == SLP_REXMIT_Q)
         &&
         hdrslp_rexmit_buffer[i]->sdu.msg_ptr
         &&
         (HDRSLP_SDU_SEQNO( hdrslp_rexmit_buffer[i]->sdu.msg_ptr->data_ptr ) 
          == 
          ack_seqno) )
    {
#ifdef HDR_ON_REXNT
      printf( "slp: rxb - clearing item #%d, rxb level = %d.\n", 
              i, hdrslp_rxb_level - 1 );
#endif        
      
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH,  "SLPF: Ack'd rexmit[%d] w/ %d",
                      i, ack_seqno );

      /* Keep track of the number of SLP retransmitions based on the 
         number of attempts before success for logging purposes. */
      switch ( hdrslp_rexmit_buffer[i]->rexmit_count )
      {
        case 0:
          hdrslp_log_inc_conn_state_stats_counter(
            HDRSLP_1ST_ATTEMPT_SUCCESS_COUNT, 1 );
          break;

        case 1:
          hdrslp_log_inc_conn_state_stats_counter(
            HDRSLP_2ND_ATTEMPT_SUCCESS_COUNT, 1 );
          break;

        case 2:
          hdrslp_log_inc_conn_state_stats_counter(
            HDRSLP_3RD_ATTEMPT_SUCCESS_COUNT, 1 );
          break;

        default:
          break;
      
      }

      /* Mark this item in the rexmit buffer as unused and   */
      /* make sure no timer is pending on it.                */
      itr = hdrslp_rexmit_buffer[i];
      hdrslp_rexmit_buffer[i] = NULL;
      hdrslp_rxb_free_mask |= (0x01 << i);
      hdrslp_rxb_timer_mask &= ~(uint8)(0x01 << i);
      
      /* Update the rexmit buffer level */
      hdrslp_rxb_level--;

      /* Check if the fragmentation buffer is pointing to this msg */
      if ( hdrslp_fragment_buffer == itr )
      {
        /* Reset the fragment buffer */
        hdrslp_fragment_buffer  = NULL;
        hdrslp_fragment_header  = 0;
        hdrslp_fragment_pointer = 0;
      }

      /* Queue the callback func to let the sending protocol know */
      (void) hdrslp_cb_enqueue( itr, 0, E_SUCCESS );
      
      hdrslptx_enter_crit_sect( &hdrslptx_crit_sect_dsmfree );           
      /* Free the dsm chain containing the message */
      if ( itr->sdu.msg_ptr )
      {
        dsm_free_packet( &(itr->sdu.msg_ptr) );
      }
      else
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "hdrslp_process_tx_ack: msg already released!" );
      }
      hdrslptx_leave_crit_sect( &hdrslptx_crit_sect_dsmfree );

      /* Update stats */
      hdrslp_stats.rx_msg_ack_cnt++;

      break;
    }
  }

  /* Check the pending-callbacks queue if we didn't find   */
  /* any re-transmit buffer item w/ a matching ack number. */
  if ( i == 4 )
  {
    itr = hdrslp_cb_wait_q_ptr; 
    while ( itr != NULL )
    {
      /* Check if the message sent was reliable and if this is its ack */
      if ( (itr->queue == SLP_CB_WAIT_Q)
           &&
           (itr->sdu.msg_ptr != NULL )
           && 
           ( HDRSLP_SDU_SEQNO( itr->sdu.msg_ptr->data_ptr ) 
             ==
             ack_seqno ))
      {
        /* Check if the fragmentation buffer is pointing to this msg */
        if ( hdrslp_fragment_buffer == itr )
        {
          /* Reset the fragment buffer */
          hdrslp_fragment_buffer  = NULL;
          hdrslp_fragment_header  = 0;
          hdrslp_fragment_pointer = 0;
        }

        /* Mark successful transmission and enqueue for cb processing */
        itr = hdrslp_cb_enqueue( itr, 0, E_SUCCESS );
        i = 0;
        
        break;
      }
      else
      {
        itr = itr->next;
      }
    }
  }

  hdrslp_leave_crit_sect();
  REX_ENABLE_PREMPTION();

  if ( i < 4 )
  {
    /* We processed a valid ack, set the TX CB signal to  */
    /* process the newly enqueued callbacks.              */
    if (hdrslp_verbose_debug)
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "=TX=  set cb sig- tx ack");
    }
    hdrtx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
    (void) rex_set_sigs( hdrtx_tcb_ptr, HDRTX_SLP_CB_SIG );
  }
  else
  {
    /* If we didn't find a corresponding message in the pending queue */
    /* either this is a duplicate or bogus ack sequence number.       */
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "SLPF: Rcvd unexpected ack %d",
                    ack_seqno );
  }

} /* hdrslp_process_tx_ack() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_FIND_MSG

DESCRIPTION
  This function finds a reliable message in the retransmission
  buffer given its callback funcptr and tag.
  It is used by hdrslp_abort_msg().
  NOTE: This function is run in the context of any task and hence
  needs to lock interrupts while accessing the retransmission buffer.

DEPENDENCIES
  None
  
PARAMETERS
  slp_outcome_cb  - Function to be called after message has been
                    transmitted successfully or transmission was
                    aborted, e.g. channel not available, TX queue full.
  outcome_tag_ptr - User data for callback function

RETURN VALUE
  None
  
SIDE EFFECTS
  hdrslp_rexmit_buffer[i] is a critical section that needs protection.
  However, we don't want to protect inside this function since usually once
  this function is called (the message is found), the caller function wants
  to do something on the found message.
  To avoid interruption between finding the message and processing the
  message, the caller function is expected to protect the resource.
===========================================================================*/

hdrslp_msg_struct_type* hdrslp_find_msg
(
  hdrslp_outcome_cb_type     slp_outcome_cb,
  void*                      outcome_tag_ptr 
)
{
  int i;
  
  for ( i=0; i<4; i++ )
  {                        

    if ( ( hdrslp_rexmit_buffer[i] != NULL ) &&
         ( hdrslp_rexmit_buffer[i]->outcome_func_ptr == slp_outcome_cb ) &&
         ( hdrslp_rexmit_buffer[i]->outcome_tag_ptr == outcome_tag_ptr )
       )
    {
      return hdrslp_rexmit_buffer[i];
    }
  }

  return NULL;

} /* hdrslp_find_msg() */

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

hdrslp_msg_struct_type* hdrslp_tx_queue_insert_item( uint8 priority )
{
  hdrslp_msg_struct_type* new_msg;
  hdrslp_msg_struct_type* itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is for debugging purpose only and can be removed later*/
  if (hdrslp_tx_queue_item_count >= 15)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_FATAL, "tx_queue_item_count %d",
                    hdrslp_tx_queue_item_count );
  }

  /* Check if we have any space left */
  if ( hdrslp_tx_queue_item_count == HDRSLP_TX_QUEUE_SIZE )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Xmit queue is full !" );
    return NULL;
  }

  /* Get a new msg buffer from the pool */
  if ( (new_msg = hdrslp_mp_get_msg()) == NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "No SLP msg buffer available !" );
    return NULL;
  }
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "SLPTX: new msg buffer from pool 0x%x ",new_msg);
  /* Insert new msg into queue according to priority */
  if ( hdrslp_tx_queue_first_item_ptr == NULL )
  {
    /* Common case (empty queue) */
    hdrslp_tx_queue_first_item_ptr = new_msg;
    new_msg->next = NULL;
  }
  else if (hdrslp_tx_queue_first_item_ptr->priority > priority)
  {
    /* We have at least one element in the list.  Check the priority of the
       first elt to see if we need to insert the new element at the head of
       the queue */
    new_msg->next = hdrslp_tx_queue_first_item_ptr;
    hdrslp_tx_queue_first_item_ptr = new_msg;
  }
  else
  {
    /* Otherwise, traverse the queue to find the insertion point */
    itr = hdrslp_tx_queue_first_item_ptr;
    while ( (itr->next != NULL) 
            && 
            (((hdrslp_msg_struct_type*) (itr->next))->priority <= priority ) ) 
    {
      itr = (hdrslp_msg_struct_type*) (itr->next);
    }

    new_msg->next = itr->next;
    itr->next = (void*) new_msg;
  }

  hdrslp_tx_queue_item_count++;

  return new_msg;

} /* hdrslp_tx_queue_insert_item() */

/* <EJECT> */ 
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
  None
===========================================================================*/

void hdrslp_tx_queue_free_first_item( void )
{
  /* Update first ptr to point at the next item (or NULL) */
  hdrslp_tx_queue_first_item_ptr = 
    (hdrslp_msg_struct_type*) (hdrslp_tx_queue_first_item_ptr->next);

  hdrslp_tx_queue_item_count--;
}

/* <EJECT> */ 
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

void hdrslp_tx_queue_advance_be_item( void )
{
  hdrslp_msg_struct_type* itr;
  hdrslp_msg_struct_type* last_itr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find end or next best-effort message in the transmit queue */
  for ( itr = hdrslp_tx_queue_first_item_ptr;
        (itr != NULL) && itr->reliable;
        itr = (hdrslp_msg_struct_type*) (itr->next) )
  {
    if ( itr == (hdrslp_msg_struct_type*) (itr->next) )
    {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "SLP TXQ linked to itself [0x%x]",
                      itr );
      itr->next = NULL;
    }
    
    last_itr = itr;
  }
  
  /* If there is a best-effort message waiting, move it to the front */
  if ( itr && ! itr->reliable )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "SLP: Advancing b-e msg 0x%x", itr);

    /* Take item out of its position */
    if ( last_itr )
    {
      last_itr->next = itr->next;
    }

    /* Insert item into the beginning */
    itr->next = hdrslp_tx_queue_first_item_ptr;
    hdrslp_tx_queue_first_item_ptr = itr;
  }

} /* hdrslp_tx_queue_advance_be_item() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_TX_QUEUE_CLEAR

DESCRIPTION
  This function clears out any items in the transmit queue.

DEPENDENCIES
  None
  
PARAMETERS
  err_num - HDR error number to use in callbacks

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_tx_queue_clear
(
  hdrerrno_enum_type err_num 
)
{
  hdrslp_msg_struct_type* itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset TX queue pointer first in case a callback enqueues */
  /* a cancelled msg again.                                   */

  hdrslp_enter_crit_sect( );

  itr = hdrslp_tx_queue_first_item_ptr;
  hdrslp_tx_queue_first_item_ptr = NULL;

  while( itr != NULL )
  {
    /* Make sure we clean out the transmit queue callbacks */
    if (err_num == E_ABORTED)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH,  
                      "Clear XMIT_Q before first probe. (P 0x%x ID 0x%x)",
                      itr->sdu.prot_id, itr->sdu.prot_msg );
    }
    ASSERT( itr->queue == SLP_XMIT_Q );
    itr = hdrslp_mp_free_msg( itr, err_num );
  }

  /* Also clear the fragmentation buffer. */
  if ( hdrslp_fragment_buffer )
  {
    (void) hdrslp_mp_free_msg( hdrslp_fragment_buffer, err_num );
  }

  hdrslp_leave_crit_sect( );

  if ( hdrslp_tx_queue_item_count )
  {
	 HDR_MSG_PROT_2( MSG_LEGACY_MED, "SLP TXQ: %d new items first item 0x%x ", 
                    hdrslp_tx_queue_item_count, hdrslp_tx_queue_first_item_ptr );
  }

} /* hdrslp_tx_queue_clear() */

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
  msg_ptr - Pointer to SLP msg buffer
  time    - time until callback needs to be processed
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
)
{
  hdrslp_msg_struct_type*  itr;
  hdrslp_msg_struct_type*  next_itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( msg_ptr )
  {
    /* Store the error code representing the current status */
    /* of the message transmission in the message structure */
    msg_ptr->tx_status = tx_status;

    /* Insert callbacks with a timeout to the waiting queue,            */
    /* so the timer update routine only needs to work on that queue.    */
    /* This function is called from RX and TX context, it is protected  */
    /* from the RX caller to avoid unecessary INTLOCKs.                 */

    next_itr = (hdrslp_msg_struct_type*) msg_ptr->next;

    if ( time )
    {
      if ( msg_ptr->queue == SLP_CB_WAIT_Q ) return next_itr;

      msg_ptr->next = hdrslp_cb_wait_q_ptr;
      hdrslp_cb_wait_q_ptr = msg_ptr;
      
      msg_ptr->queue = SLP_CB_WAIT_Q;
      msg_ptr->cb_time = time;
    }
    else
    {
      if ( msg_ptr->queue == SLP_CB_PROCESS_Q ) return next_itr;

      /* Check if msg needs to be removed from wait queue */
      if ( msg_ptr == hdrslp_cb_wait_q_ptr ) 
      {
        /* Common case */
        hdrslp_cb_wait_q_ptr = (hdrslp_msg_struct_type*) (msg_ptr->next);
        if ( msg_ptr->queue != SLP_CB_WAIT_Q )
        {
          HDR_MSG_PROT_2( MSG_LEGACY_MED,  "SLP TXQ: Wait q ptr 0x%x is %d",
                          msg_ptr, msg_ptr->queue );
        }
      }
      else
      {
        for ( itr = hdrslp_cb_wait_q_ptr; 
              itr != NULL; 
              itr = (hdrslp_msg_struct_type*) (itr->next) )
        {
          if ( itr->next == msg_ptr ) 
          {
            itr->next = msg_ptr->next; 
            if ( msg_ptr->queue != SLP_CB_WAIT_Q )
            {
              HDR_MSG_PROT_2( MSG_LEGACY_MED,  "SLP TXQ: Wait q ptr 0x%x is %d",
                              msg_ptr, msg_ptr->queue );
            }
            break;
          }
        }
      }

      hdrslptx_enter_crit_sect( &hdrslptx_crit_sect_cbpq );
      /* Add msg into processing queue */
      msg_ptr->next = hdrslp_cb_process_q_ptr;
      hdrslp_cb_process_q_ptr = msg_ptr;
      msg_ptr->queue = SLP_CB_PROCESS_Q;
      /* the assignment below is done in both if else block so that it stays 
       inside the critical section */
      msg_ptr->cb_time = time;
      hdrslptx_leave_crit_sect( &hdrslptx_crit_sect_cbpq );
      
      if (hdrslp_verbose_debug)
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH,  "Add msg to cb process Q");
      }
    }  /* end else(time) */
  } /* end if(msg_ptr)*/ 
  else
  {
    next_itr = NULL;
  }

  return next_itr;

} /* hdrslp_cb_enqueue() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_CB_WAIT_QUEUE_CLEAR

DESCRIPTION
  This function clears out any items in the cb_wait queue.

DEPENDENCIES
  None
  
PARAMETERS
  err_num - HDR error number to use in callbacks

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_cb_wait_queue_clear
(
  hdrerrno_enum_type err_num 
)
{
  hdrslp_msg_struct_type* itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We move buffers from the CB_WAIT_Q to the CB_PROCESS_Q because ACMAC
   * may not always be be able to update the timers (to zero) before we
   * come here.  This avoids potential race conditions.
   */
  itr = hdrslp_cb_wait_q_ptr; 
  while( itr != NULL )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH,  
                    "Clear CB_WAIT_Q (P 0x%x ID 0x%x)",
                    itr->sdu.prot_id, itr->sdu.prot_msg );
    ASSERT( itr->queue == SLP_CB_WAIT_Q );
    itr = hdrslp_cb_enqueue( itr, 0, err_num );
  }
} /* hdrslp_cb_wait_queue_clear() */

/* <EJECT> */ 
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

void hdrslp_cb_process( void )
{
  hdrslp_msg_struct_type* itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrslptx_enter_crit_sect( &hdrslptx_crit_sect_cbpq );
  if ( hdrslp_cb_process_q_ptr )
  {
    itr = hdrslp_cb_process_q_ptr;
    do
    {
      hdrslp_cb_process_q_ptr = itr->next;

      if (itr->tx_status == E_FAILURE)
      {
        /* Keep track of the number of SLP transmissions that failed for
           logging purposes. */
        hdrslp_log_inc_conn_state_stats_counter( 
          HDRSLP_TRANSMIT_FAILURE_COUNT,
          1 );
      }

      /* Unsuccessfull traffic channel messages should be retried */
      /* unless it was E_FAILURE, in which case it is important   */
      /* that the sending protocol know of the failure            */
      if ( (itr->tx_status != E_SUCCESS)
           &&
           (itr->tx_status != E_FAILURE)
           &&
           (itr->sdu.channel == HDRHAI_RTC) )
      {
         itr->tx_status = E_AGAIN;
      }

      /* Free msg buffer item */
      itr = hdrslp_mp_free_msg( itr, itr->tx_status );
    } while ( itr != NULL );
  
    hdrslp_cb_process_q_ptr = NULL;
  }
  hdrslptx_leave_crit_sect( &hdrslptx_crit_sect_cbpq );

#ifdef FEATURE_HDRSLP_REPORT_MSG_STATUS
  {
    int i;
  /* Checking if any message is stranded at this point. */
  for ( i=0; i<HDRSLP_MSG_POOL_SIZE; i++ )
  {
    if ((hdrslp_msg_pool[i].queue == SLP_CB_PROCESS_Q) )
    {
      HDR_MSG_PROT_3( MSG_LEGACY_FATAL, "found bug at p%x m%x t%d", 
                      hdrslp_msg_pool[i].sdu.prot_id, 
                      hdrslp_msg_pool[i].sdu.prot_msg, 
                      hdrslp_msg_pool[i].cb_time);
    }

    if ((hdrslp_msg_pool[i].queue == SLP_CB_WAIT_Q))
    {
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "msg still waiting p%x m%x t%d", 
                      hdrslp_msg_pool[i].sdu.prot_id, 
                      hdrslp_msg_pool[i].sdu.prot_msg, 
                      hdrslp_msg_pool[i].cb_time);
    }

    if ( (hdrslp_msg_pool[i].queue == SLP_REXMIT_Q) ||
          (hdrslp_msg_pool[i].queue == SLP_FRG_Q) ) 

    {
      if (( hdrslp_msg_pool[i].sdu.channel == HDRHAI_AC ))
      {
        HDR_MSG_PROT_3( MSG_LEGACY_FATAL, "found bug at p%x m%x t%d", 
                        hdrslp_msg_pool[i].sdu.prot_id, 
                        hdrslp_msg_pool[i].sdu.prot_msg, 
                        hdrslp_msg_pool[i].cb_time);
      }
      else
      {
        HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "tc msg still on q  p%x m%x q%d",
                        hdrslp_msg_pool[i].sdu.prot_id, 
                        hdrslp_msg_pool[i].sdu.prot_msg, 
                        hdrslp_msg_pool[i].queue);
      }
    }
  }

  }
#endif /* FEATURE_HDRSLP_REPORT_MSG_STATUS */

} /* hdrslp_cb_process() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_CB_FAILURE

DESCRIPTION
  This function loops through the list of pending callbacks and
  sets their tx status to the given error code.

DEPENDENCIES
  None
  
PARAMETERS
  err_num - HDR error number to use in callbacks

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_cb_failure
(
  hdrerrno_enum_type err_num 
)
{
  hdrslp_msg_struct_type* itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( itr = hdrslp_cb_process_q_ptr;
       itr != NULL;
       itr = (hdrslp_msg_struct_type*) (itr->next) )
  {
    itr->tx_status = err_num;
  }

} /* hdrslp_cb_failure() */

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

void hdrslp_mp_init_pool( void )
{
  int i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  for ( i=0; i< HDRSLP_MSG_POOL_SIZE; i++ )
  {
    hdrslp_msg_pool[i].queue = SLP_FREE_Q;
    (void) hdrslp_mp_init_msg( &hdrslp_msg_pool[i] );
  }

  hdrslp_tx_queue_first_item_ptr = NULL;
  hdrslp_tx_queue_item_count = 0;
  hdrslp_msg_pool_level = 0;

} /* hdrslp_mp_init_pool() */


/* <EJECT> */ 
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

hdrslp_msg_struct_type* hdrslp_mp_init_msg( hdrslp_msg_struct_type* msg_ptr )
{
  hdrslp_msg_struct_type* next_msg_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  next_msg_ptr = msg_ptr->next;

  if ( msg_ptr->sdu.msg_ptr )
  {
    dsm_free_packet( &(msg_ptr->sdu.msg_ptr) );
  }
  msg_ptr->priority         = 0xff;
  msg_ptr->reliable         = FALSE;
  msg_ptr->rexmit_time      = 0;
  msg_ptr->rexmit_count     = 0;
  msg_ptr->tx_status        = E_IN_PROGRESS;
  msg_ptr->outcome_func_ptr = NULL;
  msg_ptr->outcome_tag_ptr  = NULL;
  msg_ptr->cb_time          = 0;
  msg_ptr->queue            = SLP_FREE_Q;
  msg_ptr->next             = NULL;

  return next_msg_ptr;

} /* hdrslp_mp_init_msg() */
  
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_MP_GET_MSG

DESCRIPTION
  This function fetches a free SLP msg buffer

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  Pointer to an unused SLP msg buffer
  
SIDE EFFECTS
  None
===========================================================================*/

hdrslp_msg_struct_type* hdrslp_mp_get_msg( void )
{
  int i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hdrslptx_enter_crit_sect( &hdrslptx_crit_sect_mpfree );

  for ( i=0; i<HDRSLP_MSG_POOL_SIZE; i++ )
  {
    if ( hdrslp_msg_pool[i].queue == SLP_FREE_Q )
    {
      hdrslp_msg_pool[i].queue = SLP_NIRVANA_Q;
      hdrslp_msg_pool_level++;
	  hdrslptx_leave_crit_sect( &hdrslptx_crit_sect_mpfree );
      return &hdrslp_msg_pool[i];
    }
  }
  hdrslptx_leave_crit_sect( &hdrslptx_crit_sect_mpfree ); 
  return NULL;

} /* hdrslp_mp_get_msg() */

/* <EJECT> */ 
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
    a) The message was aborted due to Access Handoff or Access Entry Handoff
       [XFP]
    b) The connection setup in progress was aborted by the user   
  
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
       connected -> idle or init
       idle -> init
       init -> init
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
    g) a session was closed [XRFWP]
    h) ALMP made any of the following state transitions [XRFWP]
       connected -> inactive  
       idle -> inactive
       init -> inactive
    
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
)
{
  hdrslp_msg_struct_type* n_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  hdrslptx_enter_crit_sect( &hdrslptx_crit_sect_mpfree );

  if ( msg_ptr && msg_ptr->queue != SLP_FREE_Q)
  {
    ASSERT ((tx_status != E_IN_PROGRESS) &&
            ((msg_ptr->sdu.channel == HDRHAI_AC) || (tx_status != E_ABORTED)));

    if ( msg_ptr->queue == SLP_XMIT_Q )
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "SLPF: Freeing xmit q item 0x%x",
                      msg_ptr );
      hdrslp_tx_queue_item_count--;
    }

    /* This will catch fragmented messages - it's the       */
    /* cleanest spot to deal with those since we need this  */
    /* check for sanity anyhow.                             */
    if ( msg_ptr == hdrslp_fragment_buffer )
    {
      hdrslp_fragment_buffer   = NULL;
      hdrslp_fragment_header   = 0;
      hdrslp_fragment_pointer  = 0;
    }

    /* Call non-NULL callback function */
    if ( msg_ptr->outcome_func_ptr ) 
    {
      HDR_MSG_PROT_2( MSG_LEGACY_MED, "SLPF: Calling cb 0x%x( %d )", 
                      msg_ptr, tx_status );
      (msg_ptr->outcome_func_ptr)( msg_ptr->outcome_tag_ptr, tx_status );
    }

    if (hdrslp_verbose_debug)
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "MP free: 0x%x",
                    msg_ptr->sdu.msg_ptr );
    }

    hdrslptx_enter_crit_sect( &hdrslptx_crit_sect_dsmfree );           
    if ( msg_ptr->sdu.msg_ptr ) 
    {
      dsm_free_packet( &(msg_ptr->sdu.msg_ptr) );
    }
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "mp_free_msg: msg already released!");
    }
    hdrslptx_leave_crit_sect( &hdrslptx_crit_sect_dsmfree );

    n_ptr = hdrslp_mp_init_msg( msg_ptr );
    hdrslp_msg_pool_level--;
  }

  hdrslptx_leave_crit_sect( &hdrslptx_crit_sect_mpfree );
	
  return n_ptr;

} /* hdrslp_mp_free_msg() */

/* <EJECT> */ 
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

void hdrslp_tx_set_waiting_for_bundling_flag
( 
  boolean waiting_for_bundling
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "BUNDLE: waiting for bundle flag %d", 
                  waiting_for_bundling );

  hdrslp_waiting_for_bundling = waiting_for_bundling;
} /* hdrslp_tx_set_waiting_for_bundling_flag() */

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

boolean hdrslp_tx_is_waiting_for_bundling(void)
{
  return (hdrslp_waiting_for_bundling);
} /* hdrslp_tx_is_waiting_for_bundling() */

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

void hdrslp_tx_stop_waiting_for_uati_assignment(void)
{
  hdrslp_waiting_for_uati_assignment = FALSE;
} /* hdrslp_tx_stop_waiting_for_uati_assignment() */
/*===========================================================================

FUNCTION HDRSLP_TX_IS_WAITING_FOR_UATI_ASSIGNMENT

DESCRIPTION
  This is wrapped by hdrslp_is_waiting_for_uati_assignment, see that  
  function header for details.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

boolean hdrslp_tx_is_waiting_for_uati_assignment(void)
{
  return (hdrslp_waiting_for_uati_assignment);
} /* hdrslp_tx_is_waiting_for_uati_assignment() */

#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

/*===========================================================================

FUNCTION HDRSLPTX_ENTER_CRIT_SECT

DESCRIPTION
  This enters SLP TX critcal section to protect SLP TX data struct
  
DEPENDENCIES
  None

PARAMETERS
  Critical section pointer variable

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void hdrslptx_enter_crit_sect( rex_crit_sect_type *crit_sect )
{
  if ( !crit_sect )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "hdrslptx_enter_crit_sect:\
                    Null Critical section passed from caller tid:%x", 
                    rex_self() );
    return;
  }
  if ( hdrslp_verbose_debug )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED, "SLPTX enter crit task:%x tid:%x", 
                    crit_sect, rex_self() );
  }

  rex_enter_crit_sect(crit_sect);
}

/*===========================================================================

FUNCTION HDRSLPTX_LEAVE_CRIT_SECT

DESCRIPTION
  This leaves SLP TX critcal section
  
DEPENDENCIES
  Must be called after enter_crit_sect and must be matched in pairs.

PARAMETERS
  Critical section pointer variable

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void hdrslptx_leave_crit_sect( rex_crit_sect_type *crit_sect )
{
  if ( !crit_sect )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "hdrslptx_leave_crit_sect:\
                    Null Critical section passed from caller tid:%x", 
                    rex_self() );
    return;
  }
  rex_leave_crit_sect(crit_sect);

  if ( hdrslp_verbose_debug )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED, "SLP TX leave crit task::%x tid:%x", 
                    crit_sect, rex_self() );
  }
  }

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
void hdrslptx_init_crit_sect(void)
{
  rex_init_crit_sect(&hdrslptx_crit_sect_dsmfree);
  rex_init_crit_sect(&hdrslptx_crit_sect_cbpq);
  rex_init_crit_sect(&hdrslptx_crit_sect_mpfree);
}

