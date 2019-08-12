/*=====================================================================
  @file sensorrdiag.c

  @brief
  This contains main implementation of receiving and processing
  Remote DIAG commands.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================*/

/*====================================================================
  INCLUDE FILES
====================================================================*/
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "sensor1.h"

#include "comdef.h"
#include "diag_lsm.h"
#include "sns_common.h"
#include "sns_common_v01.h"
#include "qmi_client.h"
#include "diagcmd.h"
#include "sensorrdiag.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "sensorrdiag"
#include <utils/Log.h>

/*====================================================================
  DEFINES
====================================================================*/

#define SNS_DIAG_REMOTE_CLIENT    0x0003

#define SNS_RDIAG_MAX_CLIENTS      20
#define SNS_RDIAG_MAX_TXNS         50

/* Maximum size of a response message from the sensor1 API */
#define SNS_DIAG_RESP_MSG_MAX_LEN 2000

#define SNS_RDIAG_APPS_SENSOR1_MSG_OPEN_HDR_SIZE \
         ( sizeof(sns_rdiag_header_type) )
#define SNS_RDIAG_APPS_SENSOR1_MSG_OPEN_BODY_SIZE  0

#define SNS_RDIAG_APPS_SENSOR1_MSG_WRITE_HDR_SIZE \
         ( sizeof(sns_rdiag_header_type) )
#define SNS_RDIAG_APPS_SENSOR1_MSG_WRITE_MIN_BODY_SIZE \
         ( sizeof(sns_rdiag_write_msgbody) - 1)

#define SNS_RDIAG_APPS_SENSOR1_MSG_CLOSE_HDR_SIZE \
         ( sizeof(sns_rdiag_header_type) )
#define SNS_RDIAG_APPS_SENSOR1_MSG_CLOSE_BODY_SIZE \
         ( sizeof(sns_rdiag_close_msgbody) )

#define SNS_RDIAG_APPS_SENSOR1_RESP_MSG_SIZE \
         ( sizeof(sns_rdiag_sensor1_immediate_res_msg) )

/*====================================================================
  STRUCTURE DEFINES
====================================================================*/

/* Struct storing delayed rsp ID for a request */
typedef struct
{
  bool is_valid;
  uint16_t delay_rsp_id;
  int64_t timestamp; /* Last message sent or received with this txn id */
} delay_rsp_id_association;

/*====================================================================
  FUNCTION DECLARATIONS
====================================================================*/

void *sns_rdiag_request_handler_apps(void *, uint16_t);

/*====================================================================
  GLOBAL VARIABLES
====================================================================*/

/* Mutex/cond used to confirm response in callback function */
static pthread_mutex_t rdiag_mutex;
static pthread_cond_t rdiag_cond;
static uint8_t rdiag_exit_flag;  // flags for remote diag exit condition

/* To ensure the sensor1_write is logged prior to its response */
static pthread_mutex_t log_mutex;

/* Table of handlers to register with DIAG  */
static const diagpkt_user_table_entry_type sns_rdiag_tbl[] =
  {
    /* subsys_cmd_code lo, subsys_cmd_code hi, call back function */
    {SNS_DIAG_REMOTE_CLIENT, SNS_DIAG_REMOTE_CLIENT, sns_rdiag_request_handler_apps},
  };

/* Array used for associating cb_data (a.k.a client_id) with context
 * handle from sensor1_open Client ID starting from 1 */
static sensor1_handle_s *client_association_arr[SNS_RDIAG_MAX_CLIENTS + 1];

/* Array used for associating txn_id with delayed rsp Id from DIAG */
static delay_rsp_id_association delay_rsp_id_association_arr[SNS_RDIAG_MAX_TXNS];

extern const libsensor_svc_accessor svc_map[];
extern const uint32_t SVC_MAP_LEN;

/*====================================================================
  FUNCTIONS
====================================================================*/

size_t memscpy(void *dst, size_t dst_size, const void *src, size_t copy_size)
{
  size_t min_size = dst_size < copy_size ? dst_size : copy_size;
  memcpy(dst, src, min_size);
  return min_size;
}

/*
  Assigns a client ID to the Diag client.

  @return
  Integer value representing the Client ID.
  Zero value indicates error
  Positive value indicates Client ID.
*/
static uint32_t
sns_rdiag_get_client_id()
{
  uint8_t i;

  /* Check for a free context handle value */
  for(i = 1; i <= SNS_RDIAG_MAX_CLIENTS; i++)
  {
    if(NULL == client_association_arr[i])
    {
      return i;
    }
  }

  return 0;
}

/*
  Frees a allocated client Identifier number.

  @param[i] id: Client ID
*/
static void
sns_rdiag_free_client_id(uint32_t id)
{
  client_association_arr[id] = NULL;
  return;
}

