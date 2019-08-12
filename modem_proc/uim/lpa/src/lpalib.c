/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        L P A  L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that can be used to externally
  access the LPA.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpalib.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/19/16   ar      Add support for configuring HTTP stack via LPA EFS item
04/11/16   av      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"
#include <stringl/stringl.h>
#include "uim_common_efs.h"

#include "lpa_cmd.h"
#include "lpalib.h"
#include "lpa.h"
#include "lpa_platform.h"
#include "lpa_util.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*=============================================================================
FUNCTION        LPA_GET_PROFILES_INFO

DESCRIPTION
  This function allows the user to get complete information about all the
  profiles on the card.

PARAMETERS
  slot_id               :   Slot
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_get_profiles_info (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  lpa_result_enum_type         lpa_status     = LPA_GENERIC_ERROR;
  lpa_cmd_type                *lpa_cmd_ptr    = NULL;

  if(!lpa_util_is_slot_valid(slot_id) )
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_cmd_ptr = (lpa_cmd_type*)LPA_MALLOC(sizeof(lpa_cmd_type));
  if (lpa_cmd_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memset(lpa_cmd_ptr, 0x00, sizeof(lpa_cmd_type));

  lpa_cmd_ptr->lpa_cb_ptr                             = response_cb_ptr;
  lpa_cmd_ptr->user_data_ptr                          = user_data_ptr;
  lpa_cmd_ptr->slot_id                                = slot_id;
  lpa_cmd_ptr->cmd.cmd_enum                           = LPA_GET_PROFILES_INFO_MSG;

  lpa_status = lpa_cmd_queue_cmd(lpa_cmd_ptr, slot_id);
  if (lpa_status != LPA_SUCCESS)
  {
    /* Free the task pointer and data since the lpa_cmd_ptr has not
       been put onto the command queue */
    LPA_FREE(lpa_cmd_ptr);
    return lpa_status;
  }

  LPA_MSG_HIGH_1("Queue of command: LPA get profiles info: status 0x%x", lpa_status);

  return LPA_SUCCESS;
} /* lpa_get_profiles_info */


/*=============================================================================
FUNCTION        LPA_ENABLE_PROFILE

DESCRIPTION
  This function allows the user to enable a profile identified by the ICCID
  on the card.
PARAMETERS
  slot_id               :   Slot
  iccid                 :   ICCID of the profile on the card
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_enable_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  lpa_result_enum_type         lpa_status     = LPA_GENERIC_ERROR;
  lpa_cmd_type                *lpa_cmd_ptr    = NULL;

  if(!lpa_util_is_slot_valid(slot_id) ||
     !lpa_util_is_iccid_valid(iccid))
  {
    LPA_MSG_ERR_2(
      "lpa_enable_profile: invalid parameter(s) - slot_id=0x%x, iccid len=0x%x",
      slot_id, iccid.data_len);
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_cmd_ptr = (lpa_cmd_type*)LPA_MALLOC(sizeof(lpa_cmd_type));
  if (lpa_cmd_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  if (!lpa_util_is_slot_valid(slot_id) ||
      !lpa_util_is_iccid_valid(iccid))
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  memset(lpa_cmd_ptr, 0x00, sizeof(lpa_cmd_type));

  lpa_cmd_ptr->lpa_cb_ptr                             = response_cb_ptr;
  lpa_cmd_ptr->user_data_ptr                          = user_data_ptr;
  lpa_cmd_ptr->slot_id                                = slot_id;
  lpa_cmd_ptr->cmd.cmd_enum                           = LPA_ENABLE_PROFILE_MSG;
  lpa_cmd_ptr->cmd.cmd.enable_profile_req.iccid       = iccid;

  lpa_status = lpa_cmd_queue_cmd(lpa_cmd_ptr, slot_id);
  if (lpa_status != LPA_SUCCESS)
  {
    /* Free the task pointer and data since the lpa_cmd_ptr has not
       been put onto the command queue */
    LPA_FREE(lpa_cmd_ptr);
    return lpa_status;
  }

  LPA_MSG_HIGH_1("Queue of command: LPA enable profile: status 0x%x", lpa_status);

  return LPA_SUCCESS;
} /* lpa_enable_profile */


