#ifndef DS3GEVENTMGR_H
#define DS3GEVENTMGR_H
/*===========================================================================

                3 G   D A T A   S E R V I C E S   E V E N T   M A N A G E R

                           H E A D E R   F I L E

GENERAL DESCRIPTION
  This software unit contains functions for 3G Data Services event handling.
  It provides APIs for DS3G event registration and notification.

  Copyright (c) 2001-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:
  $Header:

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/03/14   rs      Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "ps_sys.h"
#include "ps_sys_event.h"
#include "ds3gsubsmgr.h"

#include "sys.h"
#include "sys_v.h"
#include "dstask_v.h"

#ifdef FEATURE_MMGSDI
#include "mmgsdisessionlib.h"
#endif /* FEATURE_MMGSDI */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define DS3G_TECH_ALL                0x07

/*------------------------------------------------------------------------
   Client id type
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GEVENTMGR_CLIENT_ID_MIN       = 0x01,
  DS3GEVENTMGR_CLIENT_ID_3GPP2     = DS3GEVENTMGR_CLIENT_ID_MIN,
  DS3GEVENTMGR_CLIENT_ID_3GPP      = 0x02,
  DS3GEVENTMGR_CLIENT_ID_MPPM      = 0x04,
  DS3GEVENTMGR_CLIENT_ID_WLAN      = 0x08,
  DS3GEVENTMGR_CLIENT_ID_EPC       = 0X10,
  DS3GEVENTMGR_CLIENT_ID_ATCOP     = 0x20,
  DS3GEVENTMGR_CLIENT_ID_PROFILE   = 0x40,
  DS3GEVENTMGR_CLIENT_ID_KAMGR     = 0x80,
  DS3GEVENTMGR_CLIENT_ID_DATA_TEST = 0x100,
  DS3GEVENTMGR_CLIENT_ID_APPSRV    = 0x200,
  DS3GEVENTMGR_CLIENT_ID_MAX       = DS3GEVENTMGR_CLIENT_ID_APPSRV
}ds3geventmgr_client_type;

/*-------------------------------------------------------------------------
  LPM POWER OFF Bitwise Configuration
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK      = 0x01,
  DS3GEVENTMGR_LPM_PWROFF_3GPP_CTRL_BIT_MSK       = 0x02,
  DS3GEVENTMGR_LPM_PWROFF_WLAN_CTRL_BIT_MSK       = 0x04,
  DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK     = 0x80
}ds3geventmgr_lpm_pwr_off_bit_msk_type;

/*-------------------------------------------------------------------------
  SUB_NOT_AVAIL Bitwise Configuration
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK      = 0x01,
  DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK       = 0x02,
  DS3GEVENTMGR_SUB_NOT_AVAIL_MPPM_CTRL_BIT_MSK       = 0x04,
  DS3GEVENTMGR_SUB_NOT_AVAIL_WLAN_CTRL_BIT_MSK       = 0x08,
  DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK     = 0x80
}ds3geventmgr_sub_not_avail_bit_msk_type;

/*-------------------------------------------------------------------------
  DS3G External Events Enum Type for DS3G events
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GEVENTMGR_INVALID_EV                   = -1,
  DS3GEVENTMGR_MIN_EV,
  DS3GEVENTMGR_LTE_ATTACH_INIT_EV           = DS3GEVENTMGR_MIN_EV,
  DS3GEVENTMGR_NULL_RESOLVED_APN_EV,
  DS3GEVENTMGR_CONSOLIDATED_RESOLVED_APN_EV,
  DS3GEVENTMGR_PDN_STATUS_INFO_EV,
  DS3GEVENTMGR_PDN_THROTTLE_INFO_EV,
  DS3GEVENTMGR_WLAN_MAX_CONN_EV,
  DS3GEVENTMGR_S101_STATUS_CHG_EV,
  DS3GEVENTMGR_EHRPD_PREREG_HANDOFF_COMPLETE_RESP_EV,
  DS3GEVENTMGR_MODE_LPM_EV,                  
  DS3GEVENTMGR_NV_REFRESH_EV,
#ifdef FEATURE_MMGSDI
  DS3GEVENTMGR_MMGSDI_SUBSCRIPTION_READY_EV,
  DS3GEVENTMGR_MMGSDI_REFRESH_VOTING_EV,
  DS3GEVENTMGR_MMGSDI_OK_REFRESH_EV,
  DS3GEVENTMGR_MMGSDI_REFRESH_FCN_EV,
  DS3GEVENTMGR_MMGSDI_CARD_REFRESH_COMPLETE_EV,
  DS3GEVENTMGR_MMGSDI_SESSION_CHANGED_EV,
  DS3GEVENTMGR_MMGSDI_CARD_ERROR_EV,
  DS3GEVENTMGR_MMGSDI_IMSI_INFO_EV,
  DS3GEVENTMGR_MMGSDI_IMPI_INFO_EV,
#endif /* FEATURE_MMGSDI */
  DS3GEVENTMGR_THERMAL_MITIGATION_EV,
  DS3GEVENTMGR_DATA_CHANGED_EV,
  DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV,
  DS3GEVENTMGR_APN_CHANGED_EV,
  DS3GEVENTMGR_PLMN_LIST_CHG_EV,
  DS3GEVENTMGR_LATENCY_INFO_EV,
  DS3GEVENTMGR_PROFILE_APN_NAME_CHG_EV,
  DS3GEVENTMGR_WLAN_PROXY_INFO_CHG_EV,
  DS3GEVENTMGR_SUB_NOT_AVAIL_EV,
  DS3GEVENTMGR_SUB_AVAIL_EV,
  DS3GEVENTMGR_WLAN_IDI_TYPE_EV,
  DS3GEVENTMGR_ROAMING_STATUS_EV,
  DS3GEVENTMGR_MAX_EV
}ds3geventmgr_event_type;

