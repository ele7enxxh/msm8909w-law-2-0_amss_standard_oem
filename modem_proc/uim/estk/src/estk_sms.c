/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  SHORT  MESSAGE


GENERAL DESCRIPTION : ENHANCED STK layer Send SMS support

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2008, 2010 - 2011, 2013-2016 QUALCOMM Technologies, Inc (QTI) and
its licensors.  All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_sms.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/16   gs      F3 Logging inprovements
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
01/12/15   vr      Correction in SEND SMS TR result values
12/16/14   vr      Check WMS ready before sending SMS to WMS
10/08/14   gm      Remove GSTK registration with WMS if FEATURE_ESTK is enabled
09/09/14   vr      Reduce stack usage
09/09/14   bd      Features clean up
08/07/14   dy      Process WMS events in GSTK task context 
07/21/14   sw      Wait for display confirmation before sending terminal response
07/11/14   shr     Add Support for NV Refresh
06/27/14   bd      Work-around for TR problem when FEATURE_ESTK_QMI is disabled but
                   QMI CAT is rgeistered to pick up proactive commands
05/27/14   bd      Changes to hold null alpha and no alpha indications
05/06/14   gs      Removed the ESTK featurization
04/03/14   vr      Do not send Alpha to client when IGNORE_ALPHA is enabled
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Cleanup global variables while handling STOP signal
10/04/13   gm      F3 message reduction
09/13/13   gm      Support for Recovery and hot-swap
08/06/13   vr      Centralized Call Control changes
06/20/13   vr      Triton TSTS changes
04/09/13   vr      Use safer versions of memcpy() and memmove()
06/13/11   sg      Move FEATURE_ESTK_IGNORE_ALPHA to NV
04/28/11   sg      Fix SMS over NAS on SVLTE Type II
01/12/11   adp     Fixing Compiler Error, file got renamed.
09/29/10   xz      KW Fixes
08/18/10   dd      Fix KW error
06/30/10   dd      Icon display confirmation
06/04/10   bd      Fixed the issue of handling null data object in ALPHA TLV
05/24/10   sg      Ignore Alpha TLV for targets without UI
03/09/10   bd      Fixed terminal response for different WMS status
02/15/10   nb      Moved Dual Slot Changes
07/15/09   xz      1) Fix issue of setting wrong DCS in SMS TPDU after pack
                   2) Send error TR if packing fails
12/16/08   sk      Fixed featurization
11/03/08   sk      Featurized ESTK based on external interface support
09/16/08   sk      Added lint fixes
08/21/08   xz      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "intconv.h"
#include <stringl/stringl.h>
#include "uim_msg.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_ESTK
#include "estk_sms.h"
#include "sys.h"
#include "wms.h"

