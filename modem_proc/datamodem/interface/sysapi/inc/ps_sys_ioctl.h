/**
@file ps_sys_ioctl.h
@brief
This file defines functions, enums and structs used for two modehandlers to 
communicate with each other through ioctls 
*/

#ifndef PS_SYS_IOCTL_H
#define PS_SYS_IOCTL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ I O C T L . H


GENERAL DESCRIPTION
  This file defines functions, enums and structs used for two modehandlers
  to communicate with each other through ioctls

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


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
#include "ps_iface_defs.h"
#include "ps_sys.h"
#include "ds_sys_ioctl.h"
#include "ps_acl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
  @brief Defines different SYS IOCTL types  
*/
typedef enum
{
  PS_SYS_IOCTL_MIN         = 0x00,  

  /*3GPP2 specific ioctl common to QMI and DS SYS should be added within 
    the range PS_SYS_IOCTL_3GPP2_MIN to PS_SYS_IOCTL_3GPP2_MIN + 99
    3GPP2 specific ioctl applicable only to QMI should be added within
    the range PS_SYS_IOCTL_3GPP2_MIN +100 to PS_SYS_IOCTL_3GPP2_MIN +200*/
  PS_SYS_IOCTL_3GPP2_MIN         = DS_SYS_IOCTL_3GPP2_MIN,

  PS_SYS_IOCTL_3GPP2_PRE_DORMANCY_SYS_INFO = DS_SYS_IOCTL_3GPP2_PRE_DORMANCY_SYS_INFO,
  /**< 
      This command retrieves the network and service option information for the 
      3GPP2 data session before going into dormancy.
      If the device is not in a 3GPP2 data session, the
      service option is set to the NULL service option (0xFFFF) and the
      data session network is set to no service (0x00).
      If the device is not in a CDMA-1X data session, the service option is
      set to the NULL service option (0xFFFF
  */
  PS_SYS_IOCTL_3GPP2_SET_CAM_TIMER   =  DS_SYS_IOCTL_3GPP2_SET_CAM_TIMER,
  /**< 
      This command sets the Chatty App Manager timer value to the one specified
      in the TLV, cam_timer. This timer is used for CDMA-1X calls only.
      The CAM timer value can be changed at any point for 3GPP2 devices. If set
      during an active CDMA-1X call, the new value is used in the subsequent
      1X data call, and does not affect the current call.
      The default value of the CAM timer is set to 300 seconds. A value of 0 
      causes the timer to be disabled
  */
  PS_SYS_IOCTL_3GPP2_GET_CAM_TIMER   =  DS_SYS_IOCTL_3GPP2_GET_CAM_TIMER,
  /**< 
      This command queries the Chatty App Manager timer setting for the device.
  */
  PS_SYS_IOCTL_3GPP2_SET_SCRM        =  DS_SYS_IOCTL_3GPP2_SET_SCRM,
  /**< 
      Disables/enables the Supplemental Channel Request Message (SCRM).
      The SCRM setting can be changed at any point for 3GPP2 devices. If set
      during an active CDMA-1X call, the new value is used in the subsequent
      1X data call, and does not effect the current call.
  */ 
  PS_SYS_IOCTL_3GPP2_GET_SCRM        =  DS_SYS_IOCTL_3GPP2_GET_SCRM,
  /**< 
      This command queries whether the SCRM is enabled or disabled for the device.
  */ 
  PS_SYS_IOCTL_3GPP2_SET_RDUD        =  DS_SYS_IOCTL_3GPP2_SET_RDUD,
  /**< 
      Enables or disables reduced dormancy followed by unsolicited data.
  */  
  PS_SYS_IOCTL_3GPP2_GET_RDUD        =  DS_SYS_IOCTL_3GPP2_GET_RDUD,
  /**< 
      Retrieves whether reduced dormancy followed by unsolicited
      data is enabled or disabled.
  */ 
  PS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD = 
    DS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD,
  /**< 
      Sets the EV-DO slot cycle index.
  */  
  PS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD = 
    DS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD,
  /**< 
      Retrieves details about the EV-DO page monitoring period.
  */  
  PS_SYS_IOCTL_3GPP2_SET_FORCE_LONG_SLEEP =
    DS_SYS_IOCTL_3GPP2_SET_FORCE_LONG_SLEEP,
  /**< 
      Enables or disables the EV-DO force long sleep feature.
  */ 
  PS_SYS_IOCTL_3GPP2_GET_CALL_THROTTLE_INFO  = 
    DS_SYS_IOCTL_3GPP2_GET_CALL_THROTTLE_INFO,
  /**< 
      Queries whether the system is call throttled and returns
      the remaining throttled delay.
  */  

  /*3GPP2 specific ioctl internal to data should be added here. 
    Range: DS_SYS_IOCTL_3GPP2_MAX + 1 to DS_SYS_IOCTL_3GPP2_MAX + 99*/
  PS_SYS_IOCTL_3GPP2_INTERNAL_MIN    = DS_SYS_IOCTL_3GPP2_MAX + 1,
     
  PS_SYS_IOCTL_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_CMD = 
    PS_SYS_IOCTL_3GPP2_INTERNAL_MIN,
  /**< 
       User shall call this ioctl when the system moves to eHRPD and UE has
       LTE pdn contexts. User need to register for
       PS_SYS_EVENT_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_RSP. In response,
       User receives the event
       PS_SYS_EVENT_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_RSP.
       @see PS_SYS_EVENT_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_RSP
  */

  PS_SYS_IOCTL_3GPP2_INTERNAL_MAX   = DS_SYS_IOCTL_3GPP2_MAX + 99,

  /*Max number of 3GPP2 IOCTLs.Includes internal and external 3GPP2 IOCTLs*/
  PS_SYS_IOCTL_3GPP2_MAX            = PS_SYS_IOCTL_3GPP2_INTERNAL_MAX,

  /*3GPP specific ioctl common to QMI and DS SYS should be added within 
    the range PS_SYS_IOCTL_3GPP_MIN to PS_SYS_IOCTL_3GPP_MIN + 99
    3GPP specific ioctl applicable only to QMI should be added within
    the range PS_SYS_IOCTL_3GPP_MIN +100 to PS_SYS_IOCTL_3GPP_MIN +200*/
  PS_SYS_IOCTL_3GPP_MIN             = DS_SYS_IOCTL_3GPP_MIN,

  PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST = DS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST,
  /**< 
      Users can configure the lte attach pdn list stored in the persistent
      memory of the device anytime. The ioctl contains list of profile IDs
      in the decreasing order of priority. Successful write completion of
      the list triggers the event
      PS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE to be
      generated.
   
      @see PS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE
  */

  PS_SYS_IOCTL_3GPP_GET_LTE_ATTACH_PDN_LIST = DS_SYS_IOCTL_3GPP_GET_LTE_ATTACH_PDN_LIST,
  /**< 
      Users can query the lte attach pdn list stored in the persistent
      memory of the device anytime. The ioctl contains list of profile IDs
      in the decreasing order of priority. However, it was introduced for App 
      Processor to query the modem for the Attach PDN's list whenever it 
      wants to configure the new list using the IOCTL,
      PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST
   
      @see PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST
  */

  PS_SYS_IOCTL_3GPP_GET_LTE_MAX_ATTACH_PDN_NUM = DS_SYS_IOCTL_3GPP_GET_LTE_MAX_ATTACH_PDN_NUM,
  /**< 
      Users can query the maximum of number of attach pdn supported by
      the device anytime. However, it is mandatory for the client to query
      the number of attach pdn supported before it can set the new attach pdn
      list using PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST message. 

      @see PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST
  */

  PS_SYS_IOCTL_3GPP_NSAPI = DS_SYS_IOCTL_3GPP_NSAPI,
  /**< 
     User can query the NSAPI for a specified APN.

      @see DS_SYS_IOCTL_3GPP_NSAPI
  */
  
  PS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS = DS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS,
  /**< 
      User can query PDN name anytime however it was introduced for App 
      Processor to query the modem for the Attach PDN's name whenever it 
      gets notified about LTE system via the DATA_SYSTEM_STATUS notification. 
      In response, User should expect attach pdn name.
  */

   /* Add QMI specific 3gpp IOCTLs here. If any of the below IOCTL are
      to be made DS_SYS specific, define a new DS_SYS ioctl with the values
      defined here and change the PS_SYS ioctl definition to the newly
      defined corresponding DS_SYS ioctl.
      For example, we would define a PS_SYS_IOCTL_3GPP_SET_LTE_DATA_RETRY with
      the value DS_SYS_IOCTL_3GPP_MIN + 101 and change
      PS_SYS_IOCTL_3GPP_SET_LTE_DATA_RETRY = 
      DS_SYS_IOCTL_3GPP_SET_LTE_DATA_RETRY
  */

  PS_SYS_IOCTL_3GPP_SET_LTE_DATA_RETRY = PS_SYS_IOCTL_3GPP_MIN + 101,
  /**< 
      User can change the LTE data retry setting. It dynamicaly
      overrides the NV setting that controls the same behavior.
      If enabled, modem will do LTE data reattach on a different
      PDN when attach on current PDN fails. 
  */

  PS_SYS_IOCTL_3GPP_GET_LTE_DATA_RETRY = PS_SYS_IOCTL_3GPP_MIN + 102,
  /**< 
      User can get the current LTE data retry setting. 
  */

  /*3GPP specific ioctl internal to data should be added here. 
    Range: DS_SYS_IOCTL_3GPP_MAX + 1 to DS_SYS_IOCTL_3GPP_MAX + 99*/
  PS_SYS_IOCTL_3GPP_INTERNAL_MIN    = DS_SYS_IOCTL_3GPP_MAX + 1,
  PS_SYS_IOCTL_3GPP_LTE_ATTACH_INIT = PS_SYS_IOCTL_3GPP_INTERNAL_MIN,
  /**< 
       User invokes this IOCTL as a response to
       PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT, providing the info whether a
       context transfer is needed or not. In response, user can expect the
       Modehandler to bring up the ifaces if there are no contexts to be 
       transferred. If there are context to be transferred then the Modehandler 
       does nothing. Once response is recieved, user can bring up ifaces if 
       there are contexts to be transferred
  */

  PS_SYS_IOCTL_3GPP_INTERNAL_MAX   = DS_SYS_IOCTL_3GPP_MAX + 99,

  /*Max number of 3GPP IOCTLs.Includes internal and external 3GPP IOCTLs*/
  PS_SYS_IOCTL_3GPP_MAX            = PS_SYS_IOCTL_3GPP_INTERNAL_MAX,

  /*Common ioctl applicable to QMI and DS SYS should be added within 
    the range PS_SYS_IOCTL_COMMON_MIN to PS_SYS_IOCTL_COMMON_MIN + 99
    Common ioctl applicable only to QMI should be added within
    the range PS_SYS_IOCTL_COMMON_MIN +100 to PS_SYS_IOCTL_COMMON_MIN +200*/
  PS_SYS_IOCTL_COMMON_MIN        = DS_SYS_IOCTL_COMMON_MIN,
  PS_SYS_IOCTL_PDN_THROTTLE_INFO = DS_SYS_IOCTL_PDN_THROTTLE_INFO,
  /**< 
      @see DS_SYS_IOCTL_PDN_THROTTLE_INFO
  */

  PS_SYS_IOCTL_DUN_CTRL_RESPONSE = DS_SYS_IOCTL_DUN_CTRL_RESPONSE,
  /**< 
      @see DS_SYS_IOCTL_DUN_CTRL_RESPONSE
  */

  PS_SYS_IOCTL_MODEM_POWER_COST = DS_SYS_IOCTL_MODEM_POWER_COST,
  /**< 
      @see DS_SYS_IOCTL_MODEM_POWER_COST
  */

  PS_SYS_IOCTL_SET_APN_CALL_TYPE = DS_SYS_IOCTL_SET_APN_CALL_TYPE,
    /**< 
      @see DS_SYS_IOCTL_SET_APN_CALL_TYPE
  */

  PS_SYS_IOCTL_GET_GLOBAL_STATS = DS_SYS_IOCTL_GET_GLOBAL_STATS,
    /**< 
      @see DS_SYS_IOCTL_GET_GLOBAL_STATS
  */
  
  /*Common ioctl internal to data should be added here. 
    Range: DS_SYS_IOCTL_COMMON_MAX + 1 to DS_SYS_IOCTL_COMMON_MAX + 99*/
  PS_SYS_IOCTL_COMMON_INTERNAL_MIN     = DS_SYS_IOCTL_COMMON_MAX + 1,

  PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_SW  = PS_SYS_IOCTL_COMMON_INTERNAL_MIN,
  /**< 
      IOCTL switches the data path in the modem from HW to SW. Also disables setting up 
      of the A2 bridge
  */

  PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_HW  = PS_SYS_IOCTL_COMMON_INTERNAL_MIN + 1,
  /**< 
       IOCTL switches the data path in the modem from SW to HW, enables setting up 
       of the A2 bridge. If attached on LTE RAT, sets up A2 bridge for all RMNET calls. 
       User can request for in-oder delivery of packets.
  */

  PS_SYS_IOCTL_PDN_HO_THROTTLE    = PS_SYS_IOCTL_COMMON_INTERNAL_MIN + 2,
  /**< 
     IOCTL called when Handoff failed, and EPC decides to give up the 
     HO but not release PDN context on SRAT. 
     ps_sys_pdn_ho_throttle_info_type is the argval parameter to this IOCTL
  */
  
  PS_SYS_IOCTL_RELEASE_SIGNALLING_CONNECTION  = PS_SYS_IOCTL_COMMON_INTERNAL_MIN + 3,
  /**<
    This IOCTL is called when there is no Data Activity on all IFACES. This will indicate
    Mode Handler to Release the Signalling connection to NW as data activity is not present. 
   */
  
  PS_SYS_IOCTL_REMOTE_SOCKET_ALLOCATED  = PS_SYS_IOCTL_COMMON_INTERNAL_MIN + 4,
  /**<
    This IOCTL is called when TE responds to the PS_SYS_EVENT_REMOTE_SOCKET_REQUEST. 
   */
  
  PS_SYS_IOCTL_IS_CLAT_SUPPORTED_BY_PROFILE  = PS_SYS_IOCTL_COMMON_INTERNAL_MIN + 5,
  /**<
    This IOCTL is called by CLAT ACL to check if clat is supported from
    below network layer. IOCTL sends request to all technology modehandlers
    and if one of them replies positive for clat, it sends true    
   */   
  
  PS_SYS_IOCTL_COMMON_INTERNAL_MAX     = DS_SYS_IOCTL_COMMON_MAX + 99,

  /*Max number of 3GPP2 IOCTLs.Includes internal and external common IOCTLs*/
  PS_SYS_IOCTL_COMMON_MAX              = PS_SYS_IOCTL_COMMON_INTERNAL_MAX,

  /*WLAN ioctl Min*/
  PS_SYS_IOCTL_WLAN_MIN                = DS_SYS_IOCTL_WLAN_MIN,

  PS_SYS_IOCTL_SET_WQE_PROFILE_TYPE    = DS_SYS_IOCTL_SET_WQE_PROFILE_TYPE,
  /**< 
      @see DS_SYS_IOCTL_SET_WQE_PROFILE_TYPE
  */
  
  PS_SYS_IOCTL_SET_WQE_PROFILE_QUALITY_MEAS_INFO = DS_SYS_IOCTL_SET_WQE_PROFILE_QUALITY_MEAS_INFO,
  /**< 
      @see DS_SYS_IOCTL_SET_WQE_PROFILE_THRESHOLD_INFO
  */

  /*WLAN ioctl internal to data should be added here. 
    Range: DS_SYS_IOCTL_WLAN_MAX + 1 to DS_SYS_IOCTL_WLAN_MAX + 99*/
  
  PS_SYS_IOCTL_WLAN_INTERNAL_MIN     = DS_SYS_IOCTL_WLAN_MAX + 1,

  PS_SYS_IOCTL_WLAN_WIFI_MEAS_REPORT  = PS_SYS_IOCTL_WLAN_INTERNAL_MIN,
  /**< 
      IOCTL to send WiFI measurement parameters from AP to interested module
      eg. ANDSF
  */

  PS_SYS_IOCTL_WLAN_WIFI_MEAS_REPORT_CONFIG_STATUS  = PS_SYS_IOCTL_WLAN_INTERNAL_MIN + 1,
  /**< 
      IOCTL from AP to signal the status of the previously sent
      QMI_DSD_START_WIFI_MEAS_IND to interested module eg. ANDSF
  */

  PS_SYS_IOCTL_WLAN_NAT_KEEP_ALIVE_CONFIG_STATUS  = PS_SYS_IOCTL_WLAN_INTERNAL_MIN + 2,
  /**< 
      IOCTL from AP to signal the status of the previously sent
      PS_SYS_EVENT_NAT_KEEP_ALIVE_INFO to interested module eg. IWLAN
  */

  PS_SYS_IOCTL_WLAN_SET_WQE_PROFILE_QUALITY_STATUS = PS_SYS_IOCTL_WLAN_INTERNAL_MIN + 3,
  /**< 
      IOCTL from AP to signal the status of the WQE PRofile , eg: if the threshold
      criteria was met or not to interested module. eg. ANDSF.
  */

  PS_SYS_IOCTL_WLAN_INTERNAL_MAX     = DS_SYS_IOCTL_WLAN_MAX + 99,

  /*Max number of WLAN IOCTLs.Includes internal and external common IOCTLs*/
  PS_SYS_IOCTL_WLAN_MAX              = PS_SYS_IOCTL_WLAN_INTERNAL_MAX,
  
  PS_SYS_IOCTL_MAX
} ps_sys_ioctl_enum_type;

