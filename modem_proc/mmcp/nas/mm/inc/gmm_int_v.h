#ifndef GMM_INT_V_H
#define GMM_INT_V_H
/*===========================================================================

              GPRS MOBILITY MANAGEMENT (GMM) INTERNAL HEADER FILE

DESCRIPTION
  This file data and function declarations necessary for the GMM sub-system
  of the UMTS/GSM MM task.

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmm_int.h_v   1.13   09 Jul 2002 13:21:12   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/gmm_int_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/01   jca     Created file
05/11/01   jca     Added new data types and functions.
05/23/01   jca     Changed some types to be compatible with OTA message format.
06/08/01   jca     Added gmm_power_down_active regional variable.
08/03/01   jca     Added new data types.
09/07/01   jca     Corrected numeric values for GPRS Detach Type definitions.
11/06/01   jca     Added new variable for PS signalling connection.
06/05/02   ks      Added support for Dual mode,Consolidated gmm PLMN,LAC & RAC
                   into Serving_plmn type. Removed gmm_handle_activation.
07/06/02   vdr     Added new external definition in order to support GMM-LLC interface.
07/09/02   vdr     Fixed LINT error.
10/09/02   jca     Fixed some bugs related to RR(C)_SERVICE_IND processing.
11/06/02   jca     Added function gmm_update_mm_serving_plmn_info().
02/27/03   vdr     Added prototypes in order to support UMTS to GSM RAT changes
02/27/03   mks     Added function prototypes gmm_sm_send_rat_change_req() &
                   gmm_sm_send_rat_resume_req for supporting PS data continuity
                   feature.
03/07/03   vdr     Added function gmm_process_cm_req() and some external definitions
                   required by PS data continuity feature
03/31/03   jca     Externalized function gmm_initiate_gprs_detach() and
                   added boolean variable gmm_gprs_detach_pending.
04/14/03   jca     Added boolean variable gmm_ra_update_pending.
04/29/03   jca     Added gmm_previous_network_op_mode variable.
05/29/03   jca     Added boolean variable gmm_follow_on_request.
06/10/03   ks      Added support for default values for GPRS NV Items.
06/19/03   ks      Passing the RAT info in the gmm_sm_send_rat_change_req ().
                   Made the GPRS NV items as external.
07/09/03   vdr     Removed prototype of the gmm_ptmsi_allocated_rai variable
07/16/03   jca     Added boolean variable gmm_gprs_attach_pending.
08/13/03   jca     Added boolean gmm_detach_after_rau_pending.
08/28/03   ks      Modified the GPRS_NON_DRX_TIMER value from 0x3 to 0x0
10/22/03   jca     Added function prototype gmm_change_of_network_mode_of_operation().
12/03/03   vdr     Added prototype for gmm_ptmsi_sig_tobe_deleted
12/11/03   jca     Added regional variables gmm_previous_serving_rai
                   and gmm_previous_gmm_substate.
02/20/04   vdr     Added processing required by READY timer negotiation procedure
04/08/04   vdr     Added prototype for gmm_inter_rat_follow_on_ind
05/12/04   jca     Added boolean variable gmm_directed_signalling_reestablishment.
06/18/04   jca     Added variable gmm_pending_ra_update_type.
11/02/04   sv      Added default values for EDGE features supported.
01/05/05   ks      Added new definition for GPRS_EDGE_FEATURE_NOT_SUPPORTED
01/07/05   sv      Changed the default value of EDGE_MULTISLOT_CLASS from '1' to '8'.
02/15/05   sv      Enabled 8PSK uplink modulation - changed the default value.
02/17/05   sv      Added support for Rel-4.
03/21/05   sv      Modified the EDGE Multislot class of the phone to '10'.
06/17/05   jca     Added variable gmm_previous_gprs_service_state.
09/27/05   abe     Modified GPRS_EDGE_NW_ASS_CELL_CHANGE to 0x01
11/11/05   ss      Added gmm_service_request_data_accepted for Rel5.
10/06/05   ajt     Declaration for mm_lcs_domain and its type
12/15/05   abe     Supported GSM DTM under FEATURE_GSM_DTM
02/02/06   up      Adding GPRS_INTERRAT_NACC_SUPPORT NV item for RR.
02/24/06   abe     Adding support for FEATURE_REL_6_SUPPORT
03/02/06   up      Default value of INTERRAT NACC is set to 0x03.
05/19/06   npr     Default value of GPRS_EDGE_DTM_MULTISLOT_SUBCLASS set to 0x3 (Class 11)
06/14/06   up      Modified prototype gmm_update_mm_serving_plmn_info.
07/14/06   pk     Updated for Feature DSAC
11/27/06   vsm   added the prototype for the function gmm_update_ra_type.
02/26/08   rk     removed gmm_inter_rat_follow_on_ind as its not used.
10/10/11   HC      Moved GPRS_MULTISLOT_CLASS and GPRS_EDGE_MULTISLOT_CLASS from apis(gmm_int.h) 
                   to inc(gmm_int_v.h) folder  
10/31/11   mrk     reverted above "GPRS_MULTISLOT_CLASS and GPRS_EDGE_MULTISLOT_CLASS" changes
12/21/11   gps     FREE FLOAT FEATURE changes 
03/13/12   hc     reverted CL 2011080, moved GPRS_MULTISLOT_CLASS and GPRS_EDGE_MULTISLOT_CLASS 
                    definitions form gmm_int.h 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "gmm_int.h"
#include "gmm_v.h"
#include "mmtask_int.h"
#include "ds3gpp_api.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* ------------------------------
** PS signaling connection states
** ------------------------------ */
typedef enum
{
  PMM_DETACHED,
  PMM_IDLE,
  PMM_CONNECTED
} pmm_mode_type;

#ifdef FEATURE_REL6_PS_FOP

/*-----------------------------
** PS fop resp states
**-----------------------------*/
typedef enum
{
  PS_FOP_NW_NOT_GRANTED,
  PS_FOP_NW_GRANTED,
  PS_FOP_UE_NOT_GRANTED_NW_GRANTED_BEFORE_CONN_REL,
  PS_FOP_UE_NOT_GRANTED_NW_NOT_GRANTED_BEFORE_CONN_REL
}ps_fop_resp_type;
#endif

#ifdef FEATURE_REL_6_SUPPORT
#if (defined(FEATURE_GSM_EGPRS_SAIC) && defined(FEATURE_GSM_MDSP_AEQ))
#define GSM_EGPRS_SAIC_SUPPORTED          0x1  /* FEATURE_GSM_MDSP_AEQ brings SAIC support in Data so switch it on */ 
#else
#define GSM_EGPRS_SAIC_SUPPORTED          0x0  
#endif /* FEATURE_GSM_EGPRS_SAIC */
#if defined(FEATURE_GSM_R_SACCH) && defined(FEATURE_GSM_R_FACCH)
#define GSM_R_ACCH_SUPPORTED            0x1 /* R_SACCH  and  R_FACCH is supported by default */
#elif defined (FEATURE_GSM_R_FACCH) 
#define GSM_R_ACCH_SUPPORTED            0x2 /* R_FACCH is supported by default */
#endif /* FEATURE_GSM_R_SACCH and FEATURE_GSM_R_FACCH*/
#endif /* FEATURE_REL_6_SUPPORT */

