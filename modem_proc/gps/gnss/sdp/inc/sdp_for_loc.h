#ifndef __SDP_QMI_LOC_H__
#define __SDP_QMI_LOC_H__

/*============================================================================
  @file sdp_qmi_loc.h

            API between SDP and QMI_LOC

GENERAL DESCRIPTION

  This file defines the API betwen the LocMW task and the Sensor Data Processor
  module/task.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sdp/inc/sdp_for_loc.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/11   mac     Initial version

=============================================================================*/

#define SDP_FOR_LOC_VERSION 1

/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "location_service_v02.h"


/**
 * Default error code used for all handlers to check for error 
 * conditions 
 */
#define SDP_ERROR_CODE_SUCCESS (0)
#define SDP_ERROR_CODE_DEFAULT (1)
#define SDP_ERROR_CODE_UNSUPPORTED (2)

/* ---------------------------------------------------------------------------
 * Data type definitions
 * ---------------------------------------------------------------------------*/
#ifdef FEATURE_CGPS_PLAYBACK
/** Up-revved (only for IPCs) Request Message; Used by the control point to inject time sync data. */
typedef struct {

  qmiLocInjectTimeSyncDataReqMsgT_v02 z_req; /* inject time-sync data request */
  uint64 t_ttms;                             /* current time-tick msec */

} sdp_qmiLocInjectTimeSyncDataReqMsgT; 

/** Up-revved (only for IPCs) Request Message; Used by the control point to inject sensor data. */
typedef struct {

  qmiLocInjectSensorDataReqMsgT_v02 z_req; /* inject sensor data request */
  uint64 t_ttms;                           /* current time-tick msec */

} sdp_qmiLocInjectSensorDataReqMsgT; 
#endif /* FEATURE_CGPS_PLAYBACK */

/* ---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ---------------------------------------------------------------------------*/

/**
 * called from LocMW task to handle a QMI command set_spi_status.
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 sdp_qmiloc_set_spi_status_req_proxy (int loc_client_handle,
                                            const uint32 transaction_id,
                                            const qmiLocSetSpiStatusReqMsgT_v02 * const p_req);

/**
 * called from LocMW task to handle a QMI command get cradle mount config.
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 sdp_qmiloc_get_cradle_mount_config_req_proxy (int loc_client_handle,
                                                     const uint32 transaction_id);

/**
 * called from LocMW task to handle a QMI command get external power config.
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 sdp_qmiloc_get_external_power_config_req_proxy (int loc_client_handle,
                                                       const uint32 transaction_id);

/**
 * called from LocMW task to handle a QMI command inject_time_sync_data.
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 sdp_qmiloc_set_cradle_mount_config_req_proxy (int loc_client_handle,
                                                     const uint32 transaction_id,
                                                     const qmiLocSetCradleMountConfigReqMsgT_v02 * const p_req);

/**
 * called from LocMW task to handle a QMI command inject_time_sync_data.
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 sdp_qmiloc_set_external_power_config_req_proxy (int loc_client_handle,
                                                       const uint32 transaction_id,
                                                       const qmiLocSetExternalPowerConfigReqMsgT_v02 * const p_req);

/**
 * called by LocMW task to reset the state machine in case of timeout or some other error
 *
 * @return 0 if it's accepted
 */
uint32 sdp_qmiloc_reset_proxy (void);

/**
 * called by LocMW task to stop SDP processing
 * this is called when tracking mode is stoped by HLOS
 *
 * @return 0 if the stop request is sent
 */
uint32 sdp_qmiloc_stop_processing_proxy (void);

/**
 * called under LocAPI task context to create an ipc message
 * and send to SDP task in response to a set sensor control config
 * QMI req
 *
 * @param loc_client_handle
 *                  LocAPI client handle
 * @param p_req QMI request data
 * @return 0 if ipc has been sent successfully
 */
uint32 sdp_qmiloc_set_sensor_control_config_proxy(int loc_client_handle,
                                                  const qmiLocSetSensorControlConfigReqMsgT_v02 *p_req);
/**
 * called under LocAPI task context to create an ipc message
 * and send to SDP task in response to a get sensor control config
 * QMI req
 *
 * @param loc_client_handle
 *                  LocAPI client handle
 * @return 0 if ipc has been sent successfully
 */
uint32 sdp_qmiloc_get_sensor_control_config_proxy(int loc_client_handle);

/**
 * called under LocAPI task context to create an ipc message
 * and send to SDP task in response to a set sensor sensor performance
 * control config QMI req
 *
 * @param loc_client_handle
 *                  LocAPI client handle
 * @return 0 if ipc has been sent successfully
 */
uint32 sdp_qmiloc_set_sensor_performance_control_config_proxy(int loc_client_handle,
                                                              const qmiLocSetSensorPerformanceControlConfigReqMsgT_v02 *p_req);

/**
 * called under LocAPI task context to create an ipc message
 * and send to SDP task in response to a get sensor sensor performance
 * control config QMI req
 *
 * @param loc_client_handle
 *                  LocAPI client handle
 * @return 0 if ipc has been sent successfully
 */
uint32 sdp_qmiloc_get_sensor_performance_control_config_proxy(int loc_client_handle);


#endif  /* __SDP_QMI_LOC_H__ */

