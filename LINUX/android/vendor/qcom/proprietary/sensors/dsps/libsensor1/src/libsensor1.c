/*=============================================================================
  @file libsensor1.c

  @brief Defines the types used in the sensor1 remoting library for sensor
  service communication.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "qmi_idl_lib.h"
#include "qmi_client.h"

#include "libsensor1_osa.h"
#include "libsensor1.h"
#include "sensor1.h"
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_smgr_api_v01.h"
#include "sns_common_v01.h"
#include "sns_time_api_v02.h"

/*============================================================================
  Forward Declarations
  ============================================================================*/

struct sensor1_client;

/*============================================================================
  Type Declarations
  ============================================================================*/

typedef struct sensor1_resp {
  /* The client associated with this outstanding response message */
  sensor1_client *client;
  /* Transaction handle, as provided by QMI */
  qmi_txn_handle txn_handle;
  /* Sensor1 header of the request message */
  sensor1_msg_header_s header;
  /* Pre-allocated response message buffer */
  void *msg_ptr;
} sensor1_resp;

/* A client waiting on some sensor service.  Used for writable and RETRY_OPEN */
typedef struct sensor1_waiting_client
{
  /* Callback data provided by the client in sensor1_open or sensor1_writable */
  intptr_t cb_data;
  /* Sensor1 service number */
  uint32_t sns_svc_num;

  /* Callback functions; NULL if not applicable */
  sensor1_notify_data_cb_t notify_cb;
  sensor1_write_cb_t writable_cb;

  /* In ms.  '0' if infinite timeout */
  uint32_t timeout;
} sensor1_waiting_client;

/*=============================================================================
 * Static Data Definitions
 ============================================================================*/
log_level sensor1_log_level = LOG_LEVEL_WARN;

/* Linked-list of all registered clients */
static sensor1_client *clients;
static OS_EVENT *clients_mutex;

/* Ensure Request messages are logged prior to their response. */
static OS_EVENT *log_mutex;

/* Variable to control pthread_once init */
static void *init_ctl = PTHREAD_ONCE_INIT;

/* Each time a client calls sensor1_open, we check that the SMGR service
 * is available.  In essence this is our check that the SSC is up, running,
 * and available.  This specifies how long to wait. */
static const uint32_t SMGR_TIMEOUT_MS = 5000;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* Validate handle, check for BAD sensor handle */
static bool
validate_hndl( sensor1_client *client )
{
  uint8_t err = 0;
  sns_os_mutex_pend( clients_mutex, 0, &err );
  sensor1_client *temp = clients;
  while( NULL != temp ) {
    if( client == temp ) {
      sns_os_mutex_post( clients_mutex );
      return true;
    }
    temp = temp->next;
  }
  sns_os_mutex_post( clients_mutex );
  return false;
}

/**
 * Add a pending message intended for the given client.
 *
 * @param[i] head Whether to place this message at the head or tail of the list
 */
static void
client_add_msg( sensor1_client *client, sensor1_msg const *msg_in, bool head )
{
  uint8_t err = 0;
  sensor1_msg *msg = SNS_OS_MALLOC( 0, sizeof(sensor1_msg) );

  if( NULL == msg ) {
    LOG_ERROR("%s:SNS_OS_MALLOC Failed", __func__);
    return;
  }

  SNS_OS_MEMSCPY( msg, sizeof(sensor1_msg), msg_in, sizeof(sensor1_msg) );

  sns_os_mutex_pend( client->client_mutex, 0, &err );
  if( head || NULL == client->messages ) {
    msg->next = client->messages;
    client->messages = msg;
  } else {
    sensor1_msg *curr = client->messages;
    while( NULL != curr->next ) {
      curr = curr->next;
    }
    curr->next = msg;
    msg->next = NULL;
  }
  sns_os_mutex_post( client->client_mutex );
  sns_os_sem_post( client->messages_sem, &err );
}

