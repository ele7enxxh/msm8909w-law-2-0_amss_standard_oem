/******************************************************************************
  @file    ds_profile_db.h
  @brief   Database 

  DESCRIPTION
  API for profiles database. Should be used by ds_profile and mode handlers.
  The API is divided into two sections :
   - functions to operate on the profile contents that are stored 
     in the local memory ( allocate space, set/get params, etc)
   - functions to modify/retrieve profiles in the storage medium (EFS, cache, etc)

   Profile DB API's use managed profile type which allows tracking of the changes.
   Legacy C format is provided as well which does not track any changes.    

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2013-15 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/13   ez      Created the module. First version of the file.
02/27/13   am      Implementation provided
22/04/13   am      API was changed to support complex profile types, i.e EPC

===========================================================================*/

#ifndef DS_PROFILE_DB_H
#define DS_PROFILE_DB_H

#include "comdef.h"
#include "dsumtspdpreg.h"
/*===========================================================================
  PROFILEDB VERSION
  !!! should be updated for every change in profile structure !!!
===========================================================================*/
#define DS_PROFILE_DB_VERSION 1.0

/*===========================================================================
  Common profile definitions
===========================================================================*/
#define DS_PROFILE_DB_MAX_PROFILE_NAME_LEN   15
#define DS_PROFILE_DB_MAX_APN_NAME_LEN       100
#define DS_PROFILE_DB_MAX_AUTH_PASS_LEN      127
#define DS_PROFILE_DB_MAX_AUTH_USER_LEN      127

typedef uint16    ds_profile_db_profile_num_type;
#define DB_UNSPECIFIED_PROFILE_NUM 0xFFFF

#define DB_MAXIMUM_PROFILE_NUM 32000
/* the amount is shared between 3GPP and 3GPP2 profiles */
#define DS_PROFILE_DB_MAX_PERS_PROFILE_COUNT  (16)


typedef boolean  ds_profile_db_apn_disable_flag_type;
typedef uint8    ds_profile_db_apn_class_type;
typedef boolean  ds_profile_db_req_pcscf_address_flag_type;
typedef boolean  ds_profile_db_allow_linger_type;
typedef uint16   ds_profile_db_op_pco_id_type;
typedef uint16   ds_profile_db_mcc_type;
typedef uint32   ds_profile_db_user_profile_class_type;
typedef boolean  ds_profile_db_req_dhcp_dns_address_flag_type;
typedef boolean  ds_profile_db_req_dhcp_pcscf_address_flag_type;

typedef uint32   ds_profile_db_callback_id_type;

typedef enum {
  DB_TECH_MIN     = 0,
  DB_TECH_3GPP    = 0,
  DB_TECH_3GPP2   = 1,
  DB_TECH_COMMON  = 2,
  DB_TECH_MAX     = 3,
  DB_TECH_INVALID = 0xFF
} ds_profile_db_tech_type;

typedef uint16 ds_profile_db_supported_tech_mask;

typedef enum
{
  DB_DATA_SESS_AUTH_PROTOCOL_NONE     = 0,
  DB_DATA_SESS_AUTH_PROTOCOL_PAP      = 1,
  DB_DATA_SESS_AUTH_PROTOCOL_CHAP     = 2,
  DB_DATA_SESS_AUTH_PROTOCOL_PAP_CHAP = 3,
  DB_DATA_SESS_AUTH_PROTOCOL_EAP      = 4,
  DB_DATA_SESS_AUTH_PROTOCOL_MAX      = 0xff
} ds_profile_db_auth_protocol;

typedef PACKED struct PACKED_POST
{
  ds_profile_db_auth_protocol auth_type; 
  char password[DS_PROFILE_DB_MAX_AUTH_PASS_LEN+1];
  char username[DS_PROFILE_DB_MAX_AUTH_USER_LEN+1];
} ds_profile_db_auth_info;

typedef enum {
  DB_IP_V4   = 4,       /**< IPv4 version. */
  DB_IP_V6   = 6,       /**< IPv6 version. */
  DB_IP_V4V6 = 10,     /**< IPv4v6 version. */
  DB_IP_MAX  = 0xff
} ds_profile_db_ip_version;

typedef PACKED struct PACKED_POST
{
  uint32 addr;
} ds_profile_db_ip_addr_v4_type;

typedef PACKED struct PACKED_POST
{
  PACKED union PACKED_POST
  {
    uint8   addr8[16]; /**< IPv6 8-bit PDP address size. */
    uint16  addr16[8]; /**< IPv6 16-bit PDP address size. */ 
    uint32  addr32[4]; /**< IPv6 32-bit PDP address size. */ 
    uint64  addr64[2]; /**< IPv6 64-bit PDP address size. */ 
  } addr; 
} ds_profile_db_ip_addr_v6_type;

typedef PACKED struct PACKED_POST 
{
  ds_profile_db_ip_addr_v4_type v4;                     
  ds_profile_db_ip_addr_v6_type v6;                     
} ds_profile_db_ip_addr_type;

typedef PACKED struct PACKED_POST
{
  ds_profile_db_allow_linger_type allow_linger_flag;  
  /**< Flag to indicate whether lingering is */
  /**< supported for this profile. */

  uint16  linger_timeout_val; /**< The lingering timeout value in seconds. */
} ds_profile_db_linger_params_type;

typedef PACKED struct PACKED_POST
{
  uint16 mnc_digits;   
  /**< actual mnc digit */
  boolean mnc_includes_pcs_digit;
  /**< This field is used to interpret the length of the corresponding
    MNC reported Values:
   - TRUE -- MNC is a three-digit value; e.g., a reported value of 
    90 corresponds to an MNC value of 090

  - FALSE -- MNC is a two-digit value; e.g., a reported value of 
    90 corresponds to an MNC value of 90 */
} ds_profile_db_mnc_type;

/*===========================================================================
  3GPP profile definitions
===========================================================================*/
#define DS_PROFILE_DB_3GPP_MAX_TFT_PARAM_SETS 2
#define DS_PROFILE_DB_3GPP_MAX_OTAP_NAPID_LEN 48

#define DS_PROFILE_DB_3GPP_TFT_FILTER_ID1   0
#define DS_PROFILE_DB_3GPP_TFT_FILTER_ID2   1
#define DS_PROFILE_DB_3GPP_FAILURE_TIMER_MAX  10
typedef byte     ds_profile_db_3gpp_pdp_context_number_type;
typedef boolean  ds_profile_db_3gpp_pdp_context_secondary_flag_type;
typedef byte     ds_profile_db_3gpp_pdp_context_primary_id_type;
typedef boolean  ds_profile_db_3gpp_im_cn_flag_type;
typedef uint32   ds_profile_db_3gpp_inactivity_timer_val_type;
typedef uint32   ds_profile_db_3gpp_src_stat_desc_type;
typedef uint8    ds_profile_db_3gpp_apn_bearer_type;
typedef boolean  ds_profile_db_3gpp_emergency_calls_are_supported;

typedef uint32   ds_profile_db_3gpp_failure_timers[DS_PROFILE_DB_3GPP_FAILURE_TIMER_MAX];

typedef enum
{
  DB_3GPP_IP     = 0,                    /**< PDP type IP. */
  DB_3GPP_IPV4   = 0,                  /**< PDP type IPv4. */
  DB_3GPP_PPP    = 1,                       /**< PDP type PPP. */
  DB_3GPP_IPV6   = 2,                      /**< PDP type IPv6. */
  DB_3GPP_IPV4V6 = 3,                    /**< PDP type IPv4v6. */
  DB_3GPP_IP_MAX = 0xff
} ds_profile_db_3gpp_pdp_protocol ;

typedef enum
{
  DB_3GPP_HEADER_COMP_OFF             = 0, /**< PDP header compression is off. */
  DB_3GPP_HEADER_COMP_ON              = 1, /**< Manufacturer preferred compression. */
  DB_3GPP_HEADER_COMP_RFC1144         = 2, /**< PDP header compression based on 
                                        RFC 1144.*/
  DB_3GPP_HEADER_COMP_RFC2507         = 3, /**< PDP header compression based on 
                                        RFC 2507.*/
  DB_3GPP_HEADER_COMP_RFC3095         = 4, /**< PDP header compression based on 
                                        RFC 3095.*/ 
  DB_3GPP_HEADER_COMP_MAX             = 0xff
} ds_profile_db_3gpp_pdp_header_comp;

typedef enum
{
  DB_3GPP_DATA_COMP_OFF     = 0,   /**< PDP data compression is off. */
  DB_3GPP_DATA_COMP_ON      = 1,   /**< Manufacturer preferred compression. */
  DB_3GPP_DATA_COMP_V42_BIS = 2,   /**< V.42BIS data compression. */
  DB_3GPP_DATA_COMP_V44     = 3,   /**< V.44 data compression. */ 
  DB_3GPP_DATA_COMP_MAX     = 0xff
} ds_profile_db_3gpp_pdp_data_comp;

typedef enum 
{
  DB_3GPP_ACCESS_CONTROL_NONE       = 0,    /**< No access. */
  DB_3GPP_ACCESS_CONTROL_REJECT     = 1,    /**< Reject access. */
  DB_3GPP_ACCESS_CONTROL_PERMISSION = 2,    /**< Allow access. */
  DB_3GPP_ACCESS_CONTROL_MAX        = 0xff  /**< Allow access. */
} ds_profile_db_3gpp_pdp_access_control;

