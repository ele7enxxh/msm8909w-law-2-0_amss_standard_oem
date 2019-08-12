/**
@file ds_sys.h
@brief
This file defines function, variables and data structures common to all PS
System API module
*/

#ifndef DS_SYS_H
#define DS_SYS_H

/*===========================================================================

                          D S  _ S Y S . H

DESCRIPTION

  Data Services System module interface file. This contains variables and
  definitions used by the various modules to access interface definitions.

Copyright (c) 2011-15 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_sys.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/06/11    sy     Created module

===========================================================================*/

#include "comdef.h"
#include "dserrno.h"

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/**
  RAT Mask for 3GPP
 */
#define DS_SYS_RAT_3GPP_WCDMA                0x01
#define DS_SYS_RAT_3GPP_GPRS                 0x02
#define DS_SYS_RAT_3GPP_HSDPA                0x04
#define DS_SYS_RAT_3GPP_HSUPA                0x08
#define DS_SYS_RAT_3GPP_EDGE                 0x10
#define DS_SYS_RAT_3GPP_LTE                  0x20
#define DS_SYS_RAT_3GPP_HSDPAPLUS            0x40
#define DS_SYS_RAT_3GPP_DC_HSDPAPLUS         0x80
#define DS_SYS_RAT_3GPP_64_QAM               0x100
#define DS_SYS_RAT_3GPP_TDSCDMA              0x200
#define DS_SYS_RAT_3GPP_NULL_BEARER          0x8000

/**
  RAT Mask for 3GPP2
*/
#define DS_SYS_RAT_3GPP2_1X               0x01
#define DS_SYS_RAT_3GPP2_EVDO_REV0        0x02
#define DS_SYS_RAT_3GPP2_EVDO_REVA        0x04
#define DS_SYS_RAT_3GPP2_EVDO_REVB        0x08
#define DS_SYS_RAT_3GPP2_EHRPD            0x10
#define DS_SYS_RAT_3GPP2_FMC              0x20
#define DS_SYS_RAT_3GPP2_NULL_BEARER      0x8000

/**
  SO Mask for 1x
*/
#define DS_SYS_SO_3GPP2_1X_IS95                0x01
#define DS_SYS_SO_3GPP2_1X_IS2000              0x02
#define DS_SYS_SO_3GPP2_1X_IS2000_REL_A        0x04

/**
  SO Mask for DO

  The following table gives which of the following so_masks are supported
  by which technology

  Rev0 supports @li DS_SYS_SO_3GPP2_EVDO_DPA

  RevA supports @li DS_SYS_SO_3GPP2_EVDO_DPA
                @li DS_SYS_SO_3GPP2_EVDO_MFPA
                @li DS_SYS_SO_3GPP2_EVDO_EMPA
                @li DS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD

  RevB supports @li DS_SYS_SO_3GPP2_EVDO_DPA
                @li DS_SYS_SO_3GPP2_EVDO_MFPA
                @li DS_SYS_SO_3GPP2_EVDO_EMPA
                @li DS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD
                @li DS_SYS_SO_3GPP2_EVDO_MMPA
                @li DS_SYS_SO_3GPP2_EVDO_MMPA_EHRPD

*/
#define DS_SYS_SO_3GPP2_EVDO_DPA           0x01
#define DS_SYS_SO_3GPP2_EVDO_MFPA          0x02
#define DS_SYS_SO_3GPP2_EVDO_EMPA          0x04
#define DS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD    0x08
#define DS_SYS_SO_3GPP2_EVDO_MMPA          0x10
#define DS_SYS_SO_3GPP2_EVDO_MMPA_EHRPD    0x20

/**
   Defines the MAX length for PDN/APN name string
*/
#define DS_SYS_MAX_APN_LEN        101
#define DS_SYS_MAX_APNS           15 /**< Max number of APNs */
#define DS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX  56 
/**< Max number of LTE Attach PDN profile IDs */


#define DS_SYS_MAX_NUM_THROUGHPUT_INFO        18
/**< max_thrpt_info = num_pdn * num_ifaces_per_pdn 
     Currently we have 8PDNs + 1 emergency PDN,(total 9PDNs)
     and max ifaces per PDN = 2 */

#define DS_SYS_MAX_BEARER         8

#define DS_SYS_MAX_AVAIL_SYS      15

