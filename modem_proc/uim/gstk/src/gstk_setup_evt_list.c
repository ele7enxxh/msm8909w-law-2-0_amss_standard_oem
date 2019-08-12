/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    G S T K   S E T U P   E V E N T   L I S T


GENERAL DESCRIPTION
  This source file contains functions to process setup event list
  proactive command


FUNCTIONS
  gstk_setup_evt_list_req
    This function parses the Setup event list Proactive Command from the card
    and sends it to the client.

  gstk_setup_evt_list_cnf
    This function packs the setup event list terminal response and sends
    the command to UIM server.

  gstk_setup_evt_list_cnf_cleanup
    This function cleans up any allocated pointer on the stack.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_setup_evt_list.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/12/15   vr      F3 Logging inprovements
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
11/27/14   vr      Cache setup event list after sending TR to setup evt list
10/07/14   vr      Fix SW1 SW2 TR REPORT failure for setup event list
09/09/14   vr      Reduce stack usage
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/31/14   dy      Fixing KW errors
07/11/14   shr     Add Support for NV Refresh
06/02/14   vr      Fix for sending wrong location information in setup evt list
05/28/14   gm      Support of CSG Rel-11 feature
05/06/14   gs      GSTK Featurization enhancements and clean up
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/29/13   vr      Enable feature GSTK_CFG_FEATURE_ENH_RAT_AND_LOC_EVT in
                   recovery
10/04/13   gs      F3 Message reduction
09/13/13   gm      Support for Recovery and hot-swap
07/01/13   bd      Added support for maintaining separate cache for cell
                   information from different RRCs
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/11/13   bd      Support for (e)HRPD access tech
01/09/13   hn      Update correct length for result_additional_info for
                   SETUP EVT LIST cnf
11/29/12   bd      Clean up of redundant members from client table
10/15/12   bd      Support for Closed Subscriber Group feature
05/02/12   dd      Send End of Proactive session for UI related Setup Evt List
03/28/12   av      Replaced feature flags with NVITEM
03/16/12   yt      Remove tmc.h inclusion
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/20/11   nb      GSDI Cleanup
10/31/11   sg      Class L and M support
10/18/11   av      Removed features that are always OFF
09/28/11   xz      Send AcT and Loc envelope when receive SETUP EVT LIST cmd
07/18/11   bd      Fixed incorrect issue
06/06/11   sg      Add support for mandatory TLV for result 0x20
05/20/11   xz      Reg/dereg CM_CALL_EVENT_INCOM event
04/13/11   ea      Add proactive cmd enable / disable support
01/11/11   xz      Don't wait TR if client uses old type of callback
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/02/10   xz      Fix the issue of miscouting expected TRs
03/12/10   bd      Fixed gstk_consolidate_setup_evt_list_cnf to copy necessary
                   variables
03/12/10   sg      Dual Slot and BIP bug fixes
02/15/10   nb      Moved Dual Slot Changes
01/15/10   xz      Fix the issue of freeing mem twice
11/03/09   dd      Fixed KW error
09/26/09   kp      ZI memory reduction changes
09/08/09   rn      Fixed klocwork errors
07/28/09   bd      Fix lint issues
05/07/09   sg      Fixed Lint Errors
04/07/09   yb      Added a condition to check invalid TLV while sending
                   terminal response for Setup event list
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
03/18/09   yb      Fixed condition for checking length while sending terminal
                   response for Setup event list
01/16/09   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
10/24/08   xz      Fix an issue of not send SETUP EVT LIST command to client
                   in GSTK format when registered via new API.
09/14/08   sk      Fixed lint errors
09/08/08   sk      Fixed indentation
08/21/08   sk      Added GSTK enhanced registration support
08/08/08   xz      Add support for raw set up event list command
11/13/07   tml     pipe the setup event list commands to client and wait for
                   responses from client before sending TR
04/23/07   nk      fixed possible buffer overflow
04/18/07   sk      send notification to clients only if relevant bit in the
                   set up evt list has changed from the last received command.
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mode
04/04/06   sk      Added support for 1X related to location information
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
07/13/05   sk      Added support for Access Technology
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Added SAT/USAT data support
08/13/04   tml     Centralize release memory condition in the request function
08/03/04   tml     Memory leak issue fix
05/26/04   tml     Added More debug messages, added language selection
                   and browser termination, user activity and idle screen
                   user notification supports
04/16/04   tml     linted
05/16/03   tml     linted
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstk_envelope_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

#include "err.h"
#include "string.h"
#include "cm.h"
#include <stringl/stringl.h>
#include "uim_msg.h"

/*===========================================================================

                     GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
FUNCTION gstk_setup_evt_list_req_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  request: [Input] Pointer to gstk_cmd_from_card_type
  cmd_data: [Input] Pointer to cmd_data

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
static void gstk_setup_evt_list_req_cleanup(
  gstk_cmd_from_card_type *request,
  uint8                   *cmd_data)
{
  gstk_util_release_card_cmd_memory(request);
  if (cmd_data != NULL) {
    gstk_free(cmd_data);
  }
} /* gstk_setup_evt_list_req_cleanup */

