/**
@file ps_sys_event.h
@brief
This file defines functions, enums and structs use to register for events
*/

#ifndef PS_SYS_EVENT_H
#define PS_SYS_EVENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ E V E N T . H 


GENERAL DESCRIPTION
  This file defines functions, enums and structs use to register for
  events

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/inc/ps_sys_event.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
 
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
#include "ps_iface.h"
#include "ps_sys.h"
#include "ds_sys_event.h"
#include "ps_iface_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/** 
  Defines different SYS events 
*/
typedef enum
{
  PS_SYS_EVENT_MIN                   = DS_SYS_EVENT_MIN,

  /*---------------------------------------------------------------------------
      3GPP2 EVENTS
  ---------------------------------------------------------------------------*/
  /*3GPP2 Events common to DS and PS*/
  PS_SYS_EVENT_3GPP2_MIN             = DS_SYS_EVENT_3GPP2_MIN,                                   
  PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_STATUS =
    DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_STATUS,
  /**
     @see  DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_STATUS  
  */

  PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE =
    DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE,      
  /**
     @see  DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE  
  */
                        
  PS_SYS_EVENT_3GPP2_OUTAGE_NOTIFICATION =
    DS_SYS_EVENT_3GPP2_OUTAGE_NOTIFICATION,      
  /**
     @see  DS_SYS_EVENT_3GPP2_OUTAGE_NOTIFICATION  
  */
      
  PS_SYS_EVENT_3GPP2_QOS_NETWORK_STATUS_CHANGE =
    DS_SYS_EVENT_3GPP2_QOS_NETWORK_STATUS_CHANGE,      
  /**
     @see  DS_SYS_EVENT_3GPP2_QOS_NETWORK_STATUS_CHANGE  
  */  
      
  PS_SYS_EVENT_3GPP2_MAX             = DS_SYS_EVENT_3GPP2_MAX,

  /*3GPP2 Events internal to PS*/
  PS_SYS_EVENT_3GPP2_INTERNAL_MIN    = PS_SYS_EVENT_3GPP2_MAX + 1,   
  PS_SYS_EVENT_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_RSP = 
    PS_SYS_EVENT_3GPP2_INTERNAL_MIN,
  /**<
       When epc iface detects system moves to ehrpd. It sends out ioctl
       indication if all ifaces are preregistered or not. The event is triggered
       as a response to the
       PS_SYS_IOCTL_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_CMD sent by the user.
       User will receive a boolean flag confirming success/failure of the
       prereg handoff. On success value of flag, user swaps the RAT and enables
       data transfer. On failure, user resyncs RAT. User should register for 
       the event before sending
       PS_SYS_IOCTL_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_CMD.
  */

  PS_SYS_EVENT_3GPP2_S101_STATUS_CHANGE = PS_SYS_EVENT_3GPP2_INTERNAL_MIN + 1,
  /**<       
       Event is triggered whenever the tunnel call state changes from allowed
       to disallowed and vice versa In response to event, User gets info
       whether tunnel call is allowed or not. On recieving event result, when
       the status changes to tunnel call allowed, user will preregister the LTE
       PDN contexts (those PDNs that are not preregistered earlier). When the
       status changes to tunnel call disallowed, user will not attempt
       preregistration for any new LTE PDN contexts. User register for event on
       power up.        
  */

  PS_SYS_EVENT_3GPP2_EPZID_INFO     = PS_SYS_EVENT_3GPP2_INTERNAL_MIN + 2,
  /**<       
       Event is triggered whenever the hysterisis is cancelled or expires
       on the current EPZID
  */

  PS_SYS_EVENT_3GPP2_EHRPD_PREREG_HANDOFF_STATUS =
    PS_SYS_EVENT_3GPP2_INTERNAL_MIN + 3,

  PS_SYS_EVENT_3GPP2_INTERNAL_MAX   = PS_SYS_EVENT_3GPP2_INTERNAL_MIN + 100,

  /*---------------------------------------------------------------------------
      3GPP EVENTS
  ---------------------------------------------------------------------------*/

  /*3GPP Events common to DS and PS*/
  PS_SYS_EVENT_3GPP_MIN             = DS_SYS_EVENT_3GPP_MIN,
  PS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE = 
    DS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE,
  /**<        
       Event is triggered upon lte attach pdn list write completion to the UE
       using ioctl PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST.
       Along with the event, the list of profile IDs are sent by MH.
       User Registers if interested to know that the LTE attach PDN list has
       changed
   
       @see PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST
  */
  PS_SYS_EVENT_3GPP_EMERGENCY_PDN_STATUS = DS_SYS_EVENT_3GPP_EMERGENCY_PDN_STATUS,
  /**<        
       @see DS_SYS_EVENT_3GPP_EMERGENCY_PDN_STATUS
  */
  PS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO = DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO,
  /**
    @see  DS_SYS_EVENT_ROAMING_DISALLOWED_INFO
  */

  PS_SYS_EVENT_WIFI_SETTINGS_CHANGE = DS_SYS_EVENT_WIFI_SETTINGS_CHANGE,
  /**
    @see  DS_SYS_EVENT_WIFI_SETTINGS_CHANGE
  */

  PS_SYS_EVENT_3GPP_MAX             = DS_SYS_EVENT_3GPP_MAX,


  /*3GPP Events internal to PS*/
  PS_SYS_EVENT_3GPP_INTERNAL_MIN          = PS_SYS_EVENT_3GPP_MAX + 1, 
  PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT       = PS_SYS_EVENT_3GPP_INTERNAL_MIN,
  /**< 
       Event is triggered currently if the FEATURE_EPC_HANDOFF is
       present aka there could be a context transfer from some other RAT.
       Along with the event the attach profile number is sent. On recieving
       event result user must let the Mode Handler know if there are contexts
       to be transferred by invoking the IOCTL PS_SYS_IOCTL_LTE_ATTACH_INIT.
       User registers for this event if its interested to know the LTE attach
        process has been started.
  */

  PS_SYS_EVENT_3GPP_LTE_ATTACH_COMPLETE   = PS_SYS_EVENT_3GPP_INTERNAL_MIN + 1,
  /**<        
       Event is triggered upon receiving attach complete indication from NAS
       after L2 ACK. Along with the event, the details of ifaces and APN name
       are sent by MH. On recieving event result, user shall register for iface
       events. User Registers if interested to know the completion of LTE attach.
  */

  PS_SYS_EVENT_3GPP_LTE_ATTACH_TYPE_CHANGE = PS_SYS_EVENT_3GPP_INTERNAL_MIN + 2,
  /**<        
       Event is triggered when the configuration item PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE
       is set. User registers if interested to know about change in its value.
  */

  PS_SYS_EVENT_3GPP_INTERNAL_MAX          = PS_SYS_EVENT_3GPP_INTERNAL_MIN + 100,
  
  /*---------------------------------------------------------------------------
      ALL TECH COMMON EVENTS
  ---------------------------------------------------------------------------*/
  /*Common Tech Events common to both DS and PS*/  
  PS_SYS_EVENT_COMMON_MIN            = DS_SYS_EVENT_COMMON_MIN,
  PS_SYS_EVENT_SYSTEM_STATUS_CHANGE  = DS_SYS_EVENT_SYSTEM_STATUS_CHANGE,
  /**< 
      @see DS_SYS_EVENT_SYSTEM_STATUS_CHANGE      
  */

  PS_SYS_EVENT_DUN_CTRL_CALL_STATUS  = DS_SYS_EVENT_DUN_CTRL_CALL_STATUS,
  /**
     @see  DS_SYS_EVENT_DUN_CTRL_CALL_STATUS  
  */

  PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK    =
    DS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
  /**
     @see  DS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK  
  */

  PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK  =
    DS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
  /**
     @see  DS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK  
  */

  PS_SYS_EVENT_DATA_CALL_STATUS_CHANGE  = DS_SYS_EVENT_DATA_CALL_STATUS_CHANGE,
  /**
     @see  DS_SYS_EVENT_DATA_CALL_STATUS_CHANGE  
  */

  PS_SYS_EVENT_SYSTEM_STATUS_EX = DS_SYS_EVENT_SYSTEM_STATUS_EX,
  /**
    @see  DS_SYS_EVENT_SYSTEM_STATUS_EX
  */

  PS_SYS_EVENT_UPDATED_THROUGHPUT_TIMER = DS_SYS_EVENT_UPDATED_THROUGHPUT_TIMER,
  /**
    @see  DS_SYS_EVENT_UPDATED_THROUGHPUT_TIMER
  */

  PS_SYS_EVENT_THROUGHPUT_INFO = DS_SYS_EVENT_THROUGHPUT_INFO,
  /**
    @see  DS_SYS_EVENT_THROUGHPUT_INFO
  */

  PS_SYS_EVENT_START_FAST_DORMANCY = DS_SYS_EVENT_START_FAST_DORMANCY,
   /**
    @see  DS_SYS_EVENT_START_FAST_DORMANCY
  */

  PS_SYS_EVENT_STOP_FAST_DORMANCY = DS_SYS_EVENT_STOP_FAST_DORMANCY,
   /**
    @see  DS_SYS_EVENT_STOP_FAST_DORMANCY
  */

  PS_SYS_EVENT_THERMAL_MITIGATION_INFO  =  DS_SYS_EVENT_THERMAL_MITIGATION_INFO,
  /**<
    @see  DS_SYS_EVENT_THERMAL_MITIGATION_INFO
  */
  
   PS_SYS_EVENT_UPDATED_DL_THROUGHPUT_TIMER    = DS_SYS_EVENT_UPDATED_DL_THROUGHPUT_TIMER,
   /**
    @see  PS_SYS_EVENT_UPDATED_DL_THROUGHPUT_TIMER
  */
 
   PS_SYS_EVENT_DL_THROUGHPUT_INFO              = DS_SYS_EVENT_DL_THROUGHPUT_INFO,
   /**
    @see  PS_SYS_EVENT_DL_THROUGHPUT_INFO
  */
 
   PS_SYS_EVENT_DATA_SERVICE_SETTING   = DS_SYS_EVENT_DATA_SERVICE_SETTING,
   /**
    @see  DS_SYS_EVENT_DATA_SERVICE_SETTING  
  */

  PS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING  = DS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING,
   /**
     @see  DS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING  
  */

  PS_SYS_EVENT_APN_INFO = DS_SYS_EVENT_APN_INFO,
   /**
     @see  DS_SYS_EVENT_APN_INFO
   */

  PS_SYS_EVENT_QUALITY_MEAS_POLICY_CHANGE = DS_SYS_EVENT_QUALITY_MEAS_POLICY_CHANGE,
   /**
    @see  DS_SYS_EVENT_QUALITY_MEAS_POLICY_CHANGE
  */

  PS_SYS_EVENT_DATA_ACTIVITY_RESUMED = DS_SYS_EVENT_DATA_ACTIVITY_RESUMED,
  /**
    @see  DS_SYS_EVENT_DATA_ACTIVITY_RESUMED
  */

  PS_SYS_EVENT_ROAMING_STATUS_CHANGE = DS_SYS_EVENT_ROAMING_STATUS_CHANGE,
  /**<        
       Event is triggered when the UE moves away from the home network and is roaming.
       User registers for this event to know if the UE is in the home network or roaming.
  */  

  PS_SYS_EVENT_COMMON_MAX            = DS_SYS_EVENT_COMMON_MAX,

  /*Common Events internal to PS*/
  PS_SYS_EVENT_COMMON_INTERNAL_MIN        = PS_SYS_EVENT_COMMON_MAX + 1, 
  PS_SYS_EVENT_MODEM_OUT_OF_SERVICE       = PS_SYS_EVENT_COMMON_INTERNAL_MIN,
  /**< 
       The event is triggered when qmimsglib sends indication for modem 
       restart in progress to sysapi on APPs Processor. 
        
       @see ds_sys_qmi_sys_event_handler
  */

  PS_SYS_EVENT_PDN_THROTTLE_INFO          = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 1,
  /**< 
       The event is triggered when PDN is throttled or un-throttled. In 
       response of event. User gets to know which PDN is throttled right now
       and what is the remaining time for each PDN throttling. After recieving
       this event, user has to go through PDN throttle table to see which PDN
       is throttled and only make the call for the PDN not throttled.
  */

  PS_SYS_EVENT_MODEM_IN_SERVICE	    	 =  PS_SYS_EVENT_COMMON_INTERNAL_MIN + 2,
  /**<
       This is internal event generated when modem is back in service,
       after modem restart.  The event is passed along to ps event -
       command handler which would reinitialize qmi client
  */

  PS_SYS_EVENT_APN_PREF_SYS_CHANGE        = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 3,
  /**<        
       Event is triggered when any user like IMS changes its technology 
       preference for data call. It results in handoff process if data call
       is not currently on that technology. Users can register whenever they 
       need to know about handoff tech change event.
  */

  PS_SYS_EVENT_PDN_STATUS_INFO       = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 4,
  /**< 
       The event is triggered when the PDN is active or inactive. In 
       response of event, user gets to know whether the pdn is up pr down and 
       the name of the apn.
  */

  PS_SYS_EVENT_NULL_RESOLVED_APN    = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 5,
  /**
       Event is triggered when EPC mode handler resolves the null apn.
  */

  PS_SYS_EVENT_LOW_LATENCY_TRAFFIC_STATUS        
                                          = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 6,
  /**<        
       Event is triggered to notify low latency traffic start and end
  */

  PS_SYS_EVENT_MODEM_POWER_STATS_CONFIGURATION        
                                          = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 10,
  /**<        
       Event is triggered to notify power stats configuration update
  */

  PS_SYS_EVENT_QUERY_MODEM_POWER_STATS        
                                          = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 11,
  /**<        
       Event is triggered to update that modem power stats is being queried
  */

  PS_SYS_EVENT_MODEM_POWER_STATISTICS        
                                          = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 12,
  /**<        
       Event is triggered to notify current modem power statistics
  */
  PS_SYS_EVENT_REMOTE_SOCKET_HDLR_CHANGE  = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 13,
   /**
       Event is triggered to notify whether remote socket handling
       is available/not available
  */
  PS_SYS_EVENT_REMOTE_SOCKET_REQUEST  = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 14,
   /**
       Event is triggered to request for remote sockets from the TE
  */
  PS_SYS_EVENT_REMOTE_SOCKET_RELEASE  = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 15,
   /**
       Event is triggered to release remote sockets on the TE
  */
  PS_SYS_EVENT_REMOTE_SOCKET_SET_OPTION  = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 16,
   /**
       Event is triggered to set remote socket option on the TE
  */


  PS_SYS_EVENT_COMMON_INTERNAL_MAX        = PS_SYS_EVENT_COMMON_INTERNAL_MIN + 100,

  PS_SYS_EVENT_WLAN_MIN                   = DS_SYS_EVENT_WLAN_MIN,
  PS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE       = DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE,
  /**< 
   User can register for this event whenever they want to know regarding
   change in MAC address.
  */
  PS_SYS_EVENT_WLAN_SSID_CHANGE           = DS_SYS_EVENT_WLAN_SSID_CHANGE,
  /**< 
   User can register for this event whenever they want to know regarding
   change in SSID.
  */

  PS_SYS_EVENT_WQE_PROFILES_MET           = DS_SYS_EVENT_WQE_PROFILES_MET,
  /**<
     @see  DS_SYS_EVENT_WQE_PROFILES_MET
  */
  
  
  PS_SYS_EVENT_WQE_PROFILE_STATUS         = DS_SYS_EVENT_WQE_PROFILES_STATUS,
  /**<
     @see  DS_SYS_EVENT_WQE_PROFILES_STATUS
  */
  
  PS_SYS_EVENT_WLAN_MAX                   = DS_SYS_EVENT_WLAN_MAX,

  PS_SYS_EVENT_WLAN_INTERNAL_MIN          = PS_SYS_EVENT_WLAN_MAX + 1,

  PS_SYS_EVENT_WLAN_PREFERENCE_CHANGE     = PS_SYS_EVENT_WLAN_INTERNAL_MIN,
  /**< 
     User can register for this event whenever they want to know regarding
     change in WLAN preference.
  */

  PS_SYS_EVENT_WLAN_STATUS_CHANGE         = PS_SYS_EVENT_WLAN_INTERNAL_MIN + 1,
  /**< 
   User can register for this event whenever they want to know regarding
   change in WLAN status.
  */
  
  PS_SYS_EVENT_WLAN_MAX_CONN         = PS_SYS_EVENT_WLAN_INTERNAL_MIN + 2,
  /**< 
   User can register for this event whenever they want to know if the max iwlan 
   conn is reached. The event would prevent new bringup/handoff when the max 
   connection is reached.
   For eg. If max pdns for iwlan is 3 and if there are already 2 pdns up on iwlan 
   and 1 pdn up on lte. If a 4th pdn comes up on iwlan, the bringup will be 
   prevented as the max connections are reached.
   During handoff scenarios, if lte to iwlan happens in the above case, an 
   indication would be sent as max connections is reached. This would prevent the
   handoff from happening. 
  */
  
  PS_SYS_EVENT_WLAN_START_WIFI_MEAS       = PS_SYS_EVENT_WLAN_INTERNAL_MIN + 3,
  /**< 
   User can register for this event whenever they want to know when to start
   sending WiFi measurements.
  */

  PS_SYS_EVENT_WLAN_STOP_WIFI_MEAS        = PS_SYS_EVENT_WLAN_INTERNAL_MIN + 4,
  /**< 
   User can register for this event whenever they want to know when to stop
   sending WiFi measurements.
  */

   PS_SYS_EVENT_WQE_PROFILE_CHANGE        = PS_SYS_EVENT_WLAN_INTERNAL_MIN + 5,
  /**<        
   Event is triggered by ANDSF to notify the new Wifi quality estimation profile
   type
  */

   PS_SYS_EVENT_NAT_KEEP_ALIVE_INFO        = PS_SYS_EVENT_WLAN_INTERNAL_MIN + 6,
  /**<        
   Event is triggered by IWLAN MH to configure AP for the Nat keep alive
   transmissions
  */

   PS_SYS_EVENT_SET_WQE_PROFILE_QUALITY_MEAS_INFO =  PS_SYS_EVENT_WLAN_INTERNAL_MIN + 7,
  /**<        
   Event is triggered by ANDSF to configure AP with the required WQE Profile quality 
   parameters for a given WQE Profiles.
  */

  PS_SYS_EVENT_WLAN_INTERNAL_MAX          = PS_SYS_EVENT_WLAN_INTERNAL_MIN + 100,

  PS_SYS_EVENT_MAX
} ps_sys_event_enum_type;

