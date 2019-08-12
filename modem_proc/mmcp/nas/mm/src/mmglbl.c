/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmglbl.c_v   1.24   17 Jul 2002 13:06:06   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmglbl.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/14   gps     Created New EFS item Ss_Screening_Indicator
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/08/01   jca     Added/deleted some regional variables.
08/09/01   jc      Add log initialization.
08/15/01   ab      Commented out diag.h and log_config to get latest MM to build
                   in VST.  Renamed CM to CNM.
09/18/01   sbk     Added support for using NVMEM
09/21/01   sbk     Removed unused and unnecessary code
10/19/01   sbk     Removed hardcoded ue capability value.
04/15/02   ks      Setting the mm_sig_low_priority & nreg_release_pending
                   values to default values "FALSE"
04/23/02   jca     Deleted mm_cnm_active_transaction. Added mm_cnm_rel_req_received.
06/05/02   ks      Added support for Dual mode operation.
06/26/02   ks      Setting the MSCR field to default value in
                   mm_system_information system variable.
07/03/02   jca     Deleted mm_plmn_list_state.
07/12/02   ks      Now returning the ACTIVE_RAT information rather than
                   the Preferred RAT.
07/17/02   ks      Setting the mm_gsm_supported and mm_wcdma_supported.
07/22/02   ks      Added mm_rat_change_pending & mm_serv_change_pending to support
                   Dynamic RAT and Service change.
08/09/02   jca     Now initializing mm_pending_lu_request.
08/09/02   mks     Added mm_hi_access_class & mm_lo_access_class to cache UE access
                   class information.
08/28/02   cd      Added initialization of mm_service_plmn.mm_service_state
10/07/02   jca     Changes to support CS/PS combined attach procedures.
10/30/02   mks     Added placeholder for CM service establishment cause
11/08/02   cd      Added access_class_restriction to mm_system_information
11/14/02   ks      Added SGSNR info to mm_system_information
11/14/02   ks      Added definition for mm_deep_sleep_pending and setting it
                   to default value.
11/14/02   ks      Changed the default value for mm_sim_valid to TRUE, this
                   is being set to FALSE if gsdi_get_card_mode fails.
11/25/02   mks     Reverted the default value for mm_sim_valid to FALSE. The flag
                   will be set to TRUE if gsdi_get_card_mode returns success
                   upon receiving SIM inserted event.
12/12/02   ks      Replaced mm_sim_valid with mm_serving_plmn.info.sim_state.
12/16/02   jca     Renamed mm_waiting_to_send_reg_cnf to mm_ready_to_send_reg_cnf.
12/17/02   cd      Changed initialization of SIM state to SYS_SIM_STATE_NOT_AVAILABLE
02/07/03   jca     Added boolean mm_dual_mode.
02/13/03   jca     Added boolean mm_waiting_for_service_cnf.
02/21/03   vdr     Added support for SMS Bearer Selection feature
03/07/03   mks     Initialize the ps_data_suspend flag to FALSE in mm_serving_plmn.info
04/03/03   cd      Updated initialization of mm_serving_plmn structure to account for
                   the equivalent plmn list
08/28/03   ks      Reading NV_FORCE_UE_SGSNR_R99_I and NV_FORCE_UE_MSCRR_R99_I
                   from NV during initialization.
09/10/03   ks      re-initializing NV_FORCE_UE_SGSNR_R99_I and NV_FORCE_UE_MSCRR_R99_I.
09/15/03   cd      Added get_preferred_band_class() and get_active_band()
01/16/03   ks      Removed the FEATURE_GSM_GPRS around FORCE_SGSNR_99 and
                   FORCE_MSCR_R99, as this was causing problem in setting the
                   right value in case of single mode operation.
01/28/04   jca     Added regional variables mm_previous_serving_lai and
                   mm_previous_idle_substate.
04/06/04   cd      Added initialization of global mm_rat_handler_status
04/06/04   cd      LCS support
05/07/04   ks      Replaced the data type from boolean to ue_force_rel_mode_type
                   for variables mm_nv_force_sgsnr_r99 & mm_nv_force_mscr_r99
05/12/04   cd      Added support for WCDMA multiband feature
                   Fixed lint errors.
                   Removed obsolete function mm_get_active_rat()
05/14/04   cd      Enhancements to CS FOR
06/01/04   jca     Added extern boolean mm_needs_to_send_mmr_service_ind.
06/08/04   cd      LCS support
07/21/04   ks      Now setting the UE revision level to R99 by default.
07/30/04   jca     Added regional variable mm_band_pref.
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
08/18/04   cd      Setting the default revision level to R99
09/17/04   cd      Corrected type in ERR msg reporting setting the MSCR to R99
04/14/05   jca     Added boolean flag mm_waiting_for_stop_mode_cnf.
04/20/05   jca     Added boolean flag mm_waiting_for_deact_cnf.
05/30/05   abe     Added boolean flag mm_sms_is_active, which is used to release
                   the PS session after getting the MMCNM_REL_REQ
06/06/05   abe     Added code for CCBS support
07/20/05   abe     Declared and initialized the variable mm_previous_lai_during_wtog_cco
                   for FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER.
08/08/05   abe     Definition for the variable mm_hplmn_from_imsi
                   is removed for the feature FEATURE_ENHANCED_NW_SELECTION.
08/14/05   hj      Added handling for monitoring CCCH paging channel when detached from
                   GPRS Services.
09/09/05   ajt     Added definition of mm_prev_cell_access  
10/08/05   abe     Added support for FEATURE_INTERRAT_PCCO_GTOW                  
01/18/06    up      Use NV_REL_MODE_MAX instead of NV_FORCE_UE_DYNAMIC for
                    MAX value checking.
01/30/06   hj      Added support for CS_ONLY and PS_ONLY routing options for SMS.
04/06/06   sn      Updated mm_initialize to initialise DTM status.
06/07/06   ss       Added definition for is_WtoGCCO_in_prgress
06/23/06   up      mm_gs_association_established is added.
07/14/06   pk    Updated for Feature DSAC
03/21/07   NR      Removing mm_cnm_service_type global variable
05/21/07   NR      Adding new global for storing call_type which is to be sent in RRC_EST_REQ  
26/12/07   jbk     Setting Default NAS release compliance behavior to Rel5 
12/18/09   RI      Updates for combined EFS/NV/USIM reads for multimode.
07/05/11   rajesh  Defined and initialised "mm_service_ind_blocked_due_to_cs_reg"
08/03/11   jbk      NV support to disable CB duplicate detection.
08/23/11   PM      Defined and set value from EFS for global variable preserve_ui_speech_codec_preference
10/01/11   abhi  resetting CSFB state
10/04/11   sla     Foreground search list LAI,RAC are initialized
10/12/11   sla     Defined the variable mm_is_foreground_search_to_be_initiated
                   and initialized to zero
10/28/11   rajesh  defined "mm_lai_change_force_lau_for_emergency".
12/06/11   gps     Remembering DSAC information of the cell where last registration was attempted
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "aconfig.h"
#include "mmrat.h"
#include "mm_v.h"
#include "mm_as_types.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "err.h"
#include "naslog_v.h"
#include "ghdi_nvmem.h"
#include "ghdi_nvmem_v.h"
#include "mm_coord.h"
#include "gmmllcm.h"
#include "fs_lib.h"
#include "cm.h"
#include "mcfg_fs.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#include "mmsecurity.h"
#include "mmauth.h"
#include "gmm_msg.h"
#include "reg_mm.h"
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "atomic_ops.h"
#endif
#include "mm_sim.h"
#include "dog_hb_rex.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
boolean full_srv_req_in_emerg;
boolean emergency_call_after_lu_in_lpm;
/* putting out of dual sim so that can be used for single sim as well  */
byte             mm_msg_info;
ghdi_nvmem_status_T mm_nvmem_default_values_generator(
       ghdi_nvmem_status_T in_status, nv_items_enum_type item_type, nv_item_type *nv_item);

#ifdef FEATURE_SGLTE
typedef struct forbidden_list_tag
{
   inter_task_lai_T        lai[MAX_NO_FORBIDDEN_LAIS];
   byte                    write_ptr;
} forbidden_list_T;
void extern mm_copy_auth_cs_database(mm_as_id_e_type dest,
                                                   mm_as_id_e_type src);
extern boolean mm_is_cs_timer(timer_id_T timer_id);

extern mm_cmd_type mmcoord_pending_reg_message_sim[MAX_NAS_STACKS];
extern mm_cmd_type mmcoord_pending_reg_list_req_message_sim[MAX_NAS_STACKS];
extern mm_cmd_type mmcoord_pending_svc_cnf_message_sim[MAX_NAS_STACKS];
extern byte auth_gsm_cksn_sim[MAX_NAS_STACKS];
extern boolean mmcoord_reg_req_pending_sim[MAX_NAS_STACKS];
extern boolean mm_plmn_list_req_pending_sim[MAX_NAS_STACKS];
extern boolean mm_plmn_search_in_progress_sim[MAX_NAS_STACKS];
extern boolean pending_mm_msg_after_release_sim[MAX_NAS_STACKS];
extern boolean mm_cs_domain_possible_reest_sim[MAX_NAS_STACKS];
extern boolean pending_mm_msg_valid_sim[MAX_NAS_STACKS];
extern boolean send_sequence_number_mm_cc_ss_sim[MAX_NAS_STACKS];
extern forbidden_list_T forbidden_list_sim[MAX_NAS_STACKS];
extern lai_reject_list_T lai_reject_list_sim[MAX_NAS_STACKS];
extern byte lu_cause_sim[MAX_NAS_STACKS];
boolean mmcoord_svc_cnf_pending_sim[MAX_NAS_STACKS];
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
#ifdef FEATURE_MODEM_HEAP
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr_sim[MAX_NAS_STACKS];
#define mm_wrlf_serving_plmn_ptr mm_wrlf_serving_plmn_ptr_sim[mm_as_id]
#else
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr;
#endif
#endif
#endif
/*
 * List of global variables
 */
#ifdef FEATURE_SGLTE
mm_sglte_stack_activity_status mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
#endif
#if defined(FEATURE_LTE) && defined(FEATURE_DUAL_SIM)
boolean mm_suppress_additional_update_param_sim[MAX_AS_IDS];
#define mm_suppress_additional_update_param mm_suppress_additional_update_param_sim[mm_sub_id]
#else
boolean mm_suppress_additional_update_param = FALSE;
#endif

#ifdef FEATURE_DUAL_SIM
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,mm_nv_context_id)
#else
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#endif

#ifdef FEATURE_MODEM_HEAP
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean pending_mm_msg_valid_sim[MAX_NAS_STACKS];
#else
extern boolean pending_mm_msg_valid;
#endif
#endif

/* mm_as_id is used stack Id*/
mm_as_id_e_type mm_as_id = MM_AS_ID_1;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
mm_as_id_e_type mm_cs_stack_id = MM_AS_ID_1;
mm_as_id_e_type mm_ps_stack_id = MM_AS_ID_1;

byte                      mm_state_sim[MAX_NAS_STACKS];
byte                      mm_idle_substate_sim[MAX_NAS_STACKS];
IE_info_T                 mm_IE_info_sim[MAX_NAS_STACKS][MAXNO_IES_IN_MESSAGE];
byte                      mm_no_of_ies_sim[MAX_NAS_STACKS];
mm_system_information_T   mm_system_information_sim[MAX_NAS_STACKS];
mm_last_reg_dsac_information_T mm_last_reg_dsac_information_sim[MAX_NAS_STACKS];
boolean                   mm_llc_needs_to_be_resumed_sim[MAX_NAS_STACKS];
boolean                   mm_nreg_req_received_sim[MAX_NAS_STACKS];
boolean                   mm_cnm_rel_req_received_sim[MAX_NAS_STACKS];
boolean                   mm_present_all_plmns_sim[MAX_NAS_STACKS];
location_information_T    mm_location_information_sim[MAX_NAS_STACKS];
held_service_request_T    mm_held_cnm_service_req_sim[MAX_NAS_STACKS];
boolean                   mm_reg_waiting_for_reg_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_ready_to_send_reg_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_needs_to_send_mmr_service_ind_sim[MAX_NAS_STACKS];
boolean                   mm_waiting_for_service_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_waiting_for_stop_mode_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_waiting_for_deact_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_waiting_for_ph_status_change_cnf_sim[MAX_NAS_STACKS];
sys_radio_access_tech_e_type mm_last_active_rat_stopped_sim[MAX_NAS_STACKS];
lu_reject_information_T   mm_lu_reject_info_sim[MAX_NAS_STACKS];
byte                      mm_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];
byte                      mm_lu_attempt_counter_sim[MAX_NAS_STACKS];
byte                      mm_pending_lu_request_sim[MAX_NAS_STACKS];
byte                      mm_pending_no_service_to_reg_sim[MAX_NAS_STACKS];
byte                      mm_lu_start_reason_sim[MAX_NAS_STACKS];
byte                      mm_cipher_key_sequence_number_sim[MAX_NAS_STACKS];
boolean                   mm_follow_on_request_sim[MAX_NAS_STACKS];
boolean                   mm_follow_on_proceed_sim[MAX_NAS_STACKS];
byte                      mm_reject_cause_sim[MAX_NAS_STACKS];
boolean                   mm_cnm_is_active_sim[MAX_NAS_STACKS];
boolean                   mm_sms_is_active_sim[MAX_NAS_STACKS];
boolean                   mm_T3213_timed_out_sim[MAX_NAS_STACKS];
byte                      mm_idle_transition_timer_sim[MAX_NAS_STACKS];
byte                      mm_timer_status_sim[MAX_NAS_STACKS][MAX_MM_TIMER+1];
boolean                   mm_possible_imsi_attach_sim[MAX_NAS_STACKS];
imsi_data_T               mm_stored_imsi_sim[MAX_NAS_STACKS];
ef_kc_T                   mm_ef_kc_sim[MAX_NAS_STACKS];
boolean                   mm_ef_kc_valid_sim[MAX_NAS_STACKS];
uint32                    mm_op_mode_sim[MAX_NAS_STACKS];
boolean                   mm_sig_low_priority_sim[MAX_NAS_STACKS];
boolean                   mm_nreg_release_pending_sim[MAX_NAS_STACKS];
boolean                   mm_integrity_protected_sim[MAX_NAS_STACKS];
serving_plmn_info_T       mm_serving_plmn_sim[MAX_NAS_STACKS];
boolean                   mm_gsm_supported;
boolean                   mm_wcdma_supported;
boolean                   mm_tdscdma_supported;

sys_radio_access_tech_e_type  mm_ue_pref_rat_sim[MAX_NAS_STACKS];
boolean                   mm_rat_change_pending_sim[MAX_NAS_STACKS];
boolean                   mm_serv_change_pending_sim[MAX_NAS_STACKS];
boolean                   mm_to_only_reg_sim[MAX_NAS_STACKS];
sys_radio_access_tech_e_type mm_previous_serving_rat_sim[MAX_NAS_STACKS];
boolean                   mm_dual_mode_sim[MAX_NAS_STACKS];
sys_band_mask_type        mm_band_pref_sim[MAX_NAS_STACKS];
establishment_cause_T     mm_cnm_srv_est_cause_sim[MAX_NAS_STACKS];

rrc_lo_access_class_e_type mm_lo_access_class_sim[MAX_NAS_STACKS];
rrc_hi_access_class_type   mm_hi_access_class_sim[MAX_NAS_STACKS];
boolean                    mm_deep_sleep_pending_sim[MAX_NAS_STACKS] ;
boolean                   mm_hard_abort_pending_sim[MAX_NAS_STACKS];
boolean                   mm_hard_abort_ue_sglte_mode_switch_pending_sim[MAX_NAS_STACKS];
boolean                   mm_deep_sleep_alt_scan_sim[MAX_NAS_STACKS];
mm_connection_type_T      mm_connection_type_sim[MAX_NAS_STACKS];

mm_pended_stop_mode_req_s_type mm_pended_stop_mode_req_sim[MAX_NAS_STACKS];
#define mm_pended_stop_mode_req mm_pended_stop_mode_req_sim[mm_as_id]

boolean                    mm_waiting_for_csmt_response_sim[MAX_NAS_STACKS];


#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
sys_band_mask_type         mm_ue_band_class_pref_sim[MAX_NAS_STACKS];
sys_band_mask_e_type       mm_active_band_sim[MAX_NAS_STACKS];
#endif

sys_band_class_e_type     mm_band_indicator_sim[MAX_NAS_STACKS];
byte                      mm_previous_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];

byte                      mm_gstk_previous_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];

byte                      mm_previous_idle_substate_sim[MAX_NAS_STACKS];
mm_as_cell_access_status_T mm_prev_cell_access_sim[MAX_NAS_STACKS];

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
boolean           mm_lcs_active_sim[MAX_NAS_STACKS];
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_CCBS
boolean           mm_supports_NIMO_CM_connection_sim[MAX_NAS_STACKS];
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
WtoGCCO_in_progress_enum_T  WtoGCCO_state_sim[MAX_NAS_STACKS];
#endif

#ifdef FEATURE_INTERRAT_PCCO_GTOW
WtoGCCO_in_progress_enum_T  GtoWCCO_state_sim[MAX_NAS_STACKS];
#endif

mm_rat_handler_status_type mm_rat_handler_status_sim[MAX_NAS_STACKS];

/* Used to remember if combine procedure is performed last to make MSC-GS association */
boolean mm_gs_association_established_sim[MAX_NAS_STACKS];
boolean mm_initiate_lu_upon_entry_into_new_cell_sim[MAX_NAS_STACKS];
uint32 mm_serving_cell_id_sim[MAX_NAS_STACKS];
boolean mm_serving_cell_valid_sim[MAX_NAS_STACKS];
uint32 mm_lu_failed_cell_id_sim[MAX_NAS_STACKS];

#ifdef FEATURE_DSAC
boolean                    mm_ps_access_barred_to_unbarred_sim[MAX_NAS_STACKS];
boolean                    mm_cs_access_barred_to_unbarred_sim[MAX_NAS_STACKS];
byte                       mm_last_procedure_to_update_registration_sim[MAX_NAS_STACKS];

#define  mm_ps_access_barred_to_unbarred (mm_ps_access_barred_to_unbarred_sim[mm_as_id])
#define  mm_cs_access_barred_to_unbarred (mm_cs_access_barred_to_unbarred_sim[mm_as_id])
#define  mm_last_procedure_to_update_registration (mm_last_procedure_to_update_registration_sim[mm_as_id])
#endif 

boolean                 mm_manual_mode_user_selection_procedure_sim[MAX_NAS_STACKS];

