#if !defined(_MRE_H_)
#define _MRE_H_

/**
  @file mre.h

  @brief
*/
/*
    Copyright (c) 2014-2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/mre.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include <sys/types.h>
#include "ref_cnt_obj.h"
#include "sys.h"

#include "mre_set.h"


typedef enum
{
  MRE_STATUS_SUCCESS = 0,               /**< call succeeded */

  MRE_STATUS_FAILED,
  MRE_STATUS_ERR_MALFORMED_XML,         /**< XML in config file for policy is not well-formed */
  MRE_STATUS_ERR_INVALID_ACTION,        /**< XML for action is well-formed but not correct */
  MRE_STATUS_ERR_INVALID_CONDITION,     /**< XML for condition is well-formed but not correct */
  MRE_STATUS_ERR_NOT_PRESENT,           /**< item not present */
} mre_status_t;


#define MRE_SUCCEEDED(status) (MRE_STATUS_SUCCESS == (status))
#define MRE_FAILED(status)    (!MRE_SUCCEEDED(status))

#define MRE_COND_EVAL_NEVER (-1)

typedef struct mre_xml_element_t  mre_xml_element_t;
typedef struct mre_condition_t    mre_condition_t;
typedef struct mre_action_t       mre_action_t;
typedef struct mre_timer_t        mre_timer_t;
typedef struct mre_policy_t       mre_policy_t;
typedef struct mre_policy_info_t  mre_policy_info_t;
typedef struct mre_namespace_info_t mre_namespace_info_t;
typedef struct mre_dump_ruleset_info_t  mre_dump_ruleset_info_t;
typedef struct mre_dump_rule_info_t     mre_dump_rule_info_t;



/*=============================================================================
  Policy rule conditions
=============================================================================*/

/*-------- pfnNewCondition_t --------*/
/**
@brief  Function to create a new condition from an XML element.

@param[in]  pElem       Pointer to the XML element being parsed as a condition.
@param[in]  pPolicy     Pointer to the policy being parsed.
@param[out] ppCondition Pointer to storage for a pointer to the condition parsed
                        from the XML.

@return

*/
typedef mre_status_t  (*pfnNewCondition_t)(
                          mre_xml_element_t const *pElem,
                          mre_policy_t            *pPolicy,
                          mre_condition_t         **ppCondition
                          );


/*-------- pfn_evaluate_t --------*/
/**
@brief  Evaluate a condition in the given context.

@param[in]  pCond   Pointer to the condition to evaluate.
@param[in]  pCtx    Pointer to the context in which to evaluate the condition.

@return
  TRUE or FALSE
*/
typedef boolean (*pfn_evaluate_t)(
                    mre_condition_t const *pCond,
                    void                  *pCtx
                    );


/*-------- MRE_CONDITION_BASE --------*/
/**
@brief  The basic elements of any condition.  Should be included as the first
        element of the definition of any condition.

@params _REF_CNT_OBJ  Conditions are reference-counted; this enables that.
@params pPolicy       Pointer to the policy that this condition is part of.
@params isTrue        Pointer to the function to evaluate the condition.
@params pCondSet      Pointer to a set of conditions to be evaluated by the
                      isTrue function.  This is used for compound conditions
                      like <all_of> or <any_of>.
*/
#define MRE_CONDITION_BASE    \
  _REF_CNT_OBJ;               \
  mre_policy_t    *pPolicy;   \
  pfn_evaluate_t   isTrue;    \
  mre_set_t       *pCondSet;        \
  size_t           lineNum;         \
  uint8            last_eval_result


/*-------- mre_condition_t --------*/
/**
@brief  Base (underived) condition.
*/
struct mre_condition_t
{
  MRE_CONDITION_BASE;
};


/*-------- mre_condition_map_element_t --------*/
/**
@brief  Element in an array that maps XML condition tags to the corresponding
        functions to create conditions.

@param  pTag          Pointer to the NUL-terminated ASCII string for the
                      condition.
@param  newCondition  Pointer to a function to parse the condition from an XML
                      element.
@param  preCondition  A bit-mapped specifying what preconditions are necessary
                      for evaluation of this condition.  All preConditions
                      are ORed into the overall preconditions of the policy
                      being parsed, and rules (except for "always_run" rules)
                      will not be evaluated unless the current preconditions
                      are a superset of the policy preconditions.
*/
typedef struct
{
  char const        *pTag;
  pfnNewCondition_t pfnNew;
  uint32            precondition;
} mre_condition_map_element_t;




