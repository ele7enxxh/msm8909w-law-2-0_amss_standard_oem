/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   R E F R E S H


GENERAL DESCRIPTION
  This source file contains functions to process the refresh proactive command


FUNCTIONS
  gstk_send_gsdi_refresh_req
    This function will send the GSDI a REFRESH Request.

  gstk_refresh_req
    This function parses the REFRESH Proactive Command.

  gstk_refresh_cnf
    This function packs the Refresh terminal response and sends
    the command to UIM server.

  gstk_refresh_cnf_cleanup
    This function cleans up any allocated pointer on the stack.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_refresh.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
04/27/15   vr      Non UI and non NW proactive command parallel processing
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
09/09/14   vr      Reduce stack usage
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/11/14   shr     Add Support for NV Refresh
05/20/14   dy      Fixed handling of apps_enabled in mmgsdi_session_changed
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/04/14   shr     Fix Subscription Ready caching logic
02/11/14   gm      Fix KW critical warning
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
12/16/13   bd      Added support for new CM SS event handling
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
07/15/13   hh      Reject Refresh if icon is received with empty or no alpha
07/01/13   hn      Send command_number, additional_info and other_info to ESTK
06/13/13   gm      Sending TR when UICC REFRESH RESET fail
06/11/13   hh      Fix Toolkit Compiling Errors
05/17/13   hn      Support for Alpha for Refresh
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/26/13   kb      Not sending TR for refresh UICC reset
02/21/13   hn      Enhancement of Loc Status and VPE retry
11/01/12   hk      Allow attach when subs ready received as result of refresh
02/28/12   xz      Allow ATTACH if REFRESH succeeds
02/18/12   av      Cleaned up remaining ALWAYS OFF features
12/20/11   nb      GSDI Cleanup
10/18/11   av      Removed features that are always OFF
04/25/11   bd      Fixed issue of proactive commands not being fetched
                   after REFRESH RESET
04/28/11   nk      Removing tmc.h include to resolve warning
04/12/11   bd      Fixed refresh cnf handling
02/23/11   nmb     Refresh Retry enhancements
01/21/11   nmb     Added refresh retry support
01/12/11   ssr     Fixed refresh request parsing
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
06/29/10   nb      Merged changes for DS DS
03/23/10   xz      Allow CSIM AID in REFRESH
03/15/10   xz      Pass correct slot ID when send REFRESH to MMGSDI
02/15/10   nb      Moved Dual Slot Changes
01/14/10   bd      Fixed REFRESH with alpha identifier set not understood by ME
01/21/10   nb      Fix for skip_tlv API
12/23/09   dd      Remove the timer from the refresh command
11/16/09   nb      Klockwork Fix
10/29/09   dd      Fix KW error
10/28/09   nb      Release 7 Enhanced Network Selection
10/18/09   dd      Forward raw refresh proactive message to gstk clients
09/29/09   kp      Fixed Lint errors
09/26/09   kp      ZI memory reduction changes
09/07/09   bd      Fix KW error
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
02/23/09   mib     FileList is mandatory for Init+FCN and 3G Session Reset
02/05/09   gg      Fixed error when AID TLV length is zero
02/04/09   mib     Ignore AID in refresh in case of SIM
02/04/09   mib     Fixed AID length passed to mmgsdi in case of refresh
09/14/08   sk      Fixed lint errors
09/08/08   sk      Fixed indentation issues
07/17/08   xz      1) replace mem_malloc() with gstk_malloc()
                   2) free refresh_files.data_ptr pointer in
                      gstk_send_mmgsdi_refresh_req() to avoid memory leak
03/17/08   sk      Fixed klockwork errors
02/06/08   sk      Lint fixes
11/30/07   sk      Added option parameter support to NAA refresh
11/07/07   sk      Added support for NAA Refresh
06/12/07   sp      Send error if tag is not valid and comprehension flag is set
                   while processing refresh request
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
03/26/07   sun     Send response with additional files read during refresh if
                   gsdi indicates so.
01/26/07   tml     Fixed not parsing AID issue
12/13/06   tml     Handle Refresh TR for AID not active or invalid AID
10/23/06   sk      Fixed lint errors.
10/19/06   sk      Fixed lint errors.
08/29/06   sk      Fixed lint error
08/28/06   sun     Lint Fixes
08/23/06   sun     Added support for NAA Refresh
06/24/06   sk      Lint fixes
05/11/06   tml     Properly convert GSTK refresh value to GSDI enum
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Fixed error fatal due to requst buffer not being
                   used and set
08/13/04   tml     Centralize release memory condition in the request function
                   and fix memory leak
08/03/04   tml     memory leak fix
05/26/04   tml     Added More debug messages
01/25/04   tml     Dual slot refresh support
07/07/03   tml     Refresh message fix
05/16/03   tml     linted
05/01/03   jar     Added handling for GSDI_REFRESH_IN_PROGRESS
03/06/03   tml     Update enum names
02/27/03 jar/tml   Map GSDI_PATHS_DECODE_ERROR to COMMAND_BEYOND_THE_ME_CAPABILITIES.
02/07/03   jar     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Task Definition Headers */

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

#ifdef FEATURE_ESTK
#include "estk.h"
#endif /* FEATURE_ESTK */

static gstk_status_enum_type gstk_send_mmgsdi_refresh_req(
  uint32                         refresh_mode,
  const gstk_file_list_type    * file_list_ptr,
  gstk_app_id_type               app_id,
  const gstk_generic_data_type * plmn_list_ptr,
  const gstk_string_type       * alpha,
  uint32                         cmd_details_ref_id);