typedef enum 
{
  DB_3GPP_IPV4_ADDR_ALLOC_NAS    = 0,    /**< Address allocation using NAS. */
  DB_3GPP_IPV4_ADDR_ALLOC_DHCPV4 = 1,    /**< Address allocation using 
                                              DHCPv4. */
  DB_3GPP_IPV4_ADDR_ALLOC_MAX    = 0xff
} ds_profile_db_3gpp_pdp_ipv4_addr_alloc;

typedef enum
{
  DB_3GPP_UMTS_QOS_TCLASS_SUBSCRIBE = 0,    /**< Subscribed. */
  DB_3GPP_UMTS_QOS_TCLASS_CONV      = 1,    /**< Conversational. */
  DB_3GPP_UMTS_QOS_TCLASS_STRM      = 2,    /**< Streaming. */
  DB_3GPP_UMTS_QOS_TCLASS_INTR      = 3,    /**< Interactive. */
  DB_3GPP_UMTS_QOS_TCLASS_BACK      = 4,     /**< Background. */
  DB_3GPP_UMTS_QOS_TCLASS_MAX       = 0xff
} ds_profile_db_3gpp_umts_qos_tclass;

typedef enum
{
  DB_3GPP_UMTS_QOS_DORDER_SUBSCRIBE = 0,    /**< Subscribed. */
  DB_3GPP_UMTS_QOS_DORDER_YES       = 1,    /**< With delivery order. */
  DB_3GPP_UMTS_QOS_DORDER_NO        = 2,    /**< Without delivery order. */
  DB_3GPP_UMTS_QOS_DORDER_MAX       = 0xff  /**< Without delivery order. */
} ds_profile_db_3gpp_umts_qos_dorder;

typedef enum
{
  DB_3GPP_UMTS_QOS_SDU_ERR_SUBSCRIBE  = 0,   /**< Subscribed. */
  DB_3GPP_UMTS_QOS_SDU_ERR_1E2        = 1,   /**< 1E-2. */
  DB_3GPP_UMTS_QOS_SDU_ERR_7E3        = 2,   /**< 7E-3. */
  DB_3GPP_UMTS_QOS_SDU_ERR_1E3        = 3,   /**< 1E-3. */
  DB_3GPP_UMTS_QOS_SDU_ERR_1E4        = 4,   /**< 1E-4. */
  DB_3GPP_UMTS_QOS_SDU_ERR_1E5        = 5,   /**< 1E-5. */
  DB_3GPP_UMTS_QOS_SDU_ERR_1E6        = 6,   /**< 1E-4. */
  DB_3GPP_UMTS_QOS_SDU_ERR_1E1        = 7,   /**< 1E-1. */
  DB_3GPP_UMTS_QOS_SDE_ERR_MAX        = 0xff
} ds_profile_db_3gpp_umts_qos_sdu_error;

typedef enum
{
  DB_3GPP_UMTS_QOS_RES_BER_SUBSCRIBE  = 0,   /**< Subscribed. */
  DB_3GPP_UMTS_QOS_RES_BER_5E2        = 1,   /**< 5E-2. */
  DB_3GPP_UMTS_QOS_RES_BER_1E2        = 2,   /**< 1E-2. */
  DB_3GPP_UMTS_QOS_RES_BER_5E3        = 3,   /**< 5E-3. */
  DB_3GPP_UMTS_QOS_RES_BER_4E3        = 4,   /**< 4E-3. */
  DB_3GPP_UMTS_QOS_RES_BER_1E3        = 5,   /**< 1E-3. */
  DB_3GPP_UMTS_QOS_RES_BER_1E4        = 6,   /**< 1E-4. */
  DB_3GPP_UMTS_QOS_RES_BER_1E5        = 7,   /**< 1E-5. */
  DB_3GPP_UMTS_QOS_RES_BER_1E6        = 8,   /**< 1E-6. */
  DB_3GPP_UMTS_QOS_RES_BER_6E8        = 9,   /**< 6E-8. */
  DB_3GPP_UMTS_QOS_RES_BER_MAX        = 0xff
} ds_profile_db_3gpp_umts_qos_res_ber;

typedef enum
{
  DB_3GPP_UMTS_QOS_SDU_DLVR_SUBSCRIBE = 0,  /**< Subscribed. */
  DB_3GPP_UMTS_QOS_SDU_DLVR_NODETECT  = 1,  /**< No detection. */
  DB_3GPP_UMTS_QOS_SDU_DLVR_YES       = 2,  /**< Erroneous SDU is delivered. */
  DB_3GPP_UMTS_QOS_SDU_DLVR_NO        = 3,   /**< Erroneous SDU is not delivered. */
  DB_3GPP_UMTS_QOS_SDU_DLVR_MAX       = 0xff
} ds_profile_db_3gpp_umts_qos_sdu_dlvr;

typedef PACKED struct PACKED_POST
{
  ds_profile_db_3gpp_umts_qos_tclass traffic_class; /**< Traffic class. */
  uint32    max_ul_bitrate;                 /**< Maximum UL bitrate. */
  uint32    max_dl_bitrate;                 /**< Maximum DL bitrate. */
  uint32    gtd_ul_bitrate;                 /**< Guaranteed UL bitrate. */
  uint32    gtd_dl_bitrate;                 /**< Guaranteed DL bitrate. */
  ds_profile_db_3gpp_umts_qos_dorder dlvry_order; /**< SDU delivery order. */
  uint32    max_sdu_size;                   /**< Maximum SDU size. */
  ds_profile_db_3gpp_umts_qos_sdu_error  sdu_err; /**< SDU error ratio list index. */
  ds_profile_db_3gpp_umts_qos_res_ber    res_biterr;   /**< Residual bit error list 
                                                      index. */
  ds_profile_db_3gpp_umts_qos_sdu_dlvr   dlvr_err_sdu; /**< Delivery of erroneous 
                                                      SDU. */
  uint32    trans_delay;                    /**< Transfer delay. */
  uint32    thandle_prio;                   /**< Traffic handling priority. */
  boolean   sig_ind;                        /**< Signaling indication flag. */
} ds_profile_db_3gpp_umts_qos_params_type;

typedef PACKED struct PACKED_POST
{
  uint32    precedence;     /**< Precedence class. */
  uint32    delay;          /**< Delay class. */
  uint32    reliability;    /**< Reliability class. */
  uint32    peak;           /**< Peak throughput class. */
  uint32    mean;           /**< Mean throughput class. */
} ds_profile_db_3gpp_umts_gprs_qos_params_type;

typedef enum
{
  DB_3GPP_LTE_QCI_0   = 0,   /**< LTE QoS QCI is 0. */
  DB_3GPP_LTE_QCI_1   = 1,   /**< LTE QoS QCI is 1. */ 
  DB_3GPP_LTE_QCI_2   = 2,   /**< LTE QoS QCI is 2. */
  DB_3GPP_LTE_QCI_3   = 3,   /**< LTE QoS QCI is 3. */ 
  DB_3GPP_LTE_QCI_4   = 4,   /**< LTE QoS QCI is 4. */
  DB_3GPP_LTE_QCI_5   = 5,   /**< LTE QoS QCI is 5. */ 
  DB_3GPP_LTE_QCI_6   = 6,   /**< LTE QoS QCI is 6. */
  DB_3GPP_LTE_QCI_7   = 7,   /**< LTE QoS QCI is 7. */ 
  DB_3GPP_LTE_QCI_8   = 8,   /**< LTE QoS QCI is 8. */
  DB_3GPP_LTE_QCI_9   = 9,   /**< LTE QoS QCI is 9. */  
  DB_3GPP_LTE_QCI_MAX = 0xff
} ds_profile_db_3gpp_lte_qci;

typedef PACKED struct PACKED_POST
{
  ds_profile_db_3gpp_lte_qci qci;         /**< QCI value. */
  uint32                 g_dl_bit_rate;   /**< Guaranteed DL bitrate. */
  uint32                 max_dl_bit_rate; /**< Maximum DL bitrate. */
  uint32                 g_ul_bit_rate;   /**< Guaranteed UL bitrate. */
  uint32                 max_ul_bit_rate; /**< Maximum UL bitrate. */
} ds_profile_db_3gpp_lte_qos_params_type;

typedef PACKED struct PACKED_POST
{  
  ds_profile_db_ip_addr_type   primary_dns_addr;   /**< Primary DNS address. */
  ds_profile_db_ip_addr_type   secondary_dns_addr; /**< Secondary DNS address. */
} ds_profile_db_3gpp_umts_dns_addresses_type;

typedef PACKED union PACKED_POST 
{
  ds_profile_db_ip_addr_v4_type v4;                     
  ds_profile_db_ip_addr_v6_type v6;                     
} ds_profile_db_3gpp_ip_addr_tft_type;

typedef PACKED union PACKED_POST
{
 uint32 mask;                      /**< Address mask for IPv4 address. */
 uint8  prefix_len;                /**< Prefix length for IPv6 address. */
} ds_profile_db_3gpp_mask_type;

typedef PACKED struct PACKED_POST
{
  ds_profile_db_3gpp_ip_addr_tft_type address; /**< IPv4 or IPv6 address. */
  ds_profile_db_3gpp_mask_type mask;
} ds_profile_db_3gpp_umts_address_mask;

