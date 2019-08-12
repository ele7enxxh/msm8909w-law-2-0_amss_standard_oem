#ifndef CM_GW_H
#define CM_GW_H


/*==========================================================================

               C A L L   M A N A G E R   H E A D E R   F I L E 
===========================================================================*/ 

/**
@file  cm_gw.h

GSM/WCDMA protocol definitions for clients.
*/

/*------------------------------------------------------------------------------
     Copyright (c) 2002 - 2011 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
------------------------------------------------------------------------------*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/cm_gw.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/*--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "comdef.h"


/** @addtogroup CM_GW GSM/WCDMA Protocol Definitions for Clients
    @{ */

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/


/*  NAS Definitions from nas_exp.h */


/** Maximum user data length for UUS1 explicit, UUS2 and UUS3.
*/
#define MAX_USER_USER_DATA_LENGTH       128  /* value guessed */

/** Maximum user data length for UUS1 implicit.
*/
#define MAX_USER_USER_DATA_1_LENGTH      32  


/* NAS Definitions from sys_cnst.h */

/** Identifies reasons for a synchronization indication.
*/
typedef enum
{
   /* Reasons mapped from rr_sync_ind */
   GSM_RES_ASSIGNED,            /**< &nbsp; */
   GSM_CHANNEL_MODE_MODIFIED,   /**< &nbsp; */
   GSM_GSM_HANDOVER,            /**< &nbsp; */
   WCDMA_GSM_HANDOVER,          /**< &nbsp; */

   /* Reasons mapped from rrc_sync_ind */
   WCDMA_RAB_ESTABLISHED,       /**< &nbsp; */
   WCDMA_RAB_RELEASED,          /**< &nbsp; */
   WCDMA_RAB_RECONFIGURED,      /**< &nbsp; */
   GSM_WCDMA_HANDOVER           /**< &nbsp; */
}sync_enum_T;


/*
 *  definitions for MO call setup bearer capability
 */

/** Radio channel requirement value as full rate support only.
*/
#define FULL_RATE_SUPPORT_ONLY         0x01

/** Radio Channel requirement value as dual rate support but
    half rate is preferred.
*/
#define DUAL_SUPPORT_HALF_PREFERRED    0x02

/** Radio Channel requirement value as dual rate support but
    full rate is preferred.
*/
#define DUAL_SUPPORT_FULL_PREFERRED    0x03

/** Radio Channel requirement default value as full rate support only.
*/
#define DEFAULT_SPEECH_RC_REQUIREMENT  FULL_RATE_SUPPORT_ONLY

/** Information transfer capability value as speech.
*/
#define SPEECH_ITC                     0

/*
**  NAS Definitions from sys_type.h
*/

/*
**  NAS Definitions from mn_cm_exp.h
*/

/** @name CM Data Size Limitations
    @{ */

/** Maximum length of lower layer compatibility data.
*/
#define LL_COMPATIBILITY_DATA_SIZE          13  
								
/** Maximum length of a called party number.
*/
#define CM_CALLED_PARTY_BCD_NO_LENGTH       41 

/** Maximum length of a called party subaddress.
*/
#define CM_CALLED_PARTY_SUBADDRESS_LENGTH   21 

/** Maximum length of a calling party number.
*/
#define CM_CALLING_PARTY_BCD_NO_LENGTH      17 

/** Maximum length of a calling party subaddress.
*/
#define CM_CALLING_PARTY_SUBADDRESS_LENGTH  21 

/** Maximum length of a connected number.
*/
#define CM_CONNECTED_NO_LENGTH              20 

/** Maximum length of a connected subaddress.
*/
#define CM_CONNECTED_SUBADDRESS_LENGTH      21 

/** Maximum length of the data held in a subscription violation
    extension container.
*/
#define CM_SUBS_VIOL_EXT_CONTAINER_LENGTH   50 

/** Used to pass the BCD buffer to cm_util_number_to_bcd() or
    cm_util_bcd_to_ascii(). These functions put the BCD length in
    the same array, and because of this they need an extra byte.

    Clients must check for CM_API_ECATEG_IN_NUM_STRUCT before
    using this field.
*/
#define CM_CALLED_PARTY_BCD_WITH_BCD_LEN  CM_CALLED_PARTY_BCD_NO_LENGTH + 1

/** Maximum length of the diagnostics that are present
    in a cause information element.
*/
#define MAX_DIAGNOSTICS_LENGTH              27 

/* SS List and array size values */

/** Maximum number of active calls.
*/
#define ACTIVE_CALLS_LIST_SIZE              14  

/** Maximum size of the call forward feature list.
*/
#define FWD_FEATURE_LIST_SIZE               13  

/** Maximum size of the call barring list.
*/
#define CALL_BARRING_LIST_SIZE              13  

/** Maximum size of the basic service group list.
*/
#define BASIC_SERV_GROUP_LIST_SIZE          13  

/** Maximum size of a Closed User Group subscription list.
*/
#define CUG_SUBSCRIPTION_LIST_SIZE          10  

/** Maximum size of a Closed User Group feature list.
*/
#define CUG_FEATURE_LIST_SIZE               13  

/** Maximum size of a Closed User Group indicator.
*/
#define CUG_INDICATOR_SIZE                   2  

/** Maximum size of a Closed User Group interlock.
*/
#define CUG_INTERLOCK_SIZE                   4  

/** Maximum number of characters in a password.
*/
#define MAX_PWD_CHAR                         4  

/** Maximum number of characters allowed in
    Unstructured Supplementary Services data.
*/
#define MAX_USS_CHAR                       160  

/** Maximum number of characters allowed in Supplementary Services user data.
*/
#define MAX_SS_USER_DATA_CHAR              200 

/** Maximum size of a call completion to busy subscriber list.
*/
#define MAX_CCBS_FEATURE_LIST_SIZE           5  

/* Forward Feature List params, bcd no, = 9 */

/** Maximum number of call forwarded to numbers.
*/
#define MAXNO_FORWARD_TO_NUMBER              1 

/* Codec list IE  */
/* 4 bits for each SysID */

/** Maximum size of supported multimedia codecs list.
*/
#define MAX_SUPPORTED_MULTIMEDIA_CODEC_LIST 16  

/** @} */

/** @name Bearer Capability Definitions
    @{ */

/* These definitions for Phase2+: used in both MN and DS */

/** Bearer capability configuration value. Indicates
    that Acceptable Channel Coding is present.
*/
#define ACC_NONE                              0x00 

/** Bearer capability configuration value. Indicates
  that Other Modem Type is undefined.
*/
#define OMT_UNDEFINED                         0xff 

/** Bearer capability configuration value. Indicates
  that the Fixed Network User Rate ID is undefined.
*/
#define FNUR_UNDEFINED                        0xff 

/** Bearer capability configuration value. Indicates
  that the maximum number of Traffic Channels is undefined.
*/
#define MAX_NUMBER_OF_TCH_UNDEFINED           0xff 

/** Bearer capability configuration value. Indicates
  that the User-Initiated Modification Indication is undefined.
*/
#define UIMI_UNDEFINED                        0xff 

/** Bearer capability configuration value. Indicates
  that the Wanted Air Interface User Rate is undefined.
*/
#define WAIUR_UNDEFINED                       0xff

/** @} */

/* SS Misc default setting */

/** @name SS Miscellaneous Default Settings
    @{ */

/** Default standard location. Used in the cause information element.
*/
#define STD_LOCATION                    LOCATION_USER  

/** Default standard coding standard. Used in the cause information element.
*/
#define STD_CODESTANDARD                (STANDARD_GSM) 

/** Start of the silent redial rejection causes.
*/
#define SILENT_REDIAL_REJ_CAUSE_BEGIN       (MS_INTERNAL_CAUSE_VALUE_BASE+0) 

/** End of silent redial rejection causes.
*/
#define SILENT_REDIAL_REJ_CAUSE_END         (MS_INTERNAL_CAUSE_VALUE_BASE+22)

/* TS24.008, 10.5.4.22, Repeat Indicator: */

/** Repeat indicator value; the type is circular.
*/
#define REPEAT_CIRCULAR                  0x01 

/** Repeat indicator value; the type is fallback.
*/
#define REPEAT_SUPPORTS_FALLBACK         0x02 

/* R99 */

/** Repeat indicator value; the type is sequential.
*/
#define REPEAT_SEQUENTIAL                0x03 

/* R5+ */

/** Repeat indicator; reserved value.
*/
#define REPEAT_RESERVED                  0x03 

/** Repeat indicator value; the type is service change and fallback.
*/
#define REPEAT_SERVICE_CHANGE_FALLBACK   0x04 

/* CM Information Element values */ 

/** Default cause value.
*/
#define CM_CAUSE_DEFAULT                16 

/** Default Calling Line Identification Restriction value.
*/
#define CM_CLIR_DEFAULT                 00 

/** Calling Line Identification Restriction invocation.
*/
#define CM_CLIR_INVOCATION              11 

/** Calling Line Identification Restriction suppression.
*/
#define CM_CLIR_SUPPRESSION             12 

/** Calling Line Identification Restriction suppression rejected.
*/
#define CM_CLIR_SUPPRESSION_REJECTED    46 

/** Relay protocol originator address.
*/
#define CM_RP_ORIGINATOR_ADDRESS        60 

/** Calling Line Identification Restriction.
*/
#define CM_CLI_RESTRICTION              62 

/** @} */

/* Supplementary Service Operation Codes */

/* obtained from GSM 09.02 14.7.5  */

/** @name Supplementary Services Operation Codes
    @{ */

/** Register a Supplementary Services operation code.
*/
#define registerSS                      10 

/** Erase a Supplementary Services operation code.
*/
#define eraseSS                         11 

/** Activate a Supplementary Services operation code.
*/
#define activateSS                      12 

/** Deactivate a Supplementary Services operation code.
*/
#define deactivateSS                    13

/** Interrogate a Supplementary Services operation code.
*/
#define interrogateSS                   14 

/** Notify a Supplementary Services operation code.
*/
#define notifySS                        16 

/** Register password operation code.
*/
#define registerPassword                17 

/** Get password operation code.
*/
#define getPassword                     18 

/** Process an Unstructured Supplementary Services data operation code.
*/
#define processUnstructuredSS_Data      19 

/** Forward check a Supplementary Services indication operation code.
*/
#define forwardCheckSS_Indication       38 

/** Process an Unstructured Supplementary Services request operation code.
*/
#define processUnstructuredSS_Request   59 

/** Unstructured Supplementary Services request operation code.
*/
#define unstructuredSS_Request          60 

/** Unstructured Supplementary Services notify operation code.
*/
#define unstructuredSS_Notify           61 

/** Erase a Call Control entry operation code.
*/
#define eraseCCEntry                    77 

/** Mobile Originated Location Registration operation code.
*/
#define lcs_MOLR                        115 

/** Location Notification operation code.
*/
#define lcs_LocationNotification        116 

/** Call deflection operation code.
*/
#define callDeflection                  117 

/** User to User Service operation code.
*/
#define userUserService                 118 

/** Access register Call Control entry operation code.
*/
#define accessRegisterCCEntry           119 

/** Forward Closed User Group information operation code.
*/
#define forwardCUG_Info                 120 

/** Split Multi-Party operation code.
*/
#define splitMPTY                       121 

/** Retrieve Multi-Party operation code.
*/
#define retrieveMPTY                    122 

/** Hold Multi-Party operation code.
*/
#define holdMPTY                        123 

/** Build Multi-Party operation code.
*/
#define buildMPTY                       124 

/** Forward charge advice operation code.
*/
#define forwardChargeAdvice             125 

/** Explicit Call Transfer operation code.
*/
#define explicitCT                      126 

/** Default value for operation code.
*/
#define NULL_SS_OPERATION               0xff

/** @} */

/** @name SUPS Error Codes
    @{ */

/* SS error codes */

/** Unknown subscriber.
*/
#define   unknownSubscriber            0x01 

/** Illegal subscriber.
*/
#define   illegalSubscriber            0x09 

/** Bearer service not provisioned.
*/
#define   bearerServiceNotProvisioned  0x0A 

/** Telephony service not provisioned.
*/
#define   teleServiceNotProvisioned    0x0B 

/** Call barred.
*/
#define   callBarred                   0x0D 

/** Illegal equipment.
*/
#define   illegalEquipment             0x0C 

/** Illegal operation.
*/
#define   illegalSS_Operation          0x10 

/** Status error.
*/
#define   ss_ErrorStatus               0x11 

/** SUPS not available.
*/
#define   ss_NotAvailable              0x12 

/** Subscription violation.
*/
#define   ss_SubscriptionViolation     0x13 

/** SUPS incompatibility.
*/
#define   ss_Incompatibility           0x14 

/** Facility not supported.
*/
#define   facilityNotSupported         0x15  

/** Absent subscriber.
*/
#define   absentSubscriber             0x1B 

/** Short term denial.
*/
#define   shortTermDenial              0x1D 

/** Long term denial.
*/
#define   longTermDenial               0x1E 

/** System failure.
*/
#define   systemFailure                0x22 

/** Data missing.
*/
#define   dataMissing                  0x23 

/** Unexpected data value.
*/
#define   unexpectedDataValue          0x24 

/** Password registration failure.
*/
#define   passwordRegistrationFailure  0x25 

/** Negative password check.
*/
#define   negativePasswordCheck        0x26 

/** Number of password attempts violation.
*/
#define   numberOfPW_AttemptsViolation 0x2B 

/** Position method failure.
*/
#define   positionMethodFailure        0x36 

/** Unknown alphabet.
*/
#define   unknownAlphabet              0x47 

/** USSD busy.
*/
#define   ussd_Busy                    0x48 

/** Rejected by user.
*/
#define   rejectedByUser               0x79 

/** Rejected by network.
*/
#define   rejectedByNetwork            0x7A 

/** Deflection to served subscriber.
*/
#define   deflectionToServedSubscriber 0x7B 

/** Special service code.
*/
#define   specialServiceCode           0x7C 

/** Invalid Deflected-To number.
*/
#define   invalidDeflectedToNumber     0x7D 

/** Maximum number of Multi-Party participants exceeded.
*/
#define   maxNumberOfMPTY_ParticipantsExceeded  0x7E 

/** Resources not available.
*/
#define   resourcesNotAvailable        0x7F 

/** @} */

/* TELE-SERVICE codes */

/** @name Tele-Service Codes
    @{ */

