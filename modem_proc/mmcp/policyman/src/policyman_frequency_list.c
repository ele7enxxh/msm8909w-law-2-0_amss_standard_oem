/**
  @file policyman_frequency_list.c

  @brief File to handle frequency list configuration item.
*/

/*
    Copyright (c) 2015,2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_frequency_list.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre.h"
#include "mre_efs.h"
#include "policyman_i.h"
#include "policyman_frequency_list.h"
#include "policyman_policies.h"
#include "policyman_set.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_xml.h"
#include "policyman_efs.h"
#include "sys.h"


#include "event.h"
#include <stringl/stringl.h>

/*=============================================================================

=============================================================================*/

typedef struct
{
  POLICYMAN_ACTION_BASE;

  policyman_freq_list_type_t  freq_list_type;
} freq_list_action_t;


/*-------- policyman_freq_list_item_new --------*/
STATIC policyman_freq_list_item_t *policyman_freq_list_item_new(
  policyman_freq_list_type_t   freq_list_type,
  sys_modem_as_id_e_type       subs_id
)
{
  policyman_freq_list_item_t  *pItem;

  pItem = (policyman_freq_list_item_t *)policyman_cfgitem_new_item( POLICYMAN_ITEM_FREQ_LIST,
                                                                    sizeof(policyman_freq_list_item_t),
                                                                    policyman_simple_dtor );

  pItem->freq_list_type= freq_list_type;
  pItem->asubs_id = subs_id;

  return pItem;
}


/*-------- policyman_freq_list_get_default --------*/
policyman_item_t *policyman_freq_list_get_default(
  policy_execute_ctx_t *pCtx
)
{
  policyman_freq_list_item_t  *pItem;

  pItem = policyman_freq_list_item_new(POLICYMAN_FREQ_LIST_CPFL, pCtx->asubs_id);

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "default frequency list type is %d",
                        pCtx->asubs_id,
                        pItem->freq_list_type );

  return (policyman_item_t *)pItem;
}

/*-------- policyman_freq_list_compare --------*/
boolean policyman_freq_list_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
)
{
  policyman_freq_list_item_t  *pFreqList1 = (policyman_freq_list_item_t *)pData1;
  policyman_freq_list_item_t  *pFreqList2 = (policyman_freq_list_item_t *)pData2;

  return ( pFreqList1->freq_list_type  == pFreqList2->freq_list_type);
}

/*-------- policyman_freq_list_display --------*/
void policyman_freq_list_display(
  policyman_item_t  *pItem
)
{
  policyman_freq_list_item_t  *pFreqList = (policyman_freq_list_item_t *)pItem;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "Frequency list type %d",
                        pFreqList->asubs_id,
                        pFreqList->freq_list_type );
}

/*-------- policyman_str_to_freq_list_type --------*/
STATIC mre_status_t policyman_str_to_freq_list_type(
  const char                  *pStr,
  policyman_freq_list_type_t  *pFreqListType
)
{
  typedef struct
  {
    const char                 *pStr;
    policyman_freq_list_type_t  freqListType;
  } mask_map;

  static mask_map map[] =
  {
    {"default",  POLICYMAN_FREQ_LIST_DEFAULT},
    {"cpfl",     POLICYMAN_FREQ_LIST_CPFL}
  };

  size_t       idx;
  char         token[18];
  mre_status_t status = MRE_STATUS_ERR_MALFORMED_XML;

  *pFreqListType = POLICYMAN_FREQ_LIST_DEFAULT;

  policyman_get_token(&pStr, token, sizeof(token));

  for (idx = 0 ; idx < ARR_SIZE(map) ; ++idx)
  {
    if (strcasecmp(token, map[idx].pStr) == 0)
    {
      *pFreqListType = map[idx].freqListType;
      status = MRE_STATUS_SUCCESS;
      break;
    }
  }
  return status;
}



typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_freq_list_type_t  freq_list_type;
} policyman_freq_list_condition_t;

/*=============================================================================
  Accessor function for Frequency List
=============================================================================*/

