/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   M U L T I   P R O F I L E  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the multi profile specific functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015-2016 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_multi_profile.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/16   vdc     Send successful resp for get sim profile in success case
05/31/16   lm      Added support to detect multi profile card based on EFs
05/30/16   lm      Determine number of profiles based on card response
05/03/16   tkl     F3 log prints cleanup
05/02/16   sp      Reduce F3 log prints
11/23/15   vdc     Send profile switching evt when profile has switched in card
09/25/15   vr      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib_common.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "rex.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_session.h"
#include "intconv.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_cnf.h"

#ifdef FEATURE_UIM_MULTI_PROFILE
#include "ecall_app.h"
#include "mmgsdi_multi_profile.h"
#include "uim_common_efs.h"
#include "DDIChipInfo.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define MMGSDI_MULTI_PROFILE_CARD_NUM_PROFILE_LENGTH              0x01
#define MMGSDI_MULTI_PROFILE_CARD_PROFILE_DATA_LENGTH             0x01
#define MMGSDI_MULTI_PROFILE_CARD_NUM_APDU_LENGTH                 0x01
#define MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_LENGTH                 0x01
#define MMGSDI_MULTI_PROFILE_CARD_STATUS_WORD_LENGTH              0x02
#define MMGSDI_MULTI_PROFILE_CARD_PROFILE_SWITCH_ACTION_LENGTH    0x01
#define MMGSDI_MULTI_PROFILE_CARD_PROFILE_DEDICATED_CH_LENGTH     0x01
#define MMGSDI_MULTI_PROFILE_CARD_AID_LENGTH                      0x01
#define MMGSDI_MULTI_PROFILE_CARD_MULTI_PROFILE_SUPPORT_LENGTH    0x01

/* Multi profile info global */
mmgsdi_multi_profile_card_system_data_type *mmgsdi_profile_info_ptr    = NULL;

/* ==========================================================================
FUNCTION MMGSDI_MULTI_PROFILE_ECALL_INFO_CB

DESCRIPTION
  This function handles the call info provided by eCall module

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
----------------------------------------------------------------------------*/
static void mmgsdi_multi_profile_ecall_info_cb(
  ecall_type_of_call     type_of_call,
  ecall_activation_type  activation_type
)
{
  UIM_MSG_HIGH_2("Received ECALL_INFO_CB: type of call: 0x%x, activation type: 0x%x",
                 type_of_call, activation_type);

  /* Enter client app data critical section to protect profile info global */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_profile_info_ptr != NULL)
  {
    if(type_of_call == ECALL_NO_ACTIVE_CALL)
    {
      mmgsdi_profile_info_ptr->is_ecall_active = FALSE;
    }
    else
    {
      mmgsdi_profile_info_ptr->is_ecall_active = TRUE;
    }

    /* Enter task state critical section to protect task state global */
    MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

    if(mmgsdi_task_state[MMGSDI_TASK_SLOT1] == MMGSDI_TASK_ACTIVE)
    {
    /* At present, Multi profile cards are supported on only slot1. So, we use 
       directly use slot1's TCB */
      rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_ECALL_STATUS_SIG);
    }

    /* Leave task state critical section */
    MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT
  }

  /* Leave client app data critical section */
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
} /* mmgsdi_multi_profile_ecall_info_cb */


/* ==========================================================================
FUNCTION:      MMGSDI_MULTI_PROFILE_GET_APDU

DESCRIPTION:
  This function provides the APDU data pointer & APDU count to switch the profile

DEPENDENCIES:
  NONE

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command dequeued, or no commands pending
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS:
  None

CRITICAL SECTIONS:
None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_multi_profile_get_apdu(
  mmgsdi_profile_id_enum_type  profile_id,
  mmgsdi_data_type           **apdu_data_pptr,
  uint8                       *num_apdu_count_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_3(apdu_data_pptr,
                              mmgsdi_profile_info_ptr,
                              num_apdu_count_ptr);

  UIM_MSG_HIGH_1("Multi profile APDU switch for profile id: 0x%x", profile_id);

  /* If we want to get the current active profile from the card */
  if(mmgsdi_profile_info_ptr->get_active_profile)
  {
    *apdu_data_pptr = &mmgsdi_profile_info_ptr->get_active_profile_apdu;
    *num_apdu_count_ptr = 0x01;
  }
  /* If we want to switch / swtich back the profile in the card */
  else
  {
    switch(profile_id)
    {
      case MMGSDI_PROFILE_1:
        *apdu_data_pptr = mmgsdi_profile_info_ptr->apdu_switch_back;
        *num_apdu_count_ptr = mmgsdi_profile_info_ptr->num_apdu_switch_back;
        break;

      case MMGSDI_PROFILE_2:
         *apdu_data_pptr = mmgsdi_profile_info_ptr->apdu_switch;
         *num_apdu_count_ptr = mmgsdi_profile_info_ptr->num_apdu_switch;
        break;

      default:
        mmgsdi_status = MMGSDI_ERROR;
        break;
    }
  }
  return mmgsdi_status;
} /* mmgsdi_multi_profile_get_apdu */


