/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM_UMTS_CP_GSM API  

GENERAL DESCRIPTION

  This file provides TM_UMTS_CP_GSM's APIs to TM_UMTS_CP.
   
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/gsm/src/tm_umts_cp_gsm_api.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
  07/31/15   mj      Changes to fix E911 on second SUB in G+G config
  04/02/15   mj      Cache as_id for GSM CP RR events
  09/11/14   mj      Set asid for MSB GSM CP sessions
  07/13/09   gk      ignore end session command
  05/03/07   LT      Initial version

============================================================================*/
#include "gps_variation.h"
#include "customer.h"


#ifdef FEATURE_CGPS_UMTS_CP_GSM

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msg.h"
#include <limits.h>
#include "tm_common.h"
#include "tm_prtl_iface.h"
#include "tm_umts_common_utils.h"
#include "tm_umts_cp_gsm.h" 
#include "tm_umts_cp_submodule_iface.h"
#include "tm_umts_cp_gsm_rrlp.h"
#include "aries_os_api.h"
#include "sm_nv.h"
#include "gsclassmark.h"

#ifdef MSG_LOW
#undef MSG_LOW
#endif
#define MSG_LOW(s,p1,p2,p3) MSG_MED(s,p1,p2,p3)



/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION tm_umts_cp_gsm_start_sess_req_handler

DESCRIPTION
  This function handles the start session request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: prtl_start_sess_req_result_e_type.

SIDE EFFECTS: none.

===========================================================================*/
 
