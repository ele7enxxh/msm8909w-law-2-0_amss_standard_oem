/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-UP-SUPL sub-module 

GENERAL DESCRIPTION
  This file implements TM UMTS-UP SUPL sub-module.
   
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_up/supl/src/tm_umts_up_supl_api.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/05/16   rk      Pass all measurements to protocols. It will decide which measurements should be
                     reported to server.
  04/28/16   rk      Added condition to not teardown the TM session when a AppTracking (using XTRA
                     data) session is running.
  04/18/16   skm     Fix for crash seein > 24 AD test case SUPL message queuing
  02/12/16   skm     Add queuing support to send >24AD SUPL POS
  12/15/15   skm     Process BDS PRM during SUPL sessions.
  10/01/15   rk      Use the renamed position protocol names.
  08/06/15   rk      Stop the OTDOA engine only if its running.
  04/07/15  mj      Move declaration of cell_db_type to the beginning of the function to fix SMART compile issue
  03/14/15   rh      Added support for MultiSIM NV
  03/09/15   rk      Update ME with new timeout value during fix session start when the receiver is already On.
  01/14/15   rk      Added LPP RSTD >24 measurement processing support
  12/19/14   skm     Updated NULL payload check to identity events without any payload
  05/22/12   ss       NI Geofencing with operation mode set to Standalone should not tag the session 
                            as SUPL session when starting the session
  03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
  12/23/13   mj  Remove FEATURE_DUAL_SIM and FEATURE_TRIPLE_SIM  
  02/27/13  ssh      OTA Delete to clear away existing NI Geofences
  01/23/14   skm     Support for UDP EPC_GROUP and removing Interface UP/DOWN event listeners
  12/06/13   ah      Support for non-deferrable timers
  07/11/13   mj      DSDS: Set the SET_ID choice to UNKNOWN every time. So that correct IMSI is read for SUPL
  06/03/13   mj      Allow NI initiated SUPL start during E911 for MPSS 2.0 and above
  05/16/13   ssh     When phone camps to 1x, close SUPL session only if a session is running
  05/14/13  ssh    Handle TCP connections in back to back SUPL sessions
  03/13/13   mj     Reject SUPL start session when E911/high priority session
  04/12/13   rk      Send OTDOA update timeout command to request OTDOA measurement without time alignment when no GPS
                     measurement is available.
  04/16/13   mj Retrieve IMSI every time for UMTS/TDS Phone event
  03/26/12   rk      Set the SUPL root cert directory upon boot-up unconditionally.
  09/07/12   mj     Added suport for handling external cell updates for SGLTE
  09/03/12   rh      Added NI SMS event handling
  08/1/12    ssu     Set the cached cell info of SUPL to invalid when the phone goes OOS
                     so that we dont try to use a stale cached cell-id when we come back online.
  12/13/11   gk      LPP related fixes
  03/03/11   gk      Timer race condition fix
  01/03/11   gk      Eph throttling rework
  10/23/09   rb      Added SUPL Cell ID feature
  07/13/09   gk      Process end session request by ignoring it
  01/06/09   LT      Cached position is now included in SUPL_POS_INIT.
  07/11/08   cl      Abort ongoing session when switching to 1x system
  05/12/08   LT      Ensured PDSM_PD_EVENT_END sent when no-fix.
  06/20/07   LT      Added handling of NV Item aagps_gps_lock_control.
  05/25/07   LT      Replaced a function with a TM common utility function.
  09/14/06   LT      Initial version

============================================================================*/
#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */

#ifdef FEATURE_CGPS_UMTS_UP_SUPL

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msg.h"
#include <limits.h>
#include "tm_common.h"
#include "tm_prtl_iface.h"
#include "pd_comms_api.h"


#include "tm_rrlp_asn1.h"


#include "tm_umts_common_utils.h"
#include "tm_umts_up_supl_comm.h"
#include "tm_umts_up_supl.h"  /* which includes "tm_supl_asn1.h" */
#include "sm_nv.h"
#include "tm_osysinit.h"

#include "tm_data.h"

#include "tm_cm_iface.h"

#if defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)
#include "tm_1x_up_is801_drv_iface.h"
#include "../../../1x_up/src/tm_1x_up_comm.h"
#endif /* FEATURE_CGPS_USES_CDMA && !FEATURE_GNSS_LOW_MEMORY */

#include "tm_lpp_up.h"
#include "tm_lpp.h"

#ifdef FEATURE_SUPL1_TDS /* defined in custcgps.h, if .builds defines FEATURE_CGPS_USES_TDS */
#include "tdsrrclsmif.h" 
#endif


#if !defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)
static void tm_1x_up_is801_drv_handle_pde_connected(boolean sess_start)
{
  return;
} 

boolean tm_1x_up_is801_drv_req_aiding_data
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)
{
  return TRUE;
}  

boolean tm_1x_up_is801_drv_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *sess_info_param
)
{
  return TRUE;
}
  
boolean tm_1x_up_handle_comm_write_ack(void *msg_data_ptr)
{
  return TRUE;
}

boolean tm_1x_up_handle_comm_ioctl_ack(void *msg_data_ptr)
{
  return TRUE;
}  

#endif  /* !FEATURE_CGPS_USES_CDMA && !FEATURE_GNSS_LOW_MEMORY  Dummies: for UMTS-only builds */



/*===========================================================================

FUNCTION supl_is_waiting_for_tcp_to_send_breach

DESCRIPTION
  This function is to check whether SUPL was waiting for a TCP connection to send Breach Report
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static boolean supl_is_waiting_for_tcp_to_send_breach(void)
{
  uint8  u_i;

  for ( u_i = 0; u_i < C_MAX_GEOFENCE_NUM; u_i++)
  {
    if ( z_supl_area_event_session_status[u_i].u_session_active )
    {
      
      if ( z_supl_area_event_session_status[u_i].e_cf_state &
           C_SUPL_CF_STATE_UT0_SUPLREPORT_GF_BREACH )
        {
          MSG_HIGH("Breach Report Pending",0,0,0);
          return TRUE;
        }
      }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION supl_close_session

DESCRIPTION
  This function calls a collection of functions to end the session.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

static void supl_close_session(tm_sess_stop_reason_e_type  stop_reason )
{

  /* tell TmCore to stop session */
  tm_umts_up_supl_inform_core_stop_session(stop_reason);

  /* close data link if connected or connecting */
  if ( ( stop_reason == TM_STOP_REASON_SERVING_SYSTEM_CHANGE ) ||  
       !(z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND)
     )
  {
    if (supl_is_waiting_for_tcp_to_send_breach() == FALSE)
    {
      /* if not waiting for the connection to be up in order to send SUPL_END;
         otherwise, tear down connection after sending SUPL_END */
      tm_umts_up_supl_comm_close_connection();

      /* cancel all SUPL timer(s) and call flow states */
      tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_NULL);

      /* clear all protocol session related variables */
      tm_umts_up_supl_session_init(FALSE);
    }

  }

}

#ifdef FEATURE_RET_REFPOS_UPON_NOFIX
#if defined ( FEATURE_POPULATE_NMEA_WITH_CELL_ID_POS ) || defined (FEATURE_GEOFENCE)
/*
 ******************************************************************************
 *
 * supl_convert_gad_for_tm_core_post
 *
 * Function description:
 *  This function converts position GAD shape 9 position to a format suitable for
 *  posting to TmCore as type LR ( a different type is used so that TM core 
 *  does not inject this into MGP ).                    
 *  
 * Parameters:
 *   p_gad : pointer to buffer holding GAD shaped position.
 *   p_ref_loc : pointer of structre for seed location that
 *               is to be populated by this function.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

static void supl_convert_gad9_for_tm_core_post
(const gad_ellip_alt_unc_ellip_type *p_gad_pos, gps_RefLocStructType *p_ref_loc )
{

  uint8 u_temp;
  float f_alt_unc_meter;
  uint32  q_temp = 0;
  double  d_temp;
  uint8 u_MajorUncK, u_MinorUncK, u_UncAngle; 
  float f_major_unc_meters, f_minor_unc_meters;


  /*
   *  Initialize the return structure to 0.  This sets all validity
   *  flags to FALSE and all integer and FLT/DBL numbers to 0.
   */
  memset(p_ref_loc, 0, sizeof( *p_ref_loc ) );

  /*
   *  Initialize uncertainty to MAX values "just in case"
   */
  p_ref_loc->f_LocUncrtntySemiMajor = MAX_UNCERTAINTY;
  p_ref_loc->f_LocUncrtntySemiMinor = MAX_UNCERTAINTY;
  p_ref_loc->f_AltitudeUnc          = MAX_UNCERTAINTY;

       
      
  /* LATITUDE:  construct a 24-bit GAD shape Latitude with Bit 23 being the sign bit */
  q_temp = (uint32)p_gad_pos->u_Lat0 << 16;
  q_temp |= (uint32)p_gad_pos->u_Lat1 << 8;
  q_temp |= (uint32)p_gad_pos->u_Lat2;

  /* PDAPI lat. LSB is ( 180 / 2^25 ) degrees, and GAD shape lat. LSB
     is (90 / 2^23 ), so, GadShapeLat * (90/2^23 ) / (180 / 2^25 ) = 
     GadShapeLat * 2 
  */
  if ( q_temp & 0x00800000 )
  {
    d_temp = (double) ((int32) (q_temp & 0x7FFFFFFF) * -1);

  }
  else
  {
    d_temp = (DBL) q_temp;
  }

  p_ref_loc->d_Latitude = d_temp * LAT_IS801_SCALING * DEG_TO_RADIANS;


  /* LONGITUDE: construct a 32-bit 2's complement ************************/
  q_temp = (uint32)p_gad_pos->u_Long0 << 16;
  q_temp |= (uint32)p_gad_pos->u_Long1 << 8;
  q_temp |= (uint32)p_gad_pos->u_Long2;
  if (q_temp & 0x00800000)
  {
    q_temp |=  0xFF800000; /* sign extend */
  }
  d_temp = (DBL) ((int32) q_temp);
  p_ref_loc->d_Longitude = d_temp * LON_IS801_SCALING * DEG_TO_RADIANS;


  /* altitude */
  p_ref_loc->u_AltitudeIsValid = TRUE;
  q_temp = (uint32)p_gad_pos->u_Alt0 << 8;
  q_temp |= (uint32)p_gad_pos->u_Alt1;
  if ( q_temp & 0x00008000 )
  {
    p_ref_loc->f_Altitude = -(float)(q_temp & 0x7FFF);

  }
  else
  {
    p_ref_loc->f_Altitude = (float)(q_temp & 0x7FFF);

  }

  /************************** altitude unc *********************************/
  u_temp = p_gad_pos->u_UncAlt & 0x7F;
  f_alt_unc_meter =  tm_umts_common_utils_GADK_to_meter(FALSE, u_temp);
  p_ref_loc->f_AltitudeUnc = f_alt_unc_meter;

  /************************* horizontal unc ********************************/

  /* we get major axis unc, minor axis unc, and angle from North
   * of major axis (0 to 178 degrees). */
  u_MajorUncK = p_gad_pos->u_UncMajor & 0x7F; /* Major axis unc */
  u_MinorUncK = p_gad_pos->u_UncMinor & 0x7F; /* Minor axis unc */

  /* Angle from North of Major axis: 
   *    2N < angle < 2(N+1) 
   * where N = 0..89
   */
  u_UncAngle  = p_gad_pos->u_OrientMajor << 1 ; 
  f_major_unc_meters = tm_umts_common_utils_GADK_to_meter( TRUE, u_MajorUncK );
  f_minor_unc_meters = tm_umts_common_utils_GADK_to_meter( TRUE, u_MinorUncK );
  p_ref_loc->f_LocUncrtntySemiMajor = f_major_unc_meters;
  p_ref_loc->f_LocUncrtntySemiMinor = f_minor_unc_meters;

  /* The angle is defined as follows in IS801.
   * LOC_UNCRTNTY_ANG – Angle of axis with respect to True North for 
   * position uncertainty. The mobile station shall set this field to 
   * the angle of axis for position uncertainty, in units of 5.625 
   * degrees, in the range from 0 to 84.375 degrees, where 0 degrees 
   * is True North and the angle increases toward the East.  
   *
   * The coded value of the range is hence from 0 to 15.  We'll 
   * interpret this as:
   *
   *    N * 5.625 <= angle < (N+1) * 5.625
   *
   * where N is the coded value, since this would be the most logical way 
   * of dividing up the coded space EVENLY.  This is therefore just a 
   * simple truncation.
   */
  p_ref_loc->f_LocUncrtntyAng = (float)u_UncAngle;

}
#endif /* FEATURE_POPULATE_NMEA_WITH_CELL_ID_POS or Geofence*/
#endif /* FEATURE_RET_REFPOS_UPON_NOFIX */


#ifdef FEATURE_RET_REFPOS_UPON_NOFIX
/*===========================================================================

FUNCTION supl_report_ref_loc_to_pdapi_if_avail

DESCRIPTION
  This function reports the reference position provided by the network, if
  available, as the final position to TmCore.
  
DEPENDENCIES: none.

RETURN VALUE: TRUE - reference position posted; FALSE - no reference 
              position posted.

SIDE EFFECTS: none.

===========================================================================*/