/*=============================================================================
FUNCTION        LPA_DISABLE_PROFILE

DESCRIPTION
  This function allows the user to disable a profile identified by the ICCID
  on the card.
PARAMETERS
  slot_id               :   Slot
  iccid                 :   ICCID of the profile on the card
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_disable_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  lpa_result_enum_type         lpa_status     = LPA_GENERIC_ERROR;
  lpa_cmd_type                *lpa_cmd_ptr    = NULL;

  if(!lpa_util_is_slot_valid(slot_id) ||
     !lpa_util_is_iccid_valid(iccid))
  {
    LPA_MSG_ERR_2(
      "lpa_disable_profile: invalid parameter(s) - slot_id=0x%x, iccid len=0x%x",
      slot_id, iccid.data_len);
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_cmd_ptr = (lpa_cmd_type*)LPA_MALLOC(sizeof(lpa_cmd_type));
  if (lpa_cmd_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memset(lpa_cmd_ptr, 0x00, sizeof(lpa_cmd_type));

  lpa_cmd_ptr->lpa_cb_ptr                             = response_cb_ptr;
  lpa_cmd_ptr->user_data_ptr                          = user_data_ptr;
  lpa_cmd_ptr->slot_id                                = slot_id;
  lpa_cmd_ptr->cmd.cmd_enum                           = LPA_DISABLE_PROFILE_MSG;
  lpa_cmd_ptr->cmd.cmd.disable_profile_req.iccid      = iccid;

  lpa_status = lpa_cmd_queue_cmd(lpa_cmd_ptr, slot_id);
  if (lpa_status != LPA_SUCCESS)
  {
    /* Free the task pointer and data since the lpa_cmd_ptr has not
       been put onto the command queue */
    LPA_FREE(lpa_cmd_ptr);
    return lpa_status;
  }

  LPA_MSG_HIGH_1("Queue of command: LPA disable profile: status 0x%x", lpa_status);

  return LPA_SUCCESS;
} /* lpa_disable_profile */


/*=============================================================================
FUNCTION        LPA_DELETE_PROFILE

DESCRIPTION
  This function allows the user to delete a profile identified by the ICCID
  on the card.

PARAMETERS
  slot_id               :   Slot
  iccid                 :   ICCID of the profile on the card
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_delete_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  lpa_result_enum_type         lpa_status     = LPA_GENERIC_ERROR;
  lpa_cmd_type                *lpa_cmd_ptr    = NULL;

  if(!lpa_util_is_slot_valid(slot_id) ||
     !lpa_util_is_iccid_valid(iccid))
  {
    LPA_MSG_ERR_2(
      "lpa_delete_profile: invalid parameter(s) - slot_id=0x%x, iccid len=0x%x",
      slot_id, iccid.data_len);
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_cmd_ptr = (lpa_cmd_type*)LPA_MALLOC(sizeof(lpa_cmd_type));
  if (lpa_cmd_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memset(lpa_cmd_ptr, 0x00, sizeof(lpa_cmd_type));

  lpa_cmd_ptr->lpa_cb_ptr                             = response_cb_ptr;
  lpa_cmd_ptr->user_data_ptr                          = user_data_ptr;
  lpa_cmd_ptr->slot_id                                = slot_id;
  lpa_cmd_ptr->cmd.cmd_enum                           = LPA_DELETE_PROFILE_MSG;
  lpa_cmd_ptr->cmd.cmd.delete_profile_req.iccid       = iccid;

  lpa_status = lpa_cmd_queue_cmd(lpa_cmd_ptr, slot_id);
  if (lpa_status != LPA_SUCCESS)
  {
    /* Free the task pointer and data since the lpa_cmd_ptr has not
       been put onto the command queue */
    LPA_FREE(lpa_cmd_ptr);
    return lpa_status;
  }

  LPA_MSG_HIGH_1("Queue of command: LPA delete profile: status 0x%x", lpa_status);

  return LPA_SUCCESS;
} /* lpa_delete_profile */


