/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E N D   S S


GENERAL DESCRIPTION
  This source file contains functions to process send SS
  proactive command


FUNCTIONS
  gstk_send_ss_req
    This function parses the Send SS Proactive Command from the card
    and sends it to the client.

  gstk_send_ss_cnf
    This function packs the send ss terminal response and sends
    the command to UIM server.

  gstk_send_ss_cnf_cleanup
    This function cleans up any allocated pointer on the stack.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights  Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_send_ss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/16   vr      Fix memory leak when send SS modified to USSD
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/12/15   vr      F3 Logging inprovements
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/25/14   vr      Fix copy of call control alpha in SEND SS failure
06/30/14   gm      Remove unnecessary clear of timer
06/30/14   dy      Fix high compiler warnings
05/06/14   gs      GSTK Featurization enhancements and clean up
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
01/30/14   vr      SIM initiated call control handling
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/13/13   shr     Added support for BIP over WLAN
09/13/13   gm      Support for Recovery and hot-swap
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
03/16/12   yt      Remove tmc.h inclusion
06/06/11   sg      Add support for mandatory TLV for result 0x20
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/22/10   xz      Send error TR if no service or invalid RAT
02/15/10   nb      Moved Dual Slot Changes
09/26/09   kp      ZI memory reduction changes
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/13/04   tml     Centralize release memory condition in the request function
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Memory leak fix
05/26/04   tml     Ensure successfully send TR to card when parsing error
                   occurs.  Added More debug messages
09/12/03   tml     Change source ID from NETWORK to ME
05/16/03   tml     linted
04/29/03   tml     Properly initialize data struct
03/06/03   tml     Updated enum names
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

