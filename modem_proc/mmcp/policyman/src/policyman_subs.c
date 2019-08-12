/**
  @file policyman_subs.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_subs.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/


#include "mre_efs.h"
#include "mre_set.h"
#include "policyman_cfgitem.h"
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_rat_capability.h"
#include "policyman_rules.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include "sys.h"
#include <stringl/stringl.h>

/*=============================================================================
 Data Structure for Subs info
==============================================================================*/
struct policyman_subs_info_s
{
  size_t                  num_subs;               // Count of active subscriptions.
  uint8                   active_subs_mask;       // Mask of currently active subscriptions.
  sys_modem_as_id_e_type  ddsSubs;                // Current subscriptions which is DDS
  boolean                 ddsChangeisPermanent;   // DDS change is permanent
};

/*=============================================================================
  APIs for Subs information
=============================================================================*/
boolean policyman_subs_init(
  policyman_subs_info_t **ppInfo
)
{
  nv_stat_enum_type      nv_status;
  nv_item_type           nv_item;
  sys_modem_as_id_e_type ddsSubs = SYS_MODEM_AS_ID_1;

  *ppInfo = (policyman_subs_info_t *) policyman_mem_alloc(sizeof(policyman_subs_info_t));

  /* update subs state
   */
 (*ppInfo)->num_subs = 0;
 (*ppInfo)->active_subs_mask = SYS_MODEM_AS_ID_NO_ACTIVE_MASK;

  /* Read DDS SUBS from context 0 and initialize */
  nv_status = mre_efs_get_nv_item( NV_DUAL_STANDBY_CFG_ITEMS_I,
                                   &nv_item,
                                   SYS_MODEM_AS_ID_1);

  if (nv_status == NV_DONE_S)
  {
    ddsSubs = (sys_modem_as_id_e_type)nv_item.dual_standby_cfg_items.dedicated_data_subs;
    POLICYMAN_MSG_HIGH_2("Read DDS NV: DDS subs %d", nv_status, ddsSubs);
  }
  else
  {
    POLICYMAN_MSG_ERROR_1("DDS NV read failed status %d, default DDS subs is AS_ID_1", nv_status);
  }

  (*ppInfo)->ddsSubs = ddsSubs;

  return TRUE;
}

/*-------- policyman_subs_deinit --------*/
void policyman_subs_deinit(
  policyman_subs_info_t *pInfo
)
{
  POLICYMAN_MEM_FREE_IF(pInfo);
}

/*-------- policyman_subs_info_deep_copy --------*/
policyman_subs_info_t * policyman_subs_info_deep_copy(
  policyman_subs_info_t  *pSubsState
)
{
  policyman_subs_info_t *pClone;

  pClone = (policyman_subs_info_t *) policyman_mem_alloc(sizeof(policyman_subs_info_t));

  *pClone = *pSubsState;
  return pClone;
}

/*-------- policyman_subs_get_num_subs --------*/
size_t policyman_subs_get_num_subs(
  policyman_state_t *pState
)
{
  policyman_subs_info_t *pSubsInfo;

  pSubsInfo = policyman_state_get_subs_info(pState);

  return (pSubsInfo != NULL) ? pSubsInfo->num_subs : 0;
}


/*-------- policyman_subs_set_num_subs --------*/
STATIC void policyman_subs_set_num_subs(
  policyman_state_t *pState,
  size_t             num_subs
)
{
  policyman_subs_info_t *pSubsInfo;

  pSubsInfo = policyman_state_get_subs_info(pState);

  pSubsInfo->num_subs = num_subs;
}


/*-------- policyman_subs_get_active_subs_mask --------*/
STATIC uint8 policyman_subs_get_active_subs_mask(
  policyman_state_t *pState
)
{
  policyman_subs_info_t *pSubsInfo;

  pSubsInfo = policyman_state_get_subs_info(pState);

  return (pSubsInfo != NULL) ? pSubsInfo->active_subs_mask : 0;
}


