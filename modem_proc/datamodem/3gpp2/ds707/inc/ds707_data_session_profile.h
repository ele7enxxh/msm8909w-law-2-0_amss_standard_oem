#ifndef DS_DATA_SESS_PROF_H
#define DS_DATA_SESS_PROF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                  DATA SESSION PROFILE DATA STRUCTURE AND CONTROL FUNCTIONS

GENERAL DESCRIPTION
  The file contains data structures and utilty functions to support Application
  profiles specific requirements and Dormant 2 (Lingering PPP session)
  requirements.

EXTERNALIZED FUNCTIONS

  DS707_DATA_SESSION_SET_PROFILE
    This function reads the EFS files containing data session profile
    parameters

  DS707_DATA_SESSION_GET_PROFILE
    This function reads data session profile parameters from
    ds707_data_session_profile_info_type[ ] for
    a particular data_session_profile_id.

  DS707_DATA_SESSION_PROFILE_INIT
    This function initializes the data session profile parameters for
    different profiles.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2004-2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_data_session_profile.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/14   sd      Fix to use credentials from OMH profile in case of card mode
                   during MIP to SIP fallback.
07/04/14   vm      Added profile parameters for stateless DHCPv6
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH
06/26/14   ms      Data+MMS - DSDA changes Phase 1.
05/22/14   sd      Fix to bypass OMH and LBS related validations if we are in 
                   eHRPD mode, or if both the existing and requesting profile
                   types are not OMH.
01/08/13   ssb     Adding utils function to set/get sip_active_profile_index 
01/19/12   jz      pdn failure fallback
10/24/11   jz      Add PDN LABEL in profile 
10/27/11   op      Added support for PDN level authentication 
08/03/11   var     Added return value for ds707_data_session_get_profile
06/03/11   ttv     Changed the rat_type to be in sync with DS Profile.
05/25/11   ack     Adding profile persistence option
05/06/11   ack     Added create profile api support for 3GPP2
05/05/11   op      Removed DS707_DATA_SESSION_PROFILE_ID_IS_EHRPD macro
04/29/11   ack     Write profile changes to efs
04/11/11   ttv     Added support for Hot Swap feature.
03/15/11   mg      Global variable cleanup
01/21/11   mg      Added APN class field to profile
01/17/11   ss      Added support for Iface linger.
10/20/10   op      Added support for PDN throttling timers in profiles
09/03/10   ms      Legacy or Non-OMH apps should always pickup default profile
                   instead of first profile.
07/28/10   ttv     Merged the fix for the issue of not reading MIP active
                   profile index from NV item NV_DS_MIP_ACTIVE_PROF_I during
                   call establishment.
05/13/10   ssh     Added support for PDN Inactivity timer
05/04/10   mg      Dual IP feature
12/10/09   ps      Mainlining OMH feature based on new profile management
                   design.
09/04/09   ss      CMI SU level modifications.
07/27/09   spn     Added a utility function which would decide based on
                   current profile and current system configuration if a call
                   is allowed to mature or not.
03/13/09   ss      Moved the macro definitions PPP_DEFAULT_SN_PASSWORD and
                   PPP_DEFAULT_SN_PASSWORD_LEN from iface4_hdlr source file
                   to here.
02/09/09   spn     Moved Data Session Profile Info into the pkt_state
12/05/08   sk      EAP support for EHRPD
08/29/08   psng    Added support to handle iface and cback events for OMH.
08/20/08   psng    Added profile id look up support for Laptop call for OMH
06/17/08   psng    Added support for multiple app profiles for OMH.
08/20/07   sc      Added a macro SEC_TO_MSEC
07/02/07   sc      Created the module. First version of the file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds707_jcdma_apprate.h"
#include "ps_ppp_defs.h"
#include "ds707_pkt_mgr.h"
#include "ds707_extif.h"
#include "ds707_cback.h"
#include "ds3gmgr.h"
#include "ps_acl.h"

/*===========================================================================
                            FORWARD DECLARATIONS
===========================================================================*/

typedef struct ds707_data_session_info_type_s ds707_data_session_info_type;

typedef struct ds707_min_apn_s   ds707_min_apn_type;

/*===========================================================================
                        MACROS, TYPEDEFS AND VARIABLES
===========================================================================*/

#define DATA_SESSION_MAX_APN_NUM        DATA_SESSION_EHRPD_NO_VALID_PROFILE

/* Below values are based on RUIM spec */
#define DEFAULT_PROFILE                 0x00000001
#define LBS_APP_TYPE                    0x00000020
#define TETHERED_MODE_APP_TYPE          0x00000040

#ifdef FEATURE_DATA_PPP_DEFAULT_PASSWD
/*---------------------------------------------------------------------------
  Definitions for default SN PPP password
  Note: The length of the password, PPP_DEFAULT_SN_PASSWORD_LEN is expected
  to not exceed:
  (1) The maximum size of the NV item to hold it, NV_MAX_PPP_PASSWORD_LENGTH
      (127 bytes); and
  (2) The size of PPP structure to hold it, PPP_MAX_PASSWD_LEN (127 bytes)
  So exercise care in changing this definition so that it fits.
---------------------------------------------------------------------------*/
#define PPP_DEFAULT_SN_PASSWORD       "0000000000000000"       /* 16 bytes */
#define PPP_DEFAULT_SN_PASSWORD_LEN   (sizeof( PPP_DEFAULT_SN_PASSWORD ) - 1)
#endif /* FEATURE_DATA_PPP_DEFAULT_PASSWD */

/* Enumerated type for ids of different data session profiles */
#define DATA_SESSION_PROFILE_ID_INVALID               -1
#define DATA_SESSION_PROFILE_ID_MIN                    0
#define DATA_SESSION_DEFAULT_PROFILE                   0
#define DATA_SESSION_NO_DEFAULT_PROFILE                1
#define DATA_SESSION_ALWAYS_SHARE_PROFILE              0xFE

#define DATA_SESSION_JCDMA_INET_PROFILE                1
#define DATA_SESSION_EMAIL_WITH_ATTACHMENT_PROFILE     2
#define DATA_SESSION_EMAIL_WITHOUT_ATTACHMENT_PROFILE  3
#define DATA_SESSION_PSMS_PROFILE                      4
#define DATA_SESSION_BREW_PCSV_PROFILE                 5
#define DATA_SESSION_BREW_FA_PROFILE                   6
#define DATA_SESSION_BREW_OTHER_PROFILE                7
#define DATA_SESSION_BREW_IPVT_PROFILE                 8
#define DATA_SESSION_BREW_PTM_PROFILE                  9
#define DATA_SESSION_OTA_PROFILE                      10
#define DATA_SESSION_UHM_PROFILE                      11
#define DATA_SESSION_KEITAI_UPDATE_PROFILE            12
#define DATA_SESSION_WAP_FA_PROFILE                   13
#define DATA_SESSION_PSMS_CHAT_PROFILE                14
#define DATA_SESSION_CORP_PROFILE                     15
#define DATA_SESSION_BML_PROFILE                      16
#define DATA_SESSION_DEVICE_MANAGEMENT_PROFILE        17
#define DATA_SESSION_WINGP_WAP_PROFILE                18
#define DATA_SESSION_WINGP_EMAIL_PROFILE              19
#define DATA_SESSION_WINGP_PCSV_PROFILE               20
#define DATA_SESSION_WINGP_PSMS_PROFILE               21
#define DATA_SESSION_INTERNATIONAL_ROAMING_PROFILE    22

#define DATA_SESSION_JCDMA_NO_VALID_PROFILE           22
#define DATA_SESSION_JCDMA_PROFILE_MIN                1
#define DATA_SESSION_JCDMA_PROFILE_MAX                22

