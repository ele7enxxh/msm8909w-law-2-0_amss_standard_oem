#ifndef DS3GDSDIF_H
#define DS3GDSDIF_H
/*===========================================================================

                D A T A   S E R V I C E S   D S D  I N T E R F A C E
 
                H E A D E R   F I L E

DESCRIPTION
  Interface between DSD and Data Services header file with exposed api's

  Copyright (c) 2001-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gdsdif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/23/13   rs      Added support for additional WLAN information  
05/24/13   sd      TSTS changes for DS3G.
10/17/12   dvk     Merged fix to tear down 3gpp2 call when moving from 1x/HDR 
                   to 3gpp system if handoff is not allowed. 
02/27/12   var     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "sys.h"
#include "ds_dsd_ext_i.h"
#include "dstask_v.h"
#include "ds3gcmif.h"
#include "ds3gstkproc.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ps_sys_event.h"
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------
  Struct to cache DSD pref sys information
-------------------------------------------------------------------------*/
typedef struct
{
  ds3gsubsmgr_subs_id_e_type  subs_id;
  uint8                       pref_index;
  uint32                      changed_radio_mask;
  ds_dsd_ext_radio_info_type  radio_info[DS3G_RADIO_INDEX_MAX];
}ds3gdsdif_radio_info_type;

/*-------------------------------------------------------------------------
  Number of concurrent voice calls and emergency calls supported
-------------------------------------------------------------------------*/
#define DS3GDSDIF_VOICE_CALL_COUNT 16
#define DS3GDSDIF_EMERG_CALL_COUNT 1

/*---------------------------------------------------------------------------
  Macro to be called only in case of DSDS mode
---------------------------------------------------------------------------*/
#define DS3GDSDIF_GET_OTHER_SUBS_INDX( subs_indx )       \
          ((subs_indx == DS3GSUBSMGR_SUBS_ID_1)?         \
            DS3GSUBSMGR_SUBS_ID_2:DS3GSUBSMGR_SUBS_ID_1)

/*-------------------------------------------------------------------------
  Enum for voice call statemachine states
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GDSDIF_VOICE_CALL_ORIG,             /* Voice call originated */
  DS3GDSDIF_VOICE_CALL_CONNECTED,        /* Voice call ended */
  DS3GDSDIF_VOICE_CALL_INACTIVE_PENDING, /* Voice call inactive,pending notif*/
  DS3GDSDIF_VOICE_CALL_INACTIVE,         /* Voice call inactive*/
  DS3GDSDIF_VOICE_CALL_MT_CSFB_PAGED     /* MT CSFB paged by network */
}ds3gdsdif_voice_call_sm_state_e_type;

/*-------------------------------------------------------------------------
  Struct to cache call information - CS data/ Voice call only
-------------------------------------------------------------------------*/
typedef struct
{
  sys_sys_mode_e_type                    radio;
  ds3gdsdif_voice_call_sm_state_e_type   state;
  uint8                                  call_id;
  sys_voice_handover_e_type              voice_ho_type;
}ds3gdsdif_voice_call_info_type;

/*-------------------------------------------------------------------------
  struct for voice call state - This state gives a high level info whether
  voice call is active or not
-------------------------------------------------------------------------*/
typedef struct
{
  boolean                          is_call_active;
  ds3gdsdif_voice_call_info_type  *info_p;
}ds3gdsdif_voice_call_state_type;

/*-------------------------------------------------------------------------
  Struct to post DS command for call info - CS data/ Voice call only
-------------------------------------------------------------------------*/
typedef struct
{
  byte                       call_id;
  cm_call_event_e_type       call_event;
  cm_call_type_e_type        call_type;
  sys_sys_mode_e_type        call_sys_mode;
  sys_voice_handover_e_type  call_voice_ho_type;
  sys_modem_as_id_e_type     subs_id;
}ds3gdsdif_call_info_cmd_type;

/*-------------------------------------------------------------------
    DSDIF EXTERNAL EVENTS ENUM TYPE.
    CLIENTS can register to get these events
-------------------------------------------------------------------*/
typedef enum
{
  DS3GDSDIF_EXT_INVALID_EV          = 0,
  DS3GDSDIF_EXT_LTE_ATTACH_INIT_STATUS_EV,
  DS3GDSDIF_EXT_MAX_EV              = DS3GDSDIF_EXT_LTE_ATTACH_INIT_STATUS_EV  
}ds3gdsdif_ext_event_type;

