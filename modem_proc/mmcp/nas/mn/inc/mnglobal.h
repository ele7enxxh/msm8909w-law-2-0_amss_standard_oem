#ifndef MNGLOBAL_H
#define MNGLOBAL_H
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mnglobal.h_v   1.0   21 Aug 2001 13:42:08   cdealy  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/inc/mnglobal.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
8/21/01    CD      Initial version.  Extracted global data declarations from 
                   mn header files

04/16/03   AB      Added mn_deferred_end_call_ind_T.

11/09/05    AB      Added supports for FEATURE_REL5 via NV_FORCE_UE_MSCR_R99_I

09/14/06    NR     Adding changes for passing raw byte stream to CATAPPS

05/25/11   AM      Defining MULTICALL_STATUS_SIZE

08/23/11   PM      Added a macro MAX_NUMBER_GSM_SPEECH_CODECS as part of 
                   changes to preserve speech codec preferences given by UI.
===========================================================================*/

#include "mmcp_variation.h"

typedef enum srvcc_trigger_e
{
  T_NULL = 0x00,
  T_2_SEND,
  T_MT_CALL_RES,
  T_START_T310
} srvcc_trigger_e_type;

typedef struct srvcc_transient_triggers
{
  srvcc_trigger_e_type  trigger;
  connection_id_T       connection_id;
}srvcc_transient_triggers_T;

extern sys_modem_device_mode_e_type     mmode_nv_setting_for_device_mode;
extern uint32                           mn_asid_2_vsid_map[MAX_AS_IDS];
extern byte                             nas_is_qmss_enabled;
extern byte                             long_ftn_support;


#ifdef FEATURE_DUAL_SIM
#if defined(FEATURE_DUAL_ACTIVE)
#define MN_SUB ((MNCNM_IS_SINGLE_STACK) ? (mn_as_id):(mn_dsda_as_id))

#else

#define MN_SUB (mn_as_id)
#endif

extern boolean                          mm_mn_sglte_svlte_mode;
#else

#define MN_SUB 0

#endif

#if defined FEATURE_DUAL_SIM 
#define MSG_HIGH_MN_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_HIGH, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_MED_MN_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_MED, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_LOW_MN_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_LOW, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_ERROR_MN_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_ERROR, "ds%d" str , (sub+1), (p1), (p2), (p3))

/* Warning: 3rd argument is not logged */
#define MSG_FATAL_MN_DS( sub , str, p1, p2, p3)\
    ERR_FATAL ("ds%d" str , (sub+1), (p1), (p2))

#endif



#ifndef MSG_HIGH_MN_DS
#define MSG_HIGH_MN_DS( sub , str, p1, p2, p3)\
    MSG_HIGH (str , (p1), (p2), (p3))
#endif
#ifndef MSG_MED_MN_DS
#define MSG_MED_MN_DS( sub , str, p1, p2, p3)\
    MSG_MED (str , (p1), (p2), (p3))
#endif
#ifndef MSG_LOW_MN_DS
#define MSG_LOW_MN_DS( sub , str, p1, p2, p3)\
    MSG_LOW (str , (p1), (p2), (p3))
#endif
#ifndef MSG_ERROR_MN_DS
#define MSG_ERROR_MN_DS( sub , str, p1, p2, p3)\
    MSG_ERROR (str , (p1), (p2), (p3))
#endif
#ifndef MSG_FATAL_MN_DS
#define MSG_FATAL_MN_DS( sub , str, p1, p2, p3)\
    ERR_FATAL (str , (p1), (p2), (p3))
#endif


typedef enum
{
       MN_AS_ID_1=0,
       /**< Subscription ID 1 */

       MN_AS_ID_2,
       /**< Subscription ID 2 */

       
}mn_dsda_as_id_e_type;

#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_DUAL_ACTIVE

extern mn_dsda_as_id_e_type          mn_dsda_as_id;

/* DUAL SIM : AS INDEX position in Timer IDs*/
#define MNCNM_TIMER_ID_SIM_POS 7
#define MNCNM_TIMER_TAG_AS_ID(MNCNM_TIMER, as_id) (timer_id_T)((MNCNM_TIMER) | (((as_id) & 0x01) << MNCNM_TIMER_ID_SIM_POS))
#endif
extern sys_modem_as_id_e_type           mn_as_id;

/*************MNCNM_IS_SINGLE_STACK*************
 *  This MACRO if TRUE means that MNCNM works on Single Stack like DSDS
*/

