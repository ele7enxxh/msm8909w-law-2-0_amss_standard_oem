/**
@file ps_sys.h
@brief
This file defines function, variables and data structures common to all PS 
System API module 
*/

#ifndef PS_SYS_H
#define PS_SYS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S . H


GENERAL DESCRIPTION
  This file defines function, variables and data structures common to all
  PS System API module

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/inc/ps_sys.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
 
when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
03/29/13    svj    New APIs to modify and query attach pdn list 
09/09/11    bvd    Created module 
 
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "ds_sys.h"
#include "ps_in.h"

/**
  RAT Mask for 3GPP
*/
#define PS_SYS_RAT_3GPP_WCDMA                DS_SYS_RAT_3GPP_WCDMA
#define PS_SYS_RAT_3GPP_GPRS                 DS_SYS_RAT_3GPP_GPRS
#define PS_SYS_RAT_3GPP_HSDPA                DS_SYS_RAT_3GPP_HSDPA
#define PS_SYS_RAT_3GPP_HSUPA                DS_SYS_RAT_3GPP_HSUPA
#define PS_SYS_RAT_3GPP_EDGE                 DS_SYS_RAT_3GPP_EDGE
#define PS_SYS_RAT_3GPP_LTE                  DS_SYS_RAT_3GPP_LTE
#define PS_SYS_RAT_3GPP_HSDPAPLUS            DS_SYS_RAT_3GPP_HSDPAPLUS
#define PS_SYS_RAT_3GPP_DC_HSDPAPLUS         DS_SYS_RAT_3GPP_DC_HSDPAPLUS
#define PS_SYS_RAT_3GPP_64_QAM               DS_SYS_RAT_3GPP_64_QAM
#define PS_SYS_RAT_3GPP_TDSCDMA              DS_SYS_RAT_3GPP_TDSCDMA
#define PS_SYS_RAT_3GPP_NULL_BEARER          DS_SYS_RAT_3GPP_NULL_BEARER

/**
  RAT Mask for 3GPP2
*/
#define PS_SYS_RAT_3GPP2_1X               DS_SYS_RAT_3GPP2_1X
#define PS_SYS_RAT_3GPP2_EVDO_REV0        DS_SYS_RAT_3GPP2_EVDO_REV0
#define PS_SYS_RAT_3GPP2_EVDO_REVA        DS_SYS_RAT_3GPP2_EVDO_REVA
#define PS_SYS_RAT_3GPP2_EVDO_REVB        DS_SYS_RAT_3GPP2_EVDO_REVB
#define PS_SYS_RAT_3GPP2_EHRPD            DS_SYS_RAT_3GPP2_EHRPD
#define PS_SYS_RAT_3GPP2_FMC              DS_SYS_RAT_3GPP2_FMC
#define PS_SYS_RAT_3GPP2_NULL_BEARER      DS_SYS_RAT_3GPP2_NULL_BEARER

/**
  SO Mask for 1x
*/
#define PS_SYS_SO_3GPP2_1X_IS95                DS_SYS_SO_3GPP2_1X_IS95
#define PS_SYS_SO_3GPP2_1X_IS2000              DS_SYS_SO_3GPP2_1X_IS2000
#define PS_SYS_SO_3GPP2_1X_IS2000_REL_A        DS_SYS_SO_3GPP2_1X_IS2000_REL_A

/**
  SO Mask for DO. 
   
  The following table gives which of the following so_masks are supported by 
  which technology 
   
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
#define PS_SYS_SO_3GPP2_EVDO_DPA           DS_SYS_SO_3GPP2_EVDO_DPA
#define PS_SYS_SO_3GPP2_EVDO_MFPA          DS_SYS_SO_3GPP2_EVDO_MFPA
#define PS_SYS_SO_3GPP2_EVDO_EMPA          DS_SYS_SO_3GPP2_EVDO_EMPA
#define PS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD    DS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD
#define PS_SYS_SO_3GPP2_EVDO_MMPA          DS_SYS_SO_3GPP2_EVDO_MMPA
#define PS_SYS_SO_3GPP2_EVDO_MMPA_EHRPD    DS_SYS_SO_3GPP2_EVDO_MMPA_EHRPD


/**
   Defines the MAX length for PDN/APN name string
*/
#define PS_SYS_MAX_APN_LEN        DS_SYS_MAX_APN_LEN
#define PS_SYS_MAX_APNS           DS_SYS_MAX_APNS /**< Max number of APNs */
#define PS_SYS_MAX_BEARER         DS_SYS_MAX_BEARER
#define PS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX DS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX  
/**< Max number of LTE Attach PDN profile IDs */

#define PS_SYS_MAX_AVAIL_SYS      DS_SYS_MAX_AVAIL_SYS
#define PS_SYS_PREF_SYS_INDEX_INVALID DS_SYS_PREF_SYS_INDEX_INVALID
/**< If number of available system is zero then there are no available 
system on which UE can make the call. In such cases, preferred system index
would be set to  PS_SYS_PREF_SYS_INDEX_INVALID. If any APN's preferred system
is not available then its preferred system index field would be set to 
PS_SYS_PREF_SYS_INDEX_INVALID. */

#define PS_SYS_MAC_ADDR_LEN       DS_SYS_MAC_ADDR_LEN

#define PS_SYS_MAX_NUM_THROUGHPUT_INFO    DS_SYS_MAX_NUM_THROUGHPUT_INFO
#define PS_SYS_MAX_SSID_LEN       DS_SYS_MAX_SSID_LEN
#define PS_SYS_MAX_NUM_BSSID              16
#define PS_SYS_MAX_NUM_BSSID_PER_REPORT   50
#define PS_SYS_MAX_NUM_SSID               8
#define PS_SYS_MAX_CHANNELS               16

#define PS_SYS_MAX_REMOTE_SOCKETS         255


/* For low tier architecture like triton, only 6 pdns are supported */
#ifdef FEATURE_DATA_MPSS_ULT
  #define PS_SYS_MAX_AVAIL_PDNS 6
#else 
  #define PS_SYS_MAX_AVAIL_PDNS 8
#endif /* FEATURE_DATA_MPSS_ULT */

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
  @brief Defines different Technology type values
*/
typedef enum
{
  PS_SYS_TECH_MIN     = DS_SYS_TECH_MIN,
  PS_SYS_TECH_3GPP    = DS_SYS_TECH_3GPP,  /**< Tech type 3gpp */
  PS_SYS_TECH_3GPP2   = DS_SYS_TECH_3GPP2, /**< Tech type 3gpp2 */
  PS_SYS_TECH_WLAN    = DS_SYS_TECH_WLAN,  /**< Tech type WLAN */
  PS_SYS_TECH_ALL     = DS_SYS_TECH_ALL,   /**< All the tech types */
  PS_SYS_TECH_MAX     = DS_SYS_TECH_MAX,
  PS_SYS_TECH_INVALID = DS_SYS_TECH_INVALID
}ps_sys_tech_enum_type;


