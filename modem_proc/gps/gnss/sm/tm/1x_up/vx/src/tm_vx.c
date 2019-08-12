/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM - Vx module

General Description
  This file contains implementations for TM Vx Module
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/src/tm_vx.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/16   kpn     Act on abort_reason specifically while aborting a V2 session
12/30/14   gk      drop events if is801 is not in session
05/13/14   mj      Add featurization to fix SMART compile/link issues
04/17/14   skm    NV Async Read changes
01/27/14   mj     Modified tm_vx_sms_parse_msg_cb with new params
12/06/13   ah      Support for non-deferrable timers
09/20/11   gk      Fallback to standalone only for non MSA
06/02/11   rh      Added runtime Vx configuration
06/03/10   bp      adding smart MSB support for V1/V2
05/14/10   atien   stop MS-MPC timer if connection to MPC is disconnected during
                   either SPPREQ and SPPRESP processing
07/13/09   gk      process end session command by ignoring it
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/04/08    cl     Abort Vx when entering UMTS system
12/07/06    cl     Read Vx config from NV
                   Added NI trigger handling
10/31/06    cl     Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "sm_nv.h"
#include "pdapibuf.h"
#include "tm_vx.h"
#include "tm_vx_i.h"
#include "tm_vx_ni_client.h"
#include "tm_1x_up_is801_drv_iface.h"
#include "log.h"
#include "log_dmss.h"
#include "tm_nv.h"

/* TM-Core / Vx protocl interface callback table */
tm_prtl_cb_s_type  tm_vx_prtl_iface_table;


/*---------------------------------------------------------------------------

   This section contain code which is only compiled when 
   FEATURE_GPSONE_VX_LCS_AGENT is defined.
   
----------------------------------------------------------------------------*/
#ifdef FEATURE_GPSONE_VX_LCS_AGENT

/* NV buffer, used to read NV items */
static nv_item_type      tm_vx_nv_read_buffer;

/* Global varialble which contains all necessary informaton for Vx module */
tm_vx_info_s_type   tm_vx_gen_info;

void tm_vx_init(void);
void tm_vx_ms_mpc_timer_handler(void);
void tm_vx_t7_timer_handler(void);

/* Log define for Vx forward link message */
LOG_RECORD_DEFINE(LOG_MS_MPC_FWD_LINK_C)
  byte          msg[TM_VX_MAX_MSG_SIZE];  /* Vx message */
LOG_RECORD_END

/* Log define for Vx reverse link message */
LOG_RECORD_DEFINE(LOG_MS_MPC_REV_LINK_C)
  byte          msg[TM_VX_MAX_MSG_SIZE];  /* Vx message */
LOG_RECORD_END
  
typedef LOG_MS_MPC_FWD_LINK_C_type  log_gps_ms_mpc_fwd_msg_type;
typedef LOG_MS_MPC_REV_LINK_C_type  log_gps_ms_mpc_rev_msg_type;

uint8  tm_vx_fwd_log_buf_idx = 0;
byte   tm_vx_fwd_log_buf[TM_VX_MAX_MSG_SIZE];

/*===========================================================================
FUNCTION tm_vx_rev_logging

DESCRIPTION
  This function logs MS-MPC reverse link TCP/IP messages.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_vx_rev_logging
(
  byte    *msg_ptr,
  int     msg_length
)
{
  log_gps_ms_mpc_rev_msg_type*         msmpc_rev_log_ptr;

  if((msg_ptr == NULL) || (msg_length <= 0))
  {
    MSG_ERROR("Invalid msg, can not proceed with logging", 0, 0, 0);
    return;
  }

  if(log_status( LOG_MS_MPC_REV_LINK_C))
  {
    if ((msmpc_rev_log_ptr = (log_gps_ms_mpc_rev_msg_type *)log_alloc( LOG_MS_MPC_REV_LINK_C, (msg_length + sizeof(log_hdr_type) + sizeof(byte)))) != NULL)  
    {
      memscpy((void*)msmpc_rev_log_ptr->msg, sizeof(msmpc_rev_log_ptr->msg), (const void*)(msg_ptr), msg_length);
      log_commit((log_type*) msmpc_rev_log_ptr);
    }
  }
}

/*===========================================================================
FUNCTION tm_vx_fwd_logging

DESCRIPTION
  This function logs MS-MPC forward link TCP/IP messages.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_vx_fwd_logging
(
  tm_vx_comm_recv_state recv_data_type,
  char                  *msg_ptr,
  uint8                  msg_length
)
{
  log_gps_ms_mpc_fwd_msg_type*         msmpc_fwd_log_ptr;
  
  if( (msg_ptr == NULL)
    ||(msg_length > TM_VX_MAX_MSG_SIZE))
  {
    MSG_ERROR("Invalid input parameters", 0, 0, 0);  
    return;
  }

  /* Cache header information in tm_vx_fwd_log_buf */ 
  if(recv_data_type == TM_VX_COMM_RECV_STATE_HEADER)
  {
    tm_vx_fwd_log_buf_idx = 0;
    
    tm_vx_fwd_log_buf_idx = memscpy((void *)&(tm_vx_fwd_log_buf[0]),
		   sizeof(tm_vx_fwd_log_buf),
           (void *)msg_ptr, msg_length);
  }
  else if(recv_data_type == TM_VX_COMM_RECV_STATE_PAYLOAD)
  {
    /* Make sure payload + header size is not greater than msg buffer size */
    if(tm_vx_fwd_log_buf_idx  <= (TM_VX_MAX_MSG_SIZE - msg_length))
    {
      /* Append payload after previously cached header */    
      tm_vx_fwd_log_buf_idx +=  memscpy((void *)&(tm_vx_fwd_log_buf[tm_vx_fwd_log_buf_idx]),
             sizeof(tm_vx_fwd_log_buf) - tm_vx_fwd_log_buf_idx,
             (void *)msg_ptr,
             msg_length);
      
      if ( log_status( LOG_MS_MPC_FWD_LINK_C ))
      {
        if ((msmpc_fwd_log_ptr = (log_gps_ms_mpc_fwd_msg_type *)log_alloc( LOG_MS_MPC_FWD_LINK_C,  tm_vx_fwd_log_buf_idx + sizeof(log_hdr_type) + sizeof(byte) )) != NULL)
        {
          memscpy((void*)msmpc_fwd_log_ptr->msg, TM_VX_MAX_MSG_SIZE, (const void*)&(tm_vx_fwd_log_buf[0]), tm_vx_fwd_log_buf_idx);
          log_commit((log_type *) msmpc_fwd_log_ptr);
        }
      }      
    }
    else
    {
      MSG_ERROR("log buffer size too small", 0, 0, 0);  
    }

    /* Reset buf index to 0 */
    tm_vx_fwd_log_buf_idx = 0;    
  }
  return;
}

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
boolean tm_vx_update_app_state
(
  int                     app_index,
  tm_vx_app_state_e_type  new_state
)
{
  if((app_index > (int)PDSM_CLIENT_TYPE_NONE) && (app_index < (int)PDSM_CLIENT_TYPE_MAX))
  {
    //<<<[+ by Liao] Add validity check later
    tm_vx_gen_info.app_info[app_index].app_state = new_state;
    //>>>[+ End]  
    return TRUE; /*lint !e506 */
  }
  else
  {
    MSG_ERROR("Invalid app_index", 0, 0, 0);
    return FALSE;
  }  
}

/*===========================================================================

FUNCTION tm_vx_update_comm_state

DESCRIPTION
  This function performs necessary check for comm state update

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, state transition is valid
  FALSE, otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_update_comm_state
(
  tm_vx_comm_e_type        link_type,
  tm_vx_comm_state_e_type  new_state
)
{
  if((link_type > TM_VX_COMM_TYPE_NONE) && (link_type < TM_VX_COMM_TYPE_MAX))  
  {    
    //<<<[+ by Liao] Add validity check later
    tm_vx_gen_info.comm_info[link_type].comm_state = new_state;
    //>>>[+ End]

    return TRUE; /*lint !e506 */
  }
  else
  {
    MSG_ERROR("Invalid Link Type", 0, 0, 0);
    return FALSE;
  }
}

/*===========================================================================

FUNCTION tm_vx_app_auth_required_check

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TM_VX_APP_AUTHED:         Request is trusted or already authenticated
  TM_VX_APP_AUTH_REQUIRED:  Need to get authentication from MPC
  TM_VX_APP_AUTH_ERROR:     Error condition

SIDE EFFECTS
  None.

===========================================================================*/
tm_vx_app_auth_e_type tm_vx_app_auth_required_check
(
  const prtl_start_sess_req_param_s_type  *start_sess_req_param
)
{
  tm_vx_app_auth_e_type auth_result;
  int app_info_arr_idx;

  if(start_sess_req_param == NULL)
  {
    return TM_VX_APP_AUTH_ERROR;
  }

  if( (start_sess_req_param->client_type >= PDSM_CLIENT_TYPE_BROWSER) 
    &&(start_sess_req_param->client_type <= PDSM_CLIENT_TYPE_OEM))
  {  
    app_info_arr_idx = (int)start_sess_req_param->client_type;

    /* If an application is trusted, bypass all Vx checks */
    if(tm_vx_gen_info.app_info[app_info_arr_idx].trusted == TRUE) /*lint !e506 !e731 */
    {
      auth_result = TM_VX_APP_AUTHED;
    }
    else
    {     
      /* When a request enters this function, it means it has already passed 
         1. parameter check
         2. phone state check
         3. service interaction check
         (Performed by tm_vx_start_sess_req_general_check)

         Therefore, we only have to check if it has been authed by MPC
      */     
  
      /* An incoming request is non-trusted, perform Vx checks */
 
      /* Check if the app has been authenticated */
      if(tm_vx_gen_info.app_info[app_info_arr_idx].authenticated == TRUE) /*lint !e506 !e731 */
      {
        auth_result = TM_VX_APP_AUTHED;
      }
      else
      {
        tm_vx_gen_info.sess_info.active_app_index = app_info_arr_idx;
        tm_vx_gen_info.app_info[app_info_arr_idx].brew_class_id = start_sess_req_param->pd_option.class_id;
        auth_result = TM_VX_APP_AUTH_REQUIRED;
      }
    }
  }
  else
  {
    auth_result = TM_VX_APP_AUTH_ERROR;
  }

  return auth_result;
}

/*===========================================================================
FUNCTION tm_vx_app_trustness_setting

DESCRIPTION
  This function reads Application specific settings from NV. An application can
  be Trusted or Non-Trusted from network to network. PDSM should handle reading
  NV values and this function simply return Trye/False for a 

RETURN VALUE
  Trusted = True
  Non-Trusted = False

DEPENDENCIES
  
===========================================================================*/
boolean tm_vx_app_trustness_setting(pdsm_client_type_e_type app_type)
{
  if(app_type == PDSM_CLIENT_TYPE_NI)
  {
    MSG_MED("NI is non-Trusted..",0,0,0);
    return FALSE;
  }

  if(app_type == PDSM_CLIENT_TYPE_BROWSER)
  {
    if (tm_vx_gen_info.vx_config.vx_app_trusted_settings & TM_VX_TRUSTNESS_MASK_APP_TYPE_WAP)
    {
      MSG_MED("WAP is Trusted..",0,0,0);
      return TRUE; /*lint !e506 */
    }
  }
  else if(app_type == PDSM_CLIENT_TYPE_JAVA)
  {
    if (tm_vx_gen_info.vx_config.vx_app_trusted_settings & TM_VX_TRUSTNESS_MASK_APP_TYPE_JAVA)
    {
      MSG_MED("JAVA is Trusted..",0,0,0);
      return TRUE; /*lint !e506 */
    }
  }
  else if(app_type == PDSM_CLIENT_TYPE_BREW)
  {
    if (tm_vx_gen_info.vx_config.vx_app_trusted_settings & TM_VX_TRUSTNESS_MASK_APP_TYPE_BREW)
    {
      MSG_MED("BREW is Trusted..",0,0,0);
      return TRUE; /*lint !e506 */
    }
  }
  else /* MS Resident */
  {
    if(tm_vx_gen_info.vx_config.vx_app_trusted_settings & TM_VX_TRUSTNESS_MASK_APP_TYPE_MS_RESIDENT)
    {
      MSG_MED("MS resident is Trusted..",0,0,0);
      return TRUE; /*lint !e506 */
    }
  }
  
  return FALSE;
}

/*===========================================================================
FUNCTION: tm_vx_read_nv

DESCRIPTION:

PARAMETERS:

===========================================================================*/
void tm_vx_read_nv(void)
{
  /* Read Vx version first */
  (void) sm_nv_read(NV_GPS1_VX_LCS_AGENT_I, (void *)&tm_vx_nv_read_buffer);
  tm_vx_gen_info.vx_config.vx_version = (tm_vx_version_e_type)tm_vx_nv_read_buffer.gps1_vx_lcs_agent;

  /* Read Client Trusted Settings */
  (void) sm_nv_read(NV_GPS1_VX_APP_TRUSTED_SETTINGS_I, (void *)&tm_vx_nv_read_buffer);
  tm_vx_gen_info.vx_config.vx_app_trusted_settings = (uint32)tm_vx_nv_read_buffer.gps1_vx_app_trusted_settings;

  /* Read Vx teleservice ID */
  (void) sm_nv_read(NV_GPS1_VX_NI_TELESERVICE_ID_I, (void *)&tm_vx_nv_read_buffer);
  tm_vx_gen_info.vx_config.vx_ni_teleservice_id = (uint32)tm_vx_nv_read_buffer.gps1_vx_ni_teleservice_id;

  /* Read Prev6 only setting */
  (void) sm_nv_read(NV_GPS1_VX_LCS_AGENT_PREV6_ONLY_I, (void *)&tm_vx_nv_read_buffer);
  tm_vx_gen_info.vx_config.vx_prev6_only = (uint32)tm_vx_nv_read_buffer.gps1_vx_lcs_agent_prev6_only;

  /* Read Max MO duration */
  (void) sm_nv_read(NV_GPS1_VX_MO_MAX_DURATION_I, (void *)&tm_vx_nv_read_buffer);
  tm_vx_gen_info.vx_config.vx_mo_max_duration = (uint32)tm_vx_nv_read_buffer.gps1_vx_mo_max_duration;  

  /* Read GPS during Voice setting */
  (void) sm_nv_read(NV_GPS1_VX_GPS_DURING_VOICE_CALL_I, (void *)&tm_vx_nv_read_buffer);
  tm_vx_gen_info.vx_config.vx_gps_during_voice_call = (tm_vx_gps_during_voice_call_e_type)tm_vx_nv_read_buffer.gps1_vx_gps_during_voice_call;    

  /* Read MPC IP and Port */
  (void) sm_nv_read( NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I, (void *)&tm_vx_nv_read_buffer );
   tm_vx_gen_info.vx_config.vx_mpc_adrs = tm_vx_nv_read_buffer.cgps_1x_mpc_server_addr_ipv4;

  (void)sm_nv_read( NV_CGPS_1X_MPC_SERVER_PORT_I, (void *)&tm_vx_nv_read_buffer );
  tm_vx_gen_info.vx_config.vx_mpc_port_id = tm_vx_nv_read_buffer.cgps_1x_mpc_server_port;
}

