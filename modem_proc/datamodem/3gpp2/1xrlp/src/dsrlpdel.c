/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L   D E L A Y E D   F R A M E 
  D E T E C T I O N  F I L E

GENERAL DESCRIPTION
  This file contains the module which is used in implementing delayed frame 
  detection.  This module should only be executed in the Rx task context.
  
EXTERNALIZED FUNCTIONS
  dsrlpdel_init()
     Called at phone power-up.  Initializes delayed frame detection vars.
  dsrlpdel_get_array
     Called when an RLP session first starts.  Returns an id to the arrays
     used for delayed frame detection.
  dsrlpdel_put_array
     Called when an RLP session finishes, returning the delayed frame
     detection arrays back to the global pool.
  dsrlpdel_reset
     Called when RLP is reset.  Cleans out the delayed frame detection
     arrays.
  dsrlpdel_q_empty
     Returns TRUE if the delayed frame detection arrays for a particular
     RLP session are empty.  Else returns FALSE.
  dsrlpdel_add
     Adds an element to the "add array".
  dsrlpdel_remove
     Removes elements from the "remove array" and adjusts the add and remove
     indices (which are internal to this module) accordingly.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Before calling functions, must first call dsrlpdel_init().
  
  Copyright (c) 2000-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/dsrlpdel.c_v   1.0   18 Oct 2000 18:07:02   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlpdel.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/18/11   op         Migrated to MSG 2.0 Macros
11/22/10   sn         Replaced REX premption macros with critical section/ISR 
                      lock for mutual exclusion.
05/10/07   ac         Lint fix
09/20/04   vas        Changes for QOS
06/08/00   ak         Added MSM5000_IRAM_FWD #ifndef's for IRAM support.
06/01/00   ak         Added check for insertion of null data.
05/15/00   na/ak      Created file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"

#if (defined( FEATURE_DS_RLP1) || defined( FEATURE_DS_RLP2) || \
     defined( FEATURE_DS_RLP3))
#include "memory.h"
#include "amssassert.h"
#include "dsrlp_v.h"
#include "dsrlpdel.h"
#include "err.h"
#include "dsrlpseq.h"
#include "dsutil.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
  Structure points to the received new frame.  
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_item_type*           fr_item_ptr;
  dsrlpi_curr_fr_info_type curr_info;       
} rlpdel_fr_type;

/*---------------------------------------------------------------------------
  An array of the delayed frames.  The array contains up to 9 items, as that
  is the max number of frames that can be received every 20 ms.
---------------------------------------------------------------------------*/
typedef struct
{
  rlpdel_fr_type     fr_arr[RLP_MAX_FWD_PDU];
  byte               index;                   /* points to next open slot  */
} rlpdel_fr_array_type;

/*---------------------------------------------------------------------------
  Each rlp session requires two of the above structs, as one represents the
  current 20 msec frames, and the other represents frames from the previous
  20 msec.
---------------------------------------------------------------------------*/
#define RLPDEL_ARR_SIZ 2
typedef struct
{
  rlpdel_fr_array_type       del_fr_arr[RLPDEL_ARR_SIZ];
  byte                       add_idx;    /* idx of array to add  frames to */
  byte                       remove_idx; /* idx of array to remove frames  */
                                         /* from                           */
  boolean                    used;       /* being used by rlp?             */
}rlpdel_fr_detect_type;

/*---------------------------------------------------------------------------
  Require the above struct for each RLP session.
---------------------------------------------------------------------------*/
LOCAL rlpdel_fr_detect_type  rlpdel_fr_detect[DSRLPI_MAX_SESSIONS];

/*---------------------------------------------------------------------------
  RLP DEL critical section variable.
---------------------------------------------------------------------------*/
LOCAL rex_crit_sect_type rlpdel_crit_sect;

/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION       DSRLPDEL_INIT

DESCRIPTION    Called at phone powerup.  Should only be called once per
               power-cycle.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_init(void)
{
  word     i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initialize critical section.
  -------------------------------------------------------------------------*/
  DS_INIT_CRIT_SECTION( &rlpdel_crit_sect );

  for (i = 0; i < DSRLPI_MAX_SESSIONS; i++)
  {
    memset(rlpdel_fr_detect[i].del_fr_arr, 
           0, 
           sizeof(rlpdel_fr_detect[i].del_fr_arr));
    rlpdel_fr_detect[i].add_idx    = 0;
    rlpdel_fr_detect[i].remove_idx = 0;
    rlpdel_fr_detect[i].used       = FALSE;
  }
}