#ifdef FEATURE_ENHANCED_NW_SELECTION
boolean                    mm_force_lu_sim[MAX_NAS_STACKS];
dword                      mm_foreground_search_timer_value_sim[MAX_NAS_STACKS];
mm_lai_timer_info_list_info_T mm_foreground_search_lai_timer_list_sim[MAX_NAS_STACKS];
#endif

boolean                    mm_managed_roaming_enabled_sim[MAX_AS_IDS];

/******            mm_controls_resel_dec_mt_cs_call      ********
 *  In case of CS MT call, this flag indicates to MM if MM is in 
 *  control of allowing/disallowing Reselection when asked by RRC
 *  */
byte mm_controls_resel_dec_mt_cs_call_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define mm_controls_resel_dec_mt_cs_call mm_controls_resel_dec_mt_cs_call_sim[mm_as_id]

/* defined in ms_cc_mm.h file this should be in synchronous with the ones defined in RRC 
   used for storing call_type sent by CC, which should be passed in RRC_EST_REQ for CS Domain */
call_type_for_rrc_est_req_T call_type_for_rrc_est_req_sim[MAX_NAS_STACKS];
mm_attach_failed_status_type mm_mmr_attach_ind_status_sim[MAX_NAS_STACKS];

/* DUAL SIM : Flag if HARD ABORT is pending for DUAL SWITCH */
sys_stop_mode_reason_e_type                   mm_abort_dual_switch_pending_sim[MAX_NAS_STACKS];

#ifdef FEATURE_TRIPLE_SIM
uint8 mm_nv_context_id_sim[MAX_AS_IDS] = {MM_AS_ID_1, MM_AS_ID_2, MM_AS_ID_3};
#else
uint8 mm_nv_context_id_sim[MAX_AS_IDS] = {MM_AS_ID_1, MM_AS_ID_2};
#endif

#define mm_nv_context_id (mm_nv_context_id_sim[mm_sub_id])

boolean      mm_mmr_cell_service_ind_pending_sim[MAX_NAS_STACKS];

#ifdef FEATURE_RAT_PRIORITY_LIST
sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_sim[MAX_NAS_STACKS];
#define mm_rat_pri_list_info mm_rat_pri_list_info_sim[mm_as_id]

#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_to_nw_sim[MAX_NAS_STACKS];
#define mm_rat_pri_list_info_to_nw mm_rat_pri_list_info_to_nw_sim[mm_as_id]
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */

/* Flag to indicate whether IMSI stored in NV is same as the 
   one in USIM/SIM 
*/
boolean      mm_imsi_is_native_sim[MAX_NAS_STACKS];
#define      mm_imsi_is_native mm_imsi_is_native_sim[mm_as_id]


/* Flag to indicate whether GSM access is allowed or not */
boolean      auth_gsm_access_sim[MAX_NAS_STACKS];
#define      auth_gsm_access auth_gsm_access_sim[mm_as_id]

/* SIM/USIM access class information to be sent to RR/RRC */
byte         mm_sim_access_class_sim[MAX_NAS_STACKS][MM_SIM_EFACC_SIZE];
#define      mm_sim_access_class mm_sim_access_class_sim[mm_as_id]

/*LU reject cause for each failure attempt */
byte         mm_lu_reject_cause_sim[MAX_NAS_STACKS][MAX_LU_ATTEMPT_SIZE];
#define      mm_lu_reject_cause mm_lu_reject_cause_sim[mm_as_id]

/* GSM security context for USIM */
boolean      auth_usim_gsm_security_context_sim[MAX_NAS_STACKS] ;
#define      auth_usim_gsm_security_context auth_usim_gsm_security_context_sim[mm_as_id]


/* Mobile operation mode */
byte         mm_ms_opmode_sim[MAX_NAS_STACKS];
#define      mm_ms_opmode mm_ms_opmode_sim[mm_as_id]

/* 2 or 3 digit MNC */
byte         mm_mnc_length_sim[MAX_NAS_STACKS];
#define      mm_mnc_length mm_mnc_length_sim[mm_as_id]

/* Flag to remember last network selection mode send to RR/RRC */
sys_network_selection_mode_e_type last_requested_network_selection_mode_sim[MAX_NAS_STACKS];
#define      last_requested_network_selection_mode last_requested_network_selection_mode_sim[mm_as_id]

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
byte mm_pending_lu_type_sim[MAX_NAS_STACKS];
#define mm_pending_lu_type mm_pending_lu_type_sim[mm_as_id]
#endif

mmr_cause_e_type mm_reg_cause_sim[MAX_NAS_STACKS];

#define mm_connection_type   mm_connection_type_sim[mm_as_id]

#ifdef FEATURE_TC
boolean mm_tc_is_active_sim[MAX_NAS_STACKS];
#define mm_tc_is_active mm_tc_is_active_sim[mm_as_id]
#endif

boolean mm_service_ind_blocked_due_to_cs_reg_sim[MAX_NAS_STACKS];
#define mm_service_ind_blocked_due_to_cs_reg mm_service_ind_blocked_due_to_cs_reg_sim[mm_as_id]

boolean mm_service_ind_blocked_due_to_irat_cs_reg_sim[MAX_NAS_STACKS];
#define mm_service_ind_blocked_due_to_irat_cs_reg mm_service_ind_blocked_due_to_irat_cs_reg_sim[mm_as_id]

boolean mm_connection_activated_sim[MAX_NAS_STACKS]; 
#define mm_connection_activated mm_connection_activated_sim[mm_as_id]

mm_csfb_service_ind_status_enum_T mm_csfb_service_ind_status_sim[MAX_NAS_STACKS];
#define mm_csfb_service_ind_status mm_csfb_service_ind_status_sim[mm_as_id]

boolean mm_is_foreground_search_to_be_initiated_sim[MAX_NAS_STACKS];
#define mm_is_foreground_search_to_be_initiated mm_is_foreground_search_to_be_initiated_sim[mm_as_id]

boolean mm_is_gmm_moved_dereg_due_to_csreg_sim[MAX_NAS_STACKS];
#define mm_is_gmm_moved_dereg_due_to_csreg  mm_is_gmm_moved_dereg_due_to_csreg_sim[mm_as_id]

#ifdef FEATURE_NAS_REL10
sys_plmn_id_s_type mm_3gpp_t3246_plmn_id_sim[MAX_NAS_STACKS];
#define mm_3gpp_t3246_plmn_id mm_3gpp_t3246_plmn_id_sim[mm_as_id]

boolean mm_store_integrity_protected_sim[MAX_NAS_STACKS];
#define mm_store_integrity_protected mm_store_integrity_protected_sim[mm_as_id]

dword   mm_t3246_backoff_timer_value_sim[MAX_NAS_STACKS];
#define mm_t3246_backoff_timer_value  mm_t3246_backoff_timer_value_sim[mm_as_id]
#endif

#ifdef FEATURE_LTE
boolean mm_csfb_is_blind_redir_to_lte_req_to_be_sent_sim[MAX_NAS_STACKS];
#define mm_csfb_is_blind_redir_to_lte_req_to_be_sent mm_csfb_is_blind_redir_to_lte_req_to_be_sent_sim[mm_as_id]

boolean mm_stop_mode_local_detach_peding_sim[MAX_NAS_STACKS];
#define mm_stop_mode_local_detach_peding mm_stop_mode_local_detach_peding_sim[mm_as_id]

boolean mm_stop_mode_local_detach_peding_sim[MAX_NAS_STACKS];
#define mm_stop_mode_local_detach_peding mm_stop_mode_local_detach_peding_sim[mm_as_id]

sys_ps_detach_e_type mm_ps_detach_type_sim[MAX_NAS_STACKS];
#define mm_ps_detach_type mm_ps_detach_type_sim[mm_as_id]
#endif /* FEATURE_LTE */


#ifdef FEATURE_NAS_REL10
boolean mm_is_per_ms_t3212_present_sim[MAX_NAS_STACKS];
#define mm_is_per_ms_t3212_present  mm_is_per_ms_t3212_present_sim[mm_as_id]

dword   mm_stored_per_ms_t3212_value_sim[MAX_NAS_STACKS];
#define mm_stored_per_ms_t3212_value mm_stored_per_ms_t3212_value_sim[mm_as_id]
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
boolean                   mm_received_suitable_search_end_ind_sim[MAX_NAS_STACKS];
#define mm_received_suitable_search_end_ind mm_received_suitable_search_end_ind_sim[mm_as_id]
#endif

word mm_coord_camped_cell_id_sim[MAX_NAS_STACKS];

#ifdef FEATURE_NAS_REL11
boolean  mm_is_first_rrc_connection_attempt_sim[MAX_AS_IDS];
#define  mm_is_first_rrc_connection_attempt mm_is_first_rrc_connection_attempt_sim[mm_sub_id]
#endif

boolean  mm_is_cs_connected_due_to_srvcc_sim[MAX_NAS_STACKS];
#define  mm_is_cs_connected_due_to_srvcc mm_is_cs_connected_due_to_srvcc_sim[mm_as_id]

#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
boolean mm_is_lu_to_be_initiated_t3346_ltogw_sim[MAX_NAS_STACKS];
#define mm_is_lu_to_be_initiated_t3346_ltogw mm_is_lu_to_be_initiated_t3346_ltogw_sim[mm_as_id]
#endif

boolean  mm_psm_ready_req_rejected_sim[MAX_NAS_STACKS];
#define  mm_psm_ready_req_rejected mm_psm_ready_req_rejected_sim[mm_as_id]

byte mm_plmn_search_transaction_id_sim[MAX_NAS_STACKS];
#define mm_plmn_search_transaction_id mm_plmn_search_transaction_id_sim[mm_as_id]

#ifdef FEATURE_DUAL_DATA
boolean  mm_is_volte_only_ps_sim[MAX_NAS_STACKS];
#define  mm_is_volte_only_ps mm_is_volte_only_ps_sim[mm_as_id]
#endif

boolean mm_reject_call_on_rai_change_sim[MAX_NAS_STACKS];
#define mm_reject_call_on_rai_change mm_reject_call_on_rai_change_sim[mm_as_id]

#else


byte                      mm_state;
byte                      mm_idle_substate;
IE_info_T                 mm_IE_info[MAXNO_IES_IN_MESSAGE];
byte                      mm_no_of_ies;
mm_system_information_T   mm_system_information;
mm_last_reg_dsac_information_T mm_last_reg_dsac_information;
boolean                   mm_llc_needs_to_be_resumed;
mm_csfb_service_ind_status_enum_T mm_csfb_service_ind_status;
boolean                   mm_nreg_req_received;
byte                      mm_pending_no_service_to_reg;
boolean                   mm_cnm_rel_req_received;

/******            mm_controls_resel_dec_mt_cs_call      ********
 *  In case of CS MT call, this flag indicates to MM if MM is in 
 *  control of allowing/disallowing Reselection when asked by RRC
 *  */
byte                      mm_controls_resel_dec_mt_cs_call = FALSE;

boolean                   mm_present_all_plmns;
location_information_T    mm_location_information;
held_service_request_T    mm_held_cnm_service_req;
boolean                   mm_reg_waiting_for_reg_cnf;
boolean                   mm_ready_to_send_reg_cnf;
boolean                   mm_needs_to_send_mmr_service_ind;
boolean                   mm_waiting_for_service_cnf;
boolean                   mm_waiting_for_stop_mode_cnf;
boolean                   mm_waiting_for_deact_cnf;
boolean                   mm_waiting_for_ph_status_change_cnf;
boolean                   mm_hard_abort_ue_sglte_mode_switch_pending;
sys_radio_access_tech_e_type mm_last_active_rat_stopped;
lu_reject_information_T   mm_lu_reject_info;
byte                      mm_serving_lai[LAI_SIZE];
byte                      mm_lu_attempt_counter;
byte                      mm_pending_lu_request;
byte                      mm_lu_start_reason;
byte                      mm_cipher_key_sequence_number;
boolean                   mm_follow_on_request;
boolean                   mm_follow_on_proceed;
byte                      mm_reject_cause;
boolean                   mm_cnm_is_active;
boolean                   mm_sms_is_active;
boolean                   mm_T3213_timed_out;
byte                      mm_idle_transition_timer;
byte                      mm_timer_status[MAX_MM_TIMER+1];
boolean                   mm_possible_imsi_attach;
imsi_data_T               mm_stored_imsi;
ef_kc_T                   mm_ef_kc;
boolean                   mm_ef_kc_valid;
uint32                    mm_op_mode;
boolean                   mm_sig_low_priority;
boolean                   mm_nreg_release_pending;
boolean                   mm_integrity_protected;
serving_plmn_info_T       mm_serving_plmn;
boolean                   mm_gsm_supported;
boolean                   mm_wcdma_supported;
boolean                   mm_tdscdma_supported;
sys_radio_access_tech_e_type  mm_ue_pref_rat;
boolean                   mm_rat_change_pending;
boolean                   mm_serv_change_pending;
boolean                   mm_to_only_reg;
sys_radio_access_tech_e_type mm_previous_serving_rat;

boolean                   mm_dual_mode;
sys_band_mask_type        mm_band_pref;
#ifdef FEATURE_LTE
boolean                   mm_stop_mode_local_detach_peding;
sys_ps_detach_e_type      mm_ps_detach_type;
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
sys_rat_pri_list_info_s_type    mm_rat_pri_list_info;
#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_to_nw;
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */

#if defined(FEATURE_LTE)
boolean mm_csfb_is_blind_redir_to_lte_req_to_be_sent;
#endif

establishment_cause_T     mm_cnm_srv_est_cause;

mm_connection_type_T   mm_connection_type;
rrc_lo_access_class_e_type mm_lo_access_class;
rrc_hi_access_class_type   mm_hi_access_class;
boolean                    mm_deep_sleep_pending ;
boolean                   mm_hard_abort_pending;
boolean                   mm_deep_sleep_alt_scan;

boolean                   mm_waiting_for_csmt_response;

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
sys_band_mask_type         mm_ue_band_class_pref;
sys_band_mask_e_type       mm_active_band;
#endif

sys_band_class_e_type     mm_band_indicator;
byte                      mm_previous_serving_lai[LAI_SIZE];

byte                      mm_gstk_previous_serving_lai[LAI_SIZE];

byte                      mm_previous_idle_substate;
mm_as_cell_access_status_T mm_prev_cell_access;

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
boolean           mm_lcs_active;
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_CCBS
boolean           mm_supports_NIMO_CM_connection;
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
WtoGCCO_in_progress_enum_T  WtoGCCO_state;
#endif

#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
boolean mm_is_lu_to_be_initiated_t3346_ltogw;
#endif

#ifdef FEATURE_INTERRAT_PCCO_GTOW
WtoGCCO_in_progress_enum_T  GtoWCCO_state;
#endif

mm_rat_handler_status_type mm_rat_handler_status;

/* Used to remember if combine procedure is performed last to make MSC-GS association */
boolean mm_gs_association_established;
boolean mm_initiate_lu_upon_entry_into_new_cell;
uint32 mm_serving_cell_id;
boolean mm_serving_cell_valid;
uint32 mm_lu_failed_cell_id;

mmr_cause_e_type mm_reg_cause;
#ifdef FEATURE_DSAC
boolean                    mm_ps_access_barred_to_unbarred;
boolean                    mm_cs_access_barred_to_unbarred;
byte                       mm_last_procedure_to_update_registration;
#endif 

boolean                 mm_manual_mode_user_selection_procedure;

#ifdef FEATURE_ENHANCED_NW_SELECTION
boolean                    mm_force_lu;
dword                      mm_foreground_search_timer_value;
mm_lai_timer_info_list_info_T mm_foreground_search_lai_timer_list;
#endif

boolean                    mm_managed_roaming_enabled;

/* defined in ms_cc_mm.h file this should be in synchronous with the ones defined in RRC 
   used for storing call_type sent by CC, which should be passed in RRC_EST_REQ for CS Domain */
call_type_for_rrc_est_req_T call_type_for_rrc_est_req;

boolean      mm_mmr_cell_service_ind_pending;

mm_attach_failed_status_type mm_mmr_attach_ind_status ;                              

/* Flag to indicate whether IMSI stored in NV is same as the 
   one in USIM/SIM 
*/
boolean      mm_imsi_is_native;

/* Flag to indicate whether GSM access is allowed or not */
boolean      auth_gsm_access;

/* SIM/USIM access class information to be sent to RR/RRC */
byte         mm_sim_access_class[MM_SIM_EFACC_SIZE];

/*LU reject cause for each failure attempt */
byte         mm_lu_reject_cause[MAX_LU_ATTEMPT_SIZE];

/* GSM security context for USIM */
boolean      auth_usim_gsm_security_context;

/* Mobile operation mode */
byte         mm_ms_opmode;

/* 2 or 3 digit MNC */
byte         mm_mnc_length;

/* Flag to remember last network selection mode send to RR/RRC */
sys_network_selection_mode_e_type last_requested_network_selection_mode;
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
byte mm_pending_lu_type;
#endif

#ifdef FEATURE_TC
boolean mm_tc_is_active;
#endif

boolean mm_service_ind_blocked_due_to_cs_reg;
boolean mm_service_ind_blocked_due_to_irat_cs_reg; 
boolean mm_connection_activated;
boolean mm_is_foreground_search_to_be_initiated; 

boolean mm_is_gmm_moved_dereg_due_to_csreg;

#ifdef FEATURE_NAS_REL10
sys_plmn_id_s_type mm_3gpp_t3246_plmn_id;
boolean mm_store_integrity_protected             = FALSE;
dword   mm_t3246_backoff_timer_value             = 0;

dword   mm_stored_per_ms_t3212_value          = 0;
boolean mm_is_per_ms_t3212_present            = FALSE;
#endif
mm_pended_stop_mode_req_s_type mm_pended_stop_mode_req;

#ifdef FEATURE_WRLF_SYSTEM_SEL
boolean    mm_received_suitable_search_end_ind = FALSE ;
#endif /* FEATURE_DUAL_SIM*/

word mm_coord_camped_cell_id;

#ifdef FEATURE_NAS_REL11
boolean mm_is_first_rrc_connection_attempt;
#endif

boolean mm_is_cs_connected_due_to_srvcc;

boolean  mm_psm_ready_req_rejected;

byte mm_plmn_search_transaction_id;
boolean mm_reject_call_on_rai_change;

#ifdef FEATURE_DUAL_DATA
boolean  mm_is_volte_only_ps;
#endif

#endif /*  FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

#if defined  FEATURE_LTE 
uint32 mm_extend_lte_disable_duration;
#endif


#ifdef FEATURE_LTE
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
boolean mm_multimode_rat_change_pending_sim[MAX_NAS_STACKS];
#define mm_multimode_rat_change_pending mm_multimode_rat_change_pending_sim[mm_as_id]
#else
boolean mm_multimode_rat_change_pending;
#endif
#endif

