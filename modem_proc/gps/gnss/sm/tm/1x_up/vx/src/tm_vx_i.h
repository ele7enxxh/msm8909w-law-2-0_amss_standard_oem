#ifndef TM_VX_I_H
#define TM_VX_I_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM Vx Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  Vx module.
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/src/tm_vx_i.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/16   kpn     Act on abort_reason specifically while aborting a V2 session
02/05/07   cl      Initial Release
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "tm_common.h"
#include "pdapi.h"
#include "tm_prtl_iface.h"
#include "tm_1x_up.h"
#include "pd_comms_api.h"
#include "tm_vx_msg.h"

/* Vx module should use timer ID range from 0x02010000 - 0x0201FFFF*/
typedef enum
{
  TM_VX_TIMER_ID_MIN                   = TM_1X_UP_VX_TIMER_ID_BASE,
  TM_VX_TIMER_ID_AUTH_DURATION,
  TM_VX_TIMER_ID_MS_MPC_CONN,  
  TM_VX_TIMER_ID_T7,
  TM_VX_TIMER_ID_T14,
  TM_VX_TIMER_ID_T15,  
  TM_VX_TIMER_ID_NI_TRACKING,
  TM_VX_TIMER_ID_MAX                   = TM_VX_TIMER_ID_MIN + TM_1X_UP_VX_TIMER_ID_RANGE
} tm_vx_timer_id_e_type;

/* MS-MPC Connection timer */
#define TM_VX_MS_MPC_CONN_TIMER_VALUE       30*1000

/* T7 timer (From sending of SPPReq to receipt of SPPResp) */
#define TM_VX_T7_TIMER_VALUE                10*1000

/* T14 timer (From sending of Pos Report to receipt of Pos Report Ack) */
#define TM_VX_T14_TIMER_VALUE               10*1000

/* Response timer for client to send back response */
#define TM_VX_T15_TIMER_CLNT_VALUE          15*1000

/* Internal timeer for client to send back response (5 seconds more) */
#define TM_VX_T15_TIMER_INT_VALUE          20*1000

/* Default Vx teleservice ID  */
#define TM_VX_DEFAULT_TELESERVICE_ID        65001

/* PD Comm Transaction ID definition */
#define TM_VX_TRANSACTION_ID_DISPATCH_MASK  0xFFFF8000

/* Vx TCP/IP uses transaction ID range from 0x01010000 - 0x01017FFF */
#define TM_VX_TCPIP_TRANSACTION_ID_BASE     TM_1X_UP_VX_TRANSACTION_ID_BASE
#define TM_VX_TCPIP_TRANSACTION_ID_RANGE    0x00008000

/* Vx SMS uses transaction ID range from 0x01018000 - 0x0101FFFF */
#define TM_VX_SMS_TRANSACTION_ID_BASE       (TM_VX_TCPIP_TRANSACTION_ID_BASE + TM_VX_TCPIP_TRANSACTION_ID_RANGE)
#define TM_VX_SMS_TRANSACTION_ID_RANGE      0x00008000

/* Mask for application trustness setting */
#define TM_VX_TRUSTNESS_MASK_APP_TYPE_WAP          0x01
#define TM_VX_TRUSTNESS_MASK_APP_TYPE_BREW         0x02
#define TM_VX_TRUSTNESS_MASK_APP_TYPE_JAVA         0x04
#define TM_VX_TRUSTNESS_MASK_APP_TYPE_MS_RESIDENT  0x08

/* Minimum time between fixes for a V2 tracking session */
#define TM_VX_TRACKING_MIN_TBF_MS                  30000

#define TM_VX_DEFAULT_QOS                          31

#define TM_VX_DEFAULT_ACCURACY_THRESHOLD           250


#define LITTLEEND_TO_BIGEND_WORD(w) ((((w)& 0xff00) >> 8) | (((w)& 0x00ff) << 8))

