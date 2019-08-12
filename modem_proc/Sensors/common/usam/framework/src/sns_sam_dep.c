/*============================================================================
  @file sns_sam_req.c

  @brief
  Handles processing of all incoming request messages to the SAM Framework.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_smgr_api_v01.h"
#include "sns_sam_ped_v01.h"
#include "sns_sam_common_v01.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_em.h"
#include "sns_debug_str.h"
#include "sns_sam.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_client.h"
#include "sns_sam_service.h"
#include "sns_sam_ind.h"
#include "sns_sam_pm.h"
#include "sns_sam_dep.h"
#include "sns_sam_prov.h"
#include "sns_sam_req.h"
#include "sns_sam_error.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
/* Maximum number of available Report IDs */
#define SNS_SAM_MAX_NUM_REPORT_IDS 256

/*============================================================================
  Static Data
  ===========================================================================*/

/* All sensor streams active in the framework */
STATIC sns_q_s sensorReqQ;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Get the minimum batch period from sensorReq
 *
 * @param[i] sensorReq Sensor Request to check
 *
 * @return Minimum Batch period found
 */
STATIC q16_t
sns_sam_get_min_batch_period( sns_sam_sensor_req const *sensorReq )
{
  uint8_t i;
  q16_t minBatch = FX_MAX_Q16;

  for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
  {
    if( NULL != sensorReq->algoInst[i] && sensorReq->batchPeriod[i] < minBatch )
    {
      minBatch = sensorReq->batchPeriod[i];
    }
  }
  return minBatch;
}

/**
 * Remove the reference to a particular algorithm instance from within a
 * sensor request object.
 *
 * @param[i] sensorReq Sensor Request to modify
 * @param[i] algoInst Algorithm instance to search for and remove
 */
STATIC void
sns_sam_remove_ai_ref( sns_sam_sensor_req* sensorReq,
    sns_sam_algo_inst const *algoInst )
{
  uint32_t i;

  for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
  {
    if( algoInst == sensorReq->algoInst[ i ] )
    {
      sensorReq->algoInst[ i ] = NULL;
      break;
    }
  }
}

/**
 * Find an existing sensor request that would be acceptable to this algorithm
 * instance.
 *
 * @param[i] sensorUID Sensor UID we are checking for
 * @param[i] algoInst Algorithm instance who will use the dependent sensor
 *
 * @return Existing sufficient sensor request; NULL if none found
 */
STATIC sns_sam_sensor_req*
sns_sam_find_existing_sensor_req( sns_sam_sensor_uid const *sensorUID,
  sns_sam_algo_inst const *algoInst )
{
  sns_q_link_s *qCurr;

  SNS_PRINTF_STRING_LOW_1( samModule,
    "sns_sam_find_existing_sensor_req %x", *sensorUID );

  for( qCurr = sns_q_check( &sensorReqQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorReqQ, qCurr ) )
  {
    sns_sam_sensor_req *sensorReq = (sns_sam_sensor_req*)qCurr;

    if( SNS_SMGR_SUID_PROX_1_V01 != *sensorUID &&
        sensorUID == sensorReq->sensor->sensorUID &&
        algoInst->algorithm->algoMsgAPI->sns_sam_algo_check_sensor(
          &algoInst->algoStateData,
          sensorReq->enableReq ) )
    {
      SNS_PRINTF_STRING_LOW_0( samModule, "Request Found" );
      return sensorReq;
    }
  }

  SNS_PRINTF_STRING_LOW_0( samModule, "No Request Found" );
  return NULL;
}

STATIC bool
sns_sam_sensor_req_in_use( sns_sam_sensor_req *sensorReq )
{
  uint32_t i;
  for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
  {
    if( NULL != sensorReq->algoInst[ i ] )
    {
      return true;
    }
  }

  return false;
}

/**
 * Checks if there are any remaining clients of the specified
 * sensor request, and cancels the request if necessary.  This may also involve
 * changing the sensor requests for other algorithm instances that were sharing
 * the dependent stream.
 *
 * @note This function will not dissociate a sensor request from any
 *       algorithm instance.
 *
 * @param[i] sensorRequest Sensor request to be removed
 */
