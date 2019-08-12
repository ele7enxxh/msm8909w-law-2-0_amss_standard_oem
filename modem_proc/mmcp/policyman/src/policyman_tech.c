/**
  @file policyman_tech.c

  @brief  Module to handle segment-loading of technologies.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_tech.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_tech.h"
#include "policyman_phone_events.h"
#include "policyman_policies.h"
#include "policyman_rat_capability.h"
#include "policyman_rules.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_xml.h"


#if !defined(FEATURE_SEGMENT_LOADING) && !defined(TEST_FRAMEWORK)

#define  MCFG_SEGMENT_LOAD_ALL        0x00000000
#define  MCFG_SEGMENT_EXCLUDE_WCDMA   0x00000001
#define  MCFG_SEGMENT_EXCLUDE_TDSCDMA 0x00000002

IxErrnoType create_instance(
  clsid_t       clsid,
  interface_t **ppIface
)
{
  return E_FAILURE;
}

int mcfg_segment_exclude_technologies(
  uint32  excluded_techs
)
{
  return 0;
}

#endif



/*-------- tech_info_t --------*/
/**
@brief  Structure that contains information about technologies that
        can be loaded/unloaded by segment loading.
*/
typedef struct
{
  char const      *pName;         /* name of the technology */
  clsid_t const    clsid;         /* Class ID of the technology */
  uint32 const     mask;          /* Tech mask of the technology */
  interface_t     *pInterface;    /* Pointer to interface for the technology */
} tech_info_t;


/*-------- tech_info --------*/
/**
@brief  Technologies currently handled by segment loading
*/
static tech_info_t  tech_info[] =
{
  {"WCDMA",     CLSID_WCDMA,    MCFG_SEGMENT_EXCLUDE_WCDMA,   NULL},
  {"TDSCDMA",   CLSID_TDSCDMA,  MCFG_SEGMENT_EXCLUDE_TDSCDMA, NULL},
};



/*=============================================================================
  Utility functions
=============================================================================*/


/*-------- policyman_tech_find_info_by_clsid --------*/
/**
@brief  Return a pointer to the information for a given technology

@param[in]  clsid   Class ID of the technology

@return
  Pointer to the technology information, or NULL if not found.
*/
static tech_info_t * policyman_tech_find_info_by_clsid(
  clsid_t   clsid
)
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(tech_info) ; ++i)
  {
    if (tech_info[i].clsid == clsid)
    {
      return &tech_info[i];
    }
  }

  return NULL;
}


/*-------- policyman_tech_find_info_by_name --------*/
/**
@brief  Return a pointer to the information for a given technology

@param[in]  pName   Name of the technology

@return
  Pointer to the technology information, or NULL if not found.
*/
static tech_info_t * policyman_tech_find_info_by_name(
  char const  *pName
)
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(tech_info) ; ++i)
  {
    if (strcmp(pName, tech_info[i].pName) == 0)
    {
      return &tech_info[i];
    }
  }

  return NULL;
}


/*-------- policyman_tech_is_loaded --------*/
boolean policyman_tech_is_loaded(
  clsid_t   clsid
)
{
  tech_info_t *pInfo = policyman_tech_find_info_by_clsid(clsid);

#if defined(FEATURE_SEGMENT_LOADING)
  return pInfo != NULL && pInfo->pInterface != NULL;
#else
  return pInfo == NULL || pInfo != NULL;  /* TRUE, but uses pInfo so no compile error */
#endif
}



/*=============================================================================
  Initialization/Deinitilization of the module.
=============================================================================*/


/*-------- policyman_tech_init --------*/
void policyman_tech_init(
  void
)
{
  size_t          i;
  IxErrnoType     result;

  for (i = 0 ; i < ARR_SIZE(tech_info) ; ++i)
  {
    result = create_instance(tech_info[i].clsid, &tech_info[i].pInterface);
    if (result != E_SUCCESS)
    {
      tech_info[i].pInterface = NULL;
    }
  }
}

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */


/*=============================================================================
  <tech_loaded> condition functions.
=============================================================================*/


/*-------- policyman_tech_present_condition_t --------*/
typedef struct
{
  POLICYMAN_CONDITION_BASE;

  clsid_t   clsid;
} policyman_tech_present_condition_t;


/*-------- evaluate_tech_loaded --------*/
static boolean evaluate_tech_loaded(
  policyman_condition_t const  *pCondition,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  boolean                             result = FALSE;
  policyman_tech_present_condition_t  *pCond;

  pCond = (policyman_tech_present_condition_t *) pCondition;

  result = policyman_tech_is_loaded(pCond->clsid);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "condition <tech_present> returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        result );
  return result;
}

/*-------- policyman_condition_tech_loaded_evaluate --------*/
STATIC boolean policyman_condition_tech_loaded_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_tech_loaded);
}


