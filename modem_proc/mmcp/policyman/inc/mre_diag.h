#ifndef _MRE_DIAG_H_
#define _MRE_DIAG_H_

/**
  @file mre_diag.c

  @brief Policy Manager diag macros.
*/

/*
    Copyright (c) 2015 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/mre_diag.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include <sys/types.h>
#include "mre.h"

#if defined(T_WINNT) || defined (FEATURE_WINCE)
#error code not present
#endif

#define MRE_DIAG_NUM_COND_VAL    8
#define MRE_DIAG_NUM_RULE_INFO  24

PACK(struct) mre_dump_rule_info_t {
  uint32                               last_eval_time;
  uint32                               last_action_time;
  uint16                               eval_count;
  uint16                               num_conditions;
  byte                                 cond_val[MRE_DIAG_NUM_COND_VAL];
};

PACK(struct) mre_dump_ruleset_info_t {
  uint16                               num_rules;
  mre_dump_rule_info_t                 rule_info[MRE_DIAG_NUM_RULE_INFO];
};


#define MRE_DUMP_POLICY_INFO_BASE           \
  size_t                  policy_num;       \
  size_t                  policy_type;      \
  size_t                  policy_ver;       \
  uint32                  last_exec_time;   \
  uint32                  elapsed_time_ms;  \
  mre_dump_ruleset_info_t ruleset_info


typedef PACK(struct) {
  MRE_DUMP_POLICY_INFO_BASE;
} mre_dump_policy_info_t;


/*-------- mre_rules_gather_ruleset_statistics --------*/
void mre_rules_gather_ruleset_statistics(
  mre_set_t               *pRuleset,
  void                    *pCtx,
  mre_dump_ruleset_info_t *pRulesetInfo
);


/*-------- mre_rules_gather_policy_statistics --------*/
void mre_rules_gather_policy_statistics(
  mre_policy_t            *pPolicy,
  mre_dump_policy_info_t  *pPolicyInfo
);


#if defined(T_WINNT) || defined (FEATURE_WINCE)
#error code not present
#endif

#endif /* _MRE_DIAG_H_ */