#define DS_SYS_MAC_ADDR_LEN       6

#define DS_SYS_MAX_SSID_LEN       33

#define DS_SYS_MAX_WQE_PROFILES   32

/**
  @brief Technology type values
*/
typedef enum
{
  DS_SYS_TECH_MIN     = 0x00,
  DS_SYS_TECH_3GPP    = DS_SYS_TECH_MIN, /**< Tech type 3gpp */
  DS_SYS_TECH_3GPP2   = 0x01,            /**< Tech type 3gpp2 */
  DS_SYS_TECH_WLAN    = 0x02,            /**< Tech type WLAN */
  DS_SYS_TECH_ALL     = 0x03,            /**< All the tech types */
  DS_SYS_TECH_MAX     = 0x04,
  DS_SYS_TECH_INVALID = 0xFF
} ds_sys_tech_enum_type;

/**
  @brief This is the type that is used to different network types
*/
typedef enum
{
  DS_SYS_NETWORK_3GPP,     /**< Network type 3gpp */
  DS_SYS_NETWORK_3GPP2,    /**< Network type 3gpp2 */
  DS_SYS_NETWORK_WLAN,     /**< Network type WLAN */
  DS_SYS_NETWORK_MAX,
} ds_sys_network_enum_type;

/**
  @brief This is the type that is used to define core network details
*/
typedef struct
{
  uint32   rat_mask;  /**< RAT Mask */
  uint32   so_mask;   /**< SO Mask */
} ds_sys_network_info_type;

/**
  @brief This is the type that is used to define system status. It is
         common to both configuration DS_SYS_CONF_SYSTEM_STATUS and event
         DS_SYS_EVENT_SYSTEM_STATUS_CHANGE

  @see   DS_SYS_CONF_SYSTEM_STATUS
  @see   DS_SYS_EVENT_SYSTEM_STATUS_CHANGE
*/
typedef struct
{
  ds_sys_network_enum_type         preferred_network; /**< pref network value*/
  ds_sys_network_info_type         network_info[DS_SYS_NETWORK_MAX];
  /**< Array of rat and SO mask for all networks */
} ds_sys_system_status_type;


/**
  @brief This is the type that is used for 
         DS_SYS_CONF_GET_WLAN_MAC_ADDR configuration and
         DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE
   
  @see DS_SYS_CONF_GET_WLAN_MAC_ADDR
  @see DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE 
*/
typedef struct
{
  uint8            ap_mac_addr[DS_SYS_MAC_ADDR_LEN]; 
  /**< WLAN Access Point MAC addr*/
} ds_sys_wlan_mac_addr_type;

/**
  @brief This is the type that is used for 
         DS_SYS_CONF_GET_WLAN_SSID configuration and
         DS_SYS_EVENT_WLAN_SSID_CHANGE
   
  @see DS_SYS_CONF_GET_WLAN_SSID
  @see DS_SYS_EVENT_WLAN_SSID_CHANGE 
*/
typedef struct
{
  char            ssid[DS_SYS_MAX_SSID_LEN]; 
  /**< WLAN Service Set Identifier*/
} ds_sys_wlan_ssid_type;

/**
  @brief Data type used in ds_sys_3gpp2_page_monitor_type
  @see   ds_sys_3gpp2_page_monitor_type
*/
typedef uint8 ds_sys_3gpp2_page_monitor_period_type;

/**
  @brief Data type to be used with get operation for
         DS_SYS_CONF_3GPP2_PAGE_MONITOR_PERIOD and
         DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE event

  @see   DS_SYS_CONF_3GPP2_PAGE_MONITOR_PERIOD
  @see   DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE
*/
typedef struct
{
  ds_sys_3gpp2_page_monitor_period_type    page_monitor_period;
  boolean                                  force_long_sleep;
} ds_sys_3gpp2_page_monitor_type;