//hrong TODO: deprecate hardcoded EHRPD/user created profiles
#define DATA_SESSION_USER_CREATED_MAX_PROFILES        16
#define DATA_SESSION_USER_CREATED_PROFILE_MIN         51
#define DATA_SESSION_USER_CREATED_PROFILE_MAX         66

#define DATA_SESSION_EHRPD_PROFILE_1                  101
#define DATA_SESSION_EHRPD_PROFILE_2                  102
#define DATA_SESSION_EHRPD_PROFILE_3                  103
#define DATA_SESSION_EHRPD_PROFILE_4                  104
#define DATA_SESSION_EHRPD_PROFILE_5                  105
#define DATA_SESSION_EHRPD_PROFILE_6                  106
#define DATA_SESSION_EHRPD_PROFILE_7                  107

#define DATA_SESSION_EHRPD_NO_VALID_PROFILE           7
#define DATA_SESSION_EHRPD_PROFILE_MIN                101
#define DATA_SESSION_EHRPD_PROFILE_MAX                150

#define DATA_SESSION_MIP_PROFILE_1                    41
#define DATA_SESSION_MIP_PROFILE_2                    42
#define DATA_SESSION_MIP_PROFILE_3                    43
#define DATA_SESSION_MIP_PROFILE_4                    44
#define DATA_SESSION_MIP_PROFILE_5                    45
#define DATA_SESSION_MIP_PROFILE_6                    46

#define DATA_SESSION_MIP_NO_VALID_PROFILE             6
#define DATA_SESSION_MIP_PROFILE_MIN                  41
#define DATA_SESSION_MIP_PROFILE_MAX                  46

#define DATA_SESSION_NVRUIM_MAX_PROFILES              16
#define DATA_SESSION_NVRUIM_PROFILE_MIN               25
#define DATA_SESSION_NVRUIM_PROFILE_MAX               40

#define DS_VSNCP_3GPP2_APN_MAX_VAL_LEN                100

#define PDN_THROTTLE_MAX_TIMERS                       6

#define ds707_data_session_profile_id     int32

/* Auth protocol */
#define DATA_SESS_AUTH_PROTOCOL_NONE                   0
#define DATA_SESS_AUTH_PROTOCOL_PAP                    1
#define DATA_SESS_AUTH_PROTOCOL_CHAP                   2
#define DATA_SESS_AUTH_PROTOCOL_PAP_CHAP               3
#define DATA_SESS_AUTH_PROTOCOL_EAP                    4

/*
   Data Rate
   For RUIM - Default Value = 2; High Speed.
*/

/* Low Speed: Low speed Service Options (SO15) only */
#define DS707_DATARATE_LOW                             0
/* Medium Speed: SO33 + low R-SCH */
#define DS707_DATARATE_MED                             1
/* High Speed: SO33 + high R-SCH */
#define DS707_DATARATE_HIGH                            2

/*
   Data Bearer
   For RUIM - Default Value = 0; Hybrid 1x/1xEV-DO.
*/
/* Hybrid 1x/1xEV-DO */
#define DS707_DATAMODE_CDMA_HDR                        0
/* 1x only */
#define DS707_DATAMODE_CDMA_ONLY                       1
/* 1xEV-DO only */
#define DS707_DATAMODE_HDR_ONLY                        2

/*---------------------------------------------------------------------------
  DNS address macros
---------------------------------------------------------------------------*/
#define PRIMARY_DNS_ADDR                               0
#define SECONDARY_DNS_ADDR                             1
#define MAX_NUM_DNS_ADDR                               2

/*---------------------------------------------------------------------------
  Operator Reserved PCO ID string length
---------------------------------------------------------------------------*/
#define DS707_DATA_SESS_OP_PCO_ID_LEN                  6

#define DS707_DATA_SESS_MAX_MCCMNC_VALUE               999
/*---------------------------------------------------------------------------
  Initial MCCMNC value, which means MCC MNC not available
---------------------------------------------------------------------------*/
#define DS707_DATA_SESS_INITIAL_MCCMNC_VALUE           0xFFFF
/*---------------------------------------------------------------------------
  Tech mask for DB_TECH_3GPP2
---------------------------------------------------------------------------*/
#define TECH_MASK_3GPP2                                2
/*===========================================================================
MACRO DATA_SESSION_IS_VALID()

DESCRIPTION
  This macro returns a boolean indicating whether the data session ptr passed
  in is valid.

PARAMETERS
  session_info_ptr: pointer to the data session info in question.

RETURN VALUE
  TRUE: if iface_ptr is valid
  FALSE: otherwise.
===========================================================================*/
#define DATA_SESSION_IS_VALID(session_info_ptr)                    \
  (((session_info_ptr) != NULL) &&                                 \
   ((session_info_ptr)->this_session == (session_info_ptr)))

/*===========================================================================
MACRO SEC_TO_MSEC()

DESCRIPTION
  This macro returns a msec value for the give value in sec

PARAMETERS
  int16 - value in sec

RETURN VALUE
  value in msec
===========================================================================*/
#define SEC_TO_MSEC(val_sec)                        \
  ((val_sec) * 1000)


/*===========================================================================
MACRO MSEC_TO_SEC()

DESCRIPTION
  This macro returns a sec value for the give value in msec

PARAMETERS
  int16 - value in msec

RETURN VALUE
  value in sec
===========================================================================*/
#define MSEC_TO_SEC(val_sec)                        \
  ((val_sec) / 1000)


/*===========================================================================
MACRO PROFILE_ID_IS_USER_CREATED()

DESCRIPTION
  This macro checks if profile id is a user created profile id

PARAMETERS
  ds707_data_session_profile_id - profile id

RETURN VALUE
  boolean: true if profile id is within user created profile
           false otherwise
===========================================================================*/
#define PROFILE_ID_IS_USER_CREATED(profile_id) \
  ((profile_id >= DATA_SESSION_USER_CREATED_PROFILE_MIN) && \
   (profile_id < (DATA_SESSION_USER_CREATED_PROFILE_MIN+DATA_SESSION_USER_CREATED_MAX_PROFILES)))

/*===========================================================================
MACRO PROFILE_ID_IS_MIP()

DESCRIPTION
  This macro checks if profile id is a MIP profile id, ie if it is within
  151 and 156 (MIP Profile id range)

PARAMETERS
  ds707_data_session_profile_id - profile id

RETURN VALUE
  boolean: true if profile id is within MIP profile number range else false
===========================================================================*/
#define PROFILE_ID_IS_MIP(profile_id) \
  ((profile_id >= DATA_SESSION_MIP_PROFILE_MIN) && \
  (profile_id < (DATA_SESSION_MIP_PROFILE_MIN+DATA_SESSION_MIP_NO_VALID_PROFILE)))

