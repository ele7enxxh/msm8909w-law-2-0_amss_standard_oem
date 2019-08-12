/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   N V   R E F R E S H  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to handle the NV Refresh
  commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_nv_refresh.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
05/22/16   vdc     Remove F3 messages for memory allocation failure
04/05/16   vdc     Send act resp and session changed evt at same time
02/02/16   vdc     Migrated to new MCFG Refresh Done API
12/15/14   kk      Fix MCFG UIM data sent for nv refresh
09/09/14   kk      Enhancements in NV handling
08/29/14   hh      Fix compliation warnings
08/07/14   yt      Avoid NULL pointer dereference
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_nv_refresh.h"

#include "nv.h"
#include "uim_msg.h"
#include "err.h"
#include "mmgsdiutil.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdi.h"
#include "queue.h"
#include "fs_public.h"
#include "mmgsdilib_v.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_euicc.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================
FUNCTION MMGSDI_PROCESS_CONTINUE_AFTER_NV_REFRESH

DESCRIPTION
  Processing of continue after nv refresh req from MCFG which posts
  CARD INSERTED evt or SESSION CHANGED evt based on the state
  of the card or state of application.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_continue_after_nv_refresh(
  const mmgsdi_continue_after_nv_refresh_req_type *req_ptr)
{
  mmgsdi_return_enum_type        mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_slot_data_type         *slot_data_ptr     = NULL;
  boolean                        card_init_pending = FALSE;
  mmgsdi_session_id_type         card_session_id   = 0;
  uint8                          slot_index        = MMGSDI_SLOT_1_INDEX;
  mmgsdi_nv_context_type         nv_context        = MMGSDI_NV_CONTEXT_INVALID;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_4("Continue after NV Refresh for slot: 0x%x and context 0x%x, received for ref id: 0x%x MMGSDI waiting on ref id: 0x%x",
    req_ptr->request_header.slot_id, req_ptr->nv_context,
    req_ptr->reference_id, slot_data_ptr->nv_refresh_reference_count);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                    &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Before processing further - check if the reference id matches

     Reference id is used when back to back MCFG UPDATE UIM DATA
     APIs are called during back to back hotswaps etc. MMGSDI has to
     ignore if the reference id do not match. If the reference id do
     not match, continue after nv refresh trigger from MCFG might be
     related to old MCFG NV Refresh triggers  */
  if(slot_data_ptr->nv_refresh_reference_count !=
     req_ptr->reference_id)
  {
    UIM_MSG_ERR_4("Continue trigger received for unexpected ref id: 0x%x  slot: 0x%x and context 0x%x - MMGSDI waiting on ref id: 0x%x",
      req_ptr->reference_id, req_ptr->request_header.slot_id, req_ptr->nv_context,
      slot_data_ptr->nv_refresh_reference_count);
    return MMGSDI_ERROR;
  }

  /* Get the Card Slot Session ID corresponding to the slot */
  card_session_id = mmgsdi_generic_data_ptr->slot_session_id[slot_index];

  if(slot_data_ptr->nv_refresh_in_progress)
  {
    slot_data_ptr->nv_refresh_in_progress = FALSE;
    card_init_pending = TRUE;
  }

  if(card_init_pending)
  {
    boolean   fail_reason_card_error = FALSE;
    boolean   ef_dir_not_found       = FALSE;

    /* Link established processing pending for the slot */
    UIM_MSG_HIGH_0("Processing continue after nv refresh for card inserted");

    /* Determine the Protocol used at the SIM - ME Interface */
    switch (slot_data_ptr->protocol)
    {
      case MMGSDI_ICC:
        /* GSM DCS Checking pending */
        /* As per spec 10.7 of GSM 11.11, the DF-DCS should be selected if and only if
           DF-GSM (7F20) is absent.

           NOTE 1: The selection of the GSM application using the identifier '7F21',
           if selection by means of the identifier '7F20' fails, is to ensure backwards
           compatibility with those Phase 1 SIMs which only support the DCS 1800 application
           using the Phase 1 directory DFDCS1800 coded '7F21'.

           NOTE 2: To ensure backwards compatibility with those Phase 1 DCS 1800 MEs,
           which have no means to selectDFGSM two options have been specified.
           These options are given in GSM 09.91 [17].
        */
        nv_context = mmgsdi_util_get_efs_item_index_for_slot(req_ptr->request_header.slot_id);

        /* Look for DCS 1800 DF only for 1X/GSM Targets */
        if (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_UIM_GSM_DCS_1800,
              nv_context) == MMGSDI_FEATURE_ENABLED)
        {
          if(!slot_data_ptr->gsm_df_present)
          {
            mmgsdi_status = mmgsdi_icc_is_dcs1800_df_present(card_session_id,
              req_ptr->request_header.slot_id);
            if (mmgsdi_status == MMGSDI_SUCCESS)
            {
              UIM_MSG_HIGH_0("DCS 1800 DF Present");

              slot_data_ptr->dcs1800_present = TRUE;
              slot_data_ptr->card_apps_available |= MMGSDI_DCS1800_APP_MASK;
            }
          }
        }

        break;

      case MMGSDI_UICC:
        mmgsdi_status = mmgsdi_card_init_trigger_tp_dl_for_uicc(req_ptr->request_header.slot_id,
                                                                &fail_reason_card_error,
                                                                &ef_dir_not_found);
        break;

      default:
        /* A Protocol was not established at the UIM Interface */
        UIM_MSG_HIGH_2("Invalid Protocol 0x%x for slot 0x%x",
                       slot_data_ptr->protocol, req_ptr->request_header.slot_id);
        break;
    }

    /* Some eUICCs do not have a correct information in the ATR
       regarding eUICC support. In such cases, slot_data_ptr->is_euicc
       could be set to FALSE. We need to find out if it is really an
       eUICC or not by sending a SELECT on ISDR. If SELECT on ISDR
       succeeds, it is an eUICC, otherwise not */
    if(!slot_data_ptr->is_euicc && ef_dir_not_found)
    {
      boolean is_isdr_found     = FALSE;

      if(MMGSDI_SUCCESS == mmgsdi_euicc_is_isdr_found(req_ptr->request_header.slot_id,
                                                      &is_isdr_found) &&
         is_isdr_found)
      {
        slot_data_ptr->is_euicc = TRUE;
      }
    }

    if(slot_data_ptr->card_apps_available != 0x00 ||
       slot_data_ptr->is_euicc)
    {
      /* Valid applications were found */
      mmgsdi_status = MMGSDI_SUCCESS;
    }
#ifndef FEATURE_GSTK
    /* If fail_reason_card_error is TRUE, it means UIMDRV has not processed
       this cmd because of some internal error or card_error. In that case,
       there is no need to switch the protocol */
    else if(slot_data_ptr->protocol == MMGSDI_UICC &&
            fail_reason_card_error == FALSE)
    {
      UIM_MSG_HIGH_1("No valid UICC app. found on slot 0x%x, switching protocol to ICC",
                     req_ptr->request_header.slot_id);

      mmgsdi_util_preinit_mmgsdi(card_session_id,MMGSDI_APP_NONE,MMGSDI_REFRESH_RESET,
                                 req_ptr->request_header.slot_id,FALSE);

      /* Switch card protocol to ICC */
      mmgsdi_status = mmgsdi_gen_card_reset_sync(req_ptr->request_header.slot_id, UIM_ICC);

      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        /* a new link establised req will be received */
        return mmgsdi_status;
      }
    }
