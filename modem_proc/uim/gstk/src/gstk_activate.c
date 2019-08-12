/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   A C T I V A T E


GENERAL DESCRIPTION
  This source file contains functions to process activate
  proactive command


FUNCTIONS
  gstk_activate_req
    This function parses the Activate Command from the card
    and sends it to the client.

  gstk_activate_cnf
    This function packs the Activate terminal response and sends
    the command to UIM server.

  gstk_activate_cnf_cleanup
    This function cleans up any allocated pointer on the stack.



INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2011-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_activate.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/12/15   vr      F3 Logging inprovements
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
06/30/14   gm      Remove unnecessary clear of timer
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/13/13   shr     Added support for BIP over WLAN
10/04/13   gs      F3 Message reduction
09/13/13   gm      Support for Recovery and hot-swap
06/19/13   gs      changed the Trace Message
03/16/12   yt      Remove tmc.h inclusion
09/07/11   sg      Initial Version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Memory Header Files */
#include "memheap.h"

/* GSTK Header Files   */
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

/* Output & String Header Files */
#include "err.h"
#include "string.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_activate_cnf_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  activate_cnf_ptr: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_response_ptr: [Input] Pointer to the STK intermediate message

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
static void gstk_activate_cnf_cleanup(
  gstk_activate_cnf_type          *activate_cnf_ptr,
  activate_terminal_response_type *STK_response_ptr)
{
   if(STK_response_ptr == NULL ||activate_cnf_ptr == NULL )
  {
    UIM_MSG_ERR_2("gstk_activate_cnf_cleanup ERR Check STK_response_ptr 0x%x and activate_cnf_ptr 0x%x",
                  		STK_response_ptr, activate_cnf_ptr);
    return;
  }
  /* free activate tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)((void*)activate_cnf_ptr));
  if (STK_response_ptr->data.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->data.result.additional_info);
    STK_response_ptr->data.result.additional_info = NULL;
  }
} /* gstk_activate_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_activate_req_cleanup

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
static void gstk_activate_req_cleanup(
  gstk_cmd_from_card_type *request_ptr,
  uint8                   *cmd_data_ptr
)
{
  if(request_ptr == NULL)
  {
    UIM_MSG_ERR_0("request_ptr ptr ERR:NULL");
    return;
  }
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_activate_req_cleanup */

/*===========================================================================
FUNCTION gstk_activate_parsing_err_term_rsp

DESCRIPTION
  This function send terminal response as a result of parsing error.

PARAMETERS
  cmd_details_ref_id : [Input]  command details reference id
  result             : [Input]  gstk result
  additional_info_ptr: [Input]  additional info pointer

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
static void gstk_activate_parsing_err_term_rsp(
  uint32                          cmd_details_ref_id,
  gstk_general_result_enum_type   result,
  const gstk_additional_info_type *additional_info_ptr
)
{
  gstk_status_enum_type               gstk_status       = GSTK_SUCCESS;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param;

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  memset(&dummy_extra_param, 0, sizeof(dummy_extra_param));

  /* initialize dummy_extra_param */
  dummy_extra_param.present = FALSE;

  /* Send Terminal Response to Card */
  /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
  gstk_status = gstk_send_terminal_response(
                  gstk_curr_inst_ptr->client_id,  /* client_id */
                  0,                    /* user_data */
                  cmd_details_ref_id,   /* command_details_ref */
                  command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,  /* command_num */
                  GSTK_ACTIVATE_CNF,     /* command response */
                  result,                /* general_result */
                  additional_info_ptr,   /* additional result */
                  &dummy_extra_param     /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) {
    UIM_MSG_ERR_1("TR fail, 0x%x", gstk_status);
  }
} /* gstk_activate_parsing_err_term_rsp */