#define allTeleservices                   0x00 
#define allSpeechTransmissionservices     0x10 
#define telephony                         0x11 
#define emergencyCalls                    0x12 
#define allShortMessageServices           0x20 
#define shortMessageMT_PP                 0x21 
#define shortMessageMO_PP                 0x22 
#define allFacsimileTransmissionServices  0x60 
#define facsimileGroup3AndAlterSpeech     0x61 
#define automaticFacsimileGroup3          0x62 
#define facsimileGroup4                   0x63 
#define allDataTeleservices               0x70 
#define allTeleservices_ExeptSMS          0x80 
#define allPLMN_specificTS                0xD0 
#define plmn_specificTS_1                 0xD1 
#define plmn_specificTS_2                 0xD2 
#define plmn_specificTS_3                 0xD3 
#define plmn_specificTS_4                 0xD4 
#define plmn_specificTS_5                 0xD5 
#define plmn_specificTS_6                 0xD6 
#define plmn_specificTS_7                 0xD7 
#define plmn_specificTS_8                 0xD8 
#define plmn_specificTS_9                 0xD9 
#define plmn_specificTS_A                 0xDA 
#define plmn_specificTS_B                 0xDB 
#define plmn_specificTS_C                 0xDC 
#define plmn_specificTS_D                 0xDD 
#define plmn_specificTS_E                 0xDE 
#define plmn_specificTS_F                 0xDF 

/** @} */

/* BEARER-SERVICE codes */

/** @name Bearer-Service Codes
    @{ */

#define allBearerServices                 0x00 
#define allDataCDA_Services               0x10 
#define dataCDA_300bps                    0x11 
#define dataCDA_1200bps                   0x12 
#define dataCDA_1200_75bps                0x13 
#define dataCDA_2400bps                   0x14 
#define dataCDA_4800bps                   0x15 
#define dataCDA_9600bps                   0x16 
#define allDataCDS_Services               0x18 
#define dataCDS_1200bps                   0x1A 
#define dataCDS_2400bps                   0x1C 
#define dataCDS_4800bps                   0x1D 
#define dataCDS_9600bps                   0x1E 
#define allPadAccessCA_Services           0x20 
#define padAccessCA_300bps                0x21 
#define padAccessCA_1200bps               0x22 
#define padAccessCA_1200_75bps            0x23 
#define padAccessCA_2400bps               0x24 
#define padAccessCA_4800bps               0x25 
#define padAccessCA_9600bps               0x26 
#define allDataPDS_Services               0x28 
#define dataPDS_2400bps                   0x2C 
#define dataPDS_4800bps                   0x2D 
#define dataPDS_9600bps                   0x2E 
#define allAlternateSpeech_DataCDA        0x30 
#define allAlternateSpeech_DataCDS        0x38 
#define allSpeechFollowedByDataCDA        0x40 
#define allSpeechFollowedByDataCDS        0x48 
#define allDataCircuitAsynchronous        0x50 
#define allAsynchronousServices           0x60 
#define allDataCircuitSynchronous         0x58 
#define allSynchronousServices            0x68 
#define allPLMN_specificBS                0xD1 
#define plmn_specificBS_1                 0xD1 
#define plmn_specificBS_2                 0xD2 
#define plmn_specificBS_3                 0xD3 
#define plmn_specificBS_4                 0xD4 
#define plmn_specificBS_5                 0xD5 
#define plmn_specificBS_6                 0xD6 
#define plmn_specificBS_7                 0xD7 
#define plmn_specificBS_8                 0xD8 
#define plmn_specificBS_9                 0xD9 
#define plmn_specificBS_A                 0xDA 
#define plmn_specificBS_B                 0xDB 
#define plmn_specificBS_C                 0xDC 
#define plmn_specificBS_D                 0xDD 
#define plmn_specificBS_E                 0xDE 
#define plmn_specificBS_F                 0xDF 

/** @} */

/* Invoke problem codes */

/** @name Invoke Problem Codes
    @{ */

#define   DUPLICATE_INVOKE_ID          0x00 
#define   UNRECOGNISED_OPERATION       0x01 
#define   MISTYPED_PARAMETER           0x02 
#define   RESOURCE_LIMITATION          0x03 
#define   INITIATING_RELEASE           0x04
#define   UNRECOGNISED_LINKED_ID       0x05 
#define   LINKED_RESPONSE_UNEXPECTED   0x06 
#define   UNEXPECTED_LINKED_OPERATION  0x07

/** @} */

/* Return Result problem codes */

/** @name Return Result Problem Codes
    @{ */

#define   UNRECOGNISED_INVOKE_ID       0x00 
#define   RETURN_RESULT_UNEXPECTED     0x01

/** @} */

/* Return Error problem codes */

/** @name Return Error Problem Codes
    @{ */

#define   RETURN_ERROR_UNEXPECTED      0x01 
#define   UNRECOGNISED_ERROR           0x02 
#define   UNEXPECTED_ERROR             0x03 
#define   RE_MISTYPED_PARAMETER        0x04 

/** @} */

/* MN specific values for ss errors     */
/* used by CM */

/** @name MN-Specific Values for SS Errors
    @{ */

/** Error code tag for SS error returned from network.
*/
#define   MNSS_ERROR_TAG              0x02 

/* used by MN -> network */
/** Error code tag for local SS errors.
*/
#define   MN_SS_ERROR_TAG             0xc0 

/** Local error; insufficient resources.
*/
#define   MN_INSUFFICIENT_RESOURCES   0x01 

/** Local error; split multi-party operation failure.
*/
#define   MN_SPLIT_MPTY_REJ           0x02 

/** Local error; call hold operation failure.
*/
#define   MN_CALL_HOLD_REJ            0x03 

/** Local error; call retrieve operation failure.
*/
#define   MN_CALL_RETRIEVE_REJ        0x04 

/** Failure because network is not responding.
*/
#define   MN_NETWORK_NOT_RESPONDING   0x05 

/** @} */

/** @name Problem Tags
    @{ */

/** General problem tags.
*/
#define   GENERAL_PROBLEM              0x80 

/** Invoke problem tags.
*/
#define   INVOKE_PROBLEM               0x81 

/** Return result problem tags.
*/
#define   RETURN_RESULT_PROBLEM        0x82 

/** Return error problem tags.
*/
#define   RETURN_ERROR_PROBLEM         0x83 

/** Error code tag for an SS error returned from the network.
*/
#define   ERROR_CODE_TAG             0x02 

/** @} */

/** @cond
*/
/* Call progress indicator values.
*/
enum
{
   USER_SUSPENDED = 0, /**< &nbsp; */
   USER_RESUMED,       /**< &nbsp; */
   BEARER_CHANGED,     /**< &nbsp; */
   REMOTE_ALERT,       /**< &nbsp; */
   BEARER_RELEASED     /**< &nbsp; */
};
/** @endcond
*/

/* ***********************************************************************
*                                                                       *
*               Network SS Codes                                        *
*                                                                       *
*    Note:  Only aoci, aocc are used for now                            *
* ************************************************************************
*/

/** Supplementary Service codes.
*/
typedef enum {
       all_ss                  =   0x00, /**< All SS. */

       allLineIdentificationSS =   0x10, /**< All line identification. */

       clip                    =   0x11, /**< Calling line identification
                                              presentation service. */

       clir                    =   0x12, /**< Calling line identification
                                              restriction service. */
										  
       colp                    =   0x13, /**< Connected line identification
                                              presentation service. */

       colr                    =   0x14, /**< Connected line identification
                                              restriction service. */

       cnap                    =   0x19, /**< Call name identification
	                                          presentation. */

       mci                     =   0x1a, /**< Malicious call identify. */

       allForwardingSS         =   0x20, /**< All call forwarding. */

       cfu                     =   0x21, /**< Call forwarding, unconditional. */

       cd                      =   0x24, /**< Call deflection. */

       allCondForwardingSS     =   0x28, /**< All conditional call
                                              forwarding. */

       cfb                     =   0x29, /**< Call forwarding on mobile
                                              subscriber busy. */

       cfnry                   =   0x2a, /**< Call forwarding on no reply. */

       cfnrc                   =   0x2b, /**< Call forwarding on mobile
	                                          subscriber unreachable. */

       allCallOfferingSS       =   0x30, /**< All call offering sub-services. */

       ect                     =   0x31, /**< Call transfer. */

       mah                     =   0x32, /**< Mobile access hunting. */

       allCallCompletionSS     =   0x40, /**< All call completion. */

       cw                      =   0x41, /**< Call waiting. */

       hold                    =   0x42, /**< Call hold. */

       ccbs                    =   0x43, /**< Completion of call to busy
                                              subscribers .*/

       allMultiPartySS         =   0x50, /**< All multi-party services. */

       multiPTY                =   0x51, /**< Multi-party service. */

       allCommunityOfInterest_SS  = 0x60, /**< All community of interest
	                                           service. */

       cug                     =   0x61, /**< Closed user group. */

       allChargingSS           =   0x70, /**< All charging service. */

       aoci                    =   0x71, /**< Advice of charge information. */

       aocc                    =   0x72, /**< Advice of charge charge. */

       allAdditionalInfoTransferSS = 0x80, /**< All additional info
	                                            transfer service. */

       uus                     =   0x81, /**< User to user signaling. */

       allCallRestrictionSS    =   0x90, /**< All call restriction service. */

       barringOfOutgoingCalls  =   0x91, /**< Barring of outgoing calls
	                                          service. */

       baoc                    =   0x92, /**< Barring of all outgoing calls. */

       boic                    =   0x93, /**< Barring of outgoing international
                                              calls. */

       boicExHC                =   0x94, /**< Barring of outgoing international
                                              calls except those to home
                                              PLMN. */

       barringOfIncomingCalls  =   0x99, /**< Barring of incoming calls. */

       baic                    =   0x9a, /**< Barring of all incoming calls. */

       bicRoam                 =   0x9b, /**< Barring of incoming calls when
                                              roaming outside home PLMN
                                              country. */

       allPLMN_specificSS      =   0xf0, /**< All PLMN-specific supplementary
                                              services. */

       chargingInfoId          =   0xa1 /**< Charging information. */

} ss_operation_code_T;

/** Supplementary Services password values.
*/
typedef enum
{
   enterPW,              /**< &nbsp; */
   enterNewPW,           /**< &nbsp; */
   enterNewPW_Again,     /**< &nbsp; */
   badPW_TryAgain,       /**< &nbsp; */
   badPW_FormatTryAgain  /**< &nbsp; */
}ss_password_value;      /**< &nbsp; */

/** User-to-user signaling types.
*/
typedef enum
{
    UUS_DATA = 0,    /**< User-to-user signaling data. */
	UUS1_IMPLICIT,   /**< User-to-user signaling type 1. This is
	                    implicitly activated. */

    UUS1_REQUIRED,     /**< User-to-user signaling Type 1. This is
	                    explicitly activated.\ A UUS required indication
						is sent during an activation request which implies
						that the call is not completed if user-to-user
						information cannot be passed to a remote party. */

    UUS1_NOT_REQUIRED, /**< User-to-user signaling Type 1. This is explicitly
	                     activated.\ A UUS is not required indication is sent
						 during an activation request which implies that the
						 call will be complete even if user-to-user information
						 cannot be passed to the remote party. */

    UUS2_REQUIRED,     /**< User-to-user signaling Type 2. This is
	                    explicitly activated. A UUS required indication
						is sent during an activation request which implies
						that the call will not be completed if user-to-user
						information cannot be passed to a remote party. */

    UUS2_NOT_REQUIRED, /**< User-to-user signaling Type 2. This is explicitly
	                     activated. A UUS is not required indication is sent
						 during an activation request which implies that
						 the call will complete even is user-to-user information
						 cannot be passed to the remote party. */

    UUS3_REQUIRED,     /**< User-to-user signaling Type 3. This is
	                    explicitly activated. A UUS required indication
						is sent during an activation request which implies
						that the call will not be completed if user-to-user
						information cannot be passed to a remote party. */

    UUS3_NOT_REQUIRED, /**< User-to-user Signaling Type 3. This is explicitly
	                     activated. A UUS not required indication is sent
						 during an activation request which implies that
						 the call will complete even if user-to-user information
						 cannot be passed to the remote party. */

    /*This is a max value included to evade lint error. Do not assign this value*/
    UUS_MAX           /**< Maximum value. */
} uus_type_T;
/*~ SENTINEL uus_type_T.UUS_MAX */

/** @brief Holds user-to-user signaling information.
*/
typedef struct
{
    boolean        present;    
	/**< Whether user-to-user signaling information is present. */

    uus_type_T     user_user_type;                             
    /**< User-to-user signaling type. */
    byte           user_user_data_length;                      
	/**< User-to-user signaling data length. */
    byte           user_user_protocol_disc;                    
	/**< User-to-user signaling protocol discriminator.\ Gives
	the data encoding scheme for user-to-user data. */

    byte           user_user_data [MAX_USER_USER_DATA_LENGTH]; 
    /**< User-to-user signaling data. */

    boolean        more_data_indicator;                        
	/**< Indicates if more user-to-user data will follow. */
} cm_user_data_T;
/*~ FIELD cm_user_data_T.user_user_data VARRAY MAX_USER_USER_DATA_LENGTH
    LENGTH cm_user_data_T.user_user_data_length */
 
/** @brief Repeat indicator.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a repeat indicator is present. */
   
   byte                            repeat_indication; 
   /**< Repeat indicator value. */
   
} cm_repeat_indicator_T;

/** GSM speech versions.
*/
typedef enum gsm_speech_ver_e {
    GSM_FR_SPEECH_VER_1 = 0x00, 
	/**< GSM full-rate speech Version 1. */

    GSM_HR_SPEECH_VER_1 = 0x01,   /* aka HR  */
	/**< GSM half-rate speech Version 1. */

    GSM_FR_SPEECH_VER_2 = 0x02,   /* aka EFR  */
	/**< GSM full-rate speech Version 2. */

    GSM_FR_SPEECH_VER_3 = 0x04,   /* aka GSM AMR  */
	/**< GSM full-rate speech Version 3. */

    GSM_HR_SPEECH_VER_3 = 0x05,   /* Not used  */
	/**< GSM half-rate speech Version 3. */

    GSM_FR_SPEECH_VER_5 = 0x08,   /* FR AMR-WB */
	/**< GSM full-rate speech Version 5. */

    GSM_INVALID_SPEECH_VER = 0x0e   /* Not specified in 24.008 */
	/**< Invalid speech version. */

} gsm_speech_ver_e_type;