STATIC void
sns_sam_remove_sensor_req( sns_sam_sensor_req *sensorReq )
{
  if( !sns_sam_sensor_req_in_use( sensorReq ) && NULL != sensorReq->qLink.q_ptr )
  {
    SNS_PRINTF_STRING_LOW_2( samModule, "Removing sensor %x request %i",
      *sensorReq->sensor->sensorUID, sensorReq->instanceID );
    sns_q_delete( &sensorReq->qLink );
    sns_sam_client_release( sensorReq );
  }
  else
  {
    if( NULL != sensorReq->provAPI->sns_sam_prov_set_batch_period )
    {
      SNS_PRINTF_STRING_LOW_3( samModule, "Updating batch period for sensor %x request %i to %i",
          *sensorReq->sensor->sensorUID, sensorReq->instanceID,
          sns_sam_get_min_batch_period( sensorReq ) );

      provCB.sensorReq = sensorReq;
      sensorReq->provAPI->sns_sam_prov_set_batch_period( &provCB );
    }
  }
}

/**
 * Inspect all existing dependent sensor streams, and check whether this new
 * sensor request may replace any of them.
 *
 * @param[i] sensorReq New sensor request required by an algorithm
 */
STATIC void
sns_sam_distribute_sensor_req( sns_sam_sensor_req *sensorReq )
{
  sns_q_link_s *qCurr;
  uint32_t i;
  bool removeReq = true;

  qCurr = sns_q_check( &sensorReqQ );
  while( NULL != qCurr )
  {
    sns_sam_sensor_req *tempReq = (sns_sam_sensor_req*)qCurr;
    qCurr = sns_q_next( &sensorReqQ, qCurr );

    if( tempReq == sensorReq ||
        SNS_SMGR_SUID_PROX_1_V01 == *sensorReq->sensor->sensorUID ||
        sensorReq->sensor->sensorUID != tempReq->sensor->sensorUID )
    {
      continue;
    }

    for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
    {
      sns_sam_algo_inst *algoInst = tempReq->algoInst[ i ];
      if( NULL !=  algoInst &&
          !algoInst->algorithm->algoMsgAPI->sns_sam_algo_check_sensor(
            &algoInst->algoStateData, sensorReq->enableReq ) )
      {
        removeReq = false;
        break;
      }
    }

    if( removeReq )
    {
      uint32_t j;
      for( i = 0, j = 0; i < SNS_SAM_MAX_SHARED_STREAM && j < SNS_SAM_MAX_SHARED_STREAM; )
      {
        if( NULL != sensorReq->algoInst[ j ] )
        {
          j++;
        }
        else if( NULL == tempReq->algoInst[ i ] )
        {
          i++;
        }
        else
        {
          sensorReq->algoInst[ j ] = tempReq->algoInst[ i ];
          tempReq->algoInst[ i ] = NULL;
        }
      }
      sns_sam_remove_sensor_req( tempReq );
    }
  }
}

/**
 * Shrinks the memory buffer size of the enable request message.  All messages
 * are pre-allocated with a larger-than-necessary buffer.
 *
 * @param enableReqMsg The message intended to enable some dependent sensor
 */
STATIC void
sns_sam_shrink_enable_req( sns_sam_enable_req *enableReqMsg )
{
  void *newBuffer = SNS_OS_MALLOC( SNS_SAM_DBG_MOD, enableReqMsg->msg.bufSize );
  SNS_ASSERT(NULL != newBuffer);

  SNS_OS_MEMSCPY( newBuffer, enableReqMsg->msg.bufSize, (void*)enableReqMsg->msg.buf,
    enableReqMsg->msg.bufSize );
  SNS_OS_ANY_FREE( (void*)enableReqMsg->msg.buf );
  enableReqMsg->msg.buf = (intptr_t)newBuffer;
}

/**
 * Allocate and initialize a new sensor request object.
 *
 * @param sensor Sensor object corresponding to the new stream
 * @param enableReqMsg The message intended to enable some dependent sensor
 *
 * @return New sensor request object, or NULL if the allocation failed
 */
STATIC sns_sam_sensor_req*
sns_sam_alloc_sensor_req( sns_sam_sensor *sensor,
  sns_sam_enable_req *enableReqMsg )
{
  sns_sam_sensor_req *sensorReq = NULL;
  uint32_t i;

  sensorReq = SNS_OS_ANY_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_sensor_req) );

  SNS_PRINTF_STRING_LOW_2( samModule,
    "Creating new sensor request %x for sensor %x",
    sensorReq, *sensor->sensorUID );

  if( NULL != sensorReq )
  {
    sensorReq->sensor = sensor;
    sensorReq->enableReq = enableReqMsg;
    sensorReq->clientHndl = NULL;
    sensorReq->deferredBatchReq = false;
    sensorReq->flushInProgress = false;
    sensorReq->instanceID = UINT16_MAX;
    sensorReq->sensorUID = sensor->sensorUID;
    sensorReq->provAPI = sensor->provAPI;

    for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
    {
      sensorReq->algoInst[ i ] = NULL;
      sensorReq->batchPeriod[ i ] = 0;
    }
    sns_q_link( sensorReq, &sensorReq->qLink );

    if( SAM_ENONE != sns_sam_client_init( sensorReq, 0, true ) )
    {
      SNS_OS_ANY_FREE( sensorReq );
      SNS_PRINTF_STRING_ERROR_0( samModule,
        "Error initializing client connection" );
      sensorReq = NULL;
    }
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_0( samModule,
      "Error allocating new sensor request: Out of Mem" );
  }

  return sensorReq;
}

