/*===========================================================================


            G S T K   E N V E L O P E   R S P

GENERAL DESCRIPTION
  This source file contains functions to handle envelope response from the
  card


FUNCTIONS
  gstk_uim_envelope_report
    This is the callback function for envelope response.

  gstk_start_env_retry_timer
    This function starts envelope retry timer based on retry counter value

  gstk_menu_selection_rsp
    This function builds the menu selection rsp and sends it to client

  gstk_evt_dl_rsp
    This function builds the event download rsp and sends it to client

  gstk_sms_pp_dl_rsp
    This function builds the sms pp download rsp and sends it to client

  gstk_cc_rsp
    This function builds the call control rsp and sends it to client

  gstk_mo_sms_ctrl_rsp
    This function builds the sms control download rsp and sends it to client

  gstk_timer_expire_rsp
    This function builds the timer expire rsp and sends it to client

  gstk_cell_broadcast_dl_rsp
    This function builds the cell broadcast download rsp and sends it to client

  gstk_process_envelope_rsp
    This function performs a lookup on the command id with the
    envelope_cmd_ref_table and based on the envelope rsp type, it calls the
    corresponding envelope rsp function so that GSTK can build the corrresponding
    envelope response and sends it to the client

  gstk_raw_envelope_rsp
    This function builds the raw envelope response and sends it to client

  gstk_non_idle_st_raw_env_err_rsp
    This function builds the raw envelope non idle state error rsp and sends
    it to client

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_envelope_rsp.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
01/08/16   vr      Reset usr_actvty and idl_scrn for raw envelope rsp evt dl
12/14/15   gs      Fix PDP context activation TLV parsing logic
12/19/14   gm      Added support to read envelope retry data from NV
11/21/14   gm      Reset rrc cell change indication flag
11/11/14   shr     Fixed compilation warnings
10/22/14   dy      Fix warnings
10/13/14   gm      Clear memory when sms_pp retry
09/19/14   gm      Added support for SMS_PP data download retry
09/09/14   vr      Reduce stack usage
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/20/14   gm      Added envelope response in priority-queue
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
07/31/14   dy      Fixing KW errors
07/11/14   shr     Add Support for NV Refresh
07/10/14   gm      Send success response for mo-sms/call control in non-idle state
06/24/14   sw      Linux build error fixed for declaration of
                   gstk_start_env_retry_timer()
06/03/14   sw      Implemented retry for call-control envelopes
05/28/14   gm      Support of CSG Rel-11 feature
05/15/14   dy      Fix gstk_cc_rsp() parser for EPS PDN activation
04/03/14   dy      Replace gstk_malloc() with macro
02/27/14   sw      Extended retry mechanism
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/17/14   gm      Added support for IMS communication control
01/29/14   gm      Send fail response for mo-sms/call control in non-idle state
01/27/14   gm      Clear last location info if card is busy
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
10/04/13   gs       F3 Message reduction
10/01/13   gm      Update slot information when calling client call-back
09/13/13   gm      Support for Recovery and hot-swap
09/03/13   jd      Fix client_id/slot_id issue for MO SMS error env rsp
08/01/13   bd      Fixed CCP2 parsing failure
06/24/13   xz      Support IMS Registration event download
06/20/13   vr      Triton TSTS changes
06/19/13   hh      Send error response to clients when env ref table is full
06/12/13   gm      Setting GSTK_ESTK_CMD_SIG everytime when command is posted
                   to GSTK task
05/13/13   vr      Update slot id for Cell Broadcast download envelope
05/01/13   vr      Fix command processing logic in GSTK
04/11/13   bd      Avoid notifying NAS to ATTACH before IMSI switch
04/09/13   vr      Use safer versions of memcpy() and memmove()
03/21/13   vr      update slot for event download envelopes before calling
                   client callback function
02/25/13   xz      Process attach ctrl req when envelope is in retry
02/21/13   hn      Enhancement of Loc Status and VPE retry
02/11/13   bd      Support for (e)HRPD access tech
01/31/13   gm      Changes for SMS-PP Security improvement
11/13/12   bd      Support for Closed Subscriber Group feature
09/25/12   hk      In NO SIM state there are envelopes queued for
                   GSTK_CELL_BROADCAST_DOWNLOAD_IND which are not cleaned
09/18/12   hk      Support to read IMSI after getting VPE resp
07/26/12   sg      Set SIG to reduce delay in processing commands from queue
06/05/12   sg      When CDMA is RAT dont check for RRC LocInfo difference
03/28/12   av      Replaced feature flags with NVITEM
03/16/12   yt      Remove tmc.h inclusion
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/03/12   nb      Changing FEATURE_GSTK_REPORT_SW1_SW2 to a bit mask
10/18/11   av      Removed features that are always OFF
09/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
05/21/11   bd      Update pending location status evt flag when envelope command
                   is received in non-idle state
05/12/11   xz      Fix an issue of infinite loop while process envelope cmd
05/11/11   xz      Fix for Location Envelope
04/05/11   xz      Changes to support VPE ATTACH CTRL
03/23/11   sg      Add code for Loc Info envelope retry
03/24/11   js      Fix for LTE Call Control.
03/10/11   xz      Fix issue of CC rsp more than 127 bytes
02/02/11   dd      Fix env_ref_id
12/23/10   bd      Fixed issue of passing wrong data structure object to
                   client call back
09/23/10   yb      Fixed reporting error on receiving warnings for SMS PP DL
08/26/10   xz      Add support of EPS Call Control
07/13/10   bd      Fixed issue of sending incorrect location status envelope during
                   pending location status
06/29/10   nb      Merged Chanegs for DS DS
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
05/11/10   xz      Add support of network rejection event
04/12/10   bd      Fixed issue of ME not sending PDP activation request when CC
                   "with modification" is enabled on the card.
04/07/10   xz      Fix the issue of delay in FETCH after envelope rsp
03/08/10   sg      Location information envelope command
02/26/10   yt      Fixed KW errors
02/15/10   nb      Moved Dual Slot changes
01/17/10   dd      Forward SMS envelope rsponse to UI client
12/09/09   bd      Fixed issue of reading other optional TLVs when address TLV
                   is not present in CC envelope rsp
09/26/09   kp      ZI memory reduction changes
08/27/09   sg      Fixed Lint errors
07/02/09   xz      Set status of SMS PP DL rsp to FAIL for 0x62XX or 0x63XXX
06/22/09   xz      Fixed infinite loop issue when an error TLV exists in CC rsp
06/10/09   yb      Fix to send status word as received in get response to AT COP
                   even if card returns error for envelope sent.
05/18/09   sg      Added static keyword to function defintion in
                   1)gstk_check_valid_tag_for_cc_rsp
                   2)gstk_is_tag_valid_for_command
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
03/02/09   xz      Provide SW1 and SW2 support for raw APDU responses
02/11/09   yb      Added support to handle envelope response for Channel
                   status event in gstk_process_envelope_rsp
11/13/08   xz      Fixed klockwork errors
09/14/08   sk      Fixed lint and klockwork errors
09/08/08   xz      Envelope rsp fails due to invalid client id
08/22/08   xz      Move processing of raw envelope rsp to switch statement
08/21/08   sk      Changed function name.
08/08/08   xz      Add support for raw envelope response
08/04/08   sk      Fixed sending of envelope response to clients.
06/20/08   xz      Added support for 6F C2 for call control envelope response
05/06/08   sk      Added support for event download envelope retry
03/17/08   sk      Fixed klockwork errors
02/23/08   yb      Added support for parsing unexpected and unknown tlvs for
                   envelope call control response
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
08/21/06   sk      Added featurization for GPRS call control
07/25/06   sk      Added support to handle envelope cmds in non idle state
06/24/06   sk      Added data available envelope enum to avoid holding up
                   envelope ref id.
06/06/06   sk      Added support for network search mod
05/15/06   tml     GPRS Call Control support
05/03/06   tml     lint
04/06/06/  tml     Populate information even if UIM_FAILED returned
                   but do not copy the data from uim report
03/16/06   sk      Fixed potential data abort problem
11/09/05   sk      Fixed Lint Errors
09/15/05   tml     Fixed link error: move gstk_build_error_envelope_report
                   to gstk_envelope_rsp.h
08/10/05   sk      Fixed call control type in gstk_cc_rsp()
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added new fn gstk_build_error_envelope_report()
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
01/10/05  sk/tml   Changed parser function for ussd string
11/23/04    sk     Added SAT/USAT data support
08/10/04   tml     Added checking in CALL CONNECTED envelope response to
                   determine if more CALL CONNECTED envelope is required to be
                   sent to the card
06/04/04   tml     timer management and language notification support
06/03/04   tml     Added 6F00 handling for Call Ctrl and MO SMS Ctrl
05/28/04   tml     Added F3 message for envelope status word from UIM
03/01/04   tml     Merged from branch - fixes for location status env command
07/07/03   tml     Removed unnecessary messages
05/16/03   tml     Linted and fixed call control issue
04/29/03   tml     Properly initialized data struct
04/21/03   tml     Correctly interrept UIM status words
04/15/03   tml     Fixed F3 message for MO sms control response
03/20/03   tml     MO SMS control fix
03/14/03   tml     Call Control bug fix
03/11/03   tml     Fixed call control comparison error
03/06/03   tml     Updated enum names
02/25/03   tml     Added user data support
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "intconv.h"
#include <stringl/stringl.h>
#include "gstk_envelope_rsp.h"
#include "gstk_envelope_cmd.h"
#include "uim.h"
#include "gstkutil.h"
#include "mmgsdilib_v.h"

#include "string.h"
#include "err.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_uim_envelope_report

DESCRIPTION

  This is the callback function for envelope response.  UIM will call
  this function when it receives envelope response from the Card
  Based on the return status of the envelope response, this function
  will build the corresponding envelope resp type and put it to the
  **front** of the gstk command queue

PARAMETERS
  report_ptr: [Input] Pointer to the UIM envelope report

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
void gstk_uim_envelope_report (uim_rpt_type *report_ptr)
{
  gstk_task_cmd_type   * task_cmd_ptr    = NULL;
  uint32                 ref_id          = 0;
  gstk_cmd_enum_type     type_of_command = GSTK_CMD_ENUM_MAX;
  gstk_slot_id_enum_type slot_id         = GSTK_SLOT_ID_MAX;

  if(report_ptr == NULL)
  {
    UIM_MSG_ERR_0("report ERR:NULL");
    return;
  }

  UIM_MSG_HIGH_3("GSTK recv UIM envelope rsp, 0x%x, 0x%x, Report Status 0x%x",
                 report_ptr->sw1, report_ptr->sw2, report_ptr->rpt_status);

  if (report_ptr->rpt_type != UIM_ENVELOPE_R)
  {
    UIM_MSG_ERR_0("ENVELOPE expected in CB");
    return;
  }

  ref_id = GSTK_ENV_CMD_TABLE_INDEX(report_ptr->user_data);
  if (ref_id >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("invalid ref_id 0x%x!", ref_id);
    return;
  }

  slot_id = (gstk_slot_id_enum_type)report_ptr->slot;
  if(!GSTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("Invalid Slot : 0x%x", slot_id);
    return;
  }
  UIM_MSG_HIGH_2("Envelope response received userdata 0x%x, unique_ref_id 0x%x",
                 report_ptr->user_data,
                 gstk_shared_data.envelope_cmd_ref_table[ref_id].unique_ref_id);
  if(report_ptr->user_data != gstk_shared_data.envelope_cmd_ref_table[ref_id].unique_ref_id)
  {
    UIM_MSG_ERR_0("Envelope response received for invalid request");
    return;
  }
  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  if (gstk_shared_data.envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type == GSTK_DATA_AVAIL_EVT_IND_RSP ||
      gstk_shared_data.envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type == GSTK_CH_STATUS_EVT_IND_RSP  ||
      gstk_shared_data.envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type == GSTK_CC_IND_RSP)
  {
    gstk_instances_ptr[slot_id - 1]->gstk_bip_env_in_progress = FALSE;
  }
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /* Leaving gstk critical section */

  /* get command buf */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr == NULL )
  {
    return;
  }
  /* successfully obtained command buffer . Now build the envelope response */
  task_cmd_ptr->cmd.envelope_response.message_header.sim_slot_id = slot_id;
  task_cmd_ptr->cmd.envelope_response.message_header.command_group = GSTK_ENVELOPE_RSP;
  task_cmd_ptr->cmd.envelope_response.message_header.command_id = (uint8)ref_id;
  /* index location in the gstk_shared_data.envelope_cmd_ref_table */
  task_cmd_ptr->cmd.envelope_response.message_header.user_data = ref_id;
  if(report_ptr->rpt_status == UIM_PASS)
  {
    task_cmd_ptr->cmd.envelope_response.length = report_ptr->rpt.envelope.data_length;
    memset(task_cmd_ptr->cmd.envelope_response.data, 0x00, GSTK_MAX_DATA_SIZE);
    if(report_ptr->rpt.envelope.data_length > 0)
    { /* has data */
      UIM_MSG_HIGH_1("Copy 0x%x bytes of envelope rsp.",
                     report_ptr->rpt.envelope.data_length);
      if(gstk_memcpy(task_cmd_ptr->cmd.envelope_response.data,
                     report_ptr->rpt.envelope.data,
                     report_ptr->rpt.envelope.data_length,
                     sizeof(task_cmd_ptr->cmd.envelope_response.data),
                     sizeof(report_ptr->rpt.envelope.data)) <
         report_ptr->rpt.envelope.data_length)
      {
        UIM_MSG_ERR_0("gstk_uim_envelope_report(): gstk_memcpy failed");
        gstk_task_free_cmd_buf(task_cmd_ptr);
        return;
      }
    }
  }
  /* initialize rsp_status to FAIL */
  task_cmd_ptr->cmd.envelope_response.rsp_status = GSTK_ENVELOPE_CMD_FAIL;
  /* populate sw1 and sw 2 */
  if(SW1_DLOAD_ERR == report_ptr->rpt.envelope.get_resp_sw1)
  {
    task_cmd_ptr->cmd.envelope_response.sw1 = (int)report_ptr->rpt.envelope.get_resp_sw1;
    task_cmd_ptr->cmd.envelope_response.sw2 = (int)report_ptr->rpt.envelope.get_resp_sw2;
  }
  else
  {
  task_cmd_ptr->cmd.envelope_response.sw1 = (int)report_ptr->sw1;
  task_cmd_ptr->cmd.envelope_response.sw2 = (int)report_ptr->sw2;
  }

  /* pass warning 0x62XX or 0x63XX to SMS PP DL rsp */
  type_of_command =
    gstk_shared_data.envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type;
  if (type_of_command == GSTK_SMS_PP_DOWNLOAD_IND_RSP)
  {
    if (SW1_WARNINGS1 == report_ptr->rpt.envelope.get_resp_sw1 ||
        SW1_WARNINGS2 == report_ptr->rpt.envelope.get_resp_sw1 ||
        SW1_PROBLEM   == report_ptr->rpt.envelope.get_resp_sw1)
    {
      task_cmd_ptr->cmd.envelope_response.sw1 =
        (int)report_ptr->rpt.envelope.get_resp_sw1;
      task_cmd_ptr->cmd.envelope_response.sw2 =
        (int)report_ptr->rpt.envelope.get_resp_sw2;
    }
  }

  switch(task_cmd_ptr->cmd.envelope_response.sw1)
  {
  case SW1_NORMAL_END:
    if(task_cmd_ptr->cmd.envelope_response.sw2 == (int)SW2_NORMAL_END)
    {
      task_cmd_ptr->cmd.envelope_response.rsp_status = GSTK_ENVELOPE_CMD_SUCCESS;
    }
    break;
  case SW1_END_FETCH:
    task_cmd_ptr->cmd.envelope_response.rsp_status = GSTK_ENVELOPE_CMD_SUCCESS;
    /* Overwirte the SW1 with 90 even though UIMDRV sends 91 for all commands except
         location information envelope */
    /* This change is to not impact any client with new change from driver sending 91 (sw1) */
    /* TS 102 221, 7.4.2, CAT layer allowes only 9000, 62XX, 63XX, 6F XX*/
    if(type_of_command != GSTK_LOC_INFO_ENVELOPE_RSP)
    {
      task_cmd_ptr->cmd.envelope_response.sw1 = SW1_NORMAL_END;
      task_cmd_ptr->cmd.envelope_response.sw2 = SW2_NORMAL_END;
    }
    break;
  case SW1_BUSY:
    if(task_cmd_ptr->cmd.envelope_response.sw2 == (int)SW2_NORMAL_END)
    {
      task_cmd_ptr->cmd.envelope_response.rsp_status = GSTK_ENVELOPE_CMD_CARD_BUSY;
    }
    break;
  default:
    break;
  }
  UIM_MSG_HIGH_2("UIM envelope rsp queued to front of GSTK queue, 0x%x, 0x%x",
                 task_cmd_ptr->cmd.envelope_response.sw1,
                 task_cmd_ptr->cmd.envelope_response.sw2);

  /* set ESTK CMD Q SIG also to optimize the time for processing subsequent command in GSTK Queue*/
  gstk_task_put_cmd_priority_buf(task_cmd_ptr);
}/*lint !e818 *report_ptr suppression for externalized function */
/* gstk_uim_envelope_report */