/**
  @brief Event info type for PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT event. 
  @see  PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT  
*/
typedef struct
{
  uint16         attach_profile; /**< Attach profile number for LTE */
} ps_sys_event_3gpp_lte_attach_init_type;

/**
  @brief Event info type for PS_SYS_EVENT_3GPP_LTE_ATTACH_COMPLETE event. 
  @see  PS_SYS_EVENT_3GPP_LTE_ATTACH_COMPLETE 
*/
typedef struct
{
  ps_iface_type        * v4_iface_ptr; /**< pointer to v4 iface of LTE */
  ps_iface_type        * v6_iface_ptr; /**< pointer to v6 iface of LTE */
} ps_sys_event_3gpp_lte_attach_complete_type;


/**
  @brief Event info type for 
         PS_SYS_EVENT_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_RSP event.
  @see  PS_SYS_EVENT_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_RSP  
*/
typedef struct
{
  
  boolean prereg_handoff_succeeded; /**< Flag for success/failure of prereg 
                                         handoff */
} ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type;

/**
  @brief Enum of type page monitor period result
*/
typedef enum
{
  PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_RESULT_SUCCESS =
    DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_RESULT_SUCCESS,
  PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_RESULT_FAIL_REQUEST_REJECTED =
    DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_RESULT_FAIL_REQUEST_REJECTED,
  PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_RESULT_FAIL_REQUEST_FAILED_TX =
    DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_RESULT_FAIL_REQUEST_FAILED_TX,
  PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_RESULT_FAIL_NOT_SUPPORTED =
    DS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_RESULT_FAIL_NOT_SUPPORTED
} ps_sys_event_3gpp2_evdo_page_monitor_period_status_enum_type;


