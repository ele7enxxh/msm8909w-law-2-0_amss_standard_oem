/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   G E T   C H A N N E L   S T A T U S


GENERAL DESCRIPTION
  This source file contains functions to process select item
  proactive command


FUNCTIONS
  gstk_get_ch_status_req
    This function parses the Get Channel Status Command from the card
    and sends it to the client.

  gstk_get_ch_status_cnf
    This function packs the Get Channel Status terminal response and sends
    the command to UIM server.

  gstk_get_ch_status_cnf_cleanup
    This function cleans up any allocated pointer on the stack.



INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.  
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_get_ch_status.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
08/26/14   gs      Reject the Get Channel status with right cause when there is no client registered
06/30/14   gm      Remove unnecessary clear of timer
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
02/11/14   gm      Fix KW critical warning
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/13/13   shr     Added support for BIP over WLAN
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
05/23/13   hh      Fix KW errors
04/09/13   vr      Use safer versions of memcpy() and memmove()
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
03/16/12   yt      Remove tmc.h inclusion
08/24/11   xz      Fix compiler warnings and LINT/KW errors
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
02/15/10   nb      Moved Dual Slot Changes
09/26/09   kp      ZI memory reduction changes
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
01/24/05   jk      Fixed compiler warning.
01/10/05   sk      Fixed number of channels being sent down to the SIM
12/27/04   jk      Fixed Compile Warnings.
12/06/04   sk      Changed the 'get_channel_status' terminal response structure
                   to include status of all the open and dropped channels.
                   Changed the src from GSTK_UICC_SIM_DEVICE to
                   GSTK_ME_DEVICE in gstk_get_ch_status_cnf()
                   Removed check for calid channel id from
                   gstk_get_ch_status_req().
11/04/04   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Task Definition Headers */
#include "intconv.h"

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
#include <stringl/stringl.h>
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_get_ch_status_cnf_cleanup

DESCRIPTION
  This function cleans up any stack allocated pointer.

PARAMETERS
  get_ch_status_cnf_ptr: [Input] Pointer to message that is required to
                                 be processed and sent to UIM
  STK_response_ptr     : [Input] Pointer to the STK intermediate message

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
static void gstk_get_ch_status_cnf_cleanup(
  gstk_get_ch_status_cnf_type          *get_ch_status_cnf_ptr,
  get_ch_status_terminal_response_type *STK_response_ptr)
{
  if((STK_response_ptr == NULL)||(get_ch_status_cnf_ptr == NULL))
  {
    UIM_MSG_ERR_0("ERR:NULL pointer STK_response_ptr=0x%x get_ch_status_cnf_ptr=0x%x");
    return;
  }

  /* free get channel status tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)get_ch_status_cnf_ptr);/*lint !e826 area too small */
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info =  NULL;
  }
} /* gstk_get_ch_status_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_get_ch_status_req_cleanup

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
static void gstk_get_ch_status_req_cleanup(
  gstk_cmd_from_card_type *request_ptr,
  uint8                   *cmd_data_ptr
)
{
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_get_ch_status_req_cleanup */

/*===========================================================================
FUNCTION gstk_get_ch_status_parsing_err_term_rsp

DESCRIPTION
  This function sends terminal response as a result of parsing error.

PARAMETERS
  cmd_details_ref_id: [Input]

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
static void gstk_get_ch_status_parsing_err_term_rsp(
  uint32                          cmd_details_ref_id,
  gstk_general_result_enum_type   result,
  const gstk_additional_info_type *additional_info_ptr
)
{
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param;

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  /* initialize dummy_extra_param */
  dummy_extra_param.present = FALSE;

  /* Send Terminal Response to Card */
  /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
  gstk_status = gstk_send_terminal_response(
                                            gstk_curr_inst_ptr->client_id,        /* client_id */
                                            0,                     /* user_data */
                                            cmd_details_ref_id,    /* command_details_ref */
                                            command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,  /* command_num */
                                            GSTK_GET_CH_STATUS_CNF,/* command response */
                                            result,                /* general_result */
                                            additional_info_ptr,   /* additional result */
                                            &dummy_extra_param     /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) {
    UIM_MSG_ERR_1("TR fail, 0x%x", gstk_status);
  }
} /* gstk_get_ch_status_parsing_err_term_rsp */

