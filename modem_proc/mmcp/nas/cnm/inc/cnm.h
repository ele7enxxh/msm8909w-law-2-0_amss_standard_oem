#ifndef cnm_h
#define cnm_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm.h_v   1.21   22 Apr 2002 09:46:32   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/inc/cnm.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03-May-2001  VT    Compiler Warnings Level 4: In the declarations of the 
                    following functions, removed the first param of type 
                    IMH_T *  :
                          CM_process_alerting_message(),
                          CM_process_call_proceeding_message(),
                          CM_process_connect_message(),
                          CM_process_connect_ack_message(),
                          CM_process_progress_message(),
                          CM_process_setup_message(),
                          CM_process_modify_message(),
                          CM_process_disconnect_message(),
                          CM_process_release_message(),
              CM_process_facility_message(),
              CM_process_hold_ack_message(),
              CM_process_hold_rej_message(),
              CM_process_retrieve_ack_message(),
              CM_process_retrieve_rej_message(),
              CM_process_start_dtmf_ack_message(),
              CM_process_start_dtmf_rej_message(),
              CM_process_stop_dtmf_ack_message(),
              CM_process_status_message(),
              CM_process_status_enquiry_message(),
              CM_process_user_information_message(),
              CM_process_congestion_control_message(),
              CM_process_notify_message() - removed 
                          duplicate declaration for this function too,
              CM_process_modify_complete_reject_message(),
              CM_process_mo_notify_request(),
              CM_process_abort_request(),
              CM_process_sms_release_message(),
              CM_process_mm_conn_accepted()
09-May-2001 CD/VT  Added declaration for the new function, 
                    DEBUG_get_smr_state_name()
20-Jun-2001 VT     Removed unwanted declaration void debug_output( void ); 

28-Jun-2001 VT     Removed unwanted #ifdef NUCLEUS_PLUS
 
08/02/01    CD     Replaced ms_sms.h by mn.h

08/07/01    CD     Removed redundant declarations of functions
                       
                   Fixed lint error 

8/8/01      CD     Added prototype for cnm_log_message

8/9/01      CD     Modified CNM_log_message prototype

8/9/01      CD     Added CNM_log_message_type, CNM_log_CM_service_type

08/14/01    AB     Renamed connection manager references to CNM from CM.
                   
9/13/01     CD    Changed function prototypes for QXDM logging functions

9/17/01     CD    Changed function prototype for CNM_sms_respond_to_event
                  to VOID_FUNC
                  Added MMSS_REEST_CNF_event and MMSS_REJ_IND_event

9/29/01     CD    Added cnmglobal.h

10/01/01    CD   Added CNM_log_rp_mti

24-Oct-2001  VT   DTMF: In macro, CNM_TIMER_NAME:
                    added TIMER_T336 & TIMER_T337  and
                    removed TIMER_DTMF_ON & TIMER_DTMF_OFF

                   Added connection_id_T as parameter to the following 
                    function prototypes:
                    CNM_read_dtmf_queue() & CNM_clear_dtmf_queue()

26-Oct-2001  VT   DTMF: Removed DTMF_ON_TIMEOUT_event & DTMF_OFF_TIMEOUT_event.
                    Added DTMF_T336_expiry_event & DTMF_T337_expiry_event.
                    Added prototype declarations for:
                      VOID_FUNC   CNM_process_t336_expiry() and
                      VOID_FUNC   CNM_process_t336_expiry()

29-Oct-2001  AB   SS: Added SS state access utilities functions prototypes.

04/19/02     CD   Changed function prototype of CNM_process_mm_conn_reject()
                  Replaced MMSS_REEST_CNF_event by MMSS_REEST_REJ_event

                  Added MMCC_EST_REJ_event

                  Deleted definition of mm_connections_active_T, and added 
                  mm_rel_req_sent flag

                  Deleted prototype for cnm_check_for_last_connection_released()

                  Added functions CNM_check_for_last_connection(), 
                  CNM_send_mn_rel_cnf(), CNM_check_for_establish_ind()

                  Deleted CNM_TIMER_NAME macro since it is a duplication of 
                  DEBUG_get_timer_name

                  Changed prototype for CNM_process_message_type()

                  Updated copyright