/** Cellular text telephone modem support.
*/
typedef enum ctm_e {
    CTM_NOT_SUPPORTED = 0x00,  /**< &nbsp; */
    CTM_SUPPORTED = 0x01       /**< &nbsp; */
} ctm_e_type;

/** @brief Bearer capability configuration values.
*/
typedef struct
{
   boolean                         present;                    
   /**< Whether bearer capability configuration information is present. */

   /* octet 3 */
   byte                            radio_channel_requirement;  
   /**< Radio channel requirement. */

   byte                            coding_standard;            
   /**< Coding standard. */

   byte                            transfer_mode;              
   /**< Transfer mode: circuit or packet. */

   byte                            information_transfer_capability; 
   /**< Information transfer capability. */

   /* We'll support only 3 speech versions: FR, EFR and AMR  */
   /* octet 3a */
   ctm_e_type                      ctm; 
   /**< Whether cellular text telephone modem is supported. */

   /* octet 3e */
   gsm_speech_ver_e_type           gsm_speech_ver_pref_0;      
   /**< First GSM speech version in the list of preferred GSM speech versions. */

   gsm_speech_ver_e_type           gsm_speech_ver_pref_1;      
   /**< Second GSM speech version in the list of preferred GSM speech versions. */

   /* octet 3b */
   gsm_speech_ver_e_type           gsm_speech_ver_pref_2;      
   /**< Third GSM speech version in the list of preferred GSM speech versions. */

   /* octet 3c */
   gsm_speech_ver_e_type           gsm_speech_ver_pref_3;      
   /**< Fourth GSM speech version in the list of preferred GSM speech versions. */

   /* octet 3d */
   gsm_speech_ver_e_type           gsm_speech_ver_pref_4;      
   /**< Fifth GSM speech version in the list of preferred GSM speech versions. */

   /* octet 3e */
   gsm_speech_ver_e_type           gsm_speech_ver_pref_5;      
   /**< Sixth GSM speech version in the list of preferred GSM speech versions. */

   /* octet 4 */
   byte                            compression;               
   /**< Indicates whether data compression is allowed. */

   byte                            structure;     
   /**< Structure indication: unstructured or service data unit integrity. */

   byte                            duplex_mode;               
   /**< Duplex mode indication: full duplex or half duplex. */

   byte                            configuration;             
   /**< Configuration indication. The default is point-to point. */

   byte                            NIRR;                   
   /**< Negotiation of Intermediate Rate Request. */

   byte                            establishment;              
   /**< Establishment indication. The default is demand. */


   /* octet 5 */
   /* 0 as per 24.008 V3.6.0 */
   byte                            access_id;                  
   /**< Access identity. */

   byte                            rate_adaption;              
   /**< Rate adaptation. */

   byte                            signalling_access_protocol;  
   /**< Signaling access protocol used. */

   /* octet 5a */                    
   byte                            other_ITC;                  
   /**< Other information transfer capability. */

   byte                            other_rate_adaption;        
   /**< Other rate adaptation. */

   /* octet 5b */
   byte                            rate_adaption_header;                   
   /**< Whether a rate adaptation header is present or not. */

   byte                            multiple_frame_establishment;          
   /**< Multiple frame establishment support in the data link. */

   byte                            mode_of_operation;
   /**< Mode of operation: bit transparent or protocol sensitive. */

   byte                            logical_link_identifier_negotiation;   
   /**< Logical link identifier negotiation. */

   byte                            assignor_assignee;
   /**< Message originator is assignee or assigner. */

   byte                            inband_outband_negotiation;            
   /**< In-band or out of band negotiation. */

   /* octet 6 */
   /* 1 as per 24.008 V3.6.0 */
   byte                            layer_1_id;       
   /**< Layer 1 identifier. */
                       
   byte                            user_information_layer_1_protocol;    
   /**< User information Layer 1 protocol. */

   byte                            sync_async;                           
   /**< Synchronous or asynchronous. */

   /* octet 6a */
   byte                            number_of_stop_bits;                 
   /**< Number of stop bits. Can be 1 bit or 2 bits. */

   byte                            negotiation;                         
   /**< Bearer capability negotiation procedure. */

   byte                            number_of_data_bits;                 
   /**< Number of data bits. */

   byte                            user_rate;                           
   /**< User rate. */

   /* octet 6b */
   byte                            intermediate_rate;
   /**< Rate adaptation intermediate rate. */
                     
   byte                            NIC_on_Tx;                          
   /**< Network-independent clock on transmission. */

   byte                            NIC_on_Rx;                          
   /**< Network-independent clock on reception. */

   byte                            parity;                             
   /**< Parity information. */

   /* octet 6c */
   byte                            connection_element;                
   /**< Connection element. */

   byte                            modem_type;                        
   /**< Modem type. */

   /* These are used for Phase2+ (i.e.14400bd) */
   /* octet 6d */
   byte                            other_modem_type;                  
   /**< Other modem type. */

   byte                            fixed_network_user_rate;           
   /**< Fixed network user rate. */

   /* octet 6e */
   byte                            acceptable_channel_codings;         
   /**< Acceptable channel codings. */

   byte                            maximum_number_of_trafic_channels;  
   /**< Maximum number of traffic channels. */

   /* octet 6f */
   byte                            user_initiated_modification_indication;  
   /**< User-initiated modification indication. */

   byte                            wanted_air_interface_user_rate;          
   /**< Wanted air interface user rate. */

   /* octet 6g */
   byte                            acceptable_channel_coding_extended;   
   /**< Multiple establishment support in the data link. */

   /* EDGE channel coding */
   byte                            asymmetry_indication;                
   /**< Channel coding asymmetry indication. */

   /* octet 7 */
   /* 2 as per 24.008 V3.6.0 */
   byte                            layer_2_id;       
   /**< Layer 2 identifier. */
                     
   byte                            user_information_layer_2_protocol;  
   /**< User information Layer 2 protocol. */

   /* Used only for logging!! */
   byte                            mt_fall_back_rate;
   /**< Fallback rate. */
   
} cm_bearer_capability_T;

/** @brief Low layer compatibility.
*/
typedef struct
{
   boolean                         present;
   /**< Whether low layer compatibility is present. */
   byte                            length;
   /**< Length. */
   byte                            data[LL_COMPATIBILITY_DATA_SIZE];
   /**< Data. */

} cm_low_layer_compatibility_T;

/*~ FIELD cm_low_layer_compatibility_T.data VARRAY LL_COMPATIBILITY_DATA_SIZE
    LENGTH cm_low_layer_compatibility_T.length */
 

/** @brief High layer compatibility.
*/
typedef struct
{
   boolean                         present;    
   /**< Whether high layer capability is present. */

   boolean                         not_applicable;                       
   /**< Whether high layer compatibility data is present. */

   byte                            coding_standard;                     
   /**< Coding standard. */

   byte                            interpretation;                      
   /**< How the high layer characteristic identification in octet 4 should
        be interpreted. */

   byte                            presentation_method;                 
   /**< Presentation method of the protocol profile. */

   byte                            high_layer_characteristic_id;        
   /**< Higher layer characteristic identification. */

   boolean                         ehlcid_present;                      
   /**< Whether the extended higher layer characteristic identification
        is present. */

   byte                            extended_high_layer_characteristic_id;  
   /**< Extended higher layer characteristic identification. */

} cm_high_layer_compatibility_T;

/** @brief Called party Binary Coded Decimal number.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a called party BCD number is present. */
   byte                            length;
   /**< Length of the number. */
   byte                            data[CM_CALLED_PARTY_BCD_NO_LENGTH];   
   /**< Called party BCD number. */

} cm_called_party_bcd_no_T;

 
/*~ FIELD cm_called_party_bcd_no_T.data VARRAY CM_CALLED_PARTY_BCD_NO_LENGTH
    LENGTH cm_called_party_bcd_no_T.length */
 

/** @brief Called party subaddress.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a called party subaddress is present.  */
   byte                            length;
   /**< Length of the called party subaddress. */
   byte                            data[CM_CALLED_PARTY_SUBADDRESS_LENGTH];
   /**< Called party subaddress value. */

} cm_called_party_subaddress_T;

 
/*~ FIELD cm_called_party_subaddress_T.data VARRAY CM_CALLED_PARTY_SUBADDRESS_LENGTH
    LENGTH cm_called_party_subaddress_T.length */
 

/** @brief Calling party Binary Coded Decimal number.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a calling party BCD number is present.  */
   byte                            length;
   /**< Length of the calling party BCD number. */
   byte                            data[CM_CALLING_PARTY_BCD_NO_LENGTH];
   /**< Calling party BCD number. */

} cm_calling_party_bcd_no_T;

 
/*~ FIELD cm_calling_party_bcd_no_T.data VARRAY CM_CALLING_PARTY_BCD_NO_LENGTH
    LENGTH cm_calling_party_bcd_no_T.length */
 

/** @brief Calling party subaddress.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a calling party subaddress is present.  */
   byte                            length;
   /**< Length of the calling party subaddress.  */
   byte                            data[CM_CALLING_PARTY_SUBADDRESS_LENGTH];  
   /**< Calling party subaddress value.  */

} cm_calling_party_subaddress_T;

 
/*~ FIELD cm_calling_party_subaddress_T.data VARRAY CM_CALLING_PARTY_SUBADDRESS_LENGTH
     LENGTH cm_calling_party_subaddress_T.length */
 
/** @brief Closed User Group index.
*/
typedef struct
{
   boolean                         present;        
   /**< Whether a CUG index is present. */
   byte                            msb;        
   /**< CUG index value; most significant byte. */
   byte                            lsb;            
   /**< CUG index value; least significant byte. */

} ie_cug_index_T;

/** @brief Closed User Group information.
*/
typedef struct
{
   boolean                         present;  
   /**< Whether CUG information is present. */

   ie_cug_index_T                  cm_cug_index;     
   /**< CUG index. */

   boolean                         cm_suppress_pref_cug;          
   /**< Suppress the preferential CUG indicator. */

   boolean                         cm_suppress_outgoing_access; 
   /**< Suppress the outgoing access indicator. */

} cm_forward_cug_info_T;

/** @brief Call control cause information.
*/
typedef struct
{
   boolean                         present;                     
   /**< Whether the cause information is present. */

   byte                            coding_standard;                  
   /**< Coding standard. */

   byte                            location;                    
   /**< Location. */

   boolean                         recommendation_present;      
   /**< Whether a recommendation is present. */

   byte                            recommendation;              
   /**< Recommendation value. */

   byte                            cause_value;                  
   /**< Cause value. */

   byte                            diagnostic_length;           
   /**< Diagnostics length. */

   byte                            diagnostics[MAX_DIAGNOSTICS_LENGTH];  
   /**< Diagnostics value. */

} ie_cm_cc_cause_T;

 
/*~ FIELD ie_cm_cc_cause_T.diagnostics VARRAY MAX_DIAGNOSTICS_LENGTH
    LENGTH ie_cm_cc_cause_T.diagnostic_length */
 
/** @brief Call information.
*/
typedef struct
{
   boolean                        present;
   /**< Whether call information is present. */
   cm_repeat_indicator_T          bc_repeat_indicator;
   /**< Bearer capability repeat indicator. */
   cm_bearer_capability_T         bearer_capability_1;  
   /**< First bearer capability. */
   cm_bearer_capability_T         bearer_capability_2;  
   /**< Second bearer capability. */

} ie_cm_mo_call_details_T;

/** @brief Call barred Supplementary Services error.
*/
typedef struct
{
   boolean                         present;
   /**< Whether the call barred SUPS error is present. */
   byte                            call_barring_cause; 
   /**< Cause of the error. */

} ie_cm_called_barred_error;

/** @brief Calling Line Identification restriction information.
*/
typedef struct
{
   boolean                         present;  
   /**< Whether the CLIR information is present. */
   byte                            option;
   /**< CLIR option. */

} cm_cli_restriction_option_T;

/** @brief Calling Line Identification override category.
*/
typedef struct
{
   boolean                         present;
   /**< Whether the calling line ID override category is present. */
   byte                            value;
   /**< Calling line ID override category. */

} ie_override_category_T;

/** @brief Calling Line Identification option type.
*/
typedef enum
{
  CLI_DEFAULT_OPTION,  /**< &nbsp; */
  CLI_RESTRICT_OPTION, /**< &nbsp; */
  CLI_OVERRIDE_OPTION, /**< &nbsp; */
  CLI_UNKNOWN_OPTION   /**< &nbsp; */
} cli_option_type;

/** @brief Accesses the Calling Line Identification option.
*/
typedef struct
{
  cli_option_type                  option_type;
  /**< Calling Line ID option type. */
  union
  {
    cm_cli_restriction_option_T     clir_restriction_option;
    /**< CLIR restriction option. */
    ie_override_category_T          override_category;
    /**< Override category. */
  } option;

} ie_ss_subscription_option_T;

 
/*~ FIELD ie_ss_subscription_option_T.option DISC
          ie_ss_subscription_option_T.option_type */
/*~ IF ( _DISC_ == CLI_DEFAULT_OPTION || _DISC_ == CLI_RESTRICT_OPTION )
    ie_ss_subscription_option_T.option.clir_restriction_option */
/*~ CASE CLI_OVERRIDE_OPTION
    ie_ss_subscription_option_T.option.override_category */
/*~ CASE CLI_UNKNOWN_OPTION
    ie_ss_subscription_option_T.option.void */
 

/** @brief Extended container for a Supplementary Services subscription
     violation.
*/
typedef struct 
{
   boolean                         present;
   /**< Whether the container is present. */
   byte                            size;
   /**< Size of the container. */
   byte                            data[CM_SUBS_VIOL_EXT_CONTAINER_LENGTH];
   /**< Contents of the container. */
} ie_ss_subscription_violation_ext_cont_T;

 
/*~ FIELD ie_ss_subscription_violation_ext_cont_T.data VARRAY CM_SUBS_VIOL_EXT_CONTAINER_LENGTH
    LENGTH ie_ss_subscription_violation_ext_cont_T.size */
 
/** @brief Information about a Supplementary Services subscription violation
    error.
*/
typedef  struct
{
   boolean                         present;
   /**< Whether the information is present. */
   ie_ss_subscription_option_T     ss_subscription_option;
   /**< Subscription option. */
   ie_ss_subscription_violation_ext_cont_T extension_container;
   /**< Subscription violation extension container. */
} ie_cm_subscription_violation;