/*-------- policyman_subs_set_active_subs_mask --------*/
STATIC void policyman_subs_set_active_subs_mask(
  policyman_state_t *pState,
  uint8              active_subs_mask
)
{
  policyman_subs_info_t *pSubsInfo;

  pSubsInfo = policyman_state_get_subs_info(pState);

  pSubsInfo->active_subs_mask = active_subs_mask;
}


/*-------- policyman_subs_get_dds_subs --------*/
sys_modem_as_id_e_type policyman_subs_get_dds_subs(
  policyman_state_t *pState
)
{
  policyman_subs_info_t *pSubsInfo;

  pSubsInfo = policyman_state_get_subs_info(pState);

  return (pSubsInfo != NULL) ? pSubsInfo->ddsSubs : SYS_MODEM_AS_ID_1;
}


/*-------- policyman_subs_get_dds_is_permanent --------*/
boolean policyman_subs_get_dds_is_permanent(
  policyman_state_t *pState
)
{
  policyman_subs_info_t *pSubsInfo;

  pSubsInfo = policyman_state_get_subs_info(pState);

  return (pSubsInfo != NULL) ? pSubsInfo->ddsChangeisPermanent : FALSE;
}


/*-------- policyman_subs_set_dds_subs_info --------*/
STATIC void policyman_subs_set_dds_subs_info(
  policyman_state_t     *pState,
  sys_modem_as_id_e_type ddsSubs,
  boolean                ddsIsPermanent
)
{
  policyman_subs_info_t *pSubsInfo;

  pSubsInfo = policyman_state_get_subs_info(pState);

  pSubsInfo->ddsSubs              = ddsSubs;
  pSubsInfo->ddsChangeisPermanent = ddsIsPermanent;
}


/*=============================================================================
  Subs-related APIs
=============================================================================*/

/*-------- policyman_str_to_subs --------*/
policyman_subs_t policyman_str_to_subs(
  char const  *pStr
)
{
  typedef struct
  {
    const char        *pStr;
    policyman_subs_t  subs;
  } subs_map;

  static subs_map   map[] =
  {
    {"all",           SUBS_ALL},
    {"this",          SUBS_THIS},
    {"other",         SUBS_OTHER},
    {"any",           SUBS_ANY},
  };

  policyman_subs_t  subs = SUBS_NONE;

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      subs = map[i].subs;
      break;
    }
  }

  if (subs == SUBS_NONE)
  {
    POLICYMAN_UT_MSG_1("Invalid value for 'subs' attribute: %s", token);
  }

  return subs;
}


/*=============================================================================
  Num Subs conditions
=============================================================================*/

/*-----------------------------------------------------------------------------
  Num Subs
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  size_t num_subs;
} policyman_num_subs_condition_t;

/*-------- policyman_condition_num_subs_evaluate --------*/
STATIC boolean policyman_condition_num_subs_evaluate(
  mre_condition_t const  *pCondition,
  void                   *pCtx
)
{
  policy_execute_ctx_t                  *pNSubsCtx = (policy_execute_ctx_t *)pCtx;
  size_t                                 num_subs;
  policyman_num_subs_condition_t const  *pCond     = (policyman_num_subs_condition_t *) pCondition;
  boolean                                result;

  num_subs = policyman_subs_get_num_subs(pNSubsCtx->pState);

  result = pCond->num_subs == num_subs;

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <num_subs> with num_subs %d returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        num_subs,
                        result );
  return result;
}

