#ifndef _POLICYMAN_SERVING_SYSTEM_H_
#define _POLICYMAN_SERVING_SYSTEM_H_

/**
  @file policyman_serving_system.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_serving_system.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "sys.h"
#include "cm.h"
#include "policyman_i.h"
#include "policyman_diag.h"

typedef struct policyman_ss_info_t  policyman_ss_info_t;

typedef struct policyman_cmss_history_entry_s   policyman_cmss_history_entry_t;
typedef struct policyman_cmss_history_s         policyman_cmss_history_t;

/*-------- policyman_cmss_get_history --------*/
size_t policyman_cmss_get_history(
  pm_dump_cm_ss_info_history_t *h_buffer,
  size_t                        h_len
);

/*-------- policyman_cmss_fill_diag_ss_info --------*/
void policyman_cmss_fill_diag_ss_info(
  policyman_ss_info_t     *subs_ss_info,
  pm_dump_cm_ss_info_t    *dump_ss_info
);

/*-------- policyman_cmss_init_history --------*/
void policyman_cmss_init_history(
  policyman_state_t  *pState
);

/*-------- policyman_cmss_deinit_history --------*/
void policyman_cmss_deinit_history(
  policyman_state_t  *pState
);

/*-------- policyman_ss_init --------*/
boolean policyman_ss_init(
  sys_modem_as_id_e_type   subs,
  policyman_ss_info_t     **ppInfo
);

/*-------- policyman_ss_deinit --------*/
void policyman_ss_deinit(
  policyman_ss_info_t *pInfo
);

/*-------- policyman_ss_info_deep_copy --------*/
policyman_ss_info_t * policyman_ss_info_deep_copy(
  policyman_ss_info_t  *pSsState
);


/*-------- policyman_ss_set_subs_serving_info_validity --------*/
void policyman_ss_set_subs_serving_info_validity(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId,
  boolean                 servingInfoIsValid
);

/*-------- policyman_ss_reset_acq_fail_cnt_per_subs --------*/
void policyman_ss_reset_acq_fail_cnt_per_subs(
  sys_modem_as_id_e_type     subsId
);

/*-------- policyman_ss_get_mcc --------*/
uint32 policyman_ss_get_mcc(
  policyman_ss_info_t       *pInfo,
  sys_modem_stack_id_e_type  num_stack
);

/*-------- policyman_ss_get_plmn --------*/
void policyman_ss_get_plmn(
  policyman_ss_info_t       *pInfo,
  sys_modem_stack_id_e_type  num_stack,
  sys_plmn_id_s_type        *pPlmn
);

