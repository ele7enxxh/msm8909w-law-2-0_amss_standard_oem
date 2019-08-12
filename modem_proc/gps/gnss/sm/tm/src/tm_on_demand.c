                   
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Standalone On-Demand Sub-module

General Description
  This file contains implementations for On-Demand Positioning 1.0
  
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_on_demand.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/10   gk     fix count and log changes
09/30/09   gk     Support ODP 2.0
08/10/09   gk     Support ODP 2.0
07/23/09   gk     Support ODP 2.0
07/09/09   gk      ODP 1.1 related fixes for warmup
06/12/09   gk      ODP 1.1 support
09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
08/25/08  gk      On-Demand GPS lock changes.
07/29/08  gk      Changes to logs that were not coded properly
06/09/08  gk      Changes to logs that were not coded properly
06/09/08  gk      On-Demand Changes to add some logs and events
05/1/08   gk      Initail Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "math.h"
#include "msg.h"
#include "aries_os_api.h"
#include "tm_data.h"
#include "tm_common.h"
#include "tm_prtl_iface.h"
#include "sm_api.h"
#include "pdapibuf.h"
#include "tm_cm_iface.h"
#include "sm_log.h"
#ifdef FEATURE_CGPS_CDMA_IF
  #include "tm_is801_util.h"
  #include "tm_1x_up_smod_iface.h"
#endif

#ifdef FEATURE_CGPS_XTRA
  #include "tm_xtra.h"
#endif /* FEATURE_CGPS_XTRA */
#include "tm_on_demand.h"

/*#include "chargeri.h" */ /* for charger states */
#if defined(FEATURE_BATTERY_CHARGER)
#include "charger.h"
#endif
#include "sm_dm_event.h"
#include "tm_qwip_iface.h"

static tm_on_demand_charger_status tm_on_demand_get_charger_status(void);
static tm_on_demand_battery_status tm_on_demand_get_battery_status(void );
static void tm_on_demand_charger_poll_stop(void);
static void tm_on_demand_stop_on_demand_processing(tm_sess_stop_reason_e_type reason);
static void tm_on_demand_stop_on_demand_ready_processing(tm_sess_stop_reason_e_type reason);

static void tm_on_demand_stop_sess_req(tm_sess_stop_reason_e_type reason);

static boolean on_demand_at_bootup_done = FALSE; /* have we requested a session at boot up yet? */

static void tm_on_demand_xlate_stop_reason_to_on_demand_reason
      (tm_sess_stop_reason_e_type reason, tm_on_demand_event_type *event);
// extern variable for the sess info
tm_standalone_sess_info_s_type tm_standalone_sess_info;
sm_OnDemandPosPuncTuncStruct z_PuncTunc;

/*===========================================================================

FUNCTION tm_on_demand_get_lpm_mode

DESCRIPTION   get the next mode that on-demand should go into
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_get_lpm_mode(void)
{
  uint8 current_mode = tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type;
  tm_on_demand_event_type reason = TM_ON_DEMAND_EVENT_ERROR_NONE;

  MSG_MED("LPM Mode, current_mode - %d, no-fixes - %d, ", 
          tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type,
          tm_standalone_sess_info.on_demand_sess_info.num_no_fixes, 0);

  switch (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type)
  {
    case TM_ON_DEMAND_MODE_1:
      if (tm_standalone_sess_info.on_demand_sess_info.num_no_fixes >= TM_ON_DEMAND_MODE_1_NO_FIX_COUNT)
      {
        /* change state to mode 2*/
        tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_2;
        reason = TM_ON_DEMAND_EVENT_NO_FIX_COUNT_REACHED;
      }
      break;

    case TM_ON_DEMAND_MODE_2:
      if (tm_standalone_sess_info.on_demand_sess_info.num_no_fixes >= TM_ON_DEMAND_MODE_2_NO_FIX_COUNT)
      {
        /* change state to mode 3*/
        tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_3;
        reason = TM_ON_DEMAND_EVENT_NO_FIX_COUNT_REACHED;
      }
      else if (tm_standalone_sess_info.on_demand_sess_info.num_no_fixes == 0)
      {
        /* change state to mode 1*/
        tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_1;
        reason = TM_ON_DEMAND_EVENT_FIX_OBTAINED;

      }
      break;

    case TM_ON_DEMAND_MODE_3:
      if (tm_standalone_sess_info.on_demand_sess_info.num_no_fixes == 0)
      {
        /* change state to mode 1*/
        tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_1;
        reason = TM_ON_DEMAND_EVENT_FIX_OBTAINED;

      }
      break;
 
    default:
      /* in all other modes, remain in this mode */
      break;
  }

  if (current_mode != (uint8) tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type)
  {
    sm_report_event_gps_on_demand_mode_change(current_mode, 
                                              tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, reason);
    tm_standalone_sess_info.on_demand_sess_info.num_no_fixes = 0;

  }
  MSG_MED("LPM Mode, mode changed to - %d, no-fixes - %d, ", 
          tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type,
          tm_standalone_sess_info.on_demand_sess_info.num_no_fixes, 0);


}



/*===========================================================================

FUNCTION tm_on_demand_check_gps_lpm_on_status

DESCRIPTION  check if GPS can be enabled for on_demand LPM mode
  
DEPENDENCIES

RETURN VALUE Returns if On-demand positioning can be started. If it can be 
     started, TM_ON_DEMAND_EVENT_ERROR_NONE is returned, else an error code 
     returned

SIDE EFFECTS

===========================================================================*/
tm_on_demand_event_type tm_on_demand_check_gps_lpm_on_status (void)
{

  tm_on_demand_event_type    ret_val = TM_ON_DEMAND_EVENT_ERROR_NONE;

  tm_cm_phone_state_info_s_type phone_state_info;
  /*-----------------------------------------------------------------------
  Check current XTRA module state
  -----------------------------------------------------------------------*/
/* using do-while(0) idiom to allow single point of return */
  do
  {
    // check for valid XTRA data
#if 0
    if (!tm_xtra_data_valid())
    {
      MSG_HIGH("XTRA data is not valid. On demand not started", 0, 0,0);
      ret_val = TM_ON_DEMAND_EVENT_ERROR_XTRA_DATA_NOT_VALID;
      break;
    }
#endif
    // check if phone is OOS

    tm_cm_iface_get_phone_state(&phone_state_info);

    if (phone_state_info.srv_system == CGPS_SRV_SYS_NONE ||
        phone_state_info.srv_system == CGPS_SRV_SYS_OTHER)
    {
      MSG_HIGH("Phone out of service. On-Demand not started.",0,0,0);
      ret_val = TM_ON_DEMAND_EVENT_ERROR_PHONE_OUT_OF_SERVICE;
      break;
    }

    // if session already in progress
    if (tm_core_session_active())
    {
      MSG_HIGH("Another session is active. On demand not started", 0,0,0);
      ret_val = TM_ON_DEMAND_EVENT_ERROR_GPS_SESSION_ACTIVE;
      break;
    }
  } while (0);
  if (ret_val != TM_ON_DEMAND_EVENT_ERROR_NONE)
  {
    sm_log_on_demand_not_started(tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, ret_val,
      tm_standalone_sess_info.on_demand_sess_info.num_no_fixes, 0,0);
  }
  return ret_val;
}