/*===========================================================================
FUNCTION gstk_build_error_envelope_report

DESCRIPTION
  This is the function used to build the error envelope
report to be sent back to the client.

PARAMETERS
  report: [Input] Pointer to the UIM envelope report

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
gstk_status_enum_type gstk_build_error_envelope_report
(
  uint32                                env_ref_id,
  uint32                                command_id,
  gstk_general_envelope_rsp_enum_type   rsp_type
)
{
  gstk_task_cmd_type   * task_cmd;

  UIM_MSG_HIGH_1("Building an error envelope response, env_ref_id: 0x%x",
                 env_ref_id);

  if (env_ref_id > GSTK_MAX_PROACTIVE_COMMAND)
  {
    return GSTK_ERROR;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    return GSTK_ERROR;
  }
  task_cmd->cmd.envelope_response.message_header.sim_slot_id =
    (gstk_slot_id_enum_type)gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  task_cmd->cmd.envelope_response.message_header.command_group = GSTK_ENVELOPE_RSP;
  task_cmd->cmd.envelope_response.message_header.command_id = command_id;
  /* index location in the gstk_shared_data.envelope_cmd_ref_table */
  task_cmd->cmd.envelope_response.message_header.user_data = env_ref_id;
  task_cmd->cmd.envelope_response.length = 0;
  memset(task_cmd->cmd.envelope_response.data, 0x00, GSTK_MAX_DATA_SIZE);

  /* initialize rsp_status to FAIL */
  task_cmd->cmd.envelope_response.rsp_status = rsp_type;

  /* populate sw1 and sw 2 */
  task_cmd->cmd.envelope_response.sw1 = 0;
  task_cmd->cmd.envelope_response.sw2 = 0;

  /* set GSTK_CMD_Q_SIG and put on GSTK queue */
  gstk_task_put_cmd_priority_buf(task_cmd);

  return GSTK_SUCCESS;
}/* gstk_build_error_envelope_report */

/*===========================================================================
FUNCTION gstk_start_env_retry_timer

DESCRIPTION

  This function starts retry timer for an envelope.
  This is called when card responded with '93xx'

PARAMETERS
  type_of_command: [Input] expected_cmd_rsp_type of envelope

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
gstk_status_enum_type gstk_start_env_retry_timer(
  gstk_cmd_enum_type type_of_command)
{
  gstk_nv_items_data_type          nv_data;

  memset(&nv_data, 0, sizeof(nv_data));

  (void)gstk_nv_access_read(GSTK_NV_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  /* Change state to pending retry */
  gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_IN_QUEUE;
  gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.cmd_type = type_of_command;

  if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_counter <
     nv_data.env_retry_ext.max_retries)
  {
    /* start the timer now */
    (void)rex_set_timer(&(gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_retry_timer),
                        nv_data.env_retry_ext.duration);
    return GSTK_SUCCESS;
  }
  else if((gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_counter == nv_data.env_retry_ext.max_retries)&&
          (gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress))
  {
    /* Will retry at the end of proactive cmd chain. Not starting any timer this time */
    return GSTK_SUCCESS;
  }

  UIM_MSG_HIGH_0("Reached Max number of retries for envelope");
  return GSTK_ERROR;

}

/*===========================================================================
FUNCTION gstk_menu_selection_rsp

DESCRIPTION

  This function builds the menu selection rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_menu_selection_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   if(cmd == NULL)
   {
     UIM_MSG_ERR_0("cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }
   UIM_MSG_HIGH_0(" IN GSTK_MENU_SELECTION_RSP ");

   /* build the command */
   response.hdr_cmd.command_id = GSTK_MENU_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   response.cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;
   response.hdr_cmd.sim_slot_id = cmd->envelope_response.message_header.sim_slot_id;
   if(cmd->envelope_response.message_header.user_data <= GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cmd->envelope_response.message_header.user_data);

   return gstk_status;

} /* gstk_menu_selection_rsp */

/*===========================================================================
FUNCTION gstk_loc_info_envelope_rsp

DESCRIPTION

  This function builds the location information rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_loc_info_envelope_rsp(
  const gstk_cmd_type * cmd)
{
  uint32                          env_ref_id = GSTK_MAX_ENVELOPE_COMMAND;
  gstk_cmd_from_card_type         response;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(cmd == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  memset(&response, 0x00, sizeof(gstk_cmd_from_card_type));
  UIM_MSG_HIGH_2("gstk_loc_info_envelope_rsp(): 0x%x 0x%x",
                 gstk_curr_inst_ptr->is_first_loc_info_envelope_sent,
                 gstk_curr_inst_ptr->is_tp_dl_done);

  gstk_curr_inst_ptr->is_first_loc_info_envelope_sent = TRUE;
  if (!gstk_curr_inst_ptr->is_tp_dl_done)
  {
    UIM_MSG_HIGH_0("Notify GSDI TP DL complete");
    /* Unblock MMGSDI */
    response.hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
    response.hdr_cmd.sim_slot_id          = cmd->general_cmd.message_header.sim_slot_id;
    response.hdr_cmd.cmd_detail_reference = 0;
    response.hdr_cmd.command_number       = 0;
    response.hdr_cmd.user_data            = gstk_curr_inst_ptr->tp_info.user_data;
    if(gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb != NULL)
    {
      (gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb)(&response);
    }
    (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
    gstk_curr_inst_ptr->is_tp_dl_done = TRUE;
  }

  env_ref_id = cmd->envelope_response.message_header.user_data;

  if(env_ref_id >= GSTK_MAX_ENVELOPE_COMMAND)
  {
    UIM_MSG_ERR_1("Recd bad env ref id, 0x%x", env_ref_id);
    return GSTK_ERROR;
  }

  /* if retry envelope is successful , clear the global pending envelope structure */
  if((env_ref_id == gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id) &&
     (gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD))
  {
    /* Check envelope response status words received from the card */
    if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_SUCCESS)
    {
      gstk_util_clear_retry_envelope_data(
        gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
      if(cmd->envelope_response.sw1 == SW1_NORMAL_END)
      {
        UIM_MSG_HIGH_0("Calling mmgsdi_gstk_loc_env_rsp_received");
        /* call MMGSDI API to read IMSI */
        mmgsdi_gstk_loc_env_rsp_received(
           (mmgsdi_slot_id_enum_type)cmd->general_cmd.message_header.sim_slot_id, /* slot_id */
           (mmgsdi_location_status_enum_type)0, /* loc_status */
           0, /* mcc */
           0, /* mnc */
           gstk_mmgsdi_cmd_cb, /* response_cb_ptr */
           0);
      }
    }
    else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY)
    {
      if(gstk_start_env_retry_timer(GSTK_LOC_INFO_ENVELOPE_RSP) != GSTK_SUCCESS)
      {
        gstk_util_clear_retry_envelope_data(
          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
        memset(gstk_curr_inst_ptr->gstk_last_loc_info_sent, 0xFF, 3);
      }
      else
      {
        return GSTK_SUCCESS;
      }
    }/* else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY) */
    else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL)
    {
      UIM_MSG_HIGH_0("Envelope command response FAIL");
      gstk_util_clear_retry_envelope_data(
        gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
    } /* else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL) */
  }
  gstk_util_env_cmd_ref_table_free_slot(env_ref_id);

  if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL)
  {
    gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED;

    if (gstk_curr_inst_ptr->attach_ctrl_p != NULL
        &&
        memcmp(gstk_curr_inst_ptr->gstk_last_loc_info_sent,
               gstk_curr_inst_ptr->attach_ctrl_p->mcc_mnc,
               sizeof(gstk_curr_inst_ptr->attach_ctrl_p->mcc_mnc)) == 0)
    {
      gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_ALLOWED;
      gstk_util_send_attach_ctrl_res();
    }
  }

  /* set the variable to TRUE to inform MMGSDI to read the IMSI at the end of
     proactive session */
  if(cmd->envelope_response.sw1 == SW1_END_FETCH)
  {
    gstk_curr_inst_ptr->read_imsi = TRUE;
  }

  return GSTK_SUCCESS;
} /* gstk_loc_info_envelope_rsp */


