/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Vx NI Client module

General Description
  This file contains implementations for TM Vx NI client
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/vx/src/tm_vx_ni_client.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/13    ah     Support for non-deferrable timers
12/08/06    cl     Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_GPSONE_VX_LCS_AGENT
#include "pdapiclient.h"
#include "tm_vx_i.h"
#include "tm_vx_ni_client.h"

/* Global variable to keep track of singleshot/tracking information */
tm_vx_ni_client_session_info_s_type tm_vx_ni_session_info;

/*===========================================================================
FUNCTION tm_vx_ni_client_xlate_op_mode

DESCRIPTION
  This function translates V2 SMS is801 mode to corresponding PD API op mode.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
pdsm_pd_session_operation_e_type tm_vx_ni_client_xlate_op_mode(uint8 is801_mode)
{
  switch(is801_mode)
  {
    case 0: /* MSA Only */
    return PDSM_SESSION_OPERATION_MSASSISTED;

    case 1: /* MSB Only */
    return PDSM_SESSION_OPERATION_MSBASED;

    case 2: /* MSA preferred, MSB allowed */
    return PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY;

    case 3: /* MSB preferred, MSA allowed */
    return PDSM_SESSION_OPERATION_OPTIMAL_SPEED;

    default:
    return PDSM_SESSION_OPERATION_MIN;
  }
}

/*===========================================================================
FUNCTION tm_vx_ni_client_continue_tracking

DESCRIPTION
  This function checks if tracking session needs to be performed

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_ni_client_continue_tracking(void)
{
  boolean ret_val = TRUE;
  
  /* Following conditions have to met for triggering next PD session 
     1. Previous PD session has finished
     2. Tracking timer has expired
  */
  if( (tm_vx_ni_session_info.sess_state != TM_VX_NI_CLIENT_SESS_STATE_PD_IN_PROGRESS)
    &&(os_TimerCheck(tm_vx_ni_session_info.tracking_timer) == 0))
  {     
    tm_vx_ni_session_info.current_fix_count++;

    if(tm_vx_ni_session_info.pos_tech == TM_VX_MT_SMS_POS_TECH_IND_IS801)
    {
      if(tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes >= tm_vx_ni_session_info.current_fix_count)
      {
        if(tm_core_handle_get_pos(&(tm_vx_ni_session_info.get_pos_param)))
        {      
          tm_vx_ni_session_info.sess_state = TM_VX_NI_CLIENT_SESS_STATE_PD_IN_PROGRESS;
        }
          
        /* If there are still more than 1 fix left, re-start ni tracking timer */
        if(tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes > tm_vx_ni_session_info.current_fix_count)
        {
          (void)os_TimerStart(tm_vx_ni_session_info.tracking_timer, 
                              tm_vx_ni_session_info.pd_option_param.fix_rate.time_between_fixes_ms,
                              NULL);
        }
  
        ret_val = TRUE;
      }
    }
    else if(tm_vx_ni_session_info.pos_tech == TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR)
    {
      /* For V2 cell/sector tracking, report postion now */
      if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
      {
        MSG_HIGH("Cell/sector report", 0, 0, 0);
        tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.lat = 0;
        tm_vx_gen_info.sess_info.lr_resp.pd_info.pd_data.lon = 0;

        /* Update app state */
        (void)tm_vx_update_app_state(tm_vx_gen_info.sess_info.active_app_index, TM_VX_APP_STATE_WAIT_TO_SEND_POS_RPT);
        
        /* Try to bring up a TCP connection to MPC for position report */
        (void) tm_vx_ip_connect_to_mpc();
      }

      /* Cell/Sector report started from T seconds (tracking timer callabck) after SPPResp */
      if(tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes >= tm_vx_ni_session_info.current_fix_count)
      {
        (void)os_TimerStart(tm_vx_ni_session_info.tracking_timer, 
                            tm_vx_ni_session_info.pd_option_param.fix_rate.time_between_fixes_ms,
                            NULL);
      }
    
      ret_val = TRUE;    
    }
    else
    {
      MSG_ERROR("Invalid Pos tech for tracking session", 0, 0, 0);
      return FALSE;
    }
  }

  return ret_val;
}

/*===========================================================================
FUNCTION tm_vx_ni_client_sess_param_check

DESCRIPTION
  This function is called to check if an NI session is allowed by TM-Core

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_ni_client_sess_param_check(void)
{
  if(tm_core_get_pos_para_check(&(tm_vx_ni_session_info.get_pos_param)) == PDSM_PD_CMD_ERR_NOERR)
  {
    return TRUE;
  }
  else
  {
    /* Allow V1 NI cell/sector/cached positioning while there is MO session */
    if( (tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
      &&( (tm_vx_gen_info.sess_info.incoming_v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CELL_SECTOR)
        ||(tm_vx_gen_info.sess_info.incoming_v1_mt_sms.position_tech_ind == TM_VX_MT_SMS_POS_TECH_IND_CACHED)))
    {
      MSG_HIGH("Allow cell/sector NI Session", 0, 0, 0);
      return TRUE;
    }
    else
    {
      MSG_HIGH("NI Session is not allowed by TM-Core", 0, 0, 0);
      return FALSE;
    }
  }
}

