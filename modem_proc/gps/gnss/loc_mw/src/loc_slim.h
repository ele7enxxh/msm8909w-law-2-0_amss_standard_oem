#ifndef LOC_SLIM_H
#define LOC_SLIM_H
/*============================================================================
  @file loc_slim.h
 
  Functions for SLIM module to communicate with Loc MW task
 
               Copyright (c) 2014 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_slim.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "aries_os_api.h"
#include "location_service_v02.h"
#include "loc_client.h"

/* ---------------------------------------------------------------------------
 * ..._req called by shim layer to process requests from QMI
 * ..._handler called by loc_task to process IPC from SLIM, or other, task
 * ---------------------------------------------------------------------------*/

/**
 * called from LOC-MW task in response to a QMI command to
 * inject sensor data. This is a proxy because it
 * runs under the caller task context, while the actual
 * processing would happen later in corresponding handler
 * function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to
 * SLIM for further processing
 */
int32 loc_qmiloc_inject_sensor_data_req(loc_client_info_s_type *const client_ptr,
                                        const qmiLocInjectSensorDataReqMsgT_v02 *const pReq,
                                        qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task to process an incoming Inject Sensor
 * Data indication IPC. Function will unpack the IPC and
 * forward it over QMI Loc to any clients.
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_qmiloc_inject_sensor_data_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * called from LOC-MW task in response to a QMI command to
 * inject time sync data. This is a proxy because it
 * runs under the caller task context, while the actual
 * processing would happen later in corresponding handler
 * function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to
 * SLIM for further processing
 */
int32 loc_qmiloc_inject_time_sync_data_req(loc_client_info_s_type *const client_ptr,
                                           const qmiLocInjectTimeSyncDataReqMsgT_v02 *const pReq,
                                           qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task to process an incoming Inject Time
 * Sync Data indication IPC. Function will unpack the IPC and
 * forward it over QMI Loc to any clients.
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_qmiloc_inject_time_sync_data_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/* ---------------------------------------------------------------------------
 * called by internal LocMW, like PDAPI event call backs to broadcast events,
 * directly through QCSI
 * ---------------------------------------------------------------------------*/

/**
 * handle QMI command to report pedometer data
 *
 * @param client_ptr  Loc MW client info 
 * @param pReq        pedometer report payload  
 * @param pResp       generic response from this function 
 *
 * @return TRUE : If the request has passed parameter 
 *                checking and an ipc sent to SLIM for
 *                further processing
 *         FALSE: Otherwise
 *
 */
boolean loc_slim_pedometer_report_req(
  loc_client_info_s_type *const client_ptr,
  const qmiLocPedometerReportReqMsgT_v02 *pReq,
  qmiLocGenRespMsgT_v02 *const pResp);

/**
 * handle QMI indication containing the status for pedometer 
 * report request , called from loc MW task context, sends 
 * QMI_LOC indication back. 
 *  
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_slim_pedometer_report_ind_handler(
  const os_IpcMsgType *const p_ipc_msg_in);


/**
 * handle QMI command to inject motion data
 *
 * @param client_ptr  Loc MW client info 
 * @param pReq        motion data payload  
 * @param pResp       generic response from this function 
 *
 * @return TRUE : If the request has passed parameter 
 *                checking and an ipc sent to SLIM for
 *                further processing
 *         FALSE: Otherwise
 *
 */
boolean loc_slim_inject_motion_data_req(
  loc_client_info_s_type *const client_ptr,
  const qmiLocInjectMotionDataReqMsgT_v02 *pReq,
  qmiLocGenRespMsgT_v02 *const pResp);

/**
 * handle QMI indication containing the status for pedometer 
 * report request , called from loc MW task context, sends 
 * QMI_LOC indication back. 
 *  
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */

void loc_slim_inject_motion_data_ind_handler(
  const os_IpcMsgType *const p_ipc_msg_in);

/**
 * called from LOC-MW task in response to a QMI command to
 * inject vehicle sensor data. This is a proxy because it
 * runs under the caller task context, while the actual
 * processing would happen later in corresponding handler
 * function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to
 * SLIM for further processing
 */
int32 loc_slim_inject_vehicle_sensor_data_req(loc_client_info_s_type *const client_ptr,
                                             const qmiLocInjectVehicleSensorDataReqMsgT_v02 *const pReq,
                                             qmiLocGenRespMsgT_v02 *const pResp);



/**
 * called from LOC-MW task to process an incoming Vehicle Sensor
 * Injection response indication IPC. Function will unpack the 
 * IPC and forward it over QMI Loc to any clients. 
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_slim_inject_vehicle_sensor_data_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * called from LOC-MW task in response to a QMI command to notify
 * the sensor control configuration to SLIM.
 *
 * @param p_req  QMI request
 */
void loc_slim_set_sensor_control_config_req(const qmiLocSetSensorControlConfigReqMsgT_v02 *p_req);

#endif /* #ifndef LOC_SLIM_H */