/**
  @brief This is the type that is used to  define different RATs types
*/
typedef enum
{
  DS_SYS_RAT_EX_NULL_BEARER           = 0x0,
  /* 3GPP RAT Values */
  DS_SYS_RAT_EX_3GPP_WCDMA             = 1,     
  DS_SYS_RAT_EX_3GPP_GERAN             = 2,
  DS_SYS_RAT_EX_3GPP_LTE               = 3,
  DS_SYS_RAT_EX_3GPP_TDSCDMA           = 4,
  DS_SYS_RAT_EX_3GPP_WLAN              = 5,
  DS_SYS_RAT_EX_3GPP_MAX               = 100,

  /* 3GPP2 RAT Values */
  DS_SYS_RAT_EX_3GPP2_1X               = 101,
  DS_SYS_RAT_EX_3GPP2_HRPD             = 102,
  DS_SYS_RAT_EX_3GPP2_EHRPD            = 103,
  DS_SYS_RAT_EX_3GPP2_WLAN             = 104,
  DS_SYS_RAT_EX_3GPP2_MAX              = 200, 

  /* WLAN RAT Values */
  DS_SYS_RAT_EX_WLAN                   = 201,
  DS_SYS_RAT_EX_WLAN_MAX               = 300,
 
  DS_SYS_RAT_UNKNOWN,
  DS_SYS_RAT_MAX,
} ds_sys_rat_ex_enum_type;

/* SO Mask has a size of 8 bytes */

/* SO Mask not available or not specified */
#define DS_SYS_SO_EX_UNSPECIFIED                    0x0

/* 3GPP SO Mask, it occupies the first 3 bytes. ie. bytes 0, 1 and 2 */
#define DS_SYS_SO_EX_3GPP_WCDMA                     0x01
#define DS_SYS_SO_EX_3GPP_HSDPA                     0x02
#define DS_SYS_SO_EX_3GPP_HSUPA                     0x04
#define DS_SYS_SO_EX_3GPP_HSDPAPLUS                 0x08
#define DS_SYS_SO_EX_3GPP_DC_HSDPAPLUS              0x10
#define DS_SYS_SO_EX_3GPP_64_QAM                    0x20
#define DS_SYS_SO_EX_3GPP_HSPA                      0x40
#define DS_SYS_SO_EX_3GPP_GPRS                      0x80
#define DS_SYS_SO_EX_3GPP_EDGE                      0x100

/* GSM so mask is depreciated. Can only be GPRS or EDGE */
#define DS_SYS_SO_EX_3GPP_GSM                       0x200

#define DS_SYS_SO_EX_3GPP_S2B                       0x400
#define DS_SYS_SO_EX_3GPP_LTE_LIMITED_SRVC          0x800
#define DS_SYS_SO_EX_3GPP_LTE_FDD                   0x1000
#define DS_SYS_SO_EX_3GPP_LTE_TDD                   0x2000
#define DS_SYS_SO_EX_3GPP_TDSCDMA                   0x4000
#define DS_SYS_SO_EX_3GPP_DC_HSUPA                  0x8000
#define DS_SYS_SO_EX_3GPP_LTE_CA_DL                 0x10000
#define DS_SYS_SO_EX_3GPP_LTE_CA_UL                 0x20000

/* 3GPP2 SO Mask, it occupies bytes 3, 4 and 5 */
#define DS_SYS_SO_EX_3GPP2_1X_IS95                  (uint64)0x01000000ull
#define DS_SYS_SO_EX_3GPP2_1X_IS2000                (uint64)0x02000000ull
#define DS_SYS_SO_EX_3GPP2_1X_IS2000_REL_A          (uint64)0x04000000ull
#define DS_SYS_SO_EX_3GPP2_HDR_REV0_DPA             (uint64)0x08000000ull
#define DS_SYS_SO_EX_3GPP2_HDR_REVA_DPA             (uint64)0x10000000ull
#define DS_SYS_SO_EX_3GPP2_HDR_REVB_DPA             (uint64)0x20000000ull
#define DS_SYS_SO_EX_3GPP2_HDR_REVA_MPA             (uint64)0x40000000ull
#define DS_SYS_SO_EX_3GPP2_HDR_REVB_MPA             (uint64)0x80000000ull
#define DS_SYS_SO_EX_3GPP2_HDR_REVA_EMPA            (uint64)0x100000000ull
#define DS_SYS_SO_EX_3GPP2_HDR_REVB_EMPA            (uint64)0x200000000ull
#define DS_SYS_SO_EX_3GPP2_HDR_REVB_MMPA            (uint64)0x400000000ull
#define DS_SYS_SO_EX_3GPP2_EVDO_FMC                 (uint64)0x800000000ull
#define DS_SYS_SO_EX_3GPP2_1X_CS                    (uint64)0x1000000000ull