/*===========================================================================
FUNCTION tm_vx_ni_client_start_session

DESCRIPTION
  This function is called to start Vx NI session.
  It is called in following 2 scenarios
  1. NI req without user verification needed.
  2. NI req with user verificcation needed and user accept the ni req.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_vx_ni_client_start_session(void)
{
  boolean ret_val = TRUE;
  
  if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V1_ONLY)
  {
    MSG_HIGH("Starting V1 NI session", 0, 0, 0);

    tm_v1_set_ni_param();

    /*
      If it is a IS801 session request, perform IS801 
      else no need to do anything
    */  
    if(tm_vx_ni_session_info.pos_tech == TM_VX_MT_SMS_POS_TECH_IND_IS801)
    {
      if(tm_core_handle_get_pos(&(tm_vx_ni_session_info.get_pos_param)))
      {      

        tm_vx_ni_session_info.sess_state = TM_VX_NI_CLIENT_SESS_STATE_PD_IN_PROGRESS;   

        ret_val = TRUE;
      }
      else
      {
        ret_val = FALSE;
      }
    }    
  }
  else if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
  {
    MSG_HIGH("Starting V2 NI session", 0, 0, 0);

    tm_v2_set_ni_param();

    /* Call tm_core_handle_get_pos to trigger SPPReq/SPPResp */
    if(tm_core_handle_get_pos(&(tm_vx_ni_session_info.get_pos_param)))
    {      
      tm_vx_ni_session_info.sess_state = TM_VX_NI_CLIENT_SESS_STATE_PD_IN_PROGRESS;
    
      /* If there are still more than 1 fix left, re-start ni tracking timer */
      if(tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes - tm_vx_ni_session_info.current_fix_count > 0)
      {
        (void)os_TimerStart(tm_vx_ni_session_info.tracking_timer, 
                            tm_vx_ni_session_info.pd_option_param.fix_rate.time_between_fixes_ms,
                            NULL);
      }
      ret_val = TRUE;
    }
    else
    {
      ret_val = FALSE;
    }
  }
  else
  {
    MSG_ERROR("Invalid Vx config", 0, 0, 0);
    ret_val = FALSE;
  }
      
  return ret_val;
}

/*===========================================================================
FUNCTION tm_vx_ni_client_stop_session

DESCRIPTION
  This function is called to stop Vx NI session.
  It can be called in following scenarios
  1. User aborts NI session.
  2. MPC discontiues NI tracking session.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_ni_client_stop_session(void)
{
  MSG_HIGH("Reset NI Client", 0, 0, 0);

  tm_vx_ni_session_info.sess_state                          = TM_VX_NI_CLIENT_SESS_STATE_INIT;
  tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes  = 0;
  tm_vx_ni_session_info.current_fix_count                   = 0;   
  tm_vx_ni_session_info.get_pos_param.internal_session = FALSE;
  (void) os_TimerStop(tm_vx_ni_session_info.tracking_timer);
}

/*===========================================================================
FUNCTION tm_vx_ni_client_tracking_timer_handler

DESCRIPTION
  This function handles ni tracking timer expiry event
  
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_ni_client_tracking_timer_handler(void)
{
    
  MSG_MED("NI tracking timer expires, num of fixes left %d", tm_vx_ni_session_info.pd_option_param.fix_rate.num_fixes - tm_vx_ni_session_info.current_fix_count, 0, 0);
  
  if(tm_vx_ni_client_continue_tracking() == FALSE)
  {
    if(tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
    {
      MSG_ERROR("Failed to continue V2 tracking session, send cancel MO SMS to MPC", 0, 0, 0);
      
      (void)tm_v2_send_mo_cancel_sms(TM_VX_MSG_V2_CANCEL_REASON_USER_INIT);

      tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
    }
  }
}

/*===========================================================================
FUNCTION tm_vx_ni_client_init

DESCRIPTION
  This function initialize tm vx ni client module

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_ni_client_init(void)
{
  tm_vx_ni_session_info.pos_tech          = TM_VX_MT_SMS_POS_TECH_IND_IS801;
  tm_vx_ni_session_info.sess_state        = TM_VX_NI_CLIENT_SESS_STATE_INIT;
  tm_vx_ni_session_info.tracking_timer    = os_TimerCreateInCgpsTimerGroup((uint32)TM_VX_TIMER_ID_NI_TRACKING,
                                                                           (uint32)THREAD_ID_SM_TM);
  tm_vx_ni_session_info.current_fix_count = 0;  
  tm_vx_ni_session_info.get_pos_param.internal_session = FALSE;
  tm_vx_ni_session_info.get_pos_param.mt_initiator = MT_NA;
}

/*===========================================================================
FUNCTION tm_vx_ni_handle_pd_event

DESCRIPTION
  This function is called to pass PD event to NI client

DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
 None

===========================================================================*/
void tm_vx_ni_handle_pd_event(pdsm_pd_event_type  pd_event)
{
  if(pd_event & PDSM_PD_EVENT_DONE)
  {
    tm_vx_ni_session_info.sess_state = TM_VX_NI_CLIENT_SESS_STATE_PD_DONE;

    if( (tm_vx_gen_info.vx_config.vx_version == TM_VX_VERSION_V2_ONLY)
      &&(tm_vx_ni_session_info.current_fix_count != 0))
    {
      if(tm_vx_ni_client_continue_tracking() == FALSE)
      {  
        MSG_ERROR("Failed to continue V2 tracking session, send cancel MO SMS to MPC", 0, 0, 0);
      
        (void)tm_v2_send_mo_cancel_sms(TM_VX_MSG_V2_CANCEL_REASON_USER_INIT);

        tm_vx_reinit(FALSE, TM_STOP_REASON_NONE);
      }
    }
  }
}

#endif /* FEATURE_GPSONE_VX_LCS_AGENT */

