/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      R A D I O   L I N K   P R O T O C O L  N A K  P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains HDR nak-list processing functions. 
  
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Library calls.  Assumes that the nak list passed in has been initialized
  (q_init), and that the nak free q has been initialized also.
  
DESCRIPTION
  The NAK list contains entries, each of which details the missing frames
  that are missing.  When inserting, the user should do:
  
  insert(a,b),  where a is the first missing frame number (inclusive) and
  b is one greater than the last missing frame number (so it is exclusive).
  
  remove(a,b), is a little different (a is inclusive, b is also inclusive)
  
 Copyright (c) 2005-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrlpnak.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        ------------------------------------------------------- 
09/30/13   sc         Added critical section for shared data to prevent race
                      condition issues
10/03/12   sd         Memory Reduction modification
04/20/12   msh        Fix prev check in 
03/21/12   msh        TCB changes due to Core changes  
01/02/12   msh        Coan: Feature cleanup
11/18/11   vpk        HDR SU API cleanup
08/16/11   vpk        Move hdr log packet definitions to hdrdata
07/26/11   vpk        Remove unused HDR headers
02/08/11   ls         Global variable cleanup
03/11/10   spn        Changed to use uptime instead of system time. To 
                      overcome TOD issues. 
02/10/10   spn        Clear up the NAK elements upto the Vn
03/05/08   hs         Fixed Featurization for RLP Logging
12/17/07   da         Fixed Abort stats
05/18/07   spn        Fixed Compiler Warnings
05/08/07   da         EMPA: change to avoid going through nak-q for every packet
05/08/07   da         Removed function hdrrlpnak_remove as its no longer used
06/06/06   ksu        EMPA: RLP Rx / Sig code review comments
05/30/06   ksu        EMPA: RLP init code review comments
11/28/05   ksu        CR 81596: Rev 0 data call fails all the time with dsrlpsq
                      crash during bi-directional data transfer
11/17/05   ksu        fix reentrancy problem in hdrrlpnak_age_list()
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)
09/28/05   ksu        HDR: use one-shot abort timer instead of 20ms ticks

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"
#include "hdrrlp_api.h"

#if defined(FEATURE_HDR)

#include "dsrlpi.h"
#include "hdrrlprsq.h"
#include "hdrrlpseq.h"
#include "hdrrlpnak.h"
#include "hdrcom_api.h"
#include "task.h"
#include "amssassert.h"
#include "err.h"
#include "queue.h"
#include "rlp_event_defs.h"
#include "time_svc.h"
#include "qw.h"
#include "hdrrlp_util.h"

#ifdef FEATURE_HDR_REVB
#include "hdrrlpdnak.h"
#endif
#include "hdrrlplog.h"
#include "hdrrlpi.h"
#include "hdrrlp.h"

#include "modem_mem.h"



/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for NAK list entry. The NAK list is used to keep track of missing
  RLP data frames, and the associated abort timers.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type    link;       /* Queue link used to queue/dequeue the entry */
  uint32         first_seq;  /* 1st missing Sequence number                */
  uint32         last_seq;   /* last missing Sequence number               */
  time_type      expiry;     /* when abort timer is due to expire          */
  boolean        nak_sent;   /* TRUE if NAK has been sent                  */
} hdrrlp_nak_list_entry_type;

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        MACRO DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================

MACRO          HDRRLPNAK_INSERT_ENTRY

DESCRIPTION    Inserts an entry into the nak list.  Done for code cleanliness
               below.
               
               3 params:  x, y, z
                          x = ptr to list in which to insert
                          y = ptr to element to insert
                          z = ptr to element to insert in front of
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
/* Macro is not being used, commented out for lint
#ifdef FEATURE_Q_NO_SELF_QPTR                                              
#define HDRRLPNAK_INSERT_ENTRY( x, y, z)                                    \
          q_insert(x,&(y->link),&(z->link));                               
#else                                                                      
#define HDRRLPNAK_INSERT_ENTRY( x, y, z)                                    \
          q_insert(&(y->link),&(z->link));                                 
#endif                                                                     
*/


