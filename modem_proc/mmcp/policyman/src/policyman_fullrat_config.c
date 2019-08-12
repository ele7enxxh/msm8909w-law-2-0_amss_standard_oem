/**
  @file policyman_fullrat_config.c

  @brief
*/

/*
    Copyright (c) 2015,2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_fullrat_config.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"
#include "mre_engine.h"
#include "mre_rules.h"
#include "mre_set.h"
#include "mre_xml.h"

#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_fullrat_config.h"
#include "policyman_phone_events.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_rules.h"
#include "policyman_serving_system.h"
#include "policyman_set.h"
#include "policyman_timer.h"

#include <stringl/stringl.h>

#define FULLRAT_TIMER_ID 1000
#define FULLRAT_TIMER_DEFAULT 120
#define ACQFAIL_CNT_DEFAULT 1

struct fullrat_config_t
{
  mre_policy_t           *pPolicy;
  policyman_timer_t      *pTimer;
  uint8                   scan_cnt;
  sys_modem_as_id_e_type  subsId;
  policyman_set_t        *pActionSet;
};

STATIC fullrat_config_t g_FullRatConfigs[POLICYMAN_NUM_SUBS];

/*-------- policyman_fullrat_config_find --------*/
STATIC fullrat_config_t *policyman_fullrat_config_find(
  sys_modem_as_id_e_type subsId
)
{
  return &g_FullRatConfigs[subsId];
}

/*-------- policyman_fullrat_config_init_per_subs --------*/
void policyman_fullrat_config_init_per_subs(
  sys_modem_as_id_e_type subsId
)
{
  fullrat_config_t *pFullRatCfg;

  pFullRatCfg = policyman_fullrat_config_find(subsId);

  pFullRatCfg->pTimer     = NULL;
  pFullRatCfg->scan_cnt   = ACQFAIL_CNT_DEFAULT;
  pFullRatCfg->subsId     = subsId;
  pFullRatCfg->pActionSet = NULL;
}


/*-------- policyman_fullrat_config_deinit_per_subs --------*/
void policyman_fullrat_config_deinit_per_subs(
  sys_modem_as_id_e_type subsId
)
{
  fullrat_config_t *pFullRatCfg;

  pFullRatCfg = policyman_fullrat_config_find(subsId);

  policyman_timer_stop(pFullRatCfg->pTimer);
  REF_CNT_OBJ_RELEASE_IF(pFullRatCfg->pTimer);
  REF_CNT_OBJ_RELEASE_IF(pFullRatCfg->pActionSet);
  
}

/*-------- policyman_fullrat_config_deinit --------*/
void policyman_fullrat_config_deinit(
  void
)
{
  size_t            subsIndex;

  for (subsIndex = 0;subsIndex < POLICYMAN_NUM_SUBS;subsIndex++)
  {
    policyman_fullrat_config_deinit_per_subs(subsIndex);
  }  
}

/*-------- policyman_scancnt_exceeded_config --------*/
STATIC boolean policyman_scancnt_exceeded_config(
  policyman_state_t *pState,
  fullrat_config_t *pFullRatCfg
)
{
  boolean limitWasReached = FALSE;
  
  if (pFullRatCfg->scan_cnt > 0)
  {
    limitWasReached = (policyman_ss_get_min_acq_fail_count(pState, pFullRatCfg->subsId) >= pFullRatCfg->scan_cnt);   
  }
  
  return limitWasReached;
}

/*-------- policyman_fullrat_conditions_are_met --------*/
STATIC boolean policyman_fullrat_conditions_are_met(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type subsId
)
{
  fullrat_config_t       *pFullRatCfg;
  boolean                 have_service;
  boolean                 timer_expired;
  boolean                 oos_scancnt_exceeded;
  boolean                 hysteresis_expired;
  boolean                 conditions_met;

  pFullRatCfg          = policyman_fullrat_config_find(subsId);
  have_service         = policyman_any_subs_has_service(pState);
  timer_expired        = policyman_timer_is_expired(pFullRatCfg->pTimer);
  oos_scancnt_exceeded = policyman_scancnt_exceeded_config(pState, pFullRatCfg);
  hysteresis_expired   = timer_expired || oos_scancnt_exceeded;

  /* Eligible to enter fullrat mode if no service and
     the timer expired or the oos scan count has exceeded threshold
   */
  conditions_met       = (!have_service && hysteresis_expired);

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX "fullrat conditions met: %d (have_service %d, timer_expired %d, oos_scancnt_exceeded %d)",
                        subsId,
                        conditions_met,
                        have_service,
                        timer_expired,
                        oos_scancnt_exceeded );

  return (conditions_met);
}