typedef PACKED struct PACKED_POST
{
  uint16    from;                   /**< Port range lower limit. */
  uint16    to;                     /**< Port range upper limit. */
} ds_profile_db_3gpp_umts_port_range_type;

typedef PACKED struct PACKED_POST
{
  byte                             filter_id;    /**< Filter identifier. */
  byte                             eval_prec_id; /**< Evaluation precedence index. */
  ds_profile_db_ip_version              src_addr_mask_ip_ver;
  ds_profile_db_3gpp_umts_address_mask  src_addr_mask; /**< Source address and mask. */
  byte                       prot_num;     /**< Protocol number is greater than
                                                or equal to next_header in 
                                                IPv6. */
  ds_profile_db_3gpp_umts_port_range_type    dest_port_range; /**< Destination port range. */
  ds_profile_db_3gpp_umts_port_range_type    src_port_range;  /**< Source port range. */
  uint32               ipsec_spi;    /**< Security parameter index. */
  uint16               tos_mask;     /**< Type of service and mask is 
                                                greater than or equal to 
                                                tclass_mask in IPv6. */
  uint32               flow_label;   /**< Flow label. */
} ds_profile_db_3gpp_umts_tft_params_type;

typedef enum
{
  DB_3GPP_ROAMING_IP     = 0,                    /**< PDP type IP. */
  DB_3GPP_ROAMING_IPV4   = 0,                    /**< PDP type IPv4. */
  DB_3GPP_ROAMING_IPV6   = 1,                    /**< PDP type IPv6. */
  DB_3GPP_ROAMING_IPV4V6 = 2,                    /**< PDP type IPv4v6. */
  DB_3GPP_ROAMING_IP_MAX = 0xff
} ds_profile_db_3gpp_roaming_pdp_protocol ;

typedef enum
{
  DB_3GPP_OVERRIDE_HOME_IP     = 0,                    /**< PDP type IP. */
  DB_3GPP_OVERRIDE_HOME_IPV4   = 0,                    /**< PDP type IPv4. */
  DB_3GPP_OVERRIDE_HOME_IPV6   = 1,                    /**< PDP type IPv6. */
  DB_3GPP_OVERRIDE_HOME_IPV4V6 = 2,                    /**< PDP type IPv4v6. */
  DB_3GPP_OVERRIDE_HOME_IP_MAX = 0xff
} ds_profile_db_3gpp_override_home_pdp_protocol;

/*==========================================================================*/
/*===========================================================================
  3GPP2 profile definitions
===========================================================================*/
/*==========================================================================*/
#define DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS 6
#define DS_PROFILE_DB_3GPP2_FAILURE_TIMER_MAX       6
#define DS_PROFILE_DB_3GPP2_DISALLOW_TIMER_MAX      6

typedef boolean ds_profile_db_3gpp2_negotiate_dns_server_type;
typedef uint32  ds_profile_db_3gpp2_ppp_session_close_timer_DO_type;
typedef uint32  ds_profile_db_3gpp2_ppp_session_close_timer_1X_type;
typedef uint16  ds_profile_db_3gpp2_lcp_ack_timeout_type;
typedef uint16  ds_profile_db_3gpp2_ipcp_ack_timeout_type;
typedef uint16  ds_profile_db_3gpp2_auth_timeout_type;
typedef uint8   ds_profile_db_3gpp2_lcp_creq_retry_count_type;
typedef uint8   ds_profile_db_3gpp2_ipcp_creq_retry_count_type;
typedef uint8   ds_profile_db_3gpp2_auth_retry_count_type;
typedef uint8   ds_profile_db_3gpp2_auth_protocol_type;
typedef uint8   ds_profile_db_3gpp2_data_rate_type; 
typedef uint8   ds_profile_db_3gpp2_data_mode_type;
typedef uint32  ds_profile_db_3gpp2_app_type_type;    
typedef uint8   ds_profile_db_3gpp2_app_priority_type;
typedef boolean ds_profile_db_3gpp2_apn_enabled;
typedef uint32  ds_profile_db_3gpp2_pdn_inactivity_timeout;  
typedef uint32  ds_profile_db_3gpp2_failure_timer_type;
typedef uint32  ds_profile_db_3gpp2_disallow_timer_type;
typedef ds_profile_db_3gpp2_failure_timer_type  
  ds_profile_db_3gpp2_failure_timers_type[DS_PROFILE_DB_3GPP2_FAILURE_TIMER_MAX];

typedef ds_profile_db_3gpp2_disallow_timer_type  
  ds_profile_db_3gpp2_disallow_timers_type[DS_PROFILE_DB_3GPP2_DISALLOW_TIMER_MAX];


/*---------------------------------------------------------------------------
  bitmask for allowed types of 3gpp2 profiles
---------------------------------------------------------------------------*/
typedef enum
{
  DB_3GPP2_PROFILE_INVALID     = 0,
  DB_3GPP2_PROFILE_COMMON      = 1 << 0, // ONLY COMMON PARAMS ARE VALID
  DB_3GPP2_PROFILE_KDDI        = 1 << 1, // ONLY KDDI PARAMS ARE VALID
  DB_3GPP2_PROFILE_EHRPD       = 1 << 2, // ONLY EHRPD PARAMS ARE VALID
  DB_3GPP2_PROFILE_OMH         = 1 << 3, // ONLY OMH PARAMS ARE VALID
  DB_3GPP2_PROFILE_MAX         = 1 << 30
} ds_profile_db_3gpp2_profile_subtype;

typedef enum
{
  DB_3GPP2_PDN_TYPE_V4     = 0,      /**< PDN type IPv4 only. */
  DB_3GPP2_PDN_TYPE_V6     = 1,      /**< PDN type IPv6 only. */
  DB_3GPP2_PDN_TYPE_V4_V6  = 2,      /**< PDN type IPv4 and IPv6. */
  DB_3GPP2_PDN_TYPE_UNSPEC = 3,      /**< Unspecified PDN type. */
  DB_3GPP2_PDN_TYPE_MAX    = 0xFF     /**< Internal use only. */
}ds_profile_db_3gpp2_pdn_type;

/** Radio access technology types.
*/
typedef enum
{
  DB_3GPP2_RAT_TYPE_HRPD       = 1,     
  DB_3GPP2_RAT_TYPE_EHRPD      = 2,     
  DB_3GPP2_RAT_TYPE_HRPD_EHRPD = 3,     
  DB_3GPP2_RAT_TYPE_MAX        = 0xFF   /**< Internal use only. */
}ds_profile_db_3gpp2_rat_type;

/*===========================================================================
  Profile DB definitions
===========================================================================*/

typedef enum
{
  DB_EFS_FILE_TYPE_INVALID                      =   0,
  DB_EFS_FILE_TYPE_EPC_SUPPORT                  =   1,
  DB_EFS_FILE_TYPE_MAX                          =   DB_EFS_FILE_TYPE_EPC_SUPPORT
} ds_profile_db_efs_file_e_type;

typedef enum
{
  DB_FILE_READ_STATE_TYPE_NONE                       =   0,
  DB_FILE_READ_STATE_TYPE_1                          =   1,
  DB_FILE_READ_STATE_TYPE_2                          =   2,
  DB_FILE_READ_STATE_TYPE_3                          =   3,
  DB_FILE_READ_STATE_TYPE_4                          =   4,
  DB_FILE_READ_STATE_TYPE_5                          =   5,
  DB_FILE_READ_STATE_TYPE_6                          =   6,
  DB_FILE_READ_STATE_TYPE_MAX                        =   DB_FILE_READ_STATE_TYPE_6
} ds_profile_db_efs_read_state_e_type;
/*
  Make sure the number is updated everytime there is a 
  change in Max Subs id of system.
*/
typedef enum 
{
  DB_ACTIVE_SUBSCRIPTION_NONE      = -1,
  DB_ACTIVE_SUBSCRIPTION_NO_CHANGE =  0,
  DB_PROFILE_ACTIVE_SUBSCRIPTION_1 =  1,
  DB_PROFILE_ACTIVE_SUBSCRIPTION_2 =  2,
  DB_PROFILE_ACTIVE_SUBSCRIPTION_3 =  3,
  DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX  = DS_SUBSCRIPTION_MAX + 1
} ds_profile_db_subs_type;

typedef enum
{
  DB_INVALID_EVENT          =   0,
  DB_CREATE_PROFILE_EVENT   =   1,
  DB_DELETE_PROFILE_EVENT   =   2,
  DB_MODIFY_PROFILE_EVENT   =   3,
  DB_POWER_UP_EVENT         =   4,
  DB_RESET_PROFILE_EVENT    =   5,
  DB_REFRESH_PROFILE_EVENT  =   6,
  DB_MAX_EVENT              =   0xff
} ds_profile_db_event_type;

typedef enum {
  DB_EMBEDDED_PROFILE_FAMILY = 0,
  DB_TETHERED_PROFILE_FAMILY = 1,
  DB_MAX_PROFILE_FAMILY      = 2
} ds_profile_db_family_type;

typedef struct {
  void         *buf;  /**< Buffer to store the parameter value.
                           In case of set, can be void so the param 
                           is just reset to 0 */
  uint16 len;   /* buffer length */
} ds_profile_db_info_type;

typedef PACKED struct PACKED_POST {
  ds_profile_db_profile_num_type profile_num;
  boolean persistent;
  ds_profile_db_subs_type subscription_id;
} ds_profile_db_creation_params_type;