/**
  @brief Callback function to be returned for ioctl operation 
*/
typedef int16 (* ps_sys_ioctl_tech_cback_f_ptr_type)
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  int16                         * ps_errno
);

/**
  @brief Callback function to be returned for extended ioctl 
         operation
*/
typedef int16 (* ps_sys_ioctl_tech_ex_cback_f_ptr_type)
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  ps_sys_subscription_enum_type   subscription_id,
  int16                         * ps_errno
);

/**
  @brief Defines structure where diffent modehandlers can register their ioctl
         functions
*/
typedef struct
{
  ps_sys_ioctl_tech_cback_f_ptr_type      tech_f_ptr; /**< Modehandler specific
                                                           Callback */
  ps_sys_ioctl_tech_ex_cback_f_ptr_type   tech_ex_f_ptr; 
}ps_sys_tech_func_type;


/**
  @brief Defines type for PS_SYS_IOCTL_3GPP_LTE_ATTACH_INIT ioctl command. 
   
  @see   PS_SYS_IOCTL_3GPP_LTE_ATTACH_INIT
*/
typedef boolean ps_sys_ioctl_3gpp_lte_attach_init_type;

/**
  @brief Defines structure for PS_SYS_IOCTL_PDN_THROTTLE_INFO ioctl command.
   
  @see   PS_SYS_IOCTL_PDN_THROTTLE_INFO 
  @see   ps_sys_pdn_ctxt_throttle_info_type 
*/
typedef ps_sys_pdn_ctxt_throttle_info_type 
          ps_sys_ioctl_pdn_ctxt_throttle_info_type;
