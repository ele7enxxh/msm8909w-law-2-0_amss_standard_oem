#ifndef _DS_QMI_VOICE_CM_SUPS_H
#define _DS_QMI_VOICE_CM_SUPS_H
/*===========================================================================

                         D S _ Q M I _ V O I C E _ C M _ SUPS. H

DESCRIPTION

 The Data Services Qualcomm Voice supplementary services util header file.


EXTERNALIZED FUNCTIONS



Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_cm_sups.h#1 $ $DateTime: 2010/03/16 22:23:17 $ $Author: ndanturi $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/15/09    nd     Initial revision.
===========================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "cm.h"
#include "qmi_voice_cm_if.h"
#include "ds_qmi_defs.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/* TELE-SERVICES */
#define qmi_voice_cm_sups_allTeleservices                    0x00
#define qmi_voice_cm_sups_allSpeechTransmissionservices      0x10
#define qmi_voice_cm_sups_telephony                          0x11
#define qmi_voice_cm_sups_emergencyCalls                     0x12
#define qmi_voice_cm_sups_allShortMessageServices            0x20
#define qmi_voice_cm_sups_shortMessageMT_PP                  0x21
#define qmi_voice_cm_sups_shortMessageMO_PP                  0x22
#define qmi_voice_cm_sups_allFacsimileTransmissionServices   0x60
#define qmi_voice_cm_sups_facsimileGroup3AndAlterSpeech      0x61
#define qmi_voice_cm_sups_automaticFacsimileGroup3           0x62
#define qmi_voice_cm_sups_facsimileGroup4                    0x63
#define qmi_voice_cm_sups_allDataTeleservices                0x70
#define qmi_voice_cm_sups_allTeleservices_ExeptSMS           0x80
//Voice Group Call Tele Services
#define qmi_voice_cm_sups_allVoiceGroupCallServices          0x90
#define qmi_voice_cm_sups_voiceGroupCall                     0x91
#define qmi_voice_cm_sups_voiceBroadcastCall                 0x92
//PLMN specific Tele Services
#define qmi_voice_cm_sups_ts_all                             0xd0
#define qmi_voice_cm_sups_ts_1                               0xd1
#define qmi_voice_cm_sups_ts_2                               0xd2
#define qmi_voice_cm_sups_ts_3                               0xd3
#define qmi_voice_cm_sups_ts_4                               0xd4
#define qmi_voice_cm_sups_ts_5                               0xd5
#define qmi_voice_cm_sups_ts_6                               0xd6
#define qmi_voice_cm_sups_ts_7                               0xd7
#define qmi_voice_cm_sups_ts_8                               0xd8
#define qmi_voice_cm_sups_ts_9                               0xd9
#define qmi_voice_cm_sups_ts_a                               0xda
#define qmi_voice_cm_sups_ts_b                               0xdb
#define qmi_voice_cm_sups_ts_c                               0xdc
#define qmi_voice_cm_sups_ts_d                               0xdd
#define qmi_voice_cm_sups_ts_e                               0xde
#define qmi_voice_cm_sups_ts_f                               0xdf

/* BEARER-SERVICES */

#define qmi_voice_cm_sups_allBearerServices                 0x00
#define qmi_voice_cm_sups_allDataCDA_Services               0x10
#define qmi_voice_cm_sups_dataCDA_300bps                    0x11
#define qmi_voice_cm_sups_dataCDA_1200bps                   0x12
#define qmi_voice_cm_sups_dataCDA_1200_75bps                0x13
#define qmi_voice_cm_sups_dataCDA_2400bps                   0x14
#define qmi_voice_cm_sups_dataCDA_4800bps                   0x15
#define qmi_voice_cm_sups_dataCDA_9600bps                   0x16
#define qmi_voice_cm_sups_general_dataCDA                   0x17
#define qmi_voice_cm_sups_allDataCDS_Services               0x18
#define qmi_voice_cm_sups_dataCDS_1200bps                   0x1A
#define qmi_voice_cm_sups_dataCDS_2400bps                   0x1C
#define qmi_voice_cm_sups_dataCDS_4800bps                   0x1D
#define qmi_voice_cm_sups_dataCDS_9600bps                   0x1E
#define qmi_voice_cm_sups_general_dataCDS                   0x1F
#define qmi_voice_cm_sups_allPadAccessCA_Services           0x20
#define qmi_voice_cm_sups_padAccessCA_300bps                0x21
#define qmi_voice_cm_sups_padAccessCA_1200bps               0x22
#define qmi_voice_cm_sups_padAccessCA_1200_75bps            0x23
#define qmi_voice_cm_sups_padAccessCA_2400bps               0x24
#define qmi_voice_cm_sups_padAccessCA_4800bps               0x25
#define qmi_voice_cm_sups_padAccessCA_9600bps               0x26
#define qmi_voice_cm_sups_general_padAccessCA               0x27
#define qmi_voice_cm_sups_allDataPDS_Services               0x28
#define qmi_voice_cm_sups_dataPDS_2400bps                   0x2C
#define qmi_voice_cm_sups_dataPDS_4800bps                   0x2D
#define qmi_voice_cm_sups_dataPDS_9600bps                   0x2E
#define qmi_voice_cm_sups_general_dataPDS                   0x2F
#define qmi_voice_cm_sups_allAlternateSpeech_DataCDA        0x30
#define qmi_voice_cm_sups_allAlternateSpeech_DataCDS        0x38
#define qmi_voice_cm_sups_allSpeechFollowedByDataCDA        0x40
#define qmi_voice_cm_sups_allSpeechFollowedByDataCDS        0x48
#define qmi_voice_cm_sups_allDataCircuitAsynchronous        0x50
#define qmi_voice_cm_sups_allAsynchronousServices           0x60
#define qmi_voice_cm_sups_allDataCircuitSynchronous         0x58
#define qmi_voice_cm_sups_allSynchronousServices            0x68

