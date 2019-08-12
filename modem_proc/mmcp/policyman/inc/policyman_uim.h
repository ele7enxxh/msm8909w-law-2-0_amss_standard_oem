#ifndef _POLICYMAN_UIM_H_
#define _POLICYMAN_UIM_H_

/**
  @file policyman_uim.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_uim.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_cm.h"
#include "policyman_i.h"
#include "policyman_state.h"

#include "sys.h"
#include "mmgsdisessionlib.h" /* Included for MMGSDI Session prototypes */
#include "policyman_dbg.h"


typedef struct 
{
  sys_plmn_id_s_type    plmn;
  mmgsdi_app_enum_type  sim_app_type;
  mmgsdi_app_enum_type  cdma_app_type;
  boolean               sim_refreshed;
  boolean               subs_active;
  sys_mcc_type          csim_mcc;
} policyman_uim_info_t;

#define MAX_SIMS  3

/*-------- policyman_uim_init --------*/
boolean policyman_uim_init(
  policyman_uim_info_t  **pUimInfo
);

/*-------- policyman_uim_deinit --------*/
void policyman_uim_deinit(
  policyman_uim_info_t  *pUimInfo
);

/*-------- policyman_uim_info_deep_copy --------*/
policyman_uim_info_t * policyman_uim_info_deep_copy(
  policyman_uim_info_t  *pUimState
);


/*-------- policyman_uim_get_imsi_plmn --------*/  
boolean policyman_uim_get_imsi_plmn(
  policyman_state_t  *pInfo,
  size_t              subsId,
  sys_plmn_id_s_type  *pPlmn
);

/*-------- policyman_uim_set_sim_type --------*/
boolean policyman_uim_set_sim_type(
  policyman_state_t   *pInfo,
  size_t               subsId,
  mmgsdi_app_enum_type sim_app_type  
);

/*-------- policyman_uim_get_sim_type --------*/
mmgsdi_app_enum_type policyman_uim_get_sim_type(
  policyman_state_t    *pInfo,
  size_t                subsId
);

/*-------- policyman_uim_set_cdma_sim_type --------*/
boolean policyman_uim_set_cdma_sim_type(
  policyman_state_t    *pInfo,
  size_t                subsId,
  mmgsdi_app_enum_type  sim_app_type
);

/*-------- policyman_uim_get_cdma_sim_type --------*/
mmgsdi_app_enum_type policyman_uim_get_cdma_sim_type(
  policyman_state_t     *pInfo,
  size_t                 subsId
);

/*-------- policyman_uim_set_sim_refresh --------*/
void policyman_uim_set_sim_refresh(
  policyman_state_t *pInfo,
  size_t             subsId,
  boolean            simWasRefreshed
);

/*-------- policyman_uim_get_sim_refresh --------*/
boolean policyman_uim_get_sim_refresh(
  policyman_state_t *pInfo,
  size_t  subsId
);

/*-------- policyman_uim_sim_is_3gpp2 --------*/
boolean policyman_uim_sim_is_3gpp2(
  policy_execute_ctx_t *pCtx
);

/*-------- policyman_condition_sim_type_new --------*/
mre_status_t policyman_condition_sim_type_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
);

/*-------- policyman_condition_imsi_plmn_new --------*/
mre_status_t policyman_condition_imsi_plmn_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
);

/*-------- policyman_condition_imsi_mcc_new --------*/
mre_status_t policyman_condition_imsi_mcc_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
);

/*-------- policyman_condition_sim_refresh_new --------*/
mre_status_t policyman_condition_sim_refresh_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
);

/*-------- policyman_uim_sim_refresh_reset_action_new --------*/
mre_status_t policyman_uim_sim_refresh_reset_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
);

/*-------- policyman_uim_fill_diag_session_info --------*/
void policyman_uim_fill_diag_session_info(
  size_t                          count,
  pm_dump_uim_session_info_t     *dump_uim_info
);

/*-------- policyman_uim_fill_diag_uim_info --------*/
void policyman_uim_fill_diag_uim_info(
  policyman_uim_info_t  *src_uim_info,
  pm_dump_uim_info_t    *dump_uim_info
);

/*-------- policyman_uim_del_sub --------*/  
boolean policyman_uim_del_sub(
  policyman_state_t            *pState,
  sys_modem_as_id_e_type        asubs_id,
  mmgsdi_session_type_enum_type session_type
);

/*-------- policyman_uim_get_imsi_m_mcc --------*/
uint32 policyman_uim_get_imsi_m_mcc(
  policyman_state_t  *pState,
  size_t              subsId
);


/*-------- policyman_uim_get_subs_active --------*/
boolean policyman_uim_get_subs_active(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
);

#endif /* _POLICYMAN_UIM_H_ */
