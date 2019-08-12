/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Standalone Sub-module

General Description
  This file contains implementations for TM Diag Interface

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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_standalone.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/19/15   rk      Continue with standalone session when the RAT isn't in full service to provide the AD. 
03/14/15   rh      Added support for MultiSIM NV 
12/06/13   ah      Support for non-deferrable timers
06/11/13   mj      Decouple calling qwip through standalone protocol
09/30/09   gk      Support ODP 2.0
07/30/09   gk      ODP 2.0 support
07/25/08   gk      session cancellation
07/09/09   gk      ODP 1.1 related fixes for warmup
04/02/09   ns      Send fix_timeout to sysd
01/25/09   ns      Added support for QWip
04/25/08   gk      On-Demand minor tweaks
06/09/08   gk      On-Demand Changes to add some logs and events
04/25/08   gk      On-Demand Change
08/01/07   rw      Add support for XTRA feature
07/30/07   ank     Added support for ext_pos_data in Position Report.

05/27/07   rw      Add support for JGPS (KDDI) module
05/09/07   ank     Changed prototype of tm_util_pd_info_data_init
10/16/06   cl      Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "math.h"
#include "msg.h"
#include "aries_os_api.h"
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
#include "tm_data.h"

#ifdef FEATURE_CGPS_XTRA
#include "tm_xtra.h"
#endif /* FEATURE_CGPS_XTRA */

#include "tm_on_demand.h"

#include "tm_qwip_core.h"
#include "tm_common.h"

extern tm_standalone_sess_info_s_type tm_standalone_sess_info;
pdsm_pd_dload_data_s_type      tm_standalone_dload_info;
pdsm_pd_info_s_type            tm_standalone_info_buffer;
tm_pd_position_info_s_type     tm_standalone_pos_info;
tm_ext_pos_data_s_type         tm_standalone_ext_pos_data;


/* Extra time (in seconds) when start LR timer */
#define TM_STANDALONE_TIMEOUT_EXTRA  2


/*===========================================================================

FUNCTION tm_standalone_sess_termination

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_sess_termination(tm_sess_stop_reason_e_type reason)
{
  tm_sess_req_param_u_type   sessReqParam;
#ifdef FEATURE_CGPS_ODP_2_0
  sysd_cntrl_func_packet_type sysd_cntrl_payload;
#endif  /* ifdef FEATURE_CGPS_ODP_2_0 */
  MSG_HIGH("Standalone session termination reason %d", reason, 0,0);
  sessReqParam.stop_param.stop_type   = TM_STOP_TYPE_SESSION;
  sessReqParam.stop_param.stop_reason = reason;
#ifdef FEATURE_CGPS_ODP_2_0
  if ((tm_standalone_sess_info.sess_handle == TM_SESS_HANDLE_ON_DEMAND) &&
           (TRUE == tm_core_info.qwip_sysd_triggered))
  {
    
    pdsm_cmd_s_type   z_cmd_type;
    /* Turn off WiFi immediately for ODP sessions */
    sysd_cntrl_payload.event = SYSD_CNTRL_EVT_SESS_END;

    sysd_cntrl_payload.cmd.pdsm_cmd_type = &z_cmd_type;
    sysd_cntrl_payload.cmd.pdsm_cmd_type->cmd.pd.cmd = PDSM_PD_CMD_END_SESSION;
    sysd_cntrl_payload.cmd.pdsm_cmd_type->cmd.pd.client_id = TM_ODP_CLIENT_ID;
    sysd_cntrl_payload.req_type = SYSD_CNTRL_POS_REQ_TYPE_ODP;
    tm_sysd_controller_handler(&sysd_cntrl_payload);
  }
#endif  /* ifdef FEATURE_CGPS_ODP_2_0 */
  /* Terminate session to TM-Core */
  (void)tm_sess_req(TM_PRTL_TYPE_STANDALONE,
                    tm_standalone_sess_info.sess_handle,
                    TM_SESS_REQ_STOP,
                    &sessReqParam);

  (void)os_TimerStop(tm_standalone_sess_info.sess_timer_ptr);

  tm_standalone_sess_info.sess_handle   = 0;
  tm_standalone_sess_info.session_state = TM_STANDALONE_SESS_STATE_INIT;
  /* if it is an on-demand session and fix was not successful, increment the number of no-fixes
     the num of no fixes will be used to determine if we move to a new mode */
  if (reason == TM_STOP_REASON_COMPLETED)
  {
    tm_standalone_sess_info.on_demand_sess_info.num_no_fixes = 0;
  }
  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on == TRUE)
  {
    if (reason != TM_STOP_REASON_COMPLETED)
    {
      tm_standalone_sess_info.on_demand_sess_info.num_no_fixes++;
    }
    tm_on_demand_log_done_event(reason);
  }
  /* start timer for the next on-demand ready session */
  if (tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on == TRUE)
  {
    tm_on_demand_log_done_event(reason);
    tm_on_demand_ready_timer_start();
  }

}

/*===========================================================================

FUNCTION tm_standalone_convert_op_mode

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  tm_sess_operation_mode_e_type

SIDE EFFECTS
  None.

===========================================================================*/
tm_sess_operation_mode_e_type tm_standalone_convert_op_mode(pdsm_pd_session_operation_e_type op_mode)
{
  switch(op_mode)
  {
    case PDSM_SESSION_OPERATION_STANDALONE_ONLY:
      return TM_SESS_OPERATION_MODE_STANDALONE;

    case PDSM_SESSION_OPERATION_MSBASED:
      return TM_SESS_OPERATION_MODE_MSB;

    default:
      MSG_MED("set to default operation mode", 0, 0, 0);
      return TM_SESS_OPERATION_MODE_STANDALONE;
  }
}


