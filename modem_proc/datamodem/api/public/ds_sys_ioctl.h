/**
@file ds_sys_ioctl.h
@brief
This file defines functions, enums and structs used for two modehandlers to 
communicate with each other through ioctls 
*/

#ifndef DS_SYS_IOCTL_H
#define DS_SYS_IOCTL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ I O C T L . H


GENERAL DESCRIPTION
  This file defines functions, enums and structs used for two modehandlers
  to communicate with each other through ioctls

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_sys_ioctl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/09/11    bvd    Created module

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
  @brief Defines different ioctl types
*/
typedef enum
{
  DS_SYS_IOCTL_MIN         = 0x00,

  /*3GPP2 specific ioctl*/
  DS_SYS_IOCTL_3GPP2_MIN         = DS_SYS_IOCTL_MIN,
  DS_SYS_IOCTL_3GPP2_PRE_DORMANCY_SYS_INFO  =  DS_SYS_IOCTL_3GPP2_MIN + 1,
  /**< 
      This command retrieves the network and service option information for the 
      3GPP2 data session before going into dormancy.
      If the device is not in a 3GPP2 data session, the
      service option is set to the NULL service option (0xFFFF) and the
      data session network is set to no service (0x00).
      If the device is not in a CDMA-1X data session, the service option is
      set to the NULL service option (0xFFFF)
  */
  DS_SYS_IOCTL_3GPP2_SET_CAM_TIMER          =  DS_SYS_IOCTL_3GPP2_MIN + 2,
  /**< 
      This command sets the Chatty App Manager timer value.
      This timer is used for CDMA-1X calls only.
      The CAM timer value can be changed at any point for 3GPP2 devices. If set
      during an active CDMA-1X call, the new value is used in the subsequent
      1X data call, and does not affect the current call.
      The default value of the CAM timer is set to 300 seconds. A value of 0 
      causes the timer to be disabled
  */
  DS_SYS_IOCTL_3GPP2_GET_CAM_TIMER          =  DS_SYS_IOCTL_3GPP2_MIN + 3,
  /**< 
      This command queries the Chatty App Manager timer setting for the device.
  */  
  DS_SYS_IOCTL_3GPP2_SET_SCRM               =  DS_SYS_IOCTL_3GPP2_MIN + 4,
  /**< 
      Disables/enables the Supplemental Channel Request Message (SCRM).
      The SCRM setting can be changed at any point for 3GPP2 devices. If set
      during an active CDMA-1X call, the new value is used in the subsequent
      1X data call, and does not effect the current call.
  */  
  DS_SYS_IOCTL_3GPP2_GET_SCRM               =  DS_SYS_IOCTL_3GPP2_MIN + 5,
  /**< 
      This command queries whether the SCRM is enabled or disabled for the device.
  */  
  DS_SYS_IOCTL_3GPP2_SET_RDUD               =  DS_SYS_IOCTL_3GPP2_MIN + 6,
  /**< 
      Enables or disables reduced dormancy followed by unsolicited data.
  */  
  DS_SYS_IOCTL_3GPP2_GET_RDUD               =  DS_SYS_IOCTL_3GPP2_MIN + 7,
  /**< 
      Retrieves whether reduced dormancy followed by unsolicited
      data is enabled or disabled.
  */  
  DS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD  =  DS_SYS_IOCTL_3GPP2_MIN + 8,
  /**< 
      Sets the EV-DO slot cycle index.
  */  
  DS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD  =  DS_SYS_IOCTL_3GPP2_MIN + 9,
  /**< 
      Retrieves details about the EV-DO page monitoring period.
  */  
  DS_SYS_IOCTL_3GPP2_SET_FORCE_LONG_SLEEP     =  DS_SYS_IOCTL_3GPP2_MIN + 10,
  /**< 
      Enables or disables the EV-DO force long sleep feature.
  */  
  DS_SYS_IOCTL_3GPP2_GET_CALL_THROTTLE_INFO   =  DS_SYS_IOCTL_3GPP2_MIN + 11,
  /**< 
      Queries whether the system is call throttled and returns
      the remaining throttled delay.
  */  

  DS_SYS_IOCTL_3GPP2_MAX         = 200,

  /*3GPP specific ioctl*/
  DS_SYS_IOCTL_3GPP_MIN             = 300,
  DS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST  = DS_SYS_IOCTL_3GPP_MIN + 1,
  /**< 
      Users can configure the lte attach pdn list stored in the persistent
      memory of the device anytime. The ioctl contains list of profile IDs
      in the decreasing order of priority. Successful write completion of
      the list triggers the event
      DS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE to be
      generated.
   
      @see DS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE
  */
  DS_SYS_IOCTL_3GPP_GET_LTE_ATTACH_PDN_LIST = DS_SYS_IOCTL_3GPP_MIN + 2,
  /**< 
      Users can query the lte attach pdn list stored in the persistent
      memory of the device anytime. The ioctl contains list of profile IDs
      in the decreasing order of priority. However, it was introduced for App 
      Processor to query the modem for the Attach PDN's list whenever it 
      wants to configure the new list using the IOCTL,
      DS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST
   
      @see DS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST
  */
  DS_SYS_IOCTL_3GPP_GET_LTE_MAX_ATTACH_PDN_NUM = DS_SYS_IOCTL_3GPP_MIN + 3,
  /**< 
      Users can query the maximum of number of attach pdn supported by
      the device anytime.However, it was introduced for App 
      Processor to query the modem for the number of attach pdn supported
      before it can set the new attach pdn list using
      DS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST message.

      @see DS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST
  */

  DS_SYS_IOCTL_3GPP_NSAPI = DS_SYS_IOCTL_3GPP_MIN + 4,
  /**<
       User can query the NSAPI for a specified APN.
  */
  DS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS = DS_SYS_IOCTL_3GPP_MIN + 100,
  /**< 
      User can query PDN name anytime however it was introduced for App 
      Processor to query the modem for the Attach PDN's name whenever it 
      gets notified about LTE system via the DATA_SYSTEM_STATUS notification. 
      In response, User should expect attach pdn name.
  */

  DS_SYS_IOCTL_3GPP_MAX             = 500,

  /*Common ioctl*/
  DS_SYS_IOCTL_COMMON_MIN        = 600,
  DS_SYS_IOCTL_PDN_THROTTLE_INFO = DS_SYS_IOCTL_COMMON_MIN,
  /**< 
      User shall call this IOCTL whenever they want to get PDN throttle info.
      This IOCTL will get the same info as DS_SYS_EVENT_PDN_THROTTLE_INFO.
      In response, User gets to know which PDN is throttled right now and what
      is the remaining time for each PDN throttling. Once response is
      recieved, user gets to know which PDN is throttled and only make the
      call for the PDN which is not throttled
  */

  DS_SYS_IOCTL_DUN_CTRL_RESPONSE = DS_SYS_IOCTL_COMMON_MIN + 1,
  /**< 
       Allows application like connection manager to tell modehandlers 
       whether to allow dun call for particular call id. User shall call this
       ioctl after it recieves response from event sent by modehandlers for
       entitilement check or silent redial check
  */

  DS_SYS_IOCTL_MODEM_POWER_COST = DS_SYS_IOCTL_COMMON_MIN + 2,
  /**< 
       Allows application to query for the current modem power cost.
       Power consumption is in general monotonically increasing with
       total transmit power from device to network. The higher the total
       transmit power, the higher the penalty in power.
       By determining the power cost, the application can choose to do
       some of the not-so-urgent tasks(such as background updates) when
       the penalty is low.
  */

  DS_SYS_IOCTL_SET_APN_CALL_TYPE = DS_SYS_IOCTL_COMMON_MIN + 3,
  /**< 
       Allows application to set the call type of a particular apn.
       User shall call this ioctl when there is a change in the call type for
       the apn*/

  DS_SYS_IOCTL_GET_GLOBAL_STATS = DS_SYS_IOCTL_COMMON_MIN + 4,
  /**< 
      Allows applications to get the global data statistics which contains the
      total packet count including both uplink and downlink on all RAT's
      Stats gets reset when it reaches data type limit
  */


  DS_SYS_IOCTL_COMMON_MAX        = 800,

  /*WLAN ioctl*/
  DS_SYS_IOCTL_WLAN_MIN        = 900,

  DS_SYS_IOCTL_SET_WQE_PROFILE_TYPE = DS_SYS_IOCTL_WLAN_MIN + 1,
    /**< 
       Allows application to set the Wifi quality estimation profile
       type.
    */


  DS_SYS_IOCTL_SET_WQE_PROFILE_QUALITY_MEAS_INFO = DS_SYS_IOCTL_WLAN_MIN + 2,
    /**< 
       Allows application to set the quality measurement parameters for
       Wifi quality estimation profile type.
    */

  DS_SYS_IOCTL_WLAN_MAX        = 1100,

  DS_SYS_IOCTL_MAX
} ds_sys_ioctl_enum_type;

