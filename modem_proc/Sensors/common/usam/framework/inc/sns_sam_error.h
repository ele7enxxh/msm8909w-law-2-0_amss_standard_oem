#ifndef SNS_SAM_ERROR_H
#define SNS_SAM_ERROR_H

/*============================================================================
  @file sns_sam_error.c

  @brief
  Handles processing of all errors in the SAM Framework.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_queue.h"
#include "sns_sam.h"
#include "sns_sam_algo_api.h"

/*============================================================================
  Type Declarations
  ===========================================================================*/

/**
 * Souce of the error.
 */
typedef enum
{
  /* The error occured internal to SAM */
  SNS_SAM_ERR_INTERNAL,
  /* The error was reported by an SMR error callback */
  SNS_SAM_ERR_SMR
} sns_sam_error_source;

/**
 * Error data is stored in this object, and processed asychronously
 * later within the SAM main thread.
 */
struct sns_sam_client_error
{
  /* Data fields necessary to add this object to a SAM list */
  sns_q_link_s qLink;
  /* Source of the error */
  sns_sam_error_source errorSource;
  /* Error code */
  sns_sam_err error;
   /* Data used by error handler.*/
  void *errCBData;
};
typedef struct sns_sam_client_error sns_sam_client_error;


/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Handle all received errors.
 */
void sns_sam_process_error_q( void );

/**
 * Report an error to be processed Asyncronously.
 *
 * @param[i] clientErr Error data
 */
void sns_sam_report_error( sns_sam_client_error *clientErr );

/**
 * Cancel and remove all algorithm instances dependent on this failing sensor
 * stream.  Send an error indication to all affected clients.
 *
 * @param[i] sensorReq Sensor request which failed
 * @param[i] errCode Error code to place in the error indication
 */
void sns_sam_remove_algo_instances( sns_sam_sensor_req const *sensorReq,
  uint8_t errCode );

/**
 * Initialize the data structures used by the error processing framework
 *
 * @return SAM_ENONE if successful
 *         SAM_EFAILED otherwise
 */
sns_sam_err sns_sam_init_error_fw( void );

/**
 * Search the error queue and remove all occurances of algoInst in the error
 * queue
 *
 * @param[i] algoInst  algorithm instance to be removed
 */
void
sns_sam_remove_from_error_queue ( sns_sam_algo_inst * algoInst );

#endif /* define SNS_SAM_ERROR_H */
