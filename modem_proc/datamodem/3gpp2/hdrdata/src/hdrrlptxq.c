/*===========================================================================
            H D R R L P   T R A S M I T T E D   Q U E U E

GENERAL DESCRIPTION
  
 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
   

 Copyright (c) 2007-2014 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrlptxq.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
01/22/14   sd         Fixed null pointer exception in hdrrlptxq_free_txq_item()
03/21/12   msh        TCB changes due to Core changes  
11/18/11   vpk        HDR SU API cleanup
05/12/11   op         Migrated to MSG 2.0 macros
02/08/11   ls         Global variable cleanup
12/27/10   ms         Replaced REX premption macros with critical section for 
                      mutual exclusion.
10/18/10   ms         Precede all Assert(0)s with Debug messages.
11/10/09   as         Cleared Windows Compiler warnings for MOB builds
06/02/09   spn        Lint fix
09/18/08   spn        Fixed Compiler Warning
08/18/08   spn        Added support to MARQ flush packets
07/28/08   spn        If we run out of TXQ Items, we free the oldest Item 
                      across all RLPs and then re-use the just freed item
05/23/07   spn        Lint Fixes
05/22/07   spn        Added an ASSERT() if we get a NULL txq_ptr, while 
                      adding transmitted bytes to the transmitted queue.
02/08/07   sy         Initial Revision.


===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"
#include "hdrrlp_api.h"

#ifdef FEATURE_HDR
#include "hdrrlptxq.h"
#include "hdrrlpseq.h"
#include "queue.h"
#include "time_svc.h"
#include "hdrcom_api.h"
#include "task.h"
#include "hdrrlp.h"
#include "hdrmrlpi.h"
#include "hdrrlp_util.h"
 
/*===========================================================================
                            MACROS
===========================================================================*/
#define HDRRLP_TXQ_TIMER_MS  100       

#define HDRRLPTXQ_GET_SEQ( seq ) (seq & 0x3FFFFFFF) 

/*===========================================================================
                          VARIABLES
===========================================================================*/

static rex_timer_type hdrrlptxq_periodic_timer;/*Periodic timer needed to expire the items
                                                  in the transmitted queue.*/

static boolean hdrrlptxq_timer_started; /*Flag to remember the status of the timer.*/

static  hdrrlptxq_type hdrrlptxq_pool[HDRRLPTXQ_TOTAL_NUM_ITEMS]; /*Pool to hold
                                     the unused items of the tranmsitted queue*/
static q_type hdrrlptxq_free_q;

/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================

FUNCTION  HDRRLPTXQI_CAL_TIMER_DIFF

DESCRIPTION
  
DEPENDENCIES
  Rollover should not happen multiple times. Our timer expiry is not set to over
  500ms.

RETURN VALUE

SIDE EFFECTS 
===========================================================================*/
static uint32 hdrrlptxqi_cal_timer_diff
(
  uint32 current_time, 
  uint32 txq_entry_time
)
{
  uint32 ret;
  if(current_time >= txq_entry_time)
  {
     ret = current_time - txq_entry_time;
  }
  else
  {

    DATA_RLP_MSG0( MSG_LEGACY_LOW,"!!!!!!!!!!!!!!!WRAP AROUND HAPPENED with time. Watch for any weirdness !!!!!!!!!");
    ret = (0xFFFFFFFF - txq_entry_time) + current_time;
  }
  return ret;
}

/*===========================================================================

FUNCTION HDRRLPTXQ_START_TIMER

DESCRIPTION  Start the periodic timer only if it has not been started.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_start_timer
(
  void 
)
{
  /* This timer should be started only in the TX Task context 
   * hdrrlp_util_is_task_hdrtx() is introduced in place of rex_self() to get 
   * the UT compiled
   */
  ASSERT( hdrrlp_util_is_task_hdrtx() );
  /*
    Start the timer if it is not already started.
  */
  if(hdrrlptxq_timer_started == FALSE) 
  { 
    DATA_RLP_MSG1( MSG_LEGACY_LOW,"Starting TXQ Timer, #elements:%d",q_cnt(&hdrrlptxq_free_q));
    (void)rex_set_timer(&hdrrlptxq_periodic_timer, HDRRLP_TXQ_TIMER_MS); 
    hdrrlptxq_timer_started = TRUE; 
  } 
}

/*===========================================================================

FUNCTION HDRRLPTXQ_STOP_TIMER

DESCRIPTION  Stop the periodic timer only if it has been started.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_stop_timer
(
  void
)
{
  /* This timer should be stopped only in the TX Task context */
  ASSERT( hdrrlp_util_is_task_hdrtx() );

  if(hdrrlptxq_timer_started == TRUE) 
  { 
    DATA_RLP_MSG0( MSG_LEGACY_MED,"Stopping SPD Timer");
    (void)rex_clr_timer(&hdrrlptxq_periodic_timer); 
    hdrrlptxq_timer_started = FALSE; 
  }    
}

