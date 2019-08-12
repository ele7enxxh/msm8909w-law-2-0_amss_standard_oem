/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                GPRS Mobility Management (GMM)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmm_main.c_v   1.25   18 Jul 2002 11:39:32   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmm_main.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/21/11   gps     FREE FLOAT FEATURE changes
01/26/01   jca     Created module
05/23/01   jca     Added hooks for all RRC primtives.
05/25/01   jca     Added regional variables to support service request procedures.
06/08/01   jca     Added support for passing OTA messages to SM.
06/13/01   jca     Removed RRC_UNIT_DATA_IND and RRC_OPEN_SESSION_CNF.
06/18/01   jca     Now change state to PMM_DETACHED when DEREGISTERED.
08/03/01   jca     Ensure update status is set correctly at power up.
09/12/01   jca     Now conditionally change state to PMM_DETACHED when DEREGISTERED.
09/18/01   sbk     Intertask primitives and QXDM packets are logged
11/06/01   jca     Added new variable for PS signalling connection status.
02/15/02   ks      Added Support for Test Control (TC) entity.
02/25/02   ks      Added case MS_GMM_TC to process the TC messages in
                   gmm_handle_message() function.
04/30/02   ks      Added processing of TIMER_DETACH_REL in gmm_handle_message()
                   for case MS_TIMER.
06/05/02   ks      Added support for Dual mode operation.
07/06/02   vdr     Added new global variable boolean gmm_timer_deactivated
                   Multiple changes inorder to support GMM-LLC interface feature
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/22/02   ks      Added case RRC_STOP_WCDMA_MODE_CNF in gmm_handle_message()
10/07/02   jca     Changes to support CS/PS combined attach procedures.
10/09/02   jca     Fixed some bugs related to RR(C)_SERVICE_IND processing.
10/10/02   jca     Added preliminary support for PS/CS combined detach procedures.
11/06/02   jca     Fixed code so GMM will perform re-attach procedure in NMO II.
11/8/02    cd      Modified gmm_set_substate() to allow transition from
                   GMM_PLMN_SEARCH to GMM_ATTACH_NEEDED to support access class
                   blocking requirements
11/12/02   jca     Added support for RRC_CLOSE_SESSION_CNF.
11/14/02   vdr     Added support for PS SMS message transferring
11/15/02   mks     Added support for GMM Information Message
11/26/02   jca     Fixed bug in Routing Area Update processing when transitioning
                   to substate GMM_NORMAL_SERVICE.
11/27/02   mks     Now initializing PSLOCI information in gmm_initialize().
12/05/02   jca     Changes to support PS_ONLY<->CS_ONLY mode switches in GSM mode.
12/06/02   mks     Added support for GPRS suspension procedure.
01/10/03   jca     Changes to support CS<->PS<->CS/PS mode switching.
01/16/03   vdr     Added GSM PS connection management in order to support dual mode features
01/27/03   jca     Changes to support PS_ONLY<->CS/PS mode switching.
                   Changes to support IMSI Detach via Combined GPRS Detach procedure.
02/07/03   jca     Now disallowing GPRS Attach on transition to GMM_DEREGISTERED-
                   NORMAL_SERVICE state if GPRS Update Status is equal to GU3.
02/10/03   cd      Fixed Lint errors
02/13/03   jca     Now performing a Routing Area Update procedure on PLMN change
                   instead of GPRS Attach procedure.
02/21/03   vdr     Now send MMCNM_DATA_IND to new MS_CC_MM_PS message set
02/27/03   vdr     Added routing of the CM primitives in order to support PS data
                   data continuity during UMTS to GSM RAT change
03/07/03   vdr     Change gmm_set_state() in order to support PS data continuity
                   feature during UMTS to GSM and GSM to UMTS RAT change
03/07/03   vdr     Added initialization of the gmm_ptmsi_allocated_rai when new
                   SIM card is inserted
03/14/03   vdr     Added RAU initiation when RAC has been changed during
                   GMM_NO_CELL_AVAILABLE substate
03/20/03   jca     Implemented missing requirement from 24.008 section 4.7.2.2.
03/27/03   vdr     RR_PAGING_IND & RR_SYNC_IND processing when UE is attached to CS
                   domain only or when it attached to PS domain as well but current
                   cell does not provive GPRS services or GMM is suspended
03/31/03   jca     Added boolean variable gmm_gprs_detach_pending.
04/11/03   jca     Set GMM substate to LIMITED_SERVICE instead of NORMAL_SERVICE
                   if the GPRS Update Status is set to GU3.
04/14/03   jca     Added boolean variable gmm_ra_update_pending.
04/22/03   mks     Reset mm_serving_plmn.ps_data_suspend flag upon GPRS detach.
04/29/03   jca     Added support for change of network mode of operation.
04/30/03   vdr     Added printouts to support peek/poke mechanism for
                   glif_force_local_tlli
05/07/03   jca     Only send Combined RAU if ms_op_mode == SYS_SRV_DOMAIN_CS_PS.
05/23/03   jca     Changed MSG_ERRORs to MSG_HIGHs.
05/29/03   jca     Added boolean variable gmm_follow_on_request.
06/04/03   ks      Added support for RRC_MODE_CHANGE_CNF for MODE_CHANGE
                   support for UMTS.
06/11/03   jca     When operating with ms_op_mode set to CS_PS in network op mode 2
                   send the MMR_REG_CNF upon completion of MM registration and send
                   the PS registration status later in a MMR_SERVICE_IND.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
06/19/03   ks      Setting the RAU to GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH
                   If the UE goes from NMO2 cell to NO_SERVICE, then PRAU timer
                   expires and the UE camps on a NMO1 cell.
07/09/03   vdr     Added initialization of the glif_ra_changed_rau_completed_ra variable
                   upon reception of the SIM available indication.
                   Removed processing of the gmm_ptmsi_allocated_rai variable.
07/16/03   ks      Replaced ERR_FATAL with ERR when GMM receives an MM message
                   on a PS domain.
07/16/03   jca     Added support for new RR_GMM_GPRS_RESUMPTION_IND primitive.
07/17/03   jca     Route RR_GMM_GPRS_RESUMPTION_IND to GMM-LLC interface module.
07/29/03   jca     Fixed PS Attach-on-demand problem related to clearing of the
                   mm_serv_change_pending boolean flag.
08/01/03   jca     Do not change GMM substate when GMM state is set to NULL.
08/01/03   vdr     Featured out GSM GPRS related vars (FEATURE_GSM_GPRS)
08/05/03   mks     Stopping T3317 upon receiving a loopback signalling message on
                   downlink for loopback test calls.
08/13/03   jca     Added boolean gmm_detach_after_rau_pending.
09/22/03   ks      Added GMM substate check to see if it is NORMAL_SERVICE prior
                   to sending the GMM_TRIGGER_RAU_FINISHED event to LLC.
10/09/03   jca     Now change gmm_state prior to calling gmmllc_gmm_trigger_llc_event()
                   to ensure correct actions are taken in LLC event handler.
10/13/03   jca     Moved glif_exit_mm_non_drx_mode() function calls to
                   gmm_set_state() function to ensure calls to reenable DRX
                   mode are made on ALL valid designated GMM state transitions.
10/22/03   jca     Added support for change of network mode of operation to/from NMO III.
11/04/03   jca     Found a scenario during unit test where a MMR_SERVICE_IND
                   was not being generated after successful GPRS Attach.
11/20/03   jca     Ensure that the GMM FOR bit is reset to zero when GMM transitions
                   from REGISTERED to DEREGISTERED state.
11/25/03   ks      Now Sending notification to REG when GMM substate changes from
                   GMM_LIMITED_SERVICE to GMM_NORMAL_SERVICE while REGISTERED.
12/03/03   vdr     Added boolean gmm_ptmsi_sig_tobe_deleted variable indicating
                   when P-TMSI Signature shall be deleted
12/11/03   jca     Added regional variables gmm_previous_serving_rai
                   and gmm_previous_gmm_substate.
12/16/03   jca     Corrected bugs when user changes service setting from PS_ONLY
                   to CS+PS and the network op mode equals NMO III.
12/18/03   vdr     Added recording of the RAI in which we last successfully updated
                   upon appropriate state exchange
04/08/04   vdr     Added gmm_inter_rat_follow_on_ind variable to trigger follow-on
                   request when GMM does RA update during G2W inter-RAT change and
                   user data is available
04/15/04   jca     Removed bug where GMM was performing Attach instead of RAU
                   when GMM state was set to GMM_REGISTERED.
05/07/04   jca     Now clearing the gmm_follow_on_request boolean flag when GMM moves
                   to state GMM_REGISTERED rather than GMM_DEREGISTERED to account
                   for failed GPRS Attach attempts.
05/12/04   jca     Added boolean variable gmm_directed_signalling_reestablishment.
05/26/04   jca     Ensure pmm_mode set to PMM_IDLE when setting GMM state to
                   GMM_REGISTERED.
05/27/04   jca     Perform RAU on the same RRC connection when commanded to switch
                   from GSM to WCDMA via the MMR_REG_REQ and UE is currently registered
                   for CS+PS service on GSM RAT.
06/18/04   jca     Added variable gmm_pending_ra_update_type.
06/23/04   ks      Now sending RAU_FINISHED and RAU_STARTED events to re-sequence
                   the GPRS lower layers when a RAU is triggered in RAU_INITIATED state.
08/11/04   ks      Removed the GMM_TRIGGER_RAU_FINISHED when a reselection occured
                   to a different RA cell when RAU is in progress.
09/20/04   jca     Added missing conditional statement so that GMM performs RAU
                   on transition to GMM_REGISTERED.NORMAL_SERVICE state when
                   the GPRS Update Status is set to GU2 NOT UPDATED.
09/22/04   jca     Removed restriction on attempting to initiate GPRS Attach
                   procedure when operating in NMO 2 in GSM mode.
09/30/04   jca     Fixed code to prevent RAU on WNMO1/2->GNMO3 reselection.
10/05/04   jca     Initiate CM SERVICE REQUEST if mm_held_cnm_service_req.present == TRUE
                   on transition to GMM_REGISTERED.NORMAL_SERVICE state from
                   GMM_ROUTING_AREA_UPDATING_INITIATED state.
11/16/04   sv      Additions for handling of the new primitive:
                   RRC_SERVICE_IND_FROM_OTA_MSG
12/17/04   vdr     Now invalidating authentication cache memory when GMM state
                   becomes GMM_DEREGISTERED in UMTS mode
12/23/04   jca     Added else clause to initiate GPRS Attach on transition to
                   GMM_DEREGISTERED.NORMAL_SERVICE state if op mode set to PS_ONLY.
12/24/04   abe     Once GPRS detach accepted, stop PS timers T3311, T3312 &
                   T3302
01/14/05   jca     Send an immediate CNF to PS Attach on Demand request (MMR_REG_REQ)
                   if registered for CS services and send PS registration status
                   later in a MMR_SERVICE_IND.
02/03/05   jca     Added missing conditional statement so that GMM performs RAU
                   on transition to GMM_REGISTERED.NORMAL_SERVICE state when
                   the GPRS Update Status is set to GU2 NOT UPDATED.
03/02/05   jca     Ensure gmm_substate is set to new_substate input value before
                   performing actions taken on transition to new substate
                   when executing function gmm_set_substate().
03/23/05   sm      In case of NMO 1 to 2 change and SIM is CS_INVALID,
                   then RAU  is triggred and MM is set into IDLE state
04/20/05   hj      When the UE is paged in Limited Service for GCF Test Case,
                   the page is ignored.
06/13/05   jca     Now sending GMM_TRIGGER_RAU_FINISHED event to LLC if the
                   GMM substate is set to GMM_ATTEMPTING_TO_UPDATE_MM.
06/17/05   jca     Added variable gmm_previous_gprs_service_state.
08/08/05   vdr     Now sending GMM_TRIGGER_DETACH_STARTED event to gmmllcm.c
08/14/05   hj      Added handling for monitoring CCCH paging channel when detached from
                   GPRS Services.
08/19/05  jca      Now processing GMM_NORMAL_SERVICE substate transition while
                   GMM current state set to GMM_SERVICE_REQUEST_INITIATED.
09/21/05  vdr      Now sending GMM_TRIGGER_LLC_SUSPENSION to gmmllcm.c when GMM is
                   REGISTERED and goes into Limited Service and GMM_TRIGGER_LLC_RESUMPTION
                   when it gains service again
10/06/05   ajt     Added definition for mm_lcs_domain                                      
11/11/05   ss      Added gmm_service_request_data_accepted for Rel5.
12/05/05  vdr      Now triggering GMM_TRIGGER_LLC_RESUMPTION to gmmllcm.c when GMM is
                   REGISTERED and goes into GMM_NORMAL_SERVICE, GMM_ATTEMPTING_TO_UPDATE_MM or
                   GMM_IMSI_DETACH_INITIATED substates and gmmllcm.c has been suspended earlier
                   due to Limitied Service
12/14/05  abe      Supported changes for FEATURE_GSM_DTM
01/24/06  ajt      Perform RAU when moving from PLMN_Search GMM Substate to Normal Service and 
                   GMM State is RAU Initiated. This scenario is encountered when Inter-RAT (G->W)
                   is triggered by REG while UE is in RAU Initiated state in G RAT. In W->G inter-RAT,
                   this scenario is not expected as UE will first send Abort Req to RRC, and on 
                   receiving Abort Cnf, GMM will move to Registered State.
02/24/06  abe      Now processing RR_PAGING_IND even if MM is not in idle but DTM supported.
03/22/06  vdr      Trigger IMSI_DETACH_FINISHED event when gmm substate is
                   changed from IMSI_DETACH_INITIATED to NORMAL_SERVICE.
04/25/06  up       Resume LLC when GMM substate changes from 6 to 0.
05/15/06  up       Modified gmm_set_substate to keep the RAU/Attach
                   initiated functions out of reg send message block.
05/16/06  sn       Updated gmm_set_substate to update service info before ,
                   sending reg_cnf when moving from cs only service to cs_ps.
06/07/06  up       Initiate RAU REG_REQ is received for PS call and 
                   GMM status not UPDATED.
06/14/06  up       Called new gmm_update_mm_serving_plmn_info for GSTK.
06/23/06  up       Force RAU when UE goes OOS in GMM_SUSPEND.
07/14/06   pk     Updated for Feature DSAC
01/24/07   sn      Update ra_update_type before calling RA initiated function.
02/26/08   rk     removed gmm_inter_rat_follow_on_ind as its not used.
12/18/09   RI      Renamed gmm_load_sim_data() to gmm_load_psloci_data()
07/07/11   Rajesh  Calling emm_multimode_detach function whenever GMM state
                   moves to NULL
06/30/11   rajesh    Changes made for implementing PS detach timer in NAS.
07/13/11   rajesh  Handling expiry of T3412 and T3423 when ISR is ON
08/16/11   rajesh  Reset gmm_gprs_attach_pending when GMM state is moved to REGISTERED.
08/25/11   jbk     API to check if PLMN ID is valid
09/06/11   rajesh  Resetting "mm_is_mapped_keys_valid" when GMM moves to DEREGISTERED state.
10/04/11   sla     Dont initiate LU/RAU when LAI/RAI is in foreground search LAI list
10/27/11   rajesh  Changing MSG_FATAL_DS to MSG_ERROR_DS to avoid crash when gmm receives a 
                   message with unknown message set.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "gmm_v.h"
#include "gmm_int_v.h"
#include "mm_mm_v.h"
#include "rrcmmif.h"
#include "mm_v.h"
#include "mm_sim.h"
#include "gmm_msg.h"
#include "cause.h"
#include "smtask.h"
#include "smtask_v.h"
#include "mm_coord.h"
#include "gmminfo.h"
#include "mmrat.h"
#include "mmauth.h"
#include "sys_eplmn_list_v.h"
#include <stringl/stringl.h>
#ifdef FEATURE_GSTK
#include "mm_gstk.h"
#endif

#ifdef FEATURE_TC
#include "tc_gmm.h"
#include "tctask.h"
#include "tctask_v.h"
#endif

#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#include "gmmsuspend.h"
#endif
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#ifdef FEATURE_LTE
#include "emm.h"
#include "emm_utility.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

/* -------------------
** GMM state variables
** ------------------- */
gmm_state_type         gmm_state_sim[MAX_NAS_STACKS];
gmm_substate_type      gmm_substate_sim[MAX_NAS_STACKS];
pmm_mode_type          pmm_mode_sim[MAX_NAS_STACKS];
boolean                gmm_controls_resel_dec_mt_ps_call_sim[MAX_NAS_STACKS];

/* ------------------
** GMM SIM data items
** ------------------ */
gmm_ptmsi_type           gmm_stored_ptmsi_sim[MAX_NAS_STACKS];
gmm_ptmsi_signature_type gmm_stored_ptmsi_signature_sim[MAX_NAS_STACKS];
routing_area_id_T        gmm_reg_rejected_rai_sim[MAX_NAS_STACKS];
routing_area_id_T        gmm_stored_rai_sim[MAX_NAS_STACKS];
#ifdef FEATURE_NAS_REL11
PLMN_id_T                gmm_previous_plmn_with_t3302_value_sim[MAX_NAS_STACKS];
#endif
gmm_update_status_type   gmm_update_status_sim[MAX_NAS_STACKS];

service_state_type        gmm_previous_service_state_sim[MAX_NAS_STACKS];
service_state_type        gmm_previous_gprs_service_state_sim[MAX_NAS_STACKS];
network_op_mode_type      gmm_previous_network_op_mode_sim[MAX_NAS_STACKS];
routing_area_id_T         gmm_previous_serving_rai_sim[MAX_NAS_STACKS];
routing_area_id_T         gmm_previous_registration_serving_rai_sim[MAX_NAS_STACKS];

sys_plmn_id_s_type        gmm_3gpp_t3346_plmn_id_sim[MAX_NAS_STACKS];
#define  gmm_3gpp_t3346_plmn_id  gmm_3gpp_t3346_plmn_id_sim[mm_as_id]

/*gmm_previous_T3311_gmm_substate is used to remember
    gmm substate when T3311 was started last time.*/
gmm_substate_type         gmm_previous_T3311_gmm_substate_sim[MAX_NAS_STACKS];

inter_task_lai_T          gmm_current_lai_sim[MAX_NAS_STACKS];
routing_area_code_T       gmm_current_rac_sim[MAX_NAS_STACKS];

