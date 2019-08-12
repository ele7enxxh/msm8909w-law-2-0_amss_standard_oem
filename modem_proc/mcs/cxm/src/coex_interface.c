
/*!
  @file
  coex_interface.c

  @brief
  This file abstracts CoEx service/lib interaction with the CXM connection manager

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/coex_interface.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/29/14   ckk     COEX QMI Service mssage handler conditional removal
07/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
01/10/13   tak     Added CA and TDS support for WWAN update and sleep indication 
12/19/12   btl     Port QMI message support for power limiting, band filtering
10/02/12   btl     Add support for new metrics QMI messages
07/24/12   btl     Removed coex_msgr_process_msg()
07/05/12   ejv     Add support for mqcsi layer.
04/08/12   ckk     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "cxm_utils.h"
#include "cxm_msgr.h"
#include "mqcsi_conn_mgr.h"
#include "coex_algos.h"
#include "coex_interface.h"
#include <coexistence_service_v01.h>
#include "coexistence_service_impl_v01.h"
#include "cxm_trace.h"

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/

static qmi_csi_service_handle     *coex_service_handle = NULL;
static qmi_idl_service_object_type coex_service_object;


/*=============================================================================

                           FUNCTIONS DEFINITIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  coex_get_ind_offset

=============================================================================*/
/*!
    @brief
    This method maintains the internal mapping between the indication method
    id and offset of the message in the message map.

    @return
    int32 offset
*/
/*===========================================================================*/
int32 coex_get_ind_offset (
  const uint32            ind_msg_id  /*!< Indication whose internal offset is needed */
)
{
  int32 offset = -1;

  /*-----------------------------------------------------------------------*/

  switch( ind_msg_id )
  {
    case QMI_COEX_WWAN_STATE_IND_V01:
      offset = 0;
      break;

    case QMI_COEX_METRICS_LTE_BLER_IND_V01:
      offset = 1;
      break;

    case QMI_COEX_CONDITION_FAIL_IND_V01:
      offset = 2;
      break;

    case QMI_COEX_CONDITION_SUCCESS_IND_V01:
      offset = 3;
      break;

    case QMI_COEX_SLEEP_IND_V01:
      offset = 4;
      break;

    case QMI_COEX_WAKEUP_IND_V01:
      offset = 5;
      break;

    case QMI_COEX_WCN_WAKE_SYNC_IND_V01:
      offset = 6;
      break;

    default: /* INVALID ind_msg_id for this service */
      offset = -1;
      break;
  } /* switch( ind_msg_id )*/

  /*-----------------------------------------------------------------------*/

  return offset;
}

/*=============================================================================

  FUNCTION:  coex_service_init

=============================================================================*/
/*!
    @brief
    Initializes COEX layer's functionalities
 
    @return
    None
*/
/*===========================================================================*/
void coex_service_init (
  void
)
{
  /*-----------------------------------------------------------------------*/

  /* Setup COEX's QCSI service interface */

  /* 1. Setup service object */
  coex_service_object = coex_get_service_object_v01();

  /*2. Send the table of supported fields and messages for this branch*/
  (void) qmi_si_register_object ( coex_service_object,
                                  0, /* Service Instance */
                                  coex_get_service_impl_v01() );

  /* 3. Register/Open/Initiate service with Connection Manager infrastructure */
  coex_service_handle = mqcsi_conn_mgr_service_open (
                          MQCSI_COEX_SERVICE_ID,
                          "COEX_SERVICE",
                          &cxm_tcb,
                          CXM_COEX_QMI_SIG,
                          coex_service_object,
                          &coex_qcsi_process_req,
                          coex_get_ind_offset,
                          NULL, /* No special service options required */
                          COEX_V01_IDL_MAJOR_VERS,
                          COEX_V01_IDL_MINOR_VERS
                        );

  CXM_TRACE( CXM_TRC_QMI_SVC_INIT, (uint8)MQCSI_COEX_SERVICE_ID,
                   (uint16)COEX_V01_IDL_MAJOR_VERS, (uint32)COEX_V01_IDL_MINOR_VERS,
                   CXM_TRC_AND_PKT_EN, CXM_LOG_PARAMS_NO_TABLES, CXM_LOG_PARAMS_PORT_TABLE,
                   CXM_LOG_PARAMS_VICTIM_TBL, CXM_LOG_GLOBAL_POLICY_PARAMS, CXM_LOG_STATE );
  CXM_MSG_0( HIGH, "MQCSI_COEX_SERVICE_ID service started" );

  CXM_ASSERT( NULL != coex_service_handle );
  
  /*-----------------------------------------------------------------------*/

  return;
}