/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================

FUNCTION hdrrlptxq_powerup_init

DESCRIPTION
  Called to initialize the queue and place all the free items of the pool in
  the transmitted queue.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_powerup_init
(
  void
)
{
  /* Initialize the array of handles */
  int index;

  (void)q_init(&hdrrlptxq_free_q);
  
  for(index= 0; index< (HDRRLPTXQ_TOTAL_NUM_ITEMS); index++)
  {
    (void)q_link(&hdrrlptxq_pool[index], &hdrrlptxq_pool[index].link_next);
    q_put(&hdrrlptxq_free_q, &hdrrlptxq_pool[index].link_next);
  }

  DATA_RLP_MSG0( MSG_LEGACY_LOW,"hdrrlptxq_powerup_init() called.");
}

/*===========================================================================

FUNCTION       HDRRLPTXQ_SET_SIG

DESCRIPTION    Registered function with the txq module timer, which will set a 
               signal to the HDRTX task
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
static void hdrrlptxq_set_sig
(
  unsigned long param
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  (void) rex_set_sigs( hdrrlp_util_get_hdrtx_tcb(), HDRL1_TX_RLP_TXQ_SIG );
  return;
}


/*===========================================================================

FUNCTION HDRRLPTXQ_GET_NEW_TXQ_ITEM

DESCRIPTION  Gets an item from the global txq pool.

  The timestamp on each RLP transmitted frame is saved.
  The timer value is set to value of the expiry time based on the NAK Abort 
  timer negotiated for that RLP.
  timestamp_saved_in the_RLP_tranmsitted_frame = current_timestamp 
                                    + Nak_Abort_timer_negotiated_for_that_RLP.

  RLP Nak Abort timer is different for each RLP from RevA onwards. The default
  value is 500msec.

  If there is atleast one item transmitted then the timer need to be started.
  The timer can be started for the least value of the timer needed for all 
  the RLP's.

  TODO: Just start the timer for the smallest timer value negotiated for the 
        RLP abort timer. See which one is better interms of CPU.


  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
hdrrlptxq_type* hdrrlptxq_new_txq_item
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
)
{
  hdrrlptxq_type* new_ptr = NULL;  
  hdrrlptxq_type*     tmp_txq_item_ptr = NULL; 
  uint32              oldest_txqitem_timestamp = 0xFFFFFFFF;
  uint8               rlp_index;
  uint8               rlp_route;
  dsrlpi_sess_ctl_blk_type    *rscb_ptr;       
  dsrlpi_rlp_ctl_blk_type     *oldest_rrcb_ptr = NULL;
  /*------------------------------------------------------------------------*/

  /* pop one in free_q */
  new_ptr = (hdrrlptxq_type *)q_get(&hdrrlptxq_free_q);
  
  /* If there is none available in the free_q */
  if (new_ptr == NULL)
  {
    /* try to find the oldest to free and re-use it */
    for(rlp_index=0; rlp_index<DSRLP_MAX_NUM_HDR_MRLP_REV; rlp_index++)
    {
      rscb_ptr = &(dsrlpi_rscb_array[DSRLP_HDR_MRLP_REV_BASE_INDEX + rlp_index]);
      for (rlp_route = 0; rlp_route < DSRSP_NUM_ROUTES; ++rlp_route)
      {
        rrcb_ptr = &rscb_ptr->rlp_list[rlp_route];
        
        /* handle to the first item in the queue*/
        tmp_txq_item_ptr = (hdrrlptxq_type*)q_check(&(rrcb_ptr->state.tx_q));

        if ( tmp_txq_item_ptr != NULL )
        {
          /* Is this older than what we had */
          if ( oldest_txqitem_timestamp > tmp_txq_item_ptr->timestamp )
          {
            /* Store information for the oldest RRCB */
            oldest_txqitem_timestamp = tmp_txq_item_ptr->timestamp;
            oldest_rrcb_ptr = rrcb_ptr;
          }
        }/* txq_ptr != NULL */  
      }  
    }  

    /* We must have an TXQ Item that is the oldest that can be 
       freed, and hence we should have a Pointer to the 
       RRCB holding the item
    */   
    ASSERT ( oldest_rrcb_ptr != NULL );

    /* Remove the Item, Free it and get the item from Free Queue
       NOTE: Call free_txq_item() so that the initialization of 
             the item is done*/
    new_ptr = (hdrrlptxq_type*)q_get(&(oldest_rrcb_ptr->state.tx_q));
    ASSERT ( new_ptr != NULL );

    hdrrlptxq_free_txq_item(new_ptr);
    new_ptr = (hdrrlptxq_type *)q_get(&hdrrlptxq_free_q);
  }
  
  /* We must have a txq item, that can be used, as a failsafe we are 
     freeing up the oldest item
  */   
  ASSERT ( new_ptr != NULL );

  return new_ptr;
}