/*===========================================================================
FUNCTION MMGSDI_MULTI_PROFILE_SET_SIM_PROFILE

DESCRIPTION
  This function is called in mmgsdi_process_command.
  It will switch the profile to the given profile id.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_multi_profile_set_sim_profile (
  const mmgsdi_set_sim_profile_req_type *set_sim_profile_req_ptr,
  mmgsdi_protocol_enum_type              protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_SUCCESS;
  uim_cmd_type*              uim_cmd_ptr    = NULL;
  uim_slot_type              uim_slot       = UIM_SLOT_NONE;
  int32                      index          = 0;
  uint8                      profile_index  = 0;
  uint8                      apdu_index     = 0;
  uint8                      num_apdu_count = 0;
  mmgsdi_data_type          *apdu_data_ptr  = NULL;
  mmgsdi_task_enum_type      mmgsdi_task    = MMGSDI_TASK_MAX;
  mmgsdi_status_cnf_type    *status_cnf_ptr = NULL;
  mmgsdi_status_req_type     status_req;
  mmgsdi_sw_status_type      status_word    = {0};

  MMGSDIUTIL_RETURN_IF_NULL(set_sim_profile_req_ptr);

  memset(&status_req, 0x00, sizeof(mmgsdi_status_req_type));

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_ICC ||
     mmgsdi_profile_info_ptr == NULL ||
     mmgsdi_profile_info_ptr->num_profile <= 1)
  {
    UIM_MSG_ERR_0("protocol is ICC/Multi profile is NULL/Single profile supported");
    mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_MULTI_PROFILE_NOT_SUPPORTED,
                                               &set_sim_profile_req_ptr->request_header,
                                               NULL,
                                               NULL,
                                               FALSE,
                                               status_word);
    return mmgsdi_status;
  }

  /* If the requested profile is same as current profile then dont send the command to the card and return */
  if(set_sim_profile_req_ptr->activate_profile_id == mmgsdi_profile_info_ptr->active_profile.profile_id)
  {
    UIM_MSG_ERR_1("Requested profile is already active: 0x%x",
                  mmgsdi_profile_info_ptr->active_profile.profile_id);

    mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_MULTI_PROFILE_NO_EFFECT,
                                               &set_sim_profile_req_ptr->request_header,
                                               NULL,
                                               NULL,
                                               FALSE,
                                               status_word);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_convert_uim_slot(set_sim_profile_req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Try to get the ecall status info if
     1) we are not trying to get the current active profile (for G & D).
     2) switch is from emergency to regular 
     3) Profile detection is done */
#ifdef FEATURE_ECALL_APP
  if(mmgsdi_profile_info_ptr->get_active_profile == FALSE &&
     set_sim_profile_req_ptr->activate_profile_id == MMGSDI_PROFILE_1 &&
     mmgsdi_profile_info_ptr->reg_profile_detected)
  {
    rex_sigs_type sigs = 0x00;

    (void) rex_clr_sigs(rex_self(), MMGSDI_ECALL_STATUS_SIG);

    if(FALSE == ecall_session_get_call_info(mmgsdi_multi_profile_ecall_info_cb))
    {
      UIM_MSG_ERR_0("Could not get ecall info");
      return MMGSDI_ERROR;
    }

    sigs = mmgsdi_wait(MMGSDI_ECALL_STATUS_SIG);
    if(sigs & MMGSDI_ECALL_STATUS_SIG)
    {
      (void) rex_clr_sigs(rex_self(), MMGSDI_ECALL_STATUS_SIG);
    }

    if(mmgsdi_profile_info_ptr->is_ecall_active)
    {
      UIM_MSG_ERR_0("Cannot switch from emergency to regular profile, ecall is active");
      return MMGSDI_ERROR;
    }
  }
