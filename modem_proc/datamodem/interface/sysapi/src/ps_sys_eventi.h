#ifndef PS_SYS_EVENTI_H
#define PS_SYS_EVENTI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ E V E N T . H 


GENERAL DESCRIPTION
  This is internal header file that defines functions, enums and structs
  for registering and deregistering events

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ps_sys_eventi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
 
when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
09/09/11    bvd    Created module 
 
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "ps_sys.h"
#include "ps_svc.h"
#include "ps_sys_event.h"


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/


/** 
  @brief This is sys event queue for all the callbacks registered for all the
         events
*/
q_type  ps_sys_eventi_q;


/** 
  @brief This is buffer that is used to allocate event callbacks, tech_type and
         others and passed on to PS task in order for callback to be called
*/
typedef struct
{
  q_link_type                      link;
  q_type                          *event_q_ptr;
  ps_sys_event_cback_f_ptr_type    event_cback_f_ptr;
  ps_sys_event_cback_f_ptr_ex_type event_cback_f_ex_ptr;
  ps_sys_tech_enum_type            tech_type;
  ps_sys_event_enum_type           event_name;
  uint32                           subscription_id;
  void                            *user_data_ptr;
} ps_sys_eventi_buf_type;



/** 
  @brief This is structure sent along with invoke event callback command to 
          ps task
  */
typedef struct
{
  ps_sys_tech_enum_type            tech_type;
  ps_sys_event_enum_type           event;
  void                           * event_info;
  ps_sys_subscription_enum_type    subscription_id;
} ps_sys_eventi_cmd_type;


/**
  @brief Union of all event structures. Its used in ps_sys_event_ind function.
*/
typedef union
{
  ps_sys_system_status_type                   system_status_type;

  ps_sys_pdn_throttle_info_type               pdn_throttle_info_type;

  ps_sys_event_3gpp_lte_attach_init_type      lte_attach_init_type;

  ps_sys_event_3gpp_lte_attach_complete_type  lte_attach_complete_type;

  ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type  
    handoff_complete_rsp_type;

  ps_sys_3gpp2_s101_status_change_type        s101_status_change_type;

  ps_sys_event_3gpp2_evdo_page_monitor_period_status_info_type 
    evdo_page_monitor_period_status_type;

  ps_sys_3gpp2_evdo_page_monitor_period_type 
    evdo_page_monitor_period_change_type;

  ps_sys_event_dun_ctrl_call_status_info_type            
    dun_ctrl_call_notif_type;

  ps_sys_event_dun_ctrl_entitlement_check_info_type     
    dun_ctrl_entitlement_notif_type;

  ps_sys_event_dun_ctrl_silent_redial_check_info_type   
    dun_ctrl_silent_redial_notif_type;

  ps_sys_event_data_call_status_change_type  call_status_change_type;

  ps_sys_3gpp2_epzid_info_type                epzid_info_type;
  
  ps_sys_system_status_ex_type                system_status_ex;

  ps_sys_apn_pref_sys_type                    apn_pref_sys;

#if defined(FEATURE_DATA_LTE)
  ps_sys_3gpp_lte_attach_pdn_list_type        lte_attach_pdn_list;
#endif /* defined(FEATURE_DATA_LTE) */

  ps_sys_lte_attach_enum_type                lte_attach_type;

  ps_sys_pdn_status_info_type                 pdn_status;

  ps_sys_low_latency_traffic_info_type       low_latency_traffic_info;

  ps_sys_wlan_wifi_meas_start_type           start_wifi_meas_info;

  ps_sys_wlan_wifi_meas_stop_type            stop_wifi_meas_info;

} ps_sys_eventi_info_type;


/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/**
  @brief  This function calls all callbacks for given event.

  @param[in] tech_type            States Technology type
  @param[in] event                Name of event
  @param[in] event_info           Event information 
  @param[in] subscription_id      Subscription id

*/
void ps_sys_eventi_invoke_cbacks
(
  ps_sys_tech_enum_type                      tech_type,
  ps_sys_event_enum_type                     event,
  void                                     * event_info,
  ps_sys_subscription_enum_type              subscription_id
);


/** 
  @brief  This function deallocates event callback buffer. 

  @param[in] buf             Event Buffer to deallocate 
*/
void ps_sys_eventi_free_cback_buf
(
  void *buf
);

