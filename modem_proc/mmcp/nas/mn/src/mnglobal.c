/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mnglobal.c_v   1.5   17 Aug 2001 15:21:30   abanh  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mnglobal.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
7/19/01    CD      Initial version.  Extracted global data declarations from 
                   mn_initialize_mn_data.c

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

08/12/04    AB     Added support for the SMS link control feature. 

11/02/04    AB     Added user user data buffers & queue to support the
                     SS call related User-to-User Signaling feature.

02/24/05    AB     Added a new MN client, ATCOP.

11/09/05    AB      Added supports for FEATURE_REL5 via NV_FORCE_UE_MSCR_R99_I

09/14/06    NR     Adding changes for passing raw byte stream to CATAPPS 
 
05/25/11    AM     Replacing the value with macro

05/25/11    AM     Fixing the MOB compilation 

===========================================================================*/

/* Includes */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"

/* Globals */

uint32                           mn_asid_2_vsid_map[MAX_AS_IDS] = {0,0};

#if defined(FEATURE_DUAL_SIM) 

#if defined(FEATURE_DUAL_ACTIVE)
mn_dsda_as_id_e_type           mn_dsda_as_id;

/* DUAL SIM : AS INDEX position in Timer IDs*/
#define TIMER_ID_SIM_POS 6
#define MM_TIMER_TAG_AS_ID(MM_TIMER) (timer_id_T)(MM_TIMER | ((mm_sub_id & 0x01) << (TIMER_ID_SIM_POS)))

#endif

sys_modem_as_id_e_type           mn_as_id;

#endif


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

#ifndef FEATURE_MODEM_HEAP
mn_call_information_T            mn_call_information_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
#endif
mn_call_information_T *           mn_call_information_p_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
#ifdef FEATURE_NAS_MN_CB_SERVICES
mn_cb_services_T                 mn_cb_services_sim[MAX_AS_IDS][MN_MAX_CLIENT];
#endif /* FEATURE_NAS_MN_CB_SERVICES */
/* Traffic Channel Configuration */
tch_user_connections_store_T     tch_user_connections_store_sim[MAX_AS_IDS][MAX_NUM_SPEECH_BEARERS];
boolean                          mn_waiting_for_tch_in_srvcc_sim[MAX_AS_IDS];

/* Global for SS and SS call related */
ss_invoke_data_T                 ss_invoke_data_sim[MAX_AS_IDS][MAXNO_INVOKE_IDS];
Multicall_Status_T               Multicall_Status_sim[MAX_AS_IDS][MULTICALL_STATUS_SIZE];

byte                             suppsvc_l3_data_sim[MAX_AS_IDS][MAX_SS_PARAMETER_LENGTH];
word                             suppsvc_l3_data_length_sim[MAX_AS_IDS];

#ifdef FEATURE_UUS
MN_q_T      mn_user_user_data_q_sim[MAX_AS_IDS][USER_USER_DATA_Q_ID+1];  /* No. MO connection ID + 1 user buffer q */
MN_qlink_T  mn_user_user_data_pool_sim[MAX_AS_IDS][MN_MAXNO_USER_USER_DATA+1];  /* q link of the buffer pool */
cm_mo_user_data_req_T   mn_user_user_data_buffers_sim[MAX_AS_IDS][MN_MAXNO_USER_USER_DATA+1];   /* user data buffer pool */
#endif

mn_alternate_procedure_status_T  mn_alternate_proc_status_sim[MAX_AS_IDS];

/* Globals for SMS */
rl_mo_mr_T                       rl_mo_mr_sim[MAX_AS_IDS];
sm_rl_transaction_information_T  sm_rl_transaction_information_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS];
nas_msg_transport_type_e_type    mo_sms_transport_type_sim[MAX_AS_IDS];

mo_rpdu_store_T                  mo_rpdu_store_sim[MAX_AS_IDS];
sms_link_control_T               sms_link_control_sim[MAX_AS_IDS];
mo_rpdu_store_T                  mo_memory_available_rpdu_store_sim[MAX_AS_IDS];
#ifdef FEATURE_REL5
ue_force_rel_mode_type           mn_nv_force_mscr_r99_sim[MAX_AS_IDS];
#endif

