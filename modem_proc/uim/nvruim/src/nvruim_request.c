/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            N V  R-U I M  R E Q U E S T  P R O C  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the functions that process nvruim accesses.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/nvruim/src/nvruim_request.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/16   nr      Redirect requests to NV during QCN Restore operation
01/31/15   ar      Checking for task stop before setting the signal for task
10/10/14   av      Clean up NVRUIM crit sect macro definition
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
04/01/14   av      NVRUIM headers cleanup
03/17/14   kb      Update function assert_on_crit_sect_owner
02/24/14   av      Fix is_item_supported function for some NV items
02/20/14   av      Support for async processing of NV_RTRE_OP_CONFIG_I
01/28/14   kb      Disable function assert_on_crit_sect_owner
01/10/14   df      Use v2.0 diag macros
11/01/13   at      Support for guaranteed response for each NVRUIM request
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/03/13   ks      F3 log reduction
07/02/13   vs      Reworked return values to NV API and clients
06/21/13   av      Fix handling of NV_ROAMING_LIST_683_I in card mode
05/13/13   vs      Fix in processing NV_MEID_I
05/13/13   vs      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rex.h"

#include "nvruimi.h"
#include "nvruim.h"
#include "nvruim_p.h"
#include "nvruim_request.h"
#include "nvruim_task.h"
#include "uim_msg.h"
#include "uim_p.h"

#include "mcfg_fs.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION NVRUIM_GET_NVRUIM_TIMER_CRIT_SECT

DESCRIPTION
  Get the pointer to the NVRUIM Timer critical section

RETURN VALUE
  rex_crit_sect_type*

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
rex_crit_sect_type* nvruim_get_nvruim_timer_crit_sect (
  void
)
{
  return (&(nvruim_generic_data_ptr->timer_crit_sect));
} /* nvruim_get_nvruim_timer_crit_sect */


/*===========================================================================

FUNCTION NVRUIM_GET_NVRUIM_CRIT_SECT

DESCRIPTION
  Get the pointer to the NVRUIM critical section

RETURN VALUE
  qurt_mutex_t*

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
qurt_mutex_t* nvruim_get_nvruim_crit_sect (
  void
)
{
  return (&(nvruim_generic_data_ptr->crit_sect));
} /* nvruim_get_nvruim_crit_sect */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_NV_ACCESS

DESCRIPTION
  This function uses the NVRUIM read, write APIs to attempts to queue requests to MMGSDI
  and return the appropriate response to NV

RETURN VALUE
  nv_stat_enum_type: Returned value depends on the value returned by NVRUIM
  core logic. NV_RUIM_NOT_SUPPORTED_S is returned if NVRUIM returns
  NV_RUIM_ITEM_NOT_SUPPORTED. If NVRUIM returns NV_RUIM_SUPPORTS_ITEM then
  the operation status returned by NVRUIM is directly returned.

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
nv_stat_enum_type nvruim_process_nv_access (
  nv_cmd_type *nv_cmd_ptr
)
{
  nv_stat_enum_type          nv_status = NV_DONE_S;

  if (nv_cmd_ptr == NULL)
  {
    return NV_BADPARM_S;
  }

  if (nv_cmd_ptr->cmd == NV_READ_F)
  {
    if (nvruim_read(nv_cmd_ptr, &nv_status) == NV_RUIM_ITEM_NOT_SUPPORTED)
    {
      nv_status = NV_RUIM_NOT_SUPPORTED_S;
    }
  }
  else if (nv_cmd_ptr->cmd == NV_WRITE_F)
  {
    if (nvruim_write(nv_cmd_ptr, &nv_status) == NV_RUIM_ITEM_NOT_SUPPORTED)
    {
      nv_status = NV_RUIM_NOT_SUPPORTED_S;
    }
  }
  else
  {
    UIM_MSG_ERR_1("NVRUIM does not support 0x%x cmd type", nv_cmd_ptr->cmd);
    nv_status = NV_BADPARM_S;
  }

  /* The nv_status return value is used by the NV API to determine the next
     course of action on a particular NV access.
     If return is "NV_DONE_S" it means NV API need not take any other action.
     If return is "NV_BADPARM_S" it means NV API will inform client about malformed request.
     If return is "NV_RUIM_NOT_SUPPORTED_S" then the NV API must divert the request to EFS.
     For all other return values NV API expects NVRUIM to populate the "status"
     member of the nv_cmd_ptr with the return value and return NV_DONE_S as nv_status
     as there is no action for NV API to take. */
  if(nv_status == NV_DONE_S)
  {
    nv_cmd_ptr->status = nv_status;
  }
  else
  {
    UIM_MSG_HIGH_2("Error processing NVRUIM request for item 0x%x, status 0x%x",
                   nv_cmd_ptr->item, nv_status);

    if(nv_status == NV_RUIM_NOT_SUPPORTED_S)
    {
      /* There is an extensive check prior to command processing to ensure only
         supported items are processed by NVRUIM. In the rare case that
         NVRUIM accepts the command for processing but during post-processing
         it is determined that command cannot be processed then we need
         to appropriately populate the nv_status and nv_cmd_ptr->status fields.
         If it is a sync call (i.e. TCB pointer is NULL) then NVRUIM simply
         returns the "NV_RUIM_NOT_SUPPORTED_S" status to NV API and let's NV API
         try to access the value from EFS.
         If it is an async call (i.e. a TCB pointer was provided) then NVRUIM
         is directly responding to the client hence the nv_cmd_ptr->status
         value is updated to ensure it has a value that the client can
         undrestand such as FAIL or NOTACTIVE */
      if(nv_cmd_ptr->tcb_ptr != NULL)
      {
        if (nv_cmd_ptr->cmd == NV_WRITE_F)
        {
          nv_cmd_ptr->status = NV_FAIL_S;
        }
        else if(nv_cmd_ptr->cmd == NV_READ_F)
        {
          nv_cmd_ptr->status = NV_NOTACTIVE_S;
        }
      }
    }
    else
    {
      nv_cmd_ptr->status = nv_status;
      nv_status          = NV_DONE_S;
    }
  }

  return nv_status;
} /* nvruim_process_nv_access */