/*===========================================================================
FUNCTION gstk_raw_envelope_rsp

DESCRIPTION

  This function builds the raw envelope response and sends it to client

PARAMETERS
  cmd_ptr: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_raw_envelope_rsp(
  const gstk_cmd_type * cmd_ptr)
{
  gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
  gstk_cmd_from_card_type                     response;
  gstk_evt_list_code_enum_type                evt_list    = 0xFF;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);
  /* the envelope response is enum, so there is no need to initialize
     response */

  UIM_MSG_HIGH_0("IN GSTK_RAW_ENVELOPE_RSP()");

  /* check for Null ptr */
  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr ERR:NULL");
    return GSTK_NULL_INPUT_PARAM;
  }

  if (cmd_ptr->envelope_response.length > GSTK_MAX_DATA_SIZE)
  {
    UIM_MSG_ERR_2("cmd_ptr->envelope_response.length (%d) is too long (> %d)",
                  cmd_ptr->envelope_response.length, GSTK_MAX_DATA_SIZE);
    return GSTK_BAD_PARAM;
  }

  /* build the command */
  response.hdr_cmd.command_id = GSTK_RAW_ENVELOPE_RSP;
  response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
  response.cmd.raw_envelope_rsp.general_resp =
    cmd_ptr->envelope_response.rsp_status;

  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2, GSTK_SLOT_ID_MAX) == TRUE)
  {
    response.cmd.raw_envelope_rsp.sw1 = cmd_ptr->envelope_response.sw1;
    response.cmd.raw_envelope_rsp.sw2 = cmd_ptr->envelope_response.sw2;
    if (cmd_ptr->envelope_response.sw1 == SW1_END_FETCH)
    {
      response.cmd.raw_envelope_rsp.sw1 = SW1_NORMAL_END;
      response.cmd.raw_envelope_rsp.sw2 = SW2_NORMAL_END;
    }
  }

  if(cmd_ptr->envelope_response.message_header.user_data <=
     GSTK_MAX_PROACTIVE_COMMAND)
  {
    response.hdr_cmd.user_data =
      gstk_shared_data.envelope_cmd_ref_table[cmd_ptr->envelope_response.message_header.user_data].user_data;
    evt_list = gstk_shared_data.envelope_cmd_ref_table[cmd_ptr->envelope_response.message_header.user_data].evt_list_code;
    gstk_shared_data.envelope_cmd_ref_table[cmd_ptr->envelope_response.message_header.user_data].evt_list_code = 0xFF;
  }

  UIM_MSG_HIGH_1("envelope response payload is present. len: 0x%x",
                 cmd_ptr->envelope_response.length);

  if(cmd_ptr->envelope_response.length > 0)
  {
    /* response data present */
    response.cmd.raw_envelope_rsp.length =
      cmd_ptr->envelope_response.length;
    response.cmd.raw_envelope_rsp.rsp_data_ptr =
      GSTK_CALLOC(cmd_ptr->envelope_response.length);
    if(response.cmd.raw_envelope_rsp.rsp_data_ptr != NULL)
    {
      (void)gstk_memcpy(response.cmd.raw_envelope_rsp.rsp_data_ptr,
                     cmd_ptr->envelope_response.data,
                     cmd_ptr->envelope_response.length,
                     cmd_ptr->envelope_response.length,
                     cmd_ptr->envelope_response.length);
    }
    else
    {
      return GSTK_MEMORY_ERROR;
    }
  }
  else
  {
    response.cmd.raw_envelope_rsp.length = 0;
    response.cmd.raw_envelope_rsp.rsp_data_ptr = NULL;
  }

  gstk_status = gstk_util_send_response_to_clients(&response,
    cmd_ptr->envelope_response.message_header.user_data);

  switch(evt_list)
  {
    case GSTK_USER_ACTIVITY:
      gstk_curr_inst_ptr->gstk_evt_list[GSTK_USER_ACTIVITY]= 0x00;
      break;
    case GSTK_IDLE_SCRN_AVAILABLE:
      gstk_curr_inst_ptr->gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE]= 0x00;
      break;
    default:
      break;
  }

  return gstk_status;

} /* gstk_raw_envelope_rsp */

/*===========================================================================
FUNCTION gstk_evt_dl_rsp

DESCRIPTION

  This function builds the event download rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_evt_dl_rsp(
  const gstk_cmd_type * cmd)
{
    gstk_status_enum_type                       gstk_status     = GSTK_SUCCESS;
    gstk_cmd_from_card_type                     *response_ptr   = NULL;
    gstk_cmd_enum_type                          type_of_command = GSTK_CMD_ENUM_NOT_USE;
    uint32                                      env_ref_id      = GSTK_MAX_ENVELOPE_COMMAND;
    gstk_slot_id_enum_type                      slot;
    gstk_nv_items_data_type                     nv_data;

    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

    if(cmd == NULL)
    {
      return GSTK_BAD_PARAM;
    }

    response_ptr = (gstk_cmd_from_card_type *)GSTK_CALLOC(sizeof(gstk_cmd_from_card_type));
    if(response_ptr == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }

    /* the envelope response is enum, so there is no need to initialize
       response */

    UIM_MSG_HIGH_1(" IN GSTK_EVT_DL_RSP, RSP_STATUS is 0x%x ",
                   cmd->envelope_response.rsp_status);

    slot = cmd->general_cmd.message_header.sim_slot_id;

    /* Read the CAT version */
    (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                              slot,
                              &nv_data);

    env_ref_id = cmd->envelope_response.message_header.user_data;
    if(env_ref_id >= GSTK_MAX_ENVELOPE_COMMAND) {
      UIM_MSG_ERR_1("Recd bad env ref id, 0x%x", env_ref_id);
      gstk_status = GSTK_ERROR;
      gstk_free(response_ptr);
      response_ptr = NULL;
      return gstk_status;
    }
    response_ptr->hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data;

    response_ptr->hdr_cmd.sim_slot_id = slot;

    /* Get the type of response */
    type_of_command = gstk_shared_data.envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type;

    /* if retry envelope is successful , clear the global pending envelope structure */
    if((env_ref_id == gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id) &&
       (gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD))
    {
      /* Check envelope response status words received from the card */
      if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_SUCCESS)
        gstk_util_clear_retry_envelope_data(
          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
      else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY)
      {
        if(GSTK_SUCCESS != gstk_start_env_retry_timer(type_of_command))
        {
          gstk_util_clear_retry_envelope_data(
            gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
        }
        else
        {
          gstk_free(response_ptr);
          response_ptr = NULL;
          return GSTK_SUCCESS;
        }
      }/* else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY) */
      else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL)
      {
        gstk_util_clear_retry_envelope_data(
          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
      } /* else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL) */
    }

    UIM_MSG_HIGH_2("env_ref_id is 0x%x and type_of_command is 0x%x",
                   env_ref_id, type_of_command);

    switch( type_of_command )
    {
      case GSTK_CALL_CONNECTED_EVT_IND_RSP:
      case GSTK_MT_CALL_EVT_IND_RSP:
      case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
      case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
      case GSTK_DATA_AVAIL_EVT_IND_RSP:
      case GSTK_NW_REJ_EVT_IND_RSP:
      case GSTK_CH_STATUS_EVT_IND_RSP:
      case GSTK_LANG_SELECTION_EVT_IND_RSP:
      case GSTK_ACCESS_TECH_EVT_IND_RSP:
      case GSTK_IMS_REG_EVT_IND_RSP:
        gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
        break;

      case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
        if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
        {
          gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
        }
        else
        {
          UIM_MSG_HIGH_1("GSTK_NW_SEARCH_MODE_EVT_IND_RSP: CAT version is pre-VER6 - 0x%x",
                         nv_data.cat_version);
        }

        if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_SUCCESS)
        {
          gstk_curr_inst_ptr->is_nw_search_mode_sent = TRUE;
        }
        break;
      case GSTK_LOCATION_STATUS_EVT_IND_RSP:
        gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
        gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = FALSE;
        if((gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01)
        {
          /* When CDMA is active RAT, dont compare RRC cache as during Multiple
             Active RAT scenarios, when the 3GPP RAT goes Out of Service
             this leads to duplicate Location Status */
          if (gstk_util_compare_curr_and_next_location_info()
              && (gstk_curr_inst_ptr->gstk_curr_rat != GSTK_ACCESS_TECH_CDMA &&
                  gstk_curr_inst_ptr->gstk_curr_rat != GSTK_ACCESS_TECH_HRPD &&
                  gstk_curr_inst_ptr->gstk_curr_rat != GSTK_ACCESS_TECH_EHRPD))
          {
            UIM_MSG_HIGH_0("There are Pending Loc Status");

            /*
              If GSTK_CFG_FEATURE_SLOT_LTD_SVC_WITH_NW_REJ_ONLY is turned on, limited service should only
              be sent if network reject is received. However, it is possible that GSTK first gets a
              normal service MM Idle, and before GSTK gets the envelope rsp another limited service
              MM idle is sent.

              1) If GSTK_CFG_FEATURE_PROP_LOC_STATUS is enabled then location status would've
                 been sent to card when gstk is handling MM Idle; in this case do not send LS
                 to card.

              2) If GSTK_CFG_FEATURE_PROP_LOC_STATUS is disabled then we need to check if
                 NW reject cache matches latest location info; if it does we send LS to card.
            */
            if ((gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
                                            gstk_curr_inst_ptr->slot_id) == FALSE) &&
                (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_LTD_SVC_WITH_NW_REJ_ONLY,
                                            gstk_curr_inst_ptr->slot_id) == TRUE) &&
                (gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_LIMITED_SERVICE) &&
                !gstk_util_compare_mm_to_nw_rej_cache())
            {
              UIM_MSG_HIGH_0("NW reject cache does not match latest location info - dont send LS");
              gstk_status = GSTK_SUCCESS;
              break;
            }
            /* Cleanup NW reject cache to avoid using out of date NW reject info */
            gstk_curr_inst_ptr->nw_rej.access_tech = GSTK_ACCESS_NONE;
            memset(gstk_curr_inst_ptr->nw_rej.mcc_mnc, 0xFF, GSTK_MAX_PLMN_LEN);
            memset(gstk_curr_inst_ptr->nw_rej.lac, 0xFF, GSTK_MAX_LAC_LEN);

            gstk_curr_inst_ptr->gstk_curr_location_status = gstk_curr_inst_ptr->gstk_next_curr_location_status;
            gstk_curr_inst_ptr->gstk_curr_rat = gstk_curr_inst_ptr->gstk_next_curr_rat;
            (void)memscpy(gstk_curr_inst_ptr->gstk_curr_mcc_mnc,
                     sizeof(gstk_curr_inst_ptr->gstk_curr_mcc_mnc),
                     gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc, 3);
            (void)memscpy(gstk_curr_inst_ptr->gstk_curr_lac,
                     sizeof(gstk_curr_inst_ptr->gstk_curr_lac),
                     gstk_curr_inst_ptr->gstk_next_curr_lac, 2);
            (void)memscpy(&gstk_curr_inst_ptr->gstk_curr_cell_id,
                     sizeof(gstk_curr_inst_ptr->gstk_curr_cell_id),
                     &gstk_curr_inst_ptr->gstk_next_curr_cell_id, sizeof(gstk_cell_id_type));

            switch (gstk_curr_inst_ptr->gstk_curr_rat)
            {
              case GSTK_ACCESS_TECH_LTE:
                gstk_curr_inst_ptr->is_rrc_cell_id_received[GSTK_RAT_LTE_IDX] = FALSE;
                break;
              case GSTK_ACCESS_TECH_UTRAN:
                gstk_curr_inst_ptr->is_rrc_cell_id_received[GSTK_RAT_UMTS_IDX] = FALSE;
                break;
              case GSTK_ACCESS_TECH_GSM:
              case GSTK_ACCESS_TECH_CDMA:
              case GSTK_ACCESS_TECH_HRPD:
              case GSTK_ACCESS_TECH_EHRPD:
              case GSTK_ACCESS_NONE:
              default:
                break;
            }

              gstk_status = gstk_build_location_status_envelope(
                              gstk_curr_inst_ptr->gstk_curr_rat,
                              gstk_curr_inst_ptr->gstk_curr_location_status);

            if (gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Have problem sending Loc status evp");
            }
            else
            {
              gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = TRUE;
            }
          }
        }
        break; /* ignore, doesn't have to sent back! */

        /* Send back to user */
      case GSTK_IDLE_SCRN_EVT_IND_RSP:
        /* build the command */
        response_ptr->hdr_cmd.command_id = type_of_command;
        response_ptr->hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
        response_ptr->cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;

        gstk_status = gstk_util_send_response_to_clients(response_ptr,
         cmd->envelope_response.message_header.user_data);

        /* remove the bitmask */
        gstk_curr_inst_ptr->gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE]= 0x00;

        /* notify client to remove their mask */
        response_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY;
        response_ptr->cmd.evt_notify_req.require_notification = FALSE;
        gstk_status = gstk_util_send_message_to_clients(response_ptr);
        break;

      case GSTK_USER_ACTIVITY_EVT_IND_RSP:
        /* build the command */
        response_ptr->hdr_cmd.command_id = type_of_command;
        response_ptr->hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
        response_ptr->cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;

        gstk_status = gstk_util_send_response_to_clients(response_ptr,
         cmd->envelope_response.message_header.user_data);

        /* remove the bitmask */
        gstk_curr_inst_ptr->gstk_evt_list[GSTK_USER_ACTIVITY]= 0x00;

        /* notify client to remove their mask */
        response_ptr->hdr_cmd.command_id = GSTK_SETUP_EVT_USER_ACT_NOTIFY;
        response_ptr->cmd.evt_notify_req.require_notification = FALSE;
        gstk_status = gstk_util_send_message_to_clients(response_ptr);
        break;
      case GSTK_CSG_CELL_SELECTION_EVT_IND_RSP:
        if(nv_data.cat_version >= GSTK_CFG_CAT_VER9)
        {
          gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
        }
        else
        {
          UIM_MSG_HIGH_1("GSTK_CSG_CELL_SELECTION_EVT_IND_RSP: CAT version is pre-VER6 - 0x%x",
                         nv_data.cat_version);
        }

        if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_SUCCESS)
        {
          gstk_curr_inst_ptr->is_csg_info_sent = TRUE;
        }
        break;

      default:
        /* do nothing */
        UIM_MSG_ERR_1("In Evt DL: unknown type of event 0x%x",
                      type_of_command);
        break;
    }
    gstk_free(response_ptr);
    response_ptr = NULL;
    return gstk_status;

} /* gstk_evt_dl_rsp */

