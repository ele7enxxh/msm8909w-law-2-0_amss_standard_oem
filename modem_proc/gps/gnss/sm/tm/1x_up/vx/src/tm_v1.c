/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM - V1 module

General Description
  This file contains implementations for V1 protocol
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/src/tm_v1.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/16   kpn     Act on abort_reason specifically while aborting a V2 session
05/23/08    cl     Set active index for cell/sector, cached pos request
02/05/07    cl     Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_GPSONE_VX_LCS_AGENT

#include "msg.h"
#include "tm_vx.h"
#include "tm_vx_i.h"
#include "tm_vx_ni_client.h"
#include "tm_1x_up_is801_drv_iface.h"

/*=========================================================================
FUNCTION
  tm_v2_mo_sms_cell_sector_init

DESCRIPTION
  This function is used to extract cell/sector information from MS.

DEPENDENCIES
  NA
RETURN VALUE
  The status of the message delivery.

SIDE EFFECTS
  None

=========================================================================*/
void tm_v1_mo_sms_cell_sector_init
(
  tm_v1_mo_sms_s_type *sms_ptr
)
{
  mcc_pd_bs_info_type bstation_info;
  uint16 gps_week;
  uint32 gps_ms;
  uint32 gps_sec;

  if(sms_ptr == NULL)
  {
    MSG_ERROR("Null sms_ptr", 0, 0, 0);
    return;
  }
  
  memset((void *)&bstation_info, 0, sizeof(bstation_info));
  
  mcc_pd_get_bs_info(&bstation_info);
  
  sms_ptr->sid = LITTLEEND_TO_BIGEND_WORD(bstation_info.sid);
  sms_ptr->nid = LITTLEEND_TO_BIGEND_WORD(bstation_info.nid);
  sms_ptr->base_id = LITTLEEND_TO_BIGEND_WORD(bstation_info.base_id);
  sms_ptr->base_lat[2] = (uint8)(bstation_info.base_lat&0xff) ;
  sms_ptr->base_lat[1] = (uint8)((bstation_info.base_lat>>8)&0xff) ;
  sms_ptr->base_lat[0] = (uint8)((bstation_info.base_lat>>16)&0xff) ;
  sms_ptr->base_long[2] = (uint8)(bstation_info.base_long&0xff) ;
  sms_ptr->base_long[1] = (uint8)((bstation_info.base_long>>8)&0xff) ;
  sms_ptr->base_long[0] = (uint8)((bstation_info.base_long>>16)&0xff) ;

  /* Note - The timestamp from bs_info is in a 64 bit format.
  ** This needs to be converted to the format specified in
  ** gps Weeks and days. The implementation for ConvertTimestamp
  ** left for the developer.
  */
  tm_vx_gps_week_ms(bstation_info.ts, &gps_week, &gps_ms);
  sms_ptr->weeknumber = LITTLEEND_TO_BIGEND_WORD(gps_week);
  gps_sec = gps_ms/1000;
  sms_ptr->sec[2] = ((uint8) gps_sec) & 0xff;
  gps_sec >>= 8;
  sms_ptr->sec[1] = ((uint8) gps_sec) & 0xff;
  gps_sec >>= 8;
  sms_ptr->sec[0] = ((uint8) gps_sec) & 0xff;
} /* End v1_mo_sms_cell_sector_init() */

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
boolean tm_v1_send_mo_sms
(
  tm_vx_msg_v1_consent_indicator_e_type consent_ind,
  tm_vx_int_v1_mt_sms_s_type            *mt_sms_ptr
)
{
  tm_v1_mo_sms_s_type v1_mo_sms;

  if(mt_sms_ptr == NULL)
  {
    MSG_ERROR("Null mt_sms_ptr", 0, 0, 0);
    return FALSE;
  }
  
  v1_mo_sms.version       = 1;
  v1_mo_sms.max_version   = 1;
  v1_mo_sms.consent_ind   = consent_ind;

  if(mt_sms_ptr->position_tech_ind != TM_VX_MT_SMS_POS_TECH_IND_IS801)
  {
    v1_mo_sms.gpsone_status = 0;
  }
  else
  {
    if( (consent_ind == TM_VX_MSG_V1_CONSENT_IND_CONSENT_GIVEN)
      ||(consent_ind == TM_VX_MSG_V1_CONSENT_IND_CONSENT_NOT_NEEDED))
    {
      v1_mo_sms.gpsone_status = 0;
    }
    else
    {
      v1_mo_sms.gpsone_status = 1;
    }
  }

  v1_mo_sms.correlation_id = mt_sms_ptr->correlation_id;

  tm_v1_mo_sms_cell_sector_init(&v1_mo_sms);

  if( (consent_ind == TM_VX_MSG_V1_CONSENT_IND_CONSENT_GIVEN)
    ||(consent_ind == TM_VX_MSG_V1_CONSENT_IND_CONSENT_NOT_NEEDED))
  {
    return tm_vx_send_mo_sms((uint8 *)&v1_mo_sms, sizeof(tm_v1_mo_sms_s_type));
  }
  else
  {
    /* Don't report NID / BASE_ID / BASE_LAT / BASE_LONG / TIME STAMP which are 15 bytes total */
    return tm_vx_send_mo_sms((uint8 *)&v1_mo_sms, (sizeof(tm_v1_mo_sms_s_type) - 15));  
  }
}