/*-------- policyman_get_freq_list_type --------*/
policyman_status_t policyman_get_freq_list_type(
  policyman_item_t const      *pItem,
  policyman_freq_list_type_t  *pFreqListType
)
{
  policyman_freq_list_item_t  *pFreqListItem = (policyman_freq_list_item_t *) pItem;

  if (pFreqListItem == NULL || pFreqListType == NULL)
  {
    POLICYMAN_MSG_ERROR_0("policyman_get_freq_list_type returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_FREQ_LIST)
  {
    POLICYMAN_MSG_ERROR_0("policyman_get_freq_list_type returned err POLICYMAN_STATUS_ERR_INVALID_ITEM_ID");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pFreqListType = pFreqListItem->freq_list_type;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "policyman_get_freq_list_type(); list_type %d",
                        pFreqListItem->asubs_id,
                        pFreqListItem->freq_list_type );

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- execute_freq_list --------*/
STATIC boolean execute_freq_list(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  freq_list_action_t               *pFreqListAction = (freq_list_action_t *)pAction;
  policyman_freq_list_item_t       *pFreqListItem;

  pFreqListItem = policyman_freq_list_item_new(pFreqListAction->freq_list_type, ACTION_SUBS(pFreqListAction));

  policyman_set_add(((policy_execute_ctx_t *) pCtx)->pItemSet, &pFreqListItem);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "action <freq_list> set freq_list to %d",
                        ACTION_SUBS(pFreqListAction),
                        LINE_NUM(pFreqListAction),
                        pFreqListItem->freq_list_type );

  ref_cnt_obj_release(pFreqListItem);
  return TRUE;
}

/*-------- policyman_freq_list_execute --------*/
STATIC boolean policyman_freq_list_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_freq_list);
}

/*-------- policyman_read_freq_list_type --------*/
STATIC mre_status_t policyman_read_freq_list_type(
  mre_xml_element_t const     *pElem,
  policyman_freq_list_type_t  *pFreqListType
)
{
  mre_status_t  status = MRE_STATUS_ERR_NOT_PRESENT;
  char const    *pStr = NULL;

  pStr = mre_xml_get_attribute(pElem, "list");

  if (pStr != NULL)
  {
    status = policyman_str_to_freq_list_type(pStr, pFreqListType);
  }

  POLICYMAN_MSG_HIGH_1("policyman_read_freq_list_type() status %d", status);
  return status;
}

/*-------- policyman_freq_list_create_new_action --------*/
STATIC mre_status_t policyman_freq_list_create_new_action(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction,
  pfn_execute_t             actionfn
)
{
  mre_status_t                  status         = MRE_STATUS_ERR_INVALID_ACTION;
  freq_list_action_t           *pAction        = NULL;
  policyman_freq_list_type_t    freq_list_type = POLICYMAN_FREQ_LIST_DEFAULT;

  status  = policyman_read_freq_list_type(pElem, &freq_list_type);

  if (MRE_SUCCEEDED(status))
  {
    pAction = POLICYMAN_ACTION_NEW( freq_list_action_t,
                                    pPolicy,
                                    actionfn,
                                    policyman_action_dtor,
                                    SUBS_THIS,
                                    pElem );

    pAction->freq_list_type = freq_list_type;

    *ppAction = (mre_action_t *)pAction;
  }

  return status;
}

/*-------- execute_freq_list_if --------*/
STATIC boolean execute_freq_list_if(
  policyman_action_t const       *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  freq_list_action_t *pFreqListAction  = (freq_list_action_t *) pAction;
  boolean             succeeded        = TRUE;
  boolean             item_is_present;
  policy_execute_ctx_t *pGetCtx          = (policy_execute_ctx_t *) pCtx;

  item_is_present = policyman_cfgitem_is_present_in_itemset( (policyman_set_t *)pGetCtx->pItemSet,
                                                              POLICYMAN_ITEM_FREQ_LIST,
                                                              subsId,
                                                              NULL);

  if (item_is_present)
  {
    POLICYMAN_UT_MSG_1(SRC_LINE "<freq_list_if> skipping update - item present", LINE_NUM(pFreqListAction));
  }
  else
  {
    succeeded = policyman_freq_list_execute((mre_action_t const  *)pAction, pCtx);
  }

  return succeeded;
}

