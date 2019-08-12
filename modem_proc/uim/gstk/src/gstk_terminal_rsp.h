#ifndef gstk_terminal_rsp_h
#define gstk_terminal_rsp_h
/*===========================================================================


       G S T K    T E R M I N A L    R E S P O N S E    H E A D E R


GENERAL DESCRIPTION
  This header file contains function required for gstk_terminal_rsp.c


FUNCTIONS
  gstk_populate_terminal_response_cnf
    This function build terminal response confirm type which will be put onto
    the GSTK command task later

  gstk_process_terminal_response
    This function switches on the command id and call the corresponding
    terminal response cnf function so that GSTK can build the corrresponding
    terminal response tlvs and send it to UIM Server

  gstk_uim_terminal_response_report
    This is the callback function for terminal response response from UIM.

  gstk_display_text_cnf
    This function packs the Display Text terminal response and send
    the command to UIM server.

  gstk_get_inkey_cnf
    This function packs the get inkey terminal response and send
    the command to UIM server.

  gstk_get_input_cnf
    This function packs the get input terminal response and send
    the command to UIM server.

  gstk_lang_notification_cnf
    This function packs the language notification terminal response and send
    the command to UIM server.

  gstk_launch_browser_cnf
    This function packs the launch browser terminal response and send
    the command to UIM server.

  gstk_more_time_cnf
    This function packs the more time terminal response and send
    the command to UIM server.

  gstk_play_tone_cnf
    This function packs the play tone terminal response and send
    the command to UIM server.

  gstk_polling_cnf
    This function packs the polling terminal response and send
    the command to UIM server.

  gstk_provide_local_info_cnf
    This function packs the provide local info terminal response and send
    the command to UIM server.

  gstk_refresh_cnf
    This function packs the Refresh terminal response and sends
    the command to UIM server.

  gstk_select_item_cnf
    This function packs the select item terminal response and sends
    the command to UIM server.

  gstk_send_dtmf_cnf
    This function packs the send dtmf terminal response and sends
    the command to UIM server.

  gstk_send_sms_cnf
    This function packs the send sms terminal response and sends
    the command to UIM server.

  gstk_send_ss_cnf
    This function packs the send ss terminal response and sends
    the command to UIM server.

  gstk_send_ussd_cnf
    This function packs the send ussd terminal response and sends
    the command to UIM server.

  gstk_setup_mo_call_cnf
    This function packs the setup call terminal response and sends
    the command to UIM server.

  gstk_setup_evt_list_cnf
    This function packs the setup event list terminal response and sends
    the command to UIM server.

  gstk_setup_idle_mode_text_cnf
    This function packs the setup idle mode text terminal response and sends
    the command to UIM server.

  gstk_setup_menu_cnf
    This function packs the setup menu terminal response and sends
    the command to UIM server.

  gstk_timer_management_cnf
    This function packs the timer management terminal response and send
    the command to UIM server.

  gstk_open_ch_cnf
    This function packs the open channel terminal response and sends
    the command to UIM server.

  gstk_close_ch_cnf
    This function packs the close channel terminal response and sends
    the command to UIM server.

  gstk_send_data_cnf
    This function packs the send data terminal response and sends
    the command to UIM server.

  gstk_receive_data_cnf
    This function packs the receive data terminal response and sends
    the command to UIM server.

  gstk_get_ch_status_cnf
    This function packs the get channel status terminal response and sends
    the command to UIM server.

  gstk_process_term_rsp_timer_expire
    This function process timer expiration while waiting for the terminal
    for a proactive command already sent to the client.

  gstk_activate_cnf
    This function packs the activate terminal response and sends
    the command to UIM server

INITIALIZATION AND SEQUENCING REQUIREMENTS


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003,2008 - 2012 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_terminal_rsp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/14   shr     Add Support for NV Refresh
05/28/14   gm      Support of CSG Rel-11 feature
10/29/13   jd      PLI date and time handling w.r.t. TR timer expiry fixed
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/20/11   nb      GSDI Cleanup
10/31/11   sg      Class L and M support
10/18/11   av      Removed features that are always OFF
02/25/11   js      Fixed compilation error
01/16/08   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
08/21/08   sk      Added function prototype
08/08/08   xz      Added support for raw terminal response
10/08/06   sk      Added support for unknown unsupported proactive commands.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
05/23/06   tml     lint
09/01/05   sk      Added additional result field to
                   gstk_util_error_check_terminal_rsp_other_
                   info() to support extra information for
                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR
11/23/04  sk/tml   Added SAT/USAT data support
08/03/04   tml     Changed to use additional info ptr instead of array
05/16/03   tml     linted
02/26/03  tml/jar  Added gstk_refresh_cnf
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk.h"
#include "uim.h"
#include "gstk_exp_v.h"

/*===========================================================================

                     GLOBAL VARIABLES

===========================================================================*/
extern gstk_setup_evt_list_cnf_type gstk_setup_evt_list_cnf_cache;

