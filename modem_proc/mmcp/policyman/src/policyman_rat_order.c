/**
  @file policyman_rat_order.c

  @brief
*/

/*
    Copyright (c) 2014-2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_rat_order.c#1 $
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
#include "policyman_rat_order.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include "cm.h"
#include "sys.h"
#include "bit.h"

#include <stringl/stringl.h>

#define RAT_ORDER_TAG  "rat_order"
#define PM_RAT_ORDER_TABLE_MAX_SIZE sizeof(sd_rat_acq_order_s_type)
#define PM_DEFAULT_NUM_RAT  6
#define PM_BITS_IN_BYTE 8

static cm_acq_pri_order_pref_s_type default_rat_acq_order =
  // Default TOT has all 6 RATs
  { PM_DEFAULT_NUM_RAT, { SYS_SYS_MODE_CDMA,
                          SYS_SYS_MODE_GSM,
                          SYS_SYS_MODE_WCDMA,
                          SYS_SYS_MODE_HDR,
                          SYS_SYS_MODE_LTE,
                          SYS_SYS_MODE_TDS,
                          SYS_SYS_MODE_NO_SRV,
                          SYS_SYS_MODE_NO_SRV,
                          SYS_SYS_MODE_NO_SRV,
                          SYS_SYS_MODE_NO_SRV
                        }
  };

#define PM_B_UNPACKW(buf_ptr, offset, len) b_unpackw(&(buf_ptr[offset/8]), \
                                                     (word) (offset%8), len)

#define PM_B_PACKW(src, dst_ptr, offset, len) b_packw(src, \
                                                 &(dst_ptr[offset/8]), \
                                                 (word) (offset%8), \
                                                 len)

/*=============================================================================
  Technology Order Table APIs
=============================================================================*/

typedef struct
{
  POLICYMAN_ACTION_BASE;

  cm_acq_pri_order_pref_s_type rat_order;
} rat_order_action_t;



/*-------- policyman_rat_order_item_new --------*/
policyman_rat_order_item_t * policyman_rat_order_item_new(
  void
)
{
  return (policyman_rat_order_item_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_RAT_ACQ_ORDER,
                                                                    sizeof(policyman_rat_order_item_t),
                                                                    policyman_simple_dtor );
}


/*-------- policyman_rat_order_get_default --------*/
policyman_item_t * policyman_rat_order_get_default(
  policy_execute_ctx_t  *pCtx
)
{
  policyman_rat_order_item_t *pItem;

  pItem = policyman_rat_order_item_new();

  pItem->asubs_id = pCtx->asubs_id;

  if (!policyman_retrieve_rat_order(&pItem->rat_order, pCtx->asubs_id))
  {
    pItem->rat_order = default_rat_acq_order;

    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "Could not read rat_acq_order from EFS, using default", pCtx->asubs_id);
  }

  return (policyman_item_t *) pItem;
}


/*-------- policyman_rat_order_compare --------*/
boolean policyman_rat_order_compare(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
)
{
  policyman_rat_order_item_t *pOrder1 = (policyman_rat_order_item_t *) pItem1;
  policyman_rat_order_item_t *pOrder2 = (policyman_rat_order_item_t *) pItem2;
  size_t                      nRatBytes;

  if ( pOrder1->rat_order.num_rat != pOrder2->rat_order.num_rat)
  {
    return FALSE;
  }

  nRatBytes = pOrder1->rat_order.num_rat * sizeof(sys_sys_mode_e_type);
  return memcmp(pOrder1->rat_order.acq_order, pOrder2->rat_order.acq_order, nRatBytes) == 0;
}


