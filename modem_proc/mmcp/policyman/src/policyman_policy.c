/**
  @file policyman_policy.c

  @brief Public interface to Policy Manager
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_policy.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre.h"
#include "mre_efs.h"
#include "mre_engine.h"
#include "mre_set.h"
#include "mre_util.h"

#include "policyman_call_events.h"
#include "policyman_call_mode.h"
#include "policyman_cfgitem.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_fullrat_config.h"
#include "policyman_network_events.h"
#include "policyman_plmn.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_rat_capability.h"
#include "policyman_rat_order.h"
#include "policyman_scan_optimization.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_svc_mode.h"
#include "policyman_task.h"
#include "policyman_tech.h"
#include "policyman_timer.h"
#include "policyman_ue_mode.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_voice_domain_pref.h"
#include "policyman_volte.h"
#include "policyman_xml.h"
#include "policyman_scan_optimization.h"



#include <stringl/stringl.h>


/*-------- policyman_policy_get_subs --------*/
sys_modem_as_id_e_type  policyman_policy_get_subs(
  mre_policy_t *pPolicy
)
{
  policyman_policy_t *pSubsPolicy;

  pSubsPolicy = (policyman_policy_t *)pPolicy;
  return pSubsPolicy->subs;
}

/*-------- policyman_policy_dtor --------*/
void policyman_policy_dtor(
  void  *pObj
)
{
  mre_policy_dtor(pObj);
}


/*-------- policy_disable--------*/
STATIC void policy_disable(
  policyman_policy_t              *pPolicy,
  policyman_policy_error_cause     reason
)
{
  MRE_POLICY_ENABLED((mre_policy_t *) pPolicy) = FALSE;
  POLICYMAN_POLICY_ERROR_CAUSE(pPolicy) = reason;
}



/*-------- policyman_policy_initialize --------*/
/**
@brief  Read the policy_info file for this policy and use it to initialize
        the policy.

@param[in]  pPolicy   Pointer to the policy.

@return
  TRUE if policy is initialized
*/
boolean policyman_policy_initialize(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState,
  policyman_set_t     *pItemSet 
)
{
  policy_execute_ctx_t  ctx;

  /*  If we're passed a NULL pointer for the state, we're just running
   *  an XML validation test.  Finish here.
   */
  if (pState == NULL)               { goto Done; }

  /*  Configure the policy, setting any initial configuration items.
   */
  ctx.pItemSet = pItemSet;
  ctx.pState   = pState;
  ctx.asubs_id = pPolicy->subs;

  mre_policy_init( (mre_policy_t *) pPolicy,
        policyman_get_namespace_info(),
                   &ctx );

  if (!policyman_feature_is_supported_by_hardware(pItemSet))
  {
    POLICYMAN_MSG_ERROR_2( SUBS_PREFIX "========== Mismatch in XML and device capabilities, trm cap = 0x%x ========",
                           POLICY_SUBS(pPolicy),
                           policyman_state_get_trm_cap() );

    policy_disable(pPolicy, POLICYMAN_POLICY_ERROR_FEATURE_MISMATCH);
    goto Done;
  }

Done:

  return TRUE;
}

/*-------- policyman_policy_new --------*/
mre_policy_t * policyman_policy_new(
  char              const *pPolicyName,
  mre_xml_element_t const *pPolicyElem
)
{
  policyman_policy_t  *pPolicy;


  pPolicy = policyman_mem_alloc(sizeof(policyman_policy_t));
  ref_cnt_obj_init(pPolicy, policyman_policy_dtor);

  POLICYMAN_MSG_HIGH_0("Policyman: created policy object");
  
  return (mre_policy_t *) pPolicy;
}

/*-------- policyman_policy_are_preconditions_met --------*/
boolean policyman_policy_are_preconditions_met(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState
)
{
  return mre_policy_are_preconds_met( (mre_policy_t *) pPolicy,
                                      policyman_state_get_subs_precond(pState, pPolicy->subs) );
}