/*===========================================================================

MACRO          HDRRLPNAK_DELETE_ENTRY

DESCRIPTION    Deletes an entry from the nak list, and then puts that
               element into the global nak free queue.
               
               2 params:  x, y
                          x = ptr to list from which to delete
                          y = ptr to element to delete
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
#ifdef FEATURE_Q_NO_SELF_QPTR                                              
#define HDRRLPNAK_DELETE_ENTRY(x,y)                                         \
{                                                                          \
          q_delete(x,&(y->link));                                          \
  modem_mem_free(&(y->link),MODEM_MEM_CLIENT_DATA_CRIT);            \
}
#else                                                                      
#define HDRRLPNAK_DELETE_ENTRY(x,y)                                         \
{                                                                          \
          q_delete(&(y->link));                                            \
  modem_mem_free(&(y->link),MODEM_MEM_CLIENT_DATA_CRIT);            \
}
#endif                                                                     


/*===========================================================================

MACRO          HDRRLPNAK_GET_NEXT_ENTRY

DESCRIPTION    Gets the next entry from the nak list, given the current
               entry.  This is here so the code in the function is
               readable.
               
               2 params:   q_ptr     = ptr to queue
                           entry_ptr = ptr to current entry
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
#define HDRRLPNAK_GET_NEXT_ENTRY(q_ptr,entry_ptr) \
     ((hdrrlp_nak_list_entry_type *)q_next(q_ptr,&(entry_ptr->link)))


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                     INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       HDRRLPNAK_ABORT_TIMER_EXPIRED

DESCRIPTION    Go through the list and age the nak list entries.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
static void hdrrlpnak_abort_timer_expired
(
  unsigned long param
)
{
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr;    /* current RLP ctl blk         */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type*)param;
  ASSERT(rrcb_ptr != NULL);

  rrcb_ptr->state.abort_timer_expired = TRUE;
  (void) rex_set_sigs( hdrrlp_util_get_hdrrx_tcb(), HDRL1_RX_RLP_ABT_TIMER_SIG );

}

/*===========================================================================

FUNCTION       HDRRLPNAK_RESET_ABORT_TIMER

DESCRIPTION    Reset the abort timer for the next entry, if any
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
static void hdrrlpnak_reset_abort_timer
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr         /* current RLP session ctl blk*/
)
{
  hdrrlp_nak_list_entry_type  *nak_entry_ptr;  /*   nak list entry         */

  ASSERT(rrcb_ptr != NULL);

  nak_entry_ptr = (hdrrlp_nak_list_entry_type *)q_check(&rrcb_ptr->state.nak_q);
  if (nak_entry_ptr != NULL) 
  {
    time_type now, difference;
    time_get_uptime_ms(now);

    /* Delay the extended timer definition so the callback will happen */
    /* in the context of the RX task (hdrrlpnak_init() happens in DS) */
    if (!rrcb_ptr->abort_timer_def) 
    {
      rex_def_timer_ex( &rrcb_ptr->abort_timer, 
                        hdrrlpnak_abort_timer_expired,
                        (unsigned long)rrcb_ptr );
      rrcb_ptr->abort_timer_def = TRUE;
    }

    /* If now >= expiry */
    if ( qw_cmp ( now, nak_entry_ptr->expiry ) != -1 ) 
    {
      (void)rex_set_timer(&rrcb_ptr->abort_timer, 0);
    } else 
    {
      /* difference = expiry - now */
      qw_sub( difference, nak_entry_ptr->expiry, now ); 
      (void)rex_set_timer( &rrcb_ptr->abort_timer, 
                           (dword)(qw_lo(difference)));
    }
  }
}

