/**
  @file policyman_phone_events.c

  @brief
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_phone_events.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"

#include "policyman_call_events.h"
#include "policyman_cfgitem.h"
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_phone_events.h"
#include "policyman_plmn.h"
#include "policyman_policies.h"
#include "policyman_rat_capability.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_timer.h"
#include "policyman_util.h"
#include "policyman_xml.h"
#include "policyman_thermal_state.h"

#include "cm.h"
#include "cm_v.h"
#include "sys.h"
#include "cm_dualsim.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#define PMNV_EFS_ID_TDS_BANDPREF "/nv/item_files/modem/mmode/tds_bandpref"
#define POLICYMAN_USER_PLMN_NV_PATH "/policyman/user_plmn"


/*=============================================================================
 Data Structure for CM PH Info
==============================================================================*/
#define PM_CMPH_CHANGED_ASUBS_ID               0x00000001
#define PM_CMPH_CHANGED_MODE_PREF              0x00000002
#define PM_CMPH_CHANGED_SRV_DOMAIN_PREF        0x00000004
#define PM_CMPH_CHANGED_OPRT_MODE              0x00000008
#define PM_CMPH_CHANGED_NETWORK_SEL_MODE_PREF  0x00000010
#define PM_CMPH_CHANGED_USER_PLMN              0x00000020
#define PM_CMPH_CHANGED_GW_BANDS               0x00000040
#define PM_CMPH_CHANGED_LTE_BANDS              0x00000080
#define PM_CMPH_CHANGED_TDS_BANDS              0x00000100
#define PM_CMPH_CHANGED_VOLTE_STATE            0x00000200

struct policyman_cmph_info_s
{
  uint32                          changed_fields;
  sys_modem_as_id_e_type          asubs_id;              // Subscription ID to which these preferences should apply.
  cm_mode_pref_e_type             mode_pref;             // Indicates the current mode preference.
  cm_srv_domain_pref_e_type       srv_domain_pref;       // The service domain preferred by the client. (GSM/WCDMA/TDS only).
  sys_oprt_mode_e_type            oprt_mode;             // current operating mode
  cm_network_sel_mode_pref_e_type network_sel_mode_pref; // current network selection mode preference (AUTOMATIC/MANUAL)
  sys_plmn_id_s_type              user_plmn;             // user selected PLMN if network_sel_mode_pref = MANUAL
  cm_band_pref_e_type             gw_bands;              // user preference for GW bands
  sys_lte_band_mask_e_type        lte_bands;             // user preference for LTE bands
  cm_band_pref_e_type             tds_bands;             // user preference for TDS bands
  cm_volte_enable_e_type          volte_state;           // Current VoLTE state
  cm_rtre_control_e_type          rtre_control;          // RTRE control tells 1x subscription source
};

/*=============================================================================
 Data Structure for event registration
==============================================================================*/
 struct policyman_event_s
 {
   cm_client_id_type  cm_clnt_id;
 };

 struct policyman_cmph_event_s
 {
   cm_ph_event_e_type beg;
   cm_ph_event_e_type end;
 };

 struct policyman_cmss_event_s
 {
   cm_ss_event_e_type beg;
   cm_ss_event_e_type end;
 };

 struct policyman_cmcall_event_s
 {
   cm_call_event_e_type beg;
   cm_call_event_e_type end;
 };

 struct policyman_event_s policyman_event;

/*=============================================================================
  Phone Event callback definition.
=============================================================================*/
typedef struct
{
  POLICYMAN_CMD_HDR;

  cm_ph_event_e_type evt;
  cm_ph_info_s_type info;
  uint32                timestamp;
} policyman_cmph_evt_cb_t;

/*=============================================================================
  Phone Event History definitions
=============================================================================*/
struct policyman_cmph_history_entry_s {
  size_t                         next;
  uint32                         timestamp;
  cm_ph_event_e_type             ph_evt;
  policyman_cmph_info_t          ph_info;
};

struct policyman_cmph_history_s {
  size_t                          index;
  policyman_cmph_history_entry_t  h[PM_DUMP_HISTORY_DEPTH];
};

/*=============================================================================
  Function Protoypes
=============================================================================*/
static cm_mode_pref_e_type policyman_map_mode_pref_nv2cm( nv_mode_enum_type  nv_mode );
static cm_srv_domain_pref_e_type  policyman_map_srv_domain_pref_nv2cm(nv_srv_domain_pref_enum_type   nv_mode);

/*------------------------------------------------------------------------
  FUNCTION POLICYMAN_GET_CM_EVENT_CLIENT_ID()

  DESCRIPTION
    Function to get CM client id that registered for events
--------------------------------------------------------------------------*/
cm_client_id_type policyman_get_cm_event_client_id(void);


sd_ss_mode_pref_e_type cmph_map_cm_mode_pref_to_sd_mode_pref(
  cm_mode_pref_e_type mode_pref
);

/*=============================================================================
  APIs for CM Phone Event information
=============================================================================*/

/*-------- policyman_cmph_read_nv_mode_pref --------*/
cm_mode_pref_e_type policyman_cmph_read_nv_mode_pref(
  sys_modem_as_id_e_type asubs_id
)
{
  nv_stat_enum_type   nv_status;
  nv_item_type        nv_item;
  cm_mode_pref_e_type mode_pref = CM_MODE_PREF_GSM_ONLY;
  
  memset(&nv_item, 0x00, sizeof(nv_item_type));

  // read NV item for NV #10 to get mode_pref
  nv_status = mre_efs_get_nv_item(NV_PREF_MODE_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    mode_pref = policyman_map_mode_pref_nv2cm(nv_item.pref_mode.mode); 
  }

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "Got CM mode_pref %d",
                        asubs_id,
                        mode_pref );

  return mode_pref;
}


/*-------- policyman_cmph_read_nv_srv_domain_pref --------*/
cm_srv_domain_pref_e_type policyman_cmph_read_nv_srv_domain_pref(
  sys_modem_as_id_e_type asubs_id
)
{
  nv_stat_enum_type         nv_status;
  nv_item_type              nv_item;
  cm_srv_domain_pref_e_type srv_domain_pref = CM_SRV_DOMAIN_PREF_CS_ONLY;
  
  memset(&nv_item, 0x00, sizeof(nv_item_type));

  // read NV item for NV #850 to get srv_domain_pref
  nv_status = mre_efs_get_nv_item(NV_SERVICE_DOMAIN_PREF_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    srv_domain_pref = policyman_map_srv_domain_pref_nv2cm(nv_item.service_domain_pref.srv_domain);
  }

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "Got CM service_domain_pref %d",
                        asubs_id,
                        srv_domain_pref );

  return srv_domain_pref;
}


/*-------- policyman_cmph_read_nv_band_pref --------*/
cm_band_pref_e_type policyman_cmph_read_nv_band_pref(
  sys_modem_as_id_e_type asubs_id
)
{
  nv_stat_enum_type   nv_status;
  nv_item_type        nv_item;
  cm_band_pref_e_type gw_bands;
  
  memset(&nv_item, 0x00, sizeof(nv_item_type));

  /* read NV item 441 to get first 16 bits of GW Band pref
   */
  nv_status = mre_efs_get_nv_item(NV_BAND_PREF_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    gw_bands = (cm_band_pref_e_type)(nv_item.band_pref.band);
  }
  else
  {
    gw_bands = CM_BAND_PREF_ANY; 
  }

  /* read NV item 946 to get 16-31 bits of GW Band pref
   */
  nv_status = mre_efs_get_nv_item(NV_BAND_PREF_16_31_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    gw_bands |= ((cm_band_pref_e_type)(nv_item.band_pref.band) << 16);
  }
  else
  {
    gw_bands = CM_BAND_PREF_ANY; 
  }

  /* read NV item 962 to get 32-63 bits of GW Band pref
   */
  nv_status = mre_efs_get_nv_item(NV_BAND_PREF_32_63_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    gw_bands |= ((cm_band_pref_e_type)(nv_item.band_pref_32_63.band) << 32);
  }
  else
  {
    gw_bands = CM_BAND_PREF_ANY; 
  }

  return gw_bands;
}


/*-------- policyman_cmph_read_nv_lte_bc_config --------*/
sys_lte_band_mask_e_type policyman_cmph_read_nv_lte_bc_config(
  sys_modem_as_id_e_type asubs_id
)
{
  nv_stat_enum_type        nv_status;
  nv_item_type             nv_item;
  sys_lte_band_mask_e_type lte_bands;
  
  memset(&nv_item, 0x00, sizeof(nv_item_type));

  /* read NV item 6828 to get LTE Band pref
   */
  nv_status = mre_efs_get_nv_item(NV_LTE_BC_CONFIG_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    lte_bands = sys_map_band_mask_to_lte_band_mask(nv_item.lte_bc_config.lte_bc_config);
  }
  else
  {
    SYS_LTE_BAND_MASK_SET_ALL_BANDS(&lte_bands);
  }

  return lte_bands;
}