/*===========================================================================

FUNCTION tm_on_demand_start_on_demand_processing

DESCRIPTION start on demand positioning 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_start_on_demand_processing(void)
{
  tm_on_demand_event_type ret_val;

  /* Req PUNC, TUNC etc to check possibility of Quick GPS Fix*/
  tm_core_get_gps_state_param_s_type get_gps_state_param;

  get_gps_state_param.source = SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND;
  get_gps_state_param.cmd.p_cmd_type = NULL;

  MSG_HIGH("Start On demand processing",0,0,0);
  ret_val = tm_on_demand_check_gps_lpm_on_status();

  if (TM_ON_DEMAND_EVENT_ERROR_NONE != ret_val)
  {
    MSG_HIGH("Cannot start on demand processing %d. Wait for the next timeout", ret_val,0,0);
    tm_on_demand_wait_for_next_timeout(TRUE);
  }
  else
  {
    MSG_HIGH("Start on demand. Get PUNC TUNC from MGP" ,0,0,0);
#if 0
    mgp_GetPosPuncTunc();
#endif
    tm_core_get_gps_state(&get_gps_state_param);
  }
}

/*===========================================================================

FUNCTION tm_on_demand_stop_on_demand_processing

DESCRIPTION stop on demand positioning 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_stop_on_demand_processing(tm_sess_stop_reason_e_type reason)
{

  MSG_MED("ON DEMAND LPM Stoppped due to %d", reason,0,0);

  tm_standalone_sess_info.on_demand_sess_info.num_no_fixes = 0;
  tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on = FALSE;
  tm_on_demand_stop_sess_req(reason);
}


/*===========================================================================

FUNCTION tm_on_demand_lpm_enable_handler

DESCRIPTION handle on demand low power mode enable or disable

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_lpm_enable_handler(boolean on_demand_enabled)
{
  MSG_MED("On Demand LPM enable handler. LPM status %d, ", on_demand_enabled,0,0);
  if (on_demand_enabled == FALSE)
  {
    /* stop on demand positioning */
    tm_on_demand_stop_on_demand_processing(TM_STOP_REASON_ON_DEMAND_DISABLED);
    on_demand_at_bootup_done = FALSE;
  }
  else
  {
    /* when on demand is enabled, start the poll timer. this will trigger the first
    session and then the lpm mode 1*/
    tm_on_demand_charger_poll_init();
  }
}


