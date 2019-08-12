/*===========================================================================

  D A T A   S E R V I C E S   R L P   

DESCRIPTION
  This file contains internal variable and function definitions specific to 
  the Radio Link Protocol implementation that supports EVDV.

EXTERNALIZED FUNCTIONS
  dsdvrlpi_rx_pre_q_init()
    This function initializes RLP's preQ.

  dsdvrlpi_rx_pre_q_cleanup()
    This function empties and cleans up RLP's preQ.

  dsdvrlpi_rx_pre_q_put()
    This function inserts elements into RLP's preQ.
  
  dsdvrlpi_rx_pre_q_get()
    This function gets and removes next element from RLP's preQ.

  dsdvrlpi_rx_pre_q_get_max()
    This function gets and removes maximum elements possible
    (contiguous, inorder and needed) from RLP's preQ.
   
  dsdvrlpi_rx_pre_q_age_list()
    This function processes RLP's preQ - ages the preQ
    entries, removes the ones expired, etc.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsdvrlpi_rx_preq.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/18/11   op      Migrated to MSG 2.0 Macros
05/14/09   ls      Fix the lint errors
01/03/06   ksu     EMPA: hook up forward link with new phys link architecture
12/23/05   ksu     EMPA RSP: two fwd rx queues
12/15/05   TMR     Changed RLP RX queue to watermark and added callback data
                   to RLP RX function callback.
08/29/05   ksu/sy  Remove dsrlp_reg_hdlc() in favor of uint32 app_field
                   parameter to dsrlp_reg_srvc().
05/03/05   atp     Set app_field to hdlc instance before inserting rx_q.
01/17/05   atp     Added include of msg.h and err.h.
09/30/04   atp     modulus field now in cfg instead of ver in rscb_ptr.
07/22/04   atp     After lint.
06/08/04   atp     Support for upper bound on DDW and REXMIT_TIMER.
05/07/04   atp     Initial version. First cut at supporting EVDV (IS707-B).

===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "data_msg.h"

#include "dsdvrlp.h"
#include "dsdvrlp_rx.h"
#include "dsdvrlpi_rx_preq.h"
#include "dsrlpnak.h"
#include "dsrlprsq.h"
#include "dsrlpseq.h"
#include "queue.h"


/*===========================================================================
                      EXTERNAL DATA DEFINITIONS
===========================================================================*/


/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_INIT

DESCRIPTION
  This function initializes RLP's preQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlpi_rx_pre_q_init
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr      /* Ptr to RLP ctl block      */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);

 /*-------------------------------------------------------------------------
   Initialize RLP's pre-q where new or idle fwd frames from Mux Rx are
   queued if they're out-of-order.
 -------------------------------------------------------------------------*/
  /*lint -save -e613 */
  (void) q_init ( &rrcb_ptr->state.fwd_pre_q );
  /*lint -restore */

} /* dsdvrlpi_rx_pre_q_init() */


/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_CLEANUP

DESCRIPTION
  This function empties and cleans up RLP's preQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlpi_rx_pre_q_cleanup
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr      /* Ptr to RLP ctl block     */
)
{
  dsm_item_type   *q_elem_ptr=NULL;            /* Ptr to element in queue  */
  dsm_item_type   *actual_fr_ptr=NULL;       /* Ptr to RLP frame pkt chain */
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;         /* Ptr to frame info */
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);
  /*lint -save -e613 */
  state_ptr     = &rrcb_ptr->state;
  /*lint -restore */

  /*-------------------------------------------------------------------------
    Cleanup the input q where fwd frames from Mux Rx are queued.
  -------------------------------------------------------------------------*/
  while ( ( q_elem_ptr = (dsm_item_type *) q_get (&state_ptr->fwd_pre_q) )
          != NULL)
  {
    fr_info_ptr = ( dsdvrlp_fwd_fr_info_type * ) q_elem_ptr->data_ptr;
    actual_fr_ptr =  fr_info_ptr->actual_fr_item_ptr;
    if ( actual_fr_ptr != NULL )
    {
      dsm_free_packet ( &actual_fr_ptr);
    }
    (void) dsm_free_buffer ( q_elem_ptr );
  }
} /* dsdvrlpi_rx_pre_q_cleanup() */


/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_PUT

DESCRIPTION
  This function inserts elements into RLP's preQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlpi_rx_pre_q_put