/**
 * Create and start a new sensor request.
 *
 * @param enableReqMsg The message intended to enable some dependent sensor
 *
 * @return New sensor request object, or NULL upon failure
 */
STATIC sns_sam_sensor_req*
sns_sam_create_new_sensor_req( sns_sam_enable_req *enableReqMsg )
{
  sns_sam_sensor *sensor;
  int32_t errQMI;
  sns_sam_sensor_req *sensorReq = NULL;
  sns_sam_err err;

  sensor = sns_sam_lookup_sensor_from_suid( enableReqMsg->sensorUID );
  SNS_ASSERT(NULL != sensor);

  errQMI = qmi_idl_get_message_c_struct_len( sensor->serviceObj,
    QMI_IDL_REQUEST, enableReqMsg->msgID, &enableReqMsg->msg.bufSize );
  if( QMI_NO_ERR != errQMI )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule, "QMI error: %d", errQMI );
  }
  else
  {
    sns_sam_shrink_enable_req( enableReqMsg );

    sensorReq = sns_sam_alloc_sensor_req( sensor, enableReqMsg );
    if( NULL != sensorReq )
    {
      err = SAM_ENONE;
      if( NULL != sensorReq->sensor->provAPI->sns_sam_prov_handle_req )
      {
        provCB.sensorReq = sensorReq;
        err = sensorReq->sensor->provAPI->sns_sam_prov_handle_req( &provCB, enableReqMsg );
      }

      if( SAM_ENONE == err )
      {
        err = sns_sam_client_send( sensorReq, enableReqMsg );
      }

      if( SAM_ENONE != err )
      {
        SNS_PRINTF_STRING_ERROR_2( samModule,
          "Error sending enable request: %x, error %i",
          *enableReqMsg->sensorUID, err );

        sns_sam_client_release( sensorReq );
        sensorReq = NULL;
      }
      else
      {
        sns_q_put( &sensorReqQ, (sns_q_link_s*)sensorReq );
      }
    }
  }

  SNS_PRINTF_STRING_MEDIUM_2( samModule,
      "Created new sensor req %x (SUID: %x)",
      sensorReq, *enableReqMsg->sensorUID );

  return sensorReq;
}

/**
 * Mark a sensor request as in-use by a specific algorithm instance.
 *
 * @param[i] sensorReq Stream to update
 * @param[i] algoInst Algorithm instance to add
 *
 * @return true if marking completed successfully, false otherwise
 */
STATIC bool
sns_sam_mark_sensor_req( sns_sam_sensor_req *sensorReq,
    sns_sam_algo_inst *algoInst )
{
  uint32_t i;

  for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
  {
    if( NULL == sensorReq->algoInst[ i ] )
    {
      sensorReq->algoInst[ i ] = algoInst;
      return true;
    }
  }

  return false;
}

/**
 * Allocate and associate a new dependent sensor request with an algorithm
 * instance. Only one request per dependent sensor is allowed, so this
 * request may replace an existing one (will send a new request, and cancel
 * the existing one). If the algo already has a pending request and not other 
 * algos are have requested it, the stream will be modified, rather than replaced.
 *
 * @note The enableReqMsg pointer will be stored for later use.
 *
 * @param[i] algoInst Algorithm instance who will use the dependent sensor
 * @param[i] enableReqMsg The request message used to enable the sensor
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Error creating or sending enable request message
 *         SAM_EMAX Too many sensor requests for this algorithm instance
 */
