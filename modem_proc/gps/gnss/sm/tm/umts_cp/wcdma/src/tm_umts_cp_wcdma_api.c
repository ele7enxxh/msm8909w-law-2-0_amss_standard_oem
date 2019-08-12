/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-CP-WCDMA API

GENERAL DESCRIPTION

  This file provides TM_UMTS_CP_WCDMA's APIs to TM_UMTS_CP.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/wcdma/src/tm_umts_cp_wcdma_api.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/16   rk      Pass all measurements to protocols. It will decide which measurements should be
                   reported to server.
10/29/15   gk      Stop sess should handle the reason E911 if another session starts
10/01/15   rk      Use internal API to know if A_GNSS position methods are supported.
07/03/15   jv      Added QZSS Support
06/09/15   mj      Add support for W+W feature
04/30/15   mj      Add support for early session start for WCDMA for CSFB during E911
12/06/13   ah      Support for non-deferrable timers
10/16/13   rk      Added RRC R8 A-GLO NILR MSA support.
08/23/09   gk      Added new event for guard timer expiry and MM ind
08/18/09   gk      Stop guard timer when end session is received.
07/23/09   gk      Add handling of MO_LR completed message
07/13/09   gk      Reset the allow RRC flag when end session is received
07/07/09   atien   Merge Liao's change for Case#180577 - MT-LR cancellation before MC
06/30/09   atien   add new function parameter to pass cf timer value in
                   tm_umts_cp_wcdma_set_cf_state
05/12/08   LT      If reference position is returned, no PDSM_PD_EVENT_END.
08/02/07   LT      Handling of SV-not-exist.
05/03/07   LT      Initial version

============================================================================*/
#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */
#ifdef FEATURE_CGPS_UMTS_CP_WCDMA

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
  #include "msg.h"
  #include <limits.h>
  #include "tm_common.h"
  #include "tm_prtl_iface.h"
  #include "tm_umts_common_utils.h"
  #include "pdapi.h"

  #include "tm_umts_cp_wcdma.h"
  #include "tm_umts_cp_ss.h"
  #include "tm_umts_cp_submodule_iface.h" /* function prototypes defined by umts_cp */
  #include "rrclsmif.h"

  #include "tm_cm_iface.h"

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
static void tm_umts_cp_wcdma_e911_sess_start();


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_start_sess_req_handler

DESCRIPTION
  This function handles the start session request from TM-core.

DEPENDENCIES: none.

RETURN VALUE: prtl_start_sess_req_result_e_type.

SIDE EFFECTS: none.

===========================================================================*/

prtl_start_sess_req_result_e_type tm_umts_cp_wcdma_start_sess_req_handler
(
  tm_sess_handle_type sess_handle,
  prtl_start_sess_req_param_s_type *start_sess_req_param,
  tm_sess_req_start_param_s_type *actions
 )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  molr_type_e_type mo_lr_type = LOCATION_ESTIMATE; /* enum LOCATION_ESTIMATE = 0 */
  prtl_start_sess_req_result_e_type ret_val = PRTL_START_SESS_REQ_ALLOWED;

  MSG_MED("UMTS-CP: start_sess_req_handler", 0, 0, 0);


  p_umts_cp_wcdma_session_status->tm_active_session_handle = sess_handle;

  if ( tm_umts_cp_wcdma_pdapi_mapping(start_sess_req_param) )
  {

    if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NONE )
    {
      tm_umts_cp_wcdma_callflow_kind_set(C_UMTS_CP_WCDMA_SESSION_UI);

      #ifdef FEATURE_EXTENDED_RESP_TIME
      {

        /* extended_reporting_interval() is an OEM provided function
           that provides extended reporting time (used for network intiated) and
           additional_request_margin which is the number of seconds before
           RRC's ReportingIntervalLong expires, at which UE sends UEP error
           assistanceDataMissing asking for IONO; */

        /* NOTE: for MO, which is the case here, extended_ni_rep_time_sec is not needed,
           pdapi get_pos()'s QosPerformance will be used */
        tm_umts_cp_wcdma_extended_reporting_interval
           (&p_umts_cp_wcdma_session_status->u_extended_ni_resp_time_sec,
            &p_umts_cp_wcdma_session_status->q_additional_req_margin_sec
           );

      }
      #endif

    } /* if starting a session */

    if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_BASED )
    {

      /* if not UI, it would be a NI through an "ni-client" */
      if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
      {

        MSG_MED("MO-MB starts", 0, 0, 0);

        if ( z_umts_nv_item_ram_copy.mo_lr_support & AAGPS_NV_3G_MO_LR_ASSIST_DATA )
        {
          mo_lr_type = ASSISTANCE_DATA; /* AAGPS_NV_3G_MO_LR_ASSIST_DATA dictates the use of AssistData for MO-MB */
        }
        else
        {
          mo_lr_type = LOCATION_ESTIMATE; /* AAGPS_NV_3G_MO_LR_ASSIST_DATA dictates the use of LocEst for MO-MB */
        }
        p_umts_cp_wcdma_session_status->mo_lr_type = mo_lr_type;
      }

      actions->op_req = TM_OP_REQ_LR | TM_OP_REQ_AIDING_DATA_STATUS;
      actions->lr_qos = TM_MAX_SESSION_TIMEOUT; /* pdapi's max: 255  */
      actions->accuracy_threshold = p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters;
      actions->dynamic_qos_enabled = TRUE;

      actions->op_mode = TM_SESS_OPERATION_MODE_MSB;
      actions->num_fixes = start_sess_req_param->pd_option.fix_rate.num_fixes;
      actions->tbf_ms = start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;
    }

    else if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED )
    {
      /* if not UI, it would be a NI through an "ni-client" */
      if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
      {
        MSG_MED("MO-MA starts", 0, 0, 0);
        mo_lr_type = LOCATION_ESTIMATE; /* for UE-Assisted, this is the only type */
        p_umts_cp_wcdma_session_status->mo_lr_type = mo_lr_type;
      }

      actions->op_req = TM_OP_REQ_PRM;  /* receiver should be set to IDLE waiting for acq. assist. */
      actions->prm_qos = TM_MAX_SESSION_TIMEOUT; /* pdapi's max: 255  */
      actions->accuracy_threshold = p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters;
      actions->op_mode = TM_SESS_OPERATION_MODE_MSA;
      actions->num_fixes = start_sess_req_param->pd_option.fix_rate.num_fixes;
      actions->tbf_ms = start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;
      actions->multi_report_msa = FALSE;

      /* the wishlist setting is not absolutely needed as it may be sending LocEst. at this point  */
      p_umts_cp_wcdma_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_REFTIME;
      p_umts_cp_wcdma_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_ACQ;

    }

    else if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_CONV )
    {
      actions->op_req = TM_OP_REQ_LR;
      actions->lr_qos = p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs;
      actions->accuracy_threshold = p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters;
      actions->dynamic_qos_enabled = TRUE;

      actions->op_mode = TM_SESS_OPERATION_MODE_MSB;
      actions->num_fixes = start_sess_req_param->pd_option.fix_rate.num_fixes;
      actions->tbf_ms = start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;
    }


    if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
    {
      if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED )
      {
        /****************** if UI: request CM to send MOLR-Invoke *******************************************/
        if ( !tm_umts_cp_request_molr(start_sess_req_param, LOCATION_ESTIMATE, NULL) )
        {
          /* if failed, no hope of getting network support, tell TmCore request not allowed. */
          MSG_HIGH("Attempt sending MoLrInvoke failed, reject TmCore sess. req", 0, 0, 0);
          ret_val = PRTL_START_SESS_REQ_NOT_ALLOWED;
        }
        else
        {
          p_umts_cp_wcdma_session_status->u_network_session_on = TRUE;
        }
      } /* if not-standalone */
    } /* if UI */

  } /* if mapping from pdapi parameters successful */

  else
  {
    /* if the above function returns failure, UMTS-CP-WCDMA has no valid mode for the pdapi
       request, disallow the session */
    MSG_HIGH("GPS mode from TmCore invalid, reject TmCore sess. req", 0, 0, 0);
    ret_val = PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  if ( ret_val == PRTL_START_SESS_REQ_ALLOWED )
  {
    /* upon TmCore getting TRUE, a TmCore session will be on */
    p_umts_cp_wcdma_session_status->tm_core_session_started = TRUE;

	#ifdef FEATURE_CGPS_DUAL_WCDMA
	/* Set DD as_id as the as_id for this session */
	if (!p_umts_cp_wcdma_session_status->z_rrc_as_id_info.u_rrc_event_as_id_received)
	{
	  p_umts_cp_wcdma_session_status->z_rrc_as_id_info.e_cached_rrc_as_id = tm_core_get_current_dd_subs_id();
	  p_umts_cp_wcdma_session_status->z_rrc_as_id_info.u_rrc_event_as_id_received = TRUE;
	  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: AS_ID cached to %d for MO-CP session", (uint8)p_umts_cp_wcdma_session_status->z_rrc_as_id_info.e_cached_rrc_as_id);
	}
	#endif
  }

  return ( ret_val );

}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_stop_sess_req_handler

