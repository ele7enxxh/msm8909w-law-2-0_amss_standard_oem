/*===========================================================================

  D A T A   S E R V I C E S   R L P   

DESCRIPTION
   This file contains functions that deal with Rx interface for the
   Radio Link Protocol implentation that supports EVDV. 

EXTERNALIZED FUNCTIONS
   dsdvrlp_alloc_fwd_frames( n )
     Called by Mux to allocate n elements when it has received n pdus 
     at that point on Forward Link which will later be enqueued to RLP.

   dsdvrlp_enqueue_fwd_frames()
     Called by Mux to enqueue a certain number of frames received on Forward
     Link to RLP after it has filled up info regarding rate, muxpdutype, etc
     for each pdu.

   dsdvrlp_process_fwd_frames()
     Called by Mux at fixed intervals for RLP to process enqueued frames.    

   dsdvrlp_process_20ms_tick()
     Called by Mux once in 20 ms.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsdvrlp_rx.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/13   sd      Added critical sections to avoid race condition issues
04/26/13   sd      Fixed assert failure for RLP state crash.
02/09/11   ack     Added is_1xrlp_ready flag to chk for 1X RLP init state  
01/18/11   op      Migrated to MSG 2.0 Macros
10/14/10   sa      Adding debug messages before ASSERT.
05/10/07   ac      Lint fix
03/05/07   spn     Fixed lint errors
07/25/06   ksu     EMPA: 1x RLP code review comments
12/23/05   ksu     EMPA RSP: two fwd rx queues
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
10/07/05   atp     CR78332: Got rid of assert.
07/27/05   atp     CR70146: Replaced assert with print msg when mux gives
                   rx data after rlp deregistered.
01/17/05   atp     Added include of msg.h and err.h.
07/22/04   atp     After lint.
06/08/04   atp     Support for upper bound on DDW and REXMIT_TIMER.
05/10/04   atp     dsm_item tracers under  DSDVRLP_TEST_ONLY for time being.
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

#include "ds707_chs.h"
#include "dsm.h"
#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "dsdvrlp.h"
#include "dsdvrlp_rx.h"
#include "dsdvrlpi_rx_preq.h"
#include "dsdvrlpi_rx.h"
#include "dsrlpnak.h"
#include "dsrlprsq.h"
#include "queue.h"

#include "ds707_cta_rlp.h"
#include "dsrlp3.h"
#include "rlp_event_defs.h"


/*===========================================================================
            EXTERNAL DATA DEFINITIONS AND DECLARATIONS
===========================================================================*/


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
  Upper limit on frames to remove and process at a time from the input queue.
---------------------------------------------------------------------------*/
#define DSDVRLP_MAX_FR_TO_RM_FROM_IN_Q  DSDVRLP_MAX_FR_IN_SLOT

/*---------------------------------------------------------------------------
  Upper limit on frames to remove and process at a time from the pre-queue.
---------------------------------------------------------------------------*/
#define DSDVRLP_MAX_FR_TO_RM_FROM_PRE_Q  DSDVRLP_MAX_FR_IN_SLOT

/*---------------------------------------------------------------------------
  Upper limit on frames to remove and process at a time from the reseq queue.
---------------------------------------------------------------------------*/
#define DSDVRLP_MAX_FR_TO_RM_FROM_RSQ_Q  DSDVRLP_MAX_FR_IN_SLOT

/*---------------------------------------------------------------------------
  When TRUE, causes a reset to all RLP's.  This is because the number of
  remaining dsm small items has reached a dangerous level, and so a reset
  is the only remedy.  Set to TRUE in dsm callback dsrlp_mem_event when the
  Do Not Exceed memory event occurs.
---------------------------------------------------------------------------*/
LOCAL boolean dsdvrlp_mem_critical_ev= FALSE;

LOCAL boolean dsdvrlp_data_fr_rxed  = FALSE; /* received any data frs?     */
LOCAL boolean dsdvrlp_age_nak_list  = FALSE; /* Age Nak list               */
LOCAL uint16  dsdvrlp_curr_erasure  = 0;     /* count contiguous erasures  */
LOCAL uint16  dsdvrlp_curr_rlp_delay= 0;     /* count rlp_delay            */

LOCAL event_rlp_payload_type dsdvrlp_event_payload;/*payload for RLP events*/

