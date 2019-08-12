/*======================================================================

                        Location Manager MGP interaction file

GENERAL DESCRIPTION
 This file contains functions by which Location Manager(LM) handles all 
 the MGP related processing. 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/lm/src/lm_mgp.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

 when      who   what, where, why
 ----      ----  -------------------------------------------------------
08/28/15   mj     Add support for INS only fixes for SAP changes
04/29/15   jv    QZSS Support added.
03/09/15   rk    Update ME with new timeout value during fix session start when the receiver is already On.
10/24/14   jv    Added Galileo Support
09/26/14   rk    Update ME with correct session timeout in the MSA to stop  probational scanning close to session timeout.
08/25/14   jv    16-bit SVID Support
01/15/14   rk    Resolve magnetic variation of GPRMC is zero by removing duplicate variable.
10/08/13   ah    Moved Tunnel Detection enablement bit to resolve bit conflict
08/22/13   mj    Reinstate Dynamic Hepe logic for session continue for MSB apptracking for tunnel exit scenario
06/27/13   rk    Fix the malformed message output.
06/07/13   rh    Removed NO_FIX report handling (now BestAvailPos)
09/12/12   rh    Added SV freshness filtering featurized by FEATURE_GNSS_SV_FRESHNESS_FILTERING
11/08/11   rh    Converts SBAS SVID from internal(120~151) to external(33~64)
11/08/11   rh    Tracking decommissioned SVs
06/22/11   rh    Added PRM/PPM/SA/AA data in OEM layer
05/18/11   rc    Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
02/13/11   gk    eph throttling rework. 
06/18/10   gk    For single shot fixes, no need to align to sec. 
05/21/10   gk    Reset dynamic QoS thresolds if TBF >=5 to match behavior of DPO off case
02/10/10   vp    OEM DR Data Enablement
02/17/10   lt    Populated slow clk source of injected time (GPS / GLO)
01/12/10   gk    Force receiver off when end session is called
06/10/09   gk    ODP 2.0 support
08/13/08   gk    Added force receiver off
07/24/08   gk    Added a field for on-demand in 1370 log
07/21/08   ank   Apply stricter Hepe limit than 250m at qos timeout for tunnel-exit scenario.
06/24/08   ank   Reinstate dynamic hepe logic for tunnel-exit scenario.
06/12/08   lt    Support for external coarse position injection.
01/03/08   lt    If receiver reports the same state, it is no longer processed.
11/14/07   ank   For late ramp, double the ramp slope to reach Hmax at (Tmax+Tr/2)
08/01/07   rw    Add support for XTRA feature, get GPS time info
07/31/07   ank   Added Dynamic QoS and Override logic.
07/17/07   ank   Corrected the logic to qualify final fix.
05/11/07   mr    Removed call to mgp_ComputeGpsFix() to strobe MGP for fix
11/16/06   mr    Added generation of PD_API GPS_BEGIN/END events to TM 
6/20/06    mr    Initial creation of header file.
======================================================================*/
#include "gps_variation.h"
#include <stdio.h>
#include "lm_api.h"
#include "lm_iface.h"
#include "lm_data.h"
#include "mgp_api.h"
#include "aries_os_api.h"
#include "lm_mgp.h"
#include "lm_mgp_aflt.h"
#include "lm_tm.h"
#include "sm_log.h"
#include "event.h"
#include "time_svc.h"
#include "math.h"
#include "tm_api.h"
#include "sm_util.h"
#include "sm_nv.h"
#include "sm_oemdre.h"
#include "sm_oemdrsync.h"
#include "sm_oemdata.h"
#include "mgp_api.h"
#include "sm_api.h"
#include "sm_util.h"

#ifndef __GRIFFON__
#include "lbs_sdp.h"
#endif // __GRIFFON__

/****************** Function Definitions**************************************/
static void lm_mgp_fix_oem_data(const sm_GnssFixRptStructType* p_fixReport);
static void lm_mgp_sv_poly_oem_data(const gnss_SvPoly* p_svPolyReport);
static void lm_mgp_pos_meas_oem_data(const sm_GpsMeasRptStructType* p_measReport);

/*
******************************************************************************
* lm_rcvr_state_change_notification
*
* Function description:
*  This function is a hook where other GPS modules on the modem can add 
*  their code which depends on MGP receiver state changes. Other modules
*  should just send a notification to their task from here and not add
*  any processing / logic for their respective modules in this function.
*
*  If for some reason this is not possible, please talk with SM folks 
*  "BEFORE" making any changes.
*
* Parameters: 
*  u_EventData - enum lm_log_event_data_mgp_state
*
* Return value:
*  none
******************************************************************************
*/
void lm_rcvr_state_change_notification(uint8 u_EventData)
{
  switch(u_EventData)
  {

#ifndef __GRIFFON__

  case LM_LOG_EVENT_DATA_MGP_STATE_ON:
    sdp_gnss_state_change_notification_proxy(SDP_GNSS_STATE_ENGINE_ON);
    /* add other notifiers that need this indication here */

    break;

  case LM_LOG_EVENT_DATA_MGP_STATE_OFF:
    sdp_gnss_state_change_notification_proxy(SDP_GNSS_STATE_ENGINE_OFF);
    /* add other notifiers that need this indication here */

      break;

#endif //__GRIFFON__

  default:
    break;
  }
}

/*
******************************************************************************
* sm_log_event_report_ex 
*
* Function description:
*  Helper event generation function allowing to add a place to add in hooks
*  for notification generation to other GPS modules.
*
* Parameters: 
*  u_EventId   - enum lm_log_event_id
*  u_EventData - enum lm_log_event_data_mgp_state
*
* Return value:
*  none
******************************************************************************
*/
void sm_log_event_report_ex(uint8 u_EventId, uint8 u_EventData)
{
  switch(u_EventId)
  {
  case LM_LOG_EVENT_ID_MGP_STATE_CHANGE:
    lm_rcvr_state_change_notification(u_EventData);
    break;

  default:
    break;
  }

  sm_log_event_report(u_EventId, u_EventData);
}

/*
 ******************************************************************************
 * lm_mgp_set_sess_best_fix_report 
 *
 * Function description:
 *  This function saves the lowest HEPE fix for a session
 *
 * Parameters: 
 *  pFixReport - fix report received from PE
 *
 * Return value:
 *  none
 ******************************************************************************
*/
static void lm_mgp_set_sess_best_fix_report(sm_GnssFixRptStructType* pFixReport)
{
  FLT f_hepe;
  
  LM_MSG_HIGH("Set Best Sess Fix: [%d]", z_lmControl.u_sessBestFixValid, 0, 0);

  if(C_GPS_WEEK_UNKNOWN == pFixReport->z_NavPos.w_GpsWeek)
  {
    LM_MSG_HIGH("Fix with no GPS time, ignored for session best", 0, 0, 0);
    return;
  }

  if(!z_lmControl.u_sessBestFixValid)
  {
    z_lmControl.z_sessBestFixRpt = *pFixReport;
    z_lmControl.u_sessBestFixValid = TRUE;

    z_lmControl.f_sessBestHepe = (FLT) sqrt((pFixReport->z_NavPos.f_ErrorEllipse[1] * 
                                             pFixReport->z_NavPos.f_ErrorEllipse[1]) + 
                                            (pFixReport->z_NavPos.f_ErrorEllipse[2] * 
                                             pFixReport->z_NavPos.f_ErrorEllipse[2]));

    if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
    {
      char b_Buffer[250]; 
      (void)snprintf(b_Buffer, sizeof(b_Buffer), "New fix saved as best. hepe: %f", z_lmControl.f_sessBestHepe);
      MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buffer); 
    }

    return;
  }

  f_hepe = (FLT) sqrt((pFixReport->z_NavPos.f_ErrorEllipse[1] * 
                       pFixReport->z_NavPos.f_ErrorEllipse[1]) + 
                      (pFixReport->z_NavPos.f_ErrorEllipse[2] * 
                       pFixReport->z_NavPos.f_ErrorEllipse[2]));

  if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_HIGH ) )
  {
    char b_Buffer[250]; 
    (void)snprintf(b_Buffer, sizeof(b_Buffer), "HEPE New: %f, Old: %f", f_hepe, z_lmControl.f_sessBestHepe); 
    MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer ); 
  }

  if(f_hepe < z_lmControl.f_sessBestHepe)
  {
    if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
    {
      char b_Buffer[250]; 
      (void)snprintf(b_Buffer, sizeof(b_Buffer), "New fix saved as best. hepe: %f", f_hepe);
      MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buffer);
    }

    z_lmControl.z_sessBestFixRpt = *pFixReport;
    z_lmControl.f_sessBestHepe = f_hepe;
  }
}

/*
 ******************************************************************************
 * lm_mgp_get_mgp_operation_mode 
 *
 * Function description:
 *
 * This function gets corresponding mgp_ConfigOperationEnumType for a given
 * lm_gps_mode_e_type
 *
 * Parameters: 
 * lm_gps_mode_e_type - operation mode enumeration in lm_gps_mode_e_type 
 *
 * Return value: 
 *
 * mgp_ConfigOperationEnumType - operation mode enumeration in 
 * mgp_ConfigOperationEnumType
 *
 ******************************************************************************
*/
mgp_ConfigOperationEnumType lm_mgp_get_mgp_operation_mode(lm_gps_mode_e_type e_lm_mode)
{
  switch(e_lm_mode)
  {
  case LM_GPS_MODE_MSA:
    return MGP_OPERATION_MSASSISTED; 
  case LM_GPS_MODE_1X_MSA:
    return MGP_OPERATION_1X_MSASSISTED; 
  case LM_GPS_MODE_MSB:
    return MGP_OPERATION_MSBASED;
  case LM_GPS_MODE_FTM:
    return MGP_OPERATION_FTM;
  case LM_GPS_MODE_SA_RF:
    return MGP_OPERATION_STANDALONE_RF_VERIF;
  case LM_GPS_MODE_STANDALONE:
    return MGP_OPERATION_STANDALONE;
  case LM_GPS_MODE_ODP:
    return MGP_OPERATION_ODP;
  default:
    return MGP_OPERATION_MAX;
  }
}

/*
 ******************************************************************************
 * lm_mgp_get_mgp_state_type 
 *
 * Function description:
 *
 * This function gets corresponding lm_mgp_state_e_type for a given
 * gps_RcvrStateEnumType
 *
 * Parameters: 
 * gps_RcvrStateEnumType - MGP state enumeration in gps_RcvrStateEnumType 
 *
 * Return value: 
 *
 * lm_mgp_state_e_type - MGP state enumeration in 
 * lm_mgp_state_e_type
 *
 ******************************************************************************
*/
lm_mgp_state_e_type lm_mgp_get_mgp_state_type(gps_RcvrStateEnumType e_rcvrState)
{
  switch(e_rcvrState)
  {
  case C_GPS_RCVR_STATE_OFF:
    return LM_MGP_STATE_OFF;

  case C_GPS_RCVR_STATE_ON:
  case C_GPS_RCVR_STATE_KEEP_WARM_DFRD_OFF:
  case C_GPS_RCVR_STATE_KEEP_WARM_DFRD_IDLE:
  case C_GPS_RCVR_STATE_DPO_SLEEP:            /* DPO Sleep is a transition state for Rcvr ON */
    return LM_MGP_STATE_ON;

  case C_GPS_RCVR_STATE_IDLE: 
    return LM_MGP_STATE_IDLE;
  default:
    return LM_MGP_STATE_INVALID; /*Invalid state*/      
  }

}


/*
 ******************************************************************************
 * lm_reset_dynamic_hepe_threshold 
 *
 * Function description:
 *
 * This function resets the dynamic hepe threshold limits 
 *
 * Parameters: 
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void lm_reset_dynamic_hepe_thresold(void)
{
  LM_MSG_MED("Reset Dynamic Hepe thresholds",0,0,0);

  /* Reset time of first reportable fix */
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = 0;
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn = FALSE;
  (void) time_get(z_lmControl.z_early_exit_decision.d_TimeEngOn);
  z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn = FALSE;

  z_lmControl.z_early_exit_decision.f_LastValidFixHepe = LM_LAST_VALID_FIX_HEPE_MAX;
  z_lmControl.z_early_exit_decision.q_NullFixReportCount = 0;
  z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
  z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate = FALSE;
  z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = FALSE;
}
/*
 ******************************************************************************
 * lm_get_time_from_start 
 *
 * Function description:
 *
 * This function gets corresponding time from Engine ON/ Fix start depending on enum
 *
 * Parameters: 
 *
 * Return value: Time in ms
 *
 *
 ******************************************************************************
*/
uint32 lm_get_time_from_start(lm_get_time_e_type e_StartType)
{
  qword d_CurrentTime = {0}, d_TimeDiff = {0}, d_TimeDiffMs = {0};

  (void) time_get(d_CurrentTime);

  switch(e_StartType)
  {
  case LM_TIME_FROM_ENGINE_ON:

    qw_sub(d_TimeDiff,d_CurrentTime,z_lmControl.z_early_exit_decision.d_TimeEngOn);
    break;

  case LM_TIME_FROM_SESSION_START:

    qw_sub(d_TimeDiff,d_CurrentTime,z_lmControl.z_early_exit_decision.d_TimeSessionStart);
    break;

  default:
    break;
  }

  /* Convert time diff to milliseconds */
  gps_total_msec(d_TimeDiff,d_TimeDiffMs);
  return d_TimeDiffMs[0];
}

/*
 ******************************************************************************
 * lm_mgp_reinstate_dynamic_hepe
 *
 * Function description:
 * restart the dynamic HEPE limit algorithm if the following conditions are both true:
 * (1) If ((MGP is ON) and (dynamic HEPE limit == MAX(250m, QoS Accuracy Threshold)))
 * (2)     IF ( [(NF reported 2 consecutive null fixes) and (HEPE of last valid fix from NF < 50m)]  OR 
 *              [NF reported 10 consecutive null fixes] )
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_mgp_reinstate_dynamic_hepe(void)
{
  if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
  {
    char b_Buffer[100];
    (void) snprintf( b_Buffer, sizeof(b_Buffer), "Check reinstate dynamic hepe: Null Fix report count=%lu LastValidFixHepe=%f\r\n",
                     z_lmControl.z_early_exit_decision.q_NullFixReportCount,z_lmControl.z_early_exit_decision.f_LastValidFixHepe);
    MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buffer);
  }
  
  /* If NF reported 2 null fixes and hepe of last valid fix is <= 50m, reinstate dynamic Qos */
  if(((z_lmControl.z_early_exit_decision.q_NullFixReportCount >= LM_MIN_NULL_FIXES_WITH_HEPE_LIMIT) &&
      (z_lmControl.z_early_exit_decision.f_LastValidFixHepe <= LM_LAST_VALID_FIX_HEPE_LIMIT)) ||
     (z_lmControl.z_early_exit_decision.q_NullFixReportCount >= LM_MIN_NULL_FIXES_WITHOUT_HEPE_LIMIT))
  {
    LM_MSG_MED("Reinstate dynamic hepe logic",0,0,0);
    z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = TRUE;
  }
}