STATIC sns_sam_err
sns_sam_create_sensor_req( sns_sam_algo_inst *algoInst,
    sns_sam_enable_req *enableReqMsg )
{
  sns_sam_err rv = SAM_ENONE;
  sns_sam_sensor_req *sensorReq, *reqToDelete;

  sns_sam_sensor *depSensor = sns_sam_lookup_sensor_from_suid( enableReqMsg->sensorUID );
  bool asyncSAM = NULL != depSensor &&
    depSensor->attributes[ SAM_ALGO_ATTR_REPORT_MODE ].supported &&
    depSensor->attributes[ SAM_ALGO_ATTR_REPORT_MODE ].attrValue & SNS_SAM_ASYNC_REPORT_V01;

  reqToDelete = sns_sam_find_sensor_req( algoInst, enableReqMsg->sensorUID );

  sensorReq = sns_sam_find_existing_sensor_req(
    enableReqMsg->sensorUID, algoInst );
  if( (NULL != sensorReq) && !asyncSAM && 
       sns_sam_mark_sensor_req( sensorReq, algoInst ) )
  {
    SNS_PRINTF_STRING_LOW_1( samModule, "Using existing request for %x",
      *sensorReq->sensor->sensorUID );

    SNS_OS_ANY_FREE( (void*)enableReqMsg->msg.buf );
    SNS_OS_ANY_FREE( (void*)enableReqMsg );

    if( NULL != reqToDelete )
    {
      reqToDelete->flushInProgress = false;
      sns_sam_handle_flush_response( reqToDelete );
      sns_sam_remove_ai_ref( reqToDelete, algoInst );
      sns_sam_remove_sensor_req( reqToDelete );
    }
    sns_sam_set_batch_period(algoInst, sensorReq->sensor->sensorUID, 0);
  }
  else
  { // see if the existing request can be updated. 
    uint32_t shareCnt = 0, i;
    if (NULL != reqToDelete)
    {
      for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
      {
        if( NULL != reqToDelete->algoInst[ i ] )
        {
          shareCnt++;
        }
      }
    }
    if((1 == shareCnt ))
    { // yes, the existing request can be updated.
      int32_t errQMI;
      sns_sam_sensor_req *sensorReq = NULL;
      sns_sam_err err;
      sns_sam_sensor *sensor = sns_sam_lookup_sensor_from_suid( enableReqMsg->sensorUID );
      SNS_ASSERT(NULL != sensor);
      errQMI = qmi_idl_get_message_c_struct_len( sensor->serviceObj,
            QMI_IDL_REQUEST, enableReqMsg->msgID, &enableReqMsg->msg.bufSize );
      sensorReq = reqToDelete;
      reqToDelete = NULL; // So we don't delete it later
      SNS_PRINTF_STRING_MEDIUM_1( samModule, "UPDATING request %x",*sensorReq->sensor->sensorUID );
      sns_sam_shrink_enable_req( enableReqMsg );
      rv = sns_sam_client_send( sensorReq, enableReqMsg );
      if( SAM_ENONE != rv )
      {
        SNS_PRINTF_STRING_ERROR_2( samModule,
          "Error sending enable request: %x, error %i",
          *enableReqMsg->sensorUID, rv );
      }
      // replace the request message
      SNS_OS_ANY_FREE( (void*)sensorReq->enableReq->msg.buf );
      SNS_OS_ANY_FREE( (void*)sensorReq->enableReq );
      sensorReq->enableReq = enableReqMsg;
    }
    else
    { // nope, need a new request
      sensorReq = sns_sam_create_new_sensor_req( enableReqMsg );

      if( NULL == sensorReq )
      {
        SNS_PRINTF_STRING_ERROR_1( samModule,
          "Failure generating new sensor request: %x",
          *enableReqMsg->sensorUID );
        rv =  SAM_EFAILED;
      }
      else
      {
        sensorReq->algoInst[ 0 ] = algoInst;
      }

      // We delay deletion, to optimize SMGR requests
      if( NULL != reqToDelete )
      {
        reqToDelete->flushInProgress = false;
        sns_sam_handle_flush_response( reqToDelete );
        sns_sam_remove_ai_ref( reqToDelete, algoInst );
        sns_sam_remove_sensor_req( reqToDelete );
      }

      if( NULL != sensorReq && !asyncSAM )
      {
        sns_sam_distribute_sensor_req( sensorReq );
      }
    }
  }

  return rv;
}

/**
 * Allocate an enable request message object and its associated message buffer
 * for a specific sensor UID.
 *
 * @param sensorUID The Sensor to which we intend to send a request message
 *
 * @return Sensor enable request buffer, or NULL upon failure
 */
