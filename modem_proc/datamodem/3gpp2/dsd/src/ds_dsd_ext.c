/*===========================================================================
                     D S _ D S D _ E X T . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2014 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_ext.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
08/23/13    rs    Added support for additional WLAN information 
11/11/11    sk    Created Module

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "event.h"
#include "queue.h"
#include "time_svc.h"
#include "rex.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#include "ds_dsd_ext_i.h"
#include "ds_dsd_ext.h"
#include "ds_dsd_common.h"
#include "ds_dsd_1xSm.h"
#include "ds_dsd_gsmSm.h"
#include "ds_dsd_hdrSm.h"
#include "ds_dsd_lteSm.h"
#include "ds_dsd_tdscdmaSm.h"
#include "ds_dsd_wcdmaSm.h"
#include "ds_dsd_wlanSm.h"
#include "ds_dsd_event_factory.h"
#include "ds_dsd_event_defs.h"
#include "ds_dsd_event_dp.h"
#include "ds_dsd_pref_sys.h"
#include "ds_dsd_proc.h"
#include "ds_dsd_timer.h"
#include "ds_dsd_efs_parser.h"
#include "ds_dsd_fmc_if.h"
#include "ds_dsd_apm.h"
#include "ds_dsd_apm_ext_i.h"
#include "data_msg.h"

#include "ps_sys.h"
#include "ds_sys_event.h"
#include "ps_sys_event.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ps_sys_conf.h"

#include "ds3gsubsmgr.h"
#include "ds3gdsdif.h"

#ifdef FEATURE_EPC_HANDOFF
#include "ds_epc_util.h"
#endif /* FEATURE_EPC_HANDOFF*/

/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/

/*---------------------------------------------------------------------------
  DSD Notify internal reasons
---------------------------------------------------------------------------*/
#define NOTIFY_FROM_APM      0x00

/*-------------------------------------------------------------------------
  Client registration info structure
-------------------------------------------------------------------------*/
typedef struct
{
  ds_dsd_ext_client_type      dsd_ext_client;
  ds_dsd_ext_event_type       dsd_ext_event;
}ds_dsd_ext_client_reg_info;

/*-------------------------------------------------------------------------
  DSD event data block structure used for queues
-------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                  link;
  ds_dsd_ext_client_type       dsd_ext_event_client_id;
  ds_dsd_ext_event_type        dsd_ext_event_id;
  ds_dsd_ext_notify_func_type  dsd_ext_event_notify_func_ptr;
  void                        *data_ptr;
}ds_dsd_ext_event_data_block;

typedef struct
{
  ps_sys_system_status_type   system_status_info;
  boolean                     roaming_status;
  ds3gsubsmgr_subs_id_e_type  subs_id;
}ds_dsd_ext_notify_ext_type;

/*===========================================================================

                      GLOBAL VARIABLES

===========================================================================*/

/*-------------------------------------------------------------------------
  DSD events queues
-------------------------------------------------------------------------*/
static q_type ds_dsd_ext_event_q = {{NULL}};
static q_type ds_dsd_ext_event_free_q = {{NULL}};

/*-------------------------------------------------------------------------
  DSD critical section
-------------------------------------------------------------------------*/
rex_crit_sect_type       ds_dsd_critical_section = {{0}};

/*-------------------------------------------------------------------------
  Array of DSD event data block
-------------------------------------------------------------------------*/
ds_dsd_ext_event_data_block ds_dsd_ext_event[DS_DSD_EXT_MAX_EV *
                                             DS_DSD_EXT_CLIENT_MAX] = {{{NULL}}};

/*-------------------------------------------------------------------------
  Function pointer to SS event complete callback
-------------------------------------------------------------------------*/
static ds_dsd_ext_notify_ss_event_complete_func_type 
                                ds_dsd_ext_ss_event_complete_cb = {NULL};

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_COMPARE_FUNCTION

DESCRIPTION   Utility function to compare two items.

DEPENDENCIES  None.

RETURN VALUE  1: if the items are equal
              0: if itemsa are not equal