/*-------- rat_order_encode_efs --------*/
boolean rat_order_encode_efs(
  const cm_acq_pri_order_pref_s_type *pRatOrder,
  byte                               *buf_ptr
)
{
  uint16 offset = 0;
  uint16 size   = 0;
  uint16 i      = 0;

  if (NULL == pRatOrder || NULL == buf_ptr)
  {
    POLICYMAN_MSG_ERROR_0("Unable to encode rat_order, NULL inputs");
    return FALSE;
  }

  /* size for the b_unpackw functions is the size
        of the field, in bits.
   */
  // CM Structure does not have version field, pack a 0!  Version is a uint16
  {
    uint16 version = 0;

    size = sizeof(version) * PM_BITS_IN_BYTE;
    PM_B_PACKW(version, buf_ptr, offset, size);
    offset += size;
  }

  if (   pRatOrder->num_rat > SYS_MAX_ACQ_PREF_RAT_LIST_NUM
      || pRatOrder->num_rat == 0)
  {
    POLICYMAN_MSG_ERROR_1("TOT encoding: invalid num_rat %d", pRatOrder->num_rat);
    return FALSE;
  }

  size = sizeof(pRatOrder->num_rat) * PM_BITS_IN_BYTE;
  PM_B_PACKW(pRatOrder->num_rat, buf_ptr, offset,size);
  offset += size;

  POLICYMAN_MSG_HIGH_8( "TOT encoding: num_rat %d, order %d %d %d %d %d %d %d",
                        pRatOrder->num_rat,
                        pRatOrder->acq_order[0],
                        pRatOrder->acq_order[1],
                        pRatOrder->acq_order[2],
                        pRatOrder->acq_order[3],
                        pRatOrder->acq_order[4],
                        pRatOrder->acq_order[5],
                        pRatOrder->acq_order[6] );

  size = PM_BITS_IN_BYTE;
  for (i = 0; i < pRatOrder->num_rat; i++)
  {
    if (!policyman_util_check_valid_sys_mode(pRatOrder->acq_order[i]))
    {
      return FALSE;
    }

    PM_B_PACKW((word)(pRatOrder->acq_order[i]), buf_ptr, offset, size);

    offset += size;
  }

  return TRUE;
}


/*-------- policyman_rat_order_update_to_efs --------*/
void policyman_rat_order_update_to_efs(
  policyman_item_t const *pItem
)
{
  policyman_rat_order_item_t *pRatOrder            = (policyman_rat_order_item_t *) pItem;
  byte                        efs_tot[PM_RAT_ORDER_TABLE_MAX_SIZE] = {0};
  boolean                     writeSucceeded;

  if (rat_order_encode_efs(&pRatOrder->rat_order, efs_tot))
  {
    writeSucceeded = policyman_cfgitem_persist_item( POLICYMAN_RAT_ORDER_NV_PATH,
                                                     efs_tot,
                                                     PM_RAT_ORDER_TABLE_MAX_SIZE,
                                                     pRatOrder->asubs_id );

    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "Wrote %d bytes to EFS rat_order, succeeded %d",
                          pRatOrder->asubs_id,
                          PM_RAT_ORDER_TABLE_MAX_SIZE,
                          writeSucceeded );
  }
}


/*-------- rat_order_decode_efs --------*/
boolean rat_order_decode_efs(
  byte                         *buf_ptr,
  cm_acq_pri_order_pref_s_type *pRatOrder
)
{
  uint16 offset = 0;
  uint16 size   = 0;
  uint16 i      = 0;

  /* size for the b_unpackw functions is the size
        of the field, in bits.
   */
  // CM Structure does not have version field, skip unpacking it.  Version is a uint16
  size = sizeof(uint16) * PM_BITS_IN_BYTE;
  offset += size;

  size = sizeof(pRatOrder->num_rat) * PM_BITS_IN_BYTE;
  pRatOrder->num_rat  = PM_B_UNPACKW(buf_ptr, offset, size);
  offset += size;

  if (   pRatOrder->num_rat > SYS_MAX_ACQ_PREF_RAT_LIST_NUM
      || pRatOrder->num_rat == 0
     )
  {
    POLICYMAN_MSG_ERROR_1("TOT decoding: invalid num_rat %d", pRatOrder->num_rat);
    return FALSE;
  }

  size = PM_BITS_IN_BYTE;
  for (i = 0; i < pRatOrder->num_rat; i++)
  {
    pRatOrder->acq_order[i] = (sys_sys_mode_e_type)PM_B_UNPACKW(buf_ptr, offset, size);

    if (!policyman_util_check_valid_sys_mode(pRatOrder->acq_order[i]))
    {
      return FALSE;
    }
    offset += size;
  }

  POLICYMAN_MSG_HIGH_8( "TOT decoding: num_rat %d, order %d %d %d %d %d %d %d",
                        pRatOrder->num_rat,
                        pRatOrder->acq_order[0],
                        pRatOrder->acq_order[1],
                        pRatOrder->acq_order[2],
                        pRatOrder->acq_order[3],
                        pRatOrder->acq_order[4],
                        pRatOrder->acq_order[5],
                        pRatOrder->acq_order[6] );


  return TRUE;
}


