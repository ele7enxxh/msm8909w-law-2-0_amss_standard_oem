#ifndef SNS_SAM_CLIENT_H
#define SNS_SAM_CLIENT_H

/*============================================================================
  @file sns_sam_client.h

  @brief
  The SAM Framework both acts as a QMI service and a QMI client.  This file
  contains the declarations for all client duties.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "qmi_idl_lib.h"
#include "fixed_point.h"
#include "sns_queue.h"
#include "sns_sam_algo_api.h"
#include "sns_usmr.h"

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Register as a SMR client for the specified sensor request. Note that the
 * serviceObj and clientHndl fields must be set prior calling this function.
 *
 * @param[i] sensorReq Sensor to initialize
 * @param[i] timeout How long to wait for the service (in milliseconds);
 *                   0 = indefinitely
 * @param[i] uImage Whether this connection can benefit from being in uImage
 *
 * @return SAM_ENONE
 *         SAM_ENOT_AVAILABLE Client has already been initialized
 *         SAM_ESMR SMR error received
 */
sns_sam_err sns_sam_client_init( sns_sam_sensor_req *sensorReq,
    uint32_t timeout, bool uImage );

/**
 * Release an SMR client connection.  Dynamically allocated memory associated
 * with the sensor request object will be freed asynchronously.
 *
 * @param[i] sensorReq Sensor to release
 */
void sns_sam_client_release( sns_sam_sensor_req *sensorReq );

/**
 * Check if the specified QMI service-Instance ID combo is available.
 * Registers a callback function to alert the SAM Framework.
 *
 * @param[i] serviceObj QMI service to look for
 * @param[i] instanceID Instance ID to look for; or SMR_CLIENT_INSTANCE_ANY
 * @param[i] timeout How long to wait for the service; 0 indefinitely
 *
 * @return SAM_ENONE
 *         SAM_ESMR SMR error received
 */
sns_sam_err sns_sam_client_check( qmi_idl_service_object_type serviceObj,
    qmi_service_instance instanceID, uint32_t timeout );

/**
 * Send a request message to some other sensor service.
 *
 * @note The callee should not free the request message at this time, but
 *       instead wait until after the response has been received/processed.
 *
 * @param[i] sensorReq Sensor request associated with this message
 * @param[i] reqMsg Message to be sent
 *
 * @return SAM_ENONE
 *         SAM_ETYPE Unknown sensor UID or message ID
 *         SAM_ESMR SMR error received
 */
sns_sam_err sns_sam_client_send( sns_sam_sensor_req const *sensorReq,
    struct sns_sam_msg const *reqMsg );

/**
 * Process all indication and response messages on the queue.  Will result
 * in calls to sns_sam_handle_resp() and sns_sam_handle_ind().
 */
void sns_sam_process_clnt_msg_q( void );

/**
 * Process all client release callback events.  Will result in calls to
 * sns_sam_handle_release_cb().
 */
void sns_sam_process_release_q( void );

/**
 * Initialize all static objects contained within sns_sam_client.c.
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to initialize Framework
 *         SAM_EFAILED Framework initialization failed
 */
sns_sam_err sns_sam_client_init_fw( void );

/**
 * Lookup the maximum enable request message size for an algorithm's
 * dependencies.
 *
 * @param[i] sensorUID Sensor that we wish to query
 *
 * @return Size of the QMI c-struct in bytes
 */
uint32_t sns_sam_max_message_size( sns_sam_sensor_uid const *sensorUID );

#endif /* SNS_SAM_CLIENT_H */