gmm_attach_type_type          gmm_attach_type_sim[MAX_NAS_STACKS];
gmm_ra_update_type_type       gmm_ra_update_type_sim[MAX_NAS_STACKS];
gmm_ra_update_type_type       gmm_pending_ra_update_type_sim[MAX_NAS_STACKS];
gmm_service_request_type_type gmm_service_request_type_sim[MAX_NAS_STACKS];
gmm_detach_type_type          gmm_detach_type_sim[MAX_NAS_STACKS];

byte t3310_timeout_counter_sim[MAX_NAS_STACKS];
byte t3321_timeout_counter_sim[MAX_NAS_STACKS];
byte t3330_timeout_counter_sim[MAX_NAS_STACKS];

dword gmm_t3312_timer_value_sim[MAX_NAS_STACKS];
dword gmm_t3302_timer_value_sim[MAX_NAS_STACKS];

#ifdef FEATURE_LTE
byte gmm_nv_eutra_measurement_reporting_support_sim[MAX_AS_IDS];
#endif

boolean gmm_periodic_rau_pending_sim[MAX_NAS_STACKS];
boolean gprs_attach_upon_entry_to_a_new_cell_sim[MAX_NAS_STACKS];
boolean gmm_power_down_active_sim[MAX_NAS_STACKS];

byte gmm_service_request_initiator_sim[MAX_NAS_STACKS];

boolean gmm_integrity_protection_activated_sim[MAX_NAS_STACKS];
boolean gmm_ps_signalling_connected_sim[MAX_NAS_STACKS];

boolean gmm_timer_deactivated_sim[MAX_NAS_STACKS];

boolean gmm_gprs_attach_pending_sim[MAX_NAS_STACKS];
boolean gmm_gprs_detach_pending_sim[MAX_NAS_STACKS];
boolean gmm_ra_update_pending_sim[MAX_NAS_STACKS];
boolean gmm_detach_after_rau_pending_sim[MAX_NAS_STACKS];

boolean gmm_follow_on_request_sim[MAX_NAS_STACKS];
boolean gmm_directed_signalling_reestablishment_sim[MAX_NAS_STACKS];

boolean gmm_xid_negotiation_completed_sim[MAX_NAS_STACKS];
uint16  gmm_max_L3_length_sim[MAX_NAS_STACKS] ;

#ifdef FEATURE_NAS_REL10
byte    gmm_stored_nw_feature_support_sim[MAX_NAS_STACKS];

boolean gmm_is_congestion_due_to_rau_sim[MAX_NAS_STACKS]; 
#define gmm_is_congestion_due_to_rau gmm_is_congestion_due_to_rau_sim[mm_as_id]

dword   gmm_t3346_backoff_timer_value_sim[MAX_NAS_STACKS];
#define gmm_t3346_backoff_timer_value  gmm_t3346_backoff_timer_value_sim[mm_as_id]
#endif

#ifdef FEATURE_NAS_REL11
boolean gmm_is_lw_to_g_interrat_in_progress_sim[MAX_NAS_STACKS];
#endif

boolean gmm_foreground_search_pending_sim[MAX_NAS_STACKS];

boolean gmm_ptmsi_sig_tobe_deleted_sim[MAX_NAS_STACKS] ;

boolean gmm_manual_mode_user_selection_procedure_sim[MAX_NAS_STACKS] ;
boolean gmm_processing_directed_signalling_reestablishment_sim[MAX_NAS_STACKS];

#ifdef FEATURE_REL6_PS_FOP
/* PS Follow on Proceed flag */
boolean gmm_ps_fop_req_sim[MAX_NAS_STACKS];
ps_fop_resp_type gmm_ps_fop_resp_sim[MAX_NAS_STACKS];
#endif/* FEATURE_REL6_PS_FOP */

#ifdef FEATURE_GSM_GPRS

/*
** RAU Required Indicator
*/
boolean gmm_rau_is_required_sim[MAX_NAS_STACKS];

/*
** Status indication from GERAN that RAU Request message has been txed successfully has been received
*/
boolean gmm_rau_tx_is_acked_sim[MAX_NAS_STACKS];

/*
** The purpose of the REG Report Indicator to report to REG upon
** completion of the UMTS-To-GSM Handover or Cell Reselection
*/
boolean gmm_reg_report_required_sim[MAX_NAS_STACKS];

/*
** PS Data available indicator
*/
boolean gmm_ps_data_available_sim[MAX_NAS_STACKS];



static boolean gmm_resume_llc_ind_sim[MAX_NAS_STACKS];

#endif /* #ifdef FEATURE_GSM_GPRS */

mm_lcs_domain_e_type mm_lcs_domain_sim[MAX_NAS_STACKS];

boolean gmm_is_suspended_sim[MAX_NAS_STACKS];

sys_radio_access_tech_e_type gmm_last_signalling_sent_rat_sim[MAX_NAS_STACKS] ;

#ifdef FEATURE_GPRS_PS_HANDOVER
gmm_psho_status_e_type gmm_psho_status_sim[MAX_NAS_STACKS];

byte gmm_psho_nas_container_sim[MAX_NAS_STACKS][PSHO_NAS_CNT_SIZE];

boolean gmm_psho_gea_change_sim[MAX_NAS_STACKS];
#endif

boolean gmm_is_ps_active_sim[MAX_NAS_STACKS];

/*
** This will be set during WtoG inter-rat if  mode is PMM_CONNECTED or
** during GtoW if glif state is GMM_READY_STATE
*/
boolean gmm_is_prev_mode_connected_sim[MAX_NAS_STACKS];

#define  gmm_resume_llc_ind gmm_resume_llc_ind_sim[mm_as_id]

boolean gmm_is_ptmsi_present_in_prev_gmm_msg_sim[MAX_NAS_STACKS] ;
boolean gmm_is_tmsi_present_in_prev_gmm_msg_sim[MAX_NAS_STACKS] ;

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
uint8 gmm_transaction_id_sim[MAX_NAS_STACKS];
#define gmm_transaction_id gmm_transaction_id_sim[mm_as_id]
byte  gmm_last_signalling_message_type_sim[MAX_NAS_STACKS];
#define gmm_last_signalling_message_type gmm_last_signalling_message_type_sim[mm_as_id]
#endif

boolean gmm_pended_rat_resume_req_sim[MAX_NAS_STACKS];
#define gmm_pended_rat_resume_req gmm_pended_rat_resume_req_sim[mm_as_id]

boolean gmm_detach_in_progress_sim[MAX_NAS_STACKS];
#define gmm_detach_in_progress gmm_detach_in_progress_sim[mm_as_id]

boolean gmm_multi_slot_ra_update_pending_sim[MAX_NAS_STACKS];
#define gmm_multi_slot_ra_update_pending gmm_multi_slot_ra_update_pending_sim[mm_as_id]

#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
boolean gmm_force_registration_sim[MAX_NAS_STACKS];
#define gmm_force_registration gmm_force_registration_sim[mm_as_id]
#endif
extern sys_radio_access_tech_e_type     mm_service_loss_on_rat_sim[MAX_NAS_STACKS];
#define  mm_service_loss_on_rat mm_service_loss_on_rat_sim[mm_as_id]

dword  gmm_t3319_timer_value_sim[MAX_NAS_STACKS];
#define gmm_t3319_timer_value gmm_t3319_timer_value_sim[mm_as_id]

boolean gmm_attach_request_on_network_reject_sim[MAX_NAS_STACKS];
#define gmm_attach_request_on_network_reject gmm_attach_request_on_network_reject_sim[mm_as_id]

byte    gprs_attach_attempt_counter_sim[MAX_NAS_STACKS];
#define gprs_attach_attempt_counter gprs_attach_attempt_counter_sim[mm_as_id]

byte    gmm_rau_attempt_counter_sim[MAX_NAS_STACKS];
#define gmm_rau_attempt_counter gmm_rau_attempt_counter_sim[mm_as_id]

boolean gmm_registration_initiated_sim[MAX_NAS_STACKS];
#define gmm_registration_initiated gmm_registration_initiated_sim[mm_as_id]

boolean gmm_power_on_attach_completed_sim[MAX_NAS_STACKS];
#define gmm_power_on_attach_completed gmm_power_on_attach_completed_sim[mm_as_id]

boolean gmm_processed_mt_imsi_detach_request_sim[MAX_NAS_STACKS];
#define gmm_processed_mt_imsi_detach_request gmm_processed_mt_imsi_detach_request_sim[mm_as_id]

boolean gmm_cause_directed_signalling_reest_sim[MAX_NAS_STACKS];
#define gmm_cause_directed_signalling_reest gmm_cause_directed_signalling_reest_sim[mm_as_id]

boolean gmm_extend_connection_due_to_mt_csfb_sim[MAX_NAS_STACKS];
#define gmm_extend_connection_due_to_mt_csfb gmm_extend_connection_due_to_mt_csfb_sim[mm_as_id]


extern boolean gmm_force_local_detach_sim[];

#else

/* -------------------
** GMM state variables
** ------------------- */
boolean                gmm_controls_resel_dec_mt_ps_call;
gmm_state_type         gmm_state;
gmm_substate_type      gmm_substate;
pmm_mode_type          pmm_mode;

/* ------------------
** GMM SIM data items
** ------------------ */
gmm_ptmsi_type           gmm_stored_ptmsi;
gmm_ptmsi_signature_type gmm_stored_ptmsi_signature;
routing_area_id_T        gmm_stored_rai;
#ifdef FEATURE_NAS_REL11
PLMN_id_T                gmm_previous_plmn_with_t3302_value;
#endif
routing_area_id_T        gmm_reg_rejected_rai;
gmm_update_status_type   gmm_update_status;

service_state_type        gmm_previous_service_state;
service_state_type        gmm_previous_gprs_service_state;
network_op_mode_type      gmm_previous_network_op_mode;
routing_area_id_T         gmm_previous_serving_rai;
routing_area_id_T         gmm_previous_registration_serving_rai;

sys_plmn_id_s_type        gmm_3gpp_t3346_plmn_id  ;

/*gmm_previous_T3311_gmm_substate is used to remember 
    gmm substate when T3311 was started last time.*/
gmm_substate_type         gmm_previous_T3311_gmm_substate;

inter_task_lai_T          gmm_current_lai;
routing_area_code_T       gmm_current_rac;

gmm_attach_type_type          gmm_attach_type;
gmm_ra_update_type_type       gmm_ra_update_type;
gmm_ra_update_type_type       gmm_pending_ra_update_type;
gmm_service_request_type_type gmm_service_request_type;
gmm_detach_type_type          gmm_detach_type;

byte gprs_attach_attempt_counter;
byte gmm_rau_attempt_counter;
byte t3310_timeout_counter;
byte t3321_timeout_counter;
byte t3330_timeout_counter;

dword gmm_t3312_timer_value;
dword gmm_t3302_timer_value;

#if defined(FEATURE_LTE)
byte gmm_nv_eutra_measurement_reporting_support;
#endif
boolean gmm_periodic_rau_pending;
boolean gprs_attach_upon_entry_to_a_new_cell;
boolean gmm_power_down_active;

byte gmm_service_request_initiator;

boolean gmm_integrity_protection_activated;
boolean gmm_ps_signalling_connected;

boolean gmm_timer_deactivated;

boolean gmm_gprs_attach_pending;
boolean gmm_gprs_detach_pending;
boolean gmm_ra_update_pending;
boolean gmm_multi_slot_ra_update_pending;
boolean gmm_detach_after_rau_pending;

boolean gmm_is_ptmsi_present_in_prev_gmm_msg;
boolean gmm_is_tmsi_present_in_prev_gmm_msg;

boolean gmm_follow_on_request;
boolean gmm_directed_signalling_reestablishment;

boolean gmm_xid_negotiation_completed;
uint16  gmm_max_L3_length;

#ifdef FEATURE_NAS_REL10
byte    gmm_stored_nw_feature_support;

boolean gmm_is_congestion_due_to_rau;
dword   gmm_t3346_backoff_timer_value;
#endif

#ifdef FEATURE_NAS_REL11
boolean gmm_is_lw_to_g_interrat_in_progress;
#endif

boolean gmm_foreground_search_pending;

boolean gmm_ptmsi_sig_tobe_deleted;

boolean gmm_manual_mode_user_selection_procedure;
boolean gmm_processing_directed_signalling_reestablishment;

#ifdef FEATURE_REL6_PS_FOP
/* PS Follow on Proceed flag */
boolean gmm_ps_fop_req;
ps_fop_resp_type gmm_ps_fop_resp;
#endif/* FEATURE_REL6_PS_FOP */

#ifdef FEATURE_GSM_GPRS

/*
** RAU Required Indicator
*/
boolean gmm_rau_is_required;

/*
** Status indication from GERAN that RAU Request message has been txed successfully has been received
*/
boolean gmm_rau_tx_is_acked;

/*
** The purpose of the REG Report Indicator to report to REG upon
** completion of the UMTS-To-GSM Handover or Cell Reselection
*/
boolean gmm_reg_report_required;

/*
** PS Data available indicator
*/
boolean gmm_ps_data_available;



static boolean gmm_resume_llc_ind;

#endif /* #ifdef FEATURE_GSM_GPRS */

mm_lcs_domain_e_type mm_lcs_domain;

boolean gmm_is_suspended;

sys_radio_access_tech_e_type gmm_last_signalling_sent_rat;

#ifdef FEATURE_GPRS_PS_HANDOVER
gmm_psho_status_e_type gmm_psho_status ;

byte gmm_psho_nas_container[PSHO_NAS_CNT_SIZE];

boolean gmm_psho_gea_change;
#endif

boolean gmm_is_ps_active;

/*
** This will be set during WtoG inter-rat if  mode is PMM_CONNECTED or 
** during GtoW if glif state is GMM_READY_STATE 
*/
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
boolean gmm_is_prev_mode_connected;
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
uint8 gmm_transaction_id;
byte  gmm_last_signalling_message_type;
#endif


boolean gmm_pended_rat_resume_req;
boolean gmm_detach_in_progress;
/* gmm_force_registration is also used to trigger RAU whenever lte rat is added
    to rat priority list apart from triggering RAU on EF RAT REFRESH */
#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
boolean gmm_force_registration;
#endif

extern sys_radio_access_tech_e_type mm_service_loss_on_rat;
dword gmm_t3319_timer_value;

boolean gmm_attach_request_on_network_reject;

boolean gmm_registration_initiated;
boolean gmm_processed_mt_imsi_detach_request;
boolean gmm_extend_connection_due_to_mt_csfb;


boolean gmm_cause_directed_signalling_reest;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

#if defined(FEATURE_LTE)
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
byte gmm_nv_geran_to_eutra_measurement_support_sim[MAX_AS_IDS];
#define gmm_nv_geran_to_eutra_measurement_support gmm_nv_geran_to_eutra_measurement_support_sim[mm_sub_id]
#else
byte gmm_nv_geran_to_eutra_measurement_support;
#endif
#endif

#ifdef FEATURE_LTE
/* This variable is used to indicate if we advertized LTE capability to the network
   in last RAU or Attach request*/
#if defined (FEATURE_SGLTE) || defined (FEATURE_DUAL_SIM)
boolean gmm_is_lte_advertised_in_last_reg_sim[MAX_NAS_STACKS];
#define gmm_is_lte_advertised_in_last_reg gmm_is_lte_advertised_in_last_reg_sim[mm_as_id]
#else
boolean gmm_is_lte_advertised_in_last_reg = FALSE;
#endif 
#endif

#ifdef FEATURE_LTE
#ifdef FEATURE_DUAL_SIM
boolean gmm_set_drx_req_pending_sim[MAX_AS_IDS];
#define gmm_set_drx_req_pending gmm_set_drx_req_pending_sim[mm_sub_id]
boolean gmm_lte_deact_isr_timer_expired_sim[MAX_AS_IDS];
#define gmm_lte_deact_isr_timer_expired gmm_lte_deact_isr_timer_expired_sim[mm_sub_id]
dword gmm_t3323_timer_value_sim[MAX_AS_IDS];
#define gmm_t3323_timer_value gmm_t3323_timer_value_sim[mm_sub_id]
#else
boolean gmm_set_drx_req_pending;
/* This variable is set when T3423 expires and TIN is RAT RELATED TMSI
 ** This is used to start RAU or LA update when UE comes back to service in G or W
 */
boolean gmm_lte_deact_isr_timer_expired;
dword gmm_t3323_timer_value;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_drx_cn_coeff_s1 gmm_drx_cn_coeff_s1_sim[mm_sub_id]
extern sys_drx_cn_coefficient_s1_e_type gmm_nv_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_nv_drx_cn_coeff_s1 gmm_nv_drx_cn_coeff_s1_sim[mm_sub_id]

/* gmm_reg_lte_cs_param is updated after attach/RAU is complete*/
sys_lte_cs_domain_param_s_type  gmm_reg_lte_cs_param_sim[MAX_NAS_STACKS];
#define gmm_reg_lte_cs_param gmm_reg_lte_cs_param_sim[mm_as_id]
/* gmm_reg_pended_lte_cs_param is used for storing voice domain pref of service request from CM,
   this is temp variable to store the last Voice domain preference */

sys_lte_cs_domain_param_s_type  gmm_reg_pended_lte_cs_param_sim[MAX_NAS_STACKS];
#define gmm_reg_pended_lte_cs_param gmm_reg_pended_lte_cs_param_sim[mm_as_id]

#else
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1;
extern sys_drx_cn_coefficient_s1_e_type gmm_nv_drx_cn_coeff_s1;
/* gmm_reg_lte_cs_param is updated after attach/RAU is complete*/
sys_lte_cs_domain_param_s_type  gmm_reg_lte_cs_param;
/* gmm_reg_pended_lte_cs_param is used for storing voice domain pref of service request from CM,
   this is temp variable to store the last Voice domain preference */
sys_lte_cs_domain_param_s_type  gmm_reg_pended_lte_cs_param;

#endif
#endif/*FEATURE_LTE*/

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
static boolean gmm_resume_llc_ind_sim[MAX_NAS_STACKS] = { FALSE, FALSE,FALSE } ;
#define  gmm_resume_llc_ind gmm_resume_llc_ind_sim[mm_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
static boolean gmm_resume_llc_ind_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
#define  gmm_resume_llc_ind gmm_resume_llc_ind_sim[mm_as_id]

#else
static boolean gmm_resume_llc_ind = FALSE ;
#endif


extern void mmcoord_process_mmr_reg_req( mm_cmd_type *message );

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a,(sys_modem_as_id_e_type)mm_as_id)
#endif
#ifdef FEATURE_MODEM_HEAP
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern byte* gmm_rat_chng_pended_msg_sim[MAX_NAS_STACKS];
extern boolean pending_gmm_msg_valid_sim[MAX_NAS_STACKS];
extern boolean gmm_rat_chng_pended_msg_valid_sim[MAX_NAS_STACKS];
#else
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
extern byte *gmm_rat_chng_pended_msg;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
extern boolean pending_gmm_msg_valid;
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
extern boolean gmm_rat_chng_pended_msg_valid;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
#endif