07/29/02   CD     Changed prototype of CNM_process_mm_conn_reject() and 
                  CNM_process_mm_reestablishment()

09/16/02   AB     Added new function prototype, CNM_process_mo_release_complete().

04/06/03   AB     Changed the CNM_Setup_MNCC_REJ_IND() interface to new reject 
                      type and reject value.   

05/10/02   AB     Added new function prototype to CC calls statistics.

05/11/05   HS     Added support for FEATURE_CCBS

09/01/05   HS     CNM_emulate_sms_TC1M_MO_timeout() added

09/26/05   AB     Replace CNM_emulate_sms_TC1M_MO_timeout() w/ CNM_active_SMS().

11/02/06   NR     Removing lint erros

04/09/07   RD     SMS optimization changes are incorporated. 

05/21/07   NR     Adding new function for calculatin the type of call, 
                  to pass it to MM.
09/12/11   HC     Replacing the #include "ms_timer.h" with #include "ms_timer_v.h" 
                  as a part of CMI activity 

07/05/11   PM      Adding lower_layer_cause as new argument to function 
                   CNM_send_error_indication_to_rp_layer(). This will determine
                   the error_status to be sent to WMS by MN while handling
                   MNSMS_ERROR_IND.
==========================================================================*/

#include "mmcp_variation.h"
#include <stdio.h>
#include <string.h>

#include "mn.h"
#include "ms_timer_v.h"
#include "cc_cc.h"

#define MN_CNM_SLEEP_SIG   0x0001
#define MN_CNM_SLEEP_1_SEC   1000   /* 1000 milliseconds */

typedef struct cnm_request_id_tag
{
   CNMServiceType_T     CNM_service_type;
   connection_id_T      connection_id;
   mmcnm_data_req_T     mmcnm_data_req;

} cnm_request_id_T;


/*
 * Timers store - mapping of CNM timer id to actual timer id
 */

typedef struct timer_store_tag
{
   byte timer_id;
   byte ti_pd;
} timer_store_T;



typedef struct ie_info_tag
{
   byte  *start;        /* pointer to start of IE in message */
   byte  IEI;           /* identity of this IE */
   byte  length;        /* actual length of the IE in this message */
   byte  presence;      /* flag to indicate if mandatory or not    */
   byte  syntax_ok;     /*  flag to indicate whether the IE has passed the syntax check  */
} IE_info_T;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

extern cnm_request_id_T           mm_connection_pending_sim[MAX_AS_IDS];
extern boolean                    mm_link_reestablishing_sim[MAX_AS_IDS];
extern boolean                    mm_rel_req_sent_sim[MAX_AS_IDS];
extern cc_state_T                last_state_sim[MAX_AS_IDS]; 
extern byte                      cc_progress_information_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
extern timer_store_T             timer_store_sim[MAX_AS_IDS][MAX_CNM_TIMER];
extern byte                      timer_store_pointer_sim[MAX_AS_IDS];
/* TCH assignment */
extern mmcc_sync_ind_T           CNM_mmcc_sync_ind_sim[MAX_AS_IDS];

extern byte  no_of_IEs_sim[MAX_AS_IDS];        /* set to zero initially in this function */
extern IE_info_T  cc_IE_info_sim[MAX_AS_IDS][MAXNO_IES_IN_MESSAGE];

#define mm_connection_pending (mm_connection_pending_sim[mn_dsda_as_id])
#define mm_link_reestablishing (mm_link_reestablishing_sim[mn_dsda_as_id])
#define mm_rel_req_sent (mm_rel_req_sent_sim[mn_dsda_as_id])
#define last_state (last_state_sim[mn_dsda_as_id])
#define cc_progress_information (cc_progress_information_sim[mn_dsda_as_id])
#define timer_store (timer_store_sim[mn_dsda_as_id])
#define timer_store_pointer (timer_store_pointer_sim[mn_dsda_as_id])
#define CNM_mmcc_sync_ind (CNM_mmcc_sync_ind_sim[mn_dsda_as_id])
#define no_of_IEs (no_of_IEs_sim[mn_dsda_as_id])
#define cc_IE_info (cc_IE_info_sim[mn_dsda_as_id])


