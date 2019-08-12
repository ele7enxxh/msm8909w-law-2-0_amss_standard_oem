#ifndef CM_ACCESSCTRL_H
#define CM_ACCESSCTRL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   A C C E S S   C O N T R O L

GENERAL DESCRIPTION
  This module contains declarations used by the call manager in processing
  Access Control command. The module uses the callback function for the caller 
  and also MSGR is used for broadcasting the information between CM and all 
  other modules who are interested in the information of the access controller.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmdbm_init() must be called to initialize this module before any other
  function declared in this module is being called.

Copyright (c) 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmaccessctrl.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
08/08/13    xs      Remove emg handling from CMAC
02/15/13    th      Initial design
06/18/13    gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement

===========================================================================*/

        
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                  GLOBALS                                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define THIRD_PARTY_E911_CALL_ID 0xFE

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Enumeration of internal current state of access control module
**/
typedef enum
{
  CMAC_STATE_NORMAL               = 0,
  /**< AC is ready to enter into any state. */

  CMAC_STATE_WAIT_1X_SRV,
  /**< AC is waiting for establishing 1x NW service. */

  CMAC_STATE_WAIT_1X_DEREG,
  /**< AC is waiting for 1x NW to complete de-registration
   ** when clearing 1XPPP*/

  CMAC_STATE_1X_DEREG_END,
  /**< 1X PPP clean up operation completed - After 1X successful deregistration
  **   or after timeout trying to deregister with the network.
  */

  CMAC_STATE_EMERGENCY_PENDING,
  /* CM-clients have requested emergency state while e911 call is active */

  CMAC_STATE_3GPP_DETACH_PENDING,
  /* If modem has full srv, modem should detach before putting in lmtd srv */

  CMAC_STATE_EMERGENCY_ONLY,
  /* Modem allows emergency traffic only */

  CMAC_STATE_THERMAL_EMERG_LEVEL_2,
  /* Modem doesnt allow local hold calls */
  
  CMAC_STATE_WAIT_1X_DEREG_EMERG,
  /**< AC is waiting for 1x NW to complete de-registration. 
    ** when entering emergency mode*/
  
  CMAC_STATE_WAIT_CLNUP,
  /**< IMS deregistration and DS cleanup underway. Postpone restricting phone
  ** to EMERG only traffic.
  */

  CMAC_STATE_MAX

} cmac_state_e_type;


typedef struct
{
  uint16              reason_mask;

  cm_ac_reason_e_type reason;

  cmac_state_e_type   state;

  dword               bound_timer;

  cm_mode_pref_e_type cur_mode_pref;
  /* Current mode pref. */

  uint32              cm_act_ppp_cleanup_id;

  void                *data_block_ptr;
  /* pointer to a client provided callback data block */

  cm_ac_cmd_e_type    cmd_type;
  /* command */

  void (*cmd_cb_func)(void*, cm_ac_cmd_e_type, cm_ac_cmd_err_e_type);
  /* client callback function */

  void (*state_cb_func)(void*, cm_ac_cmd_e_type, cm_ac_state_e_type, cm_ac_end_reason_e_type);
  /* client callback function for ac state */

  sys_modem_as_id_e_type asubs_id;

  uint8     stop_mode_cnfs_pending;
  
} cmac_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               FUNCTION DECLARATIONS                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION cmac_init

DESCRIPTION
  Access control initialize

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmac_init( void );

/*===========================================================================

FUNCTION cmac_is_in_emergency_only

DESCRIPTION
  Returns TRUE if CM AC is in emergency only state due to any of the reasons

===========================================================================*/
boolean cmac_is_in_emergency_only(void);

/*===========================================================================

FUNCTION cmac_get_act_id

DESCRIPTION
  Return act id for access control  

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cm_act_id_type cmac_get_act_id (void);


/*==========================================================================
FUNCTION cmac_is_1x_ppp_dereg_end

DESCRIPTION
  Return TRUE if 1X PPP clean up operation is completed and CM is restoring  
  phone preference and forcing to SD

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/

extern boolean cmac_is_1x_ppp_dereg_end(void);


/*===========================================================================

FUNCTION cmac_init

DESCRIPTION
  Access control initialize

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmac_init( void );

/*===========================================================================

FUNCTION cmac_is_in_thermal_emergency

DESCRIPTION
  Returns TRUE if CM AC is in emergency due to UNSAFE temperature

===========================================================================*/
boolean cmac_is_in_thermal_emergency(void);