/**
  @brief This is the type that is used to define different network types
*/
typedef enum
{
  PS_SYS_NETWORK_3GPP,  /**< Network type 3gpp */
  PS_SYS_NETWORK_3GPP2, /**< Network type 3gpp2 */
  PS_SYS_NETWORK_WLAN,  /**< Network type WLAN */
  PS_SYS_NETWORK_MAX,
} ps_sys_network_enum_type;


/**
  @brief This is the type that is used to define core network details 
  @see   ds_sys_network_info_type 
*/
typedef struct
{
  uint32   rat_mask;   /**< RAT Mask */
  uint32   so_mask;    /**< SO Mask */
} ps_sys_network_info_type;

/**
  @brief This is the type that is used to define system status. It is 
         common to both configuration PS_SYS_CONF_SYSTEM_STATUS and event
         PS_SYS_EVENT_SYSTEM_STATUS_CHANGE
   
  @see PS_SYS_CONF_SYSTEM_STATUS
  @see PS_SYS_EVENT_SYSTEM_STATUS_CHANGE 
  @see ds_sys_system_status_type 
*/
typedef struct
{
  ps_sys_network_enum_type        preferred_network; /**< pref network value*/
  ps_sys_network_info_type        network_info[PS_SYS_NETWORK_MAX];
  /**< Array of rat and SO mask for all networks */
} ps_sys_system_status_type;

/**
  @brief This is the type that is used to define WQE status
*/
typedef enum
{
  PS_SYS_WLAN_WQE_INACTIVE     = 0x1,
  PS_SYS_WLAN_WQE_ACTIVE       = 0x2
} ps_sys_wlan_wqe_status_enum_type;

/**
  @brief This is the mask for WQE profile type and also used for 
         PS_SYS_EVENT_WQE_PROFILE_CHANGE payload
*/
typedef ds_sys_wqe_profile_type ps_sys_wqe_profile_type;

/**
  @brief This is the type that is used to define Wifi association type 
*/
typedef enum {
  PS_SYS_WLAN_ASSOC_P2P_GO,     /**< Peer to Peer Group WLAN association */
  PS_SYS_WLAN_ASSOC_P2P_CLIENT, /**< Peer to Peer Client WLAN association */
  PS_SYS_WLAN_ASSOC_STA,        /**< Station WLAN association */
  PS_SYS_WLAN_ASSOC_SOFTAP,     /**< Software Access Point WLAN association */
  PS_SYS_WLAN_ASSOC_INVALID = 0xFF
} ps_sys_wlan_assoc_type_enum;

typedef enum {
  PS_SYS_WLAN_CONN_STATUS_DISASSOCIATED,/** Connection status specifying if wlan is 
                                            disassociated from an access point */
  PS_SYS_WLAN_CONN_STATUS_ASSOCIATED,   /** Connection status specifying if wlan is 
                                            associated with an access point */
  PS_SYS_WLAN_CONN_STATUS_IP_CONNECTED,  /** Connection status specifying if wlan is 
                                            IP connected */
  PS_SYS_WLAN_CONN_STATUS_INVALID = 0xFF
} ps_sys_wlan_conn_status_type_enum;

typedef enum {
  PS_SYS_WLAN_NETWORK_MODE_802_11_1997,/**< Legacy network Mode. */
  PS_SYS_WLAN_NETWORK_MODE_802_11a,    /**< OFDM Network mode. */
  PS_SYS_WLAN_NETWORK_MODE_802_11b,    /**< 802.11b */
  PS_SYS_WLAN_NETWORK_MODE_802_11g,    /**< 802.11g */
  PS_SYS_WLAN_NETWORK_MODE_802_11_2007,/**< 802.11.2007 */
  PS_SYS_WLAN_NETWORK_MODE_802_11n,    /**< 802.11n */
  PS_SYS_WLAN_NETWORK_MODE_802_11_2012,/**< 802.11.2012 */
  PS_SYS_WLAN_NETWORK_MODE_802_11ac,   /**< 802.11.ac */
  PS_SYS_WLAN_NETWORK_MODE_802_11ad,   /**< 802.11.ad */
  PS_SYS_WLAN_NETWORK_MODE_802_11af,   /**< 802.11.af */
  PS_SYS_WLAN_NETWORK_MODE_802_11ah,   /**< 802.11.ah */
  PS_SYS_WLAN_NETWORK_MODE_802_11ai,   /**< 802.11.ai */
  PS_SYS_WLAN_NETWORK_MODE_802_11aj,   /**< 802.11.aj */
  PS_SYS_WLAN_NETWORK_MODE_802_11aq,   /**< 802.11.aq */
  PS_SYS_WLAN_NETWORK_MODE_802_11ax,   /**< 802.11.ax */
  PS_SYS_WLAN_NETWORK_MODE_802_11ay,   /**< 802.11.ay */
  PS_SYS_WLAN_NETWORK_MODE_INVALID = 0xFF
} ps_sys_wlan_network_mode_type_enum;

/**
  @brief This is the type that is used for 
         PS_SYS_CONF_WLAN_STATUS configuration and
         PS_SYS_EVENT_WLAN_STATUS_CHANGE
   
  @see PS_SYS_CONF_WLAN_STATUS
  @see PS_SYS_EVENT_WLAN_STATUS_CHANGE 
*/

typedef struct
{
  boolean                            is_wlan_available; /**< WLAN availability*/
  ps_ip_addr_type                    ipv4_addr;  /**< WLAN IPV4 address */
  ps_ip_addr_type                    ipv6_addr;  /**< WLAN IPV6 address */
  uint8                              ipv6_prefix_len;  /**< IPV6 addr prefix length */
  uint8                              ap_mac_addr[PS_SYS_MAC_ADDR_LEN]; /**< WLAN Access Point MAC addr*/
  ps_sys_wlan_wqe_status_enum_type   wqe_status;  /**< Whether Wifi Quality Estimation was performed on the Apps Proc*/
  ps_ip_addr_type                    dns_ipv4_addr1;  /**< DNS IPV4 address 1 */
  ps_ip_addr_type                    dns_ipv4_addr2;  /**< DNS IPV4 address 2 */
  ps_ip_addr_type                    dns_ipv6_addr1;  /**< DNS IPV6 address 1 */
  ps_ip_addr_type                    dns_ipv6_addr2;  /**< DNS IPV6 address 2 */
  ps_ip_addr_type                    epdg_ipv4_addr1;  /**< EPDG IPV4 address 1 */
  ps_ip_addr_type                    epdg_ipv4_addr2;  /**< EPDG IPV4 address 2 */
  ps_ip_addr_type                    epdg_ipv6_addr1;  /**< EPDG IPV6 address 1 */
  ps_ip_addr_type                    epdg_ipv6_addr2;  /**< EPDG IPV6 address 2 */
  char                               ssid[PS_SYS_MAX_SSID_LEN]; /**< SSID */
  boolean                            is_channel_valid;  /**< Is channel valid*/
  uint16                             channel;           /**< Channel in MHz*/
  boolean                            is_channel_bandwidth_valid;  /**< Is channel valid*/
  uint16                             channel_bandwidth; /**< Channel bandwidth*/ 
  ps_sys_wqe_profile_type            wqe_profile_type;  /**< Wifi Quality Estimation Profile Type*/
  ps_sys_wlan_assoc_type_enum        wlan_assoc_type;   /**< Wlan Association Type*/
  ps_sys_wlan_network_mode_type_enum network_mode;      /**< Network Mode*/
  ps_sys_wlan_conn_status_type_enum  connection_status; /**< Wlan Connection Status */
  boolean                            is_default_route;  /**< Default Route. */
} ps_sys_wlan_status_type;