/*-------- mre_condition_dtor --------*/
/**
@brief  Destructor for the MRE_CONDITION_BASE portion of a condition.
*/
void mre_condition_dtor(
  void  *pObj
  );



/*=============================================================================
  Policy rule actions
=============================================================================*/


/*-------- pfnNewAction_t --------*/
/**
@brief  Function to create a new action from an XML element.

@param[in]  pElem     Pointer to the XML element being parsed as an action.
@param[in]  pPolicy   Pointer to the policy being parsed.
@param[out] ppAcion   Pointer to storage for a pointer to the action parsed
                      from the XML.
@return

*/
typedef mre_status_t  (*pfnNewAction_t)(
                          mre_xml_element_t const *pElem,
                          mre_policy_t            *pPolicy,
                          mre_action_t            **ppAction
                          );

/*-------- pfn_execute_t --------*/
/**
@brief  Execute an action in the given context.

@param[in]  pAction   Pointer to the action to execute.
@param[in]  pCtx      Pointer to the context in which to execute the action.

*/
typedef boolean (*pfn_execute_t)(
                    mre_action_t const  *pAction,
                    void                *pCtx
                    );


/*-------- pfn_update_logging_t --------*/
/**
@brief  Function that updates specific logging information for an action.
*/
typedef void    (*pfn_update_logging_t)(
  mre_action_t  *pAction
);


/*-------- pfn_gather_info_t --------*/
/**
@brief  Function to gather rule logging information for an action.
*/
typedef void    (*pfn_gather_info_t)(
  mre_action_t            *pAction,
  mre_dump_ruleset_info_t *pRulesetInfo,
  mre_dump_rule_info_t    *pRuleInfo
);



/*-------- mre_logging_vtbl_t --------*/
/**
@brief  Vtable of functions for logging actions.
*/
typedef struct
{
  pfn_update_logging_t  clear_info;
  pfn_update_logging_t  update_eval_count;
  pfn_update_logging_t  update_eval_time;
  pfn_update_logging_t  update_action_time;
  pfn_update_logging_t  update_else_time;
  pfn_gather_info_t     gather_rule_info;
} mre_logging_vtbl_t;


#define LOG_VTBL(pAction)             pAction->pLogVtbl
#define CLEAR_LOGGING_INFO(pAction)   LOG_VTBL(pAction)->clear_info((mre_action_t *) pAction)
#define UPDATE_EVAL_COUNT(pAction)    LOG_VTBL(pAction)->update_eval_count((mre_action_t *) pAction)
#define UPDATE_EVAL_TIME(pAction)     LOG_VTBL(pAction)->update_eval_time((mre_action_t *) pAction)
#define UPDATE_ACTION_TIME(pAction)   LOG_VTBL(pAction)->update_action_time((mre_action_t *) pAction)
#define UPDATE_ELSE_TIME(pAction)     LOG_VTBL(pAction)->update_else_time((mre_action_t *) pAction)
#define GATHER_RULE_INFO(a, rsi, ri)  LOG_VTBL(pAction)->gather_rule_info((mre_action_t *) a, rsi, ri)


/*-------- MRE_ACTION_BASE --------*/
/**
@params _REF_CNT_OBJ  Actions are reference-counted; this enables that.
@params pPolicy       Pointer to the policy that this actions is part of.
@params execute       Pointer to the function to call to execute the condition.
@params pLogVtbl        Pointer to logging vtable for this action.
@params last_eval_time  Time at which this action was last executed.
@params eval_count      Number of times this actions has been executed.
*/
#define MRE_ACTION_BASE     \
  _REF_CNT_OBJ;             \
  mre_policy_t    *pPolicy; \
  pfn_execute_t        execute;         \
  mre_logging_vtbl_t  *pLogVtbl;        \
  uint32               last_eval_time;  \
  size_t               eval_count;      \
  size_t               lineNum



