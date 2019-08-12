
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     I D L E  S T A T E  P R O T O C O L   S T A T E

GENERAL DESCRIPTION
  This module contains code related to the state machine of idle state
  protocol (IDLE). Its primary function is to encapsulate the state
  variable and require code external to this module to reference the state
  through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdridles_change_state() - Changes the IDLE state
  hdridles_get_state()    - Returns the IDLE state
  hdridles_init()         - Initiazlie the state module
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdridles.c_v   1.18   10 Dec 2002 18:11:46   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdridles.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/12/15   vko     Do not start connection setup timer, if ACAck is not received for CR
02/03/15   sjj	   WLAN coexistence improvements to Type 7 connected Using RRC states
09/11/14   gga     Start connection setup timer when IDLE protocol moves from
                   connection setup suspend to connection setup state.
04/14/14   vko     Reset idle_reacq_suspend_to_reacq if LMAC AWAKE is recieved when IDLE is notin MONITOR
02/21/14   vko     FR2399 - improved cc handling
10/28/11   pba     Optimized handoff phase 2 support
07/27/11   cnx     Merged Optimized Handoff changes.
03/24/10   pxu     Removed FEATURE_HDR_QSM features 
03/04/09   pba     Reset connection set up sub-state when HDR task is brought 
                   online.
02/27/09   wsh     Added run-time check for KDDI feature flags
02/10/09   sju     Added support for intra scc sleep.
01/25/08   wsh     Reset ConSetup substate if idle is exiting ConSetup.
08/21/07   wsh     Made changes to support Silent Redial for sending CR+RoR 
11/15/06   wsh     Added logging when changing IDLE connection setup substate.
11/14/06   wsh     Disallow message on access probe when connection attempt is in
                   progress. This should not affect bundling with ConReq.
10/10/06   hal     Added support to throttle 1x acquisitions if AT negotiates low SCI
09/05/06   mpa     Added support for FEATURE_HDR_QSM.
07/31/06   mpa     Added support for Connection Setup Suspend state.
07/06/06   mpa     Supervision timers should be suspended instead of stopped
                   when IDLE is suspended.
03/31/06   yll     Before AT goes to sleep, if it has made an access attempt 
                   since last time entering Monitor state, then call SD's
                   reselection function.
11/15/05   mpa     Give HDRIND_IDLE_MONITOR_CONNECTION_FAILED ind on 
                   connection_setup to monitor transition
08/28/05   mpa     Removed FEATURE_HDR_BCMCS_MFRQ.
05/24/05   pba     Look at RUP state instead of RMAC to find if RTC is assigned
07/22/04   mpa     Added temporary support for bcmcs multifrequency
12/10/02   dna     Added support for HDRIDLES_SUSPEND_STATE
11/12/02   mpa     Added flag to remember if previous state was sleep
03/29/02   dna     Move call to hdrlmac_sleep to hdridle.c to pass duration
03/29/02   dna     Only give SDSS_EVT_HDR_OPR_RESEL_OK on first sleep
08/18/01   SH      Added protocol logging support
07/11/01   SH      Used the new connect reason
03/23/01   SH      Fixed a bug for entering conn setup state when RTCMAC is 
                   active
03/10/01   SH      Removed the Open Conn State
09/21/00   SH      Added slotted operation
07/28/00   SH      Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrtrace.h"
#include "rex.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdridles.h"
#include "hdridlei.h"
#include "hdrcmac.h"
#include "hdrerrno.h"
#include "hdrovhd.h"
#include "hdrlmac.h"
#include "hdrmci.h"
#include "hdrrmac.h"
#include "hdrplog.h"
#include "hdrrup.h"
#include "hdrmc.h"

#ifdef FEATURE_COEX_RPT_RAT_L3_STATE
#include "wcn_coex_mgr.h"
#endif


/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains defidleions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

typedef void ( hdridles_exit_func_type  ) ( hdridles_state_enum_type );
typedef void ( hdridles_enter_func_type ) ( hdridles_state_enum_type );