/*===========================================================================
FUNCTION gstk_refresh_req_cleanup

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
static void gstk_refresh_req_cleanup(
  gstk_cmd_from_card_type *request_ptr,
  uint8                   *cmd_data_ptr
)
{
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_refresh_req_cleanup */

/*===========================================================================
FUNCTION gstk_refresh_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  refresh_cnf: [Input] Pointer to message that is required to
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
static void gstk_refresh_cnf_cleanup(
    gstk_refresh_cnf_type          *refresh_cnf,
    refresh_terminal_response_type *STK_response)
{
  if((refresh_cnf == NULL)||(STK_response == NULL))
  {
    UIM_MSG_ERR_2("ERR NULL pointer: refresh_cnf=0x%x, STK_response=0x%x",
                  refresh_cnf, STK_response);
    return;
  }
  /* free refresh tlv */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)refresh_cnf);/*lint !e826 area too small */
  if (STK_response->data.result.additional_info != NULL)
  {
    gstk_free(STK_response->data.result.additional_info);
    STK_response->data.result.additional_info = NULL;
  }
} /* gstk_refresh_cnf_cleanup */

/*lint +e715 app_id not used when FEATURE_NAA_REFRESH_NOT_SUPPORTED defined */
/*===========================================================================
FUNCTION gstk_refresh_req

DESCRIPTION
  This function parses the REFRESH Proactive Command.

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
  The functionality of this command centers around the following command
  qualifiers.

  00 -> SIM Initialization and Full File Change Notification
  01 -> File Change Notification
  02 -> SIM Initialization and File Change Notification
  03 -> SIM Initialization
  04 -> SIM Reset
  05 -> Application Reset, only applicable for a 3G platform
  06 -> Session Reset, only applicable for a 3G platform
  07 -> "Steering of Roaming" REFRESH support
  08 to FF -> reserved values

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_refresh_req (gstk_proactive_cmd_type* command)
{
  uint8                              *cmd_data = NULL;
  int32                               cmd_data_len;
  uint32                              cmd_details_ref_id;
  int32                               curr_offset = 0;
  int32                               next_tlv_offset = 0;
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
  gstk_additional_info_type           additional_info;
  gstk_terminal_rsp_extra_param_type  extra_param;
  boolean                             more_tlv = FALSE;
  uint8                               refresh_mode = 0xFF;
  boolean                             comprehension_flag = FALSE;
  tags_enum_type                      expected_tag = GSTK_NO_TAG;
  boolean                             file_list_tag_needed = FALSE;
  boolean                             plmnwact_list_tag_needed = FALSE;
  boolean                             has_alpha = FALSE;
  boolean                             tr_sent = FALSE;
  gstk_cmd_from_card_type            *request_ptr = NULL;
  uint32                              index = 0;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(command == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  request_ptr = (gstk_cmd_from_card_type *)GSTK_CALLOC(sizeof(gstk_cmd_from_card_type));
  if(request_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  /* initialize additional_info's length to 0  */
  additional_info.length = 0;

  /* initialize extra_param */
  extra_param.present = FALSE;

  /* Check and populate command details into the cmd_details_ref_table.
   ** Check Device ID validity put the rest of the command into the cmd_data,
   ** and specifies the length of the cmd_data in the cmd_data_len
   */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command,
                                              &cmd_data,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );
  index = GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id);

  if (gstk_status == GSTK_SUCCESS)
  {
    refresh_mode =
      command_details_ref_table_ptr[index].command_details.qualifier;
    if (refresh_mode == (int)GSTK_REFRESH_FILE_CHANGE_NOTIFICATION   ||
        refresh_mode == (int)GSTK_REFRESH_SIM_INIT_AND_FILE_CHANGE   ||
        refresh_mode == (int)GSTK_REFRESH_3G_SESSION_RESET)
    {
      file_list_tag_needed = TRUE;
    }
    else if (refresh_mode == (int)GSTK_REFRESH_STEERING_OF_ROAMING)
    {
      plmnwact_list_tag_needed = TRUE;
    }
  }
  /* we parsed all mandatory tags, now remaining are optional or conditional tag
  */
  if ((cmd_data != NULL) && (gstk_status == GSTK_SUCCESS))
  {
    more_tlv = TRUE;
    next_tlv_offset = 0;

    /*
      1) Known unexpected tag will ignore during parsing.
      2) Unknown tag without comprehension required bit will ignore during parsing.
      3) Unknown tag with comprehension required bit will break the parsing
         and return error.
      4) We will continue the parsing while we receiving the TAG in expected order,
         or until we reach the parsing end stage (NO_TAG expected stage).
      5) Any optional tag which comes in wrong sequence order will ignore.
      6) Refresh has following TLV order.
         File List, AID, Alpha ID, Icon ID, Txt Attr, Frame ID, PLMNwAcT List, No Tag
    */
    expected_tag = GSTK_FILE_LIST_TAG;
    while ((gstk_status == GSTK_SUCCESS)    &&
           (next_tlv_offset < cmd_data_len) &&
           (more_tlv == TRUE) && (expected_tag != GSTK_NO_TAG))
    {
      curr_offset = next_tlv_offset;
      comprehension_flag = gstk_is_tag_comprehension_req(cmd_data[curr_offset]);
      if(gstk_is_tag_known(cmd_data[curr_offset]))
      {
        UIM_MSG_HIGH_2("GSTK TAG=0x%x, file_list_tag_needed=%d",
                       (0x7F & cmd_data[curr_offset]), file_list_tag_needed );
        switch(0x7F & cmd_data[curr_offset])
        {
          case GSTK_FILE_LIST_TAG:
            if (expected_tag != GSTK_FILE_LIST_TAG)
            {
              UIM_MSG_HIGH_0("Not expecting GSTK_FILE_LIST_TAG");
              break;
            }
            UIM_MSG_HIGH_0("GSTK_FILE_LIST_TAG parsing");
            /* Start Parsing the File List Information */
            gstk_status =  gstk_parser_file_list_tlv( cmd_data,
                                                      cmd_data_len,
                                                      curr_offset,
                                                      &(request_ptr->cmd.refresh_pro_cmd_req.file_list),
                                                      &next_tlv_offset,
                                                      &more_tlv,
                                                      GSTK_OPTIONAL,
                                                      &cmd_details_ref_id);
            if(gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0("REFRESH TLV parsing error for GSTK_FILE_LIST_TAG");
              /* check if comprehension flag is set*/
              if(comprehension_flag)
              {
               /* Send Terminal Response to Card indicating
                   COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
                gstk_status = GSTK_INVALID_COMMAND;
                break;
              }/* if(comprehension_flag)*/
            }
            expected_tag = GSTK_AID_TAG;
            file_list_tag_needed = FALSE;
            break;

          case GSTK_AID_TAG:
            if (file_list_tag_needed)
            {
              gstk_status = GSTK_INVALID_COMMAND;
              break;
            }
            if (expected_tag == GSTK_FILE_LIST_TAG || expected_tag == GSTK_AID_TAG)
            {
              UIM_MSG_HIGH_0("GSTK_AID_TAG parsing");
              if(gstk_curr_inst_ptr->card_protocol == MMGSDI_UICC)
              {
                /* Start Parsing the AID Information */
                gstk_status = gstk_parser_aid_tlv( cmd_data,
                                                   cmd_data_len,
                                                   curr_offset,
                                                   &(request_ptr->cmd.refresh_pro_cmd_req.app_id),
                                                   &next_tlv_offset,
                                                   &more_tlv,
                                                   GSTK_OPTIONAL,
                                                   &cmd_details_ref_id);

                if(gstk_status != GSTK_SUCCESS)
                {
                  UIM_MSG_ERR_0("REFRESH TLV parsing errors for GSTK_AID_TAG");
                  /* reset AID value */
                  memset(&(request_ptr->cmd.refresh_pro_cmd_req.app_id), 0x00, sizeof(gstk_app_id_type));

                  /* check if comprehension flag is set*/
                  if(comprehension_flag)
                  {
                    /* Send Terminal Response to Card COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
                    gstk_status = GSTK_INVALID_COMMAND;
                  } /* if(comprehension_flag) */
                }
              }
              expected_tag = GSTK_ALPHA_IDENTIFIER_TAG;
            }
            break;

          case GSTK_ALPHA_IDENTIFIER_TAG:
            if (file_list_tag_needed)
            {
              gstk_status = GSTK_INVALID_COMMAND;
              break;
            }
            if (expected_tag == GSTK_FILE_LIST_TAG || expected_tag == GSTK_AID_TAG ||
                expected_tag == GSTK_ALPHA_IDENTIFIER_TAG)
            {
              gstk_status = gstk_parser_alpha_identifier_tlv(cmd_data,
                                                             cmd_data_len,
                                                             curr_offset,
                                                             &(request_ptr->cmd.refresh_pro_cmd_req.alpha),
                                                             &next_tlv_offset,
                                                             &more_tlv,
                                                             GSTK_OPTIONAL,
                                                             &cmd_details_ref_id);
              UIM_MSG_HIGH_1("GSTK_ALPHA_IDENTIFIER_TAG parsed: gstk_status=0x%x",
                             gstk_status);

              if (gstk_status != GSTK_SUCCESS) {
                if(comprehension_flag)
                {
                 /* Send Terminal Response to Card indicating
                     COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
                  gstk_status = GSTK_INVALID_COMMAND;
                  break;
                }/* if(comprehension_flag)*/
              }
              else if (request_ptr->cmd.refresh_pro_cmd_req.alpha.length > 1)
              {
                UIM_MSG_HIGH_0("GSTK_ALPHA_IDENTIFIER_TAG parsed: None Empty Alpha");
                has_alpha = TRUE;
              }

              expected_tag = GSTK_ICON_IDENTIFIER_TAG;
            }
            break;

          case GSTK_ICON_IDENTIFIER_TAG:
            if (file_list_tag_needed)
            {
              gstk_status = GSTK_INVALID_COMMAND;
              break;
            }
            if (expected_tag == GSTK_FILE_LIST_TAG || expected_tag == GSTK_AID_TAG ||
                expected_tag == GSTK_ALPHA_IDENTIFIER_TAG || expected_tag == GSTK_ICON_IDENTIFIER_TAG)
            {
              UIM_MSG_HIGH_1("GSTK_ICON_IDENTIFIER_TAG parsing, has_alpha=0x%x",
                             has_alpha);
              if (has_alpha)
              {
                gstk_status = gstk_parser_icon_id_tlv(&(request_ptr->cmd.refresh_pro_cmd_req.alpha),
                                                      cmd_data,
                                                      cmd_data_len,
                                                      curr_offset,
                                                      &(request_ptr->cmd.refresh_pro_cmd_req.icon),
                                                      &next_tlv_offset,
                                                      &more_tlv,
                                                      GSTK_OPTIONAL,
                                                      &cmd_details_ref_id);

                if (gstk_status != GSTK_SUCCESS) {
                  UIM_MSG_ERR_0("REFRESH TLV parsing errors for GSTK_ICON_IDENTIFIER_TAG");
                }
              }
              else
              {
                gstk_status = GSTK_INVALID_COMMAND;
                break;
              }

              expected_tag = GSTK_TEXT_ATTRIBUTE_TAG;
            }
            break;

          case GSTK_TEXT_ATTRIBUTE_TAG:
            if (file_list_tag_needed)
            {
              gstk_status = GSTK_INVALID_COMMAND;
              break;
            }
            if (expected_tag == GSTK_FILE_LIST_TAG || expected_tag == GSTK_AID_TAG ||
                expected_tag == GSTK_ALPHA_IDENTIFIER_TAG || expected_tag == GSTK_ICON_IDENTIFIER_TAG ||
                expected_tag == GSTK_TEXT_ATTRIBUTE_TAG )
            {
              UIM_MSG_HIGH_0("GSTK_TEXT_ATTRIBUTE_TAG parsing");
              /* We dont parse TEXT ATTRIBUTE, we skip it and update the parsing structures */
              expected_tag = GSTK_FRAME_IDENTIFIER_TAG;
            }
            break;

          case GSTK_FRAME_IDENTIFIER_TAG:
            if (file_list_tag_needed)
            {
              gstk_status = GSTK_INVALID_COMMAND;
              break;
            }
            if (expected_tag == GSTK_FILE_LIST_TAG || expected_tag == GSTK_AID_TAG ||
                expected_tag == GSTK_ALPHA_IDENTIFIER_TAG || expected_tag == GSTK_ICON_IDENTIFIER_TAG ||
                expected_tag == GSTK_TEXT_ATTRIBUTE_TAG || expected_tag == GSTK_FRAME_IDENTIFIER_TAG  )
            {
              UIM_MSG_HIGH_0("GSTK_FRAME_IDENTIFIER_TAG parsing");
              /* We dont parse the Frame Identifier, we skip it and update the parsing structures */
              expected_tag = GSTK_PLMNWACT_LIST_TAG;
            }
            break;

          case GSTK_PLMNWACT_LIST_TAG:
            if (file_list_tag_needed)
            {
              gstk_status = GSTK_INVALID_COMMAND;
              break;
            }
            if (expected_tag == GSTK_FILE_LIST_TAG || expected_tag == GSTK_AID_TAG ||
                expected_tag == GSTK_ALPHA_IDENTIFIER_TAG || expected_tag == GSTK_ICON_IDENTIFIER_TAG ||
                expected_tag == GSTK_TEXT_ATTRIBUTE_TAG || expected_tag == GSTK_FRAME_IDENTIFIER_TAG  ||
                expected_tag ==GSTK_PLMNWACT_LIST_TAG)
            {
              UIM_MSG_HIGH_0("GSTK_PLMNWACT_LIST_TAG parsing");
              /* Parse PLMNWACT List */
              gstk_status = gstk_parser_plmn_list_tlv( cmd_data,
                                                 cmd_data_len,
                                                 curr_offset,
                                                 &(request_ptr->cmd.refresh_pro_cmd_req.plmn_wact_list),
                                                 &next_tlv_offset,
                                                 &more_tlv,
                                                 GSTK_OPTIONAL,
                                                 &cmd_details_ref_id);
              if(gstk_status != GSTK_SUCCESS)
              {
                UIM_MSG_ERR_0("REFRESH TLV parsing errors for GSTK_PLMNWACT_LIST_TAG");
                /* check if comprehension flag is set*/
                if(comprehension_flag)
                {
                  /* Send Terminal Response to Card COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
                  gstk_status = GSTK_INVALID_COMMAND;
                } /* if(comprehension_flag) */
              }
              expected_tag = GSTK_NO_TAG;
              plmnwact_list_tag_needed = FALSE;
            }
            break;

          default:
            UIM_MSG_HIGH_1("Known unexpected tag 0x%x", cmd_data[curr_offset]);
            break;
        }
      }
      else
      {
        UIM_MSG_HIGH_1("Unknown tag found 0x%x", cmd_data[curr_offset]);
        if (comprehension_flag)
        {
          UIM_MSG_ERR_0("Unknown tag with comprehension req set");
          gstk_status = GSTK_INVALID_COMMAND;
        }
      }
      if ((curr_offset == next_tlv_offset) && (gstk_status == GSTK_SUCCESS))
      {
        /* Need to skip this Tag data */
        UIM_MSG_HIGH_0(" Skipping the current tag ");
        gstk_status =  gstk_parser_skip_tlv(cmd_data,
                                            cmd_data_len,
                                            curr_offset,
                                            &next_tlv_offset,
                                            &more_tlv);
        if(gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_1("Tag with wrong length 0x%x ", cmd_data[curr_offset]);
          gstk_status = GSTK_INVALID_COMMAND;
        }
      }
    }
  }
  UIM_MSG_HIGH_2("file_list_tag_needed=%d, plmnwact_list_tag_needed=%d",
                 file_list_tag_needed, plmnwact_list_tag_needed );
  if (file_list_tag_needed)
  {
    gstk_status = GSTK_INVALID_COMMAND;
  }
  if (plmnwact_list_tag_needed )
  {
    gstk_status = GSTK_INVALID_COMMAND;
  }

  if(gstk_status == GSTK_SUCCESS)
  {
    /* No parsing error, proceed to process REFRESH */
    if ((refresh_mode == (int)GSTK_REFRESH_FILE_CHANGE_NOTIFICATION) &&
        (request_ptr->cmd.refresh_pro_cmd_req.file_list.num_of_files == 0) )
    {
      UIM_MSG_HIGH_0("No files to be updated");

      tr_sent = TRUE;
      gstk_status = gstk_send_terminal_response(
                      gstk_curr_inst_ptr->client_id, /* client_id */
                      0,              /* user_data */
                      cmd_details_ref_id, /* command_details_ref */
                      command_details_ref_table_ptr[index].command_details.command_number,
                      /* command_num */
                      command_details_ref_table_ptr[index].expected_cmd_rsp_type,
                      /* command response */
                      GSTK_COMMAND_PERFORMED_SUCCESSFULLY, /* general_result */
                      &additional_info, /* additional result */
                      &extra_param /* extra param */ );

      if(gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_1("TR fail, 0x%x", gstk_status);
      }
    }
    else
    {
      /* Send Request to GSDI */
      gstk_status = gstk_send_mmgsdi_refresh_req(refresh_mode,
                                                 &(request_ptr->cmd.refresh_pro_cmd_req.file_list),
                                                 request_ptr->cmd.refresh_pro_cmd_req.app_id,
                                                 &(request_ptr->cmd.refresh_pro_cmd_req.plmn_wact_list),
                                                 &(request_ptr->cmd.refresh_pro_cmd_req.alpha),
                                                 cmd_details_ref_id);

      if ( gstk_status != GSTK_SUCCESS )
      {
        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        UIM_MSG_ERR_0("Got REFRESH TLV parsing errors");
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

        tr_sent = TRUE;
        gstk_status = gstk_send_terminal_response(
                        gstk_curr_inst_ptr->client_id, /* client_id */
                        0,              /* user_data */
                        cmd_details_ref_id, /* command_details_ref */
                        command_details_ref_table_ptr[index].command_details.command_number,
                        /* command_num */
                        command_details_ref_table_ptr[index].expected_cmd_rsp_type,
                        /* command response */
                        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
                        &additional_info, /* additional result */
                        &extra_param /* extra param */ );
        if(gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_1("TR fail, 0x%x", gstk_status);
        }
      } /* GSTK_SUCCESS != gstk_send_mmgsdi_refresh_req */
    }
  }
  else
  {
    /* Parsing Error */
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    UIM_MSG_ERR_0("Got REFRESH TLV parsing errors");

    tr_sent = TRUE;
    gstk_status = gstk_send_terminal_response(
                    gstk_curr_inst_ptr->client_id, /* client_id */
                    0,              /* user_data */
                    cmd_details_ref_id, /* command_details_ref */
                    command_details_ref_table_ptr[index].command_details.command_number,
                    /* command_num */
                    command_details_ref_table_ptr[index].expected_cmd_rsp_type,
                    /* command response */
                    GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                    &additional_info, /* additional result */
                    &extra_param /* extra param */ );

    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("TR fail, 0x%x", gstk_status);
    }
  }

  /* Only send alpha to client if there is error yet (no immediate TR sent) */
  if(!tr_sent)
  {
    request_ptr->hdr_cmd.command_id = GSTK_REFRESH_REQ;
    request_ptr->hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

    /* command number */
    request_ptr->hdr_cmd.command_number =
        command_details_ref_table_ptr[index].command_details.command_number;

    /* Send command to clients */
    gstk_status = gstk_util_send_message_to_clients(request_ptr);

    if(gstk_status != GSTK_SUCCESS)
    {
      /* Ignore failure to send alpha, move on to process REFRESH */
      UIM_MSG_HIGH_0("REFRESH send Alpha to client(s) failed");
    }
    else
    {
      /* Do not start timer for REFRESH request since MMGSDI already
         has a timer */
      /* reset gstk_status to SUCCESS since there has been no error*/
      /* This is needed since some of the TLVs are optional, which might
      ** cause the various parsing functions to return invalid_command */
      gstk_status = GSTK_SUCCESS;
    }
  }

  /* release memory allocation for refresh cmd */
  gstk_refresh_req_cleanup(request_ptr, cmd_data);
  gstk_free(request_ptr);
  request_ptr = NULL;
  return gstk_status;
} /* gstk_refresh_req */


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
gstk_status_enum_type gstk_refresh_cnf (
  gstk_refresh_cnf_type* refresh_cnf
)
{
  gstk_status_enum_type                     gstk_status = GSTK_SUCCESS;
  refresh_terminal_response_type            STK_response;
  uim_cmd_type                             *uim_cmd_ptr;
  int                                       offset = 0;

  UIM_MSG_HIGH_1("IN GSTK REFRESH CNF: refresh_cnf=0x%x", refresh_cnf);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(refresh_cnf == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0, sizeof(refresh_terminal_response_type));

  if (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
        gstk_curr_inst_ptr->slot_id) == TRUE)
  {
    if (refresh_cnf->command_result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY)
    {
      gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED;
    }
    else
    {
      gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_INVALID;
      memset(gstk_curr_inst_ptr->gstk_last_loc_info_sent,
             0xFF,
             sizeof(gstk_curr_inst_ptr->gstk_last_loc_info_sent));
    }
  }

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
     refresh_cnf->cmd_details_ref_id,
     refresh_cnf->message_header.command_id,
     refresh_cnf->command_number,
     &STK_response.data.command_details );

  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_refresh_cnf_cleanup(refresh_cnf, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_response.data.device_id );


  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_refresh_cnf_cleanup(refresh_cnf, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
     refresh_cnf->command_result,
     refresh_cnf->result_additional_info,
     &STK_response.data.result);

  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_refresh_cnf_cleanup(refresh_cnf, &STK_response);
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
  if (uim_cmd_ptr != NULL )
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
    {
      /*
      ** Clear the command detail table
      ** reference slot, we are about to
      ** reply to the card
      */
      gstk_util_cmd_details_ref_table_free_slot(refresh_cnf->cmd_details_ref_id);

      /* Set user_data */
      uim_cmd_ptr->hdr.user_data               = gstk_curr_inst_ptr->client_id;
      /* populate the terminal_response info */
      uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

      /* Copy command details */
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 &(STK_response.data.command_details),
                                 offset,
                                 (size_t)(STK_response.data.command_details.length + 2),
                                 sizeof(uim_cmd_ptr->terminal_response.data),
                                 sizeof(STK_response.data.command_details)) <
         (size_t)(STK_response.data.command_details.length + 2))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      /* Copy device identities*/
      else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &(STK_response.data.device_id),
                                      offset +=
                                      (int)(STK_response.data.command_details.length + 2),
                                      (size_t)(STK_response.data.device_id.device_tag_length + 2),
                                      sizeof(uim_cmd_ptr->terminal_response.data),
                                      sizeof(STK_response.data.device_id)) <
              (size_t)(STK_response.data.device_id.device_tag_length + 2))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      /* Copy result */
      else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &(STK_response.data.result),
                                      offset +=
                                      (int)(STK_response.data.device_id.device_tag_length + 2),
                                      3,
                                      sizeof(uim_cmd_ptr->terminal_response.data),
                                      sizeof(STK_response.data.result)) < 3)
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }

      offset += 3;
      /* copy additional info */
      if (gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                       STK_response.data.result.additional_info,
                                       offset,
                                       (size_t)(STK_response.data.result.result_tag_length - 1),
                                       sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                                       (size_t)(STK_response.data.result.result_tag_length - 1)) <
               (size_t)(STK_response.data.result.result_tag_length - 1))
      {
        UIM_MSG_ERR_0("gstk_refresh_cnf(): gstk_byte_offset_memcpy failed");
        gstk_status = GSTK_MEMORY_ERROR;
      }

      offset = (int)(offset + STK_response.data.result.result_tag_length - 1);

      if (gstk_status == GSTK_SUCCESS)
      {
        gstk_util_dump_byte_array("Refresh TR",
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
    else
    {
      UIM_MSG_HIGH_0("Never here");
    }
  }
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_refresh_cnf_cleanup(refresh_cnf, &STK_response);
  return gstk_status;
} /* gstk_refresh_cnf */

