
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B R O A D C A S T   F L O W   S T A T E   C H A N G E   M O D U L E

GENERAL DESCRIPTION
  This module contains code related to the state machine of the Broadcast
  Flows.  Its primary function is to encapsulate the state variable and 
  require code external to this module to reference the state through access 
  functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrbccpfs_change_state() - Changes the BCCP state
  hdrbccpfs_get_new_flow_ptr() - Get a new flow pointer

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005, 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrbccpfs.c_v   1.28   07 Feb 2003 16:45:16   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccpfs.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/06   mpa     Use hdrbccp_update_flow_monitoring_status() wrapper fn.
04/05/06   mpa     Keep track of format 1 program info in flow list
03/31/06   mpa     Mainlined FEATURE_Q_NO_SELF_QPTR 
11/07/05   mpa     Fixed priority ordering
09/30/05   mpa     Enable flows through hdrbcframe instead of hdrbctask.
09/19/05   mpa     Always add new test flows at the end of the flow list
09/12/05   mpa     Added hdrbccpfs_get_new_flow_ptr().
08/28/05   mpa     Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrdebug.h"
#include "hdrtrace.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrbccpfs.h"
#include "hdrbcframe.h"
#include "hdrbccps.h"
#include "hdrbccpi.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
extern hdrbccp_flow_info_type hdrbccp_flow_info;

typedef void ( hdrbccpfs_exit_func_type ) ( hdrbccp_flow_buf_type*, hdrbccpfs_state_enum_type );
typedef void ( hdrbccpfs_enter_func_type ) ( hdrbccp_flow_buf_type*, hdrbccpfs_state_enum_type );

STATIC boolean const
  hdrbccpfs_is_valid_trans[HDRBCCPFS_NUM_STATES][HDRBCCPFS_NUM_STATES] =
{
/* To NULL  UNDESIRED  UNAV.  MON.   UNMON.   */
    { FALSE,  FALSE,   TRUE,  FALSE, FALSE }, /* From HDRBCCPFS_NULL */
    { TRUE,   FALSE,   TRUE,  FALSE, FALSE }, /* From HDRBCCPFS_UNDESIRED           */
    { TRUE,   TRUE,    FALSE, TRUE,  TRUE  }, /* From HDRBCCPFS_DESIRED_UNAVAILABLE */
    { TRUE,   TRUE,    TRUE,  FALSE, TRUE  }, /* From HDRBCCPFS_DESIRED_MONITORED   */
    { TRUE,   TRUE,    TRUE,  TRUE,  FALSE }  /* From HDRBCCPFS_DESIRED_UNMONITORED */
};

STATIC void hdrbccpfs_exit_null 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state
);


STATIC void hdrbccpfs_exit_undesired 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state
);

STATIC void hdrbccpfs_exit_desired_unavailable 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state
);

STATIC void hdrbccpfs_exit_desired_monitored 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state
);

STATIC void hdrbccpfs_exit_desired_unmonitored 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state
);

STATIC hdrbccpfs_exit_func_type * const hdrbccpfs_exit[HDRBCCPFS_NUM_STATES] =
{
  hdrbccpfs_exit_null,
  hdrbccpfs_exit_undesired,
  hdrbccpfs_exit_desired_unavailable,
  hdrbccpfs_exit_desired_monitored,
  hdrbccpfs_exit_desired_unmonitored
};

STATIC void hdrbccpfs_enter_null
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state
);

STATIC void hdrbccpfs_enter_undesired 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state
);

STATIC void hdrbccpfs_enter_desired_unavailable 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state
);

STATIC void hdrbccpfs_enter_desired_monitored 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state
);

STATIC void hdrbccpfs_enter_desired_unmonitored 
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state
);

STATIC hdrbccpfs_enter_func_type * const hdrbccpfs_enter[HDRBCCPFS_NUM_STATES] =
{
  hdrbccpfs_enter_null,
  hdrbccpfs_enter_undesired,
  hdrbccpfs_enter_desired_unavailable,
  hdrbccpfs_enter_desired_monitored,
  hdrbccpfs_enter_desired_unmonitored
};

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_ENTER_NULL

DESCRIPTION
  Called when the flow entry is removed from the Flow List

DEPENDENCIES
  None

PARAMETERS
  flow_id    - Flow ID of flow being deleted  
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_enter_null
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state 
)
{
                       
  if (prev_state != HDRBCCPFS_NULL)
  {
    /* We no longer keep track of this flow: return entry to the free queue */
    HDR_MSG_PROT_1(MSG_LEGACY_MED, "Delete Flow %d from the flow list",
                   flow_ptr->flow_info.flow_id);

    q_put(&hdrbccp_flow_info.flow_free_q, &flow_ptr->link);
  }

} /* hdrbccpfs_enter_null() */ 

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_EXIT_NULL

DESCRIPTION
  Called when BCCP Flow exits the NULL state.  This is a temporary state used
  to create a new desired flow entry.
  
  When adding a new flow entry, we always transition through the 
  DesiredUnavailable state first.  Flows will transition to Monitor or 
  Unmonitored state if applicable after bcovhd updates its ovhd info  

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to flow changing state
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

