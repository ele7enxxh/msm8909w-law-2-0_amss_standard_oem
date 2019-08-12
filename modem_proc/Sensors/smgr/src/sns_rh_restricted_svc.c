/*=============================================================================
  @file sns_rh_restricted_svc.c

  Implementation of SMGR Restricted service

*******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_restricted_svc.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-09-15  pn   Generates DAF cancel request only when DAF is active
  2015-08-25  tc   Propagate cancel request, even if the device is IDLE or OFF
  2014-06-11  vy   Replace SNS_OS_FREE by SNS_OS_ANY_FREE
  2014-05-23  pn   Reduced misses on Restricted service cancellation attempts
  2014-04-23  pn   Initial version
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stringl.h>

#include "sns_smgr_restricted_api_v01.h"
#include "sensor1.h"
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"
#include "sns_rh_mr.h"
#include "sns_rh_main.h"
#include "sns_rh_restricted_svc.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define __SNS_MODULE__ SNS_RH

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Macro
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_rh_process_restricted_svc_cancel_request

===========================================================================*/
/*!
  @brief Processes SNS_SMGR_RESTRICTED_CANCEL_REQ_V01.

  @param[in]  msg_ptr   the request message
 
  @return     device-specific test error code
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_restricted_svc_cancel_request(
  sns_rh_mr_req_q_item_s* msg_ptr)
{
  sns_common_cancel_resp_msg_v01* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_common_cancel_resp_msg_v01);

  /* -------------------------------------------------------------------- */

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  sns_rh_cancel_restricted_service(msg_ptr->header.connection_handle);
  msg_ptr->header.msg_id      = SNS_SMGR_RESTRICTED_CANCEL_RESP_V01;
  msg_ptr->header.body_len    = sizeof(sns_common_cancel_resp_msg_v01);
  resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
  sns_rh_mr_send_resp(&msg_ptr->header, resp_ptr);

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_restricted_svc_version_request

===========================================================================*/
/*!
  @brief Processes SNS_SMGR_RESTRICTED_VERSION_REQ_V01.

  @param[in]  msg_ptr   the request message
 
  @return     device-specific test error code
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_restricted_svc_version_request(
  sns_rh_mr_req_q_item_s* msg_ptr)
{
  sns_common_version_resp_msg_v01* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_common_version_resp_msg_v01);

  /* -------------------------------------------------------------------- */

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  msg_ptr->header.msg_id      = SNS_SMGR_RESTRICTED_VERSION_RESP_V01;
  msg_ptr->header.body_len    = sizeof(*resp_ptr);
  resp_ptr->max_message_id    = SNS_SMGR_RESTRICTED_SVC_V01_MAX_MESSAGE_ID;
  resp_ptr->interface_version_number = SNS_SMGR_RESTRICTED_SVC_V01_IDL_MINOR_VERS;
  resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
  sns_rh_mr_send_resp(&msg_ptr->header, resp_ptr);

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_driver_access_request

