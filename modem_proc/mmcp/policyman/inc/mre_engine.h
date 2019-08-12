#ifndef _MRE_ENGINE_H_
#define _MRE_ENGINE_H_

/**
  @file mre_engine.h

  @brief
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/mre_engine.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre.h"
#include "mre_namedobj.h"


#define MRE_POLICY_VTBL_BASE                                                      \
boolean (*pfnPrecondMet)(mre_policy_t *pPolicy, void *pCtx)


typedef struct
{
  MRE_POLICY_VTBL_BASE;
} mre_policy_vtbl_t;



/*-------- mre_policy_info_t --------*/
/**
@brief  Policy info structure. This holds all the private-to-MRE information
        about the policy.

@param  pName           Pointer to NUL-terminated ASCII string that is the policy name.
@param  pContext        Pointer to whatever context is necessary for evaluation
                        of the conditions and actions for this policy.
@param  preconditions   This is the set of preconditions that have been satisfied
                        for this policy.
@param  enabled         Specifies whether the policy is enabled - it may be
                        disabled for various reasons like initialization failure.
@param  contEval        Rule evaluatoin should continue despite a rule firing
                        (i.e. a <continue /> was executed).
@param  ruleNum         Number of the rule currently being executed (used if
                        there is no rule ID).
@param  pInitSet        Pointer to the set of actions to execute when the policy
                        is initialized.
@param  pRuleSet        Pointer to the set of rules executed whenever the rules

--  The following are logged each time a policy is executed and can be used
    to identify which policy is running.  They are parsed from the policy_ver
    attribute of the policy XML.
@param  policy_num      The policy number.
@param  policy_type     This is a sub-type of the policy specified by policy_num.
@param  policy_ver      This is the version of the policy.  Increment it
                        whenever the policy changes.
                        are evaluated.
*/

struct  mre_policy_info_t
{
  mre_policy_vtbl_t           const *pVtbl;
  uint32                             preconditions;
  uint32                             currentPreconds;
  mre_condition_map_element_t const *pCondTable;
  mre_action_map_element_t    const *pActionTable;
  mre_precond_map_element_t   const *pPrecondTable;
  mre_set_t                         *pInitSet;
  mre_set_t                         *pActionSet;
  mre_set_t                         *pNamedObjs;
  mre_namespace_info_t              *pNamespaceInfo;
  size_t                             policy_num;
  size_t                             policy_type;
  size_t                             policy_ver;
  size_t                             changeList;
  size_t                             ruleLevel;
  boolean                            precondsAreMet;
  boolean                            enabled;
  boolean                            contEval;
  boolean                            stopEval;
};



#define MRE_POLICY_INFO(p)              ((p)->pInfo)
#define MRE_POLICY_VTBL(p)              (MRE_POLICY_INFO(p)->pVtbl)
#define MRE_POLICY_PRECONDITIONS(p)     (MRE_POLICY_INFO(p)->preconditions)
#define MRE_POLICY_CURRENT_PRECONDS(p)  (MRE_POLICY_INFO(p)->currentPreconds)
#define MRE_POLICY_CONDTBL(p)           (MRE_POLICY_INFO(p)->pCondTable)
#define MRE_POLICY_ACTTBL(p)            (MRE_POLICY_INFO(p)->pActionTable)
#define MRE_POLICY_PRECONDTBL(p)        (MRE_POLICY_INFO(p)->pPrecondTable)
#define MRE_POLICY_INITSET(p)           (MRE_POLICY_INFO(p)->pInitSet)
#define MRE_POLICY_ACTIONSET(p)         (MRE_POLICY_INFO(p)->pActionSet)
#define MRE_POLICY_PNAMEDOBJS(p)        (MRE_POLICY_INFO(p)->pNamedObjs)
#define MRE_POLICY_NAMESPACE_INFO(p)    (MRE_POLICY_INFO(p)->pNamespaceInfo)
#define MRE_POLICY_NUMBER(p)            (MRE_POLICY_INFO(p)->policy_num)
#define MRE_POLICY_TYPE(p)              (MRE_POLICY_INFO(p)->policy_type)
#define MRE_POLICY_VERSION(p)           (MRE_POLICY_INFO(p)->policy_ver)
#define MRE_POLICY_CHANGELIST(p)        (MRE_POLICY_INFO(p)->changeList)
#define MRE_POLICY_PRECONDSAREMET(p)    (MRE_POLICY_INFO(p)->precondsAreMet)
#define MRE_POLICY_ENABLED(p)           (MRE_POLICY_INFO(p)->enabled)
#define MRE_POLICY_CONTEVAL(p)          (MRE_POLICY_INFO(p)->contEval)
#define MRE_POLICY_STOPEVAL(p)          (MRE_POLICY_INFO(p)->stopEval)

#define MRE_POLICY_RULELEVEL(p)         (MRE_POLICY_INFO(p)->ruleLevel)
#define MRE_POLICY_SET_TOP_LEVEL(p)     MRE_POLICY_RULELEVEL(p) = 0
#define MRE_POLICY_DESCEND_LEVEL(p)     ++(MRE_POLICY_RULELEVEL(p))
#define MRE_POLICY_ASCEND_LEVEL(p)      --(MRE_POLICY_RULELEVEL(p))
#define MRE_POLICY_AT_TOP_LEVEL(p)      (0 == MRE_POLICY_RULELEVEL(p))

#endif /* _MRE_ENGINE_H_ */
