/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  H D R   B R O A D C A S T  P R O T O C O L
                            
                          S T A T E   M A C H I N E      

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Broadcast Control 
  Protocol state machine.

EXTERNALIZED FUNCTIONS
  hdrbccps_change_state - Change the state of the protocol.
  hdrbccps_get_state - Returns the current state.
        
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  The procotol must be activated before an access probe 
  sequence can be sent.

  Copyright (c) 2005, 2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccps.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/06   mpa     Reset the bcdb queries upon entering CLOSE state.
07/10/06   mpa     Notify searcher of the new page cycle in OPEN state.
05/15/06   mpa     Use hdrbccp_update_flow_monitoring_status() wrapper fn.
02/13/06   mpa     Pass in disable reason to disable_all_monitored_flows.
10/07/05   mpa     Added hdrbccps_powerup_init().
08/28/05   mpa     Created Module.

===========================================================================*/

/* EJECT */
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrdebug.h"
#include "hdrtrace.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrlmac.h"
#include "hdrbccps.h"
#include "hdrbccp.h"
#include "hdrbccpc.h"
#include "hdrbccpi.h"

/* EJECT */
/*===========================================================================

                             STATIC DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                           PROTOCOL STATE VARIABLE
                          
 The protocol state variable is encapsulated in this module.                      
---------------------------------------------------------------------------*/

STATIC struct
{
  hdrbccps_state_enum_type state;
} hdrbccps;

/*---------------------------------------------------------------------------
                           STATE TRANSITION TABLE 
                           
 This table indicates which state transitions are valid for this protocol.                      
---------------------------------------------------------------------------*/

STATIC const boolean hdrbccps_is_valid_transition[][ HDRBCCPS_NUM_STATES ] =
{
  { FALSE, TRUE  },                /* Transitions from the Close state. */
  { TRUE,  FALSE },                /* Transitions from the Open state.   */
};

/* EJECT */
/*===========================================================================

FUNCTION HDRBCCPS_POWERUP_INIT

DESCRIPTION
  Initializes the BCCP state module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbccps_powerup_init( void )
{
  hdrbccps.state = HDRBCCPS_CLOSE_STATE;
} /* hdrbccps_powerup_init() */

/* EJECT */
/*===========================================================================

FUNCTION HDRBCCPS_ENTER_CLOSE_STATE

DESCRIPTION
  This function performs the processing required upon entering the Close 
  state.
  
DEPENDENCIES 
  None.

PARAMETERS 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrbccps_enter_close_state ( void )
{
  hdrbccp_disable_all_monitored_flows( SYS_BCMCS_FLOW_STATUS_LOST );
  hdrbccp_closing_reset_dsdb_query();
}

/* EJECT */
/*===========================================================================

FUNCTION HDRBCCPS_EXIT_CLOSE_STATE

DESCRIPTION
  This function performs the processing required upon exiting the Close
  state.
  
DEPENDENCIES
  None.

PARAMETERS   
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrbccps_exit_close_state ( void )
{

}

/* EJECT */
/*===========================================================================

FUNCTION HDRBCCPS_ENTER_OPEN_STATE

DESCRIPTION 
  This function performs the processing required upon entering the Open 
  state.
  
DEPENDENCIES
  None.

PARAMETERS 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void hdrbccps_enter_open_state ( void )
{

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "Enter BCMCS OPEN STATE" );

  /* Send broadcast status to searcher */
  hdrbccp_update_flow_monitoring_status( hdrbccp_are_any_flows_monitored() );

  /* Set the page cycle - this notification may be redundant.  This is a catch
     all in the event that srch/hdrbc got out of sync while the AT was not
     acquired */
  if ( hdrbccp_are_any_flows_requested() )
  {
    hdrlmac_set_bcmcs_page_cycle(hdrbccpc_get_curr_bc_ovhd_period());
  }
  else
  {
    hdrlmac_set_bcmcs_page_cycle(0);
  }

}

/* EJECT */
/*===========================================================================

FUNCTION HDRBCCPS_EXIT_OPEN_STATE

DESCRIPTION  
  This function performs the processing required upon exiting the Open 
  state.
  
DEPENDENCIES  
  None.

PARAMETERS  
  None.

RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void hdrbccps_exit_open_state ( void )
{

}

/* EJECT */
/*===========================================================================

FUNCTION HDRBCCPS_CHANGE_STATE

DESCRIPTION 
  This function manages the protocol state transitions.  For valid state 
  transitions, the current state is exited by performing any required exit
  processing, then the new state is entered by perfroming any required entry 
  processing.
  
DEPENDENCIES
  None.

PARAMETERS 
  new_state - new state to enter.

RETURN VALUE
  None.
  
SIDE EFFECTS
  Exit processing for current state and entry processing for new state.  
  Protocol state is changed.
  
===========================================================================*/
void hdrbccps_change_state 
( 
  hdrbccps_state_enum_type new_state 
)
{
  /*-------------------------------------------------------------------------
   Determine if the state transition is valid.
  -------------------------------------------------------------------------*/
  if ( hdrbccps_is_valid_transition[hdrbccps.state][new_state] == FALSE )
  {
    ERR("Invalid state transition!",0,0,0);
  }
  else
  {
    HDRTRACE_STATE( HDRHAI_BROADCAST_PROTOCOL, hdrbccps.state, new_state);

    /*-----------------------------------------------------------------------
     Exit the current state.
    -----------------------------------------------------------------------*/
    switch ( hdrbccps.state )
    {
      case HDRBCCPS_CLOSE_STATE:
      {
        hdrbccps_exit_close_state();
        break;
      }

      case HDRBCCPS_OPEN_STATE:        
      {
        hdrbccps_exit_open_state();
        break;
      }
    }

    /*-----------------------------------------------------------------------
     Enter the new state.
    -----------------------------------------------------------------------*/
    switch ( new_state )
    {
      case HDRBCCPS_CLOSE_STATE:      
      {
        hdrbccps_enter_close_state();
        break;
      }

      case HDRBCCPS_OPEN_STATE: 
      {
        hdrbccps_enter_open_state();
        break;
      }
    }

    /*-----------------------------------------------------------------------
     Update the state variable.
    -----------------------------------------------------------------------*/
    hdrbccps.state = new_state;
    HDRTRACE_STATE_ENTRY( HDRHAI_BROADCAST_PROTOCOL, new_state);
  } 
}

/* EJECT */
/*===========================================================================

FUNCTION HDRBCCPS_GET_STATE

DESCRIPTION
  This function returns the current state of the protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  Current protocol state.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrbccps_state_enum_type hdrbccps_get_state ( void )
{
  return ( hdrbccps.state );
}

#endif /* FEATURE_HDR_BCMCS */
