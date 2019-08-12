/**
@file ds_sys_conf.h
@brief
This file defines functions, enums and structs used to read and update
technology configurations
*/

#ifndef DS_SYS_CONF_H
#define DS_SYS_CONF_H

/*===========================================================================

                          D S  _ S Y S _ C O N F . H

DESCRIPTION

  DS Sys Conf module interface file. This contains variables and
  definitions used by the various modules to access interface definitions.

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_sys_conf.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/10/12    msh    moved an auth status to modem level api
10/01/11    bvd    Rearranged conf enum type
09/06/11    sy     Created module

===========================================================================*/

#include "comdef.h"
#include "dserrno.h"
#include "ds_sys.h"
#include "ds_sys_event.h"

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
#define   DS_SYS_CONF_MAX_APN_STRING_LEN       101 /**< PDN name str length */
#define   DS_SYS_CONF_MAX_NUM_BEARER_CONTEXT     8 /**< MAX PDN */

/**
  @brief Definitions of various interface ctrl operations.
*/
typedef enum
{
  DS_SYS_CONF_MIN                          = 0,

  /*-------------------------------------------------------------------------
    3GPP2 CONF
  -------------------------------------------------------------------------*/
  DS_SYS_CONF_3GPP2_CONF_MIN               = DS_SYS_CONF_MIN,
  DS_SYS_CONF_3GPP2_PRE_DORMANCY_SYS_INFO  = DS_SYS_CONF_3GPP2_CONF_MIN,
  /**<
       This query gets pre-dormant data call information such as system mode
       and service option used. In response to query, User gets last system mode
       and previous service option
  */

  DS_SYS_CONF_3GPP2_PAGE_MONITOR_PERIOD    = DS_SYS_CONF_3GPP2_CONF_MIN + 1,
  /**<
       This query gets page monitor info. When doing set operation, event
       DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE would be registered
       with callback provided
  */

  DS_SYS_CONF_3GPP2_FORCE_LONG_SLEEP       = DS_SYS_CONF_3GPP2_CONF_MIN + 2,
  DS_SYS_CONF_3GPP2_CAM_TIMER              = DS_SYS_CONF_3GPP2_CONF_MIN + 4,
  /**<
       This query gets CAM timer value currently being used for data call in
       seconds from CAM module. In response, User gets CAM timer in seconds.
  */

  DS_SYS_CONF_3GPP2_SCRM                   = DS_SYS_CONF_3GPP2_CONF_MIN + 5,
  /**<
       This query returns the information about whether SCRM (Supplemental
       Channel Request Message) is enabled or not for data calls. In response,
       User gets Boolean flag to indicate SCRM is enabled or not. "TRUE"= SCRM
       is enabled. "FALSE"=SCRM is disabled
  */

  DS_SYS_CONF_3GPP2_RDUD                   = DS_SYS_CONF_3GPP2_CONF_MIN + 6,
  /**<
       This query gets to know if RDUD(Reduced Dormancy followed by Unsolicited
       Data ) is enabled or not for data call.User gets Boolean flag to
       indicate RDUD is enabled or not. "TRUE" = RDUD is enabled. "FALSE"=RDUD
       is disabled
  */

  DS_SYS_CONF_3GPP2_CALL_THROTTLE_INFO     = DS_SYS_CONF_3GPP2_CONF_MIN + 7,
  /**<
       This query gets the call throttle info for both 1x and HDR from CM and
       returns them in the callback function registered by the caller. In
       reponse, User gets call throttle info
  */

  DS_SYS_CONF_3GPP2_AN_AUTH_STATUS      = DS_SYS_CONF_3GPP2_CONF_MIN + 8,
  /**<
       Configuration holds value for current AN Auth Status.It is updated by
       3GPP2 mode handler every time there is a change.
  */

  DS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS     = DS_SYS_CONF_3GPP2_CONF_MIN + 9,
  /**<
       Configuration holds value for current qos network status.It is updated by
       3GPP2 mode handler every time there is a change.
  */

  DS_SYS_CONF_3GPP2_CONF_MAX               = 100,

  /*-------------------------------------------------------------------------
    3GPP CONF
  -------------------------------------------------------------------------*/
  DS_SYS_CONF_3GPP_CONF_MIN                = 300,
  DS_SYS_CONF_3GPP_NSAPI                   = DS_SYS_CONF_3GPP_CONF_MIN,
  DS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS    = DS_SYS_CONF_3GPP_CONF_MIN + 1,
  /**<
       Configuration holds value for the emergency pdn status.It is updated by
       3GPP mode handler. If the emergency pdn is the only pdn, the status 
       is set to true. If the emergency pdn is not the only one present, the 
       status is set to false.
  */
  DS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO = DS_SYS_CONF_3GPP_CONF_MIN + 2,
  /**<
       This configuration will be used by applications (modem and AP) and the  
       information is populated by mode handler. In response, user who has   
       registered for the roaming info will get DS_SYS_EVENT_ROAMING_DISALLOWED_INFO 
       event. On receiving event, user  shall be able to know the list of apn names.
       If the list is empty, client can attempt call on any of the apns. If the list 
       is non-empty, client will teardown any active calls on the listed apns and 
       will not attempt further calls on those APNs.
  */
  DS_SYS_CONF_3GPP_CONF_MAX                = 400,

  /*-------------------------------------------------------------------------
    COMMON TECH CONF VALUES
  -------------------------------------------------------------------------*/
  DS_SYS_CONF_COMMON_CONF_MIN              = 600,
  DS_SYS_CONF_SYSTEM_STATUS                = DS_SYS_CONF_COMMON_CONF_MIN,
  /**<
       This query shall be only used by internal DS to configure system with
       current network info. In response, user shall get
       DS_SYS_EVENT_SYSTEM_STATUS_CHANGE event if they registered. On recieving
       response, based on current network info, user will know which network is
       on.
  */

  DS_SYS_CONF_DUN_CTRL_PREF                = DS_SYS_CONF_COMMON_CONF_MIN + 1,
  /**<
     Connection manager sends out dun control preference using this
     configuration. The default behaviour is to allow dun calls. Connection
     manager can set three possible mask, one to whether allow dun call or not.
     another for call notification (whether to recieve notification when dun
     call is made), another for entitlement check ( when call is made, ask
     connection manager to allow dun call or not) and another for silent redial
     check (everytime call doesnt goes through, tell connection manager
     something is bad and allow connection manager to fix the issue).
     Connection manager has to give callback pointers for those whose mask is
     set except for mask where we say whether to allow dun call or not. In case
     if user doesnt specify the mask then values for corresponding callbacks
     and other parameter wont take in effect.
  */

  DS_SYS_CONF_SYSTEM_STATUS_EX               = DS_SYS_CONF_COMMON_CONF_MIN + 2,
  /**<
       This configuration shall be only used by DSD to configure system with
       current system status info. In response, user who has registered for 
       system status shall get DS_SYS_EVENT_SYSTEM_STATUS_EX event.
       On receiving event, user shall be able to know what is the preferred
       system, system status info per pdn and system status info per subscription
       id.
  */

  DS_SYS_CONF_APN_PREF_SYS                 = DS_SYS_CONF_COMMON_CONF_MIN + 3,
  /**<
       This configuration shall be only used by user to let below layers like 
       Modehandlers know that their highest technology preference for data call.
       So if data call is on WWAN and if IMS let Modehandlers know that their 
       highest tech preference is WLAN then handoff process needs to be triggered.
       All interested user in knowing about technology preference of APNs needs
       to register for event DS_SYS_EVENT_APN_PREF_SYS_CHANGE.
  */

  DS_SYS_CONF_THROUGHPUT_INFO_TIMER         = DS_SYS_CONF_COMMON_CONF_MIN + 4,
  /**<
       This configuration shall be only used by user to let below layers like 
       Modehandlers know the interval at which throughput information should
       be generated. All interested users in knowing throughput info should be
       registered for event DS_SYS_EVENT_THROUGHPUT_INFO.
  */

  DS_SYS_CONF_THROUGHPUT_INFO               = DS_SYS_CONF_COMMON_CONF_MIN + 5,
  /**<
       This query is available to the user to learn about the throughput
       information. The list of APNs with active data call along with their actual,
       allowed throughput information is returned in the response.
  */

  DS_SYS_CONF_START_FAST_DORMANCY           = DS_SYS_CONF_COMMON_CONF_MIN + 6,
  /**<
      This configuration is used by mode handler to give START
      fast dormancy indication to PS 
   */

  DS_SYS_CONF_STOP_FAST_DORMANCY			= DS_SYS_CONF_COMMON_CONF_MIN + 7,
  /**<
	This configuration is used by mode handler to give STOP
	fast dormancy indication to PS 
  */

   DS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER      = DS_SYS_CONF_COMMON_CONF_MIN + 8,
   /**<
        This configuration shall be only used by user to let below layers like 
        Modehandlers know the interval at which downlink throughput information should
        be generated. All interested users in knowing throughput info should be
        registered for event DS_SYS_EVENT_THROUGHPUT_INFO.
   */

   DS_SYS_CONF_DL_THROUGHPUT_INFO            = DS_SYS_CONF_COMMON_CONF_MIN + 9,
   /**<
        This query is available to the user to learn about the downlink throughput
        information. 
   */

   DS_SYS_CONF_DL_MIN_THROUGHPUT_TIMER      = DS_SYS_CONF_COMMON_CONF_MIN + 10,
   /**< 
        This configuration represents the minimum timer interval used to report
        downlink throughput timer interval. This value is set by the ModeHandlers.
   */

  DS_SYS_CONF_THERMAL_MITIGATION_INFO		= DS_SYS_CONF_COMMON_CONF_MIN + 11,
  /**<
        This configuration is used by DS3g to set the thermal level mitigation
        action info. External clients like QMI and DSS can use this to query
        the same.
  */

  DS_SYS_CONF_DATA_SERVICE_WIFI_SETTING  = DS_SYS_CONF_COMMON_CONF_MIN + 18,
  /**<
       This configuration shall be only used by external application to set  the
       WIFI on/off switch. Setting this configuration
       generates DS_SYS_EVENT_WIFI_SETTINGS_CHANGE
  */

  DS_SYS_CONF_ROAMING_STATUS_INFO   = DS_SYS_CONF_COMMON_CONF_MIN + 19,
  /**<        
       Configuration is set by technology modehandler when the UE moves away 
       from the home network and is roaming. User registers for this event to 
       know if the UE is in the home network or roaming.
  */  
  
  DS_SYS_CONF_COMMON_CONF_MAX              = 700,

  DS_SYS_CONF_WLAN_MIN                     = 800,
  DS_SYS_CONF_GET_WLAN_MAC_ADDR            = DS_SYS_CONF_WLAN_MIN,
  /**<       
       Configuration retrieves MAC address for WLAN. User wont be able to do
       set operation.Whenever WLAN mode handler sets the mac address and if 
       there is change in MAC address then we would trigger 
       DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE
       
       @see DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE
  */

  DS_SYS_CONF_GET_WLAN_SSID            = DS_SYS_CONF_WLAN_MIN + 1,
  /**<       
       Configuration retrieves SSID for WLAN. User wont be able to do
       set operation. Whenever WLAN mode handler sets the SSID and if 
       there is change in SSID then we would trigger 
       DS_SYS_EVENT_WLAN_SSID_CHANGE
       
       @see DS_SYS_EVENT_WLAN_SSID_CHANGE
  */

  DS_SYS_CONF_WLAN_MAX                     = 900,

  DS_SYS_CONF_MAX
} ds_sys_conf_enum_type;


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
} ds_sys_conf_3gpp2_call_throttle_delay_type;


