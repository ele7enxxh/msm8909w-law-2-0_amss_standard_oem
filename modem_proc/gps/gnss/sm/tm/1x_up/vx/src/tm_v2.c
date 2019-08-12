/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM - V2 module

General Description
  This file contains implementations for V2 protocol
  
  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/src/tm_v2.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/16   kpn     Act on abort_reason specifically while aborting a V2 session
05/13/14   mj      Add featurization to fix SMART compile/link issues
2/5/07     cl     Initial Release
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

/*===========================================================================
FUNCTION tm_v2_ni_pos_req_allowed

DESCRIPTION
  This function is called to check if the phone can accept the incoming 
  NI positioning request

  1. Phone state check
  2. V2 MT Req paramter check  

DEPENDENCIES 
  None.

RETURN VALUE
  tm_vx_msg_v2_rej_reason_e_type
  
SIDE EFFECTS
 None

===========================================================================*/	
tm_vx_msg_v2_rej_reason_e_type tm_v2_ni_pos_req_allowed(void)
{
  tm_vx_msg_v2_rej_reason_e_type ret_val = TM_VX_MSG_V2_REJ_REASON_VERSION_NOT_SUPPORTED;

  if(tm_prtl_check_mt_up_allowed(TM_PRTL_TYPE_1X_UP) != TRUE)
  {
    MSG_HIGH("MT session not allowed", 0, 0, 0);
    return ret_val;
  }

  /* Check if there is already an outstanding session */
  if(tm_vx_gen_info.sess_info.active_app_index != -1)
  {
    MSG_ERROR("Got NI Request while there is an active Vx session", 0, 0, 0);
    ret_val = TM_VX_MSG_V2_REJ_REASON_PRIV_IS801_IN_PROGRESS;
    return ret_val;
  }  

  switch(tm_core_get_pos_para_check(NULL))
  {
    case PDSM_PD_CMD_ERR_BUSY_S:
      MSG_HIGH("Got NI Req while TM-core is busy", 0, 0, 0);
      ret_val = TM_VX_MSG_V2_REJ_REASON_PRIV_IS801_IN_PROGRESS;
    break;

    case PDSM_PD_CMD_ERR_GPS_LOCK_S:
      MSG_HIGH("GPS is locked", 0, 0, 0);
      ret_val = TM_VX_MSG_V2_REJ_REASON_PRIVACY;
    break;

    case PDSM_PD_CMD_ERR_STATE_S:
      MSG_HIGH("TM-Core in E911 state", 0, 0, 0);
      ret_val = TM_VX_MSG_V2_REJ_REASON_PRIV_IS801_IN_PROGRESS;
    break;

    case PDSM_PD_CMD_ERR_NOERR:
    case PDSM_PD_CMD_ERR_OTHER:
      ret_val = TM_VX_MSG_V2_REJ_REASON_NO_REJECT;
    break;

    default:
      MSG_HIGH("TM-Core does not allow NI", 0, 0, 0);
      ret_val = TM_VX_MSG_V2_REJ_REASON_PRIV_IS801_IN_PROGRESS;      
    break;
  }  
  
  return ret_val;
}

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
void tm_v2_mo_sms_cell_sector_init
(
  tm_v2_mo_sms_rej_s_type *sms_ptr
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
  
  memset((void *) &bstation_info, 0, sizeof(bstation_info));
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
} /* End v2_mo_sms_cell_sector_init() */

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
boolean tm_v2_send_mo_reject_sms
(
  tm_vx_msg_v2_rej_reason_e_type reject_reason,
  uint8                          corr_id
)
{
  tm_v2_mo_sms_rej_s_type v2_mo_rej_sms;

  MSG_HIGH("Sending V2 MO Reject with reason %d", reject_reason, 0, 0);
  
  v2_mo_rej_sms.version        = (uint8)tm_vx_gen_info.vx_config.vx_version;
  v2_mo_rej_sms.msg_type       = TM_V2_MO_SMS_MESSAGE_TYPE_REJECT;    
  /* Length does not include version, msg_type ,and length fields */  
  v2_mo_rej_sms.length         = sizeof(tm_v2_mo_sms_rej_s_type) - 3; 

  v2_mo_rej_sms.correlation_id = corr_id;
 
  v2_mo_rej_sms.rej_reason     = (uint8)reject_reason;  
  tm_v2_mo_sms_cell_sector_init(&v2_mo_rej_sms);

  return tm_vx_send_mo_sms((uint8 *)&v2_mo_rej_sms, sizeof(tm_v2_mo_sms_rej_s_type));
}

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
boolean tm_v2_send_mo_cancel_sms
(
  tm_vx_msg_v2_cancel_reason_e_type cancel_reason
)
{
  tm_v2_mo_sms_cancel_s_type v2_mo_cancel_sms;

  MSG_HIGH("Sending V2 MO Cancel with reason %d", cancel_reason, 0, 0);
  
  v2_mo_cancel_sms.version        = (uint8)tm_vx_gen_info.vx_config.vx_version;
  v2_mo_cancel_sms.msg_type       = TM_V2_MO_SMS_MESSAGE_TYPE_CANCEL;    
  /* Length does not include version, msg_type ,and length fields */  
  v2_mo_cancel_sms.length         = sizeof(tm_v2_mo_sms_cancel_s_type) - 3; 

  v2_mo_cancel_sms.correlation_id = tm_vx_gen_info.sess_info.correlation_id;
 
  v2_mo_cancel_sms.cancel_reason     = (uint8)cancel_reason;  

  return tm_vx_send_mo_sms((uint8 *)&v2_mo_cancel_sms, sizeof(tm_v2_mo_sms_cancel_s_type));
}

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
boolean tm_v2_check_incoming_mt_sms(void)
{
  boolean ret_val = TRUE;

  if(tm_vx_gen_info.sess_info.incoming_v2_mt_sms.message_type > 3)
  {
    ret_val = FALSE;
  }

  if(tm_vx_gen_info.sess_info.incoming_v2_mt_sms.notification_ind > 2)
  {
    ret_val = FALSE;
  }

  if(tm_vx_gen_info.sess_info.incoming_v2_mt_sms.position_tech_ind > 2)
  {
    ret_val = FALSE;
  }

  if(tm_vx_gen_info.sess_info.incoming_v2_mt_sms.is801_mode > 3)
  {
    ret_val = FALSE;
  }
  
  return ret_val;
}

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
void tm_v2_handle_incoming_mt_sms(void)
{
  tm_vx_msg_v2_rej_reason_e_type rej_reason;
  pdsm_lcs_info_s_type           vx_app_req_info;
    
  switch(tm_vx_gen_info.sess_info.incoming_v2_mt_sms.message_type)
  {
    case TM_V2_MT_SMS_MSG_TYPE_POS_REQUEST:
    {
      /* 
         Make sure the phone can accept incoming Pos req 
         1. Phone state check
         2. V2 MT Req paramter check
      */
      rej_reason = tm_v2_ni_pos_req_allowed();

      if(rej_reason == TM_VX_MSG_V2_REJ_REASON_NO_REJECT)
      {
        /* Now set active index to NI */
        tm_vx_gen_info.sess_info.active_app_index = (int)PDSM_CLIENT_TYPE_NI;
  
        /* Incoming V2 MT SMS validated, now it's safe to copy it to v2_mt_sms */
        memscpy((void *)&(tm_vx_gen_info.sess_info.v2_mt_sms), 
               sizeof(tm_vx_gen_info.sess_info.v2_mt_sms),
               (void *)&(tm_vx_gen_info.sess_info.incoming_v2_mt_sms),
               sizeof(tm_vx_gen_info.sess_info.incoming_v2_mt_sms));       

        /* Update Correction ID to match MT SMS */
        tm_vx_gen_info.sess_info.correlation_id = tm_vx_gen_info.sess_info.v2_mt_sms.correlation_id;

        /* If need to notify user */
        if(tm_vx_gen_info.sess_info.v2_mt_sms.notification_ind != (uint8)TM_VX_MSG_NOTIFY_AND_VERIFY_NOT_REQ)
        {
          vx_app_req_info.client_id            = 0;
          vx_app_req_info.lcs_cb_cmd_type      = PDSM_LCS_CMD_TYPE_VX_REQ;
          vx_app_req_info.lcs_info.vx_req.notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;

          if(tm_vx_gen_info.sess_info.v2_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_IS801)
          {
            if(tm_vx_gen_info.sess_info.v2_mt_sms.num_fixes > 1)
            {
              vx_app_req_info.lcs_info.vx_req.pos_tech = PDSM_LCS_POS_TECH_TRACK;
            }
            else
            {
              vx_app_req_info.lcs_info.vx_req.pos_tech = PDSM_LCS_POS_TECH_S_SHOT;
            }
          }
          else if(tm_vx_gen_info.sess_info.v2_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR)
          {
            if(tm_vx_gen_info.sess_info.v2_mt_sms.num_fixes > 1)
            {
              vx_app_req_info.lcs_info.vx_req.pos_tech = PDSM_LCS_POS_TECH_CELL_TRACK;
            }
            else
            {
              vx_app_req_info.lcs_info.vx_req.pos_tech = PDSM_LCS_POS_TECH_CELL_S_SHOT;
            }          
          }
          else
          {
            vx_app_req_info.lcs_info.vx_req.pos_tech = PDSM_LCS_POS_TECH_CACHED;
          }
   
          vx_app_req_info.lcs_info.vx_req.pos_qos_incl           = tm_vx_gen_info.sess_info.v2_mt_sms.qos_incl;
          vx_app_req_info.lcs_info.vx_req.pos_qos                = tm_vx_gen_info.sess_info.v2_mt_sms.qos;
          vx_app_req_info.lcs_info.vx_req.num_fixes              = tm_vx_gen_info.sess_info.v2_mt_sms.num_fixes;
          vx_app_req_info.lcs_info.vx_req.tbf                    = tm_vx_gen_info.sess_info.v2_mt_sms.tbf;
          vx_app_req_info.lcs_info.vx_req.pos_mode               = (pdsm_lcs_pos_mode_e_type)tm_vx_gen_info.sess_info.v2_mt_sms.is801_mode;
          vx_app_req_info.lcs_info.vx_req.encoding_scheme        = (pdsm_lcs_requester_id_encoding_scheme_e_type)tm_vx_gen_info.sess_info.v2_mt_sms.requester_id_encoding;

          vx_app_req_info.lcs_info.vx_req.requester_id.requester_id_length =
		  	memscpy((void *)&(vx_app_req_info.lcs_info.vx_req.requester_id.requester_id[0]),
		  	     sizeof(vx_app_req_info.lcs_info.vx_req.requester_id.requester_id),
                 (void *)&(tm_vx_gen_info.sess_info.v2_mt_sms.requester_id[0]),
                 tm_vx_gen_info.sess_info.v2_mt_sms.requester_id_len);

          vx_app_req_info.lcs_info.vx_req.user_resp_timer_val = TM_VX_T15_TIMER_CLNT_VALUE;

          /* Only start T15 timer when user consent is needed */
          if(tm_vx_gen_info.sess_info.v2_mt_sms.notification_ind == (uint8)TM_VX_MSG_NOTIFY_AND_VERIFY_REQ)
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

        }

        /* Check if user verification is needed */
        /* If needed, resume call flow when getting users response (or timeout) */        
        if(tm_vx_gen_info.sess_info.v2_mt_sms.notification_ind != (uint8)TM_VX_MSG_NOTIFY_AND_VERIFY_REQ)
        {
          if(tm_vx_ni_client_sess_param_check() == TRUE)
          {
            MSG_MED("V2 NI session started successfully", 0, 0, 0);      
                   
            (void)tm_vx_ni_client_start_session();
          }
          else
          {
            MSG_ERROR("Failed to start V2 NI session, send reject MO SMS to MPC", 0, 0, 0);
      
            (void) tm_v2_send_mo_reject_sms(TM_VX_MSG_V2_REJ_REASON_PRIV_IS801_IN_PROGRESS,
                                     tm_vx_gen_info.sess_info.v2_mt_sms.correlation_id);

            tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
          }
        }
      } /* rej_reason == TM_VX_MSG_V2_REJ_REASON_NO_REJECT */
      else
      {      
        /* MS can not accept the request, send out  MO SMS reject now.
           TM Vx clean up is not needed because it's rejected at incoming SMS level
        */
        MSG_HIGH("NI pos request check failed", 0, 0, 0);
        (void) tm_v2_send_mo_reject_sms(rej_reason, tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id);
      } /* rej_reason == TM_VX_MSG_V2_REJ_REASON_NO_REJECT */
    }  
    break;

    case TM_V2_MT_SMS_MSG_TYPE_NI_CANCEL:
    {
      MSG_MED("Processing NI Cancel MT SMS", 0, 0, 0);

      /* Two things need to be checked 
         1. If current session is NI
         2. If correlation ID matches
      */
      if( (tm_vx_gen_info.sess_info.active_app_index == (int)PDSM_CLIENT_TYPE_NI)
        &&(tm_vx_gen_info.sess_info.correlation_id == tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id))
      {
        MSG_MED("Cancelling V2 NI session", 0, 0, 0);
        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
      }
      else
      {
        MSG_HIGH("Ignore NI cancel MT SMS, active app idx = %d, correlation id = %d", 
                 tm_vx_gen_info.sess_info.active_app_index,
                 tm_vx_gen_info.sess_info.correlation_id, 0);
      }
    }
    break;

    case TM_V2_MT_SMS_MSG_TYPE_MO_CANCEL:
    {
      MSG_MED("Processing MO Cancel MT SMS", 0, 0, 0);
      
      /* Two things need to be checked 
         1. If current session is MO
         2. If correlation ID matches
      */
      if( (tm_vx_gen_info.sess_info.active_app_index != (int)PDSM_CLIENT_TYPE_NI)
        &&(tm_vx_gen_info.sess_info.correlation_id == tm_vx_gen_info.sess_info.incoming_v2_mt_sms.correlation_id))
      {
        MSG_MED("Cancelling V2 MO session", 0, 0, 0);
        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
      }
      else
      {
        MSG_HIGH("Ignore MO cancel MT SMS, active app idx = %d, correlation id = %d", 
                 tm_vx_gen_info.sess_info.active_app_index,
                 tm_vx_gen_info.sess_info.correlation_id, 0);
      }      
      
    }
    break;
    
    default:
    break;
  }
  return;
}