/*=============================================================================

  FUNCTION:  coex_service_deinit

=============================================================================*/
/*!
    @brief
    Cleans up/tears down the underlying transport layer functionalities

    @return
    None
*/
/*===========================================================================*/
void coex_service_deinit (
  void
)
{

  qmi_csi_error estatus;

  /*-----------------------------------------------------------------------*/

  /* Teardown COEX's QCSI service interface */

  /* 1. Deregister/Close/Deinit service with Connection Manager infrastructure */
  estatus = mqcsi_conn_mgr_service_close( MQCSI_COEX_SERVICE_ID );
  CXM_MSG_0( HIGH, "MQCSI_COEX_SERVICE_ID service ended" );
  CXM_TRACE( CXM_TRC_QMI_SVC_DEINIT, (uint8)MQCSI_COEX_SERVICE_ID, estatus, 0,
             CXM_TRC_AND_PKT_EN, CXM_LOG_PARAMS_NO_TABLES, CXM_LOG_PARAMS_PORT_TABLE,
             CXM_LOG_PARAMS_VICTIM_TBL, CXM_LOG_GLOBAL_POLICY_PARAMS, CXM_LOG_STATE );
  CXM_ASSERT( QMI_CSI_NO_ERR == estatus );

  /* 2. Reset service handle pointer */
  coex_service_handle = NULL;

  /* 3. Setup service object */
  coex_service_object = NULL;

  /*-----------------------------------------------------------------------*/

  return;
}

/*=============================================================================

  FUNCTION:  coex_qcsi_process_req

=============================================================================*/
/*!
    @brief
    Callback function called by QCSI infrastructure when a REQ message is received
 
    @note
    QCSI infrastructure decodes the data before forwarding it to this layer
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_qcsi_process_req (
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  qmi_csi_cb_error      req_cb_retval    = QMI_CSI_CB_NO_ERR;

#ifdef T_WINNT  
  #error code not present
#else  
  #ifndef REMOVE_QMI_COEX_RESET_V01
  coex_reset_resp_msg_v01 reset_response = {
    .resp.result = QMI_RESULT_SUCCESS_V01,
    .resp.error  = QMI_ERR_NONE_V01
  };
  #endif /* REMOVE_QMI_COEX_RESET_V01 */
#endif

  /*-----------------------------------------------------------------------*/

#ifdef T_WINNT  
  #error code not present
