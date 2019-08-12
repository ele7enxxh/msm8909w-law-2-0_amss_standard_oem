/*======================================================================

               LM task main file

 GENERAL DESCRIPTION
  This file contains the implementation of Location Manager(LM) task.
  This task handles requests from various tasks like TM, MGP, GPS DIAG. 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/lm/src/lm_tm.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
05/26/15   mj    Do not send FORCE IDLE for MSB to SA transitions and vice-versa within MO sessions  
03/20/15   mj    Send FORCE RECVR IDLE during mode change OR tbf change OR apptracking to single shot and vice versa
03/09/15   rk    Update ME with new timeout value during fix session start when the receiver is already On.
01/07/15   rk    Fix the print qualifier.
08/21/14   rk    Fix MGP receiver wasn't set correctly when SI MSA session is followed by Standalone/MSB.
06/07/13   rh    Removed NO_FIX report handling (now BestAvailPos) 
01/22/13   ah    Support extraction of MPG event from union tm_info_s_type
05/18/11   rc    Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
03/03/10   gk    For MSB session, PPM does not need to turn MGP to IDLE
01/12/10   gk    Force receiver off when end session is called
07/24/08   gk    Added on-demand flag for log 1370
07/21/08   ank   Apply stricter Hepe limit than 250m at qos timeout for tunnel-exit scenario.
06/24/08   ank   Reinstate dynamic hepe logic for tunnel-exit scenario.
06/12/08   lt    Support for external coarse position injection.
06/09/08   gk    Add a flag for background fixes
02/08/08   lt    Added support for featurized "allow staled GPS position"
08/01/07   rw    Add support for GPS time info request/response
07/31/07   ank   Added Dynamic QoS and Override logic.
04/13/07   jd    Updated cgps_logGpsFix call to pass NULL ptr for p_Meas parameter
03/27/07   mr    Added Code to save a GET_PPM req, while AFLT_ON is pending
03/27/07   mr    Added 0x1370 position fix log
11/16/06   mr    Added function to send  PD_API GPS_BEGIN/END events to TM 
04/04/06   mr    Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include <stdio.h>
#include "task.h"
#include "msg.h"
#include "aries_os_api.h"

#include "lm_data.h"
#include "tm_lm_iface.h"
#include "lm_mgp.h"
#include "lm_mgp_aflt.h"
#include "sm_api.h"
#include "lm_tm.h"
#include "event.h"
#include "gps_common.h"
#include "gnss_common.h"
#include "lm_diag.h"
#include "math.h"
#include "sm_util.h"

static lm_info_s_type z_lm_info_s;
/*
******************************************************************************
* lm_tm_timeout_finalfix_no_hepe_check 
*
* Function description:
*
* Determines if it is ok to send the final fix at fix timeout
* without any HEPE check constraint.
*
* Parameters: 
*  None
*
* Return value: 
*   TRUE - ship the session Best Fix as final fix.
*   FALSE - do not ship a final fix
*
******************************************************************************
*/
static boolean lm_tm_timeout_finalfix_no_hepe_check(void)
{
  /* There will be a NV in RoW branches to turn this feature off.
     The default value of the NV will be to keep this feature on.
  */

  LM_MSG_HIGH("Fix Timeout, FinalFix No HEPE check [%d,%d,%d]",
    (uint32)z_lmControl.u_sessBestFixValid,
    (uint32)z_lmControl.z_session_request.e_mode,
    z_lmControl.q_sessHandle);

  /*If there was no fix received in this session*/
  if(!z_lmControl.u_sessBestFixValid) {
    LM_MSG_MED("lm_tm_timeout_finalfix_no_hepe_check returned FALSE",0,0,0);
    return FALSE;
  }

  /*Only for MSB session which are originated by the N/W*/
  if ((LM_GPS_MODE_MSB != z_lmControl.z_session_request.e_mode) ||
       (LM_MO_MT_MODE_MT != z_lmControl.z_session_request.e_mo_mt)) {
    LM_MSG_MED("lm_tm_timeout_finalfix_no_hepe_check returned FALSE", 0, 0, 0);
    return FALSE;
  }

  LM_MSG_MED("lm_tm_timeout_finalfix_no_hepe_check returned TRUE",0,0,0);
  return TRUE;
}

/*
 ******************************************************************************
 * sarf_cleanup 
 *
 * Function description:
 *
 * Cleans up SARF state machine vars. Sets them to init.
 *
 * Parameters: 
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void sarf_cleanup( void )
{
  z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_SLEEP;
  if( z_lm_sarf_state_db.e_client_type == LM_SARF_CLIENT_FTM )
  {
    if(z_lm_sarf_state_db.p_client_cb)
    {
      z_lm_sarf_state_db.p_client_cb(FALSE);
    }
  }
  event_report( EVENT_GPS_LM_ERROR_SA_RF_VERIF );
  lm_update_session_state(LM_SESSION_STATE_NO_SESSION);
  z_lmControl.z_session_request.e_mode = LM_GPS_MODE_NONE;
}

/*
 ******************************************************************************
 * lm_send_tm_session_update_ack 
 *
 * Function description:
 *
 * This function sends a STATUS REPORT session update to TM  with a ACK
 *
 * Parameters: 
 * q_handle - LM-TM session handle for which update has to be sent
 * e_Cause  - The cause value to be sent with the update
 *
 * Return value: 
 *
 * boolean - session update successfully sent to TM or not
 *
 ******************************************************************************
*/

boolean lm_send_tm_session_update_ack(uint32 q_handle, tm_status_cause_e_type e_Cause)
{
  tm_session_update_info_s_type z_sessUpdate;
  LM_MSG_MED("Sending session update ACK to TM",0,0,0);

  z_sessUpdate.q_handle = q_handle;
  z_sessUpdate.e_update_type = TM_INFO_STATUS_REPORT;
  z_sessUpdate.z_update.z_status_rpt.e_status = TM_STATUS_ACK;
  z_sessUpdate.z_update.z_status_rpt.e_cause = e_Cause;

   return tm_session_update(&z_sessUpdate);

}

/*
 ******************************************************************************
 * lm_send_tm_session_update_nack 
 *
 * Function description:
 *
 * This function sends a STATUS REPORT session update to TM  with a NACK
 *
 * Parameters: 
 * q_handle - LM-TM session handle for which update has to be sent
 * e_Cause  - The cause value to be sent with the update
 *
 * Return value: 
 *
 * boolean - session update successfully sent to TM or not
 *
 ******************************************************************************
*/

boolean lm_send_tm_session_update_nack(uint32 q_handle, tm_status_cause_e_type e_Cause)
{
  tm_session_update_info_s_type z_sessUpdate;

  LM_MSG_MED("Sending session update NACK to TM",0,0,0);

  z_sessUpdate.q_handle = q_handle;
  z_sessUpdate.e_update_type = TM_INFO_STATUS_REPORT;
  z_sessUpdate.z_update.z_status_rpt.e_status = TM_STATUS_NACK;
  z_sessUpdate.z_update.z_status_rpt.e_cause = e_Cause;

   return tm_session_update(&z_sessUpdate);

}

/*
 ******************************************************************************
 * lm_send_tm_session_request_abort 
 *
 * Function description:
 *
 * This function sends a abort request to TM when a error occurs
 *
 * Parameters: 
 * q_handle - LM-TM session handle for which abort request has to be sent
 * e_Cause  - The cause value for abort
 *
 * Return value: 
 *
 * boolean - session request successfully sent to TM or not
 *
 ******************************************************************************
*/

boolean lm_send_tm_session_request_error_notify(uint32 q_handle, tm_session_error_cause_e_type e_Cause)
{
  tm_session_request_s_type z_sessReq;
  LM_MSG_MED("Sending session ERROR NOTIFICATION to TM. Error Cause [%d]",(uint32)e_Cause,0,0);

  z_sessReq.q_handle = q_handle;
  z_sessReq.e_request_type = TM_REQUEST_ERROR_NOTIFY;
  z_sessReq.z_request.e_errorCause = e_Cause;

   return tm_session_request(&z_sessReq);

}

