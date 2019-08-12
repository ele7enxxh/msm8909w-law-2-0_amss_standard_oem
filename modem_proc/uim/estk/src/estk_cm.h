#ifndef ESTK_CM_H
#define ESTK_CM_H

/*======================================================
FILE:  estk_cm.h

GENERAL DESCRIPTION:
    ESTK CM interface include file

        Copyright © 2008, 2010 - 2012QUALCOMM Technologies, Inc(QTI).
        All Rights Reserved.
        QUALCOMM Technologies Proprietary
=====================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_cm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/14   gs      Removed the ESTK featurization
08/06/13   vr      Centralized Call Control Changes
06/20/13   vr      Triton TSTS changes
05/16/12   dd      Use CM event to start DTMF timer
12/08/11   sg      Move ESTK-CM call event handling outside feature
10/17/11   bd      Fixed incorrect use of pointers in CM’s context
08/16/11   xz      Change DTMF pause timer to 3400 ms
03/31/11   ea      Add STK Redial support
01/22/11   bd      Clean up
01/12/11   adp     Fixing Compiler Error (file got renamed)  
08/23/10   xz      Change DTMF pause timer to 3600 ms
07/22/10   sg      Fix compilation warnings on 6610
07/19/10   dd      Add tag to the raw and decoded hdr for Set Up Call
02/15/10   nb      Moved Dual Slot Changes
02/05/10   xz      Define ESTK_MAX_SUBADDR_LEN as GSTK_MAX_SUBADDR_LEN
05/14/09   xz      Change DTMF Pause timer to 3200 ms
11/03/08   sk      Featurized ESTK based on external interface support
09/16/08   sk      Added lint fixes
09/08/08   tml     Fixed auto header version generation and add Send SS
                   support
09/02/08   sk      Initial Creation.
=============================================================================*/

/*--------------------------------------------------------------------------
                             COMMON DEFINTION HEADER FILES
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             INTERFACE HEADER FILES
---------------------------------------------------------------------------*/
#include "uim_variation.h"
#include "customer.h"

#if defined(FEATURE_ESTK)
#include "gstk_exp.h"
#include "estk_priv.h"
#include "cm_dualsim.h"
#include "cm.h"
#include "mn_cm_exp.h"

/*--------------------------------------------------------------------------
                             MACRO DEFINTIONS
---------------------------------------------------------------------------*/
#define ESTK_MAX_SUPS_REQ    10

#define ESTK_MAX_SUBADDR_LEN CM_CALLED_PARTY_SUBADDRESS_LENGTH

#define ESTK_DTMF_PAUSE_TIME        3400
#define ESTK_DTMF_DURATION          2000
#define ESTK_DTMF_PAUSE_TIMER_VALUE 3000

/*--------------------------------------------------------------------------
                             ENUMERATIONS
---------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                             STRUCTURE DEFINITIONS
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             DECLARATIONS
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION: estk_process_setup_call_req

DESCRIPTION:
  GSTK calls this ESTK function to process the set up call request.After 
  receiving this command from GSTK, the command is parsed for dtmf digits
  and a subsequent command is queued to CM. If a request to place the call is 
  successfully placed in the CM queue, ESTK then waits to receive the call
  connected event in order to send a Terminal Response. Other error events from
  CM received with respect to the call will be considered as a failure to set up
  the call and ESTK will send a suitable error Terminal Response to GSTK.

PARAMETERS:
  gstk_req_ptr : [Input] gstk set up call cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS: If the processing of the command was successful
    ESTK_ERROR  : If the processing of the command was unsuccessful

COMMENTS:
  The caller function will return a Terminal Response to GSTK if the
  return value from this function indicates any error.

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_setup_call_req(
   gstk_cmd_from_card_type      *gstk_req_ptr);

/*===========================================================================
FUNCTION: estk_process_send_dtmf_req

DESCRIPTION:
  GSTK calls this ESTK function to process the send dtmf request.After 
  receiving this command from GSTK, the command is parsed for dtmf digits
  and a subsequent command is queued to CM. 

PARAMETERS:
  gstk_req_ptr : [Input] gstk set up call cmd pointer
 
DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_dtmf_req(
   gstk_cmd_from_card_type      *gstk_req_ptr);

/*===========================================================================

FUNCTION: estk_cm_preprocess_call_orig_params

DESCRIPTION : 
  This ESTK function will look at the calling criteria such as
    1. No other calls
    2. Disconnect all other calls
    3. Putting existing calls on hold 
  and call the appropriate CM API.

PARAMETERS:
  gstk_req_ptr [Input]: GSTK request pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing call criteria is successful
    ESTK_ERROR   : If processing call criteria is not successful

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_cm_preprocess_call_orig_params(
   gstk_cmd_from_card_type   *gstk_req_ptr);

/*===========================================================================

FUNCTION: estk_process_setup_call_user_cnf

DESCRIPTION : 
  This function will be called by ESTK to process the user confirmation response
  received from the UI client.If the user confirmed for an affirmative action, 
  the call will be set up, otherwise an appropriate Terminal Response will be 
  sent to the card saying "The user rejected the call setup".
  If the user fails to respond within a certain time, a Terminal response of 
  "No response from the user will be sent to the card.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer 

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_setup_call_user_cnf_cmd(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_end_all_calls

DESCRIPTION:
  This function issues a request to CM to end call(s).

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE:     The end request is sent, callback function
            will be called.
  FALSE:    The end request failed.

COMMENTS:
  None

SIDE EFFECTS:
  None
  
===========================================================================*/
boolean estk_end_all_calls(void);

