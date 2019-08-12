#ifndef DS_PROFILE_3GPP2_H
#define DS_PROFILE_3GPP2_H
/** 
  @file ds_profile_3gpp2.h 
  @brief 
   The file contains data structures to support 3GPP2 Application profiles
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dsProfile group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_profile_3gpp2.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/11   ty      Added PDN throttle failure and disallow timers to profile parameters.
12/06/11   msh     Corrected the indexes of DS_PROFILE_3GPP2_RAT_TYPEs 
11/10/11   jz      Add PDN label 
10/27/11   op      Added support for PDN level authentication
08/28/11   sd      (Tech Pubs) Applied new Doxygen grouping markup.
05/24/11   ack     Decreased the JCDMA profile number range 
04/19/11   ttv     Changes to use RAT type as bit mask and to have unique 
                   identifiers for apn related parameters. 
03/02/11   mg      doxygen comments
01/31/11   ttv     Changed the offset for 3GPP2 params to avoid the overlap
                   between the 3gpp and 3gpp2 params.
01/19/11   mg      Added identifiers for eHRPD parameters. Corrected enum 
                   values for pdn type
01/14/11   ss      Added support for Iface linger parameters.
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

/*===========================================================================
                        MACROS, TYPEDEFS AND VARIABLES
===========================================================================*/
/* group: dsprofile */

/** @addtogroup constants 
  @{
*/
/** Datatype for a 3GPP2 profile ID.
*/
#define ds_profile_3gpp2_profile_id  int32

/* Profile numbers for 3GPP2
*/
/** Invalid profile ID. */
#define DS_PROFILE_3GPP2_PROFILE_ID_INVALID               -1
/** Minimum profile ID. */
#define DS_PROFILE_3GPP2_PROFILE_ID_MIN                    0
/** Default profile ID. */
#define DS_PROFILE_3GPP2_DEFAULT_PROFILE_NUM               0
/** JCDMA Internet profile. */
#define DS_PROFILE_3GPP2_JCDMA_INET_PROFILE                1
/** Email with attachment profile. */
#define DS_PROFILE_3GPP2_EMAIL_WITH_ATTACHMENT_PROFILE     2
/** Email without attachment profile. */
#define DS_PROFILE_3GPP2_EMAIL_WITHOUT_ATTACHMENT_PROFILE  3
/** PSMS profile. */
#define DS_PROFILE_3GPP2_PSMS_PROFILE                      4
/** BREW PCSV profile. */
#define DS_PROFILE_3GPP2_BREW_PCSV_PROFILE                 5
/** BREW FA profile. */
#define DS_PROFILE_3GPP2_BREW_FA_PROFILE                   6
/** BREW other profile. */
#define DS_PROFILE_3GPP2_BREW_OTHER_PROFILE                7
/** BREW IPVT profile. */
#define DS_PROFILE_3GPP2_BREW_IPVT_PROFILE                 8
/** BREW PTm profile. */
#define DS_PROFILE_3GPP2_BREW_PTM_PROFILE                  9
/** OTA profile. */
#define DS_PROFILE_3GPP2_OTA_PROFILE                      10
/** UHM profile. */
#define DS_PROFILE_3GPP2_UHM_PROFILE                      11
/** Keitai profile. */
#define DS_PROFILE_3GPP2_KEITAI_UPDATE_PROFILE            12
/** WAP FA profile. */
#define DS_PROFILE_3GPP2_WAP_FA_PROFILE                   13
/** PSMS chat profile. */
#define DS_PROFILE_3GPP2_PSMS_CHAT_PROFILE                14
/** Corp profile. */
#define DS_PROFILE_3GPP2_CORP_PROFILE                     15
/** BML profile. */
#define DS_PROFILE_3GPP2_BML_PROFILE                      16
/** Device management profile. */
#define DS_PROFILE_3GPP2_DEVICE_MANAGEMENT_PROFILE        17
/** WINGP WAP profile. */
#define DS_PROFILE_3GPP2_WINGP_WAP_PROFILE                18
/** WINGP email profile. */
#define DS_PROFILE_3GPP2_WINGP_EMAIL_PROFILE              19
/** WINGP PCSV profile. */
#define DS_PROFILE_3GPP2_WINGP_PCSV_PROFILE               20
/** WINGP PSMS profile. */
#define DS_PROFILE_3GPP2_WINGP_PSMS_PROFILE               21
/** International roaming profile. */
#define DS_PROFILE_3GPP2_INTERNATIONAL_ROAMING_PROFILE    22
        