#define  QMI_VOICE_CM_SUPS_UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )

#define QMI_VOICE_CM_SUPS_TA_UNKNOWN       129 /* 0x80|CM_TON_UNKNOWN      |CM_NPI_ISDN */
#define QMI_VOICE_CM_SUPS_TA_INTERNATIONAL 145 /* 0x80|CM_TON_INTERNATIONAL|CM_NPI_ISDN */
#define QMI_VOICE_CM_SUPS_TA_INTER_PREFIX  '+' /* ETSI international call dial prefix */
#define CHAR_CR           0x0D
#define MAX_MT_USSD_CHAR    183  /* (160*8)/7 Max num of char is MT USS data */
#define QMI_VOICE_CM_SUPS_USS_DEF_ALPHABET_LANG_UNSPEC     0x0F  /*default coding scheme */

#define   QMI_VOICE_CM_SUPS_INVOKE_PROBLEM                        0x81
#define   QMI_VOICE_CM_SUPS_UNRECOGNISED_OPERATION                0x01
#define   QMI_VOICE_CM_SUPS_FACILITY_REJECTED                     29
#define   QMI_VOICE_CM_SUPS_STATUS_ACTIVE                         0x01
#define   QMI_VOICE_CM_SUPS_STATUS_IN_ACTIVE                      0x00

#define   QMI_VOICE_CM_SUPS_TIME_MONTH_MAX   12
#define   QMI_VOICE_CM_SUPS_TIME_MONTH_MIN    1
#define   QMI_VOICE_CM_SUPS_TIME_DAY_MAX     31
#define   QMI_VOICE_CM_SUPS_TIME_DAY_MIN      1
#define   QMI_VOICE_CM_SUPS_TIME_HOUR_MAX    23
#define   QMI_VOICE_CM_SUPS_TIME_HOUR_MIN     0
#define   QMI_VOICE_CM_SUPS_TIME_MINUTE_MAX  59
#define   QMI_VOICE_CM_SUPS_TIME_MINUTE_MIN   0
#define   QMI_VOICE_CM_SUPS_TIME_SECOND_MAX  59
#define   QMI_VOICE_CM_SUPS_TIME_SECOND_MIN   0

/* Possible combinations for the class values for supplementary services
    taken from ATCOP spec 27.007 */