/** @brief Supplementary Services code.
*/
typedef struct
{
   boolean                       present;
   /**< Whether the code is present. */
   byte                          ss_code;
   /**< Supplementary Services code. */

} ie_ss_code_T;


/** @brief Basic service group code type.
*/
typedef enum
{
  NO_CODE = -1,   /**< &nbsp; */
  BS_CODE,        /**< Bearer Service type. */
  TS_CODE,        /**< Telephony Service. */
  MAX_CODE        /**< &nbsp; */

} bsg_code_type;
/*~ SENTINEL bsg_code_type.MAX_CODE */

/** @brief Basic service information.
*/
typedef struct
{
  boolean         present;   /**< Whether the basic service information is
							      present. */
  bsg_code_type   code_type; /**< Basic service code type. */
  byte            bs_code;   /**< Basic service code. */

} basic_service_T;

/** @brief Supplementary Services status information.
*/
typedef struct
{
   boolean                         present;
   /**< Whether the status is present. */

   boolean                         cm_ss_operative;
   /**< Supplementary Services is operative. */

   boolean                         cm_ss_provisioned;
   /**< Supplementary Services is provisioned. */

   boolean                         cm_ss_registered;
   /**< Supplementary Services is registered. */

   boolean                         cm_ss_active;
   /**< Supplementary Services is active. */

} ie_ss_status_T;

/** @brief Supplementary Services error; SS incompatibility.
*/
typedef struct
{
   boolean                         present;
   /**< Whether the error information is present. */

   ie_ss_code_T                    ss_code;
   /**< Supplementary Service for which the error is returned. */

   basic_service_T                 basic_service;
   /**< Basic service information. */

   ie_ss_status_T                  ss_status;
   /**< Supplementary Service status. */

} ie_ss_incompatability_error;

/** @brief Supplementary Services error; absent subscriber.
*/
typedef struct
{       
   boolean                         present;
   /**< Whether the error information is present. */
   boolean                         mwd_set;
   /**< Whether Message Waiting Data is set. */

} ie_absent_subscriber_error;

/** @brief Supplementary Services error; system failure.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a system failure error is present. */

   byte                            network_resource;
   /**< Part of the network that led to the failure. */

} ie_system_failure_error;

/** @brief Supplementary Services error; password registration failure.
*/
typedef struct
{
   boolean                         present;
   /**< Whether the error is present. */
   byte                            pw_registration_failure_cause;
   /**< Cause of the password registration failure. */

} ie_pw_registration_failure_error;


/** @brief Supplementary Services error; position method failure.
*/
typedef struct
{
   boolean                         present;
   /**< Whether the error information is present. */
   byte                            position_method_failure_diagnostic;
   /**< Position method failure diagnostics. */
   
} ie_position_method_failure_error;

/** Identifies the Supplementary Services error type.
*/
typedef enum
{
  NO_ERR,                      /**< &nbsp; */
  CALL_BARRED_ERR,             /**< &nbsp; */
  SUBSCRIPTION_VIOLATION_ERR,  /**< &nbsp; */
  SS_INCOMPATIBILITY_ERR,      /**< &nbsp; */
  ABSENT_SUBSCRIBER_ERR,       /**< &nbsp; */
  SYSTEM_FAILURE_ERR,          /**< &nbsp; */
  PW_REG_FAILURE_ERR,          /**< &nbsp; */
  POSITION_METHOD_FAILURE_ERR, /**< &nbsp; */
  UNKNOWN_ERR                  /**< &nbsp; */
} ss_error_type;

/** @brief Accesses the Supplementary Services error.
*/
typedef struct
{
   boolean                             present;
   /**< Whether the error is present. */
   byte                                error_code_tag;
   /**< Error code tag. */
   byte                                error_code;
   /**< Error code. */
   ss_error_type                       error_type;
   /**< Type of error.  */
   union
   {
      ie_cm_called_barred_error        cm_called_barred_error;
      /**< Call barred error. */
      ie_cm_subscription_violation     cm_subscription_violation;
      /**< Subscription violation error. */
      ie_ss_incompatability_error      cm_ss_incompatability_error;
      /**< Supplementary Services incompatibility error. */
      ie_absent_subscriber_error       cm_absent_subscriber_error;
      /**< Absent subscriber error. */
      ie_system_failure_error          cm_system_failure_error;
      /**< System failure error. */
      ie_pw_registration_failure_error cm_pw_registration_failure_error;
      /**< Password registration failure error. */
    ie_position_method_failure_error cm_position_method_failure_error;
      /**< Position method failure error. */
   } parameter;

} ie_cm_ss_error_T;

/*~ FIELD ie_cm_ss_error_T.parameter DISC ie_cm_ss_error_T.error_type */
/*~ CASE NO_ERR ie_cm_ss_error_T.parameter.void */
/*~ CASE UNKNOWN_ERR ie_cm_ss_error_T.parameter.void */
/*~ CASE CALL_BARRED_ERR ie_cm_ss_error_T.parameter.cm_called_barred_error */
/*~ CASE SUBSCRIPTION_VIOLATION_ERR ie_cm_ss_error_T.parameter.cm_subscription_violation */
/*~ CASE SS_INCOMPATIBILITY_ERR ie_cm_ss_error_T.parameter.cm_ss_incompatability_error */
/*~ CASE ABSENT_SUBSCRIBER_ERR ie_cm_ss_error_T.parameter.cm_absent_subscriber_error */
/*~ CASE SYSTEM_FAILURE_ERR ie_cm_ss_error_T.parameter.cm_system_failure_error */
/*~ CASE PW_REG_FAILURE_ERR ie_cm_ss_error_T.parameter.cm_pw_registration_failure_error */
/*~ CASE POSITION_METHOD_FAILURE_ERR ie_cm_ss_error_T.parameter.cm_position_method_failure_error */
/*~ DEFAULT ie_cm_ss_error_T.parameter.void */


/** @brief List of active calls.
*/
typedef struct
{
   byte size;
   /**< Number of active calls. */
   byte table[ACTIVE_CALLS_LIST_SIZE];
   /**< Active calls list. */

} active_calls_list_T;

 
/*~ FIELD active_calls_list_T.table VARRAY ACTIVE_CALLS_LIST_SIZE
    LENGTH active_calls_list_T.size */
 

/** @brief Connected number.
*/
typedef  struct
{
   boolean                         present;
   /**< Whether a connected number is present. */
   byte                            length;
   /**< Length of the connected number. */
   byte                            data[CM_CONNECTED_NO_LENGTH];
   /**< Connected number. */

} cm_connected_no_T;
/*~ FIELD cm_connected_no_T.data VARRAY CM_CONNECTED_NO_LENGTH
    LENGTH cm_connected_no_T.length */
 

/** @brief Connected subaddress.
*/
typedef  struct
{
   boolean                         present;
   /**< Whether a connected subaddress is present. */
   byte                            length;
   /**< Length of the connected subaddress. */
   byte                            data[CM_CONNECTED_SUBADDRESS_LENGTH];
   /**< Connected subaddress. */

} cm_connected_subaddress_T;
/*~ FIELD cm_connected_subaddress_T.data VARRAY CM_CONNECTED_SUBADDRESS_LENGTH
    LENGTH cm_connected_subaddress_T.length */

/** @brief Call forwarding notification.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a call forwarding notification is present. */
   byte                            forwarding_reason;
   /**< Call forwarding reason. */
   boolean                         notification_to_fp;
   /**< Whether to send notification to the Forwarding Party. */
   boolean                         notification_to_cp;
   /**< Whether to send notification to the Calling Party. */

} ie_forwarding_options_T;

/** @brief No reply condition timer.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a no reply condition timer is present. */
   byte                            timer_value;
   /**< No reply condition timer value. */

} ie_no_reply_condition_timer_T;

/** @brief Call forwarding feature.
*/
typedef struct
{
   basic_service_T                 basic_service;
   /**< Basic service information. */
   ie_ss_status_T                  ss_status;
   /**< Call forwarding status. */
   cm_called_party_bcd_no_T        forwarded_to_number[2*MAXNO_FORWARD_TO_NUMBER];
   /**< Number to which the call is forwarded. */ /* x2 = overload long forward number into same structure */
   ie_forwarding_options_T         forwarding_options;
   /**< Call forwarding options. */
   ie_no_reply_condition_timer_T   no_reply_condition_timer;
   /**< No reply condition timer. */
   cm_called_party_subaddress_T    forwarded_to_subaddress;
   /**< Subaddress to which the call is forwarded. */
} forwarding_feature_T;

/** @brief List of call forwarding features.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a list of call forwarding features is present. */
   byte                            forwarding_feature_list_length;
   /**< Length of the call forwarding feature list. */
   forwarding_feature_T            forwarding_feature[FWD_FEATURE_LIST_SIZE];
   /**< Forwarding feature information. */

} ie_forwarding_feature_list_T;
/*~ FIELD ie_forwarding_feature_list_T.forwarding_feature VARRAY FWD_FEATURE_LIST_SIZE
    LENGTH ie_forwarding_feature_list_T.forwarding_feature_list_length */
 
/** @brief Call forwarding information.
*/
typedef struct
{
   boolean                         present;
   /**< Whether call forwarding information is present. */
   ie_ss_code_T                    cm_ss_code;
   /**< Supplementary Services code. */
   ie_forwarding_feature_list_T    forwarding_feature_list;
   /**< Forwarding feature list. */

} ie_forwarding_info_T;

/** @brief Call barring feature.
*/
typedef struct
{
   basic_service_T                 basic_service;
   /**< Basic service information. */
   ie_ss_status_T                  cm_ss_status;
   /**< Supplementary Services status. */

} call_barring_feature_T;

/** @brief Call barring information.
*/
typedef struct
{
   boolean                         present;
   /**< Whether call barring information is present. */
   byte                            call_barring_info_length;
   /**< Length of the call barring information. */
   ie_ss_code_T                    cm_ss_code;
   /**< Supplementary Services code. */
   call_barring_feature_T          call_barring_feature_list[CALL_BARRING_LIST_SIZE];
   /**< Call barring feature list. */

} ie_call_barring_info_T;

/** @brief Closed User Group restrictions.
*/
typedef struct
{
   boolean                         present;
   /**< Whether CUG restrictions are present. */
   byte                            restrictions;
   /**< CUG restrictions. */

} ie_inter_cug_restrictions_T;

/** @brief Preferential Closed User Group indicator.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a preferred CUG indicator is present. */
   byte                            cug_indicator[CUG_INDICATOR_SIZE];
   /**< CUG indicator. */

} preferential_cug_indicator_T;

/** @brief Closed User Group feature.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a CUG feature is present. */
   basic_service_T                 basic_service;
   /**< Basic service information. */
   preferential_cug_indicator_T    preferential_cug_indicator;
   /**< Preferential CUG indicator. */
   ie_inter_cug_restrictions_T     inter_cug_restrictions;
   /**< Inter-CUG restrictions. */

} ie_cug_feature_T;

/** Closed User Group options.
*/
typedef byte    intra_cug_options_T;

/** @brief List of basic service groups.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a list of basic service groups is present. */
   byte                            list_length;
   /**< Length of the basic service groups list. */
   basic_service_T                 list[BASIC_SERV_GROUP_LIST_SIZE];
   /**< Basic service group list. */

} basic_service_group_list_T;

 
/*~ FIELD basic_service_group_list_T.list VARRAY BASIC_SERV_GROUP_LIST_SIZE
    LENGTH basic_service_group_list_T.list_length */
 
/** @brief Closed User Group subscription.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a CUG subscription is present. */
   byte                            cug_index[CUG_INDICATOR_SIZE];
   /**< CUG index. */
   byte                            cug_interlock[CUG_INTERLOCK_SIZE];
   /**< CUG interlock. */
   intra_cug_options_T             intra_cug_options;
   /**< Intra-CUG options. */
   basic_service_group_list_T      basic_service_group_list;
   /**< Basic service group list. */

} ie_cug_subscription_T;
                
/** @brief Closed User Group information.
*/
typedef struct
{
   boolean                         present;
   /**< Whether CUG information is present. */
   byte                            cug_subscription_list_length;
   /**< Length of the CUG subscription list. */
   ie_cug_subscription_T           cug_subscription_list[CUG_SUBSCRIPTION_LIST_SIZE];
   /**< CUG subscription list. */
   byte                            cug_feature_list_length;
   /**< Length of the CUG feature list. */
   ie_cug_feature_T                cug_feature_list[CUG_FEATURE_LIST_SIZE];
   /**< CUG feature list. */

} ie_cug_info_T;

 
/*~ FIELD ie_cug_info_T.cug_subscription_list VARRAY CUG_SUBSCRIPTION_LIST_SIZE
    LENGTH ie_cug_info_T.cug_subscription_list_length */
/*~ FIELD ie_cug_info_T.cug_feature_list VARRAY CUG_FEATURE_LIST_SIZE
    LENGTH ie_cug_info_T.cug_feature_list_length */
 

/** @brief Calling Line Identification Restriction information.
*/
typedef struct
{
   boolean                         present;
   /**< Whether CLIR information is present. */
   ie_ss_status_T                  cm_ss_status;
   /**< CLIR status. */
   cm_cli_restriction_option_T     cm_cli_restriction_option;
   /**< CLIR option.*/

} cli_restriction_info_T;


/** @brief Supplementary Services password.
*/
typedef struct
{
   boolean                       present;
   /**< Whether a Supplementary Services password is present. */
   char                          ss_password[MAX_PWD_CHAR];
   /**< Supplementary Services password. */

} ie_ss_password_T;

/* for phase 2 uss messages */

/** @brief Unstructured Supplementary Services data.
*/
typedef struct
{
   boolean                         present;
   /**< Whether USS data is present. */
   byte                            uss_data_coding_scheme;
   /**< USS data coding scheme. */
   uint8                           orig_dcs_from_sim;
   /**< Orig DCS from SIM */
   byte                            size;
   /**< Length of the USS data. */
   byte                            ussData[MAX_USS_CHAR];
   /**< USS data. */

} uss_data_T;

/*~ FIELD uss_data_T.ussData VARRAY MAX_USS_CHAR LENGTH uss_data_T.size */
 

/** @brief Supplementary Services data.
*/
typedef struct
{
   boolean                         present;
   /**< Whether Supplementary Services data is present. */
   byte                            length;
   /**< Length of the Supplementary Services data. */
   ie_ss_code_T                    cm_ss_code;
   /**< Supplementary Services code. */
   ie_ss_status_T                  cm_ss_status;
   /**< Supplementary Services status. */
   ie_ss_subscription_option_T     ss_subscription_option;
   /**< Supplementary Services subscription option. */
   basic_service_group_list_T      basic_service_group_list;
   /**< Basic service group list. */

} ie_cm_ss_data_T;

