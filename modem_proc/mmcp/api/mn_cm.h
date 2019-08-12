#ifndef mn_cm_h
#define mn_cm_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_cm.h_v   1.17   10 Jul 2002 13:30:26   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mn_cm.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
22-Mar-2001  VT    Introduced MMI_CALL_CONNECTED_IND message
03-Apr-2001  VT    Removed mmi_repeat_T as it was same as 
                    mmi_repeat_indicator_T & changed all the 
                    references of the former to the latter.

5/21/01      CD    Parenthesized definition of STD_CODESTANDARD to avoid lint error

6/7/01       CD    Deleted message id MMI_DELIVER_SM_CONF, MMI_MT_SM_AVAILABLE_IND,
                   MMI_SUBMIT_SM_FROM_SIM_REQ, MMI_READ_SHORT_MESSAGE_REQ, 
                   MMI_ERASE_RAM_SM_REQ,   MMI_CONFIGURE_SM_ROUTING_REQ,

                   Changed MMI prefix in SMS message group and message id to UASMS

                   Changed message id UASMS_CHECK_FOR_SMMA_REQ to 
                   UASMS_MEMORY_AVAIL_NOTIFICATION_REQ

                   Added message_id UASMS_SM_REPORT_REQ

6/8/01       CD    Deleted all #define constants related to SM-TL
                   Deleted all typedefs related to SM-TL
                   Deleted all mmi-sms primitives 

7/12/01      CD    Removed SMS related data

                   Renamed file to mn_cm.h

7/18/01      CD    Transferred external declaration of mn_call_information from 
                   mn_cc_processing.c

7/24/01      CD    Removed SS operation codes and put them in mn_ss.h

25-Jul-2001  VT    Re-arranged the elements in the struct 
                     mmi_bearer_capability_T so that it is more readable
                   Added the following elements for CS Data in the struct,
                     mmi_bearer_capability_T:
                         compression, access_id, layer_1_id & layer_2_id

8/01/01      CD    Moved definition of MMI_IEI_T to nas_exp.h

                   Removed CM, MN common data to mn_cnm_exp.h

8/06/01      CD    Moved ss_invoke_data_T, ss_ForBS_T, registerSS_ARG_T and 
                   MN-specific ss error values to mn_ss.h

                   Deleted 
                        ie_tp_time_T
                        mmi_called_party_bcd_address_T
                        mmi_calling_party_bcd_address_T
                        ie_mmi_presScreen_ind_T
                        mmi_notify_ss_data_T
                        message_status_T
                        FREE_SPACE
                        MT_MESSAGE_READ
                        MT_MESSAGE_TO_BE_READ
                        MO_MESSAGE_SENT
                        MO_MESSAGE_TO_SEND
        
                   Removed definitions for ie_call_on_hold:  CALL_RETRIEVE and
                   CALL_ON_HOLD

                   Moved mn_call_information_T, MNCC_CALL_PROC_data_T, 
                   MNCC_MODIFY_data_T to mn_cc.h

08/14/01     AB    Renamed connection manager references to CNM from CM.

08/16/01     AB    Renamed MMI references to CM.

08/23/01     TS    Removed enums from list of inter-task commands that are sent
                    to Call Manager. These are now defined in cmll.h.
                    Moved several #defines to mn_cm_exp.h.

08/30/01     TS    Removed extra comma in enum type that caused compiler warning.

11/02/01     AB    Removed the ie_ss_password_T from cm_activate_ss_req_T &
                    cm_deactivate_ss_req_T as CM does not support this 
                    interface element.

01/08/02     AB    Added signal IE to cm_mt_call_ind_T structure.

01/25/02     CD    In cm_progress_indicator_T, replaced IEI by boolean flag
                   Updated Copyright


                   Moved cli_restriction_ind_T, InterrogateSS_Res_T and
                   cm_progress_indicator_T to mn_cm.h

02/11/02    AB     Added new CM/MN interface message type and structure
                   for SS CNAP, ECT, and CD call features.

02/18/02    AB     Added success flag to SS call independent confirmation
                   message to indicate whether parameters received
                   from the network were processed or formated correctly. 

3/06/02     AB     Updated the cm_4_x_send_T message structure used
                   CD to include connection ID.

5/06/02     AB     Reordered the cm_ss_ref element in ss_UserData_info_T to 
                   be more consitent with other CM, MN interface message 
                   structure.  

                   Added ss_operation element in cm_process_unstructured_ss_req_T
                   so that MN can also processUnstructuredSS Data Req 
                   in a REGISTER instead of Facility message.
 
05/15/02    AB     Undo the previous interface changes to 
                   cm_process_unstructured_ss_req_T.

05/24/02    AB     Reordered the REMOTE_ALERT enum to match the user_alert
                   literal that is defined in cc_iei.h.

05/30/02    AB     Added MAX_MO_CONNECTION_ID literal that defines the 
                   highest MO connection ID value. 