( 
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,     /* Ptr to RLP ctl block     */
  dsm_item_type *insrt_elem_ptr           /* Ptr to element to be inserted */
) 
{
  dsm_item_type *curr_list_elem_ptr=NULL;/* Ptr to current  element in list*/
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;    /*Ptr to frame info       */
  dsdvrlp_fwd_fr_info_type *insrt_info_ptr=NULL; /*Ptr to frame info of
                                                   element being inserted  */
  dsrlpi_state_info_type  *state_ptr=NULL;       /*Local state ptr         */
  uint32  insrt_seq;                  /* Seq num of element being inserted */
  uint32  curr_seq;                   /* Seq num of current element in list*/
  uint32  modulus;                    /* Sequence modulus                  */
  boolean found= FALSE;               /* Found appropriate position in list*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);
  /*lint -save -e613 */
  state_ptr = &rrcb_ptr->state;
  modulus   = rrcb_ptr->cfg_ptr->seq_modulus; 
  /*lint -restore */
  
  insrt_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                           insrt_elem_ptr->data_ptr;
  insrt_seq = insrt_info_ptr->seq_num;
  
  curr_list_elem_ptr = (dsm_item_type *) 
                        q_check ( &(state_ptr->fwd_pre_q) );
  while ( ( curr_list_elem_ptr != NULL ) && ( found == FALSE ) )
  {
    fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                             curr_list_elem_ptr->data_ptr;
    curr_seq = fr_info_ptr->seq_num;
    if ( DSRLPSEQ_GT_SEQ_CHECK ( curr_seq, insrt_seq, modulus ) )
    {
      found = TRUE;
      break;
    }
    curr_list_elem_ptr = (dsm_item_type *) 
        q_next ( &(state_ptr->fwd_pre_q), &(curr_list_elem_ptr->link) );
  }
 
  if ( found == FALSE )
  {
    q_put ( &(state_ptr->fwd_pre_q), &(insrt_elem_ptr->link) );
  }
  else
  {
  /*lint -save -e613 */
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert ( &(state_ptr->fwd_pre_q),
               &(insrt_elem_ptr->link),
               &(curr_list_elem_ptr->link)
             );
#else
    q_insert ( &(insrt_elem_ptr->link),
               &(curr_list_elem_ptr->link)
             );
#endif
  /*lint -restore */
  }
  
} /* dsdvrlpi_rx_pre_q_put() */


/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_GET

DESCRIPTION
  This function gets and removes next element from RLP's preQ. 
  (No matter whether it's the needed one or not).

DEPENDENCIES
  None

RETURN VALUE
  Ptr to next element

SIDE EFFECTS
  None.
===========================================================================*/
dsm_item_type *dsdvrlpi_rx_pre_q_get
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* Ptr to RLP ctl block      */
)
{
  dsm_item_type *curr_list_elem_ptr=NULL; /* Ptr to current element in list*/
  dsrlpi_state_info_type  *state_ptr=NULL;/* Local state ptr               */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);
  /*lint -save -e613 */
  state_ptr     = &rrcb_ptr->state;
  /*lint -restore */
  
  curr_list_elem_ptr = (dsm_item_type *) 
                        q_get ( &(state_ptr->fwd_pre_q) );

  return curr_list_elem_ptr;

} /* dsdvrlpi_rx_pre_q_get() */


/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_GET_MAX

