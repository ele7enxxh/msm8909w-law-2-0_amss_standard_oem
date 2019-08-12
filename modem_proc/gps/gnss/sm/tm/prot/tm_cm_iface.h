#ifndef TM_CM_IFACE_H
#define TM_CM_IFACE_H


/*===========================================================================
  
                            TM_CM_IFACE  h e a d e r
                   
DESCRIPTION

  
Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 Qualcomm Technologies, Inc.
Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/prot/tm_cm_iface.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
03/11/16    mj      Changes to check as_sys_mode from cell info first for CELL DCH request to CM
10/19/15    mj      Add changes to provide srv_status of selected sys_mode through phone status
07/20/15    mj      Add support for MultiMode config for MultiSim devices
06/24/15    rh      Support Cell ID update from CM  
04/15/15    mj     Add a new bool to tm_cm_stack_ss_info_s_type to indicate a CM transistion from operational to non-operational
03/18/15    sj      Add sys mode to call events 
06/04/14    mj      Handle MultiSIM subscription feature SRLTE
01/24/14    mj      Remove featurization FEATURE_DUAL_SIM to fix SGLTE featured out issues
10/25/13    mj      Add definitions to support policy manager changes that retrieve device mode
09/27/13    mj      TSTS Support: Define variables for third SUB
01/04/12    ssu     Obtaining th CELL id information in CELL_DCH from CM APIs 
06/23/11    rh      Added cdma_mt_end_call_immed parameter for CR 280070
01/20/11    rh      Fix for CR 258513
03/03/10    ns      DSDS Support
04/02/09    ns      support for Qwip
04/24/07    ank     Added support for CELL_ID_DB feature.
11/30/06    ank     Initial Release
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"

#if !defined (WIN32) && !defined (FEATURE_CGPS_PLAYBACK)
#include "queue.h"
#include "task.h"
#endif


#include "aries_os_api.h"
#include "cm.h"
#include "cm_v.h"
#include "tm_prtl_iface.h"
#include "tm_common.h"

#include "cm_dualsim.h"
#include "sys.h"

#include "gps_common.h"

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               DEFINES                                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define TM_CM_HOLD_ORIG_TIMEOUT_SECS 10
#define TM_CM_L3_ACK_WAIT_TIME       0
#define TM_CM_AUTO_DISCONNECT_TIME   45000/* Time in ms */

#define TM_CM_IFACE_INACTIIVITY_TIMER_ID     (TM_CM_IFACE_TIMER_ID_RANGE+1)
#define TM_CM_IFACE_TIMER_DISPATCH_MASK      ((TM_CM_IFACE_INACTIIVITY_TIMER_ID)) 
#define TM_CM_CELL_DCH_CELL_ID 				 (0xFFFFFFFF)

#ifdef FEATURE_HDR
#define TM_CM_ISDATA_HDR(so) \
  ( CAI_SO_HDR_PKT_DATA == (so) ) 
#endif /*  FEATURE_HDR */

#define TM_CM_WCDMA_DCH_UPDATE_GAP_SECS  1  /* need to wait 1s between WCDMA DCH CID udpate requests */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              DATA TYPES                                 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef enum 
{
  TM_CM_REGISTER_NONE              =0x00, /* Deregister */
  TM_CM_REGISTER_SS_EVENT          =0x01, /* Register for SS Event for HPT */
  TM_CM_REGISTER_TC_DOWN_EVENT     =0x02, /* Register for traffic channel down event */
  TM_CM_REGISTER_TC_UP_EVENT       =0x04  /* Register for traffic channel up event */
} tm_cm_register_events_bmap_e_type;

/*---------------------------------
  Possible states that the phone call could be in.
 *-------------------------------*/

typedef enum
{
  TM_CM_PHONE_STATE_IDLE,
  TM_CM_PHONE_STATE_WAIT_CONNECT,
  TM_CM_PHONE_STATE_CONNECTED
} tm_cm_phone_state_e_type;

typedef enum
{
  TM_CM_E911_STATE_NONE,
  TM_CM_E911_STATE_ORIG,
  TM_CM_E911_STATE_CB
} tm_cm_e911_state_e_type;

typedef struct
{
  cgps_SrvSystemType srv_system;
  tm_cm_e911_state_e_type e911_state;
  sys_srv_status_e_type srv_status;
} tm_cm_phone_state_info_s_type;