/*!
  Return the current LA timestamp (in ns).

  @return Apps timestamp; nanoseconds since last epoch.
*/
static int64_t
sns_rdiag_curr_ts()
{
  struct timespec ts_apps;

  if(0 != clock_gettime( CLOCK_REALTIME, &ts_apps ))
  {
    LOGE("%s: Error with clock_gettime %i", __func__, errno );
    return 0;
  }

  return ((int64_t)ts_apps.tv_sec * 1000000000) + ts_apps.tv_nsec;
}

/*
  Assigns a transaction ID.

  @param[o] txn_id: A Pointer to the placeholder for the transaction id value
*/
static void
sns_rdiag_assign_txn_id(uint8_t *txn_id)
{
  uint8_t i, oldest_txn_id = 0;
  int64_t oldest_ts = 0;

  /* check for a free transaction ID value */
  for(i = 0; i < SNS_RDIAG_MAX_TXNS; i++)
  {
    if(!delay_rsp_id_association_arr[i].is_valid)
    {
      *txn_id = i;
      delay_rsp_id_association_arr[i].is_valid = true;
      delay_rsp_id_association_arr[i].delay_rsp_id = 0;
      delay_rsp_id_association_arr[i].timestamp = sns_rdiag_curr_ts();

      return;
    }
    else if(delay_rsp_id_association_arr[i].timestamp < oldest_ts)
    {
      oldest_ts = delay_rsp_id_association_arr[i].timestamp;
      oldest_txn_id = i;
    }
  }

  LOGE("%s: No txn ID available, using %i (last used %"PRIu64", now %"PRIu64")",
       __func__, oldest_txn_id, delay_rsp_id_association_arr[oldest_txn_id].timestamp,
       sns_rdiag_curr_ts());
  *txn_id = i;
  delay_rsp_id_association_arr[oldest_txn_id].is_valid = true;
  delay_rsp_id_association_arr[oldest_txn_id].delay_rsp_id = 0;
  delay_rsp_id_association_arr[oldest_txn_id].timestamp = sns_rdiag_curr_ts();
}

/*
  Clears a transaction ID.

  @param[i] txn_id: A Pointer to the placeholder for the transaction id value
*/
static void
sns_rdiag_clear_txn_id(uint8_t txn_id)
{
  if(txn_id < SNS_RDIAG_MAX_TXNS ) {
      delay_rsp_id_association_arr[txn_id].is_valid = false;
      delay_rsp_id_association_arr[txn_id].delay_rsp_id = 0;
      delay_rsp_id_association_arr[txn_id].timestamp = 0;
  } else {
    LOGE("%s: Input ERROR, txn_id = %d", __func__, txn_id);
  }
}

/*
  Clears a transaction ID.

  @param[i] txn_id : Value of the transaction ID
  @param[i] rsp    : Pointer to the immediate response message
*/
static void
sns_rdiag_assign_delayed_rsp_id(uint8_t txn_id,
              sns_rdiag_sensor1_immediate_res_msg *rsp)
{
  if(txn_id < SNS_RDIAG_MAX_TXNS ) {
      if(delay_rsp_id_association_arr[txn_id].is_valid)
      {
        delay_rsp_id_association_arr[txn_id].delay_rsp_id =
          diagpkt_subsys_get_delayed_rsp_id( rsp);
        delay_rsp_id_association_arr[txn_id].timestamp = sns_rdiag_curr_ts();
      }
  } else {
    LOGE("%s: Input ERROR, txn_id = %d", __func__, txn_id);
  }
}

/*
  Gets the delayed response ID.

  @param[i] txn_id: Value of the transaction ID
  @param[o] delay_rsp_id: Response ID found.

  @return
  Error code for the operation.
*/
static sensor1_error_e
sns_rdiag_get_delay_rsp_id(uint8_t txn_id, uint16_t *delay_rsp_id)
{
  sensor1_error_e rv = SENSOR1_EFAILED;

  if(NULL == delay_rsp_id ||
     txn_id >= SNS_RDIAG_MAX_TXNS)
  {
    LOGE("%s: Input ERROR, txn_id = %d", __func__, txn_id);
  }
  else if(delay_rsp_id_association_arr[txn_id].is_valid)
  {
    delay_rsp_id_association_arr[txn_id].timestamp = sns_rdiag_curr_ts();
    *delay_rsp_id = delay_rsp_id_association_arr[txn_id].delay_rsp_id;
    rv = SENSOR1_SUCCESS;
  }

  return rv;
}

