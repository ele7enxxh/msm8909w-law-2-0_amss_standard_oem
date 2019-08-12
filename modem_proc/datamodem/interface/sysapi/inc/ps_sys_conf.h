/**
@file ps_sys_conf.h
@brief
This file defines functions, enums and structs used to read and update 
technology configurations 
*/

#ifndef PS_SYS_CONF_H
#define PS_SYS_CONF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ C O N F . H


GENERAL DESCRIPTION
  This file defines functions, enums and structs used to read and update
  technology configurations

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/inc/ps_sys_conf.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/10/12    msh    moved an auth status to modem level api 
03/10/12    msh    Added changes for PS_SYS_CONF_3GPP2_AN_AUTH_STATUS  
09/09/11    bvd    Created module 
 
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ps_svc.h"
#include "ps_sys.h"
#include "ds_sys_conf.h"
#include "ps_sys_event.h"


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/
/**
  @brief Definition of various interface ctrl operations. Please do NOT
         change the values of the existing enumerated types when adding
         new ones (to ensure that object code released with previous
         versions of the code do not break). Each of the following ctrl
         operations have associated tyepdef (with corresponding names)
         that are passed in or returned for get/set operations.
*/
#define   PS_SYS_CONF_MAX_APN_STRING_LEN      DS_SYS_CONF_MAX_APN_STRING_LEN 
/**< PDN name str length */
#define   PS_SYS_CONF_MAX_NUM_BEARER_CONTEXT  DS_SYS_CONF_MAX_NUM_BEARER_CONTEXT
 /**< MAX PDN */