STATIC sns_sam_enable_req*
sns_sam_alloc_enable_msg( sns_sam_sensor_uid const *sensorUID )
{
  sns_sam_enable_req *enableReqMsg = NULL;

  enableReqMsg = SNS_OS_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_enable_req) );
  if( NULL != enableReqMsg )
  {
    enableReqMsg->sensorUID = sensorUID;
    enableReqMsg->msg.bufSize = sns_sam_max_message_size( sensorUID );

    enableReqMsg->msg.buf = (intptr_t)SNS_OS_MALLOC(
      SNS_SAM_DBG_MOD,enableReqMsg->msg.bufSize );
    if( (intptr_t)NULL != enableReqMsg->msg.buf )
    {
      SNS_OS_MEMZERO( (void*)enableReqMsg->msg.buf, enableReqMsg->msg.bufSize );
    }
    else
    {
      SNS_OS_ANY_FREE( enableReqMsg );
      enableReqMsg = NULL;
    }
  }

  return enableReqMsg;
}

/**
 * The flush procedure completed, either due to receiving all dependent
 * response messages, or because of some error.  Now create all
 * appropriate periodic reports, batched indications, etc.
 *
 * @param[io] clientReq Client whose flush procedure has been completed
 */
STATIC void
sns_sam_flush_complete( sam_client_req *clientReq )
{
  while( SNS_SAM_REPORT_MODE_PERIODIC == clientReq->clientAttr.reportMode &&
         SNS_SAM_ASYNC_REPORT_V01 & clientReq->algorithm->sensor.attributes[ SAM_ALGO_ATTR_REPORT_MODE ].attrValue &&
         0 >= sns_sam_compare_ts(
           clientReq->nextReportTS,
           sns_em_get_timestamp() + sns_em_convert_sec_in_q16_to_localtick( clientReq->clientAttr.reportPeriod ) * .1) )
  {
    sns_sam_handle_report( clientReq );
    clientReq->nextReportTS +=
        sns_em_convert_sec_in_q16_to_localtick( clientReq->clientAttr.reportPeriod );
  }
  sns_sam_send_all_batch_ind( clientReq, SAM_BATCH_LAST_IND_V01 );
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_sam_sensor_req* sns_sam_find_sensor_req( sns_sam_algo_inst const *algoInst,
  sns_sam_sensor_uid const *sensorUID )
{
  sns_q_link_s *qCurr;
  uint32_t i;

  for( qCurr = sns_q_check( &sensorReqQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorReqQ, qCurr ) )
  {
    sns_sam_sensor_req *tempReq = (sns_sam_sensor_req*)qCurr;

    if( sensorUID == tempReq->sensor->sensorUID )
    {
      for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
      {
        if( algoInst == tempReq->algoInst[ i ] )
        {
          return tempReq;
        }
      }
    }
  }
  return NULL;
}

sns_sam_err sns_sam_handle_sensor_change( sns_sam_algo_inst *algoInst,
  sns_sam_sensor_uid const *sensorUID )
{
  sns_sam_err err, rv = SAM_EFAILED;
  sns_sam_enable_req *enableReqMsg;

  SNS_PRINTF_STRING_HIGH_2( samModule,
    "Algo %x Generating dependent req %x",
    *algoInst->algorithm->sensor.sensorUID, *sensorUID );

  enableReqMsg = sns_sam_alloc_enable_msg( sensorUID );
  SNS_ASSERT(NULL != (void*)enableReqMsg);

  err = algoInst->algorithm->algoMsgAPI->sns_sam_algo_gen_req(
      &algoInst->algoStateData, sensorUID, enableReqMsg );

  if( SAM_ESTATE == err )
  {
    sns_sam_sensor_req *sensorReq = sns_sam_find_sensor_req( algoInst, sensorUID );
    if( NULL != sensorReq )
    {
      sensorReq->flushInProgress = false;
      sns_sam_handle_flush_response( sensorReq );
      sns_sam_remove_ai_ref( sensorReq, algoInst );
      sns_sam_remove_sensor_req( sensorReq );
    }
    rv = SAM_ENONE;
  }
  else if( SAM_ENONE == err )
  {
    sns_sam_sensor *sensor;
    sensor = sns_sam_lookup_sensor_from_suid( sensorUID );
    if( NULL != sensor && sensor->isAvailable )
    {
      enableReqMsg->sensorUID = sensorUID;
      rv = sns_sam_create_sensor_req( algoInst, enableReqMsg );
    }
    else
    {
      rv = SAM_ETYPE;
      SNS_PRINTF_STRING_ERROR_1( samModule,
        "Unavailable sensor %x",
        *algoInst->algorithm->sensor.sensorUID );
    }
    err = rv;
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_2( samModule,
        "Algo error generating dep req %x, err %d",
        *algoInst->algorithm->sensor.sensorUID, err );
  }

  if( SAM_ENONE != err )
  {
    SNS_OS_ANY_FREE( (void*)enableReqMsg->msg.buf );
    SNS_OS_ANY_FREE( (void*)enableReqMsg );
  }

  if( SAM_ENONE != rv )
  {
    SNS_PRINTF_STRING_ERROR_3( samModule,
      "Error starting dependency %x for %x. Removing algo instance.%x",
      *sensorUID, *algoInst->algorithm->sensor.sensorUID, algoInst );

    sns_sam_client_error *errorData =
      SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(sns_sam_client_error));
    SNS_ASSERT( NULL != errorData );
    errorData->error = SAM_ENOT_AVAILABLE;
    errorData->errCBData = (void *)algoInst;
    errorData->errorSource = SNS_SAM_ERR_INTERNAL;

    sns_sam_report_error(errorData);
  }

  return rv;
}