/*
  Callback function registerd by calling sensor1_writable
*/
void
rdiag_writable_cb(intptr_t cbdata_in, uint32_t service_id)
{
  uint32_t cbdata = (uint32_t)cbdata_in;

  if(SNS_RDIAG_MAX_CLIENTS < cbdata)
  {
    LOGE("%s: Sensor1 Writable Callback: Input parameter err, cbdata = %i",
         __func__, cbdata);
  }
  else
  {
    sns_rdiag_sensor1_delayed_rsp_msg *sensor1_delay_rsp = NULL;
    sensor1_handle_s *context_hdl = client_association_arr[cbdata];

    sensor1_delay_rsp = diagpkt_subsys_alloc_v2_delay(
          DIAG_SUBSYS_SENSORS,
          SNS_DIAG_REMOTE_CLIENT, 0,
          (sizeof(sns_rdiag_sensor1_delayed_rsp_msg) - 1));

    if(NULL == sensor1_delay_rsp)
    {
      LOGE("%s: No memory for delayed response", __func__);
    }
    else
    {
      diagpkt_subsys_set_rsp_cnt(sensor1_delay_rsp, 1);

      sensor1_delay_rsp->cb_data        = cbdata;
      sensor1_delay_rsp->context        = (uint32_t)(uintptr_t)context_hdl;
      sensor1_delay_rsp->srvc_num       = service_id;

      diagpkt_delay_commit((void *)sensor1_delay_rsp);
    }
  }
}

/*
  Callback function registered by calling sensor1_open
*/
void
rdiag_notify_data_cb(intptr_t cbdata_in,
                     sensor1_msg_header_s *msg_hdr,
                     sensor1_msg_type_e msg_type,
                     void *msg)
{
  uint32_t cbdata = (uint32_t)cbdata_in;
  uint16_t delay_rsp_id;

  pthread_mutex_lock(&log_mutex);
  if(NULL == msg_hdr)
  {
    LOGE("%s: msg_hdr is NULL(Sensor1_close)", __func__);
  }
  else if(SNS_RDIAG_MAX_CLIENTS < cbdata)
  {
    LOGE("%s: Input parameters error, cbdata=%d", __func__, cbdata);
  }
  else if(SENSOR1_MSG_TYPE_IND == msg_type)
  {
    LOGV("%s: Received indication message %i from %i",
        __func__, msg_hdr->msg_id, msg_hdr->service_number);
  }
  else if(SENSOR1_MSG_TYPE_RESP != msg_type &&
          SENSOR1_MSG_TYPE_RESP_INT_ERR != msg_type)
  {
    LOGE("%s: Received invalid message type in callback %i", __func__, msg_type);
  }
  else if(SENSOR1_SUCCESS != sns_rdiag_get_delay_rsp_id(msg_hdr->txn_id, &delay_rsp_id))
  {
    LOGE("%s: Could not get delayed rsp_id", __func__);
    sns_rdiag_clear_txn_id(msg_hdr->txn_id);
  }
  else
  {
    sns_rdiag_sensor1_delayed_rsp_msg *sensor1_delay_rsp = NULL;
    uint8_t encoded_arr[SNS_DIAG_RESP_MSG_MAX_LEN];
    uint32_t encoded_length = 0;
    int32_t qmi_err;

    if(msg_hdr->msg_size > 0)
    {
      qmi_idl_service_object_type service =
            svc_map[msg_hdr->service_number].get_svc(
            svc_map[msg_hdr->service_number].maj_ver,
            svc_map[msg_hdr->service_number].min_ver,
            svc_map[msg_hdr->service_number].tool_ver);

      qmi_err =
        qmi_idl_message_encode(
            service, QMI_IDL_RESPONSE, msg_hdr->msg_id,
            msg, msg_hdr->msg_size, encoded_arr,
            SNS_DIAG_RESP_MSG_MAX_LEN, &encoded_length);
      if(QMI_IDL_LIB_NO_ERR != qmi_err)
      {
        LOGE("%s: QMI Encode failed=%d", __func__, qmi_err);
        encoded_length = 0;
      }
    }

    LOGD("%s: Retrieved delay_rsp_id = %d, txn_id = %d, enc msg_size %d",
        __func__, delay_rsp_id, msg_hdr->txn_id, encoded_length);

    sensor1_delay_rsp = diagpkt_subsys_alloc_v2_delay(
        DIAG_SUBSYS_SENSORS, SNS_DIAG_REMOTE_CLIENT, delay_rsp_id,
        sizeof(sns_rdiag_sensor1_delayed_rsp_msg) -1 + encoded_length);

    if(NULL == sensor1_delay_rsp)
    {
      LOGE("%s: No memory for delayed response", __func__);
    }
    else
    {
      diagpkt_subsys_set_rsp_cnt(sensor1_delay_rsp, 1);

      sensor1_delay_rsp->delayed_rsp_id = delay_rsp_id;
      sensor1_delay_rsp->txn_id         = msg_hdr->txn_id;
      sensor1_delay_rsp->cb_data        = cbdata;
      sensor1_delay_rsp->context        = (uint32_t)(uintptr_t)client_association_arr[cbdata];
      sensor1_delay_rsp->srvc_num       = msg_hdr->service_number;
      sensor1_delay_rsp->msg_id         = msg_hdr->msg_id;
      sensor1_delay_rsp->msg_type       = msg_type;
      sensor1_delay_rsp->msg_size       = encoded_length;
      memscpy(&sensor1_delay_rsp->msg, encoded_length, encoded_arr, encoded_length);

      diagpkt_delay_commit((void*)sensor1_delay_rsp);
      LOGD("%s: Delay response committed.", __func__);
    }
    sns_rdiag_clear_txn_id(msg_hdr->txn_id);
  }

  if(NULL != msg && SNS_RDIAG_MAX_CLIENTS > cbdata)
  {
    sensor1_free_msg_buf(client_association_arr[cbdata], msg);
  }
  pthread_mutex_unlock(&log_mutex);
}