/*===========================================================================

FUNCTION       HDRRLPNAK_AGE_LIST

DESCRIPTION    Go through the list and age the nak list entries.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
static void hdrrlpnak_age_list
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr       /* current RLP ctl blk      */
)
{
  q_type                       *nak_q_ptr;     /*  ptr to rlp's nak list   */
  hdrrlp_nak_list_entry_type   *nak_entry_ptr; /*  nak list entry          */
  hdrrlp_nak_list_entry_type   *temp_ptr;      /*  nak list entry          */
  dsrlpi_state_info_type       *state_ptr;     /*  curr state              */ 
  uint32                        modulus;       /* sequence modulus         */
  uint32                        old_vn;        /* the old V(N)             */
  uint32                       first,last;     /* dummy place holders      */
  event_rlp_payload_type   rlp_event_payload;  /* payload for RLP events   */
  event_rlp_nak_abort_payload_type rlp_nak_abort_event_payload; 
  time_type                       now;            /* current time             */ 
  hdrrlp_debug_ctl *hdrrlp_debug_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);

  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();

  /*------------------------------------------------------------------------
    Set up some local vars.  These are used more for cleanliness of code,
    so that some statements can fit on one line
    ------------------------------------------------------------------------*/
  state_ptr = &(rrcb_ptr->state);
  nak_q_ptr = &(state_ptr->nak_q);
  modulus   = rrcb_ptr->cfg_ptr->seq_modulus;

  rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
  temp_ptr = (hdrrlp_nak_list_entry_type *)q_check(nak_q_ptr);

  time_get_uptime_ms(now);
  while (temp_ptr != NULL)
  {
    /*---------------------------------------------------------------------
      Within the loop, it is possible for the current nak entry to be
      deleted. Therefore, make a copy of the ptr to the current entry, and
      then immediately get the next entry.  This way, the current entry can
      be deleted safely.  (Otherwise, once the current entry is deleted, it
      becomes impossible to find the next entry).
      ---------------------------------------------------------------------*/ 
    nak_entry_ptr = temp_ptr;
    temp_ptr = HDRRLPNAK_GET_NEXT_ENTRY(nak_q_ptr,temp_ptr);

    /*---------------------------------------------------------------------
      Since this routine goes through every entry in the list, this is a
      good place to confirm that all RLP entries are sytactically correct:
      
      4  -> 7    GOOD
      4  -> 4    INCORRECT
      4  -> 3    INCORRECT (assuming no modulus rollover)
      ---------------------------------------------------------------------*/ 
    first = nak_entry_ptr->first_seq;
    last  = nak_entry_ptr->last_seq;

    ASSERT( (last != first) &&
            (HDRRLPSEQ_GE_SEQ_CHECK(last,first,modulus)) );


    /* If now > expiry */
    if ( ( qw_cmp ( now, nak_entry_ptr->expiry ) != -1 ) ||
         (HDRRLPSEQ_GE_SEQ_CHECK(state_ptr->needed_seq_num,
                                nak_entry_ptr->last_seq,
                                modulus ) ) )
    {
      /*---------------------------------------------------------------------
        Nak entry has expired. Check to see if the hole corresponding
        to this Nak entry was filled or not. If not, do Abort processing
        ---------------------------------------------------------------------*/
      if (HDRRLPSEQ_GE_SEQ_CHECK(state_ptr->needed_seq_num,
                                nak_entry_ptr->last_seq,
                                modulus) )
      {
        if (hdrrlp_debug_ptr->enable_rx_debug) 
        {
          /* Do nothing since the hole was filled before this timer expires */
          DATA_RLP_MSG6( MSG_LEGACY_MED, 
                          "RLP Rx[%d,%d] V[%d,%d] hole 0x%x to 0x%x filled before AbortTimer expired",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced, 
                          nak_entry_ptr->first_seq,
                          nak_entry_ptr->last_seq);
        }
      }
      else 
      {
        /* Nak Abort processing */
        DATA_RLP_MSG6( MSG_LEGACY_HIGH, 
                        "RLP Rx[%d,%d] V[%d,%d] NAK ABORT on 0x%x to 0x%x",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced,
                        state_ptr->needed_seq_num,
                        nak_entry_ptr->last_seq );

        /* update the stats */
        rrcb_ptr->stats.aborts++;
        rrcb_ptr->stats.nak_abort_data_units_approx += 
          (uint32) HDRRLPSEQ_DIFF_SEQ( nak_entry_ptr->last_seq,
                              state_ptr->needed_seq_num,
                              modulus );

        /* Move the available contiguous data to higher layers */
        old_vn   = state_ptr->needed_seq_num;
        if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
        {
          /* Update the rx processing stats to be logged after abort processing is done */
          rrcb_ptr->state.rx_proc_state.event_type = DSRLPI_RX_PROC_EVENT_ABORT;
          rrcb_ptr->state.rx_proc_state.vn_before = rrcb_ptr->state.needed_seq_num;
          rrcb_ptr->state.rx_proc_state.vr_before = rrcb_ptr->state.expected_seq_num;
        }

        /* Move Vn past the hole */
        state_ptr->needed_seq_num = nak_entry_ptr->last_seq;

        /* Go through the reseq_q and update V(N) to beyond the last recvd contiguous data unit */
        hdrrlprsq_update_vn_upon_nak_abort( rrcb_ptr );
    
        if (old_vn != state_ptr->needed_seq_num)
        {
          DATA_RLP_MSG6( MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] V(n) moved 0x%x to 0x%x",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced,
                          old_vn, 
                          state_ptr->needed_seq_num );
                          
          hdrrlprsq_remove(rrcb_ptr);
        
        #ifdef FEATURE_HDRRLP_LOGGING
          if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
          {
            rrcb_ptr->state.rx_proc_state.vn_after = rrcb_ptr->state.needed_seq_num;
            rrcb_ptr->state.rx_proc_state.vr_after = rrcb_ptr->state.expected_seq_num;
            
            /* Log the Fwd RX Processing log type */
            hdrrlplog_multilink_fwd_log_rx_processing( rrcb_ptr );
          }
        #endif /* FEATURE_HDRRLP_LOGGING */
        }
    
        /*---------------------------------------------------------------------
          Report the RLP NAK abort event 
        ---------------------------------------------------------------------*/
        rlp_event_payload.event_rlp_type = event_rlp_nak_abort;
        rlp_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
        event_report_payload (EVENT_RLP, 
                              sizeof(event_rlp_payload_type),
                              &rlp_event_payload);
            
        rlp_nak_abort_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
        rlp_nak_abort_event_payload.first_seq = nak_entry_ptr->first_seq;
        rlp_nak_abort_event_payload.last_seq  = nak_entry_ptr->last_seq;
        event_report_payload (EVENT_RLP_NAK_ABORT, 
                              sizeof(event_rlp_nak_abort_payload_type),
                              &rlp_nak_abort_event_payload);

      } /* end of Nak Abort processing */

      HDRRLPNAK_DELETE_ENTRY(&(state_ptr->nak_q),nak_entry_ptr);
    }
    else
    {
      /* Reached an entry which has not yet expired. No need to look further */
      break;
    }
  } /* while */

  rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
  
  hdrrlpnak_reset_abort_timer(rrcb_ptr);
}

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    PUBLIC FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================

