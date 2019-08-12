#include "uim_variation.h"
#ifndef GSTK_S_TERM_PROFILE_RSP_WAIT_C
#define GSTK_S_TERM_PROFILE_RSP_WAIT_C


/*===========================================================================


   G S T K   T E R M I N A L   P R O F I L E   R E S P O N S E   W A I T


GENERAL DESCRIPTION
  This source file contains function to handle event for
  GSTK_TERM_PROFILE_RSP_WAIT_S state.


FUNCTIONS
  gstk_state_tp_rsp_wait
    This function handles events/commands/signal for
    GSTK_TERM_PROFILE_RSP_WAIT_S


INITIALIZATION AND SEQUENCING REQUIREMENTS


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_s_term_profile_rsp_wait.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/17/16   vr      F3 reduction
09/25/14   gm      Added support to send failure indication for OTASP
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info 
09/15/14   sw      Defining gstk_env_retry_timer during gstk initialization
09/14/14   dy      Adding handling for GSTK_CM_PH_EVENT_DUAL_STANDBY_PREF
                   in non-idle GSTK states
09/09/14   vr      Reduce stack usage
09/02/14   dy      Adding handling for session changed in non-idle states
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
07/11/14   bd      Enable retry for call control envelopes by default
06/18/14   sw      Call/SMS Control retry
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
06/20/13   vr      Triton TSTS changes
05/01/13   vr      Fix command processing logic in GSTK
04/26/13   gm      Not allowing fetch before TP download
02/11/13   bd      Support for (e)HRPD access tech
10/18/12   vr      reset the GSTK current instace ptr's apps_enabled, when
                   there is a GSTK_MMGSDI_CARD_ERROR_EVT
03/28/12   av      Replaced feature flags with NVITEM
03/20/12   dd      Delay VPE if sent before TP
12/20/11   nb      GSDI cleanup
09/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
08/24/11   xz      Fix compiler warnings and LINT/KW errors
06/20/11   js      Fix issue with uim recovery on fusion
06/08/11   nb      Read envelope retry NV item upfront ot avoid probable
                   deadlock between NV, GSTK and GSDI
05/13/11   xz      Fix compile error
05/12/11   xz      Fix issue of not notify TP DL complete when TP DL fails
05/11/11   nb      Support for Async TP
03/24/11   sg      Enable retry for Location Info propreitary envelope
02/07/11   dd      Fix dual slot define
01/31/11   ssr     Update code to stop UIM polling on current slot
01/26/11   xz      Add support of HRPD access tech
12/17/10   xz      Decouple feature bitmask
09/13/10   xz      Add slot ID in terminal profile response
06/21/10   xz      Use correct feature mask
06/18/10   xz      Moving num of expected TPs to config file
05/10/10   sg      Unblock mmgsdi on early loc info timer expiry
03/19/10   sg      Clear timer if for location information if running
03/08/10   sg      Location information envelope command
02/15/10   nb      Moved Dual Slot Changes
09/26/09   kp      ZI memory reduction changes
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
10/28/08   xz      Fixed wrong debug message
10/22/08   sk      Handle client id reg and proactive command reg
09/14/08   sk      Fixed lint errors
09/08/08   sk      Fixed indentation issues
09/02/08   sk      Fixed error message
05/06/08   sk      Added support for event download envelope retry
06/11/07   jk      Revert changes from previous fix as they are not necessary,
                   and introduce possiblity of FATAL ERROR if TP-DL delayed
02/13/07   pv      If GSDI_INTERNAL_SIM_RESET is received even when waiting
                   for a terminal response, do a TP-DL again.
08/25/06   sk      Featurized uim recovery
07/25/06   sk      Added support to handle envelope cmds in non idle state
07/12/06   sk      During UIM recovery, if TP dl was not done prior to
                   receiving this cmd, return TP dl done to GSDI
06/14/06   sk      Removed warnings
06/13/06   sk      UIM recovery support
06/09/06   sk      Lint fixes
04/04/06   sk      Added support for 1X related to location information
11/09/05   sk      Fixed Lint Errors
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
07/29/05   sk      Fixed compilation errors.
04/02/04   tml     linted
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
02/19/04   tml     compilation fix
02/04/04 jar/tml   Turn off Proactive UIM Bit for failed terminal profile dl.
09/04/03   tml     Dual slot support
07/07/03   tml     Changed message to capture more meaningful info
05/16/03   tml     linted
03/21/03   tml     Reflect API change in gsdi
02/07/03   tml     Initial Version



===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_state.h"
#include "gstkutil.h"
#include "gstk_term_profile.h"
#include "gstk_envelope_cmd.h"
#include "gstk.h"

#include "err.h"
#include "mmgsdilib_p.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_state_tp_rsp_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_TERM_PROFILE_RSP_WAIT_S
  GSTK_TERM_PROFILE_RSP_WAIT_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile response from uim -> transition to GSTK_IDLE_S/GSTK_NO_SIM_S
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
gstk_status_enum_type gstk_state_tp_rsp_wait (
  gstk_cmd_type            * cmd_ptr )
{
  gstk_status_enum_type   gstk_status           = GSTK_SUCCESS;
  gstk_cmd_from_card_type terminal_profile_rsp;
  uint8                   default_plmn[GSTK_MAX_PLMN_LEN];

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(cmd_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  memset(&terminal_profile_rsp, 0x00, sizeof(gstk_cmd_from_card_type));
  memset(default_plmn, 0xFF, sizeof(default_plmn));

  UIM_MSG_HIGH_1("Command group=%x",
                 cmd_ptr->general_cmd.message_header.command_group );
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

  case GSTK_TERMINAL_PROFILE_RSP: /* received rsp from UIM */
    terminal_profile_rsp.hdr_cmd.command_id = GSTK_PROFILE_DL_IND_RSP;
    /* command_number, cmd_ref_id are ignore */
    terminal_profile_rsp.hdr_cmd.cmd_detail_reference = 0;
    terminal_profile_rsp.hdr_cmd.command_number = 0;
    terminal_profile_rsp.hdr_cmd.sim_slot_id =
      cmd_ptr->terminal_profile_rsp.message_header.sim_slot_id;
    terminal_profile_rsp.hdr_cmd.user_data = gstk_curr_inst_ptr->tp_info.user_data;

    /* Signal GSDI regardless of whether TP success or fails so that
    ** GSDI can continue with initialization */
    if(gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
         gstk_curr_inst_ptr->slot_id) == TRUE)
    {
      gstk_curr_inst_ptr->is_tp_rsp_received = TRUE;
    }
    if ((gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
           gstk_curr_inst_ptr->slot_id) == FALSE)
        ||
        (!cmd_ptr->terminal_profile_rsp.success))
    {
      if(gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb != NULL)
      {
        (gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb)(&terminal_profile_rsp);
      }
    }
    else
    {
      if(rex_get_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer)) > 0)
      {
        if(memcmp(gstk_curr_inst_ptr->gstk_last_loc_info_sent,
                  default_plmn,
                  GSTK_MAX_PLMN_LEN) != 0)
        {
          /* We have already received a message from RRC */
          gstk_status = gstk_send_envelope_loc_info_command(gstk_curr_inst_ptr->client_id,
                                                            gstk_curr_inst_ptr->gstk_last_loc_info_sent, 0);
          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0("Have problem sending location information envelope");
          }
        }
      }
      else if (rex_get_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer)) == 0)
      {
        // timer already expired
        if(gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb != NULL)
        {
          (gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb)(&terminal_profile_rsp);
        }
        gstk_curr_inst_ptr->is_tp_dl_done = TRUE;
      }
    }
    if(cmd_ptr->terminal_profile_rsp.success)
    {
      UIM_MSG_HIGH_0("Received UIM TP Rsp");
      terminal_profile_rsp.cmd.terminal_profile_rsp = GSTK_PROFILE_RSP_SUCCESS;
      gstk_curr_inst_ptr->gstk_state = GSTK_IDLE_S;
      uim_set_proactive_uim_slot(TRUE, (uim_slot_type)cmd_ptr->timer_msg.message_header.sim_slot_id);
    }
    else
    {
      terminal_profile_rsp.cmd.terminal_profile_rsp = GSTK_PROFILE_RSP_FAIL;
      gstk_curr_inst_ptr->gstk_state = GSTK_NO_SIM_S;
    }

    /* send response to client if there is client registered for this */
    gstk_status = gstk_util_send_message_to_clients(&terminal_profile_rsp);
    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_HIGH_1("No client reg for TP Rsp, 0x%x", gstk_status);
      /* reset the gstk status for terminal profile response failure */
      gstk_status = GSTK_SUCCESS;
    }
    break;

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

      case GSTK_MMGSDI_SESSION_CHANGED_EVT:
        gstk_process_session_changed_evt(cmd_ptr);
        break; /* GSTK_MMGSDI_SESSION_CHANGED_EVT */
      
    default:
        break; /* ignore all other MMGSDI events */
    }
    break;

  case GSTK_MODE_CHANGE_IND:
