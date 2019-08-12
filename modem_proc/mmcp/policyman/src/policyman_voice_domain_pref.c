/**
  @file policyman_voice_domain_pref.c

  @brief
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_voice_domain_pref.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"

#include "policyman_cfgitem.h"
#include "policyman_cm.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_policies.h"
#include "policyman_set.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_voice_domain_pref.h"
#include "policyman_xml.h"

#include <stringl/stringl.h>

#define VOICE_DOMAIN_PREF_TAG  "voice_domain_pref"

/*=============================================================================
 Data Structure for Voice Domain Info
==============================================================================*/
struct policyman_voice_domain_pref_item_t
{
  POLICYMAN_ITEM;

  boolean mandatory;
  sys_voice_domain_pref_e_type voice_domain;
};

/*=============================================================================
  Voice Domain Pref APIs
=============================================================================*/

typedef struct
{
  POLICYMAN_ACTION_BASE;

  boolean mandatory;
  sys_voice_domain_pref_e_type voice_domain_pref;
} voice_domain_pref_action_t;



/*-------- policyman_voice_domain_pref_item_new --------*/
policyman_voice_domain_pref_item_t * policyman_voice_domain_pref_item_new(
  void
)
{
  return (policyman_voice_domain_pref_item_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_VOICE_DOMAIN_PREF,
                                                                            sizeof(policyman_voice_domain_pref_item_t),
                                                                            policyman_simple_dtor );
}


/*-------- policyman_voice_domain_pref_get_default --------*/
policyman_item_t * policyman_voice_domain_pref_get_default(
  policy_execute_ctx_t *pCtx
)
{
  policyman_voice_domain_pref_item_t *pItem;
  sys_voice_domain_pref_e_type        voiceDomain;

  pItem = policyman_voice_domain_pref_item_new();

  pItem->asubs_id = pCtx->asubs_id;
  pItem->mandatory = FALSE;

  if (policyman_retrieve_voice_domain(pCtx->asubs_id, &voiceDomain) )
  {
    pItem->voice_domain = voiceDomain;
    POLICYMAN_MSG_MED_2( SUBS_PREFIX "default voice_domain_pref from NV: %d",
                         pCtx->asubs_id,
                         voiceDomain );
  }
  else
  {
    /* Set default value for voice_domain_pref */
#if (defined(FEATURE_3GPP_CSFB) && !defined (FEATURE_IP_CALL))

    pItem->voice_domain = SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY;

#elif (defined(FEATURE_3GPP_CSFB) && defined (FEATURE_IP_CALL) && defined (FEATURE_LTE))

    pItem->voice_domain = SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED;

#elif (!defined(FEATURE_3GPP_CSFB) && defined (FEATURE_IP_CALL) && defined (FEATURE_LTE))

    pItem->voice_domain = SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY;

#else

    pItem->voice_domain = SYS_VOICE_DOMAIN_PREF_NONE;

#endif

    POLICYMAN_MSG_MED_2( SUBS_PREFIX "default voice_domain_pref from feature flags: %d",
                         pItem->asubs_id,
                         pItem->voice_domain );
  }

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "default voice_domain_pref: %d",
                        pItem->asubs_id,
                        pItem->voice_domain );

  return (policyman_item_t *) pItem;
}

/*-------- policyman_voice_domain_pref_compare --------*/
boolean policyman_voice_domain_pref_compare(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
)
{
  policyman_voice_domain_pref_item_t  *pPref1 = (policyman_voice_domain_pref_item_t *) pItem1;
  policyman_voice_domain_pref_item_t  *pPref2 = (policyman_voice_domain_pref_item_t *) pItem2;

  return  pPref1->voice_domain == pPref2->voice_domain
         && pPref1->mandatory == pPref2->mandatory;
}

/*-------- policyman_retrieve_voice_domain --------*/
boolean policyman_retrieve_voice_domain(
  sys_modem_as_id_e_type        subsId,
  sys_voice_domain_pref_e_type *pVoiceDomain
)
{
  mre_efs_status_t              status;
  sys_voice_domain_pref_e_type *pPref    = NULL;
  size_t                        prefSize;

  /* Initialize Voice Domain to NONE
   */
  *pVoiceDomain = SYS_VOICE_DOMAIN_PREF_NONE;

  /*  Try and read from the NV item for Voice Domain.
   */
  status = mre_efs_get_file( POLICYMAN_VOICE_DOMAIN_NV_PATH,
                             (void **)&pPref,
                             &prefSize,
                             subsId );

  /*  If we were able to read the NV and if the value is in the proper range,
   *  set the voice domain to that value.  Otherwise just return NONE.
   */
  if (   (status == MRE_EFS_STATUS_SUCCESS)
      && (prefSize == sizeof(sys_voice_domain_pref_e_type))
      && (pPref != NULL)
      && (*pPref > SYS_VOICE_DOMAIN_PREF_NONE)
      && (*pPref <= SYS_VOICE_DOMAIN_PREF_NO_CHANGE) )
  {
    *pVoiceDomain = *pPref;
  }

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "retrieve_voice_domain_pref: status %d, voice_domain_pref %d",
                        subsId,
                        status,
                        *pVoiceDomain );

  POLICYMAN_MEM_FREE_IF(pPref);

  return (*pVoiceDomain != SYS_VOICE_DOMAIN_PREF_NONE);
}