#endif

#if defined (FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE) 
byte    gmm_reject_cause_sim[MAX_NAS_STACKS];
#else
byte    gmm_reject_cause;
#endif

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM) || defined FEATURE_SGLTE
byte    gmm_irat_lu_pending_sim[MAX_NAS_STACKS];
#define gmm_irat_lu_pending gmm_irat_lu_pending_sim[mm_as_id]

boolean gmm_detach_started_sim[MAX_NAS_STACKS];
#define gmm_detach_started gmm_detach_started_sim[mm_as_id]
#else
byte    gmm_irat_lu_pending;
boolean gmm_detach_started;
#endif

#ifdef FEATURE_DUAL_SIM
boolean gmm_cm_ds_tuneaway_status; 
#endif

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION gmm_is_lte_cap_changed

DESCRIPTION
  This function returns TRUE if there is a change in LTE capabilites
  from the one advertised in last registration to GW network.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean gmm_is_lte_cap_changed( void )
{
  boolean is_lte_present_in_rat_list = mm_checkif_rat_pri_list_has_rat(
       &mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS);
  MSG_HIGH_DS_2(MM_SUB,"gmm_is_lte_advertised_in_last_reg %d is_lte_present_in_rat_list %d",
                  gmm_is_lte_advertised_in_last_reg,is_lte_present_in_rat_list);
  if(gmm_is_lte_advertised_in_last_reg != is_lte_present_in_rat_list)
  {
    return TRUE;
  }
  return FALSE;
}
#endif

/*===========================================================================

FUNCTION GMM_CHANGE_OF_NETWORK_MODE_OF_OPERATION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_change_of_network_mode_of_operation( void )
{
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif  

  switch (gmm_previous_network_op_mode)
  {
    case NETWORK_OP_MODE_1:
      if
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
      ( 
#endif
       (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
         ||
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
        )
#endif
      {
#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (mm_is_lai_in_foreground_search_lai_timer_list((byte*)&gmm_current_lai))
        {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Do LU after foreground timer expiry");
            mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list((byte*)&gmm_current_lai);
        }
        else 
#endif
#ifdef FEATURE_NAS_REL10
        if((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
             &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
          )
        {
            MSG_HIGH_DS_0(MM_SUB, "=MM= T3246 running - No LU ");
        }
        else
#endif
        if (mm_state == MM_IDLE)
        {
            mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }
        else if ((mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                 (mm_state != MM_LOCATION_UPDATE_INITIATED))
        {
          mm_pending_lu_request = NORMAL_LU;
        }

        gmm_ra_update_type = GMM_RA_UPDATING;
        gmm_initiate_routing_area_update();
      }
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3
      else if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
      {
#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (mm_is_lai_in_foreground_search_lai_timer_list((byte*)&gmm_current_lai))
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= Do LU after foreground timer expiry");
          mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list((byte*)&gmm_current_lai);
        }
        else
#endif
#ifdef FEATURE_NAS_REL10
        if((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
              &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif 
          )
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= T3246 running - No LU in NMO3 ");
        }
        else
#endif

        if (mm_state == MM_IDLE)
        {
          mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }
        else if ((mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                 (mm_state != MM_LOCATION_UPDATE_INITIATED))
        {
          mm_pending_lu_request = NORMAL_LU;
        }

        gmm_gprs_detach_pending = TRUE;
#ifdef FEATURE_GSM_GPRS
        gmm_rau_is_required     = FALSE;
#endif /*FEATURE_GSM_GPRS*/
       
        /*****************************************************************************
        *Reset this flag to false as we are not trying PS registration and if PS fop
        *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
        *of setting PS fop to true
        *****************************************************************************/ 
        
        gmm_follow_on_request   = FALSE;

      }
#endif
      else
      {
        MSG_FATAL_DS( MM_SUB, "=MM= Unexpected value %d", mm_serving_plmn.network_op_mode, 0,0 );
      }
      break;

    case NETWORK_OP_MODE_2:
      if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
      {
      
#ifdef FEATURE_DSAC
        if (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) &&
                  mm_cs_access_barred_to_unbarred)
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= LU Required as cs domain is unbarred while PS is barred");

#ifdef FEATURE_NAS_REL10
          if((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                 &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif 
            )
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= T3246 running - No LU in NMO1 ");
          }
          else
#endif
          if (mm_state == MM_IDLE)
          {
            mm_initiate_lu( MM_LU_CAUSE_NORMAL );
          }          
          else
          {
            mm_pending_lu_request = NORMAL_LU;
          }
          mm_cs_access_barred_to_unbarred = FALSE;
        }
        else
        {
          gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
#ifdef FEATURE_LTE
          if (emm_ctrl_data_ptr->sg_association_to_be_established == 
                                       NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC)
          {
            emm_ctrl_data_ptr->sg_association_to_be_established = 
                            NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
          }
#endif
                gmm_initiate_routing_area_update();
        }               
#else      
        gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
        gmm_initiate_routing_area_update();
#endif      
      }
      else if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
      {
 #ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
        if (mm_state == MM_IDLE)
        {
          gmm_ra_update_type = GMM_RA_UPDATING;
          gmm_initiate_routing_area_update();
        }
        else
        {
          mm_stop_timer( TIMER_T3311 );
          mm_stop_timer( TIMER_T3302 );
          
          gmm_pending_ra_update_type = GMM_RA_UPDATING;
          gmm_ra_update_pending      = TRUE;
        }
 #else
 #ifdef FEATURE_ENHANCED_NW_SELECTION
        if (mm_is_lai_in_foreground_search_lai_timer_list((byte*)&gmm_current_lai))
        {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Do LU after foreground timer expiry");
            mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list((byte*)&gmm_current_lai);
        }
        else 
#endif
#ifdef FEATURE_NAS_REL10
        if((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                 &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
          )
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= T3246 running - No LU ");
        }
        else
#endif
        if (mm_state == MM_IDLE)
        {
          mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }
        else if ((mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                 (mm_state != MM_LOCATION_UPDATE_INITIATED))
        {
          mm_pending_lu_request = NORMAL_LU;
        }

        gmm_gprs_detach_pending = TRUE;
#ifdef FEATURE_GSM_GPRS
        gmm_rau_is_required     = FALSE;
#endif /*FEATURE_GSM_GPRS*/

        /*****************************************************************************
        *Reset this flag to false as we are not trying PS registration and if PS fop
        *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
        *of setting PS fop to true
        *****************************************************************************/

        gmm_follow_on_request   = FALSE;
#endif
      }
      else
      {
        MSG_FATAL_DS( MM_SUB, "=MM= Unexpected value %d", mm_serving_plmn.network_op_mode, 0,0 );
      }
      break;

    case NETWORK_OP_MODE_3:
      if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
      {
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
        gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
        gmm_initiate_routing_area_update();
#else
       if(gmm_state == GMM_REGISTERED)
       {
           gmm_previous_network_op_mode = mm_serving_plmn.network_op_mode;
           gmm_gprs_detach_pending = FALSE;
           gmm_initiate_routing_area_update();
       }
       else
       {
          gmm_initiate_gprs_attach();
       }
#endif
      }
      else if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
      {
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
       if (mm_state == MM_IDLE)
       {
         gmm_ra_update_type = GMM_RA_UPDATING;
         gmm_initiate_routing_area_update();
       }
       else
       {
         mm_stop_timer( TIMER_T3311 );
         mm_stop_timer( TIMER_T3302 );
          
         gmm_pending_ra_update_type = GMM_RA_UPDATING;
         gmm_ra_update_pending      = TRUE;
       }
#else
#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (mm_is_lai_in_foreground_search_lai_timer_list((byte*)&gmm_current_lai))
        {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Do LU after foreground timer expiry");
            mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list((byte*)&gmm_current_lai);
        }
        else 
#endif
#ifdef FEATURE_NAS_REL10
        if((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                 &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
          )
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= T3246 running - No LU in NMO2");
        }
        else
#endif
        if (mm_state == MM_IDLE)
        {
          mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }
        else if ((mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                 (mm_state != MM_LOCATION_UPDATE_INITIATED))
        {
          mm_pending_lu_request = NORMAL_LU;
        }

        gmm_gprs_attach_pending = TRUE;
#endif
      }
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected value %d", gmm_previous_network_op_mode, 0,0 );
  } /* end switch (gmm_previous_network_op_mode) */

  if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
  {
    gmm_previous_network_op_mode = mm_serving_plmn.network_op_mode;
  }

} /* end gmm_change_of_network_mode_of_operation() */


#ifdef FEATURE_NAS_REL10
/*===========================================================================

FUNCTION GMM_CHANGE_OF_NMO_DURING_T3346_ACTIVE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_change_of_nmo_during_t3346_active( void )
{
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif  

  MSG_HIGH_DS_0(MM_SUB, "=MM= NMO changed during T3346 running ");

  switch (gmm_previous_network_op_mode)
  {
    case NETWORK_OP_MODE_1:
      if 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
      (
#endif
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
         ||
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
        )
#endif
      {
#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (mm_is_lai_in_foreground_search_lai_timer_list((byte*)&gmm_current_lai))
        {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Do LU after foreground timer expiry");
            mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list((byte*)&gmm_current_lai);
        }
        else 
#endif
        if (mm_state == MM_IDLE)
        {
            mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }
        else if ((mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                 (mm_state != MM_LOCATION_UPDATE_INITIATED))
        {
          mm_pending_lu_request = NORMAL_LU;
        }
      }
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3
      else if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
      {
#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (mm_is_lai_in_foreground_search_lai_timer_list((byte*)&gmm_current_lai))
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= Do LU after foreground timer expiry" );
          mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list((byte*)&gmm_current_lai);
        }
        else
#endif
        if (mm_state == MM_IDLE)
        {
          mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }
        else if ((mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                 (mm_state != MM_LOCATION_UPDATE_INITIATED))
        {
          mm_pending_lu_request = NORMAL_LU;
        }

        gmm_gprs_detach_pending = TRUE;
#ifdef FEATURE_GSM_GPRS
        gmm_rau_is_required     = FALSE;
#endif /*FEATURE_GSM_GPRS*/
       
        /*****************************************************************************
        *Reset this flag to false as we are not trying PS registration and if PS fop
        *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
        *of setting PS fop to true
        *****************************************************************************/ 
        
        gmm_follow_on_request   = FALSE;

      }
#endif
      else
      {
        MSG_FATAL_DS( MM_SUB, "=MM= Unexpected value %d", mm_serving_plmn.network_op_mode, 0,0 );
      }
      break;

    case NETWORK_OP_MODE_2:
      if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Ignore GMM Procedure in NMO1 - T3346 running " );
#ifdef FEATURE_DSAC
        if (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) &&
                  mm_cs_access_barred_to_unbarred)
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= LU Required as cs domain is unbarred while PS is barred");
          if (mm_state == MM_IDLE)
          {
            mm_initiate_lu( MM_LU_CAUSE_NORMAL );
          }          
          else
          {
            mm_pending_lu_request = NORMAL_LU;
          }
          mm_cs_access_barred_to_unbarred = FALSE;
        }               
#endif      
      }
      else if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
      {
 #ifdef FEATURE_ENHANCED_NW_SELECTION
        if (mm_is_lai_in_foreground_search_lai_timer_list((byte*)&gmm_current_lai))
        {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Do LU after foreground timer expiry");
            mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list((byte*)&gmm_current_lai);
        }
        else 
#endif
        if (mm_state == MM_IDLE)
        {
          mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }
        else if ((mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                 (mm_state != MM_LOCATION_UPDATE_INITIATED))
        {
          mm_pending_lu_request = NORMAL_LU;
        }

        gmm_gprs_detach_pending = TRUE;
#ifdef FEATURE_GSM_GPRS
        gmm_rau_is_required     = FALSE;
#endif /*FEATURE_GSM_GPRS*/

        /*****************************************************************************
        *Reset this flag to false as we are not trying PS registration and if PS fop
        *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
        *of setting PS fop to true
        *****************************************************************************/

        gmm_follow_on_request   = FALSE;
      }
      else
      {
        MSG_FATAL_DS( MM_SUB, "=MM= Unexpected value %d", mm_serving_plmn.network_op_mode, 0,0 );
      }
      break;

    case NETWORK_OP_MODE_3:
      if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
      {
         MSG_HIGH_DS_0( MM_SUB, "=MM= Ignore GMM Procedure since T3346 congestion timer is running" );
      }
      else if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
      {

#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (mm_is_lai_in_foreground_search_lai_timer_list((byte*)&gmm_current_lai))
        {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Do LU after foreground timer expiry");
            mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list((byte*)&gmm_current_lai);
        }
        else 
#endif
        if (mm_state == MM_IDLE)
        {
          mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }
        else if ((mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                 (mm_state != MM_LOCATION_UPDATE_INITIATED))
        {
          mm_pending_lu_request = NORMAL_LU;
        }
      }
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected value %d", gmm_previous_network_op_mode, 0,0 );
  } /* end switch (gmm_previous_network_op_mode) */

  if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
  {
    gmm_previous_network_op_mode = mm_serving_plmn.network_op_mode;
  }

} /* end gmm_change_of_nmo_during_t3346_active() */

#endif


/*===========================================================================

FUNCTION GMM_SET_STATE

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_set_state( gmm_state_type new_state )
{
  gmm_state_type previous_state;
  mmr_service_ind_s_type mmr_service_ind;

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  previous_state = gmm_state;

  /* ---------------------------------------
  ** Actions to take on entry into new state
  ** --------------------------------------- */
  switch (new_state)
  {
    case GMM_REGISTERED:
      gmm_gprs_attach_pending = FALSE;
#ifdef FEATURE_DUAL_SIM
      gmm_power_on_attach_completed = TRUE;
#endif
      break;

    case GMM_ROUTING_AREA_UPDATING_INITIATED:
#ifdef FEATURE_LTE
      if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
      {
        gmm_is_lte_advertised_in_last_reg = TRUE;
      }
      else
      {
        gmm_is_lte_advertised_in_last_reg = FALSE;
      }
#endif
#ifdef FEATURE_DUAL_DATA
    if ((mmode_nv_setting_for_device_mode != SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE) && mm_dds_sub_id != mm_sub_id)
    {
      mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_HIGH);
    }
#endif

#ifdef FEATURE_DUAL_SIM
      gmm_power_on_attach_completed = TRUE;
#endif
      break;

    case GMM_SERVICE_REQUEST_INITIATED:
      break ;

#ifdef FEATURE_GSM_GPRS
    case GMM_DEREGISTERED_INITIATED:
      if (is_gsm_mode())
      {
        gmmllc_gmm_trigger_llc_event( GMM_TRIGGER_DETACH_STARTED, FALSE, 0 );
      }
#if defined (FEATURE_SGLTE_DUAL_SIM) || defined (FEATURE_SVLTE_DUAL_SIM)
#ifdef FEATURE_DUAL_DATA
      if (((mm_current_ue_mode == SYS_UE_MODE_SGLTE) ||
         (mm_current_ue_mode == SYS_UE_MODE_SGLTE_ONLY) ||
         (mm_current_ue_mode == SYS_UE_MODE_SVLTE) ||
         (mm_current_ue_mode == SYS_UE_MODE_SVLTE_ONLY)) &&
         (mm_sub_id == MM_AS_ID_2))
      {
        mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_HIGH);
      }
#endif 
#endif
#ifdef FEATURE_DUAL_DATA
    if ((mmode_nv_setting_for_device_mode != SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE) && mm_dds_sub_id != mm_sub_id)
    {
      mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_HIGH);
    }
#endif

#ifdef FEATURE_DUAL_SIM
     gmm_power_on_attach_completed = TRUE;
#endif
      break;
#endif /*FEATURE_GSM_GPRS*/
    case GMM_DEREGISTERED:
     gmm_last_signalling_sent_rat = SYS_RAT_NONE;
      if (is_umts_mode())
      {
        auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID) ;
      }
      mm_send_cm_cipher_ind(SYS_SRV_DOMAIN_PS_ONLY,FALSE);
      mm_stop_timer( TIMER_T3319 );
      mm_stop_timer(TIMER_T3321);
      gmm_processing_directed_signalling_reestablishment = FALSE;
      gmm_is_ptmsi_present_in_prev_gmm_msg               = FALSE;
      gmm_is_tmsi_present_in_prev_gmm_msg                = FALSE;
      auth_ps_ciphering_algorithm                        = 0xFF ;
      gmm_gprs_detach_pending                            = FALSE;
#if defined(FEATURE_LTE)
      if ( is_umts_mode() || is_gsm_mode() )
      {
        emm_multimode_detach(); 

        //If set DRX is pending then send failure to REG
        if(gmm_set_drx_req_pending == TRUE)
        {
          mm_send_mmr_set_drx_cnf(FALSE);
        }  
      }
      if (mm_timer_status[ TIMER_T3323 ] == TIMER_ACTIVE)
      {
        mm_stop_timer(TIMER_T3323);
      }
      gmm_lte_deact_isr_timer_expired = FALSE;
      mm_service_loss_on_multimode_rat = SYS_RAT_NONE;
      /*In attach UE will always use default DRX
      Copy the DRX read from EFS to DRX used in registration*/
      gmm_drx_cn_coeff_s1 = gmm_nv_drx_cn_coeff_s1;
#endif
      mm_service_loss_on_rat = SYS_RAT_NONE;
      gmm_multi_slot_ra_update_pending = FALSE;
      /* Reset the below flag in case UE performs Local Detach to CS_ONLY op mode */
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
      {
         mm_is_gmm_moved_dereg_due_to_csreg = FALSE;
      }    
      gmm_detach_after_rau_pending   = FALSE;
/*Reset the flag when GMM is in De-registered state*/
#ifdef FEATURE_NAS_REL11
      gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif
#ifdef FEATURE_DUAL_DATA
      mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_LOW);
#endif 
      break;

    case GMM_NULL:
#if defined(FEATURE_LTE)
      if(previous_state != GMM_NULL &&
         previous_state != GMM_DEREGISTERED)
      {
        if (is_umts_mode() || is_gsm_mode())
        {
          emm_multimode_detach(); 
        }
        if (mm_timer_status[ TIMER_T3323 ] == TIMER_ACTIVE)
        {
          mm_stop_timer(TIMER_T3323);
        }
      }
#ifdef FEATURE_SGLTE
      if (MM_IS_SGLTE_ON_PS_STACK_ID)
      {
        mm_service_loss_on_multimode_rat = SYS_RAT_NONE;
      }
