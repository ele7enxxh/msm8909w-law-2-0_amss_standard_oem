#ifndef _POLICYMAN_FREQ_LIST_H_
#define _POLICYMAN_FREQ_LIST_H_

/**
  @file policyman_frequency_list.h

  @brief  Internal definitions and declarations for the frequency list
             configuration item.
*/

/*
    Copyright (c) 2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_frequency_list.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_cfgitem.h"
#include "policyman_rules.h"


/*-------- policyman_freq_list_action_new --------*/
/**
@brief  API to create a new policyman action for the frequency_list
        action in the policy_info.

@param[in]  pElem     Pointer to the XML document element for the
                      <frequency_list> node.
@param[out] ppAction  Pointer to storage for a pointer to the policyman_action_t
                      object created from the XML.

@return
  Status
*/
mre_status_t  policyman_freq_list_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

/*-------- policyman_freq_list_if_action_new --------*/
/**
@brief  API to create a new policyman action for the frequency_list
        action in the policy_info.

@param[in]  pElem     Pointer to the XML document element for the
                      <frequency_list> node.
@param[out] ppAction  Pointer to storage for a pointer to the policyman_action_t
                      object created from the XML.

@return
  Status
*/
mre_status_t policyman_freq_list_if_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t                 **ppAction
);


/*-------- frequency_list_t --------*/
/**
@brief  Configuration item structure for frequency_list.
*/
typedef struct
{
  POLICYMAN_ITEM;

  policyman_freq_list_type_t    freq_list_type;
} policyman_freq_list_item_t;


/*-------- policyman_freq_list_init --------*/
void policyman_freq_list_init(
 void
);


/*-------- policyman_freq_list_get_default --------*/
/**
@brief  Get the default configuration for POLICYMAN_ITEM_FREQ_LIST
        if there is no XML configuration file.
*/
policyman_item_t *policyman_freq_list_get_default(
  policy_execute_ctx_t *pCtx
);


/*-------- policyman_freq_list_get_default_from_efs --------*/
/**
@brief  Get the persisted POLICYMAN_ITEM_FREQ_LIST from efs
*/
policyman_status_t policyman_freq_list_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
);

/*-------- policyman_freq_list_update_to_efs --------*/
/**
@brief  persist POLICYMAN_ITEM_FREQ_LIST in efs
*/
void policyman_freq_list_update_to_efs(
  policyman_item_t const *pItem
);

/*-------- policyman_freq_list_compare --------*/
/**
@brief
*/
boolean policyman_freq_list_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
);


/*-------- policyman_freq_list_display --------*/
/**
@brief
*/
void policyman_freq_list_display(
  policyman_item_t  *pItem
);

/*-------- policyman_freq_list_fill_subs_diag_log --------*/
void policyman_freq_list_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
);

#endif /* _POLICYMAN_FREQ_LIST_H_ */