/**
  @brief Event info type for PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_STATUS
         event
*/
typedef struct
{
  ps_sys_event_3gpp2_evdo_page_monitor_period_status_enum_type status;
} ps_sys_event_3gpp2_evdo_page_monitor_period_status_info_type;

/**
  @brief Event info type for PS_SYS_EVENT_DUN_CTRL_CALL_STATUS
*/
typedef struct
{
  ps_sys_tech_enum_type          tech_type;
  unsigned char                  cb_data;
  boolean                        is_call_established;
  ps_iface_net_down_reason_type  net_down_reason;
} ps_sys_event_dun_ctrl_call_status_info_type;

/**
  @brief Event info type for PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK
*/
typedef struct
{
  ps_sys_tech_enum_type          tech_type;
  unsigned char                  cb_data;
} ps_sys_event_dun_ctrl_entitlement_check_info_type;

/**
  @brief Event info type for PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK
*/
typedef struct
{
  ps_sys_tech_enum_type          tech_type;
  unsigned char                  cb_data;
  ps_iface_net_down_reason_type  net_down_reason;
} ps_sys_event_dun_ctrl_silent_redial_check_info_type;


/**
  @brief Enum representing WDS call status
*/
typedef enum
{
  PS_SYS_EVENT_DATA_CALL_ESTABLISHED = DS_SYS_EVENT_DATA_CALL_ESTABLISHED,
  PS_SYS_EVENT_DATA_CALL_TERMINATED  = DS_SYS_EVENT_DATA_CALL_TERMINATED
} ps_sys_event_data_call_status_enum_type;