#endif
#endif
      mm_stop_timer(TIMER_T3321);
      mm_service_loss_on_rat = SYS_RAT_NONE;
      gmm_previous_T3311_gmm_substate = GMM_NULL_SUBSTATE;
      gmm_previous_registration_serving_rai.plmn_id.identity[0]       = 0xFF;
      gmm_previous_registration_serving_rai.plmn_id.identity[1]       = 0xFF;
      gmm_previous_registration_serving_rai.plmn_id.identity[2]       = 0xFF;
      gmm_previous_registration_serving_rai.location_area_code.lac[0] = 0xFF;
      gmm_previous_registration_serving_rai.location_area_code.lac[1] = 0xFE;
      gmm_previous_registration_serving_rai.routing_area_code         = 0xFF;
      
      gmm_delete_gprs_forbidden_rai();
      gmm_multi_slot_ra_update_pending = FALSE;
      gmm_detach_after_rau_pending     = FALSE;
#ifdef FEATURE_NAS_REL11
      gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif
      gmm_ra_update_pending = FALSE;
#ifdef FEATURE_GSM_GPRS
      gmm_rau_is_required = FALSE ;
#endif /*FEATURE_GSM_GPRS*/
      gmm_gprs_attach_pending = FALSE;
#ifdef FEATURE_DUAL_DATA
      mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_LOW);
#endif 

      break;

    case GMM_REGISTERED_INITIATED:
      gprs_attach_upon_entry_to_a_new_cell = FALSE;
#ifdef FEATURE_LTE
      if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
      {
        gmm_is_lte_advertised_in_last_reg = TRUE;
      }
      else
      {
        gmm_is_lte_advertised_in_last_reg = FALSE;
      }
#endif 
      

#if defined (FEATURE_SGLTE_DUAL_SIM) || defined (FEATURE_SVLTE_DUAL_SIM)
#ifdef FEATURE_DUAL_DATA
      if (((mm_current_ue_mode == SYS_UE_MODE_SGLTE) ||
         (mm_current_ue_mode == SYS_UE_MODE_SGLTE_ONLY) ||
         (mm_current_ue_mode == SYS_UE_MODE_SVLTE) ||
         (mm_current_ue_mode == SYS_UE_MODE_SVLTE_ONLY)) &&
         (mm_sub_id == MM_AS_ID_2))
      {
        mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_HIGH);
      }
#endif 
#endif
#ifdef FEATURE_DUAL_DATA
      if ((mmode_nv_setting_for_device_mode != SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE) && mm_dds_sub_id != mm_sub_id)
      {
        mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_HIGH);
      }
#endif

      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Invalid GMM state %d", new_state, 0,0 );
  }

  MSG_HIGH_DS_2( MM_SUB, "=MM= GMM state change from %d to %d", gmm_state, new_state );
  gmm_state = new_state;

#ifdef FEATURE_GSM_GPRS
  if (previous_state != new_state)
  {

    switch (new_state)
    {
      case GMM_REGISTERED_INITIATED:

        switch (previous_state)
        {
          case GMM_NULL:
          case GMM_DEREGISTERED:

            if (is_gsm_mode())
            {
              gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_GMM_ATTACH_STARTED,FALSE,0);
            }
            break;

          default:
            break;
        }
        break;

      case GMM_REGISTERED:

        gmm_follow_on_request = FALSE;

        if(!is_gsm_mode())
        {
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_GMM_STABILIZED,FALSE,0);
        }

        switch (previous_state)
        {
          case GMM_NULL:
          case GMM_DEREGISTERED:
          case GMM_REGISTERED_INITIATED:
            if (is_gsm_mode())
            {
              gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_GMM_ATTACHED,FALSE,0);

              /* TS 4.60 5.5.1.5 Discontinuous reception (DRX).*/
              glif_exit_mm_non_drx_mode();
            }

            if (pmm_mode == PMM_DETACHED)
            {
              pmm_mode = PMM_IDLE;
              MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_IDLE");
              if ( (is_umts_mode()) && (gmm_t3312_timer_value != 0) )
              {
                mm_start_timer( TIMER_T3312, gmm_t3312_timer_value );
              }

              mm_stop_timer( TIMER_T3319 );
/*24.008:4.7.7.2 
The RAND and RES values stored in the mobile station shall be deleted and timer T3316, if running, shall be stopped:
-   if the mobile station enters PMM-IDLE mode (Iu mode only).*/
#ifdef FEATURE_NAS_REL11
              if (is_umts_mode())
              {
                auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID) ;
              }
#endif
            }
            break;

          case GMM_ROUTING_AREA_UPDATING_INITIATED:
            if ((gmm_substate == GMM_NORMAL_SERVICE) ||
                (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM))
            {
              if (is_gsm_mode())
              {
                gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_RAU_FINISHED,FALSE,0) ;
              }
              /*
                4.7.2.2            Periodic routing area updating 
                The MS shall initiate the routeing area updating procedure and stop the timer T3323 when the MS enters the state GMM-REGISTERED.NORMAL-SERVICE before timer T3323 expires.  
              */
              if (gmm_substate == GMM_NORMAL_SERVICE)
              {
                mm_stop_timer(TIMER_T3323);
              }
              if (gmm_rai_changed(glif_ra_changed_rau_completed_ra.location_area_code,
                                  glif_ra_changed_rau_completed_ra.routing_area_code,
                                  mm_serving_plmn.lac, mm_serving_plmn.rac ) ||
                  gmm_plmn_changed(glif_ra_changed_rau_completed_ra.plmn_id,
                                   mm_serving_plmn.info.plmn))
              {
                /* Store RAI where current PTMSI has become valid */
                memscpy((void *)&glif_ra_changed_rau_completed_ra, sizeof(routing_area_id_T),
                           (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;
              }
            }

            if  ((mm_held_cnm_service_req.present == TRUE) &&
                 (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) 
#if defined(FEATURE_LTE)
                 && ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                  (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
#endif
                 )
            {
               if (is_gsm_mode() || (is_umts_mode() &&
                    ((rrc_connection_status == MMRRC_IDLE)
#ifdef FEATURE_REL6_PS_FOP
                             ||
                      (gmm_ps_fop_resp == PS_FOP_NW_GRANTED)
#endif
                     )
                   )
                 )
               {
                 if (((mm_location_information.location_update_status == UPDATED )&&
                      (memcmp( mm_serving_lai,mm_location_information.lai, LAI_SIZE ) == 0 ))
#ifdef FEATURE_NO_EMCALL_UNTIL_COMBINED_REG_SUCCESS
                            ||
                      (mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL)
#endif
                    )
                 {
                    mm_check_pending_requests();
                 }
                 else
                 {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
                 }
               }
               else
               {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                 mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
               }
            }
            if (is_gsm_mode())
            {
              /* TS 4.60 5.5.1.5 Discontinuous reception (DRX).*/
              glif_exit_mm_non_drx_mode();
            }
            break;

          case GMM_SERVICE_REQUEST_INITIATED:
#if defined(FEATURE_LTE)
           /*
           ** If Timer T3412 expired while GMM State is SERVICE REQUEST INITIATED
           ** then we need to start RAU after moving to REGISTERED state. 
           */
           if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
              (gmm_ra_update_pending == TRUE) &&
              (gmm_ra_update_type == GMM_PERIODIC_UPDATING) &&
              (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
           {
             gmm_pending_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
             gmm_initiate_routing_area_update();
           }
#endif
            break;

          default:
            break;
        }
        break;

      case GMM_NULL :
        /*Cancel the authentication request while moving to
          GMM_NULL state
        */
        if(!is_gsm_mode())
        {
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_GMM_STABILIZED,FALSE,0);
        }
        gmm_cancel_authentication();
        gmmllc_gmm_trigger_llc_event( GMM_TRIGGER_TLLI_UNASSIGNMENT, FALSE, 0 );
#ifdef FEATURE_NAS_FREE_FLOAT           
        gmmllc_reset_llc_conn_state();
#endif
        
#ifdef FEATURE_DUAL_SIM
        if(previous_state == GMM_REGISTERED_INITIATED)
        {
          gmm_power_on_attach_completed = TRUE;
        }
#endif
        if (previous_state == GMM_DEREGISTERED)
        {
          break;
        }
        /* fall through to handle CS only requests */

      case GMM_DEREGISTERED :
        /*Cancel the authentication request while moving to
          GMM_DEREGISTERED state
        */
        if(!is_gsm_mode())
        {
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_GMM_STABILIZED,FALSE,0);
        }
        gmm_cancel_authentication();
        if (previous_state != GMM_NULL)
        {
          if (is_gsm_mode())
          {
            gmmllc_gmm_trigger_llc_event( GMM_TRIGGER_GMM_DETACHED, FALSE, 0 );
          }
          else if (is_umts_mode() || is_lte_mode())
          {
            gmmllc_gmm_trigger_llc_event( GMM_TRIGGER_TLLI_UNASSIGNMENT, FALSE, 0 );
#ifdef FEATURE_NAS_FREE_FLOAT           
            gmmllc_reset_llc_conn_state();
#endif
          }
          if ((mm_held_cnm_service_req.present == TRUE) &&
               ((previous_state == GMM_REGISTERED_INITIATED) ||
                (previous_state == GMM_ROUTING_AREA_UPDATING_INITIATED 
#if defined(FEATURE_LTE)
                 && ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                    (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
#endif
                ) ) &&
               (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
            )
          {
            if (is_gsm_mode() ||
                (is_umts_mode() && (rrc_connection_status == MMRRC_IDLE) ))
            {
              if ((( mm_location_information.location_update_status == UPDATED )&&
                     ( memcmp( mm_serving_lai,mm_location_information.lai, LAI_SIZE ) == 0 ))
#ifdef FEATURE_NO_EMCALL_UNTIL_COMBINED_REG_SUCCESS
                            ||
                    (mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL)
#endif
                 )
              {
                mm_check_pending_requests();
              }
              else
              {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
              }
            }
            else
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
              mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
              mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif                   
            }
          }   
#ifdef FEATURE_LTE
          /* If GMM state is changed to Deregistered and there is a pending ps detach confirmation
          ** then send the confirmation.
          */
          if((mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF ||
              mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING))
          {
            if(!(is_lte_mode() || is_umts_mode()))
            {
              mm_stop_timer(TIMER_MM_PS_DETACH);
              gmm_gprs_attach_pending = FALSE;
#ifdef FEATURE_NAS_FREE_FLOAT
              if(mm_timer_status[TIMER_NW_INIT_DETACH] == TIMER_ACTIVE) 
              {
                MSG_HIGH_DS_0( MM_SUB, "Pend PS detach cnf in case of detach accept");
              }
              else
#endif      
              {
                mm_send_mmr_ps_detach_cnf();
                mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
#ifdef FEATURE_MODEM_HEAP
                if(mm_pending_mmr_ps_detach_req_ptr != NULL)
                {
                  modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
                  mm_pending_mmr_ps_detach_req_ptr = NULL;
                }
#else
                memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
              }
			}
            else if(is_umts_mode())
            {
              if(mm_timer_status[TIMER_MM_PS_DETACH] == TIMER_ACTIVE && MMCNM_IS_MODE_DUAL_STANDBY &&
                  (ps_session_status == MMRRC_ACTIVE)
#ifdef FEATURE_WRLF_SYSTEM_SEL
                   && (mm_wcdma_rlf_state == WCDMA_RLF_IDLE)
#endif				  
				)
              {
                if(mm_timer_status[TIMER_DETACH_REL] == TIMER_ACTIVE)
                {
                  MSG_HIGH_DS_0(MM_SUB,"=MM= timer detach rel already running so no need to start explicitely");
                }
                else
                {
                  mm_nreg_release_pending = TRUE;
                  mm_start_timer(TIMER_DETACH_REL, 3000);
                }
              }
              else
              {
                gmm_gprs_attach_pending = FALSE;
                mm_send_mmr_ps_detach_cnf();
                mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
                if(mm_nreg_release_pending)
                {
                  mm_stop_timer(TIMER_DETACH_REL);
                  mm_nreg_release_pending = FALSE;
                }
#ifdef FEATURE_MODEM_HEAP
                if(mm_pending_mmr_ps_detach_req_ptr != NULL)
                {
                  modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
                  mm_pending_mmr_ps_detach_req_ptr = NULL;
                }
#else
                memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
			  }
            }
          }
#endif
        }
#ifdef FEATURE_GSM_GPRS
        gmm_rau_is_required = FALSE ;
#endif /*FEATURE_GSM_GPRS*/
        gmm_reg_report_required = FALSE ;
        gmm_ps_data_available = FALSE ;
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)        
        gmm_rat_chng_pended_msg_valid = FALSE ;
#ifdef FEATURE_MODEM_HEAP
        if(gmm_rat_chng_pended_msg != NULL)
        {
          modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
          gmm_rat_chng_pended_msg = NULL;
        }
#endif
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
        mm_serving_plmn.info.ps_data_suspend = FALSE;
        gmm_ra_update_pending = FALSE;
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3
        if ((!mm_reg_waiting_for_reg_cnf) &&
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3))
        {
          mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
          mmr_service_ind.service_state = mm_serving_plmn.info;
          mm_send_mmr_service_ind( &mmr_service_ind );
        }
#endif
        switch (previous_state)
        {
          case GMM_REGISTERED_INITIATED:
          case GMM_ROUTING_AREA_UPDATING_INITIATED:
          case GMM_DEREGISTERED_INITIATED:
            if (is_gsm_mode())
            {
              /* TS 4.60 5.5.1.5 Discontinuous reception (DRX).*/
              glif_exit_mm_non_drx_mode();
            }
            if(previous_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
            {
              gmm_follow_on_request = FALSE;
            }
#ifdef FEATURE_DUAL_SIM
            if(previous_state == GMM_REGISTERED_INITIATED)
            {
              gmm_power_on_attach_completed = TRUE;
            }
#endif
            break;

          default:
            break;
        }

        /*
           GPRS Service getting deactivated. Stop the timers T3311, T3312 & T3302
        */

        if ((previous_state == GMM_DEREGISTERED_INITIATED) ||
            (previous_state == GMM_REGISTERED) ||
            (previous_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
            (previous_state == GMM_SERVICE_REQUEST_INITIATED))
        {
          mm_stop_timer( TIMER_T3311 );
          mm_stop_timer( TIMER_T3312 );
          mm_stop_timer( TIMER_T3302 );
          mm_stop_timer( TIMER_T3330 );
#ifdef FEATURE_DUAL_SIM
          mm_stop_timer( TIMER_GMM_RADIO_RETRY );
#endif
#if defined(FEATURE_LTE)
          mm_stop_timer(TIMER_T3323);
#endif
#ifdef FEATURE_DUAL_SIM
          if(MM_MODEM_STATISTICS_SUB1)
#endif
          {
            mm_set_signal_for_cm(SYS_NAS_REGISTRATION_INFO_MASK);
          }
#if defined(FEATURE_LTE)
          if( emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
          {
            gmm_send_sm_isr_status_ind(FALSE);
          }
#endif
        }

        break;
#ifdef FEATURE_GSM_GPRS
      case GMM_ROUTING_AREA_UPDATING_INITIATED:
        if (is_gsm_mode())
        {
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_RAU_STARTED,FALSE,0) ;
        }

        if (gmm_rau_is_required)
        {
          gmm_reg_report_required = TRUE ;

          gmm_rau_is_required = FALSE ;
        }
        break;
#endif /*FEATURE_GSM_GPRS*/

      default:
        break;
    }

    if (
#ifdef FEATURE_LTE 
        !is_lte_mode() &&
#endif
        ((new_state == GMM_REGISTERED) ||
         (new_state == GMM_DEREGISTERED) ||
         (new_state == GMM_NULL)))
    {
      if ((mm_psm_ready_req_rejected == TRUE) &&
          (mm_check_ready_for_psm() == SYS_PSM_STATUS_READY))
      {
        mm_send_mmr_psm_ready_ind();
      }
    }
  }
#endif

  if ( (previous_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
       (new_state      == GMM_ROUTING_AREA_UPDATING_INITIATED)    )
  {
    /* Routing Area Updating has been re-initiated, likely due to a
       reselection into different RAI following processing of an
       RR_SERVICE_IND message. Ensure the GMM-LLC interface module
       is notified to resequence lower layers, and reassign TLLI
       appropriately for this RAI. */
#ifdef FEATURE_GSM_GPRS
    if ( is_gsm_mode() )
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM=  RAU Restart - Resequence Lower Layers");
      gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_RAU_STARTED,FALSE,0) ;
    }
#endif /*FEATURE_GSM_GPRS*/
  }
  else if ((previous_state == GMM_REGISTERED_INITIATED) &&
           (new_state      == GMM_REGISTERED_INITIATED))
  {
    /* ---------------------------------------------------------------
    ** GPRS Attach procedure has been re-initiated. Ensure the GMM-LLC
    ** interface module is notified to resequence lower layers.
    ** --------------------------------------------------------------- */
#ifdef FEATURE_GSM_GPRS
    if (is_gsm_mode())
    {
      gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_GMM_ATTACH_STARTED,FALSE,0);
    }
#endif /*FEATURE_GSM_GPRS*/
  }

  /* Send the log packet which updates gmm_state */
  mmsend_gmm_state_log_packet();

  if (previous_state == GMM_DEREGISTERED_INITIATED)
  {
    /* ------------------------------------------------------------------------
    ** Now that we are transitioning out of this state forward the STOP_MODE or
    ** REG_REQ primitive to MM to complete the deregistration procedure.
    ** ------------------------------------------------------------------------ */
    if (mmcoord_pending_reg_message.cmd.hdr.message_set != 0)
    {
      if ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_STOP_MODE_REQ
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3
           ||
          ((mm_reg_waiting_for_reg_cnf) &&
           (is_gsm_mode()) && (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)) ||
          ((is_gsm_mode()) && (mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_3))
#endif
                  )
                  && (mm_rat_handler_status != MM_RAT_STOP_TDSCDMA_IN_PROGRESS) && (mm_rat_handler_status != MM_RAT_STOP_WCDMA_IN_PROGRESS) &&
                     (mm_rat_handler_status != MM_RAT_STOP_GSM_IN_PROGRESS)
                 )
      {
        /* ---------------------------------------------------------------------
        ** Set ATT flag to FALSE so we don't attempt to perform multiple DETACHs
        ** --------------------------------------------------------------------- */
        mm_stop_mode_req_pended = FALSE;
        mm_system_information.ATT = FALSE;          
        mm_handle_message( &mmcoord_pending_reg_message );
      }
    }
    else
    {
      MSG_FATAL_DS( MM_SUB, "=MM= GMM State Machine Error!", 0,0,0 );
    }
  }