/** Number of valid profiles for JCDMA. */
#define DS_PROFILE_3GPP2_JCDMA_NO_VALID_PROFILE           22
/** JCDMA minimum profile number. */
#define DS_PROFILE_3GPP2_JCDMA_PROFILE_MIN                1
/** JCDMA maximum profile number. */
#define DS_PROFILE_3GPP2_JCDMA_PROFILE_MAX                50

/** eHRPD profile 1. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_1                  101
/** eHRPD profile 2. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_2                  102
/** eHRPD profile 3. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_3                  103
/** eHRPD profile 4. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_4                  104
/** eHRPD profile 5. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_5                  105
/** eHRPD profile 6. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_6                  106
/** eHRPD profile 7. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_7                  107

/** Number of valid profiles in eHRPD. */
#define DS_PROFILE_3GPP2_EHRPD_NO_VALID_PROFILE           7
/** eHRPD minimum profile number. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_MIN                101
/** eHRPD maximum profile number. */
#define DS_PROFILE_3GPP2_EHRPD_PROFILE_MAX                200
        
/** Maximum number of profiles. */
#define DS_PROFILE_3GPP2_PROFILE_ID_MAX                   30

#define DS_PROFILE_FAILURE_TIMER_MAX                      6
#define DS_PROFILE_DISALLOW_TIMER_MAX                     6

/** Maximum length for the profile name. Supported value: 0; profile names 
  are not supported for 3GPP2. 
*/
#define DS_PROFILE_3GPP2_MAX_PROFILE_NAME_LEN 0

/** Maximum length for userid. Supported value: Same as defined in #PPP_MAX_PASSWD_LEN.
    Please use DS_PROFILE_3GPP2_PPP_MAX_USER_ID_LEN + 1 for ds_profile_get_param()
*/
#define DS_PROFILE_3GPP2_PPP_MAX_USER_ID_LEN       127
/** Maximum length for password. Supported value: Same as defined in #PPP_MAX_USER_ID_LEN.
    Please use DS_PROFILE_3GPP2_PPP_MAX_PASSWD_LEN + 1 for ds_profile_get_param()
*/
#define DS_PROFILE_3GPP2_PPP_MAX_PASSWD_LEN        127

/** Maximum length of APN string. 
    Please use DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1 for ds_profile_get_param()
*/
#define DS_PROFILE_3GPP2_APN_MAX_VAL_LEN           100

/** No authentication */
#define DS_PROFILE_3GPP2_AUTH_PROTOCOL_NONE        0
/** PAP authentication protocol. */
#define DS_PROFILE_3GPP2_AUTH_PROTOCOL_PAP         1
/** CHAP authentication protocol. */
#define DS_PROFILE_3GPP2_AUTH_PROTOCOL_CHAP        2
/** PAP/CHAP authentication protocol. */
#define DS_PROFILE_3GPP2_AUTH_PROTOCOL_PAP_CHAP    3

/** Data rate: Low Speed -- Low speed Service Options (SO15) only. */
#define DS_PROFILE_3GPP2_DATARATE_LOW   0              
/** Data rate: Medium Speed -- SO33 + low R-SCH. */
#define DS_PROFILE_3GPP2_DATARATE_MED   1 
/** Data rate: High Speed -- SO33 + high R-SCH. (Default for RUIM)  */
#define DS_PROFILE_3GPP2_DATARATE_HIGH  2 

/** Data bearer: Hybrid 1x/1xEV-DO. (Default for RUIM)     */
#define DS_PROFILE_3GPP2_DATAMODE_CDMA_HDR     0   
/** Data bearer: 1x only. */
#define DS_PROFILE_3GPP2_DATAMODE_CDMA_ONLY    1   
/** Data bearer: 1xEV-DO only. */
#define DS_PROFILE_3GPP2_DATAMODE_HDR_ONLY     2   

