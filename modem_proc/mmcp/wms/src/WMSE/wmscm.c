/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmscm.c -- Interface with CM

  The WMS module which implements the User API for SMS. This source file
  implements the interfaces with Call Manager.

  -------------

    Copyright (c) 2001-2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmscm.c#1 $
  $DateTime: 2013/01/21 08:14:35
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmscm.h"
#include "wmscfg.h"
#include "wmsdc.h"
#include "cm.h"
#include "wmsi.h"
#include "err.h"
#include "wmsmsg.h"
#include "wmscm_gw.h"
#include "wmssim.h"
#include "wmsutils.h"

#ifdef FEATURE_CDSMS
#include "wmsbc.h"
#endif /* FEATURE_CDSMS */

#include "mmgsdisessionlib.h"
#include "wmsnv.h"
#include "stringl.h"

#include "sys.h"
#include "cmcall.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
static rex_crit_sect_type            wms_cm_crit_sect;

// Define a variable to hold the WMS task TCB.  This is done for
// a several reasons -- see definition of wms_myself in wms.c
static rex_tcb_type *wms_myself = NULL;  // To be initialized to rex_self()
                                         // when task is running

/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_cm_s_ptr

DESCRIPTION
  Return the pointer to the CM data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal CM data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_cm_s_type * wms_cm_s_ptr(
  void
)
{
  static wms_cm_s_type  cm;

  return &cm;
} /* wms_cm_s_ptr() */

/*===========================================================================
FUNCTION wms_cm_lpm_notify

DESCRIPTION
  Notify WMS that LPM is entered.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_lpm_notify(
  boolean   in_lpm
)
{
  wms_cfg_s_type      *cfg_s_ptr = wms_cfg_s_ptr();
  wms_cmd_type        *cmd_ptr;

  rex_enter_crit_sect(&wms_cm_crit_sect);
  cfg_s_ptr->lpm = in_lpm;
  rex_leave_crit_sect(&wms_cm_crit_sect);

  MSG_HIGH_1("Set LPM mode in WMS = %d", cfg_s_ptr->lpm);

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_LPM_E;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.appInfo_ptr = NULL;

    cmd_ptr->cmd.oprt_mode.in_lpm = in_lpm;

    wms_put_cmd( cmd_ptr );

    MSG_HIGH_0("Sending WMS_CMD_CM_LPM_E event to WMS");
  }
  else
  {
    MSG_ERROR_0("No command buffer available for WMS_CMD_CM_LPM_E");
  }

  return;
} /* wms_cm_lpm_notify() */

#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cm_online_proc

DESCRIPTION
  Forwards CM Online event to WMS

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_online_proc(
  void
)
{
#ifdef FEATURE_CDSMS_BROADCAST
  wms_cmd_type        *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_ONLINE_E;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.appInfo_ptr = NULL;
    /* No command data */

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    MSG_ERROR_0("No command buffer available for WMS_CMD_CM_ONLINE_E");
  }
#endif /* FEATURE_CDSMS_BROADCAST */

  return;
} /* wms_cm_online_proc() */


#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION wms_cm_bc_enable_failure_proc

DESCRIPTION
  Forwards Broadcast Enable Failure event to WMS

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_bc_enable_failure_proc(
  cm_sms_cmd_err_e_type  sms_cmd_err
)
{
  wms_cmd_type        *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_BC_ENABLE_FAILURE_E;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_1;
    cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

    cmd_ptr->cmd.bc_enable_failure.sms_cmd_err = sms_cmd_err;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    MSG_ERROR_0("No command buffer available for WMS_CMD_CM_BC_ENABLE_FAILURE_E");
  }

  return;
} /* wms_cm_bc_enable_failure_proc() */

#endif /* FEATURE_CDSMS_BROADCAST */
#endif /* FEATURE_CDSMS */


/*===========================================================================
FUNCTION wms_cm_init_domain_pref

DESCRIPTION
  Reads GW domain preference from NV and sends it to MM

DEPENDENCIES
  AS_ID should be set correctly to the appInfo_ptr before calling this func.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_init_domain_pref(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
#ifdef FEATURE_GWSMS
  if (NULL != appInfo_ptr && asid_is_valid(appInfo_ptr->asid))
  {
    // Only read the item if it hasn't already been cached
    if ( !appInfo_ptr->gw_domain_pref_has_been_read )
    {
      boolean nv_read_is_successful;
      nv_read_is_successful = wms_nv_read_gw_domain_pref(appInfo_ptr->asid, &appInfo_ptr->gw_domain_pref);

      if ( !nv_read_is_successful || appInfo_ptr->gw_domain_pref >= WMS_GW_DOMAIN_PREF_MAX)
      {
        // Couldn't read the item or it is out of valid range, set it properly
        appInfo_ptr->gw_domain_pref = WMS_GW_DOMAIN_PREF_CS_ONLY;
        (void) wms_nv_write_gw_domain_pref( appInfo_ptr->asid, appInfo_ptr->gw_domain_pref );
      }

      appInfo_ptr->curr_gw_domain_pref = appInfo_ptr->gw_domain_pref;
      (void) wms_cfg_mm_set_domain_pref( appInfo_ptr->asid, appInfo_ptr->gw_domain_pref );
      appInfo_ptr->gw_domain_pref_has_been_read = TRUE;
    }
  }
  else
  {
    MSG_ERROR_1( "appInfo_ptr is NULL: 0x%08x or AS_ID is invalid", 
                 appInfo_ptr );
  }
#endif /* FEATURE_GWSMS */

  return;
} /* wms_cm_init_domain_pref() */

/*===========================================================================
FUNCTION wms_cm_handle_sys_sel_pref_event

DESCRIPTION
  Handles CM_PH_EVENT_SYS_SEL_PREF

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_handle_sys_sel_pref_event(
  cm_ph_info_s_type       *ph_info_ptr,
  wms_message_mode_e_type  message_mode
)
{
  if (NULL != ph_info_ptr && asid_is_valid(ph_info_ptr->asubs_id))
  {
    wms_cfg_s_type           *lcfg_s_ptr   = wms_cfg_s_ptr();
    wms_prov_app_info_s_type *appInfo_ptr = NULL;

    if (WMS_MESSAGE_MODE_CDMA == message_mode)
    {
      appInfo_ptr = wms_get_appinfo_1x();
    }
    else
    {
      appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( ph_info_ptr->asubs_id, message_mode );
    }

    if ( NULL != appInfo_ptr )
    {
      // CM_PH_STATE_EMERG_CB may be followed by another CM_PH_STATE_EMERG_CB if some
      // parameters change. WMS need to reset counter only after CM_PH_STATE_NORMAL
      // comes
      if (CM_PH_STATE_EMERG_CB == ph_info_ptr->ph_state && lcfg_s_ptr->sms_allowed_in_ecbm)
      {
        appInfo_ptr->ecbm_mode_is_enabled = TRUE;
      }
      else
      {
        appInfo_ptr->ecbm_mode_is_enabled = FALSE;
        appInfo_ptr->mt_msg_rec_count_in_ecbm = 0;
      }
      MSG_HIGH_4( "ECBM enabled: %d on asid: %d, message_mode %d, sms_in_ecbm: %d",
                  appInfo_ptr->ecbm_mode_is_enabled,
                  ph_info_ptr->asubs_id,
                  message_mode,
                  lcfg_s_ptr->sms_allowed_in_ecbm );
    }
    else
    {
      MSG_ERROR_2( "appInfo_ptr is NULL: asid: %d, message_mode %d",
                   ph_info_ptr->asubs_id,
                   message_mode );
    }
  }
  else
  {
    MSG_ERROR_1("ph_info_ptr is NULL: 0x%08x or AS_ID is invalid", ph_info_ptr);
  }

  return;
}  /* wms_cm_handle_sys_sel_pref_event() */

void wms_cm_init_routes(
  sys_modem_as_id_e_type  as_id
)
{
  wms_cfg_s_type      *cfg_s_ptr = wms_cfg_s_ptr();

  if (!asid_is_valid(as_id))
  {
    MSG_ERROR_1("Invalid AS_ID calling wms_cm_init_routes(): %d", as_id);
    return;
  }

  cfg_s_ptr->routes[as_id].as_id = as_id;

  if (wms_nv_read_routing(&cfg_s_ptr->routes[as_id]) == FALSE )
  {
    // Message routes init: these are the default settings
#ifdef FEATURE_GWSMS
    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_0].route        = WMS_ROUTE_TRANSFER_AND_ACK;
    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_0].mem_store    = WMS_MEMORY_STORE_NONE;

    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_1].route        = WMS_ROUTE_STORE_AND_NOTIFY;
    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_1].mem_store    = WMS_MEMORY_STORE_NV_GW;

    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_2].route        = WMS_ROUTE_STORE_AND_NOTIFY;
    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_2].mem_store    = WMS_MEMORY_STORE_SIM;

    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_3].route        = WMS_ROUTE_STORE_AND_NOTIFY;
    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_3].mem_store    = WMS_MEMORY_STORE_NV_GW;

    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_NONE].route     = WMS_ROUTE_STORE_AND_NOTIFY;
    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_NONE].mem_store = WMS_MEMORY_STORE_NV_GW;

    cfg_s_ptr->routes[as_id].transfer_status_report = FALSE;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_CDMA].route     = WMS_ROUTE_STORE_AND_NOTIFY;
    cfg_s_ptr->routes[as_id].pp_routes[WMS_MESSAGE_CLASS_CDMA].mem_store = WMS_MEMORY_STORE_NV_CDMA;
#endif /* FEATURE_CDSMS */
  }

  return;
} /* wms_cm_init_routes() */