/*===========================================================================
FUNCTION       DSRLPDEL_GET_ARRAY

DESCRIPTION    Called when an rlp session is initialized.  Assigns a delayed
               frame detection struct to the calling task.

DEPENDENCIES   None

RETURN VALUE   DSRLPDEL_NO_ARRAY if no queue could be assigned, else returns
               the index of the queue.

SIDE EFFECTS   None
===========================================================================*/
uint32 dsrlpdel_get_array(void)
{
  uint32     i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    This function is called by dsrlp_init() which gets called in an interrupt 
    context for 1x and so using ISR lock to protect 'rlpdel' data.
  -------------------------------------------------------------------------*/
  REX_ISR_LOCK(&rlpdel_crit_sect);
  for (i = 0; i < DSRLPI_MAX_SESSIONS; i++)
  {
    if (rlpdel_fr_detect[i].used == FALSE)
    {
      rlpdel_fr_detect[i].used = TRUE;
      break;
    }
  }

  /*-------------------------------------------------------------------------
    Unlock interrupts/tasks.
  -------------------------------------------------------------------------*/
  REX_ISR_UNLOCK(&rlpdel_crit_sect);

  if (i == DSRLPI_MAX_SESSIONS)
  {
    return(DSRLPDEL_NO_ARRAY);
  }
  else
  {
    return(i);
  }
}

/*===========================================================================
FUNCTION       DSRLPDEL_PUT_ARRAY

DESCRIPTION    Called when an rlp session is over.  Returns the array back to
               the free pool of delayed frame detect arrays.  Clears out the
               struct.

DEPENDENCIES   None

RETURN VALUE   NONE

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_put_array
(
  uint32      rlpdel_id
)
{
  ASSERT( (rlpdel_id < DSRLPI_MAX_SESSIONS) &&
          (rlpdel_fr_detect[rlpdel_id].used == TRUE) );

  dsrlpdel_reset(rlpdel_id);


  /*-------------------------------------------------------------------------
    Protect 'rlpdel' data using critical section as this gets called from 
    DS task as well as HDR task.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlpdel_crit_sect);
  rlpdel_fr_detect[rlpdel_id].used = FALSE;
  DS_LEAVE_CRIT_SECTION(&rlpdel_crit_sect);
}

/*===========================================================================
FUNCTION       DSRLPDEL_RESET

DESCRIPTION    Cleans up the arrays, and resets struct back to initial state.

DEPENDENCIES   None

RETURN VALUE   NONE

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_reset
(
  uint32  rlpdel_id
)
{
  rlpdel_fr_array_type*       fr_arr_ptr;  /* points to struct to clean up */
  word                        i, j;        /* for loop indices             */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rlpdel_id < DSRLPI_MAX_SESSIONS);
  ASSERT(rlpdel_fr_detect[rlpdel_id].used == TRUE);

  for (i = 0; i < RLPDEL_ARR_SIZ; i++)
  {
    fr_arr_ptr = &(rlpdel_fr_detect[rlpdel_id].del_fr_arr[i]);
    for( j = 0; j < fr_arr_ptr->index; j++)
    {
      dsm_free_packet( &( fr_arr_ptr->fr_arr[j].fr_item_ptr));
    }
    fr_arr_ptr->index = 0;
  }

  rlpdel_fr_detect[rlpdel_id].add_idx =0;
  rlpdel_fr_detect[rlpdel_id].remove_idx =0;
}

/*===========================================================================
FUNCTION       DSRLPDEL_Q_EMPTY

DESCRIPTION    Returns TRUE if both arrays which make up the delayed frame
               mechanism are empty.  Else returns FALSE.

DEPENDENCIES   None

RETURN VALUE   TRUE if both arrays are empty, else FALSE.

SIDE EFFECTS   None
===========================================================================*/
boolean dsrlpdel_q_empty
( 
  uint32  rlpdel_id
)
{
  byte i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT((rlpdel_id < DSRLPI_MAX_SESSIONS) &&
         (rlpdel_fr_detect[rlpdel_id].used == TRUE));

  for(i = 0; i < RLPDEL_ARR_SIZ; i++)
  { 
    if( rlpdel_fr_detect[rlpdel_id].del_fr_arr[i].index != 0)
    {
      return( FALSE);
    }
  }

  return( TRUE);
} /* dsrlpdel_q_empty() */


#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION       DSRLPDEL_ADD