/*===========================================================================
FUNCTION tm_on_demand_start_sess_req

DESCRIPTION  Start On-Demand positioning. this will get the mode and then
start the positoning session with tm-core .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_on_demand_start_sess_req(sm_OnDemandPosPuncTuncStruct *pos_data_ptr)
{
  tm_sess_req_param_u_type  sess_req_param;
  boolean                   ret_val = TRUE;
  boolean                   start_sess = TRUE;
    
#ifdef FEATURE_CGPS_ODP_2_0   
  os_IpcMsgType             *ipc_msg_ptr = NULL;
  tm_pdsm_cmd_s_type* sess_req_ptr;
#endif
  /* Update Standalone Session Info */
  MSG_MED("Start on demand session, current_mode - %d ", 
          tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, 0, 0);

 tm_standalone_sess_info.op_mode                     = TM_SESS_OPERATION_MODE_STANDALONE;

  switch (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type)
  {
    case TM_ON_DEMAND_MODE_1:
      MSG_HIGH("ON Demand Mode 1", 0, 0, 0);
      tm_standalone_sess_info.qos.gps_session_timeout = TM_ON_DEMAND_LPM_MODE1_SESS_TIMEOUT;
      tm_standalone_sess_info.qos.accuracy_threshold = TM_ON_DEMAND_LPM_MODE1_ACCURACY_THRESHOLD;
      tm_standalone_sess_info.fix_rate.num_fixes = TM_ON_DEMAND_LPM_FIX_NUMBER;
      tm_standalone_sess_info.fix_rate.time_between_fixes_ms = TM_ON_DEMAND_LPM_MODE1_FIX_TBF_MS;
 #ifdef FEATURE_CGPS_ODP_2_0   
      tm_standalone_sess_info.op_mode                     = TM_SESS_OPERATION_MODE_ODP;
#endif
      break;

    case TM_ON_DEMAND_MODE_2:
      MSG_HIGH("ON Demand Mode 2", 0, 0, 0);
      tm_standalone_sess_info.qos.gps_session_timeout = TM_ON_DEMAND_LPM_MODE2_SESS_TIMEOUT;
      tm_standalone_sess_info.qos.accuracy_threshold = TM_ON_DEMAND_LPM_MODE2_ACCURACY_THRESHOLD;
      tm_standalone_sess_info.fix_rate.num_fixes = TM_ON_DEMAND_LPM_FIX_NUMBER;
      tm_standalone_sess_info.fix_rate.time_between_fixes_ms = TM_ON_DEMAND_LPM_MODE2_FIX_TBF_MS;
      break;

    case TM_ON_DEMAND_MODE_3:
      MSG_HIGH("ON Demand Mode 3", 0, 0, 0);
      tm_standalone_sess_info.qos.gps_session_timeout = TM_ON_DEMAND_LPM_MODE3_SESS_TIMEOUT;
      tm_standalone_sess_info.qos.accuracy_threshold = TM_ON_DEMAND_LPM_MODE3_ACCURACY_THRESHOLD;
      tm_standalone_sess_info.fix_rate.num_fixes = TM_ON_DEMAND_LPM_FIX_NUMBER;
      tm_standalone_sess_info.fix_rate.time_between_fixes_ms = TM_ON_DEMAND_LPM_MODE3_FIX_TBF_MS;
      break;
    case TM_ON_DEMAND_MODE_WARM_UP:
      
      MSG_HIGH("ON Demand Mode Warm Up", 0, 0, 0);
      tm_standalone_sess_info.qos.gps_session_timeout = TM_ON_DEMAND_LPM_WARMUP_SESS_TIMEOUT;
      tm_standalone_sess_info.qos.accuracy_threshold = TM_ON_DEMAND_LPM_WARMUP_ACCURACY_THRESHOLD;
      tm_standalone_sess_info.fix_rate.num_fixes = TM_ON_DEMAND_LPM_FIX_NUMBER;
      tm_standalone_sess_info.fix_rate.time_between_fixes_ms = TM_ON_DEMAND_LPM_WARMUP_FIX_TBF_MS;
      break;

    case TM_ON_DEMAND_MODE_READY:
      MSG_HIGH("ON Demand Mode Ready", 0, 0, 0);
      tm_standalone_sess_info.qos.gps_session_timeout = TM_ON_DEMAND_READY_1HZ_SESS_TIMEOUT;
      tm_standalone_sess_info.qos.accuracy_threshold = TM_ON_DEMAND_READY_1HZ_ACCURACY_THRESHOLD;
      tm_standalone_sess_info.fix_rate.num_fixes = TM_ON_DEMAND_READY_FIX_NUMBER;
      tm_standalone_sess_info.fix_rate.time_between_fixes_ms = TM_ON_DEMAND_READY_1HZ_FIX_TBF_MS;
      break;

    case TM_ON_DEMAND_MODE_BATTERY_CHARGING:
      MSG_HIGH("ON Demand Mode Battery Charging", 0, 0, 0);
      tm_standalone_sess_info.qos.gps_session_timeout = TM_ON_DEMAND_LPM_MODE1_SESS_TIMEOUT;
      tm_standalone_sess_info.qos.accuracy_threshold = TM_ON_DEMAND_LPM_MODE1_ACCURACY_THRESHOLD;
      tm_standalone_sess_info.fix_rate.num_fixes = TM_ON_DEMAND_LPM_FIX_NUMBER;
      tm_standalone_sess_info.fix_rate.time_between_fixes_ms = TM_ON_DEMAND_LPM_MODE1_FIX_TBF_MS;
      break;

    case TM_ON_DEMAND_MODE_BATTERY_CHARGED:
      MSG_HIGH("ON Demand Mode Battery Charged", 0, 0, 0);
      tm_standalone_sess_info.qos.gps_session_timeout = TM_ON_DEMAND_LPM_MODE1_SESS_TIMEOUT;
      tm_standalone_sess_info.qos.accuracy_threshold = TM_ON_DEMAND_LPM_MODE1_ACCURACY_THRESHOLD;
      tm_standalone_sess_info.fix_rate.num_fixes = TM_ON_DEMAND_LPM_FIX_NUMBER;
      tm_standalone_sess_info.fix_rate.time_between_fixes_ms = TM_ON_DEMAND_LPM_MODE1_FIX_TBF_MS;
      break;

    default:
      MSG_ERROR("Unknown On-Demand Mode ", 0,0,0);
      return;
  }

  if (start_sess == TRUE)
  {
    tm_standalone_sess_info.sess_handle                 = TM_SESS_HANDLE_ON_DEMAND;
    tm_standalone_sess_info.session_state               = TM_STANDALONE_SESS_STATE_WAIT_WISHLIST_UPDATE;
    sess_req_param.start_param.abort_curr_sess    = FALSE;
    sess_req_param.start_param.op_mode            = tm_standalone_sess_info.op_mode;
    sess_req_param.start_param.op_req             = TM_OP_REQ_LR;
    sess_req_param.start_param.lr_qos             = tm_standalone_sess_info.qos.gps_session_timeout;
    sess_req_param.start_param.accuracy_threshold = tm_standalone_sess_info.qos.accuracy_threshold;
    sess_req_param.start_param.num_fixes          = tm_standalone_sess_info.fix_rate.num_fixes;
    sess_req_param.start_param.tbf_ms             = tm_standalone_sess_info.fix_rate.time_between_fixes_ms;
  
    sess_req_param.start_param.dynamic_qos_enabled = FALSE;
    MSG_HIGH("Start on demand LPM Mode, Mode is %d , sess_timeout %d, acc_threshold %d  ",
             tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, 
             tm_standalone_sess_info.qos.gps_session_timeout, 
             tm_standalone_sess_info.qos.accuracy_threshold );
   #ifdef FEATURE_CGPS_ODP_2_0   
    /* Enable QWiP only for Modes 2, 3 and Warmup */
    if (tm_core_info.config_info.use_qwip_for_odp == FALSE ||
        tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_1 ||
        tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_READY )
    {
      /* Instruct TM-Core to kick of sessions */
      ret_val = tm_sess_req(TM_PRTL_TYPE_STANDALONE,
                            tm_standalone_sess_info.sess_handle,
                            TM_SESS_REQ_START,
                            &sess_req_param );
    }
    else
    {
      ipc_msg_ptr = os_IpcCreate(sizeof(tm_pdsm_cmd_s_type),IPC_ALLOCATION_DYNAMIC, THREAD_ID_SM_TM);
      if (ipc_msg_ptr)
      {
        sess_req_ptr = (tm_pdsm_cmd_s_type*)(ipc_msg_ptr->p_Data);
        ipc_msg_ptr->q_MsgId = TM_CORE_MSG_ID_ODP_SESS_START;
        sess_req_ptr->odp_mode_type = tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type;
        sess_req_ptr->odp_session = TRUE;
        if (pos_data_ptr)
        {
          sess_req_ptr->p_SysdPosPuncTunc.b_Valid = pos_data_ptr->b_Valid;
          sess_req_ptr->p_SysdPosPuncTunc.f_Punc = pos_data_ptr->f_Punc;
          sess_req_ptr->p_SysdPosPuncTunc.f_Tunc = pos_data_ptr->f_Tunc; 
        }
        sess_req_ptr->pdsm_cmd_type.cmd.pd.cmd = PDSM_PD_CMD_GET_POS;
        sess_req_ptr->pdsm_cmd_type.cmd.pd.client_id =  3333;
        sess_req_ptr->pdsm_cmd_type.cmd.pd.info.option_type.operation_mode = PDSM_SESSION_OPERATION_STANDALONE_ONLY;
        sess_req_ptr->pdsm_cmd_type.cmd.pd.info.option_type.session = PDSM_PD_SESS_TYPE_NEW;
        sess_req_ptr->pdsm_cmd_type.cmd.pd.info.option_type.fix_rate.num_fixes = 1; 
        sess_req_ptr->pdsm_cmd_type.cmd.pd.info.option_type.fix_rate.time_between_fixes_ms = tm_standalone_sess_info.fix_rate.time_between_fixes_ms;
        
        sess_req_ptr->pdsm_cmd_type.cmd.pd.info.qos.gps_session_timeout = tm_standalone_sess_info.qos.gps_session_timeout;
         sess_req_ptr->pdsm_cmd_type.cmd.pd.info.qos.accuracy_threshold = tm_standalone_sess_info.qos.accuracy_threshold ;

        if (!os_IpcSend(ipc_msg_ptr, THREAD_ID_SM_TM))
        {
          MSG_MED("tm_on_demand_start_fix_req(): os_IpcSend() failed!", 0, 0, 0);
          if (!os_IpcDelete(ipc_msg_ptr))
          {
            MSG_MED("tm_on_demand_start_fix_req(): os_IpcDelete() failed!", 0, 0, 0);
          }
        }
      } /* end if(ipc_msg_ptr) */      
    }
#else
      ret_val = tm_sess_req(TM_PRTL_TYPE_STANDALONE,
                          tm_standalone_sess_info.sess_handle,
                          TM_SESS_REQ_START,
                          &sess_req_param
                         );
#endif
    
    sm_report_event_gps_on_demand_mode_begin(tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type);
    
    if (ret_val == TRUE)
    {
      tm_standalone_sess_info.session_state = TM_STANDALONE_SESS_STATE_WAIT_LR;
  
      switch (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type)
      {
        case TM_ON_DEMAND_MODE_1:
        case TM_ON_DEMAND_MODE_2:
        case TM_ON_DEMAND_MODE_3:
        case TM_ON_DEMAND_MODE_BATTERY_CHARGING:
        case TM_ON_DEMAND_MODE_BATTERY_CHARGED:
          tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on = TRUE;
          break;
        case TM_ON_DEMAND_MODE_WARM_UP:
          tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on = TRUE;
          on_demand_at_bootup_done = TRUE;
          break;
        case TM_ON_DEMAND_MODE_READY:
          tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on = TRUE;
          break;
      }
    }
  }
}


/*===========================================================================
FUNCTION tm_on_demand_stop_sess_req

DESCRIPTION  Stop On-Demand positioning. this will send a session termimation req
to tm-core 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_on_demand_stop_sess_req(tm_sess_stop_reason_e_type reason)
{
  MSG_MED("On-Demand Session Stop request", 0,0,0);

  tm_on_demand_abort_handler(reason);

  /* if the on-demand has been disabled stop all the battery charger timers etc*/
  if (tm_core_get_on_demand_state() == FALSE)
  {
    tm_on_demand_charger_poll_stop();
  }
  if (tm_core_is_on_demand_session_on())
  {
    /* send a stop sess to standalone module to terminate the on-demand standalone session */
    sm_log_on_demand_stop(tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type,
                          (uint8)reason, tm_standalone_sess_info.on_demand_sess_info.num_no_fixes);
    tm_standalone_sess_termination(reason);
  }
}