/*-------- policyman_freq_list_if_execute --------*/
STATIC boolean policyman_freq_list_if_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;
  return policyman_execute_action(pAct, pCtx, execute_freq_list_if);
}


/*-------- policyman_freq_list_action_new --------*/
mre_status_t policyman_freq_list_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_freq_list_create_new_action( pElem,
                                                pPolicy,
                                                ppAction,
                                                policyman_freq_list_execute );
}


/*-------- policyman_freq_list_if_action_new --------*/
mre_status_t policyman_freq_list_if_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
 return policyman_freq_list_create_new_action( pElem,
                                               pPolicy,
                                               ppAction,
                                               policyman_freq_list_if_execute );

}


/*-------- policyman_retrieve_freq_list_type --------*/
STATIC boolean policyman_retrieve_freq_list_type(
  sys_modem_as_id_e_type      subsId,
  policyman_freq_list_type_t *pFreqListType
)
{
  mre_efs_status_t            status;
  policyman_freq_list_type_t *pEfsItem = NULL;
  size_t                      fileSize;
  boolean                     retVal = FALSE;

  /*  Try and read from the NV item
   */
  status = mre_efs_get_file( POLICYMAN_FREQ_LIST_NV_PATH,
                             (void **)&pEfsItem,
                             &fileSize,
                             subsId );

  /*  If we were able to read the NV and if the value is in the proper range,
   *  set the mode to that value.  Otherwise just return NORMAL mode.
   */
  if (   (status == MRE_EFS_STATUS_SUCCESS)
      && (fileSize == sizeof(policyman_freq_list_type_t))
      && (pEfsItem != NULL)
      && (*pEfsItem >= POLICYMAN_FREQ_LIST_DEFAULT)
      && (*pEfsItem <= POLICYMAN_FREQ_LIST_CPFL) )
  {
    *pFreqListType = *pEfsItem;
    retVal = TRUE;
  }

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "retrieve_freq_list_type(); status %d, FreqListType %d",
                        subsId,
                        status,
                        *pFreqListType );

  POLICYMAN_MEM_FREE_IF(pEfsItem);

  return retVal;
}

/*-------- policyman_freq_list_get_default_from_efs --------*/
policyman_status_t policyman_freq_list_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
)
{
  policyman_freq_list_item_t    *pItem         = NULL;
  policyman_status_t             status        = POLICYMAN_STATUS_ERR;
  policyman_freq_list_type_t     freqListType;

  if (policyman_retrieve_freq_list_type(pCfgCtx->asubs_id, &freqListType))
  {
    pItem          = policyman_freq_list_item_new(freqListType, pCfgCtx->asubs_id);
    pCfgCtx->pItem = (policyman_item_t *)pItem;

    status = POLICYMAN_STATUS_SUCCESS;
  }

  return status; 
}

/*-------- policyman_freq_list_update_to_efs  --------*/
void policyman_freq_list_update_to_efs(
  policyman_item_t const *pItem
)
{
  boolean                      status;
  policyman_freq_list_item_t  *pType;

  pType = (policyman_freq_list_item_t *) pItem;

  status = policyman_cfgitem_persist_item( POLICYMAN_FREQ_LIST_NV_PATH,
                                           &pType->freq_list_type,
                                           sizeof(pType->freq_list_type),
                                           pType->asubs_id );

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "write frequency list to EFS; type %d, status %d",
                        pType->asubs_id,
                        pType->freq_list_type,
                        status );
}

/*-------- policyman_freq_list_fill_subs_diag_log --------*/
void policyman_freq_list_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  policyman_freq_list_item_t  *pFreqList = (policyman_freq_list_item_t *)pItem;

  log_ptr->freq_list  = (uint8)pFreqList->freq_list_type;
}