typedef struct {
  ds_profile_db_profile_num_type profile_num;
  ds_profile_db_supported_tech_mask tech_mask;
  boolean is_persistent;
}ds_profile_db_num_tech_type;

typedef enum {
  DB_PDP_TYPE_IP     = 0,                    /**< PDP type IP. */
  DB_PDP_TYPE_IPV4   = 0,                    /**< PDP type IPv4. */
  DB_PDP_TYPE_IPV6   = 1,                    /**< PDP type IPv6. */
  DB_PDP_TYPE_IPV4V6 = 2                     /**< PDP type IPv4v6. */
} ds_profile_db_pdp_type;
    

/* List of param idents */
/* !!! WARNING !!! 
   Make sure that the ranges defined by MIN/MAX for various tech
   do not intersect and each have a dedicated space */
typedef enum {

  /* for internal usage */
  INVALID_PROFILE_PARAM                                = 0,

  /* Technology specific params - 3GPP */
  /* *************************************/  

  /* Make sure ranges do not overlap ! */
  TECH_3GPP_PROFILE_PARAM_MIN                          = 0x10,

  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE         = 0x1 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PDP context type. */
  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP           = 0x2 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PDP header compression support. */
  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP           = 0x3 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PDP data compression support. */  
  TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY          = 0x5 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Primary IPv4 DNS address. */
  TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY        = 0x6 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Secondary IPv4 DNS address. */
  TECH_3GPP_PROFILE_PARAM_UMTS_REQ_QOS_EXTENDED        = 0x7 + TECH_3GPP_PROFILE_PARAM_MIN, 
    /**< Internal use only. */
  TECH_3GPP_PROFILE_PARAM_UMTS_MIN_QOS_EXTENDED        = 0x8 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Internal use only. */
  TECH_3GPP_PROFILE_PARAM_GPRS_REQ_QOS                 = 0x9 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< GPRS QoS request. */
  TECH_3GPP_PROFILE_PARAM_GPRS_MIN_QOS                 = 0xA + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< GPRS minimum QoS request. */
  TECH_3GPP_PROFILE_PARAM_AUTH_USERNAME                = 0xB + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PDP authentication username. */
  TECH_3GPP_PROFILE_PARAM_AUTH_PASSWORD                = 0xC + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PDP authentication password. */
  TECH_3GPP_PROFILE_PARAM_AUTH_TYPE                    = 0xD + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PDP authentication type. */
  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4      = 0xE + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< IPv4 address in PDP context. */
  TECH_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG               = 0xF + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PCSCF request flag. */
  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG = 0x10 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Mobile Terminated Access Control flag. */
  TECH_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG          = 0x11 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PCSCF request using the DHCP flag. */
  TECH_3GPP_PROFILE_PARAM_IM_CN_FLAG                   = 0x12 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< IP Multimedia Core Network flag. */
  TECH_3GPP_PROFILE_PARAM_TFT_FILTER_ID1               = 0x13 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< TFT filter ID 1. */
  TECH_3GPP_PROFILE_PARAM_TFT_FILTER_ID2               = 0x14 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< TFT filter ID 2. */
  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER           = 0x15 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< PDP context number. */
  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG   = 0x16 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Secondary PDP context flag. */
  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID       = 0x17 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Primary PDP context ID. */
  TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6      = 0x18 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< IPv6 address in PDP context. */
  TECH_3GPP_PROFILE_PARAM_UMTS_REQ_QOS                 = 0x19 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Request UMTS QoS. */
  TECH_3GPP_PROFILE_PARAM_UMTS_MIN_QOS                 = 0x1A + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Request for minimum UMTS QoS. */
  TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY          = 0x1B + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Primary IPv6 DNS address. */
  TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY        = 0x1C + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Secondary IPv6 DNS address. */
  TECH_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC              = 0x1D + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< IPv4 address allocation. */
  TECH_3GPP_PROFILE_PARAM_LTE_REQ_QOS                  = 0x1E + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Request LTE QoS. */
  TECH_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG             = 0x1F + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< APN Disable flag. */
  TECH_3GPP_PROFILE_PARAM_INACTIVITY_TIMER_VAL         = 0x20 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< APN inactivity timer value. */  
  TECH_3GPP_PROFILE_PARAM_APN_CLASS                    = 0x21 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< APN class parameter. */
  TECH_3GPP_PROFILE_PARAM_LINGER_PARAMS                = 0x22 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Iface linger parameter */
  TECH_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ            = 0x23 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Source statistics descriptor for requested UMTS QoS. */
  TECH_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN            = 0x24 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Source Statistics Descriptor for minimum UMTS QoS. */
  TECH_3GPP_PROFILE_PARAM_APN_BEARER                   = 0x25 + TECH_3GPP_PROFILE_PARAM_MIN,
    /** < APN bearer type specifier */
  TECH_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED    = 0x26 + TECH_3GPP_PROFILE_PARAM_MIN,
    /**< Flag used to identify is emergency calls can be made using 
      this profile. */
  TECH_3GPP_PROFILE_PARAM_OPERATOR_RESERVED_PCO        = 0x27 + TECH_3GPP_PROFILE_PARAM_MIN,  
      /** OPERATOR RESERVED PCO */
  TECH_3GPP_PROFILE_PARAM_MCC                          = 0x28 + TECH_3GPP_PROFILE_PARAM_MIN,
     /** MCC */
  TECH_3GPP_PROFILE_PARAM_MNC                          = 0x29 + TECH_3GPP_PROFILE_PARAM_MIN,
     /** MNC */
  TECH_3GPP_PROFILE_PARAM_MAX_PDN_CONN_PER_BLOCK       = 0x2A + TECH_3GPP_PROFILE_PARAM_MIN,  
      /** Max PDN Connections per Time Block */
  TECH_3GPP_PROFILE_PARAM_MAX_PDN_CONN_TIMER           = 0x2B + TECH_3GPP_PROFILE_PARAM_MIN,
     /** Max PDN Connection Time */
  TECH_3GPP_PROFILE_PARAM_PDN_REQ_WAIT_TIMER           = 0x2C + TECH_3GPP_PROFILE_PARAM_MIN,
     /** PDN Request Wait Time */
  TECH_3GPP_PROFILE_PARAM_USER_APP_DATA                = 0x2D + TECH_3GPP_PROFILE_PARAM_MIN,
     /** Application/User Data */
  TECH_3GPP_PROFILE_PARAM_ROAMING_DISALLOWED           = 0x2E + TECH_3GPP_PROFILE_PARAM_MIN,
    /** Roaming enable/disable flag */
  TECH_3GPP_PROFILE_PARAM_PDN_DISCON_WAIT_TIME         = 0x2F + TECH_3GPP_PROFILE_PARAM_MIN,
   /** PDN disconnectivity wait time */
  TECH_3GPP_PROFILE_PARAM_DNS_DHCP_REQ_FLAG            = 0x30 + TECH_3GPP_PROFILE_PARAM_MIN,
   /**< DNS request using the DHCP flag. */
  TECH_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE         = 0x31 + TECH_3GPP_PROFILE_PARAM_MIN,
  /**< LTE Roaming PDP context type*/
  TECH_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE        = 0x32 + TECH_3GPP_PROFILE_PARAM_MIN,
  /**< UMTS Roaming PDP context type*/
  TECH_3GPP_PROFILE_PARAM_IWLAN_TO_LTE_ROAMING_HO_FLAG = 0x33 + TECH_3GPP_PROFILE_PARAM_MIN,
  /**< Roaming handover from IWLAN to LTE allowed flag*/
  TECH_3GPP_PROFILE_PARAM_LTE_TO_IWLAN_ROAMING_HO_FLAG = 0x34 + TECH_3GPP_PROFILE_PARAM_MIN,
  /**< Roaming handover from LTE to IWLAN allowed flag*/
  TECH_3GPP_PROFILE_PARAM_FAILURE_TIMERS               = 0x35 + TECH_3GPP_PROFILE_PARAM_MIN,
  /**< PDN Failure Throttle Timers */
  TECH_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE       = 0x36 + TECH_3GPP_PROFILE_PARAM_MIN,
  /**< PDN Type override for home PDP type*/
  TECH_3GPP_PROFILE_PARAM_MAX                          = TECH_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE,
  /* Make sure ranges do not overlap ! */

                                      
  /* Technology specific params - 3GPP2 */
  /* *************************************/
  /* Make sure ranges do not overlap ! */
  TECH_3GPP2_PROFILE_PARAM_MIN                         = 0x90,

  TECH_3GPP2_PROFILE_PARAM_NEGOTIATE_DNS_SERVER        = 0x0 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< Negotiate the DNS server. */
  TECH_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_DO      = 0x1 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PPP session close timer DO. */
  TECH_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_1X      = 0x2 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PPP session close timer 1x. */
  TECH_3GPP2_PROFILE_PARAM_ALLOW_LINGER                = 0x3 + TECH_3GPP2_PROFILE_PARAM_MIN,                                    
    /**< Allow linger. */
  TECH_3GPP2_PROFILE_PARAM_LCP_ACK_TIMEOUT             = 0x4 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< LCP acknowledge timeout. */
  TECH_3GPP2_PROFILE_PARAM_IPCP_ACK_TIMEOUT            = 0x5 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< IPCP acknowledge timeout. */
  TECH_3GPP2_PROFILE_PARAM_AUTH_TIMEOUT                = 0x6 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< Authentication timeout. */
  TECH_3GPP2_PROFILE_PARAM_LCP_CREQ_RETRY_COUNT        = 0x7 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< LCP configuration request count. */
  TECH_3GPP2_PROFILE_PARAM_IPCP_CREQ_RETRY_COUNT       = 0x8 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< IPCP configuration request count. */
  TECH_3GPP2_PROFILE_PARAM_AUTH_RETRY_COUNT            = 0x9 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< Authentication retry count. */
  TECH_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL               = 0xA + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< Authentication protocol. */
  TECH_3GPP2_PROFILE_PARAM_USER_ID                     = 0xB + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< User ID. */
  TECH_3GPP2_PROFILE_PARAM_AUTH_PASSWORD               = 0xC + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< Authentication password. */
  TECH_3GPP2_PROFILE_PARAM_DATA_RATE                   = 0xD + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< Data rate. */
  TECH_3GPP2_PROFILE_PARAM_DATA_MODE                   = 0xF + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< Data mode. */
  TECH_3GPP2_PROFILE_PARAM_APP_TYPE                    = 0xE + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< Application type. */
  TECH_3GPP2_PROFILE_PARAM_APP_PRIORITY                = 0x10 + TECH_3GPP2_PROFILE_PARAM_MIN,   
    /**< Application priority. */
  TECH_3GPP2_PROFILE_PARAM_PDN_TYPE                    = 0x12 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN type. */
  TECH_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED        = 0x13 + TECH_3GPP2_PROFILE_PARAM_MIN,    
    /**< Indicates whether the PCSCF address is needed. */
  TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY         = 0x14 + TECH_3GPP2_PROFILE_PARAM_MIN, 
    /**< IPv4 primary DNS address. */
  TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY       = 0x15 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< IPv4 secondary DNS address. */
  TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY         = 0x16 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< IPv6 primary DNS address. */
  TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY       = 0x17 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< IPv6 secondary DNS address. */
  TECH_3GPP2_PROFILE_PARAM_RAT_TYPE                    = 0x18 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< RAT type. */
  TECH_3GPP2_PROFILE_PARAM_APN_ENABLED                 = 0x19 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< APN enabled. */
  TECH_3GPP2_PROFILE_PARAM_PDN_INACTIVITY_TIMEOUT      = 0x1A + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN inactivity timeout. */
  TECH_3GPP2_PROFILE_PARAM_APN_CLASS                   = 0x1B + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< APN class. */
  TECH_3GPP2_PROFILE_PARAM_LINGER_PARAMS               = 0x1C + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< Linger parameter. */
  TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PROTOCOL     = 0x1D + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Level Authentication Protocol Type */
  TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID           = 0x1E + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Level Authentication User ID */
  TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD     = 0x1F + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Level Authentication Password */
  TECH_3GPP2_PROFILE_PARAM_PDN_LABEL                   = 0x20 + TECH_3GPP2_PROFILE_PARAM_MIN,                                    
    /**< PDN Label */  
  TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_1             = 0x21 + TECH_3GPP2_PROFILE_PARAM_MIN,                                    
    /**< PDN Failure Throttle Timer 1 */
  TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_2             = 0x22 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Failure Throttle Timer 2 */
  TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_3             = 0x23 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Failure Throttle Timer 3 */
  TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_4             = 0x24 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Failure Throttle Timer 4 */
  TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_5             = 0x25 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Failure Throttle Timer 5 */
  TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_6             = 0x26 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Failure Throttle Timer 6 */
  TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_1            = 0x27 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Disallow Throttle Timer 1 */
  TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_2            = 0x28 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Disallow Throttle Timer 2 */
  TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_3            = 0x29 + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Disallow Throttle Timer 3 */
  TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_4            = 0x2A + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Disallow Throttle Timer 4 */
  TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_5            = 0x2B + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Disallow Throttle Timer 5 */
  TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_6            = 0x2C + TECH_3GPP2_PROFILE_PARAM_MIN,                                     
    /**< PDN Disallow Throttle Timer 6 */
  TECH_3GPP2_PROFILE_PARAM_OP_PCO_ID                   = 0x2D + TECH_3GPP2_PROFILE_PARAM_MIN,    
    /**< Operator Reserved PCO ID  */
  TECH_3GPP2_PROFILE_PARAM_MCC                         = 0x2E + TECH_3GPP2_PROFILE_PARAM_MIN,
  TECH_3GPP2_PROFILE_PARAM_MNC                         = 0x2F + TECH_3GPP2_PROFILE_PARAM_MIN,
  TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMERS              = 0x30 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< PDN Failure Throttle Timers */
  TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMERS             = 0x31 + TECH_3GPP2_PROFILE_PARAM_MIN, 
    /**< PDN Disallow Throttle Timers */
  TECH_3GPP2_PROFILE_PARAM_USER_APP_DATA               = 0x32 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< 4 byte opaque data */
  TECH_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG         = 0x33 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< Indicates if P-CSCF address is requested via DHCP. */
  TECH_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG           = 0x34 + TECH_3GPP2_PROFILE_PARAM_MIN,
    /**< Indicates if DNS address is requested via DHCP. */
  TECH_3GPP2_PROFILE_PARAM_MAX                         = 0xC4,
  /* Make sure ranges do not overlap ! */


  /* Common technology params that can be common and can be
     overwritten in complex profile types (i.e EPC)
     * Important *, new params should be added in the beginning by decrementing 
     COMMON_TECH_PARAM_MIN and adjusting other offsets*/
  /* *************************************/
  /* Make sure ranges do not overlap ! */
  COMMON_TECH_PARAM_MIN                                = 0x7D,

  COMMON_TECH_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG        = 0x0 + COMMON_TECH_PARAM_MIN,
    /** Indicates if P-CSCF address is requested via DHCP. */
  COMMON_TECH_PROFILE_PARAM_DNS_DHCP_REQ_FLAG          = 0x1 + COMMON_TECH_PARAM_MIN,
    /** Indicates if DNS address is requested via DHCP. */
  COMMON_TECH_PROFILE_PARAM_PDP_TYPE                   = 0x2 + COMMON_TECH_PARAM_MIN,
    /** Common PDP Type */
  COMMON_TECH_PROFILE_PARAM_USER_APP_DATA              = 0x3 + COMMON_TECH_PARAM_MIN,
    /** 4 byte opaque data */
  COMMON_TECH_PROFILE_PARAM_MNC                        = 0x4 + COMMON_TECH_PARAM_MIN,
    /** MNC */
  COMMON_TECH_PROFILE_PARAM_MCC                        = 0x5 + COMMON_TECH_PARAM_MIN,
    /** MCC */
  COMMON_TECH_PROFILE_PARAM_OPERATOR_RESERVED_PCO      = 0x6 + COMMON_TECH_PARAM_MIN,
    /** OPERATOR RESERVED PCO */
  COMMON_TECH_PROFILE_PARAM_AUTH_PASSWORD              = 0x7 + COMMON_TECH_PARAM_MIN,
    /**< PDP authentication password. */
  COMMON_TECH_PROFILE_PARAM_AUTH_USERNAME              = 0x8 + COMMON_TECH_PARAM_MIN,
    /**< PDP authentication username. */
  COMMON_TECH_PROFILE_PARAM_AUTH_TYPE                  = 0x9 + COMMON_TECH_PARAM_MIN,
    /**< PDP authentication type. */
  COMMON_TECH_PROFILE_PARAM_PCSCF_REQ_FLAG             = 0xA + COMMON_TECH_PARAM_MIN,
    /**< PCSCF request flag. */
  COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS              = 0xB + COMMON_TECH_PARAM_MIN,
    /**< Iface linger parameter */    
  COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY      = 0xC + COMMON_TECH_PARAM_MIN,
    /**< Secondary IPv6 DNS address. */   
  COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY        = 0xD + COMMON_TECH_PARAM_MIN,
    /**< Primary IPv6 DNS address. */    
  COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY      = 0xE + COMMON_TECH_PARAM_MIN,
    /**< Secondary IPv4 DNS address. */    
  COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY        = 0xF + COMMON_TECH_PARAM_MIN,  
    /**< Primary IPv4 DNS address. */    
  COMMON_TECH_PROFILE_PARAM_APN_CLASS                  = 0x10 + COMMON_TECH_PARAM_MIN,
    /**< APN class parameter. */    
  COMMON_TECH_PROFILE_PARAM_APN_DISABLE_FLAG           = 0x11 + COMMON_TECH_PARAM_MIN,
    /**< APN Disable flag. */
    

  COMMON_TECH_PARAM_MAX                                = 0x8E,
  /* Make sure ranges do not overlap ! */


  /* General Params used in all profiles */
  /* *************************************/
  /* currently there is no representation for those in legacy ds profile */
  GENERAL_PROFILE_PARAM_MIN                            = 0x1000,

  GENERAL_PROFILE_PARAM_PROFILE_NAME                   = 0x0 + GENERAL_PROFILE_PARAM_MIN,
    /**< Profile name. */
  GENERAL_PROFILE_PARAM_APN_NAME                       = 0x1 + GENERAL_PROFILE_PARAM_MIN,
    /**< access point name. */
  GENERAL_PROFILE_PARAM_SUBS_ID                        = 0x2 + GENERAL_PROFILE_PARAM_MIN,
    /** SUBS ID */
  GENERAL_PROFILE_PARAM_IPV6_DELEGATION                = 0x03 + GENERAL_PROFILE_PARAM_MIN,
    /** IPV6 Delegation */
  GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG               = 0x04 + GENERAL_PROFILE_PARAM_MIN,
   /** XLAT Enabled Flag */
  GENERAL_PROFILE_PARAM_MAX                            = GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG
  
} ds_profile_db_ident_type;


