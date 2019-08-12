#ifndef _MM_V_H_
#define _MM_V_H_
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm.h_v   1.47   17 Jul 2002 12:29:30   kishores  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/12   am      Back off timer code
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE 
10/28/11   rajesh  extern declaration for "m_lai_change_force_lau_for_emergeny".
10/15/11   rajesh  Added "include_additional_update_ie" param in "held_service_request_T".
09/30/11   rajesh  Added exclude_identity_type_ie feild in emm_ctrl_data_type structure.
08/01/11   rajesh  Added t3442_value feild in emm_ctrl_data_type structure.
07/13/11   rajesh  Handling expiry of T3412 and T3423 when ISR is ON
07/05/11   rajesh Added prototype for "mm_service_ind_blocked_due_to_cs_reg"
07/14/11   abhi   Added ph status change confirm pending flag in emm ctrl data
06/24/11   rajesh  Updated prototype of mm_send_rrc_disable_bplmn_ind function 
06/16/11   PA      Added changes for Rel 9 Emergency Bearer services 
04/04/01   jca     Added missing includes to quiet LINT.
04/12/01   jca     Changed mm_handle_message() prototype.
04/24/01   sbk     Added is_of_mm_msg_type prototype
05/03/01   jca     Added new function prototypes to support MMCOORD.
05/11/01   jca     Added GMM timers.
05/25/01   jca     Updates to reflect changes to REG<->MM interface.
06/03/01   sbk     Global variables are renamed to use mm_ prefix.
06/08/01   jca     Added new state change event to support MM/GMM mode change.
06/18/01   jca     Added function mm_send_mmr_act_cnf().
08/03/01   jca     Modified mm_send_mmr_reg_cnf() and mm_send_mmr_service_ind().
08/15/01   ab      Renamed CM to CNM.
08/22/01   jca     Removed some errant CNM renames.
09/18/01   sbk     Added prototypes for the logging functions
10/11/01   jc      Add Reserved signal for GSDI library.
10/19/01   sbk     New prototypes for plmn matching and handling of T3212
11/08/01   jca     Added MM_STD_AUTHENTICATION_REJECT event code.
02/01/02   jca     Renamed MM_STD_ABORT_RECEIVED -> MM_STD_ABORT_MESSAGE_RECEIVED.
04/15/02   ks      Added mm_sig_low_priority and nreg_release_pending variables.
04/23/02   jca     Deleted active_transaction_T.
                   Deleted unused function prototypes.
04/24/02   vdr     Added domain ID prototype
04/24/02   vdr     Removed unused domain ID prototype
06/05/02   ks      Added support for Dual mode
06/26/02   ks      Added mscr field in mm_system_information_T
07/01/02   jca     Removed functions mm_query_timer() and mm_handle_t3212_enquiry().
07/03/02   jca     Deleted mm_plmn_list_state variable and function mm_plmn_list_control().
07/09/02   mks     Added function mm_send_rr_sim_update_req().
07/11/02   ks      Corrected the data type for mm_ue_pref_rat variable. Added
                   sys_radio_access_tech_e_type parameter in mm_request_list_from_rr.
07/17/02   ks      Defined mm_gsm_supported and mm_wcdma_supported global
                   variables
07/22/02   ks      Added variables and function declarations to support the
                   dynamic RAT change and Service domain in the UE.
08/02/02   ks      Added new prototype for mm_convert_to_reg_cause() and modified
                   mmr_service_ind with input parameter mmr_service_ind_s_type.
08/03/02   mks     Added function declarations to support inter-RAT handover/
                   cell reselection procedures
08/08/02   jca     Deleted some MM timer functions and variables.
08/13/02   mks     Added function prototype for mm_has_valid_ptmsi().
08/13/02   mks     Added mm_hi_access_class & mm_lo_access_class to cache UE
                   access class information.
08/13/02   mks     Added function prototype mm_send_rrc_sim_update_req () &
                   mm_send_rrc_sim_inserted_req (), moved inter-RAT related
                   function declarations to mmrat.h
08/15/02   ks      replaced the input parameter in mm_request_list_from_rr
                   with mmr_plmn_search_req_s_type* type
08/26/02   mks     Added function prototype mm_send_rrc_security_update_req()
                   to notify security updates to RRC
09/04/02   vdr     Added definition of the security context type
10/07/02   jca     Changes to support combined CS/PS attach procedures.
10/11/02   cd      Changed prototype of mm_send_mmcnm_est_rej to include rej cause
11/06/02   jca     Added function mm_update_mm_serving_plmn_info().
11/07/02   mks     Added new function prototype mm_get_lu_cause ().
11/08/02   cd      Added access_class_restriction to mm_system_information_T
11/12/02   jca     Added domain_id parameter to function mm_abort_rr().
11/14/02   vdr     Added new function prototype mm_send_ps_cnm_data()
11/14/02   ks      Added SGSNR in mm_system_information
11/14/02   ks      Added extern boolean mm_deep_sleep_pending for deep sleep.
11/26/02   jca     Modified prototype for function mm_update_mm_serving_plmn_info().
12/12/02   ks      Removed mm_sim_valid field, as it is no longer used.
12/16/02   jca     Renamed mm_waiting_to_send_reg_cnf to mm_ready_to_send_reg_cnf.
01/10/03   vdr     Removed prototype of mm_get_ms_security_context()
02/07/03   jca     Added extern boolean mm_dual_mode.
02/10/03   cd      Fixed Lint errors
02/13/03   jca     Added extern boolean mm_waiting_for_service_cnf.
02/21/03   vdr     Changed mm_confirm_mm_connection() interface
02/21/03   vdr     Added prototype for mm_sms_preferred_domain_id var
04/03/03   cd      Added equivalent plmn list to the mm_serving_plmn structure
04/08/03   jca     Added new reason code for pending LU request.
04/15/03   cd      Added new reason code for pending LU request
07/08/03   jca     Added function prototype mm_rat_handle_srv_status_update().
07/31/03   jca     Added input parameter to function mm_activate_state() to indicate
                   whether call to function was due to RAT change.
08/28/03   ks      Added variables mm_nv_force_sgsnr_r99 & mm_nv_force_mscr_r99;
08/29/03   ks      Added extern variable gmm_anite_gcf_flag for R97 GCF testing
09/10/03   ks      Added Enumeration ue_force_rel_mode that is used along with NV
                   items to give flexibility to force UE Revision behavior.
09/15/03   cd      Added prototype for get_preferred_band_class() and get_active_band()
09/19/03   jca     Added prototypes for mm_convert_semi_octets() and
                   mm_convert_time_zone( byte value ).
09/19/03   mks     Merged Kurt's changes to support silent redial feature - Added a
                   new MM state change event MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED.
10/22/03   jca     Moved network_op_mode_type from mm.h to sys_stru.h.
11/25/03   jca     Externalized regional variable rrc_conn_est_in_progress.
01/28/04   jca     Added regional variables mm_previous_serving_lai and
                   mm_previous_idle_substate.
03/24/04   jca     Added regional prototype for function mm_is_lai_forbidden().
04/06/04   cd      LCS support
04/06/04   cd      Added include for mmrat.h
04/19/04   jca     Corrected errors introduced by new AMSS LINT config file.
05/07/04   ks      Replaced the data type from boolean to ue_force_rel_mode_type
                   for variables mm_nv_force_sgsnr_r99 & mm_nv_force_mscr_r99
05/12/04   cd      Added support for WCDMA multiband feature
                   Removed prototype for obsolete function mm_get_active_rat()
05/14/04   cd      Enhancements to CS FOR
06/01/04   jca     Added boolean mm_needs_to_send_mmr_service_ind.
06/08/04   cd      LCS support. Externalize mm_lcs_active
07/07/04   cd      Deleted function prototype for mm_get_ef_kc
07/30/04   jca     Added regional variable mm_band_pref.
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
10/06/04   ks      Added prototype for mmr_send_mmr_camped_ind() & mm_get_ef_kc()
12/17/04   vdr     Changed prototypes for mm_send_rrc_sim_update_req() &
                   mm_send_rrc_security_update_req()
02/14/05   sm      Added LOCATION_AREA_NOT_ALLOWED enum for
                   SIM Location update status
02/18/05   sm      Added support for EVENT_RAT_CHANGE
03/31/05   hj      Added support for CM_CIPHER_IND
04/12/05   hj      Changed declaration of mm_send_cm_cipher_ind to reflect new
                   enum type for cipher_domain.
04/14/05   jca     Added boolean flag mm_waiting_for_stop_mode_cnf.
04/20/05   jca     Added boolean flag mm_waiting_for_deact_cnf.
05/31/05   abe     Added boolean flag mm_sms_is_active.
06/06/05   abe     Added code for CCBS support
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT,
                   EVENT_SMGMM_REJECT_RECEIVED.
07/22/05   up      Added function mm_send_rrc_eq_plmn_list_change_ind and
                   mm_send_rr_eq_plmn_list_change_ind.
07/23/05   abe     Added variable mm_previous_lai_during_wtog_cco & WtoGCCO_in_progress
                   under the feature FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
07/26/05   abe     Changed the enum CCO_SUCCESS to CCO_COMPLETE in WtoGCCO_in_progress_enum_T
08/08/05   abe     extern definition for the variable mm_hplmn_from_imsi
                   is removed for the feature FEATURE_ENHANCED_NW_SELECTION.
08/23/05   kvk     Adedd prototype for mm_send_cm_ps_data_suspend_ind()
09/09/05   ajt     Added prototype for mm_prev_cell_access
09/16/05   vdr     Added INTER RAT reselection status into held_service_request_T
09/16/05   ajt     Prototypes for mm_send_mmr_emergency_num_list_ind, and mm_parse_emergency_number,
                   mm_nullify_mmr_emergency_num_list_if_needed, mm_update_mmr_emergency_num_list_if_needed
                   and is_plmn_country_changed functions.
10/06/05   abe     Added definition for MM_STD_CCO_FAILED
10/08/05   abe     Added support for FEATURE_INTERRAT_PCCO_GTOW
11/11/05   abe     ue_force_rel_mode_type is moved to sys_cnst.h.
                   Added definition for ue_rev_level_indicator_type.
11/30/05   jca     Modified mmsend_request_reject_event() function prototype for
                   strong type checking and eliminating LINT errors.
12/14/05   abe     Supported changes for FEATURE_GSM_DTM
01/02/06   up      Added mm_send_rrc/rr_bplmn_search_abort_req to send abort 
                   request to RRC/RR to abort BPLMN Search.
03/13/06   abe     Added mm_process_lcs_ue_pos_stored_info() for handling UE Postioning Info
04/06/06   sn      Added mm_send_mmr_cell_service_ind
05/09/06   up      Added function mm_send_rrc_change_mode_ind.
06/14/06   up      Modified prototype mm_update_mm_serving_plmn_info
                   to block/unblock GSTK event.
06/15/06   nr      Modified prototype mm_notify_reestablishment_fail
                   mm_send_mmcc_reest_rej and added mm_get_mmcc_rej_cause.
06/23/06   up      mm_gs_association_established is added.
07/14/06   pk     Updated for Feature DSAC
07/21/06   up      Added new LU reject cause CAUSE_IMM_ASSIGN_REJECT and
                   LU pending reason T3122_EXPIRED.
08/28/06   sn      Added functions for LAI reject list handling.
03/21/07   NR      Removing mm_cnm_service_type global variable
04/09/07   RD      SMS optimization changes are incorporated
11/30/09   zren    Modified access barring timeout handling
12/18/09   RI      Modifications for Multimode EFS/NV/USIM globals
01/22/10   RI      Updated the EPS security context
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
04/09/10   zren    Added combined procedure feature
04/22/10   zren    Updated to use global structure for NAS outgoing message
05/07/10   zren    Added temp FPLMN feature
05/11/10   RI      Mainlined spec changes upto Dec 09. 
06/21/10   zren    Updated to not include UE network capability field in TAU 
                   Request for periodic TAU
06/21/10   zren    Updated EMM to include correct MME routing info in conn est 
                   req for TAU after IRAT
08/30/10   MNK     CSFB changes 
09/30/10   MNK     pended_esr_msg changed to a pointer 
10/07/10   MNK     1XCSFB introduction 
08/25/11   jbk       API to check if PLMN ID is valid
10/12/11   sla     Declared new variable mm_is_foreground_search_to_be_initiated
12/06/11   gps     Remembering DSAC information of the cell where last registration was attempted
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "assert.h"
#include "sys.h"
#include "sys_v.h"
#include "gs_v.h"
#include "mmtask_int.h"
#include "com_iei_v.h"
#include "mm_mm_v.h"
#include "naslog_v.h"
#include "sys_plmn_selection_v.h"
#include "mm.h"
#include "reg_sim_v.h"
#include "timers_v.h"
#include "rrccmd.h"
#include "reg_cm.h"
#ifdef FEATURE_LTE
#include "lte_nas.h"
#include "emm_connection_type.h"
#include "emm_state.h"
#include "lte_nas_emm_message_description.h"
#include "lte_nas_esm_message_type.h"
#ifdef FEATURE_LTE_REL9
#include "lte.h"
#include "emm_esm_if_msg.h"
#endif
#endif
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define EQUIVALENT_PLMN_LIST_MAX_LENGTH 15
#define MM_CMD_Q_SIG         0x0001 /* Command queue signal               */
#define MM_DOGRPT_TIMER_SIG  0x0002 /* Watchdog report signal             */
#define MM_NVIF_SIG          0x0004 /* NV interface signal                */

#define MM_RESERVED_SIG      0x1000 /* In use by GSDI library             */

#ifdef FEATURE_FEMTO_CSG
#define MM_TIMER_DISABLED ~0
#endif 
/*
 * Useful definitions
 */
 
#define QWORD_LOW( qword ) ( (dword) ((uint64) qword & (uint64) 0xFFFFFFFF))
#define QWORD_HIGH( qword ) ( (dword) ((uint64) qword >> 32) )
 
#define TIMER_NAME(id) \
(id==TIMER_T3210?"TIMER_T3210": \
(id==TIMER_T3211?"TIMER_T3211": \
(id==TIMER_T3212?"TIMER_T3212": \
(id==TIMER_T3213?"TIMER_T3213": \
(id==TIMER_T3220?"TIMER_T3220": \
(id==TIMER_T3230?"TIMER_T3230": \
(id==TIMER_T3240?"TIMER_T3240": \
(id==TIMER_MM_REL?"TIMER_MM_REL": \
(id==TIMER_MM_FORBIDDEN_LAI_CLEAR?"TIMER_MM_FORBIDDEN_LAI_CLEAR": \
(id==TIMER_T3302?"TIMER_T3302": \
(id==TIMER_T3310?"TIMER_T3310": \
(id==TIMER_T3311?"TIMER_T3311": \
(id==TIMER_T3312?"TIMER_T3312": \
(id==TIMER_T3314?"TIMER_T3314": \
(id==TIMER_T3317?"TIMER_T3317": \
(id==TIMER_T3318?"TIMER_T3318": \
(id==TIMER_T3320?"TIMER_T3320": \
(id==TIMER_T3321?"TIMER_T3321": \
(id==TIMER_T3330?"TIMER_T3330": \
(id==TIMER_POWERDOWN?"TIMER_POWERDOWN": \
(id==TIMER_T3402?"TIMER_T3402": \
(id==TIMER_T3410?"TIMER_T3410": \
(id==TIMER_T3411?"TIMER_T3411": \
(id==TIMER_T3416?"TIMER_T3416": \
(id==TIMER_T3417?"TIMER_T3417": \
(id==TIMER_T3418?"TIMER_T3418": \
(id==TIMER_T3420?"TIMER_T3420": \
(id==TIMER_T3421?"TIMER_T3421": \
(id==TIMER_T3430?"TIMER_T3430": \
(id==EMM_POWEROFF_DETACH_TIMER?"EMM_POWEROFF_DETACH_TIMER": \
(id==EMM_ACCESS_BARRING_TIMER?"EMM_ACCESS_BARRING_TIMER": \
"Unknown MM timer" ))))))))))))))))))))))))))))))))))))))



extern boolean                   mm_gsm_supported;
extern boolean                   mm_wcdma_supported;
extern boolean                   mm_tdscdma_supported;

/*
 * -------- global variable mmode_nv_setting_for_device_mode --------------
 *
 * This flag tells the choice for this device - whether the device is DSDS or DSDA capable
 * This will be set after reading MMODE EFS - /nv/item_files/modem/mmode/device_mode
 */
extern sys_modem_device_mode_e_type   mmode_nv_setting_for_device_mode;
#define MMCNM_IS_MODE_DUAL_STANDBY   ((mmode_nv_setting_for_device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY) || \
                                        (mmode_nv_setting_for_device_mode == SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY))
#define MMCNM_IS_MODE_DUAL_ACTIVE    (mmode_nv_setting_for_device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)

typedef void(mm_emm_dispatcher_fp)(rex_sigs_type,void**);


#ifdef FEATURE_SGLTE_DUAL_SIM
#define MM_SGLTE_STACK_2 MM_AS_ID_3
#else
#define MM_SGLTE_STACK_2 MM_AS_ID_2
#endif 


/*
 * Define macro for trivial function
 */

#define mm_clear_lu_attempt_counter( ) \
      mm_lu_attempt_counter = 0;

extern void mm_clear_regs_complete_counter(void) ;


void mm_emm_normal_msg_handler(rex_sigs_type sigs,void **mm_process_msg_fn_ptr);

void mm_emm_oos_rlf_msg_handler(rex_sigs_type sigs,void **mm_process_msg_fn_ptr);

void mm_emm_irat_msg_handler(rex_sigs_type sigs,void **mm_process_msg_fn_ptr);

/* Macro for controlling modem statistics operations on first subscription */
#define MM_MODEM_STATISTICS_SUB1\
                               (SYS_MODEM_AS_ID_1 == (sys_modem_as_id_e_type)MM_SUB) || (mm_dual_standby_mode == FALSE)

#if defined  FEATURE_LTE
#define MM_DEFAULT_LTE_DISABLE_DURATION  43200000   //12 hrs in milliseconds
#endif
#if (defined FEATURE_DUAL_SIM)
extern boolean mm_dual_standby_mode;
#endif

#if (defined FEATURE_SGLTE) || (defined FEATURE_1XSRLTE) ||(defined FEATURE_SVLTE_DUAL_SIM)
extern sys_ue_mode_e_type mm_current_ue_mode ;
#endif

#ifdef FEATURE_SGLTE
extern boolean mm_sglte_mode;
extern boolean mm_is_sglte_stack_merged;
#endif

#ifdef  FEATURE_SGLTE   /*  MM_SGLTE_STACK_2 use as stackid for the hybrid stack*/
#define HYBRID_STACK_DATA_COPY(varname,val)  { varname=val;\
                                               if(mm_as_id == mm_ps_stack_id) \
                                               { \
                                                *(&varname +(MM_SGLTE_STACK_2-mm_sub_id))=val;\
                                               } \
                                             }                                               


#define HYBRID_STACK_DATA_MEMS_COPY(dest,size_dest,src,src_size) { memscpy((void *)dest,size_dest,(void*)src,src_size);\
                                                                   if(mm_as_id == mm_ps_stack_id) \
                                                                   {\
                                                                     memscpy((void*)(dest+MM_SGLTE_STACK_2-mm_sub_id),size_dest,(void *)src,src_size);\
                                                                   }\
                                                                 }
                                                                 
#define MM_IS_IN_SGLTE_MODE   (MM_SUB_IS_SGLTE && mm_sglte_mode==TRUE)
#ifdef FEATURE_DUAL_SIM
#define MM_IS_IN_SGLTE_MODE_SUB(x)   (MM_SUB_IS_SGLTE_SUB(x) && mm_sglte_mode==TRUE)
#endif
#define MM_IS_NOT_IN_SGLTE_MODE (!MM_SUB_IS_SGLTE || mm_sglte_mode==FALSE)  
#define MM_IS_CSFB_OR_NORMAL_MODE (MM_IS_NOT_IN_SGLTE_MODE && ((mm_current_ue_mode==SYS_UE_MODE_CSFB_ONLY)|| \
                                                                mm_current_ue_mode==SYS_UE_MODE_NORMAL))
#define MM_IS_SGLTE_ON_CS_STACK_ID  (!MM_IS_IN_SGLTE_MODE || (mm_as_id==mm_cs_stack_id))              
#define MM_IS_SGLTE_ON_PS_STACK_ID (!MM_IS_IN_SGLTE_MODE ||(mm_as_id==mm_ps_stack_id))
#define MM_IS_IN_SGLTE_STATE_4 (MM_IS_IN_SGLTE_MODE && (mm_cs_stack_id == mm_ps_stack_id))
#define MM_IS_IN_SGLTE_DUAL_STACK_MODE (MM_IS_IN_SGLTE_MODE && (mm_cs_stack_id != mm_ps_stack_id))
#define NAS_MAX_SGLTE_STACK 2
 typedef enum
 {
   SGLTE_MM_UPPER_LAYER_MERGING,
   SGLTE_MM_UPPER_LAYER_SPLITING,
   SGLTE_MM_LOWER_LAYER_MERGING,
   SGLTE_MM_LOWER_LAYER_SPLITING,
   SGLTE_INACTIVE_STATE
 }mm_sglte_stack_activity_status;
#endif


#if defined( FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)

extern mm_system_information_T   mm_system_information_sim[MAX_NAS_STACKS];
extern imsi_data_T               mm_stored_imsi_sim[MAX_NAS_STACKS];
#define  mm_system_information (mm_system_information_sim[mm_as_id])
#define  mm_stored_imsi (mm_stored_imsi_sim[mm_as_id])
#else
extern mm_system_information_T   mm_system_information;
extern imsi_data_T               mm_stored_imsi;
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern sys_overall_feature_t mm_sub_config_sim[MAX_AS_IDS];
#define mm_sub_config mm_sub_config_sim[mm_sub_id]

