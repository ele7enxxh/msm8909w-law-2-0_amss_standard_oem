/**
  @file policyman_svc_mode.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_svc_mode.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"

#include "policyman_svc_mode.h"
#include "policyman_device_config.h"
#include "policyman_dbg.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_set.h"
#include "policyman_subs.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include <stringl/stringl.h>

#define SVC_MODE_TAG  "svc_mode"

/*=============================================================================
  SVC mode APIs
=============================================================================*/

 typedef struct
 {
   POLICYMAN_ACTION_BASE;

   sys_modem_as_id_e_type   subsId;
   policyman_svc_mode_t     svc_mode;
 } svc_mode_action_t;


/*-------- policyman_svc_mode_item_new --------*/
policyman_svc_mode_item_t * policyman_svc_mode_item_new(
  policyman_svc_mode_t    svcMode,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_svc_mode_item_t *pItem;

  if (!policyman_state_get_simlock_verified(subsId))
  {
    POLICYMAN_MSG_HIGH_2("simlock is not verified. actual requested Svc Mode %d for subs %d", svcMode, subsId);
    svcMode = POLICYMAN_SVC_MODE_LIMITED;
  }

  pItem = (policyman_svc_mode_item_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_SVC_MODE,
                                                                    sizeof(policyman_svc_mode_item_t),
                                                                    policyman_simple_dtor );
  pItem->mode = svcMode;
  pItem->asubs_id = subsId;

  return pItem;
}

/*-------- policyman_retrieve_svc_mode --------*/
STATIC boolean policyman_retrieve_svc_mode(
  sys_modem_as_id_e_type   subsId,
  policyman_svc_mode_t    *pSvcMode
)
{
  mre_efs_status_t        status;
  policyman_svc_mode_t   *pMode    = NULL;
  size_t                  modeSize;
  boolean                 retval   = FALSE;

  /*  Try and read from the NV item for UE mode.
   */
  status = mre_efs_get_file( POLICYMAN_SVC_MODE_NV_PATH,
                             (void **)&pMode,
                             &modeSize,
                             subsId );

  /*  If we were able to read the NV and if the value is in the proper range,
   *  set the mode to that value.  Otherwise just return NORMAL mode.
   */
  if (   (status == MRE_EFS_STATUS_SUCCESS)
      && (modeSize == sizeof(policyman_svc_mode_t))
      && (pMode != NULL)
      && (*pMode >= POLICYMAN_SVC_MODE_LIMITED)
      && (*pMode <= POLICYMAN_SVC_MODE_ALL) )
  {
    *pSvcMode = *pMode;
    retval = TRUE;
  }

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "retrieve_svc_mode: status %d, svc_mode %d",
                        subsId,
                        status,
                        *pSvcMode );

  POLICYMAN_MEM_FREE_IF(pMode);

  return retval;
}

/*-------- policyman_svc_mode_get_default_from_efs --------*/
policyman_status_t policyman_svc_mode_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
)
{
  policyman_svc_mode_item_t *pSvcMode = NULL;
  policyman_svc_mode_t       svcMode;
  policyman_status_t         status   = POLICYMAN_STATUS_ERR;

  if (policyman_retrieve_svc_mode(pCfgCtx->asubs_id, &svcMode))
  {
    pSvcMode = policyman_svc_mode_item_new(svcMode, pCfgCtx->asubs_id);
    pCfgCtx->pItem = (policyman_item_t *)pSvcMode;

    status = POLICYMAN_STATUS_SUCCESS;
  }

  return status;
}

/*-------- policyman_svc_mode_get_default --------*/
policyman_item_t * policyman_svc_mode_get_default(
  policy_execute_ctx_t *pCtx
)
{
  policyman_svc_mode_item_t  *pItem;

  pItem = policyman_svc_mode_item_new(POLICYMAN_SVC_MODE_FULL, pCtx->asubs_id);

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "default svc_mode %d",
                        pCtx->asubs_id,
                        pItem->mode );

  return (policyman_item_t *) pItem;
}

/*-------- policyman_svc_mode_compare --------*/
boolean policyman_svc_mode_compare(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
)
{
  policyman_svc_mode_item_t  *pMode1 = (policyman_svc_mode_item_t *) pItem1;
  policyman_svc_mode_item_t  *pMode2 = (policyman_svc_mode_item_t *) pItem2;

  return pMode1->mode == pMode2->mode;
}


/*-------- policyman_get_svc_mode --------*/
policyman_status_t policyman_get_svc_mode(
  policyman_item_t const  *pItem,
  policyman_svc_mode_t    *pSvcMode
)
{
  policyman_svc_mode_item_t  *pMode = (policyman_svc_mode_item_t *) pItem;

  if (pMode == NULL || pSvcMode == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_svc_mode returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }
  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_SVC_MODE)
  {
    POLICYMAN_MSG_ERROR_0("get_svc_mode returned err POLICYMAN_STATUS_ERR_INVALID_ITEM_ID");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pSvcMode = pMode->mode;

  POLICYMAN_MSG_HIGH_3( "get_svc_mode (from item 0x%x, subs %d) returned svc_mode %d",
                        pItem,
                        pItem->asubs_id,
                        pMode->mode );

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_svc_mode_display --------*/
void policyman_svc_mode_display(
  policyman_item_t  *pItem
)
{
  policyman_svc_mode_item_t  *pMode = (policyman_svc_mode_item_t *) pItem;

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "svc_mode: %d (item 0x%x)",
                        pMode->asubs_id,
                        pMode->mode,
                        pItem );
}

/*-------- policyman_svc_mode_update_to_efs --------*/
void policyman_svc_mode_update_to_efs(
  policyman_item_t const *pItem
)
{
  policyman_svc_mode_item_t *pSvcMode    = (policyman_svc_mode_item_t *)pItem;
  boolean                    writeStatus;
  size_t                     nSim        = 0;

  /* Don't update UE mode into EFS for the SUBS which is not configured
   */
  (void)policyman_get_current_num_sim(&nSim);

  if ((size_t)pItem->asubs_id >= nSim)
  {
    POLICYMAN_MSG_MED_1(SUBS_PREFIX "subs not configured; EFS not updated for svc_mode", pItem->asubs_id);
    return;
  }

  writeStatus = policyman_cfgitem_persist_item( POLICYMAN_SVC_MODE_NV_PATH,
                                                &pSvcMode->mode,
                                                sizeof(pSvcMode->mode),
                                                pItem->asubs_id );

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "svc_mode %d write to efs status %d",
                        pItem->asubs_id,
                        pSvcMode->mode,
                        writeStatus );
}

/*-------- policyman_str_to_svc_mode --------*/
static boolean policyman_str_to_svc_mode(
  const char            *pStr,
  policyman_svc_mode_t  *pSvcMode
)
{
  typedef struct
  {
    const char            *pStr;
    policyman_svc_mode_t  svcMode;
  } mask_map;

  static mask_map map[] =
  {
    {"FULL",      POLICYMAN_SVC_MODE_FULL},
    {"CAMP_ONLY", POLICYMAN_SVC_MODE_CAMP_ONLY},
    {"LIMITED",   POLICYMAN_SVC_MODE_LIMITED},
    {"ALL",       POLICYMAN_SVC_MODE_ALL},
  };

  size_t  i;
  char    token[32];
  boolean succeeded = FALSE;


  *pSvcMode = POLICYMAN_SVC_MODE_FULL;

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      *pSvcMode = map[i].svcMode;
      succeeded = TRUE;
      break;
    }
  }

  return succeeded;
}


/*-------- policyman_svc_mode_read_info --------*/
policyman_status_t policyman_svc_mode_read_info(
  policyman_xml_element_t const *pElem,
  policyman_svc_mode_t          *pSvcmode
)
{
  policyman_status_t status = POLICYMAN_STATUS_ERR_INVALID_ACTION;
  char const         *pStr;

  *pSvcmode = POLICYMAN_SVC_MODE_FULL;

  if (policyman_xml_tag_is(pElem, SVC_MODE_TAG))
  {
    pStr = policyman_xml_get_text(pElem);
    policyman_str_to_svc_mode(pStr, pSvcmode);
    status = POLICYMAN_STATUS_SUCCESS;
  }

  return status;
}


/*-------- execute_svc_mode --------*/
static boolean execute_svc_mode(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  svc_mode_action_t         *psvcAction = (svc_mode_action_t *) pAction;
  policy_execute_ctx_t      *psvcCtx = (policy_execute_ctx_t *)pCtx;
  policyman_svc_mode_item_t *pSm = policyman_svc_mode_item_new(psvcAction->svc_mode, subsId);

  policyman_set_replace(psvcCtx->pItemSet, &pSm);
  ref_cnt_obj_release(pSm);

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "action <svc_mode pmsubs=%d> set mode to %d",
                        ACTION_SUBS(psvcAction),
                        LINE_NUM(psvcAction),
                        subsId,
                        pSm->mode );
  return TRUE;
}


/*-------- policyman_svc_mode_execute --------*/
static boolean policyman_svc_mode_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_svc_mode);
}


/*-------- policyman_svc_mode_action_new --------*/
mre_status_t policyman_svc_mode_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  mre_status_t             status = MRE_STATUS_ERR_INVALID_ACTION;
  svc_mode_action_t       *pAction = NULL;
  policyman_svc_mode_t     mode;
  policyman_subs_t         pm_subs;

  status = policyman_svc_mode_read_info(pElem, &mode);

  if (MRE_FAILED(status))
  {
    POLICYMAN_UT_MSG_2(SUBS_PREFIX SRC_LINE "<svc_mode> invalid service mode",
                       POLICY_SUBS(pPolicy),
                       mre_xml_get_line_number(pElem) );
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( svc_mode_action_t,
                                  pPolicy,
                                  policyman_svc_mode_execute,
                                  policyman_action_dtor,
                                  pm_subs,
                                  pElem );

  pAction->svc_mode = mode;

  *ppAction = (mre_action_t *) pAction;

Done:
  return status;
}


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_svc_mode_t svc_mode;
} policyman_svc_mode_condition_t;


/*-------- evaluate_svc_mode_is --------*/
static boolean evaluate_svc_mode_is(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
)
{
  policyman_svc_mode_condition_t *pCond = (policyman_svc_mode_condition_t *) pCondition;
  policyman_status_t              status;
  policyman_item_id_t             id = POLICYMAN_ITEM_SVC_MODE;
  policyman_svc_mode_item_t      *pItem = NULL;
  policyman_svc_mode_t            current_svc_mode;
  boolean                         result = FALSE;

  pItem = policyman_cfgitem_find_item_in_context( pCtx, id, subsId );

  if (NULL != pItem)
  {
    status = policyman_get_svc_mode((policyman_item_t const *)pItem, &current_svc_mode);

    if (POLICYMAN_SUCCEEDED(status))
    {
      result = (current_svc_mode == pCond->svc_mode);
    }
  }

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <svc_mode_is pmsubs=%d> returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        result );

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return result;
}


/*-------- policyman_condition_svc_mode_evaluate --------*/
static boolean policyman_condition_svc_mode_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_svc_mode_is);
}


/*-------- policyman_condition_svc_mode_new --------*/
mre_status_t policyman_condition_svc_mode_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  mre_status_t                     status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_svc_mode_condition_t  *pCondition;
  char const                      *pStr;
  policyman_svc_mode_t             svcMode;
  policyman_subs_t                 pm_subs;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_0("<svc_mode_is>: service mode required");
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  if (!policyman_str_to_svc_mode(pStr, &svcMode))
  {
    POLICYMAN_UT_MSG_1("<svc_mode_is>: invalid service mode: %s", pStr);
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_svc_mode_condition_t,
                                        pPolicy,
                                        policyman_condition_svc_mode_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  pCondition->svc_mode = svcMode;

  *ppCondition = (mre_condition_t *) pCondition;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*-------- policyman_svc_mode_fill_subs_diag_log --------*/
void policyman_svc_mode_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  policyman_svc_mode_item_t     *pSvcMode = (policyman_svc_mode_item_t *)pItem;

  log_ptr->svc_mode = (uint8)pSvcMode->mode;
}

/*-------- policyman_svc_mode_get_limited_item --------*/
void policyman_svc_mode_get_limited_item(
  mre_set_t             *pItemSet,
  sys_modem_as_id_e_type subsId
)
{
  policyman_svc_mode_item_t *pItem;

  pItem = policyman_svc_mode_item_new(POLICYMAN_SVC_MODE_LIMITED, subsId); 

  mre_set_add(pItemSet, &pItem);
  ref_cnt_obj_release(pItem);
}
