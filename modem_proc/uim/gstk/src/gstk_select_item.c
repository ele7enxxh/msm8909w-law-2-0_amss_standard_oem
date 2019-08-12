/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E L E C T    I T E M


GENERAL DESCRIPTION
  This source file contains functions to process select item
  proactive command


FUNCTIONS
  gstk_select_item_cmd_details
    This function will check the command details stored in the
    Global GSTK Command Details Reference Table.

  gstk_sel_item_populate_alpha_identifier
    This function will parse cmd_data and populate the first item
    in this select item type.

  gstk_select_item_req
    This function parses the Select item Command from the card
    and sends it to the client.

  gstk_select_item_cnf
    This function packs the select item terminal response and sends
    the command to UIM server.

  gstk_select_item_cnf_cleanup
    This function cleans up any allocated pointer on the stack.



INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2010, 2012-2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_select_item.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
06/30/14   gm      Remove unnecessary clear of timer
05/08/14   dy      Fix KW errors
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
02/11/14   gm      Fix KW critical warning
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/13/13   shr     Added support for BIP over WLAN
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
04/09/13   vr      Use safer versions of memcpy() and memmove()
03/16/12   yt      Remove tmc.h inclusion
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
02/26/10   yt      Fixed KW errors 
02/15/10   nb      Moved Dual Slot Changes
11/06/09   dd      Check if the number of menu items exceeds the limit
09/26/09   kp      ZI memory reduction changes
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
05/03/06   tml     lint
11/15/05   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
09/01/05   tml     Fix Select Item default item fix
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/13/04   tml     Centralize release memory condition in the request function
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Memory leak fix
05/26/04   tml     Ensure successfully send TR to card when parsing error
                   occurs.  Added More debug messages