FUNCTION       HDRRLPNAK_INSERT

DESCRIPTION    Insert this new entry into the nak list.  Inserted according
               to sequence number.  Nak entry is filled out with first nak
               round information.
               
               If entry already exists, then nothing is done.
               
               Insert(a,b):  a is the first missing frame, inclusive.
               b is the sequence number of the frame just received.
               
DEPENDENCIES   Input rscb_ptr is non-null.

RETURN VALUE   TRUE if the information could be inserted, else FALSE.

SIDE EFFECTS   NONE
===========================================================================*/
boolean hdrrlpnak_insert
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        first_num,     /* first missing seq num    */
  uint32                        last_num       /* last missing seq num     */
)
{
  q_type                       *nak_q_ptr;     /*  ptr to rlp's nak list   */
  hdrrlp_nak_list_entry_type   *nak_entry_ptr; /*  nak list entry          */
  hdrrlp_nak_list_entry_type   *temp_ptr;      /*  nak list entry          */
  dsrlpi_state_info_type       *state_ptr;     /*  curr state              */ 
  uint32                        modulus;       /* sequence modulus         */
  uint32                        nak_q_cnt;     /* cnt of nodes in nak_q    */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT((rrcb_ptr != NULL) &&
         (first_num != last_num));              /* must be different numbers */
  
  /*-------------------------------------------------------------------------
    Set up some local vars.  These are used more for cleanliness of code,
    so that some statements can fit on one line
    -------------------------------------------------------------------------*/
  state_ptr = &(rrcb_ptr->state);
  nak_q_ptr = &(state_ptr->nak_q);
  modulus   = rrcb_ptr->cfg_ptr->seq_modulus;

  /*------------------------------------------------------------------------
    Nak entries either already exist in the list or are inserted at the end
    (always).  See which case applies.  If new, make sure that new entry is
    greater than last entry in list.  Also possible that list is empty, in
    which case the entry is automatically inserted.
    ------------------------------------------------------------------------*/
  temp_ptr = (hdrrlp_nak_list_entry_type *)q_last_check(nak_q_ptr);
  if (temp_ptr != NULL)
  {
    /*---------------------------------------------------------------------
      It is possible for the last seq of the prev entry to equal the first
      seq of this entry. (since the last_seq of prev entry is an exclusive
      value, and the first_seq of this entry is an inclusive value). 
      ---------------------------------------------------------------------*/
    if (HDRRLPSEQ_GT_SEQ_CHECK(temp_ptr->last_seq,
                              first_num,
                              modulus))
    {
      /*---------------------------------------------------------------------
        If here, than seq number is less than last one in our list. That is 
        an error.  Differing nak entries should never overlap.  Put back 
        the old entry, and return.  Still return TRUE, as this is not a case
        to cause RLP reset.
        ---------------------------------------------------------------------*/
      DATA_RLP_MSG4( MSG_LEGACY_ERROR, 
                      "RLP Rx[%d,%d] V[%d,%d] Trying to insert old NAK info into list",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced );
      return(TRUE);
    }
  }/* temp_ptr != NULL */
  

  /*-------------------------------------------------------------------
    Insert a nak entry.  Have to get one from the free queue.  
    -------------------------------------------------------------------*/
  nak_entry_ptr = (hdrrlp_nak_list_entry_type *)modem_mem_alloc(sizeof(hdrrlp_nak_list_entry_type),MODEM_MEM_CLIENT_DATA_CRIT);
  if (nak_entry_ptr == NULL)
  {
    DATA_RLP_MSG0( MSG_LEGACY_ERROR, "Ran out of nak items in free q" );
    return FALSE;
  }
  memset(nak_entry_ptr, 0, sizeof(hdrrlp_nak_list_entry_type));

  time_get_uptime_ms(nak_entry_ptr->expiry);
  
  qw_inc( nak_entry_ptr->expiry, 
          rrcb_ptr->cfg_ptr->nak_abort_timer_val );
  nak_entry_ptr->first_seq = first_num;
  nak_entry_ptr->last_seq  = last_num;

  /*-------------------------------------------------------------------
    Do not send a nak immediately if this is MMPA. Instead add a node
    into the delayed_nak_q to wait for delayed_nak timer to expire
    before sending a multilink nak.
    -------------------------------------------------------------------*/
  if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
  {
    /* Set the nak_sent to TRUE for this node, since we dont want an immediate NAK 
       for this hole. This is checked by hdmrlpsig to see if Nak has been sent. */
    nak_entry_ptr->nak_sent  = TRUE;

    if (!hdrrlpdnak_insert(rrcb_ptr, first_num, last_num))
    {
      DATA_RLP_MSG4( MSG_LEGACY_ERROR, 
                      "RLP Rx[%d,%d] V[%d,%d] Could not insert into Delayed NAK list",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced );

      hdrrlpi_mobile_initiated_reset(rrcb_ptr);
    }
  }
  else  /* Not MMPA, send nak immediately */
  {
    nak_entry_ptr->nak_sent  = FALSE;
    state_ptr->naks_to_send  = TRUE;
  }
  
  q_put(nak_q_ptr,&nak_entry_ptr->link);

  hdrrlpnak_reset_abort_timer(rrcb_ptr);

  /* Update stats with q_cnt */
  nak_q_cnt = (uint32) q_cnt( nak_q_ptr );
  if (nak_q_cnt > rrcb_ptr->stats.rx_max_nak_abort_q_size)
  {
    rrcb_ptr->stats.rx_max_nak_abort_q_size = nak_q_cnt;
  }

  return TRUE;

} /* hdrrlpnak_insert() */