/*---------------------------------------------------------------------------
  Enum for parameter names specified in the profile file in EFS.
  Any update to this enum requires a corresponding update to
    profile_param_name_id. This enum is also used to update the custom_val_mask
    in ds707_data_session_profile_info_type above
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_PROFILE_PARAM_ID_MIN = 0,
  NEGOTIATE_DNS_SERVER  = DS707_PROFILE_PARAM_ID_MIN,
  PPP_SESSION_CLOSE_TIMER_DO,                            /* = 1 */
  PPP_SESSION_CLOSE_TIMER_1X,                            /* = 2 */
  ALLOW_LINGER,                                          /* = 3 */
  LCP_ACK_TIMEOUT_VAL,                                   /* = 4 */
  IPCP_ACK_TIMEOUT_VAL,                                  /* = 5 */
  AUTH_TIMEOUT_VAL,                                      /* = 6 */
  LCP_CREQ_RETRY_COUNT,                                  /* = 7 */
  IPCP_CREQ_RETRY_COUNT,                                 /* = 8 */
  AUTH_RETRY_COUNT,                                      /* = 9 */
  AUTH_PROTOCOL,                                         /* = 10 */
  PPP_USER_ID,                                           /* = 11 */
  PPP_PASSWORD,                                          /* = 12 */
  DATA_RATE,                                             /* = 13 */
  DATA_MODE,                                             /* = 14 */
  APP_TYPE,                                              /* = 15 */
  APP_PRIORITY,                                          /* = 16 */
  APN_STRING,                                            /* = 17 */
  PDN_IP_VERSION_TYPE,                                   /* = 18 */
  PCSCF_ADDR_NEEDED,                                     /* = 19 */
  PRIMARY_DNS_ADDR_V4,                                   /* = 20 */
  SECONDARY_DNS_ADDR_V4,                                 /* = 21 */
  PRIMARY_DNS_ADDR_V6,                                   /* = 22 */
  SECONDARY_DNS_ADDR_V6,                                 /* = 23 */
  RAN_TYPE,                                              /* = 24 */
  PDN_INACTIVITY_TIMEOUT,                                /* = 25 */
  APN_ENABLED,                                           /* = 26 */
  FAILURE_TIMER_1,                                       /* = 27 */
  FAILURE_TIMER_2,                                       /* = 28 */
  FAILURE_TIMER_3,                                       /* = 29 */
  FAILURE_TIMER_4,                                       /* = 30 */
  FAILURE_TIMER_5,                                       /* = 31 */
  FAILURE_TIMER_6,                                       /* = 32 */
  DISALLOW_TIMER_1,                                      /* = 33 */
  DISALLOW_TIMER_2,                                      /* = 34 */
  DISALLOW_TIMER_3,                                      /* = 35 */
  DISALLOW_TIMER_4,                                      /* = 36 */
  DISALLOW_TIMER_5,                                      /* = 37 */
  DISALLOW_TIMER_6,                                      /* = 38 */
  LINGER_TIMEOUT_VAL,                                    /* = 39 */
  APN_CLASS,                                             /* = 40 */
  PDN_LEVEL_AUTH_PROTOCOL,                               /* = 41 */
  PDN_LEVEL_USER_ID,                                     /* = 42 */
  PDN_LEVEL_AUTH_PASSWORD,                               /* = 43 */
  PDN_LABEL,                                             /* = 44 */
  OP_PCO_ID,                                             /* = 45 */
  MCC,                                                   /* = 46 */
  MNC,                                                   /* = 47 */
  USER_APP_DATA,                                         /* = 48 */
  IPV6_DELEGATION,                                       /* = 49 */
  SUBS_ID,                                               /* = 50 */
  DNS_DHCP_REQ_FLAG,                                     /* = 51 */
  PCSCF_DHCP_REQ_FLAG,                                   /* = 52 */
  FAILURE_TIMER,                                         /* = 53 */
  DISALLOW_TIMER,                                        /* = 54 */
  DS707_PROFILE_PARAM_ID_MAX = DISALLOW_TIMER
}profile_param_name_type;


/* PDN Type */
typedef enum
{
    DS707_DATA_SESS_PDN_TYPE_V4 = 0,
    DS707_DATA_SESS_PDN_TYPE_V6 = 1,
    DS707_DATA_SESS_PDN_TYPE_V4_V6 = 2
} ds707_data_sess_pdn_type_enum_type;

/* RAT (Radio Access Technology) Type */
typedef enum
{
    DS707_DATA_SESS_RAT_TYPE_HRPD       = 1,
    DS707_DATA_SESS_RAT_TYPE_EHRPD      = 2,
    DS707_DATA_SESS_RAT_TYPE_HRPD_EHRPD = 3,
} ds707_data_sess_rat_type_enum_type;

/*---------------------------------------------------------------------------
  bitmask for allowed types of profiles
---------------------------------------------------------------------------*/
typedef enum
{
  PROFILE_INVALID     = 0,
  PROFILE_COMMON      = 1 << 0, // ONLY COMMON PARAMS ARE VALID
  PROFILE_KDDI        = 1 << 1, // ONLY KDDI PARAMS ARE VALID
  PROFILE_EHRPD       = 1 << 2, // ONLY EHRPD PARAMS ARE VALID
  PROFILE_OMH         = 1 << 3, // ONLY OMH PARAMS ARE VALID
  PROFILE_MAX         = 1 << 30
} ds_profile_type_enum_type;

typedef enum {
  V4   = 4,       /**< IPv4 version. */
  V6   = 6,       /**< IPv6 version. */
  V4V6 = 10,     /**< IPv4v6 version. */
  IP_MAX  = 0xff
} ds_profile_ip_version_enum_type;

typedef struct  
{
  uint16  mnc;
  boolean mnc_includes_pcs_digit; //TRUE: 3 digits
}ds707_data_sess_mnc_type;

typedef enum
{
  ACTIVE_SUBSCRIPTION_NONE  = 0x0,
  ACTIVE_SUBSCRIPTION_1     = 0x1,
  ACTIVE_SUBSCRIPTION_2     = 0x2,
  ACTIVE_SUBSCRIPTION_3     = 0x3,
  ACTIVE_SUBSCRIPTION_MAX   = ACTIVE_SUBSCRIPTION_3 + 1
} ds_profile_subs_id_enum_type;

/* Internal data structure for storing data session profile parameters */
typedef struct
{
    /* profile id */
    ds707_data_session_profile_id data_session_profile_id;

    /*Data profile specific (common across all the builds) */
    boolean negotiate_dns_server; /* ON or OFF default ON */
    uint32 ppp_session_close_timer_DO;
    uint32 ppp_session_close_timer_1X;
    boolean allow_linger; /* allowed or disallowed */

    /* Linger timeout value */
    uint16 linger_timeout_val;

    /*Expose only to KDDI though it is common to all carriers */
    uint16 lcp_ack_timeout; /* lcp C-Req Retry Timer */
    uint16 ipcp_ack_timeout; /* ipcp C-Req Retry Timer */
    uint16 auth_timeout; /* PAP or CHAP Retry Timer */

    uint8 lcp_creq_retry_count; /* number of retries */
    uint8 ipcp_creq_retry_count; /* number of retries */
    uint8 auth_retry_count; /* number of retries */

    /*Which auth protocol is negotiated PAP or CHAP*/
    uint8 auth_protocol;

    char user_id[PPP_MAX_USER_ID_LEN];
    uint8 user_id_len;
    char auth_password[PPP_MAX_PASSWD_LEN];
    uint8 auth_password_len;

    uint8 data_rate; /* high, medium or low */
    uint8 data_mode; /* hybrid or 1x */

    /* OMH related params */
    uint32 app_type;
    uint8  app_priority;

    /* eHRPD related params */
    char apn_string[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN];
    uint8 apn_string_len;
	char pdn_label[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN];
	uint8 pdn_label_len;
    boolean apn_flag; /* TRUE if APN is enabled in profile */
    uint8 apn_class;
    ds707_data_sess_pdn_type_enum_type pdn_type;
    boolean is_pcscf_addr_needed;
    struct ps_in_addr v4_dns_addr[MAX_NUM_DNS_ADDR];
    struct ps_in6_addr v6_dns_addr[MAX_NUM_DNS_ADDR];
    ds707_data_sess_rat_type_enum_type rat_type;
    uint32 pdn_inactivity_timeout;
    uint32 failure_timer[PDN_THROTTLE_MAX_TIMERS];
    uint32 disallow_timer[PDN_THROTTLE_MAX_TIMERS];

    ds_profile_type_enum_type profile_type; //validity mask

    /* This mask is updated whenever a custom value is set for a
       parameter */
    uint64 custom_val_mask;
    boolean is_persistent;

    /* PDN level authentication parameters */
    uint8 pdn_level_auth_protocol;
    char  pdn_level_user_id[PPP_MAX_USER_ID_LEN];
    uint8 pdn_level_user_id_len;
    char  pdn_level_auth_password[PPP_MAX_PASSWD_LEN];
    uint8 pdn_level_auth_password_len;
    /*operator reserved PCO ID, 0xFF00 - 0xFFFF*/
    uint16  op_pco_id;
    /*valid range:0 - 999*/
    uint16  mcc; 
    /*valid range:0 - 999*/
    ds707_data_sess_mnc_type  mnc;

    /* 4 byte opaque data */
    uint32 user_app_data;

    /* IPV6_PREFIX_DELEGATION_ENABLED*/
    boolean ipv6_delegation;

    /* CLAT enabled*/
    boolean clat_enabled;

    /* subscription ID */
    ds_profile_subs_id_enum_type subs_id;

    /* Profile db internal parameters */
    uint16 supported_tech_mask; /* tech mask for profile db */
    boolean is_linger_params_valid;
    ds_profile_ip_version_enum_type ip_version[MAX_NUM_DNS_ADDR];

    boolean dns_addr_using_dhcp;
    boolean pcscf_addr_using_dhcp;

} ds707_data_session_profile_info_type;