typedef enum
{
  TM_CM_STOP_REASON_MOLR_COMPLETE,
  TM_CM_STOP_REASON_LOC_NOTIF_RET_RES
} tm_cm_stop_reason_e_type;


typedef enum
{
  TM_CM_LTE_EMERGENCY_START,
  TM_CM_LTE_EMERGENCY_STOP,
  TM_CM_LTE_EMERGENCY_ECBM
} tm_cm_lte_emergency_e_type;

typedef union
{
  #ifdef FEATURE_CGPS_UMTS_CP
  cm_lcs_molr_params_s_type lcs_molr_cmd_params;
  #endif 
  int for_compilation;
} tm_cm_start_info_u_type;

typedef union
{
  #ifdef FEATURE_CGPS_UMTS_CP
  cm_lcs_molr_completed_params_s_type              lcs_molr_completed_cmd_params;
  cm_lcs_location_notification_res_params_s_type   lcs_loc_notif_return_result_params;
  #endif 
  int for_compilation;
} tm_cm_stop_info_u_type;

typedef struct
{
  tm_cm_stop_reason_e_type   stop_reason;
  tm_cm_stop_info_u_type     stop_params;
} tm_cm_stop_info_s_type;



typedef enum
{
  TM_CM_CALL_STATE_IDLE,
  TM_CM_CALL_STATE_WAIT_CONNECT,
  TM_CM_CALL_STATE_CONNECTED
} tm_cm_call_state_e_type;

typedef struct
{
  cm_call_type_e_type           call_type;
  tm_cm_call_state_e_type       call_state;
  word                          srv_opt;
} tm_cm_call_info_s_type;

typedef struct
{
  tm_cm_call_info_s_type call[CM_CALL_ID_MAX];
} tm_cm_phone_info_s_type;

/* Data structures needed for Event CB */
typedef enum
{
  TM_CM_CALL_EVENT,
  TM_CM_PH_EVENT,
  TM_CM_DBM_EVENT,
  TM_CM_PD_EVENT,
  TM_CM_SS_EVENT,
  TM_CM_SUPS_EVENT,
  TM_CM_PH_SUB_PREF_EVENT
} tm_cm_event_cb_e_type;

typedef struct {
  cm_ph_state_e_type                     ph_state;
  sys_modem_dual_standby_pref_e_type     standby_pref;
  sys_modem_as_id_e_type                 active_subs;
  sys_modem_as_id_e_type                 default_voice_subs;
  sys_modem_as_id_e_type                 default_data_subs;
  sys_modem_as_id_e_type                 priority_subs;
  boolean            lpm_on;
} tm_cm_ph_info_s_type;

typedef union 
{
  cm_call_event_e_type   call_event;
  cm_ph_event_e_type     ph_event;
  cm_dbm_event_e_type    dbm_event;
#if !defined(__GRIFFON__) || defined(FEATURE_CMI)
  cm_pd_event_e_type     pd_event;
#endif
  cm_ss_event_e_type     ss_event;
#ifdef FEATURE_CGPS_UMTS_CP
  cm_sups_event_e_type   sups_event;
#endif /* FEATURE_CGPS_UMTS_CP */
  cm_ph_event_e_type     subs_event;
} tm_cm_event_cb_u_type;

typedef struct {
  cm_call_id_type         call_id;
  cm_call_type_e_type     call_type;
  uint16                  cdma_call_srv_opt;
  sys_sys_mode_e_type     sys_mode;
#ifdef FEATURE_LTE
  sys_voice_handover_e_type  voice_ho_type; /* used for identifying HO due to SRVCC */
#endif /* FEATURE_LTE */
} tm_cm_mm_call_info_s_type;

typedef struct {
  void                      *user_data;  // looks like no one is using pointer
                                         // coming from CM callback. Will remove
                                         // later.
  cm_dbm_mo_status_e_type   mo_status;
  tm_cm_rcv_data_s_type     data_payload;
} tm_cm_dbm_info_s_type;

typedef struct  {
  uint8                           invoke_id;
  uint8                           ss_ref;
  location_notification_s_type    location_notification;
  lcs_molr_res_s_type             lcs_molr_res;
} tm_cm_sups_info_s_type;