/*===========================================================================

FUNCTION tm_standalone_start_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.

===========================================================================*/
prtl_start_sess_req_result_e_type tm_standalone_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *action_ptr
)
{
  if((start_sess_req_param == NULL) || (action_ptr == NULL))
  {
    MSG_ERROR("Null start_sess_req_param or action_ptr", 0, 0, 0);
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

#ifdef FEATURE_CGPS_XTRA

  /* gpsOneXTRA feature enabled, check XTRA data to see if download request is necessary
     for standlaone session */
  if(PDSM_SESSION_OPERATION_STANDALONE_ONLY == start_sess_req_param->pd_option.operation_mode)
  {
    if(!tm_xtra_data_validity_check(GPS_TIME_CHECK_SOURCE_PER_FIX))
    {
#ifdef FEATURE_CGPS_XTRA_T
      if(!tm_xtra_invoke_xtrat(PDSM_XTRA_CMD_ERR_NONE))
      {
        MSG_ERROR("Failed to invoke XTRA-T", 0, 0, 0);
      }
#endif /* FEATURE_CGPS_XTRA_T */
    }
    else
    {
      // Successfully sent time request to MGP, clear the flag
      tm_core_info.session_info.req_xtra_trig_by_agps = FALSE;
    }
  }
#endif /* FEATURE_CGPS_XTRA */

  /* Update Standalone Session Info */
  tm_standalone_sess_info.sess_handle   = sess_handle;
  tm_standalone_sess_info.session_state = TM_STANDALONE_SESS_STATE_WAIT_WISHLIST_UPDATE;

  tm_standalone_sess_info.client_type                 = start_sess_req_param->client_type;
  tm_standalone_sess_info.client_id                   = start_sess_req_param->client_id;
  tm_standalone_sess_info.op_mode                     = tm_standalone_convert_op_mode(start_sess_req_param->pd_option.operation_mode);
  tm_standalone_sess_info.fix_rate.num_fixes          = start_sess_req_param->pd_option.fix_rate.num_fixes;
  tm_standalone_sess_info.fix_rate.time_between_fixes_ms =
		                                                start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;
  tm_standalone_sess_info.qos.accuracy_threshold      = start_sess_req_param->pd_qos.accuracy_threshold;
  tm_standalone_sess_info.qos.gps_session_timeout     = start_sess_req_param->pd_qos.gps_session_timeout;

  /* Instruct TM-Core to kick of sessions */
  action_ptr->op_mode   = tm_standalone_sess_info.op_mode;
  action_ptr->num_fixes = tm_standalone_sess_info.fix_rate.num_fixes;
  action_ptr->tbf_ms    = tm_standalone_sess_info.fix_rate.time_between_fixes_ms;
  action_ptr->op_req    = TM_OP_REQ_AIDING_DATA_STATUS;
  action_ptr->abort_curr_sess = FALSE;

  return PRTL_START_SESS_REQ_ALLOWED;
} /*lint !e818 */

/*===========================================================================

FUNCTION tm_standalone_sess_req_data_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)
{
  (void) session_handle;
  (void) req_data;

  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_standalone_resume_session

DESCRIPTION
  This function looks at aiding status and phone status to decide if standalone
  module should continue with the session or it should hand over the session to
  other umts/1x protcol sub-modules.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_resume_session(void)
{
  boolean need_dload = FALSE;
  boolean retVal;
  tm_sess_req_param_u_type       sessReqParam;
  tm_cm_phone_state_info_s_type  phone_state;
  #ifdef FEATURE_GNSS_LOW_MEMORY
    int                          prtl_tbl_idx = -1;
    tm_prtl_type                 active_prtl;
  #endif /* FEATURE_GNSS_LOW_MEMORY */

  tm_cm_iface_get_phone_state(&phone_state);

  /* Determine if assistance data is needed */
  if (phone_state.srv_system == CGPS_SRV_SYS_CDMA)
  {
    if ((tm_standalone_sess_info.aiding_data_status.eph_validity == FALSE) || 
        (tm_standalone_sess_info.aiding_data_status.pos_unc >= TM_LR_THRESHOLD_NO_POS))
    {
      need_dload = TRUE;
    }
  }
  else
  {
    if( (tm_standalone_sess_info.aiding_data_status.eph_validity == FALSE)
        ||(tm_standalone_sess_info.aiding_data_status.alm_validity == FALSE)
        ||(tm_standalone_sess_info.aiding_data_status.pos_unc >= TM_LR_THRESHOLD_NO_SEED_POS))
    {
      need_dload = TRUE;
    }
  }

  if ( ( tm_standalone_sess_info.aiding_data_status.time_unc == TM_TIME_THRESHOLD_NO_TIME ) &&
       ( ( phone_state.srv_system == CGPS_SRV_SYS_WCDMA ) ||
         ( phone_state.srv_system == CGPS_SRV_SYS_GSM )
       )
     )
  {
    need_dload = TRUE; /*lint !e506 */
  }

  /* Switch to 1x/umts protocol sub-modules only when all 3 conditions are satisfied
     1. Data download is needed
     2. MSB request
     3. Dload is possible (i.e not OoS)
     4. RAT is not in full service
  */

  if ( (phone_state.srv_system == CGPS_SRV_SYS_NONE) || (phone_state.srv_system == CGPS_SRV_SYS_OTHER) ||
       (phone_state.srv_status != SYS_SRV_STATUS_SRV) )
  {
    tm_standalone_sess_info.oos = TRUE;  
  }
  else
  {
    tm_standalone_sess_info.oos = FALSE;
  }

  /* If session originated as an 1x UP NI session, allow session to start and then resume from 1x
     in order to report LR data from to the network
  */
  if (tm_core_is_ni_standalone())
  {
	  MSG_MED("Standalone session initiated by 1x UP NI. Resume 1x to handle LR Reporting",0,0,0);
	  need_dload = TRUE;
  }
  #ifdef FEATURE_GNSS_LOW_MEMORY
  active_prtl = tm_core_get_active_prtl_module(MT_NA);
  prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);
  if (prtl_tbl_idx == -1)
  {
    if( (need_dload)
      &&((tm_standalone_sess_info.op_mode == TM_SESS_OPERATION_MODE_MSB) || (tm_standalone_sess_info.op_mode == TM_SESS_OPERATION_MODE_MSA))
      &&(tm_standalone_sess_info.oos != TRUE)) /*lint !e506  !e731 */
    {
      MSG_MED("NO AGPS protocol supported in this configuration. Fall back to Standalone",0,0,0);
      need_dload = FALSE;
    }
  }
  #endif

  if( (need_dload)
    &&(tm_standalone_sess_info.op_mode == TM_SESS_OPERATION_MODE_MSB)
    &&(tm_standalone_sess_info.oos != TRUE)) /*lint !e506  !e731 */
  {
    /* Clean up standalone module */
    tm_standalone_sess_termination(TM_STOP_REASON_MSB_NEED_DLOAD);
  }
  else
  {
    /* Let standalone module continue this session */
    /* Request LR now */
    sessReqParam.continue_param.op_mode            = tm_standalone_sess_info.op_mode;
    sessReqParam.continue_param.op_req             = TM_OP_REQ_LR;
    sessReqParam.continue_param.lr_qos             = tm_standalone_sess_info.qos.gps_session_timeout;
    sessReqParam.continue_param.accuracy_threshold = tm_standalone_sess_info.qos.accuracy_threshold;
    sessReqParam.continue_param.dynamic_qos_enabled = TRUE;

    retVal = tm_sess_req(TM_PRTL_TYPE_STANDALONE,
                         tm_standalone_sess_info.sess_handle,
                         TM_SESS_REQ_CONTINUE,
                         &sessReqParam);

    if(retVal == TRUE) /*lint !e506 !e731 */
    {
      /* Start session timeout timer and wait for LR */
      tm_standalone_sess_info.session_state = TM_STANDALONE_SESS_STATE_WAIT_LR;

      (void)os_TimerStart(tm_standalone_sess_info.sess_timer_ptr,
                         (tm_standalone_sess_info.qos.gps_session_timeout + TM_STANDALONE_TIMEOUT_EXTRA) * 1000,
                          0 );
    }
    else
    {
      /* Clean up standalone module */
      tm_standalone_sess_termination(TM_STOP_REASON_GENERAL_ERROR);
    }
  }
}