/**
  @brief Defines different Configuration types
*/
typedef enum
{
  /* Conf common to both DS and PS layer*/
  PS_SYS_CONF_MIN                        = DS_SYS_CONF_MIN,

  /*---------------------------------------------------------------------------
      3GPP2 CONF VALUES
  ---------------------------------------------------------------------------*/
  /*3GPP2 conf */
  PS_SYS_CONF_3GPP2_CONF_MIN               = DS_SYS_CONF_3GPP2_CONF_MIN,
  PS_SYS_CONF_3GPP2_PRE_DORMANCY_SYS_INFO  = 
    DS_SYS_CONF_3GPP2_PRE_DORMANCY_SYS_INFO,
  /** 
   @see    DS_SYS_CONF_3GPP2_PRE_DORMANCY_SYS_INFO
  */

  PS_SYS_CONF_3GPP2_PAGE_MONITOR_PERIOD    = 
    DS_SYS_CONF_3GPP2_PAGE_MONITOR_PERIOD,
  /** 
   @see    DS_SYS_CONF_3GPP2_PAGE_MONITOR_PERIOD
  */

  PS_SYS_CONF_3GPP2_FORCE_LONG_SLEEP       = 
    DS_SYS_CONF_3GPP2_FORCE_LONG_SLEEP,
  /** 
   @see    DS_SYS_CONF_3GPP2_FORCE_LONG_SLEEP
  */

  PS_SYS_CONF_3GPP2_CAM_TIMER              = DS_SYS_CONF_3GPP2_CAM_TIMER,
  /** 
   @see    DS_SYS_CONF_3GPP2_CAM_TIMER
  */

  PS_SYS_CONF_3GPP2_SCRM                   = DS_SYS_CONF_3GPP2_SCRM,
  /** 
   @see    DS_SYS_CONF_3GPP2_SCRM
  */

  PS_SYS_CONF_3GPP2_RDUD                   = DS_SYS_CONF_3GPP2_RDUD,
  /** 
   @see    DS_SYS_CONF_3GPP2_RDUD
  */

  PS_SYS_CONF_3GPP2_CALL_THROTTLE_INFO     = 
    DS_SYS_CONF_3GPP2_CALL_THROTTLE_INFO,
  /** 
   @see    DS_SYS_CONF_3GPP2_CALL_THROTTLE_INFO
  */

  PS_SYS_CONF_3GPP2_AN_AUTH_STATUS         = DS_SYS_CONF_3GPP2_AN_AUTH_STATUS,
  /** 
   @see    DS_SYS_CONF_3GPP2_AN_AUTH_STATUS
  */

  PS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS     = DS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS,
  /**<
   @see    DS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS
  */

  PS_SYS_CONF_3GPP2_CONF_MAX               = DS_SYS_CONF_3GPP2_CONF_MAX,

  /*3GPP2 conf specific to PS*/
  PS_SYS_CONF_3GPP2_INTERNAL_MIN        = PS_SYS_CONF_3GPP2_CONF_MAX + 1,
  PS_SYS_CONF_3GPP2_S101_STATUS_CHANGE  = PS_SYS_CONF_3GPP2_INTERNAL_MIN,
  /**< 
       User shall query this configuration whenever user wants to know if
       preregistration in eHRPD tunnel mode is allowed or not. In response,
       user recieves information about  whether tunnel call is allowed or not.
       Once the response is recieved, and if the status changes to tunnel call
       allowed, user will preregister the LTE PDN contexts (those PDNs that
       are not preregistered earlier) if the status changes to tunnel call
       disallowed, user will not attempt preregistration for any new LTE PDN
       contexts
  */

  PS_SYS_CONF_3GPP2_EPZID_INFO          = PS_SYS_CONF_3GPP2_INTERNAL_MIN + 1,
  /**<       
       Configuration holds value for current EPZID when hysterisis is cancelled        
       or expires
  */

  PS_SYS_CONF_3GPP2_INTERNAL_MAX        = PS_SYS_CONF_3GPP2_INTERNAL_MIN + 100,

  /*---------------------------------------------------------------------------
      3GPP CONF VALUES
  ---------------------------------------------------------------------------*/
  /*3GPP conf */
  PS_SYS_CONF_3GPP_CONF_MIN              = DS_SYS_CONF_3GPP_CONF_MIN,
  PS_SYS_CONF_3GPP_NSAPI                 = DS_SYS_CONF_3GPP_NSAPI,
  /** 
   @see    DS_SYS_CONF_3GPP_NSAPI
  */
  PS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS  = DS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS,
  /** 
   @see    DS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS
  */

  PS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO = DS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO,
  /**< 
       @see DS_SYS_CONF_ROAMING_DISALLOWED_INFO
  */

  PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE,
  /**<       
       Configuration holds value for modem to decide whether to perform initial
       attach or handoff attach on LTE. PS_SYS_EVENT_3GPP_LTE_ATTACH_TYPE_CHANGE
       will be generated when this value is set.
  */

  PS_SYS_CONF_3GPP_CONF_MAX              = DS_SYS_CONF_3GPP_CONF_MAX,

  /*3GPP conf specific to PS*/
  PS_SYS_CONF_3GPP_INTERNAL_MIN          = PS_SYS_CONF_3GPP_CONF_MAX + 1,
  PS_SYS_CONF_3GPP_INTERNAL_MAX          = PS_SYS_CONF_3GPP_INTERNAL_MIN + 100,
  
  /*---------------------------------------------------------------------------
      COMMON TECH CONF VALUES
  ---------------------------------------------------------------------------*/
  /*Common conf */
  PS_SYS_CONF_COMMON_CONF_MIN            = DS_SYS_CONF_COMMON_CONF_MIN,
  PS_SYS_CONF_SYSTEM_STATUS              = DS_SYS_CONF_SYSTEM_STATUS,
  /**< 
       @see DS_SYS_CONF_SYSTEM_STATUS
  */

  PS_SYS_CONF_DUN_CTRL_PREF              = DS_SYS_CONF_DUN_CTRL_PREF,

  PS_SYS_CONF_SYSTEM_STATUS_EX           = DS_SYS_CONF_SYSTEM_STATUS_EX,
  /**< 
       @see DS_SYS_CONF_SYSTEM_STATUS_EX
  */

  PS_SYS_CONF_APN_PREF_SYS               = DS_SYS_CONF_APN_PREF_SYS,
  /**< 
       @see PS_SYS_CONF_APN_PREF_SYS
  */

  PS_SYS_CONF_THROUGHPUT_INFO_TIMER      = DS_SYS_CONF_THROUGHPUT_INFO_TIMER,
  /**< 
       @see DS_SYS_CONF_THROUGHPUT_INFO_TIMER
  */

  PS_SYS_CONF_THROUGHPUT_INFO           = DS_SYS_CONF_THROUGHPUT_INFO,
  /**< 
       @see DS_SYS_CONF_THROUGHPUT_INFO
  */

  PS_SYS_CONF_START_FAST_DORMANCY       = DS_SYS_CONF_START_FAST_DORMANCY,
  /**< 
       @see DS_SYS_CONF_START_FAST_DORMANCY
  */

   PS_SYS_CONF_STOP_FAST_DORMANCY       = DS_SYS_CONF_STOP_FAST_DORMANCY,
  /**< 
       @see DS_SYS_CONF_STOP_FAST_DORMANCY
  */

   PS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER  = DS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER,
   /**<
       @see DS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER
   */

   PS_SYS_CONF_DL_THROUGHPUT_INFO        = DS_SYS_CONF_DL_THROUGHPUT_INFO,
   /**<
       @see DS_SYS_CONF_DL_THROUGHPUT_INFO
   */


   PS_SYS_CONF_DL_MIN_THROUGHPUT_TIMER    = DS_SYS_CONF_DL_MIN_THROUGHPUT_TIMER,
   /**<
       @see DS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER
   */

  PS_SYS_CONF_THERMAL_MITIGATION_INFO	= DS_SYS_CONF_THERMAL_MITIGATION_INFO,
  /**<
       @see DS_SYS_CONF_THERMAL_MITIGATION_INFO
  */

  PS_SYS_CONF_DATA_SERVICE_WIFI_SETTING   = DS_SYS_CONF_DATA_SERVICE_WIFI_SETTING,
  /**<
      @see DS_SYS_CONF_DATA_SERVICE_WIFI_SETTING
  */

  PS_SYS_CONF_ROAMING_STATUS_INFO   = DS_SYS_CONF_ROAMING_STATUS_INFO,
  /**<
      @see DS_SYS_CONF_ROAMING_STATUS_INFO
  */  

  PS_SYS_CONF_COMMON_CONF_MAX            = DS_SYS_CONF_COMMON_CONF_MAX,

  /*Common conf for all tech specific to PS*/
  PS_SYS_CONF_COMMON_INTERNAL_MIN      = PS_SYS_CONF_COMMON_CONF_MAX + 1, 
  PS_SYS_CONF_ENABLE_LOOPBACK          = PS_SYS_CONF_COMMON_INTERNAL_MIN,
  /**<       
       Configuration is set by QBI through QMI to tell lower layer to enable
       loopback iface so that future requests goes to loopback iface rather
       than um iface.
  */
  PS_SYS_CONF_NULL_RESOLVED_APN         = PS_SYS_CONF_COMMON_INTERNAL_MIN + 1,
  /**< 
       This query is available to the user to learn about the apn which is 
       null resolved. This configuration shall be used by the mode handlers to
       set the null resolved apn. This is stored in the technology specific cache.
       EPC modehandler resolves the NULL apn and the resolved apn info is sent 
       in the event PS_SYS_EVENT_NULL_RESOLVED_APN.  
  */

  PS_SYS_CONF_DATA_SERVICE_SETTING      = PS_SYS_CONF_COMMON_INTERNAL_MIN + 2,
  /**< 
        This configuration is set by the tethered application once on boot up
        and when there is a change in the UI settings. This conveys if the data
        services setting is ON/OFF.
  */

  PS_SYS_CONF_DATA_SERVICE_ROAMING_SETTING   = PS_SYS_CONF_COMMON_INTERNAL_MIN + 3,
  /**< 
        This configuration is set by the tethered application once on boot up
        and when there is a change in the UI settings. This conveys if the data
        services roaming setting is ON/OFF.
  */

  PS_SYS_CONF_APN_INFO                    = PS_SYS_CONF_COMMON_INTERNAL_MIN + 4,
  /**< 
        This configuration is set by the tethered application once on boot up
        and when there is a change in the UI settings. This conveys the APN
        related information.
  */

  PS_SYS_CONF_MODEM_POWER_STATS_CONFIGURATION  = PS_SYS_CONF_COMMON_INTERNAL_MIN + 7,
  /**<       
       Configuration is set by QMI to enable/disable modem power stats.
       This is read by DS3g to start and stop the power statistics calculation
  */

  PS_SYS_CONF_QUERY_MODEM_POWER_STATS  = PS_SYS_CONF_COMMON_INTERNAL_MIN + 8,
  /**<       
       Configuration is set by QMI to request Ds3g for the modem power statistics.
       This is read by Ds3g, which in turn generates power statistics information.
  */

  PS_SYS_CONF_MODEM_POWER_STATISTICS   = PS_SYS_CONF_COMMON_INTERNAL_MIN + 9,
  /**<       
       Configuration is set by Ds3g to specify the current power statistics of the
       modem. This is ready by QMI.
  */

  PS_SYS_CONF_REMOTE_SOCKET_HDLR_AVAIL  = PS_SYS_CONF_COMMON_INTERNAL_MIN + 10,
  /**< 
        This configuration is set by QMI when remote socket handler on the TE
        registers for the related indications. It will result in generation of
        PS_SYS_EVENT_REMOTE_SOCKET_HDLR_CHANGE.
  */

  PS_SYS_CONF_COMMON_INTERNAL_MAX      = PS_SYS_CONF_COMMON_INTERNAL_MIN + 100,

  PS_SYS_CONF_WLAN_MIN                 = DS_SYS_CONF_WLAN_MIN,
  PS_SYS_CONF_GET_WLAN_MAC_ADDR        = DS_SYS_CONF_GET_WLAN_MAC_ADDR,
  /**<       
       @see DS_SYS_CONF_GET_WLAN_MAC_ADDR.
  */

  PS_SYS_CONF_GET_WLAN_SSID            = DS_SYS_CONF_GET_WLAN_SSID,
  /**<       
       @see DS_SYS_CONF_GET_WLAN_SSID.
  */

  PS_SYS_CONF_WLAN_MAX                 = DS_SYS_CONF_WLAN_MAX,

  PS_SYS_CONF_WLAN_INTERNAL_MIN        = PS_SYS_CONF_WLAN_MAX + 1,

  PS_SYS_CONF_WLAN_PREFERENCE         = PS_SYS_CONF_WLAN_INTERNAL_MIN,
  /**<       
       Configuration is set by Application Processor through QMI to tell lower layer
       whether the preference is for WLAN Local Breakout
  */

  PS_SYS_CONF_WLAN_STATUS              = PS_SYS_CONF_WLAN_INTERNAL_MIN + 1,
  /**<       
       Configuration is set by Application Processor through QMI to tell lower layer
       whether WLAN is available or not, and the IP address, MAC address and WQE
       status if applicable.
  */
  PS_SYS_CONF_WLAN_MAX_CONN            = PS_SYS_CONF_WLAN_INTERNAL_MIN + 2,
  /**<       
       Configuration to tell whether the max connection is reached on IWLAN
  */

  PS_SYS_CONF_WLAN_INTERNAL_MAX        = PS_SYS_CONF_WLAN_INTERNAL_MIN + 100,



  PS_SYS_CONF_MAX
} ps_sys_conf_enum_type;

