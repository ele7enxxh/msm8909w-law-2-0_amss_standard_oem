/*===========================================================================


                     U I M   S U B S C R I P T I O N  M A N A G E R

DESCRIPTION
  This contains all the declarations for the UIM Subscription Manager.
  The Subscription Manager allows Dual Slots and two cards simultaneously
  while making the presence of only one known to outside clients.  This
  way subscription information from only one card is published at a time and
  only one card is used by the rest of the modem.

Copyright (c)2012-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimsub_manager.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/16   na      Remove memory free in subscription manager report callback
06/06/16   dd      F3 Optimization on curr path
05/18/16   ssr     Remove msg for malloc failure
09/08/15   ll      UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
08/12/15   ks      UIM votes for sleep while powering down inactive slot.
12/05/14   ks      Clearing path and POLL voting upon card insertion
09/18/14   akv     UIMDRV UT enhancements
08/25/14   ll      Fix compile warnings medium
08/14/14   sam     Add support for silent PIN verification for extended recovery
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
01/29/14   sam     Usage of newer style diag macros
02/05/14   sam     Fix for klocwork issues
12/27/13   ks      Subscription manager review fixes
12/21/13   am      Silent Recover Feature Implementation
09/30/13   akv     Removal of multiple efs reads performed by uim tasks
09/24/13   nmb     Remove extra uim_free param
09/17/13   ssr     Added support for new recovery notification for GSTK
09/12/13   js      Fixed compilation error
09/04/13   ks      Subscription manager - Added CARD_STATUS_UNKNOWN
08/13/31   js      UIMDRV Memory De-Couple changes
08/08/13   ks      Advanced Subscription Manager changes
06/19/13   ms      Subscription Manager changes
05/21/12   nmb     UIM Dual SIM Subscription Manager Initial Revision
===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_exp.h"
#include "gstk_p.h"
#include "mmgsdilib_p.h"
#include "uimsub_manager.h"
#include "fs_fcntl.h"
#include "fs_public.h"
#include "uimglobals.h"
#include "uim_hotswap.h"
#include "uimdrv_main.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR CARD MANAGER

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

uim_sm_data_type          uim_sm_data                   = {0};
uim_active_slot_cfg_type  uim_active_slot_configuration = {0};


/*===========================================================================
                       FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
   FUNCTION:      UIM_SM_MMGSDI_GET_RECOVERY_DATA_SYNC

   DESCRIPTION:
     Wrapper for the appropriate mmgsdi get recovery data function depending
     on the card manager settings

   DEPENDENCIES:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type uim_sm_mmgsdi_get_recovery_data_sync(
  uim_recovery_data_type *recovery_data_ptr,
  uim_slot_type           uim_slot_id
)
{
#ifndef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  return mmgsdi_get_recovery_data_sync(recovery_data_ptr,
                                       uim_slot_id);
#else
  return mmgsdi_get_recovery_data_sync(recovery_data_ptr,
                                       UIM_SLOT_1);
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
} /* uim_sm_mmgsdi_get_recovery_data_sync */


#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
/*===========================================================================

FUNCTION UIM_SM_RPT_FUNCTION

DESCRIPTION
  Report function to map UIM client calls to the active slot

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  All UIM client commands are sent to the active slot and all reports
  back to UIM clients are reported as slot1

===========================================================================*/
void uim_sm_rpt_function (
  uim_rpt_type *report_ptr
)
{
  uim_instance_global_type *uim_ptr = NULL;

  uim_sm_cmd_saved_data_type *cm_saved_client_info = NULL;

  if(NULL == report_ptr)
  {
    return;
  }

  /* Get the saved data we stored during the cmd translation */
  cm_saved_client_info = (uim_sm_cmd_saved_data_type*)report_ptr->user_data;

  if(cm_saved_client_info == NULL)
  {
    return;
  }

  /* Restore user data and always report as SLOT_1 to clients of UIM */
  report_ptr->user_data = cm_saved_client_info->userdata;
  report_ptr->slot      = UIM_SLOT_1;

  /* Execute the callback saved from the client */
  if(cm_saved_client_info->rpt_function != NULL)
  {
    (*(cm_saved_client_info->rpt_function))(report_ptr);
  }

  uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_1);
  /* free the card manager table entry */
  UIM_FREE(cm_saved_client_info);
  report_ptr->user_data = 0;
} /* uim_sm_rpt_function */
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */


/*============================================================================
   FUNCTION:      UIM_SM_MMGSDI_NOTIFY_CARD_ERROR

   DESCRIPTION:
     Wrapper for mmgsdi_notify_card_error: Notifies MMGSDI of any Card Error
     and the cause.  If card manager is enabled only notifies MMGSDI if the
     error is associated with the currently active slot.

   DEPENDENCIES:
     None

   RETURN VALUE:
     None
============================================================================*/
void uim_sm_mmgsdi_notify_card_error(
  uim_status_type           error_condition,
  uim_instance_global_type *uim_ptr
)
{
#ifndef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  if((TRUE == uim_ptr->extended_recovery.extended_recovery_enabled) && 
     (TRUE == uim_ptr->extended_recovery.extended_recovery_in_progress))
  {
    if(FALSE == uim_ptr->flag.temporary_card_error_notified)
    {
      /*this is the first time we are notifying after all the recoveries exhausted.
        since extended recovery is enabled send temporary error to mmgsdi for 
        caching pin data*/
      (void)mmgsdi_notify_card_error( error_condition, (uim_slot_type)(uim_ptr->id + 1), TRUE);
      uim_ptr->flag.temporary_card_error_notified = TRUE;
    }
  }
  else
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id ,"notifying permanent card error");
    uim_ptr->flag.temporary_card_error_notified = FALSE;
    (void)mmgsdi_notify_card_error( error_condition, (uim_slot_type)(uim_ptr->id + 1), FALSE);
  }
#else

  UIMDRV_MSG_HIGH_0(uim_ptr->id ,"Notifying QMI the latest slot status");
  uim_sm_notify_qmi_slot_status(uim_ptr);

  rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
  if(UIM_SLOT_STATE_ACTIVE ==
     uim_sm_data.uim_slot_info[uim_ptr->id].activity_status)
  {
    /* Always use SLOT1 for reporting to UIM clients */
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Notifying MMGSDI for current active slot 0x%x",
                      (uim_slot_type)(uim_ptr->id + 1));
    if((TRUE == uim_ptr->extended_recovery.extended_recovery_enabled) && 
       (TRUE == uim_ptr->extended_recovery.extended_recovery_in_progress))
    {
      if(FALSE == uim_ptr->flag.temporary_card_error_notified)
      {
        /*this is the first time we are notifying after all the recoveries exhausted.
          since extended recovery is enabled send temporary error to mmgsdi for 
          caching pin data*/
        UIMDRV_MSG_HIGH_0(uim_ptr->id ,"extended recovery to be triggered, notifying temporary card error for mmgsdi to cache puin data");
        (void)mmgsdi_notify_card_error( error_condition, UIM_SLOT_1, TRUE);
        uim_ptr->flag.temporary_card_error_notified = TRUE;
      }
      else
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id ,"not notifying command error as this is an intermedeat powerup");
      }
    }
    else
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id ,"notifying permanent card error");
      uim_ptr->flag.temporary_card_error_notified = FALSE;
      (void)mmgsdi_notify_card_error( error_condition, UIM_SLOT_1, FALSE);
    }
  }
  else if (uim_ptr->state.status != UIM_ERR_S)
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,
                 "Power down the non-active slot 0x%x ",
                      (uim_slot_type)(uim_ptr->id + 1));
    /*Set the secondary slot status to error state and power-down */
    /* This scenario hits during me_powerup when both slots are powered-up
       and secondary slot needs to be powered-down */
    uim_ptr->state.status = UIM_ERR_S;
    uim_power_down(uim_ptr);
    uim_hotswap_reset_slot(uim_ptr);
  }
  else
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,
                 "Inactive slot 0x%x is already in power-down error state ",
                      (uim_slot_type)(uim_ptr->id + 1));
  }
  rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
} /* uim_sm_mmgsdi_notify_card_error */


/*===========================================================================
FUNCTION UIM_SM_GSTK_UIM_SET_RECOVER_SIG

DESCRIPTION
  This function is the uim card manager wrapper called by UIM to set
  Recovery signal for GSTK

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void uim_sm_gstk_uim_set_recovery_sig(
  gstk_slot_id_enum_type slot_id
)
{
  uim_instance_global_type   *uim_ptr  = NULL;

#ifndef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  gstk_send_recovery_cmd((uim_slot_type)slot_id, UIMDRV_RECOVERY_STAGE_START);
#else
  gstk_send_recovery_cmd((uim_slot_type)GSTK_SLOT_1, UIMDRV_RECOVERY_STAGE_START);
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

  /* Notify the RECOVERY EVENT if callback is registered for SILENT RECOVERY */
  uim_ptr = uim_get_instance_ptr((uim_instance_enum_type)(slot_id -1));
  
  if(uim_ptr == NULL)
  {
    return;
  }
  
  if(NULL != uim_ptr->silent_recover.uim_silent_recovery_callback) 
  {
    uim_ptr->silent_recover.uim_silent_recovery_callback((uim_slot_type)(slot_id),
                                                         (const void *)uim_ptr->silent_recover.user_data,
                                                         UIMDRV_RECOVERY_STAGE_START);
  }
    
} /* uim_sm_gstk_uim_set_recovery_sig */