/* -------------------------------------
** GPRS NV PARAMETER DEFAULT VALUES
** ------------------------------------- */

#ifdef FEATURE_GPRS_EDA
#if defined (FEATURE_GSM_GPRS_MSC34) && defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
  #define GPRS_MULTISLOT_CLASS            0x21  /* Code for Multi-slot class 33 */
#else
  #define GPRS_MULTISLOT_CLASS            0xC  /* Code for Multi-slot class 12 */
#endif
#else
  #define GPRS_MULTISLOT_CLASS            0xA  /* Code for Multi-slot class 10 */
#endif /* FEATURE_GPRS_EDA */

#ifdef FEATURE_GPRS_EDA
#if defined (FEATURE_GSM_GPRS_MSC34) && defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
  #define GPRS_EDGE_MULTISLOT_CLASS       0x21  /* Currently supported EDGE multislot class is '33' */
#else
  #define GPRS_EDGE_MULTISLOT_CLASS       0xC  /* Currently supported EDGE multislot class is '12' */
#endif
#else
  #define GPRS_EDGE_MULTISLOT_CLASS       0xA  /* Currently supported EDGE multislot class is '10' */
#endif /* FEATURE_GPRS_EDA */

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern pmm_mode_type pmm_mode_sim[MAX_NAS_STACKS];
#define pmm_mode pmm_mode_sim[mm_as_id]
#else
extern pmm_mode_type pmm_mode;
#endif

/* ----------------------------------
** Service type (TS 24.008 10.5.5.20)
** ---------------------------------- */
#define GMM_SIGNALLING      0x0
#define GMM_DATA            0x1
#define GMM_PAGING_RESPONSE 0x2


typedef byte gmm_service_request_type_type;

/* -------------------------------------
** GPRS Attach type (TS 24.008 10.5.5.2)
** ------------------------------------- */
#define GMM_GPRS_ATTACH                     0x01
#define GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED 0x02
#define GMM_COMBINED_GPRS_IMSI_ATTACH       0x03

typedef byte gmm_attach_type_type;

/* ----------------------------------------------
** Routing Area Update Type (TS 24.008 10.5.5.18)
** ---------------------------------------------- */
#define GMM_RA_UPDATING                              0x0
#define GMM_COMBINED_RA_LA_UPDATING                  0x1
#define GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH 0x2
#define GMM_PERIODIC_UPDATING                        0x3

typedef byte gmm_ra_update_type_type;

/* -------------------------------------
** GPRS Detach type (TS 24.008 10.5.5.5)
** ------------------------------------- */
#define GMM_GPRS_DETACH               0x1
#define GMM_MO_IMSI_DETACH            0x2
#define GMM_COMBINED_GPRS_IMSI_DETACH 0x3

/* -------------------------------------
** GPRS PARAMETER DEFAULT VALUES
** ------------------------------------- */

#define GMM_REQ_READY_TIMER_VALUE_SECS  10

#define MAX_ATTEMPT_COUNTER_LAU   4
#define MAX_ATTEMPT_COUNTER_RAU   5
#define MAX_ATTEMPT_COUNTER_ATTACH   5


typedef byte gmm_detach_type_type;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern gmm_substate_type gmm_substate_sim[MAX_NAS_STACKS];

/* ------------------
** GMM SIM data items
** ------------------ */
extern gmm_ptmsi_type           gmm_stored_ptmsi_sim[MAX_NAS_STACKS];

extern gmm_ptmsi_signature_type gmm_stored_ptmsi_signature_sim[MAX_NAS_STACKS];
extern routing_area_id_T        gmm_stored_rai_sim[MAX_NAS_STACKS];
#ifdef FEATURE_NAS_REL11
extern PLMN_id_T                gmm_previous_plmn_with_t3302_value_sim[MAX_NAS_STACKS];
#endif
extern gmm_update_status_type   gmm_update_status_sim[MAX_NAS_STACKS];

/* ------------------------------------------------------------------------------------------
** The following two variables hold the service state of the previous serving cell.
** If the ms_op_mode is set to PS_ONLY they both are set to the value of gmm_service_state.
** If the ms_op_mode is set to CS+PS then gmm_previous_service_state is set to mm_service_state
** and gmm_previous_gprs_service_state is set to gmm_service_state.  This distinction is made
** so that the UE will perform a RAU when it moves from a cell supporting CS_ONLY to a cell
** supporting CS+PS.
** ------------------------------------------------------------------------------------------ */
extern service_state_type            gmm_previous_service_state_sim[MAX_NAS_STACKS];
extern service_state_type            gmm_previous_gprs_service_state_sim[MAX_NAS_STACKS];

extern network_op_mode_type          gmm_previous_network_op_mode_sim[MAX_NAS_STACKS];
extern routing_area_id_T             gmm_previous_serving_rai_sim[MAX_NAS_STACKS];
extern routing_area_id_T             gmm_previous_registration_serving_rai_sim[MAX_NAS_STACKS];
extern gmm_substate_type             gmm_previous_T3311_gmm_substate_sim[MAX_NAS_STACKS];
#ifdef FEATURE_NAS_REL10
extern sys_plmn_id_s_type            gmm_3gpp_t3346_plmn_id_sim[MAX_NAS_STACKS];
#endif
extern inter_task_lai_T              gmm_current_lai_sim[MAX_NAS_STACKS];
extern routing_area_code_T           gmm_current_rac_sim[MAX_NAS_STACKS];
extern gmm_attach_type_type          gmm_attach_type_sim[MAX_NAS_STACKS];
extern gmm_ra_update_type_type       gmm_ra_update_type_sim[MAX_NAS_STACKS];
extern gmm_ra_update_type_type       gmm_pending_ra_update_type_sim[MAX_NAS_STACKS];
extern gmm_service_request_type_type gmm_service_request_type_sim[MAX_NAS_STACKS];
extern gmm_detach_type_type          gmm_detach_type_sim[MAX_NAS_STACKS];
extern boolean                       gmm_controls_resel_dec_mt_ps_call_sim[MAX_NAS_STACKS];
extern boolean gmm_periodic_rau_pending_sim[MAX_NAS_STACKS];
extern boolean gprs_attach_upon_entry_to_a_new_cell_sim[MAX_NAS_STACKS];
extern boolean gmm_power_down_active_sim[MAX_NAS_STACKS];

extern byte gmm_service_request_initiator_sim[MAX_NAS_STACKS];

