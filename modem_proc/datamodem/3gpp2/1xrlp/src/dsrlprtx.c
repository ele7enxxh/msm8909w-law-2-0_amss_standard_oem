/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                                              
                   R A D I O   L I N K   P R O T O C O L
              R E X M I T   Q U E U E   P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains generic Re-Transmit Queue processing functions. The 
  Re-Transmit Queue is a queue that holds structures which define the
  frames/parts of frames that the other side has NAK'd.
  
  The re-transmit queue is not  specific to any one RLP type, but is a
  general purpose data structure.
  
  It is assumed that the user will not directly access any retxq information
  contained in the retxq structure.  Instead, all retxq access can be done
  via the externalized functions or macros provided in this file and the
  rlpretxq.h file.

  The retxq is a doubly-linked list, where each element is an element from
  an array of re-txq structs.  The prev and next pointers are really just
  indices into the array.  

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  rlprtxq_init() must be called before any other RETXQ functions can be
  called. 
  
  rlprtxq_init_ptr() must be called when an RLP session needs to set up
  its state variables.  This must be called after rlprtxq_init, but before
  any rtxq other functions.
  

 Copyright (c) 1999-2010 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlprtx.c_v   1.5   12 Nov 2002 17:08:36   jysong  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlprtx.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/27/12   msh        Ignore NAK from BS if first seq > last seq 
11/22/10   sn         Replaced REX premption macros with critical section for 
                      mutual exclusion.
10/14/10   sa         Adding debug messages before ASSERT.
3/17/10    gc         Remove q_destroy() @ li 387 as it's an array index
02/25/10   ss         Using q_destroy() to cleanup the queue resources.
05/10/07   ac         Lint fix
07/19/06   aw         Port Sc2x featurization changes to 6800
05/30/06   ksu        EMPA: RLP init code review comments
01/10/06   TMR        Added FEATURE_HDR
12/04/05   ksu        EMPA segment sequencing: don't fragment for Format B
10/10/05   ksu/sy     Eliminate hdrmrlpi_rx_process_nak()
07/27/05   sy         Fixed retransmit Packet Accept scenario issue and
                      rlprtx_pool get locally function.
06/24/05   tc         Changes to supprt HDR Rev A RTCMAC
5/17/05    vas/tchin  Added function to get retx queue size
09/20/04   vas        QOS related changes
11/12/02   js         Removed some redundant F3 messages. 
10/15/02   js         Optimized F3 messages by adding more information,
                      and removing unnecessary printouts. Also added RLP  
                      stream identifiers to differentiate different RLP
                      streams. 
05/24/01   hcg        Made changes to support HDR (1xEV).
05/21/01   sy         Changed the way reset function is called.
02/16/00   ak         When checking the BS NAK, reset if NAK for a frame
                      equal to L_V(S).
05/10/00   ak         Return TRUE at end of put_rexmit_info.  Do not reset
                      if nak info is out of range of V(S).  Instead return
                      FALSE.
05/02/00   na         If RLP gets a NAK with a sequence number > V(S), it 
                      now resets.
04/13/00   ak         print out message on base station nak.
01/25/00   na         Fixed rlprtx_get_last() to return the last element
                      in tbe RTX array correctly.
01/07/00   ak         Inserted INTLOCKs where appropriate.
08/05/99 na/hcg/ak    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"

#if (defined( FEATURE_DS_RLP3) || defined (FEATURE_HDR))

#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "dsrlpseq.h"
#include "dsrlprtx.h"
#include "amssassert.h"
#include "err.h"
#include "dsm.h"
#include "dsutil.h"

#ifdef FEATURE_HDR
#include "hdrrlp.h"
#include "hdrrlpi.h"
#endif

#ifdef FEATURE_HDR_QOS
#include "hdrmrlp.h"
#include "dsm.h"
#include "hdrrlp.h"
#endif


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#define   RLPRTX_MAX_ELEMENTS    100  /* max elements for all rlp session*/

/*---------------------------------------------------------------------------
  The info for each frame to rexmit is stored in the following struct. 
  
  The elements in the queue are defined in an array.  The prev and next ptrs
  used to access them are really just indices into the array; this gives a
  big memory savings (using 1 byte vs. 4, not having to use the queue fcns).
  
  The queue itself is circular, so that adding to the end of the list is 
  easy (does not require walking the list every time).  There is the only 
  reason the prev pointer is kept.
---------------------------------------------------------------------------*/
typedef struct 
{
  uint32        prev;           /* points to previous item                 */
  uint32        next;           /* points to next  item                    */
  uint32        first_seq;      /* first seq num to rexmit (inclusive)     */
  uint32        last_seq;       /* last seq num to rexmit (inclusive)      */
} rlprtx_type;


