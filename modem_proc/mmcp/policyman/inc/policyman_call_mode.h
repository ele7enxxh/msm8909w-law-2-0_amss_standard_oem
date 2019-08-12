#ifndef _POLICYMAN_CALL_MODE_H_
#define _POLICYMAN_CALL_MODE_H_

/**
  @file policyman_call_mode.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_call_mode.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "sys.h"
#include "policyman_i.h"
#include "policyman_cfgitem.h"
#include "policyman_diag.h"

/*=============================================================================
  Call mode APIs
=============================================================================*/


typedef struct
{
  POLICYMAN_ITEM;

  sys_call_mode_t  call_mode;
} policyman_call_mode_item_t;

/*-------- policyman_call_mode_get_default --------*/
/**
@brief

@param

@return

*/
policyman_item_t *policyman_call_mode_get_default(
  policy_execute_ctx_t *pCtx
);


/*-------- policyman_call_mode_item_new --------*/
/**
@brief

@param

@return

*/
policyman_call_mode_item_t *policyman_call_mode_item_new(
  sys_call_mode_t          ecall_mode,
  sys_modem_as_id_e_type   asubs_id
);




/*-------- policyman_call_mode_compare --------*/
/**
@brief

@param

@return

*/
boolean policyman_call_mode_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
);

/*-------- policyman_call_mode_display --------*/
/**
@brief

@param

@return

*/
void policyman_call_mode_display(
  policyman_item_t  *pItem
);


/*-------- policyman_condition_call_mode_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_condition_call_mode_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
);


/*-------- policyman_call_mode_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_call_mode_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

/*-------- policyman_call_mode_if_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_call_mode_if_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

/*-------- policyman_call_mode_update_to_efs --------*/
/**
@brief

@param

@return

*/
void policyman_call_mode_update_to_efs(
  policyman_item_t const *pItem
);

/*-------- policyman_call_mode_fill_subs_diag_log --------*/
void policyman_call_mode_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
);

#endif