typedef struct {
  uint64                     changed_fields;
  boolean                    is_operational;
  sys_srv_status_e_type      srv_status;
  sys_srv_domain_e_type      srv_domain;
  sys_sys_mode_e_type        sys_mode;

  /* Mode info*/
  sys_base_id_type           mode_info_cdma_info_base_id;
  sys_base_lat_type          mode_info_cdma_info_base_lat;
  sys_base_long_type         mode_info_cdma_info_base_long;
  sys_srch_win_type          mode_info_cdma_info_srch_win_n;
  uint8                      mode_info_cdma_info_p_rev_in_use;
  sys_mm_information_s_type  mm_information;

  /*HDR info*/
  boolean                    hdr_hybrid;
  sys_srv_status_e_type      hdr_srv_status;
  sys_roam_status_e_type     hdr_roam_status;

  /*Roaming info*/
  sys_roam_status_e_type     roam_status;
  
  /*Sys id info*/
  sys_sys_id_s_type          sys_id;
  uint16                     sys_id_id_is95_sid;
  uint16                     sys_id_id_is95_nid;
  sys_plmn_id_s_type         sys_id_id_plmn;
    
  /*Cell info*/
  sys_cell_info_s_type       cell_info;
} tm_cm_mm_msim_ss_stack_info_s_type;

typedef struct {
  sys_modem_as_id_e_type              asubs_id ;
  uint8                               number_of_stacks ;
  tm_cm_mm_msim_ss_stack_info_s_type  multisim_stack_info[TM_CM_MAX_NO_STACKS];
  uint64                              changed_fields_subs;
} tm_cm_mm_ss_info_s_type;


typedef union
{
  tm_cm_mm_call_info_s_type call_info;
  tm_cm_ph_info_s_type      ph_info;
  tm_cm_dbm_info_s_type     dbm_info;
  tm_cm_mm_ss_info_s_type   ss_info;
#ifdef FEATURE_CGPS_UMTS_CP
  tm_cm_sups_info_s_type       sups_info;
#endif /* FEATURE_CGPS_UMTS_CP */
  cm_subs_info_s_type       ph_subs_info;
} tm_cm_event_cb_info_u_type;

typedef struct
{
  tm_cm_event_cb_u_type      cm_event_type;
  tm_cm_event_cb_info_u_type cm_event_info;
} tm_cm_event_cb_info_s_type;

typedef struct 
{
  tm_cm_event_cb_e_type            event_type;
  tm_cm_event_cb_info_s_type       event_info;
} tm_cm_event_info_s_type;

/* Data structures needed for Cmd CB */

typedef enum
{
  TM_CM_CALL_CMD,
  TM_CM_PH_CMD,
  TM_CM_DBM_CMD,
  TM_CM_PD_CMD,
  TM_CM_SS_CMD
} tm_cm_cmd_cb_e_type;

typedef union 
{
  cm_call_cmd_e_type   call_cmd;
  cm_ph_cmd_e_type     ph_cmd;
  cm_dbm_cmd_e_type    dbm_cmd;
  cm_pd_cmd_e_type     pd_cmd;
  cm_ss_cmd_e_type     ss_cmd;
} tm_cm_cmd_cb_u_type;

typedef union
{
  cm_call_cmd_err_e_type       call_cmd_err;
  cm_ph_cmd_err_e_type         ph_cmd_err;
  cm_dbm_cmd_err_e_type        dbm_cmd_err;
  cm_pd_cmd_err_e_type         pd_cmd_err;
  cm_ss_cmd_err_e_type         ss_cmd_err;
} tm_cm_cmd_cb_err_u_type;

typedef struct
{
  tm_cm_cmd_cb_u_type        cm_cmd_type;
  tm_cm_cmd_cb_err_u_type    cm_cmd_err;
  void                       *data_block_ptr;
} tm_cm_cmd_cb_info_s_type;

typedef struct
{
  tm_cm_cmd_cb_e_type            cmd_type;
  tm_cm_cmd_cb_info_s_type       cmd_info;
} tm_cm_cmd_info_s_type;

typedef enum
{
  TM_CM_CMD_WAKEUP_CDMA
} tm_cm_iface_cmd_e_type;