DESCRIPTION
  This function handles the stop session request from TM-core.

DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_wcdma_stop_sess_req_handler
(
  tm_sess_handle_type sess_handle,
  prtl_sess_stop_type stop_type,
  tm_sess_stop_reason_e_type stop_reason
 )
{
  rrc_positioning_error_cause e_rrc_pos_error_cause = RRC_UNDEFINED_ERROR;
  pdsm_pd_end_e_type err_type = PDSM_PD_END_UNKNWN_SYS_ERROR;
  #ifdef FEATURE_RET_REFPOS_UPON_NOFIX
  boolean u_ret_ref_pos = FALSE;
  #endif
  boolean u_ret_val = FALSE;


  if ( sess_handle == TM_SESS_HANDLE_NONE )
  {
    /* terminate the guard timer IF such timer is running */
    tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME);
    z_umts_cp_wcdma_session_status.allow_rrc = z_umts_nv_item_ram_copy.default_allow_rrc;
    u_ret_val = TRUE;
    return ( u_ret_val );
  }

  if ( ((sess_handle == TM_SESS_HANDLE_E911) &&
       (stop_reason == TM_STOP_REASON_E911_OVERRIDE) ) ||
	   (TM_STOP_REASON_E911 == stop_reason))
  {
    MSG_HIGH("CP-WCDMA: Another emergency session started. clean up WCDMA_SESS ", 0, 0, 0);
    #ifdef FEATURE_CGPS_DUAL_WCDMA
    tm_umts_common_utils_mm_session_status_notify(FALSE, z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id);
	#else
    tm_umts_common_utils_mm_session_status_notify(FALSE);
	#endif
    tm_umts_cp_wcdma_session_cleanup();
    tm_umts_cp_submodule_end_session();
    u_ret_val = TRUE;
    return ( u_ret_val );
  }
  /* For low priority emergency support NV item setting allow a new E911 call to
     continue with the existing E911 session. */
  if ( (stop_reason == TM_STOP_REASON_E911) &&
       (tm_umts_common_utils_emergency_effect() == C_EMERGENCY_HANDLING_ACCEPT_ALL) )
  {
    MSG_HIGH("CP-WCDMA: Sess Stop req rejected due to NV E911 support setting", 0, 0, 0);
    u_ret_val = FALSE;
    return ( u_ret_val );
  }

  #ifdef FEATURE_CONTINUE_MTLR_SESSION
  /* If an MT-LR session is in progress and this is an abort attempt due to
     a new E911 voice call, then continue with the existing MT-LR session. */
  if ( (stop_reason == TM_STOP_REASON_E911) &&
       (z_umts_cp_wcdma_session_status.u_mtlr_in_progress) )
  {
    MSG_HIGH("CP-WCDMA: MT-LR sess in progress. Stop attempt rejected ", 0, 0, 0);
    u_ret_val = FALSE;
    return ( u_ret_val );
  }
  #endif

  if ( sess_handle == z_umts_cp_wcdma_session_status.tm_active_session_handle )
  {
    if ( z_umts_cp_wcdma_session_status.e_cf_state == C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC )
    {
      MSG_HIGH("EndSession: Clear assist req. holdoff times", 0, 0, 0);

      /* ending session while still waiting for MC */
      (void)os_TimerStop(cp_wcdma_alm_req_holdoff_timer);
      (void)os_TimerStop(cp_wcdma_eph_req_holdoff_timer);
      (void)os_TimerStop(cp_wcdma_iono_req_holdoff_timer);
      (void)os_TimerStop(cp_wcdma_utc_req_holdoff_timer);
      (void)os_TimerStop(cp_wcdma_refloc_req_holdoff_timer);

      #ifdef FEATURE_GANSS_SUPPORT
      /* Glonass timers */
      (void)os_TimerStop(cp_wcdma_glo_eph_req_holdoff_timer);
      (void)os_TimerStop(cp_wcdma_glo_alm_req_holdoff_timer);
      (void)os_TimerStop(cp_wcdma_glo_auxi_info_req_holdoff_timer);
      (void)os_TimerStop(cp_wcdma_glo_time_model_req_holdoff_timer);
      #endif
    }

    if ( stop_reason == TM_STOP_REASON_COMPLETED )
    {
      err_type = PDSM_PD_END_NORMAL_ENDING;
      u_ret_val = TRUE;
    }
    else if ( stop_reason == TM_STOP_REASON_GENERAL_ERROR )
    {
      err_type = PDSM_PD_END_UNKNWN_SYS_ERROR;
      u_ret_val = TRUE;
    }
    else if ( (stop_reason == TM_STOP_REASON_FIX_TIMEOUT) ||
              (stop_reason == TM_STOP_REASON_PRM_TIMEOUT)
             )
    {
      err_type = PDSM_PD_END_TIMEOUT;
      u_ret_val = TRUE;
    }
    else if ( stop_reason == TM_STOP_REASON_NONE )
    {
      err_type = PDSM_PD_END_SESS_NONE;
      u_ret_val = TRUE;
    }
    else if ( stop_reason == TM_STOP_REASON_PPM_TIMEOUT )
    {
      u_ret_val = FALSE; /* no such reason should ever reach SUPL */
    }
    else if ( stop_reason == TM_STOP_REASON_USER_TERMINATED )
    {
      err_type = PDSM_PD_END_CLIENT_END;
      u_ret_val = TRUE;
    }
    else if ( stop_reason == TM_STOP_REASON_ON_RESET_LOCATION_SERVICE )
    {
      err_type = PDSM_PD_END_RESET_LOCATION_SERVICE;
      u_ret_val = TRUE;
    }
    else if ( stop_reason == TM_STOP_REASON_E911 )
    {
      err_type = PDSM_PD_END_E911;
      u_ret_val = TRUE;
    }
    else
    {
      err_type = PDSM_PD_END_UNKNWN_SYS_ERROR;
      u_ret_val = TRUE; /* undefined stop reason */
    }

    if ( u_ret_val == TRUE )
    {

      if ( (err_type == PDSM_PD_END_CLIENT_END) ||
           (err_type == PDSM_PD_END_TIMEOUT)
          )
      {
        #ifdef FEATURE_RET_REFPOS_UPON_NOFIX
        if ( err_type == PDSM_PD_END_TIMEOUT )
        {
          u_ret_ref_pos = tm_umts_cp_wcdma_report_ref_loc_to_pdapi_if_avail();
          if ( u_ret_ref_pos )
          {
            /* if ref. loc is reported, considered successful */
            stop_reason = TM_STOP_REASON_COMPLETED;
          }
        }
        #endif

        if ( z_umts_cp_wcdma_session_status.e_cf_state & C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT )
        {

          tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

          #ifdef FEATURE_RET_REFPOS_UPON_NOFIX /* return reference position upon no-fix */
          if ( u_ret_ref_pos == TRUE )
          {
            if ( err_type == PDSM_PD_END_TIMEOUT )
            {
              if ( !tm_umts_cp_wcdma_encode_rrc_msrReport_pos(&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip,
                                                              z_last_refloc_info.z_RefPos_GAD.q_tow_ms, NULL)
                  )
              {
                /* if sending of this message fails, stop the session */
                tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
              }
              else
              {

                if ( (z_umts_cp_wcdma_session_status.u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI) &&
                     (z_umts_cp_wcdma_session_status.mo_lr_type == LOCATION_ESTIMATE)
                    )
                {
                  /* give the server a chance to see if it will provide some kind of position
                   via return result. */
                  tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);
                }
                else
                {
                  tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
                }

              } /* refLoc sent to the network successfully */
            }
            else
            {
              /* if still in the state waiting to respond to the network, early termination is needed */
              if ( !tm_umts_cp_wcdma_encode_rrc_msrReport_err(e_rrc_pos_error_cause, NULL) )
              {
                /* if sending of this message fails, move on the stop the session */
                tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
              }
              else /* RRC message sent */
              {
                if ( (z_umts_cp_wcdma_session_status.u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI) &&
                     (z_umts_cp_wcdma_session_status.mo_lr_type == LOCATION_ESTIMATE) &&
                     (e_rrc_pos_error_cause == RRC_NOT_ENOUGH_GPS_SATELLITES)
                    )
                {
                  /* give the server a chance to see if it will provide some kind of position
                     via return result. */
                  tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);
                }
                else
                {
                  tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
                }
              }  /* RRC sent */
            }  /* ( err_type == PDSM_PD_END_TIMEOUT) */
          }
          else
          {
          #endif

            if ( err_type == PDSM_PD_END_TIMEOUT )
            {
              if ( (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED == z_umts_cp_wcdma_session_status.e_gnssPosMode) ||
                   (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED == z_umts_cp_wcdma_session_status.e_gnssPosMode) )
              {
                e_rrc_pos_error_cause = RRC_NOT_ENOUGH_GANSS_SATELLITES;
              }
              else
              {
                e_rrc_pos_error_cause = RRC_NOT_ENOUGH_GPS_SATELLITES;
              }
            }

            /* if still in the state waiting to respond to the network, early termination is needed */
            if ( !tm_umts_cp_wcdma_encode_rrc_msrReport_err(e_rrc_pos_error_cause, NULL) )
            {
              /* if sending of this message fails, move on the stop the session */
              tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
            }
            else /* RRC message sent */
            {
              if ( (z_umts_cp_wcdma_session_status.u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI) &&
                   (z_umts_cp_wcdma_session_status.mo_lr_type == LOCATION_ESTIMATE) &&
                   (e_rrc_pos_error_cause == RRC_NOT_ENOUGH_GPS_SATELLITES)
                  )
              {
                /* give the server a chance to see if it will provide some kind of position
                   via return result. */
                tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);
              }
              else
              {
                tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
              }
            }  /* RRC sent */

            #ifdef FEATURE_RET_REFPOS_UPON_NOFIX /* return reference position upon no-fix */
          }
            #endif

        } /* has been in the state of waiting to respond to RRC */

        else if ( z_umts_cp_wcdma_session_status.u_network_session_on )
        {

          /* QQQ: TBD - not ready to send RRC, but network communication is already on */

          /* this for now, may develop a pending scheme */
          tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
        }

        else /* no need to inform network */
        {
          tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
        }

      } /* if client_end or timeout */

      else
      {
        tm_umts_cp_wcdma_inform_core_stop_session(stop_reason);
      }

    } /* will return TRUE: a legitimate stop session command */

  } /* session handle checked ok */


  return ( u_ret_val );


}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_sess_req_data_handler