/*
 ******************************************************************************
 * lm_tm_send_position_report_to_tm
 *
 * Function description:
 *
 * This function sends position report to TM
 *
 * Parameters: 
 *
 * p_fixReport   - fix report to be sent to TM
 * u_finalReport - if the report is a final report for session or an intermediate
 *                 diag report.
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_position_report_to_tm(const sm_GnssFixRptStructType* const p_fixReport,
                                         boolean u_finalReport)
{
  tm_session_update_info_s_type p_update;

  if(p_fixReport == NULL)
  {
    LM_MSG_ERROR("NULL fix report received",0,0,0);
    return FALSE;
  }
  
  p_update.q_handle = z_lmControl.q_sessHandle;

  p_update.z_update.z_fix_rpt.z_GpsFixRpt = *p_fixReport;
  /*Populate the time reamining to the fix timeout*/
  p_update.z_update.z_fix_rpt.q_FixTimeRemainingMSec = 
  os_TimerCheck(z_lmControl.p_fixTimer); 
    

  if(u_finalReport)
  {
    /*Send final position fix report*/
    LM_MSG_MED("Sending FINAL_FIX report to TM.Time remaining to fix timeout: %lu",
         p_update.z_update.z_fix_rpt.q_FixTimeRemainingMSec,0,0);
    p_update.e_update_type = TM_INFO_FINAL_FIX_REPORT;
  }
  else
  {
    /*Send intermediate QoS qualified fix*/
    LM_MSG_MED("Sending INTERMEDIATE_FIX report to TM. Time remaining to fix timeout: %lu",
         p_update.z_update.z_fix_rpt.q_FixTimeRemainingMSec,0,0);
    p_update.e_update_type = TM_INFO_INTERMEDIATE_FIX_REPORT;
  }

  return tm_session_update(&p_update);
  
}

/*
 ******************************************************************************
 * lm_tm_send_sa_data_request_to_tm 
 *
 * Function description:
 *
 * This function sends a SA Data request from MGP to TM
 *
 * Parameters: 
 *
 * Return value: 
 *
 * boolean - session request successfully sent to TM or not
 *
 ******************************************************************************
*/

boolean lm_tm_send_sa_data_request_to_tm(void)
{
  tm_session_request_s_type z_sessReq;
  LM_MSG_MED("Sending SA Data request to TM.",0,0,0);

  z_sessReq.q_handle = z_lmControl.q_sessHandle;
  z_sessReq.e_request_type = TM_REQUEST_SA_DATA;

   return tm_session_request(&z_sessReq);
}