/*===========================================================================

FUNCTION HDRRLPTXQ_FREE_TXQ_ITEM

DESCRIPTION  Placws the item back in the global txq pool.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_free_txq_item
(
  hdrrlptxq_type *free_item
)
{
  if(free_item == NULL)
  {
    DATA_RLP_MSG0( MSG_LEGACY_HIGH, "Null item pointer "
    	           "in hdrrlptxq_free_txq_item()" );
    return;
  }

  DATA_RLP_MSG2( MSG_LEGACY_LOW,"Freeing item with seq_num:0x%x timestamp:%d", 
                      free_item->seq_num, free_item->timestamp );
  /* push one in free_q */
  if(free_item->dsm_chain_ptr != NULL)
  {
    dsm_free_packet(&free_item->dsm_chain_ptr);
  }
  free_item->seq_num = 0;  
  free_item->timestamp = 0;
  free_item->dsm_chain_ptr = NULL;

  q_put(&hdrrlptxq_free_q, &free_item->link_next);

  return;
}

/*===========================================================================

FUNCTION      HDRRLPTXQ_ADD_FRAME

DESCRIPTION   This function adds a new frame into the transmit queue. This
              function is called whenever RLP sends a new frame. It runs in 
              the context of the task that does RLP TX.
              
DEPENDENCIES  Called from Tx task.

RETURN VALUE  None

SIDE EFFECTS    
===========================================================================*/
void hdrrlptxq_add_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    
  hdrrlptxq_type               *fr_param_ptr 
)
{
  q_type                       *tx_q_ptr;     /* ptr to the tx q           */
  hdrrlptxq_type               *temp_ptr; 
  qword                        current_timestamp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

  ASSERT(rrcb_ptr != NULL);
  tx_q_ptr = &(rrcb_ptr->state.tx_q);

  /*-------------------------------------------------------------------------
    This file assumes that sequence numbers do not wrap within the Tx queue.
    -------------------------------------------------------------------------*/
  while ((temp_ptr = (hdrrlptxq_type *)q_check(tx_q_ptr)) != NULL) 
  {
    /*-------------------------------------------------------------------------
      Remove all the items in the TX-Q that have a sequence number that can cause
      wrap around confusion. So any sequence numbers that are greater than the
      sequence number that we are adding are considered to be stale items and need
      to be removed.
      NOTES: If needed optimizing, maybe this check can be avoided considering
      that things will work.
      -------------------------------------------------------------------------*/
    if( HDRRLPSEQ_GT_SEQ_CHECK(HDRRLPTXQ_GET_SEQ(temp_ptr->seq_num),
                              fr_param_ptr->seq_num,
                              rrcb_ptr->cfg_ptr->seq_modulus))
    {
      /* Dequeue from tx_q */
      temp_ptr = q_get(tx_q_ptr);
      hdrrlptxq_free_txq_item(temp_ptr);
    }
    else
    {
      /*-------------------------------------------------------------------------
        The first element in the transmit queue has a sequence number less than
        the seq number of element being added. Thus, break
        -------------------------------------------------------------------------*/
      break;
    }
  }

  if(dsm_length_packet(fr_param_ptr->dsm_chain_ptr) == 0)
  {
    
    ASSERT(0);
  }
  time_get_uptime_ms(current_timestamp);
  fr_param_ptr->timestamp = qw_lo(current_timestamp);

  if(dsm_length_packet(fr_param_ptr->dsm_chain_ptr) == 0)
  {
   
    ASSERT(0);
  }
  /*-------------------------------------------------------------------------
    Add a new frame, just insert it at the tail  of the queue 
  -------------------------------------------------------------------------*/
  DATA_RLP_MSG1( MSG_LEGACY_LOW,"Adding frame to txq with seq: 0x%x ", fr_param_ptr->seq_num);
  q_put(tx_q_ptr, &fr_param_ptr->link_next);

  /*-------------------------------------------------------------------------------
   Since, an item is placed in the txq, make sure the timer is started. If the timer
   is already started, then this function returns.
  --------------------------------------------------------------------------------*/
  hdrrlptxq_start_timer();
} /* hdrrlptxq_add_frame () */