/* Status return values from functions */ 
typedef enum   
{
  DB_RESULT_SUCCESS    =       0,  
    /**< Successful operation. */
  DB_RESULT_FAIL,                  
    /**< General failure in the library. */
  DB_RESULT_ERR_INVAL_HNDL,        
    /**< Invalid profile handle. */ 
  DB_RESULT_ERR_INVAL_OP,          
    /**< Operation not supported. */                                                 
  DB_RESULT_ERR_INVAL_PROFILE_TYPE,
    /**< Invalid technology type. */
  DB_RESULT_ERR_INVAL_PROFILE_NUM, 
    /**< Invalid profile number. */
  DB_RESULT_ERR_INVAL_IDENT,       
    /**< Invalid identifier. */
  DB_RESULT_ERR_INVAL,             
    /**< Other invalid argument. */                                                
  DB_RESULT_ERR_LIB_NOT_INITED,    
    /**< Library has not been initialized. */                                                 
  DB_RESULT_ERR_LEN_INVALID,       
    /**< Buffer size length is invalid.\n
         For get_param, the buffer size cannot be less than the maximum.\n
         For set_param, the buffer size cannot be greater than the maximum. */
  DB_RESULT_LIST_END,              
    /**< End of the list has been reached. This status is the return value for 
         _itr_next. */  
  DB_RESULT_ERR_INVAL_SUBS_ID,     
    /**< Subscription ID is invalid. */
  DB_RESULT_ERR_INVAL_PROFILE_FAMILY,         
    /**< Invalid profile family. */
  DB_RESULT_ERR_PROFILE_VERSION_MISMATCH,
    /**< Profile Version Mismatch */
  DB_RESULT_ERR_OUT_OF_MEMORY,
    /**< Out of memory */
  DB_RESULT_ERR_FILE_ACCESS,            
    /**< Error accessing the embedded file system. */
  DB_RESULT_ERR_EOF,            
    /**< Error end of file. */
  DB_RESULT_ERR_VALID_FLAG_NOT_SET,    
    /**< Profile valid flag is not set.  */
  DB_RESULT_ERR_OUT_OF_PROFILES,   
    /**< No profiles are available while creating a new profile. */
  DB_RESULT_NO_EMERGENCY_PDN_SUPPORT,
    /**< Emergency PDN Feature disabled */
  DB_RESULT_NOT_SUPPORTED,
    /**< Emergency PDN Feature disabled */
  DB_RESULT_ERR_SET_IDENT_NOT_SUPPORTED,
    /**< Set Ident not supported on the profile*/
  DB_TECH_3GPP_SPEC_MIN = 0x1000,     
    /**< Offset for UMTS technology errors. */
  DB_RESULT_ERR_3GPP_INVAL_PROFILE_FAMILY,  
    /**< Invalid profile family. */
  DB_RESULT_ERR_3GPP_FILE_ACCESS,            
    /**< Error accessing the embedded file system. */
  DB_RESULT_ERR_3GPP_CONTEXT_NOT_DEFINED,   
    /**< Profile context is not defined. */
  DB_RESULT_ERR_3GPP_READ_ONLY_FLAG_SET,    
    /**< Read only flag is set in the profile. */
  DB_RESULT_ERR_3GPP_OUT_OF_PROFILES,   
    /**< No profiles are available while creating a new profile. */
  TECH_3GPP_SPEC_MAX = 0x10FF,     
    /**< Maximum for UMTS technology errors. */
  
  DB_TECH_3GPP2_SPEC_MIN = 0x1100,    
    /**< Offset for CDMA technology errors. */
  DB_RESULT_ERR_3GPP2_INVALID_IDENT_FOR_PROFILE, 
    /**< Identifier is not valid for the profile. */
  DB_RESULT_ERR_3GPP2_OUT_OF_PROFILES,
    /**< Error indicating out of 3GPP2 profiles. */
  DB_RESULT_ERR_3GPP2_OMH_PROFILE_NOT_VALID,
    /**< OMH profile with this number does not exist. */
  TECH_3GPP2_SPEC_MAX = 0x11FF,    
    /**< Maximum for CDMA technology errors. */
  /** @cond
  */
  DB_RESULT_MAX          = 0xFFFF
} ds_profile_db_result_type;