/*===========================================================================
FUNCTION UIM_SM_LINK_ESTABLISHED

DESCRIPTION
  This function is the uim card manager wrapper called by UIM to set
  send the link established to MMGSDI.  When Card Manager is actve the
  preferences are set here and CARD ERROR is sent if no cards are inserted.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void uim_sm_link_established(uim_instance_global_type *uim_ptr)
{
#ifndef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  if (uim_ptr->state.status != UIM_ERR_S)
  {
  
  if((TRUE == uim_ptr->extended_recovery.extended_recovery_enabled) && 
     (TRUE == uim_ptr->extended_recovery.extended_recovery_in_progress))
  {
    /*extended powerup is succesful. call extended_recovery_evt_handler
      to clear extended recovery globals*/
    uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TERMINATE);
    uim_ptr->flag.temporary_card_error_notified = FALSE;
  }
    /* Send a link established messgae to GSDI */
    uim_gsdi_link_est(uim_ptr);
  }
#else
  if(uim_ptr->command.cmd_ptr != NULL)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"command pointer is NULL");
    if(uim_ptr->command.cmd_ptr->hdr.req_type == UIM_NO_NOTIFICATION_TO_QMI_REQ)
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"QMI is not notified as ICCID might not be updated yet");
    }
    else
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Notifying QMI the latest slot status");
      uim_sm_notify_qmi_slot_status(uim_ptr);
    }
  }
  else
  {
     UIMDRV_MSG_HIGH_0(uim_ptr->id,"Notifying QMI the latest slot status");
     uim_sm_notify_qmi_slot_status(uim_ptr);
  }

  rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
  if(UIM_SLOT_STATE_ACTIVE ==
     uim_sm_data.uim_slot_info[(uim_slot_type)(uim_ptr->id)].activity_status)
  {
    if(uim_ptr->state.status == UIM_ERR_S)
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"uim should have already notified error");
    }
    else
    {
       UIMDRV_MSG_HIGH_1(uim_ptr->id,"Send link est for active slot instance 0x%x",
                         (uim_slot_type)(uim_ptr->id));
	   
      if((TRUE == uim_ptr->extended_recovery.extended_recovery_enabled) && 
         (TRUE == uim_ptr->extended_recovery.extended_recovery_in_progress))
      {
         /*extended powerup is succesful. call extended_recovery_evt_handler
           to clear extended recovery globals*/
         uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TERMINATE);
         uim_ptr->flag.temporary_card_error_notified = FALSE;
      }
      /* Send a link established messgae to GSDI
         Will hit this scenario if card inserted in slot 2
      */
      uim_gsdi_link_est(uim_ptr);
    }
  }
  else
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Power down inactive slot instance 0x%x",
                      (uim_slot_type)(uim_ptr->id));
    uim_ptr->state.status = UIM_ERR_S;
    uim_hotswap_reset_slot(uim_ptr);
    uim_power_down(uim_ptr);
  }
  rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

} /* uim_sm_link_established */


/*===========================================================================
FUNCTION   UIM_SM_GSTK_UIM_FETCH_ALLOW

DESCRIPTION
  UIM Card Manager wrapper for gstk_uim_fetch_allow

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_sm_gstk_uim_fetch_allow(
  gstk_slot_id_enum_type slot
)
{
#ifndef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  return gstk_uim_fetch_allow(slot);
#else
  return gstk_uim_fetch_allow(GSTK_SLOT_1);
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
} /* uim_sm_gstk_uim_fetch_allow */