extern boolean gmm_integrity_protection_activated_sim[MAX_NAS_STACKS];
extern boolean gmm_ps_signalling_connected_sim[MAX_NAS_STACKS];

extern byte gprs_attach_attempt_counter_sim[MAX_NAS_STACKS];
extern byte gmm_rau_attempt_counter_sim[MAX_NAS_STACKS];
extern byte t3310_timeout_counter_sim[MAX_NAS_STACKS];
extern byte t3321_timeout_counter_sim[MAX_NAS_STACKS];
extern byte t3330_timeout_counter_sim[MAX_NAS_STACKS];

extern dword gmm_t3312_timer_value_sim[MAX_NAS_STACKS];
extern dword gmm_t3302_timer_value_sim[MAX_NAS_STACKS];

extern boolean gmm_timer_deactivated_sim[MAX_NAS_STACKS];

extern boolean gmm_gprs_attach_pending_sim[MAX_NAS_STACKS];
extern boolean gmm_gprs_detach_pending_sim[MAX_NAS_STACKS];
extern boolean gmm_ra_update_pending_sim[MAX_NAS_STACKS];
extern boolean gmm_multi_slot_ra_update_pending_sim[MAX_NAS_STACKS];
extern boolean gmm_detach_after_rau_pending_sim[MAX_NAS_STACKS];

extern boolean gmm_follow_on_request_sim[MAX_NAS_STACKS];
extern boolean gmm_directed_signalling_reestablishment_sim[MAX_NAS_STACKS];
extern boolean gmm_xid_negotiation_completed_sim[MAX_NAS_STACKS];
extern uint16  gmm_max_L3_length_sim[MAX_NAS_STACKS];

#ifdef FEATURE_NAS_REL10
extern byte    gmm_stored_nw_feature_support_sim[MAX_NAS_STACKS];

extern boolean gmm_is_congestion_due_to_rau_sim[MAX_NAS_STACKS];
#define  gmm_is_congestion_due_to_rau gmm_is_congestion_due_to_rau_sim[mm_as_id]

extern dword   gmm_t3346_backoff_timer_value_sim[MAX_NAS_STACKS];
#define gmm_t3346_backoff_timer_value  gmm_t3346_backoff_timer_value_sim[mm_as_id]
#endif

#ifdef FEATURE_NAS_REL11
extern boolean gmm_is_lw_to_g_interrat_in_progress_sim[MAX_NAS_STACKS];
#endif

extern boolean gmm_foreground_search_pending_sim[MAX_NAS_STACKS];

extern boolean gmm_manual_mode_user_selection_procedure_sim[MAX_NAS_STACKS];

extern boolean gmm_processing_directed_signalling_reestablishment_sim[MAX_NAS_STACKS];

extern boolean gmm_ptmsi_sig_tobe_deleted_sim[MAX_NAS_STACKS] ;

#ifdef FEATURE_REL6_PS_FOP
/* PS Follow on Proceed flag */
extern boolean gmm_ps_fop_req_sim[MAX_NAS_STACKS];
extern ps_fop_resp_type gmm_ps_fop_resp_sim[MAX_NAS_STACKS];
#endif/* FEATURE_REL6_PS_FOP */

#ifdef FEATURE_GSM_GPRS

/*
** RAU Required Indicator
*/
extern boolean gmm_rau_is_required_sim[MAX_NAS_STACKS] ;


/*
** Status indication from GERAN that RAU Request message has been txed successfully has been received
*/
extern boolean gmm_rau_tx_is_acked_sim[MAX_NAS_STACKS] ;

/*
** The purpose of the REG Report Indicator to report to REG upon
** completion of the UMTS-To-GSM Handover or Cell Reselection
*/
extern boolean gmm_reg_report_required_sim[MAX_NAS_STACKS] ;

/*
** PS Data available indicator
*/
extern boolean gmm_ps_data_available_sim[MAX_NAS_STACKS] ;

/*
** This will be set during WtoG inter-rat if  mode is PMM_CONNECTED or
** during GtoW if glif state is GMM_READY_STATE
*/
extern boolean gmm_is_prev_mode_connected_sim[MAX_NAS_STACKS];

#ifdef FEATURE_MODEM_HEAP
extern byte* gmm_rat_chng_pended_msg_sim[MAX_NAS_STACKS];
#else
extern byte gmm_rat_chng_pended_msg_sim[MAX_NAS_STACKS][MAX_MESSAGE_SIZE];
#endif


extern boolean gmm_rat_chng_pended_msg_valid_sim[MAX_NAS_STACKS] ;
extern rrc_establish_cause_e_type gmm_rat_chng_pended_msg_cause_sim[MAX_NAS_STACKS] ;


#endif /* #ifdef FEATURE_GSM_GPRS */

typedef enum
{
   MM_CS,
   MM_PS
}mm_lcs_domain_e_type;

extern mm_lcs_domain_e_type mm_lcs_domain_sim[MAX_NAS_STACKS];

extern boolean gmm_is_suspended_sim[MAX_NAS_STACKS];

#ifdef FEATURE_GPRS_PS_HANDOVER
typedef enum
{
   GMM_PSHO_NONE,
   GMM_PSHO_STARTED,
   GMM_PSHO_IN_PROGRESS
} gmm_psho_status_e_type;

#define PSHO_NAS_CNT_SIZE         5
#define PSHO_IOV_UI_SIZE          GLLC_IOV_UI_SIZE

extern gmm_psho_status_e_type gmm_psho_status_sim[MAX_NAS_STACKS];

extern byte gmm_psho_nas_container_sim[MAX_NAS_STACKS][PSHO_NAS_CNT_SIZE];

extern boolean gmm_psho_gea_change_sim[MAX_NAS_STACKS];
#endif

extern boolean gmm_is_ps_active_sim[MAX_NAS_STACKS];
extern sys_radio_access_tech_e_type gmm_last_signalling_sent_rat_sim[MAX_NAS_STACKS];

#define  gmm_substate gmm_substate_sim[mm_as_id]
#define  gmm_stored_ptmsi gmm_stored_ptmsi_sim[mm_as_id]

#define  gmm_stored_rai gmm_stored_rai_sim[mm_as_id]
#ifdef FEATURE_NAS_REL11
#define  gmm_previous_plmn_with_t3302_value gmm_previous_plmn_with_t3302_value_sim[mm_as_id] 
#endif
#define  gmm_update_status gmm_update_status_sim[mm_as_id]
#define  gmm_previous_service_state gmm_previous_service_state_sim[mm_as_id]
#define  gmm_previous_gprs_service_state gmm_previous_gprs_service_state_sim[mm_as_id]

#define  gmm_previous_network_op_mode gmm_previous_network_op_mode_sim[mm_as_id]
#define  gmm_previous_serving_rai gmm_previous_serving_rai_sim[mm_as_id]
#define  gmm_previous_registration_serving_rai gmm_previous_registration_serving_rai_sim[mm_as_id]