/*-------- rat_order_remove_duplicates --------*/
boolean rat_order_remove_duplicates(
  cm_acq_pri_order_pref_s_type   *pRatOrder
)
{
  uint16              num_rat     = 0;
  int                 i;
  int                 j;
  int                 n_rat;
  sys_sys_mode_e_type tmp_tot[SYS_MAX_ACQ_PREF_RAT_LIST_NUM] = {0};
  boolean             dup_removed = FALSE;

  num_rat = MIN(SYS_MAX_ACQ_PREF_RAT_LIST_NUM, pRatOrder->num_rat);

  for (i = 0, n_rat = 0; i < num_rat; i++)
  {
    for (j = 0; j < i; j++)
    {
      if (pRatOrder->acq_order[i] == pRatOrder->acq_order[j])
      {
        break;
      }
    }
    if (j == i)
    {
      tmp_tot[n_rat++] = pRatOrder->acq_order[i];
    }
  }

  if (n_rat < num_rat)
  {
    dup_removed = TRUE;
  }
  pRatOrder->num_rat = n_rat;
  memscpy( pRatOrder->acq_order, sizeof(pRatOrder->acq_order), tmp_tot, sizeof(tmp_tot) );

  return dup_removed;
}


/*-------- policyman_retrieve_rat_order --------*/
boolean policyman_retrieve_rat_order(
  cm_acq_pri_order_pref_s_type *pRatOrder,
  sys_modem_as_id_e_type        subsId
)
{
  mre_efs_status_t   fs_status;
  byte              *pPackedTable = NULL;
  size_t             tableSize;
  boolean            efs_read     = FALSE;

  /*  Try and read from the EFS item for Tech Order Table.
   */
  fs_status = mre_efs_get_file( POLICYMAN_RAT_ORDER_NV_PATH,
                                (void **) &pPackedTable,
                                &tableSize,
                                subsId );

  /*  If we were able to read the EFS and if the file is within the right size,
   *  decode the table.  Otherwise just return NONE.
   */
  if (   fs_status == MRE_EFS_STATUS_SUCCESS
      && tableSize <= PM_RAT_ORDER_TABLE_MAX_SIZE
      && pPackedTable != NULL
     )
  {
    if (rat_order_decode_efs(pPackedTable, pRatOrder))
    {
      rat_order_remove_duplicates(pRatOrder);
      efs_read = TRUE;
    }
  }

  POLICYMAN_MEM_FREE_IF(pPackedTable);
  return efs_read;
}

