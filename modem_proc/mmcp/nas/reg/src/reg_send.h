#ifndef REG_SEND_H
#define REG_SEND_H
/*==============================================================================


                   R E G  -  S E N D   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains functions for sending messages to CM and MM.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_send.h_v   1.2   07 Jun 2002 14:22:58   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_send.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/15/02   kwa     Initial version.
05/29/02   kwa     Replaced rat_search_order with rat in mmr_reg_req.
06/06/02   kwa     Replaced mode_pref with dual_mode and rat in
                   mmr_plmn_search_req.
06/27/02   kwa     Updated functions that send the mmr_plmn_search_req and 
                   mmr_reg_req.
09/19/02   kwa     Added function to send an mmr_act_req.
10/18/02   kwa     Added function to send a cm_deep_sleep_ind.
05/21/03   kwa     Modified reg_send_cm_mm_information_ind to pass the info_p
                   parameter as a const pointer to eliminate a lint warning.
                   Modified reg_send_cm_network_list_cnf to pass the plmn_list_p
                   parameter as a const pointer to eliminate a lint warning.
08/12/03   kwa     Added function reg_send_last_mmr_reg_req.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added sys_band_mask_type to reg_send_mmr_act_req, 
                   reg_send_mmr_plmn_search_req, and reg_send_mmr_reg_req.
12/10/03   kwa     Added reg_send_last_mmr_reg_req_gsm_only to support a change
                   in handling MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED so the phone
                   will attempt to acquire GSM_ONLY service on the same PLMN.
02/04/04   kwa     Added ps_for_request parameter to reg_send_mmr_reg_req.
05/16/04   kwa     Added cs_for_request parameter to reg_send_mmr_reg_req.
                   Added function to send a cm_camped_ind.
07/19/04   kwa     Removed service_state parameter from cm_camped_ind.
07/26/04   kwa     Added active_rat and active_band parameter to
                   reg_send_cm_camped_ind.
11/10/04   kwa     Added higher_priority_plmn_list_p parameter to
                   reg_send_mmr_plmn_search_req.
07/15/05   up      Added function reg_send_mmr_eq_plmn_change_ind.
09/19/05   up      Added reg_send_cm_hplmn_search_cnf to support on demand 
                   HPLMN search from user.
11/07/05   ajt     Added prototype function reg_send_cm_emergency_num_list_ind.                   
12/08/05   up      Updated reg_send_cm_network_list_cnf to add status field.
01/02/06   up      Added reg_send_mmr_plmn_search_abort_req to abort ongoing
                   PLMN Search.
04/06/06   sn      Added reg_send_cm_cell_service_ind.
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH 
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"
#include "reg_cm_v.h"
#include "sys.h"
#include "sys_v.h"
#include "sys_plmn_selection_v.h"
#include "reg_mm_v.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/


/*==============================================================================

FUNCTION NAME

  reg_send_cm_camped_ind

DESCRIPTION

  Function that fills and sends a cm_camped_ind message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_camped_ind
(
#ifndef FEATURE_GSM_BAND_PREF
  sys_radio_access_tech_e_type    active_rat
#else
  sys_radio_access_tech_e_type    active_rat,
  sys_active_band_u_type          active_band
#endif
   ,sys_plmn_id_s_type             plmn
  ,sys_lac_type                   lac_id
);
/*==============================================================================

FUNCTION NAME

  reg_send_cm_attach_failed_ind

DESCRIPTION

  Function that fills and sends a cm_camped_ind message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_attach_failed_ind
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_send_cm_mm_information_ind

DESCRIPTION

  Function that fills and sends a cm_mm_information_ind message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_mm_information_ind
(
  const sys_mm_information_s_type*      info_p
);


/*==============================================================================

FUNCTION NAME

  reg_send_cm_network_list_cnf

DESCRIPTION

  Function that fills and sends a cm_network_list_cnf message to CM.

RETURN VALUE
  None

==============================================================================*/