#endif /* FEATURE_GSTK */

    /* Need send out card inserted based on mmgsdi_status and slot TP status */
    if (((slot_data_ptr->mmgsdi_tp_state != MMGSDI_TP_STATE_IN_PROGRESS_OK)      &&
         (slot_data_ptr->mmgsdi_tp_state != MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)) &&
        (slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED))
    {
      UIM_MSG_HIGH_1("MMGSDI_CARD_INSERTED for SLOT 0x%x",
                     req_ptr->request_header.slot_id);

      mmgsdi_evt_build_and_send_card_inserted(
        TRUE, (mmgsdi_client_id_type)0, req_ptr->request_header.slot_id);
    }
    mmgsdi_status = MMGSDI_SUCCESS;
  }/*slot refresh continue handling */
  else
  {
    /* no of types of RAT tech that mmgsdi supports in prov sessions are 2 - GW and 1x */
    mmgsdi_session_id_type         prov_sessions[2]  = {0};
    mmgsdi_session_info_type      *session_info_ptr  = 0;
    mmgsdi_int_app_info_type      *sel_app_ptr       = NULL;
    mmgsdi_channel_info_type      *channel_info_ptr  = NULL;
    mmgsdi_evt_session_notify_type notify_type;
    uint8                          index             = 0;

    UIM_MSG_HIGH_0("Processing continue after nv refresh for session changed evt");

    memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

    /* NV refresh after prov session activation  -
       send out session changed evt for all applicable sessions */
    /*req_ptr->nv_context indicates the session type on which pending activations */

    /* check if gw and 1x sessions are waiting for NV refresh -
           if so, trigger session changed for each session types */
    switch(req_ptr->nv_context)
    {
      case MMGSDI_NV_CONTEXT_PRIMARY:
        prov_sessions[0] = mmgsdi_generic_data_ptr->pri_gw_session_id;
        prov_sessions[1] = mmgsdi_generic_data_ptr->pri_1x_session_id;
      break;

      case MMGSDI_NV_CONTEXT_SECONDARY:
        prov_sessions[0] = mmgsdi_generic_data_ptr->sec_gw_session_id;
        prov_sessions[1] = mmgsdi_generic_data_ptr->sec_1x_session_id;
      break;

      case MMGSDI_NV_CONTEXT_TERTIARY:
        prov_sessions[0] = mmgsdi_generic_data_ptr->ter_gw_session_id;
        prov_sessions[1] = mmgsdi_generic_data_ptr->ter_1x_session_id;
      break;

      default:
      return MMGSDI_ERROR;
    }

    for(index = 0; index < 2; index++)
    {
      sel_app_ptr = NULL;
      channel_info_ptr = NULL;
      session_info_ptr = NULL;
      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        prov_sessions[index],
                        NULL,
                        NULL,
                        &sel_app_ptr,
                        &channel_info_ptr,
                        &session_info_ptr,
                        NULL);
      if((mmgsdi_status != MMGSDI_SUCCESS) || (sel_app_ptr == NULL) ||
         (session_info_ptr == NULL) || (channel_info_ptr == NULL))
      {
        UIM_MSG_ERR_3("index 0x%x session inactive,chnnl_info_p=0x%x,app_info_p=0x%x",
                      index,channel_info_ptr, sel_app_ptr);
        continue;
      }

      if(sel_app_ptr != NULL &&
         sel_app_ptr->nv_refresh_in_progress)
      {
        uint8 prov_index = MMGSDI_MAX_PROV_APPS;

        /* trigger sending out Session changed event
           only if nv refresh was in progress*/
        UIM_MSG_HIGH_1("After nv refresh index: 0x%x session active sending out session changed evt",
                        index);

        prov_index = mmgsdi_util_get_prov_index_from_session(session_info_ptr->session_type);

        /* Send the confirmation for activation before sending session changed event */
        if(prov_index < MMGSDI_MAX_PROV_APPS &&
           mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index] != NULL &&
           mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index]->client_req_cb != NULL)
        {
          (mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index]->client_req_cb)
            (mmgsdi_status,
            MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF,
            &mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index]->mmgsdi_cnf);

          MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index]);
        }

        sel_app_ptr->nv_refresh_in_progress = FALSE;
        notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
        notify_type.slot_id     = channel_info_ptr->slot_id;
        mmgsdi_evt_build_and_send_session_changed(
          notify_type, TRUE, (const mmgsdi_int_app_info_type*)sel_app_ptr);
      }
    }

    mmgsdi_status = MMGSDI_SUCCESS;
  } /* Session refresh continue handling */

  return mmgsdi_status;
}/*mmgsdi_process_continue_after_nv_refresh*/