/*===========================================================================
FUNCTION   UIM_SM_TRANSLATE_CMD_SLOT

DESCRIPTION
  If UIM CARD MANAGER is enabled this function sends all external client
  commands to the active slot.  The client rpt function is saved and an
  intermediate report function is used to map the active slot back to the
  slot indicated by the client so the translation remains transparent

PARAMETERS
  cmd_ptr: uim command to be manipulated

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  No matter the slot indicated by the client commands are routed to the active
  slot but are reported as the slot indicated by the client
  The command report function is replaced by the uim_cm report function
  to perform translation before calling the original client report function.

===========================================================================*/
void uim_sm_translate_cmd_slot(
  uim_cmd_type *cmd_ptr
)
{
#ifndef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  /* do nothing if CARD MANAGER is not being used */
  (void)cmd_ptr;
  return;
#else
  uint8 instance_counter = UIM_INSTANCE_INVALID;
  uim_sm_cmd_saved_data_type *cm_saved_client_info = NULL;

  /* Send all external commands to the active slot but save the client's
     data to restore in the response before calling client's rpt_function */
  cm_saved_client_info = uim_malloc(sizeof(uim_sm_cmd_saved_data_type));
  if(cm_saved_client_info != NULL)
  {
    cm_saved_client_info->rpt_function = cmd_ptr->hdr.rpt_function;
    cm_saved_client_info->userdata     = cmd_ptr->hdr.user_data;
    cmd_ptr->hdr.options              |= UIM_OPTION_ALWAYS_RPT;
    rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
    for(instance_counter = UIM_INSTANCE_1; instance_counter < UIM_NUM_PHY_SLOTS;
        instance_counter++ )
    {
      if(uim_sm_data.uim_slot_info[instance_counter].activity_status ==
         UIM_SLOT_STATE_ACTIVE)
      {
        break;
      }
    }
    if(instance_counter == UIM_INSTANCE_INVALID)
    {
      UIM_MSG_HIGH_0("None of the slots are active!!!");
      rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
      return;
    }
    cmd_ptr->hdr.slot                  = (instance_counter+1);
    rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
    cmd_ptr->hdr.rpt_function          = uim_sm_rpt_function;
    cmd_ptr->hdr.user_data             = (int32)cm_saved_client_info;
  }
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
} /* uim_sm_translate_cmd_slot */


/*===========================================================================
FUNCTION   UIM_SM_MODIFY_LINK_EST

DESCRIPTION
  If UIM CARD MANAGER is enabled only information from one slot should be
  sent to MMGSDI - remove all information regarding the non-active slot
  from the link est structure sent to MMGSDI.

PARAMETERS
  link_est_data_ptr : MMGSDI link established structure to be manipulated

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
void uim_sm_modify_link_est(
  uim_slot_link_established_data_type link_est_data,
  uim_instance_global_type            *uim_ptr
)
{
#ifndef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
   /* Send the link established message to MMGSDI */
   (void)mmgsdi_notify_link_est_slot(link_est_data, (uim_slot_type)(uim_ptr->id + 1));
  return;
#else
  /* If the preferred slot is active send its data to mmgsdi
     Otherwise copy the data from the other SIM. */
  /* Here we should hit only only for active instance */
  /*if(uim_ptr->state.status != UIM_ERR_S)
  {
    UIM_MSG_HIGH_1("COPY active slot instance %x data to mmgsdi",uim_instance_id);
    memcpy(&link_est_data_ptr->slot_data[UIM_INSTANCE_1], &link_est_data_ptr->slot_data[uim_instance_id],
                     sizeof(link_est_data_ptr->slot_data[uim_instance_id]));
  }*/
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"Copy active slot instance 0x%x data to mmgsdi",uim_ptr->id);
  (void)mmgsdi_notify_link_est_slot(link_est_data,UIM_SLOT_1);
  /*Sending down link establishment to MMGSDI*/

  if(uim_ptr->id != UIM_INSTANCE_1)
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Clear slot instance 0x%x link est data",uim_ptr->id);
    memset(&link_est_data, 0x00, sizeof(uim_slot_link_established_data_type));
  }
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

} /* uim_sm_modify_link_est */


#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION   UIM_SM_SEND_PROACTIVE_CMD

DESCRIPTION
  A wrapper for the appropriate send_proactive_cmd to make all proacvite
  commands appear as they are comming from SLOT_1 when Card Manager is active

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
void uim_sm_send_proactive_cmd(
  uim_slot_type          slot,
  uint32                 data_length,
  const uint8            *data_ptr
)
{
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  /* send all proactive commands as SLOT1 to GSTK */
  slot = UIM_SLOT_1;
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

#ifdef FEATURE_GSTK_TAL
        (void)gstk_tal_send_proactive_cmd(
                 slot,
                 data_length,
                 data_ptr);
#else
        /* Pass raw data to GSTK */
        (void) gstk_send_proactive_cmd(
                 slot,
                 data_length,
                 data_ptr);
#endif /* FEATURE_GSTK_TAL */

} /* uim_sm_send_proactive_cmd */
#endif /* FEATURE_GSTK */


