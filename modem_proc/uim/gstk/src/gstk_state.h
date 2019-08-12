#ifndef GSTK_STATE_H
#define GSTK_STATE_H


/*===========================================================================


                    G S T K    S T A T E    H E A D E R 


GENERAL DESCRIPTION
  This header file contains functions that are needed by the gstk_state.c.


FUNCTIONS
  gstk_state_start
    This function handles events/commands/signal for GSTK_START_S

  gstk_state_gsdi_sig_wait
    This function handles events/commands/signal for GSTK_GSDI_SIG_WAIT_S

  gstk_state_tp_wait
    This function handles events/commands/signals for 
    GSTK_TERM_PROFILE_WAIT_S

  gstk_state_tp_rsp_wait
    This function handles events/commands/signal for 
    GSTK_TERM_PROFILE_RSP_WAIT_S

  gstk_state_idle
    This function handles events/commands/signals for GSTK_IDLE_S

  gstk_state_no_sim
    This function handles events/commands/signals for GSTK_NO_SIM_S

  gstk_start_toolkit_session
    This function starts the toolkit session in the GSDI sign wait state or
    terminal profile wait state

  gstk_state_determine_action_after_gsdi_evt
    This function determine the state and the preferred toolkit slot based
    on the current toolkit_info mask
 
  gstk_state_non_idle_cleanup
    This function performs necessary clean up when state is not idle

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2012 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_state.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/12   nb      Fix compilation warning
12/20/11   nb      GSDI Cleanup
08/24/11   xz      Fix compiler warnings and LINT/KW errors
05/03/11   xz      Add cleanup when state is not idle
02/15/10   nb      Moved Dual Slot Changes
10/23/06   sk      Fixed lint errors.
06/09/06   sk      Lint fixes
03/05/04   tml     Allowed single slot to operate individually in Dual Slot 
                   solution
09/04/03   tml     Dual slot support
05/16/03   tml     Removed pwer down function definition
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"

/*===========================================================================
FUNCTION gstk_state_start

DESCRIPTION

  This function handles events/commands/signal for GSTK_START_S
  GSTK_START_S may receive any of the following events:
  - SIM_INSERTED from GSDI -> do nothing
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - GSDI_OK_TP_DL from GSDI -> transition to GSTK_TERM_PROFILE_WAIT_S
  - GSDI_NOT_OK_TP_DL from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile download from client -> transition to GSTK_GSDI_SIG_WAIT_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_state_start (
        gstk_cmd_type            * cmd_p );



/*===========================================================================
FUNCTION gstk_state_gsdi_sig_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_GSDI_SIG_WAIT_S
  GSTK_GSDI_SIG_WAIT_S may receive any of the following events:
  - SIM_INSERTED from GSDI -> do nothing
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - GSDI_OK_TP_DL from GSDI -> transition to GSTK_TERM_PROFILE_RSP_WAIT_S
  - GSDI_NOT_OK_TP_DL from GSDI -> transition to GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_state_gsdi_sig_wait (
  gstk_cmd_type            * cmd_p );


/*===========================================================================
FUNCTION gstk_state_tp_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_TERM_PROFILE_WAIT_S
  GSTK_TERM_PROFILE_WAIT_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile info from client -> transition to GSTK_TERM_PROFILE_RSP_WAIT_S
  - timer expiration from GSTK -> transition to GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_state_tp_wait (
        gstk_cmd_type            * cmd_p );


/*===========================================================================
FUNCTION gstk_state_tp_rsp_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_TERM_PROFILE_RSP_WAIT_S
  GSTK_TERM_PROFILE_RSP_WAIT_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile response from uim -> transition to GSTK_IDLE_S/GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_state_tp_rsp_wait (
  gstk_cmd_type            * cmd_p );


/*===========================================================================
FUNCTION gstk_state_idle

DESCRIPTION

  This function handles events/commands/signal for GSTK_IDLE_S
  GSTK_IDLE_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal response from client -> stay in GSTK_IDLE_S
  - proactive command from uim -> stay in GSTK_IDLE_S
  - envelope command from client -> stay in GSTK_IDLE_S
  - envelope response from uim -> stay in GSTK_IDLE_S
  - timer expiration from GSTK -> stay in GSTK_IDLE_S
  - end proactive session from uim -> stay in GSTK_IDLE_S
  - GSDI_REFRESH_SIM_INIT from GSDI -> GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_state_idle (
        gstk_cmd_type            * cmd_p );

/*===========================================================================
FUNCTION gstk_state_no_sim

DESCRIPTION

  This function handles events/commands/signal for GSTK_NO_SIM_S
  GSTK_NO_SIM_S may receive any of the following events:
  - sim valid from GSDI -> transition to GSTK_GSDI_SIG_WAIT_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_state_no_sim (
    gstk_cmd_type            * cmd_p );
                         
/*===========================================================================
FUNCTION gstk_state_non_idle_cleanup

DESCRIPTION
  This function performs necessary clean up when state is not idle

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_state_non_idle_cleanup(void);
#endif /* GSTK_STATE_H */