/*---------------------------------------------------------------------------
  Definition of the rexmit elements.  These are shared across all RLP 
  versions.  An extra item is declared, as the zero'th element is not used.
---------------------------------------------------------------------------*/
LOCAL rlprtx_type        rlprtx_pool[RLPRTX_MAX_ELEMENTS + 1];

/*---------------------------------------------------------------------------
  Pointer to list of free elements.
---------------------------------------------------------------------------*/
LOCAL uint32             rlprtx_free_q = DSRLPRTX_NO_Q;

/*---------------------------------------------------------------------------
  RLP RTX critical section variable.
---------------------------------------------------------------------------*/
LOCAL rex_crit_sect_type rlprtx_crit_sect;

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================

FUNCTION       RLPRTX_GET

DESCRIPTION    Gets an element from the front of the given list.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
uint32 rlprtx_get
(
  uint32                *rtxq_ptr       /* queue of elements to add to     */
)
{
  uint32                 first_ele;     /* first element in the list       */
  uint32                 second_ele;    /* second element in the list      */
  uint32                 last_ele;      /* last element in the list        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (*rtxq_ptr == DSRLPRTX_NO_Q)
  {
    return DSRLPRTX_NO_Q;
  }

  /*-------------------------------------------------------------------------
    Enter critical section to protect 'rlprtx_pool' data. This function does 
    not get called in an interrupt and so critical section suffice.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlprtx_crit_sect);

  first_ele  = *rtxq_ptr;
  second_ele = rlprtx_pool[first_ele].next;
  last_ele   = rlprtx_pool[first_ele].prev;

  if (first_ele != second_ele)
  {
    rlprtx_pool[second_ele].prev = rlprtx_pool[first_ele].prev;
    rlprtx_pool[last_ele].next   = rlprtx_pool[first_ele].next;

    *rtxq_ptr = second_ele;
  }
  else
  {
    /*-----------------------------------------------------------------------
      One element in the queue, so queue is now empty.
    -----------------------------------------------------------------------*/
    *rtxq_ptr = DSRLPRTX_NO_Q;
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rlprtx_crit_sect);

  rlprtx_pool[first_ele].next = first_ele;
  rlprtx_pool[first_ele].prev = first_ele;

  return first_ele;
}

/*===========================================================================

FUNCTION       RLPRTX_GET_LAST

DESCRIPTION    Gets the last element from the given list.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
uint32 rlprtx_get_last
(
  uint32                *rtxq_ptr       /* queue of elements to add to     */
)
{
  uint32                 first_ele;     /* first element in the list       */
  uint32                 last_ele;      /* last element in the list        */
  uint32                 new_last_ele;  /* second to last element in list  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (*rtxq_ptr == DSRLPRTX_NO_Q)
  {
    return DSRLPRTX_NO_Q;
  }

  /*-------------------------------------------------------------------------
    Enter critical section to protect 'rlprtx_pool' data. This function does 
    not get called in an interrupt and so critical section suffice.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlprtx_crit_sect);

  first_ele    = *rtxq_ptr;
  last_ele     = rlprtx_pool[first_ele].prev;
  new_last_ele = rlprtx_pool[last_ele].prev;

  if (first_ele != last_ele)
  {
    rlprtx_pool[first_ele].prev    = new_last_ele;
    rlprtx_pool[new_last_ele].next = first_ele;
  }
  else
  {
    /*-----------------------------------------------------------------------
      One element in the queue, so queue is now empty.
    -----------------------------------------------------------------------*/
    *rtxq_ptr = DSRLPRTX_NO_Q;
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rlprtx_crit_sect);

  rlprtx_pool[last_ele].next = last_ele;
  rlprtx_pool[last_ele].prev = last_ele;

  return last_ele;
}

/*===========================================================================

FUNCTION       RLPRTX_PUT

DESCRIPTION    Adds an element to the end of the list.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void rlprtx_put
(
  uint32                *rtxq_ptr,     /* queue of elements to add to     */
  uint32                 element        /* element to insert               */
)
{
  uint32                 old_last;      /* The current last item           */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(element <= RLPRTX_MAX_ELEMENTS);

  /*-------------------------------------------------------------------------
    Enter critical section to protect 'rlprtx_pool' data. This function does 
    not get called in an interrupt and so critical section suffice.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlprtx_crit_sect);

  if (*rtxq_ptr == DSRLPRTX_NO_Q)
  {
    /*-----------------------------------------------------------------------
       Inserting into an empty list.  First item points to itself.
    -----------------------------------------------------------------------*/
    *rtxq_ptr = element;
    rlprtx_pool[element].next = element;
    rlprtx_pool[element].prev = element;
  }
  else
  {
    /*-----------------------------------------------------------------------
       Inserting into non-empty list.  Always insert at end of list.
    -----------------------------------------------------------------------*/
    
    old_last = rlprtx_pool[*rtxq_ptr].prev;

    rlprtx_pool[*rtxq_ptr].prev  = element;
    rlprtx_pool[element].prev    = old_last;

    rlprtx_pool[old_last].next = element;
    rlprtx_pool[element].next  = *rtxq_ptr;
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rlprtx_crit_sect);
} /* rlprtx_put() */


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    PUBLIC FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       DSRLPRTX_INIT

