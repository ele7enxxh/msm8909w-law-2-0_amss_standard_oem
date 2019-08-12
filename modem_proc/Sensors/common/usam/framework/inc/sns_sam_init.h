#ifndef SNS_SAM_INIT_H
#define SNS_SAM_INIT_H

/*============================================================================
  @file sns_sam_init.h

  @brief
  Functions and operations that are primarily exercised during the SAM
  Framework initialization process.  Handles initialization of local SAM
  algorithms, as well as their remote dependent sensors and registry data.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_common.h"
#include "qmi_idl_lib.h"
#include "sns_queue.h"
#include "sns_sam_algo_api.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/* How long to wait for dependent sensors/services before giving up on them.
 * In microseconds. */
#define SNS_SAM_INIT_CLIENT_TIMEOUT_US 30000000 // 30 Seconds

/* How long to wait for all registry responses before marking them as
 * unattainable.  In microseconds. */
#define SNS_SAM_INIT_REG_TIMEOUT_US 5000000

/*============================================================================
  Type Declarations
  ===========================================================================*/

/**
 * An instance of this object is created whenever a new QMI service becomes
 * available.  We will want to send an info query to the service, and register
 * SAM as a client.
 */
struct sns_sam_client_init_data
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* QMI service object */
  qmi_idl_service_object_type serviceObj;
  /* Instance ID of the service found */
  qmi_service_instance instanceID;
  /* Whether the timeout expired */
  bool timeoutExpired;
};
typedef struct sns_sam_client_init_data sns_sam_client_init_data;

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Adds recently received client init data to the internal queue.  Will be
 * processed in sns_sam_init_clients().
 *
 * @param[i] initData Init data as returned from sns_sam_client_init_cb().
 */
void sns_sam_handle_client_init_cb( sns_sam_client_init_data *initData );

/**
 * Initializes a SMR connection to all available but not yet connected
 * dependent algorithms and sensors.  Typically would be called after the
 * SNS_SAM_CLIENT_INIT_SIG signal is received.
 */
void sns_sam_init_clients( void );

/**
 * A local algorithm has signalled that its dependencies have been met.  Now
 * we must register it with SMR and perform other initialization duties.
 *
 * @note This function may be called multiple times per algorithm; subsequent
 * calls will be effectively ignored.
 *
 * @param[i] algo Algorithm whose dependencies have been met
 */
void sns_sam_init_dep_met( sns_sam_sensor_algo const *algo );

/**
 * Begin the initialization process for the SAM Framework.  Sends numerous
 * request messages simultaneously, and the response messages will prompt
 * further processing.
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Algorithm initialization failed
 */
sns_sam_err sns_sam_init_algos( void );

/**
 * Allocate memory for a new sensor algorithm object.
 *
 * @param[o] algo Algorithm pointer
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate sensor_algo object
 */
sns_sam_err
sns_sam_init_algo( sns_sam_sensor_algo **algo );

/**
 * De-initialize and de-allocate memory of a sensor algorithm object.
 *
 * @param[i] algo Algorithm pointer
 */
void
sns_sam_deinit_algo(sns_sam_sensor_algo *algo);

/**
 * Initialize all framework lists and data structures unrelated to algorithms.
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Framework initialization failed
 */
sns_sam_err sns_sam_init_fw( void );

/**
 * Handle the registry timeout event.  This event will occur exactly once
 * during the SAM Framework initialization.  Set all outstanding registry
 * requests as fulfilled.
 */
void sns_sam_handle_reg_timeout( void );

/**
 * Allocate memory for a new sensor object.
 *
 * @param[o] sensor Sensor object pointer
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate sns_sam_sensor
 */
sns_sam_err sns_sam_init_sensor( sns_sam_sensor **sensor );

#endif /* SNS_SAM_INIT_H */
