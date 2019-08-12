/**
  @file mre_rules.c

  @brief  Utility APIs to help in interpreting components of rules.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/mre_rules.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre.h"
#include "mre_dbg.h"
#include "mre_diag.h"
#include "mre_engine.h"
#include "mre_lang.h"
#include "mre_rules.h"
#include "mre_set.h"
#include "mre_util.h"
#include "mre_xml.h"

#include <stringl/stringl.h>


/*=============================================================================
  Conditions
=============================================================================*/


/*  Mappings between condition tags and the functions to create a new condition
 *  from the XML document.
 */
static mre_condition_map_element_t const  condition_map[] =
{
  /*---------------------------------------------------------------------------
    Language primitives
  ---------------------------------------------------------------------------*/
  {"cond",                mre_condition_and_new,    MRE_PRECOND_NONE},
  {"conditions",          mre_condition_and_new,    MRE_PRECOND_NONE},
  {"all_of",              mre_condition_and_new,    MRE_PRECOND_NONE},
  {"any_of",              mre_condition_or_new,     MRE_PRECOND_NONE},
  {"true",                mre_condition_true_new,           MRE_PRECOND_NONE},
  {"not",                 mre_condition_not_new,            MRE_PRECOND_NONE},
  {"boolean_test",        mre_boolean_test_new,             MRE_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Terminator
  ---------------------------------------------------------------------------*/
  {NULL,                  NULL,                             MRE_PRECOND_NONE}
};


/*-------- mre_condition_dtor --------*/
void mre_condition_dtor(
  void  *pObj
)
{
  mre_condition_t *pCond = (mre_condition_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pCond->pCondSet);
  mre_mem_free(pCond);
}


/*-------- mre_find_condition --------*/
static mre_condition_map_element_t const *mre_find_condition(
  char                        const *pCondName,
  mre_condition_map_element_t const *pMap
)
{
  while (NULL != pMap->pTag)
  {
    if (strcmp(pCondName, pMap->pTag) == 0)
    {
      return pMap;
    }

    ++pMap;
  }

  return NULL;
}


/*-------- mre_find_condition_entry --------*/
static mre_condition_map_element_t const *mre_find_condition_entry(
  char const    *pCondName,
  mre_policy_t  *pPolicy
)
{
  mre_condition_map_element_t const *pEntry;

  /*  Search the MRE condition map first
   */
  pEntry = mre_find_condition(pCondName, condition_map);

  /*  If not found in the MRE condition map, search the client's map.
   */
  if (NULL == pEntry && NULL != MRE_POLICY_CONDTBL(pPolicy))
  {
    pEntry = mre_find_condition(pCondName, MRE_POLICY_CONDTBL(pPolicy));
  }

  if (pEntry == NULL)
  {
    MRE_UT_MSG_1("Unknown condition tag: %s", pCondName);
  }

  return pEntry;
}


/*-------- mre_rules_add_condition --------*/
mre_status_t mre_rule_add_condition(
  mre_xml_element_t const *pElem,
  void                    *pCtx
)
{
  mre_status_t                       status = MRE_STATUS_ERR_MALFORMED_XML;
  policy_set_ctx_t                  *pCondCtx = (policy_set_ctx_t *) pCtx;
  char const                        *pCondName;
  mre_condition_map_element_t const *pEntry;
  mre_condition_t                   *pCondition = NULL;

  pCondName = mre_xml_get_tag(pElem);
  pEntry = mre_find_condition_entry(pCondName, pCondCtx->pPolicy);

  /*  If we found an entry, call its create function to create a condition.
   */
  if (NULL != pEntry)
  {
    status = pEntry->pfnNew(pElem, pCondCtx->pPolicy, &pCondition);
    if (MRE_FAILED(status))
    {
      MRE_UT_MSG_2("Error %08x while creating condition %s", status, pCondName);
    }
  }

  /*  If condition creation was successful, fill out the condition.
   */
  if (NULL != pCondition)
  {
    pCondition->pPolicy = pCondCtx->pPolicy;
    mre_set_add(pCondCtx->pSet, &pCondition);
    ref_cnt_obj_release(pCondition);

    MRE_POLICY_PRECONDITIONS(pCondCtx->pPolicy) |= pEntry->precondition;
  }

  return status;
}


/*-------- mre_rule_get_condition --------*/
boolean mre_rule_get_condition(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCond
)
{
  char const                        *pCondName;
  mre_condition_map_element_t const *pEntry;

  pCondName = mre_xml_get_tag(pElem);
  MRE_UT_MSG_1("Processing tag %s as condition", pCondName);
  
  pEntry = mre_find_condition_entry(pCondName, pPolicy);

  /*  If we found an entry, call its create function to create a condition.
   */
  if (NULL != pEntry)
  {
    mre_status_t      status;

    status = pEntry->pfnNew(pElem, pPolicy, ppCond);
    if (MRE_SUCCEEDED(status))
    {
      (*ppCond)->pPolicy = pPolicy;
      MRE_POLICY_PRECONDITIONS(pPolicy) |= pEntry->precondition;
    }
    else
    {
      MRE_UT_MSG_2("Error 0x%08x while creating condition %s", status, pCondName);
    }
  }

  return *ppCond != NULL;
}



/*=============================================================================
  Actions
=============================================================================*/

STATIC mre_status_t mre_action_list_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

/*  Mappings between action tags and the functions to create a new action
 *  from the XML document.
 */
static mre_action_map_element_t const action_map[] =
{
  /*---------------------------------------------------------------------------
    Language actions
  ---------------------------------------------------------------------------*/
  {"if",                      mre_if_action_new},
  {"rule",                    mre_if_action_new},

  {"actions",                 mre_action_list_action_new},
  {"then",                    mre_action_list_action_new},
  {"else",                    mre_action_list_action_new},

  {"boolean_define",          mre_boolean_define_new},
  {"boolean_set",             mre_boolean_set_new},

  {"continue",                mre_continue_new},
  {"stop",                    mre_stop_action_new},

  {"select",                  mre_select_action_new},

  /*---------------------------------------------------------------------------
    Terminator
  ---------------------------------------------------------------------------*/
  {NULL,                      NULL}
};


/*=============================================================================
  General internal functions to handle rules
=============================================================================*/


/*-------- mre_action_dtor --------*/
void mre_action_dtor(
  void  *pObj
)
{
  mre_mem_free(pObj);
}


/*-------- mre_find_action --------*/
static mre_action_map_element_t const *mre_find_action(
  char const                      *pActName,
  mre_action_map_element_t const  *pMap
)
{
  while (NULL != pMap->pTag)
  {
    if (strcmp(pActName, pMap->pTag) == 0)
    {
      return pMap;
    }

    ++pMap;
  }

  return NULL;
}


/*-------- mre_find_action_entry --------*/
static mre_action_map_element_t const *mre_find_action_entry(
  char const    *pActName,
  mre_policy_t  *pPolicy
)
{
  mre_action_map_element_t const *pEntry;

  /*  Search the MRE action map first
   */
  pEntry = mre_find_action(pActName, action_map);

  /*  If not found in the MRE action map, search the client's map.
   */
  if (NULL == pEntry && NULL != MRE_POLICY_ACTTBL(pPolicy))
  {
    pEntry = mre_find_action(pActName, MRE_POLICY_ACTTBL(pPolicy));
  }

  if (NULL == pEntry)
  {
    MRE_UT_MSG_1("Unknown action tag: %s", pActName);
  }

  return pEntry;
}


/*-------- mre_rule_get_single_action --------*/
boolean mre_rule_get_single_action(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
)
{
  char const                      *pActName;
  mre_action_map_element_t const  *pEntry;
  mre_status_t                     status = MRE_STATUS_ERR_MALFORMED_XML;

  *ppAction = NULL;

  pActName = mre_xml_get_tag(pElem);
  pEntry = mre_find_action_entry(pActName, pPolicy);

  /*  If we found an entry, call its create function to create an action.
   */
  if (NULL != pEntry)
  {
    status = pEntry->pfnNew(pElem, pPolicy, ppAction);
    if (MRE_SUCCEEDED(status))
    {
      (*ppAction)->pPolicy = pPolicy;
    }
    else
    {
      MRE_UT_MSG_2("Error %08x while creating action %s", status, pActName);
    }
  }

  return MRE_SUCCEEDED(status);
}


/*-------- mre_rule_add_action_to_set --------*/
boolean mre_rule_add_action_to_set(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_set_t               *pActionSet
)
{
  boolean        succeeded;
  mre_action_t  *pAction = NULL;

  succeeded = mre_rule_get_single_action(pElem, pPolicy, &pAction);
  if (succeeded)
  {
    mre_set_append(pActionSet, &pAction);
    ref_cnt_obj_release(pAction);
  }

  return succeeded;
}


/*-------- gather_action --------*/
STATIC mre_status_t gather_action(
  mre_xml_element_t const *pElem,
  void                    *pCtx
)
{
  policy_set_ctx_t *pPsCtx = (policy_set_ctx_t *) pCtx;
  boolean           succeeded;
    
  MRE_XML_MSG("processing %s", mre_xml_get_tag(pElem), 0, 0);

  succeeded = mre_rule_add_action_to_set(pElem, pPsCtx->pPolicy, pPsCtx->pSet);

  return succeeded? MRE_STATUS_SUCCESS : MRE_STATUS_ERR_MALFORMED_XML;
}


/*-------- mre_rule_get_actionset --------*/
boolean mre_rule_get_actionset(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_set_t              **ppActionSet
)
{
  size_t  nActions;
  mre_status_t      status;
  policy_set_ctx_t  ctx;

  nActions = mre_xml_num_children(pElem);

  *ppActionSet = mre_set_refcnt_new(
                        (nActions == 0)? 1 : nActions,  /* zero actions is OK, but we need a set */
                        MRE_SET_NO_MAX_SIZE,
                        NULL
                        );

  ctx.pPolicy = pPolicy;
  ctx.pSet    = *ppActionSet;
  status = mre_xml_process_children(pElem, gather_action, &ctx);

  return MRE_SUCCEEDED(status);
}


/*-------- execute_action --------*/
boolean execute_action(
  void  *pElem,
  void  *pCtx
)
{
  mre_action_t  *pAction = *((mre_action_t **) pElem);

  return mre_execute_action(pAction, pCtx);
}


/*-------- mre_actionset_execute --------*/
void mre_actionset_execute(
  mre_set_t const *pActionSet,
  void            *pCtx
)
{
  mre_set_iterate((mre_set_t *) pActionSet, execute_action, pCtx);
}


/*-------- mre_action_list_action_t --------*/
typedef struct
{
  MRE_ACTION_BASE;

  mre_set_t *pActionSet;
} mre_action_list_action_t;


/*-------- mre_action_list_action_dtor --------*/
STATIC void mre_action_list_action_dtor(
  void  *pObj
)
{
  mre_action_list_action_t *pAction = (mre_action_list_action_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pAction->pActionSet);
  mre_action_dtor(pObj);
}


/*-------- mre_action_list_action_execute --------*/
STATIC boolean mre_action_list_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  mre_action_list_action_t *pAct = (mre_action_list_action_t *) pAction;

  MRE_POLICY_DESCEND_LEVEL(pAct->pPolicy);
  mre_actionset_execute(pAct->pActionSet, pCtx);
  MRE_POLICY_ASCEND_LEVEL(pAct->pPolicy);

  return !MRE_POLICY_STOPEVAL(pAct->pPolicy);
}