#define  gmm_3gpp_t3346_plmn_id  gmm_3gpp_t3346_plmn_id_sim[mm_as_id]
#define  gmm_previous_T3311_gmm_substate gmm_previous_T3311_gmm_substate_sim[mm_as_id]

#ifdef FEATURE_LTE
extern byte gmm_nv_eutra_measurement_reporting_support_sim[MAX_AS_IDS];
#define  gmm_nv_eutra_measurement_reporting_support gmm_nv_eutra_measurement_reporting_support_sim[mm_sub_id]
#endif

#define  gmm_current_lai gmm_current_lai_sim[mm_as_id]
#define  gmm_current_rac gmm_current_rac_sim[mm_as_id]
#define  gmm_attach_type gmm_attach_type_sim[mm_as_id]
#define  gmm_ra_update_type gmm_ra_update_type_sim[mm_as_id]
#define  gmm_pending_ra_update_type gmm_pending_ra_update_type_sim[mm_as_id]
#define  gmm_service_request_type gmm_service_request_type_sim[mm_as_id]
#define  gmm_detach_type gmm_detach_type_sim[mm_as_id]
#define  gmm_controls_resel_dec_mt_ps_call gmm_controls_resel_dec_mt_ps_call_sim[mm_as_id]
#define  gmm_periodic_rau_pending gmm_periodic_rau_pending_sim[mm_as_id]
#define  gprs_attach_upon_entry_to_a_new_cell gprs_attach_upon_entry_to_a_new_cell_sim[mm_as_id]
#define  gmm_power_down_active gmm_power_down_active_sim[mm_as_id]

#define  gmm_service_request_initiator gmm_service_request_initiator_sim[mm_as_id]

#define  gmm_integrity_protection_activated gmm_integrity_protection_activated_sim[mm_as_id]
#define  gmm_ps_signalling_connected gmm_ps_signalling_connected_sim[mm_as_id]

#define  gprs_attach_attempt_counter gprs_attach_attempt_counter_sim[mm_as_id]
#define  gmm_rau_attempt_counter gmm_rau_attempt_counter_sim[mm_as_id]
#define  t3310_timeout_counter t3310_timeout_counter_sim[mm_as_id]
#define  t3321_timeout_counter t3321_timeout_counter_sim[mm_as_id]
#define  t3330_timeout_counter t3330_timeout_counter_sim[mm_as_id]

#define  gmm_t3312_timer_value gmm_t3312_timer_value_sim[mm_as_id]
#define  gmm_t3302_timer_value gmm_t3302_timer_value_sim[mm_as_id]

#define  gmm_timer_deactivated gmm_timer_deactivated_sim[mm_as_id]

#define  gmm_gprs_attach_pending gmm_gprs_attach_pending_sim[mm_as_id]
#define  gmm_gprs_detach_pending gmm_gprs_detach_pending_sim[mm_as_id]
#define  gmm_ra_update_pending gmm_ra_update_pending_sim[mm_as_id]
#define  gmm_multi_slot_ra_update_pending gmm_multi_slot_ra_update_pending_sim[mm_as_id]
#define  gmm_detach_after_rau_pending gmm_detach_after_rau_pending_sim[mm_as_id]

#define  gmm_follow_on_request gmm_follow_on_request_sim[mm_as_id]
#define  gmm_directed_signalling_reestablishment gmm_directed_signalling_reestablishment_sim[mm_as_id]

#define gmm_xid_negotiation_completed gmm_xid_negotiation_completed_sim[mm_as_id]
#define gmm_max_L3_length gmm_max_L3_length_sim[mm_as_id]


#ifdef FEATURE_NAS_REL10
#define  gmm_stored_nw_feature_support gmm_stored_nw_feature_support_sim[mm_as_id]
#endif

#ifdef FEATURE_NAS_REL11
#define gmm_is_lw_to_g_interrat_in_progress gmm_is_lw_to_g_interrat_in_progress_sim[mm_as_id]
#endif

#define  gmm_foreground_search_pending gmm_foreground_search_pending_sim[mm_as_id]
#define  gprs_attach_attempt_counter  gprs_attach_attempt_counter_sim[mm_as_id]
#define  gmm_rau_attempt_counter     gmm_rau_attempt_counter_sim[mm_as_id]

#define  gmm_manual_mode_user_selection_procedure gmm_manual_mode_user_selection_procedure_sim[mm_as_id]

#define  gmm_processing_directed_signalling_reestablishment gmm_processing_directed_signalling_reestablishment_sim[mm_as_id]



#define  gmm_ptmsi_sig_tobe_deleted gmm_ptmsi_sig_tobe_deleted_sim[mm_as_id]

#ifdef FEATURE_REL6_PS_FOP
#define  gmm_ps_fop_req gmm_ps_fop_req_sim[mm_as_id]
#define  gmm_ps_fop_resp gmm_ps_fop_resp_sim[mm_as_id]
#endif/* FEATURE_REL6_PS_FOP */

#ifdef FEATURE_GSM_GPRS
#define  gmm_rau_is_required gmm_rau_is_required_sim[mm_as_id]
#define  gmm_rau_tx_is_acked gmm_rau_tx_is_acked_sim[mm_as_id]
#define  gmm_reg_report_required gmm_reg_report_required_sim[mm_as_id]
#define  gmm_ps_data_available gmm_ps_data_available_sim[mm_as_id]
#define  gmm_is_prev_mode_connected gmm_is_prev_mode_connected_sim[mm_as_id]

#endif /* #ifdef FEATURE_GSM_GPRS */

#define  mm_lcs_domain mm_lcs_domain_sim[mm_as_id]

#define gmm_is_suspended gmm_is_suspended_sim[mm_as_id]

#ifdef FEATURE_GPRS_PS_HANDOVER
#define  gmm_psho_status gmm_psho_status_sim[mm_as_id]

#define  gmm_psho_nas_container gmm_psho_nas_container_sim[mm_as_id]

#define  gmm_psho_gea_change gmm_psho_gea_change_sim[mm_as_id]
#endif

#define gmm_is_ps_active (gmm_is_ps_active_sim[mm_as_id])
#define  gmm_last_signalling_sent_rat gmm_last_signalling_sent_rat_sim[mm_as_id]

/* NV related data has to be always accessed using AS ID - NV Context ID mapping */
#define  gmm_stored_ptmsi_signature gmm_stored_ptmsi_signature_sim[mm_as_id]

extern boolean gmm_is_ptmsi_present_in_prev_gmm_msg_sim[MAX_NAS_STACKS] ;
extern boolean gmm_is_tmsi_present_in_prev_gmm_msg_sim[MAX_NAS_STACKS] ;