typedef enum qmi_voice_cm_sups_class_e
{
  QMI_VOICE_CM_SUPS_CLASS_MIN                 = 0x0,
  QMI_VOICE_CM_SUPS_CLASS_VOICE               = 0x1,
  QMI_VOICE_CM_SUPS_CLASS_DATA                = 0x2,
  QMI_VOICE_CM_SUPS_CLASS_FAX                 = 0x4,
  QMI_VOICE_CM_SUPS_CLASS_SMS                 = 0x8,
  QMI_VOICE_CM_SUPS_CLASS_GROUP_CALL          = 0x0100,
  QMI_VOICE_CM_SUPS_CLASS_BROADCAST_CALL      = 0x0200,
  QMI_VOICE_CM_SUPS_CLASS_ALL_GROUP_CALL      = ( QMI_VOICE_CM_SUPS_CLASS_GROUP_CALL |
                                            QMI_VOICE_CM_SUPS_CLASS_BROADCAST_CALL ),
  QMI_VOICE_CM_SUPS_ALL_TELE_SERV             = ( QMI_VOICE_CM_SUPS_CLASS_VOICE |
                                            QMI_VOICE_CM_SUPS_CLASS_SMS |
                                            QMI_VOICE_CM_SUPS_CLASS_FAX ),
  QMI_VOICE_CM_SUPS_ALL_TELE_SERV_EXT         = ( QMI_VOICE_CM_SUPS_ALL_TELE_SERV |
                                            QMI_VOICE_CM_SUPS_CLASS_ALL_GROUP_CALL ),
  QMI_VOICE_CM_SUPS_ALL_TELE_SERV_EX_SMS      = ( QMI_VOICE_CM_SUPS_CLASS_VOICE |
                                            QMI_VOICE_CM_SUPS_CLASS_FAX ),
  QMI_VOICE_CM_SUPS_CLASS_ALL_TS_DATA         = ( QMI_VOICE_CM_SUPS_CLASS_FAX |
                                            QMI_VOICE_CM_SUPS_CLASS_SMS ),
  QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC           = 0x10,
  QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC          = 0x20,
  QMI_VOICE_CM_SUPS_CLASS_DATA_PKT            = 0x40,
  QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_SYNC       = ( QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC |
                                            QMI_VOICE_CM_SUPS_CLASS_DATA_PKT ),
  QMI_VOICE_CM_SUPS_CLASS_DATA_PAD            = 0x80,
  QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_ASYNC      = ( QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC |
                                            QMI_VOICE_CM_SUPS_CLASS_DATA_PAD),
  QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_SYNC_ASYNC = ( QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC |
                                            QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC ),
  QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_PDS        = ( QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC |
                                            QMI_VOICE_CM_SUPS_CLASS_VOICE ),
  QMI_VOICE_CM_SUPS_CLASS_DATA_ALL            = 0xF0,
  QMI_VOICE_CM_SUPS_CLASS_ALL                 = 0xFF,
  QMI_VOICE_PLMN_SPECIFIC_TS_ALL			= 0xd000,
  QMI_VOICE_PLMN_SPECIFIC_TS_1				= 0xd100,
  QMI_VOICE_PLMN_SPECIFIC_TS_2				= 0xd200,
  QMI_VOICE_PLMN_SPECIFIC_TS_3				= 0xd300,
  QMI_VOICE_PLMN_SPECIFIC_TS_4				= 0xd400,
  QMI_VOICE_PLMN_SPECIFIC_TS_5				= 0xd500,
  QMI_VOICE_PLMN_SPECIFIC_TS_6				= 0xd600,
  QMI_VOICE_PLMN_SPECIFIC_TS_7				= 0xd700,
  QMI_VOICE_PLMN_SPECIFIC_TS_8				= 0xd800,
  QMI_VOICE_PLMN_SPECIFIC_TS_9				= 0xd900,
  QMI_VOICE_PLMN_SPECIFIC_TS_A				= 0xda00,
  QMI_VOICE_PLMN_SPECIFIC_TS_B				= 0xdb00,
  QMI_VOICE_PLMN_SPECIFIC_TS_C				= 0xdc00,
  QMI_VOICE_PLMN_SPECIFIC_TS_D				= 0xdd00,
  QMI_VOICE_PLMN_SPECIFIC_TS_E				= 0xde00,
  QMI_VOICE_PLMN_SPECIFIC_TS_F				= 0xdf00,
  QMI_VOICE_CM_SUPS_CLASS_MAX                 = 0xFFFF
} qmi_voice_cm_sups_class_e_type;

/* Call Independent supplementary services modes
     like de-activation, activation, registration , erasure etc.. 
*/
typedef enum qmi_voice_cm_sups_mode_e
{
  QMI_VOICE_CM_SUPS_MODE_NONE       = -1, /*internal use only*/
  QMI_VOICE_CM_SUPS_MODE_DISABLE    = 0,
  QMI_VOICE_CM_SUPS_MODE_ENABLE     = 1,
  QMI_VOICE_CM_SUPS_MODE_QUERY      = 2,
  QMI_VOICE_CM_SUPS_MODE_REG        = 3,
  QMI_VOICE_CM_SUPS_MODE_ERASURE    = 4,
  QMI_VOICE_CM_SUPS_MODE_REG_PASSWD = 5,
  QMI_VOICE_CM_SUPS_MODE_MAX
} qmi_voice_cm_sups_mode_e_type;

typedef enum qmi_voice_cm_sups_service_e
{
  QMI_VOICE_CM_SUPS_SERVICE_ACTIVATE     = 1,
  QMI_VOICE_CM_SUPS_SERVICE_DEACTIVATE      = 2,
  QMI_VOICE_CM_SUPS_SERVICE_REGISTER       = 3,
  QMI_VOICE_CM_SUPS_SERVICE_ERASE    = 4,
  QMI_VOICE_CM_SUPS_SERVICE_MAX
} qmi_voice_cm_sups_service_e_type;

/* different types of call forwarding SS */
typedef enum qmi_voice_cm_sups_ccfc_reason_e
{
  QMI_VOICE_CM_SUPS_CCFC_REASON_UNCOND    = 1,
  QMI_VOICE_CM_SUPS_CCFC_REASON_BUSY      = 2,
  QMI_VOICE_CM_SUPS_CCFC_REASON_NOREPLY   = 3,
  QMI_VOICE_CM_SUPS_CCFC_REASON_NOTREACH  = 4,
  QMI_VOICE_CM_SUPS_CCFC_REASON_ALLCALL   = 5,
  QMI_VOICE_CM_SUPS_CCFC_REASON_ALLCOND   = 6,
  QMI_VOICE_CM_SUPS_CCFC_REASON_MAX
} qmi_voice_cm_sups_ccfc_reason_e_type;