/**
  @brief Data type to be used with DS_SYS_CONF_3GPP2_CALL_THROTTLE_INFO 
  @see   DS_SYS_CONF_3GPP2_CALL_THROTTLE_INFO
*/
typedef struct
{
  unsigned long  hdr_call_throttle_delay;  /**< hdr call throtlle delay 
                                                in milliseconds*/
  unsigned long  cdma_call_throttle_delay; /**< cdma call throtlle delay 
                                                in milliseconds*/
} ps_sys_conf_3gpp2_call_throttle_delay_type;


/** Mask whether to allow dun call or not */
#define PS_SYS_CONF_DUN_CTRL_PREF_ALLOW_DUN_CALL_MASK DS_SYS_CONF_DUN_CTRL_PREF_ALLOW_DUN_CALL_MASK

/** Mask for entitlement check */
#define PS_SYS_CONF_DUN_CTRL_PREF_ENTITLEMENT_CHECK_MASK DS_SYS_CONF_DUN_CTRL_PREF_ENTITLEMENT_CHECK_MASK

/** Mask for silent redial check */
#define PS_SYS_CONF_DUN_CTRL_PREF_SILENT_REDIAL_CHECK_MASK DS_SYS_CONF_DUN_CTRL_PREF_SILENT_REDIAL_CHECK_MASK