DESCRIPTION
  This function gets and removes as many elements as possible from RLP's
  preQ, provided they are contiguous, in-order and start with the needed
  seq number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsdvrlpi_rx_pre_q_get_max
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* Ptr to RLP ctl block      */
) 
{
  dsm_item_type *curr_list_elem_ptr=NULL;/* Ptr to current  element in list*/
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;  /*Ptr to frame info         */
  dsrlpi_curr_fr_info_type out_fr_det;   
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
  uint32  need_seq;                   /* V(N)                              */
  uint32  curr_seq;                   /* Seq num of current element in list*/
  boolean keep_going = TRUE;          /* So far frames contiguous & needed */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);
  /*lint -save -e613 */
  state_ptr = &rrcb_ptr->state;
  /*lint -restore */
  
  curr_list_elem_ptr = (dsm_item_type *) 
                        q_check ( &(state_ptr->fwd_pre_q) );
  while ( ( curr_list_elem_ptr != NULL ) && ( keep_going == TRUE ) )
  {
    fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                             curr_list_elem_ptr->data_ptr;
    curr_seq = fr_info_ptr->seq_num;
    need_seq = state_ptr->needed_seq_num;
    if ( curr_seq == need_seq )
    {
      curr_list_elem_ptr = (dsm_item_type *)
                        q_get ( &(state_ptr->fwd_pre_q) );

      out_fr_det.seq_num      = fr_info_ptr->seq_num;
      out_fr_det.fr_cat       = fr_info_ptr->fr_cat;
      out_fr_det.mux_pdu_type = fr_info_ptr->mux_pdu_type;
      
      /*lint -save -e613 */
      if ( fr_info_ptr->fr_det_enum == DSRLPI_NEW )
      {
        fr_info_ptr->fr_det_enum = 
              rrcb_ptr->cfg_ptr->ver_ptr->fcn_ptr_table->rx_new_data ( rrcb_ptr,
                                         &(fr_info_ptr->actual_fr_item_ptr),
                                                              &out_fr_det
                                                            );
      }
      else if ( fr_info_ptr->fr_det_enum == DSRLPI_IDLE )
      {
        fr_info_ptr->fr_det_enum = 
              rrcb_ptr->cfg_ptr->ver_ptr->fcn_ptr_table->rx_idle ( rrcb_ptr,
                                                          &out_fr_det
                                                        );
      }
      else
      {
        DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Got fr_det_enum %d from preq",
                      fr_info_ptr->fr_det_enum);
      }
      /*lint -restore */

      if ( fr_info_ptr->fr_det_enum != DSRLPI_DONE )
      {
        
      }

      (void) dsm_free_buffer ( curr_list_elem_ptr );
    }
    else
    {
      keep_going = FALSE;
    }

    curr_list_elem_ptr = (dsm_item_type *)
                        q_check ( &(state_ptr->fwd_pre_q) );
  }
} /* dsdvrlpi_rx_pre_q_get_max() */



/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_AGE_LIST

DESCRIPTION
  This function processes RLP's preQ - ages the preQ entries, removes the
  ones expired, etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May result in Naks.
===========================================================================*/
void dsdvrlpi_rx_pre_q_age_list
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block      */
  uint16 num_pkts_in_last_20ms /* Num encoder pkts rxed by mux in last 20ms*/
) 
{
  dsm_item_type *curr_list_elem_ptr=NULL;/* Ptr to current  element in list*/
  dsm_item_type *next_list_elem_ptr=NULL;/* Ptr to next     element in list*/
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;  /*Ptr to frame info         */
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);
  /*lint -save -e613 */
  state_ptr = &rrcb_ptr->state;
  /*lint -restore */

  curr_list_elem_ptr = (dsm_item_type *) 
                        q_check ( &(state_ptr->fwd_pre_q) );
  while ( curr_list_elem_ptr != NULL ) 
  {
    next_list_elem_ptr=(dsm_item_type *) q_next(&(state_ptr->fwd_pre_q),
                                                &(curr_list_elem_ptr->link));
    fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                             curr_list_elem_ptr->data_ptr;
    fr_info_ptr->time_to_expire_ddw -= 20;
    fr_info_ptr->pkts_after_counter += num_pkts_in_last_20ms;
    if (
        ( fr_info_ptr->time_to_expire_ddw == (uint16) 0 ) ||
        ( fr_info_ptr->pkts_after_counter >= DSDVRLP_MAX_PKTS_RXED_AFTER )
       )
    {
      dsdvrlpi_rx_pre_q_process_entries_till_expired_one 
          ( rrcb_ptr, curr_list_elem_ptr );
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete ( &(state_ptr->fwd_pre_q), &(curr_list_elem_ptr->link)  );
#else
      q_delete ( &(curr_list_elem_ptr->link) );
#endif
      dsdvrlpi_rx_pre_q_process_expired_entry
           ( rrcb_ptr, curr_list_elem_ptr );
      (void) dsm_free_buffer ( curr_list_elem_ptr );      
    }
    curr_list_elem_ptr = next_list_elem_ptr; 
  } /* while */
} /* dsdvrlpi_rx_pre_q_age_list() */


/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_PROCESS_ENTRIES_TILL_EXPIRED_ONE