/*-------- mre_action_t --------*/
/**
@brief  Base (underived) action.
*/
struct mre_action_t
{
  MRE_ACTION_BASE;
};


/*-------- mre_action_map_element_t --------*/
/**
@brief  Element in an array that maps XML action tags to the corresponding
        functions to create actions.

@param  pTag        Pointer to the NUL-terminated ASCII string for the
                    action.
@param  newAction   Pointer to a function to parse the action from an XML
                    element.
*/
typedef struct
{
  char const      *pTag;
  pfnNewAction_t  pfnNew;
} mre_action_map_element_t;


/*-------- mre_action_dtor --------*/
/**
@brief  Destructor for the MRE_ACTION_BASE portion of an action.
*/
void mre_action_dtor(
  void  *pObj
  );


/*=============================================================================
  Policy precondition definitions
=============================================================================*/

typedef struct
{
  char const  *pTag;
  uint32      mask;
} mre_precond_map_element_t;

/*  Predefined precondition masks
 */
#define MRE_PRECOND_NONE    0x00000000  /* No preconditions for this rule. */
#define MRE_PRECOND_POLICY  0xffffffff  /* Use overall policy preconditions for
                                         * this rule. (Default).
                                         */


/*=============================================================================
  Policy definitions
=============================================================================*/

/*-------- MRE_POLICY_BASE --------*/
/**
@brief  Base structure for a policy.

This macro must be the first element of any policy structure; private elements
specific to a given policy should be added after this.

@param  _REF_CNT_OBJ    Policies are reference-counted objects; this enables that.
@param  pName           NUL-terminated ASCII name of this policy.
@param  pInfo           Pointer to private-to-MRE information about the policy.
@param  last_exec_time  Time (low 32 bits from time_get_ms) at which policy last executed.
@param  elapsed_time_ms Number of milliseconds taken by last policy execution.
*/
#define MRE_POLICY_BASE          \
  _REF_CNT_OBJ;                  \
  char          const *pName;    \
  sys_modem_as_id_e_type   subs; \
  mre_policy_info_t       *pInfo;  \
  uint32                   last_exec_time;  \
  uint32                   elapsed_time_ms



/*-------- mre_policy_t --------*/
/**
@brief  Base (underived) policy.
*/
struct mre_policy_t
{
  MRE_POLICY_BASE;
};


/*-------- pfnNewPolicy_t --------*/
/**
@brief  Callback used by the parser to request a new instance of a policy
        for the given name.
*/
typedef mre_policy_t  *(*pfnNewPolicy_t)(
                          char const              *pPolicyName,
                          mre_xml_element_t const *pPolicyElem
                          );

/*=============================================================================
  Subscription (SUBS) Accessors
=============================================================================*/

#define POLICY_SUBS(pPolicy)       (((mre_policy_t *)(pPolicy))->subs)
#define POLICY_SUBS_SAFE(pPolicy)  ((NULL == (void *)(pPolicy)) ? SYS_MODEM_AS_ID_NONE : ((mre_policy_t *)(pPolicy))->subs)
#define ACTION_SUBS(pAction)       (POLICY_SUBS((pAction)->pPolicy))
#define CONDITION_SUBS(pCond)      (POLICY_SUBS((pCond)->pPolicy))

/*=============================================================================
  Public APIs to parse and execute policies.
=============================================================================*/

