/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                  O V H E R H E A D  P R O T O C O L
                        S T A T E  M O D U L E

GENERAL DESCRIPTION
  This is the state module for Overhead Protocol

EXTERNALIZED FUNCTIONS 
  hdrovhds_powerup_init - Initializes the state module
  hdrovhds_change_state - Change overhead state
  hdrovhds_get_state    - Get current overhead state.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrovhds_powerup_init has to be called before this moduel is used.

Copyright (c) 2000 - 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrovhds.c_v   1.11   10 Dec 2002 18:08:38   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrovhds.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/04/13   cnx     Don't reset tunnel info if AT comes back to the same cell
                   after OOS. 
08/06/12   cnx     Make OVHD not updated in processing CGI command.
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
10/12/11   cnx     Restored changes made on 10/03/11.
10/10/11   cnx     Reset tunnel info when leaving tunnel active state.
10/03/11   cnx     Don't start timers for QC if waiting for SIB8 update.
09/24/11   cnx     Don't send HDRIND_OVHD_PREREG_INFO_UPDATED if prereg 
                   flag is not updated yet.
07/27/11   cnx     Merged Optimized Handoff changes.
02/10/09   sju     Added support for intra scc sleep.
12/10/08   smd     Replaced assert.h with amssassert.h
07/06/06   mpa     Suspend QC/SP supervision timers if IDLE is suspending. 
06/26/03   mpa     (for dna) Allow transition from WaitForLink to AccHoInProg
12/10/02   dna     Allow transition from wait for link to sleep
01/12/01   vas     Changed state transition table to allow transition from
                   Wait for Link to AHO in Prog state and from AHO in Prog/
                   Freq Change in Prog to Process All Msgs.
08/18/01   sh      Added protocol logging support
05/09/01   vas     Re-naming for consistency
01/17/01   vas     Changed state transition table to allow transition from 
                   Sleep to WaitForLink state
09/07/00   vas     Created module.

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
#include "amssassert.h"
#include "hdrovhd.h"
#include "hdrovhdi.h"
#include "hdrovhds.h"
#include "hdrlmac.h"
#include "hdrmci.h"
#include "hdrplog.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrmc_v.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */

/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#define HDROVHD_NUM_STATES 7

LOCAL const boolean 
      hdrovhds_is_valid_trans[HDROVHD_NUM_STATES][HDROVHD_NUM_STATES] =
{
  /* INACT  PAM    SLEEP  FCIP   AHIP   WFL    TA       <- To */
  {  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE   }, /* From INACTIVE */
  {  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE  }, /* From PROCESS_ALL_MSG */
  {  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE  }, /* From SLEEP */
  {  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE  }, /* From FREQ_CHANGE_IN_PROG*/
  {  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE  }, /* From ACC_HO_IN_PROG */
  {  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE  }, /* From WAIT_FOR_LINK */
  {  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE  }  /* From TUNNEL_ACTIVE */
};

LOCAL struct
{
  hdrovhds_state_enum_type state;   /* Protocol State variable */
} hdrovhds;

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

FUNCTION HDROVHDS_POWERUP_INIT

DESCRIPTION
  Initializes the OVHD state module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrovhds_powerup_init( void )
{
  hdrovhds.state = HDROVHDS_INACTIVE;
} /* hdrovhds_powerup_init() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_enter_inactive_state

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
LOCAL void hdrovhds_enter_inactive_state ( void )
{
  /* Stop all timers and clear any saved times. */
  hdrovhd_clear_timers();

  /* Set qc_is_received to FALSE after entering inactive state, 
   * it will be set to TRUE after receiving QC */
  hdrovhd_set_qc_is_received(FALSE);
} /* hdrovhds_enter_inactive_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_enter_process_all_msgs_state

DESCRIPTION
  This function performs the processing required upon entry to the
  Process All Messages state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
LOCAL void hdrovhds_enter_process_all_msgs_state( void )
{
  hdrovhd_restart_timers();
} /* hdrovhds_enter_process_all_msgs_state() */

/* <EJECT> */