/*-------- mre_action_list_action_new --------*/
STATIC mre_status_t mre_action_list_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t               status = MRE_STATUS_ERR_MALFORMED_XML;
  mre_set_t                 *pActionSet = NULL;
  mre_action_list_action_t  *pAction    = NULL;

  if (mre_rule_get_actionset(pElem, pPolicy, &pActionSet))
  {
    status = MRE_STATUS_SUCCESS;

    pAction = (mre_action_list_action_t *) mre_action_new(sizeof(mre_action_list_action_t),
                                                          pPolicy,
                                                          mre_action_list_action_execute,
                                                          mre_action_list_action_dtor,
                                                          pElem );

    pAction->pActionSet = pActionSet;
    
    *ppAction = (mre_action_t *) pAction;
  }
  else
  {
    REF_CNT_OBJ_RELEASE_IF(pActionSet);
  }

  return status;
}


/*-------- mre_rules_continue_evaluation --------*/
void mre_rules_continue_evaluation(
  mre_policy_t  *pPolicy
)
{
  MRE_POLICY_INFO(pPolicy)->contEval = TRUE;
}


/*-------- mre_rule_preconditions_met --------*/
boolean mre_rule_preconditions_met(
  mre_rule_t  const *pRule
)
{
  boolean        met;
  mre_policy_t  *pPolicy;
  uint32         rulePreconds;

  pPolicy = pRule->pPolicy;
  rulePreconds = pRule->preconds;

  if (MRE_PRECOND_NONE == rulePreconds)
  {
      met = TRUE;
      MRE_MSG_HIGH_1(SRC_LINE "<if>/<rule> evaluated; no preconditions", LINE_NUM(pRule));
  }
  else
  {
    uint32  precondsMet;

    if (MRE_PRECOND_POLICY == rulePreconds)
    {
      rulePreconds = MRE_POLICY_PRECONDITIONS(pPolicy);
    }

    precondsMet  = MRE_POLICY_CURRENT_PRECONDS(pPolicy);
    met = ((rulePreconds & precondsMet) == rulePreconds);
    if (!met)
    {
      MRE_MSG_HIGH_3( SRC_LINE "<if>/<rule> skipped due to preconditions; rule %d, met %d",
                      LINE_NUM(pRule),
                      rulePreconds,
                      precondsMet );
    }
  }

  return met;
}


