#ifndef CMWCALL_H
#define CMWCALL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   W C D M A   C A L L   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Call Object for WCDMA/GSM.

  The Call Object is responsible for:
  1. Processing clients' call commands.
  2. Processing Lower Layer replies for clients' call commands.
  3. Processing Lower Layer call related notifications.
  4. Notifying the client list of call events.


EXTERNALIZED FUNCTIONS

  cmcall_init
    Initializing the call object.

  cmwcall_info_get
    Copy the current call state information into a specified buffer.

  cmwcall_client_cmd_proc
    Process clients' call commands.

  cmwcall_rpt_proc
    Process Lower Layer call commands.

  cmwcall_client_orig_proc
    Perform origination processing in accordance with the loaded
    call origination plan.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmwcall.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/12   vk      Changes for 1XCSFB RLF handling.
01/20/12   gm      Data synchronization for CM Call objects
12/23/10   sv      Integrate DSDS feature
05/21/10   fj      Expose cmwcall_reset_gw_wcdma_ps_info().
10/31/08   rn      On going offline, send PDP_ABORT to NAS
07/07/07   pk      Adding support for handling calls in origination state
                   while service is lost
06/27/07   cl      Add new CM API to expose cmwcall_get_user_data() to RPC
09/14/06   ka      Removing function prototypes.
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Changes to support call control on PS calls.
11/08/05   sk      Separated CM reports and commands.
02/01/05   dk      Added WLAN Support.
11/18/04   ka      Adding support for MT PDP feature
06/25/04   dk      Changed signature of cmwcall_end_call().
05/27/04   ic      Comment fix
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/19/04   ka      Added cmwcall_get_nsapi function prototype.
12/12/03   ws      Added cmwcall_set_lmtd_srvc_req
10/03/03   ws      Fixed compiler warnings.
09/18/03   RI      Added prototypes -cmwcall_set_emergency_call_no_sim(),
                   cmwcall_reset_lmtd_srvc_req(),cmwcall_lmtd_srvc_requested().
07/29/03   ws      Added changes for Automatic Silent ReDial.
02/03/03   prk     Added cmwcall_end_req function prototype.
01/21/03   ws      Updated copyright information for 2003
11/22/02   ic      Added prototype of cmwcall_orig_proc() to remove compiler
                   warnings for LTK 6300 run time CM MM build
11/14/02   vt      Merged changes from GW mainline.
                   [CS]Added definition of cmwcall_end.
06/11/02   RI      Deleted the pointer to the call object parameter in
                   cmwcall_client_cmd_check().
03/11/02   RI      Used default value defintions for cause value and
                   CLIR from mn_cm_exp.h
02/19/02   PRK     Removed check of service state for CM_CALL_CMD_ANSWER
                   commands.  Client is allowed to answer incoming call with
                   no service.
02/06/02   PRK     Added function cmcall_overall_call_state.
01/28/02   PRK     Replaced cmutil_ prefix with cm_util_ prefix.
                   Modified cmcall_client_cmd_check to check is_subscription_-
                   available flag.
01/25/02   RI      Modified NotifySS processing for Call Forwarding
                   Removed the parameter check for N SEND and
                   N X SEND commands.
01/04/02   HD      Merge to Mainline
                   Do not reset call at end_req and copy cc_cause for Disc msg.
11/21/01   RI      Modified cmwcall_info_get() to copy rab_id for CS Data.
                   Modified PS Data ind/conf processing.
                   Modified cm_find_call_id() to include the connection ID src.
11/15/01   AT      Updated AOC to respond to MN.
11/06/01   RI      Modified cmwcall_cmd_copy_fields().
11/01/01   AT      Cleaning up AOC at end of call.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Modified code due to srv_state moving from phone group
                     to serving system group (FEATURE_NEWSS).
10/17/01   AT      Added Support for Advice of Charge (AOC)
10/01/01   AT      Added check for E911 changes.
08/23/01   HD      NAS header file reorg changes.
04/27/01   HD      Initial release for WCDMA/GSM.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "cm.h"
#include "cm_v.h"
#include "cmi.h"
#include "sm.h"        /* GET_NSAPI macro definition */
#include "smix.h"      /* L3 pack and unpack functions */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Delay in sending end on sec pdp in seconds */
#define CMWCALL_DELAY_PDP_END  1
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION cmwcall_send_orig