/*===========================================================================
FUNCTION: estk_process_wms_msg_submit_rpt_event_cmd

DESCRIPTION :
  This function will be called by ESTK to process WMS Message Submit
  Report event

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing of WMS event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_wms_msg_submit_rpt_event_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  gstk_additional_info_ptr_type       additional_result       = {0x00};
  uint8                               additional_result_data  = 0x00;
  uint32                              cmd_ref_id              = 0x00;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */  
  gstk_general_result_enum_type       general_result          = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  gstk_additional_info_ptr_type      *additional_result_ptr   = NULL;

  if (cmd_ptr == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  cmd_ref_id =
    estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;

  UIM_MSG_HIGH_2("estk_process_wms_msg_submit_rpt_event_cmd, cmd_ref_id: 0x%x, report_status: 0x%x",
           cmd_ref_id,
           cmd_ptr->cmd_data.wms_msg_sumbit_rpt_evt.report_status);

  switch(cmd_ptr->cmd_data.wms_msg_sumbit_rpt_evt.report_status)
  {
    case WMS_RPT_OK:
      general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      additional_result_ptr = NULL;
      break;

    case WMS_RPT_MO_CONTROL_DISALLOW:
    case WMS_RPT_MO_CONTROL_ERROR:
      /* Call control did not allow mo sms send or Call control Error*/
      additional_result.length = 1;
      additional_result_data = int32touint8((int32)GSTK_ACTION_NOT_ALLOWED);
      additional_result.additional_info_ptr = &additional_result_data;
      general_result = GSTK_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM;
      additional_result_ptr = &additional_result;
      break;

    case WMS_RPT_RP_ERROR:
      general_result = GSTK_SMS_RP_ERROR;
      additional_result.length = 1;
      additional_result_data =
        0x7F & cmd_ptr->cmd_data.wms_msg_sumbit_rpt_evt.cause_info.cause_value;
      additional_result.additional_info_ptr = &additional_result_data;
      additional_result_ptr = &additional_result;
      break;

    case WMS_RPT_CP_ERROR:
    case WMS_RPT_NO_RESPONSE_FROM_NETWORK:
    case WMS_RPT_NO_RESPONSE_FROM_NEWTWORK_FOR_SUBMIT_REPORT:
    case WMS_RPT_NETWORK_NOT_READY:
    case WMS_RPT_CDMA_TL_ERROR:
    case WMS_RPT_NO_ACK:
      general_result = GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      additional_result.length = 1;
      additional_result_data = 
        int32touint8((int32)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      additional_result.additional_info_ptr = &additional_result_data;
      additional_result_ptr = &additional_result;
      break;

    case WMS_RPT_1X_AND_VOICE_NOT_ALLOWED:
      general_result = GSTK_ACCESS_TECH_UNABLE_TO_PROCESS_CMD;
      additional_result_ptr = NULL;
      break;

    default:
      additional_result.length = 1;
      additional_result_data =
        int32touint8((int32)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      additional_result.additional_info_ptr = &additional_result_data;
      general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      additional_result_ptr = &additional_result;
      break;
  }

  estk_curr_inst_ptr->additional_result.additional_info_ptr = NULL;
  estk_curr_inst_ptr->additional_result.length = 0;

   /* SMS message has been sent, this event received from wms event callback */
  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
  {
    UIM_MSG_HIGH_0("Saving SEND SMS TR: Waiting for alpha display rsp");        
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
    estk_curr_inst_ptr->result = general_result;

    if(additional_result_ptr != NULL)
    {
      estk_cache_additional_info(&additional_result);
    }
#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */   
  }
  else
  {
    (void)estk_send_terminal_response(
            cmd_ref_id,                          /* command_details_ref */
            GSTK_SEND_SMS_CNF,                   /* command response    */
            general_result,                      /* general result      */
            additional_result_ptr,               /* additional result   */
            NULL);                               /* extra param         */
  }
  return ESTK_SUCCESS;
} /* estk_process_wms_msg_submit_rpt_event_cmd */

/*===========================================================================
FUNCTION: estk_wms_event_cb

DESCRIPTION:
  This function is the event callback function for WMS.

PARAMETERS:
  wms_event:  [Input] Command whose status is being reported
  info_ptr:   [Input] WMS info buffer
  shared_ptr: [Input] WMS indication of whether the info can be passed to
                      other clients.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_wms_event_cb(
  wms_msg_event_e_type              wms_event,
  const wms_msg_event_info_s_type  *info_ptr,
  const boolean                    *shared_ptr)
{
  estk_cmd_type          *task_cmd_ptr = NULL;
  gstk_slot_id_enum_type  slot_id      = GSTK_SLOT_ID_MAX;

  (void)shared_ptr;

  if (info_ptr == NULL)
  {
    UIM_MSG_ERR_2("estk_wms_event_cb, NULL PTR: info_ptr = 0x%x, shared_ptr = 0x%x",
              info_ptr, shared_ptr);
    return;
  }

  if(wms_event == WMS_MSG_EVENT_MT_MESSAGE_ERROR)
  {
    gstk_io_ctrl(
       GSTK_IO_SEND_WMS_MT_MESSAGE_ERROR_EVENT, 
       (void *)info_ptr, 
       &slot_id);
    return;    
  }

  slot_id = estk_shared_info.slot_owns_wms_cli;
  if (!ESTK_IS_VALID_SLOT_ID(slot_id) || slot_id == GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_1("estk_wms_event_cb, invalid slot id 0x%x", slot_id);
    return;
  }

  UIM_MSG_HIGH_3("In estk_wms_event_cb, event: 0x%x, report_status: 0x%x, slot_id: 0x%x",
           wms_event,
           info_ptr->submit_report_info.report_status,
           slot_id);

  if (estk_instances_ptr[(uint8)slot_id - 1] == NULL)
  {
    UIM_MSG_ERR_1("estk_wms_event_cb, estk_instances_ptr[%d] is NULL",
              ((uint8)slot_id - 1));
    return;
  }

  if (!estk_instances_ptr[(uint8)slot_id - 1]->curr_cmd.hdr_cmd.command_id)
  {
    UIM_MSG_ERR_1("estk_wms_event_cb, slot 0x%x, No WMS command", slot_id);
    return;
  }

  switch(wms_event)
  {
    case WMS_MSG_EVENT_SUBMIT_REPORT:
      if(info_ptr->submit_report_info.client_id == WMS_CLIENT_TYPE_CAT)
      {
        task_cmd_ptr = estk_get_cmd_buf();
        
        if(task_cmd_ptr == NULL)
        {
          UIM_MSG_ERR_0("estk_wms_event_cb, No memory");
          return;
        }
        
        /* Set the command header */
        task_cmd_ptr->hdr.cli_user_data    = 0;
        task_cmd_ptr->hdr.cmd              = ESTK_WMS_MSG_SUBMIT_RPT_EVT_CMD;
        task_cmd_ptr->hdr.sim_slot_id      = slot_id;
        
        /* Set the report status */   
        task_cmd_ptr->cmd_data.wms_msg_sumbit_rpt_evt.report_status =
          info_ptr->submit_report_info.report_status;
        task_cmd_ptr->cmd_data.wms_msg_sumbit_rpt_evt.cause_info =
          info_ptr->submit_report_info.cause_info;

        /* Queue the command to ESTK */
        estk_put_cmd(task_cmd_ptr);
      }
      break;

    default:
      UIM_MSG_MED_1("estk_wms_event_cb, Unhandled event 0x%x", wms_event);
      break;
  }
} /* estk_wms_event_cb */

