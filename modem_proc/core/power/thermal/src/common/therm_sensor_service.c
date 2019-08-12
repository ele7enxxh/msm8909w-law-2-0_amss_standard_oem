/*============================================================================
  @file therm_sensor_service.c

  Thermal sensor QMI service implementation

  Copyright (c) 2012-2013 QUALCOMM Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/thermal/src/common/therm_sensor_service.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "therm_log.h"
#include "qmi_csi.h"
#include "thermal_sensor_service_v01.h"
#include "qmi_csi_target.h"
#include "qmi_sap.h"
#include "npa.h"
#include "npa_resource.h"
#include <stringl/stringl.h>
#include "DALSys.h"
#include "CoreMutex.h"
#include "thermal_sensor_service_impl_v01.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define TS_QMI_SENSOR_LIST_MAX   4
#define TS_QMI_MAX_CLIENT_NUM    4
#define TS_QMI_MAX_SEQ_NUM_SLOTS 4

#define THERM_K_TO_C(x) ((int32)(((int32)x) - 273))
#define THERM_C_TO_K(x) ((int32)(((int32)x) + 273))

typedef struct
{
  npa_event_handle     event_handle;
  npa_query_handle     query_handle;
  boolean              low_thresh_temp_valid;
  int                  low_thresh_temp;
  boolean              high_thresh_temp_valid;
  int                  high_thresh_temp;
  boolean              seq_num_valid;
  unsigned int         seq_num;
  unsigned int         sensor_idx;
  unsigned int         seqnum_idx;
  void                 *client;
  boolean              seq_num_slot_used;
} therm_ind_info_type;

typedef therm_ind_info_type arr_of_SEQNUM_therm_ind_info[TS_QMI_MAX_SEQ_NUM_SLOTS];

typedef struct
{
  qmi_client_handle             qmi_client_handle;
  arr_of_SEQNUM_therm_ind_info *ind_info;
  boolean                       connected;
} therm_qmi_ts_client_type;

static therm_qmi_ts_client_type therm_qmi_ts_client[TS_QMI_MAX_CLIENT_NUM];

static qmi_csi_service_handle therm_qmi_ts_service_handler = NULL;

typedef union {
  qmi_response_type_v01                      resp;
  ts_get_sensor_list_resp_msg_v01            sensor_list;
  ts_register_notification_temp_resp_msg_v01 reg_notify;
} ts_qmi_general_resp_v01_type;

/* Allocate response an indication message bodies outside of stack space */
static ts_qmi_general_resp_v01_type ts_qmi_resp;

/* Used for generating callback indications */
static ts_temp_report_ind_msg_v01   ts_qmi_ind_msg_cb_buffer;

/* Used for generating immediate indications */
static ts_temp_report_ind_msg_v01   ts_qmi_ind_msg_immediate;

static CoreMutexType *ts_service_mutex = NULL;

extern const char *therm_sensor_list[];

extern npa_node_definition therm_sensor_node;

static boolean ts_qmi_validate_connection_handle(void *connection_handle);

/* Cooreleates to ts_temp_report_type_enum_v01 */
static const char *report_type_str[] =
{
  "CURRENT",
  "HIGH",
  "LOW",
};

static qmi_sap_client_handle qsap_handle;