#endif /* FEATURE_ECALL_APP */

  /* At the time of link establishment, MMGSDI sends the set sim profile
     command to determine the current active profile. In order to satisfy the
     card behavior at this link established time, we have to send the status
     command before the set sim profile command */
  if(set_sim_profile_req_ptr->request_header.client_id == mmgsdi_generic_data_ptr->client_id)
  {
    status_req.request_header.client_id = set_sim_profile_req_ptr->request_header.client_id;
    status_req.request_header.request_type = MMGSDI_CARD_STATUS_REQ;
    status_req.request_header.orig_request_type = MMGSDI_SET_SIM_PROFILE_REQ;
    status_req.request_header.slot_id = set_sim_profile_req_ptr->request_header.slot_id;

    status_req.me_app_status   = MMGSDI_STATUS_APP_NO_INDICATION;
    status_req.ret_data_struct = MMGSDI_STATUS_NO_DATA;

    mmgsdi_status = mmgsdi_uim_uicc_status(&status_req, TRUE, &status_cnf_ptr, NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Status command is not successful");
    }
    MMGSDIUTIL_TMC_MEM_FREE(status_cnf_ptr);
  }

  /* Get the APDU data & count to perform switch profile or get current active profile */
  mmgsdi_status = mmgsdi_multi_profile_get_apdu(set_sim_profile_req_ptr->activate_profile_id,
                                                &apdu_data_ptr,
                                                &num_apdu_count);
  if(mmgsdi_status != MMGSDI_SUCCESS || apdu_data_ptr == NULL)
  {
    UIM_MSG_ERR_1("Could not get APDU, status: 0x%x", mmgsdi_status);
    return mmgsdi_status;
  }

  /* 1) Get a UIM buffer for the request.
     2) Check for if the buffer is NULL or not. */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /* Populate UIM information */
  uim_cmd_ptr->hdr.command  = UIM_STREAM_ISO7816_APDU_F;
  uim_cmd_ptr->hdr.slot     = uim_slot;
  uim_cmd_ptr->hdr.protocol = UIM_UICC;
  
  /* Get a new index from client req info table for response data.
     Set the uim data pointer to the client request table data buffer */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &set_sim_profile_req_ptr->request_header,
                    uim_cmd_ptr,
                    NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  for(apdu_index = 0; apdu_index < num_apdu_count; apdu_index++)
  {
    /* Set UIM Stream APDU IS07816 Channel Command Options */
    if(apdu_data_ptr->data_len > MMGSDI_MAX_CHANNEL_DATA_CMD_LEN)
    {
      UIM_MSG_ERR_1("APDU data length is more than max data len: 0x%x",
                    apdu_data_ptr->data_len);
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
    }

    uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes = int32touint16(apdu_data_ptr->data_len);
    mmgsdi_memscpy(uim_cmd_ptr->stream_iso7816_apdu.cmd_data,
                   sizeof(uim_cmd_ptr->stream_iso7816_apdu.cmd_data),
                   apdu_data_ptr->data_ptr,
                   int32touint32(apdu_data_ptr->data_len));

    /* Modify the CLA byte of the APDU to include dedicated channel */
    if(mmgsdi_profile_info_ptr->dedicated_channel_needed)
    {
      if(mmgsdi_profile_info_ptr->dedicated_channel_id < MMGSDI_MAX_CHANNEL_INFO &&
         mmgsdi_profile_info_ptr->dedicated_channel_id != UIM_CHANNEL0)
      {
        uim_cmd_ptr->stream_iso7816_apdu.cmd_data[0] =
          mmgsdi_profile_info_ptr->dedicated_channel_id;
      }
      else
      {
        UIM_MSG_ERR_1("Invalid channel id provided: 0x%x",
                      mmgsdi_profile_info_ptr->dedicated_channel_id);
        mmgsdi_util_free_client_request_table_index(index);
        return MMGSDI_ERROR;
      }
    }

    /* If the request is not from MMGSDI & this is the first APDU to switch 
       the profile then update the global and send the profile data event. */
    if(set_sim_profile_req_ptr->request_header.client_id != mmgsdi_generic_data_ptr->client_id &&
       apdu_index == 0)
    {
      /* Update the global with the new profile id & profile type */
      for(profile_index = 0; profile_index < mmgsdi_profile_info_ptr->num_profile; profile_index++)
      {
        if(set_sim_profile_req_ptr->activate_profile_id ==
             mmgsdi_profile_info_ptr->profile_data[profile_index].profile_id)
        {
          mmgsdi_profile_info_ptr->active_profile.profile_id =
            mmgsdi_profile_info_ptr->profile_data[profile_index].profile_id;
          mmgsdi_profile_info_ptr->active_profile.profile_type =
            mmgsdi_profile_info_ptr->profile_data[profile_index].profile_type;
          break;
        }
      }

      if(profile_index == mmgsdi_profile_info_ptr->num_profile)
      {
        UIM_MSG_ERR_1("Not able to retrieve the profile type from given profile id: 0x%x",
                      set_sim_profile_req_ptr->activate_profile_id);
        mmgsdi_util_free_client_request_table_index(index);
        return MMGSDI_ERROR;
      }

      /* Send the event when we send the set SIM profile request to the card */
      mmgsdi_evt_build_and_send_profile_data_evt(
        TRUE,
        mmgsdi_profile_info_ptr->active_profile,
        set_sim_profile_req_ptr->request_header.client_id,
        set_sim_profile_req_ptr->request_header.slot_id);
    }

    /* Assign uim_cmd_ptr's user data */
    uim_cmd_ptr->hdr.user_data = int32touint32(index);
    UIM_MSG_HIGH_0("Sending down SIM profile APDU command to UIM");
  
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /*---------------------------------------------------------------------------
      1) Release the client table (Others pointer will be released in the
         function)
      ---------------------------------------------------------------------------*/
      mmgsdi_util_free_client_request_table_index(index);
      UIM_MSG_ERR_1("send cmd to UIM fail table index = 0x%x!", index);
    }
    else
    {
      mmgsdi_task = mmgsdi_util_get_current_task();
      if(mmgsdi_task != MMGSDI_TASK_MAX &&
         mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
        mmgsdi_status = mmgsdi_uicc_rsp_sim_profile(
                 mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task],
                 set_sim_profile_req_ptr->activate_profile_id,
                 apdu_index,
                 num_apdu_count);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);

        if(mmgsdi_status != MMGSDI_SUCCESS && mmgsdi_status != MMGSDI_MULTI_PROFILE_NO_EFFECT)
        {
          break;
        }

        /* If get active profile is TRUE then we make it FALSE and dont send
           the status command to the card. We prepare the status request when
           the request was a switch profile command. We send it to the card to
           avoid delay in sending the REFRESH */
        if(mmgsdi_profile_info_ptr->get_active_profile)
        {
          mmgsdi_profile_info_ptr->get_active_profile = FALSE;
        }
        else if(mmgsdi_status == MMGSDI_SUCCESS &&
                mmgsdi_profile_info_ptr->switch_action == MMGSDI_MULTI_PROFILE_CARD_SWITCH_ACTION_REFRESH)
        {
          /* If the request is to switch the profile to emergency then set
             the emergency refresh flag */
          if(set_sim_profile_req_ptr->activate_profile_id == MMGSDI_PROFILE_2)
          {
            mmgsdi_profile_info_ptr->is_emergency_refresh = TRUE;
          }

          status_req.request_header.client_id = set_sim_profile_req_ptr->request_header.client_id;
          status_req.request_header.request_type = MMGSDI_CARD_STATUS_REQ;
          status_req.request_header.orig_request_type = MMGSDI_SET_SIM_PROFILE_REQ;
          status_req.request_header.slot_id = set_sim_profile_req_ptr->request_header.slot_id;

          status_req.me_app_status   = MMGSDI_STATUS_APP_NO_INDICATION;
          status_req.ret_data_struct = MMGSDI_STATUS_NO_DATA;

          mmgsdi_status = mmgsdi_uim_uicc_status(&status_req, TRUE, &status_cnf_ptr, NULL);
          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            UIM_MSG_ERR_0("Status command is not successful");
          }
          MMGSDIUTIL_TMC_MEM_FREE(status_cnf_ptr);
        }
      }
      else
      {
        UIM_MSG_ERR_1("Sync process for request at index 0x%x has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index);
        mmgsdi_status = MMGSDI_ERROR;
      }
    }
    apdu_data_ptr++;
  }

  mmgsdi_util_free_client_request_table_index(index);

  if(mmgsdi_status == MMGSDI_MULTI_PROFILE_NO_EFFECT)
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  return mmgsdi_status;
}/* mmgsdi_multi_profile_set_sim_profile */