/*-------- policyman_condition_num_subs_new --------*/
mre_status_t policyman_condition_num_subs_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  mre_status_t                     status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_num_subs_condition_t  *pCondition;
  char const                      *pStr;
  char                             numSubsStr[4];
  int                              numSubs;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL ||
      !policyman_get_token(&pStr, numSubsStr, sizeof(numSubsStr)))
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<num_subs> must specify a number of subscriptions", POLICY_SUBS(pPolicy));
    goto Done;
  }

  numSubs = atoi(numSubsStr);
  if (numSubs < 0)
  {
    POLICYMAN_UT_MSG_2( SUBS_PREFIX "<num_subs> invalid num_subs %s",
                        POLICY_SUBS(pPolicy),
                        numSubsStr );
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_num_subs_condition_t,
                                        pPolicy,
                                        policyman_condition_num_subs_evaluate,
                                        policyman_condition_dtor,
                                        SUBS_THIS,
                                        pElem );

  pCondition->num_subs = numSubs;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "policyman_condition_num_subs_new status %d",
                        POLICY_SUBS(pPolicy),
                        status );
  return status;
}

/*=============================================================================
===============================================================================

  Preference handling

===============================================================================
=============================================================================*/

typedef struct
{
  POLICYMAN_CMD_HDR;

  size_t     num_subs;
  uint8      active_subs_mask;
} policyman_cmd_report_subs_chg_t;

/*-------- policyman_subs_update_multimode_subs --------*/
void policyman_subs_update_multimode_subs(
  policyman_cmd_t         *pCmd
)
{
  policyman_set_t   *pItemSet = policyman_itemset_new();

  /* Run a policy check on the multimode subs.
   */
  policyman_policies_run_policy_check(pCmd->pState, pItemSet);

  policyman_cfgitem_update_items(pItemSet, pCmd);
  REF_CNT_OBJ_RELEASE_IF(pItemSet);
}


/*-------- policyman_report_multimode_subs_execute --------*/
STATIC void policyman_report_multimode_subs_execute(
  policyman_cmd_t *pCmd
)
{
  uint8                             active_subs_mask;
  size_t                            num_subs;
  policyman_cmd_report_subs_chg_t  *pSubsCmd         = NULL;

  pSubsCmd         = (policyman_cmd_report_subs_chg_t *) pCmd;
  num_subs         = policyman_subs_get_num_subs(pCmd->pState);
  active_subs_mask = policyman_subs_get_active_subs_mask(pCmd->pState);

  if (active_subs_mask == pSubsCmd->active_subs_mask &&
      num_subs == pSubsCmd->num_subs)
  {
    return;
  }

  POLICYMAN_MSG_HIGH_4( "executing subs change command active_subs_mask %d->%d, num_subs %d->%d",
                        active_subs_mask,
                        pSubsCmd->active_subs_mask,
                        num_subs,
                        pSubsCmd->num_subs );

  policyman_state_enter_crit_section();

  policyman_subs_set_active_subs_mask(pCmd->pState, pSubsCmd->active_subs_mask);
  policyman_subs_set_num_subs(pCmd->pState, pSubsCmd->num_subs);

  policyman_state_leave_crit_section();

  policyman_subs_update_multimode_subs(pCmd);
}

/*-------- policyman_report_subs_have_changed --------*/
STATIC boolean policyman_report_subs_have_changed(
  policyman_state_t *pState,
  uint8              active_subs_mask,
  size_t             num_subs
)
{
  size_t state_num_subs         = policyman_subs_get_num_subs(pState);
  uint8  state_active_subs_mask = policyman_subs_get_active_subs_mask(pState);

  return    state_num_subs != num_subs
         || state_active_subs_mask != active_subs_mask;
}

/*-------- policyman_block_for_multimode_subs --------*/
STATIC boolean policyman_block_for_multimode_subs(
  policyman_state_t *pState,
  uint8              active_subs_mask,
  size_t             num_subs
)
{

  /*  Block the caller of policyman_report_preference_change() if:
   *    - We're not initializing
   *    - and there are changes in preferences that we care about
   */
  return    !policyman_is_initializing()
         && policyman_report_subs_have_changed(pState, active_subs_mask, num_subs);
}