/** Mask for whether to retrieve dun call notification or not */
#define PS_SYS_CONF_DUN_CTRL_PREF_CALL_STATUS_NOTIFICATION_MASK DS_SYS_CONF_DUN_CTRL_PREF_CALL_STATUS_NOTIFICATION_MASK

/**
  @brief Data type to be used with PS_SYS_CONF_DUN_CTRL_PREF when calling 
         set operation for dun control preference 
  @see   PS_SYS_CONF_DUN_CTRL_PREF 
*/
typedef struct
{
  boolean                        enable_dun_ctrl; /**< enable or disable dun
                                                       ctrl preference*/
  struct
  {
    unsigned long                  param_mask; /**< Mask to set different
                                                    dun ctrl preference*/
    boolean                        allow_dun_call;
    /**< To allow dun call or disallow dun call. Valid if 
         PS_SYS_CONF_DUN_CTRL_PREF_ALLOW_DUN_CALL_PARAM_MASK is set in
         param_mask */
    
    ps_sys_event_cback_f_ptr_type  entitlement_check_cback_f_ptr;
    /**< Callback to recieve for allowing or disallowing call. Valid if 
         PS_SYS_CONF_DUN_CTRL_PREF_ENTITLEMENT_CHECK_MASK is is set in
         param_mask */

    void                         * entitlement_check_cback_user_data_ptr;
    /**< User data to be sent along with entitlement callback */
    
    ps_sys_event_cback_f_ptr_type  silent_redial_check_cback_f_ptr;
    /**< Callback to recieve when Modehandlers have to tell connection 
         manager about something went wrong with dun call. Valid if
         PS_SYS_CONF_DUN_CTRL_PREF_SILENT_REDIAL_CHECK_MASK is set in
         param_mask */

    void                         * silent_redial_check_cback_user_data_ptr;           
    /**< User data to be sent along with silent redial callback */

    ps_sys_event_cback_f_ptr_type call_status_notification_cback_f_ptr;
    /**< Callback for call notification status. Valid if 
         PS_SYS_CONF_DUN_CTRL_PREF_CALL_STATUS_NOTIFICATION_MASK is set.
    */

    void                        * call_status_notification_cback_user_data_ptr;
    /**< User data to be sent along with call notification callback */       
  } optional_pref; /**< Optional structure used when enable_dun_ctrl set to 
                        true */
} ps_sys_conf_set_dun_ctrl_pref_type;