/*===========================================================================
FUNCTION NVRUIM_IS_TASK_NVRUIM

DESCRIPTION
  This function is called to find out if the passed tcb ptr matches the nvruim
  task pointer or not.

DEPENDENCIES
  nvruim_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
===========================================================================*/
boolean nvruim_is_task_nvruim(rex_tcb_type *in_tcb_ptr)
{
  if(UIM_NVRUIM_TCB == in_tcb_ptr)
  {
    return TRUE;
  }
  return FALSE;
} /* nvruim_is_task_nvruim */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONFIGURATION

DESCRIPTION
  This function updates the global RTRE control and RTRE polling control
  values based on the configuration value provided in the command

DEPENDENCIES
  This function should only be called by the NV_CMD function to process
  an NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  nv_stat_enum_type

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type nvruim_process_rtre_configuration
(
  nv_cmd_type                  *cmd_ptr,    /* Pointer to command received on nv_cmd_q */
  nv_rtre_control_type         *nv_rtre_ctrl,
  nv_rtre_polling_control_type *nv_rtre_polling_ctrl
)
{
  nv_stat_enum_type             nv_status = NV_DONE_S;

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* Check if the command is valid */
  if(cmd_ptr == NULL)
  {
    /* Return a status indicating a bad command. */
    return NV_BADPARM_S;
  }

  NVRUIM_ENTER_CRIT_SECT;

  do
  {
    if((cmd_ptr->data_ptr == NULL) ||
       (cmd_ptr->item != NV_RTRE_CONFIG_I))
    {
      nv_status = NV_BADPARM_S;
      break;
    }

    if(nvruim_generic_data_ptr == NULL)
    {
      nv_status = NV_FAIL_S;
      break;
    }

    nvruim_generic_data_ptr->rtre_config_init = TRUE;

    UIM_MSG_HIGH_2(" nvruim_process_rtre_configuration : cmd_ptr : 0x%x  for RTRE 0x%x",
                   cmd_ptr, cmd_ptr->data_ptr->rtre_config);

    /* Determine the control based on the configuration */
    switch(cmd_ptr->data_ptr->rtre_config)
    {
      /* RTRE configuration: use R-UIM only */
      case NV_RTRE_CONFIG_RUIM_ONLY:
        /* Set the control to use the R-UIM */
        nvruim_generic_data_ptr->rtre_control_value =
          NV_RTRE_CONTROL_USE_RUIM;
        nvruim_generic_data_ptr->rtre_polling_control_value =
          NV_RTRE_POLLING_CONTROL_POLL;
        break; /* end case - NV_RTRE_CONFIG_RUIM_ONLY */

        /* RTRE configuration: use NV only    */
      case NV_RTRE_CONFIG_NV_ONLY:
        /* Set the control to NOT use the R-UIM */
        nvruim_generic_data_ptr->rtre_control_value =
          NV_RTRE_CONTROL_NO_RUIM;
        nvruim_generic_data_ptr->rtre_polling_control_value =
          NV_RTRE_POLLING_CONTROL_NO_POLL;
        break; /* end case - NV_RTRE_CONFIG_NV_ONLY */

        /* RTRE: use R-UIM if available, otherwise use NV */
      case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
        /* Check if the CDMA subscription is on the card. */
        if(NVRUIM_CDMA_SUBSCRIP_PRESENT & nvruim_dir_present)
        {
          /* Set the control to use the R-UIM */
          nvruim_generic_data_ptr->rtre_control_value =
            NV_RTRE_CONTROL_USE_RUIM;
          nvruim_generic_data_ptr->rtre_polling_control_value =
            NV_RTRE_POLLING_CONTROL_POLL;
        }
        else /* There was no CDMA subscription on the card */
        {
          /* Set the control to NOT use the R-UIM */
          nvruim_generic_data_ptr->rtre_control_value =
            NV_RTRE_CONTROL_NO_RUIM;
          nvruim_generic_data_ptr->rtre_polling_control_value =
            NV_RTRE_POLLING_CONTROL_NO_POLL;
        } /* end if -  found CDMA subscription on card. */
        UIM_MSG_HIGH_1("NVRUIM CDMA subscription active 0x%x",
                       (NVRUIM_CDMA_SUBSCRIP_PRESENT & nvruim_dir_present));
        break; /* end case - NV_RTRE_CONFIG_RUIM_OR_DROP_BACK */

        /* This is a bad value */
      default:
        /* Return a status indicating a bad command. */
        nv_status = NV_BADPARM_S;
        break;
    } /* end switch - cmd_ptr->data_ptr.rtre_configuration */

    if(nv_status != NV_DONE_S)
    {
      break;
    }

    if((nvruim_generic_data_ptr->rtre_control_value == NV_RTRE_CONTROL_USE_RUIM) &&
       (NV_RUIM_ZERO_ESN == nvruim_get_esn_usage()))
    {
      nvruim_set_esn_usage(NV_RUIM_USE_ESN);
    }

    /* Indicate the command has been successfully completed */
    if(nv_rtre_ctrl != NULL)
    {
      *nv_rtre_ctrl = nvruim_generic_data_ptr->rtre_control_value;
    }
    if(nv_rtre_polling_ctrl != NULL)
    {
      *nv_rtre_polling_ctrl = nvruim_generic_data_ptr->rtre_polling_control_value;
    }
  }
  while(0);

  NVRUIM_LEAVE_CRIT_SECT;

  cmd_ptr->status = nv_status;
  return nv_status;
#else
  (void)cmd_ptr;
  (void)nv_rtre_ctrl;
  (void)nv_rtre_polling_ctrl;
  return NV_BADTG_S;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
} /* nvruim_process_rtre_configuration */