byte                      mm_nv_tighter_capability;
byte                      mm_nv_vamos_feature_support;
mm_emm_debug_buffer_s_type mm_emm_debug_buffer[MM_DEBUG_MESSAGE_BUFFER_SIZE];
dword mm_emm_debug_buffer_index = 0;
/* this variable is used in classmark information reading from EFS
    Allowed values o and 1, default value 1*/
byte ss_screening_indicator;

/* this variable is used to perform retry for emergency call on access
   class not allowed cell. Allowed values 0 and 1, default value 0 */
byte mm_retry_em_call_on_access_blocked_cell = 0;
#ifdef FEATURE_NAS_REL12
byte mm_optimised_ps_signalling_connection_release = 0x01;
#else
byte mm_optimised_ps_signalling_connection_release = 0;
#endif

byte mm_ps_sig_conn_rel_after_ps_sms = 0;

#ifdef FEATURE_NAS_REL11
mm_nas_nv_sim_parameters_T  mm_nas_nv_sim_hfns;
#endif
boolean mm_is_qrd_device = FALSE;
byte mm_managed_roaming_retry_lu_in_manual_mode = 0;
/* Variables to control the UE revision level behavior based on network support */
#ifdef FEATURE_DUAL_SIM
ue_force_rel_mode_type     mm_nv_force_sgsnr_r99_sim[MAX_AS_IDS];
#define mm_nv_force_sgsnr_r99 mm_nv_force_sgsnr_r99_sim[mm_sub_id]

ue_force_rel_mode_type     mm_nv_force_mscr_r99_sim[MAX_AS_IDS];
#define mm_nv_force_mscr_r99 mm_nv_force_mscr_r99_sim[mm_sub_id]
#else
ue_force_rel_mode_type     mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
ue_force_rel_mode_type     mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
#endif

ue_nas_rel_compliance_type    mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL6;

byte mm_nv_wcdma_hsupa_category = MM_RRC_WCMDA_HSUPA_DEFAULT_CATEGORY; /*RRC_WCMDA_HSUPA_DEFAULT_CATEGORY*/
/*   The EFS item would indicate a 0x0 or 0x1 for the als value to be used     */
/*   The value 0x0 => 4   and 0x1  =>  6 */

/* NV Variable which is User Configured through UI, about his AMR preference */
/* -------------AMR CODEC  USER PREFERENCES-- ------------------*/                    
/*                                  -----  BITMAP  ------                                         */          
/* --------------------------------------------------------------*/                    
/* |  08   |  07  |   06  |  05  |       04     |      03      |     02      |     01     |  */   
/* --------------------------------------------------------------*/          
/* |  XX..|  XX..|  XX..|  XX  | WCDMA   |     GSM    |    GSM    |   GSM    |  */          
/* |  XX..|  XX..|  XX..|  XX  | AMR  WB  |  HR AMR  | AMR WB | AMR NB |   */          
/* ---------------------------------------------------------------*/                    
byte                      gsm_wcdma_amr_codec_preference = 0x0;

/*
 * -------- global variable preserve_ui_speech_codec_preference --------------
 *
 * This flag tells whether or not to preserve speech codec preferences given by UI.
 * This will be set after reading EFS. This is done inside mm_init()
 */
byte                      preserve_ui_speech_codec_preference = 0; 

/* -----------------   QRD CSVT call NV item. -------------- 
 *
 * This indicates whether device is QRD device or not
 */
byte                      nas_is_qmss_enabled                 = 0;

/* This indiactes whether Long FTN is supported or not  */
byte                      long_ftn_support                    = 0;


/*
 * -------- global variable mmode_nv_setting_for_device_mode --------------
 *
 * This flag tells the choice for this device - whether the device is DSDS or DSDA capable
 * This will be set after reading MMODE EFS - /nv/item_files/modem/mmode/device_mode
 */
sys_modem_device_mode_e_type  mmode_nv_setting_for_device_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
uint16       mm_client_activated_group_sim[MAX_NAS_STACKS];
uint16       mm_nas_changed_group_sim[MAX_NAS_STACKS];
/* use in MN to use appropriate as_id */
boolean      mm_mn_sglte_svlte_mode = FALSE;
#ifdef FEATURE_NAS_ECALL
mmr_ecall_mode_e_type mm_ecall_mode_type_sim[MAX_NAS_STACKS];
boolean mm_need_to_perform_ecall_inactivity_sim[MAX_NAS_STACKS];
byte    mm_call_est_cause_sim[MAX_NAS_STACKS];
uint16  mm_ecall_inactivity_timer_sim[MAX_NAS_STACKS];
#endif
#else
uint16       mm_client_activated_group;
uint16       mm_nas_changed_group ;
#ifdef FEATURE_NAS_ECALL
mmr_ecall_mode_e_type mm_ecall_mode_type;
boolean mm_need_to_perform_ecall_inactivity;
byte    mm_call_est_cause = NO_CNM_SERVICE;
uint16  mm_ecall_inactivity_timer = 0;  
#endif
#endif

sys_manual_search_timer_value_type mm_emm_manual_search_timer_value; 
boolean mm_emm_is_qrd_device = FALSE;

// Define the TIMER REX critical section variable
rex_crit_sect_type  timer_crit_sect;

// Define the NV REX critical section variable
rex_crit_sect_type  nas_nv_crit_sect;
 

//Define the MM REX critical section variable
rex_crit_sect_type  mm_ftd_crit_sect;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
rex_crit_sect_type  mm_sim_crit_sect;
#endif

mm_nas_feature_to_be_configured_T mm_nas_feature_to_be_configured;
 
#if defined(FEATURE_SGLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
nas_sglte_feature_config_T sglte_nas_mm_config;
#endif

#ifdef FEATURE_GSM_CB
void cb_get_nv_items(mm_nas_nv_items_T*);
#endif
void mn_set_nv_items(mm_nas_feature_to_be_configured_T *);
#ifdef FEATURE_LTE
#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM) || defined(FEATURE_1XSRLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
sys_ue_mode_e_type mm_current_ue_mode =SYS_UE_MODE_NONE;
#endif
/*Store the previous RAT info, as this is required to 
  post an event during RAT change (b/w G,W & L)*/
#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM)
sys_radio_access_tech_e_type mm_previous_rat_sim[MAX_NAS_STACKS];
#define mm_previous_rat mm_previous_rat_sim[mm_as_id]
byte mm_sglte_nv_enabled =FALSE;
#else
sys_radio_access_tech_e_type mm_previous_rat = SYS_RAT_NONE;
#endif
#endif

#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
sys_overall_feature_t mm_sub_config_sim[MAX_AS_IDS];
#define mm_sub_config mm_sub_config_sim[mm_sub_id]
#endif
mm_as_id_e_type mm_sub_id = MM_AS_ID_1;

mm_as_id_e_type  mm_dds_sub_id = MM_AS_ID_1;


#if defined(FEATURE_TRIPLE_SIM)
mm_as_id_e_type mm_sub_id_stack[MAX_NAS_STACKS] = {MM_AS_ID_1, MM_AS_ID_2, MM_AS_ID_3};
#elif defined FEATURE_SGLTE_DUAL_SIM
mm_as_id_e_type mm_sub_id_stack[MAX_NAS_STACKS] = {MM_AS_ID_1, MM_AS_ID_2, MM_AS_ID_1};
#elif defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
mm_as_id_e_type mm_sub_id_stack[MAX_NAS_STACKS] = {MM_AS_ID_1, MM_AS_ID_2};
#endif 

#ifdef FEATURE_SGLTE
mm_sms_bearer_selection_enum_T mm_nv_sms_preferred_domain_id;
#endif

#ifdef FEATURE_DUAL_SIM
mm_sms_bearer_selection_enum_T mm_sms_preferred_domain_id_sim[MAX_AS_IDS];
#else
mm_sms_bearer_selection_enum_T mm_sms_preferred_domain_id;
#endif

#if defined(FEATURE_LTE)
/* This EFS item if set to TRUE will be used to perform LAU for 
   emergency call whenever there is change in LA */
#ifdef FEATURE_DUAL_SIM
boolean mm_lai_change_force_lau_for_emergency_sim[MAX_AS_IDS];
#define mm_lai_change_force_lau_for_emergency mm_lai_change_force_lau_for_emergency_sim[mm_sub_id]
#else
boolean mm_lai_change_force_lau_for_emergency;
#endif
boolean mm_nv_exclude_old_lai_type_ie    = FALSE;
sys_pch_state_s_type mm_rrc_pch_state_info;
#endif
boolean mm_conn_mode_manual_search_enabled = FALSE;
boolean mm_nv_conn_mode_manual_search = FALSE;
boolean mm_nv_disable_conn_mode_mplmn = FALSE;

#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
#ifdef FEATURE_DUAL_SIM
mm_utran_ps_voice_parameters_info mm_utran_ps_voice_parameters_sim[MAX_NAS_STACKS];
#define mm_utran_ps_voice_parameters mm_utran_ps_voice_parameters_sim[mm_as_id]
#else
mm_utran_ps_voice_parameters_info mm_utran_ps_voice_parameters;
#endif
#endif


#ifdef FEATURE_FEMTO_CSG
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
sys_csg_id_type     mm_serving_csg_id_sim[MAX_NAS_STACKS] = {SYS_CSG_ID_INVALID, SYS_CSG_ID_INVALID};
#define                   mm_serving_csg_id  mm_serving_csg_id_sim[mm_as_id]
#else
sys_csg_id_type     mm_serving_csg_id = SYS_CSG_ID_INVALID;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/
#ifdef FEATURE_DISABLE_CSG_BYDEFAULT
sys_csg_support_e_type    mm_nv_csg_support = SYS_CSG_SUPPORT_DISABLED;
#else
sys_csg_support_e_type    mm_nv_csg_support = SYS_CSG_SUPPORT_WCDMA;
#endif
#endif /*FEATURE_FEMTO_CSG*/
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  boolean mm_is_sim_busy_sim[MAX_AS_IDS] = {FALSE,FALSE};
#define mm_is_sim_busy mm_is_sim_busy_sim[mm_as_id]
#else
  boolean mm_is_sim_busy = FALSE;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean mm_cs_auth_sent_to_card_sim[MAX_NAS_STACKS];
#else
boolean mm_cs_auth_sent_to_card;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean mm_ps_auth_sent_to_card_sim[MAX_NAS_STACKS];
#else
boolean mm_ps_auth_sent_to_card;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

#ifdef FEATURE_FEMTO_CSG
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
sys_csg_selection_config_e_type mm_csg_selection_config_sim[MAX_NAS_STACKS] = 
                                               {SYS_CSG_SELECTION_CONFIG_NONE,
                                                SYS_CSG_SELECTION_CONFIG_NONE};
#define mm_csg_selection_config mm_csg_selection_config_sim[mm_as_id]
sys_csg_list_s_type mm_rejected_csg_list_sim[MAX_NAS_STACKS] = {{0}, {0}};
#define mm_rejected_csg_list mm_rejected_csg_list_sim[mm_as_id]
sys_csg_search_type_e_type mm_csg_search_type_sim[MAX_NAS_STACKS] = 
                                                 {SYS_CSG_SEARCH_TYPE_NONE,
                                                  SYS_CSG_SEARCH_TYPE_NONE};
#define mm_csg_search_type mm_csg_search_type_sim[mm_as_id]
#else
sys_csg_selection_config_e_type  mm_csg_selection_config = 
                                                 SYS_CSG_SELECTION_CONFIG_NONE;
sys_csg_list_s_type mm_rejected_csg_list = {0};
sys_csg_search_type_e_type mm_csg_search_type = SYS_CSG_SEARCH_TYPE_NONE;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#endif /* FEATURE_FEMTO_CSG*/

#ifdef FEATURE_DUAL_DATA
/*This stores that data priroty indicated by each client*/
uint8 mm_data_pri_value_sim[MAX_AS_IDS];
#define mm_data_pri_value mm_data_pri_value_sim[mm_sub_id]

#ifdef FEATURE_DUAL_SIM
size_t   mm_max_active_data = 1;
#endif
#endif
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_DUAL_SIM
boolean mm_policy_man_suspended_sim[MAX_AS_IDS];
#define mm_policy_man_suspended mm_policy_man_suspended_sim[mm_as_id]
#else
boolean mm_policy_man_suspended;
#endif 
#endif 

 
/*===========================================================================

FUNCTION MM_INITIALISE

DESCRIPTION
  This function initializes resources and global data used by the mm task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_initialise(void)
{

   byte    null_lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0, 0 };
   int     i = 0;
   mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
   cm_operator_e_type operator_name = OPERATOR_NULL;
   mm_nas_nv_items_T mm_nas_nv_items;
#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM)
   uint8 index;
#endif 
#ifdef FEATURE_NAS_REL10   
   mm_backoff_nv_info_s_type nv_backoff_info;
#endif
#ifdef FEATURE_SEGMENT_LOADING
   interface_t* ptr_T = NULL;
   interface_t* ptr_W = NULL;
   ptr_T = get_tdscdma_interface();
   ptr_W = get_wcdma_interface();
#endif

   mm_gsm_supported = mm_wcdma_supported = mm_tdscdma_supported = FALSE;
   /* 
   * Initialize STRUCT mm_nas_nv_items to 0.
   * Later in the function, values read from EFS will be stored here.
   */
   memset( &mm_nas_nv_items, 0x00, sizeof(mm_nas_nv_items_T) );

#ifdef FEATURE_NAS_REL10   
   memset( &nv_backoff_info, 0x00, sizeof(mm_backoff_nv_info_s_type) );
#endif

   if( ghdi_nvmem_register_task(MM_NVIF_SIG, mm_wait,
      mm_nvmem_default_values_generator) != GHDI_SUCCESS)
   {
      MSG_FATAL_DS(MM_SUB, "=MM= UE could not register with nvmem task", 0,0,0);
   }

    /* Read the UE NAS Release Compliance control NV item */
   if (ghdi_nvmem_read(NV_NAS_RELEASE_COMPLIANCE_I,
            (ghdi_nvmem_data_T *)&mm_nv_nas_rel_compliance) != GHDI_SUCCESS)
   {
     mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL6;
   }
   else if (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL_MAX)
   {
     MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for NAS_REL_COMPLIANCE NV %d, default R 5",
          mm_nv_nas_rel_compliance,0,0);
     mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL6;
   }

   /* Read the WCDMA HSUPA UE CATEGORY which is owned by lower layers */
   if (ghdi_nvmem_read(NV_WCDMA_HSUPA_CATEGORY_I,
            (ghdi_nvmem_data_T *)&mm_nv_wcdma_hsupa_category) != GHDI_SUCCESS)
   {
     mm_nv_wcdma_hsupa_category = MM_RRC_WCMDA_HSUPA_DEFAULT_CATEGORY;  /* Set the default value  */
     MSG_ERROR_DS(MM_SUB, "Unable to Read WCDMA HSUPA category NV (4210); Set to Default value : 5",0,0,0);
   }
   else
   {
     MSG_HIGH_DS_1(MM_SUB, "WCDMA HSUPA category NV (4210) is Read and Set to : %d", mm_nv_wcdma_hsupa_category);
   }

   /* User Preferences of AMR Speech Codecs is captured by the NV_UMTS_AMR_CODEC_PREFERENCE_CONFIG_I NV Item */
   if (ghdi_nvmem_read(NV_UMTS_AMR_CODEC_PREFERENCE_CONFIG_I,
            (ghdi_nvmem_data_T *)&gsm_wcdma_amr_codec_preference) != GHDI_SUCCESS)
   {
     gsm_wcdma_amr_codec_preference = 0x0f;  /* Set the default value */
     MSG_ERROR_DS(MM_SUB, "=MM= Unable to read NV for WCDMA/GSM AMR CODEC Preferences flag",0,0,0);
   }
   else
   {
     if (gsm_wcdma_amr_codec_preference > 0x0F)
     {
       MSG_HIGH_DS_1(MM_SUB ,"=MM= Wrong value for WCDMA GSM AMR CODEC Preference %d", gsm_wcdma_amr_codec_preference);
       gsm_wcdma_amr_codec_preference = 0x0f;  /* Set the default value */
     }
   }
   /*  Add the AMR Feature Flag specific guarding for the Values that could be set in NV
       if the underlying  AMR Feature Flag support is enabled only then the corresponding value should allowed to be set in NV */
#ifndef FEATURE_GSM_AMR_WB
   /* Unset the GSM AMR WB Bit      0000 00X0 */
   gsm_wcdma_amr_codec_preference &= ~0x02; //0xFD;
#endif
#ifndef FEATURE_VOC_AMR_WB
   /* Unset the WCDMA AMR WB Bit    0000 X000 */
   gsm_wcdma_amr_codec_preference &= ~0x08;
#endif
   MSG_HIGH_DS_1(MM_SUB ,"=MM= WCDMA GSM AMR CODEC  Preference : %X ",gsm_wcdma_amr_codec_preference);
   /* setting to default value*/
   ss_screening_indicator = 0x01;
   mm_retry_em_call_on_access_blocked_cell = 0;
   mm_managed_roaming_retry_lu_in_manual_mode = 0;
   mm_mmr_attach_ind_status = ATTACH_FAILED_IND_NOT_REQUIRED;
#ifdef FEATURE_NAS_REL12
   mm_optimised_ps_signalling_connection_release = 0x01;
#else
   mm_optimised_ps_signalling_connection_release = 0;
#endif
   mm_ps_sig_conn_rel_after_ps_sms = 0;

#ifdef FEATURE_NAS_REL11
   memset(&mm_nas_nv_sim_hfns,0,sizeof(mm_nas_nv_sim_parameters_T));
#endif
   mm_is_qrd_device = FALSE;             

#ifdef FEATURE_NAS_ECALL
#ifdef FEATURE_DUAL_SIM
    status =  mcfg_fs_read("/nv/item_files/modem/nas/t3242",
                          &mm_ecall_inactivity_timer_sim[MM_AS_ID_1],
                          sizeof(mm_ecall_inactivity_timer),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV t3242, status : %d ",status);
     /*ERA GLONASS has 8hrs as default value*/
     mm_ecall_inactivity_timer_sim[MM_AS_ID_1] = 8*60;
   }

      /* ecall inactivity configurable timer in mins t3242 and t3243 */
   status =mcfg_fs_read("/nv/item_files/modem/nas/t3242",
                          &mm_ecall_inactivity_timer_sim[MM_AS_ID_2],
                          sizeof(mm_ecall_inactivity_timer),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_2);
   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV t3242, status : %d ",status);
     /*ERA GLONASS has 8hrs as default value*/
     mm_ecall_inactivity_timer_sim[MM_AS_ID_2] = 8*60;
   }

   MSG_HIGH_DS_3(MM_SUB, "=MM= NV NAS eCall inactivity timer: %d %d %d msecs ",status,
                                               mm_ecall_inactivity_timer_sim[MM_AS_ID_1],
                                               mm_ecall_inactivity_timer_sim[MM_AS_ID_2]);