STATIC void hdrbccpfs_exit_null
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state 
)
{

} /* hdrbccpfs_exit_null() */ /*lint !e715*/

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRBCCPFS_ENTER_UNDESIRED

DESCRIPTION
  Called when BCCP enters the Undesired state.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to flow changing state
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_enter_undesired
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state 
)
{
  switch (prev_state)
  {
    case HDRBCCPFS_DESIRED_MONITORED:
    case HDRBCCPFS_DESIRED_UNMONITORED:
    case HDRBCCPFS_DESIRED_UNAVAILABLE:
    /*-----------------------------------------------------------------------
   
    -----------------------------------------------------------------------*/
    q_put( &hdrbccp_flow_info.flow_undes_priority_q, 
           q_link(flow_ptr, &flow_ptr->link) );

    break;

    default:
    /* Should not happen */
    ERR("Invalid state trans (%d undes->undes)",
        flow_ptr->flow_info.flow_id,0,0);
    break;
  }

} /* hdrbccpfs_enter_undesired() */ 

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_EXIT_UNDESIRED

DESCRIPTION
  Called when BCCP Flow exits the undesired state.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to flow changing state
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_exit_undesired
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state 
)
{
  switch (next_state)
  {
    /* Remove this flow from the Undesired queue.  It will be either returned
       to the Free queue or moved to the Desired queue. */
    case HDRBCCPFS_NULL:
    case HDRBCCPFS_DESIRED_UNAVAILABLE:

      q_delete( &hdrbccp_flow_info.flow_undes_priority_q, &flow_ptr->link );

    break;

    default:
    break;
  }
} /* hdrbccpfs_exit_undesired() */ /*lint !e715*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_ENTER_DESIRED_UNAVAILABLE

DESCRIPTION
  Called when BCCP enters the Desired Unavailable state.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to flow changing state
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_enter_desired_unavailable
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state 
)
{
  hdrbccp_flow_buf_type *q_buf_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (prev_state)
  {
    case HDRBCCPFS_NULL:
    case HDRBCCPFS_UNDESIRED:
      /*-----------------------------------------------------------------------
      Search the queue for the first element that has lesser priority than the 
      priority of the new flow entry. The new element will be inserted right 
      before this element. (1 = highest priority).
      -----------------------------------------------------------------------*/
      q_buf_ptr = (hdrbccp_flow_buf_type*) 
                   q_check(&hdrbccp_flow_info.flow_des_priority_q);

      while ( q_buf_ptr != NULL )
      {        
        if (flow_ptr->priority < q_buf_ptr->priority)
        {
          /* Found insertion point */
          break;
        }

        q_buf_ptr = (hdrbccp_flow_buf_type *)
                     q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                             &q_buf_ptr->link );
      }

      /*-----------------------------------------------------------------------
      If the q_buf_ptr is NULL, then the entry is inserted at the end of the
      priority queue, otherwise, the entry is inserted before q_buf_ptr. 
      -----------------------------------------------------------------------*/
      if( (q_buf_ptr == NULL) ||
          (hdrbccp_flow_info.test_flow_enabled) )
      {
        q_put( &hdrbccp_flow_info.flow_des_priority_q, 
               q_link(flow_ptr, &flow_ptr->link) );

      }
      else
      {
        q_insert( &hdrbccp_flow_info.flow_des_priority_q, 
                  &flow_ptr->link, 
                  &q_buf_ptr->link );
      }

    break;

    default:
    break;
  }

} /* hdrbccpfs_enter_desired_unavailable() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_EXIT_DESIRED_UNAVAILABLE

DESCRIPTION
  Called when BCCP exits the desired unavailable state.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to flow changing state
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_exit_desired_unavailable
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state 
)
{

  /*---------------------------------------------------------------------------
     If the flow is no longer desired, it needs to be removed from the 
     desired flow list.
  ---------------------------------------------------------------------------*/
  switch (next_state)
  {
    /* Remove this flow from the Desired queue.  It will be either returned
       to the Free queue or moved to the Undesired queue. */
    case HDRBCCPFS_NULL:
    case HDRBCCPFS_UNDESIRED:

      q_delete( &hdrbccp_flow_info.flow_des_priority_q, &flow_ptr->link );

    break;

    default:
    break;
  }

} /* hdrbccpfs_exit_desired_unavailable() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_ENTER_DESIRED_MONITORED

DESCRIPTION
  Called when BCCP enters the desired monitored state.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to flow changing state
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_enter_desired_monitored
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state 
)
{

  ASSERT(hdrbccps_get_state() == HDRBCCPS_OPEN_STATE);

  hdrbcframe_flow_enable(flow_ptr->flow_info.flow_id,
                         flow_ptr->data_cb,
                         HDRHAI_BROADCAST_PROTOCOL);

  /* We were not monitoring any flows, this is the first one.
     Notify the searcher. */
  if (  hdrbccp_flow_info.mon_flow_cnt == 0 )
  {
    hdrbccp_update_flow_monitoring_status(TRUE);
  }

  ++hdrbccp_flow_info.mon_flow_cnt;

} /* hdrbccpfs_enter_desired_monitored() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_EXIT_DESIRED_MONITORED

DESCRIPTION
  Called when BCCP exits the desired monitored state.

DEPENDENCIES
  None
  
PARAMETERS
  flow_ptr   - Pointer to flow changing state
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_exit_desired_monitored
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state 
)
{
  /*---------------------------------------------------------------------------
    The flow is no longer monitored, disable it.
  ---------------------------------------------------------------------------*/
   hdrbcframe_flow_disable(flow_ptr->flow_info.flow_id, 
                           HDRHAI_BROADCAST_PROTOCOL);

   --hdrbccp_flow_info.mon_flow_cnt;

   /* When we stop monitoring a flow, we set the last monitored time
      for the flow... */
   time_get_ms(flow_ptr->last_mon_time);
      
   /* We just stopped monitoring our last flow.
      Notify the searcher. */
   if ( (hdrbccps_get_state() == HDRBCCPS_OPEN_STATE) && 
        (hdrbccp_flow_info.mon_flow_cnt == 0) )
   {
     hdrbccp_update_flow_monitoring_status(FALSE);
   }

   /*---------------------------------------------------------------------------
      If the flow is no longer desired, it needs to be removed from the 
      desired flow list.
   ---------------------------------------------------------------------------*/
   switch (next_state)
   {
     /* Remove this flow from the Undesired queue.  It will be either returned
        to the Free queue or moved to the Desired queue. */
     case HDRBCCPFS_NULL:
     case HDRBCCPFS_UNDESIRED:

       q_delete( &hdrbccp_flow_info.flow_des_priority_q, &flow_ptr->link );

     break;

     default:
     break;
   }

} /* hdrbccpfs_exit_desired_monitored() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_ENTER_DESIRED_UNMONITORED

DESCRIPTION
  Called when BCCP enters the desired unmonitored state.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to flow changing state
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_enter_desired_unmonitored
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type prev_state 
)
{

} /* hdrbccpfs_enter_desired_unmonitored() */ 

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_EXIT_DESIRED_UNMONITORED