typedef enum qmi_voice_cm_sups_supps_notification_mo_e
{
  QMI_VOICE_CM_SUPS_CSSI_ORIG_FWD_STAT         = -1,
  QMI_VOICE_CM_SUPS_CSSI_UNCOND_FWD_ACTIVE     = 0,
  QMI_VOICE_CM_SUPS_CSSI_COND_FWD_ACTIVE       = 1,
  QMI_VOICE_CM_SUPS_CSSI_CALL_FORWARDED        = 2,
  QMI_VOICE_CM_SUPS_CSSI_CALL_WAITING          = 3,
  QMI_VOICE_CM_SUPS_CSSI_CUG_CALL              = 4,
  QMI_VOICE_CM_SUPS_CSSI_OUTGOING_CALLS_BARRED = 5,
  QMI_VOICE_CM_SUPS_CSSI_INCOMING_CALLS_BARRED = 6,
  QMI_VOICE_CM_SUPS_CSSI_CLIR_SUPPRESSION_REJ  = 7,
  QMI_VOICE_CM_SUPS_CSSI_CALL_DEFLECTED        = 8,
  QMI_VOICE_CM_SUPS_CSSI_MAX 
}qmi_voice_cm_sups_supps_notification_mo_e_type;

typedef enum qmi_voice_cm_sups_supps_notification_mt_e
{
  QMI_VOICE_CM_SUPS_CSSU_FORWARDED_CALL                = 0,
  QMI_VOICE_CM_SUPS_CSSU_CUG_CALL                      = 1,
  QMI_VOICE_CM_SUPS_CSSU_CALL_HOLD                     = 2,
  QMI_VOICE_CM_SUPS_CSSU_CALL_RETRIEVED                = 3,
  QMI_VOICE_CM_SUPS_CSSU_MPTY_CALL                     = 4,
  QMI_VOICE_CM_SUPS_CSSU_CALL_HOLD_RELEASED            = 5,
  QMI_VOICE_CM_SUPS_CSSU_FWD_CHECK_SS_RECVD            = 6,
  QMI_VOICE_CM_SUPS_CSSU_ECT_CALL_REMOTE_PTY_ALERT     = 7,
  QMI_VOICE_CM_SUPS_CSSU_ECT_CALL_REMOTE_PTY_CONNECTED = 8,
  QMI_VOICE_CM_SUPS_CSSU_DEFLECTED_CALL                = 9,
  QMI_VOICE_CM_SUPS_CSSU_ADDITIONAL_INCOM_CALL_FWD     = 10,
  QMI_VOICE_CM_SUPS_CSSU_MAX 
}qmi_voice_cm_sups_supps_notification_mt_e_type;

typedef enum qmi_voice_cm_sups_notification_ect_call_state_e
{
  qmi_voice_cm_sups_notification_alerting_ECT, 
  qmi_voice_cm_sups_notification_alerting_active_ECT 

} qmi_voice_cm_sups_notification_ect_call_state_e_type;

typedef enum qmi_voice_cm_sups_notification_type_e
{
 QMI_VOICE_CM_SUPS_MIN_NOTIFICATION = -1,
 QMI_VOICE_CM_SUPS_MO_NOTIFICATION = 0,
 QMI_VOICE_CM_SUPS_MT_NOTIFICATION = 1,
 QMI_VOICE_CM_SUPS_MAX_NOTIFICATION
}qmi_voice_cm_sups_notification_type_e_type;

#define QMI_VOICE_CM_SUPS_CCFC_NUM_MAX_LEN 80

/*  Network SS Codes taken from mn_cm_exp.h
*/