/*-------- mre_move_to_actions --------*/
/**
@brief  The <initial> block was originally defined to require an <actions> node.
        This is completely unnecessary - we should be able to embed actions
        directly in <initial>.  This function returns the <actions> subnode if
        present, otherwise it returns the <initial> node.
*/
static mre_xml_element_t const  *mre_move_to_actions(
    mre_xml_element_t const *pElem
)
{
  mre_xml_element_t const *pChild;

  /*  See if this has an <actions> node - if so move the element to
   *  point to that rather than the <initial> node.
   */
  pChild = mre_xml_get_child(pElem, 0);
  if (NULL != pChild)
  {
    char const  *pStr;

    pStr = mre_xml_get_tag(pChild);
    if (strcasecmp(pStr, "actions") == 0)
    {
      pElem = pChild;
    }
  }

  return pElem;
}


/*-------- mre_ensure_initset --------*/
/**
@brief  Return a pointer to the initset, creating it if necessary.
*/
static mre_set_t *mre_ensure_initset(
  mre_policy_t  *pPolicy,
  size_t        nChildren
)
{
  mre_set_t *pInitSet;

  pInitSet = MRE_POLICY_INITSET(pPolicy);
  if (NULL == pInitSet)
  {
    pInitSet = mre_set_refcnt_new(nChildren, MRE_SET_NO_MAX_SIZE, NULL);
    MRE_POLICY_INITSET(pPolicy) = pInitSet;
  }

  return pInitSet;
}
  