/*===========================================================================
FUNCTION gstk_sms_pp_dl_rsp

DESCRIPTION

  This function builds the sms pp download rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_sms_pp_dl_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status     = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     *response_ptr   = NULL;
   uint32                                      env_ref_id      = GSTK_MAX_ENVELOPE_COMMAND;
   gstk_cmd_enum_type                          type_of_command = GSTK_CMD_ENUM_NOT_USE;

   UIM_MSG_HIGH_0(" IN GSTK_SMS_PP_DL_RSP ");

   /* check for Null ptr */
   if(cmd == NULL)
   {
     return GSTK_BAD_PARAM;
   }

    response_ptr = (gstk_cmd_from_card_type *)GSTK_CALLOC(sizeof(gstk_cmd_from_card_type));
    if(response_ptr == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }

   /* build the command */
   response_ptr->hdr_cmd.command_id = GSTK_SMS_PP_DOWNLOAD_IND_RSP;
   response_ptr->hdr_cmd.cmd_detail_reference = 0; /* doens't matter */

   env_ref_id = cmd->envelope_response.message_header.user_data;

   if(env_ref_id >= GSTK_MAX_ENVELOPE_COMMAND) {
     UIM_MSG_ERR_1("Recd bad env ref id, 0x%x", env_ref_id);
     gstk_status = GSTK_ERROR;
     gstk_free(response_ptr);
     response_ptr = NULL;
     return gstk_status;
   }
   response_ptr->hdr_cmd.user_data =
      gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data;

   response_ptr->cmd.sms_pp_download_rsp.general_resp = cmd->envelope_response.rsp_status;

   /* Get the type of response */
   type_of_command = gstk_shared_data.envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type;

   /* if retry envelope is successful , clear the global pending envelope structure */
   if((env_ref_id == gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id) &&
      (gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD))
   {
     /* Check envelope response status words received from the card */
     if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_SUCCESS)
     {
       gstk_util_clear_retry_envelope_data(
         gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
     }
     else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY)
     {
       if(GSTK_SUCCESS != gstk_start_env_retry_timer(type_of_command))
       {
         gstk_util_clear_retry_envelope_data(
           gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
       }
       else
       {
         gstk_free(response_ptr);
         response_ptr = NULL;
         return GSTK_SUCCESS;
       }
     }/* else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY) */
     else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL)
     {
       gstk_util_clear_retry_envelope_data(
         gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
     } /* else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL) */
   }

   if(cmd->envelope_response.length > 0) { /* reponse data present */
     UIM_MSG_HIGH_1("In SMS PP: There are extra data len: 0x%x",
                    cmd->envelope_response.length);
     response_ptr->cmd.sms_pp_download_rsp.response_data_present = TRUE;
     if (cmd->envelope_response.length < GSTK_MAX_DATA_SIZE)
     {
       response_ptr->cmd.sms_pp_download_rsp.ack.ack =
         GSTK_CALLOC(cmd->envelope_response.length);
       if(response_ptr->cmd.sms_pp_download_rsp.ack.ack != NULL) {
         response_ptr->cmd.sms_pp_download_rsp.ack.length = (uint8)cmd->envelope_response.length;
         (void)memscpy(response_ptr->cmd.sms_pp_download_rsp.ack.ack,
           cmd->envelope_response.length,
           cmd->envelope_response.data,
           cmd->envelope_response.length);
       }
       else {
         gstk_free(response_ptr);
         response_ptr = NULL;
         return GSTK_MEMORY_ERROR;
       }
     }
     else
     {
       UIM_MSG_ERR_2("cmd->envelope_response.length (0x%x) is too long (>= 0x%x)!",
                     cmd->envelope_response.length, GSTK_MAX_DATA_SIZE);
       gstk_free(response_ptr);
       response_ptr = NULL;
       return GSTK_BAD_PARAM;
     }
   }

   response_ptr->cmd.sms_pp_download_rsp.sw1 = cmd->envelope_response.sw1;
   response_ptr->cmd.sms_pp_download_rsp.sw2 = cmd->envelope_response.sw2;

   gstk_status = gstk_util_send_response_to_clients(response_ptr,
     cmd->envelope_response.message_header.user_data);

   gstk_free(response_ptr);
   response_ptr = NULL;

   return gstk_status;

} /* gstk_sms_pp_dl_rsp */

/*===========================================================================
FUNCTION gstk_check_valid_tag_for_cc_rsp

DESCRIPTION

  This function if the tag [input] is a valid tag for call control envelope
  response

PARAMETERS
  sim_tlv_tag    : [Input] tag
  curr_tag_level : [input] current postion of the tag in the list of tlvs

DEPENDENCIES
  None

RETURN VALUE
  int32 - postion that the tag should have in the list of tlvs

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int32 gstk_check_valid_tag_for_cc_rsp(
 uint8              sim_tlv_tag,
 int32              curr_tag_level)
{
    if (sim_tlv_tag > 0x80)
    { /* tag with CR */
      sim_tlv_tag -= 0x80;
    }

    switch(sim_tlv_tag)
    {
    case GSTK_ADDRESS_TAG:
    case GSTK_SS_STRING_TAG:
    case GSTK_USSD_STRING_TAG:
    case GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG:
    case GSTK_EPS_PDN_CONN_PARAM_TAG:
    case GSTK_IMS_URI_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_1)
        return GSTK_TAG_LEVEL_1;
      break;
    case GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_2)
        return GSTK_TAG_LEVEL_2;
      else if(curr_tag_level <  GSTK_TAG_LEVEL_6)
        return GSTK_TAG_LEVEL_6;
      break;
    case GSTK_CALLED_PARTY_SUB_ADDRESS_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_3)
        return GSTK_TAG_LEVEL_3;
      break;
    case GSTK_ALPHA_IDENTIFIER_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_4)
        return GSTK_TAG_LEVEL_4;
      break;
    case GSTK_BC_REPEAT_INDICATOR_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_5)
        return GSTK_TAG_LEVEL_5;
      break;
    default:
      break;
    }
    return 0;
}

