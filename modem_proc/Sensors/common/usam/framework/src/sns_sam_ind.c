/*============================================================================
  @file sns_sam_ind.c

  @brief
  All indication processing (both incoming and outgoing) for the SAM
  Framework.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_sam_ped_v01.h"
#include "sns_sam_ped_v01.h"
#include "sns_smgr_api_v01.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_smgr_common_v01.h"
#include "sns_init.h"
#include "sns_sam_algo_api.h"
#include "sns_sam.h"
#include "sns_sam_client.h"
#include "sns_sam_service.h"
#include "sns_sam_req.h"
#include "sns_sam_cb.h"
#include "sns_sam_ind.h"
#include "sns_sam_pm.h"
#include "sns_sam_dep.h"
#include "sns_profiling.h"
#include "sns_sam_io.h"
#include "sns_sam_prov.h"

/*============================================================================
  Type Declarations
  ===========================================================================*/

/**
 * Used to store lists of client requests whose report or batch timers have
 * fired and need to be handled.
 */
struct sns_sam_client_event
{
  sns_q_link_s qLink;
  sam_client_req *clientReq;
};
typedef struct sns_sam_client_event sns_sam_client_event;

typedef void (*sns_sam_timer_cb)(void *);

/*============================================================================
  Static Data
  ===========================================================================*/
/* Client Requests, whose Batch Timers have fired (and need to be handled) */
sns_q_s samBatchQ SNS_SAM_UIMAGE_DATA;
OS_EVENT *samBatchQMutex SNS_SAM_UIMAGE_DATA;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_alloc_ind_msg( sns_sam_sensor_algo const *algo,
    struct sns_sam_msg *message, bool isBatchInd )
{
  uint32_t msgSize = isBatchInd ? algo->qmiBatchIndSize : algo->qmiIndSize;
  if( NULL != algo && 0 < msgSize )
  {
    message->msg.buf = (intptr_t)SNS_OS_ANY_MALLOC( SNS_SAM_DBG_MOD, msgSize );
    if(message->msg.buf == (intptr_t)NULL)
    {
      return SAM_ENOMEM;
    }
    SNS_OS_MEMZERO( (void*)message->msg.buf, algo->qmiIndSize );
    message->msg.bufSize = msgSize;

    message->msgID = isBatchInd ? SNS_SAM_ALGO_BATCH_IND : SNS_SAM_ALGO_REPORT_IND;
    message->sensorUID = algo->sensor.sensorUID;
  }
  else
  {
    SNS_UMSG_ERROR_0( samModule, "Sensor lookup failure" );
    return SAM_ETYPE;
  }

  return SAM_ENONE;
}

/**
 * Create and send a new SAM report indication message.
 *
 * @param[i] clientReq Destination client
 * @param[i] outputData Data to be used to generated the report
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to allocate indication buffer
 *         SAM_ENOT_AVAILABLE SMR connection is presently busy
 *         SAM_EFAILED Error generating or sending the indication message
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_err
sns_sam_create_ind( sam_client_req const *clientReq,
    sns_sam_algo_output const *outputData )
{
  sns_sam_err err, rv = SAM_ENONE;
  sns_sam_ind_msg indMsg;
  sns_sam_ped_report_ind_msg_v01 *msg;

  sns_profiling_log_qdss( SNS_SAM_ALGO_REPORT_ENTER, 2,
    (uint32_t)*clientReq->algoInstance->algorithm->sensor.sensorUID,
    (uint32_t)clientReq->algoInstance );

  err = sns_sam_alloc_ind_msg( clientReq->algorithm, &indMsg, false );
  if( SAM_ENONE == err )
  {
    msg = (sns_sam_ped_report_ind_msg_v01*)indMsg.msg.buf;
    msg->instance_id = clientReq->extInstanceID;

    cbFuncs.algoHndl = (struct sns_sam_algo_handle*)clientReq;
    err = clientReq->algorithm->algoMsgAPI->sns_sam_algo_gen_ind(
        &cbFuncs, outputData, clientReq->extInstanceID, &indMsg );

    if( SAM_ENONE != err )
    {
      SNS_UMSG_ERROR_2( samModule,
        "Error generating indication for %x, error: %i",
        *clientReq->algorithm->sensor.sensorUID, err );
      rv = SAM_EFAILED;
    }
    else
    {
      err = sns_sam_service_send_ind( &indMsg, clientReq );
      if( SAM_ENOT_AVAILABLE == err )
      {
        SNS_UMSG_HIGH_0( samModule, "Connection busy, buffering data" );
        rv = SAM_ENOT_AVAILABLE;
      }
      else if( SAM_ENONE != err )
      {
        SNS_UMSG_ERROR_2( samModule,
          "Error sending indication for %x, error %i",
          *clientReq->algorithm->sensor.sensorUID, err );
        rv = SAM_EFAILED;
      }
    }

    SNS_OS_ANY_FREE( (void*)indMsg.msg.buf );
  }
  else
  {
    SNS_UMSG_ERROR_1( samModule,
        "Not enough memory to allocate indication message for %x",
        *clientReq->algorithm->sensor.sensorUID );
    rv = SAM_ENOMEM;
  }
  sns_profiling_log_qdss( SNS_SAM_ALGO_REPORT_EXIT, 3,
    (uint32_t)*clientReq->algoInstance->algorithm->sensor.sensorUID,
    (uint32_t)clientReq->algoInstance, 1 );

  return rv;
}

/**
 * Mark all matching clients as available (aka "not busy").  Resume sending any
 * pending samples.  Follows q_compare_func_t template.
 *
 * @note We are assuming that the first indication of the batch stream was sent
 * successfully, and therefore all subsequent indications will be marked as
 * either INTERMEDIATE or LAST.
 *
 * @note We do not handle periodic (i.e. non-batching) clients here.  It is
 * unlikely for those clients to become busy, and if they do, sending a large
 * stream of indications after the resume will likely fail.  Therefore, we will
 * attempt to send these clients a batch of data (which they may not process).
 *
 * @param[i] itemPtr The current client request to inspect
 * @param[i] compareObj SMR service handle
 *
 * @return 0
 */