/**
  @brief Data type to be used with PS_SYS_CONF_DUN_CTRL_PREF when calling 
         get operation for dun control preference 
  @see   PS_SYS_CONF_DUN_CTRL_PREF 
*/
typedef struct
{
  boolean                                 is_dun_ctrl_enabled;
  /**< Whether dun control is enabled or disabled*/
  boolean                                 allow_dun_call;
  /**< Whether preference is set to allow or disallow call */
  boolean                                 is_entitlement_check_enabled;
  /**< Whether entitlement check is enabled or disabled */
  boolean                                 is_silent_redial_check_enabled;
  /**< Whether silent redial check is enabled or disabled */
  boolean                                 is_call_status_notification_enabled;
  /**< Whether call status notification is enabled or disabled */
} ps_sys_conf_get_dun_ctrl_pref_type;

/**
  @brief AN authentication status definition
  */
typedef enum
{
  PS_SYS_CONF_3GPP2_AN_AUTH_STATUS_NOT_AUTHENTICAED = 
    DS_SYS_CONF_3GPP2_AN_AUTH_STATUS_NOT_AUTHENTICAED,   /* Not Authenticated */
  PS_SYS_CONF_3GPP2_AN_AUTH_STATUS_PASS  =
    DS_SYS_CONF_3GPP2_AN_AUTH_STATUS_PASS,      /* Authenticated successfully */
  PS_SYS_CONF_3GPP2_AN_AUTH_STATUS_FAILED =
    DS_SYS_CONF_3GPP2_AN_AUTH_STATUS_FAILED          /* Failed authentication */
} ps_sys_conf_3gpp2_an_auth_status_enum_type;