/*--------------------------------------------------------------------------
  Internal structure which contains information about current data session.
  This information is stored in the client data pointer in the ps_iface
  structre.
--------------------------------------------------------------------------*/

struct ds707_data_session_info_type_s
{
    /* if NULL then block is invalid */
    ds707_data_session_info_type *this_session;
    struct
    {
        ds707_data_session_profile_info_type current_data_session_profile;
        ds707_data_session_profile_info_type requesting_data_session_profile;
    } iface_profile_info;
    /* to track if the iface is lingering */
    boolean is_iface_lingering;
    uint8 pdn_id;
    char apn_string[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN];
    uint8 apn_string_len;
};

/* Mandatory APN list information */
struct ds707_min_apn_s
{
  char *apn_name;
  uint8 apn_len;
  boolean is_modified;
};

/*--------------------------------------------------------------------------
  Internal structure which contains information about the list of APNs read
  from the UICC card
--------------------------------------------------------------------------*/

typedef struct
{
  int num_apn;
  boolean ds3g_mmgsdi_init;
  boolean data_session_profile_init;
  struct
  {
    char *apn_name;
    uint8 apn_len;
  }profile_apn_list[DATA_SESSION_MAX_APN_NUM];
}ds707_data_session_acl_info_type;

/*--------------------------------------------------------------------------
  Enum type to describe how to configure the Linger Timer for a given IFACE.
--------------------------------------------------------------------------*/
typedef enum
{
  /* assigns positive value to linger timer without checking the bearer tech */
  STATIC_LINGER_CONFIG = 1,
  /* assigns DO or 1x timer value depending upon the current bearer tech */
  DYNAMIC_LINGER_CONFIG = 2,
} ds_linger_tmr_config_enum_type;

typedef struct
{
  uint16 lcp_ack_timeout; /* lcp C-Req Retry Timer */
  uint16 ipcp_ack_timeout; /* ipcp C-Req Retry Timer */
  uint16 ip6cp_ack_timeout; /* ip6cp C-Req Retry Timer*/
  uint16 auth_timeout; /* PAP or CHAP Retry Timer */

  uint8 lcp_creq_retry_count; /* number of retries */
  uint8 ipcp_creq_retry_count; /* number of retries */
  uint8 ip6cp_creq_retry_count; /* number of retries */
  uint8 auth_retry_count;
} data_sess_profile_ppp_params_type;

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_PROFILE_CACHE_INIT

DESCRIPTION   This function initializes the data session profile parameters
              for different profiles. It also sets customized profile
              parameters depending on the EFS files. And finally it registers
              iface events.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_profile_cache_init(void);
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_PROFILE_INIT

DESCRIPTION   This function initializes the data session profile parameters
              for different profiles.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_profile_init(void);