/*===========================================================================
FUNCTION gstk_handle_mmgsdi_refresh_cnf

DESCRIPTION
  This is the function that is called from gstk callback invoked by  MMGSDI to
  process refresh cnf.

PARAMETERS
  mmgsdi_return_enum_type  mmgsdi_status - MMGSDI status
  mmgsdi_refresh_cnf_type *refresh_cnf   - Refresh conf

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
gstk_status_enum_type gstk_handle_mmgsdi_refresh_cnf (
  mmgsdi_return_enum_type        mmgsdi_status,
  const mmgsdi_refresh_cnf_type *refresh_cnf_ptr
)
{
  gstk_additional_info_type           additional_info      = {0};
  gstk_terminal_rsp_extra_param_type  extra_param          = {0};
  uint32                              cmd_details_ref_id   = 0;
  gstk_general_result_enum_type       me_response          = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  gstk_client_id_type                 client_id            = 0;
#ifdef FEATURE_ESTK
  estk_cmd_type                       *task_cmd_ptr        = NULL;
#else
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
#endif /* FEATURE_ESTK */
  uint32                              index                = 0;

  UIM_MSG_HIGH_1("IN GSTK / MMGSDI REFRESH CNF: refresh_cnf_ptr=0x%x",
                 refresh_cnf_ptr);

  if(refresh_cnf_ptr == NULL)
  {
    return GSTK_ERROR;
  }

  extra_param.present     = FALSE;
  additional_info.length  = 0;
  UIM_MSG_HIGH_1("mmgsdi_status=0x%x", mmgsdi_status);
  switch (mmgsdi_status)
  {
  case MMGSDI_SUCCESS:
  case MMGSDI_REFRESH_IN_PROGRESS:
    /* Do Nothing */
    break;
  case MMGSDI_NOT_SUPPORTED:
  case MMGSDI_PATHS_DECODE_ERROR:
    me_response = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
    break;
  case MMGSDI_ERROR:
  case MMGSDI_CAN_NOT_REFRESH:
    me_response = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    gstk_set_additional_info(&additional_info,GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
    break;
  case MMGSDI_REFRESH_SUCCESS:
    me_response = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    break;
  case MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE:
    me_response = GSTK_REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE;
    break;
  case MMGSDI_REFRESH_FAIL_INVALID_AID:
    me_response = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    break;
  case MMGSDI_REFRESH_ADDITIONAL_FILES_READ:
    me_response = GSTK_PCMD_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ;
    break;
  case MMGSDI_REFRESH_LATER:
    me_response = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    gstk_set_additional_info(&additional_info, GSTK_ME_CURRENTLY_BUSY_ON_CALL);
    break;
  default:
    UIM_MSG_ERR_1("Unknown MMGSDI Status %x, GSTK ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND",
                  mmgsdi_status);
    me_response = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    gstk_set_additional_info(&additional_info,GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
    break;
  }

  /* Need to send back a Terminal Response indicating the status of the Refresh */
  cmd_details_ref_id = refresh_cnf_ptr->response_header.client_data;
  index = GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id);

  /* Find right client id of GSTK for the slot that has sent REFRESH command */
  for (client_id = 0; client_id < GSTK_MAX_CLIENT; ++client_id )
  {
    if ((gstk_shared_data.gstk_client_table[client_id].sim_slot_id ==
        (gstk_slot_id_enum_type)refresh_cnf_ptr->response_header.slot_id) &&
        (gstk_shared_data.gstk_client_table[client_id].client_type ==
        GSTK_GSTK_TYPE))
    {
      break;
    }
  }
  if (client_id == GSTK_MAX_CLIENT)
  {
    UIM_MSG_ERR_0("failed to send refresh cnf. can't find slot!");
    return GSTK_ERROR;
  }

  if (command_details_ref_table_ptr[index].command_details.type_of_command == GSTK_CMD_STK_REFRESH &&
       (command_details_ref_table_ptr[index].command_details.qualifier != GSTK_SIM_REFRESH_RESET ||
       (mmgsdi_status != MMGSDI_REFRESH_SUCCESS  && mmgsdi_status != MMGSDI_SUCCESS)))
  {
#ifdef FEATURE_ESTK
    if (GSTK_IDLE_S_RECOVERY((gstk_slot_id_enum_type)
                         refresh_cnf_ptr->response_header.slot_id))
    {
      UIM_MSG_ERR_0("gstk_handle_mmgsdi_refresh_cnf: GSTK_IDLE_S_RECOVERY");
      return GSTK_ERROR;
    }
    /* get the command from buffer */
    task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
    if (task_cmd_ptr == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }
    task_cmd_ptr->hdr.cmd_ref_id               = cmd_details_ref_id;
    task_cmd_ptr->hdr.cli_user_data            = 0;
    task_cmd_ptr->hdr.cmd                      = ESTK_REFRESH_CNF_CMD;
    task_cmd_ptr->hdr.sim_slot_id              = (gstk_slot_id_enum_type)refresh_cnf_ptr->response_header.slot_id;
    task_cmd_ptr->hdr.command_number           = command_details_ref_table_ptr[index].command_details.command_number;
    task_cmd_ptr->result                       = ESTK_SUCCESS;
    task_cmd_ptr->cmd_data.refresh_cnf.refresh_result  = me_response;

    task_cmd_ptr->cmd_data.refresh_cnf.other_info_ptr =
      (gstk_terminal_rsp_extra_param_type*)GSTK_CALLOC(sizeof(gstk_terminal_rsp_extra_param_type));

    task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr =
      (gstk_additional_info_ptr_type*)GSTK_CALLOC(sizeof(gstk_additional_info_ptr_type));
    if (task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr && additional_info.length > 0)
    {
      task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr =
        (uint8*)GSTK_CALLOC(additional_info.length);
    }

    if (!task_cmd_ptr->cmd_data.refresh_cnf.other_info_ptr ||
        !task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr ||
        (additional_info.length > 0 && !task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr))
    {
      if (task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr)
      {
        if (task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr)
        {
          gstk_free(task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr);
        }
        task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr = NULL;
        gstk_free(task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr);
      }
      task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr = NULL;
      if (task_cmd_ptr->cmd_data.refresh_cnf.other_info_ptr)
      {
        gstk_free(task_cmd_ptr->cmd_data.refresh_cnf.other_info_ptr);
      }
      task_cmd_ptr->cmd_data.refresh_cnf.other_info_ptr = NULL;
      return GSTK_ERROR;
    }

    task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->length = additional_info.length;
    if (additional_info.length > 0)
    {
      (void)memscpy(task_cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr,
                    additional_info.length,
                    additional_info.additional_info,
                    additional_info.length);
    }
    (void)memscpy(task_cmd_ptr->cmd_data.refresh_cnf.other_info_ptr,
                  sizeof(gstk_terminal_rsp_extra_param_type),
                  &extra_param,
                  sizeof(extra_param));
    /* Ignore return value for estk_put_cmd() as the function returns error
    ** for a null task_cmd_ptr which has already been checked for in this function
    */
    UIM_MSG_HIGH_0("Sending MMGSDI Refresh result to ESTK");
    estk_put_cmd(task_cmd_ptr);
    return GSTK_SUCCESS;
#else
    gstk_status = gstk_send_terminal_response(
                    client_id + 1, /* client_id */
                    0,                  /* user_data */
                    cmd_details_ref_id, /* command_details_ref */
                    command_details_ref_table_ptr[index].command_details.command_number,
                                        /* command_num */
                    command_details_ref_table_ptr[index].expected_cmd_rsp_type,
                    /* command response: poll interval or polling off */
                    me_response,        /* general_result */
                    &additional_info,   /* additional result */
                    &extra_param);      /* extra param */
    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Unable to send Terminal Response, 0x%x", gstk_status);
      return GSTK_ERROR;
    }
#endif /* FEATURE_ESTK */
  }

  return GSTK_SUCCESS;
}/* gstk_handle_mmgsdi_refresh_cnf */