/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/**
  @brief therm_qmi_ts_send_indication_cb

  This function handles NPA event changes for mitigation resources.  Sends
  indications to registered connections.  Sanitizing function.

  @param  : context: User callback data provided to notify the correct qmi
                     connection
            event_type: Ignored
            data: New mitigation state information.
            data_size: Ignored

  @return : None.
*/
static void therm_qmi_ts_send_indication_cb( void         *context,
                                             unsigned int  event_type,
                                             void         *data,
                                             unsigned int  data_size )
{
  therm_qmi_ts_client_type *client         = NULL;
  npa_event_data           *event_data_ptr = NULL;
  therm_ind_info_type      *ind_info       = NULL;
  qmi_csi_error             qmi_error;
  boolean                   mutex_locked   = FALSE;

  do
  {
    if ((event_type != NPA_EVENT_THRESHOLD_LO) &&
        (event_type != NPA_EVENT_THRESHOLD_HI))
    {
      break;
    }

    if (context == NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "TS indication_cb: NULL context param");
      break;
    }

    if (data == NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "TS indication_cb: NULL data param");
      break;
    }

    ind_info = (therm_ind_info_type *)context;

    /* Insure callback data is not changed mid callback indication */
    Core_MutexLock(ts_service_mutex);
    mutex_locked = TRUE;

    if (ind_info->seq_num_slot_used == FALSE)
    {
      therm_log_printf(THERM_LOG_LEVEL_WARNING, 0,
                       "TS indication_cb: Stale data");
    }

    client = (therm_qmi_ts_client_type *)ind_info->client;

    if (ts_qmi_validate_connection_handle(client) == FALSE)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "TS indication_cb: Invalid client handle.");
      break;
    }

    if (ind_info->sensor_idx >= therm_sensor_node.resource_count)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                       "TS indication_cb: Invalid index %d",
                       ind_info->sensor_idx);
      break;
    }

    event_data_ptr = (npa_event_data*)data;

    strlcpy(ts_qmi_ind_msg_cb_buffer.sensor_id.sensor_id,
            therm_sensor_list[ind_info->sensor_idx],
            sizeof(ts_qmi_ind_msg_cb_buffer.sensor_id.sensor_id));

    ts_qmi_ind_msg_cb_buffer.temp = (float)THERM_K_TO_C(event_data_ptr->state);
    ts_qmi_ind_msg_cb_buffer.temp_valid = TRUE;

    ts_qmi_ind_msg_cb_buffer.seq_num_valid = ind_info->seq_num_valid;
    if (ind_info->seq_num_valid)
    {
      ts_qmi_ind_msg_cb_buffer.seq_num = ind_info->seq_num;
    }

    if (event_type == NPA_EVENT_THRESHOLD_LO)
    {
      ts_qmi_ind_msg_cb_buffer.report_type = QMI_TS_TEMP_REPORT_THRESHOLD_LOW_V01;
    }
    else
    {
      ts_qmi_ind_msg_cb_buffer.report_type = QMI_TS_TEMP_REPORT_THRESHOLD_HIGH_V01;
    }

    /* Set thresholds to min and max resource value */
    ind_info->seq_num_slot_used = FALSE;
    ind_info->seq_num = 0;
    ind_info->seq_num_valid = FALSE;
    ind_info->low_thresh_temp_valid  = FALSE;
    ind_info->low_thresh_temp = THERM_K_TO_C(0);
    ind_info->high_thresh_temp_valid = FALSE;
    ind_info->high_thresh_temp = THERM_K_TO_C(therm_sensor_node.resources[ind_info->sensor_idx].max);
    Core_MutexUnlock(ts_service_mutex);
    mutex_locked = FALSE;
    npa_set_event_thresholds(ind_info->event_handle, 0,
                             therm_sensor_node.resources[ind_info->sensor_idx].max);

    therm_log_printf(THERM_LOG_LEVEL_INFO, 4,
                     "TS Indication SENSOR: %s, TYPE: %s, TEMP: %d, SEQNUM: %d",
                     ts_qmi_ind_msg_cb_buffer.sensor_id.sensor_id,
                     report_type_str[ts_qmi_ind_msg_cb_buffer.report_type],
                     THERM_K_TO_C(event_data_ptr->state),
                     ts_qmi_ind_msg_cb_buffer.seq_num);


    qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                                 QMI_TS_TEMP_REPORT_IND_V01,
                                 &ts_qmi_ind_msg_cb_buffer,
                                 sizeof(ts_temp_report_ind_msg_v01));
    if (qmi_error != QMI_CSI_NO_ERR)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                       "TS Failure to send indication, Err: %d, Sensor %s",
                       qmi_error,
                       ts_qmi_ind_msg_cb_buffer.sensor_id.sensor_id);
    }
  } while (0);

  if (mutex_locked)
  {
    Core_MutexUnlock(ts_service_mutex);
  }
}

