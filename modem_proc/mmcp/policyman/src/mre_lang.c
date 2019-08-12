/**
  @file mre_lang.c

  @brief  Code for the "language" used in policyman XML.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/mre_lang.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_dbg.h"
#include "mre_engine.h"
#include "mre_lang.h"
#include "policyman_diag.h"
#include "mre_rules.h"
#include "mre_set.h"
#include "mre_util.h"
#include "policyman_diag.h"

#include <stringl/stringl.h>



typedef struct
{
  MRE_NAMED_OBJECT_BASE;

  uint32  value;
} mre_boolean_t;




/*=============================================================================
  Public utility functions
=============================================================================*/

/*-------- mre_boolean_get_value_from_set --------*/
mre_status_t mre_boolean_get_value_from_set(
  char const          *pName,
  boolean             *pValue,
  mre_set_t           *pSet
)
{
  mre_status_t  status = MRE_STATUS_ERR_NOT_PRESENT;
  mre_boolean_t *pBoolean;
  boolean       value = FALSE;

  pBoolean = (mre_boolean_t *) mre_named_object_find_in_set( pName,
                                        MRE_NAMED_BOOLEAN,
                                                             pSet );
  if (pBoolean != NULL)
  {
    value = (boolean) pBoolean->value;
    status = MRE_STATUS_SUCCESS;
  }

  *pValue = value;
  return status;
}


/*-------- mre_boolean_get_value --------*/
mre_status_t mre_boolean_get_value(
  char const          *pName,
  boolean             *pValue,
  mre_policy_t        *pPolicy
)
{
  mre_status_t   status = MRE_STATUS_ERR_NOT_PRESENT;
  mre_boolean_t *pBoolean;

  pBoolean = (mre_boolean_t *) mre_named_object_find( pName,
                                                      MRE_NAMED_BOOLEAN,
                                                      pPolicy );
  if (NULL != pBoolean)
  {
    status = MRE_STATUS_SUCCESS;
    *pValue = (boolean) pBoolean->value;
  }

  return status;
}


/*=============================================================================
  Private utility functions
=============================================================================*/

/*-------- mre_str_to_boolean --------*/
static mre_status_t mre_str_to_boolean(
  char const  *pStr,
  uint32      *pBoolean
)
{
  mre_status_t  status = MRE_STATUS_ERR_MALFORMED_XML;

  if (pStr == NULL)
  {
    goto Done;
  }

  if      (strcasecmp(pStr, "true") == 0)
  {
    *pBoolean = TRUE;
  }
  else if (strcasecmp(pStr, "false") == 0)
  {
    *pBoolean = FALSE;
  }
  else
  {
    goto Done;
  }

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}




/*=============================================================================
  Conditions
=============================================================================*/

mre_condition_t * mre_condition_new(
  size_t                 condSize,
  mre_policy_t          *pPolicy,
  pfn_evaluate_t         pEvaluate,
  destructor_ptr            pDtor,
  mre_xml_element_t const  *pElem
)
{
  mre_condition_t    *pCondition;
  destructor_ptr      pDestruct  = mre_condition_dtor;

  pCondition = (mre_condition_t *)mre_mem_alloc(condSize);

  if (pDtor != NULL)
  {
    pDestruct = pDtor;
  }

  ref_cnt_obj_init(pCondition, pDestruct);

  pCondition->isTrue           = pEvaluate;
  pCondition->pPolicy          = pPolicy;
  pCondition->lineNum          = mre_xml_get_line_number(pElem);
  pCondition->last_eval_result = MRE_COND_EVAL_NEVER;

  return pCondition;
}

/*-----------------------------------------------------------------------------
  <true>
-----------------------------------------------------------------------------*/

typedef struct
{
  MRE_CONDITION_BASE;
} mre_true_condition_t;

static boolean mre_condition_true_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  MRE_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "condition <true> returns 1",
                  CONDITION_SUBS(pCondition),
                  LINE_NUM(pCondition) );

  return TRUE;
}


mre_status_t mre_condition_true_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
)
{
  *ppCondition = mre_condition_new( sizeof(mre_true_condition_t),
                                    pPolicy,
                                    mre_condition_true_evaluate,
                                    NULL,
                                    pElem );

  MRE_MSG_HIGH_1(SUBS_PREFIX "mre_condition_true_new returned SUCCESS", POLICY_SUBS(pPolicy));

  return MRE_STATUS_SUCCESS;
}


/*-----------------------------------------------------------------------------
  <boolean_is_true>
-----------------------------------------------------------------------------*/


typedef struct
{
  MRE_CONDITION_BASE;

  char const  *pBooleanName;
} mre_boolean_test_t;


/*-------- mre_boolean_test_dtor --------*/
static void mre_boolean_test_dtor(
  void  *pObj
)
{
  mre_boolean_test_t  *pMe = (mre_boolean_test_t *) pObj;

  MRE_MEM_FREE_IF(pMe->pBooleanName);
  mre_action_dtor(pMe);
}