/*===========================================================================

FUNCTION tm_trigger_on_demand_lpm

DESCRIPTION
  i
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_trigger_on_demand_lpm (sm_OnDemandPosPuncTuncStruct *pos_data_ptr)
{

  boolean trigger_lpm = TRUE;
  uint8 prev_mode, u_i;
  uint8 u_num_eph_have_known_in_view;
  uint32 q_long_word2;
  boolean fix_fail = TRUE;
#ifdef FEATURE_CGPS_ODP_2_0
  tm_cm_phone_state_info_s_type phone_state_info;
#endif
#define LIGHT_USEC    (299792458.0/(1000.0 * 1000.0))  /* m/s */
  
#define C_MIN_SV_IN_VIEW_THRESHOLD 6  
  prev_mode = tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type;
  
  q_long_word2 = pos_data_ptr->eph_svmask & pos_data_ptr->visible_svmask ; 
  u_num_eph_have_known_in_view = 0;
  for ( u_i=0; u_i<32; u_i++)
  {
    if ( q_long_word2 & 1<<u_i )
    {
      u_num_eph_have_known_in_view++;
    }
  }
  



  z_PuncTunc = *pos_data_ptr;
  MSG_HIGH("Got pos punc from mgp %d meters, %d us, num visible ephemeris %d" , (uint32)pos_data_ptr->f_Punc, (uint32)pos_data_ptr->f_Tunc, u_num_eph_have_known_in_view);

  if (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type != TM_ON_DEMAND_MODE_WARM_UP)
  {
    if (pos_data_ptr->b_Valid == FALSE)
    {
      MSG_HIGH("POS PUNC TUNC INVALID", 0, 0, 0);
      /* treat as a no fix*/
      trigger_lpm = FALSE;
    }
  }
  if (!tm_xtra_data_valid())
  {
    MSG_HIGH("XTRA data is not valid. ", 0, 0,0);
    /* all existing SVs should have almanac or eph */
    if (pos_data_ptr->health_svmask)
    {
      if ((pos_data_ptr->health_svmask & (pos_data_ptr->alm_svmask | pos_data_ptr->eph_svmask)) == (pos_data_ptr->health_svmask))
      {
        if (u_num_eph_have_known_in_view < C_MIN_SV_IN_VIEW_THRESHOLD )
        {
          if (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type != TM_ON_DEMAND_MODE_WARM_UP)
          {
            trigger_lpm = FALSE;
          }
        }
      }
      else
      {
        trigger_lpm = FALSE;
      }
    }
    else
    {
      trigger_lpm = FALSE;
    }
  }

  MSG_HIGH("Enough Almanac/Eph/XTRA to trigger ODP %d ", trigger_lpm, 0,0);
  MSG_HIGH("Visible SVs with ephemeris %d, HealthMask for SVs %d, almanac mask %d ", u_num_eph_have_known_in_view,
           pos_data_ptr->health_svmask, pos_data_ptr->alm_svmask);     

  
  tm_on_demand_get_lpm_mode();

  if (prev_mode != tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type)
  {
    MSG_HIGH(" On-demand state changed from %d to %d.", prev_mode,
      tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, 0);
    //trigger_lpm = FALSE;
  }
  z_PuncTunc = *pos_data_ptr;
  /* check the punc tunc */
  if (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_1)
  {
#ifdef FEATURE_CGPS_ODP_2_0
    /* if PUNC < 20 km, but TUNC is above threshold, trigger ODP for UMTS */
    if ((pos_data_ptr->f_Punc <= C_GPS_ODP_MODE1_PUNC_UPPER_LIMIT))
    { 
      tm_cm_iface_get_phone_state(&phone_state_info);
      if (phone_state_info.srv_system == CGPS_SRV_SYS_GSM ||
          phone_state_info.srv_system == CGPS_SRV_SYS_GW  ||
          phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA)
      {
        if ((pos_data_ptr->f_Tunc >= C_GPS_ODP_MODE1_TUNC_LOWER_LIMIT) &&
            (pos_data_ptr->f_Tunc <= C_GPS_ODP_MODE1_TUNC_UPPER_LIMIT))
        {
          MSG_HIGH(" On-demand state %d PUNC %d, TUNC %d", 
            tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, pos_data_ptr->f_Punc, 
            pos_data_ptr->f_Tunc);
        }
        else
        {
          MSG_HIGH("PUNC TUNC Within limits in mode 1. Tunc is %d us, Punc is %d meters", 
            (uint32)pos_data_ptr->f_Tunc,(uint32)pos_data_ptr->f_Punc ,0);
          trigger_lpm = FALSE;
        }
      }
      else
      {
        /* on 1x no need to trigger ODP*/
        MSG_HIGH("PUNC TUNC Within limits in mode 1. Tunc is %d us, Punc is %d meters", 
          (uint32)pos_data_ptr->f_Tunc,(uint32)pos_data_ptr->f_Punc ,0);
        trigger_lpm = FALSE;
      }
    }
    else
    {
      MSG_HIGH(" On-demand state %d PUNC %d, TUNC %d", 
        tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, pos_data_ptr->f_Punc, 
        pos_data_ptr->f_Tunc);
    }
#else
    if ((pos_data_ptr->f_Punc >= C_GPS_ODP_MODE1_PUNC_UPPER_LIMIT) &&
        ((pos_data_ptr->f_Tunc >= C_GPS_ODP_MODE1_TUNC_LOWER_LIMIT) &&
         (pos_data_ptr->f_Tunc <= C_GPS_ODP_MODE1_TUNC_UPPER_LIMIT)))
    {
      MSG_HIGH(" On-demand state %d PUNC %d, TUNC %d", tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, pos_data_ptr->f_Punc, pos_data_ptr->f_Tunc);
    }
    else
    {
      MSG_HIGH("PUNC TUNC Within limits in mode 1. Tunc is %d us, Punc is %d meters", (uint32)pos_data_ptr->f_Tunc,(uint32)pos_data_ptr->f_Punc ,0);
      trigger_lpm = FALSE;
    }
#endif
  }
  else if (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_2)
  {
    FLT f_Tmp;
    f_Tmp = pos_data_ptr->f_Punc/(LIGHT_USEC );
    f_Tmp += pos_data_ptr->f_Tunc;
    if ((uint32)f_Tmp > C_GPS_ODP_MODE2_TUNC_LIMIT)
    {
      MSG_HIGH(" On-demand state %d PUNC %d, TUNC %d", tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, pos_data_ptr->f_Punc, pos_data_ptr->f_Tunc);
    }
    else
    {
      MSG_HIGH("PUNC TUNC Within limits in mode 2. Tunc is %d us, Punc is %d meters", (uint32)f_Tmp,(uint32)pos_data_ptr->f_Punc ,0);
      trigger_lpm = FALSE;
    }
  }
  else if (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_3 )
  {
    MSG_HIGH(" On-demand state %d PUNC %d, TUNC %d", tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, pos_data_ptr->f_Punc, pos_data_ptr->f_Tunc);
  }
  else if (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_WARM_UP)
  {
    MSG_HIGH("WARM Up mode start", 0,0,0);
    tm_standalone_sess_info.on_demand_sess_info.num_no_fixes = 0;
    
  }
  if (trigger_lpm == FALSE)
  {
    /* if punc-tunc criteria not met, start the timer*/
    
    MSG_HIGH("Visible SVs with ephemeris %d, HealthMask for SVs %d, almanac mask %d ", u_num_eph_have_known_in_view,
             pos_data_ptr->health_svmask, pos_data_ptr->alm_svmask);     
    MSG_HIGH("Tunc is %d us, Punc is %d meters, ODP not started in mode %d. Wait for the next interval", 
      (uint32)pos_data_ptr->f_Tunc, (uint32)pos_data_ptr->f_Punc, tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type );
    /* if punc tunc are within limits, no need to increment the no-fix count */
    if (pos_data_ptr->f_Tunc < C_GPS_ODP_MODE1_TUNC_LOWER_LIMIT ||
        pos_data_ptr->f_Punc < C_GPS_ODP_MODE1_PUNC_UPPER_LIMIT)
    {
      fix_fail = FALSE; 
    }
    tm_on_demand_wait_for_next_timeout(fix_fail);
    sm_log_on_demand_not_started(tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, 
            TM_ON_DEMAND_EVENT_PUNC_TUNC_WITHIN_RANGE,
            tm_standalone_sess_info.on_demand_sess_info.num_no_fixes,
            pos_data_ptr->f_Punc, pos_data_ptr->f_Tunc);
  }
  else 
  {
    MSG_HIGH("PUNC TUNC values violated. Tunc is %d us, Punc is %d meters, Starting LPM mode %d", (uint32)pos_data_ptr->f_Tunc, (uint32)pos_data_ptr->f_Punc, tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type);
    /* trigger the session only if we have valid eph/alm/xtra data */
    sm_log_on_demand_start(tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, pos_data_ptr->f_Punc,
                           pos_data_ptr->f_Tunc );

    tm_on_demand_start_sess_req(pos_data_ptr);
  }




}