/*===========================================================================
FUNCTION gstk_is_tag_valid_for_command

DESCRIPTION

  This function checks the validity of the tag for the command specified

PARAMETERS
  cmd_type:          [Input] command type
  sim_tlv_tag:       [Input] tag
  tlv_tag_level_ptr: [Input] ptr to the tag level
  curr_tag_level:    [Input] current tag postion in the tlv

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE  - if tag is valid
    FALSE - if tag is not valid

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_tag_valid_for_command(
 gstk_cmd_enum_type cmd_type,
 uint8              sim_tlv_tag,
 int32              *tlv_tag_level_ptr,
 int32              curr_tag_level)
{

  if(tlv_tag_level_ptr == NULL)
  {
    UIM_MSG_HIGH_0("tlv_tag_level_ptr NULL");
    return FALSE;
  }

  switch(cmd_type)
  {
  case GSTK_CC_IND_RSP:
    *tlv_tag_level_ptr = gstk_check_valid_tag_for_cc_rsp(sim_tlv_tag, curr_tag_level);
    if(*tlv_tag_level_ptr != 0)
    {
      return TRUE;
    }
    break;
  default:
    break;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION gstk_cc_rsp

DESCRIPTION

  This function builds the call control rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_cc_rsp(
  gstk_cmd_type * cmd)
{
  gstk_status_enum_type   gstk_status          = GSTK_SUCCESS;
  gstk_cmd_from_card_type response;
  uint32                  curr_offset          = 0;
  boolean                 more_tlvs            = TRUE;
  uint32                  new_offset           = 0;
  boolean                 error_already_raised = FALSE;
  int32                   tlv_tag_level        = 0;
  int32                   curr_tag_level       = 0;
  boolean                 parsing_done         = FALSE;
  boolean                 comprehension_flag   = FALSE;
  uint32                  cc_rsp_len           = 0;
  uint32                  cc_rsp_data_len      = 0;
  uint32                  tlv_len              = 0;
  uint32                  len_of_tlv_len       = 0;
  uint32                  env_ref_id           = GSTK_MAX_ENVELOPE_COMMAND;

  if(cmd == NULL)
  {
    UIM_MSG_ERR_0("cmd ERR:NULL");
    return GSTK_BAD_PARAM;
  }
  UIM_MSG_HIGH_2(" IN GSTK_CC_RSP, RSP_STATUS 0x%x, call_ctrl_result is 0x%x ",
                  cmd->envelope_response.rsp_status,
                 cmd->envelope_response.data[0]);

  /* initialize response */
  memset(&response, 0, sizeof(gstk_cc_rsp_type)+sizeof(gstk_exp_hdr_type));
  env_ref_id = cmd->envelope_response.message_header.user_data;
  /* build the command */
  response.hdr_cmd.command_id           = GSTK_CC_IND_RSP;
  response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
  if(env_ref_id < GSTK_MAX_ENVELOPE_COMMAND)
  {
    response.hdr_cmd.user_data =
      gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data;
  }
  response.cmd.cc_rsp.general_resp = cmd->envelope_response.rsp_status;

  /* if retry envelope is successful , clear the global pending envelope structure */
  if((env_ref_id == gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id) &&
     (gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD))
  { /* Check envelope response status words received from the card */
    if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_SUCCESS)
      gstk_util_clear_retry_envelope_data(
        gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
    else if(response.cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_CARD_BUSY)
    {
      if(GSTK_SUCCESS != gstk_start_env_retry_timer(GSTK_CC_IND_RSP))
      {
        gstk_util_clear_retry_envelope_data(
          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
      }
      else
      {
        return GSTK_SUCCESS;
      }
    }/* else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY) */
    else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL)
    {
      UIM_MSG_HIGH_0("Envelope command response FAIL");
      gstk_util_clear_retry_envelope_data(
        gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
    } /* else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL) */
  }

  /* Check for sw 1 = 6F, sw 2 = 00 || C2 */
  if ((cmd->envelope_response.sw1 == 0x6F) &&
      (cmd->envelope_response.sw2 == 0x00 || cmd->envelope_response.sw2 == 0xC2)) {
    /* treat as success */
    UIM_MSG_HIGH_1("Recv 0x6F%x for CC, changed to SUCCESS, No MOD",
                   cmd->envelope_response.sw2);
    response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_SUCCESS;
    response.cmd.cc_rsp.call_ctrl_result = GSTK_ALLOWED_NO_MOD;
  }
  else {
    /* SW != 6F00, proceed with normal parsing */
    if (response.cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS) {
      if(cmd->envelope_response.length > 0) { /* response data present */
        response.cmd.cc_rsp.call_ctrl_result = (gstk_control_result_enum_type)cmd->envelope_response.data[0];
        switch(response.cmd.cc_rsp.call_ctrl_result) {
        case GSTK_ALLOWED_NO_MOD:
          break;
        case GSTK_NOT_ALLOWED:
          break;
        case GSTK_ALLOWED_BUT_MOD:
          if(cmd->envelope_response.data[1] == 0) { /* error */
            UIM_MSG_ERR_0("In CC: Allowed but Mod with no more data ");
            response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
          }
          break;
        case GSTK_CONTROL_RESULT_MAX:
          break;
        }/*  switch(response.cmd.cc_rsp.call_ctrl_result) */

        if(response.cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS) {
          response.cmd.cc_rsp.cc_req_action.call_type = GSTK_NONE;
          response.cmd.cc_rsp.cc_req_action.alpha.length = 0;
          /* success and length and has more data */
          if(cmd->envelope_response.data[1] > 0) {

            /* Current offset starts at 2, as call control result is byte 0 and
               length is byte 1 or 2  */
            curr_offset = gstk_find_length_of_length_value(
                            &cmd->envelope_response.data[1]) + 1;
            if ((curr_offset == 1) || ((curr_offset + 1) >= GSTK_MAX_DATA_SIZE))
            {
              UIM_MSG_ERR_2("Invalid length field 0x%x 0x%x",
                            cmd->envelope_response.data[1], curr_offset);
              error_already_raised = TRUE;
            }
            else
            {
              cc_rsp_len = cmd->envelope_response.data[curr_offset - 1];
              cc_rsp_data_len = 1 + ((cc_rsp_len > 127) ? 2 : 1) + cc_rsp_len;
            }

            while(!parsing_done && !error_already_raised)
            {
              if ((curr_offset + 1) >= GSTK_MAX_DATA_SIZE)
              {
                UIM_MSG_ERR_1("Invalid length field 0x%x 0x%x", curr_offset);
                error_already_raised = TRUE;
                break;
              }

              len_of_tlv_len = gstk_find_length_of_length_value(
                                 &cmd->envelope_response.data[curr_offset + 1]);
              if ((curr_offset + len_of_tlv_len) < GSTK_MAX_DATA_SIZE)
              {
                tlv_len = cmd->envelope_response.data[curr_offset + len_of_tlv_len];
              }

              if (curr_offset >= GSTK_MAX_DATA_SIZE)
              {
                UIM_MSG_ERR_1("Invalid length field 0x%x", curr_offset);
                error_already_raised = TRUE;
                break;
              }

              /* Check for tag validity */
              if((len_of_tlv_len == 0)
                 ||
                 (FALSE == gstk_is_tag_valid_for_command(
                             GSTK_CC_IND_RSP,
                             cmd->envelope_response.data[curr_offset],
                             &tlv_tag_level,
                             curr_tag_level)))
              {
                UIM_MSG_ERR_3("Unexpected tag 0x%x at 0x%x or invalid length 0x%x",
                              cmd->envelope_response.data[curr_offset],
                              curr_offset,
                              len_of_tlv_len);

                /* check for tag comprehension required flag */
                comprehension_flag = gstk_is_tag_comprehension_req(cmd->envelope_response.data[curr_offset]);
                if(comprehension_flag)
                {
                  response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
                  break; /* break out of the while loop */
                }

                if (len_of_tlv_len == 0)
                {
                  UIM_MSG_ERR_3("Invalid length field 0x%x 0x%x!",
                                cmd->envelope_response.data[curr_offset+1],
                                curr_offset,
                                len_of_tlv_len);
                  curr_offset += 1;
                }
                else if ((curr_offset + len_of_tlv_len) >= GSTK_MAX_DATA_SIZE)
                {
                  UIM_MSG_ERR_2("Invalid length value 0x%x 0x%x",
                                curr_offset, len_of_tlv_len);
                  error_already_raised = TRUE;
                  break;
                }
                else
                {
                  tlv_len = cmd->envelope_response.data[curr_offset + len_of_tlv_len];
                  curr_offset += len_of_tlv_len + tlv_len;
                }

                /* ignore the current tlv and get the next tlv */
                gstk_status = gstk_parser_get_next_tlv_offset(
                                cc_rsp_data_len,
                                curr_offset,
                                (int32 *)&new_offset,
                                &more_tlvs);

                if(gstk_status != GSTK_SUCCESS)
                {
                  UIM_MSG_ERR_2("Failed to skip invalid TLV 0x%x 0x%x!",
                                curr_offset, new_offset);
                  response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
                  break; /* break out of the while loop */
                }
                curr_offset = new_offset;
                continue; /* continue with the next iteration and check for new tag validity */

              } /* tag validity check */

              switch(tlv_tag_level)
              {
              case GSTK_TAG_LEVEL_1:
              {
                /* Parse Address String */
                new_offset = curr_offset;
                gstk_status = gstk_parser_address_ss_string_tlv(
                  GSTK_ADDRESS_TAG,
                  cmd->envelope_response.data,
                  cc_rsp_data_len, /* data length */
                  curr_offset,
                  &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address),
                  (int32 *)&new_offset,
                  &more_tlvs,
                  GSTK_OPTIONAL,
                  NULL);

                if(gstk_status != GSTK_SUCCESS)
                {
                  UIM_MSG_ERR_1("In CC: Parse Address Fail 0x%x", gstk_status);
                  error_already_raised = TRUE;
                }
                else
                {
                  if(new_offset == curr_offset)
                  {
                    /* Seems that the offset has not moved */
                    UIM_MSG_HIGH_0("NOT a GSTK_ADDRESS_STRING_TAG");
                  }
                  else
                  {
                    response.cmd.cc_rsp.cc_req_action.call_type = GSTK_VOICE;
                    curr_offset = new_offset;
                  }
                }

                /* Parse SS String */

                if(response.cmd.cc_rsp.cc_req_action.call_type != GSTK_VOICE && !error_already_raised)
                {
                  /* check to see if there is SS/USSD */
                  new_offset = curr_offset;
                  gstk_status = gstk_parser_address_ss_string_tlv(
                    GSTK_SS_STRING_TAG,
                    cmd->envelope_response.data,
                    cc_rsp_data_len, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.ss_string),
                    (int32 *)&new_offset,
                    &more_tlvs,
                    GSTK_OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    UIM_MSG_ERR_1("In CC: Parse SS String Fail 0x%x",
                                  gstk_status);
                      error_already_raised = TRUE;
                  }
                  else
                  {
                    if(new_offset == curr_offset)
                    {
                      /* Seems that the offset has not moved */
                      UIM_MSG_HIGH_0("NOT a GSTK_SS_STRING_TAG");
                    }
                    else
                    {
                      response.cmd.cc_rsp.cc_req_action.call_type = GSTK_SS;
                      curr_offset = new_offset;
                    }
                  }

                  /* Parse USSD String */
                  if(response.cmd.cc_rsp.cc_req_action.call_type != GSTK_SS && !error_already_raised ) {
                    /* check to see if there is a ussd */
                    new_offset = curr_offset;
                    gstk_status = gstk_parser_ussd_string_tlv(
                      cmd->envelope_response.data,
                      cc_rsp_data_len, /* data length */
                      curr_offset,
                      &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string),
                      (int32 *)&new_offset,
                      &more_tlvs,
                      GSTK_OPTIONAL,
                      NULL);

                    if(gstk_status != GSTK_SUCCESS) {
                      UIM_MSG_ERR_1("In CC: Parse USSD Fail 0x%x",
                                    gstk_status);
                      error_already_raised = TRUE;
                    }
                    else
                    {
                      if(new_offset == curr_offset)
                      {
                        /* Seems that the offset has not moved */
                        UIM_MSG_HIGH_0("NOT a GSTK_USSD_STRING_TAG");
                      }
                      else
                      {
                        response.cmd.cc_rsp.cc_req_action.call_type = GSTK_USSD;
                        curr_offset = new_offset;
                      }
                    }

                    /* Parse PDP Context Act Param  */
#ifdef FEATURE_GPRS_CALLCONTROL
                    if(response.cmd.cc_rsp.cc_req_action.call_type != GSTK_USSD &&
                       !error_already_raised)
                    {
                      /* check to see if there is PDP Context Param */
                      new_offset = curr_offset;
                      gstk_status = gstk_parser_pdp_context_act_param_tlv(
                        cmd->envelope_response.data,
                        cc_rsp_data_len, /* data length */
                        curr_offset,
                        &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.pdp_context_param),
                        (int32 *)&new_offset,
                        &more_tlvs,
                        GSTK_OPTIONAL,
                        NULL);

                      if(gstk_status != GSTK_SUCCESS)
                      {
                        UIM_MSG_ERR_1("In CC: Parse PDP CONTEXT ACT String Fail 0x%x",
                                      gstk_status);
                        error_already_raised = TRUE;
                      }
                      else
                      {
                        if(new_offset == curr_offset)
                        {
                          /* Seems that the offset has not moved */
                           UIM_MSG_HIGH_0("NOT a GSTK_PDP_CONTEXT_ACT_STRING_TAG");
                        }
                        else
                        {
                          response.cmd.cc_rsp.cc_req_action.call_type = GSTK_PDP_CONTEXT_ACT;
                          curr_offset = new_offset;
                        }
                      }

                      if(response.cmd.cc_rsp.cc_req_action.call_type != GSTK_PDP_CONTEXT_ACT &&
                         !error_already_raised)
                      {
                        /* check to see if there is PDN Context Param */
                        new_offset = curr_offset;
                        gstk_status = gstk_parser_pdn_conn_act_param_tlv(
                          cmd->envelope_response.data,
                          cc_rsp_data_len, /* data length */
                          curr_offset,
                          &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.eps_conn_act),
                          (int32 *)&new_offset,
                          &more_tlvs,
                          GSTK_OPTIONAL,
                          NULL);

                        if(gstk_status != GSTK_SUCCESS)
                        {
                          UIM_MSG_ERR_1("In CC: Parse PDN CONN ACT Fail 0x%x",
                                        gstk_status);
                          error_already_raised = TRUE;
                        }
                        else
                        {
                          if(new_offset == curr_offset)
                          {
                            /* Seems that the offset has not moved */
                            UIM_MSG_HIGH_0("NOT a GSTK_PDN_CONN_ACT_STRING_TAG");
                          }
                          else
                          {
                            response.cmd.cc_rsp.cc_req_action.call_type = GSTK_EPS_PDN_CONN_ACT;
                            curr_offset = new_offset;
                          }
                        }

                        if(response.cmd.cc_rsp.cc_req_action.call_type != GSTK_EPS_PDN_CONN_ACT &&
                           !error_already_raised)
                        {
                          /* check to see if there is IMS CALL Param */
                          new_offset = curr_offset;
                          gstk_status = gstk_parser_ims_call_param_tlv(
                            cmd->envelope_response.data,
                            cc_rsp_data_len, /* data length */
                            curr_offset,
                            &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.ims_call),
                            (int32 *)&new_offset,
                            &more_tlvs,
                            GSTK_OPTIONAL,
                            NULL);

                          if(gstk_status != GSTK_SUCCESS)
                          {
                            UIM_MSG_ERR_1("In CC: Parse IMS_CALL param Fail 0x%x", gstk_status);
                            error_already_raised = TRUE;
                          }
                          else
                          {
                            if(new_offset == curr_offset)
                            {
                              /* Seems that the offset has not moved */
                              UIM_MSG_HIGH_0("NOT a GSTK_IMS_REQUEST_URI_TAG");
                            }
                            else
                            {
                              response.cmd.cc_rsp.cc_req_action.call_type = GSTK_IMS_CALL;
                              curr_offset = new_offset;
                            }
                          }
                        }/* IMS communication control */
                      } /* EPS PDN Connection Activation */
                    } /*  PDP Context Activation */
#endif /* FEATURE_GPRS_CALLCONTROL */
                  } /* no SS string either, try ussd */
                } /* check if there is SS/USSD */
                /* If no call type is assigned that could mean address TLV might not be present
                   as it is optional. In such case let's default it to voice for clients as they
                   depend on this to read "response" correctly */
                if(response.cmd.cc_rsp.cc_req_action.call_type == GSTK_NONE)
                {
                  response.cmd.cc_rsp.cc_req_action.call_type = GSTK_VOICE;
                }
                break;
              }

              case GSTK_TAG_LEVEL_2:
              {
                if(more_tlvs
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_SS)
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_USSD)
#ifdef FEATURE_GPRS_CALLCONTROL
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_PDP_CONTEXT_ACT)
#endif /* FEATURE_GPRS_CALLCONTROL */
#ifdef FEATURE_CM_LTE
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_EPS_PDN_CONN_ACT)
#endif /* FEATURE_CM_LTE */
                  )
                {
                  /* initialize all other optional to zero length */
                  response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.ccp1.length = 0;
                  response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.ccp2.length = 0;
                  response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.subaddress.length = 0;
                  /* ccp1? */
                  gstk_status = gstk_parser_cap_config_tlv(
                    cmd->envelope_response.data,
                    cc_rsp_data_len, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.ccp1),
                    (int32 *)&new_offset,
                    &more_tlvs,
                    GSTK_OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    UIM_MSG_ERR_1("In CC: Parse CCP1 Fail %x", gstk_status);
                    error_already_raised = TRUE;
                  }
                  else
                  {
                    curr_offset = new_offset;
                  }
                }
                break;
              }

              case GSTK_TAG_LEVEL_3:
              {
                if(more_tlvs
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_SS)
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_USSD)
#ifdef FEATURE_GPRS_CALLCONTROL
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_PDP_CONTEXT_ACT)
#endif /* FEATURE_GPRS_CALLCONTROL */
#ifdef FEATURE_CM_LTE
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_EPS_PDN_CONN_ACT)
#endif /* FEATURE_CM_LTE */
                  )
                {/* more tlv after ccp1 parsing attempt */
                 /* subaddress? */
                  gstk_status = gstk_parser_subaddress_tlv(
                    cmd->envelope_response.data,
                    cc_rsp_data_len, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.subaddress),
                    (int32 *)&new_offset,
                    &more_tlvs,
                    GSTK_OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    UIM_MSG_ERR_1("In CC: Parse SubAddr Fail 0x%x",
                                  gstk_status);
                    error_already_raised = TRUE;
                  }
                  else {
                    curr_offset = new_offset;
                  }
                }
                break;
              }

              case GSTK_TAG_LEVEL_4:
              {
                if(!error_already_raised && more_tlvs) {/* more tlv after subaddress parsing attempt */
                  /* alpha? */
                  gstk_status = gstk_parser_alpha_identifier_tlv(
                    cmd->envelope_response.data,
                    cc_rsp_data_len, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.alpha),
                    (int32 *)&new_offset,
                    &more_tlvs,
                    GSTK_OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    UIM_MSG_ERR_1("In CC: Parse Alpha Fail 0x%x", gstk_status);
                    error_already_raised = TRUE;
                  }
                  else {
                    curr_offset = new_offset;
                  }
                }
                break;
              }

              case GSTK_TAG_LEVEL_5:
              {
                if(more_tlvs
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_SS)
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_USSD)
#ifdef FEATURE_GPRS_CALLCONTROL
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_PDP_CONTEXT_ACT)
#endif /* FEATURE_GPRS_CALLCONTROL */
#ifdef FEATURE_CM_LTE
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_EPS_PDN_CONN_ACT)
#endif /* FEATURE_CM_LTE */
                  )
                {/* more tlv after alpha parsing attempt */
                  /* bc repeat indicator */
                  gstk_status = gstk_parser_bc_repeat_ind_tlv(
                    cmd->envelope_response.data,
                    cc_rsp_data_len, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.bc_repeat_ind),
                    (int32 *)&new_offset,
                    &more_tlvs,
                    GSTK_OPTIONAL,
                    NULL);

                  if ((gstk_status != GSTK_SUCCESS) || (!more_tlvs)) {
                    /* bc repeat indicator always has ccp2 */
                    UIM_MSG_ERR_1("In CC: Parse BC Repeat Fail 0x%x",
                                  gstk_status);
                    error_already_raised = TRUE;
                  }
                  else {
                    curr_offset = new_offset;
                  }
                }
                break;
              }

              case GSTK_TAG_LEVEL_6:
              {
                if(more_tlvs
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_SS)
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_USSD)
#ifdef FEATURE_GPRS_CALLCONTROL
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_PDP_CONTEXT_ACT)
#endif /* FEATURE_NO_GPRS_CALLCONTROL */
#ifdef FEATURE_CM_LTE
                   && (response.cmd.cc_rsp.cc_req_action.call_type != GSTK_EPS_PDN_CONN_ACT)