/*===========================================================================
FUNCTION gstk_populate_provide_local_info_terminal_rsp_cnf

DESCRIPTION
  This function build terminal response confirm type for PLI proactive command,
  which will be put onto the GSTK command task later

PARAMETERS
  task_cmd_ptr:      [Input/Output] Pointer to the task_cmd_type to which the
                                    terminal response is going to be populated
  cmd_detail_ref_id: [Input]        Specifies the reference ID for command
                                    details of the proactive command that the
                                    client is sending a terminal response to.
                                    The reference id value is sent to the client
                                    earlier in the structure of the proactive
                                    command request, inside the
                                    gstk_exp_hdr_type
  cmd_num:           [Input]        This is the same value as that was being
                                    passed to the client in the corresponding
                                    proactive command request
  general_result:    [Input]        Specifies the result of the proactive command
                                    operation
  additional_
  result_ptr:        [Input]        Specifies the optional additional result for
                                    the proactive command, the length field in
                                    the gstk_additonal_info_type specifies number
                                    of additional_info in the structure
  other_info_ptr:    [Input]        Pointer to the gstk_local_info_rsp_type
  user_data:         [Input]        client user data

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
gstk_status_enum_type gstk_populate_provide_local_info_terminal_rsp_cnf(
  gstk_task_cmd_type                  *task_cmd_ptr,
  uint32                               cmd_details_ref_id,
  uint8                                cmd_num,
  gstk_general_result_enum_type        general_result,
  const gstk_additional_info_ptr_type *additional_result_ptr,
  const gstk_local_info_rsp_type      *local_info_rsp_ptr,
  uint32                               user_data
);

/*===========================================================================
FUNCTION gstk_populate_terminal_response_cnf

DESCRIPTION
  This function build terminal response confirm type which will be put onto
  the GSTK command task later

PARAMETERS
  command: [Input/Output] Pointer to the task_cmd_type to which the terminal
                          response is going to be populated
  terminal_response_enum: [Input] Response type for thie terminal response
  general_result: [Input] General response
  additional_result: [Input] Additional info for response
  other_info: [Input] extra info for some terminal response type
  cmd_num: [Input] command number of the proactive command that this terminal
                   response is responding
  cmd_details_ref_id: [Input] reference id in the command details table
  user_data: [Input] client user data

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
extern gstk_status_enum_type gstk_populate_terminal_response_cnf(
        gstk_task_cmd_type                       *command,
        gstk_cmd_enum_type                       terminal_response_enum,
        gstk_general_result_enum_type            general_result,
        const gstk_additional_info_ptr_type      *additional_result,
        const gstk_terminal_rsp_extra_param_type *other_info,
        uint8                                    cmd_num,
        uint32                                   cmd_details_ref_id,
        uint32                                   user_data);

/*===========================================================================
FUNCTION gstk_process_terminal_response

DESCRIPTION

  This function switches on the command id and call the corresponding
  terminal response cnf function so that GSTK can build the corrresponding
  terminal response tlvs and send it to UIM Server

PARAMETERS
  cmd: [Input] Pointer to the terminal response confirm types to be processed

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
extern gstk_status_enum_type gstk_process_terminal_response(
    gstk_cmd_type* cmd );

/*===========================================================================
FUNCTION gstk_uim_terminal_response_report

DESCRIPTION

  This is the callback function for terminal response response from UIM.
  UIM will call this function when it receives terminal response response
  from the Card.
  Based on the return status of the terminal response response, this function
  will decide whether an end proactive session request should be sent
  to the client or not

PARAMETERS
  report: [Input] Pointer to the UIM terminal response report

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
extern void gstk_uim_terminal_response_report(uim_rpt_type *report);

/*===========================================================================
FUNCTION gstk_display_text_cnf

DESCRIPTION
  This function packs the Display Text terminal response and send
  the command to UIM server.

PARAMETERS
  display_text_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_display_text_cnf (
         gstk_display_text_cnf_type* display_text_cnf );

/*===========================================================================
FUNCTION gstk_get_input_cnf

DESCRIPTION
  This function packs the get input terminal response and send
  the command to UIM server.

PARAMETERS
  get_input_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_get_input_cnf(
       gstk_get_input_cnf_type* get_input_cnf);

/*===========================================================================
FUNCTION gstk_get_inkey_cnf

DESCRIPTION
  This function packs the get inkey terminal response and send
  the command to UIM server.

PARAMETERS
  get_inkey_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_get_inkey_cnf(
       gstk_get_inkey_cnf_type* get_inkey_cnf);

/*===========================================================================
FUNCTION gstk_play_tone_cnf

DESCRIPTION
  This function packs the play tone terminal response and send
  the command to UIM server.

PARAMETERS
  play_tone_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_play_tone_cnf(
       gstk_play_tone_cnf_type* play_tone_cnf);

/*===========================================================================
FUNCTION gstk_consolidate_setup_evt_list_cnf

DESCRIPTION
  This function waits for all clients to send terminal responses of setup
  event list command. After receiving all responses, it sends the final
  response to the card

  This function will wait for all the clients to send SETUP EVT LIST terminal
  response, consolidate all responses and send the final response to the card.
  However, if any client sends error terminal response, this function will
  send this error to the card without further waiting. If both of these
  features are not defined, the function will simply send the terminal
  response right away to the card.

PARAMETERS
  setup_evt_list_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_consolidate_setup_evt_list_cnf(
       gstk_setup_evt_list_cnf_type* setup_evt_list_cnf);

/*===========================================================================
FUNCTION gstk_convert_raw_setup_evt_list_cnf

DESCRIPTION
  This function converts setup evt list terminal response from raw format
  to GSTK format

PARAMETERS
  raw_cnf_ptr: [Input] Pointer to message that is required to
                                  be processed and sent to UIM

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
gstk_status_enum_type gstk_convert_raw_setup_evt_list_cnf(
  const gstk_client_raw_term_resp_type *raw_cnf_ptr);

/*===========================================================================
FUNCTION gstk_provide_local_info_cnf

DESCRIPTION
  This function packs the provide local info terminal response and send
  the command to UIM server.

PARAMETERS
  provide_local_info_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_provide_local_info_cnf(
       gstk_provide_local_info_cnf_type* provide_local_info_cnf);

/*===========================================================================
FUNCTION gstk_setup_mo_call_cnf

DESCRIPTION
  This function packs the Set Up Call terminal response and send
  the command to UIM server.

PARAMETERS
  setup_call_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_setup_mo_call_cnf (
         gstk_setup_call_cnf_type* setup_call_cnf );

/*===========================================================================
FUNCTION gstk_setup_idle_mode_text_cnf

DESCRIPTION
  This function packs the Set up Idle mode Text terminal response and send
  the command to UIM server.

PARAMETERS
  idle_text_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_setup_idle_mode_text_cnf (
         gstk_setup_idle_text_cnf_type* idle_text_cnf );

/*===========================================================================
FUNCTION gstk_launch_browser_cnf

DESCRIPTION
  This function packs the Launch Browser terminal response and send
  the command to UIM server.

PARAMETERS
  launch_browser_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_launch_browser_cnf (
         gstk_launch_browser_cnf_type* launch_browser_cnf );

/*===========================================================================
FUNCTION gstk_send_ss_cnf

DESCRIPTION
  This function packs the Send SS terminal response and send
  the command to UIM server.

PARAMETERS
  send_ss_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_send_ss_cnf (
         gstk_send_ss_cnf_type* send_ss_cnf );

/*===========================================================================
FUNCTION gstk_send_ussd_cnf

DESCRIPTION
  This function packs the Send USSD terminal response and send
  the command to UIM server.

PARAMETERS
  send_ussd_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_send_ussd_cnf (
         gstk_send_ussd_cnf_type* send_ussd_cnf );

/*===========================================================================
FUNCTION gstk_send_sms_cnf

DESCRIPTION
  This function packs the Send SMS terminal response and send
  the command to UIM server.

PARAMETERS
  send_sms_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_send_sms_cnf (
         gstk_send_sms_cnf_type* send_sms_cnf );

/*===========================================================================
FUNCTION gstk_polling_cnf

DESCRIPTION
  This function packs the poll interval terminal response and send
  the command to UIM server.

PARAMETERS
  polling_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_polling_cnf(
       gstk_poll_interval_cnf_type* polling_cnf);

/*===========================================================================
FUNCTION gstk_more_time_cnf

DESCRIPTION
  This function packs the more time terminal response and send
  the command to UIM server.

PARAMETERS
  more_time_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_more_time_cnf(
       gstk_more_time_cnf_type* more_time_cnf);

/*===========================================================================
FUNCTION gstk_refresh_cnf

DESCRIPTION
  This function packs the Refresh terminal response and sends
  the command to UIM server.

PARAMETERS
  refresh_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_refresh_cnf (
    gstk_refresh_cnf_type* refresh_cnf
);

/*===========================================================================
FUNCTION gstk_setup_menu_cnf

DESCRIPTION
  This function packs the Setup Menu terminal response and sends
  the command to UIM server.

PARAMETERS
  setup_menu_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_setup_menu_cnf (
    gstk_setup_menu_cnf_type* setup_menu_cnf
);

/*===========================================================================
FUNCTION gstk_select_item_cnf

DESCRIPTION
  This function packs the Setup Menu terminal response and sends
  the command to UIM server.

PARAMETERS
  select_item_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_select_item_cnf (
    gstk_select_item_cnf_type* select_item_cnf
);

/*===========================================================================
FUNCTION gstk_util_error_check_terminal_rsp

DESCRIPTION
  This function will perform some error checking to ensure that
  the Application sending the Terminal Response has an
  opportunity to resend the Terminal Response incase it is
  invalid

PARAMETERS
  terminal_response_enum: [Input] Used to indicate which Terminal Response
                                   to check.
  general_result:         [Input] Used to check the result of the command
  *other_info             [Input] Union of Proactive Commands
                                  Poll Interval
                                  Get Inkey
                                  Get Input
                                  Select Item
                                  Setup Call
                                  Send SS
                                  Send USSD



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
extern gstk_status_enum_type gstk_util_error_check_terminal_rsp_other_info (
    gstk_cmd_enum_type                           terminal_response_enum,
    gstk_general_result_enum_type                general_result,
    const gstk_terminal_rsp_extra_param_type    *other_info,
    const gstk_additional_info_ptr_type         *additional_result,
    uint8                                       cat_version
);

/*===========================================================================
FUNCTION gstk_timer_management_cnf

DESCRIPTION
  This function packs the timer management terminal response and send
  the command to UIM server.

PARAMETERS
  timer_manage_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_timer_management_cnf(
       gstk_timer_manage_cnf_type* timer_manage_cnf);

/*===========================================================================
FUNCTION gstk_lang_notification_cnf

DESCRIPTION
  This function packs the language notification terminal response and send
  the command to UIM server.

PARAMETERS
  lang_notification_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_lang_notification_cnf(
       gstk_lang_notification_cnf_type* lang_notification_cnf);

/*===========================================================================
FUNCTION gstk_send_dtmf_cnf

DESCRIPTION
  This function packs the Send DTMF terminal response and send
  the command to UIM server.

PARAMETERS
  send_dtmf_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_send_dtmf_cnf (
         gstk_send_dtmf_cnf_type* send_dtmf_cnf );

/*===========================================================================
FUNCTION gstk_open_ch_cnf

DESCRIPTION
  This function packs the open channel terminal response and sends
  the command to UIM server.

PARAMETERS
  open_ch_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_open_ch_cnf (
    gstk_open_ch_cnf_type* open_ch_cnf
);

/*===========================================================================
FUNCTION gstk_close_ch_cnf

DESCRIPTION
  This function packs the close channel terminal response and sends
  the command to UIM server.

PARAMETERS
  close_ch_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_close_ch_cnf (
    gstk_close_ch_cnf_type* close_ch_cnf
);

/*===========================================================================
FUNCTION gstk_send_data_cnf

DESCRIPTION
  This function packs the get channel status terminal response and sends
  the command to UIM server.

PARAMETERS
  send_data_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_send_data_cnf (
    gstk_send_data_cnf_type* send_data_cnf
);

/*===========================================================================
FUNCTION gstk_receive_data_cnf

DESCRIPTION
  This function packs the get channel status terminal response and receives
  the command to UIM server.

PARAMETERS
  receive_data_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_receive_data_cnf (
    gstk_receive_data_cnf_type* receive_data_cnf
);

/*===========================================================================
FUNCTION gstk_get_ch_status_cnf

DESCRIPTION
  This function packs the get channel status terminal response and sends
  the command to UIM server.

PARAMETERS
  get_ch_status_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_get_ch_status_cnf (
    gstk_get_ch_status_cnf_type* get_ch_status_cnf
);

/*===========================================================================
FUNCTION gstk_activate_cnf

DESCRIPTION
  This function packs the activate terminal response and sends
  the command to UIM server.

PARAMETERS
  activate_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
extern gstk_status_enum_type gstk_activate_cnf (
    gstk_activate_cnf_type* activate_cnf
);

/*===========================================================================
FUNCTION gstk_unsupported_unknown_cnf

DESCRIPTION
  This function packs the Terminal response for an unsupported or unknown
  proactive command the command to the card through the UIM server.

PARAMETERS
  gstk_unsupported_unknown_cnf: [Input] Pointer to message that is required to
                                be processed and sent to UIM

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
extern gstk_status_enum_type gstk_unsupported_unknown_cnf (
  gstk_unsupported_proactive_cmd_cnf_type* unsupported_unknown_cnf );

/*===========================================================================
FUNCTION gstk_process_term_rsp_timer_expire

DESCRIPTION
  For each proactive command sent to client, there is a timer associated with
  it. The corresponding terminal response has to be received within this period.
  And if the timer expires before the terminal response is revceived, this function
  will free up the entry in command details reference table and send "No Response"
  terminal response down to UIM server.

PARAMETERS
  ref_id: [Input] The reference id of the entry in command_details_ref_table
                  whose timer expired.

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
extern gstk_status_enum_type gstk_process_term_rsp_timer_expire (
  uint32  ref_id);

/*===========================================================================
FUNCTION gstk_process_raw_terminal_response

DESCRIPTION
  This function packs raw terminal response and sends response to the card
  through the UIM server.

PARAMETERS
  raw_term_resp_ptr: [Input] Pointer to raw terminal response that is required
                             to be processed and sent to UIM

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
gstk_status_enum_type gstk_process_raw_terminal_response (
  gstk_client_raw_term_resp_type   *raw_term_resp_ptr );
#endif /*gstk_terminal_rsp_h */