/*-------- mre_initset_add --------*/
boolean mre_initset_add(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem
)
{
  mre_status_t      status;
  size_t     nChildren;
  policy_set_ctx_t  ctx;

  /*  Ensure we're at the node with the actions.
   */
  pElem = mre_move_to_actions(pElem);

  /*  Get the number of actions in this node
   */
  nChildren = mre_xml_num_children(pElem);

  ctx.pPolicy = pPolicy;
  ctx.pSet    = mre_ensure_initset(pPolicy, nChildren);

  status = mre_xml_process_children(pElem, gather_action, &ctx);

  return MRE_SUCCEEDED(status);
}



/*-------- mre_rule_dtor --------*/
void mre_rule_dtor(
  void  *pObj
)
{
  mre_rule_t  *pRule = (mre_rule_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pRule->pCond);
  REF_CNT_OBJ_RELEASE_IF(pRule->pThenAction);
  REF_CNT_OBJ_RELEASE_IF(pRule->pElseAction);
  mre_mem_free(pRule);
}


/*-------- clear_if_logging_info --------*/
STATIC void clear_if_logging_info(
  mre_action_t  *pAction
)
{
  mre_rule_t  *pRule = (mre_rule_t *) pAction;

  clear_action_logging_info(pAction);

  pRule->last_action_time = 0;
  pRule->last_else_time   = 0;
}