typedef ps_sys_pdn_throttle_info_type ps_sys_ioctl_pdn_throttle_info_type;
/**
  @brief Defines structure for PS_SYS_IOCTL_PDN_HO_THROTTLE 
    	 ioctl command.
   
  @see   PS_SYS_IOCTL_PDN_HO_THROTTLEs  
*/
typedef struct
{
  boolean                       is_ipv4_throttled;/**< ipv4 is throttled */
  boolean                       is_ipv6_throttled;/**< ipv6 is throttled */
  char                          apn_string[PS_SYS_MAX_APN_LEN]; /**< PDN name */
  uint16                        apn_string_len; /**< PDN name length */
  ps_iface_net_down_reason_type net_down_reason; /**< Net down reason*/
} ps_sys_pdn_ho_throttle_info_type;

/**
  @brief Defines structure for 
         PS_SYS_IOCTL_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_CMD ioctl command.
   
  @see   PS_SYS_IOCTL_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_CMD  
*/
typedef struct
{
  boolean all_pdn_ctxts_prereg; /**< true means all iface are prereg and 
                                     false means not all iface are prereg */
} ps_sys_ioctl_3gpp2_prereg_handoff_complete_cmd_type;

/**
  @brief Defines enum type representing different ip types
   
  @see   ps_sys_ioctl_3gpp_lte_attach_pdn_params_type 
*/
typedef enum
{
  PS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV4,     /**<  IPV4 type */
  PS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV6,     /**<  IPV4 type */
  PS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV4IPV6  /**<  IPV4 and IPV6 type */
} ps_sys_ioctl_ip_support_type_enum_type;