prtl_start_sess_req_result_e_type tm_umts_cp_gsm_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions
)
{
  molr_type_e_type                    e_mo_lr_type = LOCATION_ESTIMATE;
  umts_cp_gsm_session_status_s_type   *p_session_status;
  boolean                             pdapi_mapping_success;

  MSG_LOW("CP-GSM: start_sess_req_handler() called; handle=%d", sess_handle, 0, 0);

  if ((NULL == start_sess_req_param) || (NULL == actions))
  {
	MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer passed to tm_umts_cp_gsm_start_sess_req_handler");
	return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  /*
   *  Create a local pointer to make the code easier to read
   */
  p_session_status = &z_umts_cp_gsm_session_status;

  /*
   *  NOTE - This routine must NEVER be called to start an MT/NI session.
   *          All of the logic in this function assumes this is only called
   *          to start an MO/UI session.
   */

  /*
   *  Reject Start Session Requests if the callflow state is anything
   *  other than NONE.  In other words, reject Start Session requests that
   *  arrive while a previous session is still in progress.
   */
  if( p_session_status->e_cf_state != C_UMTS_CP_GSM_CF_STATE_NONE )
  {
    MSG_HIGH("CP-GSM: Session Start Request Rejected", 0, 0, 0);

    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  /*
   *  Map PDAPI provided session parameters to GSM modes
   *
   *  NOTE - This function takes inputs ONLY from start_sess_req_param
   *         and it outputs the following:
   *            z_umts_cp_gsm_session_status.e_agps_mode
   *            z_umts_cp_gsm_session_status.q_pdapi_client_id
   *            z_umts_cp_gsm_session_status.z_session_qos
   */
  pdapi_mapping_success = tm_umts_cp_gsm_pdapi_mapping( start_sess_req_param );

  /*
   *  IF the PDAPI mapping is successful THEN
   *    initialize a bunch more state variables to be used later
   */
  if( pdapi_mapping_success )
  {
    const pdsm_pd_fix_rate_s_type             *p_fix_rate;
    uint32                                    num_fixes;
    uint32                                    time_between_fixes_ms;
    umts_cp_gsm_session_agps_mode_enum_type   e_agps_mode;

    /*
     *  Save a copy of the start_sess_req_param structure for use later.
     */
    p_session_status->z_start_sess_req_param = *start_sess_req_param;

    /*
     *  Initialize more state variables inside "session status"
     *  as necessary.
     */
    p_session_status->tm_active_session_handle = sess_handle;

    tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_STARTED );

    tm_umts_cp_gsm_callflow_kind_set( C_UMTS_CP_GSM_SESSION_UI );
    
    /*
     *  Store some parameters in local variables to make the code 
     *  a bit easier to read below.  This eliminate all of the extra
     *  long lines and line wrapping that would occur otherwise.
     */
    p_fix_rate            = &start_sess_req_param->pd_option.fix_rate;
    num_fixes             = p_fix_rate->num_fixes;
    time_between_fixes_ms = p_fix_rate->time_between_fixes_ms;
    e_agps_mode           = p_session_status->e_agps_mode;

    /*
     *  The number of fixes, time_between_fixes, and
     *  accuracy_threshold are the same regardless of whether doing
     *  UE-Based or UE-Assisted
     */
    actions->num_fixes          = num_fixes;
    actions->tbf_ms             = time_between_fixes_ms;

    /* GSM_CP does not support "multiple MT reporting" */
    actions->multi_report_msa = FALSE;
 
    if( p_session_status->z_session_qos.q_hor_acc_meters <= 255 )
    {
      actions->accuracy_threshold = 
            (uint8) p_session_status->z_session_qos.q_hor_acc_meters;
    }
    else
    {
      actions->accuracy_threshold = 255;
    }

    /*
     *  IF doing UE-Based THEN
     *    create an "actions" structure for UE-Based
     *  ELSE
     *    create an "actions" structure for UE-Assisted
     */ 
    if ( e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_BASED )
    {
      MSG_MED("CP-GSM: MO-MB starts", 0, 0, 0);

      actions->op_req   = TM_OP_REQ_LR | TM_OP_REQ_AIDING_DATA_STATUS;
      actions->lr_qos   = TM_MAX_SESSION_TIMEOUT;
      actions->op_mode  = TM_SESS_OPERATION_MODE_MSB;
      actions->dynamic_qos_enabled  = TRUE;

      if( z_umts_nv_item_ram_copy.mo_lr_2g_support & 
                                          NV_AAGPS_2G_MO_LR_ASSIST_DATA)
      {
        e_mo_lr_type = ASSISTANCE_DATA;
      } 
    }
    else if ( e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
    {
      MSG_MED("CP-GSM: MO-MA starts", 0, 0, 0);

      /*
       *  don't set TM_OP_REQ_PRM yet.  Wait until AcqAssist is injected
       */
      actions->op_req   = TM_OP_REQ_PRM | TM_OP_REQ_AIDING_DATA_STATUS;
      actions->prm_qos  = TM_MAX_SESSION_TIMEOUT;
      actions->op_mode  = TM_SESS_OPERATION_MODE_MSA;

      /*
       *  The wishlist setting is not absolutely needed as it will be set 
       *  like that later
       */
      p_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH |= 
                    ( C_G0931_GPS_ASSIST_REFTIME | C_G0931_GPS_ASSIST_ACQ );
    }
    else if ( e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_CONV )
    {
      /* handling of "conventional GPS" mapped from pdapi's DATA_OPTIMAL mode */
      actions->op_req   = TM_OP_REQ_LR;
      actions->lr_qos   = p_session_status->z_session_qos.u_gps_session_timeout_value_secs;
      actions->op_mode  = TM_SESS_OPERATION_MODE_MSB;
      actions->dynamic_qos_enabled  = TRUE;

      /*
       *  Tell TM-Core that there is no AGPS session to follow
       */
      tm_umts_cp_gsm_inform_core_nt_assist_end();
    }

    /*
     *  Save the mo_lr_type in case we need to know later
     */
    z_umts_cp_gsm_session_status.e_mo_lr_type = e_mo_lr_type;

    /*
     *  IF doing an MOLR "ASSISTANCE_DATA" callflow THEN
     *    Set a flag now indicating that the wishlist has been requested
     *    This flag will be used later (when an MPR is received) to determine 
     *    whether the wishlist should be requested after the MPR is received.
     *  ENDIF
     */    
    if( e_mo_lr_type == ASSISTANCE_DATA )
    {
      z_umts_cp_gsm_session_status.u_wishlist_requested = TRUE;
    }
  }
  else
  {
    MSG_HIGH("CP-GSM: pdapi_mapping failure", 0, 0, 0);

    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  /* TM core session has started */
  p_session_status->u_tm_core_session_started = TRUE;

  if (!z_umts_cp_gsm_session_status.z_rr_as_id_info.v_rr_event_as_id_received)
  {
    /* Cache the as_id, so that we can use it for rest of the session */
    z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id = tm_core_get_current_dd_subs_id();
    z_umts_cp_gsm_session_status.z_rr_as_id_info.v_rr_event_as_id_received = TRUE;
	MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-GSM: AS_ID cached to %u", (uint32)z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id);
  }

  return PRTL_START_SESS_REQ_ALLOWED;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_stop_sess_req_handler

DESCRIPTION
  This function handles the stop session request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_gsm_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
)
{
  const umts_cp_gsm_session_status_s_type   *p_session_status;
  pdsm_pd_end_e_type                  err_type = PDSM_PD_END_UNKNWN_SYS_ERROR;

  MSG_LOW("CP-GSM: stop_sess_req_handler() called", 
          (int) stop_type, 
          (int) stop_reason, 
          0);
  
  /*
   *  Create a local pointer to make the code easier to read
   */
  if (sess_handle == TM_SESS_HANDLE_NONE)
  {
    return TRUE;
  }
  p_session_status = &z_umts_cp_gsm_session_status;

  /*
   *  Reject Stop Session Requests if the callflow state is NONE
   */
  if( p_session_status->e_callflow_kind == C_UMTS_CP_GSM_SESSION_NONE )
  {
    MSG_HIGH("CP-GSM: Session Stop Request Rejected", 0, 0, 0);

    return FALSE;
  }

  /* For low priority emergency support NV item setting allow a new E911 call to
     continue with the existing E911 session. */
  if ( (stop_reason == TM_STOP_REASON_E911) &&
       (tm_umts_common_utils_emergency_effect() == C_EMERGENCY_HANDLING_ACCEPT_ALL) )
  {
    MSG_HIGH("CP-GSM: Sess Stop Req rejected due to NV E911 support setting", 0, 0, 0);
    return FALSE;
  }

  if( sess_handle == p_session_status->tm_active_session_handle )
  {
    switch( stop_reason )
    {
      case TM_STOP_REASON_COMPLETED:
        err_type  = PDSM_PD_END_NORMAL_ENDING;
        break;

      case TM_STOP_REASON_GENERAL_ERROR:
        err_type  = PDSM_PD_END_UNKNWN_SYS_ERROR;
        break;

      case TM_STOP_REASON_FIX_TIMEOUT:
      case TM_STOP_REASON_PRM_TIMEOUT:
        err_type  = PDSM_PD_END_TIMEOUT;
        break;

      case TM_STOP_REASON_NONE:
        err_type  = PDSM_PD_END_SESS_NONE;
        break;

      case TM_STOP_REASON_PPM_TIMEOUT:
        /* This case should never occur in UMTS */
        return FALSE;

      case TM_STOP_REASON_USER_TERMINATED:
        err_type  = PDSM_PD_END_CLIENT_END;
        break;

      case TM_STOP_REASON_E911:
        err_type  = PDSM_PD_END_E911;
        break;

      case TM_STOP_REASON_ON_RESET_LOCATION_SERVICE:
        err_type = PDSM_PD_END_RESET_LOCATION_SERVICE;
        break;

      default:
        err_type  = PDSM_PD_END_UNKNWN_SYS_ERROR;
        break;
    }

    /*
     *  IF error in the handset as opposed to the server THEN
     *    IF the server is waiting for a response THEN
     *      send an error message to the server
     *  ENDIF
     */
    if( (err_type == PDSM_PD_END_CLIENT_END) ||
        (err_type == PDSM_PD_END_RESET_LOCATION_SERVICE) ||
        (err_type == PDSM_PD_END_TIMEOUT) )
    {
      if( p_session_status->e_cf_state == 
          C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME )
      {
        (void) tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_notEnoughSats );
      }
    }

    /*
     *  Terminate the session with TM Core
     */
    if ( ( stop_reason == TM_STOP_REASON_FIX_TIMEOUT) ||
         ( stop_reason == TM_STOP_REASON_PRM_TIMEOUT)
       )
    {
      tm_umts_cp_gsm_inform_core_stop_session(stop_reason);
    }
    else
    {
      tm_umts_cp_gsm_inform_core_stop_session(
                                        TM_STOP_REASON_USER_TERMINATED );
    }

    return TRUE;
  }

  return FALSE;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_sess_req_data_handler

DESCRIPTION
  This function handles the session data request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_gsm_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)
{
  /*
   *  This function isn't used by UMTS
   */
  MSG_LOW("CP-GSM: sess_req_data_handler() called", (int) session_handle, (int) req_data, 0);

  return TRUE;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_sess_info_handler

DESCRIPTION
  This function handles the session info. request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_gsm_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *p_sess_info_param
)
{
  MSG_LOW("CP-GSM: sess_info_handler() called", 0, 0, 0 );
 
  /*
   *  Reject calls to this API if the callflow state is NONE (ie. no session
   *  in progress OR if the session_handle doesn't match the current
   *  active session handle for GSM CP.
   */
  if( z_umts_cp_gsm_session_status.e_callflow_kind == C_UMTS_CP_GSM_SESSION_NONE )
  {
    MSG_HIGH("CP-GSM: Session Info Rejected", 0, 0, 0);

    return FALSE;
  }
 
  if ( session_handle != z_umts_cp_gsm_session_status.tm_active_session_handle )
  {
    MSG_HIGH("CP-GSM: Invalid session handle", 0, 0, 0);

    return FALSE;
  }

  switch ( sess_info_type )
  {
    case PRTL_SESS_INFO_PPM: 
      /*
       *  Not relevant to UMTS
       */
      break;

    case PRTL_SESS_INFO_PRM:
      /*
       *  Handle Measurement data
       */
      if(p_sess_info_param->prm_data.z_MeasBlk.e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GPS)
      { 
        if ( z_umts_cp_gsm_session_status.e_agps_mode == 
                                        C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
        {
          MSG_MED("CP-GSM: Handle PRM", 0, 0, 0);

          tm_umts_cp_gsm_handle_prm_data ( &p_sess_info_param->prm_data );
        }
      }  
      break;

    case PRTL_SESS_INFO_LR:
      /*
       *  Handle Location Data
       */  
      MSG_MED("CP-GSM: got LR", 0, 0, 0);

      tm_umts_cp_gsm_handle_lr_data( &p_sess_info_param->lr_data );
      break;

    case PRTL_SESS_INFO_STATUS:
      /*
       *  Handle Aiding Data Wishlist
       */
      MSG_MED("CP-GSM: got SessInfo", 0, 0, 0 );
      
      tm_umts_cp_gsm_handle_sess_info_status( 
                            &p_sess_info_param->aiding_data_status );
      break;

    default:
      break;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_timer_cb_handler

DESCRIPTION
  This function handles timer expirations
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_timer_cb_handler 
( 
  void *timer_data 
)
{
  os_TimerExpiryType                        *timer_param;
  uint32                                    q_timer_id;
 
  MSG_LOW("CP-GSM: timer_cb_handler() called", 0, 0, 0);

  /*
   *  Check for a null pointer here (just in case)
   */ 
  timer_param = (os_TimerExpiryType *)timer_data;
  if( timer_param == NULL)
  {
    MSG_ERROR("CP-GSM: Invalid NULL Timer Data Pointer", 0, 0, 0);
    return;
  }

  /* Expected behaviors of corresponding timers are described in their
     call back functions 
   */
  q_timer_id = timer_param->q_TimerId;
  if( q_timer_id == TM_UMTS_CP_GSM_CF_MAX_MGP_ON_TIMER_ID )
  {
    tm_umts_cp_gsm_handle_max_mgp_on_timer_cb();
  }
  else if ( q_timer_id == TM_UMTS_CP_GSM_ACC_RESET_TIMER_ID )
  {
    tm_umts_cp_gsm_handle_acc_reset_timer_cb();
  }
  else if( q_timer_id == TM_UMTS_CP_GSM_CF_TIMER_ID )
  {
      if( z_umts_cp_gsm_session_status.e_cf_state == C_UMTS_CP_GSM_CF_STATE_NONE )
      {
        MSG_HIGH("CP-GSM: Timer Expired with no session in progress", 0, 0, 0 );
      }
      else
      {
        tm_umts_cp_gsm_handle_cf_timer_cb();
      }
  }
  else
  {
    MSG_ERROR("CP-GSM: Invalid Timer Data Pointer", 0, 0, 0);
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_event_cb_handler

DESCRIPTION
  This function handles the session data pipe from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_gsm_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *p_event_payload
)
{
  MSG_MED("CP-GSM: event_cb_handler(); ev=%d", (int)event_type, 0, 0);

  switch( event_type )
  {
    case PRTL_EVENT_TYPE_LCS:
      tm_umts_cp_gsm_handle_lcs_event(
                        &p_event_payload->lcs_event_payload );
      break;

    case PRTL_EVENT_TYPE_PHONE:
      tm_umts_cp_gsm_handle_phone_event( 
                        &p_event_payload->phone_event_payload );
      break;

    case PRTL_EVENT_TYPE_RR:
      tm_umts_cp_gsm_handle_rr_event( 
                        &p_event_payload->rr_event_payload );
      break;

    case PRTL_EVENT_TYPE_MM:
      tm_umts_cp_gsm_handle_mm_event( 
                        &p_event_payload->mm_event_payload );
      break;

    default:
      MSG_HIGH( "CP-GSM: Invalid Event in event_cb_handler", 0, 0, 0 );
      return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_general_query_handler

DESCRIPTION
  This function handles the start session request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/
boolean tm_umts_cp_gsm_general_query_handler
( prtl_query_e_type query_type, 
  prtl_ans_u_type   *answer_ptr 
)
{
  boolean u_ret_val = FALSE;

  if ( query_type == PRTL_QUERY_SMART_MSB )
  {
    if ( z_umts_cp_gsm_session_status.e_callflow_kind == C_UMTS_CP_GSM_SESSION_NI )
    {
      answer_ptr->ans_smart_msb = PRTL_ANS_SMART_MSB_NOT_ALLOWED;
    }
    else
    {
      answer_ptr->ans_smart_msb = PRTL_ANS_SMART_MSB_ALLOWED;
    }

    u_ret_val = TRUE;
  }

  return u_ret_val;            


}




/*===========================================================================

FUNCTION  tm_umts_cp_gem_ue_capabilities_get

DESCRIPTION
  This function is used by TM RR-IFACE to provide information about the
  positioning capabilities supported by GSM-CP.
  
DEPENDENCIES: none.

RETURN VALUE: none

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_gsm_ue_capabilities_get
(
  gsclassmark_cgps_pos_capability_struct_type   *p_pos_capabilities
)
{
  gsclassmark_cgps_pos_support_enum_type    *p_cgps_pos_support;
  uint32                                    positioning_modes_supported;

  memset( p_pos_capabilities, 0, sizeof( *p_pos_capabilities ) );

  p_cgps_pos_support = &p_pos_capabilities->cgps_pos_support;

  positioning_modes_supported = TM_CORE_CONFIG_DD(positioning_modes_supported);

  if (positioning_modes_supported & PDSM_POS_MODE_STANDALONE)
  {
    p_pos_capabilities->standalone_location_method_supported = TRUE;
  }

  if( (positioning_modes_supported & PDSM_POS_MODE_2G_CP_MS_BASED) &&
      (positioning_modes_supported & PDSM_POS_MODE_2G_CP_MS_ASSISTED) )
  {
    *p_cgps_pos_support = GSCLASSMARK_POS_BOTH;
  } 
  else if (positioning_modes_supported & PDSM_POS_MODE_2G_CP_MS_BASED)
  {
    *p_cgps_pos_support = GSCLASSMARK_POS_UE_BASED;
  } 
  else if (positioning_modes_supported & PDSM_POS_MODE_2G_CP_MS_ASSISTED)
  {
    *p_cgps_pos_support = GSCLASSMARK_POS_NETWORK_BASED;
  }
  else 
  {
    *p_cgps_pos_support = GSCLASSMARK_POS_NONE;
  }

  p_pos_capabilities->lcs_value_added_lcs_supported =
          (z_umts_nv_item_ram_copy.mt_lr_support & 
           NV_AAGPS_MT_LR_CP_GSM_WCDMA_SUPPORTED);  
}


/*===========================================================================

FUNCTION 

DESCRIPTION
  This function handles . 
  
DEPENDENCIES: none.

RETURN VALUE: .

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_gsm_init 
(
  void
)
{
  MSG_LOW("CP-GSM: init called", 0, 0, 0);

  /*
   *  Initialize this module
   */
  tm_umts_cp_gsm_module_init();
}

#endif /* FEATURE_CGPS_UMTS_CP_GSM */