/*===========================================================================
FUNCTION wms_cm_ph_event_cb()                                      (INTERNAL)

DESCRIPTION
  CM Phone event callback.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_ph_event_cb(
  cm_ph_event_e_type         ph_event,
  const cm_ph_info_s_type   *ph_info_ptr
)
{
   wms_cmd_type        *cmd_ptr;

   cmd_ptr = wms_get_cmd_buf();

   if (NULL != cmd_ptr)
   {
     cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_PH_EVENT_CB;
     cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
     cmd_ptr->hdr.cmd_cb      = NULL;
     cmd_ptr->hdr.user_data   = NULL;
     cmd_ptr->hdr.appInfo_ptr = NULL;

     cmd_ptr->cmd.cm_ph_cb.ph_event    = ph_event;
     (void)memscpy( &cmd_ptr->cmd.cm_ph_cb.ph_info,
                    sizeof(cm_ph_info_s_type),
                    ph_info_ptr,
                    sizeof(cm_ph_info_s_type));

     wms_put_cmd( cmd_ptr );
   }
   else
   {
     MSG_ERROR_0("Out of cmd buffer");
   }
} /* wms_cm_ph_event_cb() */

/*===========================================================================
FUNCTION wms_cm_ph_event_cb_proc()                                 (INTERNAL)

DESCRIPTION
  CM Phone event callback.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_ph_event_cb_proc(
  wms_cmd_type *cmd_ptr
)
{
  wms_cfg_s_type     *cfg_s_ptr   = wms_cfg_s_ptr();
  cm_ph_event_e_type  ph_event    =  cmd_ptr->cmd.cm_ph_cb.ph_event;
  cm_ph_info_s_type  *ph_info_ptr = &cmd_ptr->cmd.cm_ph_cb.ph_info;

  MSG_HIGH_1("Receive ph event %d", ph_event);

  switch (ph_event)
  {
  case CM_PH_EVENT_INFO_AVAIL:
  case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
  case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:

    if (CM_PH_EVENT_INFO_AVAIL == ph_event)
    {
      MSG_HIGH_0("CM_PH_EVENT_INFO_AVAIL");
    }
    else if (CM_PH_EVENT_SUBSCRIPTION_AVAILABLE == ph_event)
    {
      MSG_HIGH_0("CM_PH_EVENT_SUBSCRIPTION_AVAILABLE");
    }
    else
    {
      MSG_HIGH_0("CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE");
    }

    MSG_HIGH_1("ph_info_ptr->is_cdma_subscription_available = %d",     ph_info_ptr->is_cdma_subscription_available);
    MSG_HIGH_1("ph_info_ptr->is_gw_subscription_available = %d",       ph_info_ptr->is_gw_subscription_available);
    MSG_HIGH_1("ph_info_ptr->is_hybr_gw_subscription_available = %d",  ph_info_ptr->is_hybr_gw_subscription_available);
    MSG_HIGH_1("ph_info_ptr->is_hybr_gw3_subscription_available = %d", ph_info_ptr->is_hybr_gw3_subscription_available);
    MSG_HIGH_1("ph_info_ptr->rtre_control = %d",                       ph_info_ptr->rtre_control);
    MSG_HIGH_1("ph_info_ptr->rtre_config = %d",                        ph_info_ptr->rtre_config);
    MSG_HIGH_1("ph_info_ptr->mode_capability = %d",                    ph_info_ptr->mode_capability);

    if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
    {
      // Multi-sim
      cfg_s_ptr->wms_ph_info.active_subs  = ph_info_ptr->active_subs;
      cfg_s_ptr->wms_ph_info.standby_pref = ph_info_ptr->standby_pref;

      MSG_HIGH_2( "active_subs %d, standby_pref %d", ph_info_ptr->active_subs, ph_info_ptr->standby_pref );
    }

    //
    // Do not handle subscription (un)available here any longer.
    // CM subscription availability is provided in the SUBSCRIPTION_PREF_INFO event
    // 
    break;

  case CM_PH_EVENT_DUAL_STANDBY_PREF:
    cfg_s_ptr->wms_ph_info.active_subs        = ph_info_ptr->active_subs;
    cfg_s_ptr->wms_ph_info.priority_subs      = ph_info_ptr->priority_subs;
    cfg_s_ptr->wms_ph_info.standby_pref       = ph_info_ptr->standby_pref;
    cfg_s_ptr->wms_ph_info.default_data_subs  = ph_info_ptr->default_data_subs;
    cfg_s_ptr->wms_ph_info.default_voice_subs = ph_info_ptr->default_voice_subs;

    MSG_HIGH_2( "standby_pref = %d active_subs = %d",
                 ph_info_ptr->standby_pref,
                 ph_info_ptr->active_subs );
    break;

  case CM_PH_EVENT_OPRT_MODE:

     MSG_HIGH_2("CM_PH_EVENT_OPRT_MODE: oprt_mode %d, lpm %d", ph_info_ptr->oprt_mode, cfg_s_ptr->lpm);

     if ( SYS_OPRT_MODE_LPM == ph_info_ptr->oprt_mode && (FALSE == cfg_s_ptr->lpm) )
     {
        wms_cm_lpm_notify( TRUE );
     }
     else if ( SYS_OPRT_MODE_ONLINE == ph_info_ptr->oprt_mode && (TRUE == cfg_s_ptr->lpm) )
     {
        wms_cm_lpm_notify( FALSE );
     }
     break;

  case CM_PH_EVENT_SYS_SEL_PREF:
    {
      if (SYS_MODEM_AS_ID_1 == ph_info_ptr->asubs_id)
      {
        cfg_s_ptr->hybr_pref = ph_info_ptr->hybr_pref;
        MSG_HIGH_1("cfg_s_ptr->hybr_pref = %d", cfg_s_ptr->hybr_pref);
      }	
      MSG_HIGH_1("CM_PH_EVENT_SYS_SEL_PREF: ph_state %d", ph_info_ptr->ph_state);
      wms_cm_handle_sys_sel_pref_event( ph_info_ptr, WMS_MESSAGE_MODE_CDMA );
    }
    break;

  default:
     break;
  }

  return;
} /* wms_cm_ph_event_cb_proc() */

/*===========================================================================
FUNCTION wms_cm_ss_event_cb

DESCRIPTION
  CM SS event callback.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_ss_event_cb(
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type  *cm_ss_info_ptr
)
{
  wms_cmd_id_e_type               cmd_id = WMS_CMD_DUMMY;
  wms_cmd_ss_change_info_s_type   tmp_ss_info;
  int8                            stack_idx; 
  
  if ( NULL == cm_ss_info_ptr )
  {
    MSG_ERROR_0("NULL cm_ss_info_ptr pointer");
    return;
  }

  MSG_HIGH_1("wms_cm_ss_event_cb(): event = %d", ss_event );
    
  switch( ss_event )
  {
    // IS-95 and UMTS SS SRV change
    case CM_SS_EVENT_SRV_CHANGED:
    case CM_SS_EVENT_INFO:
       
      MSG_HIGH_2( "asubs_id = %d, number_of_stacks = %d",
                  cm_ss_info_ptr->asubs_id, 
                  cm_ss_info_ptr->number_of_stacks );

      tmp_ss_info.hs_based_plus_dial_setting  = cm_ss_info_ptr->plus_dial_setting;
      tmp_ss_info.home_mobile_country_code    = cm_ss_info_ptr->home_mcc;
      tmp_ss_info.current_mobile_country_code = cm_ss_info_ptr->current_mcc;
      tmp_ss_info.number_of_stacks            = MIN(cm_ss_info_ptr->number_of_stacks, CM_NO_STACKS);

      for ( stack_idx = 0; stack_idx < tmp_ss_info.number_of_stacks; stack_idx++ ) 
      {
        MSG_HIGH_6( "[%d] is_oper %d, srv_status %d, sys_mode %d, id_type %d, srv_domain %d",
                    stack_idx,
                    cm_ss_info_ptr->stack_info[stack_idx].is_operational,
                    cm_ss_info_ptr->stack_info[stack_idx].srv_status,
                    cm_ss_info_ptr->stack_info[stack_idx].sys_mode,
                    cm_ss_info_ptr->stack_info[stack_idx].sys_id.id_type,
                    cm_ss_info_ptr->stack_info[stack_idx].srv_domain );

        MSG_HIGH_2( "    chg_fields 0x%x, chg_fields2 0x%x",
                    cm_ss_info_ptr->stack_info[stack_idx].changed_fields,
                    cm_ss_info_ptr->stack_info[stack_idx].changed_fields2 );

        tmp_ss_info.stack_info[stack_idx].asubs_id              = cm_ss_info_ptr->asubs_id; 
        tmp_ss_info.stack_info[stack_idx].is_stack_operational  = cm_ss_info_ptr->stack_info[stack_idx].is_operational;
        tmp_ss_info.stack_info[stack_idx].sys_srv_has_changed   = FALSE;

        if ( cm_ss_info_ptr->stack_info[stack_idx].is_operational )
        {
          tmp_ss_info.stack_info[stack_idx].sys_mode    = cm_ss_info_ptr->stack_info[stack_idx].sys_mode;
          tmp_ss_info.stack_info[stack_idx].srv_domain  = cm_ss_info_ptr->stack_info[stack_idx].srv_domain;
          tmp_ss_info.stack_info[stack_idx].srv_status  = cm_ss_info_ptr->stack_info[stack_idx].srv_status;
          tmp_ss_info.stack_info[stack_idx].sys_id      = cm_ss_info_ptr->stack_info[stack_idx].sys_id;
          tmp_ss_info.stack_info[stack_idx].roam_status = cm_ss_info_ptr->stack_info[stack_idx].roam_status;

          tmp_ss_info.umts_plmn_changed = ( (SYS_SYS_ID_TYPE_UMTS == cm_ss_info_ptr->stack_info[stack_idx].sys_id.id_type) &&
                                             (cm_ss_info_ptr->stack_info[stack_idx].changed_fields & CM_SS_EVT_SYS_ID_MASK) );

          if ( (cm_ss_info_ptr->stack_info[stack_idx].changed_fields & CM_SS_EVT_SRV_DOMAIN_MASK)    ||
              (cm_ss_info_ptr->stack_info[stack_idx].changed_fields & CM_SS_EVT_SYS_MODE_MASK)       ||
              (cm_ss_info_ptr->stack_info[stack_idx].changed_fields & CM_SS_EVT_SRV_STATUS_MASK)     ||
              (cm_ss_info_ptr->stack_info[stack_idx].changed_fields & CM_SS_EVT_ROAM_STATUS_MASK)    ||
              (cm_ss_info_ptr->stack_info[stack_idx].changed_fields & CM_SS_EVT_SYS_ID_MASK) )
          {
            tmp_ss_info.stack_info[stack_idx].sys_srv_has_changed = TRUE;
            cmd_id = WMS_CMD_SS_CHANGE_INFO;
          }
        }
      }  //end for()

      break;

    // HDR and EVDO SS SRV change 
    case CM_SS_EVENT_IPAPP_REG_STATUS:
      if ( cm_ss_info_ptr->ipapp_info.call_type_mask[CM_CALL_TYPE_SMS] & CM_CALL_TYPE_SMS_MASK )
      {
        cmd_id = WMS_CMD_IPAPP_CHANGE_INFO;
      }
      break; 

    default:
      break;
  }  /* end for() */
     
  if ( WMS_CMD_DUMMY != cmd_id ) 
  {
    // send command to SMS task
    wms_cmd_type *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if ( NULL != cmd_ptr ) 
    {
      cmd_ptr->hdr.client_id      = (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb         = NULL;
      cmd_ptr->hdr.user_data      = NULL;
      cmd_ptr->hdr.as_id          = cm_ss_info_ptr->asubs_id;
      cmd_ptr->hdr.cmd_id         = cmd_id;

      if ( WMS_CMD_SS_CHANGE_INFO == cmd_id ) 
      {
        cmd_ptr->cmd.ss_change_info = tmp_ss_info;
      }
      else 
      {
        cmd_ptr->cmd.ipapp_change_info = cm_ss_info_ptr->ipapp_info;
      }

      wms_put_cmd( cmd_ptr );
    }
  }

  return;
} /* wms_cm_ss_event_cb() */

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION wms_cm_call_event_cb