09/26/03   tml     Initialize extra param and additional info
05/16/03   tml     linted
04/29/03   tml     Initialized the structure to zero when first created
03/19/03   tml     Perform checking on item chosen data present before copying
                   to uim buffer
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
02/07/03   jar     Initial Version

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
#include <stringl/stringl.h>
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_select_item_req_cleanup

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
static void gstk_select_item_req_cleanup(
  gstk_cmd_from_card_type *request_ptr,
  uint8                   *cmd_data_ptr)
{
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_select_item_req_cleanup */

/*===========================================================================
FUNCTION gstk_select_item_cnf_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  select_item_cnf_ptr: [Input] Pointer to message that is required to
                               be processed and sent to UIM
  STK_response_ptr   : [Input] Pointer to the STK intermediate message

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
static void gstk_select_item_cnf_cleanup(
  gstk_select_item_cnf_type           *select_item_cnf_ptr,
  select_item_terminal_response_type  *STK_response_ptr)
{
  if((STK_response_ptr == NULL)||(select_item_cnf_ptr == NULL))
  {
    UIM_MSG_ERR_2("ERR NULL pointer: STK_response_ptr=0x%x select_item_cnf_ptr=0x%x",
                  STK_response_ptr, select_item_cnf_ptr);
    return;
  }

  /* free select item tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)select_item_cnf_ptr);/*lint !e826 area too small */
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
      STK_response_ptr->header.result.additional_info= NULL;
  }
} /* gstk_select_item_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_select_item_cmd_details

DESCRIPTION
  This function will check the command details stored in the
  Global GSTK Command Details Reference Table.

  From this information, it will be determined whether there is
  a preferred MENU Selection.  It will also be determined whether
  there is help available.

PARAMETERS
  request_ptr           : [Input] request pointer
  cmd_details_ref_id_ptr: [Input] Pointer to command details reference id

DEPENDENCIES
  command_details_ref_table

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_select_item_cmd_details(
  gstk_cmd_from_card_type *request_ptr,
  const uint32            *cmd_details_ref_id_ptr
)
{
  boolean                   presentation_specified    = FALSE;
  boolean                   present_as_choice_of_nav  = FALSE;

  if((request_ptr == NULL)||(cmd_details_ref_id_ptr == NULL))
  {
    UIM_MSG_ERR_2("ERR NULL pointer: request_ptr=0x%x cmd_details_ref_id_ptr=0x%x",
                  request_ptr, cmd_details_ref_id_ptr);
    return;
  }

  /* Need to Determine If the presentation type */
  /* is specified                                 */
  /* BIT1:                                    */
  /*        0 = presentation not specified        */
  /*        1 = presentation type specified in    */
  /*          Bit 2                           */
  if ( command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*cmd_details_ref_id_ptr)].command_details.qualifier
      & GSTK_SEL_ITEM_PRESENTATION_BITMASK )
  {
    presentation_specified = TRUE;
  }
  else
  {
    presentation_specified = FALSE;
  }
  
  /* Need to determine presentation method    */
  /* BIT2:                                    */
  /*        0 = presentation as choice of data    */
  /*          values if bi1 = 1                 */
  /*      1 = presentation as choice of navi -  */
  /*          gation options if bit1 = 1        */
  if ( command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*cmd_details_ref_id_ptr)].command_details.qualifier
        & GSTK_SEL_ITEM_PRESENTATION_AS_NAV_BITMASK)
  {
    present_as_choice_of_nav = TRUE;
  }
  else
  {
    present_as_choice_of_nav = FALSE;
  }
  UIM_MSG_HIGH_2("Presentation specified=%d Presention as choice of nav=%d",
                 presentation_specified, present_as_choice_of_nav);
  if ( presentation_specified )
  {
    if ( present_as_choice_of_nav )
    {
      request_ptr->cmd.select_item_pro_cmd_req.presentation =
          GSTK_NAVIGATION_PRESENTATION;
    }
    else
    {
      request_ptr->cmd.select_item_pro_cmd_req.presentation =
          GSTK_DATA_VALUE_PRESENTATION;
    }
  }
  else
  {
    request_ptr->cmd.select_item_pro_cmd_req.presentation =
        GSTK_PRESENTATION_NOT_SPECIFIED;
  }


  /* Need to determine Selection Preference   */
  /* BIT3:                                    */
  /*        0 = no slelection preference          */
  /*      1 = selection using soft key pref     */
  if ( command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*cmd_details_ref_id_ptr)].command_details.qualifier
      & GSTK_SEL_ITEM_SEL_PREF_BITMASK )
  {
    request_ptr->cmd.select_item_pro_cmd_req.softkey_selection = TRUE;
  }
  else
  {
    request_ptr->cmd.select_item_pro_cmd_req.softkey_selection = FALSE;
  }
  UIM_MSG_HIGH_1("Softkey selection: %d",
                 request_ptr->cmd.select_item_pro_cmd_req.softkey_selection);
  /* Need to Determine If Help is available   */
  /* BIT: 8 -> 0 No Help Available          */
  /*            -> 1 Help Available             */
  if (command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*cmd_details_ref_id_ptr)].command_details.qualifier
      & GSTK_SEL_ITEM_HELP_AVAIL_BITMASK)
  {
    request_ptr->cmd.select_item_pro_cmd_req.help_available = TRUE;
    UIM_MSG_HIGH_0("HELP AVAILABLE");
  }
  else
  {
    request_ptr->cmd.select_item_pro_cmd_req.help_available = FALSE;
    UIM_MSG_HIGH_0("HELP NOT AVAILABLE");
  }
} /* gstk_select_item_cmd_details */


/*===========================================================================
FUNCTION gstk_sel_item_populate_alpha_identifier

DESCRIPTION
  This function will parse cmd_data and populate the first item
  in this select item type.

PARAMETERS
  gstk_cmd_from_card_type *

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
static gstk_status_enum_type gstk_sel_item_populate_alpha_identifier(
  gstk_cmd_from_card_type   *request_ptr,
  uint8                     *cmd_data_ptr,
  const int32               *cmd_data_len_ptr,
  int32                     *offset_ptr,
  boolean                   *more_tlvs_ptr,
  uint32                     cmd_details_ref_id
)
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  int32                 new_offset  = 0;
  boolean               more_tlv    = FALSE;

  if((!request_ptr) ||(!cmd_data_ptr) ||
     (!cmd_data_len_ptr) || (!offset_ptr) || (!more_tlvs_ptr))
  {
    UIM_MSG_ERR_0("Ptr ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  /* Should probably add some Code checking */
  /* for the offset passed in               */

  /* Need to Parse Alpha Identifier       */
  /* Text String: DCS, length and text    */
  gstk_status = gstk_parser_alpha_identifier_tlv (
            cmd_data_ptr,
            *cmd_data_len_ptr,
            *offset_ptr,
            (gstk_string_type *)&request_ptr->cmd.select_item_pro_cmd_req.title,
            &new_offset,
            &more_tlv,
            GSTK_OPTIONAL,
            &cmd_details_ref_id);

  if ( gstk_status == GSTK_SUCCESS )
  {
    *offset_ptr     = new_offset;
    *more_tlvs_ptr  = more_tlv;
  }

  return gstk_status;
} /* gstk_populate_alpha_identifier */

