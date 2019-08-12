/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                R O U T E   U P D A T E   P R O T O C O L
                        S T A T E   M O D U L E

GENERAL DESCRIPTION
  This module contains code related to the state machine of RUP.  Its
  primary function is to encapsulate the state variable and require code
  external to this module to reference the state through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrrups_change_state() - Changes RUP state
  hdrrups_get_state()    - Returns RUP state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  
All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrrups.c_v   1.15   17 Oct 2002 15:16:04   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrrups.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/14/14   vko/dsp FR19338:Reduce call setup time by skipping RF Tx shutdown.
11/02/12   wsh     Disabled tuneaway after active HO for 3 sec.  
07/27/11   cnx     Merged Optimized Handoff changes.
08/08/11   wsh     Fixed race cond. in RMAC shutdown, created RX/TX crit_sec
09/18/09   wsh     CMI: support changes from modem DATA
09/07/07   sju     Added support for revB RU msg
02/01/07   sju     Added support for QOS HHO
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
03/07/05   pba     Modified RUP timers to use callback functions instead of 
                   signals.
03/07/05   pba     Added synchronization between RUP and PAC to allow 
                   initialization of RLP before AT starts receiving data on FTC
01/27/04   vas     Added code to initialize/cleanup RLP when TCAM is received,
                   doing it before the connect is actually opened to avoid 
                   initial packet loss.
05/13/03   mpa     Initialize TCAM seqNum when receiving first TCAM in idle.
                   Not when returning to idle.
10/09/02   mpa     Added Set Management Override functionality
08/02/02   vas     Do not command LMAC to idle when entering Idle state if
                   prev. state is conn setup. This is taken care of elsewhere.
09/04/01   dna     Added HDRIND_RUP_CONNECTION_INITIATED based on DDR-46
                   to reset SLP before reliable RTCACK message comes.
08/18/01   sh      Added protocol logging support
06/01/01   vas     Re-naming for consistency. Code cleanup. Some re-organizing
06/27/00   vas     Created module.

=============================================================================*/

/* <EJECT> */

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "hdr_variation.h"
#include "hdrtrace.h"
#include "comdef.h"
#include "hdrdebug.h"
#include "err.h"
#include "rex.h"
#include "hdrrup.h"
#include "hdrrupc.h"
#include "hdrrupi.h"
#include "hdrrups.h"
#include "hdrlmac.h"
#include "hdramac.h"
#include "hdrfmac.h"
#include "hdrrmac.h"
#include "hdrplog.h"
#include "hdrpac_api.h"

/* <EJECT> */

/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

LOCAL const boolean hdrrups_is_valid_trans[NUM_RUP_STATES][NUM_RUP_STATES] = 
{
/* To INACTIVE  IDLE   CONN_SETUP CONNECTED  SYNC_CONN  TUNNEL*/
  {    FALSE,   TRUE,    FALSE,     FALSE,     FALSE,    TRUE    }, /* From INACTIVE  */
  {    TRUE,    FALSE,   FALSE,     FALSE,     TRUE,     TRUE    }, /* From IDLE      */
  {    TRUE,    TRUE,    FALSE,     TRUE,      FALSE,    FALSE   }, /* From CONN_SETUP*/
  {    TRUE,    TRUE,    FALSE,     FALSE,     FALSE,    FALSE   }, /* From CONNECTED */
  {    TRUE,    TRUE,    TRUE,      FALSE,     FALSE,    TRUE    }, /* From SYNC_CONN */
  {    TRUE,    TRUE,    FALSE,      FALSE,     TRUE,    FALSE   }, /* From TUNNEL */
};

/* Protocol State variable */
LOCAL struct 
{
  hdrrups_state_enum_type state;
} hdrrups;

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*=============================================================================

                  LOCAL FUNCTION DECLARATIONS FOR MODULE

=============================================================================*/

/* <EJECT> */

/*=============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION HDRRUPS_POWERUP_INIT

DESCRIPTION
  Initializes the RUP state module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrups_powerup_init( void )
{
  hdrrups.state = HDRRUPS_INACTIVE;
} /* hdrrups_powerup_init() */

/*===========================================================================
FUNCTION hdrrups_enter_inactive_state

DESCRIPTION
  This function performs the processing required upon entry to the inactive
  state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrrups_enter_inactive_state ( void )
{
  hdrlmac_deactivate(HDRHAI_ROUTE_UPDATE_PROTOCOL);
} /* hdrrups_enter_inactive_state() */

/* <EJECT> */