#define MM_SUB_IS_SGLTE  (mm_sub_config == SYS_OVERALL_FEATURE_MODE_SGLTE)
#define MM_SUB_IS_SGLTE_SUB(x)  (mm_sub_config_sim[x] == SYS_OVERALL_FEATURE_MODE_SGLTE)
#endif

#if defined(FEATURE_LTE) && defined(FEATURE_DUAL_SIM)
extern boolean mm_suppress_additional_update_param_sim[MAX_AS_IDS];
#define mm_suppress_additional_update_param mm_suppress_additional_update_param_sim[mm_sub_id]
#else
extern boolean mm_suppress_additional_update_param;
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern boolean mm_stopped_due_to_mode_cap_change_sim[MAX_NAS_STACKS];
#define mm_stopped_due_to_mode_cap_change mm_stopped_due_to_mode_cap_change_sim[mm_as_id]
#endif

extern nas_crit_sect_type mm_rat_prioritylist_crit_sec;

typedef struct mm_last_reg_dsac_information_tag
{
    mm_as_cell_access_status_T cell_access;
    boolean                   dsac_present;
    mm_as_cell_access_status_T cs_cell_access;
    mm_as_cell_access_status_T ps_cell_access;
}mm_last_reg_dsac_information_T;

extern sys_manual_search_timer_value_type mm_emm_manual_search_timer_value;   
extern boolean mm_emm_is_qrd_device;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern mm_last_reg_dsac_information_T mm_last_reg_dsac_information_sim[MAX_NAS_STACKS];
#define  mm_last_reg_dsac_information (mm_last_reg_dsac_information_sim[mm_as_id])

#else
extern mm_last_reg_dsac_information_T mm_last_reg_dsac_information;
#endif

/*
 * Definition of structures used in the link lists of data relating to
 * message content
 *
 * Structure MsgDetail is a linked list of all the expected L3 messages
 * (including separater entries for each direction if the message is
 * different between MO and MT directions)
 * If the corresponding message has and IEs (apart from PD/TI and message
 * type then the entry FirstMessageIE is the root of a linked list of
 * MessageIE structures, which identifies all the IEs which can be present
 * in the message in the order of occurrence.
 * These MessageIE structures also have a pointer to the entry in a data
 * table of IE descriptors, corresponding to the IE.
 * This entry has information on the IEI for the IE, the maximum length,
 * and the format type.
 */

/* Below ENUM is introduced to check whether service status is informed to CM after CSFB redirection*/

 typedef enum
{
  MM_CSFB_SEND_SERVICE_STATUS_NONE,
  MM_CSFB_SEND_STATUS_LIMITED_REGIONAL,
  MM_CSFB_SEND_STATUS_SRV_CONNECTED,
  MM_CSFB_SEND_STATUS_NO_SERV,
}mm_csfb_service_ind_status_enum_T;

typedef enum
{
  AUTH_GSM_SECURITY_CONTEXT,
  AUTH_GSM_GPRS_SECURITY_CONTEXT,
  AUTH_CS_GPRS_SECURITY_CONTEXT,
  AUTH_PS_GPRS_SECURITY_CONTEXT,
  AUTH_COMBINED_SECURITY_CONTEXT
} auth_security_context_id ;

typedef struct equiv_plmn_list_s
{
  byte                              length;
  sys_plmn_id_s_type                plmn[EQUIVALENT_PLMN_LIST_MAX_LENGTH];
}equiv_plmn_list_s_type;

struct MessageIETag
{
   struct  IETypeTag *     IEType;
   byte                    Format;
   boolean                 Mandatory;
   struct MessageIETag *NextMessageIE;
};

struct MsgDetailTag
{
   byte                    message_type;
   byte                    ImperativeLength;
   struct MsgDetailTag     *NextMsgDetail;
   struct MessageIETag     *FirstMessageIE;
};

struct IETypeTag
{
   byte IEI;
   byte Length;
   byte Type;
};



typedef struct loci_data_tag
{
   byte        tmsi[TMSI_SIZE];
   byte        lai[LAI_SIZE];
   byte        tmsi_time;        /* not used */
   byte        location_update_status;
} location_information_T;

typedef struct ie_info_tag
{
   byte  *start;        /* pointer to start of IE in message */
   byte  IEI;           /* identity of this IE */
   byte  length;        /* actual length of the IE in this message */
   byte  presence;      /* flag to indicate if mandatory or not    */
   byte  syntax_ok;     /*  flag to indicate whether the IE has passed the syntax check  */
} IE_info_T;



typedef struct lu_reject_information_tag
{
   byte     reject_cause;
   byte     lai[LAI_SIZE];
   uint8    mm_conn_rej_timer_value;
} lu_reject_information_T;

typedef struct held_service_request_tag
{
   boolean                    present;
   cm_service_request_msg_T   message;
   byte                       establishment_cause;
   auth_security_context_id   security_context ;
   mm_as_status_T             interat_status ;
#if defined(FEATURE_LTE)
   /* This field will be used to decide whether to include additonal update IE
      * in the SERVICE_REQ after CS fall back to a different LA */
   boolean                    include_additional_update_ie;
#endif
} held_service_request_T;

/*
 * definitions of timer states
 */

enum
{
   TIMER_STOPPED,
   TIMER_PAUSED,
   TIMER_ACTIVE
};

/* ------------------------------
**  OTA and NON_OTA message
** --------------------------- */
typedef enum
{
  OTA_SERVICE_IND_MESSAGE,
  NORMAL_SERVICE_MESSAGE
} service_ind_message_type;

/* ------------------------------
** Service states for MM and GMM
** --------------------------- */
typedef enum
{
  SERVICE_AVAILABLE,
  SERVICE_NOT_AVAILABLE
} service_state_type;

typedef enum mm_attach_failed_ind                                               
{
  ATTACH_FAILED_IND_REQUIRED,
  ATTACH_FAILED_IND_WAITING_FOR_GMM,
  ATTACH_FAILED_IND_NOT_REQUIRED
} mm_attach_failed_status_type;
typedef enum
{
  CCO_INACTIVE,
  CCO_IN_PROGRESS,
  CCO_FAILED,
  CCO_COMPLETE
} WtoGCCO_in_progress_enum_T ;

#ifdef FEATURE_WRLF_SYSTEM_SEL
/* ------------------------------
**  WRLF state
** --------------------------- */
typedef enum
{
  WCDMA_RLF_IDLE,
  WCDMA_RLF_STARTED,
  WCDMA_RLF_SERVICE_CNF_PENDING
} wcdma_rlf_state_enum_T;
#endif

typedef enum
{
  EMERGENCY_MM_CONNECTION,
  NORMAL_MM_CONNECTION,
  NO_MM_CONNECTION
} mm_connection_type_T;

typedef enum nas_procedure_for_cell_access
{
 PROCEDURE_NONE,
 PROCEDURE_CS_REGISTRATION,
 PROCEDURE_PS_REGISTRATION,
 PROCEDURE_CS_PAGING_RSP,
 PROCEDURE_PS_PAGING_RSP
}nas_procedure_for_cell_access_T;

typedef enum 
{
  MM_CS_DOMAIN,
  MM_PS_DOMAIN,
  
 }mm_cn_domain_type_T;
 
#if defined (FEATURE_WRLF_SYSTEM_SEL) || defined (FEATURE_LTE)
typedef enum
{
  MM_RLF_STOP_MODE_PENDING_NONE,
  MM_WCDMA_RLF_STOP_MODE_PENDING,
  MM_LTE_RLF_STOP_MODE_PENDING
}mm_rlf_stop_mode_pending_enum_T;
#endif

#ifdef FEATURE_DUAL_DATA
typedef enum
{
  MM_DATA_PRI_CLIENT_CM,
  MM_DATA_PRI_CLIENT_GMM,
  MM_DATA_PRI_CLIENT_EMM
} mm_data_pri_client_enum_T;

#endif 
/*
 * types specific to mm
 */

typedef byte mm_event_T;

#define MT_DETACH_CAUSE_NONE 0x00
#define MT_DETACH_TYPE_NONE 0x00
#define IMSI_DETACH 0x04

/*
 * definitions of MM reject causes
 */

#define MM_SEMANTICALLY_INCORRECT_MESSAGE                        0x5f
#define MM_INVALID_MANDATORY_INFORMATION                         0x60
#define MM_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED          0x61
#define MM_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE       0x62
#define MM_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED   0x63
#define MM_CONDITIONAL_IE_ERROR                                  0x64
#define MM_MESSAGE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE        0x65
#define MM_PROTOCOL_ERROR_UNSPECIFIED                            0x6f

#ifdef FEATURE_CCBS
#define MM_SERVICE_OPTION_NOT_SUPPORTED                          0x20
#define MM_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER               0x22
#endif
/*
 * special cause value to be used to indicate reason for failure of LU
 * being due to access barring
 */

#define CAUSE_ACCESS_BARRED                                      0xfe

#ifdef FEATURE_CONNECTED_MODE_SERV_IND                              
#define MM_CAUSE_RRC_OOS_CONNECTED_MODE                          0xfc
#define MM_CAUSE_RRC_CONN_LOCAL_REL_OOS                          0xfb
#endif
/*
** Cause to indicate Immediate assignment failure during LU.
*/
#define CAUSE_IMM_ASSIGN_REJECT                                  0xfd
/*
** Cause to indicate LAI change i.e RR found that current LAI 
** different from the one LAU attempted on.
*/
#define CAUSE_LAI_CHANGED                                        0xfa
#define CAUSE_RANDOM_ACCESS_FAILURE                              0xf8

#define CAUSE_CONN_EST_NOT_ATTEMPTED                   0xf7

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/*
** Cause to indicate that the other subcription hasnt yielded TRM
*/
#define CAUSE_RADIO_UNAVAILABLE                            0xf9
#endif

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
#define   CAUSE_SIM_BUSY                                   0xFA    
#endif        

#ifdef FEATURE_SGLTE
#define CAUSE_SRVCC_SGLTE_CS_HANDOVER                      0xfB
#endif
                
/*
 * Definitions of reasons for pending LU requests
 */

#define EMM_DEFAULT_T3402  12*60*1000   /* 12 minutes default value*/

#define EMM_DEFAULT_T3412  54*60*1000   /* 54 minutes default value*/

enum
{
   NO_LU,
   T3211_EXPIRED,
   T3212_EXPIRED,
   T3213_EXPIRED,
   T3312_EXPIRED,
   NORMAL_LU,
   IMSI_ATTACH,
   T3122_EXPIRED,
   LTE_IRAT_LU,
   LU_ABORTED_IN_UPDATED_CELL,
   LU_PENDED_TMSI_REALLOCATION
};

/*
 * Definition of MM main state change events
 */

#define MM_STD_NULL_EVENT                             0
#define MM_STD_MS_ACTIVATED                           1
#define MM_STD_MS_DEACTIVATED_NO_ATTACH               2
#define MM_STD_ATTACH_LU_NOT_NEEDED                   3
#define MM_STD_LU_REQUIRED                            4
#define MM_STD_LOW_LEVEL_FAIL                         5
#define MM_STD_LU_REJECT                              6
#define MM_STD_LU_REJECTED_RETRY                      7
#define MM_STD_RR_CONNECTION_ESTABLISHED              8
#define MM_STD_RR_CONNECTION_RELEASED                 9
#define MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED          10
#define MM_STD_LU_ACCEPT                              11
#define MM_STD_LU_ACCEPT_FOLLOW_ON                    12
#define MM_STD_RR_CONNECTION_ABORTED                  13
#define MM_STD_T3240_TIMEOUT                          14
#define MM_STD_MM_CONNECTION_REQUEST                  15
#define MM_STD_SERVICE_REJECT                         16
#define MM_STD_SERVICE_REJECT_CAUSE_4_6               17
#define MM_STD_CIPHERING_HAS_STARTED                  18
#define MM_STD_CNM_SERVICE_ACCEPT                     19
#define MM_STD_T3230_TIMEOUT                          20
#define MM_STD_SERVICE_REQUEST_SENT                   21
#define MM_STD_LOW_LEVEL_FAIL_REEST_ALLOWED           22
#define MM_STD_LOW_LEVEL_FAIL_REEST_NOT_ALLOWED       23
#define MM_STD_REESTABLISHMENT_REQUESTED              24
#define MM_STD_LAST_CONNECTION_RELEASED               25
#define MM_STD_NETWORK_MESSAGE_RECEIVED               26
#define MM_STD_RANDOM_ACCESS_FAILURE                  27
#define MM_STD_T3210_TIMEOUT                          28
#define MM_STD_ABORT_MESSAGE_RECEIVED                 29
#define MM_STD_AUTHENTICATION_REJECT                  30
#define MM_STD_MS_NREG_REQ_NO_POWER_DOWN              31
#define MM_STD_COMBINED_RA_UPDATE_INITIATED           32
#define MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED      33
#define MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE              51

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
#define MM_STD_RR_CONNECTION_REL_NOT_ALLOWED          34   /* last CM conn release req received
                                                            * but LCS is in progress
                                                            */
#define MM_STD_LCS_STARTED                            35
#define MM_STD_LCS_IN_PROGRESS                        36
#define MM_STD_LCS_FINISHED                           37
#define MM_STD_LCS_FINISHED_RR_CONN_ACTIVE            38
#endif
#ifdef FEATURE_CCBS
#define MM_STD_CM_SERVICE_PROMPT_RECEIVED             39
#define MM_STD_CM_SERVICE_PROMPT_ACCEPT               60
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
#define MM_STD_CCO_FAILED                             61
#endif

#ifdef FEATURE_LTE
#define MM_STD_LOCALLY_DEACTIVATE_MM                  62
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define MM_STD_RADIO_UNAVAILABLE                      63
#endif
#define MM_STD_SRVCC_HANDOVER                      64

#define MM_STD_REESTABLISHMENT_REQUIRED               65
#ifdef FEATURE_FEMTO_CSG
#define MM_STD_SERVICE_REJECT_CAUSE_25                66
#endif 

#define MM_STD_MMCC_REEST_REJ                         67
/*
 * Definition for mm sub-state events
 */

#define MM_STD_DEACTIVATE                             40
#define MM_STD_NORMAL_SERVICE                         41
#define MM_STD_PLMN_SEARCH                            42
#define MM_STD_UPDATE_PENDING                         43
#define MM_STD_UPDATE_ATTEMPTED                       44
#define MM_STD_LIMITED_SERVICE                        45
#define MM_STD_NO_SERVICE                             46
#define MM_STD_NO_IMSI                                47
#define MM_STD_PLMN_SEARCH_NORMAL_SERVICE             48
#ifdef FEATURE_NAS_ECALL
#define MM_STD_ECALL_INACTIVE                         49
#endif

#ifdef FEATURE_LTE
#define MM_STD_UPDATE_PENDING_IRAT_FROM_LTE           50
#endif

#define MM_STD_INVALID_EVENT                          255




/*
 * Definition of MM states
 */

#define MM_NULL                                       0
#define MM_IDLE                                       19
#define MM_WAIT_FOR_RR_ACTIVE                         18
#define MM_WAIT_FOR_RR_CONNECTION_LU                  13
#define MM_LOCATION_UPDATE_REJECTED                   10
#define MM_LOCATION_UPDATE_INITIATED                  3
#define MM_WAIT_FOR_RR_CONNECTION_IMSI_DETACH         15
#define MM_IMSI_DETACH_INITIATED                      7
#define MM_WAIT_FOR_NETWORK_COMMAND                   9
#define MM_WAIT_FOR_RR_CONNECTION_MM                  14
#define MM_WAIT_FOR_OUTGOING_MM_CONNECTION            5
#define MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION 20
#define MM_WAIT_FOR_REESTABLISH_DECISION              22
#define MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT     21
#define MM_REESTABLISHMENT_INITIATED                  17
#define MM_CONNECTION_ACTIVE                          6
#define MM_LOCATION_UPDATING_PENDING                  23
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
#define MM_RR_CONNECTION_RELEASE_NOT_ALLOWED          25
#endif
#ifdef FEATURE_CCBS
#define MM_PROCESS_CM_SERVICE_PROMPT                  8
#endif
 /*
 * Definitions for idle substates
 */

#define MM_NULL_SUBSTATE                              0
#define MM_NO_IMSI                                    1
#define MM_PLMN_SEARCH                                2
#define MM_LIMITED_SERVICE                            3
#define MM_ATTEMPTING_TO_UPDATE                       4
#define MM_LOCATION_UPDATE_NEEDED                     5
#define MM_NO_CELL_AVAILABLE                          6
#define MM_PLMN_SEARCH_NORMAL_SERVICE                 7
#define MM_NORMAL_SERVICE                             8
#ifdef FEATURE_NAS_ECALL
#define MM_ECALL_INACTIVE                             9
#endif

/*
 * Definitions for update status
 */

#define  UPDATED                                      0
#define  NOT_UPDATED                                  1
#define  ROAMING_NOT_ALLOWED                          2
#define  LOCATION_AREA_NOT_ALLOWED                    3
#ifdef FEATURE_NAS_ECALL
#define  UPDATING_DISABLED                            4 
#endif
/*
 * Definition of LU causes, and LU reason
 */

#define  MM_LU_CAUSE_NORMAL                           0
#define  MM_LU_CAUSE_PERIODIC                         1
#define  MM_LU_CAUSE_IMSI_ATTACH                      2
#define  MM_LU_CAUSE_RESERVED                         3

/*
 * Definitions of LU origins, ie the origin of the request for a LU
 */

//#define  NORMAL                                       0
//#define  T3212_TIMEOUT                                1

/*
 * Length to be used for the NO_IDENTITY type of Identity in 
 * Identification Procedure.
 */
#define  IDENTIFICATION_PROC_NO_IDENTITY_LEN    1
#define  GMM_IDENTIFICATION_PROC_NO_IDENTITY_LEN    3

#ifdef FEATURE_ENHANCED_NW_SELECTION
#define  MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH     10
#endif

#define MAX_LU_ATTEMPT_SIZE 4

/* Serving PLMN information used by MM */

typedef struct serving_plmn_info
{
  sys_plmn_service_state_s_type     info;
  location_area_code_T              lac;
  routing_area_code_T               rac;
  service_state_type                mm_service_state;
  service_state_type                gmm_service_state;
  network_op_mode_type              network_op_mode;
  sys_srv_domain_e_type             ms_op_mode;
  boolean                           update_equiv_plmn_list;
  equiv_plmn_list_s_type            equiv_plmn_list;
#ifdef FEATURE_GSM_DTM
  boolean                           dtm_supported;
#endif /* FEATURE_GSM_DTM */
  boolean                           egprs_supported;
  sys_hs_ind_e_type                 hsdpa_hsupa_ind;
  boolean                           t3122_running;
  ARFCN_T                           arfcn;
  uint8                             bsic;
} serving_plmn_info_T;

/*Putting definition out of dual sim  */
extern byte mm_msg_info;

typedef enum 
{
  MMRRC_IDLE,
  MMRRC_EST_IN_PROGRESS,
  MMRRC_ACTIVE,
  MMRRC_RELEASE_IN_PROGRESS
}mmrrc_connection_status_T;

typedef struct{
  uint8 mcc1;
  uint8 mcc2;
  uint8 mcc3;
 } mm_mcc_data_type;

#ifdef FEATURE_ENHANCED_NW_SELECTION
typedef struct mm_lai_timer_info
{
  byte                              lai[LAI_SIZE];
  byte                              rac;
  byte                              reject_cause[MAX_LU_ATTEMPT_SIZE];
  dword                             timer_count;
  boolean                           is_ps_registered;
  byte                              ps_attempt_counter;
}mm_lai_timer_info_T;

typedef struct mm_lai_timer_info_list_info
{
  byte                               length;
  mm_lai_timer_info_T                lai_timer_list[MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH];
 }mm_lai_timer_info_list_info_T ;
#endif
/******************************************************************************/
/*                                                                            */
/*  MM_PENDED_STOP_MODE_REQ                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mm_pended_stop_mode_req_s
{
  mmr_stop_mode_req_s_type                  mmr_stop_mode_req;
  boolean                                   valid;
} mm_pended_stop_mode_req_s_type;

typedef struct emm_location_info_s_type
{
  byte        lai[LAI_SIZE];
  boolean     lai_exists;
} emm_location_info_s_type;

/*
 * DEFINITION/DECLARATION OF GLOBALS
 *
 */

/**
** Enumeration of Active Subscription ID's 
*/
typedef enum {

  MM_AS_ID_1 = 0,
    /**< Subscription ID 1 */

  MM_AS_ID_2,
    /**< Subscription ID 2 */

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
  MM_AS_ID_3,
#endif
} mm_as_id_e_type;

extern mm_as_id_e_type mm_as_id;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

#define MSG_HIGH_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_HIGH, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_MED_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_MED, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_LOW_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_LOW, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_ERROR_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_ERROR, "ds%d" str , (sub+1), (p1), (p2), (p3))

/* Warning: 3rd argument is not logged */
#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
    ERR_FATAL ("ds%d" str , (sub+1), (p1), (p2))

extern mm_as_id_e_type mm_cs_stack_id;
extern mm_as_id_e_type mm_ps_stack_id;
extern boolean                   mm_llc_needs_to_be_resumed_sim[MAX_NAS_STACKS];
extern byte                      mm_state_sim[MAX_NAS_STACKS];
extern byte                      mm_idle_substate_sim[MAX_NAS_STACKS];