/*============================================================================
  VARIOUS BASE PROFILE TYPES
===========================================================================*/

/* Params that are common to all profiles */
typedef struct 
{
  ds_profile_db_profile_num_type profile_number;
  ds_profile_db_supported_tech_mask tech_mask; /* 0-bit is for the common-tech params */
  boolean persistent;
  char apn_name[DS_PROFILE_DB_MAX_APN_NAME_LEN+1];
  char profile_name[DS_PROFILE_DB_MAX_PROFILE_NAME_LEN+1];
  ds_profile_db_subs_type subscription_id;
  boolean ipv6_delegation;
  boolean clat_enabled;
} ds_profile_db_gen_params_type;
/*===========================================================================*/

/* Params that are common for all technologies. 
   For Complex Profile types (consisting of more than one base 
   technology, i.e EPC = 3GPP + 3GPP2) these will stored separately
   and also additionally in every underlying base technology. 
   This will allow user to set/get them in common place and also
   overwrite in a specific technology, according to his needs */
typedef PACKED struct PACKED_POST 
{  
  ds_profile_db_apn_disable_flag_type apn_disable_flag; 
  ds_profile_db_apn_class_type               apn_class; 

  PACKED struct PACKED_POST {
    ds_profile_db_ip_addr_type   primary_addr;   /**< Primary DNS address. */
    ds_profile_db_ip_version     primary_ip_version; /* INTERNAL */
    ds_profile_db_ip_addr_type   secondary_addr; /**< Secondary DNS address. */
    ds_profile_db_ip_version     secondary_ip_version; /* INTERNAL */
  } dns_addr;   

  PACKED struct PACKED_POST {
    ds_profile_db_linger_params_type    linger_params;
    boolean is_valid; /* INTERNAL */
  } iface_linger_params;
    
  ds_profile_db_req_pcscf_address_flag_type   request_pcscf_address_flag; 
  ds_profile_db_auth_info auth_info;
  /*Operator Reserved pco container ID*/
  ds_profile_db_op_pco_id_type  op_pco_id;
  ds_profile_db_mcc_type mcc;
  ds_profile_db_mnc_type mnc;
  ds_profile_db_user_profile_class_type user_app_data;
  ds_profile_db_pdp_type  pdp_type;    
  ds_profile_db_req_dhcp_dns_address_flag_type   request_dns_address_using_dhcp_flag;
  /* Flag to indicate if dns address should be requested using DHCP */
  ds_profile_db_req_dhcp_pcscf_address_flag_type   request_pcscf_address_using_dhcp_flag;
  /* Flag to indicate if pcscf address should be requested using DHCP */
} ds_profile_db_common_tech_params_type; 
/*===========================================================================*/