/*-------- policyman_cmph_read_tds_band_pref --------*/
cm_band_pref_e_type policyman_cmph_read_tds_band_pref(
  sys_modem_as_id_e_type asubs_id
)
{
  mre_efs_status_t     status;
  cm_band_pref_e_type *pBand     = NULL;
  size_t               bandSize;
  cm_band_pref_e_type  tds_bands;

  /* read TDSCDMA RF Band preference
    */
  status = mre_efs_get_file( PMNV_EFS_ID_TDS_BANDPREF,
                             (void **)&pBand,
                             &bandSize,
                             asubs_id );

  /*  If we were able to read the NV and if the value is in the proper range,
   *  set the mode to that value.  Otherwise just return NORMAL mode.
   */
  if (   (status == MRE_EFS_STATUS_SUCCESS)
      && (bandSize == sizeof(cm_band_pref_e_type))
      && (pBand != NULL) )
  {
    tds_bands = *pBand;
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "read TDS band %lld from EFS success",
                          asubs_id,
                          tds_bands );
  }
  else
  {
    tds_bands = CM_BAND_PREF_TDS_ANY;
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "unable to read TDS band from EFS (status %d); setting to default CM_BAND_PREF_TDS_ANY",
                          asubs_id,
                          status );
  }    

  POLICYMAN_MEM_FREE_IF(pBand);
  return tds_bands;
}


/*-------- policyman_cmph_nv_init --------*/
void policyman_cmph_nv_init(
  policyman_cmph_info_t   **ppInfo,
  sys_modem_as_id_e_type    asubs_id
)
{
  (*ppInfo)->mode_pref       = policyman_cmph_read_nv_mode_pref(asubs_id);
  (*ppInfo)->srv_domain_pref = policyman_cmph_read_nv_srv_domain_pref(asubs_id);
  (*ppInfo)->gw_bands        = policyman_cmph_read_nv_band_pref(asubs_id);
  (*ppInfo)->lte_bands       = policyman_cmph_read_nv_lte_bc_config(asubs_id);
  (*ppInfo)->tds_bands       = policyman_cmph_read_tds_band_pref(asubs_id);
}


/*-------- policyman_cmph_init --------*/
boolean policyman_cmph_init(
  policyman_cmph_info_t   **ppInfo,
  sys_modem_as_id_e_type    asubs_id
)
{
  *ppInfo = (policyman_cmph_info_t *)policyman_mem_alloc(sizeof(policyman_cmph_info_t));

  /* initialize PM client id to 0 for CM Events
   */
  memset(&policyman_event, 0x00, sizeof(struct policyman_event_s));

  policyman_cmph_nv_init(ppInfo, asubs_id);

  /* update Ph state with asubs id and set the precondition
   */
 (*ppInfo)->asubs_id    = asubs_id;
 (*ppInfo)->oprt_mode   = SYS_OPRT_MODE_PWROFF;
 (*ppInfo)->volte_state = CM_VOLTE_NO_CHANGE;
 (*ppInfo)->rtre_control = CM_RTRE_CONTROL_NONE;

  (*ppInfo)->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
  policyman_plmn_clear(&(*ppInfo)->user_plmn);

  return TRUE;
}


/*-------- policyman_cmph_deinit --------*/
void policyman_cmph_deinit(
  policyman_cmph_info_t *pInfo
  )
{
  POLICYMAN_MEM_FREE_IF(pInfo);
}


/*-------- policyman_cmph_init_history --------*/
void policyman_cmph_init_history(
  policyman_state_t  *pState
)
{
  if (NULL == pState->pCmphHistory)
  {
    int   idx;

    pState->pCmphHistory = (policyman_cmph_history_t *)policyman_mem_alloc(sizeof(policyman_cmph_history_t));

    for (idx = 0; idx < (PM_DUMP_HISTORY_DEPTH); ++idx)
    {
      pState->pCmphHistory->h[idx].next      = idx + 1;
      pState->pCmphHistory->h[idx].timestamp = 0xC0FFEE;
      pState->pCmphHistory->h[idx].ph_evt    = CM_PH_EVENT_NONE;
    }

    pState->pCmphHistory->h[PM_DUMP_HISTORY_DEPTH - 1].next = 0;
  }
}

/*-------- policyman_cmph_deinit_history --------*/
void policyman_cmph_deinit_history(
  policyman_state_t  *pState
)
{
  POLICYMAN_MEM_FREE_IF(pState->pCmphHistory);
}

/*-------- policyman_cmph_update_history --------*/
void policyman_cmph_update_history(
  policyman_state_t      *pState,
  cm_ph_event_e_type      ph_evt,
  policyman_cmph_info_t  *ph_info,
  uint32                  timestamp
)
{
  if (NULL != pState->pCmphHistory)
  {
    pState->pCmphHistory->h[pState->pCmphHistory->index].timestamp  = timestamp;
    pState->pCmphHistory->h[pState->pCmphHistory->index].ph_evt     = ph_evt;
    pState->pCmphHistory->h[pState->pCmphHistory->index].ph_info    = *ph_info;

    pState->pCmphHistory->index = pState->pCmphHistory->h[pState->pCmphHistory->index].next;
  }
}


/*-------- policyman_cmph_fill_diag_ss_info --------*/
void policyman_cmph_fill_diag_ph_info(
  policyman_cmph_info_t   *src_ph_info,
  pm_dump_cm_ph_info_t    *dump_ph_info
)
{
  dump_ph_info->changed_fields          = src_ph_info->changed_fields;
  dump_ph_info->asubs_id                = src_ph_info->asubs_id;
  dump_ph_info->mode_pref               = src_ph_info->mode_pref;
  dump_ph_info->srv_domain_pref         = src_ph_info->srv_domain_pref;
  dump_ph_info->oprt_mode               = src_ph_info->oprt_mode;
  dump_ph_info->network_sel_mode_pref   = src_ph_info->network_sel_mode_pref;
  dump_ph_info->user_plmn.identity[0]   = src_ph_info->user_plmn.identity[0];
  dump_ph_info->user_plmn.identity[1]   = src_ph_info->user_plmn.identity[1];
  dump_ph_info->user_plmn.identity[2]   = src_ph_info->user_plmn.identity[2];
  dump_ph_info->gw_bands                = src_ph_info->gw_bands;
#if (LTE_BAND_NUM == 256)
  dump_ph_info->lte_band_count          = 4;
  dump_ph_info->lte_bands0              = src_ph_info->lte_bands.bits_1_64;
  dump_ph_info->lte_bands1              = src_ph_info->lte_bands.bits_65_128;
  dump_ph_info->lte_bands2              = src_ph_info->lte_bands.bits_129_192;
  dump_ph_info->lte_bands3              = src_ph_info->lte_bands.bits_193_256;
#else
  dump_ph_info->lte_band_count          = 1;
  dump_ph_info->lte_bands0              = src_ph_info->lte_bands;
#endif
  dump_ph_info->tds_bands               = src_ph_info->tds_bands;
  dump_ph_info->volte_state             = src_ph_info->volte_state;
}

/*-------- policyman_cmph_history_entry_to_dump --------*/
STATIC void policyman_cmph_history_entry_to_dump(
  policyman_cmph_history_entry_t *hist_entry,
  pm_dump_cm_ph_info_history_t   *dump_entry
)
{
  dump_entry->timestamp      = hist_entry->timestamp;

  dump_entry->cm_ph_evt      = hist_entry->ph_evt;

  policyman_cmph_fill_diag_ph_info( &hist_entry->ph_info,
                                    &dump_entry->cm_ph_info );
}

/*-------- policyman_cmph_get_history --------*/
size_t policyman_cmph_get_history(
  pm_dump_cm_ph_info_history_t *h_buffer,
  size_t                        h_len
)
{
  policyman_state_t      *pState;
  int                     oldest_hist_idx;
  int                     start_idx;
  int                     hist_idx;
  int                     offset;

  pState = policyman_state_get_state();

  // If given length larger than history, get only max history available
  if (h_len > PM_DUMP_HISTORY_DEPTH)
  {
    h_len = PM_DUMP_HISTORY_DEPTH;
  }

  oldest_hist_idx = pState->pCmphHistory->index;  // points to oldest history record

  // Calculate the index to start with for the most recent h_len history entries
  start_idx = (oldest_hist_idx + (PM_DUMP_HISTORY_DEPTH - h_len)) % PM_DUMP_HISTORY_DEPTH;

  hist_idx = start_idx;

  for (offset = 0; offset < h_len; offset++ )
  {
    policyman_cmph_history_entry_to_dump( &pState->pCmphHistory->h[hist_idx],
                                          &h_buffer[offset] );

    hist_idx = pState->pCmphHistory->h[hist_idx].next;
  }

  return h_len;
}


/*-------- policyman_cmph_info_deep_copy --------*/
policyman_cmph_info_t * policyman_cmph_info_deep_copy(
  policyman_cmph_info_t  *pCmPhState
)
{
  policyman_cmph_info_t *pInfo;
  
  pInfo = (policyman_cmph_info_t *)policyman_mem_alloc(sizeof(policyman_cmph_info_t));

  *pInfo = *pCmPhState;

  return pInfo;
}