extern boolean                   mm_nreg_req_received_sim[MAX_NAS_STACKS];
extern boolean                   mm_cnm_rel_req_received_sim[MAX_NAS_STACKS];
extern boolean                   mm_present_all_plmns_sim[MAX_NAS_STACKS];
extern location_information_T    mm_location_information_sim[MAX_NAS_STACKS];
extern held_service_request_T    mm_held_cnm_service_req_sim[MAX_NAS_STACKS];
extern boolean                   mm_reg_waiting_for_reg_cnf_sim[MAX_NAS_STACKS];
extern boolean                   mm_ready_to_send_reg_cnf_sim[MAX_NAS_STACKS];
extern boolean                   mm_needs_to_send_mmr_service_ind_sim[MAX_NAS_STACKS];
extern boolean                   mm_waiting_for_service_cnf_sim[MAX_NAS_STACKS];
extern boolean                   mm_waiting_for_stop_mode_cnf_sim[MAX_NAS_STACKS];
extern boolean                   mm_waiting_for_deact_cnf_sim[MAX_NAS_STACKS];
extern boolean                   mm_waiting_for_ph_status_change_cnf_sim[MAX_NAS_STACKS];
extern sys_radio_access_tech_e_type mm_last_active_rat_stopped_sim[MAX_NAS_STACKS];
extern lu_reject_information_T   mm_lu_reject_info_sim[MAX_NAS_STACKS];
extern byte                      mm_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];
extern byte                      mm_lu_attempt_counter_sim[MAX_NAS_STACKS];
extern byte                      mm_pending_lu_request_sim[MAX_NAS_STACKS];
extern byte                      mm_pending_no_service_to_reg_sim[MAX_NAS_STACKS];
extern byte                      mm_lu_start_reason_sim[MAX_NAS_STACKS];
extern boolean                   mm_follow_on_request_sim[MAX_NAS_STACKS];
extern boolean                   mm_follow_on_proceed_sim[MAX_NAS_STACKS];
extern byte                      mm_reject_cause_sim[MAX_NAS_STACKS];
extern boolean                   mm_cnm_is_active_sim[MAX_NAS_STACKS];
extern boolean                   mm_sms_is_active_sim[MAX_NAS_STACKS];
extern boolean                   mm_T3213_timed_out_sim[MAX_NAS_STACKS];
extern byte                      mm_idle_transition_timer_sim[MAX_NAS_STACKS];
extern byte                      mm_timer_status_sim[MAX_NAS_STACKS][MAX_MM_TIMER+1];
extern boolean                   mm_possible_imsi_attach_sim[MAX_NAS_STACKS];

extern byte                      mm_cipher_key_sequence_number_sim[MAX_NAS_STACKS];
extern ef_kc_T                   mm_ef_kc_sim[MAX_NAS_STACKS];
extern boolean                   mm_ef_kc_valid_sim[MAX_NAS_STACKS];
extern IE_info_T                 mm_IE_info_sim[MAX_NAS_STACKS][MAXNO_IES_IN_MESSAGE];
extern byte                      mm_no_of_ies_sim[MAX_NAS_STACKS];
extern boolean                   mm_sig_low_priority_sim[MAX_NAS_STACKS];
extern boolean                   mm_nreg_release_pending_sim[MAX_NAS_STACKS];
extern byte                      send_sequence_number_mm_cc_ss_sim[MAX_NAS_STACKS];
/* mm_cnm_srv_est_cause is used to indicate CNM service request type*/
extern establishment_cause_T     mm_cnm_srv_est_cause_sim[MAX_NAS_STACKS];

extern sys_radio_access_tech_e_type mm_ue_pref_rat_sim[MAX_NAS_STACKS];
extern rrc_lo_access_class_e_type   mm_lo_access_class_sim[MAX_NAS_STACKS];
extern rrc_hi_access_class_type     mm_hi_access_class_sim[MAX_NAS_STACKS];


extern uint32                    mm_op_mode_sim[MAX_NAS_STACKS];
extern boolean                   mm_integrity_protected_sim[MAX_NAS_STACKS];
extern serving_plmn_info_T       mm_serving_plmn_sim[MAX_NAS_STACKS];

extern boolean                   mm_rat_change_pending_sim[MAX_NAS_STACKS];
extern boolean                   mm_serv_change_pending_sim[MAX_NAS_STACKS];
extern boolean                   mm_to_only_reg_sim[MAX_NAS_STACKS];
extern sys_radio_access_tech_e_type mm_previous_serving_rat_sim[MAX_NAS_STACKS];

extern boolean                   mm_dual_mode_sim[MAX_NAS_STACKS];
extern sys_band_mask_type        mm_band_pref_sim[MAX_NAS_STACKS];


extern mmrrc_connection_status_T    rrc_connection_status_sim[MAX_NAS_STACKS];
extern mmrrc_connection_status_T    cs_session_status_sim[MAX_NAS_STACKS];
extern mmrrc_connection_status_T    ps_session_status_sim[MAX_NAS_STACKS];

extern boolean                   mm_deep_sleep_pending_sim[MAX_NAS_STACKS];

extern byte                      mm_plmn_list_req_pending_sim[MAX_NAS_STACKS];

extern boolean                 mm_manual_mode_user_selection_procedure_sim[MAX_NAS_STACKS];

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
extern sys_band_mask_type        mm_ue_band_class_pref_sim[MAX_NAS_STACKS];
extern sys_band_mask_e_type      mm_active_band_sim[MAX_NAS_STACKS];
#endif

extern sys_band_class_e_type     mm_band_indicator_sim[MAX_NAS_STACKS];
extern byte                      mm_previous_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];

extern byte                      mm_gstk_previous_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];

extern byte                      mm_previous_idle_substate_sim[MAX_NAS_STACKS];
extern mm_as_cell_access_status_T mm_prev_cell_access_sim[MAX_NAS_STACKS];
extern boolean                   mm_hard_abort_pending_sim[MAX_NAS_STACKS];
extern boolean                   mm_hard_abort_ue_sglte_mode_switch_pending_sim[MAX_NAS_STACKS];
extern sys_stop_mode_reason_e_type                   mm_abort_dual_switch_pending_sim[MAX_NAS_STACKS];
extern boolean                   mm_deep_sleep_alt_scan_sim[MAX_NAS_STACKS];
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
extern boolean mm_lcs_active_sim[MAX_NAS_STACKS];
#endif

#ifdef FEATURE_CCBS
extern boolean mm_supports_NIMO_CM_connection_sim[MAX_NAS_STACKS];
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
extern WtoGCCO_in_progress_enum_T  WtoGCCO_state_sim[MAX_NAS_STACKS];
#endif

#ifdef FEATURE_INTERRAT_PCCO_GTOW
extern WtoGCCO_in_progress_enum_T  GtoWCCO_state_sim[MAX_NAS_STACKS];
#endif /* FEATURE_INTERRAT_PCCO_GTOW */

#ifdef FEATURE_DSAC
/********************************************************
 *Variables mm_ps_access_barred_to_unbarred and 
 *mm_ps_access_barred_to_unbarred are marked only if 
 *UE is in NMO 1 network.
 *******************************************************/
extern boolean mm_ps_access_barred_to_unbarred_sim[MAX_NAS_STACKS];
extern boolean mm_cs_access_barred_to_unbarred_sim[MAX_NAS_STACKS];

#define MM_NONE 0          /*0000 0000*/
#define MM_LU   1          /*0000 0001*/
#define MM_RAU  2          /*0000 0010*/
#define MM_COMBINED 4      /*0000 0100*/


extern byte mm_last_procedure_to_update_registration_sim[MAX_NAS_STACKS];

#endif 

extern boolean mm_gs_association_established_sim[MAX_NAS_STACKS];

/*****************************************************
*Variable mm_initiate_lu_upon_entry_into_new_cell is marked 
*only when LU got failed because of d,f,g of subclause 4.4.4.9 of
*24.0.08
*****************************************************/

extern boolean mm_initiate_lu_upon_entry_into_new_cell_sim[MAX_NAS_STACKS];

/* mm_serving_cell_id is to store the  serving cell */

extern uint32    mm_serving_cell_id_sim[MAX_NAS_STACKS];

extern boolean   mm_serving_cell_valid_sim[MAX_NAS_STACKS];

/* mm_lu_failed_cell_id is to store the cell id 
where location update got failed because of 
d,f,g of section  4.4.9 of 24.008 */

extern uint32    mm_lu_failed_cell_id_sim[MAX_NAS_STACKS];

#ifdef FEATURE_ENHANCED_NW_SELECTION
extern boolean mm_force_lu_sim[MAX_NAS_STACKS];
#endif

extern boolean mm_plmn_search_in_progress_sim[MAX_NAS_STACKS];

extern byte mm_plmn_search_transaction_id_sim[MAX_NAS_STACKS];
#define mm_plmn_search_transaction_id mm_plmn_search_transaction_id_sim[mm_as_id]

extern call_type_for_rrc_est_req_T call_type_for_rrc_est_req_sim[MAX_NAS_STACKS];
extern mm_attach_failed_status_type mm_mmr_attach_ind_status_sim[MAX_NAS_STACKS];
extern boolean mm_managed_roaming_enabled_sim[MAX_AS_IDS];


extern boolean mm_cs_domain_possible_reest_sim[MAX_NAS_STACKS];
extern boolean mm_ps_domain_possible_reest_sim[MAX_NAS_STACKS];
extern boolean gmm_procedure_rerun_ongoing_sim[MAX_NAS_STACKS];
extern word   mm_coord_camped_cell_id_sim[MAX_NAS_STACKS];
#define mm_cs_domain_possible_reest mm_cs_domain_possible_reest_sim[mm_as_id]
#define mm_ps_domain_possible_reest mm_ps_domain_possible_reest_sim[mm_as_id]
#define gmm_procedure_rerun_ongoing gmm_procedure_rerun_ongoing_sim[mm_as_id]

#define  mm_state (mm_state_sim[mm_as_id])
#define  mm_idle_substate (mm_idle_substate_sim[mm_as_id])
#define  mm_IE_info (mm_IE_info_sim[mm_as_id])
#define  mm_no_of_ies (mm_no_of_ies_sim[mm_as_id])
#define  mm_llc_needs_to_be_resumed (mm_llc_needs_to_be_resumed_sim[mm_as_id])
#define  mm_nreg_req_received (mm_nreg_req_received_sim[mm_as_id])
#define  mm_cnm_rel_req_received (mm_cnm_rel_req_received_sim[mm_as_id])
#define  mm_present_all_plmns (mm_present_all_plmns_sim[mm_as_id])
#define  mm_location_information (mm_location_information_sim[mm_as_id])
#define  mm_held_cnm_service_req (mm_held_cnm_service_req_sim[mm_as_id])
#define  mm_reg_waiting_for_reg_cnf (mm_reg_waiting_for_reg_cnf_sim[mm_as_id])
#define  mm_ready_to_send_reg_cnf (mm_ready_to_send_reg_cnf_sim[mm_as_id])
#define  mm_needs_to_send_mmr_service_ind (mm_needs_to_send_mmr_service_ind_sim[mm_as_id])
#define  mm_waiting_for_service_cnf (mm_waiting_for_service_cnf_sim[mm_as_id])
#define  mm_waiting_for_stop_mode_cnf (mm_waiting_for_stop_mode_cnf_sim[mm_as_id])
#define  mm_waiting_for_deact_cnf (mm_waiting_for_deact_cnf_sim[mm_as_id])
#define  mm_waiting_for_ph_status_change_cnf (mm_waiting_for_ph_status_change_cnf_sim[mm_as_id])
#define  mm_last_active_rat_stopped (mm_last_active_rat_stopped_sim[mm_as_id])
#define  mm_lu_reject_info (mm_lu_reject_info_sim[mm_as_id])
#define  mm_serving_lai (mm_serving_lai_sim[mm_as_id])
#define  mm_lu_attempt_counter (mm_lu_attempt_counter_sim[mm_as_id])
#define  mm_pending_lu_request (mm_pending_lu_request_sim[mm_as_id])
#define  mm_pending_no_service_to_reg (mm_pending_no_service_to_reg_sim[mm_as_id])
#define  mm_lu_start_reason (mm_lu_start_reason_sim[mm_as_id])
#define  mm_cipher_key_sequence_number (mm_cipher_key_sequence_number_sim[mm_as_id])
#define  mm_follow_on_request (mm_follow_on_request_sim[mm_as_id])
#define  mm_follow_on_proceed (mm_follow_on_proceed_sim[mm_as_id])
#define  mm_reject_cause (mm_reject_cause_sim[mm_as_id])
#define  mm_cnm_is_active (mm_cnm_is_active_sim[mm_as_id])
#define  mm_sms_is_active (mm_sms_is_active_sim[mm_as_id])
#define  mm_T3213_timed_out (mm_T3213_timed_out_sim[mm_as_id])
#define  mm_idle_transition_timer (mm_idle_transition_timer_sim[mm_as_id])
#define  mm_timer_status (mm_timer_status_sim[mm_as_id])
#define  mm_possible_imsi_attach (mm_possible_imsi_attach_sim[mm_as_id])

#define  mm_ef_kc (mm_ef_kc_sim[mm_as_id])
#define  mm_ef_kc_valid (mm_ef_kc_valid_sim[mm_as_id])
#define  mm_op_mode (mm_op_mode_sim[mm_as_id])
#define  mm_sig_low_priority (mm_sig_low_priority_sim[mm_as_id])
#define  mm_nreg_release_pending (mm_nreg_release_pending_sim[mm_as_id])
#define  mm_integrity_protected (mm_integrity_protected_sim[mm_as_id])
#define  mm_serving_plmn (mm_serving_plmn_sim[mm_as_id])

#define  mm_ue_pref_rat (mm_ue_pref_rat_sim[mm_as_id])
#define  mm_rat_change_pending (mm_rat_change_pending_sim[mm_as_id])
#define  mm_serv_change_pending (mm_serv_change_pending_sim[mm_as_id])
#define  mm_to_only_reg (mm_to_only_reg_sim[mm_as_id])
#define  mm_previous_serving_rat (mm_previous_serving_rat_sim[mm_as_id])

#define  mm_dual_mode (mm_dual_mode_sim[mm_as_id])
#define  mm_band_pref (mm_band_pref_sim[mm_as_id])
#define  mm_cnm_srv_est_cause (mm_cnm_srv_est_cause_sim[mm_as_id])

#define  mm_lo_access_class (mm_lo_access_class_sim[mm_as_id])
#define  mm_hi_access_class (mm_hi_access_class_sim[mm_as_id])
#define  mm_deep_sleep_pending (mm_deep_sleep_pending_sim[mm_as_id])
#define  mm_hard_abort_pending (mm_hard_abort_pending_sim[mm_as_id])
#define  mm_hard_abort_ue_sglte_mode_switch_pending (mm_hard_abort_ue_sglte_mode_switch_pending_sim[mm_as_id])

#define  mm_abort_dual_switch_pending (mm_abort_dual_switch_pending_sim[mm_as_id])
#define  mm_deep_sleep_alt_scan (mm_deep_sleep_alt_scan_sim[mm_as_id])

#define  mm_plmn_list_req_pending (mm_plmn_list_req_pending_sim[mm_as_id])

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
#define  mm_ue_band_class_pref (mm_ue_band_class_pref_sim[mm_as_id])
#define  mm_active_band (mm_active_band_sim[mm_as_id])
#endif

#define  mm_band_indicator (mm_band_indicator_sim[mm_as_id])
#define  mm_previous_serving_lai (mm_previous_serving_lai_sim[mm_as_id])

#define  mm_gstk_previous_serving_lai (mm_gstk_previous_serving_lai_sim[mm_as_id])

#define  mm_previous_idle_substate (mm_previous_idle_substate_sim[mm_as_id])
#define  mm_prev_cell_access (mm_prev_cell_access_sim[mm_as_id])

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
#define  mm_lcs_active (mm_lcs_active_sim[mm_as_id])
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_CCBS
#define  mm_supports_NIMO_CM_connection (mm_supports_NIMO_CM_connection_sim[mm_as_id])
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
#define  WtoGCCO_state (WtoGCCO_state_sim[mm_as_id])
#endif

#ifdef FEATURE_INTERRAT_PCCO_GTOW
#define  GtoWCCO_state (GtoWCCO_state_sim[mm_as_id])
#endif

#define  mm_gs_association_established (mm_gs_association_established_sim[mm_as_id])
#define  mm_initiate_lu_upon_entry_into_new_cell (mm_initiate_lu_upon_entry_into_new_cell_sim[mm_as_id])
#define  mm_serving_cell_id (mm_serving_cell_id_sim[mm_as_id])
#define  mm_serving_cell_valid (mm_serving_cell_valid_sim[mm_as_id])
#define  mm_lu_failed_cell_id (mm_lu_failed_cell_id_sim[mm_as_id])

#ifdef FEATURE_DSAC
#define  mm_ps_access_barred_to_unbarred (mm_ps_access_barred_to_unbarred_sim[mm_as_id])
#define  mm_cs_access_barred_to_unbarred (mm_cs_access_barred_to_unbarred_sim[mm_as_id])
#define  mm_last_procedure_to_update_registration (mm_last_procedure_to_update_registration_sim[mm_as_id])
#endif 

#define  mm_manual_mode_user_selection_procedure (mm_manual_mode_user_selection_procedure_sim[mm_as_id])

#ifdef FEATURE_ENHANCED_NW_SELECTION
#define  mm_force_lu (mm_force_lu_sim[mm_as_id])
#endif

#define  mm_plmn_search_in_progress (mm_plmn_search_in_progress_sim[mm_as_id])

#define  call_type_for_rrc_est_req (call_type_for_rrc_est_req_sim[mm_as_id])
#define  mm_mmr_attach_ind_status (mm_mmr_attach_ind_status_sim[mm_as_id])                         

#define  rrc_connection_status (rrc_connection_status_sim[mm_as_id])
#define  cs_session_status (cs_session_status_sim[mm_as_id])
#define  ps_session_status (ps_session_status_sim[mm_as_id])

#define  cs_session_est_in_progress (cs_session_est_in_progress_sim[mm_as_id])
#define  ps_session_est_in_progress (ps_session_est_in_progress_sim[mm_as_id])


#ifdef FEATURE_MODEM_HEAP
extern byte *pending_gmm_msg_sim[MAX_NAS_STACKS];
extern byte* pending_mm_msg_sim[MAX_NAS_STACKS];
#else
extern byte pending_gmm_msg_sim[MAX_NAS_STACKS][MAX_MESSAGE_SIZE];
extern byte pending_mm_msg_sim[MAX_NAS_STACKS][MAX_MESSAGE_SIZE];
#endif

#define  pending_gmm_msg (pending_gmm_msg_sim[mm_as_id])
#define  pending_gmm_msg_cause (pending_gmm_msg_cause_sim[mm_as_id])

#define  gmm_rat_chng_pended_msg (gmm_rat_chng_pended_msg_sim[mm_as_id])
#define  gmm_rat_chng_pended_msg_valid (gmm_rat_chng_pended_msg_valid_sim[mm_as_id])
#define  gmm_rat_chng_pended_msg_cause (gmm_rat_chng_pended_msg_cause_sim[mm_as_id])

#define  mm_coord_camped_cell_id (mm_coord_camped_cell_id_sim [mm_as_id])

#define  pending_mm_msg (pending_mm_msg_sim[mm_as_id])
extern rrc_establish_cause_e_type pending_mm_msg_cause_sim[MAX_NAS_STACKS];
#define  pending_mm_msg_cause (pending_mm_msg_cause_sim[mm_as_id])

extern rrc_establish_cause_e_type pending_gmm_msg_cause_sim[MAX_NAS_STACKS];

#define  send_sequence_number_mm_cc_ss (send_sequence_number_mm_cc_ss_sim[mm_as_id])

/* DUAL SIM : AS INDEX position in Timer IDs*/
#define TIMER_ID_SIM_POS 6
#define MM_TIMER_TAG_AS_ID(MM_TIMER) (timer_id_T)(MM_TIMER | ((mm_as_id & 0x01) << (TIMER_ID_SIM_POS)))

extern uint8 mm_nv_context_id_sim[MAX_AS_IDS];
#define mm_nv_context_id (mm_nv_context_id_sim[mm_sub_id])

/* Flag to indicate whether mm connection type is emergency or normal */
extern mm_connection_type_T   mm_connection_type_sim[MAX_NAS_STACKS];
#define mm_connection_type mm_connection_type_sim[mm_as_id]

/* NV data has to be always accessed using AS ID - NV Context ID mapping */
#define  mm_managed_roaming_enabled (mm_managed_roaming_enabled_sim[mm_sub_id])

extern boolean      mm_mmr_cell_service_ind_pending_sim[MAX_NAS_STACKS];
#define      mm_mmr_cell_service_ind_pending   mm_mmr_cell_service_ind_pending_sim[mm_as_id]

/* Flag to indicate whether IMSI stored in NV is same as the 
   one in USIM/SIM 
*/
extern boolean mm_imsi_is_native_sim[MAX_NAS_STACKS];
#define mm_imsi_is_native mm_imsi_is_native_sim[mm_as_id]

/* Flag to indicate whether GSM access is allowed or not */
extern boolean auth_gsm_access_sim[MAX_NAS_STACKS];
#define auth_gsm_access auth_gsm_access_sim[mm_as_id]

/* GSM security context for USIM */
extern boolean auth_usim_gsm_security_context_sim[MAX_NAS_STACKS] ;
#define auth_usim_gsm_security_context auth_usim_gsm_security_context_sim[mm_as_id]

/* SIM/USIM access class information to be sent to RR/RRC */
extern byte mm_sim_access_class_sim[MAX_NAS_STACKS][MM_SIM_EFACC_SIZE];
#define mm_sim_access_class mm_sim_access_class_sim[mm_as_id]

/*LU reject cause for each failure attempt */
extern byte mm_lu_reject_cause_sim[MAX_NAS_STACKS][MAX_LU_ATTEMPT_SIZE];
#define mm_lu_reject_cause mm_lu_reject_cause_sim[mm_as_id]


extern mmgsdi_perso_status_enum_type mm_sim_perso_state_sim[MAX_NAS_STACKS];
#define mm_sim_perso_state mm_sim_perso_state_sim[mm_as_id]

/* Mobile operation mode */
extern byte mm_ms_opmode_sim[MAX_NAS_STACKS];
#define mm_ms_opmode mm_ms_opmode_sim[mm_as_id]

/* Flag to remember last network selection mode send to RR/RRC */
extern sys_network_selection_mode_e_type  last_requested_network_selection_mode_sim[MAX_NAS_STACKS];
#define last_requested_network_selection_mode last_requested_network_selection_mode_sim[mm_as_id]