DESCRIPTION    Called at initialization, this takes all rexmit entries from
               the array and puts them onto the free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_init(void)
{
  int i;                               /* loop counter                     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rlprtx_free_q = DSRLPRTX_NO_Q;

  /*-------------------------------------------------------------------------
    Initialize critical section.
  -------------------------------------------------------------------------*/
  DS_INIT_CRIT_SECTION( &rlprtx_crit_sect );

  for (i = 1; i < RLPRTX_MAX_ELEMENTS + 1; i++)
  {
    rlprtx_put(&rlprtx_free_q,i);
  }
}


/*===========================================================================

FUNCTION       DSRLPRTX_RESET

DESCRIPTION    Called when an RLP instance is reset.  This takes any rexmit
               queue entries and puts them back into the global free q.
               
DEPENDENCIES   rrcb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
  uint32                      *q_ptr;         /* ptr to rlp's rtx list     */
  uint32                       element;       /* element from the list     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);

  q_ptr = &(rrcb_ptr->state.rtxq);

  ASSERT(q_ptr != NULL);

  /*-------------------------------------------------------------------------
    Removed disable and enable premption as rlprtx_get and rlprtx_put enters 
     into critical section and then leaves during get and put operation.
  -------------------------------------------------------------------------*/
  while ((element = rlprtx_get(q_ptr)) != DSRLPRTX_NO_Q)
  {
    rlprtx_put(&rlprtx_free_q,element);
  }
}


/*===========================================================================

FUNCTION       DSRLPRTX_INIT_PTR

DESCRIPTION    Called when at the start of an RLP session.  Required to 
               set up the rtx ptr in the rscb.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_init_ptr
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);

  rrcb_ptr->state.rtxq = DSRLPRTX_NO_Q;
}

/*===========================================================================

FUNCTION       DSRLPRTX_PUT_REXMIT_INFO

DESCRIPTION    Called by the Rx task when an incoming NAK is received.  This
               function creates an entry for the rexmit list.
               
               Items are not sorted; they are always put at the end of the
               list.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   TRUE  - if the element could be added to rexmit list.
               FALSE - if the element could not be added to the rexmit list.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlprtx_put_rexmit_info
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  uint32                    first_seq,       /* first seq num to xmit      */
  uint32                    last_seq         /* last seq num to xmit       */
)
{
  uint32                    element;          /* element from the list     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);
  /*--------------------------------------------------------------------- 
   Checking first_seq with the last_seq, if first_seq is greater than
  last seq, we ignore 
  ----------------------------------------------------------------------*/ 
  if(DSRLPSEQ_GT_SEQ_CHECK( first_seq,
                            last_seq,
                            rrcb_ptr->cfg_ptr->seq_modulus) )
  {
    DATA_RLP_MSG1( MSG_LEGACY_HIGH, "RLP: %d", rrcb_ptr->cfg_ptr->sr_id );
    DATA_RLP_MSG3( MSG_LEGACY_HIGH, "BS bad nak, first seq > last seq: "
                                    "%x -> %x, Vs %x",
                   first_seq,
                   last_seq,
                   rrcb_ptr->state.next_seq_num );
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    If the peer RLP is NAKing for a sequence number, which haven't even
    sent( last_seq < V(S)), reset RLP.
  -------------------------------------------------------------------------*/
  if( DSRLPSEQ_GE_SEQ_CHECK( last_seq,
                             rrcb_ptr->state.next_seq_num,
                             rrcb_ptr->cfg_ptr->seq_modulus))
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP: %d", rrcb_ptr->cfg_ptr->sr_id);
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "BS bad nak: %x -> %x, Vs %x",
             first_seq,last_seq,rrcb_ptr->state.next_seq_num);
    return( FALSE);
  }

  element = rlprtx_get(&rlprtx_free_q);

  /*-------------------------------------------------------------------------
    If there are no free elements on the retransmit list, then treat
    the bytes as unavailable (i.e. no retx), but don't reset RLP as
    the other side will recover via its Abort timer.
  -------------------------------------------------------------------------*/
  if( element == DSRLPRTX_NO_Q)
  {
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "RLP: %d - No free rexmits: first_seq: %x, last_seq: %x",
             rrcb_ptr->cfg_ptr->sr_id, first_seq, last_seq);
    return( TRUE );
  }

  rlprtx_pool[element].first_seq = first_seq;
  rlprtx_pool[element].last_seq = last_seq; 

  
  rlprtx_put(&rrcb_ptr->state.rtxq ,element);

  return(TRUE);

}/* dsrlprtx_put_rexmit_info() */