/*===========================================================================

FUNCTION tm_on_demand_timer_stop

DESCRIPTION   Stop the on-demand timer. this is usually done with MGP is turned
            ON
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_on_demand_timer_stop(void)
{
  // check which mode should be on

  MSG_MED("Stop on demand LPM Mode timer ",0,0,0);

  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired == FALSE)
  {
    tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired = TRUE;
    (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_mode_timer_ptr);
  }
}


/*===========================================================================

FUNCTION tm_on_demand_timer_start

DESCRIPTION   Start the on-demand timer. this is usually done with MGP is turned
            OFF
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_on_demand_timer_start(void)
{

  // if we are done doing GPS at bootup, set to lpm mode 1
  if (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_WARM_UP)
  {
    tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_1;
  }
  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired == FALSE)
  {
    tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired = TRUE;
    (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_mode_timer_ptr);
  }
  /* if we got a fix at any time, set the mode type to 1 */
  if (tm_standalone_sess_info.on_demand_sess_info.num_no_fixes == 0)
  {
    tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_1;
    on_demand_at_bootup_done = TRUE;
    if (tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr)
    {
      (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr);
    }
  }
  else if (on_demand_at_bootup_done == FALSE)
  {
    tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_WARM_UP;
    if (tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr)
    {
      (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr);
    }
  }
  MSG_MED("Start on demand LPM Mode timer for mode %d ",
          tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type,0,0);
  switch (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type )
  {
    case TM_ON_DEMAND_MODE_1:
    case TM_ON_DEMAND_MODE_BATTERY_CHARGING:
    case TM_ON_DEMAND_MODE_BATTERY_CHARGED:
      MSG_MED("Start on demand LPM Mode timer %d", tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type,
              0,0);
      (void)os_TimerStart(tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_mode_timer_ptr, 
                          TM_ON_DEMAND_LPM_MODE_1_TIMER_COUNT,
                          0);
      tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired = FALSE;
      break;

    case TM_ON_DEMAND_MODE_2:
      MSG_MED("Start on demand LPM Mode timer 2 ",0,0,0);
      (void)os_TimerStart(tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_mode_timer_ptr, 
                          TM_ON_DEMAND_LPM_MODE_2_TIMER_COUNT,
                          0);
      tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired = FALSE;
      break;
    case TM_ON_DEMAND_MODE_3:
      MSG_MED("Start on demand LPM Mode timer 3 ",0,0,0);
      (void)os_TimerStart(tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_mode_timer_ptr, 
                          TM_ON_DEMAND_LPM_MODE_3_TIMER_COUNT,
                          0);
      tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired = FALSE;
      break;
    case TM_ON_DEMAND_MODE_WARM_UP:
      MSG_MED("Start on demand Warm Up Timer", 0,0,0);
      (void)os_TimerStart(tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_mode_timer_ptr, 
                          TM_ON_DEMAND_CHARGER_POLL_FIRST_TIMER_COUNT,
                          0);
      tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired = FALSE;
      break;
  }
  tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on = FALSE;
}


/*===========================================================================
FUNCTION tm_handle_on_demand_lpm_timer

DESCRIPTION this fucntion handles the timer expiry of the LPM timer. Upon timer
expiry it starts the next session

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_lpm_timer(void)
{
  tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired = TRUE;

  MSG_HIGH("On Demand starting next fix on timer expiry ", 0, 0, 0);
  tm_on_demand_start_on_demand_processing();
}


/*===========================================================================
FUNCTION tm_on_demand_wait_for_next_timeout

DESCRIPTION this fucntion waits for the next timeout to continue on-demand processing

DEPENDENCIES
  None.

RETURN VALUE
  None.

INPUT
   fix_fail is used to indicate if the num of failed fixes need to be incremented

SIDE EFFECTS
  None.
===========================================================================*/
void tm_on_demand_wait_for_next_timeout(boolean fix_fail)
{
  MSG_MED("Start on demand Wait for next timeout",0,0,0);
  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired == TRUE)
  {
    (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_mode_timer_ptr);
  }
  tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on = FALSE;
  if (fix_fail)
  {
    tm_standalone_sess_info.on_demand_sess_info.num_no_fixes++;
  }
  tm_on_demand_timer_start();

}


/*===========================================================================

FUNCTION tm_on_demand_ready_enable_handler

DESCRIPTION handle on demand ready mode enable or disable

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_start_on_demand_ready_processing(uint8 mode_type)
{
  tm_on_demand_event_type ret_val;
  MSG_MED("Start On Demand Ready Processing ",0,0,0);
  /* check if on demand positioning is running. if so stop it */

  if ( tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on == TRUE)
  {
    MSG_MED("Start On Demand Ready Processing. Ready mode already running ",0,0,0);
    /* we may be in ready mode because battery was charged. if the user enabled ready
    mode, then change the mode type and contine with the processing */
    tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = mode_type;
    return;
  }
  /* make sure no on-demand sessions are running */
  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on == TRUE)
  {
    MSG_MED("Start On Demand Ready Processing. LPM mode already running. Stopping it ",0,0,0);
    tm_on_demand_stop_on_demand_processing(TM_STOP_REASON_ON_DEMAND_READY_ENABLED);
  }
  /* check if we are allowed to run the ready mode */
  ret_val = tm_on_demand_check_gps_lpm_on_status();

  if (ret_val != TM_ON_DEMAND_EVENT_ERROR_NONE)
  {
    /* start the ready timer for the next timeout*/
    MSG_MED("Start On Demand Ready Processing. Lpm status returned %d. Wait for the next interval ",ret_val,0,0);
    tm_on_demand_ready_timer_start();
  }
  else
  {
    /* Instruct TM-Core to kick of sessions */
    tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = mode_type;
    sm_log_on_demand_start(tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type, 0,0);
    tm_on_demand_start_sess_req(NULL);
  }
}