/**
  @brief Defines structure for DS_SYS_IOCTL_PDN_THROTTLE_INFO ioctl command 
  @see DS_SYS_IOCTL_PDN_THROTTLE_INFO
*/
typedef struct
{
  boolean    is_ipv4_throttled;             /**< ipv4 is throttled */
  boolean    is_ipv6_throttled;             /**< ipv6 is throttled */
  uint32     remaining_ipv4_throttled_time; /**< ipv4 remaining time in 
                                                 miliseconds */
  uint32     remaining_ipv6_throttled_time; /**< ipv6 remaining time in 
                                                 miliseconds */
  char       apn_string[DS_SYS_MAX_APN_LEN]; /**< PDN name */
  uint8      apn_string_len;                 /**< PDN name length */
} ds_sys_ioctl_pdn_ctxt_throttle_info_type;

typedef struct
{
  
  ds_sys_ioctl_pdn_ctxt_throttle_info_type pdn_throttle_info[DS_SYS_MAX_APNS];
  /**< For each internet, we have the values ie max apns */
 
  uint8 num_apn;
  /**< Refers to the no. of valid array elements in pdn_throttle_info[] */
} ds_sys_ioctl_pdn_throttle_info_type;


/**
  @brief Defines structure for DS_SYS_IOCTL_DUN_CALL_PREF_INFO ioctl command 
   
  @see  DS_SYS_IOCTL_DUN_CTRL_RESPONSE
*/
typedef struct
{
  unsigned char         cb_data;     /**< Callback data sent by Modehandler */
  boolean               allow_call;  /**< Allow or disallow call for callid */
} ds_sys_ioctl_dun_ctrl_response_type;