/* QMI response message callback.  See qmi_client_recv_msg_async_cb. */
static void
resp_msg_cb ( qmi_client_type user_handle, unsigned int msg_id,
    void *resp_c_struct, unsigned int resp_c_struct_len,
    void *resp_cb_data, qmi_client_error_type transp_err )
{
  UNREFERENCED_PARAMETER(msg_id);
  UNREFERENCED_PARAMETER(user_handle);
  sensor1_resp *resp = (sensor1_resp*)resp_cb_data;
  uint8_t err;
  sensor1_msg msg =
  { .header = resp->header,
    .msg_type = SENSOR1_MSG_TYPE_RESP,
    .msg = resp_c_struct
  };

  msg.header.msg_size = resp_c_struct_len;
  if( QMI_NO_ERR != transp_err ) {
    sns_common_resp_s_v01 *resp_msg = (sns_common_resp_s_v01*)resp_c_struct;
    LOG_ERROR("%s: QMI transport error %i from svc %i on %p", __func__,
        transp_err, resp->header.service_number, (void*)resp->client);

    msg.header.msg_size = sizeof(sns_common_resp_s_v01);
    resp_msg->sns_result_t = SNS_RESULT_FAILURE_V01;
    resp_msg->sns_err_t = SENSOR1_EFAILED;
  } else {
    LOG_DEBUG("%s: Received response from %i on %p",
        __func__, resp->header.service_number, (void*)resp->client);
  }

  sns_os_mutex_pend( log_mutex, 0, &err );
  log_resp( resp->client, &msg.header, resp_c_struct );
  sns_os_mutex_post( log_mutex );
  client_add_msg( resp->client, &msg, false );

  SNS_OS_FREE( resp_cb_data );
  return;
}

/* QMI indication callback.  See qmi_client_ind_cb. */
static void
ind_msg_cb( qmi_client_type user_handle, unsigned int msg_id,
    void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data )
{
  UNREFERENCED_PARAMETER(user_handle);
  sensor1_client_conn *client_conn = (sensor1_client_conn*)ind_cb_data;
  uint32_t ind_len;
  int32_t qmi_err;

  if( NULL != client_conn ) {
    qmi_idl_service_object_type svc_obj =
      get_qmi_svc_obj( client_conn->sns_svc_num );
    LOG_VERBOSE("%s: Received indication from %i on %p",
        __func__, client_conn->sns_svc_num, (void*)client_conn->client);

    qmi_err = qmi_idl_get_message_c_struct_len( svc_obj,
        QMI_IDL_INDICATION, msg_id, &ind_len );
    if( QMI_IDL_LIB_NO_ERR != qmi_err ) {
      LOG_ERROR("%s: qmi_idl_get_message_c_struct_len error %i", __func__, qmi_err);
    } else {
      uint8_t err;
      void *ind_c_struct = SNS_OS_MALLOC( 0, ind_len );

      qmi_err = qmi_idl_message_decode( svc_obj, QMI_IDL_INDICATION, msg_id,
          ind_buf, ind_buf_len, ind_c_struct, ind_len );
      if( qmi_err != QMI_IDL_LIB_NO_ERR ) {
        LOG_ERROR("%s: QMI decode error %i", __func__, qmi_err);
      } else {
        sensor1_msg msg =
          { .header.service_number = client_conn->sns_svc_num,
            .header.msg_id = msg_id,
            .header.msg_size = ind_len,
            .header.txn_id = 0,
            .msg_type = SENSOR1_MSG_TYPE_IND,
            .msg = ind_c_struct
          };

        sns_os_mutex_pend( log_mutex, 0, &err );
        log_ind( client_conn->client, &msg.header, ind_buf, ind_buf_len );
        sns_os_mutex_post( log_mutex );
        client_add_msg( client_conn->client, &msg, false );
      }
    }
  }
}