/** Mask whether to allow dun call or not */
#define DS_SYS_CONF_DUN_CTRL_PREF_ALLOW_DUN_CALL_MASK           0x00000001
/** Mask for entitlement check */
#define DS_SYS_CONF_DUN_CTRL_PREF_ENTITLEMENT_CHECK_MASK        0x00000010
/** Mask for silent redial check */
#define DS_SYS_CONF_DUN_CTRL_PREF_SILENT_REDIAL_CHECK_MASK      0x00000100
/** Mask for whether to retrieve dun call notification or not */
#define DS_SYS_CONF_DUN_CTRL_PREF_CALL_STATUS_NOTIFICATION_MASK 0x00001000

/**
  @brief Data type to be used with DS_SYS_CONF_DUN_CTRL_PREF when calling
         set operation for dun control preference
  @see   DS_SYS_CONF_DUN_CTRL_PREF
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
         DS_SYS_CONF_DUN_CTRL_PREF_ALLOW_DUN_CALL_PARAM_MASK is set in
         param_mask */

    ds_sys_event_cback_f_ptr_type  entitlement_check_cback_f_ptr;
    /**< Callback to recieve for allowing or disallowing call. Valid if
         DS_SYS_CONF_DUN_CTRL_PREF_ENTITLEMENT_CHECK_MASK is is set in
         param_mask */

    void                         * entitlement_check_cback_user_data_ptr;
    /**< User data to be sent along with entitlement callback */

    ds_sys_event_cback_f_ptr_type  silent_redial_check_cback_f_ptr;
    /**< Callback to recieve when Modehandlers have to tell connection
         manager about something went wrong with dun call. Valid if
         DS_SYS_CONF_DUN_CTRL_PREF_SILENT_REDIAL_CHECK_MASK is set in
         param_mask */

    void                         * silent_redial_check_cback_user_data_ptr;
    /**< User data to be sent along with silent redial callback */

    ds_sys_event_cback_f_ptr_type call_status_notification_cback_f_ptr;
    /**< Callback for call notification status. Valid if
         DS_SYS_CONF_DUN_CTRL_PREF_CALL_STATUS_NOTIFICATION_MASK is set.
    */

    void                        * call_status_notification_cback_user_data_ptr;
    /**< User data to be sent along with call notification callback */
  } optional_pref; /**< Optional structure used when enable_dun_ctrl set to
                        true */
} ds_sys_conf_set_dun_ctrl_pref_type;