#ifdef FEATURE_CDMA
    (void)gstk_util_merge_cm_stacks(&gstk_curr_inst_ptr->cdma_ext_loci_cache, &cmd_ptr->mode_change_ind);
    /* Update bmsk_3gpp2 to latest information */
    gstk_curr_inst_ptr->bmsk_3gpp2 = 0x0000;
    gstk_process_mode_change_update_3gpp2_bmsk(&(cmd_ptr->mode_change_ind));
#else  /* #ifdef FEATURE_CDMA */
    UIM_MSG_HIGH_0("GSTK_MODE_CHANGE_IND : Feature CMDA not supported");
#endif /*#ifdef FEATURE_CDMA */
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

  case GSTK_ENVELOPE_CMD:
    gstk_status = gstk_process_non_idle_state_envelope_cmd(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Envelope Cmd ERR, %x", gstk_status);
    }
    break;

  case GSTK_CLIENT_REG_REQ_CMD:
    (void)gstk_process_client_reg_req(cmd_ptr);
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
    UIM_MSG_HIGH_1("Cmd 0x%x Not handled in T_P_RSP_WAIT_S",
                   cmd_ptr->general_cmd.message_header.command_group );
    gstk_util_send_err_to_client(cmd_ptr);
    break;
  }

  return gstk_status;
} /*gstk_state_tp_rsp_wait*/


#endif  /* GSTK_S_TERM_PROFILE_RSP_WAIT_C */
/*lint -e750 local macro gstk_c not being used */