/* QMI Error callback.  See qmi_client_error_cb. */
static void
error_cb( qmi_client_type user_handle,
    qmi_client_error_type error, void *err_cb_data )
{
  UNREFERENCED_PARAMETER(user_handle);
  sensor1_client_conn *client_conn = (sensor1_client_conn*)err_cb_data;

  LOG_ERROR("%s: QMI error callback %i on %p", __func__, error,
      (void*)client_conn->client);

  sensor1_msg msg =
    { .msg_type = SENSOR1_MSG_TYPE_BROKEN_PIPE,
      .msg = NULL };

  client_add_msg( client_conn->client, &msg, false );

  return;
}

/**
 * Check whether a Sensor service is available.
 *
 * @param[i] sns_svc_num Sensor1 service number
 * @param[i] timeout Timeout period in ms; 0 if infinite
 *
 * @return true if the service is available, false if timeout
 */
static bool
wait_for_service( uint32_t sns_svc_num, uint32_t timeout )
{
  qmi_client_type notifier_handle;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;
  bool rv = true;

  LOG_DEBUG("%s: Waiting for service %i timeout %u", __func__, sns_svc_num, timeout);

  qmi_err = qmi_client_notifier_init( get_qmi_svc_obj( sns_svc_num ),
      &os_params, &notifier_handle );
  if( QMI_NO_ERR != qmi_err ) {
    LOG_ERROR("%s: qmi_client_notifier_init error %i (svc %i)",
        __func__, qmi_err, sns_svc_num);
    rv = false;
  } else {
    QMI_CCI_OS_SIGNAL_WAIT( &os_params, timeout );
    QMI_CCI_OS_SIGNAL_CLEAR( &os_params );
    if( os_params.timed_out ) {
      LOG_ERROR("%s: Service %i is not available (%i ms)", __func__,
          sns_svc_num, timeout );
      rv = false;
    }
  }

  if( rv ) {
    rv = sensor1_enabled_check();
  }

  qmi_client_release(notifier_handle);

  LOG_DEBUG("%s: Waiting complete for service %i (rv %i)",
      __func__, sns_svc_num, rv);

  return rv;
}

/**
 * Thread for clients waiting from sensor1_open or sensor1_writable.
 *
 * @param[i] arg Client-specific information
 */
static void
waiting_thread( void *arg )
{
  sensor1_waiting_client *client = (sensor1_waiting_client*)arg;
  bool found;

  LOG_DEBUG("%s: Entered waiting thread %i", __func__, client->sns_svc_num);

  found = wait_for_service( client->sns_svc_num, client->timeout );

  if( found ) {
    sensor1_enabled_wait();

    if( NULL != client->notify_cb ) {
      client->notify_cb( client->cb_data, NULL,
          SENSOR1_MSG_TYPE_RETRY_OPEN, NULL );
    } else if( NULL != client->writable_cb ) {
      client->writable_cb( client->cb_data,
          client->sns_svc_num );
    }
  }

  LOG_DEBUG("%s: Exited waiting thread %i", __func__, client->sns_svc_num);

  SNS_OS_FREE( client );
}

/**
 * Given a list of available service instances, choose the one with the
 * greatest instance ID.
 *
 * @param[i] svc_info_array List of services found by QMI
 * @param[i] num_entries How many entires in svc_info_array are valid
 * @param[i] sns_svc_num Service number
 *
 * @return The chosen service instance to use
 */
static qmi_service_instance
choose_svc_instance( qmi_service_info svc_info_array[ 5 ], uint32_t num_entries, uint32_t sns_svc_num )
{
  qmi_client_error_type qmi_err;
  uint32_t i, max_idx;
  qmi_service_instance instance_id, max_iid;

  for( i = 0, max_idx = 0, max_iid = 0; i < num_entries; i++ ) {
    if( QMI_NO_ERR == ( qmi_err =
        qmi_client_get_instance_id( &svc_info_array[ i ], &instance_id) )) {
      if( max_iid < instance_id ) {
        max_idx = i;
        max_iid = instance_id;
      }
    }
  }

  if( max_iid == SNS_SSC_IMPL_V02 &&
      sns_svc_num == SNS_TIME2_SVC_ID_V01 ){
    return SENSOR_TIME_INSTANCE_ID_E_MIN_ENUM_VAL_V02;
  }
  else{
    return max_iid;
  }
}

