#ifndef SNS_SAM_PROV_API_H
#define SNS_SAM_PROV_API_H

/*============================================================================
  @file sns_sam_prov_api.h

  @brief
  Defines the interface between the SAM Framework and the various providers
  of sensor data input to algorithms.

  @note
  File line length should generally be limited to <= 80 columns.

  @note
  Dynamically linked algorithms should be compiled with SNS_QDSS_SWE undefined.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "qmi_idl_lib.h"
#include "sns_usmr.h"
#include "sns_queue.h"
#include "sns_common_v01.h"
#include "sns_sam_algo_api.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/* Max number of algorithm instances that may share a single sensor stream */
#define SNS_SAM_MAX_SHARED_STREAM 5

/*============================================================================
  Type Declarations
  ===========================================================================*/

struct sns_sam_algo_inst;
struct sns_sam_dl_info;
struct sns_sam_sensor;

/*
 * SAM classifies sensors as streaming sensors and as event sensor. A streaming
 * sensor can be combined with an event sensor to create an event gated sensor.
 *
 * An event gated sensor is created by bitwise XOR'ing the SUID of a streaming
 * sensor and an event sensor.
 */
typedef enum {
  /* All physical sensors, eg: Accel, Gyro*/
  SNS_SAM_SENSOR_STREAM,
  /* Sensor events, eg: MD interrupt */
  SNS_SAM_SENSOR_EVENT,
  /* Event gated sensor stream. Eg: MD interrupt gated accel */
  SNS_SAM_SENSOR_GATED_STREAM
} sns_sam_gated_sensor_stream_type;

/**
 * Refers to a request for a specific sensor type.
 *
 * PEND: Each provider subclasses this structure, and adds provider-specific
 *       data.  For example, only SAM uses instanceID or deferredBatchReq.
 */
struct sns_sam_sensor_req
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* Algorithm instances using this sensor stream */
  struct sns_sam_algo_inst *algoInst[ SNS_SAM_MAX_SHARED_STREAM ];
  /* Sensor type associated with this request */
  struct sns_sam_sensor *sensor;
  /* Pointer to the sensor UID associated with this request */
  sns_sam_sensor_uid const *sensorUID;
  /* An enable request message for this sensor type */
  sns_sam_enable_req const *enableReq;
  /* SMR Client handle used to send/receive messages for this stream */
  smr_client_hndl clientHndl;
  /* Batch period; only applies to dependent SAM sensors */
  uint32_t batchPeriod[ SNS_SAM_MAX_SHARED_STREAM ];

  /* SAM instance ID; UINT16_MAX for unset values */
  uint16_t instanceID;
  /* Whether a batch request should be sent upon enable response */
  bool deferredBatchReq;
  /* If we actively waiting for a flush response message on this connection. */
  bool flushInProgress;

  bool isTemporary;
  /* Provider API associated with this sensor. */
  struct sns_sam_prov_api *provAPI;
};
typedef struct sns_sam_sensor_req sns_sam_sensor_req;

/**
 * General information for each sensor, whether it be a SAM algorithm or SMGR
 * physical sensor.  Information is populated and used by the SAM framework.
 * An instance of this object will be created for all known SAM algorithms,
 * regardless of service location.
 *
 * A global list of known sensors will be available for look-up.
 */
struct sns_sam_sensor
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* Sensor type; global unique IDs for all SAM algorithms and SMGR sensors.
   * This field is the definitive location for this ID; all other users,
   * including algorithms, must use its address. */
  sns_sam_sensor_uid *sensorUID;
  /* Which processor does this sensor come from (or unknown) */
  sns_proc_type_e_v01 sensorLocation;
  /* Sensor specific attributes provided by the algorithm or SMGR */
  sns_sam_algo_attr attributes[ SAM_ALGO_ATTR_CNT ];
  /* QMI service object corresponding to the service */
  qmi_idl_service_object_type serviceObj;
  /* Sensor request only used for SMR handle during Framework initialization */
  sns_sam_sensor_req sensorReq;
  /* Gating type, if any, of this sensor  */
  sns_sam_gated_sensor_stream_type gatingType;
  /* Whether this sensor is currently available via SMR */
  bool isAvailable;
  /* Whether this is a local algorithm serviced by SAM;
   *i.e. is this of type sns_sam_sensor_algo  */
  bool isLocal;
  /* Dynamic Shared Object File Info */
  struct sns_sam_dl_info *dlInfo;
  /* Provider API associated with this sensor.
   * PEND: To avoid duplication, provAPI and sensorUID should be stored in sensor.sensorReq */
  struct sns_sam_prov_api *provAPI;
};
typedef struct sns_sam_sensor sns_sam_sensor;


struct sns_sam_prov_callback
{
  uint32_t structSize; /**< Size of this structure. */

  /**
   * Opaque handle back to the SAM Framework. This is used within callback
   * functions so the framework knows from which client the request
   * originates. */
  struct sns_sam_sensor_req const *sensorReq;

  /**
   * Send a request message on a specific connection.
   *
   * @param[io] sensorReq Connection handle for this stream
   * @param[io] enableReqMsg Request message to be sent
   *
   * @return
   * SAM_ENONE -- Operation was successful
   * SAM_ETYPE -- Unknown sensor UID or message ID
   * SAM_ESMR -- SMR error received
   */
  sns_sam_err (*sns_sam_prov_cb_send_req)(
      sns_sam_sensor_req const *sensorReq,
      sns_sam_enable_req const *enableReqMsg );

