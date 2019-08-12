

#ifndef CM_H
#define CM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*==========================================================================

              C A L L   M A N A G E R   H E A D E R   F I L E

===========================================================================*/

/**
@file cm.h
  This header file contains all the definitions necessary for
  clients to interface with the Call Manager module. Details are provided
  below.
*/
/*===========================================================================
Copyright (c) 2002 - 2011 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/cm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/


/*-----------------------------------------------------------------------------

  Organization of the file:
  -------------------------
    The file is divided into multiple sections. Each section describes a
    call manager object. You can jump from one sections to the other by
    searching for / followed by 3 *'s. The order of objects defined is as
    follows:

        Common section
        Call Object
        Phone Object
        Serving System Object
        Inband Object
        SMS Object
        Position Determination Object
        Supplementary Services Object (GSM/WCDMA)
        DBM Object
        BCMCS Object
        MBMS object 
        Client Object
        Utility functions

    Each section is further subdivided to represent different types of info.
    You can jump from one subsections to another by searching for /
    followed by 2 *'s. The data in each section is organized in the
    following order.

        #defines
        enums
        macros
        datatypes
        functions

  Abbreviations Used:
  -------------------
    GW  - GSM/WCDMA
    DBM - Data Burst Message

-----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------

Common to all Call Manager Objects

----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
   Defines that can be checked by CM clients to detect support for a feature.
   Added for client code to co-exist between different versions of CM.
----------------------------------------------------------------------------*/


/** @addtogroup COMMON Common Definitions
    @{ */

/** OTASP activation code.
*/
#define CM_API_OTASP_ACTIVATE_CODE

/* Add new activation code support for BC 18, 19
   and remove activation code for BC 7 D band. */

/** Enables backward compatibility from the UI side.
*/
#define CM_API_OTASP_BC18_19_SUPPORT

/** Secondary Packet Data Protocol (PDP) support in the CM can be detected by
    checking for this define.
*/
#define CM_API_SEC_PDP

/** Support for user-to-user data signaling can be detected by checking for
    this define.
*/
#define CM_API_USER_USER_DATA

/** Support for WLAN in the CM can be detected by checking for this define.
*/
#define CM_API_WLAN

/** Support for remote Wireless Messaging Services (WMS). This requires that the
    CM handle certain Network Access Server (NAS) events and commands for WMS.
*/
#define CM_API_REMOTE_WMS

/** Support for Video Telephony calls.
*/
#define CM_API_VT

/** Support for 64-bit band preferences.
*/
#define CM_API_BAND_PREF_64_BIT

/** Support for the IP call API in the CM can be detected by checking for this
    define.
*/
#define CM_API_IP_CALL

/** Support for GSM/WCDMA ciphering indication.
*/
#define CM_CIPHERING_IND_SUPP

/** Support for Call Completion to Busy Subscriber (CCBS).
*/
#define CM_API_CCBS

/** Support for Home Public Land Mobile Network (HPLMN) search requests.
*/
#define CM_API_INITIATE_HPLMN_SRCH

/** Changes to PDP modify API request.
*/
#define CM_API_CHG_PDP_MOD

/** Support for reporting emergency number list.
*/
#define CM_API_EMERGENCY_NUM_LIST

/** Support for additional incoming substates.
*/
#define CM_API_ADD_INCOM_SUBST

/** Support for videoshare.
*/
#define CM_API_VIDEOSHARE

/** Support for Advice of Charge (AOC) information.
*/
#define CM_API_AOC_INFO

/** Support for sending unparsed L3 data from a Mobile Network (MN) to clients.
*/
#define CM_SUPS_UNPARSED_L3_DATA

/** APIs and enumerations for VT fallback to voice feature.
*/
#define CM_API_VT_FALLBACK_TO_VOICE

/** Support for emergency service category in call origination and call control.
*/
#define CM_API_ECATEG_IN_NUM_STRUCT

/** Support to inform client on the success/failure of PLMN list request.
*/
#define CM_API_AVAIL_NET_LIST_CNF

/** Supports Supplementary Services (SUPS) events for cell-supported services.
*/
#define CM_API_CELL_SUPP_SERVICES

/** Defines support for GW Packet-Switched (PS) call control.
*/
#define CM_API_GW_PS_CC

/** Support for IP call SS success/failure.
*/
#define CM_API_IPCALL_SS_INFO

/** Support for the WLAN WiFi Positioning System (WPS) procedure.
*/
#define CM_API_WLAN_WPS

/** Support for enumeration on PDP header and data compression.
*/
#define CM_API_PDP_COMPR

/** Support for the HDR active protocol revision.
*/
#define CM_API_HDR_ACTIVE_PROT

/** Support for rab_id for Circuit-Switched calls.
*/
#define CM_API_CS_RABID

/** Supports Persistent implementations for acq_order, band_pref, roam_pref,
    hybrid_pref, and srv_domain_pref.
*/
#define CM_API_PERSISTENT_PREF

/** Support for gw_ecio.
*/
#define CM_API_GW_ECIO

/** Check for this flag before using cm_result_from_cc_s_type.
*/
#define CM_API_RESULT_FROM_CC

/** Support for FEATURE_SMS_OVER_IP.
*/
#define CM_API_SMS_OVER_IP

/** Field to report Handset Services call status.
*/
#define CM_API_CELL_SUPP_SERVICES_HS_CALL

/** Support for handset-based plus dialing.
*/
#define CM_API_PLUS_DIALING

/** Support for MBMS.
*/
#define CM_API_MBMS

/** Support for reporting an OTASP Special Prefix Code (SPC) error.
*/
#define CM_API_OTASP_SPC_ERR

/** Support for FDN call control parameters in call origination API.
*/
#define CM_API_CC_MOD_PARAM

/** The spare_bits field in mm_information that indicates the number of
    spare bits in the end octet of Network names(full/short).
*/
#define CM_API_SPARE_BITS_IN_NW_NAMES

/** Support for PRL preferences in CM can be detected by checking for this
    define.
*/
#define CM_API_SYS_SEL_PRL_PREF

/** Support for centralized call control in CM call origination and SUPS request
    APIs.
*/
#define CM_API_MODEM_CALL_CONTROL

/** Initial initialization value for all call event fields.
*/
#define CM_CALL_EVENT_DEFAULT_FIELD_VALUE    CM_CALL_CMD_PARAM_DEFAULT_VALUE


/* To discriminate the union net_profile_s_type.profile
** on the basis of byte type discriminator, same type
** of macro is needed in the union metacomment, hence
** defining the same type of macros.
*/

#define  CM_NET_PROFILE_TYPE_NONE_RPC   0

#define  CM_NET_PROFILE_TYPE_WLAN_RPC   1

/*---------------------------------------------------------------------------
   Includes
---------------------------------------------------------------------------*/

#include "comdef.h"     /* Definition for basic types and macros */
#include "sys.h"        /* system wide common types */
#include "sys_gw.h"     /* GSM/WCDMA specific types */
#include "sys_wlan.h"
#include "cm_gw.h"      /* Definitions from NAS required by clients */

/*---------------------------------------------------------------------------
   Defines
---------------------------------------------------------------------------*/

/** CM signal for NV synchronous read by UI in CM thread context.
*/
#define CM_UI_NV_SIG        0x0080

/** Used in cm_sups_unparsed_l3_data_s_type to define the maximum size of the L3
    message. Clients must check for CM_SUPS_UNPARSED_L3_DATA before using this
    macro.
*/
#define CM_MAX_SUPS_PARAMETER_LENGTH         260


/* Bit masks of fundamental call types. Combination of these can be generated
** by using ADD_CALL_TYPE_MASK.
*/

#define CM_CALL_TYPE_NONE_MASK    ((cm_call_type_mask_type)0)
  /**< Bitmask for call type None. */

#define CM_CALL_TYPE_VOICE_MASK   (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_VOICE)
  /**< Bitmask for a voice call type. */

#define CM_CALL_TYPE_CS_DATA_MASK (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_CS_DATA)
  /**< Bitmask for a CS data call type. */

#define CM_CALL_TYPE_EMERG_MASK   (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_EMERGENCY)
  /**< Bitmask for an emergency call type. */

#define CM_CALL_TYPE_VT_MASK      (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_VT)
  /**< Bitmask for a VT call type. */

#define CM_CALL_TYPE_VS_MASK      (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_VS)
  /**< Bitmask for a VS call type. */

#define CM_CALL_TYPE_SMS_MASK     (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_SMS)
  /**< Bitmask for an SMS call type. */

#define ADD_CALL_TYPE_MASK(CALL_TYPE1,CALL_TYPE2) (cm_call_type_mask_type) (((uint64)CALL_TYPE1) | ((uint64)CALL_TYPE2))
  /**< Utility to create a combination of call type capabilities. */

/** @} */ /* end_addtogroup COMMON */

/** @addtogroup COMMON
    @{ */

/*---------------------------------------------------------------------------
   Enums
---------------------------------------------------------------------------*/

/** Enumeration for signal type (for GSM/WCDMA).
*/
typedef enum cm_gw_signal_e
{
  CM_SIGNAL_NONE,          /**< &nbsp; */
  CM_SIGNAL_REMOTE_ALERT,  /**< &nbsp; */
  CM_SIGNAL_NETWORK_ALERT, /**< &nbsp; */

/** @cond
*/
  CM_SIGNAL_MAX            /* internal only */
/** @endcond
*/
} cm_gw_signal_e_type;
/*~ SENTINEL cm_gw_signal_e_type.CM_SIGNAL_MAX */

/** Enumeration of lines supported (FEATURE_ALS).
*/
typedef enum cm_als_line_e {

  /** @cond
 */
  CM_ALS_LINE_NONE = -1, /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_ALS_LINE_01,
    /**< Line #1. */

  CM_ALS_LINE_02,
    /**< Line #2. */

  /** @cond
 */
  CM_ALS_LINE_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_als_line_e_type;
/*~ SENTINEL cm_als_line_e_type.CM_ALS_LINE_MAX */

/** Enumeration of line switching status (FEATURE_ALS).
*/
typedef enum cm_als_line_switching_e {

  /** @cond
 */
  CM_ALS_LINE_SWITCHING_NONE = -1,  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_ALS_LINE_SWITCHING_NOT_ALLOWED,
    /**< User is not allowed to switch between lines. */

  CM_ALS_LINE_SWITCHING_ALLOWED,
    /**< User is allowed to switch between lines. */

  /** @cond
 */
  CM_ALS_LINE_SWITCHING_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_als_line_switching_e_type;
/*~ SENTINEL cm_als_line_switching_e_type.CM_ALS_LINE_SWITCHING_MAX */

/** Enumeration of Common PCN Handset Specification (CPHS)
    procedures status (FEATURE_ALS).
*/
typedef enum cm_als_cphs_proc_e {

  /** @cond
 */
  CM_CPHS_PROC_NONE = -1,  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_CPHS_PROC_NOT_ALLOWED,
    /**< CPHS procedures are not supported. */

  CM_CPHS_PROC_ALLOWED,
    /**< CPHS procedures are supported. */

  /** @cond
 */
  CM_ALS_CPHS_PROC_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_cphs_proc_e_type;
/*~ SENTINEL cm_cphs_proc_e_type.CM_ALS_CPHS_PROC_MAX */

/** Enumeration of ALS procedures status (FEATURE_ALS).
*/
typedef enum cm_als_proc_e {

  /** @cond
 */
  CM_ALS_PROC_NONE = -1,  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_ALS_PROC_NOT_ALLOWED,
    /**< ALS procedures are not supported. */

  CM_ALS_PROC_ALLOWED,
    /**< ALS procedures are supported. */

  /** @cond
 */
  CM_ALS_PROC_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_als_proc_e_type;
/*~ SENTINEL cm_als_proc_e_type.CM_ALS_PROC_MAX */

/** Stores the various status codes that the MC can send to the CM regarding
    Mobile Originated (MO) Data Burst Messaging (DBM) messages.
*/
typedef enum cm_dbm_mo_status_e {

  /** @cond
  */
  CM_DBM_MO_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* 0 */
  CM_DBM_MO_OK,
    /**< SMS message has been completed successfully. */

  CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT,
    /**< Hold Origination retry timeout. */

  CM_DBM_MO_HOLD_ORIG,
    /**< MC cannot process MO SMS because Hold Origination is TRUE. */

  CM_DBM_MO_NO_SRV,
    /**< Cannot send SMS message; no service. */

  /* 4 */
  CM_DBM_MO_ABORT,
    /**< MC or CM aborts the SMS request. */

  CM_DBM_MO_NOT_ALLOWED_IN_AMPS,
    /**< Cannot send an SMS message in analog mode. */

  CM_DBM_MO_NOT_ALLOWED_IN_HDR,
    /**< Cannot send data burst message while HDR call is up. */

  CM_DBM_MO_L2_ACK_FAILURE,
    /**< Not receiving L2 ACK from base station. */

  /* 8 */
  CM_DBM_MO_L3_ACK_FAILURE,
    /**< Not receiving L3 ACK from base station. */

  CM_DBM_MO_OUT_OF_RESOURCES,
    /**< E.g., out of memory buffer. */

  CM_DBM_MO_ACCESS_TOO_LARGE,
    /**< Message is too large to be sent over access. */

  CM_DBM_MO_DTC_TOO_LARGE,
    /**< Message is too large to be sent over a DTC. */

  /* 12 */
  CM_DBM_MO_OTHER,
  /**< Any status response other than the above. */

  CM_DBM_MO_ACCT_BLOCK,
    /**< Access control based on call type (service option) is blocked. */

  /** @cond
  */
  CM_DBM_MO_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_dbm_mo_status_e_type;
/*~ SENTINEL cm_dbm_mo_status_e_type.CM_DBM_MO_MAX */

/** Used for call originations to fill in cm_num_s_type.
*/
typedef enum cm_num_type_e {

  /** @cond
  */
  CM_NUM_TYPE_NONE = -1,  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_NUM_TYPE_UNKNOWN = 0,
    /**< Unknown number type. */

  CM_NUM_TYPE_INTERNATIONAL = 1,
    /**< International number. */

  CM_NUM_TYPE_NATIONAL = 2,
    /**< Domestic number. */

  CM_NUM_TYPE_NETWORK_SPECIFIC = 3,
    /**< Network-specific number. */

  CM_NUM_TYPE_SUBSCRIBER = 4,
    /**< Subscriber number. */

  CM_NUM_TYPE_ABBREVIATED = 6,
    /**< Abbreviated number. */

  CM_NUM_TYPE_RESERVED_EXTENSION = 7,
    /**< Reserved for extension. */

  /** @cond
  */
  CM_NUM_TYPE_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_num_type_e_type;
/*~ SENTINEL cm_num_type_e_type.CM_NUM_TYPE_MAX */

/** Used for call originations to fill in cm_num_s_plan.
*/
typedef enum cm_num_plan_e {

  /** @cond
  */
  CM_NUM_PLAN_NONE = -1,  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_NUM_PLAN_UNKNOWN = 0,
    /**< Unknown number plan. */

  CM_NUM_PLAN_ISDN = 1,
    /**< ISDN/Telephony numbering plan (CCITT E.164 and CCITT E.163). */

  CM_NUM_PLAN_DATA = 3,
    /**< Data numbering plan (CCITT X.121). */

  CM_NUM_PLAN_TELEX = 4,
    /**< Telex numbering plan (CCITT F.69). */

  CM_NUM_PLAN_NATIONAL = 8,
    /**< National numbering plan. */

  CM_NUM_PLAN_PRIVATE = 9,
    /**< Private numbering plan. */

  CM_NUM_PLAN_RESERVED_CTS = 11,
    /**< Reserved for CTS. */

  CM_NUM_PLAN_RESERVED_EXTENSION = 15,
    /**< Reserved for extension. */

  /** @cond
  */
  CM_NUM_PLAN_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_num_plan_e_type;
/*~ SENTINEL cm_num_plan_e_type.CM_NUM_PLAN_MAX */

/** Response to a recall indication from the network. Enumeration can
    be used only if CM_API_CCBS is defined.
*/
typedef enum cm_recall_rsp_e {

  /** @cond
  */
    CM_RECALL_RSP_CCBS_NONE = -1,
  /** @endcond
 */

  CM_RECALL_RSP_CCBS_ACCEPT,
    /**< Accept the recall. */

  CM_RECALL_RSP_CCBS_REJECT,
    /**< Reject the recall. */

  CM_RECALL_RSP_CCBS_HOLD_ACCEPT,
    /**< Hold active calls and accept the recall. */

  CM_RECALL_RSP_CCBS_REL_ACCEPT,
    /**< Release active calls and accept the recall. */

  /** @cond
  */
    CM_RECALL_RSP_CCBS_MAX
  /** @endcond
 */

} cm_ccbs_recall_rsp_e_type;
/*~ SENTINEL cm_ccbs_recall_rsp_e_type.CM_RECALL_RSP_CCBS_MAX */

/** Responses that can be received in 5_send_cnf and recall
   indication. Enumeration can be used only if CM_API_CCBS is
   defined.
*/
typedef enum cm_ccbs_cnf_e {

  /** @cond
  */
    CM_CCBS_CNF_NONE       = -1,
  /** @endcond
 */

  CM_CCBS_CNF_RET_RES,
    /**< CCBS-specific information present. */

  CM_CCBS_CNF_RET_ERR,
    /**< SS error. */

  CM_CCBS_CNF_REJ,
    /**< SS request rejected. */

  CM_CCBS_CNF_RECALL_IND,
    /**< Recall indication for CCBS call. */

  /** @cond
  */
    CM_CCBS_CNF_MAX
  /** @endcond
 */

} cm_ccbs_cnf_e_type;
/*~ SENTINEL cm_ccbs_cnf_e_type.CM_CCBS_CNF_MAX */


/** Enumerations for power collapse functionality and to
    indicate which processor the clients are executing on.
*/
typedef enum cm_processor_e {

  /** @cond
  */
  CM_PROCESSOR_NONE   = -1,
  /** @endcond
 */

  CM_PROCESSOR_MODEM,
    /**< The client is executing on the modem processor. */

  CM_PROCESSOR_APPS_1,
    /**< The client is executing on the applications processor. */

  /** @cond
  */
  CM_PROCESSOR_MAX
  /** @endcond
 */

} cm_processor_e_type;
/*~ SENTINEL cm_processor_e_type.CM_PROCESSOR_MAX */

/** Indicates event notification preferences while in power collapse.
*/
typedef enum cm_pwr_collapse_notify_e {

  /** @cond
  */
  CM_PWR_COLLAPSE_NOTIFY_NULL = -1,
  /** @endcond
 */

  CM_PWR_COLLAPSE_NOTIFY_NONE,
    /**< Do not notify anything in power save. */

  CM_PWR_COLLAPSE_NOTIFY_NORMAL,
    /**< Notify all events in power save. */

  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT,
    /**< Notify the default important events as defined by the CM. */

  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM,
    /**< Notify the important events as defined by the clients. */

  /** @cond
  */
  CM_PWR_COLLAPSE_NOTIFY_MAX
  /** @endcond
 */

} cm_pwr_collapse_notify_e_type;
/*~ SENTINEL cm_pwr_collapse_notify_e_type.CM_PWR_COLLAPSE_NOTIFY_MAX */


/** Enumeration for client categories to differentiate between activities
    initiated by the user (or platform application) and the card (SIM/RUIM).
*/
typedef enum cm_orig_client_categ_e
{
  /** @cond
 */
  CM_ORIG_CLIENT_CATEG_NONE = -1,   /* internal */
  /** @endcond
 */

  /* 0 */
  CM_ORIG_CLIENT_CATEG_USER,
  /**< Reserved for user-initiated activity. */

  /* 1 */
  CM_ORIG_CLIENT_CATEG_CARD,
  /**< Reserved for SIM/RUIM card initiated activity. */
  
  /** @cond
 */
  CM_ORIG_CLIENT_CATEG_MAX   /* internal */
  /** @endcond
 */
  
} cm_orig_client_categ_e_type;
/*~ SENTINEL cm_orig_client_categ_e_type.CM_ORIG_CLIENT_CATEG_MAX */


/** Enumeration for the causes of CM call command errors to be sent to clients
    when command errors are detected by the CM.
*/
typedef enum cm_call_cmd_err_cause_e
{
  /** @cond
 */
  CM_CALL_CMD_ERR_CAUSE_NONE = -1,   /* internal */
  /** @endcond
 */

  /* 0 */
  CM_CALL_CMD_ERR_CAUSE_NO_ERROR,
  /**< No error. */

  /* 1 */
  CM_CALL_CMD_ERR_CAUSE_FDN_FAIL,
  /**< Error cause for FDN failure. */

  /* 2 */
  CM_CALL_CMD_ERR_CAUSE_ACL_FAIL,
  /**< Error cause for Access Control List CC failure. */

  /* 3 */
  CM_CALL_CMD_ERR_CAUSE_NO_SUBSC,
  /**< Error cause for Subscription Not Available. */

  /* 4 */
  CM_CALL_CMD_ERR_CAUSE_CARD_CC_FAIL,
  /**< Error cause for SIM rejection. */

  /* 5 */
  CM_CALL_CMD_ERR_CAUSE_OTHER,
  /**< All other causes. */
  
  /* 6 */
  CM_CALL_CMD_ERR_CAUSE_ACL_NO_MATCH,
  /**< Error cause for no LTE APN matching in the Adjacent Channel Leakage
       Ratio (ACLR). */

  /** @cond
 */
  CM_CALL_CMD_ERR_CAUSE_MAX    /* internal */
  /** @endcond
 */
  
} cm_call_cmd_err_cause_e_type;
/*~ SENTINEL cm_call_cmd_err_cause_e_type.CM_CALL_CMD_ERR_CAUSE_MAX */


/** Enumeration for the causes of CM SUPS command errors to be sent to clients
    when command errors are detected by the CM.
*/
typedef enum cm_sups_cmd_err_cause_e
{
  /** @cond
 */
  CM_SUPS_CMD_ERR_CAUSE_NONE = -1,   /* internal */
  /** @endcond
 */

  /* 0 */
  CM_SUPS_CMD_ERR_CAUSE_NO_ERROR,
  /**< No error. */

  /* 1 */
  CM_SUPS_CMD_ERR_CAUSE_FDN_FAIL,
  /**< Error cause for FDN failure. */

  /* 2 */
  CM_SUPS_CMD_ERR_CAUSE_NO_SUBSC,
  /**< Error cause for Subscription Not Available. */

  /* 3 */
  CM_SUPS_CMD_ERR_CAUSE_CARD_CC_FAIL,
  /**< Error cause for SIM rejection. */

  /* 4 */
  CM_SUPS_CMD_ERR_CAUSE_OTHER,
  /**< All other causes. */
  
  /** @cond
 */
  CM_SUPS_CMD_ERR_CAUSE_MAX   /* internal */
  /** @endcond
 */
  
} cm_sups_cmd_err_cause_e_type;
/*~ SENTINEL cm_sups_cmd_err_cause_e_type.CM_SUPS_CMD_ERR_CAUSE_MAX */


/*---------------------------------------------------------------------------
    Datatypes
---------------------------------------------------------------------------*/

/** Client ID type to differentiate CM client objects from one another.
*/
typedef int   cm_client_id_type;

/** Data type for call ID.
*/
typedef uint8 cm_call_id_type;

/** Data type for number presentation indicator. Values are defined by
    CM_PRESENTATION_XXX.
*/
typedef uint8 cm_npi_type;

/** Data type for screening indicator. Values are defined by
    CM_USER_PROVIDED_XXX.
*/
typedef uint8 cm_si_type;

/** Data type for CCBS index. Can be used only if CM_API_CCBS is defined.
*/
typedef uint8 cm_ccbs_indx_type;

/** Data type to carry bitmasks of call types. CM_CALL_TYPE_x_MASK are the
    defines that go with this type. Clients must check for
    CM_API_VIDEOSHARE before using this type.
*/
typedef uint64 cm_call_type_mask_type;

/** Data type for the emergency service category.
*/
typedef uint32 cm_emerg_srv_categ_type;

typedef uint8 cm_con_ref_type;
/**<  Data type for connection reference uniquely 
 ** identifying the service option connection 
*/

/** @} */ /* end_addtogroup COMMON Common Definitions */

/*-----------------------------------------------------------------------------

Call Object

-----------------------------------------------------------------------------*/

/** @addtogroup CALL Call-Related Definitions
    @{ */

/*---------------------------------------------------------------------------
** Defines
**-------------------------------------------------------------------------*/

#define MAX_DISPLAY_TEXT_LEN       255
  /**< The maximum display text length. */

#define CM_CALL_CMD_PARAM_DEFAULT_VALUE      0
  /**< Initial value for all call command parameter buffers. */

#define CM_MAX_EPS_BEARERS_CONTEXTS ( SYS_MAX_EPS_BEARERS_CONTEXTS - \
                                      SYS_MAX_PRIMARY_PDP_CONTEXTS )
  /**< The maximum number of LTE Evolved Packet System (EPS) bearer contexts. */

/* Define the number of simultaneous calls allowed:
    - For CDMA-only builds, 8 for SVD, 1 otherwise.
    - For GSM/WCDMA or Multimode, 8 always.  
*/

#define CM_CALL_ID_UNASSIGNED       ((cm_call_id_type) 0xFE)
  /**< Call ID unknown at this time. */

#define CM_CALL_ID_MAX              ( 6 + SYS_MAX_PDP_CONTEXTS + \
                                      CM_MAX_EPS_BEARERS_CONTEXTS )
  /**< The maximum number of simultaneous calls allowed. 
       - CDMA -- Voice/test voice, data/test data, SMS, GPS, OTASP, OTAPA
       - GWL -- Six voice + N PS (or EPS bearer contexts in LTE-only mode). */

#define CM_CALL_ID_INVALID          ((cm_call_id_type) 0xFF)
  /**< Invalid call ID. */

  /* internal No call id */

#define CM_MAX_DIALED_DIGITS  36
  /**< Maximum length of dialed digit string. This should be same as 
       NV_MAX_DIAL_DIGITS. This is hardcoded here to eliminate dependency 
       on nv.h for remote processors. */

#define CM_NULL_CON_REF  0
  /**< NULL value for connection reference */

/*
** Subaddress values
*/

#define CM_MAX_SUBADDRESS_DIGITS   21
  /**< Maximum length of the subaddress digit string. */

#define CM_MAX_CALLER_ID_CHARS      32
  /**< Maximum length of the caller ID string. */

#define CM_MAX_CALLED_INFO_CHARS    81
  /**< 3GPP TS 24.008 (Rel 5), section 10.5.4.7, specifies the called party
       BCD number to 40 BCD digits (80 bytes). The UI can supply the CM with
       a "+" in the beginning (1 byte) for international numbers.
       CAUTION: An x increase in value of INFO_CHARS increases the
       call object by nearly 8x times. */

#define CM_MAX_CALLED_INFO_CHARS_NON_UMTS    65
  /**< Maximum length of the connected number string CM_MAX_DATA_DIGITS. */

#define CM_MAX_NUMBER_CHARS         CM_MAX_CALLED_INFO_CHARS
  /**< Maximum length of the CM number string. This is the maximum of 
       CM_MAX_DIALED_DIGITS, CM_MAX_CALLER_ID_CHARS,
       CM_MAX_CALLED_INFO_CHARS, and CM_MAX_CALLED_INFO_CHARS. */

#define CM_MAX_NUMBER_CHARS_NON_UMTS     CM_MAX_CALLED_INFO_CHARS_NON_UMTS
  /**< Maximum length of the CM number string. This is the maximum of 
       CM_MAX_DIALED_DIGITS, CM_MAX_CALLER_ID_CHARS,
       CM_MAX_CALLED_INFO_CHARS, and CM_MAX_CALLED_INFO_CHARS. */

#define CM_MAX_ALPHA_LEN_FOR_CALL_ORIG      90
  /**< Maximum number of characters for the alpha tag on call origination. The
       size was increased to support SIM-related GCF test cases, where a
       string of up to size 90 can be required to be displayed on the UE. */

#define CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO   182
  /**< CM_MAX_DATA_DIGITS. Same as for CM_MAX_ALPHA_LEN_FOR_CALL_ORIG. Call
       events deliver an alpha string to the UI, so they must carry as much as 
       the origination can give. */

#define CM_MAX_ALPHA_TAG_CHARS      CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO
  /**< Maximum length of the CM alpha tag string. This is the maximum of 
       CM_MAX_ALPHA_LEN_FOR_CALL_ORIG and CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO. */

#define CM_MAX_ALPHA_TAG_BYTES      160
  /**< Maximum length of the CM alpha tag string in bytes. This is taken to be
       the same as MAX_CNAP_LENGTH, as defined in NAS. */

#define CM_ORIG_QOS_LEN_MAX         31
  /**< Maximum length of the Quality of Service buffer. Should be the same as 
       CAI_ORIG_QOS_LEN_MAX. */

/* Bits 7-6 for presentation indicator */

#define CM_PRESENTATION_ALLOWED    0x00
  /**< Presentation allowed. */

#define CM_PRESENTATION_RESTRICTED 0x01
  /**< Presentation restricted. */

#define CM_NUMBER_NOT_AVAILABLE    0x02
  /**< Number not available. */

/* Presentation Indicator */

#define CM_PRESENTATION_IND_MASK   0x60
  /**< Presentation indicator  (octet 3a). */

/* Screening Indicator */

#define CM_SCREENING_IND_MASK       0x03
  /**< Screening indicator (octet 3a). */

/* Bits 2-1 */

#define CM_USER_PROVIDED_NOT_SCRND  0
  /**< User-provided, not screened. */

#define CM_NUM_PLAN_MASK            0x0F
  /**< Numbering plan identification. */

/* Type of number */

#define CM_TON_MASK                 0x70
/**< Type of Number (octet 3). */

/* Bits 7-5 */

#define CM_TON_UNKNOWN              (0x00 << 4)
  /**< Type of Number unknown. */

#define CM_TON_INTERNATIONAL        (0x01 << 4)
  /**< International number. */

#define CM_TON_NATIONAL             (0x02 << 4)
  /**< National number. */

#define CM_TON_NETWORK_SPECIFIC     (0x03 << 4)
  /**< Network-specific number. */

  /* Dedicated access, short code */

#define CM_TON_RESERVED_5           (0x05 << 4)
  /**< Reserved. */

  /* Numbering Plan Identification (octet 3) */

#define CM_NPI_UNKNOWN              0x00
  /**< Numbering Plan Identification unknown. */

#define CM_NPI_ISDN                 0x01
  /**< ISDN/telephony numbering plan. */

#define CM_NPI_DATA                 0x03
  /**< Data numbering plan. */

#define CM_NPI_TELEX                0x04
  /**< Telex numbering plan. */

  /* National numbering plan */

#define CM_NPI_PRIVATE              0x09
  /**< Private numbering plan. */

  /* Reserved for CTS */

#define CM_NPI_RESERVED_EXTENSION   0x0f
  /**< Reserved for extension. */

#define CM_MAX_CCP_LEN              16


#define CM_SMS_ADDRESS_MAX          32

#define CM_SMS_MAX_LEN              255
  /**< Mask used to decide on Unstructured Supplementary Services Data (USSD)
       coding scheme. Clients must check for CM_API_USSD_DCS before using this
       macro. */

#define CM_CCBS_INDX_INVALID       ((uint8) 0xFF)
  /**< Invalid CCBS index. Can be used only if CM_API_CCBS is defined. */

#define CM_MAX_CCBS_REQ            5  /* see 24.093 fig 4.5.3 NOTE */
  /**< Maximum number of CCBS indexes possible. Do not use define
       CM_MAX_CCBS_REQ MAX_CCBS_FEATURE_LIST_SIZE, because then it must be
       featurized under GSM. Can be used only if CM_API_CCBS is defined. */

#define CM_MAX_DO_NEIGHBOR_CNT      16 
  /**< The maximum number of Data-Optimized (DO) neighbors of an LTE system that
       is sent for pilot strength measurements to the CM. */

#define CM_MIN_DO_NEIGHBOR_CNT      1 
  /**< The minimum number of DO neighbors of an LTE system that
       is sent for pilot strength measurements to the CM. */

#define CM_MAX_DO_SRCH_WIN          15 
  /**< The maximum search window size allowed by the HDR searcher. */

#define CM_MAX_EMERGENCY_NUM_COUNT 16
  /**< The emergency list IE is a maximum of 50 bytes. One byte of IEI and one
       byte of List Length leaves a maximum of 48 bytes. Each individual
       emergency number will have one byte of length indicator, one byte of type
       indicator (e.g., Hospital or Police), and at least one byte of BCD digits
       for the emergency number. This means that each individual emergency
       number will take at least three bytes. Therefore, the maximum number of
       emergency numbers that can be sent by the network in this IE is 48/3=16.
       CM_MAX_EMERGENCY_NUM_COUNT should be the same as CM_MAX_EMERGENCY_NO_COUNT
       in reg_cm.h. Clients must check for CM_API_EMERGENCY_NUM_LIST before using
       CM_MAX_EMERGENCY_NUM_COUNT and CM_MIN_EMERGENCY_NUM_COUNT.*/

#define CM_MIN_EMERGENCY_NUM_COUNT     1
  /**< See CM_MAX_EMERGENCY_NUM_COUNT. */

/*-------------------------------------------------------------------------*/

/* Clients must check for CM_API_VIDEOSHARE before
   using the following three #defines. */

#define CM_IPAPP_HOST_SIZE                  20
  /**< Size of the host name generating an IP warning.
       CAUTION: An increase of x in HOST_SIZE increases cm_cmd_s_type by 
       CM_CALL_ID_MAX x times. */

#define CM_IPAPP_TEXT_SIZE                  20
  /**< Size of the warning text generated from the IP application.
       CAUTION: An increase of x in TEXT_SIZE increases cm_cmd_s_type by 
       CM_CALL_ID_MAX x times. */

#define CM_MAX_OCTETS_L3_MESSAGE           251
  /**< Maximum size of an L3 message. */

#define CM_MAX_PDN_PACKED_SIZE             255 
  /**< Size of packed PDN data. */

/** @name CDMA 1X Bitmask Values
    @{ */

#define CM_CDMA_INFO_MASK_NONE          SYS_CDMA_INFO_MASK_NONE
#define CM_CDMA_STATE_INFO_BIT_MASK     SYS_CDMA_STATE_INFO_BIT_MASK
#define CM_CDMA_CHANNEL_MEAS_INFO_BIT_MASK \
                                     SYS_CDMA_CHANNEL_MEAS_INFO_BIT_MASK
#define CM_CDMA_ACTIVE_SET_INFO_BIT_MASK \
                                     SYS_CDMA_ACTIVE_SET_INFO_BIT_MASK
#define CM_CDMA_SERV_SYS_INFO_BIT_MASK      SYS_CDMA_SERV_SYS_INFO_BIT_MASK
#define CM_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK \
                                     SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK
#define CM_CDMA_CP_CALL_STAT_BIT_MASK     SYS_CDMA_CP_CALL_STAT_BIT_MASK
#define CM_CDMA_REG_STAT_BIT_MASK          SYS_CDMA_REG_STAT_BIT_MASK
#define CM_CDMA_INFO_MASK_ALL               SYS_CDMA_INFO_MASK_ALL

/** @} */

/** @name HDR Bitmask Values
    @{ */

#define CM_HDR_INFO_MASK_NONE             SYS_HDR_INFO_MASK_NONE
#define CM_HDR_INFO_MASK_ALL                SYS_HDR_INFO_MASK_ALL

/** @} */

/** @name Data Services Bitmask Values
    @{ */

#define CM_DS_INFO_MASK_NONE                SYS_DS_INFO_MASK_NONE
#define CM_DS_INFO_BIT_MASK                 SYS_DS_INFO_BIT_MASK
#define CM_DS_INFO_MASK_ALL                 SYS_DS_INFO_MASK_ALL

/** @} */

/** @name CM Bitmask Values
    @{ */

#define CM_STATS_INFO_MASK_NONE             SYS_CM_INFO_MASK_NONE
#define CM_STATS_PH_PREF_BIT_MASK           SYS_CM_PH_PREF_BIT_MASK
#define CM_STATS_SYS_SEL_INFO_BIT_MASK      SYS_CM_SYS_SEL_INFO_BIT_MASK
#define CM_STATS_PRL_INFO_BIT_MASK          SYS_CM_PRL_INFO_BIT_MASK
#define CM_STATS_ORIG_TERM_STAT_BIT_MASK    SYS_CM_ORIG_TERM_STAT_BIT_MASK
#define CM_STATS_INFO_MASK_ALL              SYS_CM_INFO_MASK_ALL

/** @} */

/** @name NAS Bitmask Values
    @{ */

#define CM_NAS_INFO_MASK_NONE           SYS_NAS_INFO_MASK_NONE          
#define CM_NAS_REGISTRATION_INFO_MASK   SYS_NAS_REGISTRATION_INFO_MASK
#define CM_NAS_ATTACH_TYPE_MASK         SYS_NAS_ATTACH_TYPE_MASK
#define CM_NAS_PERIODIC_TIMER_MASK    SYS_NAS_PERIODIC_TIMER_MASK
#define CM_NAS_CS_REJECT_CAUSE_MASK   SYS_NAS_CS_REJECT_CAUSE_MASK
#define CM_NAS_PS_REJECT_CAUSE_MASK   SYS_NAS_PS_REJECT_CAUSE_MASK
#define CM_NAS_PS_CALL_INFO_MASK        SYS_NAS_PS_CALL_INFO_MASK
#define CM_NAS_INFO_MASK_ALL            SYS_NAS_INFO_MASK_ALL

/** @} */

/** @name WCDMA Bitmask Values
    @{ */

#define CM_WCDMA_INFO_MASK_NONE              SYS_WCDMA_INFO_MASK_NONE
#define CM_WCDMA_SERVING_CELL_INFO_BIT_MASK \
                           SYS_WCDMA_SERVING_CELL_INFO_BIT_MASK
#define CM_WCDMA_SERVING_CELL_POWER_INFO_BIT_MASK \
                           SYS_WCDMA_SERVING_CELL_POWER_INFO_BIT_MASK
#define CM_WCDMA_ACTIVE_SET_REFERENCE_CELL_INFO_BIT_MASK \
                           SYS_WCDMA_ACTIVE_SET_REFERENCE_CELL_INFO_BIT_MASK
#define CM_WCDMA_ACTIVE_SET_INFO_BIT_MASK \
                           SYS_WCDMA_ACTIVE_SET_INFO_BIT_MASK
#define CM_WCDMA_MONITORED_SET_BIT_MASK     SYS_WCDMA_MONITORED_SET_BIT_MASK
#define CM_WCDMA_RRC_STATE                  SYS_WCDMA_RRC_STATE
#define CM_WCDMA_INFO_MASK_ALL              SYS_WCDMA_INFO_MASK_ALL

/** @} */

/** @name GERAN Bitmask Values
    @{ */

#define CM_GERAN_ENG_MODE_NONE             SYS_GERAN_ENG_MODE_NONE
#define CM_GERAN_ENG_MODE_TIMING_ADVANCE \
                                     SYS_GERAN_ENG_MODE_TIMING_ADVANCE
#define CM_GERAN_ENG_MODE_NMR_INFO         SYS_GERAN_ENG_MODE_NMR_INFO
#define CM_GERAN_ENG_MODE_RX_QUAL          SYS_GERAN_ENG_MODE_RX_QUAL
#define CM_GERAN_ENG_MODE_TX_PWR           SYS_GERAN_ENG_MODE_TX_PWR
#define CM_GERAN_ENG_MODE_SCELL_INFO       SYS_GERAN_ENG_MODE_SCELL_INFO
#define CM_GERAN_ENG_MODE_SCELL_MEASUREMENTS \
                                    SYS_GERAN_ENG_MODE_SCELL_MEASUREMENTS
#define CM_GERAN_ENG_MODE_AMR_INFO            SYS_GERAN_ENG_MODE_AMR_INFO
#define CM_GERAN_ENG_MODE_DTX_UL_STATUS    SYS_GERAN_ENG_MODE_DTX_UL_STATUS
#define CM_GERAN_ENG_MODE_TBF_INFO         SYS_GERAN_ENG_MODE_TBF_INFO
#define CM_GERAN_ENG_MODE_NW_PARAMS        SYS_GERAN_ENG_MODE_NW_PARAMS
#define CM_GERAN_ENG_MODE_GPRS_UL_CS       SYS_GERAN_ENG_MODE_GPRS_UL_CS
#define CM_GERAN_ENG_MODE_GPRS_DL_CS       SYS_GERAN_ENG_MODE_GPRS_DL_CS
#define CM_GERAN_ENG_MODE_EGPRS_UL_MCS     SYS_GERAN_ENG_MODE_EGPRS_UL_MCS
#define CM_GERAN_ENG_MODE_EGPRS_DL_MCS     SYS_GERAN_ENG_MODE_EGPRS_DL_MCS
#define CM_GERAN_ENG_MODE_EDGE_MEAS        SYS_GERAN_ENG_MODE_EDGE_MEAS
#define CM_GERAN_ENG_SCELL_CONFIG          SYS_GERAN_ENG_SCELL_CONFIG
#define CM_GERAN_ENG_HOPPING_STATUS        SYS_GERAN_ENG_HOPPING_STATUS
#define CM_GERAN_ENG_MODE_ALL              SYS_GERAN_ENG_MODE_ALL

/** @} */

/** @} */ /* end_addtogroup CALL */

/** @addtogroup CALL
    @{ */

/*---------------------------------------------------------------------------
** Enums
**-------------------------------------------------------------------------*/

/** Enumeration of call commands clients are able to send to the CM.
*/
typedef enum cm_call_cmd_e {

  /** @cond
  */
  CM_CALL_CMD_NONE=-1,  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* 0 */
  CM_CALL_CMD_ORIG,
    /**< Originate a call. */

  CM_CALL_CMD_ANSWER,
    /**< Answer an incoming call. */

  CM_CALL_CMD_END,
    /**< End a call in Originating, Incoming, or Conversation state. */

  CM_CALL_CMD_SUPS,
    /**< Send a call related to a Supplementary Services command, e.g., nn CDMA, 
         which is a FLASH command with information. In WCDMA/GSM, this may be a
         1 SEND command to release the active call and accept the waiting call. */

  /* 4 */
  CM_CALL_CMD_PRIVACY_PREF,
    /**< Change privacy preference for CDMA. */

  CM_CALL_CMD_INFO_GET,
    /**< Get a snapshot of the call information in the CM. */

  CM_CALL_CMD_INFO_LIST_GET,
    /**< Get a snapshot of the call information for all calls in the CM. */

  CM_CALL_CMD_SETUP_RES,
    /**< Response to a setup message for WCDMA/GSM/IP Voice application. */

  /* 8 */
  CM_CALL_CMD_PDP_ACT_RES,
    /**< Response to a PDP activation request for WCDMA/GSM PS. */

  CM_CALL_CMD_PDP_MODIFY,
    /**< Command to modify a PDP for WCDMA/GSM PS. */

  CM_CALL_CMD_RAB_REESTAB,
    /**< Command to re-establish a Reverse Activity Bit (RAB) for WCDMA/GSM PS. */

  CM_CALL_CMD_PS_DATA_AVAILABLE,
    /**< Command to inform the protocol stack that data is available for 
         transmission. */

  /* 12 */
  /** @cond
  */ 
  CM_CALL_CMD_CC_COMPLETE_REORIG,   /* FOR INTERNAL CM USE ONLY!
     Command to inform the CM that call control is complete. */
  /** @endcond
 */

  CM_CALL_CMD_UMTS_CDMA_HANDOVER_REQ,
    /**< Request to handoff UMTS call to 1X. */

  CM_CALL_CMD_UMTS_CDMA_HANDOVER_CNF,
    /**< Status confirmation for UMTS->1X handover. */

  CM_CALL_CMD_RECALL_RSP,
    /**< Client response to recall indication from network. */

  /* 16 */
  CM_CALL_CMD_MO_MODIFY_REQ,
    /**< Client request to modify call type. */

  CM_CALL_CMD_MODIFY_RES,
    /**< Client response to modify request from network. */

  CM_CALL_CMD_HANDOVER_REQ,
    /**< Generic handoff request. */

  CM_CALL_CMD_PS_SIG_REL,
    /**< Command to request the CM to release PS signaling. */

    /* 20 */
  CM_CALL_CMD_PDP_NW_MODIFY_RSP,
    /**< Client response to a modify request for a secondary PDP from the
       network. */
  
  CM_CALL_CMD_ACT_BEARER_RSP,
    /**< Command to respond to an ACT_BEARER_CONTEXT_REQUEST. */

  CM_CALL_CMD_PDN_CONNECTIVITY_REQ,
    /**< Command to request the CM to send PDN_CONNECTIVIRY_REQ to EPS Session
       Management (ESM). */

  CM_CALL_CMD_PDN_CONNECTIVITY_ABORT_REQ,
    /**< Command to request the CM to send PDN_CONNECTIVIRY_ABORT_REQ to ESM. */

  /* 24 */
  CM_CALL_CMD_PDN_DISCONNECT_REQ,
    /**< Command to request the CM to send PDN_DISCONNECT_REQ to ESM. */

  CM_CALL_CMD_RES_ALLOC_REQ,
    /**< Command to request the CM to send RES_ALLOC_REQ to ESM. */

  CM_CALL_CMD_RES_ALLOC_ABORT_REQ,
    /**< Command to request the CM to send RES_ALLOC_ABORT_REQ to ESM. */

  CM_CALL_CMD_BEARER_RES_MOD_REQ,
    /**< Command to request the CM to send BEARER_RES_MOD_REQ to ESM. */

  /* 28 */
  CM_CALL_CMD_MOD_BEARER_RSP,
    /**< Command to respond to MOD_BEARER_CONTEXT_REQUEST. */

  CM_CALL_CMD_INCOMING_PAGE_RSP,
    /**< Command to respond to CM_INCOMING_PAGE_F from 1xcp */

  /** @cond
  */
  CM_CALL_CMD_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_cmd_e_type;
/*~ SENTINEL cm_call_cmd_e_type.CM_CALL_CMD_MAX */

/** Enumeration of call command errors returned by call command
    callback functions.
*/
typedef enum cm_call_cmd_err_e {

  /** @cond
  */
  CM_CALL_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* 0 */
  CM_CALL_CMD_ERR_NOERR,
    /**< No errors found. */

  /* Error codes resulting from clients feeding a call command
     function with bad or inappropriate parameters. */

  CM_CALL_CMD_ERR_CLIENT_ID_P,
    /**< Invalid client ID; NULL or not initialized. */

  CM_CALL_CMD_ERR_CALL_ID_P,
    /**< Invalid call ID in the command. */

  CM_CALL_CMD_ERR_NUM_CALL_IDS_P,
    /**< Number of call IDs specified exceeds the maximum
         number of call IDs allocated. */

  /* 4 */
  CM_CALL_CMD_ERR_CALL_TYPE_P,
    /**< Bad call type parameter. */

  CM_CALL_CMD_ERR_SRV_TYPE_P,
    /**< Bad service type parameter. */

  CM_CALL_CMD_ERR_SRV_OPT_P,
    /**< Bad service option parameter. CDMA only. */

  CM_CALL_CMD_ERR_MODE_PREF_P,
    /**< Requested call type cannot be made in the current mode preference. */

  /* 8 */
  CM_CALL_CMD_ERR_NO_NUM_P,
    /**< An expected number in the number parameter was not provided. */

  CM_CALL_CMD_ERR_NULL_NUM_P,
    /**< Number length is > 0, but the number buffer points to NULL. */

  CM_CALL_CMD_ERR_NUM_P,
    /**< Invalid digits found in the number buffer. */

  CM_CALL_CMD_ERR_NUM_LEN_P,
    /**< Out-of-range value found in the number length field. */

  /* 12 */
  CM_CALL_CMD_ERR_NULL_ALPHA_P,
    /**< Alpha length is > 0, but the alpha buffer points to NULL. */

  CM_CALL_CMD_ERR_ALPHA_LEN_P,                                     /* 10 */
    /**< Out-of-range value found in the alpha length field. */

  CM_CALL_CMD_ERR_MISMATCH_P,
    /**< Call type and parameter type do not match. GW only. */

  CM_CALL_CMD_ERR_NULL_PARAM_P,
    /**< NULL pointer parameter in the API. */

  /* 16 */
  CM_CALL_CMD_ERR_ACTIVATE_CODE_P,
    /**< Invalid OTASP activation code. CDMA only. */

  CM_CALL_CMD_ERR_CALL_SUPS_P,
    /**< Invalid call related to the Supplementary Services parameter. */

  CM_CALL_CMD_ERR_QOS_LEN_P,
    /**< Length of QOS parameters exceed maximum size. */

  CM_CALL_CMD_ERR_LAST_ACT_DATA_NET_P,
    /**< Invalid last active data network. */

  /* 20 */
  CM_CALL_CMD_ERR_INVALID_PRIMARY_CALL_ID_P,
    /**< OBSOLETE with definition of CM_API_SEC_PDP. The primary PDP context's
         call ID is invalid. Sent during a secondary PDP context origination,
         where the specified primary PDP context is invalid. */

  CM_CALL_CMD_ERR_INVALID_GROUP_ID_P,
    /**< Clients check CM_API_SEC_PDP for SEC PDP support. Sent during a
         secondary PDP context origination if the specified group ID is 
         not valid. */

  CM_CALL_CMD_ERR_PROFILE_NUMBER_IN_USE_P,
    /**< The profile is already in use by another PDP context. */

  CM_CALL_CMD_ERR_UMTS_CDMA_HANDOVER_REQ_P,
    /**< Error in a parameter for a UMTS -> 1X handover request. */

  /* 24 */
  CM_CALL_CMD_ERR_UMTS_CDMA_HANDOVER_CNF_P,
    /**< Error in a parameter for UMTS -> 1X handover confirmation. */

  /* Error codes resulting from clients trying to instruct
     CM to perform inappropriate actions relative to the current
     state of the call and/or phone.
  */

  CM_CALL_CMD_ERR_OFFLINE_S,
    /**< Phone is offline. */

  CM_CALL_CMD_ERR_CDMA_LOCK_S,
    /**< Phone is CDMA locked. */

  CM_CALL_CMD_ERR_FLASH_S,
    /**< Client tried to send a flash while a call that does not support
         flash is in progress. CDMA only. */

  /* 28 */
  CM_CALL_CMD_ERR_ORIG_RES_S,
    /**< Dialed number is not allowed under the current origination
         restriction. */

  CM_CALL_CMD_ERR_NO_FUNDS_S,
    /**< Dialed number is not allowed because the phone is out of funds. GW
         only. */

  CM_CALL_CMD_ERR_EMERGENCY_MODE_S,
    /**< Client tried to originate a NON-EMERGENCY call while EMERGENCY mode
         preference is in effect. CDMA only. */

  CM_CALL_CMD_ERR_IN_USE_S,
    /**< Cannot perform the command in question while the phone is in use. */

  /* 32 */
  CM_CALL_CMD_ERR_SRV_TYPE_S,
    /**< Client originated with service type that is currently not
         supported by the phone/PRL. */

  CM_CALL_CMD_ERR_CALL_TYPE_S,
    /**< The command in question is invalid for the call(s) in progress
         (e.g., the client answered with an inappropriate call type relative
         to the type of the incoming call). */

  CM_CALL_CMD_ERR_CALL_STATE_S,
    /**< The command in question is invalid in the current call state
         (e.g., answering a call that was already answered, ending a call 
         that was already ended). */

  CM_CALL_CMD_ERR_SRV_STATE_S,
    /**< The command in question is invalid in the current Phone Serving state
         (e.g., answering a call, but the Serving state is No Service). */

  /* 36 */
  CM_CALL_CMD_ERR_NO_SO_S,
    /**< No valid service found for origination with AUTOMATIC
         or CDMA_AUTOMATIC service type. CDMA only. */

  CM_CALL_CMD_ERR_ALERT_ANS_BYPASS_S,
    /**< Client is not allowed to answer the incoming call. CDMA only. 
         Valid for FEATURE_IS95B_ALERT_ANS_BYPASS. */

  CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S,
    /**< UIM not present. Valid for FEATURE_UIM_RUIM. */

  CM_CALL_CMD_ERR_BAD_PRIVACY_PREF_S,
    /**< Invalid privacy preference. CDMA only. */

  /* 40 */
  CM_CALL_CMD_ERR_NO_CALL_ID_S,
    /**< No call IDs can be allocated. */

  CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S,
    /**< Call origination failed in the lower layer. */

  CM_CALL_CMD_ERR_INVALID_MODE_S,
    /**< Received a command in a mode different from the phone's
         operational mode. */

  CM_CALL_CMD_ERR_INVALID_SIM_STATE_S,
    /**< Call origination failed because the SIM was marked by the network as
         invalid for the circuit and/or the packet service domain. */

  /* 44 */
  CM_CALL_CMD_ERR_MAX_PRIMARY_PDP_CONTEXTS_S,
    /**< The maximum number of primary PDP contexts has been activated. */

  CM_CALL_CMD_ERR_MAX_SEC_PDP_CONTEXT_PER_PRI_S,
    /**< The maximum number of secondary PDP contexts has been reached 
         for this primary PDP context. */

  CM_CALL_CMD_ERR_NO_COLLOC_HDR,
    /**< There is no colocated HDR. */

  /* Error codes resulting from operational limitations of the CM.
  */

  CM_CALL_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  /* 48 */
  CM_CALL_CMD_ERR_MC_COM_L,
    /**< Communication problems with the MC, such as MC buffer shortage. */

  CM_CALL_CMD_ERR_OTHER,
    /**< A problem other than the above was found. */

  CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to a relevant feature being undefined. */

  CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED,
    /**< Call control has rejected the command. */

  /* 52 */
  CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION,
    /**< The CM was not able to determine the mode preference for origination
         for the given call type, service type, and current mode preference. */

  CM_CALL_CMD_ERR_CONNECTION_ID_OUT_L,
    /**< The CM was not able to get a connection ID from the MN or SM. */

  CM_CALL_CMD_ERR_SUPS_SRV_ON_PROHIBITED_LINE_S,
    /**< FEATURE_ALS: Supplementary Services are not allowed for calls that are
         on a prohibited line. A prohibited line is one that is different than
         the currently selected line when line switching is not allowed. */

  CM_CALL_CMD_ERR_USER_DATA_LENGTH_P,
    /**< FEATURE_UUS: Invalid user data length parameter. */

  /* 56 */
  CM_CALL_CMD_ERR_NON_NEGOT_BEARER_P,
    /**< When a call gets connected, the bearer capabilities that both parties
         can support are negotiated and stored in the call object. An MO Call
         Modification request gets rejected by the CM if the bearer for the new
         type requested has not been negotiated during call set up. GSM WCDMA
         only. Clients must check for CM_API_VT_FALLBACK_TO_VOICE definition
         before using this enumeration. */

  CM_CALL_CMD_ERR_INVALID_BC_P,
    /**< The setup response to an MT call setup indication should carry a valid
         BC in BC1 if there is a valid BC in BC2. GSM WCDMA only. Clients must
         check for CM_API_VT_FALLBACK_TO_VOICE definition before using this
         enumeration. */

  CM_CALL_CMD_ERR_INVALID_ORIG_CLIENT_P,   
    /**< Error returned if the call originating client type is not USER or CARD. */

  /** @cond
  */
  CM_CALL_CMD_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_cmd_err_e_type;
/*~ SENTINEL cm_call_cmd_err_e_type.CM_CALL_CMD_ERR_MAX */


/** Enumeration of call events. Clients are able to register
    with the CM to be notified of any subsets of these events.
*/
typedef enum cm_call_event_e {

  /** @cond
  */
  CM_CALL_EVENT_NONE=-1,  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* 0 */
  CM_CALL_EVENT_ORIG,
    /**< Phone originated a call. */

  CM_CALL_EVENT_ANSWER,
    /**< Incoming call was answered. */

  CM_CALL_EVENT_END_REQ,
    /**< Started call-end process. */

  CM_CALL_EVENT_END,
    /**< Originated/incoming call was ended .*/

  CM_CALL_EVENT_SUPS,
    /**< Phone sent Flash/Flash-with-Info to the BS. */

  /* 5 */
  CM_CALL_EVENT_INCOM,
    /**< Phone received an incoming call. */

  CM_CALL_EVENT_CONNECT,
    /**< Originated/incoming call was connected. */

  CM_CALL_EVENT_SRV_OPT,
    /**< Service option changed while in a call. CDMA only. */

  CM_CALL_EVENT_PRIVACY,
    /**< Privacy mode changed while in a call. CDMA only. */

  CM_CALL_EVENT_PRIVACY_PREF,
    /**< Privacy mode preference changed. */

  /* 10 */
  CM_CALL_EVENT_CALLER_ID,
    /**< Caller ID info was received from the BS. CDMA only. */

  CM_CALL_EVENT_ABRV_ALERT,
    /**< CMDA/AMPS abbreviated alert. CDMA only. */

  CM_CALL_EVENT_ABRV_REORDER,
    /**< AMPS abbreviated reorder. CDMA only. */

  CM_CALL_EVENT_ABRV_INTERCEPT,
    /**< AMPS abbreviated intercept. CDMA only. */

  CM_CALL_EVENT_SIGNAL,
    /**< Signal info was received from the BS. */

  /* 15 */
  CM_CALL_EVENT_DISPLAY,
    /**< Display info was received from the BS. CDMA only. */

  CM_CALL_EVENT_CALLED_PARTY,
    /**< Called party info was received from the BS. CDMA only. */

  CM_CALL_EVENT_CONNECTED_NUM,
    /**< Connected number info was received from the BS. */

  CM_CALL_EVENT_INFO,
    /**< Call information. This event is only sent to the client that 
         requests this information through cm_call_cmd_get_call_info. */

  CM_CALL_EVENT_EXT_DISP,
    /**< Extended display was received from the BS. CDMA only. */

  /* 20 */
  CM_CALL_EVENT_NDSS_START,
    /**< Start NDSS redirection. CDMA only. */

  CM_CALL_EVENT_NDSS_CONNECT,
    /**< Call was reconnected due to NDSS. CDMA only. */

  /* The following are for FEATURE_JCDMA. */

  CM_CALL_EVENT_EXT_BRST_INTL,
    /**< Extended burst type; international. */

  CM_CALL_EVENT_NSS_CLIR_REC,
    /**< National Supplementary Services; Calling Line Identity Restriction
        (CLIR). */

  CM_CALL_EVENT_NSS_REL_REC,
    /**< National Supplementary Services; release. */

  /* 25 */
  CM_CALL_EVENT_NSS_AUD_CTRL,
    /**< National Supplementary Services; audio control. */

  CM_CALL_EVENT_L2ACK_CALL_HOLD,
    /**< Call hold. */

  /* The following are for WCDMA/GSM, except 
     CM_CALL_EVENT_SETUP_RES, which is also used by the IP application. */

  CM_CALL_EVENT_SETUP_IND,
    /**< Phone received a setup indication message from the BS. */

  CM_CALL_EVENT_SETUP_RES,
    /**< A setup response was sent (also used for IP call). */

  CM_CALL_EVENT_CALL_CONF,
    /**< The call origination has been accepted. */

  /* The following are for WCDMA/GSM PS Data. */

  /* 30 */
  CM_CALL_EVENT_PDP_ACTIVATE_IND,
    /**< Phone received an incoming PDP call. */

  CM_CALL_EVENT_PDP_ACTIVATE_RES,
    /**< A response to an incoming PDP was sent. */

  CM_CALL_EVENT_PDP_MODIFY_REQ,
    /**< A PDP modify request was sent. */

  CM_CALL_EVENT_PDP_MODIFY_IND,
    /**< The phone received a PDP modify indication from the BS. */

  CM_CALL_EVENT_PDP_MODIFY_REJ,
    /**< The phone received a PDP modify rejection from the BS. */

  /* 35 */
  CM_CALL_EVENT_PDP_MODIFY_CONF,
    /**< The phone received a PDP modify confirmation from the BS. */

  CM_CALL_EVENT_RAB_REL_IND,
    /**< The phone received an RAB release indication from the BS. */

  CM_CALL_EVENT_RAB_REESTAB_IND,
    /**< The phone received an RAB re-establish indication from the BS. */

  CM_CALL_EVENT_RAB_REESTAB_REQ,
    /**< An RAB re-establish request was sent. */

  CM_CALL_EVENT_RAB_REESTAB_CONF,
    /**< The phone received an RAB re-establish confirmation from the BS. */

  /* 40 */
  CM_CALL_EVENT_RAB_REESTAB_REJ,
    /**< The phone received an RAB re-establish rejection from the BS. */

  CM_CALL_EVENT_RAB_REESTAB_FAIL,
    /**< RAB re-establishment failed. */

  CM_CALL_EVENT_PS_DATA_AVAILABLE,
    /**< A PS data available request was sent. */

  CM_CALL_EVENT_MNG_CALLS_CONF,
    /**< A confirmation for Multi-Party (MPTY) calls. */

  CM_CALL_EVENT_CALL_BARRED,
    /**< Call barred notification. */

  /* 45 */
  CM_CALL_EVENT_CALL_IS_WAITING,
    /**< A call is awaiting notification. */

  CM_CALL_EVENT_CALL_ON_HOLD,
    /**< Call on hold notification. */

  CM_CALL_EVENT_CALL_RETRIEVED,
    /**< Call retrieved notification. */

  CM_CALL_EVENT_ORIG_FWD_STATUS,
    /**< Originated call may be forwarded notification to
         forwarding subscriber. */

  CM_CALL_EVENT_CALL_FORWARDED,
    /**< Call-forwarded notification to forwarding subscriber. */

  /* 50 */
  CM_CALL_EVENT_CALL_BEING_FORWARDED,
    /**< Call being forwarded notification to calling subscriber. */

  CM_CALL_EVENT_INCOM_FWD_CALL,
    /**< Incoming forwarded-call notification to forwarded-to-subscriber. */

  CM_CALL_EVENT_CALL_RESTRICTED,
    /**< Call restricted notification. */

  CM_CALL_EVENT_CUG_INFO_RECEIVED,
    /**< Call forward Closed User Group (CUG) notification. */

  CM_CALL_EVENT_CNAP_INFO_RECEIVED,
    /**< Caller name information notification. */

  /* 55 */
  CM_CALL_EVENT_EMERGENCY_FLASHED,
    /**< A voice call was converted into an emergency call. */

  CM_CALL_EVENT_PROGRESS_INFO_IND,
    /**< Call origination progress indication. */

  CM_CALL_EVENT_CALL_DEFLECTION,
    /**< Call deflection notification. */

  CM_CALL_EVENT_TRANSFERRED_CALL,
    /**< Call transfer notification. */

  CM_CALL_EVENT_EXIT_TC,
    /**< A traffic channel has been torn down. CDMA only. */

  /* 60 */
  CM_CALL_EVENT_REDIRECTING_NUMBER,
    /**< Redirecting a number information record event. CDMA only. */

  CM_CALL_EVENT_PDP_PROMOTE_IND,
    /**< Obsolete with definition of CM_API_SEC_PDP. */

    /* Clients check CM_API_SEC_PDP for SEC PDP support.
       Secondary context being promoted to primary. */

  CM_CALL_EVENT_UMTS_CDMA_HANDOVER_START,
    /**< An event to initiate the UMTS to 1X handoff. */

  CM_CALL_EVENT_UMTS_CDMA_HANDOVER_END,
    /**< An event to indicate the UMTS to 1X handoff is completed.*/

  CM_CALL_EVENT_SECONDARY_MSM,
    /**< Invoke event call back even if the event is from a secondary MSM. The
         CM will not generate any events with this, but it is used by the thin
         client CM layer to call client callbacks for events from a secondary
         MSM. */

  /* 65 */
  CM_CALL_EVENT_ORIG_MOD_TO_SS,
    /**< An Origination command was modified to SS due to Call Control. */

  CM_CALL_EVENT_USER_DATA_IND,
    /**< Indicates user data from the remote side. Clients must check for
         CM_API_USER_USER_DATA before using this event. */

  CM_CALL_EVENT_USER_DATA_CONG_IND,
    /**< Indication from lower layers to stop sending user data. Clients
         must check for CM_API_USER_USER_DATA before using this event. */

  CM_CALL_EVENT_MODIFY_IND,
    /**< The network requested modification of VT to voice, and vice-versa. 
         This event is applicable when CM_API_VT is defined. */

  CM_CALL_EVENT_MODIFY_REQ,
    /**< A request to modify VT to Voice, and vice-versa, is sent. This event
         is only applicable when CM_API_VT is defined. */

  /* 70 */
  CM_CALL_EVENT_LINE_CTRL,
    /**< This event is sent when CM_LINE_CTRL_F report is received from
         the lower layers. 1X only. */

  CM_CALL_EVENT_CCBS_ALLOWED,
    /**< Informs that CCBS is allowed on this call. Clients must
         check for CM_API_CCBS before using this event. */

  CM_CALL_EVENT_ACT_CCBS_CNF,
    /**< Sent after CM_5_SEND_CNF is received from the network. Clients
         must check for CM_API_CCBS before using this event. */

  CM_CALL_EVENT_CCBS_RECALL_IND,
    /**< Sent in response to CM_RECALL_IND from the network. Clients
         must check for CM_API_CCBS before using this event. */

  CM_CALL_EVENT_CCBS_RECALL_RSP,
    /**< Sent after receiving a recall response from a client. Clients
         must check for CM_API_CCBS before using this event. */

  /* 75 */
  CM_CALL_EVENT_CALL_ORIG_THR,
    /**< Call origination throttled. */

  CM_CALL_EVENT_VS_AVAIL,
    /**< A Videoshare call is possible for this voice call. Clients must
         check for CM_API_VIDEOSHARE before using this event. */

  CM_CALL_EVENT_VS_NOT_AVAIL,
    /**< A Videoshare call is not possible for this voice call. Clients 
         must check for CM_API_VIDEOSHARE before using this event. */

  CM_CALL_EVENT_MODIFY_COMPLETE_CONF,
    /**< This event is sent after an in-call modification confirmation 
         has been received by the CM. Clients must check for 
         CM_API_VT_FALLBACK_TO_VOICE before using this event. */

  CM_CALL_EVENT_MODIFY_RES,
    /**< This event is sent to indicate a client's response to an MT in-call
         modification indication. Clients must check for 
         CM_API_VT_FALLBACK_TO_VOICE before using this event. */

  /* 80 */
  CM_CALL_EVENT_CONNECT_ORDER_ACK,
    /**< This event is sent to indicate a client's response to a user-answered
         call. Clients must check for CM_API_CONNECT_ORDER_ACK before
         using this event. */

  CM_CALL_EVENT_TUNNEL_MSG,
    /**< A tunneled message for the call was received. */

  CM_CALL_EVENT_END_VOIP_CALL,
    /**< Event to end a VOIP call. */

  CM_CALL_EVENT_VOIP_CALL_END_CNF,
    /**< Event to confirm a VOIP call end. */

  /* 84 */
  CM_CALL_EVENT_PS_SIG_REL_REQ,
    /**< Started PS signaling release process. */

  /* 85 */
  CM_CALL_EVENT_PS_SIG_REL_CNF,
    /**< Event to notify PS signaling release confirmation. */

  CM_CALL_EVENT_HANDOFF_CALL_ORIG,
    /**< HOM-originated call.  */

    /* 87 */
  CM_CALL_EVENT_HANDOFF_CALL_CONNECT,
    /**<  HOM-originated incoming call was connected. */

  CM_CALL_EVENT_HANDOFF_CALL_END,
    /**<  HOM-originated incoming call was ended. */

  CM_CALL_EVENT_HANDOFF_MANUAL_CALL_ORIG,
    /**<  Manually originated incoming call. */

    /* 90 */
  CM_CALL_EVENT_MPTY_CALL_IND,
    /**<  Multiparty call indication. */

  CM_CALL_EVENT_OTASP_STATUS,
    /**< OTASP status indication. */

    /* 92 */
  CM_CALL_EVENT_PDP_NW_MODIFY_REQ,
    /**< Phone received a PDP modify request from the BS. */

    /* 93 */
  CM_CALL_EVENT_PDP_NW_MODIFY_RSP,
    /**< PDP network modify response was sent. */

   /* 94 */
  CM_CALL_EVENT_ACT_BEARER_IND,
    /**< Activate bearer request. */

    /* 95 */
  CM_CALL_EVENT_MOD_BEARER_IND,
    /**< Modify bearer indication. */

    /* 96 */
  CM_CALL_EVENT_GET_PDN_CONN_IND,
    /**< Get PDN connectivity request indication. */

    /* 97 */
  CM_CALL_EVENT_PDN_CONN_REJ_IND,
    /**< PDN connectivity reject indication. */

    /* 98 */
  CM_CALL_EVENT_PDN_CONN_FAIL_IND,
    /**< PDN connectivity failed indication. */

    /* 99 */
  CM_CALL_EVENT_PDN_DISCONNECT_REJECT_IND,
    /**< PDN disconnect reject indication. */

    /* 100 */
  CM_CALL_EVENT_RES_ALLOC_REJ_IND,
    /**< Bearer resource allocation reject indication. */

    /* 101 */
  CM_CALL_EVENT_RES_ALLOC_FAIL_IND,
    /**< Bearer resource allocation failed indication. */

    /* 102 */
  CM_CALL_EVENT_BEARER_MOD_REJ_IND,
    /**< Bearer resource modification reject indication. */

    /* 103 */
  CM_CALL_EVENT_HANDOVER_COMPLETE_IND,
    /**< Event to indicate CM client about handover completion. */
  
    /* 104 */
  CM_CALL_EVENT_CTXT_TRANSFER_IND,
    /**< Event to indicate call object transfer 
         during LTE to GW redirection/reselection. */

    /* 105 */
  CM_CALL_EVENT_INCOMING_PAGE,
    /**< Event to indicate incoming 1x page indication */

  /** @cond
 */
  CM_CALL_EVENT_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_event_e_type;
/*~ SENTINEL cm_call_event_e_type.CM_CALL_EVENT_MAX */

/** Reason for redirecting a call.
*/
typedef enum cm_redirecting_reason_e {

  /** @cond
  */
  CM_REDIRECTING_REASON_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_REDIRECTING_REASON_UNKNOWN = 0,
    /**< Redirecting reason unknown. */

  CM_REDIRECTING_REASON_CALL_FORWARDING_BUSY = 1,
    /**< Redirecting because call forwarding busy or called DTE busy. */

  CM_REDIRECTING_REASON_CALL_FORWARDING_NO_REPLY = 2,
    /**< Recirecting because call forwarding no reply (Circuit mode only). */

  CM_REDIRECTING_REASON_CALLED_DTE_OUT_OF_ORDER = 9,
    /**< Redircting because called DTE out of order (Packet mode only). */

  CM_REDIRECTING_REASON_CALL_FORWARDING_BY_THE_CALLED_DTE = 10,
    /**< Redirecting because call of forwarding by the called DTE 
         (Packet mode only). */

  CM_REDIRECTING_REASON_CALL_FORWARDING_UNCONDITIONAL = 15,
    /**< Redirecting because of call forwarding unconditional or systematic 
         call redirection. */

  CM_REDIRECTING_REASON_RESERVED,
    /**< Reserved. */

  /** @cond
  */
  CM_REDIRECTING_REASON_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

}cm_redirecting_reason_e_type;
/*~ SENTINEL cm_redirecting_reason_e_type.CM_REDIRECTING_REASON_MAX */

/** @} */ /* end_addtogroup CALL */

/** @addtogroup CALL
    @{ */

/* Mapping the old band name to the new enumeration.
*/
#define CM_ABAND      CM_ACTIVATE_CODE_BC0_ABAND
#define CM_BBAND      CM_ACTIVATE_CODE_BC0_BBAND
#define CM_ABLOCK     CM_ACTIVATE_CODE_BC1_ABLOCK
#define CM_BBLOCK     CM_ACTIVATE_CODE_BC1_BBLOCK
#define CM_CBLOCK     CM_ACTIVATE_CODE_BC1_CBLOCK
#define CM_DBLOCK     CM_ACTIVATE_CODE_BC1_DBLOCK
#define CM_EBLOCK     CM_ACTIVATE_CODE_BC1_EBLOCK
#define CM_FBLOCK     CM_ACTIVATE_CODE_BC1_FBLOCK

/** @} */ /* end_addtogroup CALL */

/** @addtogroup CALL
    @{ */

/** OTASP activation code. For CDMA only.
*/
typedef enum cm_activate_code_e {

  CM_ACTIVATE_CODE_BC0_ABAND = 0,
  /**< Band Class 0, A band. */

  CM_ACTIVATE_CODE_BC0_BBAND = 1,
  /**< Band Class 0, B band. */

  CM_ACTIVATE_CODE_BC1_ABLOCK = 2,
  /**< Band Class 1, A block. */

  CM_ACTIVATE_CODE_BC1_BBLOCK = 3,
  /**< Band Class 1, B block. */

  CM_ACTIVATE_CODE_BC1_CBLOCK = 4,
  /**< Band Class 1, C block. */

  CM_ACTIVATE_CODE_BC1_DBLOCK = 5,
  /**< Band Class 1, D block. */

  CM_ACTIVATE_CODE_BC1_EBLOCK = 6,
  /**< Band Class 1, E block. */

  CM_ACTIVATE_CODE_BC1_FBLOCK = 7,
  /**< Band Class 1, F block. */

  /* Band Class 2 and 3 are not supported */

  CM_ACTIVATE_CODE_BC4_ABAND = 12,
  /**< Band Class 4, A band. */

  CM_ACTIVATE_CODE_BC4_BBAND = 13,
  /**< Band Class 4, B band. */

  CM_ACTIVATE_CODE_BC4_CBAND = 14,
  /**< Band Class 4, C band. */

  CM_ACTIVATE_CODE_BC5_ABAND = 15,
  /**< Band Class 5, A band. */

  CM_ACTIVATE_CODE_BC5_BBAND = 16,
  /**< Band Class 5, B band. */

  CM_ACTIVATE_CODE_BC5_CBAND = 17,
  /**< Band Class 5, C band. */

  CM_ACTIVATE_CODE_BC5_DBAND = 18,
  /**< Band Class 5, D band. */

  CM_ACTIVATE_CODE_BC5_EBAND = 19,
  /**< Band Class 5, E band. */

  CM_ACTIVATE_CODE_BC5_FBAND = 20,
  /**< Band Class 5, F band. */

  CM_ACTIVATE_CODE_BC5_GBAND = 21,
  /**< Band Class 5, G band. */

  CM_ACTIVATE_CODE_BC5_HBAND = 22,
  /**< Band Class 5, H band. */

  CM_ACTIVATE_CODE_BC6 = 23,
  /**< Band Class 6. */

  /* Band Class 7, A Band and B Band are not supported */

  CM_ACTIVATE_CODE_BC7_CBAND = 24,
  /**< Band Class 7, C band. */

  /* Kept for backward compatibility issues for 1H07.
     Needs to be removed once UI picks up BC 18, 19 support.
  */

  CM_ACTIVATE_CODE_BC7_DBAND = 27,
  /**< Band Class 7, D band. */

  CM_ACTIVATE_CODE_BC19_ABAND = 25,
  /**< Band Class 19, A band. */

  CM_ACTIVATE_CODE_BC19_BBAND = 26,
  /**< Band Class 19, B band. */

  CM_ACTIVATE_CODE_BC19_CBAND = 27,
  /**< Band Class 19, C band. */

  CM_ACTIVATE_CODE_BC8 = 28,
  /**< Band Class 8. */

  CM_ACTIVATE_CODE_BC9 = 29,
  /**< Band Class 9. */

  CM_ACTIVATE_CODE_BC10_ABAND = 30,
  /**< Band Class 10, A band. */

  CM_ACTIVATE_CODE_BC10_BBAND = 31,
  /**< Band Class 10, B band. */

  CM_ACTIVATE_CODE_BC10_CBAND = 32,
  /**< Band Class 10, C band. */

  CM_ACTIVATE_CODE_BC10_DBAND = 33,
  /**< Band Class 10, D band. */

  CM_ACTIVATE_CODE_BC10_EBAND = 34,
  /**< Band Class 10, E band. */

  CM_ACTIVATE_CODE_BC11_ABAND = 35,
  /**< Band Class 11, A band. */

  CM_ACTIVATE_CODE_BC11_BBAND = 36,
  /**< Band Class 11, B band. */

  CM_ACTIVATE_CODE_BC11_CBAND = 37,
  /**< Band Class 11, C band. */

  CM_ACTIVATE_CODE_BC11_DBAND = 38,
  /**< Band Class 11, D band. */

  CM_ACTIVATE_CODE_BC11_EBAND = 39,
  /**< Band Class 11, E band. */

  CM_ACTIVATE_CODE_BC11_FBAND = 40,
  /**< Band Class 11, F band. */

  CM_ACTIVATE_CODE_BC12_ABAND = 41,
  /**< Band Class 12, A band. */

  CM_ACTIVATE_CODE_BC12_BBAND = 42,
  /**< Band Class 12, B band. */

  /* Band Class 14, A Block to F Block, has the same activation code
     and the same frequency as Band Class 1.
     So we will use Band Class 1 for it. */

  CM_ACTIVATE_CODE_BC14_GBLOCK = 57,
  /**< Band Class 14, G block. */

  CM_ACTIVATE_CODE_BC15_ABAND = 58,
  /**< Band Class 15, A band. */

  CM_ACTIVATE_CODE_BC15_BBAND = 59,
  /**< Band Class 15, B band. */

  CM_ACTIVATE_CODE_BC15_CBAND = 60,
  /**< Band Class 15, C band. */

  CM_ACTIVATE_CODE_BC15_DBAND = 61,
  /**< Band Class 15, D band. */

  CM_ACTIVATE_CODE_BC15_EBAND = 62,
  /**< Band Class 15, E band. */

  CM_ACTIVATE_CODE_BC15_FBAND = 63,
  /**< Band Class 15, F band. */

  CM_ACTIVATE_CODE_BC16_ABAND = 64,
  /**< Band Class 16, A band. */

  CM_ACTIVATE_CODE_BC16_BBAND = 65,
  /**< Band Class 16, B band. */

  CM_ACTIVATE_CODE_BC16_CBAND = 66,
  /**< Band Class 16, C band. */

  CM_ACTIVATE_CODE_BC16_DBAND = 67,
  /**< Band Class 16, D band. */

  CM_ACTIVATE_CODE_BC18_DBAND = 68,
  /**< Band Class 18, D band. */

  CM_ACTIVATE_CODE_BC18_PSB_BAND = 69,
  /**< Band Class 18, PSB band. */

  /* Activation code 70 - 99 is reserved */

  /** @cond
  */
  CM_PREFERRED  = 100,
  /* Used internally. PREFFERRED is currently mapped to BC1 A block.
      This is only used for non-standard OTASP calls. */
  /** @endcond
  */

  CM_OTASP_ACT_CODE_NONE = 101
  /**< No activation code. */

} cm_activate_code_e_type;

/** Enumeration of OTASP states. While an OTASP call is in progress, Call
    Protocol informs the CM of the current state via this enumeration.
*/
typedef enum cm_otasp_status_e {

  /** @cond
  */
  CM_OTASP_STATUS_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

  CM_OTASP_STATUS_SPL_UNLOCKED,
    /**< SPL unlocked. Only for user-initiated OTASP. */

  CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED,
    /**< SPC retries exceeded. Only for user-initiated OTASP. */

  CM_OTASP_STATUS_AKEY_EXCHANGED,
    /**< A-key exchanged. Only for user-initiated OTASP. */

  /* 3 */
  CM_OTASP_STATUS_SSD_UPDATED,
    /**< SSD updated. For both user-initiated OTASP and 
         network-initiated OTASP(OTAPA). */

  CM_OTASP_STATUS_NAM_DOWNLOADED,
    /**< NAM downloaded. Only for user-initiated OTASP. */

  CM_OTASP_STATUS_MDN_DOWNLOADED,
    /**< Mobile Directory Number (MDN) downloaded. Only for 
         user-initiated OTASP. */

  CM_OTASP_STATUS_IMSI_DOWNLOADED,
    /**< IMSI downloaded. Only for user-initiated OTASP. */

  /* 7 */
  CM_OTASP_STATUS_PRL_DOWNLOADED,
    /**< Preferred Roaming List (PRL) downloaded. Only for 
         user-initiated OTASP. */

  CM_OTASP_STATUS_COMMITTED,
    /**< Commit successful. Only for user-initiated OTASP. */

  CM_OTASP_STATUS_OTAPA_STARTED,
    /**< OTAPA started. Only for network-initiated OTASP(OTAPA). */

  CM_OTASP_STATUS_OTAPA_STOPPED,
    /**< OTAPA stopped. Only for network-initiated OTASP(OTAPA). */

  /* 11 */
  CM_OTASP_STATUS_OTAPA_ABORTED,
    /**< OTAPA aborted. Only for network-initiated OTASP(OTAPA). */

  /** @cond
  */
  CM_OTASP_STATUS_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_otasp_status_e_type;
/*~ SENTINEL cm_otasp_status_e_type.CM_OTASP_STATUS_MAX */

/** Enumeration of call states. A CM call must be in one of the
    following states at all times.
*/
typedef enum cm_call_state_e {

  /** @cond
  */
  CM_CALL_STATE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* 0 */
  CM_CALL_STATE_IDLE,
    /**< Call is in the Idle state (i.e., no call). */

  CM_CALL_STATE_ORIG,
    /**< Call is in the Origination state. */

  CM_CALL_STATE_INCOM,
    /**< Call is in the Incoming state. */

  CM_CALL_STATE_CONV,
    /**< Call is in the Conversation state. */

  /* 4 */
  CM_CALL_STATE_CC_IN_PROGRESS,
    /**< Call is originating but waiting for call control to complete. */

  CM_CALL_STATE_RECALL_RSP_PEND,
    /**< Call is waiting for recall response from client. */

  /** @cond
  */
  CM_CALL_STATE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_state_e_type;
/*~ SENTINEL cm_call_state_e_type.CM_CALL_STATE_MAX */


/** Enumeration of CM call types. When originating or answering
    a call, clients must specify one of these call types. For
    compatibility with Qualcomm eXtensible Diagnostic Monitor
    (QXDM) database, do not change the following fixed assigned
    values. If new values are added, update the QXDM database
    as well.
*/
typedef enum cm_call_type_e {

  /** @cond
  */
  CM_CALL_TYPE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_CALL_TYPE_VOICE=0,
    /**< Voice call type.\n
         @note1 Answering with this type is only successful if the
         incoming call is a VOICE call type. */

  CM_CALL_TYPE_CS_DATA=1,
    /**< Circuit-Switched data call type -- (for modem, fax, etc., calls).\n
         @note1 Answering with this call is only successful if the
         incoming call is a DATA or VOICE call type (with AMPS service). */

  CM_CALL_TYPE_PS_DATA=2,
    /**< Packet-Switched data call type. */

  CM_CALL_TYPE_SMS=3,
    /**< SMS call type (SMS SO-6 and SO-14). CDMA only.\n
         @note1 Answering with this call type is only successful
         if the incoming call is an SMS call type. */

  CM_CALL_TYPE_PD=4,
    /**< Position Determination call type (SO-35 and 36). CDMA only. This call
         type is used to exchange IS-801 messages. */

  CM_CALL_TYPE_TEST=5,
    /**< Test call type (for Markov, Loopback, etc). CDMA only.\n
         @note1 Answering with this call type is only successful if the incoming
         call is a TEST call type. */

  CM_CALL_TYPE_OTAPA=6,
    /**< OTAPA call type (OTAPA SO-18 and SO-19). CDMA only.\n
         @note1 Answering with this call type is only successful
         if the incoming call is an OTAPA call type. */

  /* The following are call types that are automatically assigned by the call 
     manager when detecting an origination dial string that matches one of the
     special numbers that are stored in the phone book.
     NOTE: These call types should not be used to originate or answer calls.
  */

  CM_CALL_TYPE_STD_OTASP=7,
    /**< Standard OTASP call type. CDMA only. This call type is 
         automatically assigned by the call manager when an origination
         dial string is detected that matches one of the standard OTASP
         numbers that are stored in the phone book.\n
         @note1 This call type should not be used to originate or answer calls. */

  CM_CALL_TYPE_NON_STD_OTASP=8,
    /**< Non-standard OTASP call type. CDMA only. This call type is
         automatically assigned by the call manager when an origination
         dial string is detected that matches one of the non-standard OTASP
         numbers that are stored in the phone book.\n
         @note1 This call type should not be used to originate or answer calls. */

  CM_CALL_TYPE_EMERGENCY=9,
    /**< Emergency call type. This call type is automatically assigned by the 
         CM when detecting an origination dial string that matches one of the
         Emergency numbers that are stored in the phone book. This call type
         can be also used by clients to originate calls.

         @note1 If the CM assigned this call type, the mode preference is forced
         to Emergency before origination. If at the end of the call the phone is
         not in Emergency Callback mode, the mode is automatically restored to
         its original selection. Otherwise, the Emergency mode stays in effect
         until changed by a client or power cycle. */

  CM_CALL_TYPE_SUPS=10,
    /**< A Supplementary Services (SUPS) command is in progress. This call type
         facilitates call control, where a SUPS command can be turned into a
         voice call, or vice-versa. */

  CM_CALL_TYPE_VT=11,
    /**< Video Telephony call type. Currently applicable for GSM/WCDMA only. */

  CM_CALL_TYPE_VT_LOOPBACK,
    /**< Video Telephony Loopback call type. Currently applicable for GSM/WCDMA 
         only. */

  CM_CALL_TYPE_VS,
    /**< Used for Video Share calls. This call type differs from VT in the sense
         that voice does not have to be synchronized with video, and it is
         generally only transmitted in one direction -- from user A to user B.
         Clients must check for CM_API_VIDEOSHARE before using this call
         type. */

  CM_CALL_TYPE_PS_DATA_IS707B,
    /**< IS-707B Addendum call type. CDMA only. Used to suppress 1X
         registration and send an originating (SO-33, DRS=0) message. */

  /** @cond
  */
  CM_CALL_TYPE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_call_type_e_type;
/*~ SENTINEL cm_call_type_e_type.CM_CALL_TYPE_MAX */

/** Call processing layer failure (1X). This is the call processing layer where
    the call failed during origination. CM_CALL_ORIG_ERR_LAYER_OTHER is a lower
    layer failure than CM_CALL_ORIG_ERR_LAYER_L2, and CM_CALL_ORIG_ERR_LAYER_L2
    is a lower layer failure than CM_CALL_ORIG_ERR_LAYER_L3.
*/
typedef enum {
  /** @cond
  */
  CM_CALL_ORIG_ERR_LAYER_NONE = -1, /* CM internal. */
  /** @endcond
  */
  CM_CALL_ORIG_ERR_LAYER_OTHER,  /**< Internal failure or rejected by the BS. */
  CM_CALL_ORIG_ERR_LAYER_L2,        /**< L2 failure. */
  CM_CALL_ORIG_ERR_LAYER_L3,        /**< L3 failure. */
  /** @cond
  */
  CM_CALL_ORIG_ERR_LAYER_MAX        /* CM internal. */
  /** @endcond
  */
} cm_call_orig_fail_layer_e_type;
/*~ SENTINEL cm_call_orig_fail_layer_e_type.CM_CALL_ORIG_ERR_LAYER_MAX */

/** Enumeration of service types. When originating a call, clients must specify
    one of these service types. For compatibility with the QXDM database, do not
    change the following fixed assigned values. If new values are added,
    update the QXDM database.
*/
typedef enum cm_srv_type_e {

  /** @cond
  */
  CM_SRV_TYPE_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

  CM_SRV_TYPE_AMPS=0,
    /**< The call is to be connected over an analog service. The client does NOT
         need to specify a service option when originating with this call type.\n
         @note1 When originating with this service type, the mode preference
         might be forced to Analog Only before origination and returned to
         its original setting at the end of the call. */

  CM_SRV_TYPE_HDR=1,
    /**< The call is to be connected over High Data Rate (HDR) service. Valid
         for FEATURE_HDR. */

  CM_SRV_TYPE_CDMA_SPECIFIC=2,
    /**< The call is to be connected over CDMA service with a specific 
         client-designated CDMA service option. The client must specify
         a service option when originating with this call type.\n
         @note1 When originating with this service type, the mode preference
         might be forced to Digital Only before origination and returned to
         its original setting at the end of the call. */

  CM_SRV_TYPE_CDMA_AUTOMATIC=3,
    /**< The call is to be connected over CDMA service where the CM
         automatically originates with the best available service option for the
         specified call type. The client does NOT need to specify a service
         option when originating with this call type. */

  CM_SRV_TYPE_AUTOMATIC=4,
    /**< The call is to be connected over an available service -- AMPS or CDMA,
         GSM or WCDMA, or HDR. The client does NOT need to specify a service
         option when originating with this service type, except with a packet
         data call. */

  CM_SRV_TYPE_CDMA_HDR=5,
    /**< The call is to be connected over an available service -- CDMA or HDR.
         The client must specify a service option when originating with this
         service type for CDMA. */

  CM_SRV_TYPE_GSM_WCDMA=6,
    /**< The call is to be connected over an available service -- GSM or WCDMA. */

  CM_SRV_TYPE_WCDMA=7,
    /**< The call is to be connected over the WCDMA service. */

  CM_SRV_TYPE_GSM=8,
    /**< The call is to be connected over the GSM service. */

  CM_SRV_TYPE_WLAN=9,
    /**< The call is to be connected over the WLAN service. */
   
  /** @cond
 */
  CM_SRV_TYPE_NON_AUTOMATIC=10,   /* FOR INTERNAL CM USE ONLY! 
  Service type is non automatic. */
  /** @endcond
 */

  CM_SRV_TYPE_LTE=11,
    /**< The call is to be connected over the LTE service. */

  CM_SRV_TYPE_GSM_WCDMA_LTE=12,
    /**< The call is to be connected over an available service -- GSM, WCDMA, or
         LTE. */

  CM_SRV_TYPE_HDR_MORE_PREF=13,
    /**< The call is to be connected over the same or better preferred  HDR
         systems (as compared to the most recent HDR serving system). */

  /** @cond
  */
  CM_SRV_TYPE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_srv_type_e_type;
/*~ SENTINEL cm_srv_type_e_type.CM_SRV_TYPE_MAX */

/** Enumeration of call end statuses (i.e., the reason for ending a call). For
    compatibility with the QXDM database, do not change the following fixed
    assigned values. If new values are added, update the QXDM database.
*/
typedef enum cm_call_end_e {

  /** @cond
  */
  CM_CALL_END_NONE                         = -1, /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* Common */
  CM_CALL_END_OFFLINE                      = 0,
    /**< The phone is offline. */

  /* CDMA */
  CM_CALL_END_CDMA_LOCK                    = 20,
    /**< The phone is CDMA locked until a power cycle occurs. CDMA only. */

  CM_CALL_END_NO_SRV                       = 21,
    /**< The phone has no service. This is for backward compatibility. 
         NO_CDMA_SRV and NO_GW_SRV are mapped to this. */

  CM_CALL_END_FADE                         = 22,
    /**< The call ended abnormally. CDMA only. */

  CM_CALL_END_INTERCEPT                    = 23,
    /**< Received an intercept from the BS -- originating only. CDMA only. */

  CM_CALL_END_REORDER                      = 24,
    /**< Received a reorder from the BS -- originating only. CDMA only. */

  CM_CALL_END_REL_NORMAL                   = 25,
    /**< Received a release from the BS -- no reason given. */

  CM_CALL_END_REL_SO_REJ                   = 26,
    /**< Received a release from the BS -- SO reject. CDMA only. */

  CM_CALL_END_INCOM_CALL                   = 27,
    /**< Received an incoming call from the BS. */

  CM_CALL_END_ALERT_STOP                   = 28,
    /**< Received an alert stop from the BS -- incoming only. CDMA only. */

  CM_CALL_END_CLIENT_END                   = 29,
    /**< The client ended the call. */

  CM_CALL_END_ACTIVATION                   = 30,
    /**< Received an end activation -- OTASP call only. CDMA only. */

  CM_CALL_END_MC_ABORT                     = 31,
    /**< The MC aborted the origination/conversation. CDMA only. */

  CM_CALL_END_MAX_ACCESS_PROBE             = 32,
    /**< Maximum access probes transmitted. CDMA only. */

  CM_CALL_END_PSIST_NG                     = 33,
    /**< Persistence test failure. FEATURE_JCDMA only. CDMA only. */

  CM_CALL_END_UIM_NOT_PRESENT              = 34,
    /**< RUIM is not present. */

  CM_CALL_END_ACC_IN_PROG                  = 35,
    /**< Access attempt already in progress. */

  CM_CALL_END_ACC_FAIL                     = 36,
    /**< Access failure for reasons other than the above. */

  CM_CALL_END_RETRY_ORDER                  = 37,
    /**< Received a retry order -- originating only. IS-2000. CDMA only. */

  CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS      = 38,
    /**< Concurrent service is not supported by the base station. */

  CM_CALL_END_NO_RESPONSE_FROM_BS          = 39,
    /**< No response received from the base station. */

  CM_CALL_END_REJECTED_BY_BS               = 40,
    /**< Call rejected by the base station. CDMA only. */

  CM_CALL_END_INCOMPATIBLE                 = 41,
    /**< Concurrent services requested were not compatible. CDMA only. */

  CM_CALL_END_ACCESS_BLOCK                 = 42,
    /**< Access is blocked by the base station. CDMA only. */

  CM_CALL_END_ALREADY_IN_TC                = 43,
    /**< Corresponds to CM_CALL_ORIG_ERR_ALREADY_IN_TC. */

  CM_CALL_END_EMERGENCY_FLASHED            = 44,
    /**< The call ended because an Emergency call was flashed over this call. 
         CDMA only. */

  CM_CALL_END_USER_CALL_ORIG_DURING_GPS    = 45,
    /**< Used if the CM is ending a GPS call in favor of a user call. */

  CM_CALL_END_USER_CALL_ORIG_DURING_SMS    = 46,
    /**< Used if the CM is ending a SMS call in favor of a user call. */

  CM_CALL_END_USER_CALL_ORIG_DURING_DATA   = 47,
    /**< Used if the CM is ending a data call in favor of an emergency call. */

  CM_CALL_END_REDIR_OR_HANDOFF             = 48,
    /**< Call rejected because of redirection or handoff. */

  CM_CALL_END_ACCESS_BLOCK_ALL             = 49,
    /**< Access is blocked by the base station for all mobile devices. KDDI
         specific. CDMA only. */

  CM_CALL_END_OTASP_SPC_ERR                = 50,
    /**< To support an OTASP SPC error indication. */

  CM_CALL_END_IS707B_MAX_ACC               = 51,
    /**< Maximum access probes for IS-707B call. CDMA only .*/

  /* GSM/WCDMA */
  CM_CALL_END_LL_CAUSE                     = 100,
    /**< Received a reason for ending the call from the lower layer (look in
         cc_cause). WCDMA/GSM only. */

  CM_CALL_END_CONF_FAILED,
    /**< Call origination request failed. WCDMA/GSM only. */

  CM_CALL_END_INCOM_REJ,
    /**< The client rejected the incoming call. WCDMA/GSM only. */

  CM_CALL_END_SETUP_REJ,
    /**< The client rejected the setup_ind. WCDMA/GSM only. */

  /* 104 */
  CM_CALL_END_NETWORK_END,
    /**< The network ended the call (look in cc_cause). WCDMA/GSM only. */

  CM_CALL_END_NO_FUNDS,
    /**< GSM/WCDMA only. */

  CM_CALL_END_NO_GW_SRV,
    /**< Phone has no service. GWM/WCDMA only. */

  CM_CALL_END_NO_CDMA_SRV,
    /**< Phone has no service. 1X only. */

  /* 108 */
  CM_CALL_END_NO_FULL_SRV,
    /**< Full service is unavailable. */

  CM_CALL_END_MAX_PS_CALLS,
    /**< Indicates resources not available to handle a new MO/MT PS call. */

  /* HDR */
  CM_CALL_END_CD_GEN_OR_BUSY               = 150,
    /**< Abort connection setup due to the reception of a ConnectionDeny message
         with a deny code of general or network busy. */

  CM_CALL_END_CD_BILL_OR_AUTH              = 151,
    /**< Abort connection setup due to the reception of a ConnectionDeny message
         with a deny code of billing failure or authentication failure. */

  CM_CALL_END_CHG_HDR                      = 152,
    /**< Change the HDR system due to redirection or PRL not preferred. */

  CM_CALL_END_EXIT_HDR                     = 153,
    /**< Exit HDR  due to redirection or PRL not preferred. */

  CM_CALL_END_HDR_NO_SESSION               = 154,
    /**< No HDR session. */

  /** @cond
  */
  CM_CALL_END_CM_COLLOC_ACQ_FAIL           = 155,
    /* For internal CM use only -- Failed to acquire co-located HDR for origination. */
  /** @endcond
  */

  CM_CALL_END_HDR_ORIG_DURING_GPS_FIX      = 156,
    /**< Used if CM is ending an HDR call origination in favor of a GPS fix. */

  CM_CALL_END_HDR_CS_TIMEOUT               = 157,
    /**< Connection setup timeout. */

  CM_CALL_END_HDR_RELEASED_BY_CM           = 158,
    /**< The CM released an HDR call so that a 1X call can continue. */

  CM_CALL_END_HOLD_DBM_IN_PROG             = 159,
    /**< The CM is holding the HDR origination to allow a 1X SMS to end. */

  CM_CALL_END_OTASP_COMMIT_IN_PROG         = 160,
    /**< The CM will end the call because an OTASP commit is in progress. */

  CM_CALL_END_NO_HYBR_HDR_SRV              = 161,
    /**< Mobile has no Hybrid HDR service. */

  CM_CALL_END_HDR_NO_LOCK_GRANTED          = 162,
    /**< Call ended because HDR did not get the RF Lock. */

  CM_CALL_END_HOLD_OTHER_IN_PROG           = 163,
    /**< The CM will hold the the current call to allow another call to end. */

  CM_CALL_END_HDR_FADE                     = 164,
    /**< HDR releases a call due to fade. */

  CM_CALL_END_HDR_ACC_FAIL                 = 165,
    /**< HDR releases a call due to access failure attempts. */

  /* The following Call Release reasons are specific to VideoTelephony calls. */
  CM_CALL_END_VIDEO_CONN_LOST              = 301,
    /**< The modem released the call after the modem was connected. */

  CM_CALL_END_VIDEO_SETUP_FAILURE          = 302,
    /**< Call setup failed while trying to set up the modem. */

  CM_CALL_END_VIDEO_PROTOCOL_CLOSED        = 303,
    /**< The video protocol closed after the video protocol setup was done. */

  CM_CALL_END_VIDEO_PROTOCOL_SETUP_FAILURE = 304,
    /**< Video protocol setup failed. */

  CM_CALL_END_INTERNAL_ERROR,
    /**< A CM internal error other than any of the above. */

  /* WLAN */
  CM_CALL_END_NO_WLAN_SRV                  = 200,
    /**< Call was ended because no WLAN service was found. */

  CM_CALL_END_VOIP_FAIL_WLAN               = 201,
    /**< VOIP failed on WLAN. */

  /* IP related */
  CM_CALL_END_IP_FAIL                      = 202,
    /**< Call origination on IP failed. To be used only when CM_API_IP_CALL is
         defined. */

  CM_CALL_END_IP_RETRY                     = 203,
    /**< Call must be retried on IP. To be used only when CM_API_IP_CALL is
         defined. */

  CM_IP_CALL_END_EMERG_ORIG                = 204,
    /**< Call ended due to Emergency origination call. To be used only when
         CM_API_IP_CALL is defined. */

  CM_CALL_END_IP_END                       = 205,
    /**< Used only when CM_API_IP_CALL is defined and the call is to be
         ended.  */

  CM_CALL_END_THERMAL_EMERGENCY            = 206,
    /**< Call ended to put phone in thermal emergency. */

  CM_CALL_END_ORIG_THR                     = 401,
    /**< Origination throttled. */

  CM_CALL_END_1XCSFB_SOFT_FAILURE                     = 402,
     /**< 1XCSFB call is ended because of soft failure. */

  CM_CALL_END_1XCSFB_HARD_FAILURE                     = 403,
     /**< 1XCSFB call is ended because of hard failure. */


  /** @cond
  */
  CM_CALL_END_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_end_e_type;
/*~ SENTINEL cm_call_end_e_type.CM_CALL_END_MAX */

/** Enumeration of call mode information types.
*/
typedef enum cm_call_mode_info_e {

  /** @cond
  */
  CM_CALL_MODE_INFO_NONE,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_CALL_MODE_INFO_CDMA,
    /**< Call information is CDMA. */

  CM_CALL_MODE_INFO_GW_CS,
    /**< Call information is GSM/WCDMA, Circuit-Switched. */

  CM_CALL_MODE_INFO_GW_PS,
    /**< Call information is GSM/WCDMA, Packet-Switched. */

  CM_CALL_MODE_INFO_IP,
    /**< Call information is WLAN. To be used only if CM_API_IP_CALL is
         defined. */

  CM_CALL_MODE_INFO_LTE,
    /**< Call information is LTE. */

  CM_CALL_MODE_INFO_GSM_PS,
    /**< Call info is GSM packet switched */

  CM_CALL_MODE_INFO_WCDMA_PS,
    /**< Call info is WCDMA packet switched */

  /** @cond
  */
  CM_CALL_MODE_INFO_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_mode_info_e_type;
/*~ SENTINEL cm_call_mode_info_e_type.CM_CALL_MODE_INFO_MAX */

/** Enumeration of digit modes.
*/
typedef enum cm_digit_mode_e {

  /** @cond
  */
  CM_DIGIT_MODE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_DIGIT_MODE_4BIT_DTMF,
    /**< 4-bit DTMF. */

  CM_DIGIT_MODE_8BIT_ASCII,
    /**< 8-bit ASCII. */

  CM_DIGIT_MODE_SIP_URI,
    /**< Indicates that a buffer in cm_num_s_type carries a SIP URI. Clients
         must check for CM_API_VIDEOSHARE before using this enumeration. */

  /** @cond
  */
  CM_DIGIT_MODE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_digit_mode_e_type;
/*~ SENTINEL cm_digit_mode_e_type.CM_DIGIT_MODE_MAX */

/** Privacy mode during a call. CDMA only.
*/
typedef enum cm_privacy_pref_e {

  /** @cond
  */
  CM_PRIVACY_PREF_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_PRIVACY_PREF_STANDARD,
    /**< Standard privacy. */

  CM_PRIVACY_PREF_ENHANCED,
    /**< Enhanced privacy .*/

  /** @cond
  */
  CM_PRIVACY_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_privacy_pref_e_type;
/*~ SENTINEL cm_privacy_pref_e_type.CM_PRIVACY_PREF_MAX */

/** Retry order delay type (FEATURE_IS2000).
*/
typedef enum cm_retry_type_e {

  CM_RETRY_TYPE_NONE = -1,
    /**< No retry order. */

  CM_RETRY_TYPE_CLEAR_ALL,
    /**< Clear all retry orders. */

  CM_RETRY_TYPE_ORIGINATION,
    /**< Origination retry order. */

  CM_RETRY_TYPE_RESOURCE_REQ,
    /**< Resource request retry order. */

  CM_RETRY_TYPE_SUPP_CH_REQ,
    /**< Supplemental channel request retry order. */

  CM_RETRY_TYPE_RESERVED4,
    /**< Reserved. */

  CM_RETRY_TYPE_RESERVED5,
    /**< Reserved. */

  CM_RETRY_TYPE_RESERVED6,
    /**< Reserved. */

  CM_RETRY_TYPE_RESERVED7,
    /**< Reserved. */

  /** @cond
  */
  CM_RETRY_TYPE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_retry_type_e_type;
/*~ SENTINEL cm_retry_type_e_type.CM_RETRY_TYPE_MAX */

/** Number classification return status.
*/
typedef enum cm_nc_status_e {

  /** @cond
  */
  CM_NC_ERR_NONE=-1,  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_NC_OK,
    /**< No error, request was completed successfully. */

  CM_NC_ERR_NULL_CALL_TYPE,
    /**< Null function pointer for a resolve call type callback function. */

  CM_NC_ERR_NULL_CALL_ALLOWED,
    /**< Null function pointer for a call allowed callback function. */

  CM_NC_ERR_NULL_CC_PROCESSING_FUNC,
    /**< Null function pointer for a call control processing function. */

  CM_NC_ERR_NULL_CC_PREFERENCES_FUNC,
    /**< Null function pointer for a call control preferences function. */

  /** @cond
  */
  CM_NC_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_nc_status_e_type;
/*~ SENTINEL cm_nc_status_e_type.CM_NC_ERR_MAX */

/** Enumeration of supplementary system commands clients are
    able to send to the CM.
*/
typedef enum cm_sups_cmd_e {

  /** @cond
  */
  CM_SUPS_CMD_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* 0 */
  CM_SUPS_CMD_REGISTER,
    /**< Register a Supplementary Services request. */

  CM_SUPS_CMD_ERASE,
    /**< Erase a Supplementary Services request. */

  CM_SUPS_CMD_ACTIVATE,
    /**< Activate a Supplementary Services request. */

  CM_SUPS_CMD_DEACTIVATE,
    /**< Deactivate a Supplementary Services request. */

  /* 4 */
  CM_SUPS_CMD_INTERROGATE,
    /**< Interrogate a Supplementary Services request. */

  CM_SUPS_CMD_PROCESS_USS,
    /**< Process an Unstructured Supplementary Services. */

  CM_SUPS_CMD_REG_PASSWORD,
    /**< Register a Password Supplementary Services request. */

  CM_SUPS_CMD_ABORT,
    /**< Abort a Supplementary Services request. */

  /* 8 */
  CM_SUPS_CMD_RELEASE,
    /**< Release a Supplementary Services request. */

  CM_SUPS_CMD_GET_PASSWORD_RES,
    /**< Get a password response. */

  CM_SUPS_CMD_USS_NOTIFY_RES,
    /**< Unstructured Supplementary Services notify response. */

  CM_SUPS_CMD_USS_RES,
    /**< Unstructured Supplementary Services response. */

  CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES,
    /**< Location Services (LCS) Location Notification Supplementary Services 
         response. */

  CM_SUPS_CMD_LCS_MOLR,
    /**< Location Services LCS Mobile-Oiginated Location Request. */

  CM_SUPS_CMD_LCS_MOLR_COMPLETED,
    /**< Location Services LCS MOLR completed. */

  CM_SUPS_CMD_ERASE_CC_ENTRY,
    /**< Request to deactivate one or all pending Completion of Calls to 
         Busy Subscriber (CCBS) requests at the network. There is no CM API to
         call this request directly (it gets mapped from
         cm_sups_cmd_deactivate()). */

  /** @cond
  */
  CM_SUPS_CMD_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_sups_cmd_e_type;
/*~ SENTINEL cm_sups_cmd_e_type.CM_SUPS_CMD_MAX */

/** Enumeration of a call command Supplementary Services type.
*/
typedef enum cm_call_sups_type_e {

  /** @cond
  */
  CM_CALL_SUPS_TYPE_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB,
    /**< 0 SEND -- Release all held calls or set User Determined User Busy
         (UDUB) for a waiting call. */

  CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL,
    /**< 1 SEND -- Release all active calls and accept another call 
         (held or waiting). */

  CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL,
    /**< 1 X SEND -- Release a specific call X. */

  CM_CALL_SUPS_TYPE_HOLD_CALL,
    /**< 2 SEND -- Place all active calls on hold and accept another call 
         (held or waiting) */

  CM_CALL_SUPS_TYPE_SELECT_CALL_X,
    /**< 2 X SEND -- Place all active calls on hold except X,
         with which communication is still established. */

  CM_CALL_SUPS_TYPE_MULTIPARTY_CALL,
    /**< 3 SEND -- Add a held call to the conversation (multiparty call). */

  CM_CALL_SUPS_TYPE_CALL_DEFLECTION,
    /**< 4 * DN SEND -- Deflect an incoming call. */

  CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER,
    /**< 4 SEND -- Explicit call transfer. */

  CM_CALL_SUPS_TYPE_CALL_FORWARD,
    /**< Call forward. */

  CM_CALL_SUPS_TYPE_EMERGENCY_CALL,
    /**< Indicates an emergency call in CDMA. */

  CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD,
    /**< Activate answer hold in CDMA. */

  CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD,
    /**< Deactivate answer hold in CDMA. */

  CM_CALL_SUPS_TYPE_SIMPLE_FLASH,
    /**< Send as regular flash in CDMA. */

  CM_CALL_SUPS_TYPE_ACT_CCBS_REQ,
    /**< 5 SEND -- Activates the CCBS request. GW only. 
         Response to CCBS_ALLOWED indication from the network. */

  /** @cond
  */
  CM_CALL_SUPS_TYPE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_sups_type_e_type;
/*~ SENTINEL cm_call_sups_type_e_type.CM_CALL_SUPS_TYPE_MAX */

/** Enumeration of call direction.
*/
typedef enum cm_call_direction_e {

  CM_CALL_DIRECTION_NONE,
    /**< Undefined direction. */

  CM_CALL_DIRECTION_MO,
    /**< Mobile Originated call. */

  CM_CALL_DIRECTION_MT,
    /**< Mobile Terminated call. */

  /** @cond
  */
  CM_CALL_DIRECTION_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_direction_e_type;
/*~ SENTINEL cm_call_direction_e_type.CM_CALL_DIRECTION_MAX */

/** Enumeration of source of subscription information.
*/
typedef enum cm_cc_subscription_source_e {

  CM_CC_SUBSCRIPTION_SOURCE_NONE,
    /**< No subscription information. */

  CM_CC_SUBSCRIPTION_SOURCE_SIM_1,
    /**< Subscription information is from SIM 1. */

  CM_CC_SUBSCRIPTION_SOURCE_SIM_2,
    /**< Subscription information is from SIM 2. */

  CM_CC_SUBSCRIPTION_SOURCE_NV,
    /**< Subscription information is from the Non-Volatile memory. */

  /** @cond
  */
  CM_CC_SUBSCRIPTION_SOURCE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_cc_subscription_source_e_type;
/*~ SENTINEL cm_cc_subscription_source_e_type.CM_CC_SUBSCRIPTION_SOURCE_MAX */

/** Enumeration of call control mode preference.
*/
typedef enum cm_cc_mode_pref_e {

  CM_CC_MODE_PREF_NONE       = 0,
    /**< Reject the call. */

  CM_CC_MODE_PREF_ANY,
    /**< Use any mode that is available. */

  CM_CC_MODE_PREF_CDMA_ONLY,
    /**< Use only CDMA to originate the call. */

  CM_CC_MODE_PREF_GW_ONLY,
    /**< Use only GW to originate the call. */

  /** @cond
  */
  CM_CC_MODE_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_cc_mode_pref_e_type;
/*~ SENTINEL cm_cc_mode_pref_e_type.CM_CC_MODE_PREF_MAX */

/** Enumeration of call control status.
*/
typedef enum cm_cc_status_e {

  /** @cond
  */
  CM_CC_STATUS_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* 0 */
  CM_CC_STATUS_SUCCESS,
    /**< Call Control processing successful. */

  CM_CC_STATUS_ERROR,
    /**< Call Control processing error. */

  CM_CC_STATUS_DUPLICATE_REGISTRATION,
    /**< Call Control duplicate registration. */

  CM_CC_STATUS_CLIENT_NOT_REGISTERED,
    /**< Call Control client not registered. */

  /* 4 */
  CM_CC_STATUS_PROFILE_VALUE_NOT_ALLOWED,
    /**< Call Control status profile value is not allowed. */

  CM_CC_STATUS_PROFILE_ALREADY_SENT,
    /**< Call Control profile already sent. */

  CM_CC_STATUS_MEMORY_ERROR,
    /**< Call Control memory error. */

  CM_CC_STATUS_CLIENT_SPACE_FULL,
    /**< Call Control client space full. */

  /* 8 */
  CM_CC_STATUS_BAD_PARAM,
    /**< Call Control status bad parameter. */

  CM_CC_STATUS_NULL_INPUT_PARAM,
    /**< Call Control NULL input parameter. */

  CM_CC_STATUS_PARAM_EXCEED_BOUNDARY,
    /**< Call Control parameter exceeded the boundary. */

  CM_CC_STATUS_INVALID_LENGTH,
    /**< Call Control status invalid length. */

  /* 12 */
  CM_CC_STATUS_EXTRA_PARAM_MISSING,
    /**< Call Control extra parameter missing. */

  CM_CC_STATUS_EXTRA_PARAM_NOT_REQUIRED,
    /**< Call Control extra parameter not required. */

  CM_CC_STATUS_UNSUPPORTED_COMMAND,
    /**< Call Control unsupported command. */

  CM_CC_STATUS_INVALID_COMMAND,
    /**< Call Control invalid command. */

  /* 16 */
  CM_CC_STATUS_INSUFFICIENT_INFO,
    /**< Call Control insufficient information. */

  CM_CC_STATUS_FILE_ACCESS_FAIL,
    /**< Call Control status file access fail. */

  CM_CC_STATUS_NOT_INIT,
    /**< Call Control status not initialized. */

  /** @cond
  */
  CM_CC_STATUS_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_cc_status_e_type;
/*~ SENTINEL cm_cc_status_e_type.CM_CC_STATUS_MAX */

/** Enumeration of call control results.
*/
typedef enum cm_cc_result_e {

  /** @cond
  */
  CM_CC_RESULT_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  /* 0 */
  CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS,
    /**< Call allowed; Call Control did not make any modifications. */

  CM_CC_RESULT_NOT_ALLOWED,
    /**< Call not allowed. */

  CM_CC_RESULT_ALLOWED_BUT_MODIFIED,
    /**< Call allowed, but there were modifications. */

  CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE,
    /**< Call allowed; the call type was changed to voice. */

  /* 4 */
  CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS,
    /**< Call allowed; the call type was changed to SS. */

  CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD,
    /**< Call allowed; the call type was changed to USSD. */

  /** @cond
  */
  CM_CC_RESULT_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_cc_result_e_type;
/*~ SENTINEL cm_cc_result_e_type.CM_CC_RESULT_MAX */


/** Enumeration of call modifications.
*/
typedef enum cm_call_modify_e{

  /** @cond
  */
  CM_CALL_MODIFY_NONE  = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_CALL_MODIFY_NO_CHANGE = 0,
    /**< No modification for this call. */

  CM_CALL_MODIFY_VOCIE = 1,
    /**< Modify the call to Voice Only. */

  CM_CALL_MODIFY_VT    = 2,
    /**< Modify the call to VT. */

  /** @cond
  */
  CM_CALL_MODIFY_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

}cm_call_modify_e_type;
/*~ SENTINEL cm_call_modify_e_type.CM_CALL_MODIFY_MAX */

/** Enumeration of data coding schemes used by USSD. Clients must check for
    CM_API_USSD_DCS before using this enumeration.
*/
typedef enum cm_ussd_dcs_e {

  /** @cond
  */
  CM_USSD_DCS_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_USSD_DCS_7_BIT = 0x00,
    /**< 7-bit data encoding scheme used for USSD. */

  CM_USSD_DCS_8_BIT = 0x04,
    /**< 8-bit data encoding scheme used for USSD. */

  CM_USSD_DCS_UCS2  = 0x08,
    /**< Universal multi-octet character set encoding. Clients
         must check for CM_API_USSD_DCS and CM_API_USSD_UCS2_DCS. */

  CM_USSD_DCS_UNSPECIFIED = 0x0F,
    /**< Data encoding scheme unspecified. */

  /** @cond
  */
  CM_USSD_DCS_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_ussd_dcs_e_type;
/*~ SENTINEL cm_ussd_dcs_e_type.CM_USSD_DCS_MAX */

/** Enumeration of values carried by the handset-based plus
    dial.
*/
typedef enum cm_hs_based_plus_dial_e {

  /** @cond
  */
  CM_HS_BASED_PLUS_DIAL_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_HS_BASED_PLUS_DIAL_DISABLED = 0,
    /**< Handset-based plus code dial disabled. */

  CM_HS_BASED_PLUS_DIAL_ENABLED = 1,
    /**< Handset-based plus code enabled. */

  /** @cond
  */
  CM_HS_BASED_PLUS_DIAL_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_hs_based_plus_dial_e_type;
/*~ SENTINEL cm_hs_based_plus_dial_e_type.CM_HS_BASED_PLUS_DIAL_MAX */


/** Progress states for CM_CALL_EVENT_PROGRESS_INFO_IND. Clients
    must check for CM_API_IPCALL_SS_INFO before using this type.
*/
typedef enum cm_call_prog_info_e {

  /** @cond
  */
  CM_CALL_PROG_INFO_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_CALL_PROG_INFO_REMOTE_ALERT,
    /**< Indicates that the remote party is being alerted for an MO call. */

  /** @cond
  */
  CM_CALL_PROG_INFO_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_call_prog_info_e_type;
/*~ SENTINEL cm_call_prog_info_e_type.CM_CALL_PROG_INFO_MAX */

/** Enumeration type for the default/dedicated bearer in LTE.
*/
typedef enum cm_lte_bearer_type_e {
  
  /** @cond
 */
  CM_LTE_BEARER_TYPE_NONE = -1,   /* internal */
  /** @endcond
 */

  CM_LTE_BEARER_TYPE_DEFAULT,
    /**< Indicates the default bearer. */

  CM_LTE_BEARER_TYPE_DEDICATED,
    /**< Indicates a dedicated bearer. */

  /** @cond
 */
  CM_LTE_BEARER_TYPE_MAX   /* internal */
  /** @endcond
 */

} cm_lte_bearer_type_e_type;
/*~ SENTINEL cm_lte_bearer_type_e_type.CM_LTE_BEARER_TYPE_MAX */

/** @} */ /* end_addtogroup CALL */

/** @addtogroup CALL
    @{ */

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** @brief Type to hold warning information sent by the IP application
    during registration or ending of an IP call. Clients must
    check for CM_API_VIDEOSHARE before using this type.
*/
typedef struct cm_ipapp_warn_info_s {

  boolean   present;
    /**< Whether an IP application warning is present:
         - TRUE  -- Warning is present.
         - FALSE -- Warning is not present. */

  int       cause;
    /**< Warning code SIP RFC 3261, section 20.43. */

  char      hostname[CM_IPAPP_HOST_SIZE];
    /**< Null-terminated host name that generates the warning. */

  char      description[CM_IPAPP_TEXT_SIZE];
    /**< Null-terminated warning text. */

} cm_ipapp_warn_info_s_type;


/** @brief Structure to store CCBS indexes. Clients must check for
    CM_API_CCBS before using this structure.
*/
typedef struct {

    cm_ccbs_indx_type              ccbs_index;
      /**< CCBS index value stored. */

    cm_called_party_bcd_no_s_type  called_party;
      /**< Called a party number for the corresponding call. */

} cm_ccbs_indx_store_s_type;


/** @brief Type for CCBS information returned to clients. Clients must
    check for CM_API_CCBS before using this structure.
*/
typedef struct cm_ccbs_store_info_s {

    int                         cm_ccbs_store_info_len;
      /**< Total entries present in the CCBS store. */

    cm_ccbs_indx_store_s_type   ccbs_indx_store[CM_MAX_CCBS_REQ];
      /**< Carries the CCBS index and called number. */

} cm_ccbs_store_info_s_type;
/*~ FIELD cm_ccbs_store_info_s.ccbs_indx_store VARRAY CM_MAX_CCBS_REQ
    LENGTH cm_ccbs_store_info_s.cm_ccbs_store_info_len */

/** @brief Type for CM signal types.
*/
typedef struct cm_signal_s {

  boolean     is_signal_info_avail;
    /**< Indicates whether signal information is available.
         @note1 If this variable is FALSE, the values found in other
         fields of this structure have no meaning. */

  uint8       signal_type;
    /**< Signal type: Tone signal, ISDN alerting, or IS-54B alerting. */

  uint8       alert_pitch;
    /**< Alert pitch: Medium, high, or low. */

  uint8       signal;
    /**< Indicates which tone signals: ISDN alerting or IS_54B alerting. */

} cm_signal_s_type;


/** @brief Type for subaddress types.
*/
typedef struct cm_subaddress_s
{
  boolean   extension_bit;
    /**< Extension bit. */

  uint8     subadd_type;
    /**< Type of subaddress: NSAP or User. */

  uint8     odd_even_ind;
    /**< Indicator of even or odd bits. */

  uint8     num_char;
    /**< Number of characters in the subaddress array. */

  uint8     chari[CM_MAX_SUBADDRESS_DIGITS];
    /**< Array of subaddress digits. */

} cm_subaddress_s_type;
/*~ FIELD cm_subaddress_s.chari VARRAY CM_MAX_SUBADDRESS_DIGITS
    LENGTH cm_subaddress_s.num_char */

/** @brief Type for a CM number. Able to store a dialed number or a caller ID number.

    Digital calls include CDMA and GSM/WCDMA, but not AMPS.
    - number_type should be of type cm_num_type_e_type.
    - number_plan should be of type cm_num_plan_e_type.

    @note1hang In the case where the digit_mode parameter is set to
    CM_DIGIT_MODE_4BIT_DTMF, the number buffer may only contain
    the 0-9, #, or * characters. In the case where the
    digit_mode parameter is set to CM_DIGIT_MODE_8BIT_ASCII the
    number buffer may contain any 8-bit ASCII characters.
*/
typedef struct cm_num_s {

  uint8                    buf[CM_MAX_NUMBER_CHARS];
    /**< Number buffer. */

  uint8                    len;
    /**< Length of the number stored in num_buf. */

  cm_digit_mode_e_type     digit_mode;
    /**< Origination digit mode. */

  uint8                    number_type;
    /**< Number type. */

  uint8                    number_plan;
    /**< Numbering plan. */

  cm_npi_type              pi;
    /**< Presentation Indicator. */

  cm_si_type               si;
    /**< Screening Indicator. */

  cm_subaddress_s_type     subaddr;
    /**< Subaddress. */

  cm_emerg_srv_categ_type  emerg_srv_categ;
    /**< Holds service category bits that are ORed. These bits come from
         CM_EMERG_NUM_TYPE_xx (e.g., CM_EMERG_NUM_TYPE_POLICE_MASK). Clients
         must check for CM_API_ECATEG_IN_NUM_STRUCT before using this
         field. Applies only when the call type is Emergency. */

} cm_num_s_type;
 
/*~ FIELD cm_num_s.buf VARRAY CM_MAX_NUMBER_CHARS LENGTH cm_num_s.len */

/** @brief Type for redirecting numbers.
*/
typedef struct cm_redirecting_num_s {

  cm_num_s_type                num;
    /**< Number that is being redirected. */

  cm_redirecting_reason_e_type redirecting_reason;
    /**< Optional parameter with the reason for redirection. 
         REDIRECTING_REASON_UNKNOWN if not included. */

}cm_redirecting_num_s_type;

/** @brief Type for CM alpha. Able to store alpha tag text.
*/
typedef struct cm_alpha_s {

  uint8    buf[CM_MAX_ALPHA_TAG_CHARS];
    /**< Alpha buffer. */
  uint8    dcs; 
    /**< 0x04 for GSM Default Char 0x08 for UCS2 formatting */
  uint8    len;
    /**< Length of alpha tag stored in alpha_buf. */

} cm_alpha_s_type;
 
/*~ FIELD cm_alpha_s.buf VARRAY CM_MAX_ALPHA_TAG_CHARS LENGTH cm_alpha_s.len */

/** Type to hold the country code.
*/
typedef uint16 cm_country_code_type;

/** Type to hold the System ID.
*/
typedef uint16 cm_sid_type;

/** @brief Number type for conversion from plus to digits or digits to
    plus.
*/
typedef struct cm_num_details_for_conversion_s {

  cm_hs_based_plus_dial_e_type  hs_based_plus_dial_setting;
     /**< NV flag regarding a handset-based setting; given by
         CM_SS_EVENT_SRV_CHANGED. */

  cm_country_code_type          home_mobile_country_code;
    /**< Home MCC as given by CM_SS_EVENT_SRV_CHANGED. */

  cm_country_code_type          current_mobile_country_code;
    /**< Current MCC as given by CM_SS_EVENT_SRV_CHANGED. */

  cm_call_type_e_type           call_type;
    /**< Call type this number is associated with; Voice or SMS. */

  cm_num_s_type                 num;
     /**< CM number type carrying the number for conversion. */

} cm_num_details_for_conversion_s_type;


/** @brief Type for CM alpha with coding scheme. Able to store:
    - Encoded Calling Name Presentation (CNAP) string.
    - Number of characters in the decoded string.
    - Number of bytes in the encoded data.
    - Coding scheme used.
*/
typedef struct cm_enc_alpha_s {

  byte     buf[CM_MAX_ALPHA_TAG_BYTES];
    /**< Alpha buffer. */

  uint8    num_characters;
    /**< Length in characters of the alpha tag stored in alpha_buf. */

  uint8    num_bytes;
    /**< Length in bytes of the alpha tag stored in alpha_buf. */

  uint8    coding_scheme;
    /**< Coding scheme of the data. */

} cm_enc_alpha_s_type;

 
/*~ FIELD cm_enc_alpha_s.buf VARRAY CM_MAX_ALPHA_TAG_BYTES LENGTH cm_enc_alpha_s.num_bytes */


/** @brief Extended burst type international message (FEATURE_T53).
*/
typedef struct cm_ext_brst_intl_s {

  uint16   mcc;
   /**< Mobile Country Code (fixed '0101010011'). */

  uint8    db_subtype;
   /**< Data Burst subtype (fixed '000001'). */

  uint8    chg_ind;
   /**< Charge indication. */

  uint8    sub_unit;
   /**< Unit call time (1/10 second). */

  uint8    unit;
   /**< Unit call time (second). */

} cm_ext_brst_intl_s_type;


/** @brief National Supplementary Services Calling Line Idenity
    Restriction (CLIR) (FEATURE_T53).
*/
typedef struct cm_nss_clir_rec_s {

  uint8   cause;
    /**< Reason code. */

} cm_nss_clir_rec_s_type;


/** @brief National Supplementary Services audio control (FEATURE_T53).
*/
typedef struct cm_nss_aud_ctrl_s {

  uint8   up_link;
    /**< Audio path from the MS to the BS. */

  uint8   down_link;
    /**< Audio path from the BS to the MS. */

} cm_nss_aud_ctrl_s_type;


/** @brief Retry order information.
*/
typedef struct cm_retry_order_s {

  cm_retry_type_e_type retry_type;
    /**< Retry delay type. */

  uint16               srv_opt;
    /**< Service option. */

  boolean              infinite_delay;
    /**< TRUE if infinite delay; should always be FALSE for origination. */

  uint32               retry_delay_in_ms;
    /**< Only meaningful if infinite_delay is FALSE. */

} cm_retry_order_s_type;


/** @brief Type for holding optional Supplementary Services parameters.
*/
typedef struct cm_call_sups_params_s {

  cm_call_id_type   call_id;
    /**< Optional call ID. */

  cm_num_s_type     sups_number;
    /**< Optional user-supplied number. */

  uint8             cause;
    /**< Optional cause value for GSM/WCDMA N/Nx SEND command. */

} cm_call_sups_params_s_type;


/** @brief Redirected call information.
*/
typedef struct cm_route_dest_number_s
{
  boolean                           present;
    /**< Indicates whether the rest of the members are valid. */

  routing_number_option_s_type      type;
    /**< Routing option for the destination number. */

  cm_called_party_bcd_no_s_type     number[MAXNO_FORWARD_TO_NUMBER];
    /**< Actual number buffer. */

  cm_called_party_subaddress_s_type address;
    /**< Subaddress for the called party. */

} cm_route_dest_number_s_type;


/** @brief Explicit Call Transfer (ECT) indicator.
*/
typedef struct cm_ect_indicator_s
{
  boolean                         present;
    /**< Indicates whether the rest of the members are valid. */

  ect_call_state_s_type           callState;
    /**< State of the call under transfer. */

  cm_route_dest_number_s_type     routeDestNumber;
    /**< Redirected call information. */

} cm_ect_indicator_s_type;

/** @brief Call Complete on Busy Subscriber (CCBS) indication. Clients
    must check for CM_API_CCBS before using this structure.
*/
typedef struct cm_ccbs_indicator_s
{
  boolean            present;
    /**< TRUE if this structure is valid. */

  cm_ccbs_cnf_e_type ccbs_cnf;
    /**< CCBS confirmation:
         - CM_CCBS_CNF_RET_RES -- Check ccbs_indx_present flag and then
           ccbs_indx CM_CCBS_CNF_RET_ERR.
         - CM_CCBS_CNF_REJ -- Look inside ss_error within
           cm_gw_cs_call_info_s_type. */

  boolean            ccbs_indx_present;
    /**< TRUE if CCBS index is present. */

  cm_ccbs_indx_type  ccbs_indx;
    /**< Actual CCBS index. */

} cm_ccbs_ret_res_s_type;

/** @brief Call Supplementary Services information.
*/
typedef struct cm_call_ss_info_s
{
  ie_ss_status_s_type        ss_status;
    /**< Supplementary Service status. */

  ie_ss_code_s_type          ss_code;
    /**< Supplementary Service code. */

  ie_ss_notification_s_type  ss_notification;
    /**< Supplementary Service notification. */

}cm_call_ss_info_s_type;

/** @brief Forward declaration of cm_mm_call_info_s_type for use by the
    event_user_data function.
*/
typedef struct cm_mm_call_info_s cm_mm_call_info_s_type;


/** @brief Used by clients to query for user data signaling messages after 
    receiving a call event related to user data. Clients must check for
    CM_API_USER_USER_DATA before using.
*/
typedef struct {
     cm_user_data_s_type      mt_user_data;
       /**< Data from a remote user in the case of CM_CALL_EVENT_USER_DATA_IND. */

     boolean                  is_receiver_busy;
       /**< Used for CM_CALL_EVENT_USER_DATA_CONG_IND. */

     ie_cm_cc_cause_s_type    cm_cc_cause;
       /**< Used for CM_CALL_EVENT_USER_DATA_CONG_IND. */

} cm_call_event_user_data_s_type;

/** Type for a function to be called by clients to retrieve information
    regarding user-to-user data. Clients must check for CM_API_USER_USER_DATA
    before using this type.
*/
typedef void (cm_call_event_user_data_f_type) (

    cm_call_event_user_data_s_type  *call_event_user_data,
      /**< Structure holding user data corresponding to events. */

    const cm_mm_call_info_s_type    *call_info_ptr
        /**< Pointer to the call information pointer sent to clients. */
);

/** @brief Circuit-Switched call information returned with each event.
*/
typedef struct cm_gw_cs_call_info_s {

  uint8                               transaction_id;
    /**< Call transaction ID. */

  boolean                             accept;
    /**< Accept the call. */

  boolean                             success;
    /**< Confirmation that the message from the lower layer is successful. */

  cm_repeat_indicator_s_type          bc_repeat_indicator;
    /**< Bearer Capability (BC) repeat indicator. Included only if
         bearer capability 1 and bearer capability 2 are included. */

  cm_bearer_capability_s_type         bearer_capability_1;
    /**< Bearer capability 1. */

  cm_bearer_capability_s_type         bearer_capability_2;
    /**< Bearer capability 2. */

  cm_repeat_indicator_s_type          llc_repeat_indicator;
    /**< Low Layer Compatibility (LLC) repeat indicator. Included only if the
         BC repeat indicator and low layer compatibility 1 are included. */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_1;
    /**< Low layer compatibility 1. Included in the Network -> UE direction if 
         the calling user specifies a low layer compatibility. */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_2;
    /**< Low layer compatibility 2. Included only if the LLC repeat indicator
         is included. */

  cm_repeat_indicator_s_type          hlc_repeat_indicator;
    /**< High Layer Compatibility (HLC) repeat indicator. Included only if the 
         BC repeat indicator and high layer compatibility 1 are included. */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_1;
    /**< High layer compatibility 1. Included in the Network -> UE direction 
         if the calling user specifies a high layer compatibility. */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_2;
    /**< High layer compatibility 2. Included only if the HLC repeat indicator 
         is included. */

  cm_called_party_bcd_no_s_type       called_party_bcd_number;
    /**< Called party Binary-Coded Decimal (BCD) number. */

  cm_called_party_subaddress_s_type   called_party_subaddress;
    /**< Called party subaddress. */

  cm_calling_party_bcd_no_s_type      calling_party_bcd_number;
    /**< Calling party BCD number. */

  cm_calling_party_subaddress_s_type  calling_party_subaddress;
    /**< Calling party subaddress. */

  cm_num_s_type                       redirect_party_number;
    /**< Redirecting party number. */

  cm_forward_cug_info_s_type          forward_cug_info;
    /**< Closed User Group information. */

  uint32                              rab_id;
    /**< Radio Access Bearer ID. Valid only if rab_id_present is TRUE. Clients
         must check for CM_API_CS_RABID before using this field. */

  uint8                               cm_clir;
    /**< CLIR information; CLIR suppression, invocation, etc. */

  ie_cm_cc_cause_s_type               cc_cause;
    /**< Call Control cause information. */

  ie_cm_cc_reject_s_type              cc_reject;
    /**< Call Control rejection information. */

  ie_cm_mo_call_details_s_type        call_details;
    /**<  Bearer capability information for mobile-originated calls. */

  ie_cm_ss_error_s_type               ss_error;
    /**< Serving System error. */

  boolean                             call_ss_success;
    /**< Flag to indicate if multiple call management is successful. */

  active_calls_list_s_type            active_calls_list;
    /**< Active call ID list from multiple calls management confirmation. */

  cm_connected_no_s_type              connected_num;
    /**< Connected number. */

  cm_connected_subaddress_s_type      connected_subaddress;
    /**< Connected subaddress. */

  cm_call_sups_type_e_type            sups_type;
     /**< Supplementary Services type. */

  cm_call_sups_params_s_type          sups_params;
    /**< Supplementary Services-specific data. */

  uint8                               call_progress;
    /**< Call origination progress information. */

  cm_cause_of_no_cli_s_type           cause_of_no_cli;
    /**< Cause of no Calling Line Information (CLI) in an MT call. */

  cm_ect_indicator_s_type             ect_info;
    /**< Explicit Call Transfer information. */

  cm_call_ss_info_s_type              ss_info;
    /**< Call-related SS information. */

  sys_is2000_msg_s_type               umts_cdma_ho_info;
    /**< UMTS to 1X handoff information. */

  cm_call_event_user_data_f_type      *user_data_func;
    /**< Structure holding user data corresponding to event functions. */

  boolean                             is_user_data_present;
    /**< Can be used by clients to check if user_data_func
         can be called to return valid values. */

  cm_ccbs_ret_res_s_type              ccbs_ret_res;
    /**< Carries CCBS-specific parameters for the clients. Clients
         must check for CM_API_CCBS before using this structure. */

  cm_ccbs_recall_rsp_e_type           ccbs_recall_rsp;
    /**< Carries recall responses sent by the client for recall
         indication. Clients must check for CM_API_CCBS before
         using this structure. */

  cm_call_type_e_type                 new_call_type_req;
    /**< New call type that is requested by in-call modification API 
         cm_mm_call_cmd_modify_req(). Clients must check for 
         CM_API_VT_FALLBACK_TO_VOICE before using this field. */

  boolean                             incall_modif_err_status;
    /**< Error status returned by the MN as a result of an in-call modification
         request:
         - TRUE -- Error in processing the in-call modification request.
         - FALSE -- No error in processing the in-call modification request. \n

         Clients must check for CM_API_VT_FALLBACK_TO_VOICE before using this 
         field. */

  boolean                             is_call_mod_possible;
    /**< Used by UI clients to display icons that can be chosen by the user to
         perform call modifications:
         - TRUE -- Call can be modified (Voice to VT and VT to Voice).
         - FALSE -- Call cannot be modified. \n

         Clients must check for CM_API_VT_FALLBACK_TO_VOICE before using this 
         field. */

  boolean                             gw_cs_modify_res;
    /**< Client response to a modification request:
         - TRUE -- Modification was accepted by the client.
         - FALSE -- Modification was rejected by the client.\n

         Clients must check for CM_API_VT_FALLBACK_TO_VOICE before using this 
         field. */

  boolean                             rab_id_present;
    /**< RAB ID validation:
         - TRUE -- rab_id carries a valid value.
         - FALSE -- rab_id does not carry a valid value. \n

         Clients must check for CM_API_CS_RABID before using this field. */

  boolean                             is_network_initiated;
    /**< Used by UI clients to tailor messages/prompts indicating whether the
         network or a remote user is responsible for this event:
         - TRUE -- Network-initiated event.
         - FALSE -- Remote user-initiated event. \n

         Clients must check for CM_API_VT_FALLBACK_TO_VOICE before using 
         this field. */

  sync_enum_T            gw_handover_cause;
    /**< Handover cause from the GW stack. */    

} cm_gw_cs_call_info_s_type;
/*~ FIELD cm_gw_cs_call_info_s.user_data_func POINTER */


/** @brief Packet-Switched call information returned with each event.
*/
typedef struct cm_gw_ps_call_info_s {

  nsapi_T                         nsapi;
    /**< NSAP information. */

  qos_T                           qos;
    /**< Quality of Service information. */

  qos_T                           minimum_qos;
    /**< The minimum Quality of Service allowed for the PDP context. */

  pdp_address_T                   pdp_addr;
    /**< Packet Data Protocol address. */

  protocol_cfg_options_T          config_options;
    /**< Protocol configuration options. */

  apn_T                           apn_name;
    /**< Access Point Name; a label or a fully-qualified domain name. */

  sys_pdp_header_comp_e_type      h_comp;
    /**< IP header compression required. Clients must check for
         CM_API_PDP_COMPR before using this enumeration. */

  sys_pdp_data_comp_e_type        d_comp;
    /**< Data compression required. Clients must check for CM_API_PDP_COMPR
         before using this enumeration. */

  ti_T                            tear_down_ind;
    /**< Teardown Indicator (TI); indicates if only the PDP context associated
         with this specific TI or all active PDP contexts sharing the same PDP
         address associated with this specific TI shall be deactivated. */

  cause_enum_type_T               pdp_cause_type;
    /**< PDP cause type. */

  pdp_cause_T                     cause;
    /**< PDP cause. */

  rabm_rabid_T                    rabid;
    /**< RAB ID. */

  rabmcm_cause_T                  rab_cause;
    /**< RAB cause. */

  cm_call_id_type                 primary_call_id;
    /**< Call ID of the primary PDP context. This field is only used if this is
         a secondary PDP context origination. */

  uint16                          profile_number;
    /**< The PDP profile number used for this PDP context. */

  sys_is2000_msg_s_type           umts_cdma_ho_info;
    /**< UMTS to 1X handoff information. */

  byte                            pdp_group_id;
   /**< Group ID of the primary context that is associated with the
        secondary PDP context origination. Clients must check CM_API_SEC_PDP
        for secondary PDP support. */

 uint16                           assoc_pri_profile_num;
   /**< For primary context, holds its own profile number. For secondary
        context, it holds the profile of an associated primary. Clients must
        check CM_API_SEC_PDP for secondary PDP support. */

 boolean                          is_net_alloc_qos_diff_from_req;
   /**< Secondary PDP origination's requested QoS and minimum_qos decide this
        value:
        - FALSE -- The network has allocated a QoS between the requested and
                   minimum_qos.
        - TRUE -- The network has allocated a different QoS. \n

        Clients must check CM_API_SEC_PDP for secondary PDP support. Valid
        only for secondary PDP context. */

 cm_ps_sig_rel_status_T           ps_sig_rel_status;
   /**< Status of the PS signaling release request that was initiated
        by the client. */

 tft_type_T                       tft;
   /**<  Traffic Flow Template. */

 boolean                          is_primary_pdp;
   /**< Used to inform the client whether the call type is primary or secondary
        PDP.
        - TRUE -- Primary_pdp. 
        - FALSE -- Secondry_pdp. */

} cm_gw_ps_call_info_s_type;
/*~ FIELD cm_gw_ps_call_info_s.cause DISC cm_gw_ps_call_info_s.pdp_cause_type */

/** @brief Response for ACT_BEARER_CONTEXT_REQUEST.
*/
typedef struct cm_act_bearer_rsp_s {

  boolean                    is_accept;
    /**< Result of bearer context request. */

  esm_cause_T                esm_cause;
    /**< PDP cause. */

  lte_nas_local_cause_T      esm_local_cause;
    /**< ESM local cause. */

  sys_pdp_header_comp_e_type h_comp;       
    /**< SNDCP IP header compression required. */
  
  sys_pdp_data_comp_e_type   d_comp;     
    /**< Subnetwork-Dependent Convergence Protocol (SNDCP) data compression
         required. */

  protocol_cfg_options_T     protocol_config_options;
    /**< Protocol configuration options for the bearer. */

} cm_act_bearer_rsp_s_type;

/** @brief Request for PDN connectivity setup.
*/
typedef struct cm_pdn_connectivity_s
{
  word                    sdf_id;
    /**< Service Data Flow ID. */
  
  req_type_T              request_type;
    /**< Connection request type. */
  
  pdn_connection_data_T   pdn_data;
    /**< The payload of the PDN connection message. */
  
} cm_pdn_connectivity_s_type ;


/** @brief Request for PDN connectivity abort.
*/
typedef struct cm_pdn_connectivity_abort_s
{
  word                    sdf_id;
    /**< Service Data Flow ID. */
  
  esm_cause_T             esm_cause;
    /**< ESM cause value for the PDN connectivity message. */
  
  lte_nas_local_cause_T   esm_local_cause;
    /**< ESM local cause value for the PDN connectivity message. */
  
} cm_pdn_connectivity_abort_s_type;

/** @brief Request for PDN disconnect.
*/
typedef struct cm_pdn_disconnect_s
{
  word                     sdf_id;
    /**< Service Data Flow ID. */
  
  eps_bearer_id_T          eps_bearer_id;
    /**< Evolved Packet System bearer ID. */
  
  protocol_cfg_options_T   protocol_config_options;
    /**< Protocol options for an end point. */
  
  lte_nas_local_cause_T    esm_local_cause;
    /**< ESM local cause value for the disconnect message. */
  
} cm_pdn_disconnect_s_type; 


/** @brief Request for bearer resource allocation.
*/
typedef struct cm_res_alloc_s
{
  word                      sdf_id;
    /**< Service Data Flow ID. */
  
  sdf_qos_T                 sdf_qos;
    /**< Service Data Flow QoS. */
  
  tft_type_T                ul_dl_tft;
    /**< Traffic Flow Template filter for resource being allocated. */
  
  protocol_cfg_options_T    protocol_config_options;
    /**< Protocol options for an end point. */
  
  byte                      lbi;
    /**< Linked Evolved Packet System Bearer ID. */
  
} cm_res_alloc_s_type ;


/** @brief Request to abort bearer resource allocation.
*/
typedef struct cm_res_alloc_abort_s
{
  word                    sdf_id;
    /**< Service Data Flow ID. */
  
  esm_cause_T             esm_cause;
    /**< EPS Session Management (ESM) cause value in the abort message. */
  
  lte_nas_local_cause_T   esm_local_cause;
    /**< ESM local cause value in the abort message. */
  
} cm_res_alloc_abort_s_type ;


/** @brief Request for bearer resource modification.
*/
typedef struct cm_res_mod_s
{
  word                    sdf_id;
    /**< Service Data Flow ID. */
  
  byte                    lbi;
    /**< Linked EPS Bearer ID. */
  
  tft_type_T              ul_dl_tft ;
    /**< Traffic Flow Template filter for resource being allocated. */
  
  sdf_qos_T               sdf_qos;
    /**< Service Data Flow QoS parameters. */
  
  esm_cause_T             esm_cause;
    /**< ESM cause in the resource modification message. */
  
  lte_nas_local_cause_T   esm_local_cause;
    /**< ESM local cause in the resource modification message. */
  
  protocol_cfg_options_T  protocol_config_options;
    /**< Protocol options for an end point. */
  
} cm_res_mod_s_type;


/** @brief Response for bearer resource modification.
*/
typedef struct cm_mod_bearer_rsp_s
{
  eps_bearer_id_T           eps_bearer_id;
    /**< Bearer ID in the resource modification response. */
  
  esm_cause_T               esm_cause;
    /**< ESM cause in the resource modification message. */
  
  lte_nas_local_cause_T     esm_local_cause;
    /**< ESM local cause in the resource modification message. */
  
  protocol_cfg_options_T    protocol_config_options;
    /**< Protocol options for an end point. */
  
  boolean                   is_accept;
    /**< Result of resource modification request. */
  
} cm_mod_bearer_rsp_s_type;


/** @brief LTE call information returned with each event.
*/
typedef struct cm_lte_call_info_s {

  eps_bearer_id_T                    eps_bearer_id;
    /**< Evolved Packet System bearer ID. */
  
  byte                               rb_id;
    /**< Resource Block ID for an LTE call. */
  
  byte                               connection_id; 
    /**< Connection ID for an LTE call. */
  
  word                               sdf_id;
    /**< Resource Service Data Flow ID. */
  
  byte                               lbi;
    /**< Linked EPS Bearer ID. */
  
  sdf_qos_T                          sdf_qos;
    /**< Service Data Flow QoS parameters. */
  
  qos_T                              neg_qos;
    /**< Quality of Service information. */
  
  pdn_address_T                      pdn_addr;
    /**< Packet Data Network address. */
  
  protocol_cfg_options_T             config_options;
    /**< Protocol configuration options. */
  
  apn_T                              apn_name;
    /**< Access Point Name -- A label or a fully-qualified domain name. */
  
  apn_ambr_T                         apn_ambr;
    /**< Aggregate Maximum Bitrate (AMBR) for the APN. */
  
  esm_cause_T                        esm_cause;
    /**< ESM cause. */
  
  lte_nas_local_cause_T              esm_local_cause;
    /**< ESM local cause. */
  
  uint16                             profile_number;
    /**< The PDP profile number used for this PDP context. */
  
  byte                               pdp_group_id;
    /**< Clients must check CM_API_SEC_PDP for the secondary PDP support group
         ID of the primary context that is associated with the secondary PDP
         context origination. */
  
  tft_type_T                         ul_dl_tft;
    /**< Trafic Flow Template filter for the resource being allocated. */
  
  llc_sapi_T                         neg_llc_sapi;
    /**< Negative Service Access Point Identifier (SAPI) Logical Link Control
       (LLC). */
  
  cm_lte_bearer_type_e_type          bearer_type;
    /**< Default or dedicated bearer type. */
  
  req_type_T                         request_type;
    /**< Indicates how current request was issued. */
  
  pdn_connection_data_T              pdn_data;
    /**< Connection information between Public Data Network points. */
  
  boolean                            is_accept;
    /**< LTE bearer's response to the call setup request. */
  
  boolean                            result;
    /**< Result of the bearer setup attempt. */
  
  gsm_umts_connection_id_T           gsm_umts_connection_id;
    /**< ID used between GSM and UMTS to transfer call objects. */
  
  sys_pdp_header_comp_e_type         h_comp;
    /**< SNDCP IP header compression required. */
  
  sys_pdp_data_comp_e_type           d_comp;
    /**< SNDCP data compression required. */

} cm_lte_call_info_s_type;


/** @brief Generic Access Network (GAN) status information structure.
*/
typedef struct cm_gw_gan_info_s{

    sys_gw_gan_state_e_type                       gan_state;
      /**< GAN state (initiation or shutdown). */

    sys_gw_gan_conn_status_e_type                 gan_conn_status;
      /**< GAN connection status. */

    sys_gw_gan_sig_chan_status_e_type             gan_sig_chan_status;
      /**< Signal channel status. */

    sys_gw_gan_mode_display_s_type                gan_mode_display;
      /**< Mode display; indicates if WLAN or GERAN is the active Radio 
           Access Technology (RAT). */

    sys_gw_gan_error_s_type                       gan_error;
      /**< GAN error information (category and code point). */

}cm_gw_gan_info_s_type;


/** @brief CDMA call information returned with each event. Applicable
    for targets that support CDMA.
*/
typedef struct cm_cdma_call_info_s {

  uint16                          srv_opt;
    /**< Service option as defined in the cai.h file.\n
         @note1 The service option takes on the CM_SO_ANALOG value (defined in 
         this file) when the call is connected (or to be connected) over analog 
         service. */

  cm_privacy_pref_e_type          privacy_pref;
    /**< User preference. This is different from is_privacy, which
         indicates the actual status of privacy during a call. */

  boolean                         is_privacy;
    /**< Indicates whether privacy mode is on. */

  boolean                         is_otasp_commit;
    /**< Indicates whether OTASP/OTAPA activation is completed. */

  cm_otasp_status_e_type          otasp_status;
    /**< Indicates the status of an OTASP call. */

  boolean                         is_last_cdma_info_rec;
    /**< Indicates whether this is the last CDMA information record. */

  cm_call_sups_type_e_type        sups_type;
      /**< Supplementary Services (flash) type. */

  cm_call_id_type                 emergency_flash_call_id;
      /**< Call used in sending emergency flash. */

  boolean                         emergency_flash_over_incoming_call;
      /**< Indicates whether a previous emergency request was flashed on this 
           incoming call. */

  boolean                         qos_parms_incl;
    /**< QoS parameters presence indicator:
             - TRUE -- Present
             - FALSE -- Not present. */

  uint8                           qos_parms_len;
    /**< Length of the QoS parameters list. */

  uint8                           qos_parms[CM_ORIG_QOS_LEN_MAX];
    /**< QoS parameters list. */

  uint8                           end_reason;
    /**< Reason for ending a call. This is different from end_status to keep
         the old interface unchanged while adding more IS2000-specific reasons. */

  cm_retry_order_s_type           retry_order;
    /**< Retry order information. */

  uint8                           ext_disp_type;
    /**< Display type for an extended disaply information record
         (FEATURE_IS95B_EXT_DISP). */

  boolean                         alert_ans_bypass;
    /**< Indicates whether the incoming call must be answered by the client's
         FEATURE_IS95B_ALERT_ANS_BYPASS. */

  boolean                         ndss_in_progress;
    /**< The MC is perfoming NDSS redirection (FEATURE_IS95B_NDSS). */

  cm_ext_brst_intl_s_type         ext_brst_intl_msg;
    /**< Contains charge rate information (FEATURE_T53). */

  cm_nss_clir_rec_s_type          nss_clir_rec;
    /**< National Supplementary Services CLIR (FEATURE_T53). */

  cm_nss_aud_ctrl_s_type          nss_aud_ctrl_rec;
    /**< National Supplementary Services audio control (FEATURE_T53). */

  sys_umts_cdma_handover_e_type   umts_cdma_ho_status;
    /**< UMTS-to-CDMA handover status cause. */

  sys_ho_status_e_type            ho_status;
    /**< UMTS-to-CDMA handover error status. */

  boolean                         line_ctrl_polarity_included;
    /**< Line control information. TRUE if polarity information is included. */

  boolean                         line_ctrl_toggle;
    /**< Line control information. TRUE = toggle polarity. */

  boolean                         line_ctrl_reverse;
    /**< Line control information:
         - TRUE = reverse polarity.
         - FALSE = normal polarity. */

  byte                            line_ctrl_power_denial;
    /**< Line control information; power denial time. */

  cm_call_orig_fail_layer_e_type  orig_fail_layer;
    /**< Highest layer where the call failed during origination. This field is
         adjusted after every silent redial failure during origination.
         CM_CALL_ORIG_ERR_LAYER_OTHER is a lower layer failure than
         CM_CALL_ORIG_ERR_LAYER_L2, and CM_CALL_ORIG_ERR_LAYER_L2 is
         a lower layer failure than CM_CALL_ORIG_ERR_LAYER_L3. */

   cm_con_ref_type                      con_ref;
    /**< Connection reference of the call. Reported by 1XCP to 
    ** uniquely identify the service option connection 
    */

} cm_cdma_call_info_s_type;


/** @brief IP call information returned with IP call-related events. Clients 
    must check for CM_API_IPCALL_SS_INFO before using
    this type.
*/
typedef struct cm_ip_call_info_s {

  cm_call_sups_type_e_type      sups_type;
     /**< Indicates the Supplementary Services request for which
          call_ss_success is being generated. Relevant for
          CM_CALL_EVENT_MNG_CALLS_CONF. */

  boolean                       call_ss_success;
    /**< Flag to indicate whether the Suplementary Services request on an IP
         call succeded for CM_CALL_EVENT_MNG_CALLS_CONF. */

  cm_call_prog_info_e_type      call_prog_info;
    /**< Gives the specific progress states for CM_CALL_EVENT_PROGRESS_INFO_IND. */

} cm_ip_call_info_s_type;


/** @brief Network type used for a call.
*/
typedef union cm_call_mode_info_u {

  cm_gw_cs_call_info_s_type     gw_cs_call;
      /**< CS GSM or WCDMA-specific information. */

  cm_gw_ps_call_info_s_type     gw_ps_call;
      /**< PS GSM or WCDMA-specific information. */

  cm_cdma_call_info_s_type      cdma_call;
      /**< CDMA-specific information. */

  cm_ip_call_info_s_type        ip_call;
      /**< IP-specific information. Clients must check for
           CM_API_IPCALL_SS_INFO before using this type. */

  cm_lte_call_info_s_type       lte_call;
      /**< PS LTE-specific information. */

} cm_call_mode_info_u_type;
 
/*~ CASE CM_CALL_MODE_INFO_NONE  cm_call_mode_info_u.void */
/*~ CASE CM_CALL_MODE_INFO_GW_CS cm_call_mode_info_u.gw_cs_call */
/*~ CASE CM_CALL_MODE_INFO_GW_PS cm_call_mode_info_u.gw_ps_call */
/*~ CASE CM_CALL_MODE_INFO_CDMA  cm_call_mode_info_u.cdma_call */
/*~ CASE CM_CALL_MODE_INFO_IP  cm_call_mode_info_u.ip_call */
/*~ CASE CM_CALL_MODE_INFO_LTE   cm_call_mode_info_u.lte_call*/

/** @brief Network-specific information where a call is attempted.
*/
typedef struct cm_call_mode_info_s {

  cm_call_mode_info_e_type  info_type;
    /**< Which mode information is present. */

  cm_call_mode_info_u_type  info;
    /**< Mode information. */

} cm_call_mode_info_s_type;
 
/*~ FIELD cm_call_mode_info_s.info DISC cm_call_mode_info_s.info_type */


/* The following data structures are primarily for passing information
** through the functions. They are not used anywhere else. */

/** @brief Origination parameters for GSM/WCDMA Circuit-Switched calls
    set by the clients.
*/
typedef struct cm_gw_cs_orig_params_s {

  cm_repeat_indicator_s_type          bc_repeat_indicator;
    /**< Bearer capability repeat indicator. Included only if
         bearer capability 1 and bearer capability 2 are included. */

  cm_bearer_capability_s_type         bearer_capability_1;
    /**< Bearer capability 1. */

  cm_bearer_capability_s_type         bearer_capability_2;
    /**< Bearer capability 2. */

  cm_repeat_indicator_s_type          llc_repeat_indicator;
    /**< LLC repeat indicator. Included only if the BC repeat indicator
         and low layer compatibility 1 are included. */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_1;
    /**< Low layer compatibility 1. Included in the Network -> UE direction 
         if the calling user specifies a low layer compatibility. */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_2;
    /**< Low layer compatibility 2. Included only if an LLC repeat indicator
         is included. */

  cm_repeat_indicator_s_type          hlc_repeat_indicator;
    /**< HLC repeat indicator. Included only if a BC repeat indicator
         and high layer compatibility 1 are included. */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_1;
    /**< High layer compatibility 1. Included in the Network -> UE direction 
         if the calling user specifies a high layer compatibility. */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_2;
    /**< High layer compatibility 2. Included only if an HLC repeat indicator 
         is included. */

  cm_called_party_bcd_no_s_type       called_party_bcd_number;
    /**< Called party BCD number. */

  cm_called_party_subaddress_s_type   called_party_subaddress;
    /**< Called party subaddress. */

  cm_calling_party_subaddress_s_type  calling_party_subaddress;
    /**< Calling party subaddress. */

  cm_forward_cug_info_s_type          forward_cug_info;
    /**< Closed User Group information. */

  uint8                               cm_clir;
    /**< CLIR information (e.g., CLIR suppression or invocation). */

  boolean                             call_control_completed;
    /**< Whether the calling task has already applied call control. */

  cm_user_data_s_type                 mo_user_data;
    /**< User to user data request structure. Clients must check for
         CM_API_USER_USER_DATA before using this field. */

} cm_gw_cs_orig_params_s_type;


/** @brief Origination parameters for GSM/WCDMA Packet-Switched calls.
*/
typedef struct cm_gw_ps_orig_params_s {

  boolean                       accept;
  /**< Applicable only for MT calls:
       - TRUE -- MT call is accepted.
       - FALSE -- MT call is rejected. \n
       "Accept" was set as the first parameter to align with other call answer
       structures. */

  boolean                       primary_pdp;
  /**< Whether it is a primary or secondary PDP:
       - TRUE -- Primary PDP.
       - FALSE -- Secondary PDP. */

  qos_T                         qos;
    /**< Quality of Service. */

  qos_T                         minimum_qos;
    /**< The minimum QoS allowed for the PDP context. */

  pdp_address_T                 pdp_addr;
    /**< Packet Data Protocol address. */

  apn_T                         apn_name;
    /**< Access Point Name -- A label or a fully-qualified domain name. */

  protocol_cfg_options_T        config_options;
    /**< Protocol configuration options. */

  sys_pdp_header_comp_e_type    h_comp;
    /**< IP header compression required. Clients must check for
         CM_API_PDP_COMPR before using this enumeration. */

  sys_pdp_data_comp_e_type      d_comp;
    /**< Data compression required. Clients must check for CM_API_PDP_COMPR
         before using this enumeration. */

  tft_type_T                    tft;
    /**< Traffic Flow Template. */

  nsapi_T                       primary_nsapi;
    /**< Primary NSAP information. */

  cm_call_id_type               primary_call_id;
    /**< Call ID of the primary PDP context. This field is only used if this is
         a secondary PDP context origination. */

  uint16                        profile_number;
    /**< The PDP profile number used for this PDP context. */

  byte                          pdp_group_id;
    /**< Group ID of the primary context that is associated with the
         secondary PDP context origination. Clients must check CM_API_SEC_PDP
         for secondary PDP support. */

  uint16                        assoc_pri_profile_num;
   /**< Secondary PDP origination contains requested QoS and minimum_qos:
        - FALSE -- The network has allocated QoS between requested and
                   minimum_qos.
        - TRUE -- The network has allocated a different QoS. \n

        Clients must check CM_API_SEC_PDP for secondary PDP support.
        Valid only for secondary PDP context. */


} cm_gw_ps_orig_params_s_type;

/** @brief Carries the length of the PDP activation request and bytes in L3 PDP
    activation message format. Clients must check for CM_API_GW_PS_CC before
    using this structure.

    @note1hang Call control parameters for GSM/WCDMA packet-switched calls are
    in the 3GPP Technical Specification (TS) 31.111-660, section 7.3.1.6.
    This specification refers to the 3GPP TS 24.008, section 9.5.1.
*/
typedef struct cm_gw_ps_cc_params_s {

  word length;
    /**< Length of the PDP activation byte. */

  byte pdp_act_mesg[CM_MAX_OCTETS_L3_MESSAGE];
    /**< Buffer carrying PDP activation parameters. Maximum size is
         [CM_MAX_OCTETS_L3_MESSAGE]. */

  apn_T apn_name;
    /**< apn_name is required by the UI to compare with the ACL in the SIM for
         accepting or rejecting a PS call. */

} cm_gw_ps_cc_params_s_type;

/** @brief Call control parameters for a PS call. If ps_cc_sys_mode is LTE,
    use this structure.
*/
typedef struct cm_lte_ps_cc_params_s {

  word sdf_id;  
    /**< Service Data Flow ID. Returned to DS and used in the abort
         procedure. */
    
  word length;
    /**< Length of the PDN packed message byte. */

  byte pkd_msg[CM_MAX_PDN_PACKED_SIZE];
    /**< Buffer carrying packed PDN data. */

  apn_T apn_name;
    /**< Required by the UI to compare with the ACLR in the SIM. Used for
         accepting or rejecting a PS call. */

}cm_lte_ps_cc_params_s_type;

/** @brief Answer parameters for GSM/WCDMA circuit-switched calls set
    by the clients.
*/
typedef struct cm_gw_answer_params_s {

  boolean                         accept;
    /**< Flag indicating whether the call is accepted. */

  cm_call_type_e_type             call_type;
    /**< Call type. */

  cm_connected_subaddress_s_type  connected_subaddress;
    /**< Connected subaddress. */

} cm_gw_answer_params_s_type;

/** @brief Answer parameters for WLAN voice calls set by the clients. To be 
    used only if CM_API_IP_CALL is defined.
*/
typedef struct cm_ip_answer_params_s {

  boolean                         accept;
    /**< Flag indicating whether the call is accepted. */

  cm_call_type_e_type             call_type;
    /**< Call type. */

} cm_ip_answer_params_s_type;


/** @brief Call end parameters for GSM/WCDMA Circuit-Switched calls set
    by the clients.
*/
typedef struct cm_gw_cs_end_params_s {

  uint8                      reserved;
    /**< Empty for now. */

} cm_gw_cs_end_params_s_type;


/** @brief Call end parameters for GSM/WCDMA Packet-Switched calls set
    by the clients.
*/
typedef struct cm_gw_ps_end_params_s {

  nsapi_T                    nsapi;
    /**< NSAP information. */

  ti_T                       tear_down_ind;
    /**< Teardown Indicator. Indicates whether only the PDP context associated
         with this specific TI or all active PDP contexts sharing the same PDP
         address associated with this specific TI will be deactivated. */

  sm_status_T                cause;
    /**< Cause of the call being ended. */

} cm_gw_ps_end_params_s_type;

/** @brief Call end parameters for an LTE call set by the clients.
*/
typedef struct cm_lte_end_params_s {

  byte                         eps_bearer_id;
    /**< Evolved Packet System bearer ID. */

  esm_cause_T                  esm_cause;
    /**< Cause for ending the call. */

} cm_lte_end_params_s_type;


/** @brief Setup result parameters for GSM/WCDMA circuit-switched calls
    and VOIP calls set by the clients.
*/
typedef struct cm_setup_res_params_s {

  boolean                             accept;
    /**< Flag indicating whether the call is accepted. */

    /* The following fields are only used for responding to GSM/WCDMA
       setup requests.
  */
  ie_cm_cc_cause_s_type               cc_cause;
    /**< Call control cause information. */

  cm_repeat_indicator_s_type          bc_repeat_indicator;
    /**< Bearer capability repeat indicator. Included only if
         bearer capability 1 and Bearer Capability 2 are included. */

  cm_bearer_capability_s_type         bearer_capability_1;
    /**< Bearer capability 1. */

  cm_bearer_capability_s_type         bearer_capability_2;
    /**< Bearer capability 2. */

  cm_repeat_indicator_s_type          llc_repeat_indicator;
    /**< LLC repeat indicator. Included only if the BC repeat indicator
         and Low Layer Compatibility 1 are included. Clients must check for
         CM_API_VT_FALLBACK_TO_VOICE before using this field. */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_1;
    /**< Low Layer Compatibility 1. Included in the Network -> UE direction if 
         the calling user specifies a Low Layer Compatibility. Clients must
         check for CM_API_VT_FALLBACK_TO_VOICE before using this field. */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_2;
    /**< Low Layer Compatibility 2. Included only if the LLC repeat indicator
         is included. Clients must check for CM_API_VT_FALLBACK_TO_VOICE before
         using this field. */

  cm_repeat_indicator_s_type          hlc_repeat_indicator;
    /**< HLC repeat indicator. Included only if the BC repeat indicator
         and High Layer Compatibility 1 are included. Clients must check for
         CM_API_VT_FALLBACK_TO_VOICE before using this field. */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_1;
    /**< High Layer Compatibility 1. Included in the Network -> UE direction if 
         the calling user specifies a High Layer Compatibility. Clients must
         check for CM_API_VT_FALLBACK_TO_VOICE before using this field. */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_2;
    /**< High Layer Compatibility 2. Included only if the HLC repeat indicator 
         is included. Clients must check for CM_API_VT_FALLBACK_TO_VOICE before
         using this field. */

} cm_setup_res_params_s_type;


/** @brief PDP activation response parameters for GSM/WCDMA packet switched 
    calls.
*/
typedef struct cm_pdp_act_res_params_s {

  boolean                accept;
    /**< Flag indicating whether the call is accepted. */

  sm_status_T            cause;
    /**< Cause information for the Session Manager. */

} cm_pdp_act_res_params_s_type;


/** @brief PDP modify parameters for GSM/WCDMA packet switched calls.
*/
typedef struct cm_pdp_modify_params_s {

  nsapi_T                nsapi;
    /**< NSAP information. */

  qos_T                  qos;
    /**< Quality of Service information. */

  tft_type_T             tft;
    /**< Traffic Flow Template. */

  qos_T                  minimum_qos;
    /**< Minimum Quality of Service required for the PDP context. Clients must
         check for CM_API_CHG_PDP_MOD before using this macro. */

} cm_pdp_modify_params_s_type;

/** @brief RAB re-establish request parameters for GSM/WCDMA/LTE 
    packet switched calls.
*/
typedef union cm_rab_reestab_params_u {

  struct {

    rabm_rabid_T            rabid;
      /**< RAB ID. */

  } cm_wcdma_rab_reestab_params; 

  struct {

    eps_bearer_id_T         eps_bearer_id;
      /**< EPS bearer ID. */

  } cm_lte_rab_reestab_params; 

} cm_rab_reestab_params_u_type;
/*~ DEFAULT  cm_rab_reestab_params_u.cm_wcdma_rab_reestab_params */
/*~ CASE SYS_SYS_MODE_LTE cm_rab_reestab_params_u.cm_lte_rab_reestab_params */

/** @brief RAB re-establish request parameters for GSM/WCDMA 
    packet switched calls.
*/
typedef struct cm_rab_reestab_params_s {

  sys_sys_mode_e_type           sys_mode;
    /**< RAB parameter that is present. */

  cm_rab_reestab_params_u_type  rab_reestab_params;
    /**< RAB parameter information. */

} cm_rab_reestab_params_s_type;
/*~ FIELD cm_rab_reestab_params_s.rab_reestab_params DISC cm_rab_reestab_params_s.sys_mode */


/** @brief CDMA call origination parameter structure. Applicable for
    FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_cdma_orig_params_s {

  uint16                    srv_opt;
    /**< Service options as defined in the cai.h file. */

  cm_activate_code_e_type   activate_code;
    /**< OTASP activation code. */

  boolean                   drs_bit;
    /**< Data Ready Set is ON/OFF. */

  boolean                   sr_id_included;
    /**< Whether the Service Identifier is included. */

  uint8                     sr_id;
    /**< Service Identifier. */

  boolean                   qos_parms_incl;
    /**< Whether Quality of Service parameters are included. */

  uint8                     qos_parms_len;
    /**< Length of Quality of Service parameter list. */

  uint8                     qos_parms[CM_ORIG_QOS_LEN_MAX];
    /**< Quality of Service parameter list. */

  sys_sys_mode_e_type       last_act_data_net;
    /**< Last active data network (FEATURE_HDR_HANDOFF). */

} cm_cdma_orig_params_s_type;
 
/*~ FIELD cm_cdma_orig_params_s.qos_parms VARRAY CM_ORIG_QOS_LEN_MAX
    LENGTH cm_cdma_orig_params_s.qos_parms_len */


/** @brief CDMA call answer parameter structure. Applicable for
    FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_cdma_answer_params_s {

  cm_call_type_e_type        call_type;
    /**< Call type. */

} cm_cdma_answer_params_s_type;


/** @brief CDMA call end parameter structure. Applicable for
    FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_cdma_end_params_s {

  boolean      end_reason_included;
    /**< Whether the reason the call was ended is included. */

  uint8        end_reason;
    /**< Reason the call was ended. */

} cm_cdma_end_params_s_type;

/** @brief IP cause values for call hold, call retrieve, and call end.
*/
typedef struct cm_ip_cause_s {

  uint32        cause;
    /**< IP cause. */

} cm_ip_cause_s_type;


/** @brief Type for CM alpha. Able to store alpha tag text.
*/
typedef struct cm_orig_alpha_s {

  uint8    buf[CM_MAX_ALPHA_LEN_FOR_CALL_ORIG];
    /**< Alpha buffer. */

  uint8    len;
    /**< Length of alpha tag stored in alpha_buf. */

} cm_orig_alpha_s_type;
 
/*~ FIELD cm_orig_alpha_s.buf VARRAY CM_MAX_ALPHA_LEN_FOR_CALL_ORIG
    LENGTH cm_orig_alpha_s.len */

/** @brief Union for all mode answer parameters.
*/
typedef union cm_ans_params_u {

  cm_gw_answer_params_s_type             gw_cs_ans;
    /**< GW CS answer parameters. */

  cm_gw_ps_orig_params_s_type            gw_ps_ans;
    /**< GW PS answer parameters. The parameters required are exactly the same
         as those required for a PS call origination, so the call origination
         structure is re-used. */

  cm_ip_answer_params_s_type             ip_ans;
    /**< Parameters to answer an MT IP voice call. To be used only if
         CM_API_IP_CALL is defined. */

  cm_cdma_answer_params_s_type           cdma_ans;
    /**< CDMA answer parameters. */

} cm_ans_params_u_type;
 
/*~ CASE CM_CALL_MODE_INFO_NONE  cm_ans_params_u.void */
/*~ CASE CM_CALL_MODE_INFO_GW_CS cm_ans_params_u.gw_cs_ans */
/*~ CASE CM_CALL_MODE_INFO_CDMA  cm_ans_params_u.cdma_ans */
/*~ CASE CM_CALL_MODE_INFO_GW_PS cm_ans_params_u.gw_ps_ans */
/*~ CASE CM_CALL_MODE_INFO_IP    cm_ans_params_u.ip_ans */

/** @brief Call answer parameters depending on the mode (e.g., CDMA, GSM, WCDMA).
*/
typedef struct cm_ans_params_s {

  cm_call_mode_info_e_type  info_type;
    /**< Mode type. */

  cm_ans_params_u_type      ans_params;
    /**< Union for mode-specific parameters. */

} cm_ans_params_s_type;
 
/*~ FIELD cm_ans_params_s.ans_params DISC cm_ans_params_s.info_type */


/** @brief cm_ip_end_params_s_type is part of cm_end_params_u_type, which is 
    used by clients to call the call_cmd_end() function with end parameters.
    Here, the structure is used to report information with a call end event.
    Clients must check for CM_API_VIDEOSHARE before using this type.
*/
typedef struct cm_ip_end_params_s {

  int                         cause;
    /**< Reason for ending the call. */

  cm_ipapp_warn_info_s_type   warn_info;
    /**< Warning information given by the IP application, as in RFC 3261 to the
        SIP: Session Initiation Protocol, June 2002, section 20.43. */

  char                        reason[CM_IPAPP_TEXT_SIZE];
    /**< Null terminated reason string, as per RFC 3326 to the Reason Header
         File for the Session Initiation Protocol. RFC 3326 does not mention
         any specific reason string lengths. */

} cm_ip_end_params_s_type;


/** @brief Union of all mode call end parameters.
*/
typedef union cm_end_params_u {

  cm_gw_cs_end_params_s_type           gw_cs_end;
    /**< GSM/WCDMA Circuit-Switched call end parameters. */

  cm_gw_ps_end_params_s_type           gw_ps_end;
    /**< GSM/WCDMA Packet-Switched call end parameters. */

  cm_cdma_end_params_s_type            cdma_end;
    /**< CDMA call end parameters. */

  cm_ip_cause_s_type                   ip_cause;
    /**< IP call cause values for call hold/retrieve and call end. Clients must
         check for CM_API_IP_CAUSE before using this field. */

  cm_ip_end_params_s_type              ip_end;
    /**< IP call end parameters. Clients must check for CM_API_VIDEOSHARE
         before using this field. */

  cm_lte_end_params_s_type             lte_end;
      /**< LTE call end parameters. */

} cm_end_params_u_type;

/*~ PARTIAL cm_end_params_u */
/*~ CASE CM_CALL_MODE_INFO_NONE  cm_end_params_u.void */
/*~ CASE CM_CALL_MODE_INFO_GW_CS cm_end_params_u.gw_cs_end */
/*~ CASE CM_CALL_MODE_INFO_GW_PS cm_end_params_u.gw_ps_end */
/*~ CASE CM_CALL_MODE_INFO_CDMA  cm_end_params_u.cdma_end */
/*~ CASE CM_CALL_MODE_INFO_IP    cm_end_params_u.ip_cause */
/*~ CASE CM_CALL_MODE_INFO_LTE   cm_end_params_u.lte_end */

/** @brief Call end parameters depending on the mode (e.g., CDMA, GSM, WCDMA).
*/
typedef struct cm_end_params_s {

  cm_call_id_type           call_id;
    /**< Call ID to end. */

  cm_call_mode_info_e_type  info_type;
    /**< Mode type. */

  cm_end_params_u_type      end_params;
    /**< Mode-specific end parameters. */

} cm_end_params_s_type;
 
/*~ FIELD cm_end_params_s.end_params DISC cm_end_params_s.info_type */

/** @} */ /* end_addtogroup CALL */

/** @addtogroup CALL
    @{ */

/** Enumeration for call substates in the Idle state.
*/
typedef enum cm_call_idle_substate_e {

/** @cond
  */
    CM_CALL_IDLE_SUBST_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
 */

    CM_CALL_IDLE_SUBST_ENDWAIT,
      /**< Waiting for END ACK. */

/** @cond
  */
    CM_CALL_IDLE_SUBST_MAX    /* FOR INTERNAL CM USE ONLY! */
/** @endcond
 */

} cm_call_idle_subst_e_type;
/*~ SENTINEL cm_call_idle_subst_e_type.CM_CALL_IDLE_SUBST_MAX */

/** Enumeration for call substates in Incoming state.
*/
typedef enum cm_call_incom_substate_e {

/** @cond
  */
    CM_CALL_INCOM_SUBST_NONE = -1,   /*< FOR INTERNAL CM USE ONLY! */
/** @endcond
 */

    /* 0 */
    CM_CALL_INCOM_SUBST_REGULAR,
        /**< All other SO incoming calls. */

    CM_CALL_INCOM_SUBST_OTAPA, 
        /**< OTAPA SO 18/19 incoming call. */

    CM_CALL_INCOM_SUBST_SETUP, 
        /**< Processing incoming setup(GSM/WCDMA). Setup is pending for the
             client to respond. Clients must check for
             CM_API_ADD_INCOM_SUBST before using this enumerator. */

    CM_CALL_INCOM_SUBST_SETUP_PROCESSED,
        /**< Client response sent to protocol layer (GSM/WCDMA). Clients must
             check for CM_API_ADD_INCOM_SUBST before using this enumerator. */

    CM_CALL_INCOM_SUBST_ALERTING,
        /**< Client being alerted to an incoming call (GSM/WCDMA). Clients must
             check for CM_API_ADD_INCOM_SUBST before using this enumerator. */

    CM_CALL_INCOM_SUBST_ANSWERED,
        /**< Answer from the client sent to the protocol layer (GSM/WCDMA). 
             Clients must check for CM_API_ADD_INCOM_SUBST before 
             using this enum. */

    CM_CALL_INCOM_SUBST_ACT_BEARER_REQUEST, 
        /**< Used when ESM sends ACT_BEARER_CONTEXT_REQUEST. */

    /** @cond
  */
    CM_CALL_INCOM_SUBST_MAX   /* FOR INTERNAL CM USE ONLY! */
    /** @endcond
 */

} cm_call_incom_subst_e_type;
/*~ SENTINEL cm_call_incom_subst_e_type.CM_CALL_INCOM_SUBST_MAX */


/** Enumeration for call substates in the Conversation state.
*/
typedef enum cm_call_conv_substate_e {

/** @cond
  */
    CM_CALL_CONV_SUBST_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
 */

    /* 0 */
    CM_CALL_CONV_SUBST_REGULAR,    
        /**< All other calls. */

    CM_CALL_CONV_SUBST_COMMIT,
        /**< OTASP or busy OTAPA call. Commit is in progress. */

    CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR,
        /**< SO 18/19 OTAPA call without commit (either commit never happens 
             or commit finishes). */

    CM_CALL_CONV_SUBST_PC_OTAPA_COMMIT,
        /**< SO 18/19 OTAPA call. Commit is in progress. */

    /* 4 */
    CM_CALL_CONV_CALL_ACTIVE,
        /**< The call is in the Conversation state and is active. */

    CM_CALL_CONV_CALL_ON_HOLD,
        /**< The call is in the Conversation state, but the call is on hold. */

    CM_CALL_CONV_CALL_DEACT_BEARER_REQUEST,
        /**< The call is in conversation state, but ESM sends 
             DEACT_BEARER_REQUEST. */

    /** @cond
  */
    CM_CALL_CONV_SUBST_MAX   /* FOR INTERNAL CM USE ONLY! */
    /** @endcond
 */

} cm_call_conv_subst_e_type;
/*~ SENTINEL cm_call_conv_subst_e_type.CM_CALL_CONV_SUBST_MAX */

/** @} */ /* end_addtogroup CALL */

/** @addtogroup CALL
    @{ */

/** @brief Type for call substate.
*/
typedef union cm_call_substate_u {

    cm_call_idle_subst_e_type   idle;
      /**< Call substate Idle. */

    cm_call_incom_subst_e_type  incom;
      /**< Call substate Incoming. */

    cm_call_conv_subst_e_type   conv;
      /**< Call substate Conversation. */

} cm_call_substate_type;
 
/*~ CASE CM_CALL_STATE_IDLE      cm_call_substate_u.idle */
/*~ CASE CM_CALL_STATE_INCOM     cm_call_substate_u.incom */
/*~ CASE CM_CALL_STATE_CONV      cm_call_substate_u.conv */
/*~ DEFAULT                      cm_call_substate_u.void */

/** @brief Defines the state information for a particular call object.
*/
typedef struct cm_call_state_info_s {

  cm_call_id_type                call_id;
    /**< Call ID. */

  cm_call_type_e_type            call_type;
    /**< Call type: VOICE, DATA, OTASP, etc. */

  cm_call_state_e_type           call_state;
    /**< Call state: Idle, Originating, Incoming, or Conversation. */

  sys_sys_mode_e_type            sys_mode;
    /**< The mode in which the call was originated. */

  cm_call_direction_e_type       call_direction;
    /**< Indicates if the call was mobile originated or mobile terminated. */

  dword                          call_connect_time;
    /**< The time the call was connected. The units are in seconds since the 
         phone has powered on (clock up time). **/

  cm_alpha_s_type                alpha;
    /**< Alpha tag to be displayed along with dialed address of an originated 
         call. */

  /* The following fields are only valid if
     call_type is CM_CALL_TYPE_PS_DATA. */

  boolean                        primary_pdp;
    /**< Whether the PDP context is primary or secondary:
         - TRUE -- Primary PDP context.
         - FALSE -- Secondary PDP context. */

  uint16                         profile_number;
    /**< The number/ID of the PDP profile used for this PDP context. */

  cm_call_id_type                primary_call_id;
    /**< Call ID of the primary PDP context. */

  uint16                         assoc_pri_profile_num;
    /**< Secondary PDP origination contains requested QoS and minimum_qos:
         - FALSE -- The network has allocated a QoS between the requested QoS
                    and minimum_qos.
         - TRUE -- The network has allocated a different QoS. \n

         Clients must check CM_API_SEC_PDP for secondary PDP support.
         Valid only for secondary PDP context. */

  boolean                        is_net_alloc_qos_diff_from_req;
    /**< The secondary PDP origination's requested QoS and minimum_qos decide
         this value:
         - FALSE -- The network has allocated a QoS between the requested QoS
                    and minimum_qos.
         - TRUE -- The network has allocated a different QoS. \n

         Clients must check CM_API_SEC_PDP for secondary PDP support.
         Valid only for secondary PDP context. */

  byte                           pdp_group_id;
     /**< Group ID of primary context particular secondary PDP is associated
          with. Clients must check CM_API_SEC_PDP for SEC PDP support. Valid
          only for secondary PDP context. */

  cm_call_substate_type          call_subst;
     /**< Call substates: Idle, Conversation, or Incoming.  */

} cm_call_state_info_s_type;
 
/*~ FIELD cm_call_state_info_s.call_subst DISC cm_call_state_info_s.call_state */

/** @brief The state information for all call objects.
*/
typedef struct cm_call_state_info_list_s {

  byte                      number_of_active_calls;
    /**< The number of calls whose state is not idle. */

  cm_call_state_info_s_type info[CM_CALL_ID_MAX];
    /**< The state information for all call objects. */
  
} cm_call_state_info_list_s_type;
/*~ FIELD cm_call_state_info_list_s.info VARRAY CM_CALL_ID_MAX
    LENGTH cm_call_state_info_list_s.number_of_active_calls */


/** @brief Structure for holding call compatibility and capability 
    details. Clients must check for CM_API_VT_FALLBACK_TO_VOICE before using 
    this field.
*/
typedef struct cm_call_comp_cap_info_s {

  cm_bearer_capability_s_type         bearer_capability;
    /**< Bearer capability. */

  cm_low_layer_compatibility_s_type   low_layer_compatibility;
    /**< Low layer compatibility. Included in the Network -> UE direction if
         the calling user specifies a low layer compatibility. */

  cm_high_layer_compatibility_s_type  high_layer_compatibility;
    /**< High layer compatibility. Included in the Network -> UE direction if
         the calling user specifies a high layer compatibility. */

} cm_call_comp_cap_info_s_type;

/** @brief Structure for holding supplementary service related parameters
    when call control modifies voice call to sups.
*/
typedef struct cm_voice_to_ss_param_s {

  uint8                                ss_code;
    /**< Supplementary Services code. */

  uint8                                ss_ref;
    /**< Supplementary Services reference. */

  uint8                                ss_oper;
    /**< Supplementary Services operation. */

  uss_data_s_type                      uss_data;
    /**< Unstructured Supplementary Services data. */

}cm_voice_to_ss_param_s_type;

/** @brief Structure to hold the alpha and number returned by call 
    control. Clients must check for the CM_API_RESULT_FROM_CC flag before 
    using this type.
*/
typedef struct cm_result_from_cc_s
{
  cm_cc_result_e_type                call_control_result;
    /**< Indicates whether there were modifications. */

  cm_num_s_type                      num;
    /**< Number buffer:
         - CM_CALL_EVENT_ORIG -- Contains the dialed digits string.
         - CM_CALL_EVENT_FLASH -- Contains the dialed digits string.
         - CM_CALL_EVENT_INCOM -- Contains the caller ID string.
         - CM_CALL_EVENT_CALLER_ID -- Contains the caller ID string. */

  cm_alpha_s_type                    alpha;
    /**< Alpha tag returned by call control. */

  cm_voice_to_ss_param_s_type        voice_to_ss;
    /**< SS params related to call control */

} cm_result_from_cc_s_type;

/** @brief Type to hold call state information. When notified of call
    events, clients are given a reference to such a structure.
*/
struct cm_mm_call_info_s {

  cm_client_id_type                    call_client_id;
    /**< Call client: Originating or answering client. */

  cm_call_id_type                      call_id;
    /**< Call ID. */

  cm_call_state_e_type                 call_state;
    /**< Call state: Idle, Originating, Incoming, or Conversation. */

  cm_call_type_e_type                  call_type;
    /**< Call type: Voice, data, OTASP, etc. */

  cm_num_s_type                        num;
    /**< Number buffer:
         - CM_CALL_EVENT_ORIG -- Contains the dialed digits string.
         - CM_CALL_EVENT_FLASH -- Contains the dialed digits string.
         - CM_CALL_EVENT_INCOM -- Contains the caller ID string.
         - CM_CALL_EVENT_CALLER_ID -- Contains the caller ID string. */

  cm_num_s_type                        waiting_num;
    /**< Call waiting number buffer. */

  cm_redirecting_num_s_type            redirecting_number;
    /**< Number buffer: CM_CALL_EVENT_REDIRECTING_NUMBER contains the
         redirecting number. */

  cm_alpha_s_type                      alpha;
    /**< Alpha tag to be displayed along with the dialed address of an 
         originated call.
         Alpha buffer:
         - CM_CALL_EVENT_ORIG
         - CM_CALL_EVENT_DISPLAY
         - CM_CALL_EVENT_EXT_DISP

         @note1 This is not a phone book name match, but rather an indication 
         for the UI to display some text for an outgoing call. */

       /* We probably need to change this one to a UI text component to
          support I18N. */

  cm_enc_alpha_s_type                  enc_alpha;
    /**< Alpha tag to be displayed along with dialed address of an
       originated call. This has coding information that may be
       used later by the UI. */

  cm_signal_s_type                     signal;
    /**< Alert signal information to be played for the incoming call. */

  cm_call_end_e_type                   end_status;
    /**< Call end status (i.e., the reason for ending a call in the Origination,
         Incoming, or Conversation state). */

  uint8                                num_call_ids_ended;
    /**< Number of calls ended. */

  cm_end_params_s_type                 end_params;
    /**< Call end information. */

  sys_sys_mode_e_type                  sys_mode;
    /**< Indicates the system on which the call is made. */

  dword                                call_connect_time;
    /**< Time that the call was connected. This is the number of seconds since
         the phone powered on (clock up time). */

  dword                                call_end_time;
    /**< Time that the call was ended. The is the number of seconds since
         the phone powered on (clock up time). */

  uint32                               bytes_received;
    /**< Number of bytes received during the call (PS data call only). This
         field is populated after the call has ended. */

  uint32                               bytes_transmitted;
    /**< Number of bytes transmitted during the call (PS data call only). This
         field is populated after the call has ended. */
  cm_call_mode_info_s_type             mode_info;
    /**< System-specific information: CDMA, GSM/WCDMA-CS, or GSM/WCDMA-PS. */

  cm_als_line_e_type                   line;
    /**< Which line is being used for this call (FEATURE_ALS). */

  cm_client_id_type                    requesting_client_id;
    /**< Requesting client. */

  sys_msm_id_e_type                    msm_id;
    /**< Identifies the MSM that generated this event. */

  cm_call_direction_e_type             direction;
    /**< Holds the direction of the current call. This can be Mobile Originated
         or Mobile Terminated. */

  cm_call_modify_e_type                modify;
    /**< Indicates the modified call type. It indicates the network's
         preferred modify action when the CM sends CM_CALL_EVENT_MODIFY. In
         all other cases, it represents the modified call type being used.\n

         @note1 The CALL_TYPE is not changed as a result of MODIFY. */

  cm_result_from_cc_s_type             result_from_cc;
    /**< Holds alpha and numbers returned by Call Control. Clients must check
         for CM_API_RESULT_FROM_CC before using this field. */

  sys_modem_as_id_e_type               asubs_id;
    /**< Holds the active subscription ID on which this call is originated 
         or received. */
};

/** @brief Type for capability configuration.
*/
typedef struct cm_cap_config_s {

    uint8                        length;
      /**< Size of the capability configuration array. */

    uint8                        ccp[CM_MAX_CCP_LEN];
      /**< Capability configuration array. */

} cm_cap_config_s_type;


/** Type for call compatibility and capability information callback
    function. The function is registered along with the call type to retrieve
    compatibility capability. Clients must check for CM_API_VT_FALLBACK_TO_VOICE
    before using this type definition.
*/
typedef void (cm_call_comp_cap_cb_f_type) (

  cm_call_comp_cap_info_s_type    *call_comp_cap_info_ptr
    /**< Pointer to call compatibility and capability information. */

);
 
/*~ CALLBACK cm_call_comp_cap_cb_f_type */
/*~ PARAM call_comp_cap_info_ptr POINTER */


/** @brief Call Control generic parameters for a call. The sructure
    carries FDN checking information.
*/
typedef struct cm_cc_generic_params_s{

  boolean     is_fdn_to_check;
    /**< This can be checked by clients to decide whether FDN checking must
         be done. */

} cm_cc_generic_params_s_type;


/** @brief Type to hold CDMA position information.
*/
typedef struct cm_cdma_position_info_s {

  uint16                                            sid;       
    /**< System identification. */

  uint16                                            nid;  
    /**< Network identification. */

  uint16                                            base_id;
    /**< Base station identification. */

  uint16                                            refpn;            
    /**< Reference PN. */

  uint32                                            base_lat;      
    /**< Base station latitude. */

  uint32                                            base_long;    
    /**< Base station longitude. */

}cm_cdma_position_info_s_type;


/*---------------------------------------------------------------------------
** Functions
**-------------------------------------------------------------------------*/

/* CM command functions.

   The CM command functions define the Client->CM interface. Each command 
   function takes a pointer to a client provided callback function, a 
   client-provided void pointer, and the command's required set of fields.

   If the command function finds that one or more of the parameters contains
   invalid values (e.g., not one of the enum values for an enum parameter),
   it calls the client's callback function IMMEDIATELY with the appropriate 
   command error code. In this case, the callback function is actually being 
   called from the client context, NOT the CM.

   If all parameters are found to be valid, the command function queues a
   command to the CM. When the CM gets the command, it checks if the 
   command is valid relative to the current state of the phone/call.

   If the command is not valid (e.g., trying to end a call that was already
   ended), the CM calls the client callback function with the appropriate
   command error code. In this case the, callback function is being called 
   from the CM context.

   If the command is successful, the CM calls the client callback function
   with an error code that indicates a successful command. In addition, the CM
   notifies registered clients of the corresponding CM event (e.g., if a
   client successfully ends a call, the CM notifies clients that are registered
   for the call-end-event of this event). */

/** Type for call command callback functions. A client that wants to be
    notified of the results of a call command must supply a pointer to a
    function of this type as the first parameter to a call command function.

   @note1hang The client-provided callback functions must be re-entrant, since
   they might be called from the client and the CM context simultaneously.
*/
typedef void (cm_call_cmd_cb_f_type) (

  void                      *data_block_ptr,
    /**< Pointer to the client-provided data block.\n

         @note1 Depending on the client implementation of this function,
         the data block pointed to by this pointer might be accessed from
         the client and the CM context simultaneously. */

  cm_call_cmd_e_type        call_cmd,
    /**< Indicates which command this error status is for. */

  cm_call_cmd_err_e_type    call_cmd_err
    /**< Call command error code indicating if the command is
         rejected and the reason for the rejection. */

);

/** Client callback function used to notify clients of an error. */
typedef void (cm_call_cmd_exten_cb_f_type) (

  void                          *data_block_ptr,
    /**< Pointer to the client-provided data block.\n
         @note1 Depending on the client implementation of this function
         the data block pointed by this pointer might be accessed from
         client, as well as the CM context simultaneously. */

  cm_call_cmd_e_type            call_cmd,
    /**< Indicates which command this error status is for. */

  cm_call_cmd_err_e_type        call_cmd_err,
    /**< Call command error code, indicating if the command
         is rejected and for what reason. */

  cm_call_cmd_err_cause_e_type  call_cmd_err_cause,
     /**< Cause of the error. This is used when call_cmd_err is anything other
          than CM_CALL_CMD_ERR_NOERR. It is currently only supported for
          CC_REJECT. */
   
  cm_alpha_s_type               alpha
    /**< Modified alpha to be used only if alpha.len > 0. */

);
/*~ CALLBACK cm_call_cmd_exten_cb_f_type */

/** @} */ /* end_addtogroup CALL */

/** @addtogroup CALL
    @{ */

/*==========================================================================

FUNCTION: cm_mm_call_cmd_orig

==========================================================================*/

/**
  Commands the CM to originate a call. The client ID
  should be from a registered CM client. 
  
  The activate_code field is only used for OTASP calls (i.e., the call type is
  CM_CALL_TYPE_STD_OTASP or CM_CALL_TYPE_NON_STD_OTASP). For non-OTASP calls,
  this field should be set to CM_OTASP_ACT_CODE_NONE. For called_number, if the
  number->digit_mode is set to CM_DIGIT_MODE_4BIT_DTMF, the number->buf may
  contain only the 0-9, #, or * characters, and if the number->digit_mode is set
  to CM_DIGIT_MODE_8BIT_ASCII, the number buffer may contain any 8-bit ASCII
  characters.

  If the call type is voice, the CM calls the application registered resolve
  call type callback function to determine if the dialed number is one of the
  special numbers for OTASP or Emergency. If it is a special number, the resolve
  call type function modifies the original call type accordingly. If no
  application registered a resolve call type callback function with the CM, the
  call type is not changed. For all call types, the CM calls the application
  registered call allowed callback function to determine if the dialed number is
  allowed. If no application registered a call-allowed callback function with
  the CM, the call origination for all dialed numbers is always allowed. 
  
  For targets supporting dual slots, the application registered resolve call
  type and call-allowed callback functions are called as described above if
  there is no call control processing callback function registered. If there is
  a call control processing callback function registered, the CM calls the call
  control processing callback type function instead. This call control
  processing callback type function performs call type resolutions, call allowed
  checks, and any call control that is required. In the case of dual slots in
  automatic mode, conflicts may arise during call control. The CM calls an
  application-registered call control preferences callback type function to
  resolve any such conflicts.

  The client-supplied callback function cmd_cb_func will be called to
  notify the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Originating client.
@param[in] call_type             Call type (e.g., voice, data).
@param[in] srv_type              Service type (e.g., automatic).
@param[in] calling_number        Number of the phone that is originating the call.
@param[in] called_number         Number dialed (phone that is being called). 
@param[in] alpha_buf             Alpha tag to be displayed for the origination. 
@param[in] cdma_orig_params_ptr  Pointer to CDMA mode-specific data.
@param[in] gw_cs_orig_params_ptr Pointer to GW CS mode-specific data.
@param[in] gw_ps_orig_params_ptr Pointer to GW PS mode-specific data.
@param[out] return_call_id_ptr   Call ID will be allocated and returned.

@return 
  call_id is returned in call_id_ptr. \n
  - TRUE -- Request sent.
  - FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_orig(

  cm_call_cmd_cb_f_type             cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_call_type_e_type               call_type,
  cm_srv_type_e_type                srv_type,
  const cm_num_s_type               *calling_number,
  const cm_num_s_type               *called_number,
  const cm_orig_alpha_s_type        *alpha_buf,
  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
  cm_call_id_type                   *return_call_id_ptr

);
 
/*~ FUNCTION cm_mm_call_cmd_orig */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */

/*==========================================================================

FUNCTION: cm_mm_call_cmd_orig_exten

==========================================================================*/
/**
  Commands the CM to originate a handover call. The client ID should be from a
  registered CM client.

  The client-supplied callback function cmd_cb_func will be called to
  notify the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Originating client.
@param[in] call_type             Call type (e.g., voice, data).
@param[in] srv_type              Service type (e.g., automatic).
@param[in] calling_number        Number of the phone that is originating the call.
@param[in] called_number         Number dialed (phone that is being called).
@param[in] alpha_buf             Alpha tag to be displayed for the origination.
@param[in] cdma_orig_params_ptr  Pointer to CDMA mode-specific data.
@param[in] gw_cs_orig_params_ptr Pointer to GW CS mode-specific data.
@param[in] gw_ps_orig_params_ptr Pointer to GW PS mode-specific data.
@param[in] switch_out_call_id    Switch out call ID.
@param[out] return_call_id_ptr   Call ID will be allocated and returned.

@return 
  switch_in_call_id is returned in switch_in_call_id_ptr. \n
  - TRUE -- Request sent.
  - FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_orig_exten(

  cm_call_cmd_cb_f_type             cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_call_type_e_type               call_type,
  cm_srv_type_e_type                srv_type,
  const cm_num_s_type               *calling_number,
  const cm_num_s_type               *called_number,
  const cm_orig_alpha_s_type        *alpha_buf,
  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
  cm_call_id_type                   switch_out_call_id,
  cm_call_id_type                   *return_call_id_ptr

);
/*~ FUNCTION cm_mm_call_cmd_orig_exten */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */

/*==========================================================================

FUNCTION: cm_mm_call_cmd_orig_exten2

==========================================================================*/

/**
  Commands the CM to originate a call with call control generic parameters. 
  Clients must check if CM_API_CC_MOD_PARAM is defined before calling this API.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Originating client.
@param[in] call_type             Call type (e.g., voice, data).
@param[in] srv_type              Service type (e.g., automatic).
@param[in] calling_number        Number of the phone that is originating the call.
@param[in] called_number         Number dialed (phone that is being called).
@param[in] alpha_buf             Alpha tag to be displayed for the origination.
@param[in] cdma_orig_params_ptr  Pointer to CDMA mode-specific data.
@param[in] gw_cs_orig_params_ptr Pointer to GW CS mode-specific data.
@param[in] gw_ps_orig_params_ptr Pointer to GW PS mode-specific data.
@param[in] switch_out_call_id    Switch out call ID.
@param[in] cc_generic_params_ptr Call control generic parameters. Default 
                                 value = TRUE.
@param[out] return_call_id_ptr   Call ID will be allocated and returned.

@return 
  Switch_in Call_id will be returned in switch_in_call_id_ptr.
  - TRUE -- Request sent.
  - FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_orig_exten2(

  cm_call_cmd_cb_f_type             cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_call_type_e_type               call_type,
  cm_srv_type_e_type                srv_type,
  const cm_num_s_type               *calling_number,
  const cm_num_s_type               *called_number,
  const cm_orig_alpha_s_type        *alpha_buf,
  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
  cm_call_id_type                   switch_out_call_id,
  const cm_cc_generic_params_s_type *cc_generic_params_ptr,
  cm_call_id_type                   *return_call_id_ptr

);
/*~ FUNCTION cm_mm_call_cmd_orig_exten2 */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM cc_generic_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_orig_exten3

===========================================================================*/
/**
  Commands the CM to originate a call. Call control will be done in the CM
  before originating the call. The client ID should be from a registered CM 
  client. Clients must check if CM_API_MODEM_CALL_CONTROL is defined 
  before calling this API.

  The client-supplied callback function cm_call_cmd_exten_cb_f_type
  will be called to notify the client of the command status. The error code is
  passed back to the client through the client-provided command callback 
  function cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@param[in] cmd_cb_func           Client callback function.   
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Originating client.
@param[in] call_type             Call type (e.g., voice, data).
@param[in] srv_type              Service type (e.g., automatic).
@param[in] calling_number        Number of the phone that is originating the call.
@param[in] called_number         Number dialed (phone that is being called).
@param[in] alpha_buf             Alpha tag to be displayed for the origination.
@param[in] cdma_orig_params_ptr  Pointer to CDMA mode-specific data.
@param[in] gw_cs_orig_params_ptr Pointer to GW CS mode-specific data.
@param[in] gw_ps_orig_params_ptr Pointer to GW PS mode-specific data.
@param[in] switch_out_call_id    Switch out call ID.
@param[in] orig_client_categ     Initiating client category.
@param[out] return_call_id_ptr   Call ID will be allocated and returned.

@return 
  Switch_in Call_id will be returned in switch_in_call_id_ptr.
  - TRUE -- request sent.
  - FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_orig_exten3(

  cm_call_cmd_exten_cb_f_type       cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_call_type_e_type               call_type,
  cm_srv_type_e_type                srv_type,
  const cm_num_s_type               *calling_number,
  const cm_num_s_type               *called_number,
  const cm_orig_alpha_s_type        *alpha_buf,
  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
  cm_call_id_type                   switch_out_call_id,
  cm_orig_client_categ_e_type       orig_client_categ,
  cm_call_id_type                   *return_call_id_ptr

);
/*~ FUNCTION cm_mm_call_cmd_orig_exten3 */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_answer

===========================================================================*/
/**
  Commands the CM to answer an incoming call. The call ID passed has a range
  from 0 to CM_CALL_ID_MAX. The client ID should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to
  notify the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func    Client callback function. 
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Answering client.
@param[in] call_id        Call ID.
@param[in] ans_params_ptr Pointer to mode-specific data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.

*/
extern boolean cm_mm_call_cmd_answer(

  cm_call_cmd_cb_f_type        cmd_cb_func,
  void                         *data_block_ptr,
  cm_client_id_type            client_id,
  cm_call_id_type              call_id,
  const cm_ans_params_s_type   *ans_params_ptr

);
/*~ FUNCTION cm_mm_call_cmd_answer */
/*~ PARAM ans_params_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_act_bearer_rsp

===========================================================================*/
/**
  Commands the CM to respond to the ACT_BEARER_CONTEXT_REQUEST. The call ID
  passed has a range from 0 to CM_CALL_ID_MAX. The client ID should be from a
  registered CM client.

  The client-supplied callback function cmd_cb_func will be called to
  notify the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Answering client.
@param[in] call_id            Call ID of the call.
@param[in] act_bearer_rsp_ptr Pointer to the response-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_act_bearer_rsp(

  cm_call_cmd_cb_f_type           cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type               client_id,
  cm_call_id_type                 call_id,
  const cm_act_bearer_rsp_s_type  *act_bearer_rsp_ptr

);
/*~ FUNCTION cm_mm_call_cmd_act_bearer_rsp */
/*~ PARAM act_bearer_rsp_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_pdn_connectivity_req

===========================================================================*/
/**
  Commands the CM to send PDN_CONNECTIVITY_REQ to ESM. The client ID should be 
  from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to
  notify the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func          Client callback function.
@param[in] data_block_ptr       Pointer to the client callback data block.
@param[in] client_id            Answering client.
@param[in] pdn_connectivity_ptr Pointer to the request-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_pdn_connectivity_req(

  cm_call_cmd_cb_f_type               cmd_cb_func,
  void                                *data_block_ptr,
  cm_client_id_type                   client_id,
  const cm_pdn_connectivity_s_type    *pdn_connectivity_ptr

);
/*~ FUNCTION cm_mm_call_cmd_pdn_connectivity_req */
/*~ PARAM pdn_connectivity_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_pdn_connectivity_abort_req

===========================================================================*/
/**
  Commands the CM to send PDN_CONNECTIVITY_ABORT_REQ to ESM. The client ID
  should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to
  notify the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func                Client callback function.
@param[in] data_block_ptr             Pointer to the client callback data block.
@param[in] client_id                  Answering client.
@param[in] pdn_connectivity_abort_ptr Pointer to the abort-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_pdn_connectivity_abort_req(

  cm_call_cmd_cb_f_type                     cmd_cb_func,
  void                                      *data_block_ptr,
  cm_client_id_type                         client_id,
  const cm_pdn_connectivity_abort_s_type    *pdn_connectivity_abort_ptr

);
/*~ FUNCTION cm_mm_call_cmd_pdn_connectivity_abort_req */
/*~ PARAM pdn_connectivity_abort_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_pdn_disconnect_req

===========================================================================*/
/**
  Commands the CM to send PDN_DISCONNECT_REQ to ESM. The client ID should be 
  from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to
  notify the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Answering client.
@param[in] pdn_disconnect_ptr Pointer to the disconnect-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_pdn_disconnect_req(

  cm_call_cmd_cb_f_type           cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type               client_id,
  const cm_pdn_disconnect_s_type  *pdn_disconnect_ptr

);
/*~ FUNCTION cm_mm_call_cmd_pdn_disconnect_req */
/*~ PARAM pdn_disconnect_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_res_alloc_req

===========================================================================*/
/**
  Commands the CM to send RES_ALLOC_REQ to eSM. The client ID should be 
  from a registered CM client.

  The client supplied callback function, cmd_cb_func, will be called to 
  notify the client of the command status. The error code is passed back 
  to the client through the client-provided, command-callback function, 
  cmd_cb_func. If the command request is processed without errors, 
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Answering client.
@param[in] res_alloc_ptr  Pointer to the request-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_res_alloc_req(

  cm_call_cmd_cb_f_type       cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type           client_id,
  const cm_res_alloc_s_type   *res_alloc_ptr

);
/*~ FUNCTION cm_mm_call_cmd_res_alloc_req */
/*~ PARAM res_alloc_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_res_alloc_abort_req

===========================================================================*/
/**
  Commands the CM to send RES_ALLOC_ABORT_REQ to ESM. The client ID should be 
  from a registered CM client.

  The client supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.


@param[in] cmd_cb_func         Client callback function.
@param[in] data_block_ptr      Pointer to the client callback data block.
@param[in] client_id           Answering client.
@param[in] res_alloc_abort_ptr Pointer to the abort-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_res_alloc_abort_req(

  cm_call_cmd_cb_f_type             cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  const cm_res_alloc_abort_s_type   *res_alloc_abort_ptr

);
/*~ FUNCTION cm_mm_call_cmd_res_alloc_abort_req */
/*~ PARAM res_alloc_abort_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_bearer_res_mod_req

===========================================================================*/
/**
  Commands the CM to send RES_MOD_REQ to ESM. The client ID should be from a
  registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Answering client.
@param[in] res_mod_ptr    Pointer to the modification-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_bearer_res_mod_req(
  cm_call_cmd_cb_f_type     cmd_cb_func,
  void                      *data_block_ptr,
  cm_client_id_type         client_id,
  const cm_res_mod_s_type   *res_mod_ptr

);
/*~ FUNCTION cm_mm_call_cmd_bearer_res_mod_req */
/*~ PARAM res_mod_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_mod_bearer_rsp

===========================================================================*/
/**
  Commands the CM to send MOD_BEARER_ACCEPT or MOD_BEARER_REJ to ESM. The client 
  ID should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Answering client.
@param[in] call_id            Call ID of the call.
@param[in] mod_bearer_rsp_ptr Pointer to the response-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_mod_bearer_rsp(

  cm_call_cmd_cb_f_type             cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_call_id_type                   call_id,
  const cm_mod_bearer_rsp_s_type    *mod_bearer_rsp_ptr

);
/*~ FUNCTION cm_mm_call_cmd_mod_bearer_rsp */
/*~ PARAM mod_bearer_rsp_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_end

===========================================================================*/
/**
  Commands the CM to end a call, whether in the Origination, Incoming, or
  Conversation state. The call ID passed has a range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client. Multiple calls can also
  be ended, where num_call_ids holds the number of calls.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.


@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Ending client.
@param[in] num_call_ids   Number of call IDs sent.
@param[in] end_params_ptr Pointer to mode-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_end(

  cm_call_cmd_cb_f_type         cmd_cb_func,
  void                          *data_block_ptr,
  cm_client_id_type             client_id,
  uint8                         num_call_ids,
  const cm_end_params_s_type    *end_params_ptr

);
/*~ FUNCTION cm_mm_call_cmd_end */
/*~ PARAM end_params_ptr VARRAY LENGTH num_call_ids */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_end2

===========================================================================*/
/**
  Commands the CM to end a call, whether in the Origination, Incoming, or
  Conversation state. The call ID passed has a range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client. Multiple calls can also
  be ended, where num_call_ids holds the number of calls.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Ending client.
@param[in] num_call_ids   Number of call IDs sent.
@param[in] end_params_ptr Pointer to mode-specific data.
@param[in] msm_id         MSM ID to which this command is directed.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_end2(

  cm_call_cmd_cb_f_type  cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  uint8                  num_call_ids,
  cm_end_params_s_type   *end_params_ptr,
  sys_msm_id_e_type      msm_id

);

/*============================================================================

FUNCTION: cm_mm_call_cmd_incoming_page_resp

===========================================================================*/
/**
  Commands CM to proceed with incoming 1x page. 

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func       Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Answering client.
@param[in] boolean           command data - whether to proceed with incoming page

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.
  applicable for 1x incoming pages only
  
@sideeffects
  None
*/
extern boolean cm_mm_call_cmd_incoming_page_resp(

  cm_call_cmd_cb_f_type               cmd_cb_func,
  void                                *data_block_ptr,
  cm_client_id_type                   client_id,
  boolean                             page_response

);
/*~ FUNCTION cm_mm_call_cmd_incoming_page_resp */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_sups

===========================================================================*/
/**
  Commands the CM to send a Supplementary Services (SUPS) request to the BS. All
  GW SUPS come with a valid call ID. The call ID passed has a range from 0 to
  CM_CALL_ID_MAX. The client ID should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func     Client callback function.
@param[in] data_block_ptr  Pointer to the client callback data block.
@param[in] client_id       Sending client.
@param[in] sups_type       Type of supplementary service.
@param[in] sups_params_ptr Pointer to supplementary service-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_sups(

  cm_call_cmd_cb_f_type       cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type           client_id,
  cm_call_sups_type_e_type    sups_type,
  const cm_call_sups_params_s_type  *sups_params_ptr

);
/*~ FUNCTION cm_mm_call_cmd_sups */
/*~ PARAM sups_params_ptr POINTER */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_privacy_pref

===========================================================================*/
/**
  Commands the CM to change the privacy preference. The client ID should be from
  a registered CM client. Since the privacy preference applies to all calls,
  this function does not need a call ID parameter.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Ending client.
@param[in] call_id        Call ID.
@param[in] privacy_pref   Privacy preference setting.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. FEATURE_CDMA_800
  or FEATURE_CDMA_1900 must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_mm_call_cmd_privacy_pref(

  cm_call_cmd_cb_f_type   cmd_cb_func,
  void                    *data_block_ptr,
  cm_client_id_type       client_id,
  cm_call_id_type         call_id,
  cm_privacy_pref_e_type  privacy_pref

);
 
/*~ FUNCTION cm_mm_call_cmd_privacy_pref */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_get_call_info

===========================================================================*/
/**
  Gets a snapshot of the current call information in the CM. The call ID passed
  has a range from 0 to CM_CALL_ID_MAX. The client ID should be from a
  registered CM client. The client must register for CM_CALL_EVENT_INFO to
  receive a response to this command. The CM_CALL_EVENT_INFO is sent only to the
  client that issues this command.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.
@param[in] call_id        Call ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_mm_call_cmd_get_call_info(

  cm_call_cmd_cb_f_type  cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  cm_call_id_type        call_id

);
/*~ FUNCTION cm_mm_call_cmd_get_call_info */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_get_call_info_list

===========================================================================*/
/**
  Gets a snapshot of the state of all call objects maintained by the CM. This is
  a synchronous function. The client ID should be from a registered CM client.
  The client must register for CM_CALL_EVENT_INFO to receive a response to
  this command. The CM_CALL_EVENT_INFO is sent only to the client that issues
  this command.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_get_call_info_list(

  cm_call_cmd_cb_f_type  cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id

);
/*~ FUNCTION cm_mm_call_cmd_get_call_info_list */
 
/*===========================================================================

FUNCTION: cm_mm_call_comp_cap_cb_reg

===========================================================================*/
/**
  Clients call this function to register callbacks that can be used to receive
  capability and compatibility information for a call type. The client ID should
  be from a registered CM client.

  BC (Bearer Capability), LLC (Low Level Capability) and HLC (High Level
  Capability) values are different for call types. For mixed mode call
  origination, a voice call must carry BC, LLC, and HLC details for a VT (Video
  Telephony) call. The CM provides a storage for these values so that clients
  can query for other call type's details. For example, the UI can query bearer
  details for a VT call in order to originate a mixed mode call.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.
@param[in] call_type      Call type for which callback has to be set.
@param[in] comp_cap       Callback that retrieves compatibility and capability
                          for the call type.

@return
  TRUE -- Call compatibility and capability being returned. \n
  FALSE -- Call compatibility and capability could not be returned.

@dependencies
 The CM must have already been initialized with cm_init_before_task_start() and
 cm_init_after_task_start(). CM_API_VT_FALLBACK_TO_VOICE must be defined.
*/
extern boolean cm_mm_call_comp_cap_cb_reg(

  cm_call_cmd_cb_f_type  cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  cm_call_type_e_type    call_type,
  cm_call_comp_cap_cb_f_type  *comp_cap

);
 
/*~ FUNCTION cm_mm_call_comp_cap_cb_reg */
/*~ PARAM comp_cap POINTER */

/*===========================================================================

FUNCTION: cm_mm_get_call_comp_cap

===========================================================================*/
/**
  Returns the call compatibility and capability for a given call
  type. The client ID should be from a registered CM client.

@param[in] cmd_cb_func             Client callback function.
@param[in] data_block_ptr          Pointer to the client callback data block.
@param[in] client_id               Requesting client ID.
@param[in] call_type               Call type.
@param[out] call_comp_cap_info_ptr Pointer used to return compatibility and
                                   capability for call type.

@return
  TRUE -- Call compatibiliy and capability being returned. \n
  FALSE -- Call compatibiliy and capability could not be returned.

@dependencies
  CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start(). CM_API_VT_FALLBACK_TO_VOICE must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_get_call_comp_cap(

  cm_call_cmd_cb_f_type  cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  cm_call_type_e_type    call_type,
  cm_call_comp_cap_info_s_type   *call_comp_cap_info_ptr

);
 
/*~ FUNCTION cm_mm_get_call_comp_cap */
/*~ PARAM call_comp_cap_info_ptr POINTER */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_setup_res

==========================================================================*/
/**
  Commands the CM to send a setup message response for an incoming call. The
  call ID passed has a range from 0 to CM_CALL_ID_MAX. The client ID should be
  from a registered CM client. Setup response to MN requires just BC, but the CM
  also copies LLC and HLC so that this information can be used for in-call
  modifications after a call gets connected. For example, for VT setup, DS sends
  the setup response as voice. LLC for VT must be stored in LLC2 so that it can
  be used later for in-call modifications to VT.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func      Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Answering client.
@param[in] call_id          Call ID.
@param[in] setup_params_ptr Pointer to mode-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM or FEATURE_WCDMA must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_setup_res(

  cm_call_cmd_cb_f_type         cmd_cb_func,
  void                          *data_block_ptr,
  cm_client_id_type             client_id,
  cm_call_id_type               call_id,
  const cm_setup_res_params_s_type   *setup_params_ptr

);
 
/*~ FUNCTION cm_mm_call_cmd_setup_res */
/*~ PARAM setup_params_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_pdp_modify

===========================================================================*/
/**
  Commands the CM to request a PDP modify command. This function commands CM to
  change the privacy preference during a call. The call ID passed has range from
  0 to CM_CALL_ID_MAX. The client ID should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func       Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Answering client.
@param[in] call_id           Call ID.
@param[in] modify_params_ptr Pointer to command data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM or FEATURE_WCDMA must be defined.
  
@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_pdp_modify(

  cm_call_cmd_cb_f_type               cmd_cb_func,
  void                                *data_block_ptr,
  cm_client_id_type                   client_id,
  cm_call_id_type                     call_id,
  const cm_pdp_modify_params_s_type   *modify_params_ptr

);
/*~ FUNCTION cm_mm_call_cmd_pdp_modify */
/*~ PARAM modify_params_ptr POINTER */

/*============================================================================

FUNCTION: cm_mm_call_cmd_pdp_nw_modify_res

===========================================================================*/
/**
  Commands the CM to respond to a network-inititated PDP modify request.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status.

@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Answering client.
@param[in] call_id            Call ID.
@param[in] act_res_params_ptr Pointer to command data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.
  
@dependencies
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM or FEATURE_WCDMA must be defined.
  
@sideeffects 
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_pdp_nw_modify_res(

  cm_call_cmd_cb_f_type               cmd_cb_func,
  void                                *data_block_ptr,
  cm_client_id_type                   client_id,
  cm_call_id_type                     call_id,
  const cm_pdp_act_res_params_s_type  *act_res_params_ptr

);
/*~ FUNCTION cm_mm_call_cmd_pdp_nw_modify_res */
/*~ PARAM act_res_params_ptr POINTER */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_rab_reestablish

===========================================================================*/
/**
  Commands the CM to send a RAB re-establish request. The call ID passed has a
  range from 0 to CM_CALL_ID_MAX. The client ID should be from a registered CM
  client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Answering client.
@param[in] call_id            Call ID.
@param[in] reestab_params_ptr Pointer to mode-specific data.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM or FEATURE_WCDMA must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.

*/
extern boolean cm_mm_call_cmd_rab_reestablish(

  cm_call_cmd_cb_f_type                 cmd_cb_func,
  void                                  *data_block_ptr,
  cm_client_id_type                     client_id,
  cm_call_id_type                       call_id,
  const cm_rab_reestab_params_s_type    *reestab_params_ptr

);

/*~ FUNCTION cm_mm_call_cmd_rab_reestablish */
/*~ PARAM reestab_params_ptr POINTER */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_ps_data_available

===========================================================================*/
/**
  Informs the CM that uplink data is available for transmission. The client ID
  should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM or FEATURE_WCDMA must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_ps_data_available(

  cm_call_cmd_cb_f_type          cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id

);

/*~ FUNCTION cm_mm_call_cmd_ps_data_available */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_ps_sig_release

===========================================================================*/
/**
  Requests the CM to release PS signaling. The client ID should be from a
  registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM or FEATURE_WCDMA must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_ps_sig_release(

  cm_call_cmd_cb_f_type          cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id

);
 
/*~ FUNCTION cm_mm_call_cmd_ps_sig_release */

/*============================================================================

FUNCTION: cm_mm_call_cmd_handover_req

===========================================================================*/
/**
  Commands the CM to complete the handoff between different RATs. The client ID
  should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. The error code is passed back to the
  client through the client-provided command callback function cmd_cb_func.
  If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@param[in] cmd_cb_func         Client callback function.
@param[in] data_block_ptr      Pointer to the client callback data block.
@param[in] client_id           Originating client.
@param[in] call_type           Call type (e.g., voice, data).
@param[in] call_state          Call state (originating, incoming, or
                               conversation).
@param[out] return_call_id_ptr Call ID will be allocated and returned.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_mm_call_cmd_handover_req
(

  cm_call_cmd_cb_f_type  cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  cm_call_type_e_type    call_type,
  cm_call_state_e_type   call_state,
  cm_call_id_type        *return_call_id_ptr

);

/*~ FUNCTION cm_mm_call_cmd_handover_req*/
/*~ PARAM OUT return_call_id_ptr POINTER */

/** @} */ /* end_addtogroup CALL Call-Related Definitions */

/** @addtogroup DSTF Data Session Types and Functions
    @{ */
/*
    This block groups all functionality that is associated with data session
    callback functions. When the CM ends a data call, it consults the data
    session callback functions for the total number of bytes received and
    transmitted during the data call.
*/

/** Data session registration status.
*/
typedef enum cm_data_session_status_e {

/** @cond
*/
  CM_DATA_SESSION_CB_ERR_NONE=-1,  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_DATA_SESSION_CB_OK,
    /**< No error; request was completed successfully. */

  CM_DATA_SESSION_ERR_NULL_DATA_SESSION_TOTAL_BYTE_CTRS_FUNC,
    /**< Null function provided for checking if a request is time constrained. */

  CM_DATA_SESSION_ERR_DATA_SESSION_TOTAL_BYTE_CTRS_FUNC_PREV_REG,
    /**< Get data session total byte counter function already registered. */

/** @cond
*/
  CM_DATA_SESSION_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_data_session_status_e_type;
/*~ SENTINEL cm_data_session_status_e_type.CM_DATA_SESSION_ERR_MAX */

/** @} */ /* end_addtogroup DSTF */

/** @addtogroup DSTF
    @{ */

/** Type for getting data session byte counters callback functions. This
    callback function is provided to the CM so that it can obtain the total
    number of bytes received and transmitted during a data call. \n

@note1hang The client-provided callback functions must be re-entrant, since
    they could be called from the client and the CM context simultaneously.
*/
typedef boolean (cm_get_data_session_total_byte_ctrs_cb_f_type) (

    cm_call_id_type                call_id,
    /**< Call ID. */

    uint32                        *bytes_received,
    /**< Number of bytes received during the call. */

    uint32                        *bytes_transmitted
    /**< Number of bytes transmitted during the call. */
);

 
/*~ CALLBACK cm_get_data_session_total_byte_ctrs_cb_f_type
    ONERROR return FALSE */
/*~ PARAM OUT bytes_received POINTER */
/*~ PARAM OUT bytes_transmitted POINTER */

/** @} */ /* end_addtogroup DSTF */

/** @addtogroup DSTF
    @{ */

/*===========================================================================

FUNCTION: cm_data_session_reg

===========================================================================*/

/**
  Registers data session callback functions with the CM. When the CM ends a data
  call, it consults the data session callback functions for the total number of
  bytes received and transmitted during the data call.

@param[in] data_session_total_byte_ctrs_cb_func_ptr Pointer to the data session
                                                    total byte counters callback
                                                    function.

@return
  Status of the registration.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern cm_data_session_status_e_type cm_data_session_reg(

  cm_get_data_session_total_byte_ctrs_cb_f_type
                                *data_session_total_byte_ctrs_cb_func_ptr

);
//MMODE: DSTF end

/*~ FUNCTION cm_data_session_reg */

/** @} */ /* end_addtogroup DSTF */

/** @addtogroup SUPS Supplementary Services Object
    @{ */

/** @brief Register Supplementary Services (SS) parameters.
*/
typedef struct cm_reg_ss_params_s {

  uint8                                ss_code;
    /**< Supplementary Services code. */

  uint8                                ss_ref;
    /**< Supplementary Services reference. */

  basic_service_s_type                 basic_service;
    /**< Basic service group information. */

  cm_called_party_bcd_no_s_type        fwd_to_num;
    /**< Forwarded to number. */

  cm_called_party_subaddress_s_type    fwd_to_subaddr;
    /**< Forwarded to subaddress. */

  ie_no_reply_condition_timer_s_type   nr_timer;
    /**< No reply condition timer. */

} cm_reg_ss_params_s_type;


/** @brief Erase a Supplementary Services request.
*/
typedef struct cm_erase_ss_params_s {

  uint8                     ss_ref;
    /**< Supplementary Services reference. */

  uint8                     ss_code;
    /**< Supplementary Services code. */

  basic_service_s_type      basic_service;
    /**< Basic service group information. */

} cm_erase_ss_params_s_type;


/** @brief Activate Supplementary Services parameters.
*/
typedef struct cm_act_ss_params_s {

  uint8                     ss_ref;
    /**< Supplementary Services reference. */

  uint8                     ss_code;
    /**< Supplementary Services code. */

  basic_service_s_type      basic_service;
    /**< Basic service group information. */

  
  ie_ss_password_s_type     ss_pwd;
    /**< Supplementary Services password. */

} cm_act_ss_params_s_type;

/** @brief Deactivate Supplementary Services parameters.
*/
typedef struct cm_deact_ss_params_s {

  uint8                     ss_ref;
    /**< Supplementary Services reference. */

  uint8                     ss_code;
    /**< Supplementary Services code. */

  basic_service_s_type      basic_service;
    /**< Basic service group information. */


  cm_ccbs_indx_type         ccbs_indx;
    /**< ccbs_indx = CM_CCBS_INDX_INVALID results in the deactivation of all
         pending CCBS requests. ccbs_indx = Valid results in one particular CCBS
         getting deactivated. Clients must check for CM_API_CCBS before using
         this field. This field is used only for ss_code = CCBS. */

  ie_ss_password_s_type     ss_pwd;
    /**< SS password. */

} cm_deact_ss_params_s_type;


/** @brief Interrogate Supplementary Services parameters.
*/
typedef struct
{
  uint8                     ss_ref;
    /**< Supplementary Services reference. */

  uint8                     ss_code;
    /**< Supplementary Services code. */

  basic_service_s_type      basic_service;
    /**< Basic service group information. */

  cm_ccbs_indx_type         ccbs_indx;
    /**< ccbs_indx = CM_CCBS_INDX_INVALID results in the deactivation of all
         pending CCBS requests. ccbs_indx = VALID results in one particular
         CCBS getting deactivated. Clients must check for CM_API_CCBS before
         using this field. This field is used only for ss_code = CCBS. */

} cm_interr_ss_params_s_type;


/** @brief Register password parameters.
*/
typedef struct cm_reg_pwd_params_s {

  ie_ss_code_s_type       ss_code;
    /**< Supplementary Services code. */

  uint8                   ss_ref;
    /**< Supplementary Services reference. */

  ie_ss_password_s_type   ss_old_pwd;
    /**< Old password. */

  ie_ss_password_s_type   ss_new_pwd;
    /**< New password. */

  ie_ss_password_s_type   ss_new_pwd_again;
    /**< Confirmation of new password. */

} cm_reg_pwd_params_s_type;

/** @brief Process Unstructured SS parameters.
*/
typedef  struct cm_uss_params_s {

  uint8                  ss_ref;
    /**< Supplementary Services reference. */

  uss_data_s_type        uss_data;
    /**< Process unstructured Supplementary Services request data. */

} cm_uss_params_s_type;


/** @brief Release Supplementary Services parameters.
*/
typedef struct cm_release_ss_params_s {

  uint8                ss_ref;
    /**< Supplementary Services reference. */

  uint8                ss_oper;
    /**< Supplementary Services operation to be released. */

} cm_release_ss_params_s_type;


/** @} */ /* end_addtogroup SUPS Suplementary Services Object */ 


/** @addtogroup NCTF Number Classification Types and Functions
    @{ */

/** @name Masks for Emergency Number Types
    Clients must check for CM_API_EMERGENCY_NUM_LIST before using these masks.
  @{ */

#define CM_EMERG_NUM_TYPE_NONE                        ( (uint32) 0 )
   /**< Clients must use this type when no
        category is defined for an emergency call. */

#define CM_EMERG_NUM_TYPE_POLICE_MASK                 ( BM(0) )
   /**< Police. */

#define CM_EMERG_NUM_TYPE_AMBULANCE_MASK              ( BM(1) )
   /**< Ambulance. */

#define CM_EMERG_NUM_TYPE_FIRE_BRIGADE_MASK           ( BM(2) )
   /**< Fire brigade. */

#define CM_EMERG_NUM_TYPE_MARINE_GUARD_MASK           ( BM(3) )
   /**< Marine guard. */

#define CM_EMERG_NUM_TYPE_MOUNTAIN_RESCUE_MASK        ( BM(4) )
   /**< Mountain rescue. */

#define CM_EMERG_NUM_TYPE_MANUAL_ECALL_MASK           ( BM(5) )
   /**< Manual emergency call. */

#define CM_EMERG_NUM_TYPE_AUTO_ECALL_MASK             ( BM(6) )
   /**< Automatic emergency call. */

/** @cond
*/
#define CM_EMERG_NUM_TYPE_MAX                         ( BM(7) )
   /* For bounds checking, CM INTERNAL USE ONLY */
/** @endcond
*/

/** @} */ /* end_namegroup */

/** @brief Type to hold mode-specific information returned by call
    control.
*/
typedef struct cm_orig_call_control_s {

  cm_cc_result_e_type                call_control_result;
    /**< Indicates whether there were modifications. */

  cm_alpha_s_type                    alpha;
    /**< Alpha tag. */

  cm_num_s_type                      num;
    /**< Modified number. */

  cm_activate_code_e_type            activate_code;
    /**< Associated OTASP activation code (CDMA only). */

  cm_cap_config_s_type               ccp1;
    /**< Capability configuration 1. */

  cm_cap_config_s_type               ccp2;
    /**< Capability configuration 2. */

  cm_sups_cmd_e_type                 sups_cmd;
    /**< Indicates to which SUPS command the dialed digits are
         modified. Must be filled in if the call_control_result
         indicates a change to SS or USSD. */

  /** One of the fields in this union must be filled in if
      call_control_result indicates a change to SS or USSD.
  */
  union
  {
    cm_erase_ss_params_s_type        erase_ss_params;
    cm_reg_ss_params_s_type          reg_ss_params;
    cm_act_ss_params_s_type          activate_ss_params;
    cm_deact_ss_params_s_type        deactivate_ss_params;
    cm_interr_ss_params_s_type       interrogate_ss_params;
    cm_uss_params_s_type             uss_params;
    cm_reg_pwd_params_s_type         reg_pwd_params;
    cm_release_ss_params_s_type      release_ss_params;
  } sups_params;

  sys_sys_mode_e_type               ps_cc_sys_mode;
    /**< System mode for PS call call control (ps_cc_sys_mode). Can be
         LTE or GW. */
  cm_gw_ps_cc_params_s_type          gw_ps_cc_params;
    /**< Call control parameters for a GW PS call. Clients must check for
         CM_API_GW_PS_CC before using this structure. */

  cm_lte_ps_cc_params_s_type         lte_ps_cc_params;
    /**< Call control parameters for a PS call. If ps_cc_sys_mode is LTE,
         use lte_cc_params. */

  cm_call_cmd_err_cause_e_type       error_cause;
    /**< Error cause in case of call control failure. This is to be read if
         call_control_result is CM_CC_RESULT_NOT_ALLOWED. */

} cm_orig_call_control_s_type;

/** Type for a callback function to be called by a client when
    all call control processing is complete.
*/
typedef void (cm_nc_call_control_complete_cb_f_type) (

  cm_call_id_type                         call_id,
    /**< Call ID. */

  cm_cc_status_e_type                     status,
    /**< Status of command. */

  cm_call_type_e_type                     call_type,
    /**< Call type. */

  const cm_orig_call_control_s_type       *cdma_call_control_info_ptr,
    /**< Pointer to an information struct (NULL if not allowed on CDMA). */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /**< Pointer to an information struct (NULL if not allowed on GW). */

  const void                              *user_data
   /**< Data passed from original call. */

);

/** Type for a call control function to be registered by a client. The CM calls
    this function during an origination to resolve call types, check if the call
    is allowed, and perform call control.
*/
typedef boolean (cm_nc_call_control_processing_f_type) (

  cm_call_id_type                       call_id,
    /**< Call ID. */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /**< Where the CM is getting its CDMA subscription information. */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /**< Where the CM is getting its GW subscription information. */

  cm_call_type_e_type                   call_type,
    /**< Call type. */

  cm_num_s_type                         *orig_num_ptr,
    /**< Originating number. */

  void                                  *user_data,
    /**< Data to be passed back in the callback function (clients should not
         modify this field). */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /**< Function to be called when processing is complete. */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params
    /**< Call control parameters for packet data call. Clients must check for
         CM_API_GW_PS_CC before using this structure. */
);

/** Type for a call control function to be registered by a client.The CM calls
    this function during an origination to resolve call types, checks whether
    the call is allowed, and performs call control.
*/
typedef boolean (cm_nc_call_control_processing_extn_f_type) (

  cm_call_id_type                       call_id,
    /**< Call ID. */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /**< Where the CM is getting its CDMA subscription information. */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /**< Where the CM is getting its GW subscription information. */

  cm_call_type_e_type                   call_type,
    /**< Call type. */

  cm_num_s_type                         *orig_num_ptr,
    /**< Originating number. */

  void                                  *user_data,
    /**< Data to be passed back in the CB function (clients should not modify
         this field). */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /**< Function to be called when processing is complete. */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params
    /**< Call control parameters for Packet data call. Clients must check for
         CM_API_GW_PS_CC before using this structure. */

);

/** Type for a call control preferences function to be registered by a client.
    The CM calls this function after call control is complete to mediate between
    which modes to use in the case where the call is allowed on both modes.
*/
typedef cm_cc_mode_pref_e_type (cm_nc_call_control_preferences_f_type) (

  cm_orig_call_control_s_type            *cdma_call_control_info_ptr,
    /**< CDMA information for the call. */

  cm_orig_call_control_s_type            *gw_call_control_info_ptr,
    /**< GW information for the call. */

  cm_num_s_type                          *orig_num_ptr,
    /**< Original number information before call control. */

  cm_call_type_e_type                    call_type,
    /**< The type of call returned by call control. */

  sys_sys_mode_e_type                    current_mode
    /**< Mode, if any, that is currently acquired. */

);

/** Type for a call control function to be registered by a client. The CM calls
    this function during an origination to resolve call types, check if the call
    is allowed, and perform call control, with or without FDN checking.
*/
typedef boolean (cm_nc_call_control_processing_mod_f_type) (

  cm_call_id_type                       call_id,
    /**< Call ID. */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /**< Where the CM is getting its CDMA subscription information. */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /**< Where the CM is getting its GW subscription information. */

  cm_call_type_e_type                   call_type,
    /**< Call type. */

  cm_num_s_type                         *orig_num_ptr,
    /**< Originating number. */

  void                                  *user_data,
    /**< Data to be passed back in the CB function (clients should not modify
         this field). */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /**< Function to be called when processing is complete. */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params,
    /**< Call control parameters for packet data call. Clients must check for
         CM_API_GW_PS_CC before using this structure. */

  const cm_cc_generic_params_s_type   *cc_generic_params
    /**< Call control generic parameters for a call. */

);

/** Type for a call control function to be registered by a client. The CM calls
    this function during an origination to resolve call types, check whether the
    call is allowed, and perform call control, with or without FDN checking.
*/
typedef boolean (cm_nc_call_control_processing_mod_extn_f_type) (

  cm_call_id_type                       call_id,
    /**< Call ID. */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /**< Where the CM is getting its CDMA subscription information. */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /**< Where the CM is getting its GW subscription information. */

  cm_call_type_e_type                   call_type,
    /**< Call type. */

  cm_num_s_type                         *orig_num_ptr,
    /**< Originating number. */

  void                                  *user_data,
    /**< Data to be passed back in the CB function (clients should not modify
         this field). */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /**< Function to be called when processing is complete. */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params,
    /**< Call control parameters for Packet data call. Clients must check for
         CM_API_GW_PS_CC before using this structure. */

  const cm_cc_generic_params_s_type     *cc_generic_params
    /**< Call control; generic parameters for the call. */
  
);

/** @} */ /* end_addtogroup NCTF */

/** @addtogroup NCTF
    @{ */

/*============================================================================

FUNCTION: cm_nc_reg_with_call_control

===========================================================================*/

/**
  Registers number classification callback functions with the CM. When the CM is
  commanded to originate a call, it consults the number classification callback
  functions for the following information:

  - Whether the originating number matches one of the special numbers that
    are stored in the number classification (e.g., Emergency, OTASP). If a
    match is found, the CM modifies the originating call type accordingly.
  - Whether the originating number is allowed on the current SIM cards. If the
    calls are allowed and call control is required, the CM performs any
    necessary modifications.

@param[in] cm_nc_call_control_processing_func Pointer to the function that will
                                              resolve call type, check if the
                                              call is allowed, perform any
                                              necessary call control
                                              modifications, and check for any
                                              pre-pending dial information for
                                              JCDMA.

@param[in] cm_nc_call_control_preferences_func Pointer to the function that will
                                               resolve which mode to originate
                                               on when the call is allowed on
                                               both slots and/or the
                                               modifications are conflicting.

@return
  Request status.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern cm_nc_status_e_type cm_nc_reg_with_call_control(

  cm_nc_call_control_processing_f_type    *cm_nc_call_control_processing_func,
  cm_nc_call_control_preferences_f_type   *cm_nc_call_control_preferences_func

);

/*===========================================================================

FUNCTION: cm_nc_reg_with_call_control_extn

===========================================================================*/
/**
   Registers number classification callback functions with the CM. 

   When the CM is commanded to originate a call, it consults the number
   classification callback function for the following information:

   - Whether the originating number matches one of the special numbers that
     are stored in the number classification (e.g., Emergency, OTASP).
     If a match is found, the CM modifies the originating call type
     accordingly.
   - If the originating number is allowed on the current SIM cards, the CM
     performs any necessary modifications.

@param[in] cm_nc_call_control_processing_func Pointer to the function that will
                                              resolve call type, check if the
                                              call is allowed, perform any
                                              necessary call control
                                              modifications, and check for any
                                              pre-pending dial information for
                                              JCDMA.

@param[in] cm_nc_call_control_preferences_func Pointer to the function that will 
                                               resolve which mode to originate
                                               on when the call is allowed
                                               on both slots and/or the
                                               modifications are conflicting.

@return
  Request status.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern cm_nc_status_e_type cm_nc_reg_with_call_control_extn(

  cm_nc_call_control_processing_extn_f_type 
                                  *cm_nc_call_control_processing_func,
  cm_nc_call_control_preferences_f_type *cm_nc_call_control_preferences_func

);

/*===========================================================================

FUNCTION: cm_nc_reg_with_call_control_mod

===========================================================================*/
/**
  Modifies registered number classification callback functions with the CM,
  with the option to not check for FDN.

  When the CM is commanded to originate a call, it consults the number
  classification callback function for the following information:

  - Whether the originating number matches one of the special numbers that
    are stored in the number classification (e.g., Emergency, OTASP). If a
    match is found, the CM modifies the originating call type accordingly.
  - If the originating number is allowed on the current SIM cards, the CM
    performs any necessary modifications.

@param[in] cm_nc_call_control_processing_func_mod Pointer to the function that
                                                  will resolve call type, check
                                                  if the call is allowed,
                                                  perform any necessary call
                                                  control modifications, and
                                                  check for any pre-pending dial
                                                  information for JCDMA.

@param[in] cm_nc_call_control_preferences_func Pointer to the function that will
                                               resolve which mode to originate
                                               on when the call is allowed on
                                               both slots and/or the
                                               modifications are conflicting.

@return
  Request status.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern cm_nc_status_e_type cm_nc_reg_with_call_control_mod(

  cm_nc_call_control_processing_mod_f_type    *cm_nc_call_control_processing_func_mod,
  cm_nc_call_control_preferences_f_type   *cm_nc_call_control_preferences_func

);

/*===========================================================================

FUNCTION: cm_nc_reg_with_call_control_mod_extn

===========================================================================*/
/**
  Registers number classification callback functions with the CM.

  When the CM is commanded to originate a call, it consults the number
  classification callback function for the following information:
 
  - Whether the originating number matches one of the special numbers that
    are stored in the number classification(e.g., Emergency, OTASP).
    If a match is found, the CM modifies the originating call type
    accordingly.
  - If the originating number is allowed on the current SIM cards and call
    control is required, the CM performs any necessary modifications.

@param[in] cm_nc_call_control_processing_func_mod Pointer to the function that
                                                  will resolve call type, check
                                                  if the call is allowed,
                                                  perform any necessary call
                                                  control modifications, and
                                                  check for any pre-pending dial
                                                  information for JCDMA.

@param[in] cm_nc_call_control_preferences_func Pointer to the function that will
                                               resolve which mode to originate
                                               on when the call is allowed on
                                               both slots and/or the
                                               modifications are conflicting.

@return
  Request status.
    
@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern cm_nc_status_e_type cm_nc_reg_with_call_control_mod_extn(

  cm_nc_call_control_processing_mod_extn_f_type
                                    *cm_nc_call_control_processing_func_mod,
  cm_nc_call_control_preferences_f_type
                                        *cm_nc_call_control_preferences_func

);


/** @} */ /* end_addtogroup NCTF Number Classification Types and Functions */

/*-----------------------------------------------------------------------------

Phone Object

-----------------------------------------------------------------------------*/

/** @addtogroup PHONE Phone-Related Definitions
    @{ */

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/** Invalid User Zone ID.
*/
#define CM_INVALID_UZID         (-1)

/** @} */ /* end_addtogroup PHONE */

/** @addtogroup PHONE
    @{ */

/** Enumeration of phone commands clients are able to send to the CM.
*/
typedef enum cm_ph_cmd_e {

/** @cond
*/
  CM_PH_CMD_NONE=-1,  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_PH_CMD_OPRT_MODE,
    /**< Change the operating mode. */

  CM_PH_CMD_SYS_SEL_PREF,
    /**< Change the system selection preference (SD20). */

  CM_PH_CMD_ANSWER_VOICE,
    /**< Change the answer-voice-as-data setting (CDMA only). */

  CM_PH_CMD_NAM_SEL,
    /**< Change the Name and Address Module (NAM) selection (CDMA only). */

  /* 4 */
  CM_PH_CMD_CLIENT_ACT,
    /* Command to activate a client and send by that particular client. */

  CM_PH_CMD_INFO_GET,
    /**< Get a snapshot of the phone information in the CM. */

  CM_PH_CMD_RSSI_DELTA,
    /**< Set Reverse Signal Strength Indicator (RSSI); change delta threshold 
         for RSSI reporting. */

  CM_PH_CMD_SUBSCRIPTION_AVAILABLE,
    /**< Subscription information available for use. */

  /* 8 */
  CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE,
    /**< Subscription information not available for use. */

  CM_PH_CMD_SUBSCRIPTION_CHANGED,
    /**< Subscription information changed. */

  CM_PH_CMD_GET_NETWORKS,
    /**< Get the preferred or available networks (GSM/WCDMA only). */

  CM_PH_CMD_TERMINATE_GET_NETWORKS,
    /**< Terminate the request to get the preferred or available networks
         (GSM/WCDMA only). */

  /* 12 */
  CM_PH_CMD_SET_PREFERRED_NETWORKS,
    /**< Client made a request to save the preferred network list to the
         SIM/USIM (GSM/WCDMA only). */

  CM_PH_CMD_PACKET_STATE,
    /**< Set Rm packet state. CDMA only. */

  CM_PH_CMD_VOC_ACTIVITY,
    /**< Voice activity (CDMA only; FEATURE_JCDMA). */

  CM_PH_CMD_AVOID_SYS,
    /**< Command to avoid a particular system (CDMA only). */

  /* 16 */
  CM_PH_CMD_WAKEUP_FROM_STANDBY,
    /**< Command to wake up lower layers from deep sleep (standby). */

  CM_PH_CMD_CHANGE_RTRE_CONFIG,
    /**< Command to change Runtime RUIM Enable (RTRE) configuration (CDMA only). */

  CM_PH_CMD_DDTM_PREF,
    /**< Data-dedicated transmission mode preference. */

  CM_PH_CMD_RESET_ACM,
    /**< Reset the Accumulated Call Meter (ACM) to 0. This also resets the
         ability for the CM to send the FUNDS_LOW message. */

  /* 20 */
  CM_PH_CMD_SET_ACMMAX,
    /**< Set ACMmax to the specified value. This command could cause the 
         FUNDS_LOW message to be resent if the new value would cause calls to
         end within an estimated 30 seconds. */

  CM_PH_CMD_LINE_SWITCHING,
    /**< Allow or prevent the user from switching lines (i.e., changing 
         selected line). Sent to the CM by the UI client (FEATURE_ALS). */

  CM_PH_CMD_SELECTED_LINE,
    /**< Set the selected line. Sent to the CM by the UI client (FEATURE_ALS). */

  CM_PH_CMD_ALS_SIM_INFO_AVAIL,
    /**< All Alternate Line Service (ALS) information is available. Sent to the
         CM by the ALS when all ALS related items have been read from the SIM
         (FEATURE_ALS). */

  /* 24 */
  CM_PH_CMD_LINE_SWITCHING_SAVED,
    /**< Update the setting in the CM and notify clients. Sent to the CM by the 
         ALS when a line switching setting has been written to the SIM
         (FEATURE_ALS). */

  CM_PH_CMD_SELECTED_LINE_SAVED,
    /**< Update the setting in the CM and notify clients. Sent to the CM by the 
         ALS when the selected line has been written to the SIM (FEATURE_ALS). */

  CM_PH_CMD_UPDATE_CDMA_CAPABILITY,
    /**< Update the CDMA capability information. */

/** @cond
*/
  CM_PH_CMD_CLIENT_RELEASE, /* FOR INTERNAL CM USE ONLY! 
       Release the client object associated with the deregistering client. */
/** @endcond
*/

  /* 28 */
  CM_PH_CMD_SIGNAL_STRENGTH_DELTA,
    /**< Set the RSSI, Ec/Io, and IO change delta threshold for signal strength 
         reporting. */

  /* 29 */
  CM_PH_CMD_WPS_START,
    /**< Command to get WLAN security credentials using the WiFi Positioning
         System (WPS) procedure. */

  /* 30 */
  CM_PH_CMD_WPS_ABORT,
    /**< Command to abort the WPS procedure. */

  /* 31 */
  CM_PH_CMD_MEAS_MODE,
    /**< Command to put the MS in measurement mode. Currently applicable only 
         for High Data Rate (HDR). In this mode, HDR disables network
         registration. */

  /* 32 */
  CM_PH_CMD_SET_USER_NET_SEL_MODE,
    /**< Command to set user-selected net_sel_mode. */    

  /* 33 */
  CM_PH_CMD_DUAL_STANDBY_PREF,
    /**< Command to change the Dual Standby preferences. */

  /* 34 */
  CM_PH_CMD_SUBSCRIPTION_INFO_GET,
    /**< Get a snapshot of the Subscription information in the CM */

  /* 35 */
  CM_PH_CMD_PLMN_BLOCKING,
    /**< Command to LTE detach and/or PLMN blocking */

   /** @cond
   */
   CM_PH_CMD_MAX   /* FOR INTERNAL CM USE ONLY! */
   /** @endcond
   */


} cm_ph_cmd_e_type;
/*~ SENTINEL cm_ph_cmd_e_type.CM_PH_CMD_MAX */

/** Enumeration of phone command errors returned by phone command
    callback functions.
*/
typedef enum cm_ph_cmd_err_e {

/** @cond
*/
  CM_PH_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_PH_CMD_ERR_NOERR,
    /**< No errors found. */

  CM_PH_CMD_ERR_ANSWER_VOICE_F,
    /**< Answer voice feature is not defined. */

  /* Error codes resulting from clients feeding a phone command
     function with bad or inappropriate parameters. */

  CM_PH_CMD_ERR_CLIENT_ID_P,
    /**< Invalid client ID; NULL or not initialized. */

  CM_PH_CMD_ERR_OPRT_MODE_P,
    /**< Bad operating mode parameter. */

  /* 4 */
  CM_PH_CMD_ERR_MODE_PREF_P,
    /**< Bad mode preference parameter. */

  CM_PH_CMD_ERR_PREF_TERM_P,
    /**< Bad mode term parameter. */

  CM_PH_CMD_ERR_SYS_PREF_P,
    /**< Bad system preference parameter (obsolete in SD 2.0). */

  CM_PH_CMD_ERR_BAND_PREF_P,
    /**< Bad band preference parameter. */

  /* 8 */
  CM_PH_CMD_ERR_ROAM_PREF_P,
    /**< Bad roam preference parameter. */

  CM_PH_CMD_ERR_ANSWER_VOICE_P,
    /**< Bad answer voice parameter (CDMA only). */

  CM_PH_CMD_ERR_ANSWER_DURATION_P,
    /**< Bad answer duration parameter. When indicating a temporary setting,
         such as CM_ANSWER_VOICE_AS_FAX_ONCE or DB_VOICE_AS_MODEM_ONCE,
         a non-zero duration must be indicated as well (CDMA only). */

  CM_PH_CMD_ERR_NAM_P,
    /**< Bad NAM parameter (CDMA only). */

  /* 12 */
  CM_PH_CMD_ERR_PACKET_STATE_P,
    /**< Bad packet state parameter (CDMA only). */

  CM_PH_CMD_ERR_HYBR_PREF_P,
   /**< Hybrid preference. */

  CM_PH_CMD_ERR_SERVICE_DOMAIN_P,
    /**< Invaild service domain parameter (GSM/WCDMA only). */

  CM_PH_CMD_ERR_ACQ_ORDER_PREF_P,
    /**< Invalid acquisition order preference parameter (GSM/WCDMA only). */

  /* 16 */
  CM_PH_CMD_ERR_NETWORK_LIST_TYPE_P,
    /**< Invalid network list type parameter (GSM/WCDMA only). */

  CM_PH_CMD_ERR_NETWORK_SEL_MODE_P,
    /**< Invalid network selection mode parameter (GSM/WCDMA only). */

  CM_PH_CMD_ERR_SYSTEM_MODE_P,
    /**< Invalid system mode parameter (GSM/WCDMA only). */

  CM_PH_CMD_ERR_PLMN_P,
    /**< Invalid PLMN identifier parameter (GSM/WCDMA only). */

  /* 20 */
  CM_PH_CMD_ERR_RTRE_CONFIG_P,
    /**< Invalid RTRE configuration parameter (CDMA only). */

  CM_PH_CMD_ERR_CDMA_CAPABILITY_P,
    /**< Invalid CDMA capability parameter error. */

  CM_PH_CMD_ERR_PRL_PREF_P,
    /**< Bad Preferred Roaming List (PRL) preference parameter. */

  CM_PH_CMD_ERR_WLAN_PREF_P,
    /**< Bad WLAN preference parameter. Clients must check CM_API_WLAN for WLAN
         support before using this field. */

  /* Error codes resulting from clients trying to instruct the
  ** CM to perform inappropriate actions relative to the current
  ** state of the phone.
  */

  /* 24 */
  CM_PH_CMD_ERR_IN_USE_S,
    /**< Phone is in-use (i.e., in a call). */

  CM_PH_CMD_ERR_ONLINE_S,
    /**< Phone is offline. */

  CM_PH_CMD_ERR_OPRT_MODE_S,
    /**< Invalid command or operating mode switch relative to the current
         operating mode (e.g., OFFLINE -> ONLINE). */

  CM_PH_CMD_ERR_MODE_PREF_S,
    /**< Invalid mode preference selection relative to what is
         currently in the PRL (e.g., ANALOG_ONLY when there are no
         analog entries in the PRL). */

  /* 28 */
  CM_PH_CMD_ERR_SYS_PREF_S,
    /**< Invalid system preference selection for this target
         (e.g., A_ONLY for a non-800 MHz target) (obsolete in SD20). */

  CM_PH_CMD_ERR_SYS_SEL_PREF_S,
    /**< Invalid system selection preference for this target/PRL
         (e.g., band class 1 for a PCS-only target). Also returned when 
         CM_NETWORK_SEL_MODE_PREF_HPLMN_SRCH is requested in a 
         mode other than Automatic mode. */

  CM_PH_CMD_ERR_HYBR_PREF_S,
    /**< Invalid command or hybrid preference selection (FEATURE_HDR). */

  CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S,
    /**< Invalid command relative to the current value of Subscription
         Available (e.g., transitioning from Subscription Available to
         Subscription Available = Invalid or issuing a command when
         the subscription information is not available). */

  /* 32 */
  CM_PH_CMD_ERR_GET_NETWORKS_S,
    /**< A Terminate Get Networks request was made and a request for 
         an available network list was not previously made, OR a
         Get Network request was made while a previous Get Network
         request for a different technology was still pending. */

  /* Error codes resulting from operational limitation
     of the CM. */

  CM_PH_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  CM_PH_CMD_ERR_MC_COM_L,
    /**< MC communication problems. */

  /* Error codes related to an Avoid System command. */

  CM_PH_CMD_ERR_AVOID_SYS_P,
      /**< At least one of the parameters in an Avoid System
           command from a phone client was invalid (CDMA only). */

  /* 36 */
  CM_PH_CMD_ERR_SYS_NOT_IN_UZ,
      /**< Indicates that the MS is not in the user zone when
           Avoid System on the user zone was processed (CDMA only). */

  CM_PH_CMD_ERR_OTHER,
    /**< An error other than the above was found. */

  CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to a relevant feature being undefined. */

  CM_PH_CMD_ERR_CURRENT_NAM_IS_NOT_RUIM_NAM_S,
    /**< Current NAM is not RUIM NAM (CDMA only). */

  /* 40 */
  CM_PH_CMD_ERR_USER_PREFERRED_NETWORK_LIST_ACCESS,
    /**< An error occurred accessing the user preferred network list 
         (GSM/WCDMA only). */

  CM_PH_CMD_ERR_DDTM_PREF_P,
    /**< Bad Data Dedicated Transmission Mode (DDTM) preference parameter. */

  CM_PH_CMD_ERR_DDTM_NUM_SRV_OPTIONS_P,
    /**< Bad number of service options. */

  CM_PH_CMD_ERR_LINE_SWITCHING_P,
    /**< Invalid line switching parameter (FEATURE_ALS). */

  /* 44 */
  CM_PH_CMD_ERR_LINE_SWITCHING_S,
    /**< Client requested that the selected line be changed
         but line switching is not allowed (FEATURE_ALS). */

  CM_PH_CMD_ERR_SELECTED_LINE_P,
    /**< Invalid selected line parameter (FEATURE_ALS). */

  CM_PH_CMD_ERR_ALS_NOT_SUPPORTED_S,
    /**< ALS is not supported and therefore an ALS
         command cannot be processed (FEATURE_ALS). */

  CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S,
    /**< SIM write failed (FEATURE_ALS). */

  CM_PH_CMD_ERR_AOC_NOT_READY_S,
     /**< Values related to Advice of Charge cannot be read or written 
          at this time. Clients must check for CM_API_AOC_INFO before
          using this enum. */

  /* 49 */
  CM_PH_CMD_ERR_NET_TYPE_P,
    /**< Invalid network type requested in the cm_ph_cmd_get_networks API. 
         Clients must check CM_API_WLAN for WLAN support. */

  CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P,
    /**< Bad service options pointer. */

  CM_PH_CMD_ERR_INVALID_HYBR_PREF_MODE_PREF,
    /**< Invalid hybrid preference and mode preference combination. */

  /* 53 */
  CM_PH_CMD_ERR_MODE_PREF_CHANGE_NOT_ALLOWED_S,
    /**< Mode preference change is not allowed. */

  CM_PH_CMD_ERR_INVALID_WPS_MODE,
    /**< Invalid WPS mode used in WPS start command. */

  CM_PH_CMD_ERR_INVALID_WPS_CONFIG_METHOD,
    /**< Invalid WPS configuration method used in WPS start command. */

  CM_PH_CMD_ERR_INVALID_WPS_DEV_PASSWD,
    /**< Invalid WPS device password used in WPS start command. */

  /* 57 */
  CM_PH_CMD_ERR_NO_WPS_IN_PROGRESS,
    /**< Client request to abort WPS even when it is not started. */

  CM_PH_CMD_ERR_WPS_ALREADY_IN_PROGRESS,
    /**< Client sent another WPS Start request to abort WPS before 
         the previous request has been completed. */

  CM_PH_CMD_ERR_NO_SUBSCRIPTION_S,
    /**< Phone command cannot be processed because subscription  
         is not available (WLAN only). */

  CM_PH_CMD_ERR_API_NOT_SUPPORTED_S,
     /**< API is not supported for the client. */

  /* 61 */
  CM_PH_CMD_ERR_INVALID_USER_NET_SEL_MODE_S,
    /**< User provided an invalid user_net_sel_mode. */

  CM_PH_CMD_ERR_STANDBY_PREF_P,
    /**< Standby Preferences command cannot be processed due to
         errors in the parameters passed. */

  CM_PH_CMD_ERR_STANDBY_PREF_S,
    /**< Standby Preferences command processing failed due to
         invalid state. */

  CM_PH_CMD_ERR_INVALID_SUBSCRIPTION_P,
    /**< Invalid subscription ID is sent as part of the phone command. */

  CM_PH_CMD_ERR_PLMN_BLOCKING_P,
    /**< Invalid plmn blocking parameter. */

  /** @cond
  */
  /* 66 */
  CM_PH_CMD_ERR_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_ph_cmd_err_e_type;
/*~ SENTINEL cm_ph_cmd_err_e_type.CM_PH_CMD_ERR_MAX */


/** Enumeration of phone events. Clients are able to register with the CM to be
    notified of any subsets of these phone events.
*/
typedef enum cm_ph_event_e {

/** @cond
*/
  CM_PH_EVENT_NONE=-1,  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_PH_EVENT_OPRT_MODE,
    /**< Operating mode was changed. */

  CM_PH_EVENT_TEST_CONTROL_TYPE,
    /**< Operating mode type is to be updated. */

  CM_PH_EVENT_SYS_SEL_PREF,
    /**< System selection preference was changed. */

  CM_PH_EVENT_ANSWER_VOICE,
    /**< Answer voice as data was changed (CDMA only). */

  /* 4 */
  CM_PH_EVENT_NAM_SEL,
    /**< NAM selection was changed (CDMA only). */

  CM_PH_EVENT_CURR_NAM,
    /**< Current NAM was changed (CDMA only). */

  CM_PH_EVENT_IN_USE_STATE,
    /**< In use state was changed. */

  CM_PH_EVENT_CDMA_LOCK_MODE,
    /**< CDMA lock mode was changed (CDMA only). */

  /* 8 */
  CM_PH_EVENT_UZ_CHANGED,
    /**< User zone selection was changed (CDMA only). */

  CM_PH_EVENT_MAINTREQ,
    /**< CDMA maintenance required command (CDMA only). */

  CM_PH_EVENT_STANDBY_SLEEP,
    /**< Entering powerdown sleep mode. */

  CM_PH_EVENT_STANDBY_WAKE,
    /**< Existing powerdown sleep mode. */

  /* 12 */
  CM_PH_EVENT_INFO,
    /**< Phone information. This event is only sent to the client
         that requested this information through cm_ph_cmd_get_ph_info. */

  CM_PH_EVENT_PACKET_STATE,
    /**< Packet state was changed (CDMA only). */

  CM_PH_EVENT_INFO_AVAIL,
    /**< Phone information is now available. Automatically generated
         after a client performs an activation. */

  CM_PH_EVENT_SUBSCRIPTION_AVAILABLE,
    /**< A client is informing the CM that the subscription information 
         is available. */

  /* 16 */
  CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE,
    /**< A client is informing the CM that the subscription information
         is not available. */

  CM_PH_EVENT_SUBSCRIPTION_CHANGED,
    /**< A client is informing the CM that the subscription information
         has changed. */

  CM_PH_EVENT_AVAILABLE_NETWORKS_CONF,
    /**< The requested networks are available (GSM/WCDMA/WLAN only). */

  CM_PH_EVENT_PREFERRED_NETWORKS_CONF,
    /**< The requested preferred networks are available (GSM/WCDMA only). */

  /* 20 */
  CM_PH_EVENT_FUNDS_LOW,
    /**< Funds running low (GSM/WCDMA only). */

  CM_PH_EVENT_WAKEUP_FROM_STANDBY,
    /**< A command is sent to the lower layers to wake up from 
         deep sleep (standby). */

  CM_PH_EVENT_NVRUIM_CONFIG_CHANGED,
    /**< RTRE configuration has changed (CDMA only). */

  CM_PH_EVENT_PREFERRED_NETWORKS_SET,
    /**< The requested preferred networks were written to the USIM
         (GSM/WCDMA only). */

  /* 24 */
  CM_PH_EVENT_DDTM_PREF,
    /**< DDTM preference changed. */

  CM_PH_EVENT_PS_ATTACH_FAILED,
    /**< A client request for Packet Service attach failed. Sent if the
         cm_ph_cmd_sys_sel_pref() API is called with the srv_domain_pref
         parameter set to CM_SRV_DOMAIN_PREF_PS_ATTACH and the PS attach
         fails. */

  CM_PH_EVENT_RESET_ACM_COMPLETED,
    /**< As requested, the ACM has been reset on the USIM (GSM/WCDMA only). */

  CM_PH_EVENT_SET_ACMMAX_COMPLETED,
    /**< The requested value was written to the USIM (GSM/WCDMA only). */

  /* 28 */
  CM_PH_EVENT_CDMA_CAPABILITY_UPDATED,
    /**< A event to indicate that the CDMA capability was updated. */

  CM_PH_EVENT_LINE_SWITCHING,
    /**< The Line Switching setting has been updated in the CM,
         as per the client's request (FEATURE_ALS). */

   CM_PH_EVENT_SELECTED_LINE,
    /**< The selected line setting has been updated in the CM
         as per the client's request (FEATURE_ALS). */

  CM_PH_EVENT_SECONDARY_MSM,
    /**< Invoke event call back even if the event is from a secondary MSM. The
         CM will not generate any events with this, but it is used by the thin
         client CM layer to call client callbacks for events from a secondary
         MSM. */

  /* 32 */
  CM_PH_EVENT_TERMINATE_GET_NETWORKS,
    /**< Indicates that the current Get Network request is terminated. */

  CM_PH_EVENT_DDTM_STATUS,
    /**< Update the DDTM status. DDTM status is ON if either DDTM_PREF is ON
         and GPS fix is in progress, OR if DDTM_PREF is ON and there is a hybrid
         HDR call, no MO SMS, and no MO DBM. The event is triggered if the DDTM
         status changes. */

  CM_PH_EVENT_CCBS_STORE_INFO_CHANGED,
    /**< Event generated when an entry is added to or removed from
         ccbs_store_info of phone event structure. Clients must check for
         CM_API_CCBS before using this event. */

  CM_PH_EVENT_WPS_COMPLETED,
    /**< Event generated when the WPS procedure has been completed. 
         Completed does not necessarily mean any credential was obtained.
         Clients must check for CM_API_WLAN_WPS. */

  /* 36 */
  CM_PH_EVENT_WPS_ABORTED,
    /**< Event generated when the WPS procedure is aborted by a client. 
         Clients must check for CM_API_WLAN_WPS. */

  CM_PH_EVENT_WPS_START,
    /**< Event generated when the WPS security credentials are requested 
         by the clients. Clients must check for CM_API_WLAN_WPS. */

  CM_PH_EVENT_PRL_INIT,
    /**< Event generated when the PRL is loaded. It is triggerred by a report
         from the SD. */
   
  CM_PH_EVENT_MEAS_MODE,
    /**< Event generated when the measurement mode change is requested by a 
         client. */
   
  /* 40 */
  CM_PH_EVENT_DUAL_STANDBY_PREF,
    /**< GW system preferences changed. */

  CM_PH_EVENT_SUBSCRIPTION_PREF_INFO,
    /**< Event generated when subscription information has changed.\ 
         It contains all the information related to a particular subscription.
    */

  CM_PH_EVENT_THERMAL_EMERG_ENTER,
    /**< Event generated when CM enters emergency mode due to unsafe phone
         temperatures. Only emergency calls are allowed in this mode */

  CM_PH_EVENT_THERMAL_EMERG_EXIT,
    /**< Event generated when CM exits thermal emergency mode, due to normal
         phone temperatures. Phone is in normal operation. */

  /** @cond
  */
  CM_PH_EVENT_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */


} cm_ph_event_e_type;
/*~ SENTINEL cm_ph_event_e_type.CM_PH_EVENT_MAX */

/** The current state of the phone.
*/
typedef enum cm_ph_state_e {

/** @cond
*/
  CM_PH_STATE_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_PH_STATE_NORMAL,
    /**< The phone is in normal operation. */

  CM_PH_STATE_EMERG_CB,
    /**< The phone is in Emergency callback mode. */

/** @cond
*/
  CM_PH_STATE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ph_state_e_type;
/*~ SENTINEL cm_ph_state_e_type.CM_PH_STATE_MAX */


/* For compatibility with the QXDM database, please do not change the following
** fixed assigned values. If new values are added, please also update the QXDM
** database. */

/** Enumeration of mode preferences.
*/
typedef enum cm_mode_pref_e {

/** @cond
*/
  CM_MODE_PREF_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_MODE_PREF_AMPS_ONLY=0,
    /**< Service is limited to analog only (NV_MODE_ANALOG_ONLY). */

  CM_MODE_PREF_DIGITAL_ONLY=1,
    /**< Service is limited to digital (CDMA, HDR, or GW) only
         (NV_MODE_DIGITAL_ONLY). */

  CM_MODE_PREF_AUTOMATIC=2,
    /**< Determine the mode automatically (NV_MODE_AUTOMATIC). */

/** @cond
*/
  CM_MODE_PREF_EMERGENCY=3,
    /* = NV_MODE_EMERGENCY, Emergency mode. This is intended to be used internally
         by the CM. The client is not allowed to use it as a parameter to change a 
         system selection preference command. */
/** @endcond
*/

  /* For compatibility with QPST, do not change values or
     order. We start with NV_MODE_CELL_CDMA_ONLY+1 (i.e. 9). */

  CM_MODE_PREF_CDMA_ONLY = 9,
    /**< Service is limited to CDMA only. */

  CM_MODE_PREF_HDR_ONLY=10,
    /**< Service is limited to HDR only. */

  CM_MODE_PREF_CDMA_AMPS_ONLY=11,
    /**< Service is limited to CDMA and AMPS only. */

  CM_MODE_PREF_GPS_ONLY=12,
    /**< Service is limited to GPS only. */

  /* The following enums are the radio access technologies for
     WCDMA and GSM. */

  CM_MODE_PREF_GSM_ONLY=13,
    /**< Service is limited to GSM only. */

  CM_MODE_PREF_WCDMA_ONLY=14,
    /**< Service is limited to WCMDA only. */

  /* Others */

  CM_MODE_PREF_PERSISTENT=15,
    /**< Return to persistent mode preference. Will be mapped to something else
         by the CM; ph_ptr->mode_pref is never set to this value. */

  CM_MODE_PREF_NO_CHANGE=16,
    /**< Do not change the mode preference. Will be mapped to something else by
         the CM; ph_ptr->mode_pref is never set to this value. */

  /* Even though logically the below belong with enums
     listed above, they were added at the end so as
     not to break compatibility with test tools.
  */

  CM_MODE_PREF_ANY_BUT_HDR=17,
    /**< Use any service but HDR. */

  CM_MODE_PREF_CURRENT_LESS_HDR=18,
    /**< Remove the HDR component from the current mode preference. Will be
         mapped to something else by the CM; ph_ptr->mode_pref is never set to
         this value. */

  CM_MODE_PREF_GSM_WCDMA_ONLY=19,
    /**< Service is limited to GSM and WCDMA only. */

  CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY=20,
    /**< Acquire digital, non-HDR mode, systems only (CDMA, GSM, or WCDMA). */

  CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS=21,
    /**< Remove the HDR and AMPS components from the current mode preference.  
         Will be mapped to something else by the CM; ph_ptr->mode_pref is never
         set to this value. */

  CM_MODE_PREF_CDMA_HDR_ONLY=22,
    /**< Acquire CDMA or HDR systems only. */

  CM_MODE_PREF_CDMA_AMPS_HDR_ONLY=23,
    /**< Acquire CDMA or AMPS or HDR systems only. */

  CM_MODE_PREF_CURRENT_LESS_AMPS=24,
    /**< Remove the AMPS component from the current mode preference. Will be
         mapped to something else by the CM; ph_ptr->mode_pref is never set to
         this value.\n
         @note1 Clients must check CM_API_WLAN for WLAN support for following
                preferences. */

  CM_MODE_PREF_WLAN_ONLY=25,
    /**< Acquire WLAN systems only. */

  CM_MODE_PREF_CDMA_WLAN=26,
    /**< Acquire CDMA and WLAN systems only. */

  CM_MODE_PREF_HDR_WLAN=27,
    /**< Acquire HDR and WLAN systems only. */

  CM_MODE_PREF_CDMA_HDR_WLAN=28,
    /**< Acquire CDMA, HDR, and WLAN systems only. */

  CM_MODE_PREF_GSM_WLAN=29,
    /**< Acquire GSM and WLAN systems only. */

  CM_MODE_PREF_WCDMA_WLAN=30,
    /**< Acquire WCDMA and WLAN systems only. */

  CM_MODE_PREF_GW_WLAN=31,
    /**< Acquire GSM/WCDMA and WLAN systems only. */

  CM_MODE_PREF_CURRENT_PLUS_WLAN=32,
    /**< Acquire WLAN systems in addition to the current system. */

  CM_MODE_PREF_CURRENT_LESS_WLAN=33,
    /**< Remove WLAN systems from the current system. */

  CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY = 34,
    /**< Acquire CDMA, AMPS, HDR, and WLAN systems. */

  CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY = 35,
    /**< Acquire CDMA, AMPS, and WLAN systems. */

  CM_MODE_PREF_INTERSECT_OR_FORCE = 36,
    /**< Acquire the mode that is common (intersection) to the current 
         mode and the requested mode. If the intersection is NULL, force the
         requested preferences. */

  CM_MODE_PREF_ANY_BUT_HDR_WLAN = 37,
    /**< Use any service except HDR and WLAN. */

  CM_MODE_PREF_LTE_ONLY = 38,
    /**<  Service is limited to LTE only. */

  CM_MODE_PREF_GWL = 39,
    /**< Service is limited to GSM, WCDMA or LTE. */

  CM_MODE_PREF_HDR_LTE_ONLY = 40,
    /**< Service is limited to HDR or LTE. */

  CM_MODE_PREF_CDMA_HDR_LTE_ONLY = 41,
    /**< Service is limited to CDMA, HDR or LTE. */

  CM_MODE_PREF_CDMA_HDR_GW = 42,
    /**< Service is limited to CDMA, HDR, GSM or WCDMA. */

  CM_MODE_PREF_CDMA_GW = 43,
    /**< Service is limited to CDMA, GSM or WCDMA. */

  CM_MODE_PREF_ANY_BUT_WLAN = 44,
    /**< Use any service except WLAN. */

  CM_MODE_PREF_GWL_WLAN = 45,
    /**< Service is limited to WLAN, GSM or WCDMA, and LTE. */

  CM_MODE_PREF_CDMA_LTE_ONLY = 46,
    /**< Service is limited to CDMA and LTE. */

  CM_MODE_PREF_ANY_BUT_HDR_LTE = 47,
    /**< Use any service except HDR and LTE */

  CM_MODE_PREF_ANY_BUT_LTE = 48,
    /**< Use any service except LTE */

  CM_MODE_PREF_DIGITAL_LESS_LTE_ONLY=49,
    /**< Acquire digital, non-LTE mode (CDMA, HDR, GSM, WCDMA, WLAN). */

  CM_MODE_PREF_DIGITAL_LESS_HDR_LTE_ONLY = 50,
    /**< Acquire digital, non-LTE mode (CDMA, GSM, WCDMA & WLAN). */

  CM_MODE_PREF_GSM_LTE = 51,
    /**< Acquire GSM and LTE	systems only. */

  CM_MODE_PREF_CDMA_GSM_LTE = 52,
    /**< Acquire CDMA, GSM and LTE  systems only. */
  
  CM_MODE_PREF_HDR_GSM_LTE = 53, 
    /**< Acquire HDR, GSM and LTE  systems only. */
  
  CM_MODE_PREF_WCDMA_LTE = 54, 
    /**< Acquire WCDMA and LTE  systems only. */
  
  CM_MODE_PREF_CDMA_WCDMA_LTE = 55,
    /**< Acquire CDMA, WCDMA and LTE  systems only. */
  
  CM_MODE_PREF_HDR_WCDMA_LTE = 56,
    /**< Acquire HDR, WCDMA and LTE  systems only. */

  CM_MODE_PREF_CDMA_HDR_GSM = 57,
    /**< Service is limited to CDMA, HDR and GSM. */

  CM_MODE_PREF_CDMA_GSM = 58,
    /**< Service is limited to CDMA and GSM. */

    /** @cond
  */
  CM_MODE_PREF_MAX   /* FOR INTERNAL CM USE ONLY! FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */


} cm_mode_pref_e_type;
/*~ SENTINEL cm_mode_pref_e_type.CM_MODE_PREF_MAX */



/** Enumeration of PLMN blocking preferences.
*/
typedef enum cm_plmn_blocking_pref_e {

    /** @cond
    */
  CM_PLMN_BLOCKING_PREF_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
    /** @endcond
    */

  CM_PLMN_BLOCKING_PREF_LTE_DETACH_ONLY=0,
    /**< LTE detach only */

  CM_PLMN_BLOCKING_PREF_PLMN_BLOCKING_ONLY = 1,
    /**< PLMN blocking only */

  CM_PLMN_BLOCKING_PREF_LTE_DETACH_PLMN_BLOCKING = 2,
    /**< LTE detach and PLMN blocking */

  CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY = 3,
    /**< Unblock a specified PLMN */

  CM_PLMN_BLOCKING_PREF_PLMN_RESET = 4,
    /**< Reset the PLMN forbidden list */

    /** @cond
    */
  CM_PLMN_BLOCKING_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
    /** @endcond
    */

} cm_plmn_blocking_pref_e_type;
/*~ SENTINEL cm_plmn_blocking_pref_e_type.CM_PLMN_BLOCKING_PREF_MAX */



/* For compatibility with the QXDM database, please do not change the following
   fixed assigned values. If new values are added, please also update the QXDM
   database.
*/

/** Enumeration of the GSM/WCDMA acquisition order.
*/
typedef enum cm_gw_acq_order_pref_e {

/** @cond
*/
  CM_GW_ACQ_ORDER_PREF_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_GW_ACQ_ORDER_PREF_AUTOMATIC=0,
    /**< Determine mode automatically from the PRL order. */

  CM_GW_ACQ_ORDER_PREF_GSM_WCDMA=1,
    /**< Acquisition order is GSM followed by WCDMA. */

  CM_GW_ACQ_ORDER_PREF_WCDMA_GSM=2,
    /**< Acquisition order is  WCDMA followed by GSM. */

  CM_GW_ACQ_ORDER_PREF_NO_CHANGE=3,
    /**< Do not change GSM/WCDMA acquisition order. */

  CM_GW_ACQ_ORDER_PREF_PERSISTENT=4,
    /**< Return to acquisition order preference provided by the NV. Clients
         must check for CM_API_PERSISTENT_PREF before using this enum. */

/** @cond
*/
  CM_GW_ACQ_ORDER_PREF_MAX  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_gw_acq_order_pref_e_type;
/*~ SENTINEL cm_gw_acq_order_pref_e_type.CM_GW_ACQ_ORDER_PREF_MAX */


/** Enumeration of mode terms (i.e., designate the mode span).
*/
typedef enum cm_pref_term_e {

/** @cond
*/
  CM_PREF_TERM_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_PREF_TERM_PERMANENT,
    /**< Permanent mode change. */

  CM_PREF_TERM_PWR_CYCLE,
    /**< Until a power cycle. */

  CM_PREF_TERM_1_CALL,
    /**< Until the end of the next call or power cycle. */

  CM_PREF_TERM_1_CALL_OR_TIME,
    /**< Until the end of the next call, specified timeout, or power cycle. */

  /* 4 */
  CM_PREF_TERM_CM_1_CALL,      /**< &nbsp; */
  
  CM_PREF_TERM_CM_1_CALL_PLUS, /**< &nbsp; */
  
  CM_PREF_TERM_CM_MO_SMS,
    /**< Mode terms that are assigned automatically by the CM when the mode is 
         forced on the fly during a call origination or mobile-originated SMS.

         The CM unforces the mode back to its original settings at the end of
         a call or some short time after the end of call/message transmission.

         @note1 These mode terms should NOT be used by clients when changing 
         the mode preference. */

  /** @cond
  */
  CM_PREF_TERM_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_pref_term_e_type;
/*~ SENTINEL cm_pref_term_e_type.CM_PREF_TERM_MAX */

/** @} */ /* end_addtogroup PHONE */

/** @addtogroup PHONE
    @{ */

/** 64-bit cm_band_pref is to be used if CM_API_BAND_PREF_64_BIT is defined.
*/
typedef uint64 cm_band_pref_e_type;

/** @} */ /* end_addtogroup PHONE */

/** @addtogroup PHONE
    @{ */

/** When CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE is defined, revise the
    CM_BAND_PREF_NO_CHANGE back to the value where band_pref is still 32 bits
    for backward compatibility. Bit 30 is reserved for NO_CHANGE.
*/
#define  CM_BAND_PREF_NO_CHANGE   (cm_band_pref_e_type)(0x40000000)
    /* Do not change the band preference. */

/** @} */ /* end_addtogroup PHONE */

/** @addtogroup PHONE
    @{ */

/* For compatibility with the QXDM database, do not change the following
** fixed assigned values. If new values are added, also update the QXDM
** database.
*/

/** Enumeration of system selection roam preferences.
*/
typedef enum cm_roam_pref_e {

/** @cond
*/
  CM_ROAM_PREF_NONE=0,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_ROAM_PREF_HOME=SYS_BM(SYS_ROAM_STATUS_OFF),
    /**< Acquire only systems for which the roaming indication is off
         (SD_SS_ROAM_PREF_HOME). */

  CM_ROAM_PREF_ROAM_ONLY=SYS_BM(SYS_ROAM_STATUS_ON),
    /**<Acquire a system as long as its roaming indication is not off. */

  CM_ROAM_PREF_AFFIL=SYS_BM(SYS_ROAM_STATUS_OFF)|SYS_BM(SYS_ROAM_STATUS_ON),
    /**< Acquire only systems for which the roaming indication is off or
         solid on (i.e., not flashing). CDMA only (SD_SS_ROAM_PREF_AFFIL). */

  CM_ROAM_PREF_ANY=0xFF,
    /**< Acquire systems regardless of their roaming indication
         (SD_SS_ROAM_PREF_ANY). */

  CM_ROAM_PREF_NO_CHANGE,
    /**< Do not change the roaming preference. */

  CM_ROAM_PREF_PERSISTENT,
    /**< Return to the roaming preference provided by the NV. Clients must
         check for CM_API_PERSISTENT_PREF before using this enum. */

/** @cond
*/
  CM_ROAM_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_roam_pref_e_type;
/*~ SENTINEL cm_roam_pref_e_type.CM_ROAM_PREF_MAX */


/* For compatibility with the QXDM database, do not change the following
** fixed assigned values. If new values are added, also update the QXDM
** database.
*/

/** Enumeration of Preferred Roaming List (PRL) preferences.
*/
typedef enum cm_prl_pref_e {

/** @cond
*/
  CM_PRL_PREF_NONE=0,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_PRL_PREF_AVAIL_BC0_A=SYS_BM(SYS_PRL_AVAIL_BC0_A),
    /**< Acquire the available system only on the A side. */

  CM_PRL_PREF_AVAIL_BC0_B=SYS_BM(SYS_PRL_AVAIL_BC0_B),
    /**< Acquire the available system only on the B side. */

  CM_PRL_PREF_ANY=0x3FFF,
    /**< Acquire any available systems. */

  CM_PRL_PREF_NO_CHANGE,
    /**< Do not change the PRL preference. */

  CM_PRL_PREF_PERSISTENT,
    /**< Return to the persistent preference setting. */

/** @cond
*/
  CM_PRL_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_prl_pref_e_type;
/*~ SENTINEL cm_prl_pref_e_type.CM_PRL_PREF_MAX */


/* For compatibility with the QXDM database, do not change the following
** fixed assigned values. If new values are added, also update the QXDM
** database.
*/

/** Enumeration of system selection WLAN technology preferences.
*/
typedef enum cm_wlan_tech_pref_e {

/** @cond
*/
  CM_WLAN_TECH_PREF_NONE=0,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_WLAN_TECH_PREF_A       =          SYS_BM(SYS_TECH_WLAN_80211A),
    /**< Acquire 802.11a only systems. */

  CM_WLAN_TECH_PREF_B       =          SYS_BM(SYS_TECH_WLAN_80211B),
    /**< Acquire 802.11b only systems. */

  CM_WLAN_TECH_PREF_G_ONLY  =          SYS_BM(SYS_TECH_WLAN_80211G),
    /**< Acquire 802.11g only systems. */

  CM_WLAN_TECH_PREF_G       =          SYS_BM(SYS_TECH_WLAN_80211B) | \
                                       SYS_BM(SYS_TECH_WLAN_80211G),
    /**< Acquire any 802.11g systems. */

  CM_WLAN_TECH_PREF_ANY     =          0x3FFFFFFF,
    /**< Acquire any 802.11 systems. */

  CM_WLAN_TECH_PREF_NO_CHANGE,
    /**< Do not change the WLAN technology preference. */

/** @cond
*/
  CM_WLAN_TECH_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_wlan_tech_pref_e_type;
/*~ SENTINEL cm_wlan_tech_pref_e_type.CM_WLAN_TECH_PREF_MAX */

/* For compatibility with the QXDM database, do not change the following
** fixed assigned values. If new values are added, also update the QXDM
** database
*/

/** Enumeration of system selection WLAN Basic Service Set (BSS)
    preferences.
*/
typedef enum cm_wlan_bss_type_pref_e {

/** @cond
*/
  CM_WLAN_BSS_TYPE_PREF_NONE=0,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_WLAN_BSS_TYPE_PREF_ADHOC,
    /**< Acquire adhoc systems only. */

  CM_WLAN_BSS_TYPE_PREF_INFRA,
    /**< Acquire Infra systems only. */

  CM_WLAN_BSS_TYPE_PREF_ANY,
    /**< Acquire any systems. */

  CM_WLAN_BSS_TYPE_PREF_NO_CHANGE,
    /**< Do not change the WLAN BSS type preference. */

/** @cond
*/
  CM_WLAN_BSS_TYPE_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_wlan_bss_type_pref_e_type;
/*~ SENTINEL cm_wlan_bss_type_pref_e_type.CM_WLAN_BSS_TYPE_PREF_MAX */


/* For compatibility with the QXDM database, do not change the following
** fixed assigned values. If new values are added, also update the QXDM
** database
*/

/** Enumeration of system selection hybrid preferences (FEATURE_HDR).
*/
typedef enum cm_hybr_pref_e {

/** @cond
*/
  CM_HYBR_PREF_NONE        = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_HYBR_PREF_OFF=0,
    /**< Hybrid operation is not allowed (SD_SS_HYBR_PREF_NONE). */

  CM_HYBR_PREF_ON=1,
    /**< Hybrid operation is allowed (SD_SS_HYBR_PREF_CDMA_HDR). */

  CM_HYBR_PREF_CDMA__HDR = CM_HYBR_PREF_ON,
    /**< CDMA + HDR hybrid operation. */

  CM_HYBR_PREF_NO_CHANGE=2,
    /**< Do not change the hybrid preference. */

  CM_HYBR_PREF_PERSISTENT=3,
    /**< Return to the value provided in the NV. Clients must check for
         CM_API_PERSISTENT_PREF before using this enum. */
    
        /* Diagnostics clients depend on CM_HYBR_PREF_PERSISTENT
           to hold same the value across targets. */

  CM_HYBR_PREF_CDMA__HDR_WCDMA,
    /**< CDMA and (HDR or WCDMA), whichever is available .*/

  CM_HYBR_PREF_CDMA__WCDMA,
    /**< CDMA and WCDMA hybrid operation. */

  CM_HYBR_PREF_CDMA__LTE__HDR,
    /**< CDMA and (LTE or HDR) hybrid operation. */

  CM_HYBR_PREF_CDMA__GWL__HDR,
    /**< CDMA and (GWL or HDR) hybrid operation. */

  /** @cond
  */
  CM_HYBR_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_hybr_pref_e_type;
/*~ SENTINEL cm_hybr_pref_e_type.CM_HYBR_PREF_MAX */

/** Enumeration of DDTM preferences (FEATURE_DDTM_CNTL).
*/
typedef enum cm_ddtm_pref_e {

/** @cond
*/
  CM_DDTM_PREF_NONE        = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_DDTM_PREF_OFF,
    /**< Disable Data Dedicated Transmission Mode. */

  CM_DDTM_PREF_ON,
    /**< Enable Data Dedicated Transmission Mode. */

  CM_DDTM_PREF_NO_CHANGE,
    /**< Do not change the DDTM preference. */

/** @cond
*/
  CM_DDTM_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ddtm_pref_e_type;
/*~ SENTINEL cm_ddtm_pref_e_type.CM_DDTM_PREF_MAX */

/** Enumeration of action types to add, replace, or delete the user-specified
    Service Options (SOs) in the SO list maintained by the CM for DDTM.
*/
typedef enum cm_ddtm_pref_act_e {

/** @cond
*/
  CM_DDTM_SO_LIST_ACT_NONE        = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_DDTM_SO_LIST_ACT_ADD,
    /**< Add the specified SOs to the current DDTM SO list maintained by CM. */

  CM_DDTM_SO_LIST_ACT_REPLACE,
     /**< Replace the current DDTM SO list maintained by CM with the one
          specified. */

  CM_DDTM_SO_LIST_ACT_DELETE,
     /**< Delete the user-specified SOs from the DDTM SO list maintained
          by the CM. */

  CM_DDTM_SO_LIST_ACT_NO_CHANGE,
     /**< No change in the DDTM SO list. */

/** @cond
*/
  CM_DDTM_SO_LIST_ACT_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ddtm_so_list_act_e_type;
/*~ SENTINEL cm_ddtm_so_list_act_e_type.CM_DDTM_SO_LIST_ACT_MAX */

/** Enumeration to indicate the type of system to be avoided.
*/
typedef enum {

/** @cond
*/
  CM_SS_AVOID_SYS_NONE        = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_SS_AVOID_SYS_SRV_IF_UZ,
    /**< Avoid idle system only if the Mobile System (MS) has a user zone 
         currently selected. */

  CM_SS_AVOID_SYS_SRV,
    /**< Avoid idle systems. */

/** @cond
*/
  CM_SS_AVOID_SYS_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ss_avoid_sys_e_type;
/*~ SENTINEL cm_ss_avoid_sys_e_type.CM_SS_AVOID_SYS_MAX */

/** Enumeration of Answer Voice as Data settings (CDMA only).
*/
typedef enum cm_answer_voice_e {

/** @cond
*/
  CM_ANSWER_VOICE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_ANSWER_VOICE_AS_VOICE,
    /**< Answer voice as voice (DB_VOICE_AS_DATA_NEVER). */

  CM_ANSWER_VOICE_AS_FAX_ONCE,
    /**< Answer voice as fax once (within the next 10 minutes)
        (DB_VOICE_AS_FAX_ONCE). */

  CM_ANSWER_VOICE_AS_FAX_ALWAYS,
     /**< Answer voice as fax always (DB_VOICE_AS_FAX_ALWAYS). */

  CM_ANSWER_VOICE_AS_MODEM_ONCE,
    /**< Answer voice as modem once (within the next 10 minutes)
         (DB_VOICE_AS_MODEM_ONCE). */

  CM_ANSWER_VOICE_AS_MODEM_ALWAYS,
    /**< Answer voice as modem always (DB_VOICE_AS_MODEM_ALWAYS). */

/** @cond
*/
  CM_ANSWER_VOICE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_answer_voice_e_type;
/*~ SENTINEL cm_answer_voice_e_type.CM_ANSWER_VOICE_MAX */

/** Enumeration of NAM selections.
*/
typedef enum cm_nam_e {

/** @cond
*/
  CM_NAM_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_NAM_1,
    /**< NAM 1.*/

  CM_NAM_2,
    /**< NAM 2; for targets with more than 1 NAM. */

  CM_NAM_3,
    /**< NAM 3; for targets with more than 2 NAMs. */

  CM_NAM_4,
    /**< NAM 4; for targets with more than 3 NAMs. */

  CM_NAM_AUTO,
    /**< Auto NAM; for targets with more than 1 NAM. */

/** @cond
*/
  CM_NAM_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_nam_e_type;
/*~ SENTINEL cm_nam_e_type.CM_NAM_MAX */

/** Enumeration of phone CDMA lock modes.
*/
typedef enum cm_cdma_lock_mode_e {

/** @cond
*/
  CM_CDMA_LOCK_MODE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_CDMA_LOCK_MODE_OFF,
    /**< Phone is NOT CDMA locked. */

  CM_CDMA_LOCK_MODE_ON,
    /**< Phone is CDMA locked until power cycle. */

/** @cond
*/
  CM_CDMA_LOCK_MODE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_cdma_lock_mode_e_type;
/*~ SENTINEL cm_cdma_lock_mode_e_type.CM_CDMA_LOCK_MODE_MAX */

/** Enumeration of phone service states.
*/
typedef enum cm_packet_state_e {

/** @cond
*/
  CM_PACKET_STATE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_PACKET_STATE_NULL,
    /**< Packet state is NULL. */

  CM_PACKET_STATE_CONNECT,
    /**< Packet state is connect. */

  CM_PACKET_STATE_DORMANT,
    /**< Packet state is dormant. */

/** @cond
*/
  CM_PACKET_STATE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_packet_state_e_type;
/*~ SENTINEL cm_packet_state_e_type.CM_PACKET_STATE_MAX */

/* For compatibility with the QXDM database, do not change the following
   fixed assigned values. If new values are added, also update the QXDM
   database.
*/
/** Enumeration that defines the service domain being requested by
    the client (the preferred service domain).
*/
typedef enum cm_srv_domain_pref_e {

/** @cond
*/
  CM_SRV_DOMAIN_PREF_NONE=-1, /* FOR INTERNAL CM USE ONLY!
  No service domain is requested. */
/** @endcond
*/

  CM_SRV_DOMAIN_PREF_CS_ONLY=0,
    /**< Prefer Circuit-Switched only. */

  CM_SRV_DOMAIN_PREF_PS_ONLY=1,
    /**< Prefer Packet-Switched only. */

  CM_SRV_DOMAIN_PREF_CS_PS=2,
    /**< Prefer Circuit and Packet Switched. */

  CM_SRV_DOMAIN_PREF_ANY=3,
    /**< Any domain will do; no preference. */

  CM_SRV_DOMAIN_PREF_NO_CHANGE=4,
    /**< To be used by clients who do not want to change the service domain. */

  CM_SRV_DOMAIN_PREF_PS_ATTACH=5,
    /**<  PS attach on demand. This value of service domain preference is 
          not saved in the NV. */

  CM_SRV_DOMAIN_PREF_PS_DETACH=6,
    /**< PS detach on demand. This value of service domain preference is not 
         saved in the NV. */

  CM_SRV_DOMAIN_PREF_PERSISTENT=7,
    /**< Return to value stored in the NV. Clients must check for
         CM_API_PERSISTENT_PREF before using this enum. */

/** @cond
*/
  CM_SRV_DOMAIN_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_srv_domain_pref_e_type;
/*~ SENTINEL cm_srv_domain_pref_e_type.CM_SRV_DOMAIN_PREF_MAX */

/* For compatibility with the QXDM database, do not change the following
   fixed assigned values. If new values are added, also update the QXDM
   database
*/

/** Enumeration that defines whether the network should be selected
    automatically, manually, or for limited service (GSM/WCDMA or WLAN only).
*/
typedef enum cm_network_sel_mode_pref_e {

/** @cond
*/
  CM_NETWORK_SEL_MODE_PREF_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_NETWORK_SEL_MODE_PREF_AUTOMATIC=0,
    /**< Automatic network selection. */

  CM_NETWORK_SEL_MODE_PREF_MANUAL=1,
    /**< Manual network selection. */

  CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV=2,
    /**< Select a network for limited service. Not applicable for WLAN. */

  CM_NETWORK_SEL_MODE_PREF_NO_CHANGE=3,
    /**< No change. */

  CM_NETWORK_SEL_MODE_PREF_PERSISTENT=4,
   /**< Return to persistent preference. Not applicable for WLAN. */

  CM_NETWORK_SEL_MODE_PREF_HPLMN_SRCH=5,
   /**< Search and camp on Home Public Land Mobile Network (HPLMN). When
        requested, cm_ph_cmd_sys_sel_pref(), pref_duration, and plmn_ptr are
        ignored. term_pref = CM_PREF_TERM_PWR_CYCLE. Other parameters should
        carry NO_CHANGE. Clients must check for define
        CM_API_INITIATE_HPLMN_SRCH before using this enum. */

/** @cond
*/
  CM_NETWORK_SEL_MODE_PREF_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_network_sel_mode_pref_e_type;
/*~ SENTINEL cm_network_sel_mode_pref_e_type.CM_NETWORK_SEL_MODE_PREF_MAX */


/**
** The following enumerated type defines the UE 
** of mode of operation when operating in LTE network.
*/

typedef enum
{

/** @cond
*/
  CM_LTE_UE_MODE_PREF_NONE               = -1, /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_LTE_UE_MODE_PREF_PS_MODE1           = 0, 
   /**< UE registers only to EPS services, 
       and UE's usage setting is voice centric. */

  CM_LTE_UE_MODE_PREF_PS_MODE2           = 1,
   /**< UE registers only to EPS services, 
        and UE's usage setting is data centric. */

  CM_LTE_UE_MODE_PREF_CS_PS_MODE1        = 2,
   /**< UE registers to both EPS and non-EPS services, 
        and UE's usage setting is voice centric. */
  
  CM_LTE_UE_MODE_PREF_CS_PS_MODE2        = 3, 
   /**< CS/PS mode 2 of operation: the UE registers to both EPS
        and non-EPS services, and UE's usage setting is data centric */

  CM_LTE_UE_MODE_PREF_NO_CHANGE          = 4,
    /**< No change. */

/** @cond
*/
  CM_LTE_UE_MODE_PREF_MAX     /* FOR INTERNAL USE ONLY */
/** @endcond
*/

}cm_lte_ue_mode_pref_e_type;
/*~ SENTINEL cm_lte_ue_mode_pref_e_type.CM_LTE_UE_MODE_MAX */


/* For compatibility with QXDM database, please do not change the following
   fix assigned values. If new values are added please also update the QXDM
   database
*/

/** Defines whether the network should be selected
    automatically, manually, or by default. GSM/WCDMA only.
*/
typedef enum cm_user_net_sel_mode_pref_e {

  /** @cond
  */
  CM_USER_NETWORK_SEL_MODE_PREF_NONE   = -1,   /* internal */
  /** @endcond */

  CM_USER_NETWORK_SEL_MODE_PREF_DEFAULT = 0,
    /**< Default network selection. No change. */

  CM_USER_NETWORK_SEL_MODE_PREF_AUTOMATIC = 1,
    /**< Automatic network selection. */    

  CM_USER_NETWORK_SEL_MODE_PREF_MANUAL = 2,
    /**< Manual network selection. */

  /**@cond
  */
  CM_USER_NETWORK_SEL_MODE_PREF_MAX  /* internal */
  /**@endcond */


} cm_user_net_sel_mode_pref_e_type;
/*~ SENTINEL cm_user_net_sel_mode_pref_e_type.CM_USER_NETWORK_SEL_MODE_PREF_MAX */

/** Enumeration that defines the type of network list to be requested. The
    requested list will either contain the preferred networks or all available
    networks.
*/
typedef enum cm_network_list_type_e {

/** @cond
*/
  CM_NETWORK_LIST_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_NETWORK_LIST_AVAILABLE,
    /**< The available network list. */

  CM_NETWORK_LIST_USER_PREFERRED,
    /**< The preferred network list from SIM/USIM. This request returns only
         the list of PLMNs that are valid. The list does not contain any holes,
         even if there are holes in the SIM/USIM file. */

  CM_NETWORK_LIST_USER_PREFERRED_UNPACKED,
    /**< The preferred network list from SIM/USIM. This returns the list the
         way it is stored in the SIM/USIM. If there are holes between two
         records, they will show in the returned list. */

/** @cond
*/
  CM_NETWORK_LIST_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_network_list_type_e_type;
/*~ SENTINEL cm_network_list_type_e_type.CM_NETWORK_LIST_MAX */

/** Enumeration of the subscription status.
*/
typedef enum cm_subscription_status_e {

/** @cond
*/
  CM_SUBSCRIPTION_STATUS_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_SUBSCRIPTION_STATUS_CHANGE,
    /**< Subscription for the specified mode changed. */

  CM_SUBSCRIPTION_STATUS_NO_CHANGE,
    /**< Subscription for the specified mode has not changed. */

/** @cond
*/
  CM_SUBSCRIPTION_STATUS_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_subscription_status_e_type;
/*~ SENTINEL cm_subscription_status_e_type.CM_SUBSCRIPTION_STATUS_MAX */


/** Enumeration of the Runtime RUIM Enable (RTRE) configuration.
*/
typedef enum cm_rtre_config_e {

/** @cond
*/
  CM_RTRE_CONFIG_NONE,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_RTRE_CONFIG_RUIM_ONLY,
    /**< RTRE configuration; use the RUIM only. */

  CM_RTRE_CONFIG_NV_ONLY,
    /**< RTRE configuration; use the NV only. */

  CM_RTRE_CONFIG_RUIM_OR_DROP_BACK,
    /**< RTRE; use the RUIM if available, otherwise use the NV. */

  CM_RTRE_CONFIG_SIM_ACCESS,
    /**< RTRE; use SIM. */

/** @cond
*/
  CM_RTRE_CONFIG__MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_rtre_config_e_type;
/*~ SENTINEL cm_rtre_config_e_type.CM_RTRE_CONFIG__MAX */

/** Enumeration of the RTRE control type.
*/
typedef enum cm_rtre_control_e {

/** @cond
*/
  CM_RTRE_CONTROL_NONE,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_RTRE_CONTROL_RUIM,
    /**< RTRE control; use the RUIM. */

  CM_RTRE_CONTROL_NV,
    /**< RTRE control; use the the NV. */

  CM_RTRE_CONTROL_SIM
    /**< RTRE control; use the SIM. */

} cm_rtre_control_e_type;

/** Enumeration of the test control type.
*/
typedef enum cm_test_control_type_e {

/** @cond
*/
  CM_TEST_CONTROL_TYPE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_TEST_CONTROL_TYPE_NO_TEST,
    /**< Initial value when no testing is going on. */

  CM_TEST_CONTROL_TYPE_LB_MODE1,
    /**< Loopback mode 1. */

  CM_TEST_CONTROL_TYPE_LB_MODE2,
    /**< Loopback mode 2. */

  CM_TEST_CONTROL_TYPE_LB_MODE1_WITH_DUMMY_SIGNALLING,
    /**< Loopback mode 1 with dummy signaling. */

  CM_TEST_CONTROL_TYPE_LB_MODE2_WITH_DUMMY_SIGNALLING,
    /**< Loopback mode 2 with dummy signaling. */

/** @cond
*/
  CM_TEST_CONTROL_TYPE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_test_control_type_e_type;
/*~ SENTINEL cm_test_control_type_e_type.CM_TEST_CONTROL_TYPE_MAX */

/** @} */ /* end_addtogroup PHONE */

/** @addtogroup PHONE
    @{ */

/** @brief Gives clients Advice of Charge (AOC) information. Clients
    must check for CM_API_AOC_INFO before using this field.
*/
typedef struct cm_ph_aoc_info_s {

  boolean  aoc_ready;
    /**< Tells if the AOC subsystem is ready. */

  unsigned long ACM;
    /**< Accumulated Call Meter. aoc_ready must be TRUE for this to be valid. */

  unsigned long ACMmax;
    /**< Maximum allowed ACM value. aoc_ready must be TRUE for this to be
         valid. */

} cm_ph_aoc_info_s_type;

/** @} */ /* end_addtogroup PHONE */

/** @addtogroup PHONE
    @{ */

/** Enumeration that defines the type of wakeup action required. Clients must
    check CM_API_WLAN for WLAN support before using this enum.
*/
typedef enum cm_wakeup_e {

/** @cond
*/
  CM_WAKEUP_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_WAKEUP_MAIN   = SYS_BM(0),
    /**< Wake up main service. It could be CDMA, HDR, GSM, WCDMA, or GW,
         depending on the mode_pref and hybr_pref. */

  CM_WAKEUP_HDR    = SYS_BM(1),
    /**< Wake up HDR service. This is valid only if hybr_pref is on. */

  CM_WAKEUP_WLAN   = SYS_BM(2),
    /**< Wake up WLAN service. */

  CM_WAKEUP_HYBR_GW   = SYS_BM(3),
    /**< Wake up HYBR_GW service. */

  CM_WAKEUP_ALL    =  CM_WAKEUP_MAIN | \
                      CM_WAKEUP_HDR  | \
                      CM_WAKEUP_WLAN | \
                      CM_WAKEUP_HYBR_GW,
    /**< Wake up all services. */

/** @cond
*/
  CM_WAKEUP_MAX  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_wakeup_e_type;
/*~ SENTINEL cm_wakeup_e_type.CM_WAKEUP_MAX */

/** Enumeration that defines the WLAN scan preference.
*/
typedef enum cm_wlan_scan_pref_e {

/** @cond
*/
  CM_WLAN_SCAN_PREF_NONE =-1,  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_WLAN_SCAN_PREF_ACTIVE   = SYS_WLAN_SCAN_PREF_ACTIVE,
    /**< Active WLAN scan. */

  CM_WLAN_SCAN_PREF_PASSIVE  = SYS_WLAN_SCAN_PREF_PASSIVE,
    /**< Passive WLAN scan. */

  CM_WLAN_SCAN_PREF_AUTO     = SYS_WLAN_SCAN_PREF_AUTO,
    /**< Let the CM decide active or passive scan automatically. */

  CM_WLAN_SCAN_PREF_NO_CHANGE,
    /**< Let the scan remain as is. */

/** @cond
*/
  CM_WLAN_SCAN_PREF_MAX   /* internal */
/** @endcond
*/

} cm_wlan_scan_pref_e_type;
/*~ SENTINEL cm_wlan_scan_pref_e_type.CM_WLAN_SCAN_PREF_MAX */

/** @} */ /* end_addtogroup PHONE */

/** @addtogroup PHONE
    @{ */

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** @brief Type to hold net lists phone information. When notified of phone events
    about network lists, clients are given a reference to such a structure.
*/
typedef struct cm_ph_net_lists_info_s {

  sys_user_pref_plmn_list_s_type   gw_user_pref_networks;
    /**< User preferred network list (GSM/WCDMA only). */

  sys_detailed_plmn_list_s_type    gw_available_networks;
    /**< Available networks (GSM/WCDMA only). */

  sys_wlan_bss_info_list_s_type    wlan_bss_list;
    /**< Available WLAN BSS list. */

} cm_ph_net_lists_info_s_type;

/** @brief Type to select WLAN preferences.
*/
typedef struct cm_wlan_pref_s {

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
    /**< WLAN selection is automatic or manual. */

  cm_wlan_scan_pref_e_type          scan_mode;
  /**< Active or Passive scan mode. */

  cm_wlan_tech_pref_e_type          tech_pref;
   /**< Specifies 802.11 a/b/g, etc. */

  cm_wlan_bss_type_pref_e_type      bss_type_pref;
   /**< Specifies Adhoc/Infra/Any for Automatic mode. */

  /* The following information is required for the Manual mode of WLAN
     selection.
  */

  sys_wlan_bss_e_type               bss_type;
   /**< BSS type: AD-HOC, ADHOC_START, or INFRA for Manual mode. */

  sys_wlan_bssid_type               bss_id;
    /**< If the WLAN network selection is set to manual, this specifies the
         BSS. If the BSS ID is unknown, set bss_id to 0. */

  sys_wlan_ssid_s_type              ssid;
    /**< If the WLAN network selection is set to manual, this specifies the 
         Service Set Identifier (SSID). If the SSID is unknown, set ssid_length
         to 0. */

  sys_chan_type                     chan;
   /**< The channel type is specified as a bitmask. See the definition in
        net.h for the bit pattern of the channel, as defined in
        net_chan_wlan_2400_e_type or net_chan_wlan_5000_e_type.

        If the WLAN network selection is set to manual, this specifies the
        channel. If the WLAN channel is unknown, set channel to 0. */

  sys_band_mask_type                band;
   /**< If the WLAN network selection is set to manual, this specifies the WLAN
        band. If the WLAN channel is unknown, set channel to
        SYS_BAND_MASK_WLAN_ANY. */

  sys_profile_id_type               profile_id;
   /**< Profile ID to be used with this manual system. */  

}cm_wlan_pref_s_type;

/** @This structure is used by client to indicate LTE detach and/or PLMN blocking. 
     If PLMN blocking is needed, PLMN ID and blocking interval should be filled in.
*/
typedef struct 
{
  cm_plmn_blocking_pref_e_type    request_pref;
    /**< Specify the request type - detach, PLMN blocking,
         or PLMN unblocking, PLMN forbidden list reset. */
  sys_plmn_id_s_type              plmn;
    /**< PLMN id. */
  uint32                          blocking_interval;
    /**< The blocking interval for PLMN blocking request. (ms) */
} cm_plmn_blocking_info_s_type ;

/** @brief Type to hold phone information. When notified of phone
    events, clients are given a reference to such a structure.
*/
typedef struct cm_ph_info_s {

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **            Phone State
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean                           is_in_use;
    /**< Indicates whether the phone is currently in use. */

  boolean                           is_in_thermal_emerg;
    /**< Indicates whether the phone is thermal emergency state. */

  sys_oprt_mode_e_type              oprt_mode;
    /**< Indicates the current operating mode. */

  cm_test_control_type_e_type       test_control_type;
    /**< Indicates the current test control type. */

  cm_cdma_lock_mode_e_type          cdma_lock_mode;
    /**< Indicates whether the phone is CDMA locked until a power cycle. */

  cm_answer_voice_e_type            answer_voice;
    /**< Indicates whether the phone should answer incoming voice calls as
         voice, fax, or modem (CDMA only). */

  sys_sys_mode_mask_e_type          mode_capability;
    /**< Indicates the mode capability. */

  sys_band_mask_e_type              band_capability;
    /**< Indicates the band capability. */

  sys_band_mask_e_type              lte_band_capability;
    /**< Indicates the LTE band capability. */
  
  sys_is2000_msg_s_type             cdma_capability;
    /**< CDMA capability in the form of IS2000 Status Message.
    */

  sys_msm_id_e_type                 msm_id;
    /**< Identifies the MSM which generated this event.
    */

  cm_nam_e_type                     nam_sel;
    /**< Indicates the NAM selection.\ CDMA only. */

  cm_nam_e_type                     curr_nam;
    /**< Indicates the current NAM - needed when NAM selection 
         is set to AUTO-NAM.\ CDMA only. */

  cm_packet_state_e_type            packet_state;
    /**< Indicates the current packet state.\ CDMA only */

  cm_cphs_proc_e_type               cphs_allowed;
    /**< Indicates if common PCN handset specification procedures are allowed.
         (FEATURE_ALS). */

  cm_als_proc_e_type                als_allowed;
    /**< Indicates if Alternate Line Service (ALS) procedures are allowed.
         (FEATURE_ALS). */

  cm_als_line_e_type                line;
    /**< Indicates the currently selected line (all MO voice calls will use it).
         (FEATURE_ALS). */

  cm_als_line_switching_e_type      line_switching;
    /**< Indicates if the user is allowed to switch between lines.
         (FEATURE_ALS). */

  cm_ccbs_store_info_s_type         ccbs_store_info;
    /**< Holds CCBS information for clients. Clients must check for
         CM_API_CCBS before using this event. */

  cm_ph_state_e_type                ph_state;
    /**< Indicates the current operating mode of the phone. Used to indicate
         that the phone is in emergency call back mode. */

  sys_meas_mode_e_type              meas_mode;
    /**< Indicates the measurement mode (enable/disable HDR access). */

  boolean                           prl_pref_only;
    /**< Indicates the PRL's prl_pref only setting. */

  word                              prl_id;
   /**< Indicates the PRL's ID. */

  cm_client_id_type                 requesting_client_id;
    /**< Requesting client */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **            Preferences per Subscription
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_modem_as_id_e_type            asubs_id;
    /**< Subscription ID to which these preferences should apply. */

  cm_mode_pref_e_type               mode_pref;
    /**< Indicates the current mode preference. */

  cm_pref_term_e_type               pref_term;
    /**< Indicates the current preference term (e.g., one-call, power-cycle).*/

  cm_prl_pref_e_type                prl_pref;
    /**< Indicates the current PRL preferred setting. */

  cm_gw_acq_order_pref_e_type       acq_order_pref;
    /**< The acquisition order preferred by the client (GSM/WCDMA only). */

  cm_srv_domain_pref_e_type         srv_domain_pref;
    /**< The service domain preferred by the client. This field is set after a
         call to cm_ph_cmd_sys_sel_pref (GSM/WCDMA only). */

  cm_band_pref_e_type               band_pref;
    /**< Indicates the current band preference. */

  cm_band_pref_e_type               lte_band_pref;
    /**< Indicates the current LTE band preference. */

  cm_roam_pref_e_type               roam_pref;
    /**< Indicates the current roam preference. */

  cm_hybr_pref_e_type               hybr_pref;
    /**< Indicates the hybrid preference. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
    /**< Defines whether the network should be selected automatically
         or manually. */ 

  sys_plmn_id_s_type                plmn;
    /**< If the network selection is set to manual, 
         this specifies the PLMN ID. */

  cm_ph_aoc_info_s_type             aoc_info;
    /**< Provides clients with Advice of Charge information.\ 
         Clients must check for CM_API_AOC_INFO before
         using this field. */

  sys_user_pref_plmn_list_s_type    user_pref_networks;
    /**< The user-preferred network list (GSM/WCDMA only). */

  sys_detailed_plmn_list_s_type     available_networks;
    /**< Available networks (GSM/WCDMA only). */

  boolean                           is_net_lists_present;
    /**< Flag indicating whether network lists information is available for
         clients. */

  sys_plmn_list_status_e_type       available_networks_list_cnf;
    /**< Provides the status of a PLMS list request (success/abort).\ 
         Clients must check for CM_API_AVAIL_NET_LIST_CNF
         before using this field. */

  cm_srv_domain_pref_e_type         orig_srv_domain_pref;
    /**< Service domain preference for PS attach/detach commands. 
         This is different from the phone service domain preference. */

  cm_user_net_sel_mode_pref_e_type  user_net_sel_mode;
    /**< User-selected network selection mode preference. */

  cm_lte_ue_mode_pref_e_type        lte_ue_mode_pref;
    /**< UE mode of operation pref when operating in LTE network */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **            WLAN Preferences
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_wlan_pref_s_type               wlan_pref;
    /**< WLAN preferences. */

  sys_wlan_bss_info_s_type          wlan_bss_info;
    /**< The identifier of the BSS to be selected manually (WLAN only). */

  sys_wlan_bss_info_list_s_type     wlan_bss_list;
    /**< Available WLAN BSS list. Clients must check CM_API_WLAN for WLAN
         support before using this field. */

  sys_wlan_wps_result_s_type        wlan_wps_result;
    /**< WPS procedure result. Clients must check for CM_API_WLAN_WPS before
         using this field. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **            Subscription Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean                           is_cdma_subscription_available;
    /**< Indicates whether the CDMA subscription/provisioning information
         is available for consumption. */

  boolean                           is_gw_subscription_available;
    /**< Indicates whether the GSM/WCDMA subscription/provisioning information
         is available for consumption. */

  boolean                          is_hybr_gw_subscription_available;
    /**< Indicates if the hybrid GSM/WCDMA subscription/provisioning information
         is available for consumption. */

  boolean                           is_wlan_subscription_available;
    /**< Indicates whether the WLAN subscription/provisioning information
         is available for consumption. Clients must check CM_API_WLAN for WLAN
         support before using this field. */

  cm_rtre_config_e_type             rtre_config;
    /**< Current RTRE configuration; RUIM, NV, or RUIM fallback NV (CDMA only). */

  cm_rtre_control_e_type            rtre_control;
    /**< Current RTRE control; RUIM or NV (CDMA only). */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **            DDTM Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_ddtm_pref_e_type               ddtm_pref;
    /**< Data Dedicated Tranmission Mode (DDTM) preference. */

  boolean                           cur_ddtm_status;
    /**< DDTM status. */

  sys_ddtm_act_mask_e_type          ddtm_act_mask;
    /**< DDTM action mask settings. */

  sys_ddtm_act_mask_e_type          orig_ddtm_act_mask;
    /**< DDTM action mask settings. */

  uint16                            ddtm_num_srv_opt;
    /**< Number of service options specified in ddtm_srv_opt_list. */

  sys_srv_opt_type             ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /**< List of service options whose pages have to be ignored, provided
         ddtm_pref and ddtm_act_mask allow it. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **            Dual Standby Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_modem_dual_standby_pref_e_type     standby_pref;
      /**< Standby preference of the phone. */

  sys_modem_as_id_e_type                 active_subs;
      /**< The active subscription in Single Standby mode.\ 
           Only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY. */

  sys_modem_as_id_e_type                 default_voice_subs;
      /**< Default voice subscription. */

  sys_modem_as_id_e_type                 default_data_subs;
      /**< Default data subscription. */

  sys_modem_as_id_e_type                 priority_subs;
      /**< Priority subscription. */

} cm_ph_info_s_type;
 
/*~ FIELD cm_ph_info_s.ddtm_srv_opt_list VARRAY SYS_DDTM_MAX_SO_LIST_SIZE
    LENGTH cm_ph_info_s.ddtm_num_srv_opt */


/** Type for phone command callback functions. A client that wants to be
    notified of the result of a phone command function must supply a pointer to
    a function of this type as the first parameter to a phone command function.
*/
typedef void (cm_ph_cmd_cb_f_type) (

  void                    *data_block_ptr,
    /**< Pointer to a client-provided data block. */

  cm_ph_cmd_e_type        ph_cmd,
    /**< Indicates which phone command a ph_cmd_err error status is for. */

  cm_ph_cmd_err_e_type    ph_cmd_err
    /**< Phone command error code indicating if the command
         is rejected and the reason for rejection. */
);

/** @} */ /* end_addtogroup PHONE */

/** @addtogroup PHONE
    @{ */

/*===========================================================================

FUNCTION: cm_ph_cmd_oprt_mode

===========================================================================*/
/**
  Commands the CM to put the phone in a specified operating
  mode, such as offline, online, Low Power Mode (LPM), etc.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.


@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] oprt_mode      Switch to this operating mode.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_oprt_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  sys_oprt_mode_e_type   oprt_mode

);
 
/*~ FUNCTION cm_ph_cmd_oprt_mode */

/*===========================================================================

FUNCTION: cm_ph_cmd_meas_mode

===========================================================================*/
/**
  Commands the CM to put the phone in or out of measurement mode. Currently
  valid only for High Data Rate (HDR).
  Measurement mode disables all HDR call processing and puts the modem into a
  special mode that allows monitoring of the HDR air interface without creating
  an HDR session or using the existing HDR session

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] meas_mode      Switch to this measurement mode.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event (CM_PH_EVENT_MEAS_MODE).
*/
extern boolean cm_ph_cmd_meas_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  sys_meas_mode_e_type   meas_mode

);
/*~ FUNCTION cm_ph_cmd_meas_mode */

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref

===========================================================================*/
/**
  Commands the CM to change the phone's system selection preference (i.e., the
  mode, band, and roaming preference).

  The preferred modes that can be used for this function are defined in
  cm_mode_pref_e_type(). The preference terms that can be used in this function
  are defined in cm_pref_term_e_type(). The values for the other preferences are
  defined in cm_hybr_pref_e_type() and cm_srv_domain_pref_e_type(). If a
  particular system selection preference is not to be changed, the appropriate
  NO_CHANGE enumerated value should be passed to this function.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Requesting client.
@param[in] mode_pref             Switch to this mode preference.
@param[in] pref_term             Switch the preference for this term.
@param[in] pref_duration         Specify the duration in seconds for a
                                 temporary term.
@param[in] acq_order_pref        GW acquisition order preference.
@param[in] band_pref             Switch to this band preference.
@param[in] prl_pref              Switch to this PRL preference.
@param[in] roam_pref             Switch to this roaming preference.
@param[in] hybr_pref             Switch to this hybrid preference.
@param[in] srv_domain_pref       Switch to this service domain (e.g., Circuit
                                 Switched). Set to CM_SRV_DOMAIN_PREF_NO_CHANGE
                                 if a service domain is not being changed.
@param[in] network_sel_mode_pref Defines whether the network should be selected
                                 automatically or manually.
@param[in] plmn_ptr              If network selection is set to manual, this
                                 specifies the PLMN ID.
@param[in] wlan_pref_ptr         WLAN preferences. Clients must check
                                 CM_API_WLAN for WLAN support.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_sys_sel_pref(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_mode_pref_e_type               mode_pref,
  cm_pref_term_e_type               pref_term,
  dword                             pref_duration,
  cm_gw_acq_order_pref_e_type       acq_order_pref,
  cm_band_pref_e_type               band_pref,
  cm_prl_pref_e_type                prl_pref,
  cm_roam_pref_e_type               roam_pref,
  cm_hybr_pref_e_type               hybr_pref,
  cm_srv_domain_pref_e_type         srv_domain_pref,
  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
  const sys_plmn_id_s_type          *plmn_ptr,
  const cm_wlan_pref_s_type         *wlan_pref_ptr

);
 
/*~ FUNCTION cm_ph_cmd_sys_sel_pref */
/*~ PARAM plmn_ptr POINTER */
/*~ PARAM wlan_pref_ptr POINTER */

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref_with_lte

===========================================================================*/
/**
  Commands the CM to change the phone's system selection preferences (i.e., the
  mode, band, LTE band, and roaming preferences). This function supports
  2G, 3G, and 4G.
  
  The preferred modes that can be used for this function are defined in 
  cm_mode_pref_e_type(). The preference terms that can be used in this function
  are defined in cm_pref_term_e_type(). The values for the other preferences are
  defined in cm_hybr_pref_e_type() and cm_srv_domain_pref_e_type(). If a
  particular system selection preference is not to be changed, the appropriate
  NO_CHANGE enumerated value should be passed to this function.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Requesting client.
@param[in] mode_pref             Switch to this mode preference.
@param[in] pref_term             Switch the preference for this term.
@param[in] pref_duration         Specify the duration in seconds for temporary
                                 term.
@param[in] acq_order_pref        GW acquisition order preference.
@param[in] band_pref             Switch to this band preference.
@param[in] lte_band_pref         Switch to this LTE band preference.
@param[in] prl_pref              Switch to this PRL preference.
@param[in] roam_pref             Switch to this roaming preference.
@param[in] hybr_pref             Switch to this hybrid preference.
@param[in] srv_domain_pref       Switch to this service domain (e.g., Circuit
                                 Switched). Set to CM_SRV_DOMAIN_PREF_NO_CHANGE
                                 if no service domain is being changed.
@param[in] network_sel_mode_pref Defines whether the network should be selected
                                 automatically or manually.
@param[in] plmn_ptr              If network selection is set to manual, this
                                 specifies the PLMN ID.
@param[in] wlan_pref_ptr         WLAN preferences. Clients must check
                                 CM_API_WLAN for WLAN support before using
                                 this field.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_sys_sel_pref_with_lte(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_mode_pref_e_type               mode_pref,
  cm_pref_term_e_type               pref_term,
  dword                             pref_duration,
  cm_gw_acq_order_pref_e_type       acq_order_pref,
  cm_band_pref_e_type               band_pref,
  cm_band_pref_e_type               lte_band_pref,
  cm_prl_pref_e_type                prl_pref,
  cm_roam_pref_e_type               roam_pref,
  cm_hybr_pref_e_type               hybr_pref,
  cm_srv_domain_pref_e_type         srv_domain_pref,
  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
  const sys_plmn_id_s_type          *plmn_ptr,
  const cm_wlan_pref_s_type         *wlan_pref_ptr

);
 
/*~ FUNCTION cm_ph_cmd_sys_sel_pref_with_lte */
/*~ PARAM plmn_ptr POINTER */
/*~ PARAM wlan_pref_ptr POINTER */

/*===========================================================================

FUNCTION: cm_ph_cmd_set_user_net_sel_mode_pref

===========================================================================*/
/** 
  Commands the CM to change the user_sel_net_sel_mode_pref of the phone.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func       Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Requesting client.
@param[in] ddtm_pref         Switch to this DDTM preference.
@param[in] user_net_sel_mode Network selection option
                             (automatic, manual, default, or user).

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_set_user_net_sel_mode_pref(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_user_net_sel_mode_pref_e_type  user_net_sel_mode

);
/*~ FUNCTION cm_ph_cmd_set_user_net_sel_mode_pref */

/*===========================================================================

FUNCTION: cm_ph_cmd_ddtm_pref

===========================================================================*/
/**
  Commands the CM to change the phone's Data Dedicated Transmission Mode (DDTM)
  preference to a specified selection, such as ON or OFF.
  On successful execution of this command, an event is returned to the client
  indicating the current DDTM preference. Possible values for the DDTM
  preference parameter are defined in cm_ddtm_pref_e_type().

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] ddtm_pref      Switch to this DDTM preference.
@param[in] ddtm_act       When DDTM is enabled, this field specifies which
                          combined DDTM actions should take place to enhance
                          HDR data throughput. This is a bitmapped enumeration,
                          with each bit specifying an action.
@param[in] so_list_act    Specifies what to do (add, replace, delete) in the
                          the user-specified SO list in the following parameters.
@param[in] num_srv_opt    Number of SOs specified in the list. The
                          maximum = SYS_DDTM_MAX_SO_LIST_SIZE.
@param[in] srv_opt_list   List of SOs for which 1X pages have to be
                          ignored while in the HDR Connected state. If
                          srv_opt_list[] contains CAI_SO_NULL and
                          ddtm_pref is ON, ignore all pages.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_ph_cmd_ddtm_pref(

  cm_ph_cmd_cb_f_type         cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type           client_id,
  cm_ddtm_pref_e_type         ddtm_pref,
  sys_ddtm_act_mask_e_type    ddtm_act,
  cm_ddtm_so_list_act_e_type  so_list_act,
  uint16                      num_srv_opt,
  const sys_srv_opt_type      srv_opt_list[]

);
 
/*~ FUNCTION cm_ph_cmd_ddtm_pref */
/*~ PARAM srv_opt_list VARRAY LENGTH num_srv_opt */

/*===========================================================================

FUNCTION: cm_ph_cmd_avoid_sys

===========================================================================*/
/**
  Commands the CM to avoid the systems defined in avoid_type
  (avoid_time is no longer used). CDMA only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] avoid_type     Type of system to avoid.
@param[in] avoid_time     OBSOLETE.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_avoid_sys(

  cm_ph_cmd_cb_f_type     cmd_cb_func,
  void                    *data_block_ptr,
  cm_client_id_type       client_id,
  cm_ss_avoid_sys_e_type  avoid_type,
  dword                   avoid_time

);
 
/*~ FUNCTION cm_ph_cmd_avoid_sys */

/*===========================================================================

FUNCTION: cm_ph_cmd_answer_voice

===========================================================================*/
/**
  Commands the CM to change the phone's answer-voice-as setting to a specified
  selection defined in cm_answer_voice_e_type(), such as answer-voice-as-data,
  answer-voice-as-voice, etc. CDMA only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.


@param[in] cmd_cb_func     Client callback function.
@param[in] data_block_ptr  Pointer to the client callback data block.
@param[in] client_id       Requesting client.
@param[in] answer_voice    Indicates if the phone should answer incoming
                           voice calls as voice, fax, or modem.
@param[in] answer_duration Indicates the duration (in seconds) for
                           temporary settings, such as
                           CM_ANSWER_VOICE_AS_FAX_ONCE or
                           DB_VOICE_AS_MODEM_ONCE.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_CDMA_800 or FEATURE_CDMA_1900 must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_answer_voice(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
  void                       *data_block_ptr,
  cm_client_id_type          client_id,
  cm_answer_voice_e_type     answer_voice,
  dword                      answer_duration

);
 
/*~ FUNCTION cm_ph_cmd_answer_voice */

/*===========================================================================

FUNCTION: cm_ph_cmd_nam_sel

==========================================================================*/
/**
  Commands the CM to change the phone's NAM selection to NAM-1,
  NAM-2, etc. CDMA only.

  The client-supplied callback function cmd_cb_func will be called
  to notify the client of the command status.

@note1hang
  NV_MAX_NAMS defines the maximum number of NAMs supported
  in the phone This command fails when trying to change the
  NAM to an illegal selection (e.g., changing to NAM-2 for a
  NAM-1 target).

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] nam_sel        Change NAM selection to this NAM.

@return
  None.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_CDMA_800 or FEATURE_CDMA_1900 must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_nam_sel(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
  void                       *data_block_ptr,
  cm_client_id_type          client_id,
  cm_nam_e_type              nam_sel

);
 
/*~ FUNCTION cm_ph_cmd_nam_sel */

/*===========================================================================

FUNCTION: cm_ph_cmd_get_ph_info

===========================================================================*/
/**
  Gets a snapshot of the current phone information in the CM.
  This function submits a request for a snapshot of the phone
  object. The client must register for CM_PH_EVENT_INFO in
  order to receive the response to this command.
  CM_PH_EVENT_INFO is sent only to the client that issues this command.

  The client can call this function only after it has
  received a CM_PH_EVENT_INFO_AVAIL notification from the CM.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_ph_cmd_get_ph_info(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id

);
 
/*~ FUNCTION cm_ph_cmd_get_ph_info */

/*===========================================================================

FUNCTION: cm_ph_cmd_rssi_delta_threshold

===========================================================================*/
/**
  Commands the CM to set the Receive Signal Strength Indicator
  (RSSI) delta threshold to the specified value. The default
  delta threshold is 5 dBm. When the RSSI change exceeds the
  delta threshold, the CM sends CM_SS_EVENT_RSSI to the
  registered client.

  RSSI Default = 5 dBm

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.
@param[in] delta          RSSI change delta threshold for RSSI reporting.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_ph_cmd_rssi_delta_threshold(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  uint8                  delta

);
 
/*~ FUNCTION cm_ph_cmd_rssi_delta_threshold */

/*===========================================================================

FUNCTION: cm_ph_cmd_signal_strength_delta

===========================================================================*/
/**
  Commands the CM to set the RSSI, Ec/Io, and IO delta thresholds to the
  specified values.

  RSSI Default = 5 dBm
  ECIO Default = 2 dBm
  IO Default = 5 dBm

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client         Requesting client ID.
@param[in] rssi_delta     RSSI change delta threshold for signal strength
                          reporting.
@param[in] ecio_delta     Ec/Io change delta threshold for signal strength
                          reporting.
@param[in] io_delta       IO change delta threshold for signal strength
                          reporting.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_ph_cmd_signal_strength_delta(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client,
  uint8                  rssi_delta,
  uint8                  ecio_delta,
  uint8                  io_delta

);
/*~ FUNCTION cm_ph_cmd_signal_strength_delta */

/*===========================================================================

FUNCTION: cm_ph_cmd_signal_strength_delta2

===========================================================================*/
/**
  Commands the CM to set the RSSI, Ec/Io, IO, SIR, and PATHLOSS
  delta thresholds to the specified values.

  RSSI Default = 5 dBm
  ECIO Default = 2 dBm
  IO Default = 5 dBm
  SIR Default = 3 dBm
  Pathloss Default = 5 dBm


@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client         Requesting client ID.
@param[in] rssi_delta     RSSI change delta threshold for signal strength
                          reporting.
@param[in] ecio_delta     Ec/Io change delta threshold for signal strength
                          reporting.
@param[in] io_delta       IO change delta threshold for signal strength
                          reporting.
@param[in] sir_delta      Signal to Interference Ratio (SIR) change delta
                          threshold for signal strength reporting.
@param[in] pathloss_delta PATHLOSS change delta threshold for signal
                          strength reporting.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_ph_cmd_signal_strength_delta2(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client,
  uint8                  rssi_delta,
  uint8                  ecio_delta,
  uint8                  io_delta,
  uint8                  sir_delta,
  uint8                  pathloss_delta

);
/*~ FUNCTION cm_ph_cmd_signal_strength_delta2 */

/*===========================================================================

FUNCTION: cm_ph_cmd_signal_strength_delta3

==========================================================================*/
/**
  Sets the RSSI, Ec/Io, IO, SIR, PATHLOSS, and Channel Quality Indication (CQI)
  delta values.

  RSSI Default = 5 dBm
  ECIO Default = 2 dBm
  IO Default = 5 dBm
  SIR Default = 3 dBm
  Pathloss Default = 5 dBm
  CQI Default = 1 dBm

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client         Requesting client ID.
@param[in] rssi_delta     RSSI change delta threshold for signal strength
                          reporting.
@param[in] ecio_delta     Ec/Io change delta threshold for signal strength
                          reporting.
@param[in] io_delta       IO change delta threshold for signal strength
                          reporting.
@param[in] sir_delta      SIR change delta threshold for signal strength
                          reporting.
@param[in] pathloss_delta PATHLOSS change delta threshold for signal
                          strength reporting.
@param[in] cqi_delta      CQI change delta threshold for signal strength
                          reporting.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start().
*/
boolean cm_ph_cmd_signal_strength_delta3(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
    void                   *data_block_ptr,
    cm_client_id_type      client,
    uint8                  rssi_delta,
    uint8                  ecio_delta,
    uint8                  io_delta,
    uint8                  sir_delta,
    uint8                  pathloss_delta,
    uint8                  cqi_delta

);
/*~ FUNCTION cm_ph_cmd_signal_strength_delta3 */

/*===========================================================================

FUNCTION cm_ph_cmd_signal_strength_delta_lte

DESCRIPTION
  Set the RSSI/ECIO/IO/SIR/PATHLOSS/CQI/RSRQ/RSRP delta value.

  RSSI Default = 5 dBm
  ECIO Default = 2 dBm
  IO Default = 5 dBm
  SIR Default = 3 dBm
  Pathloss Default = 5 dBm
  CQI Default = 1 dBm
  RSRP Default = 5 dBm
  RSRQ Default = 5 dBm

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_signal_strength_delta_lte(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  rssi_delta,
        /* RSSI change delta threshold for Signal Strength reporting */

    uint8                  ecio_delta,
        /* ECIO change delta threshold for Signal Strength  reporting */

    uint8                  io_delta,
        /* IO change delta threshold for Signal Strength  reporting */

    uint8                  sir_delta,
        /* SIR change delta threshold for Signal Strength  reporting */

    uint8                  pathloss_delta,
        /* PATHLOSS change delta threshold for Signal Strength  reporting */

    uint8                  cqi_delta,
        /* CQI change delta threshold for Signal Strength  reporting */

    uint8                   rsrp_delta,
        /* LTE RSRP change delta threshold for Signal Strength  reporting */
        
    uint8                   rsrq_delta
        /* LTE RSRQ change delta threshold for Signal Strength  reporting */
);
/*~ FUNCTION cm_ph_cmd_signal_strength_delta_lte */

/*===========================================================================

FUNCTION: cm_ph_cmd_subscription_available

==========================================================================*/
/**
   Informs the CM that subscription/provisioning information
   has been verified and is available for consumption.

   The result of this command may be the start of full service
   acquisition. The client-supplied callback function cmd_cb_func
   will be called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] cdma_status    Subscription status of a CDMA system.
@param[in] gwl_status     Subscription status of a GSM/WCDMA/LTE system.
@param[in] wlan_status    Subscription status of a WLAN system. Clients must
                          check CM_API_WLAN for WLAN support before using
                          this field.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the subscription is currently available, the request is ignored and
  CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned. \n
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_subscription_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type               client_id,
  cm_subscription_status_e_type   cdma_status,
  cm_subscription_status_e_type   gwl_status,
  cm_subscription_status_e_type   wlan_status

);

/*~ FUNCTION cm_ph_cmd_subscription_available */

/*===========================================================================

FUNCTION: cm_ph_cmd_subscription_not_available

===========================================================================*/
/**
  Informs the CM that subscription/provisioning information
  is not available for consumption.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] cdma_status    Subscription status of a CDMA system.
@param[in] gwl_status     Subscription status of a GSM/WCDMA/LTE system.
@param[in] wlan_status    Subscription status of a WLAN system. Clients must
                          check CM_API_WLAN for WLAN support before using
                          this field.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the subscription is currently available, the request is ignored and
  CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned. \n
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_subscription_not_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type               client_id,
  cm_subscription_status_e_type   cdma_status,
  cm_subscription_status_e_type   gwl_status,
  cm_subscription_status_e_type   wlan_status

);
 
/*~ FUNCTION cm_ph_cmd_subscription_not_available */

/*===========================================================================

FUNCTION: cm_ph_cmd_subscription_changed

===========================================================================*/
/**
  Informs the CM that subscription/provisioning information
  has changed.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] cdma_status    Subscription status of a CDMA system.
@param[in] gwl_status     Subscription status of a GSM/WCDMA/LTE system.
@param[in] wlan_status    Subscription status of a WLAN system. Clients must
                          check CM_API_WLAN for WLAN support before using
                          this field.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the subscription is currently available, the request is ignored and
  CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned. \n
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_subscription_changed(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type               client_id,
  cm_subscription_status_e_type   cdma_status,
  cm_subscription_status_e_type   gwl_status,
  cm_subscription_status_e_type   wlan_status

);
 
/*~ FUNCTION cm_ph_cmd_subscription_changed */

/*===========================================================================

FUNCTION: cm_ph_cmd_get_networks

===========================================================================*/
/**
  Commands the CM to retrieve either a list of preferred
  networks or a list of all available networks. GSM/WCDMA only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] list_type      Indicates whether to retrieve the preferred or
                          available networks.
@param[in] network_type   Indicates the desired system, such as WLAN or GW.
                          Only WLAN or GW is supported, and they can only
                          be requested one at a time. Clients must check
                          CM_API_WLAN for WLAN support before using this field.

@return 
  TRUE -- Command was successfully sent to the CM. \n
  FALSE -- Command was not successfully sent to the CM.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_GSM or FEATURE_WCDMA must be defined.

@sideeffects
  If the command is successful, CM clients receive the list of networks via the
  CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or CM_PH_EVENT_PREFERRED_- NETWORKS_CONF
  event.
*/
extern boolean cm_ph_cmd_get_networks(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type            client_id,
  cm_network_list_type_e_type  list_type,
  cm_mode_pref_e_type          network_type

);
 
/*~ FUNCTION cm_ph_cmd_get_networks */

/*===========================================================================

FUNCTION: cm_ph_cmd_terminate_get_networks

===========================================================================*/
/**
  Commands the CM to terminate the retrieval operation of a list
  of all available networks. GSM/WCDMA only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.

@return
  TRUE -- Command was successfully sent to the CM. \n
  FALSE -- Command was not successfully sent to the CM.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_GSM or FEATURE_WCDMA must be defined.

@sideeffects
  If the command is successful, the previous command to retrieve the list is
  terminated.
*/
extern boolean cm_ph_cmd_terminate_get_networks(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type            client_id

);
 
/*~ FUNCTION cm_ph_cmd_terminate_get_networks */

/*===========================================================================

FUNCTION: cm_ph_cmd_wps_start

===========================================================================*/
/**
  Commands the CM to get WLAN security credentials using WPS. WLAN only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func     Client callback function.
@param[in] data_block_ptr  Pointer to the client callback data block.
@param[in] client_id       Requesting client.
@param[in] wps_start_param Information required to start the WPS procedure.

@return
  TRUE -- Command was successfully sent to the CM. \n
  FALSE -- Command was not successfully sent to the CM.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients receive a list of WLAN credentials
  via the CM_PH_EVENT_WPS_COMPLETED event.
*/
extern boolean cm_ph_cmd_wps_start(

        cm_ph_cmd_cb_f_type              cmd_cb_func,
        void                            *data_block_ptr,
        cm_client_id_type                client_id,
        sys_wlan_wps_start_param_s_type  const *wps_start_param

);
/*~ FUNCTION cm_ph_cmd_wps_start */
/*~ PARAM wps_start_param POINTER */

/*===========================================================================

FUNCTION: cm_ph_cmd_wps_abort

===========================================================================*/
/**
  Commands the CM to terminate a WPS procedure. WLAN only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.

@return
  TRUE -- Command was successfully sent to the CM. \n
  FALSE -- Command was not successfully sent to the CM.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, the previous command to retrieve the WLAN
  credentials using WPS is aborted.
*/
extern boolean cm_ph_cmd_wps_abort(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type            client_id

);
/*~ FUNCTION cm_ph_cmd_wps_abort */

/*===========================================================================

FUNCTION: cm_ph_cmd_set_preferred_networks

===========================================================================*/
/**
  Commands the CM to save the preferred networks to the
  SIM/USIM. GSM/WCDMA only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func            Client callback function.
@param[in] data_block_ptr         Pointer to the client callback data block.
@param[in] client_id              Requesting client.
@param[in] preferred_networks_ptr Preferred networks to be saved.

@return
  TRUE -- Command was successfully sent to the CM. \n
  FALSE -- Command was not successfully sent to the CM.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern boolean cm_ph_cmd_set_preferred_networks(

  cm_ph_cmd_cb_f_type              cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type                client_id,
  const sys_user_pref_plmn_list_s_type  *preferred_networks_ptr

);
 
/*~ FUNCTION cm_ph_cmd_set_preferred_networks */
/*~ PARAM preferred_networks_ptr POINTER */

/*===========================================================================

FUNCTION: cm_ph_cmd_packet_state

===========================================================================*/
/**
  Commands the CM to change the Rm packet state in packet mode. CDMA Only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] packet_state   Change the Rm packet state to this state.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_ph_cmd_packet_state(

  cm_ph_cmd_cb_f_type         cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type           client_id,
  cm_packet_state_e_type      packet_state

);

 
/*~ FUNCTION cm_ph_cmd_packet_state */

/*===========================================================================

FUNCTION: cm_ph_cmd_wakeup_from_standby

===========================================================================*/
/**
  Commands the CM to inform lower layers to wake up from deep sleep (Standby
  mode).

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_wakeup_from_standby(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
  void                      *data_block_ptr,
  cm_client_id_type          client_id

);
 
/*~ FUNCTION cm_ph_cmd_wakeup_from_standby */

/*===========================================================================

FUNCTION: cm_ph_cmd_wakeup_from_standby2

===========================================================================*/
/**
  Commands the CM to inform lower layers to wake up from deep
  sleep (Standby mode). Clients must check CM_API_WLAN for WLAN
  support before using this function.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] service        Service to be awakened.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_JCDMA must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_wakeup_from_standby2(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
  void                      *data_block_ptr,
  cm_client_id_type          client_id,
  cm_wakeup_e_type           service

);
 
/*~ FUNCTION cm_ph_cmd_wakeup_from_standby2 */

/*===========================================================================

FUNCTION: cm_ph_cmd_change_rtre_config

===========================================================================*/
/**
  Commands the CM to change RTRE configuration. On successful
  execution of this command, an event is returned to the client
  indicating the current RTRE configuration as well as RTRE
  control. Possible values for the RTRE configuration are listed in
  cm_rtre_config_e_type. Possible values for the RTRE control
  are defined in cm_rtre_control_e_type. CDMA only.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] rtre_config    New RTRE configuration.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_UIM_RUN_TIME_ENABLE must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_change_rtre_config(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  cm_rtre_config_e_type  rtre_config

);
 
/*~ FUNCTION cm_ph_cmd_change_rtre_config */

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_reset_acm

==========================================================================*/
/**
  Resets the ACM value to zero on a system supporting ACM/ACMAX.

@note1hang
  Ensure that PIN2 (CHV2) has been unlocked on the UIM
  before issuing this command, or it will fail.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern boolean cm_ph_cmd_aoc_reset_acm(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id

);
 
/*~ FUNCTION cm_ph_cmd_aoc_reset_acm */

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_set_acmmax

===========================================================================*/
/**
  Sets the ACMMax value in the SIM to the requested value.

@note1hang
  Ensure that PIN2 (CHV2) has been unlocked on the UIM
  before issuing this command, or it will fail.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] new_max        New maximum value for ACMMAX.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern boolean cm_ph_cmd_aoc_set_acmmax(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  uint32                 new_max

);
 
/*~ FUNCTION cm_ph_cmd_aoc_set_acmmax */

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_get_ccm

===========================================================================*/
/**
  Returns the current value of the Current Call Meter (CCM).

@return
  Current call meter value.

@dependencies
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern unsigned long cm_ph_cmd_aoc_get_ccm(void);
 
/*~ FUNCTION cm_ph_cmd_aoc_get_ccm */

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_get_acm

===========================================================================*/
/**
  Returns the current value of the Accumulated Call Meter (ACM).

@return
  Accumulated Call Meter value.

@dependencies
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern unsigned long cm_ph_cmd_aoc_get_acm(void);
 
/*~ FUNCTION cm_ph_cmd_aoc_get_acm */

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_get_acmmax

===========================================================================*/
/**
  Returns the current value of ACMMax.

@return
  Current value of ACMMax.

@dependencies
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern unsigned long cm_ph_cmd_aoc_get_acmmax(void);
 
/*~ FUNCTION cm_ph_cmd_aoc_get_acmmax */

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_get_puct

===========================================================================*/
/**
  Returns the current value of the AOC price unit conversion value.
 
@return
  AOC price unit conversion value.

@dependencies
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern unsigned long cm_ph_cmd_aoc_get_puct(void);
 
/*~ FUNCTION cm_ph_cmd_aoc_get_puct */

/*===========================================================================

FUNCTION: cm_ph_cmd_line_switching

===========================================================================*/
/**
  Queues the CM_PH_CMD_LINE_SWITCHING command to the CM.

@param[in] cmd_cb_func    Client command callback function.
@param[in] data_block_ptr Pointer to the client command callback data block.
@param[in] client_id      Commanding client's ID.
@param[in] status         Indicates if the user is allowed to switch between
                          lines.

@return 
  TRUE -- Command was queued successfully to the CM. \n
  FALSE -- No CM buffer available to queue the command.

@dependencies
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern boolean cm_ph_cmd_line_switching(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
  void                                *data_block_ptr,
  cm_client_id_type                    client_id,
  cm_als_line_switching_e_type         status

);
 
/*~ FUNCTION cm_ph_cmd_line_switching */

/*===========================================================================

FUNCTION: cm_ph_cmd_selected_line

===========================================================================*/
/**
  Queues the CM_PH_CMD_SELECTED_LINE command to the CM.

@param[in] cmd_cb_func    Client command callback function.
@param[in] data_block_ptr Pointer to the client command callback data block.
@param[in] client_id      Commanding client's ID.
@param[in] selected_line  Selected line.

@return 
  TRUE -- Command was queued successfully to the CM. \n
  FALSE -- No CM buffer available to queue the command or
           incorrect parameters passed to the function.

@dependencies
  FEATURE_ALS and FEATURE_GSM must be defined.
*/
extern boolean cm_ph_cmd_selected_line(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
  void                                *data_block_ptr,
  cm_client_id_type                    client_id,
  cm_als_line_e_type                   selected_line

);
 
/*~ FUNCTION cm_ph_cmd_selected_line */

/*===========================================================================

FUNCTION: cm_ph_cmd_detach_and_plmn_blocking 

===========================================================================*/
/**
  Commands the CM to trigger LTE detach and/or PLMN blocking.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.


@param[in] cmd_cb_func         Client callback function.
@param[in] data_block_ptr      Pointer to the client callback data block.
@param[in] client_id           Requesting client.
@param[in] plmn_blocking_info  detailed info for LTE detach and/or PLMN blocking.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_detach_and_plmn_blocking (

  cm_ph_cmd_cb_f_type                    cmd_cb_func,
  void                                   *data_block_ptr,
  cm_client_id_type                      client_id,
  cm_plmn_blocking_info_s_type           plmn_blocking_info

);
 
/*~ FUNCTION cm_ph_cmd_detach_and_plmn_blocking */

/*===========================================================================

FUNCTION: cm_ph_cmd_update_cdma_capability

==========================================================================*/
/**
  Commands the CM to update the CDMA2000 capability information
  in the form of an IS2000 status message.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] is2000_msg_ptr Pointer to the IS2000 message.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_ph_cmd_update_cdma_capability(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  const sys_is2000_msg_s_type       *is2000_msg_ptr

);

/** @} */ /* end_addtogroup PHONE Phone-Related Definitions */

/*-----------------------------------------------------------------------------

Serving System Object

-----------------------------------------------------------------------------*/

/** @addtogroup SERVSYSOBJ Serving System Object
    @{ */
 
/*~ FUNCTION cm_ph_cmd_update_cdma_capability */
/*~ PARAM is2000_msg_ptr POINTER */

/* Default values for serving system stored information.*/


#define CM_SS_SID_NONE                         ( (uint16) 0 )
  /**< Indicates an undefined System Identifier (SID). CDMA only. */

#define CM_SS_NID_NONE                         ( (uint16) 0 )
  /**< Indicates an undefined Network Identifier (NID). CDMA only. */

#define CM_SS_REG_ZONE_NONE                    ( (uint16) 0xFFFF )
  /**< Indicates an undefined registration zone. CDMA only. */

#define CM_SS_PACKET_ZONE_NONE                 ( (uint8) 0 )
  /**< Indicates an undefined packet zone. CDMA only. */

#define CM_SS_BS_P_REV_NONE                    ( (uint8) 0 )
  /**< Indicates an undefined base station P-rev. CDMA only. */

#define CM_SS_P_REV_IN_USE_NONE                ( (uint8) 0 )
  /**< Indicates an undefined P-rev. CDMA only. */

#define CM_COUNTRY_CODE_LIST_SIZE                      5
   /**< List of countries presented for conflict resolution. */

#define CM_INVALID_MOBILE_COUNTRY_CODE                (cm_country_code_type)(-1)
   /**< Country code not pointing to any country. Mirrors the 
        SD_MCC_NONE value. */

/* Bitmasks indicating which fields of the MAIN stack are changed
     in cm_mm_ss_info_s_type structure. */

#define CM_SS_SID_MASK                          ( SYS_BM_64BIT(0) )
  /**< SID. */

#define CM_SS_NID_MASK                          ( SYS_BM_64BIT(1)  )
  /**< NID. */

#define CM_SS_REG_ZONE_MASK                     ( SYS_BM_64BIT(2)  )
  /**< Registration zone. */

#define CM_SS_PACKET_ZONE_MASK                  ( SYS_BM_64BIT(3)  )
  /**< Packet zone. */

#define CM_SS_BS_P_REV_MASK                     ( SYS_BM_64BIT(4)  )
  /**< Basestation P-rev. */

#define CM_SS_P_REV_IN_USE_MASK                 ( SYS_BM_64BIT(5)  )
  /**< P-rev in use. */

#define CM_SS_CCS_SUPPORTED_MASK                ( SYS_BM_64BIT(6)  )
  /**< Concurrent services supported. */

#define CM_SS_SRV_STATUS_MASK                   ( SYS_BM_64BIT(7)  )
  /**< Serving status: No, limited, full. */

#define CM_SS_CDMA_SRV_STATUS_MASK              CM_SS_SRV_STATUS_MASK
  /**< CDMA serving status. */

#define CM_SS_SRV_DOMAIN_MASK                   ( SYS_BM_64BIT(8)  )
  /**< Service domain. */

#define CM_SS_CDMA_SRV_DOMAIN_MASK              CM_SS_SRV_DOMAIN_MASK
  /**< CDMA service domain. */

#define CM_SS_SRV_CAPABILITY_MASK               ( SYS_BM_64BIT(9)  )
  /**< Serving System service capability. */

#define CM_SS_CDMA_SRV_CAPABILITY_MASK          CM_SS_SRV_CAPABILITY_MASK
  /**< CDMA Serving System service capability. */

#define CM_SS_SYS_MODE_MASK                     ( SYS_BM_64BIT(10) )
  /**< System mode. */

#define CM_SS_ROAM_STATUS_MASK                  ( SYS_BM_64BIT(11) )
  /**< Roaming status. */

#define CM_SS_CDMA_ROAM_STATUS_MASK             CM_SS_ROAM_STATUS_MASK
  /**< CDMA roaming status. */

#define CM_SS_SYS_ID_MASK                       ( SYS_BM_64BIT(12) )
  /**< System ID. */

#define CM_SS_CDMA_SYS_ID_MASK                  CM_SS_SYS_ID_MASK
  /**< CDMA system ID. */

#define CM_SS_SRV_IND_MASK                      ( SYS_BM_64BIT(13) )
  /**< Service indicators. This mask indicates that a change of service
       occurred. It is set even if the service is the same as the previous
       service. */

#define CM_SS_MOBILITY_MGMT_MASK                ( SYS_BM_64BIT(14) )
  /**< Mobility management. */

#define CM_SS_IDLE_DIGITAL_MODE_MASK            ( SYS_BM_64BIT(15) )
  /**< HDR; idle digital mode. */

#define CM_SS_SIM_STATE_MASK                    ( SYS_BM_64BIT(16) )
  /**< SIM card status. */

#define CM_SS_PLMN_FORBIDDEN_MASK               ( SYS_BM_64BIT(17) )
  /**< Forbidden PLMN. */

#define CM_SS_SYS_FORBIDDEN_MASK                CM_SS_PLMN_FORBIDDEN_MASK
  /**< Forbidden system. */

#define CM_SS_PS_DATA_SUSPEND_MASK              ( SYS_BM_64BIT(18) )
  /**< PS data suspend mask. */

#define CM_SS_UZ_CHANGED_MASK                   ( SYS_BM_64BIT(19) )
  /**< User Zone values changed. */

#define CM_SS_HDR_SRV_STATUS_MASK               ( SYS_BM_64BIT(20) )
  /**< HDR Serving Status: No, limited, full. */

#define CM_SS_HDR_ROAM_STATUS_MASK              ( SYS_BM_64BIT(21) )
  /**< HDR roaming status. */

#define CM_SS_MAIN_BCMCS_SRV_SUPPORTED_MASK     ( SYS_BM_64BIT(22) )
  /**< Whether 1X supports Broadcast and Multicast Services (BCMCS). */

#define CM_SS_MAIN_BCMCS_SRV_STATUS_MASK        ( SYS_BM_64BIT(23) )
  /**< BCMCS service status on 1X. */

#define CM_SS_HYBR_HDR_BCMCS_SRV_SUPPORTED_MASK ( SYS_BM_64BIT(24) )
  /**< Whether HDR supports BCMCS service. */

#define CM_SS_HYBR_HDR_BCMCS_SRV_STATUS_MASK    ( SYS_BM_64BIT(25) )
  /**< BCMCS service status on HDR. */

#define CM_SS_BASE_STATION_PARMS_CHGD_MASK      ( SYS_BM_64BIT(26) )
  /**< Base station parameters changed. */

/* Clients check CM_API_WLAN for WLAN support for following*/

#define CM_SS_WLAN_SRV_STATUS_MASK              ( SYS_BM_64BIT(27) )
  /**< WLAN serving status: No, full. Clients must check CM_API_WLAN 
       for WLAN support. */

#define CM_SS_WLAN_BSS_INFO_MASK                ( SYS_BM_64BIT(28) )
  /**< WLAN BSS information change is indicated. Clients must check 
       CM_API_WLAN for WLAN support. */

#define CM_SS_ORIG_THR_STATUS_MASK              ( SYS_BM_64BIT(29) )
  /**< Origination throttle status changed. */

#define CM_SS_HDR_SYS_ID_MASK                   ( SYS_BM_64BIT(30) )
  /**< HDR system ID. */

#define CM_SS_MCC_MASK                          ( SYS_BM_64BIT(31) )
  /**< MCC changed. */

#define CM_SS_IMSI_11_12_MASK                   ( SYS_BM_64BIT(32) )
  /**< International Mobile Subscriber Identity (IMSI) changed. */

#define CM_SS_PSIST_CHGD_MASK                   ( SYS_BM_64BIT(33) )
  /**< Persist information changed. */

#define CM_SS_ACTIVE_BAND_MASK                  ( SYS_BM_64BIT(34) )
  /**< Active band information. Clients must check for
       CM_API_ECATEG_IN_NUM_STRUCT before using this field. */

#define CM_SS_HDR_ACTIVE_PROT_MASK              ( SYS_BM_64BIT(35) )
  /**< HDR active protocol is changed.\ Must define
       CM_API_HDR_ACTIVE_PROT to use this field. */

#define CM_SS_WLAN_LINK_QUAL_MASK               ( SYS_BM_64BIT(36) )
  /**< WLAN link quality has changed in the WLAN_STATS SS event. */

#define CM_SS_WLAN_TX_RX_STATS_MASK             ( SYS_BM_64BIT(37) )
  /**< WLAN Tx and Rx stats have changed in the WLAN STATS SS event. */

#define CM_SS_TRUE_SRV_STATUS_MASK              ( SYS_BM_64BIT(38)  )
  /**< True serving status: No, limited, limited_regional. */

#define CM_SS_CELL_INFO_MASK                    ( SYS_BM_64BIT(39) )
  /**< CELL_INFO: change in cell_id and neighbor_cell_id. */

#define CM_SS_GAN_STATE_MASK                    ( SYS_BM_64BIT(40) )
  /**< UMA GAN client state: Initialization and shutdown indication. */

#define CM_SS_GAN_CONN_STATUS_MASK              ( SYS_BM_64BIT(41) )
  /**< Unlicensed Mobile Access (UMA) GAN connection status. */

#define CM_SS_GAN_SIG_CHAN_STATUS_MASK          ( SYS_BM_64BIT(42) )
  /**< UMA GAN signal channel status. */

#define CM_SS_GAN_MODE_DISPLAY_MASK             ( SYS_BM_64BIT(43) )
  /**< UMA GAN indication whether WLAN or GERAN becomes the active RAT. */

#define CM_SS_GAN_ERROR_PROMPT_MASK             ( SYS_BM_64BIT(44) )
  /**< UMA GAN error category. */

#define CM_SS_ACTIVE_CHANNEL_MASK               ( SYS_BM_64BIT(45) )
  /**< Active channel information. */

#define CM_SS_HYBRID_ACTIVE_BAND_MASK              ( SYS_BM_64BIT(46) )
  /**< Active HYBRID band information. */

#define CM_SS_HYBRID_ACTIVE_CHANNEL_MASK           ( SYS_BM_64BIT(47) )
  /**< Active HYBRID channel information. */

#define CM_SS_DEF_ROAM_IND_MASK                    ( SYS_BM_64BIT(48) )
  /**< Default roaming indicator from PRL. */

#define CM_SS_SYS_PRL_MATCH_MASK                   ( SYS_BM_64BIT(49) )
  /**< PRL Match information changed. */

#define CM_SS_CDMA_TIME_CHGD_MASK                  ( SYS_BM_64BIT(50) )
  /**< CDMA/HDR time-related information changed. */

#define CM_SS_LAC_TAC_RAC_CHGD_MASK                ( SYS_BM_64BIT(51) )
  /**< Location Area Code (LAC), Tracking Area Code (TAC), or Routing Area
       Code (RAC) changed. */

#define CM_SS_NO_CHANGE_MASK                       ( SYS_BM_64BIT(52) )
  /**< No change in System Selection. */

#define CM_SS_HDR_PERSONALITY_MASK                 ( SYS_BM_64BIT(53) )
  /**< HDR personality is changed. */

#define CM_SS_SUBS_ID_MASK                      ( SYS_BM_64BIT(54) )
  /**< Subscription ID of main stack. */

#define CM_SS_CSFB_CALL_ACTIVE_MASK                ( SYS_BM_64BIT(55) )
  /**< CSFB call status is changed */

#define CM_SS_VOICE_SUPPORT_ON_LTE_MASK            ( SYS_BM_64BIT(56) )
  /**< Voice support status on LTE is changed */

#define CM_SS_RESEL_TARGET_RAT_MASK                ( SYS_BM_64BIT(57) )
  /**< Target RAT mask when doing DO to LTE reselection  */

#define CM_SS_CELL_BROADCAST_MASK                  ( SYS_BM_64BIT(58) )
  /**< Cell broadcast capability changed */

#define CM_SS_LTE_IMS_VOICE_AVAIL_MASK             ( SYS_BM_64BIT(59) )
  /**< IMS voice support status on LTE is changed */

#define CM_SS_BSR_PROGRESS_MASK                    ( SYS_BM_64BIT(60) )
  /**< BSR progress info is changed */

#define CM_SS_CELLID_DCH_MASK                      ( SYS_BM_64BIT(61) ) 
  /**< CM_SS_EVENT_SRV_CHANGED is in response to SS_CMD_GET_DCH_CEL_ID */

/*---------------------------------------------------------------------------*/
/* HYBR GW stack bitmasks.
*/

#define CM_SS_GW_SUBS_ID_MASK                   ( SYS_BM_64BIT(0) )
  /**< Subscription ID of hybrid stack. */

#define CM_SS_GW_SRV_STATUS_MASK                ( SYS_BM_64BIT(1) )
  /**< GW Serving Status - No, limited, full. */

#define CM_SS_GW_TRUE_SRV_STATUS_MASK           ( SYS_BM_64BIT(2) )
  /**< True Serving Status - No, limited, limited_regional. */

#define CM_SS_GW_SRV_IND_MASK                   ( SYS_BM_64BIT(3) )
  /**< Service indicators - This mask indicates a change of service occurred.
       It is set even if the service is the same as the previous service. */

#define CM_SS_GW_CELL_INFO_MASK                 ( SYS_BM_64BIT(4) )
  /**< CELL_INFO - change in cell_id and neighbor cell_id. */

#define CM_SS_GW_ROAM_STATUS_MASK               ( SYS_BM_64BIT(5) )
  /**< GW roaming status. */

#define CM_SS_GW_SRV_DOMAIN_MASK                ( SYS_BM_64BIT(6) )
  /**< Service domain on GW changed. */

#define CM_SS_GW_SRV_CAPABILITY_MASK            ( SYS_BM_64BIT(7) )
  /**< Service capability on GW changed. */

#define CM_SS_GW_SYS_MODE_MASK                  ( SYS_BM_64BIT(8) )
  /**< System mode on GW changed. */

#define CM_SS_GW_ACTIVE_BAND_MASK               ( SYS_BM_64BIT(9) )
  /**< Active band information.\ Clients must check for
       CM_API_ECATEG_IN_NUM_STRUCT before using this field. */

#define CM_SS_GW_ACTIVE_CHANNEL_MASK            ( SYS_BM_64BIT(10) )

#define CM_SS_GW_SYS_ID_MASK                    ( SYS_BM_64BIT(11) )
  /**< System ID. */

#define CM_SS_GW_MOBILITY_MGMT_MASK             ( SYS_BM_64BIT(12) )
  /**< Mobility management information of hybrid stack. */

#define CM_SS_GW_SIM_STATE_MASK                 ( SYS_BM_64BIT(13) )
  /**< Hybrid GW SIM card status. */

#define CM_SS_GW_RSSI_MASK                      ( SYS_BM_64BIT(14) )
  /**< RSSI. */

#define CM_SS_GW_RSCP_MASK                      ( SYS_BM_64BIT(15) )
  /**< RSCP. */

#define CM_SS_GW_ECIO_MASK                      ( SYS_BM_64BIT(16) )
  /**< ECIO. */

#define CM_SS_GW_IO_MASK                        ( SYS_BM_64BIT(17) )
  /**< IO. */

#define CM_SS_GW_SINR_MASK                      ( SYS_BM_64BIT(18) )
  /**< SINR. */

#define CM_SS_GW_SIR_MASK                       ( SYS_BM_64BIT(19) )
  /**< SIR. */

#define CM_SS_GW_PATHLOSS_MASK                  ( SYS_BM_64BIT(20) )
  /**< PATHLOSS. */

#define CM_SS_GW_SYS_PRL_MATCH_MASK             ( SYS_BM_64BIT(21) )
  /**< PRL match information changed. */

#define CM_SS_GW_SYS_FORBIDDEN_MASK             ( SYS_BM_64BIT(22) )
  /**< System forbidden. */

#define CM_SS_SIMULTANEOUS_MODE_MASK            ( SYS_BM_64BIT(23) )
  /**< Simultaneous mode changed. */

#define CM_SS_GW_PS_DATA_SUSPEND_MASK           ( SYS_BM_64BIT(24) )
  /**< Packet Switched data suspended. */
   
#define CM_SS_GW_BER_MASK                       ( SYS_BM_64BIT(25) )
  /**< Bit Error Rate */

#define CM_SS_GW_CELL_BROADCAST_MASK            ( SYS_BM_64BIT(26) )
  /**< Cell broadcast capability changed */


#define CM_SS_GWL_LAC_TAC_RAC_CHGD_MASK         ( SYS_BM_64BIT(27) )
  /**< Location Area Code (LAC), Tracking Area Code (TAC), or Routing Area
       Code (RAC) changed. */

#define CM_SS_GWL_RSRP_MASK                     ( SYS_BM_64BIT(28) )
  /**< LTE Reference Signal Received Power. */

#define CM_SS_GWL_RSRQ_MASK                     ( SYS_BM_64BIT(29) )
  /**< LTE Reference Signal Received Quality. */

#define CM_SS_GW_CELLID_DCH_MASK                   ( SYS_BM_64BIT(30) )
  /**< CM_SS_EVENT_SRV_CHANGED is in response to SS_CMD_GET_DCH_CEL_ID */

/* Bit masks indicating which signal_strength fields are changed
** in cm_mm_ss_info_s_type structure used in RSSI event. */

#define CM_SS_RSSI_MASK                          ( SYS_BM_64BIT(0) )
  /**< Reverse Signal Strength Indicator. */

#define CM_SS_RSCP_MASK                          ( SYS_BM_64BIT(1) )
  /**< Received Signal Code Power. */

#define CM_SS_ECIO_MASK                          ( SYS_BM_64BIT(2) )
  /**< Pilot energy to spectral density (Ec/Io). */

#define CM_SS_IO_MASK                            ( SYS_BM_64BIT(3) )
  /**< IO. */

#define CM_SS_SINR_MASK                          ( SYS_BM_64BIT(4) )
  /**< Signal-to-Interference plus Noise Ratio. */

#define CM_SS_SIR_MASK                           ( SYS_BM_64BIT(5) )
  /**< Signal-to-Interference Ratio. */

#define CM_SS_PATHLOSS_MASK                      ( SYS_BM_64BIT(6) )
  /**< PATHLOSS. */

#define CM_SS_BER_MASK                           ( SYS_BM_64BIT(7) )
  /**< Bit Error Rate. */

#define CM_SS_FER_MASK                           ( SYS_BM_64BIT(8) )
  /**< Frame Error Rate. */

#define CM_SS_HDR_PER_MASK                       ( SYS_BM_64BIT(9) )
  /**< High Data Rate Packet Error Rate. */

#define CM_SS_HDR_RSSI_MASK                      ( SYS_BM_64BIT(10) )
  /**< HDR RSSI. */

#define CM_SS_HDR_IO_MASK                        ( SYS_BM_64BIT(11) )
  /**< HDR IO. */

#define CM_SS_HDR_ECIO_MASK                      ( SYS_BM_64BIT(12) )
  /**< HDR Ec/Io. */

#define CM_SS_HDR_SINR_MASK                      ( SYS_BM_64BIT(13) )
  /**< HDR SINR. */

#define CM_SS_CDMA_ECIO_MASK                     ( SYS_BM_64BIT(14) )
  /**< CDMA Ec/Io. */

#define CM_SS_RSRP_MASK                          ( SYS_BM_64BIT(15) )
  /**< LTE Reference Signal Received Power. */

#define CM_SS_RSRQ_MASK                          ( SYS_BM_64BIT(16) )
  /**< LTE Reference Signal Received Quality. */

/** @} */ /* end_addtogroup SERVSYSOBJ */

/** @addtogroup SERVSYSOBJ
    @{ */

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Enumeration of serving system commands clients are able to
    send to the CM.
*/
typedef enum cm_ss_cmd_e {

/** @cond
*/
  CM_SS_CMD_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_SS_CMD_INFO_GET,
    /**< Get Serving System information. */

  CM_SS_CMD_PS_DATA_AVAIL,
    /**< Get PS data available on request from Data Services. */

  CM_SS_CMD_ORIG_THR_UPDATE_TBL,
    /**< Update the throttle table. */

  CM_SS_CMD_COUNTRY_SELECTED,
    /**< Update current country as selected by user. */

  CM_SS_CMD_GET_MEASUREMENT,
    /**< Obtain system measurements. */

  CM_SS_CMD_GET_DCH_CELL_ID,	 
    /**< Get Valid CELL_INFO in DCH state */

/** @cond
*/
  CM_SS_CMD_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ss_cmd_e_type;
/*~ SENTINEL cm_ss_cmd_e_type.CM_SS_CMD_MAX */

/** Enumeration of Serving System command errors returned by
    call command callback functions.
*/
typedef enum cm_ss_cmd_err_e {

/** @cond
*/
  CM_SS_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_SS_CMD_ERR_NOERR,
    /**< No errors found. */

  /* Error codes resulting from clients feeding a serving system command
     function with bad or inappropriate parameters.
  */

  CM_SS_CMD_ERR_CLIENT_ID_P,
    /**< Bad client parameter. */

  /* Error codes resulting from operational limitations
  ** of the CM. */

  CM_SS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  CM_SS_CMD_ERR_OTHER,
    /**< Any other errors not listed above. */

  /* Error codes resulting from clients trying to instruct
     the CM to perform inappropriate actions relative to the current
     state of the call and/or phone.
  */

  CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to a relevant feature being undefined. */

  CM_SS_CMD_ERR_OFFLINE_S,
    /**< Phone is offline. */

  CM_SS_CMD_ERR_SRV_STATE_S,
    /**< Serving status is wrong. */

  CM_SS_CMD_ERR_PS_DATA_SUSPEND_S,
    /**< PS data suspend state is not correct. */

  CM_SS_CMD_ERR_INCORRECT_MODE_S,
    /**< Phone is not in the correct mode to honor the request. */

  CM_SS_CMD_ERR_BAD_PARM_P,
    /**< One of the parameters of the command is in error. */

/** @cond
*/
  CM_SS_CMD_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ss_cmd_err_e_type;
/*~ SENTINEL cm_ss_cmd_err_e_type.CM_SS_CMD_ERR_MAX */

/** Enumeration of call origination throttling reasons.
*/
typedef enum cm_ss_orig_thr_reasons_e {

/** @cond
*/
  CM_SS_ORIG_THR_REASON_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_SS_ORIG_THR_DS_MIP_UP_SUCCESS,
    /**< Mobile IP (MIP) success. */

  CM_SS_ORIG_THR_DS_MIP_PDSN_FAILURE,
    /**< Public Data Switching Network (PDSN), Point-to-Point 
         Protocol (PPP) setup failure. */

  CM_SS_ORIG_THR_DS_MIP_SOL_TIMEOUT,
    /**< MIP solicitation timeout. */

  CM_SS_ORIG_THR_DS_MIP_RRQ_TIMEOUT,
    /**< MIP Registration Request (RRQ) timeout. */

  /* 4 */
  CM_SS_ORIG_THR_DS_MIP_FA_FAILURE,
    /**< MIP Foreign Agent (FA) failure. */

  CM_SS_ORIG_THR_DS_MIP_HA_FAILURE,
    /**< MIP Home Agent (HA) failure. */

  CM_SS_ORIG_THR_DS_MIP_FAILURE,
    /**< MIP failed due to another network reason. */

  CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS,
    /**< PPP success. */

  /* 8 */
  CM_SS_ORIG_THR_DS_PPP_AUTH_FAILURE,
    /**< PPP authorization failure. */

  CM_SS_ORIG_THR_DS_PPP_LCP_TIMEOUT,
    /**< PPP Link Control Protocol (LCP) timeout. */

  CM_SS_ORIG_THR_DS_PPP_NCP_TIMEOUT,
    /**< PPP IP Control Protocol (IPCP) timeout. */

  CM_SS_ORIG_THR_DS_PPP_FAILURE,
    /**< PPP fail due to another network reason. */

  /* 12 */
  CM_SS_ORIG_THR_CM_INTERCEPT,
    /**< Received intercept from the BS. */

  CM_SS_ORIG_THR_CM_REL_SO_REJ,
    /**< SO reject. */

  CM_SS_ORIG_THR_CM_CD_GEN_OR_BUSY,
    /**< Connection denied with deny code busy or general. */

  CM_SS_ORIG_THR_CM_CD_BILL_OR_AUTH,
    /**< Billing or authorization failure. */

  /* 16 */
  CM_SS_ORIG_THR_DS_SIP_FAILURE,
    /**< SIP failure. */

  CM_SS_ORIG_THR_DS_V6_FAILURE,
    /**< IPV6 Failure. */

  CM_SS_ORIG_THR_DS_V6_SUCCESS,
    /**< IPV6 setup success. */

  CM_SS_ORIG_THR_DS_PPP_AUTH_FAILURE_PERMANENT,
    /**< Existing High-Rate Packet Data (EHRPD) authentication failure. */

/** @cond
*/
  CM_SS_ORIG_THR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ss_orig_thr_reasons_e_type;
/*~ SENTINEL cm_ss_orig_thr_reasons_e_type.CM_SS_ORIG_THR_MAX */

/** Enumeration of Serving System events. Clients are able to
    register with the CM to be notified of any subsets of these
    serving system events.
*/
typedef enum cm_ss_event_e {

/** @cond
*/
  CM_SS_EVENT_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
 */

  /* 0 */
  CM_SS_EVENT_SRV_CHANGED,
    /**< Serving system information changed. */

  CM_SS_EVENT_RSSI,
    /**< RSSI changed. */

  CM_SS_EVENT_INFO,
    /**< Serving system information. This event is only sent to the client
         that requested this information through cm_ss_cmd_get_ss_info(). */

  /* Registration results - FEATURE_JCDMA only
  */
  CM_SS_EVENT_REG_SUCCESS,
    /**< Registration success. */

  CM_SS_EVENT_REG_FAILURE,
    /**< Registration failure. */

  /* 5 */
  CM_SS_EVENT_HDR_RSSI,
    /**< RSSI changed. */

  CM_SS_EVENT_WLAN_RSSI,
    /**< WLAN RSSI. Clients must check CM_API_WLAN
         for WLAN support before using this enum. */

  CM_SS_EVENT_SRV_NEW,
    /**< Serving system information changed. */

  CM_SS_EVENT_SECONDARY_MSM,
    /**< Invoke event callback even if the event is from a secondary MSM. The
         CM will not generate any events with this, but it is used by the thin
         client CM layer to call client callbacks for events from the secondary
         MSM. */

  CM_SS_EVENT_PS_DATA_AVAIL,
    /**< Received PS data available request. */

  /* 10 */
  CM_SS_EVENT_PS_DATA_FAIL,
    /**< Indicates fail to resume PS data. */

  CM_SS_EVENT_PS_DATA_SUCCESS,
    /**< Indicates success in resuming PS data. */

  CM_SS_EVENT_WLAN_STATS,
    /**< Indicates updates in WLAN statistics. */

  CM_SS_EVENT_ORIG_THR_TBL_UPDATE,
    /**< Indicates updates to the call origination throttle table. */

  CM_SS_EVENT_EMERG_NUM_LIST,
    /**< Indicates that a list of emergency numbers is available from the
         network. emerg_num_list in cm_mm_ss_info_s_type contains the numbers.
         CM_API_EMERGENCY_NUM_LIST must be checked before using this enum. */

  /* 15 */
  CM_SS_EVENT_IPAPP_REG_STATUS,
    /**< Indicates registration status of an IP application. Clients must
         check for CM_API_VIDEOSHARE before using this field. */

  CM_SS_EVENT_GW_RSSI,
    /**< RSSI for GW systems. */

  CM_SS_EVENT_HDR_LN_STATUS,
    /**< Location notification status event. */

  CM_SS_EVENT_SELECT_COUNTRY,
    /**< Event sent to prompt the user to select a country from a given
         list. The country_list field in cm_mm_ss_info_s_type contains
         the list of countries. Clients must check for
         CM_API_PLUS_DIALING before using this enum.

         The UI should also present an option called OTHER COUNTRY to the user,
         which can be selected if the user does not find any of the listed
         countries applicable. country_code can be set to
         CM_INVALID_MOBILE_COUNTRY_CODE when cm_ss_cmd_country_selected()
         is called. */

  CM_SS_EVENT_COUNTRY_SELECTED,
    /**< Event sent to inform clients of the user selected country. 
         current_country_code in cm_mm_ss_info_s_type contains the
         current country. CM_API_PLUS_DIALING must be checked before
         using this enum. */

  /* 20 */
  CM_SS_EVENT_REG_REJECT,
    /**< Event to send to indicate a registration failure. 
         Feature GSM or WCDMA only. */

  CM_SS_EVENT_MEAS_RESPONSE,
    /**< Event providing details regarding measurement response. Sent in
         response to the command CM_SS_CMD_GET_MEASUREMENT. Functionality is
         controlled by FEATURE_CM_SS_MEAS. */

  /* 22 */
  CM_SS_EVENT_WLAN_CONN_FAIL,
    /**< Event sent to inform clients that WLAN connection
         has failed. */

  CM_SS_EVENT_WLAN_SECURITY_INFO,
    /**< Event sent to inform clients about security info. */

  /* 24 */
  CM_SS_EVENT_CELL_ACCESS_IND,
    /**< Event providing details of Domain Specific Access
         Control (DSAC) values.This will give access barring
         status for CS and PS calls. */

  CM_SS_EVENT_RESEL_TARGET_RAT,
    /**< Event to notify client the target RAT when doing 
         eHRPD to LTE reselection */

/** @cond
*/
  CM_SS_EVENT_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ss_event_e_type;
/*~ SENTINEL cm_ss_event_e_type.CM_SS_EVENT_MAX */

/** Enumeration of Serving System measurement response status.
*/
typedef enum cm_ss_meas_status_e{

/** @cond
*/
  CM_SS_MEAS_STATUS_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_SS_MEAS_STATUS_OK,
    /**< Measurement was successful. */

  CM_SS_MEAS_STATUS_NO_SRV,
    /**< Measurement failed; no service. */

  CM_SS_MEAS_STATUS_BAR_PARM,
    /**< Measurement required; parameters incorrect. */

  /* 3 */
/** @cond
*/
  CM_SS_MEAS_STATUS_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ss_meas_status_e_type;
/*~ SENTINEL cm_ss_meas_status_e_type.CM_SS_MEAS_STATUS_MAX */


/** Enumeration of Serving System origination throttle action.
*/
typedef enum cm_ss_orig_thr_act_e{

  CM_SS_ORIG_THR_ACT_NONE = -1,
    /**< No action should be taken. */

  CM_SS_ORIG_THR_RESET_TBL,
    /**< Reset the original throttle table. */

  CM_SS_ORIG_THR_UPDATE_SS,
    /**< Update a particular original throttle entry. */

  CM_SS_ORIG_THR_UPDATE_CURR_SS,
    /**< Update the current original throttle entry. */

/** @cond
*/
  CM_SS_ORIG_THR_ACT_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ss_orig_thr_act_e_type;
/*~ SENTINEL cm_ss_orig_thr_act_e_type.CM_SS_ORIG_THR_ACT_MAX */

/** Enumeration of the Persist (PSIST) setting.
*/

typedef enum cm_psist_e{

/** @cond
*/
  CM_PSIST_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_PSIST_LOW,
    /**< Low PSIST. */

  CM_PSIST_HIGH,
    /**< High PSIST. */

  CM_PSIST_UNKNOWN,
    /**< Unknown PSIST. */

/** @cond
*/
  CM_PSIST_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_psist_e_type;
/*~ SENTINEL cm_psist_e_type.CM_PSIST_MAX */

/** @} */ /* end_addtogroup SERVSYSOBJ */

/** @addtogroup SERVSYSOBJ
    @{ */

/*---------------------------------------------------------------------------
** Datatypes
**-------------------------------------------------------------------------*/

/** @brief Structure to hold emergency numbers and types. Clients must check
    for CM_API_EMERGENCY_NUM_LIST before using this struct.
*/
typedef struct cm_emerg_num_s {

  uint32                    num_type;
    /**< Type of emergency number. Carries bits with
         positions that are defined by CM_EMERG_NUM_TYPE_*. */

  cm_num_s_type             num;
    /**< Digits in the number. */

} cm_emerg_num_s_type;

/** @brief Structure to hold a list of emergency numbers. Clients must check 
    for CM_API_EMERGENCY_NUM_LIST before using this struct.
*/
typedef struct cm_emerg_num_list_s {

  int                       num_list_len;
    /**< Count of emergency numbers present. */

  cm_emerg_num_s_type       num_list [CM_MAX_EMERGENCY_NUM_COUNT];
    /**< Emergency number types and digits. */

} cm_emerg_num_list_s_type;
 
/*~ FIELD cm_emerg_num_list_s.num_list VARRAY CM_MAX_EMERGENCY_NUM_COUNT
    LENGTH cm_emerg_num_list_s.num_list_len */

/** @brief Structure to hold Channel Quality Indication (CQI) information.
*/
typedef struct cm_cqi_info_s
{
  byte      gw_cqi;
  /**< Value of CQI; range = 0 to 30. */

  uint16    num_cqi_samples;
  /**< Total number of CQI samples; range = 1000 to 2500. */

}cm_cqi_info_s_type;

/** @brief Structure to hold information about services in the current
    cell. Clients must check for CM_API_CELL_SUPP_SERVICES and
    CM_API_CELL_SUPP_SERVICES_HS_CALL before using this structure.
*/
typedef struct cm_cell_srv_ind_s {

  sys_hs_ind_e_type         hs_ind;
    /**< High Speed service indication (over WCDMA). */

  sys_dtm_support_e_type    dtm_supp;
    /**< Dual Transfer Mode indication (over GSM). */

  sys_egprs_support_e_type  egprs_supp;
    /**< Enhanced General Packet Radio Service (EGPRS) indication. */

  sys_hs_ind_e_type         hs_call_status;
    /**< Reusing Handset Services enumerations to indicate call status on HS:
         - SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL -- No calls in progress on HS.
         - SYS_HS_IND_HSDPA_SUPP_CELL -- Call in progress on HSDPA.
         - SYS_HS_IND_HSUPA_SUPP_CELL -- Call in progress on HSUPA.
         - SYS_HS_IND_HSDPAPLUS_SUPP_CELL -- Call in progress on HSDPAPLUS.
         - SYS_HS_IND_HSDPAPLUS_HSUPA_SUPP_CELL -- Call in progress on HSDPA_PLUS 
                                                   and HSUPA supplementary cell. */

} cm_cell_srv_ind_s_type;

/** @brief Structure for registration rejection information.
*/
typedef struct cm_reg_reject_info_s {

  sys_srv_domain_e_type        reject_srv_domain;
    /**<   Service domain in which the registration is rejected. */

  byte                         reject_cause;
    /**<  Registration rejection cause. */
    
  sys_nas_procedure_e_type     nas_procedure;
    /**<  NAS procedure where reject happened */

} cm_reg_reject_info_s_type;

/** @brief Type to hold Serving System information for GSM or WCDMA modes.
*/
typedef struct cm_ss_gw_info_s {

  uint8                                   bit_err_rate;
    /**< Bit error rate/RX QUAL in GSM. */

  sys_sim_state_e_type                    sim_state;
    /**< Current state of the SIM. Mask used is CM_SS_SIM_STATE_MASK. */

  boolean                                 plmn_forbidden;
    /**< Indicates whether the PLMN is forbidden. Mask used is
         CM_SS_PLMN_FORBIDDEN_MASK. */

  sys_mm_information_s_type                mm_information;
    /**< Mobility management information (i.e., network name, date,
        time, and time zone of registered network). */

} cm_ss_gw_info_s_type;


/** @brief Type to hold Serving System information for CDMA modes. Applicable 
    for FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_ss_cdma_info_s {

#ifndef FEATURE_GNSS_SA
  sys_band_class_e_type        band_class;
    /**< Cellular or Personal Communications Services (PCS). */
#endif
  sys_blksys_e_type            block_or_system;
    /**< PCS frequency block: A to F cellular system A or B. */

  uint16                       reg_zone;
    /**< Registration zone (12-bit). */

  uint16                       packet_zone;
    /**< Packet zone (8-bit): 0xffff means PACKET ZONE NONE. Mask used is
         CM_SS_PACKET_ZONE_MASK. */

  uint8                        bs_p_rev;
    /**< Base station p_rev (8-bit). Mask used is CM_SS_BS_P_REV_MASK. */

  uint8                        p_rev_in_use;
    /**< p_rev in use (8-bit); the lesser of BS p_rev and mobile p_rev. Mask
         used is CM_SS_P_REV_IN_USE_MASK. */

  boolean                      is_registered;
    /**< Indicates whether the mobile is registered on the current system. */

  boolean                      ccs_supported;
    /**< Indicates whether Concurrent Services are supported. */

  int32                        uz_id;
     /**< User zone identification of the selected user zone
          (FEATURE_USERZONE). == CM_INVALID_UZID if no user zone is
          selected. Mask used is CM_SS_UZ_CHANGED_MASK. */

  sys_puzl_uz_name_s_type      uz_name;
     /**< User zone name of the selected user zone (FEATURE_USERZONE). Valid
          only if uz_id = CM_INVALID_UZID. Mask used is CM_SS_UZ_CHANGED_MASK. */

  sys_srch_win_type            srch_win_n;
    /**< Neighbor set window search size. Mask used is
         CM_SS_BASE_STATION_PARMS_CHGD_MASK. */

  sys_base_lat_type            base_lat;
    /**< Base station latitude (0.25 sec and already sign extended). Mask used
         is CM_SS_BASE_STATION_PARMS_CHGD_MASK. */

  sys_base_long_type           base_long;
    /**< Base station longitude (0.25 sec and already sign extended). Mask used
         is CM_SS_BASE_STATION_PARMS_CHGD_MASK. */

  sys_base_id_type             base_id;
    /**< Base station identification number. Mask used is
         CM_SS_BASE_STATION_PARMS_CHGD_MASK. */

  sys_time_info_s_type         time_info;
    /**< Time information obtained from the CDMA/HDR OTA messages. Mask used is
         CM_SS_CDMA_TIME_CHGD_MASK. If time_info.sys_mode is
         SYS_SYS_MODE_NO_SRV, it means that valid time information is currently
         unavailable. */

} cm_ss_cdma_info_s_type;


/** @brief SS information.
*/
typedef union cm_ss_info_u {

  cm_ss_gw_info_s_type     gw_info;
    /**< GSM/WCDMA SS information. */

  cm_ss_cdma_info_s_type   cdma_info;
    /**< CDMA SS information. */

} cm_ss_info_u_type;
 
/*~ IF (_DISC_ == SYS_SYS_MODE_GSM || _DISC_ == SYS_SYS_MODE_WCDMA)
       cm_ss_info_u.gw_info */
/*~ DEFAULT cm_ss_info_u.cdma_info */
 

/** @brief HDR-specific measurement response information.
*/
typedef struct cm_ss_hdr_resp_s{

  sys_pilot_pn_type        ref_pilot_pn;
    /**< Reference pilot Pseudorandom Noise (PN). */

  uint8                    ref_pilot_strength;
    /**< Reference pilot PN strength. */

  sys_channel_type         ref_pilot_chan;
    /**< Reference pilot band class and channel number. */

  uint8                    num_pilots; 
    /**< Number of pilots for which measurement is returned. */

  sys_pilot_info_s_type    pilot_info [ CM_MAX_DO_NEIGHBOR_CNT ];
    /**< Pilot measurements. */

  cm_ss_meas_status_e_type  meas_status;
    /**< The status of the measurement request. Can convey
         information about failure scenarios. */

}cm_ss_hdr_resp_s_type;
/*~ FIELD cm_ss_hdr_resp_s.pilot_info VARRAY CM_MAX_DO_NEIGHBOR_CNT
    LENGTH cm_ss_hdr_resp_s.num_pilots */

/** @brief Union of technology-specific measurement response
    information.
*/
typedef union cm_ss_meas_resp_info_u {

  cm_ss_hdr_resp_s_type   hdr_resp;
    /**< HDR system information. */

} cm_ss_meas_resp_info_u_type;
/*~ IF (_DISC_ == SYS_SYS_MODE_HDR ) cm_ss_meas_resp_info_u.hdr_resp */
/*~ DEFAULT cm_ss_meas_resp_info_u.void */

/** @brief Response to measurement request.
*/
typedef struct cm_ss_meas_resp_s {

  sys_sys_mode_e_type            sys_mode;
    /**< Mode of the system for which a measurement
         response is being given. */

  cm_ss_meas_resp_info_u_type    info;
    /**< Technology-specific information that is 
         conveyed by the underlying protocol. */

}cm_ss_meas_resp_s_type; 
/*~ FIELD cm_ss_meas_resp_s.info DISC cm_ss_meas_resp_s.sys_mode */

/** @brief HDR measurement request information.
*/
typedef struct cm_ss_hdr_info_s{

  boolean                  srch_win_size_inc;
    /**< Indicates whether the search window size is included. */

  uint8                    srch_win_size;
    /**< Search window size. Valid only if srch_win_size_inc = TRUE. */

  sys_channel_type         channel;
    /**< HDR channel information. */

  uint8                    neighbor_count;
    /**< Number of neighbors for which a measurement is requested. */

  sys_pilot_pn_type        neighbor_pilot_pn [ CM_MAX_DO_NEIGHBOR_CNT ];
    /**< Pilot PNs of neighbors for which a measurement is requested. */

}cm_ss_hdr_info_s_type;    
/*~ FIELD cm_ss_hdr_info_s.neighbor_pilot_pn VARRAY CM_MAX_DO_NEIGHBOR_CNT
    LENGTH cm_ss_hdr_info_s.neighbor_count */

/** @brief Technology-specific measurement request information.
*/
typedef union cm_ss_meas_request_info_u {

  cm_ss_hdr_info_s_type   hdr_info;
    /**< HDR system information.  */

} cm_ss_meas_request_info_u_type;
/*~ IF (_DISC_ == SYS_SYS_MODE_HDR ) cm_ss_meas_request_info_u.hdr_info */
/*~ DEFAULT cm_ss_meas_request_info_u.void */

/** @brief Measurement request information.
*/
typedef struct cm_ss_meas_req_s {

  sys_sys_mode_e_type            sys_mode;
    /**< The mode of the system for which a measurement is requested. */

  cm_ss_meas_request_info_u_type info;
    /**< The technology-specific information needed to make the measurement. */

}cm_ss_meas_req_s_type;    
/*~ FIELD cm_ss_meas_req_s.info DISC cm_ss_meas_req_s.sys_mode */

/** @brief Structure to hold throttling information.
*/
typedef struct cm_ss_orig_thr_info_type {

  boolean                       ss_thr_status;
    /**< Throttle status of the current SS. */

  dword                         thr_uptime;
    /**< Throttling time period. */

  unsigned int                  idx;
    /**< Index in the throttle_time table. Used internally by the CM to
         keep track of the next throttle time increment. */

  cm_ss_orig_thr_reasons_e_type call_status;
    /**< Call end reason. */

} cm_ss_orig_thr_info_s_type;

/** @brief Structure to hold a list of country codes with which to prompt users.
*/
typedef struct cm_ss_country_list_s {

  boolean                present;
    /**< Present flag if this structure is populated. */

  cm_sid_type            sid;
    /**< SID that is in conflict with countries. */

  uint8                  country_code_len;
    /**< Length of country_code_list[]. */

  cm_country_code_type   country_code_list [CM_COUNTRY_CODE_LIST_SIZE];
    /**< Array of mobile country codes that conflict with a given SID. */

} cm_ss_country_list_s_type;

/** @} */ /* end_addtogroup SERVSYSOBJ */


/* -------------------------------------------------------------------------*/
/** @cond
*/

/* This group is blocked out of the MSM7x30 documentation because the IP
   Multimedia functions are not used in the 7x30. */

/** @addtogroup IMS IP Multimedia Subsystem
    @{ */

/** Enumeration of IP application registration states. Clients
    must check for CM_API_VIDEOSHARE before using this type.
*/
typedef enum {

  CM_IPAPP_REG_STATUS_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */

  CM_IPAPP_REG_STATUS_REGISTERED,
    /**< The IP application has successfully registered with the network. */

  CM_IPAPP_REG_STATUS_DEREGISTERED,
    /**< The IP application has deregistered from the network. */

  CM_IPAPP_REG_STATUS_FAILURE,
    /**< The IP application has failed to register. */

  CM_IPAPP_REG_STATUS_MAX   /* FOR INTERNAL CM USE ONLY! */

} cm_ipapp_reg_status_e_type;
/*~ SENTINEL cm_ipapp_reg_status_e_type.CM_IPAPP_REG_STATUS_MAX */


/** @brief Information returned by the IP application on 
    registration/deregistration. Clients must check for CM_API_VIDEOSHARE 
    before using this type.
*/
typedef struct cm_ipapp_info_s {

  cm_ipapp_reg_status_e_type reg_status;
    /**< IP application registration status. */

  int                        cause;
    /**< Cause given when an IP application registration fails or deregisters. */

  cm_ipapp_warn_info_s_type  warn_info;
    /**< Warning information given by an IP application. */

  cm_call_type_mask_type     call_type_mask;
    /**< Call types that the IP application can service. */

  sys_sys_mode_mask_e_type   sys_mode;
    /**< System modes the IP application can support for calls. Can be used
         only if CM_API_SMS_OVER_IP is defined. */

} cm_ipapp_info_s_type;

/** @} */ /* end_addtogroup IMS IP Multimedia Subsystem */

/** @endcond
*/

/* ------------------------------------------------------------------------ */

/** @addtogroup SERVSYSOBJ
    @{ */

/** @brief Type to hold serving system information. When notified of serving 
    system events, clients are given a reference to such a structure.
*/
typedef struct cm_mm_ss_info_s {

  uint64                     changed_fields;
    /**< Bitmask indicating which of the following serving system fields
         were changed since the last notification. Refer to the define
         CM_SS_*_MASK. */

  uint64                     signal_strength_changed_fields;
    /**< Bitmask indicating which of the following signal strength fields
         were changed since the last notification. Refer to the define
         CM_SS_*_MASK. */

  uint64                     hybr_gw_changed_fields;
    /**< Bitmask indicating which of the following fields were changed          
         since the last notification.
         Refer to the definition CM_SS_GW_*_MASK.
         DSDS uses this mask under feature FEATURE_MMODE_DUAL_SIM.
         SVLTE uses this mask for LTE service info. */
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **                     MAIN SS INFO
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_modem_as_id_e_type     asubs_id;
    /**< Subscription ID corresponding to the Main GW service.\ Mask used is 
         CM_SS_SUBS_ID_MASK.\ This is only valid when the sys_mode is GSM/WCDMA. */

  boolean                    is_main_operational;
    /**< Operating status for a MAIN subscription.\ Decided by selected dual 
         standby preference. */

  sys_srv_status_e_type      srv_status;
    /**< Service status (SERVICE/NO SERVICE) of the system. Mask used is
         CM_SS_SRV_STATUS_MASK. */

  sys_srv_status_e_type      true_srv_status;
    /**< True service status (LIMITED/LIMITED_REGIONAL) of the
         system. Mask used is CM_SS_TRUE_SRV_STATUS_MASK. */

  sys_srv_domain_e_type      srv_domain;
    /**< Service domain registered on the system. Mask used is
         CM_SS_SRV_DOMAIN_MASK. */

  sys_srv_domain_e_type      srv_capability;
    /**< System's service capability. Mask used is
         CM_SS_SRV_CAPABILITY_MASK. */

  sys_sys_mode_e_type        sys_mode;
    /**< System's mode. Mask used is CM_SS_SYS_MODE_MASK. */

  cm_ss_info_u_type          mode_info;
    /**< Mode-specific Serving System information. */

  sys_roam_status_e_type     roam_status;
    /**< Current roaming status. Mask used is
         CM_SS_ROAM_STATUS_MASK. */

  sys_sys_id_s_type          sys_id;
    /**< ID of the selected system. Mask used is CM_SS_SYS_ID_MASK. */

  boolean                    is_sys_prl_match;
    /**< Indicates if the system is in PRL. Mask used is
         CM_SS_SYS_PRL_MATCH_MASK. */

  boolean                    is_sys_forbidden;
    /**< Indicates whether the system is forbidden. Mask used is
         CM_SS_SYS_FORBIDDEN_MASK. */

  boolean                   is_colloc;
    /**< Indicates whether the system is colocated. Mask used is
         CM_SS_SYS_ID_MASK. */

  uint16                     rssi;
    /**< RSSI in positive dBm. 75 means -75 dBm. The ranges are as follows:
         - AMPS:      -110 to -89 dBm.
         - 800 CDMA:  -105 to -90 dBm.
         - 1900 CDMA: -108 to -93 dBm.
         - GSM/WCDMA: -105 to -60 dBm. */

  int16                      ecio;
    /**< Ec/Io in negative 0.5 dBm. 63 means -31.5 dBm. */

  int                        io;
    /**< IO field is valid only when in HDR mode-only operation. */

  byte                       sinr;
    /**< Signal-to-Interface plus Noise Ratio (SINR)  SINR is applicable 
         when sys_mode = HDR or LTE

         For HDR SINR: range is from 0 to 8,where Level 8 represents 
                      the highest SINR.
         For LTE SINR: 10xdB values which are from -200 to 300 
                      are mapped to range 0 - 250. -20 db maps to 0, 
                      -19.8 db maps to 1, -19.6 maps to 2 ....
                      30 db maps to 250 */

  int16                      pathloss;
    /**< Pathloss is applicable only in connected mode (otherwise 0xFF). */

  int16                      sir;
    /**< SIR is applicable and valid (otherwise 0xFF) only in WCDMA
         connected mode when the Dedicated Physical Control Channel 
         (DPCCH) is active.  */

  int16                      rssi2;
    /**< RSSI -ve value, coming from the Radio Resource Control (RRC) layer. */

  int16                      rscp;
    /**< Range of Received Signal Code Power (RSCP) is
         from -28 to -121 dBm. */

  int16                      gw_ecio;
    /**< Ec/Io for WCDMA only. Ec/Io is measured in -0.5 dB. WCDMA L1 sends
         positive values in 0.5dB increments to RRC. For GSM, it is defaulted
         to CMSS_ECIO_VALUE_INVALID_SIGNAL. Clients must check for
         CM_API_GW_ECIO to use this field. */

  cm_cqi_info_s_type      cqi_info;
    /**< CQI information. Applicable for WCDMA only. */

  int16                      rsrp;  
    /**< Current Reference Signal Received Power (RSRP) in dBm as measured
         by L1. The range is -44 to -140 dBm. */
  
  int8                       rsrq;  
    /**< Current Reference Signal Receive Quality (RSRQ) as measured
         by L1. Quantities are in dB. The range is -20 to -3 dB. */

  sys_lac_type               lac;
    /**< LAC information for GSM. */

  sys_lac_type               tac;
    /**< TAC information for LTE. */

  uint8                      rac_or_mme_code;
    /**< RAC or Mobility Management Entity (MME) code for GW or LTE. */

  boolean                    ecio_for_display;
    /**< TRUE if User Equipment (UE) is in data call. */

  cm_psist_e_type            psist;
    /**< PSIST of the system. */

  boolean                    ps_data_suspend;
    /**< Indicates whether the current PS data service has been suspended. Mask
         used is CM_SS_PS_DATA_SUSPEND_MASK.  */

  boolean                    is_csfb_call_active;
   /**< Indicates whether service is acquired because of CSFB call. 
        Mask used is CM_SS_CSFB_CALL_ACTIVE_MASK.  */
  
#ifndef FEATURE_GNSS_SA
  sys_band_class_e_type       active_band;
    /**< Clients must check for CM_API_EMERGENCY_NUM_LIST before
         using this structure. */
#endif
  sys_channel_num_type        active_channel;
    /**< Report the active channel of the current serving system. */

  dword                      curr_uptime;
    /**< Current up time. */

  cm_emerg_num_list_s_type   emerg_num_list;
    /**< Emergency number list returned from the network. Clients must check
         for CM_API_EMERGENCY_NUM_LIST before using this structure. */

  cm_ss_orig_thr_info_s_type main_thr_status;
    /**< DCTM status; indicates the current throttle status of the MAIN
         system. */

  sys_cell_info_s_type      cell_info;
    /**< CELL_INFO of the selected system. Mask used is
         CM_SS_CELL_INFO_MASK. */

  sys_roam_status_e_type     def_roam_ind;
   /**< Default roaming indicator from the PRL. */

  cm_cell_srv_ind_s_type      cell_srv_ind;
    /**< Structure to hold information about services in the current cell. The
         information is tied to CM_SS_SRV_IND_MASK. Clients must check for
         CM_API_CELL_SUPP_SERVICES before using this structure. */

  cm_reg_reject_info_s_type reg_reject_info;
    /**< Registration rejection information.  */

  sys_srv_domain_e_type      cipher_domain;
    /**< Ciphering on CS or PS domain or both. Can be used only if
         CM_CIPHERING_IND_SUPP is defined. */

  uint16                     geo_sys_idx;
    /**< The system table index referencing the beginning of the geo in which 
    ** the current serving system is present.
    ** When sys idx is not known 0xFFFF is used
    */

  uint16                      cdma_reg_prd;
    /**< Indicates the registration period after the CDMA system is acquired 
    ** When CDMA registration period is not valid (-1) is sent
    */

  sys_cell_broadcast_info_s_type   cell_bc_info;
    /**< Cell broadcast capability info of main stack. */

  boolean                   bsr_in_progress;
    /**< Indicates if BSR is in progress or not. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **                     HDR SS INFO
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean                    hdr_hybrid;
    /**< Indicates whether the system is hybrid (FEATURE_HDR_HYBRID). */

  sys_srv_status_e_type      hdr_srv_status;
     /**< Indicates the current service state (FEATURE_HDR_HYBRID). Mask used
          is CM_SS_HDR_SRV_STATUS_MASK. */

  sys_roam_status_e_type     hdr_roam_status;
     /**< Indicates the current hybrid HDR roaming status (FEATURE_HDR_HYBRID).  
          Mask used is CM_SS_HDR_ROAM_STATUS_MASK. */

  uint16                     hdr_rssi;
    /**< HDR RSSI in positive dBm. 75 means -75dBm (FEATURE_HDR_HYBRID). 
         The range is -105 to -90 dBm. */

  int16                      hdr_ecio;
    /**< HDR Ec/Io values (used when mobile is in hybrid operation)
         (FEATURE_HDR). HDR Ec/Io is in negative 0.5 dBm i.e. 63 means -31.5dBm. */

  int                        hdr_io;
    /**< HDR IO values (used when mobile is in hybrid operation) (FEATURE_HDR).   
         IO range is from -106 dBm to -21 dBm. */

  byte                       hdr_sinr;
    /**< SINR range is from 0 to 8, where Level 8 represents the highest SINR. */

  sys_active_prot_e_type     hdr_active_prot;
    /**< HDR active protocol revision information. Mask used is
         CM_SS_HDR_ACTIVE_PROT_MASK. Must define CM_API_HDR_ACTIVE_PROT before
         accessing this field. */

  cm_ss_orig_thr_info_s_type hdr_thr_status;
    /**< DCTM status; indicates the current throttle status of the HDR
         system. */

  sys_sys_id_s_type          hdr_sys_id;
    /**< ID of the HDR subsystem. Mask used is CM_SS_HDR_SYS_ID_MASK. */
#ifndef FEATURE_GNSS_SA
  sys_band_class_e_type       hybrid_active_band;
    /**< Report the active band of the hybrid system. */
#endif
  sys_channel_num_type        hybrid_active_channel;
    /**< Report the active channel of the current serving system. */

  sys_hdr_location_notification_status_e_type hdr_ln_status;
    /**< HDR LN status. */

  sys_sys_mode_e_type        prev_idle_digital_mode;
    /**< Previous idle digital mode (FEATURE_HDR_HANDOFF). Mask used is
         CM_SS_IDLE_DIGITAL_MODE_MASK. */

  sys_sys_mode_e_type        cur_idle_digital_mode;
    /**< Current idle digital mode (FEATURE_HDR_HANDOFF). Mask used is
         CM_SS_IDLE_DIGITAL_MODE_MASK. */

  uint16                    hdr_packet_err_rate; 
    /**< Value range from 1 to 800. 0xFFFF is an invalid value. */

  sys_personality_e_type     hdr_personality;
    /**< HDR personality information. Mask used is CM_SS_HDR_PERSONALITY_MASK. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **                     BCMCS INFO
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* FEATURE_BCMCS
     These fields specify whether lower layer
     supports broadcast/multicast as per
     High Rate Broadcast-Multicast Packet data Air
     Interface Specification C.S0054 and
     BCMCS Framework Draft Document 0.1.3 XP0019.
  */

  boolean                    hybr_hdr_bcmcs_srv_supported;
    /**< Specifies whether HDR is configured to support BCMCS service. Mask
         used is CM_SS_HYBR_HDR_BCMCS_SRV_SUPPORTED_MASK. */

  sys_bcmcs_srv_status_e_type hybr_hdr_bcmcs_srv_status;
    /**< Specifies whether BCMCS service is available on HDR. Mask used is
         CM_SS_HYBR_HDR_BCMCS_SRV_STATUS_MASK. */

  boolean                    ss_main_bcmcs_srv_supported;
    /**< Specifies whether MAIN is configured to support BCMCS service. Mask
         used is CM_SS_MAIN_BCMCS_SRV_SUPPORTED_MASK. */

  sys_bcmcs_srv_status_e_type ss_main_bcmcs_srv_status;
    /**< Specifies whether BCMCS service is available on MAIN. Mask used is
         CM_SS_MAIN_BCMCS_SRV_STATUS_MASK. */

  cm_client_id_type          requesting_client_id;
    /**< Requesting client. */

  sys_msm_id_e_type          msm_id;
     /**< Identifies the MSM that generated this event. */

  /* Clients check CM_API_WLAN for WLAN support for following
  **
  ** wlan_rssi, wlan_srv_status, wlan_bss_info, wlan_stats
  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **                     WLAN SS INFO
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uint16                     wlan_rssi;
     /**< WLAN RSSI. Clients must check CM_API_WLAN for WLAN support. */

  sys_srv_status_e_type      wlan_srv_status;
     /**< Indicates the current WLAN service state. Mask used is 
          CM_SS_WLAN_SRV_STATUS_MASK. Clients must check CM_API_WLAN
          for WLAN support. */

  sys_wlan_bss_info_s_type   wlan_bss_info;
    /**< 802.11 (WLAN) System Information. Clients must check
         CM_API_WLAN for WLAN support. */

  sys_wlan_stats_s_type      wlan_stats;
    /**< WLAN statistics, such as current transmit rate, Tx/Rx bytes, etc.  
         Clients must check CM_API_WLAN for WLAN support. */

  cm_ipapp_info_s_type       ipapp_info;
    /**< Registration information of the application. Clients must check for
         CM_API_VIDEOSHARE before using this structure type. */

  sys_wlan_acquire_fail_reason_e_type  wlan_acq_fail_cause;
    /**< Acq fail reason for WLAN. */
  
  sys_wlan_security_info_s_type wlan_sec_info;
    /**< WLAN security info. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **                     HYBRID GW SS INFO
  **                    (use for GWL service info)
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean                    is_hybr_gw_operational;
    /**< Operating status for HYBR_GW subscription.
         (DSDS)Decided by the selected dual standby preference.
         (SVLTE)Decided by the RF configuration. */
  
  sys_modem_as_id_e_type     gw_asubs_id;
    /**< Subscription ID corresponding to the hybrid GW service.\  
         Mask used is CM_SS_GW_SUBS_ID_MASK. 
         This is only valid when the sys_mode is GSM/WCDMA. */
#ifndef FEATURE_GNSS_SA
  sys_band_class_e_type      gw_active_band;
    /**< Clients must check for CM_API_EMERGENCY_NUM_LIST before
         using this structure. */
#endif
  sys_channel_num_type       gw_active_channel;
    /**< Report the active channel of the current serving system. */

  sys_srv_status_e_type      gw_srv_status;
    /**< Service status (SERVICE/NO SERVICE) of the system. Mask used is
         CM_SS_GW_SRV_STATUS_MASK. */

  sys_srv_status_e_type      true_gw_srv_status;
    /**< True service status (LIMITED/LIMITED_REGIONAL) of the system. Mask
         used is CM_SS_TRUE_GW_SRV_STATUS_MASK. */

  sys_srv_domain_e_type      gw_srv_domain;
    /**< Service domain registered on the system. Mask used is
         CM_SS_GW_SRV_DOMAIN_MASK. */

  sys_srv_domain_e_type      gw_srv_capability;
    /**< System's service capability. Mask used is
         CM_SS_GW_SRV_CAPABILITY_MASK. */

  sys_sys_mode_e_type        gw_sys_mode;
    /**< System's mode. Mask used is CM_SS_GW_SYS_MODE_MASK. */

  sys_roam_status_e_type     gw_roam_status;
    /**< Current roaming status. Mask used is CM_SS_GW_ROAM_STATUS_MASK. */

  sys_sys_id_s_type          gw_sys_id;
    /**< ID of the selected system. Mask used is CM_GW_SS_SYS_ID_MASK. */

  boolean                    gw_is_sys_prl_match;
    /**< Indicates if the system is in PRL.\ 
         Mask used is CM_SS_GW_SYS_PRL_MATCH_MASK. */

  boolean                    gw_is_sys_forbidden;
    /**< Indicates whether the system is forbidden. Mask used is
         CM_SS_GW_SYS_FORBIDDEN_MASK. */

  uint16                     gw_rssi;
    /**< RSSI in positive dBm. 75 means -75dBm. The ranges are as follows.
         - AMPS:      -110 to -89 dBm.
         - 800 CDMA:  -105 to -90 dBm.
         - 1900 CDMA: -108 to -93 dBm.
         - GSM/WCDMA: -105 to -60 dBm. */

  int                        gw_io;
    /**< IO field is only valid when in HDR mode only operation. */

  byte                       gw_sinr;
    /**< SINR range is from 0 to 8, where Level 8 represents the highest SINR. 
         The SINR is applicable only when sys_mode = HDR. */

  int16                      gw_pathloss;
    /**< Pathloss is applicable only in connected mode (otherwise 0xFF). */

  cm_cqi_info_s_type         gw_cqi_info;
    /**< CQI information. Applicable for WCDMA only.
    */

  int16                      gw_sir;
    /**< SIR applicable and valid (otherwise 0xFF) only in WCDMA
         connected mode when DPCCH is active. */

  int16                      gw_rssi2;
    /**< RSSI -ve value, from the Radio Resource Control (RRC) layer. */

  int16                      gw_rscp;
    /**< Signal strength in RSSI; change for WCDMA only (otherwise 0xFF). */

  boolean                    gw_ecio_for_display;
    /**< TRUE if UE is in a data call; otherwise FALSE. */

  sys_cell_info_s_type       gw_cell_info;
    /**< CELL_INFO of the selected system. Mask used is
         CM_SS_GW_CELL_INFO_MASK. */

  int16                      gw2_ecio;
    /**< ECIO for WCDMA only.\ EC/IO is measured in multiples of -0.5 dB.\ 
         What WCDMA L1 sends to RRC is in 0.5 dB increments (positive value).
         For GSM, it defaults to CMSS_ECIO_VALUE_INVALID_SIGNAL.
         Clients should check for CM_API_GW_ECIO to use this field. */

  cm_ss_info_u_type          gw_mode_info;
    /**< Mode-specific serving system information. */

  cm_cell_srv_ind_s_type     gw_cell_srv_ind;
    /**< Structure to hold information about services in the current cell. 
         The information is tied to CM_SS_GW_SRV_IND_MASK.
         Clients must check for CM_API_CELL_SUPP_SERVICES before
         using this structure. */

  boolean                    gw_ps_data_suspend;
    /**< Indicates that the current PS data service has been suspended. 
         Mask used is CM_SS_GW_PS_DATA_SUSPEND_MASK. */

  cm_reg_reject_info_s_type  gw_reg_reject_info;
    /**< Registration rejection information. */

  sys_srv_domain_e_type      gw_cipher_domain;
    /**< Ciphering on CS or PS domain or both.\ 
         Can be used only if CM_CIPHERING_IND_SUPP is defined. */

  cm_emerg_num_list_s_type   gw_emerg_num_list;
    /**< Emergency number list returned from the network. 
         Clients must check for CM_API_EMERGENCY_NUM_LIST before
         using this structure. */

  sys_simultaneous_mode_e_type simultaneous_mode;
    /**< Simultaneous; used to differentiate between WX and non-WX modes. Mask
         used is CM_SS_SRV_IND_MASK. */

  sys_cell_broadcast_info_s_type     gw_cell_bc_info;
      /**< Cell broadcast capability info of hybrid stack. */
    
  int16                      gwl_rsrp;  
    /**< Current Reference Signal Received Power (RSRP) in dBm as measured
         by L1. The range is -44 to -140 dBm. */
  
  int8                       gwl_rsrq;  
    /**< Current Reference Signal Receive Quality (RSRQ) as measured
         by L1. Quantities are in dB. The range is -20 to -3 dB. */

  sys_lac_type               gwl_lac;
    /**< LAC information for GSM. */
  
  sys_lac_type               gwl_tac;
    /**< TAC information for LTE. */
  
  uint8                      gwl_rac_or_mme_code;
    /**< RAC or Mobility Management Entity (MME) code for GW or LTE. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **                     HPCD INFO
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_ss_country_list_s_type   country_list;
    /**< Structure populated when sending CM_SS_EVENT_SELECT_COUNTRY. Clients
         must check for CM_API_PLUS_DIALING before using this structure. */

  /* cm_plus_code*/

  cm_country_code_type        current_mcc;
    /**< Carries the country code of the current serving country. Associated
         with CM_SS_SRV_STATUS_MASK. Clients must check for CM_API_PLUS_DIALING
         before using this structure. */

  cm_country_code_type        home_mcc;
    /**< Carries the home country code. Associated with
         CM_SS_SRV_STATUS_MASK. Clients must check for
         CM_API_PLUS_DIALING before using this structure. */

  cm_hs_based_plus_dial_e_type  plus_dial_setting;
    /**< Carries the NV setting corresponding to plus dialing. Associated
         with CM_SS_SRV_STATUS_MASK. Clients must check for CM_API_PLUS_DIALING
         before using this structure. */

  cm_gw_gan_info_s_type       gan_info;
   /**< GAN controller information. */

  uint16                      frame_err_rate; 
    /**< Value range from 1 to 800. 0xFFFF is an invalid value. */

  cm_ss_meas_resp_s_type      meas_resp;
    /**< Indicates the response to the measurement request made by the
         connection manager. */
         
  sys_domain_access_bar_s_type  cell_access_info;
    /**< Cell access information */

  boolean                     voice_support_on_lte;
    /**< Indicates the voice support status on LTE. */

  sys_sys_mode_e_type         target_rat;
    /**< Indicate the target RAT during DO to LTE reselection */

  boolean                       lte_ims_voice_avail;
    /**< Indicate the IMS voice support status on LTE */

} cm_mm_ss_info_s_type;

/*~ FIELD cm_mm_ss_info_s.mode_info DISC cm_mm_ss_info_s.sys_mode */
/*~ FIELD cm_mm_ss_info_s.gw_mode_info DISC cm_mm_ss_info_s.gw_sys_mode */

/** Type for serving system command callback functions. A
    client that wants to be notified of the result of a serving
    system command function must supply a pointer to a function
    of this type as the first parameter to a serving system
    command function.
*/
typedef void (cm_ss_cmd_cb_f_type) (

  void                    *data_block_ptr,
    /**< Pointer to the client-provided data block. */

  cm_ss_cmd_e_type        ss_cmd,
    /**< Indicates which serving system command the ss_cmd_err 
         error status is for. */

  cm_ss_cmd_err_e_type    ss_cmd_err
    /**< Serving system command error code indicating if the 
         command is rejected and the reason for the rejection. */
);

/** @} */ /* end_addtogroup SERVSYSOBJ */

/** @addtogroup SERVSYSOBJ
    @{ */

/*===========================================================================

FUNCTION: cm_ss_cmd_get_ss_info

===========================================================================*/
/**
  Gets a snapshot of the current serving system information in
  the CM. The client must register for CM_SS_EVENT_INFO in
  order to receive the response to this command.
  CM_SS_EVENT_INFO is only sent to the client that issues this
  command.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_ss_cmd_get_ss_info(

  cm_ss_cmd_cb_f_type  cmd_cb_func,
  void                 *data_block_ptr,
  cm_client_id_type    client_id

);
 
/*~ FUNCTION cm_ss_cmd_get_ss_info */

/*===========================================================================

FUNCTION: cm_ss_cmd_get_valid_cell_info

===========================================================================*/
/**
  Gets the CELL_INFO(Cell_ID, PLMN ID) when the UE is in DCH state.
  The client must register for CM_SS_EVENT_SRV_CHANGED in order to receive the 
  response to this command. This event is sent to all clients that register
  for this event.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
boolean cm_ss_cmd_get_dch_cell_info ( 
  cm_ss_cmd_cb_f_type     cmd_cb_func,
  /* client callback function */
                    
  void                    *data_block_ptr,
  /* pointer to client callback data block */
                          
  cm_client_id_type       client
  /* requesting client ID */
);
/*~ FUNCTION cm_ss_cmd_get_valid_cell_info */

/*===========================================================================

FUNCTION: cm_ss_cmd_get_measurement

===========================================================================*/
/**
  Gets the measurement results for systems specified in the
  payload of this command. The client must register for
  CM_SS_EVENT_MEAS_RESPONSE in order to receive the response to
  this command. This event is sent to all clients that register
  for this event.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.
@param[in] meas_req       Measurement request information.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_ss_cmd_get_measurement(

  cm_ss_cmd_cb_f_type      cmd_cb_func,
  void                     *data_block_ptr,
  cm_client_id_type        client_id,
  cm_ss_meas_req_s_type    meas_req

);
/*~ FUNCTION cm_ss_cmd_get_measurement */


/*===========================================================================

FUNCTION: cm_mm_ss_cmd_ps_data_available

===========================================================================*/
/**
  Informs the CM that data is available for transmission.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_GSM or FEATURE_WCDMA must be defined.
*/
extern boolean cm_mm_ss_cmd_ps_data_available(

  cm_ss_cmd_cb_f_type          cmd_cb_func,
  void                         *data_block_ptr,
  cm_client_id_type            client_id

);
 
/*~ FUNCTION cm_mm_ss_cmd_ps_data_available */

/*===========================================================================

FUNCTION: cm_ss_cmd_orig_thr_update_tbl

===========================================================================*/
/**
  Queues a command to the CM to update the origination
  throttle table. This function is only for CDMA and HDR. Call
  origination is throttled with the help of the throttle table,
  which this function updates.

@param[in] ss_cmd_cb_func Pointer to the callback function.
@param[in] data_block_ptr Pointer to user data.
@param[in] client         Requesting client ID.
@param[in] orig_thr_act   Action to be taken.
@param[in] call_status    Call end reason.
@param[in] call_type      Type of call (e.g., PS_DATA).
@param[in] mode           Mode in which the call was connected.
@param[in] sys_id         ID of the system; SID, NID. This field is only used if
                          orig_thr_act = CM_SS_ORIG_THR_UPDATE_TBL_ENTRY.
@param[in] band_class     Cellular or PCS. This field is only used if
                          orig_thr_act = CM_SS_ORIG_THR_UPDATE_TBL_ENTRY.
@param[in] packet_zone    Packet zone ID. This field is only used if
                          orig_thr_act = CM_SS_ORIG_THR_UPDATE_SS.

@return 
  TRUE -- Command was queued. \n
  FALSE -- Command was not queued.

@dependencies
  cmss_init() must have been called previously.
*/
boolean cm_ss_cmd_orig_thr_update_tbl
(
  cm_ss_cmd_cb_f_type                *ss_cmd_cb_func,
  void                               *data_block_ptr,
  cm_client_id_type                  client,
  cm_ss_orig_thr_act_e_type          orig_thr_act,
  cm_ss_orig_thr_reasons_e_type      call_status,
  cm_call_type_e_type                call_type,
  sys_sys_mode_e_type                mode,
  const sys_sys_id_s_type            *sys_id,
  #ifndef FEATURE_GNSS_SA
  sys_band_class_e_type              band_class,
  #endif
  uint16                             packet_zone

);
 
/*~ FUNCTION cm_ss_cmd_orig_thr_update_tbl */
/*~ PARAM ss_cmd_cb_func POINTER */
/*~ PARAM sys_id POINTER */

/*===========================================================================

FUNCTION: cm_ss_cmd_country_selected

===========================================================================*/
/**
  Carries the country code selected by the user in response to
  CM_SS_EVENT_SELECT_COUNTRY.

@param[in] cmd_cb_func  Client callback function.
@param[in] client_id    Requesting client ID.
@param[in] sid          SID for which the country is being resolved.
@param[in] country_code Code of the country that has been selected.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_GSM or FEATURE_WCDMA must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.  
*/
extern boolean cm_ss_cmd_country_selected (

  cm_ss_cmd_cb_f_type          cmd_cb_func,
  cm_client_id_type            client_id,
  cm_sid_type                  sid,
  cm_country_code_type         country_code

);
//MMODE: SS Obj end

 
/*~ FUNCTION cm_ss_cmd_country_selected */

/** @} */ /* end_addtogroup SERVSYSOBJ */

/** @addtogroup IO Inband/Outband Object
    @{ */

/*---------------------------------------------------------------------------
** Defines
**-------------------------------------------------------------------------*/

#define CM_INBAND_MAX_FWD_DIALED_DIGITS 64
  /**< Maximum length of the forward DTMF digit string. */

#define CM_INBAND_MAX_REV_DIALED_DIGITS_JCDMA 255  
  /**< Value of CAI_SBDTMF_MAX. */

#define CM_INBAND_MAX_DIALED_DIGITS_JCDMA  \
                             CM_INBAND_MAX_REV_DIALED_DIGITS_JCDMA
  /**< Maximum reverse dialed digits JCDMA. */

#define CM_INBAND_MAX_REV_DIALED_DIGITS 32
  /**< Maximum length of the reverse DTMF digit string. */

#define CM_INBAND_MAX_DIALED_DIGITS CM_INBAND_MAX_FWD_DIALED_DIGITS
  /**< Maximum forward dialed digits. */

/** @} */ /* end_addtogroup IO */

/** @addtogroup IO Inband/Outband Object
    @{ */

/*---------------------------------------------------------------------------
** Enums
**-------------------------------------------------------------------------*/

/** Enumeration of inband commands clients are able to send to the CM.
*/
typedef enum cm_inband_cmd_e {

/** @cond
*/
  CM_INBAND_CMD_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_INBAND_CMD_BURST_DTMF,
    /**< Request to send a burst DTMF. */

  CM_INBAND_CMD_START_CONT_DTMF,
    /**< Request to start a continuous DTMF tone. */

  CM_INBAND_CMD_STOP_CONT_DTMF,
    /**< Request to stop a continuous DTMF tone. */

/** @cond
*/
  CM_INBAND_CMD_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_inband_cmd_e_type;
/*~ SENTINEL cm_inband_cmd_e_type.CM_INBAND_CMD_MAX */

/** Enumeration of inband command errors returned by call
    command callback functions.
*/
typedef enum cm_inband_cmd_err_e {

/** @cond
*/
  CM_INBAND_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_INBAND_CMD_ERR_NOERR,
    /**< No errors found. */

  /* Parameter-related errors */

  CM_INBAND_CMD_ERR_CLIENT_ID_P,
    /**< Invalid client ID. */

  CM_INBAND_CMD_ERR_ON_LEN_P,
    /**< Invalid DTMF pulse width. */

  CM_INBAND_CMD_ERR_OFF_LEN_P,
    /**< Invalid DTMF inter-digit interval. */

  /* 4 */
  CM_INBAND_CMD_ERR_NULL_BUF_P,
    /**< Null digit buffer. */

  CM_INBAND_CMD_ERR_DIGIT_P,
    /**< Invalid digits. */

  CM_INBAND_CMD_ERR_CNT_P,
    /**< The count exceeds the maximum allowed value. */

  CM_INBAND_CMD_ERR_CALL_ID_P,
    /**< Invalid call ID. */

  /* Error codes resulting from clients trying to instruct the
     CM to perform inappropriate actions relative to the current
     state of the phone. */

  /* 8 */
  CM_INBAND_CMD_ERR_BAD_STATE_S,
    /**< Invalid state for inband request. */

  CM_INBAND_CMD_ERR_OFFLINE_S,
    /**< Phone is offline. */

  /* Error codes resulting from operational limitations
     of the CM. */

  CM_INBAND_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  /* Other errors */

  CM_INBAND_CMD_ERR_OTHER,
    /**< A problem other than the above was found. */

  /* 12 */
  CM_INBAND_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to a relevant feature being undefined. */

/** @cond
*/
  CM_INBAND_CMD_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_inband_cmd_err_e_type;
/*~ SENTINEL cm_inband_cmd_err_e_type.CM_INBAND_CMD_ERR_MAX */

/** Enumeration of inband events. Clients are able to register
    with the CM to be notified of any subsets of these inband
    events.
*/
typedef enum cm_inband_event_e {

/** @cond
*/
  CM_INBAND_EVENT_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_INBAND_EVENT_REV_BURST_DTMF,
    /**< Send a CDMA-burst DTMF. */

  CM_INBAND_EVENT_REV_START_CONT_DTMF,
    /**< Start a continuous DTMF tone. */

  CM_INBAND_EVENT_REV_START_CONT_DTMF_CONF,
    /**<  Confirm that a continuous DTMF tone is played (WCDMA/GSM only). */

  CM_INBAND_EVENT_REV_STOP_CONT_DTMF,
    /**< Stop a continuous DTMF tone. */

  /* 4 */
  CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF,
    /**< Confirm a that continuous DTMF tone is stopped (WCDMA/GSM only). */

  CM_INBAND_EVENT_FWD_BURST_DTMF,
    /**< Received a CDMA-burst DTMF message. */

  CM_INBAND_EVENT_FWD_START_CONT_DTMF,
    /**< Received a start continuous DTMF tone order. */

  CM_INBAND_EVENT_FWD_STOP_CONT_DTMF,
    /**< Received a stop continuous DTMF tone order. */

  /* 8 */
  CM_INBAND_EVENT_REV_BURST_DTMF_ACK,
    /**< Received an L2 ACK for a CDMA-burst DTMF message (FEATURE_T53). */

/** @cond
*/
  CM_INBAND_EVENT_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_inband_event_e_type;
/*~ SENTINEL cm_inband_event_e_type.CM_INBAND_EVENT_MAX */

/** @} */ /* end_addtogroup IO */

/** @addtogroup IO Inband/Outband Object
    @{ */

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** @brief Type to hold inband information. When notified of inband
    events, clients are given a reference to such a structure.
*/
typedef struct cm_inband_info_s {

  cm_client_id_type       client_id;
    /**< Request client ID for reverse DTMF requests. */

  cm_call_id_type         call_id;
    /**< Call ID. */

  boolean                 success;
    /**< Whether a continuous DTMF is played (GSM/WCDMA). */

  uint8                   on_length;
    /**< DTMF pulse width. */

  uint8                   off_length;
    /**< DTMF inter-digit interval. */

  uint8                   cnt;
    /**< Digit count. */

  uint8                   digits[CM_INBAND_MAX_DIALED_DIGITS_JCDMA];
    /**< DTMF digit buffer. */

  uint16                  duration;
    /**< Duration for continuous DTMF in milliseconds (for reverse stop
         continuous DTMF event only). */

  cm_client_id_type       requesting_client_id;
    /**< Requesting client. */

} cm_inband_info_s_type;
 
/*~ FIELD cm_inband_info_s.digits VARRAY CM_INBAND_MAX_DIALED_DIGITS_JCDMA
    LENGTH cm_inband_info_s.cnt */

/** Type for inband command callback functions. A client that
    wants to be notified of the result of an inband command
    function must supply a pointer to a function of this type as
    the first parameter to an inband command function.
*/
typedef void (cm_inband_cmd_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to a client-provided data block. */

  cm_inband_cmd_e_type        inband_cmd,
    /**< Indicates which inband command this error status is for. */

  cm_inband_cmd_err_e_type    inband_cmd_err
    /**< Inband command error code, indicating if the command
         is rejected and the reason for rejection. */
);

/** @} */ /* end_addtogroup IO */

/** @addtogroup IO Inband/Outband Object
    @{ */

/*===========================================================================

FUNCTION: cm_mm_inband_cmd_burst_dtmf

===========================================================================*/
/**
  Commands the CM to send a burst DTMF.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] call_id        Call ID.
@param[in] on_length      DTMF pulse width.
@param[in] off_length     DTMF inter-digit interval.
@param[in] cnt            DTMF digit count.
@param[in] dtmf_digit_ptr Pointer to the DTMF digit buffer.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_CDMA_800 or FEATURE_CDMA_1900 must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_inband_cmd_burst_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type           client_id,
  cm_call_id_type             call_id,
  uint8                       on_length,
  uint8                       off_length,
  uint8                       cnt,
  const uint8                 *dtmf_digit_ptr

);
 
/*~ FUNCTION cm_mm_inband_cmd_burst_dtmf */
/*~ PARAM dtmf_digit_ptr VARRAY LENGTH cnt */

/*===========================================================================

FUNCTION: cm_mm_inband_cmd_start_cont_dtmf

===========================================================================*/
/**
  Commands the CM to start a continuous DTMF.
  
  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] call_id        Call ID.
@param[in] digit          DTMF digit.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start().

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_inband_cmd_start_cont_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type           client_id,
  cm_call_id_type             call_id,
  uint8                       digit

);
 
/*~ FUNCTION cm_mm_inband_cmd_start_cont_dtmf */

/*===========================================================================

FUNCTION: cm_mm_inband_cmd_stop_cont_dtmf

===========================================================================*/
/**
  Commands the CM to stop a continuous DTMF.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client.
@param[in] call_id        Call ID.
@param[in] duration       DTMF duration.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start().

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_inband_cmd_stop_cont_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
  void                        *data_block_ptr,
  cm_client_id_type           client_id,
  cm_call_id_type             call_id,
  uint16                      duration

);
/*~ FUNCTION cm_mm_inband_cmd_stop_cont_dtmf */


/** @} */ /* end_addtogroup IO Inband/Outband Object */

/*-----------------------------------------------------------------------------

Short Messaging Service Object
** For internal use (uasms) only **

-----------------------------------------------------------------------------*/

/** @addtogroup SMSO Short Messaging Service Object
    @{ */

/*---------------------------------------------------------------------------
** Enums
**-------------------------------------------------------------------------*/

/** Enumeration of SMS commands.
*/
typedef enum cm_sms_cmd_e {

/** @cond
*/
  CM_SMS_CMD_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_SMS_CMD_MO_MSG,
    /**< MO SMS (FEATURE_CDSMS). */

  CM_SMS_CMD_AWI_ACK,
    /**< Analog alert with information ACK (FEATURE_CDSMS). */

  CM_SMS_CMD_BC_ENABLE,
    /**< Enable Broadcast (FEATURE_BROADCAST_SMS). */

/** @cond
 */
  CM_SMS_CMD_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_sms_cmd_e_type;
/*~ SENTINEL cm_sms_cmd_e_type.CM_SMS_CMD_MAX */

/** Enumeration of SMS command errors returned by SMS command
    callback functions.
*/
typedef enum cm_sms_cmd_err_e {

/** @cond
*/
  CM_SMS_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_SMS_CMD_ERR_NOERR,
    /**< No errors found (FEATURE_CDSMS). */

  /* Error codes resulting from clients feeding a serving system command
  ** function with bad or inappropriate parameters. */

  CM_SMS_CMD_ERR_CLIENT_ID_P,
    /**< Bad client parameter (FEATURE_CDSMS). */

  CM_SMS_CMD_ERR_NOT_IDLE_S,
    /**< Cannot accept SMS origination while the SMS state is not Idle
         (FEATURE_CDSMS).*/

  CM_SMS_CMD_ERR_OFFLINE_S,
    /**< Phone is offline (FEATURE_CDSMS). */

  /* 4 */
  CM_SMS_CMD_ERR_CDMA_LOCK_S,
    /**< Phone is CDMA locked (FEATURE_CDSMS). */

  CM_SMS_CMD_ERR_CALL_ORIG_S,
    /**< Call origination is in progress (FEATURE_CDSMS). */

  CM_SMS_CMD_ERR_EMERGENCY_MODE_S,
    /**< Phone is in Emergency callback mode; only Emergency calls are allowed
         (FEATURE_CDSMS). */

  CM_SMS_CMD_ERR_NO_SRV_S,
    /**< Client tried to send MO SMS, but there is no service
         (FEATURE_CDSMS). */

  /* 8 */
  CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S,
    /**< Client tried to send MO SMS, but the service state is analog
        (FEATURE_CDSMS). */

  CM_SMS_CMD_ERR_NOT_ANALOG_S,
    /**< Client tried to send an Alert With Information (AWI) ACK, but the
         service state is not analog (FEATURE_CDSMS). */

  /* Error codes resulting from operational limitations of the CM. */

  CM_SMS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command (FEATURE_CDSMS). */

  CM_SMS_CMD_ERR_OTHER,
    /**< An error other than those listed above (FEATURE_CDSMS). */

  /* 12 */
  CM_SMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to a relevant feature being undefined. */

  CM_SMS_CMD_ERR_DDTM,
    /**< SMS blocked due to high PSIST on a 1X and DO call in progress. */

  CM_SMS_CMD_ERR_MODE_PREF,
    /**< Wrong mode preference over which SMS is sent. 
         Can be used only if CM_API_SMS_OVER_IP is defined. */

  CM_SMS_CMD_ERR_SIP,
    /**< Wrong SIP status when SMS is sent over SIP. 
         Can be used only if CM_API_SMS_OVER_IP is defined. */

/** @cond
*/
  CM_SMS_CMD_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_sms_cmd_err_e_type;
/*~ SENTINEL cm_sms_cmd_err_e_type.CM_SMS_CMD_ERR_MAX */


/** Enumeration of SMS events. Clients are able to register
    with the CM to be notified of any subsets of these events.
*/
typedef enum cm_sms_event_e {

/** @cond
*/
  CM_SMS_EVENT_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_SMS_EVENT_MO_MSG,
    /**< FEATURE_CDSMS. */

  CM_SMS_EVENT_AWI_ACK,
    /**< FEATURE_CDSMS. */

  CM_SMS_EVENT_MO_STATUS,
    /**< FEATURE_CDSMS. */

  CM_SMS_EVENT_MT_MSG,
    /**< FEATURE_CDSMS. */

  /* 4 */
  CM_SMS_EVENT_BC_ENABLED,
    /**< FEATURE_BROADCAST_SMS. */

  CM_SMS_EVENT_MSG_WAITING,
    /**< FEATURE_CDSMS. */

  CM_SMS_EVENT_N_VOICE,
    /**< FEATURE_CDSMS. */

/** @cond
*/
  CM_SMS_EVENT_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_sms_event_e_type;
/*~ SENTINEL cm_sms_event_e_type.CM_SMS_EVENT_MAX */


/** Enumeration of the possible statuses, as returned by the MC. Since SMS 
    uses Data Burst Messaging (DBM), these must map to the DBM enum
    (FEATURE_CDSMS only).
*/
typedef enum cm_sms_mo_status_e {

/** @cond
*/
  CM_SMS_MO_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_SMS_MO_OK=CM_DBM_MO_OK,
    /**< SMS message has been completed successfully. */

  CM_SMS_MO_HOLD_ORIG_RETRY_TIMEOUT=CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT,
    /**< Hold origination retry timeout.*/

  CM_SMS_MO_HOLD_ORIG=CM_DBM_MO_HOLD_ORIG,
    /**< MC cannot process MO SMS because HOLD_ORIG is TRUE. */

  CM_SMS_MO_NO_SRV=CM_DBM_MO_NO_SRV,
    /**< Cannot send an SMS message because there is no service. */

  /* 4 */
  CM_SMS_MO_ABORT=CM_DBM_MO_ABORT,
    /**< MC or CM aborts the SMS request. */

  CM_SMS_MO_NOT_ALLOWED_IN_AMPS=CM_DBM_MO_NOT_ALLOWED_IN_AMPS,
    /**< Cannot send an SMS message in analog mode. */

  CM_SMS_MO_NOT_ALLOWED_IN_HDR=CM_DBM_MO_NOT_ALLOWED_IN_HDR,
    /**< Cannot sent an SMS message while an HDR call is up. */

  CM_SMS_MO_L2_ACK_FAILURE=CM_DBM_MO_L2_ACK_FAILURE,
    /**< Not receiving L2 ACK from the base station. */

  /* 8 */
  CM_SMS_MO_OUT_OF_RESOURCES=CM_DBM_MO_OUT_OF_RESOURCES,
    /**< Out of resources (e.g., out of memory buffer). */

  CM_SMS_MO_ACCESS_TOO_LARGE=CM_DBM_MO_ACCESS_TOO_LARGE,
    /**< Message is too large to be sent over access. */

  CM_SMS_MO_DTC_TOO_LARGE=CM_DBM_MO_DTC_TOO_LARGE,
    /**< Message is too large to be sent over a DTC. */

  CM_SMS_MO_ACCT_BLOCK = CM_DBM_MO_ACCT_BLOCK,
    /**< Access Control based on call type (service option) is blocked. */

  /* 12 */
  CM_SMS_MO_SIP_PERMANENT_FAILURE,
    /**< Permanent SIP transport failure for MO SMS. Refer to 4xx 5xx 6xx SIP
         error codes. Can be used only if CM_API_SMS_OVER_IP is defined. */

  CM_SMS_MO_SIP_TEMPORARY_FAILURE,
    /**< The temporary SIP transport failure for MO SMS (i.e., SIP layer
         timeout). Can be used only if CM_API_SMS_OVER_IP is defined. */

/** @cond
*/
  CM_SMS_MO_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_sms_mo_status_e_type;
/*~ SENTINEL cm_sms_mo_status_e_type.CM_SMS_MO_MAX */


/** Enumeration of SMS states. A CM SMS must be in
   one of these states at all times.
*/
typedef enum cm_sms_state_e {

/** @cond
*/
  CM_SMS_STATE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_SMS_STATE_IDLE,
    /**< SMS is in Idle state. */

  CM_SMS_STATE_HOLD_ORIG_RETRY,
    /**< SMS is checking hold origination status periodically to determine
         if MO SMS can be sent to the MC. */

  CM_SMS_STATE_WAIT_FOR_STATUS,
    /**< SMS is waiting for MO SMS status from MC. */

/** @cond
*/
  CM_SMS_STATE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_sms_state_e_type;
/*~ SENTINEL cm_sms_state_e_type.CM_SMS_STATE_MAX */

/** @} */ /* end_addtogroup SMSO */

/** @addtogroup SMSO
    @{ */

/** @brief Structure for analog alert with information ACK message.
*/
typedef struct cm_sms_awi_ack_s {

  boolean                 bf;
    /**< Begin or Final flag. */

  uint8                   seq_no;
    /**< Sequence number. */

  uint8                   status;
    /**< Status. */

  uint8                   error_class;
    /**< Error class. */

} cm_sms_awi_ack_s_type;

/** @brief Type to hold SMS information. When notified of SMS events, clients 
    are given a reference to such a structure. FEATURE_CDSMS is removed in the 
    data structure when CM_API_SMS_OVER_IP is defined.
*/
typedef struct cm_sms_info_s {

  cm_client_id_type       sms_client_id;
    /**< Requesting client (MO_SMS/AWI_ACK) .*/

  cm_sms_state_e_type     sms_state;
    /**< SMS state for MO SMS. */

  cm_sms_awi_ack_s_type   awi_ack;
    /**< Field for AWI ACK. */

  uint8                   mt_sms_format;
    /**< Message format for MT SMS. */

  uint16                  length;
    /**< Length for SMS buffer; used by MT SMS only. */

  uint8                   *data;
    /**< Pointer to the MT SMS buffer. */

  sys_sys_mode_e_type     sys_mode;
    /**< System mode over which SMS is received. Cannot be used when
         CM_API_SMS_OVER_IP is defined. */

  boolean                 l2_ack_requested;
    /**< Whether an L2 ACK is required; used by MO SMS. */

  cm_sms_mo_status_e_type mo_status;
    /**< Status for MO SMS. */

  boolean                 bc_enable;
    /**< Enable broadcast SMS (FEATURE_BROADCAST_SMS). */

  cm_client_id_type       requesting_client_id;
    /**< Requesting client. */

} cm_sms_info_s_type;
// TODO - callbacks must copy data array instead of pointer
/*~ FIELD cm_sms_info_s.data VARRAY LENGTH cm_sms_info_s.length */

/** @} */ /* end_addtogroup SMSO */

/** @addtogroup SMSO
    @{ */

/** Type for Teleservice (TS) data formats. Clients must check
    for CM_API_REMOTE_WMS before using this enum.
*/
typedef enum cm_format_e
{
/** @cond
*/
  CM_SMS_FORMAT_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_SMS_FORMAT_CDMA,
    /**< IS-95. */

  CM_SMS_FORMAT_ANALOG_CLI,
    /**< IS-91. */

  CM_SMS_FORMAT_ANALOG_VOICE_MAIL,
    /**< IS-91. */

  CM_SMS_FORMAT_ANALOG_SMS,
    /**< IS-91. */

  /* 4 */
  CM_SMS_FORMAT_ANALOG_AWISMS,
    /**< IS-95 Alert With Information SMS. */

  CM_SMS_FORMAT_MWI,
    /**< Message Waiting Indication as voice mail. */

  CM_SMS_FORMAT_GW_PP,
    /**< GW Point-to-Point SMS. */

  CM_SMS_FORMAT_GW_CB,
    /**< GW CB SMS. */

/** @cond
*/
  CM_SMS_FORMAT_MAX  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_format_e_type;
/*~ SENTINEL cm_format_e_type.CM_SMS_FORMAT_MAX */

/** Type for number mode. Clients must check for
    CM_API_REMOTE_WMS before using this enum.
*/
typedef enum cm_num_mode_e
{
/** @cond
*/
  CM_NUM_MODE_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_NUM_MODE_NONE_DATA_NETWORK,
    /**< No data network. */

  CM_NUM_MODE_DATA_NETWORK,
    /**< Data network. */

/** @cond
*/
  CM_NUM_MODE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_num_mode_e_type;
/*~ SENTINEL cm_num_mode_e_type.CM_NUM_MODE_MAX */

/** @} */ /* end_addtogroup SMSO */

/** @addtogroup SMSO
    @{ */

/** @brief Type for SMS address. Clients must check for
    CM_API_REMOTE_WMS before using this struct.
*/
typedef struct cm_address_s
{
  cm_digit_mode_e_type          digit_mode;
    /**< Indicates 4-bit or 8-bit. */

  cm_num_mode_e_type            number_mode;
    /**< Only meaningful when digit_mode is 8-bit (used in CDMA only). */

  cm_num_type_e_type            number_type;
    /**< In a CDMA address, this is used only when the digit_mode is 8-bit. 
         To specify an international address for CDMA, use the following:
         - digit_mode  = 8-bit
         - number_mode = NONE_DATA_NETWORK
         - number_type = INTERNATIONAL
         - number_plan = TELEPHONY
         - number_of_digits = number of digits
         - digits = ASCII digits, e.g., 1, 2, 3, 4, and 5. */

  cm_num_plan_e_type            number_plan;
    /**< When using a CDMA address, this is used only when digit_mode is 8-bit. */

  uint8                         number_of_digits;
    /**< Number of digits. */

  uint8                         digits[ CM_SMS_ADDRESS_MAX ];
    /**< Each byte in this array represents a 4-bit or 8-bit digit of
         address data. */

} cm_address_s_type;

/** @brief Type for Over-The-Air (raw) message structure.  Clients must
    check for CM_API_REMOTE_WMS before using this struct.
*/
typedef struct cm_OTA_message
{

  cm_format_e_type                format;
    /**< Format of the OTA message. */

  uint16                          data_len;
    /**< OTA message data length. */

  uint8                           data[ CM_SMS_MAX_LEN ];
    /**< OTA message data. */

} cm_OTA_message_type;
 
/*~ PARAM data VARRAY LENGTH data_len */

/** @brief Type for sending SMS. Clients must check for
    CM_API_REMOTE_WMS before using this struct.
*/
typedef struct cm_sms_msg
{

  cm_OTA_message_type             ota;
    /**< Raw SMS data to be sent. */

  cm_address_s_type               address;
    /**< SMS destination address for the authentication. */

} cm_sms_msg_type;


/** @brief Type for received SMS. Clients must check for CM_API_MT_SMS before 
    using this struct. Can be used only if CM_API_SMS_OVER_IP is defined.
*/
typedef struct cm_mt_sms_msg
{

  cm_OTA_message_type             sms;
    /**< Raw SMS data to be received. */

  sys_sys_mode_e_type             sys_mode;
    /**< System mode in which the SMS is received. */

} cm_mt_sms_msg_type;

/** Type for SMS command callback functions. A client that
    wants to be notified of the result of an SMS command
    function must supply a pointer to a function of this type as
    the first parameter to an inband command function.
*/
typedef void (cm_sms_cmd_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to the client-provided data block. */

  cm_sms_cmd_e_type           sms_cmd,
    /**< Indicates which SMS command the error status sms_cmd_err is for. */

  cm_sms_cmd_err_e_type       sms_cmd_err
    /**< SMS command error code indicating if the command
         is rejected and the reason for the rejection. */
);

/** @} */ /* end_addtogroup SMSO */

/** @addtogroup SMSO
    @{ */

/*===========================================================================

FUNCTION: cm_sms_cmd_awi_ack

===========================================================================*/
/**
  Sends an analog AWI ACK message to the base station. Applicable for
  FEATURE_UASMS.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.
@param[in] seq_no         Sequence number.
@param[in] bf             Begin/Final flag.
@param[in] error_class    Error class.
@param[in] status         ACK status.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start(). \n
  FEATURE_CDSMS or FEATURE_UASMS must be defined.
*/
extern boolean cm_sms_cmd_awi_ack(

  cm_sms_cmd_cb_f_type   cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  uint8                  seq_no,
  boolean                bf,
  uint8                  error_class,
  uint8                  status

);
 
/*~ FUNCTION cm_sms_cmd_awi_ack */

/*===========================================================================

FUNCTION: cm_sms_cmd_bc_enable

===========================================================================*/
/**
  Enables/disables broadcast SMS. Applicable for FEATURE_BROADCAST_SMS.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.
@param[in] bc_enable      Enable broadcast message.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start(). \n 
  FEATURE_SMS and FEATURE_CDSMS must be defined.
*/
extern boolean cm_sms_cmd_bc_enable(

  cm_sms_cmd_cb_f_type  cmd_cb_func,
  void                  *data_block_ptr,
  cm_client_id_type     client_id,
  boolean               bc_enable

);
 
/*~ FUNCTION cm_sms_cmd_bc_enable */

/*===========================================================================

FUNCTION: cm_sms_cmd_mo_msg

===========================================================================*/
/**
  Sends a mobile-originated SMS message. Applicable for FEATURE_UASMS.

@param[in] cmd_cb_func      Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Requesting client ID.
@param[in] l2_ack_requested Whether an L2 ACK is required.
@param[in] data             Pointer to the SMS buffer.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start(). \n
  FEATURE_CDSMS or FEATURE_UASMS must be defined.
*/
extern boolean cm_sms_cmd_mo_msg(

  cm_sms_cmd_cb_f_type    cmd_cb_func,
  void                    *data_block_ptr,
  cm_client_id_type       client_id,
  boolean                 l2_ack_requested,
  const uint8             *data

);
/* TODO~ FUNCTION cm_sms_cmd_mo_msg */

/* Can only be used when CM_API_SMS_OVER_IP is defined.
*/
/*===========================================================================

FUNCTION: cm_sms_cmd_mo_msg2

===========================================================================*/
/**
  Sends a mobile-originated SMS message.

@param[in] cmd_cb_func      Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client           Requesting client ID.
@param[in] l2_ack_requested Whether an L2 ACK is required.
@param[in] data             Pointer to the SMS buffer.
@param[in] mode_pref        Mode over which the SMS is sent.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start().
*/
extern boolean cm_sms_cmd_mo_msg2(

    cm_sms_cmd_cb_f_type    cmd_cb_func,
    void                    *data_block_ptr,
    cm_client_id_type       client,
    boolean                 l2_ack_requested,
    const cm_sms_msg_type   *data,
    cm_mode_pref_e_type     mode_pref

);/* cm_sms_cmd_mo_msg2 */


/** @} */ /* end_addtogroup SMSO Short Messaging Service Object */

/*-----------------------------------------------------------------------------

Position Determination Object
** For internal use (uapdms) only **

-----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
** Defines
**-------------------------------------------------------------------------*/

/** @addtogroup PDO Position Determination Object
    @{ */

/*---------------------------------------------------------------------------
** Enums
**-------------------------------------------------------------------------*/

/** Possible Position Determination (PD) commands.
*/
typedef enum cm_pd_cmd_e {

/** @cond
*/
  CM_PD_CMD_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_PD_CMD_SEND_MSG,
    /**< Send a data burst message. */

  CM_PD_CMD_SESSION_START,
    /**< Start session command. */

  CM_PD_CMD_SESSION_END,
    /**< End session command. */

  CM_PD_CMD_FIX_START,
    /**< Block HDR data call origination to start GPS search. */

  /* 4 */
  CM_PD_CMD_FIX_END,
    /**< Unblock HDR data call origination. */

  CM_PD_CMD_SEND_REVA,
    /**< Command to send a Rev. A message. */

  CM_PD_CMD_SESSION_DORMANT,
    /**< The dormancy status of the session changed. */

/** @cond
*/
  CM_PD_CMD_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_pd_cmd_e_type;
/*~ SENTINEL cm_pd_cmd_e_type.CM_PD_CMD_MAX */


/** Possible PD errors.
*/
typedef enum cm_pd_cmd_err_e {

/** @cond
*/
  CM_PD_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_PD_CMD_ERR_NOERR,
    /**< No errors found. */

  /* Error codes resulting from clients feeding a serving system command
     function with bad or inappropriate parameters. */

  CM_PD_CMD_ERR_CLIENT_ID_P,
    /**< Bad client parameter. */

  CM_PD_CMD_ERR_NOT_IDLE_S,
    /**< Cannot accept SMS origination while SMS state is not Idle. */

  CM_PD_CMD_ERR_OFFLINE_S,
    /**< Phone is offline. */

  /* 4 */
  CM_PD_CMD_ERR_CDMA_LOCK_S,
    /**< Phone is CDMA locked. */

  CM_PD_CMD_ERR_CALL_ORIG_S,
    /**< Call origination is in progress. */

  CM_PD_CMD_ERR_EMERGENCY_MODE_S,
    /**<  Phone is in Emergency callback mode. Only Emergency calls are
          allowed. */

  CM_PD_CMD_ERR_NO_SRV_S,
    /**< Client tried to send an MO SMS, but there is no service. */

  /* 8 */
  CM_PD_CMD_ERR_NOT_ALLOWED_IN_AMPS_S,
    /**< Client tried to send, but the service state is analog. */

  CM_PD_CMD_ERR_NOT_ALLOWED_IN_HDR_S,
    /**< Client tried to send, but an HDR call is up. */

  /* Error codes resulting from operational limitations
     of the CM. */

  CM_PD_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  CM_PD_CMD_ERR_PHONE_BUSY,
    /**< Phone is already in use. */

  /* 12 */
  CM_PD_CMD_ERR_SESSION_TYPE_INVALID,
    /**< Session type is invalid. */

  CM_PD_CMD_ERR_OTHER,
    /**< An error other than those listed above. */

  CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to a relevant feature being undefined. */

  CM_PD_CMD_ERR_PAYLOAD_SIZE_P,
    /**< Invalid size of message payload. */

  CM_PD_CMD_ERR_PAYLOAD_NULL_PTR_P,
    /**< NULL pointer passed as message payload. */

/** @cond
*/
  CM_PD_CMD_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_pd_cmd_err_e_type;
/*~ SENTINEL cm_pd_cmd_err_e_type.CM_PD_CMD_ERR_MAX */


/** Type for PD command callback functions. A client that wants
    to be notified of the result of a PD command function must
    supply a pointer to a function of this type.
*/
typedef void (cm_pd_cmd_cb_f_type) (

  void                       *data_block_ptr,
    /**< Pointer to the client-provided data block. */

  cm_pd_cmd_e_type           pd_cmd,
    /**< Indicates which PD command the pd_cmd_err error status is for. */

  cm_pd_cmd_err_e_type       pd_cmd_err
    /**< Reason for rejection. */
);

/** @} */ /* end_addtogroup PDO */

/** @addtogroup SUPS Supplementary Services Object
    @{ */

/** Enumeration of SUPS command errors returned by command
    callback functions.
*/
typedef enum cm_sups_cmd_err_e {

/** @cond
*/
  CM_SUPS_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_SUPS_CMD_ERR_NOERR,
    /**< No errors found. */

  /* Error codes resulting from clients providing a sups command
  ** function with bad or invalid parameters. */

  CM_SUPS_CMD_ERR_CLIENT_ID_P,
    /**< Invalid client ID; NULL or not initialized. */

  CM_SUPS_CMD_ERR_NULL_PARAM_P,
    /**< Null pointer parameter. */

  /* Error codes resulting from clients trying to instruct the
  ** CM to perform inappropriate actions relative to the current
  ** state of the call and/or phone. */

  CM_SUPS_CMD_ERR_OFFLINE_S,
    /**< Phone is offline. */

  /* 4 */
  CM_SUPS_CMD_ERR_SRV_STATE_S,
    /**< The command in question is invalid in the current phone serving state
         (e.g., answering a call but the serving state is no service). */

  /* Error codes resulting from operational limitations
     of the CM. */

  CM_SUPS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  CM_SUPS_CMD_ERR_OTHER,
    /**< An error other than those listed above was found. */

  CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to a relevant feature being undefined. */

  /* 8 */
  CM_SUPS_CMD_ERR_CALL_CONTROL_FAILURE,
    /**< Unable to complete call control. */

  CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED,
    /**< Call control rejected SUPS request. */

  CM_SUPS_CMD_ERR_SUBS_ID_P,
    /**< Wrong Subscription-Id parameter */

  /** @cond
*/
  CM_SUPS_CMD_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
*/

} cm_sups_cmd_err_e_type;
/*~ SENTINEL cm_sups_cmd_err_e_type.CM_SUPS_CMD_ERR_MAX */


/** Enumeration of Supplementary Services events. Clients are
    able to register with CM to be notified of any subsets of
    these Supplementary Services events.
*/
typedef enum cm_sups_event_e {

/** @cond
*/
  CM_SUPS_EVENT_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_SUPS_EVENT_REGISTER,
    /**< Register Supplementary Services. */

  CM_SUPS_EVENT_REGISTER_CONF,
    /**< Register Supplementary Services confirmation. */

  CM_SUPS_EVENT_ERASE,
    /**< Erase Supplementary Services request. */

  CM_SUPS_EVENT_ERASE_CONF,
    /**< Erase Supplementary Services confirmation. */

  /* 4 */
  CM_SUPS_EVENT_ACTIVATE,
    /**< Activate Supplementary Services request. */

  CM_SUPS_EVENT_ACTIVATE_CONF,
    /**< Activate Supplementary Services confirmation. */

  CM_SUPS_EVENT_DEACTIVATE,
    /**< Deactivate Supplementary Services request. */

  CM_SUPS_EVENT_DEACTIVATE_CONF,
    /**< Deactivate Supplementary Services confirmation. */

  /* 8 */
  CM_SUPS_EVENT_INTERROGATE,
    /**< Interrogate Supplementary Services request. */

  CM_SUPS_EVENT_INTERROGATE_CONF,
    /**< Interrogate Supplementary Services confirmation. */

  CM_SUPS_EVENT_REG_PASSWORD,
    /**< Register password Supplementary Services request*/

  CM_SUPS_EVENT_REG_PASSWORD_CONF,
    /**< Register password Supplementary Services confirmation. */

  /* 12 */
  CM_SUPS_EVENT_PROCESS_USS,
    /**< Process Unstructured Supplementary Services request. */

  CM_SUPS_EVENT_PROCESS_USS_CONF,
    /**< Process Unstructured Supplementary Services confirmation. */

  CM_SUPS_EVENT_RELEASE,
    /**< Release Supplementary Services request. */

  CM_SUPS_EVENT_ABORT,
    /**< Abort Supplementary Services request. */

  /* 16 */
  CM_SUPS_EVENT_FWD_CHECK_IND,
    /**< Forward check Supplementary Services indication. */

  CM_SUPS_EVENT_USS_NOTIFY_IND,
    /**< Unstructured Supplementary Services notify indication. */

  CM_SUPS_EVENT_USS_NOTIFY_RES,
    /**< Unstructured Supplementary Services notify response. */

  CM_SUPS_EVENT_USS_IND,
    /**< Unstructured Supplementary Services indication. */

  /* 20 */
  CM_SUPS_EVENT_USS_RES,
    /**< Unstructured Supplementary Services response. */

  CM_SUPS_EVENT_RELEASE_USS_IND,
    /**< Release Unstructured Supplementary Services indication. */

  CM_SUPS_EVENT_GET_PASSWORD_IND,
    /**< Get password Supplementary Services indication. */

  CM_SUPS_EVENT_GET_PASSWORD_RES,
    /**< Get password Supplementary Services response. */

  /* 24 */
  CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND,
    /**< LCS location notification indication. */

  CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_RES,
    /**< LCS location notification response. */

  CM_SUPS_EVENT_LCS_MOLR,
    /**< LCS MOLR request. */

  CM_SUPS_EVENT_LCS_MOLR_COMPLETED,
    /**< LCS MOLR completed. */

  /* 28 */
  CM_SUPS_EVENT_LCS_MOLR_CONF,
    /**< LCS MOLR confirmation. */

  CM_SUPS_EVENT_SS_MOD_TO_ORIG,
    /**< SUPS command was modified to origination due to call control. */

/** @cond
*/
  CM_SUPS_EVENT_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_sups_event_e_type;
/*~ SENTINEL cm_sups_event_e_type.CM_SUPS_EVENT_MAX */

/* apitrim -include */

/** Response to Unstructured Supplementary Services indication
    from the network.
*/
typedef enum cm_uss_res_e {

/** @cond
*/
  CM_USS_RES_SS_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_USS_RES_SS_DATA,
    /**< USS response with data. */

  CM_USS_RES_SS_ERROR,
    /**< USS response indicating SS error. */

/** @cond
*/
  CM_USS_RES_SS_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_uss_res_e_type;
/*~ SENTINEL cm_uss_res_e_type.CM_USS_RES_SS_MAX */
/* apitrim -end-include */

/** Enumeration of the USS data type.
*/
typedef enum cm_uss_data_e
{
/** @cond
*/
  CM_USS_UNKNOWN_TYPE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_USS_PHASE1_TYPE,
    /**< Phase 1 USS data. */

  CM_USS_PHASE2_TYPE,
    /**< Phase 2 USS data. */

  CM_USS_DATA_NONE,
    /**< USS data with no type. */

/** @cond
*/
  CM_USS_PHASE2_TYPE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_uss_data_e_type;
/*~ SENTINEL cm_uss_data_e_type.CM_USS_PHASE2_TYPE_MAX */


/*---------------------------------------------------------------------------
** Datatypes
**-------------------------------------------------------------------------*/

/** @brief Structure to hold voice call related parameters when call control
    modifies sups call to voice.
*/
typedef struct cm_ss_to_voice_param_s{

    cm_cc_result_e_type              call_control_result;
      /**< Indicates whether there were modifications. */

    cm_call_id_type                  call_id;
      /**< Call ID. */

} cm_modified_ss_param_s_type;

/** @brief Unparsed SUPS L3 message passed from the MN. Clients must check
    for CM_SUPS_UNPARSED_L3_DATA before using this field.
*/
typedef struct cm_sups_unparsed_l3_data_s
{
   boolean                         present;
     /**< To check whether size and data are valid. */

   uint16                            size;
     /**< Actual size of the array data. */

   byte                            data[CM_MAX_SUPS_PARAMETER_LENGTH];
     /**< Array to hold the L3 raw data from the MN. */

} cm_sups_unparsed_l3_data_s_type;


/** @brief Type to hold Supplementary Services information. When
    notified of Supplementary Services events, clients are given
    a reference to such a structure.
*/
typedef struct cm_sups_info_s {

  cm_client_id_type                   sups_client_id;
    /**< Client. */

  sys_modem_as_id_e_type              asubs_id;
    /**< Active subscription ID. */

  boolean                             ss_success;
    /**< Supplementary Services success. */

  uint8                               ss_ref;
    /**< Supplementary Services reference. */

  uint8                               ss_code;
    /**< Supplementary Services code. */

  uint8                               ss_operation;
    /**< Supplementary Services operation. */

  uint8                               invoke_id;
    /**< Invocation ID. */

  basic_service_s_type                basic_service;
    /**< Basic service group information. */

  cm_called_party_bcd_no_s_type       forwarded_to_number;
    /**< Forwarded to number. */

  cm_called_party_subaddress_s_type   forwarded_to_subaddress;
    /**< Forwarded to subaddress. */

  ie_no_reply_condition_timer_s_type  nr_timer;
    /**< No reply condition timer. */

  ie_ss_password_s_type               ss_password;
    /**< Supplementary Services password. */

  ie_ss_password_s_type               ss_new_password;
    /**< New password. */

  ie_ss_password_s_type               ss_new_password_again;
    /**< Confirmation of new password. */

  ie_ss_code_s_type                   code;
    /**< Supplementary Services code. */

  ss_password_value                   guidance_info;
    /**< Password guidance information. */

  cm_ss_conf_e_type                   conf_type;
    /**< Supplementary Services confirmation type. */

  ie_cm_ss_error_s_type               ss_error;
    /**< Supplementary Services error. */

  ie_forwarding_info_s_type           forwarding_info;
    /**< Call forwarding information. */

  ie_call_barring_info_s_type         call_barring_info;
    /**< Call barring information. */

  ie_cug_info_s_type                  cug_info;
    /**< Closed User Group information. */

  ie_cm_cc_cause_s_type               cc_cause;
    /**< Call Control cause. */

  ie_ss_status_s_type                 ss_status;
    /**< Supplementary Services status. */

  basic_service_group_list_s_type     bsg_list;
    /**< Basic service group list information. */

  ie_forwarding_feature_list_s_type   fwd_feature_list;
    /**< Forwarding feature list information. */

  cli_restriction_info_s_type         cli_restriction;
    /**< Calling Line Information (CLI) restriction information. */

  cm_uss_data_e_type                  uss_data_type;
    /**< Type of Unstructured Supplementary Services message (phase1, phase2). */

  uss_data_s_type                     uss_data;
    /**< Unstructured Supplementary Services data. */

  ie_cm_ss_data_s_type                ss_data;
    /**< Supplementary Services data. */

  location_notification_s_type    location_notification;
    /**< LCS location notification data. */

  lcs_molr_res_s_type                     lcs_molr_res;
    /**< LCS MOLR response. */

  cm_client_id_type                   requesting_client_id;
    /**< Requesting client. */

  cm_ccbs_indx_type                   ccbs_indx;
    /**< ccbs_indx = CM_CCBS_INDX_INVALID results in deactivation of all pending
         CCBS requests.\n
         ccbs_indx = A valid value results in one particular CCBS
         being deactivated.\n
         Clients must check for CM_API_CCBS before using
         this field. */

  ccbs_info_s_type                    ccbs_info;
    /**< Carries a list of CCBS indices returned as a result of
         interrogation. Note that ccbs_Index contains the call IDs. 
         Clients must check for CM_API_CCBS before using this field. */

  cm_sups_unparsed_l3_data_s_type      sups_unparsed_l3_data;
    /**< Unparsed L3 message from the MN. Clients must check for
         CM_SUPS_UNPARSED_L3_DATA before using this field. */

  cm_alpha_s_type                      cc_result_alpha;
    /**< Alpha sent by SIM toolkit application after call control. 
         This is to be used if cc_result_alpha.len > 0. */

  cm_modified_ss_param_s_type          mod_ss_params;
    /**< Modified parameters of ss */

} cm_sups_info_s_type;


/** @brief Get password response parameters.
*/
typedef struct cm_get_pwd_res_params_s {

  uint8                      invoke_id;
    /**< Invocation ID. */

  ie_ss_password_s_type      ss_pwd;
    /**< Supplementary Services password; old, new, or confirmation of
         new password, as requested by the network. */

} cm_get_pwd_res_params_s_type;


/** @brief LCS location notification response parameters.
*/
typedef struct cm_lcs_location_notification_res_params_s {

  uint8                        invoke_id;
    /**< Invocation ID. */

  ie_ss_location_notification_res_s_type  location_notification_res;
    /**< Enumerated response. */

} cm_lcs_location_notification_res_params_s_type;

/** @brief LCS MOLR Supplementary Services parameters.
*/
typedef  struct cm_lcs_molr_ss_params_s {

  uint8                  ss_ref;
    /**< Supplementary Services reference. */

  cm_molr_type_e_type             molr_type;
    /**< MOLR type. */

  location_method_s_type       location_method;
    /**< Location method. */

  lcs_qos_s_type               lcs_qos;
    /**< QoS type. */

  lcs_client_external_id_s_type external_id;
    /**< LCS client external ID. */

  mlc_number_s_type            mlc_number;
    /**< MLC number. */

  gps_assistance_data_s_type   gps_assistance_data;
    /**< GPS Assistance Data (GAD). */

  supported_gad_shapes_s_type  supported_gad_shapes;
    /**< Supported GAD shapes. */

} cm_lcs_molr_params_s_type;

/** @brief LCS MOLR completed Supplementary Services parameters.
*/
typedef struct cm_lcs_molr_completed_params_s {

  uint8  ss_ref;
    /**< Supplementary Services reference. */

} cm_lcs_molr_completed_params_s_type;

/** @brief Unstructured Supplementary Services notify response parameters.
*/
typedef struct cm_uss_notify_res_params_s {

  ie_cm_ss_error_s_type        ss_error;
    /**< USS notify response Supplementary Services error. */

  uint8                        invoke_id;
    /**< Invocation ID. */

} cm_uss_notify_res_params_s_type;

/** @brief Unstructured Supplementary Services response parameters.
*/
typedef struct cm_uss_res_params_s {

  uint8                           invoke_id;
    /**< Invocation ID. */

  uss_res_data_s_type             uss_res_data;
    /**< USS response data. */

} cm_uss_res_params_s_type;

/*---------------------------------------------------------------------------
** Macros
**-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
** Functions
**-------------------------------------------------------------------------*/

/** Type for Supplementary Services command callback functions.
    A client that wants to be notified of the result of a SUPS
    command function must supply a pointer to a function of this
    type as the first parameter to a SUPS command function.
*/
typedef void (cm_sups_cmd_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to the client-provided data block. */

  cm_sups_cmd_e_type           sups_cmd,
    /**< Indicates which SUPS command the sups_cmd_err error status is for. */

  cm_sups_cmd_err_e_type       sups_cmd_err
    /**< SUPS command error code, indicating if the command
         is rejected and the reason for the rejection. */
);

/** Type for SUPS command callback functions.
    A client that wants to be notified of the result of a
    SUPS command function must supply a pointer to a function
    of this type as the first parameter to a SUPS command function.
*/
typedef void (cm_sups_cmd_exten_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to the client-provided data block. */

  cm_sups_cmd_e_type           sups_cmd,
    /**< Indicate which SUPS command this error status is for. */

  cm_sups_cmd_err_e_type       sups_cmd_err,
    /**< SUPS command error code, indicating whether the command
         is rejected and, if so, the reason for the rejection. */

  cm_sups_cmd_err_cause_e_type err_cause,
  /**< Cause of Error to be used when sups_cmd_err is other than
       CM_SUPS_CMD_ERR_NOERR, currently it is supported only for
       CC_REJECT error. */

  cm_alpha_s_type              alpha
  /**< Modified alpha to be used only if alpha.len > 0. */  

);
/*~ CALLBACK cm_sups_cmd_exten_cb_f_type */

/** @} */ /* end_addtogroup SUPS */

/** @addtogroup SUPS Supplementary Services Object
    @{ */

/*===========================================================================

FUNCTION: cm_sups_cmd_register

===========================================================================*/
/**
  Registers a SUPS command to the CM. This function allows the
  client to send a Register Supplementary Services request to
  the CM. This is a common function that can be used to
  register different Supplementary Services. The client ID
  should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status. If the
  command request is processed without errors,
  CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Originating client.
@param[in] reg_cmd_params   Pointer to register SUPS command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_register(

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type               client_id,
  const cm_reg_ss_params_s_type   *reg_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_register */
/*~ PARAM reg_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_erase

===========================================================================*/
/**
  Erases a SUPS command to the CM. This function enables the
  client to send an Erase Supplementary Services request to the
  CM. This is a common function that can be used to erase a
  previous registration of a Supplementary Service. The client
  ID should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status. If the
  command request is processed without errors,
  CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Originating client.
@param[in] erase_cmd_params Pointer to erase SUPS command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_erase(

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  const cm_erase_ss_params_s_type   *erase_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_erase */
/*~ PARAM erase_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_activate

===========================================================================*/
/**
  Sends an Activate SUPS request to the CM. This is a common function that can
  be used to activate different Supplementary Services. The client ID should be
  from a registered CM client.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status. If the
  command request is processed without errors,
  CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Originating client.
@param[in] act_cmd_params   Pointer to activate SUPS command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_activate(

  cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id,
  const cm_act_ss_params_s_type  *act_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_activate */
/*~ PARAM act_cmd_params POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_activate_exten

===========================================================================*/
/**
  Sends an Activate Supplementary Services request. This is a common function
  that can be used to activate different Supplementary Services. The client ID
  should be from a registered CM client.

  Clients must specify the initiating client category and provide an
  appropriate callback function pointer. Clients must also check if
  CM_API_MODEM_CALL_CONTROL is defined before calling this API.

  The client-supplied callback function sups_cmd_cb_func will be called to
  notify the client of the command status. If the command request is
  processed without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the
  client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] act_cmd_params    Pointer to activate SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_activate_exten(

  cm_sups_cmd_exten_cb_f_type    sups_cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id,
  const cm_act_ss_params_s_type  *act_cmd_params,
  const cm_num_s_type            *dialed_digits,
  cm_orig_client_categ_e_type    orig_client_categ

);
/*~ FUNCTION cm_sups_cmd_activate_exten */
/*~ PARAM act_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_register_exten

===========================================================================*/
/**
  Registers a SUPS command with the CM.

  This is a common function that can be used to register different Supplementary
  Services. The client ID should be from a registered CM client.

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients must also specify the initiating client category and provide an
  appropriate callback function pointer.

  The client-supplied callback function sups_cmd_cb_func will be called to
  notify the client of the command status. If the command request is
  processed without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the
  client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] reg_cmd_params    Pointer to register SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_register_exten (

  cm_sups_cmd_exten_cb_f_type    sups_cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id,
  const cm_reg_ss_params_s_type  *reg_cmd_params,
  const cm_num_s_type            *dialed_digits,
  cm_orig_client_categ_e_type    orig_client_categ

);
/*~ FUNCTION cm_sups_cmd_register_exten */
/*~ PARAM reg_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_reg_password_exten

===========================================================================*/
/**
  Sends a Register Password Supplementary Services request to the CM.

  This function is called to register a password for a Supplementary Service
  (e.g., register a password for call restriction service) or to set a new
  password (overwrite the existing password with a new password). The client ID
  should be from a registered CM client. 

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling
  this API. Clients must also specify the initiating client category and provide
  an appropriate callback function pointer.  
  
  The client-supplied callback function sups_cmd_cb_func will be called to
  notify the client of the command status. If the command request is
  processed without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the
  client.

@param[in] sups_cmd_cb_func   Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] reg_pwd_cmd_params Pointer to register password SUPS command data.
@param[in] dialed_digits      Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ  Initiating client category.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_reg_password_exten (

  cm_sups_cmd_exten_cb_f_type     sups_cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type               client_id,
  const cm_reg_pwd_params_s_type  *reg_pwd_cmd_params,
  const cm_num_s_type             *dialed_digits,
  cm_orig_client_categ_e_type     orig_client_categ

);
/*~ FUNCTION cm_sups_cmd_reg_password_exten */
/*~ PARAM reg_pwd_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_process_uss_exten

===========================================================================*/
/**
  Processes Unstructured SS commands to the CM.

  This command sends a mobile-initiated Unstructured Supplementary Services Data
  request. It enables the client to send a Process Unstructured Supplementary
  Services request to the CM. This function is used to originate an Unstructured
  SS Data operation by the mobile station. 

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients must also specify the initiating client category and provide an
  appropriate callback function pointer.  

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client. 

@param[in] sups_cmd_cb_func       Client callback function.
@param[in] data_block_ptr         Pointer to the client callback data block.
@param[in] client_id              Originating client.
@param[in] process_uss_cmd_params Pointer to process Unstructured SUPS command
                                  data.
@param[in] dialed_digits          Pointer to a cm_num_s_type of the digits 
                                  dialed.
@param[in] orig_client_categ      Initiating client category.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_process_uss_exten (

  cm_sups_cmd_exten_cb_f_type    sups_cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id,
  const cm_uss_params_s_type     *process_uss_cmd_params,
  const cm_num_s_type            *dialed_digits,
  cm_orig_client_categ_e_type    orig_client_categ

);
/*~ FUNCTION cm_sups_cmd_process_uss_exten */
/*~ PARAM process_uss_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_deactivate_exten

===========================================================================*/
/**
  Deactivates a SUPS command to the CM.

  This function enables the client to send a Deactivate Supplementary 
  Services request to the CM. This is a common function that can be used to 
  deactivate different Supplementary Services. The client ID should be from a
  registered CM client.

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients also must specify the initiating client category and provide
  an appropriate callback function pointer. 

  The client-supplied callback function cmd_cb_func will be called to notify the
  client of the command status. If the command request is processed without
  errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] deact_cmd_params  Pointer to erase SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_deactivate_exten (

  cm_sups_cmd_exten_cb_f_type       sups_cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  const cm_deact_ss_params_s_type   *deact_cmd_params,
  const cm_num_s_type               *dialed_digits,
  cm_orig_client_categ_e_type       orig_client_categ

);
/*~ FUNCTION cm_sups_cmd_deactivate_exten */
/*~ PARAM deact_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_interrogate_exten

===========================================================================*/
/**
  Interrogates a SUPS command to the CM.

  This function enables the client to send an Interrogate Supplementary 
  Services request to the CM. The status of a Supplementary Service can be 
  checked using this command. The client ID should be from a registered 
  CM client. 

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients must also specify the initiating client category and provide
  an appropriate callback function pointer. 
  
  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client. 

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] interr_cmd_params Pointer to interrogate SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_interrogate_exten (

  cm_sups_cmd_exten_cb_f_type        sups_cmd_cb_func,
  void                               *data_block_ptr,
  cm_client_id_type                  client_id,
  const cm_interr_ss_params_s_type   *interr_cmd_params,
  const cm_num_s_type                *dialed_digits,
  cm_orig_client_categ_e_type        orig_client_categ

);
/*~ FUNCTION cm_sups_cmd_interrogate_exten */
/*~ PARAM interr_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_erase_exten

===========================================================================*/
/**
  Erases a SUPS command to the CM.

  This function enables the client to send an Erase Supplementary Services
  request to the CM. This is a common function that can be used to erase
  a previous registration of a Supplementary Service. The client ID should
  be from a registered CM client.
 
  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients must also specify the initiating client category and provide
  an appropriate callback function pointer. 

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] erase_cmd_params  Pointer to erase SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_erase_exten (

  cm_sups_cmd_exten_cb_f_type       sups_cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  const cm_erase_ss_params_s_type   *erase_cmd_params,
  const cm_num_s_type               *dialed_digits,
  cm_orig_client_categ_e_type       orig_client_categ

);
/*~ FUNCTION cm_sups_cmd_erase_exten */
/*~ PARAM erase_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_deactivate

===========================================================================*/
/**
  Deactivate a SUPS command to the CM.

  This function enables the client to send a Deactivate Supplementary 
  Services request to the CM. This is a common function that can be used to 
  deactivate different Supplementary Services. The client ID should be from a
  registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Originating client.
@param[in] deact_cmd_params Pointer to erase SUPS command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_deactivate(

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  const cm_deact_ss_params_s_type   *deact_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_deactivate */
/*~ PARAM deact_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_interrogate

===========================================================================*/
/**
  Interrogates a SUPS command to the CM.

  This function enables the client to send an Interrogate Supplementary Services
  request to the CM. The status of a Supplementary Service can be checked using
  this command. The client ID should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] interr_cmd_params Pointer to interrogate SUPS command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_interrogate(

  cm_sups_cmd_cb_f_type              sups_cmd_cb_func,
  void                               *data_block_ptr,
  cm_client_id_type                  client_id,
  const cm_interr_ss_params_s_type   *interr_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_interrogate */
/*~ PARAM interr_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_process_uss

===========================================================================*/
/**
  Processes an Unstructured SUPS command to the CM.

  This fucntion enables the client to send a Process Unstructured Supplementary
  Services request to the CM. It is used to originate an Unstructured SUPS Data
  operation by the mobile station.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func       Client callback function.
@param[in] data_block_ptr         Pointer to the client callback data block.
@param[in] client_id              Originating client.
@param[in] process_uss_cmd_params Pointer to process unstructured SUPS command 
                                  data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_process_uss(

  cm_sups_cmd_cb_f_type         sups_cmd_cb_func,
  void                          *data_block_ptr,
  cm_client_id_type             client_id,
  const cm_uss_params_s_type    *process_uss_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_process_uss */
/*~ PARAM process_uss_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_reg_password

===========================================================================*/
/**
  Registers a Password SUPS command to the CM.

  This function enables the client to send a Register Password Supplementary
  Services request to the CM. It is called to register a password for a
  Supplementary Service (for example, register a password for call restriction
  service) or to set a new password (overwrite the existing password with a new
  password). The client ID should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func   Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] reg_pwd_cmd_params Pointer to register password SUPS command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_reg_password(

  cm_sups_cmd_cb_f_type            sups_cmd_cb_func,
  void                             *data_block_ptr,
  cm_client_id_type                client_id,
  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_reg_password */
/*~ PARAM reg_pwd_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_release

===========================================================================*/
/**
  Releases a SUPS command to the CM.

  This function enables the client to send a Release SUPS request to the CM. The
  release request is sent to end a Get Password operation or a mobile-originated
  Unstructured Supplementary Services data operation. The client ID should be
  from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func   Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] release_cmd_params Pointer to release SUPS command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
              FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_release(

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
  void                                *data_block_ptr,
  cm_client_id_type                   client_id,
  const cm_release_ss_params_s_type   *release_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_release */
/*~ PARAM release_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_get_password_res

===========================================================================*/
/**
  Sends a response to a Get Password Indication.

  This function is called by the client to send a Get Password Response to the
  CM. This response is sent when the mobile station receives a Get Password
  Indication from the network. The client ID should be from a registered CM
  client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func   Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] get_pwd_res_params Pointer to get password response data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_get_password_res(

  cm_sups_cmd_cb_f_type                sups_cmd_cb_func,
  void                                 *data_block_ptr,
  cm_client_id_type                    client_id,
  const cm_get_pwd_res_params_s_type   *get_pwd_res_params

);
 
/*~ FUNCTION cm_sups_cmd_get_password_res */
/*~ PARAM get_pwd_res_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_lcs_molr

===========================================================================*/
/**
  Sends an LCS MOLR SUPS command to the CM. The client ID should
  be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] sups_cmd_cb_func    Client callback function.
@param[in] data_block_ptr      Pointer to the client callback data block.
@param[in] client              Originating client.
@param[in] lcs_molr_cmd_params Pointer to LCS MOLR SUPS command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_sups_cmd_lcs_molr
(

        cm_sups_cmd_cb_f_type            sups_cmd_cb_func,
        void                             *data_block_ptr,
        cm_client_id_type                client,
        const cm_lcs_molr_params_s_type  *lcs_molr_cmd_params

);
 
/*~ FUNCTION cm_sups_cmd_lcs_molr */
/*~ PARAM lcs_molr_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_lcs_location_notification_res

===========================================================================*/
/**
  Sends an LCS location notification response to the network. The client ID
  should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func                     Client callback function.
@param[in] data_block_ptr                       Pointer to the client callback
                                                data block.
@param[in] client                               Originating client.
@param[in] lcs_location_notification_res_params Pointer to location
                                                notification response data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_sups_cmd_lcs_location_notification_res(

        cm_sups_cmd_cb_f_type              sups_cmd_cb_func,
        void                               *data_block_ptr,
        cm_client_id_type                  client,
  const cm_lcs_location_notification_res_params_s_type       *lcs_location_notification_res_params


);
 
/*~ FUNCTION  cm_sups_cmd_lcs_location_notification_res */
/*~ PARAM lcs_location_notification_res_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_lcs_molr_completed

===========================================================================*/
/**
  Sends an LCS MOLR SUPS Completed command to the CM. The client ID
  should be from a registered CM client.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] sups_cmd_cb_func              Client callback function.
@param[in] data_block_ptr                Pointer to the client callback data
                                         block.
@param[in] client                        Originating client.
@param[in] lcs_molr_completed_cmd_params Pointer to LCS MOLR Completed SUPS
                                         command data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_sups_cmd_lcs_molr_completed(

        cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
        void                        *data_block_ptr,
        cm_client_id_type           client,
 const cm_lcs_molr_completed_params_s_type    *lcs_molr_completed_cmd_params

);

 
/*~ FUNCTION  cm_sups_cmd_lcs_molr_completed */
/*~ PARAM lcs_molr_completed_cmd_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_uss_notify_res

===========================================================================*/
/**
  Sends an Unstructured SUPS Notify Response command to the CM.

  This response message is sent by the mobile station when an Unstructured SUPS
  Notification received from the network has an error.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func      Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Originating client.
@param[in] uss_notify_res_params Pointer to Unstructured SUPS Notify
                                 response data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_uss_notify_res(

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
  void                                *data_block_ptr,
  cm_client_id_type                   client_id,
  const cm_uss_notify_res_params_s_type     *uss_notify_res_params

);
 
/*~ FUNCTION cm_sups_cmd_uss_notify_res */
/*~ PARAM uss_notify_res_params POINTER */

/*===========================================================================

FUNCTION: cm_sups_cmd_uss_res

===========================================================================*/
/**
  Sends an Unstructured SUPS response command to the CM.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] sups_cmd_cb_func Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Originating client.
@param[in] uss_res_params   Pointer to Unstructured SUPS response data.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_uss_res(

  cm_sups_cmd_cb_f_type            sups_cmd_cb_func,
  void                             *data_block_ptr,
  cm_client_id_type                client_id,
  const cm_uss_res_params_s_type   *uss_res_params

);
 
/*~ FUNCTION cm_sups_cmd_uss_res */
/*~ PARAM uss_res_params POINTER */

/** @} */ /* end_addtogroup SUPS Supplementary Services */

/*-----------------------------------------------------------------------------

Data Burst Message Object
** For internal use (uapdms) only **

-----------------------------------------------------------------------------*/

/** @addtogroup DBM Data Burst Messaging
    @{ */

/*---------------------------------------------------------------------------
** Defines
**-------------------------------------------------------------------------*/

/** Data Burst Message maximum payload size.
*/
#define CM_DBM_MAX_PAYLOAD_SIZE 255 

/** @} */ /* end_addtogroup DBM */

/*---------------------------------------------------------------------------
** Enums
**-------------------------------------------------------------------------*/

/** @addtogroup DBM
    @{ */

/** Transmission preference whether to send on Enhanced Access Channel (EACH),
    Access Channel (ACH), or set up a call.
*/
typedef enum
{
  /** @cond
  */
  CM_DBM_NONE = -1, /* <internal> */
  /** @endcond */

  CM_DBM_TRANSMIT_AUTOMATIC,
  /**< The CM will try to send the DBM on either the access channel, enhanced
       access channel, or traffic channel (if the traffic channel is
       available). If it fails to do that, it originates a call and sends the
      DBM on the new traffic channel. */

  CM_DBM_TRANSMIT_ENH_ACCESS_OR_TRAFFIC,
  /**< The CM will try to send the DBM on the enhanced access
       channel or the traffic channel (if the traffic channel is 
       available). If it fails to do that or the enhanced access
       channel is not available, it sends a failure event to 
       the interested clients. */

  CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE,
  /**< The CM will try to send the DBM on the access channel or the
       traffic channel (if the traffic channel is available). If it 
       fails to do that, it sends a failure event to the interested 
       clients. */

  /** @cond
  */
  CM_DBM_MAX /* <internal> */
  /** @endcond */

} cm_dbm_trans_pref_e_type;
/*~ SENTINEL cm_dbm_trans_pref_e_type.CM_DBM_MAX */

/*---------------------------------
  Possible DBM events
 ------------------------------- */

/** Enumeration of possible DBM events.
*/
typedef enum cm_dbm_event_e {
/** @cond
*/
    CM_DBM_EVENT_NONE = -1,    /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

    CM_DBM_EVENT_MO_STATUS,
      /**< Mobile Originated status event. */

    CM_DBM_EVENT_MT_MSG,
      /**< Mobile Terminated message event. */

/** @cond
*/
    CM_DBM_EVENT_MAX         /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_dbm_event_e_type;
/*~ SENTINEL cm_dbm_event_e_type.CM_DBM_EVENT_MAX */

/*---------------------------------
  Possible DBM classes
 ------------------------------- */

/** Enumeration of possible DBM classes. 
*/
typedef enum cm_dbm_class_e {
/** @cond
*/
    CM_DBM_CLASS_NONE=-1,    /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

    CM_DBM_CLASS_PD,         
      /**< Class for Position Determination. */

    CM_DBM_CLASS_SDB,
      /**< Class for Short Data Burst. */

/** @cond
*/
    CM_DBM_CLASS_MAX         /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_dbm_class_e_type;
/*~ SENTINEL cm_dbm_class_e_type.CM_DBM_CLASS_MAX */

/*---------------------------------
  Possible DBM commands
 ------------------------------- */

/** Enumeration of possible DBM commands.
*/
typedef enum cm_dbm_cmd_e {

/** @cond
*/
    CM_DBM_CMD_NONE = -1,
/** @endcond
*/

    CM_DBM_CMD_SEND_MSG,
      /**< Send message command. */

/** @cond
*/
    CM_DBM_CMD_MAX
/** @endcond
*/

} cm_dbm_cmd_e_type;
/*~ SENTINEL cm_dbm_cmd_e_type.CM_DBM_CMD_MAX */

/*---------------------------------
  Possible DBM errors
 ------------------------------- */

/** Enumeration of possible DBM errors.
*/
typedef enum cm_dbm_cmd_err_e {

/** @cond
*/
    CM_DBM_CMD_ERR_NONE=-1,    /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

    /* 0 */
    CM_DBM_CMD_ERR_NOERR,
        /**< No errors found. */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Error codes resulting from clients feeding a serving system command
       function with bad or inappropriate parameters.
    */

    CM_DBM_CMD_ERR_CLIENT_ID_P,
        /**< Bad client parameter. */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    CM_DBM_CMD_ERR_NOT_IDLE_S,
        /**< Cannot accept SMS origination while SMS state is not Idle. */

    CM_DBM_CMD_ERR_OFFLINE_S,
        /**< Phone is offline. */

    /* 4 */
    CM_DBM_CMD_ERR_CDMA_LOCK_S,
        /**< Phone is CDMA locked. */

    CM_DBM_CMD_ERR_CALL_ORIG_S,
        /**< Call origination is in progress. */

    CM_DBM_CMD_ERR_E911_MODE_S,
        /**< Phone is in E911 callback mode. Only E911 calls are allowed. */

    CM_DBM_CMD_ERR_NO_SRV_S,
        /**< Client tried to send a MO SMS but there is no service. */

    /* 8 */
    CM_DBM_CMD_ERR_NOT_ALLOWED_IN_AMPS_S,
        /**< Client tried to send but the service state is analog. */

    /* Error codes resulting from operational limitations
       of the CM. */

    CM_DBM_CMD_ERR_NO_BUF_L,
        /**< No available CM command buffers to queue the command. */

    CM_DBM_CMD_ERR_OTHER,
        /**< Any other errors not listed above. */

    CM_DBM_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
        /**< The function is unavailable due to a relevant feature being
             undefined. */

    /* 12 */
    CM_DBM_CMD_ERR_PAYLOAD_NULL_PTR_P,
        /**< Bad payload pointer. */

    CM_DBM_CMD_ERR_PAYLOAD_SIZE_P,
        /**< Bad payload size parameter. */

    CM_DBM_CMD_ERR_DDTM,
        /**< 1X in high PSIST and DO call in progress. */

/** @cond
*/
    CM_DBM_CMD_ERR_MAX    /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_dbm_cmd_err_e_type;
/*~ SENTINEL cm_dbm_cmd_err_e_type.CM_DBM_CMD_ERR_MAX */

/** Enumeration of the DBM source.
*/
typedef enum cm_dbm_source_e
{

  CM_DBM_FROM_1X,
    /**< Source is 1X. */

  CM_DBM_FROM_HDR
    /**< Source is HDR. */

} cm_dbm_source_e_type;

/** @} */ /* end_addtogroup DBM */

/** @addtogroup DBM
    @{ */

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** @brief Type for DBM command information.
*/
typedef struct cm_dbm_cmd_info_s {

  uint8                    payload[CM_DBM_MAX_PAYLOAD_SIZE];
    /**< Payload of the DBM command. */
  
  uint16                   num_bytes;
    /**< Length of the payload. */

  cm_dbm_trans_pref_e_type trans_pref;
    /**< Transmission preference whether to send on
         EACH, ACH, or set up a call. */

  uint8                    rlp_flow;
      /**< Radio Link Protocol (RLP) flow. Only used in HDR Data over 
           Signaling (DoS) messages to identify RLP. */

  uint8                    route;
      /**< Route indication. */

} cm_dbm_cmd_info_s_type;
 
/*~ FIELD cm_dbm_cmd_info_s.payload VARRAY CM_DBM_MAX_PAYLOAD_SIZE
    LENGTH cm_dbm_cmd_info_s.num_bytes */

/** @brief DBM information structure.
*/
typedef struct
{

  uint8                   burst_type;
    /**< Burst type, whether a PD or Short Data Burst (SDB)
        (e.g., CAI_POSITION_DET). */

  cm_dbm_source_e_type    burst_source;
    /**< Whether the DBM came over 1X or HDR. */

  /*---------------------------------
    The following 2 fields used for MO
    message status event.
   *------------------------------ */

  void                   *user_data;
    /**< The user data tag associated with an MO message. */

  cm_dbm_mo_status_e_type mo_status;
    /**< The status of an MO message. */

  word                    burst_overflow_size;
    /**< Number of bytes until the message exceeds its maximum allowed limit. */

  /*---------------------------------
    The following 2 fields used for receiving
    messages over CDMA.
   ------------------------------- */

  uint8                  *payload;
    /**< Pointer to data for a received message. */

  uint8                   num_bytes;
    /**< Number of bytes in the payload. */

  /*---------------------------------
    The following 2 fields are used for receiving
    MT DoS messages over HDR.
   ------------------------------- */

  void                   *dos_payload;
    /**< Pointer to a DoS payload from HDR passed as a data services management
       item. */

  uint32                 dos_num_bytes;
    /**< Number of bytes in the DoS payload. */

  cm_client_id_type      requesting_client_id;
    /**< Requesting client. */

  uint8                  rlp_flow;
    /**< Corresponding link flow number for the DoS packet
         (FEATURE_HDR_EMPA). */

  byte                   route;
    /**< Route the DoS packet is to use (FEATURE_HDR_EMPA). */


} cm_dbm_info_s_type;
 
 /*~ FIELD cm_dbm_info_s_type.payload VARRAY LENGTH cm_dbm_info_s_type.num_bytes */

/*---------------------------------------------------------------------------
   Functions
---------------------------------------------------------------------------*/

/** Pointer to a callback function for a DBM command.
*/
typedef void (cm_dbm_cmd_cb_f_type) (

    void                        * data_block_ptr,
        /**< Pointer to the client-provided data block. */

    cm_dbm_cmd_e_type           dbm_cmd,
        /**< Indicates which DBM command the dbm_cmd_err error status is for. */

    cm_dbm_cmd_err_e_type       dbm_cmd_err
        /**< Reason for the rejection. */
);

/** This callback function is used for copying the payload from
    the client's space to CM's space. Not all client are
    required to provide an implementation for this. If this is
    NULL, CM does a memcopy itself. If this not NULL, this
    function is called to copy the payload into CM's space. This
    was provided for DS to avoid multiple copying of long
    payloads.
*/
typedef int8 (cm_dbm_payload_copy_cb_f_type) (

    void                        * payload,
        /**<  Pointer to the client-provided payload input parameter. */

    byte                        * dbm_payload,
        /**<  Pointer to the memory buffer to store the payload data in
              output parameter. */

    int                         * num_bytes
        /**<  Number of bytes to copy. */
);
 
/*~ CALLBACK cm_dbm_payload_copy_cb_f_type
    ONERROR return 0 */
/*~ PARAM OUT dbm_payload VARRAY CM_DBM_MAX_PAYLOAD_SIZE LENGTH *num_bytes */
/*~ PARAM num_bytes POINTER */

/** @} */ /* end_addtogroup DBM */

/** @addtogroup DBM
    @{ */

/*===========================================================================

FUNCTION: cm_dbm_cmd_mo_msg

===========================================================================*/
/**
  Sends a data burst message and places a request on the CM queue.

@param[in] burst_type        Burst type, whether a PD or Short Data Burst
                             (SDB) (e.g., CAI_POSITION_DET).
@param[in] cmd_cb_func       Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Requesting client ID.
@param[in] payload_ptr       Pointer to the start of the message.
@param[in] num_bytes         Number of bytes in the message.
@param[in] payload_copy_func Pointer to the callback function to copy the
                             payload from the client's memory space to the
                             CM's memory space.
@param[in] l2_ack_wait_time  Maximum number of seconds to wait for
                             receiving L2 ACK.
@param[in] l3_ack_wait_time  Maximum number of seconds to wait for receiving L3
                             ACK before sending the preferred system change
                             command.
@param[in] trans_pref        Transmission preference (e.g., EACH, ACH).

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_dbm_cmd_mo_msg(
  uint8                              burst_type,
  cm_dbm_cmd_cb_f_type               cmd_cb_func,
  void                               *data_block_ptr,
  cm_client_id_type                  client_id,
  void *                             payload_ptr,
  uint16                             num_bytes,
  cm_dbm_payload_copy_cb_f_type      *payload_copy_func,
    uint8                     l2_ack_wait_time,
    uint8                     l3_ack_wait_time,
    cm_dbm_trans_pref_e_type  trans_pref

);
 
/*~ FUNCTION cm_dbm_cmd_mo_msg */
/*~ PARAM payload_copy_func POINTER */

/*===========================================================================

FUNCTION: cm_dbm_cmd_mo_msg2

===========================================================================*/
/**
  Sends a data burst message and places a request on the CM queue.

@param[in] burst_type       Burst type, whether a PD or SDB (e.g.,
                            CAI_POSITION_DET).
@param[in] cmd_cb_func      Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Requesting client ID.
@param[in] dbm_cmd_info_ptr DBM information pointer.
@param[in] l2_ack_wait_time Maximum number of seconds to wait for
                            receiving L2 ACK.
@param[in] l3_ack_wait_time Maximum number of seconds to wait for
                            receiving L3 ACK before sending the
                            preferred system change command.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().
*/
extern boolean cm_dbm_cmd_mo_msg2 (
  uint8                              burst_type,
  cm_dbm_cmd_cb_f_type               cmd_cb_func,
  void                               *data_block_ptr,
  cm_client_id_type                  client_id,
  const cm_dbm_cmd_info_s_type       *dbm_cmd_info_ptr,
  uint8                              l2_ack_wait_time,
  uint8                              l3_ack_wait_time

);
 
/*~ FUNCTION cm_dbm_cmd_mo_msg2 */
/*~ PARAM dbm_cmd_info_ptr POINTER */

/** @} */ /* end_addtogroup DBM Data Burst Messaging */

/*-----------------------------------------------------------------------------

BCMCS Object

This object implements the CM part of "High Rate BroadCast-MultiCast packet
data air interface Specification C.S0054" and "BCMCS Framework Draft Document
0.1.3 XP0019."

-----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
** Defines
**-------------------------------------------------------------------------*/

/** @addtogroup BO Broadcast and Multicast Services Object
    @{ */

/*---------------------------------------------------------------------------
** Enums
**-------------------------------------------------------------------------*/

/** Enumeration of BCMCS commands that clients are able to send
    to the CM.
*/
typedef enum cm_bcmcs_cmd_e {

/** @cond
*/
  CM_BCMCS_CMD_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_BCMCS_CMD_FLOW_REQUEST_F,
    /**< Request for flow registration. */

  CM_BCMCS_CMD_REG_REQUEST_F,
    /**< BCMCS registration request. */

  CM_BCMCS_CMD_BOM_CACHING_SETUP_F,
    /**< BCMCS strict caching setup. */

  CM_BCMCS_CMD_REG_HANDOFF_F,
    /**< Enable event-driven registrations on handoff. */
  
  CM_BCMCS_CMD_FLOW_DEACT_REQUEST_F,
    /**< Request for flow deactivation. */

/** @cond
*/
  CM_BCMCS_CMD_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_bcmcs_cmd_e_type;
/*~ SENTINEL cm_bcmcs_cmd_e_type.CM_BCMCS_CMD_MAX */

/** Enumeration of BCMCS command errors returned by
** BCMCS command callback functions.
*/
typedef enum cm_bcmcs_cmd_err_e {

/** @cond
*/
  CM_BCMCS_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_BCMCS_CMD_ERR_NOERR,
    /**< No errors found. */

  /*
     Error codes resulting from clients feeding a command
     function with bad or inappropriate parameters.
  */

  CM_BCMCS_CMD_ERR_INVALID_NUMBER_OF_FLOW_IDS_P,
    /**< Number of flow IDs is < 0 or > SYS_BCMCS_MAX_FLOWS, or the number of
         flow IDs is 0 while the pointer to the flow ID list is not NULL. 
         Returned when the client tries to queue a BCMCS command to the CM. */

  CM_BCMCS_CMD_ERR_FLOW_IDS_NULL_PTR_P,
     /**< Null pointer given as a parameter instead of a pointer to a list of
          flow IDs when the number of flow IDs is not 0. Returned when the
          client tries to queue a BCMCS command to the CM. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Error codes resulting from CM being in the wrong state
     for the command to be processed. */

  CM_BCMCS_CMD_ERR_BCMCS_SRV_NOT_AVAILABLE_S,
    /**< Returned in response to CM_BCMCS_CMD_FLOW_REQUEST_F when no BCMCS
         service is available at any of the acquired systems. \n

         @note1 This error is returned only after CM_BCMCS_CMD_FLOW_REQUEST_F 
                is processed by the CM. This is because BCMCS service may have 
                become available/not available between the time
                CM_BCMCS_CMD_FLOW_REQUEST_F was queued and when it was 
                processed by the CM. */

  /* Error codes resulting from operational limitations
     of the CM. */

  /* 4 */
  CM_BCMCS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to FEATURE_BCMCS being undefined. */

  CM_BCMCS_CMD_ERR_OTHER,
    /**< Error other than any of the above. */

/** @cond
*/
  CM_BCMCS_CMD_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_bcmcs_cmd_err_e_type;
/*~ SENTINEL cm_bcmcs_cmd_err_e_type.CM_BCMCS_CMD_ERR_MAX */

/** Enumeration of BCMCS events. Clients are able to register
    with the CM to be notified of any subsets of these events.
*/
typedef enum cm_bcmcs_event_e {

/** @cond
*/
  CM_BCMCS_EVENT_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_BCMCS_EVENT_FLOW_REQUEST_SUCCESS,
    /**< CM_BCMCS_CMD_FLOW_REQUEST_F succeeded. Sent to the client when the CM 
         receives a CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F or 
         CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F report from the HDR protocol.
         The event payload indicates the flow status for each flow that was
         registered/deregistered. */

  CM_BCMCS_EVENT_FLOW_REQUEST_FAILURE,
    /**< CM_BCMCS_CMD_FLOW_REQUEST_F failed. Sent to the client when the CM
         fails to deliver a flow registration request to HDR (currently, 
         this only happens when BCMCS service is permanently lost). The
         event payload indicates the flow status for each flow. */

  CM_BCMCS_EVENT_FLOW_STATUS_CHANGE,
    /**< HDR flow status change. Sent to the the client when the CM receives
         a CM_HDR_BCMCS_FLOW_STATUS_F report from the HDR protocol. The
         event payload indicates the flow status for each flow. */

  CM_BCMCS_EVENT_BROADCAST_RESET,
    /**< Broadcast reset. Sent to the client when the CM receives a
         CM_HDR_BCMCS_BROADCAST_RESET_F report from the HDR protocol. The
         event payload indicates the flow status for each flow. */

  /* 4 */
  CM_BCMCS_EVENT_REGISTRATION_STATUS,
    /**< HDR registration status update. Sent to the client when the CM
         receives a CM_HDR_BCMCS_REG_STATUS_F report from the HDR protocol. 
         The event payload indicates the flow registration status for each 
         flow that was registered. */

  CM_BCMCS_EVENT_FLOW_DEACT_REQUEST_SUCCESS,
     /**< Sent to the client when an MBMS deactivation confirmation
          is received. */

/** @cond
*/
  CM_BCMCS_EVENT_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_bcmcs_event_e_type;
/*~ SENTINEL cm_bcmcs_event_e_type.CM_BCMCS_EVENT_MAX */

/** @} */ /* end_addtogroup BO */

/** @addtogroup BO
    @{ */

/*---------------------------------------------------------------------------
** Datatypes
**-------------------------------------------------------------------------*/

/** @brief Type to hold BCMCS information. When notified of BCMCS
    events, clients are given a reference to such a structure.
*/
typedef struct cm_bcmcs_info_s {

  sys_sys_mode_e_type             protocol;
    /**< Indicates what protocol this event comes from. */

  uint8                           num_of_flows;
    /**< Number of entries in flow_status array. */

  sys_bcmcs_flow_status_element_s_type  flow_status_array[ SYS_BCMCS_MAX_FLOWS ];
    /**< Array of IP:port number addresses for which 
         flows should be requested. */

  cm_client_id_type               requesting_client_id;
    /**< Requesting client. */

  sys_bcmcs_reg_status_element_s_type reg_status_array[ SYS_BCMCS_MAX_FLOWS ];
    /**< List of all the flows and registration statuses. */

} cm_bcmcs_info_s_type;
 
/*~ FIELD cm_bcmcs_info_s.flow_status_array VARRAY SYS_BCMCS_MAX_FLOWS
    LENGTH cm_bcmcs_info_s.num_of_flows */
/*~ FIELD cm_bcmcs_info_s.reg_status_array VARRAY SYS_BCMCS_MAX_FLOWS
    LENGTH cm_bcmcs_info_s.num_of_flows */


/** Type for BCMCS command callback functions.
    A client that wants to be notified of the result of a BCMCS
    command must supply a pointer to a function of this type as
    the first parameter to a BCMCS command function.

@note1hang Client-provided callback functions must be re-entrant, since they
           could be called from the client and the CM context simultaneously. */
typedef void (cm_bcmcs_cmd_cb_f_type) (

  void                      *data_block_ptr,
    /**< Pointer to the client-provided data block.
    
         @note1 Depending on the client implementation of this function,
         the data block pointed to by this pointer could be accessed from
         the client and CM context simultaneously. */

  cm_bcmcs_cmd_e_type        bcmcs_cmd,
    /**< Indicates which command the bcmcs_cmd_err error status is for. */

  cm_bcmcs_cmd_err_e_type    bcmcs_cmd_err
    /**< Call command error code indicating if the command
         is rejected and the reason for the rejection. */
);

/** @} */ /* end_addtogroup BO */

/** @addtogroup BO
    @{ */

/*===========================================================================

FUNCTION: cm_bcmcs_cmd_flow_request

===========================================================================*/
/**
  Queues the CM_BCMCS_CMD_FLOW_REQUEST_F command to the CM.

  This command is used to register BCMCS data flows with the
  lower layer.

  If num_of_flows > 0 and flow_ids_array_ptr is not NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F results in an Update Flow
  Registration command being sent to the lower layer.

  If num_of_flows = 0 and flow_ids_array_ptr is NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F results in a Discontinue
  Broadcast command being sent to the lower layer.

@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] num_of_flows       Number of entries in the array in
                              flow_ids_array_ptr.
@param[in] flow_ids_array_ptr Array of IP:port number addresses for which
                              flows should be requested.

@return 
  TRUE -- Command was queued successfully to the CM. \n
  FALSE -- No CM buffer available to queue the command.

@dependencies None.
*/
boolean cm_bcmcs_cmd_flow_request(

  cm_bcmcs_cmd_cb_f_type   cmd_cb_func,
  void                     *data_block_ptr,
  cm_client_id_type        client_id,
  uint8                    num_of_flows,
  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr

);
 
/*~ FUNCTION cm_bcmcs_cmd_flow_request */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */

/*===========================================================================

FUNCTION: cm_bcmcs_cmd_flow_request2

===========================================================================*/
/**
  Queues a CM_BCMCS_CMD_FLOW_REQUEST_F command to the CM.

  This command is used to register BCMCS data flows with the lower layer.

  If num_of_flows > 0 and flow_ids_array_ptr is not NULL, queueing
  CM_BCMCS_CMD_FLOW_REQUEST_F results in an Update Flow Registration
  command being sent to the lower layer.

  If num_of_flows = 0 and flow_ids_array_ptr is NULL, queueing
  CM_BCMCS_CMD_FLOW_REQUEST_F results in a Discontinue Broadcast
  command being sent to the lower layer.

@param[in] cmd_cb_func              Client callback function.
@param[in] data_block_ptr           Pointer to the client callback data block.
@param[in] client_id                Originating client.
@param[in] num_of_flows             Number of entries in the array in
                                    flow_ids_array_ptr.
@param[in] flow_ids_array_ptr       Array of IP:port number addresses for which
                                    flows should be requested.
@param[in] flow_reg_setup_array_ptr Whether non-required registration should be
                                    allowed for each flow.

@return 
  TRUE -- Command was queued successfully to the CM. \n
  FALSE -- No CM buffer available to queue the command.

@dependencies
  None.
*/
boolean cm_bcmcs_cmd_flow_request2(

  cm_bcmcs_cmd_cb_f_type           cmd_cb_func,
  void                             *data_block_ptr,
  cm_client_id_type                client_id,
  uint8                            num_of_flows,
  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr,
  const sys_bcmcs_reg_setup_e_type *flow_reg_setup_array_ptr

);
 
/*~ FUNCTION cm_bcmcs_cmd_flow_request2 */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */
/*~ PARAM flow_reg_setup_array_ptr VARRAY LENGTH num_of_flows */

/*===========================================================================

FUNCTION: cm_bcmcs_cmd_registration_request

===========================================================================*/
/**
  Queues a CM_BCMCS_CMD_REG_REQUEST_F command to the CM.

  This command is used to register BCMCS data flows with the lower layer.

@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] num_of_flows       Number of entries in the array in
                              flow_ids_array_ptr.
@param[in] flow_ids_array_ptr Array of IP:port number addresses for which
                              flows should be registered.

@return 
  TRUE -- Command was queued successfully to the CM. \n
  FALSE -- No CM buffer available to queue the command.

@dependencies
  None.
*/
boolean cm_bcmcs_cmd_registration_request(

  cm_bcmcs_cmd_cb_f_type           cmd_cb_func,
  void                             *data_block_ptr,
  cm_client_id_type                client_id,
  uint8                            num_of_flows,
  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr

);
 
/*~ FUNCTION cm_bcmcs_cmd_registration_request */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */
 
/*===========================================================================

FUNCTION: cm_bcmcs_cmd_registration_handoff

===========================================================================*/
/**
  Queues a CM_BCMCS_CMD_REG_HANDOFF_F command to the CM.

  This command is used to enable event-driven registrations on handoff.

@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] num_of_flows       Number of entries in the array in
                              flow_ids_array_ptr.
@param[in] flow_ids_array_ptr Array of IP:port number addresses for which
                              flows should be registered.

@return 
  TRUE -- Command was queued successfully to the CM. \n
  FALSE -- No CM buffer available to queue the command or invalid parameters
  passed to the function.

@dependencies
  None.
*/
boolean cm_bcmcs_cmd_registration_handoff(

  cm_bcmcs_cmd_cb_f_type           cmd_cb_func,
  void                             *data_block_ptr,
  cm_client_id_type                client_id,
  uint8                            num_of_flows,
  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr

);
 
/*~ FUNCTION cm_bcmcs_cmd_registration_handoff */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */

/*===========================================================================

FUNCTION: cm_mbms_cmd_flow_deact_request

===========================================================================*/
/**
  Deactivates MBMS data flow requests with the lower layers.
  
@param[in] cmd_cb_func        Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] num_of_flows       Number of entries in the array in
                              flow_ids_array_ptr.
@param[in] flow_ids_array_ptr Array of IP:port number addresses for which
                              flows should be requested.

@return 
  TRUE -- Command was queued successfully to the CM. \n
  FALSE -- No CM buffer available to queue the command.

@dependencies
  None.
*/
boolean cm_mbms_cmd_flow_deact_request(

  cm_bcmcs_cmd_cb_f_type           cmd_cb_func,
  void                             *data_block_ptr,
  cm_client_id_type                client_id,
  uint8                            num_of_flows,
  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr

);
 
/*~ FUNCTION cm_mbms_cmd_flow_deact_request */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */

/** @} */ /* end_addtogroup BO BCMCS Object */

/*-----------------------------------------------------------------------------

MBMS Object

This object implements CM part of "Multicast Broadcast Multimedia Services"

-----------------------------------------------------------------------------*/

/** @addtogroup MBMS Multicast Broadcast Multimedia Service Object
    @{ */

/*--------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
** Enums
** ------------------------------------------------------------------------*/

/** Enumeration of MBMS commands that clients are able to send
    to the CM.
*/
typedef enum cm_mbms_cmd_e {

/** @cond
*/
  CM_MBMS_CMD_NONE = -1,  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_MBMS_CMD_GENERIC,
    /**< Generic MBMS command. */

/** @cond
*/
  CM_MBMS_CMD_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_mbms_cmd_e_type;
/*~ SENTINEL cm_mbms_cmd_e_type.CM_MBMS_CMD_MAX */

/** Enumeration for MBMS command errors to be sent to clients
    when command errors are detected by the CM. Clients must
    check for CM_API_MBMS before using this enum.
*/
typedef enum cm_mbms_cmd_err_e {

/** @cond
*/
  CM_MBMS_CMD_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_MBMS_CMD_ERR_NOERR,
    /**< No errors found. */

  CM_MBMS_CMD_ERR_INVALID_COMMAND,
    /**< Command requested by the client in mbms_generic_cmd is invalid. */

  CM_MBMS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  CM_MBMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to FEATURE_MODEM_MBMS being undefined. */

  CM_MBMS_CMD_ERR_MBMS_SRV_UNAVAILABLE,
    /**< MBMS service unavailable. */

  /* 5 */ 
  CM_MBMS_CMD_ERR_OTHER,
    /**< Error other than any of the above. */

/** @cond
*/
  CM_MBMS_CMD_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_mbms_cmd_err_e_type;
/*~ SENTINEL cm_mbms_cmd_err_e_type.CM_MBMS_CMD_ERR_MAX */

/** Enumeration of MBMS events. Clients are able to register
    with the CM to be notified of any subsets of these events.
    Clients must check for CM_API_MBMS before using this enum.
*/
typedef enum cm_mbms_event_e {

/** @cond
*/
  CM_MBMS_EVENT_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_MBMS_EVENT_GENERIC_CMD,
    /**< This is sent to registered clients whenever
         an MBMS command is issued to the CM. */

  CM_MBMS_EVENT_GENERIC_RPT,
    /**< This is sent to registered clients whenever
         lower layers send an MBMS report to the CM. */

/** @cond
*/
  CM_MBMS_EVENT_MAX  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_mbms_event_e_type;
/*~ SENTINEL cm_mbms_event_e_type.CM_MBMS_EVENT_MAX */

/** @} */ /* end_addtogroup MBMS */

/** @addtogroup MBMS
    @{ */

/*--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/** @brief Struct to hold MBMS commands sent to the lower layers. 
    When notified of MBMS events, clients are given a reference
    to such a structure. Clients must check for CM_API_MBMS
    before using this struct.
*/
typedef struct cm_mbms_generic_cmd_s
{

   sys_gw_mbms_req_e_type                          cmd_type;
     /**< MBMS command being sent by the client. */

     /** @brief Union of generic MBMS commands. */
   union {
     sys_gw_mbms_activate_req_s_type               activate_req;
     sys_gw_mbms_deactivate_req_s_type             deactivate_req;
     sys_gw_mbms_session_complete_ind_s_type       session_complete_ind;
     sys_gw_mbms_abort_req_s_type                  abort_req;
     sys_gw_mbms_suspend_req_s_type                suspend_req;
     sys_gw_mbms_resume_req_s_type                 resume_req;
     sys_gw_mbms_avail_srv_list_req_s_type         avail_srv_list_req;
     sys_gw_mbms_capability_req_s_type             capability_req;
     sys_gw_mbms_set_attr_req_s_type               set_attr;                    
    
   }cmd;   
} cm_mbms_generic_cmd_s_type ;

 
/*~ FIELD cm_mbms_generic_cmd_s.cmd DISC cm_mbms_generic_cmd_s.cmd_type */
/*~ CASE SYS_GW_MBMS_REQ_ACTIVATE  cm_mbms_generic_cmd_s.cmd.activate_req */
/*~ CASE SYS_GW_MBMS_REQ_DEACTIVATE  cm_mbms_generic_cmd_s.cmd.deactivate_req */
/*~ CASE SYS_GW_MBMS_REQ_SESSION_COMPLETE_IND  cm_mbms_generic_cmd_s.cmd.session_complete_ind */
/*~ CASE SYS_GW_MBMS_REQ_ABORT  cm_mbms_generic_cmd_s.cmd.abort_req */
/*~ CASE SYS_GW_MBMS_REQ_SUSPEND  cm_mbms_generic_cmd_s.cmd.suspend_req */
/*~ CASE SYS_GW_MBMS_REQ_RESUME  cm_mbms_generic_cmd_s.cmd.resume_req */
/*~ CASE SYS_GW_MBMS_REQ_AVAIL_SERVICES_LIST  cm_mbms_generic_cmd_s.cmd.avail_srv_list_req */
/*~ CASE SYS_GW_MBMS_REQ_GET_CAPABILITY cm_mbms_generic_cmd_s.cmd.capability_req */
/*~ CASE SYS_GW_MBMS_REQ_SET_ATTR  cm_mbms_generic_cmd_s.cmd.set_attr */

/** @brief Struct to hold MBMS report parameters sent by lower layers.
*/
typedef struct cm_mbms_generic_rpt_s
{

  sys_gw_mbms_resp_e_type   rpt_type;
    /**< MBMS report type. */

    /** Union of generic MBMS responses.
  */
  union{
    sys_gw_mbms_resp_activate_cnf_s_type            activate_cnf;
    sys_gw_mbms_resp_deactivate_cnf_s_type          deactivate_cnf;
    sys_gw_mbms_resp_service_ind_s_type             service_ind;
    sys_gw_mbms_resp_capability_ind_s_type          capability_ind;
    sys_gw_mbms_resp_context_activated_ind_s_type   context_activated_ind;
    sys_gw_mbms_resp_context_deactivated_ind_s_type context_deactivated_ind;
    sys_gw_mbms_resp_abort_cnf_s_type               abort_cnf;
    sys_gw_mbms_resp_suspend_cnf_s_type             suspend_cnf;
    sys_gw_mbms_resp_resume_cnf_s_type              resume_cnf;
    sys_gw_mbms_resp_avail_srv_list_cnf_s_type      avail_srv_list_cnf;
    sys_gw_mbms_resp_ll_ready_ind_s_type            ll_ready_ind;
    sys_gw_mbms_resp_set_attr_s_type                set_attr;                    

  }rpt;
} cm_mbms_generic_rpt_s_type;

 
/*~ FIELD cm_mbms_generic_rpt_s.rpt DISC cm_mbms_generic_rpt_s.rpt_type */
/*~ CASE SYS_GW_MBMS_RESP_ACTIVATE_CNF  cm_mbms_generic_rpt_s.rpt.activate_cnf */
/*~ CASE SYS_GW_MBMS_RESP_DEACTIVATE_CNF  cm_mbms_generic_rpt_s.rpt.deactivate_cnf */
/*~ CASE SYS_GW_MBMS_RESP_SERVICE_IND  cm_mbms_generic_rpt_s.rpt.service_ind */
/*~ CASE SYS_GW_MBMS_RESP_CAPABILITY_IND  cm_mbms_generic_rpt_s.rpt.capability_ind */
/*~ CASE SYS_GW_MBMS_RESP_CONTEXT_ACTIVATED_IND cm_mbms_generic_rpt_s.rpt.context_activated_ind */
/*~ CASE SYS_GW_MBMS_RESP_CONTEXT_DEACTIVATED_IND  cm_mbms_generic_rpt_s.rpt.context_deactivated_ind */
/*~ CASE SYS_GW_MBMS_RESP_ABORT_CNF  cm_mbms_generic_rpt_s.rpt.abort_cnf */
/*~ CASE SYS_GW_MBMS_RESP_SUSPEND_CNF  cm_mbms_generic_rpt_s.rpt.suspend_cnf */
/*~ CASE SYS_GW_MBMS_RESP_RESUME_CNF  cm_mbms_generic_rpt_s.rpt.resume_cnf */
/*~ CASE SYS_GW_MBMS_RESP_AVAIL_SERVICES_LIST_CNF  cm_mbms_generic_rpt_s.rpt.avail_srv_list_cnf */
/*~ CASE SYS_GW_MBMS_RESP_LL_READY_IND  cm_mbms_generic_rpt_s.rpt.ll_ready_ind */
/*~ CASE SYS_GW_MBMS_RESP_SET_ATTR  cm_mbms_generic_rpt_s.rpt.set_attr */
 
/** @brief Type for Multimedia Broadcast/Multimedia Services (MBMS)
    information.
*/
typedef struct cm_mbms_info_s {

  cm_mbms_generic_rpt_s_type     mbms_generic_rpt;
    /**< MBMS generic report structure. */

    cm_mbms_generic_cmd_s_type     mbms_generic_cmd;
    /**< MBMS generic command structure. */

    cm_client_id_type               requesting_client_id;
    /**< Requesting client. */

} cm_mbms_info_s_type;

/*--------------------------------------------------------------------------
** Macros
** ------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------*/

/*  CM command functions.

    The CM command functions define the client->CM interface.

    Each command function takes a pointer to a client-provided
    callback function, a client-provided void pointer, and the
    command's required set of fields.

    If the command function finds one or more of the parameters
    to contain invalid values (e.g., not one of the enum values
    for an enum parameter), it calls the client's callback
    function IMMEDIATELY with the appropriate command error
    code. In this case the callback function is actually being
    called from the client context, NOT the CM.

    If all parameters found to be valid, the command function
    queues a command to the CM. When the CM gets the command, it
    checks if the command is valid relative to the current
    state of the system.

    If the command is not valid, the CM calls the client
    callback function with the appropriate command error code.
    In this case, the callback function is being called from the
    CM context.

    If the command is successful, the CM calls the client
    callback function with an error code that indicates a
    successful command. In addition, the CM notifies registered
    clients of the corresponding CM event.
*/

/** Type for  MBMS command callback functions. 
    A client that wants to be notified of the result of an MBMS
    command must supply a pointer to a function of this type as
    the first parameter to an MBMS command function.

@note1hang Client-provided callback functions must be re-entrant,
    since they could be called from the client and the CM
    context simultaneously. Clients must check for CM_API_MBMS
    before using this function. */
typedef void (cm_mbms_cmd_cb_f_type) (

  void                      *data_block_ptr,
    /**< Pointer to the client-provided data block. \n
         @note1 Depending on the client implementation of this function,
         the data block pointed to by this pointer could be accessed from
         the client and the CM context simultaneously. */

  cm_mbms_cmd_e_type        mbms_cmd,
    /**< Indicates which MBMS request the mbms_cmd_err error status is for. */

  cm_mbms_cmd_err_e_type     mbms_cmd_err
    /**< Call command error code indicating if the command
         was rejected and the reason for the rejection. */
);

/** @} */ /* end_addtogroup MBMS */

/** @addtogroup MBMS
    @{ */

/*===========================================================================

FUNCTION: cm_mbms_cmd_generic

===========================================================================*/
/**
  Used by MBMS applications to send commands to the MBMS service
  manager. Clients should fill in the appropriate MBMS request
  type and provide the corresponding parameters in the
  mbms_generic_req struct. Clients must check for CM_API_MBMS
  before using this function

@param[in] cmd_cb_func      Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Originating client.
@param[in] mbms_generic_cmd Structure to hold MBMS commands and their
                            parameters.

@return 
  TRUE -- Command was queued successfully to the CM. \n
  FALSE -- No CM buffer available to queue the command.

@dependencies
  cm_init_before_task_start() must have already been called.
*/
boolean cm_mbms_cmd_generic (

  cm_mbms_cmd_cb_f_type               cmd_cb_func,
  void                                *data_block_ptr,
  const cm_client_id_type             client_id,
  const cm_mbms_generic_cmd_s_type    *mbms_generic_cmd

);
 
/*~ FUNCTION cm_mbms_cmd_generic */
/*~ PARAM mbms_generic_cmd POINTER */


/** @} */ /* end_addtogroup MBMS Multicast Broadcast Multimedia Service Object */

/** @addtogroup STATS Statistics Object
    @{ */

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Enumeration of statistics events.
    Clients are able to register with CM to be notified of any subsets
    of these Statistics events.
*/
typedef enum cm_stats_event_e {
  /** @cond
 */
  CM_STATS_EVENT_NONE = -1,   /* internal */
  /** @endcond
 */

  /* 0 */
  CM_STATS_EVENT_MODEM_INFO,
    /**< Statistics event for clients. */
  
  /** @cond
 */
  CM_STATS_EVENT_MAX   /* internal */
  /** @endcond
 */
}cm_stats_event_e_type;
/*~ SENTINEL cm_stats_event_e_type.CM_STATS_EVENT_MAX */

/** Command type.
*/
typedef enum cm_stats_cmd_e {
  /** @cond
  */
  CM_STATS_CMD_NONE=-1,  /* Internal usage. */
  /** @endcond
  */

  /* 0 */
  CM_STATS_CMD_SET_MODEM_INFO,
   /**< Command to set required modem information mask. */
  
  /** @cond
 */
  CM_STATS_CMD_MAX   /* Internal usage. */
  /** @endcond 
*/
} cm_stats_cmd_e_type;
/*~ SENTINEL cm_stats_cmd_e_type.CM_STATS_CMD_MAX */

/** Phone command error code.
*/
typedef enum cm_stats_cmd_err_e {
  /** @cond
 */
  CM_STATS_CMD_ERR_NONE=-1,   /* internal usage */
  /** @endcond
 */
  
/* 0 */
  CM_STATS_CMD_ERR_NOERR,
    /**< No errors found. */

  /* 1 */
  CM_STATS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command. */

  /* 2 */
  CM_STATS_CMD_ERR_INVALID_TYPE,
   /**< Type specified is not a valid module (NAS/RRC). */
  
  /** @cond
 */
  CM_STATS_CMD_ERR_MAX   /* internal usage */
  /** @endcond
 */
} cm_stats_cmd_err_e_type;
/*~ SENTINEL cm_stats_cmd_err_e_type.CM_STATS_CMD_ERR_MAX */

/** @} */ /* end_addtogroup STATS */

/** @addtogroup STATS Statistics Object
    @{ */
/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

#define CM_STATS_CMD_PARAM_DEFAULT_VALUE      0
  /**< Default parameter value for statistics. */

/** @} */ /* end_addtogroup STATS */

/** @addtogroup STATS Statistics Object
    @{ */

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/** Type for statistics command error callback function.
    A client that wants to be notified of other clients' statistics command
    errors must register for such a function with the CM.
*/
typedef void (cm_stats_cmd_cb_f_type) (

  void                      *data_block_ptr,
    /**< Pointer to the client-provided data block. */
  
  cm_stats_cmd_e_type        cmd,
    /**< Indicate for which phone command this error is reported. */

  cm_stats_cmd_err_e_type    cmd_err
    /**< Indicate nature of error. */

);

/*==========================================================================

FUNCTION: cm_stats_set_modem_info_mask

==========================================================================*/
/**
  Commands the CM to change the Modem Statistics information bitmask.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Originating client.
@param[in] module_type    Module for which the mask is being set.
@param[in] bit_mask       Bitmask value.

@dependencies
  cm_init_before_task_start() must have already been called.

@return 
  TRUE -- request sent. \n
  FALSE -- No buffer to send request

@sideeffects
  If the command is successful, CM clients will be notified of the
  corresponding CM event.
*/
extern boolean cm_stats_set_modem_info_mask (

   cm_stats_cmd_cb_f_type            cmd_cb_func,
   void                              *data_block_ptr,
   cm_client_id_type                 client_id,
   sys_modem_module_e_type           module_type,
   uint64                            bit_mask

);
/*~ FUNCTION cm_stats_set_modem_info_mask */

/*==========================================================================

FUNCTION: cm_stats_set_modem_info_signal

==========================================================================*/
/**
  Commands the CM to set the Modem Statistics information signal.

  Protocol layers (NAS/RR/RRC) will call this API to set the
  respective signal.

@param[in] module_type Module for which the mask is being set.

@dependencies
  None.  

@return 
  TRUE -- Request ACK. \n
  FALSE -- No buffer to send request.

@sideeffects
  If the command is successful, CM clients will be notified of the
  corresponding CM event.

*/
extern boolean cm_stats_set_modem_info_signal (

   sys_modem_module_e_type           module_type
   
);
/*~ FUNCTION cm_stats_set_modem_info_signal */

/** @} */ /* end_addtogroup STATS */


/*-----------------------------------------------------------------------------

Client Object

-----------------------------------------------------------------------------*/

/** @addtogroup CO Client Object
    @{ */

/*---------------------------------------------------------------------------
** Defines
**-------------------------------------------------------------------------*/

#define CM_CLIENT_ID_ANONYMOUS  (cm_client_id_type) (-1)
  /**< Anonymous client ID to be used by clients that are not
       interested in being notified of CM events and therefore do
       not allocate an object of the type cm_client_s_type in their
       own program space. */

/*---------------------------------------------------------------------------
** Enums
**-------------------------------------------------------------------------*/

/** Clients are predefined. Floating types are provided
    for clients that use and release the CM as needed.
*/
typedef enum cm_client_type_e {

/** @cond
*/
  CM_CLIENT_TYPE_NONE = -1,  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/
  /* 0 */
  CM_CLIENT_TYPE_UI,
    /**< Reserved for UI running on the same processor as the CM. */

  CM_CLIENT_TYPE_SMS,
    /**< Reserved for SMS running on the same processor as the CM. */

  CM_CLIENT_TYPE_DS,
    /**< Reserved for Data Services running on the same processor as the CM. */

  CM_CLIENT_TYPE_PD,
    /**< Reserved for Position Determination client on the same processor as the CM. */

  /* 4 */
  CM_CLIENT_TYPE_ATCOP,
    /**< Reserved for DS Access Terminal Command Processor (ATCoP) client on the
         same procesor as the CM. */

  CM_CLIENT_TYPE_CAT_APP,
    /**< Reserved for USIM Application Toolkit (USAT)/CDMA Analisys Toolkit
       (CAT) application client on the same procesor as the CM. */

  CM_CLIENT_TYPE_GSTK,
    /**< Reserved for Generic SIM application Toolkit (GSTK) client on the same
         procesor as the CM. */

  CM_CLIENT_TYPE_FLOATING1,
    /**< Floating client 1 running on the same/remote processor as the CM. */

  /* 8 */
  CM_CLIENT_TYPE_FLOATING2,
    /**< Floating client 2 running on the same/remote processor  as the CM. */

  CM_CLIENT_TYPE_FLOATING3,
    /**< Floating client 3 running on the same/remote processor as the CM. */

  CM_CLIENT_TYPE_FLOATING4,
    /**< Floating client 4 running on the same/remote processor as the CM. */

  CM_CLIENT_TYPE_DIAG,
    /**< Client used for diagnostic logging running on the same processor as the CM. */

  /* 12 */
  CM_CLIENT_TYPE_BREW,
    /**< Reserved for a BREW interface to the CM. */

  CM_CLIENT_TYPE_BLUETOOTH,
    /**< Reserved for a Bluetooth&reg;&nbsp; application. */

  CM_CLIENT_TYPE_HDR_AN,
    /**< Reserved for HDR AN authentication client on the same procesor as the CM. */

  CM_CLIENT_TYPE_CAMERA,
    /**< Reserved for camera. */

  /* 16 */
  CM_CLIENT_TYPE_CMHM,
    /**< Reserved for the CM Handover Manager. */

  CM_CLIENT_TYPE_RM_NETWORK_INTERFACE,
    /**< Reserved for a Resource Manager interface object. */

  CM_CLIENT_TYPE_DS_BCMCS,
    /**< Reserved for a DS BCMCS client. */

  CM_CLIENT_TYPE_MFLO,
    /**< Reserved for MediaFLO&tm;. */

  /* 20 */
  CM_CLIENT_TYPE_OEMTELEPHONE,
    /**< Reserved for OEMTelephone. */

  CM_CLIENT_TYPE_PBM,
    /**< Phone Book Manager. Check for CM_API_EMERGENCY_NUM_LIST before using
         this enum. */

/** @cond
*/
  CM_CLIENT_TYPE_END_LEGACY,  /* FOR INTERNAL CM USE ONLY!
         All new client types should be added AFTER this. */
/** @endcond
*/

  CM_CLIENT_TYPE_GSDI,
    /**< Reserved for a Generic SIM Driver Interface (GSDI). */

  /* 24 */
  CM_CLIENT_TYPE_BREW_APPS,
    /**< Reserved for BREW applications. */

  CM_CLIENT_TYPE_OEMNET,
    /**< Reserved for OEMNet. */

  CM_CLIENT_TYPE_HOMER,
    /**< Reserved for a WLAN homer. */

  CM_CLIENT_TYPE_CNE,
  /**< Reserved for the Connection Engine. */
    
  /* 28 */
  CM_CLIENT_TYPE_QCHAT,
  /**< Reserved for QChat. */

/** @cond
*/
  CM_CLIENT_TYPE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_client_type_e_type;
/*~ SENTINEL cm_client_type_e_type.CM_CLIENT_TYPE_MAX */


/** Enumeration of client API return status.
*/
typedef enum cm_client_status_e {

/** @cond
*/
  CM_CLIENT_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  /* 0 */
  CM_CLIENT_OK,
    /**< No error; request was completed successfully. */

  CM_CLIENT_ERR_CLIENT_ID_PTR,
    /**< Invalid client ID pointer. */

  CM_CLIENT_ERR_CLIENT_ID,
    /**< Invalid client ID. */

  CM_CLIENT_ERR_EVENT_REG_TYPE,
    /**< Invalid event registration type. */

  /* 4 */
  CM_CLIENT_ERR_FROM_EVENT,
    /**< Invalid From event. */

  CM_CLIENT_ERR_TO_EVENT,
    /**< Invalid To event. */

  CM_CLIENT_ERR_CLIENT_INIT_FAIL,
    /**< Unable to initialize the client. */

  CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to a relevant feature being undefined. */

  /* 8 */
  CM_CLIENT_ERR_PWR_COLLAPSE_TYPE,
    /**< Invalid power collapse type. */

  CM_CLIENT_ERR_PROC_ID,
    /**< Invalid processor ID. */

/** @cond
*/
  CM_CLIENT_ERR_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_client_status_e_type;
/*~ SENTINEL cm_client_status_e_type.CM_CLIENT_ERR_MAX */


/** Enumeration of client event registration type.
*/
typedef enum cm_client_event_reg_e {

/** @cond
*/
  CM_CLIENT_EVENT_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_CLIENT_EVENT_REG,
    /**< Event registration. */

  CM_CLIENT_EVENT_DEREG,
    /**< Event deregistration. */

/** @cond
*/
  CM_CLIENT_EVENT_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_client_event_reg_e_type;
/*~ SENTINEL cm_client_event_reg_e_type.CM_CLIENT_EVENT_MAX */


/** SIP response codes, from RFC 3261 to the SIP: Session Initiation Protocol
    standard from June 2002.
*/
typedef enum
{

/** @cond
*/
  CM_IPAPP_CAUSE_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_IPAPP_CAUSE_NORMAL = 0,
    /**< Indicates a user-requested call end. Cause is also used when
         an operation completes successfully. */

  CM_IPAPP_CAUSE_GENERAL_FAILURE = 1,
    /**< Used by the CM and applications to map end causes
         that are not listed in this enumeration. */

  /* Call end cause due to redirection. */

  CM_IPAPP_CAUSE_MULTIPLE_CHOICES  = 300,
    /**< Request resolved in several choices, each with its own specific
         location. Users can select from these to redirect requests. */

  CM_IPAPP_CAUSE_MOVED_PERMANENTLY = 301,
    /**< User is no longer at the requested address and the
         client should retry at the new given address. */


  CM_IPAPP_CAUSE_MOVED_TEMPORARILY = 302,
    /**< Requesting client should retry request at the new given address. 
         The Expires header field shows the expiration time. */


  CM_IPAPP_CAUSE_USE_PROXY = 305,
    /**< Requested resource must be accessed
         through a proxy given by the contact field. */


  CM_IPAPP_CAUSE_ALTERNATE_SERVICE = 380,
    /**< Call was not successful, but alternate services are possible. */


  /* Call end due to Request Failure 4xx. */

  CM_IPAPP_CAUSE_BAD_REQUEST  = 400,
    /**< Bad syntax in the request. */

  CM_IPAPP_CAUSE_UNAUTHORIZED = 401,
    /**< Request requires user authentication. */

  CM_IPAPP_CAUSE_PAYMENT_REQUIRED = 402,
    /**< Future use. */

  CM_IPAPP_CAUSE_FORBIDDEN        = 403,
    /**< Server refusing to fulfill a request. The request should not be
         repeated. */

  CM_IPAPP_CAUSE_NOT_FOUND        = 404,
    /**< User does not exist at the domain specified, or the domain 
         in the requested URI does not match domains handled by the
         recipient of the request. */

  CM_IPAPP_CAUSE_METHOD_NOT_ALLOWED = 405,
    /**< Method requested in the address line not allowed for the address 
         identified by the request-URI. */

  CM_IPAPP_CAUSE_NOT_ACCEPTABLE     = 406,
    /**< Resource identified by the request can only generate a Content
         Not Acceptable response. */

  CM_IPAPP_CAUSE_PROXY_AUTHENTICATION_REQUIRED = 407,
    /**< Client must first authenticate with the proxy. */

  CM_IPAPP_CAUSE_REQUEST_TIMEOUT = 408,
    /**< Server could not respond within the required amount of time. */

  CM_IPAPP_CAUSE_GONE = 410,
    /**< Requested resource no longer available
         at the server, and no forwarding address. */

  CM_IPAPP_CAUSE_REQUEST_ENTITY_TOO_LARGE = 413,
    /**< Request entity body is larger than what the
         server is willing to process. */

  CM_IPAPP_CAUSE_REQUEST_URI_TOO_LARGE    = 414,
    /**< Server refusing to service since request-URI is longer
         than what the server is willing to interpret. */

  CM_IPAPP_CAUSE_UNSUPPORTED_MEDIA_TYPE   = 415,
    /**< Message body of the request not in a format supported
         by the server for the requested method. */

  CM_IPAPP_CAUSE_UNSUPPORTED_URI_SCHEME   = 416,
    /**< Unsupported URI scheme. */

  CM_IPAPP_CAUSE_BAD_EXTENSION            = 420,
    /**< Server did not understand the protocol extension specified
         in the Proxy-Required or Require header fields. */

  CM_IPAPP_CAUSE_EXTENSION_REQUIRED       = 421,
    /**< Extension to process a request is not listed
         in the supported header field in the request. */

  CM_IPAPP_CAUSE_INTERVAL_TOO_BRIEF       = 422,
    /**< Expiration time of the resource refreshed by the
         request is too short. */

  CM_IPAPP_CAUSE_TEMPORARILY_UNAVAILABLE  = 480,
    /**< End system contacted but callee is currently
         unavailable. */

  CM_IPAPP_CAUSE_CALL_OR_TRANS_DOES_NOT_EXIST = 481,
    /**< Request received by the User Agent Server (UAS) does not match
         any existing dialog or transaction. */

  CM_IPAPP_CAUSE_LOOP_DETECTED              = 482,
    /**< Server detected a loop. */

  CM_IPAPP_CAUSE_TOO_MANY_HOPS              = 483,
    /**< Request received has Max-Forwards header field at 0. */

  CM_IPAPP_CAUSE_ADDRESS_INCOMPLETE         = 484,
    /**< Request had incomplete URI. */

  CM_IPAPP_CAUSE_AMBIGUOUS                  = 485,
    /**< Request URI was ambiguous. */

  CM_IPAPP_CAUSE_BUSY_HERE                  = 486,
    /**< Callee is unwilling or unable to take
         additional calls at this end system. */

  CM_IPAPP_CAUSE_REQUEST_TERMINATED         = 487,
    /**< Request terminated by a Bye/Cancel. */

  CM_IPAPP_CAUSE_NOT_ACCEPTABLE_HERE        = 488,
    /**< Resource requested by request-URI not acceptable. */

  CM_IPAPP_CAUSE_REQUEST_PENDING            = 491,
    /**< Request was received by UAS that had a pending
         request within the same dialog. */

  CM_IPAPP_CAUSE_UNDECIPHERABLE             = 493,
    /**< Request has an encrypted Multipurpose Internet Mail Extension (MIME)
         body for which the recipient does not possess an appropriate
         decryption key. */

  /* Call end due to Server Failure 5xx. */

  CM_IPAPP_CAUSE_SERVER_INTERNAL_ERROR      = 500,
    /**< Server encountered an unexpected condition that
         prevented it from fulfilling the request. */

  CM_IPAPP_CAUSE_NOT_IMPLEMENTED            = 501,
    /**< Server does not support the functionality to
         fulfill the request. */

  CM_IPAPP_CAUSE_BAD_GATEWAY                = 502,
    /**< Server received an invalid response from a
         downstream gateway. */

  CM_IPAPP_CAUSE_SERVER_UNAVAILABLE         = 503,
    /**< Server is temporarily unable to process the
         request due to overloading or maintenance. */

  CM_IPAPP_CAUSE_SERVER_TIME_OUT            = 504,
    /**< Server did not receive a timely response from
         the external server it accessed. */

  CM_IPAPP_CAUSE_VERSION_NOT_SUPPORTED      = 505,
    /**< Server does not support the SIP protocol version
         used in the request. */

  CM_IPAPP_CAUSE_MESSAGE_TOO_LARGE          = 513,
    /**< Server unable to process the request since the message
         length exceeded capabilities. */

  /* Call end due to Global Failure 6xx. */

  CM_IPAPP_CAUSE_BUSY_EVERYWHERE            = 600,
    /**< Callee's end system was contacted, but the callee
         is busy and does not wish to take call. */

  CM_IPAPP_CAUSE_DECLINE                    = 603,
    /**< Callee's end system was contacted, but the user
         explicitly does not wish to or cannot participate. */

  CM_IPAPP_CAUSE_DOES_NOT_EXIST_ANYWHERE    = 604,
    /**< Server has information that the user indicated in
         the request-URI does not exist anywhere. */

  CM_IPAPP_CAUSE_SESS_DESCR_NOT_ACCEPTABLE  = 605,
    /**< User's agent contacted, but some aspects of
         the session description not acceptable. */

/** @cond
*/
  CM_IPAPP_CAUSE_MAX = 0x7FFFFFFF    /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_ipapp_cause_e_type;

/** Number conversion request types.
*/
typedef enum cmutil_num_conv_req_e {

/** @cond
*/
  CMUTIL_NUM_CONV_REQ_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CMUTIL_NUM_CONV_REQ_NO_REQ,
    /**< Request to perform no conversion at all. */

  CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS,
    /**< Request to convert from plus to digits. */

  CMUTIL_NUM_CONV_REQ_DIGITS_TO_PLUS,
    /**< Request to convert from digits to plus. */

/** @cond
*/
  CMUTIL_NUM_CONV_REQ_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cmutil_num_conv_req_e_type;
/*~ SENTINEL cmutil_num_conv_req_e_type.CMUTIL_NUM_CONV_REQ_MAX */


/** Result of resolving current country status.
*/
typedef enum  {

/** @cond
*/
  CMUTIL_CURRENT_COUNTRY_NONE = -1, /* FOR INTERNAL CM USE ONLY! Bounds checking. */
/** @endcond
*/

  CMUTIL_CURRENT_COUNTRY_RESOLVED,
    /**< Current country could be identified. */

  CMUTIL_CURRENT_COUNTRY_CONFLICT,
    /**< Current country resolution ends up with two or more countries. */

  CMUTIL_CURRENT_COUNTRY_FAILED,
    /**< Current country could be not identified. */

/** @cond
*/
  CMUTIL_CURRENT_COUNTRY_MAX  /* FOR INTERNAL CM USE ONLY! Bounds checking. */
/** @endcond
*/

}cmutil_current_country_e_type;
/*~ SENTINEL cmutil_current_country_e_type.CMUTIL_CURRENT_COUNTRY_MAX */


/** Result of outgoing number conversion with plus.
*/
typedef enum {

/** @cond
*/
  CMUTIL_CONVER_RESULT_NONE = -1, /* FOR INTERNAL CM USE ONLY! Bounds checking. */
/** @endcond
*/

  CMUTIL_CONVER_RESULT_NUM_UNCHANGED = 0,
    /**< Number is unchanged. */

  CMUTIL_CONVER_RESULT_NUM_MODIFIED,
    /**< Number has been modified. */

  CMUTIL_CONVER_RESULT_NUM_INPUT_ERR,
    /**< Error in the number given as Input. */

/** @cond
*/
  CMUNTIL_CONVER_RESULT_MAX  /* FOR INTERNAL CM USE ONLY! Bounds checking. */
/** @endcond
*/

} cmutil_conver_result_e_type;
/*~ SENTINEL cmutil_conver_result_e_type.CMUNTIL_CONVER_RESULT_MAX */

/** Result of country information request.
*/
typedef enum {

/** @cond
*/
  CMUTIL_COUNTRY_INFO_RESULT_NONE = -1, /* FOR INTERNAL CM USE ONLY!
  Bounds checking. */
/** @endcond
*/

  CMUTIL_COUNTRY_INFO_RESULT_FOUND = 0,
    /**< Mobile Country Code (MCC) is found in the country table. */

  CMUTIL_COUNTRY_INFO_RESULT_NOT_FOUND = 1,
    /**< MCC is not found in the country table. */

  CMUTIL_COUNTRY_INFO_RESULT_MCC_ERR = 2,
    /**< Error in the MCC. */

/** @cond
*/
  CMUTIL_COUNTRY_INFO_RESULT_MAX  /* FOR INTERNAL CM USE ONLY! Bounds checking. */
/** @endcond
*/

} cmutil_country_info_result_e_type;
/*~ SENTINEL cmutil_country_info_result_e_type.CMUTIL_COUNTRY_INFO_RESULT_MAX */

/** @} */ /* end_addtogroup CO */

/** @addtogroup CO
    @{ */

/** @brief Structure to hold a STATIC table.
*/
struct country_specific_record_s
{
   uint16 sid1;
     /**< Start of the SID range, including SID1. */
   
   uint16 sid2;
     /**< End of the SID range, including SID2. */
   
   uint16 country_code;
     /**< CC carries the country code directly. An array to accommodate a
          leading zero. Zero can be neglected for now; used for some zones. */
   
   char inter_dialed_digits[6];
    /**< International dialed digits start after the last F. */
   
   char special_prefix_code[6];
     /**< Special prefix code digits start after the last F. */
   
   char national_direct_dial[6];
     /**< National direct dial. */
   
   boolean is_network_soln;
     /**< Whether there is a network solution. */
   
   uint16  mobile_country_code;
     /**< Mobile country code. */
};

/** @brief Type to hold country-specific static information.
*/
typedef struct country_specific_record_s cmutil_country_specific_record_s ;

/** Type for call event callback function. A client
    that wants to be notified of call events must
    register for that function with the CM.
*/
typedef void (cm_mm_call_event_f_type) (

  cm_call_event_e_type         call_event,
    /**< Indicates a call event. */

  const cm_mm_call_info_s_type *call_info_ptr
    /**< Pointer to a call state information structure. */

);
 
/*~ PARAM call_info_ptr POINTER */

/** Type for a phone event callback function. A client that wants
    to be notified of phone events must register for that
    function with the CM. See cm_client_ph_reg() for information
    on registration.
*/
typedef void (cm_ph_event_f_type) (

  cm_ph_event_e_type           ph_event,
    /**< Indicates a phone event. */

  const cm_ph_info_s_type      *ph_info_ptr
    /**< Pointer to a phone state information structure. */

);
 
/*~ PARAM ph_info_ptr POINTER */

/** Type for BCMCS data event callback function. A client that
    wants to be notified of BCMCS data events must register for
    that function with the CM.
*/
typedef void (cm_bcmcs_event_f_type) (

  cm_bcmcs_event_e_type            bcmcs_event,
    /**< Indicates a BCMCS data event. */

  const cm_bcmcs_info_s_type      *bcmcs_info_ptr
    /**< Pointer to a BCMCS data information structure. */

);

 
/*~ PARAM bcmcs_info_ptr POINTER */

/** Type for an MBMS event callback function. A client that wants
    to be notified of MBMS data events must register for that
    function with the CM. Clients must check for CM_API_MBMS
    before using this function.
*/
typedef void (cm_mbms_event_f_type) (

  cm_mbms_event_e_type            mbms_event,
    /**< Indicates an MBMS data event. */

  const cm_mbms_info_s_type      *mbms_info_ptr
    /**< Pointer to a BCMCS data information structure. */

);

 
/*~ PARAM mbms_info_ptr POINTER */

/** Type for a serving system event callback function. A client
    that wants to be notified of serving system events must
    register for that function with the CM. See cm_client_ss_reg()
    for information on registering for SS events with the CM.
*/
typedef void (cm_mm_ss_event_f_type) (

  cm_ss_event_e_type           ss_event,
    /**< Indicates a serving system event. */

  const cm_mm_ss_info_s_type   *ss_info_ptr
    /**< Pointer to a serving system information structure. */

);
 
/*~ PARAM ss_info_ptr POINTER */

/** Type for an inband event callback function. A client that
    wants to be notified of inband events must register for that
    function with the CM.
*/
typedef void (cm_inband_event_f_type) (

  cm_inband_event_e_type           inband_event,
    /**< Indicates an inband event. */

  const cm_inband_info_s_type      *inband_info_ptr
    /**< Pointer to an inband information structure. */

);
 
/*~ PARAM inband_info_ptr POINTER */

/** Type for an SMS event callback function. A client that wants
    to be notified of inband events must register for that
    function with the CM.
*/
typedef void (cm_sms_event_f_type) (

  cm_sms_event_e_type           sms_event,
    /**< Indicates an SMS event. */

  const cm_sms_info_s_type      *sms_info_ptr
    /**< Pointer to an SMS information structure. */

);
 
/*~ PARAM sms_info_ptr POINTER */
/*~ PARAM nas_info_ptr POINTER */


/** Type for a DBM event callback function. A client that wants
    to be notified of DBM events must register for that function
    with the CM.
*/
typedef void (cm_dbm_event_f_type) (

    cm_dbm_event_e_type           dbm_event,
        /**< Indicates a DBM event. */

    const cm_dbm_info_s_type      *dbm_info_ptr
        /**< Pointer to a DBM information structure. */

);

 
/*~ PARAM dbm_info_ptr POINTER */

/** Type for a SUPS event callback function. A client that wants
    to be notified of SUPS events must register for that
    function with the CM.
*/
typedef void (cm_sups_event_f_type) (

  cm_sups_event_e_type                 sups_event,
    /**< Indicates a SUPS event. */

  const cm_sups_info_s_type            *sups_info_ptr
    /**< Pointer to a SUPS information structure. */

);
 
/*~ PARAM sups_info_ptr POINTER */

/** Type for a call information list callback function. A client that
    wants to be called with a call information list must
    register for that function with the CM.
*/
typedef void (cm_call_info_list_f_type) (

  const cm_call_state_info_list_s_type    *cm_call_info_list_ptr
    /**< Pointer to a call information list structure. */

);
/*~ PARAM cm_call_info_list_ptr POINTER */

/** Type to hold country specific static information.
*/
typedef void (cm_stats_event_f_type) (

  cm_stats_event_e_type                  stats_event,
    /**< Indicate statistics event. */

  const sys_modem_stats_info_s_type      *stats_info_ptr
    /**< A pointer to a modem statistics information structure. */

);
/*~ PARAM stats_info_ptr POINTER */


/** Type for a call command error callback function. A client
    that wants to be notified of other clients' call command
    errors must register for that function with the CM.
*/
typedef void (cm_mm_call_cmd_err_f_type) (

  cm_call_id_type              call_id,
    /**< Call ID. */

  cm_client_id_type            client_id,
    /**< Indicates which client caused the error. */

  cm_call_cmd_e_type           cmd,
    /**< Indicates for which call command this error is reported. */

  cm_call_cmd_err_e_type       cmd_err
    /**< Indicates the nature of the error. */

);

/** Type for a phone command error callback function. A client
    that wants to be notified of other clients' phone command
    errors must register for that function with the CM. Refer to
    cm_client_ph_reg() for information on registering this
    callback function.
*/
typedef void (cm_ph_cmd_err_f_type) (

  cm_client_id_type       client_id,
    /**< Indicates which client caused the error. */

  cm_ph_cmd_e_type        cmd,
    /**< Indicates for which phone command this error is reported. */

  cm_ph_cmd_err_e_type    cmd_err
    /**< Indicates the nature of the error. */

);

/** Type for a BCMCS data command error callback function. A
    client that wants to be notified of other clients' BCMCS
    data command errors must register for that function with the CM.
*/
typedef void (cm_bcmcs_cmd_err_f_type) (

  cm_client_id_type          client_id,
    /**< Indicates which client caused the error. */

  cm_bcmcs_cmd_e_type        cmd,
    /**< Indicates for which BCMCS data command this error is reported. */

  cm_bcmcs_cmd_err_e_type    cmd_err
    /**< Indicates the nature of the error. */

);

/** Type for an MBMS data command error callback function. A
    client that wants to be notified of other clients' MBMS data
    command errors must register for that function with the CM.
    Clients must check for CM_API_MBMS before using this function.
*/
typedef void (cm_mbms_cmd_err_f_type) (

  cm_client_id_type          client_id,
    /**< Indicates which client caused the error. */

  cm_mbms_cmd_e_type        cmd,
    /**< Indicates for which MBMS command this error is reported. */

  cm_mbms_cmd_err_e_type    cmd_err
    /**< Indicates the nature of the error. */

);

/** Type for a statistics data command error callback function. A
    client that wants to be notified of other clients' statistics data
    command errors must register such a function with CM.
*/
typedef void (cm_stats_cmd_err_f_type) (

  cm_client_id_type          client_id,
    /**< Indicates which client caused the error. */

  cm_stats_cmd_e_type        cmd,
    /**< Indicates for which statistics command this error is reported. */

  cm_stats_cmd_err_e_type    cmd_err
    /**< Indicates the nature of the error. */

);

/** Type for a serving system command error callback function. A
    client that wants to be notified of other clients' serving
    system command errors must register for that function with
    the CM. See cm_client_ss_reg() for information on registering
    with the CM.
*/
typedef void (cm_ss_cmd_err_f_type) (

  cm_client_id_type       client_id,
    /**< Indicates which client caused the error. */

  cm_ss_cmd_e_type        cmd,
    /**< Indicates for which serving system command this error is reported. */

  cm_ss_cmd_err_e_type    cmd_err
    /**< Indicates the nature of the error. */

);

/** Type for an inband command error callback function. A client
    that wants to be notified of other clients' inband command
    errors must register for that function with the CM.
*/
typedef void (cm_mm_inband_cmd_err_f_type) (

  cm_call_id_type             call_id,
    /**< Call ID. */

  cm_client_id_type           client_id,
    /**< Indicates which client caused the error. */

  cm_inband_cmd_e_type        cmd,
    /**< Indicates for which inband command this error is reported. */

  cm_inband_cmd_err_e_type    cmd_err
    /**< Indicates the nature of the error. */

);

/** Type for an SMS command error callback function. A client that
    wants to be notified of other clients' SMS command errors
    must register for that function with the CM.
*/
typedef void (cm_sms_cmd_err_f_type) (

  cm_client_id_type           client_id,
    /**< Indicates which client caused the error. */

  cm_sms_cmd_e_type           cmd,
    /**< Indicates for which SMS command this error is reported. */

  cm_sms_cmd_err_e_type       cmd_err
    /**< Indicates the nature of the error. */

);

/** Type for a DBM command error callback function. A client that
    wants to be notified of other clients' DBM command errors
    must register for that function with the CM.
*/
typedef void (cm_dbm_cmd_err_f_type) (

    cm_client_id_type           client,
        /**< Indicates which client caused the error. */

    cm_dbm_cmd_e_type           cmd,
        /**< Indicates for which DBM command this error is reported. */

    cm_dbm_cmd_err_e_type       cmd_err
        /**< Indicates the nature of the error. */
);

/** Type for a SUPS command error callback function. A client
    that wants to be notified of other clients' SUPS command
    errors must register for that function with the CM.
*/
typedef void (cm_sups_cmd_err_f_type) (

  cm_client_id_type            client_id,
    /**< Indicates which client caused the error. */

  cm_sups_cmd_e_type           cmd,
    /**< Indicates for which SUPS command this error is reported. */

  cm_sups_cmd_err_e_type       cmd_err
    /**< Indicates the nature of the error. */

);

/** @} */ /* end_addtogroup CO */

/** @addtogroup CO
    @{ */


/*===========================================================================

FUNCTION: cm_client_init

===========================================================================*/
/**
  Initializes a client object. After initialization, the client
  is assigned a unique client ID. This function must be called
  exactly once on a client object before it is used in any way,
  place, or form.

@note1hang In order to get notified of CM events, the client must
  do the following:

  - Register its callback functions and specific events with
    each functional group.
  - Activate its client object with cm_client_act().

@param[in] client_type    Type of client.
@param[out] client_id_ptr Client ID pointer.

@return
  Request status.

@dependencies
  None.
*/
extern cm_client_status_e_type cm_client_init(

  cm_client_type_e_type           client_type,
  cm_client_id_type               *client_id_ptr

);

/*~ FUNCTION cm_client_init 
    RELEASE_FUNC cm_client_release( *client_id_ptr )
    ONERROR return CM_CLIENT_ERR_CLIENT_INIT_FAIL */
/*~ PARAM OUT client_id_ptr POINTER */

/*===========================================================================

FUNCTION: cm_client_release

===========================================================================*/
/**
  Releases a client object. This function should be called if a
  client no longer needs the CM. After this call, the client can
  no longer perform any CM functions. This call releases the
  client object. That object can be reused for another client.
  If the client needs to use the CM again, it must call the
  cm_client_init() function to establish a new connection.

@param[in] client_id Client ID pointer.

@return
  Request status.

@dependencies
  None.
*/
extern cm_client_status_e_type cm_client_release(

  cm_client_id_type               client_id

);
 
/*~ FUNCTION cm_client_release */

/*===========================================================================

FUNCTION: cm_mm_client_call_reg

===========================================================================*/
/**
  Allows a client to register the following call-related
  callbacks and events with the CM:

  - Callback function to receive registered call events.
  - Register and deregister call events.
  - Callback function to receive other clients' call command
    errors.

   If a client registers a callback function to receive call event
   notification, it must also register one or more call events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client must call this API twice; once
   for each range of call events.

   If a client registers a callback function to receive other
   clients' call command errors, it will receive notifications
   on all possible call command errors caused by other clients.
   A client cannot register to receive only a subset of call
   command errors.

   The NULL function pointer can also be used for the callback
   function if a client is not interested in receiving that
   callback. For example, if a client is not interested in
   receiving other clients' call command errors, it can set
   call_cmd_err_func = NULL. However, if the call_event_func is
   set to NULL, all registered call events are also
   deregistered, and the event_reg_type, from_call_event, and
   to_call_event parameters are ignored.

@param[in] client_id         Requesting client.
@param[in] call_event_func   Pointer to a callback function to notify the
                             client of call events.
@param[in] event_reg_type    Register or deregister.
@param[in] from_call_event   Register from this event (inclusive).
@param[in] to_call_event     Register to this event (inclusive).
@param[in] call_cmd_err_func Pointer to a callback function to notify the
                             client of call command errors of other clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_call_reg(

  cm_client_id_type           client_id,
  cm_mm_call_event_f_type     *call_event_func,
  cm_client_event_reg_e_type  event_reg_type,
  cm_call_event_e_type        from_call_event,
  cm_call_event_e_type        to_call_event,
  cm_mm_call_cmd_err_f_type   *call_cmd_err_func

);
 
/*~ FUNCTION cm_mm_client_call_reg */

/*===========================================================================

FUNCTION: cm_mm_client_call_pwr_collapse_reg

==========================================================================*/
/**
  Allows a client to register/deregister the events it wants to
  be notified of during power collapse. If the client wants to
  be notified of all the events that it is registered for in the
  normal (power restored) state, it should use
  CM_PWR_COLLAPSE_NOTIFY_NORMAL as the pwr_collapse_type. If the
  client wants the CM to decide on important events to wake up
  the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants
  to override the CM's default, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_call_event, to_call_event, and event_reg_type are
  used only if the pwr_collapse_type is
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If event_reg_type is
  CM_CLIENT_EVENT_REG, the event will be notified while in power
  collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the
  event will not be reported while in power collapse.

  This API has a cumulative effect for event registration. For
  example, in order to register/deregister for two different
  ranges of call events, a client must call this API twice;
  once for each range of call events. In case of conflicting
  pwr_collapse_type, the last one is honored.

@param[in] client_id         Client ID.
@param[in] call_event_func   Pointer to a callback function to notify the
                             client of call events.
@param[in] proc_id           Processor on which the client is executing.
@param[in] pwr_collapse_type Power collapse type: None, Normal, Default,
                             or Custom.
@param[in] event_reg_type    Registration type.
@param[in] from_call_event   Register from this event (inclusive).
@param[in] to_call_event     Register to this event (inclusive).

@return 
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_call_pwr_collapse_reg(

    cm_client_id_type             client,
    cm_mm_call_event_f_type      *call_event_func,
    cm_processor_e_type           proc_id,
    cm_pwr_collapse_notify_e_type pwr_collapse_type,
    cm_client_event_reg_e_type    event_reg_type,
    cm_call_event_e_type          from_call_event,
    cm_call_event_e_type          to_call_event

);
 
/*~ FUNCTION cm_mm_client_call_pwr_collapse_reg */

/*===========================================================================

FUNCTION: cm_client_ph_reg

===========================================================================*/
/**
   Allows a client to register the following phone-related
   callbacks and events with the CM:

  - Callback function to receive registered phone events.
  - Register and deregister phone events.
  - Callback function to receive other clients' phone command
    errors.

   If a client registers a callback function to receive phone
   event notification, it also must register one or more
   phone events it is interested in receiving. This API has a
   cumulative effect for event registration. For example, in
   order to register for two different ranges of call events, a
   client must call this API twice; once for each range of
   call events.

   If a client registers a callback function to receive other
   clients' phone command errors, it will receive notifications
   on all possible phone command errors caused by other clients.
   A client cannot register to receive only a subset of phone
   command errors.

   The NULL function pointer can also be used for the callback
   function if a client is not interested in receiving that
   callback. For example, if a client is not interested in
   receiving other clients' phone command errors, it can set
   ph_cmd_err_func = NULL. However, if the ph_event_func is set
   to NULL, all registered phone events are also deregistered
   and the event_reg_type, from_ph_event, and to_ph_event
   parameters are ignored.

@param[in] client_id       Requesting client.
@param[in] ph_event_func   Pointer to a callback function to notify the
                           client of phone events.
@param[in] event_reg_type  Register or deregister.
@param[in] from_ph_event   Register from this event (inclusive).
@param[in] to_ph_event     Register to this event (inclusive).
@param[in] ph_cmd_err_func Pointer to a callback function to notify the
                           client of phone command errors of other clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_client_ph_reg(

  cm_client_id_type               client_id,
  cm_ph_event_f_type              *ph_event_func,
  cm_client_event_reg_e_type      event_reg_type,
  cm_ph_event_e_type              from_ph_event,
  cm_ph_event_e_type              to_ph_event,
  cm_ph_cmd_err_f_type            *ph_cmd_err_func

);
 
/*~ FUNCTION cm_client_ph_reg */

/*===========================================================================

FUNCTION: cm_mm_client_ph_pwr_collapse_reg

===========================================================================*/
/**
   Allows a client to register/deregister the events it wants to
   be notified of during power collapse. If the client wants to
   be notified of all events it is registered for in the normal
   (power restored) state, it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL
   as the pwr_collapse_type. If the client wants the CM to decide
   on important events to wake up the processor from the power collapse
   state, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If
   the client wants to override the CM's default, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

   The from_ph_event, to_ph_event, and event_reg_type are used only if the
   pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
   event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified
   while in power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG,
   the event will not be reported while in power collapse.

   This API has a cumulative effect for event registration. For
   example, in order to register/deregister for two different
   ranges of phone events, a client must call this API twice;
   once for each range of phone events. In case of
   conflicting pwr_collapse_type, the last one is honored.

@param[in] client            Client ID.
@param[in] ph_event_func     Pointer to a callback function to notify the
                             client of call events.
@param[in] proc_id           Processor on which the client is executing.
@param[in] pwr_collapse_type Power collapse type: None, Normal, Default,
                             or Custom.
@param[in] event_reg_type    Registration type.
@param[in] from_ph_event     Register from this event (inclusive).
@param[in] to_ph_event       Register to this event (inclusive).

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_ph_pwr_collapse_reg(

    cm_client_id_type             client,
    cm_ph_event_f_type            *ph_event_func,
    cm_processor_e_type           proc_id,
    cm_pwr_collapse_notify_e_type pwr_collapse_type,
    cm_client_event_reg_e_type    event_reg_type,
    cm_ph_event_e_type            from_ph_event,
    cm_ph_event_e_type            to_ph_event

);
 
/*~ FUNCTION cm_mm_client_ph_pwr_collapse_reg */

/*===========================================================================

FUNCTION: cm_mm_client_ss_reg

===========================================================================*/
/**
   Allows a client to register the following serving
   system-related callbacks and events with the CM:

   - Callback function to receive registered serving system events.
   - Register and deregister serving system events.
   - Callback function to receive other clients' serving
     system command errors.

   If a client registers a callback function to receive serving
   system event notification, it also must register one or
   more serving system events it is interested in receiving.
   This API has a cumulative effect for event registration. For
   example, in order to register for two different ranges of
   call events, a client must call this API twice; once for
   each range of call events.

   If a client registers a callback function to receive other
   clients' serving system command errors, it will receive
   notifications on all possible serving system command errors
   caused by other clients. A client cannot register to receive
   only a subset of serving system command errors.

   The NULL function pointer can also be used for the callback
   function if a client is not interested in receiving that
   callback. For example, if a client is not interested in
   receiving other clients' serving system command errors, it
   can set ss_cmd_err_func = NULL. However, if the ss_event_func
   is set to NULL, all registered serving system events are also
   deregistered and the event_reg_type, from_ss_event, and
   to_ss_event parameters are ignored.

@param[in] client_id       Requesting client.
@param[in] ss_event_func   Pointer to a callback function to notify the
                           client of serving system events.
@param[in] event_reg_type  Register or deregister.
@param[in] from_ss_event   Register from this event (inclusive).
@param[in] to_ss_event     Register to this event (inclusive).
@param[in] ss_cmd_err_func Pointer to a callback function to notify the
                           client of serving system command errors of
                           other clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_ss_reg(

  cm_client_id_type               client_id,
  cm_mm_ss_event_f_type           *ss_event_func,
  cm_client_event_reg_e_type      event_reg_type,
  cm_ss_event_e_type              from_ss_event,
  cm_ss_event_e_type              to_ss_event,
  cm_ss_cmd_err_f_type            *ss_cmd_err_func

);
 
/*~ FUNCTION cm_mm_client_ss_reg */

/*===========================================================================

FUNCTION: cm_mm_client_ss_pwr_collapse_reg

===========================================================================*/
/**
   Allows a client to register/deregister the events it wants to
   be notified of during power collapse. If the client wants to
   be notified of all events it is registered for in the normal
   (power restored) state, it should use
   CM_PWR_COLLAPSE_NOTIFY_NORMAL as the pwr_collapse_type. If
   the client wants the CM to decide on important events to wake
   up the processor from the power collapse state, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants
   to override the CM's default, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

   The from_ss_event, to_ss_event, and event_reg_type are used
   only if the pwr_collapse_type is
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If event_reg_type is
   CM_CLIENT_EVENT_REG, the event will be notified while in
   power collapse. If the event_reg_type is
   CM_CLIENT_EVENT_DEREG, the event will not be reported while
   in power collapse.

   This API has a cumulative effect for event registration. For
   example, in order to register/deregister for two different
   ranges of SS events, a client must call this API twice;
   once for each range of phone events. In case of conflicting
   pwr_collapse_type, the last one is honored.

@param[in] client            Client ID.
@param[in] ss_event_func     Pointer to a callback function to notify the
                             client of call events.
@param[in] proc_id           Processor on which the client is executing.
@param[in] pwr_collapse_type Power collapse type: None, Normal, Default,
                             or Custom.
@param[in] event_reg_type    Registration type.
@param[in] from_ss_event     Register from this event (inclusive).
@param[in] to_ss_event       Register to this event (inclusive).

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_ss_pwr_collapse_reg(

    cm_client_id_type             client,
    cm_mm_ss_event_f_type         *ss_event_func,
    cm_processor_e_type           proc_id,
    cm_pwr_collapse_notify_e_type pwr_collapse_type,
    cm_client_event_reg_e_type    event_reg_type,
    cm_ss_event_e_type            from_ss_event,
    cm_ss_event_e_type            to_ss_event
 
);
 
/*~ FUNCTION cm_mm_client_ss_pwr_collapse_reg */

/*===========================================================================

FUNCTION: cm_client_inband_reg

===========================================================================*/
/**
   Allows a client to register the following inband-related
   callbacks and events with the CM:

   - Callback function to receive registered inband events.
   - Register and deregister inband events.
   - Callback function to receive other clients' inband command errors.

    If a client registers a callback function to receive inband
    event notification, it also must register one or more
    inband events it is interested in receiving. This API has a
    cumulative effect for event registration. For example, in
    order to register for two different ranges of inband events,
    a client must call this API twice; once for each range of
    inband events.

    If a client registers a callback function to receive other
    clients' inband command errors, it will receive
    notifications on all possible inband command errors caused
    by other clients. A client cannot register to receive only a
    subset of inband command errors.

    The NULL function pointer can also be used for the callback
    function if a client is not interested in receiving that
    callback. For example, if a client is not interested in
    receiving other clients' inband command errors, it can set
    inband_cmd_err_func = NULL. However, if the
    inband_event_func is set to NULL, all registered inband
    events are also deregistered and the event_reg_type,
    from_inbnad_event, and to_inband_event parameters are
    ignored.

@param[in] client_id           Requesting client.
@param[in] inband_event_func   Pointer to a callback function to notify the
                               client of inband events.
@param[in] event_reg_type      Register or deregister.
@param[in] from_inband_event   Register from this event (inclusive).
@param[in] to_inband_event     Register to this event (inclusive).
@param[in] inband_cmd_err_func Pointer to a callback function to notify
                               the client of inband command errors of other
                               clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_inband_reg(

  cm_client_id_type               client_id,
  cm_inband_event_f_type          *inband_event_func,
  cm_client_event_reg_e_type      event_reg_type,
  cm_inband_event_e_type          from_inband_event,
  cm_inband_event_e_type          to_inband_event,
  cm_mm_inband_cmd_err_f_type     *inband_cmd_err_func

);
 
/*~ FUNCTION cm_mm_client_inband_reg */

/*===========================================================================

FUNCTION: cm_mm_client_inband_pwr_collapse_reg

===========================================================================*/
/**
   Allows a client to register/deregister the events it wants to
   be notified of during power collapse. If the client wants to
   be notified of all events it is registered for in the normal
   (power restored) state, it should use
   CM_PWR_COLLAPSE_NOTIFY_NORMAL as the pwr_collapse_type. If
   the client wants the CM to decide on important events to wake
   up the processor from the power collapse state, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants
   to override the CM's default, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

   The from_inband_event, to_inband_event, and event_reg_type
   are used only if the pwr_collapse_type is
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If event_reg_type is
   CM_CLIENT_EVENT_REG, the event will be notified while in
   power collapse. If the event_reg_type is
   CM_CLIENT_EVENT_DEREG, the event will not be reported while
   in power collapse.

   This API has a cumulative effect for event registration. For
   example, in order to register/deregister for two different
   ranges of inband events, a client must call this API twice;
   once for each range of phone events. In case of conflicting
   pwr_collapse_type, the last one is honored.

@param[in] client            Client ID.
@param[in] inband_event_func Pointer to a callback function to notify the
                             client of call events.
@param[in] proc_id           Processor on which the client is executing.
@param[in] pwr_collapse_type Power collapse type: None, Normal, Default,
                             or Custom.
@param[in] event_reg_type    Registration type.
@param[in] from_inband_event Register from this event (inclusive).
@param[in] to_inband_event   Register to this event (inclusive).

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_inband_pwr_collapse_reg(

    cm_client_id_type             client,
    cm_inband_event_f_type        *inband_event_func,
    cm_processor_e_type           proc_id,
    cm_pwr_collapse_notify_e_type pwr_collapse_type,
    cm_client_event_reg_e_type    event_reg_type,
    cm_inband_event_e_type        from_inband_event,
    cm_inband_event_e_type        to_inband_event

);
 
/*~ FUNCTION cm_mm_client_inband_pwr_collapse_reg */

/*===========================================================================

FUNCTION: cm_client_sms_reg

===========================================================================*/
/**
   Allows a client to register the following SMS-related
   callbacks and events with the CM:

   - Callback function to receive registered SMS events.
   - Register and deregister SMS events.
   - Callback function to receive other clients' SMS command errors.

    If a client registers a callback function to receive SMS
    event notifications, it must also register one or more SMS
    events it is interested in receiving. This API has a
    cumulative effect for event registration. For example, in
    order to register for two different ranges of SMS events, a
    client must call this API twice; once for each range of SMS
    events.

    If a client registers a callback function to receive other
    clients' SMS command errors, it will receive notifications
    on all possible SMS command errors caused by other clients.
    A client cannot register to receive only a subset of SMS
    command errors. The NULL function pointer can also be used
    for the callback function if a client is not interested in
    receiving that callback. For example, if a client is not
    interested in receiving other clients' SMS command errors,
    it can set sms_cmd_err_func = NULL. However, if the
    sms_event_func is set to NULL, all registered SMS events are
    also deregistered and the event_reg_type, from_sms_event,
    and to_sms_event parameters are ignored.

@param[in] client_id        Requesting client.
@param[in] sms_event_func   Pointer to a callback function to notify the
                            client of SMS events.
@param[in] event_reg_type   Register or deregister.
@param[in] from_sms_event   Register from this event (inclusive).
@param[in] to_sms_event     Register to this event (inclusive).
@param[in] sms_cmd_err_func Pointer to a callback function to notify the
                            client of SMS command errors of other clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().\n
  FEATURE_CDSMS must be defined.
*/
cm_client_status_e_type cm_client_sms_reg(

  cm_client_id_type               client_id,
  cm_sms_event_f_type             *sms_event_func,
  cm_client_event_reg_e_type      event_reg_type,
  cm_sms_event_e_type             from_sms_event,
  cm_sms_event_e_type             to_sms_event,
  cm_sms_cmd_err_f_type           *sms_cmd_err_func

);
 
/*~ FUNCTION cm_client_sms_reg */

/*===========================================================================

FUNCTION: cm_mm_client_sms_pwr_collapse_reg

===========================================================================*/
/**
   Allows a client to register/deregister the events it wants to
   be notified of during power collapse. If the client wants to
   be notified of all events it is registered for in the normal
   (power restored) state, it should use
   CM_PWR_COLLAPSE_NOTIFY_NORMAL as the pwr_collapse_type. If
   the client wants the CM to decide on important events to wake
   up the processor from the power collapse state, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants
   to override the CM's default, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

   The from_sms_event, to_sms_event, and event_reg_type are used
   only if the pwr_collapse_type is
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If event_reg_type is
   CM_CLIENT_EVENT_REG, the event will be notified while in
   power collapse. If the event_reg_type is
   CM_CLIENT_EVENT_DEREG, the event will not be reported while
   in power collapse.

   This API has a cumulative effect for event registration. For
   example, in order to register/deregister for two different
   ranges of SMS events, a client must call this API twice;
   once for each range of phone events. In case of conflicting
   pwr_collapse_type, the last one is honored

@param[in] client            Client ID.
@param[in] sms_event_func    Pointer to a callback function to notify the
                             client of call events.
@param[in] proc_id           Processor on which the client is executing.
@param[in] pwr_collapse_type Power collapse type: None, Normal, Default,
                             or Custom.
@param[in] event_reg_type    Register or deregister.
@param[in] from_sms_event    Register from this event (inclusive).
@param[in] to_sms_event      Register to this event (inclusive).

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_sms_pwr_collapse_reg(

    cm_client_id_type             client,
    cm_sms_event_f_type           *sms_event_func,
    cm_processor_e_type           proc_id,
    cm_pwr_collapse_notify_e_type pwr_collapse_type,
    cm_client_event_reg_e_type    event_reg_type,
    cm_sms_event_e_type           from_sms_event,
    cm_sms_event_e_type           to_sms_event

);
 
/*~ FUNCTION cm_mm_client_sms_pwr_collapse_reg */

/*===========================================================================

FUNCTION: cm_mm_client_call_info_list_reg

===========================================================================*/
/**
  Clients call this function to register a callback for
  receiving a call information list from the CM. The CM API must
  be called separately to receive the information through
  callback.

@param[in] client                      Client ID.
@param[in] call_info_list_func         Pointer to a callback function to notify
                                       the client of call events.
@param[in] event_reg_type              Event register type.
@param[in] call_info_list_cmd_err_func Pointer to a callback function to 
                                       notify the client of call command
                                       errors of other clients.

@return
  Results of the registration.

@dependencies
  cm_client_init must have been called previously.
*/
cm_client_status_e_type cm_mm_client_call_info_list_reg(

    cm_client_id_type             client,
    cm_call_info_list_f_type      *call_info_list_func,
    cm_client_event_reg_e_type    event_reg_type,
    cm_mm_call_cmd_err_f_type     *call_info_list_cmd_err_func

);
 
/*~ FUNCTION cm_mm_client_call_info_list_reg */

/*===========================================================================

FUNCTION: cm_client_dbm_event_reg

===========================================================================*/
/**
   Used by CM clients interested in registering for DBM call
   manager events.

@param[in] client           Requesting client.
@param[in] dbm_event_func   Pointer to a callback function to notify the
                            client of DBM events.
@param[in] event_reg_type   Register or deregister.
@param[in] from_dbm_event   Register from this event (inclusive).
@param[in] to_dbm_event     Register to this event (inclusive).
@param[in] dbm_cmd_err_func Pointer to a callback function to notify the
                            client of DBM command errors of other clients.

@return
  Results of the registration.

@dependencies
  cm_client_init must have been called previously.
*/
cm_client_status_e_type cm_client_dbm_event_reg(

    cm_client_id_type               client,
    cm_dbm_event_f_type             *dbm_event_func,
    cm_client_event_reg_e_type      event_reg_type,
    cm_dbm_event_e_type             from_dbm_event,
    cm_dbm_event_e_type             to_dbm_event,
    cm_dbm_cmd_err_f_type           *dbm_cmd_err_func

);
 
/*~ FUNCTION cm_client_dbm_event_reg */
/*~ PARAM dbm_event_func POINTER */
/*~ PARAM dbm_cmd_err_func POINTER */

/*===========================================================================

FUNCTION: cm_client_dbm_class_reg

===========================================================================*/
/**
   Used by CM clients interested in registering for DBM classes,
   such as PD, SDB, etc.

@param[in] client         Requesting client.
@param[in] event_reg_type Register or deregister.
@param[in] from_dbm_class Register from this class (inclusive).
@param[in] to_dbm_class   Register to this class (inclusive).

@return
  Results of the registration.

@dependencies
  cm_client_init must have been called previously.
*/
cm_client_status_e_type cm_client_dbm_class_reg(

    cm_client_id_type               client,
    cm_client_event_reg_e_type      event_reg_type,
    cm_dbm_class_e_type             from_dbm_class,
    cm_dbm_class_e_type             to_dbm_class

);
/*~ FUNCTION cm_client_dbm_class_reg */

/*===========================================================================

FUNCTION: cm_mm_client_dbm_pwr_collapse_reg

===========================================================================*/
/**
   Allows a client to register/deregister the events it want to
   be notified of during power collapse. If the client wants to
   be notified of all events it is registered for in the normal
   (power restored) state, it should use
   CM_PWR_COLLAPSE_NOTIFY_NORMAL as the pwr_collapse_type. If
   the client wants the CM to decide on important events to wake
   up the processor from the power collapse state, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants
   to override the CM's default, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

   The from_dbm_event, to_dbm_event, and event_reg_type are used
   only if the pwr_collapse_type is
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If event_reg_type is
   CM_CLIENT_EVENT_REG, the event will be notified while in
   power collapse. If the event_reg_type is
   CM_CLIENT_EVENT_DEREG, the event will not be reported while
   in power collapse.

   This API has a cumulative effect for event registration. For
   example, in order to register/deregister for two different
   ranges of DBM events, a client must call this API twice;
   once for each range of DBM events. In case of conflicting
   pwr_collapse_type, the last one is honored

@param[in] client            Client ID.
@param[in] dbm_event_func    Pointer to a callback function to notify the
                             client of call events.
@param[in] proc_id           Processor on which the client is executing.
@param[in] pwr_collapse_type Power collapse type: None, Full, Default,
                             or Custom.
@param[in] event_reg_type    Register or deregister.
@param[in] from_dbm_event    Register from this event (inclusive).
@param[in] to_dbm_event      Register to this event (inclusive).

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_dbm_pwr_collapse_reg(

    cm_client_id_type             client,
    cm_dbm_event_f_type           *dbm_event_func,
    cm_processor_e_type           proc_id,
    cm_pwr_collapse_notify_e_type pwr_collapse_type,
    cm_client_event_reg_e_type    event_reg_type,
    cm_dbm_event_e_type           from_dbm_event,
    cm_dbm_event_e_type           to_dbm_event

);
 
/*~ FUNCTION cm_mm_client_dbm_pwr_collapse_reg */

/*===========================================================================

FUNCTION: cm_client_sups_reg

===========================================================================*/
/**
   Allows a client to register the following serving
   system-related callbacks and events with the CM:

   - Callback function to receive registered SUPS events.
   - Register and deregister SUPS events.
   - Callback function to receive other clients' SUPS command errors.

    If a client registers a callback function to receive SUPS
    event notification, it also must register one or more
    SUPS events it is interested in receiving. This API has a
    cumulative effect for event registration. For example, in
    order to register for two different ranges of SUPS events, a
    client must call this API twice; once for each range of
    SUPS events.

    If a client registers a callback function to receive other
    clients' SUPS command errors, it will receive notifications
    on all possible SUPS command errors caused by other clients.
    A client cannot register to receive only a subset of SUPS
    command errors.

    The NULL function pointer can also be used for the callback
    function if a client is not interested in receiving that
    callback. For example, if a client is not interested in
    receiving other clients' SUPS command errors, it can set
    sups_cmd_err_func = NULL. However, if the sups_event_func is
    set to NULL, all registered SUPS events are also
    deregistered, and the event_reg_type, from_event, and
    to_event parameters are ignored.

@param[in] client_id         Requesting client.
@param[in] sups_event_func   Pointer to a callback function to notify the
                             client of SUPS events.
@param[in] event_reg_type    Register or deregister.
@param[in] from_event        Register from this event (inclusive).
@param[in] to_event          Register to this event (inclusive).
@param[in] sups_cmd_err_func Pointer to a callback function to notify the
                             client of SUPS command errors of other clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init(). \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.
*/
cm_client_status_e_type cm_client_sups_reg(

  cm_client_id_type             client_id,
  cm_sups_event_f_type          *sups_event_func,
  cm_client_event_reg_e_type    event_reg_type,
  cm_sups_event_e_type          from_event,
  cm_sups_event_e_type          to_event,
  cm_sups_cmd_err_f_type        *sups_cmd_err_func

);
 
/*~ FUNCTION cm_client_sups_reg */

/*===========================================================================

FUNCTION: cm_mm_client_sups_pwr_collapse_reg

===========================================================================*/
/**
   Allows a client to register/deregister the events it wants to
   be notified of during power collapse. If the client wants to
   be notified of all events it is registered for in the normal
   (power restored) state, it should use
   CM_PWR_COLLAPSE_NOTIFY_NORMAL as the pwr_collapse_type. If
   the client wants the CM to decide on important events to wake
   up the processor from the power collapse state, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants
   to override the CM's default, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

   The from_sups_event, to_sups_event, and event_reg_type are
   used only if the pwr_collapse_type is
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If event_reg_type is
   CM_CLIENT_EVENT_REG, the event will be notified while in
   power collapse. If the event_reg_type is
   CM_CLIENT_EVENT_DEREG, the event will not be reported while
   in power collapse.

   This API has a cumulative effect for event registration. For
   example, in order to register/deregister for two different
   ranges of SUPS events, a client must call this API twice;
   once for each range of SUPS events. In case of conflicting
   pwr_collapse_type, the last one is honored

@param[in] client            Client ID.
@param[in] sups_event_func   Pointer to a callback function to notify the
                             client of call events.
@param[in] proc_id           Processor on which the client is executing.
@param[in] pwr_collapse_type Power collapse type: None, Full, Default,
                             or Custom.
@param[in] event_reg_type    Register or deregister.
@param[in] from_sups_event   Register from this event (inclusive).
@param[in] to_sups_event     Register to this event (inclusive).

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_sups_pwr_collapse_reg(

    cm_client_id_type             client,
    cm_sups_event_f_type          *sups_event_func,
    cm_processor_e_type           proc_id,
    cm_pwr_collapse_notify_e_type pwr_collapse_type,
    cm_client_event_reg_e_type    event_reg_type,
    cm_sups_event_e_type          from_sups_event,
    cm_sups_event_e_type          to_sups_event

);
 
/*~ FUNCTION cm_mm_client_sups_pwr_collapse_reg */

/*===========================================================================

FUNCTION: cm_client_bcmcs_reg

===========================================================================*/
/**
   Allows a client to register the following BCMCS callbacks and
   events with the CM:

   - Callback function to receive registered BCMCS events.
   - Register for and deregister from receiving BCMCS events.
   - Callback function to receive other clients' BCMCS command errors.

    If a client registers a callback function to receive BCMCS
    event notification, it must also register one or more BCMCS
    events it is interested in receiving. This API has a
    cumulative effect for event registration. For example, in
    order to register for two different ranges of BCMCS events,
    a client must call this API twice; once for each range of
    BCMCS events.

    If a client registers a callback function to receive other
    clients' BCMCS command errors, it will receive notifications
    on all possible BCMCS command errors caused by other
    clients. A client cannot register to receive only a subset
    of BCMCS command errors.

    The NULL function pointer can also be used for the callback
    function if a client is not interested in receiving that
    callback. For example, if a client is not interested in
    receiving other clients' BCMCS command errors, it can set
    bcmcs_cmd_err_func = NULL. However, if the bcmcs_event_func
    is set to NULL, all registered BCMCS events are also
    deregistered, and the event_reg_type, from_bcmcs_event, and
    to_bcmcs_event parameters are ignored.

@param[in] client             Client ID.
@param[in] bcmcs_event_func   Pointer to a callback function to notify the
                              client of BCMCS events.
@param[in] event_reg_type     Register or deregister.
@param[in] from_bcmcs_event   Register from this event (inclusive).
@param[in] to_bcmcs_event     Register to this event (inclusive).
@param[in] bcmcs_cmd_err_func Pointer to a callback function to notify
                              the client of BCMCS command errors of other
                              clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_client_bcmcs_reg(

    cm_client_id_type              client,
    cm_bcmcs_event_f_type          *bcmcs_event_func,
    cm_client_event_reg_e_type     event_reg_type,
    cm_bcmcs_event_e_type          from_bcmcs_event,
    cm_bcmcs_event_e_type          to_bcmcs_event,
    cm_bcmcs_cmd_err_f_type        *bcmcs_cmd_err_func

);
 
/*~ FUNCTION cm_client_bcmcs_reg */
/*~ PARAM bcmcs_event_func POINTER */
/*~ PARAM bcmcs_cmd_err_func POINTER */

/*===========================================================================

FUNCTION: cm_mm_client_bcmcs_pwr_collapse_reg

==========================================================================*/
/**
   Allows a client to register/deregister the events it wants to
   be notified of during power collapse. If the client wants to
   be notified of all events it is registered for in the normal
   (power restored) state, it should use
   CM_PWR_COLLAPSE_NOTIFY_NORMAL as the pwr_collapse_type. If
   the client wants the CM to decide on important events to wake
   up the processor from the power collapse state, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants
   to override the CM's default, it should use
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

   The from_bcmcs_event, to_bcmcs_event, and event_reg_type are
   used only if the pwr_collapse_type is
   CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If event_reg_type is
   CM_CLIENT_EVENT_REG, the event will be notified while in
   power collapse. If the event_reg_type is
   CM_CLIENT_EVENT_DEREG, the event will not be reported while
   in power collapse.

   This API has a cumulative effect for event registration. For
   example, in order to register/deregister for two different
   ranges of BCMCS events, a client must call this API twice;
   once for each range of BCMCS events. In case of conflicting
   pwr_collapse_type, the last one is honored

@param[in] client            Client ID.
@param[in] bcmcs_event_func  Pointer to a callback function to notify the
                             client of call events.
@param[in] proc_id           Processor on which the client is executing.
@param[in] pwr_collapse_type Power collapse type: None, Full, Default,
                             or Custom.
@param[in] event_reg_type    Register or deregister.
@param[in] from_bcmcs_event  Register from this event (inclusive).
@param[in] to_bcmcs_event    Register to this event (inclusive).

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_mm_client_bcmcs_pwr_collapse_reg(

    cm_client_id_type             client,
    cm_bcmcs_event_f_type         *bcmcs_event_func,
    cm_processor_e_type           proc_id,
    cm_pwr_collapse_notify_e_type pwr_collapse_type,
    cm_client_event_reg_e_type    event_reg_type,
    cm_bcmcs_event_e_type         from_bcmcs_event,
    cm_bcmcs_event_e_type         to_bcmcs_event

);
 
/*~ FUNCTION cm_mm_client_bcmcs_pwr_collapse_reg */

/*===========================================================================

FUNCTION: cm_client_mbms_reg

===========================================================================*/
/**
   Allows a client to register the following MBMS callbacks and
   events with the CM:
   - Callback function to receive registered MBMS events.
   - Register for and deregister from receiving MBMS events.
   - Callback function to receive other clients' MBMS command errors.

    If a client registers a callback function to receive MBMS
    event notification, it must also register one or more MBMS
    events it is interested in receiving. This API has a
    cumulative effect for event registration. For example, in
    order to register for two different ranges of BCMCS events,
    a client must call this API twice; once for each range of
    MBMS events.

    If a client registers a callback function to receive other
    clients' MBMS command errors, it will receive notifications
    on all possible MBMS command errors caused by other clients.
    A client cannot register to receive only a subset of MBMS
    command errors.

    The NULL function pointer can also be used for the callback
    function if a client is not interested in receiving that callback.
    For example, if a client is not interested in receiving other
    clients' MBMS command errors, it can set mbms_cmd_err_func = NULL.
    However, if the mbms_event_func is set to NULL, all registered
    MBMS events are also deregistered, and the event_reg_type,
    from_mbms_event, and to_mbms_event parameters are ignored.

@param[in] client            Client ID.
@param[in] mbms_event_func   Pointer to a callback function to notify the
                             client of MBMS events.
@param[in] event_reg_type    Register or deregister.
@param[in] from_mbms_event   Register from this event (inclusive).
@param[in] to_mbms_event     Register to this event (inclusive).
@param[in] mbms_cmd_err_func Pointer to a callback function to notify the
                             client of MBMS command errors of other clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_client_mbms_reg(

    cm_client_id_type             client,
    cm_mbms_event_f_type          *mbms_event_func,
    cm_client_event_reg_e_type    event_reg_type,
    cm_mbms_event_e_type          from_mbms_event,
    cm_mbms_event_e_type          to_mbms_event,
    cm_mbms_cmd_err_f_type        *mbms_cmd_err_func

);
 
/*~ FUNCTION cm_client_mbms_reg */
/*~ PARAM mbms_event_func POINTER */
/*~ PARAM mbms_cmd_err_func POINTER */

/*===========================================================================

FUNCTION: cm_client_stats_reg

===========================================================================*/
/**
  This function allows a client to register the following statistics-related
  callbacks and events with CM:

  - Callback function to receive registered statistics events.
  - Register and deregister statistics events.

   If a client registers a callback function to receive statistics event
   notification, it must also register one or more statistics events it
   it interested in receiving. This API has a cumulative effect for event
   registration. 
  
@param[in] client             Client ID.
@param[in] stats_event_func   Pointer to a callback function to notify the
                              client of statistics events.
@param[in] event_reg_type     Register or deregister.
@param[in] from_stats_event   Register from this event (inclusive).
@param[in] to_stats_event     Register to this event (inclusive).
@param[in] stats_cmd_err_func Pointer to a callback function to notify
                              the client of statistics command errors of
                              other clients.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().
*/
cm_client_status_e_type cm_client_stats_reg(

    cm_client_id_type             client,
    cm_stats_event_f_type         *stats_event_func,
    cm_client_event_reg_e_type    event_reg_type,
    cm_stats_event_e_type         from_stats_event,
    cm_stats_event_e_type         to_stats_event,
    cm_stats_cmd_err_f_type       *stats_cmd_err_func

);
/*~ FUNCTION cm_client_stats_reg */

/*===========================================================================

FUNCTION: cm_client_act

===========================================================================*/
/**
  Activates the client object so that the client will be notified of CM
  events for which it is registered. Client-registered callback functions
  include the event callback function and other clients' command error
  callback functions. If the client is already activated, this request has
  no effect.

@param[in] client_id Requesting client.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().

@sideeffects
  Adds the client object to the client list.
*/
extern cm_client_status_e_type cm_client_act(

  cm_client_id_type               client_id

);
 
/*~ FUNCTION cm_client_act */

/*===========================================================================

FUNCTION: cm_client_deact

===========================================================================*/
/**
  Deactivates a client object so that the client will NOT be notified
  of CM events for which it is registered. Client-registered callback
  functions include the event callback function and other clients' command
  error callback functions. If the client is already disabled, this request
  has no effect.

@param[in] client_id Requesting client.

@return
  Request status.

@dependencies
  The client must have already been initialized with cm_client_init().

@sideeffects
  Removes the client object from the client list.
*/
extern cm_client_status_e_type cm_client_deact(

  cm_client_id_type               client_id

);
 
/*~ FUNCTION cm_client_deact */

/** @} */ /* end_addtogroup CO Client Object */

/*------------------------------------------------------------------------------

  Utility Functions

------------------------------------------------------------------------------*/

/** @addtogroup UF Utility Functions
    @{ */

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

#define CM_EXTDISP_MAX_SUBREC_LEN  20
  /**< Maximum subrecord length; from ANSI T1.610 Annex D. */

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/* Enumeration of band preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
/* The following values need to be consistant with sd_ss_band_pref_e_type and
** sys_band_mask_e_type. Any enum added here need to be updated in above two
** places.
*/

/* CM Band preference is a mask of various bands. Each Bit represents
** a particular band. Bit location of each band is specified in the
** sys.h in sys_sband_e_type enumeration.
**
** Please Note that certain bits are reserved as evident in bit numbering
** of sys_sband_e_type.
**
** Band_PREF ANY is a special value. We reserve Bit number 31 as a special
** Bit. If this Bit is set, then subsequent number forming out of it
** carry a special meaning, that is reserved for future. For example,
** CM_BAND_PREF_NO_CHANGE is right after CM_BAND_PREF_ANY. This has the
** effect of setting the 31st bit to 1 and takes a special meaning.
**
** If we were to add more magic valued like this, they all be after
** BAND_PREF_NO_CHANGE and in all these case, Bit 31 will be set and
** would signify a special value. Since it is 31 bit space afterwards,
** we don't anticipate to outgrow this.
*/

/** @cond
*/
#define  CM_BAND_PREF_NONE   (cm_band_pref_e_type)0   /* internal */
/** @endcond */

/** @} */ /* end_addtogroup UF */

/** @addtogroup UF Utility Functions
    @{ */

#define  CM_BAND_PREF_BC0_A  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC0_A))
    /**< Acquire band class 0, A-side systems only. */

#define  CM_BAND_PREF_BC0_B  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC0_B))
    /**< Acquire band class 0, B-side systems only. */

#define  CM_BAND_PREF_BC0    ((cm_band_pref_e_type)(SYS_BM_64BIT(SYS_SBAND_BC0_A)|SYS_BM_64BIT(SYS_SBAND_BC0_B)))
    /**< Acquire band class 0 systems only. */

#define  CM_BAND_PREF_BC1    ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC1))
    /**< Acquire band class 1 systems only. */

#define  CM_BAND_PREF_BC3    ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC3))
    /**< Acquire band class 3 systems only. */

#define  CM_BAND_PREF_BC4    ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC4))
    /**< Acquire band class 4 systems only. */

#define  CM_BAND_PREF_BC5    ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC5))
    /**< Acquire band class 5 systems only. */

#define  CM_BAND_PREF_GSM_DCS_1800  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_GSM_DCS_1800))
    /**< Acquire GSM DCS systems only. */

#define  CM_BAND_PREF_GSM_EGSM_900  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_GSM_EGSM_900))
    /**< Acquire GSM Extended GSM 900 systems only. */

#define  CM_BAND_PREF_GSM_PGSM_900  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_GSM_PGSM_900))
    /**< Acquire GSM Primary GSM 900 systems only. */

#define  CM_BAND_PREF_BC6  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC6))
    /**< Acquire band class 6 systems only. */

#define  CM_BAND_PREF_BC7  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC7))
    /**< Acquire band class 7 systems only. */

#define  CM_BAND_PREF_BC8 ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC8))
    /**< Acquire band class 8 systems only. */

#define  CM_BAND_PREF_BC9  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC9))
    /**< Acquire band class 9 systems only. */

#define  CM_BAND_PREF_BC10  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC10))
    /**< Acquire band class 10 systems only. */

#define  CM_BAND_PREF_BC11  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC11))
    /**< Acquire band class 11 systems only. */

#define  CM_BAND_PREF_BC12  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC12))
    /**< Acquire band class 12 systems only. Clients must check for
         CM_API_BAND_CLASS_12_14_15_16 before using this macro. */

#define  CM_BAND_PREF_BC14  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC14))
    /**< Acquire band class 14 systems only. Clients must check for
         CM_API_BAND_CLASS_12_14_15_16 before using this macro. */

#define  CM_BAND_PREF_BC15  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC15))
    /**< Acquire band class 15 systems only. Clients must check for
         CM_API_BAND_CLASS_12_14_15_16 before using this macro. */

#define  CM_BAND_PREF_BC16  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC16))
    /**< Acquire band class 16 systems only. Clients must check for
         CM_API_BAND_CLASS_12_14_15_16 before using this macro. */

#define  CM_BAND_PREF_GSM_450  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_450 ))
    /**< Acquire GSM 450 systems only. */

#define  CM_BAND_PREF_GSM_480  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_480 ))
    /**< Acquire GSM 480 systems only. */

#define  CM_BAND_PREF_GSM_750  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_750 ))
    /**< Acquire GSM 750 systems only. */

#define  CM_BAND_PREF_GSM_850  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_850 ))
    /**< Acquire GSM 850 systems only. */

#define  CM_BAND_PREF_GSM_RGSM_900  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_GSM_RGSM_900))
    /**< Acquire GSM Railway GSM 900 systems only. */

#define  CM_BAND_PREF_GSM_PCS_1900  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_PCS_1900 ))
    /**< Acquire GSM PCS systems only. */

#define  CM_BAND_PREF_WCDMA_I_IMT_2000  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_I_IMT_2000 ))
    /**< Acquire WCDMA I IMT 2000 systems only. */

#define  CM_BAND_PREF_WCDMA_II_PCS_1900  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_II_PCS_1900 ))
    /**< Acquire WCDMA II PCS systems only. */

#define  CM_BAND_PREF_WCDMA_III_1700  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_III_1700 ))
    /**< Acquire WCDMA III 1700 systems only. */

#define  CM_BAND_PREF_WCDMA_IV_1700  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_IV_1700 ))
    /**< Acquire WCDMA IV 1700 systems only. */

#define  CM_BAND_PREF_WCDMA_V_850  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_V_850 ))
    /**< Acquire WCDMA V 850 systems only. */

#define  CM_BAND_PREF_WCDMA_VI_800  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_VI_800 ))
    /**< Acquire WCDMA VI 800 systems only. */

/* Clients need to check for CM_API_WCDMA900 for the following three band
** classes.
** - CM_BAND_PREF_WCDMA_VII_2600
** - CM_BAND_PREF_WCDMA_VIII_900
** - CM_BAND_PREF_WCDMA_IX_1700
*/
#define CM_BAND_PREF_WCDMA_VII_2600      ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_VII_2600 ))
    /**< Acquire WCDMA Europe 2600 systems only. */

#define CM_BAND_PREF_WCDMA_VIII_900      ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_VIII_900 ))
    /**< Acquire WCDMA Europe and China 900 systems only. */

#define CM_BAND_PREF_WCDMA_IX_1700      ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_IX_1700 ))
    /**< Acquire WCDMA Japan 1700 systems only. */

#define CM_BAND_PREF_WCDMA_XI_1500      ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_XI_1500 ))
    /**< Acquire WCDMA 1500 systems only. */

#define CM_BAND_PREF_WCDMA_XIX_850      ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_XIX_850 ))
    /**< Acquire WCDMA Japan 850 systems only. */

  /* Clients check CM_API_WLAN for WLAN support for WLAN bands.*/
#define  CM_BAND_PREF_WLAN_2400_US  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_US_2400))
    /**< Acquire WLAN in U.S. 2.4 GHz. */

#define  CM_BAND_PREF_WLAN_2400_JP  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_2400))
    /**< Acquire WLAN in Japan 2.4 GHz. */

#define  CM_BAND_PREF_WLAN_2400_ETSI ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_2400))
    /**< Acquire WLAN in Europe 2.4 GHz. */

#define CM_BAND_PREF_WLAN_2400_SP    ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_2400))
    /**< Acquire WLAN in Spain 2.4 GHz. */

#define CM_BAND_PREF_WLAN_2400_FR    ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_2400))
    /**< Acquire WLAN in France 2.4 GHz. */

#define  CM_BAND_PREF_WLAN_5000_US  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_US_5000))
    /**< Acquire WLAN in U.S. 5 GHz. */

#define  CM_BAND_PREF_WLAN_5000_JP  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_5000))
    /**< Acquire WLAN in Japan 5 GHz. */

#define  CM_BAND_PREF_WLAN_5000_ETSI ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_5000))
    /**< Acquire WLAN in Europe 5 GHz. */

#define CM_BAND_PREF_WLAN_5000_SP    ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_5000))
    /**< Acquire WLAN in Spain 5 GHz. */

#define CM_BAND_PREF_WLAN_5000_FR    ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_5000))
    /**< Acquire WLAN in France 5 GHz. */

/** @} */ /* end_addtogroup UF */

/** @addtogroup UF Utility Functions
    @{ */

#define CM_BAND_PREF_LTE_EUTRAN_BAND1   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND1))
#define CM_BAND_PREF_LTE_EUTRAN_BAND2   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND2))
#define CM_BAND_PREF_LTE_EUTRAN_BAND3   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND3))
#define CM_BAND_PREF_LTE_EUTRAN_BAND4   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND4))
#define CM_BAND_PREF_LTE_EUTRAN_BAND5   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND5))
#define CM_BAND_PREF_LTE_EUTRAN_BAND6   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND6))
#define CM_BAND_PREF_LTE_EUTRAN_BAND7   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND7))
#define CM_BAND_PREF_LTE_EUTRAN_BAND8   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND8))
#define CM_BAND_PREF_LTE_EUTRAN_BAND9   ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND9))
#define CM_BAND_PREF_LTE_EUTRAN_BAND10  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND10))
#define CM_BAND_PREF_LTE_EUTRAN_BAND11  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND11))
#define CM_BAND_PREF_LTE_EUTRAN_BAND12  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND12))
#define CM_BAND_PREF_LTE_EUTRAN_BAND13  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND13))
#define CM_BAND_PREF_LTE_EUTRAN_BAND14  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND14))
#define CM_BAND_PREF_LTE_EUTRAN_BAND17  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND17))
#define CM_BAND_PREF_LTE_EUTRAN_BAND18  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND18))
#define CM_BAND_PREF_LTE_EUTRAN_BAND19  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND19))
#define CM_BAND_PREF_LTE_EUTRAN_BAND20  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND20))
#define CM_BAND_PREF_LTE_EUTRAN_BAND21  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND21))
#define CM_BAND_PREF_LTE_EUTRAN_BAND24  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND24))
#define CM_BAND_PREF_LTE_EUTRAN_BAND25  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND25))
#define CM_BAND_PREF_LTE_EUTRAN_BAND33  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND33))
#define CM_BAND_PREF_LTE_EUTRAN_BAND34  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND34))
#define CM_BAND_PREF_LTE_EUTRAN_BAND35  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND35))
#define CM_BAND_PREF_LTE_EUTRAN_BAND36  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND36))
#define CM_BAND_PREF_LTE_EUTRAN_BAND37  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND37))
#define CM_BAND_PREF_LTE_EUTRAN_BAND38  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND38))
#define CM_BAND_PREF_LTE_EUTRAN_BAND39  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND39))
#define CM_BAND_PREF_LTE_EUTRAN_BAND40  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND40))
#define CM_BAND_PREF_LTE_EUTRAN_BAND41  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND41))
#define CM_BAND_PREF_LTE_EUTRAN_BAND42  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND42))
#define CM_BAND_PREF_LTE_EUTRAN_BAND43  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_LTE_EUTRAN_BAND43))


/** LTE band preference of any of the defined values.
*/
#define CM_BAND_PREF_LTE_ANY            (CM_BAND_PREF_LTE_EUTRAN_BAND1 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND2 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND3 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND4 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND5 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND6 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND7 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND8 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND9 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND10 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND11 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND12 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND13 |\
                                         CM_BAND_PREF_LTE_EUTRAN_BAND14 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND17 |\
                                         CM_BAND_PREF_LTE_EUTRAN_BAND18 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND19 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND20 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND21 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND24 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND25 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND33 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND34 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND35 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND36 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND37 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND38 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND39 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND40 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND41 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND42 | \
                                         CM_BAND_PREF_LTE_EUTRAN_BAND43)

/** @cond
*/
#define  CM_BAND_PREF_LTE_MAX       (cm_band_pref_e_type)(CM_BAND_PREF_LTE_ANY + 1)
    /* internal */
/** @endcond
*/

/* When CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE is defined, updated the
** CM_BAND_PREF_ANY to align with CM_BAND_PREF_NO_CHANGE. */

#define  CM_BAND_PREF_ANY   (cm_band_pref_e_type)0xFFFFFFFFBFFFFFFFULL
    /**< Acquire any of the band class systems. ULL designates it as Unsigned
         Long Long (ULL) to get rid of compiler warnings "literally treated as
         unsigned long long". */

/* When CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE is defined, updated the
** CM_BAND_PREF_MAX to align with CM_BAND_PREF_NO_CHANGE. */

/** @cond
*/
#define  CM_BAND_PREF_MAX       (cm_band_pref_e_type)(CM_BAND_PREF_ANY + 1)
    /* internal */
/** @endcond
*/

#define  CM_BAND_PREF_PERSISTENT        ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_PERSISTENT))
    /**< Return to the value given by NV. Clients must check for
         CM_API_PERSISTENT_PREF before using this. */

#define CM_CALL_ID_NONE             ((cm_call_id_type) 0xFC)
  /**< No call ID type. */

/* Cellular and PCS band preference to band-class preference mapping.
**
** Note that IS-683-A PRL only provide support for one cellular band and one
** PCS band. Therefore we define a Cellular and PCS band preferences and map
** them to different band-classes preferences depending on the market, as
** follows:
**
** Cellular band preference is mapped to band-class 3 preference for the
** Japanese market or band-class 0 preference for any other market.
**
** PCS band preference is mapped to band-class 4 preference for the Korean
** market, band-class 5 preference for Band Class 5 market or band-class 1
** preference for any other market.
*/

  #define CM_BAND_PREF_CELL_A  CM_BAND_PREF_BC0_A  /**< US Cell A. */
  #define CM_BAND_PREF_CELL_B  CM_BAND_PREF_BC0_B  /**< US Cell B. */
  #define CM_BAND_PREF_CELL    CM_BAND_PREF_BC0    /**< US Cell A/B. */

  #define CM_BAND_PREF_PCS   CM_BAND_PREF_BC1      /**< US PCS. */

/** @} */ /* end_addtogroup UF */

/** @addtogroup UF
    @{ */

/*---------------------------------------------------------------------------
** Datatypes
**-------------------------------------------------------------------------*/

/** @brief Extended display subrecord (FEATURE_IS95B_EXT_DISP).
*/
typedef struct {

  uint8 display_tag;
    /**< Indicator of the display information type. */

  uint8 display_len;
    /**< The display length. */

  uint8 chari[CM_EXTDISP_MAX_SUBREC_LEN];
    /**< Display characters. */

} cm_ext_disp_subrecord;


/** @brief Extended display subrecord iterator (FEATURE_IS95B_EXT_DISP).
*/
typedef struct {

  cm_alpha_s_type*  info_ptr;
    /**< Pointer to information being iterated. */
    /*~ FIELD cm_ext_disp_itr_type.info_ptr POINTER */

  uint8 curr_offset;
    /**< Byte offset of start of current record. */

} cm_ext_disp_itr_type;
/*~ TYPE struct cm_ext_disp_itr_type */

/** @} */ /* end_addtogroup UF */

/** @addtogroup UF
    @{ */

/*---------------------------------------------------------------------------
** Macros
**-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
** Functions
**-------------------------------------------------------------------------*/

/*                 MISC CONVERSION FUNCTIONS                               */

/*===========================================================================

FUNCTION: cm_util_def_alphabet_to_ascii

===========================================================================*/
/**
   Converts the default alphabet to ASCII.

@note1hang USE THE cm_util_gsm_alphabet_to_ascii() FUNCTION INSTEAD OF THIS
  FUNCTION.\n This function has bugs and exists for backward compatibility only.

@param[in] default_alphabet_string Default alphabet string.
@param[in] num_chars               Number of characters.
@param[out] ascii_string           Resulting ASCII string.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_def_alphabet_to_ascii(

    const byte    *default_alphabet_string,
    byte          num_chars,
    byte          *ascii_string

);
/*~ FUNCTION cm_util_def_alphabet_to_ascii */
/*~ PARAM IN default_alphabet_string VARRAY LENGTH num_chars */
/*~ PARAM OUT ascii_string STRING MAX_DISPLAY_TEXT_LEN */

/*===========================================================================

FUNCTION: cm_util_number_to_bcd

===========================================================================*/
/**
   Converts a phone number from ASCII to Binary Coded Decimal
   (BCD).

@param[in] number      Phone number to convert.
@param[out] bcd_number Resulting BCD number.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_number_to_bcd(

    const cm_num_s_type *number,
    byte                *bcd_number

);
/*~ FUNCTION cm_util_number_to_bcd */
/*~ PARAM number POINTER */
/*~ PARAM OUT bcd_number VARRAY CM_CALLED_PARTY_BCD_WITH_BCD_LEN 
    LENGTH (bcd_number[0]+1) */

/*===========================================================================

FUNCTION: cm_util_bcd_to_ascii

===========================================================================*/
/**
   Converts a phone number from BCD to ASCII.

@param[in] bcd_number    BCD number to convert.
@param[out] ascii_number Resulting ASCII number.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_bcd_to_ascii(

    const byte    *bcd_number,
    byte          *ascii_number

);
/*~ FUNCTION cm_util_bcd_to_ascii */
/*~ PARAM IN bcd_number VARRAY CM_CALLED_PARTY_BCD_WITH_BCD_LEN 
    LENGTH (bcd_number[0]+1) */
/*~ PARAM OUT ascii_number STRING CM_MAX_NUMBER_CHARS */

/*===========================================================================

FUNCTION: cm_util_ascii_to_def_alphabet

===========================================================================*/
/**
   Converts the ASCII to default alphabet.

@note1hang USE THE cm_util_ascii_to_gsm_alphabet() FUNCTION INSTEAD OF THIS
  FUNCTION.\n This function has bugs and exists for backward compatibility only.

@param[out] default_alphabet_string Default alphabet string.
@param[in] num_chars                Number of characters.
@param[in] ascii_string             Resulting ASCII string.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_ascii_to_def_alphabet(

    byte          *default_alphabet_string,
    byte          num_chars,
    const byte    *ascii_string

);
/*~ FUNCTION cm_util_ascii_to_def_alphabet */
/*~ PARAM OUT default_alphabet_string VARRAY MAX_DISPLAY_TEXT_LEN 
    LENGTH ((num_chars *7)%8)?(((num_chars*7)/8)+1):((num_chars*7)/8)*/
/*~ PARAM IN ascii_string VARRAY LENGTH num_chars */

/*===========================================================================

FUNCTION: cm_util_pack

===========================================================================*/
/**
   Packs the bits in a byte.

@note1hang USE THE cm_util_ussd_pack() FUNCTION INSTEAD OF THIS FUNCTION.\n This
 function has bugs and exists for backward compatibility only.

@param[out] coded_data       Coded data bits.
@param[in] num_chars         Number of characters.
@param[in] string_to_convert String of bits to convert.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_pack(

  byte         *coded_data,
  byte         num_chars,
  const byte   *string_to_convert

);
/*~ FUNCTION cm_util_pack */
/*~ PARAM OUT coded_data VARRAY ((num_chars *7)%8)?(((num_chars*7)/8)+1):((num_chars*7)/8)
    LENGTH ((num_chars *7)%8)?(((num_chars*7)/8)+1):((num_chars*7)/8) */
/*~ PARAM IN string_to_convert VARRAY LENGTH num_chars */

/*===========================================================================

FUNCTION: cm_util_unpack

===========================================================================*/
/**
   Unpacks the bits into bytes.

@note1hang USE THE cm_util_ussd_unpack() FUNCTION INSTEAD OF THIS FUNCTION.\n
This function has bugs and exists for backward compatibility only.

@param[in] user_data       Packed bit string.
@param[in] num_chars       Length of packed string.
@param[out] message_string Unpacked byte string.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_unpack(const byte *user_data, byte num_chars, byte *message_string);
/*~ FUNCTION cm_util_unpack */
/*~ PARAM IN user_data VARRAY LENGTH num_chars */
/*~ PARAM OUT message_string VARRAY ((num_chars *8)%7)?(((num_chars*8)/7)+1):((num_chars*8)/7)
    LENGTH ((num_chars *8)%7)?(((num_chars*8)/7)+1):((num_chars*8)/7) */

/*===========================================================================

FUNCTION: cm_util_ussd_pack

===========================================================================*/
/**
   Packs 7-bit GSM characters into bytes (8-bits).

@param[out] packed_data Packed GSM data.
@param[in] str          Character string.
@param[in] num_chars    Number of characters.

@return
  None.

@dependencies
  None.
*/
extern byte cm_util_ussd_pack(

    byte       *packed_data,
    const byte *str,
    byte       num_chars

);
/*~ FUNCTION cm_util_ussd_pack */
/*~ PARAM OUT packed_data VARRAY ((num_chars *7)%8)?(((num_chars*7)/8)+1):((num_chars*7)/8)
    LENGTH ((num_chars *7)%8)?(((num_chars*7)/8)+1):((num_chars*7)/8) */
/*~ PARAM IN str VARRAY LENGTH num_chars */

/*===========================================================================

FUNCTION: cm_util_ussd_unpack

===========================================================================*/
/**
   Unpacks the bytes (8-bit) into 7-bit GSM characters.

@param[out] str        8-bit strings.
@param[in] packed_data Packed data to unpack.
@param[in] num_bytes   Number of bytes.

@return
  None.

@dependencies
  None.
*/
extern byte cm_util_ussd_unpack(

    byte       *str,
    const byte *packed_data,
    byte       num_bytes

);
/*~ FUNCTION cm_util_ussd_unpack */
/*~ PARAM OUT str VARRAY ((num_bytes *8)%7)?(((num_bytes*8)/7)+1):((num_bytes*8)/7)
    LENGTH ((num_bytes *8)%7)?(((num_bytes*8)/7)+1):((num_bytes*8)/7) */
/*~ PARAM IN packed_data VARRAY LENGTH num_bytes */

/*===========================================================================

FUNCTION: cm_util_ascii_to_gsm_alphabet

===========================================================================*/
/**
   Converts the ASCII string to a GSM default alphabet string
   and packs it into bytes.

@param[out] gsm_alphabet_string GSM alphabet string.
@param[in] ascii_string         ASCII string to convert.
@param[in] num_chars            Number of characters.

@return 
  None.

@dependencies
  None.
*/
extern byte cm_util_ascii_to_gsm_alphabet(

    byte          *gsm_alphabet_string,
    const byte    *ascii_string,
    byte          num_chars

);
/*~ FUNCTION cm_util_ascii_to_gsm_alphabet */
/*~ PARAM OUT gsm_alphabet_string VARRAY MAX_DISPLAY_TEXT_LEN 
    LENGTH ((num_chars *7)%8)?(((num_chars*7)/8)+1):((num_chars*7)/8) */
/*~ PARAM IN ascii_string VARRAY LENGTH num_chars */

/*===========================================================================

FUNCTION: cm_util_gsm_alphabet_to_ascii

===========================================================================*/
/**
   Unpacks bytes of data into a 7-bit GSM default alphabet
   string and then converts it to an ASCII string.

@param[out] ascii_string       Resulting ASCII string.
@param[in] gsm_alphabet_string GSM alphabet string.
@param[in] num_bytes           Number of bytes.

@return
  None.

@dependencies
  None.
*/
extern byte cm_util_gsm_alphabet_to_ascii(

    byte         *ascii_string,
    const byte   *gsm_alphabet_string,
    byte         num_bytes

);
/*~ FUNCTION cm_util_gsm_alphabet_to_ascii */
/*~ PARAM OUT ascii_string STRING MAX_DISPLAY_TEXT_LEN */
/*~ PARAM IN gsm_alphabet_string VARRAY LENGTH num_bytes */

/** @} */ /* end_addtogroup UF */

/** @addtogroup UF
    @{ */

/** Type for an M511 mode change function. */
typedef boolean ( cm_m511_mode_settings_f_type ) (

  sys_jcdma_m511_e_type      new_mode
    /**< New mode M511. */

);
 
/*~ CALLBACK cm_m511_mode_settings_f_type
  ONERROR return FALSE */
 
/** Type for an M512 mode change function. */
typedef boolean ( cm_m512_mode_settings_f_type ) (

  sys_jcdma_m512_e_type      new_mode
    /**< New mode M512. */

);
 
/*~ CALLBACK cm_m512_mode_settings_f_type
  ONERROR return FALSE */
 
/** Type for an M513 mode change function. */
typedef boolean ( cm_m513_mode_settings_f_type ) (

  sys_jcdma_m513_e_type      new_mode
    /**< New mode M513. */

);
 
/*~ CALLBACK cm_m513_mode_settings_f_type
  ONERROR return FALSE */

/** @} */ /* end_addtogroup UF */

/** @addtogroup UF
    @{ */

/*===========================================================================

FUNCTION: cm_change_m511_mode

===========================================================================*/
/**
   Requests DS to change to M511 mode by calling the appropriate
   handler function in the M51 mode object.

@param[in] new_mode New M511 mode change requested.

@return
  TRUE -- Mode change was successful. \n
  FALSE -- Mode change failed.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern boolean cm_change_m511_mode(sys_jcdma_m511_e_type new_mode);

 
/*~ FUNCTION cm_change_m511_mode */

/*===========================================================================

FUNCTION: cm_change_m512_mode

===========================================================================*/
/**
   Requests DS to change to M512 mode by calling the appropriate
   handler function in the M51 mode object.

@param[in] New M512 mode change requested.

@return
  TRUE -- Mode change was successful. \n
  FALSE -- Mode change failed.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern boolean cm_change_m512_mode(sys_jcdma_m512_e_type new_mode);

 
/*~ FUNCTION cm_change_m512_mode */

/*===========================================================================

FUNCTION: cm_change_m513_mode

===========================================================================*/
/**
   Requests DS to change to M513 mode by calling the appropriate
   handler function in the M51 mode object.

@param[in] new_mode New M513 mode change requested.

@return
  TRUE -- Mode change was successful. \n
  FALSE -- Mode change failed.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern boolean cm_change_m513_mode(sys_jcdma_m513_e_type new_mode);

 
/*~ FUNCTION cm_change_m513_mode */

/*===========================================================================

FUNCTION: cm_m51_mode_reg

===========================================================================*/
/**
   Registers M51 mode change callback functions with the CM.
   When cm_change_m511/512/513 mode functions are called by
   the UI, the CM looks at the M51 mode object and calls the
   appropriate handler that was registered for the required
   mode change.

@param[in] change_m511_mode_func Pointer to a DS function that changes
                                 to M511 mode.
@param[in] change_m512_mode_func Pointer to a DS function that changes
                                 to M512 mode.
@param[in] change_m513_mode_func Pointer to a DS function that changes
                                 to M513 mode.

@return
  TRUE -- Registration was successful. \n
  FALSE -- Registration failed.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start().
*/
extern boolean cm_m51_mode_reg
(

  cm_m511_mode_settings_f_type     *change_m511_mode_func,
  cm_m512_mode_settings_f_type     *change_m512_mode_func,
  cm_m513_mode_settings_f_type     *change_m513_mode_func

);
 
/*~ FUNCTION cm_m51_mode_reg */

/*===========================================================================

FUNCTION: cm_is_valid_mode_band_pref

===========================================================================*/
/**
   Checks if a specified combination of mode and band preference
   is supported by the current target and the Preferred Roaming
   List (PRL) that is associated with the currently selected
   NAM.

@param[in] mode_pref Mode preference to check.
@param[in] band_pref Band preference to check.

@return
  TRUE -- Specified combination of mode and band preference is supported. \n
  FALSE -- Specified combination of mode and band preference is not supported.

@dependencies
  None.
*/
extern boolean cm_is_valid_mode_band_pref(

  cm_mode_pref_e_type    mode_pref,
  cm_band_pref_e_type    band_pref

);
 
/*~ FUNCTION cm_is_valid_mode_band_pref */

/*===========================================================================

FUNCTION: cm_is_valid_mode_band_pref_with_lte

===========================================================================*/
/**
  Checks whether a specified combination of mode and band preference is
  supported by the current target and the PRL that is associated with the
  currently selected NAM.

@param[in] mode_pref     Mode preference to check.
@param[in] band_pref     2G/3G band preference to check.
@param[in] lte_band_pref LTE band preference to check.

@return
  TRUE -- Specified combination of mode and band preference is supported. \n
  FALSE -- Specified combination of mode and band preference is not supported.

@dependencies
  None.
*/
boolean cm_is_valid_mode_band_pref_with_lte(

    cm_mode_pref_e_type    mode_pref,
    cm_band_pref_e_type    band_pref,
    cm_band_pref_e_type    lte_band_pref

);

/*~ FUNCTION cm_is_valid_mode_band_pref_with_lte */

/*===========================================================================

FUNCTION: cm_is_valid_mode_pref

===========================================================================*/
/**
   Checks if a given mode preference is valid.

@param[in] mode_pref Mode preference to check.

@return
  TRUE -- Mode preference is valid. \n
  FALSE -- Mode preference is not valid.

@dependencies
  None.
*/
extern boolean cm_is_valid_mode_pref(

  cm_mode_pref_e_type    mode_pref

);
 
/*~ FUNCTION cm_is_valid_mode_pref */

/*===========================================================================

FUNCTION: cm_is_sys_mode_digital

===========================================================================*/
/**
   Determines if the given system mode is a digital mode.

@param[in] sys_mode System mode to check.

@return
  TRUE -- System mode is digital. \n
  FALSE -- System mode is not digital.

@dependencies
  None.
*/
extern boolean cm_is_sys_mode_digital(

  sys_sys_mode_e_type    sys_mode

);
 
/*~ FUNCTION cm_is_sys_mode_digital */

/*===========================================================================

FUNCTION: cm_get_pref_srv_type_for_packet_orig

===========================================================================*/
/**
   Returns the service type on which the next packet data call
   is likely to be originated.

@return
  System mode of the serving or attempted system. For 3GPP, GSM, WCDMA, and
  LTE are all grouped in one indication which, is GSM.

@dependencies
  None.
*/
extern sys_sys_mode_e_type cm_get_pref_srv_type_for_packet_orig(

  void

);
 
/*~ FUNCTION cm_get_pref_srv_type_for_packet_orig */

/*===========================================================================

FUNCTION: CM_GET_CUR_HYBR_STATUS

===========================================================================*/
/**
   Gets the current hybrid status. Applicable for FEATURE_HDR_HYBRID.

@return
  TRUE -- Hybrid operation is enabled. \n
  FALSE -- Hybrid operation is not enabled.

@dependencies
  The phone object must have already been initialized with cmph_init().
*/
boolean cm_get_cur_hybr_status(void);
 
/*~ FUNCTION cm_get_cur_hybr_status */

/*===========================================================================

FUNCTION: cm_num_init

===========================================================================*/
/**
    Initializes a CM number object.

@param[out] num_ptr Number to initialize.

@return 
  None.

@dependencies
  This function must be called on a CM number object before it can be used in
  any other function.
*/
extern void cm_num_init(

  cm_num_s_type *num_ptr

);
 
/*~ FUNCTION cm_num_init */
/*~ PARAM OUT num_ptr POINTER */

/*===========================================================================

FUNCTION: cm_num_fill

===========================================================================*/
/**
   Initializes a CM number object and fills it with a number.

@param[out] num_ptr   Pointer to a CM number object to initialize.
@param[in] num_buf    Dialed address buffer. \n
                      @note1 If the digit_mode parameter is set to
                             CM_DIGIT_MODE_4BIT_DTMF, the number buffer may
                             only contain the 0-9, #, or * characters.
                             If the digit_mode parameter is set to
                             CM_DIGIT_MODE_8BIT_ASCII, the number buffer may
                             contain any 8-bit ASCII characters.
@param[in] num_len    Dialed address length.
@param[in] digit_mode Digit mode.

@return
  None.

@dependencies
  None.

*/
extern void cm_num_fill(

    cm_num_s_type          *num_ptr,
    const uint8            *num_buf,
    uint8                  num_len,
    cm_digit_mode_e_type   digit_mode

);
 
/*~ FUNCTION cm_num_fill */
/*~ PARAM OUT num_ptr POINTER */
/*~ PARAM num_buf VARRAY LENGTH num_len */

/*===========================================================================

FUNCTION: CM_EXT_DISP_ITR_RESET

===========================================================================*/
/**
  Resets the iterator that traverses through the extended display subrecords,
  which exist in a packed format in the alpha field of the call event
  information.

  Applicable for FEATURE_IS95B_EXT_DISP.

@param[out] itr_ptr  Pointer to the iterator.
@param[in] info_ptr  Pointer to the packed information structure.

@return
  None.

@dependencies
  None.
*/
extern void cm_ext_disp_itr_reset(

  cm_ext_disp_itr_type  *itr_ptr,
  cm_alpha_s_type       *info_ptr

);
/*~ FUNCTION cm_ext_disp_itr_reset */
/*~ PARAM OUT itr_ptr POINTER */
/*~ PARAM info_ptr POINTER */



/*===========================================================================

FUNCTION: CM_EXT_DISP_ITR_NEXT

===========================================================================*/
/**
  Increments the iterator that traverses through the extended display subrecords,
  which exist in a packed format in the alpha field of the call event
  information.

  If the end was already reached, the iterator is unchanged.

  Applicable for FEATURE_IS95B_EXT_DISP.

@param[out] itr_ptr Pointer to the iterator.

@return
  None.

@dependencies
  The iterator is initialized using cm_ext_disp_itr_reset.
*/
extern void cm_ext_disp_itr_next(

  cm_ext_disp_itr_type *itr_ptr

);
/*~ FUNCTION cm_ext_disp_itr_next */
/*~ PARAM OUT itr_ptr POINTER */

/*===========================================================================

FUNCTION: CM_EXT_DISP_ITR_MORE

===========================================================================*/
/**
  Tests the iterator to see if it points to the end of the records.

  Applicable for FEATURE_IS95B_EXT_DISP.

@param[in] itr_ptr Pointer to the iterator.

@return
  TRUE -- Iterator points to the end of the record. \n
  FALSE -- Iterator points past the end of the record. 

@dependencies
  The iterator is initialized using cm_ext_disp_itr_reset.
*/
extern boolean cm_ext_disp_itr_more(

  const cm_ext_disp_itr_type *itr_ptr

);
/*~ FUNCTION cm_ext_disp_itr_more */
/*~ PARAM itr_ptr POINTER */

/*===========================================================================

FUNCTION: CM_EXT_DISP_ITR_GET

===========================================================================*/
/**
   Copies the contents of the current record pointed to by the
   iterator into the supplied cm_ext_disp_subrecord structure.
   Applicable for FEATURE_IS95B_EXT_DISP.

@param[in] itr_ptr     Pointer to the iterator.
@param[out] subrec_ptr Area to which the next record is copied.

@return
  None.

@dependencies
  The iterator must have already been initialized using cm_ext_disp_itr_reset.
*/
extern void cm_ext_disp_itr_get(

  const cm_ext_disp_itr_type *itr_ptr,
  cm_ext_disp_subrecord      *subrec_ptr

);
/*~ FUNCTION cm_ext_disp_itr_get */
/*~ PARAM itr_ptr POINTER */
/*~ PARAM OUT subrec_ptr POINTER */

/*===========================================================================

FUNCTION: cm_get_current_acq_sys_mode

===========================================================================*/
/**
   Returns the current system mode that is acquired.

@return
  System mode.

@dependencies
  None.
*/
sys_sys_mode_e_type cm_get_current_acq_sys_mode( void );
 
/*~ FUNCTION cm_get_current_acq_sys_mode */

/*===========================================================================

FUNCTION: cm_pd_cmd_fix_start

===========================================================================*/
/**
   Sends a start fix command to the CM and places it in the CM queue.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client         Requesting client ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start().
*/
extern boolean cm_pd_cmd_fix_start(

    cm_pd_cmd_cb_f_type   cmd_cb_func,
    void                  *data_block_ptr,
    cm_client_id_type     client

);
 
/*~ FUNCTION cm_pd_cmd_fix_start */

/*===========================================================================

FUNCTION: cm_pd_cmd_fix_end

===========================================================================*/
/**
   Sends an end fix command to the CM and places it in the CM queue.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to client callback data block.
@param[in] client         Requesting client ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start()
  and cm_init_after_task_start().
*/
extern boolean cm_pd_cmd_fix_end(

    cm_pd_cmd_cb_f_type   cmd_cb_func,
    void                  *data_block_ptr,
    cm_client_id_type     client

    );
 
/*~ FUNCTION cm_pd_cmd_fix_end */

/*===========================================================================

FUNCTION: cm_wlan_pref_init

===========================================================================*/
/**
   Initializes WLAN preferences.

@param[out] wlan_pref Pointer to a WLAN preference structure.

@return
  None.

@dependencies
  Clients must check CM_API_WLAN for WLAN support.
*/
void cm_wlan_pref_init(

    cm_wlan_pref_s_type  *wlan_pref

);
 
/*~ FUNCTION cm_wlan_pref_init */
/*~ PARAM OUT wlan_pref POINTER */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_recall_rsp

===========================================================================*/
/**
   Carries a response from a client for a recall indication from the
   network. The call ID passed has range from 0 to CM_CALL_ID_MAX.
   The client ID should be from a registered CM client.

@param[in] cmd_cb_func     Client callback function.
@param[in] data_block_ptr  Pointer to the client callback data block.
@param[in] call_id         Call ID.
@param[in] client_id       Commanding client's ID.
@param[in] ccbs_recall_rsp Client response to the recall indication.

@return 
  None.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  ((FEATURE_WCDMA or FEATURE_GSM) and FEATURE_CCBS) must be defined.
*/
extern boolean cm_mm_call_cmd_recall_rsp (

  cm_call_cmd_cb_f_type         cmd_cb_func,
  void                          *data_block_ptr,
  cm_call_id_type               call_id,
  cm_client_id_type             client_id,
  cm_ccbs_recall_rsp_e_type     ccbs_recall_rsp

);
 
/*~ FUNCTION cm_mm_call_cmd_recall_rsp */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_modify_req

===========================================================================*/
/**
   Requests an in-call (connected state) modification of the call
   type. A call must be connected for this request to be
   processed. The call ID passed has a range from 0 to
   CM_CALL_ID_MAX. The client ID should be from a registered CM
   client.

@param[in] cmd_cb_func       Client callback function.
@param[in] data_block_ptr    Pointer to client callback data block.
@param[in] client_id         Commanding client's ID.
@param[in] call_id           Call ID.
@param[in] new_call_type_req New call type requested for call modification.

@return
  TRUE -- Command has been queued to the CM. \n
  FALSE -- Command did not get queued to the CM.

@dependencies
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA or FEATURE_GSM) and CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

@sideeffects None.

*/
boolean cm_mm_call_cmd_modify_req (

  cm_call_cmd_cb_f_type         cmd_cb_func,
  void                          *data_block_ptr,
  cm_client_id_type             client_id,
  cm_call_id_type               call_id,
  cm_call_type_e_type           new_call_type_req

);
 
/*~ FUNCTION cm_mm_call_cmd_modify_req */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_modify_res

===========================================================================*/
/**
   Response to a network-initiated in-call modification request.
   The call must be connected for this request to be
   processed. The call ID passed has range from 0 to
   CM_CALL_ID_MAX. The client ID should be from a registered CM
   client.

@param[in] cmd_cb_func      Client callback function.
@param[in] data_block_ptr   Pointer to the client callback data block.
@param[in] client_id        Commanding client's ID.
@param[in] call_id          Call ID.
@param[in] gw_cs_modify_res Client response to the modification request;
                            TRUE if the modification is accepted by the
                            client, FALSE otherwise.
@param[in] cc_cause_ptr     Specific cause value when gw_cs_modify_res is
                            FALSE. Ignored when gw_cs_modify_res is TRUE.

@return 
  TRUE -- Command has been queued to the CM. \n
  FALSE -- Command did not get queued to the CM.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  ((FEATURE_WCDMA or FEATURE_GSM) and CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.
*/
boolean cm_mm_call_cmd_modify_res (

  cm_call_cmd_cb_f_type         cmd_cb_func,
  void                          *data_block_ptr,
  cm_client_id_type             client_id,
  cm_call_id_type               call_id,
  boolean                       gw_cs_modify_res,
  ie_cm_cc_cause_s_type         *cc_cause_ptr

);
 
/*~ FUNCTION cm_mm_call_cmd_modify_res */
/*~ PARAM cc_cause_ptr POINTER */

/*===========================================================================

FUNCTION: cm_get_user_data

===========================================================================*/
/**
   Used by the callback client to get details regarding user data.

@param[in] call_info_ptr         Pointer to the call information pointer
                                 sent to the client.
@param[out] call_event_user_data Pointer to a client for user data details.

@return
  None.

@dependencies
  The call object must have already been initialized with cmcall_init().
*/
extern void cm_get_user_data
(
  const cm_mm_call_info_s_type      *call_info_ptr,
  cm_call_event_user_data_s_type    *call_event_user_data

);
 
/*~ FUNCTION cm_get_user_data       */
/*~ PARAM call_info_ptr POINTER             */
/*~ PARAM OUT call_event_user_data POINTER  */

/*===========================================================================

FUNCTION: cm_ph_net_list_copy

===========================================================================*/
/**
   Copies network lists from phone event information to the client when
   requested by the client.

@param[out] info_ptr Pointer to a copy of the network lists.

@return
  None.

@dependencies
  None.
*/
extern void cm_ph_net_list_copy
(
  cm_ph_net_lists_info_s_type   *info_ptr
);
 
/*~ FUNCTION cm_ph_net_list_copy */
/*~ PARAM OUT info_ptr POINTER */

/*===========================================================================

FUNCTION: cm_ph_get_gw_subscription_source

===========================================================================*/
/**
  Determines whether a GW subscription is coming from the NV or SIM.

@return
  This function returns where GW is currently getting its 
  subscription or none if there is no subscription available.

@dependencies
  None.
*/
cm_cc_subscription_source_e_type cm_ph_get_gw_subscription_source (void); 

/*===========================================================================

FUNCTION: cm_ph_get_cdma_subscription_source

===========================================================================*/
/**
  Determines whether a CDMA subscription is coming from the NV or SIM.

@return
  This function returns where CDMA is currently getting its 
  subscription or none if there is no subscription available.

@dependencies
  None.
*/
cm_cc_subscription_source_e_type cm_ph_get_cdma_subscription_source (void);

/*===========================================================================
                          CMUTIL FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================

FUNCTION: cm_util_subaddr_to_ascii

===========================================================================*/
/**
   Converts the subaddress to ASCII.

@param[in] subaddress        Address in subnet format.
@param[out] ascii_subaddress Address in ASCII format.
@param[in] length            Length of the subnet address.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_subaddr_to_ascii(

    byte   *subaddress,
    byte   *ascii_subaddress,
    byte   length
);
/*~ FUNCTION cm_util_subaddr_to_ascii */
/*~ PARAM IN subaddress VARRAY LENGTH length  */
/*~ PARAM OUT ascii_subaddress STRING CM_MAX_SUBADDRESS_DIGITS */

/*===========================================================================

FUNCTION: cm_util_default_cdma_orig_params

===========================================================================*/
/**
   Returns a cm_cdma_orig_params_s_type of default origination parameters.

@param[out] cdma_orig_param_ptr CDMA call origination configuration.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_default_cdma_orig_params(

  cm_cdma_orig_params_s_type         *cdma_orig_param_ptr

);
/*~ FUNCTION cm_util_default_cdma_orig_params */
/*~ PARAM OUT cdma_orig_param_ptr POINTER */

/*===========================================================================

FUNCTION: cm_util_default_gw_orig_params

===========================================================================*/
/**
   Returns a cm_gw_orig_params_s_type of default origination parameters.

@param[out] gw_orig_param_ptr GW call origination configuration.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_default_gw_orig_params(

  cm_gw_cs_orig_params_s_type         *gw_orig_param_ptr

);
/*~ FUNCTION cm_util_default_gw_orig_params */
/*~ PARAM OUT gw_orig_param_ptr POINTER */

/*===========================================================================

FUNCTION: cm_util_set_default_voice_bearer_cap

===========================================================================*/
/**
   Sets the default bearer capability for a voice call.

@param[out] bc Bearer capability.

@return
  None.

@dependencies
  None.
*/
extern void cm_util_set_default_voice_bearer_cap(

  cm_bearer_capability_s_type    *bc

);
/*~ FUNCTION cm_util_set_default_voice_bearer_cap */
/*~ PARAM OUT bc POINTER */

/*===========================================================================

FUNCTION: CM_UTIL_ENCODE_DTMF

===========================================================================*/
/**
   Populates the DTMF equivalent of a given ASCII value.

@param[in] ascii_val     ASCII value.
@param[out] dtmf_val_ptr DTMF value.

@return
  TRUE -- dtmf_val contains the converted value. \n
  FALSE -- dtmf_val contains 0 (represented by CMUTIL_DTMF_FOR_DIAL_0).

@dependencies
  None.

*/
extern boolean cm_util_encode_dtmf
(
  uint8 ascii_val,
  uint8 *dtmf_val_ptr

);
/*~ FUNCTION cm_util_encode_dtmf */
/*~ PARAM OUT dtmf_val_ptr POINTER */

/*===========================================================================

FUNCTION: CM_UTIL_DTMF_TO_ASCII

===========================================================================*/
/**
   Populates the ASCII equivalent of a given DTMF value.

@param[in] dtmf_val       DTMF value.
@param[out] ascii_val_ptr ASCII value.

@return
  TRUE -- ascii_val contains the converted value. \n
  FALSE -- ascii_val contains 0 (represented by CMUTIL_DTMF_FOR_DIAL_0).

@dependencies
  None.
*/
extern boolean cm_util_dtmf_to_ascii(

  uint8 dtmf_val,
  uint8 *ascii_val_ptr

);
/*~ FUNCTION cm_util_dtmf_to_ascii */
/*~ PARAM OUT ascii_val_ptr POINTER */

/*===========================================================================

FUNCTION: cm_util_perform_num_conversion

===========================================================================*/
/**
   Performs the following conversions:

   - For CONV_REQ_PLUS_TO_DIGITS, replaces the starting "+"
     of the number with digits.
   - For CONV_REQ_DIGITS_TO_PLUS, replaces the specific
     length of the starting digits with "+".

@param[in] num_conv_req         Convert from plus to digits or
                                digits to plus.
@param[in] num_details_for_conv Holds the number on which the conversion
                                is performed.
@param[out] converted_num       Converted number as a result of the
                                operation.

@return
  The converted number.

@dependencies
  None.
*/
extern cmutil_conver_result_e_type cm_util_perform_num_conversion(

  cmutil_num_conv_req_e_type                      num_conv_req,
  const cm_num_details_for_conversion_s_type      *num_details_for_conv,
  cm_num_s_type                                   *converted_num

);
/*~ FUNCTION cm_util_perform_num_conversion */
/*~ PARAM num_details_for_conv POINTER */
/*~ PARAM OUT converted_num POINTER */

/*===========================================================================

FUNCTION: cm_util_init_num_conversion_params

===========================================================================*/
/**
   Initializes parameters that will be used in a number conversion function

@param[out] num_conv_req_ptr     Convert from plus to digits or
                                 digits to plus.
@param[out] num_details_for_conv Holds the number on which the conversion
                                 is performed.

@return
  Initialized parameters.

@dependencies
  None.
*/
extern void cm_util_init_num_conversion_params(

  cmutil_num_conv_req_e_type               *num_conv_req_ptr,
  cm_num_details_for_conversion_s_type     *num_details_for_conv

);
/*~ FUNCTION cm_util_init_num_conversion_params */
/*~ PARAM OUT num_conv_req_ptr POINTER */
/*~ PARAM OUT num_details_for_conv POINTER */

/*===========================================================================

FUNCTION: cmutil_populate_country_list

===========================================================================*/
/**
   Returns a list of countries that match a given SID, Land To
   Mobile (LTM) offset, and daylight-saving settings.

@param[in] curr_sid                     SID of the system.
@param[in] curr_ltm                     LTM offset of the system.
@param[in] curr_daylt                   Daylight-saving information.
@param[out] countries_with_sid_ltm_cnft Countries conflicting on a given SID 
                                        and LTM.

@return
  countries_with_sid_ltm_cnft contains the list of countries.

@dependencies
  None.
*/
extern void cmutil_populate_country_list (

  uint16                    curr_sid,
  int8                      curr_ltm,
  sys_daylt_savings_e_type  curr_daylt,
  cm_ss_country_list_s_type *countries_with_sid_ltm_cnft

);
/*~ FUNCTION cmutil_populate_country_list */
/*~ PARAM OUT countries_with_sid_ltm_cnft POINTER */

/*===========================================================================

FUNCTION: cmutil_identify_current_country

===========================================================================*/
/**
   Looks for a matching country based on the SID, LTM offset,
   and daylight-saving information. If a matching country is
   found, its MCC is returned through the curr_mcc parameter.

@param[in] curr_sid   SID of the system.
@param[in] curr_ltm   LTM offset of the system.
@param[in] curr_daylt Daylight-saving information.
@param[out] curr_mcc  MCC of the country with the given information.

@return
  curr_mcc containing the MCC of the current country.

@dependencies
  None.
*/
extern cmutil_current_country_e_type cmutil_identify_current_country (

  uint16                   curr_sid,
  int8                     curr_ltm,
  sys_daylt_savings_e_type curr_daylt,
  uint16                   *curr_mcc

);
/*~ FUNCTION cmutil_identify_current_country */
/*~ PARAM OUT curr_mcc POINTER */

/*===========================================================================

FUNCTION: cmutil_replace_plus_with_digits

===========================================================================*/
/**
  Specifies the number details that are required for conversion from
  plus to digits; converted_num_ptr returns the converted number.

@param[in] num_details_for_conv_ptr Details required to convert the plus digits
                                    to numbers.
@param[out] converted_num_ptr       Number with digits converted from
                                    plus to digits.

@return
  converted_num_ptr containing the number after plus has been converted 
  to digits.

@dependencies
  None.
*/
extern cmutil_conver_result_e_type cmutil_replace_plus_with_digits (

  const cm_num_details_for_conversion_s_type  *num_details_for_conv_ptr,
  cm_num_s_type                               *converted_num_ptr

); /* cmutil_replace_plus_with_digits */
/*~ FUNCTION cmutil_replace_plus_with_digits */
/*~ PARAM num_details_for_conv_ptr POINTER */
/*~ PARAM OUT converted_num_ptr POINTER */

/*===========================================================================

FUNCTION: cmutil_replace_digits_with_plus

===========================================================================*/
/**
   Specifies the number details that are required for conversion from
   digits to plus, converted_num_ptr returns the converted number.

@param[in] num_details_for_conv_ptr Details required to convert the number
                                    to plus digits.
@param[out] converted_num_ptr       Number with digits converted from
                                    digits to plus.

@return 
  converted_num_ptr containing the number after digits have been replaced
  with plus.

@dependencies
  None.
*/
cmutil_conver_result_e_type cmutil_replace_digits_with_plus (

  const cm_num_details_for_conversion_s_type  *num_details_for_conv_ptr,
  cm_num_s_type                               *converted_num_ptr

); /* cmutil_replace_digits_with_plus */
/*~ FUNCTION cmutil_replace_digits_with_plus */
/*~ PARAM num_details_for_conv_ptr POINTER */
/*~ PARAM OUT converted_num_ptr POINTER */

/*===========================================================================

FUNCTION: cm_util_country_info_for_dialing

===========================================================================*/
/**
   Provides country information for plus coding dialing (assisted dialing).

@param[in] req_mcc           Requested MCC. If the requested MCC is invalid,
                             the system will try to provide a serving MCC.
@param[out] country_info_ptr Country network information corresponding to
                             the MCC.

@return
  country_info_ptr carries country information corresponding to the MCC.

@dependencies
  None.
*/
cmutil_country_info_result_e_type cm_util_country_info_for_dialing(

  cm_country_code_type             req_mcc,
  cmutil_country_specific_record_s *country_info_ptr

); /*cm_util_country_info_for_dialing*/
/*~ FUNCTION cm_util_country_info_for_dialing */
/*~ PARAM OUT country_info_ptr POINTER */

/*===========================================================================

FUNCTION: cm_get_timing_advance  

===========================================================================*/
/**
   Gets the timing advance information.

@param[out] timing_advance_ptr Gets the timing advance information.

@return
  TRUE -- Success. \n
  FALSE -- Failure.

@dependencies
  None.
*/
extern boolean cm_get_timing_advance(

  uint32 *timing_advance_ptr 

); /*cm_get_timing_advance*/
/*~ FUNCTION cm_get_timing_advance */
/*~ PARAM OUT timing_advance_ptr POINTER */

/*===========================================================================

FUNCTION: cm_get_cdma_position_info   

===========================================================================*/
/**
   Gets the current CDMA cell position information.

@param[out] position_info_ptr Pointer to the CDMA position information
                             sent to the client.

@return
  TRUE -- Success. \n
  FALSE -- Failure.

@dependencies
  None.
*/
extern boolean cm_get_cdma_position_info(

  cm_cdma_position_info_s_type   *position_info_ptr

); /*cm_get_cdma_position_info*/
/*~ FUNCTION cm_get_cdma_position_info */
/*~ PARAM OUT position_info_ptr POINTER */

/** @} */ /* end_addtogroup UF Utility Functions */

/*===========================================================================

@FUNCTION cm_ss_wlan_security_report

@DESCRIPTION
   Queues the WLAN security params report to CM


@DEPENDENCIES
  None

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern void cm_ss_wlan_security_report (

  sys_wlan_sec_params_s_type*  wlan_sec_params

);
/*~ FUNCTION cm_ss_wlan_security_report */
/*~ PARAM wlan_sec_params POINTER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CM_H */
