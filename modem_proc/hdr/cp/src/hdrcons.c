
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               C O N N E C T E D   S T A T E   P R O T O C O L

GENERAL DESCRIPTION
  This module contains code related to the state machine of CON.  Its
  primary function is to encapsulate the state variable and require code
  external to this module to reference the state through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrcons_change_state() - Changes the CON state
  hdrcons_get_state() - Returns the CON state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrcons.c_v   1.9   19 Aug 2001 11:28:12   shuang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrcons.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/01/07   sju     Added support for QOS HHO
01/11/07   sju     Added support to check if CC and TCA are in the same 
                   connection layer packet.
10/13/03   vas     Added comments
08/18/01   sh      Added protocol logging support
09/07/00   dna     No need to activate OHVD or to close RUP
07/28/00   nmn     Created module

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrmci.h"
#include "hdrcon.h"
#include "hdrconi.h"
#include "hdrcons.h"
#include "hdrind.h"
#include "hdrplog.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Any new CON state must be added as state #2 */
#define HDRCONS_NUM_STATES 2

LOCAL hdrcons_state_enum_type hdrcons_current_state = HDRCONS_INACTIVE;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCONS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in CON.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcons_change_state
(
  hdrcons_state_enum_type new_state
)
{
  hdrind_ind_data_union_type ind_data;
  ind_data.hho_is_in_progress = hdrcon_hho_is_in_progress() ;

  if (new_state == hdrcons_current_state)
  {
    ERR ("Changing state to same state", 0, 0, 0);
    return;
  }
  
  HDRTRACE_STATE( HDRHAI_CONNECTED_STATE_PROTOCOL, 
    hdrcons_current_state, new_state);

  /* Leaving the current state */
  switch (hdrcons_current_state)
  {
    case HDRCONS_INACTIVE:
      break;

    case HDRCONS_OPEN:
      /* Reset hho_is_in_progress flag
       * this flag is needed only when connection is open
       */
      hdrcon_set_hho_is_in_progress_flag( FALSE ); 

      /* Note that the Connection Closed indication will result in 
       * RLP getting cleaned up in the HDR MC task context (from HDRPAC)
       * as part of the indication callback 
       */

      hdrind_give_ind ( HDRIND_CONNECTED_CONNECTION_CLOSED , &ind_data );
      break;
  }
  hdrcons_current_state = new_state;
  
  /* Entering the new state */
  switch (new_state)
  {
    case HDRCONS_INACTIVE:
      break;

    case HDRCONS_OPEN:
      hdrcmac_activate(HDRHAI_CONNECTED_STATE_PROTOCOL);
      break;

    default:
      break;
  }
  HDRTRACE_STATE_ENTRY( HDRHAI_CONNECTED_STATE_PROTOCOL, new_state);
  hdrplog_log_state_info();
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCONS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the CON protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state

SIDE EFFECTS
  None

===========================================================================*/
hdrcons_state_enum_type hdrcons_get_state ( void )
{
  return hdrcons_current_state;
}
