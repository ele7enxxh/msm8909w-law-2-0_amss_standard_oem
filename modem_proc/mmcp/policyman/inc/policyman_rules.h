#ifndef _POLICYMAN_RULES_H_
#define _POLICYMAN_RULES_H_

/**
  @file policyman_rules.h

  @brief  Utility APIs to help in interpreting components of rules.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_rules.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_rules.h"
#include "mre_lang.h"
#include "policyman_i.h"


/*-----------------------------------------------------------------------------
  Definitions for base policyman condition objects
-----------------------------------------------------------------------------*/

#define POLICYMAN_CONDITION_BASE  \
  MRE_CONDITION_BASE;             \
  policyman_subs_t  pmSubsId

struct policyman_condition_t
{
  POLICYMAN_CONDITION_BASE;
};

#define policyman_condition_dtor  mre_condition_dtor

typedef boolean (*pfnEvaluateCondForSubs)(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  sys_modem_as_id_e_type       subs
);


/*-----------------------------------------------------------------------------
  Definitions for base policyman action objects
-----------------------------------------------------------------------------*/

#define POLICYMAN_ACTION_BASE  \
  MRE_ACTION_BASE;             \
  policyman_subs_t  pmSubsId

struct policyman_action_t
{
  POLICYMAN_ACTION_BASE;
};

#define policyman_action_dtor       mre_action_dtor
#define policyman_actionset_execute mre_actionset_execute

typedef boolean (*pfnExecuteActionForSubs)(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subs
);


/*-----------------------------------------------------------------------------
  Other common condition definitions
-----------------------------------------------------------------------------*/
typedef struct
{
  POLICYMAN_CONDITION_BASE;

  mre_set_t         *pMccSet;
  char const        *pSetName;
} policyman_mcc_condition_t;


/*-----------------------------------------------------------------------------
  Definition of a policyman rule
-----------------------------------------------------------------------------*/
#define policyman_rule_t  mre_rule_t

#endif /* _POLICYMAN_RULES_H_ */