LOCAL const boolean hdridles_is_valid_trans[NUM_IDLE_STATES][NUM_IDLE_STATES] =
{
/* To IN     MO     SL     CS     ISS    CSS     TS  */
    { FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE   }, /* From IN  (INACTIVE_STATE)           */
    { TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE   }, /* From MO  (MONITOR_STATE)            */
    { TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE  }, /* From SL  (SLEEP_STATE)              */
    { TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE   }, /* From CS  (CONN_SETUP_STATE)         */
    { TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE  }, /* From ISS (IDLE_SUSPEND_STATE)       */
    { TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE  }, /* From CSS (CONN_SETUP_SUSPEND_STATE) */
    { TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE  }  /* From TS  (TUNNEL_STATE)             */
};

void hdridles_exit_inactive(hdridles_state_enum_type next_state);
void hdridles_exit_sleep(hdridles_state_enum_type next_state);
void hdridles_exit_monitor(hdridles_state_enum_type next_state);
void hdridles_exit_conn_setup(hdridles_state_enum_type next_state);
void hdridles_exit_suspend(hdridles_state_enum_type next_state);
void hdridles_exit_conn_setup_suspend( hdridles_state_enum_type next_state );
#ifdef FEATURE_LTE_TO_HDR_OH
void hdridles_exit_tunnel( hdridles_state_enum_type next_state );
#endif  /* FEATURE_LTE_TO_HDR_OH */

LOCAL hdridles_exit_func_type *const hdridles_exit[NUM_IDLE_STATES] =
{
  hdridles_exit_inactive,
  hdridles_exit_monitor,
  hdridles_exit_sleep,
  hdridles_exit_conn_setup,
  hdridles_exit_suspend,
  hdridles_exit_conn_setup_suspend
#ifdef FEATURE_LTE_TO_HDR_OH
  ,hdridles_exit_tunnel
#endif /* FEATURE_LTE_TO_HDR_OH */
};

void hdridles_enter_inactive(hdridles_state_enum_type prev_state);
void hdridles_enter_sleep(hdridles_state_enum_type prev_state);
void hdridles_enter_monitor(hdridles_state_enum_type prev_state);
void hdridles_enter_conn_setup(hdridles_state_enum_type prev_state);
void hdridles_enter_suspend(hdridles_state_enum_type prev_state);
void hdridles_enter_conn_setup_suspend( hdridles_state_enum_type prev_state );
#ifdef FEATURE_LTE_TO_HDR_OH
void hdridles_enter_tunnel( hdridles_state_enum_type prev_state );
#endif /* FEATURE_LTE_TO_HDR_OH */

LOCAL hdridles_enter_func_type *const hdridles_enter[NUM_IDLE_STATES] =
{
  hdridles_enter_inactive,
  hdridles_enter_monitor,
  hdridles_enter_sleep,
  hdridles_enter_conn_setup,
  hdridles_enter_suspend,
  hdridles_enter_conn_setup_suspend
#ifdef FEATURE_LTE_TO_HDR_OH
 ,hdridles_enter_tunnel
#endif /* FEATURE_LTE_TO_HDR_OH */
};

LOCAL struct
{
 hdridles_state_enum_type  state;  /* idle state */
 hdridles_csetup_substate_enum_type csetup_substate; 
 /* connection setup substates */
 boolean previous_state_is_sleep;  
} hdridles;
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_ENTER_INACTIVE

DESCRIPTION
  Called when IDLE enters the inactive state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_enter_inactive) */ 
void hdridles_enter_inactive
( 
  hdridles_state_enum_type prev_state  
)
{
  hdridle_stop_suspend_timer();
  hdridle_stop_1x_acq_throttling();
#ifdef FEATURE_HDR_CC_DECODE_FAIL
  hdridle_reset_cc_reacq_fail_counter(); /* reset the cc/re-acq fail counter on system loss */
#endif
}  /* hdridles_enter_inactive() */ 

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_EXIT_INACTIVE

DESCRIPTION
  Called when IDLE exits the inactive state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_exit_inactive) */ 