/*-------- policyman_fullrat_execute_actions --------*/
STATIC void policyman_fullrat_execute_actions(
  policy_execute_ctx_t *pCtx
)
{
  fullrat_config_t *pFullRatCfg;

  pFullRatCfg = policyman_fullrat_config_find(pCtx->asubs_id);
  if (NULL != pFullRatCfg && NULL != pFullRatCfg->pActionSet)
  {
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "entering Full Rat mode", pCtx->asubs_id);

    MRE_POLICY_DESCEND_LEVEL(pFullRatCfg->pPolicy);
    mre_actionset_execute(pFullRatCfg->pActionSet, pCtx);
    MRE_POLICY_ASCEND_LEVEL(pFullRatCfg->pPolicy);
  }
}

/*-------- policyman_get_fullrat_timer --------*/
policyman_timer_t *policyman_get_fullrat_timer(
  sys_modem_as_id_e_type  subsId
)
{
  fullrat_config_t *pFullRatCfg;

  pFullRatCfg = policyman_fullrat_config_find(subsId);

  return pFullRatCfg->pTimer;  
}

/*-------- policyman_fullrat_enter_check --------*/
STATIC void policyman_fullrat_enter_check(
  policy_execute_ctx_t  *pCtx
)
{
  if (policyman_fullrat_conditions_are_met(pCtx->pState, pCtx->asubs_id))
  {
    policyman_fullrat_execute_actions(pCtx);
  }
}

void policyman_fullrat_config_stop_timers(
	void
)
{
  policyman_timer_t      *pTimer;
  sys_modem_as_id_e_type  subsId;
  size_t                 numSim = 0;

  POLICYMAN_MSG_HIGH_0("Stopping all full RAT timers");

  (void)policyman_get_current_num_sim(&numSim);

  for (subsId = SYS_MODEM_AS_ID_1; subsId < numSim; ++subsId)
  {
    pTimer = policyman_get_fullrat_timer(subsId);
    policyman_timer_stop(pTimer);
  }
}

/*-------- policyman_fullrat_config_evaluate --------*/
void policyman_fullrat_config_evaluate(
  policy_execute_ctx_t   *pCtx
)
{
  size_t                 numSim          = 0;
  sys_modem_as_id_e_type subsId;

  (void)policyman_get_current_num_sim(&numSim);

  if (!policyman_ue_is_online(pCtx->pState))
  {
    POLICYMAN_MSG_HIGH_0("Not evaluating full RAT conditions; UE is not ONLINE");
    policyman_fullrat_config_stop_timers();
  }
  else
  {
    POLICYMAN_MSG_HIGH_0("Evaluating full RAT conditions on all subscriptions");
  for (subsId = SYS_MODEM_AS_ID_1; subsId < numSim; subsId++)
  {
    pCtx->asubs_id = subsId;
    policyman_fullrat_enter_check(pCtx);
  }
}
}

/*-------- policyman_fullrat_config_notify_service --------*/
void policyman_fullrat_config_notify_service(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_timer_t      *pTimer;
  boolean            haveLocation;

  pTimer = policyman_get_fullrat_timer(subsId);
  haveLocation = policyman_subs_has_location(pState, subsId);

  if (!haveLocation && policyman_ue_is_online(pState))
  {
    policyman_timer_start(pTimer);
  }
  else
  {
    policyman_fullrat_config_stop_timers();
  }
}

/*-------- policyman_fullrat_timer_expired --------*/
STATIC void policyman_fullrat_timer_expired(
  policyman_timer_t *pTimer
)
{
  policy_execute_ctx_t    ctx;
  sys_modem_as_id_e_type  subsId;

  subsId = policyman_timer_get_subs(pTimer);

  ctx.pState    = policyman_state_get_state();
  ctx.pItemSet  = policyman_itemset_new();
  ctx.asubs_id  = subsId;

  POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "policyman_fullrat_timer_expired", subsId);

  policyman_ss_set_subs_serving_info_validity(ctx.pState, subsId, FALSE);

  policyman_fullrat_enter_check(&ctx);

  policyman_cfgitem_update_items(ctx.pItemSet, NULL);
  ref_cnt_obj_release(ctx.pItemSet);

  policyman_timer_set_handled(pTimer);
}

/*=============================================================================
  <define_fullrat_config>
=============================================================================*/
typedef struct
{
  POLICYMAN_ACTION_BASE;

  timetick_type      timer_secs;
  uint8              scan_cnt;
  policyman_set_t   *pActionSet;
} fullrat_config_action_t;