DESCRIPTION
  Called when BCCP exits the desired unmonitored state.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to flow changing state
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpfs_exit_desired_unmonitored
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type next_state 
)
{
   /*---------------------------------------------------------------------------
      If the flow is no longer desired, it needs to be removed from the 
      desired flow list.
   ---------------------------------------------------------------------------*/
   switch (next_state)
   {
     /* Remove this flow from the Undesired queue.  It will be either returned
        to the Free queue or moved to the Desired queue. */
     case HDRBCCPFS_NULL:
     case HDRBCCPFS_UNDESIRED:

       q_delete( &hdrbccp_flow_info.flow_des_priority_q, &flow_ptr->link );

     break;

     default:
     break;
   }

} /* hdrbccpfs_exit_desired_unmonitored() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCPFS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in BCCP.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr   - Pointer to the flow being processed
  new_state  - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrbccpfs_change_state
(
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type new_state
)
{

  hdrbccpfs_state_enum_type curr_state;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (flow_ptr == NULL)
  {
    ERR("NULL flow pointer in state transition",0,0,0);
    return;
  }
    
  curr_state = flow_ptr->state;

  HDRTRACE_BCMCS_FLOW_STATE(flow_ptr->flow_info.flow_id, curr_state, new_state);
                            
                            
  if (hdrbccpfs_is_valid_trans[ curr_state ][ new_state ])
  {

    /* Do exit processing from the current state */
    (*hdrbccpfs_exit[ curr_state ])( flow_ptr, new_state);

    /* Update the flow state */
    flow_ptr->state = new_state;

    /* Do entry processing for the new state */
    (*hdrbccpfs_enter[ new_state ])( flow_ptr, curr_state );

    HDRTRACE_BCMCS_FLOW_STATE_ENTRY( flow_ptr->flow_info.flow_id, new_state);
  }
  else
  {
    ERR( "Bad transition for flow_id %d (from %d to %d)", 
         flow_ptr->flow_info.flow_id, curr_state, new_state );
  }

} /* hdrbccpfs_change_state() */

/*===========================================================================

FUNCTION HDRBCCPFS_GET_NEW_FLOW_PTR

DESCRIPTION
  Helper function to get a new flow entry.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

hdrbccp_flow_buf_type* hdrbccpfs_get_new_flow_ptr( void )
{

  hdrbccp_flow_buf_type * flow_ptr;

  flow_ptr = (hdrbccp_flow_buf_type *)
               q_get( &hdrbccp_flow_info.flow_free_q );

  if ( flow_ptr == NULL )
  {
    ERR_FATAL("No free buffers available on free flow q",0,0,0);
  }

  return flow_ptr;

} /* hdrbccpfs_get_new_flow_ptr() */

/* <EJECT> */ 

#endif /* FEATURE_HDR_BCMCS */
