#ifndef TM_SUPL_H
#define TM_SUPL_H

/*===========================================================================
  
                            TM_UMTS_UP_SUPL.H  
                   
DESCRIPTION

  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  Confidential and Proprietary - Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_up/inc/tm_umts_up_supl.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  08/30/16   skm     Fix to drop EarlyRsp when no PRM or OTDOA RSTD's in Hybrid/OTDOA only CF's 
  06/30/16   skm     Early Response MSB not sent
  04/18/16   skm      Fix for crash seein > 24 AD test case SUPL message queuing
  02/12/16   skm     Add queuing support to send >24AD SUPL POS
  10/01/15    rk      Move the position protocol definition to common module accessible to all protocols.
  03/20/15    rk      In SI do not request GLO AD if the SLP told you in SUPL RESPONSE it wants to do GPS only.
  03/14/15    rh      Added support for MultiSIM NV
  03/10/15    lt     Proper handling of TLS version in SUPL messages for areaEventTriggered sessions.
  10/27/14   rk      Added LPP RSTD >24 measurement processing support
  02/27/13  ssh      OTA Delete to clear away existing NI Geofences
  05/14/13  ssh    Handle TCP connections in back to back SUPL sessions
  04/21/13    rk      Promote 'set_assist_pos_time_stamp' to common header also accessible to UMTS CP.
  12/11/12    rk      Added LPP UP A-Glonass MSB support.
  09/07/12   mj       Added cell_db_type for SGLTE
  12/13/11   gk       LPP related fixes
  01/03/11   gk       Eph throttling rework
  12/14/09    gk      send comm failure event
  01/06/09    lt      Cached position is now included in SUPL_POS_INIT.
  01/15/07    lt      Downlink header check error condition emums made bitmap.
  08/02/07    lt      Handling SVs non-existence case.
  05/25/07    lt      Modified supl_s_type structure to accommodate TM utility function changes.
  05/03/07    lt      Added this file header.
  04/25/07    lt      Initial check in.
===========================================================================*/


#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "tm_supl_asn1.h"
#include "tm_asn1_common.h"

#include "tm_umts_common_utils.h"
#include "l1lsmif.h"

#ifdef FEATURE_CGPS_LTE_CELLDB
  #include "lte_ml1_ext_api.h"
#endif

#include "gm_api.h"

#define SUPL_CF_TIMER_ID                    TM_UMTS_UP_TIMER_ID_RANGE
#define SUPL_L1_MRL_REQ_TIMER_ID   (SUPL_CF_TIMER_ID + 1)
#define SUPL_ALM_REQ_HD_TIMER_ID            (SUPL_CF_TIMER_ID + 2)
#define SUPL_EPH_REQ_HD_TIMER_ID            (SUPL_CF_TIMER_ID + 3)
#define SUPL_IONO_REQ_HD_TIMER_ID           (SUPL_CF_TIMER_ID + 4)
#define SUPL_UTC_REQ_HD_TIMER_ID            (SUPL_CF_TIMER_ID + 5)
#define SUPL_REFLOC_REQ_HD_TIMER_ID         (SUPL_CF_TIMER_ID + 6)
#define SUPL_RTI_VALID_DUR_TIMER_ID         (SUPL_CF_TIMER_ID + 7)
#define SUPL_MULTIPART_WAP_PUSH_WAIT_TIMER_ID (SUPL_CF_TIMER_ID + 8)
#define SUPL_UDP_BIND_RETRY_TIMER_ID        (SUPL_CF_TIMER_ID + 9)
#define SUPL_TCP_STAY_ON_ONLY_TIMER_ID      (SUPL_CF_TIMER_ID + 10)
#define SUPL_LPP_OTDOA_TIMER_ID             (SUPL_CF_TIMER_ID + 11)
#define SUPL_GLO_ALM_REQ_HD_TIMER_ID        (SUPL_CF_TIMER_ID + 12) 
#define SUPL_GLO_EPH_REQ_HD_TIMER_ID        (SUPL_CF_TIMER_ID + 13)
#define SUPL_WIFI_WAIT_TIMER_ID             (SUPL_CF_TIMER_ID + 14)
#define SUPL_GM_REQ_HD_TIMER_ID             (SUPL_CF_TIMER_ID + 15)
#define SUPL_LPP_OTDOA_MUL_MEAS_TIMER_ID    (SUPL_CF_TIMER_ID + 16)
#define SUPL_LPP_EARLY_RESPONSE_TIMER_ID    (SUPL_CF_TIMER_ID + 17)

#define SUPL_PORT_NUM_OFFICIAL_V1_0  7275
#define SUPL_TIMER_UT0_VALUE_MSEC    30000

#define SUPL_UDP_BIND_RETRY_VALUE_MSEC 15000

#define SUPL_RRLP_CURRENT_VERSION_MAJOR 8 /* current ASN1's major */
#define SUPL_RRLP_CURRENT_VERSION_TECH 6  /* current ASN1's technical */
#define SUPL_RRLP_CURRENT_VERSION_EDIT 1  /* current ASN1's editorial */

#define SUPL_MULTIPART_WAP_PUSH_WAIT_MSEC 30000
#define SUPL_UDP_BIND_MAX_RETRY_COUNT  10

/* newest REL-7 prior to the REL-8 being used */
#define SUPL_RRLP_BACK_VERSION_MAJOR 7 /* backward compatible to major */
#define SUPL_RRLP_BACK_VERSION_TECH 11  /* backward compatible to technical */
#define SUPL_RRLP_BACK_VERSION_EDIT 0  /* backward compatible to editorial */

/* constants for SUPL version information */
#define SUPL_VERSION_MAIN_1  ( 1 )   /* SUPL version Main being 1 */
#define SUPL_VERSION_MAIN_2  ( 2 )   /* SUPL version Main being 2 */

/************ Mapping ASN1 tool defined constants ****************************/

#define  C_SUPL_ASN1_CHOICE_IPV4ADDRESS  T_supl_IPAddress_ipv4Address
#define  C_SUPL_ASN1_CHOICE_IPV6ADDRESS  T_supl_IPAddress_ipv6Address

#define  C_SUPL_ASN1_CHOICE_SETID_MSISDN    T_supl_SETId_msisdn
//#define      supl_SETId_mdn_chosen         T_supl_SETId_mdn
//#define      supl_SETId_min_chosen         T_supl_SETId_min
#define  C_SUPL_ASN1_CHOICE_SETID_IMSI      T_supl_SETId_imsi
//#define      supl_nai_chosen               T_supl_SETId_nai
//#define      supl_SETId_iPAddress_chosen   T_supl_SETId_iPAddress

#define  C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS   T_supl_SLPAddress_iPAddress
#define  C_SUPL_ASN1_CHOICE_SLPADRESS_FQDN         T_supl_SLPAddress_fQDN

#define  C_ASN1_SUPL_FREQINFO_MODE_SPECIFIC_INFO_FDD  T_supl_FrequencyInfo_modeSpecificInfo_fdd
//#define      supl_FrequencyInfo_modeSpecificInfo_tdd_chosen  T_supl_FrequencyInfo_modeSpecificInfo_tdd

//#define      supl_hrpdCell_chosen   T_supl_Ver2_CellInfo_extension_hrpdCell
//#define      supl_umbCell_chosen    T_supl_Ver2_CellInfo_extension_umbCell
#define        C_SUPL_ASN1_CHOICE_LTE_CELL    T_supl_Ver2_CellInfo_extension_lteCell
#define        C_SUPL_ASN1_CHOICE_WLAN_CELL   T_supl_Ver2_CellInfo_extension_wlanAP
//#define      supl_wimaxBS_chosen    T_supl_Ver2_CellInfo_extension_wimaxBS

#define  C_SUPL_ASN1_CHOICE_GSM_CELL             T_supl_CellInfo_gsmCell
#define  C_SUPL_ASN1_CHOICE_WCDMA_CELL           T_supl_CellInfo_wcdmaCell
#define  C_SUPL_ASN1_CHOICE_CDMA_CELL            T_supl_CellInfo_cdmaCell
#define  C_SUPL_ASN1_CHOICE_SUPL2EXT_CELLINFO    T_supl_CellInfo_ver2_CellInfo_extension