SIDE EFFECTS  None.
===========================================================================*/
int ds_dsd_ext_compare_function
(
  void  *item_ptr,
  void  *target_val
)
{
  ds_dsd_ext_event_data_block  *data_block;
  ds_dsd_ext_client_reg_info   *to_compare;
  boolean                       ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((item_ptr == NULL) ||(target_val == NULL))
  {
    return FALSE;
  }
  data_block = (ds_dsd_ext_event_data_block *)item_ptr;
  to_compare = (ds_dsd_ext_client_reg_info *)target_val;

  if((data_block->dsd_ext_event_client_id == to_compare->dsd_ext_client) &&
     (data_block->dsd_ext_event_id == to_compare->dsd_ext_event))
  {
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }
  return ret_val;
}/* ds_dsd_ext_compare_function()*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_CHECK_SS_EVENT_IN_PROCESS_COMPLETE

DESCRIPTION   Checks if parsing of SS event in DSD is complete.

DEPENDENCIES  None.

RETURN VALUE  TRUE: SS Event is parsing is complete
              FALSE: SS event parsing is in progress

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_ext_check_ss_event_process_complete
(
  ds3gsubsmgr_subs_id_e_type subs_id
)
{
  boolean                   ret_val = FALSE;
  dsd_ext_notif_info_type  *ext_notif_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Get ext notif info for subs id
  -------------------------------------------------------------------------*/
  ext_notif_info = ds_dsd_proc_get_ext_notif_info(subs_id);
  if(ext_notif_info == NULL)
  {
    return FALSE;
  }

  /*--------------------------------------------------------------------
    SS event events currently spawns only the following  two commands
      - SYS_CHG_CMD
      - RECOMPUTE_CMD
  --------------------------------------------------------------------*/
  if((ext_notif_info->cmd_counter.sys_chg_cmd_cnt > 0) || 
     (ext_notif_info->cmd_counter.recompute_cmd_cnt > 0))
  {
    ret_val = FALSE;
  }
  else
  {
    ret_val = TRUE;
  }
  return ret_val;
}/* ds_dsd_ext_check_ss_event_in_progress() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_GET_RESOLVED_RADIO_RAT_MASK_FOR_NOTIFY

DESCRIPTION   This function returns the resolved rat mask of a state machine 
              belonging to the radio system. For example, when Srv status is
              not indicating full src, rat mask should be NULL BEARER.

DEPENDENCIES  None.

RETURN VALUE  rat_mask.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_dsd_ext_get_resolved_radio_rat_mask_for_notify
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint32 rat_mask = PS_SYS_RAT_3GPP_NULL_BEARER;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------- 
    If the radio is not available, return NULL BEARER
  ------------------------------------------------------------------------*/
  if(ds_dsd_pref_sys_get_radio_available_flag(radio, subs_id) == FALSE)
  {
    return rat_mask;
  }

  /*----------------------------------------------------------------------- 
    If the sm is throttled, return NULL BEARER
  ------------------------------------------------------------------------*/
  if(ds_dsd_common_is_sm_throttled(radio, subs_id) == TRUE)
  {
    return rat_mask;
  }

  /*----------------------------------------------------------------------- 
    get the current srv status of the radio, if not full srv, notify NULL
    BEARER
  ------------------------------------------------------------------------*/
  if(ds_dsd_common_get_radio_srv_status(radio,FALSE, subs_id) 
       == SYS_SRV_STATUS_SRV)
  {
    if((radio == DS_DSD_RADIO_LTE) &&
       (ds_dsd_apm_attach_completed(ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id)) 
         == FALSE))
    {
      return rat_mask;
    }
    else
    {
      rat_mask = ds_dsd_common_get_radio_rat_mask(radio, FALSE, subs_id);
    }
  }

  return rat_mask;
}  /*ds_dsd_ext_get_resolved_radio_rat_mask_for_notify*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_GET_RESOLVED_RADIO_SO_MASK_FOR_NOTIFY

DESCRIPTION   This function returns the resolved so mask of a state machine 
              belonging to the radio system. For example, when Srv status is
              not indicating full src, so mask should be 0.

DEPENDENCIES  None.

RETURN VALUE  so_mask.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_dsd_ext_get_resolved_radio_so_mask_for_notify
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint32 so_mask = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------- 
    If the radio is not available, return so_mask 0
  ------------------------------------------------------------------------*/
  if(ds_dsd_pref_sys_get_radio_available_flag(radio, subs_id) == FALSE)
  {
    return so_mask;
  }

  /*----------------------------------------------------------------------- 
    If the sm is throttled, return so_mask 0
  ------------------------------------------------------------------------*/
  if(ds_dsd_common_is_sm_throttled(radio, subs_id) == TRUE)
  {
    return so_mask;
  }

  /*----------------------------------------------------------------------- 
    get the current srv status of the radio, if not full srv, notify NULL
    BEARER
  ------------------------------------------------------------------------*/
  if(ds_dsd_common_get_radio_srv_status(radio, FALSE, subs_id)
       == SYS_SRV_STATUS_SRV)
  {
    if((radio == DS_DSD_RADIO_LTE) &&
       (ds_dsd_apm_attach_completed(ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id)) 
         == FALSE))
    {
      return so_mask;
    }
    else
    {
      so_mask = ds_dsd_common_get_radio_so_mask(radio, FALSE, subs_id);
    }
  }

  return so_mask;
} /*ds_dsd_ext_get_resolved_radio_so_mask_for_notify*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_INTERNAL

DESCRIPTION   This function notifies the entities inside modem about the
              preferred system change event. These clients are registered
              an event defined by the DSD module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Notifies all registered internal clients.
===========================================================================*/
void ds_dsd_ext_notify_internal
(
  ds_dsd_ext_event_type       dsd_ext_event,
  boolean                     mode_pref_notify,
  uint32                      notify_mask,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8                         q_count = 0;
  ds_dsd_ext_event_data_block  *event_block_temp_hold = NULL;
  ds_dsd_ext_event_info_type    *notified_event = NULL;
  uint8                         radio;
  dsd_radio_type                pref_sys;

#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_dsd_ext_wlan_addr_type     wlan_addr;
#endif /*FEATURE_DATA_WLAN_MAPCON*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  pref_sys = ds_dsd_pref_sys_get_pref_system(subs_id);

  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_ext_notify_pref_system_internal event:%d DSD pref_sys=%d",
            dsd_ext_event,
            ds_dsd_ext_translate_dsd_sys_to_ss(pref_sys));

  notified_event = (ds_dsd_ext_event_info_type *)modem_mem_alloc(
                                  sizeof(ds_dsd_ext_event_info_type),
                                  MODEM_MEM_CLIENT_DATA_CRIT);

  if(notified_event == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Could not get notified_event memory");
    return;
  }

  memset(notified_event,0,sizeof(ds_dsd_ext_event_info_type));
  notified_event->mode_pref_notify = mode_pref_notify;

  q_count = q_cnt(&ds_dsd_ext_event_q);
  while(q_count--)
  {
    event_block_temp_hold = (ds_dsd_ext_event_data_block *)
                             q_get(&ds_dsd_ext_event_q);
    if(event_block_temp_hold)
    {
      if(dsd_ext_event == event_block_temp_hold->dsd_ext_event_id)
      {
        /* Fill event info structure */
        notified_event->subs_id = ds3gsubsmgr_subs_id_ds3g_to_ds(subs_id);
        notified_event->pref_index = pref_sys - DS_DSD_RADIO_MIN;

        /*This radio starts from Zero and is used to index into array, 
           whereas DS_DSD_RADIO enum starts with 1*/
        for(radio=0; radio<DS_DSD_RADIO_MAX; radio++)
        {
          notified_event->dsd_radio_info[radio].changed_fields 
            = ds_dsd_pref_sys_get_changed_fields_mask(DS_DSD_RADIO_MIN+radio,
                                                      subs_id);

          /* If notify from APM mask is set, it means APM attach is complete
             So LTE's changed field mask to RAT and SO changed */
          if( ((DS_DSD_RADIO_MIN+radio) == DS_DSD_RADIO_LTE) &&
              (notify_mask & (1 << NOTIFY_FROM_APM)) )
          {
            notified_event->dsd_radio_info[radio].changed_fields 
              |= NOTIFY_REASON_RAT_CHANGE|NOTIFY_REASON_SO_CHANGE;
          }

          notified_event->dsd_radio_info[radio].system_mode 
            = ds_dsd_ext_translate_dsd_sys_to_ss(DS_DSD_RADIO_MIN+radio);

          notified_event->dsd_radio_info[radio].srv_status
            = ds_dsd_common_get_radio_srv_status(DS_DSD_RADIO_MIN+radio,
                                                 FALSE, subs_id);

          notified_event->dsd_radio_info[radio].srv_domain
            = ds_dsd_common_get_radio_srv_domain(DS_DSD_RADIO_MIN+radio,
                                                 FALSE, subs_id);

          notified_event->dsd_radio_info[radio].rat_mask
            = ds_dsd_ext_get_resolved_radio_rat_mask_for_notify(
                                                       DS_DSD_RADIO_MIN+radio,
                                                       subs_id);

          notified_event->dsd_radio_info[radio].so_mask
            = ds_dsd_ext_get_resolved_radio_so_mask_for_notify(
                                                       DS_DSD_RADIO_MIN+radio,
                                                       subs_id);

          notified_event->dsd_radio_info[radio].cell_srv_ind
            = ds_dsd_common_get_radio_cell_srv_ind(DS_DSD_RADIO_MIN+radio,
                                                   FALSE,
                                                   subs_id);

          notified_event->dsd_radio_info[radio].remote_flag
            = ds_dsd_common_get_remote_flag(DS_DSD_RADIO_MIN+radio,
                                            subs_id);

          notified_event->dsd_radio_info[radio].roaming_status
            = ds_dsd_common_get_radio_roam_status(DS_DSD_RADIO_MIN+radio,
                                                  FALSE,
                                                  subs_id);

#ifdef FEATURE_DATA_WLAN_MAPCON
          notified_event->dsd_radio_info[radio].is_wlan_available
            = ds_dsd_get_wlan_available(subs_id);

          if(notified_event->dsd_radio_info[radio].is_wlan_available)
          {
            memset(&wlan_addr, 0, sizeof(ds_dsd_ext_wlan_addr_type));
            ds_dsd_wlansm_get_address(&wlan_addr, FALSE, subs_id);

            memscpy(&(notified_event->dsd_radio_info[radio].wlan_ipv4_addr),
                    sizeof(ps_ip_addr_type),
                    &(wlan_addr.wlan_ipv4_addr),
                    sizeof(ps_ip_addr_type));

            memscpy(&(notified_event->dsd_radio_info[radio].wlan_ipv6_addr),
                    sizeof(ps_ip_addr_type),
                    &(wlan_addr.wlan_ipv6_addr),
                    sizeof(ps_ip_addr_type)); 

            notified_event->dsd_radio_info[radio].wlan_ipv6_prefix_len 
              = wlan_addr.wlan_ipv6_prefix_len;

            memscpy(notified_event->dsd_radio_info[radio].wlan_mac_addr,
                    PS_SYS_MAC_ADDR_LEN,
                    wlan_addr.wlan_mac_addr,
                    PS_SYS_MAC_ADDR_LEN);

            memscpy(&(notified_event->dsd_radio_info[radio].wlan_dns_addr),
                    sizeof(wlan_addr_type),
                    &(wlan_addr.wlan_dns_addr),
                    sizeof(wlan_addr_type));

            memscpy(&(notified_event->dsd_radio_info[radio].wlan_epdg_addr),
                    sizeof(wlan_addr_type),
                    &(wlan_addr.wlan_epdg_addr),
                    sizeof(wlan_addr_type));

            strlcpy(notified_event->dsd_radio_info[radio].wlan_ssid,
                    wlan_addr.wlan_ssid,
                    PS_SYS_MAX_SSID_LEN);

          }
#endif /* FEATURE_DATA_WLAN_MAPCON */

          /* update changed radio mask if radio value has been changed */
          if(notified_event->dsd_radio_info[radio].changed_fields != 0)
          {
            notified_event->changed_radio_mask |= (1<<radio);
          }
        }

        /*------------------------------------------------------------------
          Notify this client
        -------------------------------------------------------------------*/
        if(event_block_temp_hold->dsd_ext_event_notify_func_ptr != NULL)
        {
          event_block_temp_hold->dsd_ext_event_notify_func_ptr(
                              dsd_ext_event,
                              notified_event,
                              event_block_temp_hold->data_ptr);
        }
      }
      q_put(&ds_dsd_ext_event_q, &(event_block_temp_hold->link));
    }
  }
  modem_mem_free((void *)notified_event, MODEM_MEM_CLIENT_DATA_CRIT );
  return;
}/* ds_dsd_ext_notify_internal() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_PREF_SYSTEM_COMMON


DESCRIPTION   This is static function and called by lower registered element
              when system change is to be notified to external world.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_notify_pref_system_common
(
  dsd_radio_type              old_radio,
  dsd_radio_type              new_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  sys_sys_mode_e_type       new_pref_sys;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  new_pref_sys = ds_dsd_ext_translate_dsd_sys_to_ss(new_radio);

  /*------------------------------------------------------------------------
  Notify Application through PS framework. This is radio independent
  bearer tech notification.
  ------------------------------------------------------------------------*/
  DATA_MSG3(MSG_LEGACY_LOW,"DSD new pref sys=%d ,old_radio: %d new_radio: %d ",
            new_pref_sys,old_radio,new_radio);

  /*------------------------------------------------------------------------
    Notify from DSD only if FMC is not present
  ------------------------------------------------------------------------*/
  if(ds_dsd_fmc_if_get_fmc_state() == FALSE)
  {
    ds_dsd_ext_notify_pref_system_external(old_radio, new_radio, subs_id);
  }

  return;
} /* ds_dsd_ext_notify_pref_system_common() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_PREF_SYSTEM


DESCRIPTION   This is static function and called by lower registered element
              when system change is to be notified to external world.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_notify_pref_system
(
  dsd_radio_type              old_radio,
  dsd_radio_type              new_radio,
  boolean                     mode_pref_notify,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                   notify_ext_clients = FALSE;
  dsd_ext_notif_info_type  *ext_notif_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Notify all internal clients registered for pref_sys_chg event
  -------------------------------------------------------------------------*/
  ds_dsd_ext_notify_internal(DS_DSD_EXT_PREF_SYS_CHANGED_EV, 
                             mode_pref_notify,
                             0,
                             subs_id);

  /*-------------------------------------------------------------------------
    Get ext notif info for subs id
  -------------------------------------------------------------------------*/
  ext_notif_info = ds_dsd_proc_get_ext_notif_info(subs_id);
  if(ext_notif_info == NULL)
  {
    return;
  }

  rex_enter_crit_sect( &ds_dsd_critical_section );
  /*-------------------------------------------------------------------------
    Clear existing pending notification if not applicable
  -------------------------------------------------------------------------*/
  if( (ext_notif_info->ext_notify_pending == TRUE) &&
      (ext_notif_info->new_radio_pending != new_radio))
  {
      DATA_MSG1(MSG_LEGACY_MED, "Clear pending flag for subs_id:%d", subs_id);
      ext_notif_info->ext_notify_pending = FALSE;
      ext_notif_info->old_radio_pending  = DS_DSD_RADIO_INVALID;
      ext_notif_info->new_radio_pending  = DS_DSD_RADIO_INVALID;
  }

  /*-------------------------------------------------------------------------
    Check if external notification allowed
  -------------------------------------------------------------------------*/
  if(new_radio == DS_DSD_RADIO_LTE)
  {
    /*------------------------------------------------------------------------
     Check the current LTE srv status and APM status.
      - If LTE is not in SRV, go ahead.
      - If LTE is in SRV (from CM) but APM is not ready, wait for APM
        indicating attach completion.
    ------------------------------------------------------------------------*/
    if(ds_dsd_common_get_radio_srv_status(DS_DSD_RADIO_LTE, FALSE, subs_id) 
         == SYS_SRV_STATUS_SRV  &&
       ds_dsd_apm_attach_completed(ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id)) 
         == FALSE)
    {
      DATA_MSG1(MSG_LEGACY_HIGH,
                "new radio:%d APM attach pending. Wait for APM ind",
                new_radio);

      ext_notif_info->ext_notify_pending = TRUE;
      ext_notif_info->old_radio_pending = old_radio;
      ext_notif_info->new_radio_pending = new_radio;
    }
  }
  else if((new_radio == DS_DSD_RADIO_HDR)||(new_radio == DS_DSD_RADIO_1X))
  {
    DATA_MSG1(MSG_LEGACY_HIGH, "new radio:%d. Wait for 3GPP2 MH ind",
              new_radio);
    ext_notif_info->ext_notify_pending = TRUE;
    ext_notif_info->old_radio_pending = old_radio;
    ext_notif_info->new_radio_pending = new_radio;
  }
  else
  {
    if(ext_notif_info->ext_notify_pending == TRUE)
    {
      DATA_MSG1(MSG_LEGACY_MED, "Clear pending flag for subs_id:%d", subs_id);
      ext_notif_info->ext_notify_pending = FALSE;
      ext_notif_info->old_radio_pending  = DS_DSD_RADIO_INVALID;
      ext_notif_info->new_radio_pending  = DS_DSD_RADIO_INVALID;
    }
  }
  notify_ext_clients = (ext_notif_info->ext_notify_pending == FALSE);

  rex_leave_crit_sect( &ds_dsd_critical_section );
 
  /*------------------------------------------------------------------------
   Conditionally notify external clients 
  ------------------------------------------------------------------------*/
  if(notify_ext_clients == TRUE)
  {
    ds_dsd_ext_notify_pref_system_common(old_radio, new_radio, subs_id);
  }
}/* ds_dsd_ext_notify_pref_system() */

