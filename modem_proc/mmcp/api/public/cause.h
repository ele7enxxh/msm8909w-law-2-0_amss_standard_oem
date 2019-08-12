#ifndef cause_h
#define cause_h
/*-------------------------------------------------------------------------------------
            C A U S E      H E A D E R     F I L E
-------------------------------------------------------------------------------------*/
/**
@file cause.h
@brief 
Rejection cause definitions.
*/

/*------------------------------------------------------------------------------
     Copyright (c) 2002 - 2010 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
------------------------------------------------------------------------------*/

/*===========================================================================
                                                    EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/cause.h_v   1.9   02 Apr 2002 15:26:06   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/cause.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/12   am      Passing internal cause for internal error while RAB reestablishment
07/27/10   kl/gm   (Tech Pubs) Edited/added Doxygen comments and markup.
05/25/01   jca     Added new GMM cause codes.
06/11/01   TS      Added GSM build support.
10/25/01   CD      Added SMS-RP/CP layer cause codes.
04/02/02   AB      Added CNM cause IE class type.
08/01/02   ks      Added new reject cause fields.
10/03/02   Rao     Added new cause ( for GSM to WCDMA Reselection Support ).
10/07/02   jca     Added new reject cause fields as per 24.008 V3.12.0.
11/22/02   ab      Added new internal cause base value.
09/18/03   mks     Merged Kurt's changes to support silent redial feature -
                   Added new cause "RR_ABORT_REDIAL_NOT_ALLOWED".
16/02/06   NR      Adding new reject cause for handling CNM MO SMS timer expiry
===========================================================================*/

/** @addtogroup CAUSE Rejection Cause Definitions
    @{ */
/*
 * Cause IE class (bit 5-7), taken from TS24.008, table 10.5.122
 */

/** @name Call Control Clause Class
    @{ */

#define  NORMAL_EVENT                           (0x00) 
#define  NORMAL_EVENT_1                         (0x01) 
#define  RESOURCE_UNAVAILABLE                   (0x02) 
#define  SERVICE_OR_OPT_UNAVAILABLE             (0x03) 
#define  SERVIcE_OR_OPT_NOT_IMPLEMENTED         (0x04)
#define  INVALID_MESSAGE                        (0x05) 
#define  PROTOCOL_ERROR                         (0x06) 
#define  INTERWORKING                           (0x07) 

/** @} */

/*
 * These definitions are taken from GSM 04.08 Table 10.86
 */

/** @name Call Control Cause Value
    @{ */

#define  UNASSIGNED_CAUSE                             1 
#define  NO_ROUTE_TO_DEST                             3 
#define  CHANNEL_UNACCEPTABLE                         6 
#define  OPERATOR_DETERMINED_BARRING                  8 
#define  NORMAL_CALL_CLEARING                         16 
#define  USER_BUSY                                    17 
#define  NO_USER_RESPONDING                           18 
#define  USER_ALERTING_NO_ANSWER                      19 
#define  CALL_REJECTED                                21 
#define  NUMBER_CHANGED                               22 
#define  NON_SELECTED_USER_CLEARING                   26 
#define  DESTINATION_OUT_OF_ORDER                     27 
#define  INVALID_NUMBER_FORMAT                        28 
#define  FACILITY_REJECTED                            29 
#define  RESPONSE_TO_STATUS_ENQUIRY                   30 
#define  NORMAL_UNSPECIFIED                           31 
#define  NO_CIRCUIT_CHANNEL_AVAILABLE                 34 
#define  NETWORK_OUT_OF_ORDER                         38 
#define  TEMPORARY_FAILURE                            41 
#define  SWITCHING_EQUIPMENT_CONGESTION               42 
#define  ACCESS_INFORMATION_DISCARDED                 43 
#define  REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE      44 
#define  RESOURCES_UNAVAILABLE_UNSPECIFIED            47 
#define  QUALITY_OF_SERVICE_UNAVAILABLE               49 
#define  REQUESTED_FACILITY_NOT_SUBSCRIBED            50 
#define  INCOMING_CALL_BARRED_WITHIN_CUG              55 
#define  BEARER_CAPABILITY_NOT_AUTHORISED             57 
#define  BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE    58 
#define  SERVICE_OR_OPTION_NOT_AVAILABLE              63 
#define  BEARER_SERVICE_NOT_IMPLEMENTED               65 
#define  ACM_GEQ_ACMMAX                               68 
#define  REQUESTED_FACILITY_NOT_IMPLEMENTED           69 
#define  ONLY_RESTRICTED_DIGITAL_INFO_BC_AVAILABLE    70 
#define  SERVICE_OR_OPTION_NOT_IMPLEMENTED            79 
#define  INVALID_TRANSACTION_ID_VALUE                 81 
#define  USER_NOT_MEMBER_OF_CUG                       87 
#define  INCOMPATIBLE_DESTINATION                     88 
#define  INVALID_TRANSIT_NETWORK_SELECTION            91 
#define  SEMANTICALLY_INCORRECT_MESSAGE               95 
#define  INVALID_MANDATORY_INFORMATION                96 
#define  MESSAGE_TYPE_NON_EXISTENT                    97 
#define  MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE  98 
#define  IE_NON_EXISTENT_OR_NOT_IMPLEMENTED           99 
#define  CONDITIONAL_IE_ERROR                         100 
#define  MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE   101 
#define  RECOVERY_ON_TIMER_EXPIRY                     102 
#define  PROTOCOL_ERROR_UNSPECIFIED                   111 
#define  ACCESS_CLASS_DSAC_REJECTION                  112 
#define  REGISTRATION_IN_PROGRESS                     113
#define  INTERWORKING_UNSPECIFIED                     127 
#define  RAB_REJ_CAUSE_LAST                           INTERWORKING_UNSPECIFIED
#define  RAB_REJ_CAUSE_ALL                            0xFF 
#define  RESERVED                                     11