typedef union
{
  int for_compilation;
} tm_cm_iface_cmd_info_u_type;

typedef struct
{
  tm_cm_iface_cmd_e_type cmd_type;
  tm_cm_iface_cmd_info_u_type cmd_payload;
} tm_cm_iface_cmd_info_s_type;

/* Structure for holding internal information */

typedef struct
{
  uint8 payload[TM_MAX_BUFFSIZE];                   /* Send data  */
  uint16 length;                   /* The number of bytes in the payload */
} tm_cm_send_data_s_type;

typedef struct
{
  q_link_type link;
  void* user_data;
  tm_cm_send_data_s_type msg;
} tm_cm_send_msg_info_s_type;

typedef struct
{
  boolean                    is_operational;
  sys_sys_mode_e_type        sys_mode;//New Sys Mode for this AS ID
  sys_sys_mode_e_type        prev_sys_mode; //Prev Sys Mode for this AS ID
  sys_srv_status_e_type      srv_status; //New SRV Status for this AS ID
  sys_srv_domain_e_type      srv_domain; //CS/PS
  uint64                     changed_fields;
  sys_sys_mode_e_type        as_sys_mode; //sys_mode updated by lower layers
  boolean					 oper_to_non_oper_transition; /*Indicates if a stack went from operational to non-operational state. 
  														In this case as per call manager, the stack should be treated as OOS */
}tm_cm_stack_ss_info_s_type;


typedef struct
{
  sys_modem_dual_standby_pref_e_type   e_StandbyPref;  /* Standby preference of the Phone */

  /* The active subscription in Single Standby mode
     only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY */
  sys_modem_as_id_e_type   e_ActiveSubs;
  sys_modem_as_id_e_type   e_VoiceSubs;    /* Designated voice subscription */
  sys_modem_as_id_e_type   e_DataSubs;     /* Designated data subscription */
  sys_modem_as_id_e_type   e_PrioritySubs; /* Priority subscription */
  sys_modem_as_id_e_type   e_PrevDataSubs;  /* Stored Designated data subscription */
}tm_cm_dsds_ph_info_s_type;

typedef struct
{
  /*---------------------------------
    Call Manager related
   *------------------------------ */

  cm_client_id_type             cm_client;

  /*---------------------------------
    Timers
   *------------------------------ */

  os_TimerMsgType*              inactivity_timer;
  boolean                       inactivity_timer_active;

  /*---------------------------------
    Misc.
   *------------------------------ */

  boolean is_init;             /* Has tm_cm_init been called? */

  boolean msg_ack_pending;         /* We are awaiting the status of a msg */
  q_type  msg_waiting_q;       /* Queue for messages waiting to be sent */

  boolean                   cdma_mt_session_active;
  /* TRUE to end 1X CP PD call immediately if MT session is not allowed due to GPS Lock, etc
     Default behavior(FALSE) is to let network end call */
  boolean                   cdma_mt_end_call_immed;  
  tm_cm_e911_state_e_type   e911_state;

  tm_cm_phone_info_s_type   phone_info;
  tm_cm_phone_state_e_type  phone_state;
  sys_sys_mode_e_type       sys_mode;
  sys_sys_mode_e_type       prev_sys_mode;
  cm_call_id_type           pd_call_id;

  /*Flag to check for RAT changes when UE has Main and GWL stack active in SVLTE type targets.*/
  boolean					prev_gw_mode;
  boolean                   call_related;
  byte                      p_rev_in_use;

  tm_cm_register_events_bmap_e_type register_events_bmap;

  tm_cm_stack_ss_info_s_type  ss_info[TM_CM_MAX_NO_SUBS][TM_CM_MAX_NO_STACKS];

  tm_cm_stack_ss_info_s_type  external_stack_ss_info;
  
// Store DSDS params
  tm_cm_dsds_ph_info_s_type dsds_ph_info;
  sys_srv_status_e_type     hdr_srv_status;

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
  /* Cell ID information from CM which is populated into the RRC data . This is populated when the CM provides the cell id information.   */
  rrcmeas_plmn_cell_info_struct_type rrc_cell_info;
  rrc_cell_id_status_type rrc_cell_id_status;

  uint32   ts_wcdma_cid_update_req;  /* Timestamp(in seconds) for last WCDMA Cell ID update request in DCH mode by TM-CM */

#endif // FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
  sys_srv_status_e_type       prev_srv_status; /* Holds the previous serving status of the selected sys_mod */
  sys_srv_status_e_type       srv_status; /* Holds the current serving status of the selected sys_mod */

} tm_cm_info_s_type;

