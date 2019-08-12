/*===========================================================================


            G S T K   P R O A C T I V E   C M D



GENERAL DESCRIPTION
  This source file contains functions to process proactive command.
  It contains functions that will dispatch the proactive command processing
  to various function based on the type of command being sent from the card


FUNCTIONS
  gstk_find_expected_proactive_cmd_rsp
    This function mapped the passed in command type to the corresponding
    response type

  gstk_preprocess_proactive_cmd
    This function parsed the command into:
    proactive command tag, total tlv length, command detail tlv
    and device identities tlv and performs early error detection on these fields

  gstk_process_proactive_command
    This function switches on the command id and call the corresponding
   proactive command to further parse/process the command

  gstk_end_proactive_session
    This function notifies all the gstk client regarding the end of a
    proactive session


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_proactive_cmd.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
08/08/15   vr      Reject the Proactive Command for Data Card case
04/27/15   vr      Non UI and non NW proactive command parallel processing
10/21/14   shr     Close channel kept open if next proactive cmd is unexpected
10/08/14   sw      Modified IMSI_M erlated F3 messages
09/25/14   gm      Added support to send failure indication for OTASP
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/31/14   dy      Fixing KW errors
07/11/14   shr     Add Support for NV Refresh
06/03/14   sw      Implemented retry for call-control envelopes
05/28/14   gm      Support of CSG Rel-11 feature
05/28/14   dy      Add GSTK_SUBS_FEATURE_MODE_SRLTE
04/03/14   dy      Replace gstk_malloc() with macro
02/27/14   sw      Extended retry mechanism
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
03/04/14   shr     Fix Subscription Ready caching logic
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
12/06/13   shr     Added support for preferred activation over 1x
12/06/13   bd      Changes in GSTK to support MM IDLE events for SGLTE+G
11/13/13   shr     Added support for BIP over WLAN
10/04/13   gs      F3 Message reduction
09/13/13   gm      Support for Recovery and hot-swap
07/23/13   sw      Removed DS API to query for RAT information
06/11/13   hh      Fix Toolkit Compiling Errors
04/11/13   bd      Avoid notifying NAS to ATTACH before IMSI switch
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/28/13   bd      Support for dynamic reg and dereg
01/22/13   hn      Support 1xRTT CSIM programming retries
01/17/13   hn      Support multiple sessions per subscription
11/29/12   bd      Clean up of redundant members from client table
11/27/12   bd      Fixed incorrect slot reading
09/18/12   hk      Support to read IMSI after getting VPE resp
08/06/12   sg      Fix starvation when reading IMSI_M failed
05/11/12   sg      Use DS SYS API for getting RAT mask from DS
03/28/12   av      Replaced feature flags with NVITEM
03/16/12   yt      Remove tmc.h inclusion
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/18/12   sg      Fix cnf type of run at command for clean up
12/20/11   nb      GSDI Cleanup
10/31/11   sg      Class L and M support
10/18/11   av      Removed features that are always OFF
08/25/11   bd      Fixed incorrect usage qualifier field from cmd table
06/10/11   bd      Fixed issue of fetching proactive command from another
                   slot while a cmd is already in progress
04/13/11   ea      Added enable / disable proactive cmd support
04/05/11   xz      Changes to support VPE ATTACH CTRL
02/18/11   tkl     Removed FEATURE_DUAL_SIM in setting sim_slot_id
02/16/11   bd      Support for delaying the TR when card is repeating the
                   same proactive command continuously.
02/02/11   tkl     Added setting of sim_slot_id for proactive cmd in raw format
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/17/10   xz      Decouple feature bitmask
12/13/10   adp     Check for Subscription Ready event only when
                   FEATURE_MMGSDI_SESSION_LIB  is defined by client
12/04/10   ssr     Fixed compilation error
11/25/10   ssr     Fixed command detail parsing
11/22/10   sg      Don't read IMSI_M before subscription is ready
10/27/10   js      Fix for open channel terminal rsp not getting sent
10/11/10   sg      Port IMSI_M enhancement
09/13/10   xz      Add support of SETUP MENU in raw APDU for DS/DS
02/15/10   nb      Moved Dual Slot Changes
01/14/10   xz      KW Fixes
01/12/10   xz      Ignore extra data of cmd details and device ID TLV
10/18/09   dd      Forward raw refresh proactive message to gstk clients
09/26/09   kp      ZI memory reduction changes
03/17/09   xz      Fix issue of unable to send error TR
01/16/09   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
11/13/08   xz      Fix issue of cmd_ref_id leak
11/13/08   xz      Fix klockwork errors
09/14/08   sk      Added fix for lint errors
09/08/08   sk      Added support to save the raw apdu of the proactive command
                   from the card for ESTK use.
09/08/08   sk      Fixed warning
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/27/08   sk      Fixed warning
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session
                   Added support to differentiate the different types of
                   Provide Local Info Proactive command requests
08/21/08   sk      Added GSTK enhanced registration support.
08/19/08   sk      Fixed compilation error
08/15/08   sk      Fixed compilation error
08/14/08   sk      Fixed warnings
08/08/08   xz      Add support for raw proactive command
11/20/07   tml     Added support to allow delay in sending TR until client
                   responded back for certain internal command that was
                   originally handled by GSTK
08/08/07   sk      Ignore command when BER TLV is unknown
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
10/08/06   sk      Added support for unknown unsupported proactive commands.
06/24/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
04/04/06   sk      Added pli - battery status support
11/09/05   sk      Fixed Lint Errors
07/26/05   sk      Changed Response Type for Access Technology
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Added SAT/USAT data support
06/14/04   tml     Added support to differentiate GSTK auto generated END or
                   End from the card
05/11/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/16/04   tml     linted
03/01/04   tml     Added language provide local info support
07/07/03   tml     Change priority of messages
05/16/03   tml     linted
04/17/03   tml     lint
03/06/03   tml     Updated enum names
02/28/03   jar     Uncommented Refreh Req Handling
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include <stringl/stringl.h>
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_envelope_rsp.h"
#include "ds_sys.h"
#include "ds_sys_event.h"
#include "ds_sys_conf.h"
#include "err.h"
#include "string.h"
#include "mmgsdilib_v.h"
#include "uim_msg.h"
#include "estk.h"

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
    gstk_status_enum_type:
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
gstk_cmd_enum_type gstk_find_expected_proactive_cmd_rsp(
                   uint32              request_cmd_type)
{
  gstk_cmd_enum_type expected_cmd_rsp = GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF;
  switch(request_cmd_type)
  {
    case GSTK_CMD_STK_REFRESH:
        expected_cmd_rsp = GSTK_REFRESH_CNF;
        break;
    case GSTK_CMD_STK_MORE_TIME:
        expected_cmd_rsp = GSTK_MORE_TIME_CNF;
        break;
    case GSTK_CMD_STK_POLL_INTERVAL:
        expected_cmd_rsp = GSTK_POLL_INTERVAL_CNF;
        break;
    case GSTK_CMD_STK_POLLING_OFF:
        expected_cmd_rsp = GSTK_POLLING_OFF_CNF;
        break;
    case GSTK_CMD_STK_SET_UP_EVENT_LIST:
        expected_cmd_rsp = GSTK_SETUP_EVENT_LIST_CNF;
        break;
    case GSTK_CMD_STK_SET_UP_CALL:
        expected_cmd_rsp = GSTK_SETUP_CALL_CNF;
        break;
    case GSTK_CMD_STK_SEND_SS:
        expected_cmd_rsp = GSTK_SEND_SS_CNF;
        break;
    case GSTK_CMD_STK_USSD:
        expected_cmd_rsp = GSTK_SEND_USSD_CNF;
        break;
    case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
        expected_cmd_rsp = GSTK_SEND_SMS_CNF;
        break;
    case GSTK_CMD_STK_LAUNCH_BROWSER:
        expected_cmd_rsp = GSTK_LAUNCH_BROWSER_CNF;
        break;
    case GSTK_CMD_STK_PLAY_TONE:
        expected_cmd_rsp = GSTK_PLAY_TONE_CNF;
        break;
    case GSTK_CMD_STK_DISPLAY_TEXT:
        expected_cmd_rsp = GSTK_DISPLAY_TEXT_CNF;
        break;
    case GSTK_CMD_STK_GET_INKEY:
        expected_cmd_rsp = GSTK_GET_INKEY_CNF;
        break;
    case GSTK_CMD_STK_GET_INPUT:
        expected_cmd_rsp = GSTK_GET_INPUT_CNF;
        break;
    case GSTK_CMD_STK_SELECT_ITEM:
        expected_cmd_rsp = GSTK_SELECT_ITEM_CNF;
        break;
    case GSTK_CMD_STK_SET_UP_MENU:
        expected_cmd_rsp = GSTK_SETUP_MENU_CNF;
        break;
    case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
        /* Language provide local info cnf type will be assigned in the
           preprocess proactive cmd function */
        expected_cmd_rsp = GSTK_PROVIDE_LOCAL_INFO_CNF;
        break;
    case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
        expected_cmd_rsp = GSTK_SETUP_IDLE_TEXT_CNF;
        break;
    case GSTK_CMD_STK_SEND_DTMF:
        expected_cmd_rsp = GSTK_SEND_DTMF_CNF;
        break;
    case GSTK_CMD_STK_TIMER_MANAGEMENT:
        expected_cmd_rsp = GSTK_TIMER_MANAGEMENT_CNF;
        break;
    case GSTK_CMD_STK_LANG_NOTIFICATION:
        expected_cmd_rsp = GSTK_LANG_NOTIFICATION_CNF;
        break;
    case GSTK_CMD_STK_OPEN_CHANNEL:
        expected_cmd_rsp = GSTK_OPEN_CH_CNF;
        break;
    case GSTK_CMD_STK_CLOSE_CHANNEL:
        expected_cmd_rsp = GSTK_CLOSE_CH_CNF;
        break;
    case GSTK_CMD_STK_RECEIVE_DATA:
        expected_cmd_rsp = GSTK_RECEIVE_DATA_CNF;
        break;
    case GSTK_CMD_STK_SEND_DATA:
        expected_cmd_rsp = GSTK_SEND_DATA_CNF;
        break;
    case GSTK_CMD_STK_GET_CHANNEL_STATUS:
        expected_cmd_rsp = GSTK_GET_CH_STATUS_CNF;
        break;
    case GSTK_CMD_STK_ACTIVATE:
        expected_cmd_rsp = GSTK_ACTIVATE_CNF;
        break;
    /* not supported for PHASE 1 development */
    case GSTK_CMD_STK_RUN_AT_COMMAND:
    case GSTK_CMD_STK_PERFORM_CARD_APDU:
    case GSTK_CMD_STK_POWER_ON_CARD:
    case GSTK_CMD_STK_POWER_OFF_CARD:
    case GSTK_CMD_STK_GET_READER_STATUS:
    default:
        expected_cmd_rsp = GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF;
        break;
    }

    // how about CMD_STK_END_PROACTIVE_CMD_SEC?
    return expected_cmd_rsp;
} /* gstk_find_expected_proactive_cmd_rsp*/