/**
  @brief Defines structure for PS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS ioctl 
         command.
   
  @see   PS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS 
*/
typedef struct
{
  ps_sys_ioctl_ip_support_type_enum_type ip_type; /** <  IP types */
  char  apn_string[PS_SYS_MAX_APN_LEN]; /**< Attached PDN name */
  uint8 apn_string_len;                /**<  Length of attached PDN name */
  boolean ota_attach_performed;        /**<  LTE OTA Attach state */
} ps_sys_ioctl_3gpp_lte_attach_pdn_params_type;

/**
  @brief Defines structure for PS_SYS_IOCTL_DUN_CTRL_RESPONSE ioctl command 
   
  @details Allows application like connection manager to tell modehandlers 
           whether to allow dun call. This is in response to event sent by 
           modehandlers for entitilement check or silen redial check
*/
typedef struct
{
  unsigned char         cb_data;
  boolean               allow_call;
} ps_sys_ioctl_dun_ctrl_response_type;

/**
  @brief Defines structure for PS_SYS_IOCTL_3GPP_SWITH_DATA_PATH_TO_HW ioctl command 
   
  set to request in-order delivery of data
*/
typedef uint32 ps_sys_ioctl_in_order_delivery_info;

/**
  @brief Refers to the no. of max lte attach pdns supported by 
         the device
   
  @see   PS_SYS_IOCTL_3GPP_GET_LTE_MAX_ATTACH_PDN_NUM
*/
typedef ds_sys_ioctl_3gpp_lte_max_attach_pdn_num_type ps_sys_ioctl_3gpp_lte_max_attach_pdn_num_type;
/**
  @brief This is the type that is used for 
         PS_SYS_IOCTL_3GPP_SET_LTE_DATA_RETRY and
         PS_SYS_IOCTL_3GPP_GET_LTE_DATA_RETRY
  
*/
typedef struct
{
  boolean lte_data_retry;
} ps_sys_ioctl_lte_data_retry_type;