static boolean supl_report_ref_loc_to_pdapi_if_avail(void)
{

  uint32                     q_time_stamp_sec = 0;
  pdsm_pd_event_type         q_pd_event; /* pdsm_pd_event_type is uint32 */
  gnss_NavSlnFlagsStructType z_pos_flags;

  memset(&z_pos_flags, 0x00, sizeof(z_pos_flags));

  #if defined ( FEATURE_POPULATE_NMEA_WITH_CELL_ID_POS ) || defined (FEATURE_GEOFENCE)
  tm_post_data_payload_type   z_tm_payload;
  #endif

  if ( !( ( z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED ) &&
            z_last_refloc_info.z_RefPos_GAD.u_pos_valid
        )
     )
  {
    return FALSE;
  }

  if ( z_last_refloc_info.z_RefPos_GAD.u_timestamp_valid )
  {

    q_time_stamp_sec = 
      (uint32)( tm_umts_common_utils_full_gps_ms
                ( z_last_refloc_info.z_RefPos_GAD.w_week,
                  z_last_refloc_info.z_RefPos_GAD.q_tow_ms
                )  * (double)0.001
              );
  }

  tm_umts_common_utils_gad_to_pdsm_pd_info 
  ( z_supl_session_status.q_pdapi_client_id,
    (uint8 *)&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip, 
    PDSM_PD_POSITION_TYPE_NEW,
    PDSM_PD_POSITION_MODE_UNKNOWN,
    PDSM_PD_POSITION_SOURCE_DEFAULT,
    q_time_stamp_sec,
    &z_supl_pd_info_buffer,
    &q_pd_event
  );

  if ( q_pd_event & PDSM_PD_EVENT_HEIGHT )
  {
    z_pos_flags.b_Is3D = TRUE;
  }

  tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                     z_supl_session_status.tm_active_session_handle,
                     q_pd_event, 
                     &z_supl_pd_info_buffer,
                     &z_pos_flags);

  /***************** post for the purpose of NMEA *******************/
  #if defined ( FEATURE_POPULATE_NMEA_WITH_CELL_ID_POS ) || defined (FEATURE_GEOFENCE)
  
  supl_convert_gad9_for_tm_core_post 
  ( &z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip, 
    &z_tm_payload.seed_data );

  if ( z_last_refloc_info.z_RefPos_GAD.u_timestamp_valid )
  {
    z_tm_payload.seed_data.w_GpsWeek = z_last_refloc_info.z_RefPos_GAD.w_week; 
    z_tm_payload.seed_data.q_GpsMsec = z_last_refloc_info.z_RefPos_GAD.q_tow_ms;
  }
  else
  {
    z_tm_payload.seed_data.w_GpsWeek = C_GPS_WEEK_UNKNOWN;
    z_tm_payload.seed_data.q_GpsMsec = 0;
  }
  tm_post_data( TM_PRTL_TYPE_UMTS_UP, 
                z_supl_session_status.tm_active_session_handle,
                TM_POST_DATA_TYPE_REFPOS_RPT,
                &z_tm_payload );
  #endif
  
  /* once used, clear it */
  memset(&z_last_refloc_info.z_RefPos_GAD, 0, sizeof(last_refloc_info) );
  
  return TRUE;

}
#endif /* FEATURE_RET_REFPOS_UPON_NOFIX */

/*===========================================================================

FUNCTION tm_umts_up_supl_start_sess_req_handler

DESCRIPTION
  This function handles the start session request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: prtl_start_sess_req_result_e_type.

SIDE EFFECTS: none.

===========================================================================*/


prtl_start_sess_req_result_e_type tm_umts_up_supl_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions
)
{
  supl_status_s_type                 *p_supl_session_status = &z_supl_session_status;
  prtl_start_sess_req_result_e_type   ret_val               = PRTL_START_SESS_REQ_ALLOWED;
  emergency_handling_e_type e_e911_effect;

  #ifdef FEATURE_USER_PLANE_MRL
  tm_cm_phone_state_info_s_type       phone_state_info;
  #endif
  cell_db_type cell_db = CELL_DB_NONE;

  if( NULL == start_sess_req_param ||
      NULL == actions )
  {
    MSG_ERROR("TM_UMTS_SUPL: INVALID PARAMETER",0,0,0);
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }  

  /*disallow if SUPL is not Enabled*/
  if(tm_umts_up_is_supl_enabled() == FALSE)
  {
    MSG_MED("TM_UMTS_SUPL: SUPL NOT ENABLED",0,0,0);
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }
  
  p_supl_session_status->tm_active_session_handle = sess_handle;

  MSG_MED("SUPL: start_sess_req_handler. PosMethodType %d", p_supl_session_status->u_agps_mode, 0, 0);

  /* disallow if NV item has LBS locked */
  if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI ) 
  {
    if ( ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_MI ) ||
         ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_ALL )
       )
    {
      return PRTL_START_SESS_REQ_NOT_ALLOWED;
    }
  }

  e_e911_effect = tm_umts_common_utils_emergency_effect();
  if ( ( e_e911_effect == C_EMERGENCY_HANDLING_REJ_ALL ) ||
       (( e_e911_effect == C_EMERGENCY_HANDLING_ACCEPT_ONLY_HIGH_PRIORITY ) &&
        ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI))
     )
  {
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }
  
  if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI )
  {
    if ( (p_supl_session_status->z_emergency_data.b_valid == TRUE) &&
         (p_supl_session_status->z_emergency_data.b_emergency_ni = TRUE) )
    {
      MSG_MED("TM_UMTS_SUPL: Emergency SUPL INIT: Allow the session",0,0,0);
    }
    else if ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_MT )  
    {
      return PRTL_START_SESS_REQ_NOT_ALLOWED;
    }
  }
  /* disallow for UE initiated case, if external modem is present and it's in roaming */
  else if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI)
  {
    cell_db = tm_umts_up_supl_get_cell_db_type();

  if ((cell_db == CELL_DB_EXTERNAL_MODEM) && 
    (z_external_cell_info_cached.serviceStatus == STATUS_CAMPED_ROAMING))
  {
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }
  } 

  if ( tm_umts_up_supl_pdapi_mapping (start_sess_req_param) )
  {

    /*Save session req param, we will use this if we have to start IS801 state machine*/
    p_supl_session_status->tm_sess_req_param = *start_sess_req_param; 

    /* this start request could be in response to a NI request of privacy-override or
       no-notif-no-verif */
    if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NONE )
    {
      tm_umts_up_supl_callflow_kind_set( C_SUPL_SESSION_UI );

      tm_cm_iface_get_phone_state(&phone_state_info);   
      /* Can't run SUPL over LTE if SUPL major version less than 2, reject TM session here for MSA/OTDOA;
         MSB/Standalone will allow a TM session here and SUPL will move MSB to fallback. */
      if ( (phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) &&
           ( (p_supl_session_status->q_supl_version & 0xFF) < 2 ) &&
           ( ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED ) ||
             ( p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE )
           )
         )
      {
        MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "LTE not supported on SUPL 1.0, disallow start_req: %u,%lx", 
              phone_state_info.srv_system, p_supl_session_status->q_supl_version );

        tm_umts_up_supl_session_init(FALSE); /* reset all SUPL session related variables */
        return PRTL_START_SESS_REQ_NOT_ALLOWED;
      }

      /*Select the positioning protocol to use*/
      if ( (p_supl_session_status->u_position_protocol = 
            tm_umts_up_supl_select_pos_prot()) == TM_POS_PRTL_NONE)
      {
        MSG_ERROR("SUPL: serving system undef", 0, 0, 0);
        return PRTL_START_SESS_REQ_NOT_ALLOWED; 
      }

      if((p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801) &&
           (p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_CELL_ID)
           /*IS801 module need not be contacted for CELL_ID fix*/
          )
      {        
        supl_set_cf_state(C_SUPL_CF_STATE_UT0_SUPLSTART, 0);
        /*Pass this UI request to IS801 state machine*/
        if(supl_start_is801_session(FALSE, actions) == TRUE)
        {         
          return PRTL_START_SESS_REQ_ALLOWED;
        }
        else
        {
          MSG_MED("IS801 did not allow the UI session",0,0,0);
          return PRTL_START_SESS_REQ_NOT_ALLOWED; 
        }
      }
    }
    
    actions->multi_report_msa = FALSE; /* SUPL does not support MT multi-reporting */

    if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
    {
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP ||
         p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
      {
        actions->op_req = TM_OP_REQ_AIDING_DATA_STATUS | TM_OP_REQ_LR;
        actions->lr_qos = TM_MAX_SESSION_TIMEOUT;
      }         
      else if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
      {
        actions->op_req = TM_OP_REQ_NONE;
      }
      else
      {
        MSG_ERROR("Pos protocol not supported",0,0,0);
      }
      actions->accuracy_threshold = p_supl_session_status->z_session_qos.q_hor_acc_meters;
      actions->dynamic_qos_enabled = TRUE;

      actions->op_mode = TM_SESS_OPERATION_MODE_MSB;
      actions->num_fixes = start_sess_req_param->pd_option.fix_rate.num_fixes;
      actions->tbf_ms = start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;

      /*Contact SLP if on 1x else wait for Assistance data*/
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
      {
        /* Initializing the flags so that we can attempt a fresh connection*/
        z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
        z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;        
        tm_umts_up_supl_start_slp_contact();
      }
    }
    else if ( (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED) ||
             (p_supl_session_status->u_agps_mode == C_SUPL_AFLT_MODE) ||
              (p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE)
            )
    {
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP ||
         p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
      {
        if ( p_supl_session_status->u_agps_mode != C_SUPL_OTDOA_MODE )
        {
          actions->op_req = TM_OP_REQ_PRM; 
          actions->prm_qos = TM_MAX_SESSION_TIMEOUT; /* pdapi's max: 255  */          
          actions->op_mode = TM_SESS_OPERATION_MODE_MSA;
        }
        else
        {          
          actions->op_req = TM_OP_REQ_NONE;           
          actions->op_mode = TM_SESS_OPERATION_MODE_OTDOA;
        }
      }
      else if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
      {
        if (p_supl_session_status->tm_sess_req_param.pd_qos.gps_session_timeout == 0)
        {
           /*For UI AFLT wil be triggered with ASSISTED mode and gps_timeout of zero*/
          p_supl_session_status->u_agps_mode = C_SUPL_AFLT_MODE;
        }
        actions->op_req = TM_OP_REQ_NONE;        
        actions->op_mode = TM_SESS_OPERATION_MODE_MSA;
      }
      else
      {
        MSG_ERROR("Pos protocol not supported",0,0,0);
      }
      actions->accuracy_threshold = p_supl_session_status->z_session_qos.q_hor_acc_meters;
      
      actions->num_fixes = start_sess_req_param->pd_option.fix_rate.num_fixes;
      actions->tbf_ms = start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;

      /* the wishlist setting is not absolutely needed as it will be set like that when forming 
         SuplPosInit */
      p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_REFTIME;
      p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_ACQ;

      /* check if cached cell ID is valid and not stale: if not, request new
         cell ID info. from L1 */
      if ( tm_umts_supl_if_need_forced_wcmda_cell_inq() == FALSE )
      {
        /* Either not on WCDMA, or on WCDMA and no need to directly
           request at this point */ 
        
        /* for non-WCDMA, check if TDSCDMA. If so do a request */
        tm_umts_up_supl_ck_req_tdscmda_cell_info();
         

        /* Initializing the flags so that we can attempt a fresh 
           connection*/
        z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
        z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
        tm_umts_up_supl_start_slp_contact();
      }
      else
      {
        if ( !tm_umts_up_supl_req_cell_info_from_rrc(C_SUPL_CF_STATE_CELL_INFO_REQ) )
        {
         /* Initializing the flags so that we can attempt a fresh 
            connection*/
         z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
         z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
          /* effort to update cell info. fails, go ahead anyway hoping new 
             update may come before it's ready to send the message */
          tm_umts_up_supl_start_slp_contact(); 
        } 
      }
    }

    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_AUTO )
    {
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP ||
         p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
      {
        actions->op_req = TM_OP_REQ_LR;
      }
      else if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
      {
        actions->op_req = TM_OP_REQ_NONE;
      }
      else
      {
        MSG_ERROR("Pos protocol not supported",0,0,0);
      }
      actions->accuracy_threshold = p_supl_session_status->z_session_qos.q_hor_acc_meters;

      actions->op_mode = TM_SESS_OPERATION_MODE_MSB;
      actions->num_fixes = start_sess_req_param->pd_option.fix_rate.num_fixes;
      actions->tbf_ms = start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;
      actions->dynamic_qos_enabled = TRUE;

      if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI )
      {
        actions->lr_qos = TM_MAX_SESSION_TIMEOUT; /* pdapi's max: 255  */

        /* check if cached cell ID is valid and not stale: if not, request new
           cell ID info. from L1 */
        if ( tm_umts_supl_if_need_forced_wcmda_cell_inq()== FALSE )
        {

          /* Either not on WCDMA, or on WCDMA and no need to directly
             request at this point */ 

          /* for non-WCDMA, check if TDSCDMA. If so do a request */
          tm_umts_up_supl_ck_req_tdscmda_cell_info();

          /* Initializing the flags so that we can attempt a fresh 
             connection*/
          z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
          tm_umts_up_supl_start_slp_contact();
        }
        else
        {
          if ( !tm_umts_up_supl_req_cell_info_from_rrc(C_SUPL_CF_STATE_CELL_INFO_REQ) )
          {
            /* Initializing the flags so that we can attempt a fresh 
               connection*/
            z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
            z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
            /* effort to update cell info. fails, go ahead anyway hoping new 
               update may come before it's ready to send the message */
            tm_umts_up_supl_start_slp_contact(); 
          } 
        }
      }
      else
      {
        actions->lr_qos = p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs;
      }

    }
    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID )
    {
       actions->op_req = TM_OP_REQ_NONE; 
       actions->prm_qos = p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs; /* pdapi's max: 255  */
       actions->accuracy_threshold = p_supl_session_status->z_session_qos.q_hor_acc_meters;
       actions->op_mode = TM_SESS_OPERATION_MODE_NONE;
       actions->num_fixes = start_sess_req_param->pd_option.fix_rate.num_fixes;
       actions->tbf_ms = start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;

       /* check if cached cell ID is valid and not stale: if not, request new
       cell ID info. from L1 */
       if ( tm_umts_supl_if_need_forced_wcmda_cell_inq()== FALSE )
       {
         /* Either not on WCDMA, or on WCDMA and no need to directly
            request at this point */ 

         /* for non-WCDMA, check if TDSCDMA. If so do a request */
         tm_umts_up_supl_ck_req_tdscmda_cell_info();

          /* Initializing the flags so that we can attempt a fresh 
          connection*/
          z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
          tm_umts_up_supl_start_slp_contact();
       }
       else
       {
          if ( !tm_umts_up_supl_req_cell_info_from_rrc(C_SUPL_CF_STATE_CELL_INFO_REQ) )
          {
             /* Initializing the flags so that we can attempt a fresh 
             connection*/
             z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
             z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
             /* effort to update cell info. fails, go ahead anyway hoping new 
             update may come before it's ready to send the message */
             tm_umts_up_supl_start_slp_contact(); 
          } 
       }
    }

    else
    {
      ret_val = PRTL_START_SESS_REQ_NOT_ALLOWED;
    }

  }

  else
  {
    /* if the above function returns failure, SUPL has no valid mode for the pdapi
       request, reject the request */
    ret_val = PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  if ( ret_val == PRTL_START_SESS_REQ_ALLOWED )
  {
    p_supl_session_status->tm_core_session_started = TRUE;

    #ifdef FEATURE_USER_PLANE_MRL
    #ifdef FEATURE_CGPS_WCDMA
    /* Query TM-CM module to get current acquired system */
    tm_cm_iface_get_phone_state(&phone_state_info);   
    if ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA )
    {
      tm_umts_up_supl_send_wcdma_cell_meas_req();
      (void) os_TimerStart(l1_mrl_req_timer, 1000, NULL); 
    }
    #endif /* FEATURE_CGPS_WCDMA */
    #ifdef FEATURE_CGPS_LTE_CELLDB
    if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) 
    {
      lte_ml1_request_gps_mrl();
      (void) os_TimerStart(l1_mrl_req_timer, 1000, NULL); 
    }
    #endif 
    
    #endif  /* MRL */
  }
  else
  {
    MSG_HIGH("SUPL: TmCore startSession denied", 0, 0, 0);
  }

  return ret_val;

}