07/02/02    CD     Added cm_cause_of_no_CLI to cm_mt_setup_ind

07/30/02    AB     Moved the notification description, USER_RESUME, 
                   USER_SUSPEND, REMOTE_ALERT, and BEARER_CHANGED to 
                   mn_cm_exp.h, where CM/UI can reference without 
                   to pull in other MN header files.

08/08/02    AB     Added cm_signal_T, signal IEI information in
                   cm_setup_ind_T.

08/14/02    AB     In cm_4_x_send_T, corrected element name to 
                   callDeflection.

09/04/02    AB     Added cm_redirect_bcd_number & cm_redirect_subaddress 
                   element into cm_setup_ind_T to support CD notification.

10/06/02    AB     Added CM_MO_END_X_CALL_REQ to enable user to abort a 
                   specific call.
                   
01/09/03    AB     Moved the CM_RELEASE_SS_REQ into the SS call independent 
                   message id group to fix routing problem for this event.

04/15/03   sbs     Added support for Location Services related SS messages

04/18/05    HS     Added support for FEATURE_CCBS

06/23/05    HS     Added cm_release_unstructured_ss_ind_T for
                   CM_RELEASE_UNSTRUCTURED_SS_IND

07/22/05    HS     cm_recall_ind_T modified for silent redial in CCBS

09/14/06    NR     Adding changes for passing raw byte stream to CATAPPS

10/04/06    NR     Adding rab_id_T to cm_call_progress_info_ind_T structure 

12/06/06    NR     Passing byte stream for interrogate SS 
===========================================================================*/


/************************************************************************
*                                                                       *
*                       MMI-MN Message IDs                              *
*                                                                       *
*************************************************************************
*/



/* Maximum # of calls that can be transferred from IMS to CS domain via SRVCC HO*/
#define MAX_NUM_OF_SRVCC_CALLS 7

/* These Constant values are used between CM and NAS in cm_asid_2_vsid_mapping_ind_T API 
   CM populates these values in asid_to_vsid_map array for each SUB based the SUB capability
*/
#define MN_CM_MULTIMODE_SUB_VS_ID        281022464
#define MN_CM_NON_MULTIMODE_SUB_VS_ID    282857472 

enum 
{
   /* CM -> MN MESSAGES */

   /* cm mo cc req messages: first nibble must = 0 */ 

   CM_MO_CC_REQ                    = 0x00,    /* message group */
   CM_MO_NORMAL_CALL_REQ,
   CM_MO_EMERGENCY_CALL_REQ,
   CM_START_DTMF_REQ,
   CM_STOP_DTMF_REQ,
   CM_0_SEND_REQ,
   CM_1_SEND_REQ,
   CM_1_X_SEND_REQ,
   CM_2_SEND_REQ,
   CM_2_X_SEND_REQ,
   CM_3_SEND_REQ,
   CM_MO_END_CALL_REQ,
   CM_MO_USER_DATA_REQ,
   CM_MO_MODIFY_REQ,
   CM_ABORT_CC_REQ,
   CM_4_SEND_REQ,   /* ECT */
   CM_4_X_SEND_REQ, /* CD */
   CM_5_SEND_REQ, /* CCBS */
   CM_MO_END_X_CALL_REQ,
   CM_ASID_2_VSID_MAPPING_IND,
   CM_TRANSFER_SRVCC_CALL_CONTEXT_RES, /* CM --> NAS */   
 
   /* Supplementary Service MO REQ messages: first nibble must = 1 */

   CM_MO_SS_REQ                    = 0x20,    /* message group */
   CM_ACTIVATE_SS_REQ,
   CM_DEACTIVATE_SS_REQ,
   CM_ERASE_SS_REQ,
   CM_INTERROGATE_SS_REQ,
   CM_REGISTER_PASSWORD_REQ,
   CM_REGISTER_SS_REQ,
   CM_PROCESS_UNSTRUCTURED_SS_REQ,
   CM_ABORT_SS_REQ,
   CM_PROCESS_UNSTRUCTURED_SS_DATA,
   CM_RELEASE_SS_REQ,

   CM_LCS_MOLR_REQ,


   CM_ERASE_CC_ENTRY_REQ,


   /* CM Short Message Service MO REQ messages: first nibble must = 2 */
   /* UASMS REQ message group = 0x30 (see mn_uasms.h) */

   /*  cm mt cc res messages: first nibble must = 4 */

   CM_MT_CC_RES                    = 0x40,    /* message group */
   CM_MT_CALL_RES,
   CM_MT_SETUP_RES,
   CM_MT_MODIFY_RES, 

   CM_RECALL_RSP,


   /* cm SS mt RES messages: first nibble must = 5  */

   CM_MT_SS_RES                    = 0x50,    /* message group */
   CM_GET_PASSWORD_RES,
   CM_UNSTRUCTURED_SS_NOTIFY_RES,
   CM_UNSTRUCTURED_SS_RES,
   CM_MT_CALL_SS_RES,