void hdridles_exit_inactive
( 
  hdridles_state_enum_type next_state 
)
{
} /* hdridles_exit_inactive() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_ENTER_MONITOR

DESCRIPTION
  Called when IDLE enters the monitor state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridles_enter_monitor
( 
  hdridles_state_enum_type prev_state 
)
{
  hdrovhd_wakeup(HDRHAI_IDLE_STATE_PROTOCOL);
  hdrcmac_activate(HDRHAI_IDLE_STATE_PROTOCOL);
  
  #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
  cxm_tech_l3_state_s l3_state_s;
      /* Struct to hold RAT's L3 state */	
  /*
   Report IDLE to CXM when the transition to monitor state happened from 
   HDRIDLES_CONN_SETUP_STATE or HDRIDLES_INACTIVE_STATE.
   These transitions happen when either connection setup fails or TCA timer times out
   and connection is terminated respectively.
   */
  if(( prev_state == HDRIDLES_CONN_SETUP_STATE )||
  	 ( prev_state == HDRIDLES_INACTIVE_STATE ))
  {
  	/* setting RAT's layer3 state and reporting it*/  	  	
	l3_state_s.mode = CXM_TECH_ACTIVE;
  	l3_state_s.state = CXM_TECH_L3_IDLE;
  	cxm_set_tech_l3_state(CXM_TECH_HDR, &l3_state_s);  	
  }
  #endif
  
  if(prev_state == HDRIDLES_INACTIVE_STATE)
  {
    if (!hdrmc_feature_is_enabled(
          HDRMC_FEATURE_HDR_DISABLE_SUSPEND_TIMER)) 
    {
      hdridle_start_suspend_timer();
    }
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_LOW, 
                   "Suspend timer disabled, not starting");
    }
  }

  if(prev_state == HDRIDLES_SLEEP_STATE)
  {
    /* Enter Monitor state from Sleep state */
    hdridle_enter_monitor( TRUE );
  }
  else
  {
    /* Enter Monitor state from states other than Sleep */
    hdridle_enter_monitor( FALSE );

    /* Flag that we have not entered sleep since being activated */
    hdridle_set_first_sleep_flag( TRUE );
  }

  if (prev_state == HDRIDLES_CONN_SETUP_STATE)
  {
    hdrind_give_ind( HDRIND_IDLE_MONITOR_CONNECTION_FAILED, NULL );
  }

  /* Set a signal to itself so it can check whether it can
     transition to the sleep state. Main reason to do this
     is to avoid changing state inside a state exit or entry
     processing routine */
  ( void ) rex_set_sigs( rex_self(), HDRMC_IDLE_MONITOR_ENTERED_SIG);

} /* hdridles_enter_monitor() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_EXIT_MONITOR

DESCRIPTION
  Called when IDLE exits the monitor state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_exit_monitor) */ 
void hdridles_exit_monitor
(
  hdridles_state_enum_type next_state
)
{
  hdridle_reset_qc_is_received_flag();
  hdridle_reset_idle_suspend_to_reacq_flag();
} /* hdridles_exit_monitor() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_ENTER_SLEEP

DESCRIPTION
  Called when IDLE enters the sleep state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_enter_sleep) */ 
void hdridles_enter_sleep( hdridles_state_enum_type prev_state )
{
  hdridle_set_first_sleep_flag( FALSE );
} /* hdridles_enter_sleep() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_EXIT_SLEEP

DESCRIPTION
  Called when IDLE exits the sleep state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_exit_sleep) */ 
void hdridles_exit_sleep( hdridles_state_enum_type next_state )
{
} /* hdridles_exit_sleep() */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_ENTER_CONN_SETUP

DESCRIPTION
  Called when IDLE enters the connection setup state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_enter_conn_setup) */ 
