/**
  @file policyman_ue_mode.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_ue_mode.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"

#include "policyman_ue_mode.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_set.h"
#include "policyman_subs.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include <stringl/stringl.h>
#include "event.h"
#include "event_defs.h"

#define UE_MODE_TAG  "ue_mode"


/*=============================================================================
  UE mode APIs
=============================================================================*/


/*-------- policyman_ue_mode_item_new --------*/
policyman_ue_mode_item_t * policyman_ue_mode_item_new(
  sys_ue_mode_e_type      ue_mode,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_ue_mode_item_t  *pItem;

  pItem = (policyman_ue_mode_item_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_UE_MODE,
                                                                   sizeof(policyman_ue_mode_item_t),
                                                                   policyman_simple_dtor );

  pItem->mode = ue_mode;
  pItem->asubs_id = subsId;

  return pItem;
}


/*-------- policyman_ue_mode_normalize_collection --------*/
void policyman_ue_mode_normalize_collection(
  policyman_item_collection_t *pCollection
)
{
  policyman_item_t          **ppItems = (policyman_item_t **) pCollection->pItems;
  policyman_ue_mode_item_t   *pOld;
  size_t                      i;
  sys_ue_mode_e_type          ue_mode;

  for (i = 0 ; i < pCollection->numItems ; ++i)
  {
    /*  If this isn't a UE_MODE item, continue.
     */
    if (policyman_item_get_id(ppItems[i]) != POLICYMAN_ITEM_UE_MODE)
    {
      continue;
    }

    /*
     */
    pOld = (policyman_ue_mode_item_t *) ppItems[i];
    switch (pOld->mode)
    {
      case SYS_UE_MODE_SVLTE_ONLY:
        ue_mode = SYS_UE_MODE_SVLTE;
        break;

      default:
        ue_mode = SYS_UE_MODE_NONE;
        break;
    }

    if (ue_mode != SYS_UE_MODE_NONE)
    {
      policyman_ue_mode_item_t  *pNew;

      pNew = policyman_ue_mode_item_new(ue_mode, pOld->asubs_id);
      ppItems[i] = (policyman_item_t *) pNew;
      ref_cnt_obj_release(pOld);
    }
  }
}


/*-------- policyman_ue_mode_get_default_from_efs --------*/
policyman_status_t policyman_ue_mode_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
)
{
  policyman_ue_mode_item_t  *pUeMode = NULL;
  sys_ue_mode_e_type         ueMode;
  policyman_status_t         status  = POLICYMAN_STATUS_ERR;

  if (policyman_retrieve_ue_mode(pCfgCtx->asubs_id, &ueMode))
  {
    pUeMode = policyman_ue_mode_item_new(ueMode, pCfgCtx->asubs_id);
    pCfgCtx->pItem = (policyman_item_t *)pUeMode;

    status  = POLICYMAN_STATUS_SUCCESS;
  }

  return status;
}

/*-------- policyman_ue_mode_get_default --------*/
policyman_item_t * policyman_ue_mode_get_default(
  policy_execute_ctx_t *pCtx
)
{
  policyman_ue_mode_item_t  *pItem;
  sys_ue_mode_e_type         ueMode = SYS_UE_MODE_NORMAL;

  pItem = policyman_ue_mode_item_new(ueMode, pCtx->asubs_id);

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "default ue_mode %d",
                        pCtx->asubs_id,
                        pItem->mode );

  return (policyman_item_t *) pItem;
}

/*-------- policyman_ue_mode_compare --------*/
boolean policyman_ue_mode_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
)
{
  policyman_ue_mode_item_t  *pMode1 = (policyman_ue_mode_item_t *) pData1;
  policyman_ue_mode_item_t  *pMode2 = (policyman_ue_mode_item_t *) pData2;

  return  pMode1->mode == pMode2->mode;
}

/*-------- policyman_ue_mode_display --------*/
void policyman_ue_mode_display(
  policyman_item_t  *pItem
)
{
  policyman_ue_mode_item_t  *pMode = (policyman_ue_mode_item_t *) pItem;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "ue_mode %d",
                        pMode->asubs_id,
                        pMode->mode );
}

/*-------- policyman_ue_mode_on_update --------*/
void policyman_ue_mode_on_update(
  policyman_item_t  *pItem
)
{
  policyman_ue_mode_item_t  *pMode = (policyman_ue_mode_item_t *) pItem;
  uint8                      event_ue_mode;

  event_ue_mode = pMode->mode;

  event_report_payload(EVENT_PM_UE_MODE, sizeof(event_ue_mode), &event_ue_mode);

  return;
}

/*-------- policyman_ue_mode_update_to_efs --------*/
void policyman_ue_mode_update_to_efs(
  policyman_item_t const *pItem
)
{
  policyman_ue_mode_item_t  *pMode       = (policyman_ue_mode_item_t *)pItem;
  boolean                    writeStatus;
  size_t                     nSim        = 0;

  /* Don't update ue_mode into EFS for the SUBS which is not configured
   */
  (void)policyman_get_current_num_sim(&nSim);

  if ((size_t)pItem->asubs_id >= nSim)
  {
    POLICYMAN_MSG_MED_1(SUBS_PREFIX "subs not configured; EFS not updated for ue_mode", pItem->asubs_id);
    return;
  }

  writeStatus = policyman_cfgitem_persist_item( POLICYMAN_UE_MODE_NV_PATH,
                                                &pMode->mode,
                                                sizeof(pMode->mode),
                                                pItem->asubs_id );

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "ue_mode %d write to efs status %d",
                        pItem->asubs_id,
                        pMode->mode,
                        writeStatus );
}

/*-------- policyman_retrieve_ue_mode --------*/
boolean policyman_retrieve_ue_mode(
  sys_modem_as_id_e_type    subsId,
  sys_ue_mode_e_type       *pUeMode
)
{
  mre_efs_status_t           status;
  sys_ue_mode_e_type        *pMode = NULL;
  size_t                     modeSize;

  /* Initialize ue_mode to NONE
   */
  *pUeMode = SYS_UE_MODE_NONE;

  /*  Try and read from the NV item for ue_mode.
   */
  status = mre_efs_get_file( POLICYMAN_UE_MODE_NV_PATH,
                             (void **)&pMode,
                             &modeSize,
                             subsId );

  /*  If we were able to read the NV and if the value is in the proper range,
   *  set the mode to that value.  Otherwise just return NORMAL mode.
   */
  if (   (status == MRE_EFS_STATUS_SUCCESS)
      && (modeSize == sizeof(sys_ue_mode_e_type))
      && (pMode != NULL)
      && (*pMode > SYS_UE_MODE_NONE)
      && (*pMode <= SYS_UE_MODE_1XSRLTE_ONLY) )
  {
    *pUeMode = *pMode;
  }

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "retrieve_ue_mode: status %d, ue_mode %d",
                        subsId,
                        status,
                        *pUeMode );

  POLICYMAN_MEM_FREE_IF(pMode);

  return (*pUeMode != SYS_UE_MODE_NONE);
}

/*-------- policyman_get_current_ue_mode --------*/
policyman_status_t policyman_get_current_ue_mode(
  sys_modem_as_id_e_type  subsId,
  sys_ue_mode_e_type     *pMode
)
{
  policyman_item_t    *pItem  = NULL;
  policyman_item_id_t  id     = POLICYMAN_ITEM_UE_MODE;
  policyman_status_t   status = POLICYMAN_STATUS_SUCCESS;
  sys_ue_mode_e_type   ueMode;
  size_t               nItem;

  if (pMode == NULL)
  {
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  /* Get ue_mode policy item
   */
   nItem = policyman_cfgitem_get_items_per_subs( (policyman_item_id_t const *)&id,
                                                 1,
                                                 (policyman_item_t const **)&pItem,
                                                 subsId );
  if (nItem == 1)
  {
    status = policyman_get_ue_mode(pItem, &ueMode);
    if (status == POLICYMAN_STATUS_SUCCESS)
    {
      *pMode = ueMode;
    }
  }
  /* Fetch current ue_mode and return
   */

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return status;
}


/*=============================================================================
  Accessor function for ue_mode
=============================================================================*/


/*-------- policyman_get_ue_mode --------*/
policyman_status_t policyman_get_ue_mode(
  policyman_item_t const  *pItem,
  sys_ue_mode_e_type      *pUeMode
)
{
  policyman_ue_mode_item_t  *pMode = (policyman_ue_mode_item_t *) pItem;

  if (pMode == NULL || pUeMode == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_ue_mode returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_UE_MODE)
  {
    POLICYMAN_MSG_ERROR_0("get_ue_mode returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pUeMode = pMode->mode;

  POLICYMAN_MSG_HIGH_4( LOG_SUBS_TAG "get_ue_mode (from item 0x%x, subs %d) returned ue_mode %d",
                        LOG_SUBS(pMode->asubs_id),
                        pItem,
                        pItem->asubs_id,
                        pMode->mode );

  return POLICYMAN_STATUS_SUCCESS;
}


typedef struct
{
  POLICYMAN_ACTION_BASE;

  sys_ue_mode_e_type ue_mode;
} ue_mode_action_t;


/*-------- policyman_str_to_ue_mode --------*/
static sys_ue_mode_e_type policyman_str_to_ue_mode(
  const char  *pUe_mode
)
{
  typedef struct
  {
    const char  *pStr;
    sys_ue_mode_e_type      ue_mode;
  } mask_map;
  sys_ue_mode_e_type ue_mode = SYS_UE_MODE_NONE;

  static mask_map map[] =
  {
    {"NORMAL",          SYS_UE_MODE_NORMAL},
    {"CSFB",            SYS_UE_MODE_CSFB},
    {"SGLTE",           SYS_UE_MODE_SGLTE},
    {"SVLTE",           SYS_UE_MODE_SVLTE},
    {"CSFB_ONLY",       SYS_UE_MODE_CSFB_ONLY},
    {"SGLTE_ONLY",      SYS_UE_MODE_SGLTE_ONLY},
    {"SVLTE_ONLY",      SYS_UE_MODE_SVLTE_ONLY},
    {"1X_CSFB_PREF",    SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED},
    {"GSM_CSFB_PREF",   SYS_UE_MODE_GSMSRLTE_CSFB_PREFERRED},
    {"GSMSRLTE_ONLY",   SYS_UE_MODE_GSMSRLTE_ONLY},
    {"1XSRLTE_ONLY",    SYS_UE_MODE_1XSRLTE_ONLY},
  };

  size_t  i;
  char    token[32];

  policyman_get_token(&pUe_mode, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      ue_mode = map[i].ue_mode;
      break;
    }
  }

  return ue_mode;
}


/*-------- execute_ue_mode --------*/
static boolean execute_ue_mode(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  ue_mode_action_t         *pueAction = (ue_mode_action_t *) pAction;
  policy_execute_ctx_t     *pueCtx    = (policy_execute_ctx_t *)pCtx;
  policyman_ue_mode_item_t *prc;

  if (pueAction->ue_mode == SYS_UE_MODE_NONE)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "<ue_mode> none attempted, action not taken", ACTION_SUBS(pueAction));
    return TRUE;
  }

  prc = policyman_ue_mode_item_new(pueAction->ue_mode, subsId);
  policyman_set_replace(pueCtx->pItemSet, &prc);
  ref_cnt_obj_release(prc);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "action <ue_mode> set mode to %d",
                        ACTION_SUBS(pueAction),
                        LINE_NUM(pueAction),
                        prc->mode );
  return TRUE;
}


/*-------- policyman_ue_mode_execute --------*/
static boolean policyman_ue_mode_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_ue_mode);
}


/*-------- policyman_ue_mode_if_execute --------*/
static boolean execute_ue_mode_if(
  policyman_action_t const       *pAction,
  void                           *pCtx,
  sys_modem_as_id_e_type          subsId
)
{
  ue_mode_action_t      *pueAction      = (ue_mode_action_t *) pAction;
  boolean                succeeded       = TRUE;
  policy_execute_ctx_t  *pGetCtx         = (policy_execute_ctx_t *) pCtx;
  boolean                item_is_present;

  
  item_is_present = policyman_cfgitem_is_present_in_itemset( (policyman_set_t *)pGetCtx->pItemSet,
                                                              POLICYMAN_ITEM_UE_MODE,
                                                              subsId,
                                                              NULL);

  if (item_is_present)
  {
    POLICYMAN_UT_MSG_2( SUBS_PREFIX SRC_LINE "<ue_mode_if> skipping update - item present",
                        ACTION_SUBS(pueAction),
                        LINE_NUM(pueAction) );
  }
  else
  {
    succeeded = policyman_ue_mode_execute((mre_action_t const  *)pAction, pCtx);
  }

  return succeeded;
}


/*-------- policyman_ue_mode_if_execute --------*/
STATIC boolean policyman_ue_mode_if_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  return policyman_execute_action((policyman_action_t const *)pAction, pCtx, execute_ue_mode_if);
}


/*-------- policyman_ue_mode_create_action --------*/
mre_status_t policyman_ue_mode_create_new_action(
  policyman_xml_element_t const  *pElem,
  policyman_policy_t             *pPolicy,
  mre_action_t                  **ppAction,
  pfn_execute_t                   actionfn
)
{
  mre_status_t         status  = MRE_STATUS_ERR_INVALID_ACTION;
  ue_mode_action_t    *pAction = NULL;
  sys_ue_mode_e_type   mode;
  char const          *pStr;
  policyman_subs_t     pm_subs;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_2(SUBS_PREFIX SRC_LINE "<ue_mode> requires a ue_mode",
                       POLICY_SUBS(pPolicy),
                       mre_xml_get_line_number(pElem) );
    goto Done;
  }

  mode = policyman_str_to_ue_mode(pStr);

  if (mode == SYS_UE_MODE_NONE)
  {
    POLICYMAN_UT_MSG_2( SUBS_PREFIX "<ue_mode> invalid ue_mode: %s",
                        POLICY_SUBS(pPolicy),
                        pStr );
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

  pAction = POLICYMAN_ACTION_NEW( ue_mode_action_t,
                                  (mre_policy_t *)pPolicy,
                                  actionfn,
                                  policyman_action_dtor,
                                  pm_subs,
                                  pElem );

  pAction->ue_mode = mode;

  *ppAction = (mre_action_t *) pAction;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- policyman_ue_mode_action_new --------*/
mre_status_t policyman_ue_mode_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_ue_mode_create_new_action( pElem,
                                              (policyman_policy_t *) pPolicy,
                                              ppAction,
                                              policyman_ue_mode_execute );
}


/*-------- policyman_ue_mode_if_action_new --------*/
mre_status_t policyman_ue_mode_if_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_ue_mode_create_new_action( pElem,
                                              (policyman_policy_t *) pPolicy,
                                              ppAction,
                                              policyman_ue_mode_if_execute );
}


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  sys_ue_mode_e_type  ue_mode;
} policyman_ue_mode_condition_t;


/*-------- evaluate_ue_mode_is --------*/
static boolean evaluate_ue_mode_is(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
)
{
  policyman_ue_mode_condition_t *pCond           = (policyman_ue_mode_condition_t *) pCondition;
  policyman_status_t             status;
  policyman_item_id_t            id              = POLICYMAN_ITEM_UE_MODE;
  policyman_ue_mode_item_t      *pItem           = NULL;
  sys_ue_mode_e_type             current_ue_mode = SYS_UE_MODE_NONE;
  boolean                        result          = FALSE;

  pItem = policyman_cfgitem_find_item_in_context( pCtx, id, subsId );

  if (NULL != pItem)
  {
    status = policyman_get_ue_mode((policyman_item_t const *)pItem, &current_ue_mode);

    if (POLICYMAN_SUCCEEDED(status))
    {
      result = (current_ue_mode == pCond->ue_mode);
    }
  }

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <ue_mode_is pmsubs=%d> returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        result );

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return result;
}


/*-------- policyman_condition_ue_mode_evaluate --------*/
static boolean policyman_condition_ue_mode_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_ue_mode_is);
}


/*-------- policyman_condition_ue_mode_new --------*/
mre_status_t policyman_condition_ue_mode_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  mre_status_t                   status     = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_ue_mode_condition_t *pCondition;
  char const                    *pStr;
  sys_ue_mode_e_type             ueMode;
  policyman_subs_t               pm_subs;

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<ue_mode_is>: ue_mode required", POLICY_SUBS(pPolicy));
    goto Done;
  }

  ueMode = policyman_str_to_ue_mode(pStr);

  if (ueMode == SYS_UE_MODE_NONE)
  {
    POLICYMAN_UT_MSG_2( SUBS_PREFIX "<ue_mode_is> invalid ue_mode: %s",
                        POLICY_SUBS(pPolicy),
                        pStr );
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "<ue_mode_is> invalid 'subs' value for condition", POLICY_SUBS(pPolicy));
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_ue_mode_condition_t,
                                        pPolicy,
                                        policyman_condition_ue_mode_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  pCondition->ue_mode = ueMode;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- policyman_ue_mode_fill_subs_diag_log --------*/
void policyman_ue_mode_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  policyman_ue_mode_item_t     *pUeMode = (policyman_ue_mode_item_t *)pItem;

  log_ptr->ue_mode = (int8)pUeMode->mode;
}

/*-------- policyman_ue_mode_get_limited_item --------*/
void policyman_ue_mode_get_limited_item(
  mre_set_t              *pItemSet,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_ue_mode_item_t *pItem;

  pItem = policyman_ue_mode_item_new(SYS_UE_MODE_NORMAL, subsId); 

  mre_set_add(pItemSet, &pItem);
  ref_cnt_obj_release(pItem);
}