/**
  @brief Refers to the no. of max lte attach pdns supported by 
         the device
   
  @see   DS_SYS_IOCTL_3GPP_GET_LTE_MAX_ATTACH_PDN_NUM
*/
typedef uint8 ds_sys_ioctl_3gpp_lte_max_attach_pdn_num_type;

/**
  @brief Defines enum type representing different ip types
   
  @see   ds_sys_ioctl_3gpp_lte_attach_pdn_params_type 
*/
typedef enum
{
  DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV4,     /**<  IPV4 type */
  DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV6,     /**<  IPV4 type */
  DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV4IPV6  /**<  IPV4 and IPV6 type */
} ds_sys_ioctl_ip_support_type_enum_type;

/**
  @brief Defines structure for DS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS ioctl 
         command.
   
  @see   DS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS 
*/
typedef struct
{
  ds_sys_ioctl_ip_support_type_enum_type ip_type; /** <  IP types */
  char  apn_string[DS_SYS_MAX_APN_LEN]; /**< Attached PDN name */
  uint8 apn_string_len;                /**<  Length of attached PDN name */
  boolean ota_attach_performed;        /**<  LTE OTA Attach state */
} ds_sys_ioctl_3gpp_lte_attach_pdn_params_type;

/**
  @brief Enumeration for the modem power cost
   
  @see   DS_SYS_IOCTL_MODEM_POWER_COST
*/

typedef enum {
  DS_SYS_MODEM_POWER_COST_UNKNOWN,
  /**< Not supported */
  DS_SYS_MODEM_POWER_COST_LOW,         
  /**< Low */
  DS_SYS_MODEM_POWER_COST_MEDIUM,      
  /**< Medium */
  DS_SYS_MODEM_POWER_COST_HIGH     
  /**< High */
} ds_sys_modem_power_cost_enum_type;

/**
  @brief Enum describing different data session
  @see   ds_sys_ioctl_3gpp2_pre_dormancy_sys_info_type
*/
typedef enum
{
  DS_SYS_IOCTL_DATA_SESSION_NO_SERVICE  = 0,
  DS_SYS_IOCTL_DATA_SESSION_NETWORK_1X  = 2,
  DS_SYS_IOCTL_DATA_SESSION_NETWORK_HDR = 4
} ds_sys_ioctl_data_session_network_enum_type;