typedef ds_sys_modem_power_cost_enum_type ps_sys_modem_power_cost_enum_type;

/**
  @brief Enum describing different data session
  @see   ps_sys_ioctl_3gpp2_pre_dormancy_sys_info_type
*/
typedef enum
{
  PS_SYS_IOCTL_DATA_SESSION_NO_SERVICE  = 0,
  PS_SYS_IOCTL_DATA_SESSION_NETWORK_1X  = 2,
  PS_SYS_IOCTL_DATA_SESSION_NETWORK_HDR = 4
} ps_sys_ioctl_data_session_network_enum_type;

/**
  @brief Data type to be used with 
         PS_SYS_IOCTL_3GPP2_PRE_DORMANCY_SYS_INFO
  @see   PS_SYS_IOCTL_3GPP2_PRE_DORMANCY_SYS_INFO
*/
typedef struct
{
  uint16                                       service_option;
  ps_sys_ioctl_data_session_network_enum_type  data_session_network;
} ps_sys_ioctl_3gpp2_pre_dormancy_sys_info_type;

/**
  @brief This is the type that is used with 
         PS_SYS_IOCTL_3GPP2_SET_CAM_TIMER/
         PS_SYS_IOCTL_3GPP2_GET_CAM_TIMER 
  @see   PS_SYS_IOCTL_3GPP2_SET_CAM_TIMER/ 
         PS_SYS_IOCTL_3GPP2_GET_CAM_TIMER 
*/
typedef ds_sys_ioctl_3gpp2_cam_timer_type ps_sys_ioctl_3gpp2_cam_timer_type;