#define gmm_is_ptmsi_present_in_prev_gmm_msg gmm_is_ptmsi_present_in_prev_gmm_msg_sim[mm_as_id]
#define gmm_is_tmsi_present_in_prev_gmm_msg gmm_is_tmsi_present_in_prev_gmm_msg_sim[mm_as_id]

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM 
extern uint8 gmm_transaction_id_sim[MAX_NAS_STACKS];
#define gmm_transaction_id gmm_transaction_id_sim[mm_as_id]

extern byte  gmm_last_signalling_message_type_sim[MAX_NAS_STACKS];
#define gmm_last_signalling_message_type gmm_last_signalling_message_type_sim[mm_as_id]
#endif

extern boolean gmm_pended_rat_resume_req_sim[MAX_NAS_STACKS];
#define gmm_pended_rat_resume_req gmm_pended_rat_resume_req_sim[mm_as_id] 

extern boolean gmm_detach_in_progress_sim[MAX_NAS_STACKS];
#define gmm_detach_in_progress gmm_detach_in_progress_sim[mm_as_id]

extern boolean gmm_registration_initiated_sim[MAX_NAS_STACKS];
#define gmm_registration_initiated gmm_registration_initiated_sim[mm_as_id]

extern boolean gmm_processed_mt_imsi_detach_request_sim[MAX_NAS_STACKS];
#define gmm_processed_mt_imsi_detach_request gmm_processed_mt_imsi_detach_request_sim[mm_as_id]

extern boolean gmm_cause_directed_signalling_reest_sim[MAX_NAS_STACKS];
#define gmm_cause_directed_signalling_reest gmm_cause_directed_signalling_reest_sim[mm_as_id]

#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
extern    boolean gmm_force_registration_sim[MAX_NAS_STACKS];
#define   gmm_force_registration gmm_force_registration_sim[mm_as_id]
#endif

extern routing_area_id_T   gmm_reg_rejected_rai_sim[MAX_NAS_STACKS];
#define gmm_reg_rejected_rai gmm_reg_rejected_rai_sim[mm_as_id]

extern dword gmm_t3319_timer_value_sim[MAX_NAS_STACKS];
#define gmm_t3319_timer_value gmm_t3319_timer_value_sim[mm_as_id]

extern boolean gmm_attach_request_on_network_reject_sim[MAX_NAS_STACKS];
#define gmm_attach_request_on_network_reject gmm_attach_request_on_network_reject_sim[mm_as_id]

extern boolean gmm_extend_connection_due_to_mt_csfb_sim[MAX_NAS_STACKS];
#define gmm_extend_connection_due_to_mt_csfb gmm_extend_connection_due_to_mt_csfb_sim[mm_as_id]

extern boolean gmm_power_on_attach_completed_sim[MAX_NAS_STACKS];
#define gmm_power_on_attach_completed gmm_power_on_attach_completed_sim[mm_as_id]

extern boolean gmm_force_local_detach_sim[MAX_NAS_STACKS];

#else /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

extern gmm_substate_type gmm_substate;
extern sys_plmn_id_s_type gmm_3gpp_t3346_plmn_id;
#ifdef FEATURE_LTE
extern byte gmm_nv_eutra_measurement_reporting_support;
#endif
/* ------------------
** GMM SIM data items
** ------------------ */
extern gmm_ptmsi_type           gmm_stored_ptmsi;

extern gmm_ptmsi_signature_type gmm_stored_ptmsi_signature;
extern routing_area_id_T        gmm_stored_rai;
#ifdef FEATURE_NAS_REL11
extern PLMN_id_T                gmm_previous_plmn_with_t3302_value;
#endif
extern gmm_update_status_type   gmm_update_status;

/* ------------------------------------------------------------------------------------------
** The following two variables hold the service state of the previous serving cell.
** If the ms_op_mode is set to PS_ONLY they both are set to the value of gmm_service_state.
** If the ms_op_mode is set to CS+PS then gmm_previous_service_state is set to mm_service_state
** and gmm_previous_gprs_service_state is set to gmm_service_state.  This distinction is made
** so that the UE will perform a RAU when it moves from a cell supporting CS_ONLY to a cell
** supporting CS+PS.
** ------------------------------------------------------------------------------------------ */
extern service_state_type            gmm_previous_service_state;
extern service_state_type            gmm_previous_gprs_service_state;

extern network_op_mode_type          gmm_previous_network_op_mode;
extern routing_area_id_T             gmm_previous_serving_rai;
extern routing_area_id_T             gmm_previous_registration_serving_rai;
extern gmm_substate_type             gmm_previous_T3311_gmm_substate;

extern inter_task_lai_T              gmm_current_lai;
extern routing_area_code_T           gmm_current_rac;
extern gmm_attach_type_type          gmm_attach_type;
extern gmm_ra_update_type_type       gmm_ra_update_type;
extern gmm_ra_update_type_type       gmm_pending_ra_update_type;
extern gmm_service_request_type_type gmm_service_request_type;
extern gmm_detach_type_type          gmm_detach_type;
extern boolean                       gmm_controls_resel_dec_mt_ps_call;
extern boolean gmm_periodic_rau_pending;
extern boolean gprs_attach_upon_entry_to_a_new_cell;
extern boolean gmm_power_down_active;

extern byte gmm_service_request_initiator;

extern boolean gmm_integrity_protection_activated;
extern boolean gmm_ps_signalling_connected;

extern byte gprs_attach_attempt_counter;
extern byte gmm_rau_attempt_counter;
extern byte t3310_timeout_counter;
extern byte t3321_timeout_counter;
extern byte t3330_timeout_counter;

extern dword gmm_t3312_timer_value;
extern dword gmm_t3302_timer_value;

#if defined(FEATURE_LTE)
#ifdef FEATURE_DUAL_SIM
extern dword gmm_t3323_timer_value_sim[MAX_AS_IDS];
#define gmm_t3323_timer_value gmm_t3323_timer_value_sim[mm_sub_id]
#else
extern dword gmm_t3323_timer_value;
#endif
#endif

extern boolean gmm_timer_deactivated;

extern boolean gmm_gprs_attach_pending;
extern boolean gmm_gprs_detach_pending;
extern boolean gmm_ra_update_pending;
extern boolean gmm_multi_slot_ra_update_pending;
extern boolean gmm_detach_after_rau_pending;

extern boolean gmm_is_ptmsi_present_in_prev_gmm_msg;
extern boolean gmm_is_tmsi_present_in_prev_gmm_msg;

extern boolean gmm_follow_on_request;
extern boolean gmm_directed_signalling_reestablishment;
extern boolean gmm_xid_negotiation_completed;
extern uint16 gmm_max_L3_length;

#ifdef FEATURE_NAS_REL10
extern byte    gmm_stored_nw_feature_support;
#endif

#ifdef FEATURE_NAS_REL11
extern boolean gmm_is_lw_to_g_interrat_in_progress;
#endif