/*===========================================================================

FUNCTION tm_v2_set_pde_info

DESCRIPTION
  This function sets the correct PDE address information for V1/V2 protocol
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void tm_v2_set_pde_info(uint8 pde_addr_source)
{
  int  active_app_index;

  active_app_index = tm_vx_gen_info.sess_info.active_app_index;
  
  if((active_app_index > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_index < (int)PDSM_CLIENT_TYPE_MAX))
  {    
    if(pde_addr_source == TM_V2_MESSAGE_TYPE_SPPRESP)
    {
      if(tm_vx_gen_info.sess_info.v2_sppresp.adrs_encoding == 1)
      {       
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs = tm_vx_gen_info.sess_info.v2_sppresp.pde_ip_adrs;
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id     = tm_vx_gen_info.sess_info.v2_sppresp.pde_port_number;
      }
      else
      {
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;
        
        memscpy((void *)&(tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url[0]),
			   sizeof(tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url),
               (void *)&(tm_vx_gen_info.sess_info.v2_sppresp.url_adrs[0]),
               sizeof(tm_vx_gen_info.sess_info.v2_sppresp.url_adrs));
        
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url_length = tm_vx_gen_info.sess_info.v2_sppresp.adrs_length - 1;
      }
    }
#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF
    else if(pde_addr_source == TM_V2_MESSAGE_TYPE_POS_RPT_RSP)
    {
      if(tm_vx_gen_info.sess_info.v2_pos_rpt_resp.adrs_encoding == 1)
      {       
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs = tm_vx_gen_info.sess_info.v2_pos_rpt_resp.pde_ip_adrs;
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id     = tm_vx_gen_info.sess_info.v2_pos_rpt_resp.pde_port_number;
      }
      else
      {
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;
        
        memscpy((void *)&(tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url[0]),
			    sizeof(tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url),
               (void *)&(tm_vx_gen_info.sess_info.v2_pos_rpt_resp.url_adrs[0]),
                sizeof(tm_vx_gen_info.sess_info.v2_pos_rpt_resp.url_adrs));
        
        tm_vx_gen_info.app_info[active_app_index].pd_req_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url_length = tm_vx_gen_info.sess_info.v2_pos_rpt_resp.adrs_length - 1;
      }      
    }
#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */
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
FUNCTION tm_v2_handle_pd_info