DESCRIPTION
  This function handles the session data request from TM-core.

DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_wcdma_sess_req_data_handler
(
  tm_sess_handle_type session_handle,
  prtl_data_req_type req_data
 )
{

  return ( TRUE );
}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_sess_info_handler

DESCRIPTION
  This function handles the session info. request from TM-core.

DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_wcdma_sess_info_handler
(
  tm_sess_handle_type session_handle,
  prtl_sess_info_e_type sess_info_type,
  prtl_sess_info_param_u_type *p_sess_info_param
 )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  if ( (session_handle != p_umts_cp_wcdma_session_status->tm_active_session_handle) ||
       ((p_sess_info_param == NULL) && (sess_info_type != PRTL_SESS_INFO_RPT_REF_POS))
      )
  {
    return ( FALSE );
  }

  switch ( sess_info_type )
  {

    case PRTL_SESS_INFO_PPM: /* not relevant to UMTS */
      break;

     case PRTL_SESS_INFO_PRM:
     {
        MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: got %d (GPS/GLO/BDS/GAL/QZSS) PRM",
               p_sess_info_param->prm_data.z_MeasBlk.e_MeasBlkSrc );
        tm_umts_cp_wcdma_handle_prm_data( &p_sess_info_param->prm_data );
        break;
     }

    case PRTL_SESS_INFO_LR:
    {
      MSG_MED("CP-WCDMA: got FINAL LR", 0, 0, 0);

      tm_umts_cp_wcdma_handle_lr_data(&p_sess_info_param->lr_data, TRUE);
      break;
    }

    case PRTL_SESS_INFO_RPT_REF_POS:
    {
      MSG_MED("CP-WCDMA: got UseRefPos", 0, 0, 0);

      tm_umts_cp_wcdma_handle_rpt_refpos();
      break;
    }


    case PRTL_SESS_INFO_INTERM_LR:
    {
      if ( z_rrc_params.rrc_periodic_multi_report &&
           (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 0)
          )
      {
        tm_umts_cp_wcdma_handle_lr_data(&p_sess_info_param->lr_data, FALSE);
      }
      break;
    }

    case PRTL_SESS_INFO_NOFIX:
    {
      if ( z_rrc_params.rrc_periodic_multi_report &&
           (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 0)
          )
      {
        tm_umts_cp_wcdma_handle_nofix();
      }
      break;
    }

    case PRTL_SESS_INFO_STATUS:
    {

      if ( p_umts_cp_wcdma_session_status->additional_assist_request_done )
      {
        MSG_MED("response to additional assist. request arrives", 0, 0, 0);
      }

      if ( p_umts_cp_wcdma_session_status->u_force_request_ref_location == TRUE )
      {
        p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask |=
                                                                                     C_GPS_ASSIS_REFLOC;

        MSG_MED("SessInfo wishlist with forcing GpsRefLoc = %lx",
                p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask, 0, 0);
      }
      else
      {
        MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SessInfo wishlist = 0x%lx (GPS valid %d), 0x%x (GLO valid %d)",
              p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask,
              p_sess_info_param->aiding_data_status.z_assist_data_status.u_Valid & SM_ASSIST_DATA_VALID_MASK_GPS,
              p_sess_info_param->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask,
              p_sess_info_param->aiding_data_status.z_assist_data_status.u_Valid & SM_ASSIST_DATA_VALID_MASK_GLONASS
             );
      }

      /* if requests in this mask include items already delivered in this session, remove them;
         check the holdoff timer and block requests that are in the holdoff periods. Note no holdoff
         for refLoc if forced request is asked for by pdapi */
      tm_umts_cp_wcdma_msb_assist_req_adj(&p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask,
                                          &p_sess_info_param->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask,
                                          p_umts_cp_wcdma_session_status->e_gnssPosMode);

      MSG_MED("After adj. SessInfo wishlist = 0x%lx (GPS) 0x%lx (GLO), additionaReq = %u ",
              p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask,
              p_sess_info_param->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask,
              p_umts_cp_wcdma_session_status->additional_assist_request_done);

      if ( (p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask)
           #ifdef FEATURE_GANSS_SUPPORT
           || (p_sess_info_param->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask)
           #endif
          )
      {

        if ( p_umts_cp_wcdma_session_status->additional_assist_request_done )
        {
          /* if need to request specific assistance data, and is in a state
             it can do so, do it now. */
          if ( p_umts_cp_wcdma_session_status->e_cf_state ==
                  C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT )
          {
            (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_ASSISTANCE_DATA_MISSING,
                                                            &p_sess_info_param->aiding_data_status.z_assist_data_status);
          }
        }

        else
        {

          /* if UI and is not in response to additional assistance data request, it's time to start SS */
          if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
          {
            prtl_start_sess_req_param_s_type start_sess_req_param;

            start_sess_req_param.pd_qos.accuracy_threshold =
                                                             p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters;

            if ( !tm_umts_cp_request_molr(&start_sess_req_param,
                                          p_umts_cp_wcdma_session_status->mo_lr_type,
                                          &p_sess_info_param->aiding_data_status.z_assist_data_status)
                )
            {
              tm_umts_cp_wcdma_callflow_disrupt_handler();
            }
            else
            {
              p_umts_cp_wcdma_session_status->u_network_session_on = TRUE;
            }
          } /* if UI */
        }

      } /* if need server assistance */

      else   /* no need for assistance data */
      {
        if ( (p_umts_cp_wcdma_session_status->u_pdapi_fix_reported) &&
             (p_umts_cp_wcdma_session_status->e_cf_state ==
                 C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT
             ) && (!z_rrc_params.rrc_periodic_multi_report)
            )
        {

          /* report to the network the same position that was sent to pdapi */
          (void)tm_umts_cp_wcdma_encode_rrc_msrReport_pos(&p_umts_cp_wcdma_session_status->z_pending_pos_to_network,
                                                          p_umts_cp_wcdma_session_status->q_time_stamp_pos_pending_msec, NULL);

          /* clear timer and call flow state */
          tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

          /* send DONE event to pdapi and end the session */
          tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);

        }   /* fix has obtained and is a state where network reporting may happen */

        else
        {
          /* tell TmCore to send pdapi event of EventGpsPdConnectionDone */
          if ( !p_umts_cp_wcdma_session_status->u_pdapi_comm_done_sent )
          {
            tm_umts_common_send_pdapi_comm_event
               (PDSM_PD_COMM_PROTOCOL_UMTS_CP_WCDMA,
                C_COMM_DONE,
                p_umts_cp_wcdma_session_status->tm_active_session_handle,
                p_umts_cp_wcdma_session_status->q_pdapi_client_id
               );

            p_umts_cp_wcdma_session_status->u_pdapi_comm_done_sent = TRUE;
          }

          /* stop expecting network assist., QoS timer starts if not yet */
          tm_umts_cp_wcdma_inform_core_nt_assist_end(FORBID_QOS_TIME_RESET);
        }
      } /* wishlist inquery comes back empty: no assistance needed */
      break;
    } /* SESS_INFO_STATUS */
    default:
      MSG_MED("Case (%d) not supported in tm_umts_cp_wcdma_sess_info_handler", sess_info_type, 0, 0);
      break;

  } /* switch_session_info */

  return ( TRUE );
}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_timer_cb_handler