/*
 ******************************************************************************
 * lm_mgp_position_accuracy_check
 *
 * Function description:
 *
 * This function checks if the position fix reported by MGP qualifies the 
 * QoS accuracy threshold provided by TM or not.
 *
 * Parameters: 
 * p_fixReport - Position fix from MGP.
 *
 * Return value: 
 *
 * TRUE - if accuracy threshold check passed
 * FALSE - if accuracy threshold check failed
 *
 ******************************************************************************
*/
boolean lm_mgp_position_accuracy_check(const sm_GnssFixRptStructType* const p_fixReport)
{
  FLT f_hepe = 0;
  uint32 q_TimeFromEngOn,q_TimeFromSessStart;
  uint32 q_current_hepe_limit = z_lmControl.z_fix_request.q_qos_accuracy_threshold;
  uint32 q_hepe_max = z_lmControl.z_fix_request.q_qos_accuracy_threshold;

  if( (p_fixReport == NULL) || ( z_lmControl.z_fix_request.q_qos_accuracy_threshold == 0 ) )
  {
    return FALSE;
  }

  f_hepe = (FLT) sqrt((p_fixReport->z_NavPos.f_ErrorEllipse[1] * 
                       p_fixReport->z_NavPos.f_ErrorEllipse[1]) + 
                      (p_fixReport->z_NavPos.f_ErrorEllipse[2] * 
                       p_fixReport->z_NavPos.f_ErrorEllipse[2]));

  if(z_lmControl.z_fix_request.u_dynamic_qos_enabled)
  {
    /* Calculate time from Session Start */
    q_TimeFromSessStart = lm_get_time_from_start(LM_TIME_FROM_SESSION_START);
    LM_MSG_MED("Time from Sess Start = %lu ms",q_TimeFromSessStart,0,0);

    /* IF QoS_override_time is > 0, this should be interpreted as the number of seconds during which the QoS_meters_override 
    ** should be used instead of the injected value from the PD-API. After QoS_override_time seconds, the injected 
    ** QoS-meters value should apply.
    ** If the QoS_override_time > injected-QoS-timeout, the injected QoS-Timeout will still apply to end the session.
    */
    if(z_lmControl.z_early_exit_decision.u_UseQoSOverride)
    {
      if(q_TimeFromSessStart < z_lmControl.z_early_exit_decision.q_QoSTimeoutOverride)
      {
        q_current_hepe_limit = z_lmControl.z_early_exit_decision.w_QoSThresholdOverride;
      }
      else
      {
        q_current_hepe_limit = z_lmControl.q_HepeThreshold; 
      }
    }

    if((z_lmControl.z_early_exit_decision.u_UseQoSOverride) || 
       (z_lmControl.z_fix_request.q_qos_accuracy_threshold < z_lmControl.q_HepeThreshold))
    {
      q_hepe_max = z_lmControl.q_HepeThreshold;
    }

    LM_MSG_MED("Current Hepe Limit=%lu Hepe Max=%lu",q_current_hepe_limit, q_hepe_max,0);

    /* Calculate time from which MGP engine is ON */
    q_TimeFromEngOn = lm_get_time_from_start(LM_TIME_FROM_ENGINE_ON);
    LM_MSG_MED("Time from Engine ON = %lu ms",q_TimeFromEngOn,0,0);

    /* If this is first fix for which f_hepe < Hmax, store Time of first reportable fix */
    if((z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn == FALSE) && (f_hepe <= q_hepe_max))
    {
      z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn = TRUE;
      z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = q_TimeFromEngOn;
      LM_MSG_MED("Time of first valid fix after Engine ON %lu ms",z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn,0,0);
    }

    /* If this is first fix for which f_hepe < Hmax, store Time of first reportable fix */
    if((z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart == FALSE) && (f_hepe <= q_hepe_max))
    {
      z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = TRUE;
      z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = q_TimeFromSessStart;
      LM_MSG_MED("Time of first valid fix after Sess Start %lu ms",z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart,0,0);
    }

    /* Check if we have hit tunnel-exit scenario */
    if((z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn == TRUE) &&
       ((z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit == q_hepe_max ) ||
        (z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit == z_lmControl.z_fix_request.q_qos_accuracy_threshold)))
    {
      LM_MSG_MED("Hit tunnel exit scenario", 0, 0, 0);
      lm_mgp_reinstate_dynamic_hepe();
    }

    /* Store the hepe as last valid fix hepe */
    z_lmControl.z_early_exit_decision.f_LastValidFixHepe = f_hepe;

    /* In tunnel-exit scenario, reinstate only after first fix w/ hepe < 250m */
    if((z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe == TRUE)  &&
       (z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart == TRUE) &&
       (z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn == FALSE))
    {
      /* Force early ramp by overriding Ter */
      z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = q_TimeFromEngOn;
      z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = TRUE;
    }

    /* If first fix since EngOn is not reported OR reinstate after tunnel-exit is enabled 
    ** AND the fix has hepe < 250m; then use ramp logic */
    if(((z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn == FALSE) || 
        (z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe == TRUE))  &&
       (z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart == TRUE))
    {
      /* Stricter for 4 seconds */
      if((q_TimeFromEngOn - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn) < LM_EARLY_RAMP_TIME_MS)
      {
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT) ((q_TimeFromEngOn - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn + LM_EARLY_RAMP_ADDER_MS) * q_current_hepe_limit);
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit /= LM_EARLY_RAMP_DIVIDER_MS;
        LM_MSG_MED("early ramp",0,0,0);
      }
      /* wait up to 7 sec in session to try to reach target HEPE */
      else if((q_TimeFromSessStart - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart) < LM_FLAT_SECTION_TIME_MS)
      {
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT) q_current_hepe_limit;
        LM_MSG_MED("flat section",0,0,0);
      }
      /* late-ramp for better TTF in difficult environments  */
      else if((q_TimeFromSessStart <= z_lmControl.z_fix_request.q_timeout) &&
              ((z_lmControl.z_fix_request.q_timeout - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart - LM_FLAT_SECTION_TIME_MS) > 0))
      {
        /* Hd = MIN( Hmax, Hc + 2*(Hmax-Hc)*(t-Tr-7)/(Tmax-Tr-7) )*/
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT) (2 * (q_hepe_max - q_current_hepe_limit) * (q_TimeFromSessStart - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart - LM_FLAT_SECTION_TIME_MS));
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit /= (z_lmControl.z_fix_request.q_timeout - z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart - LM_FLAT_SECTION_TIME_MS);
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit += q_current_hepe_limit;
        if(z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit > q_hepe_max)
        {
          z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT) q_hepe_max;
        }
        LM_MSG_MED("late ramp",0,0,0);
      }
      else
      {
        LM_MSG_ERROR("Dynamic QoS logic hit session timeout before timer expiry. Timeout %d, CurrSessTime %d, SessTimeOfFirstValidFIx %lu", 
                     z_lmControl.z_fix_request.q_timeout, q_TimeFromSessStart, z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart);
        z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT) q_hepe_max;
      }
    }

    if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
    {
      char b_Buffer[250];
      (void) snprintf( b_Buffer, sizeof(b_Buffer), "Override=%d Hepe=%f Dynamic Hepe Limit=%f\r\n",
                       z_lmControl.z_early_exit_decision.u_UseQoSOverride,f_hepe, z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit);
      MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buffer);
    }

    if(f_hepe <= z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit)
    {
      if(z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn == FALSE)
      {
        z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn = TRUE;
      }
      z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
      z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate = FALSE;
      z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = FALSE;
      z_lmControl.z_early_exit_decision.e_decision = LM_EARLY_EXIT_DECISION_REPORT_FIX;
      lm_log_early_exit_decision(q_TimeFromEngOn, q_TimeFromSessStart,p_fixReport->z_NavPos.q_RefFCount, (uint16) q_current_hepe_limit, (uint16)z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit, (uint16) q_hepe_max, (uint16) f_hepe, TRUE);
      /*lint -e506 */
      return TRUE;
    }
    else
    {
      z_lmControl.z_early_exit_decision.e_decision = LM_EARLY_EXIT_DECISION_WAIT_FIX;     
      lm_log_early_exit_decision(q_TimeFromEngOn, q_TimeFromSessStart,p_fixReport->z_NavPos.q_RefFCount, (uint16) q_current_hepe_limit, (uint16)z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit, (uint16) q_hepe_max, (uint16) f_hepe, TRUE);
      return FALSE;
    }
  }
  else /*if !u_dynamic_qos_enabled*/
  {
    if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
    {
      char b_Buffer[250];
      (void) snprintf( b_Buffer, sizeof(b_Buffer), "Hepe=%f Current Hepe Limit=%ld\r\n",
                       f_hepe, q_current_hepe_limit);
      MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", b_Buffer);  
    }

    if(f_hepe <= q_current_hepe_limit)
    {
      return TRUE; 
    }
    else
    {
      return FALSE;
    }
  }
}

/*
 ******************************************************************************
 * lm_mgp_update_mgp_state 
 *
 * Function description:
 *
 * This function gets corresponding mgp_ConfigOperationEnumType for a given
 * lm_gps_mode_e_type
 *
 * Parameters: 
 * e_desiredMgpState - desired MGP state
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
boolean lm_mgp_update_mgp_state(lm_mgp_state_e_type e_desiredMgpState,
                                const mgp_ConfigStructType* p_OnParams,
                                boolean u_Forced)
{
  mgp_PeriodicFixAndAlignEnumType e_alignType = C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ;

  MSG_HIGH("Requested State = %d Current Desired State = %d Current State = %d",
    e_desiredMgpState,z_lmControl.e_desiredMgpState, z_lmControl.e_currentMgpState );
  
  switch(e_desiredMgpState)
  {
  case LM_MGP_STATE_ON:
    {

      if( (p_OnParams->e_OperationMode != MGP_OPERATION_MSASSISTED) &&
          (p_OnParams->e_OperationMode != MGP_OPERATION_1X_MSASSISTED)
        )
      {
        uint32 q_Tbf_ms = z_lmControl.z_session_request.q_tbf;
        /* Determine the type of Fix Alignment needed based on TBF and Num Fixes requested */
        if((q_Tbf_ms < lm_get_rc_on_threshold_period()) ||
           (z_lmControl.z_session_request.q_num_fixes == 1))
        {
          e_alignType = C_MGP_FIX_TIMER_ON_ALIGN_EXP_FIRST;
        }
        else if(0 != q_Tbf_ms)
        {
          e_alignType = C_MGP_FIX_TIMER_ON_ALIGN_ALWAYS;
        }

        if( !mgp_ConfigPeriodicFixAndAlignment(e_alignType, q_Tbf_ms) )
        {
          LM_MSG_ERROR("Failed to send set the requested for periodic fix; tbf=%u",
                       q_Tbf_ms,0,0);
        }
      }

      if(((z_lmControl.e_desiredMgpState == LM_MGP_STATE_OFF)||
          (z_lmControl.e_desiredMgpState == LM_MGP_STATE_IDLE)) &&
         ((z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_OFF) || 
          (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_IDLE)) )
      {
        /*This check is done in-order to handle the extended ON mode in MGP.
        MGP may deferr the OFF/IDLE command in extended ON mode. We may get
        another ON request and want to take MGP out of the extended ON mode.*/

        if(z_lmControl.e_desiredMgpState == LM_MGP_STATE_IDLE)
        {
          (void)os_TimerStop(z_lmControl.p_goingIDLETimer);  
          LM_MSG_MED("Stopped GOING_IDLE Timer",0,0,0); 
        }
        else
        {
          (void)os_TimerStop(z_lmControl.p_goingOFFTimer);  
          LM_MSG_MED("Stopped GOING_OFF Timer",0,0,0);  
        }

        z_lmControl.e_desiredMgpState = LM_MGP_STATE_ON;

        /*Start a timer for getting the ON notification from MGP*/
        if(!os_TimerStart(z_lmControl.p_goingONTimer,LM_GOING_ON_TIMEOUT,0))
        {
          LM_MSG_ERROR("Failed starting GOING_ON timer",0,0,0);
          return FALSE;   
        }
        LM_MSG_MED("Turning MGP to ON(MGP could be in extended ON mode)",0,0,0);
        mgp_ReceiverOn(p_OnParams);

        /*Update MGP state*/
        z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_ON;
        return TRUE;
      }

      z_lmControl.e_desiredMgpState = LM_MGP_STATE_ON;

      if((z_lmControl.e_currentMgpState == LM_MGP_STATE_OFF) ||
         (z_lmControl.e_currentMgpState == LM_MGP_STATE_IDLE) ||
         (z_lmControl.z_session_request.e_mode == LM_GPS_MODE_SA_RF))
      {
        /*Start a timer for getting the ON notification from MGP*/
        if(!os_TimerStart(z_lmControl.p_goingONTimer,LM_GOING_ON_TIMEOUT,0))
        {
          LM_MSG_ERROR("Failed starting GOING_ON timer",0,0,0);
          return FALSE;   
        }
        LM_MSG_MED("Turning MGP to ON",0,0,0);
        mgp_ReceiverOn(p_OnParams);

        /*Update MGP state*/
        z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_ON;
      }
      /*lint -e506 */
      return TRUE;
    }

  case LM_MGP_STATE_OFF:
    {
      z_lmControl.e_desiredMgpState = LM_MGP_STATE_OFF;

      /*Unsubscribe 1Hz periodic fix reporting from PE*/
      if(!mgp_ConfigPeriodicFixAndAlignment(C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ, 0))
      {
        LM_MSG_ERROR("Failed to send the disable periodic fixes request to the PE.",0,0,0);
      }

      if((z_lmControl.e_currentMgpState == LM_MGP_STATE_ON) ||
         (z_lmControl.e_currentMgpState == LM_MGP_STATE_IDLE))
      {
        /*Start a timer for getting the OFF notification from MGP*/
        if(!os_TimerStart(z_lmControl.p_goingOFFTimer,LM_GOING_OFF_TIMEOUT,0))
        {
          LM_MSG_ERROR("Failed starting GOING_OFF timer",0,0,0);
          return FALSE;   
        }
        LM_MSG_MED("Turning MGP to OFF",0,0,0);
        if(u_Forced)
        {
          mgp_ReceiverForcedOff();
        }
        else
        {
          mgp_ReceiverOff();
        }
        /*Update MGP state*/
        z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_OFF;
      }

      if(((z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_OFF) ||
          (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_IDLE) ||
          (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_ON)) &&
         (u_Forced == TRUE))
      {
        /*Start a timer for getting the OFF notification from MGP*/
        if(!os_TimerStart(z_lmControl.p_goingOFFTimer,LM_GOING_OFF_TIMEOUT,0))
        {
          LM_MSG_ERROR("Failed starting GOING_OFF timer",0,0,0);
          return FALSE;   
        }
        LM_MSG_MED("Turning MGP to OFF",0,0,0);
        mgp_ReceiverForcedOff();

        z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_OFF;
      }
      /*lint -e506 */
      return TRUE;
    }

  case LM_MGP_STATE_IDLE:
    {
      /* Unsubscribe periodic fix reporting from PE */
      if(!mgp_ConfigPeriodicFixAndAlignment(C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ, 0))
      {
        LM_MSG_ERROR("Failed to send the disable periodic fixes request to the PE.",0,0,0);
      }

      if(((z_lmControl.e_desiredMgpState == LM_MGP_STATE_OFF)||
          (z_lmControl.e_desiredMgpState == LM_MGP_STATE_ON) ||
          (z_lmControl.e_desiredMgpState == LM_MGP_STATE_IDLE)) &&
         ((z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_OFF) ||
          (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_ON) ||
          (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_IDLE)) &&
         u_Forced)
      {
        /*This check is done in-order to handle the extended ON mode in MGP.
        MGP may deferr the OFF/IDLE command in extended ON mode. We may get
        another forced IDLE request and want to take MGP out of the extended ON mode.*/

        if(z_lmControl.e_desiredMgpState == LM_MGP_STATE_IDLE)
        {
          (void)os_TimerStop(z_lmControl.p_goingIDLETimer);  
          LM_MSG_MED("Stopped GOING_IDLE Timer",0,0,0); 
        }
        else
        {
          (void)os_TimerStop(z_lmControl.p_goingOFFTimer);  
          LM_MSG_MED("Stopped GOING_OFF Timer",0,0,0);  
        }

        z_lmControl.e_desiredMgpState = LM_MGP_STATE_IDLE;

        /*Start a timer for getting the IDLE notification from MGP*/
        if(!os_TimerStart(z_lmControl.p_goingIDLETimer,LM_GOING_IDLE_TIMEOUT,0))
        {
          LM_MSG_ERROR("Failed starting GOING_IDLE timer",0,0,0);
          return FALSE;   
        }

        if(u_Forced)
        {
          LM_MSG_MED("Turning MGP to IDLE(Forced)",0,0,0);
          mgp_ReceiverForcedIdle();
        }
        else
        {
          LM_MSG_MED("Turning MGP to IDLE",0,0,0);
          mgp_ReceiverIdle();
        }

        /*Update MGP state*/
        z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_IDLE;

        return TRUE;
      }

      z_lmControl.e_desiredMgpState = LM_MGP_STATE_IDLE;

      LM_MSG_MED("LM Desired State = %d, Current State = %d", z_lmControl.e_desiredMgpState,z_lmControl.e_currentMgpState,0  );

      if((z_lmControl.e_currentMgpState == LM_MGP_STATE_ON) ||
         (z_lmControl.e_currentMgpState == LM_MGP_STATE_OFF) ||
         (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_ON))
      {
        /*Start a timer for getting the IDLE notification from MGP*/
        if(!os_TimerStart(z_lmControl.p_goingIDLETimer,LM_GOING_IDLE_TIMEOUT,0))
        {
          LM_MSG_ERROR("Failed starting GOING_IDLE timer",0,0,0);
          return FALSE;   
        }

        if(u_Forced)
        {
          LM_MSG_MED("Turning MGP to IDLE(Forced)",0,0,0);
          mgp_ReceiverForcedIdle();
        }
        else
        {
          LM_MSG_MED("Turning MGP to IDLE",0,0,0);
          mgp_ReceiverIdle();
        }

        /*Update MGP state*/
        z_lmControl.e_currentMgpState = LM_MGP_STATE_GOING_IDLE;
      }
      /*lint -e506 */
      return TRUE;
    }

  default:
    {
      return FALSE;
    }
  }/*switch(e_desiredMgpState) ends*/

}