/*===========================================================================
FUNCTION       DS_DSD_EXT_EXTERNAL_NOTIFY_CMD_PROCESSING

DESCRIPTION   This function processes the external client notification cmd.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds_dsd_ext_external_notify_cmd_processing
(
  ds_cmd_type  *cmd_ptr
)
{
  int16                        ps_errno;
  ps_sys_system_status_type   *sys_info = NULL;
  ps_sys_network_enum_type     pref_nw;
  ds_dsd_ext_notify_ext_type  *notify_ext_info = NULL;
  ps_sys_roaming_status_info_type  roam_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_HIGH,"NULL cmd ptr passed");
    return;
  }

  notify_ext_info = (ds_dsd_ext_notify_ext_type*)cmd_ptr->cmd_payload_ptr;
  sys_info = &(notify_ext_info->system_status_info);
  pref_nw = sys_info->preferred_network;

  DATA_MSG3(MSG_LEGACY_HIGH,
            "Notify external client: pref network=%d, rat_mask=%d so_mask=%d",
            pref_nw,
            sys_info->network_info[pref_nw].rat_mask,
            sys_info->network_info[pref_nw].so_mask);

  ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                  PS_SYS_CONF_SYSTEM_STATUS,
                     (ps_sys_subscription_enum_type)
                       ds3gsubsmgr_subs_id_ds3g_to_ds(notify_ext_info->subs_id),
                     (void *)notify_ext_info,
                     &ps_errno);

    memset(&roam_info, 0, sizeof(ps_sys_roaming_status_info_type));
    roam_info.roaming_status = notify_ext_info->roaming_status;
    roam_info.technology = (ds_sys_network_enum_type)pref_nw;

    ps_sys_conf_set_ex(PS_SYS_TECH_ALL, 
                       PS_SYS_CONF_ROAMING_STATUS_INFO,
                       (ps_sys_subscription_enum_type)
                        ds3gsubsmgr_subs_id_ds3g_to_ds(notify_ext_info->subs_id),
                       (void *)&roam_info,
                       &ps_errno);

  /*-------------------------------------------------------------------------
    Decrement DSD's command counter
  -------------------------------------------------------------------------*/
  ds_dsd_ext_decr_cmd_counter(notify_ext_info->subs_id, DS_CMD_DSD_EXT_NOTIFY);

  return;
}/* ds_dsd_ext_external_notify_cmd_processing() */

/*===========================================================================
FUNCTION       DS_DSD_EXT_MODE_PREF_CHG_CMD_PROCESS

DESCRIPTION    Processes mode preference change command.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds_dsd_ext_mode_pref_chg_cmd_processing
(
  ds_cmd_type  *cmd_ptr
)
{
  ds_dsd_mode_pref_cmd_info_type  *dsd_mode_pref_cmd_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_HIGH, "NULL ptr. return.");
    return;
  }

  dsd_mode_pref_cmd_info
    = (ds_dsd_mode_pref_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  DATA_MSG2(MSG_LEGACY_HIGH,
            "ds_dsd_ext_mode_pref_chg_cmd_processing. mode_pref:%d subs_id:%d",
            dsd_mode_pref_cmd_info->mode_pref,
            dsd_mode_pref_cmd_info->subs_id);
  
  /* Notify mode preference change*/  
  ds_dsd_pref_sys_mode_pref_chg(dsd_mode_pref_cmd_info->mode_pref,
                                dsd_mode_pref_cmd_info->subs_id);

  /*--------------------------------------------------------------------
    Decrement DSD's command counter
  --------------------------------------------------------------------*/
  ds_dsd_ext_decr_cmd_counter(dsd_mode_pref_cmd_info->subs_id,
                              DS_CMD_DSD_MODE_PREF_CHG);

  return;
}/* ds_dsd_ext_mode_pref_chg_cmd_processing() */