/*-------- policyman_phone_get_rtre_control --------*/
cm_rtre_control_e_type policyman_phone_get_rtre_control(
  policyman_state_t     *pState,
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_cmph_info_t   *pCmPhInfo;

  pCmPhInfo = policyman_state_get_cm_ph_state(pState, asubs_id);
  return pCmPhInfo->rtre_control;
}

/*-------- policyman_phone_update_rtre_control --------*/
boolean policyman_phone_update_rtre_control(
  policyman_state_t  *pState,
  cm_ph_info_s_type  *pPhInfo
)
{
  cm_rtre_control_e_type   current_rtre_control;
  policyman_cmph_info_t   *pCmPhInfo;

  pCmPhInfo = policyman_state_get_cm_ph_state(pState, pPhInfo->asubs_id);

  current_rtre_control = pCmPhInfo->rtre_control;
  pCmPhInfo->rtre_control = pPhInfo->rtre_control;
  
  POLICYMAN_MSG_HIGH_2("RTRE Control updated from %d to %d",
                       current_rtre_control, 
                       pPhInfo->rtre_control);

  return (current_rtre_control != pPhInfo->rtre_control);
}

/*===========================================================================
  FUNCTION POLICYMAN_EXECUTE_PROCESS_CM_PHONE_EVT()

  DESCRIPTION
    Update PH state and handle changes 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void policyman_execute_process_cmph_evt(
  policyman_cmd_t *pCmd
)
{
  policyman_cmph_evt_cb_t *ph_evt = (policyman_cmph_evt_cb_t *) pCmd;

  if (NULL != ph_evt)
  {
    POLICYMAN_MSG_HIGH_1("process cm ph event %d", ph_evt->evt);

    switch(ph_evt->evt)
    {
      case CM_PH_EVENT_OPRT_MODE:
        {
          policyman_cmph_info_t  ph_info;

          memset(&ph_info, 0, sizeof(ph_info));

          ph_info.oprt_mode = ph_evt->info.oprt_mode;
          policyman_cmph_update_history( ph_evt->pState,
                                         ph_evt->evt,
                                         &ph_info,
                                         ph_evt->timestamp );
        }
        (void)policyman_phone_update_rtre_control(ph_evt->pState, &ph_evt->info);
        policyman_state_enter_crit_section();
        policyman_state_update_ph_oprt_mode(ph_evt->pState, ph_evt->info.oprt_mode);
        policyman_state_leave_crit_section();
        break;

      case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
        policyman_state_enter_crit_section();
        policyman_state_update_network_scan_result(pCmd->pState, &ph_evt->info);
        policyman_state_leave_crit_section();
        break;

      case CM_PH_EVENT_NVRUIM_CONFIG_CHANGED:
        {
          boolean rtre_has_changed = FALSE;
          
          policyman_state_enter_crit_section();
          rtre_has_changed = policyman_phone_update_rtre_control(ph_evt->pState, &ph_evt->info);
          policyman_state_leave_crit_section();

          if (rtre_has_changed)
          {
            policyman_state_handle_update(ph_evt->pState);
          }   
        }
        break;

      default:
        break;
    }
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_CMPH_EVENT_CB()

  DESCRIPTION
    Handle CM Phone event callback

  PARAMETERS
    evt     : CM Phone EVENT type
    p_info : actual payload of CM Phone EVENT

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void policyman_cmph_event_cb(
  cm_ph_event_e_type evt,
  const cm_ph_info_s_type *p_info
)
{
  policyman_cmph_evt_cb_t *pCmd = NULL;

  POLICYMAN_MSG_HIGH_1("received cm ph evt %d", evt);

  pCmd = POLICYMAN_CMD_NEW( policyman_cmph_evt_cb_t,
                            policyman_execute_process_cmph_evt,
                            NULL,
                            NULL ); // No policy associated with CM Phone Event

  pCmd->evt    = evt;
  pCmd->info = *p_info;
  pCmd->timestamp = mre_util_get_time();

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
}


/*-------- policyman_ph_get_mode_pref --------*/
cm_mode_pref_e_type policyman_ph_get_mode_pref(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
)
{
  policyman_cmph_info_t *pPhInfo;

  pPhInfo = policyman_state_get_cm_ph_state(pState, asubs_id);

  return pPhInfo->mode_pref;
}


/*-------- policyman_ph_get_mode_pref_mask --------*/
uint32 policyman_ph_get_mode_pref_mask(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
)
{
  cm_mode_pref_e_type                    mode_pref;
  uint32                                 mask;

  mode_pref = policyman_ph_get_mode_pref(pState, asubs_id);

  mask = (uint32) cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref);

  return mask;
}


/*-------- policyman_ph_get_srv_domain --------*/
cm_srv_domain_pref_e_type policyman_ph_get_srv_domain(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
)
{
  policyman_cmph_info_t *pPhInfo;

  pPhInfo = policyman_state_get_cm_ph_state(pState, asubs_id);

  return pPhInfo->srv_domain_pref;
}


/*===========================================================================
  FUNCTION POLICYMAN_PH_SET_OPRT_MODE()

  DESCRIPTION
    Set operating mode into PM Phone state

  PARAMETERS
    pPhInfo     : Pointer to PM Phone State info
    oprt_mode : operating mode to be set

  RETURN VALUE
    TRUE if mode_pref changed, FALSE otherwise
===========================================================================*/
boolean policyman_ph_set_oprt_mode(
  policyman_cmph_info_t *pPhInfo,
  sys_oprt_mode_e_type   oprt_mode
)
{
  if (pPhInfo->oprt_mode != oprt_mode)
  {
    pPhInfo->oprt_mode = oprt_mode;
    POLICYMAN_MSG_HIGH_1("oprt_mode %d updated in PM state", oprt_mode);

    /* we don't want to run rules in FTM mode
      */
    return oprt_mode != SYS_OPRT_MODE_FTM;
  }

  return FALSE;
}


/*===========================================================================
  FUNCTION POLICYMAN_PH_GET_OPRT_MODE()

  DESCRIPTION
    Get operating mode from PM Phone state

  PARAMETERS
    None

  RETURN VALUE
    TRUE if mode_pref changed, FALSE otherwise
===========================================================================*/
sys_oprt_mode_e_type policyman_ph_get_oprt_mode(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_cmph_info_t *pCmPhInfo;

  pCmPhInfo = policyman_state_get_cm_ph_state(pState, subsId);

  return pCmPhInfo->oprt_mode;
}


/*-------- policyman_ue_is_online --------*/
boolean policyman_ue_is_online(
  policyman_state_t *pState
)
{
  return policyman_ph_get_oprt_mode(pState, SYS_MODEM_AS_ID_1) == SYS_OPRT_MODE_ONLINE;
}


/*-------- policyman_ph_get_net_sel_mode_pref --------*/
cm_network_sel_mode_pref_e_type policyman_ph_get_net_sel_mode_pref(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
)
{
  policyman_cmph_info_t *pPhInfo;

  pPhInfo = policyman_state_get_cm_ph_state(pState, asubs_id);

  return pPhInfo->network_sel_mode_pref;
}



/*-------- policyman_ph_get_user_plmn --------*/
sys_plmn_id_s_type policyman_ph_get_user_plmn(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
)
{
  policyman_cmph_info_t *pPhInfo;

  pPhInfo = policyman_state_get_cm_ph_state(pState, asubs_id);

  return pPhInfo->user_plmn;
}



/*-------- policyman_ph_get_band_pref --------*/
void policyman_ph_get_band_pref(
  policyman_state_t         *pState,
  sys_modem_as_id_e_type     asubs_id,
  cm_band_pref_e_type       *gw_bands,
  sys_lte_band_mask_e_type  *lte_bands,
  cm_band_pref_e_type       *tds_bands
)
{
  policyman_cmph_info_t *pPhInfo;

  pPhInfo = policyman_state_get_cm_ph_state(pState, asubs_id);

  *gw_bands = pPhInfo->gw_bands;
  *lte_bands = pPhInfo->lte_bands;
  *tds_bands = pPhInfo->tds_bands;
}


/*===========================================================================
  FUNCTION POLICYMAN_PH_GET_VOLTE_IS_ENABLED()

  DESCRIPTION
    Get per subs volte_is_enabled from PM Phone state

  PARAMETERS
    None

  RETURN VALUE
    TRUE if volte_is_enabled, FALSE otherwise
===========================================================================*/
boolean policyman_ph_get_volte_is_enabled(
  policyman_state_t        *pState,
  sys_modem_as_id_e_type    subsId
)
{
  policyman_cmph_info_t *pCmPhInfo;

  pCmPhInfo = policyman_state_get_cm_ph_state(pState, subsId);

  return (pCmPhInfo->volte_state == CM_VOLTE_ENABLED);
}