/*===========================================================================
FUNCTION hdrrups_enter_idle_state

DESCRIPTION
  This function performs the processing required upon entry to the idle
  state. Resets the expected Traffic Channel Assignment message sequence
  number and sets the LMAC to idle mode.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrrups_enter_idle_state ( void )
{
  /*---------------------------------------------------------------------------
  If the previous state is Connection Setup, then do not command searcher mode 
  back to Idle, since this would already have been  called from 
  hdrrup_process_conn_setup_to_idle_transition(). 
  
  This is done in the above function instead of here to avoid the following 
  race condition:
  Searcher needs to be in idle state before it can process Idle Channel change
  command. Since the process_conn_setup_to_idle function takes care of 
  reverting back to the original channel before entering Connection Setup, it
  needs to command searcher to idle first. Since Searcher is already in Idle
  when this function is called, we do not need to command it back to Idle here
  ---------------------------------------------------------------------------*/
  if( ( hdrrups.state != HDRRUPS_CONNECTION_SETUP )&& 
      ( hdrrups.state != HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP ) )
  {
    hdrlmac_idle_mode(HDRHAI_ROUTE_UPDATE_PROTOCOL);
  }
} /* hdrrups_enter_idle_state() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUPS_ENTER_SYNC_CONN_SETUP_STATE

DESCRIPTION
  This function performs the processing required upon entry to the 
  synchronize connection setup state. Start the Timer for RUP to wait
  processing connection setup until PAC initializes RLP. Indicate 
  we are starting to open connection to reset SLP.
  
DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrrups_enter_sync_conn_setup_state ( void )
{
  /*-------------------------------------------------------------------------
  Start RUP-PAC synchronization Timer
  -------------------------------------------------------------------------*/
  hdrrup_start_state_timer();

  hdrind_give_ind( HDRIND_RUP_CONNECTION_INITIATED, NULL );
}

/* <EJECT> */
/*===========================================================================
FUNCTION hdrrups_enter_connection_setup_state

DESCRIPTION
  This function performs the processing required upon entry to the 
  connection setup state.
  
DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrrups_enter_conn_setup_state ( void )
{
  boolean skip_disable_tx = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Start Connection Timer 
  -------------------------------------------------------------------------*/
  hdrrup_start_conn_setup_timer();

  /*-------------------------------------------------------------------------
  Deactivate ACMAC & Activate RTC MAC & FTC MAC 
  -------------------------------------------------------------------------*/
  if(hdrlmac_feature_skip_disable_tx_enabled() == TRUE)
  {
    skip_disable_tx = hdrrup_cp_skip_disable_tx_enabled();
    hdrrup_reset_cp_skip_disable_tx();
  }

  hdramac_deactivate(HDRHAI_ROUTE_UPDATE_PROTOCOL, skip_disable_tx);
  hdrrmac_activate(HDRHAI_ROUTE_UPDATE_PROTOCOL);
  hdrfmac_activate(HDRHAI_ROUTE_UPDATE_PROTOCOL);

} /* hdrrups_enter_conn_setup_state() */

/* <EJECT> */

/*===========================================================================
FUNCTION hdrrups_enter_connected_state

DESCRIPTION
  This function performs the processing required upon entry to the connected
  state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrrups_enter_connected_state ( void )
{
} /* hdrrups_enter_connected_state() */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION hdrrups_enter_tunnel_state

DESCRIPTION
  This function performs the processing required upon entry to the tunnel
  state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrrups_enter_tunnel_state ( void )
{
} /* hdrrups_enter_connected_state() */

#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================
FUNCTION hdrrups_exit_inactive_state

DESCRIPTION
  This function performs the processing required upon exit from the inactive
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
void hdrrups_exit_inactive_state ( void )
{
} /* hdrrups_exit_inactive_state() */

/* <EJECT> */

/*===========================================================================
FUNCTION hdrrups_exit_idle_state

DESCRIPTION
  This function performs the processing required upon exit from the idle
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
void hdrrups_exit_idle_state ( void )
{
} /* hdrrups_exit_idle_state() */

/* <EJECT> */

/*===========================================================================
FUNCTION hdrrups_exit_sync_conn_setup_state

DESCRIPTION
  This function performs the processing required upon exit from the 
  synchronize connection setup state.
    
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void hdrrups_exit_sync_conn_setup_state ( void )
{
  /*-------------------------------------------------------------------------
  Stop RUP-PAC synchronization Timer
  -------------------------------------------------------------------------*/
  hdrrup_stop_state_timer();

} /* hdrrups_exit_sync_conn_setup_state() */

/* <EJECT> */

/*===========================================================================
FUNCTION hdrrups_exit_conn_setup_state

DESCRIPTION
  This function performs the processing required upon exit from the 
  connection setup state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrups_exit_conn_setup_state ( void )
{
  /*-------------------------------------------------------------------------
  Stop Connection Timer 
  -------------------------------------------------------------------------*/
  hdrrup_stop_conn_setup_timer();

} /* hdrrups_exit_conn_setup_state() */

/* <EJECT> */