/*-------- mre_boolean_test_evaluate --------*/
static boolean mre_boolean_test_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  mre_boolean_test_t  *pCond = (mre_boolean_test_t *) pCondition;
  mre_boolean_t       *pBoolean;
  boolean                   result;

  pBoolean = (mre_boolean_t *) mre_named_object_find( pCond->pBooleanName,
                                                      MRE_NAMED_BOOLEAN,
                                                      pCondition->pPolicy );
  result =    pBoolean != NULL
          &&  pBoolean->value != 0;

  MRE_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "<boolean_test> returns %d",
                  CONDITION_SUBS(pCond),
                  LINE_NUM(pCond),
                  result );
  MRE_UT_MSG_S4(  SUBS_PREFIX SRC_LINE "<boolean_test name='%s'> returns %d",
                 CONDITION_SUBS(pCond),
                  LINE_NUM(pCond),
                 pCond->pBooleanName,
                 result );

  return result;
}


/*-------- mre_boolean_test_new --------*/
mre_status_t mre_boolean_test_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  mre_status_t         status       = MRE_STATUS_ERR_INVALID_CONDITION;
  mre_boolean_test_t  *pCondition   = NULL;
  char const          *pStr;
  char                *pBooleanName = NULL;

  /*  Get the name of the boolean to test.
   */
  pStr = mre_xml_get_attribute(pElem, "name");
  if (pStr == NULL)
  {
    MRE_UT_MSG_1(SUBS_PREFIX "<boolean_test> requires a 'name' attribute", POLICY_SUBS(pPolicy));
    goto Done;
  }

  pBooleanName = mre_str_dup(pStr);

  /*  Create the condition.
   */
  pCondition = (mre_boolean_test_t *)mre_condition_new( sizeof(mre_boolean_test_t),
                                                        pPolicy,
                                                        mre_boolean_test_evaluate,
                                                        mre_boolean_test_dtor,
                                                        pElem );

  pCondition->pBooleanName = pBooleanName;

  status = MRE_STATUS_SUCCESS;

Done:
  *ppCondition = (mre_condition_t *) pCondition;

  return status;
}



/*=============================================================================
  Compound conditions (those that modify other conditions)
=============================================================================*/

/*-----------------------------------------------------------------------------
  Utility functions for compound conditions
-----------------------------------------------------------------------------*/


/*-------- mre_collect_conditions --------*/
static mre_set_t *mre_collect_conditions(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy
)
{
  size_t     nConds;
  policy_set_ctx_t  ctx;
  mre_status_t      status = MRE_STATUS_ERR_MALFORMED_XML;

  ctx.pPolicy = pPolicy;
  ctx.pSet    = NULL;

  nConds = mre_xml_num_children(pElem);
  if (0 != nConds)
  {
    ctx.pSet = mre_set_refcnt_new(nConds, nConds, NULL);

    status = mre_xml_process_children(pElem, mre_rule_add_condition, &ctx);
    if (MRE_FAILED(status))
  {
      REF_CNT_OBJ_RELEASE_IF(ctx.pSet);
    }
  }
  else
    {
    MRE_UT_MSG_S2( SUBS_PREFIX "<%s> must have sub-elements",
                       POLICY_SUBS(pPolicy),
                   mre_xml_get_tag(pElem));
  }

  return ctx.pSet;
}


/*-------- mre_compound_condition_new --------*/
static mre_status_t mre_compound_condition_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  pfn_evaluate_t          pfnEvaluate,
  mre_condition_t         **ppCondition
)
{
  mre_status_t               status     = MRE_STATUS_ERR_INVALID_CONDITION;
  mre_condition_t *pCondition = NULL;
  mre_set_t       *pCondSet;

  /*  Get the subelements of this condition.
   */
  pCondSet = mre_collect_conditions(pElem, pPolicy);
  if (pCondSet == NULL)
  {
    MRE_UT_MSG_S2( SUBS_PREFIX "<%s> unable to collect conditions",
                   POLICY_SUBS(pPolicy),
                   mre_xml_get_tag(pElem) );
    goto Done;
  }

  /*  Create the condition
   */
  pCondition = (mre_condition_t *)mre_condition_new( sizeof(mre_condition_t),
                                                              pPolicy,
                                                              pfnEvaluate,
                                                              mre_condition_dtor,
                                                              pElem );

  pCondition->pCondSet = pCondSet;

  status = MRE_STATUS_SUCCESS;

Done:
  *ppCondition = (mre_condition_t *) pCondition;
  MRE_UT_MSG_S3( SUBS_PREFIX "mre_condition_%s_new status %d",
                 POLICY_SUBS(pPolicy),
                 mre_xml_get_tag(pElem),
                 status );

  return status;
}


/*-----------------------------------------------------------------------------
  <not>
-----------------------------------------------------------------------------*/

/*-------- mre_condition_not_evaluate --------*/
static boolean mre_condition_not_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  boolean                   result;

  result = !mre_condset_evaluate(pCondition->pCondSet, pCtx);

  MRE_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "condition <not> returns %d",
                  CONDITION_SUBS(pCondition),
                  LINE_NUM(pCondition),
                  result);
  return result;
}