/*===========================================================================
  FUNCTION policyman_ph_set_volte_enabled_state()

  DESCRIPTION
    Set volte_state into PM Phone state

  PARAMETERS
    sys_modem_as_id_e_type : Subscription to update
    cm_volte_enable_e_type : Value of the volte_enabled state to be set

  RETURN VALUE
    Boolean: TRUE if the internal volte state was changed
             FALSE if the requested volte state was already in the PM state
===========================================================================*/
boolean policyman_ph_set_volte_enabled_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id,
  cm_volte_enable_e_type  volte_state
)
{
  policyman_cmph_info_t  *pPhInfo;

  pPhInfo = policyman_state_get_cm_ph_state(pState, asubs_id);

  if (pPhInfo->volte_state != volte_state)
  {
    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "volte_enabled changed %d -> %d",
                          asubs_id,
                          pPhInfo->volte_state,
                          volte_state );

    pPhInfo->volte_state = volte_state;
    return TRUE;
  }

  return FALSE;
} /* policyman_ph_set_volte_enabled_state() */

/*===========================================================================
  FUNCTION policyman_ph_get_volte_enabled_state()

  DESCRIPTION
    Get volte_enabled state from PM Phone state

  PARAMETERS
    None

  RETURN VALUE
    Value of 'volte_state' in the subscription state
===========================================================================*/
cm_volte_enable_e_type policyman_ph_get_volte_enabled_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
)
{
  policyman_cmph_info_t *pCmPhInfo;

  pCmPhInfo = policyman_state_get_cm_ph_state(pState, asubs_id);

  return pCmPhInfo->volte_state;
} /* policyman_ph_get_volte_enabled_state() */



sd_ss_mode_pref_e_type cmph_map_cm_mode_pref_to_sd_mode_pref(
  cm_mode_pref_e_type mode_pref
);


/*------------------------------------------------------------------------
  FUNCTION POLICYMAN_REGISTER_FOR_CM_EVENTS()

  DESCRIPTION
    With this function, Policy Manager registers with CM events as a client
--------------------------------------------------------------------------*/
void policyman_register_for_cm_events(
  void
)
{
  cm_client_status_e_type ret_val = CM_CLIENT_ERR_CLIENT_ID;
  int                     i = 0;
  static boolean registered_for_events;

  const struct policyman_cmph_event_s ph_event_list[] = {
                                                          { CM_PH_EVENT_INFO, CM_PH_EVENT_INFO },
                                                          { CM_PH_EVENT_OPRT_MODE, CM_PH_EVENT_OPRT_MODE },
                                                          { CM_PH_EVENT_AVAILABLE_NETWORKS_CONF, CM_PH_EVENT_AVAILABLE_NETWORKS_CONF },
                                                          { CM_PH_EVENT_NVRUIM_CONFIG_CHANGED, CM_PH_EVENT_NVRUIM_CONFIG_CHANGED}
                                                      };

  const struct policyman_cmss_event_s ss_event_list[] = {
                                                          { CM_SS_EVENT_SRV_CHANGED, CM_SS_EVENT_INFO },
                                                          { CM_SS_EVENT_REG_REJECT, CM_SS_EVENT_REG_REJECT }
                                                        };

  const struct policyman_cmcall_event_s call_event_list[] = {
                                                              { CM_CALL_EVENT_ORIG, CM_CALL_EVENT_ORIG },
                                                              { CM_CALL_EVENT_INCOM, CM_CALL_EVENT_INCOM },
                                                              { CM_CALL_EVENT_CONNECT, CM_CALL_EVENT_CONNECT },
                                                              { CM_CALL_EVENT_END, CM_CALL_EVENT_END }
                                                            };

  if(!registered_for_events)
  {

    ret_val = cm_client_init( CM_CLIENT_TYPE_POLICY_MANAGER, &policyman_event.cm_clnt_id);

    if ( ret_val != CM_CLIENT_OK )
    {
      goto Done;
    }

    /* register for CM Phone Events
      */
    for ( i=0; i<ARR_SIZE(ph_event_list); i++ )
    {
      ret_val = cm_client_ph_reg( policyman_event.cm_clnt_id,
                                  policyman_cmph_event_cb,
                                  CM_CLIENT_EVENT_REG,
                                  ph_event_list[i].beg,
                                  ph_event_list[i].end,
                                  NULL );

      if ( ret_val != CM_CLIENT_OK )
      {
        break;
      }
   }

    /* register for CM Serving System Events
      */
    for ( i=0; i<ARR_SIZE(ss_event_list); i++ )
    {
      ret_val = cm_mm_client_ss_reg_msim( policyman_event.cm_clnt_id,
                                          policyman_msim_cmss_event_cb,
                                          CM_CLIENT_EVENT_REG,
                                          ss_event_list[i].beg,
                                          ss_event_list[i].end,
                                          NULL,
                                          SYS_MODEM_AS_ID_1_MASK|SYS_MODEM_AS_ID_2_MASK
                                        );

      if ( ret_val != CM_CLIENT_OK )
      {
        break;
      }
    }

    /* register for CM Call Events
      */
    for ( i=0; i<ARR_SIZE(call_event_list); i++ )
    {
      ret_val = cm_mm_client_call_reg( policyman_event.cm_clnt_id,
                                       policyman_cmcall_event_cb,
                                       CM_CLIENT_EVENT_REG,
                                       call_event_list[i].beg,
                                       call_event_list[i].end,
                                       NULL
                                     );

    if ( ret_val != CM_CLIENT_OK )
    {
         break;
    }
  }

  registered_for_events = TRUE;
  return;

Done:
    /* release clients in case of an error
      */
    (void)cm_client_release( policyman_event.cm_clnt_id );
    POLICYMAN_MSG_ERROR_1("PM failed to register to CM Events with error cause %d", ret_val);
  }
}

/*=============================================================================
  Phone-related conditions
=============================================================================*/


/*-----------------------------------------------------------------------------
  User domain pref
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  cm_srv_domain_pref_e_type domain_pref;
} policyman_domain_pref_condition_t;



/*-------- policyman_str_to_domain --------*/
cm_srv_domain_pref_e_type policyman_str_to_domain(
  char const  *pStr
  )
{
  typedef struct
  {
    const char                *pStr;
    cm_srv_domain_pref_e_type domain;
  } domain_map;

  cm_srv_domain_pref_e_type domain = CM_SRV_DOMAIN_PREF_NONE;

  static domain_map map[] =
  {
    {"CS",    CM_SRV_DOMAIN_PREF_CS_ONLY},
    {"PS",    CM_SRV_DOMAIN_PREF_PS_ONLY},
    {"CSPS",  CM_SRV_DOMAIN_PREF_CS_PS},
    {"ANY",   CM_SRV_DOMAIN_PREF_ANY},
  };

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      domain = map[i].domain;
      break;
    }
  }

  if (domain == CM_SRV_DOMAIN_PREF_NONE)
  {
    POLICYMAN_UT_MSG_0("Invalid value for <user_domain_pref>.  Valid values are:");
    for (i = 0 ; i < ARR_SIZE(map) ; ++i)
    {
      POLICYMAN_UT_MSG_1("    %s", map[i].pStr);
    }
  }

  return domain;
}


/*-------- evaluate_user_domain_pref --------*/
static boolean evaluate_user_domain_pref(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_domain_pref_condition_t const *pCond = (policyman_domain_pref_condition_t *) pCondition;
  policy_execute_ctx_t                    *pPhCtx = (policy_execute_ctx_t *)pCtx;
  cm_srv_domain_pref_e_type                srv_domain;
  boolean                                  result;

  srv_domain = policyman_ph_get_srv_domain(pPhCtx->pState, subsId);

  result =   pCond->domain_pref == CM_SRV_DOMAIN_PREF_ANY
          || pCond->domain_pref == srv_domain
          ;

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX SRC_LINE "condition <user_domain_pref pmsubs=%d> with srv_domain %d returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId, 
                        srv_domain,
                        result);
  return result;
}


/*-------- policyman_condition_ph_domain_pref_evaluate --------*/
static boolean policyman_condition_ph_domain_pref_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;
  return policyman_evaluate_cond(pCond, pCtx, evaluate_user_domain_pref);
}


/*-------- policyman_condition_ph_domain_pref_new --------*/
mre_status_t policyman_condition_ph_domain_pref_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                      status = MRE_STATUS_ERR_MALFORMED_XML;
  cm_srv_domain_pref_e_type         domainPref;
  policyman_domain_pref_condition_t *pCondition;
  char const                        *pStr;
  policyman_subs_t                  pm_subs;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<user_domain_pref> must specify a domain preference", POLICY_SUBS(pPolicy));
    goto Done;
  }

  domainPref= policyman_str_to_domain(pStr);

  if (domainPref == CM_SRV_DOMAIN_PREF_NONE)
  {
    POLICYMAN_UT_MSG_2( SUBS_PREFIX "<user_domain_pref> invalid domain preferences: %s",
                        POLICY_SUBS(pPolicy),
                        pStr );
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_domain_pref_condition_t,
                                        pPolicy,
                                        policyman_condition_ph_domain_pref_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  pCondition->domain_pref = domainPref;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*-----------------------------------------------------------------------------
  User mode pref
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  uint32  mask;
} policyman_mode_pref_condition_t;