/**
  @brief therm_qmi_validate_request

  This function validates a request size and the device id.

  @param  : response: qmi_response_type_v01
            idx: message index
            req_c_struct_len: request struct length
            expected_req_c_struct_len: expected request struct
            length dev_id: Device ID log_str: Unique log string

  @return : TRUE if valid, FALSE otherwise
*/
static boolean therm_qmi_validate_request( qmi_response_type_v01  *response,
                                           unsigned int           *idx,
                                           unsigned int            req_c_struct_len,
                                           unsigned int            expected_req_c_struct_len,
                                           const char             *dev_id,
                                           const char             *log_str)
{
  unsigned int i;
  boolean retval = TRUE;

  do
  {
    /* Validate request. */
    if (req_c_struct_len != expected_req_c_struct_len)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s TS request wrong size: %d",
                       log_str, req_c_struct_len);
      response->result = QMI_RESULT_FAILURE_V01;
      response->error = QMI_ERR_MALFORMED_MSG_V01;
      retval = FALSE;
      break;
    }

    /* Does cooling device exist. */
    for (i = 0; i < therm_sensor_node.resource_count; i++ )
    {
      if (strcasecmp(therm_sensor_list[i], dev_id) == 0)
      {
        /* Found a match */
        *idx = i;
        break;
      }
    }

    if (i >= therm_sensor_node.resource_count)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                       "%s request on non-existent TS",
                       log_str);
      response->result = QMI_RESULT_FAILURE_V01;
      response->error = QMI_ERR_INVALID_ID_V01;
      retval = FALSE;
      break;
    }
  } while (0);

  return retval;
}

/**
  @brief ts_qmi_validate_connection_handle.

  This function validates a connection handle.

  @param  : connection_handle: Handle to validate.

  @return : TRUE if valid, FALSE otherwise
*/
static boolean ts_qmi_validate_connection_handle(void *connection_handle)
{
  unsigned int i;
  boolean retval = TRUE;

  do
  {
    /* Validate handle. */
    if (connection_handle == NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Connection handle NULL");
      retval = FALSE;
      break;
    }

    Core_MutexLock(ts_service_mutex);
    /* Find matching client slot */
    for (i = 0; i < TS_QMI_MAX_CLIENT_NUM; i++)
    {
      if (&therm_qmi_ts_client[i] ==
          (therm_qmi_ts_client_type *)connection_handle)
      {
        /* Found client */
        break;
      }
    }
    Core_MutexUnlock(ts_service_mutex);

    if (i >= TS_QMI_MAX_CLIENT_NUM)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid connection handle");
      retval = FALSE;
      break;
    }

  } while (0);

  return retval;
}


/**
  @brief therm_qmi_ts_connect

  This function processes incoming requests for the QMI thermal
  ts services to connect a client.

  @param  : client_handle: Handle used by the infrastructure
                           to identify different services.
            service_cookie: Ignored
            connection_handle: Client connection to create
                               internally.

  @return : None.
*/
static qmi_csi_cb_error therm_qmi_ts_connect(qmi_client_handle  client_handle,
                                             void              *service_cookie,
                                             void             **connection_handle)
{
  unsigned int i, j;
  qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
  therm_qmi_ts_client_type *client   = NULL;
  therm_ind_info_type      *ind_info = NULL;

  Core_MutexLock(ts_service_mutex);
  do
  {
    /* Find empty client slot */
    for (i = 0; i < TS_QMI_MAX_CLIENT_NUM; i++)
    {
      if (therm_qmi_ts_client[i].connected == FALSE)
      {
        /* Found empty slot */
        therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "TS found empty client slot: %d", i);
        break;
      }
    }

    if (i >= TS_QMI_MAX_CLIENT_NUM)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "TS no client connections available");
      retval = QMI_CSI_CB_NO_MEM;
      break;
    }

    /* Fill out slot data */
    client = &therm_qmi_ts_client[i];

    for (i = 0; i < therm_sensor_node.resource_count; i++)
    {
      for (j = 0; j < TS_QMI_MAX_SEQ_NUM_SLOTS; j++)
      {
        ind_info = &client->ind_info[i][j];
        ind_info->seq_num_slot_used = FALSE;
        ind_info->seq_num = 0;
        ind_info->seq_num_valid = FALSE;
        ind_info->low_thresh_temp_valid  = FALSE;
        ind_info->low_thresh_temp = THERM_K_TO_C(0);
        ind_info->high_thresh_temp_valid = FALSE;
        ind_info->high_thresh_temp = THERM_K_TO_C(therm_sensor_node.resources[i].max);
        ind_info->sensor_idx = i;
        ind_info->seqnum_idx = j;
      }
    }

    client->qmi_client_handle = client_handle;
    client->connected = TRUE;
    *connection_handle = client;
    therm_log_printf(THERM_LOG_LEVEL_INFO, 0, "TS Connect client");
  } while (0);

  Core_MutexUnlock(ts_service_mutex);

  return retval;
}

