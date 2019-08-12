
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E S S I O N   M A N A G E M E N T   P R O T O C O L

GENERAL DESCRIPTION
  This module contains code related to the state machine of SMP.  Its
  primary function is to encapsulate the state variable and require code
  external to this module to reference the state through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrsmps_change_state() - Changes the SMP state
  hdrsmps_get_state() - Returns the SMP state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000,2001,2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2003,2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrsmps.c_v   1.13   14 Mar 2003 11:18:02   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsmps.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/15/14   vko     Fixed race condition while session close
05/19/10   pba     Clear HMP pending message buffer when session is closing.
10/03/07   wsh     MRLPC GAUP pending queue not getting freed
03/27/07   wsh     Added support for new prior session handling
09/26/06   sju     Added removing all RASM messages after entering session 
                   CLOSING state.
10/19/04   mpa     Added new session CLOSING state.
03/12/03   mpa     Pass in session close reason as payload to SMP_CLOSE ind
08/18/01   sh      Added protocol logging support
11/21/00   nmn     Initial revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrmci.h"
#include "hdrsmp.h"
#include "hdrsmpi.h"
#include "hdrsmps.h"
#include "hdrsmpka.h"
#include "hdramp.h"
#include "hdrscp.h"
#include "hdrind.h"
#include "hdralmp.h"
#include "hdrplog.h"
#include "hdrmrlpc.h"
#include "hdrhmp.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Any new SMP state must be added as state #4 */
#define NUM_SMP_STATES 4

static boolean hdrsmps_is_valid_trans[NUM_SMP_STATES][NUM_SMP_STATES] =
{
/*                      TO INACTIVE, TO AMP SETUP, TO OPEN, TO CLOSING */
/* FROM INACTIVE  */  { FALSE      , TRUE        , TRUE   , FALSE },
/* FROM AMP SETUP */  { TRUE       , FALSE       , TRUE   , TRUE  },
/* FROM OPEN      */  { TRUE       , FALSE       , FALSE  , TRUE  },
/* FROM CLOSING   */  { TRUE       , FALSE       , FALSE  , FALSE }
};

LOCAL hdrsmps_state_enum_type hdrsmps_current_state = HDRSMPS_INACTIVE;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in SMP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsmps_change_state
(
  hdrsmps_state_enum_type new_state
)
{
  if (new_state == hdrsmps_current_state)
  {
    ERR ("Changing state to same state", 0, 0, 0);
    return;
  }
  
  HDRTRACE_STATE( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, 
    hdrsmps_current_state, new_state);

  if (hdrsmps_is_valid_trans[hdrsmps_current_state][new_state])
  {

    /* Leaving the current state */
    switch (hdrsmps_current_state)
    {
      case HDRSMPS_AMP_SETUP:
      case HDRSMPS_INACTIVE:
      case HDRSMPS_OPEN:
      case HDRSMPS_CLOSING:
      /* no action upon leaving any state */
        break;

      default:
        ERR("State Transition from unknown state %d", 
             hdrsmps_current_state, 0, 0);
        break;
    }
  
    /* Entering the new state */
    switch (new_state)
    {
      case HDRSMPS_INACTIVE:
	  	hdrsmp_set_session_closing(TRUE);
        hdralmp_reset(HDRHAI_SESSION_MANAGEMENT_PROTOCOL);
        /* ALMP reset may cause the keep alive timer to be reset before
           state has officially changed, so clr timer is moved here */
        hdrsmpka_clr_timer();
        (void)rex_clr_sigs(HDRSMP_TASK_PTR, HDRMC_SMP_KA_TIMER_SIG);
        hdrsmp_send_session_close_ind();
   	  	hdrsmp_set_session_closing(FALSE);
        break;

      case HDRSMPS_AMP_SETUP:
        hdramp_activate(HDRHAI_SESSION_MANAGEMENT_PROTOCOL);
        break;

      case HDRSMPS_OPEN:
        if (hdrsmps_current_state != HDRSMPS_INACTIVE)
        {
          hdrscp_activate(HDRHAI_SESSION_MANAGEMENT_PROTOCOL);
          hdrind_give_ind(HDRIND_SMP_BOOT_COMPLETED, NULL);
          hdrind_give_ind(HDRIND_SMP_SESSION_OPENED, NULL);
        }
        break;

      case HDRSMPS_CLOSING:
        hdrhmp_clear_rasm_queue();
        hdrhmp_clear_pending_msg_queue(E_CANCELED);
        hdrmrlpc_clear_pending_gaup_queue();

        break;

      default:
        ERR("State Transition to unknown state %d",new_state,0,0);
        break;
    }
    hdrsmps_current_state = new_state;
  }
  else
  {
    ERR("Invalid State Transition, %d to %d",hdrsmps_current_state,
         new_state, 0);
  }

  HDRTRACE_STATE_ENTRY( HDRHAI_SESSION_MANAGEMENT_PROTOCOL, new_state);
  hdrplog_log_state_info();
} /* hdrsmps_change_state */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the SMP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state

SIDE EFFECTS
  None

===========================================================================*/
hdrsmps_state_enum_type hdrsmps_get_state ( void )
{
  return hdrsmps_current_state;
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPS_SILENT_RESTORE_PRIOR_SESSION

DESCRIPTION
  Called to set SMP to OPEN state without giving any indications.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state

SIDE EFFECTS
  None

===========================================================================*/
void hdrsmps_silent_restore_prior_session ( void )
{
  /* setting SMP to INACTIVE first would produce a state transition 
     sequence similar to phone power-up. In this case SMP will not
     activate SCP and will not cause SCP to start negotiation. */
  hdrsmps_current_state = HDRSMPS_INACTIVE;
  hdrsmps_change_state(HDRSMPS_OPEN);
} /* hdrsmps_silent_restore_prior_session */