/*-------- policyman_count_num_subs --------*/
size_t policyman_count_num_subs(
  uint8 active_subs_mask
)
{
  size_t num_subs = 0;

  if (active_subs_mask != SYS_MODEM_AS_ID_NO_CHANGE_MASK)
  {
    while (active_subs_mask>0)
    {
      num_subs += active_subs_mask & 1;
      active_subs_mask >>= 1;
    }
  }

  return num_subs;
}

/*-------- policyman_report_preference_change --------*/
msgr_hdr_s * policyman_report_multimode_subs(
  uint8 active_subs_mask
)
{
  policyman_cmd_report_subs_chg_t *pCmd;
  msgr_hdr_s                      *pMsg = NULL;
  size_t                           num_subs;
  policyman_state_t               *pState = policyman_state_get_state();

  num_subs = policyman_count_num_subs(active_subs_mask);

  /*  If we aren't going to block to process this request, just return.
   */
  if (!policyman_block_for_multimode_subs(pState, active_subs_mask, num_subs))
  {
    goto Done;
  }

  /*  If we should block the calling thread to do this update, get a signal on
   *  which to block for command completion, and put it in the command.
   *  Queue the command and wait for completion.
   */
  pCmd = POLICYMAN_CMD_WITH_HOOK_NEW( policyman_cmd_report_subs_chg_t,
                                      policyman_report_multimode_subs_execute,
                                      NULL,
                                      NULL,
                                      policyman_cfgitem_update_notification_hook );

  pCmd->active_subs_mask = active_subs_mask;
  pCmd->num_subs         = num_subs;
  pCmd->pMsg = NULL;

  POLICYMAN_MSG_HIGH_0("blocking caller of policyman_report_multimode_subs()");
  pCmd->signal = policyman_client_block_sig_get();
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  policyman_client_block_signal_wait(pCmd->signal);

  /*  Get any message to be returned and then release the command.
   */
  pMsg = pCmd->pMsg;
  ref_cnt_obj_release(pCmd);

Done:
  POLICYMAN_MSG_HIGH_1("returning to caller of policyman_report_multimode_subs(pMsg 0x%x)", pMsg);
  return pMsg;
}

/*-------- policyman_is_cm_subs_active --------*/
boolean policyman_is_cm_subs_active(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  uint8 active_subs_mask = SYS_MODEM_AS_ID_NO_CHANGE_MASK;

  switch(subsId)
  {
    case SYS_MODEM_AS_ID_1:
      active_subs_mask = SYS_MODEM_AS_ID_1_MASK;
      break;

    case SYS_MODEM_AS_ID_2:
      active_subs_mask = SYS_MODEM_AS_ID_2_MASK;
      break;

    case SYS_MODEM_AS_ID_3:
      active_subs_mask = SYS_MODEM_AS_ID_3_MASK;
      break;

    default:
     POLICYMAN_MSG_ERROR_1("subsId %d is not handled for checking active subs", subsId);
     break;
  }

  return ((policyman_subs_get_active_subs_mask(pState) & active_subs_mask) == active_subs_mask);
}


/*=============================================================================
===============================================================================

  DDS handling

===============================================================================
=============================================================================*/

 /*-----------------------------------------------------------------------------
   Is SUBS DDS
 -----------------------------------------------------------------------------*/

 typedef struct
 {
   POLICYMAN_CONDITION_BASE;
 } policyman_is_subs_dds_condition_t;



/*-------- evaluate_is_subs_dds --------*/
STATIC boolean evaluate_is_subs_dds(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
)
{
  policy_execute_ctx_t      *pDdsCtx    = (policy_execute_ctx_t *)pCtx;
  sys_modem_as_id_e_type     ddsSubs    = policyman_subs_get_dds_subs(pDdsCtx->pState);
  boolean                    ddsIsPerm  = policyman_subs_get_dds_is_permanent(pDdsCtx->pState);
  boolean                    result;

  if (ddsIsPerm)
  {
    result = (subsId == ddsSubs);
  }
  else
  {
    result = (subsId != ddsSubs);
  }

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX SRC_LINE "condition <is_subs_dds subs=%d> checking with current dds subs %d returns %d",
                        CONDITION_SUBS(pCondition),
                        LINE_NUM(pCondition),
                        subsId,
                        ddsSubs,
                        result );
  return result;
}


 /*-------- policyman_condition_is_subs_dds_evaluate --------*/