/* 2 or 3 digit MNC */
extern byte mm_mnc_length_sim[MAX_NAS_STACKS];
#define mm_mnc_length mm_mnc_length_sim[mm_as_id]

extern boolean mm_waiting_for_csmt_response_sim[MAX_NAS_STACKS];
#define mm_waiting_for_csmt_response mm_waiting_for_csmt_response_sim[mm_as_id]



#ifdef FEATURE_RAT_PRIORITY_LIST
extern sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_sim[MAX_NAS_STACKS];
#define mm_rat_pri_list_info mm_rat_pri_list_info_sim[mm_as_id]

#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
extern sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_to_nw_sim[MAX_NAS_STACKS];
#define mm_rat_pri_list_info_to_nw     mm_rat_pri_list_info_to_nw_sim[mm_as_id]
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
extern byte mm_pending_lu_type_sim[MAX_NAS_STACKS];
#define mm_pending_lu_type mm_pending_lu_type_sim[mm_as_id]
#endif

extern mmr_cause_e_type   mm_reg_cause_sim[MAX_NAS_STACKS];
#define mm_reg_cause      mm_reg_cause_sim[mm_as_id]

#ifdef FEATURE_TC
extern boolean mm_tc_is_active_sim[MAX_NAS_STACKS];
#define mm_tc_is_active mm_tc_is_active_sim[mm_as_id]
#endif

#ifdef FEATURE_ENHANCED_NW_SELECTION
extern mm_lai_timer_info_list_info_T       mm_foreground_search_lai_timer_list_sim[MAX_NAS_STACKS];
#define     mm_foreground_search_lai_timer_list   (mm_foreground_search_lai_timer_list_sim[mm_as_id])

extern dword     mm_foreground_search_timer_value_sim[MAX_NAS_STACKS];
#define  mm_foreground_search_timer_value     mm_foreground_search_timer_value_sim[mm_as_id]

#endif

extern mm_csfb_service_ind_status_enum_T mm_csfb_service_ind_status_sim[MAX_NAS_STACKS];
#define mm_csfb_service_ind_status mm_csfb_service_ind_status_sim[mm_as_id]

extern boolean mm_service_ind_blocked_due_to_cs_reg_sim[MAX_NAS_STACKS];
#define mm_service_ind_blocked_due_to_cs_reg mm_service_ind_blocked_due_to_cs_reg_sim[mm_as_id]

extern boolean mm_service_ind_blocked_due_to_irat_cs_reg_sim[MAX_NAS_STACKS];
#define mm_service_ind_blocked_due_to_irat_cs_reg mm_service_ind_blocked_due_to_irat_cs_reg_sim[mm_as_id]

extern boolean mm_connection_activated_sim[MAX_NAS_STACKS];
#define mm_connection_activated mm_connection_activated_sim[mm_as_id]

extern boolean mm_is_foreground_search_to_be_initiated_sim[MAX_NAS_STACKS];
#define mm_is_foreground_search_to_be_initiated mm_is_foreground_search_to_be_initiated_sim[mm_as_id]

extern boolean pending_gmm_msg_after_release_sim[MAX_NAS_STACKS];
#define pending_gmm_msg_after_release pending_gmm_msg_after_release_sim[mm_as_id]

extern boolean pending_mm_msg_after_release_sim[MAX_NAS_STACKS];
#define pending_mm_msg_after_release pending_mm_msg_after_release_sim[mm_as_id]

extern boolean mm_is_gmm_moved_dereg_due_to_csreg_sim[MAX_NAS_STACKS];
#define mm_is_gmm_moved_dereg_due_to_csreg  mm_is_gmm_moved_dereg_due_to_csreg_sim[mm_as_id]

#ifdef FEATURE_NAS_REL10
extern sys_plmn_id_s_type mm_3gpp_t3246_plmn_id_sim[MAX_NAS_STACKS];
#define mm_3gpp_t3246_plmn_id  mm_3gpp_t3246_plmn_id_sim[mm_as_id]

extern boolean mm_store_integrity_protected_sim[MAX_NAS_STACKS];
#define mm_store_integrity_protected  mm_store_integrity_protected_sim[mm_as_id]

extern dword mm_t3246_backoff_timer_value_sim[MAX_NAS_STACKS];
#define mm_t3246_backoff_timer_value  mm_t3246_backoff_timer_value_sim[mm_as_id]
#endif

#ifdef FEATURE_NAS_REL10
extern dword mm_stored_per_ms_t3212_value_sim[MAX_NAS_STACKS];
#define mm_stored_per_ms_t3212_value  mm_stored_per_ms_t3212_value_sim[mm_as_id]

extern boolean mm_is_per_ms_t3212_present_sim[MAX_NAS_STACKS];
#define mm_is_per_ms_t3212_present mm_is_per_ms_t3212_present_sim[mm_as_id]

#endif

#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
extern boolean mm_is_lu_to_be_initiated_t3346_ltogw_sim[MAX_NAS_STACKS];
#define mm_is_lu_to_be_initiated_t3346_ltogw mm_is_lu_to_be_initiated_t3346_ltogw_sim[mm_as_id]
#endif

/**** TSTS ***/
extern sys_radio_access_tech_e_type     mm_service_loss_on_rat_sim[MAX_NAS_STACKS];
#define  mm_service_loss_on_rat mm_service_loss_on_rat_sim[mm_as_id]

extern sys_network_selection_mode_e_type mm_waiting_for_list_cnf_type_sim[MAX_NAS_STACKS];
#define  mm_waiting_for_list_cnf_type mm_waiting_for_list_cnf_type_sim[mm_as_id]

extern boolean mm_need_to_process_sim_avail_req_sim[MAX_NAS_STACKS];
#define mm_need_to_process_sim_avail_req mm_need_to_process_sim_avail_req_sim[mm_as_id]

extern boolean mm_waiting_for_open_session_cnf_sim[MAX_NAS_STACKS];
#define mm_waiting_for_open_session_cnf mm_waiting_for_open_session_cnf_sim[mm_as_id]

extern    byte mm_last_paging_cause_sim[MAX_NAS_STACKS];
#define   mm_last_paging_cause mm_last_paging_cause_sim[mm_as_id]

extern byte mm_controls_resel_dec_mt_cs_call_sim[MAX_NAS_STACKS];
#define mm_controls_resel_dec_mt_cs_call mm_controls_resel_dec_mt_cs_call_sim[mm_as_id]

extern boolean mm_gstk_send_location_status_sim[MAX_NAS_STACKS];
#define  mm_gstk_send_location_status mm_gstk_send_location_status_sim[mm_as_id]

extern boolean pending_gmm_msg_valid_sim[MAX_NAS_STACKS];
#define  pending_gmm_msg_valid (pending_gmm_msg_valid_sim[mm_as_id])

extern boolean pending_mm_msg_valid_sim[MAX_NAS_STACKS];
#define  pending_mm_msg_valid (pending_mm_msg_valid_sim[mm_as_id])
/**** TSTS End***/

void nas_swap_mmgsdi_info(mm_as_id_e_type cur_as_id, nas_mmgsdi_info_type *info, boolean *flag, mmgsdi_session_type_enum_type session_type);

extern boolean mm_sim_session_matching_with_other_sub( mmgsdi_session_type_enum_type session_type);

extern mm_pended_stop_mode_req_s_type mm_pended_stop_mode_req_sim[MAX_NAS_STACKS];
#define mm_pended_stop_mode_req mm_pended_stop_mode_req_sim[mm_as_id]

#ifdef FEATURE_LTE
extern boolean  mm_csfb_is_blind_redir_to_lte_req_to_be_sent_sim[MAX_NAS_STACKS];
#define mm_csfb_is_blind_redir_to_lte_req_to_be_sent mm_csfb_is_blind_redir_to_lte_req_to_be_sent_sim[mm_as_id]
#endif /* FEATURE_LTE */

#ifdef FEATURE_WRLF_SYSTEM_SEL
extern boolean mm_received_suitable_search_end_ind_sim[MAX_NAS_STACKS];
#define mm_received_suitable_search_end_ind mm_received_suitable_search_end_ind_sim[mm_as_id]
#endif

extern boolean  mm_is_cs_connected_due_to_srvcc_sim[MAX_NAS_STACKS];
#define  mm_is_cs_connected_due_to_srvcc mm_is_cs_connected_due_to_srvcc_sim[mm_as_id]

extern boolean  mm_psm_ready_req_rejected_sim[MAX_NAS_STACKS];
#define  mm_psm_ready_req_rejected mm_psm_ready_req_rejected_sim[mm_as_id]

#ifdef FEATURE_DUAL_DATA
extern boolean  mm_is_volte_only_ps_sim[MAX_NAS_STACKS];
#define  mm_is_volte_only_ps mm_is_volte_only_ps_sim[mm_as_id]
#endif

extern boolean  mm_reject_call_on_rai_change_sim[MAX_NAS_STACKS];
#define  mm_reject_call_on_rai_change mm_reject_call_on_rai_change_sim[mm_as_id]


#else /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
/* If DUAL SIM is not defined, no need to print subscription id */
#ifndef MSG_HIGH_DS
#define MSG_HIGH_DS( sub , str, p1, p2, p3)\
    MSG_HIGH (str , (p1), (p2), (p3))
#endif
#ifndef MSG_MED_DS
#define MSG_MED_DS( sub , str, p1, p2, p3)\
    MSG_MED (str , (p1), (p2), (p3))
#endif
#ifndef MSG_LOW_DS
#define MSG_LOW_DS( sub , str, p1, p2, p3)\
    MSG_LOW (str , (p1), (p2), (p3))
#endif
#ifndef MSG_ERROR_DS
#define MSG_ERROR_DS( sub , str, p1, p2, p3)\
    MSG_ERROR (str , (p1), (p2), (p3))
#endif
#ifndef MSG_FATAL_DS
#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
    ERR_FATAL (str , (p1), (p2), (p3))
#endif

//SGLTE changes 


#ifdef FEATURE_MODEM_HEAP
extern byte *pending_gmm_msg;
extern byte *pending_mm_msg;
#else
extern byte pending_gmm_msg[MAX_MESSAGE_SIZE];
extern byte pending_mm_msg[MAX_MESSAGE_SIZE];
#endif


extern rrc_establish_cause_e_type pending_mm_msg_cause;
extern rrc_establish_cause_e_type pending_gmm_msg_cause;

extern boolean mm_cs_domain_possible_reest;
extern boolean mm_ps_domain_possible_reest;

extern byte                      mm_state;
extern byte                      mm_idle_substate;
extern mm_system_information_T   mm_system_information;
extern boolean                   mm_llc_needs_to_be_resumed;
extern boolean                   mm_nreg_req_received;
extern boolean                   mm_cnm_rel_req_received;
extern byte                      mm_pending_no_service_to_reg;
extern boolean                   mm_present_all_plmns;
extern location_information_T    mm_location_information;
extern held_service_request_T    mm_held_cnm_service_req;
extern boolean                   mm_reg_waiting_for_reg_cnf;
extern boolean                   mm_ready_to_send_reg_cnf;
extern boolean                   mm_needs_to_send_mmr_service_ind;
extern boolean                   mm_waiting_for_service_cnf;
extern boolean                   mm_waiting_for_stop_mode_cnf;
extern boolean                   mm_waiting_for_deact_cnf;
extern boolean                   mm_waiting_for_ph_status_change_cnf;
extern boolean                   mm_hard_abort_ue_sglte_mode_switch_pending;
extern sys_radio_access_tech_e_type mm_last_active_rat_stopped;
extern lu_reject_information_T   mm_lu_reject_info;
extern byte                      mm_serving_lai[LAI_SIZE];
extern byte                      mm_lu_attempt_counter;
extern byte                      mm_pending_lu_request;
extern byte                      mm_lu_start_reason;
extern boolean                   mm_follow_on_request;
extern boolean                   mm_follow_on_proceed;
extern byte                      mm_reject_cause;
extern boolean                   mm_cnm_is_active;
extern boolean                   mm_sms_is_active;
extern boolean                   mm_T3213_timed_out;
extern byte                      mm_idle_transition_timer;
extern byte                      mm_timer_status[MAX_MM_TIMER+1];
extern boolean                   mm_possible_imsi_attach;

extern byte                      mm_cipher_key_sequence_number;
extern ef_kc_T                   mm_ef_kc;
extern boolean                   mm_ef_kc_valid;
extern IE_info_T                 mm_IE_info[MAXNO_IES_IN_MESSAGE];
extern byte                      mm_no_of_ies;
extern boolean                   mm_sig_low_priority;
extern boolean                   mm_nreg_release_pending;
extern byte                      send_sequence_number_mm_cc_ss;
extern establishment_cause_T     mm_cnm_srv_est_cause;
extern mm_connection_type_T   mm_connection_type;

extern sys_radio_access_tech_e_type mm_ue_pref_rat;
extern rrc_lo_access_class_e_type   mm_lo_access_class;
extern rrc_hi_access_class_type     mm_hi_access_class;


extern uint32                    mm_op_mode;
extern boolean                   mm_integrity_protected;
extern serving_plmn_info_T       mm_serving_plmn;

extern boolean                   mm_rat_change_pending;
extern boolean                   mm_serv_change_pending;
extern boolean                   mm_to_only_reg;
extern sys_radio_access_tech_e_type mm_previous_serving_rat;

extern boolean                   mm_dual_mode;
extern sys_band_mask_type        mm_band_pref;

extern mmr_cause_e_type mm_reg_cause;

extern word mm_coord_camped_cell_id;

#ifdef FEATURE_RAT_PRIORITY_LIST
extern sys_rat_pri_list_info_s_type    mm_rat_pri_list_info;
#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
extern sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_to_nw;
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */
#if defined(FEATURE_LTE)
extern boolean mm_csfb_is_blind_redir_to_lte_req_to_be_sent;
#endif

extern mmrrc_connection_status_T    rrc_connection_status;
extern mmrrc_connection_status_T    cs_session_status;
extern mmrrc_connection_status_T    ps_session_status;

extern boolean                   mm_deep_sleep_pending;

extern boolean                   mm_waiting_for_csmt_response ;

extern byte                      mm_plmn_list_req_pending;

extern boolean                 mm_manual_mode_user_selection_procedure;

extern mm_csfb_service_ind_status_enum_T mm_csfb_service_ind_status;

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
extern sys_band_mask_type        mm_ue_band_class_pref;
extern sys_band_mask_e_type      mm_active_band;
#endif

extern sys_band_class_e_type     mm_band_indicator;
extern byte                      mm_previous_serving_lai[LAI_SIZE];

extern byte                      mm_gstk_previous_serving_lai[LAI_SIZE];

extern byte                      mm_previous_idle_substate;
extern mm_as_cell_access_status_T mm_prev_cell_access;
extern boolean                   mm_hard_abort_pending;
extern boolean                   mm_deep_sleep_alt_scan;
#ifdef FEATURE_LTE
extern boolean                   mm_stop_mode_local_detach_peding;
#endif
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
extern boolean mm_lcs_active;
#endif

#ifdef FEATURE_CCBS
extern boolean mm_supports_NIMO_CM_connection;
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
extern WtoGCCO_in_progress_enum_T  WtoGCCO_state;
#endif

#ifdef FEATURE_INTERRAT_PCCO_GTOW
extern WtoGCCO_in_progress_enum_T  GtoWCCO_state;
#endif /* FEATURE_INTERRAT_PCCO_GTOW */

#ifdef FEATURE_DSAC
/********************************************************
 *Variables mm_ps_access_barred_to_unbarred and 
 *mm_ps_access_barred_to_unbarred are marked only if 
 *UE is in NMO 1 network.
 *******************************************************/
extern boolean mm_ps_access_barred_to_unbarred;
extern boolean mm_cs_access_barred_to_unbarred;

#define MM_NONE 0          /*0000 0000*/
#define MM_LU   1          /*0000 0001*/
#define MM_RAU  2          /*0000 0010*/
#define MM_COMBINED 4      /*0000 0100*/


extern byte mm_last_procedure_to_update_registration;

#endif 

extern boolean mm_gs_association_established;

/*****************************************************
*Variable mm_initiate_lu_upon_entry_into_new_cell is marked 
*only when LU got failed because of d,f,g of subclause 4.4.4.9 of
*24.0.08
*****************************************************/

extern boolean mm_initiate_lu_upon_entry_into_new_cell;

/* mm_serving_cell_id is to store the  serving cell */

extern uint32    mm_serving_cell_id;

extern boolean   mm_serving_cell_valid;

/* mm_lu_failed_cell_id is to store the cell id 
where location update got failed because of 
d,f,g of section  4.4.9 of 24.008 */

extern uint32    mm_lu_failed_cell_id;

#ifdef FEATURE_ENHANCED_NW_SELECTION
extern boolean mm_force_lu;
#endif

extern boolean mm_plmn_search_in_progress;

extern byte mm_plmn_search_transaction_id;

extern call_type_for_rrc_est_req_T call_type_for_rrc_est_req;

extern boolean mm_managed_roaming_enabled;

extern boolean mm_mmr_cell_service_ind_pending;

extern mm_attach_failed_status_type mm_mmr_attach_ind_status;
/* Flag to indicate whether IMSI stored in NV is same as the 
   one in USIM/SIM 
*/
extern boolean mm_imsi_is_native;

/* Flag to indicate whether GSM access is allowed or not */
extern boolean auth_gsm_access;

/* GSM security context for USIM */
extern boolean auth_usim_gsm_security_context ;

/* SIM/USIM access class information to be sent to RR/RRC */
extern byte mm_sim_access_class[MM_SIM_EFACC_SIZE];

/*LU reject cause for each attempt */
extern byte mm_lu_reject_cause[MAX_LU_ATTEMPT_SIZE];


extern mmgsdi_perso_status_enum_type mm_sim_perso_state;

/* Mobile operation mode */
extern byte mm_ms_opmode;

/* Flag to remember last network selection mode send to RR/RRC */
extern sys_network_selection_mode_e_type  last_requested_network_selection_mode;
/* 2 or 3 digit MNC */
extern byte mm_mnc_length;

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
extern byte mm_pending_lu_type;
#endif

#ifdef FEATURE_TC
extern boolean mm_tc_is_active;
#endif

#ifdef FEATURE_ENHANCED_NW_SELECTION
extern mm_lai_timer_info_list_info_T mm_foreground_search_lai_timer_list;

extern dword     mm_foreground_search_timer_value;
#endif
extern boolean mm_service_ind_blocked_due_to_cs_reg;
extern boolean mm_service_ind_blocked_due_to_irat_cs_reg;
extern boolean mm_connection_activated; 

#if defined(FEATURE_LTE)
#ifdef FEATURE_DUAL_SIM
extern boolean gmm_lte_deact_isr_timer_expired_sim[MAX_AS_IDS];
#define gmm_lte_deact_isr_timer_expired gmm_lte_deact_isr_timer_expired_sim[mm_sub_id]
#else
extern boolean gmm_lte_deact_isr_timer_expired;
#endif
#endif

extern boolean  mm_is_foreground_search_to_be_initiated;

extern boolean pending_gmm_msg_after_release;
extern boolean pending_mm_msg_after_release;
extern boolean gmm_procedure_rerun_ongoing;
extern boolean mm_is_gmm_moved_dereg_due_to_csreg;
#ifdef FEATURE_NAS_REL10
extern sys_plmn_id_s_type mm_3gpp_t3246_plmn_id;
extern boolean            mm_store_integrity_protected;
extern dword              mm_t3246_backoff_timer_value;
#endif
#ifdef FEATURE_NAS_REL10
extern boolean mm_is_per_ms_t3212_present;
extern dword   mm_stored_per_ms_t3212_value;
#endif
#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
extern boolean mm_is_lu_to_be_initiated_t3346_ltogw;
#endif
extern mm_pended_stop_mode_req_s_type mm_pended_stop_mode_req;

extern sys_radio_access_tech_e_type mm_service_loss_on_rat;
extern sys_network_selection_mode_e_type mm_waiting_for_list_cnf_type;
extern boolean mm_need_to_process_sim_avail_req;
extern boolean mm_waiting_for_open_session_cnf;
extern byte    mm_last_paging_cause;
extern byte mm_controls_resel_dec_mt_cs_call;
extern boolean mm_gstk_send_location_status; 
extern boolean pending_gmm_msg_valid;
#ifdef FEATURE_WRLF_SYSTEM_SEL
extern boolean mm_received_suitable_search_end_ind;
#endif

extern boolean  mm_psm_ready_req_rejected;

extern boolean  mm_is_cs_connected_due_to_srvcc;

#ifdef FEATURE_DUAL_DATA
extern boolean  mm_is_volte_only_ps;
#endif

extern boolean  mm_reject_call_on_rai_change;

#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_LTE
extern sys_mode_pref_e_type mm_sim_mode_pref_sim[MAX_NAS_STACKS];
#define  mm_sim_mode_pref mm_sim_mode_pref_sim[mm_as_id]
#endif
#endif

#if defined  FEATURE_LTE
extern uint32 mm_extend_lte_disable_duration;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern subs_capability_e_type mm_sub_capability_sim[MAX_AS_IDS];
#define mm_sub_capability mm_sub_capability_sim[mm_sub_id]
#endif


#ifdef FEATURE_DUAL_SIM
 extern mm_sms_bearer_selection_enum_T mm_sms_preferred_domain_id_sim[MAX_AS_IDS] ;
 #define  mm_sms_preferred_domain_id (mm_sms_preferred_domain_id_sim[mm_sub_id])
#else
 extern mm_sms_bearer_selection_enum_T mm_sms_preferred_domain_id ;
#endif

#ifdef FEATURE_SGLTE
  extern mm_sms_bearer_selection_enum_T mm_nv_sms_preferred_domain_id;
#endif


#if defined(FEATURE_LTE)
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern boolean mm_multimode_rat_change_pending_sim[MAX_NAS_STACKS];
#define  mm_multimode_rat_change_pending mm_multimode_rat_change_pending_sim[mm_as_id]
#else
extern boolean                   mm_multimode_rat_change_pending;
#endif
#endif