DESCRIPTION
  This function performs V2 specific call flow when getting pd info
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_v2_handle_pd_info
(  
  pdsm_pd_info_s_type  *pd_info_ptr,
  qword                qwtime,
  pdsm_pd_end_e_type   end_status
)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;

  MSG_MED("V2 handle pd session done", 0, 0, 0);

  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {      
    /* For single shot sessions, clean up Vx state machine */
    if(tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.session != PDSM_PD_SESS_TYPE_TRACK_IND)
    {
      /* Single shot fix done */
      MSG_MED("Singleshot PD session done", 0, 0, 0);
      tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
    }     
    else /* This is a tracking session */
    {
      /* Report position back to MPC if it's a NI tracking session */
      if(active_app_idx == PDSM_CLIENT_TYPE_NI)
      {
        if(pd_info_ptr != NULL)
        {
          /* Save pd_info_ptr to tm_vx's own buffer to report pos when connection comes up */
          memscpy((void *)&(tm_vx_gen_info.sess_info.lr_resp), 
                   sizeof(tm_vx_gen_info.sess_info.lr_resp),
                   (void *)pd_info_ptr, sizeof(*pd_info_ptr));

          /* Save the position time stamp into LCS agent */
          qw_equ(tm_vx_gen_info.sess_info.lr_time,qwtime);
        }
        else
        {
          MSG_HIGH("Got null pd_info_ptr, force cell/sector report", 0, 0, 0);
          tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.lat = 0;
          tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.lon = 0;
        }      

        /* Try to bring up a TCP connection to MPC for position report */
        (void) tm_vx_ip_connect_to_mpc();

        /* Update app state */
        (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_TO_SEND_POS_RPT);
        
      } /* active_app_idx == PDSM_CLIENT_TYPE_NI */
      else
      {
        /* This is a MO tracking session, Update app state to initial state and wait for next MO request to come in */
        MSG_MED("PD session done", 0, 0, 0);
       (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_INIT);          
      } /* active_app_idx == PDSM_CLIENT_TYPE_NI */       
    }            
  }
}

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
void tm_v2_handle_sppresp(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  tm_1x_up_is801_drv_req_param_u_type  req_param;
  boolean                              ret_val;
  tm_sess_req_param_u_type             tm_core_sess_req_param;
    tm_sess_handle_type                sess_handle;

  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {
    if(tm_vx_gen_info.sess_info.correlation_id == tm_vx_gen_info.sess_info.v2_sppresp.cor_id)
    {              
      switch(tm_vx_gen_info.sess_info.v2_sppresp.pos_status_ind)
      {
        case TM_V2_SPPRESP_POS_STATUS_IND_GPSONE:
        {
          MSG_MED("Proceed with gpsOne session", 0, 0, 0);

          if(active_app_idx == PDSM_CLIENT_TYPE_NI)
          {
            tm_vx_ni_session_info.pos_tech = TM_VX_MT_SMS_POS_TECH_IND_IS801;
          }
          
          /* If this is a tracking session (auth_druation > 0), start auth duration timer */
          if(tm_vx_gen_info.app_info[active_app_idx].auth_duration > 0)
          {
            tm_vx_gen_info.app_info[active_app_idx].authenticated = TRUE; /*lint !e506 */

            (void)os_TimerStart(tm_vx_gen_info.sess_info.auth_duration_timer,
                                tm_vx_gen_info.app_info[active_app_idx].auth_duration * 1000,
                                NULL);
          }

          /* Set PDE IP address */
          tm_v2_set_pde_info(TM_V2_MESSAGE_TYPE_SPPRESP);
          
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
            (void)tm_vx_update_app_state(active_app_idx, TM_VX_APP_STATE_WAIT_PD_SESSION);
          }     
          else
          { 
            MSG_ERROR("Failed to start gpsOne session", 0, 0, 0);

            tm_v2_handle_ms_abort_session_req(TM_STOP_REASON_GENERAL_ERROR);
            
          }
          break;
        }

        case TM_V2_SPPRESP_POS_STATUS_IND_CELL:
        {
          if( (active_app_idx == PDSM_CLIENT_TYPE_NI)
            &&(tm_vx_gen_info.sess_info.v2_mt_sms.num_fixes > 1))
          {
            tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.session = PDSM_PD_SESS_TYPE_TRACK_IND;
          }
          else
          {
            tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.session = PDSM_PD_SESS_TYPE_NEW;
          }

          /* If this is a tracking session (auth_druation > 0), start auth duration timer */
          if(tm_vx_gen_info.app_info[active_app_idx].auth_duration > 0)
          {
            tm_vx_gen_info.app_info[active_app_idx].authenticated = TRUE; /*lint !e506 */

            (void)os_TimerStart(tm_vx_gen_info.sess_info.auth_duration_timer,
                                tm_vx_gen_info.app_info[active_app_idx].auth_duration * 1000,
                                NULL);
          }
          
          tm_vx_ni_session_info.pos_tech = TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR;
          tm_vx_ni_session_info.sess_state = TM_VX_NI_CLIENT_SESS_STATE_PD_DONE;
          
          /* Start report cell/sector info after T seconds from SPPResp */
          (void)os_TimerStart(tm_vx_ni_session_info.tracking_timer, 
                              tm_vx_ni_session_info.pd_option_param.fix_rate.time_between_fixes_ms,
                              NULL);

          /* Tell TM that a PD session is done */
          tm_core_sess_req_param.stop_param.stop_type   = TM_STOP_TYPE_SESSION;
          tm_core_sess_req_param.stop_param.stop_reason = TM_STOP_REASON_COMPLETED;
         
          (void)tm_sess_req(TM_PRTL_TYPE_1X_UP,
                            tm_vx_gen_info.sess_info.tm_core_sess_handle,
                            TM_SESS_REQ_STOP,
                            &tm_core_sess_req_param);
          
          break;
        }
        
        case TM_V2_SPPRESP_POS_STATUS_IND_CACHED:
        {  

          tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.session = PDSM_PD_SESS_TYPE_NEW;
              
          if(active_app_idx == PDSM_CLIENT_TYPE_NI)
          {          
            tm_vx_ni_session_info.pos_tech   = TM_VX_MT_SMS_POS_TECH_IND_CACHED;
            tm_vx_ni_session_info.sess_state = TM_VX_NI_CLIENT_SESS_STATE_PD_DONE;
          }         

          sess_handle = tm_vx_gen_info.sess_info.tm_core_sess_handle;
          
          tm_v2_handle_pd_info(NULL, 0, PDSM_PD_END_SESS_NONE);

          /* Tell TM that a PD session is done */
          tm_core_sess_req_param.stop_param.stop_type   = TM_STOP_TYPE_SESSION;
          tm_core_sess_req_param.stop_param.stop_reason = TM_STOP_REASON_COMPLETED;
          
          (void)tm_sess_req(TM_PRTL_TYPE_1X_UP,
                            sess_handle,
                            TM_SESS_REQ_STOP,
                            &tm_core_sess_req_param);
          
          
          break;
        }       

        default:
        {
          /* MPC refused access */
          MSG_ERROR("MPC refused Access %d", tm_vx_gen_info.sess_info.v2_sppresp.pos_status_ind, 0, 0);

          if(active_app_idx == (int)PDSM_CLIENT_TYPE_NI)
          {
            MSG_MED("Handling PosStatusInd for V2 NI", 0, 0, 0);

            /* Close MPC Connection */
            (void)tm_vx_ip_disconnect_from_mpc(FALSE);

            /* Clean up Vx state machine */      
            tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);
          }
          else
          {
            /* try to fallback to SA */
            ret_val = tm_vx_fallback_to_standalone(); 

            /* Close MPC Connection */
            (void)tm_vx_ip_disconnect_from_mpc(FALSE);     
			
#if defined(FEATURE_CALL_THROTTLE)
            tm_1x_up_mpc_set_throttling_backoff_time();
#endif /* FEATURE_CALL_THROTTLE*/
    
            if(ret_val == TRUE) /*lint !e506 !e731 */
            {
              MSG_MED("Successfully fall back to standalone after MPC refuse.",0,0,0);
            }
            else    
            {
              /* if we couldn't continue with the SA fix, then clean up Vx state machine */
              tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);
            }
          }
          break;
        }
      }
    }
    else
    {
      MSG_ERROR("Correlation ID in V2 SPPResp does not match", 0, 0, 0);
    }
  }
  else
  {
    MSG_ERROR("No Active application", 0, 0, 0);
  }
}

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
void tm_v2_handle_pos_rpt_resp(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;

  if(active_app_idx == (int)PDSM_CLIENT_TYPE_NI)
  {
    if(tm_vx_gen_info.sess_info.correlation_id == tm_vx_gen_info.sess_info.v2_pos_rpt_resp.cor_id)
    { 
      if(tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes <= tm_vx_ni_session_info.current_fix_count)
      {
        MSG_MED("NI tracking session finished", 0, 0, 0);
        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
        return;
      }
      
      if(tm_vx_gen_info.sess_info.v2_pos_rpt_resp.report == 0)
      {
        MSG_MED("Got successful pos rpt response.", 0, 0, 0);
      }
      else if(tm_vx_gen_info.sess_info.v2_pos_rpt_resp.report == 1)
      {
        MSG_HIGH("MPC discontiunes pos rpt.", 0, 0, 0);
        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
      }
#ifdef FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF
      else if(tm_vx_gen_info.sess_info.v2_pos_rpt_resp.report == 2)
      {
        MSG_HIGH("MPC performed handoff", 0, 0, 0);
        /* Set PDE IP address */
        tm_v2_set_pde_info(TM_V2_MESSAGE_TYPE_POS_RPT_RSP);
      }
#endif /* FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF */
      else
      {
        MSG_ERROR("Unknown Report Status Indicator received. Ignore it.", 0, 0, 0);
      }
    }
    else
    {
      MSG_ERROR("Correlation ID in V2 Pos Rpt Resp does not match. Position reprt failed.", 0, 0, 0);
    }
  }
  else
  {
    MSG_ERROR("Got Pos Rpt Resp for invalid client type.", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION tm_v2_handle_t7_timer_expiry

DESCRIPTION
  This function is called to handle V2 T7 timeout
 (From sending of SPPReq to receipt of SPPResp) 

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_v2_handle_t7_timer_expiry(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  boolean                              ret_val;

  if(active_app_idx == (int)PDSM_CLIENT_TYPE_NI)
  {
    MSG_HIGH("Handling T7 expiry for V2 NI", 0, 0, 0);

    /* Close MPC Connection */
    (void)tm_vx_ip_disconnect_from_mpc(FALSE);
  
    if(tm_vx_gen_info.sess_info.v2_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_IS801)
    {
      /* Send cancel to MPC to cancel the session */      
      (void) tm_v2_send_mo_cancel_sms(TM_VX_MSG_V2_CANCEL_REASON_MPC_TIMEOUT);
      
      /* Clean up Vx state machine */      
      tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);
    }
    else
    {
      if( (tm_vx_gen_info.sess_info.v2_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR)
        &&(tm_vx_gen_info.sess_info.v2_mt_sms.num_fixes > 1))
      {
        MSG_HIGH("Ignore T7 expiry, continue cell/sector reporting", 0, 0, 0);

        //<<<[+ by Liao]
        // Add code to set vx state machine to allow periodical reporting
        //>>>[+ End]
      }
      else
      {
        MSG_HIGH("Ignore T7 expiry event", 0, 0, 0);

        /* Clean up Vx state machine */      
        tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);
      }
    }
  }
  else
  {
    MSG_HIGH("Handling T7 expiry for V2 MO; calling is801 driver to proceed with SA session", 0, 0, 0);

    /* try to fallback to SA */
    ret_val = tm_vx_fallback_to_standalone(); 

    /* Close MPC Connection */
    (void)tm_vx_ip_disconnect_from_mpc(FALSE);     
    
    if(ret_val == TRUE) /*lint !e506 !e731 */
    {
       MSG_MED("Successfully called IS801_drv_req after MPC error",0,0,0);
    }
    else    
    {
       /* if we couldn't continue with the SA fix, then clean up Vx state machine */
    tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);
    }
  }
}

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
void tm_v2_handle_ms_mpc_timer_expiry(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;
  boolean                              ret_val;
  
  if(active_app_idx == (int)PDSM_CLIENT_TYPE_NI)
  {
    MSG_HIGH("Handling MS-MPC expiry for V2 NI", 0, 0, 0);

    /* Send cancel to MPC to cancel the session */      
    (void) tm_v2_send_mo_cancel_sms(TM_VX_MSG_V2_CANCEL_REASON_MPC_CONN_FAIL);
      
    /* Clean up Vx state machine */      
    tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);
  }
  else
  {    
         
    MSG_HIGH("Handling  MS-MPC expiry for V2 MO; calling is801 driver to proceed with SA session", 0, 0, 0);

    /* try to fallback to standalone */
    ret_val = tm_vx_fallback_to_standalone(); 

    if(ret_val == TRUE) /*lint !e506 !e731 */
    {
      MSG_MED("Successfully called IS801_drv_req after MPC error",0,0,0);
    }
    else
    {
    /* Clean up Vx state machine */
    tm_vx_reinit(TRUE, TM_STOP_REASON_MPC_REJECTED);   
    }
  }
}

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
void tm_v2_handle_t14_timer_expiry(void)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;

  if(active_app_idx == (int)PDSM_CLIENT_TYPE_NI)
  {
    /* Update app state */
    if(tm_vx_gen_info.app_info[active_app_idx].app_state == TM_VX_APP_STATE_WAIT_TO_RECV_POS_RPT_ACK)
    {
       MSG_MED("T14 expiry for NI session is waiting for pos report ack, ignore it and continue NI session", 0, 0, 0);
      (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_INIT);
    }
    else
    {
       MSG_ERROR("T14 expiry for NI session is in %d state", tm_vx_gen_info.app_info[active_app_idx].app_state, 0, 0);    
    }    
  }
  else
  {
     MSG_ERROR("T14 expiry event in invalid state", 0, 0, 0);
  }  
}


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
void tm_v2_handle_ms_abort_session_req(const tm_sess_stop_reason_e_type abort_reason)
{
  int active_app_idx = tm_vx_gen_info.sess_info.active_app_index;

  if((active_app_idx > (int)PDSM_CLIENT_TYPE_NONE) && (active_app_idx < (int)PDSM_CLIENT_TYPE_MAX))
  {      
    if(active_app_idx == PDSM_CLIENT_TYPE_NI)
    {
      MSG_MED("Abort V2 NI session", 0, 0, 0);

      if( (tm_vx_gen_info.app_info[active_app_idx].app_state == TM_VX_APP_STATE_WAIT_USER_RESP)
        ||(tm_vx_gen_info.app_info[active_app_idx].app_state == TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ)
        ||(tm_vx_gen_info.app_info[active_app_idx].app_state == TM_VX_APP_STATE_WAIT_TO_RECV_SPPRESP))
      {
        /* If NI session has not gotten SPPResp back from MPC, send reject to abort NI */
        MSG_MED("Send reject to abort V2 NI session", 0, 0, 0);
        
        (void) tm_v2_send_mo_reject_sms(TM_VX_MSG_V2_REJ_REASON_USER_DENIED,
                                 tm_vx_gen_info.sess_info.v2_mt_sms.correlation_id);

        /* Re-init Vx Module */
        if(tm_vx_gen_info.app_info[active_app_idx].app_state == TM_VX_APP_STATE_WAIT_USER_RESP)
        {
          tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
        }
        else
        {
          /* Need to stop TM-Core because TM-Core is in wait_auth state */
          tm_vx_reinit(TRUE, TM_STOP_REASON_GENERAL_ERROR);        
        }
      }
      else
      {    
        /* For single shot, no cancel needs to be sent, just do tm_vx_reinit */
        
        if(tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.session == PDSM_PD_SESS_TYPE_TRACK_IND)
        {
          /* If NI sessin has already gotten SPPResp back from MPC, send cancel to abort NI */
        
          MSG_MED("Send cancel to abort V2 NI session", 0, 0, 0);
        
          /* For NI session, send out MO SMS Cancel */
          (void) tm_v2_send_mo_cancel_sms(TM_VX_MSG_V2_CANCEL_REASON_USER_INIT); 
        }

        /* Re-init Vx Module */        
        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
      }
    }
    else
    {
      MSG_MED("Abort V2 MO session", 0, 0, 0);

      if(tm_vx_gen_info.app_info[active_app_idx].app_state == TM_VX_APP_STATE_WAIT_TO_SEND_SPPREQ)
      {
        /* If MO session has not sent SPPReq to MPC, tm_vx_reinit and ask TM_CORE to stop to avoid
           being stuck in WAIT_AUTH state*/
        tm_vx_reinit(TRUE, abort_reason);
      }
      else
      {        
        if(tm_vx_gen_info.app_info[active_app_idx].pd_req_info.pd_option.session == PDSM_PD_SESS_TYPE_TRACK_IND)
        {
          /* If MO tracking session has already sent SPPReq to MPC, send cancel to abort MO */
        
          MSG_MED("Send cancel to abort V2 MO session", 0, 0, 0);
        
         (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_TO_SEND_CANCEL);
     
          /* For MO session, We need to send out TCP/IP Cancel when connection comes up*/
          /* Re-int Vx module after TCP/IP cancel is sent out */
          (void) tm_vx_ip_connect_to_mpc();
        }
        else
        {
          /* For single shot, no cancel needs to be sent, just do tm_vx_reinit */
          tm_vx_reinit(TRUE, TM_STOP_REASON_GENERAL_ERROR);
        }        
      }
    }
  }
  else
  {
    MSG_ERROR("Invalid active_app_idx %d", active_app_idx, 0, 0);     
  }   
}

