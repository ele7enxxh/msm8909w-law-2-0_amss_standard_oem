/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   O P E N   C H A N N E L


GENERAL DESCRIPTION
  This source file contains functions to process select item
  proactive command


FUNCTIONS
  gstk_open_ch_req
    This function parses the Open Channel Command from the card
    and sends it to the client.

  gstk_open_ch_cnf
    This function packs the Open Channel terminal response and sends
    the command to UIM server.

  gstk_open_ch_cnf_cleanup
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_open_ch.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/06/16   gs      F3 logging improvements
10/12/15   vdc     Added support for informing MMGSDI about BIP and SMS-PP 
08/05/15   gs      Moved the SCWS flag to instance specific
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
10/08/14   gs      Check for the Open Channel Capability before checking for Sub Ready
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/31/14   dy      Fixing KW errors
07/18/14   gm      Update the is_scws_ch flag
07/11/14   shr     Add Support for NV Refresh
06/30/14   gm      Remove unnecessary clear of timer
06/18/14   gs      Handling open channel rejection properly
05/06/14   gs      GSTK Featurization enhancements and clean up
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/24/14   sw      Add support for BIP Status DIAG event
03/04/14   shr     Fix Subscription Ready caching logic
03/03/14   shr     ESTK BIP clean-up and enhancements
02/11/14   gm      Fix KW critical warning
01/20/14   sw      Slot to active data subscription mapping
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/03/14   sw      Update active slot during BIP open channel
12/05/13   shr     Added support for preferred activation over 1x
12/05/13   hn      Support DNS query and Channel reuse
12/1/13    sw      BIP Multi_slot changes
11/13/13   shr     Added support for BIP over WLAN
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
07/25/13   hn      Enable FEATURE_ESTK_BIP_SERVER_MODE, add control bitmask
05/23/13   hh      Fix KW errors
05/16/13   hh      Fix KW errors
04/09/13   vr      Use safer versions of memcpy() and memmove()
03/29/13   yt      Merge change to block BIP if OTASP is in progress
02/11/13   bd      Support for (e)HRPD access tech
01/29/13   hn      Added BIP status events
01/22/13   hn      Support 1xRTT CSIM programming retries
09/20/12   yt      Manage concurrency between BIP and OTASP
03/16/12   yt      Remove tmc.h inclusion
02/18/12   av      Cleaned up remaining ALWAYS OFF features
12/20/11   nb      GSDI Cleanup
07/18/11   dd      Fix regular BIP TR
07/13/11   dd      Fix extra bytes in SCWS Open Ch TR
04/24/11   dd      SCWS support
02/11/11   sg      Don't process open channel when LS not in normal service
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/17/10   xz      Decouple feature bitmask
12/13/10   adp     Check for Subscription Ready event only when
                   FEATURE_MMGSDI_SESSION_LIB  is defined by client
12/07/10   xz      Send error TR if no service or invalid RAT
11/24/10   sg      Fix freeing of stack variable
10/11/10   sg      Port IMSI_M enhancement and multiple OTASP calls
09/03/10   xz      Fix of compile error
08/30/10   xz      Use session API to read CSIM IMSI_M
08/19/10   xz      Block OPEN CH until MMGSDI_SUBSCRIPTION_READY_EVT
06/29/10   nb      Merged changes for DS DS
06/21/10   xz      Use correct feature mask
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
05/21/10   xz      Fix the issue of reading wrong IMSI_M file
04/21/10   xz      Fix compilation error
04/12/10   xz      Fix issue of getting wrong iface.
03/15/10   xz      Fix issue of profile 107
02/25/10   adp     Arm Lint Fixes
02/22/10   xz      Fix compilation error when IMSI_M feature is enabled
02/15/10   nb      Moved Dual Slot Changes
02/09/10   xz      IMSI_M provision enhancement
02/04/10   sg      Populate global for open channel timeout TR extra parameters
12/29/09   rn      Fixed klocwork errors.
09/26/09   kp      ZI memory reduction changes
04/01/09   gg      ME does not send Terminal rsp for Open Ch if no client is
                   registered (data card case)
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
08/25/06   sk      Corrected comments
06/24/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
01/05/05   tml     Fixed open channel terminal response error
11/04/04   sk/tml  Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Memory Header Files */
#include "uim_variation.h"
#include "memheap.h"
#include <stringl/stringl.h>

/* GSTK Header Files   */
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