/*=============================================================================
FUNCTION        LPA_CARD_MEMORY_RESET

DESCRIPTION
  This function deletes all the installed profiles, regardless of their state,
  from the card.
PARAMETERS
  slot_id               :   Slot
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_card_memory_reset (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  lpa_result_enum_type         lpa_status     = LPA_GENERIC_ERROR;
  lpa_cmd_type                *lpa_cmd_ptr    = NULL;

  if(!lpa_util_is_slot_valid(slot_id) )
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_cmd_ptr = (lpa_cmd_type*)LPA_MALLOC(sizeof(lpa_cmd_type));
  if (lpa_cmd_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memset(lpa_cmd_ptr, 0x00, sizeof(lpa_cmd_type));

  lpa_cmd_ptr->lpa_cb_ptr                             = response_cb_ptr;
  lpa_cmd_ptr->user_data_ptr                          = user_data_ptr;
  lpa_cmd_ptr->slot_id                                = slot_id;
  lpa_cmd_ptr->cmd.cmd_enum                           = LPA_CARD_MEMORY_RESET_MSG;

  lpa_status = lpa_cmd_queue_cmd(lpa_cmd_ptr, slot_id);
  if (lpa_status != LPA_SUCCESS)
  {
    /* Free the task pointer and data since the lpa_cmd_ptr has not
       been put onto the command queue */
    LPA_FREE(lpa_cmd_ptr);
    return lpa_status;
  }

  LPA_MSG_HIGH_1("Queue of command: LPA card memory reset: status 0x%x", lpa_status);

  return LPA_SUCCESS;
} /* lpa_card_memory_reset */


/*=============================================================================
FUNCTION        LPA_GET_EID

DESCRIPTION
  This function allows the user to retrieve the EID of the card.

PARAMETERS
  slot_id               :   Slot
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_get_eid (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  lpa_result_enum_type         lpa_status     = LPA_GENERIC_ERROR;
  lpa_cmd_type                *lpa_cmd_ptr    = NULL;

  if(!lpa_util_is_slot_valid(slot_id))
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_cmd_ptr = (lpa_cmd_type*)LPA_MALLOC(sizeof(lpa_cmd_type));
  if (lpa_cmd_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memset(lpa_cmd_ptr, 0x00, sizeof(lpa_cmd_type));

  lpa_cmd_ptr->lpa_cb_ptr                           = response_cb_ptr;
  lpa_cmd_ptr->user_data_ptr                        = user_data_ptr;
  lpa_cmd_ptr->slot_id                              = slot_id;
  lpa_cmd_ptr->cmd.cmd_enum                         = LPA_GET_EID_MSG;

  lpa_status = lpa_cmd_queue_cmd(lpa_cmd_ptr, slot_id);
  if (lpa_status != LPA_SUCCESS)
  {
    /* Free the task pointer and data since the lpa_cmd_ptr has not
       been put onto the command queue */
    LPA_FREE(lpa_cmd_ptr);
    return lpa_status;
  }

  LPA_MSG_HIGH_1("Queue of command: LPA get EID: status 0x%x", lpa_status);

  return LPA_SUCCESS;
} /* lpa_get_eid */