#ifdef FEATURE_LTE
#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
extern sys_ps_detach_e_type      mm_ps_detach_type_sim[MAX_NAS_STACKS];
#define  mm_ps_detach_type mm_ps_detach_type_sim[mm_as_id]
#else
extern sys_ps_detach_e_type      mm_ps_detach_type;
#endif
#endif

#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
#define MM_SUB (mm_as_id)
#ifdef FEATURE_DUAL_SIM
#define MM_LOCAL_SUB (as_id)
#else
#define MM_LOCAL_SUB 0
#endif
#endif
extern byte                      mm_nv_vamos_feature_support;
extern byte                      mm_nv_tighter_capability;
#ifdef FEATURE_DUAL_SIM
extern ue_force_rel_mode_type     mm_nv_force_sgsnr_r99_sim[MAX_AS_IDS];
#define mm_nv_force_sgsnr_r99 mm_nv_force_sgsnr_r99_sim[mm_sub_id]

extern ue_force_rel_mode_type     mm_nv_force_mscr_r99_sim[MAX_AS_IDS];
#define mm_nv_force_mscr_r99 mm_nv_force_mscr_r99_sim[mm_sub_id]
#else
extern ue_force_rel_mode_type     mm_nv_force_sgsnr_r99;
extern ue_force_rel_mode_type     mm_nv_force_mscr_r99;
#endif

extern ue_nas_rel_compliance_type    mm_nv_nas_rel_compliance;

//extern boolean                   cnm_nv_gsm_amr_call_config; //replaced
extern byte                      mm_retry_em_call_on_access_blocked_cell;
extern byte                      mm_optimised_ps_signalling_connection_release;
extern byte                      mm_ps_sig_conn_rel_after_ps_sms;
#ifdef FEATURE_NAS_REL11
extern mm_nas_nv_sim_parameters_T mm_nas_nv_sim_hfns;
#endif
extern boolean                   mm_is_qrd_device;
extern byte                      mm_managed_roaming_retry_lu_in_manual_mode;
extern byte mm_nv_wcdma_hsupa_category;
#define MM_RRC_WCMDA_HSUPA_DEFAULT_CATEGORY   6
#define MM_RRC_WCMDA_DC_HSUPA_CATEGORY   8

#ifdef FEATURE_FEMTO_CSG
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_csg_id_type     mm_serving_csg_id_sim[MAX_NAS_STACKS];
#define mm_serving_csg_id   mm_serving_csg_id_sim[mm_as_id]

extern  boolean mm_pending_no_srv_ocsgl_sim_refresh_sim[MAX_NAS_STACKS];
#define   mm_pending_no_srv_ocsgl_sim_refresh mm_pending_no_srv_ocsgl_sim_refresh_sim[mm_as_id]
#else
extern sys_csg_id_type     mm_serving_csg_id;
extern  boolean mm_pending_no_srv_ocsgl_sim_refresh;
#endif /*FEATURE_DUAL_SIM || FEATURE_SGLTE*/
extern sys_csg_support_e_type    mm_nv_csg_support;
#endif /*FEATURE_FEMTO_CSG*/


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern uint16       mm_client_activated_group_sim[MAX_NAS_STACKS];
extern uint16       mm_nas_changed_group_sim[MAX_NAS_STACKS];
#define mm_client_activated_group mm_client_activated_group_sim[mm_as_id]
#define mm_nas_changed_group mm_nas_changed_group_sim[mm_as_id]
#ifdef FEATURE_NAS_ECALL
extern mmr_ecall_mode_e_type mm_ecall_mode_type_sim[MAX_NAS_STACKS];
extern boolean mm_need_to_perform_ecall_inactivity_sim[MAX_NAS_STACKS];
extern byte mm_call_est_cause_sim[MAX_NAS_STACKS]; 
extern uint16 mm_ecall_inactivity_timer_sim[MAX_NAS_STACKS]; 
#define mm_ecall_mode_type mm_ecall_mode_type_sim[mm_as_id]
#define mm_need_to_perform_ecall_inactivity mm_need_to_perform_ecall_inactivity_sim[mm_as_id]
#define mm_call_est_cause mm_call_est_cause_sim[mm_as_id]
#define mm_ecall_inactivity_timer mm_ecall_inactivity_timer_sim[mm_as_id]
#endif
#else
extern uint16  mm_client_activated_group;
extern uint16  mm_nas_changed_group;
//extern boolean lte_supported;
#ifdef FEATURE_NAS_ECALL
extern mmr_ecall_mode_e_type mm_ecall_mode_type;
extern boolean mm_need_to_perform_ecall_inactivity;
extern byte mm_call_est_cause; 
extern uint16 mm_ecall_inactivity_timer; 
#endif
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern wcdma_rlf_state_enum_T mm_wcdma_rlf_state_sim[MAX_NAS_STACKS];
#define mm_wcdma_rlf_state  mm_wcdma_rlf_state_sim[mm_as_id]
extern boolean mm_wrlf_rat_change_pending_sim[MAX_NAS_STACKS];
#define mm_wrlf_rat_change_pending  mm_wrlf_rat_change_pending_sim[mm_as_id]
#else
extern wcdma_rlf_state_enum_T mm_wcdma_rlf_state;
extern boolean mm_wrlf_rat_change_pending;
#endif
#ifdef FEATURE_MODEM_HEAP
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern mm_cmd_type *mm_wrlf_pending_rr_message_ptr_sim[MAX_NAS_STACKS];
#define mm_wrlf_pending_rr_message_ptr mm_wrlf_pending_rr_message_ptr_sim[mm_as_id]
extern mm_cmd_type *mm_wrlf_pending_rrc_message_ptr_sim[MAX_NAS_STACKS];
#define mm_wrlf_pending_rrc_message_ptr mm_wrlf_pending_rrc_message_ptr_sim[mm_as_id]
#else
extern mm_cmd_type *mm_wrlf_pending_rr_message_ptr;
extern mm_cmd_type *mm_wrlf_pending_rrc_message_ptr;
#endif
#else
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern mm_cmd_type mm_wrlf_pending_rr_message_sim[MAX_NAS_STACKS];
#define mm_wrlf_pending_rr_message mm_wrlf_pending_rr_message_sim[mm_as_id]
extern mm_cmd_type mm_wrlf_pending_rrc_message_sim[MAX_NAS_STACKS];
#define mm_wrlf_pending_rrc_message mm_wrlf_pending_rrc_message_sim[mm_as_id]
#else
extern mm_cmd_type mm_wrlf_pending_rr_message;
extern mm_cmd_type mm_wrlf_pending_rrc_message;
#endif
#endif
extern void mmcoord_set_wrlf_state(wcdma_rlf_state_enum_T rlf_state);
#endif

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_DUAL_SIM
extern boolean mm_no_service_ind_during_split_sim[MAX_AS_IDS];
#define mm_no_service_ind_during_split mm_no_service_ind_during_split_sim[mm_sub_id]
#else
extern boolean mm_no_service_ind_during_split;
#endif
#endif

extern rex_crit_sect_type  timer_crit_sect;
extern rex_crit_sect_type  nas_nv_crit_sect;

extern rex_crit_sect_type  mm_ftd_crit_sect;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern rex_crit_sect_type  mm_sim_crit_sect;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 

extern byte     gmm_anite_gcf_flag_sim[MAX_AS_IDS];
#define gmm_anite_gcf_flag gmm_anite_gcf_flag_sim[mm_sub_id]

#ifdef FEATURE_NAS_REL11
extern boolean  mm_is_first_rrc_connection_attempt_sim[MAX_AS_IDS];
#define mm_is_first_rrc_connection_attempt mm_is_first_rrc_connection_attempt_sim[mm_sub_id]
#endif

#else 
extern byte gmm_anite_gcf_flag;

#ifdef FEATURE_NAS_REL11
extern boolean mm_is_first_rrc_connection_attempt;
#endif

#endif

#if defined(FEATURE_DUAL_SIM)|| defined(FEATURE_SGLTE)
// This variable represents the SIM subscription.
extern mm_as_id_e_type mm_sub_id_stack[MAX_NAS_STACKS];
#endif
extern mm_as_id_e_type mm_sub_id;

extern mm_as_id_e_type  mm_dds_sub_id;

#ifdef FEATURE_FEMTO_CSG
#if defined( FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern sys_csg_selection_config_e_type mm_csg_selection_config_sim[MAX_NAS_STACKS];
#define mm_csg_selection_config mm_csg_selection_config_sim[mm_as_id]
extern sys_csg_list_s_type mm_rejected_csg_list_sim[MAX_NAS_STACKS];
#define mm_rejected_csg_list mm_rejected_csg_list_sim[mm_as_id]
extern sys_csg_search_type_e_type mm_csg_search_type_sim[MAX_NAS_STACKS];
#define mm_csg_search_type mm_csg_search_type_sim[mm_as_id]
extern boolean mm_sim_ocsgl_present_sim[MAX_NAS_STACKS];
#define mm_sim_ocsgl_present mm_sim_ocsgl_present_sim[mm_as_id]
#else
extern sys_csg_selection_config_e_type mm_csg_selection_config;
extern sys_csg_list_s_type mm_rejected_csg_list;
extern sys_csg_search_type_e_type mm_csg_search_type;
extern boolean mm_sim_ocsgl_present;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#define MM_REJECTED_CSG_LIST_MAX_LENGTH 50
#endif /* FEATURE_FEMTO_CSG*/

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
#if defined(FEATURE_DUAL_SIM)|| defined(FEATURE_SGLTE)
extern boolean mm_is_sim_busy_sim[MAX_AS_IDS] ;
#define  mm_is_sim_busy   mm_is_sim_busy_sim[mm_as_id]
#else
extern boolean mm_is_sim_busy = FALSE;
#endif
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean mm_cs_auth_sent_to_card_sim[MAX_NAS_STACKS];
#define  mm_cs_auth_sent_to_card mm_cs_auth_sent_to_card_sim[mm_as_id]
#else
extern boolean mm_cs_auth_sent_to_card;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

#ifdef FEATURE_DUAL_DATA
extern uint8 mm_data_pri_value_sim[MAX_AS_IDS];
#define mm_data_pri_value mm_data_pri_value_sim[mm_sub_id]

#endif 

#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_DUAL_SIM
extern boolean mm_policy_man_suspended_sim[MAX_AS_IDS];
#define mm_policy_man_suspended mm_policy_man_suspended_sim[mm_as_id]
#else
extern boolean mm_policy_man_suspended;
#endif 
#endif


#ifdef FEATURE_DUAL_SIM
extern size_t   mm_max_active_data ;
#endif 
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
extern byte mm_stored_imei_sim[MAX_NAS_STACKS][MAX_IMEI_LEN+1];
#define mm_stored_imei (mm_stored_imei_sim[mm_as_id])

#elif defined(FEATURE_DUAL_SIM)  && defined(FEATURE_DUAL_IMEI)

extern byte mm_stored_imei_sim[MAX_AS_IDS][MAX_IMEI_LEN+1];
#define mm_stored_imei (mm_stored_imei_sim[mm_sub_id])

#else
extern byte mm_stored_imei[MAX_IMEI_LEN+1];
#endif
/*
 * Function prototypes for all MM functions
 */

void mm_send_rr_bplmn_search_abort_req( void );
void mm_send_rrc_bplmn_search_abort_req( void );
void mm_send_rrc_eq_plmn_list_change_ind( void );
void mm_send_rr_eq_plmn_list_change_ind( void );
void mm_send_rr_change_mode_ind( mm_cmd_type * );

#ifdef FEATURE_PREFERRED_MODE_CHANGE
void mm_send_rrc_change_mode_ind( mm_cmd_type * );
#endif

#ifdef FEATURE_DUAL_DATA
extern void mm_send_rr_data_priority_ind( sys_data_priority_e_type priority);
#endif

rex_sigs_type mm_wait(rex_sigs_type requested_mask);   /* Mask of signals to wait for */


extern boolean is_gsm_mode(void);
extern boolean is_umts_mode_per_nas_stacks( sys_modem_as_id_e_type as_id );
extern boolean is_gsm_mode_per_nas_stacks( sys_modem_as_id_e_type as_id );
extern boolean is_lte_mode_per_nas_stacks( sys_modem_as_id_e_type as_id );
extern boolean is_wcdma_mode_per_nas_stacks( sys_modem_as_id_e_type as_id );
extern boolean mm_is_multimode_sub(void);

extern boolean is_lte_mode(void);

void            mm_initialise( void );
boolean         mm_check_for_protocol_errors( mm_int_cmd_type_T *);
void            mm_handle_registration( mm_cmd_type *);
void            mm_perform_mm_connection_management( mm_cmd_type *);
void            mm_process_diagnostic( mm_cmd_type *);
void            mm_send_message( IMH_T *, gs_queue_id_T );
void            mm_activate_rr( void );
void            mm_stop_timer( timer_id_T );
byte            get_mm_timer_queue( void );
void            mm_deactivate_rr( void );
void            mm_clear_forbidden_LAIs( void );
void            mm_init_forbidden_LAIs( void );
void            mm_send_lu_request( auth_security_context_id );
void            mm_substate_control( mm_event_T );
void            mm_check_lu_fail_action( void );
void            mm_send_imsi_detach( void );
void            mm_abort_rr( rrc_cn_domain_identity_e_type domain_id, boolean delay_sig_con_release);
void            mm_send_rr_abort_req (boolean auth_failed_bar_cells);
void            mm_check_pending_requests( void );
void            mm_send_nv_params_to_lower_layers( void ); 
void            mm_start_timer( timer_id_T, dword );
void            mm_notify_cnm_mm_connection_failure( boolean );
void            mm_notify_reestablishment_fail( reject_type_enum_T cause_type, reject_cause_T cause_value );
void            mm_send_cm_service_abort( void );
void            mm_confirm_mm_connection( byte message_set );
void            mm_confirm_reestablish( void );
void            mm_check_for_detach( void );
auth_security_context_id
            mm_get_current_security_context( rrc_cn_domain_identity_e_type ) ;
void            mm_reestablish_connection(  auth_security_context_id ) ;
void            mm_handle_deactivation( mm_cmd_type *);
void            mm_handle_rat_change( mm_cmd_type *);
void            mm_select_plmn( mm_cmd_type *);
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
void            mm_get_plmn_list( mm_cmd_type *);
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
void            mm_handle_location_updating( mm_cmd_type *);
dword           mm_get_random_value( dword );
void            mm_secapi_get_random( uint8 *ran_temp, uint16 length );
void            mm_state_control( mm_event_T );
boolean         mm_check_forbidden_national_roaming( inter_task_lai_T *);
boolean         mm_check_forbidden_regional_roaming( inter_task_lai_T *);
void            mm_remove_forbidden_national_roaming( inter_task_lai_T *);
void            mm_remove_forbidden_regional_roaming( inter_task_lai_T *);
void            mm_add_forbidden_regional_roaming( inter_task_lai_T *);
void            mm_add_forbidden_national_roaming( inter_task_lai_T *);
void            mm_initiate_rr_connection( establishment_cause_T, IMH_T *);
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
void            mm_request_list_from_rr( mmr_plmn_search_req_s_type* plmn_srch_req );
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
void            mm_delete_lu_status( byte );
void            mm_report_low_level_failure( mm_cmd_type * );
void            mm_connection_release( mm_cmd_type * );
void            mm_return_identity( mm_cmd_type * );
void            mm_perform_authentication( void);
void            mm_perform_tmsi_reallocation( mm_cmd_type * );
void            mm_send_status( byte );
void            mm_pass_CNM_data( mm_cmd_type * );
void            mm_send_ps_cnm_data( mm_cmd_type * );
void            mm_initiate_lu( byte );
void            mm_read_imei( byte * );
void            mm_activate_state( boolean rat_change );
boolean         mm_rat_change_requested(sys_radio_access_tech_e_type rat);
boolean         mm_service_change_requested(sys_srv_domain_e_type req_serv);
void            mm_stop_rat_mode( void );
void            mm_change_service_domain( void );
void            mmcoord_process_lu_procedure( mm_cmd_type *message );
mmr_cause_e_type mm_convert_to_reg_cause( byte mm_rej_cause);
boolean         mm_has_valid_ptmsi(void);
boolean         mm_has_valid_ptmsi_signature(void);
byte            mm_get_lu_cause(void);
void            gmm_send_nv_params(void);
void            gmm_read_nv_params(void);
void            mm_rat_handle_srv_status_update ( sys_srv_status_e_type service_status );

uint8           mm_convert_semi_octets( byte value );
int             mm_convert_time_zone( byte value );

void            mm_parse_emergency_number ( mmr_emergency_num_list_s_type*     emergency_number_list,
                                            byte*                              emergency_number_list_ie );

void            mm_send_mmr_emergency_num_list_ind( mmr_emergency_num_list_s_type* emergency_number_list );

boolean         is_plmn_country_changed( byte prev_lai[], byte lai[] );

void            mm_invalidate_dsac_info(void);

void            mm_invalidate_ppac_info(void);

void            mm_process_dsac_info_from_rrc_service_cnf(rrc_service_info_type* rrc_service_info);



boolean         mm_check_if_either_cs_or_ps_cell_access_barred(void);

void            mm_process_dsac_info_from_rrc_service_ind(rrc_service_info_type* rrc_service_info);

mm_as_cell_access_status_T 
                mm_get_cs_cell_access_from_rrc_service_info
(
   rrc_service_info_type* service_info
);

mm_as_cell_access_status_T 
                mm_get_ps_cell_access_from_rrc_service_info
(
   rrc_service_info_type* service_info
);


void            mm_get_prev_dsac_info
(
   boolean                       *is_prev_dsac_info_valid,
   mm_as_cell_access_status_T    *prev_cs_cell_access,
   mm_as_cell_access_status_T    *prev_ps_cell_access
);

void            mm_process_cs_access_change
(
   boolean                    is_prev_dsac_info_valid,
   mm_as_cell_access_status_T prev_cs_cell_access
);

void            mm_process_ps_access_change
(
   boolean                    is_prev_dsac_info_valid,
   mm_as_cell_access_status_T prev_ps_cell_access
);

void            mm_update_cell_access_info_from_rrc(rrc_service_info_type* rrc_service_info);

void            mm_process_dsac_info_from_rrc(rrc_service_info_type* rrc_service_info);

boolean         mm_check_if_cs_cell_access_barred (mm_as_cell_access_status_T  cs_cell_access);

boolean         mm_check_if_cs_cell_access_allowed(mm_as_cell_access_status_T  cs_cell_access);

mm_as_cell_access_status_T
                mm_get_cs_cell_access
                (
                  nas_procedure_for_cell_access_T nas_procedure
                );
mm_as_page_rsp_e_type mm_get_page_resp_info_from_rrc_service_info
(
   rrc_service_info_type* service_info
);

boolean mm_check_if_page_rsp_allowed_valid(mm_as_page_rsp_e_type page_rsp_allowed);

boolean mm_check_if_cell_access_valid(mm_as_cell_access_status_T cell_access);

void            mm_nullify_mmr_emergency_num_list_if_needed( byte previous_plmn[], byte current_plmn[]);              

void            mm_update_mmr_emergency_num_list_if_needed( mmr_emergency_num_list_s_type* emergency_number_list );

void mm_send_mmcnm_rel_ind
(
   byte    reject_cause_value,
   boolean reestablishment_possible
);

void            mm_send_mmcnm_rel_cnf( void );
void            mm_send_mmcnm_rel_cnf_ps_sms( void );
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
void            mm_send_mmcnm_est_rej( reject_type_enum_T cause_type, reject_cause_T cause_value, mmcc_domain_name_T domain);
#else
void            mm_send_mmcnm_est_rej( reject_type_enum_T cause_type, reject_cause_T cause_value);
#endif
void            mm_send_mmcc_reest_cnf( void );
void            mm_send_mmcc_reest_rej( reject_type_enum_T cause_type, reject_cause_T cause_value );
void            mm_send_mmr_stop_mode_cnf( void );
void            mm_send_mmr_plmn_search_cnf (mmr_plmn_search_cnf_s_type* cnf);
void            mm_send_mmr_sim_available_cnf(void);
void            mm_send_mmr_sim_not_available_cnf(void);

#ifdef FEATURE_CCBS
void mm_send_mmcnm_prompt_ind(byte pd_sapi);
#endif

sys_radio_access_tech_e_type  get_preferred_mode(void);

void mm_send_ota_msg_log_packet
(
   byte   direction,
   uint32 msg_size,
   byte   *msg_ptr
);

void   mm_send_mmr_reg_cnf( void );
void   mm_send_reg_attach_failed_ind(void);
void   mm_send_reg_attach_failed_ind_if_needed(void);
void   mm_send_tmsi_reallocation_complete( void );
void   mm_send_mmr_service_ind(mmr_service_ind_s_type *serving_plmn);
dword  mm_get_timeout( timer_id_T );
void   mm_send_rr_sim_removed_req( void );
void   mm_send_rr_sim_update_req( void );
#ifdef FEATURE_GPRS_PS_HANDOVER
void   mm_send_rr_psho_cnf( void );
#endif
void mm_send_rrc_sim_update_req
(
  rrc_cn_domain_identity_e_type domain_identity,
  sys_radio_access_tech_e_type rat
);
void   mm_send_rrc_security_update_req
(
  rrc_sim_update_e_type status,
  rrc_cn_domain_identity_e_type auth_domain_identity,
  sys_radio_access_tech_e_type rat
);
void   mm_send_rrc_sim_inserted_req( sys_radio_access_tech_e_type rat );
#ifdef FEATURE_DUAL_SIM
void   mm_send_mode_pref_change_ind_to_rrc(void);
#endif
void   mm_get_ef_kc( void );
byte   mm_get_cipher_key_sequence_number(void);
void   mm_unget_ef_kc(void);
void   mm_send_mmr_camped_ind(boolean policy_change_pending);
void mm_send_cm_cipher_ind
(
   sys_srv_domain_e_type      cn_domain_ciphering_established,
   boolean                    cipher_domain_status
);
#if defined(FEATURE_LTE)
void mm_send_cm_bearer_ctxt_transfer_ind
(
   sys_radio_access_tech_e_type    rat
);
#endif

