#ifndef SNS_SAM_IO_H
#define SNS_SAM_IO_H

/*============================================================================
  @file sns_sam_io.h

  @brief
  Manages all input/output data storage for algorithms and clients within
  the SAM Framework.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*============================================================================
  Include Files
  ===========================================================================*/
#include "sns_sam_algo_api.h"
#include "sns_sam.h"

/*============================================================================
  Type Declarations
  ===========================================================================*/
typedef enum {
  /* Drop all unnecessary data */
  SNS_SAM_IO_DROP_DATA,
  /* If a single AP-facing client exists, flush all data */
  SNS_SAM_IO_FLUSH_AP,
  /* Flush all output data stored in TCM to DDR */
  SNS_SAM_IO_FLUSH_DDR,
  /* Clients may request to have batched data sent when TCM is low; send data
   * for those clients now. */
  SNS_SAM_IO_SEND_BATCH,
} sns_sam_io_lowmem_action;

struct sns_sam_io_lowmem_data
{
  /* Data fields necessary to add this object to a SAM list */
  sns_q_link_s qLink;
  /* Action to be performed by the main SAM thread */
  sns_sam_io_lowmem_action action;
};
typedef struct sns_sam_io_lowmem_data sns_sam_io_lowmem_data;

#define SNS_SAM_DEFAULT_RESERVED_FIFO_SIZE 150

/*============================================================================
  External Objects
  ===========================================================================*/
extern sns_q_s samLowMemQ;
extern OS_EVENT *samLowMemQMutex;

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Determine the number of batched output objects for this client.
 *
 * @param[i] clientReq The client to investigate
 *
 * @return Number of output objects stored by SAM
 */
uint32_t sns_sam_output_cnt( sam_client_req const *clientReq );

/**
 * Check whether any of the batched output are contained in bigImage bins.
 *
 * @note In practice, this function should be cheap, even though it
 * theoretically iterates over the entire queue.  This is because DDR bins are
 * most likely at beginning, causing the loop to exit.  If everything is in TCM,
 * the queue size will be at most on the order of 20.
 *
 * @param[i] clientReq Client whose output queue to check
 *
 * @return True if there is at least one bigImage bin; false otherwise
 */
bool sns_sam_output_bimg( sam_client_req const *clientReq );

/**
 * Push a new output object to the end of the output buffer.
 *
 * @param[io] clientReq The output queue on which to copy the buffer
 * @param[i] output The output data object
 */
void sns_sam_output_push( sam_client_req *clientReq,
    sns_sam_algo_output const *output );

/**
 * Get and remove the oldest output object from the batch list.
 * Data bins are not freed until the call to sns_sam_output_release.
 *
 * @param[io] clientReq Client from whose output buffer to take from
 *
 * @return Output data object, or NULL if none are available
 */
sns_sam_algo_output sns_sam_output_pop( sam_client_req *clientReq );

/**
 * Release and free processed output data.  Data is marked as consumed
 * from within sns_sam_output_pop; may need to be rolled-back if QMI
 * connection is busy.
 *
 * @param[io] clientReq Client whose output buffer to update/release
 * @param[i] cnt Number of output objects to "un-delete"/roll-back.
 *               For most scenarios this value should be 0.
 */
void sns_sam_output_release( sam_client_req *clientReq, uint32_t cnt );

/**
 * Clear and free all batched output for this client.
 *
 * @param[io] clientReq Client whose output data to clear
 */
void sns_sam_output_clear( sam_client_req *clientReq );

/**
 * Determine the number of buffered input objects for an algorithm instance.
 *
 * @param[i] algoInst Algorithm Instance in question
 * @param[i] sensorUID If non-NULL, only return count of this input type
 *
 * @return Number of input data objects found
 */
uint32_t sns_sam_input_cnt( sns_sam_algo_inst const *algoInst,
     sns_sam_sensor_uid const *sensorUID );

/**
 * Clear and free all buffered inpuit for this algorithm instance.
 *
 * @param[io] algoInst Algorithm instance whose input buffer to clear
 */
void sns_sam_input_clear( sns_sam_algo_inst *algoInst );

/**
 * Push a new input object to the end of the input buffer.
 *
 * @param[io] algoInst Algorithm instance to add the input
 * @param[i] sensorUID Sensor type of this input data (or NULL)
 * @param[i] input Input object from which to copy
 */
void sns_sam_input_push( sns_sam_algo_inst *algoInst,
    sns_sam_sensor_uid const *sensorUID, sns_sam_algo_input const *input );

/**
 * Get and remove the oldest input object of the specified type from the
 * algorithm instance's input buffer.  If sensorUID is NULL, take the newest
 * across all input types.
 *
 * @param[io] algoInst Algorithm instance from which to remove the input
 * @param[i] sensorUID Sensor type to remove (or NULL)
 * @param[i] maxInputTS Restrict returned input to have TS less than this max
 *
 * @return Sensor object with corresponding timestamp; or NULL if N/A
 */
sns_sam_algo_input sns_sam_input_pop( sns_sam_algo_inst *algoInst,
    sns_sam_sensor_uid const *sensorUID, sns_sam_timestamp maxInputTS );

/**
 * Find the input queue of the input object with the earliest timestamp.
 *
 * @param[i] algoInst Algorithm instance's queues to search through
 * @param[i] sensorUID Sensor type to filter (or NULL for no filter)
 * @param[o] earliestTS Timestamp of the earliest input in the returned queue;
 *
 * @return Queue with the earliest input timestamp; NULL if none found.
 */
sns_sam_algo_input_q* sns_sam_input_earliest( sns_sam_algo_inst *algoInst,
    sns_sam_sensor_uid const *sensorUID, sns_sam_timestamp *earliestTS );

/**
 * Move all algorithm output from TCM to DDR.  Update queue pointers
 * appropriately.
 *
 * @param[io] clientReq Client whose output to move
 */
void sns_sam_move_output( sam_client_req *clientReq );

/**
 * Drop all unneeded batched algorithm output.  "Unneeded" is defined as:
 * - Data being stored for a non-wakeup client, past the reserved FIFO size.
 * - Data being stored for a "busy" client, past the reserved FIFO size.
 *
 * @param[i] 
 */
void sns_sam_drop_unneeded_output();

/**
 * @return The available space within a single i/o bin.  Used to check whether
 * algorithms, during initialization, will function.
 */
uint32_t sns_sam_io_size( void );

/**
 * @return The maximum # of i/o bins that will be available for use.
 */
uint32_t sns_sam_io_cnt( void );

/**
 * Initialize all data structures and internal state for the input/output
 * management portion of the SAM Framework.
 */
sns_sam_err sns_sam_io_init_fw( void );

#endif /* SNS_SAM_IO_H */