STATIC int
sns_sam_resume_client( void *itemPtr, void *compareObject )
{
  sam_client_req *clientReq = (sam_client_req*)itemPtr;
  smr_qmi_client_handle serviceHndl = (smr_qmi_client_handle)compareObject;

  if( clientReq->serviceHndl == serviceHndl )
  {
    clientReq->clientBusy = false;

    if( 0 != sns_q_cnt( &clientReq->flushReqQ ) )
    {
      sns_sam_send_all_batch_ind( clientReq, SAM_BATCH_INTERMEDIATE_IND_V01 );
    }
    else
    {
      sns_sam_send_all_batch_ind( clientReq, SAM_BATCH_LAST_IND_V01 );
    }
  }

  return 0;
}

/**
 * Handle adding a new piece of output data to a client request.
 * - If a flush is in progress, send all full batched indications immediately
 * - By definition, we will never exceed the maximum batch size; therefore
 *   no explicit checks are necessary.
 *
 * @param[i] clientReq Client request in question
 * @param[i] outputData New output data to add to the client
 */
SNS_SAM_UIMAGE_CODE STATIC void
sns_sam_handle_batch_output( sam_client_req *clientReq,
  sns_sam_algo_output const *outputData )
{
  sns_sam_output_push( clientReq, outputData );
  bool hasFullBatch = sns_sam_has_full_batch_msg( clientReq );

  if( 0 != sns_q_cnt( &clientReq->flushReqQ ) && hasFullBatch
      && !clientReq->clientBusy )
  {
    sns_sam_send_all_batch_ind( clientReq, SAM_BATCH_INTERMEDIATE_IND_V01 );
  }
  else if( hasFullBatch && SNS_SAM_BATCH_SINGLE_V01 == clientReq->batchPeriod )
  {
    sns_sam_send_all_batch_ind( clientReq, SAM_BATCH_ONLY_IND_V01 );
  }
}

/**
 * Whether to send an indication to this client, based on its location, the
 * current state of the AP, and its preferences as specified in its enable
 * request.
 *
 * @param[i] clientReq Potential indication destination
 *
 * @return True if an indication should be send; false otherwise
 */
SNS_SAM_UIMAGE_CODE STATIC bool
sns_sam_can_send_ind( sam_client_req const *clientReq )
{
  return SNS_PROC_APPS_V01 != clientReq->clientAttr.notifySuspend.proc_type ||
    clientReq->clientAttr.notifySuspend.send_indications_during_suspend ||
    !sns_sam_pm_ap_suspend();
}

/**
 * Whether the specified client request object is still valid.
 *
 * @param[i] clientReq Client request to validate
 *
 * @return True if an valid, false otherwise
 */
SNS_SAM_UIMAGE_CODE STATIC bool
sns_sam_validate_client_request( sam_client_req const *clientReq )
{
  if(clientReq->magicNumber != 0xFFFFFFFF && !( SAM_CLIENT_REQ_MAGIC_KEY ^
    (intptr_t)clientReq->algoInstance ^ clientReq->extInstanceID ^
    (intptr_t)clientReq->algorithm ^ (intptr_t)clientReq->qLink.q_ptr ^
    clientReq->magicNumber))
  {
    return true;
  }
  return false;
}

/**
 * Find the next periodic client to be run.  If none exists, return any client.
 */