#ifdef FEATURE_DUAL_SIM
   if((gmm_force_local_detach_sim[mm_sub_id]) &&
      ((gmm_state == GMM_NULL) ||
       (gmm_state == GMM_DEREGISTERED)) &&	   
       (mm_serving_plmn.info.active_rat != SYS_RAT_LTE_RADIO_ACCESS))
   {
     sys_modem_as_id_e_type       mm_other_as_id, mm_as_id_local ;

     gmm_force_local_detach_sim[mm_sub_id] = FALSE;

     for(mm_other_as_id = SYS_MODEM_AS_ID_1; mm_other_as_id < MAX_NAS_STACKS ; mm_other_as_id++)
     {
       if (mmcoord_reg_req_pending_sim[mm_other_as_id] == FALSE)
       {
         MSG_ERROR_DS_1(MM_SUB, "=MM= DSDS: MMR_REG_REQ not pended on ds%d for Detach completion",
         mm_other_as_id+1);
       }
       else
       {
         mm_as_id_local = (sys_modem_as_id_e_type)mm_as_id;
         mm_as_id = (mm_as_id_e_type)mm_other_as_id;
         mm_sub_id = mm_sub_id_stack[mm_as_id];
         mmcoord_reg_req_pending = FALSE;
         if (mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_REG_REQ)
         {
             MSG_HIGH_DS_1( MM_SUB, "=MM= DSDS: Processing pending service request as ds%d is now detached for PS", 
                          mm_as_id_local+1);
             mmcoord_process_mmr_reg_req( &mmcoord_pending_reg_message );
         }
         mm_as_id = (mm_as_id_e_type)mm_as_id_local;
         mm_sub_id = mm_sub_id_stack[mm_as_id];
       }
     }
   }
#endif  
} /* end gmm_set_state() */


/*===========================================================================

FUNCTION GMM_SET_SUBSTATE

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_set_substate( gmm_substate_type new_substate )
{
  mmr_service_ind_s_type mmr_service_ind;
  gmm_substate_type      old_substate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif
  if ((gmm_state == GMM_NULL) && (new_substate != GMM_NULL_SUBSTATE))
  {
    return;
  }

  MSG_HIGH_DS_2( MM_SUB, "=MM= GMM substate change from %d to %d", gmm_substate, new_substate );

  old_substate = gmm_substate;
  gmm_substate = new_substate;

  /* ---------------------------------------------
  ** Actions to take on exit from current substate
  ** --------------------------------------------- */
  if (old_substate == GMM_ATTEMPTING_TO_ATTACH)
  {
    gprs_attach_upon_entry_to_a_new_cell = FALSE;
  }

  /* ------------------------------------------
  ** Actions to take on entry into new substate
  ** ------------------------------------------ */
  switch (new_substate)
  {
    case GMM_NORMAL_SERVICE:
      if (gmm_state == GMM_DEREGISTERED)
      {
        /* ------------------------------------------------------------------------
        ** Valid subscriber data is available, the GPRS update status is GU1 or
        ** GU2, a cell has been selected.  In this state, a request for GPRS attach
        ** is performed using the stored temporary mobile subscriber identity for
        ** GPRS (P-TMSI), routing area identification (RAI) and GPRS ciphering key
        ** sequence number in case of GU1.  If the GPRS update status is GU2, the
        ** IMSI shall be used to attach for GPRS services.  (TS 24.008 4.1.3.1.2.1)
        ** ------------------------------------------------------------------------ */
        if ((gmm_update_status == GMM_GU3_PLMN_NOT_ALLOWED) ||
            (gmm_update_status == GMM_GU3_ROUTING_AREA_NOT_ALLOWED))
        {
          new_substate = GMM_LIMITED_SERVICE;
        }
#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac))
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= RAI is in foreground search LAI list" );
              
          gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH);
              
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_NO_SRV,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
          if (mm_reg_waiting_for_reg_cnf)
          {
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();
            
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
          else
          {
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmr_service_ind.service_state = mm_serving_plmn.info;
            mm_send_mmr_service_ind( &mmr_service_ind );
          }
        }
        else
#endif
/*  24.008: 4.7.3.1.5 j)Timer T3346 is running
    The MS shall not start the GPRS attach procedure unless the MS needs to attach for emergency bearer services or the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
        if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
           (gmm_is_plmn_equivalent_t3346()) &&
           (
/*4.7.5.1.5 (j)*/
#ifndef FEATURE_NAS_REL11
           (gmmllc_get_glif_state() != GMM_READY_STATE)&&
#endif
            (pmm_mode != PMM_CONNECTED))
#ifdef FEATURE_NAS_REL11
              &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
          )
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Ignore GMM Procedure since T3346 congestion timer is running" );
        }
        else
#endif

        if ((mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf) ||
            (!mm_reg_waiting_for_reg_cnf))
        {
          t3310_timeout_counter = 0;
          MSG_HIGH_DS_1( MM_SUB, "=MM= T3310 timeout counter = %d", t3310_timeout_counter );
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3         
          if(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
          {
            gmm_previous_network_op_mode = NETWORK_OP_MODE_3;
          }
#endif        
          gmm_initiate_gprs_attach();
          if ((mm_reg_waiting_for_reg_cnf && is_gsm_mode()) &&
              (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
              ||
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
              )&&
              (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
          {
            /* ---------------------------------------------------------------
            ** Immediately send MM registration status to REG and send the GMM
            ** registration status later in a MMR_SERVICE_IND primitive
            ** --------------------------------------------------------------- */
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();

            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
          else if ((mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf) &&
                   (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                   (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY))
          {
            /* ---------------------------------------------------------------
            ** Immediately send MM registration status to REG and send the GMM
            ** registration status later in a MMR_SERVICE_IND primitive
            ** --------------------------------------------------------------- */

            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();

            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
        }
        else if ((mm_reg_waiting_for_reg_cnf) &&
                 (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                  ||
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                ||((mm_state != MM_IDLE) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
                )&&
                 (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
        {
          t3310_timeout_counter = 0;
          MSG_HIGH_DS_1( MM_SUB, "=MM= T3310 timeout counter = %d", t3310_timeout_counter);
          gmm_initiate_gprs_attach();
        }
        else if ((mm_reg_waiting_for_reg_cnf) &&
                 (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
                 (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
        {
          t3310_timeout_counter = 0;
          MSG_HIGH_DS_1( MM_SUB, "=MM= T3310 timeout counter = %d", t3310_timeout_counter );
          gmm_initiate_gprs_attach();
        }
        /* gmm_previous_network_op_mode */
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3
        else if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3) &&
                 (mm_serving_plmn.network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE))
        {
          gmm_previous_network_op_mode = mm_serving_plmn.network_op_mode;

          /*****************************************************************************
          *Reset this flag to false as we are not trying PS registration and if PS fop
          *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
          *of setting PS fop to true
          *****************************************************************************/ 

          gmm_follow_on_request   = FALSE;

        }
#endif
#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
        else if (gmm_force_registration
#ifdef FEATURE_LTE
                  || gmm_is_lte_cap_changed()
#endif
                )
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Attach required for ENS or Mode change purposes-forced to do Attach here");
          gmm_initiate_gprs_attach();
        }
#endif
      }
      else if ((gmm_state == GMM_REGISTERED) ||
               (gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
               ((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
                (old_substate == GMM_PLMN_SEARCH 
      #ifdef FEATURE_WRLF_SYSTEM_SEL
                || mm_wcdma_rlf_state == WCDMA_RLF_SERVICE_CNF_PENDING
      #endif
                )))
      {
        MSG_HIGH_DS_1( MM_SUB, "=MM= RAU needs to be performed, if multislot RA pending is true = %d",gmm_multi_slot_ra_update_pending );
#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac))
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= RAI is in foreground search LAI list");

          gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE);

          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_NO_SRV,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
          if (mm_reg_waiting_for_reg_cnf)
          {
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
          else
          {
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmr_service_ind.service_state = mm_serving_plmn.info;
            mm_send_mmr_service_ind( &mmr_service_ind );
          }
        }
        else
#endif
/*24.008: 4.7.5.1.5 j)  Timer T3346 is running
    The MS shall not start the routing area updating procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
        if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
#ifdef FEATURE_NAS_REL11
           (mm_is_rrc_access_class_11_to_15() == TRUE)&&
#endif
           (gmm_is_plmn_equivalent_t3346()) &&
           (
/*4.7.5.1.5 (j)*/
#ifndef FEATURE_NAS_REL11
           (gmmllc_get_glif_state() != GMM_READY_STATE)&&
#endif
            (pmm_mode != PMM_CONNECTED)))
        {
          if((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
             (gmm_rai_changed( gmm_stored_rai.location_area_code,
                               gmm_stored_rai.routing_area_code,
                               mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
             ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
              (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) != 0 )))
          {
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1) &&
                (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE) &&
                ((mm_timer_status[TIMER_T3246] == TIMER_STOPPED)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                    ||(mm_is_rrc_access_class_11_to_15() == TRUE)
#endif
                 ))
            {
              gmm_change_of_nmo_during_t3346_active();
            }
            else
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Ignore GMM Procedure T3346 running - No NMO Change" );
            }
          }
          else
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Ignore GMM Procedure since T3346 congestion timer is running");
          }
        }
        else
#endif
        if (gmm_periodic_rau_pending)
        {
          /* -------------------------------------------------------------------
          ** If the MS is in other state than GMM-REGISTERED.NORMAL-SERVICE when
          ** the timer expires the periodic routing area updating procedure is
          ** delayed until the MS returns to GMM-REGISTERED.NORMAL-SERVICE
          ** ------------------------------------------------------------------- */

          if ((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
              (gmm_rai_changed( gmm_stored_rai.location_area_code,
                                gmm_stored_rai.routing_area_code, 
                                mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
              ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
               ((memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) != 0 ) ||
                 (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) != 0 ))))
          {
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE))
            {
              gmm_change_of_network_mode_of_operation();
            }
            else
            {
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }

              gmm_initiate_routing_area_update();
            }
          }
          else
          {
            if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS &&
                mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
            }
            else
            {
              gmm_ra_update_type = GMM_PERIODIC_UPDATING;
            }

            gmm_initiate_routing_area_update();
          }

          if (gmm_t3312_timer_value != 0)
          {
            mm_start_timer( TIMER_T3312, gmm_t3312_timer_value );
          }
        }
        else if (mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
        {
          mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);
#ifdef FEATURE_DSAC
          mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
#endif
          if ((mm_reg_waiting_for_reg_cnf) &&
              (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
              ||
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
              )&&
              ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) || 
               (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))&&
              (mm_serving_plmn.info.plmn_service_capability ==
                 SYS_SRV_CAPABILITY_CS_PS_SERVICE))
          {
            /* ---------------------------------------------------------------
            ** Immediately send MM registration status to REG and send the GMM
            ** registration status later in a MMR_SERVICE_IND primitive
            ** --------------------------------------------------------------- */
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();

            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
#ifdef FEATURE_WRLF_SYSTEM_SEL
          /* If GMM in routing are update initiated and UE camps into same RA 
             after RLF then do not start RAU */
          if((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
                  &&  (mm_wcdma_rlf_state == WCDMA_RLF_SERVICE_CNF_PENDING) &&
                  (!((gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                          gmm_previous_registration_serving_rai.routing_area_code,
                          mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                 (gmm_plmn_changed(gmm_previous_registration_serving_rai.plmn_id,
                            mm_serving_plmn.info.plmn)))))
          {     
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();

            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
#ifdef FEATURE_GSM_GPRS
            gmm_rau_is_required = FALSE;
#endif /*FEATURE_GSM_GPRS*/
          } 
          else
#endif          
          if ((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
              (gmm_rai_changed( gmm_stored_rai.location_area_code,
                                gmm_stored_rai.routing_area_code,
                                mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
              ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
               (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE) &&         
               ((memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) != 0 ) ||
                 (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) != 0 )))
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                  ||(gmm_is_prev_mode_connected && is_umts_mode())
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
#if defined(FEATURE_LTE)
                  /*
                  ** SPEC 24.008 rel 8:
                  ** 4.7.5.2.1 Combined routing area updating procedure initiation
                  */
                  ||(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) ||
                         (((gmm_lte_deact_isr_timer_expired == TRUE )||
                           (mm_timer_status[TIMER_T3423] == TIMER_ACTIVE &&
                             emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI))
                        &&(mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                        && (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))) &&
                       mm_timer_status[TIMER_T3311] == TIMER_STOPPED &&
                      mm_timer_status[TIMER_T3302] == TIMER_STOPPED)
#endif
                 || ((is_gsm_mode()) && (gmm_multi_slot_ra_update_pending == TRUE )))
          {
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE))
            {
              gmm_change_of_network_mode_of_operation();
            }
            else
            {
             if ((mm_state != MM_IDLE ) &&
                 ((gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                  (gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)) &&
                 (gmm_state == GMM_REGISTERED) &&
                 (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                       gmm_stored_rai.routing_area_code,
                                       mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                    (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                 (gmm_update_status == GMM_GU1_UPDATED) 
#ifdef FEATURE_LTE 
               &&(emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
#endif 
               )
              {
                 MSG_HIGH_DS_2(MM_SUB,"=MM= RAU should not be initiated since UE is already registered, gmm_pending_ra_update_type %d, gmm_update_status %d,,,", 
                            gmm_pending_ra_update_type,gmm_update_status);
              }
              else
              {
                if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                    (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
                {
                  gmm_update_ra_type();
                }
                else
                {
                  gmm_ra_update_type = GMM_RA_UPDATING;
                }
                gmm_initiate_routing_area_update();
              }
            }
          }
          else if ((gmm_update_status == GMM_GU1_UPDATED) &&
                   (gmm_ra_update_pending == FALSE))
          {
#ifdef FEATURE_DSAC
            if ((mm_last_procedure_to_update_registration != MM_COMBINED) &&
                (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE) &&
                (gmm_check_if_ps_cell_access_allowed(ps_cell_access) == TRUE) &&
                (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
              gmm_initiate_routing_area_update();
            }
            else
#endif
            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
                (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE))
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
              gmm_initiate_routing_area_update();
            }
            else if (old_substate == GMM_IMSI_DETACH_INITIATED)
            {
              if ((mm_serv_change_pending) && (is_gsm_mode()))
              {
                mm_change_service_domain();
                mm_serv_change_pending = FALSE;
              }
            }
            else if ((gmm_processing_directed_signalling_reestablishment) &&
                     (mm_timer_status[TIMER_T3311] == TIMER_STOPPED ||
                      mm_timer_status[TIMER_T3302] == TIMER_STOPPED))
            {
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }
              gmm_initiate_routing_area_update();
            }
#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
            else if (gmm_force_registration
#ifdef FEATURE_LTE
                     || gmm_is_lte_cap_changed()
#endif
/*24.008: 4.7.5.1
The normal routing area updating procedure is initiated: in A/Gb mode, after Inter RAT handover from S1 mode or Iu mode.*/
#ifdef FEATURE_NAS_REL11
                     || (gmm_is_lw_to_g_interrat_in_progress == TRUE)
#endif
                     )
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= RAU required for ENS or Mode change purposes-forced to do RAU here");
              gmm_update_ra_type();
              gmm_initiate_routing_area_update();
            }
#endif
#ifdef FEATURE_LTE
            else if (((gmm_set_drx_req_pending == TRUE) ||
                        (emm_lte_cs_domain_param_match(
                          mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param,
                          gmm_reg_lte_cs_param) == FALSE)) && 
                       (mm_timer_status[TIMER_T3311] == TIMER_STOPPED) &&
                       (mm_timer_status[TIMER_T3302] == TIMER_STOPPED))
            {           
              MSG_HIGH_DS_0(MM_SUB,"=MM= RAU required for change in drx or lte_cs_domain_param");
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }
              gmm_initiate_routing_area_update();
            }
#endif
#ifdef FEATURE_FEMTO_CSG
            else if (!mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info) &&
                         mm_timer_status[TIMER_T3311] == TIMER_STOPPED &&
                         mm_timer_status[TIMER_T3302] == TIMER_STOPPED)
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: RAU required as UE camped on user selected CSG not in white list");
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }
              gmm_initiate_routing_area_update();
            } 
#endif 
            else if ((pmm_mode == PMM_IDLE) || (is_gsm_mode())
#ifdef FEATURE_WRLF_SYSTEM_SEL
                       ||(mm_wcdma_rlf_state == WCDMA_RLF_SERVICE_CNF_PENDING)
#endif
                       )
            {
              gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               mm_serving_plmn.info.reg_service_domain,
                                               FALSE,
                                               SYS_SRV_STATUS_SRV,
                                               TRUE );
              
              if(mm_managed_roaming_enabled &&
                 (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY ||
                  mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
              {
                mm_serving_plmn.info.cs_service_status  = SYS_SRV_STATUS_SRV;  
              }
#ifdef FEATURE_ENHANCED_NW_SELECTION
              if(reg_sim_read_ens_flag() &&
                 (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY ||
                  mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
              {
                mm_serving_plmn.info.cs_service_status  = SYS_SRV_STATUS_SRV;  
              }
#endif

              /*****************************************************************************
              *Reset this flag to false as we are not trying PS registration and if PS fop
              *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
              *of setting PS fop to true
              *****************************************************************************/ 

              gmm_follow_on_request   = FALSE;

              /*Since responding to REG without Attach, set EPLMN list to valid again */
              if(sys_eplmn_list_validate(TRUE))
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= EPLMN list is now validated");
                mm_send_rrc_eq_plmn_list_change_ind();
                mm_send_rr_eq_plmn_list_change_ind(); 
#ifdef FEATURE_LTE
                 emm_send_multimode_rrc_system_update_from_GW(emm_ctrl_data_ptr);
#endif              
              }
              
              if (mm_reg_waiting_for_reg_cnf)
              {
                if (mm_ready_to_send_reg_cnf)
                {
                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();

                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
                }
              }
              else
              {
                mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                mmr_service_ind.service_state = mm_serving_plmn.info;
                mm_send_mmr_service_ind( &mmr_service_ind );
              }
            }
          }
          else
          {
            if ((mm_state != MM_IDLE ) &&
               ((gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                (gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)) &&
                (gmm_state == GMM_REGISTERED) &&
             (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                 gmm_stored_rai.routing_area_code,
                 mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                (gmm_update_status == GMM_GU1_UPDATED) 
#ifdef FEATURE_LTE 
               && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
#endif 
               )
            {
              MSG_HIGH_DS_2(MM_SUB,"=MM= RAU should not be initiated since UE is already in registered, gmm_pending_ra_update_type %d, gmm_update_status %d,,,", 
                          gmm_pending_ra_update_type,gmm_update_status);
            }
            else
            {
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }
              
              gmm_initiate_routing_area_update();
            }
          }
        }
        else if ((old_substate == GMM_NO_CELL_AVAILABLE) ||
                 (old_substate == GMM_PLMN_SEARCH) ||
                 (gmm_follow_on_request)
#ifdef FEATURE_WRLF_SYSTEM_SEL
                 ||(mm_wcdma_rlf_state == WCDMA_RLF_SERVICE_CNF_PENDING)
#endif
                 )
        {
          /* --------------------------
          ** Resumption of GPRS service
          ** -------------------------- */
          mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);

          if ((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
              ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
               (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) != 0 ))||
              ((is_gsm_mode())&& (gmm_multi_slot_ra_update_pending == TRUE))  
             )
          {
            if ((mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf) ||
                (!mm_reg_waiting_for_reg_cnf))
            {
              if ((mm_reg_waiting_for_reg_cnf) &&
                  (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                   ||
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                  )&&
                  (mm_serving_plmn.info.plmn_service_capability ==
                     SYS_SRV_CAPABILITY_CS_PS_SERVICE))
              {
                /* ---------------------------------------------------------------
                ** Immediately send MM registration status to REG and send the GMM
                ** registration status later in a MMR_SERVICE_IND primitive
                ** --------------------------------------------------------------- */
                mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                mm_send_mmr_reg_cnf();

                mm_reg_waiting_for_reg_cnf = FALSE;
                mm_ready_to_send_reg_cnf   = FALSE;
              }
            }
            
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE))
            {
              gmm_change_of_network_mode_of_operation();
            }
            else
            {
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }

              gmm_initiate_routing_area_update();
            }
                       
          }
          else if ((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                    gmm_stored_rai.routing_area_code,
                                    mm_serving_plmn.lac, mm_serving_plmn.rac ))
