#ifndef mn_cc_h
#define mn_cc_h

/*===========================================================================
                     COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                     EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_cc.h_v   1.23   10 Jul 2002 13:22:34   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mn_cc.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

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
#include "sys_cnst.h"
#include "cc_iei.h"
#include "ms_cc_mm.h"
#include "mn_ss.h"

typedef union
{
      ie_clir_invocation_T clir_invocation;
      ie_clir_suppression_T clir_suppression;
} ie_clir_instruction_T;


typedef struct
{
    connection_id_T                 connection_id;
    cm_bearer_capability_T          cm_bearer_capability;
    byte                            repeat_indication_llc;
    cm_low_layer_compatibility_T    cm_low_layer_compatibility;
    byte                            repeat_indication_hlc;
    cm_high_layer_compatibility_T   cm_high_layer_compatibility;
    boolean                         cause_present;
    ie_cm_cc_cause_T                cm_cc_cause;
    boolean                         reverse_call_direction;

    boolean                         immediate_modify_ind;
    boolean                         nw_initiated_ind; 

} MNCC_MODIFY_data_T;

typedef struct
{
    connection_id_T            connection_id;
    byte                       repeat_indication_bc;
    cm_bearer_capability_T     bearer_capability_1;
    cm_bearer_capability_T     bearer_capability_2;

    cm_network_cc_capability_T network_cc_capability;

} MNCC_CALL_PROC_data_T;




typedef struct
{
   connection_id_T                   connection_id;
   CNMServiceType_T                  CNM_service_type;
   boolean                           progress_indicator_present;
   byte                              progress_indicator;
   byte                              repeat_indication_hlc;
   ie_high_layer_compatibility_T     high_layer_compatibility_1;
   ie_high_layer_compatibility_T     high_layer_compatibility_2;
   byte                              repeat_indication_llc;
   ie_low_layer_compatibility_T      low_layer_compatibility_1;
   ie_low_layer_compatibility_T      low_layer_compatibility_2;
   byte                              repeat_indication_bc;
   ie_bearer_capability_T            bearer_capability_1;
   ie_bearer_capability_T            bearer_capability_2;
   boolean                           called_party_bcd_number_present;
   ie_called_party_bcd_no_T          called_party_bcd_number;
   boolean                           called_party_subaddress_present;
   ie_called_party_subaddress_T      called_party_subaddress;
   boolean                           calling_party_bcd_number_present;
   ie_calling_party_bcd_no_T         calling_party_bcd_number;
   boolean                           calling_party_subaddress_present;
   ie_calling_party_subaddress_T     calling_party_subaddress;
   boolean                           clir_suppression;
   boolean                           clir_invocation;
   boolean                           signal_present;
   byte                              SignalValue;
   boolean                           cause_of_no_CLI_present;
   ie_cause_of_no_CLI_T              cause_of_no_CLI;
   boolean                           alerting_pattern_present;
   ie_alerting_pattern_T             alerting_pattern;
} MNCC_SETUP_data_T;


/*  Structures added 29/01/96  */
typedef struct
{
    byte       component_type;
    byte       invoke_id;
    boolean    invoke_id_present;
    boolean    linked_id_present;
    byte       linked_id;
    byte       operational_code;
    boolean    operational_code_present;
    byte       error_code;
    byte       problem_tag;
    byte       problem_code;
    byte       parameter_length;
    byte       parameters[MAX_SS_PARAMETER_LENGTH];
} facility_component_data_T;

/* basic invoke components structure */
typedef struct
{
    byte       component_type;
    byte       invoke_id_tag;
    boolean    invoke_id_length;
    boolean    invoke_id;
    byte       operational_code_tag;
    boolean    operational_code_length;
    byte       operational_code;
    byte       parameters[MAX_SS_PARAMETER_LENGTH/MAX_COMPONENTS];  /* for 1 component */
} facility_invoke_component_data_T;

typedef struct
{
    connection_id_T     connection_id;
    facility_component_data_T facility_component_data[MAX_COMPONENTS];
} MN_FACILITY_DATA_T;


/*
 * Enum for mn_vocoder_ctrl
 */
typedef enum mn_vocoder_ctrl_e {
    MN_VOCODER_OFF = 0x00, 

    MN_VOCODER_GSM_OFF, 
    MN_VOCODER_WCDMA_OFF, 

    MN_VOCODER_SWITCH,   /* same mode */
    MN_VOCODER_SWITCH_TO_GSM,
    MN_VOCODER_SWITCH_TO_WCDMA
} mn_vocoder_ctrl_e_type;