#include "err.h"
#include "string.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_send_ss_req_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  request_ptr : [Input] Pointer to gstk_cmd_from_card_type
  cmd_data_ptr: [Input] Pointer to cmd_data_ptr

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
static void gstk_send_ss_req_cleanup(
  gstk_cmd_from_card_type *request_ptr,
  uint8                   *cmd_data_ptr)
{
  if(request_ptr == NULL)
  {
    UIM_MSG_ERR_0("send_ss_cnf_ptr ERR:NULL");
    return;
  }

  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_send_ss_req_cleanup */

/*===========================================================================
FUNCTION gstk_send_ss_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  send_ss_cnf_ptr:  [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_response_ptr: [Input] Pointer to the STK intermediate message
  has_cc          : [Input] has call control
  has_ussd_string : [Input] contains ussd string

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
static void gstk_send_ss_cnf_cleanup(
  gstk_send_ss_cnf_type                             *send_ss_cnf_ptr,
  setup_call_send_ss_terminal_response_type         *STK_response_ptr,
  boolean                                           has_cc,
  boolean                                           has_ussd_string)
{
  if(STK_response_ptr == NULL ||
     send_ss_cnf_ptr == NULL)
  {
    return;
  }

  if (has_cc)
  {
    if (STK_response_ptr->cc_req_action.alpha.alpha != NULL)
    {
      gstk_free(STK_response_ptr->cc_req_action.alpha.alpha);
      STK_response_ptr->cc_req_action.alpha.alpha = NULL;
    }
    if (STK_response_ptr->result2_for_cc.additional_info != NULL)
    {
      gstk_free(STK_response_ptr->result2_for_cc.additional_info);
      STK_response_ptr->result2_for_cc.additional_info = NULL;
    }

    if (gstk_shared_data.cc_req_action.call_type == GSTK_USSD &&
        STK_response_ptr->cc_req_action.gstk_address_string_tlv.ussd_string.text != NULL)
    {
      gstk_free(STK_response_ptr->cc_req_action.gstk_address_string_tlv.ussd_string.text);
      STK_response_ptr->cc_req_action.gstk_address_string_tlv.ussd_string.text = NULL;
    }

    if (has_ussd_string)
    {
      if (STK_response_ptr->cc_mod_text2.text != NULL)
      {
        gstk_free(STK_response_ptr->cc_mod_text2.text);
        STK_response_ptr->cc_mod_text2.text = NULL;
      }
    }

    if (gstk_shared_data.cc_req_action.alpha.text != NULL)
    {
      gstk_free(gstk_shared_data.cc_req_action.alpha.text);
      gstk_shared_data.cc_req_action.alpha.text = NULL;
    }
    if (gstk_shared_data.cc_req_action.call_type == GSTK_USSD)
    {
      if (gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text != NULL)
      {
        gstk_free(gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text);
        gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text = NULL;
      }
    }
  }

  if (STK_response_ptr->header.result.additional_info != NULL)
  {
    gstk_free(STK_response_ptr->header.result.additional_info);
  }

  /* free the text string byte pointer from STK_response_ptr and send_ss_cnf_ptr */
  /* STK_response_ptr:
        cc_req_action: ussd string, alpha
        text string 2
        result 2's additional info */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)send_ss_cnf_ptr);/*lint !e826 area too small */
} /* gstk_send_ss_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_send_ss_req

DESCRIPTION
  This function parses the Send SS Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command_ptr: [Input] Pointer to message parsed and sent to clients

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
gstk_status_enum_type gstk_send_ss_req  (gstk_proactive_cmd_type* command_ptr)
{

  uint8                              *cmd_data_ptr         = NULL;
  int32                               cmd_data_len         = 0;
  uint32                              cmd_details_ref_id   = 0;
  int32                               curr_offset          = 0;
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  boolean                             error_already_raised = FALSE;
  int32                               new_offset           = 0;
  gstk_additional_info_type           additional_info      = {0};
  gstk_terminal_rsp_extra_param_type  dummy_extra_param    = {0};
  boolean                             more_tlv             = TRUE;
  uint32                              index                = 0;
  gstk_general_result_enum_type gen_result=GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;

  /* Cannot do {0x00} in declaration because of compilation warning
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  UIM_MSG_HIGH_0(" IN GSTK_SEND_SS_REQ ");

  if(command_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* initialize request */
  memset(&request, 0x00, sizeof(gstk_cmd_from_card_type));

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data_ptr, and specifies the length of
  ** the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  index = GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id);

  if((gstk_status == GSTK_SUCCESS) && (cmd_data_ptr != NULL))
  {
    /* build the proactive cmd request into the gstk_exp.h _req structure */

    /* Message Header */
    request.hdr_cmd.command_id = GSTK_SEND_SS_REQ;
    request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

    /* command number */
    request.hdr_cmd.command_number =
        command_details_ref_table_ptr[index].command_details.command_number;

    /* initialize all optional field to not present */
    request.cmd.send_ss_pro_cmd_req.alpha.length = 0;
    request.cmd.send_ss_pro_cmd_req.icon.present = FALSE;

    /* alpha (O)
      SS string
      icon id (O)
    */

    /* optional alpha */
    gstk_status = gstk_parser_alpha_identifier_tlv(cmd_data_ptr,
                                              cmd_data_len,
                                              curr_offset,
                                              &(request.cmd.send_ss_pro_cmd_req.alpha),
                                              &new_offset,
                                              &more_tlv,
                                              GSTK_OPTIONAL,
                                              &cmd_details_ref_id);

    if (gstk_status != GSTK_SUCCESS || !more_tlv) { /* since there is at least 1 more mandatory tlv */
      error_already_raised = TRUE;
    }
    else {
      curr_offset = new_offset;
    }

    /* mandatory ss string */
    if (!error_already_raised ) {
      gstk_status = gstk_parser_address_ss_string_tlv(
                                                GSTK_SS_STRING_TAG,
                                                cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                                &(request.cmd.send_ss_pro_cmd_req.ss_string),
                                                &new_offset,
                                                &more_tlv,
                                                GSTK_MANDATORY,
                                                &cmd_details_ref_id);

      if (gstk_status != GSTK_SUCCESS) {
        error_already_raised = TRUE;
      }
      else {
        if (more_tlv) {
          curr_offset = new_offset;
          /*optional icon */
          gstk_status = gstk_parser_icon_id_tlv(
                                    &(request.cmd.send_ss_pro_cmd_req.alpha),
                                    cmd_data_ptr,
                                    cmd_data_len,
                                    curr_offset,
                                    &(request.cmd.send_ss_pro_cmd_req.icon),
                                    &new_offset,
                                    &more_tlv,
                                    GSTK_OPTIONAL,
                                    &cmd_details_ref_id);

          if (gstk_status != GSTK_SUCCESS ) {
            /* this is the last tlv, if the more_tlv is TRUE, => error */
            error_already_raised = TRUE;
          }
        } /*more tlv after ss string parsing*/
      } /* no error from ss string parsing */
    } /* no error from alpha */
  } /* preprocess command success */
  else { /* preprocess_cmd returns errors */
    error_already_raised = TRUE;
  } /* GSTK_ERROR from proprocess_cmd */

  /* No error has been encountered */
  if(!error_already_raised) {
    if (gstk_util_send_tr_if_no_service(
          cmd_details_ref_id,
          command_details_ref_table_ptr[index].command_details.command_number,
          GSTK_SEND_SS_CNF))
    {
      gstk_send_ss_req_cleanup(&request, cmd_data_ptr);
      return GSTK_SUCCESS;
    }

    if (gstk_util_send_tr_if_not_3gpp(
          cmd_details_ref_id,
          command_details_ref_table_ptr[index].command_details.command_number,
          GSTK_SEND_SS_CNF))
    {
      gstk_send_ss_req_cleanup(&request, cmd_data_ptr);
      return GSTK_SUCCESS;
    }

    /* Send command to clients */
    gstk_status = gstk_util_send_message_to_clients(&request);

    if(gstk_status != GSTK_SUCCESS)
    {
      /* Send Terminal Response to Card */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      UIM_MSG_HIGH_0("send ss call send client err");
      if(gstk_status == GSTK_NO_CLIENT_REG_FOUND)
      {
          gen_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      }

      /* Send Terminal Response to Card */
      gstk_set_additional_info(&additional_info,GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

      gstk_status = gstk_send_terminal_response(
                      gstk_curr_inst_ptr->client_id, /* client_id */
                      0,              /* user_data */
                      cmd_details_ref_id, /* command_details_ref */
                      command_details_ref_table_ptr[index].command_details.command_number,  /* command_num */
                      GSTK_SEND_SS_CNF, /* command response */
                      gen_result, /* general_result */
                      &additional_info, /* additional result */
                      &dummy_extra_param /* extra param */ );
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
      }
    }
    else
    {
      /* Start Timer */
      gstk_util_start_client_response_timer(cmd_details_ref_id,
                                            GSTK_TERM_RSP_WAIT_TIME);
      /* reset gstk_status to SUCCESS since there has been no error*/
      /* This is needed since some of the TLVs are optional, which might
      ** cause the various parsing functions to return invalid_command */
      gstk_status = GSTK_SUCCESS;
    }
  }
  else { /* error, free alpha string if created already */

    UIM_MSG_ERR_0("send ss parsing err");
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    gstk_status = gstk_send_terminal_response(
                    gstk_curr_inst_ptr->client_id, /* client_id */
                    0,              /* user_data */
                    cmd_details_ref_id, /* command_details_ref */
                    command_details_ref_table_ptr[index].command_details.command_number,  /* command_num */
                    GSTK_SEND_SS_CNF, /* command response */
                    GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                    &additional_info, /* additional result */
                    &dummy_extra_param /* extra param */ );
    if(gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
    }
  } /* if error_already_raised */

  /* release memory allocation for text string if not null */
  gstk_send_ss_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);

} /* gstk_send_ss_req */