/*===========================================================================

FUNCTION      HDRRLPTXQ_ACCESS_LAST_FRAME

DESCRIPTION   Returns a pointer to the last RLP frame transmitted.  
              The user can than access the internal contents of the RLP 
              frame.  It is assumed that the following remains unchanged:
              
                RLP frame's sequence number
                The dsm item's seq_num field is unchanged
              
DEPENDENCIES  Called from Tx task.  Also see conditions stated above.

RETURN VALUE  If the packet was found, then returns a non-zero value, 
              which indicates the length of the payload in the returned
              dsm packet chain (which contains the relevant RLP frame). 
              
              The passed fr_info_ptr is completely filled out with the info
              from the found dsm packet chain. The fr_info_ptr struct 
              contains a pointer to the head of the dsm pkt chain.
              
              If the packet was not found, the function returns zero.  

SIDE EFFECTS  
 None.
===========================================================================*/
uint16 hdrrlptxq_access_last_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* Ptr to RLP ctl block       */
  hdrrlptxq_type               **fr_info_ptr  /* points to req'd frame info */
)
{
  q_type                     *tx_q_ptr;      /* ptr to tx queue            */
  hdrrlptxq_type             *item_ptr;      /* ptr to queue item          */
  uint32                     payload_len;    /* length of stored payload   */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

  ASSERT(rrcb_ptr != NULL);
  tx_q_ptr = &(rrcb_ptr->state.tx_q);
  
  payload_len = 0;
  
  /*-------------------------------------------------------------------------
    If queue is empty, then return immediately.
    -------------------------------------------------------------------------*/
  if (q_cnt(tx_q_ptr) == 0) 
  {
  
    DATA_RLP_MSG1( MSG_LEGACY_LOW,"No items in Txq for sr_id :%d", rrcb_ptr->cfg_ptr->sr_id);
    return((uint16)payload_len);
  }

  /*-------------------------------------------------------------------------
    Get the last element in the array.
    -------------------------------------------------------------------------*/
  item_ptr = (hdrrlptxq_type *) q_last_check(tx_q_ptr);  
  if (item_ptr == NULL)
  {
    DATA_RLP_MSG1( MSG_LEGACY_LOW,"Txq bad state. DEBUG!! for sr_id :%d", rrcb_ptr->cfg_ptr->sr_id);
    return((uint16)payload_len);             
  }

  payload_len = dsm_length_packet(item_ptr->dsm_chain_ptr);
  payload_len = payload_len - hdrrlp_get_header_size(rrcb_ptr->cfg_ptr);

  if (fr_info_ptr != NULL)
  {    
    *fr_info_ptr  = item_ptr;    
  }
  else
  {
    payload_len = 0;
  }

  DATA_RLP_MSG2( MSG_LEGACY_LOW,"hdrrlptxq_access_last_frame() returning %d bytes for sr_id :%d", 
                  payload_len, rrcb_ptr->cfg_ptr->sr_id);

  return ((uint16)payload_len);

} /* hdrrlptxq_access_last_frame() */