/************************************************************************
*                                                                       *
*                       MNCC Message Identity values                    *
*                                                                       * 
*************************************************************************
*/ 
#define SIZEOF_FORWARD_CUG_INFO_FACILITY  20

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
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
} MNCC_ALERT_REQ_T;


/************************************************************************
*                                                                       *
*                       MNCC_CALL_CONF_REQ                              *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   byte               data[(sizeof(ie_repeat_T) +    /* the repeat indicator */
                         2*sizeof(ie_bearer_capability_T) + GSM_CAUSE_SIZE)

                         +
                         sizeof(ie_stream_id_T) + 
                         sizeof(ie_codecs_list_T)

                          ];
} MNCC_CALL_CONF_REQ_T;


/************************************************************************
*                                                                       *
*                       MNCC_CALL_HOLD_REQ                              *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
} MNCC_CALL_HOLD_REQ_T;


/************************************************************************
*                                                                       *
*                       MNCC_CALL_RETRIEVE_REQ                          *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
} MNCC_CALL_RETRIEVE_REQ_T;


/************************************************************************
*                                                                       *
*                               MNCC_DISC_REQ                           *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   byte               data[sizeof(facility_invoke_component_data_T) + GSM_CAUSE_SIZE + 3];
} MNCC_DISC_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_FACILITY_REQ                       *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   byte               data[(
                      sizeof(ie_facility_T) +
                      sizeof(ie_ss_version_ind_T))];
} MNCC_FACILITY_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_REJ_REQ                            *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   byte               cause[GSM_CAUSE_SIZE];
} MNCC_REJ_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_REL_REQ                            *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   byte               data[(2*GSM_CAUSE_SIZE) + sizeof(ie_user_user_T)

                           + sizeof(ie_facility_T)
                           + 3/*SS version->sizeof(ie_ss_version_ind_T) too big*/

                          ];
} MNCC_REL_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_REL_COMPL_REQ                      *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_REQ                         *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                    message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T          connection_id;
   byte                     data
                            [(
                               sizeof(ie_bearer_capability_T) +
                               sizeof(ie_low_layer_compatibility_T) +
                               sizeof(ie_high_layer_compatibility_T) +
                               sizeof(ie_reverse_call_setup_dir_T)
                            )];
} MNCC_MODIFY_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_RES                         *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                    message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T          connection_id;
   byte                     data
                            [(
                               sizeof(ie_bearer_capability_T) +
                               GSM_CAUSE_SIZE +
                               sizeof(ie_low_layer_compatibility_T) +
                               sizeof(ie_high_layer_compatibility_T) +
                               sizeof(ie_reverse_call_setup_dir_T)
                            )];
} MNCC_MODIFY_RES_T;


/************************************************************************
*                                                                       *
*                               MNCC_SETUP_COMPL_REQ                    *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
} MNCC_SETUP_COMPL_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_SETUP_COMPL_RES                    *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
} MNCC_SETUP_COMPL_RES_T;


/************************************************************************
*                                                                       *
*                               MNCC_SETUP_RES                          *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   byte               data[sizeof(
                      ie_connected_subaddress_T)];
} MNCC_SETUP_RES_T;

/************************************************************************
*                                                                       *
*                               MNCC_SETUP_REQ                          *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   CNMServiceType_T   CNM_service_type;
   byte               data
                      [
                         sizeof(ie_repeat_T) +
                         2*sizeof(ie_bearer_capability_T) +
                         sizeof(ie_calling_party_subaddress_T) +
                         sizeof(ie_called_party_bcd_no_T) +
                         sizeof(ie_called_party_subaddress_T) +
                         sizeof(ie_repeat_T) +
                         2* sizeof(ie_low_layer_compatibility_T) +
                         sizeof(ie_repeat_T) +
                         2*sizeof(ie_high_layer_compatibility_T) +
						 sizeof(ie_user_user_T)+
                         sizeof(ie_clir_instruction_T) +
                         SIZEOF_FORWARD_CUG_INFO_FACILITY

                         + MAX(10,(sizeof(ie_stream_id_T) + sizeof(ie_codecs_list_T)))
                         + 3
                      ];

} MNCC_SETUP_REQ_T;


/************************************************************************
*                                                                       *
*                               MNCC_START_DTMF_REQ                     *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                 message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T       connection_id;
   ie_keypad_facility_T  keypad_facility;
} MNCC_START_DTMF_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_STOP_DTMF_REQ                      *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                 message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T       connection_id;
} MNCC_STOP_DTMF_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_USER_USER_DATA_REQ                 *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   byte               data[(sizeof(ie_user_user_T) +
                            sizeof(ie_more_data_T))];
} MNCC_USER_USER_DATA_REQ_T;

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_ACCEPT                      *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
   boolean            accept;
} MNCC_MODIFY_ACCEPT_T;

/************************************************************************
*                                                                                     *
*                               MNCC_SRVCC_HANDOVER_FAIL_REQ               *
*                                                                                     *
*************************************************************************
*/
typedef struct
{
   IMH_T                        message_header;
   sys_modem_as_id_e_type          as_id;
}
MNCC_SRVCC_HANDOVER_FAIL_REQ_T;

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
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type        as_id;
   connection_id_T    connection_id;
} MNCC_ALERT_IND_T;

/************************************************************************
*                                                                       *
*                       MNCC_CALL_HOLD_CNF                              *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type     as_id;
   connection_id_T    connection_id;
} MNCC_CALL_HOLD_CNF_T;

/************************************************************************
*                                                                       *
*                       MNCC_CALL_HOLD_REJ                              *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type     as_id;
   connection_id_T    connection_id;
} MNCC_CALL_HOLD_REJ_T;


/************************************************************************
*                                                                       *
*                               MNCC_CALL_PROC_IND                      *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                    message_header;
   sys_modem_as_id_e_type     as_id;
   connection_id_T          connection_id;
   byte                     data
                            [
                               sizeof(ie_repeat_T) +
                               (2*sizeof(ie_bearer_capability_T))

                               +
                               sizeof(ie_network_cc_capability_T)

                            ];
} MNCC_CALL_PROC_IND_T;

/************************************************************************
*                                                                       *
*                       MNCC_CALL_RETRIEVE_CNF                          *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type     as_id;
   connection_id_T    connection_id;
} MNCC_CALL_RETRIEVE_CNF_T;

/************************************************************************
*                                                                       *
*                       MNCC_CALL_RETRIEVE_REJ                          *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type     as_id;
   connection_id_T    connection_id;
} MNCC_CALL_RETRIEVE_REJ_T;


/************************************************************************
*                                                                       *
*                               MNCC_DISC_IND                           *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type     as_id;
   connection_id_T    connection_id;
   byte               data[(
         GSM_CAUSE_SIZE +

         sizeof(ie_allowed_actions_T) +

         sizeof(ie_progress_indicator_T) )];
} MNCC_DISC_IND_T;


/************************************************************************
*                                                                       *
*                       MNCC_ERR_IND                                    *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type       as_id;
} MNCC_ERR_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_FACILITY_IND                       *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T    connection_id;
   byte               data[sizeof(ie_facility_T)];
} MNCC_FACILITY_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_CNF                         *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                    message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T          connection_id;
   byte                     data
                            [(
                               GSM_CAUSE_SIZE +
                               sizeof(ie_bearer_capability_T) +
                               sizeof(ie_low_layer_compatibility_T) +
                               sizeof(ie_high_layer_compatibility_T) +
                               sizeof(ie_reverse_call_setup_dir_T)
                            )];
} MNCC_MODIFY_CNF_T;

/************************************************************************
*                                                                       *
*                               MNCC_MODIFY_IND                         *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                    message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T          connection_id;
   byte                     data
                            [(
                               sizeof(ie_bearer_capability_T) +
                               sizeof(ie_low_layer_compatibility_T) +
                               sizeof(ie_high_layer_compatibility_T) +
                               sizeof(ie_reverse_call_setup_dir_T) 

                               +
                               sizeof(ie_immediate_modify_ind_T) +
                               sizeof(ie_nw_initiated_svc_upgrade_ind_T)

                            )];
} MNCC_MODIFY_IND_T;

/************************************************************************
*                                                                       *
*                       MNCC_NOTIFY_IND                                 *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                         message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T               connection_id;
   ie_notification_indicator_T   notification_indicator;
} MNCC_NOTIFY_IND_T;

/************************************************************************
*                                                                       *
*                       MNCC_NOTIFY_DEFERRED_IND                        *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_NOTIFY_DEFERRED_IND                        *
*                                                                       *
*************************************************************************
*/


/************************************************************************
*                                                                       *
*                       MNCC_PROGRESS_IND                               *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                         message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T               connection_id;
   ie_progress_indicator_T       progress_indicator;
} MNCC_PROGRESS_IND_T;

/************************************************************************
*                                                                       *
*                       MNCC_REEST_IND                                  *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type       as_id;
} MNCC_REEST_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_REJ_IND                            *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T      connection_id;
   ie_cm_cc_reject_T    rej_cause; /* establishment reject cause */ 
   lower_layer_cause_T  cause;
   byte                 data[ GSM_CAUSE_SIZE ];  /* normal call clearing cause */
} MNCC_REJ_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_REL_CNF                            *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T      connection_id;
   lower_layer_cause_T  cause;
   byte                 data[ GSM_CAUSE_SIZE ];
} MNCC_REL_CNF_T;

/************************************************************************
*                                                                       *
*                               MNCC_REL_IND                            *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T      connection_id;
   lower_layer_cause_T  cause;
   byte                 data[ (2*GSM_CAUSE_SIZE) ];  /* OTA release cause */
} MNCC_REL_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_SETUP_CNF                          *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T    connection_id;
   byte               data
                      [(
                         sizeof(ie_connected_number_T) +
                         sizeof(ie_connected_subaddress_T)
                      )];
} MNCC_SETUP_CNF_T;

/************************************************************************
*                                                                       *
*                       MNCC_SETUP_COMPL_IND                            *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T    connection_id;
} MNCC_SETUP_COMPL_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_SETUP_IND                          *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T    connection_id;
   byte               data
                      [(
                         sizeof(ie_repeat_T) +
                         (2*sizeof(ie_bearer_capability_T)) +
                         sizeof(ie_signal_T) +
                         sizeof(ie_calling_party_bcd_no_T) +
                         sizeof(ie_calling_party_subaddress_T) +
                         sizeof(ie_called_party_bcd_no_T) +
                         (2*sizeof(ie_called_party_subaddress_T)) +
                         sizeof(ie_redirecting_party_bcd_no_T) +
                         sizeof(ie_repeat_T) +
                         (2* sizeof(ie_low_layer_compatibility_T)) +
                         sizeof(ie_repeat_T) +
                         (2*sizeof(ie_high_layer_compatibility_T)) +
                         sizeof(ie_cause_of_no_CLI_T)  
                        
                         +
                         sizeof(ie_alerting_pattern_T)  +
                         sizeof(ie_network_cc_capability_T) +
                         sizeof(ie_bearer_capability_T)

                       )];
} MNCC_SETUP_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_START_DTMF_CNF                     *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                 message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T       connection_id;
   boolean               dtmf_success;
} MNCC_START_DTMF_CNF_T;

/************************************************************************
*                                                                       *
*                               MNCC_STOP_DTMF_CNF                      *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                 message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T       connection_id;
} MNCC_STOP_DTMF_CNF_T;

/************************************************************************
*                                                                       *
*                               MNCC_SYNC_IND                           *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T                 message_header;
   sys_modem_as_id_e_type  as_id;
   connection_id_T       connection_id;
   byte                  tch_rate;
   byte                  sub_channel;
   channel_mode_T        channel_mode;
   uint8                 radio_access_bearer_id;
   sync_enum_T           reason;  /* int */

   umts_codecs_list_T                   umts_codec;


  umts_wcdma_cs_voice_bearer_enum_T cs_voice_type;
} MNCC_SYNC_IND_T;



/************************************************************************
*                                                                       *
*                               MNCC_USER_USER_DATA_IND                 *
*                                                                       *
*************************************************************************
*/
typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type       as_id;
   connection_id_T    connection_id;
   byte               data[(sizeof(ie_user_user_T) +
                            sizeof(ie_more_data_T))];
} MNCC_USER_USER_DATA_IND_T;

/************************************************************************
*                                                                       *
*                               MNCC_CONGESTION_IND                     *
*                                                                       *
*************************************************************************
*/




/************************************************************************
*                                                                       *
*                               MNCC_RECALL_IND                         *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                       *
*                               MNCC_CC_EST_IND                         *
*                                                                       *
*************************************************************************
*/

/************************************************************************
*                                                                                     *
*                               MNCC_SRVCC_HANDOVER_COMPLETE_IND                *
*                                                                                     *
*************************************************************************
*/
typedef struct 
{
   IMH_T                         message_header;
   sys_modem_as_id_e_type        as_id;
   sys_radio_access_tech_e_type  active_rat;
   boolean                       first_trigger;  /* Based on the trigger MN desides to do the Transient procedures (2_send, MT call connect, Starting T303 ) or not */
} MNCC_SRVCC_HANDOVER_COMPLETE_IND_T;



/*
 * definition of data global to MN and CC
 */





#endif