/**
 * Find an existing client connection, else create a new one.
 *
 * @param[i] client Client who is requesting the connection
 * @param[i] sns_svc_num Sensor service number requested
 * @param[o] client_conn_out Created or found client connection object
 *
 * @return
 * SENSOR1_SUCCESS
 * SENSOR1_ENOMEM - No memory available for this operation
 * SENSOR1_EFAILED - An error has occurred which blocks successful completion
 * SENSOR1_EWOULDBLOCK - No instances of the requested service are presently
 *                       available from QMI.
 */
static sensor1_error_e
client_get_conn( sensor1_client *client, uint32_t sns_svc_num,
    sensor1_client_conn **client_conn_out )
{
  sensor1_client_conn *client_conn = client->client_connections;
  sensor1_error_e rv = SENSOR1_SUCCESS;

  while( NULL != client_conn ) {
    if( sns_svc_num == client_conn->sns_svc_num ) {
      break;
    }
    client_conn = client_conn->next;
  }

  if( NULL == client_conn ) {
    qmi_idl_service_object_type svc_obj;

    client_conn = SNS_OS_MALLOC( 0, sizeof(sensor1_client_conn) );
    if( NULL == client_conn ) {
      LOG_ERROR("%s: Unable to allocate client connection", __func__);
      rv = SENSOR1_ENOMEM;
    } else {
      client_conn->sns_svc_num = sns_svc_num;
      client_conn->client = client;
      client_conn->handle = NULL;

      svc_obj = get_qmi_svc_obj( sns_svc_num );
      if( NULL == svc_obj ) {
        LOG_ERROR("%s: Requested service is invalid or disallowed %i",
            __func__, sns_svc_num);
        rv = SENSOR1_EBAD_SVC_ID;
      } else {
        qmi_client_error_type qmi_err;
        qmi_service_info service_info;
        qmi_service_info svc_info_array[ 5 ];
        uint32_t num_services, num_entries = 5;

        qmi_err = qmi_client_get_service_list( svc_obj, svc_info_array,
            &num_entries, &num_services );
        if( QMI_NO_ERR != qmi_err ) {
          LOG_ERROR("%s: qmi_client_get_service_list error %i", __func__, qmi_err);
          rv = SENSOR1_EFAILED;
        } else if( 0 == num_entries ) {
          LOG_WARN("%s: Service %i has no available instances", __func__, sns_svc_num);
          rv = SENSOR1_EWOULDBLOCK;
        } else {
          qmi_service_info svc_info;
          qmi_service_instance instance_id =
            choose_svc_instance( svc_info_array, num_entries, sns_svc_num );

          qmi_err = qmi_client_get_service_instance( svc_obj, instance_id, &svc_info );
          if( QMI_NO_ERR != qmi_err ) {
            LOG_ERROR("%s: qmi_client_get_service_instance error %i", __func__, qmi_err);
            rv = SENSOR1_EFAILED;
          } else {
            qmi_cci_os_signal_type os_params;
            qmi_err = qmi_client_init( &svc_info, svc_obj, ind_msg_cb,
                client_conn, &os_params, &client_conn->handle );
            if( qmi_err != QMI_IDL_LIB_NO_ERR ) {
              LOG_ERROR("%s: qmi_client_init error %i", __func__, qmi_err);
              rv = SENSOR1_EFAILED;
            } else {
              qmi_err = qmi_client_register_error_cb( client_conn->handle, error_cb, client_conn );
              if( QMI_NO_ERR != qmi_err ) {
                LOG_ERROR("%s: qmi_client_register_error_cb error %i", __func__, qmi_err);
                rv = SENSOR1_EFAILED;
              } else {
                client_conn->next = client->client_connections;
                client->client_connections = client_conn;
              }
            }
          }
        }
      }
    }
  }

  if( SENSOR1_SUCCESS != rv ) {
    if( NULL != client_conn ) {
      if( NULL != client_conn->handle ) {
        qmi_client_release( client_conn->handle );
      }
      SNS_OS_FREE( client_conn );
      client_conn = NULL;
    }
  }

  *client_conn_out = client_conn;
  return rv;
}