DESCRIPTION
  This function handles the session data pipe from TM-core.

DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_wcdma_timer_cb_handler( void *timer_data )
{
  boolean b_CachedGnssMeasSent = FALSE;

  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  pdsm_ext_status_info_s_type z_ext_info_buffer;
  if ( timer_param != NULL )
  {
    MSG_HIGH("timerId 0x%lX expired", timer_param->q_TimerId, 0, 0);

    if ( (timer_param->q_TimerId & TM_UMTS_CP_WCDMA_TIMER_DISPATCH_MASK) )
    {
      switch ( timer_param->q_TimerId )
      {
        case CP_WCDMA_CF_TIMER_ID:
        {
          if ( p_umts_cp_wcdma_session_status->e_cf_state == C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC )
          {
            MSG_HIGH("UMTS-CP-W: Timeout waiting network message", 0, 0, 0);

            if ( p_umts_cp_wcdma_session_status->u_pdapi_fix_reported )
            {
              tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
            }
            else
            {
              /* no hope of getting network support, start trying on its own */
              tm_umts_cp_wcdma_callflow_disrupt_handler();
            }
          }

          else if ( p_umts_cp_wcdma_session_status->e_cf_state == C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT )
          {
            MSG_MED("UMTS-CP-W: Generate measurement timeout", 0, 0, 0);
            tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

            #ifdef FEATURE_EXTENDED_RESP_TIME
            tm_umts_cp_wcdma_req_extend_resp_time(); /* send request for IONO, buying time */

            #else /* regular handling of RRC reportIntervalLong expiration */

            /* If there is cached GNSS measurement, in a MSA session, report that
            cached measurement. This is used for E911 MsAssisted mode because
            no measurement is to be sent to network until RRC responseTime times
            out. A latest measurement is cached in case reception deteriorates
            approaching the end of the session. Note for MSB, similar caching
            mechanism is done in LM */
            if ( (z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & (SM_GNSS_MEAS_REPORTS_BMAP_GPS | SM_GNSS_MEAS_REPORTS_BMAP_GLONASS)) &&
                 ((z_umts_cp_wcdma_session_status.e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
                  (z_umts_cp_wcdma_session_status.e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED)) )
            {
              b_CachedGnssMeasSent = tm_umts_cp_wcdma_rrc_send_cached_gnss_meas_and_end_session();
            }

            /* Send RRC MRM error message & stop the session */
            if ( FALSE == b_CachedGnssMeasSent )
            {
              /* send an RRC-measurementResponse to the network */
              if ( (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED == z_umts_cp_wcdma_session_status.e_gnssPosMode) ||
                   (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED == z_umts_cp_wcdma_session_status.e_gnssPosMode)
                  )
              {
                (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_NOT_ENOUGH_GANSS_SATELLITES, NULL);
              }

              else
              {
                (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_NOT_ENOUGH_GPS_SATELLITES, NULL);
              }

              if ( z_rrc_params.rrc_periodic_multi_report ) /* multiple periodic */
              {
                p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining--;
                p_umts_cp_wcdma_session_status->w_lm_report_count = 0;

                /* if done: close session, else keep waiting */
                if ( p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining == 0 )
                {
                  tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
                }
              }

              else  /* NOT multiple periodic */
              {

                if ( (p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI) &&
                     ((p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
                      (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED)
                     ) )
                {
                  /* For UI - UeAsssited, wait for return result */
                  tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);
                }
                else if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NI )
                {
                  /* for NI, end the session now */
                  /* tell TmCore to end and send DONE event to pdapi, also clear UMTS-CP-WCDMA and UMTS-CP-SS */
                  if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
                       (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
                  {
                    tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_PRM_TIMEOUT);
                  }
                  else
                  {
                    tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_FIX_TIMEOUT);
                  }

                } /* else if NI */

                /* the only other case would be UI-UeBased, in which case do nothing here and let pdapi=Qos takes
                   its course */

              } /* NOT multiple periodic */
            } /* NOT E911 with cached GPS measurement */

            #endif
          }

          else if ( p_umts_cp_wcdma_session_status->e_cf_state == C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT )
          {
            MSG_MED("UMTS-CP-W: Wait-MoLrReturnResult timeout", 0, 0, 0);
            tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT);

            if ( p_umts_cp_wcdma_session_status->mo_lr_type == LOCATION_ESTIMATE )
            {
              /* Notify NAS that the positioning session has ended and end the session */
              #ifdef FEATURE_CGPS_DUAL_WCDMA
              tm_umts_common_utils_mm_session_status_notify(FALSE, z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id);
			  #else
              tm_umts_common_utils_mm_session_status_notify(FALSE);
			  #endif
              tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_FIX_TIMEOUT);
            }
            else
            {
              /* for AssistanceData call flow, every time an AssistanceDataDelivery is
                 received, the protocol module enters wait_for_return_result, expecting
                 this message to end the delivery; if another AssistanceDataDelivery
                 arrives, it cancels resets the same timer; if timeout, assume no more
                 assistance is on the way */
              tm_umts_cp_wcdma_inform_core_nt_assist_end(FORBID_QOS_TIME_RESET);
            }
          } /* has been in a state waiting for MOLR_returnResult */

          else if ( p_umts_cp_wcdma_session_status->e_cf_state == C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME )
          {
            /* Note: this is a special feature and is not part of a regular call flow */
            MSG_MED("UMTS-CP-W: MT guard timer timeout", 0, 0, 0);
            tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME);
            p_umts_cp_wcdma_session_status->allow_rrc = z_umts_nv_item_ram_copy.default_allow_rrc;

            if ( p_umts_cp_wcdma_session_status->q_pdapi_client_id == -1 )
            {
              z_ext_info_buffer.client_id = pdsm_ni_client_id_get();
            }
            else
            {
              z_ext_info_buffer.client_id = p_umts_cp_wcdma_session_status->q_pdapi_client_id;
            }
            z_ext_info_buffer.ext_status_info.ext_generic_event_info.event_type = PDSM_PD_EXT_STATUS_GUARD_TIMER_EXP;

            tm_post_ext_status_generic_event(TM_PRTL_TYPE_UMTS_UP,
                                             p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                             &z_ext_info_buffer);


            /* Notify NAS that the positioning session has ended */
            #ifdef FEATURE_CGPS_DUAL_WCDMA
            tm_umts_common_utils_mm_session_status_notify(FALSE, z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id);
			#else
            tm_umts_common_utils_mm_session_status_notify(FALSE);
			#endif
          }

          break;
        }

        case CP_WCDMA_MAX_MGP_ON_TIMER_ID:
        {
          MSG_HIGH("CP-WCDMA: MaxMgpOn timer expired", 0, 0, 0);

          /*
           *  IF the max_mgp_on timer (currently 40 secs) expired THEN
           *    Turn off MGP and abort the session
           */
          tm_umts_cp_wcdma_callflow_disrupt_handler();
          break;
        }

        default:
          MSG_HIGH("Timer ID %u egnored", timer_param->q_TimerId, 0, 0);


      }
    }

  }

  return;
}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_event_cb_handler

