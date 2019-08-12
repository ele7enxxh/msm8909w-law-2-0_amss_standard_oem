/**
  @file policyman_scan_optimization.c

  @brief File to handle scan optimization configuration item.
*/

/*
    Copyright (c) 2015,2016  QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_scan_optimization.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre.h"
#include "policyman_i.h"
#include "policyman_policies.h"
#include "policyman_scan_optimization.h"
#include "policyman_set.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_xml.h"
#include "sys.h"



#include <stringl/stringl.h>

/*=============================================================================

=============================================================================*/

typedef struct
{
  POLICYMAN_ACTION_BASE;

  policyman_3gpp_scan_opt_t  tgpp_scan_opt;
  policyman_3gpp2_scan_opt_t tgpp2_scan_opt;
} scan_opt_action_t;

/*-------- policyman_scan_opt_item_new --------*/
policyman_scan_opt_item_t * policyman_scan_opt_item_new(
  sys_modem_as_id_e_type      subsId,
  policyman_3gpp_scan_opt_t   tgpp_scan_opt,
  policyman_3gpp2_scan_opt_t  tgpp2_scan_opt
)
{
  policyman_scan_opt_item_t  *pItem;

  pItem = (policyman_scan_opt_item_t *)policyman_cfgitem_new_item( POLICYMAN_ITEM_SCAN_OPTIMIZATION,
                                                                   sizeof(policyman_scan_opt_item_t),
                                                                   policyman_simple_dtor );

  pItem->asubs_id       = subsId;
  pItem->tgpp2_scan_opt = tgpp2_scan_opt;
  pItem->tgpp_scan_opt  = tgpp_scan_opt;

  return pItem;
}


/*-------- policyman_scan_opt_get_default --------*/
policyman_item_t *policyman_scan_opt_get_default(
  policy_execute_ctx_t *pCtx
)
{
  policyman_scan_opt_item_t  *pItem;

  pItem = policyman_scan_opt_item_new( pCtx->asubs_id,
                                       POLICYMAN_3GPPSCAN_OPT_DISABLED,
                                       POLICYMAN_3GPP2SCAN_OPT_DISABLED );

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "default scan optimization: 3gpp %d, 3gpp2 %d",
                        pCtx->asubs_id,
                        pItem->tgpp_scan_opt,
                        pItem->tgpp2_scan_opt );

  return (policyman_item_t *)pItem;
}

/*-------- policyman_scan_opt_compare --------*/
boolean policyman_scan_opt_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
)
{
  policyman_scan_opt_item_t  *scanOpt1 = (policyman_scan_opt_item_t *)pData1;
  policyman_scan_opt_item_t  *scanOpt2 = (policyman_scan_opt_item_t *)pData2;

  return ( (scanOpt1->tgpp_scan_opt  == scanOpt2->tgpp_scan_opt)
          && (scanOpt1->tgpp2_scan_opt == scanOpt2->tgpp2_scan_opt));
}

/*-------- policyman_scan_opt_display --------*/
void policyman_scan_opt_display(
  policyman_item_t  *pItem
)
{
  policyman_scan_opt_item_t  *scanOpt = (policyman_scan_opt_item_t *)pItem;

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "Scan Optimization: 3gpp %d, 3gpp2 %d",
                        scanOpt->asubs_id,
                        scanOpt->tgpp_scan_opt,
                        scanOpt->tgpp2_scan_opt );
}

/*-------- policyman_str_to_3gpp_opt --------*/
mre_status_t policyman_str_to_3gpp_opt(
  const char                 *pStr,
  policyman_3gpp_scan_opt_t  *pTgpp_scan_opt
)
{
  typedef struct
  {
    const char                *pStr;
    policyman_3gpp_scan_opt_t  scanOpt;
  } mask_map;

  static mask_map map[] =
  {
    {"disabled",           POLICYMAN_3GPPSCAN_OPT_DISABLED},
    {"no_auto_camp_only",  POLICYMAN_3GPPSCAN_OPT_NOCAMPMODE}
  };

  size_t       idx;
  char         token[18];
  mre_status_t status = MRE_STATUS_ERR_MALFORMED_XML;

  *pTgpp_scan_opt = POLICYMAN_3GPPSCAN_OPT_DISABLED;

  policyman_get_token(&pStr, token, sizeof(token));

  for (idx = 0 ; idx < ARR_SIZE(map) ; ++idx)
  {
    if (strcasecmp(token, map[idx].pStr) == 0)
    {
      *pTgpp_scan_opt = map[idx].scanOpt;
      status = MRE_STATUS_SUCCESS;
      break;
    }
  }
  return status;
}