/*===========================================================================
FUNCTION MMGSDI_MULTI_PROFILE_GET_SIM_PROFILE

DESCRIPTION
  This function is called in mmgsdi_process_command.
  It will retrieve the profiles present in the EFS.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_multi_profile_get_sim_profile (
  const mmgsdi_get_sim_profile_req_type *get_profile_req_ptr,
  mmgsdi_protocol_enum_type              protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_sw_status_type      status_word   = {0};

  MMGSDIUTIL_RETURN_IF_NULL(get_profile_req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  /* If the protocol is ICC or profile info global is NULL then we dont
     support get sim profile API */
  if(protocol == MMGSDI_ICC || mmgsdi_profile_info_ptr == NULL)
  {
    mmgsdi_status = MMGSDI_MULTI_PROFILE_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                             &get_profile_req_ptr->request_header,
                                             NULL,
                                             NULL,
                                             FALSE,
                                             status_word);
  return mmgsdi_status;
}/* mmgsdi_multi_profile_get_sim_profile */


/*===========================================================================
FUNCTION MMGSDI_CLEAR_MULTI_PROFILE_HEAP_ALLOC

DESCRIPTION
  This function deletes multi profile related data.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_multi_profile_clear_heap_alloc(
  void
)
{
  uint8 apdu_index = 0;

  if(mmgsdi_profile_info_ptr != NULL)
  {
    for(apdu_index = 0; apdu_index < mmgsdi_profile_info_ptr->num_apdu_switch; apdu_index++)
    {
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_profile_info_ptr->apdu_switch[apdu_index].data_ptr);
    }
    for(apdu_index = 0; apdu_index < mmgsdi_profile_info_ptr->num_apdu_switch_back; apdu_index++)
    {
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_profile_info_ptr->apdu_switch_back[apdu_index].data_ptr);
    }

    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_profile_info_ptr->get_active_profile_apdu.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_profile_info_ptr);
  }
}/* mmgsdi_clear_multi_profile_heap_alloc */