/*===========================================================================
FUNCTION gstk_setup_evt_list_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  setup_evt_list_cnf: [Input] Pointer to message that is required to
                              be processed and sent to UIM
  STK_response: [Input] Pointer to the STK intermediate message

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
static void gstk_setup_evt_list_cnf_cleanup(
  gstk_setup_evt_list_cnf_type                * setup_evt_list_cnf,
  setup_evt_list_terminal_response_type       * STK_response)
{
  if(STK_response == NULL)
  {
    UIM_MSG_ERR_0(" STK_response ERR:NULL");
    return;
  }

  if(setup_evt_list_cnf == NULL)
  {
    UIM_MSG_ERR_0(" setup_evt_list_cnf ERR:NULL");
    return;
  }
  /* free setup event list cnf and tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)setup_evt_list_cnf);/*lint !e826 area too small */
  if (STK_response->data.result.additional_info != NULL) {
    gstk_free(STK_response->data.result.additional_info);
    STK_response->data.result.additional_info = NULL;
  }

} /* gstk_setup_evt_list_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_send_setup_evt_list_to_client

DESCRIPTION
  This function sends a event in setup event list proactive either in GSTK
  format or raw format based on the type of the client. Also, if it sends
  raw setup event list proactive command to a client, the corresponding
  bit in the flag pointed by cli_rvcd_raw_cmd_flg_ptr will be set.

PARAMETERS
  pro_cmd_ptr:              [Input]        Pointer to setup event list proactive
                                           command
  gstk_msg_ptr:             [Input]        Pointer to gstk event message
  cmd_details_ref_id:       [Input]        The reference id of command details
                                           reference table
  cli_rvcd_raw_cmd_flg_ptr: [Input/Output] Pointer to the flag where the bit will
                                           be set if its corresponding client
                                           received raw proactive command

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
static gstk_status_enum_type gstk_send_setup_evt_list_to_client(
  gstk_proactive_cmd_type *pro_cmd_ptr,
  gstk_cmd_from_card_type *gstk_msg_ptr,
  uint32                  cmd_details_ref_id,
  uint32                  *cli_rcvd_raw_cmd_flg_ptr
)
{
  gstk_status_enum_type                     gstk_status = GSTK_ERROR;
  gstk_client_id_type                       client_id   = 0;
  gstk_client_pro_cmd_data_format_enum_type data_format = GSTK_FORMAT_NONE;
  gstk_client_pro_cmd_reg_data_type         pro_cmd_msg_data;
  gstk_raw_cmd_from_card_type               raw_cmd_fmt;
  uint32                                    cli_bitmask = 1;

  if ( (pro_cmd_ptr == NULL)
    || (gstk_msg_ptr == NULL)
    || (cli_rcvd_raw_cmd_flg_ptr == NULL))
  {
    UIM_MSG_ERR_3("null pointer passed in: 0x%x 0x%x 0x%x!",
                  pro_cmd_ptr, gstk_msg_ptr, cli_rcvd_raw_cmd_flg_ptr);
    return GSTK_NULL_INPUT_PARAM;
  }

  memset(&pro_cmd_msg_data, 0, sizeof(gstk_client_pro_cmd_reg_data_type));
  memset(&raw_cmd_fmt, 0, sizeof(gstk_raw_cmd_from_card_type));

  /* find the client who has registered for this event */
  gstk_status = gstk_util_find_client_with_registered_event(
                   &client_id,
                   gstk_msg_ptr->hdr_cmd.command_id,
                   GSTK_GSTK_FORMAT,
                   GSTK_HANDLE_ALL_FUNC);

  if (gstk_status == GSTK_NO_CLIENT_REG_FOUND)
  {
    gstk_status = gstk_util_find_client_with_registered_event(
                     &client_id,
                     gstk_msg_ptr->hdr_cmd.command_id,
                     GSTK_RAW_FORMAT,
                     GSTK_HANDLE_ALL_FUNC);

    if (gstk_status == GSTK_NO_CLIENT_REG_FOUND)
    {
      UIM_MSG_HIGH_0("no client registered for event");
      return GSTK_CLIENT_NOT_REGISTERED;
    }
    else
    {
      data_format = GSTK_RAW_FORMAT;
    }
  }
  else
  {
    data_format = GSTK_GSTK_FORMAT;
  }