STATIC SNS_SAM_UIMAGE_CODE sam_client_req*
sns_sam_next_periodic_client( sns_sam_algo_inst const *algoInst )
{
  sns_sam_timestamp minNextReportTS;
  sns_q_link_s *qCurr = NULL;
  sam_client_req *rv = NULL, *clientReq = NULL;

  for( qCurr = sns_q_check( &algoInst->clientRequests ); NULL != qCurr;
       qCurr = sns_q_next( &algoInst->clientRequests, qCurr ) )
  {
    clientReq = (sam_client_req*)qCurr;

    if( SNS_SAM_REPORT_MODE_PERIODIC == clientReq->clientAttr.reportMode )
    {
      if( NULL == rv ||
          -1 == sns_sam_compare_ts( clientReq->nextReportTS, minNextReportTS ) )
      {
        minNextReportTS = clientReq->nextReportTS;
        rv = clientReq;
      }
    }
  }

  return NULL == rv ? clientReq : rv;
}

/**
 * Run the algorithm.  By the this function's completion, the algorithm
 * will have had the chance to consume all available input, and generate any
 * number of output.
 *
 * @param[io] algoInst Algorithm Instance to be run
 */
STATIC SNS_SAM_UIMAGE_CODE void
sns_sam_algo_run( sns_sam_algo_inst *algoInst )
{
  sns_sam_err err;
  sam_client_req *clientReq = NULL;
  uint32_t inputCnt, tempCnt = sns_sam_input_cnt( algoInst, NULL );

  do
  {
    clientReq = sns_sam_next_periodic_client( algoInst );
    if( NULL == clientReq )
    {
      break;
    }

    inputCnt = tempCnt;

    sns_profiling_log_qdss( SNS_SAM_ALGO_UPDATE_ENTER, 2,
      (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst );

    cbFuncs.algoHndl = (struct sns_sam_algo_handle*)clientReq;
    err = algoInst->algorithm->algoAPI->sns_sam_algo_update(
        &algoInst->algoStateData, &cbFuncs, &algoInst->algorithm->persistData );

    sns_profiling_log_qdss( SNS_SAM_ALGO_UPDATE_EXIT, 3,
      (uint32_t)*algoInst->algorithm->sensor.sensorUID,
      (uint32_t)algoInst, 0 );

    if( SAM_ENONE != err )
    {
      SNS_UMSG_ERROR_2( samModule,
        "Error for %x in algo update %i",
        *algoInst->algorithm->sensor.sensorUID, err );
    }

    if( SNS_SAM_REPORT_MODE_PERIODIC == clientReq->clientAttr.reportMode )
    {
      sns_sam_timestamp earliestTS;
      while ( NULL != sns_sam_input_earliest( algoInst, NULL, &earliestTS ) &&
          1 == sns_sam_compare_ts( earliestTS, clientReq->nextReportTS ) )
      {
        sns_sam_handle_report( clientReq );
        clientReq->nextReportTS +=
          sns_em_convert_sec_in_q16_to_localtick( clientReq->clientAttr.reportPeriod );
      }
    }

    tempCnt = sns_sam_input_cnt( algoInst, NULL );
  } while( 0 != tempCnt && tempCnt != inputCnt );
}

/**
 * Pass the given incoming indication message into the algorithm
 * instance, and perform any necessary processing.
 *
 * @param[i] indMsg Incoming message from algorithm's dependendent sensor
 * @param[io] algoInst Algorithm to be updated with indMsg
 */
STATIC SNS_SAM_UIMAGE_CODE void
sns_sam_process_ind( sns_sam_ind *indMsg, sns_sam_algo_inst *algoInst )
{
  sns_sam_err err;
  bool algoInstPrevUimageState = SNS_SAM_ALGOINST_IN_UIMAGE(algoInst);

  if( !algoInstPrevUimageState )
  {
    sns_sam_uimage_exit();
  }

  sns_profiling_log_qdss( SNS_SAM_ALGO_RUN_ENTER, 2,
    (uint32_t)(*algoInst->algorithm->sensor.sensorUID), (uint32_t)algoInst );

  sns_profiling_log_qdss( SNS_SAM_PROCESS_IND_ENTER, 2,
    (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst );

  cbFuncs.algoHndl = (struct sns_sam_algo_handle*)
      sns_q_check( &algoInst->clientRequests );
  err = algoInst->algorithm->algoMsgAPI->sns_sam_algo_process_ind(
      &cbFuncs, &indMsg->msg );
  if( SAM_ENONE != err )
  {
    SNS_UMSG_HIGH_2( samModule,
      "Error processing ind for %x, error %i",
      *algoInst->algorithm->sensor.sensorUID, err );
  }

  sns_profiling_log_qdss( SNS_SAM_PROCESS_IND_EXIT, 2,
    (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst );

  if( 0 < sns_sam_input_cnt( algoInst, NULL ) )
  {
    sns_sam_algo_run( algoInst );
  }

  sns_profiling_log_qdss( SNS_SAM_ALGO_RUN_EXIT, 2,
    (uint32_t)*algoInst->algorithm->sensor.sensorUID, algoInst );
}

/**
 * Generate and send a single batched indication message.  Remove sent samples
 * from client's output buffer.
 *
 * @param[io] clientReq Destination client
 * @param[i] indMsg Pre-allocated indication message to use
 * @param[i] indType What to set the ind_type field in the indication message
 *
 * @return SAM_ENONE Send is successful, or will never be
 *         SAM_ENOT_AVAILABLE SMR connection is presently busy
 */
STATIC SNS_SAM_UIMAGE_CODE sns_sam_err
sns_sam_send_batch_ind( sam_client_req *clientReq, sns_sam_ind_msg *indMsg,
    sns_sam_batch_ind_type indType )
{
  sns_sam_err err, rv = SAM_ENONE;
  sns_sam_ped_batch_ind_msg_v01 *msg;
  sns_sam_algo_inst *algoInst = clientReq->algoInstance;
  int32_t outputCnt = sns_sam_output_cnt( clientReq );

  SNS_OS_MEMZERO( (void*)indMsg->msg.buf, indMsg->msg.bufSize );
  msg = (sns_sam_ped_batch_ind_msg_v01*)indMsg->msg.buf;
  msg->instance_id = clientReq->extInstanceID;

  cbFuncs.algoHndl = (struct sns_sam_algo_handle*)clientReq;
  err = algoInst->algorithm->algoMsgAPI->sns_sam_algo_gen_batch_ind(
    &cbFuncs, clientReq->extInstanceID, indType, indMsg );

  outputCnt -= sns_sam_output_cnt( clientReq );
  SNS_UMSG_LOW_2( samModule,
      "Generated batched indication with %i samples of %x",
      outputCnt, *clientReq->algorithm->sensor.sensorUID );

  if( SAM_ESTATE == err )
  {
    SNS_UMSG_ERROR_1( samModule, "Dropping invalid output for %x",
      *clientReq->algorithm->sensor.sensorUID );
  }
  else if( SAM_ENONE != err )
  {
    SNS_UMSG_ERROR_2( samModule,
      "Unable to generate indication for %x, error %i",
      *clientReq->algorithm->sensor.sensorUID, err );
  }
  else
  {
    err = sns_sam_service_send_ind( indMsg, clientReq );
    if( SAM_ENONE != err )
    {
      SNS_UMSG_ERROR_2( samModule,
        "Error sending indication for %x, error %i",
        *clientReq->algorithm->sensor.sensorUID, err );

      if( SAM_ENOT_AVAILABLE == err )
      {
        rv = SAM_ENOT_AVAILABLE;
      }
    }
    else
    {
      clientReq->lastIndType = indType;
    }
  }

  sns_sam_output_release( clientReq,
      SAM_ENOT_AVAILABLE == rv ? outputCnt : 0 );

  return rv;
}

/**
 * Determine the appropriate ind_type to mark an outgoing batch indication
 * message.  Helper function for sns_sam_send_all_batch_ind.
 *
 * @param[i] clientReq Client request to whom we are sending indications
 * @param[i] flushInProgress (See sns_sam_send_all_batch_ind API)
 *
 * @return What to set the ind_type field in the indication message
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_batch_ind_type
sns_sam_determine_ind_type( sam_client_req const *clientReq,
    sns_sam_batch_ind_type flushInProgress )
{
  if( SAM_BATCH_FIRST_IND_V01 == flushInProgress ||
      SAM_BATCH_INTERMEDIATE_IND_V01 == flushInProgress )
  {
    return SAM_BATCH_FIRST_IND_V01 == clientReq->lastIndType ||
           SAM_BATCH_INTERMEDIATE_IND_V01 == clientReq->lastIndType
      ? SAM_BATCH_INTERMEDIATE_IND_V01 : SAM_BATCH_FIRST_IND_V01;
  }
  else if( SAM_BATCH_LAST_IND_V01 == flushInProgress ||
           SAM_BATCH_ONLY_IND_V01 == flushInProgress )
  {
    if( sns_sam_has_full_batch_msg( clientReq ) )
    {
      return SAM_BATCH_INTERMEDIATE_IND_V01 == clientReq->lastIndType ||
             SAM_BATCH_FIRST_IND_V01 == clientReq->lastIndType
        ? SAM_BATCH_INTERMEDIATE_IND_V01 : SAM_BATCH_FIRST_IND_V01;
    }
    else
    {
      return SAM_BATCH_INTERMEDIATE_IND_V01 == clientReq->lastIndType ||
             SAM_BATCH_FIRST_IND_V01 == clientReq->lastIndType
        ? SAM_BATCH_LAST_IND_V01 : SAM_BATCH_ONLY_IND_V01;
    }
  }

  return SAM_BATCH_ONLY_IND_V01;
}

/**
 * For clients that specified the SNS_SAM_BATCH_INTERNAL_V01 batch setting,
 * create and send the appropriate batched indications now.
 *
 * @param[i] sensorReq Sensor whose input indication has just been processed
 */
STATIC SNS_SAM_UIMAGE_CODE void
sns_sam_create_batch_int_ind( sns_sam_sensor_req const *sensorReq )
{
  int i;

  for( i = SNS_SAM_MAX_SHARED_STREAM - 1; i >= 0; i-- )
  {
    if( NULL != sensorReq->algoInst[ i ] )
    {
      sns_q_link_s *qCurr;
      sns_sam_algo_inst const *algoInst = sensorReq->algoInst[ i ];

      if( !SNS_SAM_ALGOINST_IN_UIMAGE(algoInst) )
      {
        sns_sam_uimage_exit();
      }

      for( qCurr = sns_q_check( &algoInst->clientRequests ); NULL != qCurr;
           qCurr = sns_q_next( &algoInst->clientRequests, qCurr ) )
      {
        sam_client_req *clientReq = (sam_client_req*)qCurr;

        if( SNS_SAM_BATCH_INTERNAL_V01 == clientReq->batchPeriod )
        {
          sns_sam_send_all_batch_ind( clientReq, SAM_BATCH_ONLY_IND_V01 );
        }
      }
    }
  }
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SAM_UIMAGE_CODE bool
sns_sam_has_full_batch_msg( sam_client_req const *clientReq )
{
  uint32_t maxSamplesPerInd =
    clientReq->algoInstance->algorithm->sensor.attributes[ SAM_ALGO_ATTR_MAX_REPORTS_PER_IND ].attrValue;

  return sns_sam_output_cnt( clientReq ) > maxSamplesPerInd;
}

SNS_SAM_UIMAGE_CODE void
sns_sam_send_all_batch_ind( sam_client_req *clientReq,
    sns_sam_batch_ind_type flushInProgress )
{
  sns_sam_err err;
  sns_sam_algo_inst *algoInst = clientReq->algoInstance;
  sns_sam_ind_msg indMsg;
  sns_sam_batch_ind_type indType;
  bool algoInstPrevUimageState = SNS_SAM_ALGOINST_IN_UIMAGE(algoInst);
  bool bimgOutput = sns_sam_output_bimg( clientReq );

  if( 0 == sns_q_cnt( &clientReq->outputDataQ ) )
  {
    return ;
  }

  sns_profiling_log_qdss( SNS_SAM_ALGO_REPORT_ENTER, 2,
    (uint32_t)*algoInst->algorithm->sensor.sensorUID, (uint32_t)algoInst );

  if( !algoInstPrevUimageState || bimgOutput )
  {
    sns_sam_uimage_exit();
  }

  err = sns_sam_alloc_ind_msg( clientReq->algorithm, &indMsg, true );
  if( SAM_ENONE != err )
  {
    indMsg.msg.buf = (intptr_t)NULL;
    sns_sam_output_clear( clientReq );
  }

  while( 0 != sns_q_cnt( &clientReq->outputDataQ ) && SAM_ENONE == err )
  {
    indType = sns_sam_determine_ind_type( clientReq, flushInProgress );

    if( SAM_BATCH_LAST_IND_V01 == indType ||
        SAM_BATCH_ONLY_IND_V01 == indType ||
        sns_sam_has_full_batch_msg( clientReq ) )
    {
      err = sns_sam_send_batch_ind( clientReq, &indMsg, indType );
    }
    else
    {
      break;
    }
  }

  SNS_OS_ANY_FREE( (void*)indMsg.msg.buf );

  sns_profiling_log_qdss( SNS_SAM_ALGO_REPORT_EXIT, 3,
     (uint32_t)*algoInst->algorithm->sensor.sensorUID,
     (uint32_t)algoInst, 0 );
}

SNS_SAM_UIMAGE_CODE void
sns_sam_handle_report( sam_client_req *clientReq )
{
  sns_sam_err err;
  sns_sam_algo_inst *algoInst = clientReq->algoInstance;

  if( 0 != clientReq->batchPeriod || sns_sam_can_send_ind( clientReq ) )
  {
    sns_sam_algo_output outputData;

    err = sns_sam_generate_output( algoInst, &outputData );
    if( SAM_ENONE != err )
    {
      SNS_UMSG_LOW_0( samModule, "No available output" );
    }
    else if( 0 == clientReq->batchPeriod && !clientReq->clientBusy )
    {
      err = sns_sam_create_ind( clientReq, &outputData );
      if( SAM_ENOT_AVAILABLE == err )
      {
        sns_sam_handle_batch_output( clientReq, &outputData );
      }
      else if( SAM_ENONE != err )
      {
        SNS_UMSG_ERROR_2( samModule,
          "Unable to generate indication for %x, error %i",
          *clientReq->algorithm->sensor.sensorUID, err );
      }
    }
    else
    {
      sns_sam_handle_batch_output( clientReq, &outputData );
    }

    SNS_OS_ANY_FREE((void*)outputData.data);
  }
  else
  {
    SNS_UMSG_LOW_0( samModule, "AP in suspend, not sending ind" );
  }
}

SNS_SAM_UIMAGE_CODE sns_sam_err
sns_sam_process_output( sns_sam_algo_inst *algoInst,
  sns_sam_algo_output const *outputData )
{
  sns_sam_err err = SAM_ENONE;
  sns_q_link_s *qCurr;

  qCurr = sns_q_check( &algoInst->clientRequests );
  while( NULL != qCurr )
  {
    sam_client_req *clientReq = (sam_client_req*)qCurr;
    qCurr = sns_q_next( &algoInst->clientRequests, qCurr );

    if( SNS_SAM_REPORT_MODE_PERIODIC != clientReq->clientAttr.reportMode )
    {
      if( 0 != clientReq->batchPeriod || clientReq->clientBusy )
      {
        sns_sam_handle_batch_output( clientReq, outputData );
      }
      else if( sns_sam_can_send_ind( clientReq ) )
      {
        err = sns_sam_create_ind( clientReq, outputData );
        if( SAM_ENOT_AVAILABLE == err )
        {
          sns_sam_handle_batch_output( clientReq, outputData );
        }
        else if( SAM_ENOMEM == err )
        {
          SNS_UMSG_ERROR_1( samModule,
            "Not enough memory to send ind for %x",
            *algoInst->algorithm->sensor.sensorUID );
          err = SAM_ENOMEM;
        }
        else if( SAM_ENONE != err )
        {
          SNS_UMSG_ERROR_2( samModule,
            "Error sending indication for %x, error %i",
            *clientReq->algorithm->sensor.sensorUID, err );
          err = SAM_EFAILED;
        }
      }
      else
      {
        clientReq->missedEvent = true;
      }
    }
  }

  return err;
}

sns_sam_err
sns_sam_ind_init_fw()
{
  const uint8_t priority0 = 0;
  uint8_t errOS = 0;

  samBatchQMutex = sns_os_mutex_create_uimg( priority0, &errOS );
  if( 0 != errOS )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule, "Cannot create mutex %i", errOS );
    return SAM_EFAILED;
  }

  sns_q_init( &samBatchQ );

  return SAM_ENONE;
}

SNS_SAM_UIMAGE_CODE void
sns_sam_handle_ind( sns_sam_ind *indMsg )
{
  int32_t i;
  sns_sam_sensor_req *sensorReq = indMsg->sensorReq;
  sns_sam_err err = SAM_ENONE;

  if( NULL != sensorReq->provAPI->sns_sam_prov_handle_ind )
  {
    provCB.sensorReq = sensorReq;
    err = sensorReq->provAPI->sns_sam_prov_handle_ind( &provCB, &indMsg->msg );
  }

  if( SAM_ENONE == err )
  {
    for( i = SNS_SAM_MAX_SHARED_STREAM - 1; i >= 0; i-- )
    {
      if( NULL != sensorReq->algoInst[ i ] )
      {
        SNS_UMSG_LOW_2( samModule, "Feeding Indication from %x to %x",
          *sensorReq->sensorUID,
          *sensorReq->algoInst[ i ]->algorithm->sensor.sensorUID );
        sns_sam_process_ind( indMsg, sensorReq->algoInst[ i ] );
      }
    }
  }
  else
  {
    SNS_UMSG_ERROR_1( samModule, "Not feeding indication from %x",
        *sensorReq->sensorUID );
  }

  SNS_OS_ANY_FREE( (void*)indMsg->msg.msg.buf );
  SNS_OS_ANY_FREE( (void*)indMsg );

  sns_sam_create_batch_int_ind( sensorReq );
}

SNS_SAM_UIMAGE_CODE void
sns_sam_handle_client_timer( void )
{
  sns_q_link_s *qCurr;
  uint8_t errOS;
  sns_sam_client_event *clientEvent;

  sns_sam_pm_adjust_mips( );
  do
  {
    sns_os_mutex_pend( samBatchQMutex, 0, &errOS );
    qCurr = sns_q_get( &samBatchQ );
    clientEvent = (sns_sam_client_event*)qCurr;
    sns_os_mutex_post( samBatchQMutex );

    if( NULL != clientEvent )
    {
      sam_client_req *clientReq = clientEvent->clientReq;
      if( sns_sam_validate_client_request( clientReq ) && !clientReq->clientBusy  )
      {
        if( SNS_PROC_APPS_V01 == clientReq->clientAttr.notifySuspend.proc_type &&
            ( clientReq->clientAttr.notifySuspend.send_indications_during_suspend ||
              !sns_sam_pm_ap_suspend() ) )
        {
          sns_sam_uimage_exit();
          sns_sam_flush_batched_ap_output();
        }
        else if( SNS_PROC_SSC_V01 == clientReq->clientAttr.notifySuspend.proc_type )
        {
          sns_sam_uimage_exit();
          sns_sam_initiate_flush( clientReq, NULL );
        }
      }
      SNS_OS_ANY_FREE( clientEvent );
    }
  } while( NULL != clientEvent );
}

SNS_SAM_UIMAGE_CODE void
sns_sam_client_timer_cb( sam_client_req *clientReq )
{
  uint8_t errOS;
  sns_sam_client_event *clientEvent;

  SNS_UMSG_LOW_0( samModule, "sns_sam_client_timer_cb" );

  clientEvent = SNS_OS_ANY_MALLOC( SNS_SAM_DBG_MOD,
      sizeof(sns_sam_client_event) );
  SNS_ASSERT(NULL != clientEvent);
  clientEvent->clientReq = clientReq;
  sns_q_link( clientEvent, (sns_q_link_s*)clientEvent );

  sns_os_mutex_pend( samBatchQMutex, 0, &errOS );
  sns_q_put( &samBatchQ, (sns_q_link_s*)clientEvent );
  sns_os_mutex_post( samBatchQMutex );

  sns_os_sigs_post( sns_sam_sig_event,
    SNS_SAM_CLIENT_TIMER_SIG, OS_FLAG_SET, &errOS );
}

void
sns_sam_flush_missed_event_output()
{
  sns_q_link_s *qCurrAI, *qCurrCR;

  SNS_PRINTF_STRING_LOW_0( samModule, "Flushing all outputs not sent earlier" );
  sns_profiling_log_qdss( SNS_SAM_START_FLUSH_TO_AP, 0 );


  for( qCurrAI = sns_q_check( &algoInstQ ); NULL != qCurrAI;
       qCurrAI = sns_q_next( &algoInstQ, qCurrAI ) )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurrAI;

    for( qCurrCR = sns_q_check( &algoInst->clientRequests ); NULL != qCurrCR;
         qCurrCR = sns_q_next( &algoInst->clientRequests, qCurrCR ) )
    {
      sam_client_req *clientReq = (sam_client_req*)qCurrCR;

      if( clientReq->missedEvent && !clientReq->clientBusy)
      {
        sns_sam_handle_report(clientReq);
        clientReq->missedEvent = false;
      }
    }
  }
}

void
sns_sam_flush_batched_ap_output()
{
  sns_q_link_s *qCurrAI, *qCurrCR;

  SNS_PRINTF_STRING_LOW_0( samModule, "Flushing all batched AP output" );
  sns_profiling_log_qdss( SNS_SAM_START_FLUSH_TO_AP, 0 );

  for( qCurrAI = sns_q_check( &algoInstQ ); NULL != qCurrAI;
       qCurrAI = sns_q_next( &algoInstQ, qCurrAI ) )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurrAI;

    for( qCurrCR = sns_q_check( &algoInst->clientRequests ); NULL != qCurrCR;
         qCurrCR = sns_q_next( &algoInst->clientRequests, qCurrCR ) )
    {
      sam_client_req *clientReq = (sam_client_req*)qCurrCR;

      if( SNS_PROC_APPS_V01 == clientReq->clientAttr.notifySuspend.proc_type &&
          ( 0 != clientReq->batchPeriod ||
            SNS_SAM_REPORT_MODE_PERIODIC == clientReq->clientAttr.reportMode )
          && !clientReq->clientBusy )
      {
        sns_sam_initiate_flush( clientReq, NULL );
      }
    }
  }
}