/*-------- policyman_str_to_3gpp2_opt --------*/
mre_status_t policyman_str_to_3gpp2_opt(
  const char                  *pStr,
  policyman_3gpp2_scan_opt_t  *pTgpp2_scan_opt
)
{
  typedef struct
  {
    const char                 *pStr;
    policyman_3gpp2_scan_opt_t  scanOpt;
  } mask_map;

  static mask_map map[] =
  {
    {"disabled",   POLICYMAN_3GPP2SCAN_OPT_DISABLED},
    {"wrlf",       POLICYMAN_3GPP2SCAN_OPT_WRLF},
    {"all",        POLICYMAN_3GPP2SCAN_OPT_ALL}
  };

  size_t       idx;
  char         token[9];
  mre_status_t status = MRE_STATUS_ERR_MALFORMED_XML;

  *pTgpp2_scan_opt = POLICYMAN_3GPP2SCAN_OPT_DISABLED;

  policyman_get_token(&pStr, token, sizeof(token));

  for (idx = 0 ; idx < ARR_SIZE(map) ; ++idx)
  {
    if (strcasecmp(token, map[idx].pStr) == 0)
    {
      *pTgpp2_scan_opt = map[idx].scanOpt;
      status = MRE_STATUS_SUCCESS;
      break;
    }
  }

  return status;
}

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_3gpp_scan_opt_t  tgpp_scan_opt;
  policyman_3gpp2_scan_opt_t tgpp2_scan_opt;
} policyman_scan_opt_condition_t;

/*=============================================================================
  Accessor function for Scan Optimizations
=============================================================================*/

/*-------- policyman_get_3gpp_scan_opt --------*/
policyman_status_t policyman_get_3gpp_scan_opt(
  policyman_item_t const     *pItem,
  policyman_3gpp_scan_opt_t  *pScanOpt
)
{
  policyman_scan_opt_item_t  *pScanOptItem = (policyman_scan_opt_item_t *) pItem;

  if (pScanOptItem == NULL || pScanOpt == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_3gpp_scan_opt returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_SCAN_OPTIMIZATION)
  {
    POLICYMAN_MSG_ERROR_0("get_3gpp_scan_opt returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pScanOpt = pScanOptItem->tgpp_scan_opt;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "get_3gpp_scan_opt returned %d ",
                        pScanOptItem->asubs_id,
                        pScanOptItem->tgpp_scan_opt);

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_get_3gpp2_scan_opt --------*/
policyman_status_t policyman_get_3gpp2_scan_opt(
  policyman_item_t const      *pItem,
  policyman_3gpp2_scan_opt_t  *pScanOpt
)
{
  policyman_scan_opt_item_t  *pScanOptItem = (policyman_scan_opt_item_t *) pItem;

  if (pScanOptItem == NULL || pScanOpt == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_3gpp2_scan_opt returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_SCAN_OPTIMIZATION)
  {
    POLICYMAN_MSG_ERROR_0("get_3gpp2_scan_opt returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pScanOpt = pScanOptItem->tgpp2_scan_opt;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "get_3gpp2_scan_opt returned %d ",
                        pScanOptItem->asubs_id,
                        *pScanOpt );

  return POLICYMAN_STATUS_SUCCESS;
}



/*-------- execute_scan_opt --------*/
static boolean execute_scan_opt(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  scan_opt_action_t               *pScanOptAction = (scan_opt_action_t *)pAction;
  policyman_scan_opt_item_t       *pScanOptItem;

  pScanOptItem = policyman_scan_opt_item_new( subsId,
                                              pScanOptAction->tgpp_scan_opt,
                                              pScanOptAction->tgpp2_scan_opt );

  policyman_set_add(((policy_execute_ctx_t *) pCtx)->pItemSet, &pScanOptItem);

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "action <scan_optimization> set 3gpp to %d, 3gpp2 to %d ",
                        pScanOptItem->asubs_id,
                        LINE_NUM(pScanOptAction),
                        pScanOptItem->tgpp_scan_opt,
                        pScanOptItem->tgpp2_scan_opt );

  ref_cnt_obj_release(pScanOptItem);
  return TRUE;
}

/*-------- policyman_scan_opt_execute --------*/
STATIC boolean policyman_scan_opt_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_scan_opt);
}