/**
 * Release all QMI connections associated with this client, and
 * perform all necessary client cleanup.
 *
 * @param[i] client Client to release
 */
static void
client_release( sensor1_client *client )
{
  sensor1_client_conn *client_conn;
  uint8_t err;

  LOG_DEBUG("%s: Destroying client %p", __func__, (void*)client);

  client_conn = client->client_connections;
  while( NULL != client_conn ) {
    qmi_client_error_type qmi_err;
    sensor1_client_conn *next = client_conn->next;

    LOG_VERBOSE("%s: Releasing client connection %p", __func__, (void*)client_conn);
    qmi_err = qmi_client_release( client_conn->handle );
    if( QMI_NO_ERR != qmi_err ) {
      LOG_WARN("%s: qmi_client_release error %i", __func__, qmi_err);
    }

    SNS_OS_FREE( client_conn );
    client_conn = next;
  }

  while( NULL != client->messages ) {
    sensor1_msg *msg = client->messages;
    client->messages = msg->next;
    if( NULL != msg->msg ) {
      SNS_OS_FREE( msg->msg );
    }
    SNS_OS_FREE( msg );
  }

  sns_os_mutex_del( client->client_mutex, 0, &err );
  sns_os_sem_del( client->messages_sem );
  SNS_OS_FREE( client );
}

/**
 * Thread passes all received QMI messages to the client.
 *
 * In theory this thread is unnecessary, and the messages could be given to
 * the client within the QMI callback context.  However, poorly written sensor1
 * clients, including the HAL, may result in deadlock.
 *
 * @param[i] p_arg Client handle
 */
static void
client_thread( void *arg )
{
  uint8_t err = 0;
  sensor1_client *client = (sensor1_client*)arg;

  LOG_DEBUG("%s: Starting thread %i for client %p",
      __func__, gettid(), (void*)client);
  for( ;; ) {
    sensor1_msg *msg;

    sns_os_sem_wait( client->messages_sem, NULL, &err );
    sns_os_mutex_pend( client->client_mutex, 0, &err );

    msg = client->messages;
    if( NULL != msg ) {
      if( SENSOR1_MSG_TYPE_RESP_INT_ERR == msg->msg_type ) {
        sns_os_mutex_post( client->client_mutex );
        break;
      }

      client->messages = msg->next;
      sns_os_mutex_post( client->client_mutex );

      client->data_cbf( client->cb_data, &msg->header,
          msg->msg_type, msg->msg );

      SNS_OS_FREE( msg );
    } else {
      sns_os_mutex_post( client->client_mutex );
    }
  }
  client_release( client );
}

/**
 * For debugging purposes, print out the process ID, and all Linux groups
 * it belongs to.
 */
static void
print_group_info( void )
{
  if( sensor1_log_level >= LOG_LEVEL_VERBOSE ) {
    int i, size_out, size_in = 32;
    gid_t list[32];

    LOG_DEBUG("%s: PID %i found", __func__, getpid());
    LOG_DEBUG("%s: Group %i found", __func__, getgid());

    size_out = getgroups(size_in, list);
    for( i = 0; i < size_out; i++ ) {
      LOG_DEBUG("%s: Group %i found", __func__, list[ i ]);
    }
  }
}

