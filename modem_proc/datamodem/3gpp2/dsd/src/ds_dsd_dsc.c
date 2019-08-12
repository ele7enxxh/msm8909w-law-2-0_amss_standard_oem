/*===========================================================================
                     D S _ D S D _ A P M . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c) 2012-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_dsc.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
5/11/12    sk    Created Module 
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "data_msg.h"
#include "assert.h"

#include "dstask_v.h"
#include "ds_dsd_dsc.h"
#include "ds_dsd_apm.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "cm.h"
#include "ran.h"

/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/
#define DSD_DSC_INVALID_PLMN_BLOCKING_INTERVAL       0

/*===========================================================================

                          INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_DSC_COMMAND

DESCRIPTION   This is a DSC command interface to DSC module. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  Commands CM module
===========================================================================*/
void ds_dsd_dsc_command
(
  sys_modem_as_id_e_type           subs_id,
  ds_dsd_dsc_command_type          command,
  uint32                           blocking_interval_value
)
{
  cm_client_id_type                client_id = 1; 
  cm_plmn_blocking_info_s_type     plmn_blocking_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_HIGH,"ds_dsd_dsc_command cmd:%d",command);
  memset (&plmn_blocking_info, 0, sizeof(cm_plmn_blocking_info_s_type));

  if (blocking_interval_value != DSD_DSC_INVALID_PLMN_BLOCKING_INTERVAL)
  {
    blocking_interval_value += (ran_dist(ran_next(), 1, 15) * 1000);
    plmn_blocking_info.blocking_interval = blocking_interval_value;

    DATA_MSG1 (MSG_LEGACY_MED, "PLMN Blocking Interval is %d",
               blocking_interval_value);
  }

  switch(command)
  {
    case DS_DSC_PLMN_BLOCKING:
      plmn_blocking_info.request_pref 
        = CM_PLMN_BLOCKING_PREF_PLMN_BLOCKING_ONLY;
      client_id = ds3g_get_cm_client_id();
      if ( FALSE == ds_dsd_apm_get_current_plmn_id(subs_id,
                                                   &(plmn_blocking_info.plmn)))
      {
        return;
      }

      cm_ph_cmd_detach_and_plmn_blocking(NULL,
                                         NULL,
                                         client_id,
                                         plmn_blocking_info);
      break;

    case DS_DSC_PS_DETACH:
      plmn_blocking_info.request_pref = CM_PLMN_BLOCKING_PREF_LTE_DETACH_ONLY;
      client_id = ds3g_get_cm_client_id();
      cm_ph_cmd_detach_and_plmn_blocking(NULL,
                                         NULL,
                                         client_id,
                                         plmn_blocking_info);
      break;

    case DS_DSC_PLMN_BLOCKING_AND_PS_DETACH:
      plmn_blocking_info.request_pref 
        = CM_PLMN_BLOCKING_PREF_LTE_DETACH_PLMN_BLOCKING;
      client_id = ds3g_get_cm_client_id();
      if ( FALSE == ds_dsd_apm_get_current_plmn_id(subs_id,
                                                   &(plmn_blocking_info.plmn)) )
      {
        return;
      }

      cm_ph_cmd_detach_and_plmn_blocking(NULL,
                                         NULL,
                                         client_id,
                                         plmn_blocking_info);
      break;

    default:
      break;
  }
  return;
}/* ds_dsd_dsc_command()*/

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_DSC_DISABLE_LTE_SELECTION

DESCRIPTION   DSC to call CM API to disable LTE selection. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  Commands CM module
===========================================================================*/
void ds_dsc_disable_lte_selection(void)
{
#ifdef FEATURE_DATA_LTE  
  sys_modem_as_id_e_type subs_id = ds3g_get_ps_subs_id(); // Vishal: Temporary

  DATA_MSG0(MSG_LEGACY_HIGH,"ds_dsc_disable_lte_selection");
  ds3g_msh_disable_lte_selection_ex(subs_id);
#else
  DATA_MSG0(MSG_LEGACY_HIGH,"Feature not enabled: LTE");
#endif
  return;
}/* ds_dsc_disable_lte_selection() */

/*===========================================================================
FUNCTION      DS_DSC_ENABLE_LTE_SELECTION

DESCRIPTION   DSC to call CM API to disable LTE selection. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  Commands CM module
===========================================================================*/
void ds_dsc_enable_lte_selection(void)
{
#ifdef FEATURE_DATA_LTE
  sys_modem_as_id_e_type subs_id = ds3g_get_ps_subs_id(); // Vishal: Temporary

  DATA_MSG0(MSG_LEGACY_HIGH,"ds_dsc_enable_lte_selection");
  ds3g_msh_enable_lte_selection_ex(subs_id);
#else
  DATA_MSG0(MSG_LEGACY_HIGH,"Feature not enabled: LTE");
#endif
  return;
}/* ds_dsc_enable_lte_selection() */

/*===========================================================================
FUNCTION      DS_DSD_DSC_COMMAND_PROCESS

DESCRIPTION   Put DSC CMD to DS task
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  Commands CM module
===========================================================================*/
void ds_dsd_dsc_command_process
(
  ds_cmd_type *cmd_ptr
)
{
  boolean  lte_enable = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"DS cmd ptr is NULL");
    return;
  }

  lte_enable = *((boolean*)cmd_ptr->cmd_payload_ptr);

  if( lte_enable )
  {
    ds_dsc_enable_lte_selection();
  }
  else
  {
    ds_dsc_disable_lte_selection();
  }
  return;
}/* ds_dsd_dsc_command_process() */

/*===========================================================================
FUNCTION      DS_DSD_DSC_PUT_CMD

DESCRIPTION   Put DSC CMD to DS task
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  Commands CM module
===========================================================================*/
void ds_dsd_dsc_put_cmd
(
  ds_dsd_dsc_command_type cmd
)
{
  ds_cmd_type  *cmd_ptr = NULL;
  boolean      *data_ptr = NULL;
  boolean       notify = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG1(MSG_LEGACY_HIGH,"ds_dsd_dsc_put_cmd: %d", cmd);

  if ( cmd == DS_DSC_COMMAND_INVALID || cmd > DS_DSC_COMMAND_MAX ) 
  {
    DATA_MSG1(MSG_LEGACY_ERROR,"Invalid dsc cmd %d", cmd);
    return;
  }

  /*----------------------------------------------------------------------
  Switch the context to DS task
  ----------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(boolean));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    /*------------------------------------------------------------------
    This is Assert condition. However the system will recovered.
    -------------------------------------------------------------------*/
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_DSC_LTE_ENABLE;
  data_ptr = (boolean*)cmd_ptr->cmd_payload_ptr;

  switch (cmd) 
  {
    case DS_DSC_DISABLE_LTE:
      *data_ptr = FALSE;
      notify = TRUE;
      break;

    case DS_DSC_ENABLE_LTE:
      *data_ptr = TRUE;
      notify = TRUE;
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR,"Unsupported cmd %d",cmd);
      notify = FALSE;
      break;
  }

  if( notify == TRUE )
  {
    ds_put_cmd(cmd_ptr);
  }

  return;
}/* ds_dsd_dsc_put_cmd ()*/
#endif /*FEATURE_DATA_WLAN_MAPCON*/