/*
  Allocates memory through DIAG api, fills up the immediate response
  and commits response.

  @param[i] sensor1_api_ret_value  : Return value from sensor1 API
          function call if sensor1 API was called. In case there were
          errors with the diag request before calling the sensor1 API,
          this field contains the corresponding sensor1 error code.
  @param[i] cb_data     : Contains the Client ID assigned by the
                          sensors Diag handler Module
  @param[i] context     : Context handle returned from sensor1 API
  @param[i] delayed_rsp_id  : Delayed response identifier. This is
                              assigned by DIAG and is used to match
                              delayed responses
  @param[i] txn id     : Transaction ID assigned by the Sensors Diag
                         Handler Module
  @param[i] rsp_cnt    : Number of delayed responses to follow. If
                         it is 1, it means the immediate response is
                         the final response

  @return
  SENSOR1_SUCCESS
  SENSOR1_ENOMEM - Not enough memory to complete this operation
*/
static sensor1_error_e
sns_rdiag_send_immed_resp(int32_t sensor1_api_ret_value, uint32_t cb_data,
    sensor1_handle_s *context, uint16_t delayed_rsp_id, uint8_t  txn_id,
    uint8_t  rsp_cnt)
{
  sensor1_error_e rv = SENSOR1_EFAILED;
  sns_rdiag_sensor1_immediate_res_msg *sns_diag_res = NULL;

  sns_diag_res = (sns_rdiag_sensor1_immediate_res_msg *)
          diagpkt_subsys_alloc_v2(DIAG_SUBSYS_SENSORS,
                                  SNS_DIAG_REMOTE_CLIENT,
                                  SNS_RDIAG_APPS_SENSOR1_RESP_MSG_SIZE);

  if(NULL == sns_diag_res)
  {
    LOGE("%s: Malloc failure: Could not allocate memory for "
         "Sensor1 Immediate Response", __func__);
    rv = SENSOR1_ENOMEM;
  }
  else
  {
    /* Fill out the immediate response */
    sns_diag_res->sensor1_api_ret_value = sensor1_api_ret_value;
    sns_diag_res->cb_data               = cb_data;
    sns_diag_res->context               = (uint32_t)(uintptr_t)context;
    sns_diag_res->delayed_rsp_id        = delayed_rsp_id;
    sns_diag_res->txn_id                = txn_id;

    /* If the immediate response is the final response, Set Response
      Cnt that is in the diag header */
    if(1 == rsp_cnt)
    {
      diagpkt_subsys_set_rsp_cnt(sns_diag_res, 1);
    }

    diagpkt_commit(sns_diag_res);
    rv = SENSOR1_SUCCESS;
  }

  return rv;
}

/*
  Handle an sensor1 open request.

  @param[i] req_pkt : The request packet (sns_rdiag_msg_body)
  @param[i] pkt_len : Length of req_pkt
*/
static void
sns_rdiag_msg_handle_open(byte *req_pkt, uint16_t pkt_len)
{
  UNREFERENCED_PARAMETER(req_pkt);
  uint32_t client_id = 0;
  sensor1_error_e sensor1_err;

  LOGI("%s: SENSOR1_Open rcvd, pkt_len(%d)", __func__, pkt_len);

  if((pkt_len < (SNS_RDIAG_APPS_SENSOR1_MSG_OPEN_BODY_SIZE +
                  SNS_RDIAG_APPS_SENSOR1_MSG_OPEN_HDR_SIZE)))
  {
    LOGE("%s: Invalid SENSOR1_Open Request due to pkt size(%d)"
          "or cb_data", __func__, pkt_len);
    sns_rdiag_send_immed_resp(SENSOR1_EBAD_PARAM, 0, NULL, 0, 0, 1);
  }
  else if(0 == (client_id = sns_rdiag_get_client_id()))
  {
    LOGE("%s: Client id couldn't be assigned", __func__);
    sns_rdiag_send_immed_resp(SENSOR1_EFAILED, 0, NULL, 0, 0, 1);
  }
  else
  {
    sensor1_err = sensor1_open(&client_association_arr[client_id],
        rdiag_notify_data_cb, client_id );

    LOGE("%s: Packet type 2 sensor1_open error = %d", __func__, sensor1_err);

    sns_rdiag_send_immed_resp(sensor1_err, client_id,
        client_association_arr[client_id], 0, 0, 1 );

    if(SENSOR1_SUCCESS != sensor1_err)
    {
      sns_rdiag_free_client_id(client_id);
    }
    pthread_cond_signal(&rdiag_cond);
  }
}