/*===========================================================================
FUNCTION: estk_wms_cmd_cb

DESCRIPTION:
  This is the command callback function for the wms_send_msg() call.

PARAMETERS:
  cmd           : [Input] Command to which the callback is invoked
  user_data_ptr : [Input] user data, used as cmd_reference_id
  cmd_err       : [Input] command result

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_wms_cmd_cb (
  wms_cmd_id_e_type  cmd,
  const void        *user_data_ptr,
  wms_cmd_err_e_type cmd_err
)
{
  uint32                    cmd_ref_id = GSTK_MAX_PROACTIVE_COMMAND;
  gstk_slot_id_enum_type    slot_id    = GSTK_SLOT_ID_MAX;  

  UIM_MSG_HIGH_3("estk_wms_cmd_cb: cmd=0x%x, user_data_ptr=0x%x, cmd_err=0x%x",
                 cmd, user_data_ptr, cmd_err);

  slot_id = estk_shared_info.slot_owns_wms_cli;
  if (!ESTK_IS_VALID_SLOT_ID(slot_id) || slot_id == GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot_id);
    return;
  }

  if (!estk_instances_ptr[(uint8)slot_id - 1]->curr_cmd.hdr_cmd.command_id)
  {
    UIM_MSG_ERR_0("estk_wms_cmd_cb: No SM cmd!");
    return;
  }

  if (cmd == WMS_CMD_MSG_SEND)
  {
    if(cmd_err != WMS_CMD_ERR_NONE)
    { /* The error in this call back is because of internal ME error */
      cmd_ref_id = (uint32)((void*)user_data_ptr);
      UIM_MSG_ERR_1("cmd_err 0x%x ", cmd_err);
      
      if(estk_instances_ptr[(uint8)slot_id - 1]->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        estk_instances_ptr[(uint8)slot_id - 1]->state = ESTK_PENDING_TR_ST;
        estk_instances_ptr[(uint8)slot_id - 1]->result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      }
      else
      {
        (void)estk_send_terminal_response(
                cmd_ref_id,                          /* command_details_ref */
                GSTK_SEND_SMS_CNF,                   /* command response    */
                GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                                     /* general result      */
                NULL,                                /* additional result   */
                NULL);                               /* extra param         */
      }
    }
  }
  /*if no error was recieved its okay, wait for event callback to be called */
} /*estk_wms_cmd_cb */