/**
  @brief Data type to be used with 
         DS_SYS_IOCTL_3GPP2_PRE_DORMANCY_SYS_INFO
  @see   DS_SYS_IOCTL_3GPP2_PRE_DORMANCY_SYS_INFO
*/
typedef struct
{
  uint16                                      service_option;
  ds_sys_ioctl_data_session_network_enum_type  data_session_network;
} ds_sys_ioctl_3gpp2_pre_dormancy_sys_info_type;

/**
  @brief This is the type that is used with 
         DS_SYS_IOCTL_3GPP2_SET_CAM_TIMER/
         DS_SYS_IOCTL_3GPP2_GET_CAM_TIMER 
  @see   DS_SYS_IOCTL_3GPP2_SET_CAM_TIMER/ 
         DS_SYS_IOCTL_3GPP2_GET_CAM_TIMER 
*/
typedef uint32 ds_sys_ioctl_3gpp2_cam_timer_type;

/**
  @brief This is the type that is used with 
         DS_SYS_IOCTL_3GPP2_SET_SCRM/
         DS_SYS_IOCTL_3GPP2_GET_SCRM 
  @see   DS_SYS_IOCTL_3GPP2_SET_SCRM/ 
         DS_SYS_IOCTL_3GPP2_GET_SCRM 
*/
typedef boolean ds_sys_ioctl_3gpp2_scrm_type;

/**
  @brief This is the type that is used with 
         DS_SYS_IOCTL_3GPP2_FORCE_LONG_SLEEP
  @see   DS_SYS_IOCTL_3GPP2_FORCE_LONG_SLEEP
*/
typedef boolean ds_sys_ioctl_3gpp2_force_long_sleep_type;


/**
  @brief This is the type that is used with 
         DS_SYS_IOCTL_3GPP2_SET_RDUD/
         DS_SYS_IOCTL_3GPP2_GET_RDUD 
  @see   DS_SYS_IOCTL_3GPP2_SET_RDUD/ 
         DS_SYS_IOCTL_3GPP2_GET_RDUD
*/
typedef boolean ds_sys_ioctl_3gpp2_rdud_type;


typedef enum
{
  DS_SYS_IOCTL_HDR_SLOTTED_MODE_REQUEST_REJECTED = 0, /* Request to change slot
                                                         cycle index rejected        */
  DS_SYS_IOCTL_HDR_SLOTTED_MODE_REQUEST_FAILED_TX,    /* Failed transmission         */
  DS_SYS_IOCTL_HDR_SLOTTED_MODE_NOT_SUPPORTED,        /* Slotted mode not supported  */
  DS_SYS_IOCTL_HDR_SLOTTED_MODE_NO_NET,               /* Not idle or connected       */
  DS_SYS_IOCTL_HDR_SLOTTED_MODE_FAILURE_REASON_MAX    /* DO NOT USE. Used for
                                               bounds check                */
} ds_sys_ioctl_hdr_slotted_mode_failure_code_enum_type;

/**
  @brief Callback function provided by client which is to be called when
         evdo set page monitor is triggered
   
  @param[in] evt              Event
  @param[in] failure_code     Failure code
  
  @see DS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD  
*/
typedef void (* ds_sys_ioctl_set_evdo_pmp_cb)
(
  int16                                                            evt,
  ds_sys_ioctl_hdr_slotted_mode_failure_code_enum_type*    failure_code
);
/**
  @brief Data type used in ds_sys_ioctl_3gpp2_page_monitor_type
  @see   ds_sys_ioctl_3gpp2_page_monitor_type
*/
typedef uint8 ds_sys_ioctl_3gpp2_page_monitor_period_type;
/**
  @brief Data type to be used with 
         DS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD/
         DS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD 
         
  @see 
         DS_SYS_IOCTL_3GPP2_SET_PAGE_MONITOR_PERIOD/
         DS_SYS_IOCTL_3GPP2_GET_PAGE_MONITOR_PERIOD
*/
typedef struct
{
  ds_sys_ioctl_3gpp2_page_monitor_period_type       page_monitor_period;
  ds_sys_ioctl_set_evdo_pmp_cb                      evdo_pmp_cback_f_ptr;
} ds_sys_ioctl_3gpp2_set_page_monitor_type;

typedef struct
{
  ds_sys_ioctl_3gpp2_page_monitor_period_type       slotted_mode_option;
  boolean                                           long_sleep_enabled;
} ds_sys_ioctl_3gpp2_get_page_monitor_type;