extern void reg_send_cm_network_list_cnf
(
        reg_cm_transaction_id_type      transaction_id,
        sys_plmn_id_s_type              rplmn,
  const sys_found_plmn_list_u_type*     found_plmn_list,
        reg_mm_plmn_search_status_e_type status
);

/*==============================================================================

FUNCTION NAME

  reg_send_cm_service_cnf

DESCRIPTION

  Function that fills and sends a cm_service_cnf message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_service_cnf
(
  reg_cm_transaction_id_type      transaction_id,
  sys_plmn_service_state_s_type   service_state
);


/*==============================================================================

FUNCTION NAME

  reg_send_cm_service_ind

DESCRIPTION

  Function that fills and sends a cm_service_ind message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_service_ind
(
  sys_plmn_service_state_s_type   service_state
);


/*==============================================================================

FUNCTION NAME

  reg_send_cm_sim_available_cnf

DESCRIPTION

  Function that fills and sends a cm_sim_available_cnf message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_sim_available_cnf
(
  reg_cm_transaction_id_type      transaction_id
);


/*==============================================================================

FUNCTION NAME

  reg_send_cm_sim_not_available_cnf

DESCRIPTION

  Function that fills and sends a cm_sim_not_available_cnf message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_sim_not_available_cnf
(
  reg_cm_transaction_id_type      transaction_id
);


/*==============================================================================

FUNCTION NAME

  reg_send_cm_stop_mode_cnf

DESCRIPTION

  Function that fills and sends a cm_stop_mode_cnf message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_stop_mode_cnf
(
  reg_cm_transaction_id_type      transaction_id
);




/*==============================================================================

FUNCTION NAME

  reg_send_cm_ph_status_chgd_cnf

DESCRIPTION

  Function that fills and sends a cm_ph_status_chgd_cnf message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_ph_status_chgd_cnf
(
  reg_cm_transaction_id_type      transaction_id,
  sys_ph_status_chgd_status_e_type status
);

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*==============================================================================

FUNCTION NAME

  reg_send_cm_hplmn_search_cnf

DESCRIPTION

  Function that fills and sends a cm_hplmn_search_cnf message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_hplmn_search_cnf
(
  boolean                         search_initiated
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_send_cm_cell_service_ind

DESCRIPTION

  Function that sends cm_cell_service_ind message to CM.

RETURN VALUE

  None

==============================================================================*/
void reg_send_cm_cell_service_ind
(
  cm_cell_service_ind_s_type* cell_service_ind
);