/*=============================================================================
FUNCTION        LPA_ADD_PROFILE

DESCRIPTION
  This function allows the user to download a profile from the SMDP server
  to the card. If the activation code suggests the confirmation code is
  needed, and no confirmation code is passed in to this API, the API call
  returns error.

PARAMETERS
  slot_id                 :   Slot
  activation_code         :   Activation code that is sent to the server to
                              download a profile. Max length is 255 characters
                              per SGP.22.
  confirmation_code       :   Confirmation code, if mandated by the
                              activation_code.
  response_cb_ptr         :   Command response call back
  event_cb_ptr            :   Event callback called every n time, with the
                              profile download progress
  user_data_ptr           :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:                          The command structure was properly
                                          generated and queued onto the LPA
                                          command queue.
    LPA_ERROR_INCORRECT_PARAMS:                 The parameters supplied to the API
                                          are not within appropriate ranges.
    LPA_ERROR_CONFIRMATION_CODE_REQUIRED: Confirmation code is required but
                                          not provided by the caller.
    LPA_GENERIC_ERROR:                    The command was not queued to the LPA
                                          command queue.
===============================================================================*/
lpa_result_enum_type  lpa_add_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_data_type                         activation_code,
  lpa_data_type                         confirmation_code,
  lpa_callback_type                     response_cb_ptr,
  lpa_event_callback_type               event_cb_ptr,
  void                                 *user_data_ptr
)
{
  lpa_result_enum_type         lpa_status                 = LPA_GENERIC_ERROR;
  lpa_cmd_type                *lpa_cmd_ptr                = NULL;

  if(!lpa_util_is_slot_valid(slot_id) ||
     activation_code.data_len == 0||
     activation_code.data_ptr == NULL)
  {
    LPA_MSG_ERR_2(
      "lpa_add_profile: invalid parameter(s) - slot_id=0x%x, activation code len",
      slot_id, activation_code.data_len);
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_cmd_ptr = (lpa_cmd_type*)LPA_MALLOC(sizeof(lpa_cmd_type));
  if (lpa_cmd_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memset(lpa_cmd_ptr, 0x00, sizeof(lpa_cmd_type));

  /* Copy over the activation code */
  lpa_cmd_ptr->cmd.cmd.add_profile_req.activation_code.data_ptr =
    LPA_MALLOC(activation_code.data_len);
  if(lpa_cmd_ptr->cmd.cmd.add_profile_req.activation_code.data_ptr == NULL)
  {
    LPA_FREE(lpa_cmd_ptr);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }
  lpa_cmd_ptr->cmd.cmd.add_profile_req.activation_code.data_len =
    activation_code.data_len;

  memscpy(lpa_cmd_ptr->cmd.cmd.add_profile_req.activation_code.data_ptr,
          lpa_cmd_ptr->cmd.cmd.add_profile_req.activation_code.data_len,
          activation_code.data_ptr,
          activation_code.data_len);

  /* Copy over the convirmation code, if any */
  if(confirmation_code.data_len > 0 &&
     confirmation_code.data_ptr)
  {
    /* It is possible that confirmation code is zero and the add profile
       request is still a valid one. */
    lpa_cmd_ptr->cmd.cmd.add_profile_req.confirmation_code.data_ptr =
    LPA_MALLOC(confirmation_code.data_len);

    if(lpa_cmd_ptr->cmd.cmd.add_profile_req.confirmation_code.data_ptr == NULL)
    {
      LPA_FREE(lpa_cmd_ptr->cmd.cmd.add_profile_req.activation_code.data_ptr);
      LPA_FREE(lpa_cmd_ptr);
      return LPA_ERROR_HEAP_EXHAUSTED;
    }
    lpa_cmd_ptr->cmd.cmd.add_profile_req.confirmation_code.data_len =
      confirmation_code.data_len;
    memscpy(lpa_cmd_ptr->cmd.cmd.add_profile_req.confirmation_code.data_ptr,
          lpa_cmd_ptr->cmd.cmd.add_profile_req.confirmation_code.data_len,
          confirmation_code.data_ptr,
          confirmation_code.data_len);
  }
  /* Whether activation code is a valid activation code and whether the
     confirmation code is needed and provided with this API or not, all
     of that is determined in LPA task context. Error is returned via the
     client callback if there is any issue with activation code or the
     confirmation code */

  lpa_cmd_ptr->lpa_cb_ptr                           = response_cb_ptr;
  lpa_cmd_ptr->event_cb_ptr                         = event_cb_ptr;
  lpa_cmd_ptr->user_data_ptr                        = user_data_ptr;
  lpa_cmd_ptr->slot_id                              = slot_id;
  lpa_cmd_ptr->cmd.cmd_enum                         = LPA_ADD_PROFILE_MSG;

  lpa_status = lpa_cmd_queue_cmd(lpa_cmd_ptr, slot_id);
  if (lpa_status != LPA_SUCCESS)
  {
    /* Free the task pointer and data since the lpa_cmd_ptr has not
       been put onto the command queue */
    LPA_FREE(lpa_cmd_ptr->cmd.cmd.add_profile_req.activation_code.data_ptr);
    LPA_FREE(lpa_cmd_ptr->cmd.cmd.add_profile_req.confirmation_code.data_ptr);
    LPA_FREE(lpa_cmd_ptr);
    return lpa_status;
  }

  LPA_MSG_HIGH_1("Queue of command: LPA add profile: status 0x%x", lpa_status);

  return LPA_SUCCESS;
} /* lpa_add_profile */


/*=============================================================================
FUNCTION        LPA_ADD_OR_UPDATE_PROFILE_NICKNAME

DESCRIPTION
  This function allows the user to add or update the nickname of a profile.

PARAMETERS
  slot_id               :   Slot
  iccid                 :   ICCID of the target profile
  nickname              :   Nickname of the target profile. Max length can be
                            64 characters per SGP.22
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_add_or_update_profile_nickname (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_name_type                         nickname,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  lpa_result_enum_type         lpa_status                 = LPA_GENERIC_ERROR;
  lpa_cmd_type                *lpa_cmd_ptr                = NULL;

  if(!lpa_util_is_slot_valid(slot_id) ||
     !lpa_util_is_iccid_valid(iccid))
  {
    LPA_MSG_ERR_2(
      "lpa_add_or_update_profile_nickname: invalid parameter(s) - slot_id=0x%x, iccid len=0x%x",
      slot_id, iccid.data_len);
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_cmd_ptr = (lpa_cmd_type*)LPA_MALLOC(sizeof(lpa_cmd_type));
  if (lpa_cmd_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memset(lpa_cmd_ptr, 0x00, sizeof(lpa_cmd_type));

  lpa_cmd_ptr->lpa_cb_ptr                                          = response_cb_ptr;
  lpa_cmd_ptr->user_data_ptr                                       = user_data_ptr;
  lpa_cmd_ptr->slot_id                                             = slot_id;
  lpa_cmd_ptr->cmd.cmd_enum                                        = LPA_ADD_OR_UPDATE_PROFILE_NICKNAME_MSG;
  lpa_cmd_ptr->cmd.cmd.add_or_update_profile_nickname_req.iccid    = iccid;
  lpa_cmd_ptr->cmd.cmd.add_or_update_profile_nickname_req.nickname = nickname;

  lpa_status = lpa_cmd_queue_cmd(lpa_cmd_ptr, slot_id);
  if (lpa_status != LPA_SUCCESS)
  {
    /* Free the task pointer and data since the lpa_cmd_ptr has not
       been put onto the command queue */
    LPA_FREE(lpa_cmd_ptr);
    return lpa_status;
  }

  LPA_MSG_HIGH_1("Queue of command: LPA add or update profile nickname: status 0x%x", lpa_status);

  return LPA_SUCCESS;
} /* lpa_add_or_update_profile_nickname */