/*
 ******************************************************************************
 * lm_tm_send_prm_report_to_tm
 *
 * Function description:
 *
 * This function sends GPS PRM report to TM
 *
 * Parameters: 
 *
 * p_gpsReport   - PRM report to be sent to TM
 * u_delayed     - Is this a late report?(Arrived after PRM timer timedout.
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/

boolean lm_tm_send_prm_report_to_tm(const sm_GpsMeasRptStructType* const p_measReport, boolean u_late)
{
  tm_session_update_info_s_type p_update;
  
  if(p_measReport == NULL)
  {
    LM_MSG_ERROR("NULL measurement report received",0,0,0);
    return FALSE;
  }

  if(u_late)
  {
    LM_MSG_MED("Sending late PRM  report to TM",0,0,0);
  p_update.e_update_type = TM_INFO_LATE_PRM_REPORT;
  }
  else
  {
  LM_MSG_MED("Sending PRM  report to TM",0,0,0);
  p_update.e_update_type = TM_INFO_PRM_REPORT;
  }

  p_update.q_handle = z_lmControl.q_sessHandle;


  p_update.z_update.z_measure_rpt.z_GpsMeasRpt = *p_measReport;



  return tm_session_update(&p_update);
  
}


/*
 ******************************************************************************
 * lm_tm_send_sv_poly_report_to_tm
 *
 * Function description:
 *
 * This function sends SV Ploynomial report to TM
 *
 * Parameters: 
 *
 * p_svPolyReport   - SV Polynomial report to be sent to TM
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/

boolean lm_tm_send_sv_poly_report_to_tm(const sm_SvPolyReportType* const p_svPolyReport)
{
  tm_session_update_info_s_type p_update;
  
  if(p_svPolyReport == NULL)
  {
    LM_MSG_ERROR("NULL Poly report received",0,0,0);
    return FALSE;
  }

  LM_MSG_MED("Sending Poly  report to TM",0,0,0);
  p_update.e_update_type = TM_INFO_SV_POLY_REPORT;

  p_update.q_handle = z_lmControl.q_sessHandle;


  p_update.z_update.z_sv_poly_rpt.z_SvPolyRpt = *p_svPolyReport;



  return tm_session_update(&p_update);
  
}


/*
 ******************************************************************************
 * lm_tm_send_ppm_report_to_tm
 *
 * Function description:
 *
 * This function sends GPS PRM report to TM
 *
 * Parameters: 
 *
 * p_gpsReport   - PRM report to be sent to TM
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_ppm_report_to_tm(const srch_ppm_rpt_type* const p_ppmReport)
{
  tm_session_update_info_s_type p_update;

  if(p_ppmReport == NULL)
  {
    LM_MSG_ERROR("NULL PPM report received",0,0,0);
    return FALSE;
  }
  
  #ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif
  {
    LM_MSG_MED("Sending PPM  report to TM",0,0,0);
    p_update.q_handle = z_lmControl.q_sessHandle;
  }


  p_update.z_update.z_ppm_rpt.z_PpmRpt = *p_ppmReport;

  p_update.e_update_type = TM_INFO_PPM_REPORT;


  return tm_session_update(&p_update);
  
}

/*
 ******************************************************************************
 * lm_tm_send_assist_data_status_report_to_tm
 *
 * Function description:
 *
 * This function sends Assitance data status + aiding data wishlist report to TM
 *
 * Parameters: 
 *
 * p_assistStatusReport   - Assistance data report to be sent to TM
 * u_forKeepWarm         - Is the status a part of keep warm process
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_assist_data_status_report_to_tm
(const sm_InternalAssistDataStatusStructType* const p_assistStatusReport, boolean u_forKeepWarm)
{
  tm_info_s_type z_info;
      
  LM_MSG_MED("Sending Assitance data status report to TM",0,0,0);

  if(p_assistStatusReport == NULL)
  {
    LM_MSG_ERROR("NULL  Assitance data status report received",0,0,0);
    return FALSE;
  }
  
  
  z_info.e_info_type = TM_INFO_AIDING_DATA_WISHLIST;
  z_info.z_info.z_wishlist.z_assist_data_status = *p_assistStatusReport;
  z_info.z_info.z_wishlist.for_keepwarm         = u_forKeepWarm;


  return tm_info(&z_info);
  
}

/*
 ******************************************************************************
 * lm_tm_send_position_estimate_report_to_tm
 *
 * Function description:
 *
 * This function sends Assitance data status + aiding data wishlist report to TM
 *
 * Parameters: 
 *
 * p_pos_estimate   - POS estimate report to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_position_estimate_report_to_tm
(const sm_GnssFixRptStructType* const p_pos_estimate)
{
  tm_info_s_type z_info;

  if(p_pos_estimate == NULL)
  {
    LM_MSG_ERROR("NULL  position estimate report received",0,0,0);
    return FALSE;
  }
  
  LM_MSG_MED("Sending position estimate report to TM",0,0,0);
  
  z_info.e_info_type = TM_INFO_POS_ESTIMATE;
  z_info.z_info.z_pos_estimate.z_GpsFixRpt = *p_pos_estimate;

  return tm_info(&z_info);
  
}

/*
 ******************************************************************************
 * lm_tm_send_pd_api_event_to_tm
 *
 * Function description:
 *
 * This function sends PD_API event info to TM
 *
 * Parameters: 
 *
 * e_pd_api_event   - PD API event to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_pd_api_event_to_tm
(tm_pd_api_event_e_type  e_pd_api_event)
{
  tm_info_s_type z_info;
      
  LM_MSG_MED("Sending PD_API event %d to TM",e_pd_api_event,0,0);

  
  z_info.e_info_type = TM_INFO_PD_API_EVENT;
  z_info.z_info.e_pdapi_event = e_pd_api_event;

  return tm_info(&z_info);
  
}

/*
 ******************************************************************************
 * lm_tm_send_rf_info_to_tm
 *
 * Function description:
 *
 * This function sends RF info to TM
 *
 * Parameters: 
 *
 * p_rf_info   - RF INFO to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_rf_info_to_tm
(sm_ReportRfInfoStructType* p_rf_info)
{
  tm_info_s_type z_info;

  if(p_rf_info == NULL)
  {
  return FALSE;
  }
  
  LM_MSG_MED("Sending RF INFO to TM",0,0,0);
  
  z_info.e_info_type = TM_RF_INFO;
  z_info.z_info.z_rf_info.z_rf_info_struct = *p_rf_info;

  return tm_info(&z_info);
  
}

/*
 ******************************************************************************
 * lm_tm_send_gps_time_to_tm
 *
 * Function description:
 *
 * This function sends GPS time to TM
 *
 * Parameters: 
 *
 * p_rf_info   - RF INFO to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_gps_time_to_tm
(
  sm_ReportGpsTimeStructType *p_gps_time_report
)
{
  tm_info_s_type z_info;

  if(p_gps_time_report == NULL)
  {
    return FALSE;
  }
  
  LM_MSG_MED("Sending GPS time to TM",0,0,0);
  
  z_info.e_info_type = TM_GPS_TIME;
  z_info.z_info.z_gps_time_info.z_gps_time_info_struct = *p_gps_time_report;

  return tm_info(&z_info);
  
}

/*
 ******************************************************************************
 * lm_tm_send_mgp_event_to_tm
 *
 * Function description:
 *
 * This function sends Events from MGP to TM
 *
 * Parameters: 
 *
 * p_mgp_event - MGP event info to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_mgp_event_to_tm (sm_ReportEventStructType* p_mgp_event)
{
  tm_info_mgp_event_s_type z_info;

  memset((void*)&z_info, 0, sizeof(z_info));

  if (p_mgp_event == NULL)
  {
    return FALSE;
  }

  LM_MSG_LOW("Sending MGP EVENT %d to TM", (uint32)p_mgp_event->e_EventType, 0, 0);

  z_info.e_info_type = TM_INFO_MGP_EVENT;
  z_info.z_mgp_event = *p_mgp_event;

  return tm_info_mgp_event(&z_info);
}

/*
 ******************************************************************************
 * lm_tm_clear_session 
 *
 * Function description:
 *
 * This function clears the session state related variables
 *
 * Parameters: 
 *
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_clear_session()
{
  LM_MSG_MED("Clearing LM-TM sesion with handle[%d]",z_lmControl.q_sessHandle,0,0);
  z_lmControl.q_reportsPending = 0;
  z_lmControl.q_sessHandle = 0;
  lm_update_session_state(LM_SESSION_STATE_NO_SESSION);
  z_lmControl.u_sessBestFixValid = FALSE;
  z_lmControl.f_sessBestHepe     = 0;
  memset((void*)&z_lmControl.z_sessBestFixRpt, 0, sizeof(sm_GnssFixRptStructType));
  z_lmControl.u_lastFixRptValid  = FALSE;
  z_lmControl.u_lastFixWasNoFixRpt = FALSE;
  memset((void*)&z_lmControl.z_lastFixRpt, 0, sizeof(sm_GnssFixRptStructType));
  z_lmControl.u_MgpONParamsValid = FALSE;
  z_lmControl.u_ppm_request_pending = FALSE;
  z_lmControl.u_ppm_get_results_pending = FALSE;
  lm_set_waitingAAData(FALSE);
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = FALSE;
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = 0;
  qw_set(z_lmControl.z_early_exit_decision.d_TimeSessionStart,0,0);
  z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = 0;
}

/*
 ******************************************************************************
 * lm_tm_start_new_session 
 *
 * Function description:
 *
 * This function handles the session request  for starting a session, fix, PPM
 * or PRM in LM.
 *
 * Parameters: 
 * p_startReq - Pointer to structure containing start request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_start_new_session(const lm_session_request_info_s_type* const p_startReq)
{  
  boolean u_send_force_idle = FALSE;
  
  if(NULL == p_startReq)
  {
    LM_MSG_ERROR("NULL start request received",0,0,0);
    return;
  }  
     
  /* When the receiver is ON, and there is an incoming E911 session OR mode change OR single shot to apptracking OR vice versa
  	 the ME may go into DPO sleep. During this time, SM still believe the engine is ON and does not send another ON command.
  	 Due to this the RCVR may not wake up and may cause delay in handling the session.
  	 To facilitate ME, we are changing receiver state from ON to IDLE(if already ON). And then later turn it ON again with the operation mode
	 in its parameter. This will help ME wake up to handle the incoming session and also have the correct mode update.
   */
  /* Send FORCE IDLE only when from SM's perspective the receiver is ON*/ 
  if ((LM_MGP_STATE_ON == z_lmControl.e_currentMgpState) ||
  	  (LM_MGP_STATE_GOING_ON == z_lmControl.e_currentMgpState))      
  {
    /* If incoming new session is E911, issue a FORCE IDLE */
    if (TRUE == p_startReq->z_request.z_session_request.b_e911_session)
    {      
	  u_send_force_idle = TRUE;
    }
	/* If mode has changed, issue a FORCE IDLE */
	else if ((z_lmControl.z_session_request.e_mode != p_startReq->z_request.z_session_request.e_mode))
	{
	  /* Even if the  GPS modes are different but if both the requests are of type MO, then no need to force MGP to Idle.
	     Currently this is treated valid only if one is of MSB and the other one is of Standalone. */
	  if (((LM_MO_MT_MODE_MO == z_lmControl.z_session_request.e_mo_mt) && 
	       (LM_MO_MT_MODE_MO == p_startReq->z_request.z_session_request.e_mo_mt)) &&
	      (((LM_GPS_MODE_MSB == z_lmControl.z_session_request.e_mode) &&
	        (LM_GPS_MODE_STANDALONE == p_startReq->z_request.z_session_request.e_mode)) ||
	       ((LM_GPS_MODE_STANDALONE == z_lmControl.z_session_request.e_mode) &&
	        (LM_GPS_MODE_MSB == p_startReq->z_request.z_session_request.e_mode))))
	  {
	    u_send_force_idle = FALSE;
	  }
	  else
	  {
	    u_send_force_idle = TRUE;
        MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mode changed from %d to %d", 
          z_lmControl.z_session_request.e_mode,p_startReq->z_request.z_session_request.e_mode);
      }
	}
	/* If apptracking to single shot session, issue a FORCE IDLE */
	else if ((z_lmControl.z_session_request.q_num_fixes > 1) && 
			 (1 == p_startReq->z_request.z_session_request.q_num_fixes))
	{	  
	  u_send_force_idle = TRUE;
	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Apptracking to Single Shot transition");
	}
	/* If single shot session to apptracking, issue a FORCE IDLE */
	else if ((1 == z_lmControl.z_session_request.q_num_fixes) && 
			 (p_startReq->z_request.z_session_request.q_num_fixes > 1))
	{
	  u_send_force_idle = TRUE;
	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Single Shot to Apptracking transition");
	}
	/* If apptracking to apptracking with TBF change, issue a FORCE IDLE */
	else if ((z_lmControl.z_session_request.q_num_fixes > 1) && 
			 (p_startReq->z_request.z_session_request.q_num_fixes > 1) &&
			 (z_lmControl.z_session_request.q_tbf != p_startReq->z_request.z_session_request.q_tbf))
	{
	  u_send_force_idle = TRUE;
	  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Apptracking to Apptracking transition with TBF change from %u to %u", 
	  	z_lmControl.z_session_request.q_tbf,p_startReq->z_request.z_session_request.q_tbf);
	}
	else
	{
	  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RCVR in ON state, but no FORCE IDLE sent. On-going session's mode=%d, No. of fixes=%u, TBF=%u", 
	  	z_lmControl.z_session_request.e_mode,z_lmControl.z_session_request.q_num_fixes,z_lmControl.z_session_request.q_tbf);
	}
  }
	
  if (u_send_force_idle)
  {
	MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Force RCVR IDLE, current RCVR state %u",z_lmControl.e_currentMgpState); 
    /* Put MGP in forced IDLE mode if not in this mode already */
    if ( !lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE, NULL, TRUE) )
    {
	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed to put MGP in forced IDLE state. But continuing the session anyway");
    }
  }  
    
  LM_MSG_MED("Starting new LM-TM sesion with mode[%d] TBF[%lu] num_fixes[%lu]",
             p_startReq->z_request.z_session_request.e_mode,
             p_startReq->z_request.z_session_request.q_tbf,
             p_startReq->z_request.z_session_request.q_num_fixes);

  LM_MSG_MED("UP/CP mode[%d],MO/MT[%d],Sess[%d]",
    (uint32)p_startReq->z_request.z_session_request.e_up_cp,
    (uint32)p_startReq->z_request.z_session_request.e_mo_mt,
    p_startReq->q_handle); 

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Emergency 911 session in progress = [%u]", 
	  	(uint32)p_startReq->z_request.z_session_request.b_e911_session);
  
  z_lmControl.q_sessHandle = p_startReq->q_handle;
  lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
  z_lmControl.z_session_request = p_startReq->z_request.z_session_request;
  /*Report event*/
  event_report_payload(EVENT_GPS_LM_SESSION_START,
                       sizeof(lm_gps_mode_e_type),
                       (void*)&p_startReq->z_request.z_session_request.e_mode);
  
  if(LM_GPS_MODE_FTM == z_lmControl.z_session_request.e_mode)
  {
  /*If this is FTM mode turn MGP to IDLE*/  
  LM_MSG_MED("FTM Mode. Turning MGP to IDLE",0,0,0);
  /*Change MGP state to IDLE*/
    (void)lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE,NULL,FALSE);  
  }
  
}