#else 
   /* ecall inactivity configurable timer in mins t3242 and t3243 */
   status =  mcfg_fs_read("/nv/item_files/modem/nas/t3242",
                          &mm_ecall_inactivity_timer_sim[MM_AS_ID_1],
                          sizeof(mm_ecall_inactivity_timer),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV t3242, status : %d ",status);
     /*ERA GLONASS has 8hrs as default value*/
     mm_ecall_inactivity_timer = 8*60;
   }

   MSG_HIGH_DS_2(MM_SUB, "=MM= NV NAS eCall inactivity timer: %d %d msecs ",status, mm_ecall_inactivity_timer);
#endif
#endif
             
   status = mcfg_fs_read("/nv/item_files/modem/nas/nas_config_feature",
                          &mm_nas_nv_items,
                          sizeof(mm_nas_nv_items_T),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);

   MSG_HIGH_DS_1(MM_SUB ,"=MM= NV NAS CONFIG FEATURE status : %d ",status);

   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV nas_config_feature, status : %d ",status);
     mm_nas_feature_to_be_configured.als_itc_value = (byte)(ALS_ITC_VALUE_4) ;
     mm_nas_feature_to_be_configured.No_auto_answer_on_hold = 0;
     mm_nas_feature_to_be_configured.MT_Call_reject_cause = 21; /*Call rejected*/
     mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page = 0;
     mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred = 0;
     /* mn global preserve_ui_speech_codec_preference will be set to default value of 0 */
     preserve_ui_speech_codec_preference = 0;
   }
   else
   {
#ifdef FEATURE_GSM_CB
     cb_get_nv_items
     (
       &mm_nas_nv_items
     ); 
#endif

     memscpy(&mm_nas_feature_to_be_configured,sizeof(mm_nas_feature_to_be_configured_T),&mm_nas_nv_items,sizeof(mm_nas_feature_to_be_configured_T));
	 
     if (mm_nas_feature_to_be_configured.als_itc_value != (byte)(ALS_ITC_VALUE_6))
     {
       MSG_HIGH_DS_1(MM_SUB ,"=MM= NV NAS CONFIG FEATURE  als_itc_value: %d ",mm_nas_feature_to_be_configured.als_itc_value);
       mm_nas_feature_to_be_configured.als_itc_value  = (byte)( ALS_ITC_VALUE_4);
     }
     if (mm_nas_feature_to_be_configured.No_auto_answer_on_hold !=1)
     {
       MSG_HIGH_DS_1(MM_SUB ,"=MM= NV NAS CONFIG FEATURE  No_auto_answer_on_hold: %d ",mm_nas_feature_to_be_configured.No_auto_answer_on_hold);
       mm_nas_feature_to_be_configured.No_auto_answer_on_hold  =0;
     }
     if (mm_nas_feature_to_be_configured.MT_Call_reject_cause != 17)
     {
       MSG_HIGH_DS_1(MM_SUB ,"=MM= NV NAS CONFIG FEATURE  MT_Call_reject_cause: %d ",mm_nas_feature_to_be_configured.MT_Call_reject_cause);
       mm_nas_feature_to_be_configured.MT_Call_reject_cause = 21;
     }
     if (mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page != 1)
     {
       MSG_HIGH_DS_1(MM_SUB ,"=MM= NV NAS CONFIG FEATURE  UE_init_rel_on_spurious_page: %d ",mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page);
       mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page = 0;
     }
     if (mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred != 1)
     {
       MSG_HIGH_DS_1(MM_SUB ,"=MM= NV NAS CONFIG FEATURE  CS_Domain_absolutely_preferred: %d ",mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred);
       mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred = 0;
     }
     if((mm_nas_nv_items.version >= 2) && (mm_nas_nv_items.cc_preserve_ui_speech_codec_preference != 0))
     {
       preserve_ui_speech_codec_preference = 1;
     }
     else
     {
       preserve_ui_speech_codec_preference = 0;
     }
     if(mm_nas_nv_items.version >= 6)
     {
       if(mm_nas_nv_items.ss_screaning_indicator > 1)
       {
         ss_screening_indicator = 0x01;
       }
       else
       {
         ss_screening_indicator = mm_nas_nv_items.ss_screaning_indicator;
       }
     }
     if(mm_nas_nv_items.version >= 7)
     {
       if(mm_nas_nv_items.retry_em_call_on_access_blocked_cell > 1)
       {
         mm_retry_em_call_on_access_blocked_cell = 0x01;
       }
       else
       {
         mm_retry_em_call_on_access_blocked_cell   =  mm_nas_nv_items.retry_em_call_on_access_blocked_cell;
       }
	 }
     if(mm_nas_nv_items.version >= 8)
     {
       if(mm_nas_nv_items.mm_managed_roaming_retry_lu_in_manual_mode > 1)
       {
         mm_managed_roaming_retry_lu_in_manual_mode = 0x01;
       }
       else
       {
         mm_managed_roaming_retry_lu_in_manual_mode   =  mm_nas_nv_items.mm_managed_roaming_retry_lu_in_manual_mode;
       }
     }
     if(mm_nas_nv_items.version >= 9)
     {
       if(mm_nas_nv_items.optimised_ps_signalling_connection_release > 1)
       {
         mm_optimised_ps_signalling_connection_release = 0x01;
       }
       else
       {
         mm_optimised_ps_signalling_connection_release   =  mm_nas_nv_items.optimised_ps_signalling_connection_release;
       }          
     }
     MSG_HIGH_DS_1(MM_SUB ,"=MM= version of mn_nas_nv_items = %d ",mm_nas_nv_items.version);

     if(mm_nas_nv_items.version >= 10)
     {
       if(mm_nas_nv_items.ps_sig_conn_rel_after_ps_sms >= 1)
       {
         mm_ps_sig_conn_rel_after_ps_sms = 0x01;
       }
       else
       {
         mm_ps_sig_conn_rel_after_ps_sms  = 0;
       }
     }
     MSG_HIGH_DS_1(MM_SUB ,"=MM= value of ps_sig_conn_rel_after_ps_sms = %d ",mm_nas_nv_items.ps_sig_conn_rel_after_ps_sms);

     if(mm_nas_nv_items.version >= 12)
     {
        if(mm_nas_nv_items.long_ftn_support >= 1)
        {
          long_ftn_support = 0x01;
        }
        else
        {
          long_ftn_support  = 0;
        }
      
     }
     MSG_HIGH_DS_1(MM_SUB ,"=MM= value of long_ftn_support = %d ",mm_nas_nv_items.long_ftn_support);

   }
#ifdef FEATURE_NAS_REL11  
   mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page = 0;
   MSG_HIGH_DS_1(MM_SUB ,"=MM= Since Feature Rel_11 is enabled, NV NAS CONFIG FEATURE  UE_init_rel_on_spurious_page is forced to: %d",mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page);
