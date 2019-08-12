/*======================================================================

               PDCOMM Application task internal header file

 GENERAL DESCRIPTION
  This file contains MACROS and data-structures internally used by 
  PDCOMM Application.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/lm/src/lm_tm.h#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 06/07/13   rh    Removed NO_FIX report handling (now BestAvailPos) 
 08/01/07   rw    Add support for GPS time info req/resp
 07/31/07   ank   Added Dynamic QoS and Override logic.
 11/16/06   mr    Added function to send  PD_API GPS_BEGIN/END events to TM 
 06/20/06   mr    Initial creation of header file.
======================================================================*/

#ifndef LM_TM_H
#define LM_TM_H

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "lm_api.h"
#include "sm_api.h"
#include "tm_lm_iface.h"
#include "aries_os_api.h"

/****************** Constants ************************************************/



/****************** Constants/Enums *******************************************/


/****************** Data Structures*********************************************/



/****************** Global variables *************************/


/****************** Function declarations *******************/


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

boolean lm_send_tm_session_update_ack(uint32 q_handle, tm_status_cause_e_type e_Cause);

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

boolean lm_send_tm_session_update_nack(uint32 q_handle, tm_status_cause_e_type e_Cause);


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

boolean lm_send_tm_session_request_error_notify(uint32 q_handle, tm_session_error_cause_e_type e_Cause);


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
                                         boolean u_finalReport);

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
boolean lm_tm_send_sa_data_request_to_tm(void);

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
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_prm_report_to_tm(const sm_GpsMeasRptStructType* const p_measReport, boolean u_late);


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
boolean lm_tm_send_ppm_report_to_tm(const srch_ppm_rpt_type* const p_ppmReport);

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
 * p_assistStatusReport  - Assistance data report to be sent to TM
 * u_forKeepWarm         - Is the status a part of keep warm process
 *
 * Return value: 
 *
 * boolean - update succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_assist_data_status_report_to_tm( const sm_InternalAssistDataStatusStructType* const p_assistStatusReport,
                                                    boolean u_forKeepWarm);

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
(const sm_GnssFixRptStructType* const p_pos_estimate);

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
 * p_RfInfo   - RF INFO to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_rf_info_to_tm
(sm_ReportRfInfoStructType* p_RfInfo);

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
(tm_pd_api_event_e_type  e_pd_api_event);


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
( sm_ReportGpsTimeStructType *p_gps_time_report );

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
 * p_mgp_event_info   - MGP event info to be sent to TM
 *
 * Return value: 
 *
 * boolean - info succesfully sent or not
 *
 ******************************************************************************
*/
boolean lm_tm_send_mgp_event_to_tm
(sm_ReportEventStructType* p_mgp_event_info);

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
void lm_tm_clear_session(void);


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
void lm_tm_start_new_session(const lm_session_request_info_s_type* const p_startReq);


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
void lm_tm_stop_session(void);


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

void lm_tm_handle_session_request_start(const lm_session_request_info_s_type* const p_startReq);


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

void lm_tm_handle_session_request_stop(const lm_session_request_info_s_type* const p_stopReq);


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

void lm_tm_handle_session_request_continue(const lm_session_request_info_s_type* const p_continueReq);

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

void lm_tm_handle_session_request(const os_IpcMsgType* const p_msg);


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

void lm_tm_handle_request(const os_IpcMsgType* const p_msg);


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

void lm_tm_handle_info(const os_IpcMsgType* const p_msg);

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
void lm_tm_handle_final_position_report(const sm_GnssFixRptStructType* const p_fixReport);


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
void lm_tm_handle_set_parameter(const lm_request_set_param_s_type* const p_Req);

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
void lm_tm_handle_nmea_on(const lm_request_nmea_s_type* const p_NmeaOnRequest);


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
void lm_tm_handle_nmea_off(void);


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
void lm_tm_handle_fix_timer_timeout(void);

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
void lm_tm_handle_gps_timer_timeout(void);

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
void lm_tm_handle_cdma_timer_timeout(void);


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
void lm_tm_handle_goingOn_timer_timeout(void);

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
void lm_tm_handle_goingOff_timer_timeout(void);

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
void lm_tm_handle_goingIdle_timer_timeout(void);

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
boolean lm_tm_send_sv_poly_report_to_tm(const sm_SvPolyReportType* const p_svPolyReport);

#endif /* LM_TM_H */