static qmi_idl_service_object_type
sns_rdiag_get_service(uint32_t svc_num)
{
  qmi_idl_service_object_type service = NULL;

  if(SVC_MAP_LEN < svc_num ||
     NULL == svc_map[svc_num].get_svc)
  {
    LOGE("%s: Unsupported service %i", __func__, svc_num);
  }
  else
  {
    service = svc_map[svc_num].get_svc(
          svc_map[svc_num].maj_ver,
          svc_map[svc_num].min_ver,
          svc_map[svc_num].tool_ver);
  }

  return service;
}

static void*
sns_rdiag_alloc_sensor1_msg(sensor1_handle_s *sensor1_hndl,
    sensor1_msg_header_s *msg_hdr,
    qmi_idl_service_object_type service, uint32_t msg_size)
{
  void *sensor1_msg = NULL;
  uint32_t struct_len = 0;
  int32_t qmi_err;
  sensor1_error_e sensor1_err = SENSOR1_SUCCESS;

  if(0 != msg_size)
  {
    qmi_err = qmi_idl_get_message_c_struct_len(
      service, QMI_IDL_REQUEST, msg_hdr->msg_id, &struct_len);
    if(QMI_NO_ERR != qmi_err)
    {
      LOGE("%s: qmi message len get failed(%d)", __func__, qmi_err);
    }
    else
    {
      sensor1_err = sensor1_alloc_msg_buf(sensor1_hndl, struct_len, &sensor1_msg);
    }
  }
  else
  {
    sensor1_err = sensor1_alloc_msg_buf(sensor1_hndl, 0, &sensor1_msg);
  }

  if(SENSOR1_SUCCESS == sensor1_err)
  {
    msg_hdr->msg_size = struct_len;
  }

  return sensor1_msg;
}