/**
  @brief AN authentication status definition

  @see   DS_SYS_CONF_3GPP2_AN_AUTH_STATUS
*/
typedef enum
{
  DS_SYS_CONF_3GPP2_AN_AUTH_STATUS_NOT_AUTHENTICAED  = 0,
  DS_SYS_CONF_3GPP2_AN_AUTH_STATUS_PASS,
  DS_SYS_CONF_3GPP2_AN_AUTH_STATUS_FAILED
} ds_sys_conf_3gpp2_an_auth_status_enum_type;

/**
  @brief Timer used to generate throughput information
   
  @see   DS_SYS_CONF_THROUGHPUT_INFO_TIMER 
  @see   DS_SYS_EVENT_THROUGHPUT_INFO 
*/

typedef uint32  ds_sys_conf_throughput_timer_type;
/**< Timer in milliseconds. Period at which throughput 
     information is generated.
*/

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/**
  @brief  This function performs operations to retrieve specified parameter
          values. Operation would be performed for default subscription

  @param[in] tech_type     States Technology type
  @param[in] conf_name     Operation name
  @param[in] arg_val_ptr   Pointer to operation specific structure
  @param[out] dss_errno    Error code is filled when return value is not
                           success. Possible values are
                           @li DS_EFAULT if parameter is invalid
                           @li DS_EINVAL if operation is invalid
                           @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS     On Success.
  @return  DSS_ERROR       On failure.

  @code

  Example
   User queries for system status using get operation which is
   synchronous

   ds_sys_system_status_type sys_status_type;

   ret_val = ds_sys_conf_get(DS_SYS_TECH_3GPP,
                             DS_SYS_CONF_SYSTEM_STATUS,
                             (void *)&sys_status_type,
                             &ps_errno);

   if(0 > ret_val)
   {
     LOG_MSG_ERROR("Configuration get failed, err %d", ps_errno,0,0);
   }
   else
   {
     LOG_MSG_INFO("Attach profile number %d",
                   sys_status_type->preferred_network);
   }

   @endcode
*/
int16 ds_sys_conf_get
(
  ds_sys_tech_enum_type              tech_type,
  ds_sys_conf_enum_type              conf_name,
  void                             * argval_ptr,
  int16                            * dss_errno
);