#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================
FUNCTION: tm_vx_ruim_read

DESCRIPTION:

PARAMETERS:

===========================================================================*/
void tm_vx_ruim_read(void)
{
  /* Read MPC IP and Port */
  (void)tm_core_nv_async_read( NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I, (void *)&tm_vx_nv_read_buffer );

  (void)tm_core_nv_async_read( NV_CGPS_1X_MPC_SERVER_PORT_I, (void *)&tm_vx_nv_read_buffer );
}

/*===========================================================================
FUNCTION: tm_vx_ruim_read_complete

DESCRIPTION:

PARAMETERS:

===========================================================================*/
void tm_vx_ruim_read_complete(const prtl_event_u_type  *event_payload)
{
   if (event_payload->ruim_read_nv_payload.mpc_adrs != NULL)
   {
        tm_vx_gen_info.vx_config.vx_mpc_adrs = event_payload->ruim_read_nv_payload.mpc_adrs;
   }
   if (event_payload->ruim_read_nv_payload.mpc_port !=NULL) 
   {
        tm_vx_gen_info.vx_config.vx_mpc_port_id = event_payload->ruim_read_nv_payload.mpc_port;
   }
   MSG_MED("RUIM ASYNC READ MPC IP: %x PORT :%x",tm_vx_gen_info.vx_config.vx_mpc_adrs, tm_vx_gen_info.vx_config.vx_mpc_port_id, 0);
}
#endif /* FEATURE_UIM_SUPPORT_LBS */

/*===========================================================================
FUNCTION: tm_vx_sess_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_vx_sess_init(void)
{
  tm_vx_gen_info.sess_info.active_app_index     = PDSM_CLIENT_TYPE_NONE;
  tm_vx_gen_info.sess_info.pending_mo_app_index = PDSM_CLIENT_TYPE_NONE;
  tm_vx_gen_info.sess_info.correlation_id       = 0;

  /* Set V2 SPPResp to 0 */
  memset((void *)&(tm_vx_gen_info.sess_info.v2_sppresp),
          0,
          sizeof(tm_vx_int_v2_sppresp_s_type));

  /* Set V2 Pos Report Resp to 0 */
  memset((void *)&(tm_vx_gen_info.sess_info.v2_pos_rpt_resp),
          0,
          sizeof(tm_vx_int_v2_pos_rpt_resp_s_type));

  /* Set MT SMS Address to 0 */
  memset((void *)&(tm_vx_gen_info.sess_info.mt_sms_address),
          0,
          sizeof(wms_address_s_type));

  /* Set Incoming V2 MT SMS to 0 */
  memset((void *)&(tm_vx_gen_info.sess_info.incoming_v2_mt_sms),
          0,
          sizeof(tm_vx_int_v2_mt_sms_s_type));

  /* Set  V2 MT SMS to 0 */
  memset((void *)&(tm_vx_gen_info.sess_info.v2_mt_sms),
          0,
          sizeof(tm_vx_int_v2_mt_sms_s_type));

  /* Set  PD LR Response to 0 */
  memset((void *)&(tm_vx_gen_info.sess_info.lr_resp),
          0,
          sizeof(pdsm_pd_info_s_type));

  /* Create MS-MPC Connection Timer */
  tm_vx_gen_info.sess_info.mpc_connection_timer = os_TimerCreateInCgpsTimerGroup((uint32)TM_VX_TIMER_ID_MS_MPC_CONN, (uint32)THREAD_ID_SM_TM);

  /* Create Timer for auth duration */
  tm_vx_gen_info.sess_info.auth_duration_timer  = os_TimerCreateInCgpsTimerGroup((uint32)TM_VX_TIMER_ID_AUTH_DURATION, (uint32)THREAD_ID_SM_TM);

  /* Create T7 timer (10 seconds from Sending SPPReq to the receipt of SPPResp) */
  tm_vx_gen_info.sess_info.t7_timer             = os_TimerCreateInCgpsTimerGroup((uint32)TM_VX_TIMER_ID_T7, (uint32)THREAD_ID_SM_TM);

  /* Create T14 timer (10 seconds from sending of Pos Report to receipt of Pos Report Ack) */
  tm_vx_gen_info.sess_info.t14_timer            = os_TimerCreateInCgpsTimerGroup((uint32)TM_VX_TIMER_ID_T14, (uint32)THREAD_ID_SM_TM);

  /* Create T15 timer (20 seconds for user response) */
  tm_vx_gen_info.sess_info.t15_timer            = os_TimerCreateInCgpsTimerGroup((uint32)TM_VX_TIMER_ID_T15, (uint32)THREAD_ID_SM_TM);
}
  
/*===========================================================================
FUNCTION: tm_vx_app_info_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_vx_app_info_init(void)
{
  int i;

  for(i = 0; i < (int)TM_VX_APP_NUM; i++)
  {
    tm_vx_gen_info.app_info[i].app_type            = (pdsm_client_type_e_type)i;
    tm_vx_gen_info.app_info[i].trusted             = tm_vx_app_trustness_setting((pdsm_client_type_e_type)i);
    tm_vx_gen_info.app_info[i].authenticated       = FALSE;
    tm_vx_gen_info.app_info[i].app_info_length     = 0;
    tm_vx_gen_info.app_info[i].brew_class_id       = 0;    
    (void)tm_vx_update_app_state(i, TM_VX_APP_STATE_INIT);
  }
}

/*===========================================================================
FUNCTION: tm_vx_mpc_comm_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_vx_mpc_comm_init(void)
{
  /* Init TCP/IP */
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].handle_valid         = FALSE;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].link_type            = LINK_TCP;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_msg_id_base   = TM_CORE_PDCOMM_MSG_ID_BASE;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].tr_id_info.base      = TM_VX_TCPIP_TRANSACTION_ID_BASE;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].tr_id_info.range     = TM_VX_TCPIP_TRANSACTION_ID_RANGE;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].tr_id_info.curr_id   = TM_VX_TCPIP_TRANSACTION_ID_BASE; 
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].recv_state           = TM_VX_COMM_RECV_STATE_HEADER;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length = 0;
  (void)tm_vx_update_comm_state(TM_VX_COMM_TYPE_TCP, TM_VX_COMM_STATE_INIT);
  
  /* Init SMS */
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].handle_valid         = FALSE;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].link_type            = LINK_SMS;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_msg_id_base   = TM_CORE_PDCOMM_MSG_ID_BASE;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].tr_id_info.base      = TM_VX_SMS_TRANSACTION_ID_BASE;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].tr_id_info.range     = TM_VX_SMS_TRANSACTION_ID_RANGE;
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].tr_id_info.curr_id   = TM_VX_SMS_TRANSACTION_ID_BASE;  
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].recv_state           = TM_VX_COMM_RECV_STATE_HEADER;  
  tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].incoming_data_length = 0;  
  (void)tm_vx_update_comm_state(TM_VX_COMM_TYPE_SMS, TM_VX_COMM_STATE_INIT);  
}

/*===========================================================================
FUNCTION: tm_vx_init

DESCRIPTION:
  This function is called at boot up time to initialize TM Vx module.

 
PARAMETERS:

===========================================================================*/
void tm_vx_init(void)
{
  /* Read NV items */
  tm_vx_read_nv();

  /* Initialize General Vx session info */
  tm_vx_sess_init();
  
  /* Initialize app info array, note: it has to be called after tm_vx_read_nv to setup trustnees info correctly */
  tm_vx_app_info_init();

  /* Setup PD Comm */
  tm_vx_mpc_comm_init();

  /* Init vx ni client module */
  tm_vx_ni_client_init();    
}

/*===========================================================================

FUNCTION tm_vx_get_next_transaction_id

DESCRIPTION
  This function returns next transaction for a particular session type.
  
DEPENDENCIES
  None.

RETURN VALUE
  transaction id

SIDE EFFECTS
  None.

===========================================================================*/
uint32 tm_vx_get_next_transaction_id
(
  tm_vx_comm_e_type link_type
)
{
  if((link_type > TM_VX_COMM_TYPE_NONE) && (link_type < TM_VX_COMM_TYPE_MAX))  
  {    
    if( (tm_vx_gen_info.comm_info[link_type].tr_id_info.curr_id < tm_vx_gen_info.comm_info[link_type].tr_id_info.base)
      ||(tm_vx_gen_info.comm_info[link_type].tr_id_info.curr_id >= (tm_vx_gen_info.comm_info[link_type].tr_id_info.base + tm_vx_gen_info.comm_info[link_type].tr_id_info.range)))
    {
      tm_vx_gen_info.comm_info[link_type].tr_id_info.curr_id = tm_vx_gen_info.comm_info[link_type].tr_id_info.base;
    }

    return tm_vx_gen_info.comm_info[link_type].tr_id_info.curr_id++;
  }
  else
  {
    MSG_ERROR("Invalid Link Type", 0, 0, 0);
    return 0;
  }
}

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
boolean tm_vx_ip_disconnect_from_mpc(boolean force_abort)
{
  pd_comms_return_type ret_val;

  ret_val = pd_comms_app_disconnect(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_handle,
                                    (uint32)THREAD_ID_SM_TM,
                                    tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_msg_id_base,
                                    tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_TCP),
                                    force_abort);
  
  if(ret_val == PDCOMM_RESULT_OK)
    {
      MSG_MED("TM Vx MPC Disconnected", 0, 0, 0);
      (void)tm_vx_update_comm_state(TM_VX_COMM_TYPE_TCP, TM_VX_COMM_STATE_DISCONNECTED);
      return TRUE; /*lint !e506 */
    }
    else if(ret_val == PDCOMM_RESULT_WAIT)
    {
      MSG_MED("TM Vx MPC Disconnecting", 0, 0, 0);
      (void)tm_vx_update_comm_state(TM_VX_COMM_TYPE_TCP, TM_VX_COMM_STATE_DISCONNECTING);
      return TRUE; /*lint !e506 */
    }
    else
    {
      MSG_ERROR("pd_comms_app_disconnect returned error: %d",ret_val,0,0);
      return FALSE;
    }
}

/*===========================================================================
FUNCTION tm_vx_reinit

DESCRIPTION
  This function is called to reset Vx module to the initial state.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_reinit
(
  boolean                     stop_tm_core, 
  tm_sess_stop_reason_e_type  tm_core_stop_reason
) 
{
  tm_sess_req_param_u_type  sess_req_param; 
  tm_sess_handle_type       tm_sess_handle;

  MSG_MED("Re-initialize TM VX", 0, 0, 0);  
  
  if(tm_vx_gen_info.sess_info.active_app_index != PDSM_CLIENT_TYPE_NONE)
  {
    /* Stop NI client First */
    tm_vx_ni_client_stop_session();
  
    /* Reset app info structure */
    tm_vx_app_info_init();

    /* Reset session info structure */
    tm_sess_handle = tm_vx_gen_info.sess_info.tm_core_sess_handle;
    tm_vx_gen_info.sess_info.tm_core_sess_handle = TM_SESS_HANDLE_NONE;

    /* Reset pending mo request */
    tm_vx_gen_info.sess_info.pending_mo_app_index = PDSM_CLIENT_TYPE_NONE;

    /* Reset active app index */       
    tm_vx_gen_info.sess_info.active_app_index = PDSM_CLIENT_TYPE_NONE;

    /* Stop Timers */
    (void) os_TimerStop(tm_vx_gen_info.sess_info.auth_duration_timer);
    (void) os_TimerStop(tm_vx_gen_info.sess_info.t7_timer);
    (void) os_TimerStop(tm_vx_gen_info.sess_info.t14_timer);
    (void) os_TimerStop(tm_vx_gen_info.sess_info.t15_timer);  
    (void) os_TimerStop(tm_vx_gen_info.sess_info.mpc_connection_timer);
 
    if(stop_tm_core == TRUE)
    {
      /* Report to TM-Core to fail the incoming MO request */
      sess_req_param.stop_param.stop_type   = TM_STOP_TYPE_SESSION;
      sess_req_param.stop_param.stop_reason = tm_core_stop_reason;
   
      (void)tm_sess_req(TM_PRTL_TYPE_1X_UP,
                        tm_sess_handle,
                        TM_SESS_REQ_STOP,
                        &sess_req_param);
    }
  }

  /* If there TCP/IP is open, close it */
  if( (tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].comm_state == TM_VX_COMM_STATE_CONNECTING)
    ||(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].comm_state == TM_VX_COMM_STATE_CONNECTED))
  {
    MSG_HIGH("Abort Vx IP connection", 0, 0, 0);
    (void) tm_vx_ip_disconnect_from_mpc(TRUE);
  }

  /* Reset MPC Throttle check flag */
  tm_1x_up_is801_drv_set_mpc_throttle_check(FALSE);
}

/*===========================================================================

FUNCTION tm_vx_start_sess_req_general_check

DESCRIPTION
  Check if incoming request can be accecpted by V1/V2 protocol
  Check includes
  1. Phone state check
  2. Parameter check
  3. Service interaction check (MO over MO, MO over NI ..etc)

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  Ready for performing V1/V2 call flow.
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
tm_vx_gen_check_e_type tm_vx_start_sess_req_general_check
(
  const prtl_start_sess_req_param_s_type  *start_sess_req_param
)
{
  tm_vx_gen_check_e_type  ret_val = TM_VX_GENERAL_CHECK_OK;
  int                     active_app_idx;
    
  if(start_sess_req_param == NULL)
  {
    MSG_ERROR("Null start_sess_req_param", 0, 0, 0);
    return TM_VX_GENERAL_CHECK_PARAMETER_ERROR;
  }

  if( (start_sess_req_param->client_type <= (int)PDSM_CLIENT_TYPE_NONE)
    ||(start_sess_req_param->client_type >= (int)PDSM_CLIENT_TYPE_MAX))
  {
    MSG_ERROR("Client type out of range", 0, 0, 0);
    return TM_VX_GENERAL_CHECK_PARAMETER_ERROR;  
  }
  
  /* If incoming request is from trusted client, bypass the checks */
  if(tm_vx_gen_info.app_info[start_sess_req_param->client_type].trusted == FALSE)
  {
    active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  
    if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
    {
      /* There is an active Vx session going on
         if the current session is NI, set MO to pending and let user decide to go with MO or continue with NI    
         if the current session is MO, check if the incoming req belongs to the same client by comparing to client type/brew cls id/app info.
      */
      if(active_app_idx == PDSM_CLIENT_TYPE_NI)
      {
        /* Sub-sequent NI fix requests */
        if(start_sess_req_param->client_type == PDSM_CLIENT_TYPE_NI)
        {
          ret_val = TM_VX_GENERAL_CHECK_OK;
        }
        else
        {
          MSG_HIGH("MO over NI, need user consent", 0, 0, 0);
          ret_val = TM_VX_GENERAL_CHECK_USER_CONSENT_REQUIRED;
        }
      }
      else
      {
        /* Incoming MO request is from a different client type */
        if(start_sess_req_param->client_type != active_app_idx)
        {
          MSG_HIGH("MO over MO is not allowed", 0, 0, 0);
          ret_val = TM_VX_GENERAL_CHECK_PARAMETER_ERROR;
        }
        else
        {
          /* Incoming MO request is from the same client type, now need to check if they
             have the same brew class id / app info
          */
          if(active_app_idx == PDSM_CLIENT_TYPE_BREW)
          {
            if(tm_vx_gen_info.app_info[active_app_idx].brew_class_id == start_sess_req_param->pd_option.class_id)
            {
              /* No active Vx session */
              ret_val = TM_VX_GENERAL_CHECK_OK;
            }
            else
            {
              MSG_ERROR("MO over MO is not allowed", 0, 0, 0);
              ret_val = TM_VX_GENERAL_CHECK_PARAMETER_ERROR;
            }
          }
          else
          {
            ret_val = TM_VX_GENERAL_CHECK_OK;            
          }
        }
      }
    }
    else
    {
      /* No active Vx session */
      MSG_MED("No active vx session", 0, 0, 0);
      ret_val = TM_VX_GENERAL_CHECK_OK;
    }      
  }
  else
  {
    MSG_MED("Bypass check for trusted cleint type %d", start_sess_req_param->client_type, 0, 0);
    ret_val = TM_VX_GENERAL_CHECK_OK;
  }

  return ret_val;
}