extern boolean gmm_foreground_search_pending;
#ifdef FEATURE_NAS_REL10
extern boolean gmm_is_congestion_due_to_rau;
extern dword   gmm_t3346_backoff_timer_value;
#endif
extern byte    gprs_attach_attempt_counter;
extern byte    gmm_rau_attempt_counter;

extern boolean gmm_manual_mode_user_selection_procedure;

extern boolean gmm_processing_directed_signalling_reestablishment;

extern boolean gmm_ptmsi_sig_tobe_deleted ;

#ifdef FEATURE_REL6_PS_FOP
/* PS Follow on Proceed flag */
extern boolean gmm_ps_fop_req;
extern ps_fop_resp_type gmm_ps_fop_resp;
#endif/* FEATURE_REL6_PS_FOP */

#ifdef FEATURE_GSM_GPRS

/*
** RAU Required Indicator
*/
extern boolean gmm_rau_is_required ;

/*
** Status indication from GERAN that RAU Request message has been txed successfully has been received
*/
extern boolean gmm_rau_tx_is_acked;

extern boolean gmm_registration_initiated;

/*
** The purpose of the REG Report Indicator to report to REG upon
** completion of the UMTS-To-GSM Handover or Cell Reselection
*/
extern boolean gmm_reg_report_required ;

/*
** PS Data available indicator
*/
extern boolean gmm_ps_data_available ;

/*
** This will be set during WtoG inter-rat if  mode is PMM_CONNECTED or 
** during GtoW if glif state is GMM_READY_STATE 
*/
extern boolean gmm_is_prev_mode_connected;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_MODEM_HEAP
extern byte *gmm_rat_chng_pended_msg;
#else
extern byte gmm_rat_chng_pended_msg[MAX_MESSAGE_SIZE] ;
#endif
extern boolean gmm_rat_chng_pended_msg_valid ;
extern rrc_establish_cause_e_type gmm_rat_chng_pended_msg_cause ;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM 
extern uint8 gmm_transaction_id;
extern byte  gmm_last_signalling_message_type;
#endif


extern uint8 gmmrr_transaction_id;

extern boolean gmm_pended_rat_resume_req;
extern boolean gmm_detach_in_progress;
#endif /* #ifdef FEATURE_GSM_GPRS */

typedef enum 
{
   MM_CS,
   MM_PS
}mm_lcs_domain_e_type;

extern mm_lcs_domain_e_type mm_lcs_domain;

extern boolean gmm_is_suspended;

#ifdef FEATURE_GPRS_PS_HANDOVER
typedef enum
{
   GMM_PSHO_NONE,
   GMM_PSHO_STARTED,
   GMM_PSHO_IN_PROGRESS
} gmm_psho_status_e_type;

#define PSHO_NAS_CNT_SIZE         5
#define PSHO_IOV_UI_SIZE          GLLC_IOV_UI_SIZE

extern gmm_psho_status_e_type gmm_psho_status;

extern byte gmm_psho_nas_container[PSHO_NAS_CNT_SIZE];

extern boolean gmm_psho_gea_change;
#endif

extern boolean gmm_is_ps_active;
extern sys_radio_access_tech_e_type gmm_last_signalling_sent_rat;

#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
extern boolean gmm_force_registration;
#endif

extern routing_area_id_T   gmm_reg_rejected_rai;
extern dword gmm_t3319_timer_value;
#ifdef FEATURE_LTE 
#ifdef FEATURE_DUAL_SIM
extern boolean gmm_set_drx_req_pending_sim[MAX_AS_IDS];
#define gmm_set_drx_req_pending gmm_set_drx_req_pending_sim[mm_sub_id]
#else
extern boolean gmm_set_drx_req_pending;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_drx_cn_coeff_s1 gmm_drx_cn_coeff_s1_sim[mm_sub_id]
extern sys_drx_cn_coefficient_s1_e_type gmm_nv_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_nv_drx_cn_coeff_s1 gmm_nv_drx_cn_coeff_s1_sim[mm_sub_id]
#else
extern sys_drx_cn_coefficient_s1_e_type gmm_nv_drx_cn_coeff_s1;
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1;
#endif
#endif /*FEATURE_LTE*/

extern boolean gmm_attach_request_on_network_reject;
extern boolean gmm_processed_mt_imsi_detach_request;
extern boolean gmm_cause_directed_signalling_reest;

extern boolean gmm_extend_connection_due_to_mt_csfb;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

#ifdef FEATURE_LTE
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern byte gmm_nv_geran_to_eutra_measurement_support_sim[MAX_AS_IDS];
#define gmm_nv_geran_to_eutra_measurement_support gmm_nv_geran_to_eutra_measurement_support_sim[mm_sub_id]
#else
extern byte gmm_nv_geran_to_eutra_measurement_support;
#endif

#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM)
extern boolean gmm_is_lte_advertised_in_last_reg_sim[MAX_NAS_STACKS];
#define gmm_is_lte_advertised_in_last_reg gmm_is_lte_advertised_in_last_reg_sim[mm_as_id]
#else
extern boolean gmm_is_lte_advertised_in_last_reg;
#endif 
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern uint64  gmm_nv_disable_call_type_sim[MAX_NAS_STACKS];

#define gmm_nv_disable_call_type (gmm_nv_disable_call_type_sim[mm_as_id])

#else

extern uint64  gmm_nv_disable_call_type;
#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE */
#define GMM_CALL_TYPE_VOICE_MASK (uint64)1

#ifdef FEATURE_DUAL_SIM
extern boolean gmm_cm_ds_tuneaway_status; 
#endif

/*
** GPRS NV Items GMM Store
*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern byte gmm_multislot_class_sim[MAX_AS_IDS];
#define gmm_multislot_class gmm_multislot_class_sim[mm_sub_id]

extern byte gmm_multislot_class_nv_sim[MAX_AS_IDS];
#define gmm_multislot_class_nv gmm_multislot_class_nv_sim[mm_sub_id]

extern byte gmm_non_drx_timer_sim[MAX_AS_IDS] ;
#define gmm_non_drx_timer gmm_non_drx_timer_sim[mm_sub_id]

extern byte gmm_split_paging_cycle_sim[MAX_AS_IDS];
#define gmm_split_paging_cycle gmm_split_paging_cycle_sim[mm_sub_id]


extern uint8 gmm_gea_support_sim[MAX_AS_IDS];
#define gmm_gea_support gmm_gea_support_sim[mm_sub_id]

#ifdef FEATURE_GSM_EDTM
extern boolean gmm_nv_edtm_feature_support_sim[MAX_AS_IDS];
#define gmm_nv_edtm_feature_support gmm_nv_edtm_feature_support_sim[mm_sub_id]
#endif

#ifdef FEATURE_GSM_EGPRS
/* Default values for EDGE NV items */
extern boolean gmm_edge_feature_support_sim[MAX_AS_IDS];
#define gmm_edge_feature_support gmm_edge_feature_support_sim[mm_sub_id]