===========================================================================*/
/*! 
  @brief Processes SNS_SMGR_DRIVER_ACCESS_REQ_V01.
 
  @details Validates the Driver Access request and forwards it to device driver.

  @param[in]  msg_ptr_ptr - reference to the request message
 
  @return SNS_ERR_NOMEM or SNS_SUCCESS
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_driver_access_request(
  sns_rh_mr_req_q_item_s** msg_ptr_ptr)
{
  sns_rh_mr_req_q_item_s* msg_ptr = *msg_ptr_ptr;
  sns_rh_mr_header_s resp_msg_header = msg_ptr->header;
  sns_smgr_driver_access_req_msg_v01* req_ptr = &msg_ptr->request.driver_access;
  sns_smgr_driver_access_resp_msg_v01* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_smgr_driver_access_resp_msg_v01);
  sns_smgr_request_response_s* req_resp_ptr = NULL;
  sns_smgr_sensor_s* sensor_ptr;
  /* -------------------------------------------------------------------- */

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  // Set default response values
  resp_ptr->Resp.sns_result_t     = SNS_RESULT_SUCCESS_V01;
  resp_ptr->Resp.sns_err_t        = SENSOR1_SUCCESS;
  resp_ptr->ResponseStatus_valid  = true;
  resp_ptr->ResponseStatus        = SNS_SMGR_DRIVER_ACCESS_STATUS_SUCCESS_V01;

  // TODO: Expand on this to allow for Multiple Sensors of the Same Type
  /* Iterate through all of the sensors and check to see if the UUID in
     the request message matches any of the UUIDs for the sensors. If a
     match is found, use that driver
     NOTE: IF THERE ARE MULTIPLE SENSORS WITH THE SAME UUID, ONLY THE
           FIRST SENSOR WILL RECEIVE THE MESSAGE. */
  sensor_ptr = sns_smgr_find_sensor_by_driver_uuid(req_ptr->Uuid);
  if ( sensor_ptr == NULL || SMGR_DRV_FN_PTR(sensor_ptr) == NULL )
  {
    SNS_SMGR_PRINTF0(ERROR, "process_daf_req - no matching driver UUID");
    resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_INVALID_UUID_V01;
  }
  else if ( SMGR_DRV_FN_PTR(sensor_ptr)->process_daf_req == NULL )
  {
    SNS_SMGR_PRINTF0(ERROR, "process_daf_req - not supported for this DD");
    resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_FAIL_V01;
  }
  else if ( (req_resp_ptr = 
             SMGR_ALLOC_STRUCT(sns_smgr_request_response_s)) == NULL )
  {
    resp_ptr->Resp.sns_result_t   = SNS_RESULT_FAILURE_V01;
    resp_ptr->Resp.sns_err_t      = SENSOR1_ENOMEM;
  }
  else
  {
    *msg_ptr_ptr = NULL;

    req_resp_ptr->ddf_sensor_ptr  = sensor_ptr->ddf_sensor_ptr[0];
    req_resp_ptr->msg_id          = msg_ptr->header.msg_id;
    req_resp_ptr->svc_num         = msg_ptr->header.svc_num;
    req_resp_ptr->req.driver_access_ptr = &msg_ptr->request.driver_access;

    /* needed when processing response from SMGR later */
    msg_ptr->header.msg_id        = SNS_SMGR_DRIVER_ACCESS_RESP_V01;
    msg_ptr->header.body_len      = sizeof(sns_smgr_driver_access_resp_msg_v01);
    req_resp_ptr->req_handle      = &msg_ptr->header;
    req_resp_ptr->conn_handle     = msg_ptr->header.connection_handle;
    req_resp_ptr->resp.driver_access_ptr = resp_ptr;

    sns_smgr_put_next_request(sensor_ptr, req_resp_ptr);
  }

  if ( *msg_ptr_ptr != NULL ) /* message not consumed? */
  {
    /* send rejection to client */
    resp_msg_header.msg_id = SNS_SMGR_DRIVER_ACCESS_RESP_V01;
    resp_msg_header.body_len = sizeof(*resp_ptr);
    sns_rh_mr_send_resp(&resp_msg_header, resp_ptr);
  }
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_restricted_smgr_indication

===========================================================================*/
/*!
  @brief Processes indications from SMGR to client of Restricted Service

  @param[i]  ind_ptr  - an indication from SMGR
 
  @return  none
 */
/*=========================================================================*/
void sns_rh_process_restricted_smgr_indication(sns_smgr_indication_s* ind_ptr)
{
  sns_rh_mr_send_indication(
    ind_ptr->conn_handle, &ind_ptr->ind, ind_ptr->msg_id, 
    ind_ptr->ind_len, false);
  SNS_OS_ANY_FREE(ind_ptr);
}