/*===========================================================================
FUNCTION hdrovhds_enter_sleep_state

DESCRIPTION
  This function performs the processing required upon entry to the sleep
  state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
LOCAL void hdrovhds_enter_sleep_state ( void )
{
  /*-------------------------------------------------------------------------
  Stop QuickConfig, SectorParameters & AccessParameters Supervision timers.
  -------------------------------------------------------------------------*/
  hdrovhd_stop_timers();

  /* Set qc_is_received to FALSE after entering sleep state, 
   * it will be set to TRUE after receiving QC */
  hdrovhd_set_qc_is_received(FALSE);
} /* hdrovhds_enter_sleep_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_enter_freq_change_in_prog_state

DESCRIPTION
  This function performs the processing required upon entry to the 
  FREQ_CHNAGE_IN_PROG state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
LOCAL void hdrovhds_enter_freq_change_in_prog_state ( void )
{
  hdrovhd_restart_timers();
} /* hdrovhds_enter_freq_change_in_prog_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_enter_acc_ho_in_prog_state

DESCRIPTION
  This function performs the processing required upon entry to the 
  ACC_HO_IN_PROG state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
LOCAL void hdrovhds_enter_acc_ho_in_prog_state ( void )
{
  /*---------------------------------------------------------------------------
  Restart timers & Instruct LMAC to perform Access HandOff
  ---------------------------------------------------------------------------*/
  hdrovhd_restart_timers();
  hdrlmac_perform_aho(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL);

} /* hdrovhds_enter_acc_ho_in_prog_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_enter_wait_for_link_state

DESCRIPTION
  This function performs the processing required upon entry to the 
  wait for link state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
LOCAL void hdrovhds_enter_wait_for_link_state ( void )
{
  hdrovhd_restart_timers();
} /* hdrovhds_enter_wait_for_link_state() */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION hdrovhds_enter_tunnel_active_state

DESCRIPTION
  This function performs the processing required upon entry to the 
  tunnel active state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
LOCAL void hdrovhds_enter_tunnel_active_state ( void )
{
  boolean prereg_is_allowed;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  prereg_is_allowed = hdrovhd_prereg_is_allowed( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL );

  /* It's possible that we haven't received any SIB8 or PreReg info yet
   * This case is same as PreReg is not allowed. 
   * prereg_is_allowed is reset to 0 when we exit tunnel active state, so we
   * shoud be fine. 
   */
  if ( hdrovhd_is_sib8_updated() )
  {
    hdrind_give_ind( HDRIND_OVHD_PREREG_INFO_UPDATED,
                     (void *)&prereg_is_allowed );
  }
  else
  {
    /* Enter tunnel active state but prereg is not available yet */
  }

} /* hdrovhds_enter_tunnel_active_state() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================
FUNCTION hdrovhds_exit_inactive_state

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
LOCAL void hdrovhds_exit_inactive_state( void )
{
} /* hdrovhds_exit_inactive_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_exit_process_all_msgs_state

DESCRIPTION
  This function performs the processing required upon exit from the 
  PROCESS_ALL_MSG state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrovhds_exit_process_all_msgs_state ( void )
{
} /* hdrovhds_exit_process_all_msgs_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_exit_sleep_state

DESCRIPTION
  This function performs the processing required upon exit from the SLEEP
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
LOCAL void hdrovhds_exit_sleep_state ( void )
{
} /* hdrovhds_exit_sleep_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_exit_freq_change_in_prog_state

DESCRIPTION
  This function performs the processing required upon exit from the 
  FREQ_CHANGE_IN_PROG state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrovhds_exit_freq_change_in_prog_state( void )
{
} /* hdrovhds_exit_freq_change_in_prog_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION hdrovhds_exit_acc_ho_in_prog_state

DESCRIPTION
  This function performs the processing required upon exit from the 
  ACC_HO_IN_PROG state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrovhds_exit_acc_ho_in_prog_state( void )
{
} /* hdrovhds_exit_acc_ho_in_prog_state() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDROVHDS_EXIT_WAIT_FOR_LINK_STATE

DESCRIPTION
  This function performs the processing required upon exit from the 
  WAIT_FOR_LINK state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrovhds_exit_wait_for_link_state( void )
{
} /* hdrovhds_exit_wait_for_link_state() */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION hdrovhds_exit_tunnel_active_state