/*===========================================================================

FUNCTION NVRUIM_CHECK_RTRE_CONFIG

DESCRIPTION
  Check to see RTRE CONFIG is Initialized. If FEATURE_UIM_RUN_TIME_ENABLE is
  NOT defined, default RTRE value of RUIM_NOT_SUPPORTED is used and RUIM
  is not accessed for any nv requests. In that event, we can safely return TRUE
  meaning rtre config has been initialized.

RETURN VALUE
  TRUE - If Config is Initilaized
  FALSE - If Config  is not intialized

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
boolean nvruim_check_rtre_config(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  if(nvruim_generic_data_ptr != NULL)
  {
    if(nvruim_generic_data_ptr->rtre_config_init == FALSE)
    {
      /* Do not read before knowing RTRE configuration */
      if(op_status != NULL)
      {
        *op_status = NV_NOTACTIVE_S;
      }
      if(nv_cmd_ptr != NULL)
      {
        nv_cmd_ptr->status = NV_NOTACTIVE_S;
      }
      return FALSE;
    }
  }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  return TRUE;
} /* nvruim_check_rtre_config */


/*===========================================================================

FUNCTION NVRUIM_GET_RTRE_CONTROL_VALUE

DESCRIPTION
  This function returns the global RTRE control value that is set based on the
  NV 855 RUIM Run-Time Enable (RTRE) configuration. If the feature flag
  FEATURE_UIM_RUN_TIME_ENABLE is DISABLED, the default is NOT to go to
  the card for any nvitem.

RETURN VALUE
  nv_rtre_control_type: NV_RTRE_CONTROL_USE_RUIM or NV_RTRE_CONTROL_NO_RUIM

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
nv_rtre_control_type nvruim_get_rtre_control_value (
  void
)
{
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  if(nvruim_generic_data_ptr != NULL)
  {
    return nvruim_generic_data_ptr->rtre_control_value;
  }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  return NV_RTRE_CONTROL_NO_RUIM;
} /* nvruim_get_rtre_control_value */