typedef enum qmi_voice_cm_sups_operation_code_e{
       qmi_voice_cm_sups_all                     =   0x00,
       qmi_voice_cm_sups_allLineIdentificationSS =   0x10, /* all_line_identification */
       qmi_voice_cm_sups_clip                    =   0x11, /* calling_line_identification_
                                                         presentation_service */
       qmi_voice_cm_sups_clir                    =   0x12, /* calling_line_identification_
                                                         restriction_service
                                                presentation_service */
       qmi_voice_cm_sups_colp                    =   0x13, /* connected_line_identification_
                                            presentation_service */
       qmi_voice_cm_sups_colr                    =   0x14, /* connected_line identification_
                                             restriction_service */
       qmi_voice_cm_sups_cnap                    =   0x19, /* call name identification presentation */
       qmi_voice_cm_sups_mci                     =   0x1a, /* malicious call identify */
       qmi_voice_cm_sups_allForwardingSS         =   0x20, /* all_call_forwarding */
       qmi_voice_cm_sups_cfu                     =   0x21, /*  call_forwarding_unconditional */
       qmi_voice_cm_sups_cd                      =   0x24, /* call deflection */
       qmi_voice_cm_sups_allCondForwardingSS     =   0x28, /* all conditional call forwarding  */
       qmi_voice_cm_sups_cfb                     =   0x29, /* call_forwarding_on_mobile_sub-
                                                scriber_busy */
       qmi_voice_cm_sups_cfnry                   =   0x2a, /* call_forwarding_on_no_reply */
       qmi_voice_cm_sups_cfnrc                   =   0x2b, /* call_forwarding_on_mobile_subsc-
                                                riber_unreachable */
       qmi_voice_cm_sups_allCallOfferingSS       =   0x30, /* all call offering sub-services*/
       qmi_voice_cm_sups_ect                     =   0x31, /* call transfer */
       qmi_voice_cm_sups_mah                     =   0x32, /* mobile access hunting */
       qmi_voice_cm_sups_allCallCompletionSS     =   0x40, /* all call completion */
       qmi_voice_cm_sups_cw                      =   0x41, /* call waiting */
       qmi_voice_cm_sups_hold                    =   0x42,
       qmi_voice_cm_sups_ccbs                    =   0x43, /* completion of call to busy 
                                                                 subscribers */
       qmi_voice_cm_sups_allMultiPartySS         =   0x50, /* all multi-party services */
       qmi_voice_cm_sups_multiPTY                =   0x51, /* multi_party_service */
       qmi_voice_cm_sups_allCommunityOfInterest_SS  = 0x60, /* *** NB name deviates from 
                                               09.02 ***/
       qmi_voice_cm_sups_cug                     =   0x61, /* closed_user_group */
       qmi_voice_cm_sups_allChargingSS           =   0x70,

       qmi_voice_cm_sups_aoci                    =   0x71, /* advice_of_charge_information */
       qmi_voice_cm_sups_aocc                    =   0x72, /* advice_of_charge_charge */

       qmi_voice_cm_sups_allAdditionalInfoTransferSS = 0x80,
       qmi_voice_cm_sups_uus                     =   0x81, /* user to user signalling */
       qmi_voice_cm_sups_allCallRestrictionSS    =   0x90,
       qmi_voice_cm_sups_barringOfOutgoingCalls  =   0x91,
       qmi_voice_cm_sups_baoc                    =   0x92, /* barring_of_all_outgoing_calls */
       qmi_voice_cm_sups_boic                    =   0x93, /* barring_of_outgoing_inter-
                                                national_calls */
       qmi_voice_cm_sups_boicExHC                =   0x94, /* barring_of_outgoing_inter-
                                                national_calls_except_those_
                                                to_home_plmn */
       qmi_voice_cm_sups_barringOfIncomingCalls  =   0x99,
       qmi_voice_cm_sups_baic                    =   0x9a, /* barring of all incoming calls */
       qmi_voice_cm_sups_bicRoam                 =   0x9b, /* barring of incomming calls when
                                                roaming outside home PLMN 
                                                Country */
       qmi_voice_cm_sups_bil                     = 0x9c,   /*Barring incoming from a list of number */
       qmi_voice_cm_sups_bin                     = 0x9d,   /*Barring incoming from anonymous numbers */
       qmi_voice_cm_sups_allPLMN_specificSS      =   0xf0, /* all PLMN specific Supplementary
                                                services *** NB name deviates
                                                from 09.02 ***/
       qmi_voice_cm_sups_chargingInfoId          =   0xa1
} qmi_voice_cm_sups_operation_code_e_type;


/*  facility values for FACLITY LOCK supplimentary services
    taken from 27.007 7.4 section
*/
typedef enum qmi_voice_cm_sups_facility_lock_string_code_e
{
  QMI_VOICE_CM_SUPS_LOCK_CS    = 0,
  QMI_VOICE_CM_SUPS_LOCK_PS    = 1,
  QMI_VOICE_CM_SUPS_LOCK_PF    = 2,
  QMI_VOICE_CM_SUPS_LOCK_SC    = 3,
  QMI_VOICE_CM_SUPS_LOCK_AO    = 4,
  QMI_VOICE_CM_SUPS_LOCK_OI    = 5,
  QMI_VOICE_CM_SUPS_LOCK_OX    = 6,
  QMI_VOICE_CM_SUPS_LOCK_AI    = 7,
  QMI_VOICE_CM_SUPS_LOCK_IR    = 8,
  QMI_VOICE_CM_SUPS_LOCK_NT    = 9,
  QMI_VOICE_CM_SUPS_LOCK_NM    = 10,
  QMI_VOICE_CM_SUPS_LOCK_NS    = 11,
  QMI_VOICE_CM_SUPS_LOCK_NA    = 12,
  QMI_VOICE_CM_SUPS_LOCK_AB    = 13,
  QMI_VOICE_CM_SUPS_LOCK_AG    = 14,
  QMI_VOICE_CM_SUPS_LOCK_AC    = 15, 
  QMI_VOICE_CM_SUPS_LOCK_FD    = 16,
  QMI_VOICE_CM_SUPS_LOCK_PN    = 17,
  QMI_VOICE_CM_SUPS_LOCK_PU    = 18,
  QMI_VOICE_CM_SUPS_LOCK_PP    = 19,   
  QMI_VOICE_CM_SUPS_LOCK_PC    = 20,   
  QMI_VOICE_CM_SUPS_LOCK_MAX
} qmi_voice_cm_sups_facility_lock_string_code_e_type;

