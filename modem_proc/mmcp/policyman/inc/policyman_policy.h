#ifndef _POLICYMAN_POLICY_H_
#define _POLICYMAN_POLICY_H_

/**
  @file policyman_policy.h

  @brief Interface to a policy.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_policy.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre.h"
#include "policyman_rules.h"
#include "policyman_set.h"
#include "policyman_state.h"

#include "policyman_i.h"

typedef enum
{
  POLICYMAN_POLICY_ERROR_NONE = 0,                    /** MRE Status  */

  POLICYMAN_POLICY_ERROR_FEATURE_MISMATCH,            /**< FEATURE mismatch */
} policyman_policy_error_cause;

/*-----------------------------------------------------------------------------
  Defines for policy preconditions.
-----------------------------------------------------------------------------*/

#define POLICYMAN_PRECOND_NONE        0x00000000  /* Only for initialization, not testing */
#define POLICYMAN_PRECOND_IMSI        0x00000001  /* Policyman has the IMSI from UIM */
#define POLICYMAN_PRECOND_SS          0x00000002  /* Policyman has received an SS event from CM */
#define POLICYMAN_PRECOND_SIMTYPE     0x00000004  /* SIM type is known*/
#define POLICYMAN_PRECOND_CDMASIMTYPE 0x00000008  /* CSIM type is known*/

typedef uint32  policy_attributes_t;

#define POLICY_ATTRIBUTE_NONE               0x00000000
#define POLICY_ATTRIBUTE_ENABLED            0x00000001


#define POLICYMAN_POLICY_ERROR_CAUSE(p)  (p->error_cause)

/*-------- POLICYMAN_POLICY_BASE --------*/
/**
@brief  Base structure for a policy.

This macro must be the first element of any policy structure; private elements
specific to a given policy should be added after this.
*/


#define POLICYMAN_POLICY_BASE                       \
  MRE_POLICY_BASE;                                  \
  policyman_policy_error_cause  error_cause;        \
  boolean                       immediate_full_rat


struct policyman_policy_t
{
  POLICYMAN_POLICY_BASE;
};


#define POLICY_IS_ENABLED(pPolicy)                                            \
          mre_policy_is_enabled((mre_policy_t *) pPolicy)

/*-------- policyman_policy_dtor --------*/
/**
@brief  Destructor for a base policy, called from destructors of derivied policies.
*/
void policyman_policy_dtor(
  void  *pObj
);

/*-------- policyman_policy_are_preconditions_met --------*/
/**
@brief  Returns whether the policy currently being evaluated has its
        preconditions met.
*/
boolean policyman_policy_are_preconditions_met(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState
);

/*-------- policyman_policy_new --------*/
/**
@brief  Returns pointer to a freshly allocated policy object.
*/
mre_policy_t * policyman_policy_new(
  char const              *pPolicyName,
  mre_xml_element_t const *pPolicyElem
);

/*-------- policy_handle_user_pref_update --------*/
/**
@brief  handles preference updates.
*/
boolean policy_handle_user_pref_update(
  void  *pElem,
  void  *pCtx
);


/*-------- policyman_policy_check_for_immediate_full_rat --------*/
void  policyman_policy_check_for_immediate_full_rat(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState
);

/*-------- policyman_policy_get_subs --------*/
/**
@brief  returns the subscription to which the policy belongs.
*/
sys_modem_as_id_e_type  policyman_policy_get_subs(
  mre_policy_t *pPolicy
);

/*-------- policyman_policy_run --------*/
/**
@brief  Execute a policy.
*/
boolean policyman_policy_run(
  void  *pElem,
  void  *pCtx
);


/*-------- policyman_policy_initialize --------*/
/**
@brief  Read the policy_info file for this policy and use it to initialize
        the policy.
*/
boolean policyman_policy_initialize(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState,
  policyman_set_t     *pItemSet
);


#endif /* _POLICYMAN_POLICY_H_ */