/*===========================================================================
                      EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION DSDVRLP_ALLOC_FWD_FRAMES

DESCRIPTION
  Allocates  num forward frame element items, where num is a parameter
  passed by the caller (mux).  Note that each element is embedded in a
  dsm item. The items are linked together in the form of a linked list. 

DEPENDENCIES
  None

RETURN VALUE
  Ptr to first item in the linked list.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type * dsdvrlp_alloc_fwd_frames 
( 
  uint8   num_frames                             /* Num items required     */
)
{
  dsm_item_type *curr_list_elem_ptr=NULL;/* Ptr to current element in list */
  dsm_item_type *prev_list_elem_ptr=NULL;/* Ptr to previous element in list*/
  dsm_item_type *first_list_elem_ptr=NULL;/* Ptr to first element in list  */
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;         /* Ptr to frame info */
  uint8            i=0;                        /* temp counter             */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*------------------------------------------------------------------------
  1X RLP might be in the process of initializing. Check the state before 
  proceeding.
  ------------------------------------------------------------------------*/
  if (dsrlp_get_1xrlp_ready_flag())
  {
    for ( i=0; i< num_frames; i++ )
    {
      curr_list_elem_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
      if ( curr_list_elem_ptr == NULL )
      {
        ERR("Running out of DSM small items.",0,0,0);       
        break;   
      }

      if ( i==0)
      {
        first_list_elem_ptr = curr_list_elem_ptr;
      }
      else
      {
        if(prev_list_elem_ptr == NULL)
        {
          
        }
        else
        {
          prev_list_elem_ptr->pkt_ptr = curr_list_elem_ptr;
        }
      }
      prev_list_elem_ptr          = curr_list_elem_ptr;
  
      curr_list_elem_ptr->used = sizeof ( dsdvrlp_fwd_fr_info_type );
      /*-----------------------------------------------------------------------
        Initialize frame info fields.
      -----------------------------------------------------------------------*/
      fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *) 
                                               curr_list_elem_ptr->data_ptr;       
      fr_info_ptr->this_dsm_item_ptr  = curr_list_elem_ptr;
      fr_info_ptr->actual_fr_item_ptr = NULL;
      fr_info_ptr->srvc_type          = DSRLP_GENERIC_SRVC;
      fr_info_ptr->mux_pdu_type       = DSRLP_MUX_PDU_MAX;
      fr_info_ptr->fr_rate            = DSRLP_RATE_INACTIVE;
      fr_info_ptr->sr_id              = dsrlp_get_sr_id (DSRLP_FIXED_SR_ID_INDEX);
      fr_info_ptr->chan               = DSDVRLP_RX_FCH;
      fr_info_ptr->fr_det_enum        = DSRLPI_NULL;
      fr_info_ptr->seq_num            = 0;
      fr_info_ptr->fr_cat             = 0;
      fr_info_ptr->time_to_expire_ddw = 0;
      fr_info_ptr->pkts_after_counter = 0;
    } /* for () */
  }
  else
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "is_1xrlp_ready is FALSE. 1X RLP not Initalized yet");
    return NULL;
  }

  if(curr_list_elem_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "curr_list_elem_ptr is NULL in dsdvrlp_alloc_fwd_frames");
  }
  else
  { 
   curr_list_elem_ptr->pkt_ptr  = NULL;
  }

  return first_list_elem_ptr;

} /* dsdvrlp_alloc_fwd_frames() */



/*===========================================================================

FUNCTION DSDVRLP_ENQUEUE_FWD_FRAMES

DESCRIPTION
  This function is called by Mux Rx to pass on a linked list of frame 
  elements received on Forward Link over to RLP's input queue. Along with 
  the pointer to the payload, the relevant info fields should have been 
  filled up by Mux. The parameter passed is a pointer to the first item in
  the list, and the number of frames (pdus) in the list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlp_enqueue_fwd_frames
(
  dsm_item_type * first_fwd_fr_elem_ptr,   /* Ptr to first dsm item in the
                                              linked list of items containing
                                              forward frame elements       */
  uint8  num_frames                        /* Number of frames in list     */
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type        *rrcb_ptr;  /* rlp-specific control block */
  dsm_item_type *curr_list_elem_ptr=NULL; /* Ptr to current element in list*/
  dsm_item_type *next_list_elem_ptr=NULL; /* Ptr to next    element in list*/
  uint8            i=0;                   /* temp counter                  */
  dsdvrlp_fwd_fr_info_type *fr_info_ptr = NULL; /*Ptr to frame info        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  rscb_ptr = &( dsrlpi_rscb_array[ DSRLP_FIXED_SR_ID_INDEX ]);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];
  curr_list_elem_ptr = first_fwd_fr_elem_ptr;
  for (i=0; i<num_frames; i++)
  {
    if ( curr_list_elem_ptr == NULL )
    {
      DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                    "Unexpected null ptr in elem queued by Mux Rx");
      break;
    }
    else if (!dsrlp_get_1xrlp_ready_flag())
    {
      DATA_RLP_MSG0 (MSG_LEGACY_ERROR, 
                   "is_1xrlp_ready is FALSE. 1X RLP not Initalized yet" );
      /* RLP has not been initialized yet. Don't accept new frames.
         Free the DSM item and continue to the next frame. */

      fr_info_ptr = 
          ( dsdvrlp_fwd_fr_info_type *) curr_list_elem_ptr->data_ptr;

      if ( fr_info_ptr->actual_fr_item_ptr != NULL )
      {
        dsm_free_packet ( &(fr_info_ptr->actual_fr_item_ptr)  );
      }
      next_list_elem_ptr = curr_list_elem_ptr->pkt_ptr;
      dsm_free_buffer( curr_list_elem_ptr );
      curr_list_elem_ptr = next_list_elem_ptr;
      continue;
    }

    next_list_elem_ptr = curr_list_elem_ptr->pkt_ptr;
    curr_list_elem_ptr->pkt_ptr = NULL;
    q_put ( &rrcb_ptr->state.fwd_input_q, &(curr_list_elem_ptr->link) );
    curr_list_elem_ptr = next_list_elem_ptr;
  }
} /* dsdvrlp_enqueue_fwd_frames() */