typedef enum qmi_voice_cm_sups_reason_code_e
{
  QMI_VOICE_CM_SUPS_REASON_NONE   = 0x00,
  QMI_VOICE_CM_SUPS_FWD_UNCONDITIONAL    = 0x01,
  QMI_VOICE_CM_SUPS_FWD_MOBILEBUSY    = 0x02,
  QMI_VOICE_CM_SUPS_FWD_NOREPLY    = 0x03,
  QMI_VOICE_CM_SUPS_FWD_UNREACHABLE   = 0x04,
  QMI_VOICE_CM_SUPS_FWD_ALLFORWARDING    = 0x05,
  QMI_VOICE_CM_SUPS_FWD_ALLCONDITIONAL    = 0x06,
  QMI_VOICE_CM_SUPS_BARR_ALLOUTGOING    = 0x07,
  QMI_VOICE_CM_SUPS_BARR_OUTGOINGINT    = 0x08,
  QMI_VOICE_CM_SUPS_BARR_OUTGOINGINTEXTOHOME   = 0x09,
  QMI_VOICE_CM_SUPS_BARR_ALLINCOMING   = 0x0A,
  QMI_VOICE_CM_SUPS_BARR_INCOMINGROAMING    = 0x0B,
  QMI_VOICE_CM_SUPS_BARR_ALLBARRING    = 0x0C,
  QMI_VOICE_CM_SUPS_BARR_ALLOUTGOINGBARRING    = 0x0D,
  QMI_VOICE_CM_SUPS_BARR_ALLINCOMINGBARRING    = 0x0E,
  QMI_VOICE_CM_SUPS_CALLWAITING    = 0x0F,
  QMI_VOICE_CM_SUPS_CLIP    = 0x10, 
  QMI_VOICE_CM_SUPS_CLIR    = 0x11, 
  QMI_VOICE_CM_SUPS_COLP    = 0x12,
  QMI_VOICE_CM_SUPS_COLR    = 0x13,
  QMI_VOICE_CM_SUPS_CNAP    = 0x14,
  QMI_VOICE_CM_SUPS_BARR_INCOMINGNUMBER    = 0x15,
  QMI_VOICE_CM_SUPS_BARR_INCOMINGANONYMOUS    = 0x16,
  QMI_VOICE_CM_SUPS_REASON_MAX
} qmi_voice_cm_sups_reason_code_e_type;

typedef enum qmi_voice_cm_sups_service_type_e
{
QMI_VOICE_CM_SUPS_SERVICE_TYPE_ACTIVATE =0x01,
QMI_VOICE_CM_SUPS_SERVICE_TYPE_DEACTIVATE =0x02,
QMI_VOICE_CM_SUPS_SERVICE_TYPE_REGISTER =0x03,
QMI_VOICE_CM_SUPS_SERVICE_TYPE_ERASE =0x04,
QMI_VOICE_CM_SUPS_SERVICE_TYPE_INTERROGATE =0x05,
QMI_VOICE_CM_SUPS_SERVICE_TYPE_REGISTER_PASSWORD =0x06,
QMI_VOICE_CM_SUPS_SERVICE_TYPE_USSD =0x07,
QMI_VOICE_CM_SUPS_SERVICE_TYPE_INVALID               =0xff
}qmi_voice_cm_sups_service_type_e_type;

/* This enum represents the CLIR provisioning status at the network */
typedef enum qmi_voice_cm_sups_clir_status_e {
  QMI_VOICE_CM_SUPS_CLIR_SRV_NOT_PROVISIONED         = 0x00,
  QMI_VOICE_CM_SUPS_CLIR_SRV_PROVISIONED_PERMANENT   = 0x01,
  QMI_VOICE_CM_SUPS_CLIR_SRV_PRESENTATION_RESTRICTED = 0x02,
  QMI_VOICE_CM_SUPS_CLIR_SRV_PRESENTATION_ALLOWED    = 0x03 
} qmi_voice_cm_sups_clir_status_e_type;

/* CLIR Active status */
typedef enum qmi_voice_cm_sups_clir_active_status_e{
  QMI_VOICE_CM_SUPS_CLIR_INACTIVE = 0,
  QMI_VOICE_CM_SUPS_CLIR_ACTIVE = 1
}qmi_voice_cm_sups_clir_active_status_e_type;


/* SVC enable/disable notification settings */
typedef enum qmi_voice_cm_sups_supps_notification_e{
  QCRIl_CM_SS_DISABLE_NOTIFICATION = 0,
  QMI_VOICE_CM_SUPS_ENABLE_NOTIFICATION = 1
}qmi_voice_cm_sups_supps_notification_e_type;