/*===========================================================================
FUNCTION       DS_DSD_EXT_LTE_EMERG_CALL_CMD_PROCESSING

DESCRIPTION    Processes LTE emergency call event command.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds_dsd_ext_lte_emerg_call_cmd_processing
(
  ds_cmd_type  *cmd_ptr
)
{
  ds_dsd_lte_emerg_call_cmd_type       *dsd_lte_emerg_cmd_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_HIGH, "NULL ptr. return.");
    return;
  }

  dsd_lte_emerg_cmd_info
    = (ds_dsd_lte_emerg_call_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_MSG2(MSG_LEGACY_HIGH,
            "ds_dsd_ext_lte_emerg_call_cmd_processing. in_prog:%d subs_id:%d",
            dsd_lte_emerg_cmd_info->in_prog,
            dsd_lte_emerg_cmd_info->subs_id);
  
  /* Notify mode preference change*/  
  ds_dsd_pref_sys_lte_emerg_call_in_prog( dsd_lte_emerg_cmd_info->in_prog,
                                          dsd_lte_emerg_cmd_info->subs_id);

  /*--------------------------------------------------------------------
    Decrement DSD's command counter
  --------------------------------------------------------------------*/
  ds_dsd_ext_decr_cmd_counter(dsd_lte_emerg_cmd_info->subs_id,
                              DS_CMD_DSD_LTE_EMERG_CALL_EVENT);

  return;
}/* ds_dsd_ext_lte_emerg_call_cmd_processing() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_RESET_CMD_PROCESS

DESCRIPTION   Processing of DSD Reset cmd. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Resets all SM and remputes preferred system.
===========================================================================*/
void ds_dsd_ext_reset_cmd_process
(
  ds_cmd_type  *cmd_ptr
)
{
  ds_dsd_reset_cmd_info_type  *dsd_reset_cmd_info = NULL;
  ds3gsubsmgr_subs_id_e_type   subs_id;
  ds_dsd_reset_enum_type       reset_type;
  dsd_radio_type               dsd_radio;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_HIGH, "NULL ptr. return.");
    return;
  }

  dsd_reset_cmd_info = (ds_dsd_reset_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
  subs_id = dsd_reset_cmd_info->subs_id;
  reset_type = dsd_reset_cmd_info->reset_type;

  DATA_MSG1(MSG_LEGACY_MED, "ds_dsd_ext_reset_cmd_process. subs_id:%d",
            subs_id);

  /* Reset all statemachines */
  for(dsd_radio = DS_DSD_RADIO_MIN; dsd_radio <= DS_DSD_RADIO_MAX; dsd_radio++)
  {
    /*----------------------------------------------------------------------- 
       Wifi can still be available during nv refresh. Do no reset the WLAN sm
       as Wifi availalbe indication needs to be passed to MPPM after refresh
       is complete
    ------------------------------------------------------------------------*/
    if (dsd_radio == DS_DSD_RADIO_WLAN && reset_type ==DS_DSD_RESET_NV_REFRESH)
    {
      continue;
    }

    ds_dsd_common_reset_sm_instance(subs_id, dsd_radio);
  }

  /* Stop timers */
  ds_dsd_timer_reset(subs_id);

  /* Re-compute best system */
  ds_dsd_pref_sys_recompute_best_system(subs_id);

  /*--------------------------------------------------------------------
    Decrement DSD's command counter
  --------------------------------------------------------------------*/
  ds_dsd_ext_decr_cmd_counter(subs_id, DS_CMD_DSD_RESET);

  return;
}/* ds_dsd_ext_reset_cmd_process() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_INIT

DESCRIPTION   This function initializes the ext system. This is during
              power up.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  1. Updates the preferred system.
              2. Notifes all clients if the preferred system changes.
===========================================================================*/
void ds_dsd_ext_init(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds_dsd_pref_sys_reg_notify(ds_dsd_ext_notify_pref_system);
  return;
}/* ds_dsd_ext_init() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_TRANSLATE_SS_TO_DSD_RADIO

DESCRIPTION   This is a utility to translate DS RADIO type to SYS type.

DEPENDENCIES  None.

RETURN VALUE  DSD radio.

SIDE EFFECTS  None.
===========================================================================*/
dsd_radio_type  ds_dsd_ext_translate_ss_to_dsd_radio
(
  sys_sys_mode_e_type  ss_sys
)
{
  dsd_radio_type translated_sys = DS_DSD_RADIO_INVALID;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(ss_sys)
  {
    case SYS_SYS_MODE_LTE:
      translated_sys =  DS_DSD_RADIO_LTE;
      break;

    case  SYS_SYS_MODE_HDR:
      translated_sys = DS_DSD_RADIO_HDR;
      break;

    case SYS_SYS_MODE_CDMA:
      translated_sys = DS_DSD_RADIO_1X;
      break;

    case SYS_SYS_MODE_GSM:
      translated_sys = DS_DSD_RADIO_GSM;
      break;

    case SYS_SYS_MODE_TDS:
      translated_sys = DS_DSD_RADIO_TDSCDMA;
      break;

    case SYS_SYS_MODE_WCDMA:
      translated_sys = DS_DSD_RADIO_WCDMA;
      break;

    case SYS_SYS_MODE_WLAN:
      translated_sys = DS_DSD_RADIO_WLAN;
      break;

    default:
      break;
  }
  return translated_sys;
}/* ds_dsd_common_translate_ss_to_dsd_radio()*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_TRANSLATE_DSD_SYS_TO_SS

DESCRIPTION   This is a utility to translate DS RADIO type to SYS type
 
DEPENDENCIES  None.

RETURN VALUE  CM SS type.

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds_dsd_ext_translate_dsd_sys_to_ss
(
  dsd_radio_type dsd_radio
)
{
  sys_sys_mode_e_type translated_sys = SYS_SYS_MODE_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(dsd_radio)
  {
    case DS_DSD_RADIO_LTE:
      translated_sys = SYS_SYS_MODE_LTE;
      break;

    case  DS_DSD_RADIO_HDR:
      translated_sys = SYS_SYS_MODE_HDR;
      break;

    case DS_DSD_RADIO_1X:
      translated_sys = SYS_SYS_MODE_CDMA;
      break;

    case DS_DSD_RADIO_GSM:
      translated_sys = SYS_SYS_MODE_GSM;
      break;

    case DS_DSD_RADIO_TDSCDMA:
      translated_sys = SYS_SYS_MODE_TDS;
      break;

    case DS_DSD_RADIO_WCDMA:
      translated_sys = SYS_SYS_MODE_WCDMA;
      break;

    case DS_DSD_RADIO_WLAN:
      translated_sys = SYS_SYS_MODE_WLAN;
      break;

    default:
      break;
  }
  return translated_sys;
}/* ds_dsd_ext_translate_dsd_sys_to_ss()*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_INCR_EVENT_COUNTER

DESCRIPTION   Increment the cmd counter.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_incr_cmd_counter
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  ds_cmd_enum_type            cmd_id
)
{
  uint8                     tmp_counter = 0;
  dsd_ext_notif_info_type  *ext_notif_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Get ext notif info for subs id
  -------------------------------------------------------------------------*/
  ext_notif_info = ds_dsd_proc_get_ext_notif_info(subs_id);
  if(ext_notif_info == NULL)
  {
    return;
  }

  switch(cmd_id)
  {
    case DS_CMD_DSD_TIMER_EXPIRED:
      ext_notif_info->cmd_counter.timer_expired_cmd_cnt++;
      tmp_counter = ext_notif_info->cmd_counter.timer_expired_cmd_cnt;
      break;

    case DS_CMD_DSD_EVENT_NOTIFY:
      ext_notif_info->cmd_counter.sys_chg_cmd_cnt++;
      tmp_counter = ext_notif_info->cmd_counter.sys_chg_cmd_cnt;
      break;

    case DS_CMD_DSD_RECOMPUTE:
      ext_notif_info->cmd_counter.recompute_cmd_cnt++;
      tmp_counter = ext_notif_info->cmd_counter.recompute_cmd_cnt;
      break;

    case DS_CMD_DSD_EXT_NOTIFY:
      ext_notif_info->cmd_counter.ext_notify_cmd_cnt++;
      tmp_counter = ext_notif_info->cmd_counter.ext_notify_cmd_cnt;
      break;

    case DS_CMD_DSD_MODE_PREF_CHG:
      ext_notif_info->cmd_counter.mode_pref_chg_cmd_cnt++;
      tmp_counter = ext_notif_info->cmd_counter.mode_pref_chg_cmd_cnt;
      break;

    case DS_CMD_DSD_RESET:
      ext_notif_info->cmd_counter.dsd_reset_cmd_cnt++;
      tmp_counter = ext_notif_info->cmd_counter.dsd_reset_cmd_cnt;
      break;

    case DS_CMD_DSD_LTE_EMERG_CALL_EVENT:
      ext_notif_info->cmd_counter.lte_emerg_cmd_cnt++;
      tmp_counter = ext_notif_info->cmd_counter.lte_emerg_cmd_cnt;
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR,"Command %d: not for DSD",cmd_id);
      break;
  }

  DATA_MSG2(MSG_LEGACY_LOW, "ds_dsd_ext_incr_cmd_counter. cmd_id:%d, ctr:%d",
            cmd_id, tmp_counter);

  return;
}/* ds_dsd_ext_incr_event_counter() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_DECR_EVENT_COUNTER

DESCRIPTION   Decrement the event counter.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_decr_cmd_counter
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  ds_cmd_enum_type            cmd_id
)
{
  dsd_ext_notif_info_type  *ext_notif_info = NULL;
  uint8                     tmp_counter = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Get ext notif info for subs id
  -------------------------------------------------------------------------*/
  ext_notif_info = ds_dsd_proc_get_ext_notif_info(subs_id);
  if(ext_notif_info == NULL)
  {
    return;
  }

  switch(cmd_id)
  {
    case DS_CMD_DSD_TIMER_EXPIRED:
      if(ext_notif_info->cmd_counter.timer_expired_cmd_cnt > 0)
      {
        ext_notif_info->cmd_counter.timer_expired_cmd_cnt--;
      }
      tmp_counter = ext_notif_info->cmd_counter.timer_expired_cmd_cnt;
      break;

    case DS_CMD_DSD_EVENT_NOTIFY:
      if(ext_notif_info->cmd_counter.sys_chg_cmd_cnt > 0)
      {
        ext_notif_info->cmd_counter.sys_chg_cmd_cnt--;
      }
      tmp_counter = ext_notif_info->cmd_counter.sys_chg_cmd_cnt;
      break;

    case DS_CMD_DSD_RECOMPUTE:
      if(ext_notif_info->cmd_counter.recompute_cmd_cnt > 0)
      {
        ext_notif_info->cmd_counter.recompute_cmd_cnt--;
      }
      tmp_counter = ext_notif_info->cmd_counter.recompute_cmd_cnt;
      break;

    case DS_CMD_DSD_EXT_NOTIFY:
      if(ext_notif_info->cmd_counter.ext_notify_cmd_cnt > 0)
      {
        ext_notif_info->cmd_counter.ext_notify_cmd_cnt--;
      }
      tmp_counter = ext_notif_info->cmd_counter.ext_notify_cmd_cnt;
      break;

    case DS_CMD_DSD_MODE_PREF_CHG:
      if(ext_notif_info->cmd_counter.mode_pref_chg_cmd_cnt > 0)
      {
        ext_notif_info->cmd_counter.mode_pref_chg_cmd_cnt--;
      }
      tmp_counter = ext_notif_info->cmd_counter.mode_pref_chg_cmd_cnt;
      break;

    case DS_CMD_DSD_RESET:
      if(ext_notif_info->cmd_counter.dsd_reset_cmd_cnt > 0)
      {
        ext_notif_info->cmd_counter.dsd_reset_cmd_cnt--;
      }
      tmp_counter = ext_notif_info->cmd_counter.dsd_reset_cmd_cnt;
      break;

    case DS_CMD_DSD_LTE_EMERG_CALL_EVENT:
      if(ext_notif_info->cmd_counter.lte_emerg_cmd_cnt > 0)
      {
        ext_notif_info->cmd_counter.lte_emerg_cmd_cnt--;
      }
      tmp_counter = ext_notif_info->cmd_counter.lte_emerg_cmd_cnt;
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR, "Command %d: not for DSD", cmd_id);
      break;
  }

  DATA_MSG2(MSG_LEGACY_MED, "ds_dsd_ext_decr_cmd_counter. cmd_id:%d, ctr:%d",
            cmd_id, tmp_counter);

  return;
}/* ds_dsd_ext_decr_event_counter() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_PUT_CMD

DESCRIPTION   DSD wrapper function for DS put command functions. 
              Caller needs free the memory allocated to data_ptr.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_put_cmd
(
  ds_dsd_cmd_queue_enum_type  cmd_q_id,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  ds_cmd_type                *cmd_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(cmd_ptr ==  NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "NULL pointer passed");
    return;
  }

  DATA_MSG2(MSG_LEGACY_HIGH, "ds_dsd_ext_put_cmd. cmd_q_id:%d cmd_id:%d",
            cmd_q_id, cmd_ptr->hdr.cmd_id);

  /*--------------------------------------------------------------------
    Put command on either DS Task's regular queue or internal queue
  --------------------------------------------------------------------*/
  switch(cmd_q_id) 
  {
    case DS_DSD_INTERNAL_CMD_QUEUE:
      ds_dsd_ext_incr_cmd_counter(subs_id, cmd_ptr->hdr.cmd_id);
      ds_put_cmd(cmd_ptr);
      break;

    case DS_DSD_REGULAR_CMD_QUEUE:
      ds_dsd_ext_incr_cmd_counter(subs_id, cmd_ptr->hdr.cmd_id);
      ds_put_cmd_ext2(cmd_ptr);
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR, "Invalid Command Queue %d",
                cmd_ptr->hdr.cmd_id);
      break;
  }

  return;
}/* ds_dsd_ext_put_cmd() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_PREF_SYSTEM_EXTERNAL

DESCRIPTION   This function notifies the entities outside modem about the
              preferred system change event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_notify_pref_system_external
(
  dsd_radio_type              old_pref_sys,
  dsd_radio_type              new_pref_sys,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ps_sys_system_status_type    dsd_sys_info;
  boolean                      report = TRUE;
  ds_cmd_type                 *cmd_ptr = NULL;
  ds_dsd_ext_notify_ext_type  *data_block_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type   def_data_subs_id;
  boolean                      roaming_status = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  def_data_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(
                              ds3gsubsmgr_get_def_data_subs_id());

  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_ext_notify_pref_system_external. old sys: %d new sys: %d",
            old_pref_sys,new_pref_sys);

  memset(&dsd_sys_info, 0, sizeof(ps_sys_system_status_type));

  roaming_status = ds_dsd_common_get_radio_roam_status(new_pref_sys,
                                                       FALSE,
                                                       subs_id);

  switch(new_pref_sys)
  {
    case DS_DSD_RADIO_1X:
      dsd_sys_info.preferred_network = PS_SYS_NETWORK_3GPP2;
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].rat_mask =
              ds_dsd_ext_get_resolved_radio_rat_mask_for_notify(new_pref_sys,
                                                                subs_id);
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].so_mask =
              ds_dsd_ext_get_resolved_radio_so_mask_for_notify(new_pref_sys,
                                                               subs_id);
      break;

    case DS_DSD_RADIO_GSM:
    case DS_DSD_RADIO_TDSCDMA:
    case DS_DSD_RADIO_WCDMA:
      dsd_sys_info.preferred_network = PS_SYS_NETWORK_3GPP;
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP].rat_mask =
                ds_dsd_ext_get_resolved_radio_rat_mask_for_notify(new_pref_sys,
                                                                  subs_id);
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP].so_mask =
                ds_dsd_ext_get_resolved_radio_so_mask_for_notify(new_pref_sys,
                                                                 subs_id);
      break;

    case DS_DSD_RADIO_HDR:
      dsd_sys_info.preferred_network = PS_SYS_NETWORK_3GPP2;
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].rat_mask =
                ds_dsd_ext_get_resolved_radio_rat_mask_for_notify(new_pref_sys,
                                                                  subs_id);
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].so_mask =
                ds_dsd_ext_get_resolved_radio_so_mask_for_notify(new_pref_sys,
                                                                 subs_id);
      break;

    case DS_DSD_RADIO_LTE:
      dsd_sys_info.preferred_network = PS_SYS_NETWORK_3GPP;
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP].rat_mask =
                ds_dsd_ext_get_resolved_radio_rat_mask_for_notify(new_pref_sys,
                                                                  subs_id);
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP].so_mask = 0;
      break;

     case DS_DSD_RADIO_OOS:
      switch(old_pref_sys)
      {
        case DS_DSD_RADIO_LTE:
        case DS_DSD_RADIO_GSM:
        case DS_DSD_RADIO_TDSCDMA:
        case DS_DSD_RADIO_WCDMA:
          dsd_sys_info.preferred_network = PS_SYS_NETWORK_3GPP;
          dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP].rat_mask =
            PS_SYS_RAT_3GPP_NULL_BEARER;
          dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP].so_mask = 0;
          break;

      case DS_DSD_RADIO_HDR:
        case DS_DSD_RADIO_1X:
          dsd_sys_info.preferred_network = PS_SYS_NETWORK_3GPP2;
          dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].rat_mask =
            PS_SYS_RAT_3GPP2_NULL_BEARER;
          dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].so_mask = 0;
          break;

      default:
          DATA_MSG0(MSG_LEGACY_ERROR, "Unknown old radio");
          report = FALSE;
          break;
      }
      break;

    default:
      report = FALSE;
      break;
  }

  if(report == TRUE)
  {
    /*--------------------------------------------------------------------
      Get command buffer for new command
    --------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_ext_notify_ext_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      return;
    }

    data_block_ptr = (ds_dsd_ext_notify_ext_type *)cmd_ptr->cmd_payload_ptr;

    /* copy the PS sys info to data block pointer */
    data_block_ptr->subs_id = subs_id;
    data_block_ptr->roaming_status = roaming_status;
    memscpy(&(data_block_ptr->system_status_info),
            sizeof(ps_sys_system_status_type), 
            &dsd_sys_info, 
            sizeof(ps_sys_system_status_type));
  
    /*------------------------------------------------------------------
      Post the command on external DS queue
    -------------------------------------------------------------------*/
    cmd_ptr->hdr.cmd_id = DS_CMD_DSD_EXT_NOTIFY;
    ds_dsd_ext_put_cmd(DS_DSD_REGULAR_CMD_QUEUE, subs_id, cmd_ptr);
  }

  return;
}/* ds_dsd_ext_notify_pref_system_external()*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_PREF_SYSTEM_APM


DESCRIPTION   This is static function and called by lower registered element
              when system change is to be notified to external world.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_notify_pref_system_apm
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  dsd_ext_notif_info_type  *ext_notif_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Get ext notif info for subs id
  -------------------------------------------------------------------------*/
  ext_notif_info = ds_dsd_proc_get_ext_notif_info(subs_id);
  if(ext_notif_info == NULL)
  {
    return;
  }

  rex_enter_crit_sect( &ds_dsd_critical_section );
  if(ext_notif_info->ext_notify_pending == TRUE)
  {
    /* check if indication complete is for the current preferred system */
    if(ext_notif_info->new_radio_pending == DS_DSD_RADIO_LTE)
    {
      DATA_MSG0(MSG_LEGACY_HIGH, "Notification Pending");
      ds_dsd_ext_notify_internal(DS_DSD_EXT_PREF_SYS_CHANGED_EV,
                                 FALSE,
                                 (1 << NOTIFY_FROM_APM),
                                 subs_id);

      
      ds_dsd_ext_notify_pref_system_common(ext_notif_info->old_radio_pending, 
                                           ext_notif_info->new_radio_pending,
                                           subs_id);
      ext_notif_info->ext_notify_pending = FALSE;
      ext_notif_info->old_radio_pending  = DS_DSD_RADIO_INVALID;
      ext_notif_info->new_radio_pending  = DS_DSD_RADIO_INVALID;
    }
  }
  rex_leave_crit_sect( &ds_dsd_critical_section );
}/* ds_dsd_ext_notify_pref_system_apm() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_PREF_SYSTEM_IND_COMPLETED


DESCRIPTION   This function is called by mode handler to notify it has 
              completed the parsing preferred system indication.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_dsd_ext_pref_system_ind_completed
(
  sys_sys_mode_e_type         sys_mode,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  dsd_radio_type            notified_dsd_radio;
  dsd_ext_notif_info_type  *ext_notif_info = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((sys_mode <= SYS_SYS_MODE_NO_SRV) || (sys_mode >= SYS_SYS_MODE_MAX))
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Invalid sys_mode: %d", sys_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Get ext notif info for subs id
  -------------------------------------------------------------------------*/
  ext_notif_info = ds_dsd_proc_get_ext_notif_info(subs_id);
  if(ext_notif_info == NULL)
  {
    return;
  }

  DATA_MSG1(MSG_LEGACY_HIGH,
            "ds_dsd_ext_pref_system_ind_completed. sys_mode: %d",
            sys_mode);

  notified_dsd_radio = ds_dsd_ext_translate_ss_to_dsd_radio(sys_mode);

  rex_enter_crit_sect( &ds_dsd_critical_section );
  if(ext_notif_info->ext_notify_pending == TRUE)
  {
    /* check if indication complete is for the current preferred system */
    if(notified_dsd_radio == ext_notif_info->new_radio_pending)
    {
      DATA_MSG2(MSG_LEGACY_HIGH,
                "notified radio:%d == new pending radio:%d. Notify",
                notified_dsd_radio, ext_notif_info->new_radio_pending);
      ds_dsd_ext_notify_pref_system_common(ext_notif_info->old_radio_pending, 
                                           ext_notif_info->new_radio_pending,
                                           subs_id);
      ext_notif_info->ext_notify_pending = FALSE;
      ext_notif_info->old_radio_pending  = DS_DSD_RADIO_INVALID;
      ext_notif_info->new_radio_pending  = DS_DSD_RADIO_INVALID;
    }
    else
    {
      DATA_MSG2(MSG_LEGACY_HIGH,
                "notified radio:%d != new pending radio:%d. Ignore",
                notified_dsd_radio, ext_notif_info->new_radio_pending);
    }
  }

  rex_leave_crit_sect( &ds_dsd_critical_section );
  return;
}/* ds_dsd_ext_pref_system_ind_completed() */