DESCRIPTION    Adds an element to the frame detection array.  Puts it in
               order in the array.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_add
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
  dsm_item_type            *fr_item_ptr,
  dsrlpi_curr_fr_info_type *curr_info_ptr
)
{
  rlpdel_fr_array_type     *fr_arr_ptr;  /* points to array to add to      */
  rlpdel_fr_detect_type    *fr_detect_ptr;/* points to delay frame struct  */
  byte      curr_idx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT((rrcb_ptr->state.del_fr_id < DSRLPI_MAX_SESSIONS) &&
         (rlpdel_fr_detect[rrcb_ptr->state.del_fr_id].used == TRUE));

  if (fr_item_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Passed null item ptr");
    return;
  }

  fr_detect_ptr = &(rlpdel_fr_detect[rrcb_ptr->state.del_fr_id]);
  fr_arr_ptr    = &(fr_detect_ptr->del_fr_arr[fr_detect_ptr->add_idx]);

  if (fr_arr_ptr->index >= RLP_MAX_FWD_PDU)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "too many items in del. fr array");
    dsm_free_packet(&fr_item_ptr);
    return;
  }

  curr_idx = fr_arr_ptr->index;
  fr_arr_ptr->index++;

  /*-------------------------------------------------------------------------
     Insert this frame into the array, in sorted order.  If the loop 
     terminates, then this frame goes into the first slot.
  -------------------------------------------------------------------------*/
  while(curr_idx > 0)
  {
    if(DSRLPSEQ_GE_SEQ_CHECK(curr_info_ptr->seq_num,
                             fr_arr_ptr->fr_arr[curr_idx - 1].curr_info.seq_num,
                             rrcb_ptr->cfg_ptr->seq_modulus))
    {
      fr_arr_ptr->fr_arr[curr_idx].fr_item_ptr = fr_item_ptr;
      fr_arr_ptr->fr_arr[curr_idx].curr_info = *curr_info_ptr;
      return;
    }
    else
    {
      fr_arr_ptr->fr_arr[curr_idx] = fr_arr_ptr->fr_arr[curr_idx - 1];
      curr_idx--;
    }
  } /* while */ 

  fr_arr_ptr->fr_arr[0].fr_item_ptr = fr_item_ptr;
  fr_arr_ptr->fr_arr[0].curr_info   = *curr_info_ptr;
} /* dsrlpdel_add() */
#endif /* MSM5000_IRAM_FWD */

#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION       DSRLPDEL_REMOVE

DESCRIPTION    Removes the elements from the delayed frame queue.  Updates
               the add and remove indexes.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_remove
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
)
{
  byte i;
  rlpdel_fr_detect_type    *fr_detect_ptr; /*pts to frame detect struct    */
  rlpdel_fr_array_type     *fr_arr_ptr;  /* points to array to remove from */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(rrcb_ptr->state.del_fr_id < DSRLPI_MAX_SESSIONS);
  ASSERT(rlpdel_fr_detect[rrcb_ptr->state.del_fr_id].used == TRUE);

  fr_detect_ptr = &(rlpdel_fr_detect[rrcb_ptr->state.del_fr_id]);

  /*-------------------------------------------------------------------------
    If the add and remove indices indicate that both arrays in the struct
    are being used, then empty out the "remove array".  Then update the
    remove index to point to the other array.
  -------------------------------------------------------------------------*/
  if(fr_detect_ptr->remove_idx != fr_detect_ptr->add_idx)
  {
    fr_arr_ptr = &(fr_detect_ptr->del_fr_arr[fr_detect_ptr->remove_idx]);

    for(i = 0; i < fr_arr_ptr->index; i++)
    {
      /*---------------------------------------------------------------------
        Take each item out of the array, mark it as having been in the
        del fr array (so that it is not re-inserted), and then pass it to
        rx_new_data()
      ---------------------------------------------------------------------*/
      fr_arr_ptr->fr_arr[i].curr_info.fr_cat |= CURR_FRAME_IN_DELQ_MASK;
      if (fr_arr_ptr->fr_arr[i].fr_item_ptr != NULL)
      {
        (void) rrcb_ptr->cfg_ptr->ver_ptr->fcn_ptr_table->rx_new_data
        (
          rrcb_ptr,
          &fr_arr_ptr->fr_arr[i].fr_item_ptr,
          &fr_arr_ptr->fr_arr[i].curr_info
        );
        fr_arr_ptr->fr_arr[i].fr_item_ptr = NULL;
      }
      else
      {
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "NULL ptr in del frame array");
      }
    }
    fr_arr_ptr->index = 0;

    /*---------------------------------------------------------------------
      Update index so that remove's happen from the other array next time.
    ---------------------------------------------------------------------*/
    fr_detect_ptr->remove_idx = 
                            (fr_detect_ptr->remove_idx + 1) % RLPDEL_ARR_SIZ;
  }

  /*-----------------------------------------------------------------------
    This function is called after every 20-msec receive; if the "add array"
    is not empty, then update the index so that frames are now added to 
    the other array.  Note that above, the remove index was prob. updated
    to point to the current "add array", so that the "add array" is emptied
    next 20 msec period.
  -----------------------------------------------------------------------*/
  if (fr_detect_ptr->del_fr_arr[fr_detect_ptr->add_idx].index > 0)
  {
    fr_detect_ptr->add_idx = (fr_detect_ptr->add_idx + 1) % RLPDEL_ARR_SIZ;
    if (fr_detect_ptr->del_fr_arr[fr_detect_ptr->add_idx].index != 0)
    {
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "NON EMPTY ARRAY...MEM LEAK???");
    }
  }
} /* dsrlpdel_remove() */
#endif /* MSM5000_IRAM_FWD */
#endif /* any of the RLPs defined */
 