/*===========================================================================

FUNCTION      HDRRLPTXQ_LOCATE_FRAME

DESCRIPTION   Given a sequence number, this returns a pointer to the
              requested RLP frame.  The user can than access the internal
              contents of the RLP frame.  It is assumed that the following
              remains unchanged:
              
                RLP frame's sequence number
                The dsm item's seq_num field is unchanged
                
              The function is passed three important params:
              
                rscb_ptr    - pointer to the session control block
                
                fr_info_ptr - the seq_num field needs to contain the
                              requested sequence number.  All other fields
                              are ignored, and are filled in when the
                              appropriate RLP frame is found.
                              
                next_seq_num - filled in if a frame is found.  no input
                               value required.
                
DEPENDENCIES  Called from Tx task.  Also see conditions stated above.

RETURN VALUE  If the packet was found, then returns a non-NULL value
              which contains the relevant RLP frame. 
              
              next_seq_num is loaded with the seq_num of the next element
              in the txq.  If there is no next element, then next_seq_num
              is set equal to the requested sequence number.
              
              If the packet was not found, it returns NULL.  
              
              If the queue is in fact empty, then next_seq_num is unchanged.

SIDE EFFECTS  NONE
===========================================================================*/
hdrrlptxq_type* hdrrlptxq_locate_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* Ptr to RLP ctl block       */
  uint32                       req_seq_num, /* points to req'd frame info */
  uint32                       *next_seq_num, /* filled with next seq num in*/
                                             /* list                       */
  boolean                      *use_next_seq_num
)
{
  uint32                     ith_seq_num;    /* holds frame's seq num      */
  uint32                     jth_seq_num;    /* holds frame's seq num      */
  word                       payload_len;    /* length of stored payload   */
  uint32                     end_seq_num=0;  /* last seq num in a pkt      */
  q_type                     *tx_q_ptr;       /* ptr to rlp's reseq q       */
  hdrrlptxq_type             *walk_item_ptr;  /* ptr to walk the queue      */
  hdrrlptxq_type             *next_item_ptr;  /* ptr to next item           */
  hdrrlptxq_type             *ret_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

  ASSERT(rrcb_ptr != NULL);
  tx_q_ptr = &(rrcb_ptr->state.tx_q);
  
  payload_len = 0;

  /*-------------------------------------------------------------------------
    If queue is empty, then return immediately.
    -------------------------------------------------------------------------*/
  if (q_cnt(tx_q_ptr) == 0) 
  {
    DATA_RLP_MSG1( MSG_LEGACY_HIGH, "return NULL, *next_seq_num: %d", *next_seq_num);
    *use_next_seq_num = FALSE;
    return(NULL);
  }
  
  /*-------------------------------------------------------------------------
    get a pointer to the begining of the queue
    we know it has at least one element
    -------------------------------------------------------------------------*/
  walk_item_ptr = (hdrrlptxq_type *)q_check(tx_q_ptr);
  ASSERT( walk_item_ptr != NULL);

  
  /*-------------------------------------------------------------------------
    Loop through transmit queue, until the element is found.  Only continue
    through loop if the requested sequence number is greater than the current
    one in the list.  Since q_items != 0, there is at least one element in
    the queue.
    -------------------------------------------------------------------------*/
  do 
  {
    /*-----------------------------------------------------------------------
      point to the next element in the queue. It may be NULL
    -------------------------------------------------------------------------*/
    next_item_ptr = (hdrrlptxq_type *)q_next(tx_q_ptr, &(walk_item_ptr->link_next));
    
    /*-----------------------------------------------------------------------
      To make the code readable, extract the sequence numbers and place
      them in the local vars.
    -------------------------------------------------------------------------*/
    ith_seq_num = walk_item_ptr->seq_num;
    
    /*-----------------------------------------------------------------------
      if this is the very last item we are looking at right now
    -------------------------------------------------------------------------*/
    if ( next_item_ptr == NULL )
    {
      /*---------------------------------------------------------------------
        Either one element in list or no other elements.  If the req'd 
        seq_num is >= i'th element's seq_num,then check against that element.
      -----------------------------------------------------------------------*/
      
      payload_len = (word)dsm_length_packet(walk_item_ptr->dsm_chain_ptr);
      payload_len = payload_len - hdrrlp_get_header_size(rrcb_ptr->cfg_ptr);

      if (payload_len > 0)
      {
        /* EMPA segment sequencing: 1 data unit per retx'd RLP packet */
        if (rrcb_ptr->cfg_ptr->enhanced &&
            rrcb_ptr->cfg_ptr->segment_sequencing)
        {
          end_seq_num = ith_seq_num;
        }
        else
        {
          end_seq_num = HDRRLPSEQ_ADD_SEQ(ith_seq_num,
                                           (payload_len - 1),
                                           rrcb_ptr->cfg_ptr->seq_modulus);
        }
      }
      else
      {
        DATA_RLP_MSG4( MSG_LEGACY_FATAL,
                        "RLP Tx[%d,%d] V[%d,%d] , Accessing Zero length frame ", 
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced );
        ASSERT(0);
      }
       
      if (HDRRLPSEQ_GE_SEQ_CHECK(req_seq_num,
                                ith_seq_num,
                                rrcb_ptr->cfg_ptr->seq_modulus)     &&
          HDRRLPSEQ_GE_SEQ_CHECK(end_seq_num,
                                req_seq_num,
                                rrcb_ptr->cfg_ptr->seq_modulus))
      {
        ret_ptr = walk_item_ptr;        
        *use_next_seq_num = FALSE;
      }
      else if (HDRRLPSEQ_GT_SEQ_CHECK(ith_seq_num,
                                     req_seq_num,
                                     rrcb_ptr->cfg_ptr->seq_modulus
                                     ))
      {
        /*-------------------------------------------------------------------
          return NULL to indicate the frame was not found
        ---------------------------------------------------------------------*/
        DATA_RLP_MSG3( MSG_LEGACY_HIGH, "return NULL, end_seq_num: %d >= req_seq_num: %d >= ith_seq_num: %d  ",
                       end_seq_num,
                       req_seq_num,
                       ith_seq_num);
        walk_item_ptr = (hdrrlptxq_type *)q_check(tx_q_ptr);
        *next_seq_num = walk_item_ptr->seq_num;
        *use_next_seq_num = TRUE;
        ret_ptr = NULL;
        DATA_RLP_MSG1( MSG_LEGACY_HIGH, "return NULL, *next_seq_num: %d use_next_seq_num TRUE", *next_seq_num);
      }
      else
      {
        *use_next_seq_num = FALSE;
        ret_ptr =  NULL;
      }
      break;
    }
    else /* more items in queue */
    {
      /*---------------------------------------------------------------------
        this should never be NULL if the code got here
        ---------------------------------------------------------------------*/
      ASSERT(next_item_ptr != NULL);

      /*---------------------------------------------------------------------
        There is at least one element after the i'th.  See if the requested
        sequence number falls in between the i'th and j'th frames
        ---------------------------------------------------------------------*/
      jth_seq_num = next_item_ptr->seq_num;

      if (HDRRLPSEQ_GE_SEQ_CHECK(req_seq_num,
                                 ith_seq_num,
                                 rrcb_ptr->cfg_ptr->seq_modulus)     &&
          HDRRLPSEQ_GT_SEQ_CHECK(jth_seq_num,
                                 req_seq_num,
                                 rrcb_ptr->cfg_ptr->seq_modulus))
      {
        /*-------------------------------------------------------------------
          Requested seq_num lies between this and the next seq num.  See if
          the i'th packet contains the correct number of bytes, such that
          it in fact does contain the seq num requested.
          -------------------------------------------------------------------*/
        payload_len = (word)dsm_length_packet(walk_item_ptr->dsm_chain_ptr);
        payload_len = payload_len - hdrrlp_get_header_size(rrcb_ptr->cfg_ptr);

        if (payload_len > 0)
        {
          /* EMPA segment sequencing: 1 data unit per retx'd RLP packet */
          if (rrcb_ptr->cfg_ptr->enhanced &&
              rrcb_ptr->cfg_ptr->segment_sequencing)
          {
            end_seq_num = ith_seq_num;
          }
          else
          {
            end_seq_num = HDRRLPSEQ_ADD_SEQ(ith_seq_num,
                                             (payload_len - 1),
                                             rrcb_ptr->cfg_ptr->seq_modulus);
          }
        }
        else
        {
          DATA_RLP_MSG4( MSG_LEGACY_FATAL,
                          "RLP Tx[%d,%d] V[%d,%d] , Accessing Zero length frame ", 
                          rrcb_ptr->cfg_ptr->sr_id, 
                          "AB"[rrcb_ptr->route],
                          rrcb_ptr->cfg_ptr->ver, 
                          rrcb_ptr->cfg_ptr->enhanced );
          ASSERT(0);
        }
        if (HDRRLPSEQ_GE_SEQ_CHECK(end_seq_num,
                                  req_seq_num,
                                  rrcb_ptr->cfg_ptr->seq_modulus))
        {
          /*-----------------------------------------------------------------
            The i'th element does contain the correct info.  Fill out the
            return struct with proper info.
            -----------------------------------------------------------------*/
          ret_ptr = walk_item_ptr;
          *use_next_seq_num = FALSE;
        }
        else
        {
          
		  DATA_RLP_MSG2(MSG_LEGACY_ERROR, " end_seq_num %d is less than req_seq_num %d ",
		  	            end_seq_num,req_seq_num);
          /*-----------------------------------------------------------------
            ASSERT, since verified ith <= req < j, so " end >= req "
            -----------------------------------------------------------------*/
          ASSERT(0);
        } 
        break;
      }
      else if (HDRRLPSEQ_GT_SEQ_CHECK(ith_seq_num,
                                     req_seq_num,
                                     rrcb_ptr->cfg_ptr->seq_modulus
                                     ))
      {
        /*-------------------------------------------------------------------
          requested seq_num is now less than remaining stored frames.  time
          to get out of the loop.
          -------------------------------------------------------------------*/
        walk_item_ptr = (hdrrlptxq_type *)q_check(tx_q_ptr);
        *next_seq_num = walk_item_ptr->seq_num;
        *use_next_seq_num = TRUE;
        ret_ptr = NULL;
        DATA_RLP_MSG1( MSG_LEGACY_HIGH, "return NULL, *next_seq_num: %d use_next_seq_num: TRUE", *next_seq_num);
        break;
      }
    }
  } while((walk_item_ptr = (hdrrlptxq_type *)q_next(tx_q_ptr, &(walk_item_ptr->link_next))) != NULL);
  
  return (ret_ptr);
} /* hdrrlptxq_locate_frame() */