/*===========================================================================

FUNCTION: estk_process_cm_call_event_cmd

DESCRIPTION : 
  This function will be called by ESTK to process CM events
  related to the following proactive commands.
  1. SETUP CALL
			  
PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer 

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If porcessing of call event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_call_event_cmd(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_process_cm_call_list_cmd

DESCRIPTION:
  This function will be called by ESTK to process CM response for the list 
  of active calls.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer 

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_call_list_cmd(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION:   estk_setup_call_cleanup

DESCRIPTION:
  This ESTK function clears thse setup call related private data.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_setup_call_cleanup(void);

/*===========================================================================
FUNCTION:   estk_send_dtmf_cleanup

DESCRIPTION:
  This ESTK function clears thse send dtmf related private data.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_send_dtmf_cleanup(void);

/*===========================================================================
FUNCTION:   estk_send_DTMF

DESCRIPTION:
  This ESTK function finds the pauses in the DTMF string and tries to send the
  DTMF digits in bursts.

PARAMETERS:
  cmf_ref_id : [INPUT] command reference ID 

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_send_DTMF(uint32 cmd_ref_id);

/*===========================================================================
FUNCTION:   estk_send_burst_DTMF

DESCRIPTION:
  This ESTK function sets a timer to handle the DTMF in bursts

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_send_burst_DTMF(void);

/*===========================================================================
FUNCTION :  estk_handle_burst_DTMF

DESCRIPTION:
  This ESTK function sends DTMF digits to CM in bursts

PARAMETERS:
  dummy : [Input] dummy parameter

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_handle_burst_DTMF(uint32 dummy);

/*===========================================================================
FUNCTION :  estk_handle_redial_timer

DESCRIPTION:
  This ESTK function clears the redial timer

PARAMETERS:
  dummy : [Input] dummy parameter

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_handle_redial_timer(uint32 dummy);

/*===========================================================================
FUNCTION :  estk_handle_cm_cmd_reattempt_timer

DESCRIPTION:
  This ESTK function will send a reattempt cm cmd

PARAMETERS:
  dummy : [Input] dummy parameter

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_handle_cm_cmd_reattempt_timer(uint32 dummy);

/*=============================================================================
FUNCTION: estk_call_err_cmd_cb

DESCRIPTION:
  Command callback from CM to report the status of queuing a request to CM

PARAMETERS:
  *data_block_ptr : [Input]  estk data block pointer
  call_cmd        : [Input]  cm command type
  call_cmd_err    : [Input]  cm command error type

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void estk_call_err_cmd_cb (
  void                   *data_block_ptr,
  cm_call_cmd_e_type     call_cmd,
  cm_call_cmd_err_e_type call_cmd_err
);

/*===========================================================================
FUNCTION: estk_process_DTMF

DESCRIPTION:
  This function processes the DTMF request

PARAMETERS:
  None
 
DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_DTMF(void);

/*===========================================================================

FUNCTION: estk_sups_process_send_ussd_req

DESCRIPTION:
  Process the Send USSD Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] request pointer for USSD in gstk format

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_sups_process_send_ussd_req (
   gstk_cmd_from_card_type *gstk_req_ptr);

/*===========================================================================
FUNCTION: estk_process_send_ss_req

DESCRIPTION:
  Process the Send SS Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] request pointer for SS in gstk format 

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_ss_req (
   gstk_cmd_from_card_type *gstk_req_ptr
);

/*===========================================================================
FUNCTION: estk_cm_sups_event_cb

DESCRIPTION:
  This is the event callback function registered with CM for supplementary
  service events

PARAMETERS:
  sups_event    : [Input] CM supplementary service event
  sups_info_ptr : [Input] CM supplementary service data info

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_sups_event_cb(
  cm_sups_event_e_type          sups_event,  
  const cm_sups_info_s_type    *sups_info_ptr
);

/*===========================================================================

FUNCTION: estk_sups_cleanup

DESCRIPTION:
  Clear the Sups related private data

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_sups_cleanup( void );

/*===========================================================================

FUNCTION:estk_cm_call_info_list_cb

DESCRIPTION :
  Callback function invoked as result of cm_mm_call_cmd_get_call_info_list().

PARAMETERS:
  list_ptr : [Input] Active call list from CM 

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_call_info_list_cb(const cm_call_state_info_list_s_type *list_ptr);

/*===========================================================================
FUNCTION :  estk_cm_call_event_cb

DESCRIPTION:
  Call event callback function, called when a call event is happening.
  This function populate the various call related indication type and
  put the command to the ESTK command queue.

PARAMETERS:
  call_event    : [Input] cm call event
  call_info_ptr : [Input] Pointer to the call info struct

DEPENDENCIES:
  gstk command buffer is available

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_call_event_cb (
  cm_call_event_e_type          call_event,
  const cm_mm_call_info_s_type *call_info_ptr
);

/*===========================================================================

FUNCTION: estk_process_cm_sups_event_cmd

DESCRIPTION :
  This function will be called by ESTK to process CM events
  related to supplimentary services

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing of call event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_sups_event_cmd(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_process_cm_inband_event_cmd

DESCRIPTION : 
  This function will be called by ESTK to process the inband event command

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer 

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_inband_event_cmd(
  estk_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION: estk_cm_inband_event_cb

DESCRIPTION:
  This is the event callback function registered with CM for inband
  service events

PARAMETERS:
  inband_event    : [Input] Inband event
  inband_info_ptr : [Input] Inband data info

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_inband_event_cb(
  cm_inband_event_e_type        inband_event,
  const cm_inband_info_s_type*  inband_info_ptr); 
#endif /* FEATURE_ESTK */
#endif /* ESTK_CM_H */