/* Bytes 6 and 7 of SO Mask are reserved for future use */


/**
  @brief This is the type that is used to  define different subscription types
         Unknown subs id is used when the client does not care/provide the subs id
*/
typedef enum 
{
  DS_SYS_DEFAULT_SUBS      = 0x0000, /** < Default Data Subscription */
  DS_SYS_PRIMARY_SUBS      = 0x0001, /**< Primary */
  DS_SYS_SECONDARY_SUBS    = 0x0002, /**< Secondary */
  DS_SYS_TERTIARY_SUBS     = 0x0003,  /**< Tertiary */
  DS_SYS_SUBS_MAX,
} ds_sys_subscription_enum_type;

/**
  @brief This is the tuple that defines network, RAT, SO and service status
*/
typedef struct 
{
  ds_sys_network_enum_type                 technology; /**< technology type*/
  ds_sys_rat_ex_enum_type                  rat_value;  /**< rat type*/
  uint64                                   so_mask;   /**< so type*/
} ds_sys_system_status_info_type;

/**
  @brief  This is the type that defines apn to preferred network relationship. 
          For every APN, there would be tuple that describes its preferred 
          system. 
*/
typedef struct 
{
   char                                apn_name[DS_SYS_MAX_APN_LEN]; /**< PDN name*/
   uint32                              num_avail_sys; 
  /**< Number of valid entries to available system status array */
   ds_sys_system_status_info_type      avail_sys[DS_SYS_MAX_AVAIL_SYS];
  /**<  Array of available systems for the APN. The first element of the 
        array will contain the current preferred system for the APN.
        @see ds_sys_system_status_info_type 
        @see ds_sys_system_status_info_type */
   uint8                          recommended_pref_system_index;
  /**< Index in avail_sys array, which contains the 
       recommended preferred system for the APN */
} ds_sys_apn_pref_sys_info_type;

/**
  @brief This is the type that is used to define system status. It is
         common to both configuration DS_SYS_CONF_SYSTEM_STATUS_EX and event
         DS_SYS_EVENT_SYSTEM_STATUS_EX
*/
typedef struct 
{
  uint32   num_avail_sys; 
  /**< Number of valid entries to available system status array */

  ds_sys_system_status_info_type  avail_sys[DS_SYS_MAX_AVAIL_SYS]; 
  /**< Array of current system status, the first element in the array will 
       contain the current preferred system */

  uint32   num_avail_apns;
  /**< Number of apn entries to pdn_info */

  ds_sys_apn_pref_sys_info_type apn_pref_sys_info[DS_SYS_MAX_APNS];    
  /**< Array of all apns available */

  uint8                          recommended_pref_system_index;
  /**< Index in avail_sys array for the APN, which contains the 
       recommended preferred system for the APN */
} ds_sys_system_status_ex_type;


/**
  @brief Enum type describing APN Preferred system.
  @see   ds_sys_conf_apn_pref_sys_type
*/
typedef enum
{
  DS_SYS_APN_PREF_SYS_WWAN  = 0,
  DS_SYS_APN_PREF_SYS_WLAN  = 1,
  DS_SYS_APN_PREF_SYS_IWLAN = 2
} ds_sys_apn_pref_sys_enum_type;

/**
  @brief Data type to be used with DS_SYS_CONF_APN_PREF_SYS and 
         DS_SYS_EVENT_APN_PREF_SYS_CHANGE. It describes preferred 
         system for apn.         
*/
typedef struct
{
  unsigned char                  apn[DS_SYS_MAX_APN_LEN];
  ds_sys_apn_pref_sys_enum_type  pref_sys;
} ds_sys_apn_pref_sys_type;

/**
  @brief Data type to be used with attach pdn list. 
   
  @see   DS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST 
  @see   DS_SYS_IOCTL_3GPP_GET_LTE_ATTACH_PDN_LIST 
  @see   DS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE  
*/
typedef struct
{
  uint8  attach_pdn_profile_list_num;   /**< Num of attach PDN in the array */
  uint16 attach_pdn_profile_list[DS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX]; 
    /**< Array of attach PDN profile IDs*/
} ds_sys_3gpp_lte_attach_pdn_list_type;