/*===========================================================================
FUNCTION MMGSDI_PROCESS_NV_REFRESH_REQ

DESCRIPTION
  This function processes nv refresh req - slot and session nv/efs items
  are refreshed.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_nv_refresh_req(
  const mmgsdi_handle_nv_refresh_req_type *req_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  UIM_MSG_HIGH_3("Received NV Refresh of type 0x%x - on slot: 0x%x, session: 0x%x",
                req_ptr->type, req_ptr->slot_index, req_ptr->sub_index);

  /*invoke NV refresh procedures here */
  if (req_ptr->type == MCFG_REFRESH_TYPE_SLOT || req_ptr->type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
  {
    mmgsdi_status = mmgsdi_nv_refresh_slot_items(req_ptr->slot_index);
    UIM_MSG_HIGH_1("Refresh of slot-based items status: 0x%x", mmgsdi_status);
  }

  if (req_ptr->type == MCFG_REFRESH_TYPE_SUBS || req_ptr->type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
  {
    mmgsdi_status = mmgsdi_nv_refresh_session_items(req_ptr->sub_index);
    UIM_MSG_HIGH_1("Refresh of session-based items status: 0x%x", mmgsdi_status);
  }

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* trigger MCFG that the procedure is done */
  (void)mcfg_refresh_done_w_status(req_ptr->reference_id, MCFG_REFRESH_SUCCESS);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
  return MMGSDI_SUCCESS;
}/*mmgsdi_process_nv_refresh_req*/


/*===========================================================================
FUNCTION MMGSDI_NV_REFRESH_MCFG_CB

DESCRIPTION
  Call back registered with MCFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_nv_refresh_mcfg_cb(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_handle_nv_refresh_req_type *msg_ptr        = NULL;
  int32                              total_mem_len  = 0;
  uint32                             task_cmd_len   = 0;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;

  if(mcfg_refresh_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("MCFG REFRESH INFO PTR NULL");
    return FALSE;
  }

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     task_cmd_len);
  if(task_cmd_ptr == NULL)
  {
   return FALSE;
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.mcfg_refresh_data;

  /* Populating mmgsdi_continue_after_nv_refresh header
     1) Client ID
     2) Request TYPE
     3) Slot ID
     4) Client Data Pointer
     5) Response CallBack
     6) Event Callback
     7) Payload len and request len will be populated after the content
        has been populated */

  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_HANDLE_NV_REFRESH_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_HANDLE_NV_REFRESH_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_HANDLE_NV_REFRESH_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.session_id        = 0;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = NULL;

  /*store mcfg refresh data */
  msg_ptr->type         = mcfg_refresh_info_ptr->type;
  msg_ptr->slot_index   = mcfg_refresh_info_ptr->slot_index;
  msg_ptr->sub_index    = mcfg_refresh_info_ptr->sub_index;
  msg_ptr->reference_id = mcfg_refresh_info_ptr->reference_id;


  /* Calculate the payload len and request len */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
   total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return FALSE;
  }

  UIM_MSG_HIGH_4("MCFG CB: MMGSDI_HANDLE_NV_REFRESH_REQ status 0x%x, type 0x%x, slot 0x%x, sub 0x%x",
                mmgsdi_status,mcfg_refresh_info_ptr->type,
                mcfg_refresh_info_ptr->slot_index,mcfg_refresh_info_ptr->sub_index);
  return TRUE;
}/*mmgsdi_nv_refresh_mcfg_cb*/