/*===========================================================================

FUNCTION tm_umts_up_supl_stop_sess_req_handler

DESCRIPTION
  This function handles the stop session request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_up_supl_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
)
{
  boolean                    u_ret_val = FALSE;
  pdsm_pd_end_e_type         err_type = PDSM_PD_END_UNKNWN_SYS_ERROR;
  supl_status_s_type        *p_supl_session_status = &z_supl_session_status;
  gnss_NavSlnFlagsStructType z_pos_flags;

  #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  uint32 flags = 0;
  tm_lpp_transaction_info_s_type *p_lpp_tx_info_type  = NULL;
  #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */

  memset(&z_pos_flags, 0x00, sizeof(z_pos_flags));

  if (sess_handle == TM_SESS_HANDLE_NONE)
  {
    u_ret_val = TRUE;
    return u_ret_val;
  }

  if ( sess_handle == p_supl_session_status->tm_active_session_handle )
  {

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
    else if ( ( stop_reason == TM_STOP_REASON_FIX_TIMEOUT ) || 
              ( stop_reason == TM_STOP_REASON_PRM_TIMEOUT )
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
    err_type = PDSM_PD_END_TIMEOUT;
      u_ret_val = TRUE; 
    }
    else if ( ( stop_reason == TM_STOP_REASON_USER_TERMINATED ) ||
              ( stop_reason == TM_STOP_REASON_ON_RESET_LOCATION_SERVICE) )
    {
      if ( stop_reason == TM_STOP_REASON_USER_TERMINATED )
      {
        err_type = PDSM_PD_END_CLIENT_END;
        u_ret_val = TRUE;
      }
      else if ( stop_reason == TM_STOP_REASON_ON_RESET_LOCATION_SERVICE)
      {
        err_type = PDSM_PD_END_RESET_LOCATION_SERVICE;
        u_ret_val = TRUE;
      }
      
      /*** if TmCore actively ends a session while SUPL protocol module
           is waiting for for asssitance data, cancel all the assistance 
           data holdoff timers */
      
      if ((p_supl_session_status->e_cf_state == C_SUPL_CF_STATE_UT0_SUPLSTART)  ||
          (p_supl_session_status->e_cf_state == C_SUPL_CF_STATE_UT0_SUPLPOSINIT) ||
          (p_supl_session_status->e_cf_state == C_SUPL_CF_STATE_UT1) ||
          (p_supl_session_status->e_cf_state == C_SUPL_CF_STATE_UT2))
      {
        MSG_HIGH ("EndSession: Clear assist req. holdoff times", 0, 0, 0);
        (void) os_TimerStop(supl_alm_req_holdoff_timer);
        (void) os_TimerStop(supl_eph_req_holdoff_timer);
        (void) os_TimerStop(supl_iono_req_holdoff_timer);
        (void) os_TimerStop(supl_utc_req_holdoff_timer);
        (void) os_TimerStop(supl_refloc_req_holdoff_timer);
        (void) os_TimerStop(supl_glo_alm_req_holdoff_timer);
        (void) os_TimerStop(supl_glo_eph_req_holdoff_timer);
      }
      
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

      /*If SUPL session was waiting for TCP connection bringip due to previous TCP connection,
            Mark the connection pending as FALSE and let the SUPL_END go over the TCP connection
            which is brought up
          */
       p_supl_session_status->u_tcp_connection_attempt_pending = FALSE;
      
      if ( ( err_type == PDSM_PD_END_CLIENT_END ) ||
           ( err_type == PDSM_PD_END_RESET_LOCATION_SERVICE ) )
      { 
        if ( ( z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLPOSINIT )
           )
        {
          p_supl_session_status->e_cf_state &= (supl_cf_state_enum_type)~C_SUPL_CF_STATE_UT0_SUPLPOSINIT; /* no need to send SUPL_POS_INIT anymore */
          (void) tm_umts_up_supl_supl_end_prepare_or_tx(&p_supl_session_status->z_session_id.z_Slp_Sess_Id, 
                                                 NULL,
                                                 supl_StatusCode_unspecified, 
                                                 NULL,
                                                 p_supl_session_status->q_supl_version);
        }
        supl_close_session(stop_reason);
      }

      else if ( err_type == PDSM_PD_END_TIMEOUT )
      {
        MSG_MED("Supl CF state when Timeout %d", p_supl_session_status->e_cf_state, 0,0);
        if ( p_supl_session_status->e_cf_state & (C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER | C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV))
        {
          
          if ( p_supl_session_status->u_position_protocol ==  TM_POS_PRTL_RRLP )
          {
            /* if still in the state waiting to respond to the network, early termination is needed */
            if ( ! tm_umts_up_supl_suplpos_rrlp_mpr_err_tx(rrlp_notEnoughSats) )
            {
              /* if sending of this message fails, move on the stop the session;
                 otherwise give the SLP a chance to close down everything gracefully; */
              supl_close_session(stop_reason);
            } 
            else
            {
              /*Mark the fact that we are waiting for SUPL close*/
              p_supl_session_status->supl_close_waiting = TRUE;
            }
          }

          #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
          else if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
          {
            if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
            {
              tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER | C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV);
              /* after the PRM is received, get the OTDOA measurements */

              p_supl_session_status->z_supl_lpp_data.q_lpp_flags &= ~(TM_SUPL_LPP_PRM_VALID) ;
              p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR;
              p_supl_session_status->z_supl_lpp_data.u_lpp_error_code  = (uint8)LPP_AGNSS_CAUSE_NOTENOUGHSAT;

              if ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA) &&
                   (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED) )
              {
                supl_set_cf_state(C_SUPL_CF_STATE_OTDOA_MEAS_REQ, 0);
                if ( p_supl_session_status->z_gnss_meas_report.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GPS )
                {
                sm_loc_lte_otdoa_get_measurements(
                  &p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock, TM_PRTL_TYPE_UMTS_UP,0);
              }
              else
              {
                  sm_loc_lte_otdoa_get_measurements(NULL, TM_PRTL_TYPE_UMTS_UP,0);
                }
              }
              else
              {
                if ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA))
                {
                  p_supl_session_status->z_supl_lpp_data.u_lpp_error_type |= (uint8)LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR;
                  p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_OTDOA_CAUSE_AD_MISSING;

                  flags = C_LPP_AGPS_MODE_UE_ASSISTED;
                  if  ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD) &&
                       (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA))
                  {
                    flags |= C_LPP_MODE_UE_OTDOA;
                  }
                  if (supl_enc_lpp_prov_loc(p_supl_session_status, flags) == TRUE)
                  {
                    if (!supl_SuplPos_lpp_tx(p_supl_session_status, NULL))
                    {
                      supl_close_session(stop_reason);
                    }
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;
                  }

                  else
                  {
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_LPP_ERROR;
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_ERROR_UNDEF;
                    if (supl_pos_enc_lpp_error_tx(NULL, p_supl_session_status, &p_lpp_tx_info_type) == FALSE)
                    {
                      MSG_ERROR("TM_LPP_UP: Error encoding LPP Error message",0,0,0);
                    }
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;

                  }
                }
                else
                {
                  if ( (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED) &&
                    (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD))
                  {

                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;
                    p_supl_session_status->z_supl_lpp_data.q_lpp_flags &= ~(TM_SUPL_LPP_PRM_VALID) ;
                    supl_set_cf_state(C_SUPL_CF_STATE_OTDOA_MEAS_REQ, 0);
                    sm_loc_lte_otdoa_get_measurements(NULL, TM_PRTL_TYPE_UMTS_UP,0);
                  }
                }
              }             
            }
            else
            {
              tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER | C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV);
              p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR;
              p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_AGNSS_CAUSE_NOTENOUGHSAT;
              flags = C_LPP_AGPS_MODE_UE_BASED;
              if (supl_enc_lpp_prov_loc(p_supl_session_status, flags) == TRUE)
              {
                if (!supl_SuplPos_lpp_tx(p_supl_session_status, NULL))
                {
                  supl_close_session(stop_reason);
                }
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;

              }

              else
              {
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_LPP_ERROR;
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_ERROR_UNDEF;
                if (supl_pos_enc_lpp_error_tx(NULL, p_supl_session_status, &p_lpp_tx_info_type) == FALSE)
                {
                  MSG_ERROR("TM_LPP_UP: Error encoding LPP Error message",0,0,0);
                }
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;

              }
            }
            p_supl_session_status->z_supl_lpp_data.b_tm_session_done = TRUE;
          }
          #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
        } /* has been in the state of waiting to respond to SLP with RRLP */
        else if ( C_SUPL_CF_STATE_OTDOA_MEAS_REQ == p_supl_session_status->e_cf_state )
        {
          /* Don't close the session when OTDOA measurements are pending */
          MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "No-op for LM PRM/LR timer expiry in SUPL OTDOA req state" );
          u_ret_val = FALSE;
        }
        else if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3 )
        {
          MSG_MED("Supl CF state when LM Timeout %d. Wait for UT3 to expire before session closes", 
            p_supl_session_status->e_cf_state, 0,0);
        }
        else  
        {
          #ifdef FEATURE_STALE_POS_ALLOWED 
          if ( p_supl_session_status->pd_info_buffer_valid )  
          {

            pdsm_pd_info_s_type*  p_pd_info_buffer = &z_supl_pd_info_buffer;
            pdsm_pd_event_type    pd_event = PDSM_PD_EVENT_POSITION;

            /* there has been a network provided position that did not satisfy qoP */

            p_pd_info_buffer->pd_info.pd_data.position_mode = PDSM_PD_POSITION_MODE_UNKNOWN;

            /* SUPL_END does not provide source info. */
            p_pd_info_buffer->pd_info.pd_data.positioning_source = PDSM_PD_POSITION_SOURCE_GPS | PDSM_PD_POSITION_SOURCE_GLO;

            /* set up proper position type in the pdapi report */
            p_pd_info_buffer->pd_info.pd_data.position_type = PDSM_PD_POSITION_TYPE_NEW;

            if ( p_pd_info_buffer->pd_info.pd_data.opt_field_mask & PDSM_PD_ALTITUDE_VALID )
            {
              pd_event |= PDSM_PD_EVENT_HEIGHT;
              z_pos_flags.b_Is3D = TRUE;
            }
            if ( p_pd_info_buffer->pd_info.pd_data.opt_field_mask & PDSM_PD_VELOCITY_VALID )
            {                     
              pd_event |= PDSM_PD_EVENT_VELOCITY;
              z_pos_flags.b_IsVelValid = TRUE;
            }

            p_pd_info_buffer->client_id = p_supl_session_status->q_pdapi_client_id;

            tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                               p_supl_session_status->tm_active_session_handle,
                               pd_event, 
                               &z_supl_pd_info_buffer,
                               &z_pos_flags);

            supl_close_session(TM_STOP_REASON_COMPLETED);

          } /* if there has been a previously disqualified network provided pos. */

          #ifdef FEATURE_RET_REFPOS_UPON_NOFIX  
          /* FEATURE_STALE_POS_ALLOWED must be defined */
          else
          {
            /* if no position has been provided by SLP in SUPL_END, check to see
               if reference position has been provided for this cell */
            if ( supl_report_ref_loc_to_pdapi_if_avail())
            {
              supl_close_session(TM_STOP_REASON_COMPLETED);
            }

            else /* no pos. reported to pdapi */
            {
              supl_close_session(stop_reason);
            }
          }
          #else /* FEATURE_RET_REFPOS_UPON_NOFIX not defined */
          /* FEATURE_STALE_POS_ALLOWED must be defined */
          else
          {
            supl_close_session(stop_reason);
          }
          #endif

          #else /*  FEATURE_STALE_POS_ALLOWED not defined  */
          supl_close_session(stop_reason);
          #endif  /*  FEATURE_STALE_POS_ALLOWED defined */

        } /* else if there is no need to respond to the network */

      } /* event is pd_end_timeout */ 

      else 
      {
        if ( err_type == PDSM_PD_END_E911 )
        {
          if ( z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED ) 
          {
            (void) tm_umts_up_supl_supl_end_tx(&z_supl_session_status.z_session_id.z_Slp_Sess_Id, 
                                        NULL,
                                        supl_StatusCode_unspecified, 
                                        NULL,
                                        z_supl_session_status.q_supl_version);
          }
        }

        supl_close_session(stop_reason);
      }

    }

  }

  return u_ret_val;

}