/*============================================================================
  Externalized Function Definitions
  ============================================================================*/

void
sensor1_close_clients( void )
{
  uint8_t err;
  sensor1_client *client;
  sensor1_msg msg =
    { .msg_type = SENSOR1_MSG_TYPE_BROKEN_PIPE,
      .msg = NULL };

  sns_os_mutex_pend( clients_mutex, 0, &err );
  client = clients;
  while( NULL != client ) {
    client_add_msg( client, &msg, true );
    client = client->next;
  }
  sns_os_mutex_post( clients_mutex );
}

sensor1_error_e
sensor1_open( sensor1_handle_s **hndl, sensor1_notify_data_cb_t data_cbf,
    intptr_t cb_data )
{
  uint8_t err = 0;
  sensor1_error_e rv = SENSOR1_EFAILED;
  sensor1_client *client = NULL;

  LOG_VERBOSE("%s", __func__);
  *hndl = NULL;
  sensor1_init();
  print_group_info();

  if( !wait_for_service( SNS_SMGR_SVC_ID_V01, SMGR_TIMEOUT_MS ) ) {
    sensor1_waiting_client *client = SNS_OS_MALLOC( 0, sizeof(sensor1_waiting_client) );
    if( NULL == client ) {
       LOG_ERROR("%s: memory allocation failed", __func__);
       return SENSOR1_ENOMEM;
    }

    client->cb_data = cb_data;
    client->sns_svc_num = SNS_SMGR_SVC_ID_V01;
    client->notify_cb = data_cbf;
    client->writable_cb = NULL;
    client->timeout = 0;

    LOG_VERBOSE("%s:creating the waiting thread", __func__);

    err = sns_os_task_create( waiting_thread, client, NULL, 0 );
    if( 0 != err ) {
      LOG_ERROR("%s: Error spawning WOULDBLOCK thread %i", __func__, err);
    }
    rv = SENSOR1_EWOULDBLOCK;
  } else {
    client = SNS_OS_MALLOC( 0, sizeof(sensor1_client) );
    if( NULL == client ) {
      rv = SENSOR1_ENOMEM;
    } else {
      client->data_cbf = data_cbf;
      client->cb_data = cb_data;

      client->messages = NULL;
      client->client_connections = NULL;
      client->client_mutex = NULL;
      client->messages_sem = NULL;

      client->client_mutex = sns_os_mutex_create( 0, &err );
      if( 0 != err ) {
        LOG_ERROR("%s: Cannot create mutex %i", __func__, err);
      } else {
        client->messages_sem = sns_os_sem_create( 0, &err );
        if( 0 != err ){
          LOG_ERROR("%s: Cannot create semaphore %i", __func__, err);
        } else {
          err = sns_os_task_create( client_thread, client, NULL, 0 );
          if( 0 != err ) {
            LOG_ERROR("%s: Error spawning client thread %i", __func__, err);
          } else {
            *hndl = (sensor1_handle_s*)client;

            sns_os_mutex_pend( clients_mutex, 0, &err );
            client->next = clients;
            clients = client;
            sns_os_mutex_post( clients_mutex );

            rv = SENSOR1_SUCCESS;
          }
        }
      }
    }
  }

  log_api_call( client, SNS_LOG_SENSOR1_API_OPEN, rv, NULL, NULL );

  if( SENSOR1_SUCCESS != rv && NULL != client ) {
    if( NULL != client->client_mutex ) {
      sns_os_mutex_del( client->client_mutex, 0, &err );
    }
    if( NULL != client->messages_sem ) {
      sns_os_sem_del( client->messages_sem );
    }
    if(SENSOR1_EWOULDBLOCK != rv)
        SNS_OS_FREE( client );
  }

  LOG_DEBUG("%s: Client %p created; rv %i", __func__, (void*)*hndl, rv);
  return rv;
}