/*===========================================================================
FUNCTION GSTK_SELECT_ITEM_REQ

DESCRIPTION
  This function parses the Select item Command from the card
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
gstk_status_enum_type gstk_select_item_req (
  gstk_proactive_cmd_type *command_ptr
)
{
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  uint8                              *cmd_data_ptr         = NULL;
  int32                               cmd_data_len         = 0;
  uint32                              cmd_details_ref_id   = 0;
  uint8                               num_menu_items       = 0;
  uint8                               menu_item_index      = 0;
  int32                               offset               = 0;
  int32                               next_tlv_offset      = 0;
  boolean                             more_tlvs            = FALSE;
  boolean                             error_already_raised = FALSE;
  gstk_additional_info_type           additional_info      = {0};
  gstk_terminal_rsp_extra_param_type  dummy_extra_param    = {0};
  boolean                             menu_items_error     = FALSE;
 
  /* Cannot do {0x00} in declaration because of compilation warning
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  UIM_MSG_HIGH_1(" IN GSTK_SELECT_ITEM_REQ: command_ptr=0x%x", command_ptr);

  if(command_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_LOW_3(" REQ Len: %x Req B1 %x Req B2 %x",
                command_ptr->length,
                command_ptr->data[0],
                command_ptr->data[1]);

  /* initialize request */
  memset(&request, 0x00, sizeof(gstk_cmd_from_card_type));

  /* First Validate Select Item Command is */
  /* Valid                                 */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  if ((gstk_status != GSTK_SUCCESS) || (cmd_data_ptr == NULL)) {
    error_already_raised = TRUE;
  }
  else { /* Build Command Header  */
    request.hdr_cmd.command_id = GSTK_SELECT_ITEM_REQ;
    request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

    /* Command Number - Assigned from Command    */
    /* Details Reference Table maintained by     */
    /* GSTK.  Set by gstk_preprocess_proactive_  */
    /* cmd and returned in cmd_details_ref_id    */
    request.hdr_cmd.command_number =
      command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number;

    /* Determine Command Details for Setup Menu  */
    gstk_select_item_cmd_details(&request,
                                  &cmd_details_ref_id);

    /* Get the Number of Items */
    gstk_status = gstk_parser_menu_num_items(
                    cmd_data_ptr,
                    &cmd_data_len,
                    &num_menu_items);

    if ( gstk_status == GSTK_SUCCESS )
    {
      /* Set the Number of Items */
      request.cmd.select_item_pro_cmd_req.num_items = num_menu_items;

      /* Start Populating the Select item Menu     */
      /* structure                            */
      if (gstk_valid_tag( cmd_data_ptr[offset], GSTK_ALPHA_IDENTIFIER_TAG)) {
        gstk_status = gstk_sel_item_populate_alpha_identifier( &request,
                            cmd_data_ptr,
                            &cmd_data_len,
                            &offset,
                            &more_tlvs,
                            cmd_details_ref_id);
      }
      else {
        request.cmd.select_item_pro_cmd_req.title.length = 0;
        more_tlvs = TRUE;
      }
    }
    else if (gstk_status == GSTK_BAD_PARAM)
    {
      menu_items_error = TRUE;
    }
  }

  if (gstk_status != GSTK_SUCCESS) {
    error_already_raised = TRUE;
  }

  if( ( gstk_status == GSTK_SUCCESS ) &&
    ( more_tlvs   == TRUE ) )
  {
    /* Alpha Identifier / Title was properly */
    /* pouplated                           */

    /* Start Parsing and Populating the      */
    /* Individual Menu Items                 */

    /* First item is mandatory */
    gstk_status = gstk_parser_item_tlv(
                      cmd_data_ptr,
                      cmd_data_len,
                      offset,
                      &request.cmd.select_item_pro_cmd_req.items[menu_item_index],
                      &next_tlv_offset,
                      &more_tlvs,
                      GSTK_MANDATORY,
                      &cmd_details_ref_id);

    if ( gstk_status == GSTK_SUCCESS )
    {
      /* Change offset to point to offset */
      /* returned by the parsing function */
      offset = next_tlv_offset;

      /* Increment the menu_item_index    */
      menu_item_index++;
      while ( menu_item_index < request.cmd.select_item_pro_cmd_req.num_items )
      {
        gstk_status = gstk_parser_item_tlv(
                        cmd_data_ptr,
                        cmd_data_len,
                        offset,
                        &request.cmd.select_item_pro_cmd_req.items[menu_item_index],
                        &next_tlv_offset,
                        &more_tlvs,
                        GSTK_OPTIONAL,
                        &cmd_details_ref_id);

        if ( gstk_status == GSTK_SUCCESS )
        {
          /* Change offset to point to offset */
          /* returned by the parsing function */
          offset = next_tlv_offset;

          /* Increment the menu_item_index    */
          menu_item_index++;
        }
        else
        {
          UIM_MSG_HIGH_1("PARSING ERROR %x", gstk_status);
          error_already_raised = TRUE;
          break;
        }
      }
    }
    else {
      UIM_MSG_HIGH_1("PARSING ERROR %x", gstk_status);
      error_already_raised = TRUE;
    }
  }

  if ( ( gstk_status == GSTK_SUCCESS ) &&
    ( more_tlvs   == TRUE ) )
  {
    /* Continue Parsing...need to look for */
    /* Next Action Indicator               */
    /* Current Offset is set to point to   */
    /* the next Action Indicator if it is  */
    /* Available                           */

    more_tlvs = FALSE;

    gstk_status = gstk_parser_next_action_indicator_tlv(
            cmd_data_ptr,
            cmd_data_len,
            offset,
            &request.cmd.select_item_pro_cmd_req.next_action_indicator,
            &next_tlv_offset,
            &more_tlvs,
            GSTK_OPTIONAL,
            &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      error_already_raised = TRUE;
    }
    else
    {
      offset = next_tlv_offset;
    }
  }

  if ( ( gstk_status == GSTK_SUCCESS ) &&
       ( more_tlvs   == TRUE ) )
  {
    /* Continue Parsing...need to look for */
    /* Optional Default item            */

    more_tlvs = FALSE;

    gstk_status = gstk_parser_item_identifier_tlv(
            cmd_data_ptr,
            cmd_data_len,
            offset,
            &request.cmd.select_item_pro_cmd_req.default_item,
            &next_tlv_offset,
            &more_tlvs,
            GSTK_OPTIONAL,
            &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      error_already_raised = TRUE;
    }
    else
    {
      offset = next_tlv_offset;
    }
  }

  if (( gstk_status == GSTK_SUCCESS ) &&
     ( more_tlvs   == TRUE ) )
  {
    /* Continue Parsing...need to look for */
    /* Optional Icon Identifier            */
    /* Current Offset is set to point to   */
    /* the Icon Identifer Tag              */

    more_tlvs = FALSE;
    /* Warning...this parsing gets memory  */
    /* from heap to store icon data which  */
    /* must be freed                       */
    gstk_status = gstk_parser_icon_id_tlv(
                  &(request.cmd.select_item_pro_cmd_req.title),
                  cmd_data_ptr,
                  cmd_data_len,
                  offset,
                  &(request.cmd.select_item_pro_cmd_req.icon),
                  &next_tlv_offset,
                  &more_tlvs,
                  GSTK_OPTIONAL,
                  &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      error_already_raised = TRUE;
    }
    else
    {
      offset = next_tlv_offset;
    }
  }

  if ( ( gstk_status == GSTK_SUCCESS ) &&
       ( more_tlvs   == TRUE ) )
  {
    /* Continue Parsing...need to look for */
    /* Optional Icon Identifier            */
    /* Current Offset is set to point to   */
    /* the Icon Identifer Tag              */

    more_tlvs = FALSE;

    gstk_status = gstk_parser_item_icon_identifier_list_tlv(
                        cmd_data_ptr,
                        cmd_data_len,
                        offset,
                        &request.cmd.select_item_pro_cmd_req.icon_id_list,
                        &next_tlv_offset,
                        &more_tlvs,
                        GSTK_OPTIONAL,
                        &cmd_details_ref_id);
    if (gstk_status != GSTK_SUCCESS)
    {
      error_already_raised = TRUE;
    }
    else
    {
      offset = next_tlv_offset;
    }
  }

  if ( !error_already_raised )
  {
    /* Send command to clients */
    gstk_status = gstk_util_send_message_to_clients(&request);

    if ( gstk_status != GSTK_SUCCESS )
    {
      /* Send Terminal Response to Card      */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      UIM_MSG_HIGH_0("Could not send Message");

      /* Send Terminal Response to Card */
      gstk_set_additional_info(&additional_info,GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      dummy_extra_param.present         = FALSE;

      gstk_status = gstk_send_terminal_response(
                        gstk_curr_inst_ptr->client_id,          /* client_id          */
                        0,                                      /* user_data          */
                        cmd_details_ref_id,                     /* command_details_ref*/
                        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,
                        GSTK_SELECT_ITEM_CNF,                   /* command response   */
                        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result     */
                        &additional_info,                           /* additional result  */
                        &dummy_extra_param                      /* extra param */
                        );
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
      }
    }
    else
    {
      /* Start Timer */
      gstk_util_start_client_response_timer(cmd_details_ref_id,
                                            GSTK_TERM_RSP_WAIT_TIME);

      /* reset gstk_status to SUCCESS           */
      /* since there has been no error          */
      /* This is needed since some of           */
      /* the TLVs are optional, which           */
      /* might cause the various parsing        */
      /* functions to return invalid_command    */
      gstk_status = GSTK_SUCCESS;
    }
  }
  else { /* error already raised */
    UIM_MSG_ERR_0("SElect Item TLV parsing err");
    /* Send Terminal Response to Card */
    if(!menu_items_error)
    {
      /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    gstk_status = gstk_send_terminal_response(
                    gstk_curr_inst_ptr->client_id, /* client_id */
                                  0,              /* user_data */
                                  cmd_details_ref_id, /* command_details_ref */
                                  command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,  /* command_num */
                                  GSTK_SELECT_ITEM_CNF, /* command response */
                                  GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                                  &additional_info, /* additional result */
                                  &dummy_extra_param /* extra param */ );
    }
    /* If there were too many menu items, return GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES */
    else
    {
      gstk_status = gstk_send_terminal_response(
                      gstk_curr_inst_ptr->client_id, /* client_id */
                      0,              /* user_data */
                      cmd_details_ref_id, /* command_details_ref */
                      command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,  /* command_num */
                      GSTK_SELECT_ITEM_CNF, /* command response */
                      GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES, /* general_result */
                      &additional_info, /* additional result */
                      &dummy_extra_param /* extra param */ );
    }
    if(gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
    }
  }

  /* release memory allocation */
  gstk_select_item_req_cleanup(&request, cmd_data_ptr);
  return gstk_status;

}/*  gstk_select_item_req */