/*-------- policyman_read_3gpp_scan_opt --------*/
mre_status_t policyman_read_3gpp_scan_opt(
  mre_xml_element_t const    *pElem,
  policyman_3gpp_scan_opt_t  *pScanOpt
)
{
  mre_status_t   status = MRE_STATUS_ERR_NOT_PRESENT;
  char const    *pStr   = NULL;

  pStr = mre_xml_get_attribute(pElem, "tgpp");

  if (pStr != NULL)
  {
    status = policyman_str_to_3gpp_opt(pStr, pScanOpt);
  }

  POLICYMAN_MSG_HIGH_1("policyman_read_3gpp_scan_opt status %d", status);
  return status;
}

/*-------- policyman_read_3gpp2_scan_opt --------*/
mre_status_t policyman_read_3gpp2_scan_opt(
  mre_xml_element_t const     *pElem,
  policyman_3gpp2_scan_opt_t  *pScanOpt
)
{
  mre_status_t   status = MRE_STATUS_ERR_NOT_PRESENT;
  char const    *pStr   = NULL;

  pStr = mre_xml_get_attribute(pElem, "tgpp2");

  if (pStr != NULL)
  {
    status = policyman_str_to_3gpp2_opt(pStr,  pScanOpt);
  }

  return status;
}


/*-------- policyman_scan_opt_action_new --------*/
mre_status_t policyman_scan_opt_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t                 status         = MRE_STATUS_ERR_INVALID_ACTION;
  scan_opt_action_t           *pAction        = NULL;
  policyman_3gpp_scan_opt_t    tgpp_scan_opt  = POLICYMAN_3GPPSCAN_OPT_DISABLED;
  policyman_3gpp2_scan_opt_t   tgpp2_scan_opt = POLICYMAN_3GPP2SCAN_OPT_DISABLED;
  mre_status_t                 ppStatus;
  mre_status_t                 pp2Status;

  ppStatus  = policyman_read_3gpp_scan_opt(pElem, &tgpp_scan_opt);
  pp2Status = policyman_read_3gpp2_scan_opt(pElem, &tgpp2_scan_opt);

  if (  ppStatus == MRE_STATUS_SUCCESS
     || pp2Status == MRE_STATUS_SUCCESS
     )
  {
    status  = MRE_STATUS_SUCCESS;
    pAction = POLICYMAN_ACTION_NEW( scan_opt_action_t,
                                    pPolicy,
                                    policyman_scan_opt_execute,
                                    policyman_action_dtor,
                                    SUBS_THIS,
                                    pElem );

    pAction->tgpp_scan_opt  = tgpp_scan_opt;
    pAction->tgpp2_scan_opt = tgpp2_scan_opt;

   *ppAction = (mre_action_t *)pAction;
  }

  return status;
}

/*-------- policyman_scan_opt_fill_subs_diag_log --------*/
void policyman_scan_opt_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  policyman_scan_opt_item_t  *pScanOpt = (policyman_scan_opt_item_t *)pItem;

  log_ptr->tgpp_scan_opt  = (int8)pScanOpt->tgpp_scan_opt;
  log_ptr->tgpp2_scan_opt = (int8)pScanOpt->tgpp2_scan_opt;
}