/** 
  @brief  This function allocates buffer for given event and tech
          type and subscription id.

  @param[in] tech_type            States Technology type
  @param[in] event_cback_f_ptr    Callback for event 
  @param[in] event_cback_f_ex_ptr Callback for event
  @param[in] user_data_ptr        Pointer to User data 
  @param[in] subscription_id      Subscription Id
  
  @return 
    Allocated Memory Buffer or Null on error 
*/
void *ps_sys_eventi_alloc_cback_buf
(
  ps_sys_tech_enum_type             tech_type,
  ps_sys_event_cback_f_ptr_type     event_cback_f_ptr,
  ps_sys_event_cback_f_ptr_ex_type  event_cback_f_ex_ptr,
  void                            * user_data_ptr,
  uint32                            subscription_id,
  ps_sys_event_enum_type            event_name
);


/** 
  @brief  This function is callback for command PS_SYS_EVENT_CMD_HANDLER
          called in ps task context

  @param[in] cmd                  Command type 
  @param[in] user_data_ptr        Pointer to User data 
  
  @return 
    Allocated Memory Buffer or Null on error 
*/
void ps_sys_eventi_cback_cmd_handler
(
  ps_cmd_enum_type    cmd,
  void *              userDataPtr
);


/** 
  @brief  This function checks if event callback for given tech type
          and event exists. It accepts two callback function. 
          One of them should be non NULL.

  @param[in] tech_type            States Technology type 
  @param[in] event_name           Name of event  
  @param[in] event_cback_f_ptr    Callback for event  
  @param[in] event_cback_f_ex_ptr Callback for event

  @retval TRUE                    Event callback exists 
  @retval FALSE                   Event Callback doesnt exist  
*/
boolean ps_sys_eventi_check_event_cback_exist
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event,
  ps_sys_event_cback_f_ptr_type                  event_cback_f_ptr,
  ps_sys_event_cback_f_ptr_ex_type               event_cback_f_ex_ptr,
  ps_sys_subscription_enum_type                  subscription_id
);


/**
  @brief  This function verifies event name and tech name parameters.

  @param[in] tech_type          States Technology type 
  @param[in] event_name         Name of event  
   
  @return  DSS_SUCCESS          On Success.
  @return  DSS_ERROR            On failure.  
  
*/
int ps_sys_eventi_verify_tech_event
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name
);

/** 
  @brief  This function empties event queue.

  @param[in] event_name          Name of event
  @param[out] ps_errno           DS_EFAULT if parameter is 
                                 invalid DS_EINVAL if operation
                                 is invalid DS_NOMEMORY if
                                 memory allocation fails

  @return  DSS_SUCCESS          On Success.
  @return  DSS_ERROR            On failure.
  */
int16 ps_sys_eventi_empty_event_queue
(
  ps_sys_event_enum_type                 event_name,
  int16                                * ps_errno
);

/** 
  @brief  This function registers user for particular event. It accepts
           two callback function. One of them should be non NULL.

  @param[in] tech_type            States Technology type
  @param[in] event_name           Name of event
  @param[in] subscription_id      Subscription Id
  @param[in] event_cback_f_ptr    Callback function when event is triggered
  @param[in] event_cback_f_ex_ptr Callback function when event is triggered
  @param[in] user_data_ptr        User Data
  @param[out] dss_errno           Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS           On Success.
  @return  DSS_ERROR             On failure.
 
*/
int16 ps_sys_eventi_reg
(
  ps_sys_tech_enum_type                  tech_type,
  ps_sys_event_enum_type                 event_name,
  ps_sys_subscription_enum_type          subscription_id,
  ps_sys_event_cback_f_ptr_type          event_cback_f_ptr,
  ps_sys_event_cback_f_ptr_ex_type       event_cback_f_ex_ptr,
  void                                 * user_data_ptr,
  int16                                * ps_errno
);

/**
  @brief  This function deregisters user for particular event. It accepts
           two callback function. One of them should be non NULL.

  @param[in] tech_type            States Technology type
  @param[in] event_name           Name of event
  @param[in] subscription_id      Subscription Id
  @param[in] event_cback_f_ptr    Callback function when event is triggered
  @param[in] event_cback_f_ex_ptr Callback function when event is triggered
  @param[in] user_data_ptr        User Data
  @param[out] dss_errno           Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  @return  DSS_SUCCESS          On Success.
  @return  DSS_ERROR            On failure.
 
*/
int16 ps_sys_eventi_dereg
(
  ps_sys_tech_enum_type                  tech_type,
  ps_sys_event_enum_type                 event_name,
  ps_sys_subscription_enum_type          subscription_id,
  ps_sys_event_cback_f_ptr_type          event_cback_f_ptr,
  ps_sys_event_cback_f_ptr_ex_type       event_cback_f_ex_ptr,
  int16                                * ps_errno
);

#ifdef __cplusplus
}
#endif

#endif /* PS_SYS_EVENTI_H */