#endif

  CXM_MSG_1( LOW, "COEX QMI serving message ID: %x", msg_id );
  
  /* Process message appropriately based on QMI msg_id */
  switch( msg_id )
  {
    #ifndef REMOVE_QMI_COEX_RESET_V01
    case QMI_COEX_RESET_REQ_V01:
      /* TO DO: reset all the state variables maintained for the client handle */
      #ifdef T_WINNT
      #error code not present
#endif

      req_cb_retval   = mqcsi_conn_mgr_send_resp_from_cb(
                          connection_handle,
                          req_handle,
                          msg_id,
                          &reset_response,
                          sizeof( qmi_response_type_v01 )
                        );
      CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );
      break;
    #endif /* REMOVE_QMI_COEX_RESET_V01 */

    #ifndef REMOVE_QMI_COEX_INDICATION_REGISTER_V01
    /* client msg to enable/disable IND(s) from this service */
    case QMI_COEX_INDICATION_REGISTER_REQ_V01:
      req_cb_retval = coex_handle_indication_register_req(
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_INDICATION_REGISTER_V01 */

    #ifndef REMOVE_QMI_COEX_GET_WWAN_STATE_V01
    /* client msg to REQ latest WWAN state info from this service */
    case QMI_COEX_GET_WWAN_STATE_REQ_V01:
      req_cb_retval = coex_handle_get_wwan_state_req(
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_GET_WWAN_STATE_V01 */

    #ifndef REMOVE_QMI_COEX_SET_WLAN_STATE_V01
    /* client msg to update service with latest WLAN info */
    case QMI_COEX_SET_WLAN_STATE_REQ_V01:
      req_cb_retval = coex_handle_set_wlan_state_req(
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_SET_WLAN_STATE_V01 */

    #ifndef REMOVE_QMI_COEX_GET_WLAN_HIGH_PRIO_STATE_REQ_V01
    /* client msg to REQ latest WLAN high prio info on server (debug) */
    case QMI_COEX_GET_WLAN_HIGH_PRIO_STATE_REQ_V01: 
      req_cb_retval = coex_handle_get_wlan_hp_state_req(
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_GET_WLAN_HIGH_PRIO_STATE_REQ_V01 */

    #ifndef REMOVE_QMI_COEX_GET_WLAN_CONN_STATE_V01
    /* client msg to REQ latest WLAN conn info on server (debug) */
    case QMI_COEX_GET_WLAN_CONN_STATE_REQ_V01:
      req_cb_retval = coex_handle_get_wlan_conn_state_req(
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_GET_WLAN_CONN_STATE_V01 */

    #ifndef REMOVE_QMI_COEX_SET_POLICY_V01
    /* client msg to set current policy for coex algorithms */
    case QMI_COEX_SET_POLICY_REQ_V01:
      req_cb_retval = coex_handle_set_policy_req(
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_SET_POLICY_V01 */

    #ifndef REMOVE_QMI_COEX_GET_POLICY_V01
    /* client msg to return coex's understanding of the last policy update */
    case QMI_COEX_GET_POLICY_REQ_V01:
      req_cb_retval = coex_handle_get_policy_req(
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_GET_POLICY_V01 */

    #ifndef REMOVE_QMI_COEX_METRICS_LTE_BLER_START_V01
    /* client msg to start collecting/collating LTE BLER metric */
    case QMI_COEX_METRICS_LTE_BLER_START_REQ_V01:
      req_cb_retval = coex_handle_lte_metric_bler_start_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_METRICS_LTE_BLER_START_V01 */

    #ifndef REMOVE_QMI_COEX_METRICS_LTE_BLER_STOP_V01
    /* client msg to stop collecting LTE BLER metrics */
    case QMI_COEX_METRICS_LTE_BLER_STOP_REQ_V01:
      req_cb_retval = coex_handle_lte_metric_bler_stop_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_METRICS_LTE_BLER_STOP_V01 */

    #ifndef REMOVE_QMI_COEX_METRICS_LTE_SINR_START_V01
    /* client msg to start collecting/collating LTE SINR metric */
    case QMI_COEX_METRICS_LTE_SINR_START_REQ_V01:
      req_cb_retval = coex_handle_lte_metric_sinr_start_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_METRICS_LTE_SINR_START_V01 */

    #ifndef REMOVE_QMI_COEX_METRICS_LTE_SINR_READ_V01
    /* client msg to read current filter output for LTE SINR metric */
    case QMI_COEX_METRICS_LTE_SINR_READ_REQ_V01:
      req_cb_retval = coex_handle_lte_metric_sinr_read_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_METRICS_LTE_SINR_READ_V01 */

    #ifndef REMOVE_QMI_COEX_METRICS_LTE_SINR_STOP_V01
    /* client req to stop collecting/collating LTE SINR metric */
    case QMI_COEX_METRICS_LTE_SINR_STOP_REQ_V01:
      req_cb_retval = coex_handle_lte_metric_sinr_stop_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_METRICS_LTE_SINR_STOP_V01 */

    #ifndef REMOVE_QMI_COEX_SET_BAND_FILTER_INFO_V01
    /* client req to pass band filter info */
    case QMI_COEX_SET_BAND_FILTER_INFO_REQ_V01:
      req_cb_retval = coex_handle_set_band_filter_info_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_SET_BAND_FILTER_INFO_V01 */

    #ifndef REMOVE_QMI_COEX_GET_BAND_FILTER_INFO_V01
    /* client req to get the band filter info previously passed */
    case QMI_COEX_GET_BAND_FILTER_INFO_REQ_V01:
      req_cb_retval = coex_handle_get_band_filter_info_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_GET_BAND_FILTER_INFO_V01 */

    #ifndef REMOVE_QMI_COEX_GET_WCI2_MWS_PARAMS_V01
    /* request to return jitter and offset info */
    case QMI_COEX_GET_WCI2_MWS_PARAMS_REQ_V01:
      req_cb_retval = coex_handle_get_wci2_mws_params_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_GET_WCI2_MWS_PARAMS_V01 */

    #ifndef REMOVE_QMI_COEX_SET_SLEEP_NOTIFICATION_V01
    /* request to set the sleep notification threshold */
    case QMI_COEX_SET_SLEEP_NOTIFICATION_REQ_V01:
      req_cb_retval = coex_handle_set_sleep_notification_thresh_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_SET_SLEEP_NOTIFICATION_V01 */

    #ifndef REMOVE_QMI_COEX_GET_SLEEP_NOTIFICATION_V01
    /* request to get the sleep notification threshold */
    case QMI_COEX_GET_SLEEP_NOTIFICATION_REQ_V01:
      req_cb_retval = coex_handle_get_sleep_notification_thresh_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_GET_SLEEP_NOTIFICATION_V01 */

    #ifndef REMOVE_QMI_COEX_WCN_WAKE_SYNC_V01
    /* request to turn on/off WCN<->WWAN page scan wakeup syncs */
    case QMI_COEX_WCN_WAKE_SYNC_REQ_V01:
      req_cb_retval = coex_handle_wcn_wake_sync_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* QMI_COEX_WCN_WAKE_SYNC_V01 */

    #ifndef REMOVE_QMI_COEX_GET_CONFLICT_PARAMS_V01
    /* request to get the conflict table */
    case QMI_COEX_GET_CONFLICT_PARAMS_REQ_V01:
      req_cb_retval = coex_handle_get_conflict_params_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* QMI_COEX_GET_CONFLICT_PARAMS_V01 */

    #ifndef REMOVE_QMI_COEX_SET_SCAN_FREQ_BAND_FILTER_V01
    /* client req to pass SCAN freq band filter info */
    case QMI_COEX_SET_SCAN_FREQ_BAND_FILTER_REQ_V01:
      req_cb_retval = coex_handle_set_scan_freq_band_filter_info_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_SET_SCAN_FREQ_BAND_FILTER_V01 */

    #ifndef REMOVE_QMI_COEX_GET_SCAN_FREQ_BAND_FILTER_V01
    /* client req to get the SCAN freq band filter info previously passed */
    case QMI_COEX_GET_SCAN_FREQ_BAND_FILTER_REQ_V01:
      req_cb_retval = coex_handle_get_scan_freq_band_filter_info_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_GET_SCAN_FREQ_BAND_FILTER_V01 */

    #ifndef REMOVE_QMI_COEX_METRICS_START_V01
    /* request to start collecting metrics */
    case QMI_COEX_METRICS_START_REQ_V01:
      req_cb_retval = coex_handle_tech_metric_start_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_METRICS_START_V01 */

    #ifndef REMOVE_QMI_COEX_METRICS_READ_V01
    /* request to read metrics */
    case QMI_COEX_METRICS_READ_REQ_V01:
      req_cb_retval = coex_handle_tech_metric_read_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_METRICS_READ_V01 */

    #ifndef REMOVE_QMI_COEX_METRICS_STOP_V01
    /* request to stop collecting metrics */
    case QMI_COEX_METRICS_STOP_REQ_V01:
      req_cb_retval = coex_handle_tech_metric_stop_req (
                        connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        service_cookie
                      );
      break;
    #endif /* REMOVE_QMI_COEX_METRICS_STOP_V01 */

    default:
      req_cb_retval   = QMI_CSI_CB_UNSUPPORTED_ERR; 
      break;

  } /* switch ( msg_id ) */  

  /*-----------------------------------------------------------------------*/

	return req_cb_retval;
}