/**
  @brief This is the type that is used for 
         PS_SYS_CONF_GET_WLAN_MAC_ADDR configuration and
         PS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE
   
  @see PS_SYS_CONF_GET_WLAN_MAC_ADDR
  @see PS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE 
*/
typedef struct
{
  uint8            ap_mac_addr[DS_SYS_MAC_ADDR_LEN]; 
  /**< WLAN Access Point MAC addr*/
} ps_sys_wlan_mac_addr_type;

/**
  @brief This is the type that is used for 
         PS_SYS_CONF_GET_WLAN_SSID configuration and
         PS_SYS_EVENT_WLAN_SSID_CHANGE
   
  @see PS_SYS_CONF_GET_WLAN_SSID
  @see PS_SYS_EVENT_WLAN_SSID_CHANGE 
*/
typedef struct
{
  char             ssid[PS_SYS_MAX_SSID_LEN]; 
  /**< WLAN Service Set Identifier*/
} ps_sys_wlan_ssid_type;

/**
  @brief This is the type that is used for 
         PS_SYS_CONF_WLAN_PREFERENCE configuration and
         PS_SYS_EVENT_WLAN_PREFERENCE_CHANGE
   
  @see PS_SYS_CONF_WLAN_PREFERENCE
  @see PS_SYS_EVENT_WLAN_PREFERENCE_CHANGE 
*/
typedef enum
{
  PS_SYS_WLAN_LB_PREFERRED     = 0x00,
  PS_SYS_WLAN_LB_NOT_PREFERRED = 0x01
} ps_sys_wlan_preference_enum_type;

/**
  @brief Defines structure for PS_SYS_EVENT_WLAN_MAX_CONN event
  
  @see PS_SYS_EVENT_WLAN_MAX_CONN 
*/
typedef struct
{
  boolean is_max_conn_reached; 
  uint8   num_apns;
  char    active_apn_name[PS_SYS_MAX_AVAIL_PDNS][PS_SYS_MAX_APN_LEN];
}ps_sys_wlan_max_conn_info_type;

/**
  @brief This is the type that is used to identify the list of 
         channels associated with each BSSID per SSID for which 
         WiFi measurements are required with each 
         PS_SYS_EVENT_WLAN_START_WIFI_MEAS
  @see PS_SYS_EVENT_WLAN_START_WIFI_MEAS
*/

typedef struct
{
  uint8              bssid[PS_SYS_MAC_ADDR_LEN];         /**BSSID*/
  uint8              num_channel_info;                   /** Num of channels*/
  uint16             channel_list[PS_SYS_MAX_CHANNELS];  /**< List of channels in MHz */
}ps_sys_wlan_wifi_meas_bssid_type;

typedef struct
{
  char               ssid[PS_SYS_MAX_SSID_LEN];                     
  /**< SSID */
  boolean            is_hidden;
  /**< Is the SSID Hidden. */
  uint8              num_bssid_info;                     
  /**< num of BSSID info*/
  ps_sys_wlan_wifi_meas_bssid_type bssid_info[PS_SYS_MAX_NUM_BSSID];
   /**< List of BSSID info*/
} ps_sys_wlan_wifi_meas_ssid_type;

typedef struct
{
  uint8              num_ssid_info;
  /**< num of SSID info*/
  ps_sys_wlan_wifi_meas_ssid_type ssid_info[PS_SYS_MAX_NUM_SSID];
  /**< List of SSID info*/
} ps_sys_wlan_wifi_meas_ssid_list;

/* Wifi measurement interested parameter mask has a size of 8 bytes */
#define PS_SYS_WIFI_MEAS_PARAM_UNSPECIFIED          0x0000000000000000
#define PS_SYS_WIFI_MEAS_PARAM_RSSI                 0x0000000000000001
#define PS_SYS_WIFI_MEAS_PARAM_SINR                 0x0000000000000002
#define PS_SYS_WIFI_MEAS_PARAM_BSS_LOAD             0x0000000000000004
#define PS_SYS_WIFI_MEAS_PARAM_PHY_RATE             0x0000000000000008
#define PS_SYS_WIFI_MEAS_PARAM_PACKET_ERROR_RATE    0x0000000000000010

typedef struct
{
  uint32                           meas_id; 
  /**< ID associated with every wifi meas start*/
  uint64                           meas_param_mask;  
  /**< Mask of interested parameters */
  ps_sys_wlan_wifi_meas_ssid_list   ssid_list;        
  /**< Key based on which the measurements are reported */
  boolean                          is_valid_sampling_timer;
  /**< boolean to specify if sampling timer is set*/
  uint32                           sampling_timer;  
  /**< Time in ms after which all measurements are to be sampled */
  boolean                          is_valid_report_interval;
  /**< boolean to specify if report interval is set*/
  uint32                           report_interval;  
  /**< Time in ms over which report is to be sent*/
  boolean                          is_valid_alpha;
  /**< boolean to specify if alpha is set*/
  float                            alpha; 
  /**< Coefficient used to calculate average */
} ps_sys_wlan_wifi_meas_start_type;

/**
  @brief This is the type that is used for 
         PS_SYS_EVENT_WLAN_STOP_WIFI_MEAS
   
  @see PS_SYS_EVENT_WLAN_STOP_WIFI_MEAS 
*/

typedef struct
{
  uint32   meas_id;
  /**< ID associated with every wifi meas start*/
} ps_sys_wlan_wifi_meas_stop_type;

/**
  @brief This is the type that is used for 
         PS_SYS_IOCTL_WLAN_WIFI_MEAS_REPORT
   
  @see PS_SYS_IOCTL_WLAN_WIFI_MEAS_REPORT 
*/