#else

extern cnm_request_id_T           mm_connection_pending;
extern boolean                    mm_link_reestablishing;
extern boolean                    mm_rel_req_sent;

/* 
 * state prior to processing a msg 
 */
extern cc_state_T                last_state; 
extern byte                      cc_progress_information[MAXNO_CC_TRANSACTIONS];
extern timer_store_T             timer_store[MAX_CNM_TIMER];
extern byte                      timer_store_pointer;
extern mmcc_sync_ind_T           CNM_mmcc_sync_ind;

/*
 * definitions of global data for storage of IE information. this is
 * filled in by CNM_check_for_protocol_errors and used by a number of functions
 * which process the message contents.
 */

extern byte  no_of_IEs;        /* set to zero initially in this function */
extern IE_info_T  cc_IE_info[MAXNO_IES_IN_MESSAGE];

#endif


/*
 * useful macro function definitions
 */

#define FORM_TI_PD(ti, pd ) ((unsigned char)  \
                        (((unsigned char)ti & (unsigned char)0x0f) << 4 ) + \
                        (unsigned char)(pd & 0x0f) )

/*
 * Definitions for c_cc_event
 */

typedef  byte      event_T;

/*
 * The following allocation of event numbers is linked to the data table
 * define in m21102.c. This allocation should not be changed without a
 * corresponding change to m21102.c data tables.
 */

#define   MNCC_SETUP_REQ_event             0
#define   MMCC_EST_CONF_event              1
#define   MMCC_REL_IND_event               2
#define   CC_CALL_PROCEEDING_event         3
#define   CC_ALERTING_event                4
#define   MMCC_SYNC_IND_event              5
#define   CC_CONNECT_event                 6
#define   CC_PROGRESS_event                7
#define   MMCC_EST_IND_event               8
#define   MNCC_CALL_CONF_REQ_event         9
#define   MNCC_REJ_REQ_event               10
#define   MNCC_ALERT_REQ_event             11
#define   MNCC_SETUP_RSP_event             12
#define   CC_CONNECT_ACK_event             13
#define   MNCC_DISC_REQ_event              14
#define   CC_RELEASE_event                 15
#define   CC_DISCONNECT_event              16
#define   MNCC_REL_REQ_event               17
#define   CC_RELEASE_COMPLETE_event        18
#define   T303_expiry_event                19
#define   T305_expiry_event                20
#define   T308_expiry_event                21
#define   T310_expiry_event                22
#define   T313_expiry_event                23
#define   CC_MODIFY_event                  24
#define   MNCC_MODIFY_REQ_event            25
#define   T323_expiry_event                26
#define   CC_MODIFY_COMPLETE_event         27
#define   CC_MODIFY_REJECT_event           28
#define   MNCC_START_DTMF_REQ_event        29
#define   MNCC_STOP_DTMF_REQ_event         30
#define   CC_START_DTMF_ACK_event          31
#define   CC_STOP_DTMF_ACK_event           32
#define   CC_START_DTMF_REJECT_event       33
#define   DTMF_T336_expiry_event           34
#define   DTMF_T337_expiry_event           35
#define   CC_STATUS_ENQUIRY_event          36
#define   CC_STATUS_event                  37
#define   MNCC_FACILITY_REQ_event          38
#define   CC_FACILITY_event                39
#define   MNCC_USER_USER_DATA_REQ_event    40
#define   CC_USER_INFORMATION_event        41
#define   CC_CONGESTION_CONTROL_event      42
#define   MNCC_NOTIFY_REQ_event            43
#define   CC_NOTIFY_event                  44
#define   MNCC_CALL_HOLD_REQ_event         45
#define   MNCC_CALL_RETRIEVE_REQ_event     46
#define   CC_HOLD_ACK_event                47
#define   CC_HOLD_REJECT_event             48
#define   CC_RETRIEVE_ACK_event            49
#define   CC_RETRIEVE_REJECT_event         50
#define   MNCC_SETUP_COMPL_RES_event       51
#define   MNCC_MODIFY_RES_event            52
#define   MMCC_EST_REJ_event               53
#define   T324_expiry_event                54
#ifdef FEATURE_CCBS
#define   MMCNM_PROMPT_IND_event           (T324_expiry_event+1)
#define   MNCC_START_CC_REQ_event          (MMCNM_PROMPT_IND_event+1)
#define   CC_CC_ESTABLISHMENT_event        (MMCNM_PROMPT_IND_event+2)
#define   MNCC_CC_EST_CONF_REQ_event       (MMCNM_PROMPT_IND_event+3)
#define   CC_RECALL_event                  (MMCNM_PROMPT_IND_event+4)
#endif /* FEATURE_CCBS */