/*===========================================================================

FUNCTION      HDRRLPTXQ_RESET
             
DESCRIPTION   This goes ahead and clears out a transmit q of all of its
              items.  Called from dsrlpi_reset() if RLP is reset or gotten
              rid of.  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlptxq_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
  q_type             *tx_q_ptr;              /*   ptr to rlp's reseq q     */
  hdrrlptxq_type     *tx_ptr;          /*   reseq  list entry        */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT(rrcb_ptr != NULL);
  tx_q_ptr = &(rrcb_ptr->state.tx_q);
   
  /*-------------------------------------------------------------------------
    now you can free up the rest of the contents of the transmit Q.
   -------------------------------------------------------------------------*/
  /* we dont need to TASK LOCK. As this function is called under enter/leave 
  data_path_crit section already */
  while ((tx_ptr = (hdrrlptxq_type *)q_get(tx_q_ptr)) != NULL)
  {
    hdrrlptxq_free_txq_item(tx_ptr);
  }/* while() */

  DATA_RLP_MSG4( MSG_LEGACY_LOW,
                  "RLP Tx[%d,%d] V[%d,%d] , hdrrlptxq() reset() Done ", 
                  rrcb_ptr->cfg_ptr->sr_id, 
                  "AB"[rrcb_ptr->route],
                  rrcb_ptr->cfg_ptr->ver, 
                  rrcb_ptr->cfg_ptr->enhanced );

}/* hdrrlptxq_reset() */