/*===========================================================================

FUNCTION tm_umts_up_supl_sess_req_data_handler

DESCRIPTION
  This function handles the session data request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_up_supl_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)

{
  /*Check to see if it should be passed to IS801. Since IS801 is using SUPL handle 
    to talk to TM-Core, the TM-Core responses are going to come here and we should 
    redirect it to IS801
  */
  if(z_supl_session_status.u_position_protocol == TM_POS_PRTL_IS801)
  {
    #ifndef FEATURE_GNSS_LOW_MEMORY
      return tm_1x_up_is801_drv_req_aiding_data(session_handle, req_data);   
    #else
      return TRUE;
    #endif
  }
  else
    return TRUE;

}


/*===========================================================================

FUNCTION tm_umts_up_supl_sess_info_handler

DESCRIPTION
  This function handles the session info. request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_up_supl_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *p_sess_info_param
)
{
  supl_status_s_type           *p_supl_session_status = &z_supl_session_status;
  uint32                       q_long_word_gps = 0;
  uint32                       q_long_word_ganss = 0;

  if ( session_handle != p_supl_session_status->tm_active_session_handle )
  {
    return FALSE;
  }
  #ifndef FEATURE_GNSS_LOW_MEMORY
  /*Check to see if it should be passed to IS801. Since IS801 is using SUPL handle 
    to talk to TM-Core,the TM-Core responses are going to come here and we should 
    redirect it to IS801*/
  if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
  {

    return tm_1x_up_is801_drv_sess_info_handler(session_handle,
                                                sess_info_type,
                                                p_sess_info_param                                                        
                                               );
  }
  #endif /*  !FEATURE_GNSS_LOW_MEMORY */
  switch ( sess_info_type )
  {

    case PRTL_SESS_INFO_PPM: /* not relevant to UMTS */
      break;

    case PRTL_SESS_INFO_PRM:
    {
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: got PRM of type %u (GPS/GLO/BDS/GAL/QZSS)",
             p_sess_info_param->prm_data.z_MeasBlk.e_MeasBlkSrc );
        tm_umts_up_supl_handle_prm_data ( &p_sess_info_param->prm_data );
      break;
    }

    case PRTL_SESS_INFO_LR:
    {
      MSG_MED("SUPL: got LR", 0, 0, 0);

      tm_umts_up_supl_handle_lr_data(&p_sess_info_param->lr_data, TRUE);
      break;
    }
    
    case PRTL_SESS_INFO_INTERM_LR:
    {
      tm_umts_up_supl_handle_lr_data(&p_sess_info_param->lr_data, FALSE);
      break;
    }
    

    case PRTL_SESS_INFO_STATUS:
    {
        // Print original wishlist from LM
        MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,"TM Wishlist: 0x%lX (GPS), 0x%lX (GLONASS)", 
              p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask, 
              p_sess_info_param->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask);

      /* -----------------*/
      /* Wishlist for GPS */
      /* -----------------*/
      tm_umts_common_assist_wishlist_build 
        ( &p_sess_info_param->aiding_data_status.z_assist_data_status );

        // Copy the constructed GPS AD wishlist
      q_long_word_gps = p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask;



      /* ---------------------*/
      /* Wishlist for GLONASS */
      /* ---------------------*/
      if ( (tm_umts_up_supl_rrlp_assisted_glonass_enabled() ||
            (tm_umts_common_glonass_enabled() && tm_core_get_asst_glo_lpp_up_enable())) &&
           (p_supl_session_status->u_position_protocol != TM_POS_PRTL_IS801)
         )
      {
        tm_umts_common_glo_assist_wishlist_build(&p_sess_info_param->aiding_data_status.z_assist_data_status);

        q_long_word_ganss = p_sess_info_param->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask;
      }
      else
      {
        /* if Glonass not supported (per NV_item provision) or SUPL ver. is 1, just 
           clear Glonass wishlist mask to make sure it does not contribute to 
           data link bringup 
         */
        p_sess_info_param->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask = 0;
        q_long_word_ganss = 0;
      }

      // Copy the constructed GPS + Glonass AD wishlist
      memscpy(&p_supl_session_status->z_OrigWishList, sizeof(p_supl_session_status->z_OrigWishList),
              &p_sess_info_param->aiding_data_status.z_assist_data_status,
             sizeof(p_sess_info_param->aiding_data_status.z_assist_data_status));

      MSG_MED( "PreHoldoff Wishlist: GPS=0x%lX, GLONASS=0x%lX ",q_long_word_gps, q_long_word_ganss, 0);

      /*Check if Geofencing throtelling is required*/
      if(tm_core_is_internal_session_on() &&
        ( q_long_word_gps || q_long_word_ganss))
      {      
        int32 l_long_word = 0;

        l_long_word = os_TimerCheck( supl_gf_holdoff_timer); 
        if ( l_long_word > 0 )
        {
          MSG_MED("GF holdoff remaining = %ld msec", l_long_word, 0, 0 );

          p_sess_info_param->aiding_data_status.z_assist_data_status.q_WishListMask = 0;
          q_long_word_gps = 0;
          
          p_sess_info_param->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask = 0;
          q_long_word_ganss = 0;
          
        }   
       }

      /* -------------------------*/
      /* Start AD Throttle Timers */
      /* -------------------------*/
      if (q_long_word_gps) 
      {
        tm_umts_up_supl_assist_req_holdoff_timers_check_start( &q_long_word_gps );
      }
      if (q_long_word_ganss) 
      {
        tm_umts_up_supl_glo_assist_req_holdoff_timers_check_start( &q_long_word_ganss );
      }
      MSG_MED( "PostHoldoff Wishlist: GPS=0x%lX, GLONASS=0x%lX",q_long_word_gps, q_long_word_ganss, 0);    
      
      /* if GPS ref. time is the only item left in GPS assistance list, remove it;
         if GNSS time model is the only item left in GNSS assistance list, remove it */
      if ( q_long_word_gps == C_GNSS_ASSIST_REFTIME ) 
      {
        q_long_word_gps = 0;
        MSG_MED( "GPS RefT only req. suppressed", 0, 0, 0);   
      }
      if ( q_long_word_ganss == C_GNSS_ASSIST_TIME_MODELS ) 
      {
        q_long_word_ganss = 0;
        MSG_MED( "GNSS time model only req. suppressed", 0, 0, 0);   
      }
      
      /* ----------------------------------------------------------*/
      /* Assistance Data IE with(out) Targeted Ephemeris - GPS/GLONASS */
      /* ----------------------------------------------------------*/
      if (q_long_word_gps)
      {
        tm_umts_common_utils_gsm0931_gps_assist_data_ie_build 
          ( &p_sess_info_param->aiding_data_status.z_assist_data_status,
            &p_supl_session_status->z_GpsAssistData);
      }
      if (q_long_word_ganss)
      {
        tm_umts_common_utils_gsm0931_ganss_assist_data_ie_build
          ( &p_sess_info_param->aiding_data_status.z_assist_data_status,
            &p_supl_session_status->z_GloAssistData);
      }

      if ( ( q_long_word_gps || q_long_word_ganss ) ||
           (  ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI ) &&
              ( p_supl_session_status->u_current_is_for_area_event == FALSE )
           )   
         )
      { 

         /*Since contacting SUPL server, do not contact SUPL server for GM  for GM Defer time*/
        (void) os_TimerStart( supl_gf_holdoff_timer, C_DEFER_GM_REQ_MSEC, NULL ); 

        /* check if cached cell ID is valid and not stale: if not, request new
           cell ID info. from L1 */
        if ( tm_umts_supl_if_need_forced_wcmda_cell_inq() == FALSE )
        {

          /* Either not on WCDMA, or on WCDMA and no need to directly
             request at this point */ 

          /* for non-WCDMA, check if TDSCDMA. If so do a request */
          tm_umts_up_supl_ck_req_tdscmda_cell_info();

          /* Initializing the flags so that we can attempt a fresh 
             connection*/
          z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
          tm_umts_up_supl_start_slp_contact();
        }
        else
        {
          if ( !tm_umts_up_supl_req_cell_info_from_rrc(C_SUPL_CF_STATE_CELL_INFO_REQ) )
          {
            /* Initializing the flags so that we can attempt a fresh 
               connection*/
            z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
            z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
            /* effort to update cell info. fails, go ahead anyway hoping new 
               update may come before it's ready to send the message */
            tm_umts_up_supl_start_slp_contact(); 
          } 
        }




      }
      else
      {
      
        
        /* SET-initiated, and no assistance needed, inform TmCore not to expect
           anything from the network and start counting down to QoS timeout */
        tm_umts_up_supl_inform_core_nt_assist_end();
        
        if (z_supl_session_status.u_current_is_for_area_event == TRUE )
        {
        
          
          /* for areaEvent trigger sessions, cancel the call flow type */
          tm_umts_up_supl_callflow_kind_set(C_SUPL_SESSION_NONE);
        }
      }

      break;
      
    } /* SESS_INFO_STATUS */ 
    
    #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
    case PRTL_SESS_INFO_OTDOA:
      MSG_HIGH("Received OTDOA measurement information",0,0,0);
      tm_umts_up_supl_handle_otdoa_data( &(p_sess_info_param->otdoa_data)); 
      break;
    #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
      
    default:
      MSG_MED("PRTL_SESS_INFO_ not processed: %u",
               sess_info_type, 0, 0);     
     
  } /* switch_session_info */

  return TRUE;

}