/**
  @brief therm_qmi_ts_disconnect

  This function processes incoming requests for the QMI thermal
  mitigation device services to disconnect a client. Sanitizing
  function.

  @param  : connection_handle: Client connection to
                               disconnect.
            service_cookie: Ignored

  @return : None.
*/
static void therm_qmi_ts_disconnect(void  *connection_handle,
                                    void  *service_cookie)
{
  unsigned int i, j;
  therm_qmi_ts_client_type *client = connection_handle;
  therm_ind_info_type      *ind_info = NULL;

  do
  {
    if (ts_qmi_validate_connection_handle(client) == FALSE)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "TS Disconnect invalid client");
      break;
    }

    /* Destroy NPA handles */
    for (i = 0; i < therm_sensor_node.resource_count; i++)
    {
      for (j = 0; j < TS_QMI_MAX_SEQ_NUM_SLOTS; j++)
      {
        Core_MutexLock(ts_service_mutex);
        ind_info = &client->ind_info[i][j];
        ind_info->seq_num_slot_used = FALSE;
        ind_info->seq_num = 0;
        ind_info->seq_num_valid = FALSE;
        ind_info->low_thresh_temp_valid  = FALSE;
        ind_info->low_thresh_temp = THERM_K_TO_C(0);
        ind_info->high_thresh_temp_valid = FALSE;
        ind_info->high_thresh_temp = THERM_K_TO_C(therm_sensor_node.resources[i].max);
        Core_MutexUnlock(ts_service_mutex);
        npa_set_event_thresholds(ind_info->event_handle, 0,
                                 therm_sensor_node.resources[i].max);
      }
    }

    Core_MutexLock(ts_service_mutex);
    /* Free client slot from being connected */
    client->connected = FALSE;
    Core_MutexUnlock(ts_service_mutex);

    therm_log_printf(THERM_LOG_LEVEL_INFO, 0, "TS Disconnect client");
  } while (0);
}