/*===========================================================================

FUNCTION       HDRRLPNAK_INSERT_ABORT

DESCRIPTION    Insert this new entry into the nak list.  Inserted according
               to sequence number. This marks the entry as nak_sent and is
               used only as a nak-abort node. To be used when 
               we need to start abort timer for missing bytes without sending 
               a nak message. eg. Flush msg processing in MMPA.

               If entry already exists, then nothing is done.
               
               Insert(a,b):  a is the first missing frame, inclusive.
               b is the sequence number of the frame just received.
               
DEPENDENCIES   Input rscb_ptr is non-null.

RETURN VALUE   TRUE if the information could be inserted, else FALSE.

SIDE EFFECTS   NONE
===========================================================================*/
boolean hdrrlpnak_insert_abort
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        first_num,     /* first missing seq num    */
  uint32                        last_num       /* last missing seq num     */
)
{
  q_type                       *nak_q_ptr;     /*  ptr to rlp's nak list   */
  hdrrlp_nak_list_entry_type   *nak_entry_ptr; /*  nak list entry          */
  hdrrlp_nak_list_entry_type   *temp_ptr;      /*  nak list entry          */
  dsrlpi_state_info_type       *state_ptr;     /*  curr state              */ 
  uint32                        modulus;       /* sequence modulus         */
  uint32                        nak_q_cnt;     /* cnt of nodes in nak_q    */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT((rrcb_ptr != NULL) &&
         (first_num != last_num));              /* must be different numbers */
  
  /*-------------------------------------------------------------------------
    Set up some local vars.  These are used more for cleanliness of code,
    so that some statements can fit on one line
    -------------------------------------------------------------------------*/
  state_ptr = &(rrcb_ptr->state);
  nak_q_ptr = &(state_ptr->nak_q);
  modulus   = rrcb_ptr->cfg_ptr->seq_modulus;

  /*------------------------------------------------------------------------
    Nak entries either already exist in the list or are inserted at the end
    (always).  See which case applies.  If new, make sure that new entry is
    greater than last entry in list.  Also possible that list is empty, in
    which case the entry is automatically inserted.
    ------------------------------------------------------------------------*/
  temp_ptr = (hdrrlp_nak_list_entry_type *)q_last_check(nak_q_ptr);
  if (temp_ptr != NULL)
  {
    /*---------------------------------------------------------------------
      It is possible for the last seq of the prev entry to equal the first
      seq of this entry. (since the last_seq of prev entry is an exclusive
      value, and the first_seq of this entry is an inclusive value). 
      ---------------------------------------------------------------------*/
    if (HDRRLPSEQ_GT_SEQ_CHECK(temp_ptr->last_seq,
                              first_num,
                              modulus))
    {
      /*---------------------------------------------------------------------
        If here, than seq number is less than last one in our list. That is 
        an error.  Differing nak entries should never overlap.  Put back 
        the old entry, and return.  Still return TRUE, as this is not a case
        to cause RLP reset.
        ---------------------------------------------------------------------*/
      DATA_RLP_MSG4( MSG_LEGACY_ERROR, 
                      "RLP Rx[%d,%d] V[%d,%d] Trying to insert old NAK info into list",
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced );
      return(TRUE);
    }
  }/* temp_ptr != NULL */
  

  /*-------------------------------------------------------------------
    Insert a nak entry.  Have to get one from the free queue.  
    -------------------------------------------------------------------*/
  nak_entry_ptr = (hdrrlp_nak_list_entry_type *)modem_mem_alloc(sizeof(hdrrlp_nak_list_entry_type),MODEM_MEM_CLIENT_DATA_CRIT);
  if (nak_entry_ptr == NULL)
  {
    DATA_RLP_MSG0( MSG_LEGACY_ERROR, "Ran out of nak items in free q" );
    return FALSE;
  }
  memset(nak_entry_ptr, 0, sizeof(hdrrlp_nak_list_entry_type));

  time_get_uptime_ms(nak_entry_ptr->expiry);

  qw_inc( nak_entry_ptr->expiry, 
          rrcb_ptr->cfg_ptr->nak_abort_timer_val );
  nak_entry_ptr->first_seq = first_num;
  nak_entry_ptr->last_seq  = last_num;

  nak_entry_ptr->nak_sent  = TRUE;
  
  q_put(nak_q_ptr,&nak_entry_ptr->link);

  hdrrlpnak_reset_abort_timer(rrcb_ptr);

  /* Update stats with q_cnt */
  nak_q_cnt = (uint32) q_cnt( nak_q_ptr );
  if (nak_q_cnt > rrcb_ptr->stats.rx_max_nak_abort_q_size)
  {
    rrcb_ptr->stats.rx_max_nak_abort_q_size = nak_q_cnt;
  }

  return TRUE;

} /* hdrrlpnak_insert() */

