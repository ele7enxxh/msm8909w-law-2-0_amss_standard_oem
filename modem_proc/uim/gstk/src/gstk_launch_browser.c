/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L A U N C H   B R O W S E R


GENERAL DESCRIPTION
  This source file contains functions to process launch browser
  proactive command


FUNCTIONS
  gstk_launch_browser_req
    This function parses the launch browser Proactive Command from the card
    and sends it to the client.

  gstk_launch_browser_cnf
    This function packs the launch browser terminal response and send
    the command to UIM server.

  gstk_launch_browser_cnf_cleanup
    This function cleans up any allocated pointer on the stack.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_launch_browser.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/12/15   vr      F3 Logging inprovements
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
06/30/14   gm      Remove unnecessary clear of timer
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/28/13   gs      Remove tmc.h inclusion
11/13/13   shr     Added support for BIP over WLAN
10/04/13   gm      F3 message reduction
09/13/13   gm      Support for Recovery and hot-swap
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
06/06/11   sg      Add support for mandatory TLV for result 0x20
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
02/15/10   nb      Moved Dual Slot changes
09/26/09   kp      ZI memory reduction changes
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/19/07   nk      Added check to avoid potential buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
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
04/16/04   tml     linted
05/16/03   tml     linted
04/29/03   tml     Properly initialize data struct
04/17/03   tml     lint
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
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
FUNCTION gstk_launch_browser_req_cleanup

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
static void gstk_launch_browser_req_cleanup(
  gstk_cmd_from_card_type *request_ptr,
  uint8                   *cmd_data_ptr
)
{
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_launch_browser_req_cleanup */

/*===========================================================================
FUNCTION gstk_launch_browser_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  launch_browser_cnf_ptr: [Input] Pointer to message that is required to
                                  be processed and sent to UIM
  STK_response_ptr      : [Input] Pointer to the STK intermediate message

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
static void gstk_launch_browser_cnf_cleanup(
  gstk_launch_browser_cnf_type           *launch_browser_cnf_ptr,
  launch_browser_terminal_response_type  *STK_response_ptr)
{

  if(STK_response_ptr == NULL)
  {
    UIM_MSG_ERR_0("STK_response_ptr ERR:NULL");
    return;
  }

  if(launch_browser_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("launch_browser_cnf_ptr ERR:NULL");
    return;
  }
  /* free launch browser cnf and tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)launch_browser_cnf_ptr);/*lint !e826 area too small */
  if (STK_response_ptr->data.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->data.result.additional_info);
    STK_response_ptr->data.result.additional_info = NULL;
  }
} /* gstk_launch_browser_cnf_cleanup */

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
gstk_status_enum_type gstk_launch_browser_req  (
  gstk_proactive_cmd_type* command_ptr)
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
  uint32                              i                    = 0;
  uint32                              index                = 0;

  /* Cannot do {0x00} in declaration because of compilation warning
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  UIM_MSG_HIGH_0(" IN GSTK_LAUNCH_BROWSER_REQ ");

  /* check for Null ptr */
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

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
    request.hdr_cmd.command_id = GSTK_LAUNCH_BROWSER_REQ;
    request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

    /* command number */
    request.hdr_cmd.command_number =
      command_details_ref_table_ptr[index].command_details.command_number;

    /* launch browser requirement */
    switch(command_details_ref_table_ptr[index].command_details.qualifier) {
    case 0x00:
      request.cmd.launch_browser_pro_cmd_req.launch_mode = GSTK_LAUNCH_IF_NOT_ALREADY_LAUNCH;
      /*. . . launch if not alredy launch*/
      break;
    case 0x02:
      request.cmd.launch_browser_pro_cmd_req.launch_mode = GSTK_USE_EXISTING_BROWSER;
      /*. . . User existing browser*/
      break;
    case 0x03:
      request.cmd.launch_browser_pro_cmd_req.launch_mode = GSTK_CLOSE_EXISTING_BROWSER;
      /*. . . Close existing browser first*/
      break;
    default:
      /*Unknown browser qualifier*/
      error_already_raised = TRUE;
    }

    if (!error_already_raised) {

      if ((request.cmd.launch_browser_pro_cmd_req.launch_mode != GSTK_LAUNCH_IF_NOT_ALREADY_LAUNCH) &&
          (request.cmd.launch_browser_pro_cmd_req.launch_mode != GSTK_USE_EXISTING_BROWSER) &&
          (request.cmd.launch_browser_pro_cmd_req.launch_mode != GSTK_CLOSE_EXISTING_BROWSER)) {
        /* some RFU values, return errors */
        error_already_raised = TRUE;
      }

      /* initialize all optional field to not present */
      request.cmd.launch_browser_pro_cmd_req.browser_id = 0x00; /* set to default value */
      request.cmd.launch_browser_pro_cmd_req.bearer.length = 0;
      request.cmd.launch_browser_pro_cmd_req.num_provisioning_files = 0;
      request.cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length = 0;
      request.cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length = 0;
      request.cmd.launch_browser_pro_cmd_req.user_confirm_icon.present = FALSE;


      /* browser ID (O)
        url
        bearer (O)
        provisioning file reference (1-N)(O)
        gateway / proxy id text string (O)
        alpha ID - user confirmation (O)
        icon ID - user confirmation (O)
      */

      /* optional browser ID */
      /* current implementation is defaulted to " default browser "
        if the tlv is presented but the values are RFU, send TR back to Card */
      /* browser id */
      gstk_status = gstk_parser_browser_id_tlv( cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                                &(request.cmd.launch_browser_pro_cmd_req.browser_id),
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

      /* mandatory url */
      if (!error_already_raised ) {
        gstk_status = gstk_parser_url_tlv(cmd_data_ptr,
                                              cmd_data_len,
                                              curr_offset,
                                              &(request.cmd.launch_browser_pro_cmd_req.url),
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
            /*optional bearer */
            /* there is bearer */
            gstk_status = gstk_parser_bearer_tlv(
                                          cmd_data_ptr,
                                          cmd_data_len,
                                          curr_offset,
                                          &(request.cmd.launch_browser_pro_cmd_req.bearer),
                                          &new_offset,
                                          &more_tlv,
                                          GSTK_OPTIONAL,
                                          &cmd_details_ref_id);

            if (gstk_status != GSTK_SUCCESS) {
              error_already_raised = TRUE;
            }
            else {
              if (more_tlv){
                i = 0;
                curr_offset = new_offset;
                /*optional provisioning file reference */
                while (gstk_valid_tag(cmd_data_ptr[curr_offset],GSTK_PROVISIONING_REF_FILE_TAG)) {
                  /* there is provisioning file reference */
                  gstk_status = gstk_parser_provisioning_ref_file_tlv(
                                            cmd_data_ptr,
                                            cmd_data_len,
                                            curr_offset,
                                            &(request.cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i]),
                                            &new_offset,
                                            &more_tlv,
                                            GSTK_OPTIONAL,
                                            &cmd_details_ref_id);
                  /* no more optional tlvs */
                  if (!more_tlv) {
                    break;
                  }
                  /* when there is an error */
                  if (gstk_status != GSTK_SUCCESS) {
                    error_already_raised = TRUE;
                    break;
                  }

                  /* increment the number of provisioning_file_ref */
                  i++;
                  if (i >= GSTK_MAX_PROVISIONING_FILE_REF) {
                    error_already_raised = TRUE;
                    UIM_MSG_HIGH_0("> Max_Provisioning_file");
                    /* more than what GSTK can handle */
                    break;
                  }
                  /* update the curr offset */
                  curr_offset = new_offset;

                } /* valid provisioning file reference present */
                request.cmd.launch_browser_pro_cmd_req.num_provisioning_files
                    = i;

                UIM_MSG_HIGH_1("num_provisioning_files 0x%x",
                               request.cmd.launch_browser_pro_cmd_req.num_provisioning_files );

                if (!error_already_raised) {
                  /* number of provisioning reference files*/
                  request.cmd.launch_browser_pro_cmd_req.num_provisioning_files
                                = i;

                  if (more_tlv){ /* gateway/proxy id text */
                    curr_offset = new_offset;
                    /*optional gateway/proxy id text */
                    gstk_status = gstk_parser_text_string_tlv(cmd_data_ptr,
                                                                cmd_data_len,
                                                                curr_offset,
                                                                GSTK_TEXT_STRING_TAG,
                                                                &(request.cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string),
                                                                &new_offset,
                                                                &more_tlv,
                                                                GSTK_OPTIONAL,
                                                                &cmd_details_ref_id);

                    if (gstk_status != GSTK_SUCCESS) {
                      error_already_raised = TRUE;
                    }
                    else {
                      if (more_tlv){
                        curr_offset = new_offset;
                        /*optional user confirm alpha */
                        gstk_status = gstk_parser_alpha_identifier_tlv(
                                                      cmd_data_ptr,
                                                      cmd_data_len,
                                                      curr_offset,
                                                      &(request.cmd.launch_browser_pro_cmd_req.user_confirm_alpha),
                                                      &new_offset,
                                                      &more_tlv,
                                                      GSTK_OPTIONAL,
                                                      &cmd_details_ref_id);

                        if (gstk_status != GSTK_SUCCESS) {
                          error_already_raised = TRUE;
                        }
                        else {
                          if (more_tlv){
                            curr_offset = new_offset;
                            /*optional user confirm icon */
                            gstk_status = gstk_parser_icon_id_tlv(
                                                  &(request.cmd.launch_browser_pro_cmd_req.user_confirm_alpha),
                                                  cmd_data_ptr,
                                                  cmd_data_len,
                                                  curr_offset,
                                                  &(request.cmd.launch_browser_pro_cmd_req.user_confirm_icon),
                                                  &new_offset,
                                                  &more_tlv,
                                                  GSTK_OPTIONAL,
                                                  &cmd_details_ref_id);

                            if (gstk_status != GSTK_SUCCESS ) {
                              error_already_raised = TRUE;
                            } /* error from user confirm icon */

                          } /* more tlv, try user confirm icon */
                        } /* no error after user confirm alpha or there is no user confirm alpha */
                      } /*  more tlv, try user confirmation alpha */
                    } /* no error after gateway text parsing or there is no gateway text */
                  } /* more tlv, try gateway text parsing next */
                } /* no error after provisioning ref file parsing or there is no provisioning ref file */
              } /* more tlv, try provisioning ref file parsing next */
            } /* no error after bearer parsing or if there is no bearer present */
          } /*more tlv after url parsing*/
        } /* no error from url parsing */
      } /* no error from browser id */
    } /* qualifier error */
  } /* preprocess command success */
  else { /* preprocess_cmd returns errors */
    error_already_raised = TRUE;
  } /* GSTK_ERROR from proprocess_cmd */

  /* No error has been encountered */
  if(!error_already_raised) {
    /* Send command to clients */
    gstk_status = gstk_util_send_message_to_clients(&request);

    if(gstk_status != GSTK_SUCCESS)
    {
      /* Send Terminal Response to Card */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      UIM_MSG_HIGH_0("launch browser send client err");

      /* Send Terminal Response to Card */
      gstk_set_additional_info(
        &additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

      gstk_status = gstk_send_terminal_response(
                      gstk_curr_inst_ptr->client_id, /* client_id */
                      0,              /* user_data */
                      cmd_details_ref_id, /* command_details_ref */
                      command_details_ref_table_ptr[index].command_details.command_number,  /* command_num */
                      GSTK_LAUNCH_BROWSER_CNF, /* command response */
                      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
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

    UIM_MSG_ERR_0("launch browser parsing err");
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    gstk_status = gstk_send_terminal_response(
                    gstk_curr_inst_ptr->client_id, /* client_id */
                    0,              /* user_data */
                    cmd_details_ref_id, /* command_details_ref */
                    command_details_ref_table_ptr[index].command_details.command_number,  /* command_num */
                    GSTK_LAUNCH_BROWSER_CNF, /* command response */
                    GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                    &additional_info, /* additional result */
                    &dummy_extra_param /* extra param */ );
    if(gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
    }
  } /* if error_already_raised */

  /* release memory allocation for alpha string if not null */
  gstk_launch_browser_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);
} /* gstk_launch_browser_req */

/*===========================================================================
FUNCTION gstk_launch_browser_cnf

DESCRIPTION
  This function packs the Launch Browser terminal response and send
  the command to UIM server.

PARAMETERS
  launch_browser_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_launch_browser_cnf (
  gstk_launch_browser_cnf_type* launch_browser_cnf_ptr )
{

  gstk_status_enum_type                   gstk_status   = GSTK_SUCCESS;
  launch_browser_terminal_response_type   STK_response  = {0};
  uim_cmd_type                            *uim_cmd_ptr  = NULL;
  uint32                                  offset        = 0;
  uint32                                  ref_id        = 0;

  UIM_MSG_HIGH_0("IN GSTK launch browser cnf ");

  if(launch_browser_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("launch_browser_cnf_ptr ERR:NULL");
    return GSTK_BAD_PARAM;
  }
  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(launch_browser_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      launch_browser_cnf_ptr->cmd_details_ref_id,
      launch_browser_cnf_ptr->message_header.command_id,
      launch_browser_cnf_ptr->command_number,
      &STK_response.data.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_launch_browser_cnf_cleanup(launch_browser_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.data.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_launch_browser_cnf_cleanup(launch_browser_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      launch_browser_cnf_ptr->command_result,
      launch_browser_cnf_ptr->result_additional_info,
      &STK_response.data.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_launch_browser_cnf_cleanup(launch_browser_cnf_ptr, &STK_response);
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
        ref_id = launch_browser_cnf_ptr->cmd_details_ref_id;

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(launch_browser_cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = gstk_curr_inst_ptr->client_id;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                   &STK_response.data.command_details,
                                   offset,
                                   (size_t)(STK_response.data.command_details.length + 2),
                                   sizeof(uim_cmd_ptr->terminal_response.data),
                                   sizeof(gstk_command_details_tlv_type)) <
           (size_t)(STK_response.data.command_details.length + 2))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy device */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.data.device_id,
                                        offset +=
                                        (int)(STK_response.data.command_details.length + 2),
                                        (size_t)(STK_response.data.device_id.device_tag_length + 2),
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_device_identities_tag_tlv_type)) <
                (size_t)(STK_response.data.device_id.device_tag_length + 2))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy result */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.data.result,
                                        offset +=
                                        (int)(STK_response.data.device_id.device_tag_length + 2),
                                        3, /* result tag +  length + general result */
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
            UIM_MSG_ERR_0("gstk_launch_browser_cnf(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
          offset = (int)(offset + STK_response.data.result.result_tag_length - 1);
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_util_dump_byte_array("Launch browser TR",
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
    }
  } /* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_launch_browser_cnf_cleanup(launch_browser_cnf_ptr, &STK_response);
  return gstk_status;

} /* gstk_launch_browser_cnf */