  /**
   * Look-up and return a particular sensor.  If not found, create and
   * initiatlize a new sensor object.
   *
   * @param[i] sensorUID SUID of the sensor intended to be added
   *
   * @return Newly created sensor
   */
  sns_sam_sensor* (*sns_sam_prov_get_sensor)(
      sns_sam_sensor_uid sensorUID );

  /**
   * The provided sensor is now fully available, and can be advertised as a
   * depdendency to the applicable algorithms.
   */
  sns_sam_err (*sns_sam_prov_sensor_available)(
      sns_sam_sensor const *sensor );

  /**
   * Remove this sensor from the sensor queue.  Stop all active streams, and
   * disconnect from SMR/QMI.
   *
   * @param[io] sensorUID Sensor to remove
   *
   * @return
   * SAM_ENONE -- Operation was successful
   */
  sns_sam_err (*sns_sam_prov_remove_sensor)(
      sns_sam_sensor_uid sensorUID );

  /**
   * Stop this stream, and disconnect from the service.
   *
   * @param[io] sensorReq Connection handle for this stream
   *
   * @return
   * SAM_ENONE -- Operation was successful
   */
  sns_sam_err (*sns_sam_prov_stop_stream)(
      sns_sam_sensor_req const *sensorReq );

  /**
   * Indicate to the Framework that the get_data process has been completed.
   *
   * @param[i] sensorReq Sensor stream associated with the response message
   */
  void (*sns_sam_prov_get_data_complete)(
      sns_sam_sensor_req const *sensorReq );

  /**
   *  Immediately exit uImage.  The Framework will handle re-entering uImage.
   */
  void (*sns_sam_prov_uimage_exit)( void );
};
typedef struct sns_sam_prov_callback sns_sam_prov_callback;

struct sns_sam_prov_api
{
  uint32_t structSize; /**< Size of this structure. */

  /**
   * An outgoing request message, generated by an algorithm, is about to be
   * sent.  The provider may perform any additional set-up or configuration.
   *
   * @param[io] cb Callback functions; Connection handle for this stream
   * @param[io] enableReqMsg Request message generated by the algorithm
   *
   * @return
   * SAM_ENONE -- Operation was successful
   * SAM_ESTATE -- Do not send this request; cleanup connection state
   */
  sns_sam_err (*sns_sam_prov_handle_req)(
      sns_sam_prov_callback const *cb, sns_sam_enable_req *enableReqMsg );

  /**
   * All incoming response messages from a service associated with this
   * provider will be handled within this function.
   *
   * @param[io] cb Callback functions; Connection handle for this stream
   * @param[io] respMsg Response message received from dependent service
   *
   * @return
   * SAM_ENONE -- Operation was successful
   */
  sns_sam_err (*sns_sam_prov_handle_resp)(
      sns_sam_prov_callback const *cb, sns_sam_resp_msg const *respMsg );

  /**
   * All incoming indication messages from a service associated with this
   * provider will be fed to this function.  Providers are expected
   * to appropriately handle error indications.
   *
   * @param[io] cb Callback functions; Connection handle for this stream
   * @param[io] indMsg Indication message received from dependent service
   *
   * @return
   * SAM_ENONE -- Operation was successful
   * SAM_ESTATE -- Do not feed this indication message to any algorithm
   */
  sns_sam_err (*sns_sam_prov_handle_ind)(
      sns_sam_prov_callback const *cb, sns_sam_ind_msg const *indMsg );

  /**
   * Obtain the QMI service object associated with this Provider.
   * Framework will call sns_sam_prov_sensor_list once when the service becomes
   * available.
   *
   * @param[io] cb Callback functions;
   *
   * @return QMI service objects
   */
  qmi_idl_service_object_type (*sns_sam_prov_service)(
      sns_sam_prov_callback const *cb );

  /**
   * The provider must start the process of generating its list of provided
   * sensors.
   *
   * @param[io] cb Callback functions; Connection handle for this stream
   *
   * @return
   * SAM_ENONE -- Operation was successful
   */
  sns_sam_err (*sns_sam_prov_sensor_list)(
      sns_sam_prov_callback const *cb );

  /**
   * Send the appropriate requests to set the batch period consistent with
   * the period set in the sensorReq object.
   *
   * @param[io] cb Callback functions; Connection handle for this stream
   *
   * @return
   * SAM_ENONE -- Operation was successful
   * SAM_EFAILED -- Unable to send request message
   * SAM_ENOT_SUPPORTED -- Provider does not support batching via this function
   */
  sns_sam_err (*sns_sam_prov_set_batch_period)(
      sns_sam_prov_callback const *cb );

  /**
   * Force this sensor to send to SAM all unsent data.
   *
   * @param[io] cb Callback functions; Connection handle for this stream
   *
   * @return
   * SAM_ENONE -- Operation was successful
   * SAM_ENOT_SUPPORTED -- Provider does not support this feature
   */
  sns_sam_err (*sns_sam_prov_get_data)(
      sns_sam_prov_callback const *cb );
};
typedef struct sns_sam_prov_api sns_sam_prov_api;

/**
 * This function is registered for each Provider. It returns basic information
 * about the provider and how the SAM Framework can use it. It is only called
 * once during the SAM Framework initialization.
 *
 * @param[o] provAPI Provider API; function is NULL if not supported.
 *
 * @return
 * SAM_ENONE -- Operation was successful.
 */
typedef sns_sam_err (*sns_sam_prov_register)(
      sns_sam_prov_api **provAPI );

/*============================================================================
  Global Data
  ===========================================================================*/

extern const sns_sam_prov_register samProvRegisterFuncs[];

/* SAM module ID to be used for debug string messages */
extern sns_debug_module_id_e samModule;

#endif /* SNS_SAM_PROV_API_H */