/*===========================================================================
FUNCTION       DS_DSD_EXT_EVENT_NOTIFY

DESCRIPTION    This is the external function called by external clients.
               The clients have to notify the DSD module by identifying
               themselves with the the publicly advertised API.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
boolean ds_dsd_ext_event_notify
(
  dsd_radio_type           client_radio,
  ds_dsd_event_info_type  *event_info
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((client_radio == DS_DSD_RADIO_INVALID) ||
     ((client_radio > DS_DSD_RADIO_MAX)) ||
      (event_info == NULL))
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Invalid Radio , event info: 0x%x",event_info);
    return FALSE;
  }

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(event_info->subs_id))
  {
    return FALSE;
  }

  DATA_MSG2(MSG_LEGACY_HIGH,
            "ds_dsd_ext_event_notify client_radio:%d event: %d",
            client_radio, event_info->event);

  return ds_dsd_event_factory_mh_event_cback(client_radio, event_info);
}/*ds_dsd_ext_event_notify()*/

/*===========================================================================
FUNCTION       DS_DSD_CMD_PROCESS

DESCRIPTION    This is the function called by dstask to dispatch any command
               posted to DSD module.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds_dsd_cmd_process
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3gsubsmgr_subs_id_e_type  subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(cmd_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "NULL cmd ptr");
    return;
  }
  DATA_MSG1(MSG_LEGACY_HIGH, "ds_dsd_cmd_process. command ID=%d",
            cmd_ptr->hdr.cmd_id);

  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_DSD_TIMER_EXPIRED:
      ds_dsd_timer_cmd_processing(cmd_ptr);
      break;

    case DS_CMD_DSD_EVENT_NOTIFY:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        subs_id = ((ds_dsd_cmd_info_type*)
                   (cmd_ptr->cmd_payload_ptr))->event_info.subs_id;
        ds_dsd_event_factory_cmd_process(cmd_ptr);
      }
      break;

    case DS_CMD_DSD_RECOMPUTE:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        subs_id = ((ds_dsd_recompute_cmd_info_type*)
                   (cmd_ptr->cmd_payload_ptr))->subs_id;
        ds_dsd_pref_sys_recompute_cmd_processing(cmd_ptr);
      }
      break;

    case DS_CMD_DSD_EXT_NOTIFY:
      ds_dsd_ext_external_notify_cmd_processing(cmd_ptr);
      break;

    case DS_CMD_DSD_MODE_PREF_CHG:
      ds_dsd_ext_mode_pref_chg_cmd_processing(cmd_ptr);
      break;

    case DS_CMD_DSD_RESET:
      ds_dsd_ext_reset_cmd_process(cmd_ptr);
      break;

    case DS_CMD_DSD_LTE_EMERG_CALL_EVENT:
      ds_dsd_ext_lte_emerg_call_cmd_processing(cmd_ptr);
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR, "Command %d: not for DSD",
                cmd_ptr->hdr.cmd_id);
      break;
  }

  /*--------------------------------------------------------------------
    Notify clients about SS event parsing complete
  --------------------------------------------------------------------*/
  if((cmd_ptr->hdr.cmd_id == DS_CMD_DSD_EVENT_NOTIFY) ||
     (cmd_ptr->hdr.cmd_id == DS_CMD_DSD_RECOMPUTE))
  {
    /* Check if SS event processing is complete */
    if( (ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id)) &&
        (ds_dsd_ext_check_ss_event_process_complete(subs_id) == TRUE) )
    {
      DATA_MSG0(MSG_LEGACY_LOW, "SS event parsing complete.");
      if(ds_dsd_ext_ss_event_complete_cb != NULL)
      {
        ds_dsd_ext_ss_event_complete_cb(subs_id);
      }
    }
  }
  return;
}/*ds_dsd_cmd_process()*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_EVENT_REG_CLIENT

DESCRIPTION   Called by clients to register to get DSD event notification.

DEPENDENCIES  None.

RETURN VALUE  TRUE: if registration successful
                    if client previously registgered

              FALSE: if registration unsuccessful

SIDE EFFECTS  Registered client is entered on a queue
===========================================================================*/
boolean ds_dsd_ext_event_reg_client
(
  ds_dsd_ext_client_type        dsd_ext_client,
  ds_dsd_ext_event_type         dsd_ext_event,
  ds_dsd_ext_notify_func_type   dsd_ext_client_cb_func_ptr,
  void                         *data_ptr
)
{
  boolean                       ret_val = FALSE;
  ds_dsd_ext_client_reg_info    client_reg_info;
  ds_dsd_ext_event_info_type    *notified_event = NULL;
  ds_dsd_ext_event_data_block  *event_block_to_insert = NULL;
  ds_dsd_ext_event_data_block  *event_block_temp = NULL;
  ds_dsd_ext_event_data_block  *event_block_epc = NULL;
  uint8                         q_count;
  uint8                         radio;
  ds3gsubsmgr_subs_id_e_type    subs_indx;
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_dsd_ext_wlan_addr_type     wlan_addr;
#endif /* FEATURE_DATA_WLAN_MAPCON */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&client_reg_info, 0, sizeof(ds_dsd_ext_client_reg_info));
  client_reg_info.dsd_ext_client = dsd_ext_client;
  client_reg_info.dsd_ext_event  = dsd_ext_event;

  if((event_block_to_insert =
      q_linear_search(&ds_dsd_ext_event_q,
                      ds_dsd_ext_compare_function,
                      (void *)&client_reg_info)) != NULL)
  {
    DATA_MSG2(MSG_LEGACY_HIGH, "Client %d already registered for event: %d",
              dsd_ext_client, dsd_ext_event);
    /*-----------------------------------------------------------------------
     Override the data block pointer in either case. If the user wants to use
     use a re-freshed data-block pointer
    -----------------------------------------------------------------------*/
    event_block_to_insert->data_ptr = data_ptr;
    ret_val = TRUE;
  }
  else
  {
    DATA_MSG2(MSG_LEGACY_HIGH, "Registering client %d: event: %d",
              dsd_ext_client,
              dsd_ext_event);

    event_block_to_insert = (ds_dsd_ext_event_data_block *)
                              q_get(&ds_dsd_ext_event_free_q);
    if(event_block_to_insert != NULL)
    {
      event_block_to_insert->dsd_ext_event_client_id        = dsd_ext_client;
      event_block_to_insert->dsd_ext_event_id               = dsd_ext_event;
      event_block_to_insert->dsd_ext_event_notify_func_ptr  =
                                                   dsd_ext_client_cb_func_ptr;
      event_block_to_insert->data_ptr                       = data_ptr;
      q_put(&ds_dsd_ext_event_q, &(event_block_to_insert->link));
      ret_val = TRUE;
    }
    else
    {
      DATA_MSG2(MSG_LEGACY_ERROR,
                "No Free event buffer to register client: %d, event: %d",
                dsd_ext_client, dsd_ext_event);
      ret_val = FALSE;
    }
  }

  notified_event = (ds_dsd_ext_event_info_type *)
                           modem_mem_alloc(sizeof(ds_dsd_ext_event_info_type),
                           MODEM_MEM_CLIENT_DATA_CRIT);

  if(notified_event == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Could not get notified_event memory");
    ret_val = FALSE;
  }

  /*-----------------------------------------------------------------------
   Notify the client if the registration succeeds. Also, call the callback
   in same context.
  -----------------------------------------------------------------------*/
  if(ret_val == TRUE)
  {
    /* Fill event info structure */
    for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
        subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
        subs_indx++)
    {
      if(ds_dsd_proc_is_dsd_proc_active(subs_indx))
      {
        memset(notified_event, 0, sizeof(ds_dsd_ext_event_info_type));
        notified_event->subs_id = ds3gsubsmgr_subs_id_ds3g_to_ds(subs_indx);

        notified_event->pref_index
          = ds_dsd_pref_sys_get_pref_system(subs_indx)
             - DS_DSD_RADIO_MIN;

        notified_event->changed_radio_mask = 0xFF;
        for(radio=0; radio < DS_DSD_RADIO_MAX; radio++)
        {
          notified_event->dsd_radio_info[radio].changed_fields = 0x00;

          notified_event->dsd_radio_info[radio].system_mode 
            = ds_dsd_ext_translate_dsd_sys_to_ss(DS_DSD_RADIO_MIN+radio);

          notified_event->dsd_radio_info[radio].srv_status
            = ds_dsd_common_get_radio_srv_status(DS_DSD_RADIO_MIN+radio,
                                                 FALSE,
                                                 subs_indx);

          notified_event->dsd_radio_info[radio].srv_domain
            = ds_dsd_common_get_radio_srv_domain(DS_DSD_RADIO_MIN+radio,
                                                 FALSE,
                                                 subs_indx);

          notified_event->dsd_radio_info[radio].rat_mask
            = ds_dsd_common_get_radio_rat_mask(DS_DSD_RADIO_MIN+radio,
                                               FALSE,
                                               subs_indx);

          notified_event->dsd_radio_info[radio].so_mask
            = ds_dsd_common_get_radio_so_mask(DS_DSD_RADIO_MIN+radio,
                                              FALSE,
                                              subs_indx);

          notified_event->dsd_radio_info[radio].cell_srv_ind
            = ds_dsd_common_get_radio_cell_srv_ind(DS_DSD_RADIO_MIN+radio,
                                                   FALSE,
                                                   subs_indx);

          notified_event->dsd_radio_info[radio].remote_flag
            = ds_dsd_common_get_remote_flag(DS_DSD_RADIO_MIN+radio,
                                            subs_indx);

          notified_event->dsd_radio_info[radio].roaming_status
            = ds_dsd_common_get_radio_roam_status(DS_DSD_RADIO_MIN+radio,
                                                  FALSE,
                                                  subs_indx);

    #ifdef FEATURE_DATA_WLAN_MAPCON
          notified_event->dsd_radio_info[radio].is_wlan_available
            = ds_dsd_get_wlan_available(subs_indx);

          if(notified_event->dsd_radio_info[radio].is_wlan_available)
          {
            memset(&wlan_addr, 0, sizeof(ds_dsd_ext_wlan_addr_type));
            ds_dsd_wlansm_get_address(&wlan_addr, FALSE, subs_indx);

            memscpy(&(notified_event->dsd_radio_info[radio].wlan_ipv4_addr),
                    sizeof(ps_ip_addr_type),
                    &(wlan_addr.wlan_ipv4_addr),
                    sizeof(ps_ip_addr_type));

            memscpy(&(notified_event->dsd_radio_info[radio].wlan_ipv6_addr),
                    sizeof(ps_ip_addr_type),
                    &(wlan_addr.wlan_ipv6_addr),
                    sizeof(ps_ip_addr_type));

            notified_event->dsd_radio_info[radio].wlan_ipv6_prefix_len 
              = wlan_addr.wlan_ipv6_prefix_len;

            memscpy(notified_event->dsd_radio_info[radio].wlan_mac_addr,
                    PS_SYS_MAC_ADDR_LEN,
                    wlan_addr.wlan_mac_addr,
                    PS_SYS_MAC_ADDR_LEN);

            memscpy(&(notified_event->dsd_radio_info[radio].wlan_dns_addr),
                    sizeof(wlan_addr_type),
                    &(wlan_addr.wlan_dns_addr),
                    sizeof(wlan_addr_type)); 

            memscpy(&(notified_event->dsd_radio_info[radio].wlan_epdg_addr),
                    sizeof(wlan_addr_type),
                    &(wlan_addr.wlan_epdg_addr),
                    sizeof(wlan_addr_type)); 

            strlcpy(notified_event->dsd_radio_info[radio].wlan_ssid,
                    wlan_addr.wlan_ssid,
                    sizeof(notified_event->dsd_radio_info[radio].wlan_ssid));
          }
    #endif /* FEATURE_DATA_WLAN_MAPCON */
        } /* end for loop radio */

        if(dsd_ext_client_cb_func_ptr != NULL)
        {
          dsd_ext_client_cb_func_ptr(dsd_ext_event,
                                     notified_event,
                                     event_block_to_insert->data_ptr);
        }
      } /* end if dsd proc not present */
    } /* end for loop subs_id */
  }

  /*-----------------------------------------------------------------------
    For optimization, sort the EPC client's pref sys change to be last
    entity in the queue. This sorting during power up makes runtime
    execution faster.

    Make sure that the QUEUE implementation is FIFO.
  -----------------------------------------------------------------------*/
  q_count = q_cnt(&ds_dsd_ext_event_q);
  while(q_count--)
  {
    event_block_temp = (ds_dsd_ext_event_data_block*)
                         q_get(&ds_dsd_ext_event_q);
    if(event_block_temp)
    {
      if((event_block_temp->dsd_ext_event_client_id == DS_DSD_EXT_CLIENT_EPC) &&
         (event_block_temp->dsd_ext_event_id == DS_DSD_EXT_PREF_SYS_CHANGED_EV) &&
         (q_count >0))
      {
        event_block_epc = event_block_temp;
      }
      else
      {
        q_put(&ds_dsd_ext_event_q, &(event_block_temp->link));
      }
    }
  }

  if((event_block_epc != NULL) &&
     (event_block_epc->dsd_ext_event_client_id == DS_DSD_EXT_CLIENT_EPC) &&
     (event_block_epc->dsd_ext_event_id == DS_DSD_EXT_PREF_SYS_CHANGED_EV))
  {
    q_put(&ds_dsd_ext_event_q, &(event_block_epc->link));
  }

  modem_mem_free((void *)notified_event, MODEM_MEM_CLIENT_DATA_CRIT );
  return ret_val;
}/* ds_dsd_ext_event_reg_client() */