/* Enum for Vx version settings */
typedef enum {
  TM_VX_VERSION_DISABLED = 0,                       /* V1/V2 disabled */
  TM_VX_VERSION_V1_ONLY,
  TM_VX_VERSION_V2_ONLY,
  TM_VX_VERSION_MAX
} tm_vx_version_e_type;

/* Enum for GPS / Voice settings */
typedef enum {
  TM_VX_DISABLE_GPS_DURING_VOICE_CALL                 = 0,
  TM_VX_DISABLE_GPS_WHEN_VOICE_CALL_IN_SHARED_RF_ONLY,
  TM_VX_ALLOW_GPS_DURING_VOICE_CALL,
  TM_VX_GPS_DURING_VOICE_CALL_MAX
} tm_vx_gps_during_voice_call_e_type;

/* Enum for authentication check */
typedef enum {
  TM_VX_APP_AUTHED = 0,
  TM_VX_APP_AUTH_REQUIRED,
  TM_VX_APP_AUTH_ERROR,  
} tm_vx_app_auth_e_type;

/* Enum for general check for start_req */
typedef enum {
  TM_VX_GENERAL_CHECK_OK = 0,
  TM_VX_GENERAL_CHECK_USER_CONSENT_REQUIRED,
  TM_VX_GENERAL_CHECK_SERVICE_INTERACTION_ERROR,  
  TM_VX_GENERAL_CHECK_PARAMETER_ERROR,
} tm_vx_gen_check_e_type;

/* Enum for communication type */
typedef enum {
  TM_VX_COMM_TYPE_NONE = -1,
  TM_VX_COMM_TYPE_TCP,
  TM_VX_COMM_TYPE_SMS,  
  TM_VX_COMM_TYPE_MAX,  
} tm_vx_comm_e_type;

/* Data structure for server addr type and addr info */
typedef enum {
  TM_VX_MPC_ADR_TYPE_MIN = -1,
  TM_VX_MPC_ADR_TYPE_IPV4,
  TM_VX_MPC_ADR_TYPE_URL,
} tm_vx_mpc_adrs_e_type;

typedef struct {
  dword   mpc_adrs;
  dword   mpc_port_id;
} tm_vx_mpc_ipv4_adr_s_type;

typedef struct {
  uint16 url_length;
  char   url[255];
} tm_vx_mpc_url_adr_s_type;

typedef union {
 tm_vx_mpc_ipv4_adr_s_type   ipv4_adrs;   
/*
<<<[- by Liao] For now, there is no support for URL, enable this when it's needed
 tm_vx_mpc_url_adr_s_type  url_adr;   
>>>[- End] 
*/
} tm_vx_mpc_adr_u_type;

typedef struct {
  tm_vx_mpc_adrs_e_type   mpc_addr_type;
  tm_vx_mpc_adr_u_type    mpc_adrs;
} tm_vx_mpc_adr_s_type;


/* Data structure for static Vx information */
typedef struct
{
  tm_vx_version_e_type                vx_version;
  uint32                              vx_app_trusted_settings;
  word                                vx_ni_teleservice_id;
  boolean                             vx_prev6_only;
  dword                               vx_mo_max_duration;
  tm_vx_gps_during_voice_call_e_type  vx_gps_during_voice_call;
  dword                               vx_mpc_adrs;
  dword                               vx_mpc_port_id;  
} tm_vx_config_s_type;

/* Enum for Vx session state */
typedef enum {
  TM_VX_APP_STATE_NONE = -1,
  TM_VX_APP_STATE_INIT,
  TM_VX_APP_STATE_WAIT_USER_RESP,     
  TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ,  
  TM_VX_APP_STATE_WAIT_TO_RECV_SPPRESP,    
  TM_VX_APP_STATE_WAIT_PD_SESSION,
  TM_VX_APP_STATE_WAIT_TO_SEND_POS_RPT,
  TM_VX_APP_STATE_WAIT_TO_RECV_POS_RPT_ACK,
  TM_VX_APP_STATE_WAIT_TO_SEND_CANCEL,
} tm_vx_app_state_e_type;