/**
  @brief This is the type that is used with 
         PS_SYS_IOCTL_3GPP2_SET_SCRM/
         PS_SYS_IOCTL_3GPP2_GET_SCRM
  @see   PS_SYS_IOCTL_3GPP2_SET_SCRM/ 
         PS_SYS_IOCTL_3GPP2_GET_SCRM
                                      */
typedef ds_sys_ioctl_3gpp2_scrm_type ps_sys_ioctl_3gpp2_scrm_type;

/**
  @brief This is the type that is used with 
         PS_SYS_IOCTL_3GPP2_FORCE_LONG_SLEEP
  @see   PS_SYS_IOCTL_3GPP2_FORCE_LONG_SLEEP
*/
typedef ds_sys_ioctl_3gpp2_force_long_sleep_type ps_sys_ioctl_3gpp2_force_long_sleep_type;

/**
  @brief This is the type that is used with 
         PS_SYS_IOCTL_3GPP2_SET_RDUD/
         PS_SYS_IOCTL_3GPP2_GET_RDUD
  @see   PS_SYS_IOCTL_3GPP2_SET_RDUD/ 
         PS_SYS_IOCTL_3GPP2_GET_RDUD
                                      */
typedef ds_sys_ioctl_3gpp2_rdud_type ps_sys_ioctl_3gpp2_rdud_type;