/*===========================================================================

FUNCTION tm_standalone_handle_wish_list_update

DESCRIPTION
#define C_GPS_ASSIS_REFTIME  (1U<<0)
#define C_GPS_ASSIS_REFLOC   (1U<<1)
#define C_GPS_ASSIS_ACQ      (1U<<2)
#define C_GPS_ASSIS_SA       (1U<<3)
#define C_GPS_ASSIS_NAVMODL  (1U<<4)
#define C_GPS_ASSIS_ALM      (1U<<5)
#define C_GPS_ASSIS_IONO     (1U<<6)
#define C_GPS_ASSIS_RTI      (1U<<7)
#define C_GPS_ASSIS_UTC      (1U<<8)

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_handle_wish_list_update(const tm_aiding_data_wishlist_s_type* aiding_data_status_ptr)
{
  tm_cm_phone_state_info_s_type phone_state_info;
  boolean aglo_ok_over_up = FALSE;

  if (aiding_data_status_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL ptr");
    return;
  }

#if 0
  /* MO, CP AGLO is not supported by any protocol at present. As and when this
   * support is introduced, this code can be enabled */
  boolean aglo_ok_over_cp = FALSE;
#endif

  tm_cm_iface_get_phone_state(&phone_state_info);

  aglo_ok_over_up = 
    /* If Glonass is enabled, AND */
    (tm_core_info.config_info.gnss_glo_control)

    && ( tm_core_info.config_info.u_mo_method == MO_UP )

    && (
        /* If AGLO over LPP UP is possible */
        ((TM_CORE_CONFIG_DD(asst_glonass_pos_protocol) & PDSM_PA_ASST_GLO_PROTOCOL_LPP_UP) &&
         (phone_state_info.srv_system == CGPS_SRV_SYS_LTE)) ||
        /* Or AGLO over WCDMA/GSM/TDSCDMA UP is possible */
        ((TM_CORE_CONFIG_DD(asst_glonass_pos_protocol) & PDSM_PA_ASST_GLO_PROTOCOL_RRLP_UP) &&
         ((phone_state_info.srv_system == CGPS_SRV_SYS_GSM)   ||
          (phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA) ||
          (phone_state_info.srv_system == CGPS_SRV_SYS_GW)    ||
          (phone_state_info.srv_system == CGPS_SRV_SYS_TDSCDMA)))
       );
#if 0
  aglo_ok_over_cp = 
    /* If Glonass is enabled, AND */
    (tm_core_info.config_info.gnss_glo_control)

    && ( tm_core_info.config_info.u_mo_method == MO_CP )

    && (
        /* AND AGLO over WCDMA CP is possible */
        ((TM_CORE_CONFIG_DD(asst_glonass_pos_protocol) & PDSM_PA_ASST_GLO_PROTOCOL_RRC_CP) &&
         (phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA)) ||
        /* Or MO over GSM is possible */
        ((TM_CORE_CONFIG_DD(asst_glonass_pos_protocol) & PDSM_PA_ASST_GLO_PROTOCOL_RRLP_CP) &&
         ((phone_state_info.srv_system == CGPS_SRV_SYS_GSM) ||
	  (phone_state_info.srv_system == CGPS_SRV_SYS_GW)))
        /* As MO over LPP CP is not possible, no need to check that */
       );
