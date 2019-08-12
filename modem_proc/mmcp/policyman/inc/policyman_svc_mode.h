#ifndef _POLICYMAN_SVC_MODE_H_
#define _POLICYMAN_SVC_MODE_H_

/**
  @file policyman_svc_mode.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_svc_mode.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"
#include "policyman_cfgitem.h"
#include "policyman_diag.h"


/*=============================================================================
  UE mode APIs
=============================================================================*/


typedef struct
{
  POLICYMAN_ITEM;
  
  policyman_svc_mode_t  mode;
} policyman_svc_mode_item_t;



/*-------- policyman_svc_mode_item_new --------*/
/**
@brief  

@param

@return
  
*/
policyman_svc_mode_item_t * policyman_svc_mode_item_new(
  policyman_svc_mode_t    svcMode,
  sys_modem_as_id_e_type  subsId
  );

/*-------- policyman_svc_mode_get_default --------*/
/**
@brief  

@param

@return
  
*/
policyman_item_t * policyman_svc_mode_get_default(
  policy_execute_ctx_t *pCtx
  );

/*-------- policyman_svc_mode_compare --------*/
/**
@brief  

@param

@return
  
*/
boolean policyman_svc_mode_compare(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
  );

/*-------- policyman_svc_mode_display --------*/
void policyman_svc_mode_display (
  policyman_item_t  *pItem
);

/*-------- policyman_svc_mode_action_new --------*/
mre_status_t policyman_svc_mode_action_new (
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
);

/*-------- policyman_condition_svc_mode_new --------*/
mre_status_t policyman_condition_svc_mode_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  );

/*-------- policyman_svc_mode_update_to_efs --------*/
void policyman_svc_mode_update_to_efs(
  policyman_item_t const *pItem
);

/*-------- policyman_svc_mode_get_default_from_efs --------*/
policyman_status_t policyman_svc_mode_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
);

/*-------- policyman_svc_mode_fill_subs_diag_log --------*/
void policyman_svc_mode_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
);

/*-------- policyman_svc_mode_get_limited_item --------*/
void policyman_svc_mode_get_limited_item(
  mre_set_t              *pItemSet,
  sys_modem_as_id_e_type  subsId
);

#endif /* _POLICYMAN_SVC_MODE_H_ */
