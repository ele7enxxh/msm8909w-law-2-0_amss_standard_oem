 /*======================================================================

                         Location Manager MGP interaction file

 GENERAL DESCRIPTION
  This file contains functions by which Location Manager(LM) handles all 
  the MGP related processing. 

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

  $Header $$DateTime $$Author $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
06/07/13   rh    Removed NO_FIX report handling (now BestAvailPos) 
02/10/10   vp    OEM DR Data Enablement
01/12/10   gk    Force receiver off when end session is called
06/12/08   lt    Support for external coarse position injection.
08/01/07   rw    Add support for XTRA feature, GPS time info
06/20/06   mr    Initial creation of header file.
======================================================================*/
#ifndef LM_MGP_H
#define LM_MGP_H


#include "lm_api.h"
#include "mgp_api.h"
#include "lm_data.h"


/****************** Data Structures*********************************************/
typedef PACKED struct PACKED_POST
{
  uint32 q_GpsRtcMs; 
  /*RTC MilliSeconds*/
  uint32 q_receiver_state;
  /*Receiver state enum*/
}lm_recvr_state_event_info;

/****************** Function Definitions**************************************/

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
mgp_ConfigOperationEnumType lm_mgp_get_mgp_operation_mode(lm_gps_mode_e_type e_lm_mode);

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
								boolean u_Forced);

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
boolean lm_mgp_start_position_fix_processing(const lm_session_request_fix_s_type* const p_FixReq);