/**
  @brief Data type to be used with 
         DS_SYS_IOCTL_3GPP2_GET_CALL_THROTTLE_INFO
         
  @see 
         DS_SYS_IOCTL_3GPP2_GET_CALL_THROTTLE_INFO
*/
typedef void (* ds_sys_ioctl_call_thr_info_hdlr_cb) (void*, int16);

typedef struct
{
  ds_sys_ioctl_call_thr_info_hdlr_cb  call_thro_info_f_ptr;
} ds_sys_ioctl_3gpp2_call_thr_info_hdlr_cb_type;

/**
  @brief Data type to be used with DS_SYS_IOCTL_NSAPI
*/
typedef struct
{
  byte            apn[DS_SYS_MAX_APN_LEN];
  uint8           nsapi[DS_SYS_MAX_BEARER];
  uint8           num_nsapi;
} ds_sys_ioctl_3gpp_nsapi_type;

/**
  @brief Data type to be used with DS_SYS_IOCTL_APN_CALL_TYPE
*/

#define DS_SYS_APN_CALL_TYPE_IDLE           0x0000000000000000
  /**< Idle */
#define DS_SYS_APN_CALL_TYPE_GENERAL        0x0000000000000001
  /**< General */
#define DS_SYS_APN_CALL_TYPE_VOICE          0x0000000000000002
  /**< Voice call */
#define DS_SYS_APN_CALL_TYPE_VIDEO          0x0000000000000004
  /**< Video call */

typedef struct
{
  byte            apn[DS_SYS_MAX_APN_LEN];
  uint64          apn_call_type_mask;
} ds_sys_ioctl_apn_call_type;

typedef struct
{
  uint64 global_pkt_cnt;
} ds_sys_ioctl_global_stats_type;

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/**
  @brief  This function performs the operation for particular ioctl

  @param[in] tech_type            States Technology type
  @param[in] ioctl_name           Operation name
  @param[in] arg_val_ptr          Pointer to operation specific
  @param[out] dss_errno           Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure.
 
  @code
 
  Example
  Connection manager can tell modehandlers whether to allow dun call for
  particular call id  or not.
 
  ds_sys_ioctl_dun_ctrl_response_type * ioctl_info;
  ioctl_info->call_id = xxx;
  ioctl_info->allow_call = TRUE;
 
  ret_val =  ps_sys_ioctl(DS_SYS_TECH_3GPP,
                          DS_SYS_IOCTL_DUN_CTRL_RESPONSE,
                          ioctl_info,
                          &ds_errno);
  if(0 > ret_val)
  {
    LOG_MSG_ERROR("Ioctl failed, err %d", ds_errno,0,0);
  }
 
  @endcode
*/
int16 ds_sys_ioctl
(
  ds_sys_tech_enum_type           tech_type,
  ds_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  int16                         * dss_errno
);

/**
  @brief  This function performs the operation for requested Ioctl 
          of a particular technology and subscription.

  @param[in] tech_type            States Technology type
  @param[in] ioctl_name           Operation name
  @param[in] subscription_id      Subscription Id
  @param[in] arg_val_ptr          Pointer to operation specific
  @param[out] dss_errno           Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure.
 
  @code
 
  Example
  Connection manager can tell modehandlers whether to allow dun call for
  particular call id  or not.
 
  ds_sys_ioctl_dun_ctrl_response_type * ioctl_info;
  ioctl_info->call_id = xxx;
  ioctl_info->allow_call = TRUE;
 
  ret_val =  ds_sys_ioctl_ex(DS_SYS_TECH_3GPP,
                             DS_SYS_IOCTL_DUN_CTRL_RESPONSE,
                             DS_SYS_SECONDARY_SUBS,
                             ioctl_info,
                             &ds_errno);
  if(0 > ret_val)
  {
    LOG_MSG_ERROR("Ioctl failed, err %d", ds_errno,0,0);
  }
 
  @endcode
*/
int16 ds_sys_ioctl_ex
(
  ds_sys_tech_enum_type           tech_type,
  ds_sys_ioctl_enum_type          ioctl_name,
  ds_sys_subscription_enum_type   subscription_id,
  void                          * arg_val_ptr,
  int16                         * dss_errno
);


#ifdef __cplusplus
}
#endif

#endif /* DS_SYS_IOCTL_H */