//Ip addr type enum
typedef enum
{
  DS_SYS_IP_TYPE_V4 = 0x00,
  DS_SYS_IP_TYPE_V6 = 0x01
} ds_sys_ip_addr_enum_type;

// Throughput quality enum
typedef enum {
  DS_SYS_HIGHER_THROUGHPUT_UNKNOWN,
  /**< Throughput quality is unknown \n */
  DS_SYS_HIGHER_THROUGHPUT_NOT_POSSIBLE,
  /**< Best throughput possible \n */
  DS_SYS_HIGHER_THROUGHPUT_POSSIBLE
  /**< Better throughput than current throughput is possible \n */
} ds_sys_throughput_quality_enum_type;

/**
  @brief Data type to be used to represent per bearer
         throughput info
*/
typedef struct
{
  uint32 bearer_rlp_mac_id;
  /**< Bearer ID representing the Bearer
       for which the throughput is being 
       reported in case of 3gpp tech
  */

  uint32 bearer_uplink_actual_rate;
  /**<  Uplink actual rate in bits per second
        corresponding to the bearer id 
  */

  uint32 bearer_uplink_queue_size;
  /**< Number of bytes pending in uplink queue
       corresponding to the bearer id 
  */
  
  boolean is_default_bearer;
  /**< Boolean to determine if the bearer
       is a default one 
  */ 
} ds_sys_bearer_throughput_info_type;

typedef struct
{
  uint8 num_bearers; 
  /**< Num of bearer trpt info in the array */

  ds_sys_bearer_throughput_info_type bearer_trpt_info[DS_SYS_MAX_BEARER];
  /**< Array of per bearer throughput info*/
} ds_sys_bearer_throughput_info_list_type;

/**
  @brief Data type to be used with throughput info timer 
   
  @see   DS_SYS_CONF_THROUGHPUT_INFO
  @see   DS_SYS_EVENT_THROUGHPUT_INFO  
*/
typedef struct
{
  char apn_string[DS_SYS_MAX_APN_LEN];
  /**< String representing the APN.
       Maximum length is 100 bytes.
  */

  ds_sys_ip_addr_enum_type ip_type;
  /**< Values: \n
       - 4 -- IPv4 \n
       - 6 -- IPv6 
  */

  ds_sys_tech_enum_type  tech_type;
  /**< Technology type */

  ds_sys_subscription_enum_type subscription;
  /**< Subscription that APN is bound to.
  */

  uint32 uplink_actual_rate;
  /**<  Uplink actual rate(kbps)
  */

  uint32 uplink_allowed_rate;
  /**<  Uplink allowed rate(kbps)
  */

  uint32 uplink_queue_size;
  /**< Number of bytes pending in uplink queue
  */

  ds_sys_throughput_quality_enum_type throughput_quality;
  /**< Indicates whether UE can have better throughput 
       rate than the current throughput reporting
  */  

  void *iface_ptr;
  /**< ptr to physical interface on which to 
       operate on.(Um interface representing the APN)
  */  

  ds_sys_bearer_throughput_info_list_type bearer_throughput_info_list;
  /**< list of per bearer throughput information
  */

} ds_sys_throughput_info_type;

typedef struct
{
   uint8 num_trpt_info; 
   /**< Num of trpt info in the list */

   ds_sys_throughput_info_type trpt_info[1];
   /**< Pointer to throughput info list. Can have more than one info in 
        the list */
} ds_sys_throughput_info_list_type;

/**
  @brief Data type to be used with DS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS and 
         DS_SYS_EVENT_3GPP_EMERGENCY_PDN_STATUS. 
         TRUE : UE is attached to emergency PDN only.
         FALSE: UE is attached to other PDN(could be emergency pdn + 
                non-emergency pdn or non-emergency pdn)       
*/
typedef struct
{
  boolean emergency_pdn_only;
} ds_sys_is_pdn_only_emergency_type;

/**
  @brief Data type to store the apn name 
   
*/
typedef struct
{
  unsigned char     apn_name[DS_SYS_MAX_APN_LEN];
} ds_sys_apn_name_type;

/**
  @brief Data type to be used with roaming disallowed info
  is_roaming_disallowed : whether roaming is allowed or disallowed
                          on the apns
  num_apn_names         : No of apn names listed
  apn_list              : List of apn names 
   
  @see   DS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO
  @see   DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO  
*/
typedef struct
{
  boolean                 is_roaming_disallowed;
  uint8                   num_apn_names;
  ds_sys_apn_name_type    apn_list[DS_SYS_MAX_APNS];
} ds_sys_roaming_disallowed_info_type;