/*===========================================================================
FUNCTION      DS_DSD_EXT_EVENT_DEREG_CLIENT

DESCRIPTION   Called by clients to de-register for event notification.

DEPENDENCIES  None.

RETURN VALUE  TRUE: if de-registration successful
                    if client previously de-registgered

              FALSE: if de-registration unsuccessful

SIDE EFFECTS  De-registered client is dequeued
===========================================================================*/
boolean ds_dsd_ext_event_dereg_client
(
  ds_dsd_ext_client_type        dsd_ext_client,
  ds_dsd_ext_event_type         dsd_ext_event
)
{
  boolean ret_val = FALSE;
  ds_dsd_ext_event_data_block   *event_block_deleted = NULL;
  ds_dsd_ext_client_reg_info    client_event_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&client_event_info, 0, sizeof(ds_dsd_ext_client_reg_info));
  client_event_info.dsd_ext_client = dsd_ext_client;
  client_event_info.dsd_ext_event  = dsd_ext_event;

  event_block_deleted = q_linear_delete_new(&ds_dsd_ext_event_q,
                                            ds_dsd_ext_compare_function,
                                            (void *)&client_event_info,
                                            NULL,
                                            NULL );
  if(event_block_deleted != NULL)
  {
    event_block_deleted->dsd_ext_event_client_id = DS_DSD_EXT_CLIENT_INVALID;
    event_block_deleted->dsd_ext_event_id = DS_DSD_EXT_INVALID_EV;
    event_block_deleted->dsd_ext_event_notify_func_ptr = NULL;
    event_block_deleted->data_ptr = NULL;
    q_put(&ds_dsd_ext_event_free_q, &(event_block_deleted->link));
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }

  DATA_MSG3(MSG_LEGACY_MED,"de-reg client is success: %d, client %d, event: %d",
              ret_val, dsd_ext_client,dsd_ext_event);

  return ret_val;
}/* ds_dsd_ext_event_dereg_client()*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_REG_SS_EVENT_PROCESS_COMPLETE

DESCRIPTION   Called by clients to register for SS event processing complete 
              by DSD. Currently only one callback registration is supported.

DEPENDENCIES  None.

RETURN VALUE  TRUE: if registration successful
              FALSE: if registration unsuccessful

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_ext_reg_ss_event_process_complete
(
  ds_dsd_ext_notify_ss_event_complete_func_type  dsd_ext_client_cb_func_ptr
)
{
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(ds_dsd_ext_ss_event_complete_cb == NULL)
  {
    ds_dsd_ext_ss_event_complete_cb = dsd_ext_client_cb_func_ptr;
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }
  return ret_val;
}/* ds_dsd_ext_reg_ss_event_process_complete() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_DEREG_SS_EVENT_PROCESS_COMPLETE

DESCRIPTION   Called by clients to de-register for SS event processing 
              complete by DSD.

DEPENDENCIES  None.

RETURN VALUE  TRUE: if de-registration successful
              FALSE: if de-registration unsuccessful

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_ext_dereg_ss_event_process_complete(void)
{
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(ds_dsd_ext_ss_event_complete_cb == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Callback previously de-registered");
    ret_val = FALSE;
  }
  else
  {
    ds_dsd_ext_ss_event_complete_cb = NULL;
    ret_val = TRUE;
  }
  return ret_val;
}/* ds_dsd_ext_dereg_ss_event_process_complete() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_EPC_NOTIFY

DESCRIPTION   This function is used to notify if EPC iface is up.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_epc_notify
(
  boolean                     epc_present,
  ds3gsubsmgr_subs_id_e_type  subs_indx
)
{
  ds_dsd_timer_epc_notify(epc_present, subs_indx);
  return;
}/* ds_dsd_ext_epc_notify() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_MODE_PREF_CHG

DESCRIPTION   This is the external function called by DS3GMGR to notify the 
              mode preference change to the DSD module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_notify_mode_pref_chg
(
  cm_mode_pref_e_type         mode_pref,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_cmd_type                     *cmd_ptr = NULL;
  ds_dsd_mode_pref_cmd_info_type  *data_block_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_MED, "ds_dsd_ext_notify_mode_pref_chg. mode_pref: %d",
            mode_pref);

  if((subs_id < DS3GSUBSMGR_SUBS_ID_MIN) ||
     (subs_id >= DS3GSUBSMGR_SUBS_ID_MAX))
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Invalid Sub id:%d. return", subs_id);
    return;
  }

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_mode_pref_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }

  data_block_ptr = (ds_dsd_mode_pref_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  /* copy the mode preference to data block pointer */
  data_block_ptr->mode_pref = mode_pref;
  data_block_ptr->subs_id = subs_id;

  /*------------------------------------------------------------------
    Post the command on internal DS queue
  -------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_MODE_PREF_CHG;
  ds_dsd_ext_put_cmd(DS_DSD_INTERNAL_CMD_QUEUE, subs_id, cmd_ptr);

  return;
}/* ds_dsd_ext_notify_mode_pref_chg() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_LTE_EMERG_CALL_IN_PROG

DESCRIPTION   This is the external function called by DS3GMGR to notify lte 
              emergency call starts or finishes to the DSD module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_notify_lte_emerg_call_in_prog
(
  ds3gsubsmgr_subs_id_e_type               subs_id,
  boolean                                  in_prog
)
{
  ds_cmd_type                         *cmd_ptr = NULL;
  ds_dsd_lte_emerg_call_cmd_type      *data_block_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_MED, 
            "ds_dsd_ext_notify_lte_emerg_call_in_prog. subs_id: %d, "
            "in_prog: %d", subs_id, in_prog);

  if((subs_id < DS3GSUBSMGR_SUBS_ID_MIN) ||
     (subs_id >= DS3GSUBSMGR_SUBS_ID_MAX))
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Invalid Sub id:%d. return", subs_id);
    return;
  }

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_lte_emerg_call_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }

  data_block_ptr = (ds_dsd_lte_emerg_call_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /* copy the mode preference to data block pointer */
  data_block_ptr->in_prog = in_prog;
  data_block_ptr->subs_id = subs_id;

  /*------------------------------------------------------------------
    Post the command on internal DS queue
  -------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_LTE_EMERG_CALL_EVENT;
  ds_dsd_ext_put_cmd(DS_DSD_INTERNAL_CMD_QUEUE, subs_id, cmd_ptr);

  return;
}/* ds_dsd_ext_notify_lte_emerg_call_in_prog() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_DEVICE_MODE

DESCRIPTION   This is the external function called by DS3GDSDIF to notify the 
              device mode to the DSD module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_notify_device_mode
(
  sys_modem_device_mode_e_type  device_mode
)
{
  boolean                     create_proc_instance[DS3GSUBSMGR_SUBS_ID_COUNT];
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_MSG1(MSG_LEGACY_MED, "ds_dsd_ext_notify_device_mode. device_mode: %d",
            device_mode);

  memset(&create_proc_instance, 0, sizeof(boolean)*DS3GSUBSMGR_SUBS_ID_COUNT);
  switch(device_mode)
  {
    case SYS_MODEM_DEVICE_MODE_SINGLE_SIM:
      create_proc_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      break;

    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY:
    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE:
      create_proc_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      create_proc_instance[DS3GSUBSMGR_SUBS_ID_2] = TRUE;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY:
      create_proc_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      create_proc_instance[DS3GSUBSMGR_SUBS_ID_2] = TRUE;
      create_proc_instance[DS3GSUBSMGR_SUBS_ID_3] = TRUE;
      break;
#endif /* #if defined(FEATURE_TRIPLE_SIM) */

    default:
      break;
  }

  /* create DSD proc instance based on device mode */
  for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
      subs_indx++)
  {
    if(create_proc_instance[subs_indx] == TRUE)
    {
      if(ds_dsd_proc_is_dsd_proc_active(subs_indx) == FALSE)
      {
        if(ds_dsd_proc_create_dsd_proc_instance(subs_indx) == FALSE)
        {
          return;
        }
        DATA_MSG1(MSG_LEGACY_HIGH, "DSD proc created for subs_id:%d",
                  subs_indx);
      }
    }
  }

  return;
}/* ds_dsd_ext_notify_device_mode() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_NOTIFY_DDS_CHANGE

DESCRIPTION   This is the external function called by DS3GDSDIF to notify the 
              Default Data Subscription change to the DSD module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_ext_notify_dds_change
(
  ds3gsubsmgr_subs_id_e_type  def_data_subs
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Recompute and notify preferred system for new subs_id */
  ds_dsd_pref_sys_dds_chg(def_data_subs);

  return;
} /* ds_dsd_ext_notify_dds_change() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_RESET

DESCRIPTION   Reset DSD statemachines. No need for clients to re-register 
              with DSD again.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Resets DSD statemachine.
===========================================================================*/
void ds_dsd_ext_reset
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  ds_dsd_reset_enum_type      reset_type
)
{
  ds_cmd_type                 *cmd_ptr = NULL;
  ds_dsd_reset_cmd_info_type  *data_block_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if((subs_id < DS3GSUBSMGR_SUBS_ID_MIN) ||
     (subs_id >= DS3GSUBSMGR_SUBS_ID_MAX))
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Invalid Sub id:%d. return", subs_id);
    return;
  }

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_reset_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }

  data_block_ptr = (ds_dsd_reset_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  data_block_ptr->subs_id = subs_id;
  data_block_ptr->reset_type = reset_type;

  /*------------------------------------------------------------------
    Post the command on internal DS queue
  -------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_RESET;
  ds_dsd_ext_put_cmd(DS_DSD_INTERNAL_CMD_QUEUE, subs_id, cmd_ptr);

  return;
}/* ds_dsd_ext_reset() */

/*===========================================================================
FUNCTION        DS_DSD_EXT_NOTIFY_NV_REFRESH_IND

DESCRIPTION     Notify DSD about NV refresh indication.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
void ds_dsd_ext_notify_nv_refresh_ind
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /* Reset DSD statemachine for subs_index */
  ds_dsd_ext_reset(subs_indx, DS_DSD_RESET_NV_REFRESH);

  /* read the NV items for the subs_index */
  ds_dsd_timer_init_timer_val_per_subs_id(subs_indx);

  return;
}/* ds_dsd_ext_notify_nv_refresh_ind() */