sensor1_error_e
sensor1_close( sensor1_handle_s *hndl )
{
  uint8_t err = 0;

  if(NULL == hndl || (!validate_hndl( (sensor1_client*)hndl ))) {
    LOG_ERROR("%s: invalid hndl", __func__);
    return SENSOR1_EFAILED;
  }

  sensor1_client *client = (sensor1_client*)hndl;
  sensor1_msg msg =
    { .msg_type = SENSOR1_MSG_TYPE_RESP_INT_ERR,
      .msg = NULL };

  LOG_DEBUG("%s: Closing client %p", __func__, (void*)client);
  log_api_call( client, SNS_LOG_SENSOR1_API_CLOSE, SENSOR1_SUCCESS, NULL, NULL );

  sns_os_mutex_pend( clients_mutex, 0, &err );
  if( client == clients ) {
    clients = client->next;
  } else {
    sensor1_client *temp = clients;
    while( NULL != temp ) {
      if( client == temp->next ) {
        temp->next = client->next;
        break;
      }
      temp = temp->next;
    }
  }
  sns_os_mutex_post( clients_mutex );
  client_add_msg( client, &msg, true );

  return SENSOR1_SUCCESS;
}

sensor1_error_e
sensor1_write( sensor1_handle_s *hndl, sensor1_msg_header_s *msg_hdr,
    void *msg_ptr )
{
  uint8_t err;
  sensor1_client *client = (sensor1_client*)hndl;
  sensor1_client_conn *client_conn;
  sensor1_error_e rv = SENSOR1_SUCCESS;

  LOG_VERBOSE("%s: %p", __func__, (void*)hndl);

  if(NULL == hndl || (!validate_hndl( client ))) {
    LOG_ERROR("%s: invalid hndl", __func__);
    return SENSOR1_EFAILED;
  }

  sns_os_mutex_pend( log_mutex, 0, &err );
  rv = client_get_conn( client, msg_hdr->service_number, &client_conn );
  if( SENSOR1_EWOULDBLOCK == rv ) {
    LOG_WARN("%s: Service presently unavailable", __func__);
  } else if( NULL == client_conn || SENSOR1_SUCCESS != rv ) {
    LOG_ERROR("%s: Unable to create client connection", __func__);
    rv = SENSOR1_EBAD_SVC_ID;
  } else {
    uint32_t resp_len;
    qmi_client_error_type qmi_err;

    qmi_err = qmi_idl_get_message_c_struct_len( get_qmi_svc_obj( client_conn->sns_svc_num ),
        QMI_IDL_RESPONSE, msg_hdr->msg_id, &resp_len );
    if( QMI_IDL_LIB_NO_ERR != qmi_err ) {
      LOG_ERROR("%s: qmi_idl_get_message_c_struct_len error %d", __func__, qmi_err);
      rv = SENSOR1_EFAILED;
    } else {
      sensor1_resp *resp = SNS_OS_MALLOC( 0, sizeof(sensor1_resp) );

      if( NULL == resp ) {
        LOG_ERROR("%s: Unable to allocate sensor1_resp", __func__);
        rv = SENSOR1_ENOMEM;
      } else {
        resp->client = client;
        resp->header = *msg_hdr;
        resp->msg_ptr = SNS_OS_MALLOC( 0, resp_len );
        if( NULL == resp->msg_ptr && 0 != resp_len ) {
          LOG_ERROR("%s: Unable to allocate response message", __func__);
          SNS_OS_FREE( resp );
          rv = SENSOR1_ENOMEM;
        } else {
          LOG_DEBUG("%s: Sending request to %i on client %p",
              __func__, client_conn->sns_svc_num, (void*)client);
          qmi_err = qmi_client_send_msg_async( client_conn->handle,
              msg_hdr->msg_id, msg_ptr, msg_hdr->msg_size, resp->msg_ptr,
              resp_len, resp_msg_cb, resp, &resp->txn_handle );

          if( QMI_NO_ERR != qmi_err ) {
            LOG_ERROR("%s: qmi_client_send_msg_async error %d", __func__, qmi_err);
            SNS_OS_FREE( resp->msg_ptr );
            SNS_OS_FREE( resp );
            rv = SENSOR1_EFAILED;
          }
        }
      }
    }
  }

  log_api_call( client, SNS_LOG_SENSOR1_API_WRITE, rv, msg_ptr, msg_hdr );
  if( SENSOR1_SUCCESS == rv ) {
    sensor1_free_msg_buf( hndl, msg_ptr );
  }

  sns_os_mutex_post( log_mutex );

  return rv;
}

