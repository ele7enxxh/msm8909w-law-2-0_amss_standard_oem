#ifndef _MM_H_
#define _MM_H_
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm.h_v   1.47   17 Jul 2002 12:29:30   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
07/21/11   jbk     Adding EFS items to mm_nas_nv_items under Version 1
07/21/11   PM      Adding EFS item to mm_nas_nv_items under Version 2
07/21/11   jbk     Adding EFS item to mm_nas_nv_items under Version 3
11/03/11   ss      Adding EFS item to mm_nas_nv_items under Version 4
02/17/12   gps     Creating a new EFS item Ss_Screening_Indicator
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>
#include <string.h>
#include "customer.h"
#include "comdef.h"
#include "environ.h"
#include "timers.h"
#include "gs.h"
#include "mm_mm.h"
#include "mmtask.h"
#include "mmtask_v.h"
#include "reg_mm.h"
#include "com_iei.h"
#include "mm_mm.h"
#include "mm_rr.h"
#include "sys_plmn.h"
#include "sys.h"
#include "sys_v.h"

#include "sys_plmn_selection.h"
#include "mm_as_types.h"
#include "naslog.h"
#include "cmwll.h"
#include "reg_mm.h"
#include "rex.h"



/* ------------------------------
**  ALS ITC VALUE
** --------------------------- */

typedef enum
{
  ALS_ITC_VALUE_4 = 0,
  ALS_ITC_VALUE_6 = 1
} als_itc_value_type;

/* defines the 15 digits represented in 8 bytes */
#define IMSI_LEN_IN_BYTES  8 
typedef struct imsi_data_tag
{
   byte              length;
   byte              data[IMSI_LEN_IN_BYTES]; 
} imsi_data_T;

typedef struct mm_system_information_tag
{
   dword                      T3212_timer_value;
   boolean                    ATT;
   boolean                    reestablishment_allowed;
   byte                       mscr;
   boolean                    sgsnr;
   mm_as_cell_access_status_T cell_access;

   boolean                    dsac_present;
   mm_as_cell_access_status_T cs_cell_access;
   mm_as_cell_access_status_T ps_cell_access;
   boolean                        ppac_present;

   boolean                        cs_reg_allowed;

   boolean                        ps_reg_allowed;

   mm_as_page_rsp_e_type          page_rsp_allowed;


  
} mm_system_information_T;

typedef struct mm_nas_nv_items
{
   byte                       version;
   byte                       als_itc_value;
   byte                       No_auto_answer_on_hold;
   byte                       MT_Call_reject_cause;
   byte                       UE_init_rel_on_spurious_page;
   byte                       CS_Domain_absolutely_preferred;
   /* Ver 1 */
   byte                       cb_p_drx_enabled; /* Index 6 */
   byte                       cb_p_drx_pre_drx_length; /* Index 7 */
   byte                       cb_p_drx_first_drx_periodicity; /* Index 8 */
   byte                       cb_p_drx_second_drx_periodicity; /* Index 9 */
   /* Ver 2 */
   byte                       cc_preserve_ui_speech_codec_preference; /* Flag to indicate whether to preserve UI speech codec preferences or not */
   /* Ver 3 */
   byte                       cb_duplication_detection_disabled;/* If true, stop discarding CB retransmissions at NAS */
   /* Ver 4 */
   /* GSM CB Feature OFF/ON */
   byte                       cb_gsm_feature_disabled;    /* Index 12 */
   /*Ver 5*/
   byte                       cb_drx_info_validation_enabled; /* Index 13 */
   /*Ver 6*/
   byte                       ss_screaning_indicator; /*index 14*/ 
   /*Ver 7*/                
   byte                       retry_em_call_on_access_blocked_cell;  /*index 15 */   
   /*Ver 8*/ 
   byte                       mm_managed_roaming_retry_lu_in_manual_mode;  /*index 16 */ 
   /*Ver 9*/ 
   byte                       optimised_ps_signalling_connection_release;  /*index 17 */ 
   /*Ver 10*/
   byte                       ps_sig_conn_rel_after_ps_sms; /*index 18*/
   /*Ver 11*/
   byte                       treserved; /*index 19*/	 
   /*Ver 12*/
   byte                       long_ftn_support; /*index 20*/

   byte                       reserved[5];

} mm_nas_nv_items_T;

typedef struct mm_nas_nv_classmark_ie
{
   byte                       version;
   byte                       geran_to_eutra_measurement_support;
   byte                       eutra_measurement_reporting_support;
   byte                       tighter_support;
   byte                       reserved[20];
}mm_nas_nv_classmark_ie_T;

typedef struct mm_nas_feature_to_be_configured
{
   byte                       version;
   byte                       als_itc_value;
   byte                       No_auto_answer_on_hold;
   byte                       MT_Call_reject_cause;
   byte                       UE_init_rel_on_spurious_page;
   byte                       CS_Domain_absolutely_preferred;
} mm_nas_feature_to_be_configured_T;

typedef struct mm_nas_nv_sim_parameters
{
  byte version;
  byte start_hfn[6];
  byte threshold[3];
  byte reserved[24];
}mm_nas_nv_sim_parameters_T;