typedef enum 
{
  TM_VX_COMM_RECV_STATE_HEADER = 0,
  TM_VX_COMM_RECV_STATE_PAYLOAD,
} tm_vx_comm_recv_state;

/* Structure for MPC communication related information */
typedef enum {
  TM_VX_COMM_STATE_INIT,
  TM_VX_COMM_STATE_CONNECTING,
  TM_VX_COMM_STATE_CONNECTED,
  TM_VX_COMM_STATE_DISCONNECTING,
  TM_VX_COMM_STATE_DISCONNECTED
} tm_vx_comm_state_e_type;

/* V1 MO SMS consent indicator */
typedef enum {
  TM_VX_MSG_V1_CONSENT_IND_CONSENT_GIVEN      = 0,
  TM_VX_MSG_V1_CONSENT_IND_CONSENT_NOT_GIVEN  = 1,
  TM_VX_MSG_V1_CONSENT_IND_NO_RESP            = 2,
  TM_VX_MSG_V1_CONSENT_IND_MS_REFUSED         = 3,
  TM_VX_MSG_V1_CONSENT_IND_CONSENT_NOT_NEEDED = 4,
  TM_VX_MSG_V1_CONSENT_IND_PROTOCOL_ERROR     = 5
} tm_vx_msg_v1_consent_indicator_e_type;


/* V2 MO SMS reject reason */
typedef enum {
  TM_VX_MSG_V2_REJ_REASON_VERSION_NOT_SUPPORTED  = 0,
  TM_VX_MSG_V2_REJ_REASON_PRIVACY                = 1,
  TM_VX_MSG_V2_REJ_REASON_PRIV_IS801_IN_PROGRESS = 2,  
  TM_VX_MSG_V2_REJ_REASON_MS_SOCKET_ERROR        = 3,  
  TM_VX_MSG_V2_REJ_REASON_USER_DENIED            = 4,    
  TM_VX_MSG_V2_REJ_REASON_POPUP_TIMEOUT          = 5, 
  TM_VX_MSG_V2_REJ_REASON_NO_REJECT              = 0xff
} tm_vx_msg_v2_rej_reason_e_type;

/* V2 MO SMS cancel reason */
typedef enum {
  TM_VX_MSG_V2_CANCEL_REASON_USER_INIT      = 0,
  TM_VX_MSG_V2_CANCEL_REASON_MPC_CONN_FAIL  = 1,
  TM_VX_MSG_V2_CANCEL_REASON_MPC_TIMEOUT    = 2,  
  TM_VX_MSG_V2_CANCEL_REASON_PDE_CONN_FAIL  = 3,  
  TM_VX_MSG_V2_CANCEL_REASON_PDE_REJ        = 4,    
  TM_VX_MSG_V2_CANCEL_REASON_IS801_TIMEOUT  = 5
} tm_vx_msg_v2_cancel_reason_e_type;

/* Structure for PD COMM transaction id info */
typedef struct {
  uint32  base;  
  uint32  range;    
  uint32  curr_id;    
} tm_vx_pdcomm_tr_id_s_type;

typedef struct {
  boolean                      handle_valid;
  tm_vx_mpc_adr_s_type         mpc_info;
  pd_comms_handle_type         pdcomm_handle;
  uint32                       pdcomm_msg_id_base; 
  tm_vx_pdcomm_tr_id_s_type    tr_id_info;
  pd_comms_link_type           link_type;
  tm_vx_comm_state_e_type      comm_state;
  tm_vx_comm_recv_state        recv_state;  
  uint16                       incoming_data_length;  
} tm_vx_mpc_comm_info_s_type;