/**
  @brief  This function performs operations to set specified parameter values.
          Operation would be performed for default subscription.

  @param[in] tech_type      States Technology type
  @param[in] conf_name      Operation name
  @param[in] arg_val_ptr    Pointer to operation specific
                            structure
  @param[out] dss_errno     Error code is filled when return value is not
                            success. Possible values are
                            @li DS_EFAULT if parameter is invalid
                            @li DS_EINVAL if operation is invalid
                            @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code

   Example
   User performs the set operation for DS_SYS_CONF_3GPP2_S101_STATUS_CHANGE

   ds_sys_3gpp2_s101_status_change_type s101_status_type;
   s101_status_type.tunnel_call_allowed = FALSE;
   s101_status_type.lcp_throttled       = FALSE;
   s101_status_type.virtual_connection_throttled = FALSE;

   ret_val = ds_sys_conf_set(DS_SYS_TECH_3GPP,
                             DS_SYS_CONF_3GPP2_S101_STATUS_CHANGE,
                             (void *)&s101_status_type,
                             &ps_errno);

   if(0 > ret_val)
   {
     LOG_MSG_ERROR("Configuration set failed, err %d", ps_errno,0,0);
   }

  @endcode
*/
int16 ds_sys_conf_set
(
  ds_sys_tech_enum_type              tech_type,
  ds_sys_conf_enum_type              conf_name,
  void                             * argval_ptr,
  int16                            * dss_errno
);


