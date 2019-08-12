#ifndef MM_MULTIMODE_COMMON_H
#define MM_MULTIMODE_COMMON_H
/*===========================================================================

        M M   MULTIMODE I N F O R M A T I O N   H E A D E R   F I L E

DESCRIPTION
  This file contains types, function prototypes and data declarations  
  required for MM Information procedure

Copyright (c) 2010 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mm_multimode_common.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/12/11   HC      Replacing the #include "gmm.h" with #include "gmm_v.h"
                   as a part of CMI activity 
10/01/11   abhi    Added function to map GUTI to RAI
07/25/11   Abhi     Added function to return CSFB state
11/29/10   MNK     More 1XCSFB changes including adversarial scenarios
09/17/10   MNK     CS_NOTIFICATION for 3GPPCSFB added
06/10/10   RI      Fixed the byte swap error in mapping of mobile identity from
                   UMTS to LTE
05/21/10   zren    Updated support of handling REG CMD coming during L2GW IRAT
05/04/10   zren    Added support for MME routing info setup in RRC CONN EST REQ
                   for different TIN types
04/14/10   zren    Consolidated GMM/MM handling code for combined procedures to
                   mm_multimode_hanlder
04/09/10   zren    Added combined procedure feature
03/05/10   MNK     TIN related changes
02/12/10   MNK     First cut of i-RAT between LTE and GW

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "mmcp_variation.h"
#include <stdio.h>
#include <string.h>
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_LTE
#include "gmm_v.h"
#include "lte_nas_emm_message_type.h"

typedef struct mm_gw_identity_param
{
 gmm_ptmsi_type ptmsi;
 gmm_ptmsi_signature_type ptmsi_signature;
 routing_area_id_T ra_id;
 byte eKSI;
 auth_gprs_cipherkey_type    Ck;  
 auth_gprs_integritykey_type Ik; 
 byte ul_count_at_generation[4];
} mm_gw_identity_param_type;

typedef enum
{
  MM_CSFB_STATE_NULL,
  MM_CSFB_STATE_MO_CALL_ORIG,
  MM_CSFB_STATE_MT_CALL_ORIG,
  MM_CSFB_STATE_LU_PENDING,
  MM_CSFB_STATE_MT_CALL_CCO,
  MM_CSFB_STATE_MAX
} mm_csfb_state_type;

#ifdef FEATURE_DUAL_SIM
extern sys_radio_access_tech_e_type mm_service_loss_on_multimode_rat_sim[MAX_AS_IDS];
#define mm_service_loss_on_multimode_rat  mm_service_loss_on_multimode_rat_sim[mm_sub_id]
#else
extern sys_radio_access_tech_e_type     mm_service_loss_on_multimode_rat;
#endif

#if defined(FEATURE_DUAL_SIM)|| defined(FEATURE_SGLTE)
extern mm_csfb_state_type mm_csfb_state_sim[MAX_NAS_STACKS];
#define mm_csfb_state mm_csfb_state_sim[mm_as_id]
#else
extern mm_csfb_state_type mm_csfb_state;
#endif

extern mm_csfb_mt_call_info_s_type    mm_csfb_mt_call_info;
#ifdef FEATURE_MODEM_HEAP
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern mm_cmd_type *mm_csfb_service_req_message_ptr_sim[MAX_NAS_STACKS];
#define mm_csfb_service_req_message_ptr mm_csfb_service_req_message_ptr_sim[mm_as_id]
#else
extern mm_cmd_type *mm_csfb_service_req_message_ptr;
#endif

#else
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern mm_cmd_type mm_csfb_service_req_message_sim[MAX_NAS_STACKS];
#define mm_csfb_service_req_message mm_csfb_service_req_message_sim[mm_as_id]
#else
extern mm_cmd_type mm_csfb_service_req_message;
#endif

#endif

extern mm_gw_identity_param_type mm_mapped_gw_identity;

extern void mm_process_cco_complete_ind(emm_ctrl_data_type *emm_ctrl_data_ptr);

void emm_process_LTE_to_GW_rat_change(rex_sigs_type sigs,void **mm_process_msg_fn_ptr);

#ifdef FEATURE_NAS_REL10
void gmm_set_gmm_is_congestion_due_to_rau(boolean value);
#endif

void mm_emm_process_LTE_to_GW_rat_change
( 
  mm_cmd_type *cmd_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);
void mm_emm_process_GW_to_LTE_rat_change
( 
  lte_rrc_act_ind_s      *act_ind_ptr,
  mm_emm_dispatcher_fp  **mm_emm_dispatcher_fn_ptr,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);
void mm_process_GW_to_LTE_rat_change
(
  rex_sigs_type sigs,
  void **mm_process_msg_fn_ptr
);

void mm_update_detach_req_status
(
  byte   cause
);

boolean mmmultimode_route_L2GW_irat_mm_emm_messages
(
  void                   *command_ptr,
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp   **mm_emm_dispatcher_fn_ptr
);

void emm_get_gw_identity_from_lte(mm_gw_identity_param_type *mm_identity,   
                                  emm_ctrl_data_type     *emm_ctrl_data_ptr );
void mm_update_gw_auth_keys(mm_gw_identity_param_type gw_identity,   
                                  emm_ctrl_data_type     *emm_ctrl_data_ptr,
                                  rrc_sim_update_e_type sim_update_type);  
void emm_map_guti_to_ptmsi(gmm_ptmsi_type *ptmsi, emm_ctrl_data_type     *emm_ctrl_data_ptr);


extern boolean mm_emm_get_vowifi_call_status(emm_ctrl_data_type *emm_ctrl_data_ptr);


void emm_reset_ctrl_data
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr,
  boolean               reset_ftai_list  
);

void emm_update_rplmn_type
(
  mm_rplmn_type_enum_T rplmn_type
);

boolean emm_update_rplmn
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

void emm_update_rrc_rplmn_info
(
  void
);

/*===========================================================================

FUNCTION      MM_UPDATE_PS_REGISTRATION_SUCCESS_STATUS

DESCRIPTION
  This updates the GMM state to registered. 
  This function is called by EMM when ever EPS only registration is successfull in LTE. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_ps_registration_success_status
(
  void
);

/*===========================================================================

FUNCTION      MM_UPDATE_ATTACH_REJECT_STATUS

DESCRIPTION
  This function will update MM/GMM when EPS or combined attach is rejected.   

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_attach_reject_status
(
  byte reject_cause
);

/*===========================================================================

FUNCTION      MM_UPDATE_TAU_SR_REJECT_STATUS

DESCRIPTION
   This function will update MM/GMM when TAU gets reject 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_tau_sr_reject_status
(
  lte_nas_message_id  msg_id, 
  byte                reject_cause
);

boolean mm_get_deep_sleep_alt_scan_status(void);
void mm_set_deep_sleep_alt_scan_status(boolean set_status);
void emm_resume_paused_mm_timers(void);
/*===========================================================================

FUNCTION      MM_ACTIVATE_GW

DESCRIPTION
  This function initilize the MM/GMM state. EMM should call this function for every REG request for LTE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_activate_gw
(
  void
);

extern
boolean mmmultimode_route_GW2L_irat_mm_emm_messages
(
  void                   *command_ptr,
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp   **mm_emm_dispatcher_fn_ptr
);

void mm_emm_store_reg_req(mmr_reg_req_s_type* reg_req_ptr,   emm_ctrl_data_type     *emm_ctrl_data_ptr);
void emm_set_possible_imsi_attach(boolean imsi_attach_required);
mm_attach_failed_status_type emm_get_mmr_attach_failed_ind_status( void );
void emm_send_reg_attach_failed_ind( void ) ;
void emm_store_attach_ind_status_type(mmr_reg_req_s_type        *reg_req_ptr);
void emm_reset_attach_ind_status_type(void);
void mm_emm_store_reg_req_nw_selection_mode(sys_network_selection_mode_e_type         network_selection_mode);

void mm_map_umts_identity_to_lte(lte_nas_emm_eps_mobile_id_type* guti, byte* ptmsi, boolean *ptmis_sig_present);
void emm_populate_guti(lte_nas_emm_eps_mobile_id_type* guti, emm_ctrl_data_type     *emm_ctrl_data_ptr);
boolean mm_is_ptmsi_valid(void);
void emm_populate_imsi(lte_nas_emm_eps_mobile_id_type* imsi,emm_ctrl_data_type     *emm_ctrl_data_ptr );
void gmm_multimode_set_state(gmm_state_type gmm_state1, gmm_substate_type gmm_sub_state1);
void mm_set_plmn_search_in_progress_flag(boolean value);

void emm_set_state_multimode_registration_success(boolean is_attach);
void emm_set_state_multimode_attach_failure(lte_nas_emm_cause_type cause);
void emm_set_radio_cap_update_state(uint32  additional_info);
void emm_set_state_multimode_rau_failure(lte_nas_emm_cause_type cause);
void emm_activate_lte(void); 
void emm_multimode_detach(void); 
void emm_set_update_status_rau_abnormal_failure(emm_eps_update_status_type update_type);
void emm_set_emm_param_attach_abnormal_failure(void);
void mm_set_update_status_tau_abnormal_failure(gmm_update_status_type update_type);
void mm_set_gmm_param_attach_abnormal_failure(void);
/*===========================================================================

FUNCTION  mm_is_tmsi_valid

DESCRIPTION
           TMSI validity check
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_tmsi_valid(byte *tmsi, word tmsi_length);

/*===========================================================================

FUNCTION  MM_UPDATE_CS_UPDATE_SUCCESS_STATUS

DESCRIPTION
  This function updates MM state and parameters when combined attach or 
  combined TAU is successfull.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_cs_update_success_status
(
  lte_nas_emm_incoming_msg_type  *msg_ptr
);

/*===========================================================================

FUNCTION MM_INVALIDATE_LU_INFO

DESCRIPTION
  This function is wrapper function of mm_delete_lu_status for EMM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  mm_invalidate_lu_info(byte update_status);

/*===========================================================================

FUNCTION  MM_UPDATE_CS_REJECT_INVALID_IMSI_STATUS

DESCRIPTION
  This function updates MM state and parameters when combined Attach/TAU is
  successful for EPS only because of IMSI unknown in HSS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_cs_reject_invalid_imsi_status(void);

/*===========================================================================
FUNCTION    EMM_SEND_MULTIMODE_RRC_SYSTEM_UPDATE_FROM_GW

DESCRIPTION
  This function will send LTE RRC SYSTEM UPDATE REQ to LTE RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_multimode_rrc_system_update_from_GW
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_GET_GMM_REG_LTE_CS_PARAM

DESCRIPTION
  This function returns gmm_reg_lte_cs_param

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
sys_lte_cs_domain_param_s_type emm_get_gmm_reg_lte_cs_param
(
  void
);


/*===========================================================================

FUNCTION  EMM SERVICE LOSS ON LTE

DESCRIPTION
  This function sets the global variable service_loss_on_multimode_rat to LTE rat.
  This function is called whenever there is loss of service in LTE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_service_loss_on_lte
(
  void
);

/*===========================================================================

FUNCTION  EMM SERVICE ACQUIRED ON LTE

DESCRIPTION
  This function sends rat change request to sm and resets the loss on service global variables
  of lte and gw

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_service_acquired_on_lte
(  
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  MM_MAP_PTMSI_RAI_TO_GUMMEI_INFO 

DESCRIPTION
  Functions maps P-TMSI and RAI to GUMMEI info - MCC + MNC + MMEGI + MMEC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  mm_map_ptmsi_rai_to_gumme_info
(
  lte_nas_emm_guti_type  *guti_ptr
);

/*===========================================================================

FUNCTION  MM_IS_RAI_VALID

DESCRIPTION
  This functions validates RAI

DEPENDENCIES
  None

RETURN VALUE
  TRUE: RAI is valid
  FALSE: RAI is invalid

SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_is_rai_valid(void);

extern void emm_get_mapped_gw_identity(mm_gw_identity_param_type *mm_identity, 
                                  emm_ctrl_data_type  *emm_ctrl_data_ptr);

extern void mm_get_native_gw_identity
(
  mm_gw_identity_param_type *gw_identity,
  auth_security_context_id cksn_context
);

extern void emm_reset_attempt_counter(void);
extern void mm_csfb_start_mo_call_orig(mm_cmd_type *msg_p);
extern void mm_csfb_send_paging_response( void );
extern void mm_csfb_set_state(mm_csfb_state_type new_csfb_state);
extern void mm_reset_mt_csfb_cco_state(void);
extern boolean mm_csfb_process_mt_page(lte_nas_paging_identity_type ue_page_id);
extern void mm_csfb_rej_pending_call(void);
extern void mm_csfb_clear_csfb_call(void);
extern mm_csfb_state_type mm_csfb_get_state(void);

void mm_set_emm_manual_selection_flag(boolean value);
void emm_set_mm_manual_selection_flag(boolean value);

void emm_build_and_send_esr_failure_ind (reject_type_enum_T  reject_type, 
                                         reject_cause_T      reject_cause,
                                         dword               trans_id);

/*===========================================================================
FUNCTION    emm_lte_cs_domain_param_match

DESCRIPTION
  This function compares lte_cs_domain_param.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if match is successful, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_lte_cs_domain_param_match
(
  const sys_lte_cs_domain_param_s_type lte_cs_domain_param_from_reg, 
  const sys_lte_cs_domain_param_s_type lte_cs_domain_param_in_emm
);


/*===========================================================================
 
FUNCTION  EMM_UPDATE_MULTIMODE_SR_REJECT

DESCRIPTION
  This function updates EMM state and parameters upon GW serivice reject

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_update_multimode_sr_reject
(
  lte_nas_emm_cause_type  emm_cause
);

/*=========================================================================== 
 
FUNCTION  EMM_UPATE_MULTIMODE_MT_DETACH

DESCRIPTION
  This function updates EMM state and parameters upon GW MT DETACH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
extern void emm_update_multimode_mt_detach
(
  lte_nas_emm_cause_type  emm_cause
);

/*===========================================================================

FUNCTION  EMM_MULTIMODE_COMPLETE_UPDATE

DESCRIPTION
  This function is called to complete EMM status update upon GW ATTACH/RAU/SR/
  MT DETACH handling

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_multimode_complete_update(void);

void mm_update_start_tau_procedure(lte_nas_emm_eps_update_val_type   ta_update_type);

extern void mm_handle_timer_t3412_expiry(emm_ctrl_data_type *emm_ctrl_data_ptr);

/*===========================================================================

FUNCTION  EMM_ATTACH_TAU_SUCCESS_UPDATE_GW

DESCRIPTION
  This function updates GW flags upon successful TAU & ATTACH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_attach_tau_success_update_gw(void);

/*===========================================================================

FUNCTION  emm_set_mm_ps_detach_state

DESCRIPTION: This function is used by EMM to set MM PS DETACH STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_mm_ps_detach_state
(
  mm_ps_detach_state_enum_T  state
);

/*===========================================================================

FUNCTION  emm_set_lte_deact_isr_timer_expired

DESCRIPTION: This function is used by EMM to set "gmm_lte_deact_isr_timer_expired"

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_lte_deact_isr_timer_expired
(
  boolean expired_status
);

#ifdef FEATURE_WRLF_SYSTEM_SEL
void  mm_send_rrc_abort_req_during_wrlf(void);
#endif

/*===========================================================================

FUNCTION  emm_map_guti_to_rai

DESCRIPTION
  Maps RAI to GUTI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_map_guti_to_rai(routing_area_id_T *ra_id, emm_ctrl_data_type *emm_ctrl_data_ptr);


/*===========================================================================

FUNCTION  emm_set_gmm_drx_cn_coff_s1

DESCRIPTION: This function is used by EMM to set "gmm_drx_cn_coeff_s1"

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_gmm_drx_cn_coff_s1(sys_drx_cn_coefficient_s1_e_type drx_value);

/*===========================================================================

FUNCTION  emm_get_gmm_drx_cn_coff_s1

DESCRIPTION: This function is used by EMM to get "gmm_drx_cn_coeff_s1"

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
sys_drx_cn_coefficient_s1_e_type emm_get_gmm_drx_cn_coff_s1(void);
#ifdef FEATURE_LTE_REL9
/*===========================================================================

FUNCTION  mm_emm_is_mmit_feature_enabled

DESCRIPTION: 
  This function checks if mobility management for IMS voice Termination(MMIT) is enabled.


DEPENDENCIES
  None

RETURN VALUE
  Return boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_emm_is_mmit_enabled(void);

/*===========================================================================

FUNCTION  mm_emm_is_reg_needed_for_mmit_on_LTE

DESCRIPTION: 
  This function checks if TAU on LTE is needed to be  performed for 
  mobility management for IMS voice Termination(MMIT).
  This function uses "mm_emm_mmit_eval_on_irat_between_lte_and_umts" and
  "mm_emm_mmit_eval_on_irat_between_lte_and_gsm" functions to dervie the outcome.

DEPENDENCIES
  None

RETURN VALUE
  Return boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean mm_is_reg_needed_for_mmit_on_LTE(sys_radio_access_tech_e_type previous_rat);




/*===========================================================================

FUNCTION  mm_emm_is_reg_needed_for_mmit_on_GW

DESCRIPTION: 
  This function checks if RAU on GSM/UMTS is needed to be  performed for 
  mobility management for IMS voice Termination(MMIT).
  This function uses "mm_emm_mmit_eval_on_irat_between_lte_and_umts" and
  "mm_emm_mmit_eval_on_irat_between_lte_and_gsm" functions to dervie the outcome.

DEPENDENCIES
  None

RETURN VALUE
  Return boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean mm_is_reg_needed_for_mmit_on_GW(void);



#endif


/*===========================================================================

FUNCTION  emm_set_gmm_manual_selection_flag

DESCRIPTION: This function is used by EMM to set GMM manual selection flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void emm_set_gmm_manual_selection_flag(boolean value);

#ifdef FEATURE_LTE_REL9
/*===========================================================================

FUNCTION  GMM_HANDLE_EMC_PS_SRV

DESCRIPTION: This function handles the emergency bearer service established 
             on LTE in L to G/W/TD RAT change scenarios

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  gmm_handle_emc_ps_srv
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);
#endif

/*===========================================================================

FUNCTION  gmm_invalidate_parameters

DESCRIPTION: A function to invalidate P-TMSI, P-TMSI signature, RAI and GPRS ciphering key sequence number. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_invalidate_parameters(void);

/*===========================================================================

FUNCTION  mm_is_ue_camped_on_gw

DESCRIPTION: This function returns TRUE if UE is camped for full service on GW 

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_is_ue_camped_on_gw(void);


#ifdef FEATURE_1XSRLTE
/*===========================================================================

FUNCTION  emm_get_current_ue_mode

DESCRIPTION: 
 
This function returns current UE mode 
  
DEPENDENCIES:
  None

RETURN VALUE
  Return sys_ue_mode_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_ue_mode_e_type emm_get_current_ue_mode(void);
#endif



#endif /* FEATURE_LTE*/

#ifdef FEATURE_SGLTE
extern void mmcoord_process_spliting(void);
extern void mmsend_mmr_service_stat_ind( mm_as_id_e_type mm_as_id_in);
extern mm_as_id_e_type mm_get_cs_stack_id(void);
#endif

#ifdef FEATURE_DUAL_SIM
void mm_abort_service_request_if_needed(void);
#endif

#ifdef FEATURE_DUAL_DATA
extern void emm_send_rr_ps_resume_ind(emm_ctrl_data_type  *emm_ctrl_data_ptr);
extern void emm_send_data_priority_on_other_sub(sys_data_priority_e_type priority);

#endif


#endif /* #ifndef MM_MULTIMODE_COMMON_H */