/*=============================================================================
FUNCTION        LPA_IS_LPA_SUPPORTED

DESCRIPTION
  Indicates if LPD and LUI are supported or not. This info is needed to be
  sent to the card in the terminal capability.

PARAMETERS
  lpd_supported_ptr     :   Boolean to indicate if lpd is supported
  lui_supported_ptr     :   Boolean to indicate if lui is supported

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:                The request was processed successfully
    LPA_ERROR_INCORRECT_PARAMS: The parameters supplied to the API are not within
                                appropriate ranges.
    LPA_GENERIC_ERROR:          Some error while processing the request
===============================================================================*/
lpa_result_enum_type lpa_is_lpa_supported(
  boolean                            *lpd_supported_ptr,
  boolean                            *lui_supported_ptr
)
{
  uint8                        lpa_support_nv_content[UIM_COMMON_LPA_EFS_ITEM_LEN];
  uim_common_efs_context_type  efs_context               = UIM_COMMON_EFS_CONTEXT_0;

  memset(&lpa_support_nv_content, 0x00, sizeof(lpa_support_nv_content));

  if(lpd_supported_ptr == NULL ||
     lui_supported_ptr == NULL)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  *lpd_supported_ptr = TRUE;
  *lui_supported_ptr = TRUE;

  /* Read new EFS item here which determines if LPD and LUI are supported
     or not */
  if ((uim_common_efs_read(UIM_COMMON_EFS_LPA_SUPPORT,
                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                           efs_context,
                           (uint8 *)lpa_support_nv_content,
                           sizeof(lpa_support_nv_content)) == UIM_COMMON_EFS_SUCCESS))
  {
    *lpd_supported_ptr = lpa_support_nv_content[UIM_COMMON_LPA_EFS_ITEM_LPD_SUPPORT_INDEX];
    *lui_supported_ptr = lpa_support_nv_content[UIM_COMMON_LPA_EFS_ITEM_LUI_SUPPORT_INDEX];
  }

  return LPA_SUCCESS;
} /* lpa_is_lpa_supported */


/*=============================================================================
FUNCTION        LPA_SET_QMI_UIM_HTTP_SIG

DESCRIPTION
  This function sets the passed signal to the LPA task

PARAMETERS
  None

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:                The request was processed successfully
    LPA_ERROR_INCORRECT_PARAMS: The parameters supplied to the API are not within
                                appropriate ranges.
===============================================================================*/
lpa_result_enum_type lpa_set_qmi_uim_http_cmd_q_sig()
{
  (void) rex_set_sigs(UIM_LPA_TCB,QMI_UIM_HTTP_CMD_Q_SIG);
  return LPA_SUCCESS;
}