/*
 ******************************************************************************
 * lm_tm_stop_session
 *
 * Function description:
 *
 * This function handles the stopping of a session request
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_stop_session(void)
{

if((z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX) ||
   (z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS))
{
  LM_MSG_MED("Stopping FIX,GPS and PPM timers",0,0,0);  
  /*lint -e534 */
  os_TimerStop(z_lmControl.p_fixTimer);  
  os_TimerStop(z_lmControl.p_gpsTimer);  
  /*lint +e534 */
 
  lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_session_request.q_num_fixes,
                                          z_lmControl.z_session_request.q_tbf);
}

if(LM_GPS_MODE_FTM == z_lmControl.z_session_request.e_mode)
{
  /*If this is FTM mode turn MGP to OFF*/  
  LM_MSG_MED("FTM Mode. Turning MGP to OFF",0,0,0);
  /*Change MGP state to OFF*/
  (void)lm_mgp_update_mgp_state(LM_MGP_STATE_OFF,NULL,FALSE);  
}
  
#if defined(FEATURE_CGPS_USES_CDMA)
  lm_mgp_aflt_stop_ppm_processing();
#endif /*FEATURE_CGPS_USES_CDMA*/

lm_tm_clear_session();

/*Report event*/
event_report(EVENT_GPS_LM_SESSION_END);
  
}


/*
 ******************************************************************************
 * lm_tm_continue_session
 *
 * Function description:
 *
 * This function updated parameters of a an ongoing LM-TM session. Currently only
 * the operation_mode (MSA,MSB etc.) parameter is updated
 *
 * Parameters: 
 * p_contReq - continue request from TM 
 *
 * Return value: 
 *
 * TRUE/FALSE: operation successful or not
 *
 ******************************************************************************
*/
boolean lm_tm_continue_session(const lm_session_request_session_s_type* const p_contReq)
{
  if(p_contReq == NULL)
  {
    LM_MSG_ERROR("NULL continue request received",0,0,0);
    return FALSE;
  }  

  LM_MSG_HIGH("Updating session operation mode from %d to %d",
               z_lmControl.z_session_request.e_mode,p_contReq->e_mode,0);
  LM_MSG_MED("MO/MT %d,%d",
               z_lmControl.z_session_request.e_mo_mt ,p_contReq->e_mo_mt ,0);
  

  if (z_lmControl.z_session_request.e_mode != p_contReq->e_mode)
  {
    if( (z_lmControl.z_session_request.e_mode == LM_GPS_MODE_MSB) &&
        (p_contReq->e_mode == LM_GPS_MODE_MSA))
    {
      /* Changing from MSB to MSA, we need to stop the fix timer*/
      MSG_LOW("Stopping Fix timer",0,0,0);
      os_TimerStop(z_lmControl.p_fixTimer); /*lint !e534 */
    }
  }
  z_lmControl.z_session_request.e_mode = p_contReq->e_mode; 

  /*Report event*/
  event_report_payload(EVENT_GPS_LM_SESSION_CONTINUE,
                       sizeof(lm_gps_mode_e_type),
                       (void*)&p_contReq->e_mode);
  
  return TRUE;
}

/*
 ******************************************************************************
 * lm_tm_handle_session_request_start 
 *
 * Function description:
 *
 * This function handles the session request  for starting a session, fix, PPM
 * or PRM in LM.
 *
 * Parameters: 
 * p_startReq - Pointer to structure containing start request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void lm_tm_handle_session_request_start(const lm_session_request_info_s_type* const p_startReq)
{

  if(p_startReq == NULL)
  {
    LM_MSG_ERROR("NULL start request received",0,0,0);
    return;
  }
  
  
  if(p_startReq->q_req_mask & (uint32)LM_SESSION_REQUEST_SESSION)
  {

    LM_MSG_MED("Received SESSION start request from TM",0,0,0);
    if(z_lmControl.e_sessionState != LM_SESSION_STATE_NO_SESSION)
    {
      /*There is a session already active. send a NACK for new session request to TM*/
      if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_PREVIOUS_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Start a new session*/
      lm_tm_start_new_session(p_startReq);
    }

  } /*if (LM_SESSION_REQUEST_SESSION) ends*/


  if(p_startReq->q_req_mask & (uint32)LM_SESSION_REQUEST_FIX)
  {

    LM_MSG_MED("Received FIX start request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_startReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
    /*Check if this report is already pending. If yes, then no need to start it again. If not, 
      start the report related processing*/
      if( (z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING) &&
          (z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX))
      {
        /*Report processing already in progress. Do nothing*/
        LM_MSG_ERROR("A previous FIX request already pending",0,0,0);    
      }
      else
      {
        /*Start position fix procesing*/
        if(!lm_mgp_start_position_fix_processing(&p_startReq->z_request.z_fix_request))
        {
          LM_MSG_MED("Failed to start fix processing.Sending NACK for handle[%d]",p_startReq->q_handle,0,0);
          if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
          {
            LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
          }
          /*lm_cleanup_session();*/
          return;
        }
      }

    }

  } /*if (LM_SESSION_REQUEST_FIX) ends*/


  if(p_startReq->q_req_mask & (uint32)LM_SESSION_REQUEST_GPS)
  {

    LM_MSG_MED("Received GPS start request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_startReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
    /*Check if this report is already pending. If yes, then no need to start it again. If not, 
      start the report related processing*/
      if( (z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING) &&
          (z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS))
      {
        /*Report processing already in progress. Do nothing*/
        LM_MSG_ERROR("A previous PRM request already pending",0,0,0);    
      }
      else
      {
        /* Copy Fix request params*/
        z_lmControl.z_gps_request = p_startReq->z_request.z_gps_request;
        /*Start position fix procesing*/
        if(!lm_mgp_start_prm_processing(&p_startReq->z_request.z_gps_request))
        {
          LM_MSG_MED("Failed to start PRM processing.Sending NACK for handle[%d]",p_startReq->q_handle,0,0);
          if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
          {
            LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
          }
          /*lm_cleanup_session();*/
          return;
        }
      }
    }

  } /*if (LM_SESSION_REQUEST_GPS) ends*/
                    
  #ifdef FEATURE_CGPS_USES_CDMA
  if (p_startReq->q_req_mask & (uint32) LM_SESSION_REQUEST_CDMA) 
  {

    LM_MSG_MED("Received CDMA start request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_startReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Check if this report is already pending. If yes, then no need to start it again. If not, 
      start the report related processing*/
      if(LM_MGP_AFLT_STATE_ON == z_lmControl.e_afltState || 
         z_lmControl.u_ppm_request_pending == TRUE)
      {
        /*Report processing already in progress. Do nothing*/
        LM_MSG_ERROR("A previous PPM request already pending",0,0,0);  
      }
      else
      {
        /* Copy PPM request params*/
        z_lmControl.z_cdma_request = p_startReq->z_request.z_cdma_request;
        
        if ((LM_MGP_STATE_ON == z_lmControl.e_currentMgpState) &&
           ((LM_GPS_MODE_1X_MSA == z_lmControl.z_session_request.e_mode))) 
        {
          /*Turn MGP to IDLE if its currently ON*/
          LM_MSG_MED("MGP found ON when PPM request is received. Turning MGP IDLE",0,0,0);  
          (void) lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE,NULL,TRUE);        
          z_lmControl.u_ppm_request_pending = TRUE;
          /*We don't want to send ACK back to TM yet*/
          return;
        }
        else
        {
          /*Start position fix procesing*/
          if(
             !lm_mgp_aflt_start_ppm_processing(&p_startReq->z_request.z_cdma_request)
            )
          {
            LM_MSG_MED("Failed to start PRM processing.Sending NACK for handle[%d]",p_startReq->q_handle,0,0);
            if(!lm_send_tm_session_update_nack(p_startReq->q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
            {
               LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
            }
             
            /*lm_cleanup_session();*/
            return;
          }
        }/*else for if(LM_MGP_STATE_ON == z_lmControl.e_currentMgpState)*/
      }/*else for if( (z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING) &&
          (z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_CDMA))*/
    }/*else for if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  */

  } /*if (LM_SESSION_REQUEST_CDMA) ends*/
  #endif /*FEATURE_CGPS_USES_CDMA*/
  /*If we reached here that means everything went OK with the request processing.
    Send an ACK back to TM*/
  if(!lm_send_tm_session_update_ack(p_startReq->q_handle,TM_STATUS_ACK_OK))
  {
    LM_MSG_ERROR("Unable to send session update ACK to TM. Stopping the entire session",0,0,0);
    /*Stop a session*/
    lm_tm_stop_session();
  }
}