/*===========================================================================
FUNCTION tm_v2_handle_app_response

DESCRIPTION
  This function is called to hanlde app response based on V2 protocol

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_v2_handle_app_response
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
    MSG_MED("NI session req accepted", 0, 0, 0);

    if(tm_vx_ni_client_sess_param_check() == TRUE)
    {
      MSG_MED("V2 NI session started successfully", 0, 0, 0);      
                   
      (void)tm_vx_ni_client_start_session();
    }
    else
    {
      MSG_ERROR("Failed to start V2 NI session, send reject MO SMS to MPC", 0, 0, 0);
      
      (void) tm_v2_send_mo_reject_sms(TM_VX_MSG_V2_REJ_REASON_PRIV_IS801_IN_PROGRESS,
                               tm_vx_gen_info.sess_info.v2_mt_sms.correlation_id);

      tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
    }
  }
  else if(app_resp == PDSM_LCS_NOTIFY_VERIFY_DENY)
  {
    MSG_MED("NI session req rejected", 0, 0, 0);

    /* Send out MO SMS Reject and do proper clean-up */
    (void) tm_v2_send_mo_reject_sms(TM_VX_MSG_V2_REJ_REASON_USER_DENIED,
                             tm_vx_gen_info.sess_info.v2_mt_sms.correlation_id);

    tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
  }
  else if(app_resp == PDSM_LCS_NOTIFY_VERIFY_NORESP)
  {
    MSG_MED("NI session req not responded", 0, 0, 0);

    /* Send out MO SMS Reject and do proper clean-up */
    (void) tm_v2_send_mo_reject_sms(TM_VX_MSG_V2_REJ_REASON_POPUP_TIMEOUT,
                             tm_vx_gen_info.sess_info.v2_mt_sms.correlation_id);

    tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
  }
  else
  {
    MSG_ERROR("Unknown app response %d", app_resp, 0, 0);

    tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
  }
}

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
void tm_v2_set_ni_param(void)
{  
  tm_vx_ni_session_info.pos_tech = (tm_vx_mt_sms_pos_tech_ind_e_type)tm_vx_gen_info.sess_info.v2_mt_sms.position_tech_ind;
    
  /* Save tracking information for later */
  tm_vx_ni_session_info.get_pos_param.client_type                   = PDSM_CLIENT_TYPE_NI;
  tm_vx_ni_session_info.get_pos_param.client_id                     = pdsmclient_get_client_id_map(PDSM_CLIENT_TYPE_NI);
  tm_vx_ni_session_info.get_pos_param.mt_initiator                  = MT_UP;  // NI for 1X UP
  tm_vx_ni_session_info.pd_option_param.class_id                    = 0;

  /* MPC address is stored in NV */
  tm_vx_ni_session_info.pd_option_param.lsinfo.server_option        = PDSM_SERVER_OPTION_USE_DEFAULT;

  tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes             = tm_vx_gen_info.sess_info.v2_mt_sms.num_fixes;
  tm_vx_ni_session_info.pd_option_param.fix_rate.time_between_fixes_ms = tm_vx_gen_info.sess_info.v2_mt_sms.tbf * 1000;

  if(tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes == 1)
  {
    tm_vx_ni_session_info.pd_option_param.session = PDSM_PD_SESS_TYPE_NEW;
  }
  else
  {
    tm_vx_ni_session_info.pd_option_param.session = PDSM_PD_SESS_TYPE_TRACK_IND;
  }
  
  /*
    Need to make sure that TM-Core implements MSB proxy query mechanism
  */
  tm_vx_ni_session_info.pd_option_param.operation_mode = tm_vx_ni_client_xlate_op_mode(tm_vx_gen_info.sess_info.v2_mt_sms.is801_mode);

  if(tm_vx_gen_info.sess_info.v2_mt_sms.qos_incl)
  {
    tm_vx_ni_session_info.pd_qos_param.gps_session_timeout = tm_vx_gen_info.sess_info.v2_mt_sms.qos;
  }
  else
  {
    tm_vx_ni_session_info.pd_qos_param.gps_session_timeout = TM_VX_DEFAULT_QOS;
  }
    
  tm_vx_ni_session_info.pd_qos_param.accuracy_threshold = TM_VX_DEFAULT_ACCURACY_THRESHOLD;

  tm_vx_ni_session_info.get_pos_param.option_ptr = &(tm_vx_ni_session_info.pd_option_param);
  tm_vx_ni_session_info.get_pos_param.qos_ptr    = &(tm_vx_ni_session_info.pd_qos_param);

  /* Set fix count to 1 for position report */
  tm_vx_ni_session_info.current_fix_count                           = 1;


  tm_vx_ni_session_info.sess_state = TM_VX_NI_CLIENT_SESS_STATE_INIT;

  return;
}

#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