/* DNS address 
    This is used in the IPv4 and IPv6 DNS address structure.
*/
/** Primary DNS address in an IPv4v6 structure. */
#define DS_PROFILE_3GPP2_PRIMARY_DNS_ADDR      0
/** Secondary DNS address in IPv4v6 structure. */
#define DS_PROFILE_3GPP2_SECONDARY_DNS_ADDR    1
/** Maximum number of DNS addresses. */
#define DS_PROFILE_3GPP2_MAX_NUM_DNS_ADDR      2  

/** Offset for 3GPP2 parameters. The offset is required to avoid the overlap 
  between 3GPP and 3GPP2 parameters. These new values are inline with the 
  updated QMI WDS spec.
*/
#define DS_PROFILE_3GPP2_PARAM_OFFSET              0x80
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/** Enumeration type for PDN Type. The UE can request IPv4 or IPv6 or dual IP PDN 
  connection.
*/
typedef enum
{
  DS_PROFILE_3GPP2_PDN_TYPE_V4     = 0,      /**< PDN type IPv4 only. */
  DS_PROFILE_3GPP2_PDN_TYPE_V6     = 1,      /**< PDN type IPv6 only. */
  DS_PROFILE_3GPP2_PDN_TYPE_V4_V6  = 2,      /**< PDN type IPv4 and IPv6. */
  DS_PROFILE_3GPP2_PDN_TYPE_UNSPEC = 3,      /**< Unspecified PDN type. */
  /** @cond
  */
  DS_PROFILE_3GPP2_PDN_TYPE_MAX    = 0xFF     /**< Internal use only. */
  /** @endcond */
}ds_profile_3gpp2_pdn_type_enum_type;

/** Radio access technology types.
*/
typedef enum
{
  DS_PROFILE_3GPP2_RAT_TYPE_HRPD       = 1,     
    /**< Radio access technology type HRPD only.       */
  DS_PROFILE_3GPP2_RAT_TYPE_EHRPD      = 2,     
    /**< Radio access technology type eHRPD only.      */
  DS_PROFILE_3GPP2_RAT_TYPE_HRPD_EHRPD = 3,     
    /**< Radio access technology type HRPD/eHRPD.      */
  /** @cond
  */
  DS_PROFILE_3GPP2_RAT_TYPE_MAX        = 0xFF   /**< Internal use only. */
  /** @endcond */
}ds_profile_3gpp2_rat_type_enum_type;

/** @brief Stores the IPv4 address. This structure is used for a DNS address
    similar to ps_in_addr structures.
*/
typedef struct ds_profile_3gpp2_in_addr
{
  uint32 ds_profile_3gpp2_s_addr; /**< Socket address. */
}ds_profile_3gpp2_in_addr_type;

/** @brief Stores the IPv6 address. This structure is used for a DNS address
    similar to ps_in6_addr structures.
*/
typedef struct ds_profile_3gpp2_in6_addr
{
    /** Stores the IPv6 address. */
    union
  {
    uint8   ds_profile_3gpp2_u6_addr8[16];  /**< IPv6 8-bit socket address. */
    uint16  ds_profile_3gpp2_u6_addr16[8];  /**< IPv6 16-bit socket address. */
    uint32  ds_profile_3gpp2_u6_addr32[4];  /**< IPv6 32-bit socket address. */
    uint64  ds_profile_3gpp2_u6_addr64[2];  /**< IPv6 64-bit socket address. */
  } ds_profile_3gpp2_in6_u;
#define ds_profile_3gpp2_s6_addr    ds_profile_3gpp2_in6_u.ds_profile_3gpp2_u6_addr8
  /**< IPv6 8-bit socket address type. */
#define ds_profile_3gpp2_s6_addr16  ds_profile_3gpp2_in6_u.ds_profile_3gpp2_u6_addr16
  /**< IPv6 16-bit socket address type. */
#define ds_profile_3gpp2_s6_addr32  ds_profile_3gpp2_in6_u.ds_profile_3gpp2_u6_addr32
  /**< IPv6 32-bit socket address type. */
#define ds_profile_3gpp2_s6_addr64  ds_profile_3gpp2_in6_u.ds_profile_3gpp2_u6_addr64
  /**< IPv6 64-bit socket address type. */
}ds_profile_3gpp2_in6_addr_type;