/*===========================================================================

FUNCTION tm_vx_sms_read_msg_cb

DESCRIPTION
  This function parses incoming SMS message and decide if the message belongs 
  to Vx module.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if the SMS belongs to Vx module
  FALSE, otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_sms_read_msg_cb
(
  const wms_client_message_s_type  *mt_msg_ptr,
  wms_client_ts_data_s_type   *decoded_mt_msg_ptr,
  uint8                       *p_MesgContent,
  uint16                       *p_MesgLen,
  uint16                       w_MesgContentMaxLen,
  boolean                     u_isWapPush
)
{
  boolean ret_val;

  (void)u_isWapPush;
  (void)w_MesgContentMaxLen;  

  if((mt_msg_ptr == NULL) || (decoded_mt_msg_ptr == NULL) || (p_MesgContent == NULL) || (p_MesgLen == NULL))
  {
    MSG_HIGH("Recv'd NULL pointer", 0, 0, 0);  
    ret_val = FALSE;
  }
  else
  {
    if ((mt_msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) &&
		(mt_msg_ptr->u.cdma_message.teleservice == (wms_teleservice_e_type)tm_vx_gen_info.vx_config.vx_ni_teleservice_id )) 
    {  
      *p_MesgLen = decoded_mt_msg_ptr->u.cdma.user_data.data_len;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].incoming_data_length = *p_MesgLen;

      *p_MesgLen = memscpy((void *)p_MesgContent, WMS_CDMA_USER_DATA_MAX-1, (void *)&(decoded_mt_msg_ptr->u.cdma.user_data.data[0]), *p_MesgLen);

      /* Save MT address for sending MO SMS */
      memscpy((void *)&(tm_vx_gen_info.sess_info.mt_sms_address),
             sizeof(tm_vx_gen_info.sess_info.mt_sms_address),
             (void*)&(mt_msg_ptr->u.cdma_message.address),
             sizeof(mt_msg_ptr->u.cdma_message.address));
      
      ret_val = TRUE; /*lint !e506 */
    }
    else
    {
      ret_val = FALSE;
    }
  }
  return ret_val;
} /*lint !e818 */

/*===========================================================================

FUNCTION tm_vx_sms_connect

DESCRIPTION
  This function estabilshes a SMS link

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if SMS link is established successfully
  FALSE, otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_sms_connect(void)
{
  pd_comms_return_type            ret_val;
  pd_comms_get_handle_params_type get_handle_param;
  pd_comms_connect_params_type    connect_param;  

  /* Make sure we have proper pdcomm hanlde first */
  if(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].handle_valid == FALSE)
  {
    get_handle_param.e_link_type   = tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].link_type;
    get_handle_param.q_base_msg_id = tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_msg_id_base;

    ret_val = pd_comms_app_get_handle(
                                       &get_handle_param, 
                                       &(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_handle)
                                     );

    if(ret_val != PDCOMM_RESULT_OK)
    {
      MSG_ERROR("pd_comms_app_get_handle returned error: %d",ret_val,0,0);
      return FALSE;
    }
    else
    {
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].handle_valid = TRUE; /*lint !e506 */
    }
  }

  if( (tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].comm_state != TM_VX_COMM_STATE_CONNECTING)
    &&(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].comm_state != TM_VX_COMM_STATE_CONNECTED))
  {
    /* When we're here, we must have a valid pd_comm handle, now try connect */
    connect_param.e_link_type                                   = LINK_SMS;
    connect_param.e_use_security                                = SECURITY_NONE;
    connect_param.q_transaction_id                              = tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_SMS);
    connect_param.z_link_params.z_sms_params.p_parse_cb_func    = tm_vx_sms_read_msg_cb;
    connect_param.z_link_params.z_sms_params.p_read_msg_cb_func = NULL;
    connect_param.z_link_params.z_sms_params.u_connectDcOnLargeSendFailure = TRUE; /*lint !e506 */
  
    ret_val = pd_comms_app_connect(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_handle,
                                   (uint32)THREAD_ID_SM_TM,
                                   tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_msg_id_base,
                                   &connect_param);

    if(ret_val != PDCOMM_RESULT_WAIT)
    {
      MSG_ERROR("pdcomm_client_connect returned error: %d", ret_val, 0, 0);
      return FALSE;
    }
    else
    {
      return tm_vx_update_comm_state(TM_VX_COMM_TYPE_SMS, TM_VX_COMM_STATE_CONNECTING);
    }
  }
  return TRUE;
}

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
boolean tm_vx_ip_connect_to_mpc(void)
{
  pd_comms_return_type            ret_val;
  pd_comms_get_handle_params_type get_handle_param;
  pd_comms_connect_params_type    connect_param;  
 
  /* Make sure we have proper pdcomm handle first */
  if(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].handle_valid == FALSE)
  {
    get_handle_param.e_link_type   = tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].link_type;
    get_handle_param.q_base_msg_id = tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_msg_id_base;

    ret_val = pd_comms_app_get_handle(
                                       &get_handle_param, 
                                       &(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_handle)
                                     );

    if(ret_val != PDCOMM_RESULT_OK)
    {
      MSG_ERROR("pd_comms_app_get_handle returned error: %d",ret_val,0,0);
      return FALSE;
    }
    else
    {
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].handle_valid = TRUE; /*lint !e506 */
    }
  }
  
  /* When we're here, we must have a valid pd_comm handle, now try connect */
  connect_param.e_link_type = LINK_TCP;
  connect_param.e_use_security =  SECURITY_NONE;
  connect_param.q_transaction_id = tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_TCP);
  connect_param.z_link_params.z_tcp_params.e_type    = PD_COMMS_IP_ADDR;
  connect_param.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4 = tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_adrs;
  connect_param.z_link_params.z_tcp_params.ip_addr_type = PD_COMMS_IP_ADDR_IPV4;
  connect_param.z_link_params.z_tcp_params.w_ip_port = tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_port_id;  /*lint !e734 */
  
  /*Fill in the APN profile type to used*/
  connect_param.z_link_params.z_tcp_params.profile_type = PDCOMM_APN_PROFILE_TYPE_LBS;

  ret_val = pd_comms_app_connect(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_handle,
                                 (uint32)THREAD_ID_SM_TM,
                                 tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_msg_id_base,
                                 &connect_param);

  if(ret_val != PDCOMM_RESULT_WAIT)
  {
    MSG_ERROR("pdcomm_client_connect returned error: %d", ret_val, 0, 0);
    return FALSE;
  }
  else
  {
    (void)os_TimerStart(tm_vx_gen_info.sess_info.mpc_connection_timer,
                        TM_VX_MS_MPC_CONN_TIMER_VALUE,
                        NULL);

    return tm_vx_update_comm_state(TM_VX_COMM_TYPE_TCP, TM_VX_COMM_STATE_CONNECTING);
  }
}

/*===========================================================================

FUNCTION tm_vx_get_pde_info

DESCRIPTION
  This function fills in the correct PDE address information for V1/V2 protocol
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void tm_vx_get_pde_info(pdsm_pd_server_info_s_type *server_info)
{
  int  active_app_index;

  if(server_info==NULL)
  {
    MSG_ERROR("NULL server", 0, 0 ,0);
    return;
  }

  active_app_index = tm_vx_gen_info.sess_info.active_app_index;
  
  if((active_app_index > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_index < (int)PDSM_CLIENT_TYPE_MAX))
  {
    memscpy((void *)server_info, sizeof(*server_info),
           (void *)&(tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo),
           sizeof(tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo));
  }
  else
  {
    MSG_ERROR("No active client", 0, 0 ,0);
  }  
}

/*===========================================================================

FUNCTION tm_vx_save_start_sess_req_param

DESCRIPTION
  This function saves the parameters passed from TM-Core in app_info array.
  It will be used in SPPReq/SPPResp and GPS PD sessions
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void tm_vx_save_start_sess_req_param
(
  int                                     app_index,
  tm_sess_handle_type                     tm_core_sess_handle,  
  const prtl_start_sess_req_param_s_type  *start_sess_req_param
)
{
  if( (app_index > (int)PDSM_CLIENT_TYPE_NONE) 
    &&(app_index < (int)PDSM_CLIENT_TYPE_MAX)
    &&(start_sess_req_param != NULL))
  {
    tm_vx_gen_info.sess_info.tm_core_sess_handle    = tm_core_sess_handle;
    memscpy((void *)&(tm_vx_gen_info.app_info[app_index].pd_req_info), 
             sizeof(tm_vx_gen_info.app_info[app_index].pd_req_info),
            (void *)start_sess_req_param, sizeof(*start_sess_req_param));
  }
}
  
/*===========================================================================
FUNCTION tm_vx_get_link_type_by_tr_id

DESCRIPTION
  This function returns link_type based on transaction id

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None

===========================================================================*/
tm_vx_comm_e_type tm_vx_get_link_type_by_tr_id
(
  uint32 transaction_id
)
{  
  switch(transaction_id & TM_VX_TRANSACTION_ID_DISPATCH_MASK)
  {
    case TM_VX_TCPIP_TRANSACTION_ID_BASE:
      return TM_VX_COMM_TYPE_TCP;

    case TM_VX_SMS_TRANSACTION_ID_BASE:
      return TM_VX_COMM_TYPE_SMS;

    default:
      return TM_VX_COMM_TYPE_NONE;
  }
}

/*===========================================================================
FUNCTION tm_vx_handle_mpc_connected

DESCRIPTION
  This function is called when MPC connection is established. Based on the current
  app state, it performs corresponding actions.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_handle_mpc_connected(void)
{
  int     active_app_idx;
  int     msg_length = 0;
  byte    *msg_ptr;
  boolean result = TRUE; /*lint !e506 */
  pd_comms_write_params_type write_params;
  pd_comms_return_type       ret_val;  

  active_app_idx = tm_vx_gen_info.sess_info.active_app_index;

  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {
    /* Clear MS-MPC timer */
    (void) os_TimerStop(tm_vx_gen_info.sess_info.mpc_connection_timer);
                
    MSG_MED("Connected to MPC when app is %d state", tm_vx_gen_info.app_info[active_app_idx].app_state, 0, 0);
    
    switch(tm_vx_gen_info.app_info[active_app_idx].app_state)
    {
      case TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ:
      {
        /* Get free buffer for packed SPPReq */
        msg_ptr = (byte *)pdsm_getbuf(TM_VX_MAX_MSG_SIZE);

        if(msg_ptr != NULL)
        {      
          if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
          {
            /* Create V1 SPPReq here */
            msg_length = tm_vx_msg_create_v1_sppreq(msg_ptr, TM_VX_MAX_MSG_SIZE);
          }
          else if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
          {
            /* Create V2 SPPReq here */
            msg_length = tm_vx_msg_create_v2_sppreq(msg_ptr, TM_VX_MAX_MSG_SIZE);
          }
          
          if(msg_length > 0)
          {
            write_params.q_transaction_id = tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_TCP);
            write_params.w_data_len = msg_length;  /*lint !e734 */
            write_params.p_data = (uint8 *)msg_ptr;
              
            ret_val = pd_comms_app_write(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_handle,
                                         (uint32)THREAD_ID_SM_TM,
                                         tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_msg_id_base,
                                         &write_params);

            if(ret_val != PDCOMM_RESULT_WAIT)
            {
              MSG_ERROR("Error (%d) when trying to send data",ret_val,0,0);
              result = FALSE;
            }
            else
            {
              /* Log SPPReq */
              tm_vx_rev_logging(write_params.p_data, write_params.w_data_len);            

              /* Start T7 timer */
              (void)os_TimerStart(tm_vx_gen_info.sess_info.t7_timer, 
                                  TM_VX_T7_TIMER_VALUE,
                                  NULL);
                
              (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_TO_RECV_SPPRESP);

               result = TRUE;
            }
          }
          else
          {
            MSG_ERROR("Error when createing SPPReq", 0, 0, 0);
            result = FALSE;
          }

          (void)pdsm_freebuf((char *)msg_ptr);
        }
        else
        {
          MSG_ERROR("No buffer available", 0, 0, 0);
          result = FALSE;
        }

        break;
      }

      case TM_VX_APP_STATE_WAIT_TO_SEND_POS_RPT:
      {
        if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
        {
          /* Get free buffer for packed Pos Report */
          msg_ptr = (byte *)pdsm_getbuf(TM_VX_MAX_MSG_SIZE);

          if(msg_ptr != NULL)
          {          
            /* Create V2 Pos Rpt here */
            msg_length = tm_vx_msg_create_v2_pos_rpt(msg_ptr, TM_VX_MAX_MSG_SIZE);

            if(msg_length > 0)
            {
              write_params.q_transaction_id = tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_TCP);
              write_params.w_data_len = msg_length;  /*lint !e734 */
              write_params.p_data = (uint8 *)msg_ptr;

              ret_val = pd_comms_app_write(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_handle,
                                           (uint32)THREAD_ID_SM_TM,
                                           tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_msg_id_base,
                                           &write_params);
  
              if(ret_val != PDCOMM_RESULT_WAIT)
              {
                MSG_ERROR("Error (%d) when trying to send data",ret_val,0,0);
                result = FALSE;
              }
              else
              {
                /* Log Pos Rpt */
                tm_vx_rev_logging(write_params.p_data, write_params.w_data_len);

                /* Start T14 timer */
                (void)os_TimerStart(tm_vx_gen_info.sess_info.t14_timer,
                                    TM_VX_T14_TIMER_VALUE,
                                    NULL);
                
                (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_TO_RECV_POS_RPT_ACK);

                result = TRUE;                
              }
            }
            else
            {
              MSG_ERROR("Error when createing V2 Pos report", 0, 0, 0);
              result = FALSE;
            }          
            (void)pdsm_freebuf((char *)msg_ptr);
          }
          else
          {
            MSG_ERROR("No buffer available", 0, 0, 0);
            result = FALSE;
          }          
        }
        else
        {
          MSG_ERROR("Connected to MPC in an invalid state", 0, 0, 0);
          result = FALSE;          
        }
        break;
      }

      case TM_VX_APP_STATE_WAIT_TO_SEND_CANCEL:
      {
        if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
        {
          /* Get free buffer for packed Pos Report */
          msg_ptr = (byte *)pdsm_getbuf(TM_VX_MAX_MSG_SIZE);

          if(msg_ptr != NULL)
          {          
            /* Create V2 Pos Rpt here */
            msg_length = tm_vx_msg_create_v2_mo_cancel(msg_ptr, TM_VX_MAX_MSG_SIZE);

            if(msg_length > 0)
            {
              write_params.q_transaction_id = tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_TCP);
              write_params.w_data_len = msg_length;  /*lint !e734 */
              write_params.p_data = (uint8 *)msg_ptr;

              ret_val = pd_comms_app_write(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_handle,
                                           (uint32)THREAD_ID_SM_TM,
                                           tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_msg_id_base,
                                           &write_params);
  
              if(ret_val != PDCOMM_RESULT_WAIT)
              {
                MSG_ERROR("Error (%d) when trying to send data",ret_val,0,0);
                result = FALSE;
              }
              else
              {
                /* Log Pos Rpt */
                tm_vx_rev_logging(write_params.p_data, write_params.w_data_len);

                result = TRUE;                
              }
            }
            else
            {
              MSG_ERROR("Error when createing V2 mo cancel", 0, 0, 0);
              result = FALSE;
            }          
            (void)pdsm_freebuf((char *)msg_ptr);
          }
          else
          {
            MSG_ERROR("No buffer available", 0, 0, 0);
            result = FALSE;
          }          

          /* No matter we've sent MO cancel successfully or not, re-init TM-Vx module */
          tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
        }
        else
        {
          MSG_ERROR("Connected to MPC in an invalid state", 0, 0, 0);
          result = FALSE;          
        }        
        break;
      }
      
      default:
        MSG_ERROR("Invalid state ( %d ) for MPC connection", tm_vx_gen_info.app_info[active_app_idx].app_state, 0, 0);
        result = FALSE;
      break;
    }  
  }
  else
  {
    MSG_ERROR("Invalid app_index", 0, 0, 0);
    result = FALSE;
  }  
  return result;
}
  
