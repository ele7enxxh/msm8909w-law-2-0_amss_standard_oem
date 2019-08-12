/******************************************************************************
  @file:  loc_pd.h
  @brief: Location Middleware PD module header

  DESCRIPTION
      This module handles the interaction with the PD-engine of GPS.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/03/15   yh       Add loc_fix_criteria_rule to hold the best fix criteria for multiple client
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
03/10/14   ah       Added support for Get Avail WWAN Pos
03/14/13   ssu      Adding support for SUPL MT ES requests.
04/27/11   spn      Updated for Loc-Api 2.0
$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_pd.h#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/


#ifndef LOC_PD_H
#define LOC_PD_H

#include "loc_api_2.h"
#include "pdapi.h"
#include "loc_task.h"
#include "loc_client.h"
#include "aries_os_api.h"

// sometimes the engine would burst at least two measurement reports within very short time
// as every measurement report is converted into a set of NMEA sentences, we have to be able
// to buffer at least 2 sets
#define NMEA_BUFFER_MAX_SIZE 20

typedef enum  {
    LOC_SUPL_MT_IDLE,
    LOC_SUPL_MT_PO_ABOUT_TO_START,
    LOC_SUPL_MT_PO_IN_PROGRESS,
    LOC_SUPL_MT_ES_ABOUT_TO_START,
    LOC_SUPL_MT_ES_IN_PROGRESS
}loc_supl_mt_state_e_type;

/* Defines the payload of the LOC_CMD_TYPE_REQUEST_FIX IPC
   It enumerates the different conditions when the locMW
   process_fix_request can be called.
*/
typedef enum
{
  eLOC_PD_START_REQUEST,
  eLOC_PD_STOP_REQUEST,
  eLOC_PD_SCHEDULE,
  eLOC_PD_FIX_TIMEOUT
}loc_pd_fix_request_e_type;

typedef enum
{
    LOC_SUPL_REQ_ACCEPT,
    LOC_SUPL_REQ_DENY,
    LOC_SUPL_REQ_DROP
}loc_supl_req_e_type;

typedef struct
{
   boolean                 fix_in_progress;
   boolean                 mo_fix_in_progress;
   boolean                 position_injection_in_progress;
   uint64                  last_get_position_time;
   uint64                  last_fix_arrival_time;
   uint32                  last_fix_ttf;
   loc_supl_mt_state_e_type    supl_mt_fix_state;
   pdsm_pd_option_s_type       pdsm_option;
   pdsm_pd_qos_type            pdsm_qos;
   boolean                     new_pd_info_received;
   boolean                     new_ext_status_info_received;
   boolean                     new_gnss_meas_info_received;
   boolean                     new_sv_poly_info_received;   
   boolean                     new_interm_pos_info_received;
   int32                       new_nmea_messages_counter;
   boolean                     new_nmea_info_received;
   boolean                     new_pd_sess_end_status_received;
   boolean                     new_pd_cmd_err_received;
   pdsm_pd_info_s_type         pd_info, pd_info_copy;
   pdsm_ext_status_info_s_type ext_status_info, ext_status_info_copy;
   pdsm_ext_status_info_s_type interm_pos_info, interm_pos_info_copy;
   pdsm_ext_status_info_s_type nmea_info_buffer_in[NMEA_BUFFER_MAX_SIZE];
   pdsm_ext_status_info_s_type gnss_meas_info,gnss_meas_info_copy;
   pdsm_ext_status_info_s_type sv_poly_info,sv_poly_info_copy;   
   pdsm_pd_end_e_type          pd_sess_end_status, pd_sess_end_status_copy;
   pdsm_pd_cmd_err_e_type      pd_cmd_err, pd_cmd_err_copy;
   pdsm_pd_event_f_type       *pd_event_cb_hook;

   os_TimerMsgType *           fix_process_timer;

   loc_client_handle_type      active_client_handle;
   loc_client_handle_type      pos_inject_client_handle;
   loc_client_handle_type      motion_data_inject_client_handle;
   loc_engine_state_e_type     engine_state;
   loc_event_payload_u_type    position_payload;
   loc_event_payload_u_type    gnss_payload;
   loc_event_payload_u_type    nmea_payload;
   loc_event_payload_u_type    gnss_meas_payload;
   loc_event_payload_u_type    sv_poly_payload;
   // Cached cell id position or GPS fix
   loc_event_payload_u_type    cached_gnss_pos;
   uint64                      cached_gnss_pos_time;

   loc_event_payload_u_type    cached_nw_ref_pos;
   uint64                      cached_nw_ref_pos_time;

   // timeout value used by loc mw layer, if a position report is not received
   // within the timeout value, loc mw will reset its state and allow subsequent
   // pdsm_get_position calls
   uint32                     session_time_out_ms;

   /* client which requested the best available position */
   loc_client_handle_type     bap_client_handle;

   /* variable to serialize the bap request, current implementation
      does not support concurrent requests for best available position*/
   boolean                    get_bap_in_progress;

   /* transaction id fo the get best available position request */
   uint32                     get_bap_txn_id;

   /* client which is currently handling the wifi ap scan info injection */
   loc_client_handle_type     wifi_ap_scan_client_handle;

   /* variable to serialize the wifi ap scan injection one client at a time */
   boolean                    wifi_ap_scan_in_progress;

   /* client which requested the available WWAN position */
   loc_client_handle_type     wwan_pos_client_handle;

   /* variable to serialize the available WWAN position request,
      current implementation does not support concurrent requests */
   boolean                    get_wwan_pos_in_progress;

   /* transaction id for the available WWAN position request */
   uint32                     get_wwan_pos_txn_id;


   /* client which requested the secure available  position */
   loc_secure_get_available_location     sec_avail_pos_client_payload;

   /* hold the best fix criteria for multiple clients */
   loc_fix_criteria_s_type    loc_fix_criteria_rule;

} loc_pd_module_data_s_type;

