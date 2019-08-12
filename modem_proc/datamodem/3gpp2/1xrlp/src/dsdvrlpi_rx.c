/*===========================================================================

  D A T A   S E R V I C E S   R L P   I N T E R N A L   

DESCRIPTION
  This file contains internal variable and function declarations specific to 
  the Radio Link Protocol implementation that supports EVDV.

EXTERNALIZED FUNCTIONS
  dsdvrlpi_rx_idle_frame()
    This function processes idle frames.
  
  dsdvrlpi_rx_new_data()
    This function processes new data frames.

   
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsdvrlpi_rx.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/18/11   op      Migrated to MSG 2.0 Macros
05/14/09   ls      Fix the lint issues
01/03/06   ksu     EMPA: hook up forward link with new phys link architecture
12/23/05   ksu     EMPA RSP: two fwd rx queues
12/15/05   TMR     Changed RLP RX queue to watermark and added callback data
                   to RLP RX function callback.
08/29/05   ksu/sy  Remove dsrlp_reg_hdlc() in favor of uint32 app_field
05/03/05   atp     Set app_field to hdlc instance before inserting rx_q.
01/17/05   atp     Added include of msg.h and err.h.
09/30/04   atp     modulus field now in cfg instead of ver in rscb_ptr.
07/22/04   atp     After lint.
06/08/04   atp     Removed commented out test portion of idle frame process.
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

#include "dsrlpi.h"
#include "dsdvrlpi_rx.h"
#include "dsrlprsq.h"
#include "dsrlpseq.h"
#include "queue.h"


/*===========================================================================
                      EXTERNAL DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION DSDVRLPI_RX_IDLE_FRAME

DESCRIPTION
  This function processes idle frames. Specific to DV RLP implementation.

DEPENDENCIES
  None

RETURN VALUE
  Always returns DONE.

SIDE EFFECTS
  None
===========================================================================*/
dsrlpi_fr_det_enum_type dsdvrlpi_rx_idle_frame
( 
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,     /* Ptr to RLP ctl block      */
  dsrlpi_curr_fr_info_type     *curr_info_ptr /* frame info ptr            */
)
{
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
  uint32                        modulus;      /*     sequence modulus      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT((rrcb_ptr != NULL) && (curr_info_ptr != NULL ));
  

  /*lint -save -e613 */
  state_ptr = &rrcb_ptr->state;
  modulus = rrcb_ptr->cfg_ptr->seq_modulus;

  if ( DSRLPSEQ_GT_SEQ_CHECK ( curr_info_ptr->seq_num,
                               state_ptr->expected_seq_num,
                               modulus ))
  {
    state_ptr->old_frames = 0;
    return DSRLPI_IDLE;
  }

  return DSRLPI_DONE;
  /*lint -restore */

} /* dsdvrlpi_rx_idle_frame() */


/*===========================================================================

FUNCTION DSDVRLPI_RX_NEW_DATA

DESCRIPTION
  This function processes new data frames. Specific to DV RLP implementation.

DEPENDENCIES
  CRC checks done by frame determination.
  Input pointers are non-NULL.
  The dsm_item embedded within contains only the RLP frame's data payload,
  and does not contain any header info or CRC info.

RETURN VALUE
  Returns DONE if data was passed on to higher layers, else returns NEW.

SIDE EFFECTS
  None.
===========================================================================*/
dsrlpi_fr_det_enum_type dsdvrlpi_rx_new_data
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block       */
  dsm_item_type            **fr_item_pkt_ptr,
  dsrlpi_curr_fr_info_type *curr_info_ptr    /* frame info ptr             */
)
{
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
  uint32          exp_seq;                   /* V(R)                       */
  uint32          need_seq;                  /* V(N)                       */
  uint32          rxed_seq;                  /* Seq num of rxed fr         */
  uint32          new_seq_num;               /* Calc next seq num          */
  uint32          data_len;                  /* Length of fr               */
  uint32          modulus;                   /*      sequence modulus      */
  dsm_item_type  *temp_item_ptr     = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT( (rrcb_ptr != NULL) && (curr_info_ptr != NULL) );


  /*lint -save -e613 */
  state_ptr = &rrcb_ptr->state;

  if (*fr_item_pkt_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "null dsm item ptr");
    return (DSRLPI_DONE);
  }

  exp_seq  = state_ptr->expected_seq_num;
  need_seq = state_ptr->needed_seq_num;
  rxed_seq = curr_info_ptr->seq_num;
  modulus  = rrcb_ptr->cfg_ptr->seq_modulus;
  /*lint -restore */

  /*-------------------------------------------------------------------------
    For a new data frame, the sequence number should be >= to V(R).  If 
    less than V(R), drop it.
  -------------------------------------------------------------------------*/
  if (DSRLPSEQ_GT_SEQ_CHECK( exp_seq, rxed_seq, modulus ))
  {
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "New seq 0x%x < Vr 0x%x Vn 0x%x",
             rxed_seq,
             exp_seq,
             need_seq
            );
    dsm_free_packet(fr_item_pkt_ptr);
    state_ptr->old_frames++;
    state_ptr->big_nak_gap = FALSE;
    return (DSRLPI_DONE);
  }
  else if ( rxed_seq == need_seq )
  {
    state_ptr->old_frames = 0;
    state_ptr->big_nak_gap = FALSE;

    /*----------------------------------------------------------------------
      V(N) == Rx'd seq #.

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
    data_len    = dsm_length_packet(*fr_item_pkt_ptr);

    if (data_len > 0)
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ( rxed_seq,
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
    /*lint -save -e613 */
    if (DSRLPI_HAS_LAST_SEG_MASK(curr_info_ptr->fr_cat))
    /*lint -restore */
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
    if  ( q_cnt( &(state_ptr->reseq_q) ) == 0 ) 
    {
      do
      {
        temp_item_ptr          = *fr_item_pkt_ptr;
        *fr_item_pkt_ptr       = (*fr_item_pkt_ptr)->pkt_ptr;
        temp_item_ptr->pkt_ptr = NULL;

/*lint -save -e613, -e746 */
        dsm_enqueue(rrcb_ptr->cfg_ptr->rx_wm_ptr[rrcb_ptr->route], &temp_item_ptr);
      }while (*fr_item_pkt_ptr != NULL);
      rrcb_ptr->cfg_ptr->rx_func_ptr(rrcb_ptr->cfg_ptr->rx_data);

      /*---------------------------------------------------------------------
        Set the flag if this is the first new packet transmitted out of 
        dormancy.
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.ppp_data_transfer_after_dormancy = TRUE;

      /*---------------------------------------------------------------------
        Update the statistic for Total Received Bytes only here. It also
        gets updated when data gets out of the re-sequencing queue.
      ---------------------------------------------------------------------*/
      rrcb_ptr->stats.rx_total_bytes  += data_len;
/*lint -restore */
    }
    else
    {
      dsrlprsq_insert(rrcb_ptr,curr_info_ptr,*fr_item_pkt_ptr);
    }

    /*-----------------------------------------------------------------------
      Update the state seq num's.  This must be done after the potential
      insertion into the resequencing queue above.  Otherwise, if L_VN is
      updated to 'a+1', and try to insert 'a', then reseq queue will drop
      the frame.
    -----------------------------------------------------------------------*/
    state_ptr->expected_seq_num = new_seq_num;
    state_ptr->needed_seq_num   = new_seq_num;

    return (DSRLPI_DONE);
  }
  else
  {
    state_ptr->old_frames = 0;
    return (DSRLPI_NEW);
  }

} /* dsdvrlpi_rx_new_data() */