/*===========================================================================
FUNCTION tm_vx_handle_tcp_incomming_data

DESCRIPTION
  This function is called when PD COMM send a read notication

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_handle_tcp_incomming_data(const void *msg_data_ptr)
{
  boolean ret_val= TRUE; /*lint !e506 */
  int     active_app_idx;
  uint8   *data_ptr;
  pd_comms_ipc_read_data_hdr_type* read_hdr_ptr;

  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Received Null msg_data_ptr",0,0,0);
    return FALSE;
  }
  
  active_app_idx = tm_vx_gen_info.sess_info.active_app_index;

  /* Make sure there is an active app to handle incoming messages */
  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {
    read_hdr_ptr = (pd_comms_ipc_read_data_hdr_type*)msg_data_ptr;    

    MSG_MED("TCP Msg length = %d", read_hdr_ptr->w_data_len, 0, 0);
    if((read_hdr_ptr->w_data_len > 0) && (read_hdr_ptr->w_data_len <= TM_VX_MAX_MSG_SIZE))
    {
      /* 
         Read the message header first to get incoming messsage payload length
         and then inform PD Comm the information
      */    
      if(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].recv_state == TM_VX_COMM_RECV_STATE_HEADER)
      {
        tm_vx_fwd_logging(TM_VX_COMM_RECV_STATE_HEADER, read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);
          
        data_ptr = (uint8 *)read_hdr_ptr->p_data; /*lint !e831 */
        
        if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
        {
          /* Verify version and length, if the message is correct, then read the payload */
          if((*data_ptr == (uint8)TM_VX_VERSION_V1_ONLY) && (read_hdr_ptr->w_data_len >= TM_V1_TCP_HDR_SIZE))
          {
            //tm_vx_gen_info.sess_info.v1_sppresp.version = TM_VX_VERSION_V1_ONLY;          
            tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length = *(data_ptr + 1); /*lint !e415 */
            tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length <<= 8;
            tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length |= *(data_ptr + 2); /*lint !e415 !e416 !e831 */
            tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length -= 3;
            MSG_HIGH("V1 Msg Payload length = %d", tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length, 0, 0);
            ret_val = TRUE; /*lint !e506 */
          }
          else
          {
            MSG_ERROR("Recv'd known message", 0, 0, 0);
            ret_val = FALSE;
          }
        }
        else if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
        {
          /* Verify version, length and msg type, if the message is correct, then read the payload */
          if(   (*data_ptr == (uint8)TM_VX_VERSION_V2_ONLY) && (read_hdr_ptr->w_data_len >= TM_V2_TCP_HDR_SIZE)
             && ((*(data_ptr + 1) == TM_V2_MESSAGE_TYPE_SPPRESP) || (*(data_ptr + 1) == TM_V2_MESSAGE_TYPE_POS_RPT_RSP))
            ) /*lint !e415 */
          {
            if(*(data_ptr + 1) == TM_V2_MESSAGE_TYPE_SPPRESP) /*lint !e415 */
            {
              tm_vx_gen_info.sess_info.v2_sppresp.version      = (uint8)TM_VX_VERSION_V2_ONLY;
              tm_vx_gen_info.sess_info.v2_sppresp.message_type = TM_V2_MESSAGE_TYPE_SPPRESP;
            }
            else
            {
              tm_vx_gen_info.sess_info.v2_pos_rpt_resp.version      = (uint8)TM_VX_VERSION_V2_ONLY;
              tm_vx_gen_info.sess_info.v2_pos_rpt_resp.message_type = TM_V2_MESSAGE_TYPE_POS_RPT_RSP;            
            }
            
            tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length = *(data_ptr + 2); /*lint !e415 !e416 !e831 */
            tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length <<= 8;
            tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length |= *(data_ptr + 3); /*lint !e415 !e416 !e831 */
            MSG_HIGH("V2 Msg Payload length = %d", tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length, 0, 0);
            ret_val = TRUE; /*lint !e506 */
          }
          else
          {
            MSG_ERROR("Recv'd unknown message", 0, 0, 0);
            ret_val = FALSE;
          }
        }       
        /* Only read payload if message header is valid */
        if(ret_val == TRUE) /*lint !e506 !e731 */
        {
          tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].recv_state = TM_VX_COMM_RECV_STATE_PAYLOAD;
        }
      }
      else /* Vx message payload */
      {
        tm_vx_fwd_logging(TM_VX_COMM_RECV_STATE_PAYLOAD, read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);
        
        tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].recv_state = TM_VX_COMM_RECV_STATE_HEADER;

        if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
        {
          switch(tm_vx_gen_info.app_info[active_app_idx].app_state)
          {
            case TM_VX_APP_STATE_WAIT_TO_RECV_SPPRESP:
            {
              /* Decode SPPResp */
              ret_val = tm_vx_msg_decode_v1_sppresp((byte *)read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);
              
              if(ret_val)
              {
                /* Clear T7 timer */
                (void) os_TimerStop(tm_vx_gen_info.sess_info.t7_timer);
             
                /* Act upon SPPResp */
                tm_v1_handle_sppresp();
              }
              else
              {
                MSG_ERROR("Error decoding V1 SPPResp", 0, 0, 0);
              }

              ret_val = tm_vx_ip_disconnect_from_mpc(FALSE);
            }
            break;

            default:
              ret_val = FALSE;
            break;
          } 
        }
        else if (tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
        {
          switch(tm_vx_gen_info.app_info[active_app_idx].app_state)
          {
            case TM_VX_APP_STATE_WAIT_TO_RECV_SPPRESP:
            {
              /* Decode SPPResp */
              ret_val = tm_vx_msg_decode_v2_sppresp((byte *)read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);
              
              if(ret_val)
              {
                /* Clear T7 timer */
                (void) os_TimerStop(tm_vx_gen_info.sess_info.t7_timer);
             
                /* Act upon SPPResp */
                tm_v2_handle_sppresp();
              }
              else
              {
                MSG_ERROR("Error decoding V2 SPPResp", 0, 0, 0);
              }

              ret_val = tm_vx_ip_disconnect_from_mpc(FALSE);
            }
            break;

            case TM_VX_APP_STATE_WAIT_TO_RECV_POS_RPT_ACK:
            {              
              /* Decode Position Report Response */
              ret_val = tm_vx_msg_decode_v2_pos_rpt_resp((byte *)read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);

              if(ret_val)
              {
                /* Clear T14 timer */
                (void) os_TimerStop(tm_vx_gen_info.sess_info.t14_timer);
                
                /* Update app state */          
                (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_INIT);

                /* Act upon Pos Rpt Resp */
                tm_v2_handle_pos_rpt_resp();
              }
              else
              {
                MSG_ERROR("Error decoding V2 Pos Report Response", 0, 0, 0);
              }
              
              ret_val = tm_vx_ip_disconnect_from_mpc(FALSE);
            }
            break;
            
            default:
              ret_val = FALSE;
            break;
          } 
        }  
      }
    }
    /* read_hdr_ptr->w_data_len means servier tears down the connection */
    else
    {
      /* Received data of zero length from TM-Core */
      MSG_ERROR("Received data of zero length => Server tears down the connection", 0, 0, 0);
      ret_val = FALSE;
    }
  }
  else
  {
    MSG_HIGH("No active app", 0, 0, 0);
    ret_val = FALSE;  
  }  
  
  return ret_val;
}

/*===========================================================================
FUNCTION tm_vx_handle_tcp_read_notification

DESCRIPTION
  This function is called when PD COMM send a read notication

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_handle_tcp_read_notification(void)
{
  pd_comms_return_type       ret_val = PDCOMM_RESULT_OK;
  pd_comms_read_params_type  read_params;
  
  if(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].recv_state == TM_VX_COMM_RECV_STATE_HEADER)
  {
    if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
    {
      MSG_HIGH("Reading V1 header",0,0,0);    

      read_params.w_data_len = TM_V1_TCP_HDR_SIZE;
    }
    else if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
    {
      MSG_HIGH("Reading V2 header",0,0,0);
      read_params.w_data_len = TM_V2_TCP_HDR_SIZE;
    }  
  }
  else
  {
    MSG_HIGH("Reading Vx payload", 0, 0, 0);
    read_params.w_data_len = tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].incoming_data_length;
  }

  read_params.u_wait_for_all   = TRUE; /*lint !e506 */
  read_params.q_transaction_id = tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_TCP);
  
  ret_val = pd_comms_app_read(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_handle,
                              (uint32)THREAD_ID_SM_TM,
                              tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].pdcomm_msg_id_base,
                              &read_params);  

  if(ret_val == PDCOMM_RESULT_WAIT)
  {
    return TRUE; /*lint !e506 */
  }
  else
  {
    return FALSE;
  }
}


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
boolean tm_vx_send_mo_sms
(
  uint8 *msg_data_ptr,
  uint8 msg_data_length
)
{
  pd_comms_return_type result;
  pd_comms_write_params_type write_params;

  memscpy((void*)&write_params.z_tl_write_params.z_sms_write_params.z_address,
         sizeof(write_params.z_tl_write_params.z_sms_write_params.z_address),
         (void*)&(tm_vx_gen_info.sess_info.mt_sms_address), 
         sizeof(tm_vx_gen_info.sess_info.mt_sms_address));
  
  write_params.z_tl_write_params.z_sms_write_params.e_teleservice = (wms_teleservice_e_type)tm_vx_gen_info.vx_config.vx_ni_teleservice_id;
   
  write_params.q_transaction_id = tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_SMS);

  write_params.w_data_len = msg_data_length;
  write_params.p_data = msg_data_ptr;

  result = pd_comms_app_write(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_handle,
                              (uint32)THREAD_ID_SM_TM,
                              tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_msg_id_base,
                              &write_params);

  if(result != PDCOMM_RESULT_WAIT)
  {
    MSG_ERROR("Error (%d) when trying to send data", result, 0, 0);
    return FALSE;
  }
  else
  {
    return TRUE; /*lint !e506 */
  }
}


/*===========================================================================
FUNCTION tm_vx_handle_tcp_comm_notification

DESCRIPTION
  This function handles pd comm notification for tcp/ip link

DEPENDENCIES 
  None.

RETURN VALUE
 TURE:  if notification is handled correctly
 FALSE: otherwiase
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_handle_tcp_comm_notification
(
  const void *msg_data_ptr
)
{
  boolean result = FALSE;
  pd_comms_ipc_notification_hdr_type* notify_ptr;

  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Received Null msg_data_ptr",0,0,0);
    return FALSE;
  }
  
  notify_ptr = (pd_comms_ipc_notification_hdr_type*)msg_data_ptr;

  if(notify_ptr->e_type == STATE_CHANGE)
  {
    MSG_MED("Vx TCP/IP comm recv'd state change notification, new stae = %d", notify_ptr->z_params.e_new_state, 0, 0);

    if(notify_ptr->z_params.e_new_state == PDCOMM_STATE_CONNECTED)
    {
      (void)tm_vx_update_comm_state(TM_VX_COMM_TYPE_TCP, TM_VX_COMM_STATE_CONNECTED);
      result = tm_vx_handle_mpc_connected();
    }
    else if(notify_ptr->z_params.e_new_state == PDCOMM_STATE_DISCONNECTED)
    {
      (void)tm_vx_update_comm_state(TM_VX_COMM_TYPE_TCP, TM_VX_COMM_STATE_DISCONNECTED);

      if((tm_vx_gen_info.sess_info.active_app_index > (int)PDSM_CLIENT_TYPE_NONE) && (tm_vx_gen_info.sess_info.active_app_index < (int)PDSM_CLIENT_TYPE_MAX))
      {    
        /* If TCP/IP is disconnected while waiting to send/receive SPPREQ then stop MS-MPC timer and re-init TM Vx state machine */
        if ((tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state == TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ) ||
            (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state == TM_VX_APP_STATE_WAIT_TO_RECV_SPPRESP))
        {
          MSG_HIGH("MS-MPC connection issue during SPPREQ/SPPRESP send/recv. Force re-init.", 0,0,0);
          (void)os_TimerStop(tm_vx_gen_info.sess_info.mpc_connection_timer);        
          tm_vx_ms_mpc_timer_handler();  /* Force the timer expiration handler routine to clean up Vx state machine */
        }
      }
      result = TRUE; /*lint !e506 */
    }
    else if(notify_ptr->z_params.e_new_state == PDCOMM_STATE_CONNECTING)
    {
      result = TRUE; /*lint !e506 */
    }
    else if(notify_ptr->z_params.e_new_state == PDCOMM_STATE_DISCONNECTING)
    {
      result = TRUE; /*lint !e506 */
    }
    else
    {
      MSG_ERROR("Unknown state %d",notify_ptr->z_params.e_new_state,0,0);
      result = FALSE;
    }
  }  
  else if(notify_ptr->e_type == READ_DATA)
  {
    MSG_MED("Vx TCP/IP comm recv'd read data notification", 0, 0, 0);

    if(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].comm_state == TM_VX_COMM_STATE_CONNECTED)
    {
      result = tm_vx_handle_tcp_read_notification();          
    }
    else if((tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].comm_state == TM_VX_COMM_STATE_DISCONNECTING)&&
            (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state == TM_VX_APP_STATE_WAIT_PD_SESSION))
    {
      MSG_HIGH("Read event in Disconnecting state after receiving SPPResp, ignore it", 0, 0, 0);
      result = TRUE;
    }
    else
    {
      MSG_ERROR("Read data in error state %d", tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].comm_state, 0, 0);
      result = FALSE;
    }
  }
  else if(notify_ptr->e_type == ERROR_RPT)
  {
    MSG_MED("Vx TCP/IP comm recv'd error_rpt notification", 0, 0, 0);
   (void)tm_vx_update_comm_state(TM_VX_COMM_TYPE_TCP, TM_VX_COMM_STATE_DISCONNECTED);

    /* If TCP/IP is disconnected while waiting to send/receive SPPREQ then stop MS-MPC timer and re-init TM Vx state machine */
    if((tm_vx_gen_info.sess_info.active_app_index > (int)PDSM_CLIENT_TYPE_NONE) && (tm_vx_gen_info.sess_info.active_app_index < (int)PDSM_CLIENT_TYPE_MAX))
    {
      if (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state == TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ) 
      {
        MSG_HIGH("MS-MPC connection issue during SPPREQ. Force stop timer.", 0,0,0);
        (void)os_TimerStop(tm_vx_gen_info.sess_info.mpc_connection_timer);      
  	    tm_vx_ms_mpc_timer_handler();  /* Force the timer expiration handler routine to clean up Vx state machine */
      }
      else if  (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state == TM_VX_APP_STATE_WAIT_TO_RECV_SPPRESP)
      {
        MSG_HIGH("MS-MPC connection issue during SPPRESP. Force stop timer.", 0,0,0);
        (void)os_TimerStop(tm_vx_gen_info.sess_info.t7_timer);   
        tm_vx_t7_timer_handler();/* Force the timer expiration handler routine to clean up Vx state machine */    
      }
      result = TRUE;
    }
  }

  return result;
}