/*-------- evaluate_user_mode_pref_contains --------*/
static boolean evaluate_user_mode_pref_contains(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_mode_pref_condition_t const *pCond = (policyman_mode_pref_condition_t *) pCondition;
  policy_execute_ctx_t                  *pPrefCtx = (policy_execute_ctx_t *)pCtx;
  cm_mode_pref_e_type                    mode_pref = policyman_ph_get_mode_pref(pPrefCtx->pState, subsId);
  uint32                                 mask;
  boolean                                result;
  uint32                                 rat_disable_mask;

  rat_disable_mask = policyman_state_get_thermal_rat_disable_mask();
  mask = (uint32) cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref);

  mask &= ~rat_disable_mask;
  result = ((mask & pCond->mask) == pCond->mask);

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX SRC_LINE "condition <user_mode_pref_contains pmsubs=%d> with mask %d returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        mask, 
                        result);
  return result;
}


static boolean policyman_condition_ph_mode_pref_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;
  return policyman_evaluate_cond(pCond, pCtx, evaluate_user_mode_pref_contains);
}


mre_status_t policyman_condition_ph_rat_pref_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                    status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_mode_pref_condition_t *pCondition;
  char const                      *pStr;
  uint32                          mask = 0;
  policyman_subs_t                pm_subs;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<user_mode_pref_contains> must specify RATs", POLICY_SUBS(pPolicy));
    goto Done;
  }

  if (!policyman_rat_config_parse_rats(pStr, &mask))
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<user_mode_pref_contains> invalid RAT", POLICY_SUBS(pPolicy));
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_mode_pref_condition_t,
                                        pPolicy,
                                        policyman_condition_ph_mode_pref_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  pCondition->mask = mask;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*-----------------------------------------------------------------------------
  Operating mode pref
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  sys_oprt_mode_e_type oprt_mode;
} policyman_operating_mode_condition_t;


/*-------- policyman_str_to_operating_mode --------*/
sys_oprt_mode_e_type
policyman_str_to_operating_mode(
  char const  *pStr
  )
{
  typedef struct
  {
    const char                *pStr;
    sys_oprt_mode_e_type      oprt_mode;
  } operating_mode_map;

  sys_oprt_mode_e_type oprt_mode = SYS_OPRT_MODE_NONE;

  static operating_mode_map map[] =
  {
    {"SLEEP",   SYS_OPRT_MODE_LPM},
    {"ONLINE",  SYS_OPRT_MODE_ONLINE},
    {"OFFLINE", SYS_OPRT_MODE_OFFLINE},
    {"RESET",   SYS_OPRT_MODE_RESET},
    {"PWROFF",  SYS_OPRT_MODE_PWROFF},
  };

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      oprt_mode = map[i].oprt_mode;
      break;
    }
  }

  if (oprt_mode == SYS_OPRT_MODE_NONE)
  {
    POLICYMAN_UT_MSG_1("Invalid value for <phone_operating_mode>: %s.  Valid values are:", token);
    for (i = 0 ; i < ARR_SIZE(map) ; ++i)
    {
      POLICYMAN_UT_MSG_1("    %s", map[i].pStr);
    }
  }

  return oprt_mode;
}


/*-------- evaluate_phone_operating_mode --------*/
static boolean evaluate_phone_operating_mode(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_operating_mode_condition_t const *pCond = (policyman_operating_mode_condition_t *) pCondition;
  policy_execute_ctx_t                       *pPrefCtx = (policy_execute_ctx_t *)pCtx;
  sys_oprt_mode_e_type                        oprt_mode;
  boolean                     result;

  oprt_mode = policyman_ph_get_oprt_mode(pPrefCtx->pState, subsId);

  result = pCond->oprt_mode == oprt_mode;

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <phone_operating_mode pmsubs=%d> returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        result );
  return result;
}


/*-------- policyman_condition_ph_operating_mode_evaluate --------*/
static boolean policyman_condition_ph_operating_mode_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_phone_operating_mode);
}


/*-------- policyman_condition_ph_operating_mode_new --------*/
mre_status_t policyman_condition_ph_operating_mode_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                          status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_operating_mode_condition_t *pCondition;
  char const                            *pStr;
  sys_oprt_mode_e_type                  mode;
  policyman_subs_t                      pm_subs;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<phone_operating_mode> must specify an operating mode", POLICY_SUBS(pPolicy));
    goto Done;
  }

  mode= policyman_str_to_operating_mode(pStr);

  if (mode == SYS_OPRT_MODE_NONE)
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<phone_operating_mode> invalid operating mode", POLICY_SUBS(pPolicy));
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_operating_mode_condition_t,
                                        pPolicy,
                                        policyman_condition_ph_operating_mode_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  pCondition->oprt_mode = mode;

  *ppCondition = (mre_condition_t *) pCondition;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*-----------------------------------------------------------------------------
  Condition volte_enabled
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  cm_volte_enable_e_type volte_state;
} policyman_volte_enabled_condition_t;


/*-------- evaluate_volte_enabled --------*/
static boolean evaluate_volte_enabled(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_volte_enabled_condition_t const *pCond = (policyman_volte_enabled_condition_t *)pCondition;
  policy_execute_ctx_t                      *pPrefCtx = (policy_execute_ctx_t *)pCtx;
  cm_volte_enable_e_type                     volte_state;
  boolean                                    result;

  volte_state = policyman_ph_get_volte_enabled_state(pPrefCtx->pState, subsId);
  
  /* If state attribute is present then check for
   */
  if (pCond->volte_state == CM_VOLTE_NO_CHANGE)
  {
    result = (volte_state == CM_VOLTE_NO_CHANGE);
  }
  else
  {
    result  = (volte_state == CM_VOLTE_ENABLED);
  }

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <volte_enabled pmsubs=%d> returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        result );
  return result;
}


/*-------- policyman_condition_volte_enabled_evaluate --------*/
STATIC boolean policyman_condition_volte_enabled_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;
  return policyman_evaluate_cond(pCond, pCtx, evaluate_volte_enabled);
}


/*-------- policyman_condition_volte_enabled_new --------*/
mre_status_t policyman_condition_volte_enabled_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
)
{
  mre_status_t                          status     = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_volte_enabled_condition_t  *pCondition;
  char const                           *pStr;
  policyman_subs_t                      pm_subs;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_volte_enabled_condition_t,
                                        pPolicy,
                                        policyman_condition_volte_enabled_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );


  /* Does this condition indicate the state attribute
        Default is VoLTE disabled
   */
  pStr = policyman_xml_get_attribute(pElem, "state");

  if (pStr != NULL)
  {
    pCondition->volte_state = CM_VOLTE_NO_CHANGE;
  }
  else
  {
    pCondition->volte_state = CM_VOLTE_DISABLED;
  }

  *ppCondition = (mre_condition_t *)pCondition;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*-----------------------------------------------------------------------------
  User MCC preference
-----------------------------------------------------------------------------*/

/*-------- evaluate_user_mcc --------*/
static boolean evaluate_user_mcc(
  policyman_condition_t const  *pCondition,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
  )
{
  sys_modem_as_id_e_type          asubs_id = CONDITION_SUBS(pCondition);
  policy_execute_ctx_t            *pPhCtx   = (policy_execute_ctx_t *)pCtx;
  sys_plmn_id_s_type               user_plmn;
  policyman_mcc_condition_t       *pCond = (policyman_mcc_condition_t *) pCondition;
  uint32                          userMcc = 0;
  boolean                         result = FALSE;

  /*  If we don't have a set but *do* have a set name, try to find the named set.
   */
  if (pCond->pMccSet == NULL && pCond->pSetName != NULL)
  {
    pCond->pMccSet = (policyman_set_t *) mre_named_object_find( pCond->pSetName,
                            POLICYMAN_NAMED_MCC_SET,
                            pCond->pPolicy  );

    if (NULL != pCond->pMccSet)
    {
      ref_cnt_obj_add_ref(pCond->pMccSet);
    }
  }

  POLICYMAN_MEM_FREE_IF(pCond->pSetName);

  /*  Without a set, we can't work.
   */
  if (pCond->pMccSet == NULL)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "named PLMN set not found", CONDITION_SUBS(pCond));
    goto Done;
  }

  /*  Check to see if some stack's MCC is in the list.
   */
  user_plmn = policyman_ph_get_user_plmn(pPhCtx->pState, asubs_id);
  userMcc = policyman_plmn_get_mcc(&user_plmn);

  if (policyman_set_contains(pCond->pMccSet, &userMcc))
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "User selected MCC %d is in the MCC set",
                          CONDITION_SUBS(pCond),
                          userMcc );
    result = TRUE;
    goto Done;
  }
  else
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "User selected MCC %d not found in set",
                          CONDITION_SUBS(pCond),
                          userMcc );
  }