void mm_send_mmr_cell_service_ind(void);

#ifdef FEATURE_HSPA_CALL_STATUS_IND
void mm_send_mmr_hspa_call_status_ind(mmr_hspa_call_status_ind_s_type* hspa_call_status_ind);
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
void mm_send_mmr_conn_mode_service_status_ind(mmr_conn_mode_service_status_ind_s_type* conn_mode_service_status_ind);
#endif

void mm_send_mmr_reg_reject_ind
(
   sys_srv_domain_e_type      reject_domain,
   byte                       reject_cause,
   sys_plmn_id_s_type         plmn,
   sys_lac_type               tac
#ifdef FEATURE_FEMTO_CSG
   , sys_csg_id_type          csg_id
#endif 
   , sys_radio_access_tech_e_type  reject_rat
);

void mm_send_cm_ps_data_suspend_ind(void);
void mm_send_cm_mt_call_fail_ind(sys_modem_mt_call_fail_cause_type cause);
extern void mm_send_mmr_cell_access_ind(void);

extern void mm_handle_message( mm_cmd_type *message );
extern boolean mm_load_sim_data( void );
extern boolean mm_already_camped(const mm_cmd_type *message );
extern void mm_send_service_req( mm_cmd_type *message , sys_radio_access_tech_e_type rat );
extern void mm_handle_plmn_found( serving_plmn_info_T serving_plmn,
                                  boolean location_update_required );

void mm_per_subs_copy_forbidden_lai_list
(
  byte dest,
  byte src
);

#if defined (FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
/*************************************************************
FUNCTION mm_read_max_active_data

DESCRIPTION
  This function reads max active data from PM

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None
 *************************************************************/
extern void mm_read_max_active_data(void);
#endif 

/* Paging Response */
void mm_paging_response( mm_cmd_type * message );
void mm_prepare_paging_response(paging_response_msg_T *page_resp, mm_cmd_type *msg);
void mm_send_paging_control_to_update_tmsi(void);
void mm_send_paging_response
(
#ifdef FEATURE_LTE_TO_GSM
  rr_paging_type_T paging_type
#else            
  void
#endif
);

#define is_OTA_message(msg) (((msg->cmd.hdr.message_set == MS_MM_RR) || \
                              (msg->cmd.hdr.message_set == MS_MM_TDSRRC)) && \
         ((msg->cmd.hdr.message_id == (int) RR_DATA_IND) ||             \
          (msg->cmd.hdr.message_id == (int) MM_DATA_IND)))

#define get_OTA_msg_type(msg) (msg_p->cmd.rr_data_ind.layer3_message[1])


void mmgsm_store_system_information( mm_cmd_type* message);
void mmumts_process_cell_change( mm_as_service_state_T service_status,
                                 rrc_service_info_type service_info, service_ind_message_type service_ind_type);

/* Send Logs */
void mmsend_mm_state_log_packet(void);
void mmsend_mm_info_log_packet(void);
void mmsend_nas_ue_dynamic_id_log_packet(uint8 id_type);
void mmsend_nas_ue_static_id_log_packet(void);
void mmsend_gmm_state_log_packet(void);
#ifdef FEATURE_NAS_ECALL
void mmsend_timer_expiry_event(byte timer_id);
#endif
void mmsend_rat_change_event(void);

#ifdef FEATURE_FEMTO_CSG
void mmsend_mm_csg_white_list_log_packet(uint8 type, sys_csg_list_s_type *csg_list);
void mmsend_mm_csg_available_list_log_packet(sys_detailed_plmn_list_s_type * available_csg_p);
#endif 

void mmsend_request_reject_event
(
  byte                     request,
  smgmm_event_message_type message_type,
  byte                     cause
);

void mm_print_outgoing_ota_message(byte PD, byte message_type);

boolean mm_plmn_match(sys_plmn_id_s_type bcch_plmn, sys_plmn_id_s_type req_plmn);
void mmumts_process_T3212(mm_cmd_type *cmd);

void mm_handle_test_control_data(mm_cmd_type *cmd);

boolean mm_is_lai_forbidden( inter_task_lai_T lai );

void mm_update_mm_serving_plmn_info
(
  sys_sim_state_e_type  sim_state,
  sys_srv_domain_e_type reg_service_domain,
  boolean               gmm_deregistered,
  sys_srv_status_e_type service_status,
  boolean               is_gstk_event_required
);

void mm_update_last_reg_dsac_info(void);

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
sys_band_mask_type get_preferred_band_class(void);

sys_band_mask_e_type get_active_band(void);
#endif

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
void mm_abort_lcs(void);
void mm_process_lcs_session_end(void);
void mm_process_lcs_ue_pos_stored_info(mm_cmd_type *msg_p);
#endif

#ifdef FEATURE_CCBS
void mm_handle_cm_service_prompt(mm_cmd_type *msg_p);
#endif

void mm_get_mmcc_rej_cause(mm_event_T event, reject_type_enum_T *cause_type, reject_cause_T *cause_value);


void mm_add_lai_in_reject_list(inter_task_lai_T* lai);

extern void mm_clear_lai_reject_list( void );

boolean mm_plmn_in_lai_reject_list(sys_plmn_id_s_type plmn);

void mm_send_nw_sel_mode_reset_ind(sys_network_selection_mode_e_type network_selection_mode, byte msg_id);

void mm_send_mmr_ph_status_change_cnf( void );

extern void mm_send_rr_status_ind(sys_srv_status_e_type service_status, sys_srv_domain_e_type service_domain);

extern void mmcoord_handle_deep_sleep_alt_scan(void);

#ifdef FEATURE_DUAL_SIM
extern void mmcoord_handle_deep_sleep_dual_switch
(
  void
);
extern void mmcoord_process_mmr_ds_stat_chgd_req
(
  mm_cmd_type *message
);
extern void mm_send_ds_stat_chgd_ind
(
  mm_cmd_type *message 
);
extern void mm_send_mmr_ds_stat_chgd_cnf
(
  void
);
#endif

#if defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA)
boolean mm_is_UE_plmn_td_compliant(void);
boolean mm_is_UE_plmn_td_compliant_per_sub(sys_modem_as_id_e_type as_id);
#endif


#ifdef FEATURE_DUAL_DATA
void mm_update_data_priority(mm_data_pri_client_enum_T client_id, sys_data_priority_e_type priority);
#endif

extern boolean has_valid_rai(void);

#ifdef FEATURE_OOSC_USER_ACTION
extern void mmcoord_process_mmr_exit_from_pwr_save( mm_cmd_type *message );
extern void mm_send_rrc_exit_from_pwr_save( void );
#endif
extern void mm_send_mmr_reg_req_ignored_ind(sys_modem_as_id_e_type as_id);
void mm_reset_global_variables(void);
sm_cm_status_T mm_convert_gmm_to_sm_cause(byte gmm_cause);

gmmsm_fast_dormancy_cnf_cause_e_type mm_convert_rrc_to_gmmsm_cause(rrc_fast_dormancy_cnf_status_e_type rrc_cause);

extern boolean mm_has_valid_lai(void);

typedef enum
{
  PROC_NAS_LAU_EVENT,  
  PROC_NAS_ATTACH_EVENT,
  PROC_NAS_RAU_EVENT,
  PROC_NAS_MO_DETACH_EVENT,
  PROC_NAS_MT_DETACH_EVENT,
  PROC_NAS_IMSI_DETACH_EVENT
} nas_event_type ;

typedef enum
{
  PROC_START,
  PROC_RESEND,
  PROC_END
} nas_event_end_result ;

typedef enum
{
  PROC_END_NO_REASON=-1,
  PROC_END_TIME_OUT,
  PROC_END_LOWER_LAYER_FAILURE,
  PROC_END_ACCEPT,
  PROC_END_REJECTED,
  PROC_END_ACCEPT_PS_ONLY
} nas_event_end_reason ;


/*===========================================================
  Make the emm_ctrl_data_req visible to inc module (as this 
  global structure is defined in src directory)
============================================================*/
#ifdef FEATURE_LTE

typedef void(mm_send_mmr_stop_mode_cnf_fn_ptr)(void);
extern void mm_send_mmr_stop_mode_cnf(void);
typedef enum
{
  EMM_STATUS_UPDATED,
  EMM_STATUS_NOT_UPDATED,
  EMM_STATUS_ROAMING_NOT_ALLOWED
} emm_eps_update_status_type ;

/* enum of the routing information to be included in rrc conn request */
typedef enum 
{
  ROUTING_INFO_NONE,
  ROUTING_INFO_STMSI, 
  ROUTING_INFO_GUMMEI

}lte_nas_conn_req_routing_info;

/* enum of causes for normal and periodic TAU */
typedef enum 
{
  TAU_CAUSE_INVALID,
  TAU_CAUSE_TA_CHANGE,
  TAU_CAUSE_LOAD_BALANCING,
  TAU_CAUSE_PERIODIC,
  TAU_CAUSE_LOW_LAYER_FAILURE,
  TAU_CAUSE_OTHERS

}lte_nas_tau_cause_type;



#define EMM_MAX_CONTEXTS  4

typedef enum 
{
  CONTEXT_TYPE_NONE,
  CONTEXT_TYPE_PARTIAL_NATIVE_EPS,   /* context created after an AKA run */
  CONTEXT_TYPE_FULL_NATIVE_EPS,      /* context created with AKA and SMC complete or from USIM/EFS at power up */
  CONTEXT_TYPE_EPS,                  /* context created for an algo change */
  CONTEXT_TYPE_MAPPED_EPS,           /* context created for Idle / HO from UTRAN/GERAN  to E-UTRAN */
  CONTEXT_TYPE_LEGACY                /* context created for HO to UTRAN/GERAN from E-UTRAN - not used */
}lte_nas_context_type;

typedef enum
{
  CONTEXT_STATE_NONE,
  CONTEXT_STATE_CURRENT,
  CONTEXT_STATE_NON_CURRENT
}lte_nas_context_state;



typedef struct emm_security_context_info
{

  byte                          nas_kasme[32];  // NAS KASME

  byte                          nas_ul_cnt[4];  // NAS UL COUNT
    
  byte                          nas_dl_cnt[4];  // NAS DL COUNT

  byte                          nas_int_key[16]; // NAS Integrity Key

  byte                          nas_enc_key[16]; // NAS Encryption Key

  lte_nas_emm_keyset_identifier_type   nasKSI;  // Key Set Identifier

  lte_nas_emm_security_alg_type security_alg;    // NAS Integrity and Ciphering Algorithms

  lte_nas_context_type          context_type;

  lte_nas_context_state         context_state;

  boolean                       in_use;

} emm_security_context_info;

/* Structure to save the current deactivation info */
typedef struct emm_rrc_deact_req
{
  /* Current deactivation reason */
  sys_stop_mode_reason_e_type  deact_reason;

  /* Flag indicating if RRC_DEACT_REQ is pending upon aborting connection */
  boolean                      deact_req_pending;

  /* Flag indicating if RRC_DEACT_CNF is pending */
  boolean                      deact_cnf_pending;

}emm_rrc_deact_req_type;

typedef enum
{
  NAS_MM_TIN_P_TMSI,
  NAS_MM_TIN_GUTI,
  NAS_MM_TIN_RAT_RELATED_TMSI,
  NAS_MM_TIN_NONE
} nas_mm_tin_type;

typedef enum
{
  NAS_SEND_DRX_NONE,
  NAS_SEND_DRX_ON_GW_RAT,
  NAS_SEND_DRX_ON_LTE_RAT,
  NAS_SEND_DRX_ON_ALL_RATS
}nas_rat_drx_param_to_be_sent_enum_T;

typedef struct
{
  lte_nas_emm_mt_detach_type  type;
  lte_nas_emm_cause_type      emm_cause;

}emm_mt_detach_info_type;

typedef enum
{
  NAS_EMM_1xCSFB,
  NAS_EMM_3GPP_CSFB,
  NAS_EMM_1X_CSFB_BUFFERED,
  NAS_EMM_3GPP_CSFB_BUFFERED,
  NAS_EMM_CSFB_NONE
} emm_esr_type;

#ifdef FEATURE_LTE
typedef enum
{
  MM_PS_DETACH_STATE_NONE,
  MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF,
  MM_PS_DETACH_STATE_DETACH_REQ_PENDING,
  MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ,
}mm_ps_detach_state_enum_T;
#ifdef FEATURE_MODEM_HEAP
extern mm_cmd_type *mm_pending_mmr_ps_detach_req_ptr;
#else
extern mm_cmd_type mm_pending_mmr_ps_detach_req;
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern mm_ps_detach_state_enum_T  mm_ps_detach_state_sim[MAX_NAS_STACKS];
#define mm_ps_detach_state mm_ps_detach_state_sim[mm_as_id]
#else
extern mm_ps_detach_state_enum_T  mm_ps_detach_state;
#endif

typedef enum
{
  EMM_PENDING_REG_ACK_NONE = -1,
  EMM_PENDING_REG_ACK_ATTACH_COMPLETE,
  EMM_PENDING_REG_ACK_TAU_COMPLETE
}emm_pending_reg_ack_enum_T;
#endif


/* Forword declaration */
struct emm_pending_irat_ul_msg_tag; 

typedef struct
{
  word                                 msg_cnt;
  struct emm_pending_irat_ul_msg_tag  *msg_list_ptr;

}emm_pending_irat_ul_msg_list_type;

typedef enum
{
  EMM_PS_DETACH_NONE,
  EMM_PS_DETACH_REQ_PENDING,
  EMM_PS_DETACH_CNF_PENDING,
  EMM_PS_DETACH_WAITING_FOR_REG_REQ
}emm_ps_detach_status;

/* Structure to save the info of the lastest REG PS DETACH REQ */
typedef struct emm_ps_detach_info_s
{
  mmr_ps_detach_req_s_type  ps_detach_req;
  emm_ps_detach_status      status;

}emm_ps_detach_info;

typedef enum
{
  EMM_LTOG_CCO_INACTIVE,
  EMM_LTOG_CCO_IN_PROGRESS,
  EMM_LTOG_CCO_ABORTED,
  EMM_LTOG_CCO_FAILED
} emm_l2gcco_in_progress_enum_T;

typedef enum
{
  NAS_EMM_SG_ASSOC_NOT_REQ,
  NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC,
  NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC,
  NAS_EMM_SG_ASSOC_REQ_FOR_CSFB_RAU                             
}sg_association_to_be_established_enum_T;

typedef enum
{
  EMM_RADIO_CAP_UPDATE_NONE,
  EMM_RADIO_CAP_UPDATE_PENDING,
  EMM_RADIO_CAP_UPDATE_IN_PROGRESS
}emm_radio_cap_update_enum_T;


typedef struct emm_ims_registration_parameters_info_s
{
  boolean is_ims_reg_for_voice_tag;
  boolean mm_voims_dm_mo;
}emm_ims_registration_parameters_info;

typedef struct emm_eutran_ps_voice_parameters_info_s
{
  boolean is_last_eutran_voims_supported; //Network indicated value in Attach/TAU
  boolean is_ps_voice_on_eutran_supported; //UE support of IMS voice in LTE
}emm_eutran_ps_voice_parameters_info;

typedef struct mm_utran_ps_voice_parameters_info_s
{
  boolean is_last_utran_voims_supported; //Network indicated value in Attach/RAU
  boolean is_ps_voice_on_utran_supported; //UE support of IMS voice in UMTS
}mm_utran_ps_voice_parameters_info;

typedef struct emm_mobility_management_for_voims_termination_type_s
{
  emm_ims_registration_parameters_info ims_registration_parameters;
  emm_eutran_ps_voice_parameters_info eutran_ps_voice_parameters;
  //This should point to global variable mm_utran_ps_voice_parameters
  mm_utran_ps_voice_parameters_info *utran_ps_voice_parameters_ptr;
}emm_mobility_management_for_voims_termination_type;

typedef struct
{
  boolean    sms_over_nas_status; /*0-inactive,1-Active*/
  boolean    sms_over_s102_status; /*0-inactive,1-Active*/
  boolean    sms_over_ims_status; /*0-inactive,1-Active*/
}wms_sms_status_type;


typedef struct
{
  boolean    voice_call_status; /*0-inactive,1-Active*/
  boolean    video_call_status; /*0-inactive,1-Active*/
  boolean    sms_status; /*0-inactive,1-Active*/
  sys_sys_mode_e_type  sys_mode;
}mmtel_call_status_type;


typedef struct
{
  boolean    ac_Barring_r12_present; /*0-sib2 doesn't have SKIP ACB info, otherwise value is set as 1*/
  boolean    skip_ac_barring_for_volte_call; /*0-inactive,1-Active*/
  boolean    skip_ac_barring_for_video_call; /*0-inactive,1-Active*/
  boolean    skip_ac_barring_for_for_sms; /*0-inactive,1-Active*/
}emm_sib2_ac_barring_status_type;

#ifdef FEATURE_LTE_REL10
#ifdef FEATURE_DUAL_SIM
extern mm_utran_ps_voice_parameters_info mm_utran_ps_voice_parameters_sim[MAX_NAS_STACKS];
#define mm_utran_ps_voice_parameters mm_utran_ps_voice_parameters_sim[mm_as_id]
#else
extern mm_utran_ps_voice_parameters_info mm_utran_ps_voice_parameters;
#endif
#endif

typedef enum
{
  EMM_PS_RESUMPTION_NONE,
  /* This state is used for case when csfb MO call is ended by user
  ** but UE still gets redirected to G later as network is not aware of call end*/
  EMM_PS_RESUMPTION_ESR_STATE,
  EMM_PS_RESUMPTION_PENDING
}emm_ps_resumption_status_enum_T;

typedef struct mm_csfb_mt_call_info_s
{
  lte_nas_paging_identity_type ue_identity;
  boolean mt_csfb_highPriority;
  boolean is_mt_call_recv_in_conn_state;
}mm_csfb_mt_call_info_s_type;

#ifdef FEATURE_LTE_REL9
typedef struct emm_reg_ctxt_tag
{
  boolean                     guti_valid;
  lte_nas_emm_guti_type       guti;
  boolean                     last_registered_tai_valid;
  lte_nas_tai_lst1_type       last_registered_tai;
  nas_mm_tin_type             tin_type;
  emm_eps_update_status_type  update_status;

}emm_reg_ctxt_type;  
#endif

typedef struct emm_t3402_attach_rej_s
{
  boolean                    is_t3402_recv_attach_rej;
  sys_plmn_id_s_type         plmn;
}emm_t3402_attach_rej_s_type;

typedef enum
{
  MM_RPLMN_TYPE_NONE,
  MM_CS_RPLMN_TYPE,
  MM_PS_RPLMN_TYPE,
  MM_EPS_RPLMN_TYPE
}mm_rplmn_type_enum_T;

typedef struct emm_rplmn_info_s
{
  mm_rplmn_type_enum_T  rplmn_type;
  sys_plmn_id_s_type    rplmn;
}emm_rplmn_info_s_type;


typedef struct emm_nas_nv_items
{
   byte                       version;
   byte                       emm_plmn_block_on_rej_15_8; /* version 0 */
   boolean                    use_lte_disable_duration_timer_nv; /* version 1 */
   boolean                    emm_plmn_block_on_rej_15_roaming; /* version 2 */
   boolean                    psm_local_detach_enabled; /* version 3 */
   byte                       reserved[27];
} emm_nas_nv_items_T;

typedef struct emm_rpm_ftai_list
{
  lte_nas_tai_lst1_type tai;
  struct emm_rpm_ftai_list *next;
}emm_rpm_ftai_list_T;