/** @} */

/* *****************/
/* Reject causes.*/
/* *****************/
/** @name Rejection Causes
    @{ */

#define IMSI_UNKNOWN_IN_HLR                             0x02 
#define ILLEGAL_MS                                      0x03 
#define IMSI_UNKNOWN_IN_VLR                             0x04
#define IMEI_NOT_ACCEPTED                               0x05 
#define ILLEGAL_ME                                      0x06 
#define GPRS_SERVICES_NOT_ALLOWED                       0x07 
#define GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED 0x08 
#define MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK    0x09 
#define IMPLICITLY_DETACHED                             0x0A 
#define PLMN_NOT_ALLOWED                                0x0B 
#define LA_NOT_ALLOWED                                  0x0C 
#define NATIONAL_ROAMING_NOT_ALLOWED                    0x0D 
#define GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN          0x0E 
#define NO_SUITABLE_CELLS_IN_LA                         0x0F 
#define MSC_TEMPORARILY_NOT_REACHABLE                   0x10 
#define NETWORK_FAILURE                                 0x11 
#define MAC_FAILURE                                     0x14 
#define SYNCH_FAILURE                                   0x15 
#define CONGESTTION                                     0x16 
#define GSM_AUTH_UNACCEPTED                             0x17 
#define CSG_NOT_AUTHORIZED                              0x19 
#define SERVICE_OPTION_NOT_SUPPORTED                    0x20 
#define REQ_SERV_OPT_NOT_SUBSCRIBED                     0x21 
#define SERVICE_OPT__OUT_OF_ORDER                       0x22 
#define CALL_CANNOT_BE_IDENTIFIED                       0x26 
#define NO_PDP_CONTEXT_ACTIVATED                        0x28 
#define RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE      0x30 
#define RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE      0x3F 
#define SEMANTICALLY_INCORRECT_MSG                      0x5F 
#define INVALID_MANDATORY_INFO                          0x60 
#define MESSAGE_TYPE_NON_EXISTANT                       0x61 
#define MESSAGE_TYPE_NOT_COMP_PRT_ST                    0x62 
#define IE_NON_EXISTANT                                 0x63 
#define MSG_NOT_COMPATIBLE_PROTOCOL_STATE               0x65

#define MAX_NAS_SPEC_CAUSE_CODE                   MSG_NOT_COMPATIBLE_PROTOCOL_STATE
#define RAB_RB_DS_L2_CLEANUP_COLLISION            MAX_NAS_SPEC_CAUSE_CODE+1

#define AUTHENTICATION_REJECTED                         0xFF
/** MN internal error for WMS in the case of Connection Manager timer
    expiration. */
#define NO_RESPONSE_FROM_NEWTWORK      126

/** Abort service is not available. */
#define ABORT_SERVICE_NOT_AVAILABLE    0x22

/** @} */

/* *******************************/
/*  RR causes sent over the air */
/* *******************************/
/** @name Radio Resource Causes Sent Over the Air
    @{ */

