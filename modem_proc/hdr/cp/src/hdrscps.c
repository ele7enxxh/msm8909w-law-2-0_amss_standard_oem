
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         S E S S I O N   C O N F I G U R A T I O N   P R O T O C O L

GENERAL DESCRIPTION
  This module contains code related to the state machine of SCP.  Its
  primary function is to encapsulate the state variable and require code
  external to this module to reference the state through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrscps_change_state() - Changes the SCP state
  hdrscps_get_state() - Returns the SCP state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000,2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrscps.c_v   1.13   13 May 2002 11:50:24   nweinsto  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscps.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/07   pba     Obsoleted substates in SCP.
03/27/07   wsh     Added support for new prior session handling
12/13/05   pba     Changes to support multiple personality negotiation
11/14/05   dna     Phase 0 support for multiple personalities
05/13/02   nmw     Limit attempts to retry configuration before failing session
08/18/01   sh      Added protocol logging support
07/10/01   nmn     changed AN INIT State timer name
01/02/01   nmn     Initial Revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrmci.h"
#include "hdrscp.h"
#include "hdrscpi.h"
#include "hdrscps.h"
#include "hdrplog.h"
#include "hdrscmdb.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Any new SCP state must be added as state #3 */
#define HDRSCPS_NUM_STATES 5

static boolean hdrscps_is_valid_trans[HDRSCPS_NUM_STATES][HDRSCPS_NUM_STATES] =
{
/*                    TO INACTIVE, TO OPEN, TO AT_INIT, TO AN_INIT, TO PS_GAUP */
/* FROM INACTIVE */  { FALSE     , TRUE   , FALSE     , FALSE     , FALSE},
/* FROM OPEN     */  { TRUE      , FALSE  , TRUE      , FALSE     , TRUE },
/* FROM AT_INIT  */  { TRUE      , FALSE  , FALSE     , TRUE      , FALSE },
/* FROM AN_INIT  */  { TRUE      , TRUE   , TRUE      , FALSE     , FALSE },
/* FROM PS_GAUP  */  { TRUE      , TRUE   , FALSE     , FALSE     , FALSE }
};

LOCAL hdrscps_state_enum_type hdrscps_current_state = HDRSCPS_INACTIVE;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in SCP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrscps_change_state
(
  hdrscps_state_enum_type new_state
)
{
  if (new_state == hdrscps_current_state)
  {
    ERR ("Changing state to same state", 0, 0, 0);
    return;
  }
  
  HDRTRACE_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, 
    hdrscps_current_state, new_state);

  if (hdrscps_is_valid_trans[hdrscps_current_state][new_state])
  {

    /* Leaving the current state */
    switch (hdrscps_current_state)
    {
      case HDRSCPS_AN_INIT:
        (void) rex_clr_timer( &hdrscp_an_init_state_timer );
        (void) rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_AN_INIT_STATE_TIMER_SIG);
        break;

      case HDRSCPS_INACTIVE:
      case HDRSCPS_OPEN:
      /* no action upon leaving these states */
        break;

      case HDRSCPS_AT_INIT:
        hdrscp_reset_con_fail_retry_count();
        break;

      case HDRSCPS_PS_GAUP:
        break;

      default:
        ERR("State Transition from unknown state %d", 
             hdrscps_current_state, 0, 0);
        return;
    }
  
    /* Entering the new state */
    switch (new_state)
    {
      case HDRSCPS_INACTIVE:
        /* no action upon inactive state */
        break;

      case HDRSCPS_AT_INIT:
        /* Whenever SCP enters AT_INIT state set the inconfig instance of each
           protocol's configuration data to the inuse values and start the
           negotiation */    
        hdrscmdb_set_inconfig_to_inuse();

        if (hdrscps_current_state == HDRSCPS_OPEN)
        {
          hdrscp_open_to_at_init_state();
        }
        else if (hdrscps_current_state == HDRSCPS_AN_INIT)
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH,
                        "Starting negotiation of another personality");
        }
        break;

      case HDRSCPS_OPEN:
        hdrscp_enter_open_state();
        if (hdrscps_current_state == HDRSCPS_AN_INIT)
        {
          hdrscp_an_init_to_open_state();
        }
        break;

      case HDRSCPS_AN_INIT:
        /* start AN Init State Timer */
        (void) rex_set_timer( &hdrscp_an_init_state_timer, 
                              HDRSCP_AN_INIT_STATE_TIME );
        break;

      case HDRSCPS_PS_GAUP:
        break;

      default:
        ERR("State Transition to unknown state %d",new_state,0,0);
        return;
    }
    hdrscps_current_state = new_state;
  }
  else
  {
    ERR("Invalid State Transition, %d to %d",hdrscps_current_state,
         new_state, 0);
  }

  HDRTRACE_STATE_ENTRY( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, new_state);
  hdrplog_log_state_info();
} /* hdrscps_change_state */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the SCP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state

SIDE EFFECTS
  None

===========================================================================*/
hdrscps_state_enum_type hdrscps_get_state ( void )
{
  return hdrscps_current_state;
}