/*===========================================================================

FUNCTION       HDRRLPNAK_NAK_WAS_SENT

DESCRIPTION    Tells the nak list if a particular nak was sent over the air.

               The input paramenter is the id given by hdrrlpnak_get_next().

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the TX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpnak_nak_was_sent
(
  const dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        nak_id         /* nak id of the nak that   */
                                               /* was sent                 */
)
{
  hdrrlp_nak_list_entry_type   *nak_entry_ptr; /*  nak list entry          */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( rrcb_ptr != NULL );

  nak_entry_ptr = (hdrrlp_nak_list_entry_type *)nak_id;

  nak_entry_ptr->nak_sent = TRUE;
}

/*===========================================================================

FUNCTION       HDRRLPNAK_GET_NEXT

DESCRIPTION    Returns an id for the next nak entry which requires a NAK to
               be sent.
               
               The entry is returned as a uint32's.  The TX task is assumed
               to only use functions/macros found in hdrrlpnak.c/.h to
               access nak entry info.  As such, it is not important for the
               calling task to know what type the nak_id is.  
               
               Input requires the user to input "curr_nak_id".  If this
               value is zero, then the very first nak entry (requiring
               nak transmission) is returned.  Else, this id refers to a
               nak list entry; in this case the id of nak entry after the
               referenced entry is returned.
               
DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the TX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.  

RETURN VALUE   An id for the next nak entry which requires a nak xmission.
               If the return value is zero, then there are no more nak
               entries requiring transmission.

SIDE EFFECTS   NONE
===========================================================================*/
uint32 hdrrlpnak_get_next
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        curr_nak_id,   /* current nak id           */
  uint32                       *first_seq_ptr, /* first seq num            */
  uint32                       *last_seq_ptr   /* last seq num             */
)
{
  q_type                       *nak_q_ptr;     /*  ptr to rlp's nak list   */
  hdrrlp_nak_list_entry_type   *temp_ptr;      /*  nak list entry          */
  dsrlpi_state_info_type       *state_ptr;     /*  curr state              */ 
  uint32                        ret_id = 0;    /*  id of next nak entry    */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT(rrcb_ptr    != NULL);

  /*------------------------------------------------------------------------
    Set up some local vars.  These are used more for cleanliness of code,
    so that some statements can fit on one line
    ------------------------------------------------------------------------*/
  state_ptr = &(rrcb_ptr->state);
  nak_q_ptr = &(state_ptr->nak_q);

  /*------------------------------------------------------------------------
    look for the next nak entry requiring transmission.  Reference off of
    the input curr_nak_id.
    ------------------------------------------------------------------------*/
  if (curr_nak_id == 0)
  {
    temp_ptr = (hdrrlp_nak_list_entry_type *)q_check(nak_q_ptr);
  }
  else
  {
    temp_ptr = (hdrrlp_nak_list_entry_type *)curr_nak_id;
    temp_ptr = HDRRLPNAK_GET_NEXT_ENTRY(nak_q_ptr,temp_ptr);
  }

  /*------------------------------------------------------------------------
    loop until the first nak entry requiring transmission is found.
    ------------------------------------------------------------------------*/
  while (temp_ptr != NULL)
  {
    if (temp_ptr->nak_sent != TRUE)
    {
      ret_id = (uint32)temp_ptr;
      if (first_seq_ptr != NULL) 
      {
        *first_seq_ptr = temp_ptr->first_seq;
      }
      if (last_seq_ptr != NULL) 
      {
        *last_seq_ptr = temp_ptr->last_seq;
      }
      break;                                                /* all done */
    }
    else
    {
      temp_ptr = HDRRLPNAK_GET_NEXT_ENTRY(nak_q_ptr,temp_ptr);
    }
  }

  return ret_id;
}