typedef struct emm_ctrl_data_tag
{
  emm_eps_update_status_type    emm_update_status ;

  sys_plmn_service_state_s_type *plmn_service_state ;

  emm_state_type                emm_state ;
  emm_substate_type             emm_substate ;

  emm_connection_state_type     emm_connection_state ;

  /* Value in msec of T3412 */
  dword                         t3412_value ;

  /* Value in msec of T3402 */
  dword                         t3402_value     ;

  emm_t3402_attach_rej_s_type   t3402_attach_rej_param;

  /* Value in msec of T3423 (ISR) */
  dword                         t3423_value ;

#ifdef FEATURE_LTE_REL9
  dword                         t3442_value;
#endif

  rex_timer_type                emm_access_barring_emergency_timer ;

  rex_timer_type                emm_access_barring_mt_access_timer ;

  rex_timer_type                emm_access_barring_mo_signaling_timer ;

  rex_timer_type                emm_access_barring_mo_data_timer ;

#ifdef FEATURE_LTE_REL10
  rex_timer_type                emm_access_barring_mo_csfb_timer;
  rex_timer_type                emm_access_barring_delay_tolerant_timer;
#endif

  dword                         access_barring_flag ;

  /* Transaction ID of EMM/RRC primitive which is initiated because of REG
     i.e. RRC Service REQ and RRC PLMN SEARCH REQ */
  dword                         reg_init_rrc_trans_id;

  dword                         nas_data_request_transaction_id ;

  struct emm_agent_list_tag    *client_list ;

  mmtel_call_status_type        mmtel_call_status;

  lte_nas_emm_tai_lst_info_type *emm_tai_list_ptr ;

  byte                          emm_attach_attempt_counter ;

  /* attach_rej with cause ESM_FAILURE counter */
  byte                          emm_attach_rej_esm_fail_counter ;

  byte                          emm_tau_attempt_counter ;

  /* This field indicates whether updated eps_bearer_context_status 
     bit map has been delivered to the NW. This field becomes TRUE
     when eps_bearer_context_status is updated and becomes FALSE when
     UE delivers it to the NW using TAU procedure */
  boolean                       eps_bearer_context_status_updated;
  /* */
  lte_nas_eps_bearer_context_status_type        eps_bearer_context_status;    
  /* */
  lte_nas_emm_eps_update_val_type      emm_ongoing_eps_ta_update ;

  byte                          emm_detach_attempt_counter ;

  emm_mo_detach_type            ongoing_detach_type ;
  boolean                       ongoing_detach_procedure ;
  emm_detach_reason_type        detach_reason;

  boolean                       emm_guti_valid ;
  lte_nas_emm_guti_type         emm_guti ;

  lte_rrc_imsi_s                imsi ;

  word                          access_class ;

  boolean                       last_registered_tai_valid ;
  lte_nas_tai_lst1_type              last_registered_tai ;

  /* UMTS/LTE CS location information */
  location_information_T       *emm_location_area ;

  sys_plmn_id_s_type            emm_home_plmn ;

  /* 
  ** Equivalent PLMN list which is assigned to EMM by the NW
  ** upon Attach or TAU
  */
  boolean                      update_eplmn_list;
  lte_nas_emm_plmn_lst_type    emm_equivalent_PLMN_list ;

  /* forbidden tracking areas for roaming */
  lte_nas_tai_lst2_type         *rrc_forbidden_list_ptr;

  /* forbidden tracking areas for regional provision of service */
  lte_nas_tai_lst2_type         *forbidden_for_service_list_ptr ;

  /* Forbidden tracking areas list for manual selection */
  lte_nas_tai_lst2_type         *manual_ftai_list_ptr;

  /* Pointer to Mutex for FTAI list for manual selection */
  pthread_mutex_t              *manual_ftai_list_mutex_ptr;

  lte_rrc_service_ind_s         *last_rrc_service_ind_ptr;
  dword                         last_rrc_service_req_trans_id ;

  /*call establish type 0-normal 1-volte*/
  sys_lte_reestab_calltype_e    call_type;

  boolean                       reg_req_pended_indication ;
  mmr_reg_req_s_type            *reg_req_pending_ptr ;
  mm_cmd_type                  *reg_req_pending_mm_cmd_ptr ;

  /* To indicate ongoing service request procedure cause */
  lte_rrc_est_cause_e           service_req_cause ;

  /* To store the transaction ID for ESM_SERVICE_REQUEST which triggers the
     ongoing MO_DATA type service request procedure or is dropped due to the
     collision with an ongoing NON-MO_DTATA type service request procedure */
  /* Note: For ongoing service request procedure:
     - If cause is MO_DATA, EMM saves esm trans ID in esm_srv_req_trans_id and
       uses it as EMM trans ID.
     - If cause is not MO_DATA, EMM generates and uses its own trans ID
     -- If there is no MO_DATA service request trigger coming during current
        non MO_DATA service request procedure, esm_srv_req_trans_id must have
        invalid value
     -- If there is MO_DATA service request trigger coming during current
        non MO_DATA service request procedure, esm_srv_req_trans_id can be 
        used to store the ESM trans ID which can be used to notify ESM any
        service request procedure failure */
  dword                         esm_srv_req_trans_id;

  lte_phy_cell_id_t             phy_cell_id;    // type:lte_phy_cell_id_t, info needed for authentication/ cell barring

  lte_earfcn_t                  freq;           // type:lte_earfcn_t, frequency of the cell to be barred.

  lte_rrc_global_cell_id_t      cell_identity;  // Global Cell Id 

  lte_nas_emm_ue_security_type  ue_security_capability;  // UE security capabilities
  
  byte                          nonce_ue[4];     // NONCE

  byte                          SQNxorAk[6];     // Save the info for KASME computation

  byte                          autn_amf[2];     // Save the AMF information for separation bit check

  boolean                       nas_secure_exchange;

  emm_security_context_info     context_info[EMM_MAX_CONTEXTS];  // EMM maintains at least 2 context information
                                                  // one non current and one current EPS context
  boolean                       reset_nas_context;  // flag to inform RRC to reset the NAS contexts

  pthread_mutex_t              *emm_mutex_ptr;

  /* PDN_CONNECTIVITY_REQ message to be used in attach procedure */
  lte_nas_esm_pdn_connectivity_req   *pdn_conn_req_ptr;

  /* ESM transaction ID for PDN Connectivity Req message. 
     Only PDN_CONNECTIVITY_REQ with valid transaction ID can be reused in 
     attach recovery/retry/restart */
  dword                              pdn_conn_req_trans_id;

  /* Sequence number in GET PDN CONN REQ IND */
  uint8                              attach_pdn_seq_num;

  /* Structure to save the current deactivation info */
  emm_rrc_deact_req_type             rrc_deact_req;

  /* The cause of ongoing normal and periodic TAU */
  lte_nas_tau_cause_type             tau_cause;

  /* start T3440 on TAU accept if tau Req was sent in IDLE mode */
  boolean                            start_t3440_on_accept;

  /* Flag to indicate whether the EPS information storage is allowed
  or not in USIM*/
  boolean                            eps_storage_supported;

  mm_send_mmr_stop_mode_cnf_fn_ptr      *mm_send_mmr_stop_mode_cnf_fp;

  lte_rrc_deactivate_cnf_s   last_rrc_deactivate_cnf;

  nas_mm_tin_type            TIN;

  /* For i-RAT*/
  boolean                    ignore_GW_activation_ind;
  mmr_emergency_num_list_s_type   *emergency_num_list_ptr;

  lte_nas_emm_cause_type      combined_attach_cs_rej_cause;

  /* Flag indicating if SMS over LTE is active */
  boolean                     emm_sms_active;

  /* Flag indicating TAU is pending after GW attach and IRAT  */
  boolean                     tau_pending_gw_attach;

  /* Flag indicating TAU is pending to update radio capabilities to network  */
  emm_radio_cap_update_enum_T   radio_cap_update;

  /* Flag to indicate GW reject handling */
  boolean                    gw_reject_update; 

  /* lte nas outgoing msg global structure */
  lte_nas_outgoing_msg_type *out_msg_ptr;

  /* lte nas incoming message structure */
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr;

  /* lte nas ota message structure */
  byte                      *emm_ota_message_ptr;

  word                       ota_msg_size;

  /* Temporary fplmn backoff timer */
  dword                      temp_fplmn_backoff_time;

#ifdef FEATURE_1XSRLTE
  /* SRLTE ESR Timer Value read from NV */
  dword                       srlte_esr_time;
#endif

  boolean                     esr_est_conn; // set to true when RRC connection is set up through ESR

  /* NAS must not send DEACTIVATE_REQ to RRC is NAS is waiting for CONN_REL_IND from RRC
    aborting_conn flag is used to decide whether to send or not send DECATIVATE_REQ */
  boolean                    aborting_conn;

  /* Detach type and emm cause of last MT detach request */
  emm_mt_detach_info_type    mt_detach_info;

  /* store the last attempted congestion plmn*/
  sys_plmn_id_s_type        *t3346_plmn_ptr;

  /* TAI Information for last attempted TAU/ATTACH*/
  lte_nas_tai_lst1_type      last_attempted_tai;

  nas_rat_drx_param_to_be_sent_enum_T  emm_rat_drx_param_to_be_sent;

  /* Pending irat uplink message list */
  emm_pending_irat_ul_msg_list_type  pending_irat_ul_msg_list;

  /* Number of total allocated HEAP memory blocks for EMM */
  uint16  emm_heap_mem_block_num;

  nv_item_type               *emm_nv_pref_mode_read_ptr;
  boolean                    emm_umts_nv_cipher_enable_flag;
  boolean                    emm_umts_nv_integ_enable_flag;
  boolean                    emm_nv_fake_sec_flag;
  boolean                    is_srvcc_in_progress;
#ifdef FEATURE_TDSCDMA
  /* Read the TDSCDMA NV items */
  boolean                    emm_tds_nv_cipher_enable_flag;
  boolean                    emm_tds_nv_integ_enable_flag;
  boolean                    emm_tds_nv_fake_sec_flag;
#endif
  emm_esr_type               esr_type_in_progress;
  lte_nas_outgoing_msg_type*  pended_esr_msg;

  /* The registered service domain on LTE */
  sys_srv_domain_e_type      srv_domain_registered;
#ifdef FEATURE_LTE_REL9
  lte_3gpp_release_ver_e     emm_3gpp_spec_ver;
#endif
   
  /*Flag indicating if combined TA/LA update with IMSI attach is to be done
     for establishing sg association */  
  sg_association_to_be_established_enum_T  sg_association_to_be_established;

  /* Forbidden PLMN list for combined registration */
  lte_nas_emm_plmn_lst_type   combined_reg_fplmn_list;

  boolean                    update_sim_params;

  boolean                    rrc_active;

 
  /*local copy to make decision for writing to sim */
  byte                       last_known_epsloci[EPSLOCI_LENGTH];

  boolean                    imsi_attach;

  /* if the flag is set indicates refresh is not 
     yet triggered as part of RAT balancing */
#ifdef FEATURE_ENHANCED_NW_SELECTION
  boolean                    refresh_pending;
#endif

  boolean                    avoid_guti_nas_security_check;

  lte_nas_emm_attach_type    eps_attach_type;

  mmr_cause_e_type           last_reg_cause;

  emm_ps_detach_info         ps_detach_info;

  boolean                    cell_barred_tau_pending;

#ifdef FEATURE_GSTK
  /* Timer to wait for GSTK response for ATTACH PLMN validation */
  uint32                         attach_ctrl_timer;
#endif

  /* Flag indicates that manual user selection is in process 
     The flag shall be only used with Manual/Limited Manual NW selection mode */
  boolean                    manual_user_sel;

  /* This flag is used to configure the bit setting of 'ISR' field in MS_NW_CAPABILITY*/
  boolean                    isr_efs_flag;

  /* This flag is used to configure the bit setting of 'COMBINED_PROC' field in MS_NW_CAPABILITY*/
  boolean                    emm_combined_proc_efs_flag;

#ifdef FEATURE_LTE_REL9
  lte_nas_emc_srv_status_type  emc_srv_status;

  /* Structure to be used for caching/restoring registration context for 
     emeregncy attached scenarios */
  emm_reg_ctxt_type            cached_reg_ctxt;

  /* This flag is set to TRUE if upon T3218/T3220 expiry EMM sends
     NAS_EMM_DEACT_NON_EMC_BEARER_IND to DS. 5.4.2.7 in TS 24.301 */
  boolean                      non_emc_bearer_deact_in_progress;
#endif

  /* This falg is used to indicate that ph status change confirm is pending*/
  boolean                    emm_ph_status_change_cnf_pending;

  /* Pointer to the pending REG CMD received in OOS/RLF handler */
  mm_cmd_type               *pending_reg_cmd_ptr;

  /* store the last incoming sqn */
  byte last_incoming_sqn;

#if defined (FEATURE_LTE_REL9)
  /*temporarily 'emm_ctrl_data_ptr->emm_ota_message_ptr' is allocated a size dynamically 
   if UL_GENERIC_NAS_TRANSPORT size of greater than EMM_MAX_MESSAGE_LENGTH, this is reverted 
   after NAS gets L2ACK*/
  boolean                   emm_ota_message_ptr_flipped; 
#endif

  /* This is set through EFS NV item. This field is used to exclude ptmsi and guti type IEs in
     * in Attach/RAU/TAU requests */
  boolean                  exclude_identity_type_ie;


  /* This is set through EFS NV item. This field gives the lte disable duration
     * for per plmn blocking*/
  uint32                  lte_disable_duration_timer_nv;

  /* This is set through EFS NV item. This field is used to exclude/include SRVCC capability indicator and IEs 
     * Attach/RAU/TAU requests */
  boolean                  srvcc_capability_support;

  /* This is set through EFS NV item. This field is used to exclude/include 1XSRVCC capability indicator and IEs 
     * Attach/RAU/TAU requests */
  boolean                  onexsrvcc_capability_support;

  /* This is set through EFS NV item. This field is used to exclude/include CLASSMARK3 IE in 
     * Attach/TAU requests */
  boolean                  l2g_srvcc_capability_support;

#ifdef FEATURE_SGLTE
  /* This is set through EFS NV item. This field is used to exclude/include CLASSMARK3 IE in 
     * Attach/TAU requests */
  boolean                  srvcc_over_sglte_support;
#endif 

#if defined (FEATURE_LTE_REL9)
  /* This is set through EFS NV item. This field is used to exclude/include LPP capability indicator and IEs 
     * Attach/RAU/TAU requests */
  boolean                  lpp_supported;

  /* This is set through EFS NV item. This field is used to exclude/include Lcs capability indicator and IEs 
     * Attach/RAU/TAU requests */
  boolean                  lcs_supported;
#endif

  boolean w2l_psho; /* Indicates if W2L PS HO is in progress */

  /* The last received RRC ACTIVE EPS BEARER UPDATE IND */
  lte_rrc_active_eps_bearer_update_ind_s  last_rrc_bearer_ind;

  emm_l2gcco_in_progress_enum_T l2gcco_state;

  /* This is to indicate if a SET DRX REQ is pending from REG. */
  boolean                  *set_drx_req_pending_ptr;

  /* Flag indicates if EMM needs to send last received RRC bearer ind to ESM to 
     sync up DRB status */
  boolean  sync_drb_status;

  /* Flag indicates whether aggression management feature is enabled */
  boolean  aggression_management;
 /* Flag indicates whether UE has been accepted on LTE ever before*/
  boolean  is_accepted_on_lte;

#ifdef FEATURE_FEMTO_CSG
  sys_csg_list_s_type             *operator_csg_list;
  sys_csg_list_s_type             *allowed_csg_list;
  sys_csg_support_e_type     *nv_csg_support;
#endif 


#ifdef FEATURE_LTE_REL9
  /* Mobility Management for IMS voice Termination */
  emm_mobility_management_for_voims_termination_type mmit_parameters;
#endif
  /* This will point to mm_csfb_mt_call_info*/
  mm_csfb_mt_call_info_s_type *csfb_mt_call_info_ptr;

  /* reads the sglte nv value */
  boolean                       is_sglte_device;

  /* Flag indicates if EMM has pended suitable search end */
  boolean           suitable_search_end_pending;


  /*This is set if EMM is waiting for L2 ack for attach complete*/
  emm_pending_reg_ack_enum_T                       reg_complete_ack_pending;
 
  /*This is set if EMM has pending PLMN blocking */
  boolean                   is_plmn_block_req_pending;
  mmr_block_plmn_req_s_type  pending_block_plmn_req;

  /* Flag to indicate that LTE mode change is pending*/
  boolean           emm_lte_mode_change_pending;

  /* LTE connection control EFS NV item*/
  byte              lte_connection_control;
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_SVLTE_DUAL_SIM 
  lte_trm_priority_e  lte_trm_priority;
#endif

#ifdef FEATURE_DUAL_SIM
  boolean                         tau_trm_priority;
  /*
  ** This variable is used to remember if IMSI is different from the IMSI present in NV.
  ** This is used for non multimode stack to clear the data when subscription changes
  ** and it becomes a multimode stack.
  */
  boolean  imsi_not_native;
#endif

  /* updated LTE CS domain params */
  sys_lte_cs_domain_param_s_type  reg_lte_cs_domain_param;

  /*pended Reg LTE CS domain Params */
  sys_lte_cs_domain_param_s_type *reg_lte_pended_cs_domain_param_ptr;
  boolean                  cs_rat_available;

 
  /*flag for manual plmn search in connected mode*/
  boolean *conn_mode_manual_search_ptr;

  boolean plmn_search_in_progress;

#ifdef FEATURE_1XSRLTE 
  /*Flag for starting TAU or not after 1xcsfb call ended for 1xSRLTE feature*/
  boolean                         srlte_tau_needed;
  boolean                         partial_esr;
#endif

#if defined FEATURE_1XSRLTE||defined FEATURE_SGLTE ||defined FEATURE_SVLTE_DUAL_SIM 
  boolean                        current_ue_submode_srlte;
#endif
  emm_ps_resumption_status_enum_T   ps_resumption_status;
  boolean                           rr_blind_redir_pending_call_abort;
  sys_plmn_id_s_type              rlf_plmn;
  emm_rplmn_info_s_type           rplmn_info;
  boolean                  early_call_init;
  boolean                  skip_acb_for_volte_call;
  /*Feature flag for ignoreing mt csfb paging during volte call*/
  boolean                  ignore_mt_csfb_during_volte_call;
  boolean                  volte_persistent_active_flag;
#ifdef FEATURE_DUAL_SIM
  boolean                 *dds_switch_pended_ptr;
#endif
  boolean                   apn_switch;

  emm_sib2_ac_barring_status_type sib2_ac_barring_info;
  wms_sms_status_type       wms_sms_status;
  /*Feature flag for sending ESR reject when VOLTE is active*/
  boolean                  volte_rej_mt_csfb;

  emm_rpm_ftai_list_T *rpm_ftai_list_ptr;
#ifdef FEATURE_LTE_REL10
  /* Remember if the rrc_est_cause was delay tolerant while sending Attach, TAU, SR or ESR message */
  boolean             delay_tolerant_access;

  /* Flag to remember if delay tolerant access is due to MO DATA or MO SIGNALING call type so that 
     mo_data_enabled_dt can be populated in RRC connection establishment request.
     TRUE = MO DATA call type, FALSE = MO SIGNALING call type */
  boolean             mo_data_enabled_dt;

  /* Flag to remember that UE started an EMM procedure as a response to paging from NW*/
  boolean             responding_to_page;

  /* Last extended wait time from RRC in conn_est_cnf or conn_rel primitive */
  uint16              extended_wait_time;
#endif

  /* Flag for enabling local detach for PSM */
  boolean             psm_local_detach_enabled;

#ifdef FEATURE_NAS_EAB
  /* Flag to remember EAB barring status */
  boolean             eab_barring;
  /* Flag to remeber if EAB barring is supported */
  boolean             eab_barring_flag;
  /* Variable to remember last most pref OPLMN */
  sys_plmn_id_s_type  most_pref_oplmn;
#endif

#ifdef FEATURE_DUAL_DATA
/*flag to indicate if Volte is allowed on Non DDS sub */
  boolean                     *is_volte_only_ps;
#endif

/*flag to rember DS TM priority send to NAS*/
  sys_data_priority_e_type       ds_data_priority;

/* block plmn on reject cause 15 on VPLMN as well*/
   boolean                    emm_plmn_block_on_rej_15_roaming;
   sys_plmn_id_s_type         emm_lte_accepted_vplmn;
   /*block plmn if emm_plmn_block_on_rej_15_8 is true*/
   boolean emm_plmn_block_on_rej_15_8;

  /*to restart the servcie request procedure*/
  /* 24.301 5.6.1.6 Abnormal cases in the UE i)  */
  boolean restart_mt_service_req;
  emm_location_info_s_type          emm_cs_loci_info;
} emm_ctrl_data_type ;
#endif

#define MM_DEBUG_MESSAGE_BUFFER_SIZE      300

/*This is to indicate the direction of MM/EMM messages */
typedef enum 
{
  TO_MM,
  FROM_MM,
  TO_EMM, 
  FROM_EMM
}mm_emm_message_direction_type_T;

typedef struct mm_emm_debug_buffer_s
{ 
  union debug_buffer
  { 
    struct mm_info
    {
      byte message_set;
      dword message_id;
    } mm_info_debug_buffer;
#ifdef FEATURE_LTE
    struct emm_info
    {
      msgr_umid_type     msg_umid;    
    } emm_info_debug_buffer;
#endif
  } debug_buffer_type;
  byte present_mm_state;
  byte present_gmm_state;
  byte present_emm_state;
  mm_emm_message_direction_type_T mm_emm_message_direction;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_modem_as_id_e_type as_id;
  byte msg_info;
#endif
}mm_emm_debug_buffer_s_type;

#ifdef FEATURE_NAS_REL10   
typedef struct mm_backoff_nv_info_s
{
  dword t3246_value;
  dword t3346_value;
  sys_plmn_id_s_type t3246_plmn;
  sys_plmn_id_s_type t3346_plmn;
}
mm_backoff_nv_info_s_type;
#endif

extern mm_emm_debug_buffer_s_type mm_emm_debug_buffer[MM_DEBUG_MESSAGE_BUFFER_SIZE];
extern dword mm_emm_debug_buffer_index;
extern void mm_add_message_to_debug_buffer
(
  byte message_set, 
  dword message_id,
  mm_emm_message_direction_type_T mm_emm_message_direction,
  sys_modem_as_id_e_type as_id
);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern void mm_update_debug_buffer(void);
#endif

#if defined(FEATURE_LTE)
extern void emm_add_message_to_debug_buffer
(
  msgr_umid_type      msg_umid,
  mm_emm_message_direction_type_T mm_emm_message_direction,
  sys_modem_as_id_e_type as_id
);
#endif

#if defined(FEATURE_LTE)
#ifdef FEATURE_DUAL_SIM
extern boolean mm_lai_change_force_lau_for_emergency_sim[MAX_AS_IDS];
#define mm_lai_change_force_lau_for_emergency mm_lai_change_force_lau_for_emergency_sim[mm_sub_id]
#else
extern boolean mm_lai_change_force_lau_for_emergency;
#endif
extern boolean mm_nv_exclude_old_lai_type_ie;
extern sys_pch_state_s_type mm_rrc_pch_state_info;
#endif

extern boolean mm_conn_mode_manual_search_enabled;
extern boolean mm_nv_conn_mode_manual_search;
extern boolean mm_nv_disable_conn_mode_mplmn;

/*==================================================================================
 Define a Function pointer that points to all nas incoming function handlers
 (like emm_process_esm_messages, emm_process_rrc_primitive, mm_emm_process_messages)
====================================================================================*/
typedef boolean (mm_process_messages_fp)( 
                                          void *cmd_ptr,
#ifdef FEATURE_LTE 
                                          emm_ctrl_data_type *emm_ctrl_data_ptr,
#endif
                                          mm_emm_dispatcher_fp     **mm_emm_dispatcher_fn_ptr
                                        );