/*-------- mre_condition_not_new --------*/
mre_status_t mre_condition_not_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  return mre_compound_condition_new( pElem,
                                     pPolicy,
                                     mre_condition_not_evaluate,
                                     ppCondition );
}


/*-----------------------------------------------------------------------------
  <or>
-----------------------------------------------------------------------------*/

typedef struct
{
  boolean condsetIsTrue;
  void    *pCtx;
} or_condition_ctx;


/*-------- or_condition_test --------*/
static boolean or_condition_test(
  void  *pElem,
  void  *pCtx
)
{
  mre_condition_t const *pCond = *((mre_condition_t const **) pElem);
  or_condition_ctx      *pOrCtx = (or_condition_ctx *) pCtx;

  /*  If the condition is true, we set the result of the test to TRUE, but
   *  return FALSE to stop execution.
   */
  if (pCond->isTrue(pCond, pOrCtx->pCtx))
  {
    pOrCtx->condsetIsTrue = TRUE;
    return FALSE;
  }

  /*  Otherwise, return TRUE to continue the iteration.
   */
  return TRUE;
}


/*-------- mre_condset_any_true --------*/
boolean mre_condset_any_true(
  mre_set_t const *pCondSet,
  void            *pCtx
)
{
  or_condition_ctx  ctx;
  
  ctx.condsetIsTrue = FALSE;
  ctx.pCtx          = pCtx;

  mre_set_iterate( (mre_set_t *) pCondSet, or_condition_test, &ctx);

  return ctx.condsetIsTrue;
}



/*-------- mre_condition_or_evaluate --------*/
static boolean mre_condition_or_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  boolean                         result;

  result = mre_condset_any_true(pCondition->pCondSet, pCtx);

  MRE_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "condition <any_of> returns %d",
                  CONDITION_SUBS(pCondition),
                  LINE_NUM(pCondition),
                  result );
  return result;
}


/*-------- mre_condition_or_new --------*/
mre_status_t mre_condition_or_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  return mre_compound_condition_new( pElem,
                                     pPolicy,
                                     mre_condition_or_evaluate,
                                     ppCondition );
}


/*-----------------------------------------------------------------------------
  <and>
-----------------------------------------------------------------------------*/

/*-------- mre_condition_and_evaluate --------*/
static boolean mre_condition_and_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  boolean                   result;

  result = mre_condset_evaluate(pCondition->pCondSet, pCtx);

  MRE_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "condition <all_of> returns %d",
                  CONDITION_SUBS(pCondition),
                  LINE_NUM(pCondition),
                  result );
  return result;
}


/*-------- mre_condition_and_new --------*/
mre_status_t mre_condition_and_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  return mre_compound_condition_new( pElem,
                                     pPolicy,
                                     mre_condition_and_evaluate,
                                     ppCondition );
}


/*=============================================================================
  Actions
=============================================================================*/


/*-------- mre_execute_action --------*/
boolean mre_execute_action(
  mre_action_t  *pAction,
  void          *pCtx
)
{
  UPDATE_EVAL_COUNT(pAction);
  UPDATE_EVAL_TIME(pAction);
  
  return pAction->execute(pAction, pCtx);
}


/*-------- clear_action_logging_info --------*/
void clear_action_logging_info(
  mre_action_t  *pAction
)
{
  pAction->last_eval_time = 0;
  pAction->eval_count     = 0;
}


/*-------- update_action_eval_count --------*/
void update_action_eval_count(
  mre_action_t  *pAction
)
{
  ++pAction->eval_count;
}


/*-------- update_action_eval_time --------*/
void update_action_eval_time(
  mre_action_t  *pAction
)
{
  pAction->last_eval_time = mre_util_get_time();
}


/*-------- update_action_action_time --------*/
STATIC void update_action_action_time(
  mre_action_t  *pAction
)
{
}


/*-------- update_action_else_time --------*/
STATIC void update_action_else_time(
  mre_action_t *pRule
)
{
}

/*-------- gather_action_rule_info --------*/
STATIC void gather_action_rule_info(
  mre_action_t            *pAction,
  mre_dump_ruleset_info_t *pRulesetInfo,
  mre_dump_rule_info_t    *pRuleInfo
)
{
}



/*-------- action_logging_vtbl --------*/
STATIC mre_logging_vtbl_t action_logging_vtbl =
{
  clear_action_logging_info,
  update_action_eval_count,
  update_action_eval_time,
  update_action_action_time,
  update_action_else_time,
  gather_action_rule_info
};

/*-------- mre_action_new_with_vtbl --------*/
mre_action_t * mre_action_new_with_vtbl(
  size_t                 actionSize,
  mre_policy_t          *pPolicy,
  pfn_execute_t          pExecute,
  destructor_ptr            pDtor,
  mre_xml_element_t const  *pElem,
  mre_logging_vtbl_t       *pVtbl
)
{
  mre_action_t       *pAction;
  destructor_ptr      pDestruct = mre_action_dtor;

  pAction = (mre_action_t *)mre_mem_alloc(actionSize);

  if (pDtor != NULL)
  {
    pDestruct = pDtor;
  }

  ref_cnt_obj_init(pAction, pDestruct);

  pAction->execute = pExecute;
  pAction->pPolicy = pPolicy;
  pAction->lineNum  = mre_xml_get_line_number(pElem);
  pAction->pLogVtbl = pVtbl;
  
  CLEAR_LOGGING_INFO(pAction);

  return pAction;
}


