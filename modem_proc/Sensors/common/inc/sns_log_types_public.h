#ifndef SNS_LOG_TYPES_PUBLIC_H
#define SNS_LOG_TYPES_PUBLIC_H

/*============================================================================

@file
  sns_log_types_public.h

@brief
  Contains public type definitions for sensors logs.

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
/*===========================================================================

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/inc/sns_log_types_public.h#1 $

when        who    what, where, why
(mm/dd/yyyy)
--------    ---    ----------------------------------------------------------
12/01/2014   MW    Rev up 0x14c1 version 
02/28/2014   MW    Initial version
===========================================================================*/

/*=====================================================================
                 INCLUDE FILES
=======================================================================*/

#include <stddef.h>
#include <stdint.h>

#include "log_codes.h"
#include "log.h"

/*=======================================================================
                  INTERNAL DEFINITIONS AND TYPES
========================================================================*/
#ifdef SENSORS_DD_DEV_FLAG

typedef enum {
   SNS_LOG_SENSOR1_REQUEST_PUBLIC,
   SNS_LOG_SENSOR1_RESPONSE_PUBLIC,
   SNS_LOG_SENSOR1_INDICATION_PUBLIC,
   SNS_LOG_RESERVED_1,
   SNS_LOG_RESERVED_2,
   SNS_LOG_RESERVED_3,
   SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC,
   SNS_LOG_NUM_IDS_MAX = 255,
   // add log IDs after this
} sns_log_id_public_e;

typedef enum {
  SNS_SUCCESS   = 0,
  SNS_ERR_BUFFER,       /* Value in the buffer is wrong */
  SNS_ERR_NOMEM,        /* Insufficient memory to process the request */
  SNS_ERR_INVALID_HNDL, /* Invalid handle */
  SNS_ERR_UNKNOWN,      /* Unknown error */
  SNS_ERR_FAILED,       /* Failure in general */
  SNS_ERR_NOTALLOWED,   /* Not allowed operation */
  SNS_ERR_BAD_PARM,     /* One or more parameters have invalid value */
  SNS_ERR_BAD_PTR,      /* Invalid pointer. This may be due to NULL values */
  SNS_ERR_BAD_MSG_ID,   /* The message ID is not supported */
  SNS_ERR_BAD_MSG_SIZE, /* The message size of the message ID does not match */
  SNS_ERR_WOULDBLOCK,   /* Indicating the data is tentatively unavailable */
  SNS_ERR_NOTSUPPORTED  /* Indicating that API is not supported */
} sns_err_code_e;

#else // SENSORS_DD_DEV_FLAG

#include "sns_common.h"

#endif // SENSORS_DD_DEV_FLAG

#define SNS_LOG_MAX_DATA_TYPES_PER_SENSOR 20

/** Sensors log packet mask type */
typedef uint64_t sns_log_mask_t;

/**
   Enum defining which sensor1 API is being logged.
   For use in sns_log_sensor1_request_s.
 */
typedef enum {
  SNS_LOG_SENSOR1_API_OPEN  = 1,
  SNS_LOG_SENSOR1_API_WRITE = 2,
  SNS_LOG_SENSOR1_API_CLOSE = 3
} sns_log_sensor1_api_e;

#if defined(SNS_PCSIM) || defined(_WIN32)
#pragma pack(push,1)
#endif

/**
   sns_log_sensor1_request_s
   This type is used to log function calls to sensor1_open, sensor1_write,
   and sensor1_close.

   The usage of values below may change depending on the function type.

   It is used for the following log types:
   LOG_SNS_SENSOR1_REQUEST_C
 */

/** Version of the sensor1 logging structures */
#define SNS_LOG_STRUCT_VERSION 2

typedef PACK(struct) {
  log_hdr_type   log_type;          /**< Type, as defined in log_codes.h */
  uint8_t        version;           /**< Set to SNS_LOG_STRUCT_VERSION */
  uint8_t        logging_processor; /**< 1 == Modem; 2 == Apps; 3 == DSPS */
  uint64_t       timestamp;         /**< Timestamp the log was created */
  uint8_t        sensor1_fn;        /**< As defined in sns_log_sensor1_api_e */
  int32_t        sensor1_rv;        /**< Return value from sensor1 API call */
  uint8_t        ext_clnt_id;      /**< External client ID assigned by ACM */
  uint64_t       cb_data;           /**< Callback data assigned to this client */
  uint64_t       context_handle;    /**< Unique handle identifying an external
                                       client.
                                       Assigned when the client calls
                                       sensor1_open */
  uint8_t        svc_num;           /**< QMI Service number
                                       Only valid when sensor1_fn == 2
                                       (sensor1_write) */
  uint16_t       msg_id;            /**< Message ID, as defined by the service */
  uint8_t        msg_type;          /**< Message Type: Response
                                       Only valid when sensor1_fn == 2
                                       (sensor1_write) */
  uint8_t        txn_id;            /**< Transaction ID chosen by the client */
  uint16_t       request_size;      /**< Size of the request field.
                                       Only valid when sensor1_fn == 2
                                       (sensor1_write).  */
  uint8_t        request[1];        /**< The QMI-encoded message, of length
                                       request_size.
                                       Only valid when sensor1_fn == 2
                                       (sensor1_write) */
} sns_log_sensor1_request_s;