/*===========================================================================

FUNCTION tm_on_demand_stop_on_demand_ready_processing

DESCRIPTION stop on demand ready positioning 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_stop_on_demand_ready_processing(tm_sess_stop_reason_e_type reason)
{
  MSG_MED("Stop On Demand Ready processing reason %d", reason, 0,0);

  tm_standalone_sess_info.on_demand_sess_info.num_no_fixes = 0;
  tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on = FALSE;
  tm_on_demand_stop_sess_req(reason);
}

/*===========================================================================

FUNCTION tm_on_demand_ready_enable_handler

DESCRIPTION handle on demand ready mode enable or disable

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_ready_enable_handler(boolean on_demand_ready_enabled, 
                                       tm_on_demand_event_type reason)
{

  MSG_MED("On Demand Ready mode enable handler. Ready mode status %d, ", on_demand_ready_enabled,0,0);
  if (tm_core_get_on_demand_state() == FALSE)
  {
    MSG_HIGH("On Demand positioning not enabled. Ready mode will not be turned on",0,0,0);
    return;
  }
  if (tm_core_get_gps_lock_state() == FALSE)
  {
    MSG_MED("On Demand positioning Ready mode not enabled as GPS is locked",0,0,0);
    return;
  }
  if (on_demand_ready_enabled)
  {
    tm_on_demand_start_on_demand_ready_processing(TM_ON_DEMAND_MODE_READY);
  }
  else
  {
    /* Check to see if we are stopping because application requested us to not be in ready mode. 
    if so, start 30 second timer and stop when timeout happens.
    else, if we are exiting ready mode because the charger disconnected, stop immediately. 
    What are there state variables that we need to set here? */
    switch (reason)
    {
      /* add reasons for stopping immediately here */
      case TM_ON_DEMAND_EVENT_CHARGER_DISCONNECTED: 
        /* let's stop the extended on timer if we have one running */
        if (tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr)
        {
          (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr);
        }
        tm_on_demand_stop_on_demand_ready_processing(TM_STOP_REASON_ON_DEMAND_CHARGER_DISCONNECTED);
        break;

        /* add reasons to enable extended on timer here */
      case TM_ON_DEMAND_EVENT_PARAMETER_CHANGED: 
        if (tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr)
        {
          MSG_MED("Extended ON Timer started \n", 0,0,0);
          (void) os_TimerStart(tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr, TM_ON_DEMAND_READY_MODE_EXTENDED_ON_TIMER_COUNT, 0);
        }
        break;

      case TM_ON_DEMAND_EVENT_TIMER_EXPIRY:
      default:
        /* let's stop the extended on timer if we have one running */
        if (tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr)
        {
          MSG_MED("Extended ON Timer stopped \n", 0,0,0);
          (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr);
        }
        tm_on_demand_ready_timer_stop();
        tm_on_demand_stop_on_demand_ready_processing(TM_STOP_REASON_ON_DEMAND_DISABLED);
        break;
    }
  }
}

/*===========================================================================

FUNCTION tm_on_demand_ready_timer_stop

DESCRIPTION   Stop the on-demand ready timer. this is usually done with MGP is turned
            ON
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_ready_timer_stop(void)
{

  MSG_MED("Stop on demand Ready Mode timer ",0,0,0);

  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired == FALSE)
  {
    tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired = TRUE;
    (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_mode_timer_ptr);
  }
}

/*===========================================================================

FUNCTION tm_on_demand_ready_timer_start

DESCRIPTION   Start the on-demand ready timer. this is usually done with MGP is turned
            OFF
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_ready_timer_start(void)
{

  MSG_MED("Start on demand Ready Mode timer ",0,0,0);
  /* if on-demand mode is enabled and ready mode is on, start the timer */
  if (( (tm_core_get_on_demand_state() == TRUE)
        &&(tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired == TRUE) ) || (tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on == TRUE))
  {
    (void)os_TimerStart(tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_mode_timer_ptr, 
                        1000,
                        0);
    tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired = FALSE;
  }
  tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on = FALSE;
}

/*===========================================================================
FUNCTION tm_handle_on_demand_ready_timer


DESCRIPTION this fucntion handles the timer expiry of the ready timer. Upon timer
expiry it starts the next session

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_ready_timer(void)
{

  tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired = TRUE;

  MSG_HIGH("On Demand READY starting next fix after timer expiry", 0, 0, 0);
  tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on = FALSE;
  tm_on_demand_ready_enable_handler(TRUE, TM_ON_DEMAND_EVENT_TIMER_EXPIRY);
}

/*===========================================================================
FUNCTION tm_handle_on_demand_charger_connected


DESCRIPTION this fucntion handles the battery charging event. In this case
 on demand positioning is started in LPM mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_charger_connected(void)
{

  MSG_MED("Start on demand Battery charging mode",0,0,0);
  
  /* if LPM or ready mode is already running, return */
  if ((tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on == TRUE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on == TRUE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired == FALSE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired == FALSE))
        
  {
    return;
  }
  //tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_BATTERY_CHARGING;
  //tm_on_demand_start_on_demand_processing();
}

/*===========================================================================
FUNCTION tm_handle_on_demand_battery_charged


DESCRIPTION this fucntion handles the battery charged event. In this case
 on demand positioning is started in ready mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_battery_charged(void)
{

  MSG_MED("Start on demand Battery charged mode",0,0,0);
  /* if LPM or ready mode is already running, return */
  if ((tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on == TRUE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on == TRUE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired == FALSE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired == FALSE))
        
  {
    return;
  }
  //tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_BATTERY_CHARGED;
  //tm_on_demand_start_on_demand_processing();
}


/*===========================================================================
FUNCTION tm_handle_on_demand_charger_disconnected


DESCRIPTION this fucntion handles the battery notcharging event. In this case
 on demand positioning is stopped.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_charger_disconnected(void)
{

  MSG_MED("Start on demand Battery charger disconnected mode",0,0,0);
  /* if LPM or ready mode is already running, return */
  if ((tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on == TRUE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on == TRUE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired == FALSE) ||
      (tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired == FALSE))
  {
    return;
  }
  if (tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_BATTERY_CHARGED ||
      tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type == TM_ON_DEMAND_MODE_BATTERY_CHARGING)
  {
    tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_1;
    tm_on_demand_start_on_demand_processing();
  }
  if (tm_core_get_on_demand_ready_state() == TRUE )
  {
    tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_READY;
    tm_on_demand_start_on_demand_ready_processing(TM_ON_DEMAND_MODE_BATTERY_CHARGED);
  }
}


