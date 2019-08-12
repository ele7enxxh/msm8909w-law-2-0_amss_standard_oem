/**
  @file policyman_call_mode.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_call_mode.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre.h"
#include "mre_efs.h"

#include "policyman_call_mode.h"
#include "policyman_cfgitem.h"
#include "policyman_cm.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_phone_events.h"
#include "policyman_policies.h"
#include "policyman_set.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_xml.h"
#include "sys.h"
#include "policyman_i.h"
#include "policyman_plmn.h"
#include "policyman_rules.h"

#include <stringl/stringl.h>

/*=============================================================================
  Voice Domain Pref APIs
=============================================================================*/

typedef struct
{
  POLICYMAN_ACTION_BASE;

  sys_call_mode_t         call_mode;
} call_mode_action_t;


/*-------- policyman_call_mode_update_to_efs  --------*/
void policyman_call_mode_update_to_efs(
  policyman_item_t const *pItem
)
{
  boolean                      status;
  policyman_call_mode_item_t  *pMode;

  pMode = (policyman_call_mode_item_t *) pItem;

  status = policyman_cfgitem_persist_item( POLICYMAN_CALL_MODE_NV_PATH,
                                           &pMode->call_mode,
                                           sizeof(pMode->call_mode),
                                           pMode->asubs_id );

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "write call_mode %d to EFS, status %d",
                        pMode->asubs_id,
                        pMode->call_mode,
                        status );
}


/*-------- policyman_retrieve_call_mode --------*/
static boolean policyman_retrieve_call_mode(
  sys_call_mode_t         *callMode,
  sys_modem_as_id_e_type   asubs_id
)
{
  sys_call_mode_t          returnMode = SYS_CALL_MODE_NORMAL;
  mre_efs_status_t         fs_status;
  sys_call_mode_t         *pEfsMode   = NULL;
  size_t                   modeSize   = 0;

  /*  If the NV can be read and the value is in the proper range,
   *  set the mode to that value. Otherwise just return NORMAL mode.
   */

  /*  Try and read from the NV item for UE mode.
   */
  fs_status = mre_efs_get_file( POLICYMAN_CALL_MODE_NV_PATH,
                                (void **)&pEfsMode,
                                &modeSize,
                                asubs_id );

  if (MRE_EFS_STATUS_SUCCESS == fs_status)
  {
    /* Success indicates pEfsMode != NULL (otherwise it wouldn't have been successful) */

    if (modeSize == sizeof(sys_call_mode_t))
    {
      if (   (*pEfsMode >= SYS_CALL_MODE_NORMAL)
          && (*pEfsMode <= SYS_CALL_MODE_VOLTE_ONLY) )
      {
        // Return the validated mode from the EFS file
        returnMode = *pEfsMode;
        POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "read call_mode %d from EFS",
                              asubs_id,
                              *pEfsMode );
      }
    }
  }

  /* Always free the returned buffer if there is one.
   * It is possible for the get_efs_file to return an error
   * after the buffer is allocated, although unlikely.
   */
  POLICYMAN_MEM_FREE_IF(pEfsMode);

  /* Caller gets the final result */
  *callMode = returnMode;

  return TRUE;
}


/*-------- policyman_call_mode_item_new --------*/
policyman_call_mode_item_t *policyman_call_mode_item_new(
  sys_call_mode_t          call_mode,
  sys_modem_as_id_e_type   asubs_id
)
{
  policyman_call_mode_item_t  *pItem;

  pItem = (policyman_call_mode_item_t *)policyman_cfgitem_new_item( POLICYMAN_ITEM_CALL_MODE,
                                                                    sizeof(policyman_call_mode_item_t),
                                                                    policyman_simple_dtor );
  pItem->asubs_id  = asubs_id;
  pItem->call_mode = call_mode;

  return pItem;
}


/*-------- policyman_call_mode_get_default --------*/
policyman_item_t *policyman_call_mode_get_default(
  policy_execute_ctx_t *pCtx
)
{
  policyman_call_mode_item_t  *pItem;
  sys_call_mode_t              callMode = SYS_CALL_MODE_NORMAL;

  /* can remove this call once get efs is ported for cfg items*/
  (void)policyman_retrieve_call_mode(&callMode, pCtx->asubs_id);

  pItem = policyman_call_mode_item_new(callMode, pCtx->asubs_id);

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "default call_mode %d",
                        pItem->asubs_id,
                        pItem->call_mode );

  return (policyman_item_t *)pItem;
}

/*-------- policyman_call_mode_compare --------*/
boolean policyman_call_mode_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
)
{
  policyman_call_mode_item_t  *eMode1 = (policyman_call_mode_item_t *)pData1;
  policyman_call_mode_item_t  *eMode2 = (policyman_call_mode_item_t *)pData2;

  return (eMode1->call_mode == eMode2->call_mode);
}