/* Params that are 3gpp specific */
typedef PACKED struct PACKED_POST 
{
  ds_profile_db_common_tech_params_type tech_common_params;
  
  PACKED struct PACKED_POST {
    boolean pdp;
    boolean qos_req_umts;
    boolean qos_min_umts;
    boolean qos_req_gprs;
    boolean qos_min_gprs;
    boolean qos_req_lte;
    boolean tft[DS_PROFILE_DB_3GPP_MAX_TFT_PARAM_SETS];
  } is_valid; /* INTERNAL */

  PACKED struct PACKED_POST {
    ds_profile_db_3gpp_pdp_context_number_type  pdp_context_number;     /**< Same as the profile number. */
    ds_profile_db_3gpp_pdp_protocol  pdp_type;    /**< PDP type (IP/PPP). */
    ds_profile_db_3gpp_pdp_header_comp   h_comp; /**< PDP header compression 
                                                  support. */
    ds_profile_db_3gpp_pdp_data_comp    d_comp; /**< Data compression support
                                                  -- v.42. */
    PACKED struct PACKED_POST {
        ds_profile_db_ip_version ip_vsn; /* INTERNAL */
        ds_profile_db_ip_addr_type addr;       
    } pdp_addr; /**< PDP address. */ 

    ds_profile_db_3gpp_pdp_context_secondary_flag_type   secondary_flag;                    /**< Secondary profile. */
    ds_profile_db_3gpp_pdp_context_primary_id_type      primary_id;                        /**< Link to primary profile. */
    ds_profile_db_3gpp_pdp_access_control     access_ctrl_flag; /**< Access control
                                                              flag. */
    ds_profile_db_3gpp_pdp_ipv4_addr_alloc ipv4_addr_alloc;  /**< Address allocation
                                                              mechanism. */
  } pdp_context;

  ds_profile_db_3gpp_umts_qos_params_type qos_request_umts; /**< UMTS QoS requested 
                                                            parameters. */
  ds_profile_db_3gpp_umts_qos_params_type qos_minimum_umts; /**< UMTS QoS minimum
                                                            parameters. */
  ds_profile_db_3gpp_umts_gprs_qos_params_type   qos_request_gprs; /**< GPRS QoS requested
                                                                   parameters. */
  ds_profile_db_3gpp_umts_gprs_qos_params_type   qos_minimum_gprs; /**< GPRS QoS minimum 
                                                                   parameters. */
  ds_profile_db_3gpp_lte_qos_params_type    qos_request_lte;  /**< LTE QoS requested 
                                                                   parameters. */
  ds_profile_db_3gpp_umts_tft_params_type        tft[DS_PROFILE_DB_3GPP_MAX_TFT_PARAM_SETS];
                                         /**< Traffic flow template. */
  ds_profile_db_3gpp_im_cn_flag_type   im_cn_flag; 
                                         /**< Flag to indicate if 
                                              im_cn_flag should be requested 
                                              for this profile. */

  ds_profile_db_3gpp_inactivity_timer_val_type   inactivity_timer_val; 
                                         /**< Value of the inactivity timer 
                                              in seconds. */
  ds_profile_db_3gpp_src_stat_desc_type           src_stat_desc_req;
                                         /**< Source Statistics Descriptor 
                                              for requested UMTS QoS */
  ds_profile_db_3gpp_src_stat_desc_type                        src_stat_desc_min;
                                         /**< Source Statistics Descriptor 
                                              for minimum UMTS QoS */
  ds_profile_db_3gpp_apn_bearer_type                          apn_bearer;
                                         /**< Flag used to determine APN 
                                              bearer type. */

  ds_profile_db_3gpp_emergency_calls_are_supported  emergency_calls_are_supported;
                                           /* Flag to indicate if IMS emergency
                                              calls can be made using this profile */
  uint16                                max_pdn_conn_per_blk;
                                        /* Indicates max PDN Connections 
                                           that can be made per Time Blk */
                                       
  uint16                                pdn_req_wait_time;
                                        /* Specifies the minimum time 
                                           interval (secondss) between
                                           the new PDN connection request
                                           and the last successful UE
                                           initiated PDN disconnection. */
                                       
  uint16                                max_pdn_conn_time;   
                                        /* Specifies the time duration 
                                           in seconds during which the UE 
                                           counts the PDN connections
                                           already made. */ 
  uint32                                user_app_data;  
                                        /* This is a 4 byte value reserved for user/application
                                         usage*/  
  boolean                        roaming_disallowed;
                                        /* This flag specifies whether the UE 
                                                is allowed to connect to the APN 
                                                specified by the profile while on 
                                                Roaming*/                                                                             
  uint8                          pdn_discon_wait_time;
                                        /* Time in minutes, given to the client
                                               application to deregister with the
                                               network before tearing down the
                                               PDN connection */
  ds_profile_db_3gpp_roaming_pdp_protocol lte_pdp_type;
                                          /* LTE Roaming PDP type (IP). */

  ds_profile_db_3gpp_roaming_pdp_protocol umts_pdp_type;
                                          /* UMTS Roaming PDP type (IP). */
  boolean                               roaming_ho_iwlan_to_lte;
                                        /* This flag specifies whether the handoff
                                           is allowed from IWLAN to LTE
                                           while on roaming. */   
  boolean                               roaming_ho_lte_to_iwlan;
                                        /* This flag specifies whether the handoff
                                           is allowed from LTE to IWLAN
                                         while on roaming.*/   
  ds_profile_db_3gpp_failure_timers     failure_timers;
                                           /*PDN Throttling Timers */ 
  ds_profile_db_3gpp_override_home_pdp_protocol override_home_ip_type;
                                        /* Override PDP type.*/

} ds_profile_db_3gpp_tech_params_type;
/*===========================================================================*/

/* Params that are 3gpp specific */
typedef PACKED struct PACKED_POST 
{
  ds_profile_db_common_tech_params_type tech_common_params;

  ds_profile_db_3gpp2_profile_subtype profile_type;
                                    
  /* Data profile specific (common across all the builds) */
  /* ON or OFF default ON */
  ds_profile_db_3gpp2_negotiate_dns_server_type       negotiate_dns_server; 
  /* session close timer values need to be specified in sec */
  ds_profile_db_3gpp2_ppp_session_close_timer_DO_type ppp_session_close_timer_DO;
  ds_profile_db_3gpp2_ppp_session_close_timer_1X_type ppp_session_close_timer_1X;

  /* Expose only to KDDI though it is common to all carriers */
  /* Timeout values need to be specified in msec*/
  /* lcp C-Req Retry Timer */
  ds_profile_db_3gpp2_lcp_ack_timeout_type       lcp_ack_timeout; 
  /* ipcp C-Req Retry Timer */
  ds_profile_db_3gpp2_ipcp_ack_timeout_type      ipcp_ack_timeout; 
  /* PAP or CHAP Retry Timer */
  ds_profile_db_3gpp2_auth_timeout_type          auth_timeout; 
                                      
  /* number of retries */
  ds_profile_db_3gpp2_lcp_creq_retry_count_type  lcp_creq_retry_count; 
  ds_profile_db_3gpp2_ipcp_creq_retry_count_type ipcp_creq_retry_count; 
  ds_profile_db_3gpp2_auth_retry_count_type      auth_retry_count; 
                                      
  /* high, medium or low */
  ds_profile_db_3gpp2_data_rate_type             data_rate; 
  /* hybrid or 1x */
  ds_profile_db_3gpp2_data_mode_type             data_mode; 
                                      
  /* OMH related params */            
  ds_profile_db_3gpp2_app_type_type              app_type;
  ds_profile_db_3gpp2_app_priority_type          app_priority;
                                      
  /* eHRPD related params */          
  char                                 pdn_label[DS_PROFILE_DB_MAX_APN_NAME_LEN + 1];

  ds_profile_db_3gpp2_pdn_type   pdn_type;
  ds_profile_db_3gpp2_rat_type   rat_type;

  ds_profile_db_3gpp2_pdn_inactivity_timeout     pdn_inactivity_timeout;
  ds_profile_db_3gpp2_failure_timer_type failure_timer[DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS];
  ds_profile_db_3gpp2_disallow_timer_type disallow_timer[DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS];

  /* PDN level authentication parameters */
  ds_profile_db_auth_info pdn_level_auth_info;
  
} ds_profile_db_3gpp2_tech_params_type;

typedef struct ds_profile_db_common_params_map
{
  ds_profile_db_ident_type ds_profile_common_param;
  ds_profile_db_ident_type ds_profile_3gpp_param;
  ds_profile_db_ident_type ds_profile_3gpp2_param;
} ds_profile_db_common_params_map_type;


/*==========================================================================*/

/*  Managed profile type */
typedef void* ds_profile_db_profile_managed_type;

typedef PACKED struct PACKED_POST{
  ds_profile_db_supported_tech_mask tech_mask;
  ds_profile_db_profile_num_type num;
  uint32 size;
  uint32 num_tlvs;
  uint32 magic_number;
  uint32 time_created;
  uint32 last_modified_time;
  uint32 last_read_time;
  uint16 version;
  uint16 reserved;
}ds_profile_db_efs_header;


typedef struct {
  uint16 len; 
  void* buf;
}ds_profile_db_param_info;

#define MAX_TLV_SIZE (sizeof(ds_profile_db_efs_header) + \
                      sizeof(ds_profile_db_3gpp2_tech_params_type) + \
                      sizeof(ds_profile_db_3gpp_tech_params_type) + \
                      sizeof(ds_profile_db_common_tech_params_type)+ \
                      sizeof(ds_profile_db_gen_params_type) + \
                      2*sizeof(uint16)*((COMMON_TECH_PARAM_MAX - COMMON_TECH_PARAM_MIN) + \
                                     (TECH_3GPP_PROFILE_PARAM_MAX - TECH_3GPP_PROFILE_PARAM_MIN) + \
                                     (TECH_3GPP2_PROFILE_PARAM_MAX - TECH_3GPP2_PROFILE_PARAM_MIN) + \
                                     (GENERAL_PROFILE_PARAM_MAX - GENERAL_PROFILE_PARAM_MIN)))

#define MAX_3GPP_PROFILE_SIZE (sizeof(ds_profile_db_efs_header) + \
                               sizeof(ds_profile_db_3gpp_tech_params_type) + \
                               sizeof(ds_profile_db_gen_params_type) + \
                               2*sizeof(uint16)*((TECH_3GPP_PROFILE_PARAM_MAX - TECH_3GPP_PROFILE_PARAM_MIN) + \
                               (GENERAL_PROFILE_PARAM_MAX - GENERAL_PROFILE_PARAM_MIN)))

#define MAX_3GPP2_PROFILE_SIZE (sizeof(ds_profile_db_efs_header) + \
                               sizeof(ds_profile_db_3gpp2_tech_params_type) + \
                               sizeof(ds_profile_db_gen_params_type) + \
                               2*sizeof(uint16)*((TECH_3GPP2_PROFILE_PARAM_MAX - TECH_3GPP2_PROFILE_PARAM_MIN) + \
                               (GENERAL_PROFILE_PARAM_MAX - GENERAL_PROFILE_PARAM_MIN)))

#define EFS_HEADER_SIZE sizeof(ds_profile_db_efs_header)

#define DS_PROFILE_DB_IDENT_SIZE 2

#define DS_PROFILE_DB_PARAM_LEN_SIZE  2 