/* Datatypes defined for all the parameters used in the profile
*/ 
/** Negotiate DNS server type. */
typedef boolean ds_profile_3gpp2_negotiate_dns_server_type;
/** PPP session close timer data optimized type. */
typedef uint32  ds_profile_3gpp2_ppp_session_close_timer_DO_type;
/** PPP session close timer 1x type. */
typedef uint32  ds_profile_3gpp2_ppp_session_close_timer_1X_type;
/** Allow linger type. */
typedef boolean ds_profile_3gpp2_allow_linger_type;
/** Linger timeout value type. */
typedef uint16  ds_profile_3gpp2_linger_timeout_val_type;
/** LCP acknowledgement timeout type. */
typedef uint16  ds_profile_3gpp2_lcp_ack_timeout_type;
/** IPCP acknowledgement timeout type. */
typedef uint16  ds_profile_3gpp2_ipcp_ack_timeout_type;
/** Authentication timeout type. */
typedef uint16  ds_profile_3gpp2_auth_timeout_type;
/** LCP configuration request retry count type. */
typedef uint8   ds_profile_3gpp2_lcp_creq_retry_count_type;
/** IPCP configuration request retry count type. */
typedef uint8   ds_profile_3gpp2_ipcp_creq_retry_count_type;
/** Authentication retry count type. */
typedef uint8   ds_profile_3gpp2_auth_retry_count_type;
/** Authentication protocol type. */
typedef uint8   ds_profile_3gpp2_auth_protocol_type;
/** Data rate type. */
typedef uint8   ds_profile_3gpp2_data_rate_type; 
/** Data mode type. */
typedef uint8   ds_profile_3gpp2_data_mode_type;
/** Application type type. */
typedef uint32  ds_profile_3gpp2_app_type_type;    
/** Application priority type. */
typedef uint8   ds_profile_3gpp2_app_priority_type;
/** Indicates whether the PCSCF address is needed. */
typedef boolean ds_profile_3gpp2_is_pcscf_addr_needed_type; 
/** APN enabled type. */
typedef boolean ds_profile_3gpp2_apn_enabled;
/** PDN inactivity timeout type. */
typedef uint32  ds_profile_3gpp2_pdn_inactivity_timeout;  
/** APN class type. */
typedef uint8   ds_profile_3gpp2_apn_class; 
/** PDN throttle failure timer type. */
typedef uint32  ds_profile_3gpp2_failure_timer_type;
/** PDN throttle disallow timer type. */
typedef uint32  ds_profile_3gpp2_disallow_timer_type;
/** Updated PDN throttle failure timer type. */
typedef uint32 ds_profile_3gpp2_failure_timers_type[DS_PROFILE_FAILURE_TIMER_MAX];
/** Updated throttle disallow timer type. */
typedef uint32 ds_profile_3gpp2_disallow_timers_type[DS_PROFILE_DISALLOW_TIMER_MAX];

typedef uint16  ds_profile_3gpp2_op_pco_id_type;
typedef uint16  ds_profile_3gpp2_mcc_type;
typedef PACKED struct PACKED_POST
{
  uint16  ds_profile_3gpp2_mnc;
  boolean mnc_includes_pcs_digit; //TRUE: 3 digits
}ds_profile_3gpp2_mnc_type;

/** 4 byte opaque user data. */
typedef uint32  ds_profile_3gpp2_user_app_data;

typedef boolean ds_profile_3gpp2_request_dns_address_using_dhcp_flag_type;

typedef boolean ds_profile_3gpp2_request_pcscf_address_using_dhcp_flag_type;

