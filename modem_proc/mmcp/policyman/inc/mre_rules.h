#ifndef _MRE_RULES_H_
#define _MRE_RULES_H_

/**
  @file mre_rules.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/mre_rules.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/


#include "mre.h"


/*-----------------------------------------------------------------------------
  Definition of a rule
-----------------------------------------------------------------------------*/

/*  A rule is just an <if> action, with a condition and <then> and <else> actions.
 */

typedef struct
{
  MRE_ACTION_BASE;

  mre_condition_t  *pCond;
  mre_action_t     *pThenAction;
  mre_action_t     *pElseAction;
  uint32            preconds;
  uint32            id;
  uint32            last_action_time;
  uint32            last_else_time;
} if_action_t;

/*  So as not to have to change all references to mre_rule_t...
 */
typedef if_action_t mre_rule_t;


typedef struct
{
  mre_policy_t  *pPolicy;
  mre_set_t     *pSet;
} policy_set_ctx_t;


/*=============================================================================
  APIs to operate on rulesets
=============================================================================*/


/*-------- mre_rule_add_condition --------*/
mre_status_t mre_rule_add_condition(
  mre_xml_element_t const *pElem,
  void                    *pCtx
);


/*-------- mre_condset_add --------*/
boolean mre_condset_add(
  mre_set_t               **ppCondSet,
  mre_xml_element_t const  *pElem
);


/*-------- mre_condset_evaluate --------*/
boolean mre_condset_evaluate(
  mre_set_t const *pCondSet,
  void            *pCtx
);


/*-------- mre_initset_add --------*/
boolean mre_initset_add(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem
);


/*-------- mre_actionset_execute --------*/
void mre_actionset_execute(
  mre_set_t const *pActionSet,
  void            *pCtx
);


/*-------- mre_ruleset_add_rule --------*/
boolean mre_ruleset_add_rule(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem
);


/*-------- mre_rules_continue_evaluation --------*/
/**
@brief  Continue rule evaluation despite having all rule conditions met.

This is specifically for the implementation of the <continue> action, so that
rule evaluation can continue despite having executed actions.

@param

@return

*/
void mre_rules_continue_evaluation(
  mre_policy_t  *pPolicy
);


/*-------- mre_rule_create --------*/
boolean mre_rule_create(
  mre_policy_t             *pPolicy,
  mre_xml_element_t const  *pElem,
  mre_rule_t              **ppRule
);


/*-------- mre_rule_evaluate --------*/
void mre_rule_evaluate(
  mre_rule_t        *pRule,
  mre_policy_t      *pPolicy,
  void              *pCtx
);


/*-------- mre_ensure_actionset --------*/
mre_set_t  *mre_ensure_actionset(
  mre_policy_t  *pPolicy
);


/*-------- mre_rule_get_actionset --------*/
boolean mre_rule_get_actionset(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_set_t              **ppActionSet
);


/*-------- mre_rule_preconditions_met --------*/
boolean mre_rule_preconditions_met(
  mre_rule_t  const *pRule
);


/*-------- mre_rule_add_action_to_set --------*/
boolean mre_rule_add_action_to_set(
  mre_xml_element_t const   *pElem,
  mre_policy_t              *pPolicy,
  mre_set_t                 *pActionSet
);


/*=============================================================================
  APIs to get conditions and actions
=============================================================================*/

/*-------- mre_rule_get_condition --------*/
boolean mre_rule_get_condition(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCond
);


/*-------- mre_rule_get_single_action --------*/
boolean mre_rule_get_single_action(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);


/*=============================================================================
  Needed in mre_lang.c
=============================================================================*/

boolean execute_action(
  void  *pElem,
  void  *pCtx
);

#endif /* _MRE_RULES_H_ */