/*
  Handle an sensor1 write request.

  @param[i] req_pkt : The request packet (sns_rdiag_msg_body_s)
  @param[i] pkt_len : Length of req_pkt
  @param[i] pkt_type: Packet type; see sns_rdiag_pkt_type
*/
static void
sns_rdiag_msg_handle_write(byte *req_pkt, uint16_t pkt_len, sns_rdiag_pkt_type pkt_type)
{
  sns_rdiag_msg_body *parsed_msg = (sns_rdiag_msg_body*)req_pkt;
  sensor1_error_e sensor1_err = SENSOR1_SUCCESS;
  void *sensor1_msg = NULL;
  sensor1_handle_s *sensor1_hndl = NULL;

  LOGI("%s: svc num: %i; msg_id: %i; msg type: %i; msg len: %i; txn id: %i (pkt len: %i)",
       __func__, parsed_msg->write_msgbody.srvc_num,
       parsed_msg->write_msgbody.msg_id, parsed_msg->write_msgbody.msg_type,
       parsed_msg->write_msgbody.msg_size, parsed_msg->write_msgbody.txn_id,
       pkt_len);

  pthread_mutex_lock(&log_mutex);
  if(SNS_DIAG_SENSOR1_QMI_MSG != pkt_type &&
     SNS_DIAG_SENSOR1_MSG != pkt_type)
  {
    LOGE("%s: Invalid pkt_type %d", __func__, pkt_type);
    sensor1_err = SENSOR1_EBAD_PARAM;
  }
  if(parsed_msg->write_msgbody.msg_size !=
     pkt_len - SNS_RDIAG_APPS_SENSOR1_MSG_WRITE_MIN_BODY_SIZE - 1)
  {
    LOGE("%s: Invalid pkt_len value -> Test set-up issue (msg len: %i; pkt len: %i)",
         __func__, parsed_msg->write_msgbody.msg_size, pkt_len);
    sensor1_err = SENSOR1_EBAD_PARAM;
  }
  else if((pkt_len < (SNS_RDIAG_APPS_SENSOR1_MSG_WRITE_MIN_BODY_SIZE +
                  SNS_RDIAG_APPS_SENSOR1_MSG_WRITE_HDR_SIZE)) ||
    (parsed_msg->write_msgbody.cbdata > SNS_RDIAG_MAX_CLIENTS) ||
    (client_association_arr[parsed_msg->write_msgbody.cbdata] == NULL))
  {
    LOGE("%s:Invalid Sensor1_write Request. pkt_len(%d)", __func__, pkt_len);
    sensor1_err = SENSOR1_EBAD_PARAM;
  }
  else
  {
    qmi_idl_service_object_type service = NULL;
    sensor1_msg_header_s msg_hdr;

    sensor1_hndl = client_association_arr[parsed_msg->write_msgbody.cbdata];

    msg_hdr.service_number = parsed_msg->write_msgbody.srvc_num;
    msg_hdr.msg_id = parsed_msg->write_msgbody.msg_id;

    if(NULL ==
        (service = sns_rdiag_get_service(msg_hdr.service_number)))
    {
      LOGE("%s: Unable to get service object", __func__);
      sensor1_err = SENSOR1_EFAILED;
    }
    else if(NULL ==
        (sensor1_msg = sns_rdiag_alloc_sensor1_msg(sensor1_hndl,
           &msg_hdr, service, parsed_msg->write_msgbody.msg_size)))
    {
      LOGE("%s: sensor1_alloc request failed", __func__);
      sensor1_err = SENSOR1_EFAILED;
    }
    else
    {
      if(SNS_DIAG_SENSOR1_QMI_MSG == pkt_type)
      {
        if(0 != parsed_msg->write_msgbody.msg_size)
        {
          int32_t qmi_err = qmi_idl_message_decode(
              service, QMI_IDL_REQUEST, msg_hdr.msg_id,
              (const void*)parsed_msg->write_msgbody.qmi_msg,
              parsed_msg->write_msgbody.msg_size, sensor1_msg,
              msg_hdr.msg_size);
          if(QMI_NO_ERR != qmi_err)
          {
            LOGE("%s: qmi message decode failed(%d)", __func__, qmi_err);
            sensor1_err = SENSOR1_EFAILED;
          }
        }
      }
      else if(SNS_DIAG_SENSOR1_MSG == pkt_type)
      {
        memset(sensor1_msg, 0x00, parsed_msg->write_msgbody.msg_size);

        memscpy(sensor1_msg, msg_hdr.msg_size,
            parsed_msg->write_msgbody.qmi_msg,
            parsed_msg->write_msgbody.msg_size);

        msg_hdr.service_number = parsed_msg->write_msgbody.srvc_num;
        msg_hdr.msg_id = parsed_msg->write_msgbody.msg_id;
        msg_hdr.msg_size = parsed_msg->write_msgbody.msg_size;
      }

      if(SENSOR1_SUCCESS == sensor1_err)
      {
        sns_rdiag_assign_txn_id(&msg_hdr.txn_id);
        sns_rdiag_sensor1_immediate_res_msg *response = NULL;

        response = (sns_rdiag_sensor1_immediate_res_msg*)diagpkt_subsys_alloc_v2(
            DIAG_SUBSYS_SENSORS, SNS_DIAG_REMOTE_CLIENT, SNS_RDIAG_APPS_SENSOR1_RESP_MSG_SIZE);
        if(NULL == response)
        {
          LOGE("%s: Could not allocate memory for CM Diag response", __func__);
          sns_rdiag_clear_txn_id(msg_hdr.txn_id);
          sensor1_err = SENSOR1_ENOMEM;
        }
        else
        {
          sns_rdiag_assign_delayed_rsp_id( msg_hdr.txn_id, response);
          response->cb_data = parsed_msg->write_msgbody.cbdata;
          response->context = (uint32_t)(uintptr_t)
            client_association_arr[parsed_msg->write_msgbody.cbdata];
          response->delayed_rsp_id = response->diag_hdr.delayed_rsp_id;
          response->txn_id = msg_hdr.txn_id;

          LOGD("%s:Sensor1_Write: service num(%d), Msg ID(%d), Msg size(%d), Txn id(%d)",
              __func__, msg_hdr.service_number, msg_hdr.msg_id,
              msg_hdr.msg_size, msg_hdr.txn_id);

          sensor1_err = sensor1_write(sensor1_hndl, &msg_hdr, sensor1_msg);
          LOGE("%s: Sensor1_write error = %d", __func__, sensor1_err);
          if(SENSOR1_SUCCESS != sensor1_err)
          {
            sns_rdiag_clear_txn_id(msg_hdr.txn_id);
            diagpkt_subsys_set_rsp_cnt(response, 1);
          }

          response->sensor1_api_ret_value = sensor1_err;
          diagpkt_commit(response);
          sensor1_err = SENSOR1_SUCCESS;  // To avoid duplicate immediate response below
        }
      }
    }
  }

  if(SENSOR1_SUCCESS != sensor1_err)
  {
    sns_rdiag_send_immed_resp(sensor1_err, 0, NULL, 0, 0, 1);
    if(NULL != sensor1_msg && NULL != sensor1_hndl)
    {
      sensor1_free_msg_buf(sensor1_hndl, sensor1_msg);
    }
  }
  pthread_mutex_unlock(&log_mutex);
}