/*
 ******************************************************************************
 *
 * tm_on_demand_charger_poll_stop
 *
 * Function description:
 *  This function stops the on-demand battery poll timer
 *  
 *
 * Parameters:
 *
 * Return value:
 *
 *
 ******************************************************************************
*/
void tm_on_demand_charger_poll_stop(void)
{
  MSG_MED("Stopping charger poll timers", 0,0,0);
  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr)
  {
    (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr);
  }

  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_poll_timer_ptr)
  {
    (void)os_TimerStop( tm_standalone_sess_info.on_demand_sess_info.on_demand_poll_timer_ptr); 
  }
}
/*
 ******************************************************************************
 *
 * tm_on_demand_charger_poll_init
 *
 * Function description:
 *  This function initializes the on-demand battery poll timer
 *  
 *
 * Parameters:
 *
 * Return value:
 *
 *  boolean: true if change in state
 *
 ******************************************************************************
*/
void tm_on_demand_charger_poll_init(void)
{
  os_TimerMsgType *first_start_timer_ptr = NULL;
  /* MSG_MED("tm_on_demand_charger_poll_init", 0, 0, 0); */

  if (tm_core_get_gps_lock_state() == FALSE)
  {
    MSG_MED("Charger Poll, On demand not enabled as GPS is locked",0,0,0);
    return;
  }                                          
  if (tm_core_get_on_demand_state() == FALSE)
  {
    MSG_MED("Charger Poll, On demand not enabled",0,0,0);
    return;
  }

  if (NULL == tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr)
  {
    /* Start deferrable timer */    
    first_start_timer_ptr =  os_TimerCreate(TM_STANDALONE_TIMER_ID_ON_DEMAND_FIRST_INIT, (uint32)THREAD_ID_SM_TM );
    tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr = first_start_timer_ptr;

  }
  else
  {
    first_start_timer_ptr = tm_standalone_sess_info.on_demand_sess_info.on_demand_first_init_timer_ptr;
  }

  if (NULL == tm_standalone_sess_info.on_demand_sess_info.on_demand_poll_timer_ptr)
  {
    /* Start deferrable timer */	  
    /* this time is started after the phone has initialized and we are done with initial startup session */
    tm_standalone_sess_info.on_demand_sess_info.on_demand_poll_timer_ptr = 
      os_TimerCreate(TM_STANDALONE_TIMER_ID_ON_DEMAND_CHARGER_POLL, (uint32)THREAD_ID_SM_TM );
  }

  /* have we already completed a session at first boot up? */
  if (on_demand_at_bootup_done == TRUE)
  {
    if (tm_standalone_sess_info.on_demand_sess_info.on_demand_poll_timer_ptr)
    {
      (void)os_TimerStart(tm_standalone_sess_info.on_demand_sess_info.on_demand_poll_timer_ptr, 
                          TM_ON_DEMAND_CHARGER_POLL_TIMER_COUNT, 0);  
    }
  }
  else /* this is the very first time we are starting, so wait for the rest of the phone to initialize first */
  {
    if (first_start_timer_ptr)
    {
      /* Start the poll timer for next kick. */
      (void)os_TimerStart(first_start_timer_ptr, TM_ON_DEMAND_CHARGER_POLL_FIRST_TIMER_COUNT, 0);  
    }
  }
  /* add other initializations here */

} /* end tm_on_demand_charger_poll_init() */

/*
 ******************************************************************************
 *
 * tm_on_demand_charger_poll
 *
 * Function description:
 *  This function polls battery charger and sends a message to SM_TM if state
 *  changes
 *
 * Parameters: 
 *  
 * Return value:
 *  
 *
 ******************************************************************************
*/

void tm_on_demand_charger_poll()
{
/* Algorithm: 
   send message if (OR clauses):
     1. new charging state is different from prev saved state (e.g. if (prev != chg_is_charging()))
     2. no change in charging, but battery is now fully charged but wasn't earlier
*/

  /* what was the charger state when last polled? */
  static tm_on_demand_charger_status  saved_charger_connected_state = TM_ON_DEMAND_CHARGER_STATUS_UNKNOWN; 

  /* is charger connected? */
  tm_on_demand_charger_status         new_charger_connected_state   = TM_ON_DEMAND_CHARGER_STATUS_UNKNOWN; 

  /* what was battery state when last polled? */
  static tm_on_demand_battery_status  saved_battery_state = TM_ON_DEMAND_BATTERY_STATE_UNKNOWN;           

  /* is battery fully charged? */
  tm_on_demand_battery_status         new_battery_state   = TM_ON_DEMAND_BATTERY_STATE_UNKNOWN;

  boolean        send_message                = FALSE;
  uint32         msg_type                    = TM_CORE_ON_DEMAND_MSG_ID_CHARGER_NOT_CONNECTED; 
  os_IpcMsgType* ipc_msg_ptr                 = NULL;


  new_charger_connected_state = tm_on_demand_get_charger_status(); /* is the charger connected? */
  if (saved_charger_connected_state != new_charger_connected_state) /* send a message if new state is different */
  {
    saved_charger_connected_state = new_charger_connected_state;
    saved_battery_state = TM_ON_DEMAND_BATTERY_STATE_UNKNOWN; /* we were just plugged-in or unplgugged , so reset our battery state*/
    msg_type = (TM_ON_DEMAND_CHARGER_STATUS_CONNECTED == saved_charger_connected_state) ? 
               TM_CORE_ON_DEMAND_MSG_ID_CHARGER_CONNECTED : 
               TM_CORE_ON_DEMAND_MSG_ID_CHARGER_NOT_CONNECTED;
    send_message = TRUE;
  }
  else if (TM_ON_DEMAND_CHARGER_STATUS_CONNECTED == saved_charger_connected_state)/* if we were already charging, then lets see if battery is fully charged*/
  {
    new_battery_state = tm_on_demand_get_battery_status(); /* is battery fully charged? */

    /* save state and send message only for the first time battery becomes fully charged*/
    if (new_battery_state != saved_battery_state)
    {
      saved_battery_state = new_battery_state; /* save state if different from prev */
      if (TM_ON_DEMAND_BATTERY_STATE_COMPLETELY_CHARGED == new_battery_state)
      {
        send_message = TRUE;
        msg_type = TM_CORE_ON_DEMAND_MSG_ID_BATTERY_FULLY_CHARGED;
      }
      /* else nothing 
         (assumption: battery can't spontaneously go from charged to uncharged when still charging) 
      */
    }
  }
  /* else no change */

  if (send_message) /* battery / charger state has changed, send a message */
  {
    ipc_msg_ptr = os_IpcCreate(sizeof(tm_on_demand_power_state),IPC_ALLOCATION_DYNAMIC, THREAD_ID_SM_TM);
    if (ipc_msg_ptr)
    {
      tm_on_demand_power_state* state_ptr = (tm_on_demand_power_state*)(ipc_msg_ptr->p_Data);
      ipc_msg_ptr->q_MsgId = msg_type;
      state_ptr->charger_status = saved_charger_connected_state;
      state_ptr->battery_status = saved_battery_state;

      if (!os_IpcSend(ipc_msg_ptr, THREAD_ID_SM_TM))
      {
        MSG_MED("tm_on_demand_charger_poll(): os_IpcSend() failed!", 0, 0, 0);
        if (!os_IpcDelete(ipc_msg_ptr))
        {
          MSG_MED("tm_on_demand_charger_poll(): os_IpcDelete() failed!", 0, 0, 0);
        }
      }
    } /* end if(ipc_msg_ptr) */
  } /* end if(send_message) */
  return ; 
} /* end tm_on_demand_charger_poll() */


/*
 ******************************************************************************
 *
 * tm_on_demand_get_charger_status
 *
 * Function description:
 *  This function gets the battery charger state
 *  changes. Abstracts tm_on_demand from charger API changes
 *
 * Parameters: 
 *  
 * Return value: tm_on_demand_charger_status
 *  
 *
 ******************************************************************************
*/
tm_on_demand_charger_status tm_on_demand_get_charger_status()
{
  tm_on_demand_charger_status status = TM_ON_DEMAND_CHARGER_STATUS_UNKNOWN;
#if defined(FEATURE_BATTERY_CHARGER)
  status = chg_is_charging() ? TM_ON_DEMAND_CHARGER_STATUS_CONNECTED
           : TM_ON_DEMAND_CHARGER_STATUS_NOT_CONNECTED;
#endif /* defined(FEATURE_BATTERY_CHARGER) */
  return status;
} /* end tm_on_demand_get_charger_status() */