/*===========================================================================
FUNCTION UIM_SM_SWITCH_SLOT

DESCRIPTION
  This function will queue the UIM command to switch to new active slot
  and update the activity status of the slots

PARAMS:
  uim_slot_type:  Logical slot numbet to be switched to
                  Reserved for future targets
  uim_slot_type:  Physical slot number to be switched to

LIMITATIONS:
  None

RETURN VALUE:
  uim_rpt_status
  UIM_PASS:          If command is successfully queued
  UIM_FAIL:          Unsuccessful queue of switch command


SIDE EFFECTS:
 None
===========================================================================*/
uim_rpt_status uim_sm_switch_slot (
  uim_slot_type logical_slot,
  uim_slot_type physical_slot
)
{
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  uim_instance_global_type *uim_ptr          = NULL;
  uint8                     curr_active_slot = UIM_INSTANCE_INVALID;
  uim_rpt_status            status           = UIM_PASS;

  for(curr_active_slot = UIM_INSTANCE_1; curr_active_slot < UIM_NUM_PHY_SLOTS;
      curr_active_slot++ )
  {
    if(uim_sm_data.uim_slot_info[curr_active_slot].activity_status ==
       UIM_SLOT_STATE_ACTIVE)
    {
      break;
    }
  }
  /* logical_slot param is added for futuristics cases */
  (void)logical_slot;
  uim_ptr = uim_get_instance_ptr(curr_active_slot);

  if( physical_slot == (curr_active_slot + 1) )
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Given physical slot 0x%x is already active",physical_slot);
    return UIM_PASS;
  }

  if(!(physical_slot >= UIM_SLOT_1 && physical_slot <= UIM_NUM_PHY_SLOTS))
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Invalid slot value received 0x%x",physical_slot);
    return UIM_FAIL;
  }
  rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"Received switch slot request to slot 0x%x", physical_slot);

  /*Queue a power-down command to power-down current active slot*/
  if(UIM_INSTANCE_INVALID == curr_active_slot)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"None of the given physical slots are active");
  }
  else
  {
    status = uim_sm_build_command(UIM_POWER_DOWN_F, uim_ptr, UIM_NO_MASK_HOTSWAP_INT_REQ );
  }

  uim_sm_data.new_active_slot = physical_slot;
  rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
  return status;
#else
  return UIM_FAIL;
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER*/
}/*uim_sm_switch_slot*/


/*===========================================================================
FUNCTION UIM_SM_CARD_SLOT_STATUS_EVT_REG_SYNC

DESCRIPTION
  Function to register qmi callback to UIMDRV to get the latest physical
  slots information.

PARAMS:
   qmi_uim_notify_card_slot_status:Callback function pointer to be registered

LIMITATIONS:
  None

RETURN VALUE:
  uim_rpt_status
  UIM_PASS:          If registration was successful
  UIM_FAIL:          Unsuccessful registration.

SIDE EFFECTS:
 None

===========================================================================*/
uim_rpt_status uim_sm_card_slot_status_evt_reg_sync(
  uim_evt_callback_type * evt_cb_ptr
)
{
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  UIM_MSG_HIGH_0("Registering QMI sub manager callback pointer with UIMDRV ");
  rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
  uim_sm_data.evt_cb_ptr = evt_cb_ptr;
  rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
  return UIM_PASS;
#else
  return UIM_FAIL;
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
} /*uim_sm_card_slot_status_evt_reg_sync*/

/*===========================================================================
FUNCTION     UIM_SM_GET_NUM_PHYSICAL_SLOTS_SYNC

DESCRIPTION:
  Function to get number of physical slots tied to the target

RETURN
  uint8       Number of physical slots tied. 0 for invalid slot
================================================================*/
uint8 uim_sm_get_num_physical_slots_sync (void)
{
  return (uint8)UIM_NUM_PHY_SLOTS;
}/*uim_sm_get_num_physical_slots_sync*/