STATIC boolean policyman_condition_is_subs_dds_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_condition_t const *pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_is_subs_dds);
}

 /*-------- policyman_condition_is_subs_dds_new --------*/
mre_status_t policyman_condition_is_subs_dds_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  mre_status_t                       status     = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_is_subs_dds_condition_t *pCondition = NULL;
  policyman_subs_t                   pm_subs;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<is_subs_dds> must specify a subscription", POLICY_SUBS(pPolicy));
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_is_subs_dds_condition_t,
                                        pPolicy,
                                        policyman_condition_is_subs_dds_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "policyman_condition_is_subs_dds_new returned status %d",
                        POLICY_SUBS(pPolicy),
                        status );
  return status;
}


typedef struct
{
  POLICYMAN_CMD_HDR;

  dds_info_t subsInfo;
  uint8      activeSubsMask;
} policyman_cmd_report_subs_info_t;

/*-------- policyman_has_dds_type_changed --------*/
STATIC boolean policyman_has_dds_type_changed(
  policyman_state_t      *pState,
  boolean                 isPermanent
)
{
  policyman_subs_info_t *pSubsInfo;

  pSubsInfo = policyman_state_get_subs_info(pState);

  return pSubsInfo->ddsChangeisPermanent != isPermanent;
}

/*-------- policyman_has_dds_changed --------*/
STATIC boolean policyman_has_dds_changed(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  ddsSubs
)
{
  sys_modem_as_id_e_type state_ddsSubs = policyman_subs_get_dds_subs(pState);

  return state_ddsSubs != ddsSubs;
}

/*-------- policyman_block_for_dds_change --------*/
STATIC boolean policyman_block_for_dds_change(
  policyman_state_t *pState,
  dds_info_t const  *pSubsInfo
)
{

  /*  Block the caller of policyman_msim_report_subs_info() if:
   *    - We're not initializing
   *    - and there is change in DDS
   */
  return    !policyman_is_initializing() &&
            (policyman_has_dds_changed(pState, pSubsInfo->subsId) ||
             policyman_has_dds_type_changed(pState, pSubsInfo->isPermanent));
}

/*-------- policyman_subs_handle_subs_update --------*/
STATIC void policyman_subs_handle_subs_update(
  policyman_cmd_t  *pCmd
)
{
  size_t                             num_subs;
  policyman_subs_info_t             *pSubsInfo  = policyman_state_get_subs_info(pCmd->pState);
  policyman_set_t                   *pItemSet   = policyman_itemset_new();
  policyman_cmd_report_subs_info_t  *pSubsCmd   = NULL;

  pSubsCmd = (policyman_cmd_report_subs_info_t *)pCmd;

  num_subs = policyman_count_num_subs(pSubsCmd->activeSubsMask);

  policyman_state_enter_crit_section();
  policyman_subs_set_num_subs(pCmd->pState, num_subs);
  policyman_subs_set_active_subs_mask(pCmd->pState, pSubsCmd->activeSubsMask);
  policyman_subs_set_dds_subs_info(pCmd->pState, pSubsCmd->subsInfo.subsId, pSubsCmd->subsInfo.isPermanent);
  policyman_state_leave_crit_section();

  /* Update subphone configs for DDS Subs first if change is permanent */
  if (pSubsInfo->ddsChangeisPermanent)
  {
    policyman_device_config_update_subphone_config(pSubsInfo->ddsSubs, pItemSet);
  }

  policyman_policies_run_policy_check(pCmd->pState, pItemSet);

  /* Ensure device config Policy item is always returned */
  policyman_cfgitem_update_device_config_for_force_notification(pItemSet);

  policyman_cfgitem_update_items(pItemSet, pCmd);
  REF_CNT_OBJ_RELEASE_IF(pItemSet);
}