/* States for USSD */
//typedef enum qmi_voice_cm_sups_ussd_state_e
//{
//  QMI_VOICE_CM_SUPS_USSD_STATE_NULL,            /* No pending USSD transaction */
//  QMI_VOICE_CM_SUPS_USSD_STATE_MO_REQ,          /* MS initiated request  */
//  QMI_VOICE_CM_SUPS_USSD_STATE_MT_REQ,          /* MT initiated request */
//  QMI_VOICE_CM_SUPS_USSD_STATE_MT_NOTIFY,       /* MT initiated notification */  
//  QMI_VOICE_CM_SUPS_USSD_STATE_MS_ABORT,        /* MS abort of transaction     */  
//  QMI_VOICE_CM_SUPS_USSD_STATE_MAX
//} qmi_voice_cm_sups_ussd_state_e_type;


typedef enum qmi_voice_cm_sups_ussd_result_e
{
  QMI_VOICE_CM_SUPS_CUSD_RESULT_DONE    = 0,    /* No further action required   */
  QMI_VOICE_CM_SUPS_CUSD_RESULT_MORE    = 1,    /* Further user action required */
  QMI_VOICE_CM_SUPS_CUSD_RESULT_ABORT   = 2,    /* USSD terminated by network   */
  QMI_VOICE_CM_SUPS_CUSD_RESULT_OTHER   = 3,    /* Other local client responded */
  QMI_VOICE_CM_SUPS_CUSD_RESULT_NOSUP   = 4,    /* Operation not supported      */
  QMI_VOICE_CM_SUPS_CUSD_RESULT_TIMEOUT = 5,    /* Network time out             */
  QMI_VOICE_CM_SUPS_CUSD_RESULT_MAX
} qmi_voice_cm_sups_ussd_result_e_type;


/* supplimentary services operations taken from mn_cm_exp.h */
#define qmi_voice_cm_sups_processUnstructuredSS_Request   59
#define qmi_voice_cm_sups_unstructuredSS_Request          60
#define qmi_voice_cm_sups_unstructuredSS_Notify           61

/* For mapping the ATCOP class value to basic service type and code
    which is recognized by the network
*/
typedef struct qmi_voice_cm_sups_bs_mapping_s
{
  qmi_voice_cm_sups_class_e_type bs_class; 
  uint8  bs_type;   /* BS type - bearer services, teleservices */
  uint8  bs_code;   /* BS Code */
} qmi_voice_cm_sups_bs_mapping_s_type;

typedef struct
{
   boolean                       present;
   char                          password[MAX_PWD_CHAR];

} qmi_voice_cm_sups_password_T;

/* Supplimentary service command information */
typedef union qmi_voice_cm_sups_sups_cmd_parm_u
{
  cm_reg_ss_params_s_type      reg;            /* Registration        */
  cm_erase_ss_params_s_type    erase;          /* Erasure             */
  cm_act_ss_params_s_type      act;            /* Activation          */
  cm_deact_ss_params_s_type    deact;          /* Deactivation        */
  cm_interr_ss_params_s_type   interr;         /* Interrogation       */
  cm_reg_pwd_params_s_type     password;       /* pwd registration    */
} qmi_voice_cm_sups_cmd_parm_u_type;


/* Call forwarding information passed from ANDROID RIL, defined as per design doc */
typedef struct qmi_voice_cm_sups_callforwd_info_param_u
{
  int         status;            
  int         reason;           /* "reason" from 27.007 7.11              */
  uint32         service_class;    /* "class" for CCFC/CLCK from 27.007  */
  int         toa;              /* type of address 27.007 7.11           */
  char        *number;          /* "number" from  27.007 7.11           */
  int         no_reply_timer;   /*  CFU timer */
} qmi_voice_cm_sups_callforwd_info_param_u_type;

/* Data for a call forwarding registration */
typedef struct qmi_voice_cm_sups_reg_data_s
{
  char *number;       /* call forwarding number */
  int nr_timer;    /*no reply timer*/
} qmi_voice_cm_sups_reg_data_s_type;

/* Password information during password change or barring related sups */
typedef struct qmi_voice_cm_sups_change_passwd_s
{
  char *old_passwd;
  char *new_passwd;
  char *new_passwd_again;
} qmi_voice_cm_sups_change_passwd_s_type;

/* Information required to construct a sups string */
typedef struct qmi_voice_cm_sups_sups_params_s
{
  qmi_voice_cm_sups_mode_e_type             mode;            /* Indicates enable/disable etc */
  qmi_voice_cm_sups_operation_code_e_type   code;            /* Sups network service code */
  qmi_voice_cm_sups_class_e_type            service_class;   /* Service class as per 27.007 */
  union
  {
    qmi_voice_cm_sups_reg_data_s_type         reg;             /* Registration data */
    qmi_voice_cm_sups_change_passwd_s_type    passwd;          /* Password information */
  } cmd;
} qmi_voice_cm_sups_params_s_type;

/* Holds MMI Service code and corresponding Network service code */
typedef struct qmi_voice_cm_sups_sc_table_s
{
  char *mmi_sc;       /* MMI value of Service Code */
  byte net_sc;      /* Network value of Service Code */
} qmi_voice_cm_sups_sc_table_s_type;

/* Holds MMI BSG code and corresponding Service class */
typedef struct qmi_voice_cm_sups_bsg_table_s
{
  char *mmi_bsg;           /* MMI value of Basic Service Group */
  uint32 service_class;    /* Service Class as per 27.007 */
} qmi_voice_cm_sups_bsg_table_s_type;