/*===========================================================================
FUNCTION UIM_SM_GET_LOGICAL_SLOT_STATUS_SYNC

DESCRIPTION:
  This function will return the status of all physical slots

PARAMS:
  uim_phy_slot_status_type :   Get current status of all logical slot;

LIMITATIONS:
    None

RETURN VALUE:
  uim_rpt_status
  UIM_PASS:     Physical slot status was successfully synced
  UIM_FAIL:     Invalid logical slot given or unsuccessful syncing

SIDE EFFECTS:
  None.
===========================================================================*/
uim_rpt_status uim_sm_get_logical_slot_status_sync (
  uint8                      *num_phy_slots_ptr,
  uim_phy_slot_status_type   *curr_slot_status
)
{
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
   uim_instance_global_type *uim_ptr          = NULL;
   uint8                     instance_counter = 0;
   if(curr_slot_status == NULL || num_phy_slots_ptr == NULL)
   {
     UIM_MSG_ERR_0("Null structure error");
     return UIM_FAIL;
   }
   /* Get the latest iccid values of card and card status */
   rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
   *num_phy_slots_ptr = UIM_NUM_PHY_SLOTS;

   for(instance_counter=0; instance_counter < UIM_NUM_PHY_SLOTS; instance_counter++)
   {
     uim_ptr = uim_get_instance_ptr(instance_counter);
     UIM_MSG_HIGH_1("copying instance 0x%x data to sm global",instance_counter);
     if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
     {
       UIM_MSG_HIGH_0("sub_mag Setting card status as unknown as hotswap is disabled");
       uim_sm_data.uim_slot_info[instance_counter].card_presence_status = UIM_CARD_STATUS_UNKNOWN; 
     }
     else
     { 
       uim_sm_data.uim_slot_info[instance_counter].card_presence_status =
         uim_ptr->hotswap.card_status;
     }
     uim_sm_data.uim_slot_info[instance_counter].iccid_len =
       uim_ptr->cache.iccid_buf.num_bytes;
     if(uim_ptr->hotswap.card_status != UIM_CARD_REMOVED)
     {
       uim_memscpy(uim_sm_data.uim_slot_info[instance_counter].iccid,
                   sizeof(uim_sm_data.uim_slot_info[instance_counter].iccid),
                   uim_ptr->cache.iccid_buf.data,
                   uim_sm_data.uim_slot_info[instance_counter].iccid_len );
     }
     else
     {
       memset(uim_sm_data.uim_slot_info[instance_counter].iccid, 0x00,
              uim_sm_data.uim_slot_info[instance_counter].iccid_len);
     }
   }

   UIM_MSG_HIGH_0("QMI syncing latest slot information");
   uim_memscpy(curr_slot_status, sizeof(uim_sm_data.uim_slot_info),
               uim_sm_data.uim_slot_info, sizeof(uim_sm_data.uim_slot_info));
   rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
   return UIM_PASS;
#else
   return UIM_FAIL;
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
}/*uim_sm_get_logical_slot_status_sync*/


/*===========================================================================
FUNCTION UIM_SM_NOTIFY_QMI_SLOT_STATUS

DESCRIPTION:
 Notify qmi the latest physical slot status information

LIMITATIONS:
    None

RETURN VALUE:
  uim_rpt_status
  UIM_PASS:     Physical slot status was successfully synced
  UIM_FAIL:     Invalid logical slot given or unsuccessful syncing

SIDE EFFECTS:
  None.
===========================================================================*/
uim_rpt_status uim_sm_notify_qmi_slot_status(
  uim_instance_global_type *uim_ptr
)
{
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  uint8  instance_counter = 0;
  uint32 i                = 0;

  UIMDRV_MSG_HIGH_0(uim_ptr->id,"Reporting QMI latest physical slot status ");

  /* Get the latest iccid values of card and card status */
  rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
    uim_sm_data.uim_slot_info[uim_ptr->id].card_presence_status =
      UIM_CARD_STATUS_UNKNOWN;
  }
  else
  {
  uim_sm_data.uim_slot_info[uim_ptr->id].card_presence_status =
    uim_ptr->hotswap.card_status;
  }
  uim_sm_data.uim_slot_info[uim_ptr->id].iccid_len =
    uim_ptr->cache.iccid_buf.num_bytes;
  if(uim_ptr->hotswap.card_status != UIM_CARD_REMOVED)
  {
    uim_memscpy(
       uim_sm_data.uim_slot_info[uim_ptr->id].iccid,
       sizeof(uim_sm_data.uim_slot_info[uim_ptr->id].iccid),
       uim_ptr->cache.iccid_buf.data,
       uim_sm_data.uim_slot_info[uim_ptr->id].iccid_len);
  }
  else
  {
    memset(uim_sm_data.uim_slot_info[uim_ptr->id].iccid, 0x00,
           uim_sm_data.uim_slot_info[uim_ptr->id].iccid_len);
  }

  for(instance_counter=0; instance_counter < UIM_NUM_PHY_SLOTS; instance_counter++)
  {
    UIMDRV_MSG_HIGH_2(uim_ptr->id,"sub_mgr logical slot 0x%x ,physical slot 0x%x",
             uim_sm_data.uim_slot_info[instance_counter].logical_slot,
             instance_counter+1);
    for(i=0;i<uim_sm_data.uim_slot_info[instance_counter].iccid_len;i++)
    {
      UIMDRV_MSG_HIGH_2(uim_ptr->id,"sub_mgr slot : iccid 0x%x = 0x%x",i,
               uim_sm_data.uim_slot_info[instance_counter].iccid[i]);
    }
    UIMDRV_MSG_HIGH_2(uim_ptr->id,"sub_mgr slot 0x%x: card-status 0x%x",instance_counter+1,
                      uim_sm_data.uim_slot_info[instance_counter].card_presence_status);
    UIMDRV_MSG_HIGH_2(uim_ptr->id,"sub_mgr slot 0x%x: activity status 0x%x",instance_counter+1,
                      uim_sm_data.uim_slot_info[instance_counter].activity_status);
  }

  if(uim_sm_data.evt_cb_ptr != NULL)
  {
    (*uim_sm_data.evt_cb_ptr)(uim_sm_data.num_physical_slots,
                              uim_sm_data.uim_slot_info);
  }
  else
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"QMI not notified as, QMI not registered yet");
  }
  rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
  return UIM_PASS;