/*-------- mre_action_new --------*/
mre_action_t  *mre_action_new(
  size_t                    actionSize,
  mre_policy_t             *pPolicy,
  pfn_execute_t             pExecute,
  destructor_ptr            pDtor,
  mre_xml_element_t const  *pElem
)
{
  return mre_action_new_with_vtbl(actionSize,
                                  pPolicy,
                                  pExecute,
                                  pDtor,
                                  pElem,
                                  &action_logging_vtbl );
}


/*-----------------------------------------------------------------------------
  <continue>
-----------------------------------------------------------------------------*/

/*-------- mre_continue_execute --------*/
static boolean mre_continue_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  mre_rules_continue_evaluation(pAction->pPolicy);
  MRE_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "<continue> encountered - continuing rule evaluation",
                  ACTION_SUBS(pAction),
                  LINE_NUM(pAction) );
  return TRUE;
}


/*-------- mre_continue_new --------*/
mre_status_t mre_continue_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
)
{
  *ppAction = mre_action_new( sizeof(mre_action_t),
                              pPolicy,
                              mre_continue_execute,
                              mre_action_dtor,
                              pElem );

  MRE_MSG_HIGH_1(SUBS_PREFIX "mre_continue_new returned status 1", POLICY_SUBS(pPolicy));

  return MRE_STATUS_SUCCESS;
}


/*-----------------------------------------------------------------------------
  <boolean_define>
-----------------------------------------------------------------------------*/


typedef struct
{
  MRE_ACTION_BASE;

  char const    *pBooleanName;  /* Name of the boolean to create */
  mre_named_obj_namespace_t ns;
  uint32        initialValue;
} boolean_define_action_t;


/*-------- mre_define_boolean_execute --------*/
static boolean mre_define_boolean_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  boolean_define_action_t *pBooleanAction = (boolean_define_action_t *) pAction;
  mre_policy_t      *pPolicy = pAction->pPolicy;
  mre_boolean_t     *pBoolean;
  boolean                 succeeded;

  if (pPolicy == NULL)
  {
    MRE_MSG_ERROR_0("Internal error - executing <define_boolean> with no policy");
    return FALSE;
  }

  pBoolean = (mre_boolean_t *) mre_mem_alloc(sizeof(mre_boolean_t));
  ref_cnt_obj_init(pBoolean, mre_named_object_dtor);
  mre_named_object_init( (mre_named_object_t *) pBoolean,
                          pBooleanAction->pBooleanName,
                         MRE_NAMED_BOOLEAN );
  pBoolean->value = pBooleanAction->initialValue;

  succeeded = mre_named_object_add( (mre_named_object_t *) pBoolean,
                                     pBooleanAction->ns,
                                    pPolicy );
  ref_cnt_obj_release(pBoolean);

  return succeeded;
}


/*-------- mre_define_boolean_dtor --------*/
static void mre_define_boolean_dtor(
  void  *pObj
)
{
  boolean_define_action_t  *pMe = (boolean_define_action_t *) pObj;

  MRE_MEM_FREE_IF(pMe->pBooleanName);
  mre_action_dtor(pMe);
}


/*-------- mre_boolean_define_new --------*/
mre_status_t mre_boolean_define_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t            status = MRE_STATUS_ERR_MALFORMED_XML;
  char const              *pName;
  mre_named_obj_namespace_t ns;
  char const              *pInitial;
  uint32                  initial = FALSE;
  boolean_define_action_t *pAction;

  pName = mre_xml_get_attribute(pElem, "name");
  if (pName == NULL)
  {
    MRE_UT_MSG_1(SUBS_PREFIX "<boolean_define> 'name' attribute required", POLICY_SUBS(pPolicy));
    goto Done;
  }

  ns = mre_named_object_get_namespace(pElem);

  pInitial = mre_xml_get_attribute(pElem, "initial");
  status = mre_str_to_boolean(pInitial, &initial);
  if (MRE_FAILED(status))
  {
    MRE_UT_MSG_1(SUBS_PREFIX "<boolean_define> valid values for 'initial' attribute are 'true' and 'false'", POLICY_SUBS(pPolicy));
    goto Done;
  }

  /*  Allocate and initialize the timer create action.
   */
  pAction = (boolean_define_action_t *)mre_action_new( sizeof(boolean_define_action_t),
                                                       pPolicy,
                                                       mre_define_boolean_execute,
                                                        mre_define_boolean_dtor,
                                                        pElem );

  pAction->pBooleanName = mre_str_dup(pName);
  pAction->ns = ns;
  pAction->initialValue = initial;

  *ppAction = (mre_action_t*) pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}



/*-----------------------------------------------------------------------------
  <boolean_set>
-----------------------------------------------------------------------------*/