/**
  @brief Enum representing different WDS call types
*/
typedef enum
{
  PS_SYS_EVENT_EMBEDDED_DATA_CALL       = DS_SYS_EVENT_EMBEDDED_DATA_CALL,
  PS_SYS_EVENT_EMBEDDED_RMNET_DATA_CALL =
    DS_SYS_EVENT_EMBEDDED_RMNET_DATA_CALL,
  PS_SYS_EVENT_TETHERED_RMNET_DATA_CALL =
    DS_SYS_EVENT_TETHERED_RMNET_DATA_CALL,
  PS_SYS_EVENT_DUN_DATA_CALL            = DS_SYS_EVENT_DUN_DATA_CALL,
} ps_sys_event_data_call_type_enum_type;


/**
  @brief Data type to be used with PS_SYS_EVENT_DATA_CALL_STATUS_CHANGE
*/
typedef struct
{
  ps_sys_event_data_call_status_enum_type        call_status;
  ps_sys_event_data_call_type_enum_type          call_type;
} ps_sys_event_data_call_status_change_type;

/**
  @brief Data type to be used with 
         PS_SYS_EVENT_3GPP2_OUTAGE_NOTIFICATION
  @see   PS_SYS_EVENT_3GPP2_OUTAGE_NOTIFICATION
*/