void hdridles_enter_conn_setup( hdridles_state_enum_type prev_state )
{
#ifdef FEATURE_LTE_TO_HDR_OH
  if (!hdrsap_tunnel_mode_is_enabled(HDRHAI_IDLE_STATE_PROTOCOL))
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdrovhd_wakeup(HDRHAI_IDLE_STATE_PROTOCOL);
    hdrcmac_activate(HDRHAI_IDLE_STATE_PROTOCOL);  
  }

  /* when we enter connection_setup, ConReq hasn't been given to RMAC */
  if (prev_state != HDRIDLES_CONN_SETUP_SUSPEND_STATE)
  {
    if(hdrrup_is_tc_assigned(HDRHAI_IDLE_STATE_PROTOCOL) )
    {
      /* a connection setup is already in progress update the
         connection setup reason to include AN as well         
      */
      hdridles_change_csetup_substate(HDRIDLES_CSETUP_TCA_RCVD);
      hdridle_set_connect_reason(HDRIDLE_CR_AN);

      /* start a state timer */
      hdridle_start_conn_setup_timer (HDRIDLE_CONN_SETUP_TIMER2_MSECS);
    }
    else
    {
      hdridle_send_conn_req_msg();
      hdridles_change_csetup_substate(HDRIDLES_CSETUP_CREQ_SLP);
    }
  }/* if (prev_state != HDRIDLES_CONN_SETUP_SUSPEND_STATE) */

} /* hdridles_enter_conn_setup() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_EXIT_CONN_SETUP

DESCRIPTION
  Called when IDLE exits the connection setup state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_exit_conn_setup) */ 
void hdridles_exit_conn_setup
(
  hdridles_state_enum_type next_state
)
{
  hdridle_stop_conn_setup_timer();
  if (next_state != HDRIDLES_CONN_SETUP_SUSPEND_STATE)
  {
    hdridles_change_csetup_substate(HDRIDLES_CSETUP_INVALID);
    hdridle_set_cr_ror_progress(FALSE);
  }
} /* hdridles_exit_conn_setup() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_ENTER_SUSPEND

DESCRIPTION
  Called when IDLE enters the suspend state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_enter_suspend) */ 
void hdridles_enter_suspend( hdridles_state_enum_type prev_state )
{

  /* The connection setup failed while suspended, notify protocols of the 
     failure.  If we are already suspended, there is no needed to suspend
     ovhd and cmac again. */
  if (prev_state == HDRIDLES_CONN_SETUP_SUSPEND_STATE)
  {
    hdrind_give_ind( HDRIND_IDLE_MONITOR_CONNECTION_FAILED, NULL );
  }
  else
  {              
    hdrovhd_suspend_and_sleep(HDRHAI_IDLE_STATE_PROTOCOL);
    hdrcmac_suspend_and_deactivate(HDRHAI_IDLE_STATE_PROTOCOL);
  }


} /* hdridles_enter_suspend() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_EXIT_SUSPEND

DESCRIPTION
  Called when IDLE exits the suspend state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_exit_suspend) */ 
void hdridles_exit_suspend( hdridles_state_enum_type next_state )
{
} /* hdridles_exit_suspend() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_ENTER_CONN_SETUP_SUSPEND

DESCRIPTION
  Called when IDLE enters the connection setup suspend state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_enter_conn_setup_suspend) */ 
void hdridles_enter_conn_setup_suspend( hdridles_state_enum_type prev_state )
{
  hdrovhd_suspend_and_sleep(HDRHAI_IDLE_STATE_PROTOCOL);
  hdrcmac_suspend_and_deactivate(HDRHAI_IDLE_STATE_PROTOCOL);
} /* hdridles_enter_conn_setup_suspend() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_EXIT_CONN_SETUP_SUSPEND

DESCRIPTION
  Called when IDLE exits the connection setup suspend state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_exit_conn_setup_suspend) */ 
void hdridles_exit_conn_setup_suspend( hdridles_state_enum_type next_state )
{
  if (next_state != HDRIDLES_CONN_SETUP_STATE)
  {
    hdridles_change_csetup_substate(HDRIDLES_CSETUP_INVALID);
    hdridle_set_cr_ror_progress(FALSE);
  }
} /* hdridles_exit_conn_setup_suspend() */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRIDLES_ENTER_TUNNEL

DESCRIPTION
  Called when IDLE enters the tunnel state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_enter_tunnel) */ 
void hdridles_enter_tunnel( hdridles_state_enum_type prev_state )
{

} /* hdridles_enter_tunnel() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_EXIT_TUNNEL

DESCRIPTION
  Called when IDLE exits the tunnel state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdridles_exit_tunnel) */ 