/*
 ******************************************************************************
 * lm_tm_handle_session_request_stop 
 *
 * Function description:
 *
 * This function handles the session request  for stopping a session, fix, PPM
 * or PRM in LM.
 *
 * Parameters: 
 * p_stopReq - Pointer to structure containing stop request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void lm_tm_handle_session_request_stop(const lm_session_request_info_s_type* const p_stopReq)
{

  if(p_stopReq == NULL)
  {
    LM_MSG_ERROR("NULL stop request received",0,0,0);
    return;
  }
  
  if(p_stopReq->q_req_mask & (uint32)LM_SESSION_REQUEST_FIX)
  {

    LM_MSG_MED("Received FIX stop request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_stopReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_stopReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {

      /*Stop position fix procesing*/
      lm_mgp_stop_position_fix_processing(p_stopReq->z_request.z_fix_request.u_force_mgp_off);

      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }

    }

  } /*if (LM_SESSION_REQUEST_FIX) ends*/


  if(p_stopReq->q_req_mask & (uint32)LM_SESSION_REQUEST_GPS)
  {

    LM_MSG_MED("Received GPS stop request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_stopReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_stopReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Stop PRM processing*/
      lm_mgp_stop_prm_processing();

      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }

    }

  } /*if (LM_SESSION_REQUEST_GPS) ends*/
                    
  #ifdef FEATURE_CGPS_USES_CDMA
  if(p_stopReq->q_req_mask & (uint32)LM_SESSION_REQUEST_CDMA)
  {

    LM_MSG_MED("Received CDMA stop request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_stopReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_stopReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Stop PPM procesing*/
      lm_mgp_aflt_stop_ppm_processing();

      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }
      
    }

  } /*if (LM_SESSION_REQUEST_CDMA) ends*/ 
  #endif /*FEATURE_CGPS_USES_CDMA*/
  if(p_stopReq->q_req_mask & (uint32)LM_SESSION_REQUEST_SESSION)
  {

    LM_MSG_MED("Received SESSION stop request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
    {
      /*There is no session already active to be stopped. 
        Send a NACK to TM*/
      if(!lm_send_tm_session_update_nack(p_stopReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Stop a session*/
      lm_tm_stop_session();
      //lm_tm_clear_session();
      //lm_send_tm_session_update_ack(p_stopReq->q_handle,TM_STATUS_ACK_OK);
    }

  } /*if (LM_SESSION_REQUEST_SESSION) ends*/

  /*If we reached here that means everything went OK with the request processing.
    Send an ACK back to TM*/
  if(!lm_send_tm_session_update_ack(p_stopReq->q_handle,TM_STATUS_ACK_OK))
  {
    LM_MSG_ERROR("Unable to send session update ACK to TM. Stopping the entire session",0,0,0);
    /*Stop a session*/
    lm_tm_stop_session();
  }
}

/*
 ******************************************************************************
 * lm_tm_handle_session_request_continue 
 *
 * Function description:
 *
 * This function handles the session request  for updating params of fix, PPM
 * or PRM processing in LM.
 *
 * Parameters: 
 * p_continueReq - Pointer to structure containing continue request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void lm_tm_handle_session_request_continue(const lm_session_request_info_s_type* const p_continueReq)
{
  if(p_continueReq == NULL)
  {
    LM_MSG_ERROR("NULL continue request received",0,0,0);
    return;
  }

  z_lmControl.q_sessHandle = p_continueReq->q_handle;
  if(p_continueReq->q_req_mask & (uint32)LM_SESSION_REQUEST_SESSION)
  {

    LM_MSG_MED("Received SESSION continue request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      /*Update session procesing params*/
      if(!lm_tm_continue_session(&p_continueReq->z_request.z_session_request))
      {
        if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
        {
          LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
        }
        return;
      }

    }

  } /*if (LM_SESSION_REQUEST_SESSION) ends*/  

    
  if(p_continueReq->q_req_mask & (uint32)LM_SESSION_REQUEST_FIX)
  {

    LM_MSG_MED("Received FIX continue request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      if(!(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX))
      {
        /*No position fix report pending. Do nothing*/
      }
      else
      {
        /*Update position fix procesing params*/

        /* update relevant request params: timeout and accuracy */

        if(!lm_mgp_continue_position_fix_processing(&p_continueReq->z_request.z_fix_request))
        {
          if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
          {
            LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
          }
          return;
        }

        z_lmControl.z_fix_request.q_qos_accuracy_threshold = p_continueReq->z_request.z_fix_request.q_qos_accuracy_threshold;
        LM_MSG_MED("SessionContinueReq updating Qos_accuracy to %d", z_lmControl.z_fix_request.q_qos_accuracy_threshold, 0, 0 );

      }

    }

  } /*if (LM_SESSION_REQUEST_FIX) ends*/


  if(p_continueReq->q_req_mask & (uint32)LM_SESSION_REQUEST_GPS)
  {

    LM_MSG_MED("Received GPS continue request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      if(!(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS))
      {
        /*No GPS processing in progress. Do nothing*/
      }
      else
      {
        /*continue PRM processing*/
        if(!lm_mgp_continue_prm_processing(&p_continueReq->z_request.z_gps_request))
        {
          if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
          {
            LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
          }
          return;
        }
      }
    }

  } /*if (LM_SESSION_REQUEST_GPS) ends*/
                    
  #ifdef FEATURE_CGPS_USES_CDMA
  if(p_continueReq->q_req_mask & (uint32)LM_SESSION_REQUEST_CDMA)
  {

    LM_MSG_MED("Received CDMA continue request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    {
      if(z_lmControl.e_afltState == LM_MGP_AFLT_STATE_OFF)
      {
        /*No PPM processing in progress. Do nothing*/
        LM_MSG_ERROR("Continue CDMA request received when AFLT is OFF",0,0,0);  
      }
      else
      {
        /*Update PPM procesing params*/
        if(!lm_mgp_aflt_continue_ppm_processing(&p_continueReq->z_request.z_cdma_request))
        {
          if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
          {
            LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
          }
          return;
        }
      }
    }

  } /*if (LM_SESSION_REQUEST_CDMA) ends*/


  if(p_continueReq->q_req_mask & (uint32)LM_SESSION_REQUEST_CDMA_GET_RESULTS)
  {

    LM_MSG_MED("Received CDMA GET RESULTS continue request from TM",0,0,0);
    if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)  
    {
      LM_MSG_MED("No Session active sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
      if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_NO_SESSION_ACTIVE))
      {
        LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
      }
      return;
    }
    else
    if (z_lmControl.e_afltState != LM_MGP_AFLT_STATE_ON)
    {
      if(z_lmControl.u_ppm_request_pending)
      {
        /*We received a GET_PPM request while we were waiting for MGP to go IDLE.
          Save this request to be processed later*/  
        LM_MSG_MED("AFLT ON pending for MGP to turn IDLE. Saving GET_PPM request..",0,0,0);
        z_lmControl.u_ppm_get_results_pending = TRUE;  
      }
      else
      {
      
        LM_MSG_MED("AFLT not ON. Sending NACK for handle[%d]",p_continueReq->q_handle,0,0);
        if(!lm_send_tm_session_update_nack(p_continueReq->q_handle,TM_STATUS_NACK_CAUSE_INVALID_PARAM))
        {
          LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
        }
        return;
      }
    }
    else
    {
      /*Request PPM result from CDMA searcher*/
      lm_mgp_aflt_get_results(FALSE);
      lm_update_session_state(LM_SESSION_STATE_IN_SESSION_REPORT_PENDING);
      z_lmControl.q_reportsPending |= (uint32)LM_SESSION_REQUEST_CDMA;

    }

  } /*if (LM_SESSION_REQUEST_CDMA_GET_RESULTS) ends*/
  #endif /*FEATURE_CGPS_USES_CDMA*/


  /*If we reached here that means everything went OK with the request processing.
    Send an ACK back to TM*/
  if(!lm_send_tm_session_update_ack(p_continueReq->q_handle,TM_STATUS_ACK_OK))
  {
    LM_MSG_ERROR("Unable to send session update ACK to TM. Stopping the entire session",0,0,0);
    /*Stop a session*/
    lm_tm_stop_session();
  }
}
/*
 ******************************************************************************
 * lm_tm_handle_session_request 
 *
 * Function description:
 *
 * This function handles the session request message from TM.
 * This message is sent by TM for:
 * 1.  starting/stopping/updating a session with LM
 * 2.  To request a PPM report, PRM report or position report
 *     for an already established session.
 *
 * Parameters: 
 * p_msg - Pointer to structure containing received message
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_tm_handle_session_request(const os_IpcMsgType* const p_msg)
{
  lm_session_request_info_s_type *p_sessReq;

  if(p_msg == NULL)
  {
    LM_MSG_ERROR("NULL OS message received",0,0,0);
    return;
  }

  /*lint -e826 */
  p_sessReq = (lm_session_request_info_s_type*) p_msg->p_Data;
  /*lint +e826 */
  
  switch(p_sessReq->e_req_type)
  {

  case LM_SESSION_REQUEST_START:
    {
      lm_tm_handle_session_request_start(p_sessReq);
      break;
    }       
  case LM_SESSION_REQUEST_STOP:
    {
      lm_tm_handle_session_request_stop(p_sessReq);
      break;
    }
  case LM_SESSION_REQUEST_CONTINUE:
    {
      lm_tm_handle_session_request_continue(p_sessReq);
      break;
    }

  }/*switch(p_sessReq->e_req_type) ends*/ 

}