/*===========================================================================
FUNCTION gstk_select_item_cnf

DESCRIPTION
  This function packs the select item terminal response and sends
  the command to UIM server.

PARAMETERS
  select_item_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_select_item_cnf (
  gstk_select_item_cnf_type* select_item_cnf_ptr
)
{
  gstk_status_enum_type                  gstk_status  = GSTK_SUCCESS;
  select_item_terminal_response_type     STK_response = {0};
  uim_cmd_type                           *uim_cmd_ptr = NULL;
  uint32                                 offset       = 0;
  uint32                                 ref_id       = 0;

  UIM_MSG_HIGH_1("IN GSTK select item cnf: select_item_cnf_ptr=0x%x",
                 select_item_cnf_ptr);

  if(select_item_cnf_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(select_item_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
     select_item_cnf_ptr->cmd_details_ref_id,
     select_item_cnf_ptr->message_header.command_id,
     select_item_cnf_ptr->command_number,
     &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_select_item_cnf_cleanup(select_item_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_select_item_cnf_cleanup(select_item_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
     select_item_cnf_ptr->command_result,
     select_item_cnf_ptr->result_additional_info,
     &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_select_item_cnf_cleanup(select_item_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* Item Identifier */
  if (select_item_cnf_ptr->data_present) {
    gstk_status = gstk_packer_item_id_tlv(
                select_item_cnf_ptr->chosen_item_identifier,
                &STK_response.item_chosen);
  }

  /* total header length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
                      + item chose id + 2
  */
  STK_response.num_bytes = STK_response.header.command_details.length +
                            STK_response.header.device_id.device_tag_length +
                            STK_response.header.result.result_tag_length + 6;

  if (select_item_cnf_ptr->data_present) {
    STK_response.num_bytes = STK_response.num_bytes +
                              STK_response.item_chosen.length +
                              2;
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
        ref_id = select_item_cnf_ptr->cmd_details_ref_id;

        /* clear the command detail table reference slot before since we are about to reply to
           the card */
        gstk_util_cmd_details_ref_table_free_slot(select_item_cnf_ptr->cmd_details_ref_id);

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

        offset += (uint32)(STK_response.header.command_details.length + 2);

        /* copy device */
        if(gstk_status == GSTK_SUCCESS &&
           gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                   &(STK_response.header.device_id),
                                   offset,
                                   (size_t)(STK_response.header.device_id.device_tag_length + 2),
                                   sizeof(uim_cmd_ptr->terminal_response.data) - offset,
                                   sizeof(STK_response.header.device_id)) <
                (size_t)(STK_response.header.device_id.device_tag_length + 2))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }

        offset += (uint32)(STK_response.header.device_id.device_tag_length + 2);

        /* copy result */
        if(gstk_status == GSTK_SUCCESS &&
           gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                   &(STK_response.header.result),
                                   offset,
                                   3,
                                   sizeof(uim_cmd_ptr->terminal_response.data) - offset,
                                   sizeof(STK_response.header.result)) < 3)
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }

        offset = offset + 3;

        if (gstk_status == GSTK_SUCCESS &&
            STK_response.header.result.additional_info != NULL )
        {
          /* copy Additional Information */
          if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                     STK_response.header.result.additional_info,
                                     offset,
                                     (size_t)(STK_response.header.result.result_tag_length - 1),
                                     sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                                     (size_t)(STK_response.header.result.result_tag_length - 1)) <
             (size_t)(STK_response.header.result.result_tag_length - 1))
          {
            UIM_MSG_ERR_0("gstk_select_item_cnf(): memscpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
          offset = offset + STK_response.header.result.result_tag_length - 1;
        }

        /* Need to add Item Selected */
        if (gstk_status == GSTK_SUCCESS &&
            (select_item_cnf_ptr->data_present) && ((offset+3) < sizeof(uim_cmd_ptr->terminal_response.data)))
        {
          if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                     &STK_response.item_chosen,
                                     offset,
                                     sizeof(STK_response.item_chosen),
                                     sizeof(uim_cmd_ptr->terminal_response.data)-offset,
                                     sizeof(STK_response.item_chosen)) <
		     sizeof(STK_response.item_chosen))
          {
            gstk_status = GSTK_MEMORY_ERROR;
          }
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_util_dump_byte_array("Select Item TR",
                                    uim_cmd_ptr->terminal_response.data,
                                    uim_cmd_ptr->terminal_response.num_bytes);

          gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
        }
        else
        {
          gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
          uim_cmd_ptr = NULL;
        }
      }/* uim_cmd_ptr != NULL */
    }
  }/* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_select_item_cnf_cleanup(select_item_cnf_ptr, &STK_response);
  return gstk_status;
} /* gstk_select_item_cnf */