/*
  Handle an sensor1 close request.

  @param[i] req_pkt : The request packet (sns_rdiag_msg_body)
  @param[i] pkt_len     : Length of req_pkt
*/
static void
sns_rdiag_msg_handle_close(byte *req_pkt, uint16_t pkt_len)
{
  sensor1_handle_s *context_hdl = NULL;
  sns_rdiag_msg_body *parsed_msg = (sns_rdiag_msg_body*)req_pkt;
  sensor1_error_e sensor1_err;

  LOGI("%s: SENSOR1_Close rcvd, pkt_len(%d)", __func__, pkt_len);

  if((pkt_len < (SNS_RDIAG_APPS_SENSOR1_MSG_CLOSE_BODY_SIZE +
                 SNS_RDIAG_APPS_SENSOR1_MSG_CLOSE_HDR_SIZE) ) ||
      (parsed_msg->close_msgbody.cbdata > SNS_RDIAG_MAX_CLIENTS) ||
      (client_association_arr[parsed_msg->close_msgbody.cbdata] == NULL) )
  {
    LOGE("%s: Invalid Sensor1_Close request, pkt len = %d, cb_data= %i",
          __func__, pkt_len, parsed_msg->close_msgbody.cbdata);
    sns_rdiag_send_immed_resp(SENSOR1_EBAD_PARAM, 0, NULL, 0, 0, 1);
  }
  else
  {
    context_hdl = client_association_arr[parsed_msg->close_msgbody.cbdata];

    sensor1_err = sensor1_close(context_hdl);
    if(SENSOR1_SUCCESS != sensor1_err)
    {
      LOGE("%s: Packet type 2 sensor1_close error = %d", __func__, sensor1_err);
    }

    sns_rdiag_send_immed_resp(sensor1_err,
        parsed_msg->close_msgbody.cbdata, context_hdl, 0, 0, 1);

    sns_rdiag_free_client_id(parsed_msg->close_msgbody.cbdata);
    pthread_cond_signal(&rdiag_cond);
  }
}

/*
  Handle an sensor1 close request.

  @param[i] req_pkt : The request packet (sns_rdiag_msg_body)
  @param[i] pkt_len : Length of req_pkt
*/
static void
sns_rdiag_msg_handle_writable(byte *req_pkt, uint16_t pkt_len)
{
  sensor1_handle_s *context_hdl = NULL;
  sns_rdiag_msg_body *parsed_msg = (sns_rdiag_msg_body*)req_pkt;
  sensor1_error_e sensor1_err;

  LOGI("%s: SENSOR1_Writable rcvd, pkt_len(%d)", __func__, pkt_len);

  context_hdl =
    client_association_arr[parsed_msg->writable_msgbody.cbdata];

  sensor1_err = sensor1_writable(context_hdl, rdiag_writable_cb,
              parsed_msg->writable_msgbody.cbdata,
              (uint32_t)parsed_msg->writable_msgbody.srvc_num);

  sns_rdiag_send_immed_resp(sensor1_err,
              parsed_msg->writable_msgbody.cbdata,
              context_hdl, 0, 0, 1);
}

/*
  This function is called to handle Remote Diag commands. The QMI encoded
  messages contained in the DIAG commands are sent to the Sensor1 API.

  @param[i] req_pkt     : A pointer to the diag command packet
  @param[i] pkt_len     : Length of the command packet
  @param[i] pkt_type    : 2 if message is QMI encoded; 3 if raw byte stream

  @return
  Immediate response (sns_rdiag_sensor1_immediate_res_msg) if
  any. In this implementation NULL pointer is returned as we are
  committing our response to DIAG within the handler function.
*/
void*
sns_rdiag_msg_handler(byte *req_pkt, uint16_t pkt_len, sns_rdiag_pkt_type pkt_type)
{
  sns_rdiag_msg_body *parsed_msg = (sns_rdiag_msg_body*) req_pkt;

  if(0 == pkt_len || NULL == req_pkt)
  {
    LOGE("%s:Invalid Request (size 0 or NULL ptr)", __func__);
    sns_rdiag_send_immed_resp(SENSOR1_EBAD_PARAM, 0, NULL, 0, 0, 1);
  }
  else
  {
    if(SNS_DIAG_SENSOR1_OPEN == parsed_msg->cm_fn)
    {
      sns_rdiag_msg_handle_open(req_pkt, pkt_len);
    }
    else if(SNS_DIAG_SENSOR1_WRITE == parsed_msg->cm_fn)
    {
      sns_rdiag_msg_handle_write(req_pkt, pkt_len, pkt_type);
    }
    else if(SNS_DIAG_SENSOR1_CLOSE == parsed_msg->cm_fn)
    {
      sns_rdiag_msg_handle_close(req_pkt, pkt_len);
    }
    else if(SNS_DIAG_SENSOR1_WRITABLE == parsed_msg->cm_fn)
    {
      sns_rdiag_msg_handle_writable(req_pkt, pkt_len);
    }
    else
    {
      LOGE("%s: Unknown Sensor1 function = %d", __func__, parsed_msg->cm_fn);
    }
  }

  return NULL;
}