DESCRIPTION
  CM Call event callback.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_call_event_cb(
  cm_call_event_e_type            call_event,
  const cm_mm_call_info_s_type   *call_info_ptr
)
{
  wms_cm_s_type         *cm_s_ptr = wms_cm_s_ptr();
  wms_dc_s_type         *dc_s_ptr = wms_dc_s_ptr();
  wms_cmd_id_e_type      cmd_id   = WMS_CMD_DUMMY;
  wms_cmd_type          *cmd_ptr;

  if ( NULL == call_info_ptr )
  {
    MSG_ERROR_0("call_info_ptr is NULL!");
    return;
  }

  MSG_HIGH_4( "call type = %d, Event = %d, call id = %d, sys mode = %d",
              call_info_ptr->call_type, 
              call_event,
              call_info_ptr->call_id,
              call_info_ptr->sys_mode );
 
  if ( TRUE == wms_cm_check_call_type(call_info_ptr->call_type,
                                      call_info_ptr->sys_mode) )
  {
    cm_s_ptr->call_info.asubs_id  = call_info_ptr->asubs_id;
    cm_s_ptr->call_info.call_type = call_info_ptr->call_type;
       
    if ( SYS_SYS_MODE_CDMA == call_info_ptr->sys_mode )
    {
      if ( (CM_CALL_EVENT_ORIG == call_event) ||
           (CM_CALL_EVENT_END  == call_event) )
      {
        dc_s_ptr->is_voice_data_call_conn = FALSE;
        cm_s_ptr->call_info.call_state    = call_info_ptr->call_state;
      }

      if ( (CM_CALL_EVENT_CONNECT == call_event) ||
           (CM_CALL_EVENT_INCOM   == call_event) )
      {
        dc_s_ptr->is_voice_data_call_conn = TRUE;
        cm_s_ptr->call_info.call_state    = call_info_ptr->call_state;

        if ( WMS_DUMMY_CALL_ID != dc_s_ptr->dc_call_id )
        {
           /* Case : One of call type is already connected and MO SMS request
                     is pending in CM as well. WMS will disconnect the MO SMS
                     request and send DISCONNECT event in CONNECT handler
          */

          cmd_id = WMS_CMD_DC_CALL_CONNECT_E;
        }
      }
    }
    else if ( CM_CALL_STATE_IDLE != cm_s_ptr->call_info.call_state )
    {
      /* Case: Orign in CDMA_MODE but connect in non-CDMA mode */
      MSG_HIGH_2( "Mode change %d, event %d, reset call state to Idle",
                  call_info_ptr->sys_mode,
                  call_event );
      cm_s_ptr->call_info.call_state = CM_CALL_STATE_IDLE;
    }
  }

  if ( CM_CALL_TYPE_SMS == call_info_ptr->call_type )
  {
    MSG_HIGH_1("Got SMS event %d", call_event);

    /* look for events that we are interested in */
    switch( call_event )
    {
      case CM_CALL_EVENT_INCOM:
        cmd_id = WMS_CMD_DC_CALL_INCOM_E;
        break;

      case CM_CALL_EVENT_CONNECT:
        cmd_id = WMS_CMD_DC_CALL_CONNECT_E;
        break;

      case CM_CALL_EVENT_END:
        cmd_id = WMS_CMD_DC_CALL_END_E;
        break;

      default:
        /* events that we are not interested in */
        break;

    } /* switch */

  } /* if */

  if ( WMS_CMD_DUMMY != cmd_id )
  {
    cmd_ptr = wms_get_cmd_buf();

    if ( NULL != cmd_ptr )
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = NULL;
      cmd_ptr->hdr.as_id       = call_info_ptr->asubs_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(call_info_ptr->asubs_id, WMS_MESSAGE_MODE_CDMA);
      cmd_ptr->cmd.call_info = *call_info_ptr;

      /* send command to SMS task
      */
      wms_put_cmd( cmd_ptr );
    }
    /* else, command buffer allocation failed. Nothing to do since
       error was reported/logged already; the command will be ignored.  */
  }
  /* else, event was ignored */

  return;
} /* wms_cm_call_event_cb() */


/*===========================================================================
FUNCTION   wms_cm_call_cmd_cb

DESCRIPTION
  This is the CM call command status callback function. It sends a command
  to the WMS task notifying the CM command error.

DEPENDENCIES
  WMS command buffer is available

RETURNS
  None

SIDE EFFECTS
  Send a command to the WMS task
===========================================================================*/
static void wms_cm_call_cmd_cb(
  void                      * user_data,
  cm_call_cmd_e_type        call_cmd,
  cm_call_cmd_err_e_type    call_cmd_err
)
{
  wms_cmd_type         * cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    MSG_HIGH_2( "CM cmd %d failed: %d",
                call_cmd,
                call_cmd_err );
    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL )
    {
      cmd_ptr->hdr.cmd_id      = WMS_CMD_DC_CALL_ERROR_E;
      cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = user_data;
      cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_1;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

      cmd_ptr->cmd.call_error.call_cmd     = call_cmd;
      cmd_ptr->cmd.call_error.call_cmd_err = call_cmd_err;

      /* send command to SMS task
      */
      wms_put_cmd( cmd_ptr );
    }
    /* else, command buffer allocation failed. Nothing to do since
       error was reported/logged already; the command will be ignored.  */
  }
  /* else, no error happened; ignore it */

  /* done */
  return;

} /* end of wms_cm_call_cmd_cb() */


/*===========================================================================
FUNCTION   wms_cm_sms_cmd_cb

DESCRIPTION
  Callback function for CM to report SMS command status

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Send a command the SMS Task.
===========================================================================*/
void wms_cm_sms_cmd_cb(
  void                       *user_data,
  cm_sms_cmd_e_type           sms_cmd,
  cm_sms_cmd_err_e_type       sms_cmd_err
)
{
  wms_status_e_type  st          = WMS_OK_S;

  (void)user_data; // parameter not used

  if( sms_cmd_err != CM_SMS_CMD_ERR_NOERR )
  {
    MSG_HIGH_2( "CM cmd %d failed: %d",
                sms_cmd,
                sms_cmd_err );
    if( sms_cmd == CM_SMS_CMD_MO_MSG )
    {
      wms_cfg_s_type  *cfg_s_ptr = wms_cfg_s_ptr();

      switch( sms_cmd_err )
      {
        case CM_SMS_CMD_ERR_CLIENT_ID_P:
          MSG_ERROR_0( "SMS cmd err: Bad client ID");
          st = WMS_GENERAL_ERROR_S;
          break;

        case CM_SMS_CMD_ERR_NOT_IDLE_S:
          MSG_ERROR_0( "SMS cmd err: Not idle");
          st = WMS_GENERAL_ERROR_S;
          break;


        case CM_SMS_CMD_ERR_OFFLINE_S:
        case CM_SMS_CMD_ERR_CDMA_LOCK_S:
        case CM_SMS_CMD_ERR_EMERGENCY_MODE_S:
          st = WMS_PHONE_NOT_READY_S;
          break;

        case CM_SMS_CMD_ERR_CALL_ORIG_S:
          st = WMS_NETWORK_NOT_READY_S;
          break;


        case CM_SMS_CMD_ERR_NO_SRV_S:
          st = WMS_NETWORK_NOT_READY_S;
          break;

        case CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S:
          st = WMS_NOT_ALLOWED_IN_AMPS_S;
          break;


        case CM_SMS_CMD_ERR_NOT_ANALOG_S:
          MSG_HIGH_0("AWI ACK was not sent successfully");
          st = WMS_GENERAL_ERROR_S;
          break;


        case CM_SMS_CMD_ERR_NO_BUF_L:
          st = WMS_OUT_OF_RESOURCES_S;
          break;

        case CM_SMS_CMD_ERR_SIP:
          st = WMS_SIP_PERM_ERROR_S;
          break;

        case CM_SMS_CMD_ERR_OTHER:
        default:
          st = WMS_GENERAL_ERROR_S;
          break;
      }

      wms_msg_mc_mo_status( WMS_ERROR_TEMP, st, cfg_s_ptr->last_used_bearer );

    }
#ifdef FEATURE_CDSMS_BROADCAST
    else if( sms_cmd == CM_SMS_CMD_BC_ENABLE )
    {
      wms_cm_bc_enable_failure_proc( sms_cmd_err );
    }
#endif /* FEATURE_CDSMS_BROADCAST */
    //else, don't care about the status of other commands
  }
  /* else, no error happened; ignore it */

  return;
} /* wms_cm_sms_cmd_cb() */