/*===========================================================================

FUNCTION tm_umts_up_supl_data_pipe_handler

DESCRIPTION
  This function handles the session data pipe from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_up_supl_data_pipe_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
)
{

  boolean ret_val = TRUE;

  switch(msg_type)
  {
    case C_PD_COMMS_CLIENT_MSG_NOTIFY:

      tm_umts_up_supl_comm_handle_notification(msg_data_ptr);

      break;


    case C_PD_COMMS_CLIENT_MSG_READ_DATA:
      ret_val = tm_umts_up_supl_comm_handle_read_data(msg_data_ptr);
      break;

    #ifndef FEATURE_GNSS_LOW_MEMORY
    case C_PD_COMMS_CLIENT_MSG_WRITE_ACK: 
      /*SUPL is not interested in this event but IS801 is, so check if this 
        need to be passed to IS801
      */      
      MSG_ERROR("SUPL_API: PDCOMM SUPL SSL ACK", 0, 0, 0);      
      if(z_supl_session_status.u_position_protocol == TM_POS_PRTL_IS801)
      {
        if(z_supl_session_status.wait_for_write_ack)
        {
          /*Supl Pos Init is sent, notify IS801 that the connection is up*/
          (void)tm_1x_up_is801_drv_handle_pde_connected(FALSE);  
          z_supl_session_status.wait_for_write_ack = FALSE;
        }
        else
        {
          tm_1x_up_handle_comm_write_ack(msg_data_ptr);  
        }
      }
      else if((z_supl_session_status.u_position_protocol == TM_POS_PRTL_LPP)&&
              (z_supl_session_status.wait_for_write_ack == TRUE))
      {
         MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED,"SUPL_API: Processing SUPL SSL ACK");
         tm_umts_up_supl_process_write_ack(msg_data_ptr);
      }
      break;

    case C_PD_COMMS_CLIENT_MSG_IOCTL_ACK:           
      /*SUPL is not interested in this event but IS801 is, so check if this need 
        to be passed to IS801
      */
      if(z_supl_session_status.u_position_protocol == TM_POS_PRTL_IS801)
      {
        tm_1x_up_handle_comm_ioctl_ack(msg_data_ptr);  
      }
      break;
    #endif /* !FEATURE_GNSS_LOW_MEMORY */
    case C_PD_COMMS_CLIENT_MSG_HASH_RESP:
      tm_umts_up_supl_comm_handle_hash_resp(msg_data_ptr); 
      break;


    default:
      break;
  }

  return ret_val;
}



/*===========================================================================

FUNCTION tm_umts_up_supl_timer_cb_handler

DESCRIPTION
  This function handles the session data pipe from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_supl_timer_cb_handler ( void *timerData )
{
  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timerData;
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  tm_cm_phone_state_info_s_type phone_state_info;
  uint8 u_i;
  boolean u_disconn_tcp_needed = FALSE;
   
  sm_gm_breach_ack_notify_type z_sm_gm_breach_ack_notify;
  uint8  u_sm_gm_error_code;
   
  #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  tm_lpp_transaction_info_s_type *p_lpp_tx_info_type  = NULL;
  uint32 flags;
  #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */

  if(timer_param != NULL)
  {
    switch(timer_param->q_TimerId)
    {
      #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
      case SUPL_LPP_OTDOA_TIMER_ID:
        /* clear timer and call flow state */
        tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_OTDOA_MEAS_REQ); 
         tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER);
        
        p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR;
        p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_OTDOA_CAUSE_UNABLE_TO_MEAS_NEIGH;
        flags = C_LPP_AGPS_MODE_UE_ASSISTED | C_LPP_MODE_UE_OTDOA;
        if (supl_enc_lpp_prov_loc(p_supl_session_status, flags) == TRUE)
        {
          (void)supl_SuplPos_lpp_tx(p_supl_session_status, NULL);
        }
        else
        {
          p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_LPP_ERROR;
          p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_ERROR_UNDEF;
          if (supl_pos_enc_lpp_error_tx(NULL, p_supl_session_status, &p_lpp_tx_info_type) == FALSE)
          {
            MSG_ERROR("TM_LPP_UP: Error encoding LPP Error message",0,0,0);
          }
        }
        
        tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
        tm_umts_up_supl_session_init(FALSE);
        break;

       case SUPL_LPP_OTDOA_MUL_MEAS_TIMER_ID:
         tm_up_supl_handle_otdoa_mul_meas_timer_expiry();
         break;

       case SUPL_LPP_EARLY_RESPONSE_TIMER_ID:
         tm_up_supl_handle_early_response_timer_expiry();
         break;

      #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
      
      case SUPL_CF_TIMER_ID: /* one timer, many possible states */
      {
        MSG_LOW("SUPL Callflow timer expired. CF State 0x%lX", p_supl_session_status->e_cf_state, 0, 0);

        if (  p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ )
        {
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_CELL_INFO_REQ );

          /* effort to update cell info. fails, go ahead anyway hoping new 
             update may come before it's ready to send the message */
          MSG_HIGH("Celll_id req. to RRC timed out, start SLP contact", 0,0,0);
          /* Initializing the flags so that we can attempt a fresh 
             connection*/
          z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
          tm_umts_up_supl_start_slp_contact(); 

          /* this state will not co-exist with other states, so exit here */
          return;
        }
        
        if (  p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ_AREA_EVENT )
        {
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_CELL_INFO_REQ_AREA_EVENT );

          /* effort to update cell info. fails, go ahead anyway hoping new 
             update may come before it's ready to send the message */
          MSG_HIGH("Celll_id req. to RRC timed out, start SLP contact", 0,0,0);
          /* Initializing the flags so that we can attempt a fresh 
             connection*/
          z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
          tm_umts_up_supl_start_slp_contact_area_event(0xFF); 

          /* this state will not co-exist with other states, so exit here */
          return;
        }

        /* Processed below are status related to network connection, if any of them expires,
           there is no point to stay waiting for WIFI scan result */
        if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP )
        {
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP );
        }

        if ( ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLSTART ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLPOSINIT )
           )
        {

          z_supl_pd_info_buffer.client_id = p_supl_session_status->q_pdapi_client_id;

          /* failure to bring up datalink, fire event_comm_failure */
          (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                                    p_supl_session_status->tm_active_session_handle,
                                    PDSM_PD_EVENT_COMM_FAILURE,
                                    &z_supl_pd_info_buffer,
                                    NULL);

          (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                                    p_supl_session_status->tm_active_session_handle,
                                    PDSM_PD_EVENT_COMM_DONE,
                                    &z_supl_pd_info_buffer,
                                    NULL);
        }
        
        if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND )
        {
          /* no action other than clearing the state needed */
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_UT0_SUPLEND );
          /*Need to close the connection*/
          u_disconn_tcp_needed = TRUE;
          MSG_MED("SUPL: UT0 - SuplEnd expired", 0, 0, 0);
        }
        if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLREPORT_QUERY )
        {
          /* no action other than clearing the state needed */
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_UT0_SUPLREPORT_QUERY );
          MSG_MED("SUPL: UT0 - SuplReport_query expired", 0, 0, 0);
        }
        if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT10_INFO_QUERY_END )
        {
          /* no action other than clearing the state needed */
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_UT10_INFO_QUERY_END );
          tm_umts_up_supl_comm_close_connection();
          supl_session_status_reset();
          MSG_MED("SUPL: UT10 - waiting SUPL_END after query expired", 0, 0, 0);
        }
        
        
        /*** checking if any areaEventTrigger session is at UT0/UT1 */
        for ( u_i = 0; u_i < C_MAX_GEOFENCE_NUM; u_i++)
        {
          if ( z_supl_area_event_session_status[u_i].u_session_active )
          {
            if ( z_supl_area_event_session_status[u_i].e_cf_state &
                 ( C_SUPL_CF_STATE_UT0_SUPLTRIGGERSTART |
                   C_SUPL_CF_STATE_UT1_SUPLTRIGGERRESPONSE |
                   C_SUPL_CF_STATE_UT3_LAST_SUPLREPORT )
               )    
            {
            
              if ( z_supl_area_event_session_status[u_i].e_cf_state &
                   (  C_SUPL_CF_STATE_UT1_SUPLTRIGGERRESPONSE |
                      C_SUPL_CF_STATE_UT3_LAST_SUPLREPORT )
                 )      
              {
                u_disconn_tcp_needed = TRUE;
              }
              
              MSG_HIGH("Clearing areaEventTrig session [%u]: UT exp. - %x", 
                       u_i, z_supl_area_event_session_status[u_i].e_cf_state, 0);  
              
              /* cancell whatever state the areaEventTrigger session may have */
              tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event( 
                C_SUPL_CF_STATE_NULL, u_i );
      
              
              memset( &z_supl_area_event_session_status[u_i], 
                      0, sizeof(supl_area_event_status_s_type));           
            } /* if for this session u_i is at either UT0 or UT1 */

            else if ( z_supl_area_event_session_status[u_i].e_cf_state & 
                      C_SUPL_CF_STATE_UT0_SUPLREPORT_GF_BREACH ) 
            {

              /* cancell whatever state the areaEventTrigger session may have */
              tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event( 
                C_SUPL_CF_STATE_NULL, u_i );

              /* "NACK" GM */
              z_sm_gm_breach_ack_notify.geofence_id =                                  
                z_supl_area_event_session_status[u_i].q_geofence_id;

              /* SUPL_REPORT for breach report has failed to be sent from SM perspective */
              z_sm_gm_breach_ack_notify.accepted = FALSE;
              u_sm_gm_error_code = GEOFENCE_ERROR_UNSPECIFIED;

              gm_breach_ack(&z_sm_gm_breach_ack_notify, &u_sm_gm_error_code);

            }

          } /* if areaEventTrigger session u_i active */  
        } /* geofence for-loop */
        if ( u_disconn_tcp_needed )
        {
          tm_umts_up_supl_comm_close_connection();
        }
        
        /* the following are mutually exclusive */
        if ( ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLSTART ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLPOSINIT ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT1 ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT2 ) || 
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3 ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_RRLP ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_ACK ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT )
           )
        {

          if (  ( ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT1 ) ||
                 ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT2 ) || 
                 ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3 ) ||
                 ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_RRLP ) ||
                 ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_ACK ) 
                ) &&
                !( ( tm_core_is_internal_session_on() == TRUE ) &&  
                   ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI )  
                 )
             )
          {
            /* send SUPL_END if UT1 - UT3 */
            (void) tm_umts_up_supl_supl_end_tx( &p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                                         NULL, 
                                         supl_StatusCode_unspecified, 
                                         NULL,
                                         p_supl_session_status->q_supl_version);
          }

          if ( !( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT ) )
          {
            tm_umts_up_supl_callflow_disrupt_handler();
          }
          else /* UT3-expecting-SUPL_REPORT expiration: tear down TCP */
          {
            tm_umts_up_supl_comm_close_connection();
            p_supl_session_status->e_cf_state &= ~C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT;
          }  
          MSG_MED("SUPL: UTx expired: %u", p_supl_session_status->e_cf_state, 0, 0);

        }
        
         else if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER )
        {
          MSG_MED("SUPL: RRLP msrPosReqRespTime expired", 0, 0, 0);
           tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER);

          if ( p_supl_session_status->u_position_protocol ==  TM_POS_PRTL_RRLP )
          {
            if ( !tm_umts_up_supl_suplpos_rrlp_mpr_err_tx(rrlp_notEnoughSats))
            {
              tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
              tm_umts_up_supl_session_init(FALSE);
            }
          }

          #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
          else if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
          {

            MSG_MED("SUPL: LPP msrPosReqRespTime expired", 0, 0, 0);
             tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER);
            if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
            {

              /* after the PRM is received, get the OTDOA measurements.
                 At time out if we have any valid PRM, ship it*/

              if (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_PRM_VALID) 
              {
                p_supl_session_status->z_supl_lpp_data.q_lpp_flags |= (TM_SUPL_LPP_PRM_VALID) ;
              }
              else
              {
              p_supl_session_status->z_supl_lpp_data.q_lpp_flags &= ~(TM_SUPL_LPP_PRM_VALID) ;
              }
              if ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA) &&
                (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED) &&
                  (p_supl_session_status->z_gnss_meas_report.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GPS) )
              {
                supl_set_cf_state(C_SUPL_CF_STATE_OTDOA_MEAS_REQ, 0);
                sm_loc_lte_otdoa_get_measurements(
                  &p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock, TM_PRTL_TYPE_UMTS_UP,0);
              }
              else
              {
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR;

                if ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA))
                {
                  p_supl_session_status->z_supl_lpp_data.u_lpp_error_type |= (uint8)LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR;
                  p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_OTDOA_CAUSE_AD_MISSING;


                  if ( p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_AD_RECVD )
                  {
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = LPP_AGNSS_CAUSE_NOTENOUGHSAT;
                  }
                  else
                  {
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = LPP_AGNSS_CAUSE_AD_MISSING;
                  }
                  
                  flags = C_LPP_AGPS_MODE_UE_ASSISTED | C_LPP_MODE_UE_OTDOA;
                  if (supl_enc_lpp_prov_loc(p_supl_session_status, flags) == TRUE)
                  {
                    if (!supl_SuplPos_lpp_tx(p_supl_session_status, NULL))
                    {
                      tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
                      tm_umts_up_supl_session_init(FALSE);
                    }
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;
                  }

                  else
                  {
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_LPP_ERROR;
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_ERROR_UNDEF;
                    if (supl_pos_enc_lpp_error_tx(NULL, p_supl_session_status, &p_lpp_tx_info_type) == FALSE)
                    {
                      MSG_ERROR("TM_LPP_UP: Error encoding LPP Error message",0,0,0);
                    }
                    p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;

                  }
                }

                else if ( (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED) &&
                  (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD))
                {
                  p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;
                  p_supl_session_status->z_supl_lpp_data.q_lpp_flags &= ~(TM_SUPL_LPP_PRM_VALID);
                  supl_set_cf_state(C_SUPL_CF_STATE_OTDOA_MEAS_REQ, 0);
                  sm_loc_lte_otdoa_get_measurements(NULL, TM_PRTL_TYPE_UMTS_UP,0);
                }
              }
            } /* MSA */
            else if ( p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE )
            {
              flags =  C_LPP_MODE_UE_OTDOA;
              if (supl_enc_lpp_prov_loc(p_supl_session_status, flags) == TRUE)
              {
                if (!(supl_SuplPos_lpp_tx(p_supl_session_status, NULL)))
                {
                  tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
                  tm_umts_up_supl_session_init(FALSE);
                }
              }
              else
              {
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_LPP_ERROR;
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_ERROR_UNDEF;
                if (supl_pos_enc_lpp_error_tx(NULL, p_supl_session_status, &p_lpp_tx_info_type) == FALSE)
                {
                  MSG_ERROR("TM_LPP_UP: Error encoding LPP Error message",0,0,0);
                }
              }

              /* Send request to stop the OTDOA engine */
              tm_lpp_proc_otdoa_stop_req( p_supl_session_status->tm_active_session_handle, TM_STOP_REASON_COMPLETED, TM_PRTL_TYPE_UMTS_UP );
            }
            else
            {
              flags = C_LPP_AGPS_MODE_UE_BASED;
              p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR;
              p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_AGNSS_CAUSE_NOTENOUGHSAT;
              if (supl_enc_lpp_prov_loc(p_supl_session_status,flags) == TRUE)
              {
                if (!supl_SuplPos_lpp_tx(p_supl_session_status, NULL))
                {
                  tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
                  tm_umts_up_supl_session_init(FALSE);
                }
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;

              }

              else
              {
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_LPP_ERROR;
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_ERROR_UNDEF;
                if (supl_pos_enc_lpp_error_tx(NULL, p_supl_session_status, &p_lpp_tx_info_type) == FALSE)
                {
                  MSG_ERROR("TM_LPP_UP: Error encoding LPP Error message",0,0,0);
                }
                p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;

              }
            }
          } /* if LPP*/
          #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
        }

        break;
      }

      #ifdef FEATURE_USER_PLANE_MRL
      case SUPL_L1_MRL_REQ_TIMER_ID:
        /* Query TM-CM module to get current acquired system */
        tm_cm_iface_get_phone_state(&phone_state_info);   
        #ifdef FEATURE_CGPS_WCDMA
        if ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA )
        {
          tm_umts_up_supl_send_wcdma_cell_meas_req(); 
          (void) os_TimerStart(l1_mrl_req_timer, 1000, NULL); 
        }
        #endif /* FEATURE_CGPS_WCDMA */
        
        #ifdef FEATURE_CGPS_LTE_CELLDB
        if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) 
        {
          lte_ml1_request_gps_mrl();
          (void) os_TimerStart(l1_mrl_req_timer, 1000, NULL); 
        }
        #endif
        break;
      #endif /* MRL */

      case SUPL_ALM_REQ_HD_TIMER_ID: 
      case SUPL_EPH_REQ_HD_TIMER_ID:
      case SUPL_IONO_REQ_HD_TIMER_ID:  
      case SUPL_GLO_ALM_REQ_HD_TIMER_ID:
      case SUPL_GLO_EPH_REQ_HD_TIMER_ID:
      case SUPL_UTC_REQ_HD_TIMER_ID:          
      case SUPL_REFLOC_REQ_HD_TIMER_ID: 
      case SUPL_GM_REQ_HD_TIMER_ID:   
        MSG_HIGH("SUPL timer %u expires", timer_param->q_TimerId, 0, 0);       
        break;
             
      case SUPL_UDP_BIND_RETRY_TIMER_ID:
        MSG_HIGH("SUPL UDP BIND retry timer expired",0,0,0);
        (void)tm_umts_up_supl_comm_udp_bind(FALSE);
        break;
      
      case SUPL_MULTIPART_WAP_PUSH_WAIT_TIMER_ID:
        MSG_HIGH("SUPL MULTIPART WAP PUSH wait timer expired", 0, 0, 0);
        tm_umts_supl_comm_sms_read_state_init();
        break;
        
      case SUPL_TCP_STAY_ON_ONLY_TIMER_ID:
        MSG_HIGH("SUPL_TCP_STAY_ON_ONLY_TIMER_ID expired", 0, 0, 0);
        tm_umts_up_supl_comm_close_connection();
        break;  

      case SUPL_WIFI_WAIT_TIMER_ID:
        MSG_HIGH("SUPL_WIFI_WAIT timer expired", 0, 0, 0);
        if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP )
        {
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP );
        }
        if ( ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_SUPLSTART ) ||
             ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLSTART )
           )
        {
          /* Got WiFi cell info, need to send SUPL_START now */
          MSG_MED("Sending SUPL_START at WIFI_WAIT timeout with cf_state of %x", p_supl_session_status->e_cf_state, 0, 0);
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_SUPLSTART );
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLSTART );

          if ( !supl_SuplStart_tx())   
          {
            tm_umts_up_supl_callflow_disrupt_handler();
          }   

        }
        if (  ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_SUPLPOSINIT ) ||
              ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLPOSINIT)
           )
        {  

          /* Got WiFi cell info, need to send SUPL_POSINIT now */

          MSG_MED("Sending SUPL_POSINIT at WIFI_WAIT timeout with cf_state of %x", p_supl_session_status->e_cf_state, 0, 0);
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_SUPLPOSINIT );
          tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLPOSINIT );

          if ( !supl_SuplPosInit_tx())
          {
            tm_umts_up_supl_callflow_disrupt_handler();
          }
          else /*SuplPosInit Sent*/
          { 
            if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801 &&
              z_supl_session_status.u_callflow_kind == C_SUPL_SESSION_NI)
            {
              /*Start IS801 state machine*/
              if( !supl_start_is801_session(TRUE, NULL))
              {
                /*Unable to start IS801 state machine*/
                tm_umts_up_supl_callflow_disrupt_handler();
              }
            }
          }    
          /*If SUPL POS INIT is succesful, then cftimer is now set to UT2,
            If SUPL POS INIT is unsuccesful, then cftimer is stopped.
            So need not take care of cftimer, just cancel the UT0 state
          */
          z_supl_session_status.e_cf_state &= (supl_cf_state_enum_type)~C_SUPL_CF_STATE_UT0_SUPLPOSINIT;
        }
        break;

      default:
        MSG_ERROR("SUPL got undefined SUPL timer ID", 0, 0, 0);
    }
  }
  else
  {
    MSG_HIGH("Cannot process null timer msg", 0, 0, 0);  
  }


  return;
}