typedef struct
{
  uint16 station_count;
  /**< Station count.  */
  uint16 channel_utilization;
  /**< Channel utilization.  */
  uint16 available_admission_capacity;
  /**< Available Admission Capacity.  */
} ps_sys_wlan_wifi_bss_load_type;

typedef struct
{
  uint16  channel;
  /**< Channel in MHz.  */
  uint64  meas_param_mask;  
  /**< Mask of valid parameters  */
  int16   rssi;
  /**< Received Signal Strength Indication value for the channel*/
  int16   sinr;
  /**< Signal to Inteference plus Noise Ratio value for the channel */
  ps_sys_wlan_wifi_bss_load_type bss_load;
  /**< Basic Service Set Load value for the channel */
} ps_sys_wlan_wifi_meas_info_per_channel_type;

typedef struct {
  uint8 bssid[PS_SYS_MAC_ADDR_LEN];
  /**< Basic Service Set Identifier.  */
  ps_sys_wlan_wifi_meas_info_per_channel_type channel_info;
  /**< WiFi measurement information for the channel.  */
} ps_sys_wlan_wifi_meas_info_per_bssid_type;

typedef struct
{
  uint32  meas_id;
  /**< ID associated with every wifi meas start*/
  uint32  tx_id;
  /**< Identify a set of measurements for a given meas_id*/
  boolean is_last_report_of_transaction;
  /**< Boolean to specify the last report, 
    if reports per SSID are split,*/
  char    ssid[PS_SYS_MAX_SSID_LEN]; 
  /**< SSID */
  uint8   num_bssid_info;
  /**< Number of BSSID info*/
  ps_sys_wlan_wifi_meas_info_per_bssid_type per_bssid_info[PS_SYS_MAX_NUM_BSSID_PER_REPORT];
  /**< WiFi measurement information for each BSSID. */
} ps_sys_wlan_wifi_meas_report_type;

/**
  @brief This is the type that is used for 
         PS_SYS_IOCTL_WLAN_WIFI_MEAS_REPORT_CONFIG_STATUS
   
  @see PS_SYS_IOCTL_WLAN_WIFI_MEAS_REPORT_CONFIG_STATUS 
*/

typedef enum 
{
  PS_SYS_WLAN_WIFI_MEAS_REPORT_CONFIG_OK, 
  /**< Configuration OK \n */
  PS_SYS_WLAN_WIFI_MEAS_REPORT_CONFIG_ERROR, 
  /**< Configuration error \n */
  PS_SYS_WLAN_WIFI_MEAS_REPORT_STOPPED  
  /**< Report Stopped */
} ps_sys_wlan_wifi_meas_report_config_status_enum_type;

typedef struct 
{
  uint32 wifi_meas_id;
  /**< ID provided with every start Wifi meas. 
  */
  ps_sys_wlan_wifi_meas_report_config_status_enum_type status;
  /**< Status regarding Wifi report config */
} ps_sys_wlan_wifi_meas_report_config_status_type;

/**
  @brief This is the type that is used for PS_SYS_CONF_3GPP2_S101_STATUS_CHANGE 
         configuration and PS_SYS_EVENT_3GPP2_S101_STATUS_CHANGE event
  @see   PS_SYS_CONF_3GPP2_S101_STATUS_CHANGE 
  @see   PS_SYS_EVENT_3GPP2_S101_STATUS_CHANGE 
*/
typedef struct
{
  boolean tunnel_call_allowed;           /**< if tunnel call is allowed */
  boolean lcp_throttled;                 /**< if lcp is throttle */
  boolean virtual_connection_throttled;  /**< if virtual connection is 
                                              throttled */
  boolean ehrpd_active_handoff_in_prog;   /**< if eHRPD Active Handoff is in
                                              progress*/
} ps_sys_3gpp2_s101_status_change_type;

/**
  @brief Defines structure for PS_SYS_IOCTL_PDN_THROTTLE_INFO ioctl command and
         PS_SYS_EVENT_PDN_THROTTLE_INFO event
  
  @see PS_SYS_EVENT_PDN_THROTTLE_INFO 
  @see PS_SYS_IOCTL_PDN_THROTTLE_INFO 
*/
typedef struct
{
  boolean    is_ipv4_throttled;             /**< ipv4 is throttled */
  boolean    is_ipv6_throttled;             /**< ipv6 is throttled */
  uint32     remaining_ipv4_throttled_time; /**< ipv4 remaining time in 
                                                 miliseconds */ 
  uint32     remaining_ipv6_throttled_time; /**< ipv6 remaining time in 
                                                 miliseconds */
  char       apn_string[PS_SYS_MAX_APN_LEN]; /**< PDN name */
  uint8      apn_string_len;                 /**< PDN name length */
} ps_sys_pdn_ctxt_throttle_info_type;

typedef struct
{
  ps_sys_pdn_ctxt_throttle_info_type pdn_throttle_info[PS_SYS_MAX_APNS];
  /**< For each internet, we have the values ie max apns */

  uint8 num_apn; /**< Refers to the no. of valid array elements in 
                      pdn_throttle_info[] */
  boolean  is_epc_ho_throttle; /**< This flag indicates whether the 
                                    throttle cmd is invoked by EPC  
                                    or mode handler */
} ps_sys_pdn_throttle_info_type;

/**
  @brief Defines the pdn status
*/
typedef enum
{
  PS_SYS_PDN_DOWN,
  PS_SYS_PDN_UP, 
} ps_sys_pdn_status_enum_type;

/**
  @brief Defines structure for PS_SYS_EVENT_PDN_STATUS_INFO event
  pdn_status     : pdn is up or down
  ip_type        : IP address type
  apn_string     : apn name
  apn_string_len : length of the apn name
  @see PS_SYS_EVENT_PDN_STATUS_INFO 
*/
typedef struct
{
  ps_sys_pdn_status_enum_type  pdn_status; 
  ip_addr_enum_type            ip_type;                     
  uint16                       apn_string_len;                 
  char                         apn_string[PS_SYS_MAX_APN_LEN]; 
} ps_sys_pdn_status_info_type;

/**
  @brief Event info type for PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE
         event
*/
typedef uint8 ps_sys_3gpp2_page_monitor_period_type;

typedef struct
{
  ps_sys_3gpp2_page_monitor_period_type       page_monitor_period;
  boolean                                     force_long_sleep;
} ps_sys_3gpp2_evdo_page_monitor_period_type;

/**
  @brief Gives the information about hysteresis cancellation 
         or expiry on the current EPZID.
   
  @see PS_SYS_EVENT_3GPP2_EPZID_INFO
*/
typedef struct
{
  uint8   epzid_info_val;
} ps_sys_3gpp2_epzid_info_type;