#define  C_SUPL_ASN1_CHOICE_VELCITY_HORVEL              T_supl_Velocity_horvel
#define  C_SUPL_ASN1_CHOICE_VELCITY_HORANDVERVEL        T_supl_Velocity_horandvervel
#define  C_SUPL_ASN1_CHOICE_VELCITY_HORVELUNCERT        T_supl_Velocity_horveluncert
#define  C_SUPL_ASN1_CHOICE_VELCITY_HORANDVERUNCERT     T_supl_Velocity_horandveruncert

//#define  supl_shortKey_chosen  T_supl_SETAuthKey_shortKey
//#define  supl_longKey_chosen   T_supl_SETAuthKey_longKey

#define  C_SUPL_ASN1_CHOICE_TIA801_PAYLOAD               T_supl_PosPayLoad_tia801payload
#define  C_SUPL_ASN1_CHOICE_RRC_PAYLOAD                  T_supl_PosPayLoad_rrcPayload
#define  C_SUPL_ASN1_CHOICE_RRLP_PAYLOAD                 T_supl_PosPayLoad_rrlpPayload
#define  C_SUPL_ASN1_CHOICE_EXT_PAYLOAD                  T_supl_PosPayLoad_ver2_PosPayLoad_extension

//#define  supl_modeSpecificInfo_fdd_1_chosen    T_supl_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd
//#define  supl_modeSpecificInfo_tdd_1_chosen    T_supl_UTRAN_GPSReferenceTime_modeSpecificInfo_tdd

#define  C_SUPL_ASN1_CHOICE_FREQINFO_MODE_SPECIFIC_INFO_FDD   T_supl_FrequencyInfo_modeSpecificInfo_fdd
//#define  supl_CellMeasuredResults_modeSpecificInfo_tdd_chosen   T_supl_FrequencyInfo_modeSpecificInfo_tdd

//#define  supl_periodicParams_chosen    T_supl_TriggerParams_periodicParams
//#define  supl_areaEventParams_chosen   T_supl_TriggerParams_areaEventParams

//#define  supl_num_interval_chosen   T_supl_BatchRepConditions_num_interval
//#define  supl_num_minutes_chosen    T_supl_BatchRepConditions_num_minutes
//#define  supl_endofsession_chosen   T_supl_BatchRepConditions_endofsession
  
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT  T_supl_UlpMessage_msSUPLINIT
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLSTART  T_supl_UlpMessage_msSUPLSTART
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLRESPONSE   T_supl_UlpMessage_msSUPLRESPONSE
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLPOSINIT    T_supl_UlpMessage_msSUPLPOSINIT
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLPOS        T_supl_UlpMessage_msSUPLPOS
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLEND        T_supl_UlpMessage_msSUPLEND
//#define  supl_msSUPLAUTHREQ_chosen 7    T_supl_UlpMessage_msSUPLAUTHREQ
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_AUTHRESP       T_supl_UlpMessage_msSUPLAUTHRESP

#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGSTART  T_supl_UlpMessage_msSUPLTRIGGEREDSTART
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGRESPONSE T_supl_UlpMessage_msSUPLTRIGGEREDRESPONSE
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGGEREDSTOP T_supl_UlpMessage_msSUPLTRIGGEREDSTOP
//#define  supl_msSUPLNOTIFY_chosen 12              T_supl_UlpMessage_msSUPLNOTIFY
//#define  supl_msSUPLNOTIFYRESPONSE_chosen 13      T_supl_UlpMessage_msSUPLNOTIFYRESPONSE
//#define  supl_msSUPLSETINIT_chosen 14             T_supl_UlpMessage_msSUPLSETINIT
#define  C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLREPORT  T_supl_UlpMessage_msSUPLREPORT

//#define  supl_circularArea_chosen     T_supl_GeographicTargetArea_circularArea
//#define  supl_ellipticalArea_chosen   T_supl_GeographicTargetArea_ellipticalArea
//#define  supl_polygonArea_chosen      T_supl_GeographicTargetArea_polygonArea

//#define  supl_gSMAreaId_chosen       T_supl_AreaId_gSMAreaId
//#define  supl_wCDMAAreaId_chosen     T_supl_AreaId_wCDMAAreaId
//#define  supl_cDMAAreaId_chosen      T_supl_AreaId_cDMAAreaId
//#define  supl_hRPDAreaId_chosen      T_supl_AreaId_hRPDAreaId
//#define  supl_uMBAreaId_chosen       T_supl_AreaId_uMBAreaId
//#define  supl_lTEAreaId_chosen       T_supl_AreaId_lTEAreaId
//#define  supl_wLANAreaId_chosen      T_supl_AreaId_wLANAreaId
//#define  supl_wiMAXAreaId_chosen     T_supl_AreaId_wiMAXAreaId

//#define  supl_periodicParams_chosen    T_supl_TriggerParams_periodicParams
//#define  supl_areaEventParams_chosen   T_supl_TriggerParams_areaEventParams

//#define  supl_absoluteTime_chosen   T_supl_TimeStamp_absoluteTime
//#define  supl_relativeTime_chosen   T_supl_TimeStamp_relativeTime

//#define  supl_logicalName_chosen           T_supl_ThirdPartyID_logicalName
//#define  supl_ThirdPartyID_msisdn_chosen   T_supl_ThirdPartyID_msisdn
//#define  supl_emailaddr_chosen             T_supl_ThirdPartyID_emailaddr
//#define  supl_sip_uri_chosen               T_supl_ThirdPartyID_sip_uri
//#define  supl_ims_public_identity_chosen   T_supl_ThirdPartyID_ims_public_identity
//#define  supl_ThirdPartyID_min_chosen      T_supl_ThirdPartyID_min
//#define  supl_ThirdPartyID_mdn_chosen      T_supl_ThirdPartyID_mdn
//#define  supl_uri_chosen                   T_supl_ThirdPartyID_uri

#define  supl_noNotificationNoVerification   supl_NotificationType_noNotificationNoVerification
#define  supl_notificationOnly               supl_NotificationType_notificationOnly
#define  supl_notificationAndVerficationAllowedNA supl_NotificationType_notificationAndVerficationAllowedNA
#define  supl_notificationAndVerficationDeniedNA  supl_NotificationType_notificationAndVerficationDeniedNA
#define  supl_privacyOverride                     supl_NotificationType_privacyOverride
#define  supl_notificationTypeOutOfRangeHex5      supl_NotificationType_notificationTypeOutOfRangeHex5
#define  supl_notificationTypeOutOfRangeHex6 supl_NotificationType_notificationTypeOutOfRangeHex6
#define  supl_notificationTypeOutOfRangeHex7 supl_NotificationType_notificationTypeOutOfRangeHex7

#define  supl_ucs2                         supl_EncodingType_ucs2
#define  supl_gsmDefault                   supl_EncodingType_gsmDefault
#define  supl_utf8                         supl_EncodingType_utf8
#define  supl_encodingtypeOutOfRangeHex3   supl_EncodingType_encodingtypeOutOfRangeHex3

#define     supl_logicalName            supl_FormatIndicator_logicalName 
#define     supl_e_mailAddress          supl_FormatIndicator_e_mailAddress 
#define     supl_msisdn                 supl_FormatIndicator_msisdn 
#define     supl_url                    supl_FormatIndicator_url 
#define     supl_sipUrl                 supl_FormatIndicator_sipUrl 
#define     supl_min                    supl_FormatIndicator_min  
#define     supl_mdn                    supl_FormatIndicator_mdn 
#define     supl_iMSPublicidentity      supl_FormatIndicator_iMSPublicidentity 

#define     supl_north    supl_PositionEstimate_latitudeSign_north
#define     supl_south    supl_PositionEstimate_latitudeSign_south

#define    supl_depth     supl_AltitudeInfo_altitudeDirection_depth
#define    supl_height    supl_AltitudeInfo_altitudeDirection_height 