/* Data structre for dynamic Vx information */
typedef struct
{
  tm_sess_handle_type          tm_core_sess_handle;
  int                          active_app_index;
  int                          pending_mo_app_index;  

  // 1. For MO sesssion, increment correlation_id for every SPPReq 
  // 2. For MT sesssion, set correlation_id to whatever is passed in MT SMS
  uint8                        correlation_id;    

  /* 
     Only one outstanding SPPResp is supported. Therefore, put it in sess_info_stype to save
     memory as compared to put it in app_info_s_type
  */
  tm_vx_int_v1_sppresp_s_type  v1_sppresp;
  tm_vx_int_v2_sppresp_s_type  v2_sppresp;  

  /* For V2 NI position report response */
  tm_vx_int_v2_pos_rpt_resp_s_type v2_pos_rpt_resp;

  /* Destination address for MO SMS */
  wms_address_s_type           mt_sms_address;

  /* 
     Only one outstanding V2 MT SMS is supported. Therefore, put it in sess_info_stype to save
     memory as compared to put it in app_info_s_type
  */
  tm_vx_int_v1_mt_sms_s_type   incoming_v1_mt_sms;
  tm_vx_int_v1_mt_sms_s_type   v1_mt_sms;
  
  /* 
     Only one outstanding V2 MT SMS is supported. Therefore, put it in sess_info_stype to save
     memory as compared to put it in app_info_s_type
  */
  tm_vx_int_v2_mt_sms_s_type   incoming_v2_mt_sms;
  tm_vx_int_v2_mt_sms_s_type   v2_mt_sms;

  /* Used to save PD info from 1x801 driver module for position report */
  pdsm_pd_info_s_type          lr_resp;
  qword                        lr_time;

  /* Timer for trying to bring up MS-MPC connection (30 seconds) */
  os_TimerMsgType              *mpc_connection_timer;
  

  /* Only one outstanding tracking session is allowed. Therefore put auth duration timer in 
     session_info_type
  */
  os_TimerMsgType              *auth_duration_timer;

  /* From sending of SPPReq to receipt of SPPResp (10 seconds) */
  os_TimerMsgType              *t7_timer;

  /* From sending of Pos Report to receipt of Pos Report Ack (10 seconds) */
  os_TimerMsgType              *t14_timer;

  /* User response timer (20 seconds) */
  os_TimerMsgType              *t15_timer;  
} tm_vx_sess_info_s_type;

/* Data structure for client application information: trustness, auth, ... etc */
/* Array app_info for each client type */
#define TM_VX_APP_NUM  PDSM_CLIENT_TYPE_MAX

typedef struct
{  
  /* Application Type: BREW, JAVA, BROWSER, ... etc */
  pdsm_client_type_e_type           app_type;

  /* Application state */
  tm_vx_app_state_e_type            app_state;
  /* 
    There might be multiple BREW applications.
    This flag is used to identify a particular BREW application
  */
  dword                             brew_class_id;
  
  /* This flag (staticly set by NV) decides if a particular client type needs to go thru authentication process */
  boolean                           trusted;

  /* This flag (runtime via SPPReq/SPPResp) tells if a paritcular client application has been authenticated */
  boolean                           authenticated;

  /* The duration (in seconds) a authentication period (tbf * (n_fix - 1) + 30) */
  uint32                            auth_duration;
  
  /* Following fields are needed for SPPReq / SPPResp */
  prtl_start_sess_req_param_s_type  pd_req_info;
  uint16                            app_info_length;  
  char                              app_info[TM_VX_MAX_APP_INFO_LEN];
} tm_vx_app_info_s_type;