/*-------- mre_policy_create --------*/
/**
@brief  Parse an XML file and return a policy that corresponds to it.

@param[in]  pPolicyPath     EFS path to the policy file.
@param[in]  subsId          The context to read the policy file (AS_ID)
@param[in]  pCondTable      Pointer to a table of condition elements that map
                            condition tags to the functions to create them.
                            The table is terminated with an element with a NULL tag.
@param[in]  pActionTable    Pointer to a table of action elements that map
                            action tags to the functions to create them.
                            The table is terminated with an element with a NULL tag.
@param[in]  pPrecondTable   Pointer to a table of precondition elements that map
                            a precondition tag to its corresponding bitmask.
                            This pointer may be NULL if you don't use preconditions.
@param[in]  pfnNewPolicy    Pointer to a callback function that creates a new
                            instance of the policy whose name is passed to it.
                            This must be initialized as a reference-counted object.
                            The name comes from the "name" attribute of the
                            policy in the XML.
@param[out] ppPolicy        Pointer to storage for a pointer to the resulting
                            policy.

@return
  MRE_STATUS_SUCCESS            if policy creation is successful
  MRE_STATUS_ERR_NOT_PRESENT    if pPolicyPath is not present in EFS
  MRE_STATUS_ERR_MALFORMED_XML  if there was an error either parsing the XML
                                or the XML does not match the schema for
                                the policy
*/
mre_status_t mre_policy_create(
  char                          const  *pPolicyPath,
  sys_modem_as_id_e_type                subsId,
  mre_condition_map_element_t   const  *pCondTable,
  mre_action_map_element_t      const  *pActionTable,
  mre_precond_map_element_t     const  *pPrecondTable,
  pfnNewPolicy_t                        pfnNewPolicy,
  mre_policy_t                        **ppPolicy
);


/*-------- mre_policy_init --------*/
/**
@brief  Function to initialize a policy using the context passed.  This executes
        the initial actions specified in the <initial> block of the XML.

@param  pPolicy         Pointer to the policy to initialize.
@param  pNamespaceInfo  Pointer to a mre_namespace_info_t that should be used
                        by *all* policies of the client.  It is the client's
                        responsibility to release any non-NULL named object
                        sets contained in this structure upon termination.
@param  pCtx            Pointer to an execution context for your rules.  This
                        will be passed to all condition evaluation and action
                        execution functions called when evaluating the policy.

@return
  None
*/
void mre_policy_init(
  mre_policy_t  *pPolicy,
  mre_namespace_info_t  *pNamespaceInfo,
  void          *pCtx
  );


/*-------- mre_policy_execute --------*/
/**
@brief  Function to execute the rules for a policy using the context passed.

@param  pPolicy       Pointer to the policy to execute.
@param  precondsMet   The bitmask of preconditions (as defined by your module)
                      that have currently been met.  The MRE will determine
                      which rules to run based on whether all preconditions
                      of the policy have been met.
@param  pCtx          Pointer to an execution context for your rules.  This
                      will be passed to all condition evaluation and action
                      execution functions called when evaluating the policy.

@return
  None
*/
void mre_policy_execute(
  mre_policy_t  *pPolicy,
  uint32        precondsMet,
  void          *pCtx
  );


/*-------- mre_policy_is_enabled --------*/
/**
@brief  Return whether the policy is enabled (i.e. was successfully initialized).

Note that it is NOT necessary to call this in order to decide whether you can
call mre_policy_execute() - MRE will handle that situation for you.  This API
is present in case you have code that executes off your task thread and needs
to know if the policy is enabled.

@param  pPolicy   Pointer to the policy to check.

@return
  TRUE  if the policy was successfully initialized
  FALSE otherwise
*/
boolean mre_policy_is_enabled(
  mre_policy_t  *pPolicy
  );


/*-------- mre_policy_are_preconds_met --------*/
/**
@brief  Return whether preconditions for the policy have been met.

Note that it is NOT necessary to call this in order to decide whether you can
call mre_policy_execute() - MRE will handle that situation for you.  This API
is present in case you have code that executes off your task thread and needs
to know if the policy's preconditions have been met.

@param  pPolicy     Pointer to the policy whose preconditions should be checked.
@param  precondsMet The bitmask of preconditions that have actually been met.

@return
  TRUE  if all preconditions required for the policy have been met.
  FALSE otherwise
*/
boolean mre_policy_are_preconds_met(
  mre_policy_t  *pPolicy,
  uint32        precondsMet
  );


/*-------- mre_policy_dtor --------*/
/**
@brief  Destructor for the base portion of the MRE policy.  Call this from
        the destructor for your specific policy.

@param  pObj  Pointer to the policy.

@return
  None
*/
void mre_policy_dtor(
  void  *pObj
  );



#ifdef __cplusplus
}
#endif

#endif /* _MRE_H_ */
