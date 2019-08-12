/*===========================================================================


                    G S T K     N O     S I M


GENERAL DESCRIPTION
  This source file contains function to handle event for
  GSTK_NO_SIM_S state.


FUNCTIONS
  gstk_state_no_sim
    This function handles events/commands/signals for GSTK_NO_SIM_S


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2006,2008-2011, 2013-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_s_no_sim.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/17/16   vr      F3 reduction
08/27/15   vr      Moved the logic to reset cache in respective event handlers
11/20/14   gm      Reset gstk sub-state after card error
09/25/14   gm      Added support to send failure indication for OTASP
09/14/14   dy      Adding handling for GSTK_CM_PH_EVENT_DUAL_STANDBY_PREF
                   in non-idle GSTK states
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/11/14   shr     Add Support for NV Refresh
03/03/14   shr     ESTK BIP clean-up and enhancements
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/29/13   vr      Handle envelope response in no sim state
11/13/13   shr     Added support for BIP over WLAN
10/04/13   sw      Reduced F3 messages
08/28/13   yt      Deprecate VIRTUIM/VIRTSAT module
07/24/13   sw      Executing location env rsp rcvd confirmation in gstk context
07/03/13   kb      Remove write of tp_client when feature is disabled
06/20/13   vr      Triton TSTS changes
06/19/13   hh      Correctly handle GSTK_LOCATION_INFORMATION_IND in all GSTK states
05/08/13   xz      Set gstk_mm_param_received to true only after SETUP EVT
                   LIST recieved
05/04/13   hh      Reset gstk_evt_list in GSTK_S_NO_SIM
05/01/13   vr      Fix command processing logic in GSTK
04/02/13   av      Move UIM tasks to group4 of RCINIT
02/25/13   xz      Process attach ctrl req when envelope is in retry
01/08/13   hn      Reset global for VPE in NO_SIM state
09/20/12   yt      Manage concurrency between BIP and OTASP
06/29/12   sg      Remove updating global for expected tp as it fails tp
06/21/12   sg      Handle timer expiry during hotswap
12/20/11   nb      GSDI cleanup
09/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
09/15/11   xz      Allow GSTK_TERMINAL_PROFILE_CMD in NO SIM state
08/24/11   xz      Fix compiler warnings and LINT/KW errors
06/20/11   js      Fix issue with uim recovery on fusion
05/14/11   bd      Removed redunant line of code
05/11/11   nb      Support for Async TP
05/03/11   xz      Add cleanup when state is not idle
04/05/11   xz      Changes to support VPE ATTACH CTRL
01/16/11   sg      Clean up globals for envelopes and TR commands
11/21/10   ssr     Fixed TP for NO SIM state
09/09/10   dd      Clear the command table in No Sim State if TR is received
06/18/10   xz      Moving num of expected TPs to config file
02/15/10   nb      Moved Dual Slot Changes
09/26/09   kp      ZI memory reduction changes
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
04/10/09   ssr     Fixed limited Length TERMINAL PROFILE for non-spec
                   compliant cards
10/22/08   sk      Handle client id reg and proactive command reg
09/08/08   sk      Fixed indentation issues
09/02/08   sk      Fixed error message
08/25/08   xz      Start a timer and wait for all expected TP download
08/06/08   xz      Remove gstk_profile_received and send term profile as soon
                   as GSTK gets GSDI_OK_FOR_TERMINAL_PROFILE_DL
10/23/06   sk      Fixed lint errors.
08/25/06   sk      Featurized uim recovery
07/25/06   sk      Added support to handle envelope cmds in non idle state
07/12/06   sk      During UIM recovery, if TP dl was not done prior to
                   receiving this cmd, return TP dl done to GSDI
06/24/06   sk      Lint fixes
06/13/06   sk      UIM recovery support
06/09/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/18/05   tml     Added support for OK and NOT OK for TP
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
05/11/05   sst     Lint fixes
11/23/04   jar     Changed GSTK_START_s to GSTK_START_S to fix Dual Slot
                   compile problem.
08/30/04   tml     Go to Start State if gstk_profile_received is set to FALSE
06/08/04   tml     Added support for timer expiration, browser term and lang
                   selection envelope support
06/07/04   tml     Returned envelope response in NO SIM state
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
09/04/03   tml     Dual slot support
05/26/03   tml     linted
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stringl/stringl.h>
#include "intconv.h"
#include "gstk_state.h"
#include "gstkutil.h"
#include "gstk_term_profile.h"
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"

#include "err.h"
#include "mmgsdilib_p.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_state_no_sim

DESCRIPTION

  This function handles events/commands/signal for GSTK_NO_SIM_S
  GSTK_NO_SIM_S may receive any of the following events:
  - sim valid from GSDI -> transition to GSTK_MMGSDI_SIG_WAIT_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_ptr: [Input] Pointer to the command to be processed in this state

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
gstk_status_enum_type gstk_state_no_sim (
  gstk_cmd_type  * cmd_ptr )
{
  gstk_status_enum_type   gstk_status       = GSTK_SUCCESS;
  uint8                   i                 = 0;
  gstk_cmd_from_card_type response;
  gstk_client_type        client_type       = GSTK_NO_TYPE;
  uint32                  num_expected_tp   = 1;
  uint32                  tp_clients        = 0;
  gstk_nv_items_data_type nv_data;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  gstk_curr_inst_ptr->otasp_blocked = FALSE;
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);

  if(cmd_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  (void)secure_memset(gstk_curr_inst_ptr->gstk_evt_list,
                      0x00,
                      sizeof(gstk_curr_inst_ptr->gstk_evt_list));

  memset(&response, 0x00, sizeof(gstk_cmd_from_card_type));

  /* Free GSTK_LOCATION_INFORMATION_IND entries in envelope_cmd_ref_table */
  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++)
  {
    if (GSTK_LOC_INFO_ENVELOPE_RSP ==
        gstk_shared_data.envelope_cmd_ref_table[i].expected_cmd_rsp_type)
    {
      gstk_util_env_cmd_ref_table_free_slot(i);
    }
  }

  if (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
        gstk_curr_inst_ptr->slot_id) == TRUE)
  {
    /* set to TRUE because there won't be TP DL in progress */
    gstk_curr_inst_ptr->is_tp_dl_done = TRUE;
  }

  gstk_curr_inst_ptr->setup_evt_list_cnt = 0;

  UIM_MSG_HIGH_1("Command group=%x",
                 cmd_ptr->general_cmd.message_header.command_group);

  switch( cmd_ptr->general_cmd.message_header.command_group )
  {
    case GSTK_MMGSDI_CMD:
      UIM_MSG_HIGH_1("Received MMGSDI, %x",
                     cmd_ptr->mmgsdi_evt.message_header.command_id);
      switch(cmd_ptr->mmgsdi_evt.message_header.command_id)
      {
        case GSTK_MMGSDI_TERMINAL_PROFILE_DL_EVT:
          if(cmd_ptr->mmgsdi_evt.evt.terminal_profile.ok_dl)
          {
            if(gstk_curr_inst_ptr->gstk_num_of_tp_dl_received == gstk_shared_data.num_of_expected_tp)
            {
              /* use the gstk_terminal_profile_cache info and send it to uim */
              gstk_status = gstk_pack_tp_cmd_to_uim();
              if (gstk_status == GSTK_SUCCESS)
              {
                gstk_curr_inst_ptr->tp_info.user_data = cmd_ptr->mmgsdi_evt.message_header.user_data;
                gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb = cmd_ptr->mmgsdi_evt.client_cb;
                /* go to wait for TP response state */
                gstk_curr_inst_ptr->gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
              }
              else
              {
                UIM_MSG_ERR_1("Unable to send TP to card in state 0x%x",
                              gstk_curr_inst_ptr->gstk_state);
                gstk_curr_inst_ptr->gstk_state        = GSTK_NO_SIM_S;
                response.hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
                response.hdr_cmd.sim_slot_id          = cmd_ptr->mmgsdi_evt.message_header.sim_slot_id;
                response.hdr_cmd.cmd_detail_reference = 0;
                response.hdr_cmd.command_number       = 0;
                response.hdr_cmd.user_data            = cmd_ptr->mmgsdi_evt.message_header.user_data;
                if(cmd_ptr->mmgsdi_evt.client_cb != NULL)
                {
                  (cmd_ptr->mmgsdi_evt.client_cb)(&response);
                }
              }
            }
            else
            {
              gstk_curr_inst_ptr->tp_info.user_data = cmd_ptr->mmgsdi_evt.message_header.user_data;
              gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb = cmd_ptr->mmgsdi_evt.client_cb;
              /* Lets Share the Timer with Async TP also, at worst case TP for UICC will get
              be delayed by TIMER TIME + TP indication for ICC(from other slot) */
              (void)rex_set_timer(&gstk_shared_data.gstk_tp_wait_timer,
                                  GSTK_TP_CMD_WAIT_TIME);
              gstk_curr_inst_ptr->gstk_state = GSTK_TERM_PROFILE_WAIT_S;
            }
          }
          else
          {
            gstk_curr_inst_ptr->gstk_state        = GSTK_NO_SIM_S;
            response.hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
            response.hdr_cmd.sim_slot_id          = cmd_ptr->mmgsdi_evt.message_header.sim_slot_id;
            response.hdr_cmd.cmd_detail_reference = 0;
            response.hdr_cmd.command_number 	  = 0;
            response.hdr_cmd.user_data            = cmd_ptr->mmgsdi_evt.message_header.user_data;
            if(cmd_ptr->mmgsdi_evt.client_cb != NULL)
            {
              (cmd_ptr->mmgsdi_evt.client_cb)(&response);
            }
          }
	  break;

        case GSTK_MMGSDI_CARD_INSERTED_EVT:
          if(gstk_curr_inst_ptr->gstk_num_of_tp_dl_received == gstk_shared_data.num_of_expected_tp)
          {
            gstk_curr_inst_ptr->gstk_state = GSTK_MMGSDI_SIG_WAIT_S;
          }
          else
          {
            gstk_curr_inst_ptr->gstk_state = GSTK_START_S;
          }
          break; /* end - case GSTK_MMGSDI_CARD_INSERTED_EVT */

        case GSTK_MMGSDI_LOC_ENV_RSP_RCVD_CNF_EVT:
          UIM_MSG_HIGH_1("Received Location Envelope Rsp Rcvd Confirmation cmd: imsi_switched=0x%x",
                         cmd_ptr->loc_env_rsp_rcvd_cnf.imsi_switched);
          if(gstk_nv_get_feature_status(
               GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
               gstk_curr_inst_ptr->slot_id) == TRUE)
          {
            gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
            gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
            gstk_util_send_attach_ctrl_res();
          }
          break;

        case GSTK_MMGSDI_CARD_ERROR_EVT:
          gstk_curr_inst_ptr->apps_enabled = GSTK_APP_BMSK_NONE;
          gstk_curr_inst_ptr->card_protocol = MMGSDI_NO_PROTOCOL;
          gstk_curr_inst_ptr->is_tp_rsp_received = FALSE;
          if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
          {
            gstk_curr_inst_ptr->gstk_sub_state   = GSTK_INVALID_SUB_S;
          }
        break;

        default:
          /* GSDI_SIM_INIT_COMPLETED should be ignored by GSTK_NO_SIM_S
             since if we receive this event, we should have already passed
             any window of opportunities for supporting (U)SAT functions */
          break; /* ignore all other MMGSDI events */
      }
      break;

  case GSTK_ENVELOPE_CMD:
    gstk_status = gstk_process_non_idle_state_envelope_cmd(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Envelope Cmd ERR, %x", gstk_status);
    }
    rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
    if(cmd_ptr->general_cmd.message_header.command_id == GSTK_DATA_AVAIL_EVT_IND
       || cmd_ptr->general_cmd.message_header.command_id == GSTK_CH_STATUS_EVT_IND
       || cmd_ptr->general_cmd.message_header.command_id == GSTK_CC_IND)
    {
      UIM_MSG_HIGH_1("BIP env in progress is %d",
                     gstk_curr_inst_ptr->gstk_bip_env_in_progress);
      gstk_curr_inst_ptr->gstk_bip_env_in_progress = FALSE;
    }
    rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
    break;

  case GSTK_STOP_CMD:
    /* flush GSTK queue, clean up and ack tmc */
    gstk_status = gstk_power_down();
    if (gstk_status == GSTK_SUCCESS)
    {
      gstk_curr_inst_ptr->gstk_state = GSTK_POWER_DOWN_S;
    }
    else
    {
      UIM_MSG_HIGH_0("Power Down err");
    }
    break;

  case GSTK_MM_IDLE_CMD:
    /*Ignore MM IDLE CMD*/
    /* gstk_curr_inst_ptr->gstk_mm_state_is_idle flag is used  as an indication to start queuing RRC messages */
    gstk_curr_inst_ptr->gstk_mm_state_is_idle = TRUE;
    break;

  case GSTK_CLIENT_REG_REQ_CMD:
    (void)gstk_process_client_reg_req(cmd_ptr);
    break;

  case GSTK_TERMINAL_RSP:
    for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++)
    {
      gstk_util_cmd_details_ref_table_free_slot(i);
    }
    UIM_MSG_HIGH_2("CC Env count is %d proactive cmd in progress flag is %d",
                   gstk_curr_inst_ptr->gstk_block_cc_env,
                   gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress);
    if(cmd_ptr->general_cmd.message_header.command_id == GSTK_SEND_DATA_CNF
       || cmd_ptr->general_cmd.message_header.command_id == GSTK_RECEIVE_DATA_CNF)
    {
      /* Enter gstk critical section */
      rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
      if(gstk_curr_inst_ptr->gstk_block_cc_env > 0)
      {
        UIM_MSG_HIGH_1("gstk_block_cc_env is positive %d",
                       gstk_curr_inst_ptr->gstk_block_cc_env);
        gstk_curr_inst_ptr->gstk_block_cc_env--;
      }
      /* Leave gstk critical section */
      rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
    }
    break;

  case GSTK_TERMINAL_PROFILE_CMD:
    gstk_curr_inst_ptr->gstk_num_of_tp_dl_received ++;
    UIM_MSG_HIGH_1("num of tp dl received is 0x%x",
                   gstk_curr_inst_ptr->gstk_num_of_tp_dl_received);
    gstk_status = gstk_cache_tp_info(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS)
    {
      gstk_util_notify_tp_dl_fail_specific_client(
        cmd_ptr->terminal_profile_cmd.message_header.client_id);
    }
    break;

  case GSTK_TIMER_EXP_CMD:
    switch (cmd_ptr->timer_msg.message_header.command_id)
    {
      case GSTK_ATTACH_CTRL_TIMER:
        gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_TIMER_EXP;
        gstk_util_send_attach_ctrl_res();
        break;

      case GSTK_CLIENTS_REG_TIMER:
        /* save clients that do TP DL to config data */
        tp_clients = 0;

        if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_DYNAMIC_TP_CLIENTS, GSTK_SLOT_ID_MAX) == TRUE)
        {
          for (i = 0; i < GSTK_MAX_CLIENT; ++i)
          {
            client_type = gstk_shared_data.gstk_client_table[i].client_type;
            if (client_type == GSTK_ESTK_TYPE || client_type == GSTK_UI_TYPE)
            {
              tp_clients |= 0x00000001 << (uint8)client_type;
              ++ num_expected_tp;
            }
          }
          UIM_MSG_HIGH_2("num_expected_tp is 0x%x, gstk_shared_data.num_of_expected_tp is 0x%x",
                         num_expected_tp,
                         gstk_shared_data.num_of_expected_tp);
          if(num_expected_tp > gstk_shared_data.num_of_expected_tp)
          {
            memset(&nv_data, 0x00, sizeof(nv_data));

            nv_data.tp_clients = tp_clients;

            /* read all clients that will do TP DL */
            (void)gstk_nv_access_write(
                    GSTK_NV_ME_TP_CLIENTS,
                    GSTK_SLOT_ID_MAX,
                    &nv_data);
          }
          else
          {
            UIM_MSG_HIGH_0("Will not write tp_clients file since there are less clients");
          }
        }
        break;
      case GSTK_TP_TIMER:
        gstk_status = gstk_pack_tp_cmd_to_uim();
        if (gstk_status == GSTK_SUCCESS)
        {
          /* go to wait for TP response state */
          gstk_curr_inst_ptr->gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
        }
        else
        {
          memset(&response, 0x00, sizeof(gstk_cmd_from_card_type));
          UIM_MSG_ERR_0("Send TP error");
          gstk_curr_inst_ptr->gstk_state = GSTK_NO_SIM_S;

          response.hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
          response.hdr_cmd.sim_slot_id          = cmd_ptr->general_cmd.message_header.sim_slot_id;
          response.hdr_cmd.cmd_detail_reference = 0;
          response.hdr_cmd.command_number       = 0;
          response.hdr_cmd.user_data            = gstk_curr_inst_ptr->tp_info.user_data;
          if(gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb != NULL)
          {
            (gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb)(&response);
          }
        }

        uim_set_proactive_uim_slot(FALSE, (uim_slot_type)cmd_ptr->timer_msg.message_header.sim_slot_id);
        break;
     }
     break;

  case GSTK_DS_EVT_CMD:
    gstk_process_ds_evt_cmd(cmd_ptr);
    break;

  case GSTK_ENVELOPE_RSP:
    (void)gstk_process_envelope_rsp(cmd_ptr);
    break;

  case GSTK_CM_PH_IND:
    switch (cmd_ptr->cm_ph_ind.cm_ph_event)
    {
      case GSTK_CM_PH_EVENT_DUAL_STANDBY_PREF:
        gstk_shared_data.active_data_subs = cmd_ptr->cm_ph_ind.active_data_subs;
        break;
      default:
        UIM_MSG_HIGH_1("CM ph evt 0x%x not handled in current GSTK state",
                       cmd_ptr->cm_ph_ind.cm_ph_event);
        break;
    }
    break;

  case GSTK_OTASP_REG_CMD:
    gstk_process_otasp_reg_cmd(cmd_ptr);
    break;  

  default:
    UIM_MSG_HIGH_1("Cmd 0x%x Not handled in NO_SIM_S",
                   cmd_ptr->general_cmd.message_header.command_group);
    gstk_util_send_err_to_client(cmd_ptr);
    break;
  }

  return gstk_status;
} /*gstk_state_no_sim*/