/*===========================================================================

FUNCTION tm_umts_up_supl_event_cb_handler

DESCRIPTION
  This function handles the session data pipe from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_up_supl_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *p_event_payload
)
{

  boolean ret_val = TRUE;

  /* Update this check when adding new EVENT TYPE
     If any new Event is added and the event does not carry a payload update the IF case*/
  if((NULL == p_event_payload) &&
     (event_type != PRTL_EVENT_TYPE_LTE_OTA_OTDOA_RESET)&&
     (event_type != PRTL_EVENT_TYPE_LTE_OTA_GPS_RESET))
  {
     MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL: Event Payload NULL" );
     return FALSE;
  }

  switch ( event_type )
  {
  
    case PRTL_EVENT_TYPE_LCS:
    {
      if ( p_event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_RESP )
      {
        tm_umts_up_supl_notif_verif_resp_proc( &p_event_payload->lcs_event_payload.lcs_event_payload.clnt_resp_payload );
      }
      else if ( p_event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_CLINET_INIT )
      {
        MSG_MED("SUPL: TmCore cb LCS type with event CLINET_INIT" , 0 , 0 ,0 ); /* QQQ: ? */
      }
      else if ( p_event_payload->lcs_event_payload.lcs_event == PRTL_LCS_EVENT_TYPE_CLINET_RELEASE )
      {
        MSG_MED("SUPL: TmCore cb LCS type with event CLINET_RELEASE" , 0 , 0 ,0 ); /* QQQ: ? */
      }

      break;
    }

    case PRTL_EVENT_TYPE_PHONE:
    {

      if ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_E911_CALL_ORIG )
      {
        MSG_MED("SUPL: got Ell call orig", 0, 0, 0);
        tm_umts_up_supl_E911_handler();
      }
      else if ( ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_UMTS_COVERAGE ) ||
                ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_TDS_COVERAGE )
              )   
      {
        /* Manually set SETID to UNKNOWN. So that the correct IMSI is used for SUPL sessions when SIMs are swapped*/
        z_Set_Id.set_id_choice = C_SETID_UNKNOWN;
    
        /* Retrieve imsi for supl */
    (void) tm_umts_up_supl_comm_retrieve_imsi();
    
        if ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_UMTS_COVERAGE )
        {
          MSG_MED("Camped to UMTS: G or W or L", 0, 0, 0);
        }


        else if ( p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_TDS_COVERAGE )
        {
          MSG_MED("Camped to TDSCDMA", 0, 0, 0);
          tm_umts_up_supl_ck_req_tdscmda_cell_info();
        }  

        if ( pdcomm_tcp_handle_get_done == FALSE )
        {
          /* get tcp handle now, since not done at TM start time */
          if ( tm_umts_up_supl_get_pdcomm_tcp_handle() )
          {
            pdcomm_tcp_handle_get_done = TRUE;
          }

          /* update the supl comm module with the root cert dir */
            tm_umts_up_supl_comm_pass_root_cert_dir();
            tm_umts_up_supl_comm_root_certs_updated();
        } /* if at TM start time, pdcomm tcp not yet started */ 

        /*RAT changed, do the UDP bind again, if the socket is already created,
          PDCOMM will just deregister and register the filters on new interface*/
        //(void)tm_umts_up_supl_comm_udp_bind(TRUE);
      }
      else if (p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_1X_COVERAGE)
      {
        MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Camped to 1X. Network %d", z_supl_session_status.u_network_session_on );

    /* Manually set SETID to UNKNOWN. So that the correct IMSI is used for SUPL sessions when SIMs are swapped*/
        z_Set_Id.set_id_choice = C_SETID_UNKNOWN;

        /* Close the supl session if SUPL protocol is not supported on 1X and SET is talking(SUPL
        *  call flow) to network/server. The later check is needed to not stop the AppTracking using
        *  XTRA session.
        */
        if( TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) == 0 )
        {
          if ( ( z_supl_session_status.u_callflow_kind != C_SUPL_SESSION_NONE ) &&
               (TRUE == z_supl_session_status.u_network_session_on) )
          {
            supl_close_session(TM_STOP_REASON_SERVING_SYSTEM_CHANGE);
          }
        }

        /*RAT changed, do the UDP bind again, if the socket is already created,
          PDCOMM will just deregister and register the filters on new interface*/
        //(void)tm_umts_up_supl_comm_udp_bind(TRUE);
      }
      else if ((p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_OOS) ||
               (p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_SYS_MODE_LPM)
              )
      {
        MSG_MED("Out Of Service or LPM mode", 0, 0, 0);
        // Set the cached cell info to invalid so that we dont try to use a stale 
        // cached cell-id when we come back online.
        z_cell_info_cached.u_CachedCellIDValid = FALSE;

        (void)tm_umts_up_supl_comm_abort_udp_bind_retries();
      }      
      else
      {
        MSG_MED("SUPL: got TmCore cb event PHONE - not E911", 0, 0, 0);
      }

      break;
    }

    case PRTL_EVENT_TYPE_CELL_DB:    /* cell DB info. update from MGP OR external cell DB info from PDAPI */
    if (p_event_payload->cell_db_event_payload.cell_db_event == PRTL_MAIN_MODEM_CELL_DB_UPD)
    {
        tm_umts_up_supl_cell_db_update( 
      p_event_payload->cell_db_event_payload.cell_db_event_payload.main_modem_cell_db_payload.cell_db_event_data_ptr );
    }
    else if (p_event_payload->cell_db_event_payload.cell_db_event == PRTL_EXTERNAL_CELL_DB_UPD)
    {
      tm_umts_up_supl_external_cell_db_update( 
      p_event_payload->cell_db_event_payload.cell_db_event_payload.external_modem_cell_db_payload );
    } 
    else
    {
      MSG_HIGH("SUPL: got unknown cell_db_event type: %u", p_event_payload->cell_db_event_payload.cell_db_event, 0, 0);
        ret_val = FALSE;
    } 
      break;

    case PRTL_EVENT_TYPE_L1:   /* WCDMA L1 call back reporting MRL */
      if ( p_event_payload->l1_event_payload.l1_event_data_ptr->e_event_type == 
           L1_CGPS_EVENT_CELL_MEAS_UPDATE 
      )
      {
        tm_umts_up_supl_cell_meas_update
        ( 
          p_event_payload->l1_event_payload.l1_event_data_ptr->u.l1_cell_meas_update_ptr
        );
      }
      break;
      
    #ifdef FEATURE_CGPS_LTE_CELLDB
    case PRTL_EVENT_TYPE_LTE_L1:   /* LTE L1 call back reporting MRL */
      if ( p_event_payload->lte_l1_event_payload.l1_event_data_ptr->e_event_type == 
           LTE_ML1_GPS_EVENT_CELL_MEAS_UPDATE 
      )
      {
        tm_umts_up_supl_lte_cell_meas_update
        ( 
          p_event_payload->lte_l1_event_payload.l1_event_data_ptr->u.l1_cell_meas_update_ptr
        );
      }
      break;
    #endif  

    case PRTL_EVENT_TYPE_PA:
    {
      tm_umts_up_supl_pa_proc ( &p_event_payload->pa_event_payload );
      break;
    } 
          
    case PRTL_EVENT_TYPE_MM:
    {
      if ( p_event_payload->mm_event_payload.mm_event_data_ptr->e_event_type == 
           MM_CGPS_MSG_EVENT_RESET_UE_POS_INFO )
      {
        tm_umts_up_supl_ota_delete_proc();
      }
      break;
    }       
   case PRTL_EVENT_TYPE_LTE_OTA_GPS_RESET:
    {
      MSG_MED("LTE OTA RESET message received, Clearing Hold off timers",0,0,0);
      tm_umts_up_supl_ota_delete_proc();
      
    }
    break;

   case PRTL_EVENT_TYPE_LTE_OTA_OTDOA_RESET:
    {
      MSG_MED("LTE OTA OTDOA RESET message received", 0, 0, 0);
      #ifdef FEATURE_LTE
      /* Set LTE OTA OTDOA reset flag */
      b_lte_ota_otdoa_reset_flag = TRUE;
      #endif /* FEATURE_LTE */
    }
    break;

    case PRTL_EVENT_TYPE_SMS:
    {
      /* Receive NI SUPL INIT SMS from PDAPI (external injection from HLOS) */
      (void)tm_umts_up_supl_comm_ni_sms(p_event_payload->sms_event_payload.length, 
                                        p_event_payload->sms_event_payload.data);
      break;
    }


    case PRTL_EVENT_TYPE_WIFI_SCAN_RESULT:
    {
      MSG_MED("SUPL got Wifi scan result: %u", p_event_payload->wifi_scan_event_payload.q_num_wifi_aps, 0, 0 );
      tm_umts_up_supl_wifi_scan_report_proc (p_event_payload->wifi_scan_event_payload );
      break;
    }
    
    case PRTL_EVENT_TYPE_CARRIER_NV_REFRESH:
    {
      MSG_MED("CARRIER NV REFRESH received for SUPL", 0, 0, 0);
      tm_umts_common_utils_refresh_carrier_specific_nv();
      #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
      tm_umts_up_supl_refresh_efs_nv();
      #endif /* FEATURE_CGPS_LTE_CELLDB && ! FEATURE_GNSS_LOW_MEMORY */
      break;
    }

    default:
      MSG_MED("SUPL: got unknown TmCore cb event: %u", event_type, 0, 0);
      ret_val = FALSE;
      
  }    


  return ret_val;

}