/*-------- update_if_action_time --------*/
STATIC void update_if_action_time(
  mre_action_t  *pAction
)
{
  mre_rule_t  *pRule = (mre_rule_t *) pAction;

  pRule->last_action_time = mre_util_get_time();
}


/*-------- update_if_else_time --------*/
STATIC void update_if_else_time(
  mre_action_t  *pAction
)
{
  mre_rule_t  *pRule = (mre_rule_t *) pAction;

  pRule->last_else_time = mre_util_get_time();
}

/*-------- gather_condition_statistics --------*/
static boolean gather_condition_statistics(
  void   *pElem,
  void  *pCtx
)
{
  mre_condition_t const *pCond     = *((mre_condition_t const **) pElem);
  mre_dump_rule_info_t  *pRuleInfo = (mre_dump_rule_info_t *) pCtx;

  pRuleInfo->cond_val[pRuleInfo->num_conditions] = pCond->last_eval_result;
  pRuleInfo->num_conditions++;

  return (pRuleInfo->num_conditions < ARR_SIZE(pRuleInfo->cond_val));
}


/*-------- gather_if_rule_info --------*/
STATIC void gather_if_rule_info(
  mre_action_t            *pAction,
  mre_dump_ruleset_info_t *pRulesetInfo,
  mre_dump_rule_info_t    *pRuleInfo
)
{
  mre_rule_t const  *pRule = (mre_rule_t const *) pAction;
  mre_condition_t   *pCond;

  pRuleInfo->num_conditions = 0;

  pCond = pRule->pCond;
  if (pCond->pCondSet != NULL)
  {
    (void) mre_set_iterate(pCond->pCondSet, gather_condition_statistics, pRuleInfo);
  }
  else
  {
    (void) gather_condition_statistics(&pCond, pRuleInfo);
  }

  pRuleInfo->last_eval_time   = pRule->last_eval_time;
  pRuleInfo->last_action_time = pRule->last_action_time;
  pRuleInfo->eval_count       = pRule->eval_count;
  pRulesetInfo->num_rules++;
}

/*-------- if_logging_vtbl --------*/
STATIC mre_logging_vtbl_t if_logging_vtbl =
{
  clear_if_logging_info,
  update_action_eval_count,
  update_action_eval_time,
  update_if_action_time,
  update_if_else_time,
  gather_if_rule_info
};