   CM_LCS_LOCATION_NOTIFICATION_RES,
   CM_LCS_MOLR_COMPLETED,


   /* cm SMS mt RES messages: first nibble must = 6 */
   /* UASMS RES message group = 0x60 (see mn_uasms.h) */


   /* MN -> CM MESSAGES */

   /* cm cc mo cnf messages: first nibble must = 8 */

   CM_MO_CC_CONF                   = 0x80,    /* message group */
   CM_5_SEND_RESP
};


#define MO                             0x00
#define MT                             0x08
#define UNKNOWN_CONNECTION_ID          0x07  /* UNKNOWN mobile originated connection ID */
#define USER_USER_DATA_Q_ID            0x07  /* QID used to store user data buffers  */
#define MAX_MO_CONNECTION_ID           0x06

/****************************************************************
 *                                                              *
 *               cm mo normal call req                          *
 *                                                              *
 ****************************************************************
 */


typedef struct 
{
   IMH_T                          message_header;
   connection_id_T                connection_id;

   sys_modem_as_id_e_type          as_id;

   cm_repeat_indicator_T          bc_repeat_indicator;
   cm_bearer_capability_T         bearer_capability_1;
   cm_bearer_capability_T         bearer_capability_2;
   cm_repeat_indicator_T          llc_repeat_indicator;
   cm_low_layer_compatibility_T   low_layer_compatibility_1;
   cm_low_layer_compatibility_T   low_layer_compatibility_2;
   cm_repeat_indicator_T          hlc_repeat_indicator;
   cm_high_layer_compatibility_T  high_layer_compatibility_1;
   cm_high_layer_compatibility_T  high_layer_compatibility_2;
   cm_called_party_bcd_no_T       cm_called_party_bcd_number;
   cm_called_party_subaddress_T   cm_called_party_subaddress;
   cm_calling_party_subaddress_T  cm_calling_party_subaddress; 
   cm_forward_cug_info_T          forward_cug_info;
   byte                           cm_clir;

   cm_stream_identifier_T         stream_id;
   cm_supported_codecs_list_T     codecs_list;


} cm_mo_normal_call_req_T;

/****************************************************************
*                                                               *
*                cm mo emergency call req                       *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id ; /* = UNKNOWN_CONECTION_ID */

   sys_modem_as_id_e_type          as_id;

   cm_bearer_capability_T          bearer_capability;

 
   cm_stream_identifier_T          stream_id;
   cm_supported_codecs_list_T      codecs_list;


   cm_emergency_category_T         service_category;   

} cm_mo_emergency_call_req_T;


/****************************************************************
*                                                               *
*                cm mo end call req                             *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;

   sys_modem_as_id_e_type          as_id;

} cm_mo_end_call_req_T;


/****************************************************************
*                                                               *
*                cm start dtmf req                              *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   byte                            dtmf_value;

   sys_modem_as_id_e_type          as_id;

} cm_start_dtmf_req_T;


/****************************************************************
*                                                               *
*                cm stop dtmf req                               *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;

   sys_modem_as_id_e_type          as_id;

} cm_stop_dtmf_req_T;


/****************************************************************
*                                                               *
*  cm n send req, used to build MPTY (multiparty)  and          *
*     explicit call transfer (ECT).                             *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;

   sys_modem_as_id_e_type          as_id;

} cm_n_send_req_T;


/****************************************************************
*                                                               *
*                cm n x send req                                *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   byte                            cause_value;

   sys_modem_as_id_e_type          as_id;

} cm_n_x_send_req_T;


/****************************************************************
*                                                               *
*         cm 4 x, directory number, send req, call deflection   *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   cm_called_party_bcd_no_T        deflectedToNumber;
   cm_called_party_subaddress_T    deflectedToSubAddress;

   sys_modem_as_id_e_type          as_id;

} cm_4_x_send_req_T;


/****************************************************************
*                                                               *
* cm 5 send req, Activates completion of call to busy subscriber*
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;

   sys_modem_as_id_e_type          as_id;

} cm_5_send_req_T;


/****************************************************************
*                                                               *
*                cm abort cc req                                *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cause;

   sys_modem_as_id_e_type          as_id;

} cm_abort_cc_req_T;

/****************************************************************
*                                                               *
*                cm mo user_data req                           *
*                                                               *
*****************************************************************
*/


typedef struct 
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   uus_type_T                      type;
   byte                            data_length; /* data length */
   byte                            protocol_discriminator;
   byte                            data[MAX_USER_USER_DATA_LENGTH];
   boolean                         more_data_indicator;

   sys_modem_as_id_e_type          as_id;
      
} cm_mo_user_data_req_T;


