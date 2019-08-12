/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        L P A   L I B R A R Y  S T U B S


GENERAL DESCRIPTION

  This file contains stubs for library function that can be used to externally
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_stubs.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/11/16   av      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "rcinit.h"
#include "lpa_platform.h"
#include "lpalib.h"

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
    LPA_SUCCESS:             The command structure was properly generated and
                             queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                             appropriate ranges.
    LPA_GENERIC_ERROR:       The command was not queued to the LPA command
                             queue.
    LPA_ERROR_NOT_SUPPORTED: LPA is not supported.
===============================================================================*/
lpa_result_enum_type  lpa_get_profiles_info (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  (void) slot_id;
  (void) response_cb_ptr;
  (void) user_data_ptr;
  LPA_MSG_HIGH_0("LPA not supported!");

  return LPA_ERROR_NOT_SUPPORTED;
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
    LPA_SUCCESS:             The command structure was properly generated and
                             queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                             appropriate ranges.
    LPA_GENERIC_ERROR:       The command was not queued to the LPA command
                             queue.
    LPA_ERROR_NOT_SUPPORTED: LPA is not supported.
===============================================================================*/
lpa_result_enum_type  lpa_enable_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  (void) slot_id;
  (void) iccid;
  (void) response_cb_ptr;
  (void) user_data_ptr;
  LPA_MSG_HIGH_0("LPA not supported!");

  return LPA_ERROR_NOT_SUPPORTED;
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
    LPA_SUCCESS:             The command structure was properly generated and
                             queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                             appropriate ranges.
    LPA_GENERIC_ERROR:       The command was not queued to the LPA command
                             queue.
    LPA_ERROR_NOT_SUPPORTED: LPA is not supported.
===============================================================================*/
lpa_result_enum_type  lpa_disable_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  (void) slot_id;
  (void) iccid;
  (void) response_cb_ptr;
  (void) user_data_ptr;
  LPA_MSG_HIGH_0("LPA not supported!");

  return LPA_ERROR_NOT_SUPPORTED;
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
    LPA_SUCCESS:             The command structure was properly generated and
                             queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                             appropriate ranges.
    LPA_GENERIC_ERROR:       The command was not queued to the LPA command
                             queue.
    LPA_ERROR_NOT_SUPPORTED: LPA is not supported.
===============================================================================*/
lpa_result_enum_type  lpa_delete_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  (void) slot_id;
  (void) iccid;
  (void) response_cb_ptr;
  (void) user_data_ptr;
  LPA_MSG_HIGH_0("LPA not supported!");

  return LPA_ERROR_NOT_SUPPORTED;
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
    LPA_SUCCESS:             The command structure was properly generated and
                             queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                             appropriate ranges.
    LPA_GENERIC_ERROR:       The command was not queued to the LPA command
                             queue.
    LPA_ERROR_NOT_SUPPORTED: LPA is not supported.
===============================================================================*/
lpa_result_enum_type  lpa_card_memory_reset (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  (void) slot_id;
  (void) response_cb_ptr;
  (void) user_data_ptr;
  LPA_MSG_HIGH_0("LPA not supported!");

  return LPA_ERROR_NOT_SUPPORTED;
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
    LPA_SUCCESS:             The command structure was properly generated and
                             queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                             appropriate ranges.
    LPA_GENERIC_ERROR:       The command was not queued to the LPA command
                             queue.
    LPA_ERROR_NOT_SUPPORTED: LPA is not supported.
===============================================================================*/
lpa_result_enum_type  lpa_get_eid (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  (void) slot_id;
  (void) response_cb_ptr;
  (void) user_data_ptr;
  LPA_MSG_HIGH_0("LPA not supported!");

  return LPA_ERROR_NOT_SUPPORTED;
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
    LPA_ERROR_NOT_SUPPORTED:              LPA is not supported.
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
  (void) slot_id;
  (void) activation_code;
  (void) confirmation_code;
  (void) response_cb_ptr;
  (void) event_cb_ptr;
  (void) user_data_ptr;
  LPA_MSG_HIGH_0("LPA not supported!");

  return LPA_ERROR_NOT_SUPPORTED;
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
    LPA_SUCCESS:             The command structure was properly generated and
                             queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                             appropriate ranges.
    LPA_GENERIC_ERROR:       The command was not queued to the LPA command
                             queue.
    LPA_ERROR_NOT_SUPPORTED: LPA is not supported.
===============================================================================*/
lpa_result_enum_type  lpa_add_or_update_profile_nickname (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_name_type                         nickname,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
)
{
  (void) slot_id;
  (void) iccid;
  (void) nickname;
  (void) response_cb_ptr;
  (void) user_data_ptr;
  LPA_MSG_HIGH_0("LPA not supported!");

  return LPA_ERROR_NOT_SUPPORTED;
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
  if(lpd_supported_ptr == NULL ||
     lui_supported_ptr == NULL)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  *lpd_supported_ptr = FALSE;
  *lui_supported_ptr = FALSE;

  return LPA_SUCCESS;
} /* lpa_is_lpa_supported */


/*===========================================================================
FUNCTION LPA_TASK_INIT

DESCRIPTION
  Function called to initialize the LPA task queue and globals

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None. Function should only be called once during task initialization.
  On targets with RCInit enabled, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void lpa_task_init (
  void
)
{
  return;
} /* lpa_task_init */


/*===========================================================================
FUNCTION LPA_MAIN

DESCRIPTION
  Entry point for the LPA task. This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_main (
  dword dummy
)
{
  (void) dummy;

  rcinit_handshake_startup();
  (void)rcevt_signal_name("LPA:READY");
} /* lpa_main */