/*===========================================================================
FUNCTION      DS707_DATA_SESSION_CREATE_PROFILE

DESCRIPTION   This function is used to create a 3GPP2 profile on modem

PARAMETERS    is_persistent : Indicates whether the created profile should
              be persistent or not

RETURN VALUE  returns the created profile id

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_create_profile(
  boolean is_persistent
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_DELETE_PROFILE

DESCRIPTION   This function is used to delete a user created 3GPP2 profile on
              modem.
              Delete profile will succeed only for:
              1. Profiles created using the DS Create Profile API and
              2. Profiles not currently in use by any iface

PARAMETERS    profile_id : 3GPP2 profile id

RETURN VALUE  TRUE  : on successful profile delete
              FALSE : on failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_delete_profile(
    ds707_data_session_profile_id profile_id
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_RESET_PROFILE_TO_DEFAULT

DESCRIPTION   This function resets all the parameters to default for the 
              profile id. 

PARAMETERS    profile id

DEPENDENCIES  None

RETURN VALUE  boolean: true if successful, false if failed

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_reset_profile_to_default
(
  ds707_data_session_profile_id profile_id
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_PROFILE

DESCRIPTION   This function reads the EFS files containing data session
              profile parameters

PARAMETERS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_profile(void);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE

DESCRIPTION   This function reads data session profile parameters from
              ds707_data_session_profile_info_type[ ] for a particular
              data_session_profile_id.

PARAMETERS    ds707_data_session_profile_id:
                     Use this type to select which set of
                     profile parameters to return.
              ds707_data_session_profile_info_type** :
                     ptr to a ptr to strcutre containing data session
                     profile parameters

DEPENDENCIES  None

RETURN VALUE  TRUE:  profile found
              FALSE: profile not valid/not found- fallback to default profile

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_profile
(
  ds707_data_session_profile_id,
  ds_profile_subs_id_enum_type  subs_id,
  ds707_data_session_profile_info_type **
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_DEFAULT_PROFILE

DESCRIPTION   This function is called if the profile parameters are not set
              for the default data session profile. In that case, the default
              paramters are read from ps ppp structure and stored in default
              data session profile structure.

PARAMETERS

DEPENDENCIES  None

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_default_profile(void);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_ds707_INFO_INIT

DESCRIPTION   This function initializes the data_session_info type structure.

PARAMETERS    ds707_data_session_info_type *

DEPENDENCIES  None

RETURN VALUE  int: 0 - on success
                    -1 - on failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_info_init
(
    ds707_data_session_info_type *
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_CURRENT_PROFILE

DESCRIPTION   This function reads the current data session profile id from
              data_session_info in the pdn cb associated with this iface.

PARAMETERS    ps_iface_type *: pointer to iface.

DEPENDENCIES  None

RETURN VALUE  TRUE  if iface current profile is valid
              FALSE if PDN CB or current profile is invalid 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_iface_current_profile
(
    ps_iface_type *,
    ds707_data_session_profile_info_type **
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_CURRENT_PROFILE

DESCRIPTION   This function sets the current data session profile id pointed
              by the client_data_ptr of the given iface.

PARAMETERS    ps_iface_type *:  access the client_data_ptr for this iface
              ds707_data_session_profile_id :
                                current data session profile id for the iface

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_iface_current_profile
(
    ps_iface_type *,
    ds707_data_session_profile_id,
    ds_profile_subs_id_enum_type
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_REQUESTING_PROFILE

DESCRIPTION   This function reads the requesting data session profile from
              data_session_info in the pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface

DEPENDENCIES  None

RETURN VALUE  TRUE  if iface requesting profile is valid
              FALSE if PDN CB or requesting profile is invalid 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_iface_requesting_profile
(
  ps_iface_type                        *iface_ptr,
  ds707_data_session_profile_info_type **data_sess_profile_ptr
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_REQUESTING_PROFILE

DESCRIPTION   This function sets the requesting data session profile id
              pointed by the client_data_ptr of the given iface.

PARAMETERS    ps_iface_type *:  access the client_data_ptr for this iface
              ds707_data_session_profile_id :
                    requesting data session profile id for the iface.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_iface_requesting_profile
(
    ps_iface_type *,
    ds707_data_session_profile_id,
    ds_profile_subs_id_enum_type
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_LINGER_STATUS

DESCRIPTION   This function sets the is_iface_lingering flag
              pointed by the client_data_ptr of the given iface.

PARAMETERS    ps_iface_type *:  access the client_data_ptr for this iface
              boolean        :  TRUE - IFACE lingering else FALSE

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_iface_linger_status
(
    ps_iface_type *,
    boolean
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_LINGER_STATUS

DESCRIPTION   This function gets the value of is_iface_lingering flag
              pointed by the client_data_ptr of the given iface.

PARAMETERS    ps_iface_type *:  access the client_data_ptr for this iface
              boolean *       :  TRUE - IFACE lingering else FALSE

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_get_iface_linger_status
(
    ps_iface_type *,
    boolean       *
);
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_PDN_ID

DESCRIPTION   This function gets the PDN ID pointed by the client_data_ptr
              of the given iface.

PARAMETERS    ps_iface_type *:  access the client_data_ptr for this iface
              uint8 *       :  PDN ID associated with the IFACE

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_get_iface_pdn_id
(
  ps_iface_type *iface_ptr,
  uint8         *pdn_id
);
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_PDN_ID

DESCRIPTION   This function sets the PDN ID pointed by the client_data_ptr
              of the given iface.

PARAMETERS    ps_iface_type *:  access the client_data_ptr for this iface
              uint8         :  PDN ID to be associated with the IFACE

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_set_iface_pdn_id
(
  ps_iface_type *iface_ptr,
  boolean       pdn_id
);
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_APN

DESCRIPTION   This function gets the APN string and string length pointed by
              the client_data_ptr of the given iface. IF APN length is 0 or
              invalid then apn_string is returned as NULL.

PARAMETERS    ps_iface_type *:  access the client_data_ptr for this iface
              uint8*         :  APN Length
              char*          :  APN String

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_get_iface_apn
(
  ps_iface_type *iface_ptr,
  uint8*        apn_length,
  char*         apn_string

);
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_APN

DESCRIPTION   This function sets the APN string and string length pointed by
              the client_data_ptr of the given iface. IF APN length is 0 or
              invalid then apn_string is set to NULL and length is set to 0.

PARAMETERS    ps_iface_type *:  access the client_data_ptr for this iface
              uint8          :  APN Length
              char*          :  APN String

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_set_iface_apn
(
  ps_iface_type *iface_ptr,
  uint8         apn_length,
  char*         apn_string

);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_IS_V6_DELEGATION_ENABLED

DESCRIPTION   This function sets the APN in the data_session_info
              in pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              uint8          :  APN Length
              char*          :  APN String

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_iface_is_v6_delegation_enabled
(
  ps_iface_type *iface_ptr
);/* ds707_data_session_get_iface_is_v6_delegation_enabled */

/*=============================================================================
FUNCTION      DS707_DATA_SESSION_RESOLVE_IFACE_ADDR_FAMILY

DESCRIPTION   This function resolves the iface addr family in case the IFACE
              is not in use. If the addr family passed in the policy is
              UNSPEC then for eHRPD profiles and eHRPD mode of operation, the
              iface address family is the PDN IP family mentioned in the
              profile. For HRPD profiles or HRPD mode of operation, it defaults
              iface address family to V4.

              for eHRPD mode, if the policy is UNSPEC and the pdn_type is
              v4v6 then use failover ip read from NV

PARAMETERS    ds707_data_session_profile_id profile_id
              acl_policy_info_type          *policy_info_ptr

DEPENDENCIES  None

RETURN VALUE  ip_addr_enum_type addr_family - resolved iface address family


SIDE EFFECTS  None
=============================================================================*/
ip_addr_enum_type ds707_data_session_resolve_iface_addr_family
(
  ds707_data_session_profile_id profile_id,
  acl_policy_info_type          *policy_info_ptr
);
/*=============================================================================
FUNCTION      DS707_DATA_SESSION_IP_VERSION_MATCH

DESCRIPTION   This function checks whether IP version mentioned in the profile
              matches the IP version mentioned in the App policy. Currently
              this function is used only for eHRPD profiles as only eHRPD
              profiles mention IP version i.e. PDN Type in the profiles.

PARAMETERS    ds707_data_session_profile_id profile_id
              acl_policy_info_type          *policy_info_ptr
              ps_iface_type                 *if_ptr

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if two IP version are same else FALSE

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_data_session_ip_version_match
(
  ds707_data_session_profile_id profile_id,
  acl_policy_info_type          *policy_info_ptr,
  ps_iface_type                 *if_ptr
);

/*=============================================================================
FUNCTION      DS707_DATA_SESSION_SUBS_ID_MATCH

DESCRIPTION   This function checks whether subs id mentioned in the profile
              matches the subs id mentioned in the App policy.

PARAMETERS    ds707_data_session_profile_id profile_id
              acl_policy_info_type          *policy_info_ptr

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if two subs ID are same else FALSE

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_data_session_subs_id_match
(
  ds707_data_session_profile_id profile_id,
  acl_policy_info_type          *policy_info_ptr
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_COMPATIBLE_PROFILES

DESCRIPTION   This function checks whether the two profiles are compatible or
              not depending upon the profile ids that are passed. It also
              checks if the iface is available to use or not. This iface
              availability check makes the laptop and socket calls
              non-compatible to each other.

PARAMETERS    ps_iface_type* - iface pointer
              acl_policy_info_type* - pointer to the acl policy
              ds707_data_session_profile_id requesting_id -
                profile id of the new requesting application
              ds707_data_session_profile_id current_id -
                profile id of the current application

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if the two profiles are compatible else FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_compatible_profiles
(
  ps_iface_type                 *if_ptr,
  acl_policy_info_type          *policy_info_ptr,
  ds707_data_session_profile_id requesting_id,
  ds707_data_session_profile_info_type *current_prof_ptr
);

#ifdef FEATURE_EHRPD
/*=============================================================================
FUNCTION      DS707_DATA_SESSION_EHRPD_FRIEND_PROFILES

DESCRIPTION   This function checks whether the two Ehrpd profiles are friends of
              each other or not. The criterion for friends profile is that they
              should have the same APN string

PARAMETERS    ds707_data_session_profile_info_type *profileA_ptr
              ds707_data_session_profile_info_type *profileB_ptr
              acl_policy_info_type          *policy_info_ptr

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if the two profiles are friends else FALSE

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_data_session_ehrpd_friend_profiles
(
  ds707_data_session_profile_info_type *profileA_ptr,
  ds707_data_session_profile_info_type *profileB_ptr,
  acl_policy_info_type          *policy_info_ptr
);

#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
/*===========================================================================
FUNCTION      DS707_DATA_SESS_SET_CM_CENTRIC_EMERG_MODE

DESCRIPTION   To set the status of CM emergency mode 

DEPENDENCIES  None

RETURN VALUE  TRUE: Is in emergency only mode 
              FALSE: in normal mode 

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_set_cm_centric_emerg_mode(boolean is_emerg_mode);
/*===========================================================================
FUNCTION      DS707_DATA_SESS_GET_CM_CENTRIC_EMERG_MODE

DESCRIPTION   To get the status of CM emergency mode 

DEPENDENCIES  None

RETURN VALUE  TRUE: Is in emergency only mode 
              FALSE: in normal mode 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_sess_get_cm_centric_emerg_mode(void);


/*===========================================================================
FUNCTION      DS707_DATA_SESS_HANDLE_CM_AC_EMERG_EVENT

DESCRIPTION   handle the event from CM for emergency mode enter/exit

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_handle_cm_ac_emerg_event
(
  ds_cmd_type  *ds_cmd_ptr
);
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
#endif

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_FRIEND_PROFILES

DESCRIPTION   This function checks whether the two profiles are friends of
              each other or not depending upon the profile ids that
              are passed.

PARAMETERS    ds707_data_session_profile_id id_A -first profile id
              ds707_data_session_profile_id id_B -second profile id

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if the two profiles are friends else FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_friend_profiles
(
    ds707_data_session_profile_id,
    ds707_data_session_profile_id
);

/*===========================================================================
FUNCTION      DS707_ASSIGN_PPP_AUTH_INFO

DESCRIPTION   This function assigns ppp_auth_info of the given data session
              profile structure to the provided PPP configuration structure.

PARAMETERS    ppp_dev_opts_type * -
                                  PPP configuration structure
              ds707_data_session_profile_info_type -
                                  data session profile struct

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_assign_ppp_auth_info
(
    ppp_auth_info_type *,
    ds707_data_session_profile_info_type *
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_LINGER_TMR_VAL

DESCRIPTION   This function sets the Linger timer value according to the data
              session profile id and the underlying bearer technology (DO/1X)
              if the configuration option is set to DYNAMIC_LINGER_CONFIG. In
              case of STATIC_LINGER_CONFIG option, the Linger timer value is
              set to some positive value.

PARAMETERS    ps_iface_type * - Iface ptr for which Liger timer is to be set
              ds_linger_tmr_config_enum_type - Config option

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_linger_tmr_val
(
  ps_iface_type                   *if_ptr,
  ds_linger_tmr_config_enum_type  config_option
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_VALID_PROFILE

DESCRIPTION   This function checks if the profile given as input is a valid
              profile or not. If the profile is valid then it returns the
              profile reg index

PARAMETERS    ds707_data_session_profile_id

DEPENDENCIES  None

RETURN VALUE  profile reg index if valid profile
              -1 if invalid profile.

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_is_valid_profile
(
  ds707_data_session_profile_id profile_id
);

/*==========================================================================
FUNCTION ds707_read_data_session_profile_from_nvruim

DESCRIPTION
  This function gets sip profile info from NV item/RUIM and stores them in
  DS cache.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'dsi_nv_get_item' results in a wait and watchdog kicking
===========================================================================*/
void ds707_read_data_session_profile_from_nvruim( void );