DESCRIPTION
  This function processes  entries in preQ until the one which has actually
  expired (exclusive).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlpi_rx_pre_q_process_entries_till_expired_one
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block      */
  dsm_item_type            *expired_elem_ptr /* expired elem               */
)
{
  dsm_item_type *curr_list_elem_ptr=NULL;/* Ptr to current  element in list*/
  dsm_item_type *next_list_elem_ptr=NULL;/* Ptr to next     element in list*/
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;  /*Ptr to frame info         */
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);
  /*lint -save -e613 */
  state_ptr = &rrcb_ptr->state;
  /*lint -restore */

  curr_list_elem_ptr = (dsm_item_type *)
                        q_check ( &(state_ptr->fwd_pre_q) );
  while  ( 
             ( curr_list_elem_ptr != NULL )
          && ( curr_list_elem_ptr != expired_elem_ptr )
          )
  {
    next_list_elem_ptr=(dsm_item_type *) q_next(&(state_ptr->fwd_pre_q),
                                                &(curr_list_elem_ptr->link));
    fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                             curr_list_elem_ptr->data_ptr;
    fr_info_ptr->time_to_expire_ddw = 0;
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_delete ( &(state_ptr->fwd_pre_q), &(curr_list_elem_ptr->link)  );
#else
    q_delete ( &(curr_list_elem_ptr->link) );
#endif
    dsdvrlpi_rx_pre_q_process_expired_entry ( rrcb_ptr, curr_list_elem_ptr );
    (void) dsm_free_buffer ( curr_list_elem_ptr );
    curr_list_elem_ptr = next_list_elem_ptr; 
  }

} /* dsdvrlpi_rx_pre_q_process_entries_till_expired_one() */