/*        last_allocated_mo_sms_connection_id
 * This variable is used to track the last allocated connection_id for MO SMS 
 * by NAS. So, for next mo sms conn_id allocation, connection ids are iterated
 * from last_allocated_mo_sms_connection_id + 1 to last_allocated_mo_sms_connection_id .
 *  */
connection_id_T                   last_allocated_mo_sms_connection_id_sim[MAX_AS_IDS];

/*                  waiting_for_next_network_ussd_trigger 
 *
 *  Scenario:     For NW initiated USSD, after user gives a response, NW releases the transaction.
 *                For some OS, after user response is given for MT USSD, a USSD running menu is thrown
 *                on UI which gets cleared only if UI get a transaction release message from LL (NAS in this case).
 *                So, this variable tracks whether such a response to CM/and above is pending from NAS.
 *
 *  Purpose:      This variable indicates if following message from MN to CM is pending or not.
 *                CM_RELEASE_UNSTRUCTURED_SS_IND
 * 
 *  
 *  Setting:       MN makes it TRUE after getting a response for CM_UNSTRUCTURED_SS_IND
 *  Checking:      When MN receives END_IND, REJ_IND, ABORT_SS_REQ or RELEASE_SS_REQ
 *  Resetting:     MN resets this variable after sending CM_RELEASE_UNSTRUCTURED_SS_IND
 * 
*/
boolean waiting_for_next_network_ussd_trigger_sim[MAX_AS_IDS];

/*                            MN_is_lte_deact_in_progress 
 *
 *  purpose:    This variable indictaes if the below message is pending or not MM to RR/RRC
 *                               RR_ACTIVATION_RSP
 *
 *
 *  Scenario:   In SRVCC cases, after MN gives MMCNM_SRVCC_HANOVER_COMPLETE_IND, 
 *              MN will get context response from CM and MN will initiate transient procedures.
 *              To initiates procedure MN should check for MN_is_lte_deact_in_progress   
 *
 *
 *  Setting     MN makes it TRUE when it Receives MMCNM_SRVCC_HANDOVER_COMPLETE_IND with Trigger=1 (Lte_deact_cnf)
 *  Checking    MN will do transient procedures if  MN_is_lte_deact_in_progress= FALSE otherwise MN will wait for deact_cnf
 *  Resetting   MN resets this variable after completing the transient procedures
 *
 */

boolean                          MN_is_lte_deact_in_progress_sim[MAX_AS_IDS];
nas_msg_transport_type_e_type    mt_sms_transport_type_sim[MAX_AS_IDS];

/* store the timer f value which is received from WMS when SMS is over IMS */
uint32                           sip_config_timer_f_sim[MAX_AS_IDS];

/* Store the Destination Address of the MO SMS here */
cm_called_party_bcd_no_T         mo_sms_destination_addr_sim[MAX_AS_IDS];


#else

#ifndef FEATURE_MODEM_HEAP
mn_call_information_T            mn_call_information[MAXNO_CC_TRANSACTIONS];
#endif
mn_call_information_T *           mn_call_information_p[MAXNO_CC_TRANSACTIONS];
#ifdef FEATURE_NAS_MN_CB_SERVICES 
mn_cb_services_T                 mn_cb_services[MN_MAX_CLIENT];
#endif /* FEATURE_NAS_MN_CB_SERVICES */

/* Traffic Channel Configuration */
tch_user_connections_store_T     tch_user_connections_store[MAX_NUM_SPEECH_BEARERS];

/* mn_waiting_for_tch_in_srvcc - This variable indicates if SRVCC call context transfer 
 * happened from IMS to CS domain without SYNC_IND (HO indication reason) indication received
 * from RRC/RR */
boolean                          mn_waiting_for_tch_in_srvcc;