typedef ds_sys_event_outage_notification_type ps_sys_event_outage_notification_type;

/**
  @brief This is PS Sys level Callback function provided by client which is to 
         be called when particular event is triggered for a given technology.
  @see   ds_sys_event_cback_f_ptr_type 
  @see   ps_sys_event_reg 
*/
typedef void (* ps_sys_event_cback_f_ptr_type)
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/**
  @brief This is PS Sys level Callback function provided by client which is to 
         be called while registering for event using ps_sys_event_reg_ex API
   
  @param[in] tech_type            States Technology type
  @param[in] event_name           Name of event
  @param[in] subscription_id      Subscription id
  @param[in] event_info_ptr       Event related information
  @param[in] user_data_ptr        User Data 
   
  @see ds_sys_event_reg  
*/
typedef void (* ps_sys_event_cback_f_ptr_ex_type)
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/**
  @brief Data type to be used with 
         PS_SYS_EVENT_NAT_KEEP_ALIVE_INFO_IND
  @see   PS_SYS_EVENT_NAT_KEEP_ALIVE_INFO_IND
*/
typedef struct
{
  boolean                                        is_start;
  ps_ip_addr_type                                dest_ip_addr;
  uint16                                         source_port;
  uint16                                         dest_port;
  uint32                                         timer_value;
} ps_sys_event_nat_keep_alive_info_type;