/**
  @brief therm_handle_ts_list_req

  This function processes incoming requests for the QMI thermal
  sensor services with msg_id QMI_TS_GET_SENSOR_LIST_RESP_V01.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/
static qmi_csi_cb_error therm_handle_ts_list_req(void           *connection_handle,
                                                 qmi_req_handle  req_handle,
                                                 unsigned int    msg_id,
                                                 void           *req_c_struct,
                                                 unsigned int    req_c_struct_len,
                                                 void           *service_cookie)
{
  ts_get_sensor_list_resp_msg_v01 *response =
    (ts_get_sensor_list_resp_msg_v01 *)&ts_qmi_resp;
  unsigned int i;

  /* Init response data */
  memset( response, 0,
          sizeof(ts_get_sensor_list_resp_msg_v01));

  Core_MutexLock(ts_service_mutex);
  do
  {
    response->sensor_list_valid = 1;
    response->sensor_list_len = therm_sensor_node.resource_count;

    therm_log_printf(THERM_LOG_LEVEL_INFO, 0,
                     "TS Get sensor list request");

    /* Copy cooling device info over to response. */
    for (i = 0; i < therm_sensor_node.resource_count; i++ )
    {
      therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                       "TS Sensor %d %s", i, therm_sensor_list[i]);
      strlcpy(response->sensor_list[i].sensor_id,
              therm_sensor_list[i],
              sizeof(response->sensor_list[i].sensor_id));
    }

    response->resp.result = QMI_RESULT_SUCCESS_V01;
    response->resp.error  = QMI_ERR_NONE_V01;
  } while (0);
  Core_MutexUnlock(ts_service_mutex);

  qmi_csi_send_resp(req_handle, msg_id, response,
                    sizeof(ts_get_sensor_list_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/**
  @brief therm_handle_ts_reg_notify_req

  This function processes incoming requests for the QMI thermal
  sensor services with msg_id
  QMI_TS_REGISTER_NOTIFICATION_TEMP_REQ_V01.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/
static qmi_csi_cb_error therm_handle_ts_reg_notify_req(void           *connection_handle,
                                                       qmi_req_handle  req_handle,
                                                       unsigned int    msg_id,
                                                       void           *req_c_struct,
                                                       unsigned int    req_c_struct_len,
                                                       void           *service_cookie)
{
  ts_register_notification_temp_req_msg_v01  *request =
    (ts_register_notification_temp_req_msg_v01 *)req_c_struct;
  qmi_response_type_v01 *response = (qmi_response_type_v01 *)&ts_qmi_resp;
  therm_qmi_ts_client_type *client = connection_handle;

  unsigned int         sensor_idx, seqnum_idx;
  boolean              modify_existing_req = FALSE;
  boolean              set_thresh_req      = FALSE;
  boolean              send_immediate_resp = FALSE;
	 therm_ind_info_type *ind_info = NULL;

  /* Init response data */
  memset( response, 0, sizeof(qmi_response_type_v01) );

  Core_MutexLock(ts_service_mutex);
  do
  {
    /* Validate request. */
    if (therm_qmi_validate_request(response, &sensor_idx, req_c_struct_len,
                                   sizeof(ts_register_notification_temp_req_msg_v01),
                                   request->sensor_id.sensor_id,
                                   "TS register notify") == FALSE)
    {
      break;
    }

    /* Look for free indication slot for seq num */
    if (request->seq_num_valid)
	   {
      unsigned int first_free_idx = 0;

      /* Start at seqnum_idx 1, since zero is reserved for reg notify without
         seq_num argument. Search all slots in case a pending request with
         matching seq_num already exists. */
      for ( seqnum_idx = 1; seqnum_idx < TS_QMI_MAX_SEQ_NUM_SLOTS; seqnum_idx++)
      {
        ind_info = &client->ind_info[sensor_idx][seqnum_idx];
        if ((ind_info->seq_num_slot_used) &&
            (ind_info->seq_num == request->seq_num))
        {
          /* Modifying existing notify request. */
          modify_existing_req = TRUE;
          therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                           "TS reg. notify mod. existing request %s seq_num %d",
                           therm_sensor_node.resources[sensor_idx].name,
                           request->seq_num);
          break;
        }

        if ((ind_info->seq_num_slot_used == FALSE) &&
            (first_free_idx == 0))
        {
          first_free_idx = seqnum_idx;
          therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                           "TS reg. notify free idx found for %s idx %d",
                           therm_sensor_node.resources[sensor_idx].name,
                           seqnum_idx);
        }
      }

      if ((modify_existing_req == FALSE) && (first_free_idx != 0))
      {
        /* Free slot found. */
        seqnum_idx = first_free_idx;
      }

      if (seqnum_idx >= TS_QMI_MAX_SEQ_NUM_SLOTS)
      {
        /* No empty slot or existing request found. */
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "TS reg. notify no free ind. slot found for %s",
                         therm_sensor_node.resources[sensor_idx].name);
        response->result = QMI_RESULT_FAILURE_V01;
        response->error  = QMI_ERR_DEVICE_IN_USE_V01;
        break;
      }
    }
    else
    {
      /* Use seqnum_idx 0 for requests without a valid seq_num*/
      seqnum_idx = 0;
      modify_existing_req = client->ind_info[sensor_idx][0].seq_num_slot_used;
    }

    ind_info = &client->ind_info[sensor_idx][seqnum_idx];


    /* Cancel existing registration */
    if (modify_existing_req)
    {
      /* Set thersholds to min and max resource value */
      ind_info->seq_num_slot_used = FALSE;
      ind_info->seq_num = 0;
      ind_info->seq_num_valid       = FALSE;
      ind_info->low_thresh_temp_valid  = FALSE;
      ind_info->low_thresh_temp = THERM_K_TO_C(0);
      ind_info->high_thresh_temp_valid = FALSE;
      ind_info->high_thresh_temp = THERM_K_TO_C(therm_sensor_node.resources[sensor_idx].max);
    }

    /* Start processing request */
    if (request->send_current_temp_report)
    {
      npa_query_type query_result;

      /* Send inidcation immediately. */
      memset(&ts_qmi_ind_msg_immediate, 0, sizeof(ts_qmi_ind_msg_immediate));

      /* Copy sensor_id to response. */
      strlcpy(ts_qmi_ind_msg_immediate.sensor_id.sensor_id,
              request->sensor_id.sensor_id,
              sizeof(ts_qmi_ind_msg_immediate.sensor_id.sensor_id));

      /* Current temperature request. */
      ts_qmi_ind_msg_immediate.report_type = QMI_TS_TEMP_REPORT_CURRENT_TEMP_V01;

      if (NPA_QUERY_SUCCESS != npa_query(ind_info->query_handle,
                                         NPA_QUERY_TYPE_STATE,
                                         &query_result))
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "TS reg. notify query state %s",
                         therm_sensor_node.resources[sensor_idx].name);
        response->result = QMI_RESULT_FAILURE_V01;
        response->error  = QMI_ERR_INTERNAL_V01;
        break;
      }
      ts_qmi_ind_msg_immediate.temp_valid = TRUE;
      ts_qmi_ind_msg_immediate.temp =
        (float)THERM_K_TO_C(query_result.data.state);

      /* Sequence number */
      if (request->seq_num_valid)
      {
        ts_qmi_ind_msg_immediate.seq_num_valid = TRUE;
        ts_qmi_ind_msg_immediate.seq_num = request->seq_num;
      }

      send_immediate_resp = TRUE;
    }
    else
    {
      if ((request->temp_threshold_high_valid) &&
          (request->temp_threshold_low_valid) &&
          (request->temp_threshold_low >= request->temp_threshold_high))
      {
        /* Invalid request lo >= hi. */
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "TS reg. notify invalid lo >= hi set %s",
                         therm_sensor_node.resources[sensor_idx].name);
        response->result = QMI_RESULT_FAILURE_V01;
        response->error  = QMI_ERR_INVALID_ARG_V01;
        break;
      }

      if ((request->temp_threshold_low_valid) &&
          ((((int)request->temp_threshold_low) < THERM_K_TO_C(0)) ||
           (((int)request->temp_threshold_low) >
            THERM_K_TO_C(therm_sensor_node.resources[sensor_idx].max))))
      {
        /* Invalid request low threshold. */
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                         "TS reg. notify invalid lo %s, %dC",
                         therm_sensor_node.resources[sensor_idx].name,
                         ((int)request->temp_threshold_low));
        response->result = QMI_RESULT_FAILURE_V01;
        response->error  = QMI_ERR_INVALID_ARG_V01;
        break;
      }

      if ((request->temp_threshold_high_valid) &&
          (((((int)request->temp_threshold_high) < THERM_K_TO_C(0)) ||
            (((int)request->temp_threshold_high) >
             THERM_K_TO_C(therm_sensor_node.resources[sensor_idx].max)))))
      {
        /* Invalid request high threshold. */
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                         "TS reg. notify invalid high %s, %dC",
                         therm_sensor_node.resources[sensor_idx].name,
                         ((int)request->temp_threshold_high));
        response->result = QMI_RESULT_FAILURE_V01;
        response->error  = QMI_ERR_INVALID_ARG_V01;
        break;
      }

      if (((request->temp_threshold_high_valid == FALSE) &&
           (request->temp_threshold_low_valid == FALSE)) &&
          (modify_existing_req))
      {
        /* Cancel existing request. */
        therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                         "TS reg. notify cancel existing request on %s",
                         therm_sensor_node.resources[sensor_idx].name);
        response->result = QMI_RESULT_SUCCESS_V01;
        response->error  = QMI_ERR_NONE_V01;
        break;
      }

      if (((request->temp_threshold_high_valid == FALSE) &&
           (request->temp_threshold_low_valid == FALSE)) &&
          (!modify_existing_req))
      {
        /* Request no threshold set. */
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "TS reg. notify no thresholds set %s",
                         therm_sensor_node.resources[sensor_idx].name);
        response->result = QMI_RESULT_FAILURE_V01;
        response->error  = QMI_ERR_INVALID_ARG_V01;
        break;
      }

      /* Setup ind_info data. */
      ind_info->high_thresh_temp_valid = request->temp_threshold_high_valid;
      if (ind_info->high_thresh_temp_valid)
      {
        ind_info->high_thresh_temp = (int)request->temp_threshold_high;
      }
      else
      {
        ind_info->high_thresh_temp =
          (int)THERM_K_TO_C(therm_sensor_node.resources[sensor_idx].max);
      }

      ind_info->low_thresh_temp_valid = request->temp_threshold_low_valid;
      if (ind_info->low_thresh_temp_valid)
      {
        ind_info->low_thresh_temp = (int)request->temp_threshold_low;
      }
      else
      {
        ind_info->low_thresh_temp = (int)THERM_K_TO_C(0);
      }

      ind_info->seq_num_valid = request->seq_num_valid;
      if (ind_info->seq_num_valid)
      {
        ind_info->seq_num = request->seq_num;
      }

      /* Only mark the slot as used if notify reg. is successfully processed. */
      ind_info->seq_num_slot_used = TRUE;
      set_thresh_req = TRUE;
    }

    therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                     "TS reg. notify event handle for %s",
                     therm_sensor_node.resources[sensor_idx].name);

    response->result = QMI_RESULT_SUCCESS_V01;
    response->error  = QMI_ERR_NONE_V01;

  } while (0);

  Core_MutexUnlock(ts_service_mutex);

  qmi_csi_send_resp(req_handle, msg_id, response,
                    sizeof(qmi_response_type_v01));

  if (send_immediate_resp)
  {
    qmi_csi_error qmi_error;

    qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                                 QMI_TS_TEMP_REPORT_IND_V01,
                                 &ts_qmi_ind_msg_immediate,
                                 sizeof(ts_temp_report_ind_msg_v01));
    if (QMI_CSI_NO_ERR == qmi_error)
    {
      therm_log_printf(THERM_LOG_LEVEL_INFO, 4,
                       "TS Indication SENSOR: %s, TYPE: %s, TEMP: %d, SEQNUM: %d",
                       therm_sensor_list[sensor_idx],
                       report_type_str[QMI_TS_TEMP_REPORT_CURRENT_TEMP_V01],
                       (int)ts_qmi_ind_msg_immediate.temp,
                       ts_qmi_ind_msg_immediate.seq_num);
    }
    else
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                       "TS reg. notify failure to send ind. for %s, Err %d",
                       therm_sensor_node.resources[sensor_idx].name,
                       qmi_error);
    }
  }

  if (set_thresh_req || modify_existing_req)
  {
      npa_set_event_thresholds(ind_info->event_handle,
                               THERM_C_TO_K(ind_info->low_thresh_temp),
                               THERM_C_TO_K(ind_info->high_thresh_temp));
  }

  return QMI_CSI_CB_NO_ERR;
}


