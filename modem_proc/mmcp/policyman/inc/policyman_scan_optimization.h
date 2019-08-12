#ifndef _POLICYMAN_SCAN_OPT_H_
#define _POLICYMAN_SCAN_OPT_H_

/**
  @file policyman_scan_optimization.h

  @brief  Internal definitions and declarations for the scan optimization
          policy.  This is used by both the policy and its unit tests.
*/

/*
    Copyright (c) 2015, 2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_scan_optimization.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_cfgitem.h"
#include "policyman_rules.h"
#include "policyman_diag.h"



/*-------- policyman_scan_opt_action_new --------*/
/**
@brief  API to create a new policyman action for the scan_optimization
        action in the policy_info.

@param[in]  pElem     Pointer to the XML document element for the
                      <scan_optimization> node.
@param[out] ppAction  Pointer to storage for a pointer to the policyman_action_t
                      object created from the XML.

@return
  Status
*/
mre_status_t  policyman_scan_opt_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

mre_status_t policyman_enable_scan_opt_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  policyman_action_t            **ppAction
);


/*-------- scan_optimization_t --------*/
/**
@brief  Configuration item structure for scan_optimization.
*/
typedef struct
{
  POLICYMAN_ITEM;
  
  policyman_3gpp_scan_opt_t    tgpp_scan_opt;
  policyman_3gpp2_scan_opt_t   tgpp2_scan_opt;
} policyman_scan_opt_item_t;


/*-------- policyman_scan_opt_init --------*/
void policyman_scan_opt_init(
 void
);


/*-------- policyman_scan_opt_get_default --------*/
/**
@brief  Get the default configuration for POLICYMAN_ITEM_SCAN_OPTIMIZATION
        if there is no XML configuration file.
*/
policyman_item_t *policyman_scan_opt_get_default(
  policy_execute_ctx_t *pCtx
);



/*-------- policyman_scan_opt_compare --------*/
/**
@brief  
*/
boolean policyman_scan_opt_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
);


/*-------- policyman_scan_opt_display --------*/
/**
@brief  
*/
void policyman_scan_opt_display(
  policyman_item_t  *pItem
);

/*-------- policyman_scan_opt_fill_subs_diag_log --------*/
void policyman_scan_opt_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
);

#endif /* _POLICYMAN_SCAN_OPT_H_ */