/*-------- policy_execute --------*/
STATIC void policy_execute(
  policyman_policy_t    *pPolicy,
  policy_execute_ctx_t  *pCtx
)
{
  uint32                precondsMet;

  precondsMet = policyman_state_get_subs_precond(pCtx->pState, POLICY_SUBS(pPolicy));
  pCtx->asubs_id = POLICY_SUBS(pPolicy);

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX "----------- Started executing policy version %d.%d.%d (%d) -----------",
                        POLICY_SUBS(pPolicy),
                        MRE_POLICY_NUMBER((mre_policy_t *)pPolicy),
                        MRE_POLICY_TYPE((mre_policy_t *)pPolicy),
                        MRE_POLICY_VERSION((mre_policy_t *)pPolicy),
                        MRE_POLICY_CHANGELIST((mre_policy_t *)pPolicy) );

  if (POLICYMAN_POLICY_ERROR_CAUSE(pPolicy) == POLICYMAN_POLICY_ERROR_FEATURE_MISMATCH)
  {
    POLICYMAN_MSG_ERROR_2( SUBS_PREFIX "Policyman feature does not match TRM capabilities of 0x%x",
                           POLICY_SUBS(pPolicy),
                           policyman_state_get_trm_cap() );
  }

    mre_policy_execute((mre_policy_t *) pPolicy, precondsMet, pCtx);

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX "----------- Finished executing policy %d.%d.%d (%d ms) -----------",
                        POLICY_SUBS(pPolicy),
                        MRE_POLICY_NUMBER((mre_policy_t *)pPolicy),
                        MRE_POLICY_TYPE((mre_policy_t *)pPolicy),
                        MRE_POLICY_VERSION((mre_policy_t *)pPolicy),
                        pPolicy->elapsed_time_ms );
}

/*-------- policyman_policy_run --------*/
boolean policyman_policy_run(
  void  *pElem,
  void  *pCtx
)
{
  policyman_policy_t  *pPolicy = *((policyman_policy_t **) pElem);

  policy_execute(pPolicy, pCtx);

  return TRUE;
}

/*-------- policyman_policy_check_for_immediate_full_rat --------*/
void  policyman_policy_check_for_immediate_full_rat(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState
)
{
  mre_status_t  status;
  boolean       immediate_fullrat;

  status = mre_boolean_get_value( "pm:xml_full_rat",
                                  &immediate_fullrat,
                                  (mre_policy_t *) pPolicy );

  policyman_state_update_full_rat(pState, immediate_fullrat);
  pPolicy->immediate_full_rat = (MRE_STATUS_SUCCESS == status);
}


/*-------- handle_user_pref_update --------*/
STATIC void handle_user_pref_update(
  policyman_policy_t       *pPolicy,
  policy_execute_ctx_t     *pCtx
)
{
  sys_oprt_mode_e_type  oprt_mode;
  policyman_ss_info_t  *pSsInfo;
  size_t                index;

  oprt_mode  = policyman_ph_get_oprt_mode(pCtx->pState, POLICY_SUBS(pPolicy));
  pSsInfo    = policyman_state_get_serving_system_info(pCtx->pState, POLICY_SUBS(pPolicy));

  /* reset the SS state info and preconditions if UE entered LPM mode
  */
  if (oprt_mode == SYS_OPRT_MODE_LPM)
  {
    for (index = SYS_MODEM_STACK_ID_1; index < SYS_MODEM_STACK_ID_MAX; index++)
    {
      policyman_ss_set_default_value(pSsInfo, index);
    }

    policyman_state_reset_precondition_met(pCtx->pState, POLICYMAN_PRECOND_SS, pCtx->asubs_id);
  }
}

/*-------- policy_handle_user_pref_update --------*/
boolean policy_handle_user_pref_update(
  void  *pElem,
  void  *pCtx
)
{
  policyman_policy_t    *pPolicy    = *((policyman_policy_t **) pElem);
  policy_execute_ctx_t  *pPrefCtx   = (policy_execute_ctx_t *) pCtx;

  if (POLICY_IS_ENABLED(pPolicy))
  {
    handle_user_pref_update(pPolicy, pPrefCtx );
  }

  return TRUE;
}