#if (GSTK_MAX_CLIENT > 32)
#error "The max num of GSTK client is more than 32. Need to extend cli_bitmask!"
#endif /* GSTK_MAX_CLIENT > 32 */

  cli_bitmask <<= (client_id - 1);

  if (gstk_shared_data.gstk_client_table[client_id-1].client_cb != NULL)
  {
    if (data_format == GSTK_RAW_FORMAT)
    {
      /* check if raw cmd is already sent */
      if (((*cli_rcvd_raw_cmd_flg_ptr) & cli_bitmask) == 0)
      {
        /* construct raw format message */
        raw_cmd_fmt.raw_hdr.cmd_ref_id = cmd_details_ref_id;
        raw_cmd_fmt.raw_hdr.user_data = pro_cmd_ptr->message_header.user_data;
        raw_cmd_fmt.raw_hdr.sim_slot_id = pro_cmd_ptr->message_header.sim_slot_id;
        raw_cmd_fmt.payload.data_buffer_ptr = pro_cmd_ptr->data;
        raw_cmd_fmt.payload.data_len = uint32toint32(pro_cmd_ptr->length);
        pro_cmd_msg_data.data_format_type = GSTK_RAW_FORMAT;
        pro_cmd_msg_data.pro_cmd_reg_data.raw_req_rsp_ptr = &raw_cmd_fmt;

        /* send to client and set the flag */
        gstk_shared_data.gstk_client_table[client_id-1].client_cb(&pro_cmd_msg_data);
        gstk_shared_data.gstk_client_table[client_id -1].send_setup_event_sw1_sw2 = FALSE;

        *cli_rcvd_raw_cmd_flg_ptr |= cli_bitmask;
        if(gstk_shared_data.gstk_client_table[client_id-1].client_type == GSTK_UI_TYPE)
        {
          gstk_curr_inst_ptr->gstk_is_curr_session_ui_related = TRUE;
        }
        /* increment the number of client responses needed before sending TR */
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].pending_responses++;
      }
    }
    else if (data_format == GSTK_GSTK_FORMAT)
    {
      pro_cmd_msg_data.data_format_type = GSTK_GSTK_FORMAT;
      pro_cmd_msg_data.pro_cmd_reg_data.gstk_req_rsp_ptr = gstk_msg_ptr;
      /* send to client and set the flag */
      gstk_shared_data.gstk_client_table[client_id-1].client_cb(&pro_cmd_msg_data);
      if(gstk_shared_data.gstk_client_table[client_id-1].client_type == GSTK_UI_TYPE)
      {
        gstk_curr_inst_ptr->gstk_is_curr_session_ui_related = TRUE;
      }
      /* increment the number of client responses needed before sending TR */
      command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].pending_responses++;
    }
    else
    {
      UIM_MSG_ERR_2("Failed to dispatch evt 0x%x! Invalid data format type 0x%x!",
                    gstk_msg_ptr->hdr_cmd.command_id, data_format);
      return GSTK_ERROR;
    }
  }
  else
  {
    UIM_MSG_ERR_1("evt cb ptr is null for cli 0x%x", client_id);
    return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_send_setup_evt_list_to_client */

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
  cmd_ptr: [Input] Pointer to message parsed and sent to clients

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
gstk_status_enum_type gstk_setup_evt_list_req (gstk_proactive_cmd_type* cmd_ptr)
{
  uint8                              *cmd_data_ptr        = NULL;
  int32                               cmd_data_len        = 0;
  uint32                              cmd_details_ref_id  = GSTK_MAX_CLIENT;
  int32                               curr_offset         = 0;
  gstk_status_enum_type               gstk_status         = GSTK_SUCCESS;
  boolean                             error_already_raised= TRUE;
  int32                               new_offset          = 0;
  gstk_additional_info_type           additional_info;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param;
  boolean                             more_tlv            = FALSE;
  uint8                               new_event_list_mask[GSTK_MAX_EVENT_LIST_LEN];
  gstk_general_result_enum_type       general_result      = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
#ifndef TEST_FRAMEWORK
  cm_client_status_e_type             cm_reg_return_status= CM_CLIENT_ERR_NONE;
#endif /*!TEST_FRAMEWORK*/
  boolean                             cmd_enable          = TRUE;
#if (GSTK_MAX_CLIENT <= 32)
  /* if the raw setup event list proactive command is sent to client,
   * the corresponding bit will be set
   */
  uint32                              cli_rcvd_raw_cmd_flg = 0;
#endif /* GSTK_MAX_CLIENT <= 32 */
  gstk_nv_items_data_type             nv_data;
  gstk_cmd_from_card_type            *request_ptr          = NULL;

  UIM_MSG_HIGH_0(" IN GSTK_SETUP_EVT_LIST_REQ ");

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  ++ gstk_curr_inst_ptr->setup_evt_list_cnt;

  memset(&additional_info, 0, sizeof(gstk_additional_info_type));
  memset(&dummy_extra_param, 0, sizeof(gstk_terminal_rsp_extra_param_type));
  gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type *)&gstk_curr_inst_ptr->setup_evt_list_cnf_cache);
  memset(&gstk_curr_inst_ptr->setup_evt_list_cnf_cache,
         0x00,
         sizeof(gstk_curr_inst_ptr->setup_evt_list_cnf_cache));

  /* initialize additional_info's length to 0 */
  additional_info.length = 0;

  /* initialize dummy_extra_param */
  dummy_extra_param.present = FALSE;

  /* initialize the new event list to 0 */
  memset(new_event_list_mask, 0, GSTK_MAX_EVENT_LIST_LEN);
  (void)secure_memset(gstk_curr_inst_ptr->gstk_temp_evt_list, 0, GSTK_MAX_EVENT_LIST_LEN);

  /* reset error_already_raise to FALSE since gstk_validate_proactive_SIM_command returns success */
  error_already_raised = FALSE;
  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data_ptr, and specifies the length of
  ** the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)cmd_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  if((gstk_status == GSTK_SUCCESS) && (cmd_data_ptr != NULL))
  {

    /* parse the event list tlv */
    gstk_status = gstk_parser_evt_list_tlv( cmd_data_ptr,
                                            cmd_data_len,
                                            curr_offset,
                                            new_event_list_mask, /* pointer to the evt_list array */
                                            &new_offset,
                                            &more_tlv,
                                            GSTK_MANDATORY,
                                            &cmd_details_ref_id);
    if (more_tlv || gstk_status != GSTK_SUCCESS ) { /* should not have any more tlvs */
      error_already_raised = TRUE;
    } /* more tlv */
  } /* GSTK_SUCCESS from preprocess_command */
  else {
    error_already_raised = TRUE;
  }

  /* Check if Event list Request have been disabled. */
  if( gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_EVT_LST_STATUS,&cmd_enable) == GSTK_SUCCESS)
  {
    if(!cmd_enable)
    {
      /* If so, then deregister all notifications to client by cleanring the new event list mask */
      memset(new_event_list_mask, 0, GSTK_MAX_EVENT_LIST_LEN);
      UIM_MSG_HIGH_0("... Setup Event List disabled. Deregister all event list requuest.");
    }
  }

  /* No error has been encountered */
  if(!error_already_raised) {
    /* update the static event list variable */
    #ifndef TEST_FRAMEWORK
    /* new_event_list_mask will contain the current set up evt list received from the card */
    /* gstk_curr_inst_ptr->gstk_evt_list will contain the last set up evt list received from the card for which
       the ME had sent a TR of success */
    /* (de)register with the corresponding modules */
    if ((new_event_list_mask[GSTK_MT_CALL] & 0x01) == 0x01) { /* mt call */
      UIM_MSG_HIGH_0("... Register MT");
      /* register for MT call event */
      cm_reg_return_status = cm_mm_client_call_reg(
                          gstk_shared_data.gstk_client_id_from_cm,     /* pointer to the client struct */
                          gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                          CM_CLIENT_EVENT_REG,        /* event registration */
                          CM_CALL_EVENT_SETUP_IND,    /* from this call event */
                          CM_CALL_EVENT_SETUP_IND,    /* to this call event */
                          NULL );                     /* pointer to a cb function for errors */
      if (cm_reg_return_status != CM_CLIENT_OK) {
        gstk_status = GSTK_ERROR;
      }
      cm_reg_return_status = cm_mm_client_call_reg(
                          gstk_shared_data.gstk_client_id_from_cm,     /* pointer to the client struct */
                          gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                          CM_CLIENT_EVENT_REG,        /* event registration */
                          CM_CALL_EVENT_INCOM,        /* from this call event */
                          CM_CALL_EVENT_INCOM,        /* to this call event */
                          NULL );                     /* pointer to a cb function for errors */
      if (cm_reg_return_status != CM_CLIENT_OK) {
        gstk_status = GSTK_ERROR;
      }
    }
    else {
      UIM_MSG_HIGH_0("... Deregister MT");
      /* deregister for MT call event */
      cm_reg_return_status = cm_mm_client_call_reg(
                          gstk_shared_data.gstk_client_id_from_cm,     /* pointer to the client struct */
                          gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                          CM_CLIENT_EVENT_DEREG,      /* event deregistration */
                          CM_CALL_EVENT_INCOM,        /* from this call event */
                          CM_CALL_EVENT_INCOM,        /* to this call event */
                          NULL );                     /* pointer to a cb function for errors */
      if (cm_reg_return_status != CM_CLIENT_OK) {
        gstk_status = GSTK_ERROR;
      }
      cm_reg_return_status = cm_mm_client_call_reg(
                          gstk_shared_data.gstk_client_id_from_cm,     /* pointer to the client struct */
                          gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                          CM_CLIENT_EVENT_DEREG,      /* event deregistration */
                          CM_CALL_EVENT_SETUP_IND,    /* from this call event */
                          CM_CALL_EVENT_SETUP_IND,    /* to this call event */
                          NULL );                     /* pointer to a cb function for errors */
      if (cm_reg_return_status != CM_CLIENT_OK) {
        gstk_status = GSTK_ERROR;
      }
    }
    #endif /* !TEST_FRAMEWORK */
    gstk_util_dump_byte_array("new_event_list_mask", new_event_list_mask, GSTK_MAX_EVENT_LIST_LEN);

    request_ptr = (gstk_cmd_from_card_type *)GSTK_CALLOC(sizeof(gstk_cmd_from_card_type));
    if(request_ptr == NULL)
    {
      gstk_status = GSTK_MEMORY_ERROR;
    }

    if (gstk_status == GSTK_SUCCESS ) {
      /* User activity */
      /* build Message to clients */
      request_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_USER_ACT_NOTIFY;
      request_ptr->hdr_cmd.sim_slot_id = cmd_ptr->message_header.sim_slot_id;
      request_ptr->hdr_cmd.cmd_detail_reference = cmd_details_ref_id;
      request_ptr->hdr_cmd.command_number =
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number;

      if ((new_event_list_mask[GSTK_USER_ACTIVITY] & 0x01) == 0x01){ /* user act */
        /* nothing */
        request_ptr->cmd.evt_notify_req.require_notification = TRUE;
        UIM_MSG_HIGH_0("... Register User act");
      }
      else {
        request_ptr->cmd.evt_notify_req.require_notification = FALSE;
        /* nothing */
      }
      /* Send command to clients only if any updates were received for this bit */
      if(new_event_list_mask[GSTK_USER_ACTIVITY] !=
         gstk_curr_inst_ptr->gstk_evt_list[GSTK_USER_ACTIVITY])
      {
        gstk_status = gstk_send_setup_evt_list_to_client(
                        cmd_ptr,
                        request_ptr,
                        cmd_details_ref_id,
                        &cli_rcvd_raw_cmd_flg);
      }

      if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
        /* reset gstk_status */
        gstk_status = GSTK_SUCCESS;
        /*idle screen */
        /* build Message to clients */
        request_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY;

        if ((new_event_list_mask[GSTK_IDLE_SCRN_AVAILABLE] & 0x01) == 0x01) { /* idle scrn */
          request_ptr->cmd.evt_notify_req.require_notification = TRUE;
          UIM_MSG_HIGH_0("... Register idle");
        }
        else {
          request_ptr->cmd.evt_notify_req.require_notification = FALSE;
        }
        /* Send command to clients only if any updates were received for this bit */
        if(new_event_list_mask[GSTK_IDLE_SCRN_AVAILABLE] !=
           gstk_curr_inst_ptr->gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE])
        {
          gstk_status = gstk_send_setup_evt_list_to_client(
                          cmd_ptr,
                          request_ptr,
                          cmd_details_ref_id,
                          &cli_rcvd_raw_cmd_flg);
        }

        if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
          /* reset gstk_status */
          gstk_status = GSTK_SUCCESS;
          /* language selection */
          /* build Message to clients */
          request_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_LANG_SEL_NOTIFY;

          if ((new_event_list_mask[GSTK_LANGUAGE_SELECTION] & 0x01) == 0x01) { /* lang sel */
            request_ptr->cmd.evt_notify_req.require_notification = TRUE;
            UIM_MSG_HIGH_0("... Register Language Selection");
          }
          else {
            request_ptr->cmd.evt_notify_req.require_notification = FALSE;
          }
          /* Send command to clients only if any updates were received for this bit */
          if(new_event_list_mask[GSTK_LANGUAGE_SELECTION] !=
             gstk_curr_inst_ptr->gstk_evt_list[GSTK_LANGUAGE_SELECTION])
          {
            gstk_status = gstk_send_setup_evt_list_to_client(
                            cmd_ptr,
                            request_ptr,
                            cmd_details_ref_id,
                            &cli_rcvd_raw_cmd_flg);
          }

          if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
            /* reset gstk_status */
            gstk_status = GSTK_SUCCESS;
            /* browser termination */
            /* build Message to clients */
            request_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY;
            if ((new_event_list_mask[GSTK_BROWSER_TERMINATION] & 0x01) == 0x01) { /* browser term */
              request_ptr->cmd.evt_notify_req.require_notification = TRUE;
              UIM_MSG_HIGH_0("... Register Browser Termination");
            }
            else {
              request_ptr->cmd.evt_notify_req.require_notification = FALSE;
            }
            /* Send command to clients only if any updates were received for this bit */
            if(new_event_list_mask[GSTK_BROWSER_TERMINATION] !=
               gstk_curr_inst_ptr->gstk_evt_list[GSTK_BROWSER_TERMINATION])
            {
              gstk_status = gstk_send_setup_evt_list_to_client(
                              cmd_ptr,
                              request_ptr,
                              cmd_details_ref_id,
                              &cli_rcvd_raw_cmd_flg);
            }

            if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
              /* reset gstk_status */
              gstk_status = GSTK_SUCCESS;

              /* future */
              if ((new_event_list_mask[GSTK_CARD_READER_STATUS] & 0x01) == 0x01) { /* card reader */
              }
              else {
              }

              /* data available */
              /* build Message to clients */
              request_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY;
              if ((new_event_list_mask[GSTK_DATA_AVAILABLE] & 0x01) == 0x01) { /* data avail */
                request_ptr->cmd.evt_notify_req.require_notification = TRUE;
                UIM_MSG_HIGH_0("... Register Data Available");
              }
              else {
                request_ptr->cmd.evt_notify_req.require_notification = FALSE;
              }
              /* Send command to clients only if any updates were received for this bit */
              if(new_event_list_mask[GSTK_DATA_AVAILABLE] !=
                 gstk_curr_inst_ptr->gstk_evt_list[GSTK_DATA_AVAILABLE])
              {
                gstk_status = gstk_util_send_message_to_clients(request_ptr);
              }

              if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
                /* reset gstk_status */
                gstk_status = GSTK_SUCCESS;

                /* channel status */
                /* build Message to clients */
                request_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_CH_STATUS_NOTIFY;
                if ((new_event_list_mask[GSTK_CHANNEL_STATUS] & 0x01) == 0x01) { /* data avail */
                  request_ptr->cmd.evt_notify_req.require_notification = TRUE;
                  UIM_MSG_HIGH_0("... Register Channel Status");
                }
                else {
                  request_ptr->cmd.evt_notify_req.require_notification = FALSE;
                }
                /* Send command to clients only if any updates were received for this bit */
                if(new_event_list_mask[GSTK_CHANNEL_STATUS] !=
                   gstk_curr_inst_ptr->gstk_evt_list[GSTK_CHANNEL_STATUS])
                {
                  gstk_status = gstk_util_send_message_to_clients(request_ptr);
                }
                if (nv_data.cat_version >= GSTK_CFG_CAT_VER6
                    &&
                    ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) )
                {
                  gstk_status = GSTK_SUCCESS;
                  if((gstk_curr_inst_ptr->nw_search_mode == GSTK_NW_SEARCH_MODE_MAX) &&
                     (new_event_list_mask[GSTK_NW_SEARCH_MODE] == 0x01))
                  {
                    cm_ph_cmd_get_ph_info(NULL,
                                          0,
                                          gstk_shared_data.gstk_client_id_from_cm);
                  }
                } /* channel status */

                if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
                  /* reset gstk_status */
                  gstk_status = GSTK_SUCCESS;

                  /* HCI Connectivity status */
                  /* build Message to clients */
                  request_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY;
                  if ((new_event_list_mask[GSTK_HCI_CONNECTIVITY] & 0x01) == 0x01) { /* hci conn */
                    request_ptr->cmd.evt_notify_req.require_notification = TRUE;
                    UIM_MSG_HIGH_0("... Register HCI Connectivity");
                  }
                  else {
                    request_ptr->cmd.evt_notify_req.require_notification = FALSE;
                  }
                  /* Send command to clients only if any updates were received for this bit */
                  if(new_event_list_mask[GSTK_HCI_CONNECTIVITY] !=
                     gstk_curr_inst_ptr->gstk_evt_list[GSTK_HCI_CONNECTIVITY])
                  {
                    gstk_status = gstk_send_setup_evt_list_to_client(
                                    cmd_ptr,
                                    request_ptr,
                                    cmd_details_ref_id,
                                    &cli_rcvd_raw_cmd_flg);
                  }
                } /* HCI Connectivity */
              } /* data available */
            } /* gstk_succses_from browser term (de)registration */
          } /* gstk_success from lang sel (de)registration */
        } /* gstk_success from idle screen (de)registration */
      } /* gstk_success from user activity (de)registration */
    } /* gstk_success from MT (de)registration */

    if(gstk_status != GSTK_SUCCESS)
    {
      /* Send Terminal Response to Card */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      UIM_MSG_HIGH_0("SETUP EVT LIST TLV parsing");

      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    }
    else
    {
      UIM_MSG_HIGH_0("Setup Evt List Success");
      /* Update gstk_curr_inst_ptr->gstk_temp_evt_list with the newly received setup evt list
         from the card this will be used to update gstk_evt_list after sending TR to the card */
      (void)memscpy(gstk_curr_inst_ptr->gstk_temp_evt_list,
             sizeof(gstk_curr_inst_ptr->gstk_temp_evt_list),
             new_event_list_mask,
             GSTK_MAX_EVENT_LIST_LEN);
      /* Send good Terminal Response to the Card */
      general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    }
  }
  else { /* parsing error */

    UIM_MSG_ERR_0("SETUP EVT LIST TLV parsing err");
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
  } /* if error_already_raised */

  /* If Setup Event List was disabled return GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES in TR */
  if(!cmd_enable)
  {
    general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
  }

  /* send the Terminal Response */
  gstk_status = gstk_send_terminal_response(
                  gstk_curr_inst_ptr->client_id, /* client_id */
                  0,              /* user_data */
                  cmd_details_ref_id, /* command_details_ref */
                  command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,
                  /* command_num */
                  GSTK_SETUP_EVENT_LIST_CNF, /* command response */
                  general_result, /* general_result */
                  &additional_info, /* additional result */
                  &dummy_extra_param /* extra param */ );
  /* no need to release memory allocation, but still call the function */
  gstk_setup_evt_list_req_cleanup(request_ptr, cmd_data_ptr);

  gstk_free(request_ptr);
  request_ptr = NULL;
  if(gstk_status != GSTK_SUCCESS) {
    UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
  }
  return gstk_status;
} /* gstk_setup_evt_list_req */