typedef enum
{
    DS3GEVENTMGR_TH_MIT_MIN                       = 0,
    DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS           = DS3GEVENTMGR_TH_MIT_MIN,
    DS3GEVENTMGR_TH_MIT_ALLOW_ONLY_IMS_CALLS,
    DS3GEVENTMGR_TH_MIT_TEAR_DOWN_BLOCK_ALL_CALLS,
    DS3GEVENTMGR_TH_MIT_MAX
}ds3geventmgr_th_mit_action_type;

/*-------------------------------------------------------------------------
  Filter Structure, used for various filter options while publishing info.
-------------------------------------------------------------------------*/
typedef struct
{
  ps_sys_subscription_enum_type          ps_subs_id;
  ps_sys_tech_enum_type                  tech;
}ds3geventmgr_filter_type;

/*-------------------------------------------------------------------------
  Structure to contain attach prof information
-------------------------------------------------------------------------*/
typedef struct
{
  boolean                                     emergency_calls_supported;
  char                                        apn_name[DS_SYS_MAX_APN_LEN];
}ds3geventmgr_attach_prof_info_type;

/*-------------------------------------------------------------------------
  Structure to notify LTE attach Init info
-------------------------------------------------------------------------*/
typedef struct
{
  boolean                                     in_progress;
  ds3geventmgr_attach_prof_info_type          prof_info;
}ds3geventmgr_attach_info_type;

typedef struct
{
  mmgsdi_slot_id_enum_type slot;
}ds3geventmgr_nv_refresh_info_type;

#ifdef FEATURE_MMGSDI
typedef struct
{
  mmgsdi_session_id_type               session_id;
  mmgsdi_session_type_enum_type        session_type;
  mmgsdi_app_enum_type                 app_type;
  mmgsdi_slot_id_enum_type             slot;
  mmgsdi_session_changed_evt_info_type session_changed_info; 
}ds3geventmgr_mmgsdi_info_type;
#endif /* FEATURE_MMGSDI */

typedef struct
{
  ds3geventmgr_th_mit_action_type action;
}ds3geventmgr_thermal_mitigation_info_type;

typedef struct
{
  sys_plmn_list_type_e_type plmn_type;
}ds3geventmgr_plmn_list_chg_info_type;

typedef struct
{
  boolean             is_roaming;
  sys_sys_mode_e_type sys_mode;
}ds3geventmgr_roaming_status_info_type;

typedef struct
{
  ps_sys_pdn_status_info_type ps_pdn_status;
  ps_sys_rat_ex_enum_type     rat;
  uint16                      profile_num;
}ds3geventmgr_pdn_status_info_type;