/*-----------------------------------------------------------------
    DSDIF Clients Enum type
-----------------------------------------------------------------*/
typedef enum
{
  DS3GDSDIF_EXT_CLIENT_INVALID     = 0,
  DS3GDSDIF_EXT_CLIENT_DATA_TEST,
  DS3GDSDIF_EXT_CLIENT_MPPM,
  DS3GDSDIF_EXT_CLIENT_MAX         = DS3GDSDIF_EXT_CLIENT_MPPM
}ds3gdsdif_ext_client_type;

/*------------------------------------------------------------------- 
 Structure that contains attach info to be handed over to MPPM when
 attach is triggered
--------------------------------------------------------------------*/
typedef struct
{
  ds_sys_subscription_enum_type    ds_subs_id;
  boolean                          in_progress;
  uint16                           profile_id;
}ds3gdsdif_attach_info_type;

/*-------------------------------------------------------------------
   Clients Registers a callback function with DS3GDSDIF module
--------------------------------------------------------------------*/
typedef void (*ds3gdsdif_ext_notify_func_type)(
               ds3gdsdif_ext_event_type        event_id,
               void                          * event_info_ptr,
               void                          * data_ptr);

/*-------------------------------------------------------------------
 Event data block for DSDIF ext event queue
--------------------------------------------------------------------*/
typedef struct
{
  q_link_type                       link;
  ds3gdsdif_ext_client_type         client_id;
  ds3gdsdif_ext_event_type          event_id;
  ds3gdsdif_ext_notify_func_type    func_ptr;
  void                             *data_ptr;
}ds3gdsdif_ext_event_data_block;

typedef struct
{
  ds3gdsdif_ext_client_type         dsdif_ext_client;
  ds3gdsdif_ext_event_type          dsdif_ext_event;
}ds3gdsdif_ext_client_reg_info;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GDSDIF_INIT

DESCRIPTION   This function regsiters DS as a client with DSD and registers 
              for preferred system change event.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_init(void);

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_PREFERRED_SYS