sns_sam_err
sns_sam_start_dependencies( sns_sam_sensor_algo const *algo,
  sns_sam_algo_inst *algoInst )
{
  sns_sam_err rv = SAM_ENONE;
  uint32_t i;

  for( i = 0; i < SNS_SAM_MAX_DEPENDENT_SENSOR && SAM_ENONE == rv; i++ )
  {
    if( NULL != algo->dependencies[ i ] )
    {
      rv = sns_sam_handle_sensor_change(
        algoInst, algo->dependencies[ i ]->sensorUID );

      if( SAM_ENONE != rv )
      {
        rv = SAM_EFAILED;
        break;
      }
    }
  }

  if( SAM_EFAILED == rv )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Error starting dependencies for %x", *algo->sensor.sensorUID );
    sns_sam_remove_all_ai_ref( algoInst );
    sns_sam_remove_unused_sensor_reqs();
  }

  return rv;
}

sns_sam_err
sns_sam_set_batch_period( sns_sam_algo_inst *algoInst,
  sns_sam_sensor_uid const *sensorUID, uint32_t batchPeriod )
{
  sns_sam_sensor_req *sensorReq = NULL;
  uint8_t i;
  q16_t minBatch;

  SNS_PRINTF_STRING_LOW_2( samModule, "sns_sam_set_batch_period for %x to %i",
    *sensorUID, batchPeriod );

  sensorReq = sns_sam_find_sensor_req( algoInst, sensorUID );
  if( NULL == sensorReq )
  {
    SNS_PRINTF_STRING_ERROR_0( samModule, "Sensor request not found" );
    return SAM_ETYPE;
  }

  minBatch = sns_sam_get_min_batch_period( sensorReq );
  for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
  {
    if( sensorReq->algoInst[i] == algoInst )
    {
      sensorReq->batchPeriod[i] = batchPeriod;
      break;
    }
  }

  // If minBatch value has changed, send out the new batch_req
  if( minBatch != sns_sam_get_min_batch_period( sensorReq ) )
  {
    if( NULL != sensorReq->provAPI->sns_sam_prov_set_batch_period )
    {
      provCB.sensorReq = sensorReq;
      return SAM_ENONE == sensorReq->provAPI->sns_sam_prov_set_batch_period( &provCB )
        ? SAM_ENONE : SAM_EFAILED;
    }
  }
  return SAM_ENONE;
}