/** @brief User data for the USSD.
*/
typedef struct
{
   boolean                         present;
   /**< Whether user data is present. */
   byte                            size;
   /**< Length of the user data. */
   char                            ss_UserData[MAX_SS_USER_DATA_CHAR];
   /**< User data. */

} ss_User_Data_T;
/* To hold data for cm_unstructured_ss_res */

/** Supplementary Service type.
*/
typedef enum
{
  FWD_INFO = 1,           /**< &nbsp; */
  FWD_TO_NUM_INFO,        /**< &nbsp; */
  FWD_FEATURE_LIST_INFO,  /**< &nbsp; */
  CALL_BARRING_INFO,      /**< &nbsp; */
  CLI_RESTRICTION_INFO,   /**< &nbsp; */
  CALLED_PARTY_NUM_INFO,  /**< &nbsp; */
  CUG_INFO,               /**< &nbsp; */
  BSG_LIST_INFO,          /**< &nbsp; */
  USS_DATA_INFO,          /**< &nbsp; */
  USSD_DATA_INFO,         /**< &nbsp; */
  NEW_PWD_INFO,           /**< &nbsp; */
  LCS_MOLR_INFO,          /**< &nbsp; */
  SS_DATA_INFO,           /**< &nbsp; */
  SS_STATUS_INFO,         /**< &nbsp; */
  SS_ERROR_INFO,          /**< &nbsp; */
  CC_CAUSE_INFO,          /**< &nbsp; */
  LL_CAUSE_INFO,          /**< &nbsp; */
  UNKNOWN_CONF_INFO,      /**< &nbsp; */
  CCBS_INFO,              /**< &nbsp; */
  NO_INFO                 /**< &nbsp; */
} cm_ss_conf_e_type;

/** @brief Unstructured Supplementary Services Data response from the network.
*/
typedef struct
{
  cm_ss_conf_e_type                 data_type;
  /**< Type of data received from the network. */
  union
  {
    uss_data_T                 uss_data;
    /**< USS data. */
    ie_cm_ss_error_T           cm_ss_error;
    /**< Error. */
  } data;
} uss_res_data_T;
/*~ FIELD uss_res_data_T.data DISC uss_res_data_T.data_type */
/*~ CASE CM_USS_RES_SS_DATA  uss_res_data_T.data.uss_data */
/*~ CASE CM_USS_RES_SS_ERROR  uss_res_data_T.data.cm_ss_error */
 

/** @brief Cause of a no calling line identification.
*/
typedef struct
{
    boolean                        present;
    /**< Whether a cause for no calling line identification is present. */
    byte                           cause_value;
    /**< Cause value. */
} cm_cause_of_no_CLI_T;

/** Routing destination number option.
*/
typedef enum
{ 
    presentationAllowedAddress,    /**< &nbsp; */
    presentationRestrictedECT,     /**< &nbsp; */
    numberNotAvailable,            /**< &nbsp; */
    presentationRestricteddAdress  /**< &nbsp; */

}  routing_number_option_T;  

/** Explicit call transfer states.
*/
typedef enum 
{
    alerting_ECT, /**< &nbsp; */
    active_ECT    /**< &nbsp; */

} ect_call_state_T;

/** @brief Supplementary Services notification.
*/
typedef struct
{
   boolean                         present;
   /**< Whether a Supplementary Services notification is present. */
   byte                            notification;
   /**< Notification value. */

} ie_ss_notification_T;

/** Classifies the rejection type based on the rejecting layer.
*/
typedef enum
{
   INVALID_REJECT_CAUSE = 0x00, /**< Invalid value. */
   OTA_REJECT_CAUSE,            /**< Over-The-Air rejection. */
   AS_REJECT_CAUSE,             /**< Access Stratum rejection. */
   MM_REJECT_CAUSE,             /**< Mobility Management rejection. */
   CNM_MN_REJECT_CAUSE,         /**< Mobile Network or Connection Manager
                                     rejection. */
   EMM_REJECT_CAUSE,            /**< EPS Connection Management (ECM) rejection. */
   ONEX_CSFB_REJECT_CAUSE       /**< Circuit-Switched Fallback (CSFB)
                                     rejection. */

}reject_type_enum_T;

/** Rejection cause value.
*/
typedef byte                      reject_cause_T;

/** @brief Call rejection information.
*/
typedef struct
{
   boolean                         present;
   /**< Whether rejection information is present. */
   reject_type_enum_T              rej_type;
   /**< Rejection type. */
   reject_cause_T                  rej_value;
   /**< Rejection value. */
} ie_cm_cc_reject_T;

/** Location method type.
*/
typedef enum
{
  MS_BASED_EOTD,    /**< &nbsp; */
  MS_ASSISTED_EOTD, /**< &nbsp; */
  ASSISTED_GPS      /**< &nbsp; */
} location_method_e_type;

/** @brief Location method information.
*/
typedef struct
{
   byte present;
   /**< Whether location method information is present. */
   location_method_e_type data;
   /**< Location method data. */
} location_method_T;

/** Response type.
*/
typedef enum
{
   DELAY_LOW,       /**< &nbsp; */
   DELAY_TOLERANT   /**< &nbsp; */
} response_time_category_e_type;

/** @brief Response time information.
*/
typedef struct
{
   byte present;
   /**< Whether response time category information is present . */
   response_time_category_e_type response_time_category;
   /**< Response time category . */
} response_time_T;

/** @brief Horizontal accuracy.
*/
typedef struct
{
   byte present;
   /**< Whether horizontal accuracy data is present. */
   byte data;
   /**< Horizontal accuracy data . */
} horiz_accuracy_T;

/** @brief Vertical coordinates.
*/
typedef struct
{
   byte present;
   /**< Whether vertical coordinates are present. */
} vert_coord_req_T;

/** @brief Vertical accuracy.
*/
typedef struct
{
   byte present; /**< Whether vertical accuracy data is present. */
   byte data;    /**< Vertical accuracy data. */
} vert_accuracy_T;

/** @brief Location Services quality of service.
*/
typedef struct
{
   byte present;
   /**< Whether location service quality of service information is present. */
   horiz_accuracy_T horiz_accuracy;
   /**< Horizontal accuracy information. */
   vert_coord_req_T vertical_coord_req;
   /**< Vertical coordinates. */
   vert_accuracy_T  vertical_accuracy;
   /**< Vertical accuracy information. */
   response_time_T  response_time;
   /**< Response time. */
} lcs_qos_T;

/** @brief Mobile Location Channel number.
*/
typedef struct
{
   byte present;
   /**< Whether a mobile location channel number is present. */
   byte size;
   /**< Length of the mobile location channel number. */
   byte data[9];
   /**< Mobile location channel number value. */
} mlc_number_T;

/** @brief External address.
*/
typedef struct
{
  byte present;
  /**< Whether external address data is present. */
  byte data[20];
  /**< External address data. */
  byte size;
  /**< External address data length. */
} external_address_T;

/** @brief Location Services client information.
*/
typedef struct
{
  byte present;
  /**< Whether location services client information is present. */
  external_address_T external_address;
  /**< External address data information. */
} lcs_client_external_id_T;

/** @brief GPS assistance data.
*/
typedef struct
{
   byte present;
   /**< Whether GPS assistance data is present. */
   byte data[38];
   /**< GPS assistance data. */
   byte size;
   /**< GPS assistance data length. */
} gps_assistance_data_T;

/** @brief Supported GAD shapes.
*/
typedef struct
{
   byte present;
   /**< Whether supported GAD shapes are present. */
   byte data;
   /**< Supported GAD shapes. */
} supported_gad_shapes_T;

/** Location Services notification response type.
*/
typedef enum
{
  PERMISSION_DENIED,  /**< &nbsp; */
  PERMISSION_GRANTED  /**< &nbsp; */
} location_notification_res_e_type;

/** @brief LCS location notification response.
*/
typedef struct
{
   boolean                           present;
   /**< Whether an LCS location notification response is present. */
   location_notification_res_e_type  location_notification_res;
   /**< LCS location notification response data. */
} ie_ss_location_notification_res_T;

/** Location notification type.
*/
typedef enum
{
   NOTIFY_LOCATION_ALLOWED,                          /**< &nbsp; */
   NOTIFY_AND_VERIFY_LCS_ALLOWED_IF_NO_RESPONSE,     /**< &nbsp; */
   NOTIFY_AND_VERIFY_LCS_NOT_ALLOWED_IF_NO_RESPONSE, /**< &nbsp; */
   LOCATION_NOT_ALLOWED,                             /**< &nbsp; */
   NOTIFY_LOCATION_ABORTED                           /**< &nbsp; */
} notification_type_e_type;

/** Location type.
*/
typedef enum
{
   CURRENT_LOCATION,                 /**< &nbsp; */
   CURRENT_OR_LAST_KNOWN_LOCATION,   /**< &nbsp; */
   INITIAL_LOCATION,                 /**< &nbsp; */
   ACTIVATION_DEFERRED_LOCATION,     /**< &nbsp; */
   CANCEL_DEFERRED_LOCATION          /**< &nbsp; */
} location_type_e_type;

/** @brief Location Services deferred.
*/
typedef struct
{
   boolean  present;
   /**< Whether deferred location information is present. */
   byte     unused_bits;
   /**< Unused bits. */
   byte     ms_available;
   /**< Mobile Station available information. */
} lcs_deferred_location_type;

/** @brief Name string.
*/
typedef struct
{
  byte present;
  /**< Whether a name string is present. */
  byte size;
  /**< Length of the name string. */
  byte data[63];
  /**< String value. */
} namestring_T;

/** @brief Location Services client name.
*/
typedef struct
{
   byte         present;
   /**< Whether a Location Services client name is present. */
   byte         data_coding_scheme;
   /**< Data coding string for the name. */
   namestring_T namestring;
   /**< Name value. */
} lcs_client_name_T;

/** @brief Location Services requester identification.
*/
typedef struct 
{
   byte         present;
   /**< Whether a location services requester ID is present. */
   byte         data_coding_scheme;
   /**< Data coding scheme for the requester ID value. */
   namestring_T requestorIDString;  
   /**< Requester ID value. */
} lcs_requestor_id_T;

/** @brief Location Services codeword string.
*/
typedef struct 
{
   byte         present;
   /**< Whether an LCS code word string is present. */
   byte         data_coding_scheme;
   /**< Data coding scheme for the codeword value. */
   namestring_T lcsCodewordString; 
   /**< Code word value. */
} lcs_codeword_string_T;

/** @brief Location Services type identification.
*/
typedef struct
{
   byte         present;
   /**< Whether an LCS type ID is present. */
   byte         lcsServiceTypeID;
   /**< LCS type ID. */
} lcs_service_type_ID_T;

/** @brief Location Services abort cause.
*/
typedef struct
{
   byte         present;
   /**< Whether an LCS abort cause is present. */
   byte         mm_cause;
   /**< Mobility Management cause value. */
   byte         lower_layer_cause;
   /**< Lower layer cause value. */
} lcs_abort_T;

/** @brief Location notification.
*/
typedef struct
{
   boolean                     present;
   /**< Whether a location notification is present. */
   notification_type_e_type    notification_type;
   /**< Notification type. */
   location_type_e_type        location_type;
   /**< Location type. */
   lcs_deferred_location_type  deferred_location_type;
   /**< Deferred location type. */
   lcs_client_external_id_T    external_id;
   /**< Client external ID. */
   lcs_client_name_T           client_name;
   /**< Client name. */
   lcs_requestor_id_T          requestor_id;
   /**< Requester ID. */
   lcs_codeword_string_T       codeword_string;
   /**< Codeword string. */
   lcs_service_type_ID_T       service_type;
   /**< Service type. */
   lcs_abort_T                 aborted;
   /**< LCS abort information. */
} location_notification_T;

/** @brief Location estimate.
*/
typedef struct
{
   byte present;
   /**< Whether location estimate information is present. */
   byte size;
   /**< Size  of location estimate data. */
   byte data[20];
   /**< Location estimate data. */
} location_estimate_T;

/** @brief Deciphering keys.
*/
typedef struct
{
   byte present;
   /**< Whether deciphering keys information is present. */
   byte data[15];
   /**< Deciphering key data. */
} deciphering_keys_T;

/** @brief Location estimate.
*/
typedef struct
{
   byte present;
   /**< Whether location estimate data is present. */
   byte size;
   /**< Size of the location estimate data. */
   byte data[91];
   /**< Location estimate data. */
} add_location_estimate_T;

/** @brief Location services response data.
*/
typedef struct
{
   location_estimate_T            location_estimate;
   /**< Location estimate information. */
   deciphering_keys_T             deciphering_keys;
   /**< Deciphering keys information. */
   add_location_estimate_T        add_location_estimate;
   /**< Additional location information. */
} lcs_molr_res_data_T;

/** @brief Access the network response for an LCS Mobile-Originated Location
    Request.
*/
typedef struct
{
  cm_ss_conf_e_type             data_type;
  /**< Data type. */
  union 
  {
    lcs_molr_res_data_T         molr_res_data;
    /**< Response data for a MOLR. */
    ie_cm_ss_error_T            cm_ss_error;
    /**< Error information/type of error. */
    ie_cm_cc_cause_T            cm_cc_cause;
    /**< Error cause. */
  } data;
} lcs_molr_res_T;
/*~ FIELD lcs_molr_res_T.data DISC lcs_molr_res_T.data_type */
/*~ IF ( _DISC_ == LCS_MOLR_INFO || _DISC_ == 0 )
    lcs_molr_res_T.data.molr_res_data */
/*~ CASE SS_ERROR_INFO lcs_molr_res_T.data.cm_ss_error */
/*~ CASE CC_CAUSE_INFO lcs_molr_res_T.data.cm_cc_cause */
/*~ CASE NO_INFO  lcs_molr_res_T.data.void */
 

/** Mobile-Originated Location Request services.
*/
typedef enum
{
  LOCATION_ESTIMATE, /**< &nbsp; */
  ASSISTANCE_DATA,   /**< &nbsp; */
  DECIPHERING_KEYS   /**< &nbsp; */
} molr_type_e_type;

/** @brief Index for Call Control to Busy Subscriber.
*/
typedef struct
{
    boolean  present;
	/**< Whether a CCBS index is present. */
    byte     ccbs_Index;
	/**< CCBS index. */

} ie_ccbs_index_T;

