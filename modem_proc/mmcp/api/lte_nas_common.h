/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/lte_nas_common.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $


when         who                          what                              
--------     --------  ------------------------------------------------ 
07/10/10     zren      Updated nas_tech_id enum with NAS_1XCP
12/10/09     zren      Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
10/13/09     zren      Enabled FEATURE_NAS_PDN_CONNECTIVITY_REQ_RSP
09/16/09     zren      Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/14/09     MNK       Remove Bearer resource release and add modification messages
07/29/09     hnam      Changed the name of the MACRO (for MARCH/DEC spec compatibiltiy), 
                        used the standard macro commonly used by LTE_MODEM_SW
07/07/09     zren      Moved define FEATURE_RELEASE_LTE_MARCH_SPEC from 
                       msg_lib_encode_emm.c
04/25/09     hnam      Removed Length parameter in EPS_BEARER_CONTEXT_STATUS
04/24/09     MNK       Moved lte_nas_eps_bearer_context_status_type from emm_message_description.
04/06/09     MNK       Added definitions for TRANSACTION_ID
03/25/09     hnam      Changed emm cause "LTE_NAS_MSG_INCOMPATIBLE_WITH_PROTOCOL_STATE" from 100 to 101
02/27/09     MNK       Added emm and esm cause values and valid flag in the header
02/03/09     hnam      Removed MSG_LOW, MSG_MED, MSG_HIGH, ERR
==============================================================================*/
#ifndef _LTE_NAS_COMMON_H_
#define _LTE_NAS_COMMON_H_

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include<customer.h>



#include "comdef.h"
#include "cm_gw.h"
//#define FEATURE_LTE_3GPP_REL8_MAR09

#define EMM_ADDITIONAL_INFORMATION_IEI 0x65

/* define a message type for lte_nas_emm_message_id; Refer Section: 9.8*/
typedef enum{
    ATTACH_REQUEST                  =65,
    ATTACH_ACCEPT                   =66,
    ATTACH_COMPLETE                 =67,
    ATTACH_REJECT                   =68,
    DETACH_REQUEST                  =69,
    DETACH_ACCEPT                   =70,

    TRACKING_AREA_UPADTE_REQUEST    =72,
    TRACKING_AREA_UPDATE_ACCEPT     =73,
    TRACKING_AREA_UPDATE_COMPLETE   =74,
    TRACKING_AREA_REJECT            =75,

    EXT_SERVICE_REQUEST             =76,
    SERVICE_REJECT                  =78,
    GUTI_REALLOCATION_COMMAND       =80,
    GUTI_REALLOCATION_COMPLETE      =81,
    EMM_AUTHENTICATION_REQUEST      =82,
    EMM_AUTHENTICATION_RESPONSE     =83,
    EMM_AUTHENTICATION_REJECT       =84,

    EMM_AUTHENTICATION_FAILURE      =92,
    EMM_IDENTITY_REQUEST            =85,
    EMM_IDENTITY_RESPONSE           =86,
    SECURITY_MODE_COMMAND           =93,
    SECURITY_MODE_COMPLETE          =94,
    SECURITY_MODE_REJECT            =95,

    EMM_STATUS                     =96,
    EMM_INFORMATION                =97,
    DOWNLINK_NAS_TRANSPORT         =98,
    UPLINK_NAS_TRANSPORT           =99,
    CS_SERV_NOTIFICATION           =100,

    DL_GENERIC_NAS_TRANSPORT       =104,
    UL_GENERIC_NAS_TRANSPORT       =105,

    ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST  = 193,
    ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT  =  194,
    ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT  = 195,

    ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST  = 197,
    ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT   = 198,
    ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT   = 199,

    MODIFY_EPS_BEARER_CONTEXT_REQUEST  = 201,
    MODIFY_EPS_BEARER_CONTEXT_ACCEPT   = 202,
    MODIFY_EPS_BEARER_CONTEXT_REJECT   = 203,

    DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST = 205,
    DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT = 206,

    PDN_CONNECTIVITY_REQUEST   = 208,
    PDN_CONNECTIVITY_REJECT    = 209,

    PDN_DISCONNECT_REQUEST     = 210,
    PDN_DISCONNECT_REJECT      = 211,

    BEARER_RESOURCE_ALLOCATION_REQUEST = 212,
    BEARER_RESOURCE_ALLOCATION_REJECT  = 213,

    BEARER_RESOURCE_MODIFICATION_REQUEST    = 214,
    BEARER_RESOURCE_MODIFICATION_REJECT     = 215,

    ESM_INFORMATION_REQUEST  =  217,
    ESM_INFORMATION_RESPONSE =  218,

    ESM_NOTIFICATION = 219,

    ESM_STATUS = 232,
    ESM_LAST_UNDEFINED

}lte_nas_message_id; 

/*
     +---------+---------+----------+--------------------------------+
      |     8 bits   |8 bits      |             16 bits              |
      |  TECHNOLOGY  |MODULE_ID   |        Sequence Numbe            |
      +--------------------------------------------------------------+
      - TECHNLOGY specifies LTE, eHRPD etc..
      - MODULE_ID specifies ESM-BCM, ESM-BPM, EMM etc..
      - Sequence number is genrated by the MODULE. It is up to the MODULE how to use these 16 bits
      - the 32 bits put together is the transaction id used between 
      the EMM and RRC
*/