void
sns_sam_inform_report_change( sns_sam_algo_inst const *algoInst )
{
  q16_t minReportPeriod = FX_MAX_Q16;
  sns_q_link_s *qCurr = NULL;
  bool appsSuspend = sns_sam_pm_ap_suspend();

  if( NULL == algoInst->algorithm->algoMsgAPI->sns_sam_algo_handle_report_change )
  {
    return ;
  }

  for( qCurr = sns_q_check( &algoInst->clientRequests ); NULL != qCurr;
       qCurr = sns_q_next( &algoInst->clientRequests, qCurr ) )
  {
    sam_client_req const *clientReq = (sam_client_req*)qCurr;

      // We treat the special batch periods as batching indefinitely
      q16_t effBatchPeriod = clientReq->batchPeriod < 0
        ? FX_MAX_Q16 : clientReq->batchPeriod;
    bool nonWakeUp = appsSuspend &&
      SNS_PROC_APPS_V01 == clientReq->clientAttr.notifySuspend.proc_type &&
      FALSE == clientReq->clientAttr.notifySuspend.send_indications_during_suspend;
    if(nonWakeUp)
    {
      SNS_PRINTF_STRING_MEDIUM_2(
        samModule, "Switching algo inst %x Client %d to non-wake up batching mode.",
        *algoInst->algorithm->sensor.sensorUID, clientReq->extInstanceID);
      effBatchPeriod = FX_MAX_Q16;
    }
    if( 0 != effBatchPeriod )
    {
      if( effBatchPeriod < minReportPeriod )
      {
        minReportPeriod = effBatchPeriod;
      }
    }
    else if( SNS_SAM_REPORT_MODE_PERIODIC == clientReq->clientAttr.reportMode )
    {
      if( clientReq->clientAttr.reportPeriod < minReportPeriod )
      {
        minReportPeriod = clientReq->clientAttr.reportPeriod;
      }
    }
    else
    {
      minReportPeriod = 0;
    }

    SNS_PRINTF_STRING_LOW_3(
      samModule, "Client batchPeriod: %i, reportMode: %i, reportPeriod: %i",
      clientReq->batchPeriod, clientReq->clientAttr.reportMode,
      clientReq->clientAttr.reportPeriod);
  }

  SNS_PRINTF_STRING_LOW_2( samModule,
    "Inform algorithm %x of min report period: %i",
    *algoInst->algorithm->sensor.sensorUID, minReportPeriod);

  cbFuncs.algoHndl = (struct sns_sam_algo_handle*)
      sns_q_check( &algoInst->clientRequests );
  algoInst->algorithm->algoMsgAPI->sns_sam_algo_handle_report_change(
    &cbFuncs, &algoInst->algorithm->persistData,
    &algoInst->algoStateData, minReportPeriod, false );
}

bool
sns_sam_fip_check( sns_sam_algo_inst const *algoInst )
{
  sns_q_link_s *qCurr;

  for( qCurr = sns_q_check( &sensorReqQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorReqQ, qCurr ) )
  {
    uint32_t i;
    sns_sam_sensor_req *sensorReq = (sns_sam_sensor_req*)qCurr;

    for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
    {
      if( algoInst == sensorReq->algoInst[ i ] && sensorReq->flushInProgress )
      {
        return true;
      }
    }
  }
  return false;
}

sns_sam_err
sns_sam_send_flush( sns_sam_algo_inst const *algoInst )
{
  sns_q_link_s *qCurr;
  sns_sam_err rv = SAM_ENOT_AVAILABLE;

  for( qCurr = sns_q_check( &sensorReqQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorReqQ, qCurr ) )
  {
    uint32_t i;
    sns_sam_sensor_req *sensorReq = (sns_sam_sensor_req*)qCurr;
    SNS_ASSERT( NULL != sensorReq && NULL != sensorReq->sensor );

    for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
    {
      if( algoInst == sensorReq->algoInst[ i ] )
      {
        if( !sensorReq->flushInProgress )
        {
          sns_sam_err err = SAM_EFAILED;
          sensorReq->flushInProgress = true;

          if( NULL != sensorReq->sensor->provAPI->sns_sam_prov_get_data )
          {
            provCB.sensorReq = sensorReq;
            err = sensorReq->sensor->provAPI->sns_sam_prov_get_data( &provCB );
          }

          if( SAM_ENONE != err )
          {
            sensorReq->flushInProgress = false;
            SNS_PRINTF_STRING_MEDIUM_2( samModule,
                "Unable to initiate flush for %x (%i)",
                *sensorReq->sensorUID, err );
            continue;
          }
          else
          {
            // Call the flush initiation a success if at least one dependent
            // sensor batch request was successful
            rv = SAM_ENONE;
          }
        }
        else
        {
          // Also call the flush initiation a success if at least one dependent
          // sensor batch request was already in progress
          rv = SAM_ENONE;
        }
      }
    }
  }

  return rv;
}

void
sns_sam_handle_flush_response( sns_sam_sensor_req const *sensorReq )
{
  uint32_t i;

  for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
  {
    sns_q_link_s *qCurr = NULL;
    sns_sam_algo_inst const *algoInst = sensorReq->algoInst[ i ];

    if( NULL != algoInst )
    {
      for( qCurr = sns_q_check( &algoInst->clientRequests ); NULL != qCurr;
           qCurr = sns_q_next( &algoInst->clientRequests, qCurr ) )
      {
        sam_client_req *clientReq = (sam_client_req*)qCurr;

        if( 0 != sns_q_cnt( &clientReq->flushReqQ ) &&
            !sns_sam_fip_check( algoInst ) )
        {
          SNS_PRINTF_STRING_LOW_2( samModule,
            "Flush complete; Sending indications for %x (%i)",
            *algoInst->algorithm->sensor.sensorUID, clientReq->extInstanceID );

          sns_sam_flush_complete( clientReq );
          sns_sam_send_batch_resp( clientReq );
        }
      }
    }
  }
}