/*===========================================================================
FUNCTION gstk_send_setup_evt_list_cnf

DESCRIPTION
  This function packs the setup event list terminal response and send
  the command to UIM server.

PARAMETERS
  cnf_ptr: [Input] Pointer to message that is required to be sent to UIM

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
static gstk_status_enum_type gstk_send_setup_evt_list_cnf(
  gstk_setup_evt_list_cnf_type* cnf_ptr)
{
  gstk_status_enum_type                 gstk_status        = GSTK_SUCCESS;
  setup_evt_list_terminal_response_type STK_response;
  uim_cmd_type                          *uim_cmd_ptr       = NULL;
  int                                   offset             = 0;
  int                                   i                  = 0;
  boolean                               send_ls_act        = FALSE;

  /* initialize STK_response */
  memset(&STK_response, 0, sizeof(setup_evt_list_terminal_response_type));

  UIM_MSG_HIGH_0("Send setup evt list cnf.");

  if(cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("cnf_ptr ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      cnf_ptr->cmd_details_ref_id,
      cnf_ptr->message_header.command_id,
      cnf_ptr->command_number,
      &STK_response.data.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_evt_list_cnf_cleanup(cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.data.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_evt_list_cnf_cleanup(cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      cnf_ptr->command_result,
      cnf_ptr->result_additional_info,
      &STK_response.data.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_evt_list_cnf_cleanup(cnf_ptr, &STK_response);
    return gstk_status;
  }


  /* total data length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  STK_response.num_bytes = STK_response.data.command_details.length +
                          STK_response.data.device_id.device_tag_length +
                          STK_response.data.result.result_tag_length +
                          6;

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(
          cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = 0;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                   &STK_response.data.command_details,
                                   offset,
                                   (size_t)(STK_response.data.command_details.length +
                                            GSTK_TAG_LENGTH_LEN ),
                                   sizeof(uim_cmd_ptr->terminal_response.data),
                                   sizeof(gstk_command_details_tlv_type)) <
           (size_t)(STK_response.data.command_details.length + GSTK_TAG_LENGTH_LEN ))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy device */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.data.device_id,
                                        offset +=
                                        (int)(STK_response.data.command_details.length +
                                              GSTK_TAG_LENGTH_LEN),
                                        (size_t)(STK_response.data.device_id.device_tag_length +
                                                 GSTK_TAG_LENGTH_LEN ),
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_device_identities_tag_tlv_type)) <
                (size_t)(STK_response.data.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy result */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.data.result,
                                        offset +=
                                        (int)(STK_response.data.device_id.device_tag_length +
                                              GSTK_TAG_LENGTH_LEN),
                                        3,/* result tag +  length + general result */
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_result_tag_tlv_type)) < 3)
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        offset = offset + 3;
        if(gstk_status == GSTK_SUCCESS &&
           STK_response.data.result.result_tag_length > 1 ) { /* has additional info */
          if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                     STK_response.data.result.additional_info,
                                     offset,
                                     (size_t)(STK_response.data.result.result_tag_length - 1),
                                     sizeof(uim_cmd_ptr->terminal_response.data),
                                     ((STK_response.data.result.result_tag_length - 1) *
                                      sizeof(byte))) <
             (size_t)(STK_response.data.result.result_tag_length - 1))
          {
            UIM_MSG_ERR_0("gstk_send_setup_evt_list_cnf(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
          offset = (int)(offset + STK_response.data.result.result_tag_length - 1);
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_util_dump_byte_array("Setup Event List",
                                    uim_cmd_ptr->terminal_response.data,
                                    uim_cmd_ptr->terminal_response.num_bytes);

          gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
          /* If result is success then update the final cache gstk_evt_list with
             the temporarily cached setup evt list */
          if(STK_response.data.result.result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY)
          {
            gstk_memcpy(gstk_curr_inst_ptr->gstk_evt_list,
                   gstk_curr_inst_ptr->gstk_temp_evt_list,
                   sizeof(gstk_curr_inst_ptr->gstk_evt_list),
                   sizeof(gstk_curr_inst_ptr->gstk_evt_list),
                   sizeof(gstk_curr_inst_ptr->gstk_temp_evt_list));
          }
        }
        else
        {
          gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
          uim_cmd_ptr = NULL;
        }
      }
      else {
        UIM_MSG_HIGH_0("Never here");
      }
    }
  } /* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  /* Cleanup temporary cache, no more required */
  (void)secure_memset(gstk_curr_inst_ptr->gstk_temp_evt_list, 0, GSTK_MAX_EVENT_LIST_LEN);

  if((gstk_curr_inst_ptr->recovery_enable_ls_act) ||
     (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_ENH_RAT_AND_LOC_EVT,
        gstk_curr_inst_ptr->slot_id) == TRUE))
  {
    send_ls_act = TRUE;
    gstk_curr_inst_ptr->recovery_enable_ls_act = FALSE;
  }

  if ((gstk_curr_inst_ptr->setup_evt_list_cnt >= 1)
      && (gstk_curr_inst_ptr->gstk_next_curr_location_status != GSTK_NO_SERVICE)
      && (send_ls_act)
     )
  {
    if ((gstk_curr_inst_ptr->gstk_evt_list[GSTK_ACCESS_TECH] & 0x01) == 0x01)
    {
      UIM_MSG_HIGH_2("Access tech already changed! Send env 0x%x 0x%x",
                     gstk_curr_inst_ptr->gstk_curr_rat,
                     gstk_curr_inst_ptr->gstk_next_curr_rat);
      gstk_curr_inst_ptr->gstk_curr_rat = gstk_curr_inst_ptr->gstk_next_curr_rat;
#if defined(FEATURE_GSTK_TAL)
      UIM_MSG_HIGH_2("Calling gstk_tal_notify_rat with client 0x%x and rat 0x%x",
                     GSTK_TAL_LOCAL_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
      (void)gstk_tal_notify_rat(GSTK_TAL_LOCAL_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
#elif defined(FEATURE_GSTK_FUSION_REMOTE)
      UIM_MSG_HIGH_2("Calling gstk_tal_notify_rat with client 0x%x and rat 0x%x",
                     GSTK_TAL_REMOTE_CLIENT,
                     gstk_curr_inst_ptr->gstk_curr_rat);
      (void)gstk_tal_notify_rat(GSTK_TAL_REMOTE_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
#endif /* FEATURE_GSTK_TAL */
      gstk_status = gstk_build_access_technology_envelope(gstk_curr_inst_ptr->gstk_next_curr_rat);
      if(gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Err in building Access Tech Envelope");
      }
    }
    if ((gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01)
    {
      UIM_MSG_HIGH_3("Location status already changed! Send env 0x%x 0x%x 0x%x",
                     gstk_curr_inst_ptr->gstk_curr_location_status,
                     gstk_curr_inst_ptr->gstk_next_curr_location_status,
                     gstk_curr_inst_ptr->gstk_pending_location_evt_envelope);

      if (gstk_curr_inst_ptr->gstk_next_curr_rat != GSTK_ACCESS_TECH_CDMA &&
          gstk_curr_inst_ptr->gstk_next_curr_rat != GSTK_ACCESS_TECH_HRPD &&
          gstk_curr_inst_ptr->gstk_next_curr_rat != GSTK_ACCESS_TECH_EHRPD)
      {
        if (!gstk_curr_inst_ptr->gstk_pending_location_evt_envelope)
        {
          UIM_MSG_HIGH_0("Building 3GPP Location Status Envelope");
          /* there are location info changes that cause the MM to enter idle */
          /* update the gstk static values */
          (void)memscpy(gstk_curr_inst_ptr->gstk_curr_mcc_mnc,
                 sizeof(gstk_curr_inst_ptr->gstk_curr_mcc_mnc),
                 gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc,
                 GSTK_MAX_PLMN_LEN);
          (void)memscpy(gstk_curr_inst_ptr->gstk_curr_lac,
                 sizeof(gstk_curr_inst_ptr->gstk_curr_lac),
                 gstk_curr_inst_ptr->gstk_next_curr_lac,
                 GSTK_MAX_LAC_LEN);
          (void)memscpy(&gstk_curr_inst_ptr->gstk_curr_cell_id,
                 sizeof(gstk_curr_inst_ptr->gstk_curr_cell_id),
                 &gstk_curr_inst_ptr->gstk_next_curr_cell_id,
                 sizeof(gstk_cell_id_type));
          gstk_curr_inst_ptr->gstk_curr_location_status = gstk_curr_inst_ptr->gstk_next_curr_location_status;
          gstk_curr_inst_ptr->gstk_curr_rat = gstk_curr_inst_ptr->gstk_next_curr_rat;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
          switch (gstk_curr_inst_ptr->gstk_curr_rat)
          {
            case GSTK_ACCESS_TECH_UTRAN:
              i = GSTK_RAT_UMTS_IDX;
              break;
            case GSTK_ACCESS_TECH_LTE:
              i = GSTK_RAT_LTE_IDX;
              break;
            default:
              i = GSTK_RAT_MAX_IDX;
          }
          if (i != GSTK_RAT_MAX_IDX)
          {
            UIM_MSG_HIGH_1("is_rrc_cell_id_received[%d] is set to FALSE", i);
            gstk_curr_inst_ptr->is_rrc_cell_id_received[i] = FALSE;
          }
#endif /* FEATURE_WCDMA || FEATURE_LTE */

          UIM_MSG_HIGH_3(" MM->GSTK->Card -- MNC_MCC is 0x%x 0x%x 0x%x",
                         gstk_curr_inst_ptr->gstk_curr_mcc_mnc[0],
                         gstk_curr_inst_ptr->gstk_curr_mcc_mnc[1],
                         gstk_curr_inst_ptr->gstk_curr_mcc_mnc[2]);
          UIM_MSG_HIGH_3("  MM->GSTK->Card -- LAC is 0x%x 0x%x, Location Status is 0x%x",
                         gstk_curr_inst_ptr->gstk_curr_lac[0],
                         gstk_curr_inst_ptr->gstk_curr_lac[1],
                         gstk_curr_inst_ptr->gstk_curr_location_status);
          UIM_MSG_HIGH_3("MM->GSTK->Card -- Cell Len  : 0x%x Cell Id [0][1]: 0x%x 0x%x",
                         gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_len,
                         gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[0],
                         gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[1]);
          UIM_MSG_HIGH_2("MM->GSTK->Card -- Cell Id [2][3]: 0x%x 0x%x",
                         gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[2],
                         gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[3]);

          gstk_status = gstk_build_location_status_envelope(
                          gstk_curr_inst_ptr->gstk_curr_rat,
                          gstk_curr_inst_ptr->gstk_curr_location_status);

          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0("Have problem sending location status envelope");
          }
          else
          {
            gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = TRUE;
          }
        }
      }
      else
      {
        if(!gstk_curr_inst_ptr->gstk_pending_location_evt_envelope)
        {
          /* Send the 3GPP2 location info only when timer is not running */
          if(rex_get_timer(&(gstk_curr_inst_ptr->gstk_cdma_loc_status_timer)) == 0)
          {
            /* Build an envelope to send down the Location change details to the card */
            UIM_MSG_HIGH_0("Building 3GPP2 Location Status Envelope");
            gstk_curr_inst_ptr->gstk_curr_rat = gstk_curr_inst_ptr->gstk_next_curr_rat;
            gstk_curr_inst_ptr->gstk_curr_location_status = gstk_curr_inst_ptr->gstk_next_curr_location_status;
            gstk_status = gstk_build_location_status_envelope(
                            gstk_curr_inst_ptr->gstk_curr_rat,
                            gstk_curr_inst_ptr->gstk_curr_location_status);
            if (gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Have problem sending location status envelope");
            }
            else
            {
              gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = TRUE;
            }
          }
        }
        else
        {
          UIM_MSG_HIGH_0("There is pending location evt env rsp");
        }
      }
    }
  }

  gstk_setup_evt_list_cnf_cleanup(cnf_ptr, &STK_response);
  return gstk_status;
} /*gstk_send_setup_evt_list_cnf*/

/*===========================================================================
FUNCTION gstk_consolidate_setup_evt_list_cnf

DESCRIPTION
  This function waits for all clients to send terminal responses of setup
  event list command. After receiving all responses, it sends the final
  response to the card

  This function will wait for all clients to send SETUP EVT LIST terminal
  response, consolidate all responses and send the final response to the card.
  However, if any client sends error terminal response, this function will
  send this error to the card without further waiting. If both of these
  features are not defined, the function will simply send the terminal response
  right away to the card.

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
gstk_status_enum_type gstk_consolidate_setup_evt_list_cnf(
  gstk_setup_evt_list_cnf_type* setup_evt_list_cnf)
{
  uint32 hdr_size   = sizeof(gstk_hdr_type);
  uint32 cache_size = sizeof(gstk_setup_evt_list_cnf_type);

  UIM_MSG_HIGH_0("IN GSTK gstk_consolidate_setup_evt_list_cnf ");

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(setup_evt_list_cnf == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  if (gstk_curr_inst_ptr->setup_evt_list_cnf_cache.message_header.command_id
      == (uint32)GSTK_CMD_ENUM_NOT_USE)
  {
    /* cache is not initialized, initialize header */
    (void)gstk_memcpy(&gstk_curr_inst_ptr->setup_evt_list_cnf_cache.message_header,
                   &setup_evt_list_cnf->message_header,
                   hdr_size,
                   hdr_size,
                   hdr_size);
  }

  switch(setup_evt_list_cnf->command_result)
  {
  case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
  case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
  case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
    gstk_curr_inst_ptr->setup_evt_list_cnf_cache.command_result =
      setup_evt_list_cnf->command_result;
    gstk_curr_inst_ptr->setup_evt_list_cnf_cache.command_number =
      setup_evt_list_cnf->command_number;
    gstk_curr_inst_ptr->setup_evt_list_cnf_cache.cmd_details_ref_id =
      setup_evt_list_cnf->cmd_details_ref_id;
    break;
  case GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND:
  case GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES:
  case GSTK_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
  case GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
  case GSTK_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
  case GSTK_ERROR_REQUIRED_VALUES_ARE_MISSING:
    /* send error response right away */
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(setup_evt_list_cnf->cmd_details_ref_id)].pending_responses = 0;
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type *)&gstk_curr_inst_ptr->setup_evt_list_cnf_cache);
    (void)gstk_memcpy(&gstk_curr_inst_ptr->setup_evt_list_cnf_cache,
                   setup_evt_list_cnf,
                   cache_size,
                   cache_size,
                   cache_size);
    if (setup_evt_list_cnf->result_additional_info.length > 0)
    {
      gstk_curr_inst_ptr->setup_evt_list_cnf_cache.result_additional_info.additional_info_ptr =
        GSTK_CALLOC(setup_evt_list_cnf->result_additional_info.length);
      if (gstk_curr_inst_ptr->setup_evt_list_cnf_cache.result_additional_info.additional_info_ptr
          != NULL)
      {
        (void)gstk_memcpy(
             gstk_curr_inst_ptr->setup_evt_list_cnf_cache.result_additional_info.additional_info_ptr,
             setup_evt_list_cnf->result_additional_info.additional_info_ptr,
             setup_evt_list_cnf->result_additional_info.length,
             setup_evt_list_cnf->result_additional_info.length,
             setup_evt_list_cnf->result_additional_info.length);
        gstk_curr_inst_ptr->setup_evt_list_cnf_cache.result_additional_info.length =
          setup_evt_list_cnf->result_additional_info.length;
      }
    }
    break;
  default:
    UIM_MSG_ERR_1("unexpected setup evt list result received 0x%x",
                  setup_evt_list_cnf->command_result);
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type *)setup_evt_list_cnf);
    return GSTK_ERROR;
  }

  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type *)setup_evt_list_cnf);

  if (command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(
  	  gstk_curr_inst_ptr->setup_evt_list_cnf_cache.cmd_details_ref_id)].pending_responses != 0)
  {
    UIM_MSG_HIGH_1("wait for 0x%x more term rsp!",
                   command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX( setup_evt_list_cnf->cmd_details_ref_id)].pending_responses);
    return GSTK_SUCCESS;
  }
  return gstk_send_setup_evt_list_cnf(&gstk_curr_inst_ptr->setup_evt_list_cnf_cache);
} /* gstk_consolidate_setup_evt_list_cnf */


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
  const gstk_client_raw_term_resp_type *raw_cnf_ptr)
{
  gstk_setup_evt_list_cnf_type cnf;
  int32                        i                = 0;
  uint32                       j                = 0;
  uint8                        tag              = 0;
  uint32                       len_of_len_field = 0;
  uint8                        len              = 0;

  memset(&cnf, 0x00, sizeof(cnf));

  if (raw_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_1("raw ptr passed in 0x%x", raw_cnf_ptr);
    return GSTK_NULL_INPUT_PARAM;
  }
  j = sizeof(gstk_hdr_type);
  (void)gstk_memcpy(&cnf.message_header,
                 &raw_cnf_ptr->message_header,
                 j,
                 j,
                 j);
  cnf.cmd_details_ref_id = raw_cnf_ptr->cmd_details_ref_id;

  i = 0;
  while (i < raw_cnf_ptr->raw_term_resp.data_len)
  {
    tag = raw_cnf_ptr->raw_term_resp.data_buffer_ptr[i];
    if ((i + 1) < raw_cnf_ptr->raw_term_resp.data_len)
    {
      len_of_len_field = gstk_find_length_of_length_value(
        &raw_cnf_ptr->raw_term_resp.data_buffer_ptr[i + 1]);
    }
    else
    {
      gstk_free(cnf.result_additional_info.additional_info_ptr);
      UIM_MSG_ERR_2("invalid TLV! i = 0x%x, data_len = 0x%x",
                    i, raw_cnf_ptr->raw_term_resp.data_len);
      return GSTK_INVALID_LENGTH;
    }
    if ((i + len_of_len_field) < raw_cnf_ptr->raw_term_resp.data_len)
    {
      len = raw_cnf_ptr->raw_term_resp.data_buffer_ptr[i + len_of_len_field];
    }
    else
    {
      gstk_free(cnf.result_additional_info.additional_info_ptr);
      UIM_MSG_ERR_3("invalid TLV! i = 0x%x, len_of_len_field = 0x%x, data_len = 0x%x",
                    i, len_of_len_field, raw_cnf_ptr->raw_term_resp.data_len);
      return GSTK_INVALID_LENGTH;
    }

    if ((i + len_of_len_field + 1) < raw_cnf_ptr->raw_term_resp.data_len)
    {
      switch (tag & 0x7F)
      {
      case GSTK_COMMAND_DETAILS_TAG:
        cnf.command_number =
          raw_cnf_ptr->raw_term_resp.data_buffer_ptr[
            i + len_of_len_field + 1];
        break;
      case GSTK_RESULT_TAG:
        /* Checking if end addr of the TLV is not out of bounds */
        if((i + len_of_len_field + len + 1) > raw_cnf_ptr->raw_term_resp.data_len)
        {
          gstk_free(cnf.result_additional_info.additional_info_ptr);
          UIM_MSG_ERR_3("invalid Result Tag Length : i = 0x%x, len = 0x%x, data_len = 0x%x",
                        i, len, raw_cnf_ptr->raw_term_resp.data_len);
          return GSTK_INVALID_LENGTH;
        }
        cnf.command_result =
          (gstk_general_result_enum_type)raw_cnf_ptr->raw_term_resp.data_buffer_ptr[i + len_of_len_field + 1];
        if((i + len_of_len_field + 2) < raw_cnf_ptr->raw_term_resp.data_len)
        {
          if ((len - 1) > 0)
          {
            if (cnf.result_additional_info.additional_info_ptr == NULL)
            {
              cnf.result_additional_info.additional_info_ptr = GSTK_CALLOC(len - 1);
            }
            else
            {
              gstk_free(cnf.result_additional_info.additional_info_ptr);
              cnf.result_additional_info.additional_info_ptr = GSTK_CALLOC(len - 1);
            }
            if (cnf.result_additional_info.additional_info_ptr == NULL)
            {
              return GSTK_ERROR;
            }
            (void)gstk_memcpy(cnf.result_additional_info.additional_info_ptr,
                           &raw_cnf_ptr->raw_term_resp.data_buffer_ptr[i + len_of_len_field + 2],
                           len - 1,
                           len - 1,
                           len - 1);
            cnf.result_additional_info.length = len - 1;
          }
          else
          {
            gstk_free(cnf.result_additional_info.additional_info_ptr);
            UIM_MSG_ERR_3("invalid Result Tag Length - i = 0x%x, len = 0x%x, data_len = 0x%x",
                          i, len, raw_cnf_ptr->raw_term_resp.data_len);
            return GSTK_INVALID_LENGTH;
          }
        }
        break;
      default:
        /*nothing to do*/
        break;
      }
    }
    else
    {
      gstk_free(cnf.result_additional_info.additional_info_ptr);
      UIM_MSG_ERR_3("invalid TLV! i = 0x%x, len_of_len_field = 0x%x, data_len = 0x%x",
                    i, len_of_len_field, raw_cnf_ptr->raw_term_resp.data_len);
      return GSTK_INVALID_LENGTH;
    }

    /* pos of next tag */
    i = i + len_of_len_field + len + 1;
  }
  if (i != raw_cnf_ptr->raw_term_resp.data_len)
  {
    gstk_free(cnf.result_additional_info.additional_info_ptr);
    UIM_MSG_ERR_0("invalid TLVs of raw setup evt list cnf!");
    return GSTK_INVALID_LENGTH;
  }

  cnf.message_header.command_id = (uint32)GSTK_SETUP_EVENT_LIST_CNF;
  return gstk_consolidate_setup_evt_list_cnf(&cnf);
} /*gstk_convert_raw_setup_evt_list_cnf*/