typedef struct
{
  MRE_ACTION_BASE;

  char const  *pBooleanName;  /* Name of the boolean to set */
  uint32      value;
  mre_set_t   *pConditions;
} boolean_set_action_t;


/*-------- mre_boolean_set_action_dtor --------*/
void mre_boolean_set_action_dtor(
  void  *pObj
)
{
  boolean_set_action_t  *pAction = (boolean_set_action_t *) pObj;

  MRE_MEM_FREE_IF(pAction->pBooleanName);
  REF_CNT_OBJ_RELEASE_IF(pAction->pConditions);
  mre_action_dtor(pAction);
}


/*-------- mre_boolean_set_execute --------*/
static boolean mre_boolean_set_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  boolean_set_action_t  *pAct = (boolean_set_action_t *) pAction;
  mre_boolean_t   *pBoolean;

  pBoolean = (mre_boolean_t *) mre_named_object_find( pAct->pBooleanName,
                                                      MRE_NAMED_BOOLEAN,
                                                      pAction->pPolicy );
  if (pBoolean == NULL)
  {
    MRE_UT_MSG_S3( SUBS_PREFIX SRC_LINE "<boolean_set> no boolean named %s",
                   ACTION_SUBS(pAct),
                   LINE_NUM(pAct),
                   pAct->pBooleanName );
    MRE_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "<boolean_set> unable to find boolean",
                    ACTION_SUBS(pAct),
                    LINE_NUM(pAct) );
    return FALSE;
  }

  if (pAct->pConditions != NULL)
  {
    MRE_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "<boolean_set> evaluating conditions for boolean",
                    ACTION_SUBS(pAct),
                    LINE_NUM(pAct) );
    pBoolean->value = mre_condset_evaluate(pAct->pConditions, pCtx);
  }
  else
  {
    pBoolean->value = pAct->value;
  }

  MRE_UT_MSG_S4( SUBS_PREFIX SRC_LINE "<boolean_set> boolean %s set to %d",
                 ACTION_SUBS(pAct),
                 LINE_NUM(pAct),
                 pBoolean->pName,
                 pBoolean->value );
  MRE_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "<boolean_set> boolean set to %d",
                  ACTION_SUBS(pAct),
                  LINE_NUM(pAct),
                  pBoolean->value );

  return TRUE;
}


/*-------- mre_boolean_set_new --------*/
mre_status_t mre_boolean_set_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t          status = MRE_STATUS_ERR_INVALID_ACTION;
  char const            *pName;
  char const            *pValue;
  uint32                value = FALSE;
  size_t                nConds;
  mre_set_t             *pCondSet = NULL;
  boolean_set_action_t  *pAction = NULL;

  pName = mre_xml_get_attribute(pElem, "name");
  if (pName == NULL)
  {
    MRE_UT_MSG_0("<boolean_set>: 'name' attribute required");
    goto Done;
  }

  /*  See if there are children of this element (conditions).
   */
  nConds = mre_xml_num_children(pElem);

  /*  see if there is a "value" attribute on this element.
   */
  pValue = mre_xml_get_attribute(pElem, "value");

  /*  You can't have both a value and conditions (children).
   */
  if (nConds != 0 && pValue != NULL)
  {
    MRE_MSG_HIGH_1(SUBS_PREFIX "<boolean_set> can't have both a 'value' attribute and conditions", POLICY_SUBS(pPolicy));
    goto Done;
  }

  /*  You must have either a value or conditions (children).
   */
  if (nConds == 0 && pValue == NULL)
  {
    MRE_MSG_HIGH_1(SUBS_PREFIX "<boolean_set> must have either a 'value' attribute or conditions", POLICY_SUBS(pPolicy));
    goto Done;
  }

  /*  If we have conditions, collect them; otherwise, get the value.
   */
  if (nConds != 0)
  {
    MRE_UT_MSG_S2( SUBS_PREFIX "<boolean_set> collecting conditions for boolean %s",
                   POLICY_SUBS(pPolicy),
                   pName );

    pCondSet = mre_collect_conditions(pElem, pPolicy);
    if (pCondSet == NULL)
    {
      MRE_UT_MSG_1(SUBS_PREFIX "<boolean_set> unable to collect conditions", POLICY_SUBS(pPolicy));
      goto Done;
    }
  }
  else
  {
    status = mre_str_to_boolean(pValue, &value);
    if (MRE_FAILED(status))
    {
      MRE_UT_MSG_2( SUBS_PREFIX "<boolean_set> 'value' attribute of 'true' or 'false' required for boolean",
                    POLICY_SUBS(pPolicy),
                    pName );
      goto Done;
    }
  }

  pAction = (boolean_set_action_t *)mre_action_new( sizeof(boolean_set_action_t),
                                                    pPolicy,
                                                    mre_boolean_set_execute,
                                                     mre_boolean_set_action_dtor,
                                                     pElem );

  pAction->pBooleanName = mre_str_dup(pName);
  pAction->value = value;
  pAction->pConditions = pCondSet;

  *ppAction = (mre_action_t *) pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  MRE_MSG_HIGH_2( SUBS_PREFIX "mre_boolean_set_new returned status %d",
                  POLICY_SUBS(pPolicy),
                  status );
  return status;
}