/*===========================================================================
FUNCTION MMGSDI_MULTI_PROFILE_INIT

DESCRIPTION
  Function to initialize the multi profile related globals. This function reads
  & parse the profile information from EFS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_multi_profile_init(
  void
)
{
  uim_common_efs_status_enum_type  efs_status              = UIM_COMMON_EFS_SUCCESS;
  uint32                           multi_profile_file_size = 0;
  uint8                           *file_buffer_ptr         = NULL;
  uint8                           *orig_file_buffer_ptr    = NULL;
  uint8                           *profile_type_ptr        = NULL;
  uint8                            profile_index           = 0;
  uint8                            profile_data_len        = 0;
  uint8                            apdu_index              = 0;
  uint8                            apdu_data_len           = 0;
  boolean                          parse_status            = FALSE;
  DalDeviceHandle                 *phChipInfo              = NULL;
  DalChipInfoIdType                ChipId                  = DALCHIPINFO_ID_UNKNOWN;

  if (DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo) == DAL_SUCCESS)
  {
    (void)DalChipInfo_GetChipId(phChipInfo, &ChipId);

    UIM_MSG_HIGH_1("Multi profile init, chipinfo_id: 0x%x", ChipId);

    /* Multi profile is supported only for 9x28 target */
    if(ChipId != DALCHIPINFO_ID_MDM9628)
    {
      return;
    }
  }
  else
  {
    UIM_MSG_ERR_1("DAL device attach request failed phChipInfo:0x%x", phChipInfo);
    return;
  }

  /* Get the size of the file present in the EFS */
  efs_status = uim_common_efs_get_file_size(UIM_COMMON_EFS_MMGSDI_MULTI_PROFILE_FILE,
                                            UIM_COMMON_EFS_CONTEXT_0,
                                            &multi_profile_file_size);

  if(efs_status != UIM_COMMON_EFS_SUCCESS || multi_profile_file_size == 0)
  {
    UIM_MSG_ERR_2("Multi profile support is not present, File Status: 0x%x File Size: 0x%x",
                   efs_status, multi_profile_file_size);
    goto send_result;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(file_buffer_ptr, multi_profile_file_size);
  if(file_buffer_ptr == NULL)
  {
    goto send_result;
  }

  orig_file_buffer_ptr = file_buffer_ptr;

  /* Get NV values from efs */
  efs_status = uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_MULTI_PROFILE_FILE,
                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                   UIM_COMMON_EFS_CONTEXT_0,
                                   file_buffer_ptr,
                                   multi_profile_file_size);

  if (efs_status != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_1("EFS read failed for multi profile: 0x%x", efs_status);
    goto send_result;
  }

  if(mmgsdi_profile_info_ptr == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_profile_info_ptr,
                                       sizeof(mmgsdi_multi_profile_card_system_data_type));
    if(mmgsdi_profile_info_ptr == NULL)
    {
      goto send_result;
    }
  }

  /* Parse num_profiles */
  if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_NUM_PROFILE_LENGTH <=
       orig_file_buffer_ptr + multi_profile_file_size)
  {
    mmgsdi_profile_info_ptr->num_profile = *file_buffer_ptr;
    if(mmgsdi_profile_info_ptr->num_profile <= 1 ||
       mmgsdi_profile_info_ptr->num_profile > MMGSDI_MAX_PROFILE)
    {
      goto send_result;
    }
  }
  else
  {
    goto send_result;
  }
  file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_NUM_PROFILE_LENGTH;

  /*Parse profile data */
  for(profile_index = 0; profile_index < mmgsdi_profile_info_ptr->num_profile; profile_index++)
  {
    if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_PROFILE_DATA_LENGTH <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      profile_data_len = *file_buffer_ptr;
      file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_PROFILE_DATA_LENGTH;

      if(file_buffer_ptr + profile_data_len <= orig_file_buffer_ptr + multi_profile_file_size)
      {
        if(*file_buffer_ptr != MMGSDI_PROFILE_1 && *file_buffer_ptr != MMGSDI_PROFILE_2)
        {
          goto send_result;
        }
        mmgsdi_profile_info_ptr->profile_data[profile_index].profile_id =
          (mmgsdi_profile_id_enum_type)*file_buffer_ptr;

        profile_type_ptr = file_buffer_ptr + 1;
        if(*profile_type_ptr != MMGSDI_PROFILE_TYPE_REGULAR &&
           *profile_type_ptr != MMGSDI_PROFILE_TYPE_EMERGENCY)
        {
          goto send_result;
        }
        mmgsdi_profile_info_ptr->profile_data[profile_index].profile_type =
          (mmgsdi_profile_type_enum_type)*profile_type_ptr;

        if(mmgsdi_profile_info_ptr->profile_data[profile_index].profile_id == MMGSDI_PROFILE_1 &&
           mmgsdi_profile_info_ptr->profile_data[profile_index].profile_type != MMGSDI_PROFILE_TYPE_REGULAR)
        {
          goto send_result;
        }
        file_buffer_ptr += profile_data_len;
      }
    }
    else
    {
      goto send_result;
    }
  }

  /*Parse num APDUs for switch */
  if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_NUM_APDU_LENGTH <=
       orig_file_buffer_ptr + multi_profile_file_size)
  {
    if(*file_buffer_ptr <= 0 ||
       *file_buffer_ptr > MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_TRANSACTIONS)
    {
      goto send_result;
    }

    mmgsdi_profile_info_ptr->num_apdu_switch = *file_buffer_ptr;
  }
  else
  {
    goto send_result;
  }
  file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_NUM_APDU_LENGTH;

  /* Parse APDU for switch and the corresponding Status Word */
  for(apdu_index = 0; apdu_index < mmgsdi_profile_info_ptr->num_apdu_switch; apdu_index++)
  {
    if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_LENGTH <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      apdu_data_len = *file_buffer_ptr;
      file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_LENGTH;

      if(file_buffer_ptr + apdu_data_len <= orig_file_buffer_ptr + multi_profile_file_size)
      {
        mmgsdi_profile_info_ptr->apdu_switch[apdu_index].data_len = apdu_data_len;

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          mmgsdi_profile_info_ptr->apdu_switch[apdu_index].data_ptr,
          apdu_data_len);
        if(mmgsdi_profile_info_ptr->apdu_switch[apdu_index].data_ptr == NULL)
        {
          goto send_result;
        }

        mmgsdi_memscpy(mmgsdi_profile_info_ptr->apdu_switch[apdu_index].data_ptr,
                       apdu_data_len,
                       file_buffer_ptr,
                       apdu_data_len);
        file_buffer_ptr += apdu_data_len;
      }
      else
      {
        goto send_result;
      }
    }
    else
    {
      goto send_result;
    }

    /* Parse Status word corresponding to this APDU */
    if(file_buffer_ptr + (MMGSDI_MULTI_PROFILE_CARD_STATUS_WORD_LENGTH * 2) <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      /* Switch profile status word for different profile*/
      mmgsdi_profile_info_ptr->sw_switch_diff_profile[apdu_index].sw1 = *file_buffer_ptr;
      mmgsdi_profile_info_ptr->sw_switch_diff_profile[apdu_index].sw2 = *(file_buffer_ptr + 1);

      /* Swich profile status word for same profile */
      mmgsdi_profile_info_ptr->sw_switch_same_profile[apdu_index].sw1 = *(file_buffer_ptr + 2);
      mmgsdi_profile_info_ptr->sw_switch_same_profile[apdu_index].sw2 = *(file_buffer_ptr + 3);
    }
    else
    {
      goto send_result;
    }
    file_buffer_ptr += (MMGSDI_MULTI_PROFILE_CARD_STATUS_WORD_LENGTH * 2);
  }

  /* Parse num APDUs for switch back */
  if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_NUM_APDU_LENGTH <=
       orig_file_buffer_ptr + multi_profile_file_size)
  {
    if(*file_buffer_ptr <= 0 ||
       *file_buffer_ptr > MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_TRANSACTIONS)
    {
      goto send_result;
    }

    mmgsdi_profile_info_ptr->num_apdu_switch_back = *file_buffer_ptr;
  }
  else
  {
    goto send_result;
  }
  file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_NUM_APDU_LENGTH;

  /*Parse APDU for switch back and the corresponding Status Word */
  for(apdu_index = 0; apdu_index < mmgsdi_profile_info_ptr->num_apdu_switch_back; apdu_index++)
  {
    if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_LENGTH <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      apdu_data_len = *file_buffer_ptr;
      file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_LENGTH;

      if(file_buffer_ptr + apdu_data_len <= orig_file_buffer_ptr + multi_profile_file_size)
      {
        mmgsdi_profile_info_ptr->apdu_switch_back[apdu_index].data_len = apdu_data_len;

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          mmgsdi_profile_info_ptr->apdu_switch_back[apdu_index].data_ptr,
          apdu_data_len);

        if(mmgsdi_profile_info_ptr->apdu_switch_back[apdu_index].data_ptr == NULL)
        {
          goto send_result;
        }

        mmgsdi_memscpy(mmgsdi_profile_info_ptr->apdu_switch_back[apdu_index].data_ptr,
                       apdu_data_len,
                       file_buffer_ptr,
                       apdu_data_len);
        file_buffer_ptr += apdu_data_len;
      }
      else
      {
        goto send_result;
      }
    }
    else
    {
      goto send_result;
    }

    /* Parse Status word corresponding to this APDU */
    if(file_buffer_ptr + (MMGSDI_MULTI_PROFILE_CARD_STATUS_WORD_LENGTH * 2) <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      /* Switch back profile status word for different profile*/
      mmgsdi_profile_info_ptr->sw_switch_back_diff_profile[apdu_index].sw1 = *file_buffer_ptr;
      mmgsdi_profile_info_ptr->sw_switch_back_diff_profile[apdu_index].sw2 = *(file_buffer_ptr + 1);

      /* Swich back profile status word for same profile */
      mmgsdi_profile_info_ptr->sw_switch_back_same_profile[apdu_index].sw1 = *(file_buffer_ptr + 2);
      mmgsdi_profile_info_ptr->sw_switch_back_same_profile[apdu_index].sw2 = *(file_buffer_ptr + 3);
    }
    else
    {
      goto send_result;
    }

    file_buffer_ptr += (MMGSDI_MULTI_PROFILE_CARD_STATUS_WORD_LENGTH * 2);
  }

  /* Parse switch action */
  if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_PROFILE_SWITCH_ACTION_LENGTH <=
       orig_file_buffer_ptr + multi_profile_file_size)
  {
    switch(*file_buffer_ptr)
    {
      case 0:
      case 2:
        mmgsdi_profile_info_ptr->switch_action = MMGSDI_MULTI_PROFILE_CARD_SWITCH_ACTION_UICC_RESET;
        break;
      case 1:
        mmgsdi_profile_info_ptr->switch_action = MMGSDI_MULTI_PROFILE_CARD_SWITCH_ACTION_REFRESH;
        break;
      default:
        goto send_result;
    }
  }
  else
  {
    goto send_result;
  }

  file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_PROFILE_SWITCH_ACTION_LENGTH;

  /* Parse dedicated channel needed */
  if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_PROFILE_DEDICATED_CH_LENGTH <=
       orig_file_buffer_ptr + multi_profile_file_size)
  {
    if(*file_buffer_ptr != 0 && *file_buffer_ptr != 1)
    {
      goto send_result;
    }
    mmgsdi_profile_info_ptr->dedicated_channel_needed = *file_buffer_ptr;
  }
  else
  {
    goto send_result;
  }

  file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_PROFILE_DEDICATED_CH_LENGTH;

  /* AID is applicable only if dedicated channel is needed */
  if(mmgsdi_profile_info_ptr->dedicated_channel_needed)
  {
    /* Parse AID length */
    if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_AID_LENGTH <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      mmgsdi_profile_info_ptr->aid.data_len = *file_buffer_ptr;
    }
    else
    {
      goto send_result;
    }

    file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_AID_LENGTH;

    /* Parse AID data */
    if(file_buffer_ptr + mmgsdi_profile_info_ptr->aid.data_len <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      mmgsdi_memscpy(mmgsdi_profile_info_ptr->aid.data_ptr,
                     sizeof(mmgsdi_profile_info_ptr->aid.data_ptr),
                     file_buffer_ptr,
                     mmgsdi_profile_info_ptr->aid.data_len);
      file_buffer_ptr += mmgsdi_profile_info_ptr->aid.data_len;
    }
    else
    {
      goto send_result;
    }

    /* Parse active profile availability APDU */
    if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_LENGTH <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      apdu_data_len = *file_buffer_ptr;
      file_buffer_ptr += MMGSDI_MULTI_PROFILE_CARD_MAX_APDU_LENGTH;

      if(file_buffer_ptr + apdu_data_len <= orig_file_buffer_ptr + multi_profile_file_size)
      {
        mmgsdi_profile_info_ptr->get_active_profile_apdu.data_len = apdu_data_len;

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          mmgsdi_profile_info_ptr->get_active_profile_apdu.data_ptr,
          apdu_data_len);

        if(mmgsdi_profile_info_ptr->get_active_profile_apdu.data_ptr == NULL)
        {
          goto send_result;
        }

        mmgsdi_memscpy(mmgsdi_profile_info_ptr->get_active_profile_apdu.data_ptr,
                       apdu_data_len,
                       file_buffer_ptr,
                       apdu_data_len);
        file_buffer_ptr += apdu_data_len;
      }
      else
      {
        goto send_result;
      }
    }
    else
    {
      goto send_result;
    }
  }
  else
  {
    /* Parse multi profile support check */
    if(file_buffer_ptr + MMGSDI_MULTI_PROFILE_CARD_MULTI_PROFILE_SUPPORT_LENGTH <=
         orig_file_buffer_ptr + multi_profile_file_size)
    {
      if(*file_buffer_ptr != 0 && *file_buffer_ptr != 1)
      {
        goto send_result;
      }
      mmgsdi_profile_info_ptr->check_multi_profile_support = *file_buffer_ptr;
    }
    else
    {
      goto send_result;
    }
  }

  /* EFS parsing is successful. Set the parse_status to TRUE */
  parse_status = TRUE;

  UIM_MSG_HIGH_0("Multi profile EFS parsing Successful");