typedef enum
{
  TM_CORE_DEVICE_MODE_NA   			  = 0, /** Invalid. Could not retrieve device info from Policy manager */
  TM_CORE_DEVICE_MODE_SS   			  = 1, /** Single SIM mode */
  TM_CORE_DEVICE_MODE_DSDS 			  = 2, /** Dual SIM, Dual Standby. Only one SUB supports VOICE and DATA */
  TM_CORE_DEVICE_MODE_DSDA_VOICE      = 3, /** Dual SIM, Dual Active. Two SUBs can support VOICE simultaneously */
  TM_CORE_DEVICE_MODE_DSDA_VOICE_DATA = 4, /** Dual SIM, Dual Active. Two SUBs can support VOICE and DATA simultaneously */
  TM_CORE_DEVICE_MODE_TSTS 			  = 5, /** Triple SIM, Triple Standby. Only one SUB support VOICE and DATA */
  TM_CORE_DEVICE_MODE_NUM_MODES  		   /** Must always be after last valid mode */
}tm_core_device_mode_e_type;

tm_cm_stack_ss_info_s_type **tm_cm_multisim_ss_info;

//#ifdef FEATURE_GPSONE_NEW_L2_ACK
typedef boolean (*tm_cm_pd_resp_is_within_t2m_f_type)(
 byte*  start,
 /* msg buffer pointer */
 uint32 length
 /* msg length */
 );
//#endif /* FEATURE_GPSONE_NEW_L2_ACK */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              FUNCTION DECLARATIONS                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION tm_cm_iface_start_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_cm_iface_start_req(tm_prtl_type prtl_type, const tm_cm_start_info_u_type *info_ptr);

/*===========================================================================

FUNCTION tm_cm_iface_stop_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_cm_iface_stop_req(tm_prtl_type prtl_type, tm_cm_stop_info_s_type *info_ptr);



/*===========================================================================

FUNCTION tm_cm_iface_get_phone_state

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_cm_iface_get_phone_state(tm_cm_phone_state_info_s_type *phone_state_info_ptr);

/*===========================================================================

FUNCTION tm_cm_iface_send_data

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_cm_iface_send_data(tm_prtl_type prtl_type, byte* data_ptr, uint8 length);

/*===========================================================================

FUNCTION tm_cm_iface_submodule_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_cm_iface_submodule_init(tm_prtl_type prtl_type,const tm_cm_pd_resp_is_within_t2m_f_type pd_resp_cb_f);

/*===========================================================================

FUNCTION tm_cm_client_init

DESCRIPTION
  Initialializes the TM_CM, registers callbacks, enables the auto-answer
  feature. This is a synchronous function.

DEPENDENCIES
  None.

RETURN VALUE
  Any of the status_e enums.

SIDE EFFECTS
  None 

===========================================================================*/
extern boolean tm_cm_client_init(void);

/*===========================================================================

FUNCTION  tm_cm_iface_event_proc

DESCRIPTION 
  Handles an incoming CM event

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/
extern void tm_cm_iface_event_proc( void* p_data );

/*===========================================================================

FUNCTION  tm_cm_iface_cmd_proc

DESCRIPTION 
  Handles CM cmd cb.

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/
extern void tm_cm_iface_cmd_proc( void* p_data );

#ifdef FEATURE_CGPS_CDMA_IF
/*===========================================================================

FUNCTION  tm_cm_iface_handle_reg_events_req

DESCRIPTION 
  Handles a request for registration of specific events

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/

extern boolean tm_cm_iface_handle_reg_events_req(tm_cm_register_events_bmap_e_type bmap);


/*===========================================================================

FUNCTION  tm_cm_iface_send_cmd

DESCRIPTION 
  Handles a request for specific command

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/
extern boolean tm_cm_iface_send_cmd(const tm_cm_iface_cmd_info_s_type *cmd_info);

/*===========================================================================

FUNCTION tm_cm_iface_is_pd_call_connected(void)

DESCRIPTION

DEPENDENCIES
   None.

RETURN VALUE
  TRUE: If PD call connected
  FALSE: Otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_cm_iface_is_pd_call_connected(void);

/*===========================================================================

FUNCTION tm_cm_iface_hdr_call_connected

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_cm_iface_hdr_call_connected(void);
#endif /* FEATURE_CGPS_CDMA_IF */