/*-------- policyman_get_voice_domain_pref --------*/
policyman_status_t policyman_get_voice_domain_pref(
  policyman_item_t const  *pItem,
  sys_voice_domain_pref_e_type *pVoiceDomain,
  boolean                 *pMandatory
)
{
  policyman_voice_domain_pref_item_t  *pPref = (policyman_voice_domain_pref_item_t *) pItem;

  if (pPref == NULL || pVoiceDomain == NULL || pMandatory == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_voice_domain_pref returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }
  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_VOICE_DOMAIN_PREF)
  {
    POLICYMAN_MSG_ERROR_0("get_voice_domain_pref returned err POLICYMAN_STATUS_ERR_INVALID_ITEM_ID");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pVoiceDomain = pPref->voice_domain;
  *pMandatory = pPref->mandatory;

  POLICYMAN_MSG_HIGH_4( "get_voice_domain_pref (from item 0x%x, subs %d) returned voice_domain_pref %d, mandatory %d",
                        pItem,
                        pItem->asubs_id,
                        pPref->voice_domain,
                        pPref->mandatory );

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_voice_domain_pref_display --------*/
void policyman_voice_domain_pref_display(
  policyman_item_t  *pItem
)
{
  policyman_voice_domain_pref_item_t  *pPref = (policyman_voice_domain_pref_item_t *) pItem;

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "voice_domain_pref %d, mandatory %d",
                        pPref->asubs_id,
                        pPref->voice_domain,
                        pPref->mandatory );
}

/*-------- policyman_str_to_voice_domain_pref --------*/
static boolean policyman_str_to_voice_domain_pref(
  const char                    *pStr,
  sys_voice_domain_pref_e_type  *pVoiceDomain
)
{
  typedef struct
  {
    const char                   *pStr;
    sys_voice_domain_pref_e_type  voiceDomain;
  } mask_map;

  static mask_map map[] =
  {
    {"CS_ONLY",  SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY},
    {"IMS_ONLY", SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY},
    {"CS_PREF",  SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED},
    {"IMS_PREF", SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED}
  };

  size_t  i;
  char    token[32];
  boolean succeeded = FALSE;


  *pVoiceDomain= SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY;

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      *pVoiceDomain = map[i].voiceDomain;
      succeeded = TRUE;
      break;
    }
  }

  return succeeded;
}


/*-------- policyman_voice_domain_pref_read_info --------*/
mre_status_t policyman_voice_domain_pref_read_info(
  policyman_xml_element_t const *pElem,
  sys_voice_domain_pref_e_type  *pVoiceDomain
)
{
  mre_status_t   status = MRE_STATUS_ERR_INVALID_ACTION;
  char const    *pStr;

  *pVoiceDomain = SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY;

  if (policyman_xml_tag_is(pElem, VOICE_DOMAIN_PREF_TAG))
  {
    pStr = policyman_xml_get_text(pElem);
    policyman_str_to_voice_domain_pref(pStr, pVoiceDomain);
    status = MRE_STATUS_SUCCESS;
  }

  return status;
}

/*-------- execute_voice_domain_pref --------*/
static boolean execute_voice_domain_pref(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  voice_domain_pref_action_t         *pvdAction = (voice_domain_pref_action_t *) pAction;
  policyman_voice_domain_pref_item_t *pSm;
  sys_modem_as_id_e_type              asid = ACTION_SUBS(pvdAction);

  if (pvdAction->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_NONE)
  {
    POLICYMAN_MSG_ERROR_2(SUBS_PREFIX SRC_LINE "<voice_domain> none attempted. action failed",
                          ACTION_SUBS(pvdAction),
                          LINE_NUM(pvdAction) );
    return TRUE;
  }

  pSm = policyman_voice_domain_pref_item_new();

  pSm->voice_domain = pvdAction->voice_domain_pref;
  pSm->asubs_id = asid;
  pSm->mandatory = pvdAction->mandatory;
  policyman_set_replace(((policy_execute_ctx_t *) pCtx)->pItemSet, &pSm);
  ref_cnt_obj_release(pSm);

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX "action <voice_domain_pref pmsubs=%d> set voice domain pref to %d, mandatory %d",
                        ACTION_SUBS(pvdAction),
                        LINE_NUM(pvdAction),
                        asid,
                        pSm->voice_domain,
                        pSm->mandatory );

  return TRUE;
}