extern byte gmm_edge_multislot_class_sim[MAX_AS_IDS];
#define gmm_edge_multislot_class gmm_edge_multislot_class_sim[mm_sub_id]

extern byte gmm_edge_multislot_class_nv_sim[MAX_AS_IDS];
#define gmm_edge_multislot_class_nv gmm_edge_multislot_class_nv_sim[mm_sub_id]

extern byte gmm_edge_8psk_power_class_sim[MAX_AS_IDS];
#define gmm_edge_8psk_power_class gmm_edge_8psk_power_class_sim[mm_sub_id]

extern byte gmm_edge_8psk_power_capability_sim[MAX_AS_IDS];
#define gmm_edge_8psk_power_capability gmm_edge_8psk_power_capability_sim[mm_sub_id]
#endif /* FEATURE_GSM_EGPRS */

#ifdef FEATURE_REL_4_SUPPORT
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
extern boolean gmm_edge_nw_ass_cell_change_sim[MAX_AS_IDS];
#define gmm_edge_nw_ass_cell_change gmm_edge_nw_ass_cell_change_sim[mm_sub_id]
extern byte gmm_interrat_nacc_support_sim[MAX_AS_IDS];
#define gmm_interrat_nacc_support gmm_interrat_nacc_support_sim[mm_sub_id]
#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */
#endif /* FEATURE_REL_4_SUPPORT */

#ifdef FEATURE_GSM_DTM
extern ghdi_nvmem_data_T gmm_nv_data_buffer;

extern boolean  gmm_nv_dtm_feature_support_sim[MAX_AS_IDS];
#define  gmm_nv_dtm_feature_support gmm_nv_dtm_feature_support_sim[mm_sub_id]

extern uint8 gmm_nv_dtm_multislot_subclass_sim[MAX_AS_IDS];
#define  gmm_nv_dtm_multislot_subclass gmm_nv_dtm_multislot_subclass_sim[mm_sub_id]

extern uint8 gmm_nv_dtm_multislot_subclass_nv_sim[MAX_AS_IDS];
#define  gmm_nv_dtm_multislot_subclass_nv gmm_nv_dtm_multislot_subclass_nv_sim[mm_sub_id]

#endif /* FEATURE_GSM_DTM */

#else
extern byte gmm_multislot_class;
extern byte gmm_multislot_class_nv;
extern byte gmm_non_drx_timer;
extern byte gmm_split_paging_cycle;
extern uint8 gmm_gea_support;

#ifdef FEATURE_GSM_EGPRS
/*
** EDGE Variables
*/
extern boolean gmm_edge_feature_support;
extern byte gmm_edge_multislot_class;
extern byte gmm_edge_multislot_class_nv;
extern byte gmm_edge_8psk_power_class;
extern byte gmm_edge_8psk_power_capability;
#endif /* FEATURE_GSM_EGPRS */

#ifdef FEATURE_GSM_EDTM
extern boolean gmm_nv_edtm_feature_support;
#endif /* FEATURE_GSM_EDTM */

#ifdef FEATURE_GSM_DTM
extern boolean gmm_nv_dtm_feature_support;
extern uint8   gmm_nv_dtm_multislot_subclass;
extern uint8   gmm_nv_dtm_multislot_subclass_nv;
#endif /* FEATURE_GSM_DTM */


#ifdef FEATURE_REL_4_SUPPORT

#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
extern boolean gmm_edge_nw_ass_cell_change;
extern byte gmm_interrat_nacc_support; 
#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */

#endif /* FEATURE_REL_4_SUPPORT */
#endif/*FEATURE_DUAL_SIM &&FEATURE_DUAL_ACTIVE*/

#ifdef FEATURE_GPRS_PS_HANDOVER
extern boolean gmm_nv_psho_feature_support;
#endif

#ifdef FEATURE_GPRS_EDA
extern boolean gmm_nv_eda_feature_support;
#endif /* FEATURE_GPRS_EDA */

#ifdef FEATURE_REL_6_SUPPORT
#if (defined(FEATURE_GSM_EGPRS_SAIC) && defined(FEATURE_GSM_MDSP_AEQ))
extern boolean gmm_nv_saic_feature_support;
#endif /* FEATURE_GSM_EGPRS_SAIC */
#endif /* FEATURE_REL_6_SUPPORT */

#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH)
extern uint16  gmm_nv_r_acch_feature_support;
#endif /* FEATURE_GSM_R_SACCH or FEATURE_GSM_R_FACCH*/

#if defined (FEATURE_DUAL_SIM ) || defined (FEATURE_SGLTE)
extern uint8  gmm_nv_multislot_gmsk_power_profile_sim[MAX_AS_IDS];
#define gmm_nv_multislot_gmsk_power_profile gmm_nv_multislot_gmsk_power_profile_sim[mm_sub_id]

extern uint8  gmm_nv_multislot_8psk_power_profile_sim[MAX_AS_IDS];
#define gmm_nv_multislot_8psk_power_profile gmm_nv_multislot_8psk_power_profile_sim[mm_sub_id]

#else
extern uint8  gmm_nv_multislot_gmsk_power_profile;
extern uint8  gmm_nv_multislot_8psk_power_profile;

#endif
#if defined (FEATURE_DUAL_SIM ) || defined (FEATURE_SGLTE)
extern byte    gmm_reject_cause_sim[MAX_NAS_STACKS];
#define gmm_reject_cause gmm_reject_cause_sim[mm_as_id]
#else
extern byte    gmm_reject_cause;
#endif

#ifdef FEATURE_LTE
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_lte_cs_domain_param_s_type  gmm_reg_lte_cs_param_sim[MAX_NAS_STACKS];
#define gmm_reg_lte_cs_param gmm_reg_lte_cs_param_sim[mm_as_id]
extern sys_lte_cs_domain_param_s_type  gmm_reg_pended_lte_cs_param_sim[MAX_NAS_STACKS];
#define gmm_reg_pended_lte_cs_param gmm_reg_pended_lte_cs_param_sim[mm_as_id]
#else
extern sys_lte_cs_domain_param_s_type  gmm_reg_lte_cs_param;
extern sys_lte_cs_domain_param_s_type  gmm_reg_pended_lte_cs_param;

#endif
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean mm_ps_auth_sent_to_card_sim[MAX_NAS_STACKS];
#define  mm_ps_auth_sent_to_card mm_ps_auth_sent_to_card_sim[mm_as_id]
#else
extern boolean mm_ps_auth_sent_to_card;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
extern void gmm_send_reset_ind_to_ds(ds_3gpp_notify_ind_e_type ind_type, ds_3gpp_rau_event_e_type event,ds_3gpp_rau_type rau_type );

extern gmm_ie_pdp_context_status_T get_pdp_context_status(sys_modem_as_id_e_type as_id); 

