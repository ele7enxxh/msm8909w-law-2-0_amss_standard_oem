
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     I N I T I A L I Z A T I O N  S T A T E  P R O T O C O L   S T A T E

GENERAL DESCRIPTION
  This module contains code related to the state machine of initialization
  state protocol (INIT). Its primary function is to encapsulate the state
  variable and require code external to this module to reference the state
  through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrinits_change_state() - Changes the INIT state
  hdrinits_get_state()    - Returns the INIT state
  hdrinits_init()         - Initialize the INIT state module 
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrinits.c_v   1.13   12 Nov 2002 16:42:48   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrinits.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/13   sat     NV control for Lock Release Enhancement (LRE)
08/29/13   vko     FR2812 - Pausing sync timer from HDRCP when HDRSRCH is in 2a state
05/02/12   wsh     Start L2D HO hystr. timer in timing transfer state
03/20/12   cnx     Fixed compiler errors for Dime.
11/08/11   cnx     Fixed compiler warnings.
10/28/11   pba     Optimized handoff phase 2 support
05/11/10   pba     Added new states to handle LTE to HDR reselection.
11/12/02   dna     Moved call to hdrlmac_acquire() to hdrinit.c
03/15/02   dna     Removed 1 minute acquisition timer.
08/18/01   SH      Added protocol logging support
07/02/00   SH      Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrinits.h"
#include "hdriniti.h"
#include "hdrcmac.h"
#include "hdrerrno.h"
#include "hdrlmac.h"
#include "hdrplog.h"
#include "sd.h"
#ifdef FEATURE_LTE_TO_HDR
#include "hdrmc_v.h"
#endif /* FEATURE_LTE_TO_HDR */

#ifdef  FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif  /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

typedef void ( hdrinits_exit_func_type  ) ( hdrinits_state_enum_type );
typedef void ( hdrinits_enter_func_type ) ( hdrinits_state_enum_type );


LOCAL const boolean hdrinits_is_valid_trans[NUM_INIT_STATES][NUM_INIT_STATES] =
{
/* To IN     NT     PA     SY     TT    FA    SS*/
    { FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE }, /* From IN (HDRINITS_INACTIVE_STATE)      */
    { TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE }, /* From NT (HDRINITS_NET_DET_STATE)       */
    { TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE }, /* From PA (HDRINITS_PILOT_ACQ_STATE)     */
    { TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE  }, /* From SY (HDRINITS_SYNC_STATE)          */
    { TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE }, /* From TT (HDRINITS_TIME_TRANSFER_STATE) */
    { TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE }, /* From FA (HDRINITS_FAST_ACQ_STATE)      */
	{ TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE }  /* From SS (HDRINITS_SYNC_SUSPEND_STATE)  */
};

void hdrinits_exit_inactive(hdrinits_state_enum_type next_state);
void hdrinits_exit_net_det(hdrinits_state_enum_type next_state);
void hdrinits_exit_pilot_acq(hdrinits_state_enum_type next_state);
void hdrinits_exit_sync(hdrinits_state_enum_type next_state);
void hdrinits_exit_time_transfer(hdrinits_state_enum_type next_state);
void hdrinits_exit_fast_acq(hdrinits_state_enum_type next_state);
void hdrinits_exit_sync_suspend(hdrinits_state_enum_type next_state);

LOCAL  hdrinits_exit_func_type *const hdrinits_exit[NUM_INIT_STATES] =
{
  hdrinits_exit_inactive,
  hdrinits_exit_net_det,
  hdrinits_exit_pilot_acq,
  hdrinits_exit_sync,
  hdrinits_exit_time_transfer,
  hdrinits_exit_fast_acq,
  hdrinits_exit_sync_suspend
};

void hdrinits_enter_inactive(hdrinits_state_enum_type prev_state);
void hdrinits_enter_net_det(hdrinits_state_enum_type prev_state);
void hdrinits_enter_pilot_acq(hdrinits_state_enum_type prev_state);
void hdrinits_enter_sync(hdrinits_state_enum_type prev_state);
void hdrinits_enter_time_transfer(hdrinits_state_enum_type next_state);
void hdrinits_enter_fast_acq(hdrinits_state_enum_type next_state);
void hdrinits_enter_sync_suspend(hdrinits_state_enum_type prev_state);