/*=============================================================================
  Action to run an embedded rule.
=============================================================================*/


/*-------- mre_condition_is_true --------*/
STATIC boolean mre_condition_is_true(
  mre_condition_t *pCond,
  void            *pCtx
)
{

  /*  If the condition is true, we continue execution; otherwise we stop
   *  evaluating conditions in the set.
   */

  pCond->last_eval_result = pCond->isTrue(pCond, pCtx);

  return pCond->last_eval_result;
}


/*-------- condition_is_true --------*/
STATIC boolean condition_is_true(
  void  *pElem,
  void  *pCtx
)
{
  mre_condition_t *pCond = *((mre_condition_t **) pElem);

  return mre_condition_is_true(pCond, pCtx);
}



/*-------- mre_condset_evaluate --------*/
boolean mre_condset_evaluate(
  mre_set_t const   *pCondSet,
  void              *pCtx
)
{
  return mre_set_iterate((mre_set_t *) pCondSet, condition_is_true, pCtx);
}


/*-------- mre_execute_if --------*/
STATIC void mre_execute_if(
  mre_rule_t        *pRule,
  mre_policy_t      *pPolicy,
  void              *pCtx
)
{
  boolean condsTrue;

  MRE_POLICY_CONTEVAL(pPolicy) = FALSE;

  condsTrue = mre_condition_is_true(pRule->pCond, pCtx);

  if (condsTrue)
  {
    MRE_MSG_HIGH_1(SRC_LINE "executing actions for <if>/<rule>", LINE_NUM(pRule));
    UPDATE_ACTION_TIME(pRule);

    MRE_POLICY_DESCEND_LEVEL(pPolicy);
    execute_action(&pRule->pThenAction, pCtx);
    MRE_POLICY_ASCEND_LEVEL(pPolicy);
  }
  else if (NULL != pRule->pElseAction)
  {
    MRE_MSG_HIGH_1(SRC_LINE "executing 'else' actions for <if>/<rule>", LINE_NUM(pRule));
    UPDATE_ELSE_TIME(pRule);

    MRE_POLICY_DESCEND_LEVEL(pPolicy);
    execute_action(&pRule->pElseAction, pCtx);
    MRE_POLICY_ASCEND_LEVEL(pPolicy);
  }
  else if (MRE_POLICY_AT_TOP_LEVEL(pPolicy))
  {
    MRE_POLICY_CONTEVAL(pPolicy) = TRUE;
  }
}


/*-------- mre_if_action_execute --------*/
boolean mre_if_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  if_action_t   *pAct = (if_action_t *) pAction;
  mre_policy_t  *pPolicy = pAct->pPolicy;
  boolean       contEval;

  /*  Top level if behaves a bit differently than embedded if, in that these
   *  are the only ifs to which preconditions apply.
   */
  if (MRE_POLICY_AT_TOP_LEVEL(pPolicy))
  {
    if (!mre_rule_preconditions_met(pAct))
    {
      return TRUE;
    }
  }

  MRE_MSG_HIGH_1(SRC_LINE "evaluating conditions for <if>/<rule>", LINE_NUM(pAct));

  /*  Embedded 'if' actions are just like rules, except for one thing: the fact
   *  that the actions are executed does *not* mean that rule evaluation should
   *  stop.  If the enclosing top-level rule has a <continue />, an 'if' action
   *  should not override this.  On the other hand, if the 'if' does a <continue />
   *  *that* should be honored.
   *
   *  This is enforced by checking the 'continue' result after evaluating the
   *  'if'.  If it is TRUE, we leave it as-is.  If it is FALSE, we restore the
   *  state prior to evaluating the 'if'.
   */

  contEval = MRE_POLICY_AT_TOP_LEVEL(pPolicy)? FALSE : MRE_POLICY_CONTEVAL(pPolicy);

  mre_execute_if(pAct, pPolicy, pCtx);

  if (!MRE_POLICY_CONTEVAL(pPolicy))
  {
    MRE_POLICY_CONTEVAL(pPolicy) = contEval;
  }

  /*  Now set whether execution is to continue based on whether a <stop> has
   *  executed; otherwise, use the "continue evaluating" setting.
   */
  if (MRE_POLICY_STOPEVAL(pPolicy))
  {
    MRE_POLICY_CONTEVAL(pPolicy) = FALSE;
    contEval = FALSE;
  }
  else
  {
    contEval = MRE_POLICY_AT_TOP_LEVEL(pPolicy)? MRE_POLICY_CONTEVAL(pPolicy) : TRUE;
  }

  return contEval;
}


/*-------- mre_lookup_precond --------*/
STATIC boolean mre_lookup_precond(
  mre_precond_map_element_t const *pPrecondTbl,
  char                      const *pStr,
  uint32                          *pMask
)
{
  while (NULL != pPrecondTbl->pTag)
  {
    if (strcasecmp(pStr, pPrecondTbl->pTag) == 0)
    {
      *pMask |= pPrecondTbl->mask;
      return TRUE;
    }
    ++pPrecondTbl;
  }

  return FALSE;
}