/*===========================================================================
FUNCTION tm_v1_ni_pos_req_allowed

DESCRIPTION
  This function is called to check if the phone can accept the incoming 
  NI positioning request

  1. Phone state check
  2. V1 MT Req paramter check  

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_v1_ni_pos_req_allowed(void)
{
  pdsm_pd_cmd_err_e_type cmd_error;

  if(tm_prtl_check_mt_up_allowed(TM_PRTL_TYPE_1X_UP) != TRUE)
  {
    MSG_HIGH("MT session not allowed", 0, 0, 0);
    return FALSE;
  }

  /* Check if there is already an Vx outstanding session */
  if(tm_vx_gen_info.sess_info.active_app_index != -1)
  {
    /* Allow V1 NI cell/sector/cached positioning while there is MO session */
    if( (tm_vx_gen_info.sess_info.active_app_index != PDSM_CLIENT_TYPE_NI)
      &&( (tm_vx_gen_info.sess_info.incoming_v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR)
        ||(tm_vx_gen_info.sess_info.incoming_v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CACHED)))
    {
      MSG_HIGH("Got NI cell/sector request while there is an active MO session", 0, 0, 0);
      return TRUE;
    }
    else
    {
      MSG_HIGH("Got NI Request while there is an active V1 session", 0, 0, 0);
      return FALSE;
    }
  }
  
  cmd_error = tm_core_get_pos_para_check(NULL);
  
  if( (cmd_error != PDSM_PD_CMD_ERR_NOERR)
    &&(cmd_error != PDSM_PD_CMD_ERR_OTHER))
  {
    MSG_ERROR("TM-Core does not allow NI session", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_v1_set_pde_info

DESCRIPTION
  This function sets the correct PDE address information for V1/V2 protocol
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void tm_v1_set_pde_info(uint8 pde_addr_source)
{
  int  active_app_index;

  active_app_index = tm_vx_gen_info.sess_info.active_app_index;
  
  if((active_app_index > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_index < (int)PDSM_CLIENT_TYPE_MAX))
  {    
    if(pde_addr_source == TM_V1_PDE_ADDR_SOURCE_SPPRESP)
    {
      tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
      tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs = tm_vx_gen_info.sess_info.v1_sppresp.pde_ip_adrs;
      tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id     = tm_vx_gen_info.sess_info.v1_sppresp.pde_port_number;
    }
    else if(pde_addr_source == TM_V1_PDE_ADDR_SOURCE_MT_SMS)
    {
      tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
      tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs = tm_vx_gen_info.sess_info.v1_mt_sms.pde_id;
      tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id     = tm_vx_gen_info.sess_info.v1_mt_sms.pde_port;
    }
    else
    {
      MSG_ERROR("Unknown PDE addr source", 0, 0 ,0);
    }
  }
  else
  {
    MSG_ERROR("No active client", 0, 0 ,0);
  }
}

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
void tm_v1_handle_pd_info
(  
  pdsm_pd_info_s_type  *pd_info_ptr,
  qword                qwtime,
  pdsm_pd_end_e_type   end_status
)
{
  /* Single shot fix done */
   MSG_MED("V1 handle PD info", 0, 0, 0);
   tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);  
}

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
void tm_v1_handle_sppresp(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  tm_1x_up_is801_drv_req_param_u_type  req_param;
  boolean                              ret_val;
  pdsm_pd_info_s_type                  pos_info;

  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {             
    switch(tm_vx_gen_info.sess_info.v1_sppresp.pos_status_ind)
    {
      case TM_V1_SPPRESP_POS_STATUS_IND_GPSONE:
      {
        MSG_MED("Proceed with gpsOne session", 0, 0, 0);


        /* Set PDE IP address */
        tm_v1_set_pde_info(TM_V1_PDE_ADDR_SOURCE_SPPRESP);
          
        /* 
          Now kick of gpsOne session by setting wake_tm_core flag to TRUE
          which will make is801 driver module to use continue to wake up TM-Core (TM-Core is in wait state )
        */
        req_param.start_param.wake_tm_core = TRUE; /*lint !e506 */
        req_param.start_param.actions      = NULL;

        req_param.start_param.start_sess_req_param = &(tm_vx_gen_info.app_info[active_app_idx].pd_req_info);

        ret_val = tm_1x_up_is801_drv_req(
                                          tm_vx_gen_info.sess_info.tm_core_sess_handle, 
                                          TM_IS801_DRV_REQ_START,
                                          req_param
                                        );
          
        if(ret_val == TRUE) /*lint !e506 !e731 */
        {
          MSG_MED("Waiting for PD result", 0, 0, 0);

          /* Update app state */          
          (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_PD_SESSION);
        }     
        else
        { 
          MSG_ERROR("Failed to start gpsOne session", 0, 0, 0);
          tm_vx_reinit(TRUE, TM_STOP_REASON_GENERAL_ERROR);
        }
        break;
      }
  
      case TM_V1_SPPRESP_POS_STATUS_IND_CACHED:
      case TM_V1_SPPRESP_POS_STATUS_IND_CELL:
      {
        pos_info.client_id = tm_vx_gen_info.app_info[active_app_idx].pd_req_info.client_id;

        MSG_MED("Proceed with cell/cached session", 0, 0, 0);
        
        if(tm_vx_gen_info.sess_info.v1_sppresp.pos_incl)
        {
          MSG_MED("position included", 0, 0, 0);
          pos_info.pd_info.pd_data.heading             = tm_vx_gen_info.sess_info.v1_sppresp.heading;
          pos_info.pd_info.pd_data.altitude            = tm_vx_gen_info.sess_info.v1_sppresp.height;
          pos_info.pd_info.pd_data.lat                 = tm_vx_gen_info.sess_info.v1_sppresp.lat;
          pos_info.pd_info.pd_data.loc_uncertainty_a   = tm_vx_gen_info.sess_info.v1_sppresp.loc_unc_a;
          pos_info.pd_info.pd_data.loc_uncertainty_ang = tm_vx_gen_info.sess_info.v1_sppresp.loc_unc_ang;
          pos_info.pd_info.pd_data.loc_uncertainty_p   = tm_vx_gen_info.sess_info.v1_sppresp.loc_unc_p;
          pos_info.pd_info.pd_data.loc_uncertainty_v   = tm_vx_gen_info.sess_info.v1_sppresp.loc_unc_v;
          pos_info.pd_info.pd_data.lon                 = tm_vx_gen_info.sess_info.v1_sppresp.lon;
          pos_info.pd_info.pd_data.velocity_hor        = tm_vx_gen_info.sess_info.v1_sppresp.velocity_hor;
          pos_info.pd_info.pd_data.velocity_ver        = tm_vx_gen_info.sess_info.v1_sppresp.velocity_ver;
        }
        else
        {
          pos_info.pd_info.pd_data.lat                 = 0;
          pos_info.pd_info.pd_data.loc_uncertainty_a   = 31;
          pos_info.pd_info.pd_data.loc_uncertainty_ang = 0;
          pos_info.pd_info.pd_data.loc_uncertainty_p   = 31;
          pos_info.pd_info.pd_data.lon                 = 0;
        }

        (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                                tm_vx_gen_info.sess_info.tm_core_sess_handle,
                                PDSM_PD_EVENT_POSITION,
                                &pos_info,
                                NULL);

        tm_vx_reinit(TRUE, TM_STOP_REASON_COMPLETED);

        break;
      }

      default:
      {
        /* MPC refused access */
        MSG_ERROR("MPC refused Access", 0, 0, 0);
        break;
      }
    }
  }
  else
  {
    MSG_ERROR("No Active application", 0, 0, 0);
  }
}

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
boolean tm_v1_check_incoming_mt_sms(void)
{
  boolean ret_val = TRUE;
  
  if(tm_vx_gen_info.sess_info.incoming_v1_mt_sms.notification_ind > 1)
  {
    ret_val = FALSE;
  }

  if(tm_vx_gen_info.sess_info.incoming_v1_mt_sms.position_tech_ind > 2)
  {
    ret_val = FALSE;
  }
  
  return ret_val;
}

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
void tm_v1_handle_incoming_mt_sms(void)
{  
  boolean                 ret_val = TRUE;
  pdsm_lcs_info_s_type    vx_app_req_info;
    

  /* 
    Make sure the phone can accept incoming Pos req 
    1. Phone state check
    2. V1 MT Req paramter check
  */
  ret_val = tm_v1_ni_pos_req_allowed();

  if(ret_val == TRUE)
  {
    /* 
      For V1 NI, once MT SMS is received, SPPReq/SPPResp is not needed for the gpsOne session
      Set authenticated flag to TRUE to achieve this.
      It will be reset to false in tm_vx_reinit()
    */
    tm_vx_gen_info.app_info[PDSM_CLIENT_TYPE_NI].authenticated = TRUE;

    /* Incoming V1 MT SMS validated, now it's safe to copy it to v1_mt_sms */
    memscpy((void *)&(tm_vx_gen_info.sess_info.v1_mt_sms), 
           sizeof(tm_vx_gen_info.sess_info.v1_mt_sms),
           (void *)&(tm_vx_gen_info.sess_info.incoming_v1_mt_sms),
           sizeof(tm_vx_gen_info.sess_info.incoming_v1_mt_sms));

    /* For cell/sector/cached, there is no need to update active_app_index */
    if(tm_vx_gen_info.sess_info.incoming_v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_IS801)
    {
      tm_vx_gen_info.sess_info.active_app_index = (int)PDSM_CLIENT_TYPE_NI;

      /* Set PDE IP address */
      tm_v1_set_pde_info(TM_V1_PDE_ADDR_SOURCE_MT_SMS);

      /* Update Correction ID to match MT SMS */
      tm_vx_gen_info.sess_info.correlation_id = tm_vx_gen_info.sess_info.v1_mt_sms.correlation_id;
    }
    else if( ( (tm_vx_gen_info.sess_info.v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR)
             ||(tm_vx_gen_info.sess_info.v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CACHED))
           &&(tm_vx_gen_info.sess_info.v1_mt_sms.notification_ind == (uint8)TM_VX_MSG_NOTIFY_AND_VERIFY_REQ))
    {
      tm_vx_gen_info.sess_info.active_app_index = (int)PDSM_CLIENT_TYPE_NI;

      /* Update Correction ID to match MT SMS */
      tm_vx_gen_info.sess_info.correlation_id = tm_vx_gen_info.sess_info.v1_mt_sms.correlation_id;
    }
    

    /* Check if user verification is needed */
    if(tm_vx_gen_info.sess_info.v1_mt_sms.notification_ind == (uint8)TM_VX_MSG_NOTIFY_AND_VERIFY_REQ)
    {
      /* 
       Both notification and verification are needed 
       Wait for user's consent to contine
      */
      vx_app_req_info.client_id            = 0;
      vx_app_req_info.lcs_cb_cmd_type      = PDSM_LCS_CMD_TYPE_VX_REQ;
      vx_app_req_info.lcs_info.vx_req.notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
      
      if(tm_vx_gen_info.sess_info.v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_IS801)
      {
        vx_app_req_info.lcs_info.vx_req.pos_tech = PDSM_LCS_POS_TECH_S_SHOT;
      }
      else if(tm_vx_gen_info.sess_info.v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR)
      {
        vx_app_req_info.lcs_info.vx_req.pos_tech = PDSM_LCS_POS_TECH_CELL_S_SHOT;
      }          
      else
      {
        vx_app_req_info.lcs_info.vx_req.pos_tech = PDSM_LCS_POS_TECH_CACHED;
      }
          
      vx_app_req_info.lcs_info.vx_req.num_fixes              = 1;
      vx_app_req_info.lcs_info.vx_req.tbf                    = 0;
      vx_app_req_info.lcs_info.vx_req.pos_mode               = PDSM_LCS_MS_ASSISTED_ONLY;
      vx_app_req_info.lcs_info.vx_req.encoding_scheme        = (pdsm_lcs_requester_id_encoding_scheme_e_type)0;
      
      vx_app_req_info.lcs_info.vx_req.requester_id.requester_id_length = 
	  	memscpy((void *)&(vx_app_req_info.lcs_info.vx_req.requester_id.requester_id[0]),
	  	     sizeof(vx_app_req_info.lcs_info.vx_req.requester_id.requester_id),
             (void *)&(tm_vx_gen_info.sess_info.v1_mt_sms.requester_id[0]),
             tm_vx_gen_info.sess_info.v1_mt_sms.requester_id_len);

      vx_app_req_info.lcs_info.vx_req.user_resp_timer_val = TM_VX_T15_TIMER_CLNT_VALUE;

      /* Only start T15 timer when user consent is needed */
      if(tm_vx_gen_info.sess_info.v1_mt_sms.notification_ind == (uint8)TM_VX_MSG_NOTIFY_AND_VERIFY_REQ)
      {
        /* Start T15 timer */
        (void)os_TimerStart(tm_vx_gen_info.sess_info.t15_timer,
                            TM_VX_T15_TIMER_INT_VALUE,
                            NULL);
        
        /* Update app state */          
        (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_USER_RESP);
      }
                
      (void)tm_request_app_response(TM_PRTL_TYPE_1X_UP,
                                    PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ,
                                    &vx_app_req_info);  /*lint !e530 */

      /* Resume call flow when getting users response (or timeout) */
    }
    else
    {      
      if(tm_vx_ni_client_sess_param_check() == TRUE)
      {
        MSG_MED("V1 NI session started successfully", 0, 0, 0);      
        
        (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_CONSENT_NOT_NEEDED,
                          &(tm_vx_gen_info.sess_info.v1_mt_sms));

        /* For cell/sector/cached NI, do not call tm_vx_reinit to affect potential on-going MO session */
        if(tm_vx_gen_info.sess_info.v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_IS801)
        {
          (void)tm_vx_ni_client_start_session();
        }
      }
      else
      {
        MSG_ERROR("Failed to start V1 NI session", 0, 0, 0);
        
        (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_MS_REFUSED,
                          &(tm_vx_gen_info.sess_info.v1_mt_sms));

        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
      }
    }
  }
  else
  {      
    /* MS can not accept the request, send out  MO SMS reject now */
    MSG_ERROR("NI pos request check failed", 0, 0, 0);

    (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_MS_REFUSED,
                      &(tm_vx_gen_info.sess_info.incoming_v1_mt_sms));
  }

  return;
}

/*===========================================================================
FUNCTION tm_v1_handle_app_response

DESCRIPTION
  This function is called to hanlde app response based on V1 protocol

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_v1_handle_app_response
(
  pdsm_lcs_agent_notification_resp_e_type  app_resp
)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  
  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {
    if(tm_vx_gen_info.app_info[active_app_idx].app_state != TM_VX_APP_STATE_WAIT_USER_RESP)
    {
      MSG_HIGH("V1 not waiting for user response", 0, 0, 0);
      return;
    }  
  }
  else
  {
    MSG_HIGH("Invalid app index", 0, 0, 0);
    return;    
  }

  if(app_resp == PDSM_LCS_NOTIFY_VERIFY_ACCEPT)
  {
    MSG_HIGH("NI session req accepted", 0, 0, 0);
      
    if(tm_vx_ni_client_sess_param_check() == TRUE)
    {
      MSG_MED("V1 NI session started successfully", 0, 0, 0);      
        
      /* Send out MO SMS first */
      (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_CONSENT_GIVEN, 
                        &(tm_vx_gen_info.sess_info.v1_mt_sms));

       /* For cell/sector/cached NI, do not call tm_vx_reinit to affect potential on-going MO session */
      if(tm_vx_gen_info.sess_info.v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_IS801)
      {
        (void)tm_vx_ni_client_start_session();
      }
      else
      {
        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
      }
    }
    else
    {
      MSG_ERROR("Failed to start V1 NI session", 0, 0, 0);
        
      (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_MS_REFUSED,
                        &(tm_vx_gen_info.sess_info.v1_mt_sms));

      tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
    }
  }
  else
  {
    MSG_HIGH("NI session req rejected (%d)", app_resp, 0, 0);

    /* Send out MO SMS and do proper clean-up */
    if(app_resp == PDSM_LCS_NOTIFY_VERIFY_DENY)
    {
      (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_CONSENT_NOT_GIVEN, 
                        &(tm_vx_gen_info.sess_info.v1_mt_sms));
    }
    else
    {
      (void) tm_v1_send_mo_sms(TM_VX_MSG_V1_CONSENT_IND_NO_RESP, 
                        &(tm_vx_gen_info.sess_info.v1_mt_sms));
    }

    tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
  }
}

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
void tm_v1_set_ni_param(void)
{  
  tm_vx_ni_session_info.pos_tech = (tm_vx_mt_sms_pos_tech_ind_e_type)tm_vx_gen_info.sess_info.v1_mt_sms.position_tech_ind;
    
  /* Save tracking information for later */
  tm_vx_ni_session_info.get_pos_param.client_type                   = PDSM_CLIENT_TYPE_NI;
  tm_vx_ni_session_info.get_pos_param.client_id                     = pdsmclient_get_client_id_map(PDSM_CLIENT_TYPE_NI);
  tm_vx_ni_session_info.get_pos_param.mt_initiator                  = MT_UP;  // NI for 1X UP
  tm_vx_ni_session_info.pd_option_param.class_id                    = 0;
    
  /* MPC address is set by V1 MT SMS already */
  tm_vx_ni_session_info.pd_option_param.lsinfo.server_option        = PDSM_SERVER_OPTION_USE_LOCAL;
    
  tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes             = 1;
  tm_vx_ni_session_info.pd_option_param.fix_rate.time_between_fixes_ms = 0;

  tm_vx_ni_session_info.pd_option_param.session = PDSM_PD_SESS_TYPE_NEW;

  tm_vx_ni_session_info.pd_option_param.operation_mode = PDSM_SESSION_OPERATION_MSASSISTED;
      
  tm_vx_ni_session_info.pd_qos_param.gps_session_timeout = TM_VX_DEFAULT_QOS;

  tm_vx_ni_session_info.pd_qos_param.accuracy_threshold = TM_VX_DEFAULT_ACCURACY_THRESHOLD;

  tm_vx_ni_session_info.get_pos_param.option_ptr = &(tm_vx_ni_session_info.pd_option_param);
  tm_vx_ni_session_info.get_pos_param.qos_ptr    = &(tm_vx_ni_session_info.pd_qos_param);

  tm_vx_ni_session_info.current_fix_count                           = 1;

  tm_vx_ni_session_info.sess_state = TM_VX_NI_CLIENT_SESS_STATE_INIT;  
  
  return;
}

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
void tm_v1_handle_ms_abort_session_req(const tm_sess_stop_reason_e_type abort_reason)
{
  MSG_HIGH("Aborting V1 session", 0, 0, 0);
  tm_vx_reinit(FALSE, abort_reason);
}

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
void tm_v1_handle_ms_mpc_failure(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  
  if(active_app_idx != (int)PDSM_CLIENT_TYPE_NI)
  {
    MSG_HIGH("Handling  MS-MPC expiry for V1 MO", 0, 0, 0);
    
    /* Clean up Vx state machine */
    tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);   
  }
  else
  {
    MSG_ERROR("Unexpected MS-MPC expiry", 0, 0, 0);        
  }
}
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