DESCRIPTION
  This function handles the session data pipe from TM-core.

DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_wcdma_event_cb_handler
(
  prtl_event_e_type event_type,
  prtl_event_u_type *p_event_payload
 )
{
  if (NULL == p_event_payload)
  {
	MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL Pointer passed to tm_umts_cp_wcdma_event_cb_handler");
	return FALSE;
  }

  switch ( event_type )
  {

    case PRTL_EVENT_TYPE_LCS:
    {
      if ( p_event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_RESP )
      {
        tm_umts_cp_wcdma_notif_verif_resp_proc(&p_event_payload->lcs_event_payload.lcs_event_payload.clnt_resp_payload);
      }
      else if ( p_event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_CLINET_INIT )
      {
        MSG_MED("UMTS-CP-W: TmCore cb LCS type with event CLINET_INIT", 0, 0, 0); /* QQQ: ? */
      }
      else if ( p_event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_CLINET_RELEASE )
      {
        MSG_MED("UMTS-CP-W: TmCore cb LCS type with event CLINET_RELEASE", 0, 0, 0); /* QQQ: ? */
      }
      break;
    }

    case PRTL_EVENT_TYPE_PHONE:
    {
      if ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_LCS_MOLR )
      {
        /* MOLR-Invoke queued at NAS */

        /* inform SS about this event */
        tm_umts_cp_ss_lcs_molr_proc();

        /* start waiting for something from the network */
        tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC, TRUE);

      }

      else if ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_LCS_MOLR_CONF )
      {
        MSG_MED("UMTS-CP-W: CM delivers LCS_MOLR_CONF", 0, 0, 0);

        tm_umts_cp_ss_lcs_molr_conf_proc(&p_event_payload->phone_event_payload.payload.sups_data);
        tm_umts_cp_wcdma_lcs_molr_conf_proc(&p_event_payload->phone_event_payload.payload.sups_data);
      }

      else if ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_LCS_MOLR_COMPLETED )
      {
        MSG_MED("UMTS-CP-W: CM delivers LCS_MOLR_COMP", 0, 0, 0);

        /* this event is recieved in the following cases.
        1. After MOLR has been passed to CM, received CM indication of MOLR-complete
           without getting prior indication of receiving the FACILITY message, or
           without TM's prior action of actively asking CM for "release complete"
           --- this is an indication of a failure in actually sending MOLR to the
           network".

        2. Normal indication of release complete, either after receiving the
          FACILITY message, or after TM actively asked CM to "release complete".
          The latter could be result of timeout waiting for a network response.
        */
        tm_umts_cp_ss_lcs_molr_comp_proc();
        /* in the first case above, tell TM core to work on its own and clean up the ss state*/
        if ( tm_umts_cp_ss_is_molr_on_comp() == TRUE )
        {
          tm_umts_cp_wcdma_callflow_disrupt_handler();
          tm_umts_cp_ss_init();
        }
        /* case 2, nothing needs to be done since the clean up already happened */
      }

      else if ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_LCS_MTLR_NTFY_IND )
      {
        MSG_MED("UMTS-CP-W: CM delivers MTLR_NTFY_IND", 0, 0, 0);
        tm_umts_cp_ss_lcs_loc_notif_ind_proc(&p_event_payload->phone_event_payload.payload.sups_data);
        //tm_umts_cp_wcdma_lcs_loc_notif_ind_proc( &p_event_payload->phone_event_payload.payload.sups_data );
      }

      else if ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_LCS_MTLR_NTFY_RES )
      {
        MSG_MED("UMTS-CP-W: CM confirms LCS-LocNotifResp queued to NAS ", 0, 0, 0);
      }

      else if ((p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_E911_CALL_ORIG) ||
	  		   (p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_UMTS_COVERAGE))
      {
        /*
         *  This event is received when an E911 call is brought up.
         *  This event must be handled by starting a session, starting
         *  the GPS engine, and starting a timer that will expire (and
         *  shut off the GPS engine) if no positioning session is
         *  started within a reasonable time after bringing up the E911 call.

         *  Start out in UE-Based mode if the NV item for Positioning modes
         *  support WCDMA MSB because this will get the engine
         *  up and running immediately in the hope that this will speed up
         *  signal acquisition and decrease TTF.
         */
        tm_umts_cp_wcdma_e911_sess_start();        
      }
      break;
    } /*  event_type == PRTL_EVENT_TYPE_PHONE */

    case PRTL_EVENT_TYPE_RRC:
    {
      tm_umts_cp_wcdma_rrc_proc(&p_event_payload->rrc_event_payload);
      break;
    }

    case PRTL_EVENT_TYPE_MM:
    {
      tm_umts_cp_wcdma_mm_proc(&p_event_payload->mm_event_payload);
      break;
    }

    case PRTL_EVENT_TYPE_PA:
    {
      tm_umts_cp_wcdma_pa_proc(&p_event_payload->pa_event_payload);
      break;
    }

    default:
      MSG_HIGH("UMTS-CP-W: wrong prtl_event_type %u", event_type, 0, 0);

  }  /* close of switch (event_type) */

  return ( TRUE );
}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_general_query_handler