/**
  @brief Event info type for roaming status change  
  @see PS_SYS_EVENT_ROAMING_STATUS_CHANGE
  @see PS_SYS_CONF_ROAMING_STATUS_INFO
*/
typedef ds_sys_roaming_status_info_type ps_sys_roaming_status_info_type;

/**
  @brief This is the type that is used to  define different RATs types
*/
typedef enum
{
  PS_SYS_RAT_EX_NULL_BEARER           = DS_SYS_RAT_EX_NULL_BEARER,

  /* 3GPP RAT Values */
  PS_SYS_RAT_EX_3GPP_WCDMA             = DS_SYS_RAT_EX_3GPP_WCDMA,     
  PS_SYS_RAT_EX_3GPP_GERAN             = DS_SYS_RAT_EX_3GPP_GERAN,
  PS_SYS_RAT_EX_3GPP_LTE               = DS_SYS_RAT_EX_3GPP_LTE,
  PS_SYS_RAT_EX_3GPP_TDSCDMA           = DS_SYS_RAT_EX_3GPP_TDSCDMA,
  PS_SYS_RAT_EX_3GPP_WLAN              = DS_SYS_RAT_EX_3GPP_WLAN,
  PS_SYS_RAT_EX_3GPP_MAX               = DS_SYS_RAT_EX_3GPP_MAX,

  /* 3GPP2 RAT Values */
  PS_SYS_RAT_EX_3GPP2_1X               = DS_SYS_RAT_EX_3GPP2_1X,
  PS_SYS_RAT_EX_3GPP2_HRPD             = DS_SYS_RAT_EX_3GPP2_HRPD,
  PS_SYS_RAT_EX_3GPP2_EHRPD            = DS_SYS_RAT_EX_3GPP2_EHRPD,
  PS_SYS_RAT_EX_3GPP2_WLAN             = DS_SYS_RAT_EX_3GPP2_WLAN,
  PS_SYS_RAT_EX_3GPP2_MAX              = DS_SYS_RAT_EX_3GPP2_MAX, 

  /* WLAN RAT Values */
  PS_SYS_RAT_EX_WLAN                   = DS_SYS_RAT_EX_WLAN,
  PS_SYS_RAT_EX_WLAN_MAX               = DS_SYS_RAT_EX_WLAN_MAX,
 
  PS_SYS_RAT_UNKNOWN                   = DS_SYS_RAT_UNKNOWN,
  PS_SYS_RAT_MAX,
} ps_sys_rat_ex_enum_type;

/* SO Mask not available or not specified */
#define PS_SYS_SO_EX_UNSPECIFIED            DS_SYS_SO_EX_UNSPECIFIED

/* 3GPP SO Mask */
#define PS_SYS_SO_EX_3GPP_WCDMA             DS_SYS_SO_EX_3GPP_WCDMA
#define PS_SYS_SO_EX_3GPP_HSDPA             DS_SYS_SO_EX_3GPP_HSDPA
#define PS_SYS_SO_EX_3GPP_HSUPA             DS_SYS_SO_EX_3GPP_HSUPA
#define PS_SYS_SO_EX_3GPP_HSDPAPLUS         DS_SYS_SO_EX_3GPP_HSDPAPLUS
#define PS_SYS_SO_EX_3GPP_DC_HSDPAPLUS      DS_SYS_SO_EX_3GPP_DC_HSDPAPLUS
#define PS_SYS_SO_EX_3GPP_64_QAM            DS_SYS_SO_EX_3GPP_64_QAM
#define PS_SYS_SO_EX_3GPP_HSPA              DS_SYS_SO_EX_3GPP_HSPA
#define PS_SYS_SO_EX_3GPP_GPRS              DS_SYS_SO_EX_3GPP_GPRS
#define PS_SYS_SO_EX_3GPP_EDGE              DS_SYS_SO_EX_3GPP_EDGE

/* GSM so mask is depreciated. Can only be GPRS or EDGE */
#define PS_SYS_SO_EX_3GPP_GSM               DS_SYS_SO_EX_3GPP_GSM

#define PS_SYS_SO_EX_3GPP_S2B               DS_SYS_SO_EX_3GPP_S2B
#define PS_SYS_SO_EX_3GPP_LTE_LIMITED_SRVC  DS_SYS_SO_EX_3GPP_LTE_LIMITED_SRVC
#define PS_SYS_SO_EX_3GPP_LTE_FDD           DS_SYS_SO_EX_3GPP_LTE_FDD
#define PS_SYS_SO_EX_3GPP_LTE_TDD           DS_SYS_SO_EX_3GPP_LTE_TDD
#define PS_SYS_SO_EX_3GPP_TDSCDMA           DS_SYS_SO_EX_3GPP_TDSCDMA
#define PS_SYS_SO_EX_3GPP_DC_HSUPA          DS_SYS_SO_EX_3GPP_DC_HSUPA
#define PS_SYS_SO_EX_3GPP_LTE_CA_DL         DS_SYS_SO_EX_3GPP_LTE_CA_DL
#define PS_SYS_SO_EX_3GPP_LTE_CA_UL         DS_SYS_SO_EX_3GPP_LTE_CA_UL

/* 3GPP2 SO Mask */
#define PS_SYS_SO_EX_3GPP2_1X_IS95           DS_SYS_SO_EX_3GPP2_1X_IS95
#define PS_SYS_SO_EX_3GPP2_1X_IS2000         DS_SYS_SO_EX_3GPP2_1X_IS2000
#define PS_SYS_SO_EX_3GPP2_1X_IS2000_REL_A   DS_SYS_SO_EX_3GPP2_1X_IS2000_REL_A
#define PS_SYS_SO_EX_3GPP2_HDR_REV0_DPA      DS_SYS_SO_EX_3GPP2_HDR_REV0_DPA
#define PS_SYS_SO_EX_3GPP2_HDR_REVA_DPA      DS_SYS_SO_EX_3GPP2_HDR_REVA_DPA
#define PS_SYS_SO_EX_3GPP2_HDR_REVB_DPA      DS_SYS_SO_EX_3GPP2_HDR_REVB_DPA
#define PS_SYS_SO_EX_3GPP2_HDR_REVA_MPA      DS_SYS_SO_EX_3GPP2_HDR_REVA_MPA
#define PS_SYS_SO_EX_3GPP2_HDR_REVB_MPA      DS_SYS_SO_EX_3GPP2_HDR_REVB_MPA
#define PS_SYS_SO_EX_3GPP2_HDR_REVA_EMPA     DS_SYS_SO_EX_3GPP2_HDR_REVA_EMPA
#define PS_SYS_SO_EX_3GPP2_HDR_REVB_EMPA     DS_SYS_SO_EX_3GPP2_HDR_REVB_EMPA
#define PS_SYS_SO_EX_3GPP2_HDR_REVB_MMPA     DS_SYS_SO_EX_3GPP2_HDR_REVB_MMPA
#define PS_SYS_SO_EX_3GPP2_EVDO_FMC          DS_SYS_SO_EX_3GPP2_EVDO_FMC
#define PS_SYS_SO_EX_3GPP2_1X_CS             DS_SYS_SO_EX_3GPP2_1X_CS