/**
  @brief Data type to be used with network aware status
   
  @see   DS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS
  @see   DS_SYS_EVENT_3GPP2_QOS_NETWORK_STATUS_CHANGE  
*/
typedef boolean ds_sys_3gpp2_network_qos_aware_status;

/**
  @brief Data type to be used with page monitor period
   
  @see   DS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD
  @see   DS_SYS_EVENT_3GPP2_PAGE_MONITOR_PERIOD_CHANGE
 */
typedef struct
{
  ds_sys_3gpp2_page_monitor_period_type       slotted_mode_option;
  boolean     long_sleep_enabled;
} ds_sys_3gpp2_page_monitor_period_info_type;

/**
  @brief Data type to be used with downlink throughput info 
         timer
   
  @see   DS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER
  @see   DS_SYS_EVENT_UPDATED_DL_THROUGHPUT_TIMER
*/
typedef uint32 ds_sys_report_dl_timer_type;

/**
  @brief Data type to be used with downlink throughput info
   
  @see   DS_SYS_CONF_DL_THROUGHPUT_INFO
  @see   DS_SYS_EVENT_DL_THROUGHPUT_INFO
*/
typedef struct
{
  uint32   downlink_allowed_rate;
  uint8    confidence_level;
} ds_sys_dl_throughput_info_type;

/**
  @brief Data type to be used with thermal level mitigation info
   
  @see   DS_SYS_CONF_THERMAL_MITIGATION_INFO
  @see   DS_SYS_EVENT_THERMAL_MITIGATION_INFO
 */
typedef enum
{
  DS_SYS_THERMAL_MITIGATION_ALL_CALLS_ALLOWED = 0,
  DS_SYS_THERMAL_MITIGATION_IMS_CALLS_ONLY    = 1,
  DS_SYS_THERMAL_MITIGATION_NO_CALLS_ALLOWED  = 2
} ds_sys_thermal_mitigation_action_enum_type;

/**
  @brief Data type to be used with data service
   
  @see   DS_SYS_EVENT_DATA_SERVICE_SETTING
 */
typedef boolean ds_sys_data_services_setting_type;

/**
  @brief Data type to be used with data service roaming

  @see   DS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING
 */
typedef boolean ds_sys_data_services_roaming_setting_type;

/**
  @brief This is the type that is used to different apn types
*/
typedef enum
{
  DS_SYS_APN_TYPE_INTERNET     = 0,  /**< Internet APN */
  DS_SYS_APN_TYPE_IMS          = 1,   /**< IMS APN */
  DS_SYS_APN_TYPE_MAX
} ds_sys_apn_enum_type;

/**
  @brief Data type to store the apn info 
  @see   DS_SYS_EVENT_APN_INFO
   
*/
typedef struct
{
  boolean   is_configured;
  /**< TRUE- if explicitly configured by apps; 
       FALSE - if default setting is used */
  ds_sys_apn_enum_type apn_type;
  char        apn_name[DS_SYS_MAX_APN_LEN];
} ds_sys_apn_params_type;

typedef struct
{
  ds_sys_apn_params_type   curr_apn_info[DS_SYS_APN_TYPE_MAX];
  /**< Current APN list indexed by apn_type*/
  ds_sys_apn_params_type   prev_apn_info[DS_SYS_APN_TYPE_MAX];
  /**< Previous APN list indexed by apn_type*/
} ds_sys_apn_info_type;

/**
  @brief Data type to be used with 
         DS_SYS_IOCTL_SET_WQE_PROFILE_TYPE and event payload for
         the DS_SYS_EVENT_WQE_PROFILES_MET

*/
#define DS_SYS_WQE_PROFILE_INACTIVE            0x0000000000000000
  /**< WiFi Quality Estimation profile inactive */
#define DS_SYS_WQE_PROFILE_INTERNET            0x0000000000000001
  /**< WiFi Quality Estimation profile internet */
#define DS_SYS_WQE_PROFILE_AUDIO               0x0000000000000002
  /**< WiFi Quality Estimation profile audio */