/**
   sns_log_sensor1_resp_ind_s
   This type is used to log response and indication packets sent to clients via
   the sensor1 API.

   It is used for the following log types:
   LOG_SNS_SENSOR1_RESPONSE_C
   LOG_SNS_SENSOR1_INDICATION_C
 */
typedef PACK(struct) {
  log_hdr_type   log_type;          /**< Type, as defined in log_codes.h */
  uint8_t        version;           /**< Set to SNS_LOG_STRUCT_VERSION */
  uint8_t        logging_processor; /**< 1 == Modem; 2 == Apps; 3 == DSPS */
  uint64_t       timestamp;         /**< Timestamp the log was created */
  uint64_t       cb_data;           /**< Callback data assigned to this client */
  uint64_t       context_handle;    /**< Unique handle identifying an external
                                       client.
                                       Assigned when the client calls
                                       sensor1_open */
  uint8_t        ext_clnt_id;       /**< External client ID assigned by ACM */
  uint8_t        svc_num;           /**< QMI Service number */
  uint16_t       msg_id;            /**< Message ID, as defined by the service */
  uint8_t        msg_type;          /**< Message Type: Response or Indication */
  uint8_t        txn_id;            /**< Transaction ID chosen by the client */
  uint16_t       resp_ind_size;     /**< Size of the resp_ind field. */
  uint8_t        resp_ind[1];       /**< The QMI-encoded response or indication,
                                       of length resp_ind_size. */
} sns_log_sensor1_resp_ind_s;


/**
   Enum defining which bus type is being used by the sensor.
   For use in sns_log_sensor_data_pkt_s.
 */
typedef enum {
  SNS_LOG_SENSOR_BUS_TYPE_NONE  = 0,
  SNS_LOG_SENSOR_BUS_TYPE_I2C = 1,
  SNS_LOG_SENSOR_BUS_TYPE_SPI = 2
} sns_log_sensor_bus_type_e;

/**
   sns_log_sensor_data_pkt_s
   This type is used to log sensor data as reported to the sensors manager.
   It is used for the following log types:
   LOG_CONVERTED_SENSOR_DATA_C
 */
#define SNS_LOG_SENSOR_DATA_PKT_VERSION 5
typedef PACK(struct) {
  log_hdr_type log_type;         /**< Type, as defined in log_codes.h */
  uint8_t      version;          /**< version of the log packet */
  uint64_t     timestamp;        /**< time when the sensor data was sampled */
  uint32_t     sensor_id;        /**< type of sensor being logged (DDF sensor type) */
  uint8_t      vendor_id;        /**< vendor for the sensor device */
  uint8_t      bus_type;         /**< As defined in sns_log_sensor_bus_type_e */
  uint16_t     bus_inst;         /**< If bus_type is I2C, this is the I2C bus instance */
  uint32_t     dev_addr;         /**< If bus_type is I2C, this is the I2C address */
  uint8_t      num_data_types;   /**< sample dimension being logged */
  int32_t      data[SNS_LOG_MAX_DATA_TYPES_PER_SENSOR]; /**< sensor data */
  uint64_t     end_timestamp;    /**< end  time when the last sensor data was
                                      sampled in this FIFO batch */
  uint16_t     num_samples;      /**< Number of samples in this log, if num_sample is 0
                                     ( to be backward compatible with older version )or 1,
                                      put sample in data field; if num_sample is 1, put
                                      sample in both data and samples */
  int32_t      samples[1];       /**< Size will depend on num_samples, fifo
                                      samples will be cascaded here, the axis info
                                      is represented by num_data_types
                                      bytes=(num_samples*num_data_types*4) */
} sns_log_sensor_data_pkt_s;

#endif /* SNS_LOG_TYPES_PUBLIC_H */
