/**
  @file policyman_refresh.c

  @brief  Handling for PM refresh on SIM swap
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_refresh.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"
#include "policyman_refresh.h"
#include "policyman_task.h"
#include "policyman_policies.h"
#include "policyman_fullrat_config.h"
#include "policyman_device_config.h"


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*=============================================================================
  MCFG Refresh Event callback definition.
=============================================================================*/
typedef struct
{
  POLICYMAN_CMD_HDR;

  mcfg_refresh_info_s_type info;
} policyman_refresh_evt_cb_t;


/*-------- policyman_resume_after_refresh --------*/
void policyman_resume_after_refresh(
  policyman_set_t     *pItemSet,
  policyman_state_t   *pState
)
{
  size_t           numClients;

  numClients = policyman_resume_internal(POLICYMAN_CLIENT_POLICYMAN);

  if (numClients == 0)
  {
    policyman_policies_run_policy_check(pState, pItemSet);
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_EXECUTE_PROCESS_MCFG_REFRESH_EVT()

  DESCRIPTION
    Execute mcfg sim refresh event

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void policyman_execute_process_mcfg_refresh_evt(
  policyman_cmd_t *pCmd
)
{
  policyman_refresh_evt_cb_t *refresh_evt  = (policyman_refresh_evt_cb_t *) pCmd;
  mcfg_refresh_info_s_type    refresh_info = refresh_evt->info;
  policyman_state_t          *pState;
  policyman_set_t            *pItemSet;
  policyman_cmph_info_t      *pCmPhInfo;
  size_t                      subs;

  if (policyman_is_test_mode())
  {
    mcfg_refresh_done(refresh_info.reference_id);
    POLICYMAN_MSG_MED_0("Policyman in test mode, no refresh action needed. Refresh done sent to MCFG.");
    return;
  }

  pItemSet  = policyman_itemset_new(); 
  subs      = refresh_info.sub_index;

  policyman_suspend_by_client(POLICYMAN_CLIENT_POLICYMAN);
  policyman_state_enter_crit_section();

  // Clear out cfgitem and per-subs state information
  policyman_per_subs_deinit(subs);

  // Restart policyman info -- re-read everything due to a potential new xmls
  pState = policyman_state_get_state();
  
  pCmPhInfo = policyman_state_get_cm_ph_state(pState, subs);
  policyman_cmph_nv_init(&pCmPhInfo, subs);

  policyman_fullrat_config_init_per_subs(subs);

  policyman_device_config_reset_max_active_data(pItemSet);

  // re-read policy for subs
  policyman_policies_per_subs_init(pState, pItemSet, subs);
  policyman_cfgitem_ensure_items_per_subs(pItemSet, subs);  
  policyman_set_policy_initialized(TRUE);

  policyman_state_leave_crit_section();

  // let mcfg know were done
  if ( mcfg_refresh_done(refresh_info.reference_id) )
  {
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "Refresh done notification sent to MCFG", refresh_info.sub_index);
  }
  else
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "Refresh done notification to MCFG failed", refresh_info.sub_index);
  }

  policyman_resume_after_refresh(pItemSet, pState);
  policyman_dbupdate_enter_idle_state();

  policyman_cfgitem_update_device_config_for_force_notification(pItemSet);
  policyman_cfgitem_update_items(pItemSet, NULL);

  REF_CNT_OBJ_RELEASE_IF(pItemSet);
}

/*===========================================================================
  FUNCTION POLICYMAN_MCFG_REFRESH_CB()

  DESCRIPTION
    Handle MCFG SIM refresh callback

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
boolean policyman_mcfg_refresh_cb(
  mcfg_refresh_info_s_type *p_info
)
{
  policyman_refresh_evt_cb_t *pCmd = NULL;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "received mcfg sim refresh for sim/slot %d",
                        p_info->sub_index,
                        p_info->slot_index);

  pCmd = POLICYMAN_CMD_NEW( policyman_refresh_evt_cb_t,
                            policyman_execute_process_mcfg_refresh_evt,
                            NULL,
                            NULL ); // No policy associated with CM Phone Event

  pCmd->info = *p_info;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);

  return TRUE;
}


/*------------------------------------------------------------------------
  FUNCTION POLICYMAN_REGISTER_FOR_MCFG_REFRESH()

  DESCRIPTION
    With this function, Policy Manager registers with MCFG refresh events as a client
--------------------------------------------------------------------------*/
void policyman_register_for_mcfg_refresh(
  void
)
{
  mcfg_refresh_registration_s_type  reg;
  boolean                           success = FALSE;

  reg.order     = MCFG_REFRESH_ORDER_30;
  reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  reg.sub_mask  = MCFG_REFRESH_INDEX_MASK_ALL;
  reg.cb        = policyman_mcfg_refresh_cb;

  success = mcfg_refresh_register(&reg);

  if (!success)
  {
    POLICYMAN_MSG_ERROR_0("mcfg_refresh_register failed");
  }
}
#endif