send_result:

  /* Free file buffer pointer which was used to access the EFS data */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(orig_file_buffer_ptr);

  /* If the parsing fails then deep free the profile info global */
  if(!parse_status && mmgsdi_profile_info_ptr != NULL)
  {
    for(apdu_index = 0; apdu_index < mmgsdi_profile_info_ptr->num_apdu_switch; apdu_index++)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_profile_info_ptr->apdu_switch[apdu_index].data_ptr);
    }
    for(apdu_index = 0; apdu_index < mmgsdi_profile_info_ptr->num_apdu_switch_back; apdu_index++)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_profile_info_ptr->apdu_switch_back[apdu_index].data_ptr);
    }

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_profile_info_ptr->get_active_profile_apdu.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_profile_info_ptr);
  }
} /* mmgsdi_multi_profile_init */


/*===========================================================================
FUNCTION MMGSDI_MULTI_PROFILE_UPDATE_TO_SINGLE_PROFILE_INFO

DESCRIPTION
  Function to update number of profile supported to only 1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void mmgsdi_multi_profile_update_to_single_profile_info(
  void
)
{
  if(mmgsdi_profile_info_ptr != NULL)
  {
    mmgsdi_profile_info_ptr->num_profile                 = 0x01;
    mmgsdi_profile_info_ptr->active_profile.profile_id   = MMGSDI_PROFILE_1;
    mmgsdi_profile_info_ptr->active_profile.profile_type = MMGSDI_PROFILE_TYPE_REGULAR;
  }
} /* mmgsdi_multi_profile_update_to_single_profile_info */