/*===========================================================================

FUNCTION tm_umts_up_supl_general_query_handler

DESCRIPTION
  This function handles general query from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_up_supl_general_query_handler
( prtl_query_e_type query_type, 
  prtl_ans_u_type *answer_ptr 
)
{
  boolean u_ret_val = FALSE;

  if ( query_type == PRTL_QUERY_SMART_MSB )
  {
    if ( z_supl_session_status.u_callflow_kind == C_SUPL_SESSION_NI )
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

FUNCTION tm_umts_up_supl_init

DESCRIPTION
  This function is used by TmCore which calls it when TM task starts, 
  initiating the SUPL submodule.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_supl_init (void)
{

  MSG_MED("tm_supl_init", 0, 0, 0);

  pdcomm_tcp_handle_get_done = FALSE; 

  z_supl_session_status.supl_msg_q_status = FALSE;

  tm_umts_up_supl_session_init(FALSE); /* reset all SUPL session related variables */

  /* create SUPL timers */ 
  supl_cf_timer                     = os_TimerCreateInCgpsTimerGroup( SUPL_CF_TIMER_ID, THREAD_ID_SM_TM );
  l1_mrl_req_timer                  = os_TimerCreateInCgpsTimerGroup( SUPL_L1_MRL_REQ_TIMER_ID, THREAD_ID_SM_TM );
  #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  supl_lpp_otdoa_req_timer          = os_TimerCreateInCgpsTimerGroup( SUPL_LPP_OTDOA_TIMER_ID, THREAD_ID_SM_TM );
  supl_lpp_otdoa_mul_meas_timer          = os_TimerCreateInCgpsTimerGroup( SUPL_LPP_OTDOA_MUL_MEAS_TIMER_ID, THREAD_ID_SM_TM );
  supl_lpp_early_response_timer          = os_TimerCreateInCgpsTimerGroup( SUPL_LPP_EARLY_RESPONSE_TIMER_ID, THREAD_ID_SM_TM);
  
  #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
  /* create assistance data request holdoff period timers */
  supl_alm_req_holdoff_timer        = os_TimerCreateInCgpsTimerGroup( SUPL_ALM_REQ_HD_TIMER_ID, THREAD_ID_SM_TM );
  supl_eph_req_holdoff_timer        = os_TimerCreateInCgpsTimerGroup( SUPL_EPH_REQ_HD_TIMER_ID, THREAD_ID_SM_TM );
  supl_iono_req_holdoff_timer       = os_TimerCreateInCgpsTimerGroup( SUPL_IONO_REQ_HD_TIMER_ID, THREAD_ID_SM_TM );
  supl_utc_req_holdoff_timer        = os_TimerCreateInCgpsTimerGroup( SUPL_UTC_REQ_HD_TIMER_ID, THREAD_ID_SM_TM );
  supl_refloc_req_holdoff_timer     = os_TimerCreateInCgpsTimerGroup( SUPL_REFLOC_REQ_HD_TIMER_ID, THREAD_ID_SM_TM );
  supl_tcp_stay_on_only_timer       = os_TimerCreateInCgpsTimerGroup( SUPL_TCP_STAY_ON_ONLY_TIMER_ID, THREAD_ID_SM_TM );
  supl_glo_alm_req_holdoff_timer    = os_TimerCreateInCgpsTimerGroup( SUPL_GLO_ALM_REQ_HD_TIMER_ID, THREAD_ID_SM_TM );
  supl_glo_eph_req_holdoff_timer    = os_TimerCreateInCgpsTimerGroup( SUPL_GLO_EPH_REQ_HD_TIMER_ID, THREAD_ID_SM_TM );
  supl_wifi_wait_timer              = os_TimerCreateInCgpsTimerGroup( SUPL_WIFI_WAIT_TIMER_ID, THREAD_ID_SM_TM );
  supl_gf_holdoff_timer              = os_TimerCreateInCgpsTimerGroup( SUPL_GM_REQ_HD_TIMER_ID, THREAD_ID_SM_TM );

  memset(&z_cell_info_cached, 0, sizeof(cgps_CellInfoCachedStruct) );
  memset(&z_external_cell_info_cached, 0, sizeof(pdsm_cell_info_s_type) );
  memset(&z_supl_cached_pos_for_net, 0, sizeof(supl_cached_pos_for_net_s_type));
  
  memset(&z_Set_Id, 0, sizeof(supl_set_id) ); 
  
  /* this needs to be read from EFS */
  memset(&z_supl_area_event_session_status[0], 
         0, 
         sizeof (supl_area_event_status_s_type) * C_MAX_GEOFENCE_NUM );

  /* initialize OSYS ASN.1 encoder/decoder */
   tm_osys_init(); //damn: move this to tm_core !!!!!!

  (void) tm_umts_up_supl_comm_init(); 

  (void) tm_umts_supl_efs_dir_init(); 
  
  tm_umts_up_supl_gm_init(); /* register with geofence */
  #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  tm_umts_up_supl_lpp_tx_mgr_init();
  #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
  memset(&z_mlid_buffer[0], 0,  ARR_SIZE(z_mlid_buffer) * sizeof (mlid_buffer_strcut_type) );
} 

/*===========================================================================

FUNCTION tm_umts_up_supl_api_close_session

DESCRIPTION
  This function is used by modules outside of SUPL protocol and calls a 
  collection of functions to end the session, after sending SUPL_END if 
  data link is available.
  
DEPENDENCIES: none.

RETURN VALUE: TRUE - a SUPL session has been closed.
              FALSE - no SUPL session to close.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_up_supl_api_close_session(tm_sess_stop_reason_e_type  stop_reason )
{

  if ( z_supl_session_status.u_callflow_kind == C_SUPL_SESSION_NONE )
  {
    /* no actiave SUPL session, nothing to close */
    return FALSE;
  }

  if ( z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED ) 
  {
    (void) tm_umts_up_supl_supl_end_tx
      ( &z_supl_session_status.z_session_id.z_Slp_Sess_Id, 
        NULL,
        supl_StatusCode_unspecified, 
        NULL,
        z_supl_session_status.q_supl_version);
  }

  supl_close_session(stop_reason);

  return TRUE;
}


/*===========================================================================

FUNCTION tm_umts_supl_rrc_cell_info_update_mlid

DESCRIPTION
  This function updates the multiple location ID date base using the cell 
  info from the response of direct request for WCDMA cell info.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
#ifdef FEATURE_CGPS_WCDMA
void tm_umts_supl_rrc_cell_info_update_mlid(rrcmeas_plmn_cell_info_struct_type*  cell_id_info_ptr)
{
  cgps_CellInfoCachedStruct z_cell_db_data;
  cgps_CellInfoCachedStruct* cell_db_data_ptr = &z_cell_db_data;
  
  if ( cell_id_info_ptr->cell_id == 0xFFFFFFFF)
  {
    return;
  }  
  
  memset(cell_db_data_ptr, 0, sizeof(cgps_CellInfoCachedStruct) );
  
  cell_db_data_ptr->e_ActiveRAT = CELLDB_ACTIVERAT_WCDMA;
  cell_db_data_ptr->q_NumCellChangesCachedCellID = 1; /* here 1 means no cell change */
  cell_db_data_ptr->u_CachedCellIDValid = TRUE;
  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id =
    cell_id_info_ptr->cell_id;
  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq =
    cell_id_info_ptr->freq;
  memscpy(&cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc[0],
          sizeof(cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc),
         &cell_id_info_ptr->plmn_id.mcc[0],
         sizeof(cell_id_info_ptr->plmn_id.mcc)
        );
  memscpy(&cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[0],
          sizeof(cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc),
         &cell_id_info_ptr->plmn_id.mnc[0], sizeof(cell_id_info_ptr->plmn_id.mnc)
        );
  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits =
    cell_id_info_ptr->plmn_id.num_mnc_digits;

  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc =
    cell_id_info_ptr->psc;
  
  tm_umts_up_supl_update_mlid_db(cell_db_data_ptr);  

}


