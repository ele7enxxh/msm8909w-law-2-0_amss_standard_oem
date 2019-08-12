#ifndef PS_SYS_CONFI_H
#define PS_SYS_CONFI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ C O N F I . H


GENERAL DESCRIPTION
  This is internal header file that defines functions, enums and structs 
  internal to ps_sys configuration module

Copyright (c) 2011-15 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ps_sys_confi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
03/10/12    msh    Added changes for PS_SYS_CONF_3GPP2_AN_AUTH_STATUS  
09/09/11    bvd    Created module 
 
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ps_sys_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/
/**
  @brief Temporary storage for configuration data 
*/
typedef struct
{
  ps_sys_tech_enum_type              tech_type;
  ps_sys_conf_enum_type              conf_name;
  void                              *argval_ptr;
} ps_sys_confi_data;


/**
  @brief Temporary storage for configuration data 
*/
typedef struct
{
  ps_sys_system_status_type             * system_status_type;
  ps_sys_conf_enable_loopback_type      * enable_loopback_ptr;
  ps_sys_conf_get_dun_ctrl_pref_type    * get_dun_ctrl_pref_ptr;
  ps_sys_system_status_ex_type          * system_status_ex_ptr;
  ps_sys_apn_pref_sys_type              * apn_pref_sys_ptr;
  ps_sys_conf_throughput_timer_type     * throughput_timer_ptr;
  ps_sys_throughput_info_list_type      * throughput_info_ptr;
  ps_sys_report_dl_timer_type           * dl_thrpt_timer_ptr;
  ps_sys_dl_throughput_info_type        * dl_thrpt_info_ptr;
  ps_sys_report_dl_timer_type           * min_dl_thrpt_timer_ptr;
  ps_sys_thermal_mitigation_action_enum_type * thermal_info_ptr;
  ps_sys_data_services_setting_type     * data_setting_ptr;
  ps_sys_data_services_roaming_setting_type *data_roaming_setting_ptr;
  ps_sys_apn_info_type                  *apn_info_ptr;
  ps_sys_modem_power_stats_config_type  * power_stats_config_ptr;
  ps_sys_modem_power_stats_info_type    * power_stats_info_ptr;
  ps_sys_data_services_wifi_setting_type  * wifi_settings_ptr;
  ps_sys_remote_socket_hdlr_avail_type  * hdlr_avail;  
  ps_sys_roaming_status_info_type      * roaming_status_info_ptr;
} ps_sys_confi_tech_all;

/**
  @brief Temporary storage for 3gpp2 configuration data 
*/
typedef struct
{
  ps_sys_3gpp2_s101_status_change_type  * s101_status_change_type;
  ps_sys_3gpp2_epzid_info_type          * epzid_info_type;
  ps_sys_conf_3gpp2_an_auth_status_enum_type * an_auth_status;
  ps_sys_null_resolved_apn_info_type    * null_resolved_apn_ptr;
  ps_sys_3gpp2_network_qos_aware_status * net_aware_status;
} ps_sys_confi_tech_3gpp2;

/**
  @brief Temporary storage for 3gpp configuration data 
*/
typedef struct
{
  ps_sys_lte_attach_enum_type           * lte_attach_type;
  ps_sys_is_pdn_only_emergency_type     * only_emergency_pdn_info;
  ps_sys_roaming_disallowed_info_type   * roaming_disallowed_info_ptr;
  ps_sys_null_resolved_apn_info_type    * null_resolved_apn_ptr;
} ps_sys_confi_tech_3gpp;

/**
  @brief Temporary storage for wlan configuration data 
*/
typedef struct
{
  ps_sys_wlan_status_type               * wlan_status;
  ps_sys_wlan_preference_enum_type      * wlan_preference;
  ps_sys_wlan_max_conn_info_type        * wlan_max_conn_ptr;
  ps_sys_wlan_mac_addr_type             * wlan_mac_addr_ptr;
  ps_sys_wlan_ssid_type                 * wlan_ssid_ptr;
  ps_sys_null_resolved_apn_info_type    * null_resolved_apn_ptr;
} ps_sys_confi_tech_wlan;




/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/


/**
  @brief  This function performs operations to allocate memory for temporary 
          storage
  
  @return 
    None 
*/
void ps_sys_confi_alloc_temp_data
(
  void
);

/**
  @brief  This function verifies authencity of configuration name and technology
          name.

  @param[in] tech_type           States Technology type 
  @param[in] conf_name           Configuration name  
   
  @return  DSS_SUCCESS          On Success.
  @return  DSS_ERROR            On failure.  
  
*/
int ps_sys_confi_verify_tech_conf_name
(
  ps_sys_tech_enum_type                         tech_type,
  ps_sys_conf_enum_type                         conf_name
);

#ifdef __cplusplus
}
#endif

#endif /* PS_SYS_CONFI_H */