/*
 ******************************************************************************
 * lm_mgp_start_position_fix_processing 
 *
 * Function description:
 *
 * This function turns MGP engine to ON for a MS-based session.
 *
 * Parameters: 
 * p_FixReq - Pointer to structure containing Fix request from TM
 *
 * Return value: 
 *
 * boolean - MGP fix processng successfully started or not
 *
 ******************************************************************************
*/
boolean lm_mgp_start_position_fix_processing(const lm_session_request_fix_s_type* const p_FixReq)
{
  uint32  q_hepe_max            = z_lmControl.z_fix_request.q_qos_accuracy_threshold;
  uint32  q_prevShutOffTimeLeft = 0;
  /* Get the current MGP operation mode before start processing the fix*/
  mgp_ConfigOperationEnumType const e_MgpCurrentOperationMode = z_lmControl.z_MgpONParams.e_OperationMode;

  /*When calling lm_mgp_update_mgp_state() to turn ON MGP, MGP might be in a transition
    state and ON command has to be delayed until MGP comes out of the transition
    state. Save the the MGP on parameters in z_lmControl so that they are readily
    available when needed to turn ON MGP*/
  mgp_ConfigStructType* pz_MgpConfig = &z_lmControl.z_MgpONParams;

  if(p_FixReq == NULL)
  {
    return FALSE;
  }

  if(!os_TimerStart(z_lmControl.p_fixTimer,p_FixReq->q_timeout,0))
  {
    LM_MSG_ERROR("Failed starting Fix timer",0,0,0);
    return FALSE;
  }

  (void) time_get(z_lmControl.z_early_exit_decision.d_TimeSessionStart);
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = FALSE;
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = 0;
  /* If first reportable fix after engine ON has been recorded */

  if((z_lmControl.z_early_exit_decision.u_UseQoSOverride) || 
     (z_lmControl.z_fix_request.q_qos_accuracy_threshold < z_lmControl.q_HepeThreshold))
  {
    q_hepe_max = z_lmControl.q_HepeThreshold;
  }
  if(z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn)
  {
    /* Set Dynamic QoS value (Hd) to Hmax for the duration of the session */
    z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT) q_hepe_max;
  }

  /* 
   * If dynamic QoS is enabled AND
   * If its been more than 12 seconds since we started the shutOffTimer, reset the dynamic 
   * hepe limit
   */
  if(z_lmControl.z_fix_request.u_dynamic_qos_enabled)
  {
    q_prevShutOffTimeLeft = os_TimerCheck(z_lmControl.p_mgpShutOffTimer);

    if(((FLT) (z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue - (int32) q_prevShutOffTimeLeft)) > 
       C_MAX_TIME_LIMIT_FOR_DPO_OPN * 1000)
    {
      LM_MSG_MED("Reinstate dynamic Hepe logic %d %d %d", 
                 z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue, 
                 q_prevShutOffTimeLeft, 
                 (z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue - q_prevShutOffTimeLeft));
      z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe  = TRUE;
      z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue = 0;
    }
  }

  /*Stop the RC_ON timer*/
  LM_MSG_MED("Stopping RC_ON timer",0,0,0);
  os_TimerStop(z_lmControl.p_rcOnTimer);/*lint !e534 */

  pz_MgpConfig->e_OperationMode     = lm_mgp_get_mgp_operation_mode(z_lmControl.z_session_request.e_mode);

  LM_ASSERT(pz_MgpConfig->e_OperationMode != (uint8)MGP_OPERATION_MAX);

  if(pz_MgpConfig->e_OperationMode == (uint8)MGP_OPERATION_MAX)
  {
    LM_MSG_ERROR("Invalid GPS operation mode received",0,0,0);
    return FALSE;
  }

  pz_MgpConfig->q_SADataAvailability  = p_FixReq->q_data_availability;
  pz_MgpConfig->q_RoundTripDelayMs  = p_FixReq->q_rtd;
  pz_MgpConfig->u_Qos               = 0;  /* Not currently used by MGP. Set zero */
  pz_MgpConfig->q_SessionTimeoutMs  = p_FixReq->q_timeout;
  pz_MgpConfig->u_SmMeasRptPeriod   = 1; /*Unknown?? Measurement report period in units of GPS_MEAS_TICK */
  pz_MgpConfig->u_EnablePpmRpt      = FALSE; /*lint -e506 */
  pz_MgpConfig->u_EnablePosRpt      = TRUE;
  pz_MgpConfig->u_EnableUtcRpt      = TRUE;
  pz_MgpConfig->e_NetPlane          = (mgp_ConfigNetPlaneEnumType)z_lmControl.z_session_request.e_up_cp;
  pz_MgpConfig->u_DataDemodFlag     = z_lmControl.z_session_request.u_do_data_demod;
  pz_MgpConfig->q_QosAccuracyThreshold = p_FixReq->q_qos_accuracy_threshold;
  pz_MgpConfig->q_tbf  = z_lmControl.z_session_request.q_tbf;

  z_lmControl.u_MgpONParamsValid = TRUE;
  pz_MgpConfig->u_OdpSession        = p_FixReq->u_on_demand;
  pz_MgpConfig->q_Punc              = p_FixReq->q_Punc;
  pz_MgpConfig->q_Tunc              = p_FixReq->q_Tunc;

  MSG_4( MSG_SSID_GPSSM, MSG_LEGACY_MED,
         "=LM TASK= START FIX processing with timeout[%d] old timeout[%d] SA_availability[%d] MGP opermode[%d]",
         p_FixReq->q_timeout,
         z_lmControl.z_fix_request.q_timeout,
         p_FixReq->q_data_availability,
         e_MgpCurrentOperationMode );
  
  if(z_lmControl.z_early_exit_decision.u_UseQoSOverride)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED,
         "=LM TASK= Qos_accuracy[%ld]", z_lmControl.z_early_exit_decision.w_QoSThresholdOverride); 
  }
  else
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED,
         "=LM TASK= Qos_accuracy[%lu]", p_FixReq->q_qos_accuracy_threshold); 
  }

  LM_MSG_MED("Dynamic QoS Enabled: %d",p_FixReq->u_dynamic_qos_enabled,0,0);

  if(!lm_mgp_update_mgp_state(LM_MGP_STATE_ON,pz_MgpConfig,FALSE))
  {
    os_TimerStop(z_lmControl.p_fixTimer);/*lint !e534 */
    return FALSE;
  }

  /* If ME in MSB receiver On mode & any change to timeout value then update ME.
  *  Caution : z_lmControl.z_fix_request shouldn't be cleaned between sessions.
  */
  if ( (LM_MGP_STATE_ON == z_lmControl.e_currentMgpState) &&
       (MGP_OPERATION_MSBASED == e_MgpCurrentOperationMode) &&
       (p_FixReq->q_timeout != z_lmControl.z_fix_request.q_timeout)
      )
  {
    mgp_UpDateAgpsAgnssSessionTimeOutMsecs(p_FixReq->q_timeout);
  }

  /*Send Fix Start Indication to MGP*/
  lm_mgp_send_fix_start_ind(z_lmControl.z_session_request.q_tbf);

  /*Update fix session related state*/
  lm_update_session_state(LM_SESSION_STATE_IN_SESSION_REPORT_PENDING);
  z_lmControl.q_reportsPending |= (uint32)LM_SESSION_REQUEST_FIX;

  /*Report event*/
  event_report(EVENT_GPS_LM_FIX_REQUEST_START);

  /* Copy fix request params internally */
  z_lmControl.z_fix_request = *p_FixReq;

  return TRUE;
}

/*
 ******************************************************************************
 * lm_mgp_stop_position_fix_processing 
 *
 * Function description:
 *
 * This function stops a position fix processing. It will stop the position fix
 * timer and turn off MGP. 
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
void lm_mgp_stop_position_fix_processing(boolean u_forceMgpOff)
{
  LM_MSG_MED("Stopping FIX timer",0,0,0);
  os_TimerStop(z_lmControl.p_fixTimer);/*lint !e534 */
  /*Stop all the MGP engine maintenance related timers to avoid un-neccessary
    timeout events*/
  LM_MSG_MED("Stopping RC_ON and SHUT_OFF timers",0,0,0);
  os_TimerStop(z_lmControl.p_mgpShutOffTimer); /*lint !e534 */
  os_TimerStop(z_lmControl.p_rcOnTimer); /*lint !e534 */

  if(!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, u_forceMgpOff))
  {
    LM_MSG_HIGH("State update to LM_MGP_STATE_OFF failed ",0,0,0);
  }

  z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_FIX);
  /*Report event only if a session is active */

  if(z_lmControl.e_sessionState != LM_SESSION_STATE_NO_SESSION)
  {
    event_report(EVENT_GPS_LM_FIX_REQUEST_END);
  }

  qw_set(z_lmControl.z_early_exit_decision.d_TimeSessionStart,0,0);
}

/*
 ******************************************************************************
 * lm_mgp_stop_continue_fix_processing 
 *
 * Function description:
 *
 * This function updatea position fix processing params. Currently it only 
 * updates the PRM timeout value. The currently running PRM timeout counter 
 * is stopped and a new one is started.
 *
 * Parameters: 
 * p_FixReq - Request containing params to be modified for position fix 
 *
 * Return value: 
 *
 * boolean - position fix processng params successfully updated or not
 *
 ******************************************************************************
*/
boolean lm_mgp_continue_position_fix_processing(const lm_session_request_fix_s_type* const p_FixReq)
{

  uint32 q_hepe_max = 0;

  if(p_FixReq == NULL)
  {
    return FALSE;
  }

  LM_MSG_MED("Continue FIX request rcvd with timeout[%d] qos_accuracy[%d] dynamic_QoS[%d]",p_FixReq->q_timeout,
             p_FixReq->q_qos_accuracy_threshold,p_FixReq->u_dynamic_qos_enabled);

  z_lmControl.z_fix_request.q_qos_accuracy_threshold = p_FixReq->q_qos_accuracy_threshold;
  z_lmControl.z_fix_request.u_dynamic_qos_enabled    = p_FixReq->u_dynamic_qos_enabled; 
  q_hepe_max = z_lmControl.z_fix_request.q_qos_accuracy_threshold;

  if(p_FixReq->u_dynamic_qos_enabled)
  {
    /*If dynamic QoS is requested to be enabled, consider this like a new session*/
    (void) time_get(z_lmControl.z_early_exit_decision.d_TimeSessionStart);
    z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = FALSE;
    z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = 0;

    if(z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn)
    {
      /* Set Dynamic QoS value (Hd) to Hmax for the duration of the session */
      z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = (FLT) q_hepe_max;
    }
  }

  if(p_FixReq->q_timeout != 0)
  {
    /*Stop the previous timer*/
    os_TimerStop(z_lmControl.p_fixTimer); /*lint !e534 */
    /* Update the LM fix req timeout */
    z_lmControl.z_fix_request.q_timeout = p_FixReq->q_timeout;
    LM_MSG_MED("Modifying FIX timer to value %d",p_FixReq->q_timeout,0,0);

    /*Start the timer again with new value*/
    if(!os_TimerStart(z_lmControl.p_fixTimer,p_FixReq->q_timeout,0))
    {
      LM_MSG_ERROR("Unable to modify position fix timer",0,0,0);
      return FALSE;
    }
  }
  /* Inform MGP of updated session duration */
  mgp_UpDateAgpsAgnssSessionTimeOutMsecs(p_FixReq->q_timeout);

  /*Currently only timeout value is looked at and modified when a continue request
    is received. Other values may be looked at and modified in future*/  /*lint -e506 */
  /*Report event*/
  event_report(EVENT_GPS_LM_FIX_REQUEST_CONTINUE);
  return TRUE;
}

/*
 ******************************************************************************
 * lm_mgp_start_prm_processing 
 *
 * Function description:
 *
 * This function turns MGP engine to IDLE for a MS-Assisted session. Once AA Data
 * is made available to LM, it will call lm_mgp_aa_data_received() to turn ON MGP
 * for taking PRM.
 *
 * Parameters: 
 * p_GpsReq - Pointer to structure containing GPS(PRM) request from TM
 *
 * Return value: 
 *
 * boolean - MGP PRM processng successfully started or not
 *
 ******************************************************************************
*/
boolean lm_mgp_start_prm_processing(const lm_session_request_gps_s_type* const p_GpsReq)
{
  if ( p_GpsReq == NULL )
  {
    return ( FALSE );
  }

  LM_MSG_MED("Start PRM processing with timeout[%d] SA_data_availability[%d], MultiRptMode[%d]",
             p_GpsReq->q_timeout,
             p_GpsReq->q_data_availability,
             p_GpsReq->multi_report_msa);

  if ( !os_TimerStart(z_lmControl.p_gpsTimer, p_GpsReq->q_timeout, 0) )
  {
    LM_MSG_ERROR("Failed starting Fix timer", 0, 0, 0);
    return ( FALSE );
  }

  /*Stop the RC_ON timer*/
  LM_MSG_MED("Stopping RC_ON timer", 0, 0, 0);
  os_TimerStop(z_lmControl.p_rcOnTimer); /*lint !e534 */

  if ( !lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE, NULL, TRUE) )
  {
    os_TimerStop(z_lmControl.p_gpsTimer); /*lint !e534 */
    return ( FALSE );
  }

  lm_update_session_state(LM_SESSION_STATE_IN_SESSION_REPORT_PENDING); /*lint -e506 */
  lm_set_waitingAAData((boolean)TRUE);
  z_lmControl.q_reportsPending |= (uint32)LM_SESSION_REQUEST_GPS;
  z_lmControl.z_gps_request.multi_report_msa = p_GpsReq->multi_report_msa;
  /*Report event*/
  event_report(EVENT_GPS_LM_PRM_REQUEST_START);

  /*lint -e506 */
  return ( TRUE );
}

/*
 ******************************************************************************
 * lm_mgp_aa_data_received 
 *
 * Function description:
 *
 * This function is called after LM receives AA data from TM which it was waiting
 * on for processing a PRM request for MS-Assisted mode. This function will turn 
 * ON MGP.
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * boolean - MGP PRM processng successfully started or not
 *
 ******************************************************************************
*/
boolean lm_mgp_aa_data_received( void )
{
  /*When calling lm_mgp_update_mgp_state() to turn ON MGP, MGP might be in a transition
  state and ON command has to be delayed until MGP comes out of the transition
  state. Save the the MGP on parameters in z_lmControl so that they are readily
  available when needed to turn ON MGP*/

  mgp_ConfigStructType* pz_MgpConfig = &z_lmControl.z_MgpONParams;
  lm_session_request_gps_s_type const * p_GpsReq = NULL;
  lm_session_request_session_s_type* p_SessReq = NULL;

  /*We are no longer waiting for AA data to turn MGP ON*/
  lm_set_waitingAAData(FALSE);

  p_GpsReq =  &z_lmControl.z_gps_request;
  p_SessReq = &z_lmControl.z_session_request;

  pz_MgpConfig->e_OperationMode     = lm_mgp_get_mgp_operation_mode(p_SessReq->e_mode);
  LM_ASSERT(pz_MgpConfig->e_OperationMode != MGP_OPERATION_MAX);

  if(pz_MgpConfig->e_OperationMode == MGP_OPERATION_MAX)
  {
    return FALSE;
  }

  if(pz_MgpConfig->e_OperationMode == MGP_OPERATION_FTM)
  {
    LM_MSG_MED("FTM Mode. Setting search Mode to %d. Disable Jammer detection= %d",
               z_lmControl.u_searchMode,z_lmControl.u_DisableJammerDetect,0);  
    mgp_FTMSetSearchMode(z_lmControl.u_searchMode);
    mgp_FTMSetJammerDetect(z_lmControl.u_DisableJammerDetect);
  }

  pz_MgpConfig->q_SADataAvailability  = p_GpsReq->q_data_availability;
  pz_MgpConfig->q_RoundTripDelayMs  = p_GpsReq->q_rtd;
  pz_MgpConfig->u_Qos               = p_GpsReq->u_qos;  /* Unknown?? Performance QOS */
  pz_MgpConfig->q_SessionTimeoutMs  = p_GpsReq->q_timeout;
  pz_MgpConfig->u_SmMeasRptPeriod   = 1; /*Unknown??*//* Measurement report period in units of GPS_MEAS_TICK */
  pz_MgpConfig->u_EnablePpmRpt      = FALSE;
  pz_MgpConfig->u_EnablePosRpt      = FALSE; /*lint -e506 */
  pz_MgpConfig->u_EnableUtcRpt      = TRUE;
  pz_MgpConfig->e_NetPlane          = (mgp_ConfigNetPlaneEnumType)z_lmControl.z_session_request.e_up_cp;
  pz_MgpConfig->u_DataDemodFlag     = z_lmControl.z_session_request.u_do_data_demod;
  pz_MgpConfig->u_multi_report_msa  = z_lmControl.z_gps_request.multi_report_msa;
  pz_MgpConfig->q_QosAccuracyThreshold = 0;
  pz_MgpConfig->q_tbf = z_lmControl.z_session_request.q_tbf;
  /*lint -e506 */
  z_lmControl.u_MgpONParamsValid  = TRUE;

  if(!lm_mgp_update_mgp_state(LM_MGP_STATE_ON,pz_MgpConfig,FALSE))
  {
    return FALSE;
  }


  /*Update fix session related state*/
  lm_update_session_state(LM_SESSION_STATE_IN_SESSION_REPORT_PENDING);
  /*lint -e506 */
  return TRUE;
}