/*===========================================================================
FUNCTION DS707_DATA_SESSION_GET_APP_PROFILE_INDEX()

DESCRIPTION
  This function will return the application profile index based on the application type
  passed as the parameter to it.

DEPENDENCIES
  None.

PARAMETERS
  app type: type of the application for which u want profile index

RETURN VALUE
  profile index  on success.
  -1             on failure.

SIDE EFFECTS
  None.
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_app_profile_index(uint32 app_type);

/*===========================================================================
FUNCTION DS707_DATA_SESSION_PRIORITY_CHECK

DESCRIPTION
  This function will compare priority of profile A and B passed as arguments.

DEPENDENCIES
  NONE

PARAMETERS
  uint8 profileA_app_priority,
  uint8 profileB_app_priority

RETURN VALUE
  TRUE  : If the priority of profileA is equal to the priority of ProfileB
  FALSE : If the priorities differ.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_data_session_profile_priority_check
(
  uint8 profileA_app_priority,
  uint8 profileB_app_priority
);

/*===========================================================================
FUNCTION DS707_DATA_SESSION_COMPATIBILITY_CHECK

DESCRIPTION
  This function will check the compatibility of LBS app with current profile.

DEPENDENCIES
   NONE

PARAMETERS
  this_if_ptr     : Current active iface pointer
  policy_info_ptr : Pointer to current application policy info block

RETURN VALUE
  TRUE  : If LBS app is comaptible with current active profile.
  FALSE : If LBS app is incomaptible with current active profile.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_data_session_compatibility_check
(
  void                 *this_if_ptr,
  acl_policy_info_type *policy_info_ptr
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_REG_CBACK_EVENTS

DESCRIPTION   Called at powerup. Registers for cback events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_reg_cback_events(void);
#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_EVENT_CBACK

DESCRIPTION   Callback registered with DS707 for CBACK events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_event_cback
(
  ds707_cback_event_type            event,
  const ds707_cback_event_info_type *event_info_ptr
);
#endif
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_PROFILE_ID_IS_VALID

DESCRIPTION   This function checks if the passed id is a valid id

PARAMETERS    ds707_data_session_profile_id_enum_type - profile id that needs
                                                        to be validated

DEPENDENCIES  None

RETURN VALUE  TRUE - if the id is valid
              FALSE - if the id is invalid

SIDE EFFECTS  None
===========================================================================*/