#endif
	
  /* Ephemeris */
  if(aiding_data_status_ptr->z_assist_data_status.q_WishListMask & C_GPS_ASSIS_NAVMODL)
  {
    MSG_MED("Eph invalid - GPS (GLO not chkd)",0,0,0);
    tm_standalone_sess_info.aiding_data_status.eph_validity = FALSE;
  }
  else
  {
    /* when CP AGLO becomes available, remove the MO_UP check */
    if (aglo_ok_over_up)
    {
      if(aiding_data_status_ptr->z_assist_data_status.z_GloAssistData.q_WishListMask & C_GPS_ASSIS_NAVMODL)
      {
        MSG_MED("Eph invalid - GLONASS",0,0,0);
        tm_standalone_sess_info.aiding_data_status.eph_validity = FALSE;
      }
      else
      {
        MSG_MED("Eph valid - GPS+GLONASS",0,0,0);
        tm_standalone_sess_info.aiding_data_status.eph_validity = TRUE;  /*lint !e506 */
      }
    }
    else
    {
      MSG_MED("Eph valid - GPS (GLO not supported)",0,0,0);
      tm_standalone_sess_info.aiding_data_status.eph_validity = TRUE;  /*lint !e506 */
    }
  }

  /* Almanac */
  if(aiding_data_status_ptr->z_assist_data_status.q_WishListMask & C_GPS_ASSIS_ALM)
  {
    MSG_MED("Alm invalid - GPS (GLO not chkd)",0,0,0);
    tm_standalone_sess_info.aiding_data_status.alm_validity = FALSE;
  }
  else
  {
    /* when CP AGLO becomes available, remove the MO_UP check */
    if (aglo_ok_over_up)
    {
      if(aiding_data_status_ptr->z_assist_data_status.z_GloAssistData.q_WishListMask & C_GPS_ASSIS_ALM)
      {
        MSG_MED("Alm invalid - GLONASS",0,0,0);
        tm_standalone_sess_info.aiding_data_status.alm_validity = FALSE;
      }
      else
      {
        MSG_MED("Alm valid - GPS+GLONASS",0,0,0);
        tm_standalone_sess_info.aiding_data_status.alm_validity = TRUE;  /*lint !e506 */
      }
    }
    else
    {
      MSG_MED("Alm valid - GPS (GLO not supported)",0,0,0);
      tm_standalone_sess_info.aiding_data_status.alm_validity = TRUE;  /*lint !e506 */
    }
  }

  /* Ref Location */
  if(aiding_data_status_ptr->z_assist_data_status.q_WishListMask & C_GPS_ASSIS_REFLOC)
  {
    MSG_MED("Ref Loc invalid",0,0,0);
    tm_standalone_sess_info.aiding_data_status.pos_unc = TM_LR_THRESHOLD_NO_POS;
  }
  else
  {
    /* Position Uncertainty */
    if(aiding_data_status_ptr->z_assist_data_status.z_PosUnc.u_Valid)
    {
      MSG_MED("Ref Loc valid",0,0,0);
      tm_standalone_sess_info.aiding_data_status.pos_unc = aiding_data_status_ptr->z_assist_data_status.z_PosUnc.f_PosUnc;
    }
    else
    {
      MSG_MED("Ref Loc invalid",0,0,0);
      tm_standalone_sess_info.aiding_data_status.pos_unc = TM_LR_THRESHOLD_NO_POS;
    }
  }

  MSG_MED("Pos Uncertainty %lu", (uint32) tm_standalone_sess_info.aiding_data_status.pos_unc,0,0);

  /* Ref Time */
  if(aiding_data_status_ptr->z_assist_data_status.q_WishListMask & C_GPS_ASSIS_REFTIME)
  {
    MSG_MED("Ref Time needed",0,0,0);
    tm_standalone_sess_info.aiding_data_status.time_unc = TM_TIME_THRESHOLD_NO_TIME;
  }
  else
  {
    /* Time Uncertainty */
    if(aiding_data_status_ptr->z_assist_data_status.z_TimeUnc.u_Valid)
    {
      tm_standalone_sess_info.aiding_data_status.time_unc = aiding_data_status_ptr->z_assist_data_status.z_TimeUnc.f_TimeUnc;
    }
    else
    {
      tm_standalone_sess_info.aiding_data_status.time_unc = TM_TIME_THRESHOLD_NO_TIME;
    }
  }

  MSG_MED("Time Uncertainty %lu", (uint32) tm_standalone_sess_info.aiding_data_status.time_unc,0,0);

  /* Now decide if we should let standalone module to continue
     or we should hand the session over to 1x or umts protocol sub-modules.
  */
  tm_standalone_resume_session();
}