sensor1_error_e
sensor1_writable( sensor1_handle_s *hndl, sensor1_write_cb_t cbf,
    intptr_t cb_data, uint32_t service_number )
{
  UNREFERENCED_PARAMETER(hndl);
  uint8_t err;
  sensor1_waiting_client *client;

  LOG_VERBOSE("%s: %p", __func__, (void*)hndl);

  if(NULL == hndl || (!validate_hndl( (sensor1_client*)hndl ))) {
    LOG_ERROR("%s: invalid hndl", __func__);
    return SENSOR1_EFAILED;
  }

  client = SNS_OS_MALLOC( 0, sizeof(sensor1_waiting_client) );
  if( NULL != client ) {
    client->cb_data = cb_data;
    client->sns_svc_num = service_number;
    client->notify_cb = NULL;
    client->writable_cb = cbf;
    client->timeout = 0;
    err = sns_os_task_create( waiting_thread, client, NULL, 0 );
    if( 0 != err ) {
    LOG_ERROR("%s: Error spawning WOULDBLOCK thread %i", __func__, err);
    return SENSOR1_EFAILED;
    }
    // PEND: If a duplicate, we want to overwrite the existing writable request
    // PEND: If the client calls sensor1_close, we need to cancel these waiting threads
    return SENSOR1_SUCCESS;
  } else {
      LOG_ERROR("%s: Error spawning sensor1_writable thread no memory", __func__);
      return SENSOR1_ENOMEM;
  }
}

sensor1_error_e
sensor1_alloc_msg_buf(sensor1_handle_s *hndl, uint16_t size, void **buffer )
{
  UNREFERENCED_PARAMETER(hndl);
  *buffer = SNS_OS_MALLOC( 0, size );

  if( NULL != *buffer ) {
    SNS_OS_MEMZERO( *buffer, size );
  } else {
    return SENSOR1_ENOMEM;
  }

  return SENSOR1_SUCCESS;
}

sensor1_error_e
sensor1_free_msg_buf(sensor1_handle_s *hndl, void *msg_buf )
{
  UNREFERENCED_PARAMETER(hndl);

  SNS_OS_FREE( msg_buf );
  return SENSOR1_SUCCESS;
}

void
sensor1_init_once( void )
{
  uint8_t err = 0;

  LOG_DEBUG("%s", __func__);

  clients = NULL;
  clients_mutex = sns_os_mutex_create( 0, &err );
  if( 0 != err ) {
    LOG_ERROR("%s: Cannot create clients mutex %i", __func__, err);
    SNS_ASSERT(false);
  }

  log_mutex = sns_os_mutex_create( 0, &err );
  if( 0 != err ) {
    LOG_ERROR("%s: Cannot create log mutex %i", __func__, err);
    SNS_ASSERT(false);
  }

  sensor1_osa_init_once();
  sensor1_log_init();
}

sensor1_error_e
sensor1_init( void )
{
  uint8_t err;
  static OS_THREAD thread;

  err = sns_os_task_once( &thread, sensor1_init_once );
  LOG_DEBUG("%s: err %i", __func__, err);
  return SENSOR1_SUCCESS;
}