/*-------- policyman_voice_domain_pref_execute --------*/
static boolean policyman_voice_domain_pref_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_voice_domain_pref);
}

/*-------- policyman_voice_domain_pref_action_new --------*/
mre_status_t policyman_voice_domain_pref_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  mre_status_t                   status = MRE_STATUS_ERR_INVALID_ACTION;
  voice_domain_pref_action_t    *pAction = NULL;
  sys_voice_domain_pref_e_type   voice_domain;
  char const                    *pStr;

  pStr = policyman_xml_get_attribute(pElem, "mandatory");

  status = policyman_voice_domain_pref_read_info(pElem, &voice_domain);

  if (MRE_FAILED(status))
  {
    POLICYMAN_UT_MSG_2( SUBS_PREFIX SRC_LINE "<voice_domain_pref>: invalid voice domain preference",
                        POLICY_SUBS(pPolicy),
                        mre_xml_get_line_number(pElem) );
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( voice_domain_pref_action_t,
                                  pPolicy,
                                  policyman_voice_domain_pref_execute,
                                  policyman_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pAction->voice_domain_pref = voice_domain;
  pAction->mandatory = (pStr == NULL || strcmp(pStr, "true") == 0);

  *ppAction = (mre_action_t *) pAction;

Done:
  return status;
}


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  sys_voice_domain_pref_e_type  voice_domain_pref;
} policyman_voice_domain_pref_condition_t;


/*-------- evaluate_voice_domain_pref --------*/
static boolean evaluate_voice_domain_pref(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
)
{
  policyman_voice_domain_pref_condition_t *pCond = (policyman_voice_domain_pref_condition_t *) pCondition;
  policyman_status_t                       status;
  policyman_item_id_t                      id = POLICYMAN_ITEM_VOICE_DOMAIN_PREF;
  policyman_item_t                        *pItem;
  size_t                                   numItems;
  sys_voice_domain_pref_e_type             current_voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NONE;
  boolean                                  mandatory;
  boolean                                  result = FALSE;

  numItems = policyman_cfgitem_get_items_per_subs( &id,
                                                   1,
                                                   (policyman_item_t const **)&pItem,
                                                   CONDITION_SUBS(pCond) );

  if (numItems != 1)
  {
    goto Done;
  }

  status = policyman_get_voice_domain_pref( (policyman_item_t const *)pItem,
                                            &current_voice_domain_pref,
                                            &mandatory );

  if (POLICYMAN_SUCCEEDED(status))
  {
    result = (current_voice_domain_pref == pCond->voice_domain_pref);
  }

Done:
  POLICYMAN_MSG_HIGH_6( SUBS_PREFIX SRC_LINE "condition <voice_domain_pref_is pmsubs=%d> returns %d (testing current %d vs condition %d)",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        pCond->pmSubsId,
                        result,
                        current_voice_domain_pref,
                        pCond->voice_domain_pref );

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return result;
}

/*-------- policyman_condition_voice_domain_pref_evaluate --------*/
STATIC boolean policyman_condition_voice_domain_pref_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_voice_domain_pref);
}


/*-------- policyman_condition_voice_domain_pref_new --------*/
mre_status_t policyman_condition_voice_domain_pref_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  mre_status_t                             status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_voice_domain_pref_condition_t *pCondition;
  char const                              *pStr;
  sys_voice_domain_pref_e_type             voiceDomain;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<voice_domain_pref_is> voice domain pref required", POLICY_SUBS(pPolicy));
    goto Done;
  }

  if (!policyman_str_to_voice_domain_pref(pStr, &voiceDomain))
  {
    POLICYMAN_UT_MSG_2( SUBS_PREFIX "<voice_domain_pref_is> invalid voice domain pref '%s'",
                        POLICY_SUBS(pPolicy),
                        pStr );
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_voice_domain_pref_condition_t,
                                        pPolicy,
                                        policyman_condition_voice_domain_pref_evaluate,
                                        policyman_condition_dtor,
                                        SUBS_THIS,
                                        pElem );

  pCondition->voice_domain_pref = voiceDomain;

  *ppCondition = (mre_condition_t *) pCondition;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- policyman_voice_domain_pref_fill_subs_diag_log --------*/
void policyman_voice_domain_pref_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  policyman_voice_domain_pref_item_t     *pVoicePref = (policyman_voice_domain_pref_item_t *)pItem;

  log_ptr->voice_dom_pref = (int8)pVoicePref->voice_domain;

  if (pVoicePref->mandatory)
  {
    log_ptr->flags        |= PM_SUBS_INFO_FLAGS_DOMAIN_PREF_MANDATORY_MASK;
  }
}