/* Output & String Header Files */
#include "err.h"
#include "string.h"
#include "dssocket.h"
#include "mmgsdisessionlib.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_open_ch_cnf_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  open_ch_cnf_ptr: [Input] Pointer to message that is required to
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
static void gstk_open_ch_cnf_cleanup(
  gstk_open_ch_cnf_type           *open_ch_cnf_ptr,
  open_ch_terminal_response_type  *STK_response_ptr)
{
  uint32 i = 0;

  if((open_ch_cnf_ptr == NULL)||(STK_response_ptr == NULL))
  {
    UIM_MSG_ERR_2("ERR:NULL pointer: open_ch_cnf_ptr=0x%x STK_response_ptr=0x%x",
                  open_ch_cnf_ptr, STK_response_ptr);
    return;
  }

  /* free open channel tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)open_ch_cnf_ptr);/*lint !e826 area too small */
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info= NULL;
  }
  if (STK_response_ptr->bearer_description.bearer_param != NULL) {
    gstk_free(STK_response_ptr->bearer_description.bearer_param);
    STK_response_ptr->bearer_description.bearer_param = NULL;
  }
  if (STK_response_ptr->dns_addrs != NULL) {
    for (i = 0; i < STK_response_ptr->num_dns_addrs; ++ i)
    {
      if (STK_response_ptr->dns_addrs[i].addr != NULL)
      {
        gstk_free(STK_response_ptr->dns_addrs[i].addr);
        STK_response_ptr->dns_addrs[i].addr = NULL;
      }
    }
    gstk_free(STK_response_ptr->dns_addrs);
    STK_response_ptr->dns_addrs = NULL;
  }
} /* gstk_open_ch_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_open_ch_req_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  request_ptr : [Input] Pointer to gstk_cmd_from_card_type
  cmd_data_ptr: [Input] Pointer to cmd_data

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
static void gstk_open_ch_req_cleanup(
  gstk_cmd_from_card_type *request_ptr,
  uint8                   *cmd_data_ptr
)
{

  if(request_ptr == NULL)
  {
    UIM_MSG_ERR_0("request ERR:NULL");
    return;
  }

  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_open_ch_req_cleanup */

/*===========================================================================
FUNCTION gstk_open_ch_populate_other_info

DESCRIPTION
  This function populate the other info required to be sent to UICC in
  Terminal Response.

PARAMETERS
  open_ch_pro_cmd_req_ptr  : [Input]  OPEN CHANNEL command parsed data
  other_info_ptr           : [Output] Extra param for Terminal Response

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
static void gstk_open_ch_populate_other_info(
  const gstk_open_ch_req_type         *open_ch_pro_cmd_req_ptr,
  gstk_terminal_rsp_extra_param_type  *other_info_ptr
)
{
  if((open_ch_pro_cmd_req_ptr == NULL)||(other_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("ERR:NULL pointer open_ch_pro_cmd_req_ptr=0x%x other_info_ptr=0x%x",
                  open_ch_pro_cmd_req_ptr, other_info_ptr);
    return;
  }

  /* Copy Bearer Description*/
  (void)gstk_memcpy(&(other_info_ptr->extra_param.open_ch_extra_param.bearer_description),
                 &(open_ch_pro_cmd_req_ptr->bearer),
                 sizeof(gstk_bearer_description_type),
                 sizeof(gstk_bearer_description_type),
                 sizeof(gstk_bearer_description_type));

  /* Copy Buffer size */
  other_info_ptr->extra_param.open_ch_extra_param.buffer_size =
    open_ch_pro_cmd_req_ptr->buffer_size;

  if(open_ch_pro_cmd_req_ptr->transport_level.transport_protocol == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
  {
    other_info_ptr->extra_param.open_ch_extra_param.ch_status.is_scws_ch = TRUE;
  }
	
  other_info_ptr->rsp_type = GSTK_OPEN_CH_REQ;

  other_info_ptr->present = TRUE;
} /* gstk_open_ch_populate_other_info */

/*===========================================================================
FUNCTION gstk_open_ch_err_term_rsp

DESCRIPTION
  This function send terminal response in case of error.

PARAMETERS
  cmd_details_ref_id  : [Input] command details reference id
  result              : [Input] GSTK result
  additional_info_ptr : Pointer to additional info

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
static void gstk_open_ch_err_term_rsp(
  uint32                              cmd_details_ref_id,
  gstk_general_result_enum_type       result,
  const gstk_additional_info_type     *additional_info_ptr,
  gstk_terminal_rsp_extra_param_type  *other_info_ptr
)
{
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  gstk_curr_inst_ptr->otasp_blocked = FALSE;
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);

  /* Send Terminal Response to Card */
  /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
  gstk_status = gstk_send_terminal_response(
                  gstk_curr_inst_ptr->client_id,     /* client_id */
                  0,                  /* user_data */
                  cmd_details_ref_id, /* command_details_ref */
                  command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,  /* command_num */
                  GSTK_OPEN_CH_CNF,   /* command response */
                  result,             /* general_result */
                  additional_info_ptr,/* additional result */
                  other_info_ptr  /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) {
    UIM_MSG_ERR_1("TR fail, 0x%x", gstk_status);
  }
} /* gstk_open_ch_err_term_rsp */