/* 
  Data structure for general Vx protocol module inforamtion
  1. vx_config contains static information -> NV settings
  2. sess_info contains dynamic session information, session state, active client, ...etc
*/
typedef struct 
{
  /* vx_config contains static information (NV settings for example)*/
  tm_vx_config_s_type          vx_config;

  /* sess_info contains session related information (Dynamic) */
  tm_vx_sess_info_s_type       sess_info;

  /* Structure for applcition related information */
  tm_vx_app_info_s_type        app_info[TM_VX_APP_NUM];  
  
  /* Structure for MPC communication related information */
  tm_vx_mpc_comm_info_s_type   comm_info[TM_VX_COMM_TYPE_MAX];  
} tm_vx_info_s_type;

/*=============================================================================

                                Extern Data Structures

=============================================================================*/
extern tm_vx_info_s_type   tm_vx_gen_info;


/*=============================================================================

                                Extern Functions

=============================================================================*/


/*===========================================================================
FUNCTION tm_v1_handle_sppresp

DESCRIPTION
  This function is called to handle V1 SPPResp

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/	  
extern void tm_v1_handle_sppresp(void);

/*===========================================================================
FUNCTION tm_v1_check_incoming_mt_sms

DESCRIPTION
  This function is called to handle incoming V1 MT SMS

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/	  
extern boolean tm_v1_check_incoming_mt_sms(void);

/*===========================================================================
FUNCTION tm_v1_handle_incoming_mt_sms

DESCRIPTION
  This function is called to handle incoming V1 MT SMS

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/	  
extern void tm_v1_handle_incoming_mt_sms(void);

/*===========================================================================
FUNCTION tm_v1_send_mo_sms

DESCRIPTION
  This function sends out V1 MO SMS via pd comm

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None

===========================================================================*/
extern boolean tm_v1_send_mo_sms
(
  tm_vx_msg_v1_consent_indicator_e_type consent_ind,
  tm_vx_int_v1_mt_sms_s_type            *mt_sms_ptr
);

/*===========================================================================
FUNCTION tm_v1_handle_pd_info

DESCRIPTION
  This function performs V1 specific call flow when getting position.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_v1_handle_pd_info
(  
  pdsm_pd_info_s_type  *pd_info_ptr,
  qword                time,
  pdsm_pd_end_e_type   end_status
);

/*===========================================================================
FUNCTION tm_v1_handle_app_response

DESCRIPTION
  This function is called to hanlde app response in V1
  or not.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_v1_handle_app_response
(
  pdsm_lcs_agent_notification_resp_e_type  app_resp
);

/*===========================================================================
FUNCTION tm_v1_set_ni_param

DESCRIPTION
  This function is called to set parameters for V1 NI session

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_v1_set_ni_param(void);

/*===========================================================================

FUNCTION tm_v1_handle_ms_abort_session_req

DESCRIPTION
  This function is to perform necessary V1 call flows when MS aborts a session

  Example: User initiates abort
           E911
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_v1_handle_ms_abort_session_req(const tm_sess_stop_reason_e_type);

/*===========================================================================
FUNCTION tm_v1_handle_ms_mpc_failure

DESCRIPTION
  This function is called to handle failure for V1 SPPReq/SPPResp.
  
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_v1_handle_ms_mpc_failure(void);

/*===========================================================================
FUNCTION tm_v2_send_mo_cancel_sms

DESCRIPTION
  This function sends out V2 MO cancel SMS via pd comm

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None

===========================================================================*/
extern boolean tm_v2_send_mo_cancel_sms
(
  tm_vx_msg_v2_cancel_reason_e_type cancel_reason
);