/*===========================================================================

  FUNCTION:   sns_rh_process_restricted_smgr_response

===========================================================================*/
/*!
  @brief Processes response from SMGR to client of Restricted Service

  @param[i]  req_resp_ptr  - a response from SMGR
 
  @return  none
 */
/*=========================================================================*/
void sns_rh_process_restricted_smgr_response(
  sns_smgr_request_response_s* req_resp_ptr)
{
  sns_rh_mr_header_s* hdr_ptr = (sns_rh_mr_header_s*)req_resp_ptr->req_handle;
  /* -------------------------------------------------------------------- */

  sns_rh_mr_send_resp(hdr_ptr, req_resp_ptr->resp.driver_access_ptr);
  SNS_OS_FREE(hdr_ptr);
  SNS_OS_FREE(req_resp_ptr);
}

/*===========================================================================

  FUNCTION:   sns_rh_process_restricted_smgr_request

===========================================================================*/
/*!
  @brief Processes requests for SNS_SMGR_RESTRICTED_SVC_ID_V01

  @detail

  @param[i]  msg_ptr - the message
 
  @return  SNS_SUCCESS or SNS_ERR_NOMEM
 */
/*=========================================================================*/
sns_err_code_e sns_rh_process_restricted_smgr_request(
  sns_rh_mr_req_q_item_s**  msg_ptr_ptr)
{
  sns_err_code_e  err_code = SNS_ERR_NOTSUPPORTED;
  /* -------------------------------------------------------------------- */

  switch ( (*msg_ptr_ptr)->header.msg_id )
  {
  case SNS_SMGR_RESTRICTED_CANCEL_REQ_V01:
    err_code = sns_rh_process_restricted_svc_cancel_request(*msg_ptr_ptr);
    break;
  case SNS_SMGR_RESTRICTED_VERSION_REQ_V01:
    err_code = sns_rh_process_restricted_svc_version_request(*msg_ptr_ptr);
    break;
  case SNS_SMGR_DRIVER_ACCESS_REQ_V01:
    err_code = sns_rh_process_driver_access_request(msg_ptr_ptr);
    break;
  default:
    SNS_SMGR_PRINTF1(
      ERROR, "restricted_smgr_req - unknown msg_id(%u)", 
      (*msg_ptr_ptr)->header.msg_id);
    break;
  }
  return err_code;
}

/*===========================================================================

  FUNCTION:   sns_rh_cancel_retricted_service

===========================================================================*/
/*!
  @brief Cancels all requests to SMGR Restricted service from client
         associated with the given connection

  @param[i]  connection_handle - QCCI handle

  @return  none
 */
/*=========================================================================*/
void sns_rh_cancel_restricted_service(void* connection_handle)
{
  uint8_t s;

  SNS_SMGR_PRINTF1(HIGH, "cancel_restricted_req - conn_handle %u", connection_handle);

  for ( s=0; s<ARR_SIZE(sns_smgr.sensor); s++ )
  {
    sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[s];
    if ( (sensor_ptr != NULL) && (SMGR_DRV_FN_PTR(sensor_ptr) != NULL) &&
         (SMGR_DRV_FN_PTR(sensor_ptr)->cancel_daf_trans != NULL) &&
         (sensor_ptr->dd_handle != NULL ) && sensor_ptr->daf_active )
    {
      sns_smgr_request_response_s* req_resp_ptr =
        SMGR_ALLOC_STRUCT(sns_smgr_request_response_s);

      if ( req_resp_ptr != NULL )
      {
        req_resp_ptr->conn_handle = connection_handle;
        req_resp_ptr->msg_id      = SNS_SMGR_RESTRICTED_CANCEL_REQ_V01;
        req_resp_ptr->svc_num     = SNS_SMGR_RESTRICTED_SVC_ID_V01;
        sns_smgr_put_next_request(sensor_ptr, req_resp_ptr);
        sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
      }
      else
      {
        SNS_SMGR_PRINTF0(ERROR, "cancel_restricted_req - out of memory!");
      }
    }
  }
}