/**
  @brief This is the type that is used to  define different subscription types
         Unknown subs id is used when the client does not care/provide the subs id
*/
typedef enum 
{
  PS_SYS_DEFAULT_SUBS      = DS_SYS_DEFAULT_SUBS, /**< Default data subscription */
  PS_SYS_PRIMARY_SUBS      = DS_SYS_PRIMARY_SUBS, /**< Primary */
  PS_SYS_SECONDARY_SUBS    = DS_SYS_SECONDARY_SUBS, /**< Secondary */
  PS_SYS_TERTIARY_SUBS     = DS_SYS_TERTIARY_SUBS,  /**< Tertiary */  
  PS_SYS_SUBS_MAX          = DS_SYS_SUBS_MAX
} ps_sys_subscription_enum_type;

/**
  @brief This is the tupple that defines network, RAT, SO and service status
*/
typedef struct 
{
  ps_sys_network_enum_type                 technology; /**< technology type*/
  ps_sys_rat_ex_enum_type                  rat_value;  /**< rat type*/
  uint64                                   so_mask;   /**< so type*/
} ps_sys_system_status_info_type;

/**
  @brief  This is the type that defines apn to preferred network relationship. 
          For every APN, there would be tuple that describes its preferred 
          system.  
*/
typedef struct 
{
   char                           apn_name[PS_SYS_MAX_APN_LEN]; /**< PDN name*/
   uint32                         num_avail_sys; 
   /**< Number of valid entries to available system status array */
   ps_sys_system_status_info_type avail_sys[PS_SYS_MAX_AVAIL_SYS];  
  /**<  Array of available systems for the APN. The topmost element of the 
        array will convey the current preferred system for the APN.
        @see ps_sys_system_status_ex_type 
        @see ps_sys_system_status_info_type */
   uint8                          recommended_pref_system_index;
  /**< Index in avail_sys array, which contains the 
       recommended preferred system for the APN */
} ps_sys_apn_pref_sys_info_type;

/**
  @brief This is the type that is used to define system status. It is
         common to both configuration PS_SYS_CONF_SYSTEM_STATUS_EX and event
         PS_SYS_EVENT_SYSTEM_STATUS_EX
*/
typedef struct 
{
  uint32   num_avail_sys; 
  /**< Number of valid entries to available system status array */

  ps_sys_system_status_info_type  avail_sys[PS_SYS_MAX_AVAIL_SYS]; 
  /**< Array of current system status,  the topmost element in the array will 
       contain the current preferred system*/

  uint32   num_avail_apns;
  /**< Number of apn entries to pdn_info */

  ps_sys_apn_pref_sys_info_type apn_pref_sys_info[PS_SYS_MAX_APNS];    
  /**< Array of all apn available */

  uint8                          recommended_pref_system_index;
  /**< Index in avail_sys array for the APN, which contains the 
       recommended preferred system for the APN */
} ps_sys_system_status_ex_type;

/**
  @brief Enum type describing APN Preferred system.
  @see   ps_sys_conf_apn_pref_sys_type
*/
typedef enum
{
  PS_SYS_APN_PREF_SYS_WWAN  = DS_SYS_APN_PREF_SYS_WWAN,
  PS_SYS_APN_PREF_SYS_WLAN  = DS_SYS_APN_PREF_SYS_WLAN,
  PS_SYS_APN_PREF_SYS_IWLAN = DS_SYS_APN_PREF_SYS_IWLAN
} ps_sys_apn_pref_sys_enum_type;

/**
  @brief Data type to be used with PS_SYS_CONF_APN_PREF_SYS and 
         PS_SYS_EVENT_APN_PREF_SYS_CHANGE. It describes preferred 
         system for apn.         
*/
typedef struct
{
  unsigned char                  apn[PS_SYS_MAX_APN_LEN];
  ps_sys_apn_pref_sys_enum_type  pref_sys;
} ps_sys_apn_pref_sys_type;

/**
  @brief Data type to hold the apn name
*/
typedef struct
{
  unsigned char     apn_name[PS_SYS_MAX_APN_LEN];
} ps_sys_apn_name_type;

/**   
  @see ds_sys_roaming_disallowed_info_type
*/
typedef struct
{
  boolean                 is_roaming_disallowed;
  uint8                   num_apn_names;
  ps_sys_apn_name_type    apn_list[PS_SYS_MAX_APNS];
} ps_sys_roaming_disallowed_info_type;

/**
  @brief Data type to be used with lte attach pdn list. 
   
  @see  PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST 
  @see  PS_SYS_IOCTL_3GPP_GET_LTE_ATTACH_PDN_LIST 
  @see  PS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE 
*/
typedef ds_sys_3gpp_lte_attach_pdn_list_type ps_sys_3gpp_lte_attach_pdn_list_type;
/**
  @brief This is the type that is used for 
         PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE configuration and
         PS_SYS_EVENT_3GPP_LTE_ATTACH_TYPE_CHANGED
   
  @see PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE
  @see PS_SYS_EVENT_3GPP_LTE_ATTACH_TYPE_CHANGED 
*/
typedef enum
{
  PS_SYS_LTE_ATTACH_TYPE_INITIAL  = 0x00,
  PS_SYS_LTE_ATTACH_TYPE_HANDOFF  = 0x01
} ps_sys_lte_attach_enum_type;

/**
  @brief system status event payload for DIAG events
         EVENT_PS_SYSTEM_STATUS
         EVENT_PS_SYSTEM_STATUS_EX
         EVENT_QMI_SYSTEM_STATUS
         EVENT_QMI_SYSTEM_STATUS_EX
  */
typedef struct 
{
  ps_sys_network_enum_type       pref_network; 
  uint32                         rat_value;  
  uint64                         so_mask;
  ps_sys_subscription_enum_type  subs_id;   
} ps_sys_system_status_diag_event_logging_type;

/**
  @brief Traffic state enum type
  Note : Donot change the existing values of below enum type. 
         START : 1 (TRUE) and STOP : 0 (FALSE)
  */
typedef enum
{
  PS_SYS_LOW_LATENCY_TRAFFIC_STATE_STOP,
  PS_SYS_LOW_LATENCY_TRAFFIC_STATE_START,
  PS_SYS_LOW_LATENCY_TRAFFIC_STATE_CONTINUE,
  PS_SYS_LOW_LATENCY_TRAFFIC_STATE_MAX
} ps_sys_low_latency_traffic_state_enum_type;