/*===========================================================================

FUNCTION       DSRLPRTX_GET_REXMIT_FIRST_SEQ

DESCRIPTION    Called by the Tx task when it needs to send a retransmission.
               Will get the first seq of the first frame requiring re-
               transmsission.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   TRUE if the list is not empty, else returns FALSE.  If it 
               returns TRUE, then the input first_seq is loaded with the
               appropriate value.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlprtx_get_rexmit_first_seq
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  uint32                   *first_seq        /* will be loaded with 1st seq*/      
)
{
  boolean                   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);

  if (rrcb_ptr->state.rtxq != DSRLPRTX_NO_Q)
  {
    ret_val = TRUE;
    *first_seq = rlprtx_pool[rrcb_ptr->state.rtxq].first_seq;
  }

  return (ret_val);
} /* dsrlprtx_get_rexmit_first_seq() */


/*===========================================================================

FUNCTION       DSRLPRTX_GET_REXMIT_LAST_SEQ

DESCRIPTION    Called by the Tx task when it needs to send a retransmission.
               Will get the last seq of the first frame requiring re-
               transmsission.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   TRUE if the list is not empty, else returns FALSE.  If it 
               returns TRUE, then the input last_seq is loaded with the
               appropriate value.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlprtx_get_rexmit_last_seq
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,       /* current RLP ctl blk         */
  uint32                   *last_seq        /* will be loaded with last seq*/      
)
{
  boolean                   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);

  if (rrcb_ptr->state.rtxq != DSRLPRTX_NO_Q)
  {
    ret_val = TRUE;
    *last_seq = rlprtx_pool[rrcb_ptr->state.rtxq].last_seq;
  }

  return (ret_val);
} /* dsrlprtx_get_rexmit_last_seq() */


/*===========================================================================

FUNCTION       DSRLPRTX_REMOVE_FIRST_ElEMENT

DESCRIPTION    Called by the Tx task when it has fully re-transmitted the
               frame(s) described by the first element in the rtx queue.
               
               Removes that element from the queue and places it in the
               free queue.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_remove_first_element
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
  uint32                       element;       /* element from the list     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);

  if (rrcb_ptr->state.rtxq != DSRLPRTX_NO_Q)
  {
    element = rlprtx_get(&(rrcb_ptr->state.rtxq));
    rlprtx_put(&rlprtx_free_q,element); 
  }
} /* dsrlprtx_remove_first_element() */


/*===========================================================================

FUNCTION       DSRLPRTX_REMOVE_LAST_ElEMENT

DESCRIPTION    Called by the Rx task when it finds out that there is a 
               memory shortage in the system.  Then the most recent NAK
               from the peer is dumped.
               
               Removes the newest element from the queue and places it in
               the free queue.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_remove_last_element
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
  uint32                       element;       /* element from the list     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);

  if (rrcb_ptr->state.rtxq != DSRLPRTX_NO_Q)
  {
    element = rlprtx_get_last(&(rrcb_ptr->state.rtxq));
    rlprtx_put(&rlprtx_free_q,element); 
  }
} /* dsrlprtx_remove_last_element() */


/*===========================================================================

FUNCTION       DSRLPRTX_UPDATE_FIRST_ElEMENT

DESCRIPTION    Called by the Tx task when it has partially re-transmitted the
               frame(s) described by the first element in the rtx queue.
               This then updates the first/last info in the element so that
               on the next pass, the Tx task can continue to send info 
               described by this element.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlprtx_update_first_element
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  uint32                    first_seq,       /* new first seq for element  */
  uint32                    last_seq         /* new last seq for element   */
)
{
  uint32                       element;       /* element from the list     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr != NULL);

  if (rrcb_ptr->state.rtxq != DSRLPRTX_NO_Q)
  {
    element = rrcb_ptr->state.rtxq;

    rlprtx_pool[element].first_seq = first_seq;
    rlprtx_pool[element].last_seq  = last_seq;
    DATA_RLP_MSG3(MSG_LEGACY_LOW, "1:Partially RTX frame sent: first_seq:0x%x, last_seq:0x%x, element:%d", 
               rlprtx_pool[element].first_seq, rlprtx_pool[element].last_seq,element);

  }
  else
  {
    
    ASSERT(0);
  }
}
#endif /* any of the RLPs defined */