/*===========================================================================

FUNCTION tm_cm_handle_timer_expiry

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_cm_handle_timer_expiry(const os_TimerExpiryType *timer_data);

/*===========================================================================

FUNCTION tm_cm_get_current_dd_sys_mode

DESCRIPTION 

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
extern sys_sys_mode_e_type tm_cm_get_current_dd_sys_mode(void);

/*===========================================================================

FUNCTION tm_cm_dsds_at_in_single_standby

DESCRIPTION
  This function returns TRUE if the AT is in single standby mode. 
  False otherwise 

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_cm_dsds_at_in_single_standby(void);

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
/*===========================================================================

FUNCTION tm_cm_request_cell_id_info

DESCRIPTION
  This function is used to request CM to obtain a valid cell
  ID CELL_DCH mode.

PARAMETERS 
  NONE.
  
DEPENDENCIES

RETURN VALUE
  booelan
 
SIDE EFFECTS

===========================================================================*/
extern boolean tm_cm_request_cell_info(void);

/*===========================================================================

FUNCTION tm_cm_request_wcdma_dch_cell_info

DESCRIPTION
  This function is used to request CM to obtain a valid cell
  ID in WCDMA DCH mode.

PARAMETERS 
  NONE.
  
DEPENDENCIES

RETURN VALUE
  booelan
 
SIDE EFFECTS

===========================================================================*/
extern boolean tm_cm_request_wcdma_dch_cell_info(void);
#endif // #ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH


/*===========================================================================

FUNCTION tm_cm_iface_is_serving_system_avail

DESCRIPTION 
  This funciton checks if the phone is camped on a particular RAT. Both main and
  GWL stack is checked for camped serving system.

DEPENDENCIES
  

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_cm_iface_is_serving_system_avail(cgps_SrvSystemType system_type);

/*===========================================================================

FUNCTION tm_cm_process_external_srv_changed

DESCRIPTION
  This function processes srv changes from external modem

DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_cm_process_external_srv_changed(const pdsm_cell_info_s_type *external_cell_db_info_ptr);


/*===========================================================================

FUNCTION tm_cm_process_ext_emergency_enter

DESCRIPTION
  This function processes emergency call indication from third party IMS
DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

extern void tm_cm_process_ext_emergency_enter( tm_cm_lte_emergency_e_type call_state);

/**===========================================================================
FUNCTION tm_cm_get_device_mode_info

DESCRIPTION: 
 Get device mode. Combination number of SIMs and active subscriptions 
 determine the device configuration mode.

DEPENDENCIES:
  None

RETURN VALUE: 
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_cm_update_device_mode_info(void);

/**===========================================================================
FUNCTION tm_cm_get_device_mode

DESCRIPTION: 
 Get device mode. Combination number of SIMs and active subscriptions 
 determine the device configuration mode.

DEPENDENCIES:
  None

RETURN VALUE: 
      TM_CORE_DEVICE_MODE_NA   		             = 0,  // Invalid. Could not retrieve device info from Policy manager 
      TM_CORE_DEVICE_MODE_SS   			      = 1,  // Single SIM mode 
      TM_CORE_DEVICE_MODE_DSDS 			      = 2,  // Dual SIM, Dual Standby. Only one SUB supports VOICE and DATA 
      TM_CORE_DEVICE_MODE_DSDA_VOICE            = 3, // Dual SIM, Dual Active. Both SUBs support VOICE simultaneously 
      TM_CORE_DEVICE_MODE_DSDA_VOICE_DATA  = 4,  // Dual SIM, Dual Active. Both SUBs support VOICE and DATA simultaneously 
      TM_CORE_DEVICE_MODE_TSTS 			       = 5,  // Triple SIM, Triple Standby. Only one SUB support VOICE and DATA 
 

SIDE EFFECTS
  None.

===========================================================================*/
tm_core_device_mode_e_type tm_cm_get_device_mode(void);


