/*============================================================================
@file
sns_debug_mr.c

@brief
Implements the message routing portion of the Debug module.

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  INCLUDE FILES
=============================================================================*/
#include "sns_common.h"
#include "sns_debug_str.h"
#include "sns_diag_dsps_v01.h"
#include "sns_memmgr.h"
#include "sns_osa.h"
#include "sns_smr_util.h"
#include "sns_debug.h"
#include <stdint.h>
#include <stdbool.h>
#include <qmi_idl_lib.h>
#include <qmi_client.h>
#include <qmi_cci_target_ext.h>

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG  0x00000001

/*============================================================================
  GLOBAL DEFINES
  ============================================================================*/
static qmi_client_type diag_user_handle;
extern OS_FLAG_GRP *sns_diag_flag_grp;

/*===========================================================================
  FUNCTIONS
  ============================================================================*/

/*===========================================================================
  FUNCTION:   sns_diag_ind_cb
  ===========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an indication for this client

@param[in]  user_handle          Opaque handle used by the infrastructure to
                                 identify different services.

@param[in]  msg_id               Message ID of the indication

@param[in]  ind_buf              Buffer holding the encoded indication

@param[in]  ind_buf_len          Length of the encoded indication

@param[in]  ind_cb_data          Cookie value supplied by the client
                                 during registration
*/
/*=========================================================================*/
static void sns_diag_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  UNREFERENCED_PARAMETER(user_handle);
  UNREFERENCED_PARAMETER(msg_id);
  UNREFERENCED_PARAMETER(ind_buf);
  UNREFERENCED_PARAMETER(ind_buf_len);
  UNREFERENCED_PARAMETER(ind_cb_data);

  SNS_PRINTF_STRING_ERROR_0( DBG_MOD_DIAG, "Invalid indication (from diag)" );
  return;
}


/*=========================================================================
  CALLBACK FUNCTION:  sns_diag_rx_cb
  =========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an asynchronous response for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to
                                 identify different services.

@param[in]   msg_id              Message ID of the response

@param[in]   buf                 Buffer holding the decoded response

@param[in]   len                 Length of the decoded response

@param[in]   resp_cb_data        Cookie value supplied by the client

@param[in]   transp_err          Error value

*/
/*=======================================================================*/
static void sns_diag_rx_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
)
{
  UNREFERENCED_PARAMETER(user_handle);
  UNREFERENCED_PARAMETER(len);
  UNREFERENCED_PARAMETER(resp_cb_data);
  UNREFERENCED_PARAMETER(transp_err);

  /* Print the appropriate message based on the message ID */
  switch (msg_id)
  {
    case SNS_DIAG_SET_DEBUG_MASK_RESP_V01:
    {
      sns_diag_set_debug_mask_resp_msg_v01 *resp_msg = (sns_diag_set_debug_mask_resp_msg_v01*)buf;
      SNS_PRINTF_STRING_MEDIUM_2(DBG_MOD_INIT,
                                 "DIAG SET DEBUG MASK RESP: Err: %d   Result:%d",
                                 resp_msg->resp.sns_err_t,
                                 resp_msg->resp.sns_result_t);
    }
    break;

    case SNS_DIAG_SET_LOG_MASK_RESP_V01:
    {
      sns_diag_set_log_mask_resp_msg_v01 *resp_msg = (sns_diag_set_log_mask_resp_msg_v01*)buf;
      SNS_PRINTF_STRING_MEDIUM_2(DBG_MOD_INIT,
                                 "DIAG SET LOG MASK RESP: Err: %d   Result:%d",
                                 resp_msg->resp.sns_err_t,
                                 resp_msg->resp.sns_result_t);
    }
    break;

    case SNS_DIAG_SET_DEBUG_OPTIONS_RESP_V01:
    {
      sns_diag_set_debug_options_resp_msg_v01 *resp_msg = (sns_diag_set_debug_options_resp_msg_v01*)buf;
      SNS_PRINTF_STRING_MEDIUM_2(DBG_MOD_INIT,
                                 "DIAG SET DEBUG OPTIONS RESP: Err: %d   Result:%d",
                                 resp_msg->resp.sns_err_t,
                                 resp_msg->resp.sns_result_t);
    }
    break;

    default:
      SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT, "Invalid DIAG response");
      break;
  }

  SNS_OS_FREE(buf);
}