DESCRIPTION
  Send an origination command to MN.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_send_orig(

    cmcall_s_type    *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmwcall_reset

DESCRIPTION
  Reset call fields to default values reflecting a call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmwcall_send_cs_end_req

DESCRIPTION
  Send call end request to MN.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_send_cs_end_req (

  byte              connection_id,
    /* Connection id of the call to be cleared
    */

  cm_ss_e_type      ss
    /* Stack on which the call is present */
);

/*===========================================================================

FUNCTION cmwcall_end_req

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
void cmwcall_end_req(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmwcall_end

DESCRIPTION
  Does call end necessary activities.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmwcall_end_call

DESCRIPTION
  Does call end necessary activities.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_end_call(

  cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmwcall_cmd_copy_fields

DESCRIPTION
  Copy the active fields of a call command into a call object.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

);




/*===========================================================================

FUNCTION  cmwcall_client_cmd_check

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
  If originating dial-string matches one of the special numbers that is
  stored in the phone-book, this function changes the call type to E911,
  OTASP, etc.

===========================================================================*/
cm_call_cmd_err_e_type  cmwcall_client_cmd_check(

    cm_call_cmd_s_type          *call_cmd_ptr
        /* Pointer to a CM command */
);





/*===========================================================================

FUNCTION cmwcall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmwcall_info_get(

    cm_mm_call_info_s_type        *call_info_ptr,
        /* Copy call state info into this buffer */

    const cmcall_s_type           *call_ptr
        /* Pointer to the call object  */
);



/*===========================================================================

FUNCTION cmwcall_client_cmd_proc

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
void cmwcall_client_cmd_proc(

    cm_call_cmd_s_type  *call_cmd_ptr
        /* pointer to a CM command */
);


/*===========================================================================

FUNCTION cmwcall_client_orig_proc

DESCRIPTION
  Process clients' call origination command.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmwcall_client_orig_proc(

    cmcall_s_type  *call_ptr
        /* pointer to a CM command */
);


/*===========================================================================

FUNCTION cmwcall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);


/*===========================================================================

FUNCTION cmwcall_lmtd_srvc_requested

DESCRIPTION
  Check if a Limited Service Request was sent for an Emergency call or not.

DEPENDENCIES

  None.

RETURN VALUE
  TRUE - if a Lmtd Service request was sent.
  FALSE - otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmwcall_lmtd_srvc_requested( void );


/*===========================================================================

FUNCTION cmwcall_reset_lmtd_srvc_req

DESCRIPTION
  Reset the Limited Service Request flag set for an Emergency call.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmwcall_reset_lmtd_srvc_req( void );

/*===========================================================================

FUNCTION cmwcall_set_lmtd_srvc_req

DESCRIPTION
  Set the Limited Service Request flag set for an Emergency call.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmwcall_set_lmtd_srvc_req( void );

/*===========================================================================

FUNCTION cmwcall_set_emergency_call_no_sim

DESCRIPTION
  Set the static boolean cmwcall_emergency_call_no_sim

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_set_emergency_call_no_sim(boolean set_value);


/*===========================================================================

FUNCTION cmwcall_calculate_nsapi

DESCRIPTION
  Given a connection Id, this function returns the associated Network Layer
  Service Access Point (NSAPI).

DEPENDENCIES
  None

RETURN VALUE
  The calculated NSAPI is returned.

SIDE EFFECTS
  None

===========================================================================*/
nsapi_T cmwcall_calculate_nsapi(

    const byte   connection_id,
  /* connection ID */
    sys_modem_as_id_e_type as_id
);



/*===========================================================================

FUNCTION cmwcall_get_nsapi

DESCRIPTION
  This function returns the associated Network Layer Service
  Access Point (NSAPI) associated with the given call object.

DEPENDENCIES
  None

RETURN VALUE
  The call object's NSAPI is returned.

SIDE EFFECTS
  None

===========================================================================*/
nsapi_T cmwcall_get_nsapi(

    const cm_call_id_type   call_id
  /* connection ID */
);