/*
 ******************************************************************************
 * lm_mgp_stop_prm_processing 
 *
 * Function description:
 *
 * This function stops PRM processing. It will stop the GPS report timer and 
 * turn MGP OFF.
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
void lm_mgp_stop_prm_processing()
{
  LM_MSG_MED("Stopping PRM timer",0,0,0);
  os_TimerStop(z_lmControl.p_gpsTimer);/*lint !e534 */
  /*Stop all the MGP engine maintenance related timers to avoid un-neccessary
    timeout events*/
  LM_MSG_MED("Stopping RC_ON and SHUT_OFF timers",0,0,0);
  os_TimerStop(z_lmControl.p_mgpShutOffTimer);/*lint !e534 */
  os_TimerStop(z_lmControl.p_rcOnTimer);/*lint !e534 */

  if(!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF,NULL,FALSE))
  {
    LM_MSG_HIGH("State update to LM_MGP_STATE_OFF failed",0,0,0);
  }

  z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_GPS);
  /*Report event*/
  event_report(EVENT_GPS_LM_PRM_REQUEST_END);
}

/*
 ******************************************************************************
 * lm_mgp_continue_prm_processing 
 *
 * Function description:
 *
 * This function updated PRM processing params. Currently it only updates the PRM
 * timeout value. The currently running PRM timeout counter is stopped and a new
 * one is started.
 *
 * Parameters: 
 * p_GpsReq - Request containing params to be modified for PRM 
 *
 * Return value: 
 *
 * boolean - Parameters successfully updated or not
 *
 ******************************************************************************
*/
boolean lm_mgp_continue_prm_processing(const lm_session_request_gps_s_type* const p_GpsReq)
{

  if(p_GpsReq == NULL)
  {
    return FALSE;
  }

  /*Stop the previous timer*/
  os_TimerStop(z_lmControl.p_gpsTimer);/*lint !e534 */

  LM_MSG_MED("Modifying PRM timer to value %d msec, MultiReptMode=%d, AAWait=%u", 
             p_GpsReq->q_timeout, p_GpsReq->multi_report_msa, z_lmControl.u_waitingAAData);

  /* Update LM variable with latest PRM session timeout value */
  z_lmControl.z_gps_request.q_timeout = p_GpsReq->q_timeout;
  /* Update MGP with PRM session timeout after RCVR is ON. Until that time,
   * update LM q_timeout with correct response time, used to turn ON RCVR.
   */
  if ( FALSE == z_lmControl.u_waitingAAData )
  {
    mgp_UpDateAgpsAgnssSessionTimeOutMsecs(p_GpsReq->q_timeout);
  }

  /*Start the timer again with new value*/
  if(!os_TimerStart(z_lmControl.p_gpsTimer,p_GpsReq->q_timeout,0))
  {
    LM_MSG_ERROR("Unable to modify PRM timer",0,0,0);
    return FALSE;
  }

  z_lmControl.z_gps_request.multi_report_msa = p_GpsReq->multi_report_msa;

  /*Currently only timeout value is looked at and modified when a continue request
    is received. Other values may be looked at and modified in future*/ /*lint -e506 */
  /*Report event*/
  event_report(EVENT_GPS_LM_PRM_REQUEST_CONTINUE);
  return TRUE;
}

/*
 ******************************************************************************
 * lm_meas_status_bits_check
 *
 * Function description:
 *
 * Function checking SV measurement status bits to determin if the SV is
 * to be included in the measrement report to the network (this will  
 * affect both NMEA and QMI output)  
 * Also, this function can do optional filtering (featurized) based on SV's  
 * freshness status bit.
 *
 * Parameters: 
 * q_sv_meas_status - SV measurement status bits
 * w_Cno - CNo. Units of 0.1 dB
 * e_SvState - SV State
 *
 * Return value: TRUE/FALSE
 *
 ******************************************************************************
*/
boolean lm_meas_status_bits_check(uint32 q_sv_meas_status, uint16 w_Cno, gnss_SvStateEnumType e_SvState)
{
  /* SV freshness-based filtering (freshness bit is defined in gnss_common.h) */
#ifdef FEATURE_GNSS_SV_FRESHNESS_FILTERING 
  if ( !( q_sv_meas_status & ( MEAS_STATUS_GNSS_FRESH_MEAS ) ) ) 
  {
    return FALSE;
  }
#endif

  if ( q_sv_meas_status & ( MEAS_STATUS_XCORR ) ) 
  {
    return FALSE;
  }
  else /* XCORR & DONT_USE bits not set*/
  {
    if( q_sv_meas_status & ( MEAS_STATUS_FROM_RNG_DIFF | MEAS_STATUS_FROM_VE_DIFF ) )
    {
      if(e_SvState == C_GNSS_SV_STATE_DPO_TRACK)
      {
        return FALSE;
      }
    }
    else /* RNG_DIFF & VE_DIFF bits not set*/
    {
      if(w_Cno == 0)
      {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/*
 ******************************************************************************
 * lm_mgp_clean_meas_rpt
 *
 * Function description:
 *
 * This function cleans the PRM Meas report and removes any invalid SV's
 *
 * Parameters: 
 * p_measReport - PRM report to be sent to TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_clean_meas_rpt(sm_GpsMeasRptStructType* p_measReport)
{
  uint8 temp_u_NumSvs = 0; 
  int itr = 0; 

  if(p_measReport == NULL || ((p_measReport->z_MeasBlk).u_NumSvs == 0))
  {
    MSG_MED("Measurement Report is empty.",0,0,0);
    return;
  }
  MSG_MED("Original u_NumSvs = %d",(p_measReport->z_MeasBlk).u_NumSvs,0,0);
  if( sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_GPS) )
  {
    gnss_MeasStructType temp_z_Gps[N_ACTIVE_GPS_CHAN];
    memset((void*)&temp_z_Gps,0,sizeof(temp_z_Gps));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_GPS_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        uint16 w_Sv;

        temp_z_Gps[temp_u_NumSvs] = (p_measReport->z_MeasBlk).z_Gnss[itr];

        // convert SBAS SVID from internal (120~151) to external format(33~64)
        w_Sv = temp_z_Gps[temp_u_NumSvs].w_Sv;
        if  ( (w_Sv >= C_FIRST_SBAS_SV_PRN) && ( w_Sv <= C_LAST_SBAS_SV_PRN) ) // 120~138 for SBAS
        {
          temp_z_Gps[temp_u_NumSvs].w_Sv = w_Sv - C_FIRST_SBAS_SV_PRN + PDSM_PD_SV_ID_SBAS_MIN;
          MSG_LOW("Converting SBAS SVID from %d to %d", w_Sv, temp_z_Gps[temp_u_NumSvs].w_Sv, 0);
        }

        temp_u_NumSvs++;
      }
      else
      {
        MSG_LOW("Dropping GPS SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Gps, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);

    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }
  else if(sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_GLO))
  {
    gnss_MeasStructType temp_z_Glo[N_ACTIVE_GLO_CHAN];
    memset((void*)&temp_z_Glo,0,sizeof(temp_z_Glo));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_GLO_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        temp_z_Glo[temp_u_NumSvs++] = (p_measReport->z_MeasBlk).z_Gnss[itr];
      }
      else
      {
        MSG_LOW("Dropping GLO SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]),
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Glo, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);
    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }
  else if(sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_BDS))
  {
    gnss_MeasStructType temp_z_Bds[N_ACTIVE_BDS_CHAN];
    memset((void*)&temp_z_Bds,0,sizeof(temp_z_Bds));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_BDS_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        temp_z_Bds[temp_u_NumSvs++] = (p_measReport->z_MeasBlk).z_Gnss[itr];
      }
      else
      {
        MSG_LOW("Dropping BDS SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Bds, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);
    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }
  else if(sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_GAL))
  {
    gnss_MeasStructType temp_z_Gal[N_ACTIVE_GAL_CHAN];
    memset((void*)&temp_z_Gal,0,sizeof(temp_z_Gal));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_GAL_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        temp_z_Gal[temp_u_NumSvs++] = (p_measReport->z_MeasBlk).z_Gnss[itr];
      }
      else
      {
        MSG_LOW("Dropping GAL SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Gal, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);
    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }
  else if(sm_is_meas_blk_active((p_measReport->z_MeasBlk).e_MeasBlkSrc, GNSS_MEAS_BLK_SRC_QZSS_SBAS))
  {
    gnss_MeasStructType temp_z_Qzss[N_ACTIVE_CHAN];
    memset((void*)&temp_z_Qzss,0,sizeof(temp_z_Qzss));

    for(itr = 0; (itr < (p_measReport->z_MeasBlk).u_NumSvs) && (itr < N_ACTIVE_CHAN); itr++ )
    {
      if(lm_meas_status_bits_check((p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno,
                                   (p_measReport->z_MeasBlk).z_Gnss[itr].q_SvState))
      {
        uint16 w_Sv;

        temp_z_Qzss[temp_u_NumSvs] = (p_measReport->z_MeasBlk).z_Gnss[itr];

        // convert SBAS SVID from internal (120~151) to external format(33~64)
        w_Sv = temp_z_Qzss[temp_u_NumSvs].w_Sv;
        if  ( (w_Sv >= C_FIRST_SBAS_SV_PRN) && ( w_Sv <= C_LAST_SBAS_SV_PRN) ) // 120~138 for SBAS
        {
          temp_z_Qzss[temp_u_NumSvs].w_Sv = w_Sv - C_FIRST_SBAS_SV_PRN + PDSM_PD_SV_ID_SBAS_MIN;
          MSG_LOW("Converting SBAS SVID from %d to %d", w_Sv, temp_z_Qzss[temp_u_NumSvs].w_Sv, 0);
        }

        temp_u_NumSvs++;
      }
      else
      {
        MSG_LOW("Dropping QZSS SV %d: q_MeasStatus=%x Cno=%d",
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Sv,
                (p_measReport->z_MeasBlk).z_Gnss[itr].q_MeasStatus,
                (p_measReport->z_MeasBlk).z_Gnss[itr].w_Cno);
      }
    }
    /* Copying the final list of SV's from the temp array back into the report*/
    memscpy((void *)&((p_measReport->z_MeasBlk).z_Gnss[0]), 
           sizeof(((p_measReport->z_MeasBlk).z_Gnss)),
           (void *)temp_z_Qzss, 
           sizeof(gnss_MeasStructType)*temp_u_NumSvs);

    (p_measReport->z_MeasBlk).u_NumSvs = temp_u_NumSvs;
  }

  MSG_LOW("New u_NumSvs = %d",(p_measReport->z_MeasBlk).u_NumSvs,0,0);
}

/*
 ******************************************************************************
 * lm_mgp_handle_gps_meas_done
 *
 * Function description:
 *
 * This function handles the PRM measurement report DONE from MGP
 *
 * Parameters: 
 * p_msg - Pointer to structure containing IPC message with meas report
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_gps_meas_done(os_IpcMsgType* p_msg)
{
  sm_GpsMeasRptStructType *p_measReport = NULL;
  boolean b_lm_state_change = FALSE;

  if ( p_msg == NULL )
  {
    return;
  }

  /*lint -e826 */
  p_measReport = (sm_GpsMeasRptStructType *)p_msg->p_Data;
  /*lint +e826 */

  LM_MSG_MED("Received MEAS_DONE from MGP for %d (GPS/GLO/BDS/GAL/QZSS) system.", p_measReport->z_MeasBlk.e_MeasBlkSrc, 0, 0);

  /* Cleaning the MEAS report */
  lm_mgp_clean_meas_rpt(p_measReport);

  if ( z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING )
  {
    if ( z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS ) /*if a PRM report pending*/
    {
      /* Wait until all measurements are received from the engine. The engine should generate measurements only for the
       * injected AD. For non-injected AD an empty measurement report will be generated.
       * Note that the order of GPS/GLO/BDS measurement isn't warranted.
       */
      if ( p_measReport->z_MeasBlk.u_SeqNum == p_measReport->z_MeasBlk.u_MaxMessageNum )
      {
        /*Stop the PRM timer*/
        os_TimerStop(z_lmControl.p_gpsTimer); /*lint !e534 */
        b_lm_state_change = TRUE;
      }

      if ( !lm_tm_send_prm_report_to_tm(p_measReport, FALSE) )
      {
        LM_MSG_ERROR("Unable to send PRM report to TM", 0, 0, 0);
      }

      if ( TRUE == b_lm_state_change )
      {
        if ( LM_GPS_MODE_FTM == z_lmControl.z_session_request.e_mode )
        {
          /*If this is FTM mode keep MGP to idle in between two fix sessions*/
          LM_MSG_MED("FTM Mode. Turning MGP to IDLE", 0, 0, 0);
          /*Change MGP state to IDLE*/
          if ( !lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE, NULL, FALSE) )
          {
            LM_MSG_HIGH("State update to LM_MGP_STATE_IDLE failed", 0, 0, 0);
          }

        }
        else
        {
          /*Set proper MGP state*/
          /*lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_session_request.q_num_fixes,
                                                  z_lmControl.z_session_request.q_tbf);*/

          if ( !lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, FALSE) )
          {
            LM_MSG_HIGH("State update to LM_MGP_STATE_OFF failed", 0, 0, 0);
          }

        }

        /*Update GPS session related state*/
        z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_GPS);

        if ( z_lmControl.q_reportsPending == 0 )
        {
          lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
        }
      }
    }
  }
  else
  {
    if ( !lm_tm_send_prm_report_to_tm(p_measReport, TRUE) )
    {
      LM_MSG_ERROR("Unable to send late PRM report to TM", 0, 0, 0);
    }
  }

  /* ME position measurements: For OEM DR enablement*/
  if ( TRUE == z_lmControl.u_oemdre_onoff )
  {
    lm_mgp_pos_meas_oem_data(p_measReport);
  }

}

/*
 ******************************************************************************
 * lm_mgp_handle_gps_meas
 *
 * Function description:
 *
 * This function handles the periodic PRM measurement report  from MGP
 *
 * Parameters: 
 * p_msg - Pointer to structure containing IPC message with meas report
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_gps_meas(const os_IpcMsgType* p_msg)
{
  sm_GpsMeasRptStructType* p_measReport;

  if(p_msg == NULL)
  {
    return;
  }

  /*lint -e826 */
  p_measReport = (sm_GpsMeasRptStructType*) p_msg->p_Data;
  /*lint +e826 */
  LM_MSG_MED("Received PERIODIC PRM report from MGP for %d (GPS/GLO/BDS/GAL/QZSS) system", p_measReport->z_MeasBlk.e_MeasBlkSrc, 0, 0);

  /* Cleaning the MEAS report */
  lm_mgp_clean_meas_rpt(p_measReport);

  /* NOTE: TM protocol modules must add code to "stitch" together per GNSS system meas reports
  from MGP */
  if(z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING)
  {
    if(!lm_tm_send_prm_report_to_tm(p_measReport,FALSE))
    {
      LM_MSG_ERROR("Unable to send periodic PRM report to TM",0,0,0);
    }
  }

  /* ME position measurements: For OEM DR enablement*/
  if(TRUE == z_lmControl.u_oemdre_onoff)
  {
    lm_mgp_pos_meas_oem_data(p_measReport);
  }

}

/*
 ******************************************************************************
 * lm_mgp_handle_sv_poly_report
 *
 * Function description:
 *
 * This function handles the periodic PRM measurement report  from MGP
 *
 * Parameters:
 * p_msg - Pointer to structure containing IPC message with meas report
 *
 * Return value:
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_sv_poly_report(os_IpcMsgType* p_msg)
{
  gnss_SvPoly* p_svPolyReport;

  if(p_msg == NULL)
  {
    return;
  }

  /*lint -e826 */
  p_svPolyReport = (gnss_SvPoly*) p_msg->p_Data;
  /*lint +e826 */
  LM_MSG_MED("Received SV POLY REPORT from MGP",0,0,0);


  {
   sm_SvPolyReportType z_SvPolyReport, * p_PolyReport = &z_SvPolyReport;


   z_SvPolyReport.z_SvPolyReport = *p_svPolyReport;

   if((z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING) ||
  	(z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING))

   {
	   if(!lm_tm_send_sv_poly_report_to_tm(p_PolyReport))
	  {
		LM_MSG_ERROR("Unable to send SV Poly report to TM",0,0,0);
	  }
   }
  }

  /* PE SV Poly info: For OEM DR enablement*/
  if(TRUE == z_lmControl.u_oemdre_onoff)
  {
    lm_mgp_sv_poly_oem_data(p_svPolyReport);
  }

}