/*===========================================================================

FUNCTION       HDRRLPNAK_GET_ENTRY

DESCRIPTION    Given an input sequence number, finds the SEQ for that number.
               
               The seq_num_ptr is updated with the found seq
               number (if it is found).  The input nak_id is updated with
               a reference point, so that if hdrrlpnak_remove is called
               with it, it can be more efficient.

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the RX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
RETURN VALUE   TRUE if frame is in the nak list.  FALSE otherwise.  If 
               the frame is in the list, then the seq is filled with the
               seq_hi (if unknown upon entry).  The nak_id is filled with
               a reference to the nak entry which contains the seq.
===========================================================================*/
boolean hdrrlpnak_get_entry
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* curr RLP ctl blk         */
  uint32                        seq_num,       /* current sequence number  */
  word                          length,        /* byte-length of data      */
  uint32                       *nak_id         /* filled with ref pt       */
)
{
  uint32                        last_num;
  hdrrlp_nak_list_entry_type   *temp_ptr = NULL;/*  nak list entry         */
  q_type                       *nak_q_ptr;      /*  ptr to rlp's nak list  */
  uint32                        modulus;        /*  seq modulus            */
  boolean                       ret_val = FALSE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);
  
  /*-------------------------------------------------------------------------
    Set up some local vars.  These are used more for cleanliness of code,
    so that some statements can fit on one line
    -------------------------------------------------------------------------*/
  nak_q_ptr = &(rrcb_ptr->state.nak_q);
  modulus   = rrcb_ptr->cfg_ptr->seq_modulus;
  last_num = HDRRLPSEQ_ADD_SEQ(seq_num, length, modulus);

  temp_ptr = (hdrrlp_nak_list_entry_type *)q_check(nak_q_ptr);
  while (temp_ptr != NULL)
  {
    if ((HDRRLPSEQ_GE_SEQ_CHECK(temp_ptr->first_seq,last_num,modulus)) ||
      (HDRRLPSEQ_GE_SEQ_CHECK(seq_num,temp_ptr->last_seq,modulus)))
    {
      temp_ptr = HDRRLPNAK_GET_NEXT_ENTRY(nak_q_ptr,temp_ptr);
    }
    else
    {
      ret_val = TRUE;
      break;
    }
  } /* while */

  if (ret_val == FALSE)
  {
    ASSERT(temp_ptr == NULL);
  }

  *nak_id = (uint32)temp_ptr;
  return (ret_val);
} 