/*-------- mre_rule_create --------*/
boolean mre_rule_create(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem,
  mre_rule_t              **ppRule
)
{
  boolean     succeeded = FALSE;
  mre_rule_t  *pRule = NULL;
  size_t      nChildren;
  size_t      childIndex;
  mre_xml_element_t const *pChild;

  /*  Check the number of child nodes.  There must be at least 2 (condition and
   *  action) but no more than 3 (condition/action/else).
   */
  nChildren = mre_xml_num_children(pElem);
  if (2 > nChildren || 3 < nChildren)
  {
    goto Done;
  }

  pRule = (mre_rule_t *) mre_action_new_with_vtbl(  sizeof(mre_rule_t),
                                                    pPolicy,
                                                    mre_if_action_execute,
                                                    mre_rule_dtor,
                                                    pElem,
                                                    &if_logging_vtbl );

  /*  First node is a condition
   */
  pChild = mre_xml_get_child(pElem, 0);
  succeeded = mre_rule_get_condition(pChild, pPolicy, &pRule->pCond);
  if (!succeeded)
  {
    goto Done;
  }

  /*  Next node is the "then" action.
   */
  pChild = mre_xml_get_child(pElem, 1);
  succeeded = mre_rule_get_single_action(pChild, pPolicy, &pRule->pThenAction);
  if (!succeeded)
  {
    goto Done;
  }

  /*  Third node, if present, is "else" action.
   */
  if (3 == nChildren)
  {
    pChild = mre_xml_get_child(pElem, 2);
    succeeded = mre_rule_get_single_action(pChild, pPolicy, &pRule->pElseAction);
  }

Done:
  if (!succeeded)
  {
    REF_CNT_OBJ_RELEASE_IF(pRule);
  }
  *ppRule = pRule;

  return succeeded;
}


/*-------- mre_ensure_actionset --------*/
mre_set_t  *mre_ensure_actionset(
  mre_policy_t  *pPolicy
)
{
  mre_set_t   *pActionSet;

  pActionSet = MRE_POLICY_ACTIONSET(pPolicy);
  if (pActionSet == NULL)
  {
    pActionSet = mre_set_refcnt_new(5, MRE_SET_NO_MAX_SIZE, NULL);
    MRE_POLICY_ACTIONSET(pPolicy) = pActionSet;
  }

  return pActionSet;
}

/*-------- gather_rule_statistics --------*/
static boolean gather_rule_statistics(
  void   *pElem,
  void  *pCtx
)
{
  mre_action_t const      *pAction      = *((mre_action_t const **) pElem);
  mre_dump_ruleset_info_t *pRulesetInfo = (mre_dump_ruleset_info_t *) pCtx;
  mre_dump_rule_info_t    *pRuleInfo;

  pRuleInfo = &pRulesetInfo->rule_info[pRulesetInfo->num_rules];

  GATHER_RULE_INFO(pAction, pRulesetInfo, pRuleInfo);

  return (pRulesetInfo->num_rules < ARR_SIZE(pRulesetInfo->rule_info));
}


/*-------- mre_rules_gather_ruleset_statistics --------*/
void mre_rules_gather_ruleset_statistics(
  mre_set_t               *pRuleset,
  void                    *pCtx,
  mre_dump_ruleset_info_t *pRulesetInfo
)
{
  pRulesetInfo->num_rules = 0;

  (void) mre_set_iterate(pRuleset, gather_rule_statistics, pRulesetInfo);
}


/*-------- mre_rules_gather_policy_statistics --------*/
void mre_rules_gather_policy_statistics(
  mre_policy_t            *pPolicy,
  mre_dump_policy_info_t  *pPolicyInfo
)
{
  pPolicyInfo->policy_num      = MRE_POLICY_NUMBER((mre_policy_t *)pPolicy);
  pPolicyInfo->policy_type     = MRE_POLICY_TYPE((mre_policy_t *)pPolicy);
  pPolicyInfo->policy_ver      = MRE_POLICY_VERSION((mre_policy_t *)pPolicy);
  pPolicyInfo->last_exec_time  = pPolicy->last_exec_time;
  pPolicyInfo->elapsed_time_ms = pPolicy->elapsed_time_ms;

  mre_rules_gather_ruleset_statistics( MRE_POLICY_ACTIONSET(pPolicy),
                                       NULL,
                                       &pPolicyInfo->ruleset_info );
}