LOCAL  hdrinits_enter_func_type *const hdrinits_enter[NUM_INIT_STATES] =
{
  hdrinits_enter_inactive,
  hdrinits_enter_net_det,
  hdrinits_enter_pilot_acq,
  hdrinits_enter_sync,
  hdrinits_enter_time_transfer,
  hdrinits_enter_fast_acq,
  hdrinits_enter_sync_suspend
};

LOCAL hdrinits_state_enum_type hdrinits_state;

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_enter_inactive

DESCRIPTION
  Called when INIT enters the inactive state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_enter_inactive) */
void hdrinits_enter_inactive( hdrinits_state_enum_type prev_state )
{
  hdrinit_stop_sync_msg_timer();
} /* hdrinits_enter_inactive() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_exit_inactive

DESCRIPTION
  Called when INIT exits the inactive state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_exit_inactive) */
void hdrinits_exit_inactive( hdrinits_state_enum_type next_state )
{
} /* hdrinits_exit_inactive() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_enter_net_det

DESCRIPTION
  Called when INIT enters the network determination state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_enter_net_det) */
void hdrinits_enter_net_det( hdrinits_state_enum_type prev_state )
{
} /* hdrinits_enter_init() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_exit_net_det

DESCRIPTION
  Called when INIT exits the network determination state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_exit_net_det) */
void hdrinits_exit_net_det( hdrinits_state_enum_type next_state )
{
#ifdef  FEATURE_LTE_TO_HDR_OH
  if (hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL))
  {
    hdrlmac_activate( HDRHAI_INITIALIZATION_STATE_PROTOCOL );
  }
#endif  /* FEATURE_LTE_TO_HDR_OH */

} /* hdrinits_exit_init() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_enter_pilot_acq

DESCRIPTION
  Called when INIT enters the pilot acquisition state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_enter_pilot_acq) */
void hdrinits_enter_pilot_acq( hdrinits_state_enum_type prev_state )
{
#ifdef FEATURE_LTE_TO_HDR
  if (hdrmc_lte_to_hdr_redir_is_in_progress() ||
      hdrmc_hdr_to_lte_time_transfer_is_in_progress())
  {
    hdrinit_start_lte_to_hdr_ho_hystr_timer();
  }
#endif /* FEATURE_LTE_TO_HDR */
} /* hdrinits_enter_pilot_acq() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_exit_pilot_acq

DESCRIPTION
  Called when INIT exits the pilot acquisition state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_exit_pilot_acq) */
void hdrinits_exit_pilot_acq
(
  hdrinits_state_enum_type next_state
)
{
#ifdef FEATURE_LTE_TO_HDR
  if (hdrmc_lte_to_hdr_redir_is_in_progress() ||
      hdrmc_hdr_to_lte_time_transfer_is_in_progress())
  {
    hdrinit_stop_lte_to_hdr_ho_hystr_timer();
  }
#endif /* FEATURE_LTE_TO_HDR */
} /* hdrinits_exit_pilot_acq() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_enter_sync

DESCRIPTION
  Called when INIT enters the synchronization state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_enter_sync) */
void hdrinits_enter_sync( hdrinits_state_enum_type prev_state )
{
  if((prev_state != HDRINITS_SYNC_SUSPEND_STATE) || 
                                    (!hdrlmac_is_lre_enabled()))
  {
     hdrcmac_activate(HDRHAI_INITIALIZATION_STATE_PROTOCOL);
  }
  hdrinit_start_sync_msg_timer();

} /* hdrinits_enter_sync() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_exit_sync

DESCRIPTION
  Called when INIT exits the synchronization state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_exit_sync) */
void hdrinits_exit_sync
(
  hdrinits_state_enum_type next_state
)
{
  if((next_state != HDRINITS_SYNC_SUSPEND_STATE) || 
                                    (!hdrlmac_is_lre_enabled()) )
     hdrinit_stop_sync_msg_timer();
  else
     hdrinit_pause_and_stop_sync_msg_timer(); /* Note the current timer value and stop it*/  
} /* hdrinits_exit_sync() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRINITS_ENTER_TIME_TRANSFER

DESCRIPTION
  Called when INIT enters the time transfer state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_enter_time_transfer) */