/*===========================================================================
FUNCTION gstk_open_ch_req

DESCRIPTION
  This function parses the Open Channel Command from the card
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
gstk_status_enum_type gstk_open_ch_req (
  gstk_proactive_cmd_type* command_ptr
)
{
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  uint8                               *cmd_data_ptr        = NULL;
  int32                               cmd_data_len         = 0;
  uint32                              cmd_details_ref_id   = 0;
  int32                               curr_offset          = 0;
  int32                               new_offset           = 0;
  boolean                             more_tlv             = FALSE;
  gstk_additional_info_type           additional_info      = {0};
  boolean                             SIM_ME_Interface     = 0;
  gstk_terminal_rsp_extra_param_type  other_info;
  boolean                             is_scws_open_ch       = FALSE;
  boolean                             cmd_enable            = FALSE;
  boolean                             error_tr              = FALSE;
  gstk_general_result_enum_type       gen_result            = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  gstk_nv_items_data_type             nv_data;

  /* Cannot do {0x00} in declaration because of compilation warning
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  UIM_MSG_HIGH_1(" IN GSTK_OPEN_CH_REQ command_ptr=0x%x", command_ptr);

  if(command_ptr == NULL)
  {
    /*ERR: command_ptr is NULL*/
    return GSTK_BAD_PARAM;
  }

  /* initialize other_info */
  memset(&other_info, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));
  other_info.present = FALSE;
  
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
    gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                              GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                              &additional_info,
                              &other_info);
    /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
    gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }

  /* build the proactive cmd request into the gstk_exp.h _req structure */

  /* Message Header */
  request.hdr_cmd.command_id = GSTK_OPEN_CH_REQ;
  request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

  /* command number */
  request.hdr_cmd.command_number =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number;

  /* on demand verse imediate link establishment */
  /* Command details, qualifier tells us the priority */
  if(command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.qualifier & 0x01)
  {
    request.cmd.open_ch_pro_cmd_req.on_demand_link_est = FALSE;
  }
  else
  {
    request.cmd.open_ch_pro_cmd_req.on_demand_link_est  = TRUE;
  }
  UIM_MSG_HIGH_1(" on_demand_link_est = %d",
                 request.cmd.open_ch_pro_cmd_req.on_demand_link_est);

  /* Check if this OPEN CH requests DNS server addresses look-up */
  /* Command details, qualifier bit 4 (0x08) tells us if DNS look-up is requested */
  if(command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.qualifier & 0x08)
  {
    /* If DNS server is requested, 'immediate link establishment' bit must be on (0x01),
       'background mode' bit must be off (0x04) */
    if (!(command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.qualifier & 0x01) ||
        (command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.qualifier & 0x04))
    {
      UIM_MSG_ERR_0("command qualifiers: bit 4 enabled (DNS requested) but bit 1/3 incorrect!");
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
    }
    request.cmd.open_ch_pro_cmd_req.dns_requested = TRUE;
  }
  else
  {
    request.cmd.open_ch_pro_cmd_req.dns_requested  = FALSE;
  }
  UIM_MSG_HIGH_1(" dns_requested = %d",
                 request.cmd.open_ch_pro_cmd_req.dns_requested);

  /* optional alpha TLV */
  gstk_status = gstk_parser_alpha_identifier_tlv_ext( GSTK_ALPHA_IDENTIFIER_TAG,
                                                      cmd_data_ptr,
                                                      cmd_data_len,
                                                      curr_offset,
                                                      &(request.cmd.open_ch_pro_cmd_req.alpha),
                                                      &new_offset,
                                                      &more_tlv,
                                                      GSTK_OPTIONAL,
                                                      &cmd_details_ref_id);

  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                              GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                              &additional_info,
                              &other_info);
    /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
    gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
  else
  {
    if (!more_tlv)
    {
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return GSTK_INVALID_COMMAND;
    }
    curr_offset = new_offset;
  }

  /* optional ICON TLV */
  gstk_status = gstk_parser_icon_id_tlv_ext(
        &(request.cmd.open_ch_pro_cmd_req.alpha),
        cmd_data_ptr,
        cmd_data_len,
        curr_offset,
        &(request.cmd.open_ch_pro_cmd_req.icon),
        &new_offset,
        &more_tlv,
        GSTK_OPTIONAL,
        &cmd_details_ref_id);

  if(gstk_status != GSTK_SUCCESS)
  {
    gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                              GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                              &additional_info,
                              &other_info);
    /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
    gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
  else
  {
    if (!more_tlv)
    {
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return GSTK_INVALID_COMMAND;
    }
    curr_offset = new_offset;
  }

  /* Check if the tlv is a CS or GPRS command,
     GPRS should be followed with the bearer description param, CSD should be followed by address
     Currently CSD is not supported */
  if(!gstk_valid_tag(cmd_data_ptr[curr_offset],GSTK_BEARER_DESCRIPTION_TAG))
  {
    /* The open channel is not a GPRS open channel command */
    /* Check if it's a SCWS open channel command*/
    if(!gstk_valid_tag(cmd_data_ptr[curr_offset],GSTK_BUFFER_SIZE_TAG))
    {
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return GSTK_UNSUPPORTED_COMMAND;
    }
    else
    {
      is_scws_open_ch = TRUE;
      command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].proc_cmd_info.open_channel.is_scws_status = TRUE;  
    }
  }

  /* Continue with GPRS Open Channel command parsing */
  /* Bearer Description: Mandatory */
  if(!is_scws_open_ch)
  {
    gstk_status = gstk_parser_bearer_description_tlv( cmd_data_ptr,
                                                      cmd_data_len,
                                                      curr_offset,
                                                      &(request.cmd.open_ch_pro_cmd_req.bearer),
                                                      &new_offset,
                                                      &more_tlv,
                                                      GSTK_MANDATORY,
                                                      &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                               &additional_info,
                                &other_info);
      /* release memory allocation for text string if not null */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
    else
    {
      (void)memscpy(&gstk_curr_inst_ptr->gstk_curr_bearer_description,
      	     sizeof(gstk_bearer_description_type),
             &(request.cmd.open_ch_pro_cmd_req.bearer),
             sizeof(gstk_bearer_description_type));

      if (!more_tlv)
      {
        gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                  GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                  &additional_info,
                                  &other_info);
        /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
        gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
        return GSTK_INVALID_COMMAND;
      }
      curr_offset = new_offset;
    }
  }
  /* Buffer size: Mandatory */
  gstk_status = gstk_parser_buffer_size_tlv( cmd_data_ptr,
                                             cmd_data_len,
                                             curr_offset,
                                             &(request.cmd.open_ch_pro_cmd_req.buffer_size),
                                             &new_offset,
                                             &more_tlv,
                                             GSTK_MANDATORY,
                                             &cmd_details_ref_id);

  if(gstk_status != GSTK_SUCCESS)
  {
    gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                              GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                              &additional_info,
                              &other_info);
    /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
    gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
  else
  {
    gstk_curr_inst_ptr->gstk_curr_bearer_buffer_size =
                  request.cmd.open_ch_pro_cmd_req.buffer_size;
    curr_offset = new_offset;
  }

  if (more_tlv) {
    /* Access Point Name: Optional */
    gstk_status = gstk_parser_network_access_name_tlv(
                                           cmd_data_ptr,
                                           cmd_data_len,
                                           curr_offset,
                                           &(request.cmd.open_ch_pro_cmd_req.access_point_name),
                                           &new_offset,
                                           &more_tlv,
                                           GSTK_OPTIONAL,
                                           &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
    else
    {
      curr_offset = new_offset;
    }
  }

  if (more_tlv) {
    /* Other address (Local Address): Optional */
    gstk_status = gstk_parser_other_address_tlv(cmd_data_ptr,
                                           cmd_data_len,
                                           curr_offset,
                                           &(request.cmd.open_ch_pro_cmd_req.local_address),
                                           &new_offset,
                                           &more_tlv,
                                           GSTK_OPTIONAL,
                                           &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
    else
    {
      curr_offset = new_offset;
    }
  }

  if (more_tlv) {
    /* Text String (login): Optional */
    gstk_status = gstk_parser_text_string_tlv_ext(
                                           GSTK_TEXT_STRING_TAG,
                                           cmd_data_ptr,
                                           cmd_data_len,
                                           curr_offset,
                                           &(request.cmd.open_ch_pro_cmd_req.text_string_user_login),
                                           &new_offset,
                                           &more_tlv,
                                           GSTK_OPTIONAL,
                                           &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
    else
    {
      curr_offset = new_offset;
    }
  }

  if (more_tlv) {
    /* Text String (Password) : Optional */
    gstk_status = gstk_parser_text_string_tlv_ext(
                                           GSTK_TEXT_STRING_TAG,
                                           cmd_data_ptr,
                                           cmd_data_len,
                                           curr_offset,
                                           &(request.cmd.open_ch_pro_cmd_req.text_string_user_passwd),
                                           &new_offset,
                                           &more_tlv,
                                           GSTK_OPTIONAL,
                                           &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
    else
    {
      curr_offset = new_offset;
    }
  }

  if (more_tlv)
  {
    if(is_scws_open_ch)
    {
      gstk_status = gstk_parser_sim_me_interface_transport_level_tlv(cmd_data_ptr,
                                                                    cmd_data_len,
                                                                    curr_offset,
                                                                    &(request.cmd.open_ch_pro_cmd_req.transport_level),
                                                                    &new_offset,
                                                                    &more_tlv,
                                                                    GSTK_MANDATORY,
                                                                    &cmd_details_ref_id);
    }
    else
    {
      /* SIM as transport level: Optional */
      gstk_status = gstk_parser_sim_me_interface_transport_level_tlv(cmd_data_ptr,
                                                                      cmd_data_len,
                                                                      curr_offset,
                                                                      &(request.cmd.open_ch_pro_cmd_req.transport_level),
                                                                      &new_offset,
                                                                      &more_tlv,
                                                                      GSTK_OPTIONAL,
                                                                      &cmd_details_ref_id);
    }

    /* If DNS is requested, Transport Level TLV must not be present */
    if (request.cmd.open_ch_pro_cmd_req.dns_requested &&
        request.cmd.open_ch_pro_cmd_req.transport_level.transport_protocol != GSTK_PKT_DATA_NOT_PRESENT)
    {
      UIM_MSG_ERR_0("DNS requested but Transport Level TLV present!");
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for text string if not null */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }

    if(gstk_status != GSTK_SUCCESS)
    {
      SIM_ME_Interface = FALSE;
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                &additional_info,
                                &other_info);
      /* release memory allocation for text string if not null */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
    else
    {
      switch (request.cmd.open_ch_pro_cmd_req.transport_level.transport_protocol)
      {
      case GSTK_PKT_DATA_UDP:
      case GSTK_PKT_DATA_TCP:
        SIM_ME_Interface = TRUE;
        if (!more_tlv)
        {
          /* When SIM interface is presented, Data address is required */
          gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                    GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                    &additional_info,
                                    &other_info);
          /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
          gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
          return GSTK_INVALID_COMMAND;
        }
        break;
      default:
        SIM_ME_Interface = FALSE;
        UIM_MSG_HIGH_0("No SIM ME Interface transport");
        break;
      }
      curr_offset = new_offset;
    }
    /* Nothing further to process for SCWS open ch*/
    if(!is_scws_open_ch)
    {
    /* data destination address: Complementary */
      if (SIM_ME_Interface) {
        gstk_status = gstk_parser_other_address_tlv(cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                               &(request.cmd.open_ch_pro_cmd_req.data_dest_address),
                                                &new_offset,
                                                &more_tlv,
                                                GSTK_MANDATORY,
                                                &cmd_details_ref_id);
      }
      else
      {
        gstk_status = gstk_parser_other_address_tlv(cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                                &(request.cmd.open_ch_pro_cmd_req.data_dest_address),
                                                &new_offset,
                                                &more_tlv,
                                                GSTK_OPTIONAL,
                                                &cmd_details_ref_id);
      }

      /* If DNS is requested, Data Destination Address TLV must not be present */
      if (request.cmd.open_ch_pro_cmd_req.dns_requested &&
          request.cmd.open_ch_pro_cmd_req.data_dest_address.address_type != GSTK_PKT_DATA_NO_ADDRESS_GIVEN)
      {
        UIM_MSG_ERR_0("DNS requested but Data Destionation Address TLV present!");
        gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                  GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                  &additional_info,
                                  &other_info);
        /* release memory allocation for text string if not null */
        gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
        return gstk_status;
      }

      if(gstk_status != GSTK_SUCCESS)
      {
        gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                  GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                  &additional_info,
                                  &other_info);
        /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
        gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
        return gstk_status;
      }
      else
      {
        curr_offset = new_offset;
      }
    }
  }
  /* Nothing further to process for SCWS open ch*/
  if(!is_scws_open_ch)
  {
    if (more_tlv) {
      /* There are more tlvs after the last spec specified tlv, ignore */
    UIM_MSG_HIGH_0("Card Provide more TLV after final TLV element, Ignore");
    }

    /* If the command_id field is set to GSTK_OPEN_CH_CNF, it means we already
     attempted 1xRTT CSIM programming. We need to send back error Terminal
     Response immediately */
    if (command_ptr->message_header.command_id == GSTK_OPEN_CH_CNF &&
        (gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH,
           gstk_curr_inst_ptr->slot_id) == TRUE))
    {
      /* Send Terminal Response to Card */
      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

      gstk_open_ch_populate_other_info(&(request.cmd.open_ch_pro_cmd_req),
                                       &other_info);

      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return GSTK_SUCCESS;
    }
	
    gstk_status = gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_BIP, &cmd_enable);
    if (gstk_status != GSTK_SUCCESS || !cmd_enable)
    {
      UIM_MSG_HIGH_0("BIP Disabled, send error TR");
      error_tr = TRUE;
    }

    if(!(error_tr) && (is_scws_open_ch))
    {
      gstk_status = gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_UICC_SERVER_MODE, &cmd_enable);
      if (gstk_status != GSTK_SUCCESS || !cmd_enable)
      {
        UIM_MSG_HIGH_0("UICC Server Mode disabled, send error TR");
    	error_tr = TRUE;
      }
    }

    if (error_tr)
    {
      /* Send Terminal Response to Card */
      gstk_set_additional_info(
    	  &additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      gstk_open_ch_populate_other_info(&(request.cmd.open_ch_pro_cmd_req),
                                           &other_info);
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES,
                                &additional_info, &other_info);
    	/* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);

      return GSTK_SUCCESS;
    }

    /* If WLAN is not available, and SUBS_READY has not happened or Cellular network is
       not available, cannot process OPEN CHANNEL request */
    if((!gstk_shared_data.wlan_info.is_wlan_available) &&
       ((gstk_curr_inst_ptr->gstk_next_curr_location_status != GSTK_NORMAL_SERVICE) ||
        ((!gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw) &&
         (!gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x))))
    {
      UIM_MSG_HIGH_2("Drop open ch, Subs Ready GW: 0x%x, Subs Ready 1X: 0x%x",
                     gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw,
                     gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x);

     UIM_MSG_HIGH_2("Loc Status: 0x%x, WLAN available: 0x%x",
                    gstk_curr_inst_ptr->gstk_next_curr_location_status,
                    gstk_shared_data.wlan_info.is_wlan_available);

     /* Send Terminal Response to Card */
      gstk_set_additional_info(
        &additional_info, GSTK_NO_SERVICE_AVAILABLE);
      gstk_open_ch_populate_other_info(&(request.cmd.open_ch_pro_cmd_req),
                                       &other_info);
      gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                                GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                &additional_info,
                                &other_info);
      /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
      gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
      return GSTK_SUCCESS;
    }

    if((gstk_curr_inst_ptr->otasp_call_id >= 0) ||
       (gstk_shared_data.is_open_ch_ok == FALSE))
    {
      UIM_MSG_HIGH_2("IMSI_M unknown or OTASP in progress 0x%x, 0x%x - send error TR",
                     gstk_curr_inst_ptr->otasp_call_id,
                     gstk_shared_data.is_open_ch_ok);
      gstk_shared_data.is_open_ch_ok = TRUE;
      gstk_status = GSTK_ERROR;
    }

    (void)gstk_nv_access_read(
            GSTK_NV_SUB_BIP_OTASP_CONCURRENCY,
            gstk_curr_inst_ptr->slot_id,
            &nv_data);

    if (nv_data.bip_otasp_concurrency)
    {
      rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
      if (gstk_curr_inst_ptr->bip_blocked)
      {
        gstk_status = GSTK_ERROR;
      }
      else
      {
        gstk_curr_inst_ptr->otasp_blocked = TRUE;
      }
      rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
    }
  }

  if(gstk_status == GSTK_SUCCESS)
  {
    /* Send command to clients */
    gstk_status = gstk_util_send_message_to_clients(&request);
  }
  if(gstk_status != GSTK_SUCCESS)
  {
    /* Send Terminal Response to Card */
    UIM_MSG_HIGH_0("Open Channel not able to send to client");
    if(gstk_status == GSTK_NO_CLIENT_REG_FOUND)
    {
       gen_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
    }

    /* Send Terminal Response to Card */
    gstk_set_additional_info(
      &additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

    gstk_open_ch_populate_other_info(&(request.cmd.open_ch_pro_cmd_req),
                                     &other_info);

    gstk_open_ch_err_term_rsp(cmd_details_ref_id,
                              gen_result,
                              &additional_info,
                              &other_info);
  }
  else
  {
    /* Start Timer */
    gstk_util_start_client_response_timer(cmd_details_ref_id,
                                          GSTK_BIP_TERM_RSP_WAIT_TIME);
    /* reset gstk_status to SUCCESS since there has been no error*/
    /* This is needed since some of the TLVs are optional, which might
    ** cause the various parsing functions to return invalid_command */
    gstk_status = GSTK_SUCCESS;
  }

  /* release memory allocation for open ch request info data and cmd_data_ptr accordingly */
  gstk_open_ch_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);

} /* gstk_open_ch_req */