/*===========================================================================

FUNCTION HDRRLPTXQ_GET_INFO

DESCRIPTION
  Called to return the information first and last seq numbers from a 
  txq item  

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_get_info
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
  hdrrlptxq_type *txq_ptr,
  uint32 *first_seq,
  uint32 *last_seq,
  uint32 *pkt_length
)
{
  const dsrlpi_cfg_info_type *cfg_ptr = NULL;
  uint32 length = 0;
  uint8                   rlp_header_size;  /* Size of the RLP header      */ 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Make sure that we are not doing a 
     NULL pointer dereferencing */
   
  ASSERT ( ( rrcb_ptr != NULL ) &&
           ( txq_ptr != NULL ) && 
           ( first_seq != NULL ) && 
           ( last_seq != NULL ) );
  
  cfg_ptr = rrcb_ptr->cfg_ptr;
  rlp_header_size = hdrrlp_get_header_size(cfg_ptr);
  
  length = dsm_length_packet(txq_ptr->dsm_chain_ptr) - rlp_header_size;  
  
  /* Already ASSERTed that first_seq will be NON-NULL */
  *first_seq = txq_ptr->seq_num;

  /* Already ASSERTed that last_seq will be NON-NULL */
  *last_seq = HDRRLPSEQ_ADD_SEQ( txq_ptr->seq_num, 
                                HDRRLPI_NUM_DATA_UNITS(cfg_ptr, (uint16)length),
                                (rrcb_ptr->cfg_ptr->seq_modulus));  

  if ( pkt_length != NULL )
  {
    *pkt_length = length; 
  }

} /* hdrrlptxq_get_info() */


/*===========================================================================

FUNCTION HDRRLPTXQ_GET_TXQ_SIZE

DESCRIPTION
  Called to return the current size of the TX Queue

DEPENDENCIES
  None.

RETURN VALUE
  Number of Items in the TX Queue

SIDE EFFECTS
===========================================================================*/
uint16 hdrrlptxq_get_txq_size
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr
)
{

  return (uint16)q_cnt( &(rrcb_ptr->state.tx_q) );

} /* hdrrlptxq_get_txq_size() */

#endif /* FEATURE_HDR */


/*=========================================================================== 

                  SU LEVEL API's

=========================================================================== */


/*=========================================================================== 
 
FUNCTION       HDRRLPTXQ_TIMER_INIT
 
DESCRIPTION    Initializes the timer for txq
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void hdrrlptxq_timer_init
(
  void
) 
{ 
#ifdef FEATURE_HDR
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_def_timer_ex( &hdrrlptxq_periodic_timer, 
                    hdrrlptxq_set_sig,
                    (unsigned long)NULL );
  DATA_RLP_MSG0( MSG_LEGACY_LOW,"hdrrlptxq_timer_init() called.");

#else
  DATA_RLP_MSG0( MSG_LEGACY_ERROR,"hdrrlptxq_timer_init() is not supported.");
#endif
  return;
}

/*===========================================================================

FUNCTION     HDRRLPTXQ_PROCESS_SIGNAL

DESCRIPTION    Function is called if the TXQ timer expires and sets a signal
               for the HDRTX task. This funcitons goes through all the RLP's 
               tranmsitted queues and remove all the items that have their 
               nak_abort timer expired. 

 Adv: 1.This will clear up all the items in the transmitted queue which 
        are no longer needed as the AN is not suppose to request for these 
        items beyond the Nak Abort timer negotiated for that RLP.
      2.Saves the system memory by releasing the unwanted DSM items 
        in the old RLP frames.
      3.Guarantees to find an item in the transmitted queue.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_process_signal( rex_sigs_type signal )
{
#ifdef FEATURE_HDR
  qword curr_timestamp;
  dsrlpi_sess_ctl_blk_type *rscb_ptr = NULL; 
  hdrrlptxq_type *txq_item;
  boolean txq_items_exist = FALSE;
  uint8 rscb_index ;
  q_type *tx_q_ptr = NULL;     /* ptr to the tx q           */
  byte    current_route = 0;
  uint8   obsolete_items = 0;
  hdrrlp_debug_ctl *hdrrlp_debug_ptr = NULL;
  hdrmrlp_app_ctl_blk_type *hdrmrlp_sn_acb_ptr = NULL;
