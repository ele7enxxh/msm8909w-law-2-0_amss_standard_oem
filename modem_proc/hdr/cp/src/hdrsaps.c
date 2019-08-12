
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S I G N A L I N G    A D A P T A T I O N    P R O T O C O L

GENERAL DESCRIPTION
  This module contains code related to the state machine of SAP.  Its
  primary function is to encapsulate the state variable and require code
  external to this module to reference the state through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrsaps_change_state() - Changes the SAP state
  hdrsaps_get_state() - Returns the SAP state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsaps.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/12   cnx     Handle TX and RX messages during active handoff
06/12/12   cnx     Don't give HDRIND_SAP_RETURNING_TO_CLOSE until SAP moves to CLOSE.
04/17/12   cnx     Added HDRIND_SAP_RETURNING_TO_CLOSE.
11/28/11   cnx     Support connection abort.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/10/11   cnx     Fixed AltLinkCloseConf timer when leaving OPEN state.
04/11/11   cnx     Fixed compiler errors.
01/01/11   pba     Initial revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "err.h"

#ifdef FEATURE_LTE_TO_HDR_OH

#include "hdrplog.h"
#include "hdrind.h"
#include "hdrsap.h"
#include "hdrsaps.h"
#include "hdrsmps.h"
#include "hdrpac_api.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Any new SAP state must be added as state #3 */
#define NUM_SAP_STATES 3

static const boolean hdrsaps_is_valid_trans[NUM_SAP_STATES][NUM_SAP_STATES] =
{
  /* Close   Setup  Open*/
  { FALSE,  TRUE,  FALSE}, /* Close */
  { TRUE,   FALSE, TRUE }, /* Setup */
  { TRUE,   FALSE, FALSE}, /* Open */
};

static hdrsaps_state_enum_type hdrsaps_current_state = HDRSAPS_CLOSE;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in SAP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsaps_change_state
(
  hdrsaps_state_enum_type new_state
)
{
  boolean returning_to_close = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (new_state == hdrsaps_current_state)
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "Changing state to same state" );
    return;
  }
  
  HDRTRACE_STATE( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, 
    hdrsaps_current_state, new_state);

  if (hdrsaps_is_valid_trans[hdrsaps_current_state][new_state])
  {

    /* Leaving the current state */
    switch (hdrsaps_current_state)
    {
      case HDRSAPS_CLOSE:
        break;

      case HDRSAPS_SETUP:
        hdrsap_stop_alt_link_open_conf_timer();
        break;

      case HDRSAPS_OPEN:
        hdrsap_stop_alt_link_close_conf_timer();

        /* Clean up the RLP if it had been initialized. */
        hdrpac_mdm_sn_cleanup();
        break;

      default:
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                        "State Transition from unknown state %d", 
                        hdrsaps_current_state );
        break;
    }
  
    /* Entering the new state */
    switch (new_state)
    {
      case HDRSAPS_CLOSE:
        returning_to_close = TRUE;
        hdrsap_reset_abort_connection();
        break;

      case HDRSAPS_SETUP:
        break;

      case HDRSAPS_OPEN:
        break;

      default:
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                        "State Transition to unknown state %d",
                        new_state );
        break;
    }
    hdrsaps_current_state = new_state;
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_ERROR, "Invalid State Transition, %d to %d",
                    hdrsaps_current_state, new_state );
  }

  if (returning_to_close)
  {
    /* This indication is to mimic HDRIND_ALMP_RETURNING_TO_IDLE */
    hdrind_give_ind( HDRIND_SAP_RETURNING_TO_CLOSE, NULL );  
  }
 
  HDRTRACE_STATE_ENTRY( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL, new_state );
  //hdrplog_log_state_info();
} /* hdrsaps_change_state */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the SAP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state

SIDE EFFECTS
  None

===========================================================================*/
hdrsaps_state_enum_type hdrsaps_get_state ( void )
{
  return hdrsaps_current_state;
}
#endif /* FEATURE_LTE_TO_HDR_OH */