/**===========================================================================
FUNCTION tm_cm_get_subs_ue_mode

DESCRIPTION: 
 Get overall device mode. 

DEPENDENCIES:
  None

RETURN VALUE: 
  sys_ue_mode_e_type
 

SIDE EFFECTS
  None.

===========================================================================*/
sys_ue_mode_e_type tm_cm_get_subs_ue_mode(sys_modem_as_id_e_type dd_sub_id);

/**===========================================================================
FUNCTION tm_cm_get_subs_feature

DESCRIPTION: 
 Get overall device mode. 
	
DEPENDENCIES:
  None
	
RETURN VALUE: 
  sys_ue_mode_e_type	 
	
SIDE EFFECTS
  None.
	
===========================================================================*/
sys_subs_feature_t tm_cm_get_subs_feature(sys_modem_as_id_e_type dd_sub_id);


/**===========================================================================
	FUNCTION tm_cm_is_sglte_sub
	
	DESCRIPTION: 
	 Get overall device mode. 
		
	DEPENDENCIES:
	  None
		
	RETURN VALUE: 
	  Boolean
		 
		
	SIDE EFFECTS
	  None.
		
===========================================================================*/
boolean tm_cm_is_sglte_sub(sys_modem_as_id_e_type sub_id);

/**===========================================================================
	FUNCTION tm_cm_is_svlte_sub
	
	DESCRIPTION: 
       Get overall device mode. 
			
	DEPENDENCIES:
	  None
			
	RETURN VALUE: 
	  Boolean
			 
			
	SIDE EFFECTS
	  None.
			
===========================================================================*/
boolean tm_cm_is_svlte_sub(sys_modem_as_id_e_type sub_id);

/**===========================================================================
	FUNCTION tm_cm_is_srlte_sub
	
	DESCRIPTION: 
       Get overall device mode. 
			
	DEPENDENCIES:
	  None
			
	RETURN VALUE: 
	  Boolean
			 
			
	SIDE EFFECTS
	  None.
			
===========================================================================*/
boolean tm_cm_is_srlte_sub(sys_modem_as_id_e_type sub_id);

/**===========================================================================
	FUNCTION tm_cm_is_multimode_sub
	
	DESCRIPTION: 
       Check if multimode is enabled as a feature for the requested sub
			
	DEPENDENCIES:
	  None
			
	RETURN VALUE: 
	  Boolean: TRUE if MultiMode feature enabled
	  		  FALSE if MultiMode feature is disabled
			 
			
	SIDE EFFECTS
	  None.
			
===========================================================================*/
boolean tm_cm_is_multimode_sub(sys_modem_as_id_e_type sub_id);

/**===========================================================================
	FUNCTION tm_cm_get_sglte_sub
	
	DESCRIPTION: 
       Return the SVLTE sub 
			
	DEPENDENCIES:
	  None
			
	RETURN VALUE: 
	  sys_modem_as_id_e_type
			 
			
	SIDE EFFECTS
	  None.
			
===========================================================================*/

sys_modem_as_id_e_type tm_cm_get_sglte_sub(void);


/*===========================================================================

FUNCTION tm_core_get_current_dd_subs_id

DESCRIPTION
  This function returns the ID for current DD subscription

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
sys_modem_as_id_e_type tm_core_get_current_dd_subs_id( void);

/*===========================================================================
FUNCTION  tm_cm_handle_ph_sub_pref_event

DESCRIPTION
  A function to handle CM_PH_EVENT_SUBSCRIPTION_PREF_INFO

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURNS
  None

SIDE EFFECTS


===========================================================================*/
/*ARGSUSED*/
void tm_cm_handle_ph_sub_pref_event(
  cm_ph_event_e_type           subs_event,
  const cm_subs_info_s_type    *subs_info_ptr
);

/*===========================================================================

FUNCTION tm_cm_dsds_get_dd_subs

DESCRIPTION
  This function returns the subscription id of current DD subs.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type tm_cm_dsds_get_dd_subs(void);


/*===========================================================================
FUNCTION tm_cm_is_phone_in_e911_state

DESCRIPTION
  This function returns TRUE if phone is in E911 state (ORIG or CB)

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_is_phone_in_e911_state(void);

#endif /* TM_CM_IFACE_H */