/* Holds Sups type string corresponding to the Sups mode*/
typedef struct qmi_voice_cm_sups_mode_table_s
{
  char *sups_mode_str;                /* Sups operation type string */
  qmi_voice_cm_sups_mode_e_type sups_mode;  /* Indicates enable/disable etc */
} qmi_voice_cm_sups_mode_table_s_type;

/* Maximum possible digits for an integer (of 4 bytes) in decimal representation */
#define QMI_VOICE_CM_SUPS_MAX_INT_DIGITS        10

/* Maximum sups string length */
#define QMI_VOICE_CM_SUPS_MAX_SUPS_LENGTH       81

typedef struct
{
 int                                cause;
 qmi_voice_cm_if_failure_cause_e_type  reason;
}qmi_voice_cm_sups_cc_cause_to_failure_cause_table_s_type;

typedef struct
{
 int                                ss_error;
 qmi_voice_cm_if_failure_cause_e_type  reason;
}qmi_voice_cm_sups_ss_error_to_failure_cause_table_s_type;

/* All the above definitions are required for converting from UTF8 to UCS2 and
   Vice Versa */

/* These two extern declarations are needed here for using 
   them in qcril_cm.c file */
extern  qmi_voice_cm_sups_bs_mapping_s_type qmi_voice_cm_sups_bs_mapping_table[];
extern  uint32 qmi_voice_cm_sups_cw_allowed_classes;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

boolean qmi_voice_cm_sups_cmd_set_call_wait_is_valid( int status, uint32 service_class);
boolean qmi_voice_cm_sups_cmd_set_clip_is_valid( int status, uint32 service_class);
boolean qmi_voice_cm_sups_cmd_set_colp_is_valid( int status, uint32 service_class);
boolean qmi_voice_cm_sups_cmd_set_call_fwd_is_valid(qmi_voice_cm_if_info_item_s *const params_ptr, qmi_voice_cm_if_set_sups_service_s *in_data_ptr);
boolean qmi_voice_cm_sups_cmd_change_cb_pwd_is_valid( int facility, char *old_password, char *new_password);
boolean qmi_voice_cm_sups_query_facility_lock_is_valid( int facility,  uint32 service_class);

boolean qmi_voice_cm_sups_facility_value_is_valid(int facility, int status);

uint8 qmi_voice_cm_sups_get_ss_ref(void);

uint8  qmi_voice_cm_sups_get_cfw_ss_code(int reason, char **cf_reason_name);
int  qmi_voice_cm_sups_get_cfw_reason(int ss_code);
int  qmi_voice_cm_sups_get_cb_ss_code(int facility);
int  qmi_voice_cm_sups_get_facility_value(const char * facility, char *facility_name);

uint32 qmi_voice_cm_sups_get_service_class(bsg_code_type bsg_type, int bsg_code);

IxErrnoType qmi_voice_cm_sups_build_sups_string ( qmi_voice_cm_sups_params_s_type* ss_params, char* buf, int max_len);

void qmi_voice_cm_sups_set_bsg_info(uint32 service_class, basic_service_s_type *bsg_info);

IxErrnoType qmi_voice_cm_sups_set_time_info(cm_ip_sups_call_fwd_time_s_type source_time_info, cm_ip_sups_call_fwd_time_s_type *dest_time_info_ptr);

boolean qmi_voice_cm_sups_service_class_is_valid(uint32 srv_class);

boolean qmi_voice_cm_sups_set_facility_lock_is_valid( int facility, int status, uint32 service_class);

uint8 qmi_voice_cm_sups_map_service_to_cm_mode(uint8 service);

qmi_voice_cm_if_failure_cause_e_type qmi_voice_cm_sups_map_cc_cause_to_failure_cause( int  cc_cause );

qmi_voice_cm_if_failure_cause_e_type qmi_voice_cm_sups_map_ss_errors_to_failure_cause( int  ss_error );

qmi_voice_cm_sups_reason_code_e_type  qmi_voice_cm_sups_get_reason_from_ss_code(uint8 ss_code);

qmi_voice_cm_sups_service_type_e_type  qmi_voice_cm_sups_get_service_type_from_event(qmi_voice_cm_if_evt_e_type event);

qmi_voice_cm_sups_service_type_e_type  qmi_voice_cm_sups_get_service_type_from_operation
(
  uint8 ss_operation
);

qmi_voice_cm_if_evt_e_type  qmi_voice_cm_sups_get_event_from_service_type
(
  qmi_voice_cm_sups_service_type_e_type srv_type
);

qmi_voice_cm_if_evt_e_type  qmi_voice_cm_sups_get_network_resp_event
(
  qmi_voice_cm_if_evt_e_type ms_orig_evt
);

boolean  qmi_voice_cm_sups_is_network_resp_event
(
  qmi_voice_cm_if_evt_e_type evt
);

#endif /* _DS_QMI_VOICE_CM_SUPS_H */