bool
sns_sam_initiate_flush( sam_client_req *clientReq, smr_req_handle reqHndl )
{
  sns_sam_err err = SAM_ENONE;
  sns_sam_flush_req *flushReq =
          (sns_sam_flush_req*)sns_q_last_check( &clientReq->flushReqQ );

  // Note: This may be updated when the flush completed, but there shouldn't
  // be any harm in doing it here.
  sns_sam_register_client_timer( clientReq );

  if( 0 == sns_q_cnt( &clientReq->flushReqQ ) )
  {
    sns_sam_send_all_batch_ind( clientReq, SAM_BATCH_FIRST_IND_V01 );

    SNS_PRINTF_STRING_MEDIUM_1( samModule, "Flushing %x",
        *clientReq->algoInstance->algorithm->sensor.sensorUID );

    err = sns_sam_send_flush( clientReq->algoInstance );
    if( SAM_ENONE != err )
    {
      sns_sam_flush_complete( clientReq );
    }
  }

  // We only want to add an entry to the flush request list if the list is
  // empty, or if we need to send a response later.  If an error occurred and
  // the queue is empty, the code above will handle it.  If an error occurred and
  // there are entries on the queue, we need to add an entry so that the response
  // message is sent once the flush process has completed.
  if( ( ( NULL != reqHndl || NULL == flushReq ) && SAM_ENONE == err ) ||
      ( NULL != flushReq && NULL != reqHndl && SAM_ENONE != err ) )
  {
    sns_sam_flush_req *flushReq =
      SNS_OS_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_flush_req) );
    SNS_ASSERT(NULL != flushReq);
    sns_q_link( flushReq, &flushReq->qLink );
    flushReq->batchPeriod = clientReq->batchPeriod;
    flushReq->reqHndl = reqHndl;
    sns_q_put( &clientReq->flushReqQ, &flushReq->qLink );

    return NULL != reqHndl ? true : false;
  }

  return false;
}

void
sns_sam_remove_unused_sensor_reqs()
{
  sns_q_link_s *qCurr = sns_q_check( &sensorReqQ );
  uint32_t i;

  while( NULL != qCurr )
  {
    sns_sam_sensor_req *tempReq = (sns_sam_sensor_req*)qCurr;
    qCurr = sns_q_next( &sensorReqQ, qCurr );

    for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
    {
      if( NULL != tempReq->algoInst[ i ] )
      {
        break;
      }
    }
    if( SNS_SAM_MAX_SHARED_STREAM == i )
    {
      sns_sam_remove_sensor_req( tempReq );
    }
  }
}

void
sns_sam_remove_all_ai_ref( sns_sam_algo_inst const *algoInst )
{
  sns_q_link_s *qCurr;
  uint32_t i;

  for( qCurr = sns_q_check( &sensorReqQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorReqQ, qCurr ) )
  {
    sns_sam_sensor_req *tempReq = (sns_sam_sensor_req*)qCurr;

    for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
    {
      if( algoInst == tempReq->algoInst[ i ] )
      {
        tempReq->algoInst[ i ] = NULL;
      }
    }
  }
}

void
sns_sam_dep_available( sns_sam_sensor const *sensor )
{
  sns_q_link_s *qCurr;
  uint32_t i;

  SNS_PRINTF_STRING_LOW_1( samModule, "New sensor available %x",
    *sensor->sensorUID );

  for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
       qCurr = sns_q_next( &sensorQ, qCurr ) )
  {
    sns_sam_sensor_algo *algo = (sns_sam_sensor_algo*)qCurr;
    if( !algo->sensor.isLocal )
    {
      continue;
    }
    for( i = 0; i < SNS_SAM_MAX_DEPENDENT_SENSOR; i++ )
    {
      if( NULL != algo->dependencies[ i ] &&
          *sensor->sensorUID == *algo->dependencies[ i ]->sensorUID )
      {
        SNS_PRINTF_STRING_MEDIUM_2( samModule, "Alerting algo %x of dependency %x",
          *algo->sensor.sensorUID, *sensor->sensorUID );
        algo->algoMsgAPI->sns_sam_algo_dep_sensors_met( algo,
            sns_sam_init_dep_met, &algo->persistData,
            sensor->sensorUID, &sensor->attributes );
      }
    }
  }
}



sns_sam_err
sns_sam_dep_init_fw( void )
{
  sns_q_init( &sensorReqQ );

  return SAM_ENONE;
}