#if defined(FEATURE_LTE)
                      || (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
#endif
/*24.008: 4.7.5.1
The normal routing area updating procedure is initiated: in A/Gb mode, after Inter RAT handover from S1 mode or Iu mode.*/
#ifdef FEATURE_NAS_REL11
                      || (gmm_is_lw_to_g_interrat_in_progress == TRUE)
#endif
                  )
          {
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE))
            {
              gmm_change_of_network_mode_of_operation();
            }
            else
            {
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)&&
                  (mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_CS_PS_SERVICE))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }
/*Reset RAU initiation flag*/
#ifdef FEATURE_NAS_REL11
              gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif
              gmm_initiate_routing_area_update(); 
            }
          }
          else if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                    (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) != 0 ))
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= RAU required due to LA change");
            if(mm_state != MM_IDLE)
            {
              gmm_ra_update_pending = TRUE;
              gmm_pending_ra_update_type = GMM_COMBINED_RA_LA_UPDATING; 
            }
            else
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
              gmm_initiate_routing_area_update();
            }
          }
          else if ((gmm_update_status != GMM_GU1_UPDATED) || 
                   (gmm_ra_update_pending == TRUE) ||
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                   (gmm_is_prev_mode_connected && is_umts_mode())||
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
/*4.7.1.7a  Intersystem change from S1 mode to A/Gb mode or S1 mode to Iu mode with ISR activated*/
#ifdef FEATURE_NAS_REL11
                   (gmm_is_prev_mode_connected &&(mm_service_loss_on_multimode_rat == SYS_RAT_LTE_RADIO_ACCESS) 
                      && emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI
					  && mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)||
#endif
                   ((gmm_processing_directed_signalling_reestablishment) &&
                    (mm_timer_status[TIMER_T3311] == TIMER_STOPPED ||
                     mm_timer_status[TIMER_T3302] == TIMER_STOPPED)))
          {
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
            }
            else
            {
              gmm_ra_update_type = GMM_RA_UPDATING;
            }

            gmm_initiate_routing_area_update();
          }
#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
          else if (gmm_force_registration
#ifdef FEATURE_LTE
                   || gmm_is_lte_cap_changed()
#endif
/*24.008: 4.7.5.1
The normal routing area updating procedure is initiated: in A/Gb mode, after Inter RAT handover from S1 mode or Iu mode.*/
#ifdef FEATURE_NAS_REL11
                   || (gmm_is_lw_to_g_interrat_in_progress == TRUE)
#endif
                  )
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= RAU required for ENS or Mode change purposes-forced to do RAU here");
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
            }
            else
            {
              gmm_ra_update_type = GMM_RA_UPDATING;
            }
            gmm_initiate_routing_area_update();
          }
#endif
#ifdef FEATURE_LTE
          else if (((gmm_set_drx_req_pending == TRUE) ||
                     (emm_lte_cs_domain_param_match(
                        mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param,
                        gmm_reg_lte_cs_param) == FALSE)) && 
                     (mm_timer_status[TIMER_T3311] == TIMER_STOPPED) &&
                     (mm_timer_status[TIMER_T3302] == TIMER_STOPPED))
          {           
            MSG_HIGH_DS_0(MM_SUB,"=MM= RAU required for change in drx or lte_cs_domain_param");
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
            }
            else
            {
              gmm_ra_update_type = GMM_RA_UPDATING;
            }
            gmm_initiate_routing_area_update();
          }
#endif /*FEATURE_LTE*/
#ifdef FEATURE_FEMTO_CSG
          else if (!mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info) &&
                       mm_timer_status[TIMER_T3311] == TIMER_STOPPED &&
                       mm_timer_status[TIMER_T3302] == TIMER_STOPPED)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: RAU required UE camped on user selected CSG not in white list");
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
            }
            else
            {
              gmm_ra_update_type = GMM_RA_UPDATING;
            }
            gmm_initiate_routing_area_update();
          } 
#endif 
          /* PLMN & RA did not change & GMM is UPDATED -> so update service to REG */
          else if((gmm_update_status == GMM_GU1_UPDATED) &&
                  (gmm_check_if_ps_cell_access_allowed(ps_cell_access) == TRUE))
          {
              if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                 (mm_location_information.location_update_status == UPDATED ) &&
                 ((memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 )))
              {
                gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                 SYS_SRV_DOMAIN_CS_PS,
                                                 FALSE,
                                                 SYS_SRV_STATUS_SRV,
                                                 TRUE );
              }
              else
              {
                gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                 SYS_SRV_DOMAIN_PS_ONLY,
                                                 FALSE,
                                                 SYS_SRV_STATUS_SRV,
                                                 TRUE );
              }
            
            
            if (mm_reg_waiting_for_reg_cnf)
            {
              mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
              mm_send_mmr_reg_cnf();
              
              mm_reg_waiting_for_reg_cnf = FALSE;
              mm_ready_to_send_reg_cnf   = FALSE;
            }
          }
        }
#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
        else if (gmm_force_registration  || ((is_gsm_mode())&& (gmm_multi_slot_ra_update_pending == TRUE))
#ifdef FEATURE_LTE
                  || gmm_is_lte_cap_changed()
#endif
/*24.008: 4.7.5.1
The normal routing area updating procedure is initiated: in A/Gb mode, after Inter RAT handover from S1 mode or Iu mode.*/
#ifdef FEATURE_NAS_REL11
                   || (gmm_is_lw_to_g_interrat_in_progress == TRUE)
#endif
                )
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= RAU required for ENS or Mode change purposes-forced to do RAU here");
          if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
          {
            gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
          }
          else
          {
            gmm_ra_update_type = GMM_RA_UPDATING;
          }
          gmm_initiate_routing_area_update();
        }
#endif
#ifdef FEATURE_LTE
        else if (((gmm_set_drx_req_pending == TRUE) ||
                    (emm_lte_cs_domain_param_match( 
                       mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param,
                       gmm_reg_lte_cs_param) == FALSE)) && 
                   (mm_timer_status[TIMER_T3311] == TIMER_STOPPED) &&
                   (mm_timer_status[TIMER_T3302] == TIMER_STOPPED))
        {           
          MSG_HIGH_DS_0(MM_SUB,"=MM= RAU required for change in drx or lte_cs_domain_param");
          if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
          {
            gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
          }
          else
          {
            gmm_ra_update_type = GMM_RA_UPDATING;
          }
          gmm_initiate_routing_area_update();
        }
#endif /*FEATURE_LTE*/
        else if ((old_substate == GMM_ATTEMPTING_TO_ATTACH) ||
                 (old_substate == GMM_LIMITED_SERVICE))
        {
          if (mm_reg_waiting_for_reg_cnf)
          {
            if (mm_ready_to_send_reg_cnf)
            {
              mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
              mm_send_mmr_reg_cnf();

              mm_reg_waiting_for_reg_cnf = FALSE;
              mm_ready_to_send_reg_cnf   = FALSE;
            }
          }
          else
          {
            mmr_service_ind.service_state = mm_serving_plmn.info;
            mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
            mm_send_mmr_service_ind( &mmr_service_ind );
          }
          
          /*****************************************************************************
          *Reset this flag to false as we are not trying PS registration and if PS fop
          *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
          *of setting PS fop to true
          *****************************************************************************/ 
          
          gmm_follow_on_request   = FALSE;

        }
        else if (((mm_reg_waiting_for_reg_cnf && is_umts_mode()) &&
                 (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) &&
                 (mm_serving_plmn.info.plmn_service_capability ==
                   SYS_SRV_CAPABILITY_CS_PS_SERVICE)) || (is_gsm_mode() && (gmm_multi_slot_ra_update_pending == TRUE)))
        {
          
          mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
          
          if (gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn ) ||
              gmm_rai_changed( gmm_stored_rai.location_area_code,
                               gmm_stored_rai.routing_area_code,
                               mm_serving_plmn.lac, mm_serving_plmn.rac)|| (is_gsm_mode() && (gmm_multi_slot_ra_update_pending == TRUE)))
          {
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE))
            {
              gmm_change_of_network_mode_of_operation();
            }
            else
            {
              gmm_ra_update_type = GMM_RA_UPDATING;
              gmm_initiate_routing_area_update();
            }
          }
          else if (gmm_update_status != GMM_GU1_UPDATED)
          {
            gmm_ra_update_type = GMM_RA_UPDATING;
            gmm_initiate_routing_area_update();
          }
          else if (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV)
          {
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
          else if(mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV 
                              && mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE
                              && gmm_check_if_ps_cell_access_allowed(ps_cell_access) == TRUE)
          {
            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           FALSE,
                                           SYS_SRV_STATUS_SRV,
                                           TRUE);

            if (mm_reg_waiting_for_reg_cnf)
            {
              if (mm_ready_to_send_reg_cnf)
              {
                mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                mm_send_mmr_reg_cnf();

                mm_reg_waiting_for_reg_cnf = FALSE;
                mm_ready_to_send_reg_cnf   = FALSE;
              }
            }
            else
            {
              mmr_service_ind.service_state = mm_serving_plmn.info;
              mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
              mm_send_mmr_service_ind( &mmr_service_ind );
            }
          }
        }
      }
      break;

    case GMM_PLMN_SEARCH:
    case GMM_LIMITED_SERVICE:
    case GMM_ATTACH_NEEDED:
    case GMM_ATTEMPTING_TO_ATTACH:
    case GMM_NO_IMSI:
    case GMM_NO_CELL_AVAILABLE:
    case GMM_SUSPENDED:
    case GMM_UPDATE_NEEDED:
    case GMM_ATTEMPTING_TO_UPDATE:
    case GMM_ATTEMPTING_TO_UPDATE_MM:
    case GMM_IMSI_DETACH_INITIATED:
    case GMM_NULL_SUBSTATE:
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Invalid GMM substate %d", new_substate, 0,0 );
  }

#ifdef FEATURE_GSM_GPRS

  if (is_gsm_mode())
  {
    if (gmm_state == GMM_REGISTERED)
    {

      if ((old_substate != GMM_LIMITED_SERVICE) && 
          (new_substate == GMM_LIMITED_SERVICE))
      {

        /* Suspend LLC until GMM stays in LIMITED SERVICE */
        gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_SUSPENSION,FALSE,0) ;

      } else if ((old_substate == GMM_LIMITED_SERVICE) || (mm_llc_needs_to_be_resumed))
      {
        if ((new_substate == GMM_NORMAL_SERVICE) || 
            (new_substate == GMM_ATTEMPTING_TO_UPDATE_MM) ||
            (new_substate == GMM_IMSI_DETACH_INITIATED))
        {
          gmm_resume_llc_ind = FALSE ;

          /* Resume LLC */
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_RESUMPTION,FALSE,0) ;
          mm_llc_needs_to_be_resumed = FALSE;
        } else
        {
          gmm_resume_llc_ind = TRUE ;
        }
      } else if ( gmm_resume_llc_ind && 
                  ((new_substate == GMM_NORMAL_SERVICE) || 
                  (new_substate == GMM_ATTEMPTING_TO_UPDATE_MM) ||
                  (new_substate == GMM_IMSI_DETACH_INITIATED)))
      {
        gmm_resume_llc_ind = FALSE ;

        /* Resume LLC */
        gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_RESUMPTION,FALSE,0) ;
      } else if (old_substate == GMM_IMSI_DETACH_INITIATED)
      {
        if(new_substate == GMM_NORMAL_SERVICE)
        {
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_IMSI_DETACH_FINISHED,FALSE,0) ;        
        } else
        {
          gmm_detach_started = TRUE;        
        }
      
      } else if (gmm_detach_started == TRUE && new_substate == GMM_NORMAL_SERVICE)
      {
        gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_IMSI_DETACH_FINISHED,FALSE,0) ;
        gmm_detach_started = FALSE;      
      }      
    }
  }

#endif /* #ifdef FEATURE_GSM_GPRS */

  /* Send the log packet which updates gmm_substate */
  mmsend_gmm_state_log_packet();

} /* end gmm_set_substate() */


/*===========================================================================

FUNCTION GMM_INITIALIZE

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_initialize( void )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  uint8 index;
#endif 
  MSG_LOW_DS( MM_SUB, "=MM= Calling gmm_initialize()", 0,0,0 );

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  for ( mm_as_id=MM_AS_ID_1; mm_as_id < (mm_as_id_e_type)MAX_NAS_STACKS ; mm_as_id++ )
#endif
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   mm_sub_id = mm_sub_id_stack[mm_as_id];
#endif
  /* ------------------------------------------------------------------------
  ** The GPRS attach attempt counter shall be reset when the MS is powered on
  ** (3GPP TS 24.008 4.7.3)
  ** ------------------------------------------------------------------------ */
  gprs_attach_attempt_counter = 0;
  gmm_controls_resel_dec_mt_ps_call = FALSE;
  gmm_rau_attempt_counter = 0;
  t3330_timeout_counter = 0;
  gmm_irat_lu_pending = NO_LU;
  /* ----------------------------
  ** Default timeout = 54 minutes
  ** ---------------------------- */
  gmm_t3312_timer_value = 3240000;

#if defined(FEATURE_LTE)
  /*The default value of this timer is identical to the
    value of timer T3312.*/
  gmm_t3323_timer_value = 3240000;
#endif

  /* ----------------------------
  ** Default timeout = 12 minutes
  ** ---------------------------- */
  gmm_t3302_timer_value = 720000;
  /* ----------------------------
  ** Default timeout = 30Sec
  ** ---------------------------- */
  gmm_t3319_timer_value = 30000;

#ifdef FEATURE_DUAL_SIM
  gmm_power_on_attach_completed                      = FALSE;
#endif
  gprs_attach_upon_entry_to_a_new_cell               = FALSE;
  gmm_power_down_active                              = FALSE;
  gmm_gprs_attach_pending                            = FALSE;
  gmm_gprs_detach_pending                            = FALSE;
  gmm_ra_update_pending                              = FALSE;
  gmm_multi_slot_ra_update_pending                   = FALSE;
  gmm_follow_on_request                              = FALSE;
  gmm_directed_signalling_reestablishment            = FALSE;
  gmm_detach_after_rau_pending                       = FALSE;
  gmm_processing_directed_signalling_reestablishment = FALSE;
  gmm_is_ptmsi_present_in_prev_gmm_msg               = FALSE;
  gmm_is_tmsi_present_in_prev_gmm_msg                = FALSE;
  gmm_attach_request_on_network_reject               = FALSE;

  /* ---------------------------------------------------------------------------
  ** GPRS capability has not yet been enabled and it has not yet been determined
  ** if the SIM is present (and valid)
  ** --------------------------------------------------------------------------- */
  gmm_set_state( GMM_NULL );
  gmm_set_substate( GMM_NULL_SUBSTATE );

  mm_serving_plmn.ms_op_mode           = SYS_SRV_DOMAIN_NO_SRV;
  mm_serving_plmn.network_op_mode      = PS_DOMAIN_SYS_INFO_UNAVAILABLE;
  mm_serving_plmn.gmm_service_state    = SERVICE_NOT_AVAILABLE;
  pmm_mode                             = PMM_DETACHED;

  gmm_service_request_initiator = NULL;

  gmm_integrity_protection_activated = FALSE;
  gmm_ps_signalling_connected        = FALSE;

  gmm_previous_service_state      = SERVICE_NOT_AVAILABLE;
  gmm_previous_gprs_service_state = SERVICE_NOT_AVAILABLE;
  gmm_previous_network_op_mode    = PS_DOMAIN_SYS_INFO_UNAVAILABLE;
  gmm_previous_T3311_gmm_substate       = GMM_NULL_SUBSTATE;

  gmm_previous_serving_rai.plmn_id.identity[0]       = 0xFF;
  gmm_previous_serving_rai.plmn_id.identity[1]       = 0xFF;
  gmm_previous_serving_rai.plmn_id.identity[2]       = 0xFF;
  gmm_previous_serving_rai.location_area_code.lac[0] = 0xFF;
  gmm_previous_serving_rai.location_area_code.lac[1] = 0xFE;
  gmm_previous_serving_rai.routing_area_code         = 0xFF;

  gmm_previous_registration_serving_rai.plmn_id.identity[0]       = 0xFF;
  gmm_previous_registration_serving_rai.plmn_id.identity[1]       = 0xFF;
  gmm_previous_registration_serving_rai.plmn_id.identity[2]       = 0xFF;
  gmm_previous_registration_serving_rai.location_area_code.lac[0] = 0xFF;
  gmm_previous_registration_serving_rai.location_area_code.lac[1] = 0xFE;
  gmm_previous_registration_serving_rai.routing_area_code         = 0xFF;

  gmm_current_lai.PLMN_id.identity[0] = 0xFF;
  gmm_current_lai.PLMN_id.identity[1] = 0xFF;
  gmm_current_lai.PLMN_id.identity[2] = 0xFF;
  gmm_current_lai.location_area_code[0] = 0xFF;
  gmm_current_lai.location_area_code[1] = 0xFE;
  gmm_current_rac = 0xFF;
  
  gmm_reg_rejected_rai.plmn_id.identity[0]       = 0xFF;
  gmm_reg_rejected_rai.plmn_id.identity[1]       = 0xFF;
  gmm_reg_rejected_rai.plmn_id.identity[2]       = 0xFF;
  gmm_reg_rejected_rai.location_area_code.lac[0] = 0xFF;
  gmm_reg_rejected_rai.location_area_code.lac[1] = 0xFE;
  gmm_reg_rejected_rai.routing_area_code         = 0xFF;

  /* -------------------------------------------------------
  ** Mark cached PTMSI, PTMSI signature and RAI as deleted
  ** ------------------------------------------------------- */
  gmm_stored_rai.plmn_id.identity[0]       = 0xFF;
  gmm_stored_rai.plmn_id.identity[1]       = 0xFF;
  gmm_stored_rai.plmn_id.identity[2]       = 0xFF;
  