/*===========================================================================

FUNCTION tm_standalone_handle_lr_data

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_handle_lr_data(sm_GnssFixRptStructType *lr_data, prtl_sess_info_e_type orig_sess_info_type)
{
  pdsm_pd_event_type pd_event;
  tm_lr_resp_type    *lr_resp_ptr = NULL;
  #if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)
  tm_1x_up_smod_req_status_e_type    req_status;
  #endif /* FEATURE_CGPS_CDMA_IF && ! FEATURE_GNSS_LOW_MEMORY */
  utc_time z_utc_time;
  boolean time_valid = FALSE;
  
  MSG_HIGH("Got LR",0, 0, 0);

  lr_resp_ptr = (tm_lr_resp_type *)pdsm_getbuf( sizeof(tm_lr_resp_type)); /*lint !e826 */

  if(lr_resp_ptr != NULL)
  {
    /* Generate CGPS event */
    sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE,0);

    //Convert from LM to IS801 format
    if(tm_util_convert_lr(lr_data,lr_resp_ptr))
    {
      //TODO is801_UtilLogRlinkMsg((void *)lr_resp_ptr, sizeof(is801_lr_resp_type));

      /* Now that Position info is available inform TM-Core about it.
      */

      tm_util_xlate_nav_soln_to_tm_pos_info( &tm_standalone_dload_info,
                                             &tm_standalone_pos_info,
                                             &lr_data->z_NavPos,
                                             PDSM_PD_DLOAD_LOCATION,
                                             PDSM_PD_DLOAD_SOURCE_STANDALONE);

      tm_standalone_pos_info.position_mode = PDSM_PD_POSITION_MODE_STANDALONE;

      tm_util_xlate_nav_soln_to_tm_ext_pos ( &tm_standalone_ext_pos_data,
                                             &lr_data->z_NavPos);

      /* TBD: Merged from 4.1 START - needs verification. */
      tm_standalone_ext_pos_data.h_dop = lr_data->z_NavPos.f_HDOP;
      tm_standalone_ext_pos_data.v_dop = lr_data->z_NavPos.f_VDOP;
      tm_standalone_ext_pos_data.p_dop = lr_data->z_NavPos.f_PDOP;;
      tm_standalone_ext_pos_data.pos_hepe =
         sqrt((lr_data->z_NavPos.f_ErrorEllipse[1] *
               lr_data->z_NavPos.f_ErrorEllipse[1]) +
              (lr_data->z_NavPos.f_ErrorEllipse[2] *
               lr_data->z_NavPos.f_ErrorEllipse[2]));

      MSG_HIGH("Time Source from PE: %d", lr_resp_ptr->time_src, 0, 0);
      if (PDSM_GNSS_TIME_GLO == lr_resp_ptr->time_src) {
        // No GPS time.
        tm_standalone_info_buffer.pd_info.pd_data.ext_opt_field_mask &= (~PDSM_PD_GPS_TIME_VALID);

        if(tm_util_glo_to_utc(lr_data->z_NavPos.u_GloNumFourYear,
                              lr_data->z_NavPos.w_GloNumDaysInFourYear,
                              lr_data->z_NavPos.q_GloTimeMs,
                              &z_utc_time))
        {
          tm_standalone_info_buffer.pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_UTC_TIME_VALID;

          tm_standalone_ext_pos_data.utc_time.hour = z_utc_time.q_hours;
          tm_standalone_ext_pos_data.utc_time.minute = z_utc_time.q_mins;
          tm_standalone_ext_pos_data.utc_time.sec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));

          tm_standalone_ext_pos_data.utc_time.date = 
            (uint32) ((z_utc_time.q_utc_days * 1000000) + 
            (z_utc_time.q_month * 10000) + z_utc_time.q_year);
        }
        else {
          tm_standalone_info_buffer.pd_info.pd_data.ext_opt_field_mask &= (~PDSM_PD_UTC_TIME_VALID);
          memset((void *)&tm_standalone_ext_pos_data.utc_time, 0, sizeof(pdsm_utc_time_s_type));
        }
      }
      else {

        z_utc_time = tm_util_construct_utc_time(lr_data->z_NavPos.w_GpsWeek,
                                                lr_data->z_NavPos.q_GpsTimeMs,
                                                lr_data->z_NavPos.q_UtcTimeMs);
        tm_standalone_ext_pos_data.utc_time.hour = z_utc_time.q_hours;
        tm_standalone_ext_pos_data.utc_time.minute = z_utc_time.q_mins;
        tm_standalone_ext_pos_data.utc_time.sec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));


        tm_standalone_ext_pos_data.utc_time.date = 
          (uint32) ((z_utc_time.q_utc_days * 1000000) + 
            (z_utc_time.q_month * 10000) + z_utc_time.q_year);
      }

      /* TBD: Merged from 4.1 END - needs verification. */
      pd_event = tm_util_pd_info_data_init ( &tm_standalone_info_buffer,
                                             lr_resp_ptr,
                                             tm_standalone_sess_info.client_id ,
                                             &tm_standalone_dload_info,
                                             &tm_standalone_pos_info,
                                             &tm_standalone_ext_pos_data);

      if (orig_sess_info_type != PRTL_SESS_INFO_LR)
      {
        /* If this was a WiFi only fix, the original session info type would have been changed.
           Check to see if the GPS and UTC time should be valid */
        time_valid = tm_core_time_validity();
        if (time_valid == TRUE)
        {
          tm_standalone_info_buffer.pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_GPS_TIME_VALID;
          tm_standalone_info_buffer.pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_UTC_TIME_VALID;
        }
        else
        {
          tm_standalone_info_buffer.pd_info.pd_data.ext_opt_field_mask &= (~PDSM_PD_GPS_TIME_VALID);
          tm_standalone_info_buffer.pd_info.pd_data.ext_opt_field_mask &= (~PDSM_PD_UTC_TIME_VALID);
          tm_standalone_info_buffer.pd_info.pd_data.gpsUtcOffset = (int8)cgps_GetNvUtcOffset();
        }
      }

      /* clear the num_data for next fix.
      */
      tm_standalone_dload_info.num_data = 0;

      /* Query SMOD iface to determine if 1x UP can post event to TM-Core */
      #if defined(FEATURE_CGPS_CDMA_IF) && !defined(FEATURE_GNSS_LOW_MEMORY)
      if( TM_1X_UP_SMOD_REQ_SUCCESS ==
                     tm_1x_up_smod_info_available(&tm_standalone_info_buffer,
                                                  PDSM_PD_END_SESS_NONE,
                                                  NULL,
                                                  &req_status)
        )
      #endif /* FEATURE_CGPS_CDMA_IF && !FEATURE_GNSS_LOW_MEMORY */
      {
        /* Fire Postion PD API event */
        (void)tm_post_pd_event(TM_PRTL_TYPE_STANDALONE,
                               tm_standalone_sess_info.sess_handle,
                               pd_event,
                               &tm_standalone_info_buffer,
                               &lr_data->z_NavPos.z_PosFlags);
      }
    }
    (void)pdsm_freebuf((char *)lr_resp_ptr);
  }
  /* Clean up standalone module */
  tm_standalone_sess_termination(TM_STOP_REASON_COMPLETED);
}