/*===========================================================================
FUNCTION tm_vx_release_current_session

DESCRIPTION
  This function is called to release all the resource allocated to a session
  so that next Vx request can go thru.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_release_current_session(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;

  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {
    MSG_MED("Release current active session", 0, 0, 0);
    
    tm_vx_gen_info.app_info[active_app_idx].authenticated       = FALSE;
    tm_vx_gen_info.app_info[active_app_idx].app_info_length     = 0;
    tm_vx_gen_info.app_info[active_app_idx].brew_class_id       = 0;    
    (void)tm_vx_update_app_state(active_app_idx, TM_VX_APP_STATE_INIT);

    /* Reset active app index */       
    tm_vx_gen_info.sess_info.active_app_index = PDSM_CLIENT_TYPE_NONE;

    /* Stop Timers */
    (void) os_TimerStop(tm_vx_gen_info.sess_info.auth_duration_timer);
    (void) os_TimerStop(tm_vx_gen_info.sess_info.t7_timer);
    (void) os_TimerStop(tm_vx_gen_info.sess_info.t14_timer);
    (void) os_TimerStop(tm_vx_gen_info.sess_info.t15_timer);  
    (void) os_TimerStop(tm_vx_gen_info.sess_info.mpc_connection_timer);    
  }
  else
  {
    MSG_MED("No active session", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION tm_vx_resume_mo_req

DESCRIPTION
  This function is called to resume a MO session
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_resume_mo_req(void)
{  
  prtl_start_sess_req_param_s_type *start_sess_req_param_ptr;
  boolean                          ret_val = FALSE;
  
  if( (tm_vx_gen_info.sess_info.pending_mo_app_index > (int)PDSM_CLIENT_TYPE_NONE) 
    &&(tm_vx_gen_info.sess_info.pending_mo_app_index < (int)PDSM_CLIENT_TYPE_MAX))
  {
    tm_vx_gen_info.sess_info.active_app_index     = tm_vx_gen_info.sess_info.pending_mo_app_index;
    tm_vx_gen_info.sess_info.pending_mo_app_index = PDSM_CLIENT_TYPE_NONE;

    /* Set app state to waiting to send sppreq */
    (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ);

     start_sess_req_param_ptr = &(tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].pd_req_info);
      
    if(start_sess_req_param_ptr->pd_option.lsinfo.server_option == PDSM_SERVER_OPTION_USE_DEFAULT)
    {
      /* Use MPC server address / port stored in NV */
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_addr_type                  = TM_VX_MPC_ADR_TYPE_IPV4;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_adrs    = tm_vx_gen_info.vx_config.vx_mpc_adrs;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_port_id = tm_vx_gen_info.vx_config.vx_mpc_port_id;
    }
    else
    {
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_addr_type                  = TM_VX_MPC_ADR_TYPE_IPV4;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_adrs    = start_sess_req_param_ptr->pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_port_id = start_sess_req_param_ptr->pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id;
    }

    ret_val = tm_vx_ip_connect_to_mpc();
        
  }
  else
  {
    MSG_ERROR("Trying to resume MO, but pending idx is invalid", 0, 0, 0);
  }

  if(ret_val == FALSE)
  {
    /* Abort TM-core and Vx */
    tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);
  }
}

/*===========================================================================
FUNCTION tm_vx_handle_client_init

DESCRIPTION
  This function is called to handle lcs client init 

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_handle_client_init
(
  pdsm_lcs_agent_client_init_s_type  *clint_init_info_ptr
)
{
  int i;
  boolean ret_val = TRUE;
  pdsm_client_type_e_type client_type;
    
  MSG_MED("tm_vx_handle_client_init", 0, 0, 0);
  
  if(clint_init_info_ptr != NULL)
  {    
    if(clint_init_info_ptr->app_info_ptr != NULL)
    {    
      client_type  = clint_init_info_ptr->client_type;
    
      if( (client_type > PDSM_CLIENT_TYPE_NONE)
        &&(client_type < PDSM_CLIENT_TYPE_MAX))
      {
        for(i=0; (clint_init_info_ptr->app_info_ptr[i] != '\0') && (i < (TM_VX_MAX_APP_INFO_LEN - 1)); i++)
        {
          tm_vx_gen_info.app_info[client_type].app_info[i] = clint_init_info_ptr->app_info_ptr[i];
        }

        tm_vx_gen_info.app_info[client_type].app_info[i] = '\0';
        tm_vx_gen_info.app_info[client_type].app_info_length = i;
        ret_val = TRUE;
      }
      else
      {
        MSG_ERROR("Invalid client type %d", client_type, 0, 0);
        ret_val = FALSE;
      }
    }
    else
    {
      MSG_ERROR("Null app_info_ptr", 0, 0, 0); 
      ret_val = FALSE;
    }
  }
  else
  {
    MSG_ERROR("Null clint_init_info_ptr", 0, 0, 0);
    ret_val = FALSE;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_vx_handle_ms_abort_session_req

DESCRIPTION
  This function is to perform necessary V2 call flows when MS aborts a session
  for various reasons
  example: user abort
           e911
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_vx_handle_ms_abort_session_req(const tm_sess_stop_reason_e_type abort_reason)
{
  if(tm_vx_gen_info.vx_config.vx_version ==  TM_VX_VERSION_V1_ONLY)
  {
    /* Perform V1 specific call flow when user cancels a session */
    tm_v1_handle_ms_abort_session_req(TM_STOP_REASON_NONE);
  }
  else if(tm_vx_gen_info.vx_config.vx_version ==  TM_VX_VERSION_V2_ONLY)
  {
    /* Perform V2 specific call flow when user cancels a session */
    tm_v2_handle_ms_abort_session_req(abort_reason);
  }
  else
  {
    MSG_MED("Ignore pd session done", 0, 0, 0);
  }
       
  return; 
}

/*===========================================================================
FUNCTION tm_vx_handle_client_release

DESCRIPTION
  This function is called to handle lcs client release 

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_handle_client_release
(
  pdsm_lcs_agent_client_release_s_type  *clint_release_info_ptr
)
{
  boolean ret_val = TRUE;
  
  MSG_MED("tm_vx_handle_client_release", 0, 0, 0);
  
  if(clint_release_info_ptr != NULL)
  {    
    if(tm_vx_gen_info.sess_info.active_app_index == clint_release_info_ptr->client_type)
    {
      if(clint_release_info_ptr->client_type == PDSM_CLIENT_TYPE_BREW)
      {
        if(tm_vx_gen_info.app_info[PDSM_CLIENT_TYPE_BREW].brew_class_id == clint_release_info_ptr->class_id)
        {
          MSG_HIGH("Release Vx session for BREW client", 0, 0, 0);
          tm_vx_handle_ms_abort_session_req(TM_STOP_REASON_GENERAL_ERROR);
          ret_val = TRUE;
        }
        else
        {
          MSG_HIGH("Can not release Vx session because of Brew class id does not match", 0, 0, 0);
          ret_val = FALSE;
        }
      }
      else
      {
        MSG_HIGH("Release Vx session for MI client", 0, 0, 0);
        tm_vx_handle_ms_abort_session_req(TM_STOP_REASON_GENERAL_ERROR);
        ret_val = TRUE;
      }
    }
    else
    {
      MSG_ERROR("Client type does not match", 0, 0, 0);
      ret_val = FALSE;    
    }
  }
  else
  {
    MSG_ERROR("Null clint_release_info_ptr", 0, 0, 0);
    ret_val = FALSE;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION tm_vx_handle_app_response

DESCRIPTION
  This function is called to see if an NI request is accepted by applciation
  or not.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_handle_app_response
(
  pdsm_lcs_agent_notification_resp_e_type  app_resp
)
{
  tm_sess_req_param_u_type  sess_req_param;
  
  /* Stop T15 timer */
  (void)os_TimerStop(tm_vx_gen_info.sess_info.t15_timer);
      
  if( (tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
    &&(tm_vx_gen_info.sess_info.pending_mo_app_index != PDSM_CLIENT_TYPE_NONE))
  {
    /* Two scenarios can happen 
       1. Continue with NI, reject MO request to MO application
       2. Abort NI, resume MO call flow
    */
    /* Deny means user wants to abort NI */
    if(app_resp == PDSM_LCS_NOTIFY_VERIFY_DENY)
    {
      /* Abort NI, and resume MO */
      MSG_HIGH("User choose MO over NI", 0, 0, 0);

      /* Abort NI first */
      (void) tm_v2_send_mo_cancel_sms(TM_VX_MSG_V2_CANCEL_REASON_USER_INIT);

      /* Stop NI client */
      tm_vx_ni_client_stop_session();
       
      /* Do partial reset to clean up current active session */
      tm_vx_release_current_session();
      
      /* Continue with MO */
      tm_vx_resume_mo_req();
    }
    else
    {
      /* Contiune NI, reject MO */
      MSG_HIGH("User continue with NI", 0, 0, 0);

      /* Report to TM-Core to fail the incoming MO request */
      sess_req_param.stop_param.stop_type   = TM_STOP_TYPE_SESSION;
      sess_req_param.stop_param.stop_reason = TM_STOP_REASON_USER_TERMINATED;
   
      (void)tm_sess_req(TM_PRTL_TYPE_1X_UP,
                        tm_vx_gen_info.sess_info.tm_core_sess_handle,
                        TM_SESS_REQ_STOP,
                        &sess_req_param);      
    }
  }
  else
  {
    if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
    {    
      tm_v1_handle_app_response(app_resp);  
    }
    else if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
    {    
      tm_v2_handle_app_response(app_resp);
    }
    else
    {
      MSG_ERROR("Ingore user resposne", 0, 0, 0);
    }
  }
}
  
/*===========================================================================
FUNCTION tm_vx_handle_sms_incomming_data

DESCRIPTION
  This function is called when PD COMM send a read notication

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/   
boolean tm_vx_handle_sms_incomming_data
(
  const void *msg_data_ptr
)
{
  boolean ret_val= FALSE;
  uint8   *data_ptr;
  pd_comms_ipc_read_data_hdr_type* read_hdr_ptr;

  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Received Null msg_data_ptr",0,0,0);
    return FALSE;
  }
  
  read_hdr_ptr = (pd_comms_ipc_read_data_hdr_type*)msg_data_ptr;    

  if((read_hdr_ptr->w_data_len > 0) && (read_hdr_ptr->w_data_len <= TM_VX_MAX_MSG_SIZE))
  {
    data_ptr = (uint8 *)read_hdr_ptr->p_data;      

    if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
    {
      if((*data_ptr == (uint8)TM_VX_VERSION_V1_ONLY) && (read_hdr_ptr->w_data_len > TM_V1_SMS_HDR_SIZE))
      {
        MSG_HIGH("Handle V1 MT SMS", 0, 0, 0);

         /* Decode V1 MT SMS */
        ret_val = tm_vx_msg_decode_incoming_v1_mt_sms((byte *)read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);

        if(ret_val)
        {
          if(tm_v1_check_incoming_mt_sms() == TRUE)
          {
            /* Act upon V1 MT SMS */
            tm_v1_handle_incoming_mt_sms();
          }
          else
          {
            MSG_ERROR("Failed V1 MT SMS check", 0, 0, 0);
            (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_PROTOCOL_ERROR,
                              &(tm_vx_gen_info.sess_info.incoming_v1_mt_sms));
          }
        }
        else
        {
          MSG_ERROR("Error decoding V1 MT SMS", 0, 0, 0);
        }
      }
      else
      {
        MSG_ERROR("Got MT SMS with wrong version", 0, 0, 0);
        (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_PROTOCOL_ERROR,
                          &(tm_vx_gen_info.sess_info.incoming_v1_mt_sms));
      }
    }
    else if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
    {
      if((*data_ptr == (uint8)TM_VX_VERSION_V2_ONLY) && (read_hdr_ptr->w_data_len > TM_V2_SMS_HDR_SIZE))
      {
        MSG_HIGH("Handle V2 MT SMS", 0, 0, 0);

         /* Decode V2 MT SMS */
        ret_val = tm_vx_msg_decode_incoming_v2_mt_sms((byte *)read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);

        if(ret_val)
        {
          if(tm_v2_check_incoming_mt_sms() == TRUE)
          {
            /* Act upon V2 MT SMS */
            tm_v2_handle_incoming_mt_sms();
          }
          else
          {
            MSG_ERROR("Failed V2 MT SMS check", 0, 0, 0);

            (void) tm_v2_send_mo_reject_sms(TM_VX_MSG_V2_REJ_REASON_VERSION_NOT_SUPPORTED,
                                     tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id);
          }
        }
        else
        {
          MSG_ERROR("Error decoding V2 MT SMS", 0, 0, 0);
        }
      } /* *data_ptr == (uint8)TM_VX_VERSION_V2_ONLY */
      else
      {
        MSG_ERROR("Got MT SMS with wrong version", 0, 0, 0);

        (void) tm_v2_send_mo_reject_sms(TM_VX_MSG_V2_REJ_REASON_VERSION_NOT_SUPPORTED,
                                 tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id);

        /* No need to do TM Vx clean up because TM Vx has not started processing the SMS */        
      } /* *data_ptr == (uint8)TM_VX_VERSION_V2_ONLY */
    } /* tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY */
  }
  else
  {
    /* Received data of invalid length from TM-Core */
    MSG_ERROR("Received SMS Msg with invalid length: %d", read_hdr_ptr->w_data_len, 0, 0);

    ret_val = FALSE;
  } 
  return ret_val;
}