/*===========================================================================

FUNCTION cmac_is_in_sim_lock

DESCRIPTION
  Returns TRUE if CM AC is in emergency due to SIM Lock

===========================================================================*/
boolean cmac_is_in_sim_lock(void);

/*===========================================================================

FUNCTION cmac_restrict_to_emergency

DESCRIPTION
  This function implements access control putting phone in emergency mode
  due to reasons in cm_ac_reason_e_type. This happens in two stages:
  1. If GWL protocols are active, then a deactivate is sent
  2. After recving conf for GWL activate, GWL protocols are put in limited
     service mode

  If at #1, GWL protocols are not active or in limited srv already, then #2
  is done without wait.
===========================================================================*/

void cmac_restrict_to_emergency(

  cm_ac_reason_e_type reason
    /* Reason for putting modem in emergency mode */
);

/*===========================================================================

FUNCTION cmac_enable_emergency_if_pending

DESCRIPTION
  This function enables emergency mode if it was deferred due to emergency
  call or callback mode or operation modes.
===========================================================================*/
void cmac_enable_emergency_if_pending(void);

/*===========================================================================

FUNCTION cmac_restore_to_normal

DESCRIPTION
  Restore access control normal state where normal calls are enabled.
  Before call of this API, phone may be in emergency only mode due to
  reasons in cm_ac_reason_e_type.
===========================================================================*/

void cmac_restore_to_normal(

  cm_ac_reason_e_type reason
    /* Reason due to which normal opr being restored. */
);

/*===========================================================================

FUNCTION cmac_timer_proc

DESCRIPTION
  Process timer events

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmac_timer_proc(

  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    **
    ** Note that currently timer event is not being used
    */
);

/*===========================================================================

FUNCTION cmbcmcs_client_cmd_proc

DESCRIPTION
  Process clients' ac data commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmac_client_cmd_proc(

  cm_ac_cmd_s_type  *ac_cmd_ptr
    /* pointer to a CM command
    */
);

/*===========================================================================

FUNCTION cmac_mc_rpt_proc

DESCRIPTION
  Process ac reports specific to DS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmac_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);

/*===========================================================================

FUNCTION cmac_nas_rpt_proc

DESCRIPTION
  Process reports from NAS. In CMAC, we are looking at STOP_MODE_CNF

===========================================================================*/
void cmac_nas_rpt_proc(  

  const cm_hdr_type   *rpt_ptr

);

/*===========================================================================

FUNCTION cmac_call_cmd_check

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_cmd_err_e_type cmac_call_cmd_check(
  cm_call_cmd_s_type       *call_cmd_ptr
);

/*===========================================================================

FUNCTION cmac_send_notify_ind

DESCRIPTION
  Send event to client through message router with reason as payload

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmac_send_notify_ind(
  msgr_umid_type ac_evt_umid,
  cm_ac_reason_e_type reason,
  void* user_data,
  sys_modem_as_id_e_type asubs_id
);

/*===========================================================================
FUNCTION cmac_ph_cmd_proc

DESCRIPTION
  Process clients' ph commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmac_ph_cmd_proc(

  const cm_ph_cmd_s_type     *ph_cmd_ptr

);

/*===========================================================================

FUNCTION cmac_enter_thermal_emergency_level_2

DESCRIPTION
  Updates AC state to thermal emergency level 2
  Ends locally held calls
===========================================================================*/

void cmac_enter_thermal_emergency_level_2(void);

/*===========================================================================

FUNCTION cmac_is_in_thermal_emergency_level2

DESCRIPTION
  Returns TRUE if CM AC is in level2 thermal emergency due to UNSAFE temperature

===========================================================================*/
boolean cmac_is_in_thermal_emergency_level2(void);

/*===========================================================================

FUNCTION cmac_is_reason_mask_set

DESCRIPTION
  Checks if the reason_mask for AC state machine is set
 
RETURN VALUE 
  FALSE: if reason_mask is NONE
  TRUE: otherwise
===========================================================================*/

boolean cmac_is_reason_mask_set(void);


/*===========================================================================

FUNCTION cmph_ptr

DESCRIPTION
  Return a pointer to the one and only access control object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmac_s_type  *cmaccessctrl_ptr( void );

#endif //CM_ACCESSCTRL_H