#endif

   MSG_HIGH_DS_2(MM_SUB, "=MM= preserve_ui_speech_codec_preference is set to %d, status of EFS read = %d ",preserve_ui_speech_codec_preference,status);

   mn_set_nv_items(&mm_nas_feature_to_be_configured);

   /* Read from NV whether or not csvti nv qmss is enable */
   if((status = mcfg_fs_read("/nv/item_files/modem/mmode/qmss_enabled", 
                     &nas_is_qmss_enabled, sizeof(byte),
                      MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1))== MCFG_FS_STATUS_OK) 
   {
     if(nas_is_qmss_enabled != TRUE)
     {
        nas_is_qmss_enabled = FALSE;
     }
     MSG_HIGH("MM: setting nas_is_qmss_enabled to %d",nas_is_qmss_enabled,0,0);
   }
   else
   {
     nas_is_qmss_enabled = FALSE;
     MSG_HIGH_DS_2(MM_SUB,"MM: nas_is_qmss_enabled could not be set from EFS, nas_is_qmss_enabled = %d, STATUS = %d",nas_is_qmss_enabled,status);
   }
   
   if((status = mcfg_fs_read("/nv/item_files/modem/mmode/qmss_enabled",
                 &mm_emm_is_qrd_device,sizeof(boolean), 
                  MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK) 
   {
     MSG_HIGH_1("MM: setting QRD device setting to %d",mm_emm_is_qrd_device);
   }
   else
   {
     MSG_HIGH_1("MM: setting QRD device to false as read failed, status = %d", status);
     mm_emm_is_qrd_device = FALSE;
   }
   memset( &mm_emm_manual_search_timer_value, 0x00, sizeof(sys_manual_search_timer_value_type) );

   if((status = mcfg_fs_read("/nv/item_files/modem/nas/mm_manual_search_timer_value", &mm_emm_manual_search_timer_value,
                       sizeof(sys_manual_search_timer_value_type), MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
   { 
     MSG_HIGH_DS_2(MM_SUB,"=MM= Manual Search  first timer Value status = %d length = %d",status,sizeof(sys_manual_search_timer_value_type));
    // FR Partial PLMN search support
     MSG_HIGH_DS_1(MM_SUB,"=MM= Manual Search  first timer Value version %u",
                                       mm_emm_manual_search_timer_value.version);
     MSG_HIGH_DS_3(MM_SUB,"=MM= Manual Search timer  read Value from NV FIRST %u MORE %u MAX %u",
                                        mm_emm_manual_search_timer_value.FirstPLMNTimer,
                                        mm_emm_manual_search_timer_value.MorePLMNTimer,
                                        mm_emm_manual_search_timer_value.MaxManualPLMNTimer);
     if(mm_emm_manual_search_timer_value.FirstPLMNTimer >= mm_emm_manual_search_timer_value.MaxManualPLMNTimer)
     {
        mm_emm_manual_search_timer_value.FirstPLMNTimer = 0;
     }
     else if(mm_emm_manual_search_timer_value.MorePLMNTimer >= mm_emm_manual_search_timer_value.MaxManualPLMNTimer)
     {
        mm_emm_manual_search_timer_value.MorePLMNTimer = 0;
     }
     MSG_HIGH_DS_3(MM_SUB,"=MM= Manual Search timer Value FIRST %u MORE %u MAX %u",
                                        mm_emm_manual_search_timer_value.FirstPLMNTimer,
                                        mm_emm_manual_search_timer_value.MorePLMNTimer,
                                        mm_emm_manual_search_timer_value.MaxManualPLMNTimer);

   }
   else
   {
     /* Setting this value to zero so that default can be 0*/
       MSG_HIGH_DS_1(MM_SUB,"=MM= Unable to read NV mm_manual_search_timer_value, status = %d",status);
       mm_emm_manual_search_timer_value.FirstPLMNTimer = 0;
       mm_emm_manual_search_timer_value.MorePLMNTimer =  0;
       mm_emm_manual_search_timer_value.MaxManualPLMNTimer = 0;
       MSG_HIGH_DS_3(MM_SUB,"=MM=EFS read failed FOR Manual PLMN Timer so appending default value FIRST %d  MORE %d  MAX %d",
                                                                          mm_emm_manual_search_timer_value.FirstPLMNTimer, 
                                                                          mm_emm_manual_search_timer_value.MorePLMNTimer,
                                                                          mm_emm_manual_search_timer_value.MaxManualPLMNTimer);
   }

   /* Set the supported mode of operation in UE */
#ifdef FEATURE_GSM
   mm_gsm_supported = TRUE;
#endif
#ifdef FEATURE_SEGMENT_LOADING
   if (ptr_W != NULL )
   {
      mm_wcdma_supported = TRUE;
   }

   if (ptr_T != NULL )
   {
      mm_tdscdma_supported = TRUE;
   }
#else
#ifdef FEATURE_WCDMA
   mm_wcdma_supported = TRUE;
#endif

#ifdef FEATURE_TDSCDMA
   mm_tdscdma_supported = TRUE;
#endif
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   for ( mm_as_id=MM_AS_ID_1; mm_as_id < (mm_as_id_e_type)MAX_NAS_STACKS; mm_as_id++ )
#endif
   {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
     mm_sub_id = mm_sub_id_stack[mm_as_id];
#endif
     mm_read_imei(NULL);
     mm_state = NULL;
     mm_idle_substate = NULL;
     mm_previous_idle_substate = NULL;
     mm_llc_needs_to_be_resumed = FALSE;
     mm_system_information.ATT = FALSE;
     mm_system_information.T3212_timer_value = 0;
     mm_system_information.reestablishment_allowed = FALSE;
     mm_system_information.mscr = 0;
     mm_system_information.sgsnr = FALSE;
     mm_system_information.cell_access = MM_AS_ACCESS_ALL_CALLS;
     mm_last_reg_dsac_information.cell_access = MM_AS_ACCESS_ALL_CALLS;
  #ifdef FEATURE_DSAC
     mm_invalidate_dsac_info();  
  #endif    
     mm_invalidate_ppac_info(); 
     mm_rat_change_pending  = FALSE;
     mm_serv_change_pending = FALSE;
     mm_to_only_reg         = FALSE;
     mm_reject_call_on_rai_change         = FALSE;
    mm_previous_serving_rat     = SYS_RAT_NONE;
     mm_nreg_req_received    = FALSE;
     mm_controls_resel_dec_mt_cs_call = FALSE;

     mm_cnm_rel_req_received = FALSE;
     mm_pending_no_service_to_reg = FALSE;
  
     mm_deep_sleep_pending = FALSE;
  
     mm_location_information.location_update_status = NOT_UPDATED;
  
     (void)memscpy( mm_location_information.lai, LAI_SIZE, null_lai, LAI_SIZE );
  
     (void)memset( mm_location_information.tmsi, 0xFF, TMSI_SIZE );

     mm_lu_reject_info.mm_conn_rej_timer_value = 0;
     
	 mm_held_cnm_service_req.present = FALSE;
  
     mm_reg_waiting_for_reg_cnf       = FALSE;
     mm_ready_to_send_reg_cnf         = FALSE;
     mm_needs_to_send_mmr_service_ind = FALSE;
     mm_waiting_for_service_cnf       = FALSE;
     mm_waiting_for_stop_mode_cnf     = FALSE;
     mm_waiting_for_deact_cnf         = FALSE;
     mmcoord_reg_req_pending          = FALSE;
     mm_waiting_for_csmt_response     = FALSE;
  
  #if defined (FEATURE_WRLF_SYSTEM_SEL) || defined (FEATURE_LTE)
     mm_rlf_stop_mode_pending = MM_RLF_STOP_MODE_PENDING_NONE;
  #endif
     mm_waiting_for_ph_status_change_cnf = FALSE;
     mm_last_active_rat_stopped  = SYS_RAT_NONE;
     mm_hard_abort_pending            = FALSE;
     mm_deep_sleep_alt_scan           = FALSE;
  #ifdef FEATURE_LTE
     mm_stop_mode_local_detach_peding = FALSE;
     mm_ps_detach_type                = SYS_PS_DETACH_TYPE_NONE;
  #endif
  #ifdef FEATURE_DUAL_SIM
     mm_abort_dual_switch_pending = SYS_STOP_MODE_REASON_NONE;
	 mm_stopped_due_to_mode_cap_change = FALSE;
  #endif
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
     mm_hard_abort_ue_sglte_mode_switch_pending = FALSE;
#endif
#ifdef FEATURE_SGLTE
     mm_no_service_ind_during_split = FALSE;
#endif
     mm_lu_reject_info.reject_cause = 0;
     (void)memset( mm_lu_reject_info.lai, 0xff, LAI_SIZE );
  
     (void)memscpy( mm_serving_lai, LAI_SIZE,
                   null_lai,
                   LAI_SIZE );
  
     (void)memscpy( mm_previous_serving_lai, LAI_SIZE,
                   null_lai,
                   LAI_SIZE );
  
     (void)memscpy( mm_gstk_previous_serving_lai, LAI_SIZE,
                   null_lai,
                   LAI_SIZE );

  #ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
     WtoGCCO_state = CCO_INACTIVE;
  #endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */
  
  #ifdef FEATURE_INTERRAT_PCCO_GTOW
     GtoWCCO_state  = CCO_INACTIVE;   
  #endif /* FEATURE_INTERRAT_PCCO_GTOW */      
  
     mm_stored_imsi.length = 0;
  
     mm_lu_attempt_counter = 0;
  
     mm_initiate_lu_upon_entry_into_new_cell = FALSE;
     mm_cipher_key_sequence_number = 7;
  
     mm_ef_kc.cipher_key_sequence_number = 7;
  
     mm_follow_on_request = FALSE;
  
     mm_follow_on_proceed = FALSE;
  
     mm_reject_cause = 0;
  
     mm_init_forbidden_LAIs();
  
     mm_serving_plmn.info.sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
  
     mm_cnm_is_active = FALSE;
  
     mm_sms_is_active = FALSE;
  
     mm_T3213_timed_out = FALSE;
  
     mm_idle_transition_timer = NULL;
  
     mm_pending_lu_request = NO_LU;
     mm_lu_start_reason = NO_LU;
  
     mm_plmn_search_in_progress = FALSE;
  
     for ( i=0; i< MAX_MM_TIMER+1; i++ )
     {
        mm_timer_status[i] = TIMER_STOPPED;
     }
  
     mm_possible_imsi_attach = FALSE;
  
     mm_ef_kc_valid = FALSE;
  
     mm_sig_low_priority = FALSE;
     mm_nreg_release_pending = FALSE;
     mm_ue_pref_rat = SYS_RAT_NONE;
  #ifndef FEATURE_RAT_PRIORITY_LIST
     mm_band_pref   = SYS_BAND_MASK_EMPTY;
  #else
     mm_rat_pri_list_info.num_items = 0;
  #endif /* FEATURE_RAT_PRIORITY_LIST */
  
     /* Set the default values to the Serving PLMN */
     mm_serving_plmn.info.plmn.identity[0] = 0xFF;
     mm_serving_plmn.info.plmn.identity[1] = 0xFF;
     mm_serving_plmn.info.plmn.identity[2] = 0xFF;
     mm_serving_plmn.lac.lac[0] = 0xFF;
     mm_serving_plmn.lac.lac[1] = 0xFF;
     mm_serving_plmn.rac = 0xFF;
  #ifdef FEATURE_FEMTO_CSG
     mm_serving_plmn.info.csg_info.csg_id = SYS_CSG_ID_INVALID;
  #endif
     
     mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
     mm_serving_plmn.info.sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
     mm_serving_plmn.info.plmn_service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
     mm_serving_plmn.info.plmn_forbidden = FALSE;
     mm_serving_plmn.info.roaming_ind = SYS_ROAM_STATUS_OFF;
     mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
     mm_serving_plmn.info.active_rat = SYS_RAT_NONE;
     mm_serving_plmn.info.ps_data_suspend = FALSE;
     mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
     mm_serving_cell_valid = FALSE;
  
     /*initialize to NA, plmn_reg_type is applicable to CAMP_ONLY */ 
     mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_NOT_APPLICABLE;
  
  #ifdef FEATURE_GSM_DTM
     mm_serving_plmn.dtm_supported = FALSE;
  #endif
     mm_serving_plmn.egprs_supported = FALSE;
  
     mm_serving_plmn.update_equiv_plmn_list = FALSE;
     memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
  
     mm_serving_plmn.t3122_running = TRUE;
  
     mm_serving_plmn.arfcn.num = 0xffff;
     mm_serving_plmn.arfcn.band = SYS_BAND_NONE;
     mm_serving_plmn.bsic = 0xff;
  
#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
     mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif
     /* Initialize the CNM service establish cause */
     mm_cnm_srv_est_cause = MM_RR_NO_CNM_SERVICE;
  
     mm_connection_type = NO_MM_CONNECTION;
     /* Set the default values for UE access class */
     mm_lo_access_class = RRC_LO_ACCESS_CLASS_ABSENT;
     mm_hi_access_class = RRC_HI_ACCESS_CLASS_ABSENT;
  
     mm_sms_preferred_domain_id = PS_PREFERRED;
  
  #ifdef FEATURE_SGLTE
     mm_nv_sms_preferred_domain_id = PS_PREFERRED;
  #endif
  
  #if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
  #ifndef FEATURE_RAT_PRIORITY_LIST
     mm_ue_band_class_pref = 0x00;
  #endif /* FEATURE_RAT_PRIORITY_LIST */
     mm_active_band = SYS_BAND_MASK_EMPTY;
  #endif
     mm_band_indicator = SYS_BAND_CLASS_NONE; 
  #if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
     mm_lcs_active = FALSE;
  #endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
  
     mm_rat_handler_status = MM_RAT_INITIAL;
  
  #ifdef FEATURE_CCBS
     mm_supports_NIMO_CM_connection = TRUE;
  #endif
  
  #ifdef FEATURE_GSM_GPRS
     glif_grr_camp_state = GLIF_CAMP_ON_CCCH ;
  #endif /* #ifdef FEATURE_GSM_GPRS */
  
     mm_managed_roaming_enabled = FALSE;
  
     mm_is_foreground_search_to_be_initiated  = TRUE;
  
     mm_is_gmm_moved_dereg_due_to_csreg = FALSE;
  #ifdef FEATURE_NAS_REL10
     mm_3gpp_t3246_plmn_id.identity[0]= 0xFF;
     mm_3gpp_t3246_plmn_id.identity[1]= 0xFF;
     mm_3gpp_t3246_plmn_id.identity[2]= 0xFF;
     mm_store_integrity_protected     = FALSE;
     mm_t3246_backoff_timer_value     = 0;

     gmm_3gpp_t3346_plmn_id.identity[0]= 0xFF;
     gmm_3gpp_t3346_plmn_id.identity[1]= 0xFF;
     gmm_3gpp_t3346_plmn_id.identity[2]= 0xFF;
     gmm_t3346_backoff_timer_value = 0;
  #endif
  
  #ifdef FEATURE_NAS_REL10
     mm_stored_per_ms_t3212_value = 0;
     mm_is_per_ms_t3212_present   = FALSE;
  #endif
  
     mm_mmr_cell_service_ind_pending = FALSE;  
  
     memset( (void *)mm_sim_access_class, 0, sizeof(mm_sim_access_class));
     
     for ( i=0; i< MAX_LU_ATTEMPT_SIZE; i++)
     {
       mm_lu_reject_cause[i] = 0xFF;
     }
     
     mm_imsi_is_native = FALSE;
  
     auth_gsm_access = FALSE;
  
     auth_usim_gsm_security_context = FALSE;
     /* TSTS Variables Initialization */  
     auth_gsm_cksn               = 0x07;
     auth_gsm_gprs_cksn          = 0x07;
     auth_cs_gprs_cksn           = 0x07;
     auth_ps_gprs_cksn           = 0x07;
     auth_ps_ciphering_algorithm = 0xFF;
     auth_gsm_gprs_keys_present  = FALSE;
  
     mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;

     mm_sim_perso_state = MMGSDI_PERSO_STATUS_NONE;
  
     mm_sim_card_mode           = MMGSDI_APP_UNKNOWN;
     mm_interrat_state          = MM_INTERRAT_CHANGE_STATE_TYPE_LO;
     mm_service_loss_on_rat     = SYS_RAT_NONE;
     mm_plmn_list_req_pending   = FALSE;
     mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
     mm_waiting_for_abort_cnf = FALSE;
     mm_stop_mode_req_pended = FALSE;
     mm_reg_req_revd_active_call = FALSE;
     mm_lu_pended_nmo_unknown      = FALSE;
     mm_cell_changed_to_cs_capable = FALSE;
     mm_need_to_process_sim_avail_req = FALSE;
     mm_waiting_for_open_session_cnf = FALSE;
     mm_last_paging_cause = 0xFF;
     rrc_connection_status = MMRRC_IDLE;
     cs_session_status = MMRRC_IDLE;
     ps_session_status = MMRRC_IDLE;
     pending_mm_msg_after_release = FALSE;
     pending_gmm_msg_after_release = FALSE;
  
     mm_cs_domain_possible_reest = FALSE;
     mm_ps_domain_possible_reest = FALSE;
     pending_gmm_msg_valid = FALSE;
     pending_mm_msg_valid = FALSE;
     send_sequence_number_mm_cc_ss = 0;
  #if defined(FEATURE_LTE)
     mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
     mm_stop_mode_local_detach_peding = FALSE;
  #endif
  
  #ifdef FEATURE_TC
     mm_tc_is_active = FALSE;
  #endif
     mm_reg_cause = MMR_CAUSE_NONE;
     mm_manual_mode_user_selection_procedure = FALSE;
  #ifdef FEATURE_ENHANCED_NW_SELECTION
     mm_force_lu = FALSE;
  #endif
  
  #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
     mm_pending_lu_type = MM_LU_CAUSE_RESERVED;
  #endif
     mm_service_ind_blocked_due_to_cs_reg = FALSE;
     mm_service_ind_blocked_due_to_irat_cs_reg = FALSE;
     mm_connection_activated = FALSE;
     mm_client_activated_group = SYS_NAS_INFO_MASK_NONE;
     mm_nas_changed_group = SYS_NAS_INFO_MASK_NONE ;
   
  #if defined(FEATURE_DUAL_SIM) && !defined(FEATURE_LTE)
     mm_sim_mode_pref = SYS_MODE_PREF_NONE;
  #endif
     mm_gstk_send_location_status = FALSE;
  
      /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
  #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  #ifdef FEATURE_MODEM_HEAP
      gmm_rat_chng_pended_msg = NULL;
  #endif
      gmm_rat_chng_pended_msg_valid = FALSE;
  #endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
  
  #ifdef FEATURE_MODEM_HEAP
      pending_mm_msg  = NULL;
      pending_gmm_msg = NULL;
  #endif
  
     /* TSTS Variables Initialization */ 
    
     mm_ms_opmode = 0;
  
     mm_mnc_length = 2;
  
  
     mm_lu_pended_nmo_unknown = FALSE;
  #ifdef FEATURE_ENHANCED_NW_SELECTION
    /* ----------------------------
    ** Default timeout = 6 minutes
    ** ---------------------------- */
     mm_foreground_search_timer_value =360000 ;
     mm_foreground_search_lai_timer_list.length =0;
     for ( i=0; i< MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH; i++)
     {
       memscpy(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai, LAI_SIZE, null_lai, LAI_SIZE);
       mm_foreground_search_lai_timer_list.lai_timer_list[i].rac = 0xFF;
       mm_foreground_search_lai_timer_list.lai_timer_list[i].is_ps_registered = FALSE;
       mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter = 0;
     }
  #endif
  #ifdef FEATURE_WRLF_SYSTEM_SEL
     mm_received_suitable_search_end_ind = FALSE;
     mm_wcdma_rlf_state = WCDMA_RLF_IDLE;
     mm_wrlf_rat_change_pending = FALSE;
  #endif
  #ifdef FEATURE_LTE
     mm_multimode_rat_change_pending = FALSE;
  #endif
  #ifdef FEATURE_LTE
     mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
     mm_csfb_set_state(MM_CSFB_STATE_NULL);
     mm_ps_detach_state = MM_PS_DETACH_STATE_NONE;
     mm_previous_rat = SYS_RAT_NONE;
  #endif
  #if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
     mm_utran_ps_voice_parameters.is_ps_voice_on_utran_supported = FALSE;
     mm_utran_ps_voice_parameters.is_last_utran_voims_supported = FALSE;
  #endif
  #ifdef FEATURE_WRLF_SYSTEM_SEL
    mm_wcdma_rlf_state = WCDMA_RLF_IDLE;
    mm_wrlf_rat_change_pending = FALSE;
    mm_wrlf_pending_rr_message_ptr = NULL;
    mm_wrlf_pending_rrc_message_ptr = NULL;	
    mm_wrlf_serving_plmn_ptr = NULL;
    mm_received_suitable_search_end_ind = FALSE;
  #endif 
  
  #ifdef FEATURE_FEMTO_CSG
    mm_serving_csg_id = SYS_CSG_ID_INVALID;
  #endif /*FEATURE_FEMTO_CSG*/
   
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
     mm_is_sim_busy = FALSE;
#endif

#ifdef FEATURE_NAS_REL11
   mm_is_first_rrc_connection_attempt = TRUE;
#endif
   mm_psm_ready_req_rejected = FALSE;

   mm_is_cs_connected_due_to_srvcc    = FALSE;
    mm_coord_camped_cell_id = 0xFF;
#ifdef FEATURE_NAS_ECALL
   mm_ecall_mode_type = MMR_ECALL_MODE_NORMAL;
   mm_need_to_perform_ecall_inactivity = FALSE;
   mm_call_est_cause = NO_CNM_SERVICE;  
#endif
   mm_service_ind_blocked_due_to_cs_reg = FALSE;
   mm_service_ind_blocked_due_to_irat_cs_reg = FALSE;
   mm_connection_activated = FALSE;
   mm_plmn_search_transaction_id = 0;
#ifdef FEATURE_DUAL_DATA
   mm_is_volte_only_ps     = FALSE;
#endif

}

#ifdef FEATURE_DUAL_SIM 
  mm_sub_capability_sim[MM_AS_ID_1] = SUBS_CAPABILITY_MULTIMODE;
  mm_sub_capability_sim[MM_AS_ID_2] = SUBS_CAPABILITY_GSM_ONLY;
#ifdef FEATURE_TRIPLE_SIM
  mm_sub_capability_sim[MM_AS_ID_3] = SUBS_CAPABILITY_GSM_ONLY;
#endif
#endif
   mm_nv_tighter_capability         = 1;
   mm_nv_vamos_feature_support      = VAMOS_OFF;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   mm_as_id = MM_AS_ID_1; 
#endif
   mm_dds_sub_id = MM_AS_ID_1;
  /*  Putting definition out of dual sim*/
   mm_msg_info = 0;
  
   memset(mm_emm_debug_buffer,0,sizeof(mm_emm_debug_buffer_s_type)*MM_DEBUG_MESSAGE_BUFFER_SIZE);
   mm_emm_debug_buffer_index = 0;


#ifdef FEATURE_VAMOS
   status = mcfg_fs_read("/nv/item_files/modem/geran/vamos_support", 
                           &mm_nv_vamos_feature_support,sizeof(vamos_support_e),
                            MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   
   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV vamos_support , status = %d",status);
     mm_nv_vamos_feature_support = VAMOS_1;
   }
   MSG_HIGH_DS_2(MM_SUB, "=MM= NV NAS VAMOS FEATURE status,VAMOS FEATURE VALUE : %d, %d",status,mm_nv_vamos_feature_support);
#endif

#ifdef FEATURE_DUAL_SIM
  /* DISABLE CONN MODE MANUAL SEARCH FOR DSDS */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/disable_conn_mode_manual_search_DSDS", 
                         &mm_nv_disable_conn_mode_mplmn, sizeof(boolean),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);

  if(status != MCFG_FS_STATUS_OK)
  {
    mm_nv_disable_conn_mode_mplmn = FALSE;
  }
  MSG_HIGH_DS_2(MM_SUB,"=MM= disable_conn_mode_manual_search_DSDS efs read, status: %d, default %d",
                         status,mm_nv_disable_conn_mode_mplmn);
#endif

#ifdef FEATURE_NAS_REL10
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
   if((status = mcfg_fs_read("/nv/item_files/modem/nas/mm_backoff_remaining_info", 
                              &nv_backoff_info, sizeof(mm_backoff_nv_info_s_type),
                              MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1))== MCFG_FS_STATUS_OK) 
   {
     mm_t3246_backoff_timer_value_sim[MM_AS_ID_1]  = nv_backoff_info.t3246_value;
     gmm_t3346_backoff_timer_value_sim[MM_AS_ID_1] = nv_backoff_info.t3346_value;
  
     memscpy(&mm_3gpp_t3246_plmn_id_sim[MM_AS_ID_1], sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3246_plmn, 
             sizeof(sys_plmn_id_s_type));
  
     memscpy(&gmm_3gpp_t3346_plmn_id_sim[MM_AS_ID_1], sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3346_plmn, 
              sizeof(sys_plmn_id_s_type));
  
   }   
   else
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV mm_backoff_remaining_info , status = %d",status);
     gmm_t3346_backoff_timer_value_sim[MM_AS_ID_1] = 0;
     mm_t3246_backoff_timer_value_sim[MM_AS_ID_1]  = 0;
     sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id_sim[MM_AS_ID_1]);
     sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id_sim[MM_AS_ID_1]);
   } /*Read SIM1 T3246/T3346  backoff timer and PLMN information*/

   if(!MM_SUB_IS_SGLTE_SUB(MM_AS_ID_1))
   {
     if((status = mcfg_fs_read("/nv/item_files/modem/nas/mm_backoff_remaining_info", 
                       &nv_backoff_info, sizeof(mm_backoff_nv_info_s_type),
                       MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_2))== MCFG_FS_STATUS_OK)
     {
       mm_t3246_backoff_timer_value_sim[MM_AS_ID_2]  = nv_backoff_info.t3246_value;
       gmm_t3346_backoff_timer_value_sim[MM_AS_ID_2] = nv_backoff_info.t3346_value;

       memscpy(&mm_3gpp_t3246_plmn_id_sim[MM_AS_ID_2], sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3246_plmn, 
               sizeof(sys_plmn_id_s_type));

       memscpy(&gmm_3gpp_t3346_plmn_id_sim[MM_AS_ID_2], sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3346_plmn, 
                sizeof(sys_plmn_id_s_type));
     }   
     else
     {
       MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV mm_backoff_remaining_info , status = %d",status);
       gmm_t3346_backoff_timer_value_sim[MM_AS_ID_2]  = 0;
       mm_t3246_backoff_timer_value_sim[MM_AS_ID_2]   = 0;
       sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id_sim[MM_AS_ID_2]);
       sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id_sim[MM_AS_ID_2]);
       MSG_HIGH_DS_0(MM_SUB,"=MM= SIM2 Unable to read NV for Back-off timer value ");
     } /*Read SIM2 T3246/T3346 backoff timer and PLMN information*/ 
   }
#else
   if((status = mcfg_fs_read("/nv/item_files/modem/nas/mm_backoff_remaining_info", 
                    &nv_backoff_info, sizeof(mm_backoff_nv_info_s_type),
                    MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1))== MCFG_FS_STATUS_OK)
   {
     mm_t3246_backoff_timer_value = nv_backoff_info.t3246_value;
     gmm_t3346_backoff_timer_value = nv_backoff_info.t3346_value;
  
     memscpy(&mm_3gpp_t3246_plmn_id, sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3246_plmn, 
             sizeof(sys_plmn_id_s_type));
  
     memscpy(&gmm_3gpp_t3346_plmn_id, sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3346_plmn, 
              sizeof(sys_plmn_id_s_type));  
   }   
   else
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV mm_backoff_remaining_info , status = %d",status);
     gmm_t3346_backoff_timer_value = 0;
     mm_t3246_backoff_timer_value = 0;
     sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id);
     sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id);
   } /*Read SIM1 T3246/T3346  backoff timer and PLMN information*/
 /*mm_timer_t3246_remaining_to_efs*/