/*--------------------------------------------------------------------------------------------*/
  
  ASSERT ( signal == HDRL1_TX_RLP_TXQ_SIG  );

  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();

  hdrmrlp_sn_acb_ptr = get_hdrmrlp_sn_acb_ptr();

  /*clear the timer, so that it can be started.*/
  hdrrlptxq_timer_started = FALSE;

  DATA_RLP_MSG0(MSG_LEGACY_MED, "Starting to remove obsolete Items from TXQ ");\

  time_get_uptime_ms( curr_timestamp );

  /* Loop for all HDR RLPs ( Default ( AN/SN ) and Rev-A )
     Remove the items from each RLP's transmitted queue that have expired 
     their NAK Abort timer value
  */
  for( rscb_index = DSRLP_HDR_BASE_INDEX;
       rscb_index < DSRLP_MAX_RSCB_ELEMENTS;
       rscb_index++ )
  {
    /* Initialize the tx_q_ptr  and the rscb_ptr*/
    tx_q_ptr = NULL;
    
    rscb_ptr = &dsrlpi_rscb_array[rscb_index];

    /* If Version Ptr is NULL, that means the RLP is not initialized */
    if ( rscb_ptr->cfg.ver_ptr == NULL )
    {
      continue;
    }
    
    /* For Forward MRLP we dont need to check to remove items */
    if ( rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD )
    {
      continue;
    }

    /* For Reverse MRLP, get the open route and then the tx_q_ptr */
    else if ( rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE )
    {
      if ( rscb_ptr->is_hdrmrlp_active == TRUE )
      {
        current_route = hdrmrlp_sn_acb_ptr->active_route;
        tx_q_ptr = &rscb_ptr->rlp_list[current_route].state.tx_q;
      }
      else
      { 
        /* Since the MRLP is not active, we skip it */
        continue;
      }
    }

    /* For Default RLP, Only one route and then the tx_q_ptr */
    else if ( rscb_ptr->cfg.ver == DSRLP_VER_HDR )
    {
      current_route = hdrmrlp_sn_acb_ptr->active_route ;
      tx_q_ptr = &rscb_ptr->rlp_list[current_route].state.tx_q;
    }

    /* We should have a tx_q_ptr by now */
    ASSERT( tx_q_ptr != NULL );

    /* Make sure that we dont empty out the txq, keep at least
       one element for the flush packet */ 
    while(((txq_item = (hdrrlptxq_type*)q_check(tx_q_ptr)) != NULL) &&
          (q_cnt(tx_q_ptr) > 1 ) 
          )
    {
      if(hdrrlptxqi_cal_timer_diff((uint32)qw_lo(curr_timestamp), txq_item->timestamp) < rscb_ptr->cfg.nak_abort_timer_val  )
      {
        /*
          Looks like there is at least one transmitted queue that has a nak_abort timer set
          and have elements in them ==> we need to start the periodic timer again.
          Set a variable to remember that an RLP has elements in its transmitted queue.
        */
        txq_items_exist = TRUE;
        break;
      }
      else
      {
        
        /* Dequeue from tx_q */
        txq_item = q_get(tx_q_ptr);

        if (hdrrlp_debug_ptr->enable_tx_debug > 1)
        {
          DATA_RLP_MSG7( MSG_LEGACY_MED,
                          "RLP Tx[%d,%d] V[%d,%d] Seq#[0x%x, ), Removing Item txq_timestapm:%d, curr_time:%d", 
                          rscb_ptr->cfg.sr_id, 
                          "AB"[current_route],
                          rscb_ptr->cfg.ver, 
                          rscb_ptr->cfg.enhanced,
                          txq_item->seq_num,
                          txq_item->timestamp,
                          (uint32)qw_lo(curr_timestamp));
        }
        /*Replace the item in the free pool*/
        hdrrlptxq_free_txq_item(txq_item);
        obsolete_items++;

      }
    }/* while loop */
  }/* for loop */

  /* Restart the periodic timer if needed.
  */
  if (txq_items_exist == TRUE)
  {
    hdrrlptxq_start_timer(); 
  }


 DATA_RLP_MSG1(MSG_LEGACY_MED, " Removed %d obsolete items ", obsolete_items );
  
#else
  
  ASSERT(0);
#endif
  return;
}/*hdrrlptxq_process_signal()*/