/*===========================================================================
FUNCTION gstk_preprocess_proactive_cmd

DESCRIPTION
  This function parsed the command into:
  proactive command tag, total tlv length, command detail tlv
  and device identities tlv and performs early error detection on these fields
  The rest of the data is put into the cmd_data_ptr (passed in parameter)
  cmd_data_len_ptr indicates the size of the remaining tlv datas after parsing
  the command details and device ID
  cmd_details_ref_id is the index location in the global command details ref
  table.  If there is still free slot in the command details ref table,
  the command details thus parsed will be assigned to the index location (as
  indicated by cmd_details_ref_id) in the global ref table

PARAMETERS
  cmd_ptr:           [Input]        Pointer to the original proactive command
                                    data to be processed
  cmd_data_ptr:      [Input/Output] Pointer to be populated for the rest of the
                                    proactive cmd_ptr data after command details
                                    and device id has been processed. If NULL
                                    pointer is passed in, this function will not
                                    allocate command data buffer.
  cmd_data_len_ptr:  [Input/Output] Length for valid data in the cmd_data_ptr that
                                    is being returned to the user. If NULL pointer
                                    is passed in, this function will not allocate
                                    command data buffer.
  cmd_ref_id_ptr:    [Input/Output] Pointer to command details reference index
                                    location allocated for this proactive command
                                    in the command_details_ref_table

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
gstk_status_enum_type gstk_preprocess_proactive_cmd(
         gstk_proactive_cmd_type*  cmd_ptr,
         uint8**                   cmd_data_ptr,
         int32*                    cmd_data_len_ptr,
         uint32*                   cmd_ref_id_ptr)
{
  uint32                   length_of_TLV_length_field = 1;
  int32                    length_of_all_tlvs         = 0; /* cmd_ptr->data[1] */
  int32                    len                        = 0;
  int32                    count                      = 0; /* incremented along the cmd_ptr as we parse
                                                            ** and verify each index location */
  gstk_cmd_enum_type             expected_cmd_rsp     = GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF;
  gstk_parsing_result_enum_type  tag_parsing_result   = GSTK_TLV_PARSING_ERROR;
  uint32                         index                = 0;
  gstk_nv_items_data_type        nv_data;

  UIM_MSG_LOW_0("IN gstk_preprocess_proactive_cmd()");

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* input param checkings */
  if (cmd_ptr == NULL || cmd_ref_id_ptr == NULL)
  {
    UIM_MSG_ERR_2("cmd_ptr is 0x%x, cmd_ref_id_ptr is 0x%x",
                  cmd_ptr, cmd_ref_id_ptr);
    return GSTK_BAD_PARAM;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  /*
  ** All STK commands have the following initial format,
  ** --------------------------------------------------
  **
  ** 1  | 1 byte  - Proactive SIM command tag (0xD0)
  ** 2  | 2 bytes - Length (either 1 or 2 bytes long - see 11.14)
  ** 3  | 1 byte  - Command Details tag (0x81) (Also encompasses next 4 bytes)
  ** 4  | 1 byte  - Length (0x03) 3 more bytes coming
  ** 5  | 1 byte  - Command number
  ** 6  | 2 bytes - Command specfic operation instructions
  ** 7  | 1 byte  - Device identites tag (0x82)
  ** 8  | 1 byte  - Length (0x02) 2 more coming
  ** 9  | 1 byte  - Source device code
  ** 10 | 1 byte  - Dest. device code
  */

  /*
  ** BER-TLV LENGTH
  ** This length field can either be 0x00-0x7F or
  ** 0x81 if the 2nd byte is used
  */

  /* Check for length of the length value has already been done in the
     calling function */

  count = GSTK_PROACTIVE_CMD_TAG_INDEX + 1; /* total BER-TLV length field */
  length_of_TLV_length_field = gstk_find_length_of_length_value(&(cmd_ptr->data[count]));

  length_of_all_tlvs = cmd_ptr->data[count+length_of_TLV_length_field-1];
  count = count + length_of_TLV_length_field;

  UIM_MSG_HIGH_2(" TLV_length = 0x%x, Current Count is 0x%x",
                 length_of_all_tlvs, count);

  index = gstk_util_cmd_details_ref_table_get_next_free_slot();

  if ((index == GSTK_CMD_DETAILS_REF_TABLE_FULL) ||
      (index > (GSTK_MAX_PROACTIVE_COMMAND-1)))
  {
    return GSTK_ERROR;
  }
  else
  {
    *cmd_ref_id_ptr = gstk_util_generate_ref_id(index);
  }


  /*
  ** COMMAND DETAILS
  ** Parse and extract the 5 bytes of command_details
  */

  /* Setting default value for error scenarion */
  /* Set pending responses to 1 because at least we need 1 responses to build the TR */
  command_details_ref_table_ptr[index].pending_responses = 1;

  command_details_ref_table_ptr[index].sim_slot_id =
    gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
 /* Command Detail Tag   */
  command_details_ref_table_ptr[index].command_details.tag =
    (uint8) GSTK_COMMAND_DETAILS_TAG;

  /* Command Detail TLV length */
  command_details_ref_table_ptr[index].command_details.length =
    (uint8)GSTK_COMMAND_DETAILS_LEN;

  /* Command Number 0x00 is indicating the last proactive command on card */
  command_details_ref_table_ptr[index].command_details.command_number = 0x00;

  /* Type of Command */
  command_details_ref_table_ptr[index].command_details.type_of_command = 0x00;

  /* Command Qualifier */
  command_details_ref_table_ptr[index].command_details.qualifier = 0x00;

  command_details_ref_table_ptr[index].expected_cmd_rsp_type  =
    GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF;

  command_details_ref_table_ptr[index].unique_ref_id = *cmd_ref_id_ptr;

  tag_parsing_result = gstk_is_tag_valid(cmd_ref_id_ptr,
                                         cmd_ptr->data[count],   /* Current Tag */
                                         (uint8)GSTK_COMMAND_DETAILS_TAG,
                                         GSTK_MANDATORY,
                                         cmd_ptr->data,          /* Proactive cmd */
                                         &count,                 /* Searched Tag index */
                                         length_of_all_tlvs);

  /* For Mandatory Tag it should return parsing continue */
  if ((GSTK_TLV_PARSING_CONTINUE != tag_parsing_result)
      ||((count + GSTK_COMMAND_DETAILS_LEN) >(length_of_all_tlvs + length_of_TLV_length_field))
      ||(length_of_all_tlvs >= GSTK_MAX_DATA_SIZE)
      ||(!gstk_valid_tag(cmd_ptr->data[count],GSTK_COMMAND_DETAILS_TAG)))
  {
    UIM_MSG_ERR_1("Invalid Cmd Tag 0x%x", cmd_ptr->data[count]);
    return GSTK_UNSUPPORTED_COMMAND;
  }

  /* Command Detail Tag */
  command_details_ref_table_ptr[index].command_details.tag =
    cmd_ptr->data[count++];

  /* Command Detail TLV length */
  command_details_ref_table_ptr[index].command_details.length =
    cmd_ptr->data[count++];

  if (command_details_ref_table_ptr[index].command_details.length <
      GSTK_COMMAND_DETAILS_LEN)
  {
    UIM_MSG_ERR_0("CMD Detail Leng ERROR");
    return GSTK_UNSUPPORTED_COMMAND;
  }

  /* Command Number */
  command_details_ref_table_ptr[index].command_details.command_number =
  cmd_ptr->data[count++];

  /* Type of Command */
  command_details_ref_table_ptr[index].command_details.type_of_command =
    cmd_ptr->data[count++];

  /* Command Qualifier */
  command_details_ref_table_ptr[index].command_details.qualifier =
    cmd_ptr->data[count++];

  if (command_details_ref_table_ptr[index].command_details.length >
      GSTK_COMMAND_DETAILS_LEN)
  {
    UIM_MSG_HIGH_3("cmd detail len too long: 0x%x 0x%x 0x%x!",
                   command_details_ref_table_ptr[index].command_details.length,
                   GSTK_COMMAND_DETAILS_LEN,
                   count);
    count += command_details_ref_table_ptr[index].command_details.length
      - GSTK_COMMAND_DETAILS_LEN;
    command_details_ref_table_ptr[index].command_details.length =
      GSTK_COMMAND_DETAILS_LEN;
  }

  /* Map the passed in command type to the corresponding response type */
  expected_cmd_rsp = gstk_find_expected_proactive_cmd_rsp(
                 command_details_ref_table_ptr[index].command_details.type_of_command);

  if (expected_cmd_rsp == GSTK_PROVIDE_LOCAL_INFO_CNF)
  {
    switch (command_details_ref_table_ptr[index].command_details.qualifier)
    {
    case GSTK_LANGUAGE_SETTING:
      /* Change expected rsp enum type for Language provide local info */
         expected_cmd_rsp = GSTK_PROVIDE_LANG_INFO_CNF;
         break;
    case GSTK_ACCESS_TECHNOLOGY:
      /* Change expected rsp enum type for Access Technology */
         expected_cmd_rsp = GSTK_PROVIDE_ACCESS_TECH_CNF;
         break;
    case GSTK_BATTERY_STATUS:
         if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
         {
           /* Change expected rsp enum type for Battery Status */
           expected_cmd_rsp = GSTK_PROVIDE_BATT_STATUS_CNF;
           break;
         }
         UIM_MSG_HIGH_1("GSTK_BATTERY_STATUS: CAT version is pre-VER6 - 0x%x",
                        nv_data.cat_version);
         break;
    case GSTK_NETWORK_SEARCH_MODE:
         if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
         {
           /* Change expected rsp enum type for Network Search Mode */
           expected_cmd_rsp = GSTK_PROVIDE_NW_SEARCH_MODE_CNF;
           break;
         }
         UIM_MSG_HIGH_1("GSTK_NETWORK_SEARCH_MODE: CAT version is pre-VER6 - 0x%x",
                        nv_data.cat_version);
         break;
    case GSTK_CSG_CELL:
         if(nv_data.cat_version >= GSTK_CFG_CAT_VER9)
         {
           /* Change expected rsp enum type for Network Search Mode */
           expected_cmd_rsp = GSTK_PROVIDE_CSG_INFO_CNF;
           break;
         }
         UIM_MSG_HIGH_1("GSTK_CSG_INFO_MODE: CAT version is pre-VER9 - 0x%x",
                        nv_data.cat_version);
         break;
    default:
          break;
    }
  }

  command_details_ref_table_ptr[index].expected_cmd_rsp_type = expected_cmd_rsp;
  UIM_MSG_LOW_2(" COMMAND DETAILS TAG: 0x%x, TAG LEN: 0x%x",
                command_details_ref_table_ptr[index].command_details.tag,
                command_details_ref_table_ptr[index].command_details.length);
  UIM_MSG_HIGH_3(" COMMAND DETAILS CMD NUM: 0x%x, TYPE OF CMD: 0x%x, QUALIFIER: 0x%x",
                 command_details_ref_table_ptr[index].command_details.command_number,
                 command_details_ref_table_ptr[index].command_details.type_of_command,
                 command_details_ref_table_ptr[index].command_details.qualifier);

  /*
  ** DEVICE IDENTITIES
  ** Parse and extract the 4 bytes of device_identities
  */

  /* If device identities field exists, sanity check on the device identity info */
  tag_parsing_result = gstk_is_tag_valid(cmd_ref_id_ptr,
                                         cmd_ptr->data[count],
                                         (uint8)GSTK_DEVICE_IDENTITY_TAG,
                                         GSTK_MANDATORY,
                                         cmd_ptr->data,
                                         &count,
                                         length_of_all_tlvs);

  /* For Mandatory Tag it should return parsing continue */
  if (GSTK_TLV_PARSING_CONTINUE != tag_parsing_result)

  {
    UIM_MSG_ERR_0("GSTK_DEVICE_IDENTITY_TAG is not found");
    return GSTK_INVALID_COMMAND;
  }

  if (((count + GSTK_DEVICE_IDENTITY_LEN) <
         (length_of_all_tlvs + length_of_TLV_length_field))&&
      gstk_valid_tag(cmd_ptr->data[count],GSTK_DEVICE_IDENTITY_TAG))
  {
    count++;
    len = cmd_ptr->data[count];
    if (len >= GSTK_DEVICE_IDENTITY_LEN) {
      count++;
      if (cmd_ptr->data[count] == (int)GSTK_UICC_SIM_DEVICE) { /* from card */
        UIM_MSG_LOW_1(" DEVICE IDs TRG DEV  : %x", cmd_ptr->data[count]);
        count++; /* increment for target device */
        switch(cmd_ptr->data[count])
        {
        case GSTK_KEYPAD_DEVICE:
        case GSTK_DISPLAY_DEVICE:
        case GSTK_EARPIECE_DEVICE:
        case GSTK_UICC_SIM_DEVICE:
        case GSTK_ME_DEVICE:
        case GSTK_NETWORK_DEVICE:
        case GSTK_CHANNEL_1_DEVICE:
        case GSTK_CHANNEL_2_DEVICE:
        case GSTK_CHANNEL_3_DEVICE:
        case GSTK_CHANNEL_4_DEVICE:
        case GSTK_CHANNEL_5_DEVICE:
        case GSTK_CHANNEL_6_DEVICE:
        case GSTK_CHANNEL_7_DEVICE:
          command_details_ref_table_ptr[index].destination_dev_id =
            (device_identity_e)cmd_ptr->data[count];
          count++;/* increment for next TLV index */
          break;
        default:
          /* save it such that it can be used later on */
          command_details_ref_table_ptr[index].destination_dev_id =
            (device_identity_e)cmd_ptr->data[count];
          UIM_MSG_HIGH_1(" Dest Dev ID invalid %x", cmd_ptr->data[count]);
           return GSTK_INVALID_DEVICE_COMMAND;
        }

        if (len > GSTK_DEVICE_IDENTITY_LEN)
        {
          UIM_MSG_HIGH_3("device id len too long: 0x%x 0x%x 0x%x",
                         len, GSTK_DEVICE_IDENTITY_LEN, count);
          count += len - GSTK_DEVICE_IDENTITY_LEN;
        }
      }
      else {
        UIM_MSG_HIGH_1(" Source Dev ID != UICC_SIM_DEVICE, %x",
                       cmd_ptr->data[count]);
        /* Mark the cmd_details_ref_id location as free slot again */
        return GSTK_INVALID_COMMAND;
      }
    }
    else {
      UIM_MSG_HIGH_1(" Dev ID Len != 2, %x", cmd_ptr->data[count]);
      /* Mark the cmd_details_ref_id location as free slot again */
      return GSTK_INVALID_COMMAND;
    }
  }
  else {
    UIM_MSG_HIGH_1(" Dev ID Tag != Device_id_tag, %x",
                   (count < GSTK_MAX_DATA_SIZE) ? cmd_ptr->data[count] : -1);
    /* Mark the cmd_details_ref_id location as free slot again */
    return GSTK_INVALID_COMMAND;
  }
  command_details_ref_table_ptr[index].raw_cmd.data_len =
    uint32toint32(cmd_ptr->length);
  if(cmd_ptr->length > 0)
  {
    command_details_ref_table_ptr[index].raw_cmd.data_buffer_ptr =
      (uint8*)GSTK_CALLOC(sizeof(uint8)*cmd_ptr->length);
    if(command_details_ref_table_ptr[index].raw_cmd.data_buffer_ptr)
    {
      (void)gstk_memcpy(command_details_ref_table_ptr[index].raw_cmd.data_buffer_ptr,
                     cmd_ptr->data,
                     cmd_ptr->length,
                     cmd_ptr->length,
                     cmd_ptr->length);
    }
    else
    {
      command_details_ref_table_ptr[index].raw_cmd.data_len = 0;
    }
  }/* if(cmd_ptr->length > 0) */

  if (cmd_data_ptr == NULL || cmd_data_len_ptr == NULL)
  {
    return GSTK_SUCCESS;
  }
  /*
  ** Store remaining bytes into command specific array
  */
  if ((cmd_data_ptr != NULL) && (cmd_data_len_ptr != NULL))
  {
    *cmd_data_len_ptr = length_of_all_tlvs +
                        length_of_TLV_length_field +
                        1 -
                        count;

    if(*cmd_data_len_ptr > 0)
    {
      UIM_MSG_HIGH_2(" REMAIN CMD LEN: 0x%x, offset: 0x%x",
                     *cmd_data_len_ptr, count);

      if (count < GSTK_MAX_DATA_SIZE && count > 0)
      {
        *cmd_data_ptr = (uint8*)GSTK_CALLOC(int32touint32(*cmd_data_len_ptr));
        if (*cmd_data_ptr == NULL) {
          return GSTK_BAD_PARAM;
        }

        /* Set Command Specific Data */
        (void)memscpy(*cmd_data_ptr,
               int32touint32(*cmd_data_len_ptr),
               (void *)(&(cmd_ptr->data[count])),
               int32touint32(*cmd_data_len_ptr));
      }
      else
      {
        UIM_MSG_ERR_0("The index of cmd_ptr->data is out of bound!");
        return GSTK_ERROR;
      }
    }
  }

  return GSTK_SUCCESS;
}/*lint !e818 *cmd_ptr suppression for externalized function */
/* gstk_preprocess_proactive_cmd */


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
void gstk_raw_proactive_cmd_cleanup_on_error (
  gstk_client_pro_cmd_reg_data_type  *reg_info_ptr,
  gstk_cmd_enum_type                 cmd_type,
  uint32                             cmd_details_ref_id
)
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  gstk_terminal_rsp_extra_param_type other_info ={0};

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  other_info.present = FALSE;

  /* check if cmd_details_ref_id is valid */
  if (GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id) < GSTK_MAX_PROACTIVE_COMMAND)
  {
    /* send UNABLE_TO_PROCESS terminal response */
    gstk_status = gstk_send_terminal_response(
                    gstk_curr_inst_ptr->client_id,
                    0,  /* user data doesn't matter */
                    cmd_details_ref_id,
                    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,
                    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].expected_cmd_rsp_type,
                    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                    NULL,
                    &other_info);
    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Failed to send term rsp in gstk_raw_proactive_cmd_cleanup_on_error()!");
      /* free cmd_details_ref_id */
      gstk_util_cmd_details_ref_table_free_slot(cmd_details_ref_id);
    }
  }

  /* free all allocated memory */
  if (reg_info_ptr != NULL)
  {
    if (reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr != NULL)
    {
      gstk_free(reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_buffer_ptr);
    }
    gstk_free(reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr);
    gstk_free(reg_info_ptr);
  }
} /* gstk_raw_proactive_cmd_cleanup_on_error */