/*
 ******************************************************************************
 *
 * tm_on_demand_get_battery_status
 *
 * Function description:
 *  This function gets battery charger state
 *  changes. Abstracts tm_on_demand from charger API changes
 *
 * Parameters: 
 *  
 * Return value:
 *  tm_on_demand_battery_status 
 *
 ******************************************************************************
*/

tm_on_demand_battery_status tm_on_demand_get_battery_status()
{
  tm_on_demand_battery_status status = TM_ON_DEMAND_BATTERY_STATE_UNKNOWN;
#if defined(FEATURE_BATTERY_CHARGER)
  /* kgc: also consider chg_ui_event_type chg_ui_event_read(void);*/
  switch (chg_ui_event_read())
  {
    case CHG_UI_EVENT__DONE: 
      status = TM_ON_DEMAND_BATTERY_STATE_COMPLETELY_CHARGED;
      break;

    default:
      status = TM_ON_DEMAND_BATTERY_STATE_NOT_COMPLETELY_CHARGED;
      break;
  }

#endif /* defined(FEATURE_BATTERY_CHARGER) */

  return status;
} /* end tm_on_demand_get_battery_status() */



/*
 ******************************************************************************
 *
 * tm_on_demand_abort_handler
 *
 * Function description:
 *  Perform cleanups when on_demand is stopped
 *  
 *
 * Parameters: 
 *  
 * Return value:
 *  
 *
 ******************************************************************************
*/

void tm_on_demand_abort_handler(tm_sess_stop_reason_e_type reason)
{
  MSG_MED("On demand abort handler %d", reason,0,0);
  if (reason == TM_STOP_REASON_ON_DEMAND_CHARGER_DISCONNECTED)
  {
    tm_on_demand_ready_timer_stop();
    (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr);
  }
  else if (reason == TM_STOP_REASON_ON_DEMAND_OVERRIDE || reason == TM_STOP_REASON_ON_DEMAND_GPS_LOCKED)
  {
    
    tm_on_demand_charger_poll_stop(); 
    tm_on_demand_timer_stop();
    tm_on_demand_ready_timer_stop();
    (void)os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr);
  }


  if (reason == TM_STOP_REASON_ON_DEMAND_DISABLED || reason == TM_STOP_REASON_ON_DEMAND_OVERRIDE ||
      reason == TM_STOP_REASON_ON_DEMAND_DELETE_PARAMS || reason == TM_STOP_REASON_ON_RESET_LOCATION_SERVICE)
  {
    /* clean up all the state variables*/
    tm_standalone_sess_info.on_demand_sess_info.num_no_fixes = 0;

    /* stop all timers if ON demand is disabled. */    
    if (tm_core_get_on_demand_state() == FALSE)
    {
      tm_on_demand_charger_poll_stop(); 
      tm_on_demand_timer_stop();
      tm_on_demand_ready_timer_stop();
      (void) os_TimerStop(tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr);
    }
    tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_lpm_timer_expired = TRUE;
    tm_standalone_sess_info.on_demand_sess_info.on_demand_tracking_ready_timer_expired = TRUE;
    tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on = FALSE;
    tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on = FALSE;
  }
}


/*
 ******************************************************************************
 *
 * tm_on_demand_xlate_stop_reason_to_on_demand_reason
 *
 * Function description:
 *  Perform cleanups when on_demand is stopped
 *  
 *
 * Parameters: 
 *  
 * Return value:
 *  
 *
 ******************************************************************************
*/
void tm_on_demand_xlate_stop_reason_to_on_demand_reason
      (tm_sess_stop_reason_e_type reason, tm_on_demand_event_type *event)
{
  switch (reason)
  {
    case TM_STOP_REASON_COMPLETED:
      *event = TM_ON_DEMAND_EVENT_FIX_OBTAINED;
      break;
      
    case TM_STOP_REASON_ON_DEMAND_OVERRIDE:
      *event = TM_ON_DEMAND_EVENT_OVERRIDE;
      break;
      
    case TM_STOP_REASON_ON_DEMAND_DISABLED:
      *event = TM_ON_DEMAND_EVENT_DISABLED; 
      break;
    case TM_STOP_REASON_ON_DEMAND_READY_ENABLED:
      *event = TM_ON_DEMAND_EVENT_ENABLED;
      break;
    case TM_STOP_REASON_ON_DEMAND_SESSION_IN_PROGRESS:
      *event = TM_ON_DEMAND_EVENT_ERROR_GPS_SESSION_ACTIVE;
      break;
    case TM_STOP_REASON_ON_DEMAND_CHARGER_DISCONNECTED:
      *event = TM_ON_DEMAND_EVENT_CHARGER_DISCONNECTED;
      break;
    default:
      *event = TM_ON_DEMAND_EVENT_ERROR_NONE;
      break;
  }
  
}


/*
 ******************************************************************************
 *
 * tm_on_demand_log_done_event
 *
 * Function description:
 *  Perform cleanups when on_demand is stopped
 *  
 *
 * Parameters: 
 *  
 * Return value:
 *  
 *
 ******************************************************************************
*/
void tm_on_demand_log_done_event(tm_sess_stop_reason_e_type reason)
{
  
  tm_on_demand_event_type event = TM_ON_DEMAND_EVENT_ERROR_NONE;
  tm_on_demand_xlate_stop_reason_to_on_demand_reason(reason, &event);
  sm_log_on_demand_stop(tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type,
          event,
          tm_standalone_sess_info.on_demand_sess_info.num_no_fixes);
  sm_report_event_gps_on_demand_mode_done(reason);
}



/*===========================================================================

FUNCTION tm_core_handle_on_demand_sess_start

DESCRIPTION
  This function calls the SysD controller to start an ODP session

RETURN VALUE
  uint8 call flow select mask

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_handle_on_demand_sess_start(void* ipc_msg_ptr)
{
  
  sysd_cntrl_func_packet_type sysd_cntrl_payload;
  tm_pdsm_cmd_s_type *p_pdsm_cmd_type = (tm_pdsm_cmd_s_type*)(ipc_msg_ptr);
  
  sysd_cntrl_payload.event = SYSD_CNTRL_EVT_GET_POS;
  tm_core_info.qwip_sysd_triggered = TRUE;
  
  sysd_cntrl_payload.req_type = SYSD_CNTRL_POS_REQ_TYPE_ODP;
  sysd_cntrl_payload.cmd.tm_pdsm_cmd_type = p_pdsm_cmd_type;
  sysd_cntrl_payload.cmd.tm_pdsm_cmd_type->pdsm_cmd_type.cmd.pd.client_id = 3333;

  tm_sysd_controller_handler(&sysd_cntrl_payload);
}

/*===========================================================================

FUNCTION tm_on_demand_get_punc_tunc

DESCRIPTION
  This function returns the PUNC tunc values

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_get_punc_tunc(FLT *p_punc, FLT *p_tunc)
{
  if (z_PuncTunc.b_Valid)
  {
    *p_punc = z_PuncTunc.f_Punc;
    *p_tunc = z_PuncTunc.f_Tunc;
  }
  else
  {
    *p_punc = 0;
    *p_tunc = 0;
  }
 
}