/*===========================================================================

FUNCTION       HDRRLPNAK_RESET

DESCRIPTION    Called when an RLP instance is reset.  This takes any nak list
               entries from that RLP instance and puts them onto the 
               global free queue.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpnak_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
  q_type                      *nak_q_ptr;      /*   ptr to rlp's nak list  */
  hdrrlp_nak_list_entry_type  *nak_entry_ptr;  /*   nak list entry         */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);

  nak_q_ptr = &(rrcb_ptr->state.nak_q);

  while ((nak_entry_ptr = 
          (hdrrlp_nak_list_entry_type *)q_get(nak_q_ptr)) != NULL)
  {
    modem_mem_free(&nak_entry_ptr->link,MODEM_MEM_CLIENT_DATA_CRIT);
  }

  if (rrcb_ptr->abort_timer_def)
  {
  (void)rex_clr_timer( &rrcb_ptr->abort_timer );
  }
}


#endif /* defined(FEATURE_HDR) */



/*===========================================================================
                     SU LEVEL API's
===========================================================================*/


/*===========================================================================

FUNCTION       HDRRLPNAK_AGE_LISTS

DESCRIPTION    Go through all the nak lists and age them.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpnak_age_lists(void)
{
#ifdef FEATURE_HDR
  dsrlpi_sess_ctl_blk_type *rscb_ptr;    /* session-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;    /* RLP-specific control block     */  
  uint8                    rlp_index;    /* Index to RLP instance          */
  byte p;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Process Nak entries for all registered RLP instances
    -------------------------------------------------------------------------*/
  for( rlp_index = DSRLP_HDR_BASE_INDEX; 
       rlp_index < DSRLP_HDR_MRLP_REV_BASE_INDEX; /* DPA and MFPA FWD */
       rlp_index++ )
  {
    /*-----------------------------------------------------------------------
      Using the rlp_index_id, get the session control block and 
      fcn_ptr_table.
      -----------------------------------------------------------------------*/
    rscb_ptr      = &(dsrlpi_rscb_array[rlp_index]);

    /*-----------------------------------------------------------------------
      Only process DPA and MFPA FWD RLPs
      -----------------------------------------------------------------------*/
    if( rscb_ptr->cfg.ver != DSRLP_VER_HDR &&
        rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD )
    {
      continue;
    }
      
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      rrcb_ptr = &rscb_ptr->rlp_list[p];


      /*---------------------------------------------------------------------
        Process NAK list entries.  Aging of the NAK list may result in data 
        being removed from the re-sequencing buffer and queued up for PPP.  
        V(N) could have changed when the NAK list is aged, because a NAK might
        have been aborted.
        -----------------------------------------------------------------------*/
      if (rrcb_ptr->state.abort_timer_expired == TRUE)
      {
        hdrrlpnak_age_list(rrcb_ptr);
        rrcb_ptr->state.abort_timer_expired = FALSE;
      }
    }
  } /* for rlp_index < DSRLPI_MAX_SR_ID */
#else
  DATA_RLP_MSG0( MSG_LEGACY_ERROR, "hdrrlpnak_age_lists() is not supported." );
#endif

  return;
} /* HDRRLPNAK_AGE_LISTS */