#ifdef FEATURE_HSPA_CALL_STATUS_IND
/*==============================================================================

FUNCTION NAME

  reg_send_cm_hspa_call_status_ind

DESCRIPTION

  Function that sends cm_hspa_call_status_ind message to CM.

RETURN VALUE

  None

==============================================================================*/
void reg_send_cm_hspa_call_status_ind
(
  cm_hspa_call_status_ind_s_type* call_status_ind
);
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
/*==============================================================================

FUNCTION NAME

  reg_send_cm_conn_mode_ss_ind

DESCRIPTION

  Function that sends cm_conn_mode_ss_ind message to CM.

RETURN VALUE

  None

==============================================================================*/
void reg_send_cm_conn_mode_ss_ind
(
  cm_conn_mode_ss_ind_s_type* conn_mode_ss_ind
);
#endif
/*==============================================================================

FUNCTION NAME

  reg_send_mmr_plmn_search_req

DESCRIPTION

  Function that fills and sends an mmr_plmn_search_req message to MM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_mmr_plmn_search_req
(
  sys_network_selection_mode_e_type         network_selection_mode,
  boolean                                   dual_mode,
  sys_radio_access_tech_e_type              rat
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                       band_pref
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) ||defined(FEATURE_RAT_PRIORITY_LIST)
  ,const sys_plmn_rat_list_s_type*          higher_priority_plmn_list_p
#endif
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) ||defined(FEATURE_LTE)
  ,sys_service_search_e_type                 service_search
#endif
#ifdef FEATURE_FEMTO_CSG
  ,sys_csg_search_type_e_type             csg_search_type
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type   *rat_pri_list_info
  ,const sys_rat_pri_list_info_s_type   *mcc_mode_band_filter 				
#endif
  ,sys_scan_scope_e_type   scan_scope
);


/*==============================================================================

FUNCTION NAME

  reg_send_mmr_reg_req

DESCRIPTION

  Function that fills and sends an mmr_reg_req message to MM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_mmr_reg_req
(
  sys_network_selection_mode_e_type         network_selection_mode,
  sys_plmn_id_s_type                        plmn,
  sys_srv_domain_e_type                     req_service_domain,
  sys_mode_pref_e_type                      mode_pref,
  boolean                                   cs_for_request,
  boolean                                   ps_for_request,
  sys_radio_access_tech_e_type              rat
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                       band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type       *rat_pri_list_info
#endif
  ,mmr_reg_req_e_type                       type
#ifdef FEATURE_LTE
  , boolean    sms_only,
    uint32     additional_info
#endif
#ifdef FEATURE_FEMTO_CSG
  ,sys_csg_id_type    csg_id
#endif 
);

extern void reg_send_mmr_gstk_service(boolean service_available);


/*==============================================================================

FUNCTION NAME

  reg_send_last_mmr_reg_req_gsm_only

DESCRIPTION

  Function that sends the last populated mmr_reg_req message to MM with
  the dual_mode flag set to FALSE and the rat set to GSM.

RETURN VALUE

  None

==============================================================================*/

void reg_send_last_mmr_reg_req_gsm_only
(
  void
);

#ifdef FEATURE_HPLMN_NOT_ALLOWED_SELECT_NEXT
/*==============================================================================

FUNCTION NAME

  reg_send_last_mmr_reg_req_invalid_plmn

DESCRIPTION

  Function that sends the last populated mmr_reg_req message to MM with
  invalid PLMN to get avaiable PLMN list from current RAT.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_last_mmr_reg_req_invalid_plmn
(
  void
);
#endif

/*=============================================================================
FUNCTION NAME

  reg_send_last_mmr_reg_req_new_type

DESCRIPTION

  Function that sends the last populated mmr_reg_req message to MM with
  mmr_reg_req type set to the one who is calling this API

RETURN VALUE

  None

=============================================================================*/

extern void reg_send_last_mmr_reg_req_new_type
(
  mmr_reg_req_e_type type
);
/*=============================================================================
FUNCTION NAME  
reg_send_last_mmr_reg_req_for_call
DESCRIPTION  
Function that sends the last populated mmr_reg_req message to MM with  mmr_reg_req cs_for_request/ps_for_request set to current value.If LTE present in given RAT priority list,  its bst_rat_acq_required and bst_band_cap will be used.
RETURN VALUE  None
=============================================================================*/
extern void reg_send_last_mmr_reg_req_for_call
(
  sys_rat_pri_list_info_s_type rat_pri_list_info
);


#ifdef FEATURE_LTE
/*==============================================================================
FUNCTION NAME

  reg_send_mmr_reg_req_curr_rat_list
==============================================================================*/
extern void reg_send_mmr_reg_req_curr_rat_list
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_send_mmr_block_plmn_req

==============================================================================*/

extern void reg_send_mmr_block_plmn_req
(
  sys_plmn_id_s_type        plmn,
  rex_timer_cnt_type        timer_count,
  boolean                   is_plmn_blocked_for_eplmn_check,
  sys_block_plmn_cause_e_type cause
);

#endif