/*
 * the following are not in the SEM
 */

#ifdef FEATURE_CCBS
#define   UNDEFINED_MESSAGE_TYPE_event     (CC_RECALL_event+1)
#else
#define   UNDEFINED_MESSAGE_TYPE_event     54
#endif /* FEATURE_CCBS */

#define MIN_CC_EVENT       0
#define MAX_CC_EVENT       UNDEFINED_MESSAGE_TYPE_event

/*
 * The following events are defined for SS transactions
 */

#define  MNSS_BEGIN_REQ_event              0
#define  MMSS_EST_IND_event                1
#define  MMSS_EST_CNF_event                2
#define  MNSS_FACILITY_REQ_event           3
#define  MNSS_FACILITY_IND_event           4
#define  MNSS_END_REQ_event                5
#define  MNSS_END_IND_event                6
#define  MMSS_REL_IND_event                7

#define  MMSS_REEST_REJ_event              8
#define  MMSS_REJ_IND_event                9

/*
 * Function prototypes for all CNM functions
 */

char * DEBUG_identify_IE( byte IEI);
char * DEBUG_get_timer_name(timer_id_T timer_id);
char * DEBUG_get_sms_state_name( byte s );
char * DEBUG_get_smr_state_name( byte s );
char * DEBUG_get_ss_state_name( byte s );
char * DEBUG_get_state_name( byte s );


VOID_FUNC        CNM_handle_call_control( IMH_T * );
VOID_FUNC        CNM_handle_short_message_services( IMH_T * ); 
VOID_FUNC        CNM_handle_supplementary_services( IMH_T * );   
byte             CNM_cc_check_respond_to_event( connection_id_T, event_T );
VOID_FUNC        CNM_cc_respond_to_event( connection_id_T, byte );
connection_id_T  CNM_get_connection_id( mmcnm_data_ind_T * );
VOID_FUNC        CNM_process_mm_reestablishment( void );
boolean          CNM_ss_respond_to_event( event_T, connection_id_T );
VOID_FUNC        CNM_set_ss_state(transaction_id_T id, ss_state_T state);
ss_state_T       CNM_get_ss_state(transaction_id_T id);
VOID_FUNC        CNM_sms_respond_to_event( event_T, sms_state_T,
                                          connection_id_T );
VOID_FUNC        CNM_fill_mmcnm_rej_ind(mmcnm_est_rej_T *p_mmcnm_rej_ind, 
                                        byte             rel_ind_cause);
VOID_FUNC        CNM_process_mm_conn_accepted( void );
VOID_FUNC        CNM_process_mm_conn_reject( IMH_T * );
VOID_FUNC        CNM_send_MNCC_REJ_IND( CNMServiceType_T, 
                                       lower_layer_cause_T,
                                       byte * );
VOID_FUNC        CNM_send_MNSS_REJ_IND( connection_id_T, lower_layer_cause_T );
VOID_FUNC        CNM_send_MNSMS_ERROR_IND( connection_id_T, 
                                          lower_layer_cause_T, 
                                          byte 
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                                          , reject_cause_T , 
                                          mmcc_domain_name_T
#endif
                                                                             );
VOID_FUNC        CNM_send_CPACK_IND(  connection_id_T, byte, byte);
VOID_FUNC        CNM_recover_on_timer_expiry( timer_expiry_T * );
boolean          CNM_read_cnm_service_request_queue( cnm_request_id_T * );
boolean          CNM_pop_cnm_service_request_queue( cnm_request_id_T * );
VOID_FUNC     CNM_send_est_request_from_service_queue(void);
boolean       CNM_check_cnm_service_request_queue(CNMServiceType_T CNM_service_type_arg);
VOID_FUNC        CNM_add_to_cnm_service_request_queue( cnm_request_id_T * );
VOID_FUNC        CNM_init_cnm_service_request_queue( void );
byte             CNM_write_cause_ie( byte *, byte );
boolean        CNM_append_IE_to_message( byte *, byte **, byte, byte * );
VOID_FUNC         CNM_process_mm_establishment_lost( mmcnm_rel_ind_T * , CNMServiceType_T CNM_service_type );


VOID_FUNC      CNM_process_alerting_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_call_proceeding_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_connect_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_connect_ack_message( connection_id_T );
VOID_FUNC      CNM_process_progress_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_setup_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_notify_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_modify_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_modify_complete_reject_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_disconnect_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_release_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_release_complete_message( connection_id_T connection_id, boolean syntax_ok, byte error_cause);
VOID_FUNC      CNM_process_facility_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_hold_ack_message( connection_id_T );
VOID_FUNC      CNM_process_hold_rej_message( connection_id_T );
VOID_FUNC      CNM_process_retrieve_ack_message( connection_id_T );
VOID_FUNC      CNM_process_retrieve_rej_message( connection_id_T );
VOID_FUNC      CNM_process_start_dtmf_ack_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_start_dtmf_rej_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_stop_dtmf_ack_message( connection_id_T );
VOID_FUNC      CNM_process_status_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_status_enquiry_message( connection_id_T );
VOID_FUNC      CNM_process_user_information_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
VOID_FUNC      CNM_process_congestion_control_message( connection_id_T connection_id, boolean syntax_ok, byte cause);
#ifdef FEATURE_CCBS
VOID_FUNC      CNM_process_cc_establishment_message( connection_id_T connection_id, boolean syntax_ok);
VOID_FUNC      CNM_process_recall_message( connection_id_T connection_id, boolean syntax_ok);
#endif /* FEATURE_CCBS */
boolean        CNM_check_IE_syntax(byte , byte *,byte);
VOID_FUNC      CNM_send_message( IMH_T *, gs_queue_id_T );
VOID_FUNC      CNM_setup_mncc_progress_ind( MNCC_PROGRESS_IND_T *, 
                                          connection_id_T, 
                                          ie_progress_indicator_T * );
VOID_FUNC      CNM_send_mmcnm_rel_req ( IMH_T *message );


VOID_FUNC      CNM_fill_mmcnm_data_req(mmcnm_data_req_T*);
#ifdef FEATURE_DUAL_SIM
VOID_FUNC      CNM_fill_mmcnm_est_req(mmcnm_est_req_T*);
#endif

VOID_FUNC      CNM_send_primitive_to_mm ( connection_id_T,
                                         byte,
                                         IMH_T * );
#ifdef FEATURE_CCBS
VOID_FUNC CNM_send_mmcnm_prompt_res( 
                                     boolean support, 
                                     boolean accept 
#if defined(FEATURE_DUAL_SIM )
                                     ,sys_modem_as_id_e_type  as_id
#endif
                                     );
#endif /* FEATURE_CCBS */
VOID_FUNC      CNM_send_mn_rel_cnf (connection_id_T, lower_layer_cause_T,
                                    int, byte * );

#ifdef FEATURE_UUS
VOID_FUNC      CNM_setup_mncc_user_user_data_ind( MNCC_USER_USER_DATA_IND_T *,
                                                connection_id_T,
                                                ie_user_user_T * );

VOID_FUNC      CNM_setup_mncc_congestion_ind( MNCC_CONGESTION_IND_T *,
                                                connection_id_T,
                                                ie_congestion_control_T * );
#endif

byte           CNM_identify_IE(void * , IMH_T * , byte * );
VOID_FUNC      CNM_process_message_type( mmcnm_data_ind_T * );
VOID_FUNC      CNM_process_tch_assigned( IMH_T * );
VOID_FUNC      CNM_process_mo_alert_request( IMH_T * );
VOID_FUNC      CNM_process_mt_call_confirm( IMH_T * );
VOID_FUNC      CNM_process_mo_call_hold_request( IMH_T * );
VOID_FUNC      CNM_process_mo_call_retrieve_request( IMH_T * );
VOID_FUNC      CNM_process_mo_disc_request( IMH_T * );
VOID_FUNC      CNM_process_mo_facility_request( IMH_T * );
VOID_FUNC      CNM_process_mt_call_reject( IMH_T * );
VOID_FUNC      CNM_process_mo_release_request( IMH_T * );
VOID_FUNC      CNM_process_mo_release_complete_request( IMH_T * );
VOID_FUNC      CNM_process_mo_modify_request( IMH_T * );
VOID_FUNC      CNM_process_SRVCC_HO_complete_indication(IMH_T *);
VOID_FUNC      CNM_process_mt_modify_response( IMH_T * );
VOID_FUNC      CNM_process_mo_notify_request( void );
VOID_FUNC      CNM_process_user_conn_confirmation( IMH_T * );
VOID_FUNC      CNM_process_mt_call_setup_completion( IMH_T * );
VOID_FUNC      CNM_process_mo_call_request( IMH_T * );
VOID_FUNC      CNM_process_mt_call_answer( IMH_T * );
VOID_FUNC      CNM_process_mo_start_dtmf_request( MNCC_START_DTMF_REQ_T * );
VOID_FUNC      CNM_process_mo_stop_dtmf_request( MNCC_STOP_DTMF_REQ_T * );
VOID_FUNC      CNM_process_t336_expiry( connection_id_T connection_id );
VOID_FUNC      CNM_process_t337_expiry( connection_id_T connection_id );
VOID_FUNC      CNM_process_t324_expiry( connection_id_T connection_id );
VOID_FUNC      CNM_process_mo_user_user_data_request( IMH_T * );
VOID_FUNC      CNM_process_mo_modify_accept( IMH_T * );
VOID_FUNC      CNM_process_srvcc_handover_fail_req( IMH_T * );
VOID_FUNC      CNM_stop_timer( timer_id_T, connection_id_T  );
VOID_FUNC      CNM_cc_start_timer( timer_id_T, connection_id_T );
VOID_FUNC      CNM_start_timer( timer_id_T, connection_id_T, byte
#if defined(FEATURE_DUAL_SIM )
                              , sys_modem_as_id_e_type
#endif
                                                       );
VOID_FUNC      CNM_read_dtmf_queue(connection_id_T );
VOID_FUNC      CNM_clear_dtmf_queue( connection_id_T );   
VOID_FUNC      CNM_process_message_content( connection_id_T,
                                          mmcnm_data_ind_T * ,
                                          byte );
boolean           CNM_perform_check_on_IEs(byte message_type, connection_id_T connection_id, byte *error_cause);
VOID_FUNC      CNM_send_status( connection_id_T, byte, byte );  

boolean        CNM_check_for_protocol_errors( mmcnm_data_ind_T *);
   
VOID_FUNC      CNM_manage_cnm_service_requests( CNMServiceType_T,
                                             connection_id_T,
                                             IMH_T * );
boolean        CNM_setup_MNCC_REJ_IND( MNCC_REJ_IND_T *,
                                       connection_id_T,
                                       reject_type_enum_T   rej_type,
                                       reject_cause_T       rej_value,
                                       byte *p_cause );
boolean        CNM_check_for_establish_ind(mmcnm_data_ind_T *);

boolean        CNM_pop_cnm_data_queue(mmcnm_data_req_T *);
VOID_FUNC      CNM_add_to_cnm_data_queue(mmcnm_data_req_T *);
VOID_FUNC      CNM_manage_cnm_data_queue(mmcnm_data_req_T *);
VOID_FUNC      CNM_init_cnm_data_queue( void );
VOID_FUNC      CNM_init_transaction_information( void );

VOID_FUNC      CNM_route_CNM_messages( IMH_T *);
VOID_FUNC      CNM_set_state_to_U0( connection_id_T );
VOID_FUNC      CNM_init_sms_request_queue ( void );
VOID_FUNC      CNM_add_to_sms_request_queue ( cp_data_msg_T *, 
                                             connection_id_T );
boolean        CNM_read_sms_request_queue ( cp_data_msg_T *, connection_id_T *);
VOID_FUNC      CNM_process_mt_sms ( IMH_T * , connection_id_T );
byte           CNM_process_received_short_message ( IMH_T * , connection_id_T );
VOID_FUNC      CNM_process_mo_sms ( IMH_T * , connection_id_T  );
byte           CNM_process_mo_sms_request ( IMH_T * , connection_id_T );
VOID_FUNC      CNM_check_for_pending_message (  void );
byte           CNM_send_rp_message ( IMH_T * , connection_id_T );
byte           CNM_send_cp_data ( connection_id_T );
byte           CNM_process_abort_request ( connection_id_T );
byte           CNM_process_sms_release_message ( connection_id_T );
byte           CNM_process_received_cp_message ( IMH_T * , connection_id_T );
VOID_FUNC      CNM_send_cp_user_data ( connection_id_T );
VOID_FUNC      CNM_send_cp_ack ( connection_id_T );
VOID_FUNC      CNM_send_error_indication_to_rp_layer ( connection_id_T, byte );
VOID_FUNC      CNM_send_cp_error ( connection_id_T , byte );
VOID_FUNC      CNM_send_cpack_indication_to_rp_layer ( connection_id_T );
boolean        CNM_check_for_last_connection( byte
#if defined(FEATURE_DUAL_SIM )
                                            , sys_modem_as_id_e_type 
#endif
                                            );
boolean        CNM_check_if_any_service_object_exists( void );
boolean        CNM_SMS_process_message_type ( mmcnm_data_ind_T * );
VOID_FUNC      CNM_SMS_process_SMS ( IMH_T * );
boolean        CNM_SS_process_message_type ( mmcnm_data_ind_T * );
VOID_FUNC      CNM_SS_process_supplementary_services ( IMH_T * );
VOID_FUNC      CNM_start_mo_ss_request ( IMH_T * );
VOID_FUNC      CNM_process_mo_facility ( IMH_T * ) ;
VOID_FUNC      CNM_process_mt_facility(  IMH_T *);
VOID_FUNC      CNM_mo_end_ss_request(  IMH_T *);
VOID_FUNC      CNM_start_mt_ss_request(  IMH_T *);
VOID_FUNC      CNM_setup_release_complete_message ( release_complete_msg_T * ,
                                                   byte );
VOID_FUNC      CNM_send_release_complete ( connection_id_T,
                                          byte,
                                          byte,
                                          ie_facility_T * );
dword          CNM_get_timeout( timer_id_T );

VOID_FUNC      CNM_send_reject_message( cnm_request_id_T *,
                                        reject_type_enum_T   rej_type,
                                        reject_cause_T       rej_value );

VOID_FUNC      CNM_log_message(IMH_T *message_p);
VOID_FUNC      CNM_log_message_type( byte ti_pd,byte m_ref);
VOID_FUNC      CNM_log_CNM_service_type( byte CNM_service );
VOID_FUNC      CNM_log_rp_mti( byte mti);
VOID_FUNC      CNM_log_event_calls_statistics(connection_id_T connection_id, byte curr_state, byte next_state);

boolean        CNM_active_SMS(byte *ti);

call_type_for_rrc_est_req_T CNM_return_calltype(void);

boolean        CNM_is_GW_MT_SMS_in_progress(void);

gs_status_T    cnm_internal_send_routine( void *msg_p );

#endif