/*-------- policyman_call_mode_display --------*/
void policyman_call_mode_display(
  policyman_item_t  *pItem
)
{
  policyman_call_mode_item_t  *eMode = (policyman_call_mode_item_t *)pItem;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "call_mode %d",
                        eMode->asubs_id,
                        eMode->call_mode);
}

/*-------- policyman_str_to_call_mode --------*/
mre_status_t policyman_str_to_call_mode(
  const char            *pStr,
  sys_call_mode_t       *pEcallMode
)
{
  typedef struct
  {
    const char           *pStr;
    sys_call_mode_t       callMode;
  } mask_map;

  static mask_map map[] =
  {
    {"NORMAL",   SYS_CALL_MODE_NORMAL},
    {"VOLTE",    SYS_CALL_MODE_VOLTE_ONLY}
  };

  size_t       idx;
  char         token[8];
  mre_status_t status   = MRE_STATUS_ERR_MALFORMED_XML;

  *pEcallMode = SYS_CALL_MODE_NORMAL;

  policyman_get_token(&pStr, token, sizeof(token));

  for (idx = 0 ; idx < ARR_SIZE(map) ; ++idx)
  {
    if (strcasecmp(token, map[idx].pStr) == 0)
    {
      *pEcallMode = map[idx].callMode;
      status = MRE_STATUS_SUCCESS;
      break;
    }
  }

  return status;
}


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  sys_call_mode_t  callMode;
} policyman_call_mode_condition_t;

/*=============================================================================
  Accessor function for CALL MODE mode
=============================================================================*/

/*-------- policyman_get_call_mode_from_item --------*/
policyman_status_t policyman_get_call_mode_from_item(
  policyman_item_t const  *pItem,
  sys_call_mode_t         *pEcallMode
)
{
  policyman_call_mode_item_t  *pMode = (policyman_call_mode_item_t *) pItem;

  if (pMode == NULL || pEcallMode == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_call_mode returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_CALL_MODE)
  {
    POLICYMAN_MSG_ERROR_0("get_call_mode returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pEcallMode = pMode->call_mode;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "get_call_mode_from_item() returns call_mode %d",
                        pMode->asubs_id,
                        pMode->call_mode );

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- evaluate_call_mode --------*/
static boolean evaluate_call_mode(
  policyman_condition_t const  *pCondition,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  policyman_status_t                    status;
  policyman_item_id_t                   id                = POLICYMAN_ITEM_CALL_MODE;
  policyman_call_mode_item_t           *pItem             = NULL;
  sys_call_mode_t                       current_call_mode = SYS_CALL_MODE_NORMAL;
  boolean                               result            = FALSE;
  policyman_call_mode_condition_t      *pCond             = (policyman_call_mode_condition_t *) pCondition;

  pItem = policyman_cfgitem_find_item_in_context( pCtx, id, SYS_MODEM_AS_ID_1);

  if (NULL != pItem)
  {
    status = policyman_get_call_mode_from_item( (policyman_item_t const *)pItem,
                                                &current_call_mode );
    if (POLICYMAN_SUCCEEDED(status))
    {
      result = (current_call_mode == pCond->callMode);
    }
  }

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <call_mode_is> %d returns %d",
                        pCond->pmSubsId,
                        LINE_NUM(pCond),
                        pCond->callMode,
                        result );

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return result;
}

/*-------- policyman_condition_call_mode_evaluate --------*/
static boolean policyman_condition_call_mode_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_call_mode);
}

/*-------- policyman_condition_call_mode_new --------*/
mre_status_t policyman_condition_call_mode_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
)
{
  mre_status_t                       status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_call_mode_condition_t   *pCondition;
  char const                        *pStr;
  sys_call_mode_t                    callMode_info;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_MSG_ERROR_1( SUBS_PREFIX "<policyman_condition_call_mode_new> emergency call mode required",
                           POLICY_SUBS(pPolicy) );
    goto Done;
  }

  status = policyman_str_to_call_mode(pStr, &callMode_info);

  if (status != MRE_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_1( SUBS_PREFIX "<policyman_condition_call_mode_new> invalid call_mode",
                           POLICY_SUBS(pPolicy) );
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_call_mode_condition_t,
                                        pPolicy,
                                        policyman_condition_call_mode_evaluate,
                                        policyman_condition_dtor,
                                        SUBS_THIS,
                                        pElem );

  pCondition->callMode  = callMode_info;

  *ppCondition = (mre_condition_t *)pCondition;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*-------- policyman_call_mode_execute --------*/
static boolean execute_call_mode(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  call_mode_action_t          *pCallModeAction = (call_mode_action_t *)pAction;
  policy_execute_ctx_t        *pCallModeCtx    = (policy_execute_ctx_t *)pCtx;
  policyman_call_mode_item_t  *pCallModeItem;

  pCallModeItem = policyman_call_mode_item_new(pCallModeAction->call_mode, subsId);

  policyman_set_add(pCallModeCtx->pItemSet, &pCallModeItem);
  ref_cnt_obj_release(pCallModeItem);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "action <call_mode> set call_mode to %d",
                        subsId,
                        LINE_NUM(pCallModeAction),
                        pCallModeItem->call_mode );

  return TRUE;
}