#ifdef FEATURE_NAS_REL11
  gmm_previous_plmn_with_t3302_value.identity[0] = 0xFF;
  gmm_previous_plmn_with_t3302_value.identity[1] = 0xFF;
  gmm_previous_plmn_with_t3302_value.identity[2] = 0xFF;
#endif  
  gmm_delete_ps_location_information ();

#ifdef FEATURE_GSM_GPRS
  /* Initialize GMM-LLC interface module */
  gmmllc_init() ;
  gmm_resume_llc_ind = FALSE ;

  MSG_HIGH_DS_2(MM_SUB, "=MM= The glif_force_local_tlli = %d, The address's %X",glif_force_local_tlli, (dword)&glif_force_local_tlli);

#endif /* #ifdef FEATURE_GSM_GPRS */
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  gmm_is_prev_mode_connected = FALSE;
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
  gmm_transaction_id = 0;
#endif

#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
  gmm_force_registration = FALSE;
#endif
#if defined(FEATURE_LTE)
  gmm_lte_deact_isr_timer_expired = FALSE;
#endif

#ifdef FEATURE_NAS_REL10
  gmm_stored_nw_feature_support = 0x00;

  gmm_is_congestion_due_to_rau    = FALSE ;

#endif
#ifdef FEATURE_NAS_REL11
  gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif
  gmm_foreground_search_pending = FALSE;
  gmm_procedure_rerun_ongoing        = FALSE;

  gmm_processed_mt_imsi_detach_request = FALSE;
  gmm_cause_directed_signalling_reest  = FALSE;
  
/* TSTS Variables Initialization  - Start */
  gmm_timer_deactivated = FALSE;
  gmm_ptmsi_sig_tobe_deleted = FALSE ;
  gmm_manual_mode_user_selection_procedure = FALSE;
  gmm_processing_directed_signalling_reestablishment = FALSE;

#ifdef FEATURE_DUAL_SIM
  gmm_force_local_detach_sim[mm_as_id] = FALSE;
#endif

#ifdef FEATURE_REL6_PS_FOP
 /* PS Follow on Proceed flag */
  gmm_ps_fop_req = FALSE;
  gmm_ps_fop_resp = PS_FOP_NW_GRANTED;
#endif/* FEATURE_REL6_PS_FOP */
 
#ifdef FEATURE_GSM_GPRS
  gmm_rau_is_required = FALSE ;
  gmm_reg_report_required = FALSE ;
  gmm_ps_data_available = FALSE ;
#endif /* #ifdef FEATURE_GSM_GPRS */
 
  gmm_is_suspended = FALSE;
  gmm_last_signalling_sent_rat = SYS_RAT_NONE;
 
  gmm_registration_initiated   = FALSE;

#ifdef FEATURE_GPRS_PS_HANDOVER
  gmm_psho_status = GMM_PSHO_NONE;
  gmm_psho_gea_change = FALSE;
#endif
  gmm_is_ps_active = FALSE;
 
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  gmm_is_prev_mode_connected = FALSE;
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
 
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
  gmm_transaction_id = 0;
#endif
 
#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
  gmm_force_registration = FALSE;
#endif 
#ifdef FEATURE_LTE
  gmm_nv_drx_cn_coeff_s1 = SYS_DRX_NOT_SPECIFIED_BY_MS;
  gmm_drx_cn_coeff_s1 = SYS_DRX_NOT_SPECIFIED_BY_MS;
  
  gmm_set_drx_req_pending = FALSE;
  gmm_reg_lte_cs_param.is_voice_domain_ie_valid = FALSE;
  gmm_reg_pended_lte_cs_param.is_voice_domain_ie_valid = FALSE;
  gmm_is_lte_advertised_in_last_reg = FALSE;
#endif /*FEATURE_LTE*/

  gmm_xid_negotiation_completed = FALSE;
  gmm_extend_connection_due_to_mt_csfb = FALSE;
  
 }


  gmm_read_nv_params();
#ifdef FEATURE_GSM_GPRS
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_sub_id = MM_AS_ID_1;
  mm_as_id = MM_AS_ID_1;
  for ( index = 0;  index < mm_sim_max_subscription; index++ )
#endif
  /* Send the GPRS NV parameters to the GPRS lower layers */
  {
    gmm_send_nv_params();
    mm_sub_id++;
    mm_as_id++;
  }
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_as_id=MM_AS_ID_1;
  mm_sub_id = MM_AS_ID_1;
#endif

#ifdef FEATURE_DUAL_SIM
  gmm_cm_ds_tuneaway_status = TRUE;
#endif

  gmm_delete_gprs_forbidden_rai();
} /* end gmm_initialize() */


/*============================================================================
FUNCTION GMM_RESET_GLOBAL_VARIABLES

DESCRIPTION

This function resets gmm global variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void gmm_reset_global_variables(void)
{

  gprs_attach_attempt_counter = 0;

  gmm_controls_resel_dec_mt_ps_call = FALSE;

  gmm_rau_attempt_counter     = 0; 

  t3330_timeout_counter       = 0; 
  gmm_irat_lu_pending = NO_LU;
  /* ----------------------------
  ** Default timeout = 54 minutes
  ** ---------------------------- */
  gmm_t3312_timer_value = 3240000; 

#if defined(FEATURE_LTE)
  /*The default value of this timer is identical to the
  value of timer T3312.*/
  gmm_t3323_timer_value = 3240000;
#endif

  /* ----------------------------
  ** Default timeout = 12 minutes
  ** ---------------------------- */
  gmm_t3302_timer_value = 720000; 
  /* ----------------------------
   ** Default timeout = 30Sec
   ** ---------------------------- */
  gmm_t3319_timer_value = 30000; 

  gprs_attach_upon_entry_to_a_new_cell              = FALSE; 

  gmm_gprs_attach_pending                           = FALSE; 

  gmm_gprs_detach_pending                           = FALSE; 

  gmm_ra_update_pending                             = FALSE; 

  gmm_detach_after_rau_pending                      = FALSE;

  gmm_ps_signalling_connected                       = FALSE; 
  gmm_previous_network_op_mode                      = PS_DOMAIN_SYS_INFO_UNAVAILABLE ;
  gmm_multi_slot_ra_update_pending                  = FALSE; 
  gmm_attach_request_on_network_reject              = FALSE;

#ifdef FEATURE_DUAL_SIM
  gmm_power_on_attach_completed                     = FALSE;
#endif
  gmm_previous_T3311_gmm_substate        = GMM_NULL_SUBSTATE;
  gmm_previous_registration_serving_rai.plmn_id.identity[0]       = 0xFF;
  gmm_previous_registration_serving_rai.plmn_id.identity[1]       = 0xFF;
  gmm_previous_registration_serving_rai.plmn_id.identity[2]       = 0xFF;
  gmm_previous_registration_serving_rai.location_area_code.lac[0] = 0xFF;
  gmm_previous_registration_serving_rai.location_area_code.lac[1] = 0xFE;
  gmm_previous_registration_serving_rai.routing_area_code         = 0xFF;

#ifdef FEATURE_MODEM_HEAP
  if(pending_gmm_msg != NULL)
  {
    modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
    pending_gmm_msg = NULL;
    pending_gmm_msg_valid = FALSE;
  }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)  
  if(gmm_rat_chng_pended_msg != NULL)
  {
    modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
    gmm_rat_chng_pended_msg = NULL;
    gmm_rat_chng_pended_msg_valid = FALSE;
  }
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/  
#endif

  #ifdef FEATURE_GSM_GPRS

  /* Make gmm_detach_in_progress to FALSE 
   * so that NAS doesn't block ATTACH at power ON.
   * If this var is not reset, NAS will pend ATTACH
   * as it will keep on waiting for LLC STATUS IND. 
   * */
  gmm_detach_in_progress = FALSE;

  /* Initialize GMM-LLC interface module */
  gmmllc_gmm_trigger_llc_event( GMM_TRIGGER_TLLI_UNASSIGNMENT, FALSE, 0 );
  gmmllc_init() ; 

  gmm_resume_llc_ind = FALSE ;

  #endif

#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
  gmm_force_registration = FALSE;
#endif
#if defined(FEATURE_LTE)
  gmm_lte_deact_isr_timer_expired = FALSE;
  gmm_reg_lte_cs_param.is_voice_domain_ie_valid = FALSE;
  gmm_reg_pended_lte_cs_param.is_voice_domain_ie_valid = FALSE;
#endif

#ifdef FEATURE_NAS_REL10
  gmm_stored_nw_feature_support   = 0x00;
#endif
#ifdef FEATURE_NAS_REL11
  gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif
  gmm_foreground_search_pending   = FALSE;
#ifdef FEATURE_NAS_REL10
  gmm_is_congestion_due_to_rau    = FALSE ;
#endif
  gmm_registration_initiated      = FALSE;
  gmm_processed_mt_imsi_detach_request = FALSE;

  gmm_cause_directed_signalling_reest  = FALSE;

  gmm_extend_connection_due_to_mt_csfb = FALSE;

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
  gmm_last_signalling_message_type = 0;
#endif
   mm_mmr_cell_service_ind_pending = FALSE;
   sys_plmn_undefine_plmn_id(&mmcoord_mmr_reg_cnf.service_state.plmn);   

   if(gmm_ptmsi_sig_tobe_deleted)
   {
     boolean ptmsi_signature_present = FALSE;
     byte i ;

     /* -------------------------------
     ** Delete the old P-TMSI signature
     ** ------------------------------- */
     for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
     {
       if(gmm_stored_ptmsi_signature.value[i] != 0xFF)
       {
         ptmsi_signature_present = TRUE;
         gmm_stored_ptmsi_signature.value[i] = 0xFF;
       }
     }

     if(ptmsi_signature_present)
     {
       mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                           gmm_stored_ptmsi_signature,
                                           gmm_stored_rai,
                                           gmm_update_status );
     }

     gmm_ptmsi_sig_tobe_deleted = FALSE;
   }

  gmm_power_down_active = FALSE;
  gmm_delete_gprs_forbidden_rai();

#ifdef FEATURE_DUAL_SIM
  gmm_cm_ds_tuneaway_status = TRUE;
#endif

}
/*===========================================================================

FUNCTION GMM_LOAD_PSLOCI_DATA

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_load_psloci_data( void )
{
  MSG_LOW_DS(MM_SUB, "Calling gmm_load_psloci_data()", 0,0,0 );

  /* -----------------------------------------------------------
  ** Retrieve PS location information (TS 31.102 section 4.2.23)
  ** ----------------------------------------------------------- */
  mmsim_get_ps_location_information( &gmm_stored_ptmsi,
                                     &gmm_stored_ptmsi_signature,
                                     &gmm_stored_rai,
                                     &gmm_update_status );

#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE)
  {
    HYBRID_STACK_DATA_MEMS_COPY(&gmm_stored_ptmsi,PTMSI_SIZE,&gmm_stored_ptmsi,PTMSI_SIZE)
    HYBRID_STACK_DATA_MEMS_COPY(&gmm_stored_ptmsi_signature,PTMSI_SIGNATURE_SIZE,&gmm_stored_ptmsi_signature,PTMSI_SIGNATURE_SIZE)
    HYBRID_STACK_DATA_MEMS_COPY(&gmm_stored_rai,RAI_SIZE,&gmm_stored_rai,RAI_SIZE)
  }
#endif

  if(!mm_is_plmn_valid(gmm_stored_rai.plmn_id))
  {
    MSG_HIGH_DS_3(MM_SUB, "=MM= PS LOCI %02x %02x %02x invalid -> mark NOT UPDATED",
                                gmm_stored_rai.plmn_id.identity[0],
                                gmm_stored_rai.plmn_id.identity[1],
                                gmm_stored_rai.plmn_id.identity[2]);

#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      HYBRID_STACK_DATA_COPY(gmm_update_status,GMM_GU2_NOT_UPDATED)
    }
    else
#endif
    {
      gmm_update_status = GMM_GU2_NOT_UPDATED;
    }
    gmm_delete_ps_location_information();
  }

#ifdef FEATURE_GSM_GPRS
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE)
  {
    HYBRID_STACK_DATA_MEMS_COPY(&glif_ra_changed_rau_completed_ra, sizeof(routing_area_id_T),&gmm_stored_rai,sizeof(routing_area_id_T))
  }
  else
#endif
  {
    memscpy((void *)&glif_ra_changed_rau_completed_ra, sizeof(routing_area_id_T),
                              (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;
  }
#endif /* #ifdef FEATURE_GSM_GPRS */

  if ((gmm_update_status == GMM_GU3_PLMN_NOT_ALLOWED) ||
      (gmm_update_status == GMM_GU3_ROUTING_AREA_NOT_ALLOWED))
  {
    /* ---------------------------------------------------------------------------
    ** If the location update status stored on the SIM is different from "updated"
    ** then the mobile shall act as if the location update status stored on the
    ** SIM is "not updated". (TS 24.008 4.2.1.1)
    ** --------------------------------------------------------------------------- */
#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      HYBRID_STACK_DATA_COPY(gmm_update_status,GMM_GU2_NOT_UPDATED)
    }
    else
#endif
    {
      gmm_update_status = GMM_GU2_NOT_UPDATED;
    }

  }
}


/*===========================================================================

FUNCTION GMM_PASS_OTA_MSG_TO_SM

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_pass_ota_msg_to_sm( mm_cmd_type *message )
{
  sm_cmd_type *sm_cmd;
  uint16 msg_length;
  uint16 gmm_max_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }

  sm_cmd->header.message_set = MS_GMM_SM;
  sm_cmd->header.cmd_id      = GMMSM_UNITDATA_IND;
  gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);
  GET_IMH_LEN(msg_length ,&message->cmd.mm_data_ind.message_header);

  sm_cmd->cmd.gmm_unitdata_ind.message_len = MIN(msg_length,gmm_max_length);

#ifndef FEATURE_MODEM_HEAP 
  sm_cmd->cmd.gmm_unitdata_ind.data = gs_alloc(sm_cmd->cmd.gmm_unitdata_ind.message_len);
#else
  sm_cmd->cmd.gmm_unitdata_ind.data = modem_mem_calloc(1,sm_cmd->cmd.gmm_unitdata_ind.message_len,MODEM_MEM_CLIENT_NAS);
#endif
  mm_check_for_null_ptr((void*)sm_cmd->cmd.gmm_unitdata_ind.data);


#ifdef FEATURE_DUAL_SIM
  sm_cmd->cmd.gmm_unitdata_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  /* --------------------
  ** Copy the OTA message
  ** -------------------- */
   (void) memscpy( sm_cmd->cmd.gmm_unitdata_ind.data, gmm_max_length,
                   &message->cmd.mm_data_ind.L3_data[0],
                   msg_length );

  MSG_HIGH_DS_0( MM_SUB, "=MM= Send GMMSM_UNITDATA_IND" );
  sm_put_cmd( sm_cmd );

} /* end gmm_pass_l3_msg_to_sm() */

#ifdef FEATURE_TC
/*===========================================================================

FUNCTION GMM_PASS_OTA_MSG_TO_TC

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_pass_ota_msg_to_tc( mm_cmd_type *message )
{
  tc_cmd_type *tc_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tc_cmd = tc_get_cmd_buf();

  if (tc_cmd == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }

  tc_cmd->header.message_set = MS_GMM_TC;
  tc_cmd->header.cmd_id      = GMMTC_UNITDATA_IND;

  tc_cmd->cmd.gmm_unitdata_ind.message_len =
      message->cmd.mm_data_ind.message_header.message_len_lsb;

  /* --------------------
  ** Copy the OTA message
  ** -------------------- */
  (void) memscpy( tc_cmd->cmd.gmm_unitdata_ind.data, MAX_OCTETS_L3_MESSAGE,
           (void *)&message->cmd.mm_data_ind.L3_data[0],
                 message->cmd.mm_data_ind.message_header.message_len_lsb );

  MSG_HIGH_DS_0( MM_SUB, "=MM= Send GMMTC_UNITDATA_IND");
  tc_put_cmd( tc_cmd );

} /* end gmm_pass_l3_msg_to_tc() */

#endif /* End of ifdef FEATURE_TC */

/*===========================================================================

FUNCTION GMM_PASS_OTA_MSG_TO_CNM

DESCRIPTION
  This function is transfer incoming message to the CNM SMS handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_pass_ota_msg_to_cnm
(
  mm_cmd_type *message
)
{
  mmcnm_data_ind_T incoming_sms ;
  uint16 gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);

  /* Initialize incoming_sms indication header */
  incoming_sms.msg_head.IMH.message_set = MS_CC_MM_PS ;
  incoming_sms.msg_head.IMH.message_id = MMCNM_DATA_IND ;
#ifdef FEATURE_DUAL_SIM
  /* Append AS ID */
  {
    uint2 length;

    GET_IMH_LEN(length, &message->cmd.mm_data_ind.message_header);

    PUT_IMH_LEN(length + sizeof(byte), &incoming_sms.msg_head.IMH);
    incoming_sms.msg_head.ti_pd = message->cmd.mm_data_ind.L3_data[0] ;
    incoming_sms.msg_head.type  = message->cmd.mm_data_ind.L3_data[1] ;
    length -= (2 * sizeof(byte));
  
    /* --------------------
    ** Copy the OTA message
    ** -------------------- */
    (void) memscpy((void *)incoming_sms.data, gmm_max_length+1,
        (void *)(&message->cmd.mm_data_ind.L3_data[2]),
                            length ) ;
    incoming_sms.data[length] = (byte)mm_sub_id;
  }