/*===========================================================================
FUNCTION gstk_process_proactive_command

DESCRIPTION

  This function switches on the command id and call the corresponding
  proactive command to further parse/process the command

PARAMETERS
  cmd_ptr: [Input] Pointer to the proactive command to be processed

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
gstk_status_enum_type gstk_process_proactive_command(
  gstk_proactive_cmd_type *cmd_ptr )
{
  gstk_status_enum_type              gstk_status                 = GSTK_SUCCESS;
  uint8                              type_of_command             = 0x00;
  uint8                              length_of_TLV_length_field  = 0x01;
  sint15                             ds_errno                    = DS_EINVAL;
  uint32                             cmd_details_ref_id          = GSTK_MAX_PROACTIVE_COMMAND;
  boolean                            cmd_enable                  = TRUE;
  uint8                              command_qual                = 0x00;
  gstk_client_id_type                client_id                   = 0x00;
  gstk_client_pro_cmd_reg_data_type  *reg_info_ptr               = NULL;
  /* Cannot do {0}, compilation warning */
  gstk_cmd_enum_type                 cmd_type;
  mmgsdi_access_type                 file_access;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  memset(&cmd_type, 0x00, sizeof(gstk_cmd_enum_type));
  memset(&file_access, 0x00, sizeof(mmgsdi_access_type));

  /* Skip TLV command details */
  /* Ignore Proactive Command Tag = 0xD0 and look at the length field value
  ** to determine length of the length field */

  /*
  ** BER-TLV LENGTH
  ** This length field can either be 0x00-0x7F or
  ** 0x81 if the 2nd byte is used
  */

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_2("In gstk_process_proactive_command() ui_nw_slot_in_progress = 0x%x, cmd slot_id 0x%x",
                 gstk_shared_data.ui_nw_proactive_session_slot_in_progress,
                 cmd_ptr->message_header.sim_slot_id);

  /* Check if we have a valid proactive SIM command tag (0xD0) */
  if(!gstk_valid_tag(cmd_ptr->data[GSTK_PROACTIVE_CMD_TAG_INDEX],
                     (tags_enum_type)GSTK_PROACTIVE_SIM_COMMAND_TAG))
  {
    /* Not a proactive command tag, ignore the command */
    UIM_MSG_HIGH_1("not a valid proactive command 0x%x",
                   cmd_ptr->data[GSTK_PROACTIVE_CMD_TAG_INDEX]);
    return GSTK_INVALID_COMMAND;
  }

  /*
  ** BER-TLV LENGTH
  ** This length field can either be 0x00-0x7F or
  ** 0x81 if the 2nd byte is used
  */

  length_of_TLV_length_field = int32touint8(gstk_find_length_of_length_value(&cmd_ptr->data[1]));

  UIM_MSG_HIGH_1(" Length of TLV: %x", length_of_TLV_length_field);

  if(length_of_TLV_length_field != 1 &&
     length_of_TLV_length_field != 2)
  {
    /* Invalid BER TLV len , ignore the command */
    return GSTK_INVALID_LENGTH;
  }
  /* 4th location after total length field is the type of command
  ** 0   Proactive SIM Command Tag
  ** 1-2 total length
  ** +1  Command Details tag
  ** +1  Command details len = 3
  ** +1  Command number
  ** +1  Command Type
  ** +1  Command Qualifier
  */
  type_of_command = cmd_ptr->data[length_of_TLV_length_field + 4];

  /* Notify ESTK of the proactive command */
  estk_notify_proactive_command(type_of_command,
                                gstk_curr_inst_ptr->slot_id);

  if(type_of_command != (uint8)GSTK_CMD_STK_END_OF_PROACTIVE_SES)
  {
    /* Compare new cmd with cached cmd */
    if (GSTK_SUCCESS == gstk_cmp_with_cached_cmd (cmd_ptr))
    {
      /* Both commands are matched */
      ++gstk_curr_inst_ptr->gstk_cmd_cache.dupe_count;
    }
    else
    {
      UIM_MSG_HIGH_0 ("Cache the proactive cmd");
      /* Cache the proactive command */
      (void)memscpy (gstk_curr_inst_ptr->gstk_cmd_cache.cmd,
               sizeof(gstk_curr_inst_ptr->gstk_cmd_cache.cmd),
               cmd_ptr->data, cmd_ptr->length);
      gstk_curr_inst_ptr->gstk_cmd_cache.cmd_length = cmd_ptr->length;
      gstk_curr_inst_ptr->gstk_cmd_cache.dupe_count = 0;
    }
    gstk_curr_inst_ptr->gstk_cmd_cache.tr_rcvd = FALSE;
  }

  command_qual  = cmd_ptr->data[length_of_TLV_length_field + 5];
  gstk_status   = gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type(
                    type_of_command,
                    command_qual,
                    &cmd_type);

  if((gstk_status == GSTK_SUCCESS)
     &&
     (cmd_type != GSTK_END_PROACTIVE_CMD_REQ)
#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
     &&
     (cmd_type != GSTK_SETUP_MENU_REQ)
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/
    )
  {
    /* Check if the proactive command has been registered by the client
    ** to receive in the raw format
    */
    gstk_status = gstk_util_find_client_with_registered_event(
                      &client_id,
                      cmd_type,
                      GSTK_RAW_FORMAT,
                      GSTK_HANDLE_ALL_FUNC);
    if(gstk_status == GSTK_SUCCESS)
    {
      gstk_status = gstk_preprocess_proactive_cmd(cmd_ptr,
                                                  NULL,
                                                  NULL,
                                                  &cmd_details_ref_id );
      if(gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Failed to pre-process raw proactive command");
        /* If func() returns GSTK_BAD_PARAM, then there is no cmd_details ref id,
        ** so return from here without TR
        */
        if(gstk_status != GSTK_BAD_PARAM)
        {
          gstk_raw_proactive_cmd_cleanup_on_error(reg_info_ptr,
                                                  cmd_type,
                                                  cmd_details_ref_id);
        }
        return gstk_status;
      }/* if(gstk_status != GSTK_SUCCESS) */

      reg_info_ptr = (gstk_client_pro_cmd_reg_data_type *)GSTK_CALLOC(
                        sizeof(gstk_client_pro_cmd_reg_data_type));
      if(reg_info_ptr == NULL)
      {
        gstk_raw_proactive_cmd_cleanup_on_error(reg_info_ptr,
                                                cmd_type,
                                                cmd_details_ref_id);
        return GSTK_MEMORY_ERROR;
      }

      reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr =
       (gstk_raw_cmd_from_card_type *)GSTK_CALLOC(sizeof(gstk_raw_cmd_from_card_type));
      if (reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr == NULL)
      {
        gstk_raw_proactive_cmd_cleanup_on_error(reg_info_ptr, cmd_type, cmd_details_ref_id);
        return GSTK_MEMORY_ERROR;
      }

      reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_buffer_ptr =
        (uint8 *)GSTK_CALLOC(cmd_ptr->length);
      if (reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_buffer_ptr == NULL)
      {
        gstk_raw_proactive_cmd_cleanup_on_error(reg_info_ptr,
                                                cmd_type,
                                                cmd_details_ref_id);
        return GSTK_MEMORY_ERROR;
      }

      reg_info_ptr->data_format_type = GSTK_RAW_FORMAT;
      reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_len =
        uint32toint32(cmd_ptr->length);
      reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.sim_slot_id =
        gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
      reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.tag =
        GSTK_TAG_ORIGINAL_CMD;
      (void)gstk_memcpy(reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_buffer_ptr,
                     cmd_ptr->data,
                     cmd_ptr->length,
                     cmd_ptr->length,
                     cmd_ptr->length);
      reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.cmd_ref_id =
        cmd_details_ref_id;
      reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.user_data =
        gstk_shared_data.gstk_client_table[client_id-1].user_data;

      UIM_MSG_HIGH_2("Client 0x%x has registered for raw bytes for cmd_ptr 0x%x",
                     client_id, type_of_command);
      if (gstk_shared_data.gstk_client_table[client_id-1].client_cb != NULL)
      {
        (gstk_shared_data.gstk_client_table[client_id-1].client_cb)(reg_info_ptr);
        if(gstk_shared_data.gstk_client_table[client_id-1].client_type == GSTK_UI_TYPE)
        {
          gstk_curr_inst_ptr->gstk_is_curr_session_ui_related = TRUE;
        }
      }
      else
      {
        gstk_raw_proactive_cmd_cleanup_on_error(reg_info_ptr,
                                                cmd_type,
                                                cmd_details_ref_id);
        UIM_MSG_ERR_1("client 0x%x cb is null!", client_id);
        return GSTK_ERROR;
      }
      gstk_free(reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_buffer_ptr);
      gstk_free(reg_info_ptr->pro_cmd_reg_data.raw_req_rsp_ptr);
      gstk_free(reg_info_ptr);

      /* start timer */
      gstk_util_start_client_response_timer(cmd_details_ref_id,
                                            GSTK_TERM_RSP_WAIT_TIME);

      return GSTK_SUCCESS;
    }/* gstk_util_find_client_with_registered_event */
  }/* if(gstk_status == GSTK_SUCCESS && (cmd_type != GSTK_END_PROACTIVE_CMD_REQ)) */
   /*
   ** WARNING
   ** If count position is modified, be sure to
   ** correct device debug statements after switch
   */

  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_DATA_CARD, GSTK_SLOT_ID_MAX) == TRUE)
  {
    switch(type_of_command)
    {
      case GSTK_CMD_STK_DISPLAY_TEXT:
      case GSTK_CMD_STK_GET_INKEY:
      case GSTK_CMD_STK_GET_INPUT:
      case GSTK_CMD_STK_SELECT_ITEM:
      case GSTK_CMD_STK_SET_UP_MENU:
        gstk_status = gstk_unknown_unsupported_proactive_cmd_req(cmd_ptr,
                                                 GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES);
        return gstk_status;
      default:
        break;
    }
  }

  switch( type_of_command )
  {
  case GSTK_CMD_STK_POLL_INTERVAL:
  case GSTK_CMD_STK_POLLING_OFF:
    gstk_status = gstk_polling_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SET_UP_EVENT_LIST:
    gstk_status = gstk_setup_evt_list_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SET_UP_CALL:
    gstk_status = gstk_setup_mo_call_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SEND_SS:
    gstk_status = gstk_send_ss_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_USSD:
    gstk_status = gstk_send_ussd_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_LAUNCH_BROWSER:
    if( gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_LAUNCH_BROWSER,&cmd_enable)
        == GSTK_SUCCESS)
    {
      if(!cmd_enable)
      {
        gstk_status =
          gstk_unknown_unsupported_proactive_cmd_req(cmd_ptr,
                                                     GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES);
      }
      else
      {
        gstk_status = gstk_launch_browser_req(cmd_ptr);
      }
    }
    else
    {
      gstk_status = gstk_launch_browser_req(cmd_ptr);
    }
    break;

  case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
    gstk_status = gstk_send_sms_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_PLAY_TONE:
    gstk_status = gstk_play_tone_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_DISPLAY_TEXT:
    gstk_status = gstk_display_text_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_GET_INKEY:
    gstk_status = gstk_get_inkey_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_GET_INPUT:
    gstk_status = gstk_get_input_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SELECT_ITEM:
    gstk_status = gstk_select_item_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SET_UP_MENU:
    gstk_status = gstk_setup_menu_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
    if( gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_NET_LOC,&cmd_enable)
        == GSTK_SUCCESS)
    {
      if(!cmd_enable)
      {
        gstk_status =
          gstk_unknown_unsupported_proactive_cmd_req(cmd_ptr,
                                                     GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES);
      }
      else
      {
        gstk_status = gstk_provide_local_info_req(cmd_ptr);
      }
    }
    else
    {
      gstk_status = gstk_provide_local_info_req(cmd_ptr);
    }
    break;

  case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
    gstk_status = gstk_setup_idle_mode_text_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_END_OF_PROACTIVE_SES:
    gstk_status = gstk_end_proactive_session(cmd_ptr, GSTK_TAG_ORIGINAL_CMD);
    break;

  case GSTK_CMD_STK_REFRESH:
    if (gstk_nv_get_feature_status(
          GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
          gstk_curr_inst_ptr->slot_id) == TRUE)
    {
      if (gstk_curr_inst_ptr->loc_info_env_res == GSTK_ATTACH_CTRL_INVALID)
      {
        gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
      }
      gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
      gstk_util_send_attach_ctrl_res();
    }
    gstk_status = gstk_refresh_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_MORE_TIME:
    gstk_status = gstk_more_time_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_TIMER_MANAGEMENT:
    gstk_status = gstk_timer_management_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_LANG_NOTIFICATION:
    gstk_status = gstk_lang_notification_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SEND_DTMF:
    gstk_status = gstk_send_dtmf_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_OPEN_CHANNEL:
    if(cmd_ptr->message_header.command_id == GSTK_PRO_CMD_ID_OPEN_CH_IMSI_M_READ_FAIL)
    {
      if(gstk_curr_inst_ptr->open_ch_imsi_m_ptr == NULL)
      {
        gstk_status = GSTK_BAD_PARAM;
        break;
      }
      else
      {
        gstk_curr_inst_ptr->open_ch_imsi_m_ptr = NULL;
      }
    }
    /* Read IMSI_M for 1x */
    if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH, gstk_curr_inst_ptr->slot_id) == TRUE)
    {
      if (!gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x)
      {
        UIM_MSG_HIGH_0("Do not read IMSI_M before subscription ready");
        gstk_status = gstk_open_ch_req(cmd_ptr);
        break;
      }

      if (gstk_curr_inst_ptr->otasp_call_id >= 0)
      {
        UIM_MSG_HIGH_0("OTASP call in progress, send error TR for Open Channel");
        gstk_shared_data.is_open_ch_ok = FALSE;
        gstk_status = gstk_open_ch_req(cmd_ptr);
        break;
      }

      if(gstk_curr_inst_ptr->ds_sys_status_info.num_avail_sys > 0)
      {
        uint8 i = 0;

        UIM_MSG_HIGH_2("num_avail_sys = 0x%x, loc_status = 0x%x",
                       gstk_curr_inst_ptr->ds_sys_status_info.num_avail_sys,
                       gstk_curr_inst_ptr->gstk_next_curr_location_status);

        UIM_MSG_HIGH_2("slot = 0x%x, command_id = 0x%x",
                       gstk_shared_data.mmgsdi_sess_info[i].slot,
                       cmd_ptr->message_header.command_id);

        (void)gstk_util_validate_slot_sessions();

        /* find the index in mmgsdi_sess_info[] corresponding to 1X_PROV_PRI_SESS */
        for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
        {
          if(gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_PRI_SESSION)
          {
            UIM_MSG_HIGH_2("1X_PROV_PRI_SESSION found at index 0x%x, slot 0x%x",
                           i, gstk_shared_data.mmgsdi_sess_info[i].slot);
            break;
          }
        }

        if (
#ifndef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
            /* If 1X Activation is not preferred, use 1X if that is the
               only system available */
            gstk_curr_inst_ptr->ds_sys_status_info.avail_sys[0].rat_value == DS_SYS_RAT_EX_3GPP2_1X &&
            gstk_curr_inst_ptr->ds_sys_status_info.num_avail_sys == 1 &&
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */
            gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_NORMAL_SERVICE &&
            gstk_shared_data.gstk_is_imsi_m_provisioned != TRUE &&
            i != GSTK_MMGSDI_SESS_INFO_SIZE &&
            gstk_shared_data.mmgsdi_sess_info[i].slot != GSTK_SLOT_ID_MAX &&
            cmd_ptr->message_header.command_id != GSTK_PRO_CMD_ID_OPEN_CH_IMSI_M_READ_FAIL)
        {
#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
          /* If 1x activation is preferred, but 1x RAT is unavailable and
             device is non-SVLTE/SRLTE, process as regular OPEN CHANNEL */
          if((gstk_curr_inst_ptr->bmsk_3gpp2 & GSTK_ACCESS_TECH_CDMA_MASK) == 0 &&
             gstk_curr_inst_ptr->subs_feature_mode != GSTK_SUBS_FEATURE_MODE_NONE &&
             gstk_curr_inst_ptr->subs_feature_mode != GSTK_SUBS_FEATURE_MODE_SVLTE &&
             gstk_curr_inst_ptr->subs_feature_mode != GSTK_SUBS_FEATURE_MODE_SRLTE)
          {
            UIM_MSG_HIGH_1("1x RAT not available and Subs Mode: 0x%x, proceed to OPEN CH immediately",
                           gstk_curr_inst_ptr->subs_feature_mode);
            gstk_status = gstk_open_ch_req(cmd_ptr);
            break;
          }
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */

          if (gstk_curr_inst_ptr->otasp_failure_cnt >= GSTK_MAX_OTASP_RETRY_CNT)
          {
            UIM_MSG_HIGH_0("Already attempted 1xRTT CSIM programming 3 times. Proceed to OPEN CH immediately");
            /* If only 1X RAT is available, set the command_id field to
               GSTK_OPEN_CH_CNF to indicate that we need to send back error
               Terminal Response immediately, else process OPEN CHANNEL */
            if((gstk_curr_inst_ptr->ds_sys_status_info.avail_sys[0].rat_value ==
                DS_SYS_RAT_EX_3GPP2_1X) &&
               (gstk_curr_inst_ptr->ds_sys_status_info.num_avail_sys == 1))
            {
              cmd_ptr->message_header.command_id = GSTK_OPEN_CH_CNF;
            }
            gstk_status = gstk_open_ch_req(cmd_ptr);
          }
          else
          {
            /* Store the open channel command and process it after we know
               about the status of IMSI_M */
            if(gstk_curr_inst_ptr->open_ch_imsi_m_ptr != NULL)
            {
              gstk_free((void *)gstk_curr_inst_ptr->open_ch_imsi_m_ptr);
              gstk_curr_inst_ptr->open_ch_imsi_m_ptr = NULL;
            }
            gstk_curr_inst_ptr->open_ch_imsi_m_ptr =
              (gstk_proactive_cmd_type *)GSTK_CALLOC(sizeof(gstk_proactive_cmd_type));

            UIM_MSG_HIGH_0("Attempting IMSI_M read...");

            if(gstk_curr_inst_ptr->open_ch_imsi_m_ptr == NULL)
            {
              gstk_status = gstk_open_ch_req(cmd_ptr);
              break;
            }

            (void)gstk_memcpy((void *)gstk_curr_inst_ptr->open_ch_imsi_m_ptr,
                              (void *)cmd_ptr,
                              sizeof(gstk_proactive_cmd_type),
                              sizeof(gstk_proactive_cmd_type),
                              sizeof(gstk_proactive_cmd_type));


            /* Queue command to MMGSDI to read IMSI_M */
            file_access.access_method = MMGSDI_BY_PATH_ACCESS;
            file_access.file.path_type.path_len = 0x3;
            file_access.file.path_type.path_buf[0] = GSTK_CSIM_IMSI_M_PATH_BUF_1;
            file_access.file.path_type.path_buf[1] = GSTK_CSIM_IMSI_M_PATH_BUF_2;
            file_access.file.path_type.path_buf[2] = GSTK_CSIM_IMSI_M_PATH_BUF_3;
            file_access.file.path_type.path_buf[3] = GSTK_CSIM_IMSI_M_PATH_BUF_4;
            file_access.file.path_type.path_buf[4] = GSTK_CSIM_IMSI_M_PATH_BUF_5;

            if (mmgsdi_session_read_transparent(
                        gstk_shared_data.mmgsdi_sess_info[i].sess_id,
                        file_access,
                        0,
                        0,
                        gstk_mmgsdi_cmd_cb,
                        (mmgsdi_client_data_type)gstk_curr_inst_ptr->open_ch_imsi_m_ptr) != MMGSDI_SUCCESS)
            {
              UIM_MSG_ERR_0("failed to read IMSI_M!");
              gstk_free((void *)gstk_curr_inst_ptr->open_ch_imsi_m_ptr);
              gstk_status = gstk_open_ch_req(cmd_ptr);
            }
            else
            {
              gstk_status = GSTK_SUCCESS;
            }
          }
        }
        else /* Either IMSI_M is already provisioned or ME not on CDMA-1x or normal service, proceed with OPEN CH */
        {
           UIM_MSG_HIGH_3("gstk_is_imsi_m_provisioned = 0x%x, rat = 0x%x, location_status = 0x%x",
                          gstk_shared_data.gstk_is_imsi_m_provisioned,
                          gstk_curr_inst_ptr->ds_sys_status_info.avail_sys[0].rat_value,
                          gstk_curr_inst_ptr->gstk_next_curr_location_status);
           gstk_status = gstk_open_ch_req(cmd_ptr);
        }
      }
      else /* Getting RAT from DS failed */
      {
        UIM_MSG_ERR_1("Failed to get RAT mask from DS 0x%x", ds_errno);
        gstk_shared_data.is_open_ch_ok = FALSE;
        gstk_status = gstk_open_ch_req(cmd_ptr);
      }
    }
    else /* IMSI_M not enabled - proceed normally */
    {
      UIM_MSG_HIGH_0("IMSI_M provisioning not needed");
      gstk_status = gstk_open_ch_req(cmd_ptr);
    }
    break;

  case GSTK_CMD_STK_CLOSE_CHANNEL:
    gstk_status = gstk_close_ch_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_RECEIVE_DATA:
    gstk_status = gstk_receive_data_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SEND_DATA:
    gstk_status = gstk_send_data_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_GET_CHANNEL_STATUS:
    gstk_status = gstk_get_ch_status_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_ACTIVATE:
    gstk_status = gstk_activate_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_RUN_AT_COMMAND:
  case GSTK_CMD_STK_PERFORM_CARD_APDU:
  case GSTK_CMD_STK_POWER_ON_CARD:
  case GSTK_CMD_STK_POWER_OFF_CARD:
  case GSTK_CMD_STK_GET_READER_STATUS:
    gstk_status =
      gstk_unknown_unsupported_proactive_cmd_req(cmd_ptr,
                                                 GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES);
    break;
  default:
    gstk_status =
      gstk_unknown_unsupported_proactive_cmd_req(cmd_ptr,
                                                 GSTK_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME);
      break;
  }
   return(gstk_status);
} /* gstk_process_proactive_command */

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
gstk_status_enum_type gstk_end_proactive_session(
    gstk_proactive_cmd_type* cmd,
    gstk_tag_enum_type       gstk_tag)
{
    gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
    gstk_cmd_from_card_type             request;

    UIM_MSG_HIGH_0("END Proactive session ");

    if(cmd == NULL)
    {
      UIM_MSG_ERR_0("cmd ERR:NULL");
      return GSTK_BAD_PARAM;
    }

    /* initialize request to 0 */
    memset(&request, 0, sizeof(gstk_end_proactive_req_type)+sizeof(gstk_exp_hdr_type));

    /* build the end proactive cmd request into the gstk_exp.h _req structure */

    /* Message Header */
    request.hdr_cmd.command_id           = GSTK_END_PROACTIVE_CMD_REQ;
    request.hdr_cmd.cmd_detail_reference = 0;
    request.hdr_cmd.tag                  = gstk_tag;

    /* Check if it is gstk generated end session */
    if (cmd->data[GSTK_GENERATED_END_SES_INDEX] != GSTK_GENERATED_END_SES) {
      request.cmd.end_pro_cmd_req.end_from_card = TRUE;
    }

    /* Send command to all clients */
    gstk_status = gstk_util_send_message_to_clients(&request);

    if (gstk_status != GSTK_SUCCESS) {
      UIM_MSG_HIGH_0("Can't SEND End proactive session to clients");
    }

    /* Call MMGSDI API to inform the end of session after VPE is sent to card so that
       it can trigger internal refresh to read IMSI related EFs */
    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);
    if (gstk_curr_inst_ptr->read_imsi == TRUE) {
      gstk_curr_inst_ptr->read_imsi = FALSE;
      UIM_MSG_HIGH_0("Calling mmgsdi_gstk_loc_env_rsp_received");
      mmgsdi_gstk_loc_env_rsp_received(
        (mmgsdi_slot_id_enum_type)cmd->message_header.sim_slot_id, /* slot_id */
        (mmgsdi_location_status_enum_type)0, /* loc_status */
        0, /* mcc */
        0, /* mnc */
        gstk_mmgsdi_cmd_cb, /* response_cb_ptr */
        0);
    }

    /* retry envelope if any in retry queue*/
    if(gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_IN_QUEUE)
    {
      if(gstk_util_retry_envelope_cmd() != GSTK_SUCCESS)
      {
        /* build envelope response back to the client */
        gstk_status = gstk_build_error_envelope_report(
          gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id,
          int32touint32((int32)gstk_shared_data.envelope_cmd_ref_table[gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id].expected_cmd_rsp_type),
          GSTK_ENVELOPE_CMD_FAIL);
        gstk_util_clear_retry_envelope_data(gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
      }
    }

    return gstk_status;

}/*lint !e818 *cmd suppression for externalized function */
/* gstk_end_proactive_session */