/*===========================================================================

FUNCTION tm_standalone_sess_info_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_sess_info_handler
(
  tm_sess_handle_type                session_handle,
  prtl_sess_info_e_type              sess_info_type,
  prtl_sess_info_param_u_type        *sess_info_param,
  prtl_sess_info_e_type              orig_sess_info_type
)
{
  if(sess_info_param == NULL)
  {
    MSG_ERROR("Null sess_info_param pointer", 0, 0, 0);
    return FALSE;
  }

  if(session_handle == tm_standalone_sess_info.sess_handle)
  {
   switch(sess_info_type)
   {
     case PRTL_SESS_INFO_STATUS:
       tm_standalone_handle_wish_list_update(&sess_info_param->aiding_data_status);
       break;

     case PRTL_SESS_INFO_LR:
       tm_standalone_handle_lr_data(&sess_info_param->lr_data, orig_sess_info_type);
       break;

     default:
       break;
   }
   return TRUE; /*lint !e506 */
  }
  else
  {
    MSG_ERROR("Incorrect sess_handle", 0, 0, 0);
    return FALSE;
  }
}

/*===========================================================================

FUNCTION tm_sysd_standalone_sess_info_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_sysd_standalone_sess_info_handler
(
  tm_sess_handle_type                session_handle,
  prtl_sess_info_e_type              sess_info_type,
  prtl_sess_info_param_u_type        *sess_info_param
)
{
  boolean ret_val;
  prtl_sess_info_e_type orig_sess_info_type = sess_info_type;

  if ((tm_core_info.qwip_sysd_triggered) &&
  	  (PRTL_SESS_INFO_DIAG_INFO == sess_info_type))
  {
    MSG_HIGH("QWiP: Trigger GPS Fix info (type %d) to TSG",sess_info_type,0,0);
    tm_sysd_inject_gnss_lr_data(sess_info_param,&sess_info_type);
  }

  ret_val = tm_standalone_sess_info_handler(session_handle, sess_info_type,sess_info_param, orig_sess_info_type);
  return ret_val;
}

/*===========================================================================

FUNCTION tm_standalone_stop_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,
  tm_sess_stop_reason_e_type    stop_reason
)
{
  boolean ret_val = FALSE;
  prtl_sess_info_param_u_type  *sess_info_param_ptr = NULL;
  
#ifdef FEATURE_CGPS_ODP_2_0
  sysd_cntrl_func_packet_type sysd_cntrl_payload;
  sysd_cntrl_payload.event = SYSD_CNTRL_MAX_EVTS;
#endif  /* ifdef FEATURE_CGPS_ODP_2_0 */

  (void)stop_type;
  if (sess_handle == TM_SESS_HANDLE_NONE)
  {
    return TRUE ;
  }

  if(sess_handle == tm_standalone_sess_info.sess_handle)
  {
    if((TM_STOP_REASON_FIX_TIMEOUT == stop_reason) &&
       (TRUE== tm_core_info.qwip_sysd_triggered))
    {
      sess_info_param_ptr = (prtl_sess_info_param_u_type *)pdsm_getbuf(sizeof(prtl_sess_info_param_u_type));
      if(tm_sysd_handle_gnss_fix_timeout(&(sess_info_param_ptr->lr_data)))
      {
        tm_standalone_sess_termination(TM_STOP_REASON_COMPLETED);  
        ret_val = TRUE; /* NSTODO : Check??*/
      }
      else
      {
        tm_standalone_sess_termination(stop_reason);
        ret_val = TRUE;
      }
      (void)pdsm_freebuf((char *)sess_info_param_ptr);
    }
#ifdef FEATURE_CGPS_ODP_2_0
    else if ((sess_handle == TM_SESS_HANDLE_ON_DEMAND) &&
             (TRUE == tm_core_info.qwip_sysd_triggered))
    {

      /* Turn off WiFi immediately for ODP sessions */
      sysd_cntrl_payload.event = SYSD_CNTRL_EVT_SESS_END;
      
      sysd_cntrl_payload.cmd.pdsm_cmd_type->cmd.pd.cmd = PDSM_PD_CMD_END_SESSION;
      sysd_cntrl_payload.cmd.pdsm_cmd_type->cmd.pd.client_id = TM_ODP_CLIENT_ID;
      sysd_cntrl_payload.req_type = SYSD_CNTRL_POS_REQ_TYPE_ODP;
      tm_sysd_controller_handler(&sysd_cntrl_payload);
      tm_standalone_sess_termination(stop_reason);
    }
#endif /* FEATURE_CGPS_ODP_2_0 */
    else /* Stop reason not Fix Timeout*/
    {
      tm_standalone_sess_termination(stop_reason);
      ret_val = TRUE;
    }
  }
  else
  {
    MSG_ERROR("Can not match handle tm-core = %d, standalone = %d",
              sess_handle,
              tm_standalone_sess_info.sess_handle,
              0);
    ret_val = FALSE;
  }
  return (ret_val);
}