/* changed for JO bring-up. mm_mn_sglte_svlte_mode is set to TRUE in single sim mode by Policy Manager. fmode is set to SVLTE mode.
#define MNCNM_IS_SINGLE_STACK       ((mm_mn_sglte_svlte_mode == FALSE) && (mmode_nv_setting_for_device_mode != SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE))
*/

#define MNCNM_IS_SINGLE_STACK       ((mmode_nv_setting_for_device_mode != SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE))
#endif

#define ITC_AUX_SPEECH_4    0x04
#define ITC_AUX_SPEECH_6    0x06
#define MULTICALL_STATUS_SIZE   3



#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

#ifndef FEATURE_MODEM_HEAP
extern mn_call_information_T            mn_call_information_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
#define  mn_call_information (mn_call_information_sim[mn_dsda_as_id])
#endif
extern cc_transaction_information_T     cc_transaction_information_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
extern mn_call_information_T            *mn_call_information_p_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
extern cc_transaction_information_T     *cc_transaction_information_p_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
extern sms_transaction_information_T    sms_transaction_information_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS];
extern ss_transaction_information_T     ss_transaction_information_sim[MAX_AS_IDS][MAXNO_SS_TRANSACTIONS];
#ifdef FEATURE_NAS_MN_CB_SERVICES 
extern mn_cb_services_T                 mn_cb_services_sim[MAX_AS_IDS][MN_MAX_CLIENT];
#endif /* FEATURE_NAS_MN_CB_SERVICES */
extern tch_user_connections_store_T     tch_user_connections_store_sim[MAX_AS_IDS][MAX_NUM_SPEECH_BEARERS];
extern boolean                          mn_waiting_for_tch_in_srvcc_sim[MAX_AS_IDS];
extern ss_invoke_data_T                 ss_invoke_data_sim[MAX_AS_IDS][MAXNO_INVOKE_IDS];
extern Multicall_Status_T               Multicall_Status_sim[MAX_AS_IDS][MULTICALL_STATUS_SIZE];
extern byte                             suppsvc_l3_data_sim[MAX_AS_IDS][MAX_SS_PARAMETER_LENGTH];
extern word                             suppsvc_l3_data_length_sim[MAX_AS_IDS];
extern mn_alternate_procedure_status_T  mn_alternate_proc_status_sim[MAX_AS_IDS];
extern sm_rl_transaction_information_T  sm_rl_transaction_information_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS];
extern nas_msg_transport_type_e_type    mo_sms_transport_type_sim[MAX_AS_IDS];
extern mo_rpdu_store_T                  mo_rpdu_store_sim[MAX_AS_IDS];
extern mo_rpdu_store_T                  mo_memory_available_rpdu_store_sim[MAX_AS_IDS];
extern connection_id_T                  last_allocated_mo_sms_connection_id_sim[MAX_AS_IDS];
extern rl_mo_mr_T rl_mo_mr_sim[MAX_AS_IDS];
extern sms_link_control_T sms_link_control_sim[MAX_AS_IDS];
extern boolean                          MN_is_lte_deact_in_progress_sim[MAX_AS_IDS];
extern srvcc_transient_triggers_T       buffer_transient_triggers_sim[MAX_AS_IDS][MAX_TRANSIENT_TRIGGERS];
/* Store the Destination Address of the MO SMS here */
extern cm_called_party_bcd_no_T         mo_sms_destination_addr_sim[MAX_AS_IDS];

/* store the timer f value which is received from WMS when SMS is over IMS */
extern uint32                           sip_config_timer_f_sim[MAX_AS_IDS];


/*          waiting_for_next_network_ussd_trigger 
 * This variable indicates if following message from MN to CM is pending or not.
 * CM_RELEASE_UNSTRUCTURED_SS_IND
 */
extern boolean waiting_for_next_network_ussd_trigger_sim[MAX_AS_IDS];

extern nas_msg_transport_type_e_type    mt_sms_transport_type_sim[MAX_AS_IDS];



#ifdef FEATURE_UUS
extern MN_q_T                           mn_user_user_data_q_sim[MAX_AS_IDS][USER_USER_DATA_Q_ID+1];
extern MN_qlink_T                       mn_user_user_data_pool_sim[MAX_AS_IDS][MN_MAXNO_USER_USER_DATA+1];  /* q link of the buffer pool */
extern cm_mo_user_data_req_T            mn_user_user_data_buffers_sim[MAX_AS_IDS][MN_MAXNO_USER_USER_DATA+1];   /* user data buffer pool */
#endif


