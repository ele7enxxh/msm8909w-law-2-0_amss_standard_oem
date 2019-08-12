/******************************************************************************
  @file    qmi_ping_clnt.c
  @brief   QMI ping client

  DESCRIPTION
  QMI ping client
   
  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "comdef.h"
#include "msg.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "test_service_v01.h"

#ifdef FEATURE_QMI_NATIVE_REX
typedef unsigned char *rex_stack_pointer_type;
typedef unsigned long rex_task_func_param_type;
#endif

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG 0x00000001

#define QMI_PING_CLIENT_STACK_SIZE  (2048)
static rex_tcb_type        qmi_ping_client_tcb;
static rex_task_attributes_type qmi_ping_client_task_attributes;
static char qmi_ping_client_stack[QMI_PING_CLIENT_STACK_SIZE];


static int pending_inds = 0;
static int pending_async = 0;
static int async_mode;

static void msleep(unsigned int ms)
{
  rex_sleep(ms);
}

/*=============================================================================
  CALLBACK FUNCTION ping_ind_cb
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
void ping_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  --pending_inds;
}

/*=============================================================================
  CALLBACK FUNCTION ping_rx_cb
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
static void ping_rx_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
 )
{
  --pending_async;
}

/*=============================================================================
  FUNCTION ping_basic_test
=============================================================================*/
/*!
@brief
  This function sends a number of basic ping messages asynchronously

@param[in]   clnt                Client handle needed to send messages
                                 
@param[in]   txn                 Transaction handle
 
@param[in]   num_pings           Number of pings to send
 
*/
/*=========================================================================*/
int ping_basic_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn,
 int num_pings
 )
{
  int i,rc;
  test_ping_req_msg_v01 req;
  test_ping_resp_msg_v01 resp;
  /* Set the value of the basic ping request */
  memset(&req, 0, sizeof(req));
  QMI_MEM_COPY(&req, sizeof(req), "ping", 4);
  for (i=0;i<num_pings;++i)
  {
    if(async_mode)
    {
      rc = qmi_client_send_msg_async(*clnt, QMI_TEST_REQ_V01, &req, sizeof(req),
          &resp, sizeof(resp), ping_rx_cb, (void *)2, txn);
      if (rc != 0)
      {
        return rc;
      }
      ++pending_async;
      msleep(10);
    }
    else
    {
      rc = qmi_client_send_msg_sync(*clnt, QMI_TEST_REQ_V01, &req, sizeof(req),
          &resp, sizeof(resp), 0);
      if (rc != 0)
      {
        return rc;
      }
    }
  }
  /* Wait until all pending async messages have been received */
  while (async_mode && pending_async != 0)
  {
    msleep(500);
  }
  return 0;
}

/*=============================================================================
  FUNCTION ping_data_test
=============================================================================*/
/*!
@brief
  This function sends a number of data ping messages asynchronously

@param[in]   clnt                Client handle needed to send messages
                                 
@param[in]   txn                 Transaction handle
 
@param[in]   num_pings           Number of data messages to send
 
@param[in]   msg_size            Size of data messages to send 
 
*/
/*=========================================================================*/
int ping_data_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn,
 int num_msgs,
 int msg_size
 )
{
  int i,rc;
  test_data_req_msg_v01 *data_req;
  test_data_resp_msg_v01 *data_resp;
  rc = QMI_CLIENT_ALLOC_FAILURE;
  data_req = (test_data_req_msg_v01*)MALLOC(sizeof(test_data_req_msg_v01));
  if(!data_req) 
  {
    return rc;
  }
  data_resp = (test_data_resp_msg_v01*)MALLOC(sizeof(test_data_resp_msg_v01));
  if(!data_resp) 
  {
    FREE(data_req);
    return rc;
  }
  memset( data_req, 0, sizeof(test_data_req_msg_v01) );
  memset( data_resp, 0, sizeof(test_data_resp_msg_v01) );
  data_req->data_len = msg_size;
  for (i=0;i<num_msgs;++i)
  {
    if(async_mode)
    {
      rc = qmi_client_send_msg_async(*clnt, QMI_TEST_DATA_REQ_V01, data_req, sizeof(test_data_req_msg_v01),
          data_resp, sizeof(test_data_resp_msg_v01), ping_rx_cb, (void *)2, txn);
      if (rc != 0)
      {
        FREE(data_req);
        FREE(data_resp);
        return rc;
      }
      ++pending_async;
      msleep(10);
    }
    else
    {
      rc = qmi_client_send_msg_sync(*clnt, QMI_TEST_DATA_REQ_V01, data_req, sizeof(test_data_req_msg_v01),
          data_resp, sizeof(test_data_resp_msg_v01), 0);
      if (rc != 0)
      {
        FREE(data_req);
        FREE(data_resp);
        return rc;
      }
    }
  }
  /* Wait until all pending async messages have been received */
  while (async_mode && pending_async != 0)
  {
    msleep(500);
  }
  FREE(data_req);
  FREE(data_resp);
  return 0;
}