/**
  @brief  This function performs operations to retrieve specified parameter
          values for given subscription id.

  @param[in] tech_type     States Technology type
  @param[in] conf_name     Operation name
  @param[in] subscription_id  Subscription id
  @param[in] arg_val_ptr   Pointer to operation specific structure
  @param[out] dss_errno    Error code is filled when return value is not
                           success. Possible values are
                           @li DS_EFAULT if parameter is invalid
                           @li DS_EINVAL if operation is invalid
                           @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS     On Success.
  @return  DSS_ERROR       On failure.
  @code

  Example
   User queries for system status using get operation for secondary
   subscription id

   ds_sys_system_status_type sys_status_type;

   ret_val = ds_sys_conf_get_ex(DS_SYS_TECH_ALL,
                                DS_SYS_CONF_SYSTEM_STATUS,
                                DS_SYS_SECONDARY_SUBS,
                                (void *)&sys_status_type,
                                &ps_errno);

   if(0 > ret_val)
   {
     LOG_MSG_ERROR("Configuration get failed, err %d", ps_errno,0,0);
   }
   else
   {
     LOG_MSG_INFO("Attach profile number %d",
                   sys_status_type->preferred_network);
   }

   @endcode  
*/
int16 ds_sys_conf_get_ex
(
  ds_sys_tech_enum_type              tech_type,
  ds_sys_conf_enum_type              conf_name,
  ds_sys_subscription_enum_type      subscription_id,
  void                             * argval_ptr,
  int16                            * dss_errno
);


/**
  @brief  This function performs operations to set specified parameter values.
          for particualr subscription.

  @param[in] tech_type      States Technology type
  @param[in] conf_name      Operation name
  @param[in] subscription_id  Subscription id
  @param[in] arg_val_ptr    Pointer to operation specific
                            structure
  @param[out] dss_errno     Error code is filled when return value is not
                            success. Possible values are
                            @li DS_EFAULT if parameter is invalid
                            @li DS_EINVAL if operation is invalid
                            @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.
  
  @code

   Example
   User performs the set operation for DS_SYS_CONF_3GPP2_S101_STATUS_CHANGE
   for secondary subscription

   ds_sys_3gpp2_s101_status_change_type s101_status_type;
   s101_status_type.tunnel_call_allowed = FALSE;
   s101_status_type.lcp_throttled       = FALSE;
   s101_status_type.virtual_connection_throttled = FALSE;

   ret_val = ds_sys_conf_set_ex(DS_SYS_TECH_3GPP2,
                                DS_SYS_CONF_3GPP2_S101_STATUS_CHANGE,
                                DS_SYS_SECONDARY_SUBS,
                                (void *)&s101_status_type,
                                &ps_errno);

   if(0 > ret_val)
   {
     LOG_MSG_ERROR("Configuration set failed, err %d", ps_errno,0,0);
   }

  @endcode  
*/
int16 ds_sys_conf_set_ex
(
  ds_sys_tech_enum_type              tech_type,
  ds_sys_conf_enum_type              conf_name,
  ds_sys_subscription_enum_type      subscription_id,
  void                             * argval_ptr,
  int16                            * dss_errno
);

#ifdef __cplusplus
}
#endif

#endif /* DS_SYS_CONF_H */