/**
  @brief Data type to be used with 
         PS_SYS_EVENT_QUALITY_MEAS_POLICY_CHANGE
  @see   DS_SYS_EVENT_QUALITY_MEAS_POLICY_CHANGE
*/
typedef ds_sys_quality_meas_policy_change_type 
  ps_sys_quality_meas_policy_change_type;

/**
  @brief Data type to be used with 
         PS_SYS_EVENT_SET_WQE_PROFILE_QUALITY_MEAS_INFO
  @see   DS_SYS_EVENT_SET_WQE_PROFILE_QUALITY_MEAS_INFO
*/
typedef ds_sys_wqe_profile_quality_meas_info_type
  ps_sys_event_set_wqe_profile_quality_meas_info_type;

/**
  @brief Data type to be used with  
         PS_SYS_EVENT_WQE_PROFILE_STATUS
  @see   DS_SYS_EVENT_WQE_PROFILE_STATUS 
*/
typedef ds_sys_wqe_profiles_status_type ps_sys_wqe_profiles_status_type;


/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/**
  @brief  This is PS Sys level API for event registeration. This function 
          allows client to register for particual event. In order to
          deregister the event registered using ds_sys_event_reg, 
          user has to call ds_sys_event_dereg API
          
  @see    ds_sys_event_reg
*/
int16 ps_sys_event_reg
(
  ps_sys_tech_enum_type                  tech_type,
  ps_sys_event_enum_type                 event_name,
  ps_sys_event_cback_f_ptr_type          event_cback_f_ptr,
  void                                 * user_data_ptr,
  int16                                * ps_errno
);