/*===========================================================================

FUNCTION tm_umts_supl_api_rrc_cell_info_proc

DESCRIPTION
  This function processes RRC response to TM's request for cell info. which
  is sent when in DCH where the regular cell info. update mechanism would
  fail to provide cell info of the current cell.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_umts_supl_api_rrc_cell_info_proc
( rrcmeas_plmn_cell_info_struct_type cell_id_info,
  rrc_cell_id_status_type            status
)
{

  uint32 q_mcc_new, q_mnc_new, q_mcc_old, q_mnc_old;
  
  if( status == RRC_NEW_CELL_ID_AVAIL )
  {

    (void) tm_umts_supl_wcdma_plmn_id_organizer ( &cell_id_info.plmn_id, &q_mcc_new, &q_mnc_new ); 

    if ( ( q_mcc_new > C_MCC_MNC_UPLIMIT ) ||  ( q_mnc_new > C_MCC_MNC_UPLIMIT ) || 
         (cell_id_info.plmn_id.num_mnc_digits > 3 ) || 
         (cell_id_info.plmn_id.num_mnc_digits < 2  ) ||
         (cell_id_info.cell_id  > C_WCDMA_CELL_ID_UPLIMIT ) ||
         (cell_id_info.freq  > C_WCDMA_CELL_FREQ_UPLIMIT ) ||
         (cell_id_info.psc  > C_WCDMA_CELL_PSC_UPLIMIT )
       ) 
    {

      MSG_6(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
            "Bogus DCH-WCDMA cell info rejected: MCC=%lu,MNC=%lu,MNC_Nbits=%u,CI=%lu,FREQ=%u,PSC=%u", 
            q_mcc_new, 
            q_mnc_new, 
            cell_id_info.plmn_id.num_mnc_digits,
            cell_id_info.cell_id,
            cell_id_info.freq,
            cell_id_info.psc);

      return;
    }

    #ifdef FEATURE_CGPS_WCDMA
    tm_umts_supl_rrc_cell_info_update_mlid(&cell_id_info);
    #endif /* FEATURE_CGPS_WCDMA */

    (void) tm_umts_supl_wcdma_plmn_id_organizer ( &cell_id_info.plmn_id, &q_mcc_new, &q_mnc_new ); 

    if ( !z_cell_info_cached.u_CachedCellIDValid )
    {
      MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
            "DCH-WCDMA cell info recved when no cached available: MCC=%lu,MNC=%lu,CI=%lu,FREQ=%u,PSC=%u", 
            q_mcc_new, 
            q_mnc_new, 
            cell_id_info.cell_id,
            cell_id_info.freq,
            cell_id_info.psc);
    }

    else
    {
      if ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
      {

        (void) tm_umts_supl_wcdma_plmn_id_organizer
          ( &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id, 
            &q_mcc_old, 
            &q_mnc_old ); 

        MSG_7(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
              "DCH-WCDMA cell info updating cached W: MCC=%lu,MNC=%lu,CI=%lu  Existing: Nchange=%lu,MCC=%lu,MNC=%lu,CI=%lu", 
              q_mcc_new, 
              q_mnc_new, 
              cell_id_info.cell_id,
              z_cell_info_cached.q_NumCellChangesCachedCellID,
              q_mcc_old,
              q_mnc_old,
              z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id);
      }
      else
      {
        MSG_MED("DCH-WCDMA cell info updating cached Non-W: New MCC=%lu,MNC=%lu,CI=%lu", 
                 q_mcc_new, 
                 q_mnc_new, 
                 cell_id_info.cell_id);
      }

    }

    /* update cell info. */
    z_cell_info_cached.e_ActiveRAT = CELLDB_ACTIVERAT_WCDMA;
    z_cell_info_cached.q_NumCellChangesCachedCellID = 1; /* here 1 means no cell change */
    z_cell_info_cached.u_CachedCellIDValid = TRUE;
    z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id =
      cell_id_info.cell_id;
    z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq =
      cell_id_info.freq;
    memscpy(&z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc[0],
            sizeof(z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc),
           &cell_id_info.plmn_id.mcc[0],
           sizeof(cell_id_info.plmn_id.mcc)
          );
    memscpy(&z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[0],
           sizeof(z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc),
           &cell_id_info.plmn_id.mnc[0],
           sizeof(cell_id_info.plmn_id.mnc)
          );
    z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits =
      cell_id_info.plmn_id.num_mnc_digits;

    z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc =
      cell_id_info.psc;


    if (  z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ )
    {
      MSG_HIGH("CELLIDDCH: Cell info updated by RRC, start SLP contact", 0,0,0);
    }
    else if (  z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ_AREA_EVENT )
    {
      MSG_HIGH("CELLIDDCH: Cell info updated by RRC, start SLP contact areaEvent", 0,0,0);
    }
    else
    {
      MSG_HIGH("CELLIDDCH: Cell info updated by RRC while not expecting", 0,0,0);
    }


  }  /* if status == RRC_NEW_CELL_ID_AVAIL */

  else
  {
    if (  z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ )
    {
      MSG_HIGH("CELLIDDCH: Recvd no-new cell info from RRC, start SLP contact",0,0,0);
    }
    else if (  z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ_AREA_EVENT )
    {
      MSG_HIGH("CELLIDDCH: Recvd no-new cell info from RRC, start SLP contact areaEvent",0,0,0);
    }
    else
    {
      MSG_HIGH("CELLIDDCH: Recvd no-new cell info from RRC while not expecting", 0,0,0);
    }
  }  /* if status is RRC_NO_CELL_ID_AVAIL */

  if (  z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_CELL_INFO_REQ );
    /* Initializing the flags so that we can attempt a fresh 
       connection*/
    z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
    z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
    tm_umts_up_supl_start_slp_contact(); 
  }
  
  else if (  z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ_AREA_EVENT )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_CELL_INFO_REQ_AREA_EVENT );
    /* Initializing the flags so that we can attempt a fresh 
       connection*/
    z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
    z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
    tm_umts_up_supl_start_slp_contact_area_event(0xFF); 
  }

}
#endif /* FEATURE_CGPS_WCDMA */

/*===========================================================================

FUNCTION tm_umts_up_supl_gm_geofence_add_event_handler

DESCRIPTION
  This function handles the Add geofence notificaton from GM
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void tm_umts_up_supl_gm_geofence_add_notify_handler(sm_gm_client_notification_type* gm_notification)
{
  uint8 u_i, u_session_num;
  
  u_session_num = 0xFF;
  
  
  for (u_i=0; u_i<C_MAX_GEOFENCE_NUM; u_i++)
  {
    if ( z_supl_area_event_session_status[u_i].u_gm_trasaction_id_valid &&
         ( z_supl_area_event_session_status[u_i].q_gm_transaction_id == 
           gm_notification->q_transaction_id )
       )
    {
      u_session_num = u_i;
      break;
    }
  }      
  
  if ( u_session_num == 0xFF )
  {
    MSG_HIGH( "no matching transaction id %lu, ignored", 
              gm_notification->q_transaction_id , 0, 0);
    return;
  }            
  
  if(gm_notification->notification.return_code == GEOFENCE_ERROR_NONE)
  {
    MSG_MED("Geofence Add Succesfull Geofence Id %d", gm_notification->notification.geofence_id, 0,0);
    z_supl_area_event_session_status[u_session_num].q_geofence_id =          
      gm_notification->notification.geofence_id;
    z_supl_area_event_session_status[u_session_num].u_geofence_id_valid = TRUE;
          
  }
  else
  {
    MSG_MED("Geofence Add Error %d", gm_notification->notification.return_code, 0, 0);
    /* TBD: add response to SLP */
  }
}

/*===========================================================================

FUNCTION tm_umts_up_supl_gm_event_handler

DESCRIPTION
  This function handles the GM events passed from TM-Core.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
boolean tm_umts_up_supl_gm_event_handler
(
  uint32    msg_type,
  void      *msg_data_ptr
)
{
  
  boolean ret_val = TRUE;
  uint8   u_session_num;
  boolean pos_request_started = FALSE;
  
  switch(msg_type)
  {
    case GEOFENCE_ADD_NOTIFY:
       tm_umts_up_supl_gm_geofence_add_notify_handler(
          (sm_gm_client_notification_type*)msg_data_ptr);
       
      break;
      
    case GEOFENCE_DELETE_NOTIFY:      
      MSG_HIGH("Geofence %d deleted by GM", ((sm_gm_client_notification_type*)msg_data_ptr)->notification.geofence_id, 0,0);
      tm_umts_up_supl_gf_delete_proc((sm_gm_client_notification_type*)msg_data_ptr);  
      break;
      
    case GEOFENCE_PURGE_NOTIFY:
      break;
      
    case GEOFENCE_BREACH_NOTIFY:
      MSG_HIGH("Geofence id %d breached", ((sm_gm_client_breach_notify_type*)msg_data_ptr)->geofence_id, 0,0);
      tm_umts_up_supl_gf_breach_proc((sm_gm_client_breach_notify_type*)msg_data_ptr);  
      break;
      
    case GEOFENCE_POS_REQUEST_NOTIFY:
      if ( z_supl_session_status.u_callflow_kind == C_SUPL_SESSION_NONE)  
      {
        tm_sess_operation_mode_e_type pos_operation_mode = TM_SESS_OPERATION_MODE_NONE;
        
        if (((sm_gm_client_pos_req_type*)msg_data_ptr)->pos_operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY)
        {
          pos_operation_mode = TM_SESS_OPERATION_MODE_STANDALONE;
        }  
        else
        {
          pos_operation_mode = TM_SESS_OPERATION_MODE_MSB;

          if(tm_core_get_active_prtl_module(MT_UP) == TM_PRTL_TYPE_UMTS_UP)
          {
            
                
            u_session_num = tm_umts_up_supl_geofence_areaEventSession_match (0xFFFFFFFF);

            if( u_session_num != 0xFF )
            {          
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GM pos req. proc: slot %u ", 
                    u_session_num);
              /* this will ask pdapi to generate pdsm_get_pos() to TmCore */
              tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

              tm_umts_up_supl_area_event_to_active(u_session_num);
            }          
          }
          else
          {
            MSG_HIGH("SUPL not active protocol",0,0,0);
          }
        }    


        
        
        /* there is no application requesting the position in this case,
           hard code gps session timeout to 40 seconds */     
        if(tm_umts_common_ni_client_start_session( 
            pos_operation_mode, 
           ((sm_gm_client_pos_req_type*)msg_data_ptr)->horizontal_accuracy, 
             ((sm_gm_client_pos_req_type*)msg_data_ptr)->session_timeout, 
            ((sm_gm_client_pos_req_type*)msg_data_ptr)->num_fixes,
            ((sm_gm_client_pos_req_type*)msg_data_ptr)->time_between_fixes,
            MT_UP, TRUE)== TRUE)           
        {
          pos_request_started = TRUE;
        }
          
        /* if MSB, expect AD call flow, turn off network_session_on flag */
        z_supl_session_status.u_network_session_on = FALSE;  
      }
      else
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SUPL ongoing: GM pos req. ignored. %u", 
              z_supl_session_status.u_callflow_kind);        
      } 

      if(pos_request_started == FALSE)
      {
        /*Send notification to GM that pos request failed*/
        gm_position_fix_request_fail(((sm_gm_client_pos_req_type*)msg_data_ptr)->pos_fix_src);
        
      }
      break;   
    
    case GEOFENCE_POS_ABORT_NOTIFY:
    if(tm_core_is_internal_session_on())
    {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_COMPLETED);
    }
    break;
    
    case GEOFENCE_CLIENT_GEOFENCE_HANDELS_NOTIFY:
    {

      sm_gm_get_geofence_client_blob_request_type  gm_blob_request;
      uint8 error_code;
      uint32 *geofence_handle = NULL;

      sm_gm_client_geofence_handels_notification_type *p_notification =  
        (sm_gm_client_geofence_handels_notification_type*)msg_data_ptr;
        
      uint8 u_i;  
  
      MSG_MED("Num. of existing geofences: %u", 
              p_notification->num_of_client_geofence_handles,
              0,
              0);

      gm_blob_request.client_handle = supl_gm_handle;

      geofence_handle = &p_notification->geofence_handles[0];
      for (u_i=0; u_i< p_notification->num_of_client_geofence_handles; u_i++)
      {
        
        gm_blob_request.geofence_id =*geofence_handle;
        geofence_handle++;
        gm_blob_request.q_transaction_id = supl_comm_getnext_gm_transaction_id();
  
        if ( gm_get_geofence_client_blob(&gm_blob_request, &error_code) == FAILED )
        {
          MSG_ERROR ("Geofence blob request failed: %u", error_code, 0, 0);
        }
        else
        {
          MSG_MED("Geofence blob request successful for id %u", gm_blob_request.geofence_id, 0, 0);
        }
      } 
      
      break;
    }  
      
    case GEOFENCE_CLIENT_BLOB_NOTIFY:
    {
      uint8 u_i;
      sm_gm_geofence_client_blob_notification_type *p_gm_blob_notification;
      supl_gm_blob                                 z_gm_blob; 
      static uint16 w_max_exiting_set_session_id = 0;
      
      p_gm_blob_notification =       
        (sm_gm_geofence_client_blob_notification_type *)msg_data_ptr;
      
      for ( u_i=0; u_i < C_MAX_GEOFENCE_NUM; u_i++)
      {
        /* find an open slot for the session */
        if ( z_supl_area_event_session_status[u_i].u_session_active == FALSE )
        {
          /* slot found, take this slot */
          z_supl_area_event_session_status[u_i].u_session_active = TRUE;
          
          memscpy(&z_gm_blob, sizeof(z_gm_blob),
                 &p_gm_blob_notification->geofences_client_blob_info.client_blob[0],
                 sizeof(p_gm_blob_notification->geofences_client_blob_info.client_blob));
                 
          z_supl_area_event_session_status[u_i].q_supl_version =
            z_gm_blob.q_supl_version;   
            
          z_supl_area_event_session_status[u_i].z_session_id.z_Set_Sess_Id.w_Session_Id =
            z_gm_blob.w_SetSessionID;     
            
          if (z_gm_blob.w_SetSessionID >  w_max_exiting_set_session_id )
          {
            w_max_exiting_set_session_id = z_gm_blob.w_SetSessionID;
          }  
          w_SetSessionID = w_max_exiting_set_session_id + 1;     
                
          memscpy (&z_supl_area_event_session_status[u_i].z_session_id,
                   sizeof(z_supl_area_event_session_status[u_i].z_session_id),
                  &z_gm_blob.z_session_id, sizeof(z_gm_blob.z_session_id));
                  
          z_supl_area_event_session_status[u_i].u_geofence_id_valid = TRUE;
          z_supl_area_event_session_status[u_i].q_geofence_id =
            p_gm_blob_notification->geofences_client_blob_info.geofence_id;          
            
          MSG_MED("Geofence blob for id %u taking slot %u", 
                   p_gm_blob_notification->geofences_client_blob_info.geofence_id, 
                   u_i, 0 );  
                   
          break; /* lowest numbered slot found and taken, exit the for-loop */         
        } 
      }
      break; 
    }
      
    default:
      break;
  }

  return ret_val;
}

#endif /* FEATURE_CGPS_UMTS_UP_SUPL */