/*===========================================================================
FUNCTION MMGSDI_MULTI_PROFILE_RESET_DATA

DESCRIPTION
  Function to reset derived multi profile information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void mmgsdi_multi_profile_reset_data(
  void
)
{
  if(mmgsdi_profile_info_ptr != NULL)
  {
    mmgsdi_profile_info_ptr->reg_profile_detected = FALSE;
    mmgsdi_profile_info_ptr->dedicated_channel_id = 0;
    mmgsdi_profile_info_ptr->is_ecall_active      = FALSE;
    mmgsdi_profile_info_ptr->is_emergency_refresh = FALSE;

    memset(&mmgsdi_profile_info_ptr->active_profile,
           0x00,
           sizeof(mmgsdi_profile_info_type));
  }
} /* mmgsdi_multi_profile_reset_data */


/*===========================================================================
FUNCTION MMGSDI_MULTI_PROFILE_SET_REGULAR_PROFILE

DESCRIPTION
  Determines the profile in the card and switches back to regular profile if required

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE: Send card inserted event
    FALSE: Do not send card inserted event

SIDE EFFECTS
  None.
===========================================================================*/
boolean mmgsdi_multi_profile_set_regular_profile(
  mmgsdi_slot_id_enum_type  slot_id
)
{
  mmgsdi_return_enum_type mmgsdi_status      = MMGSDI_SUCCESS;
  boolean                 send_card_inserted = FALSE;

  UIM_MSG_HIGH_0("mmgsdi_multi_profile_set_regular_profile");

  /* if multi profile pointer is NULL then we treat the card as single profile card */
  if(mmgsdi_profile_info_ptr == NULL)
  {
    return TRUE;
  }

  /* If dedicated channel is needed then open session with multi profile
     app. The profile determination will be done in callback of session open.
     In case no dedicated channel is required then queue set sim profile to
     determine the profile in the card */
  if(mmgsdi_profile_info_ptr->dedicated_channel_needed)
  {
    mmgsdi_non_prov_app_info_type non_prov_app_info;
    memset(&non_prov_app_info, 0x00, sizeof(mmgsdi_non_prov_app_info_type));

    non_prov_app_info.exclusive_channel = TRUE;
    non_prov_app_info.app_id_data.data_len = mmgsdi_profile_info_ptr->aid.data_len;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(non_prov_app_info.app_id_data.data_ptr,
                                       non_prov_app_info.app_id_data.data_len);
    if(non_prov_app_info.app_id_data.data_ptr == NULL)
    {
      return FALSE;
    }

    mmgsdi_memscpy(non_prov_app_info.app_id_data.data_ptr,
                   non_prov_app_info.app_id_data.data_len,
                   mmgsdi_profile_info_ptr->aid.data_ptr,
                   mmgsdi_profile_info_ptr->aid.data_len);

    /* Open a non-prov session (dedicated channel id) to get the
       current active profile and switch the profile */
    mmgsdi_status = mmgsdi_session_open(mmgsdi_generic_data_ptr->client_id,
                                        MMGSDI_NON_PROV_SESSION_SLOT_1,
                                        &non_prov_app_info,
                                        NULL,
                                        FALSE,
                                        mmgsdi_util_internal_cb,
                                        0);
    MMGSDIUTIL_TMC_MEM_FREE(non_prov_app_info.app_id_data.data_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Queuing has failed, update number of profiles to 0x01 and send card inserted event */
      mmgsdi_multi_profile_update_to_single_profile_info();
      send_card_inserted = TRUE;
    }
  }
  else
  {
    /* If regular profile is not detected then send the set sim
       profile command to the card otherwise send CARD_INSERTED event */
    if(mmgsdi_profile_info_ptr->reg_profile_detected == FALSE)
    {
      /* If multi profile check is required then send select of 
         0x2FC8 & 0x2FC9 EFs in the card */
      if(mmgsdi_profile_info_ptr->check_multi_profile_support)
      {
        mmgsdi_get_file_attr_cnf_type *select_cnf_ptr  = NULL;
        mmgsdi_get_file_attr_req_type *select_ptr      = NULL;
        uint8                          slot_index      = MMGSDI_SLOT_1_INDEX;

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
                                           sizeof(mmgsdi_get_file_attr_req_type));
        if(select_ptr == NULL)
        {
          return FALSE;
        }

       (void)mmgsdi_util_get_slot_index(slot_id ,&slot_index);

        select_ptr->request_header.client_id          = mmgsdi_generic_data_ptr->client_id;
        select_ptr->request_header.session_id         = mmgsdi_generic_data_ptr->slot_session_id[slot_index];
        select_ptr->request_header.request_type       = MMGSDI_GET_FILE_ATTR_REQ;
        select_ptr->request_header.orig_request_type  = MMGSDI_GET_FILE_ATTR_REQ;
        select_ptr->access.access_method              = MMGSDI_BY_PATH_ACCESS;
        select_ptr->access.file.path_type.path_len    = 0x02;
        select_ptr->access.file.path_type.path_buf[0] = MMGSDI_MF_DIR;
        select_ptr->access.file.path_type.path_buf[1] = 0x2FC8;
        select_ptr->request_header.slot_id            = slot_id;
        select_ptr->skip_uicc_arr                     = TRUE;

        mmgsdi_status = mmgsdi_uim_uicc_select(select_ptr,
                                               TRUE,
                                               &select_cnf_ptr,
                                               TRUE);
        if(mmgsdi_status != MMGSDI_SUCCESS ||
           select_cnf_ptr == NULL ||
           select_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS)
        {
          /* Select failed, update number of profiles to 0x01 and send card inserted event */
          mmgsdi_multi_profile_update_to_single_profile_info();
          send_card_inserted = TRUE;
        }
        else
        {
          /* Free select confirmation pointer for next select request for 0x2FC9 */
          mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
          MMGSDIUTIL_TMC_MEM_FREE(select_cnf_ptr);

          /* Update the path buffer with 0x2FC9 */
          select_ptr->access.file.path_type.path_buf[1] = 0x2FC9;

          mmgsdi_status = mmgsdi_uim_uicc_select(select_ptr,
                                                 TRUE,
                                                 &select_cnf_ptr,
                                                 TRUE);
          if(mmgsdi_status != MMGSDI_SUCCESS ||
             select_cnf_ptr == NULL ||
             select_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS)
          {
            /* Select failed, update number of profiles to 0x01 and send card inserted event */
            mmgsdi_multi_profile_update_to_single_profile_info();
            send_card_inserted = TRUE;
          }
          else
          {
            /* EF 2FC8 & EF 2FC9 are available. Update number of profiles to 0x02 */
            mmgsdi_profile_info_ptr->num_profile = 0x02;
          }
        }

        /* Free request and confirm pointers */
        mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(select_cnf_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(select_ptr);
      }

      /* If send_card_inserted is FALSE then 0x2FC8 & 0x2FC9 EFs are present.
         To make sure that card is in regular profile, send switch
         profile command to move to regular profile */
      if(send_card_inserted == FALSE)
      {
        mmgsdi_status = mmgsdi_set_sim_profile(mmgsdi_generic_data_ptr->client_id,
                                               slot_id,
                                               MMGSDI_PROFILE_1,
                                               mmgsdi_util_internal_cb,
                                               0);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          /* Queuing has failed, update number of profiles to 0x01 and send card inserted event */
          mmgsdi_multi_profile_update_to_single_profile_info();
          send_card_inserted = TRUE;
        }
      }
    }
    else
    {
      /* Regular profile is detected, send card inserted event */
      send_card_inserted = TRUE;
    }
  }

  return send_card_inserted;
}/* mmgsdi_multi_profile_set_regular_profile */

#endif /* FEATURE_UIM_MULTI_PROFILE */