boolean ds707_data_session_profile_id_is_valid
(
  ds707_data_session_profile_id profile_id
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_CALL_ALLOWED

DESCRIPTION   Utility function to make a decision on whether calls are
              allowed or not, based on profiles.

DEPENDENCIES  None.

RETURN VALUE  TRUE: eHRPD Calls are allowed
              FALSE: eHRPD Calls are not allowed

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_data_session_call_allowed
(
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr,
  ps_iface_net_down_reason_type *call_end_reason
);

/*===========================================================================
FUNCTION ds707_data_sess_profile_id_to_reg_index_mapping

DESCRIPTION
  This function takes application profile id as input and returns the actual
  index in the profile registry / database

DEPENDENCIES

RETURN VALUE
      -1 - if the given profile id is invalid
      index in the reg - if profile id is valid

SIDE EFFECTS

===========================================================================*/
ds707_data_session_profile_id ds707_data_sess_profile_id_to_reg_index_mapping
(
  ds707_data_session_profile_id profile_id
);

/*=============================================================================
FUNCTION      DS707_GET_MIP_INDEX_FROM_PROFILE_ID

DESCRIPTION   This function calculates the mip profile index from the profile
              id. This index is used to read mip parameters from NV.
              MIP profile ids range is from 151 - 200. Number of
              valid MIP profiles are 6. Profile id 151 maps to 1st MIP
              profile in NV (index 0) and so on
              If profile id passed to this function is not a MIP profile,
              the index is the active MIP index from NV

PARAMETERS    ds707_data_session_profile_id profile_id

DEPENDENCIES  None

RETURN VALUE  int8 - mip index

SIDE EFFECTS  None
=============================================================================*/
int8 ds707_get_mip_index_from_profile_id(
  ds707_data_session_profile_id profile_id
);

/*=============================================================================
FUNCTION      DS707_GET_MIP_PROFILE_INDEX

DESCRIPTION   This function returns the stored mip profile index.
              (ds707_mip_profile_index)

PARAMETERS

DEPENDENCIES  None

RETURN VALUE  int8 - mip index

SIDE EFFECTS  None
=============================================================================*/
int8 ds707_get_mip_profile_index(void);

/*=============================================================================
FUNCTION      DS707_SET_MIP_PROFILE_INDEX

DESCRIPTION   This function sets the mip profile index.
              (ds707_mip_profile_index)

PARAMETERS    int8 - Profile Index.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
=============================================================================*/
void ds707_set_mip_profile_index( int8 profile_index );

/*===========================================================================
FUNCTION     DS707_DATA_SESSION_GET_PPP_AUTH_INFO_FOR_NVRUIM_PROFILE

DESCRIPTION
  This function retrieves the PPP user ID and password from NV/RUIM
  and stores them in the provided PPP configuration structure.

DEPENDENCIES
  The NV task must be running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'ds3gcfgmgr_read_legacy_nv_ex' results in a wait and watchdog kicking.
===========================================================================*/
void ds707_data_session_get_ppp_auth_info_for_nvruim_profile
(
  ppp_auth_info_type *ppp_auth_info_ptr
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_NVRUIM_PROFILES_AVAILABLE

DESCRIPTION
  This function checks and returns TRUE if profiles (specified by RUIM 3GPD)
  exists in NV/RUIM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid profiles present in NV/RUIM
  FALSE - If no valid profiles present in NV/RUIM

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_data_session_is_nvruim_profiles_available( void );

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_NVRUIM_PROFILE

DESCRIPTION
  This function checks whether passed Profile ID is of NV/RUIM profile type
  (specified by RUIM 3GPD) or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If Profile ID is of NV/RUIM type.
  FALSE - If Profile ID is not of NV/RUIM type.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_data_session_is_nvruim_profile
(
  ds707_data_session_profile_id profile_id
);

/*===========================================================================
FUNCTION
  DS707_DATA_SESSION_GET_DEFAULT_PROFILE

DESCRIPTION
  This function returns the default profile based on if NVRUIM profile is 
  available or not.

DEPENDENCIES
  None

RETURN VALUE
  Default Profile ID in the range 201-216 if NVRUIM profiles are available. 
  Else this function returns the hardcoded default profile.

SIDE EFFECTS
  None
===========================================================================*/

ds707_data_session_profile_id ds707_data_session_get_default_profile( void );

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_ACL_INFO_PTR

DESCRIPTION   Returns pointer to ACL info. (Used in APN enable/disable
              feature in data session module, and ds3gmmgsdi module).

DEPENDENCIES  None

RETURN VALUE  ds707_data_session_acl_info_type*

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_acl_info_type* ds707_data_session_get_acl_info_ptr(
  void
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_CHECK_MANDATORY_APNS

DESCRIPTION   This function checks whether the list of mandatory APNs are all
              enabled in the profile and in the card, if its present. And
              accordingly sets the ehrpd_allowed flag to TRUE or FALSE.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_check_mandatory_apns( void );

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_PROCESS_EHRPD_ATTACH_ALLOWED

DESCRIPTION   This function checks if the ehrpd attach is allowed & sets the 
              ehrpd attach allowed flag. This is set to true/false depending on 
              whether all the mandatory APNs are enabled in card and in profile.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_process_ehrpd_attach_allowed(
  boolean flag
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_EHRPD_ATTACH_ALLOWED

DESCRIPTION   This function returns the ehrpd attach allowed flag.

DEPENDENCIES  None

RETURN VALUE  boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_ehrpd_attach_allowed(
  void
);

/*===========================================================================
FUNCTION
  DS707_DATA_SESSION_GET_DEFAULT_PROFILE_ID

DESCRIPTION
  This function returns the profile ID of the default profile.

DEPENDENCIES
  None

RETURN VALUE
  Profile IDs in the range 201-216. If no default profile is found
  then this function returns -1.

SIDE EFFECTS
  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_default_profile_id
(
  void
);

/*===========================================================================
FUNCTION      DS707_GET_PDN_THROTTLE_FAILURE_TIMER

DESCRIPTION   This function provides the PDN throttling failure timer value

PARAMETERS    profile_id - Data session profile id
              counter - Throttling counter used as an index

RETURN VALUE  PDN throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds707_get_pdn_throttle_failure_timer
(
  ds707_data_session_profile_info_type *profile_info_ptr,
  uint8                                 counter
);

/*===========================================================================
FUNCTION      DS707_GET_PDN_THROTTLE_DISALLOW_TIMER

DESCRIPTION   This function provides the PDN throttling disallow timer value

PARAMETERS    profile_id - Data session profile id
              counter - Throttling counter used as an index

RETURN VALUE  PDN throttle Disallow Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds707_get_pdn_throttle_disallow_timer
(
  ds707_data_session_profile_info_type *profile_info_ptr,
  uint8                                 counter
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_ID_FROM_REG_INDEX

DESCRIPTION   Given the index, this function returns the profile id.

PARAMETERS    reg_index

RETURN VALUE  ds707_data_session_profile_id

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_profile_id_from_reg_index(
  ds707_data_session_profile_id reg_index
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_OLDEST_ACTIVE_IFACE_PTR

DESCRIPTION   This function returns the oldest active iface pointer

PARAMETERS    None

RETURN VALUE  ps_iface_type*: iface ptr

SIDE EFFECTS  None
===========================================================================*/
ps_iface_type* ds707_data_session_get_oldest_active_iface_ptr(void);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_OLDEST_ACTIVE_IFACE_PTR

DESCRIPTION   This function sets the value of oldest active iface pointer
              to the passed value.

PARAMETERS    ps_iface_type*: iface ptr

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_oldest_active_iface_ptr(
  ps_iface_type* iface_ptr
);

/*===========================================================================
FUNCTION      DS707_WRITE_PROFILE_TO_EFS

DESCRIPTION   Writes the given profile's contents to EFS

PARAMETERS    profile_id    : profile number
              dsi_3gpp2_ptr : pointer to profile blob

DEPENDENCIES  None

RETURN VALUE  TRUE on Success
              FALSE on Failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_write_profile_to_efs(
  ds707_data_session_profile_id profile_id,
  ds707_data_session_profile_info_type *data_sess_profile_ptr
);

/*===========================================================================
FUNCTION DS707_RESET_DATA_SESSION_PROFILE_FROM_NVRUIM

DESCRIPTION
  This function reset the profile related information.

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_reset_data_session_profile_from_nvruim( void );

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_DISALLOW_CALL_NV_INIT

DESCRIPTION   Read nv value for whether to disallow any call when a mandaory 
              APN is disabled. 

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_disallow_call_nv_init();


#ifdef FEATURE_DATA_FALLBACK_LIST
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_IN_FALLBACK_LIST

DESCRIPTION   This function checks both fallback_pdn_list.txt and 
              fallback_apn_list.txt to find whether the input apn is in the
              2 list

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: is in either list
                       FALSE: is NOT in both lists

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_is_in_fallback_list(
                                          const char  *apn_name,
                                          const uint8 apn_len
                                          );
#endif /*FEATURE_DATA_FALLBACK_LIST*/

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_NV_MINAPNLIST_DISALLOW_CALL

DESCRIPTION   Returns the nv configuration for disallowing call when any 
              mandatory APN is disabled 

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  TRUE: to disallow any call when any 
              mandatory APN is disabled 
              FALSE: only disallow eHRPD call when any 
              mandatory APN is disabled 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_nv_minapnlist_disallow_call(void);
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_READ_DISALLOW_CALL_NV

DESCRIPTION   Read the NV for disallow call

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ds707_data_session_read_disallow_call_nv(void);

/*===========================================================================
FUNCTION      DS707_DATA_SESS_IS_EMERG_ONLY_MODE

DESCRIPTION   To check if it is now in emergency call only mode

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  TRUE: is in emergency call only mode 
              FALSE: otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_sess_is_ds_centric_emerg_only_mode(void);
/*===========================================================================
FUNCTION      DS707_DATA_SESS_HANDLE_CMD_IN_EMERG_ONLY

DESCRIPTION   message handler when in emergency mode

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_handle_cmd_in_emerg_only(ds_cmd_type *ds_cmd_ptr);

/*===========================================================================
FUNCTION       DS707_DATA_SESSION_SET_SIP_ACTIVE_PROFILE_INDEX

DESCRIPTION    This function sets the sip_active_profile_index.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_data_session_set_sip_active_profile_index
(
  uint8 sip_active_profile_index
);

/*===========================================================================
FUNCTION       DS707_DATA_SESSION_GET_SIP_ACTIVE_PROFILE_INDEX

DESCRIPTION    This function returns sip_active_profile_index.

DEPENDENCIES   None.

RETURN VALUE   uint8 - sip active profile index.

SIDE EFFECTS   None

===========================================================================*/
uint8 ds707_data_session_get_sip_active_profile_index
(
  void
);


/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_APN_NAME

DESCRIPTION   Checks with MPPM if call is allowed for a profile on 3GPP2.

PARAMETERS    profile_ptr - The profile to check.
              apn_name    - The apn name holder passed by caller.
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - Call is allowed. 
              FALSE  - Call is not allowed.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_profile_apn_name
( 
  ds707_data_session_profile_info_type * data_sess_prof_ptr,
  ds3g_apn_name_type                   * apn_name
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_ALLOC_CACHE_ENTRY
 
DESCRIPTION   Provide profile cache entry for newly created profile.
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - if cache for new profile is allocated successfully
              FALSE  - otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_alloc_cache_entry
(
  ds707_data_session_profile_info_type** profile_info_ptr,
  ds_profile_subs_id_enum_type           subs_id
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_DELETE_CACHE_ENTRY

DESCRIPTION   Delete the cache entry for the profile
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - if cache entry is deleted successfully
              FALSE  - otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_delete_cache_entry
(
  ds707_data_session_profile_id profile_id,
  ds_profile_subs_id_enum_type  subs_id
);

/*===========================================================================
FUNCTION ds707_data_sess_profile_get_stored_ppp_opts

DESCRIPTION
  This function returns stored default ppp opts read from NV during
  boot up.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds707_data_sess_profile_get_stored_ppp_opts
(
  data_sess_profile_ppp_params_type **ppp_params_ptr
);

/*===========================================================================
FUNCTION DS707_IS_APN_PREF_PKT_MODE_EX

DESCRIPTION
  This function runs the pref mode check for the PDN for 3GPP2.
  
PARAMETERS  
  profile_id         - 3GPP2 profile ID for this PDN
  mode               - System mode
  subs_id            - Resolved subs ID
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : If mode is the preferred pkt mode for this PDN
  FALSE: If mode is not the preferred pkt mode for this PDN
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds707_is_apn_pref_pkt_mode_ex
( 
  ds707_data_session_profile_info_type *data_sess_prof_ptr,
  sys_sys_mode_e_type               mode,
  sys_modem_as_id_e_type            subs_id
);

/*===========================================================================
FUNCTION DS707_GET_SYS_PREF_MODE

DESCRIPTION
  This function returns the sys pref mode for the PDN in 3GPP2.
  
PARAMETERS  
  pointer to the data_session_profile
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  pref_mode - Sys preferred mode corresponding to the APN
==========================================================================*/
sys_sys_mode_e_type ds707_get_sys_pref_mode
(
  ds707_data_session_profile_info_type *data_sess_prof_ptr
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_OMH_REQD

DESCRIPTION   Checks if OMH is required or not. OMH will be required if the
              following conditions are met.
              1. Call is on 1x/HRPD(non eHRPD) and
              2. Either the existing profile id or the requested profile id
                 supports OMH.

PARAMETERS    this_iface_ptr - current iface pointer
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - If OMH is required. 
              FALSE  - If OMH is not required.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_is_omh_reqd 
( 
  ps_iface_type* this_iface_ptr
);
/*===========================================================================
FUNCTION       DS707_DATA_SESSION_GET_NUM_VALID_PROFILES_IN_NVRUIM

DESCRIPTION    Returns number of valid profiles in NVRUIM

PARAMETERS     None.

DEPENDENCIES   None

RETURN VALUE   No. of valid profiles in NVRUIM.

SIDE EFFECTS   None
===========================================================================*/
uint8 ds707_data_session_get_num_valid_profiles_in_nvruim
( 
  void
);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_FREE_NON_PRIMARY_SUBS_PROFILE

DESCRIPTION   Frees the memory for profiles on non-default subscriptions. 
              This is triggered by profile refresh event sent by profile DB. 

PARAMETERS    subs_id - subscription ID on which modem refresh happens
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - If successful
              FALSE  - otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_profile_free_non_primary_subs_profile
(
  ds_profile_subs_id_enum_type subs_id
);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_FOR_ALWAYS_SHARE_PROFILE

DESCRIPTION   Determines the profile ID to use if ALWAYS_SHARE_PROFILE is 
              used for first call bring up 

PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  In 1x/HRPD system, return OMH default profile if OMH is 
              supported, else return first 1x/HRPD profile 0.
              Otherwise, return first eHRPD profile with non NULL APN.

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_profile_for_always_share_profile
(
  acl_policy_info_type *policy_info_ptr
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INIT_MANDATORY_APN_LIST

DESCRIPTION   This function reads the file mandatory APNs list and stores in local cache.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_init_mandatory_apn_list
(
  void
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_UPDATE_MIN_APN_LIST

DESCRIPTION   This function checks whether the list of mandatory APNs are all
              enabled in the profile and in the card, if its present.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: if all the APNs in the mandatory list are enabled
                             in profile and card.
                       FALSE: otherwise.

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_update_min_apn_list(char *new_apn , char *old_apn);

/*===========================================================================

FUNCTION      DS707_DATA_SESSION_PROCESS_UPDATE_MIN_APN_HDLR

DESCRIPTION   This function checks whether the list of mandatory APNs are all
              enabled in the profile and in the card, if its present.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: if all the APNs in the mandatory list are enabled
                             in profile and card.
                       FALSE: otherwise.

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_process_update_min_apn_hdlr(  ds_cmd_type *ds_cmd_ptr);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_SUBS_ID

DESCRIPTION  This function returns the internal ds707 data session profile 
             subscription ID to the id from CM.
 
PARAMETERS    as_id   - Subscription ID from CM

DEPENDENCIES  None

RETURN VALUE  ds_profile_subs_id_enum_type

SIDE EFFECTS  None
===========================================================================*/
ds_profile_subs_id_enum_type ds707_data_session_get_profile_subs_id
(
  sys_modem_as_id_e_type as_id
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_CLAT_ENABLED

DESCRIPTION   This function retrieves the "clat_enabled" flag specified for 
              a profile.

PARAMETERS      profile_number  : profile number 
                subs_id :Subscription id.
                clat_enabled : pointer to store clat_enabled
 
DEPENDENCIES  None

RETURN VALUE  TRUE:  Operation Successful
              FALSE: Operation Failed

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_is_clat_enabled
(
  ds707_data_session_profile_id  profile_id,
  ds_profile_subs_id_enum_type   subs_id,
  boolean                       *clat_enabled
);

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INIT_CRIT_SECT

DESCRIPTION   Called once at mobile power-up.  Initializes ds707 profile crit 
              section variable
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_init_crit_sect
(
  void
);
#endif /* DS707_DATA_SESSION_PROFILE_H */
