#ifndef MN_CC_V_H
#define MN_CC_V_H

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_cc.h_v   1.23   10 Jul 2002 13:22:34   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/inc/mn_cc_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

26-Mar-2001  VT     Added boolean param reserved in cc_transaction_information_T
                      for cm_reserve_connection_id()

5/22/01      CD     Changed declaration of mn_cnm_cmd_q to extern to avoid
                    lint error

7/12/01      CD     Transferred data from mn.h

7/18/01      CD     Transferred external declaration of Multicall_status 
                    from mm_cc_processing.c

                    Transferred external declaration of tch_user_connections_store 
                    from mn_process_cnm_cc_msgs.c

                    Transferred external declaration of mn_cnm_cmd_q to mn_cnmtask.h

8/01/01      CD     Included cc_iei.h

8/6/01       CD     Added mn_call_information_T, MN_CC_MODIFY_data_T and
                    MNCC_CALL_PROC_data_T

08/16/01     AB     Renamed MMI references to CM.

8/21/01      CD     Moved all external declarations to mnglobal.h

9/05/01      CD     Added enum for call type 

24-Oct-2001  VT     DTMF: Moved dtmf_queue, dtmf_queue_read_ptr & 
                      dtmf_queue_write_ptr from cnm_cc_dtmf.c into this file 
                      inside cc_transaction_information_T as we have one DTMF
                      queue per connection_id now.

                      Also moved define for MAXNO_DTMF_QUEUE_ENTRIES from 
                        cnm_cc_dtmf.c to this file

25-Oct-2001  VT     DTMF: Changed dtmf_queue_read_ptr to dtmf_queue_read_index
                      and dtmf_queue_write_ptr to dtmf_queue_write_index (as 
                      the ptr in them is misleading).

26-Oct-2001  VT     DTMF: Moved the definition of dtmf_queue_entry_T to this
                      file from cmlib.h

                    Moved the definition of cc_transaction_information_T to
                       the bottom of this file as it now depends on 
                       dtmf_que_entry_T, which in turn depends on 
                       MNCC_START/STOP_DTMF_REQ

20-Feb-2002   AB    Removed the AUX_BUILD_REQ_FRM_MPTY_CALL_HELD as its value 
                      and processing is the same as 
                      AUX_BUILD_REQUEST_FROM_CALL_HELD.

12-March-2002 AB    Added CALL DEFLECTION, and CALL_TRANSFER request event
                      that are used in MN_CC timer event and message structure.  
                    Also changed DISC_REQ interface to piggy bag CD invoke 
                      components.

8-April-2002  AB    Added MNCC_NOTIFY_DEFERRED_IND message type 
                      and related message structure.

6-May-2002    AB    Added comments for CCE_CALL_DEFLECTION_REQ and 
                      CCE_CALL_TRANSFER_REQ request events.

06/03/02      CD    Merged in changes from revision 1.19.1.0
                    Added enum for mn_vocoder_ctrl_e_type.

07/02/02      CD    Added space for cause_of_no_CLI to the MNCC_SETUP_IND
                    data field, and the mn_call_information global data

                    For consitency, added cause of no CLI fields to 
                    MNCC_SETUP_data_T.  This structure does not seem to be used.

09/04/02      AB    Added redirected number and subaddress into the 
                    mn_call_information global data.

09/16/02      AB    Added new primitives MNCC_REL_COMPL_REQ and associated
                    message structure, MNCC_REL_COMPL_REQ_T.

10/12/02      AB    Added SYNC reason to MMCC_SYNC_IND_T to support 
                    inter-RAT handoff. 

                    Added MN_VOCODER_SWITCH_TO_GSM & WCDMA to 
                    mn_vocoder_ctrl_e_type.

02/12/03      AB    Replaced the array element in Multicall_Status_T structure
                    with predefined literal per Lint.
 
04/06/02      AB    Replaced the lower_layer_cause_T with new recject type
                    and cause in MNCC_REL_IND_T.

04/16/02      AB    Added mn_deferred_end_call_ind_T.

11/02/04      AB    Added SS Call related User-to-User Signaling feature.

03/22/05      AB    Added mn_dual_service_T to support the CC dual service 
                    selection feature.