/*===========================================================================
FUNCTION tm_vx_handle_sms_read_notification

DESCRIPTION
  This function is called when PD COMM send a read notication

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/   
boolean tm_vx_handle_sms_read_notification(void)
{
  pd_comms_return_type       ret_val = PDCOMM_RESULT_OK;
  pd_comms_read_params_type  read_params;
  
  read_params.w_data_len = tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].incoming_data_length;
  
  read_params.u_wait_for_all   = TRUE; /*lint !e506 */
  read_params.q_transaction_id = tm_vx_get_next_transaction_id(TM_VX_COMM_TYPE_SMS);
  
  ret_val = pd_comms_app_read(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_handle,
                              (uint32)THREAD_ID_SM_TM,
                              tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].pdcomm_msg_id_base,
                              &read_params);  

  if(ret_val == PDCOMM_RESULT_WAIT)
  {
    return TRUE; /*lint !e506 */
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION tm_vx_handle_sms_comm_notification

DESCRIPTION
  This function handles pd comm notification for sms link

DEPENDENCIES 
  None.

RETURN VALUE
 TURE:  if notification is handled correctly
 FALSE: otherwiase
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_handle_sms_comm_notification
(
  const void *msg_data_ptr
)
{
  boolean result = FALSE;
  pd_comms_ipc_notification_hdr_type* notify_ptr;

  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Received Null msg_data_ptr",0,0,0);
    return FALSE;
  }
  
  notify_ptr = (pd_comms_ipc_notification_hdr_type*)msg_data_ptr;

  if(notify_ptr->e_type == STATE_CHANGE)
  {
    MSG_MED("Vx SMS comm recv'd state change notification, new stae = %d", notify_ptr->z_params.e_new_state, 0, 0);

    if(notify_ptr->z_params.e_new_state == PDCOMM_STATE_CONNECTED)
    {
      (void)tm_vx_update_comm_state(TM_VX_COMM_TYPE_SMS, TM_VX_COMM_STATE_CONNECTED);
      result = TRUE; /*lint !e506 */
    }
    else if(notify_ptr->z_params.e_new_state == PDCOMM_STATE_DISCONNECTED)
    {
      result = TRUE; /*lint !e506 */
    }
    else if(notify_ptr->z_params.e_new_state == PDCOMM_STATE_CONNECTING)
    {
      result = TRUE; /*lint !e506 */
    }
    else if(notify_ptr->z_params.e_new_state == PDCOMM_STATE_DISCONNECTING)
    {
      result = TRUE; /*lint !e506 */
    }
    else
    {
      MSG_ERROR("Unknown state %d",notify_ptr->z_params.e_new_state,0,0);
      result = FALSE;
    }
  }  
  else if(notify_ptr->e_type == READ_DATA)
  {
    MSG_MED("Vx SMS comm recv'd read data notification", 0, 0, 0);

    if(tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].comm_state == TM_VX_COMM_STATE_CONNECTED)
    {
      result = tm_vx_handle_sms_read_notification();   
    }
    else
    {
      MSG_ERROR("Read data in error state %d", tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_SMS].comm_state, 0, 0);
      result = FALSE;
    }
  }
  return result;
}

/*===========================================================================
FUNCTION tm_vx_handle_pdcomm_data

DESCRIPTION
  This function dispatches pdcomm messages to proper handling functions.

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_handle_pdcomm_data
(
  pd_comms_client_ipc_message_type  msg_type,  
  void                              *msg_data_ptr
)
{
  boolean           ret_val = TRUE;
  uint32            *tr_id_ptr;
  tm_vx_comm_e_type link_type;

  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Null msg_data_ptr", 0, 0, 0);
    return FALSE;
  }

  tr_id_ptr = (uint32 *)msg_data_ptr;  
  
  link_type = tm_vx_get_link_type_by_tr_id(*tr_id_ptr);

  switch(msg_type)
  {
    case C_PD_COMMS_CLIENT_MSG_NOTIFY:
      
      if(link_type == TM_VX_COMM_TYPE_TCP)
      {
        ret_val = tm_vx_handle_tcp_comm_notification(msg_data_ptr);
      }
      else if(link_type == TM_VX_COMM_TYPE_SMS)
      {
        ret_val = tm_vx_handle_sms_comm_notification(msg_data_ptr);
      }
      else
      {
        MSG_ERROR("Invalid link type", 0, 0, 0);
        ret_val = FALSE;
      }
      
    break;

    case C_PD_COMMS_CLIENT_MSG_WRITE_ACK:    

      if(link_type == TM_VX_COMM_TYPE_TCP)
      {
        ret_val = TRUE; /*lint !e506 */
      }
      else if(link_type == TM_VX_COMM_TYPE_SMS)
      {
        ret_val = TRUE; /*lint !e506 */
      }
      else
      {
        MSG_ERROR("Invalid link type", 0, 0, 0);
        ret_val = FALSE;
      }
    break;

    case C_PD_COMMS_CLIENT_MSG_IOCTL_ACK:
      
      if(link_type == TM_VX_COMM_TYPE_TCP)
      {
        ret_val = TRUE; /*lint !e506 */
      }
      else if(link_type == TM_VX_COMM_TYPE_SMS)
      {
        ret_val = TRUE; /*lint !e506 */
      }
      else
      {
        MSG_ERROR("Invalid link type", 0, 0, 0);
        ret_val = FALSE;      
      }
      
    break;

    case C_PD_COMMS_CLIENT_MSG_READ_DATA:

      if(link_type == TM_VX_COMM_TYPE_TCP)
      {
        ret_val = tm_vx_handle_tcp_incomming_data(msg_data_ptr);
      }
      else if(link_type == TM_VX_COMM_TYPE_SMS)
      {
        ret_val = tm_vx_handle_sms_incomming_data(msg_data_ptr);
      }
      else
      {
        MSG_ERROR("Invalid link type", 0, 0, 0);
        ret_val = FALSE;    
      }
      
    break;
    
    case C_PD_COMMS_CLIENT_MSG_HANDLE:

      if(link_type == TM_VX_COMM_TYPE_TCP)
      {
        ret_val = TRUE; /*lint !e506 */
      }
      else if(link_type == TM_VX_COMM_TYPE_SMS)
      {
        ret_val = TRUE; /*lint !e506 */
      }
      else
      {
        MSG_ERROR("Invalid link type", 0, 0, 0);
        ret_val = FALSE;      
      }
      
    break;
    
    case C_PD_COMMS_CLIENT_MSG_SMS_ACK_REPORT:

      if(link_type == TM_VX_COMM_TYPE_SMS)
      {
        ret_val = TRUE; /*lint !e506 */
      }
      else
      {
        MSG_ERROR("Invalid link type", 0, 0, 0);
        ret_val = FALSE;      
      }
      
    break;
    
    default:
      
      MSG_ERROR("Received unknown message", 0, 0, 0);
      ret_val = FALSE;
      
    break;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_vx_ms_mpc_timer_handler

DESCRIPTION
  This function is to handle MS-MPC Connection Timer.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_vx_ms_mpc_timer_handler(void)
{      
  MSG_HIGH("MS-MPC timer expired", 0, 0, 0);

  /* Close MPC Connection */
  (void)tm_vx_ip_disconnect_from_mpc(FALSE);

  /* Perform V1/V2 specific call flows */
  if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
  {
    /* Perform V1 specific handling */
    tm_v1_handle_ms_mpc_failure();
  }
  else if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
  {
    /* Perform V2 specific handling */
    tm_v2_handle_ms_mpc_timer_expiry();
  }     

#if defined(FEATURE_CALL_THROTTLE)
  tm_1x_up_mpc_set_throttling_backoff_time();
#endif /* FEATURE_CALL_THROTTLE*/

}

  
/*===========================================================================

FUNCTION tm_vx_timer_handler

DESCRIPTION
  This function is to figure out which TM Vx timer has expired and call proper 
  timer handler.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_vx_auth_duration_timer_handler(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  
  MSG_HIGH("Authention timer expired", 0, 0, 0);

  /* Call a reset function to reset Vx state machine */
  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {
    tm_vx_gen_info.app_info[active_app_idx].auth_duration  = 0;
    tm_vx_gen_info.app_info[active_app_idx].authenticated  = FALSE;  
    tm_vx_gen_info.sess_info.active_app_index              = -1;
    
    /* Increment correlation ID by 1 for next MO session */
    tm_vx_gen_info.sess_info.correlation_id++;

    /* Abort NI client */
    tm_vx_ni_client_stop_session();
  }
}

/*===========================================================================

FUNCTION tm_vx_t7_timer_handler

DESCRIPTION
  This function is to handle T7 expiry
  (From sending SPPReq to receiving SPPResp)
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_vx_t7_timer_handler(void)
{      
  MSG_HIGH("T7 timer expired", 0, 0, 0);

  /* Perform V1/V2 specific call flows */
  if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
  {
    /* Perform V1 specific handling */
    tm_v1_handle_ms_mpc_failure();
  }
  else if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
  {
    /* Perform V2 specific handling */
    tm_v2_handle_t7_timer_expiry();
  }     

#if defined(FEATURE_CALL_THROTTLE)
  tm_1x_up_mpc_set_throttling_backoff_time();
#endif /* FEATURE_CALL_THROTTLE*/

}

/*===========================================================================
FUNCTION tm_vx_handle_t15_timer_expiry

DESCRIPTION
  This function is called to handle Vx T15 (user resonse) timeout
  
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_handle_t15_timer_expiry(void)
{
  tm_vx_handle_app_response(PDSM_LCS_NOTIFY_VERIFY_NORESP);
}

/*===========================================================================

FUNCTION tm_vx_timer_handler

DESCRIPTION
  This function is to figure out which TM Vx timer has expired and call proper 
  timer handler.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_vx_timer_handler(const os_TimerExpiryType *timer_param)
{
  if(timer_param != NULL)
  {
    if((timer_param->q_TimerId & TM_1X_UP_TIMER_ID_DISPATCH_MASK) == TM_1X_UP_VX_TIMER_ID_BASE)
    {
      switch(timer_param->q_TimerId)
      {
        case TM_VX_TIMER_ID_MS_MPC_CONN:
          tm_vx_ms_mpc_timer_handler();
        break;
        
        case TM_VX_TIMER_ID_AUTH_DURATION:
          tm_vx_auth_duration_timer_handler();
        break;

        case TM_VX_TIMER_ID_NI_TRACKING:
          tm_vx_ni_client_tracking_timer_handler();
        break;

        case TM_VX_TIMER_ID_T7:
          tm_vx_t7_timer_handler();
        break;

        case TM_VX_TIMER_ID_T14:
          /* T14 timer is used only for V2 */
          tm_v2_handle_t14_timer_expiry();
        break;

        case TM_VX_TIMER_ID_T15:
          tm_vx_handle_t15_timer_expiry();
        break;        
                
        default:
          MSG_ERROR("Unknown timer expired", 0, 0, 0);
        break;
      }
    }
    else
    {
      MSG_ERROR("Can not proess invalid timer event", 0, 0, 0);
    }
  }
  else
  {
    MSG_ERROR("Can not proess null timer msg", 0, 0, 0);  
  }
}

/*===========================================================================

FUNCTION tm_vx_fallback_to_standalone 

DESCRIPTION
  This function tries to continue with an SA session when called in the event
  of failures in the Vx interactions. It does so by calling into the IS801
  driver module with the newly introduced TM_IS801_DRV_REQ_START_NODWLD mode.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_vx_fallback_to_standalone(void)
{
   tm_1x_up_is801_drv_req_param_u_type  req_param;
   boolean result;
   int active_app_idx;

   active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
   /* 
     Now kick of gpsOne session by setting wake_tm_core flag to TRUE
     which will make is801 driver module to use "CONTINUE" to wake up TM-Core (TM-Core is in wait state )
   */
   req_param.start_param.wake_tm_core         = TRUE;  
   req_param.start_param.actions              = NULL; 
   req_param.start_param.start_sess_req_param = &(tm_vx_gen_info.app_info[active_app_idx].pd_req_info); 

   if(req_param.start_param.start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_MSASSISTED)
   {
     MSG_HIGH("MSA Request, should not fall back to Standalone", 0,0,0);
     return FALSE;
   }

   tm_vx_app_info_init();

   /* first try to get the SA fix on its way */
   result = tm_1x_up_is801_drv_req(
                             tm_vx_gen_info.sess_info.tm_core_sess_handle, 
                             TM_IS801_DRV_REQ_START_NODWLD,
                             req_param            
                           );
   return result;
}

/*===========================================================================

FUNCTION tm_vx_handle_mo_over_ni_req

DESCRIPTION
  This function performs the necessary actions when getting an MO request
  while Vx is in NI tracking session
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_vx_handle_mo_over_ni_req
(  
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param
)
{
  pdsm_lcs_info_s_type           vx_app_req_info;
  
  /* Save request param for later */
  tm_vx_save_start_sess_req_param(tm_vx_gen_info.sess_info.pending_mo_app_index, sess_handle, start_sess_req_param);

  vx_app_req_info.client_id            = 0;
  vx_app_req_info.lcs_cb_cmd_type      = PDSM_LCS_CMD_TYPE_VX_REQ;
  vx_app_req_info.lcs_info.vx_req.notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
  vx_app_req_info.lcs_info.vx_req.user_resp_timer_val = TM_VX_T15_TIMER_CLNT_VALUE;

  /* Start T15 timer */
  (void)os_TimerStart(tm_vx_gen_info.sess_info.t15_timer,
                      TM_VX_T15_TIMER_INT_VALUE,
                      NULL);
      
  (void)tm_request_app_response(TM_PRTL_TYPE_1X_UP,
                                PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION,
                                &vx_app_req_info);  /*lint !e530 */  
}

  
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */



/*---------------------------------------------------------------------------

   This section contain code which needs to be compiled whether 
   FEATURE_GPSONE_VX_LCS_AGENT is defined or not.
   
----------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION tm_vx_start_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.

===========================================================================*/
prtl_start_sess_req_result_e_type tm_vx_start_sess_req_handler
(
  tm_sess_handle_type                     sess_handle,
  prtl_start_sess_req_param_s_type        *start_sess_req_param,
  tm_sess_req_start_param_s_type          *actions
)
{
  boolean                              retVal = FALSE;
  tm_1x_up_is801_drv_req_param_u_type  req_param;

#ifdef FEATURE_GPSONE_VX_LCS_AGENT

  tm_vx_app_auth_e_type                auth_check = TM_VX_APP_AUTH_ERROR;  

  if(start_sess_req_param == NULL)
  {
    MSG_ERROR("Null start_sess_req_param", 0, 0, 0);
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  if( (start_sess_req_param->client_type <= PDSM_CLIENT_TYPE_NONE)
    ||(start_sess_req_param->client_type >= PDSM_CLIENT_TYPE_MAX))
  {
    MSG_ERROR("invalid client type", 0, 0, 0);
    return PRTL_START_SESS_REQ_NOT_ALLOWED;  
  }
    

  if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_DISABLED)
  {  
    auth_check = TM_VX_APP_AUTHED;
  }
  else
  {
    switch(tm_vx_start_sess_req_general_check(start_sess_req_param))
    {
      case TM_VX_GENERAL_CHECK_OK:
        auth_check = tm_vx_app_auth_required_check(start_sess_req_param);
        break;

      case TM_VX_GENERAL_CHECK_USER_CONSENT_REQUIRED:
        tm_vx_gen_info.sess_info.pending_mo_app_index = start_sess_req_param->client_type;
        tm_vx_handle_mo_over_ni_req(sess_handle, start_sess_req_param);      
        return PRTL_START_SESS_REQ_WAIT;      

      case TM_VX_GENERAL_CHECK_PARAMETER_ERROR:
      case TM_VX_GENERAL_CHECK_SERVICE_INTERACTION_ERROR:
      default:
        return PRTL_START_SESS_REQ_NOT_ALLOWED;
    }
  }

  if(auth_check == TM_VX_APP_AUTHED)
  {
    /* 
       Set wake_tm_core to false, so that is801 module can instruct TM-Core what to do immediately.
    */
    req_param.start_param.wake_tm_core         = FALSE;  
    req_param.start_param.actions              = actions;
    req_param.start_param.start_sess_req_param = start_sess_req_param;

    /* If Vx is enabled, and client is non-trusted, change PDE address to what we get from V1/V2 protocol */
    if( (tm_vx_gen_info.vx_config.vx_version != TM_VX_VERSION_DISABLED)
      &&(tm_vx_gen_info.app_info[start_sess_req_param->client_type].trusted != TRUE))
    {
      tm_vx_get_pde_info(&(req_param.start_param.start_sess_req_param->pd_option.lsinfo));
    }

    retVal = tm_1x_up_is801_drv_req(
                                     sess_handle, 
                                     TM_IS801_DRV_REQ_START,
                                     req_param
                                   );

    if(retVal == TRUE) /*lint !e506 !e731 */
    {
      return PRTL_START_SESS_REQ_ALLOWED;
    }
    else
    {
      tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
      return PRTL_START_SESS_REQ_NOT_ALLOWED;
    }  
  }
  else if(auth_check == TM_VX_APP_AUTH_REQUIRED)
  {
    /* Set app state to waiting to send sppreq */
    (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ);

    if(start_sess_req_param->pd_option.lsinfo.server_option == PDSM_SERVER_OPTION_USE_DEFAULT)
    {
      /* Use MPC server address / port stored in NV */
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_addr_type                  = TM_VX_MPC_ADR_TYPE_IPV4;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_adrs    = tm_vx_gen_info.vx_config.vx_mpc_adrs;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_port_id = tm_vx_gen_info.vx_config.vx_mpc_port_id;
    }
    else
    {
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_addr_type                  = TM_VX_MPC_ADR_TYPE_IPV4;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_adrs    = start_sess_req_param->pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs;
      tm_vx_gen_info.comm_info[TM_VX_COMM_TYPE_TCP].mpc_info.mpc_adrs.ipv4_adrs.mpc_port_id = start_sess_req_param->pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id;
    }

    /* Save request param for SPPReq/SPPResp/GPS session for later */
    tm_vx_save_start_sess_req_param(tm_vx_gen_info.sess_info.active_app_index, sess_handle, start_sess_req_param);

    if (start_sess_req_param->pd_option.operation_mode!=PDSM_SESSION_OPERATION_MSBASED)
    {
      retVal = tm_vx_ip_connect_to_mpc();

      if(retVal == TRUE) /*lint !e506 !e731 */
      {
        MSG_MED("Waiting for authentication", 0, 0, 0);
        return PRTL_START_SESS_REQ_WAIT;  
      }
      else
      {
        MSG_HIGH("Failure when connecting to MPC; will try to fallback to SA", 0, 0, 0);
        retVal = tm_vx_fallback_to_standalone(); 
        if(retVal == FALSE) /* if we could not fallback, then abort session */ 
        {  
          tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
          return PRTL_START_SESS_REQ_NOT_ALLOWED;
        }
        else
        {
          MSG_MED("Successfully called IS801_drv_req after MPC error",0,0,0);
          return PRTL_START_SESS_REQ_ALLOWED;
        }
      }
    }
    else
    {  
      /* Set up actions to continue MSB session */
      actions->op_req = TM_OP_REQ_AIDING_DATA_STATUS | TM_OP_REQ_LR;
      actions->op_mode = TM_SESS_OPERATION_MODE_MSB;
      actions->num_fixes = start_sess_req_param->pd_option.fix_rate.num_fixes;
      actions->tbf_ms = start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;
      actions->accuracy_threshold = start_sess_req_param->pd_qos.accuracy_threshold;
      actions->lr_qos = (uint32)start_sess_req_param->pd_qos.gps_session_timeout;
      actions->prm_qos = (uint32)start_sess_req_param->pd_qos.gps_session_timeout;

      actions->ppm_qos = 0;
      actions->prm_prq = 0;
      actions->dynamic_qos_enabled = TRUE;  /* enable dynamic QoS */
      actions->multi_report_msa = FALSE;  /* indicate multi MSA rept */
      actions->abort_curr_sess = FALSE;

      return PRTL_START_SESS_REQ_WAIT;
    }
  }  
  else
  {
    MSG_HIGH("Unknown error", 0, 0, 0);
    tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);    
    return PRTL_START_SESS_REQ_NOT_ALLOWED;  
  }
#else /* FEATURE_GPSONE_VX_LCS_AGENT */
  /* 
    Set wake_tm_core to false, so that is801 module can instruct TM-Core what to do immediately.
  */
  req_param.start_param.wake_tm_core         = FALSE;  
  req_param.start_param.actions              = actions;
  req_param.start_param.start_sess_req_param = start_sess_req_param;


  retVal = tm_1x_up_is801_drv_req(
                                   sess_handle, 
                                   TM_IS801_DRV_REQ_START,
                                   req_param
                                 );

  if(retVal == TRUE) /*lint !e506 !e731 */
  {
    return PRTL_START_SESS_REQ_ALLOWED;
  }
  else
  {
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }
  
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
}

/*===========================================================================

FUNCTION tm_vx_stop_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_vx_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
)
{
  tm_1x_up_is801_drv_req_param_u_type  req_param;

#ifdef FEATURE_GPSONE_VX_LCS_AGENT
  boolean ret_val = TRUE;

  if (sess_handle == TM_SESS_HANDLE_NONE)
  {
    return ret_val;
  }
  if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_DISABLED)
  {  
    req_param.stop_param.stop_reason = stop_reason;
    req_param.stop_param.stop_type   = stop_type;

    return tm_1x_up_is801_drv_req(
                             sess_handle, 
                             TM_IS801_DRV_REQ_STOP,
                             req_param            
                           );
  
  }
  else
  {    
    /* Stop is801 driver first */
    req_param.stop_param.stop_reason = stop_reason;
    req_param.stop_param.stop_type   = stop_type;
    ret_val = tm_1x_up_is801_drv_req(
                             sess_handle,
                             TM_IS801_DRV_REQ_STOP,
                             req_param);

    if( (stop_reason == TM_STOP_REASON_E911)
      ||(stop_reason == TM_STOP_REASON_USER_TERMINATED)
      ||(stop_reason == TM_STOP_REASON_ON_RESET_LOCATION_SERVICE))
    {
      tm_vx_handle_ms_abort_session_req(stop_reason);
    }

    return ret_val;
  }
#else /* FEATURE_GPSONE_VX_LCS_AGENT */  

  req_param.stop_param.stop_reason = stop_reason;
  req_param.stop_param.stop_type   = stop_type;

  return tm_1x_up_is801_drv_req(
                           sess_handle,
                           TM_IS801_DRV_REQ_STOP,
                           req_param);
  
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
}

/*===========================================================================
FUNCTION tm_vx_sess_req_data_handler

DESCRIPTION

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None.

===========================================================================*/
boolean tm_vx_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,  
  prtl_data_req_type   req_data
)

{
  return tm_1x_up_is801_drv_req_aiding_data(session_handle, req_data);
}

/*===========================================================================

FUNCTION tm_vx_sess_data_dload_needed

DESCRIPTION

RETURN VALUE
  Boolean

DEPENDENCIES
==============================================================================*/
boolean tm_vx_sess_data_dload_needed(tm_aiding_data_wishlist_s_type* aiding_data_status_ptr)
{
  boolean ret_val = FALSE;
  pdsm_pd_session_operation_e_type     operation_mode;
  sm_InternalAssistDataStatusStructType  *status_ptr = &(aiding_data_status_ptr->z_assist_data_status); 

  operation_mode = 
          tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].pd_req_info.pd_option.operation_mode;

  MSG_MED("Pos_Unc=%dm,Eph_validity=%d,Alm_validity=%d",
                              status_ptr->z_PosUnc.f_PosUnc,
                              (status_ptr->q_WishListMask & C_GPS_ASSIS_NAVMODL) ? 0 : 1, 
                              (status_ptr->q_WishListMask & C_GPS_ASSIS_ALM) ? 0 : 1);

  if (operation_mode == PDSM_SESSION_OPERATION_MSBASED)
  {
    MSG_MED("Process MS based only session ..",0,0,0);

    if ( (status_ptr->q_WishListMask & C_GPS_ASSIS_NAVMODL) ||
         (status_ptr->z_PosUnc.u_Valid == FALSE) ||
         (status_ptr->z_PosUnc.f_PosUnc >= TM_LR_THRESHOLD_NO_POS))
    {
      /* MS either needs valid Ephemeris or Pos Unc is too high, request data download.
      */
      MSG_MED("MS-Based data needed",0,0,0);
      ret_val = TRUE;
    }
  }

  return ret_val;
} /* end tm_vx_sess_data_dload_needed() */


/*===========================================================================
FUNCTION tm_vx_sess_info_handler

DESCRIPTION

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None.

===========================================================================*/
boolean tm_vx_sess_info_handler

(
  tm_sess_handle_type           session_handle,
  prtl_sess_info_e_type         sess_info_type,
  prtl_sess_info_param_u_type   *sess_info_param
)
{
  boolean	retVal = FALSE;

  MSG_HIGH("TM_VX: App state = %d, session_info_type %d", tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state, sess_info_type, 0);
           
  if ((sess_info_type==PRTL_SESS_INFO_STATUS) &&
	  (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state==TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ))
  {
    /* If MSB Data is not needed or Backoff throttle timers have not expired, fallback to standalone */
    if ((tm_vx_sess_data_dload_needed(&sess_info_param->aiding_data_status)==FALSE) ||
        (tm_1x_up_is801_drv_check_msb_throttling(TRUE)==TRUE))
    {
      MSG_HIGH("Throttle this connection to MPC, will try to fallback to SA",0,0,0);
      retVal = tm_vx_fallback_to_standalone(); 
      if(retVal == FALSE) /* if we could not fallback, then abort session */ 
      {  
        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
        return FALSE;
      }
      else
      {
        MSG_MED("Successfully called IS801_drv_req after MPC throttle",0,0,0);
        return TRUE;
      }
    }

    MSG_MED("No MPC throttling necessary...",0,0,0);
	  retVal = tm_vx_ip_connect_to_mpc();

    if(retVal == TRUE) /*lint !e506 !e731 */
    {
      MSG_MED("Waiting for authentication", 0, 0, 0);
      return TRUE;  
    }
    else
    {
      MSG_HIGH("Failure when connecting to MPC; will try to fallback to SA", 0, 0, 0);
      retVal = tm_vx_fallback_to_standalone(); 
      if(retVal == FALSE) /* if we could not fallback, then abort session */ 
      {  
        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
        return FALSE;
      }
      else
      {
        MSG_MED("Successfully called IS801_drv_req after MPC error",0,0,0);
        return TRUE;
      }
    }
  }
  else if ((sess_info_type == PRTL_SESS_INFO_LR) &&
           (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state != TM_VX_APP_STATE_INIT) &&
           (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].pd_req_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_MSBASED) &&
          (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].pd_req_info.client_id !=  PDSM_CLIENT_TYPE_NI) )
    
  {
    /* This is an MO MSB request. PE already produced a final fix before MPC connection is up.
       report the fix to PDAPI and Cancel MPC connection and clean up TM-Core*/
    tm_1x_up_is801_drv_handle_v2_lr_resp_msbased(&sess_info_param->lr_data);
    tm_vx_reinit(TRUE, TM_STOP_REASON_COMPLETED);

#if defined(FEATURE_CALL_THROTTLE)
    tm_1x_up_mpc_set_throttling_backoff_time();
#endif /* FEATURE_CALL_THROTTLE*/

    return TRUE;
  }
  else if (((tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state == TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ) ||
            (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].app_state == TM_VX_APP_STATE_WAIT_TO_RECV_SPPRESP)) &&
           (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].pd_req_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_MSBASED) &&
          (tm_vx_gen_info.app_info[tm_vx_gen_info.sess_info.active_app_index].pd_req_info.client_id !=  PDSM_CLIENT_TYPE_NI) ) 
  {
    /* If MPC is still in connecting or waiting for PD result drop any messages */
    MSG_MED("TM_VX:Unhandled session info while MSB operation in V2 is still pending MPC connection",0,0,0);
    return TRUE;
  }
  else
  {
    return tm_1x_up_is801_drv_sess_info_handler(session_handle,
                                                sess_info_type,
                                                sess_info_param);
  }
}

