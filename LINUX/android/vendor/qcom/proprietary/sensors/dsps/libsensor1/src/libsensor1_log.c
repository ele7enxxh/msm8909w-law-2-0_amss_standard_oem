/*=============================================================================
  @file libsensor1_log.c

  @brief Log incoming and outgoing sensor1 requests, responses, and
  indications.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "log.h"
#include <unistd.h>
#include "diag_lsm.h"
#include "libsensor1_osa.h"
#include "libsensor1.h"
#include "sns_memmgr.h"

/*============================================================================
  Type Declarations
  ============================================================================*/

/**
 * Used to log function calls to sensor1_open, sensor1_write, and sensor1_close.
 *
 * It is used for the following log types: LOG_SNS_SENSOR1_REQUEST_C
 */
#define SNS_LOG_STRUCT_VERSION 2
typedef PACK(struct) {
  log_hdr_type   log_type;          /**< Type, as defined in log_codes.h */
  uint8_t        version;           /**< Set to SNS_LOG_STRUCT_VERSION */
  uint8_t        logging_processor; /**< 1 == Modem; 2 == Apps; 3 == DSPS */
  uint64_t       timestamp;         /**< Timestamp the log was created */
  uint8_t        sensor1_fn;        /**< As defined in sns_log_sensor1_api_e */
  int32_t        sensor1_rv;        /**< Return value from sensor1 API call */
  uint8_t        ext_clnt_id;       /**< External client ID assigned by ACM */
  uint64_t       cb_data;           /**< Callback data assigned to this client */
  uint64_t       context_handle;    /**< Unique handle identifying an external
                                         client. Assigned when the client calls
                                         sensor1_open */
  uint8_t        svc_num;           /**< QMI Service number Only valid when
                                         sensor1_fn == 2 (sensor1_write) */
  uint16_t       msg_id;            /**< Message ID, as defined by the service */
  uint8_t        msg_type;          /**< Message Type: Response only valid when
                                         sensor1_fn == 2 (sensor1_write) */
  uint8_t        txn_id;            /**< Transaction ID chosen by the client */
  uint16_t       request_size;      /**< Size of the request field. Only valid
                                         when sensor1_fn == 2 (sensor1_write).  */
  uint8_t        request[1];        /**< The QMI-encoded message, of length
                                         request_size. Only valid when sensor1_fn == 2
                                         (sensor1_write) */
} sns_log_sensor1_request;


/**
 * Used to log response and indication messages sent to clients via the sensor1 API.
 *
 * It is used for the following log types:
 * LOG_SNS_SENSOR1_RESPONSE_C
 * LOG_SNS_SENSOR1_INDICATION_C
 */
typedef PACK(struct) {
  log_hdr_type   log_type;          /**< Type, as defined in log_codes.h */
  uint8_t        version;           /**< Set to SNS_LOG_STRUCT_VERSION */
  uint8_t        logging_processor; /**< 1 == Modem; 2 == Apps; 3 == DSPS */
  uint64_t       timestamp;         /**< Timestamp the log was created */
  uint64_t       cb_data;           /**< Callback data assigned to this client */
  uint64_t       context_handle;    /**< Unique handle identifying an external
                                         client. Assigned when the client calls
                                         sensor1_open */
  uint8_t        ext_clnt_id;       /**< External client ID assigned by ACM */
  uint8_t        svc_num;           /**< QMI Service number */
  uint16_t       msg_id;            /**< Message ID, as defined by the service */
  uint8_t        msg_type;          /**< Message Type: Response or Indication */
  uint8_t        txn_id;            /**< Transaction ID chosen by the client */
  uint16_t       resp_ind_size;     /**< Size of the resp_ind field. */
  uint8_t        resp_ind[1];       /**< The QMI-encoded response or indication,
                                       of length resp_ind_size. */
} sns_log_sensor1_resp_ind;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

void
log_api_call( sensor1_client const *client, sns_log_sensor1_api sensor1_fn,
    sensor1_error_e sensor1_rv, void const *msg,
    sensor1_msg_header_s const *msg_hdr )
{
  uint32_t logpkt_size = sizeof(sns_log_sensor1_request);
  sns_log_sensor1_request *log_pkt = NULL;

  if( SNS_LOG_SENSOR1_API_WRITE == sensor1_fn ) {
    uint32_t message_len;
    int32_t qmi_err;
    qmi_idl_service_object_type service = get_qmi_svc_obj( msg_hdr->service_number );

    if( NULL != service ) {
      qmi_err = qmi_idl_get_max_message_len( service,
        QMI_IDL_REQUEST, msg_hdr->msg_id, &message_len );
      if( QMI_IDL_LIB_NO_ERR == qmi_err ) {
        uint32_t request_size;
        logpkt_size = message_len + sizeof(sns_log_sensor1_request) - 1;

        log_pkt = log_alloc( LOG_SNS_SENSOR1_REQUEST_C, logpkt_size );
        if( NULL != log_pkt ) {
          if( 0 != msg_hdr->msg_size ) {
            qmi_err = qmi_idl_message_encode( service, QMI_IDL_REQUEST, msg_hdr->msg_id,
                msg, msg_hdr->msg_size, (void*)log_pkt->request, message_len,
                &request_size );

            if( QMI_IDL_LIB_NO_ERR != qmi_err ) {
              request_size = 0;
              LOG_ERROR("%s: qmi_idl_message_encode error %i", __func__, qmi_err);
            }
          } else {
            request_size = 0;
          }
          log_pkt->request_size = (uint16_t)request_size;
          logpkt_size = request_size + sizeof(sns_log_sensor1_request) - 1;
        }
      } else {
        LOG_ERROR("%s: qmi_idl_get_max_message_len error %i", __func__, qmi_err);
      }
    } else {
      LOG_ERROR("%s: Service object not found for %i", __func__,
          msg_hdr->service_number);
    }
  } else {
    log_pkt = log_alloc( LOG_SNS_SENSOR1_REQUEST_C, logpkt_size );
  }

  if( NULL != log_pkt ) {
    sns_em_get_timestamp64( &(log_pkt->timestamp) );

    log_pkt->version = SNS_LOG_STRUCT_VERSION;
    log_pkt->logging_processor = 2;
    log_pkt->sensor1_fn = (uint8_t)sensor1_fn;
    log_pkt->sensor1_rv = sensor1_rv;
    log_pkt->ext_clnt_id = (intptr_t)client;
    log_pkt->context_handle = (uintptr_t)client;
    log_pkt->msg_type = SENSOR1_MSG_TYPE_REQ;

    log_pkt->cb_data = ( NULL != client )
        ? (uint64_t)(client->cb_data) : 0;

    if( NULL == msg_hdr ) {
      log_pkt->svc_num = 0;
      log_pkt->msg_id = 0;
      log_pkt->txn_id = 0;

      log_pkt->request_size = 0;
      log_pkt->request[0] = 0;
    } else {
      log_pkt->svc_num = (uint8_t)msg_hdr->service_number;
      log_pkt->msg_id = (uint16_t)msg_hdr->msg_id;
      log_pkt->txn_id = msg_hdr->txn_id;
    }

    log_shorten( log_pkt, logpkt_size );
    log_commit( log_pkt );
  } else {
    LOG_VERBOSE("%s: Unable to allocate log packet", __func__);
  }
}