#define     supl_agpsSETassisted               supl_PosMethod_agpsSETassisted 
#define     supl_agpsSETbased                  supl_PosMethod_agpsSETbased 
#define     supl_agpsSETassistedpref           supl_PosMethod_agpsSETassistedpref 
#define     supl_agpsSETbasedpref              supl_PosMethod_agpsSETbasedpref 
#define     supl_autonomousGPS                 supl_PosMethod_autonomousGPS 
#define     supl_aFLT                          supl_PosMethod_aFLT 
#define     supl_eCID                          supl_PosMethod_eCID 
#define     supl_eOTD                          supl_PosMethod_eOTD 
#define     supl_oTDOA                         supl_PosMethod_oTDOA 
#define     supl_noPosition                    supl_PosMethod_noPosition 
#define     supl_posmethodOutOfRangeHexA       supl_PosMethod_posmethodOutOfRangeHexA 
#define     supl_posmethodOutOfRangeHexB       supl_PosMethod_posmethodOutOfRangeHexB 
#define     supl_posmethodOutOfRangeHexC       supl_PosMethod_posmethodOutOfRangeHexC 
#define     supl_posmethodOutOfRangeHexD       supl_PosMethod_posmethodOutOfRangeHexD 
#define     supl_posmethodoutOfRangeHexE       supl_PosMethod_posmethodoutOfRangeHexE 
#define     supl_posmethodoutOfRangeHexF       supl_PosMethod_posmethodoutOfRangeHexF 
#define     supl_ver2_historicalDataRetrieval  supl_PosMethod_ver2_historicalDataRetrieval 
#define     supl_ver2_agnssSETassisted         supl_PosMethod_ver2_agnssSETassisted 
#define     supl_ver2_agnssSETbased            supl_PosMethod_ver2_agnssSETbased 
#define     supl_ver2_agnssSETassistedpref     supl_PosMethod_ver2_agnssSETassistedpref 
#define     supl_ver2_agnssSETbasedpref        supl_PosMethod_ver2_agnssSETbasedpref 
#define     supl_ver2_autonomousGNSS           supl_PosMethod_ver2_autonomousGNSS 
#define     supl_ver2_sessioninfoquery         supl_PosMethod_ver2_sessioninfoquery
  
#define supl_proxy      supl_SLPMode_proxy
#define supl_nonProxy   supl_SLPMode_nonProxy
  
#define supl_unspecified    supl_StatusCode_unspecified 
#define supl_systemFailure    supl_StatusCode_systemFailure 
#define supl_unexpectedMessage    supl_StatusCode_unexpectedMessage 
#define supl_protocolError    supl_StatusCode_protocolError 
#define supl_dataMissing    supl_StatusCode_dataMissing 
#define supl_unexpectedDataValue    supl_StatusCode_unexpectedDataValue 
#define supl_posMethodFailure       supl_StatusCode_posMethodFailure 
#define supl_posMethodMismatch      supl_StatusCode_posMethodMismatch 
#define supl_posProtocolMismatch    supl_StatusCode_posProtocolMismatch 
#define supl_targetSETnotReachable    supl_StatusCode_targetSETnotReachable 
#define supl_versionNotSupported    supl_StatusCode_versionNotSupported 
#define supl_resourceShortage    supl_StatusCode_resourceShortage 
#define supl_invalidSessionId    supl_StatusCode_invalidSessionId 
#define supl_nonProxyModeNotSupported    supl_StatusCode_nonProxyModeNotSupported 
#define supl_proxyModeNotSupported       supl_StatusCode_proxyModeNotSupported 
#define supl_positioningNotPermitted     supl_StatusCode_positioningNotPermitted 
#define supl_authNetFailure              supl_StatusCode_authNetFailure 
#define supl_authSuplinitFailure         supl_StatusCode_authSuplinitFailure 
#define supl_consentDeniedByUser         supl_StatusCode_consentDeniedByUser 
#define supl_consentGrantedByUser        supl_StatusCode_consentGrantedByUser 
#define supl_statuscodeOutOfRangeHex14      supl_StatusCode_statuscodeOutOfRangeHex14 
#define supl_statuscodeOutOfRangeHex15      supl_StatusCode_statuscodeOutOfRangeHex15 
#define supl_statuscodeOutOfRangeHex16      supl_StatusCode_statuscodeOutOfRangeHex16 
#define supl_statuscodeOutOfRangeHex17      supl_StatusCode_statuscodeOutOfRangeHex17 
#define supl_statuscodeOutOfRangeHex18      supl_StatusCode_statuscodeOutOfRangeHex18 
#define supl_statuscodeOutOfRangeHex19      supl_StatusCode_statuscodeOutOfRangeHex19 
#define supl_statuscodeOutOfRangeHex1A      supl_StatusCode_statuscodeOutOfRangeHex1A 
#define supl_statuscodeOutOfRangeHex1B      supl_StatusCode_statuscodeOutOfRangeHex1B 
#define supl_statuscodeOutOfRangeHex1C      supl_StatusCode_statuscodeOutOfRangeHex1C 
#define supl_statuscodeOutOfRangeHex1D      supl_StatusCode_statuscodeOutOfRangeHex1D 
#define supl_statuscodeOutOfRangeHex1E      supl_StatusCode_statuscodeOutOfRangeHex1E 
#define supl_statuscodeOutOfRangeHex1F      supl_StatusCode_statuscodeOutOfRangeHex1F 
#define supl_ver2_incompatibleProtectionLevel   supl_StatusCode_ver2_incompatibleProtectionLevel 
#define supl_ver2_serviceNotSupported           supl_StatusCode_ver2_serviceNotSupported 
#define supl_ver2_insufficientInterval          supl_StatusCode_ver2_insufficientInterval 
#define supl_ver2_noSUPLCoverage                supl_StatusCode_ver2_noSUPLCoverage 
#define supl_ver2_sessionStopped               supl_StatusCode_ver2_sessionStopped 

#define C_MCC_MNC_UPLIMIT 999 /* refMCC   INTEGER(0..999), -- Mobile Country Code */
                              /* refMNC   INTEGER(0..999), -- Mobile Network Code */ 
#define C_WCDMA_CELL_ID_UPLIMIT 268435455L /* refUC INTEGER(0..268435455), -- Cell identity*/
#define C_WCDMA_CELL_FREQ_UPLIMIT 16383 /* UARFCN ::= INTEGER(0..16383) */
#define C_WCDMA_CELL_PSC_UPLIMIT 511 /* INTEGER(0..511) */
#define C_LTE_CELL_GLOBAL_CELLID_UPLIMIT 268435455L /* CellIdentity ::= BIT STRING (SIZE (28)) */
#define C_LTE_CELL_PHY_CELLID_UPLIMIT 503   /* PhysCellId ::=  INTEGER (0..503) */

#define C_RRLP_GNSS_TO_ID_GPS 0 /* defined by 44031 GNSS_TO_ID: GPS: 0; Galileo: 1; QZSS: 2; GLONASS: 3 */
#define C_GANSS_SYSTEM_TIME_MODEL_REQUESTED_GPS 0x80 /* GPS as requested time model system: bit bitstring - Bit0 being GPS  */

#define MAX_SUPL_MESSAGE_SIZE  2000

/* mapping tool specific ASN1 constants to locally maintained enums */
typedef enum
{
   C_AREA_EVENT_TYPE_ENTER   =    supl_AreaEventType_enteringArea, /*  = 0, */
   C_AREA_EVENT_TYPE_INSIDE   =   supl_AreaEventType_insideArea,   /*  = 1, */
   C_AREA_EVENT_TYPE_OUTSIDE  =   supl_AreaEventType_outsideArea,  /*  = 2, */
   C_AREA_EVENT_TYPE_LEAVING  =   supl_AreaEventType_leavingArea   /*  = 3  */
} supl_area_event_type; 

  
typedef struct 
{  
  supl_FrequencyInfoFDD         z_frequency_info_fdd;
  supl_MeasuredResults          z_measured_results;
  #ifdef FEATURE_USER_PLANE_MRL
  supl_CellMeasuredResults      z_cell_measured_results[WL1_LSM_CELLS_MEAS_INFO_MAX];
  supl_CellMeasuredResults_modeSpecificInfo_fdd   z_cell_measure_results_fdd[WL1_LSM_CELLS_MEAS_INFO_MAX];
  #endif
} supl_wcdma_cell_info_struct_type; /* for allocation use with OSYS */