#else
  return UIM_FAIL;
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
}/*uim_sm_notify_qmi_slot_status*/


/*===========================================================================
FUNCTION UIM_SM_INIT

DESCRIPTION:
  Initialize globals related to subscription manager

LIMITATIONS:
  None

RETURN VALUE:
  void

SIDE EFFECTS:
  None.
===========================================================================*/
void uim_sm_init(
  uim_instance_global_type *uim_ptr
)
{
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
 uint8          instance_counter = 0;

 rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
 if (uim_sm_data.is_sm_data_initialised != TRUE)
 {
   uim_sm_data.is_sm_data_initialised = TRUE;
   uim_sm_data.num_physical_slots = UIM_NUM_PHY_SLOTS;
   uim_sm_data.evt_cb_ptr = NULL;
   uim_sm_data.new_active_slot = 0;
   for(instance_counter=0; instance_counter < UIM_NUM_PHY_SLOTS; instance_counter++)
   {
     /* Initialize the iccid value of physical slots to 0 and
        card_presence_status to CARD REMOVED */
     uim_sm_data.uim_slot_info[instance_counter].logical_slot = UIM_SLOT_1;
     uim_sm_data.uim_slot_info[instance_counter].iccid_len = UIM_ICCID_SIZE;
     uim_sm_data.uim_slot_info[instance_counter].card_presence_status =
       UIM_CARD_REMOVED;
     memset(uim_sm_data.uim_slot_info[instance_counter].iccid, 0x00,
            uim_sm_data.uim_slot_info[instance_counter].iccid_len);
     uim_sm_data.uim_slot_info[instance_counter].activity_status =
       UIM_SLOT_STATE_INACTIVE;
   }
 }
 rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
}/* uim_sm_init*/


/*===========================================================================
FUNCTION UIM_SM_UPDATE_ACTIVE_SLOT_DATA

DESCRIPTION:
  Upon receiving switch command, this function will:-
  1) Update the NV file with latest active slot configuraion
  2) Power down the current active slot and report error
  3) Update the new active slot

LIMITATIONS:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
===========================================================================*/
boolean uim_sm_update_active_slot_data(
  uim_instance_global_type *uim_ptr
)
{
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
   uint8 instance_counter = 0;
   /* By this time, active slot must already have powered-down
      NV EFS needs to be written,
      and Active slot needs to be changed. */
   if(uim_ptr->command.cmd_ptr == NULL)
   {
     UIM_MSG_ERR_0("command pointer is NULL");
     return TRUE;
   }
   if(uim_ptr->command.cmd_ptr->hdr.req_type == UIM_SWITCH_SLOT_REQ &&
      uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F)
   {
     uim_active_slot_configuration.active_slot =
       uim_ptr->command.cmd_ptr->hdr.slot;

     /*Write to NV EFS file the new updated active slot value*/
     if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_ACTIVE_SLOT,
                                                        UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                        UIM_COMMON_EFS_DEVICE,
                                                       (uint8 *)&(uim_active_slot_configuration),
                                                        sizeof(uim_active_slot_configuration)))
     {
       UIMDRV_MSG_HIGH_1(uim_ptr->id,"Successfully wrote new active slot configuration to EFS, new_active_slot 0x%x ",
                         uim_active_slot_configuration.active_slot);
     }
     else
     {
       UIM_MSG_ERR_0("Writting to EFS failed, error");
     }

    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Changing to new active slot 0x%x",uim_sm_data.new_active_slot);
    /*Older active physical slot won't no loger be active */

    rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
    for(instance_counter=0; instance_counter < UIM_NUM_PHY_SLOTS; instance_counter++)
    {
      uim_sm_data.uim_slot_info[instance_counter].activity_status =
        UIM_SLOT_STATE_INACTIVE;
    }
    /* Change the new current active which will be powered-up*/
    uim_sm_data.uim_slot_info[uim_sm_data.new_active_slot -1].activity_status
       = UIM_SLOT_STATE_ACTIVE;

    rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
    if(TRUE == uim_sm_data.sm_ap_mode_on)
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Notifying QMI the latest slot status");
      uim_sm_notify_qmi_slot_status(uim_ptr);
      return TRUE;
    }
  }
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
  return FALSE;
}/*uim_sm_update_active_slot_data*/