/**
  @brief Callback function provided by client which is to be called when
         evdo set page monitor is triggered
   
  @param[in] evt              Event
  @param[in] failure_code     Failure code
  
  @see PS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD  
*/

/**
  @brief Data type used in ds_sys_ioctl_3gpp2_page_monitor_type
  @see   ps_sys_ioctl_3gpp2_page_monitor_type
*/
typedef uint8 ps_sys_ioctl_3gpp2_page_monitor_period_type;
/**
  @brief Data type to be used with 
         PS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD/
         PS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD 
         
  @see 
         PS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD/
         PS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD
*/
typedef struct
{
  ps_sys_ioctl_3gpp2_page_monitor_period_type       page_monitor_period;
  void (* ps_sys_ioctl_set_evdo_pmp_cb) (int16   evt, 
                                         ps_hdr_slotted_mode_failure_code_enum_type*    failure_code);
} ps_sys_ioctl_3gpp2_set_page_monitor_type;

/**
  @brief Data type to be used with 
         PS_SYS_IOCTL_3GPP2_GET_CALL_THROTTLE_INFO
         
  @see 
         PS_SYS_IOCTL_3GPP2_GET_CALL_THROTTLE_INFO
*/
typedef void (* ps_sys_ioctl_call_thr_info_hdlr_cb) (void*, int16);

typedef struct
{
  ps_sys_ioctl_call_thr_info_hdlr_cb  call_thro_info_f_ptr;
} ps_sys_ioctl_3gpp2_call_thr_info_hdlr_cb_type;

/**
  @brief Data type to be used with PS_SYS_IOCTL_NSAPI
*/
typedef ds_sys_ioctl_3gpp_nsapi_type ps_sys_ioctl_3gpp_nsapi_type;

/**
  @brief Data type to be used with 
         PS_SYS_IOCTL_SET_APN_CALL_TYPE
*/
typedef ds_sys_ioctl_apn_call_type ps_sys_ioctl_apn_call_type;

/**
  @brief Data type to be used with 
         PS_SYS_IOCTL_WLAN_NAT_KEEP_ALIVE_CONFIG_STATUS
*/
typedef enum 
{
  PS_SYS_WLAN_NAT_KEEP_ALIVE_CONFIG_OK, 
  /**< Configuration OK \n */
  PS_SYS_WLAN_NAT_KEEP_ALIVE_CONFIG_ERROR, 
  /**< Configuration error \n */
} ps_sys_wlan_nat_keep_alive_config_status_enum_type;

typedef struct
{
  ps_sys_wlan_nat_keep_alive_config_status_enum_type status;
} ps_sys_wlan_nat_keep_alive_config_type;

/**
  @brief Data type to be used with PS_SYS_IOCTL_GET_GLOBAL_STATS 
*/
typedef ds_sys_ioctl_global_stats_type ps_sys_ioctl_global_stats_type;


/**
  @brief Data type to be used with 
         PS_SYS_IOCTL_WLAN_SET_WQE_PROFILE_QUALITY_STATUS
*/
typedef ds_sys_wlan_set_wqe_profile_status_type ps_sys_wlan_set_wqe_profile_status_type;

/**
  @brief Data type to be used with 
         PS_SYS_IOCTL_IS_CLAT_SUPPORTED_BY_PROFILE
  @see   PS_SYS_IOCTL_IS_CLAT_SUPPORTED_BY_PROFILE
*/
typedef struct
{
  acl_policy_info_type   * acl_policy_info_ptr;
  boolean                  is_clat_supported;
} ps_sys_ioctl_clat_support_type;

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/**
  @brief  This is PS Sys level API for ioctl. This function performs the
          operation for particular ioctl

  @see    ds_sys_ioctl 
*/
int16 ps_sys_ioctl
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  int16                         * ps_errno
);


/**
  @brief  This is PS Sys level API for ioctl. This function performs the
          operation for requested ioctl for particular technology and 
          subscription

  @see    ds_sys_ioctl_ex 
*/
int16 ps_sys_ioctl_ex
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_ioctl_enum_type          ioctl_name,
  ps_sys_subscription_enum_type   subscription_id,
  void                          * arg_val_ptr,
  int16                         * ps_errno
);