typedef struct
{
  supl_LteCellInformation       z_supl_LteCellInformation;
  uint8                         z_lte_mcc[3];
  uint8                         z_lte_mnc[3];
  #ifdef FEATURE_CGPS_LTE_CELLDB 
  supl_MeasResultEUTRA z_neighbor_EUTRA_link[LTE_ML1_GPS_MRL_CELLS_MAX-1];
  #endif
  
} supl_lte_cell_info_struct_type; 
  

typedef struct
{
  union 
  {
    supl_GsmCellInformation gsmCell;  /* to choose, set choice to
                                       * supl_gsmCell_chosen */
    supl_WcdmaCellInformation wcdmaCell;  /* to choose, set choice to
                                           * supl_wcdmaCell_chosen */
                                          /*WCDMA Cell Information/TD-SCDMA Cell Information */
    supl_CdmaCellInformation cdmaCell;  /* to choose, set choice to
                                         * supl_cdmaCell_chosen */
    supl_Ver2_CellInfo_extension ver2_CellInfo_extension;  /* extension #1;
                               * to choose, set choice to
                               * supl_ver2_CellInfo_extension_chosen */
  } u1;
  
  union 
  {
    supl_wcdma_cell_info_struct_type   z_supl_wcdma_cell_info;
    supl_lte_cell_info_struct_type     z_supl_lte_cell_info;
    supl_WlanAPInformation             z_supl_wlan_ap_info;
  } u2;
  
} supl_cell_info_struct_type;


typedef enum
{
  C_SUPL_SESSION_NONE,
  C_SUPL_SESSION_NI, /* network-initiated */
  C_SUPL_SESSION_UI, /* UE-initiated immediate */
  C_SUPL_SESSION_KEEP_WARM /* Keep Warm initiated session */
} supl_callflow_kind_enum_type;


typedef enum
{
  C_SUPL_CALLFLOW_DISRUPT_UT1,              /* UT1 expiration */
  C_SUPL_CALLFLOW_DISRUPT_UT2,              /* UT2 expiration */
  C_SUPL_CALLFLOW_DISRUPT_UT3_RRLP,         /* UT3 - expecting more RRLP - expiration */
  C_SUPL_CALLFLOW_DISRUPT_UT3,              /* UT3 - expecting PDRTP - expiration */
  C_SUPL_CALLFLOW_DISRUPT_PD_END,           /* PDAPI end session */
  C_SUPL_CALLFLOW_DISRUPT_LCS_MANAGER_ERR,  /* LCS Manager error */
  C_SUPL_CALLFLOW_DISRUPT_SUPLRESP_PS_ERR,  /* SUPLRESP indicating position server error */
  C_SUPL_CALLFLOW_DISRUPT_SUPLEND,          /* SUPLEND causing call flow disrupt */
  C_SUPL_CALLFLOW_DISRUPT_RRLP_ERR,         /* RRLP message contains mistake */
  C_SUPL_CALLFLOW_DISRUPT_SUPLINIT_PREEMPT, /* current session pre-empted by a new LCSINIT */
  C_SUPL_CALLFLOW_DISRUPT_SUPLRESP_VER,     /* SUPLRESP indicates wrong version */
  C_SUPL_CALLFLOW_DISRUPT_UNDEF
} supl_callflow_disrupt_reason_enum_type;

typedef enum
{
  C_SUPL_AGPS_MODE_UNDEF,
  C_SUPL_AGPS_MODE_UE_BASED,
  C_SUPL_AGPS_MODE_UE_ASSISTED,
  C_SUPL_AGPS_MODE_AUTO,
  C_SUPL_AGPS_NO_POSITION,
  C_SUPL_AGPS_MODE_CELL_ID,
  C_SUPL_AFLT_MODE, /*Extending the defination to include AFLT also*/
  C_SUPL_OTDOA_MODE /* OTDOA only call flow over SUPL */
} supl_session_agps_mode_enum_type;

typedef enum
{
  C_SLP_MESS_HEADER_STATUS_UNKNOWN, /* initial value */
  C_SLP_MESS_HEADER_OK_NEW,         /* this message provides new header for the new open session */
  C_SLP_MESS_HEADER_OK_CONSISTENT,  /* this message's header is consistent with that of the open session */

  /* following error conditions may coexit, so use bitmap */
  C_SLP_MESS_SLP_SESS_ID_MISSING = 0x04,   /* SlpSessionID missing */
  
  C_SLP_MESS_SLP_SESS_ID_PROBLEM = 0x08, 
  C_SLP_MESS_SET_SESS_ID_MISSING = 0x10, 
  C_SLP_MESS_SET_SESS_ID_PROBLEM = 0x20,
  C_SLP_MESS_SUPLINIT_SET_SESS_ID_PROBLEM = 0x40,
  C_SLP_MESS_VERSION_UNSUPPORTED = 0x80,    /* wrong SUPL version number found in header */
  C_SLP_MESS_SLP_SESS_ID_PROBLEM_SUPLINIT_EXISTING_AREA_EVENT = 0x100, /* SLP sessionID in SUPL_INIT identical to that in a existing areaEvent */
  C_SLP_MESS_VERSION_CHECK_FAILED = 0x200 /* indicate the failure of SUPL version check */
}slp_mess_header_check_result_e_type;


typedef enum
{
  C_SUPL_KEEP_WARM_NONE,         /* Keep Warm processing has not started yet */
  C_SUPL_KEEP_WARM_REQUESTED,    /* Data requested */
  C_SUPL_KEEP_WARM_COMM_ERROR,   /* Communication link error */
  C_SUPL_KEEP_WARM_SERVER_ERROR, /* Server could not be contacted */
  C_SUPL_KEEP_WARM_PREEMPTED,    /* Keep Warm processing was pre-empted. */
  C_SUPL_KEEP_WARM_COMPLETED     /* Processing has started and ready for next request */
} supl_keep_warm_req_status_enum_type;

typedef enum
{
  C_SUPL_INIT_TRIG_TYPE_NULL,
  C_SUPL_INIT_TRIG_TYPE_PERIODIC,
  C_SUPL_INIT_TRIG_TYPE_AREA_EVENT,
  C_SUPL_INIT_TRIG_TYPE_WRONG_DEF
} supl_init_trigger_type_enum_type;


#define C_SUPL_FQDN_VISIBLE_STRING_BYTE_LENGTH 255
typedef struct
{
  uint8 u_length;
  char fqdn[C_SUPL_FQDN_VISIBLE_STRING_BYTE_LENGTH];
} supl_fqdn;

#define C_SUPL_IPADDRESS_IPV4_BYTE_LENGTH  4
#define C_SUPL_IPADDRESS_IPV6_BYTE_LENGTH  16
typedef struct 
{
  uint8 choice;  /* supl_ipv4Address_chosen 1
                    supl_ipv6Address_chosen 2 */

  union
  {
    supl_IPAddress_ipv4Address Ip_V4;
    supl_IPAddress_ipv6Address Ip_V6;
  } u;
} supl_ip_address;

#define C_SUPL_SETID_MSISDN_BYTE_LENGTH     8
#define C_SETID_UNKNOWN 0    /* not retrieved yet */

#define C_SUPL2_GANSS_ID_GALILEO 0
#define C_SUPL2_GANSS_ID_SBAS    1
#define C_SUPL2_GANSS_ID_MGPS    2 /* modern GPS */
#define C_SUPL2_GANSS_ID_QZSS    3
#define C_SUPL2_GANSS_ID_GLONASS 4 /* only one supported */
#define OTHER_SETID_SIZE (sizeof(supl_SETId) - 4)

typedef struct 
{
  uint8 valid;
  uint8 raw_contents[OTHER_SETID_SIZE];
} supl_other_setID;


typedef struct
{
  uint32 set_id_choice; 

  union
  {
    /* use of MSISDN, though others are allowed by the SUPL spec. */
    supl_SETId_msisdn msisdn;  
  
    supl_SETId_imsi imsi; /* use IMSI if MSISDN not available */
    
    supl_other_setID other_setID;
  }u;
   
} supl_set_id;