/*===========================================================================
FUNCTION   wms_map_cm_status

DESCRIPTION
  Map the command status from CM to the SMS status.

DEPENDENCIES
  None

RETURNS
  The SMS status.

SIDE EFFECTS
  None
===========================================================================*/
wms_status_e_type wms_map_cm_status
(
  cm_sms_mo_status_e_type cm_status
)
{
  wms_status_e_type st;

  switch( cm_status )
  {
    case CM_SMS_MO_OK:
      st = WMS_OK_S;
      break;

    case CM_SMS_MO_SIP_PERMANENT_FAILURE:
      st = WMS_SIP_PERM_ERROR_S;
      break;

    case CM_SMS_MO_SIP_TEMPORARY_FAILURE:
      st = WMS_SIP_TEMP_ERROR_S;
      break;

    case CM_SMS_MO_HOLD_ORIG:
    case CM_SMS_MO_ABORT:
    case CM_SMS_MO_NO_SRV:
    case CM_SMS_MO_HOLD_ORIG_RETRY_TIMEOUT:
      st = WMS_NETWORK_NOT_READY_S;
      break;

    case CM_SMS_MO_NOT_ALLOWED_IN_AMPS:
      st = WMS_NOT_ALLOWED_IN_AMPS_S;
      break;

    case CM_SMS_MO_L2_ACK_FAILURE:
      st = WMS_NETWORK_FAILURE_S;
      break;

    case CM_SMS_MO_OUT_OF_RESOURCES:
      st = WMS_OUT_OF_RESOURCES_S;
      break;

    case CM_SMS_MO_ACCESS_TOO_LARGE:
      st = WMS_ACCESS_TOO_LARGE_S;
      break;

    case CM_SMS_MO_DTC_TOO_LARGE:
      st = WMS_DTC_TOO_LARGE_S;
      break;

    case CM_SMS_MO_PREF_CHN_NOT_AVAIL:
      st = WMS_MO_PREF_CHN_NOT_AVAIL_S;
      break;

    //case CM_SMS_MO_ACCT_BLOCK:
      //st = WMS_ACCESS_BLOCK_S;
      //break;

    default:  // bad status code
      st = WMS_GENERAL_ERROR_S;
      break;
  }

  return st;

} /* wms_map_cm_status() */


/*===========================================================================
FUNCTION wms_cm_bearer_to_mode_pref

DESCRIPTION
  Map WMS bearer type to CM mode preference.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cm_mode_pref_e_type wms_cm_bearer_to_mode_pref
(
  wms_bearer_e_type   bearer
)
{
  switch( bearer )
  {
    case WMS_BEARER_CDMA_1X:
      return CM_MODE_PREF_CDMA_ONLY;

    case WMS_BEARER_CDMA_EVDO:
      return CM_MODE_PREF_HDR_ONLY;

    case WMS_BEARER_CDMA_WLAN:
      return CM_MODE_PREF_WLAN_ONLY;

    case WMS_BEARER_GW_CS:
    case WMS_BEARER_GW_PS:
      return CM_MODE_PREF_GSM_WCDMA_ONLY;

    case WMS_BEARER_GW_PS_SIP:
      return CM_MODE_PREF_WCDMA_ONLY;

    case WMS_BEARER_GW_WLAN:
      return CM_MODE_PREF_WCDMA_WLAN;

    case WMS_BEARER_LTE:
      return CM_MODE_PREF_LTE_ONLY;

    default:
      return CM_MODE_PREF_CDMA_ONLY;
  }
} /* wms_cm_bearer_to_mode_pref() */

/*===========================================================================
FUNCTION wms_cm_sys_mode_to_bearer

DESCRIPTION
  Map CM sys mode to WMS bearer type.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_bearer_e_type wms_cm_sys_mode_to_bearer
(
  sys_sys_mode_e_type    sys_mode
)
{
  switch( sys_mode )
  {
    case SYS_SYS_MODE_CDMA:
      return WMS_BEARER_CDMA_1X;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_GW:
      return WMS_BEARER_GW_CS;

    case SYS_SYS_MODE_HDR:
      return WMS_BEARER_CDMA_EVDO;

    case SYS_SYS_MODE_WLAN:
#ifdef FEATURE_CDSMS
      return WMS_BEARER_CDMA_WLAN;
#else
      return WMS_BEARER_GW_WLAN;
#endif

    default:
      return WMS_BEARER_MAX;
  }
} /* wms_cm_sys_mode_to_bearer() */


/*===========================================================================
FUNCTION   wms_cm_sms_event_cb

DESCRIPTION
  SMS event callback function, called when a sms event is happening

DEPENDENCIES
  SMS command buffer is available

RETURNS
  None

SIDE EFFECTS
  Send command to the SMS task
===========================================================================*/
void wms_cm_sms_event_cb(
  cm_sms_event_e_type         sms_event,
  const cm_sms_info_s_type   *sms_info_ptr
)
{
  if (NULL == sms_info_ptr)
  {
    MSG_ERROR_0("sms_info_ptr is NULL!");
    return;
  }

  MSG_HIGH_1("Got CM SMS event %d", sms_event);

  /* look for events that we are interested in */
  switch( sms_event )
  {
    case CM_SMS_EVENT_MO_STATUS:
      {
        wms_cfg_s_type          *cfg_s_ptr = wms_cfg_s_ptr();
        wms_error_class_e_type   e_class;
        wms_prov_app_info_s_type *appInfo_ptr = NULL;

        appInfo_ptr = wms_get_appinfo_1x();

        if ((NULL != appInfo_ptr) && appInfo_ptr->mmgsdi_is_busy)
        {
          MSG_ERROR_0("Dropping MO Status SMS event from CM, SIM is busy");
          return;
        }

        e_class = (sms_info_ptr->mo_status == CM_SMS_MO_OK) ? WMS_ERROR_NONE : WMS_ERROR_TEMP;

        wms_msg_mc_mo_status( e_class,
                              wms_map_cm_status( sms_info_ptr->mo_status ),
                              cfg_s_ptr->last_used_bearer );
        // Can't use sms_info_ptr->sys_mode on CM_SMS_EVENT_MO_STATUS event
      }
      break;


  case CM_SMS_EVENT_MT_MSG:
      {
        sys_modem_as_id_e_type     as_id = wms_get_appinfo_1x_asid();
        wms_msg_cdma_mt_msg( (wms_format_e_type) (sms_info_ptr->mt_sms_format),
                           sms_info_ptr->length,
                           sms_info_ptr->data,
                           wms_cm_sys_mode_to_bearer( sms_info_ptr->sys_mode ),
                           TRUE, /* ack_pending */
                           as_id );
      }
      
      break;


    default:
      /* events that we are not interested in */
      break;

  } /* switch */


  /* done */
  return;

} /* end of wms_cm_sms_event_cb() */

#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION wms_cm_CT_features_supported

DESCRIPTION
  Get current operator info from cm based API cm_get_operator_info and checks if is CT

DEPENDENCIES
  CM API cm_get_operator_info

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cm_CT_features_supported
(
  sys_modem_as_id_e_type as_id
)
{
  cm_operator_e_type operator;
  boolean ct_features_supported = FALSE;

  if (!asid_is_valid(as_id))
  {
    MSG_ERROR_1("Invalid AS_ID calling cm_get_operator_info(): %d", as_id);
    return FALSE;
  }

  operator = cm_get_operator_info(as_id);
  MSG_HIGH_2( "Operator on AS_ID %d: %d",
              as_id,
              operator );
  if (operator == OPERATOR_CT)
  {
    ct_features_supported = TRUE;
  }

  return ct_features_supported;
}