/** @brief Call Control to Busy Subscriber indicator.
*/
typedef struct 
{
    boolean  present;
    /**< Whether a CCBS indicator is present. */
    ie_ccbs_index_T              ccbsIndex;
    /**< CCBS index. */
    cm_called_party_bcd_no_T     subscriberNumber[2*MAXNO_FORWARD_TO_NUMBER];
    /**< Subscriber's number. */
    cm_called_party_subaddress_T subscriberAddress;
    /**< Subscriber's address. */
    basic_service_T              basicService;     
    /**< Basic service information. */

} ie_ccbs_indicator_T;

/** @brief Call Control to Busy Subscriber information.
*/
typedef struct
{
   boolean                         present;
   /**< Whether CCBS information is present. */
   ie_ss_status_T                  cm_ss_status;
   /**< CCBS service status. */
   byte                            ccbs_feature_list_length;
   /**< Length of the CCBS feature list. */
   ie_ccbs_indicator_T             ccbs_feature_list[MAX_CCBS_FEATURE_LIST_SIZE];
   /**< CCBS feature list. */
} ccbs_info_T;

/** @brief Multimedia codec list.
*/
typedef struct 
{
    byte    sysID;
	/**< System identification. */
    byte    bitmap_length;
	/**< Length of the bitmap. */
    byte    bitmap_1_8;
	/**< Bits 1 to 8. */
    byte    bitmap_9_16;
	/**< Bits 9 to 16. */
} codec_bitmap_list_T;

/** @brief Supported codec list.
*/
typedef struct
{
    byte                   present;
    /**< Whether a supported codec list is present. */
    byte                   length;
    /**< Number of supported codecs. */
    codec_bitmap_list_T    codec_list[MAX_SUPPORTED_MULTIMEDIA_CODEC_LIST];
    /**< Codec list. */
} cm_supported_codecs_list_T;

/*
**  NAS Definitions from sm_common.h 
*/
/** PDP address maximum length.
*/
#define MAX_PDP_ADR_LEN  20

/** APN address maximum length.
*/
#define MAX_APN_ADR_LEN  102

/** Maximum value of the protocol ID content.
*/
#define MAX_PROTOCOL_ID_CONTENT 127

/** Maximum number of protocol IDs in the protocol configuration parameters.
*/
#define NUM_PROTOCOL_ID 17

/** Number of non-IMS (IP Multimedia Subsystem) protocol IDs.
*/
#define NUM_PROTOCOL_ID_NON_IMS 4

/** Maximum size of an IPV4 packet filter is 32 bytes. It is 60
    bytes for IPV6.
*/
#define MAX_FILTER_CONTENT_LENGTH 60

/** Maximum number of packet filters allowed in the Traffic Flow Template (TFT).
*/
#define MAX_PACKET_FILTERS 8

/** Whether the UE requests to establish a new connection to a PDN or to
    keep the connection(s) to which it is connected via non-3GPP access.
*/
typedef enum
{
  ESM_INITIAL_REQUEST = 0x1,  /**< &nbsp; */
  ESM_HANDOVER                /**< &nbsp; */
}req_type_T;

/** EPS bearer identification.
*/
typedef byte eps_bearer_id_T;

/* --------------------------------------------------------------------------                       
** network service access point identifier information element is to identify
** the service access point that is used for the GPRS data transfer at
** layer 3. The IE is defined in 10.5.6.2
** ------------------------------------------------------------------------*/                       
/** @brief Network Service Access Point Identifier.
*/
typedef struct nsapi_s
{
  boolean valid;
  /**< Whether NSAPI is present. */
  byte    nsapi;
  /**< NSAPI value. */
} nsapi_T;


/* --------------------------------------------------------------------------                       
** LLC service access point identifier information element is to identify 
** the service access point that is used for the GPRS data transfer at LLC 
** layer. The IE is defined in 10.5.6.9
** ------------------------------------------------------------------------*/     
/** @brief Low Layer Compatibility Service Access Point Identifier.
*/
typedef struct llc_sapi
{
  boolean valid;
  /**< Whether LLC SAPI is present. */
  byte    sapi;
  /**< LLC SAPI value. */
} llc_sapi_T;

/*----------------------------------------------------------
                   PDN TYPE
  ----------------------------------------------------------
Refer Section: 9.9.4.7
*/
/** IP version capability of the IP stack associated with the UE.
*/
typedef enum 
{
  NAS_ESM_IPV4   = 1,  /**< &nbsp; */
  NAS_ESM_IPV6   = 2,  /**< &nbsp; */
  NAS_ESM_IPV4V6 = 3   /**< &nbsp; */
}pdn_type_T;

/*----------------------------------------------------------
                   PDN ADDRESS
  ----------------------------------------------------------
 
 Refer section: 9.9.4.9*/

/** Maximum length of the PDP address.
*/
#define MAX_PDN_ADR_LEN  12

/** @brief PDN address assigned to the UE.
*/
typedef struct pdn_address
{
  boolean     valid;
  /**< Whether a PDN address is present. */
  pdn_type_T  pdn_type_val;
  /**< PDN type. */
  byte        pdn_addr_len;
  /**< Length of the PDN address. */
  byte        address[MAX_PDN_ADR_LEN];
  /**< PDN address value. */
}pdn_address_T;

/** NAS local cause used to indicate local failures within evolved Mobility
Management (eMM) and evolved Session Management (eSM).
*/
typedef enum
{
  /* Add to this section any OTA(over the air) cause values that need equivalent representation as a local cause value*/
  /* Equivalent cause values to the EMM cause values for the purpose of relaying the srvice reject cause value to the DS*/
  LTE_NAS_ILLEGAL_UE_LOCAL = 3,                               /**< &nbsp; */
  LTE_NAS_ILLEGAL_ME_LOCAL = 6,                               /**< &nbsp; */
  LTE_NAS_EPS_SERVICES_NOT_ALLOWED_LOCAL = 7,                 /**< &nbsp; */
  LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW_LOCAL = 9,            /**< &nbsp; */
  LTE_NAS_IMPLICITLY_DETACHED_LOCAL = 10,                     /**< &nbsp; */
  LTE_NAS_PLMN_NOT_ALLOWED_LOCAL = 11,                        /**< &nbsp; */
  LTE_NAS_TRACKING_AREA_NOT_ALLOWED_LOCAL =12,                /**< &nbsp; */
  LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA_LOCAL=13, /**< &nbsp; */
  LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA_LOCAL=15,        /**< &nbsp; */
  LTE_NAS_CS_DOMAIN_NOT_AVAILABLE_LOCAL=18,                   /**< &nbsp; */
  LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG_LOCAL=25,               /**< &nbsp; */
  LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED_LOCAL = 38,        /**< &nbsp; */
  LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE_LOCAL = 39,            /**< &nbsp; */
  LTE_NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT = 43,                /**< &nbsp; */

  /* Add local cause values to this section*/
  DRB_RELEASED_AT_RRC = 256,                                  /**< &nbsp; */
  NAS_SIG_CONN_RELEASED,                                      /**< &nbsp; */
  EMM_DETACHED,                                               /**< &nbsp; */
  EMM_ATTACH_FAILED,                                          /**< &nbsp; */
  EMM_ATTACH_STARTED,                                         /**< &nbsp; */
  LTE_NAS_SERVICE_REQ_FAILED,                                 /**< &nbsp; */
  ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW,              /**< &nbsp; */
  ESM_LOWER_LAYER_FAILURE,                                    /**< &nbsp; */
  ESM_SYNC_UP_WITH_NW,                                        /**< &nbsp; */
  ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER,          /**< &nbsp; */
  ESM_BAD_OTA_MESSAGE,                                        /**< &nbsp; */
  ESM_DS_REJECTED_THE_CALL,                                   /**< &nbsp; */
  ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT,                         /**< &nbsp; */
  DS_EXPLICIT_DEACT,                                          /**< &nbsp; */

  ESM_MSGR_FAILURE,                                           /**< &nbsp; */
  ESM_LOCAL_CUASE_NONE,                                       /**< &nbsp; */
  ESM_REJ_DUE_TO_CONNECTED_STATE,                             /**< &nbsp; */
  LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,                     /**< &nbsp; */
  ACL_FAILURE,                                                /**< &nbsp; */
  LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW,                     /**< &nbsp; */
  EMM_T3417_EXPIRED,                                          /**< &nbsp; */
  EMM_T3417_EXT_EXPIRED,                                      /**< &nbsp; */
  
  LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_TXN,                       /**< &nbsp; */
  LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_HO,                        /**< &nbsp; */
  LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CONN_REL,                  /**< &nbsp; */
  LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_RLF,                       /**< &nbsp; */
  LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN,             /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_SUCCESS,                              /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE,                              /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_ABORTED,                      /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED,                /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_CELL_RESEL,                   /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE,               /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED,                /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_LINK_FAILURE,                 /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_NOT_CAMPED,                   /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_SI_FAILURE,                   /**< &nbsp; */
  LTE_NAS_LRRC_CONN_EST_FAILURE_CONN_REJECT,                  /**< &nbsp; */
  LTE_NAS_LRRC_CONN_REL_NORMAL,                               /**< &nbsp; */
  LTE_NAS_LRRC_CONN_REL_RLF,                                  /**< &nbsp; */
  LTE_NAS_LRRC_CONN_REL_CRE_FAILURE,                          /**< &nbsp; */
  LTE_NAS_LRRC_CONN_REL_OOS_DURING_CRE,                       /**< &nbsp; */
  LTE_NAS_LRRC_CONN_REL_ABORTED,                              /**< &nbsp; */
  LTE_NAS_LRRC_CONN_REL_SIB_READ_ERROR,
  LTE_NAS_LRRC_CONN_REL_ABORTED_IRAT_SUCCESS,                 /**< &nbsp; */
  LTE_NAS_REJ_LRRC_RADIO_LINK_FAILURE,                        /**< &nbsp; */
  LTE_NAS_SERVICE_REQ_FAILURE_LTE_NW_REJECT,                  /**< &nbsp; */
  LTE_NAS_DETACH_WITH_REATTACH_LTE_NW_DETACH,                 /**< &nbsp; */
  LTE_NAS_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH              /**< &nbsp; */

  
}lte_nas_local_cause_enum_T;

/** @brief NAS local cause information element.
*/
typedef struct
{
  boolean valid;
  /**< Whether an NAS local cause value is present. */
  lte_nas_local_cause_enum_T local_cause;
  /**< NAS local cause value. */
}lte_nas_local_cause_T;

/*----------------------------------------------------------
                   ESM CAUSE
  ----------------------------------------------------------
Refer Section: 9.9.4.2
*/
/** Reason a session management request (eSM) was rejected.
*/
typedef enum{
  NAS_ESM_OPTR_DETERMINED_BARRING = 8,                       /**< &nbsp; */
  NAS_ESM_INSUFFICIENT_RESOURCES = 26,                       /**< &nbsp; */
  NAS_ESM_UNKNOWN_MISSING_APN  =27,                          /**< &nbsp; */
  NAS_ESM_UNKNOWN_PDN_TYPE = 28,                             /**< &nbsp; */
  NAS_ESM_USER_AUTHENTICATION_FAILED = 29,                   /**< &nbsp; */
  NAS_ESM_OPN_REJ_SERVICING_GW_OR_PDN_GW = 30,               /**< &nbsp; */
  NAS_ESM_ACTIVATION_REJECTED = 31,                          /**< &nbsp; */
  NAS_ESM_SERV_OPT_NOT_SUPPORTED = 32,                       /**< &nbsp; */
  NAS_ESM_REQ_SERV_OPT_NOT_SPECIFIED =33,                    /**< &nbsp; */
  NAS_ESM_SERV_OPT_TEMP_OUT_OF_ORDER = 34,                   /**< &nbsp; */
  NAS_ESM_PTI_ALREADY_IN_USE =35,                            /**< &nbsp; */
  NAS_ESM_REGULAR_DEACTIVATION = 36,                         /**< &nbsp; */
  NAS_ESM_EPS_QOS_NOT_ACCEPTED = 37,                         /**< &nbsp; */
  NAS_ESM_NETWORK_FAILURE = 38,                              /**< &nbsp; */
  NAS_ESM_REACTIVATION_REQUIRED= 39,                         /**< &nbsp; */
  NAS_ESM_FEATURE_NOT_SUPPORTED = 40,                        /**< &nbsp; */
  NAS_ESM_SEMANTIC_ERR_IN_TFT_OPERATION =41,                 /**< &nbsp; */
  NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION =42,              /**< &nbsp; */
  NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT = 43,                   /**< &nbsp; */
  NAS_ESM_SYMANTIC_ERR_IN_PKT_FILTER = 44,                   /**< &nbsp; */
  NAS_ESM_SYNTACTICAL_ERR_IN_PKT_FILTER =45,                 /**< &nbsp; */
  NAS_ESM_EPS_BEARER_CONT_WO_TFT_ALREADY_ACT = 46,           /**< &nbsp; */
  NAS_ESM_PTI_MISMATCH = 47,                                 /**< &nbsp; */
  /* This is not in the standard in table 9.9.4.4.1, but
     is referred to in section 7.3 of the standard */
  NAS_ESM_LAST_PDN_DISCONNECTED_NOT_ALLOWED = 49,            /**< &nbsp; */
  NAS_ESM_PDN_TYPE_IPV4_ONLY_ALLOWED = 50,                   /**< &nbsp; */
  NAS_ESM_PDN_TYPE_IPV6_ONLY_ALLOWED =51,                    /**< &nbsp; */
  NAS_ESM_SINGLE_ADR_BEARERS_ONLY_ALLOWED =52,               /**< &nbsp; */
  NAS_ESM_ESM_INFO_NOT_RECEIVED=53,                          /**< &nbsp; */
  NAS_ESM_PDN_CONN_DOES_NOT_EXIST =54,                       /**< &nbsp; */
  NAS_ESM_MULTIPLE_PDN_CONN_FOR_GIVEN_APN_NOT_ALLOWED =55,   /**< &nbsp; */
  NAS_ESM_COLLISION_WITH_NW_INIT_REQ =56,                    /**< &nbsp; */
  NAS_ESM_UNSUPPORTED_QCI_VALUE =59,                         /**< &nbsp; */
  NAS_ESM_INVALID_PTI_VALUE =81,                             /**< &nbsp; */
  NAS_ESM_SYMANTICALLY_INVALID_MSG =95,                      /**< &nbsp; */
  NAS_ESM_INVALID_MANDATORY_INFORMATION = 96,                /**< &nbsp; */
  NAS_ESM_MSG_TYPE_NON_EXISTANT = 97,                        /**< &nbsp; */
  NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE = 98,             /**< &nbsp; */
  NAS_ESM_INFO_ELEMENT_NON_EXISTANT = 99,                    /**< &nbsp; */
  NAS_ESM_CONDITIONAL_IE_ERR = 100,                          /**< &nbsp; */
  NAS_ESM_MSG_INCOMPATIBLE_WITH_PRO_STATE= 101,              /**< &nbsp; */
  NAS_ESM_PROTOCOL_ERROR = 111,                              /**< &nbsp; */
  NAS_ESM_APN_RESTRICT_VAL_INCOMPAT_WITH_ACT_CONTEXT = 112,  /**< &nbsp; */
  NAS_ESM_NO_FAILURE  /**< &nbsp; */ /* IF ADDING ANY NEW CAUSE VALUES ADD BEFORE THIS*/
  }esm_cause_enum_T;
