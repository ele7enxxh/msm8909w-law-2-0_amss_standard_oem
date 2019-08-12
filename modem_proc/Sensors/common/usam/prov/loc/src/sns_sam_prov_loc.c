/*============================================================================
  @file sns_sam_prov_loc.c

  @brief
  Implementation of the SAM Provider API for the QMI_LOC service.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_common.h"
#include "qmi_idl_lib.h"
#include "sns_debug_str.h"
#include "fixed_point.h"
#include "sns_queue.h"
#include "sns_sam_prov_api.h"
#include "sns_sam_prov_loc.h"
#include "location_service_v02.h"

/*============================================================================
  Forward Declaration
  ===========================================================================*/

STATIC sns_sam_prov_api sns_sam_prov_loc_api;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sam_prov_handle_req */
STATIC sns_sam_err
sns_sam_prov_loc_handle_req( sns_sam_prov_callback const *cb,
    sns_sam_enable_req *enableReqMsg )
{
  qmiLocRegEventsReqMsgT_v02 regReqMsg;
  sns_sam_enable_req enableReq;

  if( SNS_SAM_PROV_LOC_POS_SUID == *cb->sensorReq->sensor->sensorUID )
  {
    regReqMsg.eventRegMask = QMI_LOC_EVENT_MASK_POSITION_REPORT_V02;
  }
  else if( SNS_SAM_PROV_LOC_SV_INFO_SUID == *cb->sensorReq->sensor->sensorUID )
  {
    regReqMsg.eventRegMask = QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02;
  }

  enableReq.msg.buf = (intptr_t)&regReqMsg;
  enableReq.msg.bufSize = sizeof(qmiLocRegEventsReqMsgT_v02);
  enableReq.msgID = QMI_LOC_REG_EVENTS_REQ_V02;
  enableReq.sensorUID = cb->sensorReq->sensor->sensorUID;

  cb->sns_sam_prov_cb_send_req( cb->sensorReq, &enableReq );

  return SAM_ENONE;
}

/* See sns_sam_prov_handle_resp */
STATIC sns_sam_err
sns_sam_prov_loc_handle_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *responseMsg )
{
  qmiLocGenRespMsgT_v02 const *respMsg =
    (qmiLocGenRespMsgT_v02*)responseMsg->msg.buf;

  if( QMI_RESULT_FAILURE_V01 == respMsg->resp.result )
  {
    cb->sns_sam_prov_stop_stream( cb->sensorReq );
    // PEND: Log error message respMsg->resp.error
  }

  return SAM_ENONE;
}

/* See sns_sam_prov_handle_ind */
STATIC sns_sam_err
sns_sam_prov_loc_handle_ind( sns_sam_prov_callback const *cb,
    sns_sam_ind_msg const *indMsg )
{
  // Do nothing, always pass LOC indication to algorithm
  return SAM_ENONE;
}

/* See sns_sam_prov_service */
STATIC qmi_idl_service_object_type
sns_sam_prov_loc_service( sns_sam_prov_callback const *cb )
{
  return loc_get_service_object_v02();
}

/* See sns_sam_prov_sensor_list */
STATIC sns_sam_err
sns_sam_prov_loc_sensor_list( sns_sam_prov_callback const *cb )
{
  sns_sam_sensor *sensorPOS, *sensorINFO;

  sensorPOS = cb->sns_sam_prov_get_sensor( SNS_SAM_PROV_LOC_POS_SUID );
  if( NULL != sensorPOS )
  {
    sensorPOS->sensorLocation = SNS_PROC_MODEM_V01;
    *sensorPOS->sensorUID = SNS_SAM_PROV_LOC_POS_SUID;
    sensorPOS->serviceObj = loc_get_service_object_v02();
    sensorPOS->isAvailable = true;
    sensorPOS->provAPI = &sns_sam_prov_loc_api;
    sensorPOS->sensorReq = *cb->sensorReq;
    cb->sns_sam_prov_sensor_available( sensorPOS );
  }

  sensorINFO = cb->sns_sam_prov_get_sensor( SNS_SAM_PROV_LOC_SV_INFO_SUID );
  if( NULL != sensorINFO )
  {
    sensorINFO->sensorLocation = SNS_PROC_MODEM_V01;
    *sensorINFO->sensorUID = SNS_SAM_PROV_LOC_SV_INFO_SUID;
    sensorINFO->serviceObj = loc_get_service_object_v02();
    sensorINFO->isAvailable = true;
    sensorINFO->provAPI = &sns_sam_prov_loc_api;
    sensorINFO->sensorReq = *cb->sensorReq;
    cb->sns_sam_prov_sensor_available( sensorINFO );
  }

  // We are purposefully not setting any attributes (sensor->attributes).
  // Defaults of 0 should be appropriate.

  return SAM_ENONE;
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

/**
* QMI_LOC Provider API
*
* See sns_sam_prov_api.
*/
STATIC sns_sam_prov_api sns_sam_prov_loc_api SNS_SAM_UIMAGE_DATA =
{
  .structSize = sizeof(sns_sam_prov_api),
  .sns_sam_prov_handle_req = &sns_sam_prov_loc_handle_req,
  .sns_sam_prov_handle_resp = &sns_sam_prov_loc_handle_resp,
  .sns_sam_prov_handle_ind = &sns_sam_prov_loc_handle_ind,
  .sns_sam_prov_service = &sns_sam_prov_loc_service,
  .sns_sam_prov_sensor_list = &sns_sam_prov_loc_sensor_list,
  .sns_sam_prov_set_batch_period = NULL,
  .sns_sam_prov_get_data = NULL
};

sns_sam_err sns_sam_prov_loc_register(
      sns_sam_prov_api **provAPI )
{
  *provAPI = &sns_sam_prov_loc_api;
  return SAM_ENONE;
}