/**
  @brief  This function allows client to register callback for of particular
          technology 

  @param[in] tech_type            States Technology type
  @param[in] tech_cback_f_ptr     Callback function 
  @param[out] ps_errno            DS_EFAULT if parameter is invalid
                                  DS_EINVAL if operation is invalid
                                  DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure. 
   
  @code
 
  Example
  3GPP modehandler registers for ioctl callback as follow 
   
  int16  ioctl_3gpp_cback(...)
  {
    .....
  }
 
  ps_sys_ioctl_lte_attach_pdn_params_type * ioctl_info;
 
  ret_val =  ps_sys_ioctl_register_tech_cback(PS_SYS_TECH_3GPP,
                                              &ioctl_3gpp_cback,
                                              &ps_errno);
 
  if(0 > ret_val)
  {
    LOG_MSG_ERROR_1("Callback reg failed, err %d", ps_errno);
  }
   
  @endcode 
*/
int16 ps_sys_ioctl_register_tech_cback
(
  ps_sys_tech_enum_type                 tech_type,
  ps_sys_ioctl_tech_cback_f_ptr_type    tech_cback_f_ptr,
  int16                               * ps_errno
);


/**
  @brief  This function allows client to register callback for particular
          technology and subscription to perform given Ioctl operation

  @param[in] tech_type            States Technology type
  @param[in] tech_cback_f_ptr     Callback function 
  @param[in] subscription_id      Subscription Id
  @param[out] ps_errno            DS_EFAULT if parameter is invalid
                                  DS_EINVAL if operation is invalid
                                  DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure. 
   
  @code
 
  Example
  3GPP modehandler registers for ioctl callback as follow 
   
  int16  ioctl_3gpp_cback(...)
  {
    .....
  }
 
  ps_sys_ioctl_lte_attach_pdn_params_type * ioctl_info;
 
  ret_val =  ps_sys_ioctl_register_tech_cback(PS_SYS_TECH_3GPP,
                                              PS_SYS_SECONDARY_SUBS,
                                              &ioctl_3gpp_cback,
                                              &ps_errno);
 
  if(0 > ret_val)
  {
    LOG_MSG_ERROR("Callback reg failed, err %d",ps_errno,0,0);
  }
   
  @endcode 
*/
int16 ps_sys_ioctl_register_tech_cback_ex
(
  ps_sys_tech_enum_type                    tech_type,
  ps_sys_ioctl_tech_ex_cback_f_ptr_type    tech_cback_ex_f_ptr,
  ps_sys_subscription_enum_type            subscription_id,  
  int16                                  * ps_errno
);

/**
  @brief  This function allows client to register callback for 
          particular common IOCTL and subscription to perform
          given Ioctl operation. The handler will be invoked if
          client calls the IOCTL with tech type as
          PS_SYS_TECH_ALL
   
  @param[in] ioctl_name           States IOCTL name
  @param[in] tech_cback_f_ptr     Callback function 
  @param[in] subscription_id      Subscription Id
  @param[out] ps_errno            DS_EFAULT if parameter is invalid
                                  DS_EINVAL if operation is invalid
                                  DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure. 
   
  @code
 
  Example 
  Tech agnostic modehandler registers for ioctl callback as 
  follow 
   
  int16  ioctl_tech_agnostic_cback(...)
  {
    .....
  }
 

  ret_val = 
           ps_sys_ioctl_register_ioctl_cback(PS_SYS_IOCTL_PDN_THROTTLE_INFO,
           PS_SYS_SECONDARY_SUBS, &ioctl_tech_agnostic_cback,
           &ps_errno);
 
  if(0 > ret_val)
  {
    LOG_MSG_ERROR_1("Callback reg failed, err %d",ps_errno);
  }
   
  @endcode 
*/
int16 ps_sys_ioctl_register_ioctl_cback_ex
(
  ps_sys_ioctl_enum_type                   ioctl_name,
  ps_sys_ioctl_tech_ex_cback_f_ptr_type    tech_cback_ex_f_ptr,
  ps_sys_subscription_enum_type            subscription_id,  
  int16                                  * ps_errno
);

/**
  @brief  This API initializes default ioctl handlers for ioctls which
          are handled at ps sys level

  @return  None. 
*/
void ps_sys_ioctl_register_common_cbacks
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* PS_SYS_IOCTL_H */