#endif /* FEATURE_CM_LTE */
                  )
                { /* more tlv after bc repeat indicator parsing attempt */
                  /* ccp2 indicator */
                  gstk_status = gstk_parser_cap_config_tlv(
                    cmd->envelope_response.data,
                    cc_rsp_data_len, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.ccp2),
                    (int32 *)&new_offset,
                    &more_tlvs,
                    GSTK_OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    UIM_MSG_ERR_1("In CC: Parse CCP2 Fail 0x%x", gstk_status);
                    error_already_raised = TRUE;
                  }
                  else {
                    curr_offset = new_offset;
                  }
                } /* more tlv after bc repeat indicator parsing */
                break;
              }

              default:
                break;
              }/* switch(tlv_tag_level) */
              curr_tag_level = tlv_tag_level;
              if(!more_tlvs)
                parsing_done = TRUE;
             }/*  while(more_tlvs && !error_already_raised) */
          } /* more data cmd[1] > 0 */
        } /* envelope cmd success */
        if(error_already_raised) {
          response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
        }
      } /* cmd_data[0] > 0 */
      else { /* response length == 0 */
        UIM_MSG_HIGH_0("Resp Len = 0 in CC");
        response.cmd.cc_rsp.call_ctrl_result = GSTK_ALLOWED_NO_MOD;
      }
    } /* general resp = SUCCESS */
  }

  gstk_status = gstk_util_send_response_to_clients(&response,
    cmd->envelope_response.message_header.user_data);

  return gstk_status;

} /* gstk_cc_rsp */