/*===========================================================================

  FUNCTION:   sns_diag_send_mask_info

===========================================================================*/
/*!
  @brief
  Sends mask info to DSPS through SMR

  @param[i] mask_type    : Indicates what mask is being set (log/debugstr)
  @param[i] bit_mask     : Bit mask to be sent to DSPS
  @param[i] bit_mask_ext : Extended Bit mask to be sent to DSPS

  @return
  No return value
*/
/*=========================================================================*/
void sns_diag_send_mask_info(uint8_t mask_type,
                             uint64_t bit_mask,
                             uint64_t bit_mask_ext)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type rc;
  uint8_t req_msg_body_size=0, resp_msg_body_size=0;
  void *mask_resp_msg_ptr;
  uint32_t msg_id;

  union {
    sns_diag_set_debug_mask_req_msg_v01 str;
    sns_diag_set_log_mask_req_msg_v01 log;
  } msg;

  switch (mask_type)
  {
    case DEBUG_STR_MASK_INFO:
      {
        msg.str.debug_mask.mask = bit_mask;
        msg_id = SNS_DIAG_SET_DEBUG_MASK_REQ_V01;

        req_msg_body_size = sizeof(sns_diag_set_debug_mask_req_msg_v01);
        resp_msg_body_size = sizeof(sns_diag_set_debug_mask_resp_msg_v01);
        break;
      }
    case LOG_MASK_INFO:
      {
        msg.log.log_mask.mask = bit_mask;
        msg.log.log_mask_ext_valid = 1;
        msg.log.log_mask_ext.mask = bit_mask_ext;
        msg_id = SNS_DIAG_SET_LOG_MASK_REQ_V01;

        req_msg_body_size = sizeof(sns_diag_set_log_mask_req_msg_v01);
        resp_msg_body_size = sizeof(sns_diag_set_log_mask_resp_msg_v01);
        break;
      }
    default:
      return;
  }

  mask_resp_msg_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_APPS_DIAG, resp_msg_body_size);
  if (mask_resp_msg_ptr == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0( DBG_MOD_INIT, "send_mask_info: msg alloc failed" );
    return;
  }

  rc = qmi_client_send_msg_async(diag_user_handle,
                                 msg_id,
                                 &msg,
                                 req_msg_body_size,
                                 mask_resp_msg_ptr,
                                 resp_msg_body_size,
                                 sns_diag_rx_cb,
                                 NULL,
                                 &txn_handle);
  if (rc != QMI_NO_ERR)
  {
    SNS_PRINTF_STRING_ERROR_1( DBG_MOD_INIT, "Error sending mask rc=%d", rc );
    SNS_OS_FREE(mask_resp_msg_ptr);
  }
  else
  {
    SNS_PRINTF_STRING_MEDIUM_3( DBG_MOD_INIT, "Sent mask successfully: type=%d, mask1=0x%lx, mask2=0x%lx", mask_type, bit_mask, bit_mask_ext );
  }
}
// end of function sns_diag_send_mask_info

/*===========================================================================

  FUNCTION:   sns_diag_send_debug_options

===========================================================================*/
/*!
  @brief
  Sends debug options info to DSPS through SMR

  @param[i] options : debug options array

  @return
  No return value
*/
/*=========================================================================*/
void sns_diag_send_debug_options(uint64_t *options)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type rc;
  int req_msg_body_size=0, resp_msg_body_size=0;
  void *resp_msg_ptr;
  uint32_t msg_id;
  uint8_t i, num_options;
  sns_diag_set_debug_options_req_msg_v01 options_msg;

  num_options = 0;
  SNS_PRINTF_STRING_HIGH_0( DBG_MOD_INIT, "send_debug_options: Called");
  for (i = 0; i < SNS_DIAG_MAX_OPTIONS_V01; i++)
  {
    if (options[i] != 0) // Do not send defaults
    {
      SNS_PRINTF_STRING_HIGH_2( DBG_MOD_INIT, "Setting option %d to 0x%x: ", i, options[i]);
      options_msg.options[num_options].option= i;
      options_msg.options[num_options].value = options[i];
      num_options++;
    }
  }

  options_msg.options_len = num_options;

  msg_id = SNS_DIAG_SET_DEBUG_OPTIONS_REQ_V01;
  resp_msg_body_size = sizeof(sns_diag_set_debug_options_resp_msg_v01);
  req_msg_body_size = sizeof(sns_diag_set_debug_options_req_msg_v01);

  resp_msg_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_APPS_DIAG, resp_msg_body_size);
  if (resp_msg_ptr == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0( DBG_MOD_INIT, "send_debug_options: msg alloc failed" );
    return;
  }

  SNS_PRINTF_STRING_HIGH_0( DBG_MOD_INIT, "send_debug_options: Sending msg");
  rc = qmi_client_send_msg_async(diag_user_handle,
                                 msg_id,
                                 &options_msg,
                                 req_msg_body_size,
                                 resp_msg_ptr,
                                 resp_msg_body_size,
                                 sns_diag_rx_cb,
                                 NULL,
                                 &txn_handle);
  if (rc != QMI_NO_ERR)
  {
    SNS_PRINTF_STRING_ERROR_1( DBG_MOD_INIT, "Error sending debug options rc=%d", rc );
    SNS_OS_FREE(resp_msg_ptr);
  }
}
// end of function sns_diag_send_debug_options