/*
 ******************************************************************************
 * lm_mgp_calculate_mag_deviation
 *
 * Function description:
 *
 * This function calculated the Mag deviation from Fix report
 *
 * Parameters: 
 * p_fixReport - Pointer to Fix Report Structure
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
static void lm_mgp_calculate_mag_deviation(sm_GnssFixRptStructType* p_fixReport)
{  
  FLT   f_MagDev = 0.0;

  if (p_fixReport->z_NavPos.u_MagDevGood)
  {        
    f_MagDev = p_fixReport->z_NavPos.f_MagDeviation;        
  }
  else
  {
    /* If Mag Dev is not available, attempt to compute it */
    if (cgps_MagDeviation( p_fixReport->z_NavPos.d_PosLla[0], p_fixReport->z_NavPos.d_PosLla[1], &f_MagDev ) == TRUE)
    {          
      /*Calculation of Mag Dev Succesfull*/
      p_fixReport->z_NavPos.u_MagDevGood = TRUE;
    }
    else
    {
      p_fixReport->z_NavPos.u_MagDevGood = FALSE;
      MSG_MED("Mag Dev was not present and could not be calculated",0,0,0);
    }
  }

   p_fixReport->z_NavPos.f_MagDeviation = f_MagDev;
}

/*
 ******************************************************************************
 * lm_mgp_handle_fix_report
 *
 * Function description:
 *
 * This function handles the position fix report from MGP
 *
 * Parameters: 
 * p_msg - Pointer to structure containing IPC message with position fix
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_fix_report(os_IpcMsgType* p_msg)
{
  sm_GnssFixRptStructType* p_fixReport;
  boolean bBestAvailPos = FALSE, bFinalReport = FALSE;  
  boolean bSensorPropogatedGNSSFix = FALSE;
  float INSOnlyFixHepe = 0;

  if(p_msg == NULL || p_msg->p_Data == NULL)
  {
    return;
  }
  /*lint -e826 */
  p_fixReport = (sm_GnssFixRptStructType*) p_msg->p_Data;
  /*lint +e826 */

  if (p_fixReport->z_NavPos.z_SmPosFlags.b_IsPosBestAvail)
  {
    bBestAvailPos = TRUE; /* original NO_FIX report: CellDB fix, WiFi fix etc non-GNSS fix */
  }
  
  if (p_fixReport->z_NavPos.z_SmPosFlags.b_IsSensorPropogatedGNSSFix)
  {
    bSensorPropogatedGNSSFix = TRUE; /* INS/Dead Reckoning ONLY Fix */
  }
  
  LM_MSG_MED("Received FIX REPORT from MGP (BestAvailPos=%d, INSOnlyFix=%d)",(uint32)bBestAvailPos,(uint32)bSensorPropogatedGNSSFix,0);

  if((z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING) &&
     (z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX))
  {
    if (bBestAvailPos == TRUE)
    {
      /* original NO_FIX report: Increment null fix report counter */
      z_lmControl.z_early_exit_decision.q_NullFixReportCount++;
      z_lmControl.u_lastFixWasNoFixRpt = TRUE; 
    }
    else
    {
      /*Save the fix report to be used when session timesout*/
      z_lmControl.z_lastFixRpt = *p_fixReport; /*lint -e506 */
      z_lmControl.u_lastFixRptValid = TRUE;
      z_lmControl.u_lastFixWasNoFixRpt = FALSE;

      /*Calculate Mag Deviation*/
      lm_mgp_calculate_mag_deviation(p_fixReport);
      
      lm_mgp_set_sess_best_fix_report(p_fixReport);

      if (p_fixReport->z_NavPos.e_HoriRelIndicator != PDSM_POSITION_RELIABILITY_VERY_LOW)
	  {
	    if (lm_mgp_position_accuracy_check(p_fixReport))
	    {
	      bFinalReport = TRUE;
	    }
		else if (bSensorPropogatedGNSSFix)
	    {
	      /* Calculate HEPE for INS ONLY Fix */
		  INSOnlyFixHepe = (FLT) sqrt((p_fixReport->z_NavPos.f_ErrorEllipse[1] * 
                                       p_fixReport->z_NavPos.f_ErrorEllipse[1]) + 
                                      (p_fixReport->z_NavPos.f_ErrorEllipse[2] * 
                                       p_fixReport->z_NavPos.f_ErrorEllipse[2]));
		  /* If INS ONLY fix and HEPE <= 250m, send it out as final fix, even if it does not meet the dynamic HEPE check */
		  if (INSOnlyFixHepe <= (float)LM_HEPE_THRESHOLD)
		  {
		    LM_MSG_MED("Reporting INS ONLY fix as FINAL Fix with HEPE %d, even if INS HEPE does not meet dynamic HEPE accuracy",INSOnlyFixHepe,0,0);
		    bFinalReport = TRUE;
		  }
	    }	
      }
    }

    if (bFinalReport == TRUE)
    {
      /* handle final report (will do 0x1476 logging and do post-fix handling there) */
      lm_tm_handle_final_position_report(p_fixReport);
    }
    else
    {
      /* handle intermediate report (including original NO_FIX report) */
      /* log the 0x1476 log packet from LM. */
      gnss_NavSolutionStructType z_navSolution;
      memset((void*)&z_navSolution,0,sizeof(z_navSolution));

      if(sm_translate_sm_fix_report_to_gnss_nav_solution(&z_navSolution, p_fixReport, FALSE,
                                                         z_lmControl.z_fix_request.u_on_demand))
      {
        gnss_LogPositionReport(&z_navSolution, NULL); 
        /* count = NULL because we're not keeping track of how many messages we've sent out */
      }

      /*Send diag position report to TM and generate NMEA if enabled*/
      if(!lm_tm_send_position_report_to_tm(p_fixReport,FALSE))
      {
        LM_MSG_ERROR("Unable to send position report to TM",0,0,0);
      }

    }

    if (bBestAvailPos == FALSE)
    {
      /* Reset null fix report counter */
      z_lmControl.z_early_exit_decision.q_NullFixReportCount = 0;
    }

  }
  else
  {
    gnss_NavSolutionStructType z_navSolution; 
    memset((void*)&z_navSolution,0,sizeof(z_navSolution));

    if(sm_translate_sm_fix_report_to_gnss_nav_solution(&z_navSolution, p_fixReport, FALSE,
                                                       z_lmControl.z_fix_request.u_on_demand))
    {
      /* log the 0x1476 log packet from LM. */
      gnss_LogPositionReport(&z_navSolution, NULL); 
      /* count = NULL because we're not keeping track of how many messages we've sent out */
    }
  }

  /* PE position fix: For OEM DR enablement*/
  if(TRUE == z_lmControl.u_oemdre_onoff)
  {
    lm_mgp_fix_oem_data(p_fixReport);
  }

}


/*
 ******************************************************************************
 * lm_mgp_handle_pos_estimate_report
 *
 * Function description:
 *
 * This function handles the position estimate report from MGP
 *
 * Parameters: 
 * p_msg - Pointer to structure containing IPC message with position estimate report 
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_pos_estimate_report(os_IpcMsgType* p_msg)
{
  sm_GnssFixRptStructType* p_pos_estimate;

  if(p_msg == NULL)
  {
    return;
  }

  /*lint -e826 */
  p_pos_estimate = (sm_GnssFixRptStructType*) p_msg->p_Data;
  /*lint +e826 */

  LM_MSG_MED("Received POS estimate from MGP",0,0,0);

  /*Send position estimate report to TM*/
  if(!lm_tm_send_position_estimate_report_to_tm(p_pos_estimate))
  {
    LM_MSG_ERROR("Unable to send position estimate report to TM",0,0,0);
  }

}

/*
 ******************************************************************************
 * lm_mgp_handle_utc_info
 *
 * Function description:
 *
 * This function handles the UTC info decoded off the air by MGP or received 
 * from the PDE.
 *
 * Parameters: 
 * p_msg - Pointer to structure containing IPC message with UTC info
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_utc_info(const sm_UtcInfoRptStructType *p_newUtcInfo)
{
  lm_utc_data_type *p_curUtcInfo;

  LM_MSG_MED("Received UTC update...",0,0,0);

  if(p_newUtcInfo == NULL)
  {
    return;
  }

  p_curUtcInfo = &z_lmControl.z_utcData;

  if(p_curUtcInfo->u_utcModelValid)
  {
    /* handle case when u_WNt wraps from 255 to 0 */
    int8 b_WkDiff = (int8)(p_newUtcInfo->u_WNt - p_curUtcInfo->z_utcInfo.u_WNt);
    if( (b_WkDiff < 0) ||
        ( (p_curUtcInfo->z_utcInfo.u_WNt == p_newUtcInfo->u_WNt) && 
          (p_curUtcInfo->z_utcInfo.u_Tot >= p_newUtcInfo->u_Tot)) )
    {
      LM_MSG_MED("UTC update same/older than the current UTC data. Rejecting..",0,0,0);
      return;
    }
  }

  LM_MSG_MED("Updating UTC information..",0,0,0);
  p_curUtcInfo->z_utcInfo = *p_newUtcInfo;
  p_curUtcInfo->u_utcModelValid = TRUE;
  /*MRAWAL: Also update the EFS here*/

}

/*
 ******************************************************************************
 * lm_mgp_update_mgp_state_after_final_fix
 *
 * Function description:
 *
 * This function is called after the final fix(PRM or position fix) for a session is
 * reported. Depending on the value of N(number of fixes) and T(time between fixes)
 * for a application based tracking session, it will decide whether to keep the
 * MGP engine ON,IDLE or OFF. It also starts timers to eventually turn MGP OFF if 
 * the expected next position fix request doesn't arrive.
 *
 * Parameters: 
 * q_num_fixes - value of number of fixes for the session (N)
 * q_tbf       - value of time between fixes for the session (T)
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_update_mgp_state_after_final_fix(uint32 q_num_fixes, uint32 q_tbf)
{
  uint32 q_T1, q_T2, q_gracePeriod;
  uint32 q_prevShutOffTimeLeft;

  if(LM_GPS_MODE_FTM == z_lmControl.z_session_request.e_mode)
  {
    /*If this is FTM mode keep MGP to idle in between two fix sessions*/  
    LM_MSG_MED("FTM Mode. Turning MGP to IDLE",0,0,0);
    /*Change MGP state to IDLE*/
    if(!lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE,NULL,FALSE))
    {
      LM_MSG_HIGH("State update to LM_MGP_STATE_IDLE failed",0,0,0);
    }

    return;
  }

  LM_MSG_MED("lm_mgp_update_mgp_state_after_final_fix tbf %d num_fixes %d", q_tbf, q_num_fixes, 0);
  if(q_num_fixes > 1) /*Means app based tracking*/
  {
    q_T1          = lm_get_rc_on_threshold_period();
    q_T2          = lm_get_rc_idle_threshold();
    q_gracePeriod = lm_get_rc_shut_off_delay_margin();

    if(q_T1 >= q_T2)
    {
      LM_MSG_ERROR("RC_ON_THRESHOLD greater than or equal to RC_IDLE_THRESHOLD. Using defaults",0,0,0);
      lm_set_rc_on_threshold_period(LM_MAX_RC_ON_THRESHOLD);
      lm_set_rc_idle_threshold(LM_MAX_RC_IDLE_THRESHOLD);
      q_T1          = lm_get_rc_on_threshold_period();
      q_T2          = lm_get_rc_idle_threshold();
    }

    /*Apply the logic to decide whether to keep MGP ON, IDLE or OFF before the 
      next position fix request in app based tracking arrives*/

    if(q_tbf < q_T1)
    {
      /*Keep MGP in ON state and start RC_ON timer*/
      LM_MSG_MED("Starting RC_ON timer with value %d",q_T1,0,0);
      (void) os_TimerStart(z_lmControl.p_rcOnTimer,q_T1,0);

      /*New shut_off_time = 
                    max (TBF + shut_off_grace_period, remaining_previous_shut_off_period)*/

      q_prevShutOffTimeLeft = os_TimerCheck(z_lmControl.p_mgpShutOffTimer);

      if(q_tbf + q_gracePeriod > q_prevShutOffTimeLeft)
      {
        /*Stop previous shut-off timer*/
        os_TimerStop(z_lmControl.p_mgpShutOffTimer); /*lint !e534 */

        LM_MSG_MED("Starting SHUT_OFF timer with value %d shutofftimerinit val %d",q_tbf + q_gracePeriod,0,0);

        /* no need to kick off the dynamic qos reset logic if q_tbf < q_T1 */
        z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue = 0;

        (void) os_TimerStart( z_lmControl.p_mgpShutOffTimer, 
                              q_tbf + q_gracePeriod, 
                              0);
      }
      else
      {
        /*Let the previous shut-off timer continue*/
      }
    }
    else
      if(q_tbf < q_T2)
    {
      if(q_tbf <= C_MGP_DPO_MAX_SLEEP_TIME_MS)
      {
        /*Send FIX_END_IND to MGP and let MGP decide whether to goto DPO SLEEP or IDLE mode*/  
        lm_mgp_send_fix_end_ind();  
        /* in the case of DPO ON, we will not get a rcvr idle. as a result if the
         tbf is greater than the threshold, we need to reset the dynamic hepe logic.
         the start of next session request will not reset this logic in this case as
         LM wont sent an receiver ON since we never got a RCVR IDLE indication from ME */
        lm_reset_dynamic_hepe_thresold();
      }
      else
      {
        /*Turn MGP to IDLE*/ 
        if(!lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE, NULL,FALSE))
        {
          LM_MSG_HIGH("State update to LM_MGP_STATE_IDLE failed",0,0,0);
        }
      }

      /*New shut_off_time = 
                     max (TBF + shut_off_grace_period, remaining_previous_shut_off_period)*/

      q_prevShutOffTimeLeft = os_TimerCheck(z_lmControl.p_mgpShutOffTimer);

      if(q_tbf + q_gracePeriod > q_prevShutOffTimeLeft)
      {
        /*Stop previous shut-off timer*/
        os_TimerStop(z_lmControl.p_mgpShutOffTimer); /*lint !e534 */
        LM_MSG_MED("Starting SHUT_OFF timer with value = %d = shutofftimerinit val",q_tbf + q_gracePeriod,0,0);

        z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue = q_tbf + q_gracePeriod;

        (void) os_TimerStart( z_lmControl.p_mgpShutOffTimer, 
                              z_lmControl.z_early_exit_decision.q_ShutOffTimerInitValue, 
                              0);
      }
      else
      {
        /*Let the previous shut-off timer continue*/
      }
    }
    else
    {
      q_prevShutOffTimeLeft = os_TimerCheck(z_lmControl.p_mgpShutOffTimer);

      /*If time remaining in previous shut-off timer,
       change MGP to IDLE*/
      if(q_prevShutOffTimeLeft > 0)
      {
        if(!lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE, NULL,FALSE))
        {
          LM_MSG_HIGH("State update to LM_MGP_STATE_IDLE failed",0,0,0);
        }
      }
      else /*Else change MGP to OFF*/
      {
        if(!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL,FALSE))
        {
          LM_MSG_HIGH("State update to LM_MGP_STATE_OFF failed",0,0,0);
        }

      }

    }
  } /*if(z_lmControl.z_fix_request.q_num_fixes > 1) ends*/
  else
  {
    q_prevShutOffTimeLeft = os_TimerCheck(z_lmControl.p_mgpShutOffTimer);

    /*If time remaining in previous shut-off timer,
     change MGP to IDLE*/
    if(q_prevShutOffTimeLeft > 0)
    {
      if(!lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE, NULL,FALSE))
      {
        LM_MSG_HIGH("State update to LM_MGP_STATE_IDLE failed",0,0,0);
      }

    }
    else /*Else change MGP to OFF*/
    {
      if(!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL,FALSE))
      {
        LM_MSG_HIGH("State update to LM_MGP_STATE_OFF failed",0,0,0);
      }

    }
  }

}