/*===========================================================================
FUNCTION      DS_DSD_EXT_GET_PRF_NW_FRM_PREF_SYS

DESCRIPTION   This function returns the preferred network mode based on the 
                      preferred radio passed
 
DEPENDENCIES  None.

RETURN VALUE  Network mode.

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_preferred_network_type ds_dsd_ext_get_pref_nw_frm_pref_sys
(
  dsd_radio_type  preferred_radio
)
{
  ds_dsd_preferred_network_type preferred_network = DS_DSD_INVALID_NETWORK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(preferred_radio)
  {
    case DS_DSD_RADIO_HDR:
    case DS_DSD_RADIO_1X:
      preferred_network =  DS_DSD_3GPP2_NETWORK;
      break;
	
    case DS_DSD_RADIO_LTE:
    case DS_DSD_RADIO_GSM:
    case DS_DSD_RADIO_TDSCDMA:
    case DS_DSD_RADIO_WCDMA:
      preferred_network =  DS_DSD_3GPP_NETWORK;
      break;
		 
    default:
      DATA_MSG1(MSG_LEGACY_ERROR,"Invalid Radio: %d", preferred_radio);
      break;
  }
  return preferred_network;
}/* ds_dsd_ext_get_pref_nw_frm_pref_sys */

/*===========================================================================
FUNCTION      DS_DSD_RADIO_EVENT_GEN

DESCRIPTION   This function is used to generate radio diag events.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_radio_event_gen
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_preferred_radio_event_type  ds_dsd_diag_event;
  dsd_radio_type                     preferred_sys;
  dsd_radio_type                     last_preferred_sys;

  memset(&ds_dsd_diag_event, 0, sizeof(ds_dsd_preferred_radio_event_type));

  preferred_sys = ds_dsd_pref_sys_get_pref_system(subs_id);
  last_preferred_sys = ds_dsd_pref_sys_get_last_best_system(subs_id);

  ds_dsd_diag_event.subscription_id = subs_id;
  
  /* Filling preferred network mode and system info  */
  if(preferred_sys != DS_DSD_RADIO_OOS)
  {
    ds_dsd_diag_event.preferred_network  = 
		   ds_dsd_ext_get_pref_nw_frm_pref_sys(preferred_sys);
  }
  else
  {
    ds_dsd_diag_event.preferred_network  = 
		   ds_dsd_ext_get_pref_nw_frm_pref_sys(last_preferred_sys);
  }
  /* filling rat_mask and SO Mask */
  ds_dsd_diag_event.preferred_radio = preferred_sys;
  ds_dsd_diag_event.rat_mask = 
  	    ds_dsd_common_get_radio_rat_mask(preferred_sys, FALSE, subs_id);
  ds_dsd_diag_event.so_mask = 
  	    ds_dsd_common_get_radio_so_mask(preferred_sys, FALSE, subs_id);
  
  /* filling LTE SM information */
  ds_dsd_diag_event.lte_sm_info.radio_state = 
         ds_dsd_common_get_sm_state(DS_DSD_RADIO_LTE, subs_id);
  ds_dsd_diag_event.lte_sm_info.throttle_state = 
  	     ds_dsd_common_is_sm_throttled(DS_DSD_RADIO_LTE, subs_id);
  ds_dsd_diag_event.lte_sm_info.eligible_flag = 
  	     ds_dsd_pref_sys_get_radio_available_flag(DS_DSD_RADIO_LTE, subs_id);
  
  /* filling HDR SM Information  */
  ds_dsd_diag_event.hrpd_sm_info.radio_state = 
         ds_dsd_common_get_sm_state(DS_DSD_RADIO_HDR, subs_id);
  ds_dsd_diag_event.hrpd_sm_info.throttle_state = 
  	     ds_dsd_common_is_sm_throttled(DS_DSD_RADIO_HDR, subs_id);
  ds_dsd_diag_event.hrpd_sm_info.eligible_flag = 
  	     ds_dsd_pref_sys_get_radio_available_flag(DS_DSD_RADIO_HDR, subs_id);
  
  /* filling CDMA SM Information */
  ds_dsd_diag_event.cdma_sm_info.radio_state = 
         ds_dsd_common_get_sm_state(DS_DSD_RADIO_1X, subs_id);
  ds_dsd_diag_event.cdma_sm_info.throttle_state = 
  	     ds_dsd_common_is_sm_throttled(DS_DSD_RADIO_1X, subs_id);
  ds_dsd_diag_event.cdma_sm_info.eligible_flag = 
  	     ds_dsd_pref_sys_get_radio_available_flag(DS_DSD_RADIO_1X, subs_id);
  
  /* filling TDSCDMA SM Information */
  ds_dsd_diag_event.tdscdma_sm_info.radio_state = 
         ds_dsd_common_get_sm_state(DS_DSD_RADIO_TDSCDMA, subs_id);
  ds_dsd_diag_event.tdscdma_sm_info.throttle_state = 
  	     ds_dsd_common_is_sm_throttled(DS_DSD_RADIO_TDSCDMA, subs_id);
  ds_dsd_diag_event.tdscdma_sm_info.eligible_flag = 
  	     ds_dsd_pref_sys_get_radio_available_flag(DS_DSD_RADIO_TDSCDMA, subs_id);

  /* filling WCDMA SM Information  */
  ds_dsd_diag_event.wcdma_sm_info.radio_state = 
         ds_dsd_common_get_sm_state(DS_DSD_RADIO_WCDMA, subs_id);
  ds_dsd_diag_event.wcdma_sm_info.throttle_state = 
  	     ds_dsd_common_is_sm_throttled(DS_DSD_RADIO_WCDMA, subs_id);
  ds_dsd_diag_event.wcdma_sm_info.eligible_flag = 
  	     ds_dsd_pref_sys_get_radio_available_flag(DS_DSD_RADIO_WCDMA, subs_id);

  /* filling GSM SM Information */
  ds_dsd_diag_event.gsm_sm_info.radio_state = 
         ds_dsd_common_get_sm_state(DS_DSD_RADIO_GSM, subs_id);
  ds_dsd_diag_event.gsm_sm_info.throttle_state = 
  	     ds_dsd_common_is_sm_throttled(DS_DSD_RADIO_GSM, subs_id);
  ds_dsd_diag_event.gsm_sm_info.eligible_flag = 
  	     ds_dsd_pref_sys_get_radio_available_flag(DS_DSD_RADIO_GSM, subs_id);

  /* filling WLAN SM Information */
  ds_dsd_diag_event.wlan_sm_info.radio_state = 
         ds_dsd_common_get_sm_state(DS_DSD_RADIO_WLAN, subs_id);
  ds_dsd_diag_event.wlan_sm_info.throttle_state = 
  	     ds_dsd_common_is_sm_throttled(DS_DSD_RADIO_WLAN, subs_id);
  ds_dsd_diag_event.wlan_sm_info.eligible_flag = 
  	     ds_dsd_pref_sys_get_radio_available_flag(DS_DSD_RADIO_WLAN, subs_id);

 /* sending event to Diag  */
  event_report_payload (EVENT_DS_DSD_PREFERRED_RADIO_INFO,
                        sizeof(ds_dsd_preferred_radio_event_type),
                        (void*)&ds_dsd_diag_event);
  return;
}/* ds_dsd_radio_event_gen */

