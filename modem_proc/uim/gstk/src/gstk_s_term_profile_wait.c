#include "uim_variation.h"
#ifndef GSTK_S_TERM_PROFILE_WAIT_C
#define GSTK_S_TERM_PROFILE_WAIT_C


/*===========================================================================


            G S T K    T E R M I N A L    P R O F I L E    W A I T


GENERAL DESCRIPTION
  This source file contains function to handle event for
  GSTK_TERM_PROFILE_WAIT_S state.


FUNCTIONS
  gstk_state_tp_wait
    This function handles events/commands/signals for
    GSTK_TERM_PROFILE_WAIT_S


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2006,2008-2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_s_term_profile_wait.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/17/16   vr      F3 reduction
09/25/14   gm      Added support to send failure indication for OTASP
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info 
09/14/14   dy      Adding handling for GSTK_CM_PH_EVENT_DUAL_STANDBY_PREF
                   in non-idle GSTK states
09/09/14   vr      Reduce stack usage
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/10/14   dy      Adding handling for session changed in non-idle states
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
05/06/14   gs      GSTK Featurization enhancements and clean up
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
03/03/14   shr     ESTK BIP clean-up and enhancements
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
12/16/13   bd      Added support for new CM SS event handling
11/13/13   shr     Added support for BIP over WLAN
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
08/28/13   yt      Deprecate VIRTUIM/VIRTSAT module
07/03/13   kb      Remove write of tp_client when feature is disabled
06/20/13   vr      Triton TSTS changes
05/01/13   vr      Fix command processing logic in GSTK
04/26/13   gm      Not allowing fetch before TP download
04/02/13   av      Move UIM tasks to group4 of RCINIT
02/08/13   bd      Support for (e)HRPD access tech
10/18/12   vr      reset the GSTK current instace ptr's apps_enabled, when
                   there is a GSTK_MMGSDI_CARD_ERROR_EVT
06/27/12   sg      Update user data to notify gsdi of tp's most recent value
03/28/12   av      Replaced feature flags with NVITEM
12/21/11   kk      More GSDI Cleanup
12/20/11   nb      GSDI cleanup
12/13/11   dd      Do not overwrite tp_clients if less clients are found
09/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
06/20/11   js      Fix issue with uim recovery on fusion
03/15/11   nb      Support for Async TP
03/11/11   xz      Reset TP client content
02/07/11   dd      Fix dual slot define
01/31/11   ssr     Update code to stop UIM polling on current slot
01/26/11   xz      Add support of HRPD access tech
10/05/10   bd      Fixed issue of TP DL
06/18/10   xz      Moving num of expected TPs to config file
04/07/10   yt      Fixed compilation warnings
02/15/10   nb      Moved Dual Slot Changes
10/22/09   bd      Set a signal when event GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL
                   is received from GSDI
09/26/09   kp      ZI memory reduction changes
07/28/09   bd      Fix lint issues
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id
                   when parse client TP and simplify TP download procedure
09/08/08   sk      Fixed indentation issues
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
                   Fixed error message
08/21/08   sk      Added changes to support enhanced GSTK registration
08/08/08   xz      Added support for raw format registration
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
08/25/06   sk      Featurized uim recovery
07/25/06   sk      Added support to handle envelope cmds in non idle state
                   Added support to send basic tp dl when GSTK times out on UI
07/12/06   sk      During UIM recovery, if TP dl was not done prior to
                   receiving this cmd, return TP dl done to GSDI
06/13/06   sk      UIM recovery support
04/04/06   sk      Added support for 1X related to location information
11/09/05   sk      Fixed Lint Errors
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
08/23/05   tml     Fixed state transition issue
07/29/05   sk      Fixed compilation errors.
01/07/05   jk      Fixed Compiler warnings
10/01/04   tml     Change function prototype for terminal profile caching
04/02/04   tml     linted
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
02/04/04 jar/tml   Turn off proactive UIM Bits if timeout from client TP DL.
09/04/03   tml     Dual slot support
05/16/03   tml     Linted
03/21/03   tml     Reflected gsdi api changes
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_state.h"
#include "gstk_term_profile.h"
#include "gstk_envelope_cmd.h"

#include "err.h"
#include "gstkutil.h"
#include "mmgsdilib_p.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_state_tp_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_TERM_PROFILE_WAIT_S
  GSTK_TERM_PROFILE_WAIT_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile info from client -> transition to GSTK_TERM_PROFILE_RSP_WAIT_S
  - timer expiration from GSTK -> transition to GSTK_NO_SIM_S
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
gstk_status_enum_type gstk_state_tp_wait (
  gstk_cmd_type            * cmd_ptr )
{
  gstk_status_enum_type    gstk_status       = GSTK_SUCCESS;
  uint8                    i                 = 0;
  gstk_client_type         client_type       = GSTK_NO_TYPE;
  gstk_cmd_from_card_type  response;
  uint32                   num_expected_tp   = 1;
  uint32                   tp_clients        = 0;
  gstk_nv_items_data_type  nv_data;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(cmd_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }
  UIM_MSG_HIGH_1("Commnd group=%x",
                 cmd_ptr->general_cmd.message_header.command_group);
  switch( cmd_ptr->general_cmd.message_header.command_group ) {
  case GSTK_RECOVERY_CMD:
  UIM_MSG_HIGH_2("Received Recovery cmd, %x, gstk_sub_state %x",
                 cmd_ptr->general_cmd.message_header.command_id,
                 gstk_curr_inst_ptr->gstk_sub_state);
  switch(cmd_ptr->general_cmd.message_header.command_id)
  {
    case GSTK_RECOVERY_START_CMD:
      gstk_curr_inst_ptr->gstk_sub_state = GSTK_RECOVERY_SUB_S;
      break;
    case GSTK_RECOVERY_END_CMD:
      gstk_curr_inst_ptr->gstk_sub_state = GSTK_INVALID_SUB_S;
      break;
    default:
      break;
  }
  break;


  case GSTK_CLIENT_REG_REQ_CMD:
    (void)gstk_process_client_reg_req(cmd_ptr);
    break;

  case GSTK_TERMINAL_PROFILE_CMD:
    /* received command from client before timer expiration */
    gstk_curr_inst_ptr->gstk_num_of_tp_dl_received ++;
    gstk_status = gstk_cache_tp_info(cmd_ptr);
    if (gstk_status == GSTK_SUCCESS)
    {
      /* use the gstk_terminal_profile_cache info and send it to uim */
      UIM_MSG_HIGH_1("num of tp dl received is 0x%x",
                     gstk_curr_inst_ptr->gstk_num_of_tp_dl_received);
      if(gstk_curr_inst_ptr->gstk_num_of_tp_dl_received ==
         gstk_shared_data.num_of_expected_tp)
      {
        gstk_status = gstk_pack_tp_cmd_to_uim();
        if (gstk_status == GSTK_SUCCESS)
        {
          /* go to wait for TP response state */
          gstk_curr_inst_ptr->gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
        }
        else
        {
          UIM_MSG_ERR_0("Send TP error");
        }
      }
    }
    else
    {
      UIM_MSG_ERR_0("Cache TP Error");
      gstk_util_notify_tp_dl_fail_specific_client(cmd_ptr->terminal_profile_cmd.message_header.client_id);
    }
    break; /* end - case GSTK_TERMINAL_PROFILE_CMD */

  case GSTK_MMGSDI_CMD:
    UIM_MSG_HIGH_1("Received MMGSDI, 0x%x",
                   cmd_ptr->mmgsdi_evt.message_header.command_id);
    switch(cmd_ptr->mmgsdi_evt.message_header.command_id)
    {
      case GSTK_MMGSDI_CARD_ERROR_EVT:
        gstk_curr_inst_ptr->gstk_state   = GSTK_NO_SIM_S;
        gstk_curr_inst_ptr->apps_enabled = GSTK_APP_BMSK_NONE;
        gstk_curr_inst_ptr->is_tp_rsp_received = FALSE;
        if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
        {
          gstk_curr_inst_ptr->gstk_sub_state   = GSTK_INVALID_SUB_S;
        }
        break;

      case GSTK_MMGSDI_TERMINAL_PROFILE_DL_EVT:
        if(!cmd_ptr->mmgsdi_evt.evt.terminal_profile.ok_dl)
        {
          memset(&response, 0x00, sizeof(gstk_cmd_from_card_type));
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
        else
        {
          /* Update the latest values from gsdi here to update gsdi when cb is called */
          gstk_curr_inst_ptr->tp_info.user_data = cmd_ptr->mmgsdi_evt.message_header.user_data;
          gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb = cmd_ptr->mmgsdi_evt.client_cb;
        }
        break;

      case GSTK_MMGSDI_SESSION_CHANGED_EVT:
        gstk_process_session_changed_evt(cmd_ptr);
        break;

      default:
        break; /* ignore all other MMGSDI Events */
    }
    break;

  case GSTK_TIMER_EXP_CMD:
    UIM_MSG_HIGH_1("Received Timer Exp, %x",
                   cmd_ptr->timer_msg.message_header.command_id);
    switch (cmd_ptr->timer_msg.message_header.command_id)
    {
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
      default:
          UIM_MSG_ERR_0("Invalid timer expiry in this state");
          break;
    }
    break;

  case GSTK_STOP_CMD:
    /*Received STOP Sig*/
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
    /* gstk_curr_inst_ptr->gstk_mm_state_is_idle flag is used  as an indication
       to start queuing RRC messages */
    gstk_curr_inst_ptr->gstk_mm_state_is_idle = TRUE;
    break;

  case GSTK_MODE_CHANGE_IND:
#ifdef FEATURE_CDMA
    (void)gstk_util_merge_cm_stacks(&gstk_curr_inst_ptr->cdma_ext_loci_cache, &cmd_ptr->mode_change_ind);
    /* Update bmsk_3gpp2 to latest information */
    gstk_curr_inst_ptr->bmsk_3gpp2 = 0;
    gstk_process_mode_change_update_3gpp2_bmsk(&(cmd_ptr->mode_change_ind));
#else  /* #ifdef FEATURE_CDMA */
    UIM_MSG_HIGH_0("GSTK_MODE_CHANGE_IND : Feature CDMA not supported");
#endif /*#ifdef FEATURE_CDMA */
    break;

  case GSTK_ENVELOPE_CMD:
    gstk_status = gstk_process_non_idle_state_envelope_cmd(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Envelope Cmd ERR, %x", gstk_status);
    }
    break;

  case GSTK_DS_EVT_CMD:
    gstk_process_ds_evt_cmd(cmd_ptr);
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
    UIM_MSG_HIGH_1("Cmd 0x%x Not handled in T_P_WAIT_S",
                   cmd_ptr->general_cmd.message_header.command_group);
    break;
  }

  if (gstk_curr_inst_ptr->gstk_state != GSTK_TERM_PROFILE_WAIT_S)
  {
    /* cancel the timer since we are changing state now */
    (void) rex_clr_timer(&gstk_shared_data.gstk_tp_wait_timer);
  }

  return gstk_status;
} /*gstk_state_tp_wait*/


#endif  /* GSTK_S_TERM_PROFILE_WAIT_C */
/*lint -e750 local macro gstk_c not being used */