/*-------- policyman_condition_serving_mcc_new --------*/
mre_status_t policyman_condition_serving_mcc_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_ss_set_stack_opr --------*/
void policyman_ss_set_stack_opr(
  policyman_state_t         *pState,
  sys_modem_as_id_e_type     subsId,
  sys_modem_stack_id_e_type  stackId,
  boolean                    isOperational
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_SRV_STATUS()

  DESCRIPTION
    GET srv_status for stack
 ===========================================================================*/
sys_srv_status_e_type policyman_ss_get_srv_status(
  policyman_state_t         *pState,
  sys_modem_as_id_e_type     subsId,
  sys_modem_stack_id_e_type  num_stack
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_SET_SRV_STATUS()

  DESCRIPTION
    SET srv_status for  stack into PolicyMan state

 RETURN VALUE
   TRUE is policyman state is updated correctly, FALSE otherwise
 ===========================================================================*/
boolean policyman_ss_set_srv_status(
  policyman_ss_info_t       *pInfo,
  sys_modem_stack_id_e_type  num_stack,
  sys_srv_status_e_type      srv_status
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_UPDATE_GW_HYBR_OPERATIONAL()

  DESCRIPTION
    update for gw_hybr operational flag into PolicyMan state
 ===========================================================================*/
void policyman_ss_update_gw_hybr_operational(
  policyman_ss_info_t *pInfo,
  boolean state
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_ACTIVE_BAND()

  DESCRIPTION
    GET active band for stack
 ===========================================================================*/
sys_band_class_e_type policyman_ss_get_active_band(
  policyman_state_t         *pState,
  sys_modem_as_id_e_type     subsId,
  sys_modem_stack_id_e_type  num_stack
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_REJECT_CAUSE()

  DESCRIPTION
    GET registration reject cause for stack
 ===========================================================================*/
byte policyman_ss_get_reject_cause(
  policyman_state_t         *pState,
  sys_modem_as_id_e_type     subsId,
  sys_modem_stack_id_e_type  num_stack
);

/*-------- policyman_condition_serving_plmn_new --------*/
mre_status_t policyman_condition_serving_plmn_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_service_status_new --------*/
mre_status_t policyman_condition_service_status_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_serving_rat_new --------*/
mre_status_t policyman_condition_serving_rat_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);


/*-------- policyman_condition_serving_rf_band_new --------*/
mre_status_t policyman_condition_serving_rf_band_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_reg_reject_new --------*/
mre_status_t policyman_condition_reg_reject_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_serving_domain_has_new --------*/
mre_status_t policyman_condition_serving_domain_has_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_enforce_full_rat_new --------*/
mre_status_t policyman_condition_enforce_full_rat_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_oos_scan_cnt_new --------*/
mre_status_t policyman_condition_oos_scan_cnt_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_location_mcc_new --------*/
mre_status_t policyman_condition_location_mcc_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_have_service_new --------*/
mre_status_t policyman_condition_have_service_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_condition_have_location_new --------*/
mre_status_t policyman_condition_have_location_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t              **ppCondition
);

/*===========================================================================
  FUNCTION POLICYMAN_CMSS_EVT_CB()

  DESCRIPTION
    Queue CM SS events into PolicyMan task

  PARAMETERS
    evt     : CM SS EVENT type
    p_info : actual payload of CM SS EVENT
===========================================================================*/
void policyman_cmss_event_cb(
  cm_ss_event_e_type          evt,
  const cm_mm_ss_info_s_type *p_info
);

/*===========================================================================
  FUNCTION POLICYMAN_MSIM_CMSS_EVENT_CB()

  DESCRIPTION
    Handle CM SS event callback function for Multisim

  PARAMETERS
    evt     : CM SS EVENT type
    p_info : actual payload of CM SS EVENT
===========================================================================*/
void policyman_msim_cmss_event_cb(
  cm_ss_event_e_type               evt,
  const cm_mm_msim_ss_info_s_type *p_info
);


/*-------- policyman_ss_update_state --------*/
/**
@brief  Handle a serving system state update.

@param

@return

*/
void policyman_ss_update_state(
  policyman_state_t       *pState,
  policyman_ss_info_t     *pNewInfo,
  sys_modem_as_id_e_type   asubs_id
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_PLMN_ID_TYPE()

  DESCRIPTION
    GET PLMN ID type for MAIN stack
 ===========================================================================*/
sys_sys_id_type_e_type policyman_ss_get_plmn_id_type(
  policyman_ss_info_t       *pSsInfo,
  sys_modem_stack_id_e_type  num_stack
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_SET_DEFAULT_VALUE()

  DESCRIPTION
    Set Default value on both stacks
 ===========================================================================*/
void policyman_ss_set_default_value(
  policyman_ss_info_t       *pInfo,
  sys_modem_stack_id_e_type  num_stack
);

/*-------- policyman_ss_subs_is_in_pwr_save --------*/
boolean policyman_ss_subs_is_in_pwr_save(
  policyman_state_t     *pState,
  sys_modem_as_id_e_type subsId
);

/*-------- policyman_any_subs_has_service --------*/
boolean policyman_any_subs_has_service(
  policyman_state_t *pState
);

/*-------- policyman_ss_get_min_acq_fail_count --------*/
int policyman_ss_get_min_acq_fail_count(
  policyman_state_t     *pState,
  sys_modem_as_id_e_type subsId
);

/*-------- policyman_find_subs_with_location_info --------*/
sys_modem_as_id_e_type policyman_find_subs_with_location_info(
  policyman_state_t       *pState
);

/*-------- policyman_subs_has_location --------*/
boolean policyman_subs_has_location(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
);

/*-------- location_is_in_mcc_set --------*/
boolean location_is_in_mcc_set(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type   subsId,
  policyman_set_t         *pMccSet
);


#endif /* _POLICYMAN_SERVING_SYSTEM_H_ */