extern gmm_ie_pdp_context_status_T get_uplink_data_status(sys_modem_as_id_e_type as_id); 

extern void gmm_set_state( gmm_state_type new_state );

extern void gmm_set_substate( gmm_substate_type new_substate );

extern void gmm_handle_registration( mm_cmd_type *message );

extern void gmm_handle_multi_slot_class_ind( mm_cmd_type *message);

extern void gmm_handle_connection_management( mm_cmd_type *message );

extern void gmm_initiate_gprs_attach( void );

extern void gmm_handle_gprs_attach( mm_cmd_type *message );

extern void gmm_handle_gprs_detach( mm_cmd_type *message );

extern void gmm_initiate_routing_area_update( void );

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
extern void gmm_handle_routing_area_update( mm_cmd_type *message );
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

extern void gmm_handle_service_request( mm_cmd_type *message );

extern void gmm_initiate_service_request( rrc_establish_cause_e_type establish_cause,
                                          auth_security_context_id security_context,
                                          gmm_ie_pdp_context_status_T* uplink_data_status
                                        );
#ifdef FEATURE_WCDMA
extern void gmm_process_rrc_page_ind( mm_cmd_type *message );
#endif /*FEATURE_WCDMA*/

extern dword gmm_convert_timer_value_ie_to_usecs( byte timer_value );


extern dword gmm_convert_timer3_value_ie_to_msecs(byte timer_value );

extern void gmm_delete_ps_location_information( void );

extern void gmm_delete_gprs_cksn( void );

extern void gmm_process_abnormal_page_ind( void ) ;

extern void gmm_sm_send_rat_change_req
( 
  sys_radio_access_tech_e_type
#if defined(FEATURE_LTE)
  , boolean abort_procedure
#endif
);

extern void gmm_sm_send_rat_resume_req (void);

extern void gmm_send_gmmsm_establish_ind(sm_cm_status_T cause);

extern void gmm_send_gmmsm_establish_cnf(boolean  is_pdp_retransmit_req);

extern void gmm_send_gmmsm_establish_rej(sm_cm_status_T      cause);


extern void gmm_sm_send_con_release_ind (cc_cause_T cause) ;

extern void gmm_sm_send_fast_dormancy_cnf  (gmmsm_fast_dormancy_cnf_cause_e_type cause) ;

extern void gmm_sm_send_con_release_rej(void);
extern void gmm_process_cm_req( mm_cmd_type *message ) ;
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
extern void gmm_initiate_gprs_detach( mm_cmd_type *message );
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

extern void gmm_change_of_network_mode_of_operation( void );

#ifdef FEATURE_NAS_REL10
extern void gmm_change_of_nmo_during_t3346_active( void );
#endif

extern mm_as_cell_access_status_T gmm_get_ps_cell_access
(
  nas_procedure_for_cell_access_T nas_procedure
);

extern void gmm_update_ra_type(void);

extern boolean gmm_check_if_ps_cell_access_barred
(
  mm_as_cell_access_status_T ps_cell_access
);

extern boolean gmm_check_if_ps_cell_access_allowed
(
  mm_as_cell_access_status_T ps_cell_access
);

extern boolean gmm_plmn_changed
(
  sys_plmn_id_s_type previous_plmn,
  sys_plmn_id_s_type new_plmn
);

#ifdef FEATURE_LTE
boolean gmm_is_lte_cap_changed( void );
#endif

#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
extern void gmm_process_ims_voice_tag_reg_status_ind
(
  boolean is_ims_registered
);
#endif

typedef struct gmm_trig
{
 dword trigger;
 boolean fts;
 dword rtv;
}gmm_trig_T;
 

typedef struct gmm_trig_queue
{
 q_link_type link;
 gmm_trig_T  trigg;
}gmm_trig_queue_T;

extern boolean gmm_rai_changed
(
  location_area_code_T previous_lac,
  routing_area_code_T  previous_rac,
  location_area_code_T new_lac,
  routing_area_code_T  new_rac
);

extern void gmm_update_mm_serving_plmn_info
(
  sys_sim_state_e_type  sim_state,
  sys_srv_domain_e_type reg_service_domain,
  boolean               mm_deregistered,
  sys_srv_status_e_type service_status,
  boolean               is_gstk_event_required
);

extern void gmm_increment_routing_area_update_attempt_counter
( 
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
boolean npdu_no_list_present
#else
void 
#endif
);

extern void gmm_increment_gprs_attach_attempt_counter( void );
extern void gmm_reset_global_variables(void);

#ifdef FEATURE_ENHANCED_NW_SELECTION

extern boolean gmm_is_rai_in_foreground_search_lai_rai_timer_list(byte* lai,byte rac);

extern void gmm_add_rac_to_foreground_search_lai_rai_timer_list(byte* lai,byte rac);

extern void gmm_get_foreground_timer_expiry_rai(byte* lai,byte* rac);
extern void gmm_invalidate_foreground_search_rai(byte * serving_lai,byte  rac);
extern void gmm_update_ps_attempt_counter_from_foreground_search_lai_rai_timer_list(byte* lai,byte rac);

#endif

extern boolean gmm_check_gprs_forbidden_rai(PLMN_id_T plmn_id,location_area_code_T lac,routing_area_code_T rac);

extern void gmm_update_gprs_forbidden_rai(routing_area_id_T rai);

extern void gmm_delete_gprs_forbidden_rai(void);
extern void gmm_update_cell_status(rrc_abort_cause_e_type abort_cause);
#ifdef FEATURE_NAS_REL10
extern boolean gmm_is_plmn_equivalent_t3346(void);
#endif
extern void gmm_sm_send_gprs_rel_ind_if_needed(void);
extern uint16 gmm_max_L3_msg_length(mm_cn_domain_type_T);

#ifdef FEATURE_SGLTE
extern void gmm_sglte_reset_ps_attempt_counters(void);
#endif

/*===========================================================================
FUNCTION gmm_send_nv_params_to_rr

DESCRIPTION
  This function sends NV params to RR

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/
extern void gmm_send_nv_params_to_rr(void);

 /*===========================================================================
FUNCTION  gmm_read_carrier_specific_efs

DESCRIPTION
  This function reads carrier specific EFS/NV values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gmm_read_carrier_specific_efs(void);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*************************************************************
FUNCTION gmm_process_mcfg_refresh

DESCRIPTION
  This function processes MCFG NV refresh indication for GMM specific
  EFS/NVs

DEPENDENCIES
  None

Input

RETURN VALUE
  None


SIDE EFFECTS
  None
 *************************************************************/
extern void gmm_process_mcfg_refresh(void);
#endif

#ifdef FEATURE_DUAL_SIM
extern boolean gmm_get_cm_ds_tuneaway_status();
#endif /*FEATURE_DUAL_SIM*/

extern void gmm_send_gstk_idle_event_if_needed( byte, boolean); 


#endif /* GMM_INT_H */