void
sns_sam_resume_all_client( smr_qmi_client_handle clientHndl )
{
  sns_q_link_s *qCurr;

  for( qCurr = sns_q_check( &algoInstQ ); NULL != qCurr;
       qCurr = sns_q_next( &algoInstQ, qCurr ) )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurr;

    sns_q_linear_search( &algoInst->clientRequests,
      &sns_sam_resume_client, clientHndl );
  }
}

void
sns_sam_register_client_timer( sam_client_req *clientReq )
{
  sns_err_code_e err;
  uint32_t deltaTickTime = 0;

  if( (0 < clientReq->batchPeriod && FX_MAX_Q16 != clientReq->batchPeriod) ||
       SNS_SAM_REPORT_MODE_PERIODIC == clientReq->clientAttr.reportMode )
  {
    deltaTickTime = (0 < clientReq->batchPeriod && FX_MAX_Q16 != clientReq->batchPeriod) ?
                    sns_em_convert_sec_in_q16_to_localtick( clientReq->batchPeriod ) :
                    sns_em_convert_sec_in_q16_to_localtick( clientReq->clientAttr.reportPeriod );

    if( NULL == clientReq->clientTimer )
    {
      if( SNS_SAM_ALGOINST_BIGIMAGE != clientReq->algoInstance->imageMode)
      {
        err = sns_em_create_utimer_obj( (sns_sam_timer_cb)sns_sam_client_timer_cb,
          clientReq, SNS_EM_TIMER_TYPE_PERIODIC, &clientReq->clientTimer );
      }

      if( NULL == clientReq->clientTimer )
      {
        SNS_PRINTF_STRING_LOW_0( samModule, "Creating big image client timer" );
        err = sns_em_create_timer_obj( (sns_sam_timer_cb)sns_sam_client_timer_cb,
          clientReq, SNS_EM_TIMER_TYPE_PERIODIC, &clientReq->clientTimer );
      }

      if( SNS_SUCCESS != err )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
          "Error creating client timer %i", err );
        clientReq->clientTimer = NULL;
      }
    }

    if( NULL != clientReq->clientTimer )
    {
      if(sns_em_is_utimer(clientReq->clientTimer))
      {
        err = sns_em_register_utimer( clientReq->clientTimer, deltaTickTime );
      }
      else
      {
        err = sns_em_register_timer( clientReq->clientTimer, deltaTickTime );
      }
      if( SNS_SUCCESS != err )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
              "Error registering client timer %i", err );
      }
    }
  }
  else
  {
    if(NULL != clientReq->clientTimer)
    {
      if(sns_em_is_utimer(clientReq->clientTimer))
      {
        sns_em_cancel_utimer( clientReq->clientTimer );
        sns_em_delete_utimer_obj( clientReq->clientTimer );
      }
      else
      {
        sns_em_cancel_timer( clientReq->clientTimer );
        sns_em_delete_timer_obj( clientReq->clientTimer );
      }
      clientReq->clientTimer = NULL;
    }
  }
}