/*=============================================================================
  FUNCTION ping_ind_test
=============================================================================*/
/*!
@brief 
  This function tells the service to send a specified number of indication messages

@param[in]   clnt                Client handle needed to send messages
                                 
@param[in]   txn                 Transaction handle
 
@param[in]   num_inds            Number of indications for the service to send 
 
@param[in]   ind_size            Size of indications for the service to send 
 
@param[in]   delay               Amount of time the server should wait between indications
 
*/
/*=========================================================================*/
int ping_ind_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn,
 int num_inds,
 int ind_size,
 int delay
 )
{
  int rc;
  test_data_ind_reg_req_msg_v01 data_ind_reg_req;
  test_data_ind_reg_resp_msg_v01 data_ind_reg_resp;

  /* Set the number of pending indications */
  pending_inds = num_inds;
  memset( &data_ind_reg_req, 0, sizeof(test_data_ind_reg_req_msg_v01) );
  memset( &data_ind_reg_resp, 0, sizeof(test_data_ind_reg_resp_msg_v01) );
  data_ind_reg_req.num_inds_valid = 1;
  data_ind_reg_req.num_inds = num_inds;
  /* Send the optional TLVs if these values are passed as arguments */
  if (delay > 0)
  {
    data_ind_reg_req.ms_delay_valid = 1;
    data_ind_reg_req.ms_delay = delay;
  }
  if (ind_size > 0)
  {
    data_ind_reg_req.ind_size_valid = 1;
    data_ind_reg_req.ind_size = ind_size;
  }
  if(async_mode)
  {
    rc = qmi_client_send_msg_async(*clnt, QMI_TEST_DATA_IND_REG_REQ_V01, &data_ind_reg_req, 
        sizeof(data_ind_reg_req),&data_ind_reg_resp, 
        sizeof(data_ind_reg_resp), ping_rx_cb, (void *)2, txn);
    if (rc != 0)
    {
      return rc;
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync(*clnt, QMI_TEST_DATA_IND_REG_REQ_V01, &data_ind_reg_req, 
        sizeof(data_ind_reg_req),&data_ind_reg_resp, 
        sizeof(data_ind_reg_resp), 0);
    if (rc != 0)
    {
      return rc;
    }
  }
  /* Wait until all pending indications have been received */
  while (pending_inds != 0)
  {
    msleep(500);
  }
  return 0;
}

void qmi_ping_client_thread(uint32 handle)
{
  qmi_client_type clnt, notifier;
  qmi_txn_handle txn;
  unsigned int num_services, num_entries=10, i=0, num_services_old=0;
  int rc;
  qmi_cci_os_signal_type os_params;
  qmi_service_info info[10];
  qmi_idl_service_object_type ping_service_object = test_get_service_object_v01();

  os_params.tcb = rex_self();
  os_params.sig = QMI_CLNT_WAIT_SIG;
  os_params.timer_sig = QMI_CLNT_TIMER_SIG;

  rc = qmi_client_notifier_init(ping_service_object, &os_params, &notifier);

  /* Check if the service is up, if not wait on a signal */
  while(1)
  {
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

    /* The server has come up, store the information in info variable */
    num_entries=10;
    rc = qmi_client_get_service_list( ping_service_object, info, &num_entries, &num_services);

    if(rc != QMI_NO_ERR || num_services == num_services_old)
      continue;

    num_services_old = num_services;

    for(i = 0; i < num_services; i++)
    {
      qmi_service_instance instance = 0;
      rc = qmi_client_init(&info[i], ping_service_object, ping_ind_cb, NULL, &os_params, &clnt);
      qmi_client_get_instance_id(&info[i],&instance);
      MSG_1(MSG_SSID_ONCRPC, MSG_LEGACY_HIGH,
            "PING: Starting Ping Tests on instance %d", instance);
      rc = ping_basic_test(&clnt,&txn,1);
      rc |= ping_basic_test(&clnt,&txn,10);
      rc |= ping_data_test(&clnt, &txn, 1, 1024);
      rc |= ping_data_test(&clnt, &txn, 10, 1024);
      rc |= ping_ind_test(&clnt, &txn, 1, 1024, 10);
      rc |= ping_ind_test(&clnt, &txn, 10, 1024, 10);
      MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_HIGH, 
            "PING: Tests on instance %d returned %d", instance, rc);
      rc = qmi_client_release(clnt);
    }
  }
  /* Not reached */
  // rc = qmi_client_release(notifier);
  // MSG_HIGH("PING: qmi_client_release notifier returned %d\n", rc, 0,0);
}

void qmi_ping_client_start(void)
{
  char task_name[] = "QMI_PING_CLNT";
  rex_task_attr_init(&qmi_ping_client_task_attributes);
  rex_task_attr_set_stack_addr(&qmi_ping_client_task_attributes,
                               (rex_stack_pointer_type)qmi_ping_client_stack);
  rex_task_attr_set_stack_size(&qmi_ping_client_task_attributes,
                               QMI_PING_CLIENT_STACK_SIZE);
  rex_task_attr_set_prio(&qmi_ping_client_task_attributes, 10);
  rex_task_attr_set_entry_func(&qmi_ping_client_task_attributes,
                               qmi_ping_client_thread,
                               NULL);
  rex_task_attr_set_initially_suspended(&qmi_ping_client_task_attributes,
                                        FALSE);
  rex_task_attr_set_task_name(&qmi_ping_client_task_attributes,
                              task_name);
  rex_common_def_task(&qmi_ping_client_tcb, &qmi_ping_client_task_attributes);
}