#define DS_SYS_WQE_PROFILE_VIDEO               0x0000000000000004
  /**< WiFi Quality Estimation profile video */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_1          0x0000000100000000
  /**< WiFi Quality Estimation profile IMS 1 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_2          0x0000000200000000
  /**< WiFi Quality Estimation profile IMS 2 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_3          0x0000000400000000
  /**< WiFi Quality Estimation profile IMS 3 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_4          0x0000000800000000
  /**< WiFi Quality Estimation profile IMS 4 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_5          0x0000001000000000
  /**< WiFi Quality Estimation profile IMS 5 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_6          0x0000002000000000
  /**< WiFi Quality Estimation profile IMS 6 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_7          0x0000004000000000
  /**< WiFi Quality Estimation profile IMS 7 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_8          0x0000008000000000
  /**< WiFi Quality Estimation profile IMS 8 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_9          0x0000010000000000
  /**< WiFi Quality Estimation profile IMS 9 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_10         0x0000020000000000
  /**< WiFi Quality Estimation profile IMS 10 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_11         0x0000040000000000
  /**< WiFi Quality Estimation profile IMS 11 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_12         0x0000080000000000
  /**< WiFi Quality Estimation profile IMS 12 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_13         0x0000100000000000
  /**< WiFi Quality Estimation profile IMS 13 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_14         0x0000200000000000
  /**< WiFi Quality Estimation profile IMS 14 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_15         0x0000400000000000
  /**< WiFi Quality Estimation profile IMS 15 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_16         0x0000800000000000
  /**< WiFi Quality Estimation profile IMS 16 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_17         0x0001000000000000
  /**< WiFi Quality Estimation profile IMS 17 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_18         0x0002000000000000
  /**< WiFi Quality Estimation profile IMS 18 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_19         0x0004000000000000
  /**< WiFi Quality Estimation profile IMS 19 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_20         0x0008000000000000
  /**< WiFi Quality Estimation profile IMS 20 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_21         0x0010000000000000
  /**< WiFi Quality Estimation profile IMS 21 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_22         0x0020000000000000
  /**< WiFi Quality Estimation profile IMS 22 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_23         0x0040000000000000
  /**< WiFi Quality Estimation profile IMS 23 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_24         0x0080000000000000
  /**< WiFi Quality Estimation profile IMS 24 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_25         0x0100000000000000
  /**< WiFi Quality Estimation profile IMS 25 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_26         0x0200000000000000
  /**< WiFi Quality Estimation profile IMS 26 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_27         0x0400000000000000
  /**< WiFi Quality Estimation profile IMS 27 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_28         0x0800000000000000
  /**< WiFi Quality Estimation profile IMS 28 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_29         0x1000000000000000
  /**< WiFi Quality Estimation profile IMS 29 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_30         0x2000000000000000
  /**< WiFi Quality Estimation profile IMS 30 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_31         0x4000000000000000
  /**< WiFi Quality Estimation profile IMS 31 */
#define DS_SYS_WQE_PROFILE_IMS_TYPE_32         0x8000000000000000
  /**< WiFi Quality Estimation profile IMS 32 */


typedef uint64 ds_sys_wqe_profile_type;

/**
  @brief Data type to be used with wifi switch settings
   
  @see  DS_SYS_CONF_DATA_SERVICE_WIFI_SETTING 
  @see   DS_SYS_EVENT_DATA_SERVICE_WIFI_SETTING
 */
typedef boolean ds_sys_data_services_wifi_setting_type;

/**
  @brief Data type to be used with 
         DS_SYS_IOCTL_SET_WQE_PROFILE_QUALITY_MEAS_INFO
*/

/* Wifi RSSI Threshold Type */
typedef struct 
{
  boolean is_rssi_high_valid;
  /**< Valid RSSI High threshold */
  int16 rssi_high;
  /**< RSSI high threshold value for the channel*/
  boolean is_rssi_low_valid;
  /**< Valid RSSI low threshold */
  int16 rssi_low;
  /**< RSSI low  threshold value for the channel*/
} ds_sys_wifi_ident_rssi_threshold_type;


typedef struct
{
   ds_sys_wqe_profile_type wqe_profile;
   /**< Particular WQE profile for which threshold has to be configured. 
   */

   ds_sys_wifi_ident_rssi_threshold_type rssi_threshold;
   /**< RSSI threshold for the wqe profile. */

} ds_sys_wqe_profile_quality_meas_info_type;