05/10/05      HS    Added support for FEATURE_CCBS

11/09/05      AB    Added support for FEATURE_EMERGENCY_CATEGORY

02/13/06      NR    Answering the incoming call for abnormal cases in case of 2_SEND request

03/05/06      NR    Adding changes for WB_AMR

21/03/07      NR    Adding new fiels and enum for tracking SS pending request
===========================================================================*/




#include "mmcp_variation.h"
#include "cc_iei_v.h"
#include "ms_cc_mm_v.h"
#include "mn_ss_v.h"
#include "mn_cm.h"
#include "mn_cc.h"
#include "cc_state.h"









typedef  struct
{
    byte      call_activity;
    boolean   call_is_mpty;
    boolean   split_mpty;
    byte      id_index;
    byte      connection_id[MN_MAXNO_CONNECTION_IN_SESSION];
} Multicall_Status_T;


typedef  struct
{ boolean                OTA_channel_connected;
  boolean                speech_channel_connected;
  boolean                data_channel_connected;
  byte                   tch_rate;
  byte                   subchannel;
  channel_mode_T         channel_mode;
#ifdef FEATURE_VOC_AMR_WB 
  umts_codecs_list_T                   umts_codec;
#endif  /*end of FEATURE_VOC_AMR_WB */
#ifdef FEATURE_CS_VOICE_OVER_HSPA
  umts_wcdma_cs_voice_bearer_enum_T cs_voice_type;
#endif

}tch_user_connections_store_T;
 

typedef struct
{
    boolean                deferred;    /* last call of given RR connection */
    cm_mt_end_call_ind_T   end_call_ind;

} mn_deferred_end_call_ind_T;


typedef struct {                /* Handover notification */
    mn_client_T          client;
    mn_client_service_T  type;
    union {
        mn_cm_cb_T           aoc_response;
        mn_sync_notify_cb_T  sync_notify;
    } cb_function;
} mn_cb_services_T;


#ifdef FEATURE_UUS
typedef struct {
    boolean              present;
    byte                 type;
    boolean              activated;
    boolean              receiver_busy;   /* TRUE = not ready */
} mn_user_user_data_T;  /* queue */
#endif

typedef enum {
    DUAL_SERVICE_NONE,           /* non-Multical dual services */
    DUAL_SERVICE_IN_VOICE,
    DUAL_SERVICE_IN_DATA,
    DUAL_SERVICE_IN_MULTICALL
} mn_dual_service_T;