#else
  PUT_IMH_LEN(message->cmd.mm_data_ind.message_header.message_len_lsb,&incoming_sms.msg_head.IMH);

  incoming_sms.msg_head.ti_pd = message->cmd.mm_data_ind.L3_data[0] ;
  incoming_sms.msg_head.type  = message->cmd.mm_data_ind.L3_data[1] ;

  /* --------------------
  ** Copy the OTA message
  ** -------------------- */
    (void) memscpy((void *)incoming_sms.data, gmm_max_length+1,
        (void *)(&message->cmd.mm_data_ind.L3_data[2]),
                     message->cmd.mm_data_ind.message_header.message_len_lsb - 2) ;   
#endif
  mm_sms_is_active = TRUE;
  mm_stop_timer(TIMER_PS_CONN_REL);
  MSG_HIGH_DS_0( MM_SUB, "=MM= Send MMCNM_DATA_IND (PS SMS)");

  mm_send_message( (IMH_T *)&incoming_sms, GS_QUEUE_MN_CM );
} /* end gmm_pass_ota_msg_to_cnm() */


/*===========================================================================

FUNCTION GMM_HANDLE_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_message( mm_cmd_type *message )
{
  byte protocol_discriminator;

  sm_cmd_type    *sm_cmd = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (message->cmd.hdr.message_set)
  {
    case MS_GMM_SMS:
    case MS_GMM_SM:
    case MS_GMM_RABM:

#ifdef FEATURE_TC
    case MS_GMM_TC:
#endif
      gmm_handle_connection_management( message );
      break;

#ifdef FEATURE_GSM_GPRS
    case MS_MM_RR:
      switch (message->cmd.hdr.message_id)
      {
        case RR_PLMN_SELECT_CNF:
        case RR_SERVICE_IND:
#ifdef FEATURE_GSM_DTM
        case RR_SERVICE_IND_FROM_OTA_MSG:
#endif
        case RR_DEACT_CNF:
          gmm_handle_registration( message );
          break;

        case RR_SYNC_IND:
        case RR_PAGING_IND:
          if ((mm_serving_plmn.info.reg_service_domain != SYS_SRV_DOMAIN_CS_ONLY) &&
              ((gmm_substate != GMM_SUSPENDED) && (!gmm_is_suspended)) &&
#ifdef FEATURE_GSM_DTM
              ((mm_state == MM_IDLE) || (mm_serving_plmn.dtm_supported == TRUE)) && 
#else 
              (mm_state == MM_IDLE) &&
#endif
              (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
          {            
            /* GMM-LLC interface processes GRR indications */
            gmmllc_handle_grr_event( message );
          }
          break;

        case RR_REL_IND:
        case RR_ABORT_IND:
        case RR_ABORT_CNF:
          if ((mm_rat_handler_status == MM_RAT_STOP_GSM_IN_PROGRESS) && mm_nreg_req_received && (message->cmd.hdr.message_id == (int) RR_ABORT_IND)) 
          {
            MSG_HIGH_DS_1( MM_SUB, "=MM= RR_ABORT_IND(msg_id=%d) ignored here as it would delay powering off!!", message->cmd.hdr.message_id);

            if(gmm_gprs_detach_pending)
            {
              gmm_set_state(GMM_DEREGISTERED); /*as detach is being ignored here, this state change is made.*/


              sm_cmd = sm_get_cmd_buf();

              if (sm_cmd == NULL)
              {
                MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
              }

              sm_cmd->header.message_set = MS_GMM_SM;
              sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
              sm_cmd->cmd.gmm_gprs_rel_ind.cause = (cc_cause_T)NO_GPRS_CONTEXT;
#ifdef FEATURE_DUAL_SIM
              sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

              sm_put_cmd( sm_cmd );
            }
          }
#ifdef FEATURE_LTE
          else if(mm_csfb_state == MM_CSFB_STATE_MT_CALL_CCO)
          {
            MSG_HIGH_DS_1( MM_SUB, "=MM= RR_ABORT_IND(msg_id=%d) ignored here as we are making another attempt to send csfb page!!",
                    message->cmd.hdr.message_id );
          }
#endif
          else
          {
            gmm_handle_connection_management( message );
          }
          break;

        case RR_GMM_GPRS_SUSPENSION:
          /* Handle GPRS suspension notification from GRR */
#ifdef FEATURE_NAS_FREE_FLOAT
          gmmllc_reset_service_state();
#endif
          gmm_handle_gprs_suspension();
          break;

        case RR_GMM_GPRS_RESUMPTION_IND:
          /* ------------------------------------------------------------------
          **  Process GPRS resumption info if any, while leaving dedicated mode
          ** ------------------------------------------------------------------*/
          gmm_handle_gprs_resumption( message );
          gmmllc_handle_grr_event( message );
          break;

        case RR_RESELECTION_IND:
#ifndef FEATURE_NAS_FREE_FLOAT
        case RR_GPRS_PCCCH_CAMP_CNF:
#else
        case RR_GMM_GPRS_SERVICE_CNF:
#endif
          gmmllc_handle_grr_event( message );
          break;

        case RR_GMM_GPRS_MULTISLOT_CLASS_IND:
          gmm_handle_multi_slot_class_ind( message );
          break;

#ifdef FEATURE_DUAL_SIM
        case RR_PS_ABORT_CNF:
          gmm_handle_connection_management( message );
          break;
#endif
        case RR_GMM_GPRS_MS_RA_CAP_REQ:
          glif_grr_notify_ms_ra_cap();
          break;

        default:
          MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected RR primitive (id=%d)", message->cmd.hdr.message_id );
          break ;
      }
      break ;
#endif /* #ifdef FEATURE_GSM_GPRS */
    case MS_MM_RRC:
    case MS_MM_TDSRRC:
      switch (message->cmd.hdr.message_id)
      {
        case RRC_SERVICE_CNF:
        case RRC_SERVICE_IND:
        case RRC_SERVICE_IND_FROM_OTA_MSG:
          gmm_handle_registration( message );
          break;

        case RRC_ABORT_IND:
        case RRC_ABORT_CNF:
        case RRC_SYNC_IND:
        case RRC_REL_IND:
        case RRC_OPEN_SESSION_CNF:
        case RRC_CLOSE_SESSION_IND:
        case RRC_CLOSE_SESSION_CNF:
        case RRC_FAST_DORMANCY_CNF:
          gmm_handle_connection_management( message );
          break;

        case RRC_PAGE_IND:
          gmm_handle_connection_management( message );
          break;

        case MM_DATA_IND:
          protocol_discriminator =
           (byte) (message->cmd.mm_data_ind.L3_data[0] & 0x0F);

          /* --------------------------------------------------------------------
          ** 24.008 4.7.3.1.5,4.7.4.1.4, 4.7.5.1.5, 4.7.13.5: cases b.1 and b.2
          ** The procedure shall be initiated again, if 
          ** ii)No SECURITY MODE COMMAND message and no NAS messages relating 
          ** to the PS signalling/RR connection were received after the OTA msg was transmitted
          ** -------------------------------------------------------------------- */
          if(mm_ps_domain_possible_reest)
          {
            mm_ps_domain_possible_reest = FALSE;
          }

          if (protocol_discriminator ==  PD_GMM)
          {
            /* ------------------------------------------------------------
            ** Bits 5 to 8 of the first octet of every GMM message contains
            ** the skip indicator.  A message received with skip indicator
            ** different from 0000 shall be ignored (TS 24.008 10.3.1)
            ** When a message is received that is too short to contain a
            ** complete message type IE, that message shall be ignored
            ** (TS 24.008 8.2)
            ** ------------------------------------------------------------ */
            if (((message->cmd.mm_data_ind.L3_data[0] & 0xF0) == 0) &&
              (message->cmd.mm_data_ind.message_header.message_len_lsb >= 2))
            {
#ifdef FEATURE_TDSCDMA
              /* ---------------------------------------------------------------------
              ** Since Initiation of Security Mode procedures are not guaranteed on
              ** TD-SCDMA networks, move to PMM_CONNECTED mode upon receiving
              ** the first loopback signalling message on DL.
              ** --------------------------------------------------------------------- */
              if ((pmm_mode != PMM_CONNECTED) && is_tdscdma_mode() && !gmm_anite_gcf_flag)
              {
                pmm_mode = PMM_CONNECTED;
                MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_CONNECTED");
                /* ----------------------------------------------------------------  
              ** The timer T3312 is stopped when the MS enters PMM-CONNECTED mode
              ** (24.008 4.7.2.2)
              ** ---------------------------------------------------------------- */
                mm_stop_timer(TIMER_T3312);
              }
#endif /* FEATURE_TDSCDMA */
              /* ---------------------------------
              ** Process according to message type
              ** --------------------------------- */
              switch (message->cmd.mm_data_ind.L3_data[1])
              {
                case GMM_ATTACH_ACCEPT:
                case GMM_ATTACH_REJECT:
                case GMM_DETACH_REQUEST:
                case GMM_DETACH_ACCEPT:
                case GMM_ROUTING_AREA_UPDATE_ACCEPT:
                case GMM_ROUTING_AREA_UPDATE_REJECT:
                  gmm_handle_registration( message );
                  break;

                case GMM_PTMSI_REALLOCATION_COMMAND:
    /**------------------------------------------------------------------------------------------------
        ** spec 24.008 4.7.4.1.4   Abnormal cases in the MS:
         *  d) Detach and GMM common procedure collision:
                  --GPRS detach containing cause "power off":
                        If the MS receives a message used in a GMM common procedure before the GPRS detach procedure has 
                        been completed, this message shall be ignored and the GPRS detach procedure shall continue.

                  --GPRS detach containing other causes than "power off" and containing other detach types than "IMSI detach":
                         If the MS receives a P-TMSI REALLOCATION COMMAND, a GMM STATUS, or a GMM INFORMATION message 
                         before the GPRS detach procedure has been completed, this message shall be ignored and the GPRS detach
                         procedure shall continue. 
    **--------------------------------------------------------------------------------------------------*/
#if defined FEATURE_NAS_REL11
                if(gmm_state == GMM_DEREGISTERED_INITIATED &&
                  (gmm_power_down_active || (gmm_detach_type != GMM_MO_IMSI_DETACH)))
                { 
                   if(gmm_power_down_active)
                   {
                     MSG_HIGH_DS_2( MM_SUB, "=MM= Ignoring gmm_common_procedure(PTMSI_REALLOC), because gmm_state is %d cause poweroff and detach type %d\n", gmm_state,gmm_detach_type);
                   }
                   else
                   {
                     MSG_HIGH_DS_2( MM_SUB, "=MM= Ignoring gmm_common_procedure(PTMSI_REALLOC), because gmm_state is %d and detach type %d\n", gmm_state,gmm_detach_type);
                   }
                }
                else
#endif
                {
                  gmm_handle_security( message );
                }
                
                break;

                case GMM_AUTHENTICATION_AND_CIPHERING_REQUEST:
                case GMM_AUTHENTICATION_AND_CIPHERING_REJECT:
                case GMM_IDENTITY_REQUEST:
#if defined FEATURE_NAS_REL11 
                  if(gmm_state == GMM_DEREGISTERED_INITIATED && gmm_power_down_active)
                  {
                     MSG_HIGH_DS_3( MM_SUB, "=MM= Ignoring gmm_common_procedure %d, because gmm_state is %d cause poweroff and detach type %d\n", message->cmd.mm_data_ind.L3_data[1],gmm_state,gmm_detach_type);
                  }
                  else
#endif 
                  {
                    gmm_handle_security( message );
                  }
    
                  break;

                case GMM_SERVICE_ACCEPT:
                case GMM_SERVICE_REJECT:
                  gmm_handle_connection_management( message );
                  break;

                case GMM_STATUS:
                  /* --------------------------------------------------------------
                  ** If the MS receives a GMM STATUS message no state transition
                  ** and no specific action shall be taken as seen from the radio
                  ** interface, i.e. local actions are possible. (TS 24.008 4.7.10)
                  ** -------------------------------------------------------------- */
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMM_STATUS" );
                  break;

                case GMM_INFORMATION:
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMM_INFORMATION");
#if defined FEATURE_NAS_REL11  
                  if(gmm_state == GMM_DEREGISTERED_INITIATED &&
                    (gmm_power_down_active|| (gmm_detach_type != GMM_MO_IMSI_DETACH)))
                  {
                     if(gmm_power_down_active)
                     {
                       MSG_HIGH_DS_2( MM_SUB, "=MM= Ignoring gmm_common_procedure(GMM_INFO) becuase gmm_state is %d cause poweroff and detach type %d\n", gmm_state,gmm_detach_type);
                     }
                     else
                     {
                       MSG_HIGH_DS_2( MM_SUB, "=MM= Ignoring gmm_common_procedure(GMM_INFO) becuase gmm_state is %d and detach type %d\n", gmm_state,gmm_detach_type);
                     }
                  }
                  else
#endif                
                  {
                    gmm_handle_information ( message );
                  }
                  break;

                default:
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Received unknown GMM message");
                  gmm_send_gmm_status_message( MESSAGE_TYPE_NON_EXISTENT );
                  break;
              } /* end switch (message_type) */
            }
          }
          else if ((protocol_discriminator == PD_GPRS_SM) ||
                   (protocol_discriminator == PD_GTTP))
          {
#ifdef FEATURE_GSM_GPRS
            if ((is_umts_mode() && (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                                    || (!gmm_integrity_protection_activated && is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                                    )) || is_gsm_mode())
#else
            if (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                 || (!gmm_integrity_protection_activated && is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                )
#endif
            {
              gmm_pass_ota_msg_to_sm( message );
            }
            else
            {
              MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated" );
            }
          }
          else if (protocol_discriminator == PD_SMS)
          {
            gmm_pass_ota_msg_to_cnm(message) ;
          }

          #ifdef FEATURE_TC
          else if (protocol_discriminator == PD_TEST)
          {
              byte message_type = (byte) message->cmd.mm_data_ind.L3_data[1];
              /* ---------------------------------------------------------------------
              ** Since Initiation of Security Mode procedures are not guaranteed
              ** during loopback test calls, move to PMM_CONNECTED mode upon receiving
              ** the first loopback signalling message on DL.
              ** --------------------------------------------------------------------- */
              if ( pmm_mode != PMM_CONNECTED )
              {
                pmm_mode = PMM_CONNECTED;
                MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_CONNECTED");
                mm_stop_timer( TIMER_T3317 );
#if defined(FEATURE_LTE)
                mm_stop_timer(TIMER_T3323); 
#endif
              }
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
              if(message_type == RESET_UE_POSITIONING_STORED_INFORMATION)
              {
                MSG_HIGH_DS_0( MM_SUB, "Passing message to GPS");
                mm_process_lcs_ue_pos_stored_info(message);
              }
              else
#endif
              {
                MSG_HIGH_DS_0( MM_SUB, "Passing message to TC");    
#ifdef FEATURE_NAS_REL11
                mm_stop_timer(TIMER_T3240);
#endif				
                gmm_pass_ota_msg_to_tc( message );
              }
          }
          #endif /*FEATURE_TC*/
          else
          {
            MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected PD %d", protocol_discriminator );
          }
          break;

        case RRC_PLMN_LIST_CNF:
        case RRC_EST_CNF:
        case RRC_STOP_WCDMA_MODE_CNF:
          /* --------------------------------------------
          ** No processing associated with this primitive
          ** -------------------------------------------- */
          break;

        case RRC_DEACT_CNF:
        case RRC_MODE_CHANGE_CNF:
          gmm_handle_registration( message );
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message id %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_MM_REG:
      gmm_handle_registration( message );
      break;

    case MS_TIMER:
      if (message->cmd.hdr.message_id == MID_TIMER_EXPIRY)
      {
        MSG_HIGH_DS_1( MM_SUB, "=MM= Timer %d expired", message->cmd.timer_expiry.timer_id);

        /* -------------------------------------------
        ** Flag that timer has expired in timer_status
        ** ------------------------------------------- */
        if (message->cmd.timer_expiry.timer_id <= MAX_MM_TIMER)
        {
          mm_timer_status[message->cmd.timer_expiry.timer_id] = TIMER_STOPPED;
        }

        /* ---------------------------------------------
        ** Send timer message off to appropriate handler
        ** --------------------------------------------- */
        switch (message->cmd.timer_expiry.timer_id)
        {
          case TIMER_T3302:
          case TIMER_T3310:
          case TIMER_T3311:
          case TIMER_T3312:
          case TIMER_T3321:
#ifdef FEATURE_NAS_FREE_FLOAT
          case TIMER_NW_INIT_DETACH:
#endif
          case TIMER_T3330:
#ifdef FEATURE_NAS_REL10
          case TIMER_T3346:
#endif
          case TIMER_T3350:
          case TIMER_POWERDOWN:
          case TIMER_DETACH_REL:
#ifdef FEATURE_DUAL_SIM
          case TIMER_GMM_RADIO_RETRY:
#endif
#if defined(FEATURE_LTE)
          case TIMER_T3323:
#endif
#ifdef FEATURE_LTE
          case TIMER_MM_PS_DETACH:
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
          case TIMER_FOREGROUND_SEARCH:
#endif
            gmm_handle_registration( message );
            break;

          case TIMER_T3314:
          case TIMER_T3317:
          case TIMER_T3319:
          case TIMER_T3340:  
          case TIMER_PS_CONN_REL:
            gmm_handle_connection_management( message );
            break;
          case TIMER_FORBID_TDS_RESELECTION:
            gmm_handle_connection_management( message );
            break;
          case TIMER_T3318:
          case TIMER_T3320:
            gmm_handle_security( message );
            break;

          default:
            MSG_FATAL_DS( MM_SUB, "=MM= Unexpected timer ID %d",
                        message->cmd.timer_expiry.timer_id, 0,0 );
        } /* end switch (message->cmd.timer_expiry.timer_id) */
      }
      break;

#ifdef FEATURE_GSM_GPRS
    case MS_CM_MM:
      gmm_process_cm_req(message) ;
      break ;
#endif /* #ifdef FEATURE_GSM_GPRS */

    default:
      MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set);
  } /* end switch (message->message_set) */
} /* end gmm_handle_message() */

#ifdef FEATURE_LTE
void gmm_perform_local_detach(void)
{
#ifdef FEATURE_GSM_GPRS
   gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_SUSPENSION,FALSE,0) ;
#endif /*FEATURE_GSM_GPRS*/
   gmm_set_state(GMM_DEREGISTERED);
   gmm_set_state(GMM_NULL);
   gmm_set_substate(GMM_NULL_SUBSTATE);
   gmm_reset_global_variables();   
}
#endif