typedef struct
{
  int num_wqe_profiles;
  /**< Particular WQE Profiles for which information is being provided */
  ds_sys_wqe_profile_quality_meas_info_type meas_info[DS_SYS_MAX_WQE_PROFILES];
  /**< Info per WQE profile. */
} ds_sys_set_wqe_profile_quality_meas_info_list_type;

/**
 *@brief Data type to be used with 
 *       DS_SYS_EVENT_WQE_PROFILES_STATUS . It describes
 *       status of all the wqe profiles for which
 *       client has registered.
 *       Eg: Status =
 *       DS_SYS_WLAN_WQE_PROFILE_STATUS_QUALITY_NOT_MET
 *       and  status_code =
 *       DS_SYS_WQE_PROFILE_ERROR_CQ_FAIL_INCONCLUSIVE
 *       for a WQE profile X means threshold is in middle
 *       state 
*/

typedef enum
{
  DS_SYS_WLAN_WQE_PROFILE_STATUS_QUALITY_MET = 0x00,
  /**< Quality Met \n*/
  DS_SYS_WLAN_WQE_PROFILE_STATUS_QUALITY_NOT_MET = 0x01,
  /**< Quality Not Met \n*/
  DS_SYS_WLAN_WQE_PROFILE_STATUS_QUALITY_UNKNOWN = 0x03,
  /**< Quality is unknown \n 
       0x02 is reserved */
  DS_SYS_WLAN_WQE_PROFILE_STATUS_QUALITY_INVALID = 0xFF
  /**< Invalid/No status \n */
} ds_sys_wlan_wqe_profile_status_enum_type;


typedef enum
{ 
  DS_SYS_WQE_PROFILE_ERROR_NONE,
  /**< No error \n*/
  DS_SYS_WQE_PROFILE_ERROR_CQ_FAIL_RSSI,
  /**< Rssi threshold not met \n*/
  DS_SYS_WQE_PROFILE_ERROR_CQ_FAIL_MAC,
  /**< MAC Metrics not met \n*/
  DS_SYS_WQE_PROFILE_ERROR_CQ_FAIL_INCONCLUSIVE,
  /**< Could not conclude wlan status \n*/
  DS_SYS_WQE_PROFILE_ERROR_BQ_FAIL,
  /**<  Backhaul quality is degraded. \n*/
  DS_SYS_WQE_PROFILE_ERROR_INTERNET_CONNECTIVITY_FAIL
  /**< Internet Connectivity failure \n*/
} ds_sys_wqe_profile_cause_code_enum_type;

typedef struct
{
  ds_sys_wqe_profile_type wqe_profile;
  /**< Particular WQE profile for which threshold 
    has to be configured. */

  ds_sys_wlan_wqe_profile_status_enum_type wqe_status;
  /**< Enum with wqe status stating the quality*/

  ds_sys_wqe_profile_cause_code_enum_type cause_code;
  /**< Cause code for the wqe status*/
} ds_sys_wlan_set_wqe_profile_status_type;

typedef struct
{
  uint8 num_profiles;
  /**< Specifies the number of profiles */

  ds_sys_wlan_set_wqe_profile_status_type profile_status[DS_SYS_MAX_WQE_PROFILES];
  /**< Profile status array*/
} ds_sys_wqe_profiles_status_type;

/**
  @brief Gives whether the UE is roaming or in the home network.
  roaming_status = TRUE  : UE is roaming
  roaming_status = FALSE : UE is in the home network   
  
  technology : Technology for which roaming status is set/unset
  @see DS_SYS_EVENT_ROAMING_STATUS_CHANGE
  @see DS_SYS_CONF_ROAMING_STATUS_INFO
*/
typedef struct
{
  boolean                  roaming_status;
  ds_sys_network_enum_type technology;
} ds_sys_roaming_status_info_type;

/*===========================================================================

                           EXTERNALIZED FUNCTIONS

===========================================================================*/
/**
  @brief   This function initializes the DS Sys Conf library.

  @param   None

  @return  DSS_SUCCESS          On Success.
  @return  DSS_ERROR            On failure.
*/
int16 ds_sys_init_lib
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* DS_SYS_H */