Done:
  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <serving_mcc_in> with user MCC %d returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        userMcc, 
                        result);
  return result;
}

/*-------- policyman_condition_user_mcc_evaluate --------*/
static boolean policyman_condition_user_mcc_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_user_mcc);
}

/*-------- policyman_condition_ph_user_mcc_new --------*/
mre_status_t policyman_condition_ph_user_mcc_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  return  policyman_plmn_mcc_new( pElem,
                                  pPolicy,
                                  ppCondition,
                                  policyman_condition_user_mcc_evaluate                                );

}

/*-----------------------------------------------------------------------------
  Network Selection mode pref
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  cm_network_sel_mode_pref_e_type user_selection_mode;
} policyman_user_selection_mode_condition_t;


/*-------- policyman_str_to_user_selection_mode --------*/
cm_network_sel_mode_pref_e_type policyman_str_to_user_selection_mode(
  char const  *pStr
  )
{
  typedef struct
  {
    const char                           *pStr;
    cm_network_sel_mode_pref_e_type      user_selection_mode;
  } operating_mode_map;

  cm_network_sel_mode_pref_e_type user_selection_mode = CM_NETWORK_SEL_MODE_PREF_NONE;

  static operating_mode_map map[] =
  {
    {"MANUAL",     CM_NETWORK_SEL_MODE_PREF_MANUAL},
    {"AUTOMATIC",  CM_NETWORK_SEL_MODE_PREF_AUTOMATIC},
  };

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      user_selection_mode = map[i].user_selection_mode;
      break;
    }
  }

  if (user_selection_mode == CM_NETWORK_SEL_MODE_PREF_NONE)
  {
    POLICYMAN_UT_MSG_1("Invalid value for <phone_user_selection_mode>: %s.  Valid values are:", token);
    for (i = 0 ; i < ARR_SIZE(map) ; ++i)
    {
      POLICYMAN_UT_MSG_1("    %s", map[i].pStr);
    }
  }

  return user_selection_mode;
}


/*-------- evaluate_phone_user_selection_mode --------*/
static boolean evaluate_phone_user_selection_mode(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_user_selection_mode_condition_t const *pCond = (policyman_user_selection_mode_condition_t *) pCondition;
  policy_execute_ctx_t                            *pPhCtx = (policy_execute_ctx_t *)pCtx;
  cm_network_sel_mode_pref_e_type                  net_sel_mode_pref;
  boolean                                          result;

  net_sel_mode_pref = policyman_ph_get_net_sel_mode_pref(pPhCtx->pState, subsId);
  result = pCond->user_selection_mode == net_sel_mode_pref;

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX SRC_LINE "condition <phone_user_selection_mode pmsubs=%d> with user_sel %d returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        net_sel_mode_pref,
                        result );
  return result;
}

/*-------- policyman_condition_ph_network_selection_mode_evaluate --------*/
static boolean policyman_condition_ph_network_selection_mode_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_phone_user_selection_mode);
}

