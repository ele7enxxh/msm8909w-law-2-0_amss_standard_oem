/*============================================================================
  @file loc_for_sdp.h

            Proxy functions for SDP task to communicate with Loc MW task

GENERAL DESCRIPTION

  This file defines interfaces for SDP task to call

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/inc/loc_for_sdp.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/11   mac     Initial version

=============================================================================*/

#ifndef _LOC_FOR_SDP_H_
#define _LOC_FOR_SDP_H_

#include "location_service_v02.h"
#include "locEng_sm_common_msg_v01.h"

#define LOC_FOR_SDP_VERSION 2

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_set_spi_status_ind_proxy (const int32 client_handle, 
                                         const uint32 transaction_id, 
                                         const qmiLocSetSpiStatusIndMsgT_v02 * const p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_get_cradle_mount_config_ind_proxy (const int32 client_handle, 
                                                  const uint32 transaction_id, 
                                                  const qmiLocGetCradleMountConfigIndMsgT_v02 * const p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_get_external_power_config_ind_proxy (const int32 client_handle, 
                                                    const uint32 transaction_id, 
                                                    const qmiLocGetExternalPowerConfigIndMsgT_v02 * const p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_set_cradle_mount_config_ind_proxy (const int32 client_handle, 
                                                  const uint32 transaction_id, 
                                                  const qmiLocSetCradleMountConfigIndMsgT_v02 * const p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_set_external_power_config_ind_proxy (const int32 client_handle, 
                                                    const uint32 transaction_id, 
                                                    const qmiLocSetExternalPowerConfigIndMsgT_v02 * const p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_event_switch_spi_streaming_report_ind_proxy (const qmiLocEventSetSpiStreamingReportIndMsgT_v02 * const p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_set_sensor_properties_ind_proxy(const int32 client_handle,
 const uint32 transaction_id,
 const qmiLocSetSensorPropertiesIndMsgT_v02 *p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_get_sensor_properties_ind_proxy(const int32 client_handle,
 const uint32 transaction_id,
 const qmiLocGetSensorPropertiesIndMsgT_v02 *p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_set_sensor_control_config_ind_proxy(const int32 client_handle,
 const uint32 transaction_id,
 const qmiLocSetSensorControlConfigIndMsgT_v02 *p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_get_sensor_control_config_ind_proxy(const int32 client_handle,
 const uint32 transaction_id,
 const qmiLocGetSensorControlConfigIndMsgT_v02 *p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_set_sensor_performance_control_config_ind_proxy(const int32 client_handle,
 const uint32 transaction_id,
 const qmiLocSetSensorPerformanceControlConfigIndMsgT_v02 *p_ind);

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 * 
 * @param [in] client_handle
 *               LocAPI client handle
 * @param [in] transaction_id
 *               same transaction id passed in earlier by the request
 * @param [in] p_ind  the QMI indication
 * 
 * @return 0 if operation is successful
 */
uint32 loc_sdp_get_sensor_performance_control_config_ind_proxy(const int32 client_handle,
 const uint32 transaction_id,
 const qmiLocGetSensorPerformanceControlConfigIndMsgT_v02 *p_ind);

/**
 * called by SDP task to send status report for set navigation 
 * config request to LocMW task
 *
 * @param [in] p_ind  the LocEng format response indication
 *
 * @return 0 if operation is successful
 */
uint32 loc_sdp_set_navigation_config_resp_ind_proxy (
  const locEngSetNavigationConfigRespMsgT_v01 * const p_ind);

/**
 * called by SDP task to send status report for get navigation 
 * config request to LocMW task  
 *
 * @param [in] p_ind  the LocEng format response indication
 *
 * @return 0 if operation is successful
 */
uint32 loc_sdp_get_navigation_config_resp_ind_proxy (
  const locEngGetNavigationConfigRespMsgT_v01 * const p_ind);

#endif // #ifndef _LOC_FOR_SDP_H_