/*===========================================================================
FUNCTION: estk_wms_cfg_cb

DESCRIPTION:
  This function is the cfg callback function for WMS.

PARAMETERS:
  wms_event:  [Input] Configuration event type
  info_ptr:   [Input] Configuration event information


DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_wms_cfg_cb(
  wms_cfg_event_e_type              wms_event,
  wms_cfg_event_info_s_type        *info_ptr)
{
  estk_cmd_type          *task_cmd_ptr = NULL;
  gstk_slot_id_enum_type  slot_id      = GSTK_SLOT_ID_MAX;

  if (info_ptr == NULL)
  {
    UIM_MSG_ERR_1("estk_wms_cfg_cb, NULL PTR: info_ptr = 0x%x",
              info_ptr);
    return;    
  }

  if (wms_event != WMS_CFG_EVENT_MS_READY &&
      wms_event != WMS_CFG_EVENT_MS_NOT_READY)
  {
    UIM_MSG_ERR_1("estk_wms_cfg_cb, invalid wms_event id 0x%x", wms_event);
    return;
  }

  if (gstk_io_ctrl(
        GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
        &info_ptr->ms_ready.as_id,
        &slot_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get slot_id for as_id 0x%x!",
                  info_ptr->ms_ready.as_id);
    return;
  }
  if (!ESTK_IS_VALID_SLOT_ID(slot_id) || slot_id == GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_1("estk_wms_event_cb, invalid slot id 0x%x", slot_id);
    return;
  }

  UIM_MSG_HIGH_3("In estk_wms_cfg_cb, event: 0x%x, sys type: 0x%x, slot: 0x%x",
           wms_event,
           info_ptr->ms_ready.sys_type,
           slot_id);

  if (estk_instances_ptr[(uint8)slot_id - 1] == NULL)
  {
    UIM_MSG_ERR_1("estk_wms_cfg_cb, estk_instances_ptr[%d] is NULL",
              ((uint8)slot_id - 1));
    return;
  }

  task_cmd_ptr = estk_get_cmd_buf();
  
  if(task_cmd_ptr == NULL)
  {
    return;
  }
  
  /* Set the command header */
  task_cmd_ptr->hdr.cli_user_data    = 0;
  task_cmd_ptr->hdr.cmd              = ESTK_WMS_CFG_EVT_CMD;
  task_cmd_ptr->hdr.sim_slot_id      = slot_id;
  
  /* Set the report status */   
  task_cmd_ptr->cmd_data.wms_cfg_evt.wms_cfg_event = wms_event;
  task_cmd_ptr->cmd_data.wms_cfg_evt.sys_type = info_ptr->ms_ready.sys_type;

  /* Queue the command to ESTK */
  estk_put_cmd(task_cmd_ptr);

} /* estk_wms_cfg_cb */