/**
  @brief Config type representing whether we need to enable loopback or not
  @see   PS_SYS_CONF_ENABLE_LOOPBACK
  */
typedef boolean ps_sys_conf_enable_loopback_type;

/**
  @brief Timer used to generate throughput information
   
  @see   PS_SYS_CONF_THROUGHPUT_INFO_TIMER 
  @see   PS_SYS_EVENT_THROUGHPUT_INFO 
*/
typedef ds_sys_conf_throughput_timer_type ps_sys_conf_throughput_timer_type;

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/** 
   @brief  This is PS Sys level API for retrieving configuration value. This
           function retrieves configuration value for requested configuration
           of a particular technology and default subscription.
   @see    ds_sys_conf_get
*/
int16 ps_sys_conf_get
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_conf_enum_type           conf_name,
  void                          * arg_val_ptr,
  int16                         * ps_errno
);

/**
  @brief  This is PS Sys level API for setting configuration value. This 
          function performs operations to set specified parameter values.
          for particular technology and default subscription.
  @see    ds_sys_conf_set
*/
int16 ps_sys_conf_set
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_conf_enum_type           conf_name,
  void                          * arg_val_ptr,
  int16                         * ps_errno
);


/** 
   @brief  This is PS Sys level API for retrieving configuration value. This
           function retrieves configuration value for requested configuration
           of a particular technology and subscription id.
   @see    ds_sys_conf_get_ex
*/
int16 ps_sys_conf_get_ex
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_conf_enum_type           conf_name,
  ps_sys_subscription_enum_type   subscription_id,
  void                          * arg_val_ptr,
  int16                         * ps_errno
);

/**
  @brief  This is PS Sys level API for setting configuration value. This 
          function performs operations to set specified parameter values for
          provided subscription id
  @see    ds_sys_conf_set_ex
*/
int16 ps_sys_conf_set_ex
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_conf_enum_type           conf_name,
  ps_sys_subscription_enum_type   subscription_id,
  void                          * arg_val_ptr,
  int16                         * ps_errno
);

/**
  @brief  This function sets default values for certain 
          configuration items stored in the cache.
   
  @param[in] subs_id     Subs id of the config cache to be 
                         initialised
*/
void ps_sys_conf_set_default_values
(
  uint32              subs_id
);

/**
  @brief  This function sets default values for apn info 
          configuration stored in the cache. 
   
  @param[in] param_val   APN info to be written
  @param[in] subs_id     Subs id of the apn cache 
   
  @see  PS_SYS_CONF_EFS_APN_INFO 
*/
void ps_sys_conf_update_apn_info_cache
(
  char             * param_val, 
  uint32             subs_id
);

/**
  @brief  This function writes the apn info from the 
          cache into the input buffer 
   
  @param[in] param_val   APN info input buffer
  @param[in] subs_id     Subs id of the apn cache 
  @param[in] avail_buff_size  Size available for the input 
                              buffer
   
  @see  PS_SYS_CONF_EFS_APN_INFO 
*/
void ps_sys_conf_write_apn_info_param
(
  char            *param_val,
  uint32           subs_id,
  int              avail_buff_size 
);
/**
  @brief  This function writes all the configuration items 
          to the input file buffer for the given subs id 
   
  @param[in] file_buffer Input file buffer; Memory is allocated 
                         by the caller 
  @param[in] subs_id     Subs id of the config file buffer
  @param[out] total_bytes  Total num of bytes written to the 
                           file buffer 
   
  @return  0             If write to file buffer is success
  @return  -1            On any failure.  
*/
int ps_sys_conf_write_conf_items_buffer
(
  char        *file_buffer,
  int         *total_bytes,
  uint32       subs_id
);

/**
  @brief  This function updates the cache from the EFS 
          configuration
   
  @param[in] param_name  Configuration name
  @param[in] param_val   Configuration value
  @param[in] subs_id     Subscription id
   
  @return  0             If write to cache is success
  @return  -1            On any failure.  
*/
int ps_sys_conf_update_cache_from_efs
( 
  int      param_name_index, 
  char     *param_val,
  uint32    subs_id
);

#ifdef __cplusplus
}
#endif

#endif /* PS_SYS_CONF_H */
