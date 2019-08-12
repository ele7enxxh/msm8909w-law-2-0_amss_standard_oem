#ifndef SNS_SAM_DEP_H
#define SNS_SAM_DEP_H

/*============================================================================
  @file sns_sam_dep.h

  @brief
  Functions and operations related to handling and maintaining dependent sensor
  requests.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_common.h"
#include "qmi_idl_lib.h"
#include "sns_queue.h"
#include "sns_sam_algo_api.h"

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Calculate the aggregate minimum report period for all clients sharing
 * this algorithm instance, and use the algorithm API to inform the instance.
 *
 * @param[i] algoInst Active algorithm instance
 *
 */
void sns_sam_inform_report_change( sns_sam_algo_inst const *algoInst );

/**
 * Find an acceptable sensor request for this algorithm dependency.  Create
 * new request if none are available.  This function will also rectify all
 * requests to this particular sensor request.
 *
 * Only one request per dependent sensor is allowed, so this
 * request may replace an existing one (will send a new request, and cancel
 * the existing one).
 *
 * @param[i] algoInst Active algorithm instance
 * @param[i] sensorUID Dependent sensor to be enabled
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Error sending enable request message
 *         SAM_ENOMEM Not enough memory to allocate or enable new sensor req
 *         SAM_EMAX Too many sensor requests for this algorithm instance
 *         SAM_ETYPE Unknown/unavailable sensor UID
 */
sns_sam_err sns_sam_handle_sensor_change( sns_sam_algo_inst *algoInst,
  sns_sam_sensor_uid const *sensorUID );

/**
 * Start all dependencies for a specified algorithm
 *
 * @param[i] algo The associated algorithm
 * @param[o] algoInst The created algorithm instance object
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Unable to initialize the algorithm dependencies
 */
sns_sam_err sns_sam_start_dependencies( sns_sam_sensor_algo const *algo,
  sns_sam_algo_inst *algoInst );

/**
 * Update the batch period for a dependent sensor (only applicable for SAM
 * sensors).  If the sensor request is shared amongst other algorithm
 * instances, we will simply note the request, but not change the stream.
 *
 * @param[i] algoInst Active algorithm instance
 * @param[i] sensorUID Dependent sensor to be disabled
 * @param[i] batchPeriod Batch period requested (in seconds, Q16)
 *
 * @return SAM_ENONE
 *         SAM_ETYPE There is no sensor of sensorUID presently associated
 *         SAM_EFAILED Error sending batch request
 */
sns_sam_err sns_sam_set_batch_period( sns_sam_algo_inst *algoInst,
  sns_sam_sensor_uid const *sensorUID, uint32_t batchPeriod );

/**
 * Iterate over the sensorReqQ; remove and disable all sensor streams
 * that have no active users.
 */
void sns_sam_remove_unused_sensor_reqs( void );

/**
 * Remove the reference to a algorithm instance, from all matching sensor
 * requests in the sensor request queue.
 *
 * @param[i] algoInst Algorithm instance to match in each sensorReq->algoInst
 */
void sns_sam_remove_all_ai_ref( sns_sam_algo_inst const *algoInst );

/**
 * Initialize all static objects contained within sns_sam_dep.c.
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to initialize Framework
 *         SAM_EFAILED Framework initialization failed
 */
sns_sam_err sns_sam_dep_init_fw( void );

/**
 * Send flush requests to each of an algorithm's dependencies.
 *
 * @param[i] algoInst Algorithm Instance to flush
 *
 * @return SAM_ENONE All requests sent successfully
 *         SAM_EFAILED One or more request messages failed
 *         SAM_ENOT_AVAILABLE No requests were available for flushing
 */
sns_sam_err sns_sam_send_flush( sns_sam_algo_inst const *algoInst );

/**
 * Start the flush procedure for the given client request.  Will involve
 * sending flush requests to this algorithm's dependencies, and sending
 * all batched data.
 *
 * @param[i] clientReq Client whom initiated the flush
 * @param[i] reqHndl Handle to use when ultimately sending the response;
 *                   NULL if flush was not prompted by a request
 *
 * @return true if the response message will be sent asynchronously
 *         false if the caller is expected to send the response
 */
bool sns_sam_initiate_flush( sam_client_req *clientReq,
    smr_req_handle reqHndl );

/**
 * Check whether any dependent sensors of this algorithm instance have their
 * flushInProgress flags set.
 *
 * @param[i] algoInst Algorithm Instance to check
 *
 * @return true if any dependent sensor has a flush in progress; false otherwise
 */
bool sns_sam_fip_check( sns_sam_algo_inst const *algoInst );

/**
 * After receiving a flush response message (currently batch response for
 * SAM algorithms, or SMGR buffering response for physical sensors), check
 * whether any flushes-in-progress were waiting for it.
 *
 * @param[i] sensorReq Sensor stream associated with the response message
 */
void sns_sam_handle_flush_response( sns_sam_sensor_req const *sensorReq );

/**
 * Update all applicable algorithms that a new dependent sensor has become
 * available.
 *
 * @param[i] sensor The new sensor that is available for use
 */
void sns_sam_dep_available( sns_sam_sensor const *sensor );

/**
 * Find a sensor request object matching the <algoInst, sensorUID> pair
 * from within the sensorReqQ.
 *
 * @param[i] algoInst Algo instance that must be referenced by the sensor req
 * @param[i] sensorUID Sensor type of the sensor request
 *
 * @return Found sensor request; or NULL if none exist matching criteria
 */
sns_sam_sensor_req* sns_sam_find_sensor_req( sns_sam_algo_inst const *algoInst,
  sns_sam_sensor_uid const *sensorUID );

/**
 * Cancel and remove all algorithm instances dependent on this failing sensor
 * stream.  Send an error indication to all affected clients.
 *
 * @param[i] sensorReq Sensor request which failed
 * @param[i] errCode Error code to place in the error indication
 */
void sns_sam_remove_algo_instances( sns_sam_sensor_req const *sensorReq,
  uint8_t errCode );

#endif /* SNS_SAM_DEP_H */
