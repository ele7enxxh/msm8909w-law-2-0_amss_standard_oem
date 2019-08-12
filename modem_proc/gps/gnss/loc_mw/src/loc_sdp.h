/*============================================================================
  @file loc_sdp.h

            Handlers for ISAGNAV related commands and IPC messages

GENERAL DESCRIPTION

  This file defines the handlers for ISAGNAV related QMI commands and IPC messages.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

            Copyright (c) 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved
            Qualcomm Confidential and Proprietary
 
            Copyright (c) 2012-2014 Qualcomm Atheros Incorporated.
            All Rights Reserved
            Qualcomm Atheros Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_sdp.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/11   mac     Initial version

=============================================================================*/

#ifndef _LOC_SDP_H_
#define _LOC_SDP_H_

#include "location_service_v02.h"

/* ---------------------------------------------------------------------------
 * ..._req called by shim layer to process requests from QMI
 * ..._handler called by loc_task to process IPC from SDP, or other, task
 * ---------------------------------------------------------------------------*/

/**
 * called from LOC-MW task in response to a QMI command to
 * set the stationary position indicator. This is a proxy because it
 * runs under the caller task context, while the actual
 * processing would happen later in corresponding handler
 * function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_set_spi_status_req(loc_client_info_s_type *const client_ptr,
                                    const qmiLocSetSpiStatusReqMsgT_v02 *const pReq,
                                    qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task to process an incoming Set SPI status
 * indication IPC. Function will unpack the IPC and forward it
 * over QMI Loc to any clients.
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_qmiloc_set_spi_status_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

// eQMI_LBS_CMD_EXTERNAL_SPI_REQ_IND
// need to send an IPC to SDP task, when you see this event PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION
int32 loc_qmiloc_set_spi_streaming_report(loc_client_info_s_type *const client_ptr, const boolean flag);

/**
 * called from LOC-MW task in response to a QMI command to get
 * the cradle mount config. This is a proxy because it runs
 * under the caller task context, while the actual processing
 * would happen later in corresponding handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_get_cradle_mount_config_req(loc_client_info_s_type *const client_ptr,
                                             qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task to process an incoming Get Cradle
 * Mount Config indication IPC. Function will unpack the
 * IPC and forward it over QMI Loc to any clients.
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_qmiloc_get_cradle_mount_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * called from LOC-MW task in response to a QMI command to get
 * the external power config. This is a proxy because it runs
 * under the caller task context, while the actual processing
 * would happen later in corresponding handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_get_external_power_config_req(loc_client_info_s_type *const client_ptr,
                                               qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task to process an incoming Get External
 * Power Config indication IPC. Function will unpack the
 * IPC and forward it over QMI Loc to any clients.
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_qmiloc_get_external_power_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * called from LOC-MW task in response to a QMI command to set
 * the cradle mount config. This is a proxy because it runs
 * under the caller task context, while the actual processing
 * would happen later in corresponding handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_set_cradle_mount_config_req(loc_client_info_s_type *const client_ptr,
                                             const qmiLocSetCradleMountConfigReqMsgT_v02 *const pReq,
                                             qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task to process an incoming Set Cradle
 * Mount Config indication IPC. Function will unpack the
 * IPC and forward it over QMI Loc to any clients.
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_qmiloc_set_cradle_mount_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * called from LOC-MW task in response to a QMI command to set
 * the external power config. This is a proxy because it runs
 * under the caller task context, while the actual processing
 * would happen later in corresponding handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_set_external_power_config_req(loc_client_info_s_type *const client_ptr,
                                               const qmiLocSetExternalPowerConfigReqMsgT_v02 *const pReq,
                                               qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task to process an incoming Set External
 * Power Config indication IPC. Function will unpack the IPC and
 * forward it over QMI Loc to any clients.
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_qmiloc_set_external_power_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * called from LOC-MW task to process an incoming SPI streaming
 * report indication IPC. Function will unpack the IPC and
 * forward it over QMI Loc to any clients.
 *
 * @param p_ipc_msg_in IPC message to process
 *
 * @return None
 */