/*===========================================================================
FUNCTION: estk_process_wms_cfg_event_cmd

DESCRIPTION :
  This function will be called by ESTK to process WMS Configuration event

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing of WMS event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_wms_cfg_event_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  if (cmd_ptr == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  UIM_MSG_HIGH_2("estk_process_wms_cfg_event_cmd, existing GW ready :0x%x,\
                  CDMA ready :0x%x", estk_curr_inst_ptr->wms_msg_cfg.wms_ready_gw,
                  estk_curr_inst_ptr->wms_msg_cfg.wms_ready_cdma);

  switch(cmd_ptr->cmd_data.wms_cfg_evt.wms_cfg_event)
  {
    case WMS_CFG_EVENT_MS_READY:
      if(cmd_ptr->cmd_data.wms_cfg_evt.sys_type == WMS_CFG_SYS_GW)
      {
        estk_curr_inst_ptr->wms_msg_cfg.wms_ready_gw = TRUE;
      }
      else
      {
        estk_curr_inst_ptr->wms_msg_cfg.wms_ready_cdma = TRUE;
      }
      break;
    case WMS_CFG_EVENT_MS_NOT_READY:
      if(cmd_ptr->cmd_data.wms_cfg_evt.sys_type == WMS_CFG_SYS_GW)
      {
        estk_curr_inst_ptr->wms_msg_cfg.wms_ready_gw = FALSE;
      }
      else
      {
        estk_curr_inst_ptr->wms_msg_cfg.wms_ready_cdma = FALSE;
      }
      break;
    default:
      break;
  }

  UIM_MSG_HIGH_2("estk_process_wms_cfg_event_cmd, wms cfg evnet :0x%x,\
                  wms msg ptr :0x%x", cmd_ptr->cmd_data.wms_cfg_evt.wms_cfg_event,
                  estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr);

  /* Check if any cached wms msg is present and send if WMS is ready */
  if(cmd_ptr->cmd_data.wms_cfg_evt.wms_cfg_event == WMS_CFG_EVENT_MS_READY &&
     estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr != NULL)
  {
    wms_client_message_s_type *wms_msg_ptr = estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr;
    sys_modem_as_id_e_type     as_id       = SYS_MODEM_AS_ID_1;
    wms_status_e_type          wms_status  = WMS_OK_S;

    if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id != GSTK_SEND_SMS_REQ)
    {
      UIM_MSG_ERR_0("No SEND SMS proactive command pending");
      gstk_free(estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr);
      estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr = NULL;
      return ESTK_ERROR;
    }

    if (gstk_io_ctrl(
          GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
          &estk_shared_info.slot_owns_wms_cli,
          &as_id) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                    estk_shared_info.slot_owns_wms_cli);
      gstk_free(estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr);
      estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr = NULL;
      return ESTK_ERROR;
    }

    if((wms_msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW &&
        estk_curr_inst_ptr->wms_msg_cfg.wms_ready_gw) ||
       (wms_msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA &&
        estk_curr_inst_ptr->wms_msg_cfg.wms_ready_cdma))
    {
      wms_status = wms_msg_ms_send(
        estk_shared_info.wms_client_id,
        as_id,
        (wms_cmd_cb_type)estk_wms_cmd_cb,
        (const void*)estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
        WMS_SEND_MODE_CLIENT_MESSAGE,
        wms_msg_ptr);

      if (wms_status != WMS_OK_S)
      {
        UIM_MSG_ERR_1("wms_msg_send failed 0x%x", wms_status);
      }
      gstk_free(estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr);
      estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr = NULL;
    }
  }
  return ESTK_SUCCESS;
} /* estk_process_wms_cfg_event_cmd */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE)
/*=========================================================================
FUNCTION:  estk_ts_decode_address

DESCRIPTION:
  Decode address data into a structure.

PARAMETERS:
  data_ptr : [Input] Input data to be decode
  addr_ptr : [Ouput] WMS Address data structure to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  Number of bytes decoded.

COMMENTS:
  None

SIDE EFFECTS:
  None

=========================================================================*/
static uint8 estk_ts_decode_address (
  const uint8               * data_ptr,
  wms_address_s_type        * addr_ptr
)
{
  uint8   i, pos = 0;

  if((data_ptr == NULL) || addr_ptr == NULL)
  {
    /* data is NULL */
    UIM_MSG_ERR_0("Parameter for estk_ts_decode_address is NULL");
    return 0;
  }

  /* Len field: number of digits */
  i = data_ptr[pos];

  if( i > WMS_GW_ADDRESS_MAX )
  {
    /* Address is too long */
    UIM_MSG_ERR_1("Addr len too long: %d", i);
    return 0;
  }

  addr_ptr->number_of_digits = i;

  pos ++;

  /* TON & NPI: */
  addr_ptr->digit_mode  = WMS_DIGIT_MODE_4_BIT;

  addr_ptr->number_type =
    (wms_number_type_e_type)(( data_ptr[pos] & 0x70 ) >> 4);

  addr_ptr->number_plan = (wms_number_plan_e_type)(data_ptr[pos] & 0x0F);
  pos ++;

  if (addr_ptr->number_type == WMS_NUMBER_ALPHANUMERIC )
  {
    uint8 bytes_increment = 0;

    /* Alphanumberic Number Type */
    addr_ptr->digit_mode = WMS_DIGIT_MODE_8_BIT;

    /* length = number of BCD digits */
    bytes_increment = (addr_ptr->number_of_digits+1)/2;

    pos += bytes_increment;
  }
  else
  {
    /* the digits: */
    for( i = 0; i < addr_ptr->number_of_digits; i++ )
    {
      /* unpack two digits each time */
      addr_ptr->digits[i++] = data_ptr[pos] & 0x0F;
      addr_ptr->digits[i]   = ( data_ptr[pos] & 0xF0 ) >> 4;
      pos ++;
    }
  }
  return pos;
} /* estk_ts_decode_address() */
#endif /* #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE)*/

