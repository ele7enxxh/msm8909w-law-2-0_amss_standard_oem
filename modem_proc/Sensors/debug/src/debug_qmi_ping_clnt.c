/******************************************************************************
  @file    debug_qmi_ping_clnt.c
  @brief   DEBUG QMI ping client

  DESCRIPTION
  Test client based on QMI ping client. Used to test DEBUG in ADSP only
  test scenario.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-08-13  gju  Remove smr_util
  2014-07-22  ps   Fix compiler warnings
  2013-08-09  ps   Eliminate compilier warnings
*******************************************************************************/
#include "comdef.h"
#include "msg.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "sns_debug_interface_v01.h"
#include "qurt.h"
#include "qurt_sclk.h"
#include "sns_common.h"
#include "sns_debug_api.h"
#include "sns_debug_str.h"
#include "sns_init.h"

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG 0x00000001
#define QMI_CLNT_DIAG_WAIT_SIG 0x00000002
#define QMI_CLNT_DIAG_TIMER_SIG 0x00000004

#define QMI_PING_CLIENT_STACK_SIZE  (2048)

static qurt_thread_attr_t  debug_qmi_ping_client_thread_attr;
static qurt_thread_t       debug_qmi_ping_client_tcb;
static char                debug_qmi_ping_client_stack[QMI_PING_CLIENT_STACK_SIZE];
static qurt_anysignal_t    debug_qmi_ping_client_sig;


/*=============================================================================
  CALLBACK FUNCTION debug_ping_ind_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when 
  infrastructure receives an indication for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to 
                 identify different services.
 
@param[in]   msg_id              Message ID of the indication

@param[in]  ind_buf              Buffer holding the encoded indication

@param[in]  ind_buf_len          Length of the encoded indication
 
@param[in]  ind_cb_data          Cookie value supplied by the client during registration

*/
/*=========================================================================*/
void debug_ping_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  switch (msg_id)
  {
    case SNS_DEBUG_STRING_ID_IND_V01:
      {
        qmi_client_error_type err;
        sns_debug_string_id_ind_msg_v01 ind_msg;
        uint16_t string_identifier;
        uint8_t module_id;
        uint32_t param_values_len; /* Must be set to # of elements in param_values */
        uint32_t line_number;
        uint32_t i;

        err = qmi_client_message_decode(user_handle, 
                                        QMI_IDL_INDICATION,
                                        SNS_DEBUG_STRING_ID_IND_V01,
                                        ind_buf, ind_buf_len,
                                        &ind_msg, 
                                        sizeof(sns_debug_string_id_ind_msg_v01));
        if(err != QMI_NO_ERR) {
           qurt_printf("\nError: qmi_client_message_decode %d\n", err);
        }

        string_identifier = ind_msg.string_identifier;
        module_id = ind_msg.module_id;
        line_number = ind_msg.line_number;
        param_values_len = ind_msg.param_values_len;
        qurt_printf("\n--- DEBUG MSG: Filename: %s \n", ind_msg.file_name);
        qurt_printf("                 Module: %u   SID: %u   Line: %lu   Num_Params: %lu \n", module_id, string_identifier, line_number, param_values_len);
        for (i=0; i<param_values_len; i++)
        {
          qurt_printf("                 Param[%lu]: %ld \n", i, ind_msg.param_values[i]);
        }
      }
      break;

    case SNS_DEBUG_LOG_IND_V01:
      {
        qmi_client_error_type err;
        sns_debug_log_ind_msg_v01 ind_msg;
        uint32_t i;

        err = qmi_client_message_decode(user_handle, 
                                        QMI_IDL_INDICATION,
                                        SNS_DEBUG_LOG_IND_V01,
                                        ind_buf, ind_buf_len,
                                        &ind_msg, 
                                        sizeof(SNS_DEBUG_LOG_IND_V01));
        if(err != QMI_NO_ERR) {
           qurt_printf("\nError: qmi_client_message_decode %d\n", err);
        }

        qurt_printf("\n--- LOG PKT: Type: %u   Size:%u   Len:%lu  \n", ind_msg.log_pkt_type,ind_msg.logpkt_size, ind_msg.log_pkt_contents_len);
        for (i=0; i<ind_msg.log_pkt_contents_len; i++)
        {
          qurt_printf("                 Pkt[%lu]: %lu \n", i, ind_msg.log_pkt_contents[i]);
        }
      }
      break;

    default:
      break;
  }
}