/*
 ******************************************************************************
 * lm_tm_handle_set_parameter
 *
 * Function description:
 * This function handles set parameter request from TM
 * 
 *
 * Parameters: 
 * *p_Req - set parameter request from TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_set_parameter(const lm_request_set_param_s_type* const p_Req)
{

   if(p_Req == NULL)
   {
     LM_MSG_ERROR("Received NULL Set parameter request from TM",0,0,0);
     return;
   }
    
   LM_MSG_MED("Received SET PARAMETER request from TM",0,0,0);
   switch (p_Req->e_param_type)
   {
   case   LM_SET_PARAM_APP_TRK_GPS_ON_THRESHOLD:
       {
         lm_set_rc_on_threshold_period(p_Req->z_param_value.q_gpsOnThreshold);
         break;
       }
   case LM_SET_PARAM_APP_TRK_GPS_IDLE_THRESHOLD:
       {
         lm_set_rc_idle_threshold(p_Req->z_param_value.q_gpsIdleThreshold);
         break;
       }
   case LM_SET_PARAM_APP_TRK_DELAY_MARGIN:
       {
         lm_set_rc_shut_off_delay_margin(p_Req->z_param_value.q_delayMargin);
         break;
       }
   }
}


/*
 ******************************************************************************
 * lm_tm_handle_nmea_on
 *
 * Function description:
 * This function handles NMEA ON request from TM
 * 
 *
 * Parameters: 
 * *p_NmeaOnRequest -  NMEA ON request from TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_nmea_on(const lm_request_nmea_s_type* const p_NmeaOnRequest)
{
  if(p_NmeaOnRequest != NULL)
  {
    z_lmControl.q_NmeaBitmask = p_NmeaOnRequest->q_bitmask;
    LM_MSG_MED("NMEA ON request from TM. NMEA bitmask set to: %d",z_lmControl.q_NmeaBitmask,0,0);
  }
}

/*
 ******************************************************************************
 * lm_tm_handle_nmea_off
 *
 * Function description:
 * This function handles NMEA OFF request from TM
 * 
 *
 * Parameters: 
 * 
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_nmea_off(void)
{
    z_lmControl.q_NmeaBitmask = 0;
    LM_MSG_MED("NMEA OFF request from TM",0,0,0);
}

/*
 ******************************************************************************
 * lm_tm_handle_request 
 *
 * Function description:
 *
 * This function handles the session independent request message from TM.
 * This message is sent by TM to send a LM-TM session independent request to LM.
 * The type of request could be:
 * 1.  Turn keep warm processing ON/OFF
 * 2.  Turn NMEA sentence generation ON/OFF
 * 3.  Get MGP's Aiding data wishlist
 * 4.  Delete certain parameters in MGP database
 *
 * Parameters: 
 * p_req - Pointer to structure containing received message
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_tm_handle_request(const os_IpcMsgType* const p_msg)
{
  lm_request_info_s_type *p_Req;

  if(p_msg == NULL)
  {
    LM_MSG_ERROR("NULL request recieved from TM",0,0,0);
    return;
  }

  /*lint -e826 */
  p_Req = (lm_request_info_s_type*) p_msg->p_Data;
  /*lint +e826 */
  
  switch(p_Req->e_req_type)
  {

  case LM_REQUEST_AIDING_DATA_WISHLIST:
    {
      lm_mgp_handle_aiding_wishlist_request(p_Req->z_request.e_wishlist_req_type);
      break;
    }       
  case LM_REQUEST_DELETE_MGP_INFO:
    {
      lm_mgp_handle_delete_mgp_info(&p_Req->z_request.z_delete_request);
      break;
    }
  case LM_REQUEST_SET_PARAM:
    {
      lm_tm_handle_set_parameter(&p_Req->z_request.z_set_param_request);
      break;
    }
  case LM_REQUEST_NMEA_ON:
    {
      lm_tm_handle_nmea_on(&p_Req->z_request.z_nmea_on_request);
      break;
    }
  case LM_REQUEST_NMEA_OFF:
    {
      lm_tm_handle_nmea_off();
      break;
    }
  case LM_REQUEST_KEEP_WARM_ON:
  case LM_REQUEST_KEEP_WARM_OFF:
    {
      /*Do nothing. No keep warm support for now*/
      LM_MSG_MED("No keep warm support currently",0,0,0);
      break;
    }
  case LM_REQUEST_POS_ESTIMATE:
    {
      lm_mgp_handle_pos_estimate_request();  
      break;
    }
  case LM_REQUEST_SLOW_CLK_TIME_INJECT:
    {
      lm_mgp_handle_slow_clk_time_inject_request(&p_Req->z_request.z_slow_clk_inject_request);
      break;  
    }

  case LM_REQUEST_EXTERNAL_COARSE_POS_INJECT:
    {
      lm_mgp_handle_external_coarse_pos_inject_request(&p_Req->z_request.z_extern_coarse_pos_inject_request);
      break;  
    }

  case LM_REQUEST_RF_INFO:
    {
      lm_mgp_handle_rf_info_request(&p_Req->z_request.z_rf_info_request);
      break;  
    } 

  case LM_REQUEST_GNSS_TIME:
    {
      lm_mgp_handle_gps_time_request(&p_Req->z_request.z_gps_time_request);
      break;  
    } 
  case LM_REQUEST_FORCE_RECVR_OFF:
    {
      lm_mpg_handle_force_rcvr_off();
      break;
  }

  case LM_REQUEST_RECVR_OFF:
    {
      lm_mpg_handle_rcvr_off();
      break;    
  }

  case LM_REQUEST_RESET_LOCATION_SERVICE:
    {
      lm_mpg_handle_reset_location_service();
      break;
    }
  }/*switch(p_Req->e_req_type) ends*/ 

}

/*
 ******************************************************************************
 * lm_tm_handle_info 
 *
 * Function description:
 *
 * This function handles the session independent info message from TM. 
 * This message contains LM-TM session independent info from TM.
 * The information type can be:
 * 1.  Aiding data to be injected in MGP.
 * 2.  Expected network delays for getting aiding data.
 *
 * Parameters: 
 * p_info - Pointer to structure containing received message
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_tm_handle_info(const os_IpcMsgType* const p_msg)
{
  lm_info_s_type *p_Info = &z_lm_info_s;
  
  if(p_msg == NULL)
  {
    LM_MSG_ERROR("NULL info recieved from TM",0,0,0);
    return;
  }

  memset((void*)p_Info, 0, sizeof(lm_info_s_type));
  

  /*lint -e826 */
  if(lm_translate_lm_info_ipc_type_to_lm_info((lm_info_s_type_ipc_type*) p_msg->p_Data,
                                            p_Info) == FALSE)
  {
    LM_MSG_ERROR("LM Info Ipc transalation Failure",0,0,0);
    return;
  }
  
  /*lint +e826 */
  
  switch(p_Info->e_info_type)
  {

  case LM_SESSION_INFO_AIDING_DATA:
    {
      LM_MSG_LOW("Received %d(GPS/GLO) %d AIDING DATA from TM", p_Info->z_info.z_aiding_data.z_assist_data.e_GnssType,
                 p_Info->z_info.z_aiding_data.z_assist_data.e_AssistanceType, 0);
      lm_mgp_handle_aiding_data_info(&p_Info->z_info.z_aiding_data);
      break;
    }       
  case LM_SESSION_INFO_AIDING_DATA_DELAY:
    {
      /*Not currently handled. Do Nothing*/
      LM_MSG_LOW("Received AIDING DATA DELAY from TM",0,0,0);
      break;
    }
  case LM_SESSION_INFO_POS_UNC:  
    {
      LM_MSG_LOW("Received New POS UNC [%f] from TM",p_Info->z_info.f_new_pos_unc
                 ,0,0);
      lm_mgp_handle_pos_unc(p_Info->z_info.f_new_pos_unc);
      break;    
    }
  case LM_SESSION_INFO_POS_OFFSET:
    {
      LM_MSG_LOW("Received POS OFFSET[%d] from TM",p_Info->z_info.q_pos_offset
                 ,0,0);
      lm_mgp_handle_pos_offset(p_Info->z_info.q_pos_offset);
      break;  
    }
  case LM_SESSION_INFO_TIME_UNC:
    {
      LM_MSG_LOW("Received New TIME UNC[%f] from TM",p_Info->z_info.f_new_time_unc
                 ,0,0);
      lm_mgp_handle_time_unc(p_Info->z_info.f_new_time_unc);
      break;    
    }
  case LM_SESSION_INFO_TIME_OFFSET:   
    {
      LM_MSG_LOW("Received TIME Offset[%f] from TM",p_Info->z_info.f_time_offset
                 ,0,0);
      lm_mgp_handle_time_offset(p_Info->z_info.f_time_offset);
      break;    
    }
  case LM_SESSION_INFO_SESSION_TYPE:
    LM_MSG_LOW("Received Session Type from TM", p_Info->z_info.z_sess_info.e_mode, p_Info->z_info.z_sess_info.e_srv_system, 0);
    lm_mgp_handle_session_type(p_Info->z_info.z_sess_info);
    break;

  }/*switch(p_Info->e_info_mask) ends*/ 

}