/*===========================================================================
FUNCTION gstk_mo_sms_ctrl_rsp

DESCRIPTION

  This function builds the sms control download rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_mo_sms_ctrl_rsp(
  gstk_cmd_type * cmd)
{
  gstk_status_enum_type             gstk_status          = GSTK_SUCCESS;
  gstk_cmd_from_card_type           response;
  uint32                            offset               = 0;
  uint32                            new_offset           = 0;
  boolean                           more_tlv             = TRUE;
  boolean                           error_already_raised = FALSE;
  uint32                            cc_rsp_len           = 0;
  uint32                            cc_rsp_data_len      = 0;
  uint32                            env_ref_id           = GSTK_MAX_ENVELOPE_COMMAND;

  UIM_MSG_HIGH_0(" IN GSTK_MO_SMS_CTRL_RSP ");

  if(cmd == NULL)
  {
    UIM_MSG_ERR_0("cmd ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize response */
  memset(&response, 0, sizeof(gstk_mo_sms_ctrl_rsp_type)+sizeof(gstk_exp_hdr_type));

  /* build the command */
  response.hdr_cmd.command_id = GSTK_MO_SMS_CTRL_IND_RSP;
  response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
  env_ref_id = cmd->envelope_response.message_header.user_data;
  if(cmd->envelope_response.message_header.user_data < GSTK_MAX_ENVELOPE_COMMAND)
  {
    response.hdr_cmd.user_data =
      gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data;
  }
  response.cmd.mo_sms_cc_rsp.general_resp = cmd->envelope_response.rsp_status;
  /* if retry envelope is successful , clear the global pending envelope structure */
  if((env_ref_id == gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id) &&
     (gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD))
  { /* Check envelope response status words received from the card */
    if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_SUCCESS)
        gstk_util_clear_retry_envelope_data(
          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
    else if(response.cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_CARD_BUSY)
    {
      if(GSTK_SUCCESS != gstk_start_env_retry_timer(GSTK_MO_SMS_CTRL_IND_RSP))
      {
        gstk_util_clear_retry_envelope_data(
          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
      }
      else
      {
        return GSTK_SUCCESS;
      }
    }/* else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY) */
    else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL)
    {
      UIM_MSG_HIGH_0("Envelope command response FAIL");
      gstk_util_clear_retry_envelope_data(
        gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
    } /* else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL) */
  }

  /* Check for sw 1 = 6F, sw 2 = 00 */
  if ((cmd->envelope_response.sw1 == 0x6F) && (cmd->envelope_response.sw2 == 0x00))
  {
    /* treat as success */
    UIM_MSG_HIGH_0("Recv 0x6F00 for MO SMS Ctrl, changed to SUCCESS, No MOD");
    response.cmd.mo_sms_cc_rsp.general_resp = GSTK_ENVELOPE_CMD_SUCCESS;
    response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result = GSTK_ALLOWED_NO_MOD;
  }
  else
  {
    if (response.cmd.mo_sms_cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS)
    {
      if(cmd->envelope_response.length > 0)
      { /* response data present */
        response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result = (gstk_control_result_enum_type)cmd->envelope_response.data[0];
        if(cmd->envelope_response.data[1] > 0)
        {
          offset = 1 + gstk_find_length_of_length_value(&cmd->envelope_response.data[1]);
          if ((offset == 1) || ((offset + 1) >= GSTK_MAX_DATA_SIZE))
          {
            UIM_MSG_ERR_2("Invalid length field 0x%x 0x%x",
                          cmd->envelope_response.data[1], offset);
            error_already_raised = TRUE;
          }
          else
          {
            cc_rsp_len = cmd->envelope_response.data[offset - 1];
            cc_rsp_data_len = 1 + ((cc_rsp_len > 127) ? 2 : 1) + cc_rsp_len;
          }

          if(!error_already_raised
             &&
             gstk_valid_tag(cmd->envelope_response.data[offset], GSTK_ADDRESS_TAG))
          {
            gstk_status = gstk_parser_address_ss_string_tlv(
                      GSTK_ADDRESS_TAG,
                      cmd->envelope_response.data,
                      cc_rsp_data_len, /* data length */
                      offset,
                      &(response.cmd.mo_sms_cc_rsp.rp_dest_address),
                      (int32 *)&new_offset,
                      &more_tlv,
                      GSTK_OPTIONAL,
                      NULL);

            if(gstk_status != GSTK_SUCCESS || !more_tlv)
            {
              /* with rp address, there has to be tp address */
              UIM_MSG_ERR_1("In MO SMS Ctrl: Parse RP Addr Fail 0x%x",
                            gstk_status);
              error_already_raised = TRUE;
            }
            else
            {
              offset = new_offset;
            }

            if(!error_already_raised && more_tlv)
            {
              if(gstk_valid_tag(cmd->envelope_response.data[offset],
                                GSTK_ADDRESS_TAG))
              {
                gstk_status = gstk_parser_address_ss_string_tlv(
                                GSTK_ADDRESS_TAG,
                                cmd->envelope_response.data,
                                cc_rsp_data_len, /* data length */
                                offset,
                                &(response.cmd.mo_sms_cc_rsp.tp_dest_address),
                                (int32 *)&new_offset,
                                &more_tlv,
                                GSTK_OPTIONAL,
                                NULL);

                if(gstk_status != GSTK_SUCCESS)
                {
                   UIM_MSG_ERR_1("In MO SMS Ctrl: Parse TP Addr Fail 0x%x",
                                 gstk_status);
                   error_already_raised = TRUE;
                }
                else
                {
                  offset = new_offset;
                }
              } /* has tp address */
              else
              { /* error! can't have just rp and no tp address */
                error_already_raised = TRUE;
              }
            }  /* no error raised and more tlv */
            else
            { /* error already raised from rp parsing or only rp address! */
              error_already_raised = TRUE;
            }
          } /* has rp address */
          /* check for alpha */
          if(!error_already_raised && more_tlv)
          {
            if(gstk_valid_tag(cmd->envelope_response.data[offset],
                                GSTK_ALPHA_IDENTIFIER_TAG))
            {
              /* there is alpha */
              gstk_status = gstk_parser_alpha_identifier_tlv(cmd->envelope_response.data,
                                                cc_rsp_data_len, /* data length */
                                                offset,
                                                &(response.cmd.mo_sms_cc_rsp.alpha),
                                                (int32 *)&new_offset,
                                                &more_tlv,
                                                GSTK_OPTIONAL,
                                                NULL);

              if (gstk_status != GSTK_SUCCESS || more_tlv)
              { /* there shouldn't be any more tlvs */
                UIM_MSG_ERR_1("In MO SMS Ctrl: Parse Alpha Fail 0x%x",
                              gstk_status);
                error_already_raised = TRUE;
              }
              else
              {
                offset = new_offset;
              }

            } /* has alpha */
            else
            { /* error!, since alpha is the last possible tlv */
              error_already_raised = TRUE;
            }
          } /* no error raised from rp and tp checking and there is  more tlv */
        } /* if data[1] > 0 */
        if (!error_already_raised)
        {
          if (response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result == GSTK_ALLOWED_BUT_MOD)
          {
            if (response.cmd.mo_sms_cc_rsp.rp_dest_address.length == 0 ||
                response.cmd.mo_sms_cc_rsp.tp_dest_address.length == 0)
            {
              UIM_MSG_ERR_0("In MO SMS Ctrl: GSTK_ALLOWED_BUT_MOD with no data => Fail ");
              response.cmd.mo_sms_cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
            }
          }
        }
      } /* data is > 0 */
      else
      { /* no response data, -> ALLOW NO MOD */
        UIM_MSG_ERR_0("In MO SMS Ctrl: No response data when gen rsp = SUCCESS ");
        response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result = GSTK_ALLOWED_NO_MOD;
      }
      if(error_already_raised)
      {
        response.cmd.mo_sms_cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
      }
    } /* envelope command success */
  }

  gstk_status = gstk_util_send_response_to_clients(&response,
    cmd->envelope_response.message_header.user_data);

  return gstk_status;

} /* gstk_mo_sms_ctrl_rsp */

/*===========================================================================
FUNCTION gstk_timer_expire_rsp

DESCRIPTION

  This function builds the timer expiry rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_timer_expire_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   uint32                                      env_ref_id;
   uint8                                       timer_table_index;

   /* the envelope response is enum, so there is no need to initialize
   response */

   UIM_MSG_HIGH_0(" IN GSTK_TIMER_EXPIRE_RSP ");

   GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

   if(cmd == NULL)
   {
     return GSTK_BAD_PARAM;
   }

   env_ref_id = cmd->envelope_response.message_header.user_data;
   if(env_ref_id > GSTK_MAX_PROACTIVE_COMMAND) {
     UIM_MSG_ERR_1("Recd bad env ref id, 0x%x", env_ref_id);
     gstk_status = GSTK_ERROR;
     return gstk_status;
   }

   /* re-schedule next time expire envelope command if the response from card
      is busy.
      the next envelope cmd for the timer expiry envelope will be sent to card
      when the next successful terminal response is received from the card */

   /* reset the entry in the timer management table */
   timer_table_index = uint32touint8(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data);
   if (timer_table_index >= GSTK_MAX_TIMER_SUPPORTED) {
     UIM_MSG_ERR_1("Invalid Timer Table Index 0x%x", timer_table_index);
   }
   else {
     switch (cmd->envelope_response.rsp_status)
     {
     case GSTK_ENVELOPE_CMD_CARD_BUSY:
        UIM_MSG_HIGH_0("SIM Busy, Timer Expire Retry later");
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].waiting_rsp
          = FALSE;
        break;

     case GSTK_ENVELOPE_CMD_SUCCESS:
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].timer_started
          = FALSE;
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].pending
          = FALSE;
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].time = 0;
        (void)rex_clr_timer(
          &(gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].timer_elapsed));
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].waiting_rsp
          = FALSE;
        break;

     default:
        UIM_MSG_ERR_0("Timer Expire Envelope Err");
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].timer_started = FALSE;
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].pending = FALSE;
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].time = 0;
        (void)rex_clr_timer( &(gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].timer_elapsed));
        gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].waiting_rsp = FALSE;
        break;
     }
   }

   /* free the env cmd ref table */
   gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
   return gstk_status;

} /* gstk_timer_expire_rsp */

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_rsp

DESCRIPTION

  This function builds the cell broadcast rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
static gstk_status_enum_type gstk_cell_broadcast_dl_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   UIM_MSG_HIGH_0(" IN GSTK_CELL_BROADCAST_DL_RSP ");

   if(cmd == NULL)
   {
     UIM_MSG_ERR_0("cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* build the command */
   response.hdr_cmd.command_id = GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   response.cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;
   if(cmd->envelope_response.message_header.user_data <= GSTK_MAX_PROACTIVE_COMMAND)
   {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cmd->envelope_response.message_header.user_data);

   return gstk_status;

} /* gstk_cell_broadcast_dl_rsp */

/*===========================================================================
FUNCTION gstk_process_envelope_rsp

DESCRIPTION

  This function performs a lookup on the command id with the
  gstk_shared_data.envelope_cmd_ref_table and based on the envelope rsp type, it calls the
  corresponding envelope rsp function so that GSTK can build the corrresponding
  envelope response and sends it to the client

PARAMETERS
  cmd_ptr: [Input] Pointer to the envelope response types to be processed

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
gstk_status_enum_type gstk_process_envelope_rsp(
    gstk_cmd_type* cmd_ptr )
{
    gstk_status_enum_type    gstk_status    = GSTK_SUCCESS;
    gstk_cmd_enum_type       type_of_command= GSTK_CMD_ENUM_MAX;
    uint32                   env_ref_id     = GSTK_MAX_PROACTIVE_COMMAND+1;

    if(cmd_ptr == NULL)
    {
      UIM_MSG_ERR_0("cmd_ptr ERR:NULL");
      return GSTK_BAD_PARAM;
    }
    env_ref_id = cmd_ptr->envelope_response.message_header.user_data;

    if (env_ref_id > GSTK_MAX_PROACTIVE_COMMAND)
    {
      UIM_MSG_ERR_1("envelope ref id (0x%x) is invalid!",
                    cmd_ptr->envelope_response.message_header.user_data);
      return GSTK_BAD_PARAM;
    }

    /* Get the type of response */
    type_of_command = gstk_shared_data.envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type;

    switch( type_of_command )
    {
      case GSTK_MENU_IND_RSP:
        gstk_status = gstk_menu_selection_rsp(cmd_ptr);
        break;
      case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
        gstk_status = gstk_sms_pp_dl_rsp(cmd_ptr);
        break;
      case GSTK_MT_CALL_EVT_IND_RSP:
      case GSTK_CALL_CONNECTED_EVT_IND_RSP:
      case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
      case GSTK_IDLE_SCRN_EVT_IND_RSP:
      case GSTK_LOCATION_STATUS_EVT_IND_RSP:
      case GSTK_USER_ACTIVITY_EVT_IND_RSP:
      case GSTK_LANG_SELECTION_EVT_IND_RSP:
      case GSTK_ACCESS_TECH_EVT_IND_RSP:
      case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
      case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
      case GSTK_DATA_AVAIL_EVT_IND_RSP:
      case GSTK_CH_STATUS_EVT_IND_RSP:
      case GSTK_NW_REJ_EVT_IND_RSP:
      case GSTK_CSG_CELL_SELECTION_EVT_IND_RSP:
      case GSTK_IMS_REG_EVT_IND_RSP:
        gstk_status = gstk_evt_dl_rsp(cmd_ptr);
        break;
      case GSTK_MO_SMS_CTRL_IND_RSP:
        gstk_status = gstk_mo_sms_ctrl_rsp(cmd_ptr);
        break;
      case GSTK_CC_IND_RSP:
        gstk_status = gstk_cc_rsp(cmd_ptr);
        break;
      case GSTK_TIMER_EXPIRE_IND_RSP:
        gstk_status = gstk_timer_expire_rsp(cmd_ptr);
        break;
      case GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP:
        gstk_status = gstk_cell_broadcast_dl_rsp(cmd_ptr);
        break;
      case GSTK_RAW_ENVELOPE_RSP:
        gstk_status = gstk_raw_envelope_rsp(cmd_ptr);
        break;
      case GSTK_LOC_INFO_ENVELOPE_RSP:
        gstk_status = gstk_loc_info_envelope_rsp(cmd_ptr);
        break;
      default:
        UIM_MSG_ERR_1("In Env Rsp Process, unknown cmd type 0x%x",
                      type_of_command);
        gstk_status = GSTK_ERROR;
        break;
    }/* End of switch */
    return gstk_status;
} /* gstk_process_envelope_rsp*/