/*===========================================================================
FUNCTION gstk_send_ss_cnf

DESCRIPTION
  This function packs the Send SS terminal response and send
  the command to UIM server.

PARAMETERS
  send_ss_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_send_ss_cnf (
  gstk_send_ss_cnf_type* send_ss_cnf_ptr )
{

  gstk_status_enum_type                     gstk_status     = GSTK_SUCCESS;
  setup_call_send_ss_terminal_response_type STK_response    = {0};
  uim_cmd_type                              *uim_cmd_ptr    = NULL;
  uint8                                     header_len      = 0;
  boolean                                   has_ussd_string = FALSE;
  boolean                                   has_cc          = FALSE;
  uint32                                    offset          = 0;
  uint32                                    ref_id          = 0;

  UIM_MSG_HIGH_0("IN GSTK send ss cnf ");

  if(send_ss_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("send_ss_cnf_ptr ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(setup_call_send_ss_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      send_ss_cnf_ptr->cmd_details_ref_id,
      send_ss_cnf_ptr->message_header.command_id,
      send_ss_cnf_ptr->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, FALSE, FALSE);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, FALSE, FALSE);
    return gstk_status;
  }

  if (send_ss_cnf_ptr->extra_param.has_cc_type_modification) {
    if((send_ss_cnf_ptr->extra_param.command_result2 & 0xF0) == 0x20)
    {
      send_ss_cnf_ptr->command_result = GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM;
    }
    else if((send_ss_cnf_ptr->extra_param.command_result2 & 0xF0) == 0x30)
    {
      send_ss_cnf_ptr->command_result = GSTK_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM;
    }
    else
    {
      send_ss_cnf_ptr->command_result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
    }
  }


  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      send_ss_cnf_ptr->command_result,
      send_ss_cnf_ptr->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, FALSE, FALSE);
    return gstk_status;
  }

  /* header_len:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  header_len = STK_response.header.command_details.length +
              STK_response.header.device_id.device_tag_length +
              STK_response.header.result.result_tag_length +
              6;


  /* Pack extra param if presented */
  if (send_ss_cnf_ptr->extra_param.has_cc_type_modification) {
    /* extra param present due to call control type change */
    /* CC requested action */

    /* memalloc alpha and ussd string inside the packing function */
    gstk_status = gstk_packer_cc_req_action_tlv(
      &(gstk_shared_data.cc_req_action),
      &STK_response.cc_req_action);

    if (gstk_status != GSTK_SUCCESS) {
      if (gstk_shared_data.cc_req_action.call_type == GSTK_USSD) {
        gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, TRUE, TRUE);
      }
      else {
        gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, TRUE, FALSE);
      }
      return gstk_status;
    }

    /* result object 2 */
    gstk_status = gstk_packer_result_tlv(
      send_ss_cnf_ptr->extra_param.command_result2,
      send_ss_cnf_ptr->extra_param.additional_info2_ptr,
      &STK_response.result2_for_cc);

    if (gstk_status != GSTK_SUCCESS) {
      if (gstk_shared_data.cc_req_action.call_type == GSTK_USSD) {
        gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, TRUE, TRUE);
      }
      else {
        gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, TRUE, FALSE);
      }
      return gstk_status;
    }

    /* length == header len +
                cc requested action + 2
                result object 2 + 2
                */
    STK_response.num_bytes = header_len +
                            STK_response.cc_req_action.length +
                            STK_response.result2_for_cc.result_tag_length + 4;
    has_cc = TRUE;

    if (gstk_shared_data.cc_req_action.call_type == GSTK_USSD &&
        send_ss_cnf_ptr->extra_param.text_string2.length > 0) {
      /* text string2 required if change to USSD */
      STK_response.cc_mod_text2.text = (uint8*)GSTK_CALLOC(
                                       send_ss_cnf_ptr->extra_param.text_string2.length);
      if (STK_response.cc_mod_text2.text == NULL) {
        gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, has_cc, TRUE);
        return gstk_status;
      }

      gstk_status = gstk_packer_text_string_tlv(
          GSTK_TEXT_STRING_TAG,
          send_ss_cnf_ptr->extra_param.text_string2.dcs,
          send_ss_cnf_ptr->extra_param.text_string2.length,
          send_ss_cnf_ptr->extra_param.text_string2.text,
          &STK_response.cc_mod_text2);

      if (gstk_status != GSTK_SUCCESS) {
        gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr, &STK_response, has_cc, TRUE);
        return gstk_status;
      }

      /* length == length from above +
                  text string for changing to USSD + 2 */
      STK_response.num_bytes += STK_response.cc_mod_text2.length + 2;
      has_ussd_string = TRUE;
    }
  }
  else { /* not extra param */
    STK_response.num_bytes = header_len;
  }


  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        ref_id = send_ss_cnf_ptr->cmd_details_ref_id;

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(send_ss_cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = gstk_curr_inst_ptr->client_id;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                   &STK_response.header.command_details,
                                   offset,
                                   (size_t)(STK_response.header.command_details.length +
                                            GSTK_TAG_LENGTH_LEN),
                                   sizeof(uim_cmd_ptr->terminal_response.data),
                                   sizeof(gstk_command_details_tlv_type)) <
           (size_t)(STK_response.header.command_details.length + GSTK_TAG_LENGTH_LEN))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy device */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.header.device_id,
                                        offset +=
                                        (int)(STK_response.header.command_details.length +
                                              GSTK_TAG_LENGTH_LEN),
                                        (size_t)(STK_response.header.device_id.device_tag_length +
                                                 GSTK_TAG_LENGTH_LEN),
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_device_identities_tag_tlv_type)) <
                (size_t)(STK_response.header.device_id.device_tag_length +
                         GSTK_TAG_LENGTH_LEN))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy result */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.header.result,
                                        offset +=
                                        (int)(STK_response.header.device_id.device_tag_length +
                                              GSTK_TAG_LENGTH_LEN),
                                        3, /* result tag +  length + general result */
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_result_tag_tlv_type)) < 3)
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }

        offset = offset + 3;

        if(gstk_status == GSTK_SUCCESS &&
           STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
          if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                     STK_response.header.result.additional_info,
                                     offset,
                                     (size_t)(STK_response.header.result.result_tag_length - 1),
                                     sizeof(uim_cmd_ptr->terminal_response.data),
                                     ((STK_response.header.result.result_tag_length - 1) *
                                      sizeof(byte))) <
             (size_t)(STK_response.header.result.result_tag_length - 1))
          {
            UIM_MSG_ERR_0("gstk_send_ss_cnf(): memscpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
          offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
        }

        if (gstk_status == GSTK_SUCCESS && has_cc) {
          /* there are CC extra param */
          /* copy call control requested action tlv */

          /* copy in separate section */
          uim_cmd_ptr->terminal_response.data[header_len++] = STK_response.cc_req_action.tag;
          if(STK_response.cc_req_action.alpha.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
          {
            uim_cmd_ptr->terminal_response.data[header_len++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
            uim_cmd_ptr->terminal_response.data[header_len++] = STK_response.cc_req_action.length + 1;
          }
          else
          {
            if( STK_response.cc_req_action.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
            {
              uim_cmd_ptr->terminal_response.data[header_len++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
            }
            uim_cmd_ptr->terminal_response.data[header_len++] = STK_response.cc_req_action.length;
          }

          if(gstk_status == GSTK_SUCCESS &&
             gstk_shared_data.cc_req_action.call_type == GSTK_VOICE) {
            if((tags_enum_type)STK_response.cc_req_action.gstk_address_string_tlv.address.tag == GSTK_ADDRESS_TAG) {
              /* has address */
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.cc_req_action.gstk_address_string_tlv.address,
                                         header_len,
                                         (size_t)(STK_response.cc_req_action.gstk_address_string_tlv.address.length
                                                  + 2),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_address_tag_tlv_type)) <
                 (size_t)(STK_response.cc_req_action.gstk_address_string_tlv.address.length + 2))
              {
                UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
              header_len = header_len +
                STK_response.cc_req_action.gstk_address_string_tlv.address.length + 2;
            }
            if(gstk_status == GSTK_SUCCESS &&
               (tags_enum_type)STK_response.cc_req_action.ccp1.tag == GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG) {
              /* has ccp1 */
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.cc_req_action.ccp1,
                                         header_len,
                                         (size_t)(STK_response.cc_req_action.ccp1.length + 2),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_cap_config_tlv_type)) <
                 (size_t)(STK_response.cc_req_action.ccp1.length + 2))
              {
                UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
              header_len = header_len +
                STK_response.cc_req_action.ccp1.length + 2;

            }
            if(gstk_status == GSTK_SUCCESS &&
               (tags_enum_type)STK_response.cc_req_action.subaddress.tag == GSTK_CALLED_PARTY_SUB_ADDRESS_TAG) {
              /* has subaddr */
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.cc_req_action.subaddress,
                                         header_len,
                                         (size_t)(STK_response.cc_req_action.subaddress.length + 2),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_subaddress_tlv_type)) <
                 (size_t)(STK_response.cc_req_action.subaddress.length + 2))
              {
                UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
              header_len = header_len +
                STK_response.cc_req_action.subaddress.length + 2;
            }
          } /* ! ussd_string */
          else {
            if (gstk_status == GSTK_SUCCESS &&
                (tags_enum_type)STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.tag ==
                GSTK_USSD_STRING_TAG) {
            /* has ussd string */
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.cc_req_action.gstk_address_string_tlv.ussd_string,
                                         header_len,
                                         3, /* tag, length and dcs */
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_string_tlv_type)) < 3)
              {
                UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
              header_len += 3;
              if(gstk_status == GSTK_SUCCESS &&
                 STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.length > 1) {
                if(gstk_byte_offset_memcpy(
                     uim_cmd_ptr->terminal_response.data,
                     STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.text,
                     header_len,
                     (size_t)(STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.length - 1),
                     sizeof(uim_cmd_ptr->terminal_response.data),
                     gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.length)
                   <
                   (size_t)(STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.length - 1))
                {
                  UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                  gstk_status = GSTK_MEMORY_ERROR;
                }
                header_len = header_len +
                STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.length - 1;
              }
            }
          }
          if (gstk_status == GSTK_SUCCESS &&
              (tags_enum_type)STK_response.cc_req_action.alpha.tag == GSTK_ALPHA_IDENTIFIER_TAG) {
            /* copy alpha tag */
            if(header_len < UIM_MAX_TR_BYTES)
            {
              uim_cmd_ptr->terminal_response.data[header_len++] = STK_response.cc_req_action.alpha.tag;
            }
            /* copy alpha length */
            if(header_len < UIM_MAX_TR_BYTES &&
               STK_response.cc_req_action.alpha.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
            {
              uim_cmd_ptr->terminal_response.data[header_len++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
            }
            if(header_len < UIM_MAX_TR_BYTES)
            {
              uim_cmd_ptr->terminal_response.data[header_len++] = STK_response.cc_req_action.alpha.length;
            }
            /* copy the alpha text */
            if(gstk_status == GSTK_SUCCESS &&
               STK_response.cc_req_action.alpha.length > 0) {
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         STK_response.cc_req_action.alpha.alpha,
                                         header_len,
                                         (size_t)(STK_response.cc_req_action.alpha.length),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         gstk_shared_data.cc_req_action.alpha.length) <
                 (size_t)(STK_response.cc_req_action.alpha.length))
              {
                UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
              header_len += STK_response.cc_req_action.alpha.length;
            }
          }
          if(gstk_status == GSTK_SUCCESS &&
             gstk_shared_data.cc_req_action.call_type == GSTK_VOICE) {
            if ((tags_enum_type)STK_response.cc_req_action.bc_repeat_ind.tag == GSTK_BC_REPEAT_INDICATOR_TAG) {
              /* has bc repeat indicator */
                if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                           &STK_response.cc_req_action.bc_repeat_ind,
                                           header_len,
                                           (size_t)(STK_response.cc_req_action.bc_repeat_ind.length + 2),
                                           sizeof(uim_cmd_ptr->terminal_response.data),
                                           sizeof(gstk_bc_repeat_ind_tlv_type)) <
                   (size_t)(STK_response.cc_req_action.bc_repeat_ind.length + 2))
                {
                  UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                  gstk_status = GSTK_MEMORY_ERROR;
                }
                header_len = header_len +
                  STK_response.cc_req_action.bc_repeat_ind.length + 2;
            }
            if(gstk_status == GSTK_SUCCESS &&
               (tags_enum_type)STK_response.cc_req_action.ccp2.tag == GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG) {
              /* has ccp2 */
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.cc_req_action.ccp2,
                                         header_len,
                                         (size_t)(STK_response.cc_req_action.ccp2.length + 2),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_cap_config_tlv_type)) <
                 (size_t)(STK_response.cc_req_action.ccp2.length + 2))
              {
                UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
              header_len = header_len +
                STK_response.cc_req_action.ccp2.length + 2;
            }
          }

          /* copy result object 2 tlv */
          if(gstk_status == GSTK_SUCCESS &&
          gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                  &STK_response.result2_for_cc,
                                  header_len,
                                  3,
                                  sizeof(uim_cmd_ptr->terminal_response.data),
                                  sizeof(gstk_result_tag_tlv_type)) < 3)
          {
            UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
          header_len += 3;

          if(gstk_status == GSTK_SUCCESS &&
             STK_response.result2_for_cc.result_tag_length > 1 ) { /* has additional info */
            if(gstk_byte_offset_memcpy(
                 uim_cmd_ptr->terminal_response.data,
                 STK_response.result2_for_cc.additional_info,
                 header_len,
                 (size_t)(STK_response.result2_for_cc.result_tag_length - 1),
                 sizeof(uim_cmd_ptr->terminal_response.data),
                 (send_ss_cnf_ptr->extra_param.additional_info2_ptr.length *
                  sizeof(byte))) <
               (size_t)(STK_response.result2_for_cc.result_tag_length - 1))
            {
              UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
              gstk_status = GSTK_MEMORY_ERROR;
            }
            header_len = header_len + STK_response.result2_for_cc.result_tag_length - 1;
          }

          /* if there is text string 2 */
          if (gstk_status == GSTK_SUCCESS &&
              has_ussd_string) {
            /* copy text string 2 */
            /* copy text tag, length */
            if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                       &STK_response.cc_mod_text2,
                                       header_len,
                                       3,
                                       sizeof(uim_cmd_ptr->terminal_response.data),
                                       sizeof(gstk_string_tlv_type)) < 3)
            {
              UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
              gstk_status = GSTK_MEMORY_ERROR;
            }
            header_len += 3;
            /* copy the text */
            if(gstk_status == GSTK_SUCCESS &&
               STK_response.cc_mod_text2.length > 1) {
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         STK_response.cc_mod_text2.text,
                                         header_len,
                                         (size_t)(STK_response.cc_mod_text2.length - 1),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         send_ss_cnf_ptr->extra_param.text_string2.length) <
                 (size_t)(STK_response.cc_mod_text2.length - 1))
              {
                UIM_MSG_ERR_0("gstk_send_ss_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
            }
          }
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_util_dump_byte_array("SS TR",
                                    uim_cmd_ptr->terminal_response.data,
                                    uim_cmd_ptr->terminal_response.num_bytes);

          gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
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

  gstk_send_ss_cnf_cleanup(send_ss_cnf_ptr,
                           &STK_response,
                           has_cc,
                           has_ussd_string);
  return gstk_status;
} /* gstk_send_ss_cnf */

