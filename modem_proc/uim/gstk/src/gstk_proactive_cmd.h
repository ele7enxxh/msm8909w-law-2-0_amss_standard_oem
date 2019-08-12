#ifndef gstk_proactive_cmd_h
#define gstk_proactive_cmd_h
/*===========================================================================


            G S T K   P R O A C T I V E   C M D   H E A D E R



GENERAL DESCRIPTION
  This header file contains function definitions to be used by
  gstk_proactive_cmd.c


FUNCTIONS
  gstk_preprocess_proactive_cmd
    This function parsed the command into:
    proactive command tag, total tlv length, command detail tlv
    and device identities tlv and performs early error detection on these fields
    with length field == uint32

  gstk_find_expected_proactive_cmd_rsp
    This function mapped the passed in command type to the corresponding
    response type

  gstk_display_text_req
    This function parses the Display Text Proactive Command from the card
    and sends it to the client.

  gstk_get_inkey_req
    This function parses the Get Inkey Proactive Command from the card
    and sends it to the client.

  gstk_get_input_req
    This function parses the Get Input Proactive Command from the card
    and sends it to the client.

  gstk_lang_notification_req
    This function parses the language notification Proactive Command from
    the card and sends it to the client.

  gstk_launch_browser_req
    This function parses the launch browser Proactive Command from the card
    and sends it to the client.

  gstk_more_time_req
    This function parses the more time Proactive Command from the card
    and sends it to the client.

  gstk_play_tone_req
    This function parses the play tone Proactive Command from the card
    and sends it to the client.

  gstk_polling_req
    This function parses the polling Proactive Command from the card
    and sends it to the client.

  gstk_provide_local_info_req
    This function parses the provide local info Proactive Command from the card
    and sends it to the client.

  gstk_select_item_req
    This function parses the Select item Command from the card
    and sends it to the client.

  gstk_send_dtmf_req
    This function parses the Send dtmf Proactive Command from the card
    and sends it to the client.

  gstk_send_sms_req
    This function parses the Send SMS Proactive Command from the card
    and sends it to the client.

  gstk_send_ss_req
    This function parses the Send SS Proactive Command from the card
    and sends it to the client.

  gstk_send_ussd_req
    This function parses the Send USSD Proactive Command from the card
    and sends it to the client.

  gstk_setup_mo_call_req
    This function parses the Setup call Proactive Command from the card
    and sends it to the client.

  gstk_setup_evt_list_req
    This function parses the Setup event list Proactive Command from the card
    and sends it to the client.

  gstk_setup_idle_mode_text_req
    This function parses the Setup idle mode text Proactive Command from the
    card and sends it to the client.

  gstk_setup_menu_req
    This function parses the Setup menu Command from the card
    and sends it to the client.

  gstk_timer_management_req
    This function parses the timer management Proactive Command from the card
    and sends it to the client.

  gstk_open_ch_req
    This function parses the Open Channel Command from the card
    and sends it to the client.

  gstk_close_ch_req
    This function parses the Close Channel Command from the card
    and sends it to the client.

  gstk_send_data_req
    This function parses the Send Data Command from the card
    and sends it to the client.

  gstk_receive_data_req
    This function parses the Receive Data Command from the card
    and sends it to the client.

  gstk_get_ch_status_req
    This function parses the Get Channel Status Command from the card
    and sends it to the client.

  gstk_activate_req
    This function parses the Activate Command from the card
    and sends it to the client.

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2005 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_proactive_cmd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/14   gm      Support of CSG Rel-11 feature 
09/13/13   gm      Support for Recovery and hot-swap
10/31/11   sg      Class L and Class M support
02/16/11   bd      Support for delaying the TR when card is repeating the
                   same proactive command continuously.
11/25/10   ssr     Fixed command detail parsing
10/11/10   sg      Port IMSI_M enhancement
10/18/09   dd      Forward raw refresh proactive message to gstk clients
10/08/06   sk      Added support for unknown unsupported proactive commands.
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Added SAT/USAT data support
06/14/04   tml     Added support to differentiate GSTK auto generated END or
                   End from the card
05/11/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection 
                   evt dl supports
05/16/03   tml     linted
02/19/03   jar     Added gstk_refresh_req
02/12/03   tml     Added gstk_find_expected_proactive_cmd_rsp
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk.h"
#include "mmgsdisessionlib.h"

/*===========================================================================
FUNCTION gstk_raw_proactive_cmd_cleanup_on_error

DESCRIPTION
  This function free up memory allocated while processing raw proactive command,
  free command details reference id and send GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  terminal response to UIM server.

PARAMETERS
  reg_info_ptr        [input] The pointer to the raw proactive command data
  cmd_type            [input] The proactive command id in GSTK type
  cmd_detaisl_ref_id  [input] The command details reference id

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
extern void gstk_raw_proactive_cmd_cleanup_on_error(
  gstk_client_pro_cmd_reg_data_type  *reg_info_ptr,
  gstk_cmd_enum_type                 cmd_type,
  uint32                             cmd_details_ref_id
);

/*===========================================================================
FUNCTION gstk_preprocess_proactive_cmd

DESCRIPTION
  This function parsed the command into:
  proactive command tag, total tlv length, command detail tlv
  and device identities tlv and performs early error detection on these fields
  The rest of the data is put into the cmd_data (passed in parameter)
  cmd_data_len indicates the size of the remaining tlv datas after parsing
  the command details and device ID
  cmd_details_ref_id is the index location in the global command details ref
  table.  If there is still free slot in the command details ref table,
  the command details thus parsed will be assigned to the index location (as
  indicated by cmd_details_ref_id) in the global ref table

PARAMETERS
  cmd: [Input] Pointer to the original proactive command data to be processed
  cmd_data: [Input/Output] Pointer to be populated for the rest of the proactive
                           cmd data after command details and device id has been
                           processed
  cmd_data_len: [Input/Output] Length for valid data in the cmd_data that is
                               being returned to the user
  cmd_details_ref_id: [Input/Output] Pointer to command details reference
                                     index location allocated for this
                                     proactive command in the
                                     command_details_ref_table

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
extern gstk_status_enum_type gstk_preprocess_proactive_cmd(
         gstk_proactive_cmd_type*  cmd,
         uint8**                   cmd_data,
         int32*                    cmd_data_len,
         uint32*                   cmd_details_ref_id);

/*===========================================================================
FUNCTION GSTK_PROCESS_PROACTIVE_COMMAND

DESCRIPTION

  This function switches on the command id and call the corresponding
  proactive command to further parse/process the command

PARAMETERS
  cmd: [Input] Pointer to the proactive command to be processed

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
extern gstk_status_enum_type gstk_process_proactive_command(
    gstk_proactive_cmd_type* cmd );


/*===========================================================================
FUNCTION gstk_find_expected_proactive_cmd_rsp

DESCRIPTION
  This function mapped the passed in command type to the corresponding
  response type

PARAMETERS
  request_cmd_type: [Input] Original proactive command type to be mapped
  expected_cmd_rsp: [Input/Output] Pointer to response command type to be
                                   populated

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type
      GSTK_SUCCESS
      GSTK_UNSUPPORTED_COMMAND
      GSTK_INVALID_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_cmd_enum_type gstk_find_expected_proactive_cmd_rsp(
                           uint32              request_cmd_type);

/*===========================================================================
FUNCTION gstk_display_text_req

DESCRIPTION
  This function parses the Display Text Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_display_text_req  (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_get_input_req

DESCRIPTION
  This function parses the Get Input Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_get_input_req (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_get_inkey_req

DESCRIPTION
  This function parses the Get Input Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_get_inkey_req (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_play_tone_req

DESCRIPTION
  This function parses the Play Tone Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_play_tone_req (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_setup_evt_list_req

DESCRIPTION
  This function parses the Set Up Event List Proactive Command from the card
  and update the static event list variable and register with the correct
  modules for the event indicated.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_setup_evt_list_req (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_provide_local_info_req

DESCRIPTION
  This function parses the Provide Local Info Proactive Command from the card
  and get the information from the various module and send the terminal
  response to the Card.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_provide_local_info_req (gstk_proactive_cmd_type* command);

/*===========================================================================
FUNCTION gstk_setup_mo_call_req

DESCRIPTION
  This function parses the Set Up Call Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_setup_mo_call_req  (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_end_proactive_session

DESCRIPTION

  This function notifies all the gstk client regarding the end of a
  proactive session

PARAMETERS
  None

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
extern gstk_status_enum_type gstk_end_proactive_session(
                                  gstk_proactive_cmd_type* cmd,
                                  gstk_tag_enum_type       gstk_tag);


/*===========================================================================
FUNCTION gstk_setup_idle_mode_text_req

DESCRIPTION
  This function parses the Setup Idle Text Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_setup_idle_mode_text_req  (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_launch_browser_req

DESCRIPTION
  This function parses the Launch Browser Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_launch_browser_req  (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_send_ss_req

DESCRIPTION
  This function parses the Send SS Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_send_ss_req  (gstk_proactive_cmd_type* command);

/*===========================================================================
FUNCTION gstk_send_ussd_req

DESCRIPTION
  This function parses the Send ussd Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_send_ussd_req  (gstk_proactive_cmd_type* command);

/*===========================================================================
FUNCTION gstk_send_sms_req

DESCRIPTION
  This function parses the Send SMS Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_send_sms_req  (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_polling_req

DESCRIPTION
  This function parses the Poll Interval Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_polling_req (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_more_time_req

DESCRIPTION
  This function parses the more time proactive command, restart the various
  proactive command timers within gstk.  The function also sends the
  more time command to its clients.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_more_time_req (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION GSTK_SETUP_MENU_REQ

DESCRIPTION
  This function parses the Set Up Menu Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_setup_menu_req (
    gstk_proactive_cmd_type* command
);


/*===========================================================================
FUNCTION GSTK_SELECT_ITEM_REQ

DESCRIPTION
  This function parses the select item Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_select_item_req (
    gstk_proactive_cmd_type* command
);
/*===========================================================================
FUNCTION gstk_refresh_req

DESCRIPTION
  This function parses the REFRESH Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_refresh_req (gstk_proactive_cmd_type* command);

/*===========================================================================
FUNCTION gstk_timer_management_req

DESCRIPTION
  This function parses the Timer Management Proactive Command from the card
  and get the information from the various module and send the terminal
  response to the Card.
  In case of any parsing errors or when GSTK can't set up the timer,
  it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_timer_management_req (gstk_proactive_cmd_type* command);


/*===========================================================================
FUNCTION gstk_lang_notification_req

DESCRIPTION
  This function parses the language notification proactive command, restart
  the various proactive command timers within gstk.  The function also sends
  the more time command to its clients.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_lang_notification_req (gstk_proactive_cmd_type* command);

/*===========================================================================
FUNCTION gstk_send_dtmf_req

DESCRIPTION
  This function parses the Send dtmf Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_send_dtmf_req  (gstk_proactive_cmd_type* command);

/*===========================================================================
FUNCTION gstk_open_ch_req

DESCRIPTION
  This function parses the Open Channel Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_open_ch_req ( gstk_proactive_cmd_type* command );

/*===========================================================================
FUNCTION gstk_close_ch_req

DESCRIPTION
  This function parses the Close Channel Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_close_ch_req (
    gstk_proactive_cmd_type* command
);


/*===========================================================================
FUNCTION gstk_send_data_req

DESCRIPTION
  This function parses the Send Data Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_send_data_req (
    gstk_proactive_cmd_type* command
);

/*===========================================================================
FUNCTION gstk_receive_data_req

DESCRIPTION
  This function parses the Send Data Command from the card
  and receives it to the client.

  In case of any parsing errors or when GSTK can't receive the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_receive_data_req (
    gstk_proactive_cmd_type* command
);

/*===========================================================================
FUNCTION gstk_get_ch_status_req

DESCRIPTION
  This function parses the Get Channel Status Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_get_ch_status_req (
    gstk_proactive_cmd_type* command
);

/*===========================================================================
FUNCTION gstk_activate_req

DESCRIPTION
  This function parses the Activate Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
extern gstk_status_enum_type gstk_activate_req (
    gstk_proactive_cmd_type* command
);

/*===========================================================================
FUNCTION gstk_build_unknown_unsupported_term_rsp

DESCRIPTION
  This function send terminal response for an unsupported 
  or unknown proactive command.

PARAMETERS
  cmd_details_ref_id: [Input]
  result:             [Input] 
  cnf_type:           [Input]    

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
extern void gstk_build_unknown_unsupported_term_rsp(
    uint32                              cmd_details_ref_id,
    gstk_general_result_enum_type       result,
    gstk_cmd_enum_type                  cnf_type);

/*===========================================================================
FUNCTION gstk_unknown_unsupported_proactive_cmd_req

DESCRIPTION
  This function parses unknown or unsupported proactive command and populates 
  result as GSTK_BEYOND_ME_CAPABILITIES or GSTK_COMMAND_TYPE_UNKNOWN respectively.

PARAMETERS
  command:     [Input] Pointer to command info from card
  result_type: [Input] Result info that needs to be sent to the card.

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
gstk_status_enum_type gstk_unknown_unsupported_proactive_cmd_req (
    gstk_proactive_cmd_type        * command, 
    gstk_general_result_enum_type  result_type
);

/*===========================================================================
FUNCTION gstk_cmp_with_cached_cmd

DESCRIPTION

  This function compares the cached proactive command with ongoing command

PARAMETERS
  cmd_ptr: [Input] Pointer to the proactive command to be compared

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
gstk_status_enum_type gstk_cmp_with_cached_cmd (const gstk_proactive_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION gstk_send_provide_local_info_terminal_rsp

DESCRIPTION
  This function populate the gstk_provide_local_info_cnf_type and put the
  terminal response confirm type to the GSTK queue.

PARAMETERS
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  cmd_num           : [Input] This is the same value as that was being passed to
                              the client in the corresponding proactive command
                              request
  general_result    : [Input] Specifies the result of the proactive command
                              operation
  additional_result_ptr: [Input] Specifies the optional additional result for the
                             proactive command, the length field in the
                             gstk_additonal_info_type specifies number of
                             additional_info in the structure
  other_info_ptr: [Input] Pointer to the gstk_local_info_rsp_type

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
gstk_status_enum_type  gstk_send_provide_local_info_terminal_rsp(
  uint32                                  cmd_details_ref_id,
  uint8                                   cmd_num,
  gstk_general_result_enum_type           general_result,
  gstk_additional_info_type               *additional_result_ptr,
  const gstk_local_info_rsp_type          *other_info_ptr);
#endif /*gstk_proactive_cmd_h*/
