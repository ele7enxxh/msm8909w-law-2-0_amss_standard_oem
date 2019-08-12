#ifndef _POLICYMAN_RAT_ORDER_H_
#define _POLICYMAN_RAT_ORDER_H_

/**
  @file policyman_rat_order.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_rat_order.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"
#include "policyman_diag.h"

/*=============================================================================
  Technology Order Table APIs
=============================================================================*/

typedef struct
{
  POLICYMAN_ITEM;

  cm_acq_pri_order_pref_s_type rat_order;
} policyman_rat_order_item_t;


/*-------- policyman_rat_order_item_new --------*/
/**
@brief

@param

@return

*/
policyman_rat_order_item_t * policyman_rat_order_item_new(
  void
);



/*-------- policyman_rat_order_get_default --------*/
/**
@brief

@param

@return

*/
policyman_item_t * policyman_rat_order_get_default(
  policy_execute_ctx_t *pCtx
);


/*-------- policyman_rat_order_compare --------*/
/**
@brief

@param

@return

*/
boolean policyman_rat_order_compare(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
);

/*-------- policyman_rat_order_update_to_efs --------*/
void policyman_rat_order_update_to_efs(
  policyman_item_t const *pItem
);

/*-------- policyman_retrieve_rat_order --------*/
boolean policyman_retrieve_rat_order(
  cm_acq_pri_order_pref_s_type *pRatOrder,
  sys_modem_as_id_e_type        subsId
);

/*-------- policyman_rat_order_display --------*/
void policyman_rat_order_display(
  policyman_item_t  *pItem
);

/*-------- policyman_rat_order_action_new --------*/
mre_status_t policyman_rat_order_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);

/*-------- policyman_rat_order_fill_subs_diag_log --------*/
void policyman_rat_order_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
);

#endif