/*-------- mre_parse_rule_preconds --------*/
STATIC boolean mre_parse_rule_preconds(
  mre_precond_map_element_t const *pPrecondTbl,
  uint32                          *pPreconds,
  char                      const *pStr
)
{
  char    token[100];
  uint32  mask = MRE_PRECOND_NONE;
  boolean noneSpecified = FALSE;

  while (mre_get_token(&pStr, token, sizeof(token)))
  {
    if (strcasecmp(token, "none") == 0)
    {
      noneSpecified = TRUE;
      if (MRE_PRECOND_NONE != mask)
      {
        MRE_MSG_ERROR_0("'none' precondition combined with other preconditions");
        return FALSE;
      }
    }
    else if (noneSpecified)
    {
      MRE_MSG_ERROR_0("'none' precondition combined with other preconditions");
      return FALSE;
    }
    else if (strcasecmp(token, "policy") == 0)
    {
      mask = MRE_PRECOND_POLICY;
    }
    else if (NULL == pPrecondTbl)
    {
      MRE_MSG_ERROR_0("'precond' attribute specified but no precondition table passed");
      return FALSE;
    }
    else if (!mre_lookup_precond(pPrecondTbl, token, &mask))
    {
      MRE_MSG_ERROR_0("Unsupported precondition in rule");
      return FALSE;
    }
  }

  *pPreconds = mask;
  return TRUE;
}


/*-------- mre_get_rule_preconditions --------*/
STATIC boolean mre_get_rule_preconditions(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem,
  uint32                  *pPreconds
)
{
  boolean     succeeded = TRUE;
  boolean     precondFound = FALSE;
  char const  *pStr;
  uint32      preconds;

  /*  Default preconditions (in absence of override) is to use overall
   *  policy preconditions.
   */
  preconds = MRE_PRECOND_POLICY;

  /*  If there us a "precond" attribute, we'll use that.
   */
  pStr = mre_xml_get_attribute(pElem, "precond");
  if (NULL != pStr)
  {
    precondFound = TRUE;
    succeeded = mre_parse_rule_preconds(MRE_POLICY_PRECONDTBL(pPolicy), &preconds, pStr);
    if (!succeeded)
    {
      goto Done;
    }
  }

  /*  If the deprecated "always_run" attribute is present *and* if there
   *  are no preconditions set, set preconditions to none.
   */
  pStr = mre_xml_get_attribute(pElem, "always_run");
  if (NULL != pStr)
  {
    if (precondFound)
    {
      MRE_MSG_ERROR_0("Rule has both 'always_run' and 'precond' attributes");
      succeeded = FALSE;
    }
    else if (strcasecmp(pStr, "true") == 0)
    {
      preconds = MRE_PRECOND_NONE;
    }
  }

Done:
  *pPreconds = preconds;
  return succeeded;
}


/*-------- mre_if_action_new --------*/
mre_status_t mre_if_action_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
)
{
  boolean       succeeded;
  mre_rule_t    *pRule = NULL;
  if_action_t   *pAct;
  uint32         preconds;

  /*  Get preconditions, bail if invalid.
   */
  succeeded = mre_get_rule_preconditions(pPolicy, pElem, &preconds);
  if (!succeeded)
  {
    goto Done;
  }

  /*  Create the rule from the node.
   */
  succeeded = mre_rule_create(pPolicy, pElem, &pRule);
  if (succeeded)
  {
    pRule->preconds = preconds;
    *ppAction = (mre_action_t *) pRule;
  }

Done:
  return succeeded ? MRE_STATUS_SUCCESS : MRE_STATUS_ERR_MALFORMED_XML;
}


/*=============================================================================
  <select> action
=============================================================================*/

/*-------- mre_select_case_t --------*/
typedef struct
{
  _REF_CNT_OBJ;
  
  mre_policy_t    *pPolicy;
  mre_condition_t *pCond;
  mre_action_t    *pAction;
  size_t           lineNum;
} mre_select_case_t;


/*-------- select_case_dtor --------*/
STATIC void select_case_dtor(
  void  *pObj
)
{
  mre_select_case_t *pMe = (mre_select_case_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pMe->pCond);
  REF_CNT_OBJ_RELEASE_IF(pMe->pAction);
  mre_mem_free(pMe);
}

/*-------- mre_select_action_t --------*/
typedef struct
{
  MRE_ACTION_BASE;

  mre_set_t *pCaseSet;
} mre_select_action_t;


/*-------- select_action_dtor --------*/
STATIC void select_action_dtor(
  void  *pObj
)
{
  mre_select_action_t *pMe = (mre_select_action_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pMe->pCaseSet);
  mre_action_dtor(pMe);
}


/*-------- case_execute --------*/
STATIC boolean case_execute(
  void  *pElem,
  void  *pCtx
)
{
  mre_select_case_t *pCase = *((mre_select_case_t **) pElem);
  boolean            caseWasSelected;

  
  MRE_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "Evaluating <select> case",
                  POLICY_SUBS(pCase->pPolicy),
                  LINE_NUM(pCase) );
  caseWasSelected = mre_condition_is_true(pCase->pCond, pCtx);
  if (caseWasSelected)
  {
    MRE_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "Executing <select> case",
                    POLICY_SUBS(pCase->pPolicy),
                    LINE_NUM(pCase) );
    mre_execute_action(pCase->pAction, pCtx);
  }

  return !MRE_POLICY_STOPEVAL(pCase->pPolicy) && !caseWasSelected;
}