/*===========================================================================
FUNCTION gstk_get_ch_status_req

DESCRIPTION
  This function parses the Get Channel Status Command from the card
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
gstk_status_enum_type gstk_get_ch_status_req (
  gstk_proactive_cmd_type* command_ptr
)
{
  gstk_status_enum_type               gstk_status        = GSTK_SUCCESS;
  uint8                               *cmd_data_ptr      = NULL;
  int32                               cmd_data_len       = 0;
  uint32                              cmd_details_ref_id = 0;
  gstk_additional_info_type           additional_info    = {0};
  gstk_general_result_enum_type       gen_result         = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;

  /* Cannot do {0x00} in declaration because of compilation warning
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  UIM_MSG_HIGH_0(" IN GSTK_GET_CH_STATUS_REQ");

  /* initialize request to 0 */
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

  if(gstk_status != GSTK_SUCCESS)
  {
    if (gstk_status == GSTK_INVALID_DEVICE_COMMAND) {
      additional_info.length = 1;
      additional_info.additional_info[0] = 0x03;  /* Channel Identifier not valid, can't use enum because
                                                    there are multiple meaning for value 0x03 */
      gstk_get_ch_status_parsing_err_term_rsp(cmd_details_ref_id,
                                          GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                          &additional_info);
    }
    else {
      gstk_get_ch_status_parsing_err_term_rsp(cmd_details_ref_id,
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
    }
    /* release memory allocation for get ch status request info data and cmd_data_ptr accordingly */
    gstk_get_ch_status_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }

  /* build the proactive cmd request into the gstk_exp.h _req structure */

  /* Message Header */
  request.hdr_cmd.command_id = GSTK_GET_CH_STATUS_REQ;
  request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

  /* command number */
  request.hdr_cmd.command_number =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number;

  /* Send command to clients */
  gstk_status = gstk_util_send_message_to_clients(&request);

  if(gstk_status != GSTK_SUCCESS)
  {
    if(gstk_status == GSTK_NO_CLIENT_REG_FOUND)
    {
      gen_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
    }
    /* Send Terminal Response to Card */
    gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

    gstk_get_ch_status_parsing_err_term_rsp(cmd_details_ref_id,
                                            gen_result,
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

  /* release memory allocation for get ch status request info data and cmd_data_ptr accordingly */
  gstk_get_ch_status_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);

} /* gstk_get_ch_status_req */

/*===========================================================================
FUNCTION gstk_get_ch_status_cnf

DESCRIPTION
  This function packs the get channel status terminal response and sends
  the command to UIM server.

PARAMETERS
  get_ch_status_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_get_ch_status_cnf (
  gstk_get_ch_status_cnf_type* get_ch_status_cnf_ptr
)
{
  gstk_status_enum_type                  gstk_status  = GSTK_SUCCESS;
  get_ch_status_terminal_response_type   STK_response;
  uim_cmd_type                           *uim_cmd_ptr = NULL;
  int                                    offset       = 0;
  int                                    i            = 0;
  uint32                                 ref_id       = 0;

  UIM_MSG_HIGH_1("IN GSTK get channel status cnf: get_ch_status_cnf=0x%x",
                 get_ch_status_cnf_ptr);

  if(get_ch_status_cnf_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(get_ch_status_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
     get_ch_status_cnf_ptr->cmd_details_ref_id,
     get_ch_status_cnf_ptr->message_header.command_id,
     get_ch_status_cnf_ptr->command_number,
     &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_get_ch_status_cnf_cleanup(get_ch_status_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_get_ch_status_cnf_cleanup(get_ch_status_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
     get_ch_status_cnf_ptr->command_result,
     get_ch_status_cnf_ptr->result_additional_info,
     &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_get_ch_status_cnf_cleanup(get_ch_status_cnf_ptr, &STK_response);
    return gstk_status;
  }

  if (get_ch_status_cnf_ptr->data_present)
  {
    for(i=0;i<get_ch_status_cnf_ptr->ch_cnt;i++)
    {
      /* Channel Status */
      gstk_status = gstk_packer_ch_status_tlv(
                        &get_ch_status_cnf_ptr->ch_status[i],
                        &STK_response.ch_status[i]);
      if (gstk_status != GSTK_SUCCESS)
      {
        gstk_get_ch_status_cnf_cleanup(get_ch_status_cnf_ptr, &STK_response);
        return gstk_status;
      }
    }
  }

  /* total header length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  STK_response.num_bytes = STK_response.header.command_details.length +
                            STK_response.header.device_id.device_tag_length +
                            STK_response.header.result.result_tag_length + 6;

  if (get_ch_status_cnf_ptr->data_present)
  {
    int sum = 0;
    for(i=0;i<get_ch_status_cnf_ptr->ch_cnt;i++)
    {
      sum += STK_response.ch_status[i].ch_status_length +
                               2;
    }
    STK_response.num_bytes = int32touint8(sum + STK_response.num_bytes);
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL )
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info( UIM_TERMINAL_RESPONSE_F,
                                                      &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
    {
      ref_id = get_ch_status_cnf_ptr->cmd_details_ref_id;

      /* clear the command detail table reference slot before since we are about to reply to
        the card */
      gstk_util_cmd_details_ref_table_free_slot(get_ch_status_cnf_ptr->cmd_details_ref_id);

      /* set user_data */
      uim_cmd_ptr->hdr.user_data               = gstk_curr_inst_ptr->client_id;
      /* populate the terminal_response info */
      uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

      /* copy command details */
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 &(STK_response.header.command_details),
                                 offset,
                                 (size_t)(STK_response.header.command_details.length + 2),
                                 sizeof(uim_cmd_ptr->terminal_response.data),
                                 sizeof(STK_response.header.command_details)) <
         (size_t)(STK_response.header.command_details.length + 2))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      /* copy device */
      else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &(STK_response.header.device_id),
                                      offset +=
                                      (int)(STK_response.header.command_details.length + 2),
                                      (size_t)(STK_response.header.device_id.device_tag_length + 2),
                                      sizeof(uim_cmd_ptr->terminal_response.data),
                                      sizeof(STK_response.header.device_id)) <
              (size_t)(STK_response.header.device_id.device_tag_length + 2))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      /* copy result */
      else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &(STK_response.header.result),
                                      offset +=
                                      (int)(STK_response.header.device_id.device_tag_length + 2),
                                      3,
                                      sizeof(uim_cmd_ptr->terminal_response.data),
                                      sizeof(STK_response.header.result)) < 3)
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }

      offset = offset + 3;

      if (gstk_status == GSTK_SUCCESS &&
          ( STK_response.header.result.additional_info != NULL ) &&
          (STK_response.header.result.result_tag_length >= 1) &&
          ((size_t)offset <= (sizeof(uim_cmd_ptr->terminal_response.data)
                      - (STK_response.header.result.result_tag_length - 1))))
      {
        /* copy Additional Information */
        if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                     sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                     STK_response.header.result.additional_info,
                     (size_t)(STK_response.header.result.result_tag_length - 1)) <
           (size_t)(STK_response.header.result.result_tag_length - 1))
        {
          UIM_MSG_ERR_0("gstk_get_ch_status_cnf(): gstk_byte_offset_memcpy failed");
          gstk_status = GSTK_MEMORY_ERROR;
        }
        offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
      }

      /* Need to add channel status */
      if (gstk_status == GSTK_SUCCESS &&
          get_ch_status_cnf_ptr->data_present)
      {
        int j = 0;
        /* copy Channel Status */
        for(j=0; j<get_ch_status_cnf_ptr->ch_cnt; j++)
        {
          if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                     sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                     &STK_response.ch_status[j],
                     (size_t)(STK_response.ch_status[j].ch_status_length + 2)) <
             (size_t)(STK_response.ch_status[j].ch_status_length + 2))
          {
            UIM_MSG_ERR_0("gstk_get_ch_status_cnf(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
            break;
          }
          offset = (int)(offset + STK_response.ch_status[j].ch_status_length + 2);
        }
      }

      if (gstk_status == GSTK_SUCCESS)
      {
        gstk_util_dump_byte_array("Get Channel Status TR",
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
  } /* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_get_ch_status_cnf_cleanup(get_ch_status_cnf_ptr, &STK_response);
  return gstk_status;
} /* gstk_get_ch_status_cnf */