typedef  struct mn_call_information_tag
{
   CNMServiceType_T                CNM_service_type;
   boolean                         progress_indicator_present;
   cm_progress_indicator_T         progress_indicator;
   byte                            repeat_indication_hlc;
   cm_high_layer_compatibility_T   high_layer_compatibility_1; 
   cm_high_layer_compatibility_T   high_layer_compatibility_2;
   byte                            repeat_indication_llc;
   cm_low_layer_compatibility_T    low_layer_compatibility_1; 
   cm_low_layer_compatibility_T    low_layer_compatibility_2;
   byte                            repeat_indicator_bc;
   cm_bearer_capability_T          bearer_capability_1; 
   cm_bearer_capability_T          bearer_capability_2; 
   cm_called_party_bcd_no_T        called_party_bcd_number;
   boolean                         called_party_bcd_number_present;
   cm_called_party_subaddress_T    called_party_subaddress;
   boolean                         called_party_subaddress_present;
   cm_calling_party_bcd_no_T       calling_party_bcd_number;
   boolean                         calling_party_bcd_number_present;
   cm_calling_party_subaddress_T   calling_party_subaddress;
   boolean                         calling_party_subaddress_present;
   cm_calling_party_bcd_no_T       redirect_party_bcd_number;
   boolean                         redirect_party_bcd_number_present;
   cm_calling_party_subaddress_T   redirect_party_subaddress;
   boolean                         redirect_party_subaddress_present;
   cm_connected_no_T               connected_number;
   boolean                         connected_number_present;
   cm_connected_subaddress_T       connected_subaddress;
   boolean                         connected_subaddress_present;
   boolean                         clir_suppression;
   boolean                         clir_invocation;
   boolean                         signal_present;
   byte                            SignalValue;
   byte                            active_bearer_capability;
   boolean                         mo_setup_pending;
   boolean                         mo_call_clear_pending;
   byte                            mo_release_indicator; /* This field stores the reason for disconnecting current call, for ex. CM_1_SEND_REL*/
   byte                            mt_call_accept_cause;
   int                             no_of_changes;
   byte                            TCH_channel_type;
   byte                            subchannel_number;
   boolean                         reverse_call_direction;
   ie_cm_cc_cause_T                cm_cc_cause;
   ie_cm_cc_cause_T                mo_release_cause_diag_packet; /* This stores the cause to be sent to DIAG when UE sent RELEASE is first call clearing message */
   ie_cm_cc_cause_T                call_reject_cause_diag_packet; /* This stores the cause to be sent to DIAG when CNM sends MNCC_REJ_IND to MN */
   cm_forward_cug_info_T           cm_forward_cug_info;
   uint8                           radio_access_bearer_id;  /* aka as stream_id for CS calls */
   cm_cause_of_no_CLI_T            cause_of_no_CLI;
   cm_alerting_pattern_T           alerting_pattern;
#ifdef FEATURE_UUS
   mn_user_user_data_T             user_user_data;
#endif

    mn_dual_service_T              dual_services;

#ifdef FEATURE_CCBS
    boolean                        ccbs_allowed;
#endif /* FEATURE_CCBS */

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
    cm_stream_identifier_T         stream_id; 
    cm_supported_codecs_list_T     supported_codecs_list; 
    boolean                        immediate_modify_ind; 
    cm_bearer_capability_T         backup_bearer_capability; 
    cm_network_cc_capability_T     network_cc_capability;
    boolean                        nw_initiated_ind; 
    boolean                        modifiy_in_progress; 
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

    cm_emergency_category_T        emergency_category;

    boolean                        offline;
    byte                           hold_request;
    byte                           ss_request_pending;
    boolean                        received_start_dtmf;
	byte                           cause_before_gsm_reestablishment;
#ifdef FEATURE_DUAL_SIM 
    sys_modem_as_id_e_type         as_id;
#endif
} mn_call_information_T;

typedef enum 
{
    MN_SS_ALT_NONE_AWAITED,
    MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED,
    MN_SS_ALT_HOLD_ACK_AWAITED,
    MN_SS_ALT_RETRIEVE_ACK_AWAITED,
    MN_SS_ALT_NACKED
} mn_alternate_procedure_status_e_type;

/**/
typedef struct
{
    boolean                                is_alternate_procedure;
    mn_alternate_procedure_status_e_type   status;
} mn_alternate_procedure_status_T;

/* Tags for mo_release_indicator */

#define   DEFAULT_REL        0x00   /* For MT release, etc */
#define   CM_1_X_SEND_REL    0x01
#define   CM_1_SEND_REL      0x02
#define   CM_ABORT_REL       0x03
#ifdef FEATURE_CCBS
#define   CM_RECALL_REL      0x04
#endif /* FEATURE_CCBS */

/* Tags for hold_request indicator */
#define   CM_2_SEND_HOLD      0x01
#define   CM_2_X_SEND_HOLD  0x02


enum
{
  CM_NONE_PENDING,
  CM_0_SEND_PENDING,
  CM_1_SEND_PENDING,
  CM_1_X_SEND_PENDING,
  CM_2_SEND_PENDING,
  CM_2_X_SEND_PENDING,
  CM_3_SEND_PENDING
};

/*  Structures added 29/01/96  */

/* basic invoke components structure */

/* Values for call_type  */

enum
{
   VOICE_CALL,
   DATA_CALL
};

/*
 * Enum for mn_vocoder_ctrl
 */

/* Values for call_activity */

enum
{
   CALL_INACTIVE,
   CALL_IN_PROGRESS,
   CALL_ACTIVE,
   CALL_HELD,
   CALL_WAITING,
   MO_CALL_PENDING
};

/* Values for CC auxiliary state */

#define AUX_IDLE_HOLD_AND_MPTY             (MPTY_IDLE+4*HOLD_IDLE)
#define AUX_BUILD_REQUEST_FROM_CALL_ACTIVE (MPTY_REQUEST+4*HOLD_IDLE)
#define AUX_CALL_IN_MPTY                   (MPTY_CALL_IN_MPTY+4*HOLD_IDLE)
#define AUX_SPLIT_REQUEST                  (MPTY_SPLIT_REQUEST+4*HOLD_IDLE)
#define AUX_HOLD_REQUEST                   (MPTY_IDLE+4*HOLD_REQUEST)
#define AUX_HOLD_MPTY_REQUEST              (MPTY_CALL_IN_MPTY+4*HOLD_REQUEST)
#define AUX_CALL_HELD                      (MPTY_IDLE+4*HOLD_CALL_HELD)
#define AUX_BUILD_REQUEST_FROM_CALL_HELD   (MPTY_REQUEST+4*HOLD_CALL_HELD)
#define AUX_MPTY_CALL_HELD                 (MPTY_CALL_IN_MPTY+4*HOLD_CALL_HELD)
#define AUX_RETRIEVE_REQUEST               (MPTY_IDLE+4*HOLD_RETRIEVE_REQUEST)
#define AUX_RETRIEVE_MPTY_REQUEST          (MPTY_CALL_IN_MPTY+4*HOLD_RETRIEVE_REQUEST)

/* Values for c_call_event */
/* These are SS Call related Timer event used in MN_CC_TIMER + invoke ID */
enum
{
   CCE_BUILD_MPTY_REQ,
   CCE_CALL_HOLD_REQ,
   CCE_BUILD_MPTY_CONF,
   CCE_BUILD_MPTY_REJ,
   CCE_HOLD_MPTY_REQ,
   CCE_SPLIT_MPTY_REQ,
   CCE_SPLIT_MPTY_REJ,
   CCE_SPLIT_MPTY_CONF,
   CCE_CALL_HOLD_CONF,
   CCE_CALL_HOLD_REJ,
   CCE_HOLD_MPTY_REJ,
   CCE_HOLD_MPTY_CONF,
   CCE_CALL_RETRIEVE_REQ,
   CCE_RETRIEVE_MPTY_REQ,
   CCE_CALL_RETRIEVE_REJ,
   CCE_CALL_RETRIEVE_CONF,
   CCE_RETRIEVE_MPTY_REJ,
   CCE_RETRIEVE_MPTY_CONF,
   CCE_CALL_DEFLECTION_REQ,   /* T(CD) SS Call Releated Timer, MN_CC_TIMER + invoke_id */
   CCE_CALL_TRANSFER_REQ,     /* T(ECT) */
   CCE_CALL_ENDED
};

/************************************************************************
*                                                                       *
*                       MNCC Message Identity values                    *
*                                                                       *
*************************************************************************
* To be loaded into MessageId field of Message Header
*/

enum 
{

/* MN to CC primitives */

   MNCC_ALERT_REQ,
   MNCC_CALL_CONF_REQ,
   MNCC_CALL_HOLD_REQ,
   MNCC_CALL_RETRIEVE_REQ,
   MNCC_DISC_REQ,
   MNCC_FACILITY_REQ,
   MNCC_REJ_REQ,
   MNCC_REL_REQ,
   MNCC_REL_COMPL_REQ,
   MNCC_MODIFY_REJ,
   MNCC_MODIFY_REQ,
   MNCC_MODIFY_RES,
   MNCC_NOTIFY_REQ,
   MNCC_SETUP_COMPL_REQ,
   MNCC_SETUP_COMPL_RES,
   MNCC_SETUP_REQ,
   MNCC_SETUP_RES,
   MNCC_START_DTMF_REQ,
   MNCC_STOP_DTMF_REQ,
   MNCC_USER_USER_DATA_REQ,
   MNCC_MODIFY_ACCEPT,
   MNCC_SRVCC_HANDOVER_FAIL_REQ,

/* CC to MN Primitives */

   MNCC_ALERT_IND       = 0x80,
   MNCC_CALL_HOLD_CNF,
   MNCC_CALL_HOLD_REJ,
   MNCC_CALL_PROC_IND,
   MNCC_CALL_RETRIEVE_CNF,
   MNCC_CALL_RETRIEVE_REJ,
   MNCC_DISC_IND,
   MNCC_ERR_IND,
   MNCC_FACILITY_IND,
   MNCC_MODIFY_CNF,
   MNCC_MODIFY_IND,
   MNCC_NOTIFY_IND,
   MNCC_NOTIFY_DEFERRED_IND,
   MNCC_PROGRESS_IND,
   MNCC_REEST_IND,
   MNCC_REJ_IND,
   MNCC_REL_CNF,
   MNCC_REL_IND,
   MNCC_SETUP_CNF,
   MNCC_SETUP_COMPL_IND,
   MNCC_SETUP_IND,
   MNCC_START_DTMF_CNF,
   MNCC_STOP_DTMF_CNF,
   MNCC_SYNC_IND,
   MNCC_RAB_REL_IND,
   MNCC_CS_SIG_CONN_REL_IND,   
#ifdef FEATURE_UUS
   MNCC_USER_USER_DATA_IND,
   MNCC_USER_DATA_DEFERRED_IND,
   MNCC_CONGESTION_IND,
#endif
#ifdef FEATURE_CCBS
   MNCC_RECALL_IND,
   MNCC_CC_EST_IND,
#endif /* FEATURE_CCBS */
   MNCC_SRVCC_HANDOVER_COMPLETE_IND,

/*   SS to MN Primitives */

    SS_to_MN_primitives       = 0xa0,

/*  SMS to MN primitives  */

    SMS_to_MN_primitives     = 0xc0

};

/*
 * define max size of forward cug info facility IE =
 * Facility IE +
 * Length +
 * component type +
 * component length +
 * invoke id tag +
 * invoke id length
 * invoke id +
 * op code tag +
 * op code length +
 * op code +
 * Sequence tag +
 * sequence length +
 * cug index tag +
 * cug index length +
 * cug index (2 bytes) +
 * suppress pref CUG tag +
 * suppress pref CUG length (0) +
 * suppress OA tag +
 * suppress OA length (0)
 * 
 */



/************************************************************************
*                                                                       *
*                       MN to CC PRIMITIVES                             *
*                       -------------------                             *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                       MNCC_ALERT_REQ                                  *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_CALL_CONF_REQ                              *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_CALL_HOLD_REQ                              *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_CALL_RETRIEVE_REQ                          *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_DISC_REQ                           *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_FACILITY_REQ                       *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_REJ_REQ                            *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_REL_REQ                            *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_REL_COMPL_REQ                      *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   connection_id_T    connection_id;
   byte               pd;
   byte               cause;
   ie_facility_T      *p_facility;
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type       as_id;   
#endif
} MNCC_REL_COMPL_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_REQ                         *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_RES                         *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_SETUP_COMPL_REQ                    *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_SETUP_COMPL_RES                    *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_SETUP_RES                          *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_SETUP_REQ                          *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_START_DTMF_REQ                     *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_STOP_DTMF_REQ                      *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_USER_USER_DATA_REQ                 *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_ACCEPT                      *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                       CC to MN PRIMITIVES                             *
*                       -------------------                             *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                       MNCC_ALERT_IND                                  *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                       MNCC_CALL_HOLD_CNF                              *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                       MNCC_CALL_HOLD_REJ                              *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_CALL_PROC_IND                      *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_CALL_RETRIEVE_CNF                          *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                       MNCC_CALL_RETRIEVE_REJ                          *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_DISC_IND                           *
*                                                                       *
*************************************************************************
*/



/************************************************************************
*                                                                       *
*                       MNCC_ERR_IND                                    *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_FACILITY_IND                       *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_CNF                         *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_IND                         *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_NOTIFY_IND                                 *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                       MNCC_NOTIFY_DEFERRED_IND                        *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                         message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T               connection_id;
   ie_cm_notify_ss_T             *notification_indicator;

} MNCC_NOTIFY_DEFERRED_IND_T;

#ifdef FEATURE_UUS
/************************************************************************
*                                                                       *
*                       MNCC_NOTIFY_DEFERRED_IND                        *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                         message_header;
   sys_modem_as_id_e_type        as_id;
   cm_mt_user_data_ind_T         *user_data_ind;
} MNCC_USER_DATA_DEFERRED_IND_T;
#endif

/************************************************************************
*                                                                       *
*                       MNCC_PROGRESS_IND                               *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_REEST_IND                                  *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_REJ_IND                            *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_REL_CNF                            *
*                                                                       *
*************************************************************************
*/
/************************************************************************
*                                                                       *
*                               MNCC_REL_IND                            *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_SETUP_CNF                          *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_SETUP_COMPL_IND                            *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_SETUP_IND                          *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                               MNCC_START_DTMF_CNF                     *
*                                                                       *
*************************************************************************
*/
/************************************************************************
*                                                                       *
*                               MNCC_STOP_DTMF_CNF                      *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_SYNC_IND                           *
*                                                                       *
*************************************************************************
*/


typedef struct 
{
   IMH_T                     message_header;
   sys_modem_as_id_e_type    as_id;
} MNCC_RAB_REL_IND_T;


#ifdef FEATURE_UUS
/************************************************************************
*                                                                       *
*                               MNCC_USER_USER_DATA_IND                 *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_CONGESTION_IND                     *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type    as_id;
   connection_id_T    connection_id;
   byte               data[sizeof(ie_congestion_control_T)];
} MNCC_CONGESTION_IND_T;
#endif

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type    as_id;
} MNCC_CS_SIG_CONN_REL_IND_T;


#ifdef FEATURE_CCBS
/************************************************************************
*                                                                       *
*                               MNCC_RECALL_IND                         *
*                                                                       *
*************************************************************************
*/
typedef struct  {
   IMH_T              message_header;
   sys_modem_as_id_e_type    as_id;
   connection_id_T    connection_id;
   byte               data [sizeof(ie_recall_type_T)];
} MNCC_RECALL_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_CC_EST_IND                         *
*                                                                       *
*************************************************************************
*/
typedef struct  {
   IMH_T              message_header;
   sys_modem_as_id_e_type    as_id;
   connection_id_T    connection_id;
   byte               data [sizeof(ie_setup_container_T)];
} MNCC_CC_EST_IND_T;

#endif /* FEATURE_CCBS */

/*
 * definition of data global to MN and CC
 */

#define MAXNO_DTMF_QUEUE_ENTRIES     32

/*
 * type definition required for the DTMF queue access routines
 */

typedef union dtmf_queue_tag
{
   MNCC_START_DTMF_REQ_T         start_dtmf_queue_entry;
   MNCC_STOP_DTMF_REQ_T          stop_dtmf_queue_entry;
} dtmf_queue_entry_T;

typedef struct cc_transaction_information_tag
{
   cc_state_T                cc_state;
   cc_dtmf_state_e_type      cc_dtmf_state;
   CNMServiceType_T          CNM_service_type;
   hold_auxiliary_state_T    hold_auxiliary_state;
   mpty_auxiliary_state_T    mpty_auxiliary_state;
   byte                      cc_cause[GSM_CAUSE_SIZE];
   boolean                   T308_expired;
   boolean                   reserved;    /* used by CM to RESERVED & UN-RESERVED a connection ID */

   /*
    * The queue may contain STOP_DTMF_REQ messages or START_DTMF_REQ entries
    * Type of message in queue must be determined from the message
    * header information.
    */
   byte                      dtmf_queue_read_index;
   byte                      dtmf_queue_write_index;
   dtmf_queue_entry_T        dtmf_queue[MAXNO_DTMF_QUEUE_ENTRIES];

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
   boolean                   repeat; /* call retry/repeat on STATUS #100 */
#endif

} cc_transaction_information_T;


/*                                             
 *  MN Buffers Management Services              
 *                                                                   
 */
typedef struct MN_qlink_S {
    struct MN_qlink_S  *prev_ptr;
    struct MN_qlink_S  *next_ptr;
    void               *item;
} MN_qlink_T;

typedef struct MN_q_S
{
    MN_qlink_T   *head;
    MN_qlink_T   *tail;
    byte        count;
} MN_q_T; 

void   MN_q_init(MN_q_T *queue);
void   MN_q_put (MN_q_T *queue, MN_qlink_T *link);
void   *MN_q_get(MN_q_T *queue);  
MN_qlink_T *MN_q_link(void *item, MN_qlink_T *link);

#endif