/*-------- policyman_call_mode_execute --------*/
static boolean policyman_call_mode_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const *pAct = (policyman_action_t *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_call_mode);
}

/*-------- execute_call_mode_if --------*/
static boolean execute_call_mode_if(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  call_mode_action_t        *pCallModeAction  = (call_mode_action_t *)pAction;
  boolean                    succeeded        = TRUE;
  policy_execute_ctx_t      *pcallCtx         = (policy_execute_ctx_t *)pCtx;
  policyman_set_t           *pItemSet         = pcallCtx->pItemSet;
  boolean                    item_is_present;


  item_is_present = policyman_cfgitem_is_present_in_itemset( pItemSet,
                                                             POLICYMAN_ITEM_CALL_MODE,
                                                             subsId,
                                                             NULL);

  if (item_is_present)
  {
    POLICYMAN_UT_MSG_1(SRC_LINE "<call_mode_if> skipping update -- item present", LINE_NUM(pCallModeAction));
  }
  else
  {
    succeeded = policyman_call_mode_execute((mre_action_t const  *)pAction, pcallCtx);
  }

  return succeeded;
}

/*-------- policyman_call_mode_if_execute --------*/
STATIC boolean policyman_call_mode_if_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  return policyman_execute_action((policyman_action_t const *)pAction,
                                  pCtx,
                                  execute_call_mode_if);
}

/*-------- policyman_call_mode_create_new_action --------*/
mre_status_t policyman_call_mode_create_new_action(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction,
  pfn_execute_t             actionfn
)
{
  mre_status_t            status   = MRE_STATUS_ERR_INVALID_ACTION;
  call_mode_action_t     *pAction  = NULL;
  sys_call_mode_t         callMode;
  char const             *pStr;
  policyman_subs_t        pm_subs;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_MSG_ERROR_1( SUBS_PREFIX "<policyman_call_mode_create_new_action>: requires a call_mode mode",
                           POLICY_SUBS(pPolicy) );
    goto Done;
  }

  status = policyman_str_to_call_mode(pStr, &callMode);

  if (status != MRE_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_1( SUBS_PREFIX "<policyman_call_mode_create_new_action>: invalid call mode",
                           POLICY_SUBS(pPolicy) );
    goto Done;
  }

  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( call_mode_action_t,
                                  pPolicy,
                                  actionfn,
                                  policyman_action_dtor,
                                  pm_subs,
                                  pElem );

  pAction->call_mode = callMode;

  *ppAction = (mre_action_t *)pAction;

Done:
  return status;
}


/*-------- policyman_call_mode_action_new --------*/
mre_status_t policyman_call_mode_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
)
{
  return policyman_call_mode_create_new_action( pElem,
                                                (mre_policy_t *)pPolicy,
                                                ppAction,
                                                policyman_call_mode_execute );
}

/*-------- policyman_call_mode_if_action_new --------*/
mre_status_t policyman_call_mode_if_action_new(
  mre_xml_element_t const       *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t                 **ppAction
)
{
  return policyman_call_mode_create_new_action( pElem,
                                                (mre_policy_t *)pPolicy,
                                                ppAction,
                                                policyman_call_mode_if_execute );
}


/*-------- policyman_get_call_mode --------*/
sys_call_mode_t policyman_get_call_mode(
  sys_modem_as_id_e_type   subsId,
  sys_plmn_id_s_type      *pCampedPLMN
)
{
  sys_call_mode_t    ret_mode = SYS_CALL_MODE_NORMAL;

  if (NULL != pCampedPLMN)
  {
    sys_mcc_type       mcc;
    boolean            volte_is_enabled;
    policyman_state_t *pState;

    pState = policyman_state_get_state();

    /* Get MCC from given PLMN */
    mcc = policyman_plmn_get_mcc(pCampedPLMN);

    /* Get volte_is_enabled  from ph state */
    volte_is_enabled = policyman_ph_get_volte_is_enabled(pState, subsId);

    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "policyman_get_call_mode(mcc %d) volte_is_enabled: %d",
                          subsId,
                          mcc,
                          volte_is_enabled );

    /* Determine if VOLTE_ONLY mode */
    if (   volte_is_enabled
        && policyman_plmn_mcc_is_volte_only(mcc, subsId) )
    {
      ret_mode = SYS_CALL_MODE_VOLTE_ONLY;
    }
  }

  return ret_mode;
}

/*-------- policyman_call_mode_fill_subs_diag_log --------*/
void policyman_call_mode_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  policyman_call_mode_item_t  *pMode = (policyman_call_mode_item_t *)pItem;

  log_ptr->call_mode = (int8)pMode->call_mode;
}