/**
  @brief Low latency traffic status information. 
         Example : 
         1. If packet arrives at time T, then sleep for
         pkt_interval_time_in_ms - pkt_arrival_delta_in_ms. 
         2. Wait for next packet to arrive with in pkt_max_wait_time_in_ms.
         3. Once packet arrive. Goto step 1     
         4. If packet doesnt arrive then low latency traffic end indication 
            should have been sent         

  @see PS_SYS_EVENT_LOW_LATENCY_TRAFFIC_STATUS
*/
typedef struct
{
  /*! Traffic State */  
  ps_sys_low_latency_traffic_state_enum_type   traffic_state;
  
  /*! Filter result */  
  uint32    filter_handle;    
  /*! Expected periodicity of the packet arrivals for this low latency 
    traffic (in ms).
    Value 0xFFFFFFFF - means unknown */
  uint32                     pkt_interval_time_in_ms;
  
  /*! Expected delta time of the packet from its interval time to wake up
    Value 0xFFFFFFFF - means unknown */
  uint32                     pkt_arrival_delta_in_ms;
  /*! Expected time to wait for the packets at every period 
    for this low latency traffic (in ms).
    Value 0xFFFFFFFF - means unknown  */
  uint32                     pkt_max_wait_time_in_ms;
} ps_sys_low_latency_traffic_info_type;

/**
  @brief Throughput information for all active PDNs  
  @see  PS_SYS_CONF_THROUGHPUT_INFO 
  @see  PS_SYS_EVENT_THROUGHPUT_INFO 
*/

typedef ds_sys_ip_addr_enum_type                ps_sys_ip_addr_enum_type;
typedef ds_sys_throughput_quality_enum_type     ps_sys_throughput_quality_enum_type;
typedef ds_sys_throughput_info_list_type        ps_sys_throughput_info_list_type;
typedef ds_sys_throughput_info_type             ps_sys_throughput_info_type; 
typedef ds_sys_bearer_throughput_info_list_type ps_sys_bearer_throughput_info_list_type;
typedef ds_sys_bearer_throughput_info_type      ps_sys_bearer_throughput_info_type;

/**
  @brief Data type to be used with PS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS and 
         PS_SYS_EVENT_3GPP_EMERGENCY_PDN_STATUS. 
  @see   ds_sys_is_pdn_only_emergency_type
*/
typedef ds_sys_is_pdn_only_emergency_type ps_sys_is_pdn_only_emergency_type;

/**
  @brief Data type to be used with PS_SYS_CONF_NULL_RESOLVED_APN
         and PS_SYS_EVENT_NULL_RESOLVED_APN
  apn_length       : Length of the apn name
  apn_name         : Apn name 
  */
typedef struct 
{
  uint8         apn_length;  
  char          apn_name[PS_SYS_MAX_APN_LEN];
} ps_sys_null_resolved_apn_info_type;

/**
  @brief Data type to be used with network aware status
   
  @see   PS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS
  @see   PS_SYS_EVENT_3GPP2_QOS_NETWORK_STATUS_CHANGE  
*/
typedef ds_sys_3gpp2_network_qos_aware_status ps_sys_3gpp2_network_qos_aware_status;

/**
  @brief Data type to be used with page monitor period
   
  @see   PS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD
  @see   PS_SYS_EVENT_3GPP2_PAGE_MONITOR_PERIOD_CHANGE
 */
typedef ds_sys_3gpp2_page_monitor_period_info_type ps_sys_3gpp2_page_monitor_period_info_type;

/**
  @brief Data type to be used with downlink throughput info 
         timer
   
  @see   PS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER
  @see   PS_SYS_EVENT_UPDATED_DL_THROUGHPUT_TIMER
*/
typedef ds_sys_report_dl_timer_type ps_sys_report_dl_timer_type;

/**
  @brief Data type to be used with downlink throughput info 
   
  @see   PS_SYS_CONF_DL_THROUGHPUT_INFO
  @see   PS_SYS_EVENT_DL_THROUGHPUT_INFO
*/
typedef ds_sys_dl_throughput_info_type ps_sys_dl_throughput_info_type;


/**
  @brief Data type to be used with thermal level mitigation info
   
  @see   PS_SYS_CONF_THERMAL_MITIGATION_INFO
  @see   PS_SYS_EVENT_THERMAL_MITIGATION_INFO
 */
typedef enum
{
  PS_SYS_THERMAL_MITIGATION_ALL_CALLS_ALLOWED = DS_SYS_THERMAL_MITIGATION_ALL_CALLS_ALLOWED,
  PS_SYS_THERMAL_MITIGATION_IMS_CALLS_ONLY    = DS_SYS_THERMAL_MITIGATION_IMS_CALLS_ONLY,
  PS_SYS_THERMAL_MITIGATION_NO_CALLS_ALLOWED  = DS_SYS_THERMAL_MITIGATION_NO_CALLS_ALLOWED
} ps_sys_thermal_mitigation_action_enum_type;

/*---------------------------------------------------------------------------
  APN type enum
---------------------------------------------------------------------------*/
typedef enum
{
  PS_SYS_APN_TYPE_INTERNET     = DS_SYS_APN_TYPE_INTERNET,
  PS_SYS_APN_TYPE_IMS          = DS_SYS_APN_TYPE_IMS,   
  PS_SYS_APN_TYPE_MAX          = DS_SYS_APN_TYPE_MAX
} ps_sys_apn_enum_type;

/**
  @brief Data type to be used with data service
   
  @see   PS_SYS_EVENT_DATA_SERVICE_SETTING
  @see   PS_SYS_CONF_DATA_SERVICE_SETTING 
 */
typedef ds_sys_data_services_setting_type ps_sys_data_services_setting_type;

/**
  @brief Data type to be used with  data service roaming
   
  @see   PS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING
  @see   PS_SYS_CONF_DATA_SERVICE_ROAMING_SETTING
 */
typedef ds_sys_data_services_roaming_setting_type ps_sys_data_services_roaming_setting_type;

/**
  @brief Data type to store the apn info 
  @see   PS_SYS_EVENT_APN_INFO 
  @see   PS_SYS_CONF_APN_INFO 
*/
typedef ds_sys_apn_params_type ps_sys_apn_params_type;

typedef ds_sys_apn_info_type ps_sys_apn_info_type;


/**
  @brief Modem power stats config type

  @see PS_SYS_CONF_MODEM_POWER_STATS_CONFIGURATION 
  @see PS_SYS_EVENT_MODEM_POWER_STATS_CONFIGURATION 
*/
typedef struct
{
  boolean   enable_power_statistics;
} ps_sys_modem_power_stats_config_type;