DESCRIPTION
  This function handles the start session request from TM-core.

DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/
boolean tm_umts_cp_wcdma_general_query_handler
( prtl_query_e_type query_type,
  prtl_ans_u_type *answer_ptr
 )
{
  boolean u_ret_val = FALSE;

  if ( query_type == PRTL_QUERY_SMART_MSB )
  {
    if ( z_umts_cp_wcdma_session_status.u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NI )
    {
      answer_ptr->ans_smart_msb = PRTL_ANS_SMART_MSB_NOT_ALLOWED;
    }
    else
    {
      answer_ptr->ans_smart_msb = PRTL_ANS_SMART_MSB_ALLOWED;
    }

    u_ret_val = TRUE;
  }

  return ( u_ret_val );


}

/*===========================================================================

FUNCTION

DESCRIPTION
  This function handles .

DEPENDENCIES: none.

RETURN VALUE: .

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_wcdma_init( void )
{

  MSG_MED("tm_cp_wcdma init", 0, 0, 0);

  /* Initialize the main wcdma session struct with zero */
  memset(&z_umts_cp_wcdma_session_status, 0, sizeof(umts_cp_wcdma_session_status_s_type));

  tm_umts_cp_wcdma_session_cleanup(); /* Release any memory & reset all CP-WCDMA session related variables */

  /* create CP-WCDMA call flow timer */
  umts_cp_wcdma_cf_timer = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_CF_TIMER_ID, THREAD_ID_SM_TM);

  /* create GPS assistance data request holdoff period timers */
  cp_wcdma_alm_req_holdoff_timer    = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_ALM_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);
  cp_wcdma_eph_req_holdoff_timer    = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_EPH_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);
  cp_wcdma_iono_req_holdoff_timer   = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_IONO_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);
  cp_wcdma_utc_req_holdoff_timer    = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_UTC_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);
  cp_wcdma_refloc_req_holdoff_timer = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_REFLOC_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);

  #ifdef FEATURE_GANSS_SUPPORT
  /* create GANSS (GLONASS) assistance data request holdoff period timers */
  cp_wcdma_glo_eph_req_holdoff_timer        = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_GLO_EPH_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);
  cp_wcdma_glo_alm_req_holdoff_timer        = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_GLO_ALM_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);
  cp_wcdma_glo_auxi_info_req_holdoff_timer  = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_GLO_AUXI_INFO_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);
  cp_wcdma_glo_time_model_req_holdoff_timer = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_GLO_TIME_MODEL_REQ_HD_TIMER_ID, THREAD_ID_SM_TM);
  #endif

  /*create CP-WCDMA max mgp on timer*/
  umts_cp_wcdma_max_mgp_on_timer = os_TimerCreateInCgpsTimerGroup(CP_WCDMA_MAX_MGP_ON_TIMER_ID, THREAD_ID_SM_TM);
}

