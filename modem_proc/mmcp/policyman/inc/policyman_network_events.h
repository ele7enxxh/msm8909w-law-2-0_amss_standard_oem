#ifndef _POLICYMAN_NETWORK_EVENTS_H_
#define _POLICYMAN_NETWORK_EVENTS_H_

/**
  @file policyman_network_events.h

  @brief  
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_network_events.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "cm.h"
#include "sys.h"

#include "policyman_i.h"

typedef enum
{
  POLICYMAN_NET_SCAN_NOT_STARTED   = 0,
  POLICYMAN_NET_SCAN_PENDING       = 1,
  POLICYMAN_NET_SCAN_COMPLETED     = 2,
  POLICYMAN_NET_SCAN_ABORTED       = 3
} policyman_network_scan_e_type;


typedef struct policyman_network_info_s  policyman_network_info_t;


/*-------- policyman_network_init --------*/
boolean policyman_network_init(
  policyman_network_info_t **ppInfo,
  sys_modem_as_id_e_type     asubs_id
);

/*-------- policyman_network_deinit --------*/
void policyman_network_deinit(
  policyman_network_info_t *pInfo
);

/*-------- policyman_network_scan_get_scan_status --------*/
policyman_network_scan_e_type policyman_network_scan_get_scan_status(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
);

/*-------- policyman_network_scan_set_scan_status --------*/
void policyman_network_scan_set_scan_status(
  policyman_network_info_t      *pNetworkInfo,
  policyman_network_scan_e_type  status
);

/*-------- policyman_network_info_deep_copy --------*/
policyman_network_info_t * policyman_network_info_deep_copy(
  policyman_network_info_t  *pNetworkState
);


/*-------- policyman_network_scan_update_result --------*/
boolean policyman_network_scan_update_result(
  policyman_network_info_t      *pInfo,
  sys_modem_as_id_e_type         subsId,
  sys_plmn_list_status_e_type    listStatus,
  sys_detailed_plmn_list_s_type *pPlmnList
);

/*-------- policyman_network_scan_cmd_cb --------*/
void policyman_network_scan_cmd_cb( 
  void                 *pInfo, 
  cm_ph_cmd_e_type      cmd, 
  cm_ph_cmd_err_e_type  err
);

/*=============================================================================
  Network Event-related rule conditions and actions
=============================================================================*/

/*-------- policyman_condition_scan_results_new --------*/
mre_status_t policyman_condition_scan_results_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_network_scan_active_new --------*/
mre_status_t policyman_condition_network_scan_active_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_network_scan_action_new --------*/
mre_status_t policyman_network_scan_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

#endif /* _POLICYMAN_NETWORK_EVENTS_H_ */