/*=============================================================================
  CALLBACK FUNCTION diag_ping_ind_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when 
  infrastructure receives an indication for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to 
                 identify different services.
 
@param[in]   msg_id              Message ID of the indication

@param[in]  ind_buf              Buffer holding the encoded indication

@param[in]  ind_buf_len          Length of the encoded indication
 
@param[in]  ind_cb_data          Cookie value supplied by the client during registration

*/
/*=========================================================================*/
void diag_ping_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
   /* do nothing */
}

/*=============================================================================
  CALLBACK FUNCTION debug_ping_rx_cb
=============================================================================*/
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
/*=========================================================================*/
static void debug_ping_rx_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
 )
{
  /* Print the appropriate message based on the message ID */
  switch (msg_id)
  {
    case SNS_DEBUG_VERSION_RESP_V01:
    {
      sns_common_version_resp_msg_v01 *resp_msg = (sns_common_version_resp_msg_v01*)buf;
      uint32_t interface_version_number;
      uint16_t max_message_id;
      uint8_t sns_result;
      uint8_t sns_err;

      interface_version_number = resp_msg->interface_version_number;
      max_message_id = resp_msg->max_message_id;
      sns_err = resp_msg->resp.sns_err_t;
      sns_result = resp_msg->resp.sns_result_t;

      qurt_printf("\n--- DEBUG VERSION RESP: Ver: %lu Max ID:%u\n", interface_version_number, max_message_id);
      qurt_printf("\n sns_err %d, sns_result %d\n", sns_err, sns_result);
    }
    break;

    default:
      break;
  }
}


/*=============================================================================
  CALLBACK FUNCTION diag_ping_rx_cb
=============================================================================*/
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
/*=========================================================================*/
static void diag_ping_rx_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
 )
{
  /* Print the appropriate message based on the message ID */
  switch (msg_id)
  {
    case SNS_DIAG_SET_DEBUG_MASK_RESP_V01:
    {
      sns_diag_set_debug_mask_resp_msg_v01 *resp_msg = (sns_diag_set_debug_mask_resp_msg_v01*)buf;
      uint8_t sns_result;
      uint8_t sns_err;

      sns_err = resp_msg->resp.sns_err_t;
      sns_result = resp_msg->resp.sns_result_t;

      qurt_printf("\n--- DIAG SET DEBUG MASK RESP: Err: %d   Result:%d \n", sns_err, sns_result);
    }
    break;

    default:
      break;
  }
}

/*=============================================================================
  FUNCTION debug_ping_data_test
=============================================================================*/
/*!
@brief
  This function sends a number of data ping messages asynchronously

@param[in]   clnt                Client handle needed to send messages
                                 
@param[in]   txn                 Transaction handle
 
*/
/*=========================================================================*/
int debug_start_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn
 )
{
  uint32                           ver_req;
  sns_common_version_resp_msg_v01 *ver_resp;

  {
    int rc;

    ver_resp = (sns_common_version_resp_msg_v01*)MALLOC(sizeof(sns_common_version_resp_msg_v01));
    if(!ver_resp) {
      return -1;
    }
    ver_req = 0;
    memset( ver_resp, 0, sizeof(sns_common_version_resp_msg_v01) );

    rc = qmi_client_send_msg_async(*clnt, SNS_DEBUG_VERSION_REQ_V01, &ver_req, 0,
                                   ver_resp, sizeof(sns_common_version_resp_msg_v01), debug_ping_rx_cb, (void *)2, txn);
    if (rc != 0){
      FREE(ver_resp);
      return -1;
    }
  }

  FREE(ver_resp);
  return 0;
}


/*=============================================================================
  FUNCTION diag_test
=============================================================================*/
/*!
@brief
  This function sends a number of data ping messages asynchronously

@param[in]   clnt                Client handle needed to send messages
                                 
@param[in]   txn                 Transaction handle
 
*/
/*=========================================================================*/
int diag_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn
 )
{
  sns_diag_set_debug_mask_req_msg_v01 req;
  sns_diag_set_debug_mask_resp_msg_v01 *resp;

  {
    int rc;

    resp = (sns_diag_set_debug_mask_resp_msg_v01*)MALLOC(sizeof(sns_diag_set_debug_mask_resp_msg_v01));
    if(!resp) {
      return -1;
    }
    req.debug_mask.mask=0;
    memset( resp, 0, sizeof(sns_diag_set_debug_mask_resp_msg_v01) );

    rc = qmi_client_send_msg_async(*clnt, SNS_DIAG_SET_DEBUG_MASK_REQ_V01, 
                                   &req, sizeof(sns_diag_set_debug_mask_req_msg_v01),
                                   resp, sizeof(sns_diag_set_debug_mask_resp_msg_v01), 
                                   diag_ping_rx_cb, (void *)2, txn);
    if (rc != 0){
      FREE(resp);
      return -1;
    }
  }

  FREE(resp);
  return 0;
}