/*-------- policyman_named_fullrat_action_dtor --------*/
STATIC void policyman_fullrat_action_dtor(
  void  *pObj
)
{
  fullrat_config_action_t  *pAction = (fullrat_config_action_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pAction->pActionSet);
  policyman_action_dtor(pAction);
}

/*-------- execute_fullrat_config --------*/
STATIC boolean execute_fullrat_config(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  fullrat_config_action_t  *pFRAction   = (fullrat_config_action_t *)pAction;
  policyman_policy_t       *pPolicy   = (policyman_policy_t *)pFRAction->pPolicy;
  fullrat_config_t         *pFullRatCfg;
  policyman_timer_t        *pTimer;

  POLICYMAN_MSG_HIGH_1(SRC_LINE "executing <define_full_rat_config>", LINE_NUM(pFRAction));

  pFullRatCfg = policyman_fullrat_config_find(pPolicy->subs);

  /* release Timer and actionset in full rat config if already exists.
        we are now going to update with recently set configuration */
  policyman_fullrat_config_deinit_per_subs(pPolicy->subs);

  pTimer = policyman_timer_create_fixed_timer( NULL,
                                               pPolicy->subs,
                                               (mre_policy_t *)pPolicy,
                                               FULLRAT_TIMER_ID,
                                               pFRAction->timer_secs,
                                               policyman_fullrat_timer_expired );

  pFullRatCfg->pPolicy    = pFRAction->pPolicy;
  pFullRatCfg->pTimer     = pTimer;
  pFullRatCfg->scan_cnt   = pFRAction->scan_cnt;
  pFullRatCfg->pActionSet = pFRAction->pActionSet;
  ref_cnt_obj_add_ref(pFullRatCfg->pActionSet);

  return TRUE;
}

/*-------- policyman_fullrat_config_execute --------*/
STATIC boolean policyman_fullrat_config_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_fullrat_config);
}

/*-------- policyman_fullrat_get_actions --------*/
STATIC void policyman_fullrat_get_actions(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_set_t                     **ppActionSet
)
{
  mre_xml_element_t const *pChild;

  // Expecting only one child in full rat config
  pChild = mre_xml_get_child(pElem, 0);

  // The child should be actions
  if (mre_xml_tag_is(pChild, "actions"))
  {
    mre_rule_get_actionset(pChild, pPolicy, ppActionSet);
  }
}

/*-------- policyman_fullrat_config_new --------*/
mre_status_t policyman_fullrat_config_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  fullrat_config_action_t    *pAction;
  char const                 *pStr;

  pAction = POLICYMAN_ACTION_NEW( fullrat_config_action_t,
                                  pPolicy,
                                  policyman_fullrat_config_execute,
                                  policyman_fullrat_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pStr = mre_xml_get_attribute(pElem, "timer_secs");

  if (NULL != pStr)
  {
    pAction->timer_secs = atoi(pStr);
  }

  if (0 == pAction->timer_secs)
  {
    pAction->timer_secs = FULLRAT_TIMER_DEFAULT;
  }

  pStr = mre_xml_get_attribute(pElem, "scan_fail_cnt");

  if (NULL != pStr)
  {
    pAction->scan_cnt = atoi(pStr);
  }
  else
  {
    pAction->scan_cnt = ACQFAIL_CNT_DEFAULT;
  }

  policyman_fullrat_get_actions(pElem, (mre_policy_t *)pPolicy, &pAction->pActionSet);

  *ppAction = (mre_action_t *)pAction;

  return MRE_STATUS_SUCCESS;
}

/*=============================================================================
  <enter_fullrat>
=============================================================================*/
/*-------- execute_fullrat_enter --------*/
STATIC boolean execute_fullrat_enter(
  policyman_action_t const      *pAction,
  void                          *pCtx,
  sys_modem_as_id_e_type         subsId
)
{
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "executing <fullrat_enter>",
                        ACTION_SUBS(pAction),
                        LINE_NUM(pAction) );
  policyman_fullrat_execute_actions(pCtx);

  return TRUE;
}

/*-------- policyman_fullrat_enter_execute --------*/
STATIC boolean policyman_fullrat_enter_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_fullrat_enter);
}

/*-------- policyman_fullrat_enter_new --------*/
mre_status_t policyman_fullrat_enter_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t                 **ppAction
)
{
  policyman_action_t  *pAction;

  pAction = POLICYMAN_ACTION_NEW( policyman_action_t,
                                  pPolicy,
                                  policyman_fullrat_enter_execute,
                                  policyman_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  *ppAction = (mre_action_t *)pAction;

  return MRE_STATUS_SUCCESS;
}