/*===========================================================================

  FUNCTION:   sns_debug_mr_init
  ===========================================================================*/
/*!
  @brief
  Initialize with message router. Register with debug/diag services.

  @param[i]
  Not used.

  @return
  None.
*/
/*=========================================================================*/
sns_err_code_e sns_debug_mr_init(void)
{
  qmi_client_type clnt_notifier;
  qmi_cci_os_signal_type os_params;
  qmi_service_info svc_info;
  unsigned int num_services, num_entries;
  qmi_idl_service_object_type sns_diag_svc_obj = SNS_DIAG_DSPS_SVC_get_service_object_v01();
  qmi_client_error_type rc;

  if (NULL == sns_diag_svc_obj)
  {
    SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT,
                              "Sensors DIAG Init: Failed to obtain service object" );
    return SNS_ERR_FAILED;
  }

  //Check for service before initialize and wait.
  if(QMI_NO_ERR != (rc = qmi_client_get_any_service( sns_diag_svc_obj, &svc_info )))
  {
    /* Initialize diag svc client-notifier */
    rc = qmi_client_notifier_init(sns_diag_svc_obj, &os_params, &clnt_notifier);
    if (rc != QMI_NO_ERR)
    {
      SNS_PRINTF_STRING_ERROR_1(DBG_MOD_INIT,
                                "Sensors DIAG Init: Error initializing diag client notifier rc=%d",
                                rc);
      return SNS_ERR_FAILED;
    }

    /* Check if the diag service is up, if not wait on a signal */
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 10000);
    if ( QMI_CCI_OS_SIGNAL_TIMED_OUT(&os_params) )
    {
      SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT,
                                "Sensors DIAG Init: Waiting for diag service timed out");
      qmi_client_release( clnt_notifier );
      return SNS_ERR_FAILED;
    }
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

    SNS_PRINTF_STRING_LOW_0(DBG_MOD_INIT, "Init Diag service");

    /* The diag server has come up, store the information in info variable */
    num_entries=1;
    rc = qmi_client_get_service_list(sns_diag_svc_obj, &svc_info, &num_entries, &num_services);
    if (rc != QMI_NO_ERR)
    {
      SNS_PRINTF_STRING_ERROR_1(DBG_MOD_INIT,
                                "Sensors DIAG Init: Error getting diag service list rc=%d",
                                rc);
      qmi_client_release( clnt_notifier );
      return SNS_ERR_FAILED;
    }
    qmi_client_release( clnt_notifier );
  }
  SNS_PRINTF_STRING_LOW_0(DBG_MOD_INIT, "Got Diag service");

  /* Register for diag service */
  rc = qmi_client_init(&svc_info, sns_diag_svc_obj, sns_diag_ind_cb, NULL, &os_params, &diag_user_handle);
  if (rc != QMI_NO_ERR)
  {
    SNS_PRINTF_STRING_ERROR_1(DBG_MOD_INIT,
                              "Sensors DIAG Init: Error initializing diag client rc=%d",
                              rc);
    return SNS_ERR_FAILED;
  }

  return SNS_SUCCESS;
}
// end of sns_debug_mr_init()

/*===========================================================================
  FUNCTION:   sns_debug_mr_deinit
  ===========================================================================*/
/*!
  @brief
  Deinitialize with message router.

  @param[i]
  Not used.

  @return
  None.
*/
/*=========================================================================*/
sns_err_code_e sns_debug_mr_deinit(void)
{
  return SNS_SUCCESS;
}
// end of sns_debug_mr_deinit()
