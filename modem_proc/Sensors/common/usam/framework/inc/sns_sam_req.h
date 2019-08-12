#ifndef SNS_SAM_REQ_H
#define SNS_SAM_REQ_H

/*============================================================================
  @file sns_sam_req.h

  @brief
  Handles processing of all incoming request messages to the SAM Framework.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "qmi_idl_lib.h"
#include "fixed_point.h"
#include "sns_queue.h"
#include "sns_sam.h"
#include "sns_usmr.h"

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Removes a client request, and does all additional necessary cleanup.
 * Will remove orphaned algorithm instances.  See q_action_func_t template.
 *
 * @param[i] clientReq Client request to be removed
 */
void sns_sam_remove_client_req( sam_client_req *clientReq, void *unused );

/**
 * Removes all client request for a specific client connection.
 *
 * @param[i] serviceHndl Associated SMR service handle
 */
void sns_sam_remove_all_client_req( smr_qmi_client_handle serviceHndl );

/**
 * Handle a request message sent to a SAM algorithm.
 *
 * @param[i] reqMsg Request message to process
 */
void sns_sam_handle_req( sns_sam_req_msg const *reqMsg );

/**
 * Mark a SAM client as "not busy", which controls whether indications
 * will be sent.
 *
 * @param[i] serviceHndl SMR handle representing the client connection
 */
void sns_sam_mark_client_avail( smr_service_hndl serviceHndl );

/**
 * Generate a new output data from an algorithm instance.
 *
 * @param[i] algoInst The associated algorithm instance
 * @param[o] outputData Generated output
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate algo inst object
 *         SAM_EFAILED The algorithm failed to generate an output data
 */
sns_sam_err sns_sam_generate_output( sns_sam_algo_inst *algoInst,
    sns_sam_algo_output *outputDataIn );

/**
 * Form and send all batch response messages for the given client request.
 * One response will be sent for each flush request in the client's queue.
 *
 * @param[io] clientReq Process and clear its flushReqQ
 */
void sns_sam_send_batch_resp( sam_client_req *clientReq );

#endif /* SNS_SAM_REQ_H */