/*===========================================================================

FUNCTION DSDVRLPI_RX_PRE_Q_PROCESS_EXPIRED_ENTRY

DESCRIPTION
  This function processes  entries in preQ whose DDW has expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May result in Naks, due to entries being removed from PreQ.
===========================================================================*/
void dsdvrlpi_rx_pre_q_process_expired_entry
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block      */
  dsm_item_type            *expired_elem_ptr
)
{
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;  /*Ptr to frame info         */
  dsm_item_type   *actual_fr_ptr=NULL;       /* Ptr to RLP frame pkt chain */
  dsm_item_type   *temp_fr_ptr=NULL;       /* Ptr to RLP frame pkt chain */
  dsrlpi_curr_fr_info_type out_fr_det;   
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
  uint32  exp_seq;                    /* V(R)                              */
  uint32  need_seq;                   /* V(N)                              */
  uint32  rxed_seq;                   /* Seq num of current element in list*/
  uint32  new_seq_num;
  uint32  nak_gap_seq;                /* v(R) + big_nak_gap                */
  uint32  vr_seq_hi_plus;             /* v(R) + 0xFF                       */
  uint32  vr_plus_diff;               /* v(R)_seq_hi_plus-rxed seq         */
  uint32  modulus;                    /* Sequence modulus                  */
  uint32  data_len;                   /* Length of fr                      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rrcb_ptr != NULL);
  /*lint -save -e613 */
  state_ptr = &rrcb_ptr->state;
  exp_seq   =  state_ptr->expected_seq_num;
  need_seq  =  state_ptr->needed_seq_num;  
  modulus   =  rrcb_ptr->cfg_ptr->seq_modulus;
  /*lint -restore */

  fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                             expired_elem_ptr->data_ptr;
  rxed_seq      = fr_info_ptr->seq_num;
  actual_fr_ptr = fr_info_ptr->actual_fr_item_ptr;

  out_fr_det.seq_num      = fr_info_ptr->seq_num;
  out_fr_det.fr_cat       = fr_info_ptr->fr_cat;
  out_fr_det.mux_pdu_type = fr_info_ptr->mux_pdu_type;

  if ( fr_info_ptr->fr_det_enum == DSRLPI_NEW )
  {
  /*-------------------------------------------------------------------------
    For a new data frame, the sequence number should be >= to V(N).  If
    less than V(N), drop it.
  -------------------------------------------------------------------------*/
  if (DSRLPSEQ_GT_SEQ_CHECK( exp_seq, rxed_seq, modulus ))
  {
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Rx 0x%x < Vr 0x%x Vn 0x%x",
             rxed_seq,
             exp_seq,
             need_seq
            );
    if ( actual_fr_ptr != NULL )
    {
      dsm_free_packet ( &actual_fr_ptr);
    }
    state_ptr->old_frames++;
    state_ptr->big_nak_gap = FALSE;
    return; 
  }
  else if (   (state_ptr->nak_rounds_rev == 0)
           || ( rxed_seq == need_seq ) )
  {
    state_ptr->old_frames = 0;
    state_ptr->big_nak_gap = FALSE;

    /*----------------------------------------------------------------------
      V(R) == V(N) == Rx'd seq #.

      Insert item onto Rx watermark, and then update V(R) and V(N).  Note
      that the code works for both segmented and unsegmented data. If
      unsegmented, then addition of the length is non-harmful, as last_seg
      == TRUE, and so the sequence number will be inc'd up to the next SEQ.
      If segmented, then adding the length is important, as this could
      result in the S_SEQ rolling over, and SEQ being incremented.  If the
      last_seg was also set, then SEQ would be incremented once more.

      After this addition, new_seq_num equals the seq_num of the last byte
      in the frame.
    ----------------------------------------------------------------------*/
    data_len    = dsm_length_packet ( actual_fr_ptr );

    if (data_len > 0)
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ( rxed_seq,
                                        (data_len - 1),
                                        modulus );
    }
    else
    {
      new_seq_num = rxed_seq;
    }

    /*----------------------------------------------------------------------
      Increment V(R) now. If in byte sequenced mode (new frames have LST
      bit), increment SEQ, otherwise increment S_SEQ.
    ----------------------------------------------------------------------*/
    if (DSRLPI_HAS_LAST_SEG_MASK(fr_info_ptr->fr_cat))
    {
      new_seq_num = DSRLPSEQ_GET_NEXT_SEQ(new_seq_num,
                                        modulus);
    }
    else
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ( new_seq_num, 1,
                                        modulus);
    }


    /*-----------------------------------------------------------------------
      put the item onto the rx queue, and call the callback.  Callback
      assumes non-chained items, so enqueue each item in the pkt list
      separately.

      However, may be possible still stuff in reseq queue, since not all
      items are pushed up the stack at once.  make sure things go up to SIO
      in order.
    -----------------------------------------------------------------------*/
    /*lint -save -e613, -e746 */
    if (DSRLPRSQ_CNT(rrcb_ptr) == 0)
    {
      do
      {
        temp_fr_ptr          = actual_fr_ptr;
        actual_fr_ptr          = actual_fr_ptr->pkt_ptr;
        temp_fr_ptr->pkt_ptr = NULL;

        dsm_enqueue(rrcb_ptr->cfg_ptr->rx_wm_ptr[rrcb_ptr->route], &temp_fr_ptr);
      }while (actual_fr_ptr != NULL);
      rrcb_ptr->cfg_ptr->rx_func_ptr(rrcb_ptr->cfg_ptr->rx_data);
      /*---------------------------------------------------------------------
        Update the statistic for Total Received Bytes only here. It also
        gets updated when data gets out of the re-sequencing queue.
      ---------------------------------------------------------------------*/
      rrcb_ptr->stats.rx_total_bytes  += data_len;
    }
    else
    {
      dsrlprsq_insert(rrcb_ptr, &out_fr_det, actual_fr_ptr);
    }
    /*lint -restore */


    /*-----------------------------------------------------------------------
      Update the state seq num's.  This must be done after the potential
      insertion into the resequencing queue above.  Otherwise, if L_VN is
      updated to 'a+1', and try to insert 'a', then reseq queue will drop
      the frame.
    -----------------------------------------------------------------------*/
    state_ptr->expected_seq_num = new_seq_num;
    state_ptr->needed_seq_num   = new_seq_num;
  }
  else
  {
    state_ptr->old_frames = 0;
    /*----------------------------------------------------------------------
      Either V(R) != V(N) and/or V(R) != Rx'd seq #.

      In both cases, this results in an entry into the resequencing queue,
      since the needed frame was not received.  To convince:

      V(R) == V(N), but V(R) < Rxed seq:  So a hole is inserted at V(N).
                                           Nak entry from V(R) to Rxed.
                                           V(R) is updated to Rxed seq + 1.


      V(R) != V(N), but V(R) == Rxed seq:  Existing hole still not filled.
                                           Update V(R) to Rxed seq + 1.

      V(R) != V(N), and V(R) < Rxed seq:  Existing hole still not filled,
                                           Nak entry from V(R) to Rxed.
                                           V(R) is updated to Rxed seq + 1.

      Know from first if conditional at top of function that rxed seq num
      is greater than or equal to V(R).
    ----------------------------------------------------------------------*/
    /*lint -save -e10, -e26, -e731 */
    if (DSM_LESS_THAN_LINK_LAYER_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL)
          == TRUE)
    /*lint -restore */
    {
      /*-------------------------------------------------------------------
        Running out of items, so dump new frames which would cause
        holes.
      -------------------------------------------------------------------*/
      DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Too few small items, dump new frame");
      dsm_free_packet(&actual_fr_ptr);
      return; 
    }

    if (rxed_seq != exp_seq)
    {
      if (DSRLPSEQ_GT_SEQ_CHECK(rxed_seq,exp_seq,modulus))
      {
        /*-----------------------------------------------------------------
          See if the upcoming NAK gap is too big.
        -----------------------------------------------------------------*/
          nak_gap_seq = DSRLPSEQ_ADD_SEQ(state_ptr->expected_seq_num,
                                         DSRLPI_BIG_NAK_GAP,
                                         modulus);

          /*-----------------------------------------------------------------
            If a frame is on the SCH, then it is not going to be garbled
            due to RDA.  however, if burst alignment is an issue, then it is
            possible that old SCH data is being read from the decoder.  For
            example, our L_V(R) = 0x6ab.  We read a frame from SCH decoder,
            with SEQ = 0xa8.  Guessing SEQ_HI, we come up with 0x7a8.  In
            actuality, it is an old frame.  We should drop it.  To guard
            against legitimate frames, we require

             (L_V(R) + 0xff - rxed seq) < NAK_GAP.

             Note that the DIFF_SEQ requires vr_plus_diff > curr_info->seq.
             That value is used below, after an if() which include a check
             for SEQ_HI being received in the frame.  If SEQ_HI is not in
             the frame, then the rxed frame should be within 256 of our
             L_V(N) which is <= L_V(R).
          -----------------------------------------------------------------*/
          vr_seq_hi_plus = DSRLPSEQ_ADD_SEQ(state_ptr->expected_seq_num,
                                            0xFF,
                                            modulus);

          vr_plus_diff   = DSRLPSEQ_DIFF_SEQ(vr_seq_hi_plus,
                                             out_fr_det.seq_num,
                                             modulus);

          vr_plus_diff   = ((vr_plus_diff >> 12) & 0x0FFF);
          if ((DSRLPSEQ_GE_SEQ_CHECK(out_fr_det.seq_num,
                                     nak_gap_seq, 
                                     modulus)) &&
              (state_ptr->big_nak_gap == FALSE)
             )
          {
            if (
                (out_fr_det.mux_pdu_type != DSRLP_MUX_PDU_1)        &&
                (out_fr_det.mux_pdu_type != DSRLP_MUX_PDU_2)        &&
                ((out_fr_det.fr_cat & CURR_FRAME_SEQ_HI_MASK) == 0) &&
                (vr_plus_diff < DSRLPI_BIG_NAK_GAP)
               )
            {   /* changed MSG_HIGH to MSG_ERROR to help testers. */
                DATA_RLP_MSG2(MSG_LEGACY_ERROR, "BURST GAP seq %x Vr %x",
                          out_fr_det.seq_num,
                          state_ptr->expected_seq_num
                        );
            }
            else
            {
              /*-------------------------------------------------------------
                This frame is dumped, but next one is going to be accepted
                regardless.
              -------------------------------------------------------------*/
              state_ptr->big_nak_gap = TRUE;
                /* changed MSG_HIGH to MSG_ERROR to help testers. */
              DATA_RLP_MSG2(MSG_LEGACY_ERROR, "FCH RDA gap->seq %x Vr %x",
                        out_fr_det.seq_num,
                        state_ptr->expected_seq_num
                      );
            }
            dsm_free_packet( &actual_fr_ptr );
            return;
          }
      }
      DATA_RLP_MSG3(MSG_LEGACY_MED, "hole-Rx=0x%x,Vr=0x%x, Vn=0x%x",
               rxed_seq, exp_seq, need_seq);
    }
    dsrlprsq_insert( rrcb_ptr, &out_fr_det, actual_fr_ptr );

    /*----------------------------------------------------------------------
      Lastly, update V(R) only.  V(N) remains unchanged.
    ----------------------------------------------------------------------*/
    data_len    = dsm_length_packet(actual_fr_ptr);

    if (data_len > 0)
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ(rxed_seq,
                                       (data_len - 1),
                                       modulus);
    }
    else
    {
      new_seq_num = rxed_seq;
    }

    /*----------------------------------------------------------------------
      Increment V(R) now. If in byte sequenced mode (new frames have LST
      bit), increment SEQ, otherwise increment S_SEQ.
    ----------------------------------------------------------------------*/
    if (DSRLPI_HAS_LAST_SEG_MASK(out_fr_det.fr_cat))
    {
      new_seq_num = DSRLPSEQ_GET_NEXT_SEQ(new_seq_num,
                                         modulus);
    }
    else
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ( new_seq_num, 1,
                                        modulus);
    }

    state_ptr->expected_seq_num = new_seq_num;

    state_ptr->big_nak_gap = FALSE;
    /*----------------------------------------------------------------------
      A NAK list entry may be required, if Rx'ed seq > V(R).  This is done
      last because if it fails, then a reset is done.
    ----------------------------------------------------------------------*/
    if (DSRLPSEQ_GT_SEQ_CHECK(rxed_seq,exp_seq,modulus))
    {
      if (dsrlpnak_insert(rrcb_ptr,exp_seq,rxed_seq) == FALSE)
      {
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Could not insert into NAK list");
        state_ptr->reset_rlp = TRUE;
      }
    }
  }
  }
  else if ( fr_info_ptr->fr_det_enum == DSRLPI_IDLE )
  {
    if ( exp_seq == rxed_seq )
    {
      state_ptr->old_frames = 0;
      state_ptr->big_nak_gap = FALSE;
      return;
    }            
    if ( DSRLPSEQ_GE_SEQ_CHECK( rxed_seq, exp_seq, modulus) )
    {
      state_ptr->old_frames = 0;

    /*-----------------------------------------------------------------------
      If the new frame is greater than DSRLP_BIG_NAK_GAP from the prev, then
      mark it as strange, and see if next idle/data frame is also weird.
      This handles cases of bad rate determination, which can lead to
      erroneous NAKs.
    -----------------------------------------------------------------------*/

      nak_gap_seq = DSRLPSEQ_ADD_SEQ(exp_seq, DSRLPI_BIG_NAK_GAP, modulus);

      if ((DSRLPSEQ_GE_SEQ_CHECK(rxed_seq, nak_gap_seq, modulus)) &&
          (state_ptr->big_nak_gap == FALSE)
         )
      {
        /*lint -save -e613 */
        
        /*lint -restore */
        DATA_RLP_MSG3(MSG_LEGACY_HIGH, "big gap: Vr:0x%x, Vn:0x%x, idleseq:0x%x",
                 exp_seq, need_seq, rxed_seq);
        state_ptr->big_nak_gap = TRUE;
        return; 
      }

    /*-----------------------------------------------------------------------
      If the received value is greater than the expected, then make
      a new nak list entry.
    -----------------------------------------------------------------------*/
      if (
           (state_ptr->nak_rounds_rev != 0) && 
           (dsrlpnak_insert(rrcb_ptr,exp_seq,rxed_seq) == FALSE)
         )
      {
      /*---------------------------------------------------------------------
        Reset RLP.
      ---------------------------------------------------------------------*/
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Could not insert into nak list");
        state_ptr->reset_rlp = TRUE;
      }
      else
      {
        state_ptr->expected_seq_num = rxed_seq;
        if (state_ptr->nak_rounds_rev == 0)
        {
          state_ptr->needed_seq_num = rxed_seq;
        }
      }
    }
    else /* rcvd num is less than expected */
    {
    /*-----------------------------------------------------------------------
      Received frame has a sequence number less than what we expected.
      Go ahead and drop this frame on the floor.
    -----------------------------------------------------------------------*/
      /*lint -save -e613 */
      DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Rx idle w/old seq 0x%x Vr=0x%x sr_id:%d",
               rxed_seq, exp_seq, rrcb_ptr->cfg_ptr->sr_id);
      /*lint -restore */
      state_ptr->old_frames++;
    }

  /*-------------------------------------------------------------------------
    If got this far, then idle was okay.  So go ahead and reset big_nak_gap
    value.
  -------------------------------------------------------------------------*/
    state_ptr->big_nak_gap = FALSE;
    return;

  }
  else
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Got fr_det_enum %d from preq",
                  fr_info_ptr->fr_det_enum);
  }

  return;

} /* dsdvrlpi_rx_pre_q_process_expired_entry() */