/*===========================================================================
FUNCTION: estk_sms_reg

DESCRIPTION:
  ESTK registration of WMS client

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_sms_reg( void )
{
  wms_client_err_e_type wms_client_err = WMS_CLIENT_ERR_MAX;

  UIM_MSG_HIGH_0("In estk_sms_reg()");

  wms_client_err = wms_client_init(WMS_CLIENT_TYPE_CAT,
                                   &estk_shared_info.wms_client_id );
  if (wms_client_err != WMS_CLIENT_ERR_NONE)
  {
    UIM_MSG_ERR_1("Failed to init ESTK WMS client. (0x%x)", wms_client_err);
    return;
  }

  wms_client_err = wms_client_activate( estk_shared_info.wms_client_id );

  if (wms_client_err != WMS_CLIENT_ERR_NONE)
  {
    (void)wms_client_release( estk_shared_info.wms_client_id );
    UIM_MSG_ERR_1("Failed to activate ESTK WMS client. (0x%x)",
                  wms_client_err);
    return;
  }

  wms_client_err = wms_client_reg_msg_cb(
    estk_shared_info.wms_client_id,
    (wms_msg_event_cb_type)estk_wms_event_cb);

  if (wms_client_err != WMS_CLIENT_ERR_NONE)
  {
    (void)wms_client_release( estk_shared_info.wms_client_id );
    UIM_MSG_ERR_1("Failed to reg ESTK WMS evt cb. (0x%x)", wms_client_err);
  }

  wms_client_err = wms_client_reg_cfg_cb(
    estk_shared_info.wms_client_id,
    (wms_cfg_event_cb_type)estk_wms_cfg_cb);

  if (wms_client_err != WMS_CLIENT_ERR_NONE)
  {
    (void)wms_client_release( estk_shared_info.wms_client_id );
    UIM_MSG_ERR_1("Failed to reg ESTK WMS evt cb. (0x%x)", wms_client_err);
  }
  estk_shared_info.slot_owns_wms_cli = GSTK_SLOT_ID_MAX;
} /* estk_sms_reg */