/*===========================================================================

FUNCTION  tm_umts_cp_wcdma_ue_pos_capability_get

DESCRIPTION
  This function is used by cgps_api to provide UE capability to external
  modules (RRC).

DEPENDENCIES: none.

RETURN VALUE: lsm_ue_pos_capability_struct_type structure.

SIDE EFFECTS: none.

===========================================================================*/

tm_umts_cp_wcdma_ue_pos_capability_struct_type tm_umts_cp_wcdma_ue_pos_capability_get( void )
{
  tm_umts_cp_wcdma_ue_pos_capability_struct_type z_pos_capabilities;

  memset((void *)&z_pos_capabilities, 0, sizeof(z_pos_capabilities));

  /* Set non-configurable parameters */
  z_pos_capabilities.gps_timing_of_cell_frames_supported = FALSE;
  z_pos_capabilities.ipdl_supported = FALSE;
  z_pos_capabilities.rx_tx_type2_supported = FALSE;
  z_pos_capabilities.ue_based_otdoa_supported = FALSE;

  /* Inform RRC that positioning during cell PCH and URA_PCH is supported; note this
     flag is most like TRUE except if later in this function it turns out that AGPS is not
     provisioned by NV, it will turn it to FALSE */
  z_pos_capabilities.up_meas_validity_cell_pch_and_ura_pch = TRUE;

  if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE) )
  {
    z_pos_capabilities.standalone_location_method_supported = TRUE;
  }

  if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_BASED) &&
       TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_ASSISTED)
      )
  {
    z_pos_capabilities.network_assisted_gps_support = WCDMA_CLASSMARK_UE_CAPABILITY_BOTH;
  }
  else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_BASED) )
  {
    z_pos_capabilities.network_assisted_gps_support = WCDMA_CLASSMARK_UE_CAPABILITY_UE_BASED;
  }
  else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_ASSISTED) )
  {
    z_pos_capabilities.network_assisted_gps_support = WCDMA_CLASSMARK_UE_CAPABILITY_NETWORK_BASED;
  }
  else
  {
    z_pos_capabilities.network_assisted_gps_support = WCDMA_CLASSMARK_UE_CAPABILITY_NONE;
    z_pos_capabilities.up_meas_validity_cell_pch_and_ura_pch = FALSE;
  }

  z_pos_capabilities.lcs_value_added_lcs_supported =
                                                     (z_umts_nv_item_ram_copy.mt_lr_support & NV_AAGPS_MT_LR_CP_GSM_WCDMA_SUPPORTED);

  #ifdef FEATURE_GANSS_SUPPORT

  if ( tm_umts_common_glonass_enabled() && tm_umts_cp_wcdma_a_glo_rrc_cp_enabled() )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "UEPC supports RRC GANSS(Glonass). PosMode 0x%lX",
          TM_CORE_CONFIG_DD(positioning_modes_supported));

    /* UE-PositioningCapability-v770ext, only GLONASS is supported */
    z_pos_capabilities.ganss_supported = TRUE;
    z_pos_capabilities.ganss_supported_list_incl = TRUE;

    z_pos_capabilities.list_cnt = 1;
    z_pos_capabilities.ganss_supported_list[0].ganss_id_incl = TRUE; // rrc_nw_assist_ganss_supported_list_s_type ganss_supported_list[MAX_GANSS];
    z_pos_capabilities.ganss_supported_list[0].ganss_id = RRC_GLONASS;

    if ( tm_core_pos_mode_allows_agnss_msb_cp( TM_POS_PRTL_RRC ) &&
         tm_core_pos_mode_allows_agnss_msa_cp( TM_POS_PRTL_RRC ) )
    {
      z_pos_capabilities.ganss_supported_list[0].ganss_mode = GANSS_MODE_BOTH;
    }
    else if ( tm_core_pos_mode_allows_agnss_msa_cp( TM_POS_PRTL_RRC ) )
    {
      z_pos_capabilities.ganss_supported_list[0].ganss_mode = GANSS_MODE_NW_BASED;
    }
    else if ( tm_core_pos_mode_allows_agnss_msb_cp( TM_POS_PRTL_RRC ) )
    {
      z_pos_capabilities.ganss_supported_list[0].ganss_mode = GANSS_MODE_UE_BASED;
    }
    else
    {
      z_pos_capabilities.ganss_supported_list[0].ganss_mode = GANSS_MODE_NONE;
    }

    /* The GANSS Signal ID encodes the identification of the signal for each GANSS. It depends on the GANSS Id.
    Absence of this field means the default value for the GANSS identified by GANSS ID
    MSM does not support measurement on more than one GANSS signal. */

    /* Absence of 'GANSS Signal ID' field means the default value as G1 for the GANSS identified by GANSS ID = GLONASS */

    z_pos_capabilities.ganss_supported_list[0].ganss_sig_id_incl = FALSE;
    z_pos_capabilities.ganss_supported_list[0].ganss_timing_of_cell_frames = FALSE;

    /* Target doesn't support GANSS carrier-phase measurement */
    z_pos_capabilities.ganss_supported_list[0].ganss_carrier_phase_incl = FALSE;
    z_pos_capabilities.ganss_supported_list[0].ganss_carrier_phase_meas = FALSE;
  }
  else
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "RRC GANSS not supported. Glonass %d, Glo RRC %d",
          tm_umts_common_glonass_enabled(), tm_umts_cp_wcdma_a_glo_rrc_cp_enabled());
  }
  #endif /* FEATURE_GANSS_SUPPORT */

  return ( z_pos_capabilities );

}