/**
  @brief therm_qmi_ts_process_req

  This function processes incoming requests for the QMI thermal
  sensor services handler. Sanitizing function.
  req_c_struct and req_c_struct_len are validated in handling
  functions.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/
static qmi_csi_cb_error therm_qmi_ts_process_req(void           *connection_handle,
                                                 qmi_req_handle  req_handle,
                                                 unsigned int    msg_id,
                                                 void           *req_c_struct,
                                                 unsigned int    req_c_struct_len,
                                                 void           *service_cookie)
{
  qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
  qmi_response_type_v01 *response = (qmi_response_type_v01 *)&ts_qmi_resp;

  if ((ts_qmi_validate_connection_handle(connection_handle) == FALSE) ||
      (req_handle == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "Invalid ts request arguments, request id: 0x%x", msg_id);
    response->result = QMI_RESULT_FAILURE_V01;
    response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
    return (qmi_csi_cb_error)QMI_CSI_INVALID_HANDLE;
  }

  switch(msg_id)
  {
    case QMI_TS_GET_SENSOR_LIST_REQ_V01:
      retval = therm_handle_ts_list_req(connection_handle, req_handle, msg_id,
                                         req_c_struct, req_c_struct_len,
                                         service_cookie);
      break;
    case QMI_TS_REGISTER_NOTIFICATION_TEMP_REQ_V01:
      retval = therm_handle_ts_reg_notify_req(connection_handle, req_handle,  msg_id,
                                              req_c_struct, req_c_struct_len,
                                              service_cookie);
      break;
 default:
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Unhandled ts request id: 0x%x",
                       msg_id);
      response->result = QMI_RESULT_FAILURE_V01;
      response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
      qmi_csi_send_resp(req_handle, msg_id, response, sizeof(qmi_response_type_v01));
      break;
  }
  return retval;
}

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief therm_qmi_sensor_service_init

  This function initializes the QMI thermal sensors services
  handler.

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
qmi_csi_service_handle therm_qmi_sensor_service_init(qmi_csi_os_params *os_params)
{
  qmi_idl_service_object_type ts_service_object = NULL;
  do
  {
    unsigned int client_idx, sensor_idx, seqnum_idx;
    therm_qmi_ts_client_type *client   = NULL;
    therm_ind_info_type      *ind_info = NULL;
    DALResult                 result = DAL_SUCCESS;
    unsigned int ind_data_size = sizeof(therm_ind_info_type) *
                                 therm_sensor_node.resource_count *
                                 TS_QMI_MAX_SEQ_NUM_SLOTS;

    ts_service_mutex = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT);
    if (ts_service_mutex == NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "QMI ts service Core_MutexCreate error");
      break;
    }

    DALSYS_memset(&therm_qmi_ts_client, 0, sizeof(therm_qmi_ts_client));

    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "QMI ts indication data size %d",
                     ind_data_size);

    for (client_idx = 0; client_idx < ARR_SIZE(therm_qmi_ts_client);
          client_idx++)
    {
      client = &therm_qmi_ts_client[client_idx];
      result = DALSYS_Malloc(ind_data_size,
                            (void **)&(client->ind_info));
      if (result != DAL_SUCCESS)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "QMI ts service malloc error");
        break;
      }

      DALSYS_memset(client->ind_info, 0, ind_data_size);

      for (sensor_idx = 0; sensor_idx < therm_sensor_node.resource_count;
            sensor_idx++)
      {
        for (seqnum_idx = 0; seqnum_idx < TS_QMI_MAX_SEQ_NUM_SLOTS; seqnum_idx++)
        {
          ind_info = &client->ind_info[sensor_idx][seqnum_idx];
          ind_info->client = client;
          ind_info->sensor_idx = sensor_idx;
          ind_info->seqnum_idx = seqnum_idx;

          ind_info->query_handle =
            npa_create_query_handle(therm_sensor_node.resources[sensor_idx].name);
          if (ind_info->query_handle == NULL)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                             "TS failed to create query handler for %s",
                             therm_sensor_node.resources[sensor_idx].name);
            result = DAL_ERROR;
            break;
          }

          ind_info->event_handle = npa_create_event_cb(
                                   therm_sensor_node.resources[sensor_idx].name,
                                   "TS Event",
                                   NPA_TRIGGER_THRESHOLD_EVENT,
                                   therm_qmi_ts_send_indication_cb,
                                   (void *) ind_info);
          if (ind_info->event_handle == NULL)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                             "TS failed to create event handler for %s",
                             therm_sensor_node.resources[sensor_idx].name);
            result = DAL_ERROR;
            break;
          }
        }
        if (result != DAL_SUCCESS)
        {
          break;
        }
      }

      if (result != DAL_SUCCESS)
      {
        break;
      }
    }

    if (result != DAL_SUCCESS)
    {
      break;
    }

    ts_service_object = ts_get_service_object_v01();

    /* Register QMI TS service for logging of supported interfaces */
    (void) qmi_si_register_object( ts_service_object,
                                   0, /* Service Instance */
                                   ts_get_service_impl_v01() );

    if (qmi_csi_register(ts_service_object, therm_qmi_ts_connect,
                         therm_qmi_ts_disconnect,
                         therm_qmi_ts_process_req,
                         NULL, os_params,
                         &therm_qmi_ts_service_handler) != QMI_CSI_NO_ERR)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "QMI ts service register error");
    }
    else
    {
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 0, "QMI ts service register okay");
      if (QMI_SAP_NO_ERR != qmi_sap_register(ts_service_object, NULL, &qsap_handle))
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                         "QMI ts service QSAP register error");
      }
      else
      {
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 0, "QMI ts service QSAP register okay");
      }
    }
  } while (0);

  return therm_qmi_ts_service_handler;
}