#define  cc_transaction_information (cc_transaction_information_sim[mn_dsda_as_id])
#define mn_call_information_p (mn_call_information_p_sim[mn_dsda_as_id])
#ifdef FEATURE_NAS_MN_CB_SERVICES 
#define mn_cb_services (mn_cb_services_sim[mn_dsda_as_id])
#endif /* FEATURE_NAS_MN_CB_SERVICES */
#define tch_user_connections_store (tch_user_connections_store_sim[mn_dsda_as_id])
#define mn_waiting_for_tch_in_srvcc (mn_waiting_for_tch_in_srvcc_sim[mn_dsda_as_id])
#define ss_invoke_data (ss_invoke_data_sim[mn_dsda_as_id])
#define Multicall_Status (Multicall_Status_sim[mn_dsda_as_id])
#define suppsvc_l3_data (suppsvc_l3_data_sim[mn_dsda_as_id])
#define suppsvc_l3_data_length (suppsvc_l3_data_length_sim[mn_dsda_as_id])
#define mn_alternate_proc_status (mn_alternate_proc_status_sim[mn_dsda_as_id])
#define sm_rl_transaction_information (sm_rl_transaction_information_sim[mn_dsda_as_id])
#define mo_sms_transport_type (mo_sms_transport_type_sim[mn_dsda_as_id])
#define mo_rpdu_store (mo_rpdu_store_sim[mn_dsda_as_id])
#define mo_memory_available_rpdu_store (mo_memory_available_rpdu_store_sim[mn_dsda_as_id])
#define last_allocated_mo_sms_connection_id (last_allocated_mo_sms_connection_id_sim[mn_dsda_as_id])
#define rl_mo_mr (rl_mo_mr_sim[mn_dsda_as_id])
#define sms_link_control (sms_link_control_sim[mn_dsda_as_id])
#define mo_sms_destination_addr (mo_sms_destination_addr_sim[mn_dsda_as_id])
#define cc_transaction_information_p (cc_transaction_information_p_sim[mn_dsda_as_id])
#define sms_transaction_information (sms_transaction_information_sim[mn_dsda_as_id])
#define ss_transaction_information (ss_transaction_information_sim[mn_dsda_as_id])
#define waiting_for_next_network_ussd_trigger (waiting_for_next_network_ussd_trigger_sim[mn_dsda_as_id])
#define buffer_transient_triggers  (buffer_transient_triggers_sim[mn_dsda_as_id])
#define MN_is_lte_deact_in_progress (MN_is_lte_deact_in_progress_sim[mn_dsda_as_id])
#define mt_sms_transport_type (mt_sms_transport_type_sim[mn_dsda_as_id])
#define sip_config_timer_f (sip_config_timer_f_sim[mn_dsda_as_id])


#ifdef FEATURE_UUS
#define mn_user_user_data_q (mn_user_user_data_q_sim[mn_dsda_as_id])
#define mn_user_user_data_pool (mn_user_user_data_pool_sim[mn_dsda_as_id])
#define mn_user_user_data_buffers (mn_user_user_data_buffers_sim[mn_dsda_as_id])
#endif


#else /* defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE) */

#ifndef FEATURE_MODEM_HEAP
extern mn_call_information_T            mn_call_information[MAXNO_CC_TRANSACTIONS];
#endif
extern srvcc_transient_triggers_T       buffer_transient_triggers[MAX_TRANSIENT_TRIGGERS];
extern cc_transaction_information_T    cc_transaction_information[MAXNO_CC_TRANSACTIONS];
extern mn_call_information_T           *mn_call_information_p[MAXNO_CC_TRANSACTIONS];
extern cc_transaction_information_T    *cc_transaction_information_p[MAXNO_CC_TRANSACTIONS];
extern sms_transaction_information_T   sms_transaction_information[MAXNO_SMS_TRANSACTIONS];
extern ss_transaction_information_T    ss_transaction_information[MAXNO_SS_TRANSACTIONS];
#ifdef FEATURE_NAS_MN_CB_SERVICES 
extern mn_cb_services_T                mn_cb_services[MN_MAX_CLIENT];
#endif /* FEATURE_NAS_MN_CB_SERVICES */
extern tch_user_connections_store_T    tch_user_connections_store[MAX_NUM_SPEECH_BEARERS];
extern boolean                         mn_waiting_for_tch_in_srvcc;
extern ss_invoke_data_T                ss_invoke_data[MAXNO_INVOKE_IDS];
extern Multicall_Status_T              Multicall_Status[MULTICALL_STATUS_SIZE];
extern byte                            suppsvc_l3_data[MAX_SS_PARAMETER_LENGTH];
extern word                            suppsvc_l3_data_length;
extern mn_alternate_procedure_status_T mn_alternate_proc_status;
extern sm_rl_transaction_information_T sm_rl_transaction_information[MAXNO_SMS_TRANSACTIONS];