// Initialzie the location position determination module
extern boolean loc_pd_init (void);

// Initialize the location nmea moduel
extern boolean loc_pd_init_nmea (void);

// Process IOCTL for the position determination module
extern int loc_pd_process_ioctl (loc_client_handle_type       client_handle,
                                 loc_ioctl_e_type             ioctl_type,
                                 const loc_ioctl_data_u_type* ioctl_data_ptr);

// Process cmd request coming from the location middleware task
extern int loc_pd_process_cmd_request (loc_cmd_s_type * const ptr_cmd);

// Translate fix configuration from PD api to location API
extern void loc_pd_xlate_fix_config_from_locapi_to_pdapi (loc_client_info_s_type* client_ptr);

// Function to be called from loc client module to process cell id fix request from cached position
extern void loc_pd_process_concurrent_cellid_request (void);

// Installs an event callback hook for PD events (including QWIP events)
extern pdsm_client_status_e_type loc_pd_install_event_cb_hook(pdsm_pd_event_f_type *hook);

// Handles the service interaction for CP/UP, NI/MO.
extern void loc_pd_handle_service_interaction (pdsm_lcs_notification_resp_s_type* resp_info_ptr);

extern loc_supl_req_e_type loc_pd_continue_processing_supl_request (loc_ni_supl_notify_verify_req_s_type* const supl_req_ptr);

//processes a pd_fix_request
extern int loc_pd_process_fix_request (loc_pd_fix_request_e_type fix_request_type);


/** loc_pd_event_wifi_ap_scan_info_inject_ind_handler
 *  
 *  Handle to send QMI Event IND to the client from LocMW.
 *  
 *  @param [in] pIpcMsg  Pointer to ipc message containing the
 *         indication
 *
 * @return 
 * TRUE - if the QMI IND has been sent successfully TRUE 
 * FALSE - if the QMI IND send failed 
 * */
extern void loc_pd_event_wifi_ap_scan_info_inject_ind_handler(const os_IpcMsgType*   const pIpcMsg);


/** loc_pd_inject_wifi_ap_scan_info_ind_handler
 *  
 *  Handle to send QMI IND to the client from LocMW.
 *  
 *  @param [in] pIpcMsg  Pointer to ipc message containing the
 *         indication
 *
 * @return 
 * TRUE - if the QMI IND has been sent successfully TRUE 
 * FALSE - if the QMI IND send failed 
 * */
extern boolean loc_pd_inject_wifi_ap_scan_info_ind_handler(const os_IpcMsgType*  const pIpcMsg);


/**
 * Function to handle QMI command from control point to inject 
 * wifi ap data 
 *
 * @param client_ptr LocAPI client handle
 * @param pReq       QMI request
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
extern boolean loc_pd_process_inject_wifi_ap_data_req(loc_client_info_s_type     *const client_ptr,
                                                      const qmiLocInjectWifiApDataReqMsgT_v02  *pReq,
                                                      qmiLocGenRespMsgT_v02      *const pResp); 

/**
 * Function to to translate fix mode from Loc API to PD API.
 *
 * @param loc_operation_mode operation mode in loc api format
 *
 * @return operation mode in pdapi format
 */
extern pdsm_pd_session_operation_e_type  loc_pd_xlate_fix_mode_from_locapi_to_pdapi
(
      qmiLocOperationModeEnumT_v02    loc_operation_mode
);

#endif // LOC_PD_H