void debug_qmi_ping_client_thread(void *unused)
{
  qmi_client_type clnt, diag_clnt;
  qmi_txn_handle txn, diag_txn;
  qmi_client_type notifier, diag_notifier;
  unsigned int num_services, num_entries=10, i=0, status=0;
  int rc;
  qmi_cci_os_signal_type os_params, diag_os_params;
  qmi_service_info info[10];
  qmi_idl_service_object_type ping_service_object = SNS_DEBUG_SVC_get_service_object_v01();
  qmi_idl_service_object_type ping_diag_service_object = SNS_DIAG_DSPS_SVC_get_service_object_v01();

  memset(&os_params,0,sizeof(os_params));
  memset(&diag_os_params,0,sizeof(diag_os_params));

  os_params.ext_signal = NULL;
  os_params.sig = QMI_CLNT_WAIT_SIG;
  os_params.timer_sig = QMI_CLNT_TIMER_SIG;
  diag_os_params.ext_signal = NULL;
  diag_os_params.sig = QMI_CLNT_DIAG_WAIT_SIG;
  diag_os_params.timer_sig = QMI_CLNT_DIAG_TIMER_SIG;

  if ((!ping_service_object) || (!ping_diag_service_object))
  {
    status = 1;
    goto wait;
  }

  rc = qmi_client_notifier_init(ping_service_object, &os_params, &notifier);
  if(rc != QMI_NO_ERR)
  {
    status = 2;
    goto wait;
  }

  rc = qmi_client_notifier_init(ping_diag_service_object, &diag_os_params, &diag_notifier);
  if(rc != QMI_NO_ERR)
  {
    status = 3;
    goto wait;
  }

  sns_init_done();

  /* Check if the service is up, if not wait on a signal */
  QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
  QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

  /* The server has come up, store the information in info variable */
  num_entries=10;
  rc = qmi_client_get_service_list( ping_service_object, info, &num_entries, &num_services);
  if(rc != QMI_NO_ERR)
  {
    status = 4;
    goto wait;
  }

  rc = qmi_client_init(&info[i], ping_service_object, debug_ping_ind_cb, NULL, &os_params, &clnt);
  if(rc != QMI_NO_ERR)
  {
    status = 5;
    goto wait;
  }

  /* Check if the service is up, if not wait on a signal */
  QMI_CCI_OS_SIGNAL_WAIT(&diag_os_params, 0);
  QMI_CCI_OS_SIGNAL_CLEAR(&diag_os_params);

  /* The server has come up, store the information in info variable */
  num_entries=10;
  rc = qmi_client_get_service_list( ping_diag_service_object, info, &num_entries, &num_services);
  if(rc != QMI_NO_ERR)
  {
    status = 6;
    goto wait;
  }

  rc = qmi_client_init(&info[i], ping_diag_service_object, diag_ping_ind_cb, NULL, &diag_os_params, &diag_clnt);
  if(rc != QMI_NO_ERR)
  {
    status = 7;
    goto wait;
  }

  rc = diag_test(&diag_clnt, &diag_txn);
  status = 8;

  rc = debug_start_test(&clnt, &txn);
  status = 9;

wait:
  qurt_printf("Status %d\n", status);
  QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0); //wait forever
}

sns_err_code_e debug_qmi_ping_client_start(void)
{
  sns_err_code_e err_code;

  qurt_anysignal_init(&debug_qmi_ping_client_sig);
  qurt_thread_attr_init(&debug_qmi_ping_client_thread_attr);
  qurt_thread_attr_set_name(&debug_qmi_ping_client_thread_attr, "DEBUG_QMI_PING_CLNT");
  qurt_thread_attr_set_priority(&debug_qmi_ping_client_thread_attr, 10);
  qurt_thread_attr_set_stack_size(&debug_qmi_ping_client_thread_attr, QMI_PING_CLIENT_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&debug_qmi_ping_client_thread_attr, debug_qmi_ping_client_stack);
  err_code = qurt_thread_create(&debug_qmi_ping_client_tcb, &debug_qmi_ping_client_thread_attr, 
      debug_qmi_ping_client_thread, NULL);
  return err_code;

}