/*===========================================================================

FUNCTION DSDVRLP_PROCESS_FWD_FRAMES

DESCRIPTION
  This function is called by Mux Rx at periodic intervals (currently 5ms)
  for RLP to process the queued up frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlp_process_fwd_frames
(
  void
)
{
  dsm_item_type *curr_list_elem_ptr=NULL;  /*Ptr to current element in list*/
  dsm_item_type *temp_list_elem_ptr=NULL;  /*Temp ptr to element in list   */
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;  /*Ptr to frame info         */
  dsdvrlp_fwd_fr_info_type *temp_info_ptr=NULL;/*Temp ptr to frame info    */
  dsrlp_fwd_frame_type      in_fr_det;   /* Input  to fr determination fn  */
  dsrlpi_curr_fr_info_type out_fr_det;   /* Result of fr determination fn  */
  uint8        num_fr_dequeued = 0;     /* Number frames dequeued this time*/

  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;   /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr=NULL;   /* rlp-specific control block */
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
  dsrlp_stats_info_type   *stats_ptr=NULL;   /* local stats ptr            */
  struct dsrlpi_fcn_ptr_table *fcn_ptr_table;/* Ptr table to RLP functions */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------
  1X RLP might be in the process of initializing. Check the state before 
  proceeding.
  ------------------------------------------------------------------------*/
  if (!dsrlp_get_1xrlp_ready_flag())
  {
    DATA_RLP_MSG0 (MSG_LEGACY_ERROR, 
                   "is_1xrlp_ready is FALSE. 1X RLP not Initalized yet" );
    return;
  }

  rscb_ptr      = &( dsrlpi_rscb_array [ DSRLP_FIXED_SR_ID_INDEX ] );
  rrcb_ptr      = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];
  state_ptr     = &rrcb_ptr->state;
  stats_ptr     = &rrcb_ptr->stats;

  /*-------------------------------------------------------------------------
    The call may have ended, but the message has not yet gotten to rxc.
    So go ahead and check to see if the ver_ptr is null, and if so, then
    dump the rxc frames.
    This may also happen if the user has not setup both the rx_func_ptr
    and rx_queue.
  -------------------------------------------------------------------------*/
  if ((rrcb_ptr->cfg_ptr->ver_ptr == NULL) ||
      (rrcb_ptr->cfg_ptr->rx_func_ptr == NULL) ||
      (rrcb_ptr->cfg_ptr->rx_wm_ptr[DSRSP_ROUTE_A] == NULL)
     )
  {
    DATA_RLP_MSG0(MSG_LEGACY_HIGH, "process_fwd_frames called when ver_ptr null");
    /*---------------------------------------------------------------------
      Free the buffers for all the rx'ed RLP frames.
    ---------------------------------------------------------------------*/
    curr_list_elem_ptr = (dsm_item_type *) 
                             q_get ( &rrcb_ptr->state.fwd_input_q);
    while ( curr_list_elem_ptr != NULL )
    {

      fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                           curr_list_elem_ptr->data_ptr;
      if ( fr_info_ptr->actual_fr_item_ptr != NULL )
      {
        dsm_free_packet ( &(fr_info_ptr->actual_fr_item_ptr)  );
      }
      (void) dsm_free_buffer ( curr_list_elem_ptr );
      curr_list_elem_ptr = (dsm_item_type *) 
                               q_get ( &rrcb_ptr->state.fwd_input_q );
    }
    dsdvrlp_mem_critical_ev = FALSE;
    return;
  }

  fcn_ptr_table = rrcb_ptr->cfg_ptr->ver_ptr->fcn_ptr_table;
  if ( fcn_ptr_table == NULL )
  {
    DATA_RLP_MSG0(
	    MSG_LEGACY_HIGH, "process_fwd_frames called when fcn_ptr null");
    return;
  }

  /*-------------------------------------------------------------------------
    Loop until all frames have been processed or we hit the limit for a run.
  -------------------------------------------------------------------------*/
  num_fr_dequeued = 0;
  curr_list_elem_ptr = (dsm_item_type *) 
                           q_get ( &rrcb_ptr->state.fwd_input_q );
  while (
         ( curr_list_elem_ptr != NULL ) &&
         ( num_fr_dequeued < DSDVRLP_MAX_FR_TO_RM_FROM_IN_Q )
        )
  {
    num_fr_dequeued++ ;

    fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                             curr_list_elem_ptr->data_ptr;


    in_fr_det.valid_frame = TRUE;
    in_fr_det.srvc_type   = fr_info_ptr->srvc_type;
    in_fr_det.mux_pdu_type= fr_info_ptr->mux_pdu_type;
    in_fr_det.fr_rate     = fr_info_ptr->fr_rate;
    in_fr_det.fr_item_ptr = fr_info_ptr->actual_fr_item_ptr;
    memset( &out_fr_det, 0, sizeof( out_fr_det ));


    /*-----------------------------------------------------------------------
      If the frame is valid (determined by MUX layer), then determine its 
      frame type using the RLP specific frame determination function.
    -----------------------------------------------------------------------*/
    if(
       (fr_info_ptr->fr_rate != DSRLP_RATE_INACTIVE)    &&
       (fr_info_ptr->fr_rate != DSRLP_RATE_ERASURE_SCH) &&
       (fr_info_ptr->fr_rate != DSRLP_RATE_ERASURE)     &&
       (fr_info_ptr->fr_rate != DSRLP_RATE_BLANK)       &&
       (fr_info_ptr->fr_rate != DSRLP_RATE_DCCH_DTX)     
      )
    {
      /*lint -save -e613 */
      rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
      fr_info_ptr->fr_det_enum = fcn_ptr_table->rx_frame_det( rrcb_ptr, 
                                                            &in_fr_det,
                                                            &out_fr_det);
      rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
      /*lint -restore */

      fr_info_ptr->seq_num = out_fr_det.seq_num;
      fr_info_ptr->fr_cat  = out_fr_det.fr_cat;
      fr_info_ptr->actual_fr_item_ptr = in_fr_det.fr_item_ptr;/*rx_frame_det
                                                                can modify*/

      /*---------------------------------------------------------------------
        If RLP is not established, see if the received frame was Idle or New.
        In this case RLP state moves from ACK or RT_MEAS to ESTABLISHED.
        A frame other than IDLE or New will be discarded, and state reset
        to sending SYNCs.
      ---------------------------------------------------------------------*/
      rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
      if ( state_ptr->state != DSRLPI_ESTABLISHED_STATE)
      {
        /*-------------------------------------------------------------------
          Do not go dormant if RLP has not synced up
        -------------------------------------------------------------------*/
        dsdvrlp_data_fr_rxed = TRUE;

        if (( state_ptr->state == DSRLPI_ACK_STATE ) ||
            ( state_ptr->state == DSRLPI_RT_MEAS_STATE ))
        {
          /*-----------------------------------------------------------------
            In this state, either will transition to established state, or
            need to increment the rlp_delay count (in the case of erasures.
            Otherwise, down below, the frame will be passed to rx_init
            and processed accordingly.
          -----------------------------------------------------------------*/
          if ( fr_info_ptr->fr_det_enum == DSRLPI_RLP_ERASURE )
          {
            state_ptr->rlp_delay++;
          }
          else if ( fr_info_ptr->fr_det_enum != DSRLPI_INIT )
          {
            state_ptr->state= DSRLPI_ESTABLISHED_STATE;

            /*---------------------------------------------------------------
              Set this back to zero now that we are out of all 3 init states
            ---------------------------------------------------------------*/
            state_ptr->sync_delay = 0;

            DATA_RLP_MSG2(MSG_LEGACY_HIGH, "RLP (sr_id=%d) SYNCED UP with RTT=%d",
                      rrcb_ptr->cfg_ptr->sr_id, 
		      state_ptr->rlp_delay);
            stats_ptr->last_rtt = state_ptr->rlp_delay;
          }
        } /* if in ack_state  */
        else if( fr_info_ptr->fr_det_enum != DSRLPI_INIT)
        {
          /*-----------------------------------------------------------------
            in the sync or sync-ack state, and so are expecting some kind of
            init frame.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Got fr_type=%d in state=%d. Dump frame. RLP:%d",
                    fr_info_ptr->fr_det_enum,
                    state_ptr->state,
                    rrcb_ptr->cfg_ptr->sr_id);

          /*-----------------------------------------------------------------
            If the frame type is an RLP erasure or a NULL, then let the
            handling below take care of it, and do not reset RLP.  Otherwise,
            just free the packet and return.  In the latter case, since the
            frame was not what was expected, go ahead and reset RLP, so that
            RLP returns to the SYNC state (with all variables set correctly).
          -----------------------------------------------------------------*/
          if (( fr_info_ptr->fr_det_enum != DSRLPI_RLP_ERASURE ) &&
              ( fr_info_ptr->fr_det_enum != DSRLPI_NULL ))
          {
            DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP: %d", rrcb_ptr->cfg_ptr->sr_id);
            DATA_RLP_MSG3(
		    MSG_LEGACY_HIGH, "Used = 0x%x Bytes = 0x%x 0x%x",
                      dsrlpi_ctl_used,
		      dsrlpi_ctl_byte1,
		      dsrlpi_ctl_byte2);
            if (state_ptr->state != DSRLPI_SYNC_STATE)
            {
              state_ptr->reset_rlp = TRUE;  
            }
            dsm_free_packet( &(fr_info_ptr->actual_fr_item_ptr));
            fr_info_ptr->fr_det_enum = DSRLPI_RESET;
          }
        } /* if frame not init frame */

      } /* if RLP not ESTABLISHED */
      rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));

    }
    else if ( fr_info_ptr->fr_rate == DSRLP_RATE_INACTIVE )
    {
      /*---------------------------------------------------------------------
       blank item on the inactive channel, discard frame and do no further
       processing. Free dsm item for safety.
      ---------------------------------------------------------------------*/
      if ( fr_info_ptr->actual_fr_item_ptr != NULL )
      {
        dsm_free_packet ( &(fr_info_ptr->actual_fr_item_ptr)  );
      }
      fr_info_ptr->fr_det_enum = DSRLPI_DONE;
    }
    else if ( fr_info_ptr->fr_rate == DSRLP_RATE_ERASURE_SCH )
    {
      /*---------------------------------------------------------------------
        erasure on the SCH... discard frame and do no further processing.
        Do not call this an MUX_ERASURE, as that implies a MUX_ERASURE on
        the FCH.  Free dsm item for safety.
      ---------------------------------------------------------------------*/
      if ( fr_info_ptr->actual_fr_item_ptr != NULL )
      {
        dsm_free_packet ( &(fr_info_ptr->actual_fr_item_ptr)  );
      }
      fr_info_ptr->fr_det_enum = DSRLPI_DONE;
    }
    else
    {
      /*---------------------------------------------------------------------
        Received either an erasure or blank from MUX.  Specify frame type
        as such, so correctly processed below.  Increment the rlp_delay
        if in that state.
      ---------------------------------------------------------------------*/
      if (
            (state_ptr->state == DSRLPI_ACK_STATE) ||
            (state_ptr->state == DSRLPI_RT_MEAS_STATE)
         )
      {
        state_ptr->rlp_delay++;
      }
      if (( fr_info_ptr->fr_rate == DSRLP_RATE_BLANK ) ||
          ( fr_info_ptr->fr_rate == DSRLP_RATE_DCCH_DTX ))
      {
        stats_ptr->rx_blank_fr_cnt++;
      }
      if ( fr_info_ptr->fr_rate == DSRLP_RATE_DCCH_DTX )
      {
        fr_info_ptr->fr_det_enum = DSRLPI_DCCH_DTX;
      }
      else
      {
        fr_info_ptr->fr_det_enum = DSRLPI_MUX_ERASURE;
      }
    }

    if ( fr_info_ptr->fr_det_enum != DSRLPI_DONE )
    {
      if ( fr_info_ptr->fr_det_enum != DSRLPI_MUX_ERASURE)
      {
        dsdvrlp_curr_erasure = 0;
      }
      switch ( fr_info_ptr->fr_det_enum )
      {
        case DSRLPI_NEW:

          stats_ptr->rx_new_data_fr_cnt++;
          state_ptr->last_fill_nak_cnt++;
          dsdvrlp_data_fr_rxed = TRUE;
          dsdvrlp_age_nak_list = TRUE;
          /*lint -save -e613 */
          fr_info_ptr->fr_det_enum =
            fcn_ptr_table->rx_new_data ( rrcb_ptr, 
                                         &(fr_info_ptr->actual_fr_item_ptr),
                                         &out_fr_det
                                       );
          /*lint -restore */
          break;

        case DSRLPI_REXMIT:

          stats_ptr->rx_rexmit_fr_cnt++;
          dsdvrlp_data_fr_rxed = TRUE;
          /*lint -save -e613 */
          (void) fcn_ptr_table->rx_rexmit( rrcb_ptr, &in_fr_det, &out_fr_det );
          /*lint -restore */
          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        case DSRLPI_IDLE:

          stats_ptr->rx_idle_fr_cnt++;
          dsdvrlp_age_nak_list = TRUE;
          if ( fr_info_ptr->actual_fr_item_ptr != NULL ) 
          {
            dsm_free_packet( &(fr_info_ptr->actual_fr_item_ptr) );
          }
          /*lint -save -e613 */
          fr_info_ptr->fr_det_enum =
                      fcn_ptr_table->rx_idle ( rrcb_ptr, &out_fr_det );
          /*lint -restore */
          break;

        case DSRLPI_INIT:

          dsdvrlp_data_fr_rxed = TRUE;
          /*lint -save -e613 */
          (void) fcn_ptr_table->rx_init( rrcb_ptr, &out_fr_det );
          /*lint -restore */
          if ( fr_info_ptr->actual_fr_item_ptr != NULL )
          {
            dsm_free_packet( &(fr_info_ptr->actual_fr_item_ptr) );
          }
          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        case DSRLPI_RLP_ERASURE:

          stats_ptr->rx_rlp_erasures++;
          /*lint -save -e613 */
          (void) fcn_ptr_table->rx_rlp_erasure( rrcb_ptr, &in_fr_det );
          /*lint -restore */
          if ( fr_info_ptr->actual_fr_item_ptr != NULL )
          {
            dsm_free_packet( &(fr_info_ptr->actual_fr_item_ptr) );
          }
          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        case DSRLPI_MUX_ERASURE:

          stats_ptr->rx_mux_erasures++;
          if ( stats_ptr->largest_contig_erasure < ++dsdvrlp_curr_erasure )
          {
            stats_ptr->largest_contig_erasure = dsdvrlp_curr_erasure;
          }
          /*lint -save -e613 */
          (void) fcn_ptr_table->rx_mux_erasure( rrcb_ptr, &in_fr_det );
          /*lint -restore */
          if ( fr_info_ptr->actual_fr_item_ptr != NULL )
          {
            dsm_free_packet( &(fr_info_ptr->actual_fr_item_ptr) );
          }
          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        case DSRLPI_NULL:

          stats_ptr->rx_null_fr_cnt++;
          if ( fr_info_ptr->actual_fr_item_ptr != NULL )
          {
            dsm_free_packet( &(fr_info_ptr->actual_fr_item_ptr) );
          }
          dsdvrlp_age_nak_list = TRUE;
          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        case DSRLPI_NAK:

          dsdvrlp_data_fr_rxed = TRUE;
          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        case DSRLPI_FILL:

          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        case DSRLPI_RESET:

          temp_list_elem_ptr = (dsm_item_type *) 
                                 q_get ( &rrcb_ptr->state.fwd_input_q );
          while ( temp_list_elem_ptr != NULL )
          {
            temp_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                 temp_list_elem_ptr->data_ptr;
            if ( temp_info_ptr->actual_fr_item_ptr != NULL )
            {
              dsm_free_packet ( &(temp_info_ptr->actual_fr_item_ptr)  );
            }
            (void) dsm_free_buffer ( temp_list_elem_ptr );
            temp_list_elem_ptr = (dsm_item_type *) 
                                   q_get ( &rrcb_ptr->state.fwd_input_q );
          }
          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        case DSRLPI_DCCH_DTX:

          if ( fr_info_ptr->actual_fr_item_ptr != NULL )
          {
            dsm_free_packet( &(fr_info_ptr->actual_fr_item_ptr) );
          }
          dsdvrlp_age_nak_list = TRUE;
          fr_info_ptr->fr_det_enum = DSRLPI_DONE;
          break;

        default:
          /*lint -save -e506, -e774 */
          DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid RLP Frame type %d",
                            (int)fr_info_ptr->fr_det_enum);
          ASSERT( 0);
          /*lint -restore */
          break;
      } /* switch () */
    }

    if ( fr_info_ptr->fr_det_enum != DSRLPI_DONE)
    {
      fr_info_ptr->time_to_expire_ddw =
           ( state_ptr->fwd_ddw <= DSDVRLP_MAX_DDW_SUPPORTED ) ?
           state_ptr->fwd_ddw+20 : DSDVRLP_MAX_DDW_SUPPORTED;

      fr_info_ptr->pkts_after_counter = 0;
      dsdvrlpi_rx_pre_q_put ( rrcb_ptr, curr_list_elem_ptr );
    }
    else
    {
      fr_info_ptr->actual_fr_item_ptr = NULL;
      (void) dsm_free_buffer ( curr_list_elem_ptr );
    }

    curr_list_elem_ptr = (dsm_item_type *) 
                           q_get ( &rrcb_ptr->state.fwd_input_q );

  } /* main while loop processing frames from input q () */

  /*-------------------------------------------------------------------------
    Clear out contiguous inorder frames from V(N) if available.
  -------------------------------------------------------------------------*/
  dsdvrlpi_rx_pre_q_get_max ( rrcb_ptr );
  
} /* dsdvrlp_process_fwd_frames() */