/*
  Handles a remote diag request packet.

  The diag packet sent will have the following format.

 function ||             header                        ||     byte1     ||  byte 2..n ||
 _________||___________________________________________||_______________||____________||
 send_data||Command_type|Subsystem|SNS Subsystem|Unused|| Packet Type   || Sensors Hdr||
          ||   128      |   64    | 3 = REMOTE DIAG | 0 ||2/3= Msg thru ||            ||
          ||                                           ||   sensor1     ||            ||

 ||   byte n..      ||
 ||_________________||
 ||   Sensors       ||
 ||   Payload       ||
 ||                 ||
 ||                 ||

  @param[i] req_pkt    : A pointer to the diag command packet
  @param[i] pkt_len        : Length of the command packet

  @return
  Immediate response if any. In this implementation NULL pointer
  is returned as we are committing our response to DIAG within the
  handler function.
*/
void *
sns_rdiag_request_handler_apps(void *req_pkt, uint16_t pkt_len)
{
  sns_rdiag_handle_req *sns_diag_req = (sns_rdiag_handle_req*)req_pkt;
  void *sns_diag_resp = NULL;
  uint16_t payload_size = 0;

  LOGI("%s: Remote Diag Req rcvd. pkt_len: %d", __func__, pkt_len);

  if(NULL == sns_diag_req ||
     pkt_len <= sizeof(sns_diag_req->header) + sizeof(sns_diag_req->pkt_type))
  {
    LOGE("%s: Invalid Remote Diag Request!", __func__);
  }
  else
  {
    /* Payload size to send to handler =
    Payload size - Diag header field length - Pkt Type field length */
    payload_size = pkt_len -
                  sizeof(sns_diag_req->header) - sizeof(sns_diag_req->pkt_type);

    if(SNS_DIAG_SENSOR1_QMI_MSG == sns_diag_req->pkt_type)
    {
      sns_diag_resp =
        sns_rdiag_msg_handler(sns_diag_req->payload, payload_size, 2);
    }
    else if(SNS_DIAG_SENSOR1_MSG == sns_diag_req->pkt_type)
    {
      sns_diag_resp =
        sns_rdiag_msg_handler(sns_diag_req->payload, payload_size, 3);
    }
    else
    {
      LOGE("%s: Received invalid packet type %i (len %i)", __func__,
          sns_diag_req->pkt_type, payload_size);
    }
  }

  return sns_diag_resp;
}

/*
  @brief Registers diag packet to receive remote diag request from QXDM

  @return
  SENSOR1_SUCCESS
  SENSOR1_EFAILED - Error inititalizing DIAG
*/
sensor1_error_e
sns_rdiag_register_diag()
{
  boolean sns_diag_init_b = FALSE;

  sns_diag_init_b = Diag_LSM_Init(NULL);
  if(!sns_diag_init_b)
  {
    LOGE("%s:Diag_LSM_Init() failed. Exiting Sensorrdiag...", __func__);
    rdiag_exit_flag = 1;

    return SENSOR1_EFAILED;
  }

  /* Registering diag packet for delayed responses with sensors
     subsystem id.
     To execute on QXDM :: "send_data 128 64 2 0 <Sensors Payload>"
  */
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F,
                                      DIAG_SUBSYS_SENSORS,
                                      sns_rdiag_tbl);

  return SENSOR1_SUCCESS;
}

/*
  Main function for the Sensor Remote Diag Proxy process.
  Handles starting the process, registering remote diag packet for
  delayed responses with sensors subsystem id.
  To execute on QXDM :: "send_data 128 64 3 0 <Sensors Payload>"
  Handles libsensor1 initialization.

  @param[i] argc: Count of arguments on the command line.
  @param[i] argv: Array of strings containing command line arguments.

  @return
  0 - no error
*/
int
main(int argc, char *argv[])
{
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);
  int err = 0;
  int i = 0;
  sensor1_error_e sns_err;

  LOGI("Sensorrdiag starting...");

  for(i = 0; i < SNS_RDIAG_MAX_CLIENTS; i++)
  {
    sns_rdiag_free_client_id(i);
  }

  for(i = 0; i < SNS_RDIAG_MAX_TXNS; i++)
  {
    memset(&delay_rsp_id_association_arr[i], 0x00, sizeof(delay_rsp_id_association));
  }

  rdiag_exit_flag = 0;

  pthread_mutex_init(&log_mutex, NULL);
  pthread_mutex_init(&rdiag_mutex, NULL);
  pthread_cond_init(&rdiag_cond, NULL);

  sns_err = sensor1_init();
  if(SENSOR1_SUCCESS != sns_err)
  {
    LOGE("%s: Sensor1_init failed %i", __func__, sns_err);
  }

  sns_rdiag_register_diag();
  pthread_mutex_lock(&rdiag_mutex);

  while(0 == rdiag_exit_flag && 0 == err)
  {
    err = pthread_cond_wait(&rdiag_cond, &rdiag_mutex);
  }

  if(0 != err)
  {
    LOGE("%s: pthread_cond_wait err(%d)", __func__, err);
  }

  LOGI("Sensorrdiag exiting...");

  pthread_mutex_unlock(&rdiag_mutex);
  return 0;
}