/*===========================================================================

FUNCTION  tm_umts_cp_wcdma_e911_sess_start

DESCRIPTION
  This function starts a new E911 WCDMA CP session.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_umts_cp_wcdma_e911_sess_start()
{
  tm_cm_phone_state_info_s_type phone_state_info;
  emergency_handling_e_type e_e911_effect = C_EMERGENCY_HANDLING_NULL;

  memset(&phone_state_info, 0, sizeof(phone_state_info));
  tm_cm_iface_get_phone_state(&phone_state_info);

  if ((phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA) &&
  	  (phone_state_info.e911_state != TM_CM_E911_STATE_NONE))
  {
    e_e911_effect = tm_umts_common_utils_emergency_effect();
    /*Check if E911 can be accepted*/
    if ((e_e911_effect == C_EMERGENCY_HANDLING_REJ_ALL) 
        #ifdef FEATURE_MO_GPS_FOR_ECALL
        || (e_e911_effect == C_EMERGENCY_HANDLING_ACCEPT_ALL)
        #endif
       )
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: No early rcvr ON for E911_ORIG due to e911_effect %u", e_e911_effect);
    }
    else if ( ((z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_ALL) ||
               (z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_MT)) &&
              (e_e911_effect != C_EMERGENCY_HANDLING_ACCEPT_ONLY_HIGH_PRIORITY))
    {     
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: Ignore E911_ORIG due to GPS Lock %u e_e911 %u", z_umts_nv_item_ram_copy.gps_lock, e_e911_effect);
    }
    else
    {
      if ( (z_umts_cp_wcdma_session_status.e_cf_state == C_UMTS_CP_WCDMA_CF_STATE_NONE) &&
           TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_BASED)
         )
      {
        tm_sess_req_param_u_type req_param;
        tm_sess_req_param_u_type *p_req_param = &req_param;

        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: Handle E911_CALL_ORIG");

        z_umts_cp_wcdma_session_status.e_gnssPosMode = eWCDMA_POSITION_MODE_AGPS_UE_BASED;
        z_umts_cp_wcdma_session_status.tm_active_session_handle = TM_SESS_HANDLE_E911;

        p_req_param->start_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
        p_req_param->start_param.op_req = TM_OP_REQ_LR;
        p_req_param->start_param.lr_qos = TM_MAX_SESSION_TIMEOUT;
        p_req_param->start_param.accuracy_threshold = 0;
        p_req_param->start_param.abort_curr_sess = FALSE;
        p_req_param->start_param.num_fixes = 1;
        p_req_param->start_param.tbf_ms = 1000; /* don't care */
        p_req_param->start_param.dynamic_qos_enabled = TRUE;

        if ( !tm_sess_req(TM_PRTL_TYPE_E911,
                          TM_SESS_HANDLE_E911,
                          TM_SESS_REQ_START,
                          p_req_param
                         )
            )
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: tm_sess_req() REJECTED");
          tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
        }
        else
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: tm_sess_req() ACCEPTED");
          tm_core_send_pdapi_emerg_sess_type(PDSM_PD_EXT_STATUS_EMERG_SESS_START);
          z_umts_cp_wcdma_session_status.tm_core_session_started = TRUE;
          /* Starting E911: MGP will be on for at most 40 seconds. */
          tm_umts_cp_wcdma_start_max_mgp_on_timer(TM_UMTS_CP_WCDMA_MAX_MGP_ON_IN_MS);
        }
      }
      else
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: MS-Based not supported, not starting MGP. Ignore E911_CALL_ORIG");
      }
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: Not in WCDMA mode/e911_state. Ignore E911_CALL_ORIG");
  }
}


#endif /* FEATURE_CGPS_UMTS_CP_WCDMA */