/** 
  @brief  This is PS Sys level API for event deregistration. This function 
          deregisters callback for the event.Callback registered through 
          ps_sys_event_reg should be deregistered using this API
          
  @see    ds_sys_event_dereg
*/
int16 ps_sys_event_dereg
(
  ps_sys_tech_enum_type                  tech_type,
  ps_sys_event_enum_type                 event_name,
  ps_sys_event_cback_f_ptr_type          event_cback_f_ptr,
  int16                                * ps_errno
);

/**
  @brief  This is PS Sys level API for event deregistration. This function 
          deregisters callback for the event for particular subscription
          id. Callback registered through ps_sys_event_reg_ex
          should be deregistered using this API.
          
  @see    ds_sys_event_dereg_ex
*/
int16 ps_sys_event_dereg_ex
(
  ps_sys_tech_enum_type                  tech_type,
  ps_sys_event_enum_type                 event_name,
  ps_sys_subscription_enum_type          subscription_id,
  ps_sys_event_cback_f_ptr_ex_type       event_cback_f_ptr,
  int16                                * ps_errno
);


/** 
  @brief  This function sends the indication to all the user who has
          registered for the event for particular technology and default
          subscription

  @param[in] tech_type            States Technology type
  @param[in] event_name           Name of event
  @param[in] event_info           Event specific information 
  @param[out] ps_errno            Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure. 
   
  @code 
   
  Example : 
  Assuming user has already registered for PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT. 
  Modehandlers sends out event indication if there is context transfer from 
  other RAT as follow 
   
  ps_sys_event_3gpp_lte_attach_init_type event_info = NULL;
  event_info.attach_profile = 344;
   
  ret_val =  ps_sys_event_ind(PS_SYS_TECH_3GPP,
                              PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT,
                              &event_info,
                              &ps_errno);
  if(0 > ret_val)
  {
    LOG_MSG_ERROR_1("Event indication failed, err %d", ps_errno);
  } 
   
  @endcode 
*/
int16 ps_sys_event_ind
(
  ps_sys_tech_enum_type             tech_type,
  ps_sys_event_enum_type            event_name,
  void                            * event_info,
  int16                           * ps_errno
);

/** 
  @brief  This function sends the indication to all the user who has
          registered for the event for particular technology and
          subscription

  @param[in] tech_type            States Technology type
  @param[in] event_name           Name of event
  @param[in] subscription_id      Subscription Id
  @param[in] event_info           Event specific information 
  @param[out] ps_errno            Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure. 
*/
int16 ps_sys_event_ind_ex
(
  ps_sys_tech_enum_type             tech_type,
  ps_sys_event_enum_type            event_name,
  ps_sys_subscription_enum_type     subscription_id,
  void                            * event_info,
  int16                           * ps_errno
);

/** 
  @brief  This is PS Sys level API for event registration with ability to
          users to apply filter based on subscription id.In order to 
          deregister the event registered using ds_sys_event_reg_ex,  
          user has to use ps_sys_event_dereg_ex API
          
  @see    ds_sys_event_reg_ex
*/
int16 ps_sys_event_reg_ex
(
  ps_sys_tech_enum_type                  tech_type,
  ps_sys_event_enum_type                 event_name,
  ps_sys_subscription_enum_type          subscription_id,
  ps_sys_event_cback_f_ptr_ex_type       event_cback_f_ptr,
  void                                 * user_data_ptr,
  int16                                * ps_errno
);


#ifdef __cplusplus
}
#endif

#endif /* PS_SYS_EVENT_H */
