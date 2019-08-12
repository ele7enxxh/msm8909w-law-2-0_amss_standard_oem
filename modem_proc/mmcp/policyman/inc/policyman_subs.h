#ifndef _POLICYMAN_SUBS_H_
#define _POLICYMAN_SUBS_H_

/**
  @file policyman_subs.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_subs.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"

/*=============================================================================
  Num Subs APIs
=============================================================================*/

typedef struct policyman_subs_info_s  policyman_subs_info_t;

/*-------- policyman_subs_init --------*/
boolean policyman_subs_init(
  policyman_subs_info_t **ppInfo
);

/*-------- policyman_subs_deinit --------*/
void policyman_subs_deinit(
  policyman_subs_info_t *pInfo
);

/*-------- policyman_subs_info_deep_copy --------*/
policyman_subs_info_t * policyman_subs_info_deep_copy(
  policyman_subs_info_t  *pSubsState
);

/*-------- policyman_subs_get_num_subs --------*/
size_t policyman_subs_get_num_subs(
  policyman_state_t *pState
);

/*-------- policyman_count_num_subs --------*/
size_t policyman_count_num_subs(
  uint8 active_subs_mask
);

/*-------- policyman_subs_get_dds_subs --------*/
sys_modem_as_id_e_type policyman_subs_get_dds_subs(
  policyman_state_t *pState
);

/*-------- policyman_condition_is_subs_multimode_new --------*/
mre_status_t policyman_condition_is_subs_multimode_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);


/*=============================================================================
  Subs-related APIs
=============================================================================*/

/*-------- policyman_str_to_subs --------*/
policyman_subs_t policyman_str_to_subs(
  char const  *pStr
);

/*-------- policyman_subs_update_multimode_subs --------*/
void policyman_subs_update_multimode_subs(
  policyman_cmd_t         *pCmd
);

/*-------- policyman_is_cm_subs_active --------*/
boolean policyman_is_cm_subs_active(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
);

/*=============================================================================
  <num_subs> rule conditions
=============================================================================*/

/*-------- policyman_condition_num_subs_new --------*/
mre_status_t policyman_condition_num_subs_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_is_subs_dds_new --------*/
mre_status_t policyman_condition_is_subs_dds_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

#endif /* _POLICYMAN_SUBS_H_ */