/*~ SENTINEL esm_cause_enum_T.NAS_ESM_NO_FAILURE */

/** @brief Reason a session management request (eSM) was rejected.
*/
typedef struct 
{
  boolean valid;
  /**< Whether a session management cause is present. */
  esm_cause_enum_T esm_cause;
  /**< Session management cause value. */
} esm_cause_T;

/* -------------------------------------------------------------
             SDF QOS OPTIONS
   -------------------------------------------------------------
*/
/* Refer Section: 9.9.4.12*/

/** @brief QoS parameters for a bearer context.
*/
typedef struct sdf_qos_struct
{
  boolean valid;
  /**< Whether QoS parameters are present. */
  byte    sdf_qos_iei;
  /**< QoS Information Element Indicator. */
  byte    length;
  /**< Length. */
  byte    qci;
  /**< QoS Class Identifier. */
  byte    max_bit_rate_ul;
  /**< Maximum uplink bitrate. */
  byte    max_bit_rate_dl;
  /**< Maximum downlink bitrate. */
  byte    gbr_ul;
  /**< Guaranteed bitrate in uplink. */
  byte    gbr_dl;
  /**< Guaranteed bitrate in downlink. */
  byte    max_bit_rate_ul_ex;
  /**< Extended maximum uplink bitrate. */
  byte    max_bit_rate_dl_ex;
  /**< Extended maximum downlink bitrate. */
  byte    gbr_ul_ex;
  /**< Extended guaranteed bitrate in uplink. */
  byte    gbr_dl_ex;
  /**< Extended guaranteed bitrate in downlink. */
}sdf_qos_T;

/* --------------------------------------------------------------------------                       
** the quality of service information element is to specify the QoS 
** parameters for a PDP context. The IE is defined in 10.5.6.5
** ------------------------------------------------------------------------*/                       

/** @brief Quality of Service information.
*/
typedef struct qos
{
  boolean valid;
  /**< Whether QoS information is present. */
  byte    delay_class;
  /**< Delay class. */
  byte    reliability_class;
  /**< Reliability class. */
  byte    peak_throughput;
  /**< Peak throughput. */
  byte    precedence_class;
  /**< Precedence class. */
  byte    mean_throughput;
  /**< Mean throughput. */
  byte    traffic_class;
  /**< Traffic class. */
  byte    deliver_order;
  /**< Delivery order. */
  byte    deliver_err_sdu;
  /**< Delivery of an erroneous Service Data Unit. */
  byte    max_sdu_size;
  /**< Maximum Service Data Unit size. */
  byte    max_bit_rare_uplink;
  /**< Maximum uplink bitrate. */
  byte    max_bit_rare_downlink;
  /**< Maximum downlink bitrate. */
  byte    residual_ber;
  /**< Residual Bit Error Rate. */
  byte    sdu_err_ratio;
  /**< Service Data Unit error ratio. */
  byte    tranfer_delay;
  /**< Transfer delay. */
  byte    traffic_priority;
  /**< Traffic priority. */
  byte    guarantee_ber_uplink;
  /**< Guaranteed uplink Bit Error Rate. */
  byte    guarantee_ber_dwnlink;
  /**< Guaranteed downlink Bit Error Rate. */
  boolean sig_ind;
  /**< Signal indication. */
  byte    src_stat_desc;
  /**< Source statistics descriptor. */
  byte    ext_max_bit_rate_downlink;
  /**< Extended maximum downlink bitrate. */
  byte    ext_gtd_bit_rate_downlink;
  /**< Extended guaranteed downlink bitrate. */
  byte    ext_max_bit_rate_uplink;
  /**< Extended maximum uplink bitrate. */
  byte    ext_gtd_bit_rate_uplink;
  /**< Extended guaranteed uplink bitrate. */
} qos_T;


/*----------------------------------------------------------------
* Enumeration from BCM
*---------------------------------------------------------------*/
/** Bearer control mode.
*/
typedef enum
{
  BCM_MS_ONLY = 0x01,   /**< &nbsp; */
  BCM_MS_NW             /**< &nbsp; */
} bearer_ctrl_mode_T;


/* --------------------------------------------------------------------------                       
** the packet data protocol address information element is to identify an 
** address associated with a PDP. The IE is defined in 10.5.6.4.
** ------------------------------------------------------------------------*/                       
/** @brief Packet Data Protocol address.
*/
typedef struct pdp_address
{
  boolean valid;
  /**< Whether a PDP address is present. */
  byte    pdp_type_org;
  /**< PDP type organization. */
  byte    pdp_type_num;
  /**< PDP type number. */
  byte    pdp_addr_len;
  /**< PDP address length. */
  byte    address[MAX_PDP_ADR_LEN];
  /**< PDP address. */
  
} pdp_address_T;
 
/*~ FIELD pdp_address.address VARRAY MAX_PDP_ADR_LEN
    LENGTH pdp_address.pdp_addr_len */
 

/* --------------------------------------------------------------------------                       
** The Access Point Name is a label or a full qualified domain name 
** according to DNS naming conventions . The IE is defined in 10.5.6.1.
** ------------------------------------------------------------------------*/                       
/** @brief Access Point Name information.
*/
typedef struct apn
{
  boolean valid;
  /**< Whether APN information is present. */
  word    apn_addr_len;
  /**< APN address length. */
  byte    address[MAX_APN_ADR_LEN];
  /**< APN address. */
} apn_T;
 
/*~ FIELD apn.address VARRAY MAX_APN_ADR_LEN LENGTH apn.apn_addr_len */

/** @brief Indicates the initial subscribed APN AMBR (Aggregate Maximum Bitrate)
    when the UE establishes a PDN connection or indicates the new APN-AMBR
    if it is changed by the network.
*/
typedef struct
{
  boolean valid;
  /**< Whether an APN AMBR is present. */
  byte    apn_ambr_ul;
  /**< APN AMBR uplink. */
  byte    apn_ambr_dl;
  /**< APN AMBR downlink. */
  byte    apn_ambr_ul_ext;
  /**< Extended APN AMBR uplink. */
  byte    apn_ambr_dl_ext;
  /**< Extended APN AMBR downlink. */
  byte    apn_ambr_ul_ext2;
  /**< Second extended APN AMBR uplink. */
  byte    apn_ambr_dl_ext2;
  /**< Second extended APN AMBR downlink. */

}apn_ambr_T;

/** @brief Used in call object transfers between GW and LTE.
*/
typedef struct gsm_umts_connection_id
{
  boolean    valid;
  /**< Whether a connection ID is present. */
  byte       connection_id;
  /**< Connection ID value. */
} gsm_umts_connection_id_T;

/* --------------------------------------------------------------------------                       
** The purpose of the protocol configuration options information element is 
** to transfer external network protocol options associated with a PDP context
** activation . The IE is defined in 10.5.6.3.
** ------------------------------------------------------------------------*/                       

/* Structure for individual protocol information */

/** @brief Individual protocol information.
*/
typedef struct protocol_id_s
{
  byte protocol_id[2];
  /**< Protocol IDs. */
  byte protocol_len;
  /**< Protocol ID content length. */
  byte protocol_contents[MAX_PROTOCOL_ID_CONTENT];
  /**< Protocol content. */
} protocol_id_T;
 
/*~ FIELD protocol_id_s.protocol_contents VARRAY MAX_PROTOCOL_ID_CONTENT
    LENGTH protocol_id_s.protocol_len */
 
/** @brief Protocol configuration options.
*/
typedef struct protocol_cfg_options
{
  boolean         valid;
  /**< Whether protocol configuration options are present. */
  byte            config_protocol;
  /**< Configuration protocol. */
  word            num_protocol_ids;
  /**< Number of protocol IDs. */
  protocol_id_T   procol_info[NUM_PROTOCOL_ID];
  /**< Protocol information. */
} protocol_cfg_options_T;
 
/*~ FIELD protocol_cfg_options.procol_info VARRAY NUM_PROTOCOL_ID
    LENGTH protocol_cfg_options.num_protocol_ids */

/** @brief Packet Data Network information to be sent in a PDN connectivity
    message.
*/
typedef struct pdn_connection_data
{
  pdn_type_T pdn_type;
  /**< Packet Data Network type. */
  boolean esm_info_transfer_flag; 
  /**< Session Management information transfer flag. */
  apn_T apn_name;  
  /**< Access Point Network name. */
  boolean   pco_ciphering_flag;
  /**< Ciphering flag. */
  protocol_cfg_options_T  protocol_config_options;
  /**< Protocol configuration options. */
  boolean  attach_pdn_flag;
  /**< Is this a PDN REQ for ATTACH as the response for a NAS ATTACH req. */
} pdn_connection_data_T ;

/* --------------------------------------------------------------------------                       
** the tear down indicator information element is to indicate whether 
** only the PDP context associated with this specific TI or all active 
** PDP contexts sharing the same PDP address as the PDP context associated 
** with this specific TI shall be deactivated. defined in 10.5.6.10.
** ------------------------------------------------------------------------*/
/** @brief Teardown Indicator.
 */
typedef struct ti
{
  boolean valid;
  /**< Whether a teardown indicator is present. */
  boolean ti_flag;
  /**< Teardown Indicator flag. */
} ti_T;


/* --------------------------------------------------------------------
 SM cause enumeration type, defined in Table 10.5.157/TS 24.008 
 This is the cause sent to the SM-SM peer entity.
** ------------------------------------------------------------------*/

/** Session management cause type.
*/
typedef enum
{
  /** @cond
  */
  SM_UNKNOWN_CAUSE = 0x01, /* For SM internal use only, not to be used in OTA */
  /** @endcond
  */
  SM_OPERATOR_DETERMINED_BARRING = 0x08,           /**< &nbsp; */
  SM_MBMS_BEARER_CAP_INSUFFICIENT = 0x18,          /**< &nbsp; */
  SM_LLC_SNDCP_FAILURE          = 0x19,            /**< &nbsp; */
  SM_INSUFFICIENT_RESOURCES,                       /**< &nbsp; */
  SM_MISSING_OR_UNKNOWN_APN,                       /**< &nbsp; */
  SM_UNKNOWN_PDP_ADDRESS_OR_TYPE,                  /**< &nbsp; */
  SM_USER_AUTHENTICATION_FAILED,                   /**< &nbsp; */
  SM_ACTIVATION_REJECTED_BY_GGSN,                  /**< &nbsp; */
  SM_ACTIVATION_REJECTED_UNSPECIFIED,              /**< &nbsp; */
  SM_SERVICE_OPTION_NOT_SUPPORTED,                 /**< &nbsp; */
  SM_SERVICE_NOT_SUBSCRIBED,                       /**< &nbsp; */
  SM_SERVICE_OUT_OR_ORDER,                         /**< &nbsp; */
  SM_NSAPI_ALREADY_USED,                           /**< &nbsp; */
  SM_REGULAR_DEACTIVATION,                         /**< &nbsp; */
  SM_QOS_NOT_ACCEPTED,                             /**< &nbsp; */
  SM_NETWORK_FAILURE,                              /**< &nbsp; */
  SM_REACTIVATION_REQUIRED,                        /**< &nbsp; */
  SM_FEATURE_NOT_SUPPORTED,                        /**< &nbsp; */
  SM_SEMANTIC_ERR_IN_TFT,                          /**< &nbsp; */
  SM_SYNTACTIC_ERR_IN_TFT,                         /**< &nbsp; */
  SM_UNKNOWN_PDP_CONTEXT,                          /**< &nbsp; */
  SM_SEMANTIC_ERR_IN_FILTER,                       /**< &nbsp; */
  SM_SYNTACTIC_ERR_IN_FILTER,                      /**< &nbsp; */
  SM_PDP_WITHOUT_TFT_ACTIVE,                       /**< &nbsp; */
  SM_MBMS_GROUP_MEMBERSHIP_TIMEOUT,                /**< &nbsp; */
  SM_ACTIVATION_REJECTED_BCM_VIOLATION = 0x30,     /**< &nbsp; */
  SM_PDP_TYPE_IPV4_ONLY_ALLOWED = 0x32,            /**< &nbsp; */
    //0 0 1 1 0 0 1 0 PDP type IPv4 only allowed
  SM_PDP_TYPE_IPV6_ONLY_ALLOWED = 0x33,            /**< &nbsp; */
    //0 0 1 1 0 0 1 1 PDP type IPv6 only allowed
  SM_SINGLE_ADR_BEARERS_ONLY_ALLOWED = 0x34,       /**< &nbsp; */
    //0 0 1 1 0 1 0 0 Single address bearers only allowed
  SM_INVALID_TRANS_ID = 0x51,                      /**< &nbsp; */
  SM_SEMANTIC_INCORRECT_MESG = 0x5F,               /**< &nbsp; */
  SM_INVALID_MANDATORY_INFO = 0x60,                /**< &nbsp; */
  SM_MESG_TYPE_NON_EXISTENT,                       /**< &nbsp; */
  SM_MESG_TYPE_NOT_COMPATIBLE,                     /**< &nbsp; */
  SM_IE_NON_EXISTENT,                              /**< &nbsp; */
  SM_CONDITIONAL_IE_ERR,                           /**< &nbsp; */
  SM_MESG_NOT_COMPATIBLE,                          /**< &nbsp; */
  SM_PROTOCOL_ERR = 0x6F,                          /**< &nbsp; */
  SM_APN_TYPE_CONFLICT                             /**< &nbsp; */
} sm_status_T;