/*===========================================================================
FUNCTION UIM_SM_BUILD_COMMAND

DESCRIPTION:
   To create new command for switch slot or ap on/off for inactive slot

LIMITATIONS:
  None

PARAMS
  cmd_name: To identify if its RESET command or POWER down command
  uim_ptr:  Pointer to UIM global instance.
  req_type: Information to identify the actions to be performed whenever
            the queue command is executed

RETURN VALUE:
  void

SIDE EFFECTS:
  None.
===========================================================================*/
uim_rpt_status uim_sm_build_command(
  uim_cmd_name_type         cmd_name,
  uim_instance_global_type *uim_ptr,
  uim_cmd_req_type          req_type
)
{

#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  /*The implementation assumes that UIM_POWER_DOWN_F command from MMGSDI comes
    for Airplane mode ON only and UIM_RESET_F will come from MMGSDI during
    Airplane mode Off only and from QMI context to switch slot
  */
    /*Create and send reset-f command*/
  uim_cmd_type*                      uim_cmd_ptr     = NULL;

  if ((uim_cmd_ptr = (uim_cmd_type*)uim_malloc(sizeof(uim_cmd_type)))== NULL)
  {
    return UIM_FAIL;
  }
  /*
   * Populate UIM information.
   */
   uim_cmd_ptr->hdr.command            = cmd_name;
   uim_cmd_ptr->hdr.slot               = (uim_slot_type)(uim_ptr->id) + 1;
   uim_cmd_ptr->hdr.req_type           = req_type;

   uim_cmd_ptr->hdr.channel            = UIM_DEFAULT_CHANNEL;
   uim_cmd_ptr->hdr.cmd_hdr.task_ptr   = NULL;
   uim_cmd_ptr->hdr.cmd_hdr.sigs       = 0;
   uim_cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
   (void)q_link(uim_cmd_ptr, &uim_cmd_ptr->hdr.cmd_hdr.link);
   uim_cmd_ptr->hdr.options            = UIM_OPTION_ALWAYS_RPT;
   uim_cmd_ptr->hdr.rpt_function       = uim_sm_command_callback;
   uim_cmd_ptr->hdr.user_data          = (uint32)uim_cmd_ptr;
   uim_cmd_ptr->hdr.cmd_count = 0;

   UIMDRV_MSG_HIGH_2(uim_ptr->id,
                "sub_mgr Send 0x%x command to physical slot 0x%x",
                      cmd_name, (uim_slot_type)(uim_ptr->id) + 1);
   /* Posting command on uim queue */
#ifdef TEST_FRAMEWORK
   #error code not present
#endif /* TEST_FRAMEWORK */

    /* init link */
    ( void )q_link( uim_cmd_ptr, &uim_cmd_ptr->hdr.cmd_hdr.link );
    /* and queue it */
    q_put( &uim_ptr->task.cmd_q, &uim_cmd_ptr->hdr.cmd_hdr.link );
    /* and signal the UIM task */
    (void)rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_Q_SIG );

   return UIM_PASS;
#else
   return UIM_FAIL;
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
}/*uim_sm_build_command*/


/*===========================================================================
FUNCTION UIM_SM_COMMAND_CALLBACK

DESCRIPTION:
  This is a report callback function for uim command queued during
  switch slot request

LIMITATIONS:
  None

RETURN VALUE:
  void

SIDE EFFECTS:
  None.
===========================================================================*/
void uim_sm_command_callback (
  uim_rpt_type *report_ptr
)
{
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  uim_instance_global_type *uim_ptr = NULL;
  uim_cmd_type* uim_cmd_ptr = (uim_cmd_type*)(report_ptr->user_data) ;
  if(report_ptr->user_data !=NULL)
  {
    /*Check whether we are freeing command for switch slot. If so then we need to send power-up
      for new active slot
   */
    if(((uim_cmd_type*)(report_ptr->user_data))->hdr.command == UIM_POWER_DOWN_F &&
       UIM_NO_MASK_HOTSWAP_INT_REQ == ((uim_cmd_type*)(report_ptr->user_data))->hdr.req_type
      )
    {
      /* Notify MMGSDI error for current active slot */
      UIM_MSG_HIGH_0("Notifying MMGSDI for current active slot");
      (void)mmgsdi_notify_card_error( UIM_POWER_DOWN_CMD_NOTIFICATION_S, UIM_SLOT_1, TRUE );
      /*Send RESET command for inactive slot as we recieved a switch command*/
      /*Queue reset command to power-up the inactive slot*/
      uim_ptr = uim_get_instance_ptr(uim_sm_data.new_active_slot - 1);
      uim_sm_build_command(UIM_RESET_F, uim_ptr, UIM_SWITCH_SLOT_REQ);
    }
  }
  return;
#endif
} /* uim_sm_free_buffer_function */
