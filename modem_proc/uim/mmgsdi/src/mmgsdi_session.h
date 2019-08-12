#ifndef MMGSDI_SESSION_H
#define MMGSDI_SESSION_H
/*===========================================================================


           M M G S D I   S E S S I O N   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_session.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
12/09/14   stv     Add function for pre prov app start
10/23/14   yt      Updates to session and app management
05/08/14   hh      Correctly handle act_or_switch that results app deactivation
04/04/13   bcho    Function added to create and queue
                   MMGSDI_SESSION_DEACTIVATE_REQ cmd internally
03/01/12   bcho    Function prototype comment block updated
12/21/11   shr     Legacy GSDI removal updates
12/21/11   bcho    Changes for new API to open multiple sessions
                   in a single request
08/01/11   vs      Added support for session open with select response 
09/02/10   shr     Clean up Client ID dereg and Session close handling
03/29/10   shr     Switch/deactivate provisioning clean up and redesign
12/18/09   nb      Moved Dual Slot Changes
01/10/08   nk      Added featuriztion to correlate with function definition 
                   featurization.
11/15/07   sun     Added support for NAA refresh
04/03/07   sp      Send status command before terminating the app.
12/12/06   tml     Update mmgsdi_session_build_select_aid to allow for activation
                   and deactivation of aid with the same api
08/20/06   tml     Added App Reset and 3G Session Reset supports
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     Externalize mmgsdi_session_release_info.
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
07/12/05   tml     Session Management and ISIM support
02/24/05   tml     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:  MMGSDI_ISIM_AUTH_RSP_TAG_ENUM_TYPE
   Indicates what type of authentication response
-----------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_ISIM_3G_AUTH_SUCCESS    = 0xDB,
  MMGSDI_ISIM_SYNC_FAIL          = 0xDC
} mmgsdi_isim_auth_rsp_tag_enum_type;


/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/


/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_MAIN

   DESCRIPTION:
     This function will provide open session using the ICC protocol

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_main (
  mmgsdi_task_cmd_data_type      * cmd_ptr
);



/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_MAIN_RESPONSE

   DESCRIPTION:
     This function will handle the session management response processing.  

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_main_response (
  mmgsdi_cmd_enum_type              cmd_type,
  const mmgsdi_uim_report_rsp_type * rsp_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RELEASE_INFO

   DESCRIPTION:
     This function will release information in regard to the session that
     has been deactivated and closed.

   DEPENDENCIES:


   LIMITATIONS:
     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

    SIDE EFFECTS:
     The function will free the memory allocated to the input pointer
     before return under all cases.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr and channel info ptr.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_release_info (
  mmgsdi_session_info_type    *session_info_ptr,
  mmgsdi_cmd_enum_type        orig_request_type
);


/*===========================================================================

   FUNCTION:      MMGSDI_SESSION_RELEASE_INTERNAL_SESSIONS

   DESCRIPTION:
     This function will release information of sessions that
     have been opened intenrally by MMGSDI for the client.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_release_internal_sessions (
  int32 client_index
);


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_STATUS_TERMINATE

DESCRIPTION:
  This function is used to build a status command for a particular 
  application termination.

DEPENDENCIES:

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  If status command is executed succesfully, it builds up a command to 
  terminate the app also.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_build_status_terminate (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_cmd_enum_type     orig_req_cmd,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_DEACTIVATE

DESCRIPTION:
  This function is used to build a deactivate command for a particular 
  application

DEPENDENCIES:
  The client must have a valid session open.
  Achived by calling mmgsdi_session_management_open.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_build_deactivate (
  mmgsdi_client_id_type    client_id,
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_cmd_enum_type     orig_req_cmd,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_SELECT_AID

DESCRIPTION:
  This function will build the select AID command and put it into the
  mmgsdi command queue

DEPENDENCIES:


LIMITATIONS:
     

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                           but the service table indicates the card does not
                           have the support

SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_build_select_aid(
  mmgsdi_client_id_type              client_id,
  mmgsdi_session_id_type             session_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_cmd_enum_type               orig_req_type,
  mmgsdi_cmd_enum_type               curr_req_type,
  mmgsdi_static_data_type            aid,
  boolean                            select_rsp_requested,
  mmgsdi_select_rsp_enum_type        select_rsp_type,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_ACTIVATE_APP_INFO

   DESCRIPTION:
     This function will add the app information to the session index of the 
     client. Will be used by open session and refresh

   DEPENDENCIES:


   LIMITATIONS:
     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_activate_app_info(
  mmgsdi_session_id_type        session_id,
  mmgsdi_static_data_type       app_id);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_OPEN

   DESCRIPTION:
     This function will perform parameter checks, generate session ID, save
     the data into the client reg table and proceed with the Manage Channel
     Open command to the card

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to client id reg table.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_proc_open(
  mmgsdi_session_open_req_type *msg_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_OPEN_EXT

   DESCRIPTION:
     This function will perform parameter checks, generate multiple session IDs 
	   as requested, save the data into the client reg table
   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to client id reg table.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_proc_open_ext(
  mmgsdi_session_open_ext_req_type *msg_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_OPEN_EXT_CLEAN_UP

   DESCRIPTION:
     This function will perform the cleanup of all the previously opened 
     sessions if any of the requested session fails to open  in 
     MMGSDI_SESSION_OPEN_EXT command. 

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE: None

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_session_open_ext_cleanup(mmgsdi_session_open_ext_cnf_info_type *session_cnf_ext_info_ptr,
  uint8 num_sessions_opened
);

/*===========================================================================
  FUNCTION MMGSDI_SESSION_DEACTIVATE_MISMATCH_SESSIONS

DESCRIPTION
  This function will check whether the destination app is free or not.
  If no, then trigger de-activation for destination app. Also, this
  function will check if there is any wrong-mappings with respect to
  current req. If yes, then deactivate all those wrong mapped sessions
  also.
  halt_activation_ptr: This is an output parameter for knowing whether
  we need to return from caller or proceed with our activation process.
  In case there is some ERROR or destination App is not free we need to
  return and the activation will be triggered again from callback fn.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_deactivate_mismatch_sessions(
  const mmgsdi_session_act_or_switch_prov_req_type  *msg_ptr,
  boolean                                           *halt_activation_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_ACTIVATE_PROVISIONING_START

   DESCRIPTION:
     This function will verify if the activate provisioning can be started,
     and if so start the activation procedure

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_activate_provisioning_start(
  mmgsdi_session_act_or_switch_prov_req_type *msg_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_SESSION_PRE_PROV_ACTIVATION_START

DESCRIPTION
  If skip_app_termination feature is enabled ,
      a. Checks if the activation is a re-activation of an application which is
         previously deactivated and the channel can be re-used
      b. If yes , updates the channel information for session and updates the
         skip app terminated globals
      c. If no, starts an activation sequence for the requested application afresh
  If skip_app_termination feature is disabled ,
      Starts an activation sequence for the requested application afresh

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_pre_prov_activation_start(
  mmgsdi_session_act_or_switch_prov_req_type *msg_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_ACT_OR_SWITCH_PROV

   DESCRIPTION:
     This function will perform parameter checks, generate session ID, save
     the data into the client reg table and proceed with the Manage Channel
     Open command to the card

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_proc_act_or_switch_prov(
  mmgsdi_session_act_or_switch_prov_req_type *msg_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_DEACTIVATE_START

   DESCRIPTION:
     This function will perform parameter checks and proceed with the building
     of SELECT AID with deactivate request

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_deactivate_start(
  mmgsdi_session_deact_req_type *msg_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_UICC_OPEN_CHANNEL_CHK

   DESCRIPTION:
     This helper function gets called as a part of session activate/switch
     provision app processing. The function decides if we need to open a
     channel with card and also sends a open channel command to the card.

   DEPENDENCIES:
    Only valid for UICC cards. Request Input Param should contain valid AID
    data

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_uicc_open_channel_chk(
  mmgsdi_session_act_or_switch_prov_req_type *req_ptr
);

/*===========================================================================
FUNCTION:      MMGSDI_SESSION_CREATE_AND_QUEUE_INTERNAL_DEACTIVATE_REQ

DESCRIPTION:
  Function to create MMGSDI_SESSION_DEACTIVATE_REQ cmd internally with and queue it

DEPENDENCIES:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None 
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_create_and_queue_internal_deactivate_req(
  mmgsdi_session_id_type session_id
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_SEND_STATUS_BEFORE_DEACTIVATE

   DESCRIPTION:
     This function will send the STATUS command before deactivating a UICC
     appclication

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_send_status_before_deactivate(
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot_id,
  const mmgsdi_aid_type   * app_data_ptr
);

#endif /* MMGSDI_SESSION_H */