/*===========================================================================
FUNCTION wms_cm_init

DESCRIPTION
  Do CM related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_init(
  void
)
{
  wms_cm_s_type           *cm_s_ptr = wms_cm_s_ptr();
  cm_client_status_e_type  status;

  wms_myself = rex_self(); // Will never change after this

  rex_init_crit_sect(&wms_cm_crit_sect);

  status = cm_client_init( CM_CLIENT_TYPE_SMS,
                           &cm_s_ptr->cm_client );

  if (CM_CLIENT_OK != status)
  {
    MSG_ERROR_1("Failure in cm_client_init: %d", status);
  }

  status = cm_client_ph_reg( cm_s_ptr->cm_client,
                             wms_cm_ph_event_cb,
                             CM_CLIENT_EVENT_REG,
                             CM_PH_EVENT_OPRT_MODE,
                             (cm_ph_event_e_type) ((uint32)CM_PH_EVENT_MAX - (uint32)1),
                             NULL );    // not interested in other clients' cmd errors
  if (CM_CLIENT_OK != status)
  {
    MSG_ERROR_1("Failure in cm_client_ph_reg: %d", status);
  }

  status = cm_mm_client_ss_reg_msim( cm_s_ptr->cm_client,
                                     wms_cm_ss_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_SS_EVENT_SRV_CHANGED,
                                     (cm_ss_event_e_type)((int)CM_SS_EVENT_MAX - 1),
                                     NULL, 
                                     SYS_MODEM_AS_ID_MASK_ANY );   
  if (CM_CLIENT_OK != status)
  {
    MSG_ERROR_1("Failure in cm_client_ss_reg: %d", status);
  }

#ifdef FEATURE_GWSMS
  if (WMS_CMD_ERR_NONE != wms_cm_gw_init()) 
  {
     MSG_ERROR_0("Failure in wms_cm_gw_init");
  }
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
  status = cm_mm_client_call_reg( cm_s_ptr->cm_client,
                                  wms_cm_call_event_cb,
                                  CM_CLIENT_EVENT_REG,
                                  CM_CALL_EVENT_ORIG,
                                  (cm_call_event_e_type)((uint32)CM_CALL_EVENT_MAX-(uint32)1),
                                  NULL );   // not interested in other clients' cmd errors
  if (CM_CLIENT_OK != status)
  {
    MSG_ERROR_1("Failure in cm_client_call_reg: %d", status);
  }

  status = cm_client_sms_reg( cm_s_ptr->cm_client,
                              wms_cm_sms_event_cb,
                              CM_CLIENT_EVENT_REG,
                              CM_SMS_EVENT_MO_MSG,
                              (cm_sms_event_e_type)((uint32)CM_SMS_EVENT_MAX-(uint32)1),
                              NULL );   // not interested in other clients' cmd errors
  if (CM_CLIENT_OK != status)
  {
    MSG_ERROR_1("Failure in cm_client_sms_reg: %d", status);
  }

  /* Initialize called_number, to be used for originating DTC. */
  cm_num_init(&cm_s_ptr->called_number);
#endif /* FEATURE_CDSMS */

  // Register for subscription notification (CM_PH_EVENT_SUBSCRIPTION_PREF_INFO) events.
  // This used to be only available for multi-sim configurations, but it is
  // now available for all device modes.
  status = cm_client_subs_reg( cm_s_ptr->cm_client,
                               wms_cm_subs_event_cb );

  if (CM_CLIENT_OK != status)
  {
    MSG_ERROR_1("Failure in cm_client_subs_reg: %d", status);
  }

  status = cm_client_act( cm_s_ptr->cm_client );

  if (CM_CLIENT_OK != status)
  {
    MSG_ERROR_1("Failure in cm_client_act: %d", status);
  }

  return;
} /* wms_cm_init() */

/*===========================================================================
FUNCTION wms_cm_process_event

DESCRIPTION
  Process events from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_process_event
(
  wms_cmd_type    *cmd_ptr
)
{
  switch (cmd_ptr->hdr.cmd_id)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    case WMS_CMD_CM_ONLINE_E:
      wms_bc_online_proc(cmd_ptr->hdr.appInfo_ptr);
      break;

    case WMS_CMD_CM_BC_ENABLE_FAILURE_E:
      wms_bc_enable_failure_proc( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS_BROADCAST */

    case WMS_CMD_SS_CHANGE_INFO:
      wms_cfg_ss_change_info( cmd_ptr );
      break;

    case WMS_CMD_IPAPP_CHANGE_INFO:
      wms_cfg_ipapp_change_info( &cmd_ptr->cmd.ipapp_change_info );
      break;

    case WMS_CMD_CM_LPM_E:
      wms_cfg_lpm_proc( cmd_ptr );
      break;

    default:
      MSG_HIGH_1("Unhandled cmd: %d", cmd_ptr->hdr.cmd_id);
      break;
  }

  return;
} /* wms_cm_process_event() */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cm_call_orig

DESCRIPTION
  WMS CM Call Origrination Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_call_orig(
  wms_dc_so_e_type   so
)
{
  wms_cm_s_type              *cm_s_ptr = wms_cm_s_ptr();
  wms_dc_s_type              *dc_s_ptr = wms_dc_s_ptr();
  cm_srv_type_e_type          srv_type;
  cm_cdma_orig_params_s_type  op;
  cm_orig_alpha_s_type        alpha;

  MSG_HIGH_1("Originating SMS call: so=%d", so);

  if( so == WMS_DC_SO_AUTO )
  {
    srv_type = CM_SRV_TYPE_CDMA_AUTOMATIC;
  }
  else
  {
    srv_type = CM_SRV_TYPE_CDMA_SPECIFIC;
  }

  op.srv_opt        = (uint16)so;
  op.activate_code  = CM_OTASP_ACT_CODE_NONE;
  op.drs_bit        = FALSE;
  op.sr_id_included = FALSE;
  op.qos_parms_incl = FALSE;

  alpha.len = 3;
  (void)memscpy( alpha.buf, 3, "SMS", 3 );

  {
    wms_cfg_s_type            *cfg_s_ptr = wms_cfg_s_ptr();
    wms_prov_app_info_s_type  *appInfo_ptr;
    sys_modem_as_id_e_type     asid;

    appInfo_ptr = wms_get_appinfo_1x();

    if (NULL == appInfo_ptr)
    {
      MSG_HIGH_0("Warning: 1x appInfo is NULL");
      asid = SYS_MODEM_AS_ID_1;
    }
    else
    {
      asid = appInfo_ptr->asid;
    }

    if (WMS_DEVICE_MODE_SS == cfg_s_ptr->device_mode)
    {
      (void)cm_mm_call_cmd_orig( wms_cm_call_cmd_cb,
                                 NULL,
                                 cm_s_ptr->cm_client,
                                 CM_CALL_TYPE_SMS,
                                 srv_type,
                                 NULL,                      /* calling number */
                                 &cm_s_ptr->called_number, /* called number */
                                 &alpha,
                                 &op,
                                 NULL, /* gw cs param */
                                 NULL, /* gw ps param */
                                 &dc_s_ptr->dc_call_id );
    }
    else
    {
      (void)cm_mm_call_cmd_orig_per_subs( wms_cm_call_cmd_cb,
                                          NULL,
                                          cm_s_ptr->cm_client,
                                          CM_CALL_TYPE_SMS,
                                          srv_type,
                                          asid,
                                          NULL,                      /* calling number */
                                          &cm_s_ptr->called_number, /* called number */
                                          &alpha,
                                          &op,
                                          NULL, /* gw cs param */
                                          NULL, /* gw ps param */
                                          &dc_s_ptr->dc_call_id );
    }
  }

  MSG_HIGH_1("SMS call Id =%d", dc_s_ptr->dc_call_id);

  return;
} /* wms_cm_call_orig() */

/*===========================================================================
FUNCTION wms_cm_call_end

DESCRIPTION
  WMS CM Call End Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_call_end(
  void
)
{
  wms_cm_s_type         *cm_s_ptr = wms_cm_s_ptr();
  wms_dc_s_type         *dc_s_ptr = wms_dc_s_ptr();
  cm_end_params_s_type   ep;

  MSG_HIGH_1("Ending SMS call with ID = %d", dc_s_ptr->dc_call_id);

  if (WMS_DUMMY_CALL_ID != dc_s_ptr->dc_call_id)
  {
    ep.call_id = dc_s_ptr->dc_call_id;
  }
  else
  {
    ep.call_id = cm_s_ptr->call_info.call_id;
  }

  ep.info_type = CM_CALL_MODE_INFO_CDMA;
  ep.end_params.cdma_end.end_reason_included = FALSE;

  (void)cm_mm_call_cmd_end( wms_cm_call_cmd_cb,
                            NULL,
                            cm_s_ptr->cm_client,
                            1, /* one call is being released */
                            & ep );
  return;
} /* wms_cm_call_end() */

/*===========================================================================
FUNCTION wms_cm_call_answer

DESCRIPTION
  WMS CM Call Answer Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_call_answer(
  void
)
{
  wms_cm_s_type         *cm_s_ptr = wms_cm_s_ptr();
  cm_ans_params_s_type  *ap_ptr = (cm_ans_params_s_type *) wms_mem_malloc( sizeof(cm_ans_params_s_type) );;

  MSG_HIGH_0("Answering SMS call...");

  if (NULL != ap_ptr)
  {
    ap_ptr->info_type                     = CM_CALL_MODE_INFO_CDMA;
    ap_ptr->ans_params.cdma_ans.call_type = CM_CALL_TYPE_SMS;

    (void)cm_mm_call_cmd_answer( wms_cm_call_cmd_cb,
                                 NULL,
                                 cm_s_ptr->cm_client,
                                 cm_s_ptr->call_info.call_id,
                                 ap_ptr );
    wms_mem_free( ap_ptr );
  }
  return;
} /* wms_cm_call_answer() */