/** Identifies technology-specific profile parameters.
*/
typedef enum
{
  DS_PROFILE_3GPP2_PROFILE_PARAM_INVALID                 = 0x0,
    /**< Invalid profile parameter. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_MIN                     = 0x10 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Minimum profile parameter. */

  DS_PROFILE_3GPP2_PROFILE_PARAM_NEGOTIATE_DNS_SERVER    = 
                                    DS_PROFILE_3GPP2_PROFILE_PARAM_MIN,
    /**< Negotiate the DNS server. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_DO  = 0x11 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PPP session close timer DO. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_1X  = 0x12 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PPP session close timer 1x. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_ALLOW_LINGER            = 0x13 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Allow linger. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_LCP_ACK_TIMEOUT         = 0x14 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< LCP acknowledge timeout. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_IPCP_ACK_TIMEOUT        = 0x15 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< IPCP acknowledge timeout. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_TIMEOUT            = 0x16 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Authentication timeout. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_LCP_CREQ_RETRY_COUNT    = 0x17 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< LCP configuration request count. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_IPCP_CREQ_RETRY_COUNT   = 0x18 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< IPCP configuration request count. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_RETRY_COUNT        = 0x19 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET, 
    /**< Authentication retry count. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL           = 0x1A + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET, 
    /**< Authentication protocol. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_USER_ID                 = 0x1B + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< User ID. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD           = 0x1C + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Authentication password. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DATA_RATE               = 0x1D + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Data rate. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DATA_MODE               = 0x1F + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Data mode. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_APP_TYPE                = 0x1E + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Application type. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_APP_PRIORITY            = 0x20 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Application priority. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING              = 0x21 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET, 
    /**< APN string. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE                = 0x22 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN type. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED    = 0x23 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Indicates whether the PCSCF address is needed. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY     = 0x24 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< IPv4 primary DNS address. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY   = 0x25 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< IPv4 secondary DNS address. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY     = 0x26 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< IPv6 primary DNS address. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY   = 0x27 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< IPv6 secondary DNS address. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_RAT_TYPE                = 0x28 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< RAT type. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_APN_ENABLED             = 0x29 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET, 
    /**< APN enabled. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_INACTIVITY_TIMEOUT  = 0x2A + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET, 
    /**< PDN inactivity timeout. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_APN_CLASS               = 0x2B + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< APN class. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_LINGER_PARAMS           = 0x2C + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Linger parameter. */
  DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PROTOCOL = 0x2D + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Level Authentication Protocol Type */
  DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID       = 0x2E + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Level Authentication User ID */
  DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD = 0x2F + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,                                    
    /**< PDN Level Authentication Password */
  DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LABEL               = 0x30 +
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Label */  
  DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_1         = 0x31 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Failure Throttle Timer 1 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_2         = 0x32 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Failure Throttle Timer 2 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_3         = 0x33 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Failure Throttle Timer 3 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_4         = 0x34 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Failure Throttle Timer 4 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_5         = 0x35 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Failure Throttle Timer 5 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_6         = 0x36 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Failure Throttle Timer 6 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_1        = 0x37 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Disallow Throttle Timer 1 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_2        = 0x38 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Disallow Throttle Timer 2 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_3        = 0x39 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Disallow Throttle Timer 3 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_4        = 0x3A + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Disallow Throttle Timer 4 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_5        = 0x3B + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Disallow Throttle Timer 5 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_6        = 0x3C + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Disallow Throttle Timer 6 */
  DS_PROFILE_3GPP2_PROFILE_PARAM_OP_PCO_ID   = 0x3D +
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
  /**< Operator Reserved PCO ID  */

  DS_PROFILE_3GPP2_PROFILE_PARAM_MCC                     = 0x3E +
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
  DS_PROFILE_3GPP2_PROFILE_PARAM_MNC                     = 0x3F +
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
  
  DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMERS          = 0x40 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Failure Throttle Timers */
  DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMERS         = 0x41 + 
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< PDN Disallow Throttle Timers */
  
  DS_PROFILE_3GPP2_PROFILE_PARAM_USER_APP_DATA           = 0x42 +
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< 4 byte opaque data */
    
  DS_PROFILE_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG     = 0x43 +
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Indicates if P-CSCF address is requested via DHCP. */

  DS_PROFILE_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG       = 0x44 +
                                    DS_PROFILE_3GPP2_PARAM_OFFSET,
    /**< Indicates if DNS address is requested via DHCP. */
    
  /** @cond
  */
  DS_PROFILE_3GPP2_PROFILE_PARAM_MAX  = DS_PROFILE_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG
    /**< Maximum profile parameter (used internally). */
  /** @endcond */
}ds_profile_3gpp2_param_enum_type;

/** @} */ /* end_addtogroup datatypes */

#endif /* DS_PROFILE_3GPP2_H */
