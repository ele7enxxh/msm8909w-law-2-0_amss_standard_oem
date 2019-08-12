#ifndef _MRE_LANG_H_
#define _MRE_LANG_H_

/**
  @file mre_lang.h

  @brief  Code for the "language" used in the Modem Rules Engine XML.
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/mre_lang.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_xml.h"


/*=============================================================================
  Conditions
=============================================================================*/

mre_condition_t * mre_condition_new(
  size_t                    condSize,
  mre_policy_t             *pPolicy,
  pfn_evaluate_t            pEvaluate,
  destructor_ptr            pDtor,
  mre_xml_element_t const  *pElem
);

mre_status_t mre_condition_true_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
);


mre_status_t mre_boolean_test_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
);


mre_status_t mre_condition_not_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
);


mre_status_t mre_condition_or_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
);

mre_status_t mre_condition_and_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
);


/*=============================================================================
  Actions
=============================================================================*/

/*-------- mre_execute_action --------*/
boolean mre_execute_action(
  mre_action_t  *pAction,
  void          *pCtx
);

/*-----------------------------------------------------------------------------
  Logging functions
-----------------------------------------------------------------------------*/
void clear_action_logging_info(
  mre_action_t  *pAction
);

void update_action_eval_count(
  mre_action_t  *pAction
);

void update_action_eval_time(
  mre_action_t  *pAction
);


/*-----------------------------------------------------------------------------
  This is defined in _lang.c but used in _rules.c.
-----------------------------------------------------------------------------*/
// TODO: Refactor _rules.c - most everything there should probably be in either _lang.c or _engine.c

boolean mre_if_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
);


/*-----------------------------------------------------------------------------
  Action creators
-----------------------------------------------------------------------------*/
mre_action_t * mre_action_new_with_vtbl(
  size_t                    actionSize,
  mre_policy_t             *pPolicy,
  pfn_execute_t             pExecute,
  destructor_ptr            pDtor,
  mre_xml_element_t const  *pElem,
  mre_logging_vtbl_t       *pVtbl
);

mre_action_t * mre_action_new(
  size_t                    actionSize,
  mre_policy_t             *pPolicy,
  pfn_execute_t             pExecute,
  destructor_ptr            pDtor,
  mre_xml_element_t const  *pElem
);

mre_status_t mre_continue_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);


mre_status_t mre_boolean_define_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);


mre_status_t mre_boolean_set_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

mre_status_t mre_if_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

mre_status_t  mre_select_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

mre_status_t  mre_stop_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

#endif /* _MRE_LANG_H_ */
