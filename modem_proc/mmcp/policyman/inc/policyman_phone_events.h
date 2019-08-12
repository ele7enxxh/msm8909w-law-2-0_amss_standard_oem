#ifndef _POLICYMAN_PHONE_EVENTS_H_
#define _POLICYMAN_PHONE_EVENTS_H_

/**
  @file policyman_phone_events.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_phone_events.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "cm.h"
#include "policyman_i.h"
#include "policyman_diag.h"

typedef struct policyman_cmph_info_s  policyman_cmph_info_t;

typedef struct policyman_cmph_history_entry_s   policyman_cmph_history_entry_t;
typedef struct policyman_cmph_history_s         policyman_cmph_history_t;

/*-------- policyman_cmph_get_history --------*/
size_t policyman_cmph_get_history(
  pm_dump_cm_ph_info_history_t *h_buffer,
  size_t                        h_len
);

/*-------- policyman_cmph_fill_diag_ss_info --------*/
void policyman_cmph_fill_diag_ph_info(
  policyman_cmph_info_t   *subs_ph_info,
  pm_dump_cm_ph_info_t    *dump_ph_info
);

/*-------- policyman_cmph_init_history --------*/
void policyman_cmph_init_history(
  policyman_state_t  *pState
);

/*-------- policyman_cmph_deinit_history --------*/
void policyman_cmph_deinit_history(
  policyman_state_t  *pState
);

/*-------- policyman_cmph_init --------*/
boolean policyman_cmph_init(
  policyman_cmph_info_t **ppInfo,
  sys_modem_as_id_e_type asubs_id
  );

/*-------- policyman_cmph_nv_init --------*/
void policyman_cmph_nv_init(
  policyman_cmph_info_t   **ppInfo,
  sys_modem_as_id_e_type    asubs_id
);

/*-------- policyman_cmph_deinit --------*/
void policyman_cmph_deinit(
  policyman_cmph_info_t *pInfo
  );

/*-------- policyman_cmph_info_deep_copy --------*/
policyman_cmph_info_t * policyman_cmph_info_deep_copy(
  policyman_cmph_info_t  *pCmPhState
);

/*-------- policyman_ph_get_mode_pref --------*/
/**
@brief  

@param

@return
  
*/
cm_mode_pref_e_type policyman_ph_get_mode_pref(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
  );

/*-------- policyman_ph_get_srv_domain --------*/
/**
@brief  

@param

@return
  
*/
cm_srv_domain_pref_e_type policyman_ph_get_srv_domain(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
  );


/*-------- policyman_ph_get_band_pref --------*/
/**
@brief  

@param

@return
  
*/
void policyman_ph_get_band_pref(
  policyman_state_t         *pState,
  sys_modem_as_id_e_type     asubs_id,
  cm_band_pref_e_type       *gw_bands,
  sys_lte_band_mask_e_type  *lte_bands,
  cm_band_pref_e_type       *tds_bands
);


/*===========================================================================
  FUNCTION policyman_ph_get_volte_is_enabled()

  DESCRIPTION
    Get per subs volte_is_enabled from PM Phone state

  PARAMETERS
    None

  RETURN VALUE
    TRUE if volte_is_enabled, FALSE otherwise
===========================================================================*/
boolean policyman_ph_get_volte_is_enabled(
  policyman_state_t        *pState,
  sys_modem_as_id_e_type    subsId
);


/*===========================================================================
  FUNCTION policyman_ph_set_volte_enabled_state()

  DESCRIPTION
    Set volte_state into PM Phone state

  PARAMETERS
    sys_modem_as_id_e_type : Subscription to update
    cm_volte_enable_e_type : Value of the volte_enabled state to be set

  RETURN VALUE
    Boolean: TRUE if the internal volte state was changed
             FALSE if the requested volte state was already in the PM state
===========================================================================*/
boolean policyman_ph_set_volte_enabled_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id,
  cm_volte_enable_e_type  volte_state
);


/*===========================================================================
  FUNCTION policyman_ph_get_volte_enabled_state()

  DESCRIPTION
    Get volte_enabled state from PM Phone state

  PARAMETERS
    asubs_id:  subscription to get the state from

  RETURN VALUE
    Value of 'volte_state' in the subscription state
===========================================================================*/
cm_volte_enable_e_type policyman_ph_get_volte_enabled_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
);


/*-------- policyman_ue_is_online --------*/
/**
@brief

@param

@return

*/
boolean policyman_ue_is_online(
  policyman_state_t *pState
);


/*-------- policyman_ph_get_net_sel_mode_pref --------*/
cm_network_sel_mode_pref_e_type policyman_ph_get_net_sel_mode_pref(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
);


/*-------- policyman_ph_get_user_plmn --------*/
sys_plmn_id_s_type policyman_ph_get_user_plmn(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
);


/*=============================================================================
  Phone-related rule conditions
=============================================================================*/

/*-------- policyman_condition_ph_domain_pref_new --------*/
mre_status_t policyman_condition_ph_domain_pref_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  );

/*-------- policyman_condition_ph_rat_pref_new --------*/
mre_status_t policyman_condition_ph_rat_pref_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  );

/*-------- policyman_condition_ph_operating_mode_new --------*/
mre_status_t policyman_condition_ph_operating_mode_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  );

/*-------- policyman_condition_ph_user_mcc_new --------*/
mre_status_t policyman_condition_ph_user_mcc_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  );

/*-------- policyman_condition_ph_network_selection_mode_new --------*/
mre_status_t policyman_condition_ph_network_selection_mode_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  );

/*-------- policyman_condition_volte_enabled_new --------*/
mre_status_t policyman_condition_volte_enabled_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_condition_t         **ppCondition
);

/*===========================================================================
  FUNCTION POLICYMAN_PH_SET_OPRT_MODE()

  DESCRIPTION
    Set operating mode into PM Phone state

  PARAMETERS
    pPhInfo     : Pointer to PM Phone State info
    oprt_mode : operating mode to be set

  RETURN VALUE
    TRUE if mode_pref changed, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean policyman_ph_set_oprt_mode(
  policyman_cmph_info_t *pPhInfo, 
  sys_oprt_mode_e_type oprt_mode
  );

/*===========================================================================
  FUNCTION POLICYMAN_PH_GET_OPRT_MODE()

  DESCRIPTION
    Get operating mode from PM Phone state

  PARAMETERS
    sys_modem_as_id_e_type : Subscription to retrieve from

  RETURN VALUE
    TRUE if mode_pref changed, FALSE otherwise
===========================================================================*/
sys_oprt_mode_e_type policyman_ph_get_oprt_mode(
  policyman_state_t     *pState,
  sys_modem_as_id_e_type subsId
  );

/*-------- policyman_event_get_cm_client_id --------*/
cm_client_id_type policyman_event_get_cm_client_id(
  void
  );

/*===========================================================================
  FUNCTION policyman_update_persisted_user_plmn()

  DESCRIPTION
    Update user selected plmn into EFS

  PARAMETERS
    sys_modem_as_id_e_type : Subscription to update 

  RETURN VALUE
    void
===========================================================================*/
void policyman_update_persisted_user_plmn(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================
  FUNCTION policyman_phone_get_rtre_control()
===========================================================================*/
cm_rtre_control_e_type policyman_phone_get_rtre_control(
  policyman_state_t 	*pState,
  sys_modem_as_id_e_type asubs_id
);


#endif /* _POLICYMAN_PHONE_EVENTS_H_ */