/*
 ******************************************************************************
 * lm_mgp_rcvr_state_change
 *
 * Function description:
 *
 * This function handles the MGP status(ON/OFF/IDLE) updated coming from MGP.
 *
 * Parameters: 
 * e_mgpState - new MGP state
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_rcvr_state_change(lm_mgp_state_e_type e_mgpState)
{
  lm_mgp_state_e_type e_prevMgpState;

  if( e_mgpState == z_lmControl.e_currentMgpState )
  {
    /* current state the same as the reported new state, no action needed 
    one scenario this could happen is that since MGP always responds to
    LM's receiverON command, if by the time MGP receives receiverON it is
    already ON and has already sent an unsolicitated state change, another
    mgpState ON will be sent here */
    LM_MSG_MED("RCVR state report: same as current %u", e_mgpState ,0,0);
    return;
  }

  /**** proceed with the new, changed state *******************************/
  e_prevMgpState  = z_lmControl.e_currentMgpState;
  /*Update current state to the new state*/
  z_lmControl.e_currentMgpState = e_mgpState;

  /*Stop the intermediate MGP state timer*/
  if(e_mgpState == LM_MGP_STATE_ON)
  {
    LM_MSG_MED("RCVR state changed to ON",0,0,0);

    os_TimerStop(z_lmControl.p_goingONTimer);/*lint !e534 */

    /*Make sure the event goes out only first time we recieve it from MGP
    given, MGP might send this update multiple times*/
    if(LM_MGP_STATE_GOING_ON == e_prevMgpState)
    {
      /*Notify TM to generate the PD-API event*/
      (void)lm_tm_send_pd_api_event_to_tm(TM_PD_EVENT_GPS_BEGIN);

      /* Send a message to SM/TM to turn on streaming sensor data */
      if(LM_GPS_MODE_ODP != z_lmControl.z_session_request.e_mode)
      {
        sm_XspiStartInjection();
      }

      /* Reset time of first reportable fix */
      z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = 0;
      z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn = FALSE;
      (void) time_get(z_lmControl.z_early_exit_decision.d_TimeEngOn);
      z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn = FALSE;

      z_lmControl.z_early_exit_decision.f_LastValidFixHepe = LM_LAST_VALID_FIX_HEPE_MAX;
      z_lmControl.z_early_exit_decision.q_NullFixReportCount = 0;
      z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;
      z_lmControl.z_early_exit_decision.u_TimeoutAfterReinstate = FALSE;
      z_lmControl.z_early_exit_decision.u_OverrideTimeOfFirstValidFixAfterEngineOn = FALSE;
      /*Generate CGPS event*/
      sm_log_event_report_ex(LM_LOG_EVENT_ID_MGP_STATE_CHANGE,LM_LOG_EVENT_DATA_MGP_STATE_ON);
      /*Report Event*/
      event_report(EVENT_GPS_LM_MGP_ON);
    }

    if( z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_ON )
    {
      z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_ON;
      if( z_lm_sarf_state_db.e_client_type == LM_SARF_CLIENT_FTM )
      {
        if(z_lm_sarf_state_db.p_client_cb)
        {
          z_lm_sarf_state_db.p_client_cb(TRUE);
        }
      }
      event_report( EVENT_GPS_LM_ENTER_SA_RF_VERIF );
    }
  }/*if(e_mgpState == LM_MGP_STATE_ON)*/
  else if(e_mgpState == LM_MGP_STATE_OFF)
  {
    LM_MSG_MED("RCVR state changed to OFF",0,0,0);
    os_TimerStop(z_lmControl.p_goingOFFTimer);/*lint !e534 */
    /*Notify TM to generate the PD-API event*/
    (void)lm_tm_send_pd_api_event_to_tm(TM_PD_EVENT_GPS_DONE);    

    /* Send a message to SM/TM to turn off streaming sensor data */
    if(LM_GPS_MODE_ODP != z_lmControl.z_session_request.e_mode)
    {
      sm_XspiStopInjection();
    }

    /* Reset time of first reportable fix */
    lm_reset_dynamic_hepe_thresold();
    /*Generate CGPS event*/
    sm_log_event_report_ex(LM_LOG_EVENT_ID_MGP_STATE_CHANGE,LM_LOG_EVENT_DATA_MGP_STATE_OFF);
    /*Report Event*/
    event_report(EVENT_GPS_LM_MGP_OFF);
    if( z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_OFF )
    {
      z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_SLEEP;
      if( z_lm_sarf_state_db.e_client_type == LM_SARF_CLIENT_FTM )
      {
        if(z_lm_sarf_state_db.p_client_cb)
        {
          z_lm_sarf_state_db.p_client_cb(TRUE);
        }
      }
      event_report( EVENT_GPS_LM_EXIT_SA_RF_VERIF );
    }
  }/*else if(e_mgpState == LM_MGP_STATE_OFF)*/
  else if(e_mgpState == LM_MGP_STATE_IDLE)
  {
    LM_MSG_MED("RCVR state changed to IDLE",0,0,0);
    os_TimerStop(z_lmControl.p_goingIDLETimer);/*lint !e534 */
    /*Notify TM about MGP IDLE state. This is not really a PDAPI event*/
    (void)lm_tm_send_pd_api_event_to_tm(TM_PD_EVENT_GPS_IDLE);
    /*Generate CGPS event*/
    sm_log_event_report_ex(LM_LOG_EVENT_ID_MGP_STATE_CHANGE,LM_LOG_EVENT_DATA_MGP_STATE_IDLE);
    /*Report Event*/
    event_report(EVENT_GPS_LM_MGP_IDLE);
    #ifdef FEATURE_CGPS_USES_CDMA
    /*Start processing a PPM request that was pending because MGP was ON and had to be turned to IDLE*/
    if(z_lmControl.u_ppm_request_pending)
    {
      LM_MSG_MED("A PPM request was waiting for MGP to go IDLE. Processing now..",0,0,0);  
      z_lmControl.u_ppm_request_pending = FALSE;

      if(!lm_mgp_aflt_start_ppm_processing(&z_lmControl.z_cdma_request))
      {
        z_lmControl.u_ppm_get_results_pending = FALSE; /*also reset this flag for safety*/   
        LM_MSG_MED("Failed to start PPM processing.Sending NACK for handle[%d]",z_lmControl.q_sessHandle,0,0);
        if(!lm_send_tm_session_update_nack(z_lmControl.q_sessHandle,TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR))
        {
          LM_MSG_ERROR("Unable to send session update NACK to TM",0,0,0);
        }

      }
      else
      {
        /*If we also got a get_aflt_results requests while we were waiting for MGP to go IDLE,
        process that request now*/  
        if(z_lmControl.u_ppm_get_results_pending)
        {
          LM_MSG_MED("GET_PPM request also pending. Processing now..",0,0,0);  
          z_lmControl.u_ppm_get_results_pending = FALSE;  

          /*Request PPM result from CDMA searcher*/
          lm_mgp_aflt_get_results(FALSE);
          lm_update_session_state(LM_SESSION_STATE_IN_SESSION_REPORT_PENDING);
          z_lmControl.q_reportsPending |= (uint32)LM_SESSION_REQUEST_CDMA;  
        }

        if(!lm_send_tm_session_update_ack(z_lmControl.q_sessHandle,TM_STATUS_ACK_OK))
        {
          LM_MSG_ERROR("Unable to send session update ACK to TM. Stopping the entire session",0,0,0);
          /*Stop a session*/
          lm_tm_stop_session();
        }
      }

    }/*if(z_lmControl.u_ppm_request_pending)*/
    #endif
  }/*else if(e_mgpState == LM_MGP_STATE_IDLE)    */

  /*If the new state is not the desired state, update the MGP state to the 
  desired state*/
  if(z_lmControl.e_desiredMgpState != z_lmControl.e_currentMgpState)
  {
    if(z_lmControl.e_desiredMgpState == LM_MGP_STATE_ON)
    {
      LM_ASSERT(z_lmControl.u_MgpONParamsValid);

      if(z_lmControl.u_MgpONParamsValid)
      {
        if(!lm_mgp_update_mgp_state(z_lmControl.e_desiredMgpState,&z_lmControl.z_MgpONParams,FALSE))
        {
          LM_MSG_HIGH("State update to %d failed",z_lmControl.e_desiredMgpState,0,0);
        }
      }
      else
      {
        LM_MSG_ERROR("MGP ON parameters not valid for handle[%d]",z_lmControl.q_sessHandle,0,0);
      }
    }
    else
    {
      if(!lm_mgp_update_mgp_state(z_lmControl.e_desiredMgpState,NULL,FALSE))
      {
        LM_MSG_HIGH("State update to %d failed",z_lmControl.e_desiredMgpState,0,0);
      }
    }
  }


}

/*
 ******************************************************************************
 * lm_mgp_handle_status_update
 *
 * Function description:
 *
 * This function handles the MGP status updates coming from MGP.
 *
 * Parameters: 
 * *p_msg - IPC message containing the MGP status update
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_status_update(os_IpcMsgType* p_msg)
{
  sm_RcvrStateChangeStructType* p_rcvrState = NULL;
  lm_mgp_state_e_type e_mgpState;  
  lm_recvr_state_event_info rcvr_state_event_payload = {0};

  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */
  p_rcvrState = (sm_RcvrStateChangeStructType*) p_msg->p_Data;
  /*lint +e826 */ 

  /*Report QXDM Event on GNSS Receiver state*/
  rcvr_state_event_payload.q_GpsRtcMs = p_rcvrState->q_GpsRtcMs;
  rcvr_state_event_payload.q_receiver_state = (uint32)p_rcvrState->e_CurrentRcvrState;
  event_report_payload(EVENT_GNSS_RCVR_STATE, sizeof(rcvr_state_event_payload), (void *)&(rcvr_state_event_payload));

  switch(p_rcvrState->e_CurrentRcvrState)
  {
  case C_GPS_RCVR_STATE_OFF:                /* Receiver State is OFF */
  case C_GPS_RCVR_STATE_IDLE:               /* Receiver State is IDLE */
  case C_GPS_RCVR_STATE_ON:                 /* Receiver State is ON */
  case C_GPS_RCVR_STATE_KEEP_WARM_DFRD_OFF: /* Receiver State is ON (waiting to go OFF) */
  case C_GPS_RCVR_STATE_KEEP_WARM_DFRD_IDLE: /* Receiver State is ON (waiting to go IDLE) */
    {
      e_mgpState = lm_mgp_get_mgp_state_type(p_rcvrState->e_CurrentRcvrState);

      LM_ASSERT(e_mgpState != LM_MGP_STATE_INVALID);
      lm_mgp_rcvr_state_change(e_mgpState);

      /*Invalidate the ON parameters since they are to be used only 
        once to call mgp_ReceiverOn()*/
      if(e_mgpState == LM_MGP_STATE_ON)
      {
        z_lmControl.u_MgpONParamsValid = FALSE;
      }

      break;
    }

  case C_GPS_RCVR_STATE_TRANSITIONING:
  case C_GPS_RCVR_STATE_DPO_SLEEP:          /* DPO Sleep is a transition state for Rcvr ON */
    {
      /*Do nothing*/
      break;
    }
  default:
    break;
  } /*switch (p_rcvrState->e_CurrentRcvrState) ends*/

}

/*
 ******************************************************************************
 * lm_mgp_handle_aiding_wishlist_request
 *
 * Function description:
 * This function handles the aiding data wishlist request from MGP. It will 
 * request MGP to provide it's current aiding data status and the wishlist of 
 * what more is needed.
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
void lm_mgp_handle_aiding_wishlist_request(lm_wishlist_src_req_e_type wl_src)
{
  LM_MSG_MED("Received AIDING DATA WISHLIST req from TM src %d", wl_src,0,0);

  switch(wl_src)
  {
  case(LM_ASSIST_DATA_REQ_MSB):
    mgp_RequestAssistDataStatus(MGP_ASSIST_DATA_REQ_MSB_QUERY); 
    break;
  case(LM_ASSIST_DATA_REQ_QUERY):
    mgp_RequestAssistDataStatus(MGP_ASSIST_DATA_REQ_NON_MSB_QUERY); 
    break;

  default:
    break;
  }
}

/*
 ******************************************************************************
 * lm_mgp_handle_delete_mgp_info
 *
 * Function description:
 * This function handles the MGP info delete request from TM
 * 
 *
 * Parameters: 
 * *p_Req - delete MGP info request from TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void lm_mgp_handle_delete_mgp_info(lm_request_delete_mgp_info_s_type *p_Req)
{
  if(p_Req == NULL)
  {
    return;
  }

  LM_MSG_LOW("Calling MGP delete function",0,0,0);

  if(p_Req->force_rcvr_off)
  {
    os_TimerStop(z_lmControl.p_mgpShutOffTimer);/*lint !e534 */
    os_TimerStop(z_lmControl.p_rcOnTimer);/*lint !e534 */
    if(!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, TRUE))
    {
      LM_MSG_HIGH("State update to %d failed",LM_MGP_STATE_OFF,0,0);
    }
  }
  p_Req->z_mgp_info.u_DeleteAll = p_Req->delete_all;

  /* SM always calls Gen8 delete function. any translation to 
  Gen7 ME API will be handled inside mgp_GnssDeleteInfo().
  */
  mgp_GnssDeleteInfo(&p_Req->z_mgp_info);
}

/*
 ******************************************************************************
 * lm_mgp_handle_aiding_data_info
 *
 * Function description:
 * This function handles aiding data info from TM. The aiding data info is
 * injected into MGP. If the info consists of AA data and a PRM session is
 * waiting on AA data to turn MGP ON, MGP will be turned to ON state and
 * session state will be updated.
 * 
 *
 * Parameters: 
 * *p_aidingData - aiding data from TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_aiding_data_info(lm_info_aiding_data_s_type* p_aidingData)
{
  if(p_aidingData == NULL)
  {
    return;
  }

  if((p_aidingData->z_assist_data.e_AssistanceType == MGP_GNSS_ASSIST_DATA_EPH) ||
     (p_aidingData->z_assist_data.e_AssistanceType == MGP_GNSS_ASSIST_DATA_ALM))
  {
    mgp_AgpsNetworkConnectActive();
  }
  
  /*Inject the aiding data into MGP*/
  if((p_aidingData->z_assist_data.e_AssistanceType > MGP_GNSS_ASSIST_DATA_NONE) &&
     (p_aidingData->z_assist_data.e_AssistanceType < MGP_GNSS_ASSIST_DATA_MAX))
  {
    /* No need to process Ref Time here since its injected to slow clk down below */
    if( p_aidingData->z_assist_data.e_AssistanceType != MGP_GNSS_ASSIST_DATA_REF_TIME )
    {
      (void) lm_mgp_update_info(&p_aidingData->z_assist_data);
    }

    if( p_aidingData->z_assist_data.e_AssistanceType == MGP_GNSS_ASSIST_DATA_REF_TIME &&
        p_aidingData->z_assist_data.z_AssistData.z_RefTimeInfo.u_GpsWeekIsValid
      )
    {
      DBL  d_FullGpsMs;
      lm_request_slow_clk_time_inject_s_type z_slow_clk_inject;   
      memset((void*)&z_slow_clk_inject,0,sizeof(z_slow_clk_inject)); 


      if( p_aidingData->z_assist_data.e_GnssType == MGP_GNSS_TYPE_GPS )
      {
        z_slow_clk_inject.e_source = GNSS_SLOW_CLOCK_SRC_GPS;
        z_slow_clk_inject.z_slow_clk_time.b_TimeIsGps = TRUE;
      }
      else if( p_aidingData->z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO )
      {
        z_slow_clk_inject.e_source = GNSS_SLOW_CLOCK_SRC_GLONASS;
      }

      z_slow_clk_inject.z_slow_clk_time.b_ForceFlag = FALSE;
      z_slow_clk_inject.z_slow_clk_time.e_TimeSource = MGP_SLOW_CLK_SRC_NETWORK;
      z_slow_clk_inject.z_slow_clk_time.q_Uncertainty = 
      (uint32) p_aidingData->z_assist_data.z_AssistData.z_RefTimeInfo.f_GpsTimeUnc;

      d_FullGpsMs = cgps_FullGpsMs( p_aidingData->z_assist_data.z_AssistData.z_RefTimeInfo.w_GpsWeek, 
                                    p_aidingData->z_assist_data.z_AssistData.z_RefTimeInfo.q_GpsMsec );

      z_slow_clk_inject.z_slow_clk_time.t_Milliseconds = (uint64) d_FullGpsMs;

      z_slow_clk_inject.z_slow_clk_time.u_DiscontinuityCnt = 0;/* This is ignored by slow clk since its n/w time */
      (void) lm_mgp_handle_slow_clk_time_inject_request(&z_slow_clk_inject);

    }

    /*Report event*/
    event_report_payload(EVENT_GPS_LM_AIDING_DATA_RECEIVED,
                         sizeof(gps_AssistDataEnumType),
                         (void*)&p_aidingData->z_assist_data.e_AssistanceType);
  }

  /*Take the appropriate action if we were waiting for AA data for PRM session*/
  if((p_aidingData->z_assist_data.e_AssistanceType == MGP_GNSS_ASSIST_DATA_ACQ_ASSIST) &&
     (z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING)   &&
     (z_lmControl.u_waitingAAData))
  {
    LM_MSG_MED("AA data received. Continue PRM processing",0,0,0);  
    (void)lm_mgp_aa_data_received();
  }

}
/*
 ******************************************************************************
 * lm_mgp_handle_assist_status_report
 *
 * Function description:
 * This function handles assitance data status and wishlist report from MGP.
 * The report will be sent to TM regardles of whether a LM-TM session is going on
 * or not.
 * 
 *
 * Parameters: 
 * *p_aidingData - aiding data from TM
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_assist_status_report(os_IpcMsgType* p_msg)
{
  sm_InternalAssistDataStatusStructType* p_AssistDataStatus = NULL;

  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */
  p_AssistDataStatus = (sm_InternalAssistDataStatusStructType*) p_msg->p_Data;
  /*lint +e826 */ 

  if(!lm_tm_send_assist_data_status_report_to_tm(p_AssistDataStatus,FALSE))
  {
    LM_MSG_ERROR("Failed sending assistance data status report to TM",0,0,0);
  }
}