void
log_resp( sensor1_client const *client, sensor1_msg_header_s const *msg_hdr,
    void const *msg )
{
  uint32_t logpkt_size;
  sns_log_sensor1_resp_ind *log_pkt = NULL;
  uint32_t message_len;
  int32_t qmi_err;
  qmi_idl_service_object_type service = get_qmi_svc_obj( msg_hdr->service_number );

  qmi_err = qmi_idl_get_max_message_len( service,
    QMI_IDL_RESPONSE, msg_hdr->msg_id, &message_len );
  if( QMI_IDL_LIB_NO_ERR == qmi_err ) {
    uint32_t resp_size;

    logpkt_size = message_len + sizeof(sns_log_sensor1_request) - 1;
    log_pkt = log_alloc( LOG_SNS_SENSOR1_RESPONSE_C, logpkt_size );
    if( NULL != log_pkt ) {
      qmi_idl_message_encode( service, QMI_IDL_RESPONSE, msg_hdr->msg_id,
          msg, msg_hdr->msg_size, (void*)log_pkt->resp_ind, message_len,
          &resp_size );
      log_pkt->resp_ind_size = (uint16_t)resp_size;
      logpkt_size = resp_size + sizeof(sns_log_sensor1_request) - 1;
    } else {
      LOG_VERBOSE("%s: Unable to allocate log packet len %i", __func__, logpkt_size);
    }
  } else {
    LOG_ERROR("%s: qmi_idl_get_max_message_len error %i", __func__, qmi_err);
  }

  if( NULL != log_pkt ) {
    sns_em_get_timestamp64( &(log_pkt->timestamp) );

    log_pkt->version = SNS_LOG_STRUCT_VERSION;
    log_pkt->logging_processor = 2;
    log_pkt->context_handle = (uintptr_t)client;
    log_pkt->msg_type = SENSOR1_MSG_TYPE_RESP;
    log_pkt->ext_clnt_id = (intptr_t)client;

    log_pkt->svc_num = (uint8_t)msg_hdr->service_number;
    log_pkt->msg_id = (uint16_t)msg_hdr->msg_id;
    log_pkt->txn_id = msg_hdr->txn_id;
    log_pkt->cb_data = (uint64_t)client->cb_data;

    log_shorten( log_pkt, logpkt_size );
    log_commit( log_pkt );
  }
}

void
log_ind( sensor1_client const *client, sensor1_msg_header_s const *msg_hdr,
    void const *msg, uint32_t msg_len )
{
  sns_log_sensor1_resp_ind *log_pkt = NULL;
  uint32_t logpkt_size = msg_len + sizeof(sns_log_sensor1_resp_ind) - 1;

  log_pkt = log_alloc( LOG_SNS_SENSOR1_INDICATION_C, logpkt_size );
  if( NULL != log_pkt ) {
    sns_em_get_timestamp64( &(log_pkt->timestamp) );

    log_pkt->version = SNS_LOG_STRUCT_VERSION;
    log_pkt->logging_processor = 2;
    log_pkt->context_handle = (uintptr_t)client;
    log_pkt->msg_type = SENSOR1_MSG_TYPE_IND;
    log_pkt->ext_clnt_id = (intptr_t)client;

    log_pkt->svc_num = (uint8_t)msg_hdr->service_number;
    log_pkt->msg_id = (uint16_t)msg_hdr->msg_id;
    log_pkt->txn_id = msg_hdr->txn_id;
    log_pkt->cb_data = (uint64_t)client->cb_data;

    log_pkt->resp_ind_size = msg_len;
    SNS_OS_MEMSCPY( log_pkt->resp_ind, msg_len, msg, msg_len );

    log_commit( log_pkt );
  } else {
    LOG_VERBOSE("%s: Unable to allocate log packet len %i", __func__, logpkt_size);
  }
}

void
sensor1_log_init( void )
{
  Diag_LSM_Init( NULL );
}