/* --------------------------------------------------------
** Enum type for UE revision levele indicator. This could be
PRE_REL99, REL99 or REL5 based on the UE NV setting and the
NW revision number.
** ----------------------------------------------------- */

typedef enum
{
  UE_REV_PRE_REL99,
  UE_REV_REL99
} ue_rev_level_indicator_type;


#if defined(FEATURE_SGLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
typedef PACKED struct PACKED_POST
{
   byte                       version;
   uint16 ps_timer_value;
   uint16 act_gstack_timer_value;
   byte reserved[3];
} nas_sglte_feature_config_T;
extern rex_timer_cnt_type mm_get_ps_timer_value(void);
#endif
extern boolean is_umts_mode(void);
extern boolean is_tdscdma_mode(void);
extern boolean is_wcdma_mode(void);

extern sys_gw_conn_status_e_type mm_get_connection_status(void);
extern sys_gw_conn_status_e_type mm_per_subs_get_connection_status(sys_modem_as_id_e_type as_id);

extern sys_gw_conn_status_e_type mm_per_stacks_get_connection_status
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
);


extern sys_activation_allowed_e_type mm_per_subs_is_DO_activation_allowed
(
  sys_modem_as_id_e_type sub_id
);

extern boolean mm_get_optimized_ps_signaling_connection_release(void);

extern void mm_log_mm_reg_variables(void);

extern mm_nas_feature_to_be_configured_T mm_nas_feature_to_be_configured;

extern sys_modem_as_id_e_type mm_get_nas_stack_id
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
);

void    mm_set_signal_for_cm(uint16 bit_mask);

extern boolean mm_get_nas_stats_info(sys_nas_information_s_type* nas_information);
void    mm_set_nas_bit_mask(uint16 bit_mask);

void mm_per_subs_set_nas_bit_mask(uint16 bit_mask, sys_modem_as_id_e_type as_id);
void mm_per_stacks_set_nas_bit_mask
(
  uint16 bit_mask,
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type  stack_id
);

extern boolean mm_per_subs_get_nas_stats_info(sys_nas_information_s_type* nas_information, sys_modem_as_id_e_type as_id);
extern boolean mm_per_stacks_get_nas_stats_info
(
  sys_nas_information_s_type* nas_information, 
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
);

extern boolean mm_check_lai_in_reject_list(inter_task_lai_T lai);

extern boolean reg_mode_is_usim_compliant_for_cm
(
  sys_modem_as_id_e_type as_id
);


extern boolean mm_per_subs_check_lai_in_reject_list
(
  inter_task_lai_T mm_lai,
  sys_modem_as_id_e_type as_id
);


/*===========================================================================

FUNCTION MM_GET_IMSI

DESCRIPTION
  This function returns IMSI as stored in NAS context but returns null data if SIM is not available

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  imsi_out will have IMSI value

===========================================================================*/
extern void mm_get_imsi(imsi_data_T* imsi_out);


/*===========================================================================

FUNCTION MM_DS_GET_IMSI

DESCRIPTION
  DUAL SIM: This function returns IMSI as stored in NAS context but returns null data if SIM 
  is not available. 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  imsi_out will have IMSI value

===========================================================================*/
extern void mm_ds_get_imsi
(
  imsi_data_T* imsi_out,
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION MM_IS_MT_PAGE_IN_PROGRESS

DESCRIPTION
  This function returns true if MT paging is in progress.MMODE uses ir to block mode pref change.

DEPENDENCIES
  None 

RETURN VALUE
  True: MT paging in progress

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_mt_page_in_progress(void);
extern boolean  mm_per_subs_is_mt_page_in_progress(sys_modem_as_id_e_type as_id);
extern boolean mm_sub_is_dds_sub(sys_modem_as_id_e_type as_id);
extern boolean  mm_per_stacks_is_mt_page_in_progress
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
);


/*===========================================================================

FUNCTION MM_IS_MO_SMS_IN_PROGRESS

DESCRIPTION
  This function checks whether MO SMS is going on or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mm_is_mo_sms_in_progress(void);
extern boolean mm_per_subs_is_mo_sms_in_progress(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION: mm_cs_registration_vote_for_reselection()

DESCRIPTION:    Vote No RESEL if LAU is on going or PAGE is being responded

DEPENDENCIES:

PARAMETERS: as_id

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/

nas_vote_for_reselection_e_type mm_cs_registration_vote_for_reselection(sys_modem_as_id_e_type as_id);


/*===========================================================================
  
FUNCTION: gmm_ps_registration_vote_for_reselection()
  
DESCRIPTION:	  Vote No RESEL if RAU/ATTACH is on going or PAGE is being responded
  
DEPENDENCIES:
  
PARAMETERS: as_id
  
RETURN VALUE:
  
SIDE EFFECTS:
             None
===========================================================================*/
  
nas_vote_for_reselection_e_type gmm_ps_registration_vote_for_reselection(sys_modem_as_id_e_type as_id);

extern boolean mm_per_stacks_is_mo_sms_in_progress
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
);


#endif /* _MM_H_ */