typedef struct 
{
  uint8 choice; /*  supl_SLPAddress_iPAddress_chosen 1
                    supl_fQDN_chosen 2 */
  union 
  {
    supl_ip_address z_Ip_Address;
    supl_fqdn z_fqdn;
  } u;
} supl_slp_address;

#define C_SUPL_SLP_SESSION_ID_BYTE_LENGTH  4
typedef struct 
{
  boolean presence;
  uint8 session_Id[C_SUPL_SLP_SESSION_ID_BYTE_LENGTH];   
  supl_slp_address zSlpAddress;
} supl_slp_session_id;

typedef struct 
{
  uint16 w_Session_Id;  
  supl_set_id z_Set_Id;
} supl_set_session_id;

typedef struct 
{
  supl_set_session_id z_Set_Sess_Id;
  supl_slp_session_id z_Slp_Sess_Id;
} supl_session_id;


typedef struct
{
  uint8  u_gps_session_timeout_value_secs;
  uint32 q_hor_acc_meters;
} supl_session_qos;


#define C_SUPLINIT_HASH_LEN_BYTE 8

/* call flow states: while C_SUPL_CF_STATE_UT0_SUPLEND may coexit with any other states, 
   all other states are mutually exclusive  */
typedef enum  
{
  C_SUPL_CF_STATE_NULL,
  C_SUPL_CF_STATE_UT0_SUPLSTART = 0x0001,    /* wait for TCP link up, for sending SUPL_START */
  C_SUPL_CF_STATE_UT0_SUPLPOSINIT = 0x0002,  /* wait for TCP link up for sending SUPL_POSINIT */
  C_SUPL_CF_STATE_UT0_SUPLEND = 0x0004,      /* wait for TCP link up for sending SUPL_POSEND */
  C_SUPL_CF_STATE_UT1 = 0x0008,
  C_SUPL_CF_STATE_UT2 = 0x0010,
  C_SUPL_CF_STATE_UT3 = 0x0020,
  C_SUPL_CF_STATE_UT3_RRLP = 0x0040,               /* use the value of UT3, for waiting for more RRLP */
  C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER = 0x0080,    /* RRLP measurePosReq RespTime. LPP utilize this state to send 
                                                      LPP Prov Loc Info with networked specified reponse time in 'LPP Req Loc Info' */
  C_SUPL_CF_STATE_CELL_INFO_REQ = 0x0100,           /* wait for RRC to provide cell info. and move to start 
                                                       SLP contact for non-areaEvent upon receiving cell info. or timeout */
  C_SUPL_CF_STATE_UT3_ACK =  0x0200,                 /* use the value of UT3, for waiting for SUPL_END after ACK */
  C_SUPL_CF_STATE_UT0_SUPLTRIGGERSTART = 0x0400, /* wait for TCP link up for sending SUPL_TRIGGER_START */
  C_SUPL_CF_STATE_UT1_SUPLTRIGGERRESPONSE = 0x0800, /* wait to receive SUPL_TRIGGER_RESPONSE */
  C_SUPL_CF_STATE_UT3_LAST_SUPLREPORT = 0x1000, /* after max. num. report is reached, waiting for SLP SUPL_END */
  C_SUPL_CF_STATE_UT0_SUPLREPORT_QUERY = 0x2000, /* wait for TCP link up for sending SUPL_REPORT responding to query */
  C_SUPL_CF_STATE_UT10_INFO_QUERY_END = 0x4000, /* UT10 waiting for SUPL_END after sessionInfoQuery */ 
  C_SUPL_CF_STATE_UT0_SUPLTRIGSTOP = 0x8000, /* wait for TCP link up for sending SUPL_TRIGGED_STOP */
  C_SUPL_CF_STATE_UT7_SUPL_END = 0x10000, /* keep tcp on waiting for SUPL_END after sending SUPL_TRIG_STOP */
  C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT = 0x20000, /* expect SUPL_REPORT from SLP */
  C_SUPL_CF_STATE_OTDOA_MEAS_REQ = 0x40000,
  C_SUPL_CF_STATE_CELL_INFO_REQ_AREA_EVENT = 0x80000, /* wait for RRC to provide cell info. and move to start 
                                                        SLP contact for areaEvent upon receiving cell info. or timeout */
  C_SUPL_CF_STATE_UT0_SUPLREPORT_GF_BREACH = 0x100000, /* wait for tcp connection to send SUPL_REPORT for GF breach */

  C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP =  0x200000, /* wait for WiFi state with timer started when there is no TCP connection  */
  C_SUPL_CF_STATE_WIFI_WAIT_SUPLSTART =  0x400000,  /* continue waiting for WiFi state without timer; send SUPL_START once Wifi available  */
  C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLSTART = 0x800000,  /* wait for WiFi state with timer started when TCP is on, send SUPLSTART once Wifi available */
  C_SUPL_CF_STATE_WIFI_WAIT_SUPLPOSINIT =    0x100000,  /* continue waiting for WiFi state without timer; send SUPL_POSINIT once Wifi available  */
  C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLPOSINIT =  0x2000000,  /* wait for WiFi state with timer started when TCP is on, send SUPLPOSINIT once Wifi available */
  C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV =  0x4000000,  /* First OTDOA RSTD measurement received, but more is expected for completion */
  C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ = 0x8000000, /* OTDOA Only early Meas requested*/
} supl_cf_state_enum_type;

typedef struct
{
  boolean   u_final;
  uint32    q_fix_time_gps_msec; /* gps time: mses in week */
  uint32    q_fix_time_utc_msec; /* time adjusted for leap seconds */

  uint32    q_glonass_tod_modulo_1hour_ms; /* GLONASS TimeOfDay:
                              its valid range GANSSTODm ::= INTEGER (0 .. 3599999);
                            */

  uint16    w_GpsWeek; /* fix week number in GPS time */

  uint32    q_pos_sources; /* indicate GPS/GLONASS etc. */

  gad_ellip_alt_unc_ellip_type   z_pos; /* used for RRLP and LPP. 
                                For LPP the lat/long/alt are not used from this structure.
                                all remaining data is used for LPP also*/
  boolean  vel_valid;
  float    f_VelEnu[3]; /* East, North, Up velocity (m/s). */
  float    f_Vunc3dMps;
  float    f_VelVertUnc;
  float     f_ClockBias;           /* Receiver's clock bias (m).    */
  float     f_ClockBiasUncMs;      /* Receiver's clock uncertainty (m). */  
  uint16 w_Alt; /* GAD shape values stored for LPP. 
                all remaining values are used from gad_ellip_alt_unc_ellip_type */ 
  uint32 q_Lat; /* GAD shape values stored for LPP. 
                all remaining values are used from gad_ellip_alt_unc_ellip_type */
  int32 l_Long; /* GAD shap values stored for LPP. 
                all remaining values are used from dad_ellip_alt_unc_ellip_type */ 
  // Glonass
  uint32 q_GloTimeMs;     /* Glonass time in day (milliseconds). 0..3599999. Valid only when q_pos_sources is Glonass only (C_POS_SOURCE_GLO) */  
} pending_pos_to_network_s_type;

#define TM_SUPL_LPP_PRM_VALID 0x00000001                        /* Measurement report from ME is valid */
#define TM_SUPL_LPP_AD_REQUESTED 0x00000004
#define TM_SUPL_LPP_AD_RECVD 0x00000008
#define TM_SUPL_LPP_REQ_LOC_INFO_RCVD 0x00000010
#define TM_SUPL_LPP_OTDOA_DELIVERED 0x00000020
#define TM_SUPL_LPP_VEL_REQUESTED 0x00000040
#define TM_SUPL_LPP_MAY_RET_ADD_INFO 0x00000080
#define TM_SUPL_LPP_MAY_REQ_ADD_AD 0x00000100
#define TM_SUPL_LPP_REQ_LOC_INFO_POS_EST_RCVD 0x00000200
#define TM_SUPL_LPP_REQ_LOC_INFO_MEAS_RCVD 0x00000400             /* LS request LocationMeasurement from UE */
#define TM_SUPL_LPP_REQ_LOC_INFO_AGNSS_RCVD 0x00000800
#define TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD 0x00001000            /* LS request OTDOA measurements from UE */
#define TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA 0x00002000