/* mo_sms_transport_type - Stores the transport type
 * over which current SMS is on-going.
 * If this is set and a new SMS is received with transport 
 * different than this, new SMS will be rejected by NAS.
 * */
extern nas_msg_transport_type_e_type   mo_sms_transport_type;
extern mo_rpdu_store_T                 mo_rpdu_store;
extern mo_rpdu_store_T                 mo_memory_available_rpdu_store;
extern connection_id_T                 last_allocated_mo_sms_connection_id;
extern rl_mo_mr_T                      rl_mo_mr;
extern sms_link_control_T              sms_link_control;

/* This variables tells whether NAS received LTE_DEACT_CNF or not */
extern boolean                         MN_is_lte_deact_in_progress;
extern nas_msg_transport_type_e_type   mt_sms_transport_type;

/* store the timer f value which is received from WMS when SMS is over IMS */
extern uint32                          sip_config_timer_f;

/* Store the Destination Address of the MO SMS here */
extern cm_called_party_bcd_no_T        mo_sms_destination_addr;
#ifdef FEATURE_UUS
extern MN_q_T                          mn_user_user_data_q[USER_USER_DATA_Q_ID+1];
extern MN_qlink_T                      mn_user_user_data_pool[MN_MAXNO_USER_USER_DATA+1];  /* q link of the buffer pool */
extern cm_mo_user_data_req_T           mn_user_user_data_buffers[MN_MAXNO_USER_USER_DATA+1];   /* user data buffer pool */
#endif

/*          waiting_for_next_network_ussd_trigger 
 * This variable indicates if following message from MN to CM is pending or not.
 * CM_RELEASE_UNSTRUCTURED_SS_IND
 */
extern boolean waiting_for_next_network_ussd_trigger;


#endif /* defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE) */
#ifdef FEATURE_TDSCDMA
extern uint8                 mn_special_tdscdma_test_setting_enabled;
#endif /* FEATURE_TDSCDMA */


/*Refer to Spec 24.080*/
#define INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE 8 /*Component, Invoke and Operation. Length takes 1 octet*/
#define INVOKE_COMPONENT_OCTETS_MAX_SIZE_USED 12
#define INVOKE_COMPONENT_OCTETS_MAX_LENGTH 15 /*Component, Invoke, Linked ID, Operation. All with length in X.209 notation with length taking 2 octets*/
#define USSD_RESPONSE_OCTET_SIZE 9 
#define RETURN_RESULT_OCTET_SIZE 10 /* Used to allocate buffer of this size, when args size is non zero but less than 127*/
#define RETURN_RESULT_OCTETS_MAX_SIZE_USED 12 /* Max buff size used to put RETURN RESULT*/
#define RETURN_RESULT_OCTETS_MAX_SIZE 15 /*Max size allowed in spec for RETURN RESULT except args*/
#define REJECT_OCTETS_SIZE 30
#define MAX_ELEMENT_LENGTH 3
#define MAX_USSD_ARGS_SIZE 9
#define RETURN_ERROR_MAX_OCTETS 12

/* Maximum value which a variable of type unsigned char can hold */
#define MAX_UNSIGNED_CHAR_LIMIT 255

/* Default call clearing cause used in logging cause as part of log packet 0x713D to DIAG, when MN gets MNCC_REL_IND */
#define DEFAULT_CALL_CLEARING_CAUSE_VALUE 31
/* Default call clearing cause used in logging cause as part of log packet 0x713D to DIAG, when MN gets MNCC_REJ_IND */
#define CALL_CLEARING_CAUSE_REJECT 0
/* Default call clearing cause used in logging cause as part of log packet 0x713D to DIAG, when MN gets MNCC_REL_CNF */
#define DEFAULT_CALL_CLEARING_CAUSE_REL_COMP 47

/* Maximum Number of GSM speech codecs given by CM in bearer cap */
#define MAX_NUMBER_GSM_SPEECH_CODECS 6

#endif  /* MNGLOBAL_H */