/*===========================================================================

FUNCTION  cmwcall_call_cmd_orig_para_check

DESCRIPTION
  Check parameter errors for call origination command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmwcall_call_cmd_orig_para_check
(
  cm_call_cmd_s_type       *call_cmd_ptr
);

/*===========================================================================

FUNCTION  cmwcall_call_cmd_ps_orig_state_check

DESCRIPTION
  This function checks if the state of the phone can support the origination
  of this PS call.  The checks made by this function include:

    1.  Check the SIM state to determine if it supports PS calls.
    2.  Checks if the maximum number of primary PDP contexts is not exceeded.
    3.  Checks if the profile number is not in use already,
    4.  Checks if the maximum number of secondary per primary PDP context
        is not exceeded.
    5.  Checks if the primary call ID for the secondary PDP context activation
        is valid.

DEPENDENCIES
  none

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmwcall_call_cmd_ps_orig_state_check
(
  const cmcall_s_type       *ps_call_ptr
);


/*===========================================================================

FUNCTION cmwcall_encode_pdp_act_to_l3

DESCRIPTION

  Generates a byte stream after packaging pdp activation in Layer 3 format.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_encode_pdp_act_to_l3 (

  const cm_call_cmd_info_gw_ps_s_type   *ps_cmd_mode_info_ptr,

        byte                            *pdp_act_l3_ptr,

        word                            *pdp_act_l3_length,

        sys_modem_as_id_e_type          asubs_id
);


/*===========================================================================

FUNCTION cmwcall_decode_l3_to_pdp_act

DESCRIPTION

  Decodes Layer 3 format of pdp activation to CM structure.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_decode_pdp_act_to_l3 (

  const byte                            *pdp_act_l3,

  const word                            pdp_act_l3_length,

  cm_call_cmd_info_gw_ps_s_type   *ps_cmd_mode_info_ptr
);

/*===========================================================================

FUNCTION cmwcall_process_mt_setup_response

DESCRIPTION
  Sends setup response for a MT call

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmwcall_process_mt_setup_response
(
  cmcall_s_type                        *call_ptr,
  const boolean                         accept,
  const ie_cm_cc_cause_T               *cc_cause_ptr,
  const cm_repeat_indicator_T          *bc_repeat_indicator_ptr,
  const cm_bearer_capability_T         *bearer_capability_1_ptr,
  const cm_bearer_capability_T         *bearer_capability_2_ptr

);

/*===========================================================================

FUNCTION cmwcall_update_calls_on_srv_lost

DESCRIPTION
  Update the call object on service lost

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_LTE)) 

void     cmwcall_update_calls_on_srv_lost( 

  sd_ss_e_type    ss,

  boolean is_in_rlf
);
#else
void     cmwcall_update_calls_on_srv_lost( 

  sd_ss_e_type    ss
);
#endif




/*===========================================================================

FUNCTION cmwcall_send_pdp_abort_req

DESCRIPTION
  Sends PDP ABORT request to SM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmwcall_send_pdp_abort_req(

  cm_ss_e_type    ss
    /* Stack on which the abort request has to be sent */

);


/*===========================================================================

FUNCTION cmwcall_reset_gw_wcdma_ps_info

DESCRIPTION
  Resets the gw_ps_info in call_ptr

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

extern void cmwcall_reset_gw_wcdma_ps_info
(
  gsm_wcdma_cmcall_ps_type  *gsm_wcdma_ps_info
);


/*===========================================================================

FUNCTION cmwcall_timer_proc

DESCRIPTION
  Timer handling SRVCC cache expiry

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_timer_proc(cm_timer_event_type    timer_event);


 #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmwcall_handle_mid_srvcc_ims_call_obj

DESCRIPTION
  Process lower layer reports from msgr for srvcc ho

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_handle_srvcc_ims_cache(

  const cm_srvcc_call_context_rsp_s_type  *cmd_ptr
    /* Pointer to msgr report from IMS/QMI_VOICE */
);

/*===========================================================================

FUNCTION cmwcall_send_srvcc_events_to_clients

DESCRIPTION
  Sends HO events to clients for SRVCC FAIL/COMPLETE

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmwcall_send_srvcc_events_to_clients
(
  cm_srvcc_ho_ind_type     cmd,

  cm_call_id_type          complete_ho_call_id,
  /*none for 3rd FAILURE IND */

  cm_call_id_type          ims_call_id,

  sys_modem_as_id_e_type   asubs_id
);

/*===========================================================================

FUNCTION cmwcall_is_int_ims

DESCRIPTION
  Return true if int IMS enabled for SRVCC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
boolean cmwcall_is_int_ims( void );

/*===========================================================================

FUNCTION cmwcall_process_ims_audio_rel_cnf

DESCRIPTION
IMS cache ind initialize

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
extern void cmwcall_process_ims_audio_rel_cnf(void);

/*===========================================================================

FUNCTION cmwcall_handle_get_srvcc_context_req

DESCRIPTION
Code to handle get srvcc context request from NAS, and send across the context transfer response back to NAS.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
extern void cmwcall_handle_get_srvcc_context_req(cm_rpt_type *cm_rpt_ptr);


#endif // #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

#endif // CMWCALL_H