/*===========================================================================
FUNCTION      DS_DSD_TIMER_EVENT_GEN

DESCRIPTION   This function is used to generate timer diag events.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_event_gen
(
  ds_dsd_timer_event_type     timer_event,
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  rex_timer_cnt_type          timer_value
)
{
  ds_dsd_timer_event_info_type ds_dsd_timer_diag_event;
  
  memset(&ds_dsd_timer_diag_event, 0, sizeof(ds_dsd_timer_event_info_type));

  ds_dsd_timer_diag_event.subscription_id = subs_id;
  ds_dsd_timer_diag_event.timer_event = timer_event;
  ds_dsd_timer_diag_event.timer_id = dsd_timer_id;
  ds_dsd_timer_diag_event.timer_value = (uint32)timer_value;

  event_report_payload (EVENT_DS_DSD_TIMER,
                        sizeof(ds_dsd_timer_event_info_type),
                        (void*)&ds_dsd_timer_diag_event);
  return;  
}/* ds_dsd_timer_event_gen()*/

/*===========================================================================
FUNCTION      DS_DSD_THROTTLE_EVENT_GEN

DESCRIPTION   This function is used to generate throttle diag events.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_throttle_event_gen
(
  dsd_radio_type dsd_radio,
  boolean        throttle
)
{
  ds_dsd_throttle_event_type  ds_dsd_diag_event;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&ds_dsd_diag_event, 0, sizeof(ds_dsd_throttle_event_type));
  ds_dsd_diag_event.event_radio = (uint8) dsd_radio;
  if(throttle)
  {
    event_report_payload (EVENT_DS_DSD_RADIO_THROTTLE,
                          sizeof(ds_dsd_throttle_event_type),
                          (void*)&ds_dsd_diag_event);
  }
  else
  {
    event_report_payload (EVENT_DS_DSD_RADIO_UNTHROTTLE,
                          sizeof(ds_dsd_throttle_event_type),
                          (void*)&ds_dsd_diag_event);
  }

}/* ds_dsd_throttle_event_gen()*/

/*===========================================================================
FUNCTION       DS_DSD_INIT

DESCRIPTION    Entire DSD Module Initialization

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds_dsd_init(void)
{
  uint8     i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
   Initialize event queues for event registration and notification
  -----------------------------------------------------------------------*/
  q_init(&ds_dsd_ext_event_q);
  q_init(&ds_dsd_ext_event_free_q);
  for(i = 0; i<(DS_DSD_EXT_MAX_EV*DS_DSD_EXT_CLIENT_MAX); i++)
  {
    memset(&ds_dsd_ext_event[i],0, sizeof(ds_dsd_ext_event_data_block));
    ds_dsd_ext_event[i].dsd_ext_event_client_id = DS_DSD_EXT_CLIENT_INVALID;
    ds_dsd_ext_event[i].dsd_ext_event_id  = DS_DSD_EXT_INVALID_EV;
    ds_dsd_ext_event[i].data_ptr = NULL;
    ds_dsd_ext_event[i].dsd_ext_event_notify_func_ptr = NULL;

    (void) q_link((void *)&ds_dsd_ext_event[i], &ds_dsd_ext_event[i].link);
    q_put(&ds_dsd_ext_event_free_q, &ds_dsd_ext_event[i].link);
  }

  ds_dsd_ext_ss_event_complete_cb = NULL;

  /*-------------------------------------------------------------------------
  Initialize the critical section.
  -------------------------------------------------------------------------*/
  memset(&ds_dsd_critical_section, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect(&ds_dsd_critical_section);

  /*-----------------------------------------------------------------------
   Initialize Internal modules
  -----------------------------------------------------------------------*/
  ds_dsd_event_dp_init();
  ds_dsd_proc_init();

  /* Initialize statemachines */
  ds_dsd_1xSm_init();
  ds_dsd_gsmSm_init();
  ds_dsd_hdrSm_init();
  ds_dsd_lteSm_init();
  ds_dsd_tdscdmaSm_init();
  ds_dsd_wcdmaSm_init();
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_dsd_wlanSm_init();
#endif /*FEATURE_DATA_WLAN_MAPCON*/

  ds_dsd_ext_init();
  ds_dsd_pref_sys_init();

  return;
}/*ds_dsd_init()*/

/*===========================================================================
FUNCTION      DS_DSD_EXT_GET_TRAT

DESCRIPTION   This function returns trat value

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_ext_get_trat
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return ds3gdsdif_is_lte_attach_in_progress(
           ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id));
} /* ds_dsd_ext_get_trat() */