#ifdef FEATURE_RAT_PRIORITY_LIST
boolean mm_rat_pri_list_match
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_1, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_2
);
boolean mm_rat_pri_list_gw_match
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_1, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_2
);
boolean mm_check_if_rat_pri_list_contains_gw
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info
);
boolean mm_checkif_rat_pri_list_superset
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_message, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_info
);
sys_sys_mode_e_type mm_convert_sys_rat_to_acq_mode
(
  const sys_radio_access_tech_e_type rat
);
boolean mm_checkif_rat_pri_list_has_rat
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info,
  const sys_radio_access_tech_e_type rat
);
sys_band_mask_e_type mm_get_band_capacity
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info,
  const sys_sys_mode_e_type acq_sys_mode
);
boolean mm_checkif_rat_added_to_rat_pri_list
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_new, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_old,
  const sys_radio_access_tech_e_type req_rat
);

#endif /* FEATURE_RAT_PRIORITY_LIST*/


void mm_get_cell_service_capabilties
(
  mmr_cell_service_ind_s_type *mmr_cell_capability
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
);
#ifdef FEATURE_NAS_ECALL
extern void mm_perform_ecall_inactivity(void);
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
extern void mm_send_rrc_suitable_search_end_ind(void);
extern void mm_send_rrc_disable_bplmn_ind(const boolean disable_status);
#endif

#ifdef FEATURE_LTE 
extern void mm_csfb_process_service_ind_nmo1(mm_cmd_type *msg_p);
extern void mm_csfb_process_pending_call(void);
extern void mm_connection_establishment( mm_cmd_type *, auth_security_context_id );
extern void mm_send_rrc_csfb_call_status_ind(const boolean csfb_call_status);
extern void mm_send_rr_csfb_call_status_ind(boolean csfb_call_status,boolean mt_call_orig);
extern void mm_send_rr_mt_csfb_failure_ind(boolean depriotize_cells);

extern sys_lte_band_mask_e_type mm_get_lte_band_capacity
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info
);
#endif

#ifdef FEATURE_WCDMA
#if defined(FEATURE_DUAL_SIM)
void mm_send_rrc_ds_tuneaway_status_change_ind
(
  const boolean tuneaway_status
);
#endif
#endif


#ifdef FEATURE_LTE
extern void mmcoord_process_mmr_ps_detach_req
(
  mm_cmd_type *message
);

extern void mm_send_mmr_ps_detach_cnf
(
  void
);

extern void mm_send_mmr_set_drx_cnf
(
 boolean result
);

extern void mm_send_mmr_get_drx_cnf
(
 void
);

extern void mm_send_rrc_set_drx_ind
(
  void
);
#endif
extern void mm_send_rrc_forbidden_lai_update(void);
extern void mm_send_rr_forbidden_lai_update(void);

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION  EMM_CLEAR_MANUAL_FTAI_LIST

DESCRIPTION
  This function clears forbidden TAI list for manual selection

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_clear_manual_ftai_list(void);

/*===========================================================================

FUNCTION MM SEND FPLMN IND

DESCRIPTION
  This function sends MMR_FPLMN_LIST_CHANGE_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_mmr_fplmn_list_change_ind(sys_forbidden_list_type_e_type  list_type);


/*===========================================================================

FUNCTION MM_SEND_MMR_UPDATE_CURR_RAT_LIST_IND

DESCRIPTION
  This function sends MMR_UPDATE_CURR_RAT_LIST_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_mmr_update_curr_rat_list_ind(sys_rat_pri_list_info_s_type *rat_pri_list);

#endif

extern void mm_send_rrc_hard_abort_req( sys_radio_access_tech_e_type rat );

/*===========================================================================

FUNCTION MM_SEND_RRC_LAI_REJECT_LIST_IND

DESCRIPTION
  This function notifies LAI Reject list changes to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_lai_reject_list_ind( lai_reject_list_T* list );

/*===========================================================================

FUNCTION MM_SEND_RRC_TUNEAWAY_ALLOWED_IND

DESCRIPTION
  This function notifies tuneaway allowed indication to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_tuneaway_allowed_ind(boolean status);


/*===========================================================================

FUNCTION MM_SEND_RR_LAI_REJECT_LIST_CHANGE_IND

DESCRIPTION
  This function sends LAI Reject list changes to rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_lai_reject_list_ind( lai_reject_list_T* list );


/*===========================================================================

FUNCTION MM_IS_PLMN_VALID

DESCRIPTION
  This function checks if MCC/MNC does not consist of hexadecimal digits.
  ASN encoder is known to crash on violation, but spec allows hexadecimal digit.
  Table 10.5.3/3GPP TS 24.008 : The UE must send the MCC containing the hexadecimal digit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  None

===========================================================================*/

boolean mm_is_plmn_valid
(
  sys_plmn_id_s_type  plmn
);
#ifdef FEATURE_DUAL_SIM
boolean mm_per_subs_is_current_plmn_fplmn(sys_modem_as_id_e_type as_id);
#else
boolean mm_is_current_plmn_fplmn ( void );
#endif

#if defined(FEATURE_LTE)
extern void mm_set_mm_substate_on_irat_from_lte(void);
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
extern boolean mm_is_lai_in_foreground_search_lai_timer_list(byte *lai);
extern void mm_add_lai_to_foreground_search_lai_timer_list(byte *foreground_search_lai);
extern void mm_remove_lai_from_foreground_search_lai_list(byte * serving_lai);
extern void mm_clear_foreground_search_lai_list(void);
extern void mm_process_foreground_seach_tmer_expiry(void);
extern void mm_get_foreground_timer_expiry_lai(byte *foreground_timer_expiry_lai);
extern void mm_start_foreground_search_timer( dword num_millisecs);
extern void mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list(byte * serving_lai);
extern void mm_remove_ps_registered_lai_from_foreground_search_lai_list(void);
extern void mm_inform_updated_foreground_search_list(void);

#endif

extern void mm_add_plmn_to_fplmn_list(sys_plmn_id_s_type plmn);
extern void mm_remove_plmn_from_fplmn_list(sys_plmn_id_s_type plmn);
extern void mm_add_plmn_to_gprs_fplmn_list(sys_plmn_id_s_type plmn);
extern void mm_remove_plmn_from_gprs_fplmn_list(sys_plmn_id_s_type plmn);
extern void mm_update_cell_status(rrc_abort_cause_e_type abort_cause);
extern boolean is_ps_call_active_while_srv_status_SRV(mm_cmd_type *message);
#ifdef FEATURE_DUAL_SIM
extern void  mm_abort_connection_due_to_mplmn_on_other_subs( void );
extern boolean  mm_check_mplmn_in_progress_on_other_subs( void );
extern void  mm_send_rr_ps_resume_ind( void );
extern void  mm_send_rr_ps_abort_req( rr_ps_abort_cause_e_type cause );
#endif
extern void nasdiag_plmn_list_event 
(
        sys_detailed_plmn_list_s_type *plmn_list_ptr,

        sys_modem_as_id_e_type    as_id
);

extern boolean mm_is_registration_allowed_on_selected_plmn(mm_cmd_type *message );
extern boolean mm_is_mplmn_search_in_progress(void);
extern boolean mm_in_conn_management_state();
#ifdef FEATURE_NAS_REL10
extern boolean mm_is_plmn_equivalent_t3246_plmn(void);
extern boolean mm_is_lu_to_be_done_due_to_t3346(void);
extern void   mm_write_backoff_remaining_info_to_efs(void);
#endif
#ifdef FEATURE_NAS_REL11 
extern boolean mm_is_rrc_access_class_11_to_15(void);
#endif
extern void mm_stop_current_rat(void);
void mmsend_nas_reg_event_packet(nas_event_type nas_event,byte Status,byte end_reason,byte detach_type,byte detach_cause);


/*===========================================================================

FUNCTION  MM_MEM_ALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.

DEPENDENCIES
  None 

RETURN VALUE
  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  None

===========================================================================*/
void* mm_mem_alloc( size_t size /*!< Number of bytes to allocate */ );

/*===========================================================================

FUNCTION  MM_GET_RRC_CMD_BUF

DESCRIPTION
  This function returns a rrc_cmd_type buffer to the calling function.

  This function dynamically allocates memory for the necessary buffer.

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES
  None 

RETURN VALUE
  Returns a buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS
  None

===========================================================================*/
rrc_cmd_type *mm_get_rrc_cmd_buf( sys_radio_access_tech_e_type rat );

/*===========================================================================

FUNCTION mm_get_nas_stack_id

DESCRIPTION
  This function dervies the NAS internal stack id
  based on sub_id and stack_id passed by CM
  
DEPENDENCIES
  None

RETURN VALUE
  mm_get_nas_stack_id

SIDE EFFECTS
  None

===========================================================================*/

extern sys_modem_as_id_e_type mm_get_nas_stack_id
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
);

#ifdef FEATURE_LTE

/*===========================================================================

FUNCTION mm_send_rrc_lte_reselection_status_ind

DESCRIPTION
  This function send LTE reselection enable/disable indicaiton to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_rrc_lte_reselection_status_ind( void );

/*===========================================================================

FUNCTION MM_SEND_RR_LTE_RESELECTION_STATUS_IND

DESCRIPTION
  This function sends a rr_eq_plmn_list_change_ind to rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_rr_lte_reselection_status_ind( void );

/*===========================================================================

FUNCTION  mm_is_lte_reselection_allowed

DESCRIPTION



DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mm_is_lte_reselection_allowed(sys_plmn_id_s_type plmn_id);

#ifdef FEATURE_DUAL_DATA
/*===========================================================================

FUNCTION  mm_is_volte_registration_allowed

DESCRIPTION



DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mm_is_volte_registration_allowed(sys_plmn_id_s_type plmn_id, boolean eplmn_allowed);
#endif

/*===========================================================================
FUNCTION mm_send_mmcnm_srvcc_handover_complete

DESCRIPTION
  This function send the srvcc handover complete to CNM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_mmcnm_srvcc_handover_complete(boolean first_trigger);


/*===========================================================================

FUNCTION MM SEND CM MM SRVCC HANDOVER FAILURE IND

DESCRIPTION
  This function sends CM_MM_SRVCC_HANDOVER_FAILURE_IND message to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_cm_mm_srvcc_handover_failure_ind(void);

/*===========================================================================

FUNCTION MM_SEND_RR_CSFB_BLIND_REDIR_TO_LTE_REQ

DESCRIPTION
  This function sends a RR_CSFB_BLIND_REDIR_TO_LTE_REQ to rr. Also resets 
  mm_csfb_is_blind_redir_to_lte_req_to_be_sent global variable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_rr_csfb_blind_redir_to_lte_req(boolean call_aborted);

extern boolean mm_is_csfb_call_allowed(void);


#endif /*FEATURE_LTE*/

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION mmsim_read_operator_csg_list

DESCRIPTION
  This function start reading for operator CSG list 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void mmsim_start_read_operator_csg_list(void);

/*===========================================================================

FUNCTION  mm_output_detailed_csg_list

DESCRIPTION
  Function to output detailed CSG list


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_output_detailed_csg_list
(
  void
);

/*===========================================================================

FUNCTION  mm_is_csg_in_white_list

DESCRIPTION



DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mm_is_csg_in_white_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_info_s_type  *csg_info
);

/*===========================================================================

FUNCTION  mm_is_csg_in_allowed_list

DESCRIPTION


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mm_is_csg_in_allowed_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id
);

/*===========================================================================

FUNCTION  mm_is_csg_in_operator_list

DESCRIPTION
  This function checks if CSG,PLMN is present in operator CSG list

DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If CSG,PLMN is present
  FALSE: IF CSG,PLMN is not present

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mm_is_csg_in_operator_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id,
  boolean           allow_rejected
);
/*===========================================================================

FUNCTION  mm_timer_frobidden_csg_list_expiry

DESCRIPTION
This function handles operator CSG timer expiry

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

extern void mm_timer_forbidden_csg_list_expiry
(
  void
);

/*===========================================================================

FUNCTION  mm_add_csg_to_white_list

DESCRIPTION



DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_add_csg_to_white_list
(
  sys_plmn_id_s_type  plmn_id,
  sys_csg_info_s_type  *csg_info
);

/*===========================================================================

FUNCTION  mm_timer_frobidden_csg_list_expiry

DESCRIPTION


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

extern void mm_timer_forbidden_csg_list_expiry
(
  void
);

/*===========================================================================

FUNCTION  mm_delete_csg_from_white_list

DESCRIPTION



DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_delete_csg_from_white_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_info_s_type  *csg_info
);

/*===========================================================================

FUNCTION  mm_prioritize_csg_list

DESCRIPTION



DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_prioritize_csg_list
(
  sys_detailed_plmn_list_s_type  *available_csg
);

/*===========================================================================

FUNCTION mm_send_rrc_csg_white_list_ind

DESCRIPTION
  This function sends white list to WRRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_rrc_csg_white_list_ind
(
  sys_csg_list_s_type *operator_list, 
  sys_csg_list_s_type * allowed_list
);

/*===========================================================================

FUNCTION mm_send_rr_csg_white_list_ind

DESCRIPTION
  This function sends white list update to GRr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_rr_csg_white_list_ind(void);

/*===========================================================================

FUNCTION mm_check_camped_csg_not_white_list

DESCRIPTION
This function check if camped cell became forbidden. If yes send no service to CM. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_check_camped_csg_not_white_list(void);

/*===========================================================================
FUNCTION  mm_is_csg_in_rejected_list

DESCRIPTION
  This function checks if CSG,PLMN is present in rejected CSG list

DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If CSG,PLMN is present
  FALSE: IF CSG,PLMN is not present

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mm_is_csg_in_rejected_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id
);

/*===========================================================================
FUNCTION  mm_add_csg_to_rejected_list

DESCRIPTION
 This function adds a CSG,PLMN to the rejected CSG list

DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If CSG is added to the rejected CSG list
  FALSE: If CSG is not added to the rejected CSG list

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mm_add_csg_to_rejected_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id,
  byte              reject_cause
);

/*===========================================================================
FUNCTION  mm_clear_rejected_csg_list

DESCRIPTION
 This function clears the rejected CSG list

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_clear_rejected_csg_list
(
  void
);

/*===========================================================================
FUNCTION  mm_remove_csg_from_rejected_list

DESCRIPTION
 This function removes the specified CSG,PLMN entry from the rejected CSG list

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_remove_csg_from_rejected_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id
);

/*===========================================================================
FUNCTION  mm_process_csg_search_list

DESCRIPTION
  This function processes the CSG search list returned from AS

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_process_csg_search_list
(
  sys_csg_search_type_e_type     csg_search_type,
  sys_detailed_plmn_list_s_type  *available_csg_p
);

/*===========================================================================
FUNCTION  mm_sort_csg_list_config_1

DESCRIPTION
 This function sorts the CSG search list returned by AS as per CSG
 selection configuration #1

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_sort_csg_list_config_1
(
  sys_detailed_plmn_list_s_type  *available_csg_p
);

/*===========================================================================
FUNCTION MM_SEND_MMR_CSG_SEARCH_TRIGGER_IND

DESCRIPTION
  This function sends MMR_CSG_SEARCH_TRIGGER_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_send_mmr_csg_search_trigger_ind
(
  void
);

/*===========================================================================
FUNCTION mm_send_mmr_csg_list_read_ind

DESCRIPTION
  This function sends MMR_CSG_LIST_READ_COMPLETE message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_send_mmr_csg_list_read_complete_ind(void);
#endif /*FEATURE_FEMTO_CSG*/


#ifdef FEATURE_LTE

/*===========================================================================

FUNCTION  mm_check_if_rat_removed_from_rat_pri_list

DESCRIPTION
  This function checks if the requested rat is removed from the new rat list and  is present in 
  old one
  
DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mm_check_if_rat_removed_from_rat_pri_list
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_new, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_old,
  const sys_radio_access_tech_e_type rat
);


/*===========================================================================

FUNCTION  mm_remove_rat_from_rat_pri_list

DESCRIPTION
  Removes the requested rat from the rat priority list passed as
  the parameter 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mm_remove_rat_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info,
  const sys_sys_mode_e_type rat
);

#endif

/*===========================================================================

FUNCTION MM_SEND_SEND_SM_UNBLOCK_ALL_APN

DESCRIPTION
  This function sends GMMSM_UNBLOCK_ALL_APN to SM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_sm_unblock_all_apn(void);

#if defined(FEATURE_LTE) && defined(FEATURE_DUAL_SIM)
/*===========================================================================

FUNCTION MM_SEND_SM_MULTIMODE_SUBS_CHGD_REQ

DESCRIPTION
  This function sends multimode_subs_chgd_req to SM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_sm_multimode_subs_chgd_req
(
  mm_as_id_e_type multimode_as_id
);
#endif


/*===========================================================================

FUNCTION MM_TIMER_NON_DEFERRABLE

DESCRIPTION
  This function takes the timer id as input and returns TRUE if timer should
  be non-deferrable

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Timer is non-deferrable
  FALSE: Timer is deferrable

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mm_timer_non_deferrable( timer_id_T timer_id );

/*===========================================================================
FUNCTION MM_SEND_EMPTY_MMR_PLMN_SEARCH_CNF

DESCRIPTION
  This function sends MMR_PLMN_SEARCH_CNF message to the REG with 0 PLMNs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_send_empty_mmr_plmn_search_cnf
( 
  mmr_plmn_search_req_s_type  *plmn_search_req_ptr,
  reg_mm_plmn_search_status_e_type    search_result
);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION mm_send_rr_multimode_subs_chgd_req

DESCRIPTION
  This function sends cm_multimode_subs_chgd_req to RR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_rr_multimode_subs_chgd_req(subs_capability_e_type*  subs_capability);

extern void mm_send_rrc_multimode_subs_chgd_ind
(subs_capability_e_type*  subs_capability);
/*===========================================================================

FUNCTION mm_send_mmr_multimode_subs_chgd_cnf

DESCRIPTION
  This function sends multimode_subs_chgd_cnf to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_send_mmr_multimode_subs_chgd_cnf( void );
#endif
/*===========================================================================

FUNCTION: mm_clear_manual_mode_user_selection_proc_flags

DESCRIPTION:   To clear MM/GMM/EMM manual_mode_user_selection flag

DEPENDENCIES:

PARAMETERS: 

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/
extern void mm_clear_manual_mode_user_selection_proc_flags();

extern void mm_send_rr_pseudo_list_update_ind(boolean can_use_pseudo);
#ifdef FEATURE_TDSCDMA
extern void mm_send_tdsrrc_pseudo_list_update_ind(boolean can_use_pseudo);
#endif
extern void mm_send_rrc_pseudo_list_update_ind(boolean can_use_pseudo);



#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
/*************************************************************
FUNCTION mm_read_device_configuration

DESCRIPTION
  This function reads device configuration from PM

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None
 *************************************************************/
extern void mm_read_device_configuration
(
  void
);
#endif 
extern boolean  mm_per_nas_stacks_is_mt_page_in_progress
(
  sys_modem_as_id_e_type stack_id_nas
);

extern void mm_convert_rat_to_tech_id(sys_radio_access_tech_e_type  rat_arg, sys_sys_mode_e_type *tech_id_arg);

#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#endif

#ifdef FEATURE_SGLTE_DUAL_SIM
boolean  mm_get_verify_ps_transfer_conditions
(
  sys_modem_as_id_e_type as_id
);
#endif

/*************************************************************
FUNCTION mm_check_other_mm_cnm_connections

DESCRIPTION
  This is a DUAL_SIM function. This API checks if there is any other
  subscription for which MM state is 14. If yes, abort connection establishment
  procedure on that subscription.

DEPENDENCIES
  None

Input

RETURN VALUE
  None


SIDE EFFECTS
  None
 *************************************************************/
extern void mm_check_other_mm_cnm_connections(void);

#ifdef FEATURE_SGLTE
extern boolean mm_sglte_activation_ind_allowed(void);
extern void mmcoord_process_stop_mode_on_hybrid_stack(void);
extern void mm_if_needed_abort_plmn_search_and_loop_back_to_reg( mmr_reg_req_e_type req_type);
#endif
extern void mm_check_for_null_ptr(void* msg_p);
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
extern void mm_read_carrier_specific_efs(void);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*************************************************************
FUNCTION mm_process_mcfg_refresh

DESCRIPTION
  This function processes MCFG NV refresh indication for MM specific
  EFS/NVs

DEPENDENCIES
  None

Input

RETURN VALUE
  None


SIDE EFFECTS
  None
 *************************************************************/
extern void mm_process_mcfg_refresh(void);
#endif


#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
extern void mm_save_rat_pri_list_sent_to_nw(void);
#endif

/*===========================================================================
FUNCTION mm_send_mmr_t3346_timer_ind

DESCRIPTION
  This function sends MMR_T3346_TIMER_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_send_mmr_t3346_timer_ind(const sys_nas_t3346_status_e_type t3346_notification);

extern void mm_clear_serving_lai_on_moving_to_lte(void);
#ifdef FEATURE_DUAL_SIM
extern boolean mm_check_lu_allowed(void);
#endif

/*===========================================================================
FUNCTION MM_SEND_MMR_PSM_READY_RSP

DESCRIPTION
  This function sends MMR_PSM_READY_RSP message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_send_mmr_psm_ready_rsp
(
  sys_psm_status_e_type status  
);

/*===========================================================================
FUNCTION MM_SEND_MMR_PSM_READY_IND

DESCRIPTION
  This function sends MMR_PSM_READY_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mm_send_mmr_psm_ready_ind
(
  void
);

extern void mm_local_cs_detach(void);

/*===========================================================================
FUNCTION MM_CHECK_READY_FOR_PSM

DESCRIPTION
  This function checks if NAS is ready to enter PSM mode

DEPENDENCIES
  None

RETURN VALUE
   PSM ready status

SIDE EFFECTS
  None
===========================================================================*/

extern sys_psm_status_e_type mm_check_ready_for_psm(void);


#ifdef FEATURE_TDSCDMA
/*===========================================================================
Function: mm_suspend_policyman

Description:
This function sends suspend indication to policman

============================================================================*/
extern void mm_suspend_policyman(void);

/*===========================================================================
Function: mm_resume_policyman

Description:
This function sends resume indication to policman

============================================================================*/
extern void mm_resume_policyman(void);
#endif


#endif /* _MM_V_H_ */