/*
 ******************************************************************************
 * lm_mgp_get_data_src_type
 *
 * Function description:
 * This function returns gps_DataSourceType equivalent of  cgps_SrvSystemType
 *
 * Parameters: 
 * cgps_SrvSystemType 
 *
 * Return value: 
 * gps_DataSourceType* - Equivalent enumeration in gps_DataSourceType
 * boolean - Equivalent enumeration found or not
 * None
 *
 ******************************************************************************
*/
boolean lm_mgp_get_data_src_type(cgps_SrvSystemType e_srv_sys_type, gps_DataSourceType* p_data_src_type)
{

  if(p_data_src_type == NULL)
  {
    return FALSE;
  }

  switch(e_srv_sys_type)
  {
  case CGPS_SRV_SYS_NONE:          /* NO Service */
  case CGPS_SRV_SYS_GW:            /* GSM and WCDMA mode */
  case CGPS_SRV_SYS_OTHER:         /* All other modes */
    return FALSE;  
  case CGPS_SRV_SYS_CDMA:          /* CDMA mode */
    *p_data_src_type = SOURCE_1X;
    return TRUE;
  case CGPS_SRV_SYS_HDR:           /* HDR Mode */
    *p_data_src_type = SOURCE_EVDO;
    return TRUE;
  case CGPS_SRV_SYS_GSM:           /* GSM mode */
    *p_data_src_type = SOURCE_GSM;
    return TRUE;
  case CGPS_SRV_SYS_WCDMA:         /* WCDMA mode */
    *p_data_src_type = SOURCE_WCDMA;
    return TRUE;
  case CGPS_SRV_SYS_LTE:         /* LTE mode */
    *p_data_src_type = SOURCE_LTE;
    return TRUE;
  case CGPS_SRV_SYS_TDSCDMA:         /* LTE mode */
    *p_data_src_type = SOURCE_TDSCDMA;
    return TRUE;

  default:
    return FALSE;
  }

}

/*
 ******************************************************************************
 * lm_mgp_handle_rf_info_request
 *
 * Function description:
 * This function handles the RF INFO request from TM. This info is used for
 * KDDI.  
 *
 * Parameters: 
 * p_rf_info_request - pointer to the info request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_rf_info_request(lm_request_rf_info_s_type* p_rf_info_request)
{
  gps_DataSourceType e_data_src_type;

  if(p_rf_info_request == NULL)
  {
    return;
  }

  LM_MSG_MED("Received RF INFO request from TM. Serv system %d",
             (uint32)p_rf_info_request->e_serv_sys_type,0,0);   

  if(!lm_mgp_get_data_src_type(p_rf_info_request->e_serv_sys_type,&e_data_src_type))
  {
    LM_MSG_ERROR("Invalid Serving system %d received in RF Info request",
                 (uint32)p_rf_info_request->e_serv_sys_type,0,0);  
  }
  else
  {
    mgp_RfInfoReq( e_data_src_type, TRUE ); 
  }
}

/*
 ******************************************************************************
 * lm_mgp_handle_gps_time_request
 *
 * Function description:
 * This function handles the GPS time request from TM.
 * 
 *
 * Parameters: 
 * p_gps_time_request - pointer to the info request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_gps_time_request(lm_request_gps_info_s_type *p_gps_time_request)
{
  LM_MSG_MED("Received GPS time request from TM. source:%d",
             (uint32)p_gps_time_request->e_time_chk_type,0,0);   
  mgp_GnssReqGpsTime();
}

/*
 ******************************************************************************
 * lm_mgp_handle_rcOn_timer_timeout
 *
 * Function description:
 * This function handles the RC_ON timer timeout event. If there is no fix or 
 * PRM session going on, and MGP is still in ON state, MGP will be put in IDLE
 * state. 
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
void lm_mgp_handle_rcOn_timer_timeout(void)
{
  LM_MSG_MED("RC_ON Timer timedout",0,0,0);

  if( (z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION) ||
      (!(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX) &&
       !(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS)) )
  {
    if((z_lmControl.e_currentMgpState == LM_MGP_STATE_ON) ||
       (z_lmControl.e_currentMgpState == LM_MGP_STATE_GOING_ON))
    {
      if(!lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE,NULL,FALSE))
      {
        LM_MSG_HIGH("State update to %d failed",LM_MGP_STATE_IDLE,0,0);
      }

    }
  }
  /*Report Event*/
  event_report(EVENT_GPS_LM_RC_ON_TIMER_TIMEOUT);                     
}


/*
 ******************************************************************************
 * lm_mgp_handle_shutOff_timer_timeout
 *
 * Function description:
 * This function handles the shut-Off timer timeout event. If there is no fix or 
 * PRM session going on, and MGP is not in OFF state, MGP will be put in OFF
 * state. 
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
void lm_mgp_handle_shutOff_timer_timeout(void)
{
/*A shut-off timer started for previous session may expire in the current session.
  In that case just ignore the timeout*/

 /*Handle shutt off timer expiry only if the timer is not re-armed*/
  if(os_TimerCheck(z_lmControl.p_mgpShutOffTimer)== 0)
  {
    LM_MSG_MED("Shut-off timer timedout",0,0,0);

    if( (z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION) ||
        (!(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_FIX) &&
         !(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_GPS)) )
    {
      if(!lm_mgp_update_mgp_state(LM_MGP_STATE_OFF,NULL,FALSE))
      {
        LM_MSG_HIGH("State update to %d failed",LM_MGP_STATE_OFF,0,0);
      }

    }

    /*Report Event*/
    event_report(EVENT_GPS_LM_SHUT_OFF_TIMER_TIMEOUT);                     
  }
  else
  {
    LM_MSG_MED("Shut-off timer timeout neglected, timer already re-armed",0,0,0);
  }
}

void lm_mgp_handle_events( os_IpcMsgType* p_msg )
{

}

/*
 ******************************************************************************
 * lm_mgp_handle_pos_estimate_request
 *
 * Function description:
 * This function handles position estimate request from TM. MGP API to get the
 * position estimate from PE will be called.  
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
void lm_mgp_handle_pos_estimate_request(void)
{
  /*Call the MGP API to get the reference position from PE*/
  LM_MSG_MED("POS estimate request received from TM",0,0,0);  
  mgp_ReqPosEstimate();
}


/*
 ******************************************************************************
 * lm_mgp_handle_pos_unc
 *
 * Function description:
 * This function injects new POS UNC into MGP
 *
 * Parameters: 
 * f_new_pos_unc
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_pos_unc(FLT f_new_pos_unc)
{
  mgp_ModifyPosUnc(f_new_pos_unc);  
}

/*
 ******************************************************************************
 * lm_mgp_handle_pos_offset
 *
 * Function description:
 * This function injects new POS offset into MGP
 *
 * Parameters: 
 * q_pos_offset
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_pos_offset(uint32 q_pos_offset)
{
  LM_MSG_ERROR("No MGP API available for injecting POS offset",0,0,0);  
}

/*
 ******************************************************************************
 * lm_mgp_handle_time_unc
 *
 * Function description:
 * This function injects new time UNC into MGP
 *
 * Parameters: 
 * f_new_time_unc
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_time_unc(FLT f_new_time_unc)
{
  mgp_GnssModifyTimeUnc(MGP_GNSS_TYPE_ALL, f_new_time_unc);
} /* lm_mgp_handle_time_unc */

/*
 ******************************************************************************
 * lm_mgp_handle_time_offset
 *
 * Function description:
 * This function injects new time offset into MGP
 *
 * Parameters: 
 * f_time_offset
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_time_offset(FLT f_time_offset)
{
  mgp_GnssAddTimeOffset(MGP_GNSS_TYPE_ALL, f_time_offset);
}

/*
 ******************************************************************************
 * lm_mgp_handle_session_type
 *
 * Function description:
 * This function sends a message to ME of a start of a session to enable it to early
 * Time transfer
 *
 * Parameters: 
 * z_sess_info
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_session_type( lm_info_session_info_type       z_sess_info)
{
 // mgp_GnssHandleSessionType(MGP_GNSS_TYPE_ALL, z_sess_info);

}

/*
 ******************************************************************************
 * lm_mgp_handle_slow_clk_time_inject_request
 *
 * Function description:
 * This function injects external time from sources like PC or network into MGP
 *
 * Parameters: 
 * p_slow_clk_inject
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void  lm_mgp_handle_slow_clk_time_inject_request(lm_request_slow_clk_time_inject_s_type* p_slow_clk_inject)
{
  gnss_SlowClkTimeInjectionType z_slow_clk_time = {0};
  z_slow_clk_time.b_ForceFlag = p_slow_clk_inject->z_slow_clk_time.b_ForceFlag;
  z_slow_clk_time.e_TimeSrcType = p_slow_clk_inject->e_source;
  z_slow_clk_time.e_TimeSource = p_slow_clk_inject->z_slow_clk_time.e_TimeSource;
  z_slow_clk_time.q_Uncertainty = p_slow_clk_inject->z_slow_clk_time.q_Uncertainty;
  z_slow_clk_time.t_Milliseconds = p_slow_clk_inject->z_slow_clk_time.t_Milliseconds;
  z_slow_clk_time.u_DiscontinuityCnt = p_slow_clk_inject->z_slow_clk_time.u_DiscontinuityCnt;
  z_slow_clk_time.b_TimeIsGps = p_slow_clk_inject->z_slow_clk_time.b_TimeIsGps;

  mgp_GnssSlowClkExtTimePut(&z_slow_clk_time);
}


/*
 ******************************************************************************
 * lm_mgp_handle_external_coarse_pos_inject_request
 *
 * Function description:
 * This function injects external coarse position from external sources 
 * into MGP
 *
 * Parameters: 
 * lm_mgp_handle_external_coarse_pos_inject_request
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/

void  lm_mgp_handle_external_coarse_pos_inject_request
(lm_request_extern_coarse_pos_inject_s_type* p_extern_coarse_pos_inj )
{
  mgp_ExternalCoarsePosPut( &p_extern_coarse_pos_inj->z_extern_coarse_pos);  
}

/*
 ******************************************************************************
 * lm_mgp_handle_rf_info
 *
 * Function description:
 *
 * This function handles the SA data info from MGP
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
void lm_mgp_handle_rf_info(os_IpcMsgType* p_msg)
{
  sm_ReportRfInfoStructType* p_rf_info;

  if(p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("RF INFO received from MGP. Forwarding to TM..",0,0,0);

  /*lint -e826 */ 
  p_rf_info = (sm_ReportRfInfoStructType*) p_msg->p_Data;
  /*lint +e826 */ 

  (void)lm_tm_send_rf_info_to_tm(p_rf_info); 
}

/*
 ******************************************************************************
 * lm_mgp_handle_wbiq_info
 *
 * Function description:
 *
 * This function handles the WBIQ info (Cno, freq) info from MGP
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
void lm_mgp_handle_wbiq_info(os_IpcMsgType* p_msg)
{
  if(p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("WBIQ PARAMS received from MGP.",0,0,0);

  /* Notify TM of this data */
  tm_api_report_wbiq_info((sm_ReportWBIQInfoStructType*) p_msg->p_Data);

  /* This marks the termination of the msg ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS.
    New code can be added (by the customer) to print out/use these values. */
}

/*
 ******************************************************************************
 * lm_mgp_handle_nbiq_info
 *
 * Function description:
 *
 * This function handles the NBIQ info from MGP
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
void lm_mgp_handle_nbiq_info(os_IpcMsgType* p_msg)
{
  sm_ReportNBIQInfoStructType* p_nbiq_info;

  if(p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("NBIQ PARAMS received from MGP.",0,0,0);


  p_nbiq_info = (sm_ReportNBIQInfoStructType*) p_msg->p_Data;

  LM_MSG_HIGH("NBIQ data ptr 0x%x, Size %d", 
              p_nbiq_info->p_NBIQDataPtr, 
              p_nbiq_info->q_NBIQDataSize, 0);


  /* This marks the termination of the msg ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS.
    New code can be added (by the customer) to print out/use these values. */

}

/*
 ******************************************************************************
 * lm_mgp_handle_gnss_rf_status_info
 *
 * Function description:
 *
 * This function handles the GNSS RF status info from MGP
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
void lm_mgp_handle_gnss_rf_status_info(os_IpcMsgType* p_msg)
{
  sm_ReportGNSSRfStatusInfoStructType* p_GnssRfInfo;

  if(p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("GNSS RF status info received from MGP.",0,0,0);


  p_GnssRfInfo = (sm_ReportGNSSRfStatusInfoStructType*) p_msg->p_Data;

  LM_MSG_HIGH( "GNSS RF status info PGA Gain %d BP1 AmpI %d AmpQ %d", 
               p_GnssRfInfo->l_PGAGain, 
               p_GnssRfInfo->q_Bp1LbwAmplI, 
               p_GnssRfInfo->q_Bp1LbwAmplQ );

  /* This marks the termination of the msg ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS.
    New code can be added (by the customer) to print out/use these values. */

}

/*
 ******************************************************************************
 * lm_mgp_handle_request_sa_data
 *
 * Function description:
 *
 * This function handles the SA data request from MGP
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
void lm_mgp_handle_request_sa_data(void)
{
  if(z_lmControl.e_sessionState == LM_SESSION_STATE_NO_SESSION)
  {
    LM_MSG_ERROR("MGP requested SA data while no session is active",0,0,0);  
    return;
  }

  (void)lm_tm_send_sa_data_request_to_tm(); 
}

/*
 ******************************************************************************
 * lm_log_early_exit_decision
 *
 * Function description:
 *
 * This function generates a DM log message
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void lm_log_early_exit_decision(
                               const uint32 q_TimeFromEngOn, 
                               const uint32 q_TimeFromSessStart, 
                               const uint32 q_FCount,
                               const uint16 w_HepeLimitCurrent, 
                               const uint16 w_HepeLimitDynamic,
                               const uint16 w_HepeMax,
                               const uint16 w_Hepe,
                               const boolean u_FixValid
                               )
{
  LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C_type *p_LogPkt;

  lm_early_exit_decision_info_s_type *p_EarlyExitDecisionType = &z_lmControl.z_early_exit_decision;
  if(p_EarlyExitDecisionType == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C_type*)log_alloc( LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C,
                                                                      sizeof(LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C_type) );

  if(p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = 1;
    p_LogPkt->e_decision = p_EarlyExitDecisionType->e_decision;
    p_LogPkt->w_QoSTimeoutOverride = (uint16) p_EarlyExitDecisionType->q_QoSTimeoutOverride;
    p_LogPkt->q_TimeEngOn = q_TimeFromEngOn;
    p_LogPkt->q_TimeOfFirstValidFixAfterEngineOn = p_EarlyExitDecisionType->q_TimeOfFirstValidFixAfterEngineOn;
    p_LogPkt->q_TimeSessionStart = q_TimeFromSessStart;
    p_LogPkt->q_TimeOfFirstValidFixAfterSessStart = p_EarlyExitDecisionType->q_TimeOfFirstValidFixAfterSessStart;
    p_LogPkt->q_FCount = q_FCount;
    p_LogPkt->w_QoSHepeOverride = p_EarlyExitDecisionType->w_QoSThresholdOverride;
    p_LogPkt->w_HepeLimitCurrent = w_HepeLimitCurrent;
    p_LogPkt->w_HepeLimitDynamic = w_HepeLimitDynamic;
    p_LogPkt->w_HepeMax = w_HepeMax;
    p_LogPkt->w_HepeMaxDefault = z_lmControl.q_HepeThreshold;
    p_LogPkt->u_FixValid = u_FixValid;
    p_LogPkt->w_Hepe = w_Hepe;
    log_commit(p_LogPkt);
  }
  return;
}

/*
 ******************************************************************************
 * lm_mgp_handle_gps_time
 *
 * Function description:
 *
 * This function handles the GPS time info from MGP
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
void lm_mgp_handle_gps_time(os_IpcMsgType* p_msg)
{
  sm_ReportGpsTimeStructType *p_gps_time_report;

  if(p_msg == NULL)
  {
    return;
  }

  LM_MSG_HIGH("GPS time received from MGP. Forwarding to TM..",0,0,0);

  /*lint -e826 */ 
  p_gps_time_report = (sm_ReportGpsTimeStructType*) p_msg->p_Data;
  /*lint +e826 */ 

  (void)lm_tm_send_gps_time_to_tm(p_gps_time_report); 
}


/**
* @brief  LM abstraction layer around mgp_UpdateInfo. This
*         abstratcion implements a "shim" to enable backwards
*         compatibility with GEN7 MGP.
  
  @return      
  @sideeffects None
* @see mgp_GnssUpdateInfo() and mgp_UpdateInfo()

*/
boolean lm_mgp_update_info(gnss_AssistDataInfoStructType *p_assistDataInfo)
/**< GEN8 assistance data received from network */
{
  boolean b_retval = (p_assistDataInfo != NULL);
  if(b_retval)
  {
    LM_MSG_MED("Post %d (GPS/GLO/BDS) %d AD to MGP", p_assistDataInfo->e_GnssType, p_assistDataInfo->e_AssistanceType, 0 );

    /* SM always calls gen 8 update info function. Any translation
    needed to Gen 7 ME will be handled inside mgp_GnssUpdateInfo() 
    */
    mgp_GnssUpdateInfo(p_assistDataInfo);
  }
  return b_retval;
}


