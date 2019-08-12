#ifndef CMIPCALL_H
#define CMIPCALL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   I P  C A L L

                      M O D U L E

GENERAL DESCRIPTION

  This module implements the functionality of handling calls through
  IP. IP call reports are received and call object is updated. Call
  Origination, end functions are implemented to call registered
  applications that work using IP.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmipcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2005 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmipcall.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/13   qf      Added support for IMS rat_change between LTE and WLAN
10/04/12   ak/xs   Fix not sending MNG_CNF issue
03/27/12   xs      VoLTE Remove of user from Conf-call
02/13/12   xs      VT upgrade/downgrade support
05/26/10   jqi     EHRPD VOIP silent redial support.
11/05/09   mp      Added support to remove the last party added for VOIP calls
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
10/03/06   ka      Adding support for IP call on WCDMA.
07/14/06   ka      Fixing the issue where call cmd cb for call end api was
                   being called more than once.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
05/07/06   ka      Correcting signature of stop cont dtmf.
01/06/06   ic      Lint cleanup  
11/23/05   ka      Modification to allow hold/active for ip calls.
11/08/05   sk      Separated CM reports and commands.
10/08/05   ic      Added Header: field 
03/14/05   ka      cleaning up IP rpt and adding handling call cmd for IP
                   in forward_ll, reply_ll and cmd_check functions.
02/02/05   dk/ka   Initial release
===========================================================================*/

#include "mmcp_variation.h"
#if defined(FEATURE_IP_CALL)
#include "cmipappi.h"   /* Includes definitions for VoIP app */

typedef enum
{
   CMIPCALL_SUPS_ST_NONE,
     /* State None: initial state or transaction complete state */     
   CMIPCALL_SUPS_ST_WAIT_HOLD_CNF,
     /* mng calls is waiting for hold cnf */
   CMIPCALL_SUPS_ST_WAIT_RETRIEVE_CNF,
     /* mng calls is waiting for retrieve cnf */
   CMIPCALL_SUPS_ST_WAIT_ANS_CNF,
     /* mng calls is waiting for ans cnf */
   CMIPCALL_SUPS_ST_WAIT_END_CNF,
     /* mng calls is waiting for end cnf */
   CMIPCALL_SUPS_ST_WAIT_ANS_END_CNF,
     /* mng calls is waiting for answer cnf followed by end cnf */
   CMIPCALL_SUPS_ST_WAIT_HOLD_ANS_CNF,
     /* mng calls is waiting for hold cnf followed by answer cnf */
   CMIPCALL_SUPS_ST_WAIT_HOLD_RETRIEVE_CNF,
     /* mng calls is waiting for hold cnf followed by retrieve cnf */
   CMIPCALL_SUPS_ST_WAIT_END_ANS_CNF,
     /* mng calls is waiting for end cnf followed by answer cnf */
   CMIPCALL_SUPS_ST_WAIT_END_RETRIEVE_CNF,
     /* mng calls is waiting for end cnf followed by retrieve cnf */
   CMIPCALL_SUPS_ST_WAIT_CALL_DEFLECTION_CNF
   /* mng calls is waiting for call deflection cnf */
}cmipcall_sups_state_e_type;

/*===========================================================================

FUNCTION cmipcall_client_cmd_proc

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
void cmipcall_client_cmd_proc(

    cm_call_cmd_s_type  *call_cmd_ptr
        /* pointer to a CM command */
);


/*===========================================================================

FUNCTION cmipcall_send_orig

DESCRIPTION
  Directs a call origination to a VoIP app after matching an app with the
  right system mode and capability,

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  TRUE : If origination command was succesfully sent to IP APP.
  FALSE: If origination command was not sent to IP APP.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern boolean cmipcall_send_orig
(
  cmcall_s_type           *call_ptr
    /* Call object for the originating call
    */
);


/*===========================================================================

FUNCTION cmipcall_invite_res

DESCRIPTION
  Sends client response to invite request originating from App.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_invite_res
(
  const cmcall_s_type            *call_ptr,
    /* call object
    */

  boolean                         accept,
    /* TRUE or FALSE depending on if call is accepted or rejected
    */

  cmipapp_incom_rej_cause_e_type  cause
    /* causes for rejecting an incoming call. Can be set to
    ** CMIPAPP_INCOM_REJ_CAUSE_NONE for accept = TRUE
    */
);


/*===========================================================================

FUNCTION cmipcall_answer

DESCRIPTION
  Sends user response to an incoming call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_answer
(
  const cmcall_s_type            *call_ptr,
    /* call object
    */

  boolean                         accept,
    /* TRUE or FALSE depending on if call is accepted or rejected
    */

  cmipapp_incom_rej_cause_e_type  cause,
    /* causes for rejecting an incoming call. Can be set to
    ** CMIPAPP_INCOM_REJ_CAUSE_NONE for accept = TRUE
    */
  cm_ip_call_sip_reject_code_s_type      sip_reject_code
    /* SIP code sent for rejecting incoming call*/
);