typedef struct
{
  byte old_apn_name[DS_SYS_MAX_APN_LEN];
  byte new_apn_name[DS_SYS_MAX_APN_LEN];
}ds3gevent_mgr_apn_name_chg_info_type;

typedef enum
{
  DS3GEVENT_MGR_IDI_TYPE_IMSI = 0,
  DS3GEVENT_MGR_IDI_TYPE_IMPI = 1
}ds3gevent_mgr_idi_enum_type;


typedef struct
{
  boolean            wlan_availability;
  ps_ip_addr_type    ipv4_addr;
  ps_ip_addr_type    ipv6_addr;

  ps_ip_addr_type    dns_v4_addr_1;
  ps_ip_addr_type    dns_v4_addr_2;
  ps_ip_addr_type    dns_v6_addr_1;
  ps_ip_addr_type    dns_v6_addr_2;
  char               ssid[PS_SYS_MAX_SSID_LEN];
  uint8              bssid[PS_SYS_MAC_ADDR_LEN];

  uint32             info_change_mask;
}ds3geventmgr_wlan_info_type;

typedef struct
{
  boolean                 is_valid;
  nv_ehrpd_imsi_type      imsi_data;
  mmgsdi_app_enum_type    app_type;
}ds3geventmgr_imsi_info_type;

typedef struct
{
  boolean                 is_valid;
  nv_ehrpd_imsi_type      impi_data;
}ds3geventmgr_impi_info_type;

typedef struct
{
  ds3gevent_mgr_idi_enum_type  idi_type;
}ds3geventmgr_idi_info_type;

/*-------------------------------------------------------------------------
  Currently this enum only defines granular 3GPP RAT information. 
  It can be updated later to add granular 3GPP2 RAT information, if needed.
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GEVENT_MGR_THROT_RAT_UNSPECIFIFED = 0,
  DS3GEVENT_MGR_THROT_RAT_LTE          = 1,
  DS3GEVENT_MGR_THROT_RAT_UMTS         = 2,
  DS3GEVENT_MGR_THROT_RAT_MAX          = 3
} ds3gevent_mgr_throt_rat_type;

/*-------------------------------------------------------------------------
  This structure defines the PDN Throttling Information to be sent to
  Internal Clients within DS
-------------------------------------------------------------------------*/
typedef struct
{
 ps_sys_pdn_throttle_info_type  pdn_throttle_info;
                                  /* PDN Throttle Information*/
 ds3gevent_mgr_throt_rat_type   throttled_rat;  
                                  /* RAT for which given PDN Throttling Info
                                     is applicable. Only 3GPP MH and EPC
                                     need to populate this. Default value
                                     is DS3GEVENT_MGR_THROT_RAT_UNSPECIFIFED */
} ds3gevent_mgr_throttle_info_type;


/*-------------------------------------------------------------------------
  Structure to notify events
-------------------------------------------------------------------------*/
typedef union
{
  ds3geventmgr_nv_refresh_info_type           nv_refresh_info;
  ds3geventmgr_mmgsdi_info_type               mmgsdi_info;
  ds3geventmgr_attach_info_type          lte_attach_info;
  ds3geventmgr_pdn_status_info_type           pdn_status_info;
  ds3gevent_mgr_throttle_info_type          pdn_throttle_info;
  ps_sys_wlan_max_conn_info_type         wlan_max_conn_info;
  ps_sys_null_resolved_apn_info_type     null_resolved_apn_info;
  ds3geventmgr_thermal_mitigation_info_type   thermal_mitigation_info;
  ps_sys_low_latency_traffic_info_type   latency_info;
  void                                  *event_payload_p;
  ds3geventmgr_plmn_list_chg_info_type        plmn_list_chg_info;
  ds3geventmgr_roaming_status_info_type       roaming_status_info;
  ds3gevent_mgr_apn_name_chg_info_type   apn_name_chg_info;
  ds3geventmgr_imsi_info_type                 imsi_info;
  ds3geventmgr_impi_info_type                 impi_info;
  ds3geventmgr_idi_info_type                  idi_info;
  ds3geventmgr_wlan_info_type                 wlan_info;
}ds3geventmgr_event_info_type;