void hdridles_exit_tunnel( hdridles_state_enum_type next_state )
{

} /* hdridles_exit_tunnel() */
#endif /* FEATURE_LTE_TO_HDR_OH */


/* <EJECT> */ 
/*===========================================================================

FUNCTION hdridles_change_state
DESCRIPTION
  Called to execute a state transition in IDLE.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridles_change_state
(
  hdridles_state_enum_type new_state
)
{

  hdridles_state_enum_type prev_state; /* previous state */

  HDRTRACE_STATE( HDRHAI_IDLE_STATE_PROTOCOL, 
    hdridles.state, new_state);

  if (hdridles_is_valid_trans[ hdridles.state ][ new_state ])
  {
    /* Do exit processing from current state */
    (*hdridles_exit[ hdridles.state ])( new_state );

    /* Change the state */
    prev_state     = hdridles.state;
    hdridles.state = new_state;
    
    /* Do entry processing for the current state */
    (*hdridles_enter[ hdridles.state ])( prev_state );

    /* Flag if we just exited sleep state */
    hdridles.previous_state_is_sleep = 
      (prev_state == HDRIDLES_SLEEP_STATE);

    HDRTRACE_STATE_ENTRY( HDRHAI_IDLE_STATE_PROTOCOL, new_state);
    hdrplog_log_state_info();
  }
  else
  {
    ERR_FATAL( "Bad transition %d to %d", hdridles.state, new_state, 0 );
  }
} /* hdridles_change_state() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the IDLE.
  This funciton is only to be used by other code in the IDLE.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The current state of IDLE

SIDE EFFECTS
  None

===========================================================================*/

hdridles_state_enum_type hdridles_get_state( void )
{
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "hdridles_get_state %d", hdridles.state);

  return ( hdridles.state );
} /* hdridles_get_state() */

/*===========================================================================

FUNCTION HDRIDLES_IS_PREVIOUS_STATE_SLEEP

DESCRIPTION
  Called to check the previous state of the IDLE for sleep.
  This funciton is only to be used by other code in the IDLE.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  - Previous IDLE state was sleep
  FALSE - Previous IDLE state was not sleep

SIDE EFFECTS
  None

===========================================================================*/

boolean hdridles_is_previous_state_sleep( void )
{

  return ( hdridles.previous_state_is_sleep );

} /* hdridles_is_previous_state_sleep() */
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLES_INIT

DESCRIPTION
  This function initializes the idle state protocol's state module.

DEPENDENCIES
  This function must called every time when the acess terminal enters 
  the HDR mode and before the idle state protocol can be activated

PARAMETERS
  None

RETURN VALUE
  none
  
SIDE EFFECTS
  None

===========================================================================*/

void hdridles_init( void )
{
  hdridles.state = HDRIDLES_INACTIVE_STATE;
  hdridles.csetup_substate = HDRIDLES_CSETUP_INVALID;
} /* hdridles_init() */


/*===========================================================================

FUNCTION hdridles_change_csetup_substate
DESCRIPTION
  Called to execute a state transition in IDLE.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridles_change_csetup_substate
(
  hdridles_csetup_substate_enum_type new_state
)
{
  HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,  "IDLE CSETUP substate %d -> %d",
                  hdridles.csetup_substate, new_state);
  hdridles.csetup_substate = new_state;
}

/*===========================================================================

FUNCTION hdridles_get_csetup_substate
DESCRIPTION
  Called to execute a state transition in IDLE.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  Current Connection Setup Substate

SIDE EFFECTS
  None

===========================================================================*/
hdridles_csetup_substate_enum_type hdridles_get_csetup_substate
(
  void
)
{  
  return hdridles.csetup_substate;
}

/*===========================================================================

FUNCTION hdridles_creq_given_to_acmac()
DESCRIPTION
  Called to execute a state transition in IDLE.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridles_creq_given_to_acmac
(
  void
)
{
  hdridles_change_csetup_substate(HDRIDLES_CSETUP_CREQ_ACMAC);
}