/*===========================================================================
FUNCTION gstk_activate_req

DESCRIPTION
  This function parses the Activate Command from the card
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
gstk_status_enum_type gstk_activate_req (
  gstk_proactive_cmd_type* command_ptr
)
{
  gstk_cmd_from_card_type             request;
  gstk_additional_info_type           additional_info;
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  uint8                              *cmd_data_ptr         = NULL;
  int32                               cmd_data_len         = 0;
  uint32                              cmd_details_ref_id   = 0;
  int32                               new_offset           = 0;
  int32                               curr_offset          = 0;
  boolean                             more_tlv             = FALSE;

  UIM_MSG_HIGH_0(" IN GSTK_ACTIVATE_REQ");

  if(!command_ptr)
  {
    UIM_MSG_ERR_0("command_ptr is NULL");
    return GSTK_NULL_INPUT_PARAM;
  }

   /* initialize request to 0 */
  memset(&request, 0x00, sizeof(gstk_cmd_from_card_type));
  /* initialize additional_info to 0 */
  memset(&additional_info, 0x00, sizeof(gstk_additional_info_type));

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data_ptr, and specifies the length of
  ** the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  /* There exists mandatory tlv, so cmd_data_ptr should not be null */
  if ((gstk_status != GSTK_SUCCESS) || (cmd_data_ptr == NULL)) {
    gstk_activate_parsing_err_term_rsp(cmd_details_ref_id,
                                       GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                       &additional_info);
    /* release memory allocation for activate request info data and cmd_data_ptr accordingly */
    gstk_activate_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }

  /* build the proactive cmd request into the gstk_exp.h _req structure */

  /* Message Header */
  request.hdr_cmd.command_id = GSTK_ACTIVATE_REQ;
  request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

  /* command number */
  request.hdr_cmd.command_number =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number;

  /* Command details, qualifier is RFU */

  /* Activate Descriptor TLV */
  gstk_status = gstk_parser_activate_descriptor_tlv(cmd_data_ptr,
                                                    cmd_data_len,
                                                    curr_offset,
                                                    GSTK_ACTIVATE_DESCRIPTOR_TAG,
                                                    &(request.cmd.activate_pro_cmd_req.target),
                                                    &new_offset,
                                                    &more_tlv,
                                                    GSTK_MANDATORY,
                                                    &cmd_details_ref_id);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_activate_parsing_err_term_rsp(cmd_details_ref_id,
                                       GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                       &additional_info);
    /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
    gstk_activate_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }

  /* Send command to clients */
  gstk_status = gstk_util_send_message_to_clients(&request);

  if(gstk_status != GSTK_SUCCESS)
  {
    /* Send Terminal Response to Card */
    /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
    UIM_MSG_HIGH_0("Activate not able to send to client");

    /* Send Terminal Response to Card */
    gstk_set_additional_info(
      &additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

    gstk_activate_parsing_err_term_rsp(
      cmd_details_ref_id,
      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
      &additional_info);
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

  /* release memory allocation for text string if not null */
  gstk_activate_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);

} /*gstk_activate_req*/

/*===========================================================================
FUNCTION gstk_activate_cnf

DESCRIPTION
  This function packs the Activate terminal response and send
  the command to UIM server.

PARAMETERS
  activate_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_activate_cnf (
  gstk_activate_cnf_type* activate_cnf_ptr )
{

  gstk_status_enum_type                     gstk_status  = GSTK_SUCCESS;
  activate_terminal_response_type           STK_response;
  uim_cmd_type                             *uim_cmd_ptr  = NULL;
  int                                       offset       = 0;
  uint32                                    ref_id       = 0;

  UIM_MSG_HIGH_0("IN GSTK_ACTIVATE_CNF ");

  if(activate_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Activate Cnf ERR: NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(activate_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      activate_cnf_ptr->cmd_details_ref_id,
      activate_cnf_ptr->message_header.command_id,
      activate_cnf_ptr->command_number,
      &STK_response.data.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_activate_cnf_cleanup(activate_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.data.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_activate_cnf_cleanup(activate_cnf_ptr, &STK_response);
    return gstk_status;
  }


  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      activate_cnf_ptr->command_result,
      activate_cnf_ptr->result_additional_info,
      &STK_response.data.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_activate_cnf_cleanup(activate_cnf_ptr, &STK_response);
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
        ref_id = activate_cnf_ptr->cmd_details_ref_id;

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(activate_cnf_ptr->cmd_details_ref_id);

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
                                        3,/* result tag +  length + general result */
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_result_tag_tlv_type)) < 3)
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        offset = offset + 3;
        /* has additional info */
        if((STK_response.data.result.result_tag_length > 1) &&
           (gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                    STK_response.data.result.additional_info,
                                    offset,
                                    (size_t)(STK_response.data.result.result_tag_length - 1),
                                    sizeof(uim_cmd_ptr->terminal_response.data),
                                    ((STK_response.data.result.result_tag_length - 1)*
                                     sizeof(byte))) <
            (size_t)(STK_response.data.result.result_tag_length - 1)))
        {
          UIM_MSG_ERR_0("gstk_activate_cnf(): gstk_byte_offset_memcpy failed");
          gstk_status = GSTK_MEMORY_ERROR;
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_util_dump_byte_array("Activate: TR",
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

  gstk_activate_cnf_cleanup(activate_cnf_ptr, &STK_response);
  return gstk_status;
} /* gstk_activate_cnf */