typedef struct
{
  q_link_type                              link;
  long                                     length;
  char                                     msg[MAX_SUPL_MESSAGE_SIZE];
} supl_msg_queue_element;

/* LPP Related Data in SUPL */
typedef struct
{
  uint32               q_lpp_flags;
  uint32               q_lpp_earlyResponseflags;
  sm_loc_lte_otdoa_meas_info_type *p_otdoa_data; /* Point to OTDOA RSTD measurements. */
  boolean              b_status_check_needed;
  uint8                u_lpp_error_type;
  uint8                u_lpp_error_code;
  boolean              b_tm_session_done;
  uint8                u_req_ad_count;   /* Count number of RequestAssistanceData sent to LS in a session */
  boolean              b_earlyResponse;
} tm_supl_lpp_data_type; 

typedef struct
{
  boolean u_valid;
  pending_pos_to_network_s_type  z_pos_info;
}supl_cached_pos_for_net_s_type;  

typedef struct 
{
  boolean                             b_valid;
  boolean                             b_emergency_ni;
  boolean                             b_eslp_present;
  pdsm_supl_slp_address               z_eslp_address;
} supl_emergency_struct_type;

typedef struct
{
  pdsm_client_id_type                 q_pdapi_client_id;

  tm_sess_handle_type                 tm_active_session_handle; 

  /* TRUE: a network session is in progress */
  boolean   u_network_session_on;

  /* TRUE if a position has been given to pdapi */     
  boolean   u_pdapi_fix_reported;   
    
  boolean   u_supl_posMethod_ecid_valid;
    
  /* used when a position is reported to pdapi prior to completion of the 
     network call flow */
  supl_cached_pos_for_net_s_type      z_pending_new_pos_to_network; 

  /* UE-Based, UE-Assisted, ... */
  supl_session_agps_mode_enum_type    u_agps_mode;     

  supl_callflow_kind_enum_type        u_callflow_kind;  /* NI, UI */
  supl_session_id                     z_session_id;
  supl_cf_state_enum_type             e_cf_state;

  GpsAssisData_Gsm0931                z_GpsAssistData; /* GPS assist. data wishlist */
  
  GpsAssisData_ganss_Gsm0931                z_GloAssistData; /* Glonass assist. data wishlist */
  
  /* buffer to combine Glonass eph. data with the freq. number in aux. info assistance */ 
  prot_glo_eph_buffer_s_type          z_prot_glo_eph_buffer[N_GLO_SV]; 

  /* buffer to combine Glonass Acq. Assistance data with the freq. number in aux. info assistance */ 
  prot_glo_acqast_buffer_s_type       z_prot_glo_acqasst_buffer;

  /* always request reference location even if LM doesn't need it */
  boolean                             u_force_request_ref_location; 

  boolean                             u_hash_valid;
  uint8                               supl_init_hash[C_SUPLINIT_HASH_LEN_BYTE];

  /* boolean                          defer_pdapi_done;  */

  uint32                              q_wishlist_bitmask;
  
  supl_session_qos                    z_session_qos;
  
  tm_network_assist_s_type            z_network_assist;

  gnss_time_info                      z_set_assist_pos_time_stamp; 

  boolean                             tm_core_session_started;

  /* bit map of alm. delivered; SV1 on Bit 0 */
  uint32                              q_alm_delivered_sv_list;
  /* bit map of eph. delivered; SV1 on bit 0 */ 
  uint32                              q_eph_delivered_sv_list; 

  /* LM has been given the true qos_timer */
  boolean                             qos_timer_started; 

  /* z_supl_pd_info_buffer contains valid info. */
  boolean                             pd_info_buffer_valid; 
  
  uint16                              w_bearing;       /* velocity heading */
  uint16                              w_velocity_hor;  /* horizontal velocity */
  uint8                               u_velocity_ver;  /* vertical velocity */
  uint8                               u_verdirect;
  uint8                               u_speed_unc;
  uint8                               u_verspeed_unc;
  
  uint32     q_supl_version; /* from LSB: major, minor, servind, reserved */

  /*** if RTI info. is provided in this session, TRUE */
  boolean                             u_rti_delivered;

    /*Pos protocol in use. For LTE, RRLP will be selected and for 1x,HRPD,eHRPD, IS801 will be selected*/
  tm_positioning_protocols            u_position_protocol;

    /*Caching the session request param from TM-Core as this will be passed while starting IS801 state machine*/
  prtl_start_sess_req_param_s_type    tm_sess_req_param;

  /*Caching ni pos protocol, required while doing handover*/
  pdsm_lcs_pos_method_e_type          ni_pos_method;
  
  boolean                             u_current_is_for_area_event;

  /*If SUPL need to wait for write ack*/
  boolean                             wait_for_write_ack;
  
  /* optional ApplicationID - only applicable to SET_initiated */
  pdsm_pd_supl_application_id_s_type  z_supl_app_id;

  /* cache the original wish list */ 
  sm_InternalAssistDataStatusStructType z_OrigWishList;

  /*If ok to send SUPL_END at fail*/
  boolean                             u_send_supl_end_at_fail;

  /*If waiting for SUPL close*/
  boolean                             supl_close_waiting;

  tm_supl_lpp_data_type               z_supl_lpp_data;

  /* Whether to expect GLONASS Measurement report from ME */
  uint8                               u_glo_meas_status;  /* Uses SM_GNSS_GLO_AA_INJECTED & SM_GNSS_LS_REQ_GLO_MEAS macros */

  /* Hold multiple GNSS measurement reports - GPS/Glonass */
  SmGnssMeasReportStructType          z_gnss_meas_report;
  supl_emergency_struct_type          z_emergency_data;
  /*Is Session TCP connection Pending*/
  boolean                             u_tcp_connection_attempt_pending;

  boolean                             u_wifi_scan_requested;

  /* Indicate if H-SLP supports GLONASS in SI SUPL_RESPONSE or NI SUPL_INIT */
  boolean                             u_slp_support_glonass;

  q_type                              supl_msg_q;
  boolean                             supl_msg_q_status; /*Active or inactive*/

}supl_status_s_type;

#define C_AREA_EVENT_REPEATED_REPORTING_VALID 0x1
#define C_AREA_EVENT_GEO_TARGET_AREA_VALID    0x2
#define C_AREA_EVENT_MIN_REPORT_INTERVAL_FLOOR  60
#define C_AREA_EVENT_DEFAULT_STOP_TIME      864000 

#define C_SUPL_DEFAULT_TCP_ON_ONLY_TIME_SEC  16

/* information used to formulate a pending SUPL_REPORT */
typedef struct
{
  boolean u_valid;
  sm_gm_client_breach_notify_type    z_gm_client_breach_notify;
}pending_supl_report_info_s_type;


#define C_AREA_EVENT_PENDING_SLP_CONTACT_TRIIGER_START 0x1

typedef struct
{
  boolean u_session_active; /* if TRUE, an active session is maintained
                               with this structure */
  
  /* UE-Based, UE-Assisted, ... */
  supl_session_agps_mode_enum_type    u_agps_mode;     

  supl_session_id           z_session_id;
  supl_cf_state_enum_type   e_cf_state;


  uint8                supl_init_hash[C_SUPLINIT_HASH_LEN_BYTE];
                      
  uint32               q_supl_version; /* from LSB: maj, min, servind, reserved */

    /*Pos protocol in use. For LTE, RRLP will be selected and for 1x,HRPD,eHRPD, IS801 will be selected*/
  uint8                u_position_protocol;   
  
  /* q_gm_transaction_id: only used for adding deleting GF before confirmation */
  boolean              u_gm_trasaction_id_valid;
  uint32               q_gm_transaction_id;   
  
  boolean              u_geofence_id_valid;
  uint32               q_geofence_id;
  
  /* defined as (0..604800) but limited to (60 - 604800); 0 indicating 
     not present which means only one report expected. 
     Kept here for TCP connection stay-on purpose after
     sending SUPL_REPORT */
  uint32               q_min_interval_time;  
  
  pending_supl_report_info_s_type   z_pending_supl_report_info;
  
  boolean              u_delete_pending;
  
  /* 0x1 (C_AREA_EVENT_PENDING_SLP_CONTACT_TRIIGER_START): 
          pending SLP contact for sending SUPL_TRIGGER_START; 
     other bits: reserved */
  uint32               q_slp_contact_pending;
  

}supl_area_event_status_s_type;  /* this needs to be saved in EFS */