/*-------- policyman_get_rat_order --------*/
policyman_status_t policyman_get_rat_order(
  policyman_item_t const       *pItem,
  cm_acq_pri_order_pref_s_type *pRatOrder
)
{
  policyman_rat_order_item_t  *pOrder = (policyman_rat_order_item_t *) pItem;

  if (pOrder == NULL || pRatOrder == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_rat_order returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }
  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_RAT_ACQ_ORDER)
  {
    POLICYMAN_MSG_ERROR_0("get_rat_order returned err POLICYMAN_STATUS_ERR_INVALID_ITEM_ID");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pRatOrder = pOrder->rat_order;

  POLICYMAN_MSG_HIGH_3( "get_rat_order (from item 0x%x, subs %d) returned rat_order num_rats %d",
                        pItem,
                        pOrder->asubs_id,
                        pOrder->rat_order.num_rat );

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_rat_order_display --------*/
void policyman_rat_order_display(
  policyman_item_t  *pItem
)
{
  policyman_rat_order_item_t *pTable = (policyman_rat_order_item_t *) pItem;

  POLICYMAN_MSG_HIGH_9( SUBS_PREFIX "Rat Acq Order: num rats %d, order %d %d %d %d %d %d %d",
                        pItem->asubs_id,
                        pTable->rat_order.num_rat,
                        pTable->rat_order.acq_order[0],
                        pTable->rat_order.acq_order[1],
                        pTable->rat_order.acq_order[2],
                        pTable->rat_order.acq_order[3],
                        pTable->rat_order.acq_order[4],
                        pTable->rat_order.acq_order[5],
                        pTable->rat_order.acq_order[6] );
}

/*-------- policyman_str_to_rat_order --------*/
static boolean policyman_str_to_rat_order(
  const char                   *pStr,
  cm_acq_pri_order_pref_s_type *pRat
)
{
  char     token[32];
  rat_map *rat_item;
  boolean  succeeded = TRUE;

  pRat->num_rat = 0;

  while (policyman_get_token(&pStr, token, sizeof(token)))
  {
    if (pRat->num_rat >= SYS_MAX_ACQ_PREF_RAT_LIST_NUM)
    {
      POLICYMAN_UT_MSG_1("Too many RATs in order list (max %d)", SYS_MAX_ACQ_PREF_RAT_LIST_NUM);
      succeeded = FALSE;
      break;
    }

    rat_item = policyman_util_str_to_rat(token);

    if (NULL != rat_item)
    {
      pRat->acq_order[pRat->num_rat++] = rat_item->rat;
    }
    else
    {
      POLICYMAN_UT_MSG_1("invalid RAT specified: %s", token);
      succeeded = FALSE;
      break;
    }
  }

  if (rat_order_remove_duplicates(pRat))
  {
    POLICYMAN_UT_MSG_0("duplicate RATs present in order list");
    succeeded = FALSE;
  }

  return succeeded;
}


/*-------- policyman_rat_order_read_info --------*/
mre_status_t policyman_rat_order_read_info(
  policyman_xml_element_t const *pElem,
  cm_acq_pri_order_pref_s_type  *pRatOrder
)
{
  mre_status_t  status = MRE_STATUS_ERR_INVALID_ACTION;
  char const   *pStr;

  if (policyman_xml_tag_is(pElem, RAT_ORDER_TAG))
  {
    pRatOrder->num_rat = 0;

    pStr = policyman_xml_get_text(pElem);

    if (policyman_str_to_rat_order(pStr, pRatOrder))
    {
      status = MRE_STATUS_SUCCESS;
    }
  }

  return status;
}

/*-------- execute_rat_order --------*/
static boolean execute_rat_order(
  policyman_action_t const      *pAction,
  void                          *pCtx,
  sys_modem_as_id_e_type         subsId
)
{
  rat_order_action_t         *proAction = (rat_order_action_t *) pAction;
  policyman_rat_order_item_t *pSm;
  sys_modem_as_id_e_type      asid;

  pSm  = policyman_rat_order_item_new();
  asid = ACTION_SUBS(pAction);

  pSm->rat_order = proAction->rat_order;
  pSm->asubs_id  = asid;

  policyman_set_replace(((policy_execute_ctx_t *)pCtx)->pItemSet, &pSm);
  ref_cnt_obj_release(pSm);

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "action <rat_order pmsubs=%d> set rat_order with %d rats",
                        asid,
                        LINE_NUM(proAction),
                        subsId,
                        pSm->rat_order.num_rat );

  return TRUE;
}

/*-------- policyman_rat_order_execute --------*/
static boolean policyman_rat_order_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_rat_order);
}

/*-------- policyman_rat_order_action_new --------*/
mre_status_t policyman_rat_order_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  mre_status_t                  status    = MRE_STATUS_ERR_INVALID_ACTION;
  rat_order_action_t           *pAction   = NULL;
  cm_acq_pri_order_pref_s_type  rat_order;

  status = policyman_rat_order_read_info(pElem, &rat_order);

  if (MRE_FAILED(status))
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "<rat_order>: invalid rat order", POLICY_SUBS(pPolicy));
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( rat_order_action_t,
                                  pPolicy,
                                  policyman_rat_order_execute,
                                  policyman_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pAction->rat_order = rat_order;

  *ppAction = (mre_action_t *) pAction;

Done:
  return status;
}

/*-------- policyman_rat_order_fill_subs_diag_log --------*/
void policyman_rat_order_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  policyman_rat_order_item_t     *pRatOrder = (policyman_rat_order_item_t *)pItem;
  size_t                          idx;

  log_ptr->rat_order_table_len    = ARR_SIZE(log_ptr->rat_order_entries);
  log_ptr->num_rat_order_entries  = pRatOrder->rat_order.num_rat;

  for (idx = 0; idx < ARR_SIZE(log_ptr->rat_order_entries); ++idx)
  {
    boolean   valid_source;

    valid_source = (idx < pRatOrder->rat_order.num_rat) && (idx < ARR_SIZE(pRatOrder->rat_order.acq_order));

    log_ptr->rat_order_entries[idx] = valid_source ? pRatOrder->rat_order.acq_order[idx] : 0;
  }
}