/*===========================================================================

FUNCTION DSDVRLP_PROCESS_20MS_TICK

DESCRIPTION
  This function is called by Mux Rx once in 20 ms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsdvrlp_process_20ms_tick
(
  uint8  chan_assign,          /* Channels assigned at this time           */
  uint16 num_pkts_in_last_20ms /* Num encoder pkts rxed by mux in last 20ms*/
)
{
  dsm_item_type *curr_list_elem_ptr=NULL;  /*Ptr to current element in list*/
  dsdvrlp_fwd_fr_info_type *fr_info_ptr=NULL;/*Temp ptr to frame info      */
  uint8        i = 0;                   /* Temp counter                    */
  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;   /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr=NULL;   /* rlp-specific control block */
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
  dsrlp_stats_info_type   *stats_ptr=NULL;   /* local stats ptr            */
  struct dsrlpi_fcn_ptr_table *fcn_ptr_table;/* Ptr table to RLP functions */
  event_rlp_nak_abort_payload_type dsdvrlp_event_nak_abort_payload;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------
  1X RLP might be in the process of initializing. Check the state before 
  proceeding.
  ------------------------------------------------------------------------*/
  if (!dsrlp_get_1xrlp_ready_flag())
  {
    DATA_RLP_MSG0 (MSG_LEGACY_ERROR, 
                   "is_1xrlp_ready is FALSE. 1X RLP not Initalized yet" );
    return;
  }
  
  /*-------------------------------------------------------------------------
    NOTE: Although the variable is named fwd_frame_block->sr_id, it is
    actually the RLP instance for this service (Currently only one RLP
    instance is supported)
  -------------------------------------------------------------------------*/
  rscb_ptr      = &( dsrlpi_rscb_array [ DSRLP_FIXED_SR_ID_INDEX ] );
  rrcb_ptr      = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
    The call may have ended, but the message has not yet gotten to rxc.
    So go ahead and check to see if the ver_ptr is null, and if so, then
    dump the rxc frames.
    This may also happen if the user has not setup both the rx_func_ptr
    and rx_queue.
  -------------------------------------------------------------------------*/
  if ((rrcb_ptr->cfg_ptr->ver_ptr == NULL) ||
      (rrcb_ptr->cfg_ptr->rx_func_ptr == NULL) ||
      (rrcb_ptr->cfg_ptr->rx_wm_ptr[DSRSP_ROUTE_A] == NULL)
     )
  {
    /*---------------------------------------------------------------------
      Free the buffers for all the rx'ed RLP frames.
    ---------------------------------------------------------------------*/
    curr_list_elem_ptr = (dsm_item_type *)
                             q_get ( &rrcb_ptr->state.fwd_input_q);
    while ( curr_list_elem_ptr != NULL )
    {
      fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                           curr_list_elem_ptr->data_ptr;
      if ( fr_info_ptr->actual_fr_item_ptr != NULL )
      {
        dsm_free_packet ( &(fr_info_ptr->actual_fr_item_ptr)  );
      }
      (void) dsm_free_buffer ( curr_list_elem_ptr );
      curr_list_elem_ptr = (dsm_item_type *)
                               q_get ( &rrcb_ptr->state.fwd_input_q );
    }
    dsdvrlp_mem_critical_ev = FALSE;
    return;
  }

  state_ptr     = &rrcb_ptr->state;
  stats_ptr     = &rrcb_ptr->stats;
  fcn_ptr_table = rrcb_ptr->cfg_ptr->ver_ptr->fcn_ptr_table;

  /*-------------------------------------------------------------------------
    If PPP data was received after dormancy and notification was requested,
    notify the required module (PZID hysterisis).
  -------------------------------------------------------------------------*/
  if((rscb_ptr->cfg.notify_ppp_data_transfer_func_ptr != NULL) &&
     (rrcb_ptr->state.ppp_data_transfer_after_dormancy == TRUE))
  {
    rscb_ptr->cfg.notify_ppp_data_transfer_func_ptr(DSRLP_FIXED_SR_ID_INDEX);
  }


  if ( fcn_ptr_table == NULL )
  {
    DATA_RLP_MSG0 (MSG_LEGACY_HIGH, "process_20ms called when fcn_ptr null" );
    return;
  }

  state_ptr->chan_assignment = chan_assign;

  /*-------------------------------------------------------------------------
    Check the memory event variables. If they are set, then some action
    may be required, over all RLP sessions.  Normal processing can then
    proceed.  In the case of a reset, the received frames will be dumped
    as bad frames, unless they happen to be sync frames.
  -------------------------------------------------------------------------*/
  if ( dsdvrlp_mem_critical_ev == TRUE)
  {
    /*-----------------------------------------------------------------------
      Small items are in short supply.  Reset all RLP instances; otherwise
      data will start to be dropped on the floor, eventually resulting in
      rlp resets anyways.
    -----------------------------------------------------------------------*/

    for(i = 0; i < DSRLP_MAX_RSCB_ELEMENTS; i++)
    {
      if (dsrlpi_rscb_array[i].cfg.ver_ptr != 0)
      {
        DATA_RLP_MSG1(
		MSG_LEGACY_HIGH, "mem critical - resetting RLP %d", i);

        dsrlpi_reset(&(dsrlpi_rscb_array[i].rlp_list[DSRSP_ROUTE_A]));

       /*--------------------------------------------------------------------
          Report RLP reset event
       --------------------------------------------------------------------*/
       dsdvrlp_event_payload.event_rlp_type = event_rlp_reset;
       dsdvrlp_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
       event_report_payload (EVENT_RLP,
                             sizeof(event_rlp_payload_type),
                             &dsdvrlp_event_payload);

      }
    }
    dsdvrlp_mem_critical_ev = FALSE;
  }

  /*-------------------------------------------------------------------------
    Check if the maximum time to achieve synchronization has been exceeded.
    Discard the received frame(s), end call  and return, if so.
  -------------------------------------------------------------------------*/
  if(
       ((state_ptr->state == DSRLPI_SYNC_STATE)    ||
        (state_ptr->state == DSRLPI_SYNC_ACK_STATE)||
        (state_ptr->state == DSRLPI_ACK_STATE)
       )
       &&
       (++(state_ptr->sync_delay) == DSRLPI_MAX_RTT_SETUP)
    )
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "SYNC state Timeout");
    /*-----------------------------------------------------------------------
      Free the buffers for all the rx'ed RLP frames.
    -----------------------------------------------------------------------*/
    curr_list_elem_ptr = (dsm_item_type *) 
                            q_get ( &rrcb_ptr->state.fwd_input_q );
    while ( curr_list_elem_ptr != NULL ) 
    {
      fr_info_ptr = ( dsdvrlp_fwd_fr_info_type *)
                                           curr_list_elem_ptr->data_ptr;
      if ( fr_info_ptr->actual_fr_item_ptr != NULL )
      {
        dsm_free_packet ( &(fr_info_ptr->actual_fr_item_ptr)  );
      }
      (void) dsm_free_buffer ( curr_list_elem_ptr );
      curr_list_elem_ptr = (dsm_item_type *) 
                              q_get ( &rrcb_ptr->state.fwd_input_q );
    }

    /*-----------------------------------------------------------------------
      Reset RLP. This will cleanup all the memory and initialiaze all state
      variables. This also updates the "resets" stat.
    -----------------------------------------------------------------------*/
    dsrlpi_reset( rrcb_ptr);

    /*-----------------------------------------------------------------------
      Report RLP reset event
    -----------------------------------------------------------------------*/
    dsdvrlp_event_payload.event_rlp_type = event_rlp_reset;
    dsdvrlp_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
    event_report_payload (EVENT_RLP,
                          sizeof(event_rlp_payload_type),
                          &dsdvrlp_event_payload);

    /*-----------------------------------------------------------------------
      Call this function to end the current call
    -----------------------------------------------------------------------*/
    ds707_cta_rlp_sync_timeout();

    /*-----------------------------------------------------------------------
      Set the counter back to 0 as we now have reached the limit and the
      call will be dropped
    -----------------------------------------------------------------------*/
    state_ptr->sync_delay = 0;

    /*-----------------------------------------------------------------------
      Now just return, nothing else to do here
    -----------------------------------------------------------------------*/
    return;
  } /* if ( sync timeout ) */

  /*-------------------------------------------------------------------------
    Process the input queue anyway. Mux would've probably called it already
    for this bin.
  -------------------------------------------------------------------------*/
  dsdvrlp_process_fwd_frames ();

  /*-------------------------------------------------------------------------
    Increase the 20 ms counter.
  -------------------------------------------------------------------------*/
  stats_ptr->rx_20ms_fr_cnt++; 
  if (((stats_ptr->rx_20ms_fr_cnt % 100) == 0)  &&
       (stats_ptr->rx_20ms_fr_cnt  > 0)         &&
       (state_ptr->mux_erasure_cnt > 0))
  {
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, "rx'd %d erases & blanks fr mux in 100 fr times RLP:%d",
             state_ptr->mux_erasure_cnt,
	     rrcb_ptr->cfg_ptr->sr_id);
    state_ptr->mux_erasure_cnt = 0;
  }

  /*-------------------------------------------------------------------------
    Age the nak list if new data/idle/mux erasures were received. V(N) could
    have changed when the NAK list is aged, because a NAK might have been
    aborted. In this case, the resequencing queue needs to be cleared.
  -------------------------------------------------------------------------*/
  if ( dsdvrlp_age_nak_list == TRUE )
  {
    if ( dsrlpnak_age_list( rrcb_ptr) == TRUE)
    {
      DATA_RLP_MSG3(MSG_LEGACY_HIGH, " NAK ABORT new V(N)=%x, V(R)=%x RLP:%d",
                 state_ptr->needed_seq_num,
                 state_ptr->expected_seq_num,
                 rrcb_ptr->cfg_ptr->sr_id);
      stats_ptr->aborts++;
      /*---------------------------------------------------------------------
        Report the RLP NAK abort event
      ---------------------------------------------------------------------*/
      dsdvrlp_event_payload.event_rlp_type = event_rlp_nak_abort;
      dsdvrlp_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
      event_report_payload (EVENT_RLP,
                            sizeof(event_rlp_payload_type),
                            &dsdvrlp_event_payload);

      dsdvrlp_event_nak_abort_payload.info = rrcb_ptr->cfg_ptr->sr_id;
      dsdvrlp_event_nak_abort_payload.first_seq = state_ptr->needed_seq_num;
      dsdvrlp_event_nak_abort_payload.last_seq = state_ptr->expected_seq_num;
      event_report_payload (EVENT_RLP_NAK_ABORT, 
                          sizeof(event_rlp_nak_abort_payload_type),
                          &dsdvrlp_event_nak_abort_payload);
    }
  }

  if (state_ptr->old_frames >= 5)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,"Reset due to too many old frames from BS");
    state_ptr->reset_rlp = TRUE;          
  }

  /*-------------------------------------------------------------------------
    Make sure that rlp_delay hasn't been over-incremented.  If so, set
    rlp_delay such that it is only incremented by 1.
  -------------------------------------------------------------------------*/
  if (state_ptr->rlp_delay > (dsdvrlp_curr_rlp_delay + 1))
  {
    state_ptr->rlp_delay = dsdvrlp_curr_rlp_delay + 1;
  }

  /*-------------------------------------------------------------------------
   Check if reset flag is set by tx, if it does then reset rlp else continue
   processing the rx.
  -------------------------------------------------------------------------*/
  if (state_ptr->reset_rlp == TRUE)
  {
    dsrlpi_reset(rrcb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    dsdvrlp_event_payload.event_rlp_type = event_rlp_reset;
    dsdvrlp_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
    event_report_payload (EVENT_RLP,
                          sizeof(event_rlp_payload_type),
                          &dsdvrlp_event_payload);
    return;
  }

  /*-------------------------------------------------------------------------
    See if anything left to remove from resequencing queue.
  -------------------------------------------------------------------------*/
  dsrlprsq_remove( rrcb_ptr, DSDVRLP_MAX_FR_TO_RM_FROM_RSQ_Q );

  /*-------------------------------------------------------------------------
    Process PreQ.
  -------------------------------------------------------------------------*/
  dsdvrlpi_rx_pre_q_age_list ( rrcb_ptr, num_pkts_in_last_20ms );


  /*-------------------------------------------------------------------------
    Tell DSMGR if any data frames were received, so that dormant mode
    processing can occur.
  -------------------------------------------------------------------------*/
  ds_rlp_data_rxed( dsdvrlp_data_fr_rxed );

#if defined(FEATURE_IS2000_CHS)
  ds707_chs_rxed_data( dsdvrlp_data_fr_rxed );
#endif


  /*-------------------------------------------------------------------------
    Initializations for next round. Indicate no idle or data frames rxed.
  -------------------------------------------------------------------------*/
  dsdvrlp_data_fr_rxed = FALSE;
  dsdvrlp_age_nak_list = FALSE;

  /*-------------------------------------------------------------------------
    Record the current rlp_delay (the RTT).  At the end of this function,
    make sure that the rlp_delay has only incremented by one (max).  This
    covers the case where we receive SYNC frames on both FCH & DCCH in a
    VP2 call.
  -------------------------------------------------------------------------*/
  dsdvrlp_curr_rlp_delay = state_ptr->rlp_delay;
    
} /* dsdvrlp_process_20ms_tick() */
