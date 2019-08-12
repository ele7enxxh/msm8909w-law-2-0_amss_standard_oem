#ifndef RRCSCMGR_H
#define RRCSCMGR_H
/*===========================================================================
             R R C     S T A T E    C H A N G E    M A N A G E R  

DESCRIPTION

  This is the external header file for the State Change Manager module.
  This module is to be used by other RRC modules only and not by
  tasks other than RRC.
  
Copyright (c) 2001,2003-2005,2007-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcscmgr.h_v   1.1   23 Apr 2001 18:57:32   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcscmgr.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
05/08/09   ss      Updated Copyright Information
08/02/05   da      Added prototype for rrc_get_prev_state
06/06/05   da      Added prototypes for funcs rrc_register_home_zone_ind_cb(),
                   rrc_deregister_home_zone_ind_cb(), and rrc_trigger_home_zone_ind() 
12/02/04   da      Added prototypes for rrcscmrg_register_rrc_state_change_ind_cb,
                   rrcscmgr_deregister_rrc_state_change_ind_cb
04/23/01   ram     Moved rrc_init_state() and rrc_get_state() function prototypes
                   from rrctask.h. Externalized function rrc_initiate_state_change().
01/08/01   ram     Created file.


===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "stringl.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */






/* =======================================================================
**                          Macro Definitions
** ======================================================================= */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION RRC_REGISTER_HOME_ZONE__IND_CB

DESCRIPTION

This function enables upper layer to register a callback function if notification
of of current rrc state and location information is required.  Once the function is 
registered, it will be called whenever there is a state change or cell id change.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/

extern void rrc_register_home_zone_ind_cb
(
  RRC_HOME_ZONE_IND_CB_TYPE *cb_ptr
);
/*===========================================================================

FUNCTION RRC_DEREGISTER_HOME_ZONE_IND_CB

DESCRIPTION

This function enables upper layer to deregister the callback function that was
registered for Home zone indication.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/

extern void rrc_deregister_home_zone_ind_cb 
(
  void
);

/*===========================================================================

FUNCTION RRC_TRIGGER_HOME_ZONE_IND

DESCRIPTION

This function will call the register HZI cb func to notify the rrc state and 
required location information

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

extern void rrc_trigger_home_zone_ind
(
  void
);


/*===========================================================================

FUNCTION RRC_INIT_STATE

DESCRIPTION

  This function initializes the default state of RRC at powerup.
  This function should be called only once, at powerup.
  
DEPENDENCIES
  None

RETURN VALUE
  Current state of RRC.

SIDE EFFECTS
  None

===========================================================================*/
void  rrc_init_state( void );




/*===========================================================================

FUNCTION RRC_INITIATE_STATE_CHANGE

DESCRIPTION

  This function initiates a state change in RRC by posting a 
  RRC_NEW_STATE_IND command with the given RRC state. When this
  command is processed, the RRC state will be changed to the
  new state and state change notifications will be sent if needed.
  
  The reason the new state is not set righ away is that we
  want to avoid any potential race conditions with commands
  that are already queued. When commands that are already
  queued are processed before the RRC_NEW_STATE_IND command they
  may be calling rrc_get_state to get the current state which
  may lead to some race conditions.
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void  rrc_initiate_state_change
( 
  rrc_state_e_type  state  /* New state */ 
);



/*===========================================================================

FUNCTION RRC_GET_STATE

DESCRIPTION
  This function returns the current state of RRC.
  
DEPENDENCIES
  None

RETURN VALUE
  Current state of RRC.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
extern  rrc_state_e_type  rrc_get_state( void );


/*===========================================================================

FUNCTION RRC_GET_PREV_STATE

DESCRIPTION
  This function returns the previous state of RRC.
  
DEPENDENCIES
  None

RETURN VALUE
  Current state of RRC.

SIDE EFFECTS
  None

===========================================================================*/
extern  rrc_state_e_type  rrc_get_prev_state( void );

/*===========================================================================

FUNCTION RRCSCMGR_REGISTER_FOR_SCN

DESCRIPTION

  This function registers State change notification requests from 
  other procedures. When a procedure needs to be notified when a 
  particular RRC state transition occurs, that procedure must register
  using this function. The from_state is the state from which the
  transition occurs and the to_state is the state to which the transition
  occurs. Wildcard may be used for both the from_state and to_state.
  The wildcard to be used is RRC_STATE_WILDCARD. When a wildcard is
  used for both from and to states, transitions from any state to
  any other state will be notified.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcscmgr_register_for_scn
(
  rrc_proc_e_type  procedure,   /* Procedure that's doing this registration */
  rrc_state_e_type from_state,  /* State from which the transition occurs. This
                                can be "RRC_STATE_WILDCARD" to indicate that the
                                from state can be any RRC state */
  rrc_state_e_type to_state     /* State to which the transition occurs. This
                                can be "RRC_STATE_WILDCARD" to indicate that the
                                to state can be any RRC state */
);




/*===========================================================================

FUNCTION RRCSCMGR_EVENT_HANDLER

DESCRIPTION

  This is the event handler for the state change manager module.
  Only one event is expected here and that is the internal command
  RRC_NEW_STATE_IND indicating that RRC has moved to a new state.
  This function is called by the dispatcher.
  
  This function initiates the process of notifying procedures, if any,
  that need to be notified of the state change.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcscmgr_event_handler
(
  rrc_cmd_type *cmd_ptr    /* Pointer to received command */
);




#endif /* RRCSCMGR_H */