/*===========================================================================
FUNCTION gstk_send_mmgsdi_refresh_req

DESCRIPTION
  This function will send a REFRESH Request to MMGSDI

PARAMETERS
  refresh_mode        [Input] : Refresh mode
  file_list_ptr       [Input] : File List
  app_id              [Input] : App id type
  plmn_list_ptr       [Input] : PLMN List
  cmd_details_ref_id  [Input] : GSTK reference data

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:  GSTK_SUCCESS:  Indicates that Heap Space was properly allocated
                                         for the request.  Also indicates the request was
                                         properly queued onto MMGSDI.
                          GSTK_ERROR:    Indicates that either Heap Space was not
                                         available or that the Message was not properly
                                         queued onto GSDI.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_send_mmgsdi_refresh_req(
  uint32                         refresh_mode,
  const gstk_file_list_type    * file_list_ptr,
  gstk_app_id_type               app_id,
  const gstk_generic_data_type * plmn_list_ptr,
  const gstk_string_type       * alpha,
  uint32                         cmd_details_ref_id)
{
  mmgsdi_return_enum_type       mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_refresh_mode_enum_type mmgsdi_refresh_mode = MMGSDI_REFRESH_MODE_ENUM_MAX ;
  mmgsdi_data_type              refresh_files       = {0};
  mmgsdi_data_type              plmn_list;
  mmgsdi_aid_type               aid;
  mmgsdi_option_type            option;

  (void)alpha;

  if ( file_list_ptr == NULL)
  {
    UIM_MSG_ERR_0("File List is NULL");
    return GSTK_ERROR;
  }

  /* Initialize aid */
  aid.app_type = MMGSDI_APP_NONE;
  memset(&aid.aid,0x00,sizeof(mmgsdi_static_data_type));
  memset(&aid.label,0x00,sizeof(mmgsdi_static_data_type));

  /* Initialize option */
  memset(&option,0x00,sizeof(mmgsdi_option_type));
  memset(&plmn_list, 0x00, sizeof(mmgsdi_data_type));

  if(file_list_ptr->num_of_files > 0)
  {
    refresh_files.data_len = file_list_ptr->length;
    if((file_list_ptr->length == 0) || (file_list_ptr->file_paths == NULL))
    {
      UIM_MSG_ERR_0("File List is empty");
      return GSTK_ERROR;
    }
    refresh_files.data_ptr = (uint8*)GSTK_CALLOC(sizeof(uint8) * file_list_ptr->length);
    if(refresh_files.data_ptr == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }
    if(gstk_memcpy((void*)refresh_files.data_ptr,
                   (void*)file_list_ptr->file_paths,
                   sizeof(uint8) * file_list_ptr->length,
                   sizeof(uint8) * (int32touint32(refresh_files.data_len)),
                   sizeof(uint8) * file_list_ptr->length) <
       (sizeof(uint8) * file_list_ptr->length))
    {
      gstk_free(refresh_files.data_ptr);
      return GSTK_MEMORY_ERROR;
    }
  }

  if(gstk_memcpy(aid.aid.data_ptr,
                 app_id.aid_data,
                 app_id.aid_len,
                 sizeof(aid.aid.data_ptr),
                 sizeof(app_id.aid_data)) <
     app_id.aid_len)
  {
    gstk_free(refresh_files.data_ptr);
    return GSTK_MEMORY_ERROR;
  }
  aid.aid.data_len = (mmgsdi_len_type)app_id.aid_len;

  if(plmn_list_ptr->data_len > 0)
  {
    plmn_list.data_len = plmn_list_ptr->data_len;
    plmn_list.data_ptr = (uint8 *)GSTK_CALLOC(plmn_list_ptr->data_len);

    if(plmn_list.data_ptr == NULL)
    {
      if(refresh_files.data_ptr != NULL)
      {
        gstk_free(refresh_files.data_ptr);
      }
      return GSTK_MEMORY_ERROR;
    }

    if(gstk_memcpy((void*)plmn_list.data_ptr,
                   (void*)plmn_list_ptr->data_buffer_ptr,
                   sizeof(uint8) * plmn_list_ptr->data_len,
                   sizeof(uint8) * (int32touint32(plmn_list.data_len)),
                   sizeof(uint8) * plmn_list_ptr->data_len) <
       (sizeof(uint8) * plmn_list_ptr->data_len))
    {
      gstk_free(refresh_files.data_ptr);
      gstk_free(plmn_list.data_ptr);
      return GSTK_MEMORY_ERROR;
    }
  }

  switch (refresh_mode)
  {
  case 0:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT_FULL_FCN;
    break;
  case 1:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_FCN;
    break;
  case 2:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT_FCN;
    break;
  case 3:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT;
    break;
  case 4:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_RESET;
    break;
  case 5:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_APP_RESET;
    break;
  case 6:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_3G_SESSION_RESET;
    break;
  case 7:
    mmgsdi_refresh_mode = MMGSDI_REFRESH_STEERING_OF_ROAMING;
    break;
  default:
    UIM_MSG_ERR_1("Unknown refresh mode 0x%x", refresh_mode);
    if(refresh_files.data_ptr != NULL)
    {
      gstk_free(refresh_files.data_ptr);
    }

    if(plmn_list.data_ptr != NULL)
    {
      gstk_free(plmn_list.data_ptr);
    }
    return GSTK_ERROR;
  }

  UIM_MSG_HIGH_0("SENDING REFRESH REQ TO MMGSDI ...");
  if(mmgsdi_refresh_mode != MMGSDI_REFRESH_STEERING_OF_ROAMING)
  {
    mmgsdi_status = mmgsdi_naa_refresh(gstk_shared_data.gstk_mmgsdi_client_id,
                                       (mmgsdi_slot_id_enum_type)gstk_curr_inst_ptr->slot_id,
                                       aid,
                                       mmgsdi_refresh_mode,
                                       refresh_files,
                                       file_list_ptr->num_of_files,
                                       NULL, /* TODO : convert alpha to mmgsdi_alpha_string_type */
                                       option,
                                       gstk_mmgsdi_cmd_cb,
                                       cmd_details_ref_id);
  }
  else
  {
    mmgsdi_status = mmgsdi_naa_refresh(gstk_shared_data.gstk_mmgsdi_client_id,
                                       (mmgsdi_slot_id_enum_type)gstk_curr_inst_ptr->slot_id,
                                       aid,
                                       mmgsdi_refresh_mode,
                                       plmn_list,
                                       0,
                                       NULL, /* TODO : convert alpha to mmgsdi_alpha_string_type */
                                       option,
                                       gstk_mmgsdi_cmd_cb,
                                       cmd_details_ref_id);
  }

  if(refresh_files.data_ptr != NULL)
  {
    gstk_free(refresh_files.data_ptr);
  }

  if(plmn_list.data_ptr != NULL)
  {
    gstk_free(plmn_list.data_ptr);
  }

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    UIM_MSG_ERR_0("COULD NOT SEND REFRESH");
    return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_send_mmgsdi_refresh_req */