/*===========================================================================
FUNCTION gstk_build_unknown_unsupported_term_rsp

DESCRIPTION
  This function send terminal response for an unknown or unsupported
  proactive command.

PARAMETERS
  cmd_details_ref_id: [Input]
  result            : [Input]
  cnf_type          : [Input]

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
void gstk_build_unknown_unsupported_term_rsp(
    uint32                              cmd_details_ref_id,
    gstk_general_result_enum_type       result,
    gstk_cmd_enum_type                  cnf_type)
{
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param;

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  UIM_MSG_HIGH_0("Build Unknown / Unsupported Proactive command");

  /* initialize dummy_extra_param */
  dummy_extra_param.present = FALSE;

  /* Send Terminal Response to Card */
  gstk_status = gstk_send_terminal_response(
                  gstk_curr_inst_ptr->client_id,     /* client_id */
                  0,                  /* user_data */
                  cmd_details_ref_id, /* command_details_ref */
                  command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,  /* command_num */
                  cnf_type,           /* command response */
                  result,             /* general_result */
                  NULL,               /* additional result */
                  &dummy_extra_param  /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) {
    UIM_MSG_ERR_1("TR fail, 0x%x", gstk_status);
  }
} /* gstk_build_unknown_unsupported_term_rsp */

/*===========================================================================
FUNCTION gstk_unknown_unsupported_proactive_cmd_req

DESCRIPTION
  This function parses unknown or unsupported proactive command and populates
  result as GSTK_BEYOND_ME_CAPABILITIES or GSTK_COMMAND_TYPE_UNKNOWN respectively.

PARAMETERS
  command:     [Input] Pointer to command info from card
  result_type: [Input] The result that needs to be sent back to the card.

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
    gstk_proactive_cmd_type        * command_ptr,
    gstk_general_result_enum_type  result_type
)
{

  gstk_status_enum_type               gstk_status         = GSTK_SUCCESS;
  uint8                               *cmd_data_ptr       = NULL;
  int32                               cmd_data_len        = 0;
  uint32                              cmd_details_ref_id  = 0;

  UIM_MSG_HIGH_0(" IN gstk_unknown_unsupported_proactive_cmd_req");

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data, and specifies the length of
  ** the cmd_data in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );
  /* There exists mandatory tlv, so cmd_data should not be null */
  if (gstk_status != GSTK_SUCCESS)
  {
    result_type = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
  }
  /* release cmd_data as this is not needed anymore */
  if(cmd_data_ptr != NULL)
  {
    gstk_free(cmd_data_ptr);
    cmd_data_ptr = NULL;
  }
  /* cmd_details_ref_id will be valid even if gstk_preprocess_proactive_cmd() does not
     return a SUCCESS
  */
  gstk_build_unknown_unsupported_term_rsp(cmd_details_ref_id,
                                          result_type,
                                          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].expected_cmd_rsp_type);
  return(gstk_status);

}/* gstk_unknown_unsupported_proactive_cmd_req */

/*===========================================================================
FUNCTION gstk_cmp_with_cached_cmd

DESCRIPTION

  This function compres the cached proactive command with ongoing command

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
gstk_status_enum_type gstk_cmp_with_cached_cmd (
  const gstk_proactive_cmd_type *cmd_ptr
)
{
  /* Compare lengths of cached & current proactive commands */
  if (cmd_ptr->length == gstk_curr_inst_ptr->gstk_cmd_cache.cmd_length)
  {
    /* Compare cached cmd and new cmd */
    if( 0 == memcmp ((void *)gstk_curr_inst_ptr->gstk_cmd_cache.cmd,
                     (void *)cmd_ptr->data,
                     cmd_ptr->length))
    {
      return GSTK_SUCCESS;
    }
  }

  return GSTK_ERROR;
} /* gstk_cmp_with_cached_cmd */