/*
 ******************************************************************************
 * lm_tm_handle_final_position_report
 *
 * Function description:
 * 
 * This function handles the final position fix report from MGP. It stops the 
 * session related timers and sets appropriate session state. The final fix
 * is sent to TM. MGP engine state after the position fix is decided. 
 *
 * Parameters: 
 * *p_fixReport - Final position fix report
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_final_position_report(const sm_GnssFixRptStructType* const p_fixReport)
{
  if(p_fixReport == NULL)
  {
    LM_MSG_ERROR("NULL fix report received",0,0,0);
    return;
  }
  
  /*Stop the fix timer*/
  /*lint -e534 */
  os_TimerStop(z_lmControl.p_fixTimer);
  /*lint +e534 */
  
  /*Send position report to TM and generate NMEA if enabled*/
  /*lint -e506 */
  if(!lm_tm_send_position_report_to_tm(p_fixReport,(boolean)TRUE))
  {
    LM_MSG_ERROR("Unable to send position report to TM. Clearing entire session",0,0,0);
    lm_tm_stop_session();
    return;
  }
  /*lint +e506 */
  
  /*Update fix session related state*/
  z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_FIX); 

  if(z_lmControl.q_reportsPending == 0)
  {
    lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
  }
  /*Generate 0x1370 log*/

  /* translation from sm_GnssNavSolution to gps_NavSolution */
  do{ /* to create new stack */
     gnss_NavSolutionStructType z_NavSolution;
   gnss_NavSolutionStructType *pz_NavSolution = NULL;
   
   memset((void*)&z_NavSolution,0,sizeof(z_NavSolution));
   pz_NavSolution = &z_NavSolution;
   
     if(sm_translate_sm_fix_report_to_gnss_nav_solution(pz_NavSolution, p_fixReport, TRUE,
       z_lmControl.z_fix_request.u_on_demand))
     {
        /* log the 0x1476 log packet from LM. */
        gnss_LogPositionReport(pz_NavSolution, NULL);
     }
  } while(0);

  /*Maintain proper MGP state*/
  lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_session_request.q_num_fixes,
                                          z_lmControl.z_session_request.q_tbf);
}
/*
 ******************************************************************************
 * lm_tm_hepe_check
 *
 * Function description:
 *
 * This function checks if the position fix reported by MGP qualifies the 
 * Max(250m, QoS-HEPE-Requested) provided by TM or not.
 *
 * Parameters: 
 * p_fixReport - Last good Position fix from MGP.
 *
 * Return value: 
 *
 * TRUE - if HEPE check passed
 * FALSE - if HEPE check failed
 *
 ******************************************************************************
*/
boolean lm_tm_hepe_check(const sm_GnssFixRptStructType* const p_fixReport)
{
   FLT f_hepe = 0;
   uint32 q_TimeFromEngOn,q_TimeFromSessStart;
   uint32 q_hepe_max = z_lmControl.z_fix_request.q_qos_accuracy_threshold;

   if(p_fixReport == NULL)
   {
     return FALSE;
   }

   f_hepe = (FLT) sqrt((p_fixReport->z_NavPos.f_ErrorEllipse[1] * 
                        p_fixReport->z_NavPos.f_ErrorEllipse[1]) + 
                       (p_fixReport->z_NavPos.f_ErrorEllipse[2] * 
                        p_fixReport->z_NavPos.f_ErrorEllipse[2]));

   if(z_lmControl.z_early_exit_decision.u_UseQoSOverride)
   {
     q_hepe_max = z_lmControl.q_HepeThreshold;
   }
   else
   {
     if(z_lmControl.z_fix_request.q_qos_accuracy_threshold < z_lmControl.q_HepeThreshold)
     {
       q_hepe_max = z_lmControl.q_HepeThreshold;
     }
   }

   /* Override Hepe limit at timeout after tunnel-exit esp to handle low qos setting */
   if((z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe == TRUE) && 
      (z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate == FALSE) &&
      (z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart == TRUE))
   {
     LM_MSG_MED("Timeout hit after reinstate!",0,0,0);
     q_hepe_max = LM_REINSTATE_TIMEOUT_HEPE_LIMIT;
     z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate = TRUE;
   }

   if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_HIGH ) )
   {
      char b_Buffer[250]; 
      (void) snprintf( b_Buffer, sizeof(b_Buffer), "Override=%d Hepe=%f HepeMax=%lu\r\n",
             z_lmControl.z_early_exit_decision.u_UseQoSOverride,f_hepe, q_hepe_max);
      MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);
   }

   /* Calculate time from Session Start */
   q_TimeFromSessStart = lm_get_time_from_start(LM_TIME_FROM_SESSION_START);
   /* Calculate time from which MGP engine is ON */
   q_TimeFromEngOn = lm_get_time_from_start(LM_TIME_FROM_ENGINE_ON);

   if(f_hepe <= q_hepe_max)
   {
     z_lmControl.z_early_exit_decision.e_decision = LM_EARLY_EXIT_DECISION_REPORT_FIX_TIMEOUT;
     z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
     z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = FALSE;
     lm_log_early_exit_decision(q_TimeFromEngOn, q_TimeFromSessStart,p_fixReport->z_NavPos.q_RefFCount, 0, 0, (uint16) q_hepe_max, (uint16) f_hepe, TRUE);
     /*lint -e506 */
     return TRUE;
   }
   else
   {
     z_lmControl.z_early_exit_decision.e_decision = LM_EARLY_EXIT_DECISION_REPORT_NO_FIX_TIMEOUT;
     lm_log_early_exit_decision(q_TimeFromEngOn, q_TimeFromSessStart,p_fixReport->z_NavPos.q_RefFCount, 0, 0, (uint16) q_hepe_max, (uint16) f_hepe, TRUE);
     return FALSE;
   }
}