/*===========================================================================

FUNCTION NVRUIM_IS_NV_ITEM_SUPPORTED

DESCRIPTION
  This function checks whether the NV item access is supported by NVRUIM

RETURN VALUE
  Boolean: TRUE if item is supported, FALSE if not

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static boolean nvruim_is_nv_item_supported (
  const nv_cmd_type       *nv_cmd_ptr
)
{
  /* item_supported is initialized to TRUE and if there is an exception it is
     set to FALSE before returning */
  boolean item_supported = TRUE;

  if ((nv_cmd_ptr == NULL) ||
      (nv_cmd_ptr->data_ptr == NULL) ||
      (nvruim_get_rtre_control_value() == NV_RTRE_CONTROL_NO_RUIM))
  {
    return FALSE;
  }

  switch (nv_cmd_ptr->item)
  {
    case NV_ESN_CHKSUM_I:
    case NV_ESN_I:
      if(nv_cmd_ptr->cmd == NV_WRITE_F)
      {
        item_supported = FALSE;
      }
      else if(nv_cmd_ptr->cmd == NV_READ_F)
      {
        nvruim_esn_usage_type esn_usage;

        esn_usage = nvruim_get_esn_usage();
        if(esn_usage == NV_RUIM_USE_ESN  ||
           esn_usage == NV_RUIM_USE_MEID ||
           esn_usage == NV_RUIM_USE_SF_EUIMID_ESN)
        {
          if (nvruim_is_mmgsdi_item_enabled(MMGSDI_NV_UIM_JCDMA_MODE_I) == FALSE)
          {
            item_supported = FALSE;
          }
        }
      }
      break;

    case NV_MEID_I:
      if(nv_cmd_ptr->cmd == NV_WRITE_F)
      {
        item_supported = FALSE;
      }
      if ((nv_cmd_ptr->cmd == NV_READ_F) &&
          (nvruim_get_esn_usage() != NV_RUIM_USE_SF_EUIMID &&
           nvruim_get_esn_usage() != NV_RUIM_USE_SF_EUIMID_ESN))
      {
        item_supported = FALSE;
      }
      break;

    case NV_SLOT_CYCLE_INDEX_I:
      if (!nvruim_is_mmgsdi_item_enabled(MMGSDI_NV_UIM_RUIM_SUPPORT_SCI_I))
      {
        item_supported = FALSE;
      }
      break;

    case NV_PREF_MODE_I:
      if (nvruim_is_mmgsdi_item_enabled(MMGSDI_NV_PREF_MODE_NOT_FROM_RUIM_I) ||
          nv_cmd_ptr->data_ptr->pref_mode.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_MIN1_I:
    case NV_MIN2_I:
    case NV_IMSI_11_12_I:
    case NV_IMSI_MCC_I:
    case NV_IMSI_ADDR_NUM_I:
      if((nv_cmd_ptr->cmd == NV_READ_F) &&
         (nv_cmd_ptr->data_ptr->min1.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_IMSI_T_S1_I:
    case NV_IMSI_T_S2_I:
    case NV_IMSI_T_MCC_I:
    case NV_IMSI_T_11_12_I:
    case NV_IMSI_T_ADDR_NUM_I:
      if((nv_cmd_ptr->cmd == NV_READ_F) &&
         (nv_cmd_ptr->data_ptr->imsi_t_s1.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_ACCOLC_I:
      if((nv_cmd_ptr->cmd == NV_READ_F) &&
         (nv_cmd_ptr->data_ptr->accolc.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_ROAMING_LIST_683_I:
      if (nvruim_is_mmgsdi_item_enabled(MMGSDI_NV_UIM_JCDMA_MODE_I))
      {
        UIM_MSG_HIGH_0("JCDMA Service is enabled, Card PRL not supported");
        item_supported = FALSE;
      }
      if((nv_cmd_ptr->cmd == NV_READ_F) &&
         (nv_cmd_ptr->data_ptr->roaming_list.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_DIR_NUMBER_I:
      if(nv_cmd_ptr->data_ptr->dir_number.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_DIR_NUMBER_PCS_I:
      if((nv_cmd_ptr->cmd == NV_WRITE_F) &&
         (nv_cmd_ptr->data_ptr->mob_dir_number.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      if((nv_cmd_ptr->cmd == NV_READ_F) &&
         (nv_cmd_ptr->data_ptr->dir_number.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_SID_NID_I:
      if((nv_cmd_ptr->cmd == NV_WRITE_F) &&
         (nv_cmd_ptr->data_ptr->sid_nid.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_HOME_SID_NID_I:
      if(nv_cmd_ptr->data_ptr->home_sid_nid.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_ANALOG_HOME_SID_I:
      if(nv_cmd_ptr->data_ptr->analog_home_sid.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_MOB_TERM_HOME_I:
      if(nv_cmd_ptr->data_ptr->mob_term_home.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_MOB_TERM_FOR_SID_I:
      if(nv_cmd_ptr->data_ptr->mob_term_for_sid.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_MOB_TERM_FOR_NID_I:
      if(nv_cmd_ptr->data_ptr->mob_term_for_nid.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_ANALOG_FIRSTCHP_I:
      if(nv_cmd_ptr->data_ptr->analog_firstchp.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_OTAPA_ENABLED_I:
      if(nv_cmd_ptr->data_ptr->otapa_enabled.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_NAM_LOCK_I:
      if(nv_cmd_ptr->data_ptr->nam_lock.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_ASSIGNING_TMSI_ZONE_LEN_I:
      if(nv_cmd_ptr->data_ptr->assigning_tmsi_zone_length.nam != NV_RUIM_NAM)
      {
        item_supported = FALSE;
      }
      break;

    case NV_ASSIGNING_TMSI_ZONE_I:
      if((nv_cmd_ptr->cmd == NV_WRITE_F) &&
         (nv_cmd_ptr->data_ptr->assigning_tmsi_zone.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_TMSI_CODE_I:
      if((nv_cmd_ptr->cmd == NV_WRITE_F) &&
         (nv_cmd_ptr->data_ptr->tmsi_code.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_TMSI_EXP_I:
      if((nv_cmd_ptr->cmd == NV_WRITE_F) &&
         (nv_cmd_ptr->data_ptr->tmsi_exp_timer.nam != NV_RUIM_NAM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_SMS_BC_USER_PREF_I:
    case NV_SMS_BC_SERVICE_TABLE_I:
      if (nvruim_is_bcsms_svc_avail() == FALSE)
      {
        UIM_MSG_HIGH_0("Card doesn't support BCSMS Extensions, falling back to NV");
        item_supported = FALSE;
      }
      break;

    case NV_SMS_MO_RETRY_PERIOD_I:
    case NV_SMS_MO_RETRY_INTERVAL_I:
    case NV_SMS_MO_ON_TRAFFIC_CHANNEL_I:
    case NV_SMS_MO_ON_ACCESS_CHANNEL_I:
    case NV_SMS_SERVICE_OPTION_I:
      if (nv_cmd_ptr->cmd == NV_WRITE_F)
      {
        item_supported = FALSE;
      }
      if ((nv_cmd_ptr->cmd == NV_READ_F) &&
          (nvruim_is_smscap_svc_avail() == FALSE))
      {
        item_supported = FALSE;
      }
      break;

    case NV_SMS_BC_CONFIG_I:
      if (nv_cmd_ptr->cmd == NV_WRITE_F)
      {
        item_supported = FALSE;
      }
      if ((nv_cmd_ptr->cmd == NV_READ_F) &&
          (nvruim_is_bcsms_svc_avail() == FALSE))
      {
        item_supported = FALSE;
      }
      break;

    case NV_SMS_BC_SERVICE_TABLE_SIZE_I:
    case NV_SMS_BC_SERVICE_TABLE_OPTIONS_I:
      if (nv_cmd_ptr->cmd == NV_WRITE_F)
      {
        item_supported = FALSE;
      }
      if ((nv_cmd_ptr->cmd == NV_READ_F) &&
          (nvruim_is_bcsms_svc_avail() == FALSE))
      {
        item_supported = FALSE;
      }
      break;

    case NV_GPS1_CAPABILITIES_I:
    case NV_GPS1_SEEDPOS_OPTION_I:
    case NV_GPS1_DYNAMIC_MODE_I:
    case NV_GPS1_XTRA_ENABLED_I:
    case NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I:
    case NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I:
    case NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I:
    case NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I:
    case NV_GPS1_XTRA_PRIMARY_SERVER_URL_I:
    case NV_GPS1_XTRA_SECONDARY_SERVER_URL_I:
    case NV_GPS1_XTRA_TERTIARY_SERVER_URL_I:
    case NV_GPS1_PDE_ADDRESS_I:
    case NV_GPS1_PDE_PORT_I:
    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I:
    case NV_CGPS_1X_PDE_SERVER_PORT_I:
    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV6_I:
    case NV_CGPS_1X_PDE_SERVER_ADDR_URL_I:
    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I:
    case NV_CGPS_1X_MPC_SERVER_PORT_I:
    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV6_I:
    case NV_CGPS_1X_MPC_SERVER_ADDR_URL_I:
      if (nvruim_is_lbs_support_avail() == FALSE)
      {
        item_supported = FALSE;
      }
      break;

    case NV_DS_QCMIP_I:
      if(uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV)
      {
        item_supported = FALSE;
      }
      break;

    case NV_DS_SIP_PPP_SS_INFO_I:
      if((uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
         (uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV))
      {
        item_supported = FALSE;
      }
      break;

    case NV_DS_SIP_ACTIVE_PROFILE_INDEX_I:
      if((uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV) ||
         (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV))
      {
        item_supported = FALSE;
      }
      break;

    case NV_HYSTERISIS_ACTIVATION_TIMER_I:
    case NV_ACTIVE_EPZID_TYPE_I:
    case NV_DSAT707_CTA_TIMER_I:
      if ((nvruim_data_3gpd_ext_support() == FALSE) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV))
      {
        item_supported = FALSE;
      }
      break;

    case NV_DS_MIP_QC_DRS_OPT_I:
    case NV_DS_MIP_QC_HANDDOWN_TO_1X_OPT_I:
    case NV_DS_MIP_RRQ_IF_TFRK_I:
      if ((nvruim_data_3gpd_ext_support() == FALSE) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_TCP_KEEPALIVE_IDLE_TIME_I:
      if ((nvruim_data_3gpd_ext_support() == FALSE) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV))
      {
        /* For this NV item, both READ and WRITE are not supported if we
           come here */
        item_supported = FALSE;
      }
      else if((uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM) &&
              (nv_cmd_ptr->cmd == NV_READ_F))
      {
        /* For this NV item, we do not support the READ if uim_3gpd_support
           is MIP_NV_SIP_RUIM. WRITE is supported in this case... this is as
           per the legacy code in nvruim.c */
        item_supported = FALSE;
      }
      break;

    case NV_TCP_GRACEFUL_DORMANT_CLOSE_I:
      if ((nvruim_data_3gpd_ext_support() == FALSE) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_DS_SIP_PROFILE_I:
      if ((nvruim_data_3gpd_ext_support() == FALSE) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV))
      {
        item_supported = FALSE;
      }
      break;

    case NV_PPP_USER_ID_I:
      if ((nv_cmd_ptr->cmd == NV_READ_F) &&
          (nvruim_data_3gpd_ext_support() ||
           (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
           (uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV)))
      {
        item_supported = FALSE;
      }
      if((nv_cmd_ptr->cmd == NV_WRITE_F) &&
         ((uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV)))
      {
        item_supported = FALSE;
      }
      break;

    case NV_PPP_PASSWORD_I:
      if (nvruim_data_3gpd_ext_support() ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV))
      {
        UIM_MSG_HIGH_1("Returning the NV value for item 0x%x",
                       nv_cmd_ptr->item);
        item_supported = FALSE;
      }
      break;

    case NV_DS_SIP_NUM_VALID_PROFILES_I:
    case NV_DS_SIP_NAI_INFO_I:
      if ((uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV))
      {
        item_supported = FALSE;
      }
      break;

    case NV_DS_MIP_ACTIVE_PROF_I:
    case NV_DS_MIP_DMU_PKOID_I:
    case NV_DS_MIP_DMU_MN_AUTH_I:
    case NV_DS_MIP_SS_USER_PROF_I:
    case NV_DS_MIP_2002BIS_MN_HA_AUTH_I:
    case NV_DS_MIP_GEN_USER_PROF_I:
    case NV_DS_MIP_NUM_PROF_I:
    case NV_DS_MIP_RETRIES_I:
    case NV_DS_MIP_RETRY_INT_I:
    case NV_DS_MIP_PRE_RE_RRQ_TIME_I:
    case NV_DS_MIP_ENABLE_PROF_I:
      if ((uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV) ||
          (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM))
      {
        item_supported = FALSE;
      }
      break;

    case NV_HDR_AN_AUTH_USER_ID_LONG_I:
    case NV_HDR_AN_AUTH_NAI_I:
      if (uim_an_hrpd_support() == NVRUIM_AN_HRPD_NO_SUPPORT)
      {
        item_supported = FALSE;
      }
      break;

    case NV_HDR_AN_AUTH_PASSWD_LONG_I:
    case NV_HDR_AN_AUTH_PASSWORD_I:
      if ((nv_cmd_ptr->cmd == NV_WRITE_F) &&
          (uim_an_hrpd_support() == NVRUIM_AN_HRPD_NO_SUPPORT))
      {
        item_supported = FALSE;
      }
      if (nv_cmd_ptr->cmd == NV_READ_F)
      {
        item_supported = FALSE;
      }
      break;

    case NV_COUNT_I:
    case NV_SPC_CHANGE_ENABLED_I:
    case NV_SYSTEM_PREF_I:
    case NV_NXTREG_I:
    case NV_LSTSID_I:
    case NV_LOCAID_I:
    case NV_PUREG_I:
    case NV_ZONE_LIST_I:
    case NV_SID_NID_LIST_I:
    case NV_DIST_REG_I:
      break;

    default:
      item_supported = FALSE;
  }

  return item_supported;
} /* nvruim_is_nv_item_supported */


/*===========================================================================

FUNCTION NVRUIM_GET_RTRE_POLLING_CONTROL_VALUE

DESCRIPTION
  This function provides the current RTRE polling control.  The polling control
  indicates whether to poll the UIM card when originating a call or while on
  the traffic channel. If FEATURE_UIM_RUN_TIME_ENABLE not enabled, rtre control
  is set to RUIM_NOT_SUPPORTED and so there is no polling.

RETURN VALUE
  nv_rtre_polling_control_type: NV_RTRE_POLLING_CONTROL_NO_POLL or
  NV_RTRE_POLLING_CONTROL_POLL

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static nv_rtre_polling_control_type nvruim_get_rtre_polling_control_value (
  void
)
{
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  return nvruim_generic_data_ptr->rtre_polling_control_value;
#else
  return NV_RTRE_POLLING_CONTROL_NO_POLL;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
} /* nvruim_get_rtre_polling_control_value */


/*===========================================================================

FUNCTION NVRUIM_GET_NV_MAX_ROAMING_LIST_SIZE

DESCRIPTION
  This function returns the maximum number of bytes available for
  storage of a roaming list (per NAM).

RETURN VALUE
  uint16: Either 0 or NV_ROAMING_LIST_MAX_SIZE

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static uint16 nvruim_get_nv_max_roaming_list_size (
  void
)
{
  uint16 prl_max_size = 0;

  if (nvruim_get_rtre_control_value() == NV_RTRE_CONTROL_USE_RUIM)
  {
    if (nvruim_is_mmgsdi_item_enabled(MMGSDI_NV_UIM_JCDMA_MODE_I))
    {
      prl_max_size = NV_ROAMING_LIST_MAX_SIZE;
    }
  }
  else
  {
    prl_max_size = NV_ROAMING_LIST_MAX_SIZE;
  }
  return prl_max_size;
} /* nvruim_get_nv_max_roaming_list_size */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONFIG_NV_REQ

DESCRIPTION
  This function processes the NV request to set the RTRE control and
  RTRE polling control values based on the configuration (NV 855)
  read from EFS.

RETURN VALUE
  nv_stat_enum_type

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static nv_stat_enum_type nvruim_process_rtre_config_nv_req (
  void *in_ptr
)
{
  nv_stat_enum_type             nv_op_status                       = NV_DONE_S;
  nv_rtre_control_type          dummy_rtre_control_value;
  nv_rtre_polling_control_type  dummy_rtre_polling_control_value;
  nv_cmd_type                  *nv_cmd_ptr                         = (nv_cmd_type *)in_ptr;

  if (in_ptr == NULL)
  {
    UIM_MSG_ERR_0("NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL:Bad input buffer");
    return NV_BADPARM_S;
  }

  if (nv_cmd_ptr->cmd != NV_RTRE_OP_CONFIG_F)
  {
    UIM_MSG_ERR_0("NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL:Invalid request type");
    nv_cmd_ptr->status = NV_RUIM_NOT_SUPPORTED_S;
    return NV_RUIM_NOT_SUPPORTED_S;
  }

  if (nv_cmd_ptr->sigs == 0)
  {
    UIM_MSG_ERR_0("NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL:No sig provided");
    nv_cmd_ptr->status = NV_BADPARM_S;
    return NV_BADPARM_S;
  }

  if (nv_cmd_ptr->tcb_ptr)
  {
    /* Client has made an async NV access. Queue to NVRUIM task */
    nv_op_status = nvruim_cmd(nv_cmd_ptr);
  }
  else
  {
    /* Client has made a sync NV access. Process in client context */
    nv_op_status = nvruim_process_rtre_configuration(nv_cmd_ptr,
                                                     &dummy_rtre_control_value,
                                                     &dummy_rtre_polling_control_value);
  }

  return nv_op_status;
} /* nvruim_process_rtre_config_nv_req */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_ROAMING_LIST_SIZE_NV_REQ

DESCRIPTION
  This function processes the roaming list size request from NV task

RETURN VALUE
  nv_stat_enum_type

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static nv_stat_enum_type nvruim_process_roaming_list_size_nv_req (
  void *out_ptr
)
{
  if (out_ptr == NULL)
  {
    UIM_MSG_ERR_0("NV_RUIM_CMD_ROAMING_LIST_IOCTL:Bad output buffer");
    return NV_BADPARM_S;
  }

  *((uint16 *)out_ptr) = nvruim_get_nv_max_roaming_list_size();
  return NV_DONE_S;
} /* nvruim_process_roaming_list_size_nv_req */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONTROL_NV_REQ

DESCRIPTION
  This function processes the RTRE control and RTRE polling control request
  from NV task

RETURN VALUE
  nv_stat_enum_type

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static nv_stat_enum_type nvruim_process_rtre_control_nv_req (
  nv_ruim_enum_type  type,
  const void        *in_ptr,
  void              *out_ptr
)
{
  nv_stat_enum_type  nv_operation_status = NV_DONE_S;

  switch (type)
  {
    case NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL:
      if (out_ptr == NULL)
      {
        UIM_MSG_ERR_0("NV_RUIM_CMD_RTRE_CONTROL_IOCTL:Bad output buffer");
        nv_operation_status = NV_BADPARM_S;
        break;
      }

      *((nv_rtre_control_type *)out_ptr) = nvruim_get_rtre_control_value();
      break;

    case NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL:
      if (out_ptr == NULL)
      {
        UIM_MSG_ERR_0("NV_RUIM_CMD_RTRE_POLLING_CONTROL_IOCTL:Bad output buffer");
        nv_operation_status = NV_BADPARM_S;
        break;
      }

      *((nv_rtre_polling_control_type *)out_ptr) =
        nvruim_get_rtre_polling_control_value();
      break;

    case NV_RUIM_CMD_RTRE_NAM_IOCTL:
      if (in_ptr == NULL || out_ptr == NULL)
      {
        UIM_MSG_ERR_2("NV_RUIM_CMD_RTRE_NAM_IOCTL:Bad i/p 0x%x or o/p 0x%x buff",
                      in_ptr, out_ptr);
        nv_operation_status = NV_BADPARM_S;
        break;
      }

      if ((nvruim_get_rtre_control_value() == NV_RTRE_CONTROL_USE_RUIM) &&
          (*((uint8 *)in_ptr) == NV_RUIM_NAM))
      {
        *((boolean *)out_ptr) = TRUE;
      }
      else
      {
        *((boolean *)out_ptr) = FALSE;
      }
      break;

    case NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL:
      if (in_ptr == NULL || out_ptr == NULL)
      {
        UIM_MSG_ERR_2("NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL:Bad i/p 0x%x or o/p 0x%x buff",
                      in_ptr, out_ptr);
        nv_operation_status = NV_BADPARM_S;
        break;
      }

      if ((nvruim_get_rtre_polling_control_value() == NV_RTRE_POLLING_CONTROL_POLL) &&
          (*((uint8 *)in_ptr) == NV_RUIM_NAM))
      {
        *((boolean *)out_ptr) = TRUE;
      }
      else
      {
        *((boolean *)out_ptr) = FALSE;
      }
      break;

    default:
      UIM_MSG_ERR_1("Unsupported type 0x%x", type);
      nv_operation_status = NV_BADPARM_S;
      break;
  }

  return nv_operation_status;
} /* nvruim_process_rtre_control_nv_req */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_VERIFY_AND_ACCESS_NV_REQ

DESCRIPTION
  This function processes the verify and access request from NV task

RETURN VALUE
  nv_stat_enum_type

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static nv_stat_enum_type nvruim_process_verify_and_access_nv_req (
  void *in_ptr
)
{
  nv_stat_enum_type  nv_operation_status = NV_DONE_S;
  nv_cmd_type       *nv_in_cmd_ptr       = (nv_cmd_type *)in_ptr;

  if (in_ptr == NULL || nvruim_generic_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL:Bad input buffer");
    return NV_BADPARM_S;
  }

  if (nv_in_cmd_ptr->cmd != NV_READ_F &&
      nv_in_cmd_ptr->cmd != NV_WRITE_F)
  {
    UIM_MSG_ERR_0("NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL:Invalid request type");
    return NV_RUIM_NOT_SUPPORTED_S;
  }

  if (nvruim_is_nv_item_supported((const nv_cmd_type *) nv_in_cmd_ptr) == FALSE)
  {
    return NV_RUIM_NOT_SUPPORTED_S;
  }

  /*  If there was a update on EFS, the status is unknwon so re-read it */
  if (nvruim_generic_data_ptr->nvruim_qcn_restore_status == NV_RUIM_QCN_RESTORE_UNKNOWN)
  {
    uint8 qcn_restore_inprogress = 0;

    nvruim_generic_data_ptr->nvruim_qcn_restore_status = NV_RUIM_QCN_RESTORE_NOT_IN_PROGRESS;
    if(mcfg_fs_read((const char *)NVRUIM_QCN_RESTORE_IN_PROGRESS_EFS, (void *)&qcn_restore_inprogress,
                    sizeof(uint8), MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE) == MCFG_FS_STATUS_OK)
    {
      if (qcn_restore_inprogress == 0x01)
      {
        nvruim_generic_data_ptr->nvruim_qcn_restore_status = NV_RUIM_QCN_RESTORE_IN_PROGRESS;
      }
    }
  }

  /*  If status is in progress, return not_supported for both READs and WRITEs */
  if(nvruim_generic_data_ptr->nvruim_qcn_restore_status == NV_RUIM_QCN_RESTORE_IN_PROGRESS)
  {
    return NV_RUIM_NOT_SUPPORTED_S;
  }

  if (nv_in_cmd_ptr->sigs == 0)
  {
    UIM_MSG_ERR_0("NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL:No sig provided");
    return NV_BADPARM_S;
  }

  if (nv_in_cmd_ptr->tcb_ptr)
  {
    /* Client has made an async NV access. Queue to NVRUIM task */
    nv_operation_status = nvruim_cmd(nv_in_cmd_ptr);
  }
  else
  {
    /* Client has made a sync NV access. Process in client context */
    nv_operation_status =
      nvruim_process_nv_access(nv_in_cmd_ptr);
  }

  return nv_operation_status;
} /* nvruim_process_verify_and_access_nv_req */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_NV_REQ

DESCRIPTION
  This function processes the following requests from NV task:
  NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL
    - Verify if and NV item maps to EF on card and if so access (read/write) it
  NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL
    - Update/fetch rtre_control_value
  NV_RUIM_CMD_RTRE_NAM_IOCTL
    - Update/fetch nv_rtre_use_ruim_for_nam
  NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL
    - Update/fetch nv_rtre_polling_control
  NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL
    - Update/fetch nv_rtre_polling_for_nam
  NV_RUIM_CMD_ROAMING_LIST_MAX_SIZE_IOCTL
    - Update/fetch nv_max_size_of_roaming_list
  NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL
    - Process RTRE configuration command

RETURN VALUE
  Of the enumerations defined in nv_stat_enum_type this function will return:
    NV_DONE_S      - Request processed or to be processed by MMGSDI
    NV_BADPARM_S   - Bad parameter in the command block
    NV_RUIM_NOT_SUPPORTED_S - Indicates an NV item not supported in RUIM

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
nv_stat_enum_type nvruim_process_nv_req (
  nv_ruim_enum_type  type,       /* Command type to be processed */
  void              *in_ptr,     /* Input buffer for the command type */
  void              *out_ptr     /* Output buffer for the command type */
)
{
  nv_stat_enum_type  nv_operation_status = NV_DONE_S;

  switch (type)
  {
    case NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL:
      nv_operation_status =
        nvruim_process_verify_and_access_nv_req(in_ptr);
      break;

    case NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL:
    case NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL:
    case NV_RUIM_CMD_RTRE_NAM_IOCTL:
    case NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL:
      nv_operation_status =
        nvruim_process_rtre_control_nv_req(type, in_ptr, out_ptr);
      break;

    case NV_RUIM_CMD_ROAMING_LIST_MAX_SIZE_IOCTL:
      nv_operation_status =
        nvruim_process_roaming_list_size_nv_req(out_ptr);
      break;

    case NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL:
      nv_operation_status =
        nvruim_process_rtre_config_nv_req(in_ptr);
      break;

    default:
      UIM_MSG_ERR_1("Command type 0x%x not supported", type);
      nv_operation_status = NV_BADPARM_S;
  }

  return nv_operation_status;
} /* nvruim_process_nv_req */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */