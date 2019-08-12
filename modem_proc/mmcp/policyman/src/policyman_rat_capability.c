/**
  @file policy_rat_capability.c

  @brief  Policy to modify the RAT capability of the device based on the IIN
          of the current SIM.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_rat_capability.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"

#include "policyman_rat_capability.h"
#include "policyman_cfgitem.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_policies.h"
#include "policyman_rf.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_svc_mode.h"
#include "policyman_task.h"
#include "policyman_ue_mode.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include "event.h"
#include "sys.h"
#include <stringl/stringl.h>


#define RAT_CAPABILITY_TAG         "rat_capability"
#define RAT_CAPABILITY_IF_TAG      "rat_capability_if"

typedef struct
{
  BASE_EXECUTE_CTX;

  boolean    includeHdrOnDds;
  boolean    wcdmaPresentOnDds;
  boolean    nonDDSvolte;
  boolean    tdscdmaPresentOnDds;
} rat_execute_ctx_t;


/*=============================================================================
  Rat capability methods for cfgitem
=============================================================================*/

/*-------- policyman_rat_capability_item_new --------*/
policyman_item_t * policyman_rat_capability_item_new(
  policy_execute_ctx_t   *pCtx,
  uint32                  mask
)
{
  rat_capability_t *prc;
  uint32            subphoneMask;

  prc = (rat_capability_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_RAT_CAPABILITY,
                                                         sizeof(rat_capability_t),
                                                         policyman_simple_dtor );

  /*  Filter the RATs based on subphone capabilities.
   */
  subphoneMask = policyman_rat_capability_get_max_config(pCtx); 
  if (subphoneMask == 0)
  {
    subphoneMask = policyman_rf_get_overall_rat_mask();
  }

  prc->ratMask     = mask;
  prc->subphoneCap = subphoneMask;
  prc->asubs_id    = pCtx->asubs_id;

  return (policyman_item_t *) prc;
}


/*-------- policyman_rat_capability_get_default_from_efs --------*/
policyman_status_t policyman_rat_capability_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
)
{
  policyman_item_t         *pRc        = NULL;
  device_configuration_t   *pDc        = NULL;
  uint32                    ratMask;
  policy_execute_ctx_t      ctx;
  policyman_status_t        status     = POLICYMAN_STATUS_SUCCESS;

  // read EFS for RAT/BAND capability
  if (!policyman_retrieve_rats_bands(pCfgCtx->asubs_id, &ratMask, NULL))
  {
    status = POLICYMAN_STATUS_ERR;
    goto Done;
  }

  ctx.pItemSet = pCfgCtx->pItemSet;
  ctx.asubs_id = pCfgCtx->asubs_id;

  pRc = policyman_rat_capability_item_new(&ctx, ratMask);
  pCfgCtx->pItem = pRc;

Done:

  REF_CNT_OBJ_RELEASE_IF(pDc);
  return status;
}


/*-------- policyman_rat_capability_get_default --------*/
policyman_item_t * policyman_rat_capability_get_default(
  policy_execute_ctx_t *pCtx
)
{
  uint32                mask;
  rat_capability_t     *prc;
  uint32                rat_disable_mask;

  rat_disable_mask = policyman_state_get_thermal_rat_disable_mask();
  mask             = policyman_rf_get_overall_rat_mask();

  /* Apply rat disable mask becuase of thermal runaway
  */
  mask &= ~rat_disable_mask;

  prc = (rat_capability_t *)policyman_rat_capability_item_new(pCtx, mask);
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "Default RAT capability mask 0x%04x",
                        pCtx->asubs_id,
                        prc->ratMask );

  return (policyman_item_t *)prc;
}

/*-------- policyman_rat_capability_get_base_rats --------*/
STATIC uint32 policyman_rat_capability_get_base_rats(
  policyman_item_t const *pItem
)
{
  rat_capability_t  *prc  = (rat_capability_t *)pItem;  

  return prc->ratMask;
}

/*-------- policyman_rat_capability_get_effective_rats --------*/
STATIC uint32 policyman_rat_capability_get_effective_rats(
  policyman_item_t *pItem
)
{
  uint32             ratCap;  
  rat_capability_t  *prc     = (rat_capability_t *)pItem;  

  ratCap  = prc->ratMask | prc->ratIncludeMask;
  ratCap &= ~ prc->ratExcludeMask;
  ratCap &= prc->subphoneCap;

  return ratCap;
}


/*-------- policyman_rat_capability_compare --------*/
boolean policyman_rat_capability_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
  )
{
  return  (policyman_rat_capability_get_effective_rats(pData1) == policyman_rat_capability_get_effective_rats(pData2));
}

/*-------- policyman_rat_capability_display --------*/
void policyman_rat_capability_display(
  policyman_item_t  *pItem
)
{
  rat_capability_t  *prc = (rat_capability_t *)pItem;

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX "Rat capability: Rat cap 0x%04x, Base 0x%04x, subphone cap 0x%04x, exclude mask 0x%04x",
                        prc->asubs_id, 
                        policyman_rat_capability_get_effective_rats(pItem),
                        prc->ratMask,
                        prc->subphoneCap,
                        prc->ratExcludeMask );
}

/*-------- policyman_rat_capability_on_update --------*/
void policyman_rat_capability_on_update(
  policyman_item_t  *pItem
)
{
  rat_capability_t                *prc           = (rat_capability_t *)pItem;
  policyman_diag_event_rat_cap_t   event_rat_cap;

  event_rat_cap.ratMask  = policyman_rat_capability_get_effective_rats(pItem);
  event_rat_cap.subs     = (int8)prc->asubs_id;

  POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "Sending EVENT_PM_RAT_CHANGE", prc->asubs_id);

  event_report_payload( EVENT_PM_RAT_CHANGE,
                        sizeof(event_rat_cap),
                        &event_rat_cap );

  return;
}

/*-------- policyman_get_rat_capability_base_for_subs --------*/
STATIC policyman_status_t policyman_get_rat_capability_base_for_subs(
  sys_modem_as_id_e_type  subsId,
  uint32                 *pRatMask
)
{
  size_t                nItems;
  policyman_status_t    status  = POLICYMAN_STATUS_ERR;
  policyman_item_t     *pItem   = NULL;
  policyman_item_id_t   id[]    = { POLICYMAN_ITEM_RAT_CAPABILITY };

  nItems = policyman_cfgitem_get_items_per_subs( (policyman_item_id_t const *)&id, 
                                                 ARR_SIZE(id), 
                                                 (policyman_item_t const **)&pItem, 
                                                 subsId);
  if (nItems == ARR_SIZE(id))
  {
    *pRatMask = policyman_rat_capability_get_base_rats(pItem);
     status  = POLICYMAN_STATUS_SUCCESS;
  }

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return status;
}

/*-------- policyman_rat_capability_clone --------*/
STATIC policyman_item_t *policyman_rat_capability_clone(
 policy_execute_ctx_t   *pCtx
)
{  
  uint32                currentMask = 0; 
  policyman_status_t    status;
  policyman_item_t     *prc         = NULL;  

  // get current RAT capabilities for subs and create a new object
  status = policyman_get_rat_capability_base_for_subs(pCtx->asubs_id, &currentMask);
  if (POLICYMAN_SUCCEEDED(status))
  {
    prc = policyman_rat_capability_item_new(pCtx, currentMask);
  }

  return prc;
}

/*-------- policyman_ensure_rat_capability_for_each_subs --------*/
STATIC void policyman_ensure_rat_capability_for_each_subs(
  rat_execute_ctx_t *pCtx
)
{
  size_t             nSim;
  size_t             subsId;
  policyman_status_t status;
  boolean            item_is_present;

  status = policyman_get_current_num_sim(&nSim);

  // For each subs add RAT capability to itemset if not present
  for (subsId = SYS_MODEM_AS_ID_1; POLICYMAN_SUCCEEDED(status) && subsId < nSim; subsId++)
  {
    item_is_present = policyman_cfgitem_is_present_in_itemset( pCtx->pItemSet,
                                                     POLICYMAN_ITEM_RAT_CAPABILITY, 
                                                     subsId,
                                                     NULL);
    if (!item_is_present)
    {      
      policyman_item_t     *prc;
      policy_execute_ctx_t  ctx;

      // create a new context for given subsId to clone the RAT capability
      ctx.pItemSet = pCtx->pItemSet;
      ctx.pState   = pCtx->pState;
      ctx.asubs_id = subsId;

      // get current RAT capabilities and add to set
      prc = policyman_rat_capability_clone(&ctx);
      
      if (NULL != prc)
      {
        policyman_set_add(pCtx->pItemSet, &prc);
        ref_cnt_obj_release(prc);
      }
    }
  }
}

/*-------- ue_has_3gpp2_subscription --------*/
STATIC boolean ue_has_3gpp2_subscription(
  policy_execute_ctx_t *pCtx
)
{
  cm_rtre_control_e_type  rtre_control;
  boolean                 sim_has_3gpp2_subs;
  boolean                 nv_has_3gpp2_subs;

  rtre_control = policyman_phone_get_rtre_control(pCtx->pState, pCtx->asubs_id);
 
  sim_has_3gpp2_subs = policyman_uim_sim_is_3gpp2((policy_execute_ctx_t *)pCtx);
  nv_has_3gpp2_subs = (rtre_control == CM_RTRE_CONTROL_NV);

  POLICYMAN_MSG_MED_3( SUBS_PREFIX "sim_is_3gpp2(T/F) %d, rtre_control_is_nv(T/F) %d",
                        pCtx->asubs_id,
                        sim_has_3gpp2_subs,
                        nv_has_3gpp2_subs);

  return (sim_has_3gpp2_subs || nv_has_3gpp2_subs);
}

/*-------- determine_concurrency_parameters --------*/
STATIC boolean determine_concurrency_parameters(
  void  *pElem,
  void  *pCtx
)
{
  policyman_item_t  *pItem         = *((policyman_item_t **)pElem);
  rat_execute_ctx_t *pRatCtx       = (rat_execute_ctx_t *) pCtx;

  if (policyman_cfgitem_id(pItem) == POLICYMAN_ITEM_RAT_CAPABILITY)
  {
    uint32                effectiveMask;

    effectiveMask = policyman_rat_capability_get_effective_rats(pItem);

    // SubsId in context here is DDS subs
    if (pItem->asubs_id == pRatCtx->asubs_id)
    {
      // HDR present on DDS SUBS only if it is a 3GPP2 card
      pRatCtx->includeHdrOnDds =   ue_has_3gpp2_subscription((policy_execute_ctx_t *)pCtx) 
                           && ((effectiveMask & SYS_SYS_MODE_MASK_HDR) != 0);

      // WCDMA present on DDS SUBS
      pRatCtx->wcdmaPresentOnDds = ((effectiveMask & SYS_SYS_MODE_MASK_WCDMA) != 0);

      // tdscdma present on DDS SUBS
      pRatCtx->tdscdmaPresentOnDds = ((effectiveMask & SYS_SYS_MODE_MASK_TDS) != 0);
    }   
    else
    {
      pRatCtx->nonDDSvolte = FALSE;
      policyman_get_subs_boolean_value("cm:volte_allowed", pItem->asubs_id, &(pRatCtx->nonDDSvolte));
    }

    POLICYMAN_MSG_MED_3( "includeHdrOnDds %d, wcdmaPresentOnDds %d, nonDDSvolte %d",
                          pRatCtx->includeHdrOnDds,
                          pRatCtx->wcdmaPresentOnDds,
                          pRatCtx->nonDDSvolte);
  }

  return TRUE;
}


/*-------- update_thermal_masks --------*/
STATIC boolean update_thermal_masks(
  void  *pElem,
  void  *pCtx
)
{
  policyman_item_t	*pItem	 = *((policyman_item_t **)pElem);
  
  if (policyman_cfgitem_id(pItem) == POLICYMAN_ITEM_RAT_CAPABILITY)
  {
    rat_capability_t      *prc = (rat_capability_t *)pItem;

    prc->ratExcludeMask |= policyman_state_get_thermal_rat_disable_mask();
  }

  return TRUE;
}

/*-------- update_concurrency_masks --------*/
STATIC boolean update_concurrency_masks(
  void  *pElem,
  void  *pCtx
)
{
  policyman_item_t    *pItem   = *((policyman_item_t **)pElem);
  rat_execute_ctx_t   *pRatCtx = (rat_execute_ctx_t *) pCtx;

  // work only on RAT capability item
  if (policyman_cfgitem_id(pItem) == POLICYMAN_ITEM_RAT_CAPABILITY)
  {
    sys_modem_as_id_e_type  ddsSubs   = pRatCtx->asubs_id;
    rat_capability_t       *prc     = (rat_capability_t *)pItem;

    prc->ratIncludeMask = 0;
    prc->ratExcludeMask = 0;

    // non-DDS subs
    if (pItem->asubs_id != ddsSubs)
    {

      /* Exclude data Rats from non-DDS SUBS
             Remove LTE if nonDDS VoLTE is not allowed
             Remove T/W if HDR present on DDS SUBS
             Remove TDS if WCDMA present on DDS SUBS
             Remove WCDMA if TDS present on DDS SUBS
         */
      prc->ratExcludeMask |= SYS_SYS_MODE_MASK_HDR;

      if (!pRatCtx->nonDDSvolte)
      {
        prc->ratExcludeMask |= SYS_SYS_MODE_MASK_LTE;
      }


      if (pRatCtx->includeHdrOnDds)
      {
        prc->ratExcludeMask |= (SYS_SYS_MODE_MASK_TDS | SYS_SYS_MODE_MASK_WCDMA);      
      }

      if (pRatCtx->wcdmaPresentOnDds)
      {
        prc->ratExcludeMask |= SYS_SYS_MODE_MASK_TDS;
      }

      if (pRatCtx->tdscdmaPresentOnDds)
      {
        prc->ratExcludeMask |= SYS_SYS_MODE_MASK_WCDMA;
      }
    }
    // Disable HDR on DDS subs if not allowed
    else if (pItem->asubs_id == ddsSubs && !pRatCtx->includeHdrOnDds)
    {
      prc->ratExcludeMask |= SYS_SYS_MODE_MASK_HDR;
    }
  }

  return TRUE;
}


/*-------- policyman_update_rat_for_thermal_level --------*/
void policyman_update_rat_for_thermal_level(
  policyman_set_t *pItemSet
)
{  
  policyman_set_iterate(pItemSet, update_thermal_masks, NULL);
}


/*-------- policyman_set_rat_capability_concurrency_masks --------*/
void policyman_set_rat_capability_concurrency_masks(
  policyman_set_t *pItemSet
)
{
  rat_execute_ctx_t ctx;

  ctx.pItemSet = pItemSet;
  ctx.pState   = policyman_state_get_state();
  ctx.asubs_id = policyman_subs_get_dds_subs(ctx.pState);
  
  // ensure itemset has RAT capability for all SUBS
  policyman_ensure_rat_capability_for_each_subs(&ctx);

    // get a context with info on required concurrent RATs on all SUBS
    mre_set_iterate(pItemSet, determine_concurrency_parameters, &ctx);

  // update concurrency masks for all RAT capability items
    mre_set_iterate(pItemSet, update_concurrency_masks, &ctx);
}


/*-----------------------------------------------------------------------------
  rat_capability action
-----------------------------------------------------------------------------*/


typedef struct
{
  POLICYMAN_ACTION_BASE;

  rat_capability_info_t rat_info;
} rat_capability_action_t;


#define INVALID_RAT_MASK  0xffffffff


/*-------- policyman_get_rat_capability --------*/
policyman_status_t policyman_get_rat_capability(
  policyman_item_t const  *pItem,
  uint32                  *pRatCfg
  )
{
  if (pItem == NULL || pRatCfg == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_rat_capability returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_RAT_CAPABILITY)
  {
    POLICYMAN_MSG_ERROR_0("get_rat_capability returned err POLICYMAN_STATUS_ERR_INVALID_ITEM_ID");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pRatCfg = policyman_rat_capability_get_effective_rats((policyman_item_t *)pItem);
  
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "get_rat_capability returned 0x%04x",
                        pItem->asubs_id,
                        *pRatCfg );

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_get_rat_capability_for_subs --------*/
policyman_status_t policyman_get_rat_capability_for_subs(
  sys_modem_as_id_e_type  subsId,
  uint32                 *pRatMask
)
{
  size_t                nItems;
  policyman_status_t    status  = POLICYMAN_STATUS_ERR;
  policyman_item_t     *pItem   = NULL;
  policyman_item_id_t   id[]    = { POLICYMAN_ITEM_RAT_CAPABILITY };

  nItems = policyman_cfgitem_get_items_per_subs( (policyman_item_id_t const *)&id, 
                                                 ARR_SIZE(id), 
                                                 (policyman_item_t const **)&pItem, 
                                                 subsId);
  if (nItems == ARR_SIZE(id))
  {
    status = policyman_get_rat_capability(pItem, pRatMask);
  }

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return status;
}

/*-------- policyman_rat_capability_str_to_rat --------*/
uint32 policyman_rat_capability_str_to_rat(
  const char  *pRatName
  )
{
  rat_map *rat_item;
  uint32  mask = INVALID_RAT_MASK;

  rat_item = policyman_util_str_to_rat(pRatName);

  if (NULL != rat_item)
  {
    mask = rat_item->mask;
  }

  return mask;
}



/*-------- policyman_rat_config_parse_rats --------*/
boolean policyman_rat_config_parse_rats(
  char const  *pStr,
  uint32      *pMask
  )
{
  char    token[32];
  boolean succeeded = TRUE;
  uint32  mask;

  while (policyman_get_token(&pStr, token, sizeof(token)))
  {
    mask = policyman_rat_capability_str_to_rat(token);
    if (mask == INVALID_RAT_MASK)
    {
      POLICYMAN_UT_MSG_1("invalid RAT specified: %s", token);
      succeeded = FALSE;
      break;
    }
    *pMask |= mask;
  }

  return succeeded;
}


/*-------- policyman_rat_capability_read_info --------*/
policyman_status_t policyman_rat_capability_read_info(
  policyman_xml_element_t const *pElem,
  rat_capability_info_t         *pInfo
  )
{
  policyman_status_t status = POLICYMAN_STATUS_ERR_INVALID_ACTION;
  size_t            nChildren;
  size_t            i;

  pInfo->base = POLICYMAN_BASE_NONE;
  pInfo->includeMask = 0;
  pInfo->excludeMask = 0;

  if (!policyman_util_get_base(pElem, &pInfo->base))        { goto Done; }

  nChildren = policyman_xml_num_children(pElem);
  if (nChildren == 0)
  {
    status = POLICYMAN_STATUS_SUCCESS;
    goto Done;
  }

  for (i = 0 ; i < nChildren ; ++i)
  {
    policyman_xml_element_t const *pChild;
    char const                  *pStr;

    pChild = policyman_xml_get_child(pElem, i);
    if (pChild == NULL)
    {
      POLICYMAN_MSG_ERROR_1("Internal error - no element #%d", i);
      goto Done;
    }

    pStr = policyman_xml_get_text(pChild);
    if      (policyman_xml_tag_is(pChild, "include"))
    {
      if (!policyman_rat_config_parse_rats(pStr, &pInfo->includeMask)) { goto Done; }
    }
    else if (policyman_xml_tag_is(pChild, "exclude"))
    {
      if (!policyman_rat_config_parse_rats(pStr, &pInfo->excludeMask)) { goto Done; }
    }
    else
    {
      goto Done;
    }
  }

  status = POLICYMAN_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- policyman_get_configured_rats --------*/
static uint32 policyman_get_configured_rats(
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_item_t const  *pItem;
  uint32                   mask  = 0;

  pItem = policyman_cfgitem_get_item(
              POLICYMAN_ITEM_DEVICE_CONFIGURATION,
              SYS_MODEM_AS_ID_1
              );

  if (pItem != NULL)
  {
  mask = device_config_get_current_config((device_configuration_t const *)pItem, asubs_id);
  policyman_item_release(pItem);
  }
  else
  {
    policyman_cfgitem_set_limited_capabilities();
  }

  return mask;
}


/*-------- policyman_rat_capability_get_max_config --------*/
uint32 policyman_rat_capability_get_max_config(
  policy_execute_ctx_t *prcCtx
)
{
  uint32                   config_mask = 0;
  device_configuration_t  *pdcItem    = NULL;
  boolean                  item_is_present = FALSE;

    item_is_present = policyman_cfgitem_is_present_in_itemset( (policyman_set_t *)prcCtx->pItemSet,
                                                                POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                                SYS_MODEM_AS_ID_1,
                                                                (policyman_item_t  **)&pdcItem);

  if (item_is_present)
  {
    config_mask = device_config_get_current_config(pdcItem, (size_t)prcCtx->asubs_id);
  }
  else
  {
    config_mask = policyman_get_configured_rats(prcCtx->asubs_id);
  }

  return config_mask;
}


/*-------- policyman_rat_capability_evaluate --------*/
uint32 policyman_rat_capability_evaluate(
  rat_capability_info_t *pInfo,
  policy_execute_ctx_t  *prcCtx
)
{
  uint32                        mask = 0;

  switch (pInfo->base)
  {
    case POLICYMAN_BASE_NONE:
      mask = 0;
      break;

    case POLICYMAN_BASE_CONFIG:
      mask = policyman_rat_capability_get_max_config(prcCtx);
      break;

    case POLICYMAN_BASE_HARDWARE:
      mask = policyman_rf_get_overall_rat_mask();
      break;

    default:
      POLICYMAN_MSG_ERROR_1("invalid base type: %d", pInfo->base);
      break;
  }

  mask |= pInfo->includeMask;
  mask &= ~(pInfo->excludeMask);

  return mask;
}


/*-------- execute_rat_capability --------*/
static boolean execute_rat_capability(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  rat_capability_action_t *prcAction = (rat_capability_action_t *) pAction;
  policy_execute_ctx_t    *prcCtx    = (policy_execute_ctx_t *) pCtx;
  rat_capability_t        *prc;
  uint32                   mask;

  mask = policyman_rat_capability_evaluate(&prcAction->rat_info, prcCtx);
  prcCtx->asubs_id = subsId;

  /* if none rat capability is attempted, enable GSM */
  if (mask == SYS_SYS_MODE_MASK_NONE)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "<rat_capability> none attempted, action not taken", subsId);
    return TRUE;
  }

  prc = (rat_capability_t *)policyman_rat_capability_item_new(prcCtx, mask);
  policyman_set_replace(prcCtx->pItemSet, &prc);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "action <rat_capability> set Base to 0x%04x",
                        subsId,
                        LINE_NUM(prcAction),
                        prc->ratMask );

  ref_cnt_obj_release(prc);
  return TRUE;
}


/*-------- policyman_rat_capability_execute --------*/
STATIC boolean policyman_rat_capability_execute(
  mre_action_t const  *pAction,
  void                *pCtx
  )
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_rat_capability);
}


/*-------- execute_rat_capability_if --------*/
STATIC boolean execute_rat_capability_if(
  policyman_action_t const       *pAction,
  void                           *pCtx,
  sys_modem_as_id_e_type          subsId
)
{
  rat_capability_action_t *pratAction      = (rat_capability_action_t *)pAction;
  boolean                  succeeded       = TRUE;
  boolean                  item_is_present;
  policy_execute_ctx_t    *pGetCtx         = (policy_execute_ctx_t *) pCtx;

  item_is_present = policyman_cfgitem_is_present_in_itemset( (policyman_set_t *)pGetCtx->pItemSet,
                                                             POLICYMAN_ITEM_RAT_CAPABILITY,
                                                             subsId,
                                                             NULL);

  if (item_is_present)
  {
    succeeded = TRUE;
    POLICYMAN_UT_MSG_3( SUBS_PREFIX SRC_LINE "<rat_capability_if pmsubs=%d> skipping update - item present",
                        ACTION_SUBS(pratAction),
                        LINE_NUM(pratAction),
                        pratAction->pmSubsId );

  }
  else
  {
    succeeded = policyman_rat_capability_execute((mre_action_t const  *)pAction, pCtx);
  }

  return succeeded;
}

/*-------- policyman_rat_capability_if_execute --------*/
STATIC boolean policyman_rat_capability_if_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  return policyman_execute_action((policyman_action_t const *)pAction, pCtx, execute_rat_capability_if);
}


/*-------- policyman_rat_capability_create_new_action --------*/
mre_status_t policyman_rat_capability_create_new_action(
  policyman_xml_element_t const  *pElem,
  policyman_policy_t             *pPolicy,
  mre_action_t                  **ppAction,
  pfn_execute_t                   executefn
)
{
  mre_status_t             status  = MRE_STATUS_ERR_INVALID_ACTION;
  rat_capability_action_t *pAction = NULL;
  policyman_subs_t         pm_subs;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( rat_capability_action_t,
                                  (mre_policy_t *)pPolicy,
                                  executefn,
                                  policyman_action_dtor,
                                  pm_subs,
                                  pElem );

  status = policyman_rat_capability_read_info(pElem, &pAction->rat_info);

Done:
  if (MRE_SUCCEEDED(status))
  {
    *ppAction = (mre_action_t *) pAction;
  }
  else
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<rat_capability>: invalid XML", POLICY_SUBS(pPolicy));
    REF_CNT_OBJ_RELEASE_IF(pAction);
  }

  return status;
}


/*-------- policyman_rat_capability_action_new --------*/
mre_status_t policyman_rat_capability_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
  return policyman_rat_capability_create_new_action(
                pElem, (policyman_policy_t *) pPolicy, ppAction,
                policyman_rat_capability_execute);
}

/*-------- policyman_rat_capability_if_action_new --------*/
mre_status_t policyman_rat_capability_if_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
 return policyman_rat_capability_create_new_action(
          pElem, (policyman_policy_t *) pPolicy, ppAction,
          policyman_rat_capability_if_execute);
}

typedef struct
{
  POLICYMAN_CMD_HDR;

  sys_ue_mode_e_type    ue_mode;
  policyman_svc_mode_t  svc_mode;
} policyman_full_rat_mode_cmd_t;


/*-------- policyman_retrieve_rats_bands --------*/
boolean policyman_retrieve_rats_bands(
  sys_modem_as_id_e_type   subsId,
  uint32                  *pRatMask,
  rf_efs_bands_t          *pRetrievedBands
)
{
  mre_efs_status_t          efsStatus      = MRE_EFS_STATUS_ERROR;
  rat_capability_efs_t     *rat_cap_buffer = NULL;
  size_t                    rat_file_size  = 0;

  efsStatus = mre_efs_get_file( POLICYMAN_RAT_MASK_NV_PATH,
                                (void **)&rat_cap_buffer,
                                &rat_file_size,
                                subsId );

  if (MRE_EFS_STATUS_SUCCESS == efsStatus)
  {
    if (pRatMask != NULL)
    {
      *pRatMask = rat_cap_buffer->rat_mask;
    }

    if (pRetrievedBands != NULL)
    {
      pRetrievedBands->bands.gwBands  = rat_cap_buffer->gw_band_mask;
      pRetrievedBands->bands.tdsBands = rat_cap_buffer->tds_band_mask;
      pRetrievedBands->bands.lteBands = rat_cap_buffer->lte_band_mask;

    }

    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "---------- RATs and bands from EFS; RAT mask: 0x%04x", subsId, rat_cap_buffer->rat_mask);
    policyman_rf_print_bands( subsId, 
                              rat_cap_buffer->gw_band_mask,
                              rat_cap_buffer->lte_band_mask,
                              rat_cap_buffer->tds_band_mask );
  }

  POLICYMAN_MEM_FREE_IF(rat_cap_buffer);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "policyman_retrieve_rats_bands: status %d, filesize %d",
                        subsId,
                        efsStatus,
                        rat_file_size );

  return (efsStatus == MRE_EFS_STATUS_SUCCESS);
}

/*-------- policyman_rats_bands_persist --------*/
STATIC boolean policyman_rats_bands_persist(
  sys_modem_as_id_e_type     subsId,
  uint32                    *pRatMask,
  rf_efs_bands_t            *pWriteBands
)
{
  boolean                   write_status;
  rat_capability_efs_t      rat_cap_buffer;

  rat_cap_buffer.rat_mask      = *pRatMask;
  rat_cap_buffer.gw_band_mask  = pWriteBands->bands.gwBands;
  rat_cap_buffer.lte_band_mask = pWriteBands->bands.lteBands;
  rat_cap_buffer.tds_band_mask = pWriteBands->bands.tdsBands;

  write_status = policyman_cfgitem_persist_item( POLICYMAN_RAT_MASK_NV_PATH,
                                                 (void *)&rat_cap_buffer,
                                                 sizeof(rat_cap_buffer),
                                                 subsId );

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX "Wrote RAT mask to EFS: base 0x%04x, status %d, filesize %d",
                        subsId,
                        *pRatMask,
                        write_status,
                        sizeof(rat_cap_buffer) );

  return write_status;
}

/*-------- policyman_retrieve_rats_bands_per_subs --------*/
policyman_status_t policyman_retrieve_rats_bands_per_subs(
  size_t            subs,
  uint32           *pRatMask,
  rf_efs_bands_t   *pRetrievedBands
)
{
  size_t                nItems;
  policyman_status_t    status   = POLICYMAN_STATUS_ERR;
  policyman_item_t     *pRatcfg  = NULL;
  policyman_item_id_t   ratId    = POLICYMAN_ITEM_RAT_CAPABILITY;
  policyman_item_t     *pBandcfg = NULL;
  policyman_item_id_t   rfId     = POLICYMAN_ITEM_RF_BANDS;

  nItems = policyman_cfgitem_get_items_per_subs( &ratId,
                                                 1 ,
                                                 (policyman_item_t const **)&pRatcfg,
                                                 subs
                                               );
  if (nItems != 1)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "policyman_get_items failed for RAT capability", subs);
    goto Done;
  }

  *pRatMask = policyman_rat_capability_get_base_rats((policyman_item_t const *)pRatcfg);

  nItems = policyman_cfgitem_get_items_per_subs( &rfId,
                                                 1 ,
                                                 (policyman_item_t const **)&pBandcfg,
                                                 subs
                                               );
  if (nItems != 1)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "policyman_get_items failed for Band capability", subs);
    goto Done;
  }
  status = policyman_get_rf_bands( (policyman_item_t const *)pBandcfg,
                                   &pRetrievedBands->bands.gwBands,
                                   &pRetrievedBands->bands.lteBands,
                                   &pRetrievedBands->bands.tdsBands);

Done:

  REF_CNT_OBJ_RELEASE_IF(pRatcfg);
  REF_CNT_OBJ_RELEASE_IF(pBandcfg);
  return status;
}

/*-------- policyman_update_rats_bands_to_efs --------*/
void policyman_update_rats_bands_to_efs(
  uint8 subsMask
)
{
  policyman_status_t     status;
  boolean                write_status;
  uint32                 ratMask;
  rf_efs_bands_t         bandsToWrite;

  size_t                 subs;
  size_t                 nSim      = 0;

  (void)policyman_get_current_num_sim(&nSim);

  for (subs = 0; subs < nSim; subs++)
  {
    status = policyman_retrieve_rats_bands_per_subs(subs, &ratMask, &bandsToWrite);
    if (!POLICYMAN_SUCCEEDED(status))
    {
      POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "Failed to retrieve rat/band information, skipping EFS write", subs);
      continue;
    }

    /* Write data to EFS file
    */
    write_status = policyman_rats_bands_persist( (sys_modem_as_id_e_type)subs,
                                                 &ratMask,
                                                 &bandsToWrite );

    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "update_rats_bands_to_efs: write_status %d",
                          subs,
                          write_status );
  }
}

/*-------- policyman_update_rats_bands_to_efs_per_tech --------*/
void policyman_update_rats_bands_to_efs_per_tech(
  uint32  rat_include
)
{
  policyman_status_t     status;
  boolean                write_status;
  uint32                 ratMask;
  rf_efs_bands_t         bandsToWrite;
  sys_band_mask_type     hwGwBand;
  sys_band_mask_type     hwTdsBand;
  sys_modem_as_id_e_type subs;

  subs = SYS_MODEM_AS_ID_1;

  policyman_get_hardware_bands_all_techs( &hwGwBand, NULL, &hwTdsBand );

  status = policyman_retrieve_rats_bands_per_subs( subs, &ratMask, &bandsToWrite);
  if (!POLICYMAN_SUCCEEDED(status))
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "Failed to retrieve rat/band information, skipping EFS write", subs);
    return;
  }

  if ( rat_include & SYS_SYS_MODE_MASK_WCDMA )
  {
    ratMask |= SYS_SYS_MODE_MASK_WCDMA;
    bandsToWrite.bands.gwBands  |= hwGwBand & ALL_WCDMA_BANDS;
  }
  else
  {
    ratMask = ratMask & ~SYS_SYS_MODE_MASK_WCDMA;
    bandsToWrite.bands.gwBands  = bandsToWrite.bands.gwBands & ~ALL_WCDMA_BANDS;
  }

  if ( rat_include & SYS_SYS_MODE_MASK_TDS )
  {
    ratMask |= SYS_SYS_MODE_MASK_TDS;
    bandsToWrite.bands.tdsBands = hwTdsBand;
  }
  else
  {
    ratMask = ratMask & ~SYS_SYS_MODE_MASK_TDS;
    bandsToWrite.bands.tdsBands = 0;
  }

  /* Write data to EFS file
   */
  write_status = policyman_rats_bands_persist(subs, &ratMask, &bandsToWrite);

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "update_rats_bands_to_efs_per_tech: write_status %d",
                        subs,
                        write_status );
}


/*-------- policyman_rat_capability_fill_subs_diag_log --------*/
void policyman_rat_capability_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  rat_capability_t  *pRatCap = (rat_capability_t *)pItem;

  log_ptr->rat_mask = pRatCap->ratMask;
}

/*-------- policyman_rat_capability_get_limited_item --------*/
void policyman_rat_capability_get_limited_item(
  mre_set_t             *pItemSet,
  sys_modem_as_id_e_type subsId
)
{
  rat_capability_t *pRc;
  policy_execute_ctx_t  ctx;

  pRc = (rat_capability_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_RAT_CAPABILITY,
                                                         sizeof(rat_capability_t),
                                                         policyman_simple_dtor );

  ctx.pItemSet = pItemSet;
  ctx.asubs_id = subsId;

  // A rat Mask of None ensures no acqusition attempts in lower layers
  pRc->ratMask     = 0;
  pRc->subphoneCap = policyman_rat_capability_get_max_config(&ctx);
  pRc->asubs_id    = subsId;

  mre_set_add(pItemSet, &pRc);
  ref_cnt_obj_release(pRc);
}

