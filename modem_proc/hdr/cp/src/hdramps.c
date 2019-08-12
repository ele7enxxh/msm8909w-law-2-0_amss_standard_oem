
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            A D D R E S S   M A N A G E M E N T   P R O T O C O L

GENERAL DESCRIPTION
  This module contains code related to the state machine of AMP.  Its
  primary function is to encapsulate the state variable and require code
  external to this module to reference the state through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdramps_change_state() - Changes the AMP state
  hdramps_get_state() - Returns the AMP state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdramps.c_v   1.10   06 Mar 2003 09:14:04   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdramps.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/11   cnx     Don't send UATIrequest msg if PreRegistration is not allowed.
03/27/07   wsh     Added support for new prior session handling
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
07/07/05   dna     Added support for Enhanced Idle State protocol
03/05/03   mpa     Replaced clr_timer with call to stop_uati_assignment_timer
03/27/02   mpa     Removed clr_timer call to dual_addr_timer
01/21/02   sh      Regenerated session seed when session is closed
08/18/01   sh      Added protocol logging support
05/08/01   nmn     Beautification
12/11/00   nmn     Initial revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrmci.h"
#include "hdramp.h"
#include "hdrampi.h"
#include "hdramps.h"
#include "hdrind.h"
#include "hdrplog.h"
#include "hdrsmpka.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Any new AMP state must be added as state #3 */
#define HDRAMP_NUM_STATES 3

static boolean hdramps_is_valid_trans[HDRAMP_NUM_STATES][HDRAMP_NUM_STATES] =
{
/*                     TO INACTIVE, TO SETUP, TO OPEN */
/* FROM INACTIVE */  { FALSE      , TRUE    , TRUE     },
/* FROM SETUP    */  { TRUE       , FALSE   , TRUE     },
/* FROM OPEN     */  { TRUE       , FALSE   , FALSE    }
};

LOCAL hdramps_state_enum_type hdramps_current_state = HDRAMP_INACTIVE;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in AMP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdramps_change_state
(
  hdramps_state_enum_type new_state
)
{
  if (new_state == hdramps_current_state)
  {
    ERR ("Changing state to same state", 0, 0, 0);
    return;
  }
  
  HDRTRACE_STATE( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, 
    hdramps_current_state, new_state);

  if (hdramps_is_valid_trans[hdramps_current_state][new_state])
  {

    /* Leaving the current state */
    switch (hdramps_current_state)
    {
      case HDRAMP_SETUP:
      case HDRAMP_INACTIVE:
      case HDRAMP_OPEN:
      /* no action upon leaving any state */
        break;

      default:
        ERR("State Transition from unknown state %d", 
             hdramps_current_state, 0, 0);
        break;
    }
  
    /* Entering the new state */
    switch (new_state)
    {
      case HDRAMP_INACTIVE:
        /* clear timer reseting data */
        hdramp_stop_uati_assignment_timer();

        hdramp_reset_data();
        hdrind_give_ind(HDRIND_AMP_UATI_RELEASED, NULL);
        if (hdramps_current_state == HDRAMP_SETUP)
        {
          hdrplog_end_session_attempt(FALSE);
        }
        break;

      case HDRAMP_SETUP:
        hdramp_enter_setup();
        /* we do not write info to NV even though the enter_setup changes
           some values.  This is because the session is not yet open,
           so the values would not be read anyway.  */

#ifdef FEATURE_LTE_TO_HDR_OH
        /* Don't send UATIrequest msg if PreRegistration is not allowed. */
        if ( !( hdrsap_tunnel_mode_is_enabled( HDRHAI_SESSION_MANAGEMENT_PROTOCOL ) &&
                hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
                  == HDRSCP_IRAT_SAP &&
                !hdrovhd_prereg_is_allowed( HDRHAI_SESSION_MANAGEMENT_PROTOCOL ) ) )
#endif /* FEATURE_LTE_TO_HDR_OH */
        {
          hdramp_send_uati_req_msg();
        }
        break;

      case HDRAMP_OPEN:
        if (hdramps_current_state == HDRAMP_SETUP)
        {
          hdrind_give_ind(HDRIND_AMP_OPENED, NULL);
          hdrplog_end_session_attempt(TRUE);
        }
        break;

      default:
        ERR("State Transition to unknown state %d",new_state,0,0);
        break;
    }
    hdramps_current_state = new_state;
  }
  else
  {
    ERR("Invalid State Transition, %d to %d",hdramps_current_state,
         new_state, 0);
  }

  HDRTRACE_STATE_ENTRY( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, new_state);
} /* hdramps_change_state */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the AMP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state

SIDE EFFECTS
  None

===========================================================================*/
hdramps_state_enum_type hdramps_get_state ( void )
{
  return hdramps_current_state;
}

/*===========================================================================

FUNCTION HDRAMPS_SILENT_RESTORE_PRIOR_SESSION

DESCRIPTION
  This function silently set AMP to OPEN state. It does not copy
  the prior session info over current. hdramp_previous_session_opened()
  does that.

DEPENDENCIES
  hdrsmps_silent_restore_prior_session() must be called before this.

PARAMETERS
  None
  
RETURN VALUE
  TRUE if Success
  
SIDE EFFECTS
  None

===========================================================================*/
void hdramps_silent_restore_prior_session( void )
{
  /* setting current state to inactive first will in effect
     emulate a phone power-up, and ensures no AMP_OPENED_IND
     is given. */
  hdramps_current_state = HDRAMP_INACTIVE;
  hdramps_change_state(HDRAMP_OPEN);
}