#ifdef FEATURE_CGPS_PDCOMM


/*===========================================================================

FUNCTION tm_standalone_data_pipe_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_data_pipe_handler
(
  pd_comms_client_ipc_message_type        msg_type,
  void                                    *msg_data_ptr
)
{
  (void)msg_type;
  (void)msg_data_ptr;

  return TRUE; /*lint !e506 */
}
#endif

/*===========================================================================

FUNCTION tm_standalone_timer_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_timer_cb_handler
(
  void *timer_data
)
{
  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;

  if(timer_param != NULL)
  {
    switch(timer_param->q_TimerId)
    {
      case TM_STANALONE_TIMER_ID_SESSION:
        tm_standalone_sess_termination(TM_STOP_REASON_GENERAL_ERROR);
      break;

    case TM_STANDALONE_TIMER_ID_ON_DEMAND_LPM:
      tm_handle_on_demand_lpm_timer();
      break;
    case TM_STANDALONE_TIMER_ID_ON_DEMAND_READY:
      tm_handle_on_demand_ready_timer();
      break;

    case TM_STANDALONE_TIMER_ID_ON_DEMAND_FIRST_INIT:
       tm_on_demand_charger_poll_timer_handler(timer_param);
       break;

    case TM_STANDALONE_TIMER_ID_ON_DEMAND_CHARGER_POLL: /* time to poll charger */
       tm_on_demand_charger_poll_timer_handler(timer_param);
       break;

    case TM_STANDALONE_TIMER_ID_ON_DEMAND_READY_MODE_EXTENDED_ON: /* time to turn off extended on */
       tm_on_demand_ready_enable_handler(FALSE, TM_ON_DEMAND_EVENT_TIMER_EXPIRY);
       break;
      default:
        MSG_ERROR("Uknown timer expired", 0, 0, 0);
      break;
    }
  }
  else
  {
    MSG_ERROR("Can not proess null timer msg", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION tm_standalone_event_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *event_payload_ptr
)
{

  /* check for null payload ptr */
  if (event_payload_ptr == NULL)
  {
    return FALSE;
  }
  switch (event_type)
  {
  case PRTL_EVENT_TYPE_PA:
    if (event_payload_ptr->pa_event_payload.pa_event == PDSM_PA_ON_DEMAND_LOW_POWER_MODE )
    {
      MSG_HIGH("TM_STANDALONE: On Demand LPM parameter changed \n", event_payload_ptr->pa_event_payload.pa_info_ptr->on_demand_lpm, 0,0);
      tm_on_demand_lpm_enable_handler(event_payload_ptr->pa_event_payload.pa_info_ptr->on_demand_lpm);
    }
    if (event_payload_ptr->pa_event_payload.pa_event == PDSM_PA_ON_DEMAND_READY_MODE)
    {

      MSG_HIGH("TM_STANDALONE: On-Demand Ready parameter changed \n", event_payload_ptr->pa_event_payload.pa_info_ptr->on_demand_ready, 0,0);
      tm_on_demand_ready_enable_handler(event_payload_ptr->pa_event_payload.pa_info_ptr->on_demand_ready, TM_ON_DEMAND_EVENT_PARAMETER_CHANGED);
    }
    break;
  case PRTL_EVENT_TYPE_ON_DEMAND_POS:
  {
    switch (event_payload_ptr->on_demand_event_payload.event_id)
    {
    case PRTL_ON_DEMAND_POS_PUNC_TUNC:
      tm_trigger_on_demand_lpm(&event_payload_ptr->on_demand_event_payload.event_payload.pos_punc_tunc);
      break;
    case PRTL_ON_DEMAND_RCVR_STATE:
      if (event_payload_ptr->on_demand_event_payload.event_payload.rcvr_state == TRUE)
      {
        /* when the receiver is on, turn off all the on-demand timers */
        //tm_on_demand_abort_handler(TM_STOP_REASON_ON_DEMAND_SESSION_IN_PROGRESS);
      }
      else
      {
        /* when the receiver is off, start on-demand timers */
        if (tm_core_get_on_demand_ready_state() == TRUE)
        {
          /* if ready mode is enalbed, start the ready mode timers */
          tm_on_demand_ready_timer_start();
        }
        else if (tm_core_get_on_demand_state() == TRUE)
        {
          /* if on-demand lpm is enabled, start the lpm timers */
          tm_on_demand_timer_start();
          tm_on_demand_charger_poll_init();
        }
      }
      break;
    case PRTL_ON_DEMAND_BATTERY_STATE:
      switch (event_payload_ptr->on_demand_event_payload.event_payload.battery_state)
      {
        //todo-rename these events to match function names  call ondemand function directly
      case PRTL_EVENT_ON_DEMAND_CHARGER_CONNECTED:
        tm_handle_on_demand_charger_connected();
        break;
      case PRTL_EVENT_ON_DEMAND_CHARGER_DISCONNECTED:
        tm_handle_on_demand_charger_disconnected();
        break;
      case PRTL_EVENT_ON_DEMAND_CHARGER_DONE:
        tm_handle_on_demand_battery_charged();
        break;
      }
      break;
    case PRTL_ON_DEMAND_ABORT:
      tm_on_demand_abort_handler(event_payload_ptr->on_demand_event_payload.event_payload.reason);
      break;
    default:
      break;
    }
    break;
  }
  default:
    break;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_standalone_prtl_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_prtl_init(void)
{
  tm_prtl_cb_s_type prtlIfaceTbl;

  MSG_MED("tm_standalone_prtl_init", 0, 0, 0);

  /* Create Timers */
  tm_standalone_sess_info.sess_timer_ptr = os_TimerCreateInCgpsTimerGroup( (uint32)TM_STANALONE_TIMER_ID_SESSION,
                                                                           (uint32)THREAD_ID_SM_TM );

  /* Initialize function callback table for TM-Core */
  prtlIfaceTbl.start_sess_req_fp   = tm_standalone_start_sess_req_handler;
  prtlIfaceTbl.stop_sess_req_fp    = tm_standalone_stop_sess_req_handler;
  prtlIfaceTbl.sess_req_data_fp    = tm_standalone_sess_req_data_handler;

  /* Change the Session Info Handler - so that the fix information goes to QWiP SD */
  prtlIfaceTbl.sess_info_fp        = tm_sysd_standalone_sess_info_handler;

  #ifdef FEATURE_CGPS_PDCOMM
  prtlIfaceTbl.data_pipe_handle_fp = tm_standalone_data_pipe_handler;
  #endif
  prtlIfaceTbl.timer_cb_fp         = tm_standalone_timer_cb_handler;
  prtlIfaceTbl.event_cb_fp         = tm_standalone_event_cb_handler;
  prtlIfaceTbl.general_query_fp    = NULL;
  prtlIfaceTbl.gm_event_handle_fp  = NULL;

  tm_standalone_sess_info.on_demand_sess_info.num_no_fixes = 0;
  tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_on = FALSE;
  tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_on = FALSE;

  tm_standalone_sess_info.on_demand_sess_info.on_demand_lpm_mode_timer_ptr     = os_TimerCreateInCgpsTimerGroup((uint32)TM_STANDALONE_TIMER_ID_ON_DEMAND_LPM, (uint32)THREAD_ID_SM_TM);
  tm_standalone_sess_info.on_demand_sess_info.on_demand_ready_mode_timer_ptr   = os_TimerCreateInCgpsTimerGroup((uint32)TM_STANDALONE_TIMER_ID_ON_DEMAND_READY, (uint32)THREAD_ID_SM_TM);
  tm_standalone_sess_info.on_demand_sess_info.ready_mode_extended_on_timer_ptr = os_TimerCreateInCgpsTimerGroup((uint32)TM_STANDALONE_TIMER_ID_ON_DEMAND_READY_MODE_EXTENDED_ON, (uint32)THREAD_ID_SM_TM);

  /* on-demand feature is enabled */
  if (tm_core_get_on_demand_state() == TRUE)
  {
    /* on-demand feature is enabled in NV */
    tm_on_demand_charger_poll_init();
  }

  /* which mode are going to be in? */
  tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_1;
  (void)tm_prtl_reg(TM_PRTL_TYPE_STANDALONE, &prtlIfaceTbl);
}

/*
 ******************************************************************************
 *
 * tm_on_demand_charger_poll_timer_handler
 *
 * Function description:
 *  This function handles timers related to battery charger polling
 *
 * Parameters:
 *  timer_data : pointer to timer structure
 *
 * Return value:
 *
 *
 ******************************************************************************
*/
void tm_on_demand_charger_poll_timer_handler(void* timer_data)
{
   os_TimerExpiryType *pTimer = (os_TimerExpiryType *)timer_data;
   os_TimerMsgType *pPollTimer = tm_standalone_sess_info.on_demand_sess_info.on_demand_poll_timer_ptr;
   pdsm_client_id_type pdsm_client_id;
   if(pTimer != NULL)
   {
      MSG_MED("charger poll timerId 0x%x expired", pTimer->q_TimerId, 0, 0);
      switch(pTimer->q_TimerId)
      {

      case TM_STANDALONE_TIMER_ID_ON_DEMAND_FIRST_INIT:
         /* system should have initialized by now, so start poll timer */
        tm_standalone_sess_info.on_demand_sess_info.lpm_mode_type = TM_ON_DEMAND_MODE_WARM_UP;
        //tm_on_demand_start_sess_req();

        pdsm_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_ODP);
        if ( pdsm_client_id < 0 ) {
          MSG_MED("On demand not enabled because client cannot be created",0,0,0);
          //return;
        }
        else if ( PDSM_CLIENT_OK != pdsm_client_act(pdsm_client_id) ){
          MSG_MED("On demand not enabled because client cannot be activated",0,0,0);
          //return;
        }

        tm_on_demand_start_on_demand_processing();
         if(pPollTimer)
         {
            /* start timer with id TM_STANDALONE_TIMER_ID_ON_DEMAND_CHARGER_POLL so that
            we can regularly poll for battery charger status */
            (void)os_TimerStart(pPollTimer, TM_ON_DEMAND_CHARGER_POLL_TIMER_COUNT, 0);
         }
         else
         {
            MSG_MED("tm_on_demand_poll_charger_timer_handler(): poll timer is NULL!", 0, 0, 0);
         }
         break;

      case TM_STANDALONE_TIMER_ID_ON_DEMAND_CHARGER_POLL:
         tm_on_demand_charger_poll(); /* poll charger to see if phone is being charged */

         if(pPollTimer)
         {
            (void)os_TimerStart(pPollTimer, TM_ON_DEMAND_CHARGER_POLL_TIMER_COUNT, 0);  /* restart the timer */
         }
         else
         {
            MSG_MED("tm_on_demand_poll_charger_timer_handler(): poll timer is NULL!", 0, 0, 0);
         }
         break;

      default:
         MSG_MED("unhandled timer case in tm_on_demand_charger_poll_timer_handler()", 0, 0, 0);
         break;
      } /* end switch(pTimer->q_TimerId) */
   } /* end if(pTimer != NULL) */
   else
   {
      MSG_MED("poll_timer_handler(): pTimer is NULL!", 0, 0, 0);
   }
}