/****************************************************************
*                                                               *
*                cm register ss req                            *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   byte                            ss_code;
   basic_service_T                 basic_service;
   cm_called_party_bcd_no_T        forwarded_to_number;
   cm_called_party_subaddress_T    forwarded_to_subaddress;
   ie_no_reply_condition_timer_T   cm_no_reply_condition_timer;   

   sys_modem_as_id_e_type          as_id;

} cm_register_ss_req_T;

/****************************************************************
*                                                               *
*                cm activate ss req                            *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   byte                            ss_code;
   basic_service_T                 basic_service; 

   sys_modem_as_id_e_type          as_id;

} cm_activate_ss_req_T;

/****************************************************************
*                                                               *
*                cm deactivate ss req                          *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   byte                            ss_code;
   basic_service_T                 basic_service;

   sys_modem_as_id_e_type          as_id;

} cm_deactivate_ss_req_T;

/****************************************************************
*                                                               *
*                cm erase ss req                               *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   byte                            ss_code;
   basic_service_T                 basic_service;

   sys_modem_as_id_e_type          as_id;

} cm_erase_ss_req_T;

/****************************************************************
*                                                               *
*                cm interrogate ss req                         *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   byte                            ss_code;
   basic_service_T                 basic_service;

   sys_modem_as_id_e_type          as_id;

} cm_interrogate_ss_req_T;

/****************************************************************
*                                                               *
*                cm register password req                      *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   ie_ss_code_T                    ss_code;
   ie_ss_password_T                ss_password;
   ie_ss_password_T                ss_new_password;
   ie_ss_password_T                ss_new_password_again;

   sys_modem_as_id_e_type          as_id;

} cm_register_password_req_T;


/****************************************************************
*                                                               *
*                cm process unstructured ss req (phase 2)      *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   byte                            ss_operation;
   uss_data_T                      uss_data;

   sys_modem_as_id_e_type          as_id;

} cm_process_unstructured_ss_req_T;

/****************************************************************
*                                                               *
*                cm process unstructured ss data (phase 1)     *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   ss_User_Data_T                  ss_User_Data;

   sys_modem_as_id_e_type          as_id;

} cm_process_unstructured_ss_data_T;

/****************************************************************
*                                                               *
*                cm abort ss req                               *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cause;

   sys_modem_as_id_e_type          as_id;

} cm_abort_ss_req_T;

/****************************************************************
*                                                               *
*                cm release ss req                             *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   byte                            ss_operation;
   ie_cm_ss_error_T                cm_ss_error;
   sys_modem_as_id_e_type          as_id;

} cm_release_ss_req_T;

/****************************************************************
*                                                               *
*                cm mt call res                                 *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   boolean                        cm_accept;
   ie_cm_cc_cause_T               cm_cc_cause;
   cm_connected_subaddress_T      cm_connected_subaddress;

   sys_modem_as_id_e_type          as_id;

} cm_mt_call_res_T;

/****************************************************************
*                                                               *
*                cm mt setup res                                *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   boolean                        cm_accept;
   ie_cm_cc_cause_T               cm_cc_cause;
   cm_repeat_indicator_T          bc_repeat_indicator;
   cm_bearer_capability_T         bearer_capability_1;
   cm_bearer_capability_T         bearer_capability_2;

   cm_stream_identifier_T         stream_id;
   cm_supported_codecs_list_T     codecs_list;


   sys_modem_as_id_e_type          as_id;

} cm_mt_setup_res_T;

/****************************************************************
*                                                               *

*                cm unstructured ss notify res                 *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                          message_header;
   ie_cm_ss_error_T               cm_ss_error;
   byte                           invoke_id;

   sys_modem_as_id_e_type          as_id;

} cm_unstructured_ss_notify_res_T;

/****************************************************************
*                                                               *
*                cm unstructured ss res                        *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            invoke_id;
   uss_res_data_T                  uss_res_data;

   sys_modem_as_id_e_type          as_id;

} cm_unstructured_ss_res_T;

/****************************************************************
*                                                               *
*                cm get_password res                           *
*                                                               *
*****************************************************************
*/

typedef struct

{
   IMH_T                           message_header;
   byte                            invoke_id;
   ie_ss_password_T                ss_password;

   sys_modem_as_id_e_type          as_id;

} cm_get_password_res_T;


/****************************************************************
*                                                               *
*                cm recall rsp                                  *
*                                                               *
*****************************************************************
*/

typedef struct {
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   cm_recall_rsp_e_type            response;

   sys_modem_as_id_e_type          as_id;

} cm_recall_rsp_T;


/****************************************************************
*                                                               *
*                cm_asid_2_vsid_mapping_ind_T                                  *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   uint32                          asid_to_vsid_map[MAX_AS_IDS];
} cm_asid_2_vsid_mapping_ind_T;


/****************************************************************
*                                                               *
*                cm lcs location notification res               *
*                                                               *
*****************************************************************
*/

typedef struct

{
   IMH_T                               message_header;
   byte                                invoke_id;
   ie_ss_location_notification_res_T   location_notification_res;

   sys_modem_as_id_e_type          as_id;

} cm_lcs_location_notification_res_T;

/****************************************************************
*                                                               *
*                cm lcs molr_req                                *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;
   molr_type_e_type                molr_type;
   location_method_T               location_method;
   lcs_qos_T                       lcs_qos;
   lcs_client_external_id_T        external_id;
   mlc_number_T                    mlc_number;
   gps_assistance_data_T           gps_assistance_data;
   supported_gad_shapes_T          supported_gad_shapes;

   sys_modem_as_id_e_type          as_id;

} cm_lcs_molr_req_T;


/****************************************************************
*                                                               *
*                cm lcs molr_compl                              *
*                                                               *
*****************************************************************
*/
typedef struct 
{
   IMH_T                           message_header;
   byte                            cm_ss_ref;

   sys_modem_as_id_e_type          as_id;

} cm_lcs_molr_compl_T;




/****************************************************************
*                                                               *
*                cm erase cc entry req                          *
*                                                               *
*****************************************************************
*/
typedef struct
{
  IMH_T              message_header;
  byte               cm_ss_ref;
  byte               ss_code;
  ie_ccbs_index_T    ccbs_index;

   sys_modem_as_id_e_type          as_id;

} cm_erase_cc_entry_req_T;


/****************************************************************
*                                                               *
*                cm mo call conf                                *
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   boolean                        cnm_service_success;
   ie_cm_cc_cause_T               cm_cc_cause;
   ie_cm_mo_call_details_T        cm_mo_call_details;
   ie_cm_cc_reject_T              cm_cc_reject;        

   cm_network_cc_capability_T     network_cc_capability;


   sys_modem_as_id_e_type          as_id;

} cm_mo_call_conf_T;


/****************************************************************
*                                                               *
*                       cm mo modify req                        *
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   cm_bearer_capability_T         bearer_capability;
   cm_low_layer_compatibility_T   low_layer_compatibility;
   cm_high_layer_compatibility_T  high_layer_compatibility;
   boolean                        reverse_call_setup_direction;

   sys_modem_as_id_e_type          as_id;

} cm_mo_modify_req_T;
        

/****************************************************************
*                                                               *
*                cm mo modify complete cnf                      *
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   boolean                        complete;
   ie_cm_cc_cause_T               cm_cc_cause;
   cm_bearer_capability_T         cm_bearer_capability;
   cm_low_layer_compatibility_T   cm_low_layer_compatibility;
   cm_high_layer_compatibility_T  cm_high_layer_compatibility;
   boolean                        reverse_call_setup_direction;

   sys_modem_as_id_e_type          as_id;

} cm_modify_complete_conf_T;


/****************************************************************
*                                                               *
*                cm mt modify ind                               *
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   cm_bearer_capability_T         bearer_capability;
   cm_low_layer_compatibility_T   low_layer_compatibility;
   cm_high_layer_compatibility_T  high_layer_compatibility;
   boolean                        reverse_call_setup_direction;

   boolean                        immediate_modification;
   boolean                        nw_initiated_ind; 


   sys_modem_as_id_e_type          as_id;

} cm_mt_modify_ind_T;


/****************************************************************
*                                                               *
*                cm mt modify res                               *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   boolean                         cm_accept;
   ie_cm_cc_cause_T                cm_cc_cause;   /* incase of UE reject */

   sys_modem_as_id_e_type          as_id;

} cm_mt_modify_res_T;


 /****************************************************************
*                                                               *
*                cm start dtmf conf                             *
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   boolean                         dtmf_success;
   ie_cm_cc_reject_T               cm_cc_reject;

   sys_modem_as_id_e_type          as_id;
       
} cm_start_dtmf_conf_T;


/****************************************************************
*                                                               *
*                cm stop dtmf conf                              *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;          
   ie_cm_cc_reject_T               cm_cc_reject;

   sys_modem_as_id_e_type          as_id;

} cm_stop_dtmf_conf_T;


/****************************************************************
*                                                               *
*                cm abort cc conf                               *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;

   sys_modem_as_id_e_type          as_id;

} cm_abort_cc_conf_T;


/****************************************************************
*                                                               *
*                cm manage calls conf                           *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    active_calls_list_T             active_calls_list;
    boolean                         call_ss_success;
    ie_cm_ss_error_T                cm_ss_error;

   sys_modem_as_id_e_type          as_id;

} cm_manage_calls_conf_T;


/****************************************************************
*                                                               *
*                cm activate ss conf                            *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    boolean                         success;
    byte                            cm_ss_ref;
    ss_info_T                       ss_info;

    unparsed_suppsvc_l3_data_T      suppsvc_result;


   sys_modem_as_id_e_type          as_id;
  
} cm_activate_ss_conf_T;


/****************************************************************
*                                                               *
*                cm deactivate ss conf                          *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    boolean                         success;
    byte                            cm_ss_ref;
    ss_info_T                       ss_info;

    unparsed_suppsvc_l3_data_T      suppsvc_result;


   sys_modem_as_id_e_type          as_id;

} cm_deactivate_ss_conf_T;


/****************************************************************
*                                                               *
*                cm erase ss conf                               *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    boolean                         success;
    byte                            cm_ss_ref;
    ss_info_T                       ss_info;

    unparsed_suppsvc_l3_data_T      suppsvc_result;


   sys_modem_as_id_e_type          as_id;

} cm_erase_ss_conf_T;


/****************************************************************
*                                                               *
*                cm register ss conf                            *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    boolean                         success;
    byte                            cm_ss_ref;
    ss_info_T                       ss_info;

    unparsed_suppsvc_l3_data_T      suppsvc_result;


   sys_modem_as_id_e_type          as_id;

} cm_register_ss_conf_T;


/****************************************************************
*                                                               *
*                cm erase cc entry conf                         *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    boolean                         success;
    byte                            cm_ss_ref;
    ss_info_T                       ss_info;
   sys_modem_as_id_e_type          as_id;
} cm_erase_cc_entry_conf_T;

/****************************************************************
*                                                               *
*                cm 5 send conf                                 *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                   message_header;
   connection_id_T         connection_id;
   cm_5_send_conf_e_type   conf_type;
   union {
      ie_ccbs_indicator_T  ret_res;      /* for CCBS_RET_RES */
      ie_cm_ss_error_T     cm_ss_error;  /* for CCBS_RET_ERR or CCBS_REJ */
   } rsp;
   sys_modem_as_id_e_type          as_id;
} cm_5_send_conf_T;


/****************************************************************
*                                                               *
*                cm interrogate ss conf                         *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    boolean                         success;
    byte                            cm_ss_ref;
    cm_ss_conf_e_type               res_type;
    InterrogateSS_Res_T             InterrogateSS_Res;

    unparsed_suppsvc_l3_data_T      suppsvc_result;


   sys_modem_as_id_e_type          as_id;

} cm_interrogate_ss_conf_T;


/****************************************************************
*                                                               *
*                cm lcs molr conf                               *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    boolean                         success;
    byte                            cm_ss_ref;
    lcs_molr_res_T                  lcs_molr_res;
    unparsed_suppsvc_l3_data_T      suppsvc_result;
   sys_modem_as_id_e_type          as_id;
} cm_lcs_molr_conf_T;


/****************************************************************
*                                                               *
*                cm register password conf                      *
*                                                               *
*****************************************************************
*/

typedef struct
{
    IMH_T                           message_header;
    boolean                         success;
    byte                            cm_ss_ref;
    ss_password_info_T              info;

    unparsed_suppsvc_l3_data_T      suppsvc_result;


   sys_modem_as_id_e_type          as_id;

} cm_register_password_conf_T;


/****************************************************************
*                                                               *
*                cm process unstructured ss conf (phase 2)      *
*                                                               *
*****************************************************************
*/

typedef struct 
{
   IMH_T                           message_header;
   boolean                         success;
   byte                            cm_ss_ref;
   ss_data_info_T                  ss_data_info;

    unparsed_suppsvc_l3_data_T      suppsvc_result;

   sys_modem_as_id_e_type          as_id;

} cm_process_unstructured_ss_conf_T;



/****************************************************************
*                                                               *
*                cm process unstructured ss data (phase 1)      *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   boolean                         success;
   byte                            cm_ss_ref;
   ss_UserData_info_T              ss_UserData_info;

    unparsed_suppsvc_l3_data_T      suppsvc_result;


   sys_modem_as_id_e_type          as_id;

} cm_process_unstructured_ss_data_conf_T;


/****************************************************************
*                                                               *
*                cm mt call ind                                 *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   cm_signal_T                     signal;

   sys_modem_as_id_e_type          as_id;

} cm_mt_call_ind_T;

/****************************************************************
*                                                               *
*                cm mt setup ind                                *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   cm_repeat_indicator_T          bc_repeat_indicator;
   cm_bearer_capability_T         bearer_capability_1;
   cm_bearer_capability_T         bearer_capability_2;
   cm_repeat_indicator_T          llc_repeat_indicator;
   cm_low_layer_compatibility_T   low_layer_compatibility_1;
   cm_low_layer_compatibility_T   low_layer_compatibility_2;
   cm_repeat_indicator_T          hlc_repeat_indicator;
   cm_high_layer_compatibility_T  high_layer_compatibility_1;
   cm_high_layer_compatibility_T  high_layer_compatibility_2;
   cm_called_party_bcd_no_T       cm_called_party_bcd_number;
   cm_called_party_subaddress_T   cm_called_party_subaddress;
   cm_calling_party_bcd_no_T      cm_calling_party_bcd_number;
   cm_calling_party_subaddress_T  cm_calling_party_subaddress;
   cm_calling_party_bcd_no_T      cm_redirect_party_bcd_number;
   cm_calling_party_subaddress_T  cm_redirect_party_subaddress;
   cm_cause_of_no_CLI_T           cm_cause_of_no_CLI;
   cm_alerting_pattern_T          cm_alerting_pattern;
   cm_signal_T                    cm_signal;

   cm_network_cc_capability_T     cm_network_capability;
   cm_bearer_capability_T         backup_bearer_capability;

   sys_modem_as_id_e_type          as_id;

} cm_mt_setup_ind_T;

/****************************************************************
*                                                               *
*                cm srvcc handover complete ind                                *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                          message_header;
   boolean                        success; /* This flag tells whether NAS found data given by CM correct or not */
   sys_modem_as_id_e_type         as_id;

} cm_srvcc_handover_complete_ind_T;


typedef struct
{
   IMH_T                          message_header;
   sys_radio_access_tech_e_type   active_rat;   
   sys_modem_as_id_e_type          as_id;

} cm_get_srvcc_context_req_T;



/****************************************************************
*                                                               *
*                cm srvcc individual call context                                *
*                                                               *
*****************************************************************
*/


typedef enum
{	 
  CM_NAS_SRVCC_ALERT_MEDIA_NONE = -1,    /* FOR INTERNAL CM USE ONLY! */  
  CM_NAS_SRVCC_ALERT_MEDIA_LOCAL,        /** IMS indicated that Ring back tone played locally*/
  CM_NAS_SRVCC_ALERT_MEDIA_NW,           /** IMS indicated NW early media is played*/
  CM_NAS_SRVCC_ALERT_MEDIA_MAX           /* FOR INTERNAL CM USE ONLY! */  

}cm_nas_srvcc_alert_media_type;


typedef struct 
{

   connection_id_T                   srvcc_call_connection_id;
   cc_state_T                        srvcc_call_cc_state;
   hold_auxiliary_state_T            srvcc_call_hold_aux_state;
   mpty_auxiliary_state_T            srvcc_call_mpty_aux_state;
   cm_nas_srvcc_alert_media_type     srvcc_ps_early_media_type;
} 
cm_srvcc_individual_call_context_T;


/****************************************************************
*                                                               *
*                cm srvcc call context                                *
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                               message_header;
   /* Put 0 means app failed to tfer the call context from IMS to CS domain*/
   byte                                num_calls;
   /* Array with size of Maximum number of calls that can be transferred*/
   cm_srvcc_individual_call_context_T  individual_call_context[MAX_NUM_OF_SRVCC_CALLS];
   sys_modem_as_id_e_type              as_id;
}
cm_srvcc_call_context_T;


/****************************************************************
*                                                               *
*                cm mt call ss ind                              *
*                                                               *
*****************************************************************
*/

typedef enum
{
    CM_NOTIFY_SS_IND,
    CM_FWD_AOC_IND,
    CM_UNKNOWN_SS_IND

} cm_mt_call_ss_ind_e_type;

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   cm_mt_call_ss_ind_e_type        ind_type;
   union
   {
      ie_cm_notify_ss_T                  cm_notify_ss;
      ie_cm_forward_charge_advice_req_T  cm_forward_charge_advice;

   } data;

   sys_modem_as_id_e_type          as_id;

} cm_mt_call_ss_ind_T;

/****************************************************************
*                                                               *
*                cm mt call ss res                              *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                           message_header;
   boolean                         aoc_supported;

   sys_modem_as_id_e_type          as_id;

} cm_mt_call_ss_res_T;


/*****************************************************************
*
*     adding rab_id_T structure to call progress info ind 
*
******************************************************************
*/
typedef struct
{
   boolean                         present;
   uint8                           radio_access_bearer_id;

   sys_modem_as_id_e_type          as_id;

} rab_id_T;

/****************************************************************
*                                                               *
*                cm call progress info ind                      *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   byte                            call_progress;

   rab_id_T                           rab_id;

   sys_modem_as_id_e_type          as_id;

} cm_call_progress_info_ind_T;



/****************************************************************
*                                                               *
*                cm_mo_call_alert_ind_T                      *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;


   sys_modem_as_id_e_type          as_id;

} cm_mo_call_alert_ind_T;



/****************************************************************
*                                                               *
*                cm connected number ind                        *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   cm_connected_no_T              cm_connected_number;
   cm_connected_subaddress_T      cm_connected_subaddress;

   sys_modem_as_id_e_type          as_id;

} cm_connected_number_ind_T;

/****************************************************************
*                                                               *
*                cm mt end call ind                             *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   ie_cm_cc_cause_T               cm_cc_cause;   /* normal call clearing */
   ie_cm_cc_cause_T               mo_disconnect_cause; /* cause used in MO disconnect by NAS */
   ie_cm_cc_reject_T              cm_cc_reject;   /* normal call clearing */

   sys_modem_as_id_e_type          as_id;

} cm_mt_end_call_ind_T;

/****************************************************************
*                                                               *
*                cm mt disc ind                                 *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                          message_header;
   connection_id_T                connection_id;
   ie_cm_cc_cause_T               cm_cc_cause;

   sys_modem_as_id_e_type          as_id;

   boolean                        inband_tone_present;/*it indicates if inband is present or not */

} cm_mt_disc_ind_T;


/****************************************************************
*                                                               *
                                                               *
*                cm mt user data ind                            *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                   message_header;
   connection_id_T         connection_id;
   byte                    user_user_data_length; /* of user_user_data */
   byte                    user_user_protocol_discriminator;
   byte                    user_user_data[MAX_USER_USER_DATA_LENGTH];
   boolean                 more_data_indicator;

   sys_modem_as_id_e_type          as_id;
       
} cm_mt_user_data_ind_T;


typedef struct 
{
   IMH_T                   message_header;
   connection_id_T         connection_id;
   boolean                 receiver_busy;  /* 1 = not ready, suspend user_user data request */ 
   ie_cm_cc_cause_T        cm_cc_cause;

   sys_modem_as_id_e_type          as_id;

} cm_user_congestion_ind_T;



/****************************************************************
*                                                               *
*                cm forward check ss ind                        *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                           message_header;

   sys_modem_as_id_e_type          as_id;

} cm_forward_check_ss_ind_T;


/****************************************************************
*                                                               *
*                cm unstructured ss notify ind                  *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                           message_header;
   byte                            invoke_id;
   uss_data_T                      uss_data;

   sys_modem_as_id_e_type          as_id;

} cm_unstructured_ss_notify_ind_T;

/****************************************************************
*                                                               *
*                cm unstructured ss ind                         *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                           message_header;
   byte                            invoke_id;
   uss_data_T                      uss_data;

   sys_modem_as_id_e_type          as_id;

} cm_unstructured_ss_ind_T;

/****************************************************************
*                                                               *
*                cm release unstructured ss ind                 *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T        message_header;

   sys_modem_as_id_e_type          as_id;

} cm_release_unstructured_ss_ind_T;

/****************************************************************
*                                                               *
*                cm get password ind                            *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   byte                            invoke_id;
   byte                            guidance_info;

   sys_modem_as_id_e_type          as_id;

} cm_get_password_ind_T;


/****************************************************************
*                                                               *
*                cm lcs_location_notification ind               *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   byte                            invoke_id;
   location_notification_T         location_notification;
   sys_modem_as_id_e_type          as_id;
} cm_lcs_location_notification_ind_T;

/****************************************************************
*                                                               *
*                cm connection state ind                        *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   boolean                         channel_connected;

   sys_modem_as_id_e_type          as_id;

} cm_connection_state_ind_T;



/****************************************************************
*                                                               *
*                cm_mt_data_connect_ind                         *
*                                                               *
* ***************************************************************
*/

typedef struct
{
   IMH_T                           message_header;

   sys_modem_as_id_e_type          as_id;

} cm_mt_data_connect_ind_T;


/****************************************************************
*                                                               *
*                cm_call_connected_ind                          *
*                                                               *
* ***************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   uint8                           radio_access_bearer_id;

   sys_modem_as_id_e_type          as_id;

} cm_call_connected_ind_T;


/****************************************************************
*                                                               *
*                cm_cs_sig_rel_ind                              *
*                                                               *
* ***************************************************************
*/
/*  Structure added 09/16/03  */
typedef struct
{
   IMH_T                           message_header;

   sys_modem_as_id_e_type          as_id;

} cm_cs_sig_rel_ind_T;

/****************************************************************
*                                                               *
*                cm_modify_complete_data                        *
*                                                               *
* ***************************************************************
*/
typedef struct
{
    connection_id_T                 connection_id;
    boolean                         complete;
    ie_cm_cc_cause_T                cm_cc_cause;
    cm_bearer_capability_T          cm_bearer_capability;
    boolean                         low_layer_compatibility_present;
    cm_low_layer_compatibility_T    cm_low_layer_compatibility;
    boolean                         high_layer_compatibility_present;
    cm_high_layer_compatibility_T   cm_high_layer_compatibility;
    boolean                         reverse_call_direction;

   sys_modem_as_id_e_type          as_id;

} cm_modify_complete_data_T;


/****************************************************************
*                                                               *
*                cm_ccbs_info_ind                               *
*                                                               *
* ***************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   boolean                         ccbs_allowed;
   sys_modem_as_id_e_type          as_id;
} cm_ccbs_info_ind_T;

/****************************************************************
*                                                               *
*                cm_recall_ind                                  *
*                                                               *
* ***************************************************************
*/

typedef struct {
   IMH_T                          message_header;
   connection_id_T                connection_id;
   byte                           recall_type;
   cm_mo_normal_call_req_T        call_info;
   sys_modem_as_id_e_type          as_id;
} cm_recall_ind_T;


typedef struct
{
   IMH_T                           message_header;
   connection_id_T                 connection_id;
   sync_enum_T                     handover_cause; /* Can be either WCDMA_GSM_HANDOVER or GSM_WCDMA_HANDOVER*/
   sys_modem_as_id_e_type          as_id;
} cm_handover_complete_ind_T; 

#endif