#endif
#endif

    if((status = mcfg_fs_read("/nv/item_files/modem/mmode/qmss_enabled",
                       &mm_is_qrd_device,sizeof(boolean), 
                       MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH("MM: setting QRD device setting to %d",mm_is_qrd_device,0,0);
    }
    else
    {
      MSG_HIGH("MM: setting QRD device to false as read failed",0,0,0);
      mm_is_qrd_device = FALSE;
    }

   status = mcfg_fs_read("/nv/item_files/modem/nas/tighter_capability", 
                          &mm_nv_tighter_capability,sizeof(mm_nv_tighter_capability), 
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   
   if ((status != MCFG_FS_STATUS_OK) || (mm_nv_tighter_capability > 3))
   {
     mm_nv_tighter_capability = 1;
   }
   MSG_HIGH_DS_2(MM_SUB, "=MM= NV NAS Tighter capability : %d, %d",status,mm_nv_tighter_capability);

#ifdef FEATURE_LTE
  mm_rrc_pch_state_info.is_in_pch_state = FALSE;
#endif

/**************************************************
   Subscription sepcfic data initilization and NV read 
**************************************************/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_sub_id = MM_AS_ID_1;
  for ( index = 0; index < mm_sim_max_subscription ; index++ )
#endif
  {
#ifdef FEATURE_LTE
    status = mcfg_fs_read("/nv/item_files/modem/nas/suppress_add_update_param",
                          &mm_suppress_additional_update_param,
                          sizeof(boolean),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

    MSG_HIGH_DS_1(MM_SUB ,"=MM= Suppress add update param status : %d ",status);

    if (status != MCFG_FS_STATUS_OK)
    {
      mm_suppress_additional_update_param = FALSE;
    }
#endif
    if (mm_ghdi_nvmem_read(NV_MGRF_SUPPORTED_I,
       (ghdi_nvmem_data_T *)&mm_managed_roaming_enabled) != GHDI_SUCCESS)
    {
       mm_managed_roaming_enabled = FALSE;  /* Set the default value */
       MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to read NV for MANAGED ROAMING flag; disabled");
    }
    else
    {
       if ((mm_managed_roaming_enabled != FALSE) && (mm_managed_roaming_enabled != TRUE))
       {
         MSG_HIGH_DS_1(MM_SUB ,"=MM= Wrong value for MANAGED ROAMING flag %d", mm_managed_roaming_enabled);
         mm_managed_roaming_enabled = FALSE;  /* Set the default value */
       }
    }

   mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
   mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
   /* Read the UE Revision control NV item */

   operator_name = OPERATOR_NULL;

   if(mcfg_fs_read("/nv/item_files/modem/mmode/operator_name", 
						 &operator_name, sizeof(cm_operator_e_type),
						 MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id) == MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Setting operator to %d",operator_name);
   }
   else
   {
     operator_name = OPERATOR_NULL;
     MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting operator to NULL %d",operator_name);
   }

   if (mm_ghdi_nvmem_read(NV_FORCE_UE_SGSNR_R99_I,
            (ghdi_nvmem_data_T *)&mm_nv_force_sgsnr_r99) != GHDI_SUCCESS)
   {
     if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
     {
       mm_nv_force_sgsnr_r99 = NV_FORCE_UE_DYNAMIC;
     }
     else
     {
       mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
     }
   }
   else if (mm_nv_force_sgsnr_r99 >= NV_REL_MODE_MAX)
   {
     if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
     {
       MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_SGSNR_R99 NV %d",
             mm_nv_force_mscr_r99,0,0);
       mm_nv_force_sgsnr_r99 = NV_FORCE_UE_DYNAMIC;
     }
     else
     {
     MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_SGSNR_R99 NV %d, default R 99",
          mm_nv_force_sgsnr_r99,0,0);
     mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
   }
   }

   if (mm_ghdi_nvmem_read(NV_FORCE_UE_MSCR_R99_I,
            (ghdi_nvmem_data_T *)&mm_nv_force_mscr_r99) != GHDI_SUCCESS)
   {
     if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
     {
       mm_nv_force_mscr_r99 = NV_FORCE_UE_DYNAMIC;
     }
     else
     {
       mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
     }
   }
   else if (mm_nv_force_mscr_r99 >= NV_REL_MODE_MAX)
   {
     if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
     {
       MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_MSCR_R99 NV %d",
                          mm_nv_force_mscr_r99,0,0);
       mm_nv_force_mscr_r99 = NV_FORCE_UE_DYNAMIC;
     }
     else
     {
       MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_MSCR_R99 NV %d, default R99",
                          mm_nv_force_mscr_r99,0,0);
       mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
     }
   }
   /*
   Update mm_nv_nas_rel_compliance as '0' when either of NV_FORCE_UE_SGSNR_R99_I
   or NV_FORCE_UE_SGSNR_R99_I indicate '0'.
   */
   if((mm_nv_force_sgsnr_r99 == NV_FORCE_UE_REL97) || (mm_nv_force_mscr_r99 == NV_FORCE_UE_REL97))
   {
     mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL99;
   }

   MSG_HIGH_DS_3(MM_SUB, "=MM= NV_FORCE_UE_SGSNR_R99 : %d, NV_FORCE_UE_MSCR_R99_I : %d, NV_NAS_REL_COMPLIANCE : %d",
               mm_nv_force_sgsnr_r99, mm_nv_force_mscr_r99, mm_nv_nas_rel_compliance);
  

#if defined(FEATURE_LTE)
   mm_lai_change_force_lau_for_emergency = FALSE;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  #ifdef FEATURE_DUAL_DATA
    mm_data_pri_value = 0;
  #endif 
#ifdef FEATURE_TDSCDMA
  mm_policy_man_suspended = FALSE;
#endif 
    mm_sub_id++;
#endif
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_sub_id = MM_AS_ID_1;
#endif
  // If this NV is enabled then E Calls are not allowed in a NW without the UE being registered
  // in the NW. Default value should be set to 0.
  if((status = mcfg_fs_read("/nv/item_files/modem/mmode/sd/full_srv_req_in_emerg", 
                     &full_srv_req_in_emerg, sizeof(boolean),
                     MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Setting NV full_srv_req_in_emerg to %d",full_srv_req_in_emerg);
  }
  else
  {
    full_srv_req_in_emerg = FALSE;
    MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting full_srv_req_in_emerg to default %d",full_srv_req_in_emerg);
  }
  if((status =mcfg_fs_read("/nv/item_files/modem/nas/emergency_call_after_lu_in_lpm", 
                                                  &emergency_call_after_lu_in_lpm, sizeof(boolean),
                     MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Setting NV full_srv_req_in_emerg to %d",emergency_call_after_lu_in_lpm);
  }
  else
  {
    emergency_call_after_lu_in_lpm = FALSE;
    MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting emergency_call_after_lu_in_lpm to default %d",emergency_call_after_lu_in_lpm);
  }

  full_srv_req_in_emerg = full_srv_req_in_emerg|emergency_call_after_lu_in_lpm;
  mm_read_carrier_specific_efs();
}


/*===========================================================================

FUNCTION MM_RESET_GLOBAL_VARIABLES

DESCRIPTION
  This function resets the MM global variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_reset_global_variables(void)
{

   int       i = 0;
   byte    null_lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0, 0 };
   mm_system_information.T3212_timer_value = 0;

   mm_system_information.cell_access = MM_AS_ACCESS_ALL_CALLS; 
   mm_last_reg_dsac_information.cell_access = MM_AS_ACCESS_ALL_CALLS;
#ifdef FEATURE_DSAC
   mm_invalidate_dsac_info();
#endif
   mm_invalidate_ppac_info(); 
   mm_cnm_rel_req_received          = FALSE;
   mm_pending_no_service_to_reg     = FALSE;
   mm_held_cnm_service_req.present  = FALSE;

   mm_reg_waiting_for_reg_cnf       = FALSE; 

   mm_ready_to_send_reg_cnf         = FALSE; 

   mm_needs_to_send_mmr_service_ind = FALSE; 

   mm_waiting_for_service_cnf       = FALSE; 

   mmcoord_reg_req_pending          = FALSE; 
#if defined (FEATURE_WRLF_SYSTEM_SEL) || defined (FEATURE_LTE)
   mm_rlf_stop_mode_pending = MM_RLF_STOP_MODE_PENDING_NONE;
#endif

   mm_waiting_for_deact_cnf         = FALSE;

   mm_waiting_for_stop_mode_cnf     = FALSE;
   
   mm_serving_plmn.arfcn.num = 0xffff;
   mm_serving_plmn.arfcn.band = SYS_BAND_NONE;
   mm_serving_plmn.bsic = 0xff;

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
   WtoGCCO_state = CCO_INACTIVE;  
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

#ifdef FEATURE_INTERRAT_PCCO_GTOW
   GtoWCCO_state  = CCO_INACTIVE;  
#endif /* FEATURE_INTERRAT_PCCO_GTOW */      
   mm_lu_attempt_counter = 0; 

   mm_initiate_lu_upon_entry_into_new_cell = FALSE;

   mm_follow_on_request                    = FALSE;  

   mm_follow_on_proceed                    = FALSE;  

   mm_T3213_timed_out                      = FALSE;

   mm_idle_transition_timer                = NULL; 

   mm_pending_lu_request                   = NO_LU; 
   mm_lu_start_reason                      = NO_LU;

   mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;

    /* Initialize the CNM service establish cause */
   mm_cnm_srv_est_cause = MM_RR_NO_CNM_SERVICE; 
   mm_connection_type = NO_MM_CONNECTION;
   mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   mm_lcs_active = FALSE; 
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

   mm_rat_handler_status = MM_RAT_INITIAL; 

   mm_mmr_cell_service_ind_pending = FALSE;
   sys_plmn_undefine_plmn_id(&mmcoord_mmr_reg_cnf.service_state.plmn);   

   mm_serving_plmn.t3122_running = TRUE;

   mm_to_only_reg = FALSE;
   
   mm_reject_call_on_rai_change = FALSE;
   mm_previous_serving_rat = SYS_RAT_NONE;

#ifdef FEATURE_MODEM_HEAP
   if(pending_mm_msg != NULL)
   {
     modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
     pending_mm_msg = NULL;
     pending_mm_msg_valid = FALSE;
   }
#endif
#ifdef FEATURE_NAS_ECALL
   mm_need_to_perform_ecall_inactivity = FALSE;
   mm_call_est_cause = NO_CNM_SERVICE;  
#endif
#if defined(FEATURE_LTE)
   mm_service_loss_on_multimode_rat = SYS_RAT_NONE;
#endif
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
   mm_pending_lu_type = MM_LU_CAUSE_RESERVED;
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
   mm_wcdma_rlf_state = WCDMA_RLF_IDLE;
   mm_wrlf_rat_change_pending = FALSE;
#endif
   mm_lu_pended_nmo_unknown = FALSE;

   /*Reset serving LAI */
   mm_serving_lai[0] = 0xFF;
   mm_serving_lai[1] = 0xFF;
   mm_serving_lai[2] = 0xFF;
   mm_serving_lai[3] = 0xFF;
   mm_serving_lai[4] = 0xFE;

   for ( i=0; i< MAX_LU_ATTEMPT_SIZE; i++)
   {
     mm_lu_reject_cause[i] = 0xFF;
   }   
#ifdef FEATURE_TC
   mm_tc_is_active = FALSE;
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
  /* ----------------------------
  ** Default timeout = 6 minutes
  ** ---------------------------- */
   mm_foreground_search_timer_value =360000 ;
   mm_foreground_search_lai_timer_list.length =0;
   for( i=0; i< MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH; i++)
   {
    memscpy(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai, LAI_SIZE, null_lai, LAI_SIZE);
    mm_foreground_search_lai_timer_list.lai_timer_list[i].rac = 0xFF;
    mm_foreground_search_lai_timer_list.lai_timer_list[i].is_ps_registered = FALSE;
    mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter = 0;
   }
#endif

#ifdef FEATURE_LTE
   mm_stop_mode_local_detach_peding = FALSE;
   mm_ps_detach_type                = SYS_PS_DETACH_TYPE_NONE;
#endif

   mm_service_ind_blocked_due_to_cs_reg = FALSE;
   mm_service_ind_blocked_due_to_irat_cs_reg = FALSE;
   mm_connection_activated = FALSE;
#ifdef FEATURE_LTE
   mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
   mm_csfb_set_state(MM_CSFB_STATE_NULL);
#endif
   mm_is_foreground_search_to_be_initiated  = TRUE;
   
   mm_is_gmm_moved_dereg_due_to_csreg       = FALSE;
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
  mm_utran_ps_voice_parameters.is_ps_voice_on_utran_supported = FALSE;
  mm_utran_ps_voice_parameters.is_last_utran_voims_supported = FALSE;
#endif

#ifdef FEATURE_NAS_REL10
   mm_store_integrity_protected     = FALSE;

   mm_stored_per_ms_t3212_value = 0;
   mm_is_per_ms_t3212_present   = FALSE;
#endif

  mm_state = MM_NULL;
  mm_substate_control(MM_STD_DEACTIVATE);
  mm_previous_idle_substate = NULL;
  mm_llc_needs_to_be_resumed = FALSE;
#ifdef FEATURE_WRLF_SYSTEM_SEL
   mm_received_suitable_search_end_ind = FALSE;
#endif
#ifdef FEATURE_LTE
  mm_rrc_pch_state_info.is_in_pch_state = FALSE;
#endif
#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_MODE)
  {
    mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
    mm_hard_abort_ue_sglte_mode_switch_pending = FALSE;
  }
  mm_no_service_ind_during_split = FALSE;
#else
#ifdef FEATURE_1XSRLTE
  mm_hard_abort_ue_sglte_mode_switch_pending = FALSE;
#endif
#endif

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  mm_is_sim_busy = FALSE;
#endif
   mm_coord_camped_cell_id = 0xFF;
   mm_lu_reject_info.mm_conn_rej_timer_value = 0;

  mm_reg_req_revd_active_call = FALSE;
   
  (void)memscpy( mm_previous_serving_lai, LAI_SIZE,
                 null_lai,
                 LAI_SIZE );
  /*Reset search in progress during deact  */
  mm_plmn_search_in_progress = FALSE;

#ifdef FEATURE_NAS_REL11
  mm_is_first_rrc_connection_attempt = TRUE;
#endif
  mm_psm_ready_req_rejected = FALSE;
  
  mm_is_cs_connected_due_to_srvcc    = FALSE;
  
  mm_cs_auth_sent_to_card = FALSE;    
  mm_ps_auth_sent_to_card = FALSE;
  mm_plmn_search_transaction_id = 0;
#ifdef FEATURE_DUAL_DATA
  mm_is_volte_only_ps     = FALSE;
#endif
  mm_reg_cause            = MMR_CAUSE_NONE;

}
/*===========================================================================

FUNCTION IS_UMTS_MODE

DESCRIPTION
  This function tells if the UE can operate in UMTS op mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UMTS op mode, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean is_umts_mode(void)
{
#ifndef FEATURE_SEGMENT_LOADING
   if (mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
   {
     return TRUE;
   }
   else if (mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   {
     return TRUE;
   }
   else
   {
     return FALSE;
   }
#else
   if ((mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) && 
       (get_wcdma_interface()))
   {
     return TRUE;
   }
   else if ((mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) &&
            (get_tdscdma_interface()))
   {
     return TRUE;
   }
   else
   {
     return FALSE;
   }
#endif
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean is_lte_mode_per_nas_stacks( sys_modem_as_id_e_type as_id )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }

   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS)
   {
      return TRUE;
   }

   return FALSE;
#else
  (void)as_id;
  return is_lte_mode();
#endif

}
boolean is_tdscdma_mode_per_nas_stacks( sys_modem_as_id_e_type as_id )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }

#ifdef FEATURE_SEGMENT_LOADING
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS && (get_tdscdma_interface()))
   {
     return TRUE;
   }
#else
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   {
      return TRUE;
   }
#endif
   return FALSE;
#else
  (void)as_id;
   return is_tdscdma_mode();
#endif
}
boolean is_gsm_mode_per_nas_stacks(sys_modem_as_id_e_type as_id)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }

   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
   {
      return TRUE;
   }

   return FALSE;
#else
  (void) as_id;
  return is_gsm_mode();
#endif
}
boolean is_cs_umts_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_umts_mode_per_nas_stacks(stack_id_nas);
}
boolean is_cs_gsm_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_gsm_mode_per_nas_stacks(stack_id_nas);
}

boolean is_cs_wcdma_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_wcdma_mode_per_nas_stacks(stack_id_nas);

}
boolean is_cs_lte_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_lte_mode_per_nas_stacks(stack_id_nas);
}
boolean is_cs_tdscdma_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_tdscdma_mode_per_nas_stacks(stack_id_nas);
}

boolean is_ps_tdscdma_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
#endif

  return is_tdscdma_mode_per_nas_stacks(stack_id_nas);
}
boolean is_ps_umts_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
#endif

  return is_umts_mode_per_nas_stacks(stack_id_nas);
}
boolean is_ps_gsm_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
#endif

  return is_gsm_mode_per_nas_stacks(stack_id_nas);
}

boolean is_ps_wcdma_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
#endif

  return is_wcdma_mode_per_nas_stacks(stack_id_nas);
}


/*===========================================================================

FUNCTION IS_WCDMA_MODE

DESCRIPTION
  This function tells if the UE can operate in WCDMA operating mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if WCDMA op mode, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean is_wcdma_mode(void)
{
#ifdef FEATURE_SEGMENT_LOADING
   if ((mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) && (get_wcdma_interface()))
   {
     return TRUE;
   }
   else
   {
      return FALSE;
   }
#else
   return (mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS);
#endif
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION IS_TDSCDMA_MODE

DESCRIPTION
  This function tells if the UE can operate in TD-SCDMA operating mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if TD-SCDMA op mode, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean is_tdscdma_mode(void)
{
 #ifdef FEATURE_SEGMENT_LOADING
   if ((mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) && (get_tdscdma_interface()))
   {
     return TRUE;
   }
   else
   {
      return FALSE;
   }
#else
   return (mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS);
#endif

}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean is_umts_mode_per_nas_stacks( sys_modem_as_id_e_type as_id )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }
#ifdef FEATURE_SEGMENT_LOADING
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS && (get_wcdma_interface()))
   {
     return TRUE;
   }
   else if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS &&  (get_tdscdma_interface()))
   {
      return TRUE;
   }   
#else
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
   {
      return TRUE;
   }
   else if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   {
     return TRUE;
   }
#endif
   return FALSE;
#else
  (void)as_id;
   return is_umts_mode();
#endif

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean is_wcdma_mode_per_nas_stacks( sys_modem_as_id_e_type as_id )
{

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }


#ifdef FEATURE_SEGMENT_LOADING
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS && (get_wcdma_interface()))
   {
     return TRUE;
   }
#else
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
   {
      return TRUE;
   }
#endif
   return FALSE;
#else
  (void)as_id;
  return is_wcdma_mode();
#endif

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION get_preferred_mode

DESCRIPTION
  This function tells if the UE can operate in UMTS op mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UMTS op mode, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

sys_radio_access_tech_e_type get_preferred_mode(void)
{
  return mm_ue_pref_rat;
}

/*===========================================================================

FUNCTION IS_GSM_MODE

DESCRIPTION
  This function tells if the UE can operate in GSM op mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if GSM is Active RAT, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean is_gsm_mode(void)
{
   if (mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
   {
      return TRUE;
   }

   return FALSE;
}

/*===========================================================================

FUNCTION Is_WtoGCCO_inProgress

DESCRIPTION
  This function tells if W to G CCO is in progress

DEPENDENCIES
  None

RETURN VALUE
  TRUE if W to G CCO is in progress, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean is_WtoGCCO_in_progress
(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_modem_as_id_e_type as_id
#else
  void
#endif
)
{
#ifdef FEATURE_INTERRAT_PCCO_GTOW
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_FATAL_DS(MM_SUB, "=MM= Invalid AS ID:%d", as_id+1,0,0);
   }

   return (boolean) (GtoWCCO_state_sim[as_id] == CCO_IN_PROGRESS);
#else
   return (boolean) (GtoWCCO_state == CCO_IN_PROGRESS);
#endif
#else
   return FALSE;
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
}


ghdi_nvmem_status_T mm_nvmem_default_values_generator(
       ghdi_nvmem_status_T in_status, nv_items_enum_type item_type, nv_item_type *nv_item)
{
   switch(item_type)
   {
      case NV_UE_RAT_CAPABILITY_I:
         nv_item->ue_rat_capability = ue_umts_only_rat;
         break;

      default:
         return in_status;
   }

   return NV_DONE_S;
}

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
/*===========================================================================

FUNCTION get_preferred_band_class

DESCRIPTION
  This function returns the band class preference especified by the user

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/

sys_band_mask_type get_preferred_band_class(void)
{
  return mm_ue_band_class_pref;
}

/*===========================================================================

FUNCTION get_active_band

DESCRIPTION
  This function returns the active band

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/

sys_band_mask_e_type get_active_band(void)
{
  return mm_active_band;
}
#endif /* (FEATURE_GSM_BAND_PREF ||FEATURE_WCDMA_MULTIBAND) */

/*===========================================================================

FUNCTION IS_LTE_MODE

DESCRIPTION
  This function tells if the UE can operate in UMTS op mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UMTS op mode, FALSE otherwise

SIDE EFFECTS
  None
 
===========================================================================*/

boolean is_lte_mode(void)
{
   if (mm_serving_plmn.info.active_rat == SYS_RAT_LTE_RADIO_ACCESS)
   {
      return TRUE;
   }

   return FALSE;
}

#ifdef FEATURE_SGLTE

void mm_cpoy_individual_csdata(mm_as_id_e_type dest,mm_as_id_e_type src)
{
  equiv_plmn_list_s_type            equiv_plmn_list_temp;
  mm_system_information_sim[dest].T3212_timer_value    
                            =  mm_system_information_sim[src].T3212_timer_value;
  mm_system_information_sim[dest].ATT  = mm_system_information_sim[src].ATT;
  mm_system_information_sim[dest].reestablishment_allowed = 
                     mm_system_information_sim[src].reestablishment_allowed;
  mm_system_information_sim[dest].mscr = mm_system_information_sim[src].mscr;
  mm_system_information_sim[dest].sgsnr     = mm_system_information_sim[src].sgsnr;
  mm_system_information_sim[dest].cell_access     = mm_system_information_sim[src].cell_access;
  mm_system_information_sim[dest].dsac_present = mm_system_information_sim[src].dsac_present;
  mm_system_information_sim[dest].cs_cell_access = mm_system_information_sim[src].cs_cell_access;
  mm_system_information_sim[dest].ppac_present = mm_system_information_sim[src].ppac_present;
  mm_system_information_sim[dest].cs_reg_allowed = mm_system_information_sim[src].cs_reg_allowed;
  mm_system_information_sim[dest].page_rsp_allowed = mm_system_information_sim[src].page_rsp_allowed;           

  mm_last_reg_dsac_information_sim[dest].cell_access = mm_last_reg_dsac_information_sim[src].cell_access;
  mm_last_reg_dsac_information_sim[dest].dsac_present = mm_last_reg_dsac_information_sim[src].dsac_present;
  mm_last_reg_dsac_information_sim[dest].cs_cell_access = mm_last_reg_dsac_information_sim[src].cs_cell_access;

  memscpy(&mm_serving_plmn_sim[dest].info.plmn,sizeof(sys_plmn_id_s_type),
                              &mm_serving_plmn_sim[src].info.plmn,sizeof(sys_plmn_id_s_type)) ;
  memscpy(&mm_serving_plmn_sim[dest].info.registered_plmn,sizeof(sys_plmn_id_s_type),
                              &mm_serving_plmn_sim[src].info.registered_plmn,sizeof(sys_plmn_id_s_type)) ;
  memscpy(&mm_serving_plmn_sim[dest].info.active_band,sizeof(sys_active_band_u_type),
                              &mm_serving_plmn_sim[src].info.active_band,sizeof(sys_active_band_u_type)) ;
  memscpy(&mm_serving_plmn_sim[dest].info.lac,sizeof(sys_lac_type),
                              &mm_serving_plmn_sim[src].info.lac,sizeof(sys_lac_type)) ;
                              
  mm_serving_plmn_sim[dest].info.plmn_service_capability   = mm_serving_plmn_sim[src].info.plmn_service_capability;
  mm_serving_plmn_sim[dest].info.plmn_forbidden    =  mm_serving_plmn_sim[src].info.plmn_forbidden;
  mm_serving_plmn_sim[dest].info.roaming_ind  =  mm_serving_plmn_sim[src].info.roaming_ind ;
  mm_serving_plmn_sim[dest].info.service_status    =  mm_serving_plmn_sim[src].info.service_status  ;
  mm_serving_plmn_sim[dest].info.cs_service_status    =  mm_serving_plmn_sim[src].info.cs_service_status   ;
  mm_serving_plmn_sim[dest].info.no_service_cause    =  mm_serving_plmn_sim[src].info.no_service_cause   ;
  mm_serving_plmn_sim[dest].info.suitable_search_time    =  mm_serving_plmn_sim[src].info.suitable_search_time   ;
  mm_serving_plmn_sim[dest].info.acq_status    =  mm_serving_plmn_sim[src].info.acq_status   ;
  mm_serving_plmn_sim[dest].info.plmn_reg_type    =  mm_serving_plmn_sim[src].info.plmn_reg_type   ;
  mm_serving_plmn_sim[dest].info.emc_attached    =  mm_serving_plmn_sim[src].info.emc_attached   ;
  mm_serving_plmn_sim[dest].info.suitable_search_time    =  mm_serving_plmn_sim[src].info.suitable_search_time   ; 
  memscpy(&mm_serving_plmn_sim[dest].lac,sizeof(location_area_code_T),
                              &mm_serving_plmn_sim[src].lac,sizeof(location_area_code_T)) ;
  memscpy(&mm_serving_plmn_sim[dest].info.active_band,sizeof(sys_active_band_u_type),
                              &mm_serving_plmn_sim[src].info.active_band,sizeof(sys_active_band_u_type)) ;                            
  mm_serving_plmn_sim[dest].mm_service_state = mm_serving_plmn_sim[src].mm_service_state;
  mm_serving_plmn_sim[dest].network_op_mode = mm_serving_plmn_sim[src].network_op_mode;
  mm_serving_plmn_sim[dest].update_equiv_plmn_list = mm_serving_plmn_sim[src].update_equiv_plmn_list;
#ifdef FEATURE_GSM_DTM
  mm_serving_plmn_sim[dest].dtm_supported = mm_serving_plmn_sim[src].dtm_supported;
#endif /* FEATURE_GSM_DTM */
  
  mm_serving_plmn_sim[dest].t3122_running = mm_serving_plmn_sim[src].t3122_running;
  /*  Swapping of equivalent list */
  memscpy (&equiv_plmn_list_temp,sizeof(equiv_plmn_list_s_type), &mm_serving_plmn_sim[src].equiv_plmn_list,
                    sizeof(equiv_plmn_list_s_type));
  memscpy (&mm_serving_plmn_sim[src].equiv_plmn_list,sizeof(equiv_plmn_list_s_type), &mm_serving_plmn_sim[dest].equiv_plmn_list,
                    sizeof(equiv_plmn_list_s_type));
  memscpy (&mm_serving_plmn_sim[dest].equiv_plmn_list,sizeof(equiv_plmn_list_s_type), &equiv_plmn_list_temp ,
                    sizeof(equiv_plmn_list_s_type));
/* swapping end here */  

memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.plmn,sizeof(sys_plmn_id_s_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.plmn,sizeof(sys_plmn_id_s_type)) ;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.registered_plmn,sizeof(sys_plmn_id_s_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.registered_plmn,sizeof(sys_plmn_id_s_type)) ;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.active_band,sizeof(sys_active_band_u_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.active_band,sizeof(sys_active_band_u_type)) ;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.lac,sizeof(sys_lac_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.lac,sizeof(sys_lac_type)) ;
                              
  mmcoord_mmr_reg_cnf_sim[dest].service_state.sim_state = mmcoord_mmr_reg_cnf_sim[src].service_state.sim_state; 
  mmcoord_mmr_reg_cnf_sim[dest].service_state.plmn_service_capability   = mmcoord_mmr_reg_cnf_sim[src].service_state.plmn_service_capability;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.plmn_forbidden    =  mmcoord_mmr_reg_cnf_sim[src].service_state.plmn_forbidden;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.roaming_ind  =  mmcoord_mmr_reg_cnf_sim[src].service_state.roaming_ind ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.service_status    =  mmcoord_mmr_reg_cnf_sim[src].service_state.service_status  ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.active_rat    =  mmcoord_mmr_reg_cnf_sim[src].service_state.active_rat  ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.cs_service_status    =  mmcoord_mmr_reg_cnf_sim[src].service_state.cs_service_status   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.no_service_cause    =  mmcoord_mmr_reg_cnf_sim[src].service_state.no_service_cause   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.suitable_search_time    =  mmcoord_mmr_reg_cnf_sim[src].service_state.suitable_search_time   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.acq_status    =  mmcoord_mmr_reg_cnf_sim[src].service_state.acq_status   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.plmn_reg_type    =  mmcoord_mmr_reg_cnf_sim[src].service_state.plmn_reg_type   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.emc_attached    =  mmcoord_mmr_reg_cnf_sim[src].service_state.emc_attached   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.suitable_search_time    =  mmcoord_mmr_reg_cnf_sim[src].service_state.suitable_search_time   ; 
  mmcoord_mmr_reg_cnf_sim[dest].lai_forbidden = mmcoord_mmr_reg_cnf_sim[src].lai_forbidden;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].available_plmn_list,sizeof(sys_detailed_plmn_list_s_type),
                              &mmcoord_mmr_reg_cnf_sim[src].available_plmn_list,sizeof(sys_detailed_plmn_list_s_type)) ;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.active_band,sizeof(sys_active_band_u_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.active_band,sizeof(sys_active_band_u_type)) ;
  mmcoord_mmr_reg_cnf_sim[dest].update_equivalent_plmn_list = mmcoord_mmr_reg_cnf_sim[src].update_equivalent_plmn_list;
  mmcoord_mmr_reg_cnf_sim[dest].ue_oos = mmcoord_mmr_reg_cnf_sim[src].ue_oos;
  mmcoord_mmr_reg_cnf_sim[dest].only_for_reg = mmcoord_mmr_reg_cnf_sim[src].only_for_reg;
  mmcoord_mmr_reg_cnf_sim[dest].search_done = mmcoord_mmr_reg_cnf_sim[dest].search_done;
    /*  Swapping of equivalent list */
  memscpy (&equiv_plmn_list_temp,sizeof(equiv_plmn_list_s_type),&mmcoord_mmr_reg_cnf_sim[src].equivalent_plmn_list,
                    sizeof(equiv_plmn_list_s_type));
  memscpy (&mmcoord_mmr_reg_cnf_sim[src].equivalent_plmn_list,sizeof(equiv_plmn_list_s_type), &mmcoord_mmr_reg_cnf_sim[dest].equivalent_plmn_list,
                    sizeof(equiv_plmn_list_s_type));
  memscpy (&mmcoord_mmr_reg_cnf_sim[dest].equivalent_plmn_list,sizeof(equiv_plmn_list_s_type),&equiv_plmn_list_temp ,
                    sizeof(equiv_plmn_list_s_type));
/* swapping end here */ 
}
#if 0 
void mm_spliting_individually(void)
{
mm_system_information_T   mm_system_information;
mm_last_reg_dsac_information_T mm_last_reg_dsac_information;
mmr_reg_cnf_s_type   mmcoord_mmr_reg_cnf;
serving_plmn_info_T       mm_serving_plmn;
}
#endif
void mm_invalidate_cs_stack_database(void)
{
   mm_system_information_sim[mm_cs_stack_id].T3212_timer_value = 0;

   mm_system_information_sim[mm_cs_stack_id].cell_access = MM_AS_ACCESS_ALL_CALLS; 
   mm_last_reg_dsac_information_sim[mm_cs_stack_id].cell_access = MM_AS_ACCESS_ALL_CALLS; 
   mm_cnm_rel_req_received_sim[mm_cs_stack_id]          = FALSE;
   mm_held_cnm_service_req_sim[mm_cs_stack_id].present  = FALSE;
   mm_reg_waiting_for_reg_cnf_sim[mm_cs_stack_id]       = FALSE; 
   mm_ready_to_send_reg_cnf_sim[mm_cs_stack_id]         = FALSE; 
   mm_needs_to_send_mmr_service_ind_sim[mm_cs_stack_id] = FALSE; 
   mm_waiting_for_service_cnf_sim[mm_cs_stack_id]       = FALSE; 
   mmcoord_reg_req_pending_sim[mm_cs_stack_id]          = FALSE; 
   mm_waiting_for_deact_cnf_sim[mm_cs_stack_id]         = FALSE;
   mm_waiting_for_stop_mode_cnf_sim[mm_cs_stack_id]     = FALSE;
   mm_state_sim[mm_cs_stack_id] = MM_NULL;
   mm_idle_substate_sim[mm_cs_stack_id] = MM_NULL_SUBSTATE;
   mm_previous_idle_substate = NULL;
#ifdef FEATURE_WRLF_SYSTEM_SEL
   mm_received_suitable_search_end_ind = FALSE;
#endif
  mm_serving_lai_sim[mm_cs_stack_id][0] = 0xFF;
  mm_serving_lai_sim[mm_cs_stack_id][1] = 0xFF;
  mm_serving_lai_sim[mm_cs_stack_id][2] = 0xFF;
  mm_serving_lai_sim[mm_cs_stack_id][3] = 0xFF;
  mm_serving_lai_sim[mm_cs_stack_id][4] = 0xFE;
  mm_lu_attempt_counter_sim[mm_cs_stack_id] = 0; 
  mm_initiate_lu_upon_entry_into_new_cell_sim[mm_cs_stack_id] = FALSE;
  mm_follow_on_request_sim[mm_cs_stack_id]                  = FALSE;  
  mm_follow_on_proceed_sim[mm_cs_stack_id]                    = FALSE;  
  mm_T3213_timed_out_sim[mm_cs_stack_id]                      = FALSE;
  mm_idle_transition_timer_sim[mm_cs_stack_id]                = NULL; 
  mm_pending_lu_request_sim[mm_cs_stack_id]                   = NO_LU; 
  mm_pending_no_service_to_reg_sim[mm_cs_stack_id]            = FALSE;
  mm_lu_start_reason_sim[mm_cs_stack_id]                      = NO_LU;
    /* Initialize the CNM service establish cause */
  mm_cnm_srv_est_cause_sim[mm_cs_stack_id] = MM_RR_NO_CNM_SERVICE; 
  mm_connection_type_sim[mm_cs_stack_id] = NO_MM_CONNECTION;
  mm_serving_plmn_sim[mm_cs_stack_id].info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
  mm_serving_plmn_sim[mm_cs_stack_id].mm_service_state = SERVICE_NOT_AVAILABLE;
  mm_serving_plmn_sim[mm_cs_stack_id].gmm_service_state = SERVICE_NOT_AVAILABLE;
  mm_is_cs_connected_due_to_srvcc_sim[mm_cs_stack_id]         = FALSE;

}
void mm_reset_variables(mm_as_id_e_type src)
{
/* RESETTING of the variables */
  mm_initiate_lu_upon_entry_into_new_cell_sim[src] = FALSE;
  rrc_connection_status_sim[src] = FALSE;
  cs_session_status_sim[src] = MMRRC_IDLE;
  mm_held_cnm_service_req_sim[src].present = FALSE;
  mm_last_paging_cause_sim[src] = 0xFF;
  mm_integrity_protected_sim [src] = FALSE;
  mm_store_integrity_protected_sim[src] = FALSE;
  mm_is_gmm_moved_dereg_due_to_csreg_sim[src] = FALSE;   
  mm_service_ind_blocked_due_to_cs_reg_sim[src] = FALSE;
  mm_service_ind_blocked_due_to_irat_cs_reg_sim[src] = FALSE;
  mm_connection_activated_sim[src] = FALSE;
  mm_manual_mode_user_selection_procedure_sim[src] = FALSE;
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
  mm_lcs_active_sim[src] = FALSE;
#endif
  mm_connection_type_sim[src] = NO_MM_CONNECTION;
  mm_cnm_srv_est_cause_sim[src] = MM_RR_NO_CNM_SERVICE;
  mm_possible_imsi_attach_sim[src] = FALSE;
  mm_cnm_is_active_sim[src] = FALSE ;
  mm_sms_is_active_sim[src] = FALSE;
  mm_follow_on_request_sim[src] = FALSE;
  mm_follow_on_proceed_sim[src] = FALSE;
  mm_pending_lu_request_sim[src] =  FALSE;
  mm_pending_no_service_to_reg_sim[src] = FALSE;
  mm_lu_start_reason_sim[src] = NO_LU;
  mm_cnm_rel_req_received_sim[src] = FALSE;
  mm_state_sim[src] = MM_IDLE ;
  mm_idle_substate_sim[src] = MM_NORMAL_SERVICE;
  mm_plmn_search_in_progress_sim[src] = FALSE;
  mm_system_information_sim[src].T3212_timer_value = 0;
  mm_is_per_ms_t3212_present_sim[src]  = FALSE;
  mm_is_cs_connected_due_to_srvcc_sim[src]   = FALSE;
#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
  mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif

}
void mm_copy_cs_database(mm_as_id_e_type dest,mm_as_id_e_type src)
{
  uint8 i=0;
  dword remaining_timer_value = 0;
  mm_as_id_e_type as_id_local;
  mm_log_mm_reg_variables();
/* copy individual imp date first */
  mm_cpoy_individual_csdata(dest,src);
/* copy auth database as static */
  mm_copy_auth_cs_database(dest,src);
/* memcpy relate varaibles start here */
  memscpy((void *)&mm_location_information_sim[dest],sizeof(location_information_T),
               (void *)&mm_location_information_sim[src],sizeof(location_information_T));
  memscpy((void *)&mm_held_cnm_service_req_sim[dest],sizeof(held_service_request_T),
              (void *)&mm_held_cnm_service_req_sim[src],sizeof(held_service_request_T));
  memscpy((void *)&mm_lu_reject_info_sim[dest],sizeof(lu_reject_information_T),
              (void *)&mm_lu_reject_info_sim[src],sizeof(lu_reject_information_T));
  memscpy((void *)mm_serving_lai_sim[dest],LAI_SIZE,(void *)mm_serving_lai_sim[src],LAI_SIZE);  
  memscpy((void *)mm_previous_serving_lai_sim[dest],LAI_SIZE, 
                                     (void *)mm_previous_serving_lai_sim[src],LAI_SIZE);
  memscpy((void *)&mm_ef_kc_sim[dest],sizeof(ef_kc_T), 
                           (void *)&mm_ef_kc_sim[src],sizeof(ef_kc_T));
  memscpy((void *)&mm_ef_kc_sim[dest],sizeof(ef_kc_T), 
                           (void *)&mm_ef_kc_sim[src],sizeof(ef_kc_T));  
 
  memscpy((void *)&auth_gsm_cipherkey_sim[dest],sizeof(auth_gsm_cipherkey_type), 
                     (void *)&auth_gsm_cipherkey_sim[src],sizeof(auth_gsm_cipherkey_type));
  memscpy((void *)mm_sim_access_class_sim[dest],MM_SIM_EFACC_SIZE, 
                              (void *)mm_sim_access_class_sim[src],MM_SIM_EFACC_SIZE);
   /*LU reject cause for each failure attempt */
  memscpy((void *)mm_lu_reject_cause_sim[dest],MAX_LU_ATTEMPT_SIZE, 
                              (void *)mm_lu_reject_cause_sim[src],MAX_LU_ATTEMPT_SIZE);
#ifdef FEATURE_ENHANCED_NW_SELECTION
  memscpy((void *)&mm_foreground_search_lai_timer_list_sim[dest],sizeof(mm_lai_timer_info_list_info_T), 
                    (void *)&mm_foreground_search_lai_timer_list_sim[src],sizeof(mm_lai_timer_info_list_info_T));
 #endif
#ifdef FEATURE_NAS_REL10
  memscpy((void *)&mm_3gpp_t3246_plmn_id_sim[dest],sizeof(sys_plmn_id_s_type), 
                    (void *)&mm_3gpp_t3246_plmn_id_sim[src],sizeof(sys_plmn_id_s_type));
#endif
  /* This variable contains the RAND for which authentication is performing right now */
  
  memscpy((void *)&forbidden_list_sim[dest],2*sizeof (forbidden_list_T), 
                              (void*)&forbidden_list_sim[src],2*sizeof (forbidden_list_T));
  memscpy((void *)&lai_reject_list_sim[dest],sizeof(lai_reject_list_T), 
                   (void *)&lai_reject_list_sim[src],sizeof(lai_reject_list_T));

  /* memcpy variables end here */
/*###########################################################################*/
 /* All variables related to assigned  start here*/
  mm_state_sim[dest] = mm_state_sim[src];
  mm_idle_substate_sim[dest]  = mm_idle_substate_sim[src];
  mm_cnm_rel_req_received_sim[dest] =  mm_cnm_rel_req_received_sim[src];
  mm_present_all_plmns_sim[dest]  = mm_present_all_plmns_sim[src];

  mm_lu_attempt_counter_sim[dest] = mm_lu_attempt_counter_sim[src];
  mm_pending_lu_request_sim[dest] = mm_pending_lu_request_sim[src];
  mm_pending_no_service_to_reg_sim[dest] = mm_pending_no_service_to_reg_sim[src];
  mm_lu_start_reason_sim[dest] = mm_lu_start_reason_sim[src];
  mm_cipher_key_sequence_number_sim[dest] = mm_cipher_key_sequence_number_sim[src];
  mm_follow_on_request_sim[dest] = mm_follow_on_request_sim[src];
  mm_follow_on_proceed_sim[dest] = mm_follow_on_proceed_sim[src];
  mm_reject_cause_sim[dest] = mm_reject_cause_sim[src];
  mm_cnm_is_active_sim[dest] = mm_cnm_is_active_sim[src];
  mm_sms_is_active_sim[dest] = mm_sms_is_active_sim[src];
  mm_T3213_timed_out_sim[dest] = mm_T3213_timed_out_sim[src];
  mm_idle_transition_timer_sim[dest] = mm_idle_transition_timer_sim[src];


  mm_possible_imsi_attach_sim[dest]  = mm_possible_imsi_attach_sim[src];
  mm_ef_kc_valid_sim[dest]  = mm_ef_kc_valid_sim[src];
  mm_integrity_protected_sim[dest]  = mm_integrity_protected_sim[src];
  mm_dual_mode_sim[dest]  = mm_dual_mode_sim[src];
  mm_band_pref_sim[dest]  = mm_band_pref_sim[src];
  mm_cnm_srv_est_cause_sim[dest]  = mm_cnm_srv_est_cause_sim[src];
  mm_connection_type_sim[dest]  = mm_connection_type_sim[src];
  mm_ue_band_class_pref_sim[dest]  = mm_ue_band_class_pref_sim[src];
  mm_active_band_sim[dest]  = mm_active_band_sim[src];
  mm_band_indicator_sim[dest]  = mm_band_indicator_sim[src];
  mm_lu_pended_nmo_unknown_sim[dest]  = mm_lu_pended_nmo_unknown_sim[src];

  mm_previous_idle_substate_sim[dest]  = mm_previous_idle_substate_sim[src];
  mm_prev_cell_access_sim[dest]  = mm_prev_cell_access_sim[src];
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
  mm_lcs_active_sim[dest]  = mm_lcs_active_sim[src];
#endif
#ifdef FEATURE_CCBS
  mm_supports_NIMO_CM_connection_sim[dest] = mm_supports_NIMO_CM_connection_sim[src];
#endif
  mm_initiate_lu_upon_entry_into_new_cell_sim[dest] = mm_initiate_lu_upon_entry_into_new_cell_sim[src];
  mm_serving_cell_id_sim[dest] = mm_serving_cell_id_sim[src];
  mm_serving_cell_valid_sim[dest] = mm_serving_cell_valid_sim[src];
  mm_lu_failed_cell_id_sim[dest] = mm_lu_failed_cell_id_sim[src];
#ifdef FEATURE_DSAC
  mm_cs_access_barred_to_unbarred_sim[dest] = mm_cs_access_barred_to_unbarred_sim[src];
  mm_last_procedure_to_update_registration_sim[dest] = mm_last_procedure_to_update_registration_sim[src];
#endif 
  mm_manual_mode_user_selection_procedure_sim[dest] = mm_manual_mode_user_selection_procedure_sim[src];
#ifdef FEATURE_ENHANCED_NW_SELECTION
  mm_force_lu_sim[dest] = mm_force_lu_sim[src];
  mm_foreground_search_timer_value_sim[dest] = mm_foreground_search_timer_value_sim[src];

#endif
  mm_mmr_cell_service_ind_pending_sim[dest] = mm_mmr_cell_service_ind_pending_sim[src];
  mm_imsi_is_native_sim[dest] = mm_imsi_is_native_sim[src];
/* Flag to indicate whether GSM access is allowed or not */
  auth_gsm_access_sim[dest] = auth_gsm_access_sim[src];
/* GSM security context for USIM */
  auth_usim_gsm_security_context_sim[dest] = auth_usim_gsm_security_context_sim[src];
/* Flag to remember last network selection mode send to RR/RRC */
  last_requested_network_selection_mode_sim[dest] = last_requested_network_selection_mode_sim[src];
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
  mm_pending_lu_type_sim[dest] = mm_pending_lu_type_sim[src];
#endif
  mm_service_ind_blocked_due_to_cs_reg_sim[dest] = mm_service_ind_blocked_due_to_cs_reg_sim[src];
  mm_service_ind_blocked_due_to_irat_cs_reg_sim[dest] = mm_service_ind_blocked_due_to_irat_cs_reg_sim[src];
  mm_connection_activated_sim[dest] = mm_connection_activated_sim[src];
  mm_is_foreground_search_to_be_initiated_sim[dest] =  mm_is_foreground_search_to_be_initiated_sim[src];
  mm_is_gmm_moved_dereg_due_to_csreg_sim[dest] = mm_is_gmm_moved_dereg_due_to_csreg_sim[src];
#ifdef FEATURE_NAS_REL10

  mm_store_integrity_protected_sim[dest] = mm_store_integrity_protected_sim[src];
  mm_t3246_backoff_timer_value_sim[dest] = mm_t3246_backoff_timer_value_sim[src];
  mm_stored_per_ms_t3212_value_sim[dest] = mm_stored_per_ms_t3212_value_sim[src];
  mm_is_per_ms_t3212_present_sim[dest] = mm_is_per_ms_t3212_present_sim[src];
#endif
  mm_last_paging_cause_sim[dest] = mm_last_paging_cause_sim[src];
  mm_cell_changed_to_cs_capable_sim[dest] = mm_cell_changed_to_cs_capable_sim[src];
  lu_cause_sim[dest] = lu_cause_sim[src];
/* Declare mm_plmn_search_in_progress variable*/
  mm_plmn_search_in_progress_sim[dest] = mm_plmn_search_in_progress_sim[src];
  pending_mm_msg_after_release_sim[dest] = pending_mm_msg_after_release_sim[src];
  mm_cs_domain_possible_reest_sim[dest] = mm_cs_domain_possible_reest_sim[src];
  pending_mm_msg_valid_sim[dest] = pending_mm_msg_valid_sim[src];
  send_sequence_number_mm_cc_ss_sim[dest] = send_sequence_number_mm_cc_ss_sim[src];
/* newly Added copied variables */
  rrc_connection_status_sim [dest] = rrc_connection_status_sim[src];
  cs_session_status_sim[dest] = cs_session_status_sim[src];

  as_id_local = mm_as_id;
  /* resetting of the variables end here */
  /* Move CS timers */
  for (i=MIN_MM_TIMER; i < MAX_MM_TIMER+1; i++)
  {
     mm_as_id = src;
     if ((mm_timer_status_sim[src][i] != TIMER_STOPPED) && mm_is_cs_timer(i))
     {
       /* Find out how much timeout is left */
       remaining_timer_value =(dword) gs_enquire_timer_ext ( 
                                                           get_mm_timer_queue(),
                                                           i,
                                                           mm_timer_non_deferrable(i)
                                                         ); 
       MSG_HIGH_DS_3(MM_SUB, "=MM= Moving CS timer from SRC to DEST : remaining time left before expiry %d, src as id = %d, Dest as id = %d",remaining_timer_value,src,dest );
       mm_stop_timer(i);
       mm_as_id = dest;
       mm_start_timer(i,remaining_timer_value);
     }
  }
  mm_as_id = as_id_local;
  mm_coord_camped_cell_id_sim[dest] = mm_coord_camped_cell_id_sim[src];
  mm_is_cs_connected_due_to_srvcc_sim[dest] = mm_is_cs_connected_due_to_srvcc_sim[src];
  /* Assigned end here */
}
#endif 

#if defined(FEATURE_SGLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
rex_timer_cnt_type mm_get_ps_timer_value(void)
{
 return (sglte_nas_mm_config.ps_timer_value *5000);
}
#endif

 /*===========================================================================
FUNCTION  mm_read_carrier_specific_efs

DESCRIPTION
  This function reads carrier specific EFS/NV values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_read_carrier_specific_efs(void)
{
  mcfg_fs_status_e_type  status[3] = {MCFG_FS_STATUS_EFS_ERR,MCFG_FS_STATUS_EFS_ERR,MCFG_FS_STATUS_EFS_ERR};
  cm_operator_e_type operator_name = OPERATOR_NULL;
#ifdef FEATURE_LTE
  /* EXCLUDE OLD LAI TYPE */
  dog_hb_report(DOG_MM_RPT);
  status[0]  = mcfg_fs_read("/nv/item_files/modem/nas/exclude_old_lai_type_field", 
                          &mm_nv_exclude_old_lai_type_ie, sizeof(boolean),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status[0] != MCFG_FS_STATUS_OK)
  {
    mm_nv_exclude_old_lai_type_ie = FALSE;
  }

  dog_hb_report(DOG_MM_RPT);
  status[1] = mcfg_fs_read("/nv/item_files/modem/mmode/extend_lte_disable_duration", 
                         &mm_extend_lte_disable_duration, sizeof(uint32),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status[1] != MCFG_FS_STATUS_OK)
  {
    mm_extend_lte_disable_duration = MM_DEFAULT_LTE_DISABLE_DURATION;
  }
  
  MSG_HIGH_DS_4(MM_SUB,"=MM= exclude_old_lai_type_ie (status %d. value %d) extend_lte_disable_duration (status %d. value %d) suppress_additional_update_param (status %d. value %d)",
                       status[0],
                       mm_nv_exclude_old_lai_type_ie,
                       status[1],
                       mm_extend_lte_disable_duration);
  dog_hb_report(DOG_MM_RPT);
  status[1] = mcfg_fs_read("/nv/item_files/modem/nas/suppress_add_update_param",
                          &mm_suppress_additional_update_param,
                          sizeof(boolean),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  MSG_HIGH_DS_1(MM_SUB,"=MM= Suppress add update param status : %d ",status[1]);

  if (status[1] != MCFG_FS_STATUS_OK)
  {
    mm_suppress_additional_update_param = FALSE;
  }
#endif

  /* CONN MODE MANUAL SEARCH */
  dog_hb_report(DOG_MM_RPT);
  status[0] = mcfg_fs_read("/nv/item_files/modem/nas/conn_mode_manual_search", 
                         &mm_nv_conn_mode_manual_search, sizeof(boolean),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status[0] != MCFG_FS_STATUS_OK)
  {
    mm_nv_conn_mode_manual_search = FALSE;
  }
  
  MSG_HIGH_DS_2(MM_SUB,"=MM= conn_mode_manual_search efs read failed, status: %d, default %d",
                         status[0],mm_nv_conn_mode_manual_search);
  
  mm_conn_mode_manual_search_enabled = mm_nv_conn_mode_manual_search;

#if defined(FEATURE_SGLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
  dog_hb_report(DOG_MM_RPT);
  status[1] = mcfg_fs_read("/nv/item_files/modem/nas/sglte_nas_nv_config", 
                         &sglte_nas_mm_config, sizeof(nas_sglte_feature_config_T),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status[1] != MCFG_FS_STATUS_OK)
  {
    sglte_nas_mm_config.ps_timer_value = 5;  
  }

  MSG_HIGH_DS_2(MM_SUB ,"sglte_nas_nv_config efs read failed, status: %d. default PS timer value %d",
                            status[1],sglte_nas_mm_config.ps_timer_value);
#endif

#ifdef FEATURE_FEMTO_CSG
  dog_hb_report(DOG_MM_RPT);
  status[2] = mcfg_fs_read("/nv/item_files/modem/nas/csg_support_configuration", 
                         &mm_nv_csg_support, sizeof(sys_csg_support_e_type),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status[2] != MCFG_FS_STATUS_OK)
  {
#ifdef FEATURE_DISABLE_CSG_BYDEFAULT
    mm_nv_csg_support = SYS_CSG_SUPPORT_DISABLED;
#else
    mm_nv_csg_support = SYS_CSG_SUPPORT_WCDMA;
#endif
  }

  MSG_HIGH_DS_2(MM_SUB ,"mm_nv_csg_support efs read failed, status: %d. default %d",
                            status[2],mm_nv_csg_support);
#endif /*FEATURE_FEMTO_CSG*/

    mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
    mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
	operator_name = OPERATOR_NULL;
	
	   if(mcfg_fs_read("/nv/item_files/modem/mmode/operator_name", 
						 &operator_name, sizeof(cm_operator_e_type),
						 MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id) == MCFG_FS_STATUS_OK)
	   {
		 MSG_HIGH_DS_1(MM_SUB, "=MM= Setting operator to %d",operator_name);
	   }
	   else
	   {
		 operator_name = OPERATOR_NULL;
		 MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting operator to NULL %d",operator_name);
	   }
    /* Read the UE Revision control NV item */

    if (mm_ghdi_nvmem_read(NV_FORCE_UE_SGSNR_R99_I,
            (ghdi_nvmem_data_T *)&mm_nv_force_sgsnr_r99) != GHDI_SUCCESS)
    {
      if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
      {
        mm_nv_force_sgsnr_r99 = NV_FORCE_UE_DYNAMIC;
      }
      else
      {
        mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
      }
    }
    else if (mm_nv_force_sgsnr_r99 >= NV_REL_MODE_MAX)
    {
      if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
      {
        MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_SGSNR_R99 NV %d",
              mm_nv_force_mscr_r99,0,0);
        mm_nv_force_sgsnr_r99 = NV_FORCE_UE_DYNAMIC;
      }
      else
      {
        MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_SGSNR_R99 NV %d, default R 99",
          mm_nv_force_sgsnr_r99,0,0);
     mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
      }
    }

    if (mm_ghdi_nvmem_read(NV_FORCE_UE_MSCR_R99_I,
            (ghdi_nvmem_data_T *)&mm_nv_force_mscr_r99) != GHDI_SUCCESS)
    {
      if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
      {
        mm_nv_force_mscr_r99 = NV_FORCE_UE_DYNAMIC;
      }
      else
      {
        mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
      }
    }
    else if (mm_nv_force_mscr_r99 >= NV_REL_MODE_MAX)
    {
      if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
      {
        MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_MSCR_R99 NV %d",
                          mm_nv_force_mscr_r99,0,0);
        mm_nv_force_mscr_r99 = NV_FORCE_UE_DYNAMIC;
      }
      else
      {
        MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_MSCR_R99 NV %d, default R99",
                          mm_nv_force_mscr_r99,0,0);
        mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
      }
    }
    /*
    Update mm_nv_nas_rel_compliance as '0' when either of NV_FORCE_UE_SGSNR_R99_I
    or NV_FORCE_UE_SGSNR_R99_I indicate '0'.
    */
    if((mm_nv_force_sgsnr_r99 == NV_FORCE_UE_REL97) || (mm_nv_force_mscr_r99 == NV_FORCE_UE_REL97))
    {
      mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL99;
    }
    MSG_HIGH_DS_3(MM_SUB, "=MM= NV_FORCE_UE_SGSNR_R99 : %d, NV_FORCE_UE_MSCR_R99_I : %d, NV_NAS_REL_COMPLIANCE : %d",
                  mm_nv_force_sgsnr_r99, mm_nv_force_mscr_r99, mm_nv_nas_rel_compliance);

}

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION mm_process_mcfg_refresh

DESCRIPTION
  This function processes MCFG NV refresh indication for MM specific EFS/NVs

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/
void mm_process_mcfg_refresh(void)
{
  mm_read_carrier_specific_efs();

  /* NV_MGRF_SUPPORTED_I */
  if(mm_ghdi_nvmem_read(NV_MGRF_SUPPORTED_I,
     (ghdi_nvmem_data_T *)&mm_managed_roaming_enabled) != GHDI_SUCCESS)
  {
    mm_managed_roaming_enabled = FALSE;  /* Set the default value */
    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to read NV for MANAGED ROAMING flag; disabled");
  }
  else
  {
    if((mm_managed_roaming_enabled != FALSE) && (mm_managed_roaming_enabled != TRUE))
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= Wrong value for MANAGED ROAMING flag %d", mm_managed_roaming_enabled);
      mm_managed_roaming_enabled = FALSE;  /* Set the default value */
    }
  }
#if (defined FEATURE_SGLTE) || (defined FEATURE_1XSRLTE)
  mm_current_ue_mode = SYS_UE_MODE_NONE; 
#endif
  MSG_HIGH_DS_0(MM_SUB,"MM Refresh Done");
}
#endif