void sns_sam_pause_client_timers()
{
  sns_q_link_s *qCurrAI, *qCurrCR;
  for(qCurrAI = sns_q_check(&algoInstQ); NULL != qCurrAI;
      qCurrAI = sns_q_next(&algoInstQ, qCurrAI))
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst *) qCurrAI;
    for(qCurrCR = sns_q_check(&algoInst->clientRequests); NULL != qCurrCR;
        qCurrCR = sns_q_next(&algoInst->clientRequests, qCurrCR))
    {
      sam_client_req *clientReq = (sam_client_req *)qCurrCR;
      if( SNS_SAM_REPORT_MODE_PERIODIC != clientReq->clientAttr.reportMode &&
          0 != clientReq->batchPeriod &&
          SNS_PROC_APPS_V01 == clientReq->clientAttr.notifySuspend.proc_type &&
          !clientReq->clientAttr.notifySuspend.send_indications_during_suspend &&
          NULL != clientReq->clientTimer ) //Added NULL check to avoid crash in timer API
      {
        if(sns_em_is_utimer(clientReq->clientTimer))
        {
          sns_em_cancel_utimer( clientReq->clientTimer );
        }
        else
        {
          sns_em_cancel_timer( clientReq->clientTimer );
        }
      }
    }
  }
}