/*------------------------------------------------------------------------
   Clients Registers a callback function with DS3GCMIF module
   for events.
-------------------------------------------------------------------------*/
typedef void (*ds3geventmgr_notify_func_type)(
           ds3geventmgr_event_type       event_id,
           ds3geventmgr_filter_type      *ps_filter_info,
           void                          *event_info_ptr,
           void                          *data_ptr);
/*------------------------------------------------------------------------
DESCRIPTION   Returns Enum which is a mapping Subs ID to a Mask 
              (Single Sub at a time)
-------------------------------------------------------------------------*/
#define DS3GEVENTMGR_MAP_SUBS_ID_TO_MASK(subs_id, subs_mask)    \
    subs_mask = (DS3GSUBSMGR_SUBS_ID_MIN+1) << subs_id;

/*------------------------------------------------------------------------
DESCRIPTION   Returns Enum which is a mapping Subs Mask to Subs ID 
              (Single Sub at a time)
-------------------------------------------------------------------------*/
#define DS3GEVENTMGR_MAP_SUBS_MASK_TO_ID(subs_id, subs_mask)    \
    subs_id = (sys_modem_as_id_e_type)(subs_mask >> 1);

/*------------------------------------------------------------------------
DESCRIPTION   Returns Enum corresponding to a tech type at a time
-------------------------------------------------------------------------*/
#define DS3GEVENTMGR_SET_TECH_TYPE_MASK(tech, tech_mask)    \
    tech_mask = 1 << tech;

/*------------------------------------------------------------------------
DESCRIPTION   Returns Enum corresponding to a tech type at a time
-------------------------------------------------------------------------*/
#define DS3GEVENTMGR_MAP_DS3G_TO_PS_TECH(ps_tech, tech_mask)    \
    ps_tech = (ps_sys_tech_enum_type)(tech_mask >> 1);

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DS3GEVENTMGR_INIT

DESCRIPTION   This function performs initialization of ds3geventmgr event q.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_init(void);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_PROCESS_MCFG_REFRESH_IND_CMD