/*===========================================================================
FUNCTION hdrrups_exit_connected_state

DESCRIPTION
  This function performs the processing required upon exit from the connected
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
void hdrrups_exit_connected_state ( void )
{
  /*-------------------------------------------------------------------------
  Restore set mngmt params if overridden in connected state
  -------------------------------------------------------------------------*/
  hdrrupc_restore_current_srch_params();

  /*-------------------------------------------------------------------------
  Deactivate RTC MAC & FTC MAC. Reset flag that indicates a neighborlist msg
  was received while connected
  -------------------------------------------------------------------------*/
  hdrrmac_deactivate(HDRHAI_ROUTE_UPDATE_PROTOCOL);
  hdrfmac_deactivate(HDRHAI_ROUTE_UPDATE_PROTOCOL);

  /*-------------------------------------------------------------------------
  We are exiting connected state. Clean up the RLP if it had been initialized
  -------------------------------------------------------------------------*/
  hdrpac_mdm_sn_cleanup();

  /* Make the MC RUP INFO Buffer available while exiting the connected state*/
  hdrrup_init_mc_rup_info_buffer();

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrrup_set_tuneaway_allowed( TRUE );
#endif /* FEATURE_LTE_TO_HDR_OH */
} /* hdrrups_exit_connected_state() */

/* <EJECT> */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION hdrrups_exit_tunnel_state

DESCRIPTION
  This function performs the processing required upon exit from the tunnel
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
void hdrrups_exit_tunnel_state ( void )
{

} /* hdrrups_exit_tunnel_state() */

#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

FUNCTION HDRRUPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in RUP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrups_change_state
(  
  hdrrups_state_enum_type new_state 
)
{
  HDRTRACE_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL, 
    hdrrups.state, new_state);

  #ifdef FEATURE_HDR_QOS
  if ( ( hdrrups.state == HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP &&
         new_state != HDRRUPS_CONNECTION_SETUP ) ||
        ( hdrrups.state == HDRRUPS_CONNECTION_SETUP &&
          new_state != HDRRUPS_CONNECTED ) )
  {
    /* Inform ReservationModule about TCA failed, it is needed for QOS HHO*/
    hdrind_give_ind( HDRIND_RUP_ASSIGNMENT_FAILED, NULL );
  }
  #endif

  /*-------------------------------------------------------------------------
   Determine if the state transition is valid.
  -------------------------------------------------------------------------*/
  if ( hdrrups_is_valid_trans[ hdrrups.state ][ new_state ] == FALSE )
  {
    ERR("Invalid state transition!",0,0,0);
  }
  else
  {
    /*-----------------------------------------------------------------------
     Exit current state.
    -----------------------------------------------------------------------*/
    switch ( hdrrups.state )
    {
      case HDRRUPS_INACTIVE:
        hdrrups_exit_inactive_state();
        break;

      case HDRRUPS_IDLE: 
        hdrrups_exit_idle_state();
        break;

      case HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP:
        hdrrups_exit_sync_conn_setup_state();
        break;

      case HDRRUPS_CONNECTION_SETUP: 
        hdrrups_exit_conn_setup_state();
        break;

      case HDRRUPS_CONNECTED: 
        hdrrups_exit_connected_state();
        break;

#ifdef FEATURE_LTE_TO_HDR_OH
      case HDRRUPS_TUNNEL: 
        hdrrups_exit_tunnel_state();
        break;
#endif /* FEATURE_LTE_TO_HDR_OH */

      default:
        ERR_FATAL("Bad Current State!",0,0,0);
    }

    /*-----------------------------------------------------------------------
     Enter new state.
    -----------------------------------------------------------------------*/
    switch ( new_state )
    {
      case HDRRUPS_INACTIVE:
        hdrrups_enter_inactive_state();
        break;

      case HDRRUPS_IDLE: 
        hdrrups_enter_idle_state();
        break;

      case HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP:
        hdrrups_enter_sync_conn_setup_state();
        break;

      case HDRRUPS_CONNECTION_SETUP: 
        hdrrups_enter_conn_setup_state();
        break;

      case HDRRUPS_CONNECTED: 
        hdrrups_enter_connected_state();
        break;

#ifdef FEATURE_LTE_TO_HDR_OH
      case HDRRUPS_TUNNEL: 
        hdrrups_enter_tunnel_state();
        break;
#endif /* FEATURE_LTE_TO_HDR_OH */

      default:
        ERR_FATAL("Bad New State!",0,0,0);
    }

    /*-----------------------------------------------------------------------
     Update state variable.
    -----------------------------------------------------------------------*/
    hdrrups.state = new_state;

    HDRTRACE_STATE_ENTRY( HDRHAI_ROUTE_UPDATE_PROTOCOL, new_state);
    hdrplog_log_state_info();
  } /* if valid state transition */
} /* hdrrups_change_state() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the RUP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
hdrrups_state_enum_type hdrrups_get_state( void )
{
  return hdrrups.state;
} /* hdrrups_get_state() */