/*==============================================================================

FUNCTION NAME

  reg_send_last_mmr_reg_req

DESCRIPTION

  Function that sends the last populated mmr_reg_req message to MM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_last_mmr_reg_req
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_send_mmr_sim_available_req

DESCRIPTION

  Function that fills and sends a mmr_sim_available_req message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_mmr_sim_available_req
(
#if defined FEATURE_DUAL_SIM 
  sys_mode_pref_e_type dual_mode_pref,
  mmgsdi_client_id_type mmgsdi_client_id,
  mmgsdi_session_type_enum_type mmgsdi_session_type,
  uint8 nv_context_id
#else
  mmgsdi_client_id_type mmgsdi_client_id
#endif
);


/*==============================================================================

FUNCTION NAME

  reg_send_mmr_sim_not_available_req

DESCRIPTION

  Function that fills and sends a mmr_sim_not_available_req message to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_mmr_sim_not_available_req
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_send_mmr_stop_mode_req

DESCRIPTION

  Function that fills and sends an mmr_stop_mode_req message to MM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_mmr_stop_mode_req
(
  sys_stop_mode_reason_e_type     stop_mode_reason
);

/*==============================================================================

FUNCTION NAME

  reg_send_cm_plmn_list_change_ind

==============================================================================*/
void reg_send_cm_plmn_list_change_ind
(
  sys_plmn_list_type_e_type changed_list_type
);


/*==============================================================================

FUNCTION NAME

  reg_send_eq_plmn_change_ind

DESCRIPTION

  Function send the equivalent PLMN list change indication to MM when ePLMN list updated.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_eq_plmn_change_ind
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_send_mmr_clear_lai_reject_list_req

DESCRIPTION

  Function send the MMR_CLEAR_LAI_REJECT_LIST_REQ to MM task. 

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_mmr_clear_lai_reject_list_req( void );

/*==============================================================================

FUNCTION NAME

  reg_send_cm_emergency_num_list_ind

DESCRIPTION

  Function sends the emergency number list received from MM to CM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_send_cm_emergency_num_list_ind
(
  cm_emergency_num_list_ind_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  reg_send_mmr_plmn_search_abort_req

DESCRIPTION

  Function sends the PLMN search abort request to MM to AS layer 
  to abort the ongoing Manual PLMN Search.

RETURN VALUE

  None

==============================================================================*/
extern void reg_send_mmr_plmn_search_abort_req
(
  void
);
/*==============================================================================

FUNCTION NAME

 reg_send_mmr_net_sel_mode_change_ind

DESCRIPTION

  Function sends the network selection mode change indication to MM if UE already registered on PLMN before looping back with confrimation to CM.

RETURN VALUE

  None

==============================================================================*/
extern void reg_send_mmr_net_sel_mode_change_ind
(
  sys_network_selection_mode_e_type   network_selection_mode
);
/*==============================================================================

FUNCTION NAME

  reg_send_mmr_ph_status_chng_req

DESCRIPTION

  Function sends phone status changed request (due to physical power cycle)
  to MM to AS layer to deactivate UMTS stack and write to NV.

RETURN VALUE

  None

==============================================================================*/
extern void reg_send_mmr_ph_status_chng_req
(
  sys_stop_mode_reason_e_type
);