DESCRIPTION   This function returns the current preferred data system.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  Returns current preferred data system.

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds3gdsdif_get_preferred_sys
(
  ds3gsubsmgr_subs_id_e_type   subs_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_PREF_RADIO_INFO

DESCRIPTION   This function returns the current preferred radio information. 
              The memory for pref_radio_info has to be allocated by the caller.
  
PARAMETERS    pref_radio_info:  returns preferred radio information

DEPENDENCIES  Memory for the out parameters needs to be provided by the caller.

RETURN VALUE  TRUE:  pref_radio_info has valid value
              FALSE: pref_radio_info has invalid value

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_get_pref_radio_info
(
  ds3gsubsmgr_subs_id_e_type   subs_id,
  ds_dsd_ext_radio_info_type  *pref_radio_info
);

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_RADIO_INFO

DESCRIPTION   This function returns the radio information for the queried 
              system_mode. The memory for the radio_info has to be allocated
              by the caller.
  
PARAMETERS    system_mode:    mode for which the radio info is being queried 
              radio_info:     returns radio information

DEPENDENCIES  Memory for the out parameters needs to be provided by the caller.

RETURN VALUE  TRUE:  radio_info has valid value
              FALSE: radio_info has invalid value

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_get_radio_info
(
  ds3gsubsmgr_subs_id_e_type   subs_id,
  sys_sys_mode_e_type          system_mode,
  ds_dsd_ext_radio_info_type  *radio_info
);

/*===========================================================================
FUNCTION      DS3GDSDIF_IS_BSR_IN_PROGRESS

DESCRIPTION   This function returns if BSR is in progress or not.
  
PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  True:  BSR in progress
              False: BSR not in progress

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_is_bsr_in_progress
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_RESEL_TARGET_RAT

DESCRIPTION   return the reselection target rat variable cached as part of 
              CM SS event processing. 

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds3gdsdif_get_resel_target_rat
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_IS_LTE_ATTACH_IN_PROGRESS

DESCRIPTION   This function returns if LTE attach is in progress or not.
  
PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  True:  LTE attach is in progress
              False: LTE attach is not in progress

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_is_lte_attach_in_progress
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_SET_LTE_ATTACH_IN_PROGRESS

DESCRIPTION   This function returns if LTE attach is in progress or not.
  
PARAMETERS    subs_id: Current active subscription
              lte_attach_in_progress: Flag telling whether LTE attach in prog
              attach_prof_info: Emergency calls supported or not & APN info

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_set_lte_attach_in_progress
(
  sys_modem_as_id_e_type  subs_id,
  boolean                 attach_in_progress,
  ds3geventmgr_attach_prof_info_type *attach_prof_info
);

/*===========================================================================
FUNCTION      DS3GDSDIF_SET_LTE_EMERGENCY_CALL_IN_PROG

DESCRIPTION   This function notifies DSD if LTE emergency call is in progress.

DEPENDENCIES  None
 
PARAMETERS    subs_id: subscription id 
              in_prog: TRUE - emergency call starts
                       FALSE - emergency call finishes
RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gdsdif_set_lte_emergency_call_in_prog
(
  sys_modem_as_id_e_type               subs_id,
  boolean                              in_prog
);

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_VOICE_CALL_STATE

DESCRIPTION   Returns the voice call state for the given subs_id

PARAMETERS    ds3g_subs_id: subscription id.

DEPENDENCIES  None.

RETURN VALUE  ds3gdsdif_voice_call_state_type

SIDE EFFECTS  None.
===========================================================================*/
ds3gdsdif_voice_call_state_type ds3gdsdif_get_voice_call_state
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_EMERG_CALL_STATE

DESCRIPTION   returns the emergency call state for a given subs_id

PARAMETERS    subs_id: subscription id.

DEPENDENCIES  None.

RETURN VALUE  ds3gdsdif_voice_call_state_type

SIDE EFFECTS  None.
===========================================================================*/
ds3gdsdif_voice_call_state_type ds3gdsdif_get_emerg_call_state
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_EHRPD_IS_PREF_SYSTEM

DESCRIPTION   Checks whether EHRPD is the preferred system.

PARAMETERS    sub_id: cm data sub id.

DEPENDENCIES  None.

RETURN VALUE  TRUE: if EHRPD is the preferred system 
              FALSE: otherwise

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_ehprd_is_pref_system
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_EVENT

DESCRIPTION   This function notifies DSD about radio acquired or lost event.
  
PARAMETERS    event_id: DSD event id 
              radio: system mode associated with event
              rat_mask: rat mask
              so_mask: so mask
              srv_status: service status
              srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_acq_lost_event
(
  ds_dsd_client_radio_event_type  event_id,
  sys_modem_as_id_e_type          as_id,
  sys_sys_mode_e_type             radio,
  uint32                          rat_mask,
  uint32                          so_mask,
  sys_srv_status_e_type           srv_status,
  sys_srv_domain_e_type           srv_domain
);

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_LOST_EVENT

DESCRIPTION   This function fills the dsd event info and notifies DSD.
  
PARAMETERS    sys_mode: system mode associated with event

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_lost_event
(
  sys_sys_mode_e_type     sys_mode,
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_HDR_SESSION_EVENT

DESCRIPTION   This function notifies DSD about following HDR attach events
               - Session Open
               - Session Close

PARAMETERS    dsd_event: dsd event 
              rat_mask: rat mask
              so_mask: so mask

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_hdr_session_event
(
  sys_modem_as_id_e_type          as_id,
  ds_dsd_client_radio_event_type  dsd_event,
  uint32                          rat_mask,
  uint32                          so_mask
);

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_LTE_ATTACH_EVENT

DESCRIPTION   This function notifies DSD about following LTE attach events
               - Attach Init
               - Attach Complete
               - Detached
               - EPS Context Tx
  
PARAMETERS    dsd_event: dsd event 
              rat_mask: rat mask
              so_mask: so mask

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_lte_attach_event
(
  sys_modem_as_id_e_type          as_id,
  ds_dsd_client_radio_event_type  dsd_event,
  ds_dsd_attach_complete_type     attach_type,
  uint32                          rat_mask
);

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_GWT_PARAM_CHG_EVENT

DESCRIPTION   This function notifies DSD about G/W/TD param change event.
  
PARAMETERS    sys_mode: system mode associated with event

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_gwt_param_chg_event
(
  sys_sys_mode_e_type      sys_mode,
  sys_modem_as_id_e_type   as_id,
  cm_cell_srv_ind_s_type  *cell_srv_ind_ptr
);

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_LTE_DIV_DUPLEX_CHG_EVENT

DESCRIPTION   This function notifies DSD about LTE div duplex change event.
  
PARAMETERS    as_id: subs_id on which to notify
              div_duplex: LTE div duplex

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_lte_div_duplex_chg_event
(
  sys_modem_as_id_e_type  as_id,
  sys_div_duplex_e_type   div_duplex
);

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_LTE_CA_CHG_EVENT

DESCRIPTION   This function notifies DSD about LTE CA so chg events
  
PARAMETERS    as_id: Subs_id
              ca_dl_available: ca dl is available
              ca_ul_available: ca ul is available

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_lte_ca_chg_event
(
  sys_modem_as_id_e_type  as_id,
  boolean                 ca_dl_available,
  boolean                 ca_ul_available
);

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_ROAM_STATUS_CHG_EVENT  

DESCRIPTION   This function notifies DSD about roaming status change.
  
PARAMETERS    sys_mode:       system mode
              as_id:          Subs_id
              roaming_status: roaming status

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_roam_status_chg_event
(
  sys_sys_mode_e_type     sys_mode,
  sys_modem_as_id_e_type  as_id,
  boolean                 roaming_status
);

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_PS_SUSPEND

DESCRIPTION   This function process the PS suspend

PARAMETERS    subs_id: ds3g data sub id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_ps_suspend
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_DSD_PREF_SYS_CHG_CMD

DESCRIPTION   This function processes the DS_DSD_PREF_SYS_CHG_CMD.  This 
              command is posted to DS3G when preferred system changes.
              This function will call into the MH's sys change handlers
              and then call into MH's pref sys change handlers.

PARAMETERS    cmd_ptr: command pointer 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_dsd_pref_sys_chg_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_DSD_SS_EVENT_PROC_DONE_CMD

DESCRIPTION   This function processes the DS_DSD_SS_EVENT_PROC_DONE_CMD. This 
              command is posted to DS3GDSDIF when DSD calls SS event processes
              complete.

PARAMETERS    cmd_ptr: command pointer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_dsd_ss_event_proc_done_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_VOICE_CALL_EVENT_CMD

DESCRIPTION   This function process the CM voice call event command

PARAMETERS    cmd_ptr: command pointer.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void  ds3gdsdif_process_voice_call_event_cmd
(
  ds_cmd_type  *cmd_ptr
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS3GDSDIF_WLAN_SVC_CHG_CB

DESCRIPTION   This callback is called to notify WLAN status change.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_wlan_srv_chg_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  void                                          *event_info_ptr,
  void                                          *user_data_ptr
);

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_WLAN_SVC_CHG_CMD

DESCRIPTION   This function processes the DS3GDSDIF_WLAN_SYS_CHG_CMD.

PARAMETERS    cmd_ptr: command pointer.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_wlan_svc_chg_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);

/*===========================================================================
FUNCTION     DS3GDSDIF_CPY_PS_SYS_WLAN_ADDR

DESCRIPTION  This utility function is used to cpy ps_sys wlan info

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_cpy_ps_sys_wlan_addr
(
  wlan_addr_type   *wlan_addr_dest,
  ps_ip_addr_type   ipv4_addr1_src,
  ps_ip_addr_type   ipv4_addr2_src,
  ps_ip_addr_type   ipv6_addr1_src,
  ps_ip_addr_type   ipv6_addr2_src
);

#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_EMERGERGENCY_CB_MODE

DESCRIPTION   This function is used emergency callback mode

PARAMETERS    emergency_cb_mode: emergency callback mode flag

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gdsdif_notify_emergergency_cb_mode
(
  sys_modem_as_id_e_type  asubs_id,
  boolean                 emergency_cb_mode
);

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_SRV_STATUS_FROM_DS3G_RADIO_CACHE

DESCRIPTION   Utility function to get the srv status from ds3g stk table.

PARAMETERS    pref_radio: preferred radio from DSD

RETURN VALUE  The srv status of the pref radio from the ds3g radio cache

SIDE EFFECTS  None.
===========================================================================*/
sys_srv_status_e_type dsd3gif_get_srv_status_from_ds3g_radio_cache
(
  sys_modem_as_id_e_type       asubs_id,
  ds_dsd_ext_radio_info_type  *radio_ptr
);

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_MT_CSFB_HYST_TIMER_EXP_CMD

DESCRIPTION   This function processes DS_CMD_DS3GDSDIF_MT_CSFB_HYST_TIMER_EXP 
              command.

PARAMETERS    cmd_ptr

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_mt_csfb_hyst_timer_exp_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION       ds3gdsdif_is_voice_call_active_with_call_id

DESCRIPTION    This function checks if voice call instance for a given
               subs_id and call_id is present or not

DEPENDENCIES   None.

RETURN VALUE   TRUE: voice call instance found
                        FALSE: voice call instance not found

SIDE EFFECTS   None.
===========================================================================*/
boolean ds3gdsdif_is_voice_call_active_with_call_id
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint8   call_id
);


#endif /* DS3G_DSD_IF_H */