/*-------- policyman_change_subs_info_execute --------*/
STATIC void policyman_change_subs_info_execute(
  policyman_cmd_t *pCmd
)
{
  size_t                            num_subs;
  policyman_cmd_report_subs_info_t *pSubsCmd;
  policyman_subs_info_t            *pInfo     = policyman_state_get_subs_info(pCmd->pState);

  pSubsCmd  = (policyman_cmd_report_subs_info_t *)pCmd;
  num_subs  = policyman_count_num_subs(pSubsCmd->activeSubsMask);

  if (  pInfo->active_subs_mask == pSubsCmd->activeSubsMask
     && pInfo->ddsSubs == pSubsCmd->subsInfo.subsId
     && pInfo->ddsChangeisPermanent == pSubsCmd->subsInfo.isPermanent
     )
  {
    return;
  }

  POLICYMAN_MSG_HIGH_8( "executing subs change cmd: active_subs_mask %d->%d, isPermanent %d->%d, ddsSubs %d->%d, num_subs %d->%d",
                        pInfo->active_subs_mask,
                        pSubsCmd->activeSubsMask,
                        pInfo->ddsChangeisPermanent,
                        pSubsCmd->subsInfo.isPermanent,
                        pInfo->ddsSubs,
                        pSubsCmd->subsInfo.subsId,
                        pInfo->num_subs,
                        num_subs );

  policyman_subs_handle_subs_update(pCmd);
}

/*-------- policyman_msim_report_subs_info --------*/
msgr_hdr_s * policyman_msim_report_subs_info(
  dds_info_t const *pSubsInfo,
  uint8             active_subs_mask
)
{
  policyman_cmd_report_subs_info_t *pCmd;
  msgr_hdr_s                       *pMsg = NULL;
  size_t                            num_subs;
  policyman_state_t                *pState = policyman_state_get_state();

  num_subs = policyman_count_num_subs(active_subs_mask);

  /*  If we aren't going to block to process this request, just return.
   */
  if (  !policyman_block_for_multimode_subs(pState, active_subs_mask, num_subs)
     && !policyman_block_for_dds_change(pState, pSubsInfo)
     )
  {
    POLICYMAN_MSG_HIGH_0("not blocking caller of policyman_msim_report_subs_info()");
    goto Done;
  }

  /*  If we should block the calling thread to do this update, get a signal on
   *  which to block for command completion, and put it in the command.
   *  Queue the command and wait for completion.
   */
  pCmd = POLICYMAN_CMD_WITH_HOOK_NEW( policyman_cmd_report_subs_info_t,
                                      policyman_change_subs_info_execute,
                                      NULL,
                                      NULL, 
                                      policyman_cfgitem_update_notification_hook );

  pCmd->activeSubsMask       = active_subs_mask;
  pCmd->subsInfo.subsId      = pSubsInfo->subsId;
  pCmd->subsInfo.isPermanent = pSubsInfo->isPermanent;

  pCmd->pMsg = NULL;

  POLICYMAN_MSG_HIGH_0("blocking caller of policyman_msim_report_subs_info()");
  pCmd->signal = policyman_client_block_sig_get();
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  policyman_client_block_signal_wait(pCmd->signal);

  /*  Get any message to be returned and then release the command.
   */
  pMsg = pCmd->pMsg;
  ref_cnt_obj_release(pCmd);

  POLICYMAN_MSG_HIGH_1("returning to caller of policyman_msim_report_subs_info(pMsg 0x%x)", pMsg);

Done:
  return pMsg;
}