void sns_sam_resume_client_timers()
{
  sns_q_link_s *qCurrAI, *qCurrCR;
  uint32_t deltaTickTime;
  sns_err_code_e err;

  for(qCurrAI = sns_q_check(&algoInstQ); NULL != qCurrAI;
      qCurrAI = sns_q_next(&algoInstQ, qCurrAI))
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst *) qCurrAI;
    for(qCurrCR = sns_q_check(&algoInst->clientRequests); NULL != qCurrCR;
        qCurrCR = sns_q_next(&algoInst->clientRequests, qCurrCR))
    {
      sam_client_req *clientReq = (sam_client_req *)qCurrCR;
      if( SNS_SAM_REPORT_MODE_PERIODIC != clientReq->clientAttr.reportMode &&
          0 != clientReq->batchPeriod &&
          SNS_PROC_APPS_V01 == clientReq->clientAttr.notifySuspend.proc_type &&
          !clientReq->clientAttr.notifySuspend.send_indications_during_suspend )
      {
        if( NULL != clientReq->clientTimer )
        {
          deltaTickTime = (0 < clientReq->batchPeriod && FX_MAX_Q16 != clientReq->batchPeriod) ?
            sns_em_convert_sec_in_q16_to_localtick( clientReq->batchPeriod ) :
            sns_em_convert_sec_in_q16_to_localtick( clientReq->clientAttr.reportPeriod );

          if(sns_em_is_utimer(clientReq->clientTimer))
          {
            err = sns_em_register_utimer( clientReq->clientTimer, deltaTickTime );
          }
          else
          {
            err = sns_em_register_timer( clientReq->clientTimer, deltaTickTime );
          }

          if (SNS_SUCCESS != err )
          {
            SNS_PRINTF_STRING_ERROR_1( samModule,
              "Error resuming client timer %i", err );
          }
        }
      }
    }
  }
}