/*===========================================================================
FUNCTION tm_vx_data_pipe_handler

DESCRIPTION

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_data_pipe_handler
(
  pd_comms_client_ipc_message_type   msg_type,  
  void                               *msg_data_ptr
)
{
#ifdef FEATURE_GPSONE_VX_LCS_AGENT    

  boolean result;
  uint32  *tr_id_ptr;

  if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_DISABLED)
  {  
    return tm_1x_up_is801_drv_data_pipe_handler(msg_type, msg_data_ptr);
  }
  else
  {  
    if(msg_data_ptr == NULL)
    {
      MSG_ERROR("Null msg_data_ptr", 0, 0, 0);
      return FALSE;
    }

    tr_id_ptr = (uint32 *)msg_data_ptr;

    switch(*tr_id_ptr & TM_1X_UP_TRANSACTION_ID_DISPATCH_MASK)
    {
      case TM_1X_UP_IS801_DRV_TRANSACTION_ID_BASE:
        result = tm_1x_up_is801_drv_data_pipe_handler(msg_type, msg_data_ptr);
      break;
      
      case TM_1X_UP_VX_TRANSACTION_ID_BASE:
        result = tm_vx_handle_pdcomm_data(msg_type, msg_data_ptr);
      break;
      
      default:
        MSG_ERROR("Invalid transaction id received", 0, 0, 0);
        result = FALSE;
      break;
    }
  }

  if(result == FALSE)
  {
    MSG_HIGH("Handling T7 timer expiry for V2 MO; calling is801 driver to proceed with SA session", 0, 0, 0);

    /* first try to get the SA fix on its way */
    result = tm_vx_fallback_to_standalone(); 

    if(result == FALSE) /* if we could not fallback, then abort session */
    {
      tm_vx_reinit(TRUE, TM_STOP_REASON_GENERAL_ERROR);
    }
    else
    {
       MSG_MED("Successfully called IS801_drv_req after T7 timer expiry",0,0,0);
    }
  }

  return result;

#else /* FEATURE_GPSONE_VX_LCS_AGENT */  

  return tm_1x_up_is801_drv_data_pipe_handler(msg_type, msg_data_ptr);

#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
}

/*===========================================================================
FUNCTION tm_vx_timer_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_vx_timer_cb_handler
(  
  void *timer_data
)
{
#ifdef FEATURE_GPSONE_VX_LCS_AGENT

  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;

  if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_DISABLED)
  {  
    tm_1x_up_is801_drv_timer_handler(timer_data);
  }
  else
  {  
    if(timer_param != NULL)
    {
      switch(timer_param->q_TimerId & TM_1X_UP_TIMER_ID_DISPATCH_MASK)
      { 
        case TM_1X_UP_IS801_DRV_TIMER_ID_BASE:
          tm_1x_up_is801_drv_timer_handler(timer_data);
        break;

        case TM_1X_UP_VX_TIMER_ID_BASE:
          tm_vx_timer_handler(timer_param);
        break;

        default:
          MSG_ERROR("Recv'd unknown timer expiry event", 0, 0, 0);
        break;
      }
    }
    else
    {
      MSG_ERROR("Can not dispatch null timer msg", 0, 0, 0);  
    }  
  }

  return;
  
#else /* FEATURE_GPSONE_VX_LCS_AGENT */  

  tm_1x_up_is801_drv_timer_handler(timer_data);

  return;
    
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */   
}

/*===========================================================================
FUNCTION tm_vx_handle_query_vx_config

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
boolean tm_vx_handle_query_vx_config
(
  pdsm_client_type_e_type               client_type
)
{
  pdsm_lcs_info_s_type event_info;

  event_info.lcs_cb_cmd_type = PDSM_LCS_CMD_TYPE_VX_CONFIG;
  
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
  
  event_info.lcs_info.vx_config.vxVersion = (pdsm_lcs_agent_version_e_type)tm_vx_gen_info.vx_config.vx_version;
  event_info.lcs_info.vx_config.trustedApp = tm_vx_app_trustness_setting(client_type);
  
#else

  event_info.lcs_info.vx_config.vxVersion = PDSM_LCS_AGENT_DISABLED;

#endif

  (void) tm_request_app_response(
                          TM_PRTL_TYPE_1X_UP,
                          PDSM_LCS_EVENT_CLIENT_VX_CONFIG,
                          &event_info
                         );

  return TRUE;
}


/*===========================================================================
FUNCTION tm_vx_handle_set_vx_config

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
boolean tm_vx_handle_set_vx_config
(
  pdsm_lcs_agent_set_vx_config_s_type *vx_config_info_ptr
)
{
  pdsm_lcs_info_s_type event_info;

  event_info.lcs_cb_cmd_type = PDSM_LCS_CMD_TYPE_SET_VX_CONFIG;

#ifdef FEATURE_GPSONE_VX_LCS_AGENT

  // change run-time config
  if (vx_config_info_ptr->vxVersion == PDSM_LCS_AGENT_DISABLED ||
      vx_config_info_ptr->vxVersion == PDSM_LCS_AGENT_VERSION_V1_ONLY ||
      vx_config_info_ptr->vxVersion == PDSM_LCS_AGENT_VERSION_V2_ONLY)
  {
    tm_vx_gen_info.vx_config.vx_version = (tm_vx_version_e_type)vx_config_info_ptr->vxVersion;
  }
  else
  {
    MSG_ERROR("Invalid vxVersion for SetVxConfig: %d", vx_config_info_ptr->vxVersion, 0, 0);
  }

  // return current version info
  event_info.lcs_info.set_vx_config.vxVersion = 
    (pdsm_lcs_agent_version_e_type)tm_vx_gen_info.vx_config.vx_version;

#else

  event_info.lcs_info.set_vx_config.vxVersion = PDSM_LCS_AGENT_DISABLED;

#endif

  (void) tm_request_app_response(
                          TM_PRTL_TYPE_1X_UP,
                          PDSM_LCS_EVENT_CLIENT_SET_VX_CONFIG,
                          &event_info
                         );

  return TRUE;
}


/*===========================================================================
FUNCTION tm_vx_event_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
boolean tm_vx_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *event_payload
)
{
  boolean ret_val = TRUE;
  
  if(event_payload == NULL)
  {
    MSG_ERROR("null event_payload pointer", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_GPSONE_VX_LCS_AGENT

  if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_DISABLED)
  {   
    if( (event_type == PRTL_EVENT_TYPE_PHONE) ||
        (event_type == PRTL_EVENT_TYPE_PA) )
    {
      tm_1x_up_is801_drv_event_handler(event_type, event_payload);
      ret_val = TRUE;    
    }
    else
    {
      if( (event_type == PRTL_EVENT_TYPE_LCS)
        &&(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_QUERY_CONFIG))
      {
        ret_val = tm_vx_handle_query_vx_config(event_payload->lcs_event_payload.lcs_event_payload.client_type);
      }
      else if( (event_type == PRTL_EVENT_TYPE_LCS)
        &&(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_SET_CONFIG))
      {
        ret_val = tm_vx_handle_set_vx_config(&(event_payload->lcs_event_payload.lcs_event_payload.vx_config_payload));
      }
      else
      {
        ret_val = FALSE;
      }
    }     
  }
  else
  {
    if(event_type == PRTL_EVENT_TYPE_PHONE)
    {
      /* For phone events, pass it to is801 session driver module as well */
      tm_1x_up_is801_drv_event_handler(event_type, event_payload);
      
      switch(event_payload->phone_event_payload.event)
      {
        case TM_PHONE_EVENT_IN_1X_COVERAGE:
          (void)tm_vx_sms_connect();
        break;
        
        case TM_PHONE_EVENT_IN_UMTS_COVERAGE:
          MSG_HIGH("Abort Vx session in UMTS system", 0, 0, 0);
          tm_vx_reinit(TRUE, TM_STOP_REASON_GENERAL_ERROR);
        break;

        default:
        break;
      }
      ret_val = TRUE;
    }
    else if(event_type == PRTL_EVENT_TYPE_LCS)
    {
      if(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_CLINET_INIT)
      {
        ret_val = tm_vx_handle_client_init(&(event_payload->lcs_event_payload.lcs_event_payload.clnt_init_payload));
      }
      else if(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_CLINET_RELEASE)
      {
        ret_val = tm_vx_handle_client_release(&(event_payload->lcs_event_payload.lcs_event_payload.clnt_release_payload));
      }      
      else if(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_RESP)
      {
        tm_vx_handle_app_response(event_payload->lcs_event_payload.lcs_event_payload.clnt_resp_payload.resp);
        ret_val = TRUE;
      }
      else if(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_QUERY_CONFIG)
      {
        ret_val = tm_vx_handle_query_vx_config(event_payload->lcs_event_payload.lcs_event_payload.client_type);
      }
      else if(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_SET_CONFIG)
      {
        ret_val = tm_vx_handle_set_vx_config(&(event_payload->lcs_event_payload.lcs_event_payload.vx_config_payload));
      }
    }
    else if(event_type == PRTL_EVENT_TYPE_PD)
    {
      tm_vx_ni_handle_pd_event(event_payload->pd_event_payload.pd_event);
      ret_val = TRUE;
    }
    else if(event_type == PRTL_EVENT_TYPE_PA)
    {
      /* For PA events, pass it to is801 session driver module as well */
      tm_1x_up_is801_drv_event_handler(event_type, event_payload);

      /* Handle PA paramter changes here */
      if(event_payload->pa_event_payload.pa_event == PDSM_PA_1X_MPC_NETWORK_ADRS)
      {
        if(event_payload->pa_event_payload.pa_info_ptr != NULL)
        {
          tm_vx_gen_info.vx_config.vx_mpc_adrs = event_payload->pa_event_payload.pa_info_ptr->server_address.server_adrs.server_adrs_v4.server_adrs;
          tm_vx_gen_info.vx_config.vx_mpc_port_id  = event_payload->pa_event_payload.pa_info_ptr->server_address.server_adrs.server_adrs_v4.port_id;
          MSG_MED("Updated MPC address %x and port %d", tm_vx_gen_info.vx_config.vx_mpc_adrs, tm_vx_gen_info.vx_config.vx_mpc_port_id, 0);
        }
      }
    }    
  }
  if (event_type == PRTL_EVENT_TYPE_RUIM_NV_READ)
  {
     /* For RUIM events, pass it to is801 session driver module as well for IMSI fields*/
     tm_1x_up_is801_drv_event_handler(event_type, event_payload);

#ifdef FEATURE_UIM_SUPPORT_LBS
     if (event_payload->ruim_read_nv_payload.ruim_item_group == PRTL_EVENT_RUIM_VX_NV_READ) {
         (void)tm_vx_ruim_read_complete(event_payload);  
     }
#endif /*FEATURE_UIM_SUPPORT_LBS*/
     ret_val = TRUE;
  }
  return ret_val;
  
#else /* FEATURE_GPSONE_VX_LCS_AGENT */  

  if( (event_type == PRTL_EVENT_TYPE_PHONE)
    ||(event_type == PRTL_EVENT_TYPE_PA))
  {
    tm_1x_up_is801_drv_event_handler(event_type, event_payload);
    ret_val = TRUE;    
  }
  else if( (event_type == PRTL_EVENT_TYPE_LCS)
         &&(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_QUERY_CONFIG))
  {
    ret_val = tm_vx_handle_query_vx_config(event_payload->lcs_event_payload.lcs_event_payload.client_type);
  }
  else if( (event_type == PRTL_EVENT_TYPE_LCS)
    &&(event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_SET_CONFIG))
  {
    ret_val = tm_vx_handle_set_vx_config(&(event_payload->lcs_event_payload.lcs_event_payload.vx_config_payload));
  }
  else
  {
    ret_val = FALSE;
  }
  
  return ret_val;  
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */   
}

/*===========================================================================
FUNCTION tm_vx_query_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
boolean tm_vx_query_handler
(
  prtl_query_e_type         query_type,
  prtl_ans_u_type           *answer_ptr
)
{
  boolean ret_val = TRUE;
  
  if(answer_ptr == NULL)
  {
    MSG_ERROR("null answer_ptr pointer", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_GPSONE_VX_LCS_AGENT

  if(query_type == PRTL_QUERY_SMART_MSB)
  {
    if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_DISABLED)
    {
      answer_ptr->ans_smart_msb = PRTL_ANS_SMART_MSB_ALLOWED;
    }
    else
    {
      answer_ptr->ans_smart_msb = PRTL_ANS_SMART_MSB_ALLOWED;  
    }
  }
  
#else /* FEATURE_GPSONE_VX_LCS_AGENT */ 

  if(query_type == PRTL_QUERY_SMART_MSB)
  {
    answer_ptr->ans_smart_msb = PRTL_ANS_SMART_MSB_ALLOWED;
  }

#endif /* FEATURE_GPSONE_VX_LCS_AGENT */   

  return ret_val;
}


/*===========================================================================
FUNCTION: tm_vx_init_prtl_iface_tbl

DESCRIPTION:

PARAMETERS:

===========================================================================*/
void tm_vx_init_prtl_iface_tbl(void)
{  
  tm_vx_prtl_iface_table.start_sess_req_fp   = tm_vx_start_sess_req_handler;   
  tm_vx_prtl_iface_table.stop_sess_req_fp    = tm_vx_stop_sess_req_handler;  
  tm_vx_prtl_iface_table.sess_req_data_fp    = tm_vx_sess_req_data_handler;  
  tm_vx_prtl_iface_table.sess_info_fp        = tm_vx_sess_info_handler;   
  tm_vx_prtl_iface_table.data_pipe_handle_fp = tm_vx_data_pipe_handler;   
  tm_vx_prtl_iface_table.timer_cb_fp         = tm_vx_timer_cb_handler;  
  tm_vx_prtl_iface_table.event_cb_fp         = tm_vx_event_cb_handler;
  tm_vx_prtl_iface_table.general_query_fp    = tm_vx_query_handler;
  tm_vx_prtl_iface_table.gm_event_handle_fp  = NULL;
}

/*===========================================================================

FUNCTION tm_vx_prtl_init

DESCRIPTION
  This function is called by TM-Core to initialize Vx module
  
DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
void tm_vx_prtl_init(void)
{
  MSG_MED("tm_vx_prtl_init", 0, 0, 0);

#ifdef FEATURE_GPSONE_VX_LCS_AGENT  
  /* Initialize Vx internal Data Structure, NV settings, prtl callback table, ... etc */  
  tm_vx_init();
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */

  /* Initialize TM-Core / Vx function call back table */
  tm_vx_init_prtl_iface_tbl();   
  
  /* Call IS801 drv init function */      
  tm_1x_up_is801_drv_init(); 

  /* Set up callback functions in TM-Core */
  (void)tm_prtl_reg(TM_PRTL_TYPE_1X_UP, &tm_vx_prtl_iface_table);
}

/*===========================================================================
FUNCTION tm_vx_pd_session_done

DESCRIPTION
  This function is called by is801 driver module to provide fix status (Pos info).
  Pos info is used for reporting position to MPC for V2 NI sessions.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_vx_pd_info_available
(  
  pdsm_pd_info_s_type  *pd_info_ptr,
  qword                qwtime,
  pdsm_pd_end_e_type   end_status
)
{
#ifdef FEATURE_GPSONE_VX_LCS_AGENT

  if(tm_vx_gen_info.vx_config.vx_version ==  TM_VX_VERSION_V1_ONLY)
  {
    /* Perform V1 specific call flow when getting pos info */  
    tm_v1_handle_pd_info(pd_info_ptr, qwtime, end_status);
  }
  else if(tm_vx_gen_info.vx_config.vx_version ==  TM_VX_VERSION_V2_ONLY)
  {
    /* Perform V2 specific call flow when getting pos info */  
    tm_v2_handle_pd_info(pd_info_ptr, qwtime, end_status);
  }
  else
  {
    MSG_MED("Ignore pd ifno", 0, 0, 0);
  }  
#endif

  return;    
}