//Enumeration for tx mode levels
typedef enum {
  PS_SYS_MODEM_POWER_LEVEL_ONE,
  PS_SYS_MODEM_POWER_LEVEL_TWO,
  PS_SYS_MODEM_POWER_LEVEL_THREE,
  PS_SYS_MODEM_POWER_LEVEL_FOUR,
  PS_SYS_MODEM_POWER_LEVEL_FIVE,
  PS_SYS_MODEM_POWER_LEVEL_MAX
} ps_sys_modem_power_level_enum_type;

typedef struct {
  ps_sys_modem_power_level_enum_type  tx_mode_level;
  /**< Power levels in Tx(Transmitter) mode */
   
  uint32  tx_mode_duration;
  /**< Duration that modem was active on 
       Tx mode(Transmitter mode) in milliseconds 
  */
} ps_sys_tx_mode_type;

/**
  @brief Modem power stats config type

  @see PS_SYS_CONF_MODEM_POWER_STATISTICS 
  @see PS_SYS_EVENT_MODEM_POWER_STATISTICS 
*/
typedef struct
{
  uint32 idle_mode_duration;
  /**< The duration that modem was awake but RRC connection 
       was released in milliseconds
  */

  uint32 sleep_mode_duration;
  /**< Duration that modem was in power collapse in milliseconds
  */

  uint32 rx_mode_duration;
  /**< Duration that modem was active on Rx mode( Receiver mode) 
       in milliseconds
  */

  ps_sys_tx_mode_type tx_mode_duration[PS_SYS_MODEM_POWER_LEVEL_MAX];
  /**< Duration that modem was active on Tx mode(Transmitter mode)
       in milliseconds 
  */
} ps_sys_modem_power_stats_info_type;

/**
  @brief Data type to be used with wifi switch settings
   
  @see   PS_SYS_CONF_DATA_SERVICE_WIFI_SETTING 
  @see   PS_SYS_EVENT_DATA_SERVICE_WIFI_SETTING
 */
typedef ds_sys_data_services_wifi_setting_type ps_sys_data_services_wifi_setting_type;

/**
  @brief Data type that provides whether remote socket handling 
  is available on the TE or not. 
  remote_socket_hdlr_avail = TRUE : TE has registered for remote 
  socket socket handling indications 
  remote_socket_hdlr_avail = FALSE : TE has not registered for 
  remote socket socket handling indications 
  @see PS_SYS_CONF_REMOTE_SOCKET_HDLR_AVAIL 
  @see PS_SYS_EVENT_REMOTE_SOCKET_HDLR_CHANGE
*/
typedef struct
{
  boolean   remote_socket_hdlr_avail;
} ps_sys_remote_socket_hdlr_avail_type;

typedef struct
{
  ip_version_enum_type      ip_family;   //v4 or v6
  ps_ip_protocol_enum_type  xport_proto; //will only contain TCP or UDP
  uint16                    port_no;     //0 or specific port number
}ps_sys_remote_socket_type;

/**
  @brief Data type that contains parameters for remote socket 
         allocation request
  @see     PS_SYS_EVENT_REMOTE_SOCKET_REQUEST 
*/
typedef struct
{
  uint8                      num_sockets; //num of sockets in list
  ps_sys_remote_socket_type  socket_list[PS_SYS_MAX_REMOTE_SOCKETS]; 
}ps_sys_remote_socket_request_type;

typedef enum
{
  PS_SYS_REMOTE_SOCKET_ALLOC_SUCCESS,
  PS_SYS_REMOTE_SOCKET_ALLOC_IN_USE_FAILURE,
  PS_SYS_REMOTE_SOCKET_ALLOC_GENERAL_FAILURE
} ps_sys_remote_socket_alloc_status;

typedef struct
{
  ps_sys_remote_socket_alloc_status  status;   //SUCCESS or FAILURE
  uint32                             remote_socket_handle; //handle to socket on the TE
  boolean                            is_ephemeral_port; //whether original request was for ephemeral port
  ps_sys_remote_socket_type          socket_info; //Info related to socket
}ps_sys_remote_socket_allocated_info_type;

/**
  @brief Data type that contains parameters for remote socket 
         allocation information from the TE
  @see     PS_SYS_IOCTL_REMOTE_SOCKET_ALLOCATED 
*/
typedef struct
{
  uint8                                     num_sockets; //num of sockets in list
  ps_sys_remote_socket_allocated_info_type  socket_list[PS_SYS_MAX_REMOTE_SOCKETS]; 
}ps_sys_remote_socket_allocated_type;

/**
  @brief Data type that contains parameters for remote socket 
         release
  @see     PS_SYS_EVENT_REMOTE_SOCKET_RELEASE 
*/
typedef struct
{
  uint8   num_sockets; //num of sockets in list
  uint32  socket_list[PS_SYS_MAX_REMOTE_SOCKETS]; //List of handles to be released
}ps_sys_remote_socket_release_type;

/**
  @brief Data type that contains parameters for setting options 
         on a remote socket
  @see     PS_SYS_EVENT_REMOTE_SOCKET_SET_OPTION 
*/
typedef struct
{
  uint32  socket_handle; //Handle to socket on the TE
  boolean is_udp_encaps; // Is UDP encapsulated or not
}ps_sys_remote_socket_set_option_type;

/**
  @brief Data type to be used with 
         PS_SYS_IOCTL_SET_WQE_PROFILE_QUALITY_MEAS_INFO
  @see   PS_SYS_IOCTL_SET_WQE_PROFILE_QUALITY_MEAS_INFO 
*/
typedef ds_sys_set_wqe_profile_quality_meas_info_list_type 
  ps_sys_set_wqe_profile_quality_meas_info_list_type;


/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/** 
  @brief This function initializes data structures for ps_sys 
         module. It initializes event queues, ps sys critical section,
         and sets the command handler for event callback in PS task. It also
         allocates memory for holding temporary storage for config data
*/
void ps_sys_init
(
  void
);

/** 
  @brief This function returns technology type given rat value
*/
ps_sys_tech_enum_type ps_sys_get_tech_from_rat
(
  ps_sys_rat_ex_enum_type  rat
);

/** 
  @brief This function sets the DDS(Default data subscription) 
         in the system. Only Ds3g module can set this value
*/
void ps_sys_set_default_data_subscription
(
  ps_sys_subscription_enum_type  def_data_subs
);

/** 
  @brief This function gets the DDS(Default data subscription) 
         in the system
*/
ps_sys_subscription_enum_type ps_sys_get_default_data_subscription
(
  void
);


#ifdef __cplusplus
}
#endif

#endif /* PS_SYS_H */