/* Global for SS and SS call related */
ss_invoke_data_T                 ss_invoke_data[MAXNO_INVOKE_IDS];
Multicall_Status_T               Multicall_Status[MULTICALL_STATUS_SIZE];

byte                             suppsvc_l3_data[MAX_SS_PARAMETER_LENGTH];
word                             suppsvc_l3_data_length;

#ifdef FEATURE_UUS
MN_q_T      mn_user_user_data_q[USER_USER_DATA_Q_ID+1];  /* No. MO connection ID + 1 user buffer q */
MN_qlink_T  mn_user_user_data_pool[MN_MAXNO_USER_USER_DATA+1];  /* q link of the buffer pool */
cm_mo_user_data_req_T   mn_user_user_data_buffers[MN_MAXNO_USER_USER_DATA+1];   /* user data buffer pool */
#endif


/* SS Alternate-Procedure related global variable. This stores whether an alternate procedure 
(2+Send Call Hold/Retrieve), has been initated. It also stores the status of procedure, since this 
requires the synchronization of HOLD Procedure and RETRIEVE Procedure.One event each of Set A and Set B below should 
occur for the alternate procedure to end. Same applies if one of the Calls is a MPTY Call.
A = {HOLD_CNF,HOLD_REJ,HLD_TIMER_EXPIRY}
B = {RETRIEVE_CNF,RETRIEVE_REJ,RETRIEVE_TIMER_EXPIRY}
*/
mn_alternate_procedure_status_T  mn_alternate_proc_status;

/* Globals for SMS */
rl_mo_mr_T                       rl_mo_mr;
sm_rl_transaction_information_T  sm_rl_transaction_information[MAXNO_SMS_TRANSACTIONS];
mo_rpdu_store_T                  mo_rpdu_store;
sms_link_control_T               sms_link_control;
mo_rpdu_store_T                  mo_memory_available_rpdu_store;
nas_msg_transport_type_e_type    mo_sms_transport_type;
boolean                          MN_is_lte_deact_in_progress;

nas_msg_transport_type_e_type    mt_sms_transport_type;

/*      sip_config_timer_f
 * This variable used to store the timer f value which is received from WMS when SMS is over IMS.
 * when the SMS is over IMS, NAS has to start Timer f instead of TR1M timer
 * If the SM-over-IP sender uses SMR entity timers as specified in 3GPP TS 24.011 [8], then TR1M is set to a value greater than Timer F (see 3GPP TS 24.229 [10])
 */
uint32                           sip_config_timer_f;


/*        last_allocated_mo_sms_connection_id
 * This variable is used to track the last allocated connection_id for MO SMS 
 * by NAS. So, for next mo sms conn_id allocation, connection ids are iterated
 * from last_allocated_mo_sms_connection_id + 1 to last_allocated_mo_sms_connection_id .
 *  */
connection_id_T                   last_allocated_mo_sms_connection_id;


/*                  waiting_for_next_network_ussd_trigger 
 *
 *  Scenario:     For NW initiated USSD, after user gives a response, NW releases the transaction.
 *                For some OS, after user response is given for MT USSD, a USSD running menu is thrown
 *                on UI which gets cleared only if UI get a transaction release message from LL (NAS in this case).
 *                So, this variable tracks whether such a response to CM/and above is pending from NAS.
 *
 *  Purpose:      This variable indicates if following message from MN to CM is pending or not.
 *                CM_RELEASE_UNSTRUCTURED_SS_IND
 * 
 *  
 *  Setting:       MN makes it TRUE after getting a response for CM_UNSTRUCTURED_SS_IND
 *  Checking:      When MN receives END_IND, REJ_IND, ABORT_SS_REQ or RELEASE_SS_REQ
 *  Resetting:     MN resets this variable after sending CM_RELEASE_UNSTRUCTURED_SS_IND
 * 
*/
boolean waiting_for_next_network_ussd_trigger;

/* Store the Destination Address of the MO SMS here */
cm_called_party_bcd_no_T         mo_sms_destination_addr;


#endif