void loc_sdp_event_switch_spi_streaming_report_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/* ---------------------------------------------------------------------------
 * called by internal LocMW, like PDAPI event call backs to broadcast events, directly through QCSI
 * note that by calling these functions, no information is sent to SDP task
 * ---------------------------------------------------------------------------*/

/**
 * called from LOC-MW task to send an event to enable/disable
 * SPI injections from QMI Loc clients.
 *
 * @param ptr_ind Event indication to send to QMI Loc Clients
 *
 * @return 0 if the request has passed parameter checking and a
 *         QMI message is sent out to clients
 */
uint32 loc_sdp_send_event_switch_spi_streaming_report_ind(const qmiLocEventSetSpiStreamingReportIndMsgT_v02 *const ptr_ind);

/**
 * called from LOC-MW task in response to a QMI command to set
 * the sensor properties. This is a proxy
 * because it runs under the caller task context, while the
 * actual processing would happen later in corresponding
 * handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_set_sensor_properties_req(loc_client_info_s_type *const client_ptr,
                                           const qmiLocSetSensorPropertiesReqMsgT_v02 *p_req,
                                           qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task in response to a QMI command to get
 * the sensor properties. This is a proxy
 * because it runs under the caller task context, while the
 * actual processing would happen later in corresponding
 * handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_get_sensor_properties_req(loc_client_info_s_type *const client_ptr,
                                           const qmiLocGetSensorPropertiesReqMsgT_v02 *p_req,
                                           qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task in response to a QMI command to set
 * the sensor control configuration. This is a proxy
 * because it runs under the caller task context, while the
 * actual processing would happen later in corresponding
 * handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_set_sensor_control_config_req(loc_client_info_s_type *const client_ptr,
                                               const qmiLocSetSensorControlConfigReqMsgT_v02 *p_req,
                                               qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from LOC-MW task in response to a QMI command to get
 * the sensor performance control configuration. This is a proxy
 * because it runs under the caller task context, while the
 * actual processing would happen later in corresponding
 * handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_get_sensor_control_config_req(loc_client_info_s_type *const client_ptr,
                                               qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from SDP task to indicate success or failure of a QMI
 * command to get the sensor performance control configuration.
 * This is a proxy because it runs under the caller
 * task context, while the actual processing would happen later
 * in corresponding handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return LOC_API_SUCCESS if the request has passed parameter
 *         checking and an ipc sent to SDP task for further
 *         processing
 */
int32 loc_qmiloc_set_sensor_performance_control_config_req(loc_client_info_s_type *const client_ptr,
                                                           const qmiLocSetSensorPerformanceControlConfigReqMsgT_v02 *p_req,
                                                           qmiLocGenRespMsgT_v02 *const pResp);

/**
 * called from SDP task to indicate success or failure of a QMI
 * command to get the sensor performance control configuration.
 * This is a proxy because it runs under the caller
 * task context, while the actual processing would happen later
 * in corresponding handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param pResp generic response from this function
 *
 * @return LOC_API_SUCCESS if the request has passed parameter
 *         checking and an ipc sent to SDP task for further
 *         processing
 */
int32 loc_qmiloc_get_sensor_performance_control_config_req(loc_client_info_s_type *const client_ptr,
                                                           qmiLocGenRespMsgT_v02 *const pResp);

/**
 * handle QMI command to set sensor control
 * configuration
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_set_sensor_control_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * handle QMI command to get sensor control
 * configuration
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_get_sensor_control_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * handle QMI command to set sensor properties
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_set_sensor_properties_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * handle QMI command to get sensor properties
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_get_sensor_properties_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * handle QMI command to set sensor performance control
 * configuration
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_set_sensor_performance_control_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

/**
 * handle QMI command to get sensor performance control
 * configuration
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_get_sensor_performance_control_ind_handler(const os_IpcMsgType *const p_ipc_msg_in);

// PDSI_CMD_VAL_GET_NAV_CONFIG
// PDSI_CMD_VAL_SET_NAV_CONFIG

#endif // #ifndef _LOC_SDP_H_