/*===========================================================================
FUNCTION: estk_process_send_sms_req

DESCRIPTION:
  GSTK calls this ESTK function to process the SEND SMS request.

PARAMETERS:
  gstk_req_ptr : [Input] gstk SEND SMS cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS: - When SMS is successfully sent
    ESTK_ERROR:   - When there is failure in sending SMS to WMS module

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_sms_req(
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  gstk_send_sms_req_type    *send_sms_req_ptr = NULL;
  wms_status_e_type          wms_status       = WMS_OK_S;
  uint32                     dcs_pos          = 2;
  /* Cannot do {0} due to compilation warning on enumerated type
     mixed with another type */
  wms_client_message_s_type *wms_msg_ptr      = NULL;
  wms_address_s_type         dummy_address;
  sys_modem_as_id_e_type     as_id            = SYS_MODEM_AS_ID_1;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);
  
  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&dummy_address, 0x00, sizeof(wms_address_s_type));

  /* Check input parameter */
  if (gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("NULL ptr in estk_process_send_sms_req()");
    return ESTK_ERROR;
  }

  if (estk_shared_info.slot_owns_wms_cli != GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_1("WMS client is busy (0x%x)!",
                  estk_shared_info.slot_owns_wms_cli);
    return ESTK_ERROR;
  }
  else
  {
    estk_shared_info.slot_owns_wms_cli = gstk_req_ptr->hdr_cmd.sim_slot_id;
  }

  if (gstk_io_ctrl(
        GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
        &estk_shared_info.slot_owns_wms_cli,
        &as_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                  estk_shared_info.slot_owns_wms_cli);
    return ESTK_ERROR;
  }

  wms_msg_ptr = (wms_client_message_s_type *)GSTK_CALLOC(sizeof(wms_client_message_s_type));
  if(wms_msg_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  send_sms_req_ptr = &gstk_req_ptr->cmd.send_sms_pro_cmd_req;

  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == FALSE)
  {
    /* Check Alpha availability */
    /* Even in case of null data object in ALPHA (i.e. length = '00' and no value part),
       GSTK make sures at least NULL char is present in alpha text and length includes
       that NULL character. So the min length is one and hence we need to check for
       length more than one */
    if(((send_sms_req_ptr->alpha.length) > 1 && (send_sms_req_ptr->alpha.text)) ||
       (gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
           GSTK_SLOT_ID_MAX) == TRUE))
    {
      if (estk_send_alpha_to_display_client(gstk_req_ptr, TRUE) != ESTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Send SMS Alpha Display failed");
        /* priv data allocated is freed when a TR is sent or
           when a End Proactive Session is received */
        gstk_free(wms_msg_ptr);
        wms_msg_ptr = NULL;
        return ESTK_ERROR;
      }
    }
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE)
  /* system mode expected to be GSM WCDMA here */
  if( send_sms_req_ptr->is_cdma_sms == FALSE )
  {
    /*building wms structure for a UMTS SMS*/
    wms_msg_ptr->msg_hdr.message_mode      = WMS_MESSAGE_MODE_GW;
    wms_msg_ptr->msg_hdr.tag               = WMS_TAG_MO_NOT_SENT;
    wms_msg_ptr->msg_hdr.mem_store         = WMS_MEMORY_STORE_NONE;
    wms_msg_ptr->msg_hdr.index             = 0;
    wms_msg_ptr->u.gw_message.is_broadcast = FALSE;

    wms_msg_ptr->u.gw_message.sc_address.digit_mode  = WMS_DIGIT_MODE_4_BIT;
    wms_msg_ptr->u.gw_message.sc_address.number_type =
        (wms_number_type_e_type)send_sms_req_ptr->address.TON;
    wms_msg_ptr->u.gw_message.sc_address.number_plan = WMS_NUMBER_PLAN_TELEPHONY;

    wms_msg_ptr->u.gw_message.sc_address.number_of_digits =
                     send_sms_req_ptr->address.length;
    if (wms_msg_ptr->u.gw_message.sc_address.number_of_digits > 0)
    {
      (void) (void)memscpy(wms_msg_ptr->u.gw_message.sc_address.digits,
      	            sizeof(wms_msg_ptr->u.gw_message.sc_address.digits),
                    send_sms_req_ptr->address.address,
                    wms_msg_ptr->u.gw_message.sc_address.number_of_digits);
    }

    wms_msg_ptr->u.gw_message.raw_ts_data.format    = WMS_FORMAT_GW_PP;
    wms_msg_ptr->u.gw_message.raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;
    wms_msg_ptr->u.gw_message.raw_ts_data.len       = send_sms_req_ptr->sms_tpdu.length;

    UIM_MSG_HIGH_3("building wms structure for a UMTS SMS format = 0x%x, tpdu_type = 0x%x, len = 0x%x",
                   wms_msg_ptr->u.gw_message.raw_ts_data.format,
                   wms_msg_ptr->u.gw_message.raw_ts_data.tpdu_type,
                   wms_msg_ptr->u.gw_message.raw_ts_data.len);

    if (send_sms_req_ptr->sms_tpdu.length > 0)
    {
      (void) (void)memscpy(wms_msg_ptr->u.gw_message.raw_ts_data.data,
      	            sizeof(wms_msg_ptr->u.gw_message.raw_ts_data.data),
                    send_sms_req_ptr->sms_tpdu.tpdu,
                    send_sms_req_ptr->sms_tpdu.length);
    }
    else
    {
      UIM_MSG_ERR_0("tpdu len is 0!");
      gstk_free(wms_msg_ptr);
      wms_msg_ptr = NULL;
      return ESTK_ERROR;
    }

    wms_msg_ptr->u.gw_message.raw_ts_data.format    = WMS_FORMAT_GW_PP;
    wms_msg_ptr->u.gw_message.raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;

    if(send_sms_req_ptr->packing_required)
    {
      /* decode and pack the user data */
      /* find the DCS byte */
      dcs_pos += estk_ts_decode_address(&(wms_msg_ptr->u.gw_message.raw_ts_data.data[dcs_pos]),
                                        &dummy_address);
      dcs_pos++;
      if(dcs_pos < WMS_MAX_LEN && dcs_pos < send_sms_req_ptr->sms_tpdu.length)
      {
        UIM_MSG_HIGH_1("Packing Required, dcs = 0x%x",
                       wms_msg_ptr->u.gw_message.raw_ts_data.data[dcs_pos]);
        if(wms_msg_ptr->u.gw_message.raw_ts_data.data[dcs_pos] !=  0x08 ) /* i.e not UCS2_DCS */
        {
          /* check if DCS is 8-bit data */
          if (wms_msg_ptr->u.gw_message.raw_ts_data.data[dcs_pos] & 0x04)
          {
            /* reset DCS 8-bit data bit - to be 7-bit default now */
            wms_msg_ptr->u.gw_message.raw_ts_data.data[dcs_pos] &= 0xFB;
          }

          if (WMS_OK_S != wms_ts_pack_gw_user_data(
                            &wms_msg_ptr->u.gw_message.raw_ts_data))
          {
            UIM_MSG_ERR_0("Packing Message failed");
            gstk_free(wms_msg_ptr);
            wms_msg_ptr = NULL;
            return ESTK_ERROR;
          }
        }
      }
      else
      {
        UIM_MSG_ERR_1("Packing failed, dcs_pos is 0x%x", dcs_pos);
        gstk_free(wms_msg_ptr);
        wms_msg_ptr = NULL;
        return ESTK_ERROR;
      }
    }
  } /* if !cdma_sms */