#define DS_PROFILE_DB_MAGIC_EFS_NUM 0xA5A5A5A5

#define MAX_DATA_SIZE 150

#define EPC_TECH_MASK                 0x07

typedef uint16 TLV_LEN;

struct ds_profile_db_delete_entry{
  ds_profile_db_supported_tech_mask tech_mask;
  ds_profile_db_profile_num_type num;
  char apn_name[DS_PROFILE_DB_MAX_APN_NAME_LEN+1];
  struct ds_profile_db_delete_entry* next;
}; 

/* node in the list of all valid client id's */
struct ds_profile_db_callback_id_node_type {
  ds_profile_db_callback_id_type callback_id; /* should be first member */
  void* callback_data;
};


typedef struct ds_profile_db_delete_entry ds_profile_db_deleted_profile_entry;

/*===========================================================================
   Function declarations
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#include "ps_sys.h"
#include "mcfg_fs.h"
#define DB_TECH_TO_TECHMASK(tech) (1 << (tech))
#define DB_MAX_SUPPORTED_TECHMASK (DB_TECH_TO_TECHMASK(DB_TECH_MAX)-1)

/* returns true if tech_mask_subset is a subset of tech_mask
   for example, the following will return true :
   DB_IS_SUBSET_OF_TECHMASK(DB_TECH_TO_TECHMASK(TECH_3GPP), 
                            DB_TECH_TO_TECHMASK(TECH_3GPP) | DB_TECH_TO_TECHMASK(TECH_3GPP2))
*/


#define DB_IS_SUBSET_OF_TECHMASK(tech_mask_subset, tech_mask) \
        (((tech_mask) | (tech_mask_subset)) == (tech_mask))

/* example of usage :

  ds_profile_db_supported_tech_mask tech_mask;
  ds_profile_db_3gpp_tech_params_type tech_params_3gpp;
  ds_profile_db_3gpp_tech_params_type tech_params_3gpp2;

  tech_mask = ds_profile_db_get_supported_tech_mask(profile);

  DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)
  switch (tech) {
   case(DB_TECH_3GPP):
    ds_profile_db_get_tech_params(profile, tech, tech_params_3gpp, NULL);
    break;
   case(DB_TECH_3GPP2):
    ds_profile_db_get_tech_params(profile, tech, tech_params_3gpp2, NULL);
    break;
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END


  Note, that you do not need to declare "tech", you just provide the variable name, it 
  is declared for you 
*/  
#define DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)  \
  { ds_profile_db_supported_tech_mask __tech_mask = tech_mask; \
    ds_profile_db_tech_type tech; \
    for(int __tech = 0; __tech_mask ; __tech++, __tech_mask >>= 1) {   \
      if (!(__tech_mask % 2)) {\
        continue;\
      } \
      tech = (ds_profile_db_tech_type)__tech;
#define DB_FOR_EVERY_TECH_IN_MASK_DO_END  } }

ds_profile_db_profile_managed_type* ds_profile_db_alloc(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs
);

ds_profile_db_profile_managed_type* ds_profile_db_alloc_copy(
  ds_profile_db_profile_managed_type* profile
);


ds_profile_db_result_type ds_profile_db_dealloc(
  ds_profile_db_profile_managed_type* profile
);

ds_profile_db_result_type ds_profile_db_set_param( 
  ds_profile_db_profile_managed_type *profile,
  ds_profile_db_ident_type            ident,
  const ds_profile_db_info_type       *info
);

ds_profile_db_result_type ds_profile_db_get_param( 
  ds_profile_db_profile_managed_type *profile,
  ds_profile_db_ident_type            ident,
  ds_profile_db_info_type             *info
);

ds_profile_db_result_type ds_profile_db_get_tech_params(
  const ds_profile_db_profile_managed_type* profile,
  ds_profile_db_tech_type                   tech,
  void*                                     tech_params, /* can be NULL */
  ds_profile_db_gen_params_type*            gen_params /* can be NULL */
);

ds_profile_db_profile_num_type ds_profile_db_get_profile_num(
  const ds_profile_db_profile_managed_type *profile
);

boolean ds_profile_db_is_profile_persistent(
  const ds_profile_db_profile_managed_type *profile
);

ds_profile_db_supported_tech_mask ds_profile_db_get_supported_tech_mask(
 const ds_profile_db_profile_managed_type *profile
);
                               
ds_profile_db_result_type ds_profile_db_get_changed_idents(
  const ds_profile_db_profile_managed_type* profile,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type** idents, /* can be NULL */
  uint16* idents_count, /* can be NULL */
  ds_profile_db_ident_type** idents_gen, /* can be NULL */
  uint16* idents_count_gen /* can be NULL */
);

void ds_profile_db_free_idents(ds_profile_db_ident_type* idents);

ds_profile_db_result_type ds_profile_db_init_creation_params(
  ds_profile_db_creation_params_type* params
);

ds_profile_db_result_type ds_profile_db_create(
  const ds_profile_db_creation_params_type* params,
  ds_profile_db_profile_managed_type* profile
);

ds_profile_db_result_type ds_profile_db_delete(
  ds_profile_db_profile_num_type  num,
  ds_profile_db_supported_tech_mask tech_mask,
  boolean enable_deleting_default_profile,
  ds_profile_db_subs_type subs
);

ds_profile_db_result_type ds_profile_db_write(
  const ds_profile_db_profile_managed_type* profile
);

ds_profile_db_result_type ds_profile_db_read(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  ds_profile_db_profile_managed_type** profile
);

ds_profile_db_result_type ds_profile_db_get_default_profile_num(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family
);

ds_profile_db_result_type ds_profile_db_set_default_profile_num(
  ds_profile_db_profile_num_type num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family
);

ds_profile_db_result_type ds_profile_db_get_default_profile_num_per_subs(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs
);

ds_profile_db_result_type ds_profile_db_set_default_profile_num_per_subs(
  ds_profile_db_profile_num_type num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs
);
               
ds_profile_db_result_type ds_profile_db_reset(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs
);            

ds_profile_db_result_type ds_profile_db_reset_profile_param(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type  ident,
  ds_profile_db_subs_type subs,
  boolean mark_invalid
);
                
ds_profile_db_result_type ds_profile_db_get_profiles_range(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_profile_num_type *min_num,
  ds_profile_db_profile_num_type *max_num
);

ds_profile_db_result_type ds_profile_db_get_profiles(
  ds_profile_db_supported_tech_mask tech_mask,                  
  ds_profile_db_subs_type subs,                 
  ds_profile_db_profile_managed_type ***profiles,
  uint16* profiles_count
);

ds_profile_db_result_type ds_profile_db_get_profiles_with_ident(
  ds_profile_db_supported_tech_mask tech_mask, 
  ds_profile_db_ident_type ident,
  const ds_profile_db_info_type* param_info,
  ds_profile_db_subs_type subs,
  ds_profile_db_profile_managed_type ***profiles,
  uint16* profiles_count
);

ds_profile_db_result_type ds_profile_db_free_profiles(
  ds_profile_db_profile_managed_type** profiles,
  uint16 profiles_count
);

typedef void (*ds_profile_db_cb_type) (
  ds_profile_db_event_type event,
  ds_profile_db_profile_managed_type** profiles,
  uint16 profile_count,
  void* user_data,
  ds_profile_db_subs_type subs);

ds_profile_db_callback_id_type ds_profile_db_register_for_notifications(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,                  
  ds_profile_db_cb_type cback,
  ds_profile_db_subs_type subs,
  void *user_data
);

ds_profile_db_result_type ds_profile_db_deregister_for_notifications(
  ds_profile_db_callback_id_type callback_id,
  ds_profile_db_subs_type        subs
);

ds_profile_db_tech_type ds_profile_db_get_tech_type(
  ds_profile_db_profile_num_type profile_num,
  ds_profile_db_subs_type subs
);

ds_profile_db_result_type ds_profile_db_get_persistence(
  ds_profile_db_profile_num_type profile_num,
  ds_profile_db_subs_type subs,
  boolean *is_persistent
);

ds_profile_db_tech_type ds_profile_db_tech_mask_to_tech(
   uint16 tech_mask
);

ds_profile_db_result_type ds_profile_db_get_param_in_use( 
  ds_profile_db_profile_managed_type *profile,
  ds_profile_db_ident_type            ident,
  ds_profile_db_info_type             *info,
  ds_profile_db_tech_type             tech,
  ds_profile_db_subs_type             subs
);

ds_profile_db_ident_type ds_profile_db_get_tech_ident_from_common_ident (
  ds_profile_db_ident_type common_ident,
  ds_profile_db_tech_type  tech
);

ds_profile_db_result_type ds_profile_db_is_valid_profile(
  ds_profile_db_profile_num_type  num_type, 
  ds_profile_db_subs_type         db_subs,
  ds_profile_db_tech_type         tech_type,
  boolean                         *is_valid
);

boolean ds_profile_db_get_epc_support_val
(
  void
);

void* ds_profile_db_get_heap_mem_for_file( void );

void ds_profile_db_set_heap_mem_for_file(void *buf);

void ds_profile_add_entry_to_delete_profile_list (ds_profile_db_deleted_profile_entry *deleted_profile);

mcfg_fs_sub_id_e_type ds_profile_db_convert_ds_to_mcfg_subs(ds_profile_db_subs_type subs);


#ifdef __cplusplus
}
#endif

#endif /* DS_PROFILE_DB_H */