/*-------- select_action_execute --------*/
STATIC boolean select_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  mre_select_action_t const *pAct = (mre_select_action_t const *) pAction;

  return mre_set_iterate(pAct->pCaseSet, case_execute, pCtx);
}


/*-------- is_valid_case --------*/
STATIC boolean is_valid_case(
  mre_xml_element_t const *pElem
)
{
  /*  Case statements must have 2 children
   */
  {
    size_t                   nChildren;

    nChildren = mre_xml_num_children(pElem);
    if (nChildren != 2)
    {
      MRE_MSG_HIGH_2( SRC_LINE "In <case>: expected condition and action, but got %d elements instead",
                      mre_xml_get_line_number(pElem),
                      nChildren );
      return FALSE;
    }
  }

  /*  Case statements are named "case".
   */
  {
    char const  *pStr;

    pStr = mre_xml_get_tag(pElem);
    if (0 != strcasecmp(pStr, "case"))
    {
      MRE_MSG_HIGH_1(SRC_LINE "<select> statement can only contain <case> elements",
                     mre_xml_get_line_number(pElem) );
      return FALSE;
    }
  }

  return TRUE;
}


/*-------- get_case --------*/
STATIC mre_status_t get_case(
  mre_xml_element_t const  *pElem,
  void                    *pCtx
)  
{
  mre_status_t             status = MRE_STATUS_ERR_MALFORMED_XML;
  policy_set_ctx_t        *pCaseCtx = (policy_set_ctx_t *) pCtx;
  boolean                  succeeded = FALSE;
  mre_select_case_t       *pCase = NULL;
  mre_xml_element_t const *pChild;

  if (!is_valid_case(pElem))
  {
    goto Done;
  }

  pCase = (mre_select_case_t *) mre_mem_alloc(sizeof(*pCase));
  ref_cnt_obj_init(pCase, select_case_dtor);
  LINE_NUM(pCase) = mre_xml_get_line_number(pElem);
  
  /*  First child of a case is the condition
   */
  pChild = mre_xml_get_child(pElem, 0);
  succeeded = mre_rule_get_condition(pChild, pCaseCtx->pPolicy, &pCase->pCond);
  if (!succeeded)
  {
    goto Done;
  }

  /*  Second child of a case is the action
   */
  pChild = mre_xml_get_child(pElem, 1);
  succeeded = mre_rule_get_single_action(pChild, pCaseCtx->pPolicy, &pCase->pAction);
  if (!succeeded)
  {
    goto Done;
  }

  pCase->pPolicy = pCaseCtx->pPolicy;
  mre_set_append(pCaseCtx->pSet, &pCase);
  ref_cnt_obj_release(pCase);
  status = MRE_STATUS_SUCCESS;

Done:
  if (!succeeded)
  {
    REF_CNT_OBJ_RELEASE_IF(pCase);
  }

  return status;
}


/*-------- mre_select_action_new --------*/
mre_status_t  mre_select_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t         status = MRE_STATUS_ERR_MALFORMED_XML;
  mre_select_action_t *pAction = NULL;
  size_t               numCases;
  policy_set_ctx_t     ctx;

  numCases = mre_xml_num_children(pElem);
  if (0 == numCases)
  {
    MRE_MSG_ERROR_1(SRC_LINE "No cases in <select> statement", mre_xml_get_line_number(pElem));
    goto Done;
  }

  pAction = (mre_select_action_t *) mre_action_new( sizeof(*pAction),
                                                    pPolicy,
                                                    select_action_execute,
                                                    select_action_dtor,
                                                    pElem );

  pAction->pCaseSet = mre_set_refcnt_new(numCases, numCases, NULL);
  ctx.pPolicy  = pPolicy;
  ctx.pSet = pAction->pCaseSet;

  status = mre_xml_process_children(pElem, get_case, &ctx);

  *ppAction = (mre_action_t *) pAction;

Done:
  if (MRE_FAILED(status))
  {
    REF_CNT_OBJ_RELEASE_IF(pAction);
  }
  
  return status;
}


/*=============================================================================
  <stop> action
=============================================================================*/

/*-------- stop_action_execute --------*/
STATIC boolean stop_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  MRE_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "stopping policy execution",
                  POLICY_SUBS(pAction->pPolicy),
                  LINE_NUM(pAction) );

  MRE_POLICY_STOPEVAL(pAction->pPolicy) = TRUE;

  return FALSE;
}

/*-------- mre_stop_action_new --------*/
mre_status_t  mre_stop_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
)
{
  *ppAction = mre_action_new( sizeof(mre_action_t),
                              pPolicy,
                              stop_action_execute,
                              mre_action_dtor,
                              pElem );

  return MRE_STATUS_SUCCESS;
}