/*-------- policyman_condition_ph_network_selection_mode_new --------*/
mre_status_t policyman_condition_ph_network_selection_mode_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                              status = MRE_STATUS_ERR_MALFORMED_XML;
  cm_network_sel_mode_pref_e_type           user_selection_mode;
  policyman_user_selection_mode_condition_t *pCondition;
  char const                                *pStr;
  policyman_subs_t                          pm_subs;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    goto Done;
  }

  /* Get a valid value
    */
  user_selection_mode = policyman_str_to_user_selection_mode(pStr);

  if(user_selection_mode == CM_NETWORK_SEL_MODE_PREF_NONE)
  {
    goto Done;
  }

  /* Does this action indicate the subscription
    *  Default is subscription from policy
    */
   status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

   if (MRE_FAILED(status))
   {
     goto Done;
   }

  /* Create the condition
    */
  pCondition = POLICYMAN_CONDITION_NEW( policyman_user_selection_mode_condition_t,
                                        pPolicy,
                                        policyman_condition_ph_network_selection_mode_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

   /* Copy value to condition
     */
   pCondition->user_selection_mode = user_selection_mode;

  *ppCondition = (mre_condition_t *)pCondition;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*===========================================================================

FUNCTION policyman_map_mode_pref_nv2cm

DESCRIPTION
  Translates NV enum to CM enum for mode preference

DEPENDENCIES
  none

RETURN VALUE
  CM enum

SIDE EFFECTS
  none

===========================================================================*/
static cm_mode_pref_e_type
policyman_map_mode_pref_nv2cm(
  nv_mode_enum_type  nv_mode
  )
{

  switch (nv_mode)
  {
    case NV_MODE_DIGITAL_ONLY:
      return CM_MODE_PREF_DIGITAL_ONLY;

    case NV_MODE_ANALOG_ONLY:
      return CM_MODE_PREF_AMPS_ONLY;

    case NV_MODE_AUTOMATIC:
      return CM_MODE_PREF_AUTOMATIC;

    case NV_MODE_E911:
      return CM_MODE_PREF_EMERGENCY;

    case NV_MODE_CDMA_ONLY:
      return CM_MODE_PREF_CDMA_ONLY;

    case NV_MODE_HDR_ONLY:
      return CM_MODE_PREF_HDR_ONLY;

    case NV_MODE_CDMA_AMPS_ONLY:
      return CM_MODE_PREF_CDMA_AMPS_ONLY;

    case NV_MODE_GPS_ONLY:
      return CM_MODE_PREF_GPS_ONLY;

    case NV_MODE_GSM_ONLY:
      return CM_MODE_PREF_GSM_ONLY;

    case NV_MODE_WCDMA_ONLY:
      return CM_MODE_PREF_WCDMA_ONLY;

    case NV_MODE_ANY_BUT_HDR:
      return CM_MODE_PREF_ANY_BUT_HDR;

    case NV_MODE_GSM_WCDMA_ONLY:
      return CM_MODE_PREF_GSM_WCDMA_ONLY;

    case NV_MODE_DIGITAL_LESS_HDR_ONLY:
      return CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY;

    case NV_MODE_CDMA_HDR_ONLY:
      return CM_MODE_PREF_CDMA_HDR_ONLY;

    case NV_MODE_CDMA_AMPS_HDR_ONLY:
      return CM_MODE_PREF_CDMA_AMPS_HDR_ONLY;

    case NV_MODE_LTE_ONLY:
      return CM_MODE_PREF_LTE_ONLY;

    case NV_MODE_GWL:
      return CM_MODE_PREF_GWL;

    case NV_MODE_HDR_LTE_ONLY:
      return CM_MODE_PREF_HDR_LTE_ONLY;

    case NV_MODE_CDMA_HDR_LTE_ONLY:
      return CM_MODE_PREF_CDMA_HDR_LTE_ONLY;

    case NV_MODE_CDMA_HDR_GSM_WCDMA:
      return CM_MODE_PREF_CDMA_HDR_GW;

    case NV_MODE_CDMA_GSM_WCDMA:
      return CM_MODE_PREF_CDMA_GW;

    case NV_MODE_CDMA_LTE_ONLY:
      return CM_MODE_PREF_CDMA_LTE_ONLY;

    case NV_MODE_CDMA_HDR_GSM:
      return CM_MODE_PREF_CDMA_HDR_GSM;

    case NV_MODE_CDMA_GSM:
      return CM_MODE_PREF_CDMA_GSM;

    case NV_MODE_GSM_LTE_ONLY:
      return CM_MODE_PREF_GSM_LTE;

    case NV_MODE_CDMA_GSM_LTE_ONLY:
      return CM_MODE_PREF_CDMA_GSM_LTE;

    case NV_MODE_HDR_GSM_LTE_ONLY:
      return CM_MODE_PREF_HDR_GSM_LTE;

    case NV_MODE_WCDMA_LTE_ONLY:
      return CM_MODE_PREF_WCDMA_LTE;

    case NV_MODE_CDMA_WCDMA_LTE_ONLY:
      return CM_MODE_PREF_CDMA_WCDMA_LTE;

    case NV_MODE_HDR_WCDMA_LTE_ONLY:
      return CM_MODE_PREF_HDR_WCDMA_LTE;

    case NV_MODE_TDS_ONLY:
      return CM_MODE_PREF_TDS_ONLY;

    case NV_MODE_TDS_GSM:
      return CM_MODE_PREF_TDS_GSM;

    case NV_MODE_TDS_GSM_LTE:
      return CM_MODE_PREF_TDS_GSM_LTE;

    case NV_MODE_TDS_LTE:
      return CM_MODE_PREF_TDS_LTE;

    case NV_MODE_TDS_GSM_WCDMA_LTE:
      return CM_MODE_PREF_TDS_GSM_WCDMA_LTE;

    case NV_MODE_TDS_GSM_WCDMA:
      return CM_MODE_PREF_TDS_GSM_WCDMA;

    case NV_MODE_CDMA_HDR_GSM_WCDMA_LTE:
       return CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE;

    case NV_MODE_CDMA_GSM_WCDMA_LTE:
       return CM_MODE_PREF_CDMA_GSM_WCDMA_LTE;

    case NV_MODE_TDS_WCDMA:
       return CM_MODE_PREF_TDS_WCDMA;

    case NV_MODE_TDS_WCDMA_LTE:
        return CM_MODE_PREF_TDS_WCDMA_LTE;

    case NV_MODE_CDMA_HDR_GSM_TDS_LTE:
       return CM_MODE_PREF_CDMA_HDR_GSM_TDS_LTE;

    default:
      POLICYMAN_MSG_ERROR_1("Invalid NV MODE %d, return AUTO",nv_mode);
      return CM_MODE_PREF_AUTOMATIC;
  }
} /* policyman_map_mode_pref_nv2cm() */

/*===========================================================================

FUNCTION policyman_map_srv_domain_pref_nv2cm

DESCRIPTION
  Translates NV enum to CM enum for service domain preference

DEPENDENCIES
  none

RETURN VALUE
  CM enum

SIDE EFFECTS
  none

===========================================================================*/
static cm_srv_domain_pref_e_type
policyman_map_srv_domain_pref_nv2cm(
  nv_srv_domain_pref_enum_type   nv_mode
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (nv_mode)
  {
    case NV_SRV_DOMAIN_PREF_CS_ONLY: return CM_SRV_DOMAIN_PREF_CS_ONLY;

    case NV_SRV_DOMAIN_PREF_PS_ONLY: return CM_SRV_DOMAIN_PREF_PS_ONLY;

    case NV_SRV_DOMAIN_PREF_CS_PS:   return CM_SRV_DOMAIN_PREF_CS_PS;

    case NV_SRV_DOMAIN_PREF_ANY:     return CM_SRV_DOMAIN_PREF_ANY;

    default:                         return ( cm_srv_domain_pref_e_type ) nv_mode;
  }

} /* policyman_map_srv_domain_pref_nv2cm */


/*=============================================================================
===============================================================================

  Preference handling

===============================================================================
=============================================================================*/

typedef struct
{
  POLICYMAN_CMD_HDR;

  sys_modem_as_id_e_type               asubs_id;
  cm_sys_sel_pref_params_s_type       *pPref;
  uint32                               timestamp;
} policyman_cmd_report_pref_chg_t;

/*-------- policyman_pref_update_needs_policy_run --------*/
STATIC boolean policyman_pref_update_needs_policy_run(
   policyman_cmph_info_t               *pPhState,
   cm_sys_sel_pref_params_s_type const *pPref
)
{
  boolean                    doUpdate = FALSE;
  cm_mode_pref_e_type        prev_mode_pref;
  cm_srv_domain_pref_e_type  prev_srv_domain_pref;
  cm_volte_enable_e_type     prev_volte_state;

  // Save originals for logging
  prev_mode_pref       = pPhState->mode_pref;
  prev_srv_domain_pref = pPhState->srv_domain_pref;
  prev_volte_state     = pPhState->volte_state;

  /* Compare internal states to new incoming states.
   *
   * CM ensures that actual values are sent and will not send
   * any NO_CHANGE values in this interface (the volte enabled
   * status is a special case where NO_CHANGE indicates undetermined).
   */

  if (pPhState->mode_pref != pPref->mode_pref)
  {
    pPhState->mode_pref = pPref->mode_pref;
    doUpdate = TRUE;
    pPhState->changed_fields |= PM_CMPH_CHANGED_MODE_PREF;
  }

  if (pPhState->srv_domain_pref != pPref->srv_domain_pref)
  {
    pPhState->srv_domain_pref = pPref->srv_domain_pref;
    doUpdate = TRUE;
    pPhState->changed_fields |= PM_CMPH_CHANGED_SRV_DOMAIN_PREF;
  }

  /* Check for MANUAL mode if PLMN is valid and then only run rules
       otherwise reset to AUTOMATIC
    */
  if (pPref->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    if (sys_plmn_id_is_valid(*(pPref->plmn_ptr)))
    {
      pPhState->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_MANUAL;
      pPhState->user_plmn = *pPref->plmn_ptr;
      POLICYMAN_MSG_PLMN_1( SUBS_PREFIX "pref_update: Network selection = MANUAL with new PLMN: ",
                            pPhState->asubs_id,
                            pPhState->user_plmn );
      doUpdate = TRUE;
      pPhState->changed_fields |= PM_CMPH_CHANGED_NETWORK_SEL_MODE_PREF;
      pPhState->changed_fields |= PM_CMPH_CHANGED_USER_PLMN;
    }
    else
    {
      pPhState->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
      policyman_plmn_clear(&pPhState->user_plmn);
      POLICYMAN_MSG_ERROR_1( SUBS_PREFIX "pref_update: Network selection = MANUAL with invalid PLMN",
                             pPhState->asubs_id );
    }
  }
  else
  {
    pPhState->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
    policyman_plmn_clear(&pPhState->user_plmn);
  }

  /* Handle volte enable/disable pref
   * CM sends NO_CHANGE to indicate undetermined state at startup
   * until AP side indicates the actual state.
   */
   if (  pPref->volte_enable_state != CM_VOLTE_NO_CHANGE
       && pPhState->volte_state != pPref->volte_enable_state )
   {
     pPhState->volte_state = pPref->volte_enable_state;
     doUpdate = TRUE;
     pPhState->changed_fields |= PM_CMPH_CHANGED_VOLTE_STATE;
   }

  POLICYMAN_MSG_HIGH_8( SUBS_PREFIX "pref_update: changed_fields 0x%04x, mode_pref %d->%d, srv_domain_pref %d->%d, volte_enabled %d->%d",
                        pPhState->asubs_id,
                        pPhState->changed_fields,
                        prev_mode_pref,
                        pPhState->mode_pref,
                        prev_srv_domain_pref,
                        pPhState->srv_domain_pref,
                        prev_volte_state,
                        pPhState->volte_state );

  return doUpdate;
}


/*-------- policyman_report_preference_execute --------*/
STATIC void policyman_report_preference_execute(
  policyman_cmd_t *pCmd
)
{
  cm_sys_sel_pref_params_s_type const *pPref;
  policyman_cmd_report_pref_chg_t     *pPrefCmd;
  policyman_cmph_info_t               *pPhState;
  boolean                              needs_policy_run;

  pPrefCmd = (policyman_cmd_report_pref_chg_t *) pCmd;
  pPref = pPrefCmd->pPref;

  pPhState = policyman_state_get_cm_ph_state(pPrefCmd->pState, pPrefCmd->asubs_id);

  POLICYMAN_MSG_HIGH_5( LOG_SUBS_PREFIX "executing pref change command with mode_pref %d, srv_domain_pref %d, volte_enabled %d",
                        LOG_SUBS(pPrefCmd->asubs_id),
                        pPrefCmd->asubs_id,
                        pPref->mode_pref,
                        pPref->srv_domain_pref,
                        pPref->volte_enable_state );

  policyman_state_enter_crit_section();
  needs_policy_run = policyman_pref_update_needs_policy_run(pPhState, pPref);
  policyman_state_leave_crit_section();

  if (needs_policy_run)
  {
    policyman_cmph_update_history( pPrefCmd->pState,
                                   CM_PH_EVENT_SYS_SEL_PREF,
                                   pPhState,
                                   pPrefCmd->timestamp ); // History logged only when changed

    policyman_subs_update_multimode_subs(pCmd);
  }

  policyman_mem_free(pPrefCmd->pPref) ;
}


/*-------- policyman_compute_policy_execute --------*/
STATIC void policyman_compute_policy_execute(
  policyman_cmd_t *pCmd
)
{
  cm_sys_sel_pref_params_s_type const *pPref;
  policyman_cmd_report_pref_chg_t     *pPrefCmd;
  policyman_set_t                     *pItemSet;
  policyman_state_t                   *pState;
  policyman_cmph_info_t               *pPhInfo;

  pPrefCmd   = (policyman_cmd_report_pref_chg_t *) pCmd;
  pPref      = pPrefCmd->pPref;
  pCmd->pMsg = NULL;
  pItemSet   = policyman_itemset_new();

  pState     = policyman_state_clone();
  pPhInfo    = policyman_state_get_cm_ph_state(pState, pPrefCmd->asubs_id);

  POLICYMAN_MSG_HIGH_5( LOG_SUBS_PREFIX "executing compute policy command with mode_pref %d, srv_domain_pref %d, volte_enabled %d",
                        LOG_SUBS(pPrefCmd->asubs_id),
                        pPrefCmd->asubs_id,
                        pPref->mode_pref,
                        pPref->srv_domain_pref,
                        pPref->volte_enable_state );

  /* Run a policy check if needed. We should return a non-NULL message
        in case preference is accepted and no policy change is done. In such
        scenarios MSG num_attach should be set to 0.
   */
  if (policyman_pref_update_needs_policy_run(pPhInfo, pPref))
  {
    policyman_policies_run_policy_check(pState, pItemSet);
  }

  pCmd->pMsg = policyman_cfgitem_get_change_msg_hdr(pItemSet);

  policyman_mem_free(pPrefCmd->pPref);
  policyman_state_delete_clone(pState);
  REF_CNT_OBJ_RELEASE_IF(pItemSet);

  /*  Unblock the thread that issued the command.
   */
  policyman_client_block_signal_set(pCmd->signal);
}

/*-------- policyman_report_prefs_have_changed --------*/
STATIC boolean policyman_report_prefs_have_changed(
  policyman_state_t                   *pState,
  sys_modem_as_id_e_type               asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
)
{
  boolean                prefsHaveChanged;
  policyman_cmph_info_t *pInfo;

  pInfo = policyman_state_get_cm_ph_state(pState, asubs_id);

  prefsHaveChanged = (   (pInfo->mode_pref != pPref->mode_pref)
                      || (pInfo->srv_domain_pref != pPref->srv_domain_pref)
                      || (pInfo->network_sel_mode_pref != pPref->network_sel_mode_pref)
                      || (    (pPref->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
                           && !policyman_plmns_are_equal( (sys_plmn_id_s_type *)pPref->plmn_ptr, &pInfo->user_plmn ) )
                      || (pInfo->volte_state != pPref->volte_enable_state) );

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "policyman_report_prefs_have_changed() returns %d",
                        asubs_id,
                        prefsHaveChanged );

  return prefsHaveChanged;
}


/*-------- policyman_block_for_pref_change --------*/
static boolean policyman_block_for_pref_change(
  policyman_state_t                   *pState,
  sys_modem_as_id_e_type              asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
  )
{

  /*  Block the caller of policyman_report_preference_change() if:
   *    - We're not initializing
   *    - and there are changes in preferences that we care about
   */
  return (   !policyman_is_initializing()
          && policyman_report_prefs_have_changed(pState, asubs_id, pPref) );
}


/*-------- policyman_report_preference_change --------*/
msgr_hdr_s *
policyman_report_preference_change(
  sys_modem_as_id_e_type              asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
  )
{
  policyman_cmd_report_pref_chg_t *pCmd;
  msgr_hdr_s                      *pMsg = NULL;
  policyman_state_t               *pState;

  pState = policyman_state_get_state();

  /*  If we aren't going to block to process this request, just return.
   */
  if (!policyman_block_for_pref_change(pState, asubs_id, pPref))
  {
    POLICYMAN_MSG_ERROR_2( SUBS_PREFIX "not blocking caller (thread 0x%x) of policyman_report_preference_change()",
                           asubs_id,
                           POLICYMAN_CALLER_THREAD_ID );
    goto Done;
  }

  /*  If we should block the calling thread to do this update, get a signal on
   *  which to block for command completion, and put it in the command.
   *  Queue the command and wait for completion.
   */
  pCmd = POLICYMAN_CMD_WITH_HOOK_NEW( policyman_cmd_report_pref_chg_t,
                                      policyman_report_preference_execute,
                                      NULL,
                                      NULL,
                                      policyman_cfgitem_update_notification_hook );

  pCmd->asubs_id  = asubs_id;
  pCmd->timestamp = mre_util_get_time();

  pCmd->pPref = (cm_sys_sel_pref_params_s_type *) policyman_mem_alloc(sizeof(cm_sys_sel_pref_params_s_type));
  *pCmd->pPref = *pPref;

  pCmd->pMsg = NULL;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "blocking caller (thread 0x%x) of policyman_report_preference_change()",
                        asubs_id,
                        POLICYMAN_CALLER_THREAD_ID );

  pCmd->signal = policyman_client_block_sig_get();
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);

  /* Block the caller only when policy is not suspended
  */
  if (!policyman_is_suspended())
  {
    policyman_client_block_signal_wait(pCmd->signal);
  }

  /*  Get any message to be returned and then release the command.
   */
  pMsg = pCmd->pMsg;
  ref_cnt_obj_release(pCmd);