/*===========================================================================
FUNCTION gstk_open_ch_cnf

DESCRIPTION
  This function packs the open channel terminal response and sends
  the command to UIM server.

PARAMETERS
  open_ch_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_open_ch_cnf (
  gstk_open_ch_cnf_type* open_ch_cnf_ptr
)
{
  gstk_status_enum_type                  gstk_status  = GSTK_SUCCESS;
  open_ch_terminal_response_type         STK_response = {0};
  uim_cmd_type                           *uim_cmd_ptr = NULL;
  int                                    offset       = 0;
  uint32                                 ref_id       = 0;
  dss_iface_ioctl_bearer_tech_type       rat_tech;
  uint8                                  *addi_res    = NULL;
  gstk_cmd_from_card_type                terminal_response;
  uint8                                  dns_addrs_len = 0;
  gstk_data_address_type                 *dns_addrs_ptr = NULL;
  uint32                                 i             = 0;
  mmgsdi_return_enum_type                mmgsdi_status     = MMGSDI_ERROR;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  UIM_MSG_HIGH_0("IN GSTK open channel cnf ");

  if(open_ch_cnf_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  switch (open_ch_cnf_ptr->command_result)
  {
    case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
  /* Send out the BIP Status DIAG event indicating that BIP Session
     is in progress*/
    gstk_util_send_bip_status_diag_event(
      GSTK_BIP_SESSION_IN_PROGRESS,
      open_ch_cnf_ptr->ch_status.ch_id,
      open_ch_cnf_ptr->message_header.sim_slot_id);
        /* fall through */
    case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
    case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
    case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
    case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
      /* Inform MMGSDI of BIP session */
      mmgsdi_status = mmgsdi_start_potential_silent_file_change(
                        gstk_shared_data.gstk_mmgsdi_client_id,
                        (mmgsdi_slot_id_enum_type) gstk_curr_inst_ptr->slot_id);
      UIM_MSG_MED_1("mmgsdi_status: 0x%x", mmgsdi_status);
      break;
    default:
      /* No default action required at this time */
      break;
  }
  /* initialize terminal_response to 0 */
  memset(&terminal_response, 0x00, sizeof(gstk_cmd_from_card_type));

  terminal_response.hdr_cmd.command_id = GSTK_OPEN_CH_CNF;
  terminal_response.cmd.terminal_response.general_result = open_ch_cnf_ptr->command_result;
  terminal_response.cmd.terminal_response.additional_result.length = 0;
  if (open_ch_cnf_ptr->result_additional_info.length != 0)
  {
    (void)memscpy((void*)terminal_response.cmd.terminal_response.additional_result.additional_info,
    	   sizeof(terminal_response.cmd.terminal_response.additional_result.additional_info),
           open_ch_cnf_ptr->result_additional_info.additional_info_ptr,
           open_ch_cnf_ptr->result_additional_info.length);
    terminal_response.cmd.terminal_response.additional_result.length = open_ch_cnf_ptr->result_additional_info.length;
  }

  terminal_response.cmd.terminal_response.other_info.present = FALSE;
  if (open_ch_cnf_ptr->ch_status_present || open_ch_cnf_ptr->other_data_present) {
    terminal_response.cmd.terminal_response.other_info.present = TRUE;
    terminal_response.cmd.terminal_response.other_info.rsp_type = GSTK_OPEN_CH_CNF;
    terminal_response.cmd.terminal_response.other_info.extra_param.open_ch_extra_param.ch_status =
      open_ch_cnf_ptr->ch_status;
    terminal_response.cmd.terminal_response.other_info.extra_param.open_ch_extra_param.bearer_description =
      open_ch_cnf_ptr->bearer_description;
    terminal_response.cmd.terminal_response.other_info.extra_param.open_ch_extra_param.buffer_size =
      open_ch_cnf_ptr->buffer_size;
  }
  /* send TR to registered clients */
  gstk_util_send_tr_to_clients(&terminal_response);

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(open_ch_terminal_response_type));
  memset(&rat_tech, 0x00, sizeof(rat_tech));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
     open_ch_cnf_ptr->cmd_details_ref_id,
     open_ch_cnf_ptr->message_header.command_id,
     open_ch_cnf_ptr->command_number,
     &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_open_ch_cnf_cleanup(open_ch_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_open_ch_cnf_cleanup(open_ch_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  /* Check if IMSI_M feature is enabled and IMSI_M not yet provisioned*/
  if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH, gstk_curr_inst_ptr->slot_id) == TRUE)
  {
    if(gstk_util_get_rat_from_ds(&rat_tech))
    {
       UIM_MSG_HIGH_3("rat = 0x%x, rat_mask = 0x%x, loc_status = 0x%x",
                      rat_tech.current_network,
                      rat_tech.data.cdma_type.rat_mask,
                      gstk_curr_inst_ptr->gstk_next_curr_location_status);

      if (rat_tech.current_network == PS_IFACE_NETWORK_CDMA &&
          gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_NORMAL_SERVICE &&
          rat_tech.data.cdma_type.rat_mask == PS_IFACE_CDMA_1X &&
          gstk_shared_data.gstk_is_imsi_m_provisioned != TRUE)
      {
        UIM_MSG_HIGH_0("IMSI_M not provisioned - change result TLV");
        if(open_ch_cnf_ptr->command_result == GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR)
        {
          addi_res = (uint8 *)GSTK_CALLOC(sizeof(uint8));
          if(addi_res != NULL)
          {
            open_ch_cnf_ptr->command_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            *addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            open_ch_cnf_ptr->result_additional_info.additional_info_ptr = addi_res;
            open_ch_cnf_ptr->result_additional_info.length = 1;
          }
        }
      }
    }
    else /* Getting RAT form DS failed */
    {
      UIM_MSG_ERR_0("Getting RAT from DS failed");
    }
  }

  gstk_status = gstk_packer_result_tlv(
     open_ch_cnf_ptr->command_result,
     open_ch_cnf_ptr->result_additional_info,
     &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_open_ch_cnf_cleanup(open_ch_cnf_ptr, &STK_response);
    return gstk_status;
  }


  if (open_ch_cnf_ptr->ch_status_present) {
    /* Channel Status */
    gstk_status = gstk_packer_ch_status_tlv(
                &open_ch_cnf_ptr->ch_status,
                &STK_response.ch_status);
    if (gstk_status != GSTK_SUCCESS) {
      gstk_open_ch_cnf_cleanup(open_ch_cnf_ptr, &STK_response);
      return gstk_status;
    }
  }
  if (open_ch_cnf_ptr->other_data_present) {
    /* Bearer Description */
    if(!open_ch_cnf_ptr->is_scws_ch)
    {
      gstk_status = gstk_packer_bearer_description_tlv(
                    &open_ch_cnf_ptr->bearer_description,
                    &STK_response.bearer_description);
      if (gstk_status != GSTK_SUCCESS) {
        gstk_open_ch_cnf_cleanup(open_ch_cnf_ptr, &STK_response);
        return gstk_status;
      }
    }

    /* Buffer Size */
    gstk_status = gstk_packer_buffer_size_tlv(
                  open_ch_cnf_ptr->buffer_size,
                  &STK_response.buffer_size);
    if (gstk_status != GSTK_SUCCESS) {
      gstk_open_ch_cnf_cleanup(open_ch_cnf_ptr, &STK_response);
      return gstk_status;
    }

    if (open_ch_cnf_ptr->dns_addrs.num_addresses > 0)
    {
      dns_addrs_ptr = (gstk_data_address_type*)open_ch_cnf_ptr->dns_addrs.addresses;
      STK_response.dns_addrs =
        (gstk_dns_addr_tlv_type*)GSTK_CALLOC(open_ch_cnf_ptr->dns_addrs.num_addresses * sizeof(gstk_dns_addr_tlv_type));

      if (STK_response.dns_addrs == NULL)
      {
        return GSTK_MEMORY_ERROR;
      }

      STK_response.num_dns_addrs = open_ch_cnf_ptr->dns_addrs.num_addresses;
      for (i = 0; i < open_ch_cnf_ptr->dns_addrs.num_addresses; ++ i)
      {
        gstk_status = gstk_packer_dns_addr_tlv(
                      &dns_addrs_ptr[i],
                      &STK_response.dns_addrs[i]);
        if (gstk_status != GSTK_SUCCESS)
        {
          gstk_open_ch_cnf_cleanup(open_ch_cnf_ptr, &STK_response);
          return gstk_status;
        }
        dns_addrs_len = dns_addrs_len + STK_response.dns_addrs[i].length + 2;
      }
    }
  }
  /* total header length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
                      + DNS addresses (total length of all DNS TLVs)
  */
  STK_response.num_bytes = STK_response.header.command_details.length +
                            STK_response.header.device_id.device_tag_length +
                            STK_response.header.result.result_tag_length + 6 +
                            dns_addrs_len;

  if (open_ch_cnf_ptr->ch_status_present) {
    STK_response.num_bytes = STK_response.num_bytes +
                              STK_response.ch_status.ch_status_length + 2;
  }
  if (open_ch_cnf_ptr->other_data_present) {
    if(open_ch_cnf_ptr->is_scws_ch)
    {
      STK_response.num_bytes = STK_response.num_bytes +
                                STK_response.buffer_size.buffer_size_length + 2;
    }
    else
    {
      STK_response.num_bytes = STK_response.num_bytes +
                                STK_response.bearer_description.bearer_description_length +
                                STK_response.buffer_size.buffer_size_length + 4;
    }
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL )
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info( UIM_TERMINAL_RESPONSE_F,
                                                      &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS)
    {
      if (uim_cmd_ptr != NULL) {
        ref_id = open_ch_cnf_ptr->cmd_details_ref_id;

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(open_ch_cnf_ptr->cmd_details_ref_id);

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
            UIM_MSG_ERR_0("gstk_open_ch_cnf(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
          offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
        }

        /* Need to add channel status, bearer description and buffer size */
        if (gstk_status == GSTK_SUCCESS &&
            open_ch_cnf_ptr->ch_status_present)
        {
          /* copy Channel Status */
          if ((offset + STK_response.ch_status.ch_status_length + 1) < UIM_MAX_TR_BYTES)
          {
            if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                       sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                       &STK_response.ch_status,
                       (size_t)(STK_response.ch_status.ch_status_length + 2)) <
               (size_t)(STK_response.ch_status.ch_status_length + 2))
            {
              UIM_MSG_ERR_0("gstk_open_ch_cnf(): gstk_byte_offset_memcpy failed");
              gstk_status = GSTK_MEMORY_ERROR;
            }
            offset = (int)(offset + STK_response.ch_status.ch_status_length + 2);
          }
        }
        if (gstk_status == GSTK_SUCCESS &&
            open_ch_cnf_ptr->other_data_present)
        {
          /* copy Bearer Description */
          if(!open_ch_cnf_ptr->is_scws_ch)
          {
            if ((offset + 2) < UIM_MAX_TR_BYTES)
            {
              /* tag +  length + bearer type */
              if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                     sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                     &STK_response.bearer_description,
                         3) < 3)
              {
                gstk_status = GSTK_MEMORY_ERROR;
              }
              offset = offset + 3;
            }

            if (gstk_status == GSTK_SUCCESS &&
                STK_response.bearer_description.bearer_param != NULL)
            {
              if ((STK_response.bearer_description.bearer_description_length - 1 > 0)
                  &&
                  ((offset + STK_response.bearer_description.bearer_description_length - 2)
                   < UIM_MAX_TR_BYTES))
              {
                if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                           sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                           STK_response.bearer_description.bearer_param,
                           (size_t)(STK_response.bearer_description.bearer_description_length - 1)) <
                   (size_t)(STK_response.bearer_description.bearer_description_length - 1))
                {
                  UIM_MSG_ERR_0("gstk_open_ch_cnf(): gstk_byte_offset_memcpy failed");
                  gstk_status = GSTK_MEMORY_ERROR;
                }
                offset = (int)(offset + STK_response.bearer_description.bearer_description_length -1);
              }
            }
          }
          /* copy Buffer Size */
          if (gstk_status == GSTK_SUCCESS &&
              (offset + STK_response.buffer_size.buffer_size_length + 1) < UIM_MAX_TR_BYTES)
          {
            if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                   sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                   &STK_response.buffer_size,
                       (size_t)STK_response.buffer_size.buffer_size_length + 2) <
               (size_t)(STK_response.buffer_size.buffer_size_length + 2))
            {
              UIM_MSG_ERR_0("gstk_open_ch_cnf(): gstk_byte_offset_memcpy failed");
              gstk_status = GSTK_MEMORY_ERROR;
            }
            offset = offset + (int)STK_response.buffer_size.buffer_size_length + 2;
          }
          if (gstk_status == GSTK_SUCCESS &&
              dns_addrs_len > 0 &&
              (offset + dns_addrs_len - 1) < UIM_MAX_TR_BYTES)
          {
            for (i = 0; i < open_ch_cnf_ptr->dns_addrs.num_addresses; ++ i)
            {
              /* copy DNS IP address TLVs one-by-one */
              if (STK_response.dns_addrs[i].length >= 1)
              {
                /* tag, length and type */
                if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                              sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                              &STK_response.dns_addrs[i],
                              3) < 3)
                {
                  UIM_MSG_ERR_0("gstk_open_ch_cnf(): gstk_byte_offset_memcpy failed");
                  gstk_status = GSTK_MEMORY_ERROR;
                  break;
                }
                offset += 3;

                /* actual address */
                if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                           sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                           (uint8*)STK_response.dns_addrs[i].addr,
                           (size_t)(STK_response.dns_addrs[i].length - 1)) <
                   (size_t)(STK_response.dns_addrs[i].length - 1))
                {
                  UIM_MSG_ERR_0("gstk_open_ch_cnf(): gstk_byte_offset_memcpy failed");
                  gstk_status = GSTK_MEMORY_ERROR;
                  break;
                }
                offset = offset + (int)STK_response.dns_addrs[i].length - 1;
              }
              else
              {
                /* empty address */
                /* tag, length (== 0x00) */
                if(memscpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                           sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                           &STK_response.dns_addrs[i],
                           2) < 2)
                {
                  UIM_MSG_ERR_0("gstk_open_ch_cnf(): gstk_byte_offset_memcpy failed");
                  gstk_status = GSTK_MEMORY_ERROR;
                  break;
                }
                offset += 2;
              }
            }
          }
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_util_dump_byte_array("Open Channel TR",
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
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_open_ch_cnf_cleanup(open_ch_cnf_ptr, &STK_response);
  return gstk_status;
} /* gstk_open_ch_cnf */