/*-------- policyman_condition_tech_loaded_new --------*/
mre_status_t policyman_condition_tech_loaded_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  mre_status_t                         status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_tech_present_condition_t  *pCondition;
  char const                          *pStr;
  char                                 token[32];
  tech_info_t                         *pInfo;

  pCondition = POLICYMAN_CONDITION_NEW( policyman_tech_present_condition_t,
                                        pPolicy,
                                        policyman_condition_tech_loaded_evaluate,
                                        policyman_condition_dtor,
                                        SUBS_THIS,
                                        pElem );

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL || !policyman_get_token(&pStr, token, ARR_SIZE(token)))
  {
    POLICYMAN_MSG_ERROR_1( SUBS_PREFIX "No content for <tech_loaded> node", POLICY_SUBS(pPolicy));
    goto Done;
  }

  pInfo = policyman_tech_find_info_by_name(token);

  if (pInfo == NULL)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "Invalid technology specified in <tech_loaded> element", POLICY_SUBS(pPolicy));
    POLICYMAN_UT_MSG_2( SUBS_PREFIX "Invalid technology %s specified in <tech_loaded> element",
                        POLICY_SUBS(pPolicy),
                        token );
    goto Done;
  }

  pCondition->clsid = pInfo->clsid;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  if (POLICYMAN_FAILED(status))
  {
    REF_CNT_OBJ_RELEASE_IF(pCondition);
  }

  return status;
}



/*=============================================================================
  <exclude_tech> action functions.
=============================================================================*/

typedef struct
{
  POLICYMAN_CMD_HDR;

  uint32  techMask;
} policyman_tech_exclude_t;


typedef struct
{
 POLICYMAN_ACTION_BASE;

 uint32 tech_mask;
} exclude_tech_action_t;

/*-------- policyman_execute_tech_exclude --------*/
STATIC void policyman_execute_tech_exclude(
  policyman_cmd_t *pCmd
)
{
  policyman_tech_exclude_t *pTechCmd = (policyman_tech_exclude_t *) pCmd;

  if (pTechCmd)
  {
    POLICYMAN_MSG_HIGH_1("processing <exclude_tech> for mask 0x%08x", pTechCmd->techMask);

    mcfg_segment_exclude_technologies(pTechCmd->techMask);
  }
}

/*-------- execute_tech_exclude --------*/
STATIC boolean execute_tech_exclude(
  policyman_action_t const      *pAction,
  void                          *pCtx,
  sys_modem_as_id_e_type         subsId
)
{
  uint32                    mask        = ((exclude_tech_action_t *) pAction)->tech_mask;
  uint32                    rat_include = (SYS_SYS_MODE_MASK_WCDMA | SYS_SYS_MODE_MASK_TDS);
  policyman_policy_t       *pPolicy     = (policyman_policy_t *) pAction->pPolicy;
  policyman_tech_exclude_t *pCmd        = NULL;
  policy_execute_ctx_t     *pContext    = (policy_execute_ctx_t *) pCtx;

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "executing <exclude_tech> of 0x%08x",
                        ACTION_SUBS(pAction),
                        LINE_NUM(pAction),
                        mask );

  if (mask & MCFG_SEGMENT_EXCLUDE_TDSCDMA)
  {
    rat_include &= ~SYS_SYS_MODE_MASK_TDS;
  }

  if (mask & MCFG_SEGMENT_EXCLUDE_WCDMA)
  {
    rat_include &= ~SYS_SYS_MODE_MASK_WCDMA;
  }

  policyman_update_persisted_user_plmn(pContext->pState, pPolicy->subs);

  /* Update other policy items first to EFS but don't send notification
   */
#if defined(TEST_FRAMEWORK)
  #error code not present
#endif

  /* Finally update RAT and band capability as per new segment
       Queue a cmd to call MCFG API to trigger segment swap
       Suspend Policy Manager so that we don't react to any event further
   */
  policyman_update_rats_bands_to_efs_per_tech(rat_include);

  pCmd = POLICYMAN_CMD_NEW( policyman_tech_exclude_t,
                            policyman_execute_tech_exclude,
                            NULL,
                            NULL );

  pCmd->techMask = mask;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
  policyman_suspend_by_client(POLICYMAN_CLIENT_POLICYMAN);

  return TRUE;
}

/*-------- policyman_tech_exclude_execute --------*/
STATIC boolean policyman_tech_exclude_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_tech_exclude);
}

/*-------- policyman_tech_exclude_new --------*/
mre_status_t policyman_tech_exclude_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  mre_status_t           status = MRE_STATUS_SUCCESS;
  exclude_tech_action_t *pAction = NULL;
  char const            *pStr;
  char                   token[32];
  tech_info_t           *pInfo;

  pAction = POLICYMAN_ACTION_NEW( exclude_tech_action_t,
                                  pPolicy,
                                  policyman_tech_exclude_execute,
                                  policyman_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pAction->tech_mask = 0;   /* by default, exclude nothing */

  pStr = policyman_xml_get_text(pElem);
  if (pStr == NULL)   { goto Done; }

  while (policyman_get_token(&pStr, token, ARR_SIZE(token)))
  {
    pInfo = policyman_tech_find_info_by_name(token);
    if (pInfo == NULL)
    {
      POLICYMAN_MSG_ERROR_0("Invalid technology specified in <exclude_tech> element");
      POLICYMAN_UT_MSG_1("Invalid technology %s specified in <exclude_tech> element", token);
      status = MRE_STATUS_ERR_MALFORMED_XML;
      goto Done;
    }
    pAction->tech_mask |= pInfo->mask;
  }

Done:
  if (MRE_FAILED(status))
  {
    REF_CNT_OBJ_RELEASE_IF(pAction);
  }
  *ppAction = (mre_action_t *) pAction;

  return status;
}