typedef struct
{
  supl_session_id    z_session_id;
  uint16             w_SetSessionID;
  uint32             q_supl_version; /* from LSB: maj, min, servind, reserved */
}supl_gm_blob;



#define C_MAX_NUM_PENDING_SUPL_END  5  /* up to 5 supl_end in que */
typedef struct
{
  boolean                   u_validity;
  supl_set_session_id       set_session_id;
  supl_slp_session_id       slp_session_id;
  supl_StatusCode           e_supl_status_code;
  boolean                   u_is_hash_present;
  uint8                     hash[C_SUPLINIT_HASH_LEN_BYTE];
}pending_supl_end;  /* buffered for direct response to SLP */

typedef struct
{
  boolean u_valid;
  uint32  q_rti_flagged_bitmap;
} rti_info_s_type;

#ifdef SUPL_MAIN_VARIABLES_DEFINED
#define extern
#endif

/******* SUPL internal data *********************************************/
extern supl_status_s_type        z_supl_session_status;
extern supl_emergency_struct_type  z_supl_emergency_status;

extern supl_set_id               z_Set_Id;

#define C_BYTES_FOR_HASH  516 /* 512 plus a bit cusion */

#define C_MAX_NUM_AREA_ID_LIST  32
#define C_MAX_NUM_AREA_ID_PER_LIST ( GEOFENCE_MAX_AREA_ID_LIST_MAX / C_MAX_NUM_AREA_ID_LIST )
#define C_MAX_NUM_GEOAREA_PER_SUPL_INIT 1

#define C_MAX_GEOFENCE_NUM 16  /* num. the phone will operate with */
extern supl_area_event_status_s_type  z_supl_area_event_session_status[C_MAX_GEOFENCE_NUM];

/* for pdcomm rx buffer */
extern uint8 *p_pdcomm_rx_buff;
extern uint16 w_pdcomm_rx_buff_size;
extern uint8 u_buffer_counter; //temp 

/* will save SUPL INIT source (1X/GSM SMS, WAP-PUSH or UDP) */
extern pdsm_lcs_supl_init_src_e_type z_supl_init_source; 

#define C_MAX_NUM_CELL_HIST_SEC  655 /* multiple location ID relative time stamp up to 655.35 seconds */
#define C_MAX_MLID_NUM 64
typedef struct
{
  uint32 q_timestamp;
  boolean is_the_current_serving_cell;
  cgps_CellInfoCachedStruct loc_id;
} mlid_buffer_strcut_type;


#ifndef TM_SUPL_COMM_USE

extern pending_supl_end          z_pending_supl_end[C_MAX_NUM_PENDING_SUPL_END];
extern pdsm_pd_info_s_type       z_supl_pd_info_buffer;
extern supl_cached_pos_for_net_s_type    z_supl_cached_pos_for_net;
extern os_TimerMsgType *supl_cf_timer;
extern os_TimerMsgType *l1_mrl_req_timer;
extern os_TimerMsgType *supl_alm_req_holdoff_timer;
extern os_TimerMsgType *supl_eph_req_holdoff_timer;
extern os_TimerMsgType *supl_iono_req_holdoff_timer;
extern os_TimerMsgType *supl_utc_req_holdoff_timer;
extern os_TimerMsgType *supl_refloc_req_holdoff_timer;
extern os_TimerMsgType *supl_tcp_stay_on_only_timer;
extern os_TimerMsgType *supl_lpp_otdoa_req_timer;
extern os_TimerMsgType *supl_wifi_wait_timer;
extern os_TimerMsgType *supl_lpp_otdoa_mul_meas_timer; 
extern os_TimerMsgType *supl_lpp_early_response_timer;

/* number of handover since last valid cell info: 0 means no handover */
extern uint32 q_handovers_since_last_cell_id; 

extern uint8  supl_gm_handle;
extern uint32 gm_transaction_id;   /*GM Module Transaction ID*/
extern uint16 w_SetSessionID;  
extern uint8 supl_lpp_handle;

extern os_TimerMsgType*          supl_glo_alm_req_holdoff_timer;
extern os_TimerMsgType*          supl_glo_eph_req_holdoff_timer;

extern os_TimerMsgType*          supl_gf_holdoff_timer;

extern uint8 u_supl_global_num_mlid; /* number of valid entries in z_mlid_buffer */
extern mlid_buffer_strcut_type z_mlid_buffer[C_MAX_MLID_NUM + 1];  /* plus one for storage of the current serving */


#ifdef extern
#undef extern
#endif

#else

#define extern
extern os_TimerMsgType*          supl_udp_bind_retry_timer;
extern os_TimerMsgType*          supl_multipart_wap_push_wait_timer;
#undef extern

#endif /* ifndef supl_comm use */



boolean tm_umts_up_supl_pdapi_mapping (prtl_start_sess_req_param_s_type  *start_sess_req_param);
void tm_umts_up_supl_start_slp_contact(void);
void tm_umts_up_supl_start_slp_contact_area_event(uint8 u_session_num);
void tm_umts_up_supl_inform_core_nt_assist_end (void);
boolean tm_umts_up_supl_message_decode ( uint8 *p_data, uint16 w_data_len );
boolean tm_umts_up_supl_suplpos_rrlp_mpr_err_tx (rrlp_LocErrorReason e_error_reason);
void tm_umts_up_supl_handle_lr_data(sm_GnssFixRptStructType *lr_data_ptr, boolean u_final);
void tm_umts_up_supl_handle_prm_data ( sm_GpsMeasRptStructType *p_prm_data );
void tm_umts_up_supl_session_init(boolean session_clear_for_prempt);
void tm_umts_up_supl_callflow_disrupt_handler(void);
void tm_umts_up_supl_terminate_timer_cancel_cf_state( supl_cf_state_enum_type e_timer_state_to_terminate );
void tm_umts_up_supl_tcp_connect_notif(void);
void tm_umts_up_supl_hash_proc ( uint8 *p_hash_data, uint16 w_supl_init_len ); 
void tm_umts_up_supl_notif_verif_resp_proc(pdsm_lcs_notification_resp_s_type *p_user_resp);
void tm_umts_up_supl_E911_handler(void);
boolean  tm_umts_up_supl_supl_end_prepare_or_tx( supl_slp_session_id* p_provided_slp_session_id,
                                                 supl_set_session_id* p_provided_set_session_id,
                                                 supl_StatusCode e_supl_status_code, 
                                                 uint8* p_hash,
                                                 uint32 q_supl_version );
boolean tm_umts_up_supl_supl_end_tx (supl_slp_session_id* p_provided_slp_session_id, 
                         supl_set_session_id*             p_provided_set_session_id,
                         supl_StatusCode e_supl_status_code,
                         uint8* p_hash,
                         uint32 q_supl_version);
void tm_umts_up_supl_inform_core_stop_session( tm_sess_stop_reason_e_type stop_reason);
void tm_umts_up_supl_callflow_kind_set (supl_callflow_kind_enum_type u_callflow_kind);
boolean tm_umts_up_supl_get_pdcomm_tcp_handle(void);
void tm_umts_up_supl_update_mlid_db (const cgps_CellInfoCachedStruct *cell_db_data_ptr );
void tm_umts_up_supl_cell_db_update( const cgps_CellInfoCachedStruct *cell_db_data_ptr );
void tm_umts_up_supl_send_wcdma_cell_meas_req( void );
void tm_umts_up_supl_cell_meas_update( wl1_lsm_cell_meas_update_type   *cell_meas_update_ptr );

#ifdef FEATURE_CGPS_LTE_CELLDB
void tm_umts_up_supl_lte_cell_meas_update( lte_ml1_gps_mrl_cell_meas_update_t *cell_meas_update_ptr ); 
#endif