/*===========================================================================
FUNCTION wms_cm_sms_send_cdma_msg2

DESCRIPTION
  CM Send MO SMS Message Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_sms_send_cdma_msg2
(
  const wms_address_s_type    * address_ptr,
  const wms_OTA_message_type  * OTA_ptr,
  wms_bearer_e_type           bearer,
  wms_client_ch_pref_e_type   ch_pref
)
{
  wms_cm_s_type        *cm_s_ptr = wms_cm_s_ptr();
  cm_sms_msg_type       cm_sms_msg;
  cm_mode_pref_e_type   cm_mode_pref;

  cm_sms_msg.ota.format   = (cm_format_e_type)OTA_ptr->format;
  cm_sms_msg.ota.data_len = OTA_ptr->data_len;

  (void)memscpy( (void *)cm_sms_msg.ota.data,
                 MIN(OTA_ptr->data_len,CM_SMS_MAX_LEN),
                 (void *)OTA_ptr->data,
                 MIN(OTA_ptr->data_len,CM_SMS_MAX_LEN) );

  cm_sms_msg.address.digit_mode = (cm_digit_mode_e_type)address_ptr->digit_mode;
  cm_sms_msg.address.number_mode = (cm_num_mode_e_type)address_ptr->number_mode;
  cm_sms_msg.address.number_type = (cm_num_type_e_type)address_ptr->number_type;
  cm_sms_msg.address.number_plan = (cm_num_plan_e_type)address_ptr->number_plan;
  cm_sms_msg.address.number_of_digits = MIN(address_ptr->number_of_digits, CM_SMS_ADDRESS_MAX);

  (void)memscpy( cm_sms_msg.address.digits,
                 cm_sms_msg.address.number_of_digits,
                 address_ptr->digits,
                 cm_sms_msg.address.number_of_digits );

  cm_mode_pref = wms_cm_bearer_to_mode_pref( bearer );

  switch( ch_pref )
  {
    case WMS_CLIENT_CH_PREF_ACH_ONLY:
      cm_sms_msg.chn_pref = CM_SMS_CHN_ACH_ONLY;
      break;

    case WMS_CLIENT_CH_PREF_TCH_ONLY:
      cm_sms_msg.chn_pref = CM_SMS_CHN_TCH_ONLY;
      break;

    case WMS_CLIENT_CH_PREF_ACH_OR_TCH:
    default:
      // default case(WMS_CLIENT_CH_NONE) should never happen since channel pref has already been validated.
      cm_sms_msg.chn_pref = CM_SMS_CHN_ACH_OR_TCH;
      break;
  }

  (void)cm_sms_cmd_mo_msg2( wms_cm_sms_cmd_cb,
                            NULL,   // user data
                            cm_s_ptr->cm_client,
                            TRUE,   // request L2 ack
                            & cm_sms_msg,
                            cm_mode_pref
                          );

  return;
} /* wms_cm_sms_send_cdma_msg2() */


/*===========================================================================
FUNCTION wms_cm_awi_ack

DESCRIPTION
  CM AWI Acknowledgement Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_awi_ack(
  uint8                        seq_no,
  boolean                      bf_bit,
  wms_error_class_e_type       error_class,
  wms_cdma_tl_status_e_type    tl_status
)
{
  wms_cm_s_type        *cm_s_ptr = wms_cm_s_ptr();

  (void)cm_sms_cmd_awi_ack( NULL, // no cmd cb
                      NULL, // no user data
                      cm_s_ptr->cm_client,
                      seq_no,
                      bf_bit,
                      (uint8)error_class,
                      (uint8)tl_status );
  return;
} /* wms_cm_awi_ack() */

/*===========================================================================
FUNCTION wms_cm_check_call_type

DESCRIPTION
  Check if WMS is interested in following call type or not

DEPENDENCIES
  none

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cm_check_call_type
(
  cm_call_type_e_type            call_type,
  sys_sys_mode_e_type            sys_mode
)
{
  boolean ret_value = FALSE;

  if ( (CM_CALL_TYPE_VOICE     == call_type) ||
       (CM_CALL_TYPE_CS_DATA   == call_type) ||
       (CM_CALL_TYPE_EMERGENCY == call_type) ||
       ( (CM_CALL_TYPE_PS_DATA == call_type) &&
         ( (SYS_SYS_MODE_CDMA == sys_mode) || (SYS_SYS_MODE_HDR == sys_mode) ) ) )
  {
     ret_value = TRUE;
  }

  MSG_HIGH_1("wms_cm_check_call_type() returns %d", ret_value);
  return ret_value;
}


#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION wms_cm_check_active_volte_call

DESCRIPTION
  Check if there is an active VoLTE call

DEPENDENCIES
  none

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cm_check_active_volte_call
(
  sys_modem_as_id_e_type         as_id,
  sys_sys_mode_e_type            sys_mode
)
{
   cm_call_id_type  call_id;
   boolean          result   = FALSE;

   call_id = cmcall_is_there_volte_call(CM_CALL_TYPE_VOICE);

   if (CM_CALL_ID_INVALID != call_id) 
   {
      cmcall_s_type *call_ptr = NULL;
      call_ptr = cmcall_ptr(call_id);

      if (NULL != call_ptr) 
      {
        if ( ( sys_mode           == call_ptr->sys_mode ) &&
             ( call_ptr->asubs_id == as_id )  )
        {
           result = TRUE;
        }
        else
        {
           MSG_HIGH_3("call id = %d, sys mode = %d, as id = %d", call_id, call_ptr->sys_mode, call_ptr->asubs_id);
        }
      }
      else
      {
         MSG_HIGH_0("NULL call pointer");
      }
   }
   
   MSG_HIGH_1("wms_cm_check_active_volte_call() returns %d", result);
   return result;

} /* wms_cm_check_active_volte_call() */

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION wms_cm_bc_enable

DESCRIPTION
  CM Enable Broadcast Functin

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_bc_enable(
  boolean  bc_enable
)
{
  wms_cm_s_type        *cm_s_ptr = wms_cm_s_ptr();

  (void) cm_sms_cmd_bc_enable( wms_cm_sms_cmd_cb,
                               NULL,   // user data
                               cm_s_ptr->cm_client,
                               bc_enable );
  return;
} /* wms_cm_bc_enable() */

#endif /* FEATURE_CDSMS_BROADCAST */

void wms_cm_subs_event_cb(
  cm_ph_event_e_type         subs_event,
  const cm_subs_info_s_type *subs_info_ptr
)
{
  wms_cmd_type        *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_SUBS_EVENT_CB;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.appInfo_ptr = NULL;

    cmd_ptr->cmd.cm_subs_cb.subs_event = subs_event;
    (void)memscpy( &cmd_ptr->cmd.cm_subs_cb.subs_info,
                   sizeof(cm_subs_info_s_type),
                   subs_info_ptr,
                   sizeof(cm_subs_info_s_type));

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    MSG_ERROR_0("Out of cmd buffer");
  }
}