DESCRIPTION
  This function performs the processing required upon exit from the 
  tunnel active state.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
LOCAL void hdrovhds_exit_tunnel_active_state ( void )
{
  /* If we receive get CGI command in tunnel, we wil have to move OVHD to
   * PROCESS_ALL_MSG state first in order to process SP msg. We want to
   * return to tunnel mode after getting CGI without UATI update or session 
   * change. So don't reset all the tunnel info. 
   * In LTE OOS, we hope do nothing if UE moves back to same LTE cell after
   * OOS. So we don't reset tunnel info until we know whether we return to 
   * the same cell after OOS. */
  if ( !hdrmc_lte_to_hdr_get_cgi_is_in_progress() &&
       (hdrsap_get_current_tunnel_mode_value(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL) != 
          HDRHAI_TUNNEL_MODE_NO_SRV) )
  {
    hdrovhd_reset_tunnel_info();
  }

  hdrovhd_reset_info_state();

} /* hdrovhds_exit_tunnel_active_state() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

FUNCTION HDROVHDS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in OVHD.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrovhds_change_state
(  
  hdrovhds_state_enum_type new_state
)
{

  HDRTRACE_STATE( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 
    hdrovhds.state, new_state);

  /*-------------------------------------------------------------------------
   Determine if the state transition is valid.
  -------------------------------------------------------------------------*/
  if ( hdrovhds_is_valid_trans[ hdrovhds.state ][ new_state ] == FALSE )
  {
    ERR("Invalid state transition!",0,0,0);
  }
  else
  {
    /*-----------------------------------------------------------------------
     Exit current state.
    -----------------------------------------------------------------------*/
    switch ( hdrovhds.state )
    {
      case HDROVHDS_INACTIVE:
        hdrovhds_exit_inactive_state();
        break;

      case HDROVHDS_PROCESS_ALL_MSGS:
        hdrovhds_exit_process_all_msgs_state();
        break;

      case HDROVHDS_SLEEP:
        hdrovhds_exit_sleep_state();
        break;

      case HDROVHDS_FREQ_CHANGE_IN_PROG:
        hdrovhds_exit_freq_change_in_prog_state();
        break;

      case HDROVHDS_ACC_HO_IN_PROG:
        hdrovhds_exit_acc_ho_in_prog_state();
        break;

      case HDROVHDS_WAIT_FOR_LINK:
        hdrovhds_exit_wait_for_link_state();
        break;

#ifdef FEATURE_LTE_TO_HDR_OH
      case HDROVHDS_TUNNEL_ACTIVE:
        hdrovhds_exit_tunnel_active_state();
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
      case HDROVHDS_INACTIVE:
        hdrovhds_enter_inactive_state();
        break;

      case HDROVHDS_PROCESS_ALL_MSGS:
        hdrovhds_enter_process_all_msgs_state();
        break;

      case HDROVHDS_SLEEP:
        hdrovhds_enter_sleep_state();
        break;

      case HDROVHDS_FREQ_CHANGE_IN_PROG:
        hdrovhds_enter_freq_change_in_prog_state();
        break;

      case HDROVHDS_ACC_HO_IN_PROG:
        hdrovhds_enter_acc_ho_in_prog_state();
        break;

      case HDROVHDS_WAIT_FOR_LINK:
        hdrovhds_enter_wait_for_link_state();
        break;

#ifdef FEATURE_LTE_TO_HDR_OH
      case HDROVHDS_TUNNEL_ACTIVE:
        hdrovhds_enter_tunnel_active_state();
        break;
#endif /* FEATURE_LTE_TO_HDR_OH */

      default:
        ERR_FATAL("Bad New State!",0,0,0);
    }

    /*-----------------------------------------------------------------------
     Update state variable.
    -----------------------------------------------------------------------*/
    hdrovhds.state = new_state;

    HDRTRACE_STATE_ENTRY( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, new_state);
    hdrplog_log_state_info();
  } /* if valid state transition */
} /* hdrovhds_change_state() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDROVHDS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the OVHD protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
hdrovhds_state_enum_type hdrovhds_get_state( void )
{
  return hdrovhds.state;
} /* hdrovhds_get_state() */