Done:
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "returning 0x%x to caller of policyman_report_preference_change()",
                        asubs_id,
                        pMsg );
  return pMsg;
}

/*-------- policyman_compute_policy_with_newpref --------*/
msgr_hdr_s * policyman_compute_policy_with_newpref(
  sys_modem_as_id_e_type              asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
  )
{
  policyman_cmd_report_pref_chg_t *pCmd;
  msgr_hdr_s                      *pMsg = NULL;

  /*  If we aren't going to block to process this request, just return.
   */
  if (policyman_is_initializing())
  {
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "Policyman is initializing, can't accept the preferences", asubs_id);
    goto Done;
  }

  /*  If we should block the calling thread to do this update, get a signal on
   *  which to block for command completion, and put it in the command.
   *  Queue the command and wait for completion.
   */
  pCmd = POLICYMAN_CMD_NEW( policyman_cmd_report_pref_chg_t,
                            policyman_compute_policy_execute,
                            NULL,
                            NULL );

  pCmd->asubs_id = asubs_id;
  pCmd->pPref    = (cm_sys_sel_pref_params_s_type *)policyman_mem_alloc(sizeof(cm_sys_sel_pref_params_s_type));
  *pCmd->pPref   = *pPref;

  pCmd->pMsg = NULL;
  POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "blocking caller of policyman_compute_policy_with_newpref", asubs_id );

  pCmd->signal = policyman_client_block_sig_get();
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);

  /* Block the caller only when policy is not suspended
  */
  if (!policyman_is_suspended())
  {
    policyman_client_block_signal_wait(pCmd->signal);
  }

  /*  Get any message to be returned and then release the command.
   */
  pMsg = pCmd->pMsg;
  ref_cnt_obj_release(pCmd);

Done:
  POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "returning to caller of policyman_report_preference_change()", asubs_id);
  return pMsg;
}


/*-------- policyman_report_preference_msg_free --------*/
void policyman_report_preference_msg_free(
  msgr_hdr_s  *pMsg
)
{
  policyman_report_msg_free(pMsg);
}

/*-------- policyman_event_get_cm_client_id --------*/
cm_client_id_type policyman_event_get_cm_client_id(
  void
)
{
  return policyman_event.cm_clnt_id;
}

/* ------ policyman_get_persisted_user_plmn ------*/
boolean policyman_get_persisted_user_plmn(
  sys_modem_as_id_e_type  subsId,
  boolean                 clear,
  sys_plmn_id_s_type     *pPlmn
)
{
  size_t                   plmnSize;
  sys_plmn_id_s_type      *pUserPlmn = NULL;
  mre_efs_status_t         efsStatus = MRE_EFS_STATUS_SUCCESS;

  efsStatus = mre_efs_get_file( POLICYMAN_USER_PLMN_NV_PATH,
                                (void **)&pUserPlmn,
                                &plmnSize,
                                subsId );

  if (efsStatus != MRE_EFS_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_2( SUBS_PREFIX "user_plmn EFS file not read, status %d",
                           subsId,
                           efsStatus );
    goto Done;
  }

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX "read user selected PLMN from EFS, status %d, PLMN size %d and clear %d",
                        subsId,
                        efsStatus,
                        plmnSize,
                        clear );

  *pPlmn  = *pUserPlmn;
  POLICYMAN_MSG_PLMN_0("PLMN from NV: ", *pUserPlmn);

  if (clear)
  {
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "removing user_plmn EFS file", subsId);
    (void)mre_efs_file_delete( POLICYMAN_USER_PLMN_NV_PATH,
                               subsId );
  }

Done:
  POLICYMAN_MEM_FREE_IF(pUserPlmn);

  return (efsStatus == MRE_EFS_STATUS_SUCCESS);
}

/*-------- policyman_update_persisted_user_plmn --------*/
void policyman_update_persisted_user_plmn(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  cm_network_sel_mode_pref_e_type     net_sel_mode;
  sys_plmn_id_s_type                  user_plmn;
  mre_efs_status_t                    efsStatus    = MRE_EFS_STATUS_SUCCESS;

  net_sel_mode = policyman_ph_get_net_sel_mode_pref(pState, subsId);

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "network selection mode is %d",
                        subsId,
                        net_sel_mode );

  if (net_sel_mode != CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    goto Done;
  }

  user_plmn    = policyman_ph_get_user_plmn(pState, subsId);

  efsStatus = mre_efs_put_file( POLICYMAN_USER_PLMN_NV_PATH,
                                (void *)&user_plmn,
                                sizeof(user_plmn),
                                FALSE,
                                subsId );

  if (efsStatus != MRE_EFS_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_2( SUBS_PREFIX "Failed to write user plmn EFS file, status %d",
                           subsId,
                           efsStatus );
    goto Done;
  }

  POLICYMAN_MSG_PLMN_0("User PLMN persisted: ", user_plmn);

Done:
  return;
}