DESCRIPTION   This function process MCFG refresh ind command

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_process_mcfg_refresh_ind_cmd
(
  ds_cmd_type  *cmd_ptr
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /*TEST_FRAMEWORK*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_EVENT_REG

DESCRIPTION   Called by clients to register to get DS3GCMIF events.

DEPENDENCIES  None.

RETURN VALUE  TRUE: If registration successful.
              FALSE: If registration unsuccessful.

SIDE EFFECTS  Registered client is entered on a queue.
===========================================================================*/
boolean ds3geventmgr_event_reg
(
  ds3geventmgr_event_type         event_id,
  ds3geventmgr_client_type        client_id,
  ds3geventmgr_filter_type       *filter_info_p,
  ds3geventmgr_notify_func_type   cb_func_ptr,
  void                           *cb_data_ptr
);

/*===========================================================================
FUNCTION      DS3G_EVENT_DEREG

DESCRIPTION   Called by clients to de-register DS3CMIF event notification.

DEPENDENCIES  None.

RETURN VALUE  TRUE: If de-registration successful.
                    If client previously de-registgered.

              FALSE: If de-registration unsuccessful.

SIDE EFFECTS  De-registered client is dequeued.
===========================================================================*/
boolean ds3geventmgr_event_dereg
(
  ds3geventmgr_client_type       client_id,
  ds3geventmgr_event_type        event_id,
  ds3geventmgr_filter_type      *filter_info_p
);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_CLR_AND_NOTIFY_LPM_PWR_OFF_STATUS

DESCRIPTION   Sets the Incoming LPM/Power off Bit mask. If all the
              DS internal modules has finished processing successfully, 
              this function notifies CM that the processing is complete
 
DEPENDENCIES  None 

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_clr_and_notify_lpm_pwr_off_status
(
  ds3geventmgr_lpm_pwr_off_bit_msk_type bit_msk
);

/*===========================================================================
FUNCTION      DS3G_IS_LPM_PWR_OFF_MODE_PROCESSING_IN_PROGRESS

DESCRIPTION   returns whether DS processing of LPM/Power off mode is 
              still in progress (or) complete 
 
DEPENDENCIES  None 

RETURN VALUE  TRUE  --> LPM/PWR off processing still in progress 
              FALSE --> LPM/PWR Off processing complete/No LPM/PWR off
                        processing in progress

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress(void);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_SET_LPM_PWR_OFF_STATUS

DESCRIPTION   Sets master control bit for LPM processing to indicate 
              DS processing of LPM/Power off mode has been initiated
 
DEPENDENCIES  None 

RETURN VALUE  TRUE  - If the status bit has been set successfully
              FALSE - if the status bit cannot be set

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_set_lpm_pwr_off_status
(
  ds3geventmgr_lpm_pwr_off_bit_msk_type status
);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_SET_SUB_NOT_AVAIL_STATUS

DESCRIPTION   Sets the master control bit for SUB_NOT_AVAIL processing
              to indicate DS processing of SUB_NOT_AVAIL has been initiated
 
DEPENDENCIES  None 

RETURN VALUE  TRUE  - If the status bit has been set successfully
              FALSE - if the status bit cannot be set

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_set_sub_not_avail_status
(
  ds3geventmgr_sub_not_avail_bit_msk_type  client_id_mask,
  sys_modem_as_id_e_type                  subs_id
);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_CLR_AND_NOTIFY_SUB_NOT_AVAIL_STATUS

DESCRIPTION   Sets the Incoming SUB_NOT_AVAIL Bit mask. If all the
              DS internal modules has finished processing successfully, 
              this function notifies CM that the processing is complete
 
DEPENDENCIES  None 

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_clr_and_notify_sub_not_avail_status
(
  ds3geventmgr_sub_not_avail_bit_msk_type  client_id_mask,
  sys_modem_as_id_e_type                 subs_id,
  uint8                                  subs_id_mask
);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_IS_SUB_NOT_AVAIL_PROCESSING_IN_PROGRESS

DESCRIPTION   returns whether DS processing of SUBS_NOT_AVAIL is 
              still in progress (or) complete 
 
DEPENDENCIES  None 

RETURN VALUE  TRUE  --> SUB_NOT_AVAIL processing still in progress 
              FALSE --> SUB_NOT_AVAIL processing complete/No CARD REFRESH
                        processing in progress

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_is_sub_not_avail_processing_in_progress
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_SET_AND_NOTIFY_NV_REFRESH_STATUS

DESCRIPTION   Sets the Incoming bit mask, if found valid. If all required 
              bits are set, then clear all bits and notify MCFG that NV
              refresh is complete. 
 
DEPENDENCIES  None 

RETURN VALUE  TRUE  - If the status bit has been set successfully
              FALSE - If the status bit cannot be set

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_set_and_notify_nv_refresh_status
(
  ds3geventmgr_client_type status
);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_IS_NV_REFRESH_PROCESSING_IN_PROGRESS

DESCRIPTION   returns whether DS processing of NV refresh is 
              still in progress (or) complete 
 
DEPENDENCIES  None 

RETURN VALUE  TRUE  - NV refresh is still in progress
              FALSE - NV refresh is complete

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_is_nv_refresh_processing_in_progress(void);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_GET_CONF_INFO

DESCRIPTION   Called by clients to get cached information

DEPENDENCIES  None.

RETURN VALUE  TRUE: If operation was successful & payload was provided 
              FALSE: Otherwise 
 
CAVEAT        Information can be provided for 1 Subs/Tech Pair, if 
              mask has more than 1 bit set the request is rejected. 

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_get_conf_info
(
  ds3geventmgr_event_type                 event_id,
  void                                   *payload,
  ds3geventmgr_filter_type               *filter_info_p
);

/*===========================================================================
FUNCTION      DS3GEVENTMGR_SET_EVENT_INFO

DESCRIPTION   Called by clients to publish payload info to be given to 
              registered clients.

DEPENDENCIES  None.

RETURN VALUE  None. 
 
CAVEAT        Information can be provided for 1 Subs/Tech Pair, if 
              mask has more than 1 bit set the request is rejected. 

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_set_event_info
(
  ds3geventmgr_event_type                   event_id,
  void                                     *payload,
  ds3geventmgr_filter_type                 *filter_info_p
);
#endif /* DS3GEVENTMGR_H */