/*===========================================================================
FUNCTION gstk_non_idle_st_menu_selection_err_rsp

DESCRIPTION

  This function builds the menu selection error rsp in non idle state
  and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
  GSTK_BAD_PARAM,
  GSTK_SUCCESS

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_menu_selection_err_rsp(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   UIM_MSG_HIGH_0(" IN GSTK_NON_IDLE_ST_MENU_SELECITON_ERR_RSP ");

   if(menu_sel_cmd == NULL)
   {
     UIM_MSG_ERR_0("menu_sel_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* build the command */
   response.hdr_cmd.command_id           = GSTK_MENU_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
   response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
   response.hdr_cmd.sim_slot_id          = menu_sel_cmd->message_header.sim_slot_id;

   if(menu_sel_cmd->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[menu_sel_cmd->message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     menu_sel_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_menu_selection_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_evt_dl_err_rsp

DESCRIPTION

  This function builds the event download rsp error in non idle state
  and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_evt_dl_err_rsp(
  const gstk_cmd_type * cmd)
{
    gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
    gstk_cmd_from_card_type                     response;
    gstk_cmd_enum_type                          type_of_command;
    uint32                                      env_ref_id;

    /* the envelope response is enum, so there is no need to initialize
       response */

    UIM_MSG_HIGH_0(" IN GSTK_NON_IDLE_ST_EVT_DL_ERR_RSP ");

    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

    if(cmd == NULL)
    {
      return GSTK_BAD_PARAM;
    }

    env_ref_id = cmd->call_connected_envelope_ind.message_header.user_data;
    if(env_ref_id >= GSTK_MAX_PROACTIVE_COMMAND) {
      UIM_MSG_ERR_1("Recd bad env ref id, 0x%x", env_ref_id);
      gstk_status = GSTK_ERROR;
      return gstk_status;
    }

    memset(&response, 0, sizeof(response));

    response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data;

    response.hdr_cmd.sim_slot_id =
      gstk_util_get_sim_slot_id(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].client_id);

    /* Get the type of response */
    type_of_command = gstk_shared_data.envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type;

    switch( type_of_command )
    {
      case GSTK_MT_CALL_EVT_IND_RSP:
      case GSTK_CALL_CONNECTED_EVT_IND_RSP:
      case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
      case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
      case GSTK_LANG_SELECTION_EVT_IND_RSP:
      case GSTK_LOCATION_STATUS_EVT_IND_RSP:
      case GSTK_DATA_AVAIL_EVT_IND_RSP:
      case GSTK_CH_STATUS_EVT_IND_RSP:
      case GSTK_ACCESS_TECH_EVT_IND_RSP:
      case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
      case GSTK_CSG_CELL_SELECTION_EVT_IND_RSP:
        gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = FALSE;
        gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
        break; /* ignore, doesn't have to sent back! */
      /* Send back to user */
      case GSTK_IDLE_SCRN_EVT_IND_RSP:
        /* build the command */
        response.hdr_cmd.command_id           = type_of_command;
        response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
        if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
        {
          response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_CARD_BUSY;
        }
        else
        {
          response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
        }

        gstk_status = gstk_util_send_response_to_clients(&response,
          env_ref_id);

        /* remove the bitmask */
        gstk_curr_inst_ptr->gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE]= 0x00;

        /* notify client to remove their mask */
        response.hdr_cmd.command_id = GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY;
        response.cmd.evt_notify_req.require_notification = FALSE;
        gstk_status = gstk_util_send_message_to_clients(&response);
        break;

      case GSTK_USER_ACTIVITY_EVT_IND_RSP:
        /* build the command */
        response.hdr_cmd.command_id           = type_of_command;
        response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
        if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
        {
          response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_CARD_BUSY;
        }
        else
        {
          response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
        }

        gstk_status = gstk_util_send_response_to_clients(&response,
          env_ref_id);

        /* remove the bitmask */
        gstk_curr_inst_ptr->gstk_evt_list[GSTK_USER_ACTIVITY]= 0x00;

        /* notify client to remove their mask */
        response.hdr_cmd.command_id = GSTK_SETUP_EVT_USER_ACT_NOTIFY;
        response.cmd.evt_notify_req.require_notification = FALSE;
        gstk_status = gstk_util_send_message_to_clients(&response);
        break;
      default:
        /* no nothing */
        break;
    }
    return gstk_status;
} /* gstk_non_idle_st_evt_dl_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_sms_pp_err_rsp

DESCRIPTION

  This function builds the envelope SMS PP error rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_sms_pp_err_rsp(
  const gstk_envelope_sms_pp_download_ind_type * sms_pp_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   UIM_MSG_HIGH_0(" IN GSDI_NON_IDLE_ST_SMS_PP_DL_ERR_RSP ");

   if(sms_pp_cmd == NULL)
   {
     UIM_MSG_ERR_0("sms_pp_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize response */
   memset(&response, 0, sizeof(gstk_sms_pp_download_rsp_type)+sizeof(gstk_exp_hdr_type));

   /* build the command */
   response.hdr_cmd.sim_slot_id          = sms_pp_cmd->message_header.sim_slot_id;
   response.hdr_cmd.command_id           = GSTK_SMS_PP_DOWNLOAD_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   if(sms_pp_cmd->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[sms_pp_cmd->message_header.user_data].user_data;
   }

   if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_CARD_BUSY;
   }
   else
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
   }
   response.cmd.sms_pp_download_rsp.response_data_present = FALSE;

   gstk_status = gstk_util_send_response_to_clients(&response,
     sms_pp_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_sms_pp_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_cell_change_brcst_err_rsp

DESCRIPTION

  This function builds the cell brcst rsp error message in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the call as the Toolkit
  is no longer valid

PARAMETERS
  cell_brcst_cmd_ptr: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_cell_change_brcst_err_rsp(
  const gstk_envelope_cell_broadcast_download_ind_type * cell_brcst_cmd_ptr)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   UIM_MSG_HIGH_0(" IN GSTK_NON_IDLE_ST_CELL_CHANGE_BRCST_ERR_RSP ");

   if(cell_brcst_cmd_ptr == NULL)
   {
     UIM_MSG_ERR_0("cell_brcst_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   memset(&response, 0, sizeof(response));

   /* build the command */
   response.hdr_cmd.sim_slot_id          = cell_brcst_cmd_ptr->message_header.sim_slot_id;
   response.hdr_cmd.command_id           = GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_CARD_BUSY;
   }
   else
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
   }
   if(cell_brcst_cmd_ptr->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND)
   {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[cell_brcst_cmd_ptr->message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cell_brcst_cmd_ptr->message_header.user_data);

   return gstk_status;
} /* gstk_non_idle_st_cell_change_brcst_err_rsp */


/*===========================================================================
FUNCTION gstk_non_idle_st_cc_err_rsp

DESCRIPTION

  This function builds the call control rsp error message in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the call as the Toolkit
  is no longer valid

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_cc_err_rsp(
  const gstk_envelope_cc_ind_type * cc_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   UIM_MSG_HIGH_0(" IN GSTK_NON_IDLE_ST_CC_ERR_RSP ");

   if(cc_cmd == NULL)
   {
     UIM_MSG_ERR_0("cc_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize response */
   memset(&response, 0, sizeof(gstk_cc_rsp_type)+sizeof(gstk_exp_hdr_type));

   /* build the command */
   response.hdr_cmd.command_id           = GSTK_CC_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   response.hdr_cmd.sim_slot_id          = cc_cmd->message_header.sim_slot_id;

   if(cc_cmd->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[cc_cmd->message_header.user_data].user_data;
   }
   if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_CARD_BUSY;
   }
   else
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_SUCCESS;
   }

   response.cmd.cc_rsp.call_ctrl_result = GSTK_ALLOWED_NO_MOD;

   gstk_status = gstk_util_send_response_to_clients(&response,
     cc_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_cc_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_mo_sms_ctrl_err_rsp

DESCRIPTION

  This function builds the sms control download error rsp in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the SMS as the Toolkit
  is no longer valid

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_mo_sms_ctrl_err_rsp(
  const gstk_envelope_mo_sms_ctrl_ind_type * mo_sms_ctrl_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   UIM_MSG_HIGH_0(" IN GSTK_NON_IDLE_ST_MO_SMS_CTRL_ERR_RSP ");

   if(mo_sms_ctrl_cmd == NULL)
   {
     UIM_MSG_ERR_0("mo_sms_ctrl_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize response */
   memset(&response, 0, sizeof(gstk_mo_sms_ctrl_rsp_type)+sizeof(gstk_exp_hdr_type));

   /* build the command */
   response.hdr_cmd.command_id           = GSTK_MO_SMS_CTRL_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   response.hdr_cmd.sim_slot_id          = mo_sms_ctrl_cmd->message_header.sim_slot_id;
   if(mo_sms_ctrl_cmd->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[mo_sms_ctrl_cmd->message_header.user_data].user_data;
   }
   if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_CARD_BUSY;
   }
   else
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_SUCCESS;
   }
   response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result = GSTK_ALLOWED_NO_MOD;

   gstk_status = gstk_util_send_response_to_clients(&response,
     mo_sms_ctrl_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_mo_sms_ctrl_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_timer_expire_err_rsp

DESCRIPTION

  This function builds the timer expiry non idle state error rsp and sends
  it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_timer_expire_err_rsp(
  const gstk_envelope_timer_expire_ind_type * timer_expire_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   uint32                                      env_ref_id;
   uint8                                       timer_table_index;

   /* the envelope response is enum, so there is no need to initialize
   response */

   GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

   if(timer_expire_cmd == NULL)
   {
     return GSTK_BAD_PARAM;
   }

   env_ref_id = timer_expire_cmd->message_header.user_data;
   timer_table_index = uint32touint8(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data);
   UIM_MSG_HIGH_2(" IN GSTK_NON_IDLE_ST_TIMER_EXPIRE_ERR_RSP, env ref id 0x%x, Timer Table Index 0x%x",
                  timer_expire_cmd->message_header.user_data,
                  timer_table_index);

   if(env_ref_id >= GSTK_MAX_PROACTIVE_COMMAND) {
     gstk_status = GSTK_ERROR;
     return gstk_status;
   }

   /* re-schedule next time expire envelope command if the response from card
      is busy.
      the next envelope cmd for the timer expiry envelope will be sent to card
      when the next successful terminal response is received from the card */

   /* reset the entry in the timer management table */

   if (timer_table_index < GSTK_MAX_TIMER_SUPPORTED)
   {
      UIM_MSG_ERR_0("Timer Expire Envelope Err");
      gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].timer_started = FALSE;
      gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].pending       = FALSE;
      gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].time          = 0;
      (void)rex_clr_timer( &(gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].timer_elapsed));
      gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].waiting_rsp = FALSE;
   }

   /* free the env cmd ref table */
   gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
   return gstk_status;

} /* gstk_non_idle_st_timer_expire_err_rsp */


/*===========================================================================
FUNCTION gstk_non_idle_st_raw_env_err_rsp

DESCRIPTION

  This function builds the raw envelope non idle state error rsp and sends
  it to client

PARAMETERS
  raw_env_ptr: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
  GSTK_SUCCESS,
  GSTK_NULL_INPUT_PARAM

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_raw_env_err_rsp(
  const gstk_raw_envelope_ind_type * raw_env_ptr)
{
  gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
  /* the envelope response is enum, so there is no need to initialize
     response */
  gstk_cmd_from_card_type                     response;

  UIM_MSG_HIGH_0(" IN gstk_non_idle_st_raw_env_err_rsp() ");

  if(raw_env_ptr == NULL)
  {
    UIM_MSG_ERR_0("raw_env_ptr ERR:NULL");
    return GSTK_NULL_INPUT_PARAM;
  }

  memset(&response, 0, sizeof(gstk_cmd_from_card_type));
  /* build the command */
  response.hdr_cmd.command_id           = GSTK_RAW_ENVELOPE_RSP;
  response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
  response.hdr_cmd.sim_slot_id          = raw_env_ptr->message_header.sim_slot_id;
   if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_CARD_BUSY;
   }
   else
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
   }
  if(raw_env_ptr->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
    response.hdr_cmd.user_data =
      gstk_shared_data.envelope_cmd_ref_table[raw_env_ptr->message_header.user_data].user_data;
  }

  gstk_status = gstk_util_send_response_to_clients(&response,
    raw_env_ptr->message_header.user_data);
  return gstk_status;
} /* gstk_non_idle_st_streaming_env_err_rsp */