/*
 ******************************************************************************
 * lm_mgp_stop_position_fix_processing 
 *
 * Function description:
 *
 * This function stops a position fix processing. 
 *
 * Parameters: 
 * u_ForceMgpOff - Forces MGP to turn off immediately
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_stop_position_fix_processing(boolean u_forceMgpOff);


/*
 ******************************************************************************
 * lm_mgp_continue_fix_processing 
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
boolean lm_mgp_continue_position_fix_processing(const lm_session_request_fix_s_type* const p_FixReq);

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
boolean lm_mgp_start_prm_processing(const lm_session_request_gps_s_type* const p_GpsReq);


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
boolean lm_mgp_aa_data_received( void );


/*
 ******************************************************************************
 * lm_mgp_stop_prm_processing 
 *
 * Function description:
 *
 * This function stops PRM processing. 
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
void lm_mgp_stop_prm_processing(void);


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
boolean lm_mgp_continue_prm_processing(const lm_session_request_gps_s_type* const p_GpsReq);

/*
 ******************************************************************************
 * lm_meas_status_bits_check
 *
 * Function description:
 *
 * Function checking SV measurement status bits to determin if the SV is
    to be included in the measrement report to the network
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
boolean lm_meas_status_bits_check(uint32 q_sv_meas_status, uint16 w_Cno, gnss_SvStateEnumType e_SvState);

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
void lm_mgp_clean_meas_rpt(sm_GpsMeasRptStructType* p_measReport);
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
void lm_mgp_handle_gps_meas_done(os_IpcMsgType* p_msg);

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
void lm_mgp_handle_gps_meas(const os_IpcMsgType* p_msg);

/*
 ******************************************************************************
 * lm_mgp_handle_sv_poly_report
 *
 * Function description:
 *
 * This function handles the SV poly report from MGP
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
void lm_mgp_handle_sv_poly_report(os_IpcMsgType* p_msg);

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
void lm_mgp_handle_fix_report(os_IpcMsgType* p_msg);

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
void lm_mgp_handle_pos_estimate_report(os_IpcMsgType* p_msg);

/*
 ******************************************************************************
 * lm_mgp_handle_utc_info
 *
 * Function description:
 *
 * This function handles the UTC info decoded off the air by MGP
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
void lm_mgp_handle_utc_info(const sm_UtcInfoRptStructType *p_newUtcInfo);


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
void lm_mgp_rcvr_state_change(lm_mgp_state_e_type e_mgpState);

/*
 ******************************************************************************
 * lm_mgp_handle_status_update
 *
 * Function description:
 *
 * This function handles the MGP status updated coming from MGP.
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
void lm_mgp_handle_status_update(os_IpcMsgType* p_msg);

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
void lm_mgp_update_mgp_state_after_final_fix(uint32 q_num_fixes, uint32 q_tbf);

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
 * wl_src: The module that is originating the wishlist request.
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_handle_aiding_wishlist_request(lm_wishlist_src_req_e_type wl_src);

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
void lm_mgp_handle_delete_mgp_info(lm_request_delete_mgp_info_s_type *p_Req);


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
void lm_mgp_handle_aiding_data_info(lm_info_aiding_data_s_type* p_aidingData);

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
void lm_mgp_handle_assist_status_report(os_IpcMsgType* p_msg);

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
void lm_mgp_handle_rf_info_request(lm_request_rf_info_s_type* p_rf_info_request);

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
void lm_mgp_handle_wbiq_info(os_IpcMsgType* p_msg);

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
void lm_mgp_handle_nbiq_info(os_IpcMsgType* p_msg);

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
void lm_mgp_handle_gnss_rf_status_info(os_IpcMsgType* p_msg);

/*
 ******************************************************************************
 * lm_mgp_handle_gps_time_request
 *
 * Function description:
 * This function handles the GPS time request from TM.
 * 
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
void lm_mgp_handle_gps_time_request(lm_request_gps_info_s_type *p_gps_time_request);


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
void lm_mgp_handle_rcOn_timer_timeout(void);


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
void lm_mgp_handle_shutOff_timer_timeout(void);

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
void lm_mgp_handle_pos_estimate_request(void);

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
void  lm_mgp_handle_pos_unc(FLT f_new_pos_unc);

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
void  lm_mgp_handle_pos_offset(uint32 q_pos_offset);

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
void  lm_mgp_handle_time_unc(FLT f_new_time_unc);

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
void  lm_mgp_handle_time_offset(FLT f_time_offset);

/*
 ******************************************************************************
 * lm_mgp_handle_session_type
 *
 * Function description:
 * This function injects session type MGP
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
void  lm_mgp_handle_session_type(lm_info_session_info_type   z_sess_info);

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
void  lm_mgp_handle_slow_clk_time_inject_request(lm_request_slow_clk_time_inject_s_type* p_slow_clk_inject);

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
(lm_request_extern_coarse_pos_inject_s_type* p_extern_coarse_pos_inj );


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
void lm_mgp_handle_rf_info(os_IpcMsgType* p_msg);

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
void lm_mgp_handle_gps_time(os_IpcMsgType* p_msg);

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
void lm_mgp_handle_request_sa_data(void);

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
void lm_mpg_handle_force_rcvr_off(void);

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
void lm_mpg_handle_rcvr_off(void);


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
void lm_mpg_handle_reset_location_service(void);

/**
* @brief  LM abstraction layer around mgp_UpdateInfo. This
*         abstratcion implements a "shim" to enable backwards
*         compatibility with GEN7 MGP.
  
  @return      
  @sideeffects None
* @see mgp_GnssUpdateInfo() and mgp_UpdateInfo()

*/
boolean lm_mgp_update_info(gnss_AssistDataInfoStructType *p_assistDataInfo
                           /**< GEN8 assistance data received from network */);

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
   const gnss_AssistDataInfoStructType *p_Src
      /**< gen8 or newer assistance data */);
      
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
void lm_mgp_send_fix_start_ind(uint32 q_Tbf);

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
void lm_mgp_send_fix_end_ind(void);

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
void lm_debugNMEA(os_IpcMsgType* p_msg);
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
void lm_xtraInjDone(void);

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
 void lm_ReportResetLocationServiceDone(pdsm_pd_ext_reset_location_service_status_e_type *status);


#endif /* LM_MGP_H */