/** Packet filter direction.
*/
typedef enum
{
  FILTER_PRE_REL7,           /**< &nbsp; */
  FILTER_DOWNLINK,           /**< &nbsp; */
  FILTER_UPLINK,             /**< &nbsp; */
  FILTER_BIDIRECTIONAL       /**< &nbsp; */
} packet_filter_direction_T;


/* --------------------------------------------------------------------------                       
** The purpose of the traffic flow template information element is to specify 
** the TFT parameters and operations for a PDP context.. The IE is defined 
** in 10.5.6.12.
** ------------------------------------------------------------------------*/ 
/* Structure for individual packet filter information */ 

/** @brief Individual packet filter information.
*/
typedef struct packet_filter
{
  byte  identifier;
  /**< Packet filter identifier. */
  packet_filter_direction_T direction;
  /**< Packet filter direction. */
  byte  eval_precedence;
  /**< Precedence. */
  byte  content_length;
  /**< Content length. */
  byte  filter_contents[MAX_FILTER_CONTENT_LENGTH];
  /**< Packet filter contents. */
} packet_filter_type_T;
/*~ FIELD packet_filter.filter_contents VARRAY MAX_FILTER_CONTENT_LENGTH
    LENGTH packet_filter.content_length */

/** @brief Filters to be used in uplink and downlink.
*/
typedef struct
{
  byte  identifier;
  /**< Filter identifier. */
  byte  content_length;
  /**< Content length. */
  byte  filter_contents[MAX_FILTER_CONTENT_LENGTH];
  /**< Filter contents. */
}parameter_filter_type_T;


/* Data structure for TFT which has a list of individual filters */

/** @brief Traffic Flow Template parameters provide operations for a PDP
    context. They provide downlink and uplink packet filters that will
    be applied by the NW and the UE, respectively, to route the data.
*/
typedef struct tft
{
  boolean                 valid;
  /**< Whether TFT parameters are present. */

  word                    tft_total_length;
  /**< Total length of the TFT information element. */

  byte                    operation_code;
  /**< TFT operation, e.g., create, delete. */

  byte                    e_bit;
  /**< Whether the parameter list is included in the TFT. */

  byte                    num_filters;
  /**< Number of packet filters. */

  packet_filter_type_T    filter_list[MAX_PACKET_FILTERS];
  /**< Each packet filter contains an identifier, evaluation
       precedence, and the length of the packet filter. */

  parameter_filter_type_T param_list[MAX_PACKET_FILTERS];
  /**< Each parameter list contains the packet identifier, length, and
       contents. This is used to transfer extra information (e.g.,
       an authorization token) to the NW. */

} tft_type_T;
 
/*~ FIELD tft.filter_list VARRAY MAX_PACKET_FILTERS
    LENGTH tft.num_filters */

/*
**  NAS Definitions from sm_cm.h
*/

/* --------------------------------------------------------------------------
 Enumeration for failures, caused due to internal failures (inside MS)
** ------------------------------------------------------------------------*/

/** Failures, caused by internal failures (inside the MS).
*/
typedef enum
{
  INVALID_CONNECTIN_ID,                               /**< &nbsp; */
  INVALID_NSAPI,                                      /**< &nbsp; */
  INVALID_PRI_NSAPI,                                  /**< &nbsp; */
  INVALID_FIELD,                                      /**< &nbsp; */
  SNDCP_FAILURE,                                      /**< &nbsp; */
  /*--5--*/
  RAB_SETUP_FAILURE,                                  /**< &nbsp; */
  NO_GPRS_CONTEXT,                                    /**< &nbsp; */
  PDP_ESTABLISH_MAX_TIMEOUT,                          /**< &nbsp; */
  PDP_ACTIVATE_MAX_TIMEOUT,                           /**< &nbsp; */
  PDP_MODIFY_MAX_TIMEOUT,                             /**< &nbsp; */
  /*--10--*/
  PDP_INACTIVE_MAX_TIMEOUT,                           /**< &nbsp; */
  PDP_LOWERLAYER_ERROR,                               /**< &nbsp; */
  PDP_DUPLICATE,                                      /**< &nbsp; */
  UE_RAT_CHANGE,                                      /**< &nbsp; */
  PPD_UNKNOWN_REASON,                                 /**< &nbsp; */
  /*--15--*/
  PDP_MODIFY_COLLISION,                               /**< &nbsp; */
  PDP_MBMS_REQUEST_COLLISION,                         /**< &nbsp; */
  MBMS_DUPLICATE,                                     /**< &nbsp; */
  INVALID_PDP_CONTEXT_STATE,                          /**< &nbsp; */
  SM_ILLEGAL_MS,                                      /**< &nbsp; */
  /*--20--*/
  SM_ILLEGAL_ME,                                      /**< &nbsp; */
  SM_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED, /**< &nbsp; */
  SM_GPRS_SERVICES_NOT_ALLOWED,                       /**< &nbsp; */
  SM_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK,    /**< &nbsp; */
  SM_IMPLICITLY_DETACHED,                             /**< &nbsp; */
  /*--25--*/
  SM_PLMN_NOT_ALLOWED,                                /**< &nbsp; */
  SM_LA_NOT_ALLOWED,                                  /**< &nbsp; */
  SM_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN,          /**< &nbsp; */
  SM_CONGESTION,                                      /**< &nbsp; */
  SM_NO_PDP_CONTEXT_ACTIVATED,                        /**< &nbsp; */
  /*--30--*/
  SM_ACCESS_CLASS_DSAC_REJECTION,                     /**< &nbsp; */
  SM_NATIONAL_ROAMING_NOT_ALLOWED,                    /**< &nbsp; */
    /* internal to NAS to sent to other module */
  SM_NO_SUITABLE_CELLS_IN_LA,                         /**< &nbsp; */
    /* internal to NAS to sent to other module */
  SM_UNASSIGNED_CAUSE,                                /**< &nbsp; */
    /* internal to NAS to sent to other module */
  UE_GW_L_RAT_CHANGE,                                 /**< &nbsp; */
    /* Internal cause for GW to LTE redirection */
  /*--35--*/
  SM_PS_DETACHED,                                      /**< &nbsp; */
    /* Internal cause for call end due to PS DETACH */
  SM_NO_RADIO_AVAILABLE,
    /* Internal cause to indicate that Radio resource is not available */
  SM_ABORT_SERVICE_NOT_AVAILABLE
    /* Internal cause to indicate abort because service is not available */
}sm_cm_status_T;

/* --------------------------------------------------------------------------
 Enumeration that identifies if the cause is of Internal or due to Network.
** ------------------------------------------------------------------------*/

/** Whether the cause is internal or due to the network.
*/
typedef enum
{
  SM_NETWORK_CAUSE,  /**< &nbsp; */
  SM_INTERNAL_CAUSE  /**< &nbsp; */
} cause_enum_type_T;

/** @brief Packet Data Protocol cause.
*/
typedef union
{
    sm_cm_status_T    int_cause;
	/**< Internal cause. */
    sm_status_T       ext_cause;
	/**< External cause. */
} pdp_cause_T;



/*~ CASE SM_INTERNAL_CAUSE pdp_cause_T.int_cause */
/*~ CASE SM_NETWORK_CAUSE pdp_cause_T.ext_cause */
 

/*
**  NAS Definitions from rabm_cm.h
*/

/** Radio Access Bearer ID value.
*/
typedef byte rabm_rabid_T;

/** Cause during an RAB release or reject.
*/
typedef byte rabmcm_cause_T;

/*
**  NAS Definitions from sys_plmn_selection.h
*/

/** How a PLMN is selected.
*/
typedef enum sys_network_selection_mode_e
{
  /** @cond
  */
  SYS_NETWORK_SELECTION_MODE_NONE = -1,          /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
  SYS_NETWORK_SELECTION_MODE_AUTOMATIC,                   /**< &nbsp; */
  SYS_NETWORK_SELECTION_MODE_MANUAL,                      /**< &nbsp; */
  SYS_NETWORK_SELECTION_MODE_LIMITED,                     /**< &nbsp; */
  SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION, /**< &nbsp; */
  SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION,  /**< &nbsp; */
  /** @cond
  */
  SYS_NETWORK_SELECTION_MODE_MAX                /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_network_selection_mode_e_type;
/*~ SENTINEL sys_network_selection_mode_e_type.SYS_NETWORK_SELECTION_MODE_MAX */


/*
** Type definitions taken from cmwmsg.h
*/

/** User data information.
*/
typedef cm_user_data_T                   cm_user_data_s_type;

/** Repeat indicator information.
*/
typedef cm_repeat_indicator_T            cm_repeat_indicator_s_type;

/** Bearer capability information.
*/
typedef cm_bearer_capability_T           cm_bearer_capability_s_type;

/** Lower layer compatibility information.
*/
typedef cm_low_layer_compatibility_T     cm_low_layer_compatibility_s_type;

/** High layer compatibility information.
*/
typedef cm_high_layer_compatibility_T    cm_high_layer_compatibility_s_type;

/** Called party binary coded decimal number.
*/
typedef cm_called_party_bcd_no_T         cm_called_party_bcd_no_s_type;

/** Called party subaddress.
*/
typedef cm_called_party_subaddress_T     cm_called_party_subaddress_s_type;

/** Calling party Binary Coded Decimal number.
*/
typedef cm_calling_party_bcd_no_T        cm_calling_party_bcd_no_s_type;

/** Calling party subaddress.
*/
typedef cm_calling_party_subaddress_T    cm_calling_party_subaddress_s_type;

/** Forward Closed User Group information.
*/
typedef cm_forward_cug_info_T            cm_forward_cug_info_s_type;

/** Call Control error cause information.
*/
typedef ie_cm_cc_cause_T                 ie_cm_cc_cause_s_type;

/** Mobile Originated call details.
*/
typedef ie_cm_mo_call_details_T          ie_cm_mo_call_details_s_type;

/** Supplementary Services error information.
*/
typedef ie_cm_ss_error_T                 ie_cm_ss_error_s_type;

/** Active calls list.
*/
typedef active_calls_list_T              active_calls_list_s_type;

/** Connected number information.
*/
typedef cm_connected_no_T                cm_connected_no_s_type;

/** Connected subaddress.
*/
typedef cm_connected_subaddress_T        cm_connected_subaddress_s_type;

/** Call forwarding information.
*/
typedef ie_forwarding_info_T             ie_forwarding_info_s_type;

/** Call barring information.
*/
typedef ie_call_barring_info_T           ie_call_barring_info_s_type;

/** Closed User Group information.
*/
typedef ie_cug_info_T                    ie_cug_info_s_type;

/** Supplementary Services status.
*/
typedef ie_ss_status_T                   ie_ss_status_s_type;

/** Basic service group list.
*/
typedef basic_service_group_list_T       basic_service_group_list_s_type;

/** Forwarding feature list.
*/
typedef ie_forwarding_feature_list_T     ie_forwarding_feature_list_s_type;

/** Calling Line ID restriction information.
*/
typedef cli_restriction_info_T           cli_restriction_info_s_type;

/** Supplementary Services password information.
*/
typedef ie_ss_password_T                 ie_ss_password_s_type;

/** Unstructured Supplementary Services data information.
*/
typedef uss_data_T                       uss_data_s_type;

/** Supplementary Services data.
*/
typedef ie_cm_ss_data_T                  ie_cm_ss_data_s_type;

/** Basic service.
*/
typedef basic_service_T                  basic_service_s_type;

/** No reply condition timer information.
*/
typedef ie_no_reply_condition_timer_T    ie_no_reply_condition_timer_s_type;

/** Supplementary Services code.
*/
typedef ie_ss_code_T                     ie_ss_code_s_type;

/** Supplementary Services user data.
*/
typedef ss_User_Data_T                   ss_User_Data_s_type;

/** Unstructured Supplementary Services response data.
*/
typedef uss_res_data_T                   uss_res_data_s_type;

/** Cause of no Calling Line Identification.
*/
typedef cm_cause_of_no_CLI_T             cm_cause_of_no_cli_s_type;

/** Routing number option.
*/
typedef routing_number_option_T          routing_number_option_s_type;

/** Explicit Call Transfer call state.
*/
typedef ect_call_state_T                 ect_call_state_s_type;

/** Supplementary Services notification.
*/
typedef ie_ss_notification_T             ie_ss_notification_s_type;

/** Call Control rejection.
*/
typedef ie_cm_cc_reject_T                ie_cm_cc_reject_s_type;

/** Location method information.
*/
typedef location_method_T                location_method_s_type;

/** Location Services QoS information.
*/
typedef lcs_qos_T                        lcs_qos_s_type;

/** Mobile Location Channel number.
*/
typedef mlc_number_T                     mlc_number_s_type;

/** Location Services external client ID.
*/
typedef lcs_client_external_id_T         lcs_client_external_id_s_type;

/** GPS assistance data.
*/
typedef gps_assistance_data_T            gps_assistance_data_s_type;

/** Supported GAD shapes.
*/
typedef supported_gad_shapes_T           supported_gad_shapes_s_type;

/** Location notification.
*/
typedef location_notification_T          location_notification_s_type;

/** Location notification response data.
*/
typedef ie_ss_location_notification_res_T ie_ss_location_notification_res_s_type;

/** Location Services Mobile Originated Location Request.
*/
typedef lcs_molr_res_T                   lcs_molr_res_s_type;

/** Mobile Originated Location Request type.
*/
typedef molr_type_e_type                 cm_molr_type_e_type;

/** Call Completion to Busy Subscriber information.
*/
typedef ccbs_info_T                      ccbs_info_s_type;

/** Supported codecs list information.
*/
typedef cm_supported_codecs_list_T       cm_supported_codecs_list_s_type;

/** Network Service Access Point ID.
*/
typedef  nsapi_T                         nsapi_s_type;  

/** Lower Layer Channel Service Access Point ID.
*/
typedef  llc_sapi_T                      llc_sapi_s_type;  

/** Causes for no calling line identification.
*/
typedef enum no_cli_cause_e
{

  CLI_UNAVAILABLE,                  /**< &nbsp; */
  CLI_REJECT_BY_USER,               /**< &nbsp; */
  CLI_INTERACTION_WITH_OTHER_SRVC,  /**< &nbsp; */
  CLI_COIN_PAY_PHONE                /**< &nbsp; */

} no_cli_cause_e_type;

/** @} */ /* end_addtpgroup CM_GW GSM/WCDMA Protocol Definitions for Clients */

#endif /* CM_GW_H */