/*Define lte_nas_emm_protocol_discriminator; Refer section: 9.9.3.19*/

typedef enum{
    GROUP_CALL_CONTROL,
    BROADCAST_CALL_CONTROL,
    EPS_SESSION_MANAGEMENT_MESSAGES,
    CALL_CONTROL_RELATED_TO_SS_MESSAGE, 
    GPRS_TRANSPARENT_TRANSPORT_PROTOCOL,
    MOBILITY_MANAGEMENT_MESSAGES,
    RADIO_RESOURCES_MANAGEMENT_MESSAGES,
    EPS_MOBILITY_MANAGEMENT_MESSAGES,
    GPRS_SESSION_MANAGEMENT_MESSAGES,
    SMS_MESSGES,
    NON_CALL_RELATED_SS_MESSAGES,
    LOCATION_SERVICES,
    RESERVED_FOR_EXTENSION_OF_THE_PD_TO_ONE_OCTET_LENGTH,
    RESERVED_FOR_TESTS_PROCEDURES = 15
}lte_nas_protocol_discriminator;

/* define a message type for lte_nas_emm_message_id; Refer Section: 9.8*/

/* Required for type_of_emm_cause... Refer section: 9.9.3.7 */

typedef enum
{
  LTE_NAS_CAUSE_NONE = -1,
  LTE_NAS_IMSI_UNKNOWN_IN_HSS = 2,
  LTE_NAS_ILLEGAL_UE = 3,
  LTE_NAS_IMEI_NOT_ACCEPTED = 5,
  LTE_NAS_ILLEGAL_ME = 6,
  LTE_NAS_EPS_SERVICES_NOT_ALLOWED = 7,
  LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED = 8,
  LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW = 9,
  LTE_NAS_IMPLICITLY_DETACHED = 10,
  LTE_NAS_PLMN_NOT_ALLOWED = 11,
  LTE_NAS_TRACKING_AREA_NOT_ALLOWED =12,
  LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA=13,
  LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN =14,
  LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA=15,
  LTE_NAS_MSC_TEMPORARILY_NOT_REACHABLE=16,
  LTE_NAS_NW_FAILURE=17,
  LTE_NAS_CS_DOMAIN_NOT_AVAILABLE=18,
  LTE_NAS_ESM_FAILURE=19,
  LTE_NAS_MAC_FAILURE=20,
  LTE_NAS_SYNCH_FAILURE=21,
  LTE_NAS_CONGESTION=22,
  LTE_NAS_UE_SECURITY_CAPABILITIES_MISMATCH=23,
  LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED =24,
  LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG=25,
  LTE_NON_EPS_AUTH_UNACCEPTABLE = 26,
  LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED = 38,
  LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE = 39,
  LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED=40,
  LTE_NAS_SEVERE_NW_FAILURE =42,
  LTE_NAS_SEMANTICALLY_INCORRECT_MSG=95,
  LTE_NAS_INVALID_MANDATORY_INFO =96,
  LTE_NAS_NON_EXISTENT_MSG_TYPE=97,
  LTE_NAS_MSG_TYPE_INCOMPATIBLE_WITH_PROTOCOL_STATE=98,
  LTE_NAS_NON_EXISTANT_INFO=99,
  LTE_NAS_CONDITIONAL_IE_ERROR =100,
  LTE_NAS_MSG_INCOMPATIBLE_WITH_PROTOCOL_STATE=101,
  LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED =111,

  LTE_NAS_NO_FAILURE = 256 /* IF ADDING ANY NEW CAUSE VALUES ADD BEFORE THIS*/
}lte_nas_emm_cause_type;

typedef enum 
{
  LTE_ESM_BCM = 1,
  LTE_ESM_BPM = 2,
  LTE_NAS_EMM = 3,
  LTE_NAS_GPS = 4
}lte_nas_module_id_T;

typedef enum
{
  NAS_LTE  = 1,
  NAS_GW   = 2,
  NAS_1XCP = 3,
  NAS_HDR  = 4
}nas_tech_id;

typedef struct
{
  boolean valid;
  boolean isr_status;
} lte_nas_isr_status_type;


/*define a structure for header type*/
typedef struct 
{
  lte_nas_protocol_discriminator pd;
  lte_nas_message_id msg_id;
  boolean valid;
  lte_nas_emm_cause_type emm_decode_fail_reason;
  esm_cause_enum_T esm_decode_fail_reason;
  byte             *ota_msg_ptr;
  word             ota_msg_len;
} lte_nas_hdr_type;

typedef struct
{
  boolean    eps_bearer_context_5_exists;
  boolean    eps_bearer_context_6_exists;
  boolean    eps_bearer_context_7_exists;
  boolean    eps_bearer_context_8_exists;
  boolean    eps_bearer_context_9_exists;
  boolean    eps_bearer_context_10_exists;
  boolean    eps_bearer_context_11_exists;
  boolean    eps_bearer_context_12_exists;
  boolean    eps_bearer_context_13_exists;
  boolean    eps_bearer_context_14_exists;
  boolean    eps_bearer_context_15_exists;
  boolean    emc_voice_bearer_exist;
}lte_nas_eps_bearer_context_status_type;

/* ENUM to define the SRVCC handover Indicator in the Notification message,
   sent from NW to ESM to CM */
typedef enum 
{
  SRVCC_HO_CANCELLED = 1
     
}esm_notification_ind_e_type;

#endif