/*
 ******************************************************************************
 * lm_tm_handle_fix_timer_timeout
 *
 * Function description:
 * This function handles positon fix timer timeout. If a fix that didn't pass
 * QoS accuracy check is available from this session, send it over to TM, else
 * send the error to TM. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_fix_timer_timeout(void)
{
  LM_MSG_MED("Fix timer timedout for handle[%d]",z_lmControl.q_sessHandle,0,0);

  /*If there is no FIX report pending, this timeout was queued just when
  fix timer was stopped. Do nothing in this case*/
  if(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX)
  {
    #if !defined ( FEATURE_STALE_POS_ALLOWED ) /* regular case, FEATURE_STALE_POS_ALLOWED not defined*/
    /* means stale position is not generally allowed. This is the usually 
       way as this feature is not by default included */
    if( (z_lmControl.u_lastFixRptValid) && (lm_tm_hepe_check(&z_lmControl.z_lastFixRpt)) &&
        ((z_lmControl.z_fix_request.q_qos_accuracy_threshold == 0) ||
       (z_lmControl.z_fix_request.q_qos_accuracy_threshold != 0 && !z_lmControl.u_lastFixWasNoFixRpt)) )
    {
      /* If a non-qualified fix available & HEPE is acceptable report it only 
         for E911 calls(qos is passed zero for UMTS/GSM E911)*/
      lm_tm_handle_final_position_report(&z_lmControl.z_lastFixRpt);
      z_lmControl.u_lastFixRptValid = FALSE; /*This fix report should not be used again
                                               once it is reported*/
    z_lmControl.u_lastFixWasNoFixRpt = FALSE;
    }
    
    #else /* FEATURE_STALE_POS_ALLOWED */
    if( z_lmControl.u_lastFixRptValid && lm_tm_hepe_check(&z_lmControl.z_lastFixRpt) )
    {
      /* If a non-qualified fix available & HEPE is acceptable, report it now*/
      lm_tm_handle_final_position_report(&z_lmControl.z_lastFixRpt);
      z_lmControl.u_lastFixRptValid = FALSE; /*This fix report should not be used again
                                               once it is reported*/
      z_lmControl.u_lastFixWasNoFixRpt = FALSE;
    }
    #endif /* FEATURE_STALE_POS_ALLOWED */
    else if(lm_tm_timeout_finalfix_no_hepe_check())
    {
      /* this final fix is sent only for MT MSB sessions */
      LM_MSG_HIGH("Sending MT MSB Best Fix At Timeout as Final Fix", 0, 0, 0);
      lm_tm_handle_final_position_report(&z_lmControl.z_sessBestFixRpt);
      z_lmControl.u_sessBestFixValid = FALSE;
    }
    else
    {
      /*Update fix session related state*/
      z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_FIX); 

      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }

      /*Log QoS Timeout NO-FIX log*/  
      do 
      { /* create new stack */
         gnss_NavSolutionStructType z_navSolution;
     memset((void*)&z_navSolution,0,sizeof(z_navSolution));
     
         z_navSolution.z_NavSolutionFlags.b_IsBackground = SM_UTIL_MAKE_BOOL(z_lmControl.z_fix_request.u_on_demand);
         z_navSolution.z_NavSolutionFlags.b_IsFinal = TRUE;

         /* log the 0x1476 log packet from LM. */
         gnss_LogPositionReport(&z_navSolution, NULL); 
         /* count = NULL because we're not keeping track of how many messages we've sent out */
      } while(0);

      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle, TM_ERROR_CAUSE_FIX_TIMEOUT))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        lm_tm_stop_session();
        return;
      }

      /*If this is a cold start, initial fixes may fail. Keep MGP engine in proper state for
       application based tracking*/
      lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_session_request.q_num_fixes,
                                              z_lmControl.z_session_request.q_tbf);

    }

  }

}

/*
 ******************************************************************************
 * lm_tm_handle_gps_timer_timeout
 *
 * Function description:
 * This function handles GPS PRM timer timeout. 
 * It sends the error to TM. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_gps_timer_timeout(void)
{
  LM_MSG_MED("GPS PRM timer timedout for handle[%d]",z_lmControl.q_sessHandle,0,0);

  /*If there is no PRM report pending, this timeout was queued just when
  PRM timer was stopped. Do nothing in this case*/
  if(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS)
  {
    /*Update fix session related state*/
    z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_GPS); 

    if(z_lmControl.q_reportsPending == 0)
    {
      lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
    }

    if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle, TM_ERROR_CAUSE_PRM_TIMEOUT))
    {
      LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
      lm_tm_stop_session();
      return;
    }

    /*If this is a cold start, initial fixes may fail. Keep MGP engine in proper state for
     application based tracking*/
    lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_session_request.q_num_fixes,
                                            z_lmControl.z_session_request.q_tbf);

  }
  
}
#ifdef FEATURE_CGPS_USES_CDMA
/*
 ******************************************************************************
 * lm_tm_handle_cdma_timer_timeout
 *
 * Function description:
 * This function handles AFLT CDMA timer timeout. 
 * It sends the error to TM. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_cdma_timer_timeout(void)
{
  LM_MSG_MED("CDMA PPM timer timedout for handle[%d]",z_lmControl.q_sessHandle,0,0);

  /*If AFLT if OFF, this timeout was queued just when
    CDMA timer was stopped. Do nothing in this case*/
  if(z_lmControl.e_afltState == LM_MGP_AFLT_STATE_ON)
  {
    /*Stop PPM processing*/
    lm_mgp_aflt_stop_ppm_processing();

    if(z_lmControl.e_sessionState != LM_SESSION_STATE_NO_SESSION)
    {
    
      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }

      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle, TM_ERROR_CAUSE_PPM_TIMEOUT))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        lm_tm_stop_session();
        return;
      }
      
    }

  }

}
#endif /*FEATURE_CGPS_USES_CDMA*/
/*
 ******************************************************************************
 * lm_tm_handle_goingOn_timer_timeout
 *
 * Function description:
 * This function handles GOING_ON timer timeout. 
 * It sends the error to TM. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_goingOn_timer_timeout(void)
{
  LM_MSG_ERROR("GOING ON Timer timedout while in state %d",z_lmControl.e_sessionState,0,0);
  /*Start a timer for getting the OFF notification from MGP*/
  if(!os_TimerStart(z_lmControl.p_goingOFFTimer,LM_GOING_OFF_TIMEOUT,0))
  {
    LM_MSG_ERROR("Failed starting GOING_OFF timer",0,0,0);
  }  
  /*Turn receiver off in any case*/
  mgp_ReceiverOff();
  z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_OFF;
  z_lmControl.e_desiredMgpState = LM_MGP_STATE_OFF;
      
  if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
  {
  }
  else
  {
    /*Report abort state to TM if not already reported a previous non-recoverable error*/
    if(z_lmControl.e_sessionState != LM_SESSION_STATE_IN_SESSION_ERROR_REPORTED)
    {
      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle,TM_ERROR_MGP_STATUS_TIMEOUT))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        lm_tm_stop_session();
        return;
      }
    }
  }
  if( z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_ON )
  {
    sarf_cleanup();
  }
}

/*
 ******************************************************************************
 * lm_tm_handle_goingOff_timer_timeout
 *
 * Function description:
 * This function handles GOING_OFF timer timeout. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_goingOff_timer_timeout(void)
{
  LM_MSG_ERROR("GOING OFF Timer timedout while in state %d",z_lmControl.e_sessionState,0,0);
  /*Turn receiver off in any case*/
  /*Start a timer for getting the OFF notification from MGP*/
  if(!os_TimerStart(z_lmControl.p_goingOFFTimer,LM_GOING_OFF_TIMEOUT,0))
  {
    LM_MSG_ERROR("Failed starting GOING_OFF timer",0,0,0);
  }
    
  mgp_ReceiverOff();
  z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_OFF;
  z_lmControl.e_desiredMgpState = LM_MGP_STATE_OFF;

  if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
  {
  }
  else
  {
    /*Report abort state to TM if not already reported a previous non-recoverable error*/
    if(z_lmControl.e_sessionState != LM_SESSION_STATE_IN_SESSION_ERROR_REPORTED)
    {
      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle,TM_ERROR_MGP_STATUS_TIMEOUT))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        lm_tm_stop_session();
        return;
      }
    }
  }

  if( z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_OFF )
  {
    sarf_cleanup();
  }

}

/*
 ******************************************************************************
 * lm_tm_handle_goingIdle_timer_timeout
 *
 * Function description:
 * This function handles GOING_IDLE timer timeout. 
 * 
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_tm_handle_goingIdle_timer_timeout(void)
{
  LM_MSG_ERROR("GOING IDLE Timer timedout while in state %d",z_lmControl.e_sessionState,0,0);
  /*Turn receiver off in any case*/
  /*Start a timer for getting the OFF notification from MGP*/
  if(!os_TimerStart(z_lmControl.p_goingOFFTimer,LM_GOING_OFF_TIMEOUT,0))
  {
    LM_MSG_ERROR("Failed starting GOING_OFF timer",0,0,0);
  }
  mgp_ReceiverOff();
  z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_OFF;
  z_lmControl.e_desiredMgpState = LM_MGP_STATE_OFF;

  if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
  {
  }
  else
  {
    /*Report abort state to TM if not already reported a previous non-recoverable error*/
    if(z_lmControl.e_sessionState != LM_SESSION_STATE_IN_SESSION_ERROR_REPORTED)
    {
      if(!lm_send_tm_session_request_error_notify(z_lmControl.q_sessHandle,TM_ERROR_MGP_STATUS_TIMEOUT))
      {
        LM_MSG_ERROR("Failed to send error notification to TM. Clearing entire session",0,0,0);  
        lm_tm_stop_session();
        return;
      }
    }
  }
  if( z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_ON ||
      z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_OFF)
  {
    sarf_cleanup();
  }
}