void hdrinits_enter_time_transfer( hdrinits_state_enum_type prev_state )
{

} /* hdrinits_enter_time_transfer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRINITS_EXIT_TIME_TRANSFER

DESCRIPTION
  Called when INIT exits the time transfer state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_exit_time_transfer) */
void hdrinits_exit_time_transfer
(
  hdrinits_state_enum_type next_state
)
{
  
} /* hdrinits_exit_time_transfer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRINITS_ENTER_FAST_ACQ

DESCRIPTION
  Called when INIT enters the fast acquisition state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_enter_fast_acq) */
void hdrinits_enter_fast_acq( hdrinits_state_enum_type prev_state )
{
#ifdef FEATURE_LTE_TO_HDR
  if (hdrmc_lte_to_hdr_resel_is_in_progress())
  {
    hdrinit_start_lte_to_hdr_ho_hystr_timer();
  }
#endif /* FEATURE_LTE_TO_HDR */
} /* hdrinits_enter_fast_acq() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRINITS_EXIT_FAST_ACQ

DESCRIPTION
  Called when INIT exits the fast acquisition state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715, hdrinits_exit_fast_acq) */
void hdrinits_exit_fast_acq
(
  hdrinits_state_enum_type next_state
)
{
#ifdef FEATURE_LTE_TO_HDR
  if (hdrmc_lte_to_hdr_resel_is_in_progress())
  {
    hdrinit_stop_lte_to_hdr_ho_hystr_timer();
  }
#endif /* FEATURE_LTE_TO_HDR */
} /* hdrinits_exit_fast_acq() */

/*===========================================================================

FUNCTION hdrinits_enter_sync_suspend

DESCRIPTION
  Called when INIT enters the synchronization state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinits_enter_sync_suspend( hdrinits_state_enum_type prev_state )
{   
  hdrinit_start_sync_suspend_timer();
} /* hdrinits_enter_suspend_sync() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_exit_suspend_sync

DESCRIPTION
  Called when INIT exits the synchronization state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinits_exit_sync_suspend
(
  hdrinits_state_enum_type next_state
)
{
   hdrinit_stop_sync_suspend_timer();

   if(next_state != HDRINITS_SYNC_STATE)
       hdrinit_reset_saved_sync_msg_timer();
} /* hdrinits_exit_suspend_sync() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrinits_change_state
DESCRIPTION
  Called to execute a state transition in INIT.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinits_change_state
(
  hdrinits_state_enum_type new_state
)
{
  HDRTRACE_STATE( HDRHAI_INITIALIZATION_STATE_PROTOCOL, 
    hdrinits_state, new_state);

  if (hdrinits_is_valid_trans[ hdrinits_state ][ new_state ])
  {
    /* Do exit processing from current state */
    (*hdrinits_exit[ hdrinits_state ])( new_state );

    /* Do entry processing for the new state */
    (*hdrinits_enter[ new_state ])( hdrinits_state );

    /* Change the state */
    hdrinits_state = new_state;

    HDRTRACE_STATE_ENTRY( HDRHAI_INITIALIZATION_STATE_PROTOCOL, new_state);
    hdrplog_log_state_info();
  }
  else
  {
    ERR_FATAL( "Bad transition %d to %d", hdrinits_state, new_state, 0 );
  }
} /* hdrinits_change_state() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRINITS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the INIT.
  This funciton is only to be used by other code in the INIT.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The current state of INIT

SIDE EFFECTS
  None

===========================================================================*/

hdrinits_state_enum_type hdrinits_get_state( void )
{
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "hdrinits_get_state %d", hdrinits_state);

  return ( hdrinits_state );
} /* hdrinits_get_state() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_INIT

DESCRIPTION
  This function initializes the initialzation state protoco's state module.

DEPENDENCIES
  This function must called before the initialization state protocol can be
  activated

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinits_init ( void )
{
  hdrinits_state = HDRINITS_INACTIVE_STATE;
}

