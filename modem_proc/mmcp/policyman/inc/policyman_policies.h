#ifndef _POLICYMAN_POLICIES_H_
#define _POLICYMAN_POLICIES_H_

/**
  @file policyman_policies.h

  @brief Interface to policies.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_policies.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_policy.h"

typedef size_t  policyman_policy_enum_ctx;

/*-------- policyman_get_namespace_info --------*/
mre_namespace_info_t  *policyman_get_namespace_info(
  void
);

/*-------- policyman_policies_init --------*/
/**
@brief  Initialize the device info from policyman.  Private function.

@return
  None.
*/
void policyman_policies_init(
  policyman_state_t *pState,
  policyman_set_t   *pItemSet
);

/*-------- policyman_set_policy_initialized --------*/
void policyman_set_policy_initialized(
  boolean  is_initialized
);


/*-------- policyman_sub_policies_init --------*/
/**
@brief  Initialize the policy subsystem.  Private function.

@return
  None.
*/
void policyman_sub_policies_init(
  policyman_state_t *pState,
  policyman_set_t   *pItemSet
);


/*-------- policyman_policies_per_subs_init --------*/
/**
@brief  Initialize the policy files.  Private function.

@return
  None
*/
void  policyman_policies_per_subs_init(
  policyman_state_t      *pState,
  policyman_set_t        *pItemSet,
  sys_modem_as_id_e_type  subsId
);


/*-------- policyman_policies_init_succeeded --------*/
/**
@brief  Return whether the initialization of the policy succeeded.
        This is used as in unit test in order to determine if
        the XML was successfully parsed into a policy.
*/
boolean policyman_policies_init_succeeded(
  void
);


/*-------- policyman_policies_deinit --------*/
/**
@brief  Deinitialize the policy subsystem.  Private function.

@return
  None
*/
void policyman_policies_deinit(
  void
);


/*-------- policyman_policies_per_subs_deinit --------*/
/**
@brief  Deinitialize the policy subsystem for a specific
        subscription.  Private function.

@return
  None
*/
void policyman_policies_per_subs_deinit(
  sys_modem_as_id_e_type subsId
);


/*-------- policyman_policies_run_policy_check --------*/
/**
@brief  Run a policy check and collect updates into the itemset passed.

@param[in]  pState    Pointer to current state.
@param[in]  pItemSet  The itemset to which to add items.

@return

*/
void policyman_policies_run_policy_check(
  policyman_state_t       *pState,
  policyman_set_t         *pItemSet
);

/*-------- policyman_policy_handle_user_pref_update --------*/
/**
@brief  Get policies to handle phone update for user preference change

@return
  None
*/
void policyman_policy_handle_user_pref_update(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
);

/*-------- policyman_is_suspended --------*/
/**
@brief  Check if Policy Manager is in a suspended state.

@return
  TRUE if state is suspended, FALSE otherwise
*/
boolean policyman_is_suspended(
  void
);

/*-------- policyman_policies_file_enum_init --------*/
void policyman_policies_file_enum_init(
  policyman_policy_enum_ctx *pCtx
);

/*-------- policyman_policies_file_enum_next --------*/
boolean policyman_policies_file_enum_next(
  policyman_policy_enum_ctx *pCtx,
  char                      *pFileName,
  size_t                     nameLength
);

/*-------- policyman_evaluate_cond --------*/
/**
@brief  Evaluates a condition with a subs attribute.
*/
boolean policyman_evaluate_cond(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs       pfnEvaluate
);

/*-------- policyman_condition_new --------*/
policyman_condition_t * policyman_condition_new(
  size_t                   condSize,
  mre_policy_t            *pPolicy,
  pfn_evaluate_t           pEvaluate,
  destructor_ptr           pDtor,
  policyman_subs_t         pmSubsId,
  mre_xml_element_t const *pElem
);

/*-------- POLICYMAN_CONDITION_NEW --------*/
#define POLICYMAN_CONDITION_NEW(condition_type, p_policy, p_eval, p_dtor, subs, p_elem) \
  (condition_type *)policyman_condition_new( sizeof(condition_type), \
                                             (p_policy), \
                                             (p_eval), \
                                             (p_dtor), \
                                             (subs), \
                                             (p_elem) )

/*-------- policyman_execute_action --------*/
/**
@brief  Executes an action with a subs attribute.
*/
boolean policyman_execute_action(
  policyman_action_t const *pAction,
  void                     *pCtx,
  pfnExecuteActionForSubs   pfnExecute
);

/*-------- policyman_action_new --------*/
policyman_action_t * policyman_action_new(
  size_t                   actionSize,
  mre_policy_t            *pPolicy,
  pfn_execute_t            pExecute,
  destructor_ptr           pDtor,
  policyman_subs_t         pmSubsId,
  mre_xml_element_t const *pElem
);

/*-------- POLICYMAN_ACTION_NEW --------*/
#define POLICYMAN_ACTION_NEW(action_type, p_policy, p_exec, p_dtor, subs, p_elem) \
  (action_type *)policyman_action_new( sizeof(action_type), \
                                       (p_policy), \
                                       (p_exec), \
                                       (p_dtor), \
                                       (subs), \
                                       (p_elem) )

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/*-------- policyman_policy_load_global --------*/
void policyman_policy_load_global(
  policyman_state_t        *pState,
  policyman_set_t          *pItemSet  
);

/*-------- policyman_policy_init_is_complete --------*/
void policyman_policy_init_is_complete(
  void 
);


/*-------- policyman_resume_internal --------*/
size_t policyman_resume_internal(
  policyman_client_t  client
);

/*-------- policyman_policies_gather_statistics --------*/
void policyman_policies_gather_statistics(
  size_t                 policy_idx,
  pm_dump_policy_info_t *pPolicyInfo
);


#endif /* _POLICYMAN_POLICIES_H_ */