void tm_umts_up_supl_assist_req_holdoff_timers_check_start ( uint32 *p_wishlist_mask );

void tm_umts_up_supl_glo_assist_req_holdoff_timers_check_start ( uint32 *p_wishlist_mask );

void tm_umts_up_supl_pa_proc ( prtl_pa_event_s_type *p_pa_event  );
boolean tm_umts_supl_efs_dir_init(void);
boolean tm_umts_supl_if_need_forced_wcmda_cell_inq (void);
boolean tm_umts_up_supl_req_cell_info_from_rrc(supl_cf_state_enum_type cf_state);
void tm_umts_up_supl_ck_req_tdscmda_cell_info(void);
void tm_umts_up_supl_send_comm_failure(void );     
boolean tm_umts_supl_wcdma_plmn_id_organizer 
( rrc_plmn_identity_type *p_plmn_id_info,
  uint32 *p_mcc, 
  uint32 *p_mnc
);

#ifdef FEATURE_CGPS_LTE_CELLDB
boolean tm_umts_supl_lte_plmn_id_organizer 
( lte_rrc_plmn_s *p_plmn_id_info,
  uint32 *p_mcc, 
  uint32 *p_mnc
);
#endif


boolean supl_header_build ( 
 supl_ULP_PDU*        p_pdu, 
  uint16*              p_provided_set_session_id,
  supl_set_id*         p_provided_Set_Id,
  supl_slp_session_id* p_provided_slp_session_id,
  uint32               q_supl_version);

void tm_umts_up_supl_assist_wishlist_rti_adj(sm_InternalAssistDataStatusStructType* p_sm_AssistDataStatus);

void supl_set_cf_state( supl_cf_state_enum_type e_cf_state, uint32 u_supl_cf_lm_timer_ms );

uint8 tm_umts_up_supl_select_pos_prot(void);

boolean  supl_start_is801_session(boolean wake_tm_core, tm_sess_req_start_param_s_type  *actions);
boolean  supl_stop_is801_session(const tm_sess_stop_reason_e_type stop_reason);

/*Functions used by IS801 protocol*/
boolean tm_umts_up_supl_SuplPos_is801_tx (uint8 *p_is801_data, const uint16 w_byte_cnt);
void tm_umts_up_supl_is801_session_stop (const tm_sess_stop_reason_e_type   stop_reason);
void tm_umts_up_supl_is801_nt_assist_done (boolean sess_end_sent);
void tm_umts_up_supl_is801_pos_reported (void);
void tm_umts_up_supl_is801_inform_1x_pos_method (supl_session_agps_mode_enum_type mode);

uint32 supl_comm_getnext_gm_transaction_id(void);

void tm_umts_up_supl_gm_init(void);
uint8 tm_umts_up_supl_geofence_areaEventSession_match (uint32 q_geofence_id );
void tm_umts_up_supl_area_event_to_active (uint8 u_session_num);
void tm_umts_up_supl_gf_breach_proc( sm_gm_client_breach_notify_type*  p_gm_client_breach_notify);
void tm_umts_up_supl_gf_delete_proc( sm_gm_client_notification_type*  p_gm_client_notify);
void tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event 
 ( supl_cf_state_enum_type      e_timer_state_to_terminate,
   uint8                        u_session_num );

void tm_umts_up_supl_lpp_tx_mgr_init(void);

void tm_umts_up_supl_handle_otdoa_data(sm_loc_lte_otdoa_meas_info_type const *p_OtdoaData); 

slp_mess_header_check_result_e_type supl_version_negotiation
( supl_ULP_PDU* p_pdu_decoded, 
  uint32 *p_supl_version_for_calling_func,
  uint8  u_session_num );


boolean tm_umts_up_supl_rrlp_assisted_glonass_enabled(void);

void tm_umts_up_supl_external_cell_db_update( const pdsm_cell_info_s_type external_cell_info );

cell_db_type tm_umts_up_supl_get_cell_db_type(void);

void supl_session_status_reset( void );

boolean  supl_session_is_emergency_session( void);

boolean tm_umts_up_is_supl_enabled(void);
void tm_umts_up_supl_ota_delete_proc ( void);

void tm_umts_up_supl_wifi_scan_report_proc  (prtl_wifi_scan_event_s_type z_wifi_scan_result);

boolean supl_SuplStart_tx (void);
boolean supl_SuplPosInit_tx (void);
void tm_up_supl_handle_otdoa_mul_meas_timer_expiry( void );

/* 
 ******************************************************************************
 *
 * tm_up_supl_handle_early_response_timer_expiry
 *
 * Function description:
 *        This function handles Early timer expiry. UE will provide LCS with
 *        MEAS/Position report based on the type of session MSA/MSB.
 *
 * Parameters: none.
 *   
 * Return value: none.
 *
 ******************************************************************************
*/ 
void tm_up_supl_handle_early_response_timer_expiry(void);

/* 
 ******************************************************************************
 *
 * tm_up_supl_copy_rli
 *
 * Function description:
 *         This function retrieves the Transaction for the original RLI based on
 *         rer_loc_type and saves it in local data structure.
 *
 * Parameters:
 *         flags: ref_loc_type to look up RLI.
 *   
 * Return value:
 *
 ******************************************************************************
*/

void tm_up_supl_copy_rli(uint32 const flag);
/* 
 ******************************************************************************
 *
 * tm_umts_up_supl_add_r12earlyResponse_tx
 *
 * Function description:
 *     This function duplicates the original transaction for sending out
 *     early Measurement report/Location estimate.
 *
 * Parameters: none.
 *   
 * Return value: none.
 *
 ******************************************************************************
*/ 
void tm_up_supl_add_r12earlyResponse_tx (void);

/* 
 ******************************************************************************
 *
 * tm_umts_up_supl_r12earlyResponse_valid
 *
 * Function description:
 *     This function check for Early Response validity in
 *     1) Hybrid CF: if atleast one of PRM or OTDOA RSTD is present.
 *     2) OTDOA  CF: if OTDOA RSTD's are present.
 *
 * Parameters:
 *    Boolean:  v_Rstd.
 *    Flags  :  q_flags.
 *   
 * Return Boolean:
 *    TRUE  : Send Early Response
 *    FALSE : Drop Early Response
 *
 ******************************************************************************
*/ 
boolean tm_umts_up_supl_r12earlyResponse_valid (uint32 q_flags, boolean v_Rstd);

/*===========================================================================

FUNCTION tm_umts_up_supl_refresh_efs_nv

DESCRIPTION
  This function handles the refresh of EFS NV items. 
  
DEPENDENCIES: none.

RETURN VALUE: none

SIDE EFFECTS: none.

===========================================================================*/
void tm_umts_up_supl_refresh_efs_nv(void );


/*
 ******************************************************************************
 *
 * tm_up_slp_support_glonass
 *
 * Function description:
 *   Based on the receive SUPL data decide whether SLP server has GLONASS capability.
 *
 * Parameters:
 *   
 *
 * Return value:
 *  True if SLP is GLONASS capable, so UE can ask for GLONASS AD. Otherwise False.
 *
 ******************************************************************************
*/
boolean tm_up_slp_support_glonass( supl_status_s_type const *const p_supl_session_status );


/*
 ******************************************************************************
 *
 * tm_umts_up_supl_enqueue_msg
 *
 * Function description:
 *   Enqueue the SUPL message in the case where we wait for ACK before sending the
 *   next message
 *
 * Parameters:
 *   tm_pdu_buf_type *pdu_buff
 *   
 * Return value:
 *  
 *
 ******************************************************************************
*/
void tm_umts_up_supl_enqueue_msg(tm_pdu_buf_type *supl_msg);

/*
 ******************************************************************************
 *
 * tm_umts_up_supl_process_write_ack
 *
 * Function description:
 *   Dequeue the SUPL message, check if we need to send any more messages and
 *   send it to pd_comms.
 *
 * Parameters:
 * 
 *   
 * Return value:
 *  
 *
 ******************************************************************************
*/

void tm_umts_up_supl_process_write_ack(void *msg_data_ptr);

#ifdef __cplusplus
}
#endif

#endif /* TM_SUPL_H */