void wms_cm_subs_event_cb_proc(
  wms_cmd_type *cmd_ptr
)
{
  cm_ph_event_e_type      subs_event    =  cmd_ptr->cmd.cm_subs_cb.subs_event;
  cm_subs_info_s_type    *subs_info_ptr =  &cmd_ptr->cmd.cm_subs_cb.subs_info;
  sys_modem_as_id_e_type  pref_info_asid;

  if (CM_PH_EVENT_SUBSCRIPTION_PREF_INFO != subs_event)
  {
    MSG_HIGH_1("wms_cm_subs_event_cb_proc: event %d not interesting", subs_event);
    return;
  }

  MSG_HIGH_1( "wms_cm_subs_event_cb_proc: subs_event = %d", subs_event);

  pref_info_asid = subs_info_ptr->subscription_id;

  if (!asid_is_valid(pref_info_asid))
  {
    MSG_ERROR_1( "wms_cm_subs_event_cb_proc: invalid asid %d", pref_info_asid);
    return;
  }

  // SUBSCRIPTION_PREF_INFO:
  // This event contains information that maps mmgsdi session types to the
  // appropriate modem stack.  This event is necessary to support multi-sim
  // and multi-stack configurations.  In prior implementations, WMS was able
  // to predict the stack mappings based on the architecture type.  Now these
  // configurations have become more difficult to map. This event is sent for
  // ALL device mode configurations (even single sim).  This event is preferred
  // over the cm_ph_event's 'SUBSCRIPTION_AVAILABLE' event.
  // 
  // Until this event is received, the appInfo structures for the subscriptions
  // are not usable for much more than MMGSDI operations. This is because the
  // asid (see sys_modem_as_id_e_type) is not yet assigned/mapped to them. Once
  // this event has been processed, the asids are known.
  //
  // This event should always be received regardless of OTA service conditions.
  //
  // It is possible that the mapped subscription is located on a stack that is
  // currently not operational. Use the 'is_operational' field to determine this.
  //
  MSG_HIGH_6( "SUBS_PREF_INFO(asid %d): session (1X:%d, avail:%d) (GW:%d, avail:%d), operational: %d",
              pref_info_asid,
              subs_info_ptr->session_type_1x,
              subs_info_ptr->is_available_1x,
              subs_info_ptr->session_type_gw,
              subs_info_ptr->is_available_gw,
              subs_info_ptr->is_operational );

  {
    wms_cfg_s_type                 *cfg_s_ptr   = wms_cfg_s_ptr();
    wms_prov_app_info_s_type       *appInfo_ptr;
    mmgsdi_session_type_enum_type   cur_session_type;
    boolean                         cur_is_available;

    MSG_HIGH_6( "WMS_PREF_INFO (asid %d): session (1X:%d, avail:%d) (GW:%d, avail:%d), operational: %d",
                pref_info_asid,
                cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].session_type_1x,
                cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].is_available_1x,
                cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].session_type_gw,
                cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].is_available_gw,
                cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].is_operational );

    // Handle CDMA subs information.
    // CDMA/1X is only available on AS_ID_1.
    if (SYS_MODEM_AS_ID_1 == pref_info_asid)
    {

      // For CDMA, there are two configuration modes: NV and card (RUIM, CSIM).
      // When in NV mode, there is no mmgsdi session used. 
      // The session_type_1x field will contain the following values:
      //    MMGSDI_1X_PROV_PRI_SESSION:   mmgsdi session in use, card mode
      //    MMGSDI_MAX_SESSION_TYPE_ENUM: no mmgsdi session in use, NV mode or CDMA not used
      //
      // The field 'is_available_1x' is used to determine whether the subs is available.
      //

      cur_session_type  = cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].session_type_1x;
      cur_is_available  = cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].is_available_1x;

      cfg_s_ptr->hybr_pref = subs_info_ptr->pref_info.hybr_pref;
      MSG_HIGH_1("cfg_s_ptr->hybr_pref = %d", cfg_s_ptr->hybr_pref);

      if (cur_is_available != subs_info_ptr->is_available_1x)
      {
        appInfo_ptr = wms_get_appinfo_by_session_type(MMGSDI_1X_PROV_PRI_SESSION);

        if (NULL == appInfo_ptr && wms_nv_rtre_control_is_nv())
        {
          wms_cfg_s_type  *lcfg_s_ptr = wms_cfg_s_ptr();
          lcfg_s_ptr->wms_app0_info = wms_sim_alloc_appInfo_ptr(&lcfg_s_ptr->wms_mmgsdi_app0_info);
          appInfo_ptr = lcfg_s_ptr->wms_app0_info;
        }

        if (NULL != appInfo_ptr)
        {
          if (subs_info_ptr->is_available_1x)
          {
            appInfo_ptr->asid               = pref_info_asid;  // Map the as_id in appInfo_ptr.
            appInfo_ptr->is_available       = TRUE;

            if (MMGSDI_MAX_SESSION_TYPE_ENUM == subs_info_ptr->session_type_1x)
            {
              // NV MODE: Transition to available
              MSG_HIGH_1("1x:available(NV): sim_init_complete %d->TRUE", appInfo_ptr->sim_init_complete);
              appInfo_ptr->sim_init_complete = TRUE;

              /* In NV mode, 3GPP2 SIM-related commands cannot be performed. */
              wms_cfg_report_sim_init_complete_event(appInfo_ptr, FALSE);
            }
            else
            {
              // CARD MODE: Transition to available
              MSG_HIGH_1("1X:available(card): sim_init_complete %d", appInfo_ptr->sim_init_complete);
            }
            wms_sim_do_subs_ready(appInfo_ptr);
          }
          else
          {
            appInfo_ptr->is_available = FALSE;

            // Handle card mode or NV mode
            if (MMGSDI_MAX_SESSION_TYPE_ENUM == subs_info_ptr->session_type_1x)
            {
              // NV MODE: Transition to unavailable
              // If in NV mode, it is possible that no mmgsdi event arrived for
              // the 1X subscription (no card, no CSIM/RUIM app), so set
              // sim_init_complete to FALSE.  CM is driving the availablility.
              if (MMGSDI_MAX_SESSION_TYPE_ENUM == cur_session_type &&
                  TRUE == appInfo_ptr->sim_init_complete)
              {
                /* Earlier session type was MAX(NV) i.e device was in NV mode, reset the sim_init_complete flag.
                   No need to send SIM events in this case. */
                MSG_HIGH_1("1X:unavailable(NV): sim_init_complete %d->FALSE", appInfo_ptr->sim_init_complete);
                appInfo_ptr->sim_init_complete = FALSE;
              }
              else
              {
                MSG_HIGH_1("1X:unavailable(NV): sim_init_complete %d", appInfo_ptr->sim_init_complete);
              }
            }
            else
            {
              // CARD MODE: Transition to unavailable
              MSG_HIGH_1("1X:unavailable(card): sim_init_complete %d", appInfo_ptr->sim_init_complete);
            }
            
            wms_cfg_report_not_ready_event(appInfo_ptr);
          }
        }
      }
    } // Handle CDMA

    // Handle GW subs information.
    // GW is possible on all AS (AS_ID_[1,2,3]).
    {
      // GW always requires an MMGSDI session to operate.
      // The 'session_type_gw' field will contain one of the following values:
      //    MMGSDI_GW_PROV_PRI_SESSION:   subs is available
      //    MMGSDI_GW_PROV_SEC_SESSION:   subs is available
      //    MMGSDI_GW_PROV_TER_SESSION:   subs is available
      //    MMGSDI_MAX_SESSION_TYPE:      subs is not available (unmapped)
      // 
      // The 'is_available_gw' field indicates the availability of the
      // stack mapped to 'session_type_gw'.
      //
      // It is impossible to directly transition from one GW session to another.
      // For example, GW_PRI -> GW_SEC would never occur.
      // 
      // It should also be impossible for 'session_type_gw' to transition to
      // MMGSDI_MAX_SESSION_TYPE without 'is_available_gw' also transitioning to
      // FALSE at the same time. If this state occurs, it is assumed that
      // is_available_gw is FALSE.
      //
      // It is possible to receive 'session_type_gw' != MMGSDI_MAX_SESSION_TYPE and
      // 'is_available_gw' == FALSE. This is an indication that the MMGSDI session
      // is active and that the session is mapped to an AS, but the stack handling
      // that subscription is not available. The stack could be disabled by ATCoP
      // for example or in power save.  If the stack has transitioned to not available,
      // WMS should indicate NOT_READY.
      //

      cur_session_type  = cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].session_type_gw;
      cur_is_available  = cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].is_available_gw;

      // If either session_type or available has changed, there is a transition
      if ( (cur_session_type != subs_info_ptr->session_type_gw) ||  // tagged 'S'
           (cur_is_available != subs_info_ptr->is_available_gw) )   // tagged 'A'
      {
        // Invalid A: GW_X and !avail
        // Invalid B: MAX  and avail
        // TAG OLDS NEWS OLDA NEWA RES
        //     GW   GW   0    0    Session was     mapped, stack was not available; NO CHANGE
        //  A  GW   GW   0    1    Session was     mapped, stack now     available; WMS_READY
        //  A  GW   GW   1    0    Session was     mapped, stack now not available; WMS_NOT_READY
        //     GW   GW   1    1    Session was     mapped, stack was     available; NO CHANGE
        //     MAX  MAX  0    0    Session was not mapped, stack was not available; NO CHANGE
        //  A  MAX  MAX  0    1    Session was not mapped, stack was not available; NO CHANGE            (INVALID B)
        //  A  MAX  MAX  1    0    Session was not mapped, stack was not available; NO CHANGE            (INVALID B)
        //     MAX  MAX  1    1    Session was not mapped, stack was not available; NO CHANGE            (INVALID B)
        // S   GW   MAX  0    0    Session now not mapped, stack was not available; UNMAP
        // SA  GW   MAX  0    1    Session now not mapped, stack was not available; UNMAP                (INVALID B)
        // SA  GW   MAX  1    0    Session now not mapped, stack now not available; UNMAP, WMS_NOT_READY
        // S   GW   MAX  1    1    Session now not mapped, stack was     available; UNMAP, WMS_NOT_READY (INVALID B)
        // S   MAX  GW   0    0    Session now     mapped, stack     not available; MAP
        // SA  MAX  GW   0    1    Session now     mapped, stack now     available; MAP, WMS_READY
        // SA  MAX  GW   1    0    Session now     mapped, stack     not available; MAP                  (INVALID B)
        // S   MAX  GW   1    1    Session now     mapped, stack now     available; MAP, WMS_READY       (INVALID B)

        // The current session_type and availability doesn't match the new,
        // this indicates a transition is happening

        if (MMGSDI_MAX_SESSION_TYPE_ENUM != subs_info_ptr->session_type_gw)
        {
          // GW-->GW or MAX-->GW (available TRUE or FALSE)
          // GW transition toward available, but maybe not actually available
          appInfo_ptr = wms_get_appinfo_by_session_type(subs_info_ptr->session_type_gw);

          if (NULL != appInfo_ptr)
          {
            MSG_HIGH_4( "GW -> mapped: session_type %d, sim_init_complete %d, available %d->%d",
                        appInfo_ptr->mmgsdi_appInfo.session_type,
                        appInfo_ptr->sim_init_complete,
                        cur_is_available,
                        subs_info_ptr->is_available_gw );

            // Handle mapping. A valid session_type is present, session is mapped to an AS.
            // Doesn't matter if mapping was already done, just map it
            appInfo_ptr->asid         = pref_info_asid;
            
            // Initialize GW domain preference from NV
            wms_cm_init_domain_pref(appInfo_ptr);

            // Handle available change
            if (cur_is_available != subs_info_ptr->is_available_gw)
            {
              // Available has changed, use new availability
              appInfo_ptr->is_available = subs_info_ptr->is_available_gw;

              // If the previous state was ready, send not ready
              if (cur_is_available)
              {
                // GW mapped, available -> GW mapped, not available
                wms_cfg_report_not_ready_event(appInfo_ptr);
              }
              else
              {
                // GW mapped, not available -> GW mapped, available
                wms_sim_do_subs_ready(appInfo_ptr);
              }
            } // Availability changed
          }
          else
          {
            MSG_ERROR_1("GW -> available: could not get appInfo for session_type %d", subs_info_ptr->session_type_gw);
          }
        }
        else if (MMGSDI_MAX_SESSION_TYPE_ENUM != cur_session_type)
        {
          // Incoming session_type is MMGSDI_MAX_SESSION_TYPE and current is not.
          // GW-->MAX; New GW transition to UNMAPPED
          // All transitions of GW->MAX are handled here
          // The stack is unavailable AND there is no session mapped
          // NOTE: sim_init_complete is not reset here.  This will be handled
          //       when the associated session_changed event is processed.

          appInfo_ptr = wms_get_appinfo_by_session_type(cur_session_type);

          if (NULL != appInfo_ptr)
          {
            MSG_HIGH_2( "GW -> unavailable: session_type %d, sim_init_complete %d",
                        cur_session_type,
                        appInfo_ptr->sim_init_complete );

            // If the previous state was ready, send not ready
            if (cur_is_available)
            {
              wms_cfg_report_not_ready_event(appInfo_ptr);
            }

            // GW mapped, available
            //   or
            // GW mapped, not available   ---> GW unmapped, not available
            // Stack can't be available if the session is not mapped
            appInfo_ptr->is_available = FALSE;

          }
          else
          {
            MSG_ERROR_1("GW -> unavailable: could not get appInfo for session_type %d", cur_session_type);
          }
        }
      }
    } // Handle GW

    // Update subs operational status (after handling the sessions in case previous state is needed).
    cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].session_type_1x = subs_info_ptr->session_type_1x;
    cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].is_available_1x = subs_info_ptr->is_available_1x;
    cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].session_type_gw = subs_info_ptr->session_type_gw;
    cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].is_available_gw = subs_info_ptr->is_available_gw;
    cfg_s_ptr->wms_cm_subs_info.sub[pref_info_asid].is_operational  = subs_info_ptr->is_operational;

    if (subs_info_ptr->is_default_voice)
    {
      cfg_s_ptr->wms_ph_info.default_voice_subs = pref_info_asid;
    }

    if(subs_info_ptr->is_default_data)
    {
      cfg_s_ptr->wms_ph_info.default_data_subs = pref_info_asid;
    }

    if (subs_info_ptr->is_priority_subs)
    {
      cfg_s_ptr->wms_ph_info.priority_subs = pref_info_asid;
    }
  }

  return;
} /* wms_cm_subs_event_cb() */