#define RR_NORMAL_EVENT                                  0x00 
#define RR_ABNORMAL_RELEASE_UNSPECIFIED                  0x01 
#define RR_ABNORMAL_RELEASE_CHANNEL_UNACCEPTABLE         0x02 
#define RR_ABNORMAL_RELEASE_TIMER_EXPIRED                0x03 
#define RR_ABNORMAL_RELEASE_NO_ACTIVITY_ON_RADIO_PATH    0x04 
#define RR_PREEMTIVE_RELEASE                             0x05 
#define RR_UTRAN_CONFIGURATION_UNKNOWN                   0x06 
#define RR_HANDOVER_IMPOSSIBLE_TA_OUT_OF_RANGE           0x08 
#define RR_CHANNEL_MODE_UNACCEPTABLE                     0x09 
#define RR_FREQUENCY_NOT_IMPLEMENTED                     0x0a 
#define RR_LOWER_LAYER_FAILURE                           0x0c 
#define RR_CALL_ALREADY_CLEARED                          0x41 
#define RR_SEMANTICALLY_INCORRECT_MESSAGE                0x5f 
#define RR_INVALID_MANDATORY_INFORMATION                 0x60 
#define RR_MESSAGE_TYPE_NON_EXISTENT                     0x61 
#define RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE   0x62 
#define RR_CONDITIONAL_IE_ERROR                          0x64 
#define RR_NO_CELL_ALLOCATION_AVAILABLE                  0x65 
#define RR_PROTOCOL_ERROR_UNSPECIFIED                    0x6f 
#define RR_INTERRAT_RESELECTION_IN_PROGRESS              0x71 

  /** Mobile Station internally defined cause base value. */
#define MS_INTERNAL_CAUSE_VALUE_BASE                     0xA0 
#define MSG_NOT_COMP_WITH_PROT_ST                       0x65 
#define AUTH_MAC_ERROR    0x14
#define AUTH_SYNCH_ERROR  0x15 
/** @} */

/*
 RR abort causes sent to MM
*/
/** @name Radio Resource Causes Sent to MM
    @{ */

#define REASON_NOT_AVAILABLE         0x00 
#define ACCESS_CLASS_NOT_ALLOWED     0x01 
#define RANDOM_ACCESS_FAILURE        0x02 
#define OTHER_REASON                 0x03 
#define RR_CONNECTION_FAILURE        0x04 
#define REASON_INTERRAT_RESELECTION  0x05 
#define RR_ABORT_REDIAL_NOT_ALLOWED  0x06 
#define RR_ABORT_LAI_CHANGED         0X07 
#define RR_ABORT_GTOW_REDIRECTION_IN_PROGRESS 0x08 
#define RR_ABORT_IA_REJECT_RECEIVED  0x09 
#define RR_ABORT_CON_EST_NOT_ATTEMPTED  0x0A
/** @} */

/*
  Values for coding standard field -- also used by progress indication IE.
*/

/** @name Coding Standard Field Values
    @{ */

/** Coding, as specified in ITU-T.
*/
#define STANDARD_CCITT     0x00 

/** Coding reserved for international standards.
*/
#define STANDARD_INTERNAT  0x20 

/** Coding reserved for national standards.
*/
#define STANDARD_NATIONAL  0x40 

/** Standard defined for the GSM PLMNs.
*/
#define STANDARD_GSM       0x60 

/** @} */

/*
 Values for location field -- also used by progress indication IE.
*/
/** @name Location Field Values
    @{ */

/** Location user.
*/
#define LOCATION_USER      0x00 

/** Private network serving the local user.
*/
#define LOCATION_PRI_LOC   0x01 

/** Public network serving the local user.
*/
#define LOCATION_PUB_LOC   0x02 

/** Transit network.
*/
#define LOCATION_TRANSIT   0x03 

/** Public network serving the remote user.
*/
#define LOCATION_PUB_REM   0x04 

/** Private network serving the remote user.
*/
#define LOCATION_PRI_REM   0x05 

/** International network.
*/
#define LOCATION_INTERNAT  0x07 

/** Network beyond the interworking point.
*/
#define LOCATION_BEYOND    0x0a 

/** @} */

/* ********************************************************
* SMS RP/CP causes not included in 24.008/table 10.5.123 *
**********************************************************/

/* TS 24.011 table 8.2 */
/** @name SMS RP/CP Causes Not in TS 24.008
    @{ */

#define CP_NETWORK_FAILURE                  17 
#define CP_CONGESTION                       22 

/* TS 24.011 table 8.4 ( part 1) */

#define CALL_BARRED                         10 
#define SHORT_MESSAGE_TRANSFER_REJECTED     21 
#define UNIDENTIFIED_SUBSCRIBER             28 
#define UNKNOWN_SUBSCRIBER                  30 

/* TS 24.011 table 8.4 ( part 2) */

#define MEMORY_CAPACITY_EXCEEDED            22 
#define INVALID_SHORT_MESSAGE_REF_VALUE     81

/** @} */

/** @} */ /* end_addtogroup CAUSE */

#endif /* cause_h */