/*===========================================================================

FUNCTION cmipcall_simple_flash

DESCRIPTION
  Sends user response to an call waiting indication or request to remove
  the last party added to the conference call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_simple_flash
(
  const cmcall_s_type                   *call_ptr
    /* call object
    */

);

/*===========================================================================

FUNCTION cmipcall_send_end

DESCRIPTION
  Sends a call end request to the respective app

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_send_end
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);


/*===========================================================================

FUNCTION cmipcall_send_abort

DESCRIPTION
  Ends a call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_send_abort
(
  const cmcall_s_type                  *call_ptr,
    /* call object
    */

  const cmipapp_abort_cause_e_type     abort_cause
    /* The call abort cause enum type.
    */
);


/*===========================================================================

FUNCTION cmipcall_end

DESCRIPTION
  Call object is made free and phone is informed if phone not in use

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Call object is deallocated.

===========================================================================*/
extern void cmipcall_end
(
  cmcall_s_type           *call_ptr
    /* call object
    */
);


/*===========================================================================

FUNCTION cmipcall_hold

DESCRIPTION
  Request to put call on hold

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_hold
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);


/*===========================================================================

FUNCTION cmipcall_active

DESCRIPTION
  Request to put the call active

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_active
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);

/*===========================================================================

FUNCTION cmipcall_start_cont_dtmf

DESCRIPTION
  Sends the dtmf for the requested digit

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_start_cont_dtmf
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */

  uint8                    digit
    /* DTMF digit
    */
);

/*===========================================================================

FUNCTION cmipcall_stop_cont_dtmf

DESCRIPTION
  Sends request to stop dtmf operation for the digit

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_stop_cont_dtmf
(
  const cmcall_s_type     *call_ptr,
    /* call object
    */

  uint16                    duration
    /* DTMF digit duration
    */
);


/*===========================================================================

FUNCTION cmipcall_rpt_proc

DESCRIPTION
  Process app reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_rpt_proc(

    const cm_hdr_type   *rpt_ptr
        /* Pointer to a IP app report */
);

/*===========================================================================

FUNCTION cmipcall_reset

DESCRIPTION
  Reset IP call fields to default values reflecting
  a IP call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION  cmipcall_client_cmd_check

DESCRIPTION
  Check for call command parameter errors and whether a specified call command
  is allowed in the current state of the call/phone.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if command is allowed in the current state
  of the call/phone, specific cm_call_cmd_err_e_type otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_cmd_err_e_type  cmipcall_client_cmd_check(

    cm_call_cmd_s_type          *call_cmd_ptr
        /* Pointer to a CM command */
);

/*===========================================================================

FUNCTION cmipcall_cmd_copy_fields

DESCRIPTION
  Copy the active IP fields of a call command into a call object.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmipcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

);

/*===========================================================================

FUNCTION cmipcall_modify_req

DESCRIPTION
  send call modify request to IMS

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_modify_req
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);

extern void cmipcall_modify_rsp
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);

/*===========================================================================

FUNCTION cmipcall_add_new_party

DESCRIPTION
  send request to IMS to add a new party to conference

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_add_new_party
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);

/*===========================================================================

FUNCTION cmipcall_process_call_deflection

DESCRIPTION
  send request to IMS to handle communication deflection

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_process_call_deflection
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */
  const cm_call_cmd_info_s_type    *cmd_info_ptr
);



/*===========================================================================

FUNCTION cmipcall_remove_last_party

DESCRIPTION
  send request to IMS to remove last party from conference

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_remove_last_party
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);

/*===========================================================================

FUNCTION cmipcall_rat_change_rpt_proc

DESCRIPTION
  Process rat change indication from IMS and notify clients.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmipcall_rat_change_rpt_proc (
  const cmipapp_rpt_s_type  *rpt_ptr
);

/*===========================================================================

FUNCTION cmipcall_reset_mng_call_cnf

DESCRIPTION
  reset MNG call cnf structure to default values

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

extern void cmipcall_reset_mng_call_cnf(void);


/*===========================================================================

FUNCTION cmipcall_mng_state_update

DESCRIPTION
  Update mng_call_cnf states

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmipcall_mng_state_update(cm_name_type ip_ind_type, boolean ss_status, cm_call_id_type call_id, cm_ipapp_cause_e_type failure_cause);

/*===========================================================================

FUNCTION cmipcall_update_mng_state_per_call

DESCRIPTION
  Update mng_cnf state per call_id

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_update_mng_state_per_call(cm_call_id_type call_id,cmipcall_sups_state_e_type sup_state);


/*===========================================================================

FUNCTION cmipcall_end_req

DESCRIPTION
  Does call end necessary activities.
  Sends end_req event.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_end_req(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION  cmipcall_process_audio_release

DESCRIPTION
  This function handles the Audio Rel Indication from IMS.
  Basically, based on Sync flag, CM will send out the IMS Cache to NAS

DEPENDENCIES

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void cmipcall_process_audio_release(cm_audio_session_rel_msg_type *ipapp_audio_rel);


#endif /* defined(FEATURE_IP_CALL) */

#endif // CMIPCALL_H