#ifdef FEATURE_OOSC_USER_ACTION
/*==============================================================================

FUNCTION NAME

  reg_send_mmr_exit_from_pwr_save

DESCRIPTION

  Function sends key press ind to MM

RETURN VALUE

  None

==============================================================================*/
extern void reg_send_mmr_exit_from_pwr_save
(
  void
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_send_mmr_reg_reject_ind

DESCRIPTION



RETURN VALUE

  None

==============================================================================*/
extern void reg_send_cm_reg_reject_ind
(
  sys_srv_domain_e_type      reject_domain,
  byte                       reject_cause,
  sys_plmn_id_s_type  plmn,
  sys_lac_type        tac
#ifdef FEATURE_FEMTO_CSG
  , sys_csg_id_type    csg_id
#endif 
  ,sys_radio_access_tech_e_type  rat
);

 /*==============================================================================

 FUNCTION NAME

   reg_send_grat_scan_done_set

 DESCRIPTION



 RETURN VALUE

   None


 ==============================================================================*/

extern void reg_send_grat_scan_done_set
(
  boolean      flag
);

#ifdef FEATURE_DUAL_SIM
 /*==============================================================================

 FUNCTION NAME

   reg_send_mmr_ds_stat_chgd_req

 DESCRIPTION



 RETURN VALUE

   None


 ==============================================================================*/

extern void reg_send_mmr_ds_stat_chgd_req
(
  boolean     dual_standby_mode,
  sys_modem_dual_standby_pref_e_type dual_standby_mode_info,
  sys_modem_device_mode_e_type    device_mode
);

 /*==============================================================================

 FUNCTION NAME

   reg_send_cm_ds_stat_chgd_cnf

 DESCRIPTION



 RETURN VALUE

   None


 ==============================================================================*/

void reg_send_cm_ds_stat_chgd_cnf
(
  reg_cm_transaction_id_type      transaction_id
);

/*==============================================================================

 FUNCTION NAME

   reg_send_cb_ds_stat_chgd_ind

 DESCRIPTION


 RETURN VALUE

   None


 ==============================================================================*/

extern void reg_send_cb_ds_stat_chgd_ind 
(
   uint8 active_subs
);

#endif
/*==============================================================================

 FUNCTION NAME

   reg_send_cm_cell_access_ind

 DESCRIPTION
 
  Function to send barred/unbarred status of a cell to CM


 RETURN VALUE

   None


 ==============================================================================*/
extern void reg_send_cm_cell_access_ind
(
 sys_domain_access_bar_s_type       cell_access_status
);

#if defined(FEATURE_DUAL_SIM)
 /*==============================================================================

 FUNCTION NAME

   reg_send_mmr_ds_tuneaway_status_change_ind

 DESCRIPTION
   Sends MMR_DS_TUNEAWAY_STATUS_CHANGE_IND message to MM

 RETURN VALUE

   None


 ==============================================================================*/

extern void reg_send_mmr_ds_tuneaway_status_change_ind
(
  const boolean                         tuneaway_status
);
#endif

#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)

/*==============================================================================

FUNCTION NAME

 reg_send_cm_suitable_search_end_ind

DESCRIPTION



RETURN VALUE

 None


==============================================================================*/
void reg_send_cm_suitable_search_end_ind
(
  void
);
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
void reg_send_cm_disable_bplmn_ind
(
  mmr_disable_bplmn_ind_s_type mmr_disable_bplmn_ind
);
#endif
#ifdef FEATURE_NAS_ECALL
 /*==============================================================================

 FUNCTION NAME

   reg_send_mmr_ecall_mode_ind

 DESCRIPTION

   Function to send MMR_ECALL_MODE_IND to MM

 RETURN VALUE

   None


 ==============================================================================*/
extern void reg_send_mmr_ecall_mode_ind
(
 mmr_ecall_mode_e_type mode_type
);
/*==============================================================================
 FUNCTION NAME

   reg_send_mmr_profile_type_ind

 DESCRIPTION
   Function to send MMR_PROFILE_TYPE_IND to MM

 RETURN VALUE

   None
 ==============================================================================*/
extern void reg_send_mmr_profile_type_ind
(
  mmgsdi_profile_type_enum_type  profile_type
);
#endif/*FEATURE_NAS_ECALL*/

#ifdef FEATURE_FEMTO_CSG
 /*==============================================================================

 FUNCTION NAME

   reg_send_mmr_file_refresh_ind

 DESCRIPTION

   Function to send refresh indication to MM

 RETURN VALUE

   None


 ==============================================================================*/
extern void reg_send_mmr_file_refresh_ind
(
  mmgsdi_file_enum_type file_name
);
#endif 

#ifdef FEATURE_LTE
 /*==============================================================================

 FUNCTION NAME

   reg_send_mmr_ps_detach_req

 DESCRIPTION

   Function to send MMR_PS_DETACH_REQ to MM

 RETURN VALUE

   None

 ==============================================================================*/
void reg_send_mmr_ps_detach_req
(
 const sys_ps_detach_e_type  detach_type
);

 /*==============================================================================

 FUNCTION NAME

   reg_send_cm_ps_detach_cnf

 DESCRIPTION

   Function to send CM_PS_DETACH_CNF to MM

 RETURN VALUE

   None

 ==============================================================================*/
void reg_send_cm_ps_detach_cnf
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_send_cm_fplmn_list_ind

==============================================================================*/
extern void reg_send_cm_fplmn_list_ind
(
  sys_forbidden_list_type_e_type   list_type
);

/*==============================================================================

FUNCTION NAME

  reg_send_cm_set_drx_cnf

==============================================================================*/
extern void reg_send_cm_set_drx_cnf
(
  boolean result
);

/*==============================================================================

FUNCTION NAME

  reg_send_mmr_set_drx_req

==============================================================================*/
extern void reg_send_mmr_set_drx_req
(
 const sys_drx_cn_coefficient_s1_e_type drx_coefficient
);

/*==============================================================================

FUNCTION NAME

  reg_send_cm_get_drx_cnf

==============================================================================*/
extern void reg_send_cm_get_drx_cnf
(
  const sys_drx_cn_coefficient_s1_e_type drx_coefficient
);

/*==============================================================================

FUNCTION NAME

  reg_send_mmr_get_drx_req

==============================================================================*/
extern void reg_send_mmr_get_drx_req
(
 void
);
#endif /*FEATURE_LTE*/

/*==============================================================================

FUNCTION NAME

  reg_send_mmr_sim_state_update_ind

==============================================================================*/
extern void reg_send_mmr_sim_state_update_ind
(
 const sys_sim_state_e_type sim_state
);

/*==============================================================================

FUNCTION NAME

  reg_send_last_reg_req_plmn_get

==============================================================================*/

extern sys_plmn_id_s_type reg_send_last_reg_req_plmn_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_send_last_rat_pri_list_get

==============================================================================*/

extern sys_rat_pri_list_info_s_type* reg_send_last_rat_pri_list_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_send_last_reg_req_rat_get

==============================================================================*/

extern sys_radio_access_tech_e_type reg_send_last_reg_req_rat_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_send_cm_sim_update_ind

==============================================================================*/
void reg_send_cm_sim_update_ind
(
  const sys_sim_state_e_type sim_state
);

#ifdef FEATURE_SGLTE
extern void reg_send_ps_timer_expiry(void);
extern void reg_send_cm_stack_deact_ind(void);
extern void reg_send_cm_stack_act_ind(void);
extern void reg_send_cm_stat_ind
(
  sys_plmn_service_state_s_type   service_state
);

#endif

/*==============================================================================

FUNCTION NAME

  reg_send_cm_get_hplmn_search_timer_cnf

==============================================================================*/
extern void reg_send_cm_get_hplmn_search_timer_cnf
(
  uint32 timer_value
);

#ifdef FEATURE_DUAL_SIM
/*==============================================================================

FUNCTION NAME

  reg_send_cm_multimode_subs_chgd_cnf

==============================================================================*/
extern void reg_send_cm_multimode_subs_chgd_cnf
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_send_mmr_multimode_subs_chgd_req

==============================================================================*/

extern void reg_send_mmr_multimode_subs_chgd_req
(
  subs_capability_e_type  *subs_capability
);


/*==============================================================================

FUNCTION NAME

  reg_send_mmr_dds_switch_ind

==============================================================================*/
extern void reg_send_mmr_dds_switch_ind
(
 sys_modem_as_id_e_type target_dds_sub
);


#endif
#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_send_mmr_gw_plmn_sel_end_ind

==============================================================================*/
extern void reg_send_mmr_gw_plmn_sel_end_ind
(
 void
);
#endif

#if defined(FEATURE_LTE) && defined(FEATURE_FEMTO_CSG)
/*==============================================================================

FUNCTION NAME

  reg_send_cm_home_nodeb_name_ind

==============================================================================*/
extern void reg_send_cm_home_nodeb_name_ind
(
  mmr_home_nodeb_name_ind_s_type *home_nodeb_name
);
#endif

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
void reg_send_mmr_sim_busy_ind(boolean sim_busy);
#endif

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================
FUNCTION NAME:  reg_send_mmr_csg_select_config_update_ind

DESCRIPTION
  This REG function sends MMR_CSG_SELECT_CONFIG_UPDATE_IND to MM.

RETURN VALUE
  none
==============================================================================*/
extern void reg_send_mmr_csg_select_config_update_ind
(
  const sys_csg_selection_config_e_type csg_selection_config
);

#ifdef FEATURE_DUAL_DATA
/*==============================================================================
FUNCTION NAME:  reg_send_mmr_data_priority_ind

DESCRIPTION
  This REG function sends MMR_DATA_PRIORITY_IND to MM.

RETURN VALUE
  none
==============================================================================*/
void reg_send_mmr_data_priority_ind
(
   sys_data_priority_e_type  reg_data_prio_ind
);
#endif
/*==============================================================================
FUNCTION NAME:  reg_send_cm_csg_search_trigger_ind

DESCRIPTION
  This REG function sends CM_CSG_SEARCH_TRIGGER_IND to CM.

RETURN VALUE
  none
==============================================================================*/
extern void reg_send_cm_csg_search_trigger_ind
(
  void
);

/*==============================================================================
FUNCTION NAME:  reg_send_cm_csg_nw_sel_mode_update_ind

DESCRIPTION
  This REG function sends CM_CSG_NW_SEL_MODE_UPDATE_IND to CM.

RETURN VALUE
  none
==============================================================================*/
extern void reg_send_cm_csg_nw_sel_mode_update_ind
(
  const sys_network_selection_mode_e_type nw_sel_mode
);

/*==============================================================================
FUNCTION NAME:  reg_send_get_nw_sel_mode_sent_to_mm_get
==============================================================================*/
extern sys_network_selection_mode_e_type reg_send_nw_sel_mode_sent_to_mm_get
(
  void
);
#endif /*FEATURE_FEMTO_CSG*/

extern void reg_send_mmr_pseudo_list_change_ind(sys_radio_access_tech_e_type rat, boolean can_use_pseudo);


/*==============================================================================
FUNCTION NAME:  reg_send_cm_t3346_timer_ind

DESCRIPTION
  This REG function sends CM_NAS_T3346_TIMER_IND to CM.

RETURN VALUE
  none
==============================================================================*/
void reg_send_cm_t3346_timer_ind
(
   mmr_t3346_timer_ind_s_type *msg_ptr 
);


/*==============================================================================
FUNCTION NAME:  reg_send_mmr_psm_ready_req

DESCRIPTION
  This REG function sends MMR_PSM_READY_REQ to MM.

RETURN VALUE
  none
==============================================================================*/

extern void reg_send_mmr_psm_ready_req
(
  void
);

/*==============================================================================
FUNCTION NAME:  reg_send_cm_psm_ready_rsp

DESCRIPTION
  This REG function sends CM_PSM_READY_RSP to CM.

RETURN VALUE
  none
==============================================================================*/
extern void reg_send_cm_psm_ready_rsp
(
  sys_psm_status_e_type status
);

/*==============================================================================
FUNCTION NAME:  reg_send_cm_psm_ready_ind

DESCRIPTION
  This REG function sends CM_PSM_READY_IND to CM.

RETURN VALUE
  none
==============================================================================*/
extern void reg_send_cm_psm_ready_ind
(
  void
);

#endif