#endif /*F_WCDMA || GSM || LTE*/

  if( send_sms_req_ptr->is_cdma_sms == TRUE )
  {
    UIM_MSG_HIGH_0("building wms structure for a CDMA SMS ");

    if(wms_ts_cdma_OTA2cl(send_sms_req_ptr->sms_tpdu.tpdu,
                          (uint16)send_sms_req_ptr->sms_tpdu.length,
                          wms_msg_ptr) != WMS_OK_S)
    {
      UIM_MSG_ERR_0("Conversion from OTA to cl failed");
      gstk_free(wms_msg_ptr);
      wms_msg_ptr = NULL;
      return ESTK_ERROR;
    }
#ifdef FEATURE_CDMA
    else if(send_sms_req_ptr->packing_required)
    {
      /* OTA conversion to WMS format ok, pack if necessary */
      if(wms_ts_pack_cdma_user_data(
           &wms_msg_ptr->u.cdma_message.raw_ts) != WMS_OK_S)
      {
        UIM_MSG_ERR_0("Packing Message failed");
        gstk_free(wms_msg_ptr);
        wms_msg_ptr = NULL;
        return ESTK_ERROR;
      }
    }
#endif /* FEATURE_CDMA */
  } /* if cdma_sms */

  UIM_MSG_HIGH_3("gw ready: 0x%x, cdma ready: 0x%x, wms ptr: 0x%x",
                  estk_curr_inst_ptr->wms_msg_cfg.wms_ready_gw,
                  estk_curr_inst_ptr->wms_msg_cfg.wms_ready_cdma,
                  wms_msg_ptr);

  if((wms_msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW &&
      estk_curr_inst_ptr->wms_msg_cfg.wms_ready_gw) ||
     (wms_msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA &&
      estk_curr_inst_ptr->wms_msg_cfg.wms_ready_cdma))
  {
    wms_status = wms_msg_ms_send(
      estk_shared_info.wms_client_id,
      as_id,
      (wms_cmd_cb_type)estk_wms_cmd_cb,
      (const void*)estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
      WMS_SEND_MODE_CLIENT_MESSAGE,
      wms_msg_ptr);
  
    gstk_free(wms_msg_ptr);
    wms_msg_ptr = NULL;
    estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr = NULL;
  
    if (wms_status != WMS_OK_S)
    {
      UIM_MSG_ERR_1("wms_msg_send failed 0x%x", wms_status);
      return ESTK_ERROR;
    }
  }
  else
  {
    /* WMS is not ready cache the wms_msg */
    estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr = wms_msg_ptr;
    wms_msg_ptr = NULL;
  }
  return ESTK_SUCCESS;
} /* estk_process_send_sms_req */

#endif /* FEATURE_ESTK */
