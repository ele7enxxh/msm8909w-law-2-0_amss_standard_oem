#ifndef _LIBSENSOR1_H_
#define _LIBSENSOR1_H_
/*=============================================================================
  @file libsensor1.h

  @brief Defines the types used in the sensor1 remoting library for sensor
  service communication.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "qmi_idl_lib.h"
#include "qmi_client.h"
#include "sensor1.h"
#include "sns_osa.h"
#include "sns_em.h"

/*=============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

#ifndef UNREFERENCED_PARAMETER
# define UNREFERENCED_PARAMETER(x) (void)x;
#endif /* UNREFERENCED_PARAMETER */

/*============================================================================
  Type Declarations
  ============================================================================*/

/**
 * Defines which sensor1 API is logged. For use in sns_log_sensor1_request.
 */
typedef enum {
  SNS_LOG_SENSOR1_API_OPEN  = 1,
  SNS_LOG_SENSOR1_API_WRITE = 2,
  SNS_LOG_SENSOR1_API_CLOSE = 3
} sns_log_sensor1_api;

typedef enum {
  LOG_LEVEL_ALL,
  LOG_LEVEL_VERBOSE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_DISABLED
} log_level;

/* Represents a single QMI connection by a single sensor1 client.
 * A client may have at most one connection per QMI service.  But each
 * sensor1 client will have its own connections to QMI services.
 */
typedef struct sensor1_client_conn
{
  struct sensor1_client_conn *next;

  /* QMI Client handle */
  qmi_client_type handle;
  /* Sensor1 service number associated with this connection */
  uint32_t sns_svc_num;
  /* The client which owns this connection */
  struct sensor1_client *client;
} sensor1_client_conn;

/**
 * A resonse or indication message intended for a sensor1 client.
 */
typedef struct sensor1_msg
{
  struct sensor1_msg *next;

  sensor1_msg_type_e msg_type;
  /* Message header to be delivered to the client */
  sensor1_msg_header_s header;
  /* Message payload; size specified in header */
  void *msg;
} sensor1_msg;

/**
 * Each client will have it's own thread, which will monitor for QMI events,
 * and pass QMI events synchronously back to the client.
 *
 * If a client calls close, the release_async function will be called on the
 * associated QMI connection.  The thread will exit once the release_cb
 * is received.
 */
typedef struct sensor1_client
{
  struct sensor1_client *next;

  /* Messages received from QMI, but not yet delivered to the client */
  sensor1_msg *messages;
  /** Counting semaphore for the length of the mr_queue */
  OS_EVENT *messages_sem;

  /* List of connections associated with this client */
  sensor1_client_conn *client_connections;

  /* Callback function registed by sensor1 client */
  sensor1_notify_data_cb_t data_cbf;
  /* Callback data registered by sensor1 client */
  intptr_t cb_data;
  /* sensor1 clients expect close to be synchronous, but clients may be
   * poorly written, and may have called close from within a callback.
   * To guard against this, we safely check whether the callback function is
   * NULL before using it (saved in a temporary variable, to avoid race conditions
   * and deadlock).
   *
   * This mutex protects the messages list. */
  OS_EVENT *client_mutex;
} sensor1_client;

/*============================================================================
 * Extern Data Declarations
 ============================================================================*/
extern log_level sensor1_log_level;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Get the QMI service object corresponding to a Sensor1 service number
 *
 * @param[i] svc_num Sensor1 service number, as found in sns_common_v01.h
 *
 * @return QMI service object, as acquired from *_get_service_object_v01()
 */
qmi_idl_service_object_type
get_qmi_svc_obj( uint32_t svc_num );

/**
 * Logs a call to sensor1_open, sensor1_write, or sensor1_close
 *
 * @param[i] client Client handle associated with this request
 * @param[i] sensor1_fn As defined in sns_log_sensor1_api_e. Open/Write/Close
 * @param[i] sensor1_rv Return value from sensor1 API call
 * @param[i] msg Decoded sensor1 message from client
 * @param[i] msg_hdr Sensor1 header; or NULL for open/close
 */
void
log_api_call( sensor1_client const *client, sns_log_sensor1_api sensor1_fn,
    sensor1_error_e sensor1_rv, void const *msg,
    sensor1_msg_header_s const *msg_hdr );

/**
 * @brief Logs a response message sent to a client.
 *
 * @param[i] Client Client handle
 * @param[i] msg_hdr Sensor1 message header
 * @param[i] msg QMI decoded message
 */
void
log_resp( sensor1_client const *client, sensor1_msg_header_s const *msg_hdr,
    void const *msg );

/**
 * @brief Logs an indication message sent to a client.
 *
 * @param[i] Client Client handle
 * @param[i] msg_hdr Sensor1 message header
 * @param[i] msg QMI encoded message
 * @param[i] msg_len Length of msg
 */
void
log_ind( sensor1_client const *client, sensor1_msg_header_s const *msg_hdr,
    void const *msg, uint32_t msg_len );

/**
 * Initialize the logging module.  Must be called once at init.
 */
void
sensor1_log_init( void );

/**
 * Check and return whether sensor1 is presently enabled.
 */
bool
sensor1_enabled_check( void );

/**
 * Wait until sensors_enabled is true.
 */
void
sensor1_enabled_wait( void );

/**
 * Close all active clients */
void
sensor1_close_clients( void );

#endif /* _LIBSENSOR1_H_ */