void wms_cm_suspend_pscall_cb(
  void                   *data,
  cm_call_cmd_e_type      call_cmd,
  cm_call_cmd_err_e_type  call_error
)
{
  if (CM_CALL_CMD_DATA_SUSPEND == call_cmd)
  {
    if (NULL != data)
    {
      cm_call_cmd_err_e_type   *status = (cm_call_cmd_err_e_type *)data;
      *status = call_error;
    }

    (void) rex_set_sigs(wms_myself, WMS_CM_RESPONSE_SIG);
  }
} /* wms_cm_suspend_pscall_cb() */

/*===========================================================================
FUNCTION wms_cm_suspend_pscall

DESCRIPTION
  To suspended active PS call on other subscription

DEPENDENCIES
  none

RETURN VALUE
  Error code of type form enum.

SIDE EFFECTS
  None.
===========================================================================*/
cm_call_cmd_err_e_type wms_cm_suspend_pscall(
  sys_modem_as_id_e_type as_id
)
{
  wms_cm_s_type          *cm_s_ptr               = wms_cm_s_ptr();
  boolean                 suspend_status         = FALSE;
  cm_call_cmd_err_e_type  ps_call_suspend_status = CM_CALL_CMD_ERR_OTHER;
  sys_modem_as_id_e_type  suspend_asid_a         = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type  suspend_asid_b         = SYS_MODEM_AS_ID_NONE;

  if (SYS_MODEM_AS_ID_1 == as_id)
  {
    suspend_asid_a = SYS_MODEM_AS_ID_2;
    suspend_asid_b = SYS_MODEM_AS_ID_3;
  }
  else if (SYS_MODEM_AS_ID_2 == as_id)
  {
    suspend_asid_a = SYS_MODEM_AS_ID_1;
    suspend_asid_b = SYS_MODEM_AS_ID_3;
  }
  else if (SYS_MODEM_AS_ID_3 == as_id)
  {
    suspend_asid_a = SYS_MODEM_AS_ID_1;
    suspend_asid_b = SYS_MODEM_AS_ID_2;
  }
  
  if (SYS_MODEM_AS_ID_NONE != suspend_asid_a)
  {
    MSG_HIGH_2("wms_cm_suspend_pscall : suspend PS call from as_id %d, suspend_asid %d", as_id, suspend_asid_a);

    (void) rex_clr_sigs(wms_myself, WMS_CM_RESPONSE_SIG);

    suspend_status = cm_mm_call_cmd_data_suspend( wms_cm_suspend_pscall_cb,
                                                  &ps_call_suspend_status,
                                                  cm_s_ptr->cm_client,
                                                  TRUE,
                                                  suspend_asid_a );

    if (suspend_status)
    {
      wms_signal_wait(WMS_CM_RESPONSE_SIG);  /* Wait for MMGSDI response */
    }
    else
    {
      ps_call_suspend_status = CM_CALL_CMD_ERR_OTHER;
    }
  }

  if (CM_CALL_CMD_ERR_NONE == ps_call_suspend_status)
  {
    if (SYS_MODEM_AS_ID_NONE != suspend_asid_b)
    {
      MSG_HIGH_2("wms_cm_suspend_pscall : suspend PS call from as_id %d, suspend_asid %d", as_id, suspend_asid_b);

      (void) rex_clr_sigs(wms_myself, WMS_CM_RESPONSE_SIG);

      suspend_status = cm_mm_call_cmd_data_suspend( wms_cm_suspend_pscall_cb,
                                                    &ps_call_suspend_status,
                                                    cm_s_ptr->cm_client,
                                                    TRUE,
                                                    suspend_asid_b );

      if (suspend_status)
      {
        wms_signal_wait(WMS_CM_RESPONSE_SIG);  /* Wait for MMGSDI response */
      }
      else
      {
        ps_call_suspend_status = CM_CALL_CMD_ERR_OTHER;
      }
    }
  }

  return ps_call_suspend_status;
} /* wms_cm_suspend_pscall() */

void wms_cm_resume_pscall(
  sys_modem_as_id_e_type as_id
)
{
  wms_cm_s_type          *cm_s_ptr       = wms_cm_s_ptr();
  sys_modem_as_id_e_type  resume_asid_a  = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type  resume_asid_b  = SYS_MODEM_AS_ID_NONE;

  if (SYS_MODEM_AS_ID_1 == as_id)
  {
    resume_asid_a = SYS_MODEM_AS_ID_2;
    resume_asid_b = SYS_MODEM_AS_ID_3;
  }
  else if (SYS_MODEM_AS_ID_2 == as_id)
  {
    resume_asid_a = SYS_MODEM_AS_ID_1;
    resume_asid_b = SYS_MODEM_AS_ID_3;
  }
  else if (SYS_MODEM_AS_ID_3 == as_id)
  {
    resume_asid_a = SYS_MODEM_AS_ID_1;
    resume_asid_b = SYS_MODEM_AS_ID_2;
  }

  if (SYS_MODEM_AS_ID_NONE != resume_asid_a)
  {
    MSG_HIGH_2("wms_cm_resume_pscall : Resume PS call from as_id %d, resume_asid %d", as_id, resume_asid_a);
    (void) cm_mm_call_cmd_data_suspend( NULL,
                                        NULL,
                                        cm_s_ptr->cm_client,
                                        FALSE,
                                        resume_asid_a );
  }

  if (SYS_MODEM_AS_ID_NONE != resume_asid_b)
  {
    MSG_HIGH_2("wms_cm_resume_pscall : Resume PS call from as_id %d, resume_asid %d", as_id, resume_asid_b);
    (void) cm_mm_call_cmd_data_suspend( NULL,
                                        NULL,
                                        cm_s_ptr->cm_client,
                                        FALSE,
                                        resume_asid_b );
  }

  return;
} /* wms_cm_resume_pscall() */

/*===========================================================================
FUNCTION wms_cm_resume_1x_pscall

DESCRIPTION
  To resume the suspended PS call on other subscription

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_resume_1x_pscall(
  sys_modem_as_id_e_type as_id
)
{
  if (0 == wms_msg_get_pending_1x_mo_sms_count())
  {
    wms_cm_resume_pscall(as_id);
  }
} /* wms_cm_resume_1x_pscall() */

/*===========================================================================
FUNCTION wms_cm_resume_gw_pscall

DESCRIPTION
  To resume the suspended PS call on other subscription

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_resume_gw_pscall(
  sys_modem_as_id_e_type as_id
)
{
  if (0 == wms_msg_get_pending_gw_mo_sms_count(as_id))
  {
    wms_cm_resume_pscall(as_id);
  }
} /* wms_cm_resume_gw_pscall() */

#if defined (FEATURE_GWSMS_MO_CONTROL) && defined (FEATURE_GSTK)
/*===========================================================================
FUNCTION wms_cm_map_sys_mode_to_gstk_access_tech_rat

DESCRIPTION
  This function will convert the sys mode to gstk_access_technology

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
gstk_access_technology_type wms_cm_map_sys_mode_to_gstk_access_tech_rat(
  sys_sys_mode_e_type sys_mode
)
{
  switch(sys_mode)
  {
    case SYS_SYS_MODE_CDMA:
      return GSTK_ACCESS_TECH_CDMA;

    case SYS_SYS_MODE_GSM:
      return GSTK_ACCESS_TECH_GSM;

    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
      return GSTK_ACCESS_TECH_UTRAN;

    case SYS_SYS_MODE_LTE:
      return GSTK_ACCESS_TECH_LTE;

    default:
      MSG_ERROR_1("wms_cm_map_sys_mode_to_gstk_access_tech_rat(): Invalid sys_mode %d", sys_mode);
      return GSTK_ACCESS_NONE;
  }
} /* wms_cm_map_sys_mode_to_gstk_access_tech_rat */
#endif /* FEATURE_GWSMS_MO_CONTROL && FEATURE_GSTK */
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */
