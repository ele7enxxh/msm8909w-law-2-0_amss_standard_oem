#ifndef mn_cm_exp_h
#define mn_cm_exp_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_cm_exp_h_v   1.17   10 Jul 2002 13:31:12   cdealy  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mn_cm_exp.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/08   fj      Move some macros to cm_gw.h for WM6 build
07/30/08   rm      CMI defeaturisation
08/01/01   CD      Initial version

08/06/01   CD      Removed TP_BCD_NO_LENGTH

08/14/01   AB      Renamed connection manager references to CNM from CM.

08/16/01   AB      Renamed MMI references to CM.

01/25/02   CD      Modified structures per new CM-to-MN interface. Replaced IEI
                   fields by boolean flag the indicates if the IE is present or not

                   Updated Copyright

                   Moved cli_restriction_indo_T, InterrogateSS_Res_T and
                   cm_progress_indicator_T from mn_cm.h

                   Moved contents of mn_exp.h here

02/11/02   AB      Added CM_CLIR_DEFAULT and CM_CAUSE_DEFAULT default value so 
                   that CM can use to set the appropriate default value for 
                   the related information element (ie), that are send to the
                   network.

02/26/02   AB      Added MN_MAXNO_CALLS_IN_MPTY, max no of remote parties 
                     allowed in MPTY.

03/02/02   AB      Increased number of ForwardToNumber to array of  
                     MAXNO_FORWARD_TO_NUMBER, 0x9, in forwardToFeatureList 
                     structure. 

03/12/02   AB      Added cm_cc_invoke_T to contains the directory number 
                     to support CD.

                   Added ie_name_indicator_T, ie_ect_indicator_T, ie_ccbs_feature_T
                     and the ie_multicall_indicator_T to completed the 
                     cm_notifySS_T message structure.

01-Apr-2002  VT    GSM EFR: Added octets 3a, 3b, and 3c of type,
                     gsm_speech_ver_e_type, in cm_bearer_capability_T, 
                     inside FEATURE_GSM_EFR.
                   GSM EFR: In the definition of cm_bearer_capability_T,
                     added comment for octets 3a, 3b, and 3c:  We'll 
                   GSM EFR: Introduced the definition of gsm_speech_ver_e_type
                     inside #ifdef FEATURE_GSM_EFR.
                   GSM EFR: Introduced the definition of 
                     gsm_coding_bearer_cap_octet3x_e_type inside 
                     #ifdef FEATURE_GSM_EFR.
                   Actually, merged all the above changes from rev 1.11.1.1
                     (a branch) of this file.

08-Apr-2002  AB    Delete the NEW_CONNECTION_ID liternal as no longer needed
                     for the deferred notification SS implementation.

06-May-2002  AB    Added SS_VERSION literals.  Featurize the notification
                      interface message for ECT, CD and CNAP SS feature.

07/02/02     CD    Added cause of no CLI structure

07/29/02     AB    Moved the notification description here from 
                   mn_cm.h, so that CM/UI can reference it without
                   having to includes other MN header files.

                   Added CNAP SS code, extracted from UMTS TS29.02 spec.

08/08/02    AB     Set the MAXNO_FORWARD_TO_NUMBER to 9 as specified
                   in UMTS TS24.080 spec.- now that CM/UI can handle  
                   larger command buffer size. 

08/14/02    AB     Reset the MAXNO_FOwARD_TO_NUMBER to 2 because
                   of stability issue with larger command buffer
                   size at the CM/UI.

09/16/02    AB     Removed FEATURE_GSM_EFR, code exclusion by 
                   definition.

11/26/02    AB     Added the multimedia other rate adaption.
 
04/09/03    AB     Added ie_cm_cc_rej_T that use supports silent
                   redial feature.

05/12/03    AB     Added BC speech version pref 4 and 5 to supports
                      GSM AMR FR & HR.

06/02/03    AB     Reduce the MAXNO_FORWARD_TO_NUMBER further to 1 from 2 to 
                      save data memory as UI/CM only use 1 entry.

07/02/02    AB     Added call back servcies of CS handover notification 
                      and CM/UI AOC response.

07/25/03    AB     Updated the InterrogateSS_Res_T fix overlap of ss_status
                      and BSG data.  CR#33317.

02/24/05    AB     Added ATCOP client for HO notification.

03/07/05    HS     Added supports for FEATURE_TTY (CTM bit).

04/19/05    HS     Added support for FEATURE_CCBS

06/12/05    AB     Increase the added location estimate to 91 from 90 per
                       TS23.032, encoding of the polygon shape. 

11/09/05    AB     Added supports for FEATURE_EMERGENCY_CATEGORY

04/10/06    NR     Adding HTORPC metacomments to the remote CM APIs

05/02/06    NR     Correcting numberOfPW_AttemptsViolation SS error code

07/11/06    NR     Adding FEATURE_HTORPC_METACOMMENTS for remote_api working.

07/27/06    NR     Modifying the remote_api comments for removing error fatal

09/14/06    NR     Adding changes for passing raw byte stream to CATAPPS

12/22/06    RD     Defined enums to help removing lint errors
===========================================================================*/
#include "sys_cnst.h"
#include "sys_type.h"
#include "cm_gw.h"         /* Defintions from NAS required by clients */


/************************************************************************
*                                                                       *
*               Basic Service Codes                                     *
*                                                                       *
*************************************************************************
*/




/* Type of [phone] number */

#define INTERNATIONAL_TON_ISDN_NPI          0x91
#define UNKNOWN_TON_ISDN_NPI                0x81

/*subaddress params*/
#define SUBADDRESS_NSAP                     0x80
#define SUBADDRESS_AFI                      0x50



/* defs for ie_call_on_hold */

#define   CALL_RETRIEVED   0
#define   CALL_ON_HOLD     1



/* Problem codes for ss_problem_T */
/* General problem codes */

#define   UNRECOGNISED_COMPONENT       0x00
#define   MISTYPED_COMPONENT           0x01
#define   BADLY_STRUCTURED_COMPONENT   0x02

/* Definitions of component type tags */

#define INVOKE                         0xa1
#define RETURN_RESULT                  0xa2
#define RETURN_ERROR                   0xa3
#define REJECT                         0xa4
#define UNKNOWN                        0xff

#define WAIUR_NOT_APPLICABLE                  0
#define OMT_NONE                              0
#define FNUR_NOT_APPLICABLE                   0
#define UIMI_NOT_ALLOWED                      0

/* SS List and array size values */

#define MN_MAXNO_CALLS_IN_MPTY            5
#define MAX_CNAP_LENGTH                  160 /* 80 character x 16 bits ascii */  


/* SysID */
#define SYSID_GSM                                   (0x00)  /* TS 26.106 */
#define SYSID_UMTS                                  (0x04)

/* Codec BitMap bit 8 ... 1 */
#define CODEC_BITMAP_GSM_FULL_RATE                      (0x01)
#define CODEC_BITMAP_GSM_HALF_RATE                      (0x02)
#define CODEC_BITMAP_GSM_ENHANCED_FULL_RATE             (0x04)
#define CODEC_BITMAP_GSM_FULL_RATE_AMR                  (0x08)
#define CODEC_BITMAP_GSM_HALF_RATE_AMR                  (0x10)
#define CODEC_BITMAP_UMTS_AMR                           (0x20)
#define CODEC_BITMAP_UMTS_AMR2                          (0x40)
#define CODEC_BITMAP_TDMA_ENHANCED_FULL_RATE            (0x80)

/* Codec BitMap bit 16 ... 9 */
#define CODEC_BITMAP_PDC_ENHANCED_FULL_RATE             (0x01)

#define CODEC_BITMAP_GSM_AMR_WB_FULL_RATE             (0x02)



/* Stream Identifier */
typedef struct
{
    byte    present;
    byte    id;
} cm_stream_identifier_T;

/* Emergency Category */
typedef struct
{
    byte    present;
    byte    emergency_category;
} cm_emergency_category_T;

/* Network CC Capability */
typedef struct 
{
    byte    present;
    byte    length;
    byte    capability;
} cm_network_cc_capability_T;

typedef enum
{
   CCBS_RET_RES,
   CCBS_RET_ERR,
   CCBS_REJ
} cm_5_send_conf_e_type;

typedef enum {
   CCBS_LOW = -1, /*declared to include boundary checks*/
   CCBS_ACCEPT,
   CCBS_REJECT,
   CCBS_HOLD_ACCEPT,
   CCBS_REL_ACCEPT,
   CCBS_MAX /*declared to include boundary checks*/
} cm_recall_rsp_e_type;

typedef struct
{
   boolean          present;
   boolean          ccbs_possible;
} ie_cm_allowed_actions_T;

typedef struct 
{
   boolean                         present;
   byte                            invoke_id;
   cm_called_party_bcd_no_T        deflectedToNumber;
   cm_called_party_subaddress_T    defelectedToSubAddress;
} ie_cm_cc_invoke_T;

typedef struct
{
  cm_ss_conf_e_type             data_type;
  union 
  {
    uss_data_T                  uss_data;
    ie_cm_ss_error_T            cm_ss_error;
    ie_cm_cc_cause_T            cm_cc_cause;
  } data;

} ss_data_info_T;


typedef struct
{
  cm_ss_conf_e_type                      data_type;
  union
  {
    ss_User_Data_T                 ss_User_Data;
    ie_cm_ss_error_T               cm_ss_error;
    ie_cm_cc_cause_T               cm_cc_cause;
  } data;

} ss_UserData_info_T;


typedef struct
{
  cm_ss_conf_e_type            data_type;
  union
  {
    ie_ss_password_T           ss_new_password;
    ie_cm_ss_error_T           cm_ss_error;
    ie_cm_cc_cause_T           cm_cc_cause;
  } data;

} ss_password_info_T;


typedef struct
{
   cm_ss_conf_e_type              info_type;
   union
   {
      ie_forwarding_info_T            forwarding_info;
      ie_call_barring_info_T          call_barring_info;
      ie_cug_info_T                   cug_info;
      ie_cm_ss_data_T                ss_data;
      ie_cm_ss_error_T               cm_ss_error;
      ie_cm_cc_cause_T               cm_cc_cause;
   } info;

} ss_info_T;

typedef struct
{
   boolean                       present;

} ie_call_is_waiting_indicator_T;

typedef struct
{
   boolean                         present;
   byte                            call_on_hold;

} ie_call_on_hold_indicator_T;

typedef struct
{
   boolean                       present;

} ie_mpty_indicator_T;

typedef struct
{
  boolean                       present;
  byte                          clir_suppresion_rej;

} ie_clir_suppression_rejected_T;

typedef enum     /* value taken from TS 24.080, section 4.5, ANNEX A */
{
   namePresentationAllowed,
   presentationRestricted, 
   nameUnavailable,
   namePresentationRestricted

} ie_name_presentation_option_T;

typedef struct 
{
    boolean  present;
    byte     dataCodingScheme;
    byte     lengthInCharacter;
    byte     nameStringLength;
    byte     nameString[MAX_CNAP_LENGTH];

} ie_name_presentation_T;

typedef struct 
{
    boolean  present;
    ie_name_presentation_option_T  type;
    ie_name_presentation_T         name;

} ie_name_indicator_T;

typedef struct
{
    boolean  present;
    routing_number_option_T      type; 
    cm_called_party_bcd_no_T     number[MAXNO_FORWARD_TO_NUMBER]; 
    cm_called_party_subaddress_T address;

} ie_route_dest_number_T;

typedef struct 
{
    boolean   present;
    ect_call_state_T    callState; 
    ie_route_dest_number_T routeDestNumber;  /*  routing dest number  */

} ie_ect_indicator_T;
 
typedef struct 
{
    boolean  present;
    byte     pattern;

} ie_alert_pattern_T;

typedef enum 
{
    nbr_SNexceeded, /* maxim number of bearers supported by serving network, Nbr_SN */
    nbr_Userexceeded  /* maximum number of bearers chosen by the user, Nbr_user */

} ie_indicator_T;

typedef struct 
{
    boolean  present;
    ie_indicator_T indicator;

} ie_multicall_indicator_T;

/* TS24.080, section 4.5, annex A, notifySS OPERATION/ARGUMENTS */
typedef struct
{
   boolean                         present;
   ie_ss_status_T                  cm_ss_status;
   ie_ss_code_T                    cm_ss_code;
   ie_ss_notification_T            cm_ss_notification;
   ie_call_is_waiting_indicator_T  cm_call_is_waiting_indicator;
   ie_call_on_hold_indicator_T     cm_call_on_hold_indicator;
   ie_mpty_indicator_T             cm_mpty_indicator;
   ie_cug_index_T                  cm_cug_index;
   ie_clir_suppression_rejected_T  cm_clir_suppression_rejected;
   ie_name_indicator_T             cm_name_indicator;
   ie_ect_indicator_T              cm_ect_indicator;
   ie_ccbs_indicator_T             cm_ccbs_indicator;
   ie_alert_pattern_T              cm_alert_pattern;
   ie_multicall_indicator_T        cm_multicall_indicator;

} ie_cm_notify_ss_T; 

typedef struct 
{
   ie_ss_status_T                 cm_ss_status;
   basic_service_group_list_T     basic_service_group_list;
   ie_forwarding_feature_list_T   forwarding_feature_list;
   cli_restriction_info_T         cli_restriction_info;
   ie_cm_ss_error_T               cm_ss_error;
   ie_cm_cc_cause_T               cm_cc_cause;
   ccbs_info_T                    ccbs_info;

} InterrogateSS_Res_T;

typedef struct
{
   boolean                         present;
   byte                            coding_standard;
   byte                            location;
   byte                            progress_description;

} cm_progress_indicator_T;

typedef struct
{
   boolean                         present;
   byte                            msb;
   byte                            lsb;

} charge_advice_data_T;


typedef struct
{
   boolean                         present;
   charge_advice_data_T            e1;
   charge_advice_data_T            e2;
   charge_advice_data_T            e3;
   charge_advice_data_T            e4;
   charge_advice_data_T            e5;
   charge_advice_data_T            e6;
   charge_advice_data_T            e7;

} ie_cm_forward_charge_advice_req_T;

typedef struct
{
    boolean                    present;
    byte                       signal_value;
} cm_signal_T;

/* MN client - CB function owner */
typedef enum  mn_client_E 
{
    MN_CLIENT_CM,  /* Call Manager */
    MN_CLIENT_DS,      /* Data Services */
    MN_CLIENT_ATCOP,

    MN_MAX_CLIENT
} mn_client_T;

/* MN call back service type */
typedef enum
{
    MN_AOC_RESPONSE,
    MN_SYNC_NOTIFICATION,
    
    MN_MAX_CLIENT_SERVICES
} mn_client_service_T;

typedef struct
{
   boolean                         present;
   word                            size;
   byte                            data[MAX_SS_PARAMETER_LENGTH];
} unparsed_suppsvc_l3_data_T;


/* MN CB Registeration function */
boolean MN_register_cb_services(mn_client_T client, mn_client_service_T type, void *cb_function);


#endif