/**
* @brief  translates gnss assist data info to older gen7 format.
*         this is used to retain backwards compatibility with
*         gen7 mgp. Should only be used when
*         FEATURE_GPS_GEN7_ME_API is defined
  
* @return boolean. false on error, true otherwise.
  @sideeffects None
* @see mgp_GnssUpdateInfo() and mgp_UpdateInfo()

*/
boolean lm_translate_gnss_to_gps_assist_data(
                                            gps_AssistDataInfoStructType *p_Dest, 
                                            /**< gen7 assistance data */
                                            const gnss_AssistDataInfoStructType *p_Src)
/**< gen8 or newer assistance data */
{
  MSG_ERROR("lm_translate_gnss_to_gps_assist_data: "\
            "API should only be used when FEATURE_GPS_GEN7_ME_API is defined",
            0, 0, 0);
  return FALSE;
} /* end lm_translate_gnss_to_gps_assist_data */

/*
 ******************************************************************************
 * lm_mgp_handle_force_rcvr_off
 *
 * Function description:
 *
 * This function sends receiver off (forced) to MGP
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
void lm_mpg_handle_force_rcvr_off(void)
{
  lm_mgp_stop_position_fix_processing(TRUE);

}

/*
 ******************************************************************************
 * lm_mgp_handle_rcvr_off
 *
 * Function description:
 *
 * This function sends receiver off  to MGP
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
void lm_mpg_handle_rcvr_off(void)
{
  lm_mgp_stop_position_fix_processing(FALSE);
}

/*
 ******************************************************************************
 * lm_mpg_handle_reset_location_service
 *
 * Function description:
 *
 * This function sends reset location service request to MGP
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
void lm_mpg_handle_reset_location_service(void)
{  
  /*Expect MGP going to OFF*/
  z_lmControl.e_desiredMgpState = LM_MGP_STATE_OFF;

  /*Since LM may not be getting MGP STATE OFF indication, give LM
    the MGP off indication before sending reset request to MGP*/
  lm_mgp_rcvr_state_change(LM_MGP_STATE_OFF);

  /*Send reset request to MGP*/
  (void)mgp_HardReset();
}


/*
 ******************************************************************************
 * lm_mgp_send_fix_start_ind
 *
 * Function description:
 *
 * This function sends FIX_START_IND to MGP
 *
 * Parameters: 
 * q_Tbf - TBF associated with the fix request.
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_send_fix_start_ind(uint32 q_Tbf)
{
  mgp_SmEventData z_SmEventData;

  z_SmEventData.e_SmEvent = C_MGP_FIX_START_IND;
  z_SmEventData.z_SmEventpayload.z_FixStartData.q_Tbf = q_Tbf;
  z_SmEventData.z_SmEventpayload.z_FixStartData.q_RcvrOnThresholdTime = lm_get_rc_on_threshold_period ();
  z_SmEventData.z_SmEventpayload.z_FixStartData.q_RcvrIdleThresholdTime = lm_get_rc_idle_threshold ();

  (void)mgp_ReportSmEvent(&z_SmEventData);
}

/*
 ******************************************************************************
 * lm_mgp_send_fix_end_ind
 *
 * Function description:
 *
 * This function sends FIX_END_IND to MGP
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
void lm_mgp_send_fix_end_ind(void)
{
  mgp_SmEventData z_SmEventData;

  z_SmEventData.e_SmEvent = C_MGP_FIX_END_IND;

  if(z_lmControl.z_session_request.q_tbf >= lm_get_rc_on_threshold_period())
  {
    /*Unsubscribe 1Hz periodic fix reporting from PE*/
    if(!mgp_ConfigPeriodicFixAndAlignment(C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ, 0))
    {
      LM_MSG_ERROR("Failed to send the disable periodic fixes request to the PE.",0,0,0);
    }
  }

  /*Expect MGP going to IDLE*/
  z_lmControl.e_desiredMgpState = LM_MGP_STATE_IDLE;

  (void)mgp_ReportSmEvent(&z_SmEventData);    
}

/*
 ******************************************************************************
 * lm_oem_dre_data_on_off
 *
 * Function description:
 *   This function processes the (de)activation of OEM DRE data.
 *
 * Parameters:
 *   b_IsOn - TRUE to activate OEM DRE data reporting, FALSE to deactivate
 *
 * Return value:
 *   oemdre_statusType
 *
 ******************************************************************************
*/
static oemdre_statusType lm_oem_dre_data_on_off(boolean b_IsOn)
{
  oemdre_statusType oemdreStatusType = FEATURE_OEMDRE_NOT_SUPPORTED;
  tm_info_s_type tmInfo;

  /* Check for NV value for OEM DRE Data Enablement */
  if (NV_GNSS_OEM_FEATURE_MASK_1_OEMDRE == (z_lmControl.q_gnss_oem_feature & NV_GNSS_OEM_FEATURE_MASK_1_OEMDRE))
  {
    if(b_IsOn == z_lmControl.u_oemdre_onoff)
    {
      oemdreStatusType = 
      b_IsOn ? FEATURE_OEMDRE_ALREADY_ON : FEATURE_OEMDRE_ALREADY_OFF;
    }
    else
    {
      z_lmControl.u_oemdre_onoff = b_IsOn;
      mgp_OemDreSvPolyDataRequest(b_IsOn);
      oemdreStatusType = b_IsOn ? FEATURE_OEMDRE_ON : FEATURE_OEMDRE_OFF;
      
      if (z_lmControl.q_gnss_oem_feature & NV_GNSS_OEM_FEATURE_MASK_8_OEMDRE_TUNNEL_DETECT)
      {
        /* Send IPC msg to SM_TM requesting propagated fixes to be reported or not */
        memset((void*)&tmInfo, 0, sizeof(tmInfo));

        tmInfo.e_info_type = TM_INFO_OEMDRE_TD_CTRL;

        tmInfo.z_info.e_oemdre_td_ctrl = b_IsOn ? TM_OEMDRE_TD_CTRL_ON : TM_OEMDRE_TD_CTRL_OFF;

        (void) tm_info(&tmInfo);

        LM_MSG_HIGH("OEMDRETD Notify SM_TM: bit1=%d, bit4=%d", oemdreStatusType, tmInfo.z_info.e_oemdre_td_ctrl, 0);
      }
    }
  }

  LM_MSG_HIGH("lm_oem_dre_data_on_off - status:%d", oemdreStatusType, 0, 0);

  return oemdreStatusType;
}

/*
 ******************************************************************************
 * lm_mgp_fix_oem_data
 *
 * Function description:
 *   This function forwards Position Fix report to OEM
 *
 * Parameters:
 *   p_fixReport - Pointer to structure containing IPC message with position fix
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
static void lm_mgp_fix_oem_data(const sm_GnssFixRptStructType* p_fixReport)
{
  oemdre_data z_OemDreData;

  // Set type to position fix report from MGP
  z_OemDreData.oemdreDataType = OEMDRE_POS_FIX;

  // Pack it in the OEM DRE data struct
  memscpy(&(z_OemDreData.u.oemdrePosFix), sizeof(z_OemDreData.u.oemdrePosFix),
         p_fixReport, sizeof(*p_fixReport));

  // Send this to OEM DR function
  oemdreData(&z_OemDreData);
}

/*
 ******************************************************************************
 * lm_mgp_sv_poly_oem_data
 *
 * Function description:
 *   This function forwards SV Poly report to OEM
 *
 * Parameters:
 *   p_svPolyReport - Pointer to structure containing IPC message with Poly report
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
static void lm_mgp_sv_poly_oem_data(const gnss_SvPoly* p_svPolyReport)
{
  oemdre_data z_OemDreData;

  // Set type to position fix report from MGP
  z_OemDreData.oemdreDataType = OEMDRE_SVPOLY;

  // Pack it in the OEM DRE data struct
  memscpy(&(z_OemDreData.u.oemdreSvPoly), sizeof(z_OemDreData.u.oemdreSvPoly), p_svPolyReport, sizeof(*p_svPolyReport));

  // Send this to OEM DR function
  oemdreData(&z_OemDreData);
}

/*
 ******************************************************************************
 * lm_mgp_pos_meas_oem_data
 *
 * Function description:
 *   This function forwards Position Measurement report to OEM
 *
 * Parameters:
 *   p_measReport - Pointer to structure containing position meas rpt
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
static void lm_mgp_pos_meas_oem_data(const sm_GpsMeasRptStructType* p_measReport)
{
  oemdre_data z_OemDreData;

  // Set type to position fix report from MGP
  z_OemDreData.oemdreDataType = OEMDRE_MEAS;

  // Pack it in the OEM DRE data struct
  memscpy(&(z_OemDreData.u.oemdreMeas), sizeof(z_OemDreData.u.oemdreMeas), p_measReport, sizeof(*p_measReport));

  // Send this to OEM DR function
  oemdreData(&z_OemDreData);
}

/*
 ******************************************************************************
 * oemdreOn
 *
 * Function description:
 *   Actives OEM DRE data reporting.
 *
 * Parameters: 
 *   None
 *
 * Return value:
 *   FEATURE_OEMDRE_NOT_SUPPORTED
 *   FEATURE_OEMDRE_ALREADY_ON
 *   FEATURE_OEMDRE_ON
 *
 ******************************************************************************
*/
oemdre_statusType oemdreOn(void)
{
  return lm_oem_dre_data_on_off(TRUE);
} /*oemdreOn() ends*/

/*
 ******************************************************************************
 * oemdreOff
 *
 * Function description:
 *   Deactives OEM DRE data reporting.
 *
 * Parameters: 
 *   None
 *
 * Return value: 
 *   FEATURE_OEMDRE_NOT_SUPPORTED
 *   FEATURE_OEMDRE_ALREADY_OFF
 *   FEATURE_OEMDRE_OFF
 *
 ******************************************************************************
*/
oemdre_statusType oemdreOff(void)
{
  return lm_oem_dre_data_on_off(FALSE);
} /*oemdreOff() ends*/

/*===========================================================================

FUNCTION oemDrSyncSetConfig()

DESCRIPTION
  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean oemDrSyncSetConfig(gnss_DRSyncConfigType *pOemDrSyncConfigType)
{

  if(pOemDrSyncConfigType->e_Mode > GNSS_DRSYNC_TIMED_OUTPUT_ON_DEMAND)
  {
    LM_MSG_ERROR("e_Mode invalid: %d",pOemDrSyncConfigType->e_Mode,0,0);
    return FALSE;
  }

  if( (pOemDrSyncConfigType->e_Polarity != GNSS_DRSYNC_POLARITY_POSITIVE) && 
      (pOemDrSyncConfigType->e_Polarity != GNSS_DRSYNC_POLARITY_NEGATIVE))
  {
    LM_MSG_ERROR("e_Polarity invalid: %d",pOemDrSyncConfigType->e_Polarity,0,0);
    return FALSE;
  }

  // Pulse Phase 0 - 29 sec
  if( (pOemDrSyncConfigType->l_PhaseSec < 0) || 
      (pOemDrSyncConfigType->l_PhaseSec > 29))
  {
    LM_MSG_ERROR("l_PhaseSec invalid: %d",pOemDrSyncConfigType->l_PhaseSec,0,0);
    return FALSE;
  }

  // Pulse Period 1 - 30 sec
  if((pOemDrSyncConfigType->l_PeriodSec < 1) || 
     (pOemDrSyncConfigType->l_PeriodSec > 30))
  {
    LM_MSG_ERROR("l_PeriodSec invalid: %d",pOemDrSyncConfigType->l_PeriodSec,0,0);
    return FALSE;
  }

  // f_BiasNs (-10 ms to + 10 ms)
  if( (pOemDrSyncConfigType->f_BiasNs < -10000000) || 
      (pOemDrSyncConfigType->f_BiasNs > 10000000))
  {
    LM_MSG_ERROR("f_BiasNs invalid:",0,0,0);
    return FALSE;
  }

  // f_TuncStartThresholdMs max up to 1000 ms
  if(pOemDrSyncConfigType->f_TuncStartThresholdMs > 1000)
  {
    LM_MSG_ERROR("f_TuncStartThresholdMs invalid:",0,0,0);
    return FALSE;
  }

  // tunc for stop cannot be less than tunc for start
  if( (pOemDrSyncConfigType->f_TuncStopThresholdMs < 
       pOemDrSyncConfigType->f_TuncStartThresholdMs) || 
      (pOemDrSyncConfigType->f_TuncStopThresholdMs > 1000))
  {

    LM_MSG_ERROR("f_TuncStopThresholdMs invalid:",0,0,0);
    return FALSE;
  }

// ====================================================
// Checks below are for initial release of the feature
// ====================================================

  // b_TuncIgnore, always set to not ignore.
  pOemDrSyncConfigType->b_TuncIgnore = FALSE;

  // only supported modes are: 
  // GNSS_DRSYNC_TIMED_OUTPUT_OFF and 
  // GNSS_DRSYNC_TIMED_OUTPUT_WO_REARM
  if((pOemDrSyncConfigType->e_Mode != GNSS_DRSYNC_TIMED_OUTPUT_OFF) &&
     (pOemDrSyncConfigType->e_Mode != GNSS_DRSYNC_TIMED_OUTPUT_WO_REARM))
  {
    LM_MSG_ERROR("e_Mode not supported: %d",pOemDrSyncConfigType->e_Mode,0,0);
    return FALSE;
  }

  return mgp_SetDRSyncConfig(pOemDrSyncConfigType);
}

/*===========================================================================

FUNCTION oemDrSyncGetConfig()

DESCRIPTION
  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean oemDrSyncGetConfig(void)
{
  return mgp_GetDRSyncConfig();
}

/*===========================================================================

FUNCTION oemDrSyncArm()

DESCRIPTION
  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean oemDrSyncArm(void)
{
  return mgp_ArmDRSync();
}

/*
 ******************************************************************************
 * oemData_IsDataTypeOn
 *
 * Function description: Returns TRUE if this oem data type is turned on
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/
boolean oemData_IsDataTypeOn(oemData_dataType oemDataType)
{
  boolean bIsOn = FALSE;

  switch(oemDataType)
  {
  case OEMDATA_PPM_DATA:
  case OEMDATA_PRM_DATA:
  case OEMDATA_SENSTIVITY_ASSIST_DATA:
  case OEMDATA_ACQUISITION_ASSIST_DATA:
    if (z_lmControl.q_gnss_oem_feature & NV_GNSS_OEM_FEATURE_MASK_2_OEMDATA) // bit 0x2 controls these 4 data types
      bIsOn = TRUE;
    break;

  default:
    bIsOn = FALSE;
    break;
  }

  return bIsOn;
}

/*
 ******************************************************************************
 * lm_debugNMEA
 *
 * Function description:
 *
 * This function handles the debug NMEA strings from MGP
 *
 * Parameters: 
 *  os_IpcMsgType* p_msg
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_debugNMEA(os_IpcMsgType* p_msg)
{
  tm_info_s_type tmInfo;
  sm_DebugNMEA* p_debugNMEA = (sm_DebugNMEA*) p_msg->p_Data;

  if(NULL == p_debugNMEA)
  {
    LM_MSG_ERROR("NULL Debug NMEA",0,0,0);
    return;
  }
  if(MAX_NMEA_SENTENCE_LEN < p_debugNMEA->dwLength)
  {
    LM_MSG_ERROR("Invalid Debug NMEA Length: %d",p_debugNMEA->dwLength,0,0);
    return;   
  }
  memset((void*)&tmInfo, 0, sizeof(tmInfo));

  tmInfo.e_info_type = TM_NMEA_DATA;
  tmInfo.z_info.z_nmea_data.e_sentence_type = TM_NMEA_DEBUG;
  tmInfo.z_info.z_nmea_data.q_sentence_len  = p_debugNMEA->dwLength;

  memscpy((void*)&(tmInfo.z_info.z_nmea_data.p_nmea_sentence[0]),
         sizeof(tmInfo.z_info.z_nmea_data.p_nmea_sentence),
         (void*)&(p_debugNMEA->u_Buffer[0]),
         p_debugNMEA->dwLength);

  (void) tm_info(&tmInfo);
}

/*
 ******************************************************************************
 * lm_xtraInjDone
 *
 * Function description:
 *
 * This function handles IND from CD on completion of an Xtra injection
 *
 * Parameters: 
 *  None
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_xtraInjDone(void)
{
  tm_api_xtra_inj_done();

}

/*
 ******************************************************************************
 * lm_ReportResetLocationServiceDone
 *
 * Function description:
 *
 * This function handles IND from MGP on completion of Reset Location Service Request
 *
 * Parameters: 
 *  None
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void lm_ReportResetLocationServiceDone(pdsm_pd_ext_reset_location_service_status_e_type *status)
{
  tm_api_reset_location_service_done(*status);
}