/*===========================================================================
FUNCTION tm_v2_check_incoming_mt_sms

DESCRIPTION
  This function is called to handle incoming V2 MT SMS

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/	  
extern boolean tm_v2_check_incoming_mt_sms(void);

/*===========================================================================
FUNCTION tm_v2_handle_incoming_mt_sms

DESCRIPTION
  This function is called to handle incoming V2 MT SMS

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/	  
extern void tm_v2_handle_incoming_mt_sms(void);

/*===========================================================================
FUNCTION tm_v2_handle_sppresp

DESCRIPTION
  This function is called to handle V2 SPPResp

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/	  
extern void tm_v2_handle_sppresp(void);

/*===========================================================================
FUNCTION tm_v2_handle_pos_rpt_resp

DESCRIPTION
  This function is called to handle V2 position report response

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/	  
extern void tm_v2_handle_pos_rpt_resp(void);

/*===========================================================================
FUNCTION tm_v2_handle_t7_timer_expiry

DESCRIPTION
  This function is called to handle V2 T7 timeout
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_v2_handle_t7_timer_expiry(void);

/*===========================================================================
FUNCTION tm_v2_handle_t14_timer_expiry

DESCRIPTION
  This function is called to handle V2 T14 timeout
  
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_v2_handle_t14_timer_expiry(void);

/*===========================================================================
FUNCTION tm_v2_handle_ms_mpc_timer_expiry

DESCRIPTION
  This function is called to handle V2 MS-MPC Connection timeout
  
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_v2_handle_ms_mpc_timer_expiry(void);

/*===========================================================================
FUNCTION tm_v2_handle_pd_info

DESCRIPTION
  This function performs V2 specific call flow when getting pd info
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_v2_handle_pd_info
(  
  pdsm_pd_info_s_type  *pd_info_ptr,
  qword                time,
  pdsm_pd_end_e_type   end_status
);

/*===========================================================================

FUNCTION tm_v2_handle_ms_abort_session_req

DESCRIPTION
  This function is to perform necessary V2 call flows when MS aborts a session

  Example: User initiates abort
           E911
           
  For NI session, we need to send out MO SMS Cancel.
  For MO session, we need to send out TCP/IP Cancel.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_v2_handle_ms_abort_session_req(const tm_sess_stop_reason_e_type);


/*===========================================================================
FUNCTION tm_v2_handle_app_response

DESCRIPTION
  This function is called to hanlde app response in V2
  or not.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_v2_handle_app_response
(
  pdsm_lcs_agent_notification_resp_e_type  app_resp
);

/*===========================================================================
FUNCTION tm_v2_set_ni_param

DESCRIPTION
  This function is called to set parameters for V2 NI session

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
extern void tm_v2_set_ni_param(void);

/*===========================================================================
FUNCTION tm_v2_send_mo_reject_sms

DESCRIPTION
  This function sends out V2 MO Reject SMS via pd comm

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None

===========================================================================*/
extern boolean tm_v2_send_mo_reject_sms
(
  tm_vx_msg_v2_rej_reason_e_type reject_reason,
  uint8                          corr_id
);

/*===========================================================================
FUNCTION tm_vx_send_mo_sms

DESCRIPTION
  This function sends out MO SMS via pd comm

DEPENDENCIES 
  None.

RETURN VALUE
  
SIDE EFFECTS
 None

===========================================================================*/
extern boolean tm_vx_send_mo_sms
(
  uint8 *msg_data_ptr,
  uint8 msg_data_length
);

/*===========================================================================

FUNCTION tm_vx_ip_connect_to_mpc

DESCRIPTION
  This function attempts to bring up MS-MPC TCP/IP connection

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if attemp to bring up MS - MPC is allowed
  FALSE, otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_vx_ip_connect_to_mpc(void);

/*===========================================================================

FUNCTION tm_vx_ip_disconnect_from_mpc

DESCRIPTION
  This function attempts to tear down MS-MPC TCP/IP connection

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if attemp to tear down MS - MPC is allowed
  FALSE, otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_vx_ip_disconnect_from_mpc(boolean force_abort);

/*===========================================================================

FUNCTION tm_vx_update_app_state

DESCRIPTION
  This function update application state

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, state transition is valid
  FALSE, otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_vx_update_app_state
(
  int                     app_index,
  tm_vx_app_state_e_type  new_state
);


#ifdef __cplusplus
}
#endif

#endif /* TM_VX_I_H */
